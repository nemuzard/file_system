#include "command.h"


int ParseCmd(char* pszCmd, int nLen, char** pszParseCmd, char chVal)
{
	int nCnt = 0;
	char szBuf[1024] = { 0 };
	for(int i = 0, j = 0; i < nLen; i++)
	{
		if(pszCmd[i] != chVal)
		{
			szBuf[j++] = pszCmd[i];
			continue;
		}

		//获取部分长度
		int nPartLen = strlen(szBuf);
		//分配内存
		pszParseCmd[nCnt] = (char*)malloc(nPartLen);
		if(pszParseCmd[nCnt] == NULL)
		{
			perror("malloc error:");
			exit(-1);
		}
		memset(pszParseCmd[nCnt], 0, nPartLen);

		//拷贝
		strcpy(pszParseCmd[nCnt], szBuf);

		//清空buf
		memset(szBuf, 0, sizeof(szBuf));
		j = 0;
		nCnt++;
	}

	//获取部分长度
	int nPartLen = strlen(szBuf);
	//分配内存
	pszParseCmd[nCnt] = (char*)malloc(nPartLen);
	if(pszParseCmd[nCnt] == NULL)
	{
		perror("malloc error:");
		exit(-1);
	}
	memset(pszParseCmd[nCnt], 0, nPartLen);
	//拷贝
	strcpy(pszParseCmd[nCnt], szBuf);
	//清空buf
	memset(szBuf, 0, sizeof(szBuf));
	nCnt++;
		
	return nCnt;
}

void DispatchCmd(char** pszParseCmd, int nCnt)
{
	if(strcmp(pszParseCmd[0], "cd") == 0)
	{
		cd_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "cat") == 0)
	{
		cat_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "dir") == 0)
	{
		dir_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "mkdir") == 0)
	{
		mkdir_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "truncate") == 0)
	{
		truncate_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "help") == 0)
	{
		help_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "rm") == 0)
	{
		rm_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "echo") == 0)
	{
		echo_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "clr") == 0)
	{
		clr_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "environ") == 0)
	{
		environ_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "pause") == 0)
	{
		pause_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "import") == 0)
	{
		import_cmd(pszParseCmd, nCnt);
	}
	else if(strcmp(pszParseCmd[0], "export") == 0)
	{
		export_cmd(pszParseCmd, nCnt);
	}



	else
	{
		printf("\'%s\' is not command\n", pszParseCmd[0]);
	}
}
 
void ExecuteCmd()
{
	while(1)
	{
		char szCmd[1024] = { 0 };
		printf("%s$ ", g_szCurrentPath);
		//等待用户输入指令
		scanf("%[^\n]%*c", szCmd);
		if(strcmp(szCmd, "quit") == 0)
		{
			break;
		}
		//获取指令长度
		int nLen = strlen(szCmd);

		char* pszParseCmd[64] = { 0 };

		//解析指令
		nLen = ParseCmd(szCmd, nLen, pszParseCmd, 0x20);

		//处理指令
		DispatchCmd(pszParseCmd, nLen);
 	
		//释放指令内存
		for(int i = 0; i < nLen; i++)
		{
			free(pszParseCmd[i]);
		}
		
	}
}

