


#ifndef __MXUTIL_H__
#define __MXUTIL_H__
#include "mxlib_config.h"
#include "mxapi_define.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "mxlog.h"

#define MXDLNA_SYSTEM_STRING			void *

enum MXDLNA_FILE_OR_DIR_TYPE
{
	MXDLNA_FILEDIR_TYPE_NOT_FOUND_ERROR	= 0,
	MXDLNA_FILEDIR_TYPE_FILE			= 1,
	MXDLNA_FILEDIR_TYPE_DIRECTORY		= 2
};

__MXAPI MX_INT64 mxUtil_ATOI64( const char * string );		// input ascii, utf8
__MXAPI int mxUtil_IsUtf8(const char* str);
__MXAPI int mxUtil_correctionUtf8(char* str);
__MXAPI void  * mxUtil_getStaticBuffer( MX_UINT32 size );


__MXAPI enum MXDLNA_FILE_OR_DIR_TYPE mxUtil_getFileDirType(void* path);
__MXAPI enum MXDLNA_FILE_OR_DIR_TYPE mxUtil_getFastFileDirType(void *handle, const char* dirName, const char* filename);
__MXAPI long mxUtil_getFileSize(char *FileName);
__MXAPI void * mxUtil_getDirFirstFile(const char* directory, /*INOUT*/ char* filename, int filenamelength, /*INOUT*/ int* filesize, void *fd);
__MXAPI int mxUtil_getDirNextFile(void* handle, const char* dirName, char* filename, int filenamelength, int* filesize);
__MXAPI void mxUtil_closeDir(void* handle);
__MXAPI int mxUtil_createDir(void *path);
__MXAPI int mxUtil_createDirAdvance(void *path, int mode);		// 20100914 linux에서 폴더 만들때 권한 줄 수 있게 수정
__MXAPI int mxUtil_moveFile(const void *OldFileName, const void *NewFileName);
__MXAPI int mxUtil_moveFile_utf8(const char *OldFileName, const char *NewFileName);
__MXAPI int mxUtil_deleteFileFromDisk(void *FileName);
__MXAPI int mxUtil_deleteFileFromDisk_utf8(char *FileName);
__MXAPI MX_UINT64 mxUtil_getTickCount(void);
__MXAPI char *mxUtil_currentTimeMakeStringGMT(void);
__MXAPI int mxutil_snprintf( char *buffer, int length, const char *fmt , ... );
__MXAPI void *mxKernel_malloc(unsigned int _Size);
__MXAPI void mxKernel_free(void *_Memory);
__MXAPI char* mxKernel_strdup(const char* str);
__MXAPI void *mxKernel_realloc(void *ptr, size_t size);
__MXAPI void *mxKernel_calloc(size_t nmemb, size_t size);


#if defined _WIN32 && defined _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


//#define _MX_MALLOC_TEST
#ifdef _MX_MALLOC_TEST

	#define _MEMORY_TEST

	#define MX_MALLOC		MX_TEST_MALLOC
	#define MX_FREE			MX_TEST_FREE
	#define MX_REALLOC		MX_TEST_REALLOC
	#define MX_CALLOC		MX_TEST_CALLOC
	#define MX_FREE_NULL	MX_TEST_FREE_NULL
	#define MX_STRDUP		MX_TEST_STRDUP

	#define MX_STRNCPY	strncpy
	#define MX_STRNCAT	strncat
	#define MX_STRNCMP	strncmp

		#ifdef WIN32

			#define MX_SNPRINTF(buffer, cnt, fmt, ...)			mxutil_snprintf( buffer, cnt, fmt,##__VA_ARGS__)
//			#define MX_SNPRINTF	_snprintf
			#define MX_STRDUP	_strdup
		#else
			#define MX_SNPRINTF(buffer, cnt, fmt, ...)			mxutil_snprintf( buffer, cnt, fmt,##__VA_ARGS__)
//			#define MX_SNPRINTF	snprintf
			#define MX_STRDUP	strdup
		#endif

	#define MX_STRDUP_CHECK_INNULL(a) 	(a == NULL) ? NULL : MX_STRDUP(a);
