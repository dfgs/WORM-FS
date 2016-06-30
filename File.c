#include "file.h"
#include "logger.h"
#include "utils.h"
#include "retention.h"
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



int WORM_open(const char *path, struct fuse_file_info *fi)
{
	int returnStatus = 0;
    char convertedPath[PATH_MAX];
	int writeAccess;
    int create;

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeAccess=(fi->flags & (O_WRONLY | O_RDWR | O_CREAT | O_TRUNC));
    //writeLog(__func__,path,DEBUG,"Open flags are %i",fi->flags);

    create=(fi->flags & O_CREAT );

    writeLog(__func__,path,INFO,"Checking expiration");
    if ( (writeAccess!=0) && (isExpired(__func__,convertedPath)==0))
    {
		writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(UPDATE,cFILE,path,NOTEXPIRED);
		return -EACCES;
	}


    writeLog(__func__, path,INFO, "Try to open file");
    fi->fh  = open(convertedPath, fi->flags);
    if (fi->fh  == 0)
    {
        returnStatus = writeErrorNumber(__func__, path);
		auditFailure(UPDATE,cFILE,path,NOK);
	}
	else
	{
		if (create!=0)
		{
            writeLog(__func__, path,INFO, "Settng retention");
			setRetention(__func__,path,convertedPath);
			auditSuccess(UPDATE,cFILE,path,OK);
		}
	}

    return returnStatus;
}

int WORM_release(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to close file");
    returnStatus = close(fi->fh);
    if (returnStatus != 0) returnStatus = writeErrorNumber(__func__, path);


    return returnStatus;
}


int WORM_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
    int returnStatus = 0;

    logEnter(__func__,path);

    writeLog(__func__, path,INFO, "Try to read file from file_info");
    returnStatus = pread(fi->fh, buf, size, offset);
    if (returnStatus < 0) returnStatus = writeErrorNumber(__func__, path);

    return returnStatus;
}


/** Change the size of a file */
int WORM_truncate(const char *path, off_t newsize)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath)==0)
    {
        writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(UPDATE,cFILE,path,NOTEXPIRED);
		return -EACCES;
	}

    writeLog(__func__, path,INFO, "Try to truncate file");
    returnStatus = truncate(convertedPath, newsize);
    if (returnStatus != 0)
    {
		returnStatus = writeErrorNumber(__func__, path);
		auditFailure(UPDATE,cFILE,path,NOK);
	}
	else
	{
		auditSuccess(UPDATE,cFILE,path,OK);
	}

    return returnStatus;
}

int WORM_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath)==0)
    {
        writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(UPDATE,cFILE,path,NOTEXPIRED);
		return -EACCES;
	}

    writeLog(__func__, path,INFO, "Try to truncate file from file_info");
    returnStatus = ftruncate(fi->fh, offset);
    if (returnStatus != 0)
    {
		returnStatus = writeErrorNumber(__func__, path);
		auditFailure(UPDATE,cFILE,path,NOK);
	}
	else
	{
		auditSuccess(UPDATE,cFILE,path,OK);
	}


    return returnStatus;
}

int WORM_write(const char *path, const char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
    int returnStatus = 0;

    logEnter(__func__,path);

    writeLog(__func__, path,INFO, "Try to write file from file_info");
    returnStatus = pwrite(fi->fh, buf, size, offset);
    if (returnStatus < 0) 
    {
        returnStatus = writeErrorNumber(__func__, path);
        auditFailure(UPDATE,cFILE,path,NOK);
    }

    return returnStatus;
}


/*int WORM_flush(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;

    writeLog("flush path=%s, fi=0x%08x", path, fi);
	returnStatus=fflush((FILE*)fi->fh);
    if (returnStatus < 0)
    {
		returnStatus = WriteError("fflush");
	}

    return returnStatus;
}*/



int WORM_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to create file");
    fi->fh = creat(convertedPath, mode);
    if (fi->fh == 0)
    {
        returnStatus = writeErrorNumber(__func__, path);
		auditFailure(CREATE,cFILE,path,NOK);
	}
	else
	{
		auditSuccess(CREATE,cFILE,path,OK);
        writeLog(__func__, path,INFO, "Settng retention and owner");
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