void cd_cmd(char** pszParseCmd, int nCnt)
{
	MulTree* pMulTree_temp = NULL;
	if(nCnt == 1)
	{
		//显示帮助
		printf("cd: missing operand\n");
		printf("Try 'cd --help' for more infomation.\n");
	}
	else if(nCnt == 2)
	{

		//判断是否显示帮助
		if(strcmp(pszParseCmd[1], "--help") == 0)
		{
			printf("cd:\n");
			printf("cd ~\n");
			printf("cd ..\n");
			printf("cd [directory]/[directory]/[directory]\n");
			return;
		}

		char* pszParseCmd_out[64] = { 0 };
	
		//解析指令
		int nLen = ParseCmd(pszParseCmd[1], strlen(pszParseCmd[1]), pszParseCmd_out, '/');
		if(nLen == 1)
		{

			//根目录
			if(strcmp(pszParseCmd_out[0], "~") == 0)
			{
				g_pstCurrentMulTree = g_pstDisk_format->pMulTree;
				memset(g_szCurrentPath, 0, strlen(g_szCurrentPath));
				g_szCurrentPath[0] = '~';
				return;
			} 
			//上一级目录
			else if(strcmp(pszParseCmd_out[0], "..") == 0)
			{
				if(g_pstCurrentMulTree->pParent == NULL)
				{
					return;
				}
				g_pstCurrentMulTree = g_pstCurrentMulTree->pParent;
				for(int i = strlen(g_szCurrentPath) - 1; i > 0; i--)
				{
					if(g_szCurrentPath[i] == '/')
					{
						g_szCurrentPath[i] = '\0';
						break;
					}
					g_szCurrentPath[i] = '\0';
				}
				return;
			}
			//当前目录
			else
			{
				//判断当前目录下的文件夹
				pMulTree_temp = find_MulTree(g_pstCurrentMulTree, pszParseCmd_out[0]);
				if(pMulTree_temp == NULL || pMulTree_temp->stFile_Format.File_Type != s_directory)
				{
					//子节点下没有相应的目录
					return;
				}
		
				//设置当前节点为根节点
				g_pstCurrentMulTree = pMulTree_temp;
				strcat(g_szCurrentPath, "/");
				strcat(g_szCurrentPath, pMulTree_temp->stFile_Format.szName);

			}	
		
			return;	
		}

		char szBuf[4096] = { 0 };

		//多级目录
		//判断从根目录下找还是当前目录下找
		if(strcmp(pszParseCmd_out[0], "~") == 0)
		{
			//根目录
			pMulTree_temp = g_pstDisk_format->pMulTree;
		}
		else
		{
			//当前目录
			pMulTree_temp = g_pstCurrentMulTree;
		}

		//开始查找
		for(int i = 0; i < nLen && pMulTree_temp != NULL; i++)
		{
			pMulTree_temp = find_MulTree(pMulTree_temp, pszParseCmd_out[i]);
			if(pMulTree_temp == NULL)
			{
				return;
			}
			strcat(szBuf, "/");
			strcat(szBuf, pMulTree_temp->stFile_Format.szName);	
		}

		//判断是否为空
		if(pMulTree_temp == NULL)
		{
			//没找到
			//还原当前路径
			
			return;
		}

		//找到
		//设置当前节点为找到的节点
		g_pstCurrentMulTree = pMulTree_temp;
		strcat(g_szCurrentPath, szBuf);	

	}
	else
	{
		//出错
		printf("cd:\n");
		printf("cd ~\n");
		printf("cd ..\n");
		printf("cd [directory]/[directory]/[directory]\n");

	}
}

void cat_cmd(char** pszParseCmd, int nCnt)
{
	char szBuf[1024] = { 0 };
	//判断参数个数
	if(nCnt == 1)
	{
		//显示用法
		printf("cat: missing operand\n");
		printf("Try 'cat --help' for more infomation.\n");

	}
	else if(nCnt == 2)
	{
		//判断是否显示帮助
		if(strcmp(pszParseCmd[1], "--help") == 0)
		{
			printf("cat:\n");
			printf("cat > [file]\n");
			printf("cat [file]\n");
			return;
		}


		//显示文件内容
		//打开文件
		int fildes = fs_open(pszParseCmd[1]);
		if(fildes == -1)
		{
			//打开文件失败
			printf("open failure!\n");
			return;
		}
		
		//获取文件大小
		int nSize = fs_get_filesize(fildes);
		if(nSize <= 0)
		{
			return;
		}

		//分配指定大小文件的内存
		char* pszBuf = (char*)malloc(nSize);
		if(pszBuf == NULL)
		{
			perror("malloc error:");
			return;
		}
		memset(pszBuf, 0, nSize);

		//读取
		fs_read(fildes, pszBuf, nSize);
		
		//打印
		printf("%s\n", pszBuf);

		//释放内存
		free(pszBuf);
			

		//关闭文件
		fs_close(fildes);

	}
	else if(nCnt == 3)
	{
		//创建文件
		//判断第二个参数是否为 >
		if(strcmp(pszParseCmd[1], ">") != 0)
		{
			//出错
			printf("command fortmat error\n");
			return;
		}

		//创建文件
		if(fs_create(pszParseCmd[2]))
		{
			//创建失败
			printf("create failure!\n");
			return;
		}
		//打开文件
		int fildes = fs_open(pszParseCmd[2]);
		if(fildes == -1)
		{
			//打开文件失败
			printf("open failure!\n");
			return;
		}

		scanf("%[^\n]%*c", szBuf);

		//写入文件
		if(fs_write(fildes, szBuf, strlen(szBuf)) == -1)
		{
			//写入文件失败
			printf("write failure!\n");
		}
		
		//关闭文件
		fs_close(fildes);
	}
	else
	{
		//出错
		printf("cat:\n");
		printf("cat > [file]\n");
		printf("cat [file]\n");
	}
}

