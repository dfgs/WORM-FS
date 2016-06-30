#ifndef _File_
#define _File_

#include <dirent.h>
#include <fuse.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>


int WORM_open(const char *path, struct fuse_file_info *fi);
int WORM_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi);
int WORM_truncate(const char *path, off_t newsize);
int WORM_write(const char *path, const char *buf, size_t size, off_t offset,struct fuse_file_info *fi);
int WORM_flush(const char *path, struct fuse_file_info *fi);
int WORM_release(const char *path, struct fuse_file_info *fi);
int WORM_create(const char *path, mode_t mode, struct fuse_file_info *fi);
int WORM_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi);

#endif
