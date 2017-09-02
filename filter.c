#include <ini.h>
#include <stdlib.h>
#include <regex.h>
#include "filter.h"
#include "logger.h"
#include "context.h"

Filters* filters_load(IniFile* iniFile)
{
	Filters* filters;
	
	char* pattern;
	int value;
	int result;
    Section* section;
    int index;
    keyValuePair keyValuePair;

	filters=malloc(sizeof(Filter));
	
	logger_enter(__func__,NAFILE);

    section=ini_getSection(iniFile,"Retention");
    filters->items=malloc(sizeof(Filter)*section->count);

    filters->count=0;
	logger_log(__func__, NAFILE,INFO,"Parsing WORM filter rules");
    for(index=0;index<section->count;index++)
    {
        keyValuePair=section->items[index];
        pattern=keyValuePair.key;
        value=ini_getUnsignedShort(iniFile,"Retention", pattern, 0);

        // need to extract rule from key

		logger_log(__func__, NAFILE,INFO,"Compiling filter rule: %s %i",pattern,value);
		filters->items[filters->count].value=value;
		result=regcomp(&filters->items[filters->count].regex,pattern,REG_ICASE|REG_NOSUB|REG_NEWLINE|REG_EXTENDED);
		if (result!=0)
		{
			logger_log(__func__, NAFILE,ERROR,"Invalid filter regex pattern %s",pattern);
		} else filters->count++;
    }
 	logger_log(__func__, NAFILE,INFO,"Filters count=%i",filters->count);
	
	return filters;
	
}

unsigned short filters_getRetention(Filters* filters,const char* path,int* filterIndex)
{
	int index;
	int match;
	logger_enter(__func__,NAFILE);

	logger_log(__func__, path,DEBUG, "Try to match rule patterns");
 	for(index=0;index<filters->count;index++)
	{
		match=regexec(&filters->items[index].regex, path, 0, NULL, 0);
		if (match==0)
		{
			*filterIndex=index;
			return filters->items[index].value;
		}
	}
	return 65535;
}

int filters_free(Filters* filters)
{
	int index;
	
	for(index=0;index<filters->count;index++)
	{
		regfree(&filters->items[index].regex);
	}//*/
		
	free(filters->items);
	free(filters);
}


