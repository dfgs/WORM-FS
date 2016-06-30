#ifndef _Utils_
#define _Utils_

#include <stdlib.h>

char *repositoryPath;
char *mountPath;

typedef enum { false, true } bool;


void ConvertPath(char *DestPath, const char *OriginalPath);
void SetRealOwnerID(const char *Path);
int GetReadOnlyMode(const char *Path,int Mode);
int FileExists(const char *Path);
int DirectoryExists(const char *Path);
int CreateDirectory(const char *Path);


#endif
