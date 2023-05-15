/*
 $Header:   //BASE/archives/NEO-HEPVR/NEO/mw/cas/ir3/include/ir_api.h-arc   1.15   Jun 21 2007 09:51:00   choism  $
 */
/********************************************************************
 * $Workfile:   ir_api.h  $
 * Project: 	NEO Iredeto
 * Author:		Haksu Kwon
 * Description:	API for Neo Irdeto
 *
 *                              Copyright (c) 2003 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef     _IR_API_H_
#define     _IR_API_H_

/********************************************************************
 Header Files
 ********************************************************************/

#include <octo_common.h>
#include <svc_cas.h>

#include "ir_platform.h"

#include "ir_msg.h"

#include "ir_loader.h"

//#include "_svc_cas_mgr_common.h"


/********************************************************************
 Constant/Macro Definition
 ********************************************************************/
//#define	API_IR_OK		0
//#define	API_IR_ERR		-1

#define IRDRV_NULL_PID	(0x2000) // ewlee 20100817

#define INVALID_SOFTCELL_SVCHANDLE	(0xFFFF)
#define INVALID_DSC_HANDLE 				(0xFFFFFFFF)
#define INVALID_TS_UNIQUE_ID			(0xFFFF)


#define	IR_MAIL_VALID_OK							0xFFFF	// 16bit align
#define	IR_MAIL_VALID_NOT_OK						0x0000

#define	IR_MAIL_READ_STATUS_READ					0x0000
#define	IR_MAIL_READ_STATUS_NOT_READ				0xFFFF	// 16bit align

#define	IR_TEXT_MESSAGE_TYPE_MAILBOX				0x00
#define	IR_TEXT_MESSAGE_TYPE_ANNOUNCEMENT			0x10

#define	IR_EVT_DECODER_MSG_FORCE_EMMDOWN						0xF11	// EV_ForceEMMDown
#define	IR_EVT_DECODER_MSG_DOWNLOAD_ALLOWED_ENABLE			    0xF12	// EV_DownloadAllowed with flag 1
#define	IR_EVT_DECODER_MSG_DOWNLOAD_ALLOWED_DISABLE				0xF13	// EV_DownloadAllowed with flag 0
#define	IR_EVT_DECODER_MSG_CHANGE_PINCODE						0xF14	// EV_ChangePincode
#define	IR_EVT_DECODER_MSG_FINGER_PRINTING						0xF20	// CA_UI_MSG_Idx_IdDecode

#define	MAX_NUM_ECM_MONITOR_PID		8

#ifdef CONFIG_MW_CAS_NUM_SC_SLOT
#define CAS_MAX_NUM_OF_SC				CONFIG_MW_CAS_NUM_SC_SLOT
#else
#define CAS_MAX_NUM_OF_SC				0
#endif

#define IR_RECORD_TYPE_INVALID			0x00
#define IR_RECORD_TYPE_CLEAR			0x01
#define IR_RECORD_TYPE_DESCRAMBLE	0x2A
#define IR_RECORD_TYPE_ENCRYPTION		0x2B
#define IR_RECORD_TYPE_FORBIDDEN		0x2C //MV:3
#define IR_RECORD_TYPE_CHECKMSK		0x2D
#define IR_RECORD_TYPE_NOMSK			0x2E

#define	IR_RECORD_TYPE_DRM				0xC0
#define	IR_RECORD_TYPE_DRM_ERROR		0xC1
#define IR_RECORD_ID_INVALID			0xFFFFFFFF

#define IR_CAS_TYPE_SCELL			0x00
#define IR_CAS_TYPE_UC				0x01
#define IR_CAS_TYPE_BOTH			0x02	// ewlee 20100816 - this definition might not be used
#define IR_CAS_TYPE_NONE			0x03

/********************************************************************
 Structure and Type Definition
 ********************************************************************/

typedef enum
{
	IREVT_CLASS = 0,
	IREVT_CLASS_SVC_DESCRAMBLE_START  ,
	IREVT_CLASS_SVC_DESCRAMBLE_OPEN,
	IREVT_CLASS_SVC_DESCRAMBLE_DEFINED,
// Scrambel/Descramble 관련
	IREVT_CLASS_CAS_ERROR_FOR_SERVICE= 0x100,
	IREVT_CLASS_CAS_ERROR_FOR_GLOBAL,
	IREVT_CLASS_CAS_DESCRAMBLED_WELL,
	IREVT_CLASS_CAS_FTA_SERVICE,

// Decoder Data EMM 관련
	IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT= 0x200,
	IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT,
	IREVT_CLASS_DECODER_MSG_EMM_FINGERPRINT,
	IREVT_CLASS_DECODER_MSG_FORCED_DOWNLOAD,
	IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_ENABLE,
	IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_DISABLE,

	 // PC, Installer pin이 같으므로, 모두 같은 EVT로 처리 한다. SOPINC(pisys resouce에서 defult pin이 변경 가능하다. , SCPINC, GOPINI
	IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE,
	IREVT_CLASS_DECODER_MSG_NORMAL_MAIL,
	IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY,
	IREVT_CLASS_DECODER_MSG_MENU_UPDATE,
	IREVT_CLASS_DECODER_MSG_MENU_STATE,
	IREVT_CLASS_DECODER_MSG_FORCE_TUNE,

	IREVT_CLASS_DECODER_MSG_RESET_IRD,
	IREVT_CLASS_DECODER_MSG_RESET_CH_ORDER,
	IREVT_CLASS_DECODER_MSG_RESCAN,
	IREVT_CLASS_DECODER_MSG_RESCAN_FROM_HOME,
	IREVT_CLASS_DECODER_MSG_RECONNECT,

	IREVT_CLASS_DECODER_MSG_CA_VIRTUAL_BILL,
	IREVT_CLASS_DECODER_MSG_CA_DM_PARAMETERS,
	IREVT_CLASS_DECODER_MSG_CA_FEATURE_BITMAP,
	IREVT_CLASS_DECODER_MSG_CA_IPPV_IP_AND_PHONE_NO,
	IREVT_CLASS_DECODER_MSG_CA_IPPV_DEBIT_LIMIT_THRESHOLD,
	IREVT_CLASS_DECODER_MSG_CA_IPPV_SCHEDULE,
	IREVT_CLASS_DECODER_MSG_CA_REPORT_STATUS,
	IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE,

	IREVT_CLASS_DECODER_MSG_EPG_DM_SETUP,
	IREVT_CLASS_DECODER_MSG_EPG_FORCED_STANDBY,
	IREVT_CLASS_DECODER_MSG_EPG_DISPLAY_CARD_NUMBER,
	IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG,
	IREVT_CLASS_DECODER_MSG_EPG_REMOTE_RECORD,
	IREVT_CLASS_DECODER_MSG_EPG_DEL_PVOD_EVENT,
	IREVT_CLASS_DECODER_MSG_EPG_CLEAR_3PA_DATA,
	IREVT_CLASS_DECODER_MSG_EPG_COLOUR_DISPLAY,
	IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO,
	IREVT_CLASS_DECODER_MSG_EPG_PPV_PHONE_NO,
	IREVT_CLASS_DECODER_MSG_EPG_UPDATE_HDD,
	IREVT_CLASS_DECODER_MSG_EPG_FORMAT_HDD,
	IREVT_CLASS_DECODER_MSG_EPG_EPG_FEATURES,
	IREVT_CLASS_DECODER_MSG_EPG_USER_FOLDER_SETTINGS,

	IREVT_CLASS_DECODER_MSG_EPG_SET_OTHER_MODE,
	IREVT_CLASS_DECODER_MSG_EPG_EASY_SMS_DISPLAY,
	IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG_DS265,
	IREVT_CLASS_DECODER_MSG_EPG_DIGIAKTIF,
	IREVT_CLASS_DECODER_MSG_EPG_SET_FTA_LIMIT,

	IREVT_CLASS_DECODER_MSG_PVR,
#if defined(CONFIG_OP_UPC)
	IREVT_CLASS_DECODER_MSG_USAGE_ID,
#endif
// Conditional Access Menu관련
	IREVT_CLASS_SOFTCELL_MENU_NEED_TO_UPDATE = 0x300,
	IREVT_CLASS_SMARTCARD_MENU_NEED_TO_UPDATE,

// Smartcard nationality change건.
	IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_EMM = 0x400,
	IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_SC_OUT,

// Smartcard nationality change건.
	IREVT_CLASS_SOL_STATUS_CHANGED = 0x500 ,
	IREVT_CLASS_FTA_BLOCK_STATUS_CHANGED,

// to report result of MR configuration query
	IREVT_CLASS_MR_CONFIG_UPDATE = 0x600,
	IREVT_CLASS_MR_PIN_STAUS_UPDATE,// to triggering pin window regarding Maturity rating
	IREVT_CLASS_MR_PF_STAUS_UPDATE, // to triggering pin window regarding Maturity rating
	IREVT_CLASS_MR_SC_CAPABLITY_UPDATE,


	IREVT_CLASS_ECM_MONITOR_UPDATE =0x700,
	IREVT_CLASS_EMM_MONITOR_UPDATE,
#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
	IREVT_CLASS_ECM_MONITOR_PREVIEW_TIME,
#endif

	IREVT_CLASS_HOMING_CH_UPDATE = 0x800,

	IREVT_CLASS_MAX = 0xFFFF //the range should be in 2 byte size.
}IREVT_CLASSe;

