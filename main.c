#define FUSE_USE_VERSION 29

#ifndef HAVE_SETXATTR
#define HAVE_SETXATTR 1
#endif

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
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <sys/mount.h>
#include "Logger.h"
#include "Retention.h"
#include "Directory.h"
#include "Utils.h"
#include "Attribute.h"
#include "Link.h"
#include "Shared.h"
#include "File.h"
#include "inifile.h"

static const char* Version="0001";
static SectionDictionary *ini;

char **newArgs;


static void read_Conf()
{
    ini = open_ini("/etc/WORM.conf");
    if (ini==NULL)
    {
		perror("Failed to open configuration file");
		exit(EXIT_FAILURE);
    }


    ID = ini_getInt(ini, "General","ID", 1);
    mountPath=ini_getString(ini,"General","MountPath","/mnt/WORM");
    repositoryPath=ini_getString(ini,"General","RepositoryPath","/tmp");
    DefaultRetention = ini_getInt(ini, "General","DefaultRetention", 0);
    LockDelay = ini_getInt(ini, "General","LockDelay", 300);
    AutoLock = ini_getInt(ini, "General","AutoLock", 0);
    MaxLogFileLines = ini_getInt(ini, "Logs","MaxLogFileLines", 100);
    MaxAuditFileLines = ini_getInt(ini, "Logs","MaxAuditFileLines", 100);
    WriteAuditFiles=ini_getInt(ini,"Logs","WriteAuditFiles",0);

}
static void free_Conf()
{
    free_ini(ini);
}

static void LoadFilters()
{
	char* pattern;
	int value;
	int result;
    Section* section;
    int index;
    KeyValuePair keyValuePair;

    section=ini_getSection(ini,"Retention");
    filters=malloc(sizeof(struct Filter)*section->Count);


    filtersCount=0;
	WriteLog(INFO,"Parsing WORM filter rules");
    for(index=0;index<section->Count;index++)
    {
        keyValuePair=section->Items[index];
        pattern=keyValuePair.Key;
        value=ini_getUnsignedShort(ini,"Retention", pattern, 0);

        // need to extract rule freom key

		WriteLog(DEBUG,"Compiling filter rule: %s %i",pattern,value);
		filters[filtersCount].Value=value;
		result=regcomp(&filters[filtersCount].Regex,pattern,REG_ICASE|REG_NOSUB|REG_NEWLINE|REG_EXTENDED);
		if (result!=0)
		{
			WriteLog(ERROR,"Invalid filter regex pattern %s",pattern);
		} else filtersCount++;
    }
 	WriteLog(INFO,"Filters count=%i",filtersCount);


}

static void *WORM_init(struct fuse_conn_info *conn)
{


	InitLog();
	OpenLog();
	OpenAudit();

	LogEnter("WORM_init");

	WriteLog(INFO,"ID is %i",ID);
	WriteLog(INFO,"Mount path is %s",mountPath);
	WriteLog(INFO,"Repository path is %s",repositoryPath);
	WriteLog(INFO,"DefaultRetention is %i",DefaultRetention);
	WriteLog(INFO,"MaxLogFileLines is %i",MaxLogFileLines);
	WriteLog(INFO,"MaxAuditFileLines is %i",MaxAuditFileLines);
	WriteLog(INFO,"WriteAuditFiles is %i",WriteAuditFiles);
	WriteLog(INFO,"LockDelay is %i",LockDelay);
	WriteLog(INFO,"AutoLock is %i",AutoLock);
    LoadFilters();


    return NULL;
}

static void WORM_destroy(void *userdata)
{
	int index=0;

	WriteLog(INFO,"Releasing WORM filters");
	for(index=0;index<filtersCount;index++)
	{
		regfree(&filters[index].Regex);
	}//*/
	free(filters);

	CloseAudit();
	CloseLog();
	DisposeLog();

    free(newArgs);
    free_Conf();

}

static struct fuse_operations WORM_oper =
{
	.getattr = WORM_getattr,
	.readlink = WORM_readlink,
	.getdir = NULL,// no .getdir -- that's deprecated
	//.mknod =  WORM_mknod,
	.mkdir = WORM_mkdir,
	.unlink = WORM_unlink,
	.rmdir = WORM_rmdir,
	.symlink = WORM_symlink,
	.rename = WORM_rename,
	.link = WORM_link,
	.chmod = WORM_chmod,
	.chown = WORM_chown,
	.truncate = WORM_truncate,
	.utime = WORM_utime,
	.open = WORM_open,
	.read = WORM_read,
	.write = WORM_write,
	.statfs = WORM_statfs,
	//.flush = WORM_flush,
	.release = WORM_release,
	.fsync = WORM_fsync,
	.getxattr = WORM_getxattr,
	.listxattr = WORM_listxattr,

	.setxattr = WORM_setxattr,
	.removexattr = WORM_removexattr,

	.opendir = WORM_opendir,
	.readdir = WORM_readdir,
	.releasedir = WORM_releasedir,
	//.fsyncdir = WORM_fsyncdir,
	.init = WORM_init,
	.destroy = WORM_destroy,
	.access = WORM_access,
	.create = WORM_create,
	.ftruncate = WORM_ftruncate,
	.fgetattr = WORM_fgetattr
};




int main(int argc, char *argv[])
{
	int result;

 	if ((argc ==2) && (strcmp(argv[1],"-h")==0))
	{
		printf("Current version: %s\n",Version);
		printf("Usage: WORM options\n");
		return 0;
	}

	read_Conf();

    newArgs=malloc((argc+1)*sizeof(char*));
    memcpy(newArgs, argv, argc*sizeof(char*));
    newArgs[argc]=mountPath;



	result = fuse_main(argc+1, newArgs, &WORM_oper, NULL);


	return result;

}
