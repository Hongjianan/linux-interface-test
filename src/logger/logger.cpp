/*
 * logOutput.cpp
 *
 *  Created on: 2015-6-24
 *      Author: hongjianan
 */
#include "../config.h"
#if LOGGERTEST_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "logger.h"


LogInfo gLogInfo;

int InitLog(LogInfo *pLogInfo, const char *fileName, int maxFileSize)
{
    if(NULL == pLogInfo)
    {
        printf("LogInfo is NULL.\n");
        return -1;
    }
    if(NULL == fileName)
    {
        printf("fileName is NULL.\n");
        return -1;
    }
    if(maxFileSize <= 0)
    {
        printf("maxFileSize is too small.\n");
        return -1;
    }

    pLogInfo->fileFd = fopen(fileName, "wb");
    pLogInfo->maxFileSize = maxFileSize;

    return 0;
}


void isNeedRoll(LogInfo *pLogInfo)
{
    int logLen = ftell(pLogInfo->fileFd);
    if(-1 == logLen)   /* 不存在该文件 */
    {
        fclose(pLogInfo->fileFd);
    }
    else if(logLen >= pLogInfo->maxFileSize)
    {
        rewind(pLogInfo->fileFd);
    }
}

#endif /* LOGGERTEST_CPP */
/* end file */
