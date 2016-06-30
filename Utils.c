#include "Utils.h"
#include "Logger.h"
#include "Retention.h"
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



void ConvertPath(char *DestPath, const char *OriginalPath)
{
	//LogEnter("ConvertPath");
    strcpy(DestPath, repositoryPath);
    strncat(DestPath, OriginalPath, PATH_MAX); // ridiculously long paths will break here

    //WriteLog("ConvertPath:  rootdir = %s, original path = %s, converted path = %s",rootDir, OriginalPath, DestPath);
}

void SetRealOwnerID(const char *Path)
{
	int returnStatus;

	struct fuse_context *context;
	LogEnter("SetRealOwnerID");

	context=fuse_get_context();
	WriteLog(DEBUG,"Try to update fs entry owner from pid=%i to uid=%i/gid=%i, path %s",context->pid, context->uid,context->gid, Path);
	returnStatus=chown(Path,context->uid,context->gid);
	if (returnStatus<0)
	{
		returnStatus=WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot update fs entry owner with uid/gid, path %s",Path);
	}
}

int GetReadOnlyMode(const char *Path,int Mode)
{
	if (S_ISDIR(Mode)!=0) return Mode;

	if (IsExpired(Path)==false)
	{
		//WriteLog(DEBUG,"Change mode from %i to %i (readonly)",Mode,Mode & 65389);
		return Mode & 65389;
	}
	else return Mode;
}
int FileExists(const char *Path)
{
    FILE* file;

    file = fopen(Path, "r");
	if (file != NULL)
	{
        fclose(file);
        return 1;
	}
	return 0;
}
int DirectoryExists(const char *Path)
{
	DIR* dir = opendir(Path);
	if (dir)
	{
		closedir(dir);
		return 1;
	}
	else return 0;

}
int CreateDirectory(const char *Path)
{
	int returnStatus;

	returnStatus = mkdir(Path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (returnStatus != 0)
    {
		returnStatus=WriteErrorNumber(ERROR);
		WriteLog(ERROR,"Cannot create directory, path %s",Path);
	}
	return returnStatus;
}
