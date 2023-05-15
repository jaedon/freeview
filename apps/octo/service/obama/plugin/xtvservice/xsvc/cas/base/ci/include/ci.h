/***************************************************************
* $Workfile:   ci.h  $
* $Modtime:   Sep 04 2004 21:38:22  $
* Auther : Jaehee Cho / hmkim (CI+)
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
#include <octo_common.h>
#include <svc_cas.h>

//#include <pal_ci.h> // 요거 수정해야 한다... 이곳에서 pal include하면 안된다.

#define CARM_MAX_CISLOT_NUMBER	PAL_MAX_CISLOT_NUMBER
#define CARM_INVALID_CI_SLOT_ID	INVALID_CI_SLOT_ID





/***************************************************************
* definition
***************************************************************/

/* CI+ 주요 동작 확인용 : CI 스택 내부에서만 사용할 것
	octo 1.0에서 pal_ci.h에 있는 define 을 여기로 옮겨 왔음.. ci stack가 pal위로 올라와서 어쩔수 없었음.
*/
//#define _CI_PLUS_DEBUG_
/* 테스트용 정식 credentials : 오직 테스트 용도로만 사용하고 제품 릴리즈 시에 define 되어서는 안된다!
	20090409083908~20590409083908 까지 유효하다.
*/
//#define CI_PLUS_PRODUCTION_Credentials

/* LLP test credentials : 정식 credentials 입수 전까지만 한시적으로 사용하고 제품 릴리즈 시에 define 되어서는 안된다!
	20090206~20110205 까지 유효하다.
*/
//#define CI_PLUS_TEST_Technology_from_TrustCenter

/* SmarDTV test credentials : 오직 SmarDTV CI+ 테스트 킷을 위해서만 사용하고 제품 릴리즈 시에 define 되어서는 안된다! */
//#define CI_PLUS_TEST_Technology_from_SmarDTV

#define	CI_MAX_NUM_OF_CAM			CARM_MAX_CISLOT_NUMBER	/* 실제로 달려 있는 슬롯이 하나라도 이 값을 1로 변경하지는 말기. CI 모듈에서는 항시 2개까지 처리할 수 있도록 고려한다. */

#ifdef CONFIG_MW_MM_PVR
#define	CI_MAX_NUM_OF_CAM_SVC		NUM_MAX_ACTION
#else
#define	CI_MAX_NUM_OF_CAM_SVC		1
#endif

#define	CI_MAX_NUM_OF_SVC			(CI_MAX_NUM_OF_CAM * CI_MAX_NUM_OF_CAM_SVC)
//#define	CI_MAX_NUM_OF_PROG_ES	CAS_MAX_NUM_OF_ES 	// 5 --> 20 --> 60 --> CAS_MAX_NUM_OF_ES 로 통일

/* CIRH (CI Resource Handler) Task Priority & Stack Size */
#define CI_DT_TIMER_PERIOD		250	// ms

#define SUPPORT_SMART_CI_MMI_PINCODE

#ifndef NULL
#define NULL	(void *)0
#endif

#define CI_DESCRAMBLING	1
#define CI_NOT_DESCRAMBLING	0

/***************************************************************
* typedef
***************************************************************/
/* MMI : MMI 관련은 CI에 국한시키지 않고 공용화할 계획 --> pal_mmi.h 같은 곳으로 이동 */
typedef unsigned long MMI_HANDLE; /* handle for MMI_INSTANCE */

typedef struct {
	unsigned short	usSsNo;
} MMI_INSTANCE;

typedef enum {
	MMI_OPEN = 0,
	MMI_CLOSE,
	MMI_TEXT,
	MMI_ENQ,
	MMI_MENU,
	MMI_LIST,
	MMI_ERROR
} MMI_REQ;

typedef struct {
	unsigned char ucBlindOnOff;	/* 사용자 입력의 화면 표시 방식 */
	unsigned char ucAnswerLen;		/* 질의에 응답해야 할 스트링 길이 */
	unsigned char *pszPrompt;		/* 질의하는 스트링 */
} MMI_ENQ_PARAM;

typedef struct {
	unsigned int ulNumOfItem;		/* 아이템의 개수 */
	unsigned char *pszTitle;		/* 타이틀 영역에 표시될 스트링 */
	unsigned char *pszSubTitle;	/* 서브타이틀 영역에 표시될 스트링 */
	unsigned char *pszBottom;		/* 하단 영역에 표시된 스트링 */
	unsigned char **ppszItem;		/* 각 아이템의 스트링 */
} MMI_MENU_LIST_PARAM;

typedef enum {
	CI_CAM_EXTRACTED = 0,	// 캠이 제거된 경우 또는 캠 모듈을 power off 한 경우 (DD로부터 notify됨)
	CI_CAM_INSERTED,		// 캠이 삽입된 직후 (DD로부터 notify됨)
	CI_CAM_INITIALIZING,	// 캠이 삽입되어 RM 세션이 오픈된 상태 (CI 스택으로부터 notify됨)
	CI_CAM_INITIALIZED	// 캠 정보(Application Information)까지 수신이 된 상태 (CI 스택으로부터 notify됨)
} CI_CAM_STATE;			// refer to PalCi_State_e

typedef enum {
	CI_APP_TYPE_CA = 0x01,				// Application Information v1 (CI_AI_RES_V1_ID) : currently only supported
	CI_APP_TYPE_EPG = 0x02,				//
	CI_APP_TYPE_SW_UPGRADE = 0x03,		// Application Information v2 (CI_AI_RES_V2_ID)
	CI_APP_TYPE_NET_INTERFACE = 0x04,	//
	CI_APP_TYPE_ACCESS_AIDS = 0x05,		//
	CI_APP_TYPE_UNCLASSIFIED = 0x06		//
} CI_APP_TYPE; // application_type in application_info() from CI_APPL_INFO_REPLY : 스펙에 정해진 값으로 enum 값 수정하지 말 것!

typedef struct {
	CI_APP_TYPE		nType;
	unsigned short	usManufacturer;
	unsigned short	usManufacturerCode;
	unsigned char	szText[MAX_CI_STR_LEN+1];
	unsigned char	ucIsInitialized;
} CI_APP_INFO; // from CI_APPL_INFO_REPLY

typedef enum {
	CI_CPLM_MORE = 0x00,			// not supported
	CI_CPLM_FIRST = 0x01,			// not supported
	CI_CPLM_LAST = 0x02,			// not supported
	CI_CPLM_ONLY = 0x03,			// supported
	CI_CPLM_ADD = 0x04,			// not supported
	CI_CPLM_UPDATE = 0x05			// supported
} CI_CA_PMT_LIST_MANAGEMENT; // ca_pmt_list_management in ca_pmt() or ca_pvr_pmt() : 스펙에 정해진 값으로 enum 값 수정하지 말 것!

typedef enum {
	CI_CPCI_OK_DSC = 0x01,		// supported
	CI_CPCI_OK_MMI = 0x02,		// not support
	CI_CPCI_QUERY = 0x03,			// not support -> CI+ 에선 CAM mandatory 사항이다
									// <TODO_CI/TODO_CI_PLUS> : 호스트 쪽에서도 Multiple CA application selection procedure 및 CA PVR 지원을 위해 구현 필요하다 (현재의 CI mgr 시나리오 수정 필요)
	CI_CPCI_NOT_SELECTED = 0x04	// supported but... 현재 잘못되게 사용되고 있으므로 수정을 요한다 -> 수정함 (_svc_cas_mgr_common.h 에 있는 define 과 그 주석 참고할 것)
									// 즉, "not_selected" is sent after a "query" or a "ok_mmi" in order to tell to the CA application that another CA application has been selected.
} CI_CA_PMT_CMD;					// ca_pmt_cmd_id in ca_pmt() or ca_pvr_pmt() : 스펙에 정해진 값으로 enum 값 수정하지 말 것!

typedef enum {
	CI_CE_DSC_POSSIBLE = 0x01,
	CI_CE_DSC_POSSIBLE_UNDER_PURCHASE_DIALOG = 0x02,
	CI_CE_DSC_POSSIBLE_UNDER_TECHNICAL_DIALOG = 0x03,
	CI_CE_DSC_NOT_POSSIBLE_NO_ENTITLEMENT = 0x71,
	CI_CE_DSC_NOT_POSSIBLE_TECHNICAL_REASONS = 0x73
} CI_CA_ENABLE; // CA_enable in ca_pmt_reply() or ca_pvr_pmt_reply() from CI_CA_PMT_REPLY or CI_CA_PVR_PMT_REPLY : 스펙에 정해진 값으로 enum 값 수정하지 말 것!

#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.h 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.

typedef enum {
	/* No CAS protected content recording supported:
	   The CA system has no capability for re-encoding the ECM and EMM and does not support play back of the original ECM and EMM,
	   therefore no assumptions can be made regarding the ability to play-back recorded CA protected content. */
	CI_CA_PVR_REC_NO_ECM_EMM_SUPPORTED = 0x00,
	/* Record original ECM & EMM supported:
	   CA protected content may be recorded but CA imposed time-restrictions on play-back may exist. The ECM and EMM PVR Commands are not supported in this mode. */
	CI_CA_PVR_REC_ORIGINAL_ECM_EMM_SUPPORTED = 0x01,
	/* Record re-encoded ECM & EMM supported:
	   CA protected content may be recorded together with re-encoded ECM messages without time-restrictions on play-back. */
	CI_CA_PVR_REC_RE_ENCODED_ECM_EMM_SUPPORTED = 0x02
} CI_CA_PVR_REC_MODE; // CA Recording Modes in ca_pvr_info() from CI_CA_PVR_INFO_REPLY : 스펙에 정해진 값으로 enum 값 수정하지 말 것!

typedef enum {
	CI_CA_PVR_REPLY_STATUS_OK = 0x00,
	CI_CA_PVR_REPLY_ERROR_BAD_SECTION = 0x01,
	CI_CA_PVR_REPLY_ERROR_CICAM_BUSY = 0x02 // If the CICAM replies this, the host shall retry 3 times to send the EMM or ECM command.
} CI_CA_PVR_REPLY_STATUS; // Reply Status Field in ca_pvr_emm_cmd_reply() or ca_pvr_ecm_cmd_reply() from CI_CA_PVR_EMM_CMD_REPLY or CI_CA_PVR_ECM_CMD_REPLY : 스펙에 정해진 값으로 enum 값 수정하지 말 것!

typedef enum {
	CI_CA_PVR_PINCODE_USED = 0x00,
	CI_CA_PVR_PINCODE_NOT_USED = 0x01,
	CI_CA_PVR_PINCODE_ERROR_BAD_PINCODE = 0x02,
	CI_CA_PVR_PINCODE_ERROR_CICAM_BUSY = 0x03
} CI_CA_PVR_PINCODE_STATUS; // PINcode_status_field in ca_pvr_PINcode_cmd_reply() from CI_CA_PVR_PINCODE_CMD_REPLY : 스펙에 정해진 값으로 enum 값 수정하지 말 것!

#endif

#if 0
typedef enum {
	CI_CA_SYSTEM_ID_ERROR = 0,
	CI_CA_DEVICE_ERROR,
	CI_CA_PMT_SENDED
} CI_CAS_STATE; // 사용되지 않는 예전 정의.
#else
typedef enum {
	CI_CAS_STATE_CA_PMT_REPLIED,				// CI_CA_PMT with "query" 에 대한 응답으로 CI_CA_PMT_REPLY 를 받으면...
	CI_CAS_STATE_CA_PVR_PMT_REPLIED,		// CI_CA_PVR_PMT 에 대한 응답으로 CI_CA_PVR_PMT_REPLY 받으면
	CI_CAS_STATE_CA_PVR_CAT_REPLIED,		// CI_CA_PVR_CAT 에 대한 응답으로 CI_CA_PVR_CAT_REPLY 받으면
	CI_CAS_STATE_CA_PVR_EMM_REPLIED,		// CI_CA_PVR_EMM_CMD 에 대한 응답으로 CI_CA_PVR_EMM_CMD_REPLY 받으면
	CI_CAS_STATE_CA_PVR_ECM_REPLIED,		// CI_CA_PVR_ECM_CMD 에 대한 응답으로 CI_CA_PVR_ECM_CMD_REPLY 받으면
	CI_CAS_STATE_CA_PVR_PINCODE_REPLIED,	// CI_CA_PVR_PINCODE_CMD 에 대한 응답으로 CI_CA_PVR_PINCODE_CMD_REPLY 받으면
	CI_CAS_STATE_ERROR
} CI_CAS_STATE; // CI_PLUS_PVR_ENABLE (CA PVR 지원) 및 Multiple CA application selection procedure 구현 용도로 재정의.
#endif

