#ifndef _NA_GLOB_H_
#define _NA_GLOB_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <stb_params.h>
//#include <pal_pipe.h>
#include <_svc_cas_mgr_common.h>

#if defined(PID_NULL)
#undef PID_NULL
#endif
/* ETC */
#include "ca_pconv.h"
#include "ca_defs.h"
#include "ca_defsx.h"
#include "ca_dvb.h"
#include "ca_mpeg.h"
//#include "ca_dbg.h"

/* CAL API */
#include "ca_cak.h"
#include "cakapivr.h"
/* DLV API */
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#include "ca_dvl.h"
#include "prmapivr.h"
#endif
/* DAL API */
#include "dalapivr.h"
#include "ca_os.h"
#include "ca_icc.h"
#include "ca_bdc.h"
#include "ca_dmx.h"
#include "ca_rph.h"
#include "ca_sec.h"

#if !defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE) || defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#include "ca_dsc.h"
#if !defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE) && defined(CONFIG_MW_CAS_NAGRA_PRM)
#include "ca_scr.h"
#endif
#endif
#if defined (CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
#include "ca_vfs.h"
#endif
#if defined(CONFIG_MW_CAS_NAGRA_NOCS_3_X)
#include "ca_cert.h"
#endif

/* Gernerated By Humax */
#include "na_mgr_cak_mgr.h"
#include "na_mgr_cak_svc.h"
#include "na_mgr_cak_ext.h"
#include "na_mgr_cak_irdcmd.h"
#include "na_mgr_cak_ppv.h"

#include "ca_os_ext.h"
#include "ca_icc_ext.h"
#include "ca_bdc_ext.h"
#include "ca_dmx_ext.h"

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
#include "na_mgr_dvl_mgr.h"
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#include "na_ca_session_mgr.h"
#endif

#include "nagra_ctrl.h"

//#include "na_mgr_cak_pairingdata.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
//Identifier
#define NA_CAK_ID									0
#define NA_FILTER_ID								1
#define NA_SERVICE_ID								2
#define NA_APP_ID 									3
#define NA_TIMER_ID									4

//Tasks
#define NA_ICC_TASK_NAME							"iccT"
#define NA_PSI_TASK_NAME 							"psiT"
#define NA_CAK_TASK_NAME 							"cakT"
#define NA_PPV_TASK_NAME							"ppvT"

#if !defined(CONFIG_PROD_PROFILE_MINIOCTO)
#define NA_ICC_TASK_PRIORITY						(VK_TASK_PRIORITY_MW_REF + 2)
#define NA_PSI_TASK_PRIORITY 						(VK_TASK_PRIORITY_MW_REF + 3)
#else
#define NA_ICC_TASK_PRIORITY						(VK_TASK_PRIORITY_MW_REF + 1)
#define NA_PSI_TASK_PRIORITY 						(VK_TASK_PRIORITY_MW_REF + 2)
#endif
#define NA_CAK_TASK_PRIORITY 						(VK_TASK_PRIORITY_MW_REF + 0)

#define NA_ICC_TASK_STACK_SIZE					(2 * 1024)
#define NA_PSI_TASK_STACK_SIZE					(5 * 1024)
#define NA_CAK_TASK_STACK_SIZE					(5 * 1024)

//Message Queue
#define NA_ICC_MSG_Q_NAME							"iccQ"
#define NA_PSI_MSG_Q_NAME							"psiQ"
#define NA_CAK_MSG_Q_NAME							"cakQ"
#define NA_PPV_MSG_Q_NAME							"ppvQ"

//from Filter to Nagravision PSI task
#define FILTER_PSI_CAT_ARRIVAL						0
#define FILTER_PSI_PMT_ARRIVAL						1

//from Service to Nagravision PSI task
#define SERVICE_PSI_START_SERVICE					0
#define SERVICE_PSI_STOP_SERVICE					1
#define SERVICE_PSI_PAUSE_SERVICE					2
#define SERVICE_PSI_RESTART_SERVICE 				3
#define SERVICE_PSI_SET_SERVICE 					4
#define SERVICE_PSI_REQUEST_EMM 					5
#define SERVICE_PSI_STOP_EMM						6
#define SERVICE_PSI_SET_DSC							7
#define SERVICE_PSI_SET_PARENTAL_CONTROL			8

