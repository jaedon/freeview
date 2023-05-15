/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#ifndef	__MW_DOWNLOAD_H__
#define	__MW_DOWNLOAD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
// >>> CONFIG_MW_DOWNLOAD_OTA_IMAGE
#define FILE_PATH_NAME_LENGTH			128
#define HDF_OTA_SAVE_PATH				"/var/buffer/"
#define HDF_OTA_DOWNLOADFILE_NAME		"download.bin"
#define HDF_OTA_TARGETFILE_NAME			"target.hdf"
// <<< CONFIG_MW_DOWNLOAD_OTA_IMAGE

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief  SI action type   */
typedef enum
{
	eDOWN_DOWNLOAD_HBBTV = 0,			// APS HD+ IP에서 DSM-CC로 받는 Data Service (Web Hybrid)

	// CONFIG_MW_DOWNLOAD_OTA_IMAGE
	eDOWN_DOWNLOAD_OTA_IMAGE,			// Nand flash 영역에 Background OTA download를 위해 사용한다.
										// OTA 방식은  OTA Config로 결정한다.
	eDOWN_DOWNLOAD_FSAT_META,			// FSAT Meta Xml files

	eDOWN_DOWNLOAD_ARIB,				// 일본향 ARIB 스펙에서 정의하는 다운로드 규격 을 위해 사용 (채널로고, SW, 지상파재스캔)

	eDOWN_DOWNLOAD_SATRECORD,			//SES Sat Record data DSMCC OC downlod 를 위해 사용.

	eDOWN_ACTION_MAX,

    eDOWN_ACTION_NONE = 0xFF,

} SvcDownload_ActionType_e;

typedef enum
{
	// APS Web Hybrid (HBBTV) 용
	eDOWN_DATA_HBBTV_EVENT,			// APS Web Hybrid 에서 DSM-CC EVENT 정보

	eDOWN_DATA_SATRECORD_VERSION,			//SES Sat Record Vesrion 정보.
	eDOWN_DATA_SATRECORD_POLLINGPERIOD,		//SES Sat Record Polling period 정보.
	eDOWN_DATA_SATRECORD_FASTBOOKINGDATA,	//SES Sat Record Fast Booking 정보.
	eDOWN_DATA_SATRECORD_BOOKINGDATA,		//SES Sat Record Booking 정보.

	eDOWNLOAD_DATA_MAX
} SvcDownload_DataType_e;

typedef enum
{
	eDOWN_SET_HBBTV_ADDPID,						// APS Web Hybrid 에서 DSM-CC EVENT 정보
	eDOWN_SET_HBBTV_REMOVEPID,

	eDOWN_SET_MAX
} SvcDownload_SetType_e;


/** @brief Download event messages */
enum
{
	eSEVT_DOWN_ = eSEVT_DOWN_START,

	/* - Description : Stop을 요청한 Action에게 Session Stop을 통보하는 Event Message.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : TBD */
	eSEVT_DOWN_STOPPED,

	/* - Description : Download 기능이 성공했을 때.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : Current Index */
	eSEVT_DOWN_DOWNLOAD_SUCCESS,

	/* - Description : Download 기능이 실패했을 때.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : TBD */
	eSEVT_DOWN_DOWNLOAD_FAIL,

	/* - Description : Download 중 내용물이 바뀌거나 Download 후 내용물이 달라진 일이 생겼을경우.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : Carousel ID */
	eSEVT_DOWN_DOWNLOAD_CHANGED,

	/* - Description : Download progress 전달. (Total/Received).
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : Progress (in %) */
	eSEVT_DOWN_DOWNLOAD_PROGRESS,

	/* - Description : DSM-CC Download에서 Event 가 발견되었음을 표시한다.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : Stream Event Data */
	eSEVT_DOWN_DSMCC_EVENT_FOUND,

	/* DSM-CC Download에서 0x3D section의 Stream descriptor event 가 발견되었음을 표시한다.
	- Parameters Usage :
		handle : Action handle.
		p1 : Down Action Type
		p2 : PID
		p3 : Stream Event Data */
	eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT,

	eSEVT_DOWN_END  /* enum 끝 표시. 사용하는 메시지 아님. */
};

typedef struct
{
	SvcDownload_SetType_e	eSetType;

	HUINT32			 ulCarouselId;
	HUINT16			 usPid;
	HUINT8			 ucStreamType;
	HCHAR			*pszCachePath;
} SvcDownload_SetHbbtvAddPid_t;

typedef struct
{
	SvcDownload_SetType_e	eSetType;

	HUINT32			 ulCarouselId;
	HUINT16			 usPid;
} SvcDownload_SetHbbtvRemovePid_t;

typedef union
{
	SvcDownload_SetType_e	eSetType;

#if defined(CONFIG_MW_SI_AIT)
	SvcDownload_SetHbbtvAddPid_t	 stHbbtvAddPid;
	SvcDownload_SetHbbtvRemovePid_t	 stHbbtvRemovePid;
#endif
} SvcDownload_SetData_t;

typedef struct
{
	HUINT16		usOnId;
	HUINT16		usTsId;
	HUINT16		usSvcId;

	HBOOL		bEventIdFlag;

	HUINT16		usEventId;

	HUINT8		ucMonth;
	HUINT8		ucDay;
	HUINT8		ucStartHour;
	HUINT8		ucStartMinute;
	HUINT8		ucDurHour;
	HUINT8		ucDurMinutes;

}	SvcDownload_SatRecBooking_t;

typedef struct
{
	HBOOL	bSeriesFlag;
	HBOOL	bRecordCancelFlag;

	SvcDownload_SatRecBooking_t	stBookingData;
}	SvcDownload_SatRecData_t;

typedef HERROR (* SvcDownload_SiGetTimeOutCb_t)(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout);

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR SVC_DOWNLOAD_Init(PostMsgToMgrCb_t pfnMsgNotify, PostDataToMgrCb_t pfnDataNotify);
HERROR SVC_DOWNLOAD_SiRegisterTimeoutCallback(SvcDownload_SiGetTimeOutCb_t fnSiTimeout);

HERROR SVC_DOWNLOAD_Start(Handle_t hAction, HUINT16 usTsUniqId, SvcDownload_ActionType_e eActType, HUINT16 usPid, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
HERROR SVC_DOWNLOAD_Stop(Handle_t hAction, ApiSyncMode_t syncMode);
HERROR SVC_DOWNLOAD_Set (Handle_t hAction, SvcDownload_SetType_e eSetType, SvcDownload_SetData_t *pstData);
HERROR SVC_DOWNLOAD_GetData(Handle_t hAction, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
HERROR SVC_DOWNLOAD_FreeData(Handle_t hAction, SvcDownload_DataType_e eDataType, HUINT32 ulDataNum, void *pvDataArray);

HERROR SVC_DOWNLOAD_CheckDownloadComplete(Handle_t hAction, SvcDownload_ActionType_e eActType, HUINT32 ulSignalVersion, HBOOL *pbComplete);
HERROR SVC_DOWNLOAD_ClearDownloadInfo(Handle_t hAction, SvcDownload_ActionType_e eActType);


#endif		/* __MW_DOWNLOAD_H__ */
