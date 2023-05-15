
/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2010 by HUMAX Inc.
 * 
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/

 

#ifndef MX_DBC_H__
#define MX_DBC_H__


#ifdef __cplusplus
extern "C" {
#endif


#include "mxkernel.h"
#include "mxtypes.h"
#include "mxutil.h"

//#################################################
// TYPE
//#################################################

#ifndef USE_SQLITE
#define USE_SQLITE
#endif


typedef  void *	MXDBC_HANDLE;
	
typedef enum _db_type
{
    SQLITE = 1,                // 일단은 sqlite만 지원한다
    MYSQL,
    ORACLE,
    MSSQL,  
}
db_type;


typedef enum tagEnumExecMemory
{
	SQL_NOFREE ,
	SQL_NEEDFREE
} EnumExecMemory;

typedef enum _db_integrity_check
{
	NON_CHECK = 0,		// integrity check skip
	INTEGRITY_CHECK		// integrity check 수행
}db_integrity_check;

#define MAX_MXDBC_DB_INFO_PATH_SIZE			1024
#define MAX_MXDBC_DB_INFO_NAME_SIZE			128
#define MAX_MXDBC_DB_INFO_ID_SIZE			24
#define MAX_MXDBC_DB_INFO_PASSWORD_SIZE		24

typedef struct db_info_tag
{
    db_type type;
    char db_path[MAX_MXDBC_DB_INFO_PATH_SIZE];
    char *db_tempPath;
//    char db_tempPath[1024];
    char db_name[MAX_MXDBC_DB_INFO_NAME_SIZE] ;
    char id[MAX_MXDBC_DB_INFO_ID_SIZE];
    char passwd[MAX_MXDBC_DB_INFO_PASSWORD_SIZE];    
	db_integrity_check eIntegrityCheck;
}MXDBC_DB_INFO;
//}db_info;



//#################################################






//#################################################
// API 
//#################################################
// 하나의 디비 파일에 대한 인스턴스를 유지
__MXAPI MX_INT32 mxDBC_Create( MXDBC_DB_INFO * infoData , MXDBC_HANDLE * ppDb_handle);

// 인스턴스 제거
__MXAPI MX_INT32 mxDBC_Destroy( MXDBC_HANDLE handle );

__MXAPI MX_INT32 mxDBC_BackupDB( MXDBC_HANDLE handle, char * path );
__MXAPI MX_INT32 mxDBC_OpenDB( MXDBC_DB_INFO * infoData , MXDBC_HANDLE * ppDb_handle );
__MXAPI MX_INT32 mxDBC_CloseDB( MXDBC_HANDLE handle );

__MXAPI void  mxDBC_FreeDBHandle(MXDBC_HANDLE handle , const long thdid );	// mxDBC_GetDBHandle()로 처음 생성된 쓰레드가 제거 되었을때 사용한다.

//#ifdef _WOON_VERSION_SUPPORT
//__MXAPI void *mxDBC_GetDBHandle(MXDBC_HANDLE handle, int *index);
//#endif

__MXAPI MX_INT32 mxDBC_Select( MXDBC_HANDLE handle , char *sql,  char ***pppResult, int *pnRow, int *pnCol, int size, int offset);

// 20120508 for sql memory free by jonghyun
__MXAPI MX_INT32 mxDBC_SelectEx( MXDBC_HANDLE handle , char *sql,  char ***pppResult, int *pnRow, int *pnCol, int size, int offset, EnumExecMemory free );

__MXAPI MX_INT32 mxDBC_Exec( MXDBC_HANDLE handle , char *sql , EnumExecMemory free );
__MXAPI MX_INT32 mxDBC_ReleaseResult(MXDBC_HANDLE handle , char **ppResult );

// 트랜잭션 처리
__MXAPI MX_INT32 mxDBC_BeginTrans(MXDBC_HANDLE handle) ;
__MXAPI MX_INT32 mxDBC_CommitTrans(MXDBC_HANDLE handle);
__MXAPI MX_INT32 mxDBC_RollbackTrans(MXDBC_HANDLE handle);
__MXAPI MX_INT32 mxDBC_IsTransaction(MXDBC_HANDLE handle);

__MXAPI MX_INT32 mxDBC_ShowErrorDlg(char *sql, char *szMsg, int errorCode);

__MXAPI int mxDBC_free( char * sql );
__MXAPI char * mxDBC_mprintf( const char * zFormat, ... );

// 20100107 Integrity Check
typedef enum _MXDBC_INTEGRITY
{
	INTEGRITY_SUCCESS = 1,
	INTEGRITY_FAIL,
	INTEGRITY_BUSY
}MXDBC_INTEGRITY;

__MXAPI MXDBC_INTEGRITY mxDBC_IntegrityCheck( MXDBC_HANDLE handle );
__MXAPI MXDBC_INTEGRITY mxDBC_QuickCheck( MXDBC_HANDLE handle );

__MXAPI MX_INT32 mxDBC_isDbErrFileExist( MXDBC_HANDLE handle );
//#################################################




 

#ifdef __cplusplus
}
#endif

#endif//MX_DBC_H__

