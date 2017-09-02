#define _GNU_SOURCE // mandatory in order to compile successfully

#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include <errno.h>
#include <utils.h>
#include "loop.h"
#include "logger.h"
#include "context.h"

static int getFreeLoop(char* buffer,int size)
{
	FILE *fp;	
	int length;

    logger_enter(__func__,NAFILE);

	memset(buffer,0,size);
	
	logger_log(__func__,NAFILE,DEBUG,"Getting free loop device");
	fp = popen("losetup -f", "r");
	if (fp == NULL) 
	{
		logger_errno(__func__,NAFILE,"Failed to get free loop device");
		return -1;
	}
	if (fgets(buffer, size-1, fp)==NULL)
	{
		logger_errno(__func__,NAFILE,"Failed to get free loop device");
		fclose(fp);
		return -1;
	}
	pclose(fp);
	
	length=strlen(buffer);
	if (length>0) buffer[length-1]='\0';

	logger_log(__func__,NAFILE,DEBUG,"Found free loop device: %s",buffer);
	return 0;
}

static int loop_bind(Loop* loop)
{
	char command[1024];

    logger_enter(__func__,NAFILE);
	
	strcpy(command,"losetup ");
	strcat(command,loop->name);
	strcat(command," ");
	strcat(command,loop->fileName);

	logger_log(__func__,NAFILE,DEBUG,"%s",command);

	return system(command);	
}
static int loop_unbind(Loop* loop)
{
	char command[1024];

    logger_enter(__func__,NAFILE);

	strcpy(command,"losetup -d ");
	strcat(command,loop->name);
	
	return system(command);
}

Loop* loop_create(const char* fileName,const char* mountPoint)
{
	Loop* loop;

    logger_enter(__func__,NAFILE);


	loop=malloc(sizeof(Loop));
	loop->fileName=fileName;
	loop->mountPoint=mountPoint;

	logger_log(__func__,NAFILE,DEBUG,"Creating loop device");
	if (getFreeLoop(loop->name,1024)!=0)
	{
		logger_log(__func__,NAFILE,ERROR,"Failed to create loop device");
		free(loop);
		return NULL;
	}
	if (loop_bind(loop)!=0)
	{
		logger_log(__func__,NAFILE,ERROR,"Failed to bind loop device");
		free(loop);
		return NULL;
	}
	
	return loop;
}


int loop_mount(Loop* loop)
{
    logger_enter(__func__,NAFILE);
	
	
	/*logger_log(__func__,NAFILE,DEBUG,"Unshare filesystem namespace");
	if (unshare(CLONE_NEWNS | CLONE_FS) != 0) 
	{
		logger_errno(__func__,NAFILE,"Failed to unshare filesystem namespace");
		return -1;
	}//*/

	logger_log(__func__,NAFILE,DEBUG,"Mounting loop device");
	/*if (mount("none", "/", NULL, MS_REC|MS_PRIVATE, NULL) != 0)
	{
		logger_ernno(__func__,NAFILE,ERROR,"Failed to mount loop device");
		return -1;
	}*/
	if (mount(loop->name, loop->mountPoint, "ext4",0,NULL)!= 0)
	{
		logger_errno(__func__,NAFILE,"Failed to mount loop device");
		return -1;
	}

	return 0;

}
int loop_umount(Loop* loop)
{
    logger_enter(__func__,NAFILE);
	return umount(loop->mountPoint);
}

void loop_free(Loop* loop)
{
    logger_enter(__func__,NAFILE);
	loop_unbind(loop);
	free(loop);
}





