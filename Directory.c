#include "Directory.h"
#include "Logger.h"
#include "Retention.h"
#include "Utils.h"
#include <fuse.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>




int WORM_mkdir(const char *path, mode_t mode)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_mkdir");

    ConvertPath(convertedPath, path);

	WriteLog(DEBUG,"Try to create directory, path %s",convertedPath);
    returnStatus = mkdir(convertedPath, mode);
	if (returnStatus != 0)
    {
		returnStatus=WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot create directory, path %s",convertedPath);
		AuditFailure(CREATE,DIRECTORY,path,NOK);
	}
	else
	{
		AuditSuccess(CREATE,DIRECTORY,path,OK);
		SetRealOwnerID(convertedPath);
		SetRetention(path,convertedPath);
		if (AutoLock!=0) SetExpirationDate(path,convertedPath);

	}



    return returnStatus;
}

int WORM_rmdir(const char *path)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_rmdir");

    ConvertPath(convertedPath, path);

    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(DELETE,DIRECTORY,path,NOTEXPIRED);
		return returnStatus;
	}

    WriteLog(DEBUG,"Try to remove directory, path %s",convertedPath);
    returnStatus = rmdir(convertedPath);
    if (returnStatus < 0)
    {
		returnStatus=WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot remove directory, path %s",convertedPath);
		AuditFailure(DELETE,DIRECTORY,path,NOK);
	}
	else
	{
		AuditSuccess(DELETE,DIRECTORY,path,OK);
	}


    return returnStatus;
}


int WORM_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
	int returnStatus = 0;
    DIR *directory;
    struct dirent *directoryEntry;
    char convertedPath[PATH_MAX];


    LogEnter("WORM_readdir");
    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to read directory content, path %s",convertedPath);
    directory = opendir(convertedPath);
	if (directory==NULL)
	{
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot read directory content, path %s",convertedPath);
		return returnStatus;
	}

	WriteLog(DEBUG,"Try to fill filler buffer",convertedPath);
 	while ((directoryEntry = readdir(directory)) != NULL)
    {
		if (filler(buf, directoryEntry->d_name, NULL, 0) != 0)
		{
			errno= ENOMEM;
			returnStatus=WriteErrorNumber(ERROR);
			WriteLog(ERROR,"Filler buffer is full, path %s",convertedPath);
			return returnStatus;
		}
    }

	WriteLog(DEBUG,"Try to close directory content");
	returnStatus=closedir(directory);
    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot close directory from file_info");
	}

    return returnStatus;
}


int WORM_opendir(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    LogEnter("WORM_opendir");
    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to open directory, path %s",convertedPath);
    fi->fh = (intptr_t)opendir(convertedPath);
    if (fi->fh == 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot open directory, path %s",convertedPath);
	}


    return returnStatus;
}

int WORM_releasedir(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;

    LogEnter("WORM_releasedir");

    WriteLog(DEBUG,"Try to close directory from file_info");
    returnStatus=closedir((DIR *) (uintptr_t) fi->fh);
    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot close directory from file_info");
	}

    return returnStatus;
}



