#include "multree.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

//文件树初始化
MulTree* init_MulTree()
{
	MulTree* pMulTree = (MulTree*)malloc(sizeof(MulTree));
	if(pMulTree == NULL)
	{
		return NULL;
	}
	memset(pMulTree, 0, sizeof(MulTree));

	pMulTree->stFile_Format.fid = g_nFile_id++;
	pMulTree->stFile_Format.File_Type = s_directory;

	strcpy(pMulTree->stFile_Format.szName, "~");

	//设置当前节点
	g_pstCurrentMulTree = pMulTree;
	memset(g_szCurrentPath, 0, strlen(g_szCurrentPath));
	strcpy(g_szCurrentPath, pMulTree->stFile_Format.szName);

	return pMulTree;
}

//增加子节点
bool push_MulTree(MulTree* pMulTree, file_format stFile_Format)
{
	if(pMulTree == NULL)
	{
		return false;
	}

	//第一个子节点是否为空
	if(pMulTree->pFirstChild == NULL)
	{
		//为空 分配内存
		pMulTree->pFirstChild = (MulTree*)malloc(sizeof(MulTree));
		if(pMulTree->pFirstChild == NULL)
		{
			return false;
		}
		memset(pMulTree->pFirstChild, 0, sizeof(MulTree));

		//数据填充到第一个子节点
		copy_ff_MulTree(pMulTree->pFirstChild, stFile_Format);
		//设置第一个节点的父节点
		pMulTree->pFirstChild->pParent = pMulTree;
		//设置第一个子节点的上一个节点指向空
		pMulTree->pFirstChild->pPrev = NULL;
		//子节点数量为1
		pMulTree->nChildNum = 1;
		return true;
	}

	//最后一个子节点是否为空
	if(pMulTree->pLastChild == NULL)
	{
		//为空 分配内存
        pMulTree->pLastChild = (MulTree*)malloc(sizeof(MulTree));
       	if(pMulTree->pLastChild == NULL)
        {
           return false;
        }
        memset(pMulTree->pLastChild, 0, sizeof(MulTree));

		//数据填充到最后一个子节点
        copy_ff_MulTree(pMulTree->pLastChild, stFile_Format);
        //设置最后一个节点的父节点
		pMulTree->pLastChild->pParent = pMulTree;
		//设置最后一个子节点的上一个节点指向第一个子节点
        pMulTree->pLastChild->pPrev = pMulTree->pFirstChild;
		//设置第一个子节点的下一个节点为最后一个子节点
		pMulTree->pFirstChild->pNext = pMulTree->pLastChild;
		//子节点数量为1
		pMulTree->nChildNum = 2;

        return true;
	}

	//在末尾添加节点
	MulTree* pMulTree_temp = (MulTree*)malloc(sizeof(MulTree));
	if(pMulTree_temp == NULL)
	{
		return false;
	}
	memset(pMulTree_temp, 0, sizeof(MulTree));

	//数据填充到该节点
	copy_ff_MulTree(pMulTree_temp, stFile_Format);
	//设置该节点的父节点
	pMulTree_temp->pParent = pMulTree;
	//设置该节点的上一个节点为最后一个子节点
	pMulTree_temp->pPrev = pMulTree->pLastChild;
	//设置最后一个子节点的下一个节点为该节点
	pMulTree->pLastChild->pNext = pMulTree_temp;
	//最后一个子节点指向该节点
	pMulTree->pLastChild = pMulTree_temp;
	//子节点数量加一
	pMulTree->nChildNum++;

	return true;
}

