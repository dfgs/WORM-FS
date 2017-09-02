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
#include <ini.h>
#include "logger.h"
#include "retention.h"
#include "directory.h"
#include "utils.h"
#include "attribute.h"
#include "link.h"
#include "shared.h"
#include "file.h"
#include "loop.h"
#include "context.h"
#include "config.h"

Loop* loopDevice;


static void freeResources()
{
	logger_log(__func__, NAFILE,INFO,"Releasing resources");
	
	if (loopDevice!=NULL)
	{
		loop_umount(loopDevice);
		loop_free(loopDevice);
	}
	
	config_free();
	logger_free();
    
}

static void *WORM_init(struct fuse_conn_info *conn)
{
	logger_enter(__func__,NAFILE);

	logger_log(__func__,NAFILE,DEBUG,"ID is %i",config.ID);
	logger_log(__func__,NAFILE,DEBUG,"Mount path is %s",config.mountPath);
	logger_log(__func__,NAFILE,DEBUG,"Repository type is %i",config.repositoryType);
	logger_log(__func__,NAFILE,DEBUG,"Repository path is %s",config.repositoryPath);
	logger_log(__func__,NAFILE,DEBUG,"Repository file is %s",config.repositoryFile);
	logger_log(__func__,NAFILE,DEBUG,"DefaultRetention is %i",config.defaultRetention);
	logger_log(__func__,NAFILE,DEBUG,"Audit mode is %i",config.auditMode);
	logger_log(__func__,NAFILE,DEBUG,"LockDelay is %i",config.lockDelay);
	logger_log(__func__,NAFILE,DEBUG,"AutoLock is %i",config.autoLock);
     
}



static void WORM_destroy(void *userdata)
{
    logger_enter(__func__,NAFILE);
	freeResources();
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
	char* newargs[128];
	int result;

	
	if (argc==128)
	{
		fprintf(stderr,"Too many arguments\n");
		return -1;
	}

 	if ((argc ==2) && (strcmp(argv[1],"-h")==0))
	{
		printf("Current VERSION: %s\n",_VERSION);
		printf("Usage: WORM options\n");
		return 0;
	}

	if (config_init()!=0)
	{
		fprintf(stderr,"Cannot initialize configuration\n");
		freeResources();
		return -1;
	}
	if (logger_init()!=0)
	{
		fprintf(stderr,"Cannot initialize logger\n");
		freeResources();
		return -1;
	}
	if (config_loadFilters()!=0)
	{
		fprintf(stderr,"Cannot initialize filters\n");
		freeResources();
		return -1;
	}
	
	if (config.repositoryType==1)
	{
	
		loopDevice=loop_create(config.repositoryFile,config.repositoryPath);
		if (loopDevice==NULL)
		{
			fprintf(stderr,"Cannot initialize loop device\n");
			freeResources();
			return -1;
		}
		if (loop_mount(loopDevice)!=0)
		{
			fprintf(stderr,"Cannot mount loop device\n");
			freeResources();
			return -1;
		}
	}
		
	
    memcpy(newargs, argv, argc*sizeof(char*));
    newargs[argc]=config.mountPath;

	//printf("Calling fuse main...\n");
	result = fuse_main(argc+1, newargs, &WORM_oper, NULL);

	if (result!=0) fprintf(stderr,"fuse main returned an error %i\n",result);

	return result;
}
