#include "disk.h"
#include <time.h>
int make_fs(char* disk_name)
{

	char szBuf[1024] = { 0 };

	if(disk_name == NULL)
	{
		return -1;
	}

	g_fd = open(disk_name, O_CREAT | O_WRONLY | O_TRUNC, 0664);
	if(g_fd == -1)
	{
		perror("open error:");
		return -1;
	}

	//设置文件标志位
	strcpy(g_pstDisk_format->szSign, "vfos");	
	write(g_fd, g_pstDisk_format->szSign, sizeof(g_pstDisk_format->szSign));

	for(int i = 0; i < memory_block_cnt_max; i++)
	{
		g_pstDisk_format->mem_block[i].fid = -1;
		g_pstDisk_format->mem_block[i].id = i;
		g_pstDisk_format->mem_block[i].next_id = -1;
		
		//内存块id
		sprintf(szBuf, "%d", g_pstDisk_format->mem_block[i].id);
		write(g_fd, szBuf, 10);
		//下一个内存块id
		sprintf(szBuf, "%d", g_pstDisk_format->mem_block[i].next_id);
		write(g_fd, szBuf, 10);
		//文件id
		sprintf(szBuf, "%d", g_pstDisk_format->mem_block[i].fid);
		write(g_fd, szBuf, 10);
		//内存块数据
		write(g_fd, g_pstDisk_format->mem_block[i].szMemory, sizeof(g_pstDisk_format->mem_block[i].szMemory));

	}
	//可用内存块
	g_pstDisk_format->nAvailable_mem_block_num = memory_block_cnt_max;
	sprintf(szBuf, "%d", g_pstDisk_format->nAvailable_mem_block_num);
	write(g_fd, szBuf, 10);
	
	return 0;
}

int mount_fs(char* disk_name)
{
	if(disk_name == NULL)
	{
		return -1;
	}

	//判断文件是否存在
	if(access(disk_name, F_OK) == -1)
	{
		//不存在 创建
		if(make_fs((char*)"filesys") == -1)
		{
			printf("make_fs error\n");
			return -1;
		}
		return 0;
	}

	//打开文件
	g_fd = open(disk_name, O_RDWR);
	if(g_fd == -1)
	{
		printf("%s is not existed\n", disk_name);
		return -1;
	}

	//读取数据
	//先读取标志位
	if(read(g_fd, g_pstDisk_format->szSign, sizeof(g_pstDisk_format->szSign)) == -1)
	{
		printf("memory block error\n");
		return -1;
	}

	//判断标志位是否正确
	if(strcmp(g_pstDisk_format->szSign, "vfos") != 0)
	{
		//磁盘文件出错
		printf("disk file format error\n");
		return -1;
	}

	char szBuf[memory_block_size] = { 0 };	
	//读取内存块
	for(int i = 0; i < memory_block_cnt_max; i++)
	{
		//读取内存块id
		memset(szBuf, 0, memory_block_size);
		if(read(g_fd, szBuf, 10) == -1){ printf("memory block error\n"); return -1;}
		g_pstDisk_format->mem_block[i].id = atoi(szBuf);

		//读取下一个内存块id
		memset(szBuf, 0, 10);
		if(read(g_fd, szBuf, 10) == -1){ printf("memory block error\n"); return -1;}
		g_pstDisk_format->mem_block[i].next_id = atoi(szBuf);
		
		//读取文件id
		memset(szBuf, 0, 10);
		if(read(g_fd, szBuf, 10) == -1){ printf("memory block error\n"); return -1;}
		g_pstDisk_format->mem_block[i].fid = atoi(szBuf);
		
		//读取内存信息
		memset(szBuf, 0, 10);
		if(read(g_fd, g_pstDisk_format->mem_block[i].szMemory, sizeof(g_pstDisk_format->mem_block[i].szMemory)) == -1){ printf("memory block error\n"); return -1;}
	}


	//读取可用内存块
	memset(szBuf, 0, memory_block_size);
	if(read(g_fd, szBuf, 10) == -1){ printf("memory block error\n"); return -1;}
	g_pstDisk_format->nAvailable_mem_block_num = atoi(szBuf);
	
	//读取文件树
	read_MulTree(g_pstDisk_format->pMulTree);	

	//根目录名称始终为~
	//判断根目录名是否正确
	if(strcmp(g_pstDisk_format->pMulTree->stFile_Format.szName, "~") != 0)
	{
		//磁盘文件出错
		printf("disk file format error\n");
		return -1;
	}
	
	return 0;
}

