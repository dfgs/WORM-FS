#include "shared.h"
#include "logger.h"
#include "utils.h"
#include "retention.h"
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <errno.h>




// both path and newpath are fs-relative
int WORM_rename(const char *path, const char *newpath)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];
    char newConvertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);
    convertPath(newConvertedPath, newpath);

    writeLog(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath)==0)
    {
        writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(UPDATE,LOCATION,path,NOTEXPIRED);
		return -EACCES;
	}

    writeLog(__func__,path,INFO,"Try to rename");
    returnStatus = rename(convertedPath, newConvertedPath);
    if (returnStatus != 0)
    {
		returnStatus = writeErrorNumber(__func__, path);
		auditFailure(UPDATE,LOCATION,path,NOK);
	}
	else
	{
		auditSuccess(UPDATE,LOCATION,path,newpath);
	}

    return returnStatus;
}


int WORM_chown(const char *path, uid_t uid, gid_t gid)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath)==0)
    {
        writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(UPDATE,OWNER,path,NOTEXPIRED);
		return -EACCES;
	}

    writeLog(__func__,path,INFO,"Try to change owner");
    returnStatus = chown(convertedPath, uid, gid);
    if (returnStatus != 0)
    {
		returnStatus = writeErrorNumber(__func__, path);
		auditFailure(UPDATE,OWNER,path,NOK);
	}
	else
	{
		auditSuccess(UPDATE,OWNER,path,OK);
	}

    return returnStatus;
}

/** Change the permission bits of a file */
int WORM_chmod(const char *path, mode_t mode)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath)==0)
    {
        writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(UPDATE,MODE,path,NOTEXPIRED,"%i",mode);
		return -EACCES;
	}

    writeLog(__func__,path,INFO,"Try to change access mode");
    returnStatus = chmod(convertedPath, mode);
    if (returnStatus != 0)
    {
		returnStatus = writeErrorNumber(__func__, path);
		auditFailure(UPDATE,MODE,path,NOK,"%i",mode);
	}
	else
	{
		auditSuccess(UPDATE,MODE,path,OK,"%i",mode);
	}
    return returnStatus;
}


int WORM_utime(const char *path, struct utimbuf *ubuf)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];
    char accessTime[20];
    time_t expirationDate;

    logEnter(__func__,path);
    convertPath(convertedPath, path);
  
    if (isExpired(__func__,convertedPath)==0)
    {
		writeLog(__func__,path,WARN,"Media is not expired");
		auditFailure(UPDATE,TIME,path,NOTEXPIRED);
		return -EACCES;
	}

    if ((isReadOnly(convertedPath)>0) && (autoLock==0))
    {
        convertTime(ubuf->actime,accessTime,20);
        writeLog(__func__,path,INFO,"File is read only setting expiration date %s on file",accessTime);
        setExpirationDateExplicit(__func__,path,convertedPath,ubuf->actime);
        return 0;
    }

    writeLog(__func__,path,INFO,"Try to change time value");
    returnStatus = utime(convertedPath, ubuf);
    if (returnStatus != 0)
    {
		returnStatus = writeErrorNumber(__func__, path);
		auditFailure(UPDATE,TIME,path,NOK);
	}
	else
	{
		auditSuccess(UPDATE,TIME,path,OK);
	}
    
    return returnStatus;
}

int WORM_statfs(const char *path, struct statvfs *statv)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Try to get file stats");
    returnStatus = statvfs(convertedPath, statv);
    if (returnStatus != 0) returnStatus = writeErrorNumber(__func__, path);


    return returnStatus;
}


int WORM_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
    int returnStatus = 0;

    logEnter(__func__,path);

    writeLog(__func__,path,INFO,"Try to fsync from file_info");
    if (datasync) returnStatus = fdatasync(fi->fh);
    else returnStatus = fsync(fi->fh);

    if (returnStatus != 0) returnStatus = writeErrorNumber(__func__, path);

    return returnStatus;
}


int WORM_access(const char *path, int mask)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Try to get access");
    returnStatus = access(convertedPath, mask);
    if (returnStatus != 0)
    {
		returnStatus = writeErrorNumber(__func__, path);
	}
	else returnStatus=getReadOnlyMode(__func__, convertedPath,returnStatus);

    return returnStatus;
}

