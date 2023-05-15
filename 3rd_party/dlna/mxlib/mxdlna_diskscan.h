
/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2010 by HUMAX Inc.
 *
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/






#ifndef MXDMS_SCAN_H__
#define MXDMS_SCAN_H__


#include "mxapi_define.h"


#ifdef __cplusplus
extern "C"{
#endif

// ***********************************************************************
/**	\brief
// DiskScan의 제약은 mxdbc가 존재 해야 한다
// 내부의 filefind만 사용하고 싶다면 해당 파일만 참조해서 사용해라.
*/
// ***********************************************************************




//--------------------------------------
// include
//--------------------------------------
#include "mxkernel.h"
#include "mxtypes.h"
#include "mxdbc.h"
#include "mxdlna_dms.h"
//--------------------------------------




//--------------------------------------
// type
//--------------------------------------

typedef void * MXDLNA_FSS_HANDLE;

// FileScanner의 상태
typedef enum _MXDMS_SCANFILTER_STATUS
{
	FSS_NOT_INIT,			// 초기화 되지 않았다
	FSS_SCANNING,			// 스캐닝 중이다
	FSS_ONESHOT_SCANNING,	// 한번만 전체 스캔하는 스캐닝 중이다
	FSS_PAUSE_SCANNING,		// 스캐닝이 일시 정지된 상태
	FSS_IDLE,				// 휴식 상태이다
	FSS_STOPPED ,			// 멈춰 있다
	FSS_DIRECT				// direct 스캔중이다
}MXDMS_SCANFILTER_STATUS;


typedef enum _MXDMS_SCANFILTER_ERR
{
	MXDMS_SCANFILTER_INIT = 0,
	MXDMS_SCANFILTER_ERROR = -1,
	MXDMS_SCANFILTER_NOPROCESS = -5,
	MXDMS_SCANFILTER_UNKNOWNFILE = -10,
}MXDMS_SCANFILTER_ERR;


// 20100805
typedef enum _MXDMS_SCAN_INFO
{
	MXDMS_SCAN_INFO_NONE = 0,
	MXDMS_SCAN_INFO_ADDDIR_LIST_PATH_MATCHED,
	MXDMS_SCAN_INFO_ADDDIR_LIST_EXACT_MATCHED,
	MXDMS_SCAN_INFO_ADDDIR_SUBPATH_RELATION,
	MXDMS_SCAN_INFO_ADDDIR_INCLUDEPATH_RELATION,
	MXDMS_SCAN_INFO_ADDDIR_NOT_MATCHED
}MXDMS_SCAN_INFO;

typedef MX_INT32 (*MXDLNA_DMS_PREPROCESS_SCANFILTER) (void);

typedef MX_INT32 (*MXDLNA_DMS_POSTPROCESS_SCANFILTER) (void);

// 다른 헤더파일로 옮겨야 할 것 같다.
// diskScan을 통해 시스템이 CDS에 추가할 아이템이 있는 경우 사용한다
// 사용자가 세부적인 값을 설정하여 directoryBuilder안의 mxDlnaDmsSfm_addCdsItem() 함수를 사용하여 CDS에 추가한다.
typedef MX_INT32 (*MXDLNA_DMS_ADD_SCANFILTER) (
	MXDLNA_DMS_HANDLE  dmsHandle,
	DMS_DB_HANDLE db_h,
	char * location ,
	char * filename ,
	char * scanningPath
);


// diskScan을 통해 CDS에 있는 object를 제거하는 경우 사용한다
// 사용자가 세부적인 값을 설정하여 directoryBuilder안의 mxDlnaDmsSfm_removeCdsItem() 함수를 사용하여 CDS에서 제거한다.
typedef MX_INT32 (*MXDLNA_DMS_REMOVE_SCANFILTER) (
	MXDLNA_DMS_HANDLE dmsHandle ,
	DMS_DB_HANDLE h_db,
	char * location
);

typedef MX_INT32 (*MXDLNA_DMS_ADD_DETACHABLE_STORAGE) (
	DMS_DB_HANDLE hDB,
	const char* path,
	const char* parentID
);

// 20100908
typedef int (*MXDLNA_DMS_FOLDER_ADD) (
	void * userData,
	int folderCount,
	const char * callFolderName	,
	int wd
);

typedef enum _MXDMS_SCAN_PATH_OPTION
{
	MXDMS_SCAN_PATH_NORMAL = 0,					// 이 option으로 추가된 directory는 모두 Main DB에서 관리됨
	MXDMS_SCAN_PATH_DETACHABLE_ROOT,			// 이 option으로 추가된 directory에 multi DB가 생성 됨
	MXDMS_SCAN_PATH_DETACHABLE_SUB				// .recordings 와 같이 별도로 추가해야 하는 directory의 경우 본 option을 줌
												// MXDMS_SCAN_PATH_DETACHABLE_ROOT 로 추가된 directory에 종속관계이어야 함
}MXDMS_SCAN_PATH_OPTION;

typedef struct _MXDLNA_FSS_SCANDATA
{
	char * path;		// 스캔할 경로
	char * filter;		// 확장자 필터
	char * userData;	// 사용자 데이터
	int    depth;		// 하위 directory scan 여부, [0:1] 0=하위 directory scan, 1=해당 directory 만 scan
	MXDMS_SCAN_PATH_OPTION    option;
}MXDLNA_FSS_SCANDATA;


typedef struct _MXDLNA_FSS_REMOVEDIRSTATUS
{
	int totalCount;			// 삭제할 파일 및 폴더의 개수
	int currentCount;		// 현재 삭제한 아이템 및 폴더의 개수
	int isCancel;			// 취소 여부 설정

	int isEnd;				// 끝났는지 여부를 설정. 끝나면 1
	int doEvent;			// systemUpdateID, ContainerUpdateID 처리 유무
							// 1이면 처리
}MXDLNA_FSS_REMOVEDIRSTATUS;


typedef struct _scanCount
{
	int totalCount;			// file & folder total
	int filteredTotalCount;			// file & folder total
	int folderCount; //yhkang added 20120312

	int scan_new_file;		// new find file
	int scan_new_folder;	// new find folder
	int insert_file;		// filtered file
	int insert_folder;		// filtered folder

	int scan_del_file;		// del find file
	int scan_del_folder;	// del find folder
	int remove_file;		// db removed file
	int remove_folder;		// db removed folder
	int totalRemoveCount;	// 20120806 for calculate total Remove Items(Files and Folders)
}SCAN_COUNT;



// MXDLNA_FSS_SCANDATA의 리스트이다.
// FileScan할 때 리스트에 있는 모든 경로를 스캔한다.
typedef struct _MXDLNA_FSS_SCANDATA_LIST
{
  MXDLNA_FSS_SCANDATA value;
  SCAN_COUNT scanCount;
  struct _MXDLNA_FSS_SCANDATA_LIST * pNext;
}MXDLNA_FSS_SCANDATA_LIST;

typedef struct _MXDLNA_FSS_DETACHABLE_SCANDATA_LIST
{
  MXDLNA_FSS_SCANDATA value;
  SCAN_COUNT scanCount;
  MXDBC_HANDLE exDBHandle;
  int refCnt;
  int dbID;
  struct _MXDLNA_FSS_DETACHABLE_SCANDATA_LIST * pNext;
}MXDLNA_FSS_DETACHABLE_SCANDATA_LIST;

#ifdef _POSIX
#define MAX_PATH          260
#endif


#ifdef DMS_DISKSCAN_WORKING_QUEUE
typedef MX_INT32 (*MXDLNA_DMS_COMPLETED) (
	MXDLNA_DMS_HANDLE dmsHandle ,
	DMS_DB_HANDLE h_db,
	char * location
);
#endif




typedef struct _MXDLNA_FSS_SCANOPTION
{
	MX_INT32 rescan_interval_ms;			// 다시 스캔할 때까지 기다리는 시간 (ms);
	MX_INT32 per_interval_ms;				// 하나씩 스캔할 때까지 기다리는 시간 (ms);
	MX_INT32 bScanningTimePrint;			// 스캔하는 시간을 보여줄지 유무

	MXDLNA_DMS_PREPROCESS_SCANFILTER	fnScanFilter_preProcess;
	MXDLNA_DMS_POSTPROCESS_SCANFILTER	fnScanFilter_postProcess;
	MXDLNA_DMS_ADD_SCANFILTER			fnScanFilter_addEvent;			// 새로운 아이템을 찾았을때 처리할 callback
	MXDLNA_DMS_REMOVE_SCANFILTER		fnScanFilter_removeEvent;		// 아이템을 제거할때 처리할 callback
	MXDLNA_DMS_ADD_DETACHABLE_STORAGE	fnScanFilter_addDetachableStorage;

	MX_INT32 scan_maxcount;                 // oneshot scan등 한번 스캔할때 최대 몇개까지하는가, 0은 무한대, 예> 100은 100개의 아이템만 스캔하여추가함

#ifdef DMS_DISKSCAN_WORKING_QUEUE
	MXDLNA_DMS_COMPLETED	fnScanCompletedEvent;
#endif


} __attribute__((packed)) MXDLNA_FSS_SCANOPTION;	// If packed attribute is not used, fnScanFilter_addDetachableStorage field may be optimized out in another file not referencing.



//--------------------------------------








//--------------------------------------
// API
//--------------------------------------
// 1. AddEven 2. RemoveEvent

/**	\brief	The mxDlnaFileScanner_create function
	\brief	파일 스캐너를 생성하는 함수 이다.
	\brief	파일 스캐너를 끝낼 때는 mxDlnaFileScanner_destroy() 함수를 호출한다.

	\param	ppHandle	: 함수 내부에서 메모리를 할당하여 넘겨 준다.
	\param	pOption	: MXDLNA_FSS_SCANOPTION * 타입을 이용해서 파일 스캐너의 옵션을 설정한다
	\param	hDb	: db_handle * 타입으로 db의 핸들을 입력한다.
	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_create(
	MXDLNA_FSS_HANDLE * ppHandle,
	MXDLNA_FSS_SCANOPTION * pOption,
	MXDBC_HANDLE hDb,	// Thread & DB
	MXDLNA_DMS_HANDLE dmsHandle
);


/**	\brief	The mxDlnaFileScanner_destroy function
	\brief	파일 스캐너를 제거하는 함수 이다.
	\brief	mxDlnaFileScanner_create() 함수에서 생성된 데이터를 입력한다
	\param	pHandle	: void * 타입으로 mxDlnaFileScanner_create() 함수에서 생성된다

	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_destroy(
	MXDLNA_FSS_HANDLE pHandle
);



/**	\brief	The mxDlnaFileScanner_setInterval function
	\brief	스캔중에 휴식시간을 설정하는 함수이다.

	\param	pHandle	: mxDlnaFileScanner_create() 함수에서 생성된 데이터를 입력한다.
	\param	interval_ms	: 정수형 값으로 ms 단위로 휴식시간을 입력한다.

	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_setInterval(
	MXDLNA_FSS_HANDLE pHandle ,
	MX_INT32 interval_ms,
	MX_INT32 item_interval_ms
);

// 20101005 -
__MXAPI MX_INT32 mxDlnaFileScanner_getInterval(
	MXDLNA_FSS_HANDLE pHandle ,
	MX_INT32 * pInterval_ms,
	MX_INT32 * pItem_interval_ms
	);




/**	\brief	The mxDlnaFileScanner_addDirectory function
	\brief	스캔할 경로를 추가하는 함수 이다.
	\brief	단 스캐닝 중에는 추가 할수 없다.
	\param	pHandle	: mxDlnaFileScanner_create() 함수에서 생성된 데이터를 입력한다.
	\param	pAddData  	: MXDLNA_FSS_SCANDATA * 타입의 데이터를 입력한다
	\return	MX_INT32
*/
// 20100913
__MXAPI MX_INT32 mxDlnaFileScanner_addDirectory(
	MXDLNA_FSS_HANDLE pHandle ,
	MXDLNA_FSS_SCANDATA * pAddData,
	MXDMS_SCAN_INFO * pMoreInfo
);


/**	\brief	The mxDlnaFileScanner_removeDirectory function
	\brief	mxDlnaFileScanner_getDirectories() 함수로 확인
	\param	pHandle	: mxDlnaFileScanner_create() 함수에서 생성된 데이터를 입력한다.
	\param	pAddData	: MXDLNA_FSS_SCANDATA * 타입의 데이터로 path 값을 위주로 본다.
	\return	MX_INT32
*/
// 20100913
__MXAPI MX_INT32 mxDlnaFileScanner_removeDirectory(
	MXDLNA_FSS_HANDLE pHandle ,
	MXDLNA_FSS_SCANDATA * pAddData,
	MXDLNA_FSS_REMOVEDIRSTATUS * pRemoveProp
);


/**	\brief	The mxDlnaFileScanner_getDirectories function
	\brief	스캐닝하는 디렉토리의 목록이다.
	\param	pHandle	: mxDlnaFileScanner_create() 함수에서 생성된 데이터를 입력한다.
	\return	MXDLNA_FSS_SCANDATA_LIST *
*/
__MXAPI MXDLNA_FSS_SCANDATA_LIST * mxDlnaFileScanner_getDirectories(
	MXDLNA_FSS_HANDLE pHandle
);

__MXAPI MXDLNA_FSS_DETACHABLE_SCANDATA_LIST * mxDlnaFileScanner_getDetachableDirectories(
	void
);

// 스캐닝 하는 디렉토리 목록의 개수 이다. getDirectories 함수와 같이 사용하면 된다.
__MXAPI MX_INT32 mxDlnaFileScanner_scanListCount(
	MXDLNA_FSS_HANDLE pHandle
);

/**	\brief	The mxDlnaFileScanner_startScan function
	\brief	스캔을 시작한다. 이미 시작되었으면 별다른 처리를 하지 않는다.
	\param	pHandle	: mxDlnaFileScanner_create() 함수에서 생성된 데이터를 입력한다.
	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_startScan(
	MXDLNA_FSS_HANDLE pHandle ,
	int isOneShot
);



/**	\brief	The mxDlnaFileScanner_stopScan function
	\brief	스캔을 정지한다. 이미 정지 되었으면 별다른 처리를 하지 않는다.
	\param	pHandle	: mxDlnaFileScanner_create() 함수에서 생성된 데이터를 입력한다.
	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_stopScan(
	MXDLNA_FSS_HANDLE pHandle
);




/**	\brief	The mxDlnaFileScanner_getStatus function
	\brief	현재 파일 스캐너의 상태를 나타낸다. 상태는 FSS_NOT_INIT, FSS_SCANNING... 등등의 정보가 될 수 있다
	\param	pHandle	: mxDlnaFileScanner_create() 함수에서 생성된 데이터를 입력한다.
	\return	MX_INT32
*/
__MXAPI MXDMS_SCANFILTER_STATUS mxDlnaFileScanner_getStatus(
	MXDLNA_FSS_HANDLE pHandle
);	// 스캔중인지를 묻는 루틴



__MXAPI SCAN_COUNT * mxDlnaFileScanner_getScanCount(
	MXDLNA_FSS_HANDLE pHandle
);


/**	\brief	The mxDlnaFileScanner_directScanStart function
	\brief	직접 스캔하는 함수로서 index에 해당하는 경로만 검색할 수 있다.
	\brief	대소문자를 구별하므로 주의해야 한다
	\brief	상대 경로를 정해야 하기 때문이다.
	\param	pHandle	: mxDlnaFileScanner_create() 함수에서 생성된 데이터를 입력한다.
	\param	pData	: MXDLNA_FSS_SCANDATA *
	\param	index	: mxDlnaFileScanner_getStatus()를 통해 얻어오는 순서
	\param	depth	: 1이면 아이템 하나만 확인 , -1 이면 전체 스캔이다.

	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_directScanStart(
	MXDLNA_FSS_HANDLE pHandle ,
	MXDLNA_FSS_SCANDATA * pData ,
	int index ,
	int depth
);



/**	\brief	The mxDlnaFileScanner_allocDirData function
	\brief	MXDLNA_FSS_SCANDATA 데이터를 쉽게 할당 하기 위한 함수 이다.
	\brief	mxDlnaFileScanner_freeDirData() 함수로 메모리를 해제한다.
	\param	ppData	: MXDLNA_FSS_SCANDATA 타입으로 메모리가 할당 되어 리턴된다.
	\param	filter	: 확장자 필터
	\param	path	: 검색할 Path
	\param	userData	: Not Yet Use
	\return	int
*/
__MXAPI int mxDlnaFileScanner_allocDirData(
	MXDLNA_FSS_SCANDATA ** ppData ,
	char * filter ,
	char * path ,
	char * userData
);


/**	\brief	The mxDlnaFileScanner_freeDirData function
	\brief	mxDlnaFileScanner_allocDirData() 할당된 데이터의 메모리를 해제한다.
	\param	pData	: MXDLNA_FSS_SCANDATA * 타입으로 mxDlnaFileScanner_allocDirData() 함수로 할당된 값이다.
	\return	int
*/
__MXAPI int mxDlnaFileScanner_freeDirData(
	MXDLNA_FSS_SCANDATA * pData
);
//--------------------------------------


__MXAPI MX_INT32 mxDlnaFileScanner_existedFile(
	char * path ,
	char * findFilename
);


// scanList 상에서 몇번째 인덱스에 존재하는지 알기 위한 함수
__MXAPI MX_INT32 mxDlnaFileScanner_getMatchedScanIndex(
	MXDLNA_FSS_HANDLE pHandle ,
	const char * path
);

__MXAPI const MXDLNA_FSS_SCANDATA * mxDlnaFileScanner_getMatchedScanList(
	MXDLNA_FSS_HANDLE pHandle ,
	const char * filepath ,
	int * index
);


// 스캔을 시작하기 전에 부른다.. TotalCount를계산할지 말지 결정한다.
__MXAPI MX_INT32 mxDlnaFileScanner_setTotalCountCalc(
	MXDLNA_FSS_HANDLE pHandle ,
	int isSet
);

__MXAPI MX_INT32 mxDlnaFileScanner_isFilteredItem(
	const char * path ,
	char * filter
);



__MXAPI MX_INT32 mxDlnaFileScanner_directRemoveFile(
	MXDLNA_FSS_HANDLE pHandle ,
	char* path ,
	char* filename
);

__MXAPI int mxDlnaFileScanner_getChildDirs(
	void* userData,
	char * in_path ,
	int in_sleepTime,
	int * in_isCancel,
	int * out_folderCount,
	MXDLNA_DMS_FOLDER_ADD fFolderAdd
);

#ifdef DMS_DISKSCAN_WORKING_QUEUE
__MXAPI void mxDlnaFileScanner_requestScanning(
	MXDLNA_FSS_HANDLE pScan,
	int actionType,
	char * scanPath ,
	char * filename,
	int depth
);
#endif

#ifdef __cplusplus
}
#endif

#endif //MXDMS_SCAN_H__

