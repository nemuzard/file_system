#ifndef MULTREE_H
#define MULTREE_H
#include "structure.h"
#include <stdbool.h>

//文件树初始化
MulTree* init_MulTree();
//增加子节点
bool push_MulTree(MulTree* pMulTree, file_format stFile_Format);
//删除子节点
void remove_MulTree(MulTree* pMulTree, file_format stFile_Format);
//查找子节点
MulTree* find_MulTree(MulTree* pMulTree, char* pszName);
//复制文件格式
void copy_ff_MulTree(MulTree* pMulTree, file_format stFile_Format);
//打印所有子节点
void print_MultTree(MulTree* pMulTree);

//读取文件树
bool read_MulTree(MulTree* pMulTree);

//保存文件树
void save_MulTree(MulTree* pMulTree);

#endif
