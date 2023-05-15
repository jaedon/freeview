

#ifndef __MXFILE_H__
#define __MXFILE_H__

#include "mxlib_config.h"
#include "mxtypes.h"
#include "mxlog.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIMESEEK_SUPPORT

#define MXFILE_ACCESS_NONE		0
#define MXFILE_ACCESS_FILE		1
#define MXFILE_ACCESS_HTTP		2
#define MXFILE_ACCESS_WMSP		3
#define MXFILE_ACCESS_RTSP		4
#define MXFILE_ACCESS_MMS		5

#define MXFILE_ACCESS_USER		10

typedef struct mxfile_cancel_tag
{
	unsigned char cancel_code;

// --------------------------------------------------------//
} MXFILE_CANCEL_OP  ;
// --------------------------------------------------------//

__MXAPI void mxFile_resetCancel( MXFILE_CANCEL_OP *p );
__MXAPI void mxFile_setCancel( MXFILE_CANCEL_OP *p );
__MXAPI MX_INT32 mxFile_isCanceled( MXFILE_CANCEL_OP *p );


// 8 bit : general file
// 8 bit : http
// 8 bit : wmsp

// http open 을 천천히 해라 , open 후 header 를 변경하기 위해
#define MXFILEHTTP_DESC_OPTION_DEFER_OPEN	0x0010
#define MXFILEHTTP_DESC_OPTION_DTCP			0x0020
#define MXFILEHTTP_DESC_OPTION_WRITE		0x0040
#define MXFILEHTTP_DESC_OPTION_TIME			0x0080

// yhkang added 20120330. kt upload need file write.
#define MXFILEFILE_DESC_OPTION_WRITE		0x0004
#define MXFILEFILE_DESC_OPTION_APPEND		0x0008


// video  인지 audio 인지 알려준다.
#define MXFILEWMSP_DESC_OPTION_HEADER		0x0100
#define MXFILERTSP_DESC_OPTION_HEADER		0x1000

#ifdef DLNAQOS
enum TransferMode{
	transfer_unknown,
	transfer_interactive,
	transfer_stream ,
	transfer_background
};
#define DLNAQOS_0_DSCP 0x20
#define DLNAQOS_1_DSCP 0x00
#define DLNAQOS_2_DSCP 0xa0
#define DLNAQOS_3_DSCP 0xe0
#endif

typedef struct mxfile_desc_tag
{
	char *url;   // read only 로만 쓰시오

	MX_INT32 access_type;
	MX_INT32 linear_access;
	MXFILE_CANCEL_OP *cancel_op;
	long	option;
	MX_UINT8 user_data[32];
#ifdef DLNAQOS
	int nTransferMode;
#endif
// --------------------------------------------------------//
} MXFILE_DESC;
// --------------------------------------------------------//

typedef void (*MXFILE_HOOK_URL_API) ( MXFILE_DESC *desc );

typedef struct mxfile_init_option_tag
{
	MXFILE_HOOK_URL_API hook_url;
// --------------------------------------------------------//
} MXFILE_INIT_OPTION;
// --------------------------------------------------------//

typedef struct mxfile_handle_tag
{
	void *apis;
	MXFILE_DESC desc;
	MX_INT32 file_type;


	MXFILE_CANCEL_OP static_cancel_op;
	MXFILE_CANCEL_OP *cancel_op;
	MX_INT32 error;
	MX_INT32 eof;
	MX_INT32 linear_access;
	MX_INT64 filesize;
	void *extra_data;

#ifdef TIMESEEK_SUPPORT
	int flag_timeseek;
#endif

// --------------------------------------------------------//
} *MXFILE_HANDLE , MXFILE_HANDLE_STRUCT ;
// --------------------------------------------------------//


typedef struct mxfile_api_set_tag
{
	MX_INT32 (*open) ( MXFILE_HANDLE handle , MXFILE_DESC *desc, const char *clientIP );
	MX_INT32 (*read) ( MXFILE_HANDLE handle , void *buffer , MX_UINT32 buffer_size , MX_UINT32 *read_size );
	void (*close)( MXFILE_HANDLE handle );
	MX_INT32 (*seek) ( MXFILE_HANDLE handle , MX_INT64	offset, MX_INT32 	origin );
	MX_INT32 (*tell) ( MXFILE_HANDLE handle , MX_UINT64 *pCurFilePos );
	MX_INT32 (*get_filesize)(MXFILE_HANDLE handle , MX_INT64 *	pFileSize);
	MX_INT32 (*is_eof) ( MXFILE_HANDLE handle );
	MX_INT32 (*write) ( MXFILE_HANDLE handle , void *buffer , MX_UINT32 buffer_size , MX_UINT32 *write_size );
// --------------------------------------------------------//
} MXFILE_API_SET , *MXFILE_API_SET_PTR;
// --------------------------------------------------------//


