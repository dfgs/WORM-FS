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
#include "retention.h"
#include "logger.h"
#include "utils.h"
#include "context.h"
#include "filter.h"
#include "config.h"



unsigned short getParentRetention(const char* funcName,const char *path)
{
	char pathCopy[PATH_MAX];
	char* parentPath;

    logger_enter(__func__,path);
	strcpy(pathCopy,path);

    logger_log(funcName, path,DEBUG,"Try to get parent directory");
	parentPath=dirname(pathCopy);
	if (parentPath==NULL)
	{
		logger_log(funcName, path,ERROR,"Cannot find parent directory, will use default retention");
		return config.defaultRetention;
	}

	return getRetention(funcName,parentPath);
 }

unsigned short getRetention(const char* funcName,const char *path)
{
	unsigned short retention;
    int status;

    logger_enter(__func__,path);

    logger_log(funcName, path,DEBUG,"Try to get extended attribute user.Retention");
	status = lgetxattr(path, "user.Retention", &retention, sizeof(retention));
	if (status<0)
	{
        logger_errno(funcName, path,"Failed to get extended attribute user.Retention");
		return config.defaultRetention;
	}

	//writeLog(DEBUG,"Retention value is %i",retention);
	return retention;
}

void setRetention(const char* funcName,const char *path,const char *convertedPath)
{
    int status;
	unsigned short retention;
	int filterIndex;

    logger_enter(__func__,path);

	retention = filters_getRetention(config.filters,convertedPath,&filterIndex);
	if (retention==65535)
	{
        logger_log(funcName, path,DEBUG,"Path doesn't match any retention filter, trying to apply parent folder's retention");
        retention=getParentRetention(funcName,convertedPath);
	}
	else
	{
        logger_log(funcName, path,DEBUG,"Path matches retention filter %i, retention of %i day(s) will be applied",filterIndex,retention);
	}

    logger_log(funcName, path,DEBUG,"Try to set extended attribute user.Retention");
	status = lsetxattr(convertedPath, "user.Retention", &retention, sizeof(retention),0);
	if (status<0)
	{
        logger_errno(funcName, path,"Failed to set extended attribute user.Retention");
		logger_auditFailure(UPDATE,RETENTION,path,"%i",retention);
	}
	else
	{
		logger_auditSuccess(UPDATE,RETENTION,path,"%i",retention);
	}


}

time_t calcExpirationDate(const char* funcName,unsigned short retention)
{
	time_t now;
	time_t expiration;

    //logEnter(__func__,path);

	now=time(NULL);

	expiration=now+24*3600*retention;

	return expiration;

}

time_t getExpirationDate(const char* funcName,const char *path)
{
	time_t expiration;
    int status;
    int64_t value;
	//long long tmp;

    logger_enter(__func__,path);

    logger_log(funcName, path,DEBUG,"Try to get extended attribute user.ExpirationDate");
	status = lgetxattr(path, "user.ExpirationDate", &value, sizeof(value));
	if (status<0) return 0;
	/*{
        returnStatus = writeErrorNumber(funcName, path);
		return 0;
	}*/
	expiration=value; //be sure to read 64 bits

	//writeLog("Expiration date/time is %s",ctime(&expiration));
	return expiration;
}
time_t getLockDate(const char* funcName,const char *path)
{
	time_t lock;
    int status;
    int64_t value;
	//long long tmp;

    logger_enter(__func__,path);

    logger_log(funcName, path,DEBUG,"Try to get extended attribute user.LockDate");
	status = lgetxattr(path, "user.LockDate", &value, sizeof(value));
	if (status<0) return 0;
	/*{
        returnStatus = writeErrorNumber(funcName, path);
		return 0;
	}*/
	lock=value; //be sure to read 64 bits

	//writeLog("Expiration date/time is %s",ctime(&expiration));
	return lock;
}

void setExpirationDate(const char* funcName,const char *path,const char *convertedPath)
{
    int status;
	//long long tmp;
    unsigned short retention;
    time_t expirationDate;

    //logEnter(__func__,path);

    retention=getRetention(funcName,convertedPath);
	expirationDate=calcExpirationDate(funcName,retention);

    setExpirationDateExplicit(funcName,path,convertedPath,expirationDate);

}
void setExpirationDateExplicit(const char* funcName,const char *path,const char *convertedPath,time_t expirationDate)
{
    int status;
	//long long tmp;
    //unsigned short retention;
    char buffer[20];
    int64_t value;

    logger_enter(__func__,path);

	value=expirationDate; // be sure to write 64 bits

	setLockDate(funcName,path,convertedPath);

    convertTime(expirationDate,buffer,20);
    logger_log(funcName, path,DEBUG,"Try to set extended attribute user.ExpirationDate");
	status = lsetxattr(convertedPath, "user.ExpirationDate", &value,sizeof(value),0);
	if (status<0)
	{
		logger_errno(funcName, path,"Failed to set extended attribute user.ExpirationDate");
		logger_auditFailure(UPDATE,EXPIRATION,path,"%" PRId64,value);
	}
	else
	{
		logger_auditSuccess(UPDATE,EXPIRATION,path,"%" PRId64, value);
	}
}

void setLockDate(const char* funcName,const char *path,const char *convertedPath)
{
    int status;
	//long long tmp;
    int64_t value;
    time_t LockDate;

    logger_enter(__func__,path);

    LockDate=time(NULL);

	value=LockDate; // be sure to write 64 bits

    logger_log(funcName, path,DEBUG,"Try to set extended attribute user.LockDate");
	status = lsetxattr(convertedPath, "user.LockDate", &value,sizeof(value),0);
	if (status<0)
	{
        logger_errno(funcName, path,"Failed to set extended attribute user.LockDate");
        logger_auditFailure(UPDATE,LOCK,path,"%" PRId64,value);
	}
	else
	{
		logger_auditSuccess(UPDATE,LOCK,path,"%" PRId64, value);
	}
}

int isExpired(const char* funcName,const char *path)
{
	time_t expiration;
	time_t now;
	time_t lock;

    logger_enter(__func__,path);

	now=time(NULL);

    lock=getLockDate(funcName,path);
    if ((config.autoLock!=0) && (now<lock+config.lockDelay)) return 1;

	expiration=getExpirationDate(funcName,path);
	return (expiration<=now);
}