//删除子节点
void remove_MulTree(MulTree* pMulTree, file_format stFile_Format)
{
	if(pMulTree == NULL)
	{
		return;
	}

	MulTree* pMulTree_temp = pMulTree->pFirstChild;
	while(pMulTree_temp != NULL)
	{
		//判断是否要删除的节点
		if(pMulTree_temp->stFile_Format.fid == stFile_Format.fid)
		{
			break;
		}
		//当前节点指向下一个节点
		pMulTree_temp = pMulTree_temp->pNext;
	}

	if(pMulTree_temp == NULL)
	{
		return;
	}

	//删除所有子节点
	MulTree* pMulTree_child_temp = pMulTree_temp->pFirstChild;
	while(pMulTree_child_temp != NULL)
	{
		//保存下一个子节点
		MulTree* pMulTree_next_child_temp = pMulTree_child_temp->pNext;
		//递归删除
		remove_MulTree(pMulTree_child_temp, pMulTree_child_temp->stFile_Format);
		//重新设置子节点
		pMulTree_child_temp = pMulTree_next_child_temp;
	}

	//如果要删除的是第一个节点
	if(pMulTree_temp == pMulTree->pFirstChild)
	{
		//是否只有一个节点
		if(pMulTree->nChildNum > 1)
		{
			//第二个节点的上一个节点指向空
			pMulTree_temp->pNext->pPrev = NULL;
			//第一个节点指向下一个节点
			pMulTree->pFirstChild = pMulTree_temp->pNext;
			
		}
		else
		{
			//第一个节点设置为空
			pMulTree->pFirstChild = NULL;		
		}
	}
	//如果要删除的是最后一个节点
	else if(pMulTree_temp == pMulTree->pLastChild)
	{
		//最后一个节点指向空
		pMulTree->pLastChild = NULL;
		//最后一个节点的上一个节点的下一个节点指向空
		pMulTree_temp->pPrev->pNext = NULL;
	}
	//如果要删除的是中间节点
	else
	{
		//当前节点的上一个节点的下一个节点指向当前节点的下一个节点
		pMulTree_temp->pPrev->pNext = pMulTree_temp->pNext;
		//当前节点的下一个节点的上一个节点指向当前节点的上一个节点
		pMulTree_temp->pNext->pPrev = pMulTree_temp->pPrev;

	}

	//子节点数量减一
	pMulTree->nChildNum--;
	//释放自身
	free(pMulTree_temp);
	pMulTree_temp = NULL;

}

//查找子节点
MulTree* find_MulTree(MulTree* pMulTree, char* pszName)
{
	MulTree* pMulTree_temp = pMulTree->pFirstChild;
	
	while(pMulTree_temp != NULL)
	{
		if(strcmp(pMulTree_temp->stFile_Format.szName, pszName) == 0)
		{
			break;
		}

		pMulTree_temp = pMulTree_temp->pNext;
	}

	return pMulTree_temp;
}

//复制文件格式
void copy_ff_MulTree(MulTree* pMulTree, file_format stFile_Format)
{
	if(pMulTree == NULL)
	{
		return;
	}

	//文件id
	pMulTree->stFile_Format.fid = stFile_Format.fid;
	//文件类型
    pMulTree->stFile_Format.File_Type = stFile_Format.File_Type;

	//文件名
    strcpy(pMulTree->stFile_Format.szName, stFile_Format.szName);
    //文件大小
	pMulTree->stFile_Format.nSize = stFile_Format.nSize;

	//文件创建和修改时间
    strcpy(pMulTree->stFile_Format.szCreateTime, stFile_Format.szCreateTime);
    strcpy(pMulTree->stFile_Format.szModifyTime, stFile_Format.szModifyTime);

	//内存块id
	pMulTree->stFile_Format.nMem_block_id = stFile_Format.nMem_block_id; 

	//子节点
	pMulTree->pFirstChild = pMulTree->pLastChild = NULL;
	//子节点数量
	pMulTree->nChildNum = 0;
	//父节点
    pMulTree->pParent = NULL;

	//兄弟节点
	pMulTree->pPrev = NULL;
	pMulTree->pNext = NULL;


}

void print_MultTree(MulTree* pMulTree)
{
	if(pMulTree == NULL)
	{
		return;
	}

	if(pMulTree->pParent != NULL)
	{
		printf("parent: %d self:%d %s\n", pMulTree->pParent->stFile_Format.fid, pMulTree->stFile_Format.fid, pMulTree->stFile_Format.szName);
	}
	else
	{
		printf("self:%d %s\n", pMulTree->stFile_Format.fid, pMulTree->stFile_Format.szName);
	
	}



	MulTree* pMulTree_temp = pMulTree->pFirstChild;
	while(pMulTree_temp != NULL)
	{
		print_MultTree(pMulTree_temp);
		pMulTree_temp = pMulTree_temp->pNext;
	}
}


