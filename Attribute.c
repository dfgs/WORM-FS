#include "Attribute.h"
#include "Logger.h"
#include "Utils.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/xattr.h>

int WORM_getattr(const char *path, struct stat *statbuf)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_getattr");

    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to get attributes, path %s",convertedPath);
    returnStatus = lstat(convertedPath, statbuf);
    if (returnStatus != 0)
    {
		 returnStatus=WriteErrorNumber(ERROR);
		 //WriteLog(ERROR,"Cannot get attributes, path %s",convertedPath);
	}

	statbuf->st_mode=GetReadOnlyMode(convertedPath,statbuf->st_mode);

    return returnStatus;
}
int WORM_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    //LogEnter("WORM_fgetattr");
    ConvertPath(convertedPath, path);

    //WriteLog(DEBUG,"Try to get attributes from file_info, path %s",convertedPath);
    returnStatus = fstat(fi->fh, statbuf);
	if (returnStatus != 0)
    {
		returnStatus=WriteErrorNumber(ERROR);
		//WriteLog(ERROR,"Cannot get attributes from file_info, path %s",convertedPath);
	}

	statbuf->st_mode=GetReadOnlyMode(convertedPath,statbuf->st_mode);

    return returnStatus;
}
int WORM_getxattr(const char *path, const char *name, char *value, size_t size)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_getxattr");
    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to get extended attribute %s from file, path %s",name,convertedPath);
    returnStatus = lgetxattr(convertedPath, name, value, size);
    if (returnStatus < 0)
    {
		returnStatus=WriteErrorNumber(ERROR);
		//WriteLog(ERROR,"Cannot get extended attribute %s from file, path %s",name,convertedPath);
	}
    return returnStatus;
}


int WORM_listxattr(const char *path, char *list, size_t size)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_listxattr");

    ConvertPath(convertedPath, path);
	WriteLog(DEBUG,"Try to get list extended attributes from file, path %s",convertedPath);

    if (size>0)
    {
		returnStatus = llistxattr(convertedPath, list, size);
		if (returnStatus < 0)
		{
			returnStatus=WriteErrorNumber(ERROR);
			//WriteLog(ERROR,"Cannot list extended attributes from file, path %s",convertedPath);
		}
	}
	else
	{
		WriteLog(WARN,"Extended list size is 0");
		returnStatus=0;
	}


    return returnStatus;
}
int WORM_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_setxattr");
    ConvertPath(convertedPath, path);
    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(UPDATE,ATTRIBUTE,path,NOTEXPIRED,"%s",name);
		return returnStatus;
	}

	WriteLog(DEBUG,"Try to set extended attributes %s to file, path %s",name,convertedPath);
    returnStatus = lsetxattr(convertedPath, name, value, size, flags);
	if (returnStatus < 0)
	{
		returnStatus=WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot set extended attribute %s to file, path %s",name,convertedPath);
		AuditFailure(UPDATE,ATTRIBUTE,path,NOK,"%s",name);
	}
	else
	{
		AuditSuccess(UPDATE,ATTRIBUTE,path,OK,"%s",name);
	}
    return returnStatus;
}
int WORM_removexattr(const char *path, const char *name)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_removexattr");
    ConvertPath(convertedPath, path);

    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(DELETE,ATTRIBUTE,path,NOTEXPIRED,"%s",name);
		return returnStatus;
	}

	WriteLog(DEBUG,"Try to remove extended attribute %s from file, path %s",name,convertedPath);
    returnStatus = lremovexattr(convertedPath, name);
    if (returnStatus < 0)
    {
		returnStatus=WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot remove extended attribute %s from file, path %s",name,convertedPath);
		AuditFailure(DELETE,ATTRIBUTE,path,NOK,"%s",name);
	}
	else
	{
		AuditSuccess(DELETE,ATTRIBUTE,path,OK,"%s",name);
	}
    return returnStatus;
}


