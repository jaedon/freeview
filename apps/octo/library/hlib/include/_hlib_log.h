/*************************************************************
 * @file		hlib_log.h
 * @date		2011/01/18
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, related with logging system?

 * http://www.humaxdigital.com
 *************************************************************/

#define	____HXLOG_README___________________

/******************************************************************************
 [Example Code]

 [Expected Result]

 ************************************************************************************/


#ifndef __HLIB_LOG_H__
#define __HLIB_LOG_H__

//#include <htype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#include "htype.h"


#define	____HXLOG_CONFIG___________________


/**
 *  Log4c 임시 사용 플레그
 */
//#define ENABLE_LOG4C


/**
 * domain name을 지정 (macro 함수에서 사용함)
 */
#ifndef HxLOG_DOMAIN
#define	HxLOG_DOMAIN	NULL
#endif
/**
 * domain name을 'strdup'함수를 통해 제 할당 (아니면, 그냥 주소 복사)
 */
#ifndef HxLOG_DOMAIN_NAMEDUP
#define	HxLOG_DOMAIN_NAMEDUP	1
#endif
/**
 * Domain Name의 대소문자 비교를 사용할 지 여부
 */
#ifndef	HxLOG_DOMAIN_IGNORE_CASE
#define	HxLOG_DOMAIN_IGNORE_CASE	1
#endif
/**
 * assert에 해당되는 경우 시스템을 멈춤
 */
#ifndef HxLOG_ASSERT_ABORT
#define	HxLOG_ASSERT_ABORT	1
#endif
/**
 * __HxFILE__	define에 path가 포함된 경우, 삭제 하고 출력
 */
#ifndef	HxLOG_FILE_WITHOUT_PATH
#define	HxLOG_FILE_WITHOUT_PATH	1
#endif

//#define HXLOG_PRINT_LINE_FUNC
#define HXLOG_PRINT_PIDTID

#define	____HXLOG_TYPE___________________


#if defined(__FILE__)
	#define	__HxFILE__	__FILE__
#else
	#define	__HxFILE__	""
#endif

#if defined(__LINE__)
	#define	__HxLINE__	__LINE__
#else
	#define	__HxLINE__	(-1)
#endif

typedef enum
{
	HxLOG_NONE		= 0,

	HxLOG_PRINT		= 0x0001,
	HxLOG_MESSAGE		= 0x0002,
	HxLOG_INFO			= 0x0004,
	HxLOG_DEBUG		= 0x0008,

	HxLOG_TRACE		= 0x0010,

	HxLOG_WARNING	= 0x0100,
	HxLOG_ERROR		= 0x0200,
	HxLOG_CRITICAL		= 0x0400,
	HxLOG_ASSERT 		= 0x0800,

	HxLOG_FATAL		= (HxLOG_ERROR | HxLOG_CRITICAL | HxLOG_ASSERT),

	HxLOG_ALL		= 0xFFFFFFFF
} HxLOG_Level_e;



typedef void (* HxLOG_Cb_t) (const HCHAR *domain, HxLOG_Level_e lv, const HCHAR *msg, void *userdata,const HCHAR *format,...);


#define	____HXLOG_APIs___________________

/**
 * HLIB_LOG 초기화  함수
 *
 * @param	EnableNetLog	     FALSE: stdout  TRUE: stdout+netlog
 */
void	HLIB_LOG_Init(HBOOL EnableNetLog);


void	HLIB_LOG (const HCHAR *file, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv, const HCHAR *format, ...);


/**
 * HLIB_LOG 에서 ... 대신 va_list
 *
 * @param	path		호출한 파일  경로
 * @param	function	호출 함수 이름
 * @param	line		호출 라인
 * @param	domain	log 도메인(ex. TTX, EPG ...)
 * @param	lv		log level (프린트 하고자 하는 내용의 log level)
 * @param	fmt		format
 * @param	va		v-arguments
 */
