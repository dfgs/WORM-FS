#include "directory.h"
#include "logger.h"
#include "retention.h"
#include "utils.h"
#include <fuse.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>




int WORM_mkdir(const char *path, mode_t mode)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);

    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to create directory");
    returnStatus = mkdir(convertedPath, mode);

	if (returnStatus != 0)
    {
        returnStatus = writeErrorNumber(__func__, path);
		auditFailure(CREATE,DIRECTORY,path,NOK);
	}
	else
	{
		auditSuccess(CREATE,DIRECTORY,path,OK);
        writeLog(__func__, path,INFO, "Setting retention and owner");
		setRealOwnerID(__func__, convertedPath);
		setRetention(__func__,path,convertedPath);
		if (autoLock!=0)
		{
            writeLog(__func__, path,INFO, "Auto lock is on, setting expiration date");
            setExpirationDate(__func__,path,convertedPath);
        }
	}

    return returnStatus;
}

int WORM_rmdir(const char *path)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);

    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath)==0)
    {
		writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(DELETE,DIRECTORY,path,NOTEXPIRED);
		return -EACCES;
	}

    writeLog(__func__, path,INFO, "Try to remove directory");
    returnStatus = rmdir(convertedPath);
    if (returnStatus < 0)
    {
        returnStatus = writeErrorNumber(__func__, path);
		auditFailure(DELETE,DIRECTORY,path,NOK);
	}
	else
	{
		auditSuccess(DELETE,DIRECTORY,path,OK);
	}

    return returnStatus;
}


int WORM_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
	int returnStatus = 0;
    DIR *directory;
    struct dirent *directoryEntry;
    char convertedPath[PATH_MAX];


    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to read directory content");
    directory = opendir(convertedPath);
	if (directory==NULL)
	{
        returnStatus = writeErrorNumber(__func__, path);
		return returnStatus;
	}

    writeLog(__func__, path,INFO, "Try to fill filler buffer");
 	while ((directoryEntry = readdir(directory)) != NULL)
    {
		if (filler(buf, directoryEntry->d_name, NULL, 0) != 0)
		{
            writeLog(__func__, path,ERROR, "Failed to fill filler buffer");
            returnStatus = writeErrorNumber(__func__, path);
			closedir(directory);
            return -ENOMEM;
		}
    }

    writeLog(__func__, path,INFO, "Try to close directory content");
 	returnStatus=closedir(directory);
    if (returnStatus != 0 ) returnStatus = writeErrorNumber(__func__, path);

    return returnStatus;
}


int WORM_opendir(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to open directory");
    fi->fh = (intptr_t)opendir(convertedPath);
    if (fi->fh == 0) returnStatus = writeErrorNumber(__func__, path);
	
    return returnStatus;
}

int WORM_releasedir(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;

    logEnter(__func__,path);

    writeLog(__func__, path,INFO, "Try to close directory from file_info");
    returnStatus=closedir((DIR *) (uintptr_t) fi->fh);
    if (returnStatus < 0) returnStatus = writeErrorNumber(__func__, path);

    return returnStatus;
}