/********************************************************************
 Function   :	CI_NOTIFY_CAM_STATE_FUNC
 Description :	Notify CAM state
 Input      :	usSlotId - CI Slot ID
 				nCamState - CAM state
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int (*CI_NOTIFY_CAM_STATE_FUNC)(CI_SLOT_ID usSlotId, CI_CAM_STATE nCamState);

/********************************************************************
 Function   :	CI_NOTIFY_APP_INFO_FUNC
 Description :	Notify CAM application info.
 Input      :	usSlotId - CI Slot ID
 				pInfo - CAM application info. (static data)
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int (*CI_NOTIFY_APP_INFO_FUNC)(CI_SLOT_ID usSlotId, CI_APP_INFO *pInfo);

/********************************************************************
 Function   :	CI_NOTIFY_CA_INFO_FUNC
 Description :	Notify CA system IDs of CAM
 Input      :	usSlotId - CI Slot ID
 				usCaInfoCnt - Number of CA System IDs
 				pusCaSysId - CA System IDs info. (static data)
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int (*CI_NOTIFY_CA_INFO_FUNC)(CI_SLOT_ID usSlotId, unsigned short usCaInfoCnt, unsigned short *pusCaSysId);

#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.h 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.
/********************************************************************
 Function   :	CI_NOTIFY_CA_PVR_INFO_FUNC
 Description :	Notify CA system IDs and CA Recording Modes of CAM
 Input      :	usSlotId - CI Slot ID
 				usCaPvrInfoCnt - Number of CA System IDs and CA Recording Modes
 				pusCaSysId - CA System IDs info. (static data)
 				pucCaRecMode - CA Recording Modes info. (static data)
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int (*CI_NOTIFY_CA_PVR_INFO_FUNC)(CI_SLOT_ID usSlotId, unsigned short usCaPvrInfoCnt, unsigned short *pusCaSysId, CI_CA_PVR_REC_MODE *pucCaRecMode);
#endif

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
typedef int (*CI_NOTIFY_MMI_REQ_FUNC)(MMI_HANDLE hMmi, MMI_REQ nMmiReq, void *pvParam, unsigned long ulParamSize);

/********************************************************************
 Function   :	CI_NOTIFY_CAS_STATE_FUNC
 Description :	Notify CAS state
 Input      :	usSlotId - CI Slot ID
 				nCasState - CAS state
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int (*CI_NOTIFY_CAS_STATE_FUNC)(CI_SLOT_ID usSlotId, CI_CAS_STATE nCasState);


/* CI Extension callback functions list */
typedef struct {
	CI_NOTIFY_CAM_STATE_FUNC	pfnNotifyCamState;
	CI_NOTIFY_APP_INFO_FUNC		pfnNotifyAppInfo;
	CI_NOTIFY_CA_INFO_FUNC		pfnNotifyCaInfo;
#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.h 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.
	CI_NOTIFY_CA_PVR_INFO_FUNC	pfnNotifyCaPvrInfo;
#endif
	CI_NOTIFY_MMI_REQ_FUNC		pfnNotifyMmiReq;
	CI_NOTIFY_CAS_STATE_FUNC	pfnNotifyCasState;
} CI_NOTIFY_FUNC_LIST;


/***************************************************************
*
*
* global function prototypes : CI Extension API
*
*
***************************************************************/


/********************************************************************
 Function   :	CI_ResetSlot
 Description :	Reset CAM
 Input      :	usSlotId - CI Slot ID
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_ResetSlot(CI_SLOT_ID usSlotId);

/********************************************************************
 Function   :	CI_SetRS
 Description :	Reset CAM
 Input      :	usSlotId - CI Slot ID
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_SetRS(CI_SLOT_ID usSlotId);

/********************************************************************
 Function   :	CI_GetAppInfo
 Description :	Get CAM Application Info.
 Input      :	usSlotId - CI Slot ID
 Output     :	ppInfo - pointer of pointer of CAM application Info. data.
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_GetAppInfo(CI_SLOT_ID usSlotId, CI_APP_INFO **ppInfo);

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
 				pucCaPmt - pointer of ca_pmt() data to be sent to cam
 				usCaPmtLen - length of pucCaPmt. number of byte
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_SendCaPmt(CI_SLOT_ID usSlotId, unsigned char *pucCaPmt, unsigned short usCaPmtLen);

HERROR CI_CheckMmiInstance(MMI_HANDLE hMmi);

#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.h 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.
/********************************************************************
 Function   :	CI_SendCaPvrCmd
 Description :	send ca_pvr_pmt / ca_pvr_cat / ca_pvr_emm_cmd / ca_pvr_ecm_cmd / ca_pvr_PINcode_cmd to cam
 Input      :	usSlotId - CI Slot ID
 				pucCaPvrCmd - pointer of data or cmd to be sent to cam
 				usCaPvrCmdLen - length of pucCaPvrCmd. number of byte
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_SendCaPvrCmd(CI_SLOT_ID usSlotId, unsigned char *pucCaPvrCmd, unsigned short usCaPvrCmdLen);
#endif

/********************************************************************
 Function   :	CI_AddNotifyFunc / CI_DeleteNotifyFunc
 Description :	CI Extension add/delete callback APIs
 Input      :	notifyList - callback functions list to be added/deleted
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_AddNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList);
int CI_DeleteNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList);


#ifdef __cplusplus
}
#endif

#endif /* _CI_BASE_H_ */