#else

	#define MX_MALLOC					mxKernel_malloc
	#define MX_FREE						mxKernel_free
	#define MX_STRDUP					mxKernel_strdup
	#define MX_REALLOC					mxKernel_realloc
	#define MX_CALLOC					mxKernel_calloc

	#define MX_MALLOC_DEFAULT			malloc
	#define MX_FREE_DEFAULT				free
	#define MX_STRDUP_DEFAULT			strdup

	#define MX_FREE_NULL(a) if(a != NULL) { MX_FREE(a); a = NULL; }

	#define MX_STRNCPY	strncpy
	#define MX_STRNCAT	strncat
	#define MX_STRNCMP	strncmp

	#ifdef _WIN32_WCE
		#include <stdlib.h>
		#define	MX_STRDUP	_strdup
	#else
		#ifdef WIN32
			#define MX_SNPRINTF(buffer,cnt,fmt,...)			mxutil_snprintf(buffer,cnt,fmt,##__VA_ARGS__)
		#else
			#define MX_SNPRINTF(buffer,cnt,fmt,...)			mxutil_snprintf(buffer,cnt,fmt,##__VA_ARGS__)
		#endif
	#endif

	#define MX_STRDUP_CHECK_INNULL(a) 	(a == NULL) ? NULL : MX_STRDUP(a);
#endif


#ifdef _MEMORY_TEST
	void *my_malloc(int size, char * callWhere , int line);
	void *my_realloc(void *from, int size , char * callWhere, int line);
	void *my_calloc(int size, int c, char * callWhere , int line);
	void my_free(void *from,char * callWhere , int line);
	char *my_strdup(const char *from, char * callWhere , int line);

	void PRINT_CMEMINFO(void);
	void PRINT_CMEM_SNAPSHOT(void);

	#define MX_TEST_MALLOC(a)		my_malloc(a,__FILE__,__LINE__)
	#define MX_TEST_FREE(a)			my_free(a,__FILE__,__LINE__)
	#define MX_TEST_FREE_NULL(a)		if(a != NULL) { my_free(a,__FILE__,__LINE__); a = NULL; }
	#define MX_TEST_REALLOC(p, a)	my_realloc(p, a,__FILE__,__LINE__)
	#define MX_TEST_CALLOC(n, c)		my_calloc(n, c,__FILE__,__LINE__)
	#define MX_TEST_STRDUP(s)		my_strdup(s,__FILE__,__LINE__)

#else
	#define PRINT_CMEMINFO()
	#define PRINT_CMEM_SNAPSHOT()
#endif


__MXAPI int mxUtil_GetMacAddress(char * macStr);
// 20101126 whiterub
__MXAPI int mxUtil_GetFileSystemUsage(
	const char * pszDrive,
	MX_UINT64 * pTotalByte,
	MX_UINT64 * pUseByte,
	MX_UINT64 * pFreeByte
);


typedef void* (*MXKERNEL_MALLOC_CALLBACK)	(unsigned int _Size);
typedef void  (*MXKERNEL_FREE_CALLBACK)		(void *_Memory);
typedef char* (*MXKERNEL_STRDUP_CALLBACK)	(char *_str);
typedef void* (*MXKERNEL_CALLOC_CALLBACK)	(size_t nmemb, size_t size);
typedef void* (*MXKERNEL_REALLOC_CALLBACK)	(void *ptr, size_t size);

typedef struct _MXKERNEL_CONFIGURE_DATA
{
	MXKERNEL_MALLOC_CALLBACK	cb_malloc;
	MXKERNEL_FREE_CALLBACK		cb_free;
	MXKERNEL_STRDUP_CALLBACK	cb_strdup;
	MXKERNEL_CALLOC_CALLBACK	cb_calloc;
	MXKERNEL_REALLOC_CALLBACK	cb_realloc;
} MXKERNEL_CONFIGURE_DATA, *MXKERNEL_CONFIGURE_DATA_HANDLE;

__MXAPI void mxKernel_configure(MXKERNEL_CONFIGURE_DATA_HANDLE handle);
__MXAPI void mxKernel_unconfigure(void);



#ifdef __cplusplus
}
#endif

#define MX_MIN(left, right)  (left < right ? left : right)

#endif	// __MXUTIL_H__

