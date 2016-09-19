/*
 * logger.h
 *
 *  Created on: 2015-6-24
 *      Author: hongjianan
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define FILENAME ( NULL == strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '/')+1): strrchr(__FILE__, '\\')+1)

typedef struct
{
    FILE    		*fileFd;
    unsigned int	maxFileSize;
}LogInfo;

extern LogInfo gLogInfo;


#define LOG_DBG(moduleId, fmt, args...) \
{\
	isNeedRoll(&gLogInfo);\
	fprintf(gLogInfo.fileFd, "[DBG][%14.14s,L%.4d] : "fmt, FILENAME, __LINE__, ##args);\
}

#define LOG_INFO(moduleId, fmt, args...) \
{\
	isNeedRoll(&gLogInfo);\
	fprintf(gLogInfo.fileFd, "[INF][%14.14s,L%.4d] : "fmt, FILENAME, __LINE__, ##args);\
}

#define LOG_WARN(moduleId, fmt, args...) \
{\
	isNeedRoll(&gLogInfo);\
	fprintf(gLogInfo.fileFd, "[WAR][%14.14s,L%.4d] : "fmt, FILENAME, __LINE__, ##args);\
}

#define LOG_ERROR(moduleId, fmt, args...) \
{\
	isNeedRoll(&gLogInfo);\
	fprintf(gLogInfo.fileFd, "[ERR][%14.14s,L%.4d] : "fmt, FILENAME, __LINE__, ##args);\
}

int InitLog(LogInfo *pLogInfo, const char *fileName, int maxFileSize);
void isNeedRoll(LogInfo *pLogInfo);

#endif /* LOGGER_H_ */
