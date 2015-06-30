#ifndef _Directory_
#define _Directory_

#include <fuse.h>
#include <dirent.h>

int WORM_mknod(const char *path, mode_t mode, dev_t dev);
int WORM_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi);
int WORM_mkdir(const char *path, mode_t mode);
int WORM_rmdir(const char *path);
int WORM_opendir(const char *path, struct fuse_file_info *fi);
int WORM_releasedir(const char *path, struct fuse_file_info *fi);
int WORM_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi);


#endif
