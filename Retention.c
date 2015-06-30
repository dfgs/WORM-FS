#include "Retention.h"
#include "Logger.h"
#include "Utils.h"
#include <dirent.h>
#include <fuse.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <time.h>
#include <regex.h>
#include <inttypes.h>

unsigned short DefaultRetention=0;

int filtersCount=0;
struct Filter *filters;

unsigned short GetRetentionApplied(const char *Path, int* FilterIndex)
{
	int index;
	int match;
	//regmatch_t matches[100];

	LogEnter("GetRetentionApplied");

	for(index=0;index<filtersCount;index++)
	{
		WriteLog(DEBUG,"Try to match pattern %i",index);

		match=regexec(&filters[index].Regex, Path, 0, NULL, 0);
		if (match==0)
		{
			*FilterIndex=index;
			return filters[index].Value;
		}
	}

	// by default we apply retention
	return 65535;
}


unsigned short GetParentRetention(const char *Path)
{
	char pathCopy[PATH_MAX];
	char* parentPath;

	LogEnter("GetParentRetention");
	strcpy(pathCopy,Path);

    WriteLog(DEBUG,"Try to get parent directory, path %s",pathCopy);
	parentPath=dirname(pathCopy);
	if (parentPath==NULL)
	{
		WriteLog(ERROR,"Cannot find parent directory, will use default retention, path %s",Path);
		return DefaultRetention;
	}

	return GetRetention(parentPath);
 }

unsigned short GetRetention(const char *Path)
{
	unsigned short retention;
    int status;

	LogEnter("GetRetention");

    WriteLog(DEBUG,"Try to get extended attribute user.Retention, path %s",Path);
	status = lgetxattr(Path, "user.Retention", &retention, sizeof(retention));
	if (status<0)
	{
		WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot get extended attribute user.Retention, will use default retention, path %s",Path);
		return DefaultRetention;
	}


	//WriteLog(DEBUG,"Retention value is %i",retention);
	return retention;
}

void SetRetention(const char *path,const char *convertedPath,unsigned short Retention)
{
    int status;

	LogEnter("SetRetention");

    WriteLog(DEBUG,"Try to set extended attribute user.Retention, path %s",convertedPath);
	status = lsetxattr(convertedPath, "user.Retention", &Retention, sizeof(Retention),0);
	if (status<0)
	{
		WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot set extended attribute user.Retention, path %s",convertedPath);
		AuditFailure(UPDATE,RETENTION,path,"%i",Retention);
	}
	else
	{
		AuditSuccess(UPDATE,RETENTION,path,"%i",Retention);
	}


}

time_t CalcExpirationDate(unsigned short Retention)
{
	time_t now;
	time_t expiration;

	LogEnter("CalcExpirationDate");

	now=time(NULL);
	//WriteLog("Current date/time is %s",ctime(&now));

	expiration=now+24*3600*Retention;
	//WriteLog("Expiration date/time is %s",ctime(&expiration));

	return expiration;

}

time_t GetExpirationDate(const char *Path)
{
	time_t expiration;
    int status;
    int64_t value;
	//long long tmp;

	LogEnter("GetExpirationDate");

    WriteLog(DEBUG,"Try to get extended attribute user.ExpirationDate, path %s",Path);
	status = lgetxattr(Path, "user.ExpirationDate", &value, sizeof(value));
	if (status<0)
	{
		WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot get extended attribute user.ExpirationDate, path %s",Path);
		return 0;
	}
	expiration=value; //be sure to read 64 bits

	//WriteLog("Expiration date/time is %s",ctime(&expiration));
	return expiration;
}

void SetExpirationDate(const char *path,const char *convertedPath,time_t ExpirationDate)
{
    int status;
	//long long tmp;
    int64_t value;

	LogEnter("SetExpirationDate");


	value=ExpirationDate; // be sure to write 64 bits

    WriteLog(DEBUG,"Try to set extended attribute user.ExpirationDate, path %s",convertedPath);
	status = lsetxattr(convertedPath, "user.ExpirationDate", &value,sizeof(value),0);
	if (status<0)
	{
		WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot set extended attribute user.ExpirationDate, path %s",convertedPath);
		AuditFailure(UPDATE,EXPIRATION,path,"%" PRId64,value);
	}
	else
	{
		AuditSuccess(UPDATE,EXPIRATION,path,"%" PRId64, value);
	}
}
bool IsExpired(const char *Path)
{
	time_t expiration;
	time_t now;

	LogEnter("IsExpired");

	now=time(NULL);
	expiration=GetExpirationDate(Path);
	if (expiration>now)
	{
		errno=EROFS;
		//WriteLog(DEBUG,"Media is not expired, path %s",Path);
		return false;
	}
	else
	{
		//WriteLog(DEBUG,"Media is expired, path %s",Path);
		return true;
	}
}

void SetRetentionAndExpiration(const char *path,const char *convertedPath)
{
	unsigned short retention;
    time_t expirationDate;
	int filterIndex;

	LogEnter("SetRetentionAndExpiration");

	retention = GetRetentionApplied(convertedPath,&filterIndex);
	if (retention==65535)
	{
        WriteLog(DEBUG,"Path %s doesn't match any retention filter, trying to apply parent folder's retention",path);
        retention=GetParentRetention(convertedPath);
	}
	else
	{
		WriteLog(DEBUG,"Path %s matches retention filter %i, retention of %i day(s) will be applied",path,filterIndex,retention);
	}

	expirationDate=CalcExpirationDate(retention);
	SetRetention(path,convertedPath, retention);
	SetExpirationDate(path,convertedPath, expirationDate);
}