typedef enum
{
	IRMV_STATUS_FTA,					//	FTA clear 방송, Recording에 Irdeto관여 안함.
	IRMV_STATUS_CLEAR,					//	Irdeto가 관여 하였으나 PVR Service가 필요없음 not ENCRYPTION
	IRMV_STATUS_ENCRYPTION,				//	Irdeto가 Encryption KEY를 부여하였음.
	IRMV_STATUS_NOMSK,					//	Encryption을 위해 SCELL을 구동하였으나 MSK가 없어서 Recording이 안됨.
	IRMV_STATUS_FORBIDDEN				//	Irdeto가 해당 서비스의 Recording을 금지함.
}	eIR_MV_Status;

typedef enum {
	eIrDrvCaType_None = 0,
	eIrDrvCaType_Fta,
	eIrDrvCaType_irdeto,
	eIrDrvCaType_Other
} eIrDrvCaType;

typedef enum IR_MSG_e
{
	eIR_MSG_CLASS_CAS=0,
	eIR_MSG_CLASS_CAI,
	eIR_MSG_CLASS_APP,
	eIR_MSG_CLASS_PAL,
	eIR_MSG_CLASS_NUM
} eIR_MSG_CLASS_Type;

typedef enum {
// Service
	CASMGR_CAI_START_SERVICE = 0x0,
	CASMGR_CAI_STOP_SERVICE,
	CASMGR_CAI_SET_PID,
	CASMGR_CAI_ONE_PID,
	CASMGR_CAI_SET_PIDLIST,
	CASMGR_CAI_RES_CHANGED,
	CASMGR_CAI_VIEWTYPE_CHANGED,
	CASMGR_CAI_START_TS,
	CASMGR_CAI_STOP_TS,

// Device( tuner/av etc.. )
	CASMGR_DD_AV_STATUS = 0x10,
	CASMGR_DD_TUNER_STATUS,

// SI
	CASMGR_SI_CAT_UPDATE = 0x20,
	CASMGR_SI_PMT_UPDATE,
	CASMGR_SI_BAT_UPDATE,
	CASMGR_SI_NIT_UPDATE,
	CASMGR_SI_SDT_UPDATE,

// CASINST
	CASMGR_CAINST_SELECTDESELECT = 0x30,	// CAI --> CAS
	CASMGR_CAINST_CHANGED,		// CAS --> CAI

// APP
	CASMGR_APP_MENU_KIND = 0x40,

// PVR
	CASMGR_CAI_START_RECORD = 0x50,
	CASMGR_CAI_STOP_RECORD,
	CASMGR_CAI_START_PLAYBACK,
	CASMGR_CAI_STOP_PLAYBACK,

	CASMGR_IR_SETUP_RECORD,
	CASMGR_IR_SETUP_PLAYBACK,

//	CASMGR_IR_WRITE_MAIL, // in octo, it's changed with  flushing message directly.

} CAS_BASE_MSG_TYPE;

typedef	enum
{
	IR_SVC_LIVE,
	IR_SVC_PLAYBACK,
	IR_SVC_RECORD,
	IR_SVC_RECORD_BASE
} IR_SVCACTIONType_e;

#if defined(CONFIG_DEBUG)
/* for debug commd tool */
typedef enum
{
	IR_DBGTOOL_NO_ACT=0,
	IR_DBGTOOL_SCELL_INFO,
	IR_DBGTOOL_MR_STATUS,
	IR_DBGTOOL_MR_USER_READ_PF,
	IR_DBGTOOL_MR_USER_READ_ALL_PF,
	IR_DBGTOOL_MR_USER_SET_PF,
	IR_DBGTOOL_MR_USER_DELETE_PF,
	IR_DBGTOOL_MR_INPUT_PIN,
	IR_DBGTOOL_MR_CONFIG_READ,
	IR_DBGTOOL_MR_CONFIG_SET,
	IR_DBGTOOL_MR_SC_READ_STATUS,
	IR_DBGTOOL_VERITY_PLATFORM,

}IR_DBGTOOL_TYPE_e;
#endif


#define _______________SMARTCARD_STATUS__________________

#define	IR_SC_INFO_STATUS_LENGTH			50
#define	IR_SC_INFO_NUMBER_LENGTH			15
#define	IR_SC_INFO_VERSION_LENGTH			5
#define	IR_SC_INFO_NATIONALITY_LENGTH		4
#define	IR_SC_INFO_ID_LENGTH				20
#define	IR_SC_INFO_USERDATA_LENGTH			28
#define	IR_SC_INFO_TMS_DATA_LENGTH			32


typedef HUINT32	AXI_STRING_IDX;

typedef struct
{
	HUINT32		ucValid;
	HUINT8		CharSeverity;
	HUINT32		eSource;
	HUINT16		usStatus;
	HUINT32		eSeverity;
} IR_STATE;


typedef enum
{
	IR_HOME_CHANNEL_MVM_NO_ALLOWED	=	(0),
	IR_HOME_CHANNEL_MVM_EARLY 		=	(1),
	IR_HOME_CHANNEL_MVM_LATE		=	(2),
	IR_HOME_CHANNEL_MVM_MIDDLE		=	(3)
}IR_HOME_CHANNEL_MVM_e;

typedef enum
{
	IR_HOME_CHANNEL_FEC_1_2 =	(1),
	IR_HOME_CHANNEL_FEC_2_3 =	(2),
	IR_HOME_CHANNEL_FEC_3_4 =	(3),
	IR_HOME_CHANNEL_FEC_5_6 =	(4),
	IR_HOME_CHANNEL_FEC_7_8 =	(5)
}IR_HOME_CHANNEL_FEC_e;

typedef enum
{
	IR_HOME_CHANNEL_POS_EAST =	(0),
	IR_HOME_CHANNEL_POS_WEST =	(1)
}IR_HOME_CHANNEL_POS_e;

typedef enum
{
	IR_HOME_CHANNEL_POLAR_H 	=	(0),
	IR_HOME_CHANNEL_POLAR_V 	=	(1),
	IR_HOME_CHANNEL_POLAR_C_L 	=	(2),
	IR_HOME_CHANNEL_POLAR_C_R	=	(3)
}IR_HOME_CHANNEL_POLAR_e;

typedef enum
{
	IR_HOME_CHANNEL_MOD_SYS_DVB_S 	=	(0),
	IR_HOME_CHANNEL_MOD_SYS_DVB_S2 	=	(1)
}IR_HOME_CHANNEL_MOD_SYS_e;

typedef enum
{
	IR_HOME_CHANNEL_ROLL_35 	=	(0),
	IR_HOME_CHANNEL_ROLL_25 	=	(1),
	IR_HOME_CHANNEL_ROLL_20 	=	(2)
}IR_HOME_CHANNEL_ROLL_OFF_e;

typedef enum
{
	IR_HOME_CHANNEL_MOD_RSV 	=	(0),
	IR_HOME_CHANNEL_MOD_QPSK 	=	(1),
	IR_HOME_CHANNEL_MOD_8PSK 	=	(2),
	IR_HOME_CHANNEL_MOD_16QAM	=	(3) /* DVB-S2일경우에는 Reserved value */
}IR_HOME_CHANNEL_MOD_TYPE_e;

