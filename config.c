#include <stdio.h>
#include <string.h>
#include "config.h"
#include "ini.h"

Config config = {.ID=0,.auditMode=0,.lockDelay=300,.autoLock=300};


int config_init()
{
	
	config.iniFile = ini_open("/etc/worm.conf");
    if (config.iniFile==NULL)
    {
		fprintf(stderr,"Failed to open configuration file\n");
		return -1;
    }
	
	config.ID = ini_getInt(config.iniFile, "General","ID", 1);
    config.mountPath=ini_getString(config.iniFile,"General","MountPath","/mnt/WORM");
	config.repositoryType=ini_getInt(config.iniFile,"General","RepositoryType", 0);
    config.repositoryPath=ini_getString(config.iniFile,"General","RepositoryPath","/tmp");
    config.repositoryFile=ini_getString(config.iniFile,"General","RepositoryFile","NA");
    config.defaultRetention = ini_getInt(config.iniFile, "General","DefaultRetention", 0);
    config.lockDelay = ini_getInt(config.iniFile, "General","LockDelay", 300);
    config.autoLock = ini_getInt(config.iniFile, "General","AutoLock", 0);
    config.auditMode=ini_getInt(config.iniFile,"Audit","AuditMode",0);
 	
	if (config.repositoryType==1)
	{
		strcpy(config.repositoryPath,"/tmp/mnt.XXXXXX");
		config.repositoryPath=mkdtemp(config.repositoryPath);
		if (config.repositoryPath==NULL) return -1;
	}
	
	
	return 0;
}
int config_loadFilters()
{
	config.filters=filters_load(config.iniFile);
	if (config.filters==NULL) return -1;
	return 0;
}

int config_free()
{
	if ((config.repositoryType==1) && (config.repositoryPath!=NULL))
	{
		rmdir(config.repositoryPath);
	}
	
	if (config.iniFile!=NULL) ini_free(config.iniFile);
	if (config.filters!=NULL) filters_free(config.filters);
}