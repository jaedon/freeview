/***************************************************************
* $Workfile:   ci.h  $
* $Modtime:   Sep 04 2004 21:38:22  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_BASE_H_
#define _CI_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/
#include "pal_ci.h"

/***************************************************************
* definition
***************************************************************/

/* PAL에서 정의한 Context Number를 Mapping한다. */
#define	CI_MAX_NUM_OF_CAM				2	/* 실제로 달려 있는 슬롯이 하나라도 이 값을 1로 변경하지는 말기. CI 모듈에서는 항시 2개까지 처리할 수 있도록 고려한다. see PAL_MAX_CISLOT_NUMBER */
#define	CI_MAX_NUM_OF_CAM_SVC		1	// 1 ~ 4
#define	CI_MAX_NUM_OF_SVC				(CI_MAX_NUM_OF_CAM * CI_MAX_NUM_OF_CAM_SVC)
//#define	CI_MAX_NUM_OF_PROG_ES		CAS_MAX_NUM_OF_ES 	// 5 --> 20 --> 60 --> CAS_MAX_NUM_OF_ES 로 통일

/* CIRH (CI Resource Handler) Task Priority & Stack Size */
#define MAX_CI_STR_LEN				40	// CI 및 CI+ 스펙에 따라 최소한 The host shall be able to display 40 characters and 5 lines in addition of title, subtitle and bottom line.
#define CI_DT_TIMER_PERIOD			250	// ms

#define SUPPORT_SMART_CI_MMI_PINCODE

#ifndef NULL
#define NULL		(void *)0
#endif

#define CI_DESCRAMBLING 1
#define CI_NOT_DESCRAMBLING 0

/***************************************************************
* typedef
***************************************************************/
typedef enum {
	CI_CA_APP_TYPE = 1,				// currently only support
	CI_EPG_APP_TYPE,
	CI_SW_UPGRADE_APP_TYPE,
	CI_NW_IF_APP_TYPE,
	CI_ACCESS_AIDS_APP_TYPE,
	CI_UNCLASSIFIED_APP_TYPE
} CI_APP_TYPE;

typedef enum {
	CI_CA_SYSTEM_ID_ERROR = 0,
	CI_CA_DEVICE_ERROR,
	CI_CA_PMT_SENDED
} CI_CAS_STATE;

typedef enum {
	CI_TUNE_UNABLE = 0,
	CI_TUNE_FAIL,
	CI_TUNE_OK
} CI_TUNE_RESULT;

typedef struct {
	CI_APP_TYPE nType;
	unsigned short usManufacturer;
	unsigned short usManufacturerCode;
	unsigned char szText[MAX_CI_STR_LEN+1];
	unsigned char ucIsInitialized;
} CI_APP_INFO;

typedef enum {
	CI_CAM_EXTRACTED = 0,	/* 캠이 제거된 경우 또는 캠 모듈을 power off 한 경우 (DD로부터 notify됨) */
	CI_CAM_INSERTED,		/* 캠이 삽입된 직후 (DD로부터 notify됨) */
	CI_CAM_INITIALIZING,	/* 캠이 삽입되어 RM 세션이 오픈된 상태 (CI 스택으로부터 notify됨) */
	CI_CAM_INITIALIZED	/* 캠 정보(Application Information)까지 수신이 된 상태 (CI 스택으로부터 notify됨) */
} CI_CAM_STATE;			/* refer to PAL_CI_STATE */