typedef struct
{
	HUINT32						dwFreq;
	HUINT32						dwSymbolRate;
	IR_HOME_CHANNEL_MVM_e		eMuxVerifierMode;
	IR_HOME_CHANNEL_FEC_e		eFECInner;
	IR_HOME_CHANNEL_POS_e		eWestEastFlag;
	IR_HOME_CHANNEL_POLAR_e		ePolar;
	IR_HOME_CHANNEL_ROLL_OFF_e	eRollOff;
	IR_HOME_CHANNEL_MOD_SYS_e	eModulSystem;
	IR_HOME_CHANNEL_MOD_TYPE_e	eModulType;
	HUINT16						wOrbitPos;
#if defined (CONFIG_OP_ALMAJD)
	HUINT8							ucPriority;
#endif
} IR_SC_HOME_CHANNEL_INFO;

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
typedef enum
{
	/* Followings are bit wised type definition */
	IR_MR_PIN_NOTIFY_NOT_NEED =	(0),
	IR_MR_PIN_NOTIFY_E138=		(1),
	IR_MR_PIN_NOTIFY_E136	=	(IR_MR_PIN_NOTIFY_E138<<1),
	IR_MR_PIN_NOTIFY_E137	=	(IR_MR_PIN_NOTIFY_E138<<2)
}IR_MR_PIN_NOTIFY_TYPE_e;

typedef enum
{
	IR_MR_CONFIG_OK,
	IR_MR_CONFIG_CARD_ERROR,
	IR_MR_CONFIG_CARD_MR_INCAPABLE,
	IR_MR_CONFIG_PIN_WRONG,
	IR_MR_CONFIG_PIN_BLOCK,
	IR_MR_CONFIG_DISABLE_MR_NOT_ALLOWED
} IR_MR_Config_Status_e;

typedef struct
{
	HUINT8                      bProfileID;
	HUINT8	                    bUserAge;
	HUINT8                      abUserPin[ 2 ];
} IR_Msg_Mr_User_Profile_Data_St;

typedef struct
{
	/* it's called smartcards's Subscriber pin or master pin .*/
	/* and it's used to making convenient procedure for read user profile at once */
	/*it's only used in humax internal process */
	HUINT8	abSubscriberPin[2];

	/*Card capablity */
	HUINT8	bSCSupportMR; 			/* Whether the smartcard supports Maturity Rating.*/

	/*MR Configuration */
	HUINT8						bEnabledMREval;	 /* Whether enable MAturity Rating evaluation on the smartcard.*/
	IR_MR_Config_Status_e		eConfigResult;

	/* MR User Profiles */
	HUINT8							ucUserProfileResult;
	IR_Msg_Mr_User_Profile_Data_St 	ast_mr_user_profile[6]; // user profile id's value range is 0 to 5.

}IR_MATURITYRATING_INFO;
#endif

typedef struct
{
	HUINT8    bSector;
	HBOOL    fSectorValid;
	HUINT16  wSectorDateCode;
	HUINT8    bProductCount;
}IR_EXTENDED_PRODUCT_LIST_SECTOR_INFO;


typedef struct
{
	HUINT16  wProductId;
	HUINT16  wStartDate;
	HUINT8    bDuration;
}IR_EXTENDED_PRODUCT_LIST_PRODUCT_INFO;


typedef struct
{
	IR_EXTENDED_PRODUCT_LIST_SECTOR_INFO  stExtProductListSector;
	IR_EXTENDED_PRODUCT_LIST_PRODUCT_INFO  *p_ExtProductListPorduct;
}IR_EXTENDED_PRODUCT_LIST_INFO;


typedef struct
{
	IR_STATE	stStatus;
	char	szStatus[IR_SC_INFO_STATUS_LENGTH];
	HUINT8	ucValid;	// 0 - invalid, 1 - valid, it means DATA_REPLY consultlation was completed.

	char	szNumber[IR_SC_INFO_NUMBER_LENGTH];
	HUINT16	usType;
	char	szVersion[IR_SC_INFO_VERSION_LENGTH];

	HUINT8	ucBuild;
	HUINT8	ucVariant;
	HUINT16	usPatchLevel;
	HUINT16	usOwnerID;

	HUINT8	aucUserData[IR_SC_INFO_USERDATA_LENGTH];
	char	szNationality[IR_SC_INFO_NATIONALITY_LENGTH];
	char	szHashedID[IR_SC_INFO_ID_LENGTH];
#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
	IR_SC_HOME_CHANNEL_INFO stHomingInfo;
#endif
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	IR_MATURITYRATING_INFO stMRInfo;
#endif

        // 다음 정보는 Irdeto Product Status List 구현시 필요.
	IR_EXTENDED_PRODUCT_LIST_INFO stExtProductListData[MAX_SC_SECTOR_COUNT];
	HUINT8			nNumberOfSecotrs;

}IR_API_SC_INFO;

// ewlee 20100817 - move to here from ir_drv.h file
typedef enum
{
	eIR_COMPONENT_VIDEO=0,
	eIR_COMPONENT_AUDIO,
	eIR_COMPONENT_TELETEXT,
	eIR_COMPONENT_SUBTITLE,
	eIR_COMPONENT_MAX
}eIR_Component_Num;

// TODO: do not use followings.
typedef enum
{
	eTSClosed,
	eTSChanged,

}eIR_TS_EventType;

typedef enum
{
	eIR_SDT_RUNNING_STATUS0_UNDEFINED=0,
	eIR_SDT_RUNNING_STATUS1_NOT_RUNNING,
	eIR_SDT_RUNNING_STATUS2_START_SOON,
	eIR_SDT_RUNNING_STATUS3_PAUSING,
	eIR_SDT_RUNNING_STATUS4_RUNNING,
	eIR_SDT_RUNNING_STATUS5_RESERVED
}eIR_SDT_RUNNING_STATUS;

#define CASMGR_ONEPID_CHANGE		0
#define CASMGR_ONEPID_ADD			1
#define CASMGR_ONEPID_DELETE		2

typedef struct
{
	HUINT16 		usPID;
	HUINT8			ucType;
} IR_DRV_PID_INFO;

typedef struct tCAS_CAI_IF_msgTS
{
	HUINT16	usTsUniqueId;
	Handle_t	hAction; //for emm fileter.
} CAS_CAI_IF_msgTS;

	//PID : add/delete/start service
typedef struct tCAS_CAI_IF_msgPID
{
	Handle_t	hIrSvc;
	Handle_t	hAct;
	Handle_t	hSvc;
	HUINT16	usTsUniqueId;
	SvcCas_ActionType_e eActType;
	HUINT16	usPID_list[eIR_COMPONENT_MAX];
} CAS_CAI_IF_msgPID;


typedef struct tCAS_CAI_IF_msgPIDList
{
	Handle_t		hIrSvc;
	HUINT32		ulPidInfoCnt;
	IR_DRV_PID_INFO	astPidInfo[IR_SVC_MAX_PID];
} CAS_CAI_IF_msgPIDList;


typedef struct tCAS_PMT_MSG
{
	Handle_t	hIrSvc;
	HUINT16		usTsUniqueId;
	HUINT16		usLength;
	HUINT8		*pRawData;
} CAS_CAI_IF_msgPMT;


typedef struct tCAS_CAT_MSG
{
	Handle_t	hAction;
	HUINT16		usTsUniqueId;
	HUINT16		usLength;
	HUINT8		*pRawData;
} CAS_CAI_IF_msgCAT;

typedef struct tCAS_BAT_MSG
{
	Handle_t	hAction;
	HUINT16		usTsUniqueId;
	HUINT16		usLength;
	HUINT8		*pRawData;
} CAS_CAI_IF_msgBAT;

typedef struct tCAS_NIT_MSG
{
	Handle_t	hAction;
	HUINT16		usTsUniqueId;
	HUINT16		usLength;
	HUINT8		*pRawData;
} CAS_CAI_IF_msgNIT;

