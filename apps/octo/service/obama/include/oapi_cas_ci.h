/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_ci.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __OAPI_CAS_CI_H__
#define __OAPI_CAS_CI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define RPC_OAPI_CAS_CI_Init					"oapi.cas.ci.Init"
// CI Plust related.
#define RPC_OAPI_CAS_CI_RequestFileorData		"oapi.cas.ci.RequestFileorData"
#define RPC_OAPI_CAS_CI_RequestAppAbort		"oapi.cas.ci.RequestAppAbort"



#define RPC_OBAMA_CAS_CI_onStatusChanged		"obama.cas.ci.onStatusChanged"
// CI Plust related.
#define RPC_OBAMA_CAS_CI_onCiAmmiMsgReqStart		"obama.cas.ci.onCiAmmiMsgReqStart"
#define RPC_OBAMA_CAS_CI_onCiAmmiMsgFileAck			"obama.cas.ci.onCiAmmiMsgFileAck"
#define RPC_OBAMA_CAS_CI_onCiAmmiMsgReqAppAbort	"obama.cas.ci.onCiAmmiMsgReqAppAbort"
#define RPC_OBAMA_CAS_CI_onCiAmmiMsgAppAbortAck		"obama.cas.ci.onCiAmmiMsgAppAbortAck"



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


#define	APKS_CI_SLOT_MAX_NUM		3
#define	APKS_CAM_NAME_LEN			40			// svc_cas.h 파일 MAX_CI_STR_LEN 로 정의된 길이와 동일하게 셋팅
#define	APKS_CI_PLUS_VER_LENG		32

#if 0
typedef	enum
{
	_eCAM_STATUS_Extracted = 0,
	_eCAM_STATUS_Inserted,
	_eCAM_STATUS_Initializing,
	_eCAM_STATUS_Initialized,

	_eCAM_STATUS_Max
} APKS_CI_CAMState_e;
#endif

typedef struct
{
	HUINT32				ulSlotNumber;
	DxCASCI_CamStatus_e	eStatus;
	HCHAR				szCamName[APKS_CAM_NAME_LEN];
} APKS_CI_CAMInfo_t;

typedef struct
{
	HBOOL				bSupportCI;
	HBOOL				bSupportCIPlus;
	HCHAR				szCIPlusVer[APKS_CI_PLUS_VER_LENG];
	HUINT32				ulSlotCount;
	APKS_CI_CAMInfo_t		astCamInfoList[APKS_CI_SLOT_MAX_NUM];
} APKS_CI_CIInfo_t;

typedef enum
{
	_eCIInfo_Is_SupportedCI				= 0,
	_eCIInfo_Is_SupportedCIPlus,
	_eCIInfo_CIPlus_Version,
	_eCIInfo_Slot_Number,
	_eCIInfo_Max
} APKE_CI_Info_Item_e;

typedef enum
{
	_eCAMInfo_Slot_Number				= 0,
	_eCAMInfo_Status,
	_eCAMInfo_CAM_Name,
	_eCAMInfo_Max
} APKE_CAM_Info_Item_e;


typedef void (*OxCasCi_Notifier_t) (void);
typedef void (*OxCasCi_AppMmiNotifier_t) (CI_AMMI_MSG_TYPE enMsgType, void *pvUserData, HUINT32 ulUserDataSize);
typedef	void	(*CBOapiCasCiObjectHandler)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

// TODO: CI 빌드를 위해 임시로 dlib_cas.h 에 위치함. 옮겨와야함.
//typedef int (*CI_AMMI_CALLBACK_T)(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT *pustContent);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

HERROR 		OAPI_CAS_CI_Init(OxCasCi_Notifier_t fnApkNotifier, OxCasCi_AppMmiNotifier_t fnAppMmiApkNotifier);
HERROR 		OAPI_CAS_CI_GetStatus(APKS_CI_CIInfo_t *pstStatus);


// CI Plus releated.

HERROR		OAPI_CAS_CI_RequestFileorData (HUINT16 usSessionId, CI_AMMI_REQUEST_TYPE enReqType, HUINT8 *pucReqName, HULONG ulReqNameLen);
HERROR		OAPI_CAS_CI_RequestAppAbort (HUINT16 usSessionId, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode);



#endif // __OAPI_CAS_CI_H__
