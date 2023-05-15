/**
	@file     svc_swup.h
	@brief    svc_swup.h

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef __SVC_SWUP_H__
#define __SVC_SWUP_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include 	<octo_common.h>

#include	<db_svc.h>

#include	<svc_si.h>
//#include	<svc_ch.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define DDB_SAVE_FILE_PATH				"/tmp/humaxtv.hdf"
#define LIVEOTA_DETECT_HANDLE			(0xFFFF1118)

/*******************************************************************/
/********************	   Typedef		   *************************/
/*******************************************************************/

typedef enum
{
	/* - Description : SI에서 OTA check를 위해 걸은 모든 SI Table들을 제대로 받았을 때 :
					   SI Table이 다 왔다는 뜻이지, OTA 정보가 발견되었다는 뜻은 아니다.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Handle
		   p2 : SvcSwUpdate_DetectResult_t *
		   p3 : TBD */
	eSEVT_SWUP_DETECT_SUCCESS = eSEVT_SWUP_START,

	/* - Description : SI에서 OTA check를 위해 걸은 SI table들이 제대로 받아지지 않았을 때 :
					   SI Table이 다 왔다는 뜻이지, OTA 정보가 발견되었다는 뜻은 아니다.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Handle
		   p2 : SvcSwUpdate_DetectResult_t *
		   p3 : TBD */
	eSEVT_SWUP_DETECT_FAIL,

	/* - Description : SI에서 OTA check를 위해 걸은 SI Table에서 Descriptor를 Parsing해서 Private Data를 다 만들었을 때...
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Handle
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SWUP_DETECT_PRIVATE_DATA_FOUND,

	/* - Description : Detect Stop 결과 Notify
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Handle
		   p2 : HBOOL TRUE : Detect Stop Success, FALSE : Detect Stop Fail
		   p3 : TBD */
	eSEVT_SWUP_DETECT_STOP_RESULT,

	/* - Description : Data Download를 완료했을 때 :
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Handle
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SWUP_DOWNLOAD_SUCCESS,

	/* - Description : Data Download를 실패했을 때 :
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Handle
		   p2 : SvcSwUpdate_Error_e
		   p3 : TBD */
	eSEVT_SWUP_DOWNLOAD_FAIL,

	/* - Description : Notify Data Download Progress :
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Handle
		   p2 : Total Size
		   p3 : Current Size */
	eSEVT_SWUP_DOWNLOAD_PROGRESS,

	/* - Description : Download Stop 결과 Notify
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Handle
		   p2 : HBOOL TRUE : Download Stop Success, FALSE : Detect Stop Fail
		   p3 : TBD */
	eSEVT_SWUP_DOWNLOAD_STOP_RESULT,

	/* - Description : Install RAWFS에 Image Write 완료했을 때:
	   - Parameters Usage :
		   handle : HANDLE_NULL
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SWUP_INSTALL_WRITE_IMAGE_SUCCESS,

	/* - Description : Install RAWFS에 Image Write 실패했을 때:
	   - Parameters Usage :
		   handle : HANDLE_NULL
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SWUP_INSTALL_WRITE_IMAGE_FAIL,

	/* - Description : Notify Write Image Progress
	   - Parameters Usage :
		   handle : HANDLE_NULL
		   p1 : Total Size
		   p2 : Current Size
		   p3 : TBD */
	eSEVT_SWUP_INSTALL_PROGRESS,

	eSEVT_SWUP_MAX
} SvcSwUpdate_Event_e;

typedef enum
{
	eSWUP_ERROR_WRITE_FILE,
	eSWUP_ERROR_NO_DEST_FILE,
	eSWUP_ERROR_IP_NO_DATA,
	eSWUP_ERROR_IP_NOT_FOUND_PAGE,
	eSWUP_ERROR_IP_NETWORK,
	eSWUP_ERROR_IP_TIMEOUT,
	eSWUP_ERROR_IP_IDPW,
	eSWUP_ERROR_UNDEFINED
} SvcSwUpdate_Error_e;


/*	Detect
 *	New SW Version Check from RF or IP
 */

#define MAX_VERSION_LENGTH		16
#define MAX_PATH_LENGTH			256
#define MAX_DESC_SIZE			5
#define MAX_DESC_LENGTH			1024*10
#define MAX_DESC_LANG_LENGTH	10

typedef struct
{
	Handle_t				hAction;
	HUINT32					ulDemuxId;
	Handle_t				hSiModule;
	HUINT16					usTsUniqId;
	HUINT32					ulSwupMode;
	HUINT32					ulParam1;
	HUINT32					ulParam2;
	HUINT32					ulParam3;
} SvcSwUpdate_DetectParamRf_t;

typedef struct
{
	HINT8			*szUrl;
} SvcSwUpdate_DetectParamIp_t;

typedef	struct
{
	DbSvc_TsInfo_t	stSvcTsInfo;
	HUINT32			ulDataVersion;
	HUINT16			usOtaPid;
} SvcSwUpdate_DetectResultRf_t;

typedef struct
{
	SWUP_DownloadType_e		eDownloadType;
	DxDeliveryType_e  		eDeliType;
	DbSvc_TsInfo_t			stSvcTsInfo;
	DxTuneParam_t			stChTuningInfo;
	HUINT16					usPid;
} SvcSwUpdate_RfDetectInfo_t;