typedef struct tCAS_SDT_MSG
{
	Handle_t	hAction;
	Handle_t	hIrSvc;
	HUINT16		usTsUniqueId;
	HUINT16		usLength;
	HUINT8		*pRawData;
} CAS_CAI_IF_msgSDT;

typedef struct tCAS_CASINST_SELECT_MSG
{
	Handle_t	hIrSvc;
	HUINT32		ulCasInstanceId;
	HUINT8		IsSelect;	// 1 - Select , 0 - Deselect
	HUINT8		CloseFlag;	// ewlee 20110211 (1 - close service, default - fake close)
} CAS_CAI_IF_msgCAINST_SELECT;

typedef struct tCAS_VIEWTYPE_CHANGED_MSG
{
	Handle_t		hPlay;
	Handle_t		pipeHandle;
	HUINT8			IsStart;
	HUINT8			ucSource;
	HUINT8			ucOutput;
} CAS_CAI_IF_msgVIEWTYPE_CHANGED;

typedef struct tCAS_CAI_IF_msgPVR
{
//	Handle_t	hAction; //Handle_t	hPlay;
	Handle_t	hIrSvc;
	HUINT8		recordMode;
	HUINT16		errorIndex;
	HUINT32		ulStatus;
}CAS_CAI_IF_msgPVR;
// ewlee 20100817 - end


#define _______________SOFTCELL_SERVICE_STATUS__________________
typedef enum
{
	/* DVB services. */
	IR_SERVICE_DVB_DESCRAMBLE = 1,
	IR_SERVICE_DVB_EMM,

	/* IPTV services. */
	IR_SERVICE_IPTV_REGISTRATION_EMM,
	IR_SERVICE_IPTV_DESCRAMBLE_VOD,

	/* CypherCast services. */
	IR_SERVICE_CC_DESCRAMBLER ,
	IR_SERVICE_CC_OPERATOR,

	/* PVR services. */
	IR_SERVICE_PVR_RECORD,
	IR_SERVICE_PVR_PLAYBACK,
	IR_SERVICE_PVR_DRM,

	IR_SERVICE_IPDC_DESCRAMBLE,
	IR_SERVICE_IPDC_EMM,

	/* DMB services. */
	IR_SERVICE_DMB_DESCRAMBLE,
	IR_SERVICE_DMB_EMM,

	/* CMMB services. */
	IR_SERVICE_CMMB_DESCRAMBLE,
	IR_SERVICE_CMMB_EMM,

	IR_SERVICE_PVR_TSR_RECORD,
	IR_SERVICE_PVR_TSR_PLAYBACK
} IR_SVC_TYPE;

typedef struct
{
	Handle_t		hAction;
	HUINT16		usSCellSvcHandle;
	HUINT32		eSvcType;
	HUINT8		ucInstanceCount;
} IR_SVC_STATUS_INFO_MAIN;

typedef struct
{
	HUINT32		usRscID;
	char		CharSeverity;
	HUINT16		usEmmPID;
	HUINT32		eSource;
	HUINT16		usStatus;
	HUINT32		eSeverity;
} IR_RSC_STATE;

typedef struct
{
	HUINT16		usEsPID;
	HUINT16		usEcmOrEmmPID;
	char		CharSeverity;
	HUINT32		eSource;
	HUINT16		usStatus;
	HUINT32		eSeverity;
} IR_ELEM_STATE;

typedef struct		//	it must same with msg_dvb_emm_monitor_data_st
{
	HUINT16				wServiceHandle;
	HUINT32				wResourceId;
	HUINT8				bScStatus1;
	HUINT8				bScStatus2;
	HUINT16				wEmmPid;
	HUINT8				bAddressControl;
	HUINT8				abEmmPayload[ 3 ];
	HUINT32				wEmmCount;
} IR_EMM_MON_STATE;


typedef struct		//	it must same with msg_dvb_ecm_monitor_data_st
{
	HUINT16				wServiceHandle;
	HUINT32				wResourceId;
	HUINT8				bScStatus1;
	HUINT8				bScStatus2;
	HUINT16				wEcmPid;
	HUINT8				bPage;
	HUINT8				bVersion;
	HUINT8				bAlgorithm;
	HUINT8				bOddEven;
	HUINT32				fNextKey;
	HUINT8				abScReply[ 6 ];
	HUINT32				wEcmCount;
} IR_ECM_MON_STATE;

typedef struct
{
	HUINT16					usHandle;
	HUINT32					eSvcType;

	IR_STATE				GlobalStatus;
	IR_STATE				InternalStatus;

	IR_RSC_STATE			DvbDescramblerRscStatus;
	IR_RSC_STATE			EcmOrEmmRscStatus;
	IR_RSC_STATE			SmartcardRscStatus;
	IR_RSC_STATE			MacrovisionRscStatus;

	HUINT32					recordId;
	IR_RSC_STATE			SessionMngStatus;
	IR_RSC_STATE			SessionSubStatus;
	IR_RSC_STATE			CryptoOPStatus;

	HUINT16					EcmPID[CAS_MAX_NUM_OF_SC][MAX_NUM_ECM_MONITOR_PID];
	IR_ECM_MON_STATE		EcmMonitoring[CAS_MAX_NUM_OF_SC][MAX_NUM_ECM_MONITOR_PID];
	IR_EMM_MON_STATE		EmmMonitoring;

#if	defined(IRPLAT_PVR_DRM)
	HUINT32					ulCRID;
	HUINT32					ulRightsRecId;
	HUINT32					ulSeqRecId;
#endif
} IR_SVC_STATUS_INFO_EACH;

#define _______________MENU__________________
typedef enum
{
	IR_MENU_ALL_CLOSE = 1,
	IR_MENU_MAIN,
	IR_MENU_SEARCH,
	IR_MENU_STB_STATUS,
	IR_MENU_CA_STATUS,
	IR_MENU_SC_STATUS,
	IR_MENU_SOFTCELL_STATUS,
	IR_MENU_PARENTAL_CONTROL,
	IR_MENU_CH_LIST
}IR_MENU_KIND;

#define _____ECM_EMM_MONITORING_____
#define	IR_MONITOR_STOP		0
#define	IR_MONITOR_ECM		1
#define	IR_MONITOR_EMM		2


