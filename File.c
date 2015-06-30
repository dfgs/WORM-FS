#include "File.h"
#include "Logger.h"
#include "Utils.h"
#include "Retention.h"
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

    LogEnter("WORM_open");
    ConvertPath(convertedPath, path);
    
    writeAccess=(fi->fh & (O_WRONLY | O_RDWR | O_CREAT | O_TRUNC));
    
    if ( (writeAccess!=0) && (IsExpired(convertedPath)==false))
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(UPDATE,cFILE,path,NOTEXPIRED);
		return returnStatus;
	}
 
   
    WriteLog(DEBUG,"Try to open file, path %s, fh=%i",convertedPath,fi->fh);
    fi->fh  = open(convertedPath, fi->flags);
    if (fi->fh  < 0) 
    {
		returnStatus=WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot open file, path %s",convertedPath);
		AuditFailure(UPDATE,cFILE,path,NOK);
	}
	else
	{
		if (writeAccess!=0)
		{
			AuditSuccess(UPDATE,cFILE,path,OK);
			//SetRetentionAndExpiration(path,convertedPath);
		}
	}

 
    
    return returnStatus;
}

int WORM_release(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];
   	//int writeAccess;

	LogEnter("WORM_release");
    ConvertPath(convertedPath, path);

    WriteLog(DEBUG,"Try to close file_info");
    returnStatus = close(fi->fh);
    if (returnStatus < 0) 
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot close file_info");
	}
	/*else
	{
		writeAccess=(fi->fh & (O_WRONLY | O_RDWR | O_CREAT | O_TRUNC));
 		if (writeAccess!=0) SetRetentionAndExpiration(path,convertedPath);
	}*/

			    
    return returnStatus;
}


int WORM_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
    int returnStatus = 0;
    
    LogEnter("WORM_read");
    
    WriteLog(DEBUG,"Try to read file from file_info");
    returnStatus = pread(fi->fh, buf, size, offset);
    if (returnStatus < 0) 
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot read from file_info");
	}
	
    return returnStatus;
}


/** Change the size of a file */
int WORM_truncate(const char *path, off_t newsize)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];
    
    LogEnter("WORM_truncate");
    ConvertPath(convertedPath, path);
    
    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(UPDATE,cFILE,path,NOTEXPIRED);
		return returnStatus;
	}

    WriteLog(DEBUG,"Try to truncate file, path %s",convertedPath);
    returnStatus = truncate(convertedPath, newsize);
    if (returnStatus < 0) 
    {
		returnStatus=WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot truncate file, path %s",convertedPath);
		AuditFailure(UPDATE,cFILE,path,NOK);
	}
	else
	{
		AuditSuccess(UPDATE,cFILE,path,OK);
	}
		
    return returnStatus;
}
int WORM_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    char convertedPath[PATH_MAX];
    
    LogEnter("WORM_ftruncate");
    ConvertPath(convertedPath, path);
    
    if (IsExpired(convertedPath)==false)
    {
		returnStatus=WriteErrorNumber(WARN);
		WriteLog(WARN,"Media is not expired, path %s",convertedPath);
		AuditFailure(UPDATE,cFILE,path,NOTEXPIRED);
		return returnStatus;
	}
    
    WriteLog(DEBUG,"Try to truncate file from file_info, path %s",convertedPath);
    returnStatus = ftruncate(fi->fh, offset);
    if (returnStatus < 0) 
    {
		returnStatus=WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot truncate file from file_info, path %s",convertedPath);
		AuditFailure(UPDATE,cFILE,path,NOK);
	}
	else
	{
		AuditSuccess(UPDATE,cFILE,path,OK);
	}

    
    return returnStatus;
}

int WORM_write(const char *path, const char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
    int returnStatus = 0;
    
    LogEnter("WORM_write");
	
    WriteLog(DEBUG,"Try to write file with file_info");
    returnStatus = pwrite(fi->fh, buf, size, offset);
    if (returnStatus < 0) 
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot write with file_info");
	}
    
    return returnStatus;
}


/*int WORM_flush(const char *path, struct fuse_file_info *fi)
{
    int returnStatus = 0;
    
    WriteLog("flush path=%s, fi=0x%08x", path, fi);
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
    
    LogEnter("WORM_create");
    ConvertPath(convertedPath, path);
    
    WriteLog(DEBUG,"Try to create file, path %s",convertedPath);
    fi->fh = creat(convertedPath, mode);
    if (fi->fh < 0) 
    {
		returnStatus = WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot create file, path %s",convertedPath);
		AuditFailure(CREATE,cFILE,path,NOK);
	}
	else
	{
		AuditSuccess(CREATE,cFILE,path,OK);
		SetRealOwnerID(convertedPath);
		SetRetentionAndExpiration(path,convertedPath);
	}

	return returnStatus;
}




