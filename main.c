#include <stdio.h>
#include "disk.h"
#include "multree.h"
#include "command.h"
#include <string.h>
int main(int argc, char* argv[])
{
	g_pstDisk_format = (disk_format*)malloc(sizeof(disk_format));
	if(g_pstDisk_format == NULL)
	{
		perror("malloc error:");
		return -1;
	}
	memset(g_pstDisk_format, 0, sizeof(disk_format));

	//初始化文件树
	g_pstDisk_format->pMulTree = init_MulTree();
	if(g_pstDisk_format->pMulTree == NULL)
	{
		printf("init_MulTree error!\n");
		return -1;
	}

	if(mount_fs((char*)"filesys") == -1)
	{
		printf("mount_fs error\n");
		return -1;
	}

	ExecuteCmd();

	if(umount_fs((char*)"filesys") == -1)
	{
		printf("umount_fs error!\n");
		return -1;
	}

	return 0;
}