int umount_fs(char* disk_name)
{
	lseek(g_fd, 0, SEEK_SET);
	//保存数据
	//保存标志位
	write(g_fd, g_pstDisk_format->szSign, sizeof(g_pstDisk_format->szSign));

	char szBuf[1024] = { 0 };	
	//写入内存块
	for(int i = 0; i < memory_block_cnt_max; i++)
	{

		//写入内存块id
		sprintf(szBuf, "%d", g_pstDisk_format->mem_block[i].id);
		write(g_fd, szBuf, 10);
		//写入下一个内存块id
		sprintf(szBuf, "%d", g_pstDisk_format->mem_block[i].next_id);
		write(g_fd, szBuf, 10);	
		//写入文件id
		sprintf(szBuf, "%d", g_pstDisk_format->mem_block[i].fid);
		write(g_fd, szBuf, 10);	

		//写入内存块信息
		write(g_fd, g_pstDisk_format->mem_block[i].szMemory, sizeof(g_pstDisk_format->mem_block[i].szMemory));
	}

	//写入可用内存块数
	sprintf(szBuf, "%d", g_pstDisk_format->nAvailable_mem_block_num);
	write(g_fd, szBuf, 10);

	//保存文件树
	save_MulTree(g_pstDisk_format->pMulTree);
	close(g_fd);
	return 0;
}


int fs_open(char* name)
{
	MulTree* pMulTree = find_MulTree(g_pstCurrentMulTree, name);
	if(pMulTree == NULL)
	{
		//文件不存在
		return -1;
	}

	//获取文件描述符最长个数
	int fd_cnt = sizeof(pMulTree->stFile_Format.aFd) / sizeof(int);
	//遍历文件描述符数组
	for(int i = 0; i < fd_cnt; i++)
	{
		//判断是否为0
		if(pMulTree->stFile_Format.aFd[i] == 0)
		{
			//置位
			pMulTree->stFile_Format.aFd[i] = i;
			//设置当前操作的文件
			g_pstCurrentFile_format = &pMulTree->stFile_Format;
			//返回文件描述符
			return pMulTree->stFile_Format.aFd[i];
		}
	}

	
	return -1;
}

int fs_close(int fildes)
{
	//获取文件描述符最长个数
	int fd_cnt = sizeof(g_pstCurrentMulTree->stFile_Format.aFd) / sizeof(int);
	//遍历文件描述符数组
	for(int i = 0; i < fd_cnt; i++)
	{
		//判断是否相等
		if(g_pstCurrentMulTree->stFile_Format.aFd[i] == fildes)
		{
			//置位
			g_pstCurrentMulTree->stFile_Format.aFd[i] = 0;
			//设置文件写入读取点
			g_pstCurrentFile_format->nPos = 0;
			//设置当前操作的文件
			g_pstCurrentFile_format = NULL;
			//关闭成功
			return 0;
		}
	}

	//文件描述符不存在
	return -1;
}

int fs_create(char* name)
{
	//判断是否存在
	if(find_MulTree(g_pstCurrentMulTree, name) != NULL)
	{
		//存在
		printf("cat: cannot create file: File exists\n");
		return -1;
	}

	file_format stFile_format;
	memset(&stFile_format, 0, sizeof(file_format));
	//fid
	stFile_format.fid = g_fd;
	//文件类型
	stFile_format.File_Type = s_file;
	//文件名称
	strcpy(stFile_format.szName, name);
	//文件大小
	stFile_format.nSize = 0;
	char* pszTime = GetTime();
	//文件 创建 修改时间	
	strcpy(stFile_format.szCreateTime, pszTime);
	stFile_format.szCreateTime[strlen(stFile_format.szCreateTime) - 1] = '\0';
	strcpy(stFile_format.szModifyTime, pszTime);
	stFile_format.szModifyTime[strlen(stFile_format.szModifyTime) - 1] = '\0';
	//文件内存块id
	stFile_format.nMem_block_id = -1;

	//添加
	push_MulTree(g_pstCurrentMulTree, stFile_format);

	return 0;
}

