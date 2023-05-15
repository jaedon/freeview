


#ifndef __MXLOG_H__
#define __MXLOG_H__

#include "mxapi_define.h"


#ifdef __cplusplus
extern "C"
{
#endif

#include "mxtypes.h"

#define MX_LEVEL_KNOWN		-2
#define MX_LEVEL_NONE		-1
#define MX_LEVEL_ERROR		0
#define MX_LEVEL_APP		1
#define MX_LEVEL_WARNING	2
#define MX_LEVEL_INFO		3
#define MX_LEVEL_MORE		4

#define MX_MASK_ALL			(0xFFFFFF00)
#define MX_MASK_LEVEL		(0x000000FF)

typedef struct _MXLOG_FILE_PROP
{
#ifdef OPEN_FILENAME_UNICODE
	wchar_t * prevLogPath;
#else
	char * prevLogPath;
#endif
	int max_saved_line;

}MXLOG_FILE_PROP;

typedef void (*LOG_CALLBACK)(const char *filename, const char *fucntion, int line, MX_INT32 num, const char *str);


//__MXAPI void mxLog_init( MX_UINT32 level );
__MXAPI void mxLog_init( MX_INT32 level , MXLOG_FILE_PROP * fileProp, LOG_CALLBACK fn );
__MXAPI void mxLog_deinit(void);
__MXAPI void mxLog_setLevel( MX_UINT32 level );
__MXAPI MX_UINT32  mxLog_getLevel(void);
#define mxLog_log mxlog

#ifdef _ANDROID
#ifdef _ANDROID_LOG
#include <jni.h>
#include <android/log.h>
#define LOG_TAG "mxlib" // text for log tag
#endif
#endif


// 다음을 아래와 같이 변경합니다.
//__MXAPI
//void mxlog( MX_INT32 num , const char *fmt , ... );

__MXAPI
void mxlogEx( const char *filename, const char *fucntion, int line, MX_INT32 num, const char *fmt , ... );
#define	mxlog_NullPrint					while (0) ((int (*)(const char *, ...)) 0)

#if defined(CONFIG_DEBUG)
#define CONSOLE_LOG
#endif

#if defined(CONSOLE_LOG)
#define mxlog(num, fmt, ...)			mxlogEx( __FILE__, __FUNCTION__, __LINE__, num, fmt,##__VA_ARGS__)
#define MX_FUNCTION_PRINT()				mxlogEx( NULL, NULL, 0, MX_LEVEL_ERROR, "[%s %05d]\n", __FUNCTION__, __LINE__ )
#else
#define mxlog(num, fmt, ...)			mxlog_NullPrint
#define MX_FUNCTION_PRINT()				mxlog_NullPrint
#endif


#ifdef __cplusplus
}
#endif

#endif	// __MXLOG_H__