void mkdir_cmd(char** pszParseCmd, int nCnt)
{
	//判断参数个数
	if(nCnt == 1)
	{
		//显示用法
		printf("mkdir: missing operand\n");
		printf("Try 'mkdir --help' for more infomation.\n");

	}
	else if(nCnt == 2)
	{
		//判断是否显示帮助
		if(strcmp(pszParseCmd[1], "--help") == 0)
		{
			printf("mkdir:\n");
			printf("mkdir [directory]\n");;
			return;
		}

		fs_mkdir(pszParseCmd[1]);
	}
	else
	{
		//出错
		printf("mkdir:\n");
		printf("mkdir [directory]\n");;
	}
}



void dir_cmd(char** pszParseCmd, int nCnt)
{
	MulTree* pMulTree = NULL;
	if(nCnt == 1)
	{
		//获取当前节点下第一个子节点
		pMulTree = g_pstCurrentMulTree->pFirstChild;
		//遍历所有子节点
		while(pMulTree != NULL)
		{

			printf("%10d ", pMulTree->stFile_Format.nSize);
			printf("%s ", pMulTree->stFile_Format.szModifyTime);
			if(pMulTree->stFile_Format.File_Type == s_file)
			{
				printf("     file ");
			}
			else
			{
				printf("directory ");
			}

			printf("%s\n", pMulTree->stFile_Format.szName);

			//指向下一个子节点
			pMulTree = pMulTree->pNext;
		}
	}
	else if(nCnt == 2)
	{
		//判断是否显示帮助
		if(strcmp(pszParseCmd[1], "--help") == 0)
		{
			printf("dir\n");
			return;
		}

	}
	else
	{
		//出错
		printf("dir");
	}
}

void truncate_cmd(char** pszParseCmd, int nCnt)
{
	if(nCnt == 2)
	{
		//显示帮助
	}
	else if(nCnt == 4)
	{
		//大小
		int nSize = 0;
		//文件名
		char* pszName = NULL;
		//判断-s位置
		if(strcmp(pszParseCmd[1], "-s") == 0)
		{
			nSize = atoi(pszParseCmd[2]);
			pszName = pszParseCmd[3];
		}
		else if(strcmp(pszParseCmd[2], "-s") == 0)
		{
			nSize = atoi(pszParseCmd[3]);
			pszName = pszParseCmd[1];
		}
		else
		{
			printf("truncate:\n");
			printf("truncate [file] -s [size]\n");

			//出错
			return;
		}
	
		//打开文件
		int fildes = fs_open(pszName);
		if(fildes == -1)
		{
			//打开文件失败
			printf("open failure!\n");
			return;
		}

		fs_truncate(fildes, nSize);

		//关闭文件
		fs_close(fildes);
	}
	else
	{
		printf("truncate:\n");
		printf("truncate [file] -s [size]\n");
	}
}

void help_cmd(char** pszParseCmd, int nCnt)
{
	if(nCnt == 1)
	{
		printf("cd: Change the current path\n");
		printf("cat: Creating or Reading a file\n");
		printf("mkdir: Createing a derectory\n");
		printf("dir: Display all files and directories\n");
		printf("rm: Removing a file or directory\n");
		printf("echo: Display comment\n");
		printf("clr: Clear the screen\n");
		printf("environ: List all the environment string\n");
		printf("pause: Pause Operation of the shell until 'Enter' is pressedn\n");
		printf("quit: Quit the shell\n");
	}
	else
	{
		//出错
		printf("help\n");
	}
}


void rm_cmd(char** pszParseCmd, int nCnt)
{
	if(nCnt == 1)
	{
		//显示帮助
		printf("rm: missing operand\n");
		printf("Try 'rm --help' for more infomation.\n");
	}
	else if(nCnt == 2)
	{
		//判断是否显示帮助
		if(strcmp(pszParseCmd[1], "--help") == 0)
		{
			printf("rm:\n");
			printf("rm [file]\n");
			printf("rm [directory]\n");
			return;
		}

		//删除
		fs_delete(pszParseCmd[1]);

	}
	else
	{
		//出错
		printf("rm:\n");
		printf("rm [file]\n");
		printf("rm [directory]\n");

	}
}