int fs_delete(char* name)
{
	//判断是否存在
	MulTree* pstMulTree = find_MulTree(g_pstCurrentMulTree, name);
	if(pstMulTree == NULL)
	{
		//不存在
		printf("rm: cannot delete file or directory: it is not existed\n");
		return -1;
	}

	//存在
	//判断是目录还是文件
	if(pstMulTree->stFile_Format.File_Type == s_file)
	{
		//文件
	}
	else if(pstMulTree->stFile_Format.File_Type == s_directory)
	{
		//目录
		//判断是否为空目录
		if(pstMulTree->nChildNum > 0)
		{
			//非空目录
			printf("rm: The directory cannot be deleted\n");
			return -1;
		}
	}
	else
	{
		//出错
		return -1;
	}


	memory_block* pstMemory_block = NULL;

	if(pstMulTree->stFile_Format.nMem_block_id != -1)
	{
		//先删除内存块
		pstMemory_block = &g_pstDisk_format->mem_block[pstMulTree->stFile_Format.nMem_block_id];

	//删除
	while(pstMemory_block->fid != -1)
	{
		//保存当前内存块下一个内存块id
		int nNext_id = pstMemory_block->next_id;
		//清空当前内存块内容
		memset(pstMemory_block->szMemory, 0, sizeof(pstMemory_block->szMemory));
		//设置当前内存块 文件id id 下一个内存块id
		pstMemory_block->fid = pstMemory_block->id = pstMemory_block->next_id = 0;
		//可用内存块加一
		g_pstDisk_format->nAvailable_mem_block_num++;
		//判断是否有下一个内存块
		if(nNext_id == -1)
		{
			//没有
			break;
		}
		//指向下一个内存块
		pstMemory_block = &g_pstDisk_format->mem_block[nNext_id];
	}
	}
	
	//再删除文件或目录
	remove_MulTree(g_pstCurrentMulTree, pstMulTree->stFile_Format);	

	return 0;
}

int fs_mkdir(char* name)
{
	//判断是否存在
	if(find_MulTree(g_pstCurrentMulTree, name) != NULL)
	{
		//存在
		printf("mkdir: cannot create directory: File exists\n");
		return -1;
	}
	//创建目录
	file_format stFile_format;
	memset(&stFile_format, 0, sizeof(file_format));
	//fid
	stFile_format.fid = g_nFile_id++;
	//文件类型
	stFile_format.File_Type = s_directory;
	//文件名称
	strcpy(stFile_format.szName, name);
	//文件大小
	stFile_format.nSize = 0;
	char* pszTime = GetTime();
	//文件 创建 修改时间	
	strcpy(stFile_format.szCreateTime, pszTime);
	stFile_format.szCreateTime[strlen(stFile_format.szCreateTime) - 1] = '\0';
	strcpy(stFile_format.szModifyTime, pszTime);
	stFile_format.szModifyTime[strlen(stFile_format.szModifyTime) - 1] = '\0';
	//文件内存块id
	stFile_format.nMem_block_id = -1;

	//添加
	push_MulTree(g_pstCurrentMulTree, stFile_format);


	return 0;
}

int fs_read(int fildes, void* buf, size_t nbyte)
{
	//判断文件描述符是否存在
	//获取文件描述符最长个数
	int fd_cnt = sizeof(g_pstCurrentFile_format->aFd) / sizeof(int);
	//遍历文件描述符数组
	int i = 0; 
	while(i < fd_cnt)
	{
		//判断是否为相等
		if(g_pstCurrentFile_format->aFd[i] == fildes)
		{
			//存在
			break;
		}
		i++;
	}

	if(i == 64)
	{
		//文件描述符不存在
		return -1;
	}


	memory_block stMemory_block;
	memset(&stMemory_block, 0, sizeof(stMemory_block));

	//获取内存块
	memcpy(&stMemory_block, &g_pstDisk_format->mem_block[g_pstCurrentFile_format->nMem_block_id], sizeof(memory_block));

	//文件读取点
	int nPos = g_pstCurrentFile_format->nPos;

	//读取
	for(int i = 0; i < nbyte; i++)
	{
		if(nPos >= memory_block_size)
		{
			//跳转到下一个内存块
			memcpy(&stMemory_block, &g_pstDisk_format->mem_block[stMemory_block.next_id], sizeof(memory_block));
			//读取点为0
			nPos = 0;
		}

		//读取
		char ch = g_pstDisk_format->mem_block[stMemory_block.id].szMemory[nPos++];
		if(ch == '\0')
		{
			//遇到\0读取完毕
			return i + 1;
		}

		//写到缓冲区
		((char*)buf)[i] = ch;
	}


	return nbyte;
}