typedef enum {
	AXI_STATUSCODE_IDX_None = 0,
	// Service Unknown
	AXI_STATUSCODE_IDX_ServiceUnknownE37,
	// Service is not running
	AXI_STATUSCODE_IDX_ServiceIsNotCurrentlyRunningE38,
	// Locating Service
	AXI_STATUSCODE_IDX_LocatingServiceE39,
	// Decoder Memory Full
	AXI_STATUSCODE_IDX_DecoderMemoryFullE40,
	// Service is not currently available
	AXI_STATUSCODE_IDX_ServiceIsNotCurrentlyAvailableE41,
	// Parental Control Lock (and/or Ask for Parental PIN)
	AXI_STATUSCODE_IDX_ParentalControlLockE42,
	// Not allowed in this country
	AXI_STATUSCODE_IDX_NotAllowedInThisCountryE43,
	// No Event Information
	AXI_STATUSCODE_IDX_NoEventInformationE44,
	// Service not allowed
	AXI_STATUSCODE_IDX_ServiceNotAllowedE45,
	// Reading Satellite Information
	AXI_STATUSCODE_IDX_ReadingSatInformationE46,				// 10
	// Bouquet Block
	AXI_STATUSCODE_IDX_BouquetBlockE47,
	// No Signal
	AXI_STATUSCODE_IDX_NoSignalE48,
	// LNB Overload
	AXI_STATUSCODE_IDX_LNBOverloadE49,
	// No Services available
	AXI_STATUSCODE_IDX_NoServicesAvailableE50,
	// Searching for signal
	AXI_STATUSCODE_IDX_SearchingForSignalE52,
	// Invalid tuning parameters
	AXI_STATUSCODE_IDX_InvalidTuningParametersE64,
	// Service not scrambled
	AXI_STATUSCODE_IDX_ServiceNotScrambledE00,
	// Incorrect Pin
	AXI_STATUSCODE_IDX_IncorrectPINE53,
	// IPPV OK
	AXI_STATUSCODE_IDX_IPPVOKE54,
	// CAM not compatible
	AXI_STATUSCODE_IDX_CAMnotCompatibleE56,						// 20
	// Unknown Channel
	AXI_STATUSCODE_IDX_TODO_E57,
	// channel not currently available
	AXI_STATUSCODE_IDX_TODO_E58,
	// Service is currently blocked
	AXI_STATUSCODE_IDX_TODO_E66,
	// Please insert Nagravision smart card
	AXI_STATUSCODE_IDX_TODO_E67,
	// Nagravision smart card not inserted correctly
	AXI_STATUSCODE_IDX_TODO_E68,
	// Free-to-air service is blocked
	AXI_STATUSCODE_IDX_TODO_E69,
	// Please insert CA Module
	AXI_STATUSCODE_IDX_TODO_E01,
	// CA Module EEPROM Failure
	AXI_STATUSCODE_IDX_TODO_E02,
	// CA Module Failure
	AXI_STATUSCODE_IDX_TODO_E03,
	// Service is currently scrambled
	AXI_STATUSCODE_IDX_TODO_E14,								// 30
	// Service is currently scrambled
	AXI_STATUSCODE_IDX_TODO_E35,
	// Smartcard Not Compatible
	AXI_STATUSCODE_IDX_TODO_E36,
	// This service is unavailable on your second TV
	AXI_STATUSCODE_IDX_TODO_E70,
	// Song title and Artist information cannot be viewed
	AXI_STATUSCODE_IDX_TODO_E71,
	// Rescan in progress. Please wait
	AXI_STATUSCODE_IDX_TODO_E72,
	// TVLink Overload
	AXI_STATUSCODE_IDX_TODO_E73,
	// LNB1 Overload
	AXI_STATUSCODE_IDX_TODO_E74,
	// LNB2 Overload
	AXI_STATUSCODE_IDX_TODO_E75,
	// TV2 Disabled. Please contact customer services
	AXI_STATUSCODE_IDX_TODO_E76,

	// Message Errors
	// Bad Op-code or Sequence
	AXI_STATUSCODE_IDX_E100_2,									// 40
	// Incomplete Definition
	AXI_STATUSCODE_IDX_F101_2,
	// Format Error
	AXI_STATUSCODE_IDX_F102_2,

	// Service Status Messages
	// General
	// Service Ok
	AXI_STATUSCODE_IDX_D100_3,
	// Incomplete Definition
	AXI_STATUSCODE_IDX_E101_3,
	// DVB client
	// DVB EMM Service Ok
	AXI_STATUSCODE_IDX_D100_5,
	// Descrambler Service Ok
	AXI_STATUSCODE_IDX_D100_8,
	// PVR Record Service Ok
	AXI_STATUSCODE_IDX_D100_27,
	// PVR Playback Service Ok
	AXI_STATUSCODE_IDX_D100_28,
	// Card In
	AXI_STATUSCODE_IDX_D00_4,
	// Please insert Smartcard
	AXI_STATUSCODE_IDX_E04_4,									// 50
	// Unknown Smartcard
	AXI_STATUSCODE_IDX_E05_4,
	// Smartcard Failure
	AXI_STATUSCODE_IDX_E06_4,
	// Checking Smartcard
	AXI_STATUSCODE_IDX_I07_4,
	// EMM Source Ok
	AXI_STATUSCODE_IDX_D100_6,
	// No Match
	AXI_STATUSCODE_IDX_D101_6,
	// Out of EMM Filters
	AXI_STATUSCODE_IDX_E102_6,
	// No CAT
	AXI_STATUSCODE_IDX_D103_6,
	// No Vendor ID
	AXI_STATUSCODE_IDX_D104_6,
	// Out of Channels
	AXI_STATUSCODE_IDX_E105_6,
	// ECM Source Ok
	AXI_STATUSCODE_IDX_D100_9,									// 60
	// No PMT
	AXI_STATUSCODE_IDX_D101_9,

	AXI_STATUSCODE_IDX_I34_9,

	// Service is currently scrambled
	AXI_STATUSCODE_IDX_E16_4,
	AXI_STATUSCODE_IDX_E18_4,
	AXI_STATUSCODE_IDX_E19_4,
	AXI_STATUSCODE_IDX_E20_4,
	AXI_STATUSCODE_IDX_E21_4,
	// Service is currently descrambled
	AXI_STATUSCODE_IDX_D29_4,
	// Service is currently scrambled
	AXI_STATUSCODE_IDX_E30_4,
	// Viewing is temporarily blocked, Please stay tuned
	AXI_STATUSCODE_IDX_E32_4,									// 70
	// Invalid Smartcard
	AXI_STATUSCODE_IDX_E33_4,
	// Service is currently scrambled
	AXI_STATUSCODE_IDX_I34_4,
	AXI_STATUSCODE_IDX_E100_4,
	AXI_STATUSCODE_IDX_E101_4,
	// No Vendor ID
	AXI_STATUSCODE_IDX_D102_9,
	// Unspecified Status
	AXI_STATUSCODE_IDX_I102_4,
	// Service is currently scrambled
	AXI_STATUSCODE_IDX_D103_9,
	// Out of ECM Filters
	AXI_STATUSCODE_IDX_E104_9,
	// FTA Service
	AXI_STATUSCODE_IDX_D105_9,
	// Service is currently scrambled
	AXI_STATUSCODE_IDX_E106_9,									// 80
	AXI_STATUSCODE_IDX_E107_4,
	AXI_STATUSCODE_IDX_E108_4,
	// Reserved smartcard return code
	AXI_STATUSCODE_IDX_E109_4,	// E109 ~ E123
	// Smartcard error - Currently unable to view this channel
	AXI_STATUSCODE_IDX_E116_4,
	AXI_STATUSCODE_IDX_E117_4,
	AXI_STATUSCODE_IDX_E118_4,
	// Service is currently scrambled
	AXI_STATUSCODE_IDX_E124_4,
	// Descrambler Ok
	AXI_STATUSCODE_IDX_D100_10,
	// Cannot allocate descrambler
	AXI_STATUSCODE_IDX_E101_10,
	// Monitoring Enabled
	AXI_STATUSCODE_IDX_D100_11,
	// Not supported for this service
	AXI_STATUSCODE_IDX_E101_11,
	// Event Purchased
	AXI_STATUSCODE_IDX_D22_12,
	// preview Period
	AXI_STATUSCODE_IDX_D23_12,									// 90
	// Preview Period Expired
	AXI_STATUSCODE_IDX_E24_12,
	// Event already purchased
	AXI_STATUSCODE_IDX_D25_12,
	// Event is recorded
	AXI_STATUSCODE_IDX_D26_12,
	// No further purchases possible
	AXI_STATUSCODE_IDX_E27_12,
	// Credit Limit Reached
	AXI_STATUSCODE_IDX_E28_12,
	// Service is currently scrambled
	AXI_STATUSCODE_IDX_E17_13,
	// Ok
	AXI_STATUSCODE_IDX_D100_29,
	// No MSK
	AXI_STATUSCODE_IDX_E101_29,
	// No Subscription
	AXI_STATUSCODE_IDX_E102_29,
	// Session Record Storage Failure
	AXI_STATUSCODE_IDX_E103_29,									// 100
	// Session Record Data Error
	AXI_STATUSCODE_IDX_E104_29,
	// Ok
	AXI_STATUSCODE_IDX_D100_30,
	// PVR Encryption not available
	AXI_STATUSCODE_IDX_E101_30,
	// Key not Loaded
	AXI_STATUSCODE_IDX_D102_30,									//	104
	// Ok
	AXI_STATUSCODE_IDX_D100_31,
	// Resource unavailable
	AXI_STATUSCODE_IDX_E101_31,									// 106
	//Custom CA
	AXI_STATUSCODE_IDX_E80_35,
	AXI_STATUSCODE_IDX_E81_35,

	//marturity rating.
	AXI_STATUSCODE_IDX_E136_4,//109
	AXI_STATUSCODE_IDX_E137_4,
	AXI_STATUSCODE_IDX_E138_4,

	AXI_STATUSCODE_IDX_E129_4,				//112
	AXI_STATUSCODE_IDX_E133_4,

	//	recording is not allowed --> Forbidden <-- MacroVision에 대한 응답중 Forbidden에 대한 내용이 없음.
	AXI_STATUS_CODE_IDX_E135_4,				//	SmartCard Recording is not allowed 적당한것을 못찾겠음. 우선 이걸로 Forbidden 처리

	// HUMAX
	IR_EVT_F46,				// Pin code out
	IR_EVT_RecheckLock,		// recheck lock
	IR_EVT_RecheckCurrentState,		// recheck current state
	IR_EVT_MRPIN_ReCheck,			// Maturity Rating User Pin code Retry
	IR_EVT_Forbidden_Rec,	//	Forbidden Record
	IR_EVT_Block_FTA_Rec,	//	FTA이지만 recorde하지 못하는 경우. Digiturk에서만 사용

	//DRM - SOURCE_MOD_SESSION_MANAGEMENT_DRM
	AXI_STATUSCODE_IDX_E110_37,				//121
	AXI_STATUSCODE_IDX_E111_37,
	AXI_STATUSCODE_IDX_E112_37,
	AXI_STATUSCODE_IDX_I113_37,
	AXI_STATUSCODE_IDX_I114_37,
	AXI_STATUSCODE_IDX_E115_37,
	AXI_STATUSCODE_IDX_D116_37,
	AXI_STATUSCODE_IDX_E117_37,
	AXI_STATUSCODE_IDX_E118_37,

	//DRM - SOURCE_SMARTCARD
	AXI_STATUSCODE_IDX_D126_4,				//130

	AXI_STATUS_CODE_IDX_E127_4,
	AXI_STATUS_CODE_IDX_E128_4,
	AXI_STATUS_CODE_IDX_E130_4,
	AXI_STATUS_CODE_IDX_E131_4,
	AXI_STATUS_CODE_IDX_E132_4,
	AXI_STATUS_CODE_IDX_E134_4,

	// Subscriptions are expiring very soon
	AXI_STATUSCODE_IDX_I55_4,
	AXI_STATUSCODE_IDX_E501,	// Expired content delete message, Digiturk에서만 사용
	AXI_STATUSCODE_IDX_E120_4,

	AXI_STATUSCODE_IDX_E140_4,
	AXI_STATUSCODE_IDX_E145_4,
	AXI_STATUSCODE_IDX_E151_4,

#if 1//defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
	// Preview Test - Preview Time period time out.
	AXI_STATUS_CODE_IDX_E152_4,
#endif
	AXI_STATUSCODE_IDX_E154_4,
	AXI_STATUSCODE_IDX_D200_4,
	AXI_STATUSCODE_IDX_E201_4,

#if defined (CONFIG_OP_ALMAJD)
	AXI_STATUSCODE_IDX_ALMAJD_240,	// Key missed, 한동안 key update 가 안 일어 났다
	AXI_STATUSCODE_IDX_ALMAJD_254,	// pairing 된 card 가 아닌 다른 card 가 insert 되었다
	AXI_STATUSCODE_IDX_ALMAJD_255,	// 첫 생산 상태, pairing 이 필요 하다.
#endif

	IR_EVT_MAX
}AXI_STATUSCODE_IDX;

