
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
// DiskScan�� ������ mxdbc�� ���� �ؾ� �Ѵ�
// ������ filefind�� ����ϰ� �ʹٸ� �ش� ���ϸ� �����ؼ� ����ض�.
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

// FileScanner�� ����
typedef enum _MXDMS_SCANFILTER_STATUS
{
	FSS_NOT_INIT,			// �ʱ�ȭ ���� �ʾҴ�
	FSS_SCANNING,			// ��ĳ�� ���̴�
	FSS_ONESHOT_SCANNING,	// �ѹ��� ��ü ��ĵ�ϴ� ��ĳ�� ���̴�
	FSS_PAUSE_SCANNING,		// ��ĳ���� �Ͻ� ������ ����
	FSS_IDLE,				// �޽� �����̴�
	FSS_STOPPED ,			// ���� �ִ�
	FSS_DIRECT				// direct ��ĵ���̴�
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

// �ٸ� ������Ϸ� �Űܾ� �� �� ����.
// diskScan�� ���� �ý����� CDS�� �߰��� �������� �ִ� ��� ����Ѵ�
// ����ڰ� �������� ���� �����Ͽ� directoryBuilder���� mxDlnaDmsSfm_addCdsItem() �Լ��� ����Ͽ� CDS�� �߰��Ѵ�.
typedef MX_INT32 (*MXDLNA_DMS_ADD_SCANFILTER) (
	MXDLNA_DMS_HANDLE  dmsHandle,
	DMS_DB_HANDLE db_h,
	char * location ,
	char * filename ,
	char * scanningPath
);


// diskScan�� ���� CDS�� �ִ� object�� �����ϴ� ��� ����Ѵ�
// ����ڰ� �������� ���� �����Ͽ� directoryBuilder���� mxDlnaDmsSfm_removeCdsItem() �Լ��� ����Ͽ� CDS���� �����Ѵ�.
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
	MXDMS_SCAN_PATH_NORMAL = 0,					// �� option���� �߰��� directory�� ��� Main DB���� ������
	MXDMS_SCAN_PATH_DETACHABLE_ROOT,			// �� option���� �߰��� directory�� multi DB�� ���� ��
	MXDMS_SCAN_PATH_DETACHABLE_SUB				// .recordings �� ���� ������ �߰��ؾ� �ϴ� directory�� ��� �� option�� ��
												// MXDMS_SCAN_PATH_DETACHABLE_ROOT �� �߰��� directory�� ���Ӱ����̾�� ��
}MXDMS_SCAN_PATH_OPTION;

typedef struct _MXDLNA_FSS_SCANDATA
{
	char * path;		// ��ĵ�� ���
	char * filter;		// Ȯ���� ����
	char * userData;	// ����� ������
	int    depth;		// ���� directory scan ����, [0:1] 0=���� directory scan, 1=�ش� directory �� scan
	MXDMS_SCAN_PATH_OPTION    option;
}MXDLNA_FSS_SCANDATA;


