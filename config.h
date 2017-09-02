#ifndef _Config_
#define _Config_

#include <ini.h>
#include "filter.h"

typedef struct 
{
	IniFile *iniFile;
	unsigned char ID;
	char *repositoryPath;
	int repositoryType;
	char *repositoryFile;
	char *mountPath;
	unsigned short defaultRetention;
	int auditMode;
	int lockDelay;
	int autoLock;
	Filters* filters;
} Config;

extern Config config;

int config_init();
int config_loadFilters();
int config_free();




#endif 
