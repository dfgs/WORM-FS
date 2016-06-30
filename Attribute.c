#include "attribute.h"
#include "logger.h"
#include "utils.h"
#include "retention.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <errno.h>

int WORM_getattr(const char *path, struct stat *statbuf)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);

    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to get attributes");
    returnStatus = lstat(convertedPath, statbuf);
    if (returnStatus != 0) returnStatus = writeErrorNumber(__func__, path);
	else statbuf->st_mode=getReadOnlyMode(__func__, convertedPath,statbuf->st_mode);
 
    return returnStatus;
}
int WORM_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);

    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to get file attributes");
    returnStatus = fstat(fi->fh, statbuf);
    
    if (returnStatus != 0) returnStatus = writeErrorNumber(__func__, path);
    else statbuf->st_mode=getReadOnlyMode(__func__, convertedPath,statbuf->st_mode);

    return returnStatus;
}



int WORM_getxattr(const char *path, const char *name, char *value, size_t size)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to get extended attribute %s",name);
    returnStatus = lgetxattr(convertedPath, name, value, size);
    
    if (returnStatus < 0) returnStatus = writeErrorNumber(__func__, path);

    return returnStatus;
}


int WORM_listxattr(const char *path, char *list, size_t size)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to get extended attributes list");
	returnStatus = llistxattr(convertedPath, list, size);

    if (returnStatus < 0) returnStatus = writeErrorNumber(__func__, path);
	
    return returnStatus;
}
int WORM_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath)==0)
    {
		writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(UPDATE,ATTRIBUTE,path,NOTEXPIRED,"%s",name);
		return -EACCES;
	}

	writeLog(__func__,path,INFO,"Try to set extended attribute %s",name);
    returnStatus = lsetxattr(convertedPath, name, value, size, flags);
	if (returnStatus < 0)
	{
		returnStatus = writeErrorNumber(__func__, path);
		auditFailure(UPDATE,ATTRIBUTE,path,NOK,"%s",name);
	}
	else
	{
		auditSuccess(UPDATE,ATTRIBUTE,path,OK,"%s",name);
	}
    return returnStatus;
}

int WORM_removexattr(const char *path, const char *name)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath)==0)
    {
		writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(DELETE,ATTRIBUTE,path,NOTEXPIRED,"%s",name);
		return -EACCES;
	}

	writeLog(__func__,path,INFO, "Try to remove extended attribute %s",name);
    returnStatus = lremovexattr(convertedPath, name);
    if (returnStatus < 0)
    {
		returnStatus = writeErrorNumber(__func__, path);
		auditFailure(DELETE,ATTRIBUTE,path,NOK,"%s",name);
	}
	else
	{
		auditSuccess(DELETE,ATTRIBUTE,path,OK,"%s",name);
	}
    return returnStatus;
}