int fs_write(int fildes, void* buf, size_t nbyte)
{
	//判断文件描述符是否存在
	//获取文件描述符最长个数
	int fd_cnt = sizeof(g_pstCurrentFile_format->aFd) / sizeof(int);
	//遍历文件描述符数组
	int i = 0; 
	while(i < fd_cnt)
	{
		//判断是否为相等
		if(g_pstCurrentFile_format->aFd[i] == fildes)
		{
			//存在
			break;
		}
		i++;
	}

	if(i == 64)
	{
		//文件描述符不存在
		return -1;
	}


	//判断是否有足够的内存块存储数据
	if(g_pstDisk_format->nAvailable_mem_block_num <= 0)
	{
		//空间不足
		return -1;
	}


	memory_block* pstMemory_block = NULL;

	//判断是否有内存块
	if(g_pstCurrentFile_format->nMem_block_id == -1)
	{
		//没有内存块
		//寻找一个空内存块
		for(int j = 0; j < memory_block_cnt_max; j++)
		{
			if(g_pstDisk_format->mem_block[j].fid == -1)
			{
				//找到新块
				//设置新块的文件id
				g_pstDisk_format->mem_block[j].fid = g_pstCurrentFile_format->fid;
				g_pstCurrentFile_format->nMem_block_id = j;
				//可用内存块减一
				g_pstDisk_format->nAvailable_mem_block_num--;
			
				pstMemory_block = &g_pstDisk_format->mem_block[i];
				break;
			}
		}

	}
	else
	{	
		//有下一个内存块
		//获取内存块
		pstMemory_block = &g_pstDisk_format->mem_block[g_pstCurrentFile_format->nMem_block_id];

	}

	//文件写入点
	int nPos = g_pstCurrentFile_format->nPos;

	//写入
	i = 0;
	while(i < nbyte)
	{
		//判断是否超出一个块大小
		if(nPos < memory_block_size)
		{
			//未超出
			g_pstDisk_format->mem_block[pstMemory_block->id].szMemory[nPos++] = ((char*)buf)[i++];
			continue;
		}

		//超出
		//判断是否有下一个块
		if(pstMemory_block->next_id == -1)
		{
			//没有下一个内存块
			//寻找一个空内存块
			for(int j = 0; j < memory_block_cnt_max; j++)
			{
				if(g_pstDisk_format->mem_block[j].fid == -1)
				{
					//找到新块
					//设置上一块的下一个块id为找到的新块
					g_pstDisk_format->mem_block[pstMemory_block->id].next_id = j;
					//设置新块的文件id
					g_pstDisk_format->mem_block[j].fid = g_pstCurrentFile_format->fid;
					//可用内存块减一
					g_pstDisk_format->nAvailable_mem_block_num--;
					break;
				}
			}

		}
		
		//有下一个内存块
		//跳转到下一个内存块
		pstMemory_block = &g_pstDisk_format->mem_block[pstMemory_block->next_id];	
		//重置写入位置
		nPos = 0;
		
	}

	//写入成功
	g_pstCurrentFile_format->nSize += nbyte;
	return 0;
}

int fs_get_filesize(int fildes)
{
	//判断文件描述符是否存在
	//获取文件描述符最长个数
	int fd_cnt = sizeof(g_pstCurrentFile_format->aFd) / sizeof(int);
	//遍历文件描述符数组
	int i = 0; 
	while(i < fd_cnt)
	{
		//判断是否为相等
		if(g_pstCurrentFile_format->aFd[i] == fildes)
		{
			//存在
			break;
		}
		i++;
	}

	if(i == 64)
	{
		//文件描述符不存在
		return -1;
	}

	return g_pstCurrentFile_format->nSize;

}