// ewlee 20100817 - status code
typedef enum
{
	IRUC_STATUS_MSG_NONE = 0,
	IRUC_STATUS_MSG_D029,
	IRUC_STATUS_MSG_D100,
	IRUC_STATUS_MSG_D101,
	IRUC_STATUS_MSG_D126,
	IRUC_STATUS_MSG_D127,
	IRUC_STATUS_MSG_E101,
	IRUC_STATUS_MSG_E102,
	IRUC_STATUS_MSG_E103,
	IRUC_STATUS_MSG_E104,
	IRUC_STATUS_MSG_E106,
	IRUC_STATUS_MSG_E016,
	IRUC_STATUS_MSG_E017,
	IRUC_STATUS_MSG_E030,
	IRUC_STATUS_MSG_E031,
	IRUC_STATUS_MSG_E032,
	IRUC_STATUS_MSG_E033,
	IRUC_STATUS_MSG_E055,
	IRUC_STATUS_MSG_E139,
	IRUC_STATUS_MSG_E140,
	IRUC_STATUS_MSG_E130,
	IRUC_STATUS_MSG_E128,
	IRUC_STATUS_MSG_E092,
	IRUC_STATUS_MSG_E131,
	IRUC_STATUS_MSG_E094,
	IRUC_STATUS_MSG_E095,
	IRUC_STATUS_MSG_E135
}IRUC_STATUS_CODE;


#if 0
#define	IR_EVT_DecTextMsg_TextMessage			0xF01	// CA_UI_MSG_Idx_TextMessage, CA_UI_MSG_Idx_ForcedMessage, CA_UI_MSG_Idx_AddrTextMsg
#define	IR_EVT_EnhancedTextmsg_TextMessage		0xF02

#define	IR_EVT_DecControlMsg_ForceEMMDown		0xF11	// EV_ForceEMMDown
#define	IR_EVT_DecControlMsg_DownloadAllowedEnable	0xF12	// EV_DownloadAllowed with flag 1
#define	IR_EVT_DecControlMsg_DownloadAllowedDisable	0xF13	// EV_DownloadAllowed with flag 0
#define	IR_EVT_DecControlMsg_ChangePincode		0xF14	// EV_ChangePincode
#define	IR_EVT_DecControlMsg_ForceTune			0xF15
#define	IR_EVT_DecControlMsg_ChangeNation		0xF16	// EV_ChangeNationality

#define	IR_EVT_DecIdMsg_FingerPrinting			0xF20	// CA_UI_MSG_Idx_IdDecode
#define IR_EVT_E29									0xE29

#define	IR_EVT_Restart_Audio						0xF34
#define IR_EVT_DecIdMsg_PVR_NoMsk				0xF35
#define IR_EVT_DecIdMsg_RecordForbidden			0xF36
#define IR_EVT_DecIdMsg_PVR_Encryption			0xF37

#define	IR_EVT_AppStatus_EEPROMDataInvalid		0xE09	// CA_UI_MSG_Idx_EEPROMDataInvalid
#define	IR_EVT_SvcStatus_ServiceFull				0xE08	// CA_UI_MSG_Idx_ServiceFull
#define	IR_EVT_SvcStatus_PMTFormatError		0xE10	// CA_UI_MSG_Idx_PMTFormatError
#define	IR_EVT_SvcStatus_UpdateEEPROM			0xE11	// CA_UI_MSG_Idx_UpdateEEPROM
#define	IR_EVT_SvcStatus_OutOfKeys				0xE12	// CA_UI_STR_Idx_ServiceStatus_OutOfKeys
#define	IR_EVT_SvcStatus_OutOfStreams			0xE13	// CA_UI_STR_Idx_ServiceStatus_OutOfStreams
#define	IR_EVT_SvcStatus_GeneralErrors			0xE15	// CA_UI_STR_Idx_ServiceStatus_GeneralErrors
#define	IR_EVT_SvcStatus_RSAFails				0xE18	// CA_UI_STR_Idx_ServiceStatus_RSAFails
#define	IR_EVT_SvcStatus_RegionBlackout			0xE20	// CA_UI_STR_Idx_ServiceStatus_RegionBlackout
#define	IR_EVT_SvcStatus_UserBitmapFilter		0xE21	// CA_UI_STR_Idx_ServiceStatus_UserBitmapFilter
#define	IR_EVT_SvcStatus_PreviewTime			0xE23	// CA_UI_STR_Idx_ServiceStatus_PreviewTime
#define	IR_EVT_SvcStatus_NoView				0xE24	// CA_UI_STR_Idx_ServiceStatus_NoView
#define	IR_EVT_SvcStatus_AlreadyPurchase		0xE25	// CA_UI_STR_Idx_ServiceStatus_AlreadyPurchase
#define	IR_EVT_SvcStatus_EventRecorded			0xE26	// CA_UI_STR_Idx_ServiceStatus_EventRecorded
#define	IR_EVT_SvcStatus_IPPVFull				0xE27	// CA_UI_STR_Idx_ServiceStatus_IPPVFull
#define	IR_EVT_SvcStatus_CreditLimit				0xE28	// CA_UI_STR_Idx_ServiceStatus_CreditLimit
#define	IR_EVT_SvcStatus_PKeyError				0xE31	// CA_UI_STR_Idx_ServiceStatus_PKeyError
#define	IR_EVT_SvcStatus_IncorrectSlot			0xE32	// CA_UI_STR_Idx_ServiceStatus_IncorrectSlot
#define	IR_EVT_SvcStatus_NoDetweakKey			0xE80
#define	IR_EVT_SvcStatus_InvalidFixKey			0xE81

