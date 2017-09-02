#ifndef _Filter_
#define _Filter_

#include "regex.h"

typedef struct
{
	regex_t regex;
	unsigned short value;
} Filter;

typedef struct 
{
	Filter* items;
	int count;
} Filters;


Filters* filters_load();
unsigned short filters_getRetention(Filters* filters,const char* path,int* filterIndex);
int filters_free(Filters* filters);

#endif 