typedef MXFILE_API_SET_PTR (*check_url )( MXFILE_DESC *file );



#define MXFILE_OK				0
#define MXFILE_ERROR			-1
#define	MXFILE_ERROR_EOF		-2
#define MXFILE_ERROR_CANCEL		-3



__MXAPI void mxFile_init( MXFILE_INIT_OPTION *option );
__MXAPI void mxFile_deinit( void );
__MXAPI MX_INT32 mxFile_register( check_url  func );
__MXAPI MX_INT32 mxFile_unregister(  check_url  func  );

__MXAPI MX_INT32 mxFile_openFile( MXFILE_DESC *desc , MXFILE_HANDLE *, const char *clientIP);

__MXAPI MX_INT32 mxFile_closeFile(
	MXFILE_HANDLE	fileHandle		// file handle
);

__MXAPI MX_INT32 mxFile_readFile(
	MXFILE_HANDLE	fileHandle,	// file handle
	void * 	pBuffer, 	// payload buffer
	MX_UINT32	bufSize,	// amount to read
	MX_UINT32 *	pReadSize	// bytes read
);

__MXAPI MX_INT32 mxFile_writeFile(
	MXFILE_HANDLE	fileHandle,	// file handle
	void * 	pBuffer, 	// payload buffer
	MX_UINT32	bufSize,	// amount to read
	MX_UINT32 *	pWriteSize	// bytes read
);
// -1 은 무한대인 경우 또는 모르는 경우

__MXAPI MX_INT32	mxFile_getFilesize(
	MXFILE_HANDLE	fileHandle,
	MX_INT64 *	pFileSize
);


__MXAPI MX_INT32	mxFile_abortFileOperation(
	MXFILE_HANDLE 	fileHandle
);

#define MXFILE_SEEK_SET			0
#define MXFILE_SEEK_CUR			1
#define MXFILE_SEEK_END			2

__MXAPI MX_INT32	mxFile_seekFile(
	MXFILE_HANDLE	fileHandle,	// file handle
	MX_INT64	offset, 	// byte offset to seek
	MX_INT32 	origin		// SEEK_CUR, SEEK_END, SEEK_SET
);

__MXAPI MX_INT32 mxFile_tellFile(
	MXFILE_HANDLE	fileHandle,
	MX_UINT64 *	pCurFilePos
);

__MXAPI MX_INT32 mxFile_isEndOfFile(
	MXFILE_HANDLE	fileHandle
);

__MXAPI MX_INT32 mxFile_isErrorOfFile(
	MXFILE_HANDLE	fileHandle
);



#define MXFILE_GS_IS_LINEAR_ACCESS		0

__MXAPI MX_INT32 mxFile_getStatus( MXFILE_HANDLE	fileHandle , MX_INT32 prop );


__MXAPI int mxFile_ClearDtcpAke(void);

#ifdef DTCP_SUPPORT
#ifdef _WIN32
#pragma  comment(lib, "mxdtcp.lib")
#endif
#endif



// for debugging


int mxFileCheck_checkMemory(void);

#ifdef TIMESEEK_SUPPORT
__MXAPI MX_INT32 mxFile_openTime( MXFILE_DESC *desc , MXFILE_HANDLE *fileHandle);
__MXAPI MX_INT32 mxFile_closeTime( MXFILE_HANDLE	fileHandle );
__MXAPI MX_INT32 mxFile_readTime( MXFILE_HANDLE	fileHandle,	void * 	pBuffer, MX_UINT32	bufSize, MX_UINT32 *	pReadSize );
__MXAPI MX_INT32 mxFile_getTotalTime( MXFILE_HANDLE	fileHandle,	MX_INT64 *	pFileSize);
__MXAPI MX_INT32 mxFile_seekTime(	MXFILE_HANDLE	fileHandle,	MX_INT64 time );
__MXAPI MX_INT32 mxFile_tellTime(	MXFILE_HANDLE	fileHandle,	MX_UINT64 *	pCurFilePos );
__MXAPI MX_INT32 mxFile_isEndOfTime(MXFILE_HANDLE	fileHandle);
__MXAPI MX_INT32 mxFile_isErrorOfTime(	MXFILE_HANDLE	fileHandle );
__MXAPI MX_INT32 mxFile_abortTimeOperation(	MXFILE_HANDLE 	fileHandle) ;
#endif // TIMESEEK_SUPPORT




#ifdef __cplusplus
}
#endif


#endif //__MXFILE_H__

