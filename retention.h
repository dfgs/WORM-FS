#ifndef _Retention_
#define _Retention_


#include <regex.h>
#include <time.h>
#include "utils.h"



struct Filter
{
	regex_t regex;
	unsigned short value;
};

extern unsigned short defaultRetention;
extern int filtersCount;
extern struct Filter *filters;


unsigned short getParentRetention(const char* funcName,const char *path);
unsigned short getRetention(const char* funcName,const char *path);

time_t calcExpirationDate(const char* funcName,unsigned short retention);
time_t getExpirationDate(const char* funcName,const char* path);
time_t getLockDate(const char* funcName,const char* path);

void setRetention(const char* funcName,const char *path,const char *convertedPath);
void setExpirationDate(const char* funcName,const char* path,const char *convertedPath);
void setExpirationDateExplicit(const char* funcName,const char* path,const char *convertedPath,time_t expirationDate);
void setLockDate(const char* funcName,const char* path,const char *convertedPath);

int isExpired(const char* funcName,const char *path);
//void setRetentionAndExpiration(const char *path,const char *convertedPath);



#endif
