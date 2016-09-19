/*
 * baseTypedef.h
 *
 *  Created on: 2015-2-17
 *      Author: Hong
 */
#ifndef COMMON_BASETYPEDEF_H_
#define COMMON_BASETYPEDEF_H_

typedef char				Int8;
typedef unsigned char		Uint8;
typedef short				Int16;
typedef unsigned short		Uint16;
typedef int					Int32;
typedef unsigned int		Uint32;
typedef long long			Int64;
typedef unsigned long long	Uint64;

typedef Uint8		       	BOOL;
typedef Int32				STATUS;

#undef	__64bit__
#define	__64bit__

#ifdef	__64bit__
typedef long long			LPTR;
typedef unsigned long long	ULPTR;

#else	/* 32bit */
typedef int					LPTR;
typedef unsigned int		ULPTR;

#endif /* __64bit__ */

#define TRUE                1
#define FALSE               0

#define STATUS_OK          (0)
#define STATUS_ERROR       (-1)

#define APP_STATUS_SUCCESS  (0)
#define APP_STATUS_ERROR    (-1)

#define WAIT_FOREVER 		-1
#define NO_WAIT 			0


/* moduleId */
typedef enum
{
	MODULE_ID_SYS_SERVICE                   = 0x01,  /* 系统服务子系统 */
}E_ModuleId;

/* LOG */
#include <stdio.h>

#define FILENAME /*lint -save -e613 */( NULL == strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '/')+1): strrchr(__FILE__, '\\')+1)
#define FUNCNAME __FUNCTION__
#define LINE     __LINE__

#define LOG_DBG(moduleId, fmt, args...){\
		(void)moduleId;\
		printf("[DBG][%14.14s,L%.4d]: "fmt, FILENAME, LINE, ##args);}
#define LOG_INFO(moduleId,fmt, args...){\
		(void)moduleId;\
		printf("[INF][%14.14s,L%.4d]: "fmt, FILENAME, LINE, ##args);}
#define LOG_WARN(moduleId, fmt, args...){\
		(void)moduleId;\
		printf("[WAR][%14.14s,L%.4d]: "fmt, FILENAME, LINE, ##args);}
#define LOG_ERROR(moduleId, fmt, args...){\
		(void)moduleId;\
		printf("[ERR][%14.14s,L%.4d]: "fmt, FILENAME, LINE, ##args);}

#endif /* COMMON_BASETYPEDEF_H_ */
/* end file */