void	HLIB_LOG_v (const HCHAR *path, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv, const HCHAR *fmt, va_list ap);

void 	HLIB_LOG_PrintDomain(void);

/**
 * domain 별로 관리되는 log-level을 setting
 *
 * @param	domain	log 도메인(ex. TTX, EPG ...)
 * @param	lv		log level
 */
void	HLIB_LOG_SetLevel	(const HCHAR *domain, HxLOG_Level_e lv);

/**
 * domain 별로 관리되는 log-level을 get
 *
 * @param	domain	log 도메인(ex. TTX, EPG ...)
 * @return	HxLOG_Level_e		log level
 */
HxLOG_Level_e	HLIB_LOG_GetLevel	(const HCHAR *domain);

/**
 * domain 별로 관리되는 log handler를 setting
 * - 사용자가 handler를 세팅할 수
 *
 * @param	domain	log 도메인(ex. TTX, EPG ...)
 * @param	handler	log handler
 * @param	userdata	userdata -> log handler
 */
void	HLIB_LOG_SetHandler (const HCHAR *domain, HxLOG_Cb_t handler, void *userdata);

/**
 * global 하게 On/Off할 수 있는 log-level
 *
 * @param	lv	log lv
 */
void	HLIB_LOG_SetGlobalLevel (HxLOG_Level_e lv);


void HLIB_LOG_Debug(const HCHAR *file, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv, const HCHAR *format, ...);

#if HxLOG_FILE_WITHOUT_PATH == 1 && defined(__FILE__)
// __HxFILE__에서 path를 지우기 위한 함수
const char *	HLIB_LOG_RemovePath(const char *file);
#undef	__HxFILE__
#define	__HxFILE__	HLIB_LOG_RemovePath(__FILE__)
#endif

/**
 * Stream data를 print할 수 있는 Dump API
 *
 * @param	pucBuffer			pointer of stream data
 * @param	unSize			size of stream data
 * @param	unBaseAddr		base address for display
 * @param	bFlagDetail		flag of detail
 */
void HLIB_LOG_Dump (HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail);

void HLIB_LOG_DumpEx (HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail, FILE *file);

/**
 * Default Handler의 Level 세팅
 *
 * @param	lv	log level
 */
#define	HxLOG_SetDefaultLevel(lv)	HLIB_LOG_SetLevel(NULL, lv);



/**
 * domain에 등록된 핸들러 삭제
 *
 * @param	domain	log 도메인(ex. TTX, EPG ...)
 */
#define	HxLOG_RemoveHandler(d)		HLIB_LOG_SetHandler(d, HxLOG_NONE, NULL, NULL)

/**
 * Default Handler 세팅
 *
 * @param	h	handler
 * @param	u	userdata
 */
#define	HxLOG_SetDefaultHandler(h,u)	\
									HLIB_LOG_SetHandler(NULL, h, u)

/******
 * HLIB_LOG 확장!!
 */
#define	HxLOG_NullPrint			while (0) ((int (*)(const char *, ...)) 0)

#if defined(CONFIG_DEBUG) || defined(CONFIG_DEBUG_RELEASE_LOG)
#define HxLOG 	HLIB_LOG
#else
#undef	HxLOG_ASSERT_ABORT
#define HxLOG 	HxLOG_NullPrint
#endif

#if defined(CONFIG_DEVELOP_RELEASE)
#define	HxLOG_Error(...)		do {fprintf(stderr,"\t %s:<%s:%d>; ",HLIB_STD_GetCurProcessName(),__func__,__LINE__); \
					fprintf(stderr,__VA_ARGS__);} while(0)
#define	HxLOG_Critical(...)		do {fprintf(stderr,"\t %s:<%s:%d>; ",HLIB_STD_GetCurProcessName(),__func__,__LINE__); \
					fprintf(stderr,__VA_ARGS__);} while(0)
