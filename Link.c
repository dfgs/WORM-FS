#include "Link.h"
#include "Logger.h"
#include "Utils.h"
#include "Retention.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


int WORM_readlink(const char *path, char *link, size_t size)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];
    
    LogEnter("WORM_readlink");
    ConvertPath(convertedPath, path);
    
    WriteLog(DEBUG,"Try to read link, path %s",convertedPath);
    returnStatus = readlink(convertedPath, link, size - 1);
    if (returnStatus < 0) 
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot read link, path %s",convertedPath);
	}
    else  
    {
		link[returnStatus] = '\0';
		returnStatus = 0;
    }
    
    return returnStatus;
}

/** Remove a file */
int WORM_unlink(const char *path)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];
    
    LogEnter("WORM_unlink");
    ConvertPath(convertedPath, path);
        
    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired");
		AuditFailure(DELETE,cFILE,path,NOTEXPIRED);
		return returnStatus;
	}
	
    WriteLog(DEBUG,"Try to unlink target, path %s",convertedPath);
    returnStatus = unlink(convertedPath);
    if (returnStatus < 0)
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot unlink target, path %s",convertedPath);
		AuditFailure(DELETE,cFILE,path,NOK);
	}
	else
	{
		AuditSuccess(DELETE,cFILE,path,OK);
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
    
    LogEnter("WORM_symlink");
    ConvertPath(linkPath, link);
    
    WriteLog(DEBUG,"Try to create sym link, path %s, link path %s",path,linkPath);
    returnStatus = symlink(path, linkPath);
    if (returnStatus < 0) 
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot create sym link, path %s, link path %s",path,linkPath);
	}
	
    return returnStatus;
}

int WORM_link(const char *path, const char *newpath)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX], newConvertedPath[PATH_MAX];
    
    LogEnter("WORM_link");
	ConvertPath(convertedPath, path);
    ConvertPath(newConvertedPath, newpath);
    
    WriteLog(DEBUG,"Try to create hard link, path %s",convertedPath);
    returnStatus = link(convertedPath, newConvertedPath);
    if (returnStatus < 0) 
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot create hard link, path %s",convertedPath);
	}
	
    
    return returnStatus;
}
