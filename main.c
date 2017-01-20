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
#include "logger.h"
#include "retention.h"
#include "directory.h"
#include "utils.h"
#include "attribute.h"
#include "link.h"
#include "shared.h"
#include "file.h"
#include "inifile.h"

static const char* _VERSION="0001";
static SectionDictionary *ini;

char **newArgs;


static void read_Conf()
{
    ini = open_ini("/etc/worm.conf");
    if (ini==NULL)
    {
		perror("Failed to open configuration file");
		exit(EXIT_FAILURE);
    }


    ID = ini_getInt(ini, "General","ID", 1);
    mountPath=ini_getString(ini,"General","MountPath","/mnt/WORM");
    repositoryPath=ini_getString(ini,"General","RepositoryPath","/tmp");
    defaultRetention = ini_getInt(ini, "General","DefaultRetention", 0);
    lockDelay = ini_getInt(ini, "General","LockDelay", 300);
    autoLock = ini_getInt(ini, "General","AutoLock", 0);
    maxLogFileLines = ini_getInt(ini, "Logs","MaxLogFileLines", 100);

    writeAuditFiles=ini_getInt(ini,"Audit","WriteAuditFiles",0);
    maxAuditFileLines = ini_getInt(ini, "Audit","MaxAuditFileLines", 100);
    auditFilePath=ini_getString(ini,"Audit","AuditFilePath","/var/log");

	sprintf(auditFileName,"%s/WORM_Audit.log",auditFilePath);
	strcpy(logFileName,"/var/log/WORM.log");


}
static void free_Conf()
{
    free_ini(ini);
}

static void loadFilters()
{
	char* pattern;
	int value;
	int result;
    Section* section;
    int index;
    KeyValuePair keyValuePair;

	logEnter(__func__,"ini file");

    section=ini_getSection(ini,"Retention");
    filters=malloc(sizeof(struct Filter)*section->count);


    filtersCount=0;
	writeLog(__func__, "ini file",INFO,"Parsing WORM filter rules");
    for(index=0;index<section->count;index++)
    {
        keyValuePair=section->items[index];
        pattern=keyValuePair.key;
        value=ini_getUnsignedShort(ini,"Retention", pattern, 0);

        // need to extract rule freom key

		writeLog(__func__, "ini file",INFO,"Compiling filter rule: %s %i",pattern,value);
		filters[filtersCount].value=value;
		result=regcomp(&filters[filtersCount].regex,pattern,REG_ICASE|REG_NOSUB|REG_NEWLINE|REG_EXTENDED);
		if (result!=0)
		{
			writeLog(__func__, "ini file",ERROR,"Invalid filter regex pattern %s",pattern);
		} else filtersCount++;
    }
 	writeLog(__func__, "ini file",INFO,"Filters count=%i",filtersCount);


}

static void *WORM_init(struct fuse_conn_info *conn)
{
	initLog();
	openLog();
	openAudit();

	logEnter(__func__,"ini file");

	writeLog(__func__,"ini file",INFO,"ID is %i",ID);
	writeLog(__func__,"ini file",INFO,"Mount path is %s",mountPath);
	writeLog(__func__,"ini file",INFO,"Repository path is %s",repositoryPath);
	writeLog(__func__,"ini file",INFO,"DefaultRetention is %i",defaultRetention);
	writeLog(__func__,"ini file",INFO,"MaxLogFileLines is %i",maxLogFileLines);
	writeLog(__func__,"ini file",INFO,"MaxAuditFileLines is %i",maxAuditFileLines);
	writeLog(__func__,"ini file",INFO,"WriteAuditFiles is %i",writeAuditFiles);
	writeLog(__func__,"ini file",INFO,"AuditFilePath is %s",auditFilePath);
	writeLog(__func__,"ini file",INFO,"LockDelay is %i",lockDelay);
	writeLog(__func__,"ini file",INFO,"AutoLock is %i",autoLock);
    loadFilters();


    return NULL;
}

static void WORM_destroy(void *userdata)
{
	int index=0;

    logEnter(__func__,"ini file");

	writeLog(__func__, "ini file",INFO,"Releasing WORM filters");
	for(index=0;index<filtersCount;index++)
	{
		regfree(&filters[index].regex);
	}//*/
	free(filters);

	closeAudit();
	closeLog();
	disposeLog();

    free(newArgs);
    free_Conf();

}

static struct fuse_operations WORM_oper =
{
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
	.getattr = WORM_getattr,
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
		printf("Current VERSION: %s\n",_VERSION);
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