/********************************************************************
 Function   :	CI_NOTIFY_CAM_STATE_FUNC
 Description :	Notify CAM state
 Input      :	usSlotId - CI Slot ID
 				nCamState - CAM state
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int
		(*CI_NOTIFY_CAM_STATE_FUNC)
		(CI_SLOT_ID usSlotId, CI_CAM_STATE nCamState);


/********************************************************************
 Function   :	CI_NOTIFY_APP_INFO_FUNC
 Description :	Notify CAM application info.
 Input      :	usSlotId - CI Slot ID
 				pInfo - CAM application info. (static data)
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int
		(*CI_NOTIFY_APP_INFO_FUNC)
		(CI_SLOT_ID usSlotId, CI_APP_INFO *pInfo);


/********************************************************************
 Function   :	CI_NOTIFY_CA_SYS_ID_FUNC
 Description :	Notify CA system IDs of CAM
 Input      :	usSlotId - CI Slot ID
 				pusCaSysId - CAM application info. (static data)
 				ucNumCaSysId - Number of CA System IDs
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int
		(*CI_NOTIFY_CA_SYS_ID_FUNC)
		(CI_SLOT_ID usSlotId, unsigned short *pusCaSysId, unsigned short usNumCaSysId);


/********************************************************************
 Function   :	CI_NOTIFY_MMI_REQ_FUNC
 Description :	Notify MMI Request
 Input      :	hMmi - MMI handle
 				nMmiReq - MMI Request type
 				pvParam - Parameter for MMI Request
 				ulParamSize - Size of pvParam (unit:byte)
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int
		(*CI_NOTIFY_MMI_REQ_FUNC)
		(MMI_HANDLE hMmi, MMI_REQ nMmiReq, void *pvParam, unsigned long ulParamSize);


/********************************************************************
 Function   :	CI_NOTIFY_CAS_STATE_FUNC
 Description :	Notify CAS state
 Input      :	usSlotId - CI Slot ID
 				nCasState - CAS state
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int
		(*CI_NOTIFY_CAS_STATE_FUNC)
		(CI_SLOT_ID usSlotId, CI_CAS_STATE nCasState);

typedef struct {
	CI_NOTIFY_CAM_STATE_FUNC		pfnNotifyCamState;
	CI_NOTIFY_APP_INFO_FUNC			pfnNotifyAppInfo;
	CI_NOTIFY_CA_SYS_ID_FUNC		pfnNotifyCaSysId;
	CI_NOTIFY_MMI_REQ_FUNC			pfnNotifyMmiReq;
	CI_NOTIFY_CAS_STATE_FUNC		pfnNotifyCasState;
} CI_NOTIFY_FUNC_LIST;


/***************************************************************
*
*
* global function prototypes
*
*
***************************************************************/
/******** CI Extension API *********/
/********************************************************************
 Function   :	CI_Init
 Description :	Initialize CI
 Input      :
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_Init(void);


/********************************************************************
 Function   :	CI_ResetSlot
 Description :	Reset CAM
 Input      :	usSlotId - CI Slot ID
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_ResetSlot(CI_SLOT_ID usSlotId);


/********************************************************************
 Function   :	CI_GetAppInfo
 Description :	Get CAM Application Info.
 Input      :	usSlotId - CI Slot ID
 Output     :	ppInfo - pointer of pointer of CAM application Info. data.
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_GetAppInfo(CI_SLOT_ID usSlotId, CI_APP_INFO **ppInfo);


/********************************************************************
 Function   :	CI_DeleteTc
 Description :	Delete TC of CAM
 Input      :	usSlotId - CI Slot ID
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_DeleteTc(CI_SLOT_ID usSlotId);


/********************************************************************
 Function   :	CI_EnterMenu
 Description :	Enter CAM menu
 Input      :	usSlotId - CI Slot ID
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_EnterMenu(CI_SLOT_ID usSlotId);


/********************************************************************
 Function   :	CI_MmiClose
 Description :	Close CI MMI
 Input      :	hMmi - CI MMI handle
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_MmiClose(MMI_HANDLE hMmi);


/********************************************************************
 Function   :	CI_MmiAnswer
 Description :	Answer to MMI_MENU, MMI_LIST
 Input      :	hMmi - CI MMI handle
 				nMmiReq - MMI request
 				ucChoiceNo - Choice number. if nMmiReq is MMI_LIST, this is don't-care value
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_MmiAnswer(MMI_HANDLE hMmi, MMI_REQ nMmiReq, unsigned char ucChoiceNo);


/********************************************************************
 Function   :	CI_MmiAnswerString
 Description :	Answer string to MMI_ENQ
 Input      :	hMmi - CI MMI handle
 				pszStr - pointer of Answer String. must be freed by caller
 				usStrSize - size of pszStr. number of byte
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_MmiAnswerString(MMI_HANDLE hMmi, unsigned char *pszStr, unsigned short usStrSize);

/********************************************************************
 Function   :	CI_MmiGetSlotId
 Description :	Get Slot ID by MMI_HANDLE
 Input      :	hMmi - CI MMI handle
 Output     :	pusSlotId - pointer of Slot ID
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_MmiGetSlotId(MMI_HANDLE hMmi, CI_SLOT_ID *pusSlotId, unsigned short	*pusSsNo);

/********************************************************************
 Function   :	CI_MmiGetInstBySlotId
 Description :	Get CI MMI handle by Slot ID
 Input      :	usSlotId - CI Slot ID
 Output     :
 Return     :	CI MMI handle for OK, NULL for Failure
 ********************************************************************/
MMI_INSTANCE * CI_MmiGetInstBySlotId(CI_SLOT_ID usSlotId); /* 이 함수는 테스트 용도로만 사용된다. 일반적인 용도로 사용하지 말 것!! */

/********************************************************************
 Function   :	CI_SendCaPmt
 Description :	send ca_pmt to cam
 Input      :	usSlotId - CI Slot ID
 				pucCaPmt - pointer of ca_pmt() data
 				usCaPmtLen - length of pucCaPmt. number of byte
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_SendCaPmt(CI_SLOT_ID usSlotId, unsigned char *pucCaPmt, unsigned short usCaPmtLen);


/******** CI Extension add/delete Call Back API *********/
int CI_AddNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList);
int CI_DeleteNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList);


#ifdef __cplusplus
}
#endif

#endif /* _CI_BASE_H_ */

