#include "Shared.h"
#include "Logger.h"
#include "Utils.h"
#include "Retention.h"
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

    LogEnter("WORM_rename");
    ConvertPath(convertedPath, path);
    ConvertPath(newConvertedPath, newpath);

    WriteLog(DEBUG,"Try to get expiration, path %s",convertedPath);
    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(UPDATE,LOCATION,path,NOTEXPIRED);
		return returnStatus;
	}

    WriteLog(DEBUG,"Try to rename, path %s",convertedPath);
    returnStatus = rename(convertedPath, newConvertedPath);
    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot rename file, path %s",convertedPath);
		AuditFailure(UPDATE,LOCATION,path,NOK);
	}
	else
	{
		AuditSuccess(UPDATE,LOCATION,path,newpath);
	}

    return returnStatus;
}


int WORM_chown(const char *path, uid_t uid, gid_t gid)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_chown");
    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to get expiration, path %s",convertedPath);
    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(UPDATE,OWNER,path,NOTEXPIRED);
		return returnStatus;
	}

    WriteLog(DEBUG,"Try to change owner, path %s",convertedPath);
    returnStatus = chown(convertedPath, uid, gid);
    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot change owner, path %s",convertedPath);
		AuditFailure(UPDATE,OWNER,path,NOK);
	}
	else
	{
		AuditSuccess(UPDATE,OWNER,path,OK);
	}

    return returnStatus;
}

/** Change the permission bits of a file */
int WORM_chmod(const char *path, mode_t mode)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_chmod");
    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to get expiration, path %s",convertedPath);
    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(UPDATE,MODE,path,NOTEXPIRED,"%i",mode);
		return returnStatus;
	}

    WriteLog(DEBUG,"Try to change access mode, path %s, mode %i",convertedPath,mode);
    returnStatus = chmod(convertedPath, mode);
    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot change access mode, path %s",convertedPath);
		AuditFailure(UPDATE,MODE,path,NOK,"%i",mode);
	}
	else
	{
		AuditSuccess(UPDATE,MODE,path,OK,"%i",mode);
		if ( ((mode & 146)==0) && (AutoLock==0))
		{
            SetExpirationDate(path,convertedPath);
		}
	}
    return returnStatus;
}


int WORM_utime(const char *path, struct utimbuf *ubuf)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];


    LogEnter("WORM_utime");
    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to get expiration, path %s",convertedPath);
    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(UPDATE,TIME,path,NOTEXPIRED);
		return returnStatus;
	}

    WriteLog(DEBUG,"Try to change time values, path %s",convertedPath);
    returnStatus = utime(convertedPath, ubuf);
    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot change time values, path %s",convertedPath);
		AuditFailure(UPDATE,TIME,path,NOK);
	}
	else
	{
		AuditSuccess(UPDATE,TIME,path,OK);
	}
    return returnStatus;
}

int WORM_statfs(const char *path, struct statvfs *statv)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_statfs");
    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to get file stats, path %s",convertedPath);
    returnStatus = statvfs(convertedPath, statv);
    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot get file stats, path %s",convertedPath);
	}


    return returnStatus;
}


int WORM_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
    int returnStatus = 0;

    LogEnter("WORM_fsync");

    WriteLog(DEBUG,"Try to fsync from file_info");
    if (datasync) returnStatus = fdatasync(fi->fh);
    else returnStatus = fsync(fi->fh);

    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot fsync from file_info");
	}

    return returnStatus;
}


int WORM_access(const char *path, int mask)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_access");
    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to get access, path %s",convertedPath);
    returnStatus = access(convertedPath, mask);
    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot get access, path %s",convertedPath);
	}
	returnStatus=GetReadOnlyMode(convertedPath,returnStatus);

    return returnStatus;
}