#define	HxLOG_Warning(...)		do {fprintf(stderr,"\t %s:<%s:%d>; ",HLIB_STD_GetCurProcessName(),__func__,__LINE__); \
					fprintf(stderr,__VA_ARGS__);} while(0)
#else
#define	HxLOG_Error(...)		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_ERROR, __VA_ARGS__)
#define	HxLOG_Critical(...)		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_CRITICAL, __VA_ARGS__)
#define	HxLOG_Warning(...)		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_WARNING, __VA_ARGS__)
#endif
#define	HxLOG_Print(...)		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_PRINT, __VA_ARGS__)
#define	HxLOG_Message(...)		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_MESSAGE, __VA_ARGS__)
#define	HxLOG_Info(...)			HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_INFO, __VA_ARGS__)
#define	HxLOG_Debug(...)		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_DEBUG, __VA_ARGS__)
#define	HxLOG_Trace()			HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_TRACE, "\tTR[%s:%d:%s]\n", __FUNCTION__, __HxLINE__, __HxFILE__)

#if HxLOG_ASSERT_ABORT == 1
#define	HxLOG_Assert(expr)	\
	if (!(expr)) {\
		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_ASSERT, "-----------------------------------\n");\
		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_ASSERT, "%s:%d:%s: Assertion \'%s\' failed!!\n", __FUNCTION__, __HxLINE__, __HxFILE__, #expr);\
		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_ASSERT, "You will die, i will call abort()!!!!!!!!!!\n");\
		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_ASSERT, "-----------------------------------\n");\
		abort();\
	}
#else
#define	HxLOG_Assert(expr)	\
	if (!(expr)) {\
		HxLOG(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_ASSERT, "%s:%d:%s: Assertion \'%s\' failed!!\n", __FUNCTION__, __HxLINE__, __HxFILE__, #expr);\
	}
#endif

#define	HxLog_NullCheckReturnValue(ptr, ret) \
		if (NULL == ptr)	\
		{	\
			HxLOG_Error("Check null (%s) \n", #ptr); \
			return ret;	\
		}

#define	HxLog_NullCheckReturnOnly(ptr)	\
		if (NULL == ptr)	\
		{	\
			HxLOG_Error("Check null (%s) \n", #ptr); \
			return;	\
		}

#define	HxLog_ExprCheckReturnValue(expr, ret) \
		if (expr)	\
		{	\
			HxLOG_Error("Check expr (%s) \n", #expr); \
			return ret;	\
		}

#define	HxLog_ExprCheckReturnOnly(expr)	\
		if (expr)	\
		{	\
			HxLOG_Error("Check expr (%s) \n", #expr); \
			return;	\
		}


#define HxLOG_DecIntDump(x)		HxLOG_Print("\t%-20s : %d\r\n", #x, x);
#define HxLOG_HexIntDump(x)		HxLOG_Print("\t%-20s : 0x%x\r\n", #x, x);
#define HxLOG_BoolDump(x)		HxLOG_Print("\t%-20s : %s\r\n", #x, ((x == TRUE) ? "TRUE" : "FALSE"));
#define HxLOG_StringDump(x)		HxLOG_Print("\t%-20s : %s\r\n", #x, x);
#define HxLOG_RectDump(rect)	HxLOG_Print("\t%-20s : (%d,%d,%d,%d)\r\n", #rect, rect.x, rect.y, rect.w, rect.h);
#define HxLOG_AreaDump(area)	HxLOG_Print("\t%-20s : (%d,%d,%d,%d)\r\n", #area, area.x1, area.y1, area.x2, area.y2);

#define HxLOG_DxEnumDump(type, e)	HxLOG_Print("\t%-20s : %s\r\n", #e, (DLIB_ConvertEnum2Str(type, e)));

void	HLIB_LOG_ErinPrint(const HCHAR *file, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv,HINT32 err_code, const HCHAR *format, ...);




#endif /* __HLIB_LOG_H__ */
