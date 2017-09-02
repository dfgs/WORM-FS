#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <fuse.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <dirent.h>
#include "utils.h"
#include "logger.h"
#include "retention.h"
#include "context.h"
#include "config.h"

void convertTime(time_t time,char *buffer,int bufferSize)
{
   // struct stat info;
    struct tm * timeinfo;

    timeinfo = localtime(&time);
    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", timeinfo);
}

void convertPath(char *destPath, const char *originalPath)
{
	//logEnter("convertPath");
    strcpy(destPath, config.repositoryPath);
    strncat(destPath, originalPath, PATH_MAX); // ridiculously long paths will break here

    //writeLog("Convert path",originalPath,DEBUG, "convertPath:  rootdir = %s, original path = %s, converted path = %s",repositoryPath, originalPath, destPath);
}

void setRealOwnerID(const char* funcName,const char *path)
{
	int returnStatus;

	struct fuse_context *context;
    logger_enter(__func__,path);
	
	context=fuse_get_context();
	logger_log(funcName,path,DEBUG,"Try to update fs entry owner from pid=%i to uid=%i/gid=%i",context->pid, context->uid,context->gid);
	returnStatus=chown(path,context->uid,context->gid);
	if (returnStatus<0) returnStatus = logger_errno(__func__, path,"Failed to update fs entry owner");

}

int getReadOnlyMode(const char* funcName,const char *path,int mode)
{
	if (S_ISDIR(mode)!=0) return mode;
	
	if (isExpired(funcName,path) == 0)
	{
		//writeLog(DEBUG,"Change mode from %i to %i (readonly)",Mode,Mode & 65389);
		return mode & 65389;
	}
	else return mode;
}
int isReadOnly(const char *path)
{
    int returnStatus = 0;
    struct stat statbuf;

    returnStatus = lstat(path, &statbuf);
    if (returnStatus != 0)
    {
        returnStatus = logger_errno(__func__, path,"Failed to get read only status");
        return 0;
    }
    return  (statbuf.st_mode & (S_IWUSR  | S_IWGRP | S_IWOTH))==0;

}
int fileExists(const char *path)
{
    FILE* file;

    file = fopen(path, "r");
	if (file != NULL)
	{
        fclose(file);
        return 1;
	}
	return 0;
}
int directoryExists(const char *path)
{
	DIR* dir = opendir(path);
	if (dir)
	{
		closedir(dir);
		return 1;
	}
	else return 0;

}
int createDirectory(const char* funcName,const char *path)
{
	int returnStatus;

	returnStatus = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (returnStatus != 0) returnStatus = logger_errno(funcName, path,"Failed to create directory");
	return returnStatus;
}
