
/*******************************************************************************
* VALUPS R&D Lab., VALUPS INC., SEOUL, KOREA
* COPYRIGHT(c) 2010 by VALUPS Inc.
* 
* All rights reserved. No part of this work may be reproduced, stored in a
* retrieval system, or transmitted by any means without prior written
* permission of VALUPS Inc.
******************************************************************************/


#ifndef MXDLNA_CAM_H___
#define MXDLNA_CAM_H___



#ifdef __cplusplus
extern "C"
{
#endif

#include "mxapi_define.h"

	//#include "mxdlna/mxdlna_dms.h"
	#include "mxdlna/mxdlna_struct.h"
	

	// ============================================
	// Type
	// ============================================
	//preDefine
	struct _CAMSYNC_WORKING_INFO;


	typedef enum _CAM_RET
	{
		CAMRET_ERROR = -1,
		CAMRET_OK = 0,
		CAMRET_SUCCESS = 1,
	}CAM_RET;


	typedef enum _CAMSYNC_WORKER_STATUS
	{
		CAMSYNCWORKER_STATUS_NOTINIT = 0,
		CAMSYNCWORKER_STATUS_INIT,
		CAMSYNCWORKER_STATUS_START_ONE,
		CAMSYNCWORKER_STATUS_START_ALL,
		CAMSYNCWORKER_STATUS_STOP,
		CAMSYNCWORKER_STATUS_END,
	}CAMSYNCWORKER_STATUS;

	typedef enum _CAMSYNC_DNWORKER_STATUS
	{
		CAMSYNCDNWORKER_STATUS_NOTINIT = 0,
		CAMSYNCDNWORKER_STATUS_INIT,
		CAMSYNCDNWORKER_STATUS_START,		
		CAMSYNCDNWORKER_STATUS_STOP,
		CAMSYNCDNWORKER_STATUS_END,
	}CAMSYNC_DNWORKER_STATUS;
	

	typedef enum _SYNC_RET
	{
		SYNC_ERROR_CANCEL = -3,
		SYNC_ERROR_SERVERCONNECTFAIL = -2,
		SYNC_ERROR = -1,
		SYNC_OK = 0,
		SYNC_SUCCESS,	
	}SYNC_RET;

	typedef enum _MXDLNA_CAMSYNC_STATUS
	{
		CAMSYNC_STATUS_INIT = 0,           // DMS를 켜고 초기화한 상태
		CAMSYNC_STATUS_WORKING = 1,        // CAM Sync 작업 중인상태
		CAMSYNC_STATUS_ERROR = 2,          // CAM Sync 작업이 무슨 이유인가 실패한 경우
		CAMSYNC_STATUS_COMPLEATE = 3,      // CAM Sync 작업이 성공적을 끝난 경우
	}MXDLNA_CAMSYNC_STATUS;

	typedef enum _MXDLNA_CAM_META_AGGREGATIONMODE
	{
		CAM_META_AGGREGATION_NONE = 0,		// Metadata에 대한 Aggregation 을 수행하지 않는다
		CAM_META_AGGREGATION_OK				// Metadata에 대한 Aggregation 을 수행한다
	}MXDLNA_CAM_META_AGGREGATIONMODE;


	typedef enum _MXDLNA_CAM_DOWNLOADMODE
	{
		CAM_DOWNLOAD_NONE = 0,				// Download 를 하지 않는다
		CAM_DOWNLOAD_AUDIO = 1,				// Audio에 대한 아이템을 다운로드 한다
		CAM_DOWNLOAD_VIDEO = 2,				// Video에 대한 아이템을 다운로드 한다
		CAM_DOWNLOAD_IMAGE = 4,				// Image에 대한 아이템을 다운로드 한다
		CAM_DOWNLOAD_ETC = 8,				// ETC에 대한 아이템을 다운로드 한다		
	}MXDLNA_CAM_DOWNLOADMODE;

	typedef struct
		_MXDLNA_DMS_CAM_INFO
	{
		char * UDN;
		char * rootObjectID;
		MXDLNA_CAMSYNC_STATUS syncStatus;
		char * workingContainerID;

		int audioCount;
		int videoCount;
		int imageCount;
		int etcCount;
		int totalCount;

		short portnumber;

		// 20101026
		MXDLNA_CAM_META_AGGREGATIONMODE metaAggregationMode;
		MXDLNA_CAM_DOWNLOADMODE downloadMode;
	}MXDLNA_DMS_CAM_INFO;



	#define CAM_CONTAINER_NAME			"통합 홈미디어 서버"
	#define CAM_FMCD_SERVER_NAME_STR	"KT Home VHS"
	#define CAM_FMCD_SERVER_NAME_LEN	11
	#define CAM_TOP_OBJECTID			"0\\1"



	typedef CAM_RET  (*FN_WORKINGOBJECT_UPDATE_CALLBACK)(
		struct _CAMSYNC_WORKING_INFO * pInfo,
		const char * workingUpdateObject
		);
		
	typedef int (*FN_STATUSCOUNT_UPDATE_CALLBACK)(
		int aCount,int vCount,int iCount,int eCount,int tCount
		);
	
	typedef int (*FN_VIRTUALCDS_MANAGER_CALLBACK)(
		struct _CAMSYNC_WORKING_INFO * pInfo,
		MXDLNA_CDSOBJECT * pObject,
		int isContainer,
		int mode
		);

	typedef void (*FN_MSEARCH_CALLBACK)(
		void
		);

	typedef SYNC_RET (*FN_TRAVERSE_SERVER)(
		struct _CAMSYNC_WORKING_INFO * pInfo
		);

	typedef void (*FN_SET_DEVICE_NOTIFY_HANDLER)(
		void* fnNotifyHandler
		);	

	typedef CAM_RET  (*FN_CALC_ITEM_COUNT)(
		struct _CAMSYNC_WORKING_INFO * pInfo,
		int media_type
		);

	typedef CAM_RET (*FN_CREATE_CAM_TOP_OBJECT)(		
		const char * udn, 
		const char **ppObjectID
		);

	typedef CAM_RET (*FN_MAKE_CAM_CDSOBJECT)(		
		const char * rootObjectID, 
		MXDLNA_CDSOBJECT * pObject, 
		unsigned long ipaddr , 
		unsigned short port
		);

	typedef int (*FN_DISKSPACE_UNDERFLOW_CALLBACK)(
		MX_UINT64 downloadSize, MX_UINT64 freeSize
		);




	

	typedef struct _CAM_CONFIG
	{
		int sync_interval_sec;			// 전체 sync 간격
		char localUDN[48];				// 자체 서버의 UDN 값..sync Skip을 위해

		void * hDb;
		void * hDms;
		void * hScan;

		void * hFmcdMscp;
		void * hFmcdMscpA;

		// callback
		FN_STATUSCOUNT_UPDATE_CALLBACK fnStatusCountUpdate;
		FN_VIRTUALCDS_MANAGER_CALLBACK fnVirtualCdsManager;
		FN_MSEARCH_CALLBACK fnMsearch;
		FN_TRAVERSE_SERVER fnTraverse;
		FN_SET_DEVICE_NOTIFY_HANDLER fnSetNotifyHandler;
		FN_DISKSPACE_UNDERFLOW_CALLBACK fnDiskUnderflowEvent;
	}CAM_CONFIG;

	// 서버 싱크 작업중인 것에 대한 정보
	typedef struct _CAMSYNC_WORKING_INFO
	{
		int bWorking;
		int bSyncCancel;					// 현재 작업중인 Sync를 취소 할때
		int bSyncCancelCompleate;			// 현재 작업중인 Sync 취소 작업이 끝났을 때 값 1로 변경
		char currentWorkingObjID[256];		// 현재 작업중인 Container의 ObjectID 이값이 변경 되면 DB에 저장한다		
		char rootObjectID[256];				// 		
		char currentWorkingUDN[48];			// 현재 작업중인 UDN
		int bVHSDMS;						// icube 스택을 사용하는 DMS인가
		char folderObjectID[256];
		char currentFriendlyname[256];
		char folderPath[256];
		
		int audioCount;
		int videoCount;
		int imageCount;
		int etcCount;
		int totalCount;
		int bChangedCount;

		int bSameUdn;						// sync 중인 서버와 작업중인 서버가 같은지 확인
		char camTopObject[256];				// CAM의 최상위 ObjectID
		char syncTime[19+1];				// 1970-01-01 00:00:00        (19+1)
		char tdmsObject[256];				// CAM의 최상위 ObjectID

		unsigned long ipaddr;
		unsigned short port;

		FN_WORKINGOBJECT_UPDATE_CALLBACK fnWorkingObjectUpdate;
		FN_STATUSCOUNT_UPDATE_CALLBACK fnStatusCountUpdate;
		FN_VIRTUALCDS_MANAGER_CALLBACK fnVirtualCdsManager;
		FN_CALC_ITEM_COUNT			fnCalcItemCount;			// 샘플에서 사용하고 있는 함수
		FN_CREATE_CAM_TOP_OBJECT	fnCreateCamTopObject;		// 샘플에서 사용하고 있는 함수
		
	}CAMSYNC_WORKING_INFO;	

	// ============================================


	// ============================================
	// APIs
	// ============================================
	

	// CAM 을 초기화 한다
	// mxCam_closeSync() 종료한다
	__MXAPI 
	CAM_RET mxCam_initSync(CAM_CONFIG * pConfig);


	// MetaData 수집 시작
	// bAllServer값이 1이면 udn 순서와 상관없이 모든 서버를 Sync 한다 - 현재는 설정된 모든 서버를 순회한다
	__MXAPI 
	CAM_RET mxCam_startSync(const char * udn , int bAllServer);
	// MetaData 수집 중지
	__MXAPI 
	CAM_RET mxCam_stopSync(void);

	// CAM 종료 
	// mxCam_initSync() 시작한다
	__MXAPI 
	CAM_RET mxCam_closeSync(void);

	// CAM 상태 정보
	__MXAPI 
	CAMSYNCWORKER_STATUS mxCam_getSyncStatus(void);

	// CAM Synctime 설정	
	CAM_RET mxCam_setSyncTime(int sync_interval_sec);
	CAM_RET mxCam_getSyncTime(int * pSync_interval_sec);
	// ============================================




	// ============================================
	// Download
	// ============================================
	// 다운로드는 이미 받은 파일이 있으면 다시 다운로드 하지 않는다.
	// 해당 파일이 지워지면 Retry 할 수 있다.
	// 디스크 용량이 부족하면 다운로드 하지 않고 Error Callback을 호출한다
		//	fnDiskUnderflowEvent

	// Downloader 시작
	__MXAPI 
	CAM_RET mxCamDn_startDownload();
	// Downloader 중지
	__MXAPI 
	CAM_RET mxCamDn_stopDownload();

	// Call : scanfilter - remove
	__MXAPI 
	CAM_RET mxCamDn_RemoveDownloadInfo(const char * pInRemoved_path);
	// ============================================

		

	// DMS
	struct _MXDLNA_DMS_DEVICEINFO;

	// SystemFolderManager의 최상위 폴더를 만든다
	__MXAPI 
	CAM_RET mxCam_makeCamSfmContainer(const char * titleUTF8);

	// DMS 의 개수를 얻어온다
	__MXAPI 
	CAM_RET mxCam_getDmsCount(int * pCount);

	// DMS의 정보를 얻어온다
	__MXAPI 
	CAM_RET mxCam_getServerItem(int index , struct _MXDLNA_DMS_DEVICEINFO * pItem, unsigned long * pIpaddr);
	
	// DMS의 정보를 메모리 해제한다
	__MXAPI 
	CAM_RET mxCam_freeItem(struct _MXDLNA_DMS_DEVICEINFO * pItem, int bThisRemove);
	
	// 특정 서버의 Sync 데이터를 삭제한다.
	// bAllServer 값이 1이면 모든 서버의 Sync 데이터를 삭제한다
	__MXAPI 
	CAM_RET mxCam_removeSyncData(const char * udn , int bAllRemove);	

	// 서버의 download 모드 및 aggregationMode를 설정한다
	// 기본값은 어떤것도 download 하지 않는다
	// 기본값은 어떤 MetaData도 Aggregation 하지 않는다
	__MXAPI 
	CAM_RET mxCam_setServerConfig(
		const char * udn , 
		MXDLNA_CAM_DOWNLOADMODE * pDownloadMode, 
		MXDLNA_CAM_META_AGGREGATIONMODE * pMetaAggregationMode
	);
	

	// 입력한 파일 경로가 download 받은 파일인지 확인 하는 함수
	__MXAPI 
	CAM_RET mxCam_checkDownloadFile(const char * pInPath);






#ifdef __cplusplus
}
#endif


#endif // MXDLNA_CAM_H___