//读取文件树
bool read_MulTree(MulTree* pMulTree)
{
	if(pMulTree == NULL)
	{
		return false;
	}

	file_format stFile_format;
	memset(&stFile_format, 0, sizeof(file_format));
		
	//读取文件格式	
	char szBuf[1024] = { 0 };
	//fid
	if(read(g_fd, szBuf, 10) < 10){ return false; }
	stFile_format.fid = atoi(szBuf);	
	
	if(g_nFile_id < stFile_format.fid)
	{
		g_nFile_id = stFile_format.fid;
	}
	
	//文件类型
	if(read(g_fd, szBuf, 10) < 10){ return false; }
	stFile_format.File_Type = (enum file_type)atoi(szBuf);
	//文件名
	if(read(g_fd, stFile_format.szName, sizeof(stFile_format.szName)) < sizeof(stFile_format.szName))
	{
		return false;
	}
	//文件大小
	if(read(g_fd, szBuf, 10) < 10){ return false; }
	stFile_format.nSize = atoi(szBuf);
	//文件创建时间
	if(read(g_fd, stFile_format.szCreateTime, sizeof(stFile_format.szCreateTime)) < sizeof(stFile_format.szCreateTime))
	{
		return false;
	}
	//文件修改时间
	if(read(g_fd, stFile_format.szModifyTime, sizeof(stFile_format.szModifyTime)) < sizeof(stFile_format.szModifyTime))
	{
		return false;
	}

	//内存块id
	if(read(g_fd, szBuf, 10) < 10){ return false; }
	stFile_format.nMem_block_id = atoi(szBuf);
	
	copy_ff_MulTree(pMulTree, stFile_format);
	
	//读取子节点数
	if(read(g_fd, szBuf, 10) < 10){ return false; }
	pMulTree->nChildNum = atoi(szBuf);



	//读取子节点
	MulTree** pMulTree_temp = &pMulTree->pFirstChild;
	MulTree* pMulTree_temp2 = NULL;
	for(int i = 0; i < pMulTree->nChildNum; i++)
	{
		//分配内存
		*pMulTree_temp = (MulTree*)malloc(sizeof(MulTree));
		if(*pMulTree_temp == NULL)
		{
			perror("malloc error:");
			exit(-1);
		}
		memset(*pMulTree_temp, 0, sizeof(MulTree));

		//读取文件树
		read_MulTree(*pMulTree_temp);
	
		//设置父节点
		(*pMulTree_temp)->pParent = pMulTree;
		//设置上一个节点
		(*pMulTree_temp)->pPrev = pMulTree_temp2;
		//保存当前子节点
		pMulTree_temp2 = *pMulTree_temp;	

		//下一个子节点
		pMulTree_temp = &(*pMulTree_temp)->pNext;
	}

	//判断是否大于1
	if(pMulTree->nChildNum > 1)
	{
		pMulTree->pLastChild = pMulTree_temp2;
	}

	return true;
}


//保存文件树
void save_MulTree(MulTree* pMulTree)
{
	if(pMulTree == NULL)
	{
		return;
	}

	char szBuf[1024] = { 0 };
	
	

	//保存自身
	//fid
	sprintf(szBuf, "%d", pMulTree->stFile_Format.fid);
	write(g_fd, szBuf, 10);
	//文件类型
	sprintf(szBuf, "%d", pMulTree->stFile_Format.File_Type);
	write(g_fd, szBuf, 10);	
	//文件名
	write(g_fd, pMulTree->stFile_Format.szName, sizeof(pMulTree->stFile_Format.szName));	
	//文件大小
	sprintf(szBuf, "%d", pMulTree->stFile_Format.nSize);
	write(g_fd, szBuf, 10);	
	//文件创建时间
	write(g_fd, pMulTree->stFile_Format.szCreateTime, sizeof(pMulTree->stFile_Format.szCreateTime));	
	//文件修改时间
	write(g_fd, pMulTree->stFile_Format.szModifyTime, sizeof(pMulTree->stFile_Format.szModifyTime));	
	//内存块id
	sprintf(szBuf, "%d", pMulTree->stFile_Format.nMem_block_id);
	write(g_fd, szBuf, 10);	

	//子节点数
	sprintf(szBuf, "%d", pMulTree->nChildNum);
	write(g_fd, szBuf, 10);
	

	//递归所有子节点
	MulTree* pMulTree_temp = pMulTree->pFirstChild;
	while(pMulTree_temp != NULL)
	{
		save_MulTree(pMulTree_temp);
		pMulTree_temp = pMulTree_temp->pNext;
	}

}