#define	IR_EVT_SOLStatus_Changed				0xE90

#define	IR_EVT_MRConfigStatus_Changed			0xEA0
#define	IR_EVT_MRPinStatus_Changed			0xEA1
#define	IR_EVT_MRPF_Status_Changed			0xEA2
#define	IR_EVT_MRSCCapablity_Status_Changed	0xEA3
#endif

#if 0
typedef void(*IR_CB_BANNER_FUNC)(HINT32 nEvtcode, HUINT16 usParam, HUINT32 ulParam);
#endif

/* EMM TLV 를 받을 경우 TAG 식별 값 */
#define 	TLV_TAG_LPPV_TICKET					0x32
#define 	TLV_TAG_DEBIT_LIMIT					0x33
#define 	TLV_TAG_USER_INFO					0x17
#define 	TLV_TAG_IPPV_CONTROL				0xDC
/*~*/

/* Global Status를 한방에 알아오기 위한 상태 값 */
#define	IR_EVT_AppStatus_CardOk						0xF00	// Valid Smartcard
#define	IR_EVT_AppStatus_CardOut					0xE04	// CA_UI_MSG_Idx_CardOut
#define	IR_EVT_AppStatus_CardInvalid				0xE05	// CA_UI_MSG_Idx_CardInvalid
#define	IR_EVT_AppStatus_CardFault					0xE06
#define	IR_EVT_AppStatus_CardIn						0xE07	// CA_UI_MSG_Idx_CardIn
#define	IR_EVT_AppStatus_EEPROMDataInvalid			0xE09	// CA_UI_MSG_Idx_EEPROMDataInvalid

#define	IR_EVT_SvcStatus_FTAService					0xE00

#define	IR_EVT_SvcStatus_ServiceFull				0xE08	// CA_UI_MSG_Idx_ServiceFull
#define	IR_EVT_SvcStatus_PMTFormatError				0xE0A	// E10
#define	IR_EVT_SvcStatus_UpdateEEPROM				0xE0B	// E11
#define	IR_EVT_SvcStatus_OutOfKeys					0xE0C	// E12
#define	IR_EVT_SvcStatus_OutOfStreams				0xE0D	// E13
#define	IR_EVT_SvcStatus_GeneralErrors				0xE0F	// E15
#define	IR_EVT_SvcStatus_NoProductMatch				0xE10	// E16
#define	IR_EVT_SvcStatus_SectorError				0xE11	// E17
#define	IR_EVT_SvcStatus_RSAFails					0xE12	// E18
#define	IR_EVT_SvcStatus_ProductExpired				0xE13	// E19
#define	IR_EVT_SvcStatus_RegionBlackout				0xE14	// E20
#define	IR_EVT_SvcStatus_UserBitmapFilter			0xE15	// E21
//IPPV
#define	IR_EVT_SvcStatus_Purchased					0xE16	// E22
#define	IR_EVT_SvcStatus_PreviewTime				0xE17	// E23
#define	IR_EVT_SvcStatus_NoView						0xE18	// E24
#define	IR_EVT_SvcStatus_AlreadyPurchase			0xE19	// E25
#define	IR_EVT_SvcStatus_EventRecorded				0xE1A	// E26
#define	IR_EVT_SvcStatus_IPPVFull					0xE1B	// E27
#define	IR_EVT_SvcStatus_CreditLimit				0xE1C	// E28

#define	IR_EVT_SvcStatus_Descrambed					0xD1D	// CA_UI_STR_Idx_ServiceStatus_Descrambed
#define	IR_EVT_SvcStatus_FreeService					0xD64	// CA_UI_STR_Idx_ServiceStatus_Descrambed
#define	IR_EVT_SvcStatus_KeyMismatch				0xE1E	// CA_UI_STR_Idx_ServiceStatus_KeyMismatch
#define	IR_EVT_SvcStatus_PKeyError					0xE1F	// CA_UI_STR_Idx_ServiceStatus_PKeyError
#define	IR_EVT_SvcStatus_IncorrectSlot				0xE20	// CA_UI_STR_Idx_ServiceStatus_IncorrectSlot
#define	IR_EVT_SvcStatus_KeyMismatch33				0xE21
#define	IR_EVT_SvcStatus_UnknownStatus				0xE22	// CA_UI_STR_Idx_ServiceStatus_UnknownStatus
#define	IR_EVT_SvcStatus_ProductExpiryNoted			0xE37

#define IR_EVT_SvcStatus_IppvIncorrectPin			0xE35

#define	IR_EVT_SvcStatus_NotActivated				0xE5A //no product in the card.
#define	IR_EVT_SvcStatus_NoStreamMatch				0xE5F //unknown smartcard SCB.4 La7
//~

// LPPV
#define	IR_EVT_SvcStatus_LppvStoreFull			0xE57
#define	IR_EVT_SvcStatus_LppvNoOrder			0xE58
#define	IR_EVT_SvcStatus_LppvNoBuyWindow		0xE59

#define	IR_EVt_SvcStatus_LppvNoCredit			0xE5B
#define	IR_EVt_SvcStatus_LppvTooExpensive		0xE5C
#define	IR_EVt_SvcStatus_PpvParameterError		0xE5D
//~

#define	IR_EVT_DecTextMsg_TextMessage				0xF01	// CA_UI_MSG_Idx_TextMessage, CA_UI_MSG_Idx_ForcedMessage, CA_UI_MSG_Idx_AddrTextMsg
#define	IR_EVT_EnhancedTextmsg_TextMessage			0xF02

#define	IR_EVT_DecControlMsg_ForceEMMDown			0xF0B	// EV_ForceEMMDown
#define	IR_EVT_DecControlMsg_DownloadAllowedEnable	0xF0C	// EV_DownloadAllowed with flag 1
#define	IR_EVT_DecControlMsg_DownloadAllowedDisable	0xF0D	// EV_DownloadAllowed with flag 0
#define	IR_EVT_DecControlMsg_ChangePincode			0xF0E	// EV_ChangePincode
#define	IR_EVT_DecIdMsg_FingerPrinting				0xF14	// CA_UI_MSG_Idx_IdDecode
#define	IR_EVT_CaStatus_MonitorEmmEcm				0xF1E	// pfnEmmEcmUpdate
#define	IR_EVT_CaStatus_UpdateSmartCardInfo			0xF1F	// CA_UI_MSG_Idx_SmartCardInfo
#define	IR_EVT_CaStatus_UpdateTsInfo				0xF20	// CA_UI_MSG_Idx_TSInfo
#define	IR_EVT_EtcStatus_CloseBanner				0xF21	// UI_UTIL_CloseBanner( ) CAS_MODULE_STATUS_CLOSE
#define	IR_EVT_Restart_Audio						0xF22
#define	IR_EVT_AV_BLOCK								0xF23
#define	IR_EVT_AV_UNBLOCK							0xF24
#define	IR_EVT_SvcStatus_AV_Played					0xF25
#define	IR_EVT_SvcStatus_AV_NotPlayed				0xF26
/*~*/

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
typedef enum
{
	SOL_UNKNOWN_SERVICE,
	SOL_BLOCKED_SERVICE,
	SOL_VISIBLE_SERVICE
}SOL_SERVICE_STATE;
#endif

/********************************************************************
 Global Function Declaration
 ********************************************************************/
HUINT16 CAS_IR_DRV_Get16Bits(HUINT8 *base);
HUINT32 CAS_IR_DRV_Get24Bits(HUINT8 *base);
HUINT32 CAS_IR_DRV_Get32Bits(HUINT8 *base);
HUINT32	CAS_IR_DRV_Bcd2Dec(HUINT32 dwVal);

