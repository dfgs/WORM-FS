#ifndef _Link_
#define _Link_

#include <stdio.h>

int WORM_readlink(const char *path, char *link, size_t size);
int WORM_unlink(const char *path);
int WORM_symlink(const char *path, const char *link);
int WORM_link(const char *path, const char *newpath);



#endif