//from Nagravision CAK task to Nagravision PSI task
#define CAK_PSI_REGISTER_ECM						0
#define CAK_PSI_UNREGISTER_ECM					1
#define CAK_PSI_REGISTER_EMM						2
#define CAK_PSI_UNREGISTER_EMM					3

//from Nagravision CAK task to Nagravision CAK
#define CAK_ALARM_ACCESS							0
#define CAK_ACCESS_NOTIFICATION 					1
#define CAK_LOC_INFO_NOTIFICATION					2
#define CAK_SYSTEM_INFO_NOTIFICATION				3
#define CAK_LOC_INFO_CALL_BACK						4
#define CAK_SYSTEM_INFO_CALL_BACK					5
#define CAK_IRD_COMMAND 							6
#define CAK_EVENT_INFO_RESPONSE						7
#define CAK_IPPV_PURCHASE_RESPONSE					8
#define CAK_CREDIT_NOTIFICATION						9
#define CAK_CREDIT_CALL_BACK						10
#define CAK_EXPIRATION_CALL_BACK					11
#define CAK_REQUEST_PURCHASE_HISTORY				12
#define CAK_REQUEST_RECHARGE_LIST					13
#define CAK_RECHARGE_LIST_CALL_BACK					14
#define CAK_REQUEST_PACKAGE_LIST					15
#define CAK_PACKAGE_LIST_CALL_BACK					16
#define CAK_REQUEST_PACKAGE_INFO					17
#define CAK_REQUEST_PURCHASE_PACKAGE				18
#define CAK_PURCHASE_HISTORY_CALL_BACK				19
#define CAK_REQUEST_PRESENT_EVENT_CALL_BACK			20
#define CAK_REQUEST_FOLLOWING_EVENT_CALL_BACK		21
#define CAK_PPV_LOADING_LISTENER					22		// MHP_CA_API 용
#define CAK_PPV_LOADED_LISTENER						23
#define CAK_PPV_NIT_TIMER_CALL_BACK					24
#define CAK_PPV_PACKAGE_LIST_CALL_BACK				25
#define CAK_PPV_PACKAGE_TIMER_CALL_BACK				26
#define CAK_PPV_RESPONSE_NEW_RECHARGE				27
#define CAK_PPV_RESPONSE_NEW_OPPV					28
#define CAK_PPV_LIST_NOTIFICATION					29
#define CAK_PPV_LIST_CALL_BACK						30
#define CAK_EVENT_INFO_NOTIFICATION					31
#define CAK_PPV_EIT_TIMER_CALL_BACK					32
#define CAK_PPV_REQUEST_FULL_INFO_PACKAGE_LIST		33
#define CAK_PPV_MAKE_FULL_INFO_PACKAGE_LIST			34
#define CAK_PPV_PURCHASEHISTORY_TIMER_CALL_BACK		35
#define CAK_PPV_PURCHASEHISTORY_LISTENER			36
#define CAK_REQUEST_PROGRAM_DESCRAMBLE_CALL_BACK	37
#define CAK_REQUEST_SMARTCARDS_CALL_BACK			38
#define CAK_REQUEST_PROGRAMS_CALL_BACK				39
#define CAK_ACCESSRIGHT_LISTENER_NOTIFICATION		40
#define CAK_CA_POPUP_COMMAND						41

//from Filter to Nagravision CAK
#define CAK_FILTER_SDT_ARRIVAL						0
#define CAK_FILTER_EIT_PF_ARRIVAL					1
#define CAK_FILTER_NIT_ARRIVAL						2
#define CAK_FILTER_TDT_ARRIVAL						3
#define CAK_FILTER_PMT_ARRIVAL						4

//from Service change to Nagravision CAK
#define CAK_SERVICE_CHANGE_TP						0
#define CAK_SERVICE_CHANGE_CHANNEL				1
#define CAK_SERVICE_CHANGE_TP_TO_ANALOG			2

//request by app
#define CAK_LOC_INFO_REQ_ID						0
#define CAK_SYSTEM_INFO_REQ_ID					1
#define CAK_SMARTCARD_INFO_REQ_ID				2
#define CAK_PROGRAMS_INFO_REQ_ID				3
#define CAK_CREDIT_REQ_ID						4
#define CAK_TS_CHECK_REQ_ID						5
#define CAK_EVT_INFO_REQ_ID						6
#define CAK_PPV_LIST_REQ_ID						7
#define CAK_OPERATORS_REQ_ID					8
#define CAK_PARENTAL_CONTROL_REQ_ID				9