void echo_cmd(char** pszParseCmd, int nCnt)
{
	if(nCnt == 1)
	{
		//显示帮助
		printf("echo: missing operand\n");
		printf("Try 'echo --help' for more infomation.\n");
	}
	else if(nCnt == 2)
	{
		//判断是否显示帮助
		if(strcmp(pszParseCmd[1], "--help") == 0)
		{
			printf("rm:\n");
			printf("rm [comments]\n");
			return;
		}

		//显示
		printf("%s\n", pszParseCmd[1]);

	}
	else
	{
		//出错
		printf("rm:\n");
		printf("rm [comments]\n");

	}

}

void clr_cmd(char** pszParseCmd, int nCnt)
{
	if(nCnt == 1)
	{
		system("clear");
	}
	else
	{
		//出错
		printf("clr\n");
	}
}

void environ_cmd(char** pszParseCmd, int nCnt)
{
	if(nCnt == 1)
	{

	}
	else
	{
		//出错
		printf("environ\n");
	}
}

void pause_cmd(char** pszParseCmd, int nCnt)
{
	if(nCnt == 1)
	{
		//显示帮助
		printf("press enter key to continue...\n");
		while(getchar() != '\n');
	}
	else
	{
		//出错
		printf("pause\n");
	}

}

void import_cmd(char** pszParseCmd, int nCnt)
{
	if(nCnt == 1)
	{
		//显示帮助
	}
	else if(nCnt == 2)
	{
		//打开文件
		int fd = open(pszParseCmd[1], O_RDONLY);
		if(fd == -1)
		{
			//打开失败
			printf("open failed: %s\n", pszParseCmd[1]);
			return;		
		}

		//获取文件长度
		struct stat statBuf;
		memset(&statBuf, 0, sizeof(struct stat));

		//获取文件长度
		if(fstat(fd, &statBuf) != 0)
		{
			printf("fstat error!\n");
			close(fd);
			return;
		}

		//分配内存
		char* pBuf = (char*)malloc(statBuf.st_size);
		if(pBuf == NULL)
		{
			perror("malloc error: ");
			exit(-1);
		}
		memset(pBuf, 0, statBuf.st_size);
		

		//读取数据
		if(read(fd, pBuf, statBuf.st_size) == -1)
		{ 
			printf("read error!\n"); 
			close(fd);
			return;
		}
		close(fd);

		//提取文件名
		char szName[64] = { 0 };
		ExtractNameFromPath(pszParseCmd[1], strlen(pszParseCmd[1]), szName);
		
		//创建新文件
		if(fs_create(szName) == -1)
		{
			return;
		}
	
		//打开文件
		int fildes = fs_open(szName);
		if(fildes == -1)
		{
			printf("open error!\n");
			return;
		}

		//写入数据
		fs_write(fildes, pBuf, statBuf.st_size);
		//关闭文件
		fs_close(fildes);

	}
	else
	{
		//出错
		printf("import:\n");
		printf("import [real file path]\n");
	}
}


void export_cmd(char** pszParseCmd, int nCnt)
{
	if(nCnt == 1)
	{
		//显示帮助
	}
	else if(nCnt == 3)
	{
		//打开文件
		int fildes = fs_open(pszParseCmd[1]);
		if(fildes == -1)
		{
			printf("open error!\n");
			return;
		}

		char* pBuf = (char*)malloc(g_pstCurrentFile_format->nSize);
		if(pBuf == NULL)
		{
			perror("malloc error: ");
			return;
		}
		memset(pBuf, 0, g_pstCurrentFile_format->nSize);

		//读取文件
		fs_read(fildes, pBuf, g_pstCurrentFile_format->nSize);
	
		//创建文件
		int fd = open(pszParseCmd[2], O_CREAT | O_RDWR | O_TRUNC, 0664);
		if(fd == -1)
		{
			perror("open error: ");
			fs_close(fildes);
			return;
		}

		//写入
		write(fd, pBuf, g_pstCurrentFile_format->nSize);

		//关闭文件
		fs_close(fildes);

		close(fd);
	}
	else
	{
		//出错
		printf("import:\n");
		printf("import [real file path] [file]\n");
	}

}


void ExtractNameFromPath(char* pszPath, int nSize, char* pszName)
{
	int i = 0;
	for(i = nSize - 1; i >= 0; i--)
	{
		if(pszPath[i] == '\\' || pszPath[i] == '/')
		{
			break;
		}
	}

	i++;

	for(int j = 0; i < nSize; i++, j++)
	{
		pszName[j] = pszPath[i];
	}
	
}
