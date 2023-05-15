/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name�� �����ϴ�.
	@brief	  file_name & simple comment.

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
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
	eDOWN_DOWNLOAD_HBBTV = 0,			// APS HD+ IP���� DSM-CC�� �޴� Data Service (Web Hybrid)

	// CONFIG_MW_DOWNLOAD_OTA_IMAGE
	eDOWN_DOWNLOAD_OTA_IMAGE,			// Nand flash ������ Background OTA download�� ���� ����Ѵ�.
										// OTA �����  OTA Config�� �����Ѵ�.
	eDOWN_DOWNLOAD_FSAT_META,			// FSAT Meta Xml files

	eDOWN_DOWNLOAD_ARIB,				// �Ϻ��� ARIB ���忡�� �����ϴ� �ٿ�ε� �԰� �� ���� ��� (ä�ηΰ�, SW, �������罺ĵ)

	eDOWN_DOWNLOAD_SATRECORD,			//SES Sat Record data DSMCC OC downlod �� ���� ���.

	eDOWN_ACTION_MAX,

    eDOWN_ACTION_NONE = 0xFF,

} SvcDownload_ActionType_e;

typedef enum
{
	// APS Web Hybrid (HBBTV) ��
	eDOWN_DATA_HBBTV_EVENT,			// APS Web Hybrid ���� DSM-CC EVENT ����

	eDOWN_DATA_SATRECORD_VERSION,			//SES Sat Record Vesrion ����.
	eDOWN_DATA_SATRECORD_POLLINGPERIOD,		//SES Sat Record Polling period ����.
	eDOWN_DATA_SATRECORD_FASTBOOKINGDATA,	//SES Sat Record Fast Booking ����.
	eDOWN_DATA_SATRECORD_BOOKINGDATA,		//SES Sat Record Booking ����.

	eDOWNLOAD_DATA_MAX
} SvcDownload_DataType_e;

typedef enum
{
	eDOWN_SET_HBBTV_ADDPID,						// APS Web Hybrid ���� DSM-CC EVENT ����
	eDOWN_SET_HBBTV_REMOVEPID,

	eDOWN_SET_MAX
} SvcDownload_SetType_e;


/** @brief Download event messages */
enum
{
	eSEVT_DOWN_ = eSEVT_DOWN_START,

	/* - Description : Stop�� ��û�� Action���� Session Stop�� �뺸�ϴ� Event Message.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : TBD */
	eSEVT_DOWN_STOPPED,

	/* - Description : Download ����� �������� ��.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : Current Index */
	eSEVT_DOWN_DOWNLOAD_SUCCESS,

	/* - Description : Download ����� �������� ��.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : TBD */
	eSEVT_DOWN_DOWNLOAD_FAIL,

	/* - Description : Download �� ���빰�� �ٲ�ų� Download �� ���빰�� �޶��� ���� ���������.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : Carousel ID */
	eSEVT_DOWN_DOWNLOAD_CHANGED,

	/* - Description : Download progress ����. (Total/Received).
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : Progress (in %) */
	eSEVT_DOWN_DOWNLOAD_PROGRESS,

	/* - Description : DSM-CC Download���� Event �� �߰ߵǾ����� ǥ���Ѵ�.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Down Action Type
		   p2 : PID
		   p3 : Stream Event Data */
	eSEVT_DOWN_DSMCC_EVENT_FOUND,

	/* DSM-CC Download���� 0x3D section�� Stream descriptor event �� �߰ߵǾ����� ǥ���Ѵ�.
	- Parameters Usage :
		handle : Action handle.
		p1 : Down Action Type
		p2 : PID
		p3 : Stream Event Data */
	eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT,

	eSEVT_DOWN_END  /* enum �� ǥ��. ����ϴ� �޽��� �ƴ�. */
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
