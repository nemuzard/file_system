#include "global.h"
#include <stdio.h>

disk_format* g_pstDisk_format;
int g_fd;

//文件id
int g_nFile_id = 1;
//内存块id
int g_nMemory_block_id = 1;
//当前路径
char g_szCurrentPath[4096] = { 0 };
//当前节点
MulTree* g_pstCurrentMulTree = NULL;
//当前操作的文件或目录
file_format* g_pstCurrentFile_format = NULL;