//addtional CA Alarm
#define CA_ALARM_ACCESS_CLEAR						(CA_ALARM_ACCESS_DENIED_COPY_PROTECTED + 5)
#if 0
#define CA_ALARM_DENIED_PAIRING_REQUIRED		(CA_ALARM_ACCESS_CLEAR + 1)
#define CA_ALARM_DENIED_CS_PAIRING_REQUIRED		(CA_ALARM_DENIED_PAIRING_REQUIRED + 1)
#endif

//addtional SC Alarm
#define CA_ALARM_SMARTCARD_REMOVED				(CA_SMARTCARD_STATE_NOT_CERTIFIED + 5)
#define CA_ALARM_SMARTCARD_END					(CA_ALARM_SMARTCARD_REMOVED + 1)

//mepg4 av stream
#define MPEG_TYPE_VIDEO_4							0x1B
#define MPEG_TYPE_AUDIO_4							0x11
#define MPEG_TYPE_DSCMCC_U_N						0x0B

// for define of max number of sc slot
#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
#define NO_SMC_SLOT								CONFIG_MW_CAS_NUM_SC_SLOT
#else
#define NO_SMC_SLOT								1
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
#define CARDLESS_SLOT_ID						(-1)
#endif

#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define EMM_HANDLE_ID							(NUM_MAX_ACTION)
#endif

// for PVR function
#if defined(CONFIG_MW_MM_PVR)
#define CONFIG_S_MULTI_TS_CAS					1
#else
#define CONFIG_S_MULTI_TS_CAS					0
#endif

// for test cak during no nagra cak lib
#define CONFIG_MW_CAS_NAGRA_TEST				0

// for PreDescramble Test
#define CONFIG_MW_CAS_NAGRA_PREDSC_TEST			0

// for DAL test
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define CONFIG_FOR_DAL_TEST_DEF					1
#else
#define CONFIG_FOR_DAL_TEST_DEF					0
#endif

// for getting COP data (1 - by DSC API, 0 - by CAK generic API)
// DSC API를 통해 얻는것이 좋을듯 함. 쉽다.. ㅋㅋㅋㅋ
#define CONFIG_DSC_METADATA						0

// if use PROTECT CW
#if 1//defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define CONFIG_PROTECT_CW						1
#else
#define CONFIG_PROTECT_CW						0
#endif

// if not use PMT Data from CAS_MGR
#define CONFIG_USE_RAW_PMT						0

// if support scramble playback
#define CONFIG_SCRAMBLE_PLAYBACK				0

// if support mulit audio track play
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_TIVU) || defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
#define MAX_NUM_OF_AUDIO_TRACK					3
#elif defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST) && defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
// CAK-MER-PSI-01 테스트항목에서 Live가 Service11이고 Instance Rec으로 Servie 12를 Record한후 Play할때 audio가 출력되지않는 문제로인해 수정함.
#define MAX_NUM_OF_AUDIO_TRACK                                  2
#else
#define MAX_NUM_OF_AUDIO_TRACK					1
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
//#define CAS_ZAP_LOG // hgs-1000s channel 전환시 kill되는 문제로인해 임시 로그 삽입, 추후 삭제할것임. gcan
#endif

typedef struct tagNaCasMsg
{
	TUnsignedInt16 	wMsg;
	TUnsignedInt32 	hWnd;
	TUnsignedInt32 	info;
	TUnsignedInt32 	lParam1;
	TUnsignedInt32 	lParam2;
	TUnsignedInt32 	lParam3;
} NaCasMsg_t;


#ifdef WIN32
#define CAS_NA_JFFS2_PATH				"/nagra/"
#else
#define CAS_NA_JFFS2_PATH				CAS_JFFS2_PATH "/nagra/"
#endif

/* SEC 5에서 BRCM에서 지원하는 Key loaded notify를 사용할 경우 아래 define을 살리자. 하지만 현재는 필요 없다. 2013.12.18 ssgoo*/
//#define	_NA_PRM_KEY_NOTIFY_FROM_NEXUS_
#if defined(CONFIG_SUPPORT_NOVA_APP) && defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
/* PRM PVR을 구현하기위해서 Nexus로부터 key를 load한후 Nova로 key를 전달하도록 한다. */
#define _NA_PRM_PVR_KEY_NOTIFY_FRM_NEXUS_
#endif