#define _____LOADER_STATUS_____
HINT32	xsvc_cas_IrLoaderInit(void);
HIR_ERR xsvc_cas_IrLoaderGetManuCode(HUINT8 *pucManCode);
HIR_ERR xsvc_cas_IrLoaderGetManuCodeString(HUINT8 *pucManCode);
#if 0
HIR_ERR XSVC_CAS_IR_LOADER_GetClientVersion(HUINT8 *pucClientVersion);
#endif
HIR_ERR xsvc_cas_IrLoaderGetHWCode(HUINT8 *pucHWCode);
HIR_ERR xsvc_cas_IrLoaderGetHWCodeString(HUINT8 *pucHWCode);
HIR_ERR xsvc_cas_IrLoaderGetVariantString(HUINT8 *pucVariant);
HIR_ERR xsvc_cas_IrLoaderGetSysIdString(HUINT8 *pucSysId);
HIR_ERR xsvc_cas_IrLoaderGetKeyVerString(HUINT8 *pucKeyVer);
HIR_ERR xsvc_cas_IrLoaderGetSigVerString(HUINT8 *pucSigVer);
HIR_ERR xsvc_cas_IrLoaderGetStatusInfo(IHL_STATUS *pstIRStatus);
HIR_ERR xsvc_cas_IrLoaderGetLoadSeqNum(HUINT8 *pucLDNum);
HIR_ERR	xsvc_cas_IrLoaderGetSignDate(HUINT16 *pusMJD);
HIR_ERR	xsvc_cas_IrLoaderGetSignTime(HxDATETIME_Time_t *pstTime);
HIR_ERR	xsvc_cas_IrLoaderGetDnldDate(HUINT16 *pusMJD);
HIR_ERR	xsvc_cas_IrLoaderGetDnldTime(HxDATETIME_Time_t *pstTime);
HIR_ERR xsvc_cas_IrLoaderGetSerialNum(HUINT32 *pSerNum);
HIR_ERR xsvc_cas_IrLoaderGetRandomNum(HUINT32 *pRandomNum);
HIR_ERR xsvc_cas_IrLoaderGetLoaderMajor(HUINT8 *pLoaderMajor);
HIR_ERR xsvc_cas_IrLoaderGetLoaderMinor(HUINT8 *pLoaderMinor);
HIR_ERR xsvc_cas_IrLoaderGetLoaderType(HUINT8 *pLoaderType);
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
HIR_ERR xsvc_cas_IrSecureChipGetCSSN(HUINT32 *pwCSSN);
#endif


HIR_ERR xsvc_cas_IrOtaSystemReboot(HUINT32 ulMiliSecMargin);

HIR_ERR xsvc_cas_IrOtaReadOTAInfo(HUINT16 *pusDownloadServicePID, HUINT8 *pucTableID);
HIR_ERR xsvc_cas_IrOtaClearOTAInfo(void);
HIR_ERR xsvc_cas_IrOtaClearOTATuningInfo(void);


HIR_ERR xsvc_cas_IrOtaSetFlagUp(HBOOL flagDoOTA, HUINT16 usDownloadServicePID, HUINT8 ucTableID);
HIR_ERR xsvc_cas_IrOtaReadChannelParam( HUINT32 ulTunerId, void * pvDiChannel_param);
HIR_ERR xsvc_cas_IrOtaSetChannelParam( HUINT32 ulTunerId, void * pvDiChannel_param);

#define _____MENU_____
#if 0
HINT32	API_IR_SetMenuKind(IR_MENU_KIND eMenuKind, HUINT16 usData);
#endif

#define _____MAIL_MESSAGE_____
HIR_ERR	xsvc_cas_IrGetMailList( HUINT8 *pucNumberOfSavedMail, HUINT8 *paucMailIndexList);
HIR_ERR	xsvc_cas_IrGetMailBox( HUINT8 ucMailIndex, MAILBOX_TABLE *pMailBox );
HBOOL	xsvc_cas_IrCheckValidOfMail( HUINT8 ucMailIndex);
HBOOL	xsvc_cas_IrGetMailReadState( HUINT8 ucMailIndex);
HIR_ERR	xsvc_cas_IrSetMailReadState( HUINT8 ucMailIndex, HBOOL bRead );
HUINT8	xsvc_cas_IrDoYouHaveUnreadMail(void);
HIR_ERR	xsvc_cas_IrDeleteMail(HUINT8   ucMailIndex);
void xsvc_cas_IrAllClearExpiredNormalAttributeMail(void);
//HIR_ERR API_IR_MAIL_EraseAll(void);

#define _____FINGER_PRINT_____
HIR_ERR	xsvc_cas_IrGetFingerPrintCode(HUINT8 ucScSlotId, HINT8 *pnCode);


#define ____MATURITY_RATING_____
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
HINT32 xsvc_cas_IrMrUpdatePIN(Handle_t hPlay, HUINT8 * aucDecimalPIN);
HINT32 xsvc_cas_IrMrReadAllUserPF(HUINT32 resourceId, HUINT8 * aucDecimalPIN);
HINT32 xsvc_cas_IrMrSetUserPF(HUINT8 ucUserIdx, HUINT8 ucAge, HUINT8 * aucDecimalPIN);
HINT32 xsvc_cas_IrMrDeleteUserPF(HUINT8 ucUserIdx, HUINT8 * aucDecimalPIN);
#endif


extern int API_IR_IsFTA(Handle_t hPlay);
#if defined(IRPLAT_SYS_PVR)
HUINT32	xsvc_cas_IrPvrIsRecordable (Handle_t hPlay);
HUINT32 xsvc_cas_IrPvrIsNoMSK (Handle_t hPlay);
HUINT32	xsvc_cas_IrPvrIsEncryption (Handle_t hPlay);
HUINT32	xsvc_cas_IrPvrIsDescrambled (Handle_t hPlay);
#endif

HIR_ERR	xsvc_cas_IrGetSvcCasKind(Handle_t hPlay, HUINT8 *ucSvcCasKind);

#if defined(CONFIG_OP_DIGITURK)
HERROR	xsvc_cas_IrSetStandbyFlag(HUINT8 ucFlag);
HERROR	xsvc_cas_IrSetLanguageCode(HUINT8 *szLangCode);
HERROR	xsvc_cas_IrSetErrorCode(HUINT32 ulErrorCode);
HERROR	xsvc_cas_IrGetErrorCode(HUINT32 *pulErrorCode);
#endif

HUINT16 xsvc_cas_IrGetSvcType(void *pHandle);

#define _____GLOBAL_____
AXI_STRING_IDX	xsvc_cas_IrDrvGetStringIdxFromErrorCode(IR_STATE *pStatus);

#define _____SMARTCARD_STATUS_____
HIR_ERR xsvc_cas_IrScInfo(HUINT8 ucScSlotId, IR_API_SC_INFO *pstIrApiScInfo);
HIR_ERR xsvc_cas_IrScNumber(HUINT8 ucScSlotId, HUINT8 *pNumber);

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
SOL_SERVICE_STATE	xsvc_cas_IrGetSolStatus(Handle_t hAction, HUINT16 ONID, HUINT16 TSID);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
HUINT32	xsvc_cas_IrGetFTABlockStatus(Handle_t hAction);
#endif

HUINT8	xsvc_cas_IrDrvCheckValidEvtCode(HINT32 nEvtCode);

void 	xsvc_cas_IrSendJumpEvent(void);
HERROR	xsvc_cas_IrGetUiData(HUINT8 *pucUiData, DxCAS_UiDataType_e eType);

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
HIR_ERR xsvc_cas_IrSecureChipGetCSSN(HUINT32 *pwCSSN);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
#if defined(CONFIG_OP_ALMAJD)
HUINT32 xsvc_cas_IrGetAlmajdHomingChannel (HUINT8 ucSlot, IR_SC_HOME_CHANNEL_INFO *pHomeChannel);
#endif
#endif

#if defined(CONFIG_OP_ALMAJD)
HUINT32 xsvc_cas_IrGetRegionCode (HUINT16 *pusRegionCode);
HUINT32 xsvc_cas_IrGetFirstBoot (HUINT8 *pucFirstBoot);
HUINT32 xsvc_cas_IrResetFirstBoot (void);
HUINT32 xsvc_cas_IrReadNationality (HUINT8 *pucNationality);
HUINT32 xsvc_cas_IrSaveNationality (HUINT8 *pucNationality);
#endif

#endif
/* end of file */

