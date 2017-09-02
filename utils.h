#ifndef _Utils_
#define _Utils_

#include <stdlib.h>

void convertPath(char *DestPath, const char *originalPath);
void setRealOwnerID(const char* funcName,const char *path);
int getReadOnlyMode(const char* funcName,const char *path,int mode);
int fileExists(const char *path);
int directoryExists(const char *path);
int createDirectory(const char* funcName,const char *path);
void convertTime(time_t Time,char *Buffer,int bufferSize);
int isReadOnly(const char *path);
#endif
