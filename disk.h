#ifndef DISK_H
#define DISK_H
#include "structure.h"
#include "global.h"
#include "multree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

int make_fs(char* disk_name);

int mount_fs(char* disk_name);

int umount_fs(char* disk_name);

int fs_open(char* name);

int fs_close(int fildes);

int fs_create(char* name);

int fs_delete(char* name);

int fs_mkdir(char* name);

int fs_read(int fildes, void* buf, size_t nbyte);

int fs_write(int fildes, void* buf, size_t nbyte);

int fs_get_filesize(int fildes);

int fs_lseek(int fildes, off_t offset);

int fs_truncate(int fildes, off_t length);

char* GetTime();

#endif
