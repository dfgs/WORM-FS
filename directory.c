#include <fuse.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "directory.h"
#include "logger.h"
#include "retention.h"
#include "utils.h"
#include "context.h"
#include "config.h"


int WORM_mkdir(const char *path, mode_t mode)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logger_enter(__func__,path);

    convertPath(convertedPath, path);

    logger_log(__func__, path,INFO, "Try to create directory");
    returnStatus = mkdir(convertedPath, mode);

	if (returnStatus != 0)
    {
        returnStatus = logger_errno(__func__, path,"Failed to create directory");
		logger_auditFailure(CREATE,DIRECTORY,path,NOK);
	}
	else
	{
		logger_auditSuccess(CREATE,DIRECTORY,path,OK);
        logger_log(__func__, path,INFO, "Setting retention and owner");
		setRealOwnerID(__func__, convertedPath);
		setRetention(__func__,path,convertedPath);
		if (config.autoLock!=0)
		{
            logger_log(__func__, path,INFO, "Auto lock is on, setting expiration date");
            setExpirationDate(__func__,path,convertedPath);
        }
	}

    return returnStatus;
}

int WORM_rmdir(const char *path)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logger_enter(__func__,path);

    convertPath(convertedPath, path);

    logger_log(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath)==0)
    {
		logger_log(__func__,path,WARN, "Media is not expired");
		logger_auditFailure(DELETE,DIRECTORY,path,NOTEXPIRED);
		return -EACCES;
	}

    logger_log(__func__, path,INFO, "Try to remove directory");
    returnStatus = rmdir(convertedPath);
    if (returnStatus < 0)
    {
        returnStatus = logger_errno(__func__, path,"Failed to remove directory");
		logger_auditFailure(DELETE,DIRECTORY,path,NOK);
	}
	else
	{
		logger_auditSuccess(DELETE,DIRECTORY,path,OK);
	}

    return returnStatus;
}


int WORM_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
	int returnStatus = 0;
    DIR *directory;
    struct dirent *directoryEntry;
    char convertedPath[PATH_MAX];


    logger_enter(__func__,path);
    convertPath(convertedPath, path);

    logger_log(__func__, path,INFO, "Try to read directory content");
    directory = opendir(convertedPath);
	if (directory==NULL)
	{
        returnStatus = logger_errno(__func__, path,"Failed to read directory content");
		return returnStatus;
	}

    logger_log(__func__, path,INFO, "Try to fill filler buffer");
 	while ((directoryEntry = readdir(directory)) != NULL)
    {
		if (filler(buf, directoryEntry->d_name, NULL, 0) != 0)
		{
            logger_log(__func__, path,ERROR, "Failed to fill filler buffer");
            returnStatus = logger_errno(__func__, path,"Failed to fill filler buffer");
			closedir(directory);
            return -ENOMEM;
		}
    }

    logger_log(__func__, path,INFO, "Try to close directory content");
 	returnStatus=closedir(directory);
    if (returnStatus != 0 ) returnStatus = logger_errno(__func__, path,"Failed to close directory content");

    return returnStatus;
}


int WORM_opendir(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logger_enter(__func__,path);
    convertPath(convertedPath, path);

    logger_log(__func__, path,INFO, "Try to open directory");
    fi->fh = (intptr_t)opendir(convertedPath);
    if (fi->fh == 0) returnStatus = logger_errno(__func__, path,"Failed to open directory");
	
    return returnStatus;
}

int WORM_releasedir(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;

    logger_enter(__func__,path);

    logger_log(__func__, path,INFO, "Try to close directory from file_info");
    returnStatus=closedir((DIR *) (uintptr_t) fi->fh);
    if (returnStatus < 0) returnStatus = logger_errno(__func__, path,"Failed to close directory from file info");

    return returnStatus;
}