typedef struct _MXDLNA_FSS_REMOVEDIRSTATUS
{
	int totalCount;			// ������ ���� �� ������ ����
	int currentCount;		// ���� ������ ������ �� ������ ����
	int isCancel;			// ��� ���� ����

	int isEnd;				// �������� ���θ� ����. ������ 1
	int doEvent;			// systemUpdateID, ContainerUpdateID ó�� ����
							// 1�̸� ó��
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



// MXDLNA_FSS_SCANDATA�� ����Ʈ�̴�.
// FileScan�� �� ����Ʈ�� �ִ� ��� ��θ� ��ĵ�Ѵ�.
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
	MX_INT32 rescan_interval_ms;			// �ٽ� ��ĵ�� ������ ��ٸ��� �ð� (ms);
	MX_INT32 per_interval_ms;				// �ϳ��� ��ĵ�� ������ ��ٸ��� �ð� (ms);
	MX_INT32 bScanningTimePrint;			// ��ĵ�ϴ� �ð��� �������� ����

	MXDLNA_DMS_PREPROCESS_SCANFILTER	fnScanFilter_preProcess;
	MXDLNA_DMS_POSTPROCESS_SCANFILTER	fnScanFilter_postProcess;
	MXDLNA_DMS_ADD_SCANFILTER			fnScanFilter_addEvent;			// ���ο� �������� ã������ ó���� callback
	MXDLNA_DMS_REMOVE_SCANFILTER		fnScanFilter_removeEvent;		// �������� �����Ҷ� ó���� callback
	MXDLNA_DMS_ADD_DETACHABLE_STORAGE	fnScanFilter_addDetachableStorage;

	MX_INT32 scan_maxcount;                 // oneshot scan�� �ѹ� ��ĵ�Ҷ� �ִ� ������ϴ°�, 0�� ���Ѵ�, ��> 100�� 100���� �����۸� ��ĵ�Ͽ��߰���

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
	\brief	���� ��ĳ�ʸ� �����ϴ� �Լ� �̴�.
	\brief	���� ��ĳ�ʸ� ���� ���� mxDlnaFileScanner_destroy() �Լ��� ȣ���Ѵ�.

	\param	ppHandle	: �Լ� ���ο��� �޸𸮸� �Ҵ��Ͽ� �Ѱ� �ش�.
	\param	pOption	: MXDLNA_FSS_SCANOPTION * Ÿ���� �̿��ؼ� ���� ��ĳ���� �ɼ��� �����Ѵ�
	\param	hDb	: db_handle * Ÿ������ db�� �ڵ��� �Է��Ѵ�.
	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_create(
	MXDLNA_FSS_HANDLE * ppHandle,
	MXDLNA_FSS_SCANOPTION * pOption,
	MXDBC_HANDLE hDb,	// Thread & DB
	MXDLNA_DMS_HANDLE dmsHandle
);


/**	\brief	The mxDlnaFileScanner_destroy function
	\brief	���� ��ĳ�ʸ� �����ϴ� �Լ� �̴�.
	\brief	mxDlnaFileScanner_create() �Լ����� ������ �����͸� �Է��Ѵ�
	\param	pHandle	: void * Ÿ������ mxDlnaFileScanner_create() �Լ����� �����ȴ�

	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_destroy(
	MXDLNA_FSS_HANDLE pHandle
);



/**	\brief	The mxDlnaFileScanner_setInterval function
	\brief	��ĵ�߿� �޽Ľð��� �����ϴ� �Լ��̴�.

	\param	pHandle	: mxDlnaFileScanner_create() �Լ����� ������ �����͸� �Է��Ѵ�.
	\param	interval_ms	: ������ ������ ms ������ �޽Ľð��� �Է��Ѵ�.

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
	\brief	��ĵ�� ��θ� �߰��ϴ� �Լ� �̴�.
	\brief	�� ��ĳ�� �߿��� �߰� �Ҽ� ����.
	\param	pHandle	: mxDlnaFileScanner_create() �Լ����� ������ �����͸� �Է��Ѵ�.
	\param	pAddData  	: MXDLNA_FSS_SCANDATA * Ÿ���� �����͸� �Է��Ѵ�
	\return	MX_INT32
*/
// 20100913
__MXAPI MX_INT32 mxDlnaFileScanner_addDirectory(
	MXDLNA_FSS_HANDLE pHandle ,
	MXDLNA_FSS_SCANDATA * pAddData,
	MXDMS_SCAN_INFO * pMoreInfo
);


/**	\brief	The mxDlnaFileScanner_removeDirectory function
	\brief	mxDlnaFileScanner_getDirectories() �Լ��� Ȯ��
	\param	pHandle	: mxDlnaFileScanner_create() �Լ����� ������ �����͸� �Է��Ѵ�.
	\param	pAddData	: MXDLNA_FSS_SCANDATA * Ÿ���� �����ͷ� path ���� ���ַ� ����.
	\return	MX_INT32
*/
// 20100913
__MXAPI MX_INT32 mxDlnaFileScanner_removeDirectory(
	MXDLNA_FSS_HANDLE pHandle ,
	MXDLNA_FSS_SCANDATA * pAddData,
	MXDLNA_FSS_REMOVEDIRSTATUS * pRemoveProp
);


/**	\brief	The mxDlnaFileScanner_getDirectories function
	\brief	��ĳ���ϴ� ���丮�� ����̴�.
	\param	pHandle	: mxDlnaFileScanner_create() �Լ����� ������ �����͸� �Է��Ѵ�.
	\return	MXDLNA_FSS_SCANDATA_LIST *
*/
__MXAPI MXDLNA_FSS_SCANDATA_LIST * mxDlnaFileScanner_getDirectories(
	MXDLNA_FSS_HANDLE pHandle
);

__MXAPI MXDLNA_FSS_DETACHABLE_SCANDATA_LIST * mxDlnaFileScanner_getDetachableDirectories(
	void
);

// ��ĳ�� �ϴ� ���丮 ����� ���� �̴�. getDirectories �Լ��� ���� ����ϸ� �ȴ�.
__MXAPI MX_INT32 mxDlnaFileScanner_scanListCount(
	MXDLNA_FSS_HANDLE pHandle
);

/**	\brief	The mxDlnaFileScanner_startScan function
	\brief	��ĵ�� �����Ѵ�. �̹� ���۵Ǿ����� ���ٸ� ó���� ���� �ʴ´�.
	\param	pHandle	: mxDlnaFileScanner_create() �Լ����� ������ �����͸� �Է��Ѵ�.
	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_startScan(
	MXDLNA_FSS_HANDLE pHandle ,
	int isOneShot
);



/**	\brief	The mxDlnaFileScanner_stopScan function
	\brief	��ĵ�� �����Ѵ�. �̹� ���� �Ǿ����� ���ٸ� ó���� ���� �ʴ´�.
	\param	pHandle	: mxDlnaFileScanner_create() �Լ����� ������ �����͸� �Է��Ѵ�.
	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_stopScan(
	MXDLNA_FSS_HANDLE pHandle
);




/**	\brief	The mxDlnaFileScanner_getStatus function
	\brief	���� ���� ��ĳ���� ���¸� ��Ÿ����. ���´� FSS_NOT_INIT, FSS_SCANNING... ����� ������ �� �� �ִ�
	\param	pHandle	: mxDlnaFileScanner_create() �Լ����� ������ �����͸� �Է��Ѵ�.
	\return	MX_INT32
*/
__MXAPI MXDMS_SCANFILTER_STATUS mxDlnaFileScanner_getStatus(
	MXDLNA_FSS_HANDLE pHandle
);	// ��ĵ�������� ���� ��ƾ



__MXAPI SCAN_COUNT * mxDlnaFileScanner_getScanCount(
	MXDLNA_FSS_HANDLE pHandle
);


/**	\brief	The mxDlnaFileScanner_directScanStart function
	\brief	���� ��ĵ�ϴ� �Լ��μ� index�� �ش��ϴ� ��θ� �˻��� �� �ִ�.
	\brief	��ҹ��ڸ� �����ϹǷ� �����ؾ� �Ѵ�
	\brief	��� ��θ� ���ؾ� �ϱ� �����̴�.
	\param	pHandle	: mxDlnaFileScanner_create() �Լ����� ������ �����͸� �Է��Ѵ�.
	\param	pData	: MXDLNA_FSS_SCANDATA *
	\param	index	: mxDlnaFileScanner_getStatus()�� ���� ������ ����
	\param	depth	: 1�̸� ������ �ϳ��� Ȯ�� , -1 �̸� ��ü ��ĵ�̴�.

	\return	MX_INT32
*/
__MXAPI MX_INT32 mxDlnaFileScanner_directScanStart(
	MXDLNA_FSS_HANDLE pHandle ,
	MXDLNA_FSS_SCANDATA * pData ,
	int index ,
	int depth
);



/**	\brief	The mxDlnaFileScanner_allocDirData function
	\brief	MXDLNA_FSS_SCANDATA �����͸� ���� �Ҵ� �ϱ� ���� �Լ� �̴�.
	\brief	mxDlnaFileScanner_freeDirData() �Լ��� �޸𸮸� �����Ѵ�.
	\param	ppData	: MXDLNA_FSS_SCANDATA Ÿ������ �޸𸮰� �Ҵ� �Ǿ� ���ϵȴ�.
	\param	filter	: Ȯ���� ����
	\param	path	: �˻��� Path
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
	\brief	mxDlnaFileScanner_allocDirData() �Ҵ�� �������� �޸𸮸� �����Ѵ�.
	\param	pData	: MXDLNA_FSS_SCANDATA * Ÿ������ mxDlnaFileScanner_allocDirData() �Լ��� �Ҵ�� ���̴�.
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


// scanList �󿡼� ���° �ε����� �����ϴ��� �˱� ���� �Լ�
__MXAPI MX_INT32 mxDlnaFileScanner_getMatchedScanIndex(
	MXDLNA_FSS_HANDLE pHandle ,
	const char * path
);

__MXAPI const MXDLNA_FSS_SCANDATA * mxDlnaFileScanner_getMatchedScanList(
	MXDLNA_FSS_HANDLE pHandle ,
	const char * filepath ,
	int * index
);


// ��ĵ�� �����ϱ� ���� �θ���.. TotalCount��������� ���� �����Ѵ�.
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