#if 0
typedef struct
{
	HUINT8			aServerIp[SWUP_ETH_ADDR_LENGTH];
	HUINT8			aUserName[SWUP_USER_LENGTH];
	HUINT8			aPasswd[SWUP_PASSWD_LENGTH];
	HUINT8			aDirPath[SWUP_DIR_PATH_LENGTH];
	HUINT8			aFileName[SWUP_FILE_NAME_LENGTH];
} SvcSwUpdate_IpEmergencyInfo_t;
#endif

typedef struct
{
	HBOOL			bHdcp;
	HBOOL			bForced;
	HUINT32			aucIpSetting[4];		// IP, Subnet Mask, Gateway, DNS
	HINT8			szURLPath[MAX_PATH_LENGTH];
	HINT8			nDescripCount;
	HINT8			aDescripLangType[MAX_DESC_SIZE][MAX_DESC_LANG_LENGTH];
	HINT8			aDescriptions[MAX_DESC_SIZE][MAX_DESC_LENGTH];

	SwUpdate_IpEmergencyInfo_t stEmInfo;
} SvcSwUpdate_IpDetectInfo_t;

typedef struct
{
	Handle_t	hSwUpdate;
	HBOOL		bFoundImagePath;
	HCHAR		szSoftwarePath[MAX_PATH_LENGTH];
}SvcSwUpdate_UsbDetectInfo_t;

typedef struct
{
	DxSwUpdate_Source_e	 eSource;
	HBOOL			 bFound;
	HBOOL			 bAvailable;
	HUINT32			 ulDataVersion;
	HUINT16			 usOtaPid;

	union
	{
		SvcSwUpdate_RfDetectInfo_t		stRfInfo;
		SvcSwUpdate_IpDetectInfo_t		stIpInfo;
		SvcSwUpdate_UsbDetectInfo_t		stUsbInfo;
	} data;
} SvcSwUpdate_DetectResult_t;

/*	Download
 *	SW Image download from a PID(DSMCC) or FTP
 */
typedef struct
{
	Handle_t				 hAction;
	HUINT16					 usTsUniqId;
	HUINT16					 usPid;
	HUINT32					 ulSignalVersion;
	HUINT32					 ulArg1;
	HUINT32					 ulArg2;
} SvcSwUpdate_DownloadSourceInfoRf_t;

typedef struct
{
	HINT8					*szUrl;
} SvcSwUpdate_DownloadSourceInfoIp_t;

/*	Download
 *	SW Image download from a PID(DSMCC) or FTP
 */

// TODO: Define a structure for Emergency Update Information
typedef struct
{
	DxSwUpdate_Source_e	 eSource;
	HUINT32			 ulDataVersion;
	union
	{
		SvcSwUpdate_RfDetectInfo_t		stRfInfo;
		SvcSwUpdate_IpDetectInfo_t		stIpInfo;
		SvcSwUpdate_UsbDetectInfo_t		stUsbInfo;
	} unInfo;
} SvcSwUpdate_EmergencyUpdateInfo_t;

#ifdef __cplusplus
extern "C" {
#endif

HERROR		SVC_SWUPDATE_Init(PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData);

/**********************************************************************************************
 *	Detect : Get SW Version Information from RF or IP
 */
Handle_t	SVC_SWUPDATE_StartDetectIp(SvcSwUpdate_DetectParamIp_t *pstIpParam);
HERROR		SVC_SWUPDATE_StopDetectIp(Handle_t hDetect);

/**********************************************************************************************
 *	Download : Downlaod SW image from RF(dsmcc) or IP(Portal)
 */
Handle_t	SVC_SWUPDATE_StartDownloadRf(SvcSwUpdate_DownloadSourceInfoRf_t *pstRfParam, HUINT8 *szTargetPath);
HERROR		SVC_SWUPDATE_StopDownloadRf(Handle_t hDownload);
Handle_t	SVC_SWUPDATE_StartDownloadIp(SvcSwUpdate_DownloadSourceInfoIp_t *pstIpParam, HUINT8 *szTargetPath);
HERROR		SVC_SWUPDATE_StopDownloadIp(Handle_t hDownload);

HERROR		SVC_SWUPDATE_GetImageSizeIp(Handle_t hDownload, HUINT64 *pullSize);
HERROR		SVC_SWUPDATE_GetDownloadedImageSizeIp(Handle_t hDownload, HUINT64 *pullSize);

/**********************************************************************************************
 *	Install : Install SW Image to RAWFS area
 */
HERROR		SVC_SWUPDATE_InstallWriteImage(DxSwUpdate_Source_e eSource, HUINT8 *pucSourceFile);
HERROR		SVC_SWUPDATE_InstallWriteEmergencySignalInfo(SvcSwUpdate_EmergencyUpdateInfo_t *pstEmergeInfo);
HERROR		SVC_SWUPDATE_InstallWriteEmergencySettingAfterWriteSucc(DxDeliveryType_e eDeliType);
HERROR		SVC_SWUPDATE_InstallWriteEmergencySetting(DxDeliveryType_e eDeliType);
HERROR		SVC_SWUPDATE_InstallWriteEmergencyUpdateInfo(DxSwUpdate_Source_e eSource, SvcSwUpdate_EmergencyUpdateInfo_t *pstInfo);
HERROR		SVC_SWUPDATE_InstallClearEmergencySettingFlag(HUINT32 ulEmergeFlag);
HERROR		SVC_SWUPDATE_InstallCancel(void);
HERROR		SVC_SWUPDATE_InstallFinalize(HUINT32 ulVer);


/**********************************************************************************************
 *	IP Upgrade
 */


#ifdef __cplusplus
};
#endif

#endif	/* __SVC_SWUP_H__ */

