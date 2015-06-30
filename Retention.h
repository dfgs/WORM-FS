#ifndef _Retention_
#define _Retention_


#include <regex.h>
#include <time.h>
#include "Utils.h"



struct Filter
{
	regex_t Regex;
	unsigned short Value;
};

extern unsigned short DefaultRetention;
extern int filtersCount;
extern struct Filter *filters;


unsigned short GetParentRetention(const char *Path);
unsigned short GetRetention(const char *Path);
void SetRetention(const char *path,const char *convertedPath,unsigned short Retention);
time_t CalcExpirationDate(unsigned short Retention);
time_t GetExpirationDate(const char* Path);
void SetExpirationDate(const char* path,const char *convertedPath,time_t RetentionDate);
bool IsExpired(const char *Path);
void SetRetentionAndExpiration(const char *path,const char *convertedPath);



#endif
