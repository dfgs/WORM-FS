#include "logger.h"
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>

//int WORM_mknod(const char *path, mode_t mode, dev_t dev);
int WORM_rename(const char *path, const char *newpath);
int WORM_chown(const char *path, uid_t uid, gid_t gid);
int WORM_utime(const char *path, struct utimbuf *ubuf);
int WORM_statfs(const char *path, struct statvfs *statv);
int WORM_fsync(const char *path, int datasync, struct fuse_file_info *fi);
int WORM_access(const char *path, int mask);
int WORM_chmod(const char *path, mode_t mode);
