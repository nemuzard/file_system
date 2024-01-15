#include "structure.h"

extern disk_format* g_pstDisk_format;
extern int g_fd;

//文件id
extern int g_nFile_id;
//内存块id
extern int g_nMemory_block_id;
//当前路径
extern char g_szCurrentPath[];
//当前节点
extern MulTree* g_pstCurrentMulTree;
//当前操作的文件或目录
extern file_format* g_pstCurrentFile_format;