int fs_lseek(int fildes, off_t offset)
{
	//判断文件描述符是否存在
	//获取文件描述符最长个数
	int fd_cnt = sizeof(g_pstCurrentFile_format->aFd) / sizeof(int);
	//遍历文件描述符数组
	int i = 0; 
	while(i < fd_cnt)
	{
		//判断是否为相等
		if(g_pstCurrentFile_format->aFd[i] == fildes)
		{
			//存在
			break;
		}
		i++;
	}

	if(i == 64)
	{
		//文件描述符不存在
		return -1;
	}

	//判断是否小于0或者超出文件大小
	if(offset < 0 || offset > g_pstCurrentFile_format->nSize)
	{
		return -1;
	}

	//设置位置
	g_pstCurrentFile_format->nPos = offset; 

	return 0;
}

int fs_truncate(int fildes, off_t length)
{
	//判断文件描述符是否存在
	//获取文件描述符最长个数
	int fd_cnt = sizeof(g_pstCurrentFile_format->aFd) / sizeof(int);
	//遍历文件描述符数组
	int i = 0; 
	while(i < fd_cnt)
	{
		//判断是否为相等
		if(g_pstCurrentFile_format->aFd[i] == fildes)
		{
			//存在
			break;
		}
		i++;
	}

	if(i == 64)
	{
		//文件描述符不存在
		return -1;
	}

	//判断是否小于0或者超出文件大小
	if(length < 0)
	{
		return -1;
	}
	
	//拓展文件
	if(length > g_pstCurrentFile_format->nSize)
	{
		//计算要新增的大小
		int nAdd_Length = length - g_pstCurrentFile_format->nSize;
		char* pBuf = (char*)malloc(nAdd_Length);
		if(pBuf == NULL)
		{
			perror("malloc error:");
			exit(-1);
		}
		memset(pBuf, 0, nAdd_Length);
		//写入
		fs_lseek(fildes, g_pstCurrentFile_format->nSize);
		fs_write(fildes, pBuf, nAdd_Length);
		return -1;
	}

	//缩小文件
	//计算保留几个块
	int nReserve_block_num = 0;
	if(length % memory_block_size != 0)
	{
		nReserve_block_num = length / memory_block_size + 1;
	}
	else
	{
		nReserve_block_num = length / memory_block_size;
	}

	//计算总快数
	int nBlock_num = 0;
	if(g_pstCurrentFile_format->nSize % memory_block_size != 0)
	{
		nBlock_num = g_pstCurrentFile_format->nSize / memory_block_size + 1;
	}
	else
	{
		nBlock_num = g_pstCurrentFile_format->nSize / memory_block_size;
	}


	memory_block* pstMemory_block = &g_pstDisk_format->mem_block[g_pstCurrentFile_format->nMem_block_id];
	

	int nCurrentBlockLen = length;

	//跳转最后截断的内存块
	for(i = 1; i < nReserve_block_num; i++)
	{
		nCurrentBlockLen -= memory_block_size;
		pstMemory_block = &g_pstDisk_format->mem_block[pstMemory_block->next_id];	
	}
	
	if(nCurrentBlockLen != memory_block_size)
	{
		//清楚掉当前块多余的内容
		for(i = nCurrentBlockLen - 1; i < memory_block_size; i++)
		{
			pstMemory_block->szMemory[i] = '\0';
		}
	}
	
	//下一个id为-1
	int nNext_id = pstMemory_block->next_id;
	pstMemory_block->next_id = -1;
	pstMemory_block = &g_pstDisk_format->mem_block[nNext_id];	

	//清除掉后边的内存
	for(i = nReserve_block_num; i < nBlock_num; i++)
	{
		nNext_id = pstMemory_block->next_id;
		//文件id 内存块id 下一个内存块id
		pstMemory_block->fid = pstMemory_block->next_id = -1;
		//内存
		memset(pstMemory_block->szMemory, 0, sizeof(pstMemory_block->szMemory));
		//指向下一个内存块
		pstMemory_block = &g_pstDisk_format->mem_block[nNext_id];	
	}

	g_pstCurrentFile_format->nSize = length;


	return 0;
}

char* GetTime()
{
    time_t timep;
    time(&timep);

    return ctime(&timep);
}

