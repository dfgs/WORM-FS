#include "link.h"
#include "logger.h"
#include "utils.h"
#include "retention.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


int WORM_readlink(const char *path, char *link, size_t size)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__, path,INFO, "Try to read link");
    returnStatus = readlink(convertedPath, link, size - 1);
    if (returnStatus < 0)
    {
       returnStatus = writeErrorNumber(__func__, path);
	}
    else
    {
		link[returnStatus] = '\0';  // this part is required because readlink doesn't return ending char. Fuse requires it.
		returnStatus = 0;
    }
    

    return returnStatus;
}

/** Remove a file */
int WORM_unlink(const char *path)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(convertedPath, path);

    writeLog(__func__,path,INFO,"Checking expiration");
    if (isExpired(__func__,convertedPath) == 0)
    {
        writeLog(__func__,path,WARN, "Media is not expired");
		auditFailure(DELETE,cFILE,path,NOTEXPIRED);
		return -EACCES;
	}

    writeLog(__func__, path,INFO, "Try to unlink target");
    returnStatus = unlink(convertedPath);
    if (returnStatus != 0)
    {
        returnStatus = writeErrorNumber(__func__, path);
		auditFailure(DELETE,cFILE,path,NOK);
	}
	else
	{
		auditSuccess(DELETE,cFILE,path,OK);
	}

    return returnStatus;
}

// The parameters here are a little bit confusing, but do correspond
// to the symlink() system call.  The 'path' is where the link points,
// while the 'link' is the link itself.  So we need to leave the path
// unaltered, but insert the link into the mounted directory.
int WORM_symlink(const char *path, const char *link)
{
    int returnStatus = 0;
    char linkPath[PATH_MAX];

    logEnter(__func__,path);
    convertPath(linkPath, link);

    writeLog(__func__, path,INFO, "Try to create link");
    returnStatus = symlink(path, linkPath);
 	if (returnStatus != 0) returnStatus = writeErrorNumber(__func__, link);

    return returnStatus;
}

int WORM_link(const char *path, const char *newpath)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX], newConvertedPath[PATH_MAX];

    logEnter(__func__,path);
 	convertPath(convertedPath, path);
    convertPath(newConvertedPath, newpath);

    writeLog(__func__, path,INFO, "Try to create link");
    returnStatus = link(convertedPath, newConvertedPath);
    if (returnStatus != 0) returnStatus = writeErrorNumber(__func__, path);


    return returnStatus;
}
