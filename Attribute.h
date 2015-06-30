#ifndef _Attribute_
#define _Attribute_

#include <fuse.h>


int WORM_getattr(const char *path, struct stat *statbuf);
int WORM_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi);

int WORM_getxattr(const char *path, const char *name, char *value, size_t size);
int WORM_listxattr(const char *path, char *list, size_t size);

int WORM_setxattr(const char *path, const char *name, const char *value, size_t size, int flags);
int WORM_removexattr(const char *path, const char *name);
#endif