/*
	HD9000i의 경우 PRM 3.1로 시작했으나 Nagra가 관련 준비가 덜 되는 바람에 Library는 DVL 2.6 (PRM 3.1 용)을  그래도 사용하고
	PRM 2.6 시나리오와 DVL 2.5 API를 사용하도록 하여 아래와 같은 config를 추가 함. 2013.12.20, Seong-Seo, Ku.
*/
#if defined(CONFIG_PROD_HD9000I)|| defined(CONFIG_PROD_HGS1000S)
#define	__FORCED_USE_PRM_2_6__
#endif

#define MAX_PRMKEY_LENGTH		(128)	// all key size <= 16 byte (document : SEC_API_050101.chm)

typedef enum
{
	ePRM_KEYTYPE_NONE = 0,
	ePRM_KEYTYPE_CLEARTEXTKEY,
	ePRM_KEYTYPE_USECERTKEY,
	ePRM_KEYTYPE_SET0LEVELPROTECTEDKEY,
	ePRM_KEYTYPE_SET1LEVELPROTECTEDKEY,
	ePRM_KEYTYPE_SET2LEVELPROTECTEDKEY,
} NaPrmKeyType_e;

typedef struct tagNaPrmKeyInfo
{
	NaPrmKeyType_e		eKeyType;
	TUnsignedInt16 			usEmi;
	TTransportSessionId		xTSid;
	TUnsignedInt32			ulPlaybackHandle;
#if defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	TUnsignedInt32			ulKeyHandler;
	TUnsignedInt32			ulKeyIdSize;
	TUnsignedInt8			*pucKeyId;
#endif
	TUnsignedInt8			ucKey[MAX_PRMKEY_LENGTH];
	TUnsignedInt32			ulKeyLength;
} NaPrmKeyInfo_t;

typedef void (*PrmKeyInfo_Calback_Function )(void *pKeyData);

typedef struct tagNaPrmKeyInfoCallback
{
	TTransportSessionId				xTSid;
	TUnsignedInt32					ulPlaybackHandle;
	PrmKeyInfo_Calback_Function		pfnKeyDataCallback;
} NaPrmKeyInfoCallback_t;

#if defined(_NA_PRM_PVR_KEY_NOTIFY_FRM_NEXUS_)
typedef struct tagNaPrmPvrKeyInfo
{
	TTransportSessionId		xTSid;
#if defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	TUnsignedInt32			ulKeyHandler;
	TUnsignedInt8			ucKey[128];	// max key size == 16 byte
	TUnsignedInt32			ulKeyLength;
#endif
} NaPrmPvrKeyInfo_t;
#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR	CA_NA_DmxInitialization(void);
extern HERROR	CA_NA_DscInitialization(void);
extern HERROR	CA_NA_CakInitialization(void);
extern HERROR 	CA_NA_PsiInitialization(void);
extern HERROR	CA_NA_IccInitialization(void);
extern HERROR 	CA_NA_OsInitialization(void);

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
extern HERROR	CAS_NA_SecInitialize(void);
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
extern HERROR	CAS_NA_SecRegisterKeyDataCallback(TTransportSessionId xTSid, HUINT32 ulPlaybackHandle, PrmKeyInfo_Calback_Function fnKeyDataCallback);
#endif
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) //&& defined(CONFIG_MW_CAS_NAGRA_PRM_2_6)
extern HERROR CAS_NA_ScrExtRegisterKeyDataCallback(TTransportSessionId xTSid, HUINT32 ulPlaybackHandle, PrmKeyInfo_Calback_Function fnKeyDataCallback);
extern HERROR CAS_NA_ScrExtUnRegisterKeyDataCallback(void);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_NOCS_3_X)
extern HERROR	CAS_NA_CertInitialize(void);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
extern HERROR	CAS_NA_VfsInitialize(void);
#endif


#if defined(CONFIG_DEBUG)
HCHAR *NAMGR_GetStrSmartcardState(TSmartcardState eSmartcard);
HCHAR *NAMGR_GetStrAccessState(TCaAccess eAccess);
HCHAR *NAMGR_GetStrCasActionType(SvcCas_ActionType_e eActType);
#endif

#endif /* _NA_GLOB_H_ */

