#ifndef COMMAND_H
#define COMMAND_H
#include "global.h"
#include "multree.h"
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int ParseCmd(char* pszCmd, int nLen, char** pszParseCmd, char chVal);

void DispatchCmd(char** pszParseCmd, int nCnt);

void ExecuteCmd();

void cd_cmd(char** pszParseCmd, int nCnt);

void cat_cmd(char** pszParseCmd, int nCnt);

void mkdir_cmd(char** pszParseCmd, int nCnt);

void dir_cmd(char** pszParseCmd, int nCnt);

void truncate_cmd(char** pszParseCmd, int nCnt);

void help_cmd(char** pszParseCmd, int nCnt);

void rm_cmd(char** pszParseCmd, int nCnt);

void echo_cmd(char** pszParseCmd, int nCnt);

void clr_cmd(char** pszParseCmd, int nCnt);

void environ_cmd(char** pszParseCmd, int nCnt);

void pause_cmd(char** pszParseCmd, int nCnt);

void import_cmd(char** pszParseCmd, int nCnt);

void export_cmd(char** pszParseCmd, int nCnt);

void ExtractNameFromPath(char* pszPath, int nSize, char* pszName);



#endif
