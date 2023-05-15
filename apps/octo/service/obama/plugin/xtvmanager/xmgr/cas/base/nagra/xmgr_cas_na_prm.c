/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <tapi.h>
#include <mgr_action.h>
#include <mgr_drmagent.h>
#include <mgr_media.h>

#include <xmgr_cas.h>
#include <namgr_main.h>
#include <svc_output.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"
#include "local_include/_xmgr_cas_na_prm.h"

#include "mgr_live.h"
#include "_svc_cas_mgr_svc_port.h"
#if 0//defined(NEW_GETCAINFO)
#else
#if defined(CONFIG_SUPPORT_NOVA_APP)
#include "om_common.h"
#endif
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/
#define	SUPPORT_TIME_SETTING	/* DVL integration Test를 위해 편의상 임시로 추가 */

// 아래는 On 되어야함.
//#define	SUPPORT_ONLY_ONE_CHIPSYSINFORMATION	// chipset info와 system info를 HD+Portal 진입하면서 한번만 수신한다.
//#define	SUPPORT_IGNORE_ADVERTISEMENT			// 광고는 무시한다.

#define	SUPPORT_USAGERULE
#if defined(SUPPORT_USAGERULE)
#if defined (CONFIG_PROD_HD9000I)
#define SUPPORT_USAGERULE_RECOVERY			// pstop시 video/audio output 관련해서 원복해야한다. ( 모두 default 로 적용한다 )
#define SUPPORT_META_VARIATION			//HD+ service와 같이 SMD를 항상 보내도록 요구 되어지지 않을 경우, Data가 있을 때만 보내자.
#define	SUPPORT_CREDENTIAL_GMD
#define SUPPORT_REPLACE_CREDENTIAL
#define	SUPPORT_USAGERELE_EXT_DATA		// usage rule확장 data : 일단 SES 문서에 없어 무시한다.
#else
#define SUPPORT_USAGERULE_RECOVERY			// pstop시 video/audio output 관련해서 원복해야한다. ( 모두 default 로 적용한다 )
//#define SUPPORT_USAGERULE_RECOVERY_OUTPUT	// pstop시 video/audio output 관련해서 원복해야한다. < media에서 원복함으로 추후 확인 완료후 삭제하도록 하자.>
//#define	SUPPORT_CREDENTIAL_GMD			// SES에서는 support하지 않는다.
//#define SUPPORT_REPLACE_CREDENTIAL
//#define	SUPPORT_USAGERELE_EXT_DATA		// usage rule확장 data : 일단 SES 문서에 없어 무시한다.
#endif
//#define	SUPPORT_UR_HDCP					// Usage Rule에서 HDCP가 항상 ON이므로 hdcp에 대해서는 control하지 않는다.
#define MAKE_USAGERULE_VAL(val, shift, mask_num)		((val>>shift)&(mask_num==1?0x01:mask_num==2?0x03:mask_num==3?0x07:mask_num==4?0x0f:0x0)) // 쉬운 알고리즘으로 변경하자.
#endif

#define	GET_ASCII_NUM(X)				((X>= '0' && X<='9' ) ? X- '0' : ((X>= 'a' && X<='f' ) ?X-'a' + 10:X-'A' + 10))
#define	MAKE_STROF2BYTE_2_NUM(X)		((GET_ASCII_NUM(X[0]) << 4) | (GET_ASCII_NUM(X[1]) ) )

#define	MESSAGE_METHOD_MIN_LENGTH		4
#define	MESSAGE_SERVICE_NAME_LENGTH		4

#define	MESSAGE_FIELD_IDENTIFIER_SIZE	2
#define	MESSAGE_FIELD_IDENTIFIER		"::"

#define	MESSAGE_VALUE_IDENTIFIER_SIZE	1
#define	MESSAGE_VALUE_IDENTIFIER		"="

#define MESSAGE_VALUE_TRUE				"true"

#define MESSAGE_VALUE_FALSE				"false"

//#define MESSAGE_VALUE_UNDEFINED			"undefined"

#define	VERSION_STRING_LENGTH  			(20)	// NAGRA 문서에 정의
#define	UNIQUE_ID_STRING_LENGTH  		(16)	// NAGRA 문서에 정의
#define	CHIPSET_TYPE_STRING_LENGTH		(40)	// NAGRA 문서에 정의

#define	MAX_CREDENTIAL_NUMBER			(9)		// SES Test suite에 max 9개로 정의됨
#define	INVALID_CREDENTIAL_HANDLE		(0)
#define	INVALID_PLAYBACK_HANDLE			(0)


#define MAX_TEMP_BUF_LENGTH				(100)

#define	PRM_USAGE_RULE_TAG				(0xA5)	// NAGRA 문서에 정의
#define	PRM_USAGE_RULE_LENGTH			(0x04)	// NAGRA 문서에 정의
#define	PRM_USAGE_RULE_EXT_LENGTH		(0x06)	// NAGRA 문서에 정의

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eMSGTYPE_CSPG_HEXBIN = 0,
	eMSGTYPE_HDPLUSCA_SC,
	eMSGTYPE_MAX
}eXmgrCas_NaPrmSendMsgType_t;

typedef enum
{
	eMSG_PRMS_SYSINFO = 0,
	eMSG_PRMS_CHINFO,
	eMSG_PRMS_PSTART,
	eMSG_PRMS_PSTOP,
	eMSG_PRMS_PSTOPALL,
#if defined(SUPPORT_TIME_SETTING)
	eMSG_PRMS_PTIME, /* DVL integration Test를 위해 임시로 추가 */
#endif

	eMSG_CRES_CCRE,
	eMSG_CRES_DCRE,
	eMSG_CRES_DALL,
	eMSG_CRES_RCRE,
	eMSG_CRES_INFO,
	eMSG_CRES_UGMD,
	eMSG_CRES_USMD,

	eMSG_SC_INSERTED,
	eMSG_SC_PRODUCTVALIDATION,
	eMSG_MAX
}eXmgrCas_NaPrmSendMsg_t;

typedef enum
{
	eMSG_RESULTCODE_0 = 0,		/* success */
	eMSG_RESULTCODE_1,			/* unknown error.  sendDRMMessage()  failed  because  an  unspecified error occurred */
	eMSG_RESULTCODE_2,			/* Cannot  process request. sendDRMMessage()  failed  because  the  DRM  agent was unable to complete the request */
	eMSG_RESULTCODE_3,			/* Unknown MIME type. sendDRMMessage() failed, because the specified Mime Type is unknown for the specified DRM system indicated in the DRMSystemId */
	eMSG_RESULTCODE_4,			/* User Consent Needed. sendDRMMessage() failed, because user consent is needed for that action */
	eMSG_RESULTCODE_5,			/* Unknown DRM system. sendDRMMessage() failed, because the specified DRM System in DRMSystemId is unknown */
	eMSG_RESULTCODE_MAX = 0xFF
}eXmgrCas_NaPrmResultCode_t;

typedef enum
{
	eSYSTEMID_6304 = 0,
	eSYSTEMID_6211,
	eSYSTEMID_MAX
}eXmgrCas_NaPrmSendDRMSystemID_t;

typedef enum
{
	eCRES_RESULTCODE_SUCCESS = 0,					/* CCRE or DCRE : success */
	eCRES_RESULTCODE_ERROR_ACTIVE_CREDENTIAL,		/* DCRE : active credential */
	eCRES_RESULTCODE_ERROR_OVERFLOW,				/* handle number > MAX_CREDENTIAL_NUMBER */
	eCRES_RESULTCODE_ERROR_INVALID_HANDLE,			/* CCRE or DCRE : invalid handle */
	eCRES_RESULTCODE_ERROR_INVALID_DATA,			/* CCRE or DCRE : credential data is not valid */
	eCRES_RESULTCODE_ERROR_PSTOP_WITHOUT_PSTART,	/* Playback start없이 playback stop이 들어온경우 */
	eCRES_RESULTCODE_ERROR_MALLOC,					/* CCRE : memory malloc error */
	eCRES_RESULTCODE_ERROR_FREE,					/* DCRE : dredential data : free error */
	eCRES_RESULTCODE_ERROR_OTHER 					/* CCRE or DCRE : other error */
}eXmgrCas_NaPrmResultCodeStatus_t;

typedef enum
{
	eFIELD_ERROR =0,					/* Error Code */
	eFIELD_VERSION,						/* DVL Version */
	eFIELD_SNUID ,						/* String NUID */
	eFIELD_NUID,						/* NUID */
	eFIELD_SIRDSN ,						/* String STB Serial Number */
	eFIELD_IRDSN ,						/* STB Serial Number */
	eFIELD_CHTYPE ,						/* Chipset Type */
	eFIELD_CREH ,						/* Credential Handle */
	eFIELD_PLAYH ,						/* PlayBack Handle */
	eFIELD_STAT ,						/* PlayBack Status */
	eFIELD_GMD ,						/* General Meta Data */
	eFIELD_SMD ,						/* Specific Meta Data */
	eFIELD_TYPE ,						/* Credential Type */
	eFIELD_CID,							/* Content ID*/
	eFIELD_EMMI,						/* EMM VOD ID */
	eFIELD_CDA ,						/* Create Date */
	eFIELD_EDA ,						/* Expiration Date */
	eFIELD_FVD ,						/* FIrst Visualization Date */
	eFIELD_VWD,							/* Viewing Window Duration */
	eFIELD_VWED,						/* Viewing Window End Date */
	eFIELD_VWR,							/* Viewing Window Relativity */
	eFIELD_CIPH,						/* content Is Ciphered */
	eFIELD_CKPR,						/* ontent Key Is Protected */
	eFIELD_DATA,						/* Credential Data */
	eFIELD_CEMI,						/* Content EMI */
	eFIELD_CAID,						/* CA SystemID */
	eFIELD_MAX							/* other item */
}eXmgrCas_NaPrmResultFieldItem_t;

typedef enum
{
	eMESSAGE_CONTROL_HIDE_READY = 0,
	eMESSAGE_CONTROL_SHOW_READY
}eXmgrCas_NaPrmPopupControl_t;

#if defined(SUPPORT_USAGERULE)
typedef enum
{
	eUSAGERULE_UR_DATA = 0,
	eUSAGERULE_UR_EXT_DATA = 1,
	eUSAGEURLE_UR_MAX = 0xFF
}eXmgrCas_NaPrmUsageRuleVersion_t;

typedef enum
{
	eOutFlag_Disable = 0,
	eOutFlag_Enable = 1
}eXmgrCas_NaPrmOutputFlag_t;

typedef enum
{
	eHdcpMode_Off		= 0,					/* hdcp disable */
	eHdcpMode_On		= 1						/* hdcp enable */
}eXmgrCas_NaPrmHdcpMode_t;

typedef struct
{
	HBOOL      bActivate;
#if defined(SUPPORT_UR_HDCP)
	eXmgrCas_NaPrmHdcpMode_t eHdcpMode;
#endif
	Macrovision_t eMacrovisionMode;
	eXmgrCas_NaPrmOutputFlag_t eAVidOut; 	/* analog video output */
	eXmgrCas_NaPrmOutputFlag_t eAAudOut;	/* analog audio output */
}XmgrCas_NaPrmBackUpUsageRule_t;

#if defined(SUPPORT_UR_HDCP)
typedef struct
{
	HUINT8 ucActivate;
	eXmgrCas_NaPrmHdcpMode_t eMode;
}XmgrCas_NaPrmRecoveryHdcp_t;
#endif

typedef struct
{
	HUINT8 ucActivate;
	Macrovision_t eMode;
}XmgrCas_NaPrmRecoveryMacrovision_t;

typedef struct
{
	HUINT8 	ucActivate;
//	HBOOL 	bAvEnable;
	eXmgrCas_NaPrmOutputFlag_t eAVidOut; 	/* analog video output */
	eXmgrCas_NaPrmOutputFlag_t eAAudOut;	/* analog audio output */
}XmgrCas_NaPrmRecoveryAnalogVideoOutput_t;

#if defined(SUPPORT_USAGERULE_RECOVERY_OUTPUT)
typedef struct
{
	/* 지원하는 usageRule에 대해서만 처리하자 */
#if defined(SUPPORT_UR_HDCP)
	XmgrCas_NaPrmRecoveryHdcp_t stHdcp;
#endif
	XmgrCas_NaPrmRecoveryMacrovision_t stMacrovision;
	XmgrCas_NaPrmRecoveryAnalogVideoOutput_t stAVOutput;
	/* add UsageRule */
}XmgrCas_NaPrmRecoveryUsageRule_t;
#endif

typedef struct
{
	HUINT8 cVersion								: 2;	/* ignore */
	HUINT8 cAnticipation_flag					: 1;	/* ignore */
	HUINT8 cRecording_flag						: 1;
	HUINT8 cOriginal_sub_required				: 1;	/* ignore */
	HUINT8 cDvr_sub_required					: 1;	/* ignore */
	HUINT8 cTrans_coding_control				: 1;
	HUINT8 cImage_constraint					: 1;

	HUINT8 cTrick_mode_control					: 4;
	HUINT8 cPlaycount							: 4;	/* ignore */

	HUINT8 cConcurrent_viewing					: 3;
	HUINT8 cDigital_only_token					: 1;
	HUINT8 cUnprotected_digital_output_token	: 1;
	HUINT8 cHdcp_token							: 1;
	HUINT8 cDtcp_token							: 1;
	HUINT8 cWMDRM_token							: 1;

	HUINT8 cPRM_token							: 1;
	HUINT8 cOthers_token						: 1;
	HUINT8 cCci									: 2;
	HUINT8 cUnprotected_analog_output_token		: 1;
	HUINT8 cRetention_state						: 3;
#ifdef SUPPORT_USAGERELE_EXT_DATA
	HUINT8 cPc_threshold						: 4;	/* UR_ext_data */
	HUINT16 sPc_bitmap 							: 12;	/* UR_ext_data */
#endif
#if defined(SUPPORT_USAGERULE_RECOVERY_OUTPUT)
	XmgrCas_NaPrmRecoveryUsageRule_t stUR_Recovery;		/* 여기에 사용할지는 좀더 고민하자 */
#endif
}XmgrCas_NaPrmUsageRule_t;
#endif

typedef struct
{
	HUINT32 ulHandle;
	HUINT16 usSize;
	HUINT8 *pucData;

	HUINT32 ulMsgID;
	HUINT32 ulPlayBackHandle;
#ifdef SUPPORT_CREDENTIAL_GMD
	HUINT8 *pucGenericMetaData;
	HUINT32 ulGenericMetaDataSize;
#endif
	HUINT8 *pucSpecificMetaData;
	HUINT32 ulSpecificMetaDataSize;
#if defined(SUPPORT_USAGERULE)
	XmgrCas_NaPrmUsageRule_t stUsageRules;
#endif
	MgrMedia_DrmInformation_t stMgrMedia_DrmInformation;
}XmgrCas_NaPrmCredentialInfo_t;

typedef struct
{
	HUINT32 OutputCredentialsSize;
	HUINT8* pucOutputCredentials;
	HUINT8  ucPlaybackStatus;
#ifdef SUPPORT_CREDENTIAL_GMD
	HUINT32 GenericMetadataSize;
	HUINT8* pucGenericMetadata;
#endif
	HUINT32 SpecificMetadataSize;
	HUINT8* pucSpecificMetadata;
	HUINT32* ppxPlaybackHandle;
}XmgrCas_NaPrmResultPlayback_t;


typedef struct
{
	TVodEmmData           vodEmmData;
	TVodEcmData           vodEcmData;
	TLcmData              lcmData;
}XmgrCas_NaPrmCredentialUnion_t;

typedef struct
{
	HUINT8 ucCredentialsType;
	XmgrCas_NaPrmCredentialUnion_t stCredentialUnion;
#ifdef SUPPORT_CREDENTIAL_GMD
	HUINT32 ulGenericMetadataSize;
	HUINT8* pucGenericMetadata;
#endif
	HUINT32 ulSpecificMetadataSize;
	HUINT8* pucSpecificMetadata;
}XmgrCas_NaPrmResultCredentialInfo_t;

#ifdef SUPPORT_ONLY_ONE_CHIPSYSINFORMATION
typedef struct
{
	HUINT8 cEnable;
	HUINT8 szChipInfo[MAX_TEMP_BUF_LENGTH];
}XmgrCas_NaPrmBackUpChipInfo_t;

typedef struct
{
	HUINT8 cEnable;
	HUINT8 szSysInfo[MAX_TEMP_BUF_LENGTH];
}XmgrCas_NaPrmBackUpSystemInfo_t;

typedef struct
{
	XmgrCas_NaPrmBackUpChipInfo_t s_stNagraPRM_BackUpChipInfo;
	XmgrCas_NaPrmBackUpSystemInfo_t  s_stNagraPRM_BackUpSysInfo;
}XmgrCas_NaPrmBackUpSystemChipInfo_t;
#endif

typedef struct
{
	HUINT8		szVersionStr[VERSION_STRING_LENGTH];
	HUINT32		ulIrdSerialNumber;
	HUINT8		szIrdSerialNumberStr[UNIQUE_ID_STRING_LENGTH];
	HUINT32		ulNuId;
	HUINT8		szNuIdStr[UNIQUE_ID_STRING_LENGTH];
}XmgrCas_NaPrmSystemInfo_t;

typedef struct
{
	HUINT32		ulNuId;
	HUINT8		szNuIdStr[UNIQUE_ID_STRING_LENGTH];
	HUINT8		szChipsetTypeStr[CHIPSET_TYPE_STRING_LENGTH];
}XmgrCas_NaPrmChipsetInfo_t;

typedef struct
{
	eXmgrCas_NaPrmSendMsgType_t eMsgType;
	HUINT8 ucStr[50];
}XmgrCas_NaPrmSendMsgType_t;

typedef struct
{
	eXmgrCas_NaPrmSendMsg_t eMsg;
	HUINT8 ucStr[50];
}XmgrCas_NaPrmSendMsg_t;

typedef struct
{
	eXmgrCas_NaPrmSendDRMSystemID_t eDrmSystemID;
	HUINT8 ucStr[50];
}XmgrCas_NaPrmSendDrmSystemId_t;

typedef struct
{
	eXmgrCas_NaPrmResultFieldItem_t eField;
	HUINT8 ucStr[50];
}XmgrCas_NaPrmResultFieldItem_t;

typedef struct
{
	HUINT16 ucMsgID;
	HUINT8 *pucMsg;
	HUINT32 ulMsgSize;
	eXmgrCas_NaPrmResultCode_t eResultCode;

	HUINT32	hErrorCode;
	HUINT32	hStat;
	HUINT32	hCredetialHandle;
	HUINT32	hPlayBackHandle;
	HUINT16	usEcmCasSystemID;

	XmgrCas_NaPrmChipsetInfo_t			*pstChipsetInformation;
	XmgrCas_NaPrmSystemInfo_t			*pstSystemInformation;
	XmgrCas_NaPrmResultCredentialInfo_t *pstResultCredentialInfo;
}XmgrCas_NaPrmDRMResultMessage_t;

typedef struct
{
	eXmgrCas_NaPrmSendMsgType_t 	eMsgType;
	eXmgrCas_NaPrmSendMsg_t 		eMsg;
	eXmgrCas_NaPrmSendDRMSystemID_t eDRMSystemID;

	HUINT8	*pucCredentialData;
	HUINT32	ulCredentialDataSize;
	HUINT32	hCredetialHandle;
	HUINT32	ulTransportSessionId;
	HUINT32	hPlayBackHandle;
	HUINT8	*pucProductionParam;
}XmgrCas_NaPrmDRMSendMessage_t;

typedef struct
{
	XmgrCas_NaPrmCredentialInfo_t stCredentialInfo[MAX_CREDENTIAL_NUMBER];
	/* 여기에 추가사항이 존재하나 ? */
}XmgrCas_NaPrmCredentialHandleManager_t;

typedef struct
{
	XmgrCas_NaPrmDRMSendMessage_t			*pstDrmSendMessage;
	XmgrCas_NaPrmDRMResultMessage_t			*pstDrmResultMessage;
	XmgrCas_NaPrmCredentialHandleManager_t	*pstDrmCredentialHandleManager;
} XmgrCas_NaPrmContext_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaPrmSendMsgType_t stDrmSendMsgType[eMSGTYPE_MAX] =
{
	{eMSGTYPE_CSPG_HEXBIN,	"application/vnd.oipf.cspg-hexbinary"	},
	{eMSGTYPE_HDPLUSCA_SC,	"hdplusCA_SC"							}
	/* Add : MsgType */
};

STATIC XmgrCas_NaPrmSendDrmSystemId_t stDrmSendDrmSystemId[eSYSTEMID_MAX] =
{
	{eSYSTEMID_6304,			"urn:dvb:casystemid:6304"			},
	{eSYSTEMID_6211,			"urn:dvb:casystemid:6211"			}
	/* Add : DrmSystemID */
};

#if 0
// TODO
STATIC XmgrCas_NaPrmSendMsg_t stNagraPRM_SendMsg[eMSG_MAX] =
{
	{eMSG_PRMS_SYSINFO,			""			},
	{eMSG_PRMS_CHINFO,			""			},
	{eMSG_PRMS_PSTART,			""			},
	{eMSG_PRMS_PSTOP,			""			},
	{eMSG_PRMS_PSTOPALL,		""			},

	{eMSG_CRES_CCRE,			""			},
	{eMSG_CRES_DCRE,			""			},
	{eMSG_CRES_DALL,			""			},
	{eMSG_CRES_RCRE,			""			},
	{eMSG_CRES_INFO,			""			},
	{eMSG_CRES_UGMD,			""			},

	{eMSG_SC_INSERTED,			""			},
	{eMSG_SC_PRODUCTVALIDATION,	""			},
};
#endif

STATIC XmgrCas_NaPrmResultFieldItem_t stDrmResultMsgField[eFIELD_MAX] =
{
	{eFIELD_ERROR,			""			},
	{eFIELD_VERSION,		"VER"		},
	{eFIELD_SNUID,			"SNUID"		},
	{eFIELD_NUID,			"NUID"		},
	{eFIELD_SIRDSN,			"SIRDSN"	},
	{eFIELD_IRDSN,			"IRDSN"		},
	{eFIELD_CHTYPE,			"CHTYPE"	},
	{eFIELD_CREH,			"CREH"		},
	{eFIELD_PLAYH,			"PLAYH"		},
	{eFIELD_STAT,			"STAT"		},
	{eFIELD_GMD,			"GMD"		},
	{eFIELD_SMD,			"SMD"		},
	{eFIELD_TYPE,			"TYPE"		},
	{eFIELD_CID,			"CID"		},
	{eFIELD_EMMI,			"EMMI"		},
	{eFIELD_CDA,			"CDA"		},
	{eFIELD_EDA,			"EDA"		},
	{eFIELD_FVD,			"FVD"		},
	{eFIELD_VWD,			"VWD"		},
	{eFIELD_VWED,			"VWED"		},
	{eFIELD_VWR,			"VWR"		},
	{eFIELD_CIPH,			"CIPH"		},
	{eFIELD_CKPR,			"CKPR"		},
	{eFIELD_DATA,			"DATA"		},
	{eFIELD_CEMI,			"DATA"		},
	{eFIELD_CAID,			""			}
	/* Add : MsgField */
 };

//STATIC XmgrCas_NaPrmUsageRule_t s_stBackup_UsageRuleState;
//STATIC eXmgrCas_NaPrmPopupControl_t eNagraPRM_Message_Control= eMESSAGE_CONTROL_HIDE_READY;
#ifdef SUPPORT_ONLY_ONE_CHIPSYSINFORMATION
//STATIC XmgrCas_NaPrmBackUpSystemChipInfo_t s_stNagraPRM_BackUpChipSysInfo_t;
//STATIC HUINT8 firstBoot_ChipSysInfoInitFlag = FALSE;
#endif


static XmgrCas_NaPrmContext_t stNaPrmContext;


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____LOCAL_FUNCTION______________________________________________________
STATIC void _xmgr_cas_naprm_HandleMangerInit(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr);
STATIC eXmgrCas_NaPrmResultCodeStatus_t _xmgr_cas_naprm_GetCredentialInfoFromCredentialIndex(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT16 usIdx, XmgrCas_NaPrmCredentialInfo_t *pCredentialInfo);
STATIC XmgrCas_NaPrmContext_t *_xmgr_cas_naprm_GetContext(void);
STATIC void _xmgr_cas_naprm_ResultMsgInit(XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg);
STATIC HERROR _xmgr_cas_naprm_StopPlaybackByPlaybackHandle(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle);

#ifdef CONFIG_DEBUG
STATIC void _xmgr_cas_naprm_PrintDrmSendMessage(HUINT8 * pucMsgType, HUINT8 * pucMsg, HUINT8 *pucDRMSystemID, XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg);
STATIC void _xmgr_cas_naprm_PrintDrmResultMessage(XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg, XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr);
STATIC HERROR _xmgr_cas_naprm_ConvertDecimalToHex(HUINT8 *pucSource, HUINT32 ulSourceSize, HUINT8 *pucDes, HUINT32 *ulDesSize);
extern void xmgr_cas_NaPrmCmd(HxCMD_Handle_t hCmdHandle);
enum
{
	eMSG_MEMORY_CHECK = 0,
	eHDL_MEMORY_CHECK,
	eCXT_MEMORY_CHECK
};

static void  _xmgr_cas_naprm_MemoryCheck(XmgrCas_NaPrmContext_t *pstContext, HUINT8 ucHdlMgrFlag)
{
	XmgrCas_NaPrmDRMSendMessage_t			*pstSendMsg;
	XmgrCas_NaPrmDRMResultMessage_t			*pstRstMsg;
	XmgrCas_NaPrmCredentialHandleManager_t	*pstHdlMgr;
	HUINT16 i=0;

	if(ucHdlMgrFlag == eCXT_MEMORY_CHECK)
	{
		if(pstContext->pstDrmSendMessage != NULL)
		{
			HxLOG_Error("pstDrmSendMessage is error\n");
		}
		if(pstContext->pstDrmResultMessage != NULL)
		{
			HxLOG_Error("pstDrmResultMessage is error\n");
		}
		if(pstContext->pstDrmCredentialHandleManager != NULL)
	{
			HxLOG_Error("pstDrmCredentialHandleManager is error\n");
		}
		return;
	}

	pstSendMsg = pstContext->pstDrmSendMessage;
	pstRstMsg = pstContext->pstDrmResultMessage;
	pstHdlMgr = pstContext->pstDrmCredentialHandleManager;

	if(pstSendMsg->pucCredentialData != NULL)
	{
		HxLOG_Error("pucCredentialData is error\n");
	}

	if(pstSendMsg->pucProductionParam != NULL)
	{
		HxLOG_Error("pucProductionParam is error\n");
	}

	if(pstRstMsg->pucMsg != NULL)
	{
		HxLOG_Error("pucMsg is error\n");
	}

	if(pstRstMsg->pstChipsetInformation != NULL)
	{
		HxLOG_Error("pstChipsetInformation is error\n");
	}

	if(pstRstMsg->pstSystemInformation != NULL)
	{
		HxLOG_Error("pstSystemInformation is error\n");
	}

	if(pstRstMsg->pstResultCredentialInfo != NULL)
	{
		HxLOG_Error("pstResultCredentialInfo is error\n");
		if(pstRstMsg->pstResultCredentialInfo->pucSpecificMetadata != NULL)
		{
			HxLOG_Error("pucSpecificMetadata is error\n");
		}
	}

	if(ucHdlMgrFlag == eHDL_MEMORY_CHECK)
	{
		for(i=0;i<MAX_CREDENTIAL_NUMBER;i++)
		{
			if(pstHdlMgr->stCredentialInfo[i].pucData != NULL)
			{
				HxLOG_Error("pucData[%d] is error\n", i);
			}
			if(pstHdlMgr->stCredentialInfo[i].pucSpecificMetaData != NULL)
			{
				HxLOG_Error("pucSpecificMetaData[%d] is error\n", i);
			}
		}
	}
}

void _xmgr_cas_naprm_PrintUsageRules(XmgrCas_NaPrmUsageRule_t *pUsageRule)
{
	XmgrCas_NaPrmUsageRule_t *pUR = pUsageRule;

	if(pUR)
	{
		HxLOG_Print("+			< Usage Rule Info >\n");
		HxLOG_Print("+			Version = 0x%x\n", pUR->cVersion); 										//: 2;
		HxLOG_Print("+			Anticipation_flag = 0x%x\n", pUR->cAnticipation_flag); 					//: 1;
		HxLOG_Print("+			Recording_flag = 0x%x\n", pUR->cRecording_flag); 						//: 1;
		HxLOG_Print("+			Original_sub_required = 0x%x\n", pUR->cOriginal_sub_required); 			//: 1;
		HxLOG_Print("+			Dvr_sub_required = 0x%x\n", pUR->cDvr_sub_required); 					//: 1;
		HxLOG_Print("+			Trans_coding_control = 0x%x\n", pUR->cTrans_coding_control); 			//: 1;
		HxLOG_Print("+			Image_constraint = 0x%x\n", pUR->cImage_constraint); 					//: 1;

		HxLOG_Print("+			Trick_mode_control = 0x%x\n", pUR->cTrick_mode_control); 				//: 4;
		HxLOG_Print("+			Playcount = 0x%x\n", pUR->cPlaycount); 									//: 4;

		HxLOG_Print("+			Concurrent_viewing = 0x%x\n", pUR->cConcurrent_viewing); 				//: 3;
		HxLOG_Print("+			Digital_only_token = 0x%x\n", pUR->cDigital_only_token); 				//: 1;
		HxLOG_Print("+			Unprotected_digital_output_token = 0x%x\n", pUR->cUnprotected_digital_output_token); 	//: 1;
		HxLOG_Print("+			Hdcp_token = 0x%x\n", pUR->cHdcp_token); 								//: 1;
		HxLOG_Print("+			Dtcp_token = 0x%x\n", pUR->cDtcp_token); 								//: 1;
		HxLOG_Print("+			WMDRM_token = 0x%x\n", pUR->cWMDRM_token); 								//: 1;

		HxLOG_Print("+			PRM_token = 0x%x\n", pUR->cPRM_token); 									//: 1;
		HxLOG_Print("+			Others_token = 0x%x\n", pUR->cOthers_token); 							//: 1;
		HxLOG_Print("+			Cci = 0x%x\n", pUR->cCci); 												//: 2;
		HxLOG_Print("+			Unprotected_analog_output_token = 0x%x\n", pUR->cUnprotected_analog_output_token); 	//: 1;
		HxLOG_Print("+			Retention_state = 0x%x\n", pUR->cRetention_state); 						//: 3;
#ifdef SUPPORT_USAGERELE_EXT_DATA
		HxLOG_Print("+			Pc_threshold = 0x%x\n", pUR->cPc_threshold); 							//: 4;	/* UR_ext_data */
		HxLOG_Print("+			Pc_bitmap = 0x%x\n", pUR->sPc_bitmap); 									//: 12;	/* UR_ext_data */
#endif
		HxLOG_Print("\n+			< Recovery Usage Rule Control Status>\n");
#if defined(SUPPORT_USAGERULE_RECOVERY_OUTPUT)
#if defined(SUPPORT_UR_HDCP)
		HxLOG_Print("+			Hdcp : Activated = %d\n", pUR->stUR_Recovery.stHdcp.ucActivate);
		if(pUR->stUR_Recovery.stHdcp.ucActivate==TRUE)
		{
			HxLOG_Print("+			Hdcp = %d\n", pUR->stUR_Recovery.stHdcp.eMode);
		}
#endif
		HxLOG_Print("+			Macrovision : Activated = %d\n", pUR->stUR_Recovery.stMacrovision.ucActivate);
		if(pUR->stUR_Recovery.stMacrovision.ucActivate==TRUE)
		{
			HxLOG_Print("+			Macrovision = %d\n", pUR->stUR_Recovery.stMacrovision.eMode);
		}
		HxLOG_Print("+			AV Output : Activated = %d\n", pUR->stUR_Recovery.stAVOutput.ucActivate);
		if(pUR->stUR_Recovery.stAVOutput.ucActivate==TRUE)
		{
			HxLOG_Print("+			AV Output : video = %d, audio = %d\n", pUR->stUR_Recovery.stAVOutput.eAVidOut, pUR->stUR_Recovery.stAVOutput.eAAudOut);
		}
#endif
	}
}


STATIC void _xmgr_cas_naprm_PrintDrmSendMessage(HUINT8 * pucMsgType, HUINT8 * pucMsg, HUINT8 *pucDRMSystemID, XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg)
{
	if(pstDrmSendMsg == NULL || pucMsgType == NULL || pucMsg == NULL || pucDRMSystemID == NULL)
	{
		HxLOG_Print("++++++++++++++++++++ << Print Drm Send Msg >> ++++++++++\n");
		HxLOG_Print("+ msg is null\n");
		return;
	}
	HxLOG_Print("++++++++++++++++++++ << Print Drm Send Msg >> ++++++++++\n");
	HxLOG_Print("+ 	Print SendDrmMessage()\n");
	HxLOG_Print("+	pucMsgType = %s\n", pucMsgType);
	HxLOG_Print("+	pucMsg = %s\n", pucMsg);
	HxLOG_Print("+	pucDRMSystemID = %s\n\n", pucDRMSystemID);

	HxLOG_Print("\n++++++++++++++++++++ << Msg Parse >> ++++++++++\n");

	// Msg Type
	switch(pstDrmSendMsg->eMsgType)
	{
		case eMSGTYPE_CSPG_HEXBIN:
			HxLOG_Print("+	MsgType = eMSGTYPE_CSPG_HEXBIN\n"); break;
		case eMSGTYPE_HDPLUSCA_SC:
			HxLOG_Print("+	MsgType = eMSGTYPE_HDPLUSCA_SC\n"); break;
		case eMSGTYPE_MAX:
		default:
			HxLOG_Print("+	MsgType = eMSGTYPE_MAX\n"); break;
	}
	// Msg

	switch(pstDrmSendMsg->eMsg)
	{
		case eMSG_PRMS_SYSINFO:
			HxLOG_Print("+	Msg = eMSG_PRMS_SYSINFO\n"); break;
		case eMSG_PRMS_CHINFO:
			HxLOG_Print("+	Msg = eMSG_PRMS_CHINFO\n"); break;
		case eMSG_PRMS_PSTART:
			HxLOG_Print("+	Msg = eMSG_PRMS_PSTART\n"); break;
		case eMSG_PRMS_PSTOP:
			HxLOG_Print("+	Msg = eMSG_PRMS_PSTOP\n"); break;
		case eMSG_PRMS_PSTOPALL:
			HxLOG_Print("+	Msg = eMSG_PRMS_PSTOPALL\n"); break;
		case eMSG_CRES_CCRE:
			HxLOG_Print("+	Msg = eMSG_CRES_CCRE\n"); break;
		case eMSG_CRES_DCRE:
			HxLOG_Print("+	Msg = eMSG_CRES_DCRE\n"); break;
		case eMSG_CRES_DALL:
			HxLOG_Print("+	Msg = eMSG_CRES_DALL\n"); break;
		case eMSG_CRES_RCRE:
			HxLOG_Print("+	Msg = eMSG_CRES_RCRE\n"); break;
		case eMSG_CRES_INFO:
			HxLOG_Print("+	Msg = eMSG_CRES_INFO\n"); break;
		case eMSG_CRES_UGMD:
			HxLOG_Print("+	Msg = eMSG_CRES_UGMD\n"); break;
		case eMSG_CRES_USMD:
			HxLOG_Print("+	Msg = eMSG_CRES_USMD\n"); break;
		case eMSG_SC_INSERTED:
			HxLOG_Print("+	Msg = eMSG_SC_INSERTED\n"); break;
		case eMSG_SC_PRODUCTVALIDATION:
			HxLOG_Print("+	Msg = eMSG_SC_PRODUCTVALIDATION\n"); break;
		case eMSG_MAX:
		default:
			HxLOG_Print("+	Msg = eMSG_MAX\n"); break;
	}
	// DRM System ID
	switch(pstDrmSendMsg->eDRMSystemID)
	{
		case eSYSTEMID_6304:
			HxLOG_Print("+	DRMSystemID = eSYSTEMID_6304\n"); break;
		case eSYSTEMID_6211:
			HxLOG_Print("+	DRMSystemID = eSYSTEMID_6211\n"); break;
		case eSYSTEMID_MAX:
		default:
			HxLOG_Print("+	DRMSystemID = eSYSTEMID_MAX\n"); break;
	}

	if(pstDrmSendMsg->pucCredentialData != NULL)
	{
		HxLOG_Print("+	pucCredentialData = size( %d ), data = { 0x%02x, 0x%02x, ... } \n", pstDrmSendMsg->ulCredentialDataSize, pstDrmSendMsg->pucCredentialData[0], pstDrmSendMsg->pucCredentialData[1]);
	}
	else
	{
		HxLOG_Print("+	pucCredentialData is NULL \n");
	}

	HxLOG_Print("+	hCredetialHandle = (0x%08x)(%d)\n", pstDrmSendMsg->hCredetialHandle, pstDrmSendMsg->hCredetialHandle);
	HxLOG_Print("+	hPlayBackHandle = (0x%08x)(%d)\n", pstDrmSendMsg->hPlayBackHandle, pstDrmSendMsg->hPlayBackHandle);
	HxLOG_Print("+	ulTransportSessionId = (0x%08x)(%d)\n", pstDrmSendMsg->ulTransportSessionId, pstDrmSendMsg->ulTransportSessionId);
	HxLOG_Print("\n\n");
	return;
}

void xmgr_cas_naprm_PrintHexDump(HUINT8 *pusTitle, HUINT8 *pusData, HUINT32 ulSize)
{
	int i;
	if(!pusData || ulSize<=0 || !pusTitle)
	{
		return;
	}

	HxLOG_Print("\n\t\t\t*  %s Hex Dump(size = %d)\n", pusTitle, ulSize);

	for(i=0;i<ulSize;i++)
	{
		if(i%16 == 0)
			HxLOG_Print ("\n\t\t\t* %08xh: ", i);
		HxLOG_Print ("%02X ", pusData[i]);
	}
	HxLOG_Print ("\n");
}

STATIC void _xmgr_cas_naprm_PrintParseSCProcuct(HUINT8 *pucPtr, HUINT32 ulDesLen)
{
	HUINT8 j = 0;
	HUINT16 val = 0;
	HUINT16 usPriDataLen = 0;
	HUINT8 priDataBuf[100];

	if(!pucPtr)
	{
		return;
	}

	HxSTD_memset(priDataBuf, 0, sizeof(priDataBuf));

	usPriDataLen = (pucPtr[val]<<8 | pucPtr[val+1]);
	val++;
	val++;

	HxLOG_Print("Private Data Length (%d) \n", usPriDataLen);

	if(usPriDataLen>0)
	{
		HxLOG_Print ("private_data_byte = \n");
		for(j=0;j<usPriDataLen;j++)
		{
			priDataBuf[j] = pucPtr[val++];
			HxLOG_Print ("0x%02x ", priDataBuf[j]);
		}
	}
	HxLOG_Print ("\n");
}

STATIC void _xmgr_cas_naprm_PrintCredentialInfo(const TCredentialsUnion	*pxCredentialsUnion, TCredentialsType xCredentialsType)
{
#if 0
// TODO
	TVodEmmData           	vodEmmData;
	DATE_TIME_T 			dateTime;

	MWNagraPrmData(DBG_DATA, ("%s() : (type = %d)\n", __FUNCTION__, xCredentialsType));

	switch (xCredentialsType)
	{
		case CREDENTIALS_TYPE_PROFILE:
			MWNagraPrmData(DBG_DATA,("\t CREDENTIALS_TYPE_PROFILE :  \n"));
			break;

		case CREDENTIALS_TYPE_VOD_EMM:
			MWNagraPrmData(DBG_DATA,("\t CREDENTIALS_TYPE_VOD_EMM :  \n"));
			HxSTD_memset(&vodEmmData, 0x00, sizeof(TVodEmmData));
			HxSTD_memcpy(&vodEmmData, &(pxCredentialsUnion->vodEmmData), sizeof(TVodEmmData));
			MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.irdSerialNumber : %ld\n", vodEmmData.irdSerialNumber));
			MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.contentId : %ld\n", vodEmmData.contentId));
			MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.vodEmmId : %ld \n", vodEmmData.vodEmmId));

			UTIL_TCONV_ConvertUnixTimeToDateTime(vodEmmData.credentialsCreationDate, &dateTime);
			MWNagraPrmData(DBG_DATA,("\t\t credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
														dateTime.stDate.ucMonth,
														dateTime.stDate.ucDay,
														dateTime.stTime.ucHour,
														dateTime.stTime.ucMinute,
														dateTime.stTime.ucSecond ));

			UTIL_TCONV_ConvertUnixTimeToDateTime(vodEmmData.contentExpirationDate, &dateTime);
			MWNagraPrmData(DBG_DATA,("\t\t contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
														dateTime.stDate.ucMonth,
														dateTime.stDate.ucDay,
														dateTime.stTime.ucHour,
														dateTime.stTime.ucMinute,
														dateTime.stTime.ucSecond ));

			MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.contentViewingWindowDuration : %ld \n", vodEmmData.contentViewingWindowDuration));

			UTIL_TCONV_ConvertUnixTimeToDateTime(vodEmmData.contentFirstVisualizationDate, &dateTime);
			MWNagraPrmData(DBG_DATA,("\t\t contentFirstVisualizationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
														dateTime.stDate.ucMonth,
														dateTime.stDate.ucDay,
														dateTime.stTime.ucHour,
														dateTime.stTime.ucMinute,
														dateTime.stTime.ucSecond ));

			UTIL_TCONV_ConvertUnixTimeToDateTime(vodEmmData.contentViewingWindowEndDate, &dateTime);
			MWNagraPrmData(DBG_DATA,("\t\t contentViewingWindowEndDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
														dateTime.stDate.ucMonth,
														dateTime.stDate.ucDay,
														dateTime.stTime.ucHour,
														dateTime.stTime.ucMinute,
														dateTime.stTime.ucSecond) );

			if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE)
			{
				MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE \n"));
			}
			else if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION)
			{
				MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION \n"));
			}
			else 	// LAST_VIEWING_WINDOW_RELATIVITY
			{
				MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.contentViewingWindowRelativity : LAST_VIEWING_WINDOW_RELATIVITY \n"));
			}

			MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.contentIsCiphered : %d \n", vodEmmData.contentIsCiphered));
			MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.contentKeyIsProtected : %d \n", vodEmmData.contentKeyIsProtected));
			MWNagraPrmData(DBG_DATA,("\t\t vodEmmData.nuidValue : 0x%lx \n", vodEmmData.nuidValue));
			break;

		case CREDENTIALS_TYPE_VOD_ECM:
			MWNagraPrmData(DBG_DATA,("\t CREDENTIALS_TYPE_VOD_ECM :  \n"));
			break;
		case CREDENTIALS_TYPE_LCM:
			MWNagraPrmData(DBG_DATA,("\t CREDENTIALS_TYPE_LCM :  \n"));
			break;
		case CREDENTIALS_TYPE_HOME_NETWORK_LCM:
			MWNagraPrmData(DBG_DATA,("\t CREDENTIALS_TYPE_HOME_NETWORK_LCM :  \n"));
			break;
		case CREDENTIALS_TYPE_UNKNOWN:
			MWNagraPrmData(DBG_DATA,("\t CREDENTIALS_TYPE_UNKNOWN :  \n"));
			break;
		default:
			MWNagraPrmData(DBG_DATA,("\t error....\n"));
			break;
	}
#endif
}

STATIC void _xmgr_cas_naprm_PrintDrmHandleManager(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr)
{
	HUINT8 ucIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;
//	HUINT8 ucSMD[DRMAGENT_DRMMESSAGE_MAX_LEN];
	HUINT8	szCredentialHex[DRMAGENT_DRMMESSAGE_MAX_LEN];
	HUINT32 ulCredentialHexSize = 0;

	if(pstCreHdlMgr == NULL)
	{
		return;
	}

	HxLOG_Print("\n++++++++++++++++++++ << Print Drm Handle Manager >> ++++++++++\n");

	for(ucIdx=0;ucIdx<MAX_CREDENTIAL_NUMBER;ucIdx++)
	{
		HxLOG_Print("+ Index[%d]\n", ucIdx);
		pCreInfo = &pstCreHdlMgr->stCredentialInfo[ucIdx];

		if(pCreInfo)
		{
			HxLOG_Print("+		1. Handle = %d\n", pCreInfo->ulHandle);
			HxLOG_Print("+		2. PlaybackHandle = %d\n", pCreInfo->ulPlayBackHandle);

			if(pCreInfo->pucData)
			{
				HxSTD_memset(szCredentialHex, 0, sizeof(szCredentialHex));

				if(_xmgr_cas_naprm_ConvertDecimalToHex(pCreInfo->pucData, pCreInfo->usSize, szCredentialHex, &ulCredentialHexSize)==ERR_OK)
				{
					HxLOG_Print("+		3. CreData[%d]\n", pCreInfo->usSize);
					xmgr_cas_naprm_PrintHexDump("Credential", szCredentialHex, ulCredentialHexSize);
				}
				#if 0
				// TODO
				if(pCreInfo->ulPlayBackHandle>0)
				{
					TCredentialsType		xCredentialsType;
					const TCredentialsUnion	*pxCredentialsUnion=NULL;
					TSize					xGenericMetadataSize;
					const TUnsignedInt8		*pxGenericMetadata =NULL;
					TSize					xSpecificMetadataSize;
					const TUnsignedInt8		*pxSpecificMetadata = NULL;
					HUINT8	*pCredentialHex = NULL;
					HUINT32 ulCredentialHexSize = 0;

					pCredentialHex = (HUINT8*)HLIB_STD_MemAlloc(pCreInfo->usSize+2);

					HxLOG_Print("+		4. Credential Information\n");

					if(pCredentialHex==NULL)
					{
						HxLOG_Error("pCredentialHex is NULL\n");
						HxLOG_Print("+			error : CredentialHex(Malloc)\n");
					}
					else
					{
						HxSTD_memset(pCredentialHex, 0, pCreInfo->usSize+2);

						if(_xmgr_cas_naprm_ConvertDecimalToHex(pCreInfo->pucData, pCreInfo->usSize, pCredentialHex, &ulCredentialHexSize)==ERR_OK)
						{
							if(xmgr_cas_NadvlCredentialsInformationRequest(ulCredentialHexSize
																	, pCredentialHex
																	, &xCredentialsType
																	, &pxCredentialsUnion
																	, &xGenericMetadataSize
																	, &pxGenericMetadata
																	, &xSpecificMetadataSize
																	, &pxSpecificMetadata)==ERR_OK)
							{
								TVodEmmData           	vodEmmData;
								DATE_TIME_T 			dateTime;

								switch (xCredentialsType)
								{
									case CREDENTIALS_TYPE_PROFILE:
										HxLOG_Print("+			CREDENTIALS_TYPE_PROFILE :  \n");
										break;

									case CREDENTIALS_TYPE_VOD_EMM:
										HxLOG_Print("+			CREDENTIALS_TYPE_VOD_EMM :  \n");
										HxSTD_memset(&vodEmmData, 0x00, sizeof(TVodEmmData));
										HxSTD_memcpy(&vodEmmData, &(pxCredentialsUnion->vodEmmData), sizeof(TVodEmmData));
										HxLOG_Print("+				* vodEmmData.irdSerialNumber : %ld\n", vodEmmData.irdSerialNumber);
										HxLOG_Print("+				* vodEmmData.contentId : %ld\n", vodEmmData.contentId);
										HxLOG_Print("+				* vodEmmData.vodEmmId : %ld \n", vodEmmData.vodEmmId);

										UTIL_TCONV_ConvertUnixTimeToDateTime(vodEmmData.credentialsCreationDate, &dateTime);
										HxLOG_Print("+				* credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
																					dateTime.stDate.ucMonth,
																					dateTime.stDate.ucDay,
																					dateTime.stTime.ucHour,
																					dateTime.stTime.ucMinute,
																					dateTime.stTime.ucSecond );

										UTIL_TCONV_ConvertUnixTimeToDateTime(vodEmmData.contentExpirationDate, &dateTime);
										HxLOG_Print("+				* contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
																					dateTime.stDate.ucMonth,
																					dateTime.stDate.ucDay,
																					dateTime.stTime.ucHour,
																					dateTime.stTime.ucMinute,
																					dateTime.stTime.ucSecond);

										HxLOG_Print("+				* vodEmmData.contentViewingWindowDuration : %ld \n", vodEmmData.contentViewingWindowDuration);

										UTIL_TCONV_ConvertUnixTimeToDateTime(vodEmmData.contentFirstVisualizationDate, &dateTime);
										HxLOG_Print("+				* contentFirstVisualizationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
																					dateTime.stDate.ucMonth,
																					dateTime.stDate.ucDay,
																					dateTime.stTime.ucHour,
																					dateTime.stTime.ucMinute,
																					dateTime.stTime.ucSecond);

										UTIL_TCONV_ConvertUnixTimeToDateTime(vodEmmData.contentViewingWindowEndDate, &dateTime);
										HxLOG_Print("+				* contentViewingWindowEndDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
																					dateTime.stDate.ucMonth,
																					dateTime.stDate.ucDay,
																					dateTime.stTime.ucHour,
																					dateTime.stTime.ucMinute,
																					dateTime.stTime.ucSecond );

										if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE)
										{
											HxLOG_Print("+				* vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE \n");
										}
										else if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION)
										{
											HxLOG_Print("+				* vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION \n");
										}
										else 	// LAST_VIEWING_WINDOW_RELATIVITY
										{
											HxLOG_Print("+				* vodEmmData.contentViewingWindowRelativity : LAST_VIEWING_WINDOW_RELATIVITY \n");
										}

										HxLOG_Print("+				* vodEmmData.contentIsCiphered : %d \n", vodEmmData.contentIsCiphered);
										HxLOG_Print("+				* vodEmmData.contentKeyIsProtected : %d \n", vodEmmData.contentKeyIsProtected);
										HxLOG_Print("+				* vodEmmData.nuidValue : 0x%lx \n", vodEmmData.nuidValue);
										break;

									case CREDENTIALS_TYPE_VOD_ECM:
										HxLOG_Print("+			CREDENTIALS_TYPE_VOD_ECM :  \n");
										break;
									case CREDENTIALS_TYPE_LCM:
										HxLOG_Print("+			CREDENTIALS_TYPE_LCM :  \n");
										break;
									case CREDENTIALS_TYPE_HOME_NETWORK_LCM:
										HxLOG_Print("+			CREDENTIALS_TYPE_HOME_NETWORK_LCM :  \n");
										break;
									case CREDENTIALS_TYPE_UNKNOWN:
										HxLOG_Print("+			CREDENTIALS_TYPE_UNKNOWN :  \n");
										break;
									default:
										HxLOG_Print("+			error....\n");
										break;
								}
							}
							else
							{
								HxLOG_Print("+			error : Get Credential Information\n"));
							}

							if(pxGenericMetadata)
									xmgr_cas_NadvlReleaseBuffer(pxGenericMetadata);
							if(pxSpecificMetadata)
									xmgr_cas_NadvlReleaseBuffer(pxSpecificMetadata);
						}
						else
						{
							HxLOG_Print("+			error : CredentialHex(Convert)\n");
						}
					}
					HLIB_STD_MemFree(pCredentialHex);
					pCredentialHex = NULL;
				}
				#endif
			}
			else
			{
				HxLOG_Print("+		3. CreData[%d] = (null)\n", pCreInfo->usSize);
			}

			if(pCreInfo->pucSpecificMetaData)
			{
				#if 0
				// TODO
				XmgrCas_NaPrmUsageRule_t *pUR = NULL;
				pUR = &pCreInfo->stUsageRules;
				if(pUR)
				{
					HxSTD_memset(&ucSMD, 0, sizeof(ucSMD));
					_xmgr_cas_naprm_ConvertHexDecimalStr(pCreInfo->pucSpecificMetaData, ucSMD, pCreInfo->ulSpecificMetaDataSize);

					HxLOG_Print("+		4. SMD[%d] = %s\n", pCreInfo->ulSpecificMetaDataSize, ucSMD);
					HxLOG_Print("+			< Usage Rule Info >\n"));
					HxLOG_Print("+			Version = 0x%x\n", pUR->cVersion)); 										//: 2;
					HxLOG_Print("+			Anticipation_flag = 0x%x\n", pUR->cAnticipation_flag); 					//: 1;
					HxLOG_Print("+			Recording_flag = 0x%x\n", pUR->cRecording_flag); 						//: 1;
					HxLOG_Print("+			Original_sub_required = 0x%x\n", pUR->cOriginal_sub_required); 			//: 1;
					HxLOG_Print("+			Dvr_sub_required = 0x%x\n", pUR->cDvr_sub_required); 					//: 1;
					HxLOG_Print("+			Trans_coding_control = 0x%x\n", pUR->cTrans_coding_control); 			//: 1;
					HxLOG_Print("+			Image_constraint = 0x%x\n", pUR->cImage_constraint); 					//: 1;

					HxLOG_Print("+			Trick_mode_control = 0x%x\n", pUR->cTrick_mode_control); 				//: 4;
					HxLOG_Print("+			Playcount = 0x%x\n", pUR->cPlaycount); 									//: 4;

					HxLOG_Print("+			Concurrent_viewing = 0x%x\n", pUR->cConcurrent_viewing); 				//: 3;
					HxLOG_Print("+			Digital_only_token = 0x%x\n", pUR->cDigital_only_token); 				//: 1;
					HxLOG_Print("+			Unprotected_digital_output_token = 0x%x\n", pUR->cUnprotected_digital_output_token); 	//: 1;
					HxLOG_Print("+			Hdcp_token = 0x%x\n", pUR->cHdcp_token); 								//: 1;
					HxLOG_Print("+			Dtcp_token = 0x%x\n", pUR->cDtcp_token); 								//: 1;
					HxLOG_Print("+			WMDRM_token = 0x%x\n", pUR->cWMDRM_token); 								//: 1;

					HxLOG_Print("+			PRM_token = 0x%x\n", pUR->cPRM_token); 									//: 1;
					HxLOG_Print("+			Others_token = 0x%x\n", pUR->cOthers_token); 							//: 1;
					HxLOG_Print("+			Cci = 0x%x\n", pUR->cCci); 												//: 2;
					HxLOG_Print("+			Unprotected_analog_output_token = 0x%x\n", pUR->cUnprotected_analog_output_token); 	//: 1;
					HxLOG_Print("+			Retention_state = 0x%x\n", pUR->cRetention_state); 						//: 3;
#ifdef SUPPORT_USAGERELE_EXT_DATA
					HxLOG_Print("+			Pc_threshold = 0x%x\n", pUR->cPc_threshold); 							//: 4;	/* UR_ext_data */
					HxLOG_Print("+			Pc_bitmap = 0x%x\n", pUR->sPc_bitmap); 									//: 12;	/* UR_ext_data */
#endif
					HxLOG_Print("\n+			< Recovery Usage Rule Control Status>\n");
#if defined(SUPPORT_UR_HDCP)
					HxLOG_Print("+			Hdcp : Activated = %d\n", pUR->stUR_Recovery.stHdcp.ucActivate);
					if(pUR->stUR_Recovery.stHdcp.ucActivate==TRUE)
					{
						HxLOG_Print("+			Hdcp = %d\n", pUR->stUR_Recovery.stHdcp.eMode);
					}
#endif
					HxLOG_Print("+			Macrovision : Activated = %d\n", pUR->stUR_Recovery.stMacrovision.ucActivate);
					if(pUR->stUR_Recovery.stMacrovision.ucActivate==TRUE)
					{
						HxLOG_Print("+			Macrovision = %d\n", pUR->stUR_Recovery.stMacrovision.eMode);
					}
					HxLOG_Print("+			AV Output : Activated = %d\n", pUR->stUR_Recovery.stAVOutput.ucActivate);
					if(pUR->stUR_Recovery.stAVOutput.ucActivate==TRUE)
					{
						HxLOG_Print("+			AV Output : video = %d, audio = %d\n", pUR->stUR_Recovery.stAVOutput.eAVidOut, pUR->stUR_Recovery.stAVOutput.eAAudOut);
					}
				}
				#endif
			}
			else
				HxLOG_Print("+		4. SMD[%d] = (null)\n", pCreInfo->ulSpecificMetaDataSize);
#ifdef SUPPORT_CREDENTIAL_GMD
			if(pCreInfo->pucGenericMetaData)
				HxLOG_Print("+		5. GMD[%d] = %s\n", pCreInfo->ulGenericMetaDataSize, pCreInfo->pucGenericMetaData);
			else
				HxLOG_Print("+		5. GMD[%d] = (null)\n", pCreInfo->ulGenericMetaDataSize);
#endif
		}
	}
	HxLOG_Print("\n");
}


STATIC void _xmgr_cas_naprm_PrintCopyControl(MgrCopyrightControl_OutputControlInfo_t *pstCopyControl)
{
	if(pstCopyControl == NULL)
	{
		return;
	}

	HxLOG_Print("+++++++++++++++++++ Print PRM CopyControl Data +++++++++++++++++++\n");
	HxLOG_Print("+ bSetMacrovision = %d\n", pstCopyControl->bSetMacrovision);
	HxLOG_Print("+ eMacrovision = %d\n", pstCopyControl->eMacrovision);

	HxLOG_Print("+ bSetHdcp = %d\n", pstCopyControl->bSetHdcp);
	HxLOG_Print("+ bHdcp = %d\n", pstCopyControl->bHdcp);

	HxLOG_Print("+ bSetScmsCopyRight = %d\n", pstCopyControl->bSetScmsCopyRight);
	HxLOG_Print("+ eScmsCopyRight = %d\n", pstCopyControl->eScmsCopyRight);

	HxLOG_Print("+ bSetCgmsCopyRight = %d\n", pstCopyControl->bSetCgmsCopyRight);
	HxLOG_Print("+ eCgmsCopyRight = %d\n", pstCopyControl->eCgmsCopyRight);

	HxLOG_Print("+ bSetCgmsMacrovision = %d\n", pstCopyControl->bSetCgmsMacrovision);
	HxLOG_Print("+ eCgmsMacrovision = %d\n", pstCopyControl->eCgmsMacrovision);

	HxLOG_Print("+ bOutputMute = %d\n", pstCopyControl->bOutputMute);

	HxLOG_Print("+ bDigitalOnlyConstraint = %d\n", pstCopyControl->bDigitalOnlyConstraint);
}

STATIC void _xmgr_cas_naprm_PrintDrmResultMessage(XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg, XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr)
{
	if(pstDrmRstMsg==NULL || pstCreHdlMgr==NULL)
	{
		HxLOG_Error("pstDrmRstMsg or pstCreHdlMgr is NULL\n");
		return;
	}
	_xmgr_cas_naprm_PrintDrmHandleManager(pstCreHdlMgr);

	HxLOG_Print("++++++++++++++++++++ << Print DRM Result Msg >> ++++++++++\n");
	HxLOG_Print("+	CREH = %d\n", pstDrmRstMsg->hCredetialHandle);
	HxLOG_Print("+	Stat(Playback) = %d  \n", pstDrmRstMsg->hStat);
	HxLOG_Print("+	ErrorCode = %d  \n", pstDrmRstMsg->hErrorCode);

	HxLOG_Print("+	MsgID = %d  \n", pstDrmRstMsg->ucMsgID);
	HxLOG_Print("+	Msg = [%s] size[%d]  \n", pstDrmRstMsg->pucMsg, pstDrmRstMsg->ulMsgSize);
	HxLOG_Print("+	ResultCode = %d  \n", pstDrmRstMsg->eResultCode);
	HxLOG_Print("\n\n");
	return;
}

STATIC void _xmgr_cas_naprm_PrintDrmInformation(MgrMedia_DrmInformation_t *pstDrmInfo)
{
	HUINT16 i = 0;
	if(pstDrmInfo == NULL)
	{
		return;
	}

	HxLOG_Print("++++++++++++++++++++ << Print DRM Informatino >> ++++++++++\n");
	HxLOG_Print("+	eDRMType = %d\n", pstDrmInfo->eDRMType);
	HxLOG_Print("+	eEncryptionType = %d\n", pstDrmInfo->eEncryptionType);
	HxLOG_Print("+	eStreamType = %d\n", pstDrmInfo->eStreamType);
	HxLOG_Print("+	ulTransportSessionId = %d\n", pstDrmInfo->ulTransportSessionId);
	HxLOG_Print("+	hPlayBackHandle = %d\n", pstDrmInfo->hPlayBackHandle);
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	HxLOG_Print("+	usEmi = %d\n", pstDrmInfo->usEmi);
	HxLOG_Print("+	ulKeyHandle = %d\n", pstDrmInfo->ulKeyHandle);
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	HxLOG_Print("+	ulKeyIdSize = %d\n", pstDrmInfo->ulKeyIdSize);
	HxLOG_Print("+	aucKeyId = [ ");
	for(i = 0; i < pstDrmInfo->ulKeyIdSize; i++)
	{
		HxLOG_Print("%2X ", pstDrmInfo->aucKeyId[i]);
	}
	HxLOG_Print("]\n");
#endif
#else
	HxLOG_Print("+	ulKeyIdSize = %d\n", pstDrmInfo->ulKeySize);
	HxLOG_Print ("+	ucEncryptKey = [ ");
	for(i = 0; i < pstDrmInfo->ulKeySize; i++)
	{
		HxLOG_Print ("%2X ", pstDrmInfo->ucEncryptKey[i]);
	}
	HxLOG_Print ("]\n");
#endif
}

void _xmgr_cas_naprm_PrintMetaInfo(
        TCredentialsType    xCredentialsType,
  		TCredentialsUnion* pxCredentialsUnion,
        TSize               xGenericMetadataSize,
  		TUnsignedInt8*     pxGenericMetadata,
        TSize               xSpecificMetadataSize,
  		TUnsignedInt8*     pxSpecificMetadata
)
{
	HxLOG_Print("###########################################################\n");
	HxLOG_Print("###########################################################\n");
	HxLOG_Print("\t xCredentialsType : %d\n", xCredentialsType);
	switch(xCredentialsType)
	{
		case CREDENTIALS_TYPE_PROFILE:
			{
				TProfileData	profileData;
				HxDATETIME_t	dateTime;

				HxLOG_Print("+\t\t CREDENTIALS_TYPE_PROFILE :	\n");
				HxSTD_memset(&profileData, 0x00, sizeof(TProfileData));
				HxSTD_memcpy(&profileData, &(pxCredentialsUnion->profileData), sizeof(TProfileData));
				HxLOG_Print("+\t\t * profileData.profileId : %ld\n", profileData.profileId);
				HxLOG_Print("+\t\t * profileData.profileVersion : %ld\n", profileData.profileVersion);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(profileData.credentialsCreationDate, &dateTime);
				HxLOG_Print("+\t\t * credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(profileData.profileExpirationDate, &dateTime);
				HxLOG_Print("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				if (profileData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE)
				{
					HxLOG_Print("+\t\t * profileData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE \n");
				}
				else if (profileData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION)
				{
					HxLOG_Print("+\t\t * profileData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION \n");
				}
				else	// LAST_VIEWING_WINDOW_RELATIVITY
				{
					HxLOG_Print("+\t\t * profileData.contentViewingWindowRelativity : LAST_VIEWING_WINDOW_RELATIVITY \n");
				}

				HxLOG_Print("+\t\t * profileData.contentViewingWindowDuration : %ld \n", profileData.contentViewingWindowDuration);
				HxLOG_Print("+\t\t * profileData.contentRetentionDuration : %ld \n", profileData.contentRetentionDuration);
			}
			break;

		case CREDENTIALS_TYPE_VOD_EMM:
			{
				TVodEmmData		vodEmmData;
				HxDATETIME_t	dateTime;

				HxLOG_Print("+\t\t CREDENTIALS_TYPE_VOD_EMM :	\n");
				HxSTD_memset(&vodEmmData, 0x00, sizeof(TVodEmmData));
				HxSTD_memcpy(&vodEmmData, &(pxCredentialsUnion->vodEmmData), sizeof(TVodEmmData));
				HxLOG_Print("+\t\t * vodEmmData.irdSerialNumber : %ld\n", vodEmmData.irdSerialNumber);
				HxLOG_Print("+\t\t * vodEmmData.contentId : %ld\n", vodEmmData.contentId);
				HxLOG_Print("+\t\t * vodEmmData.vodEmmId : %ld \n", vodEmmData.vodEmmId);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.credentialsCreationDate, &dateTime);
				HxLOG_Print("+\t\t * credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentExpirationDate, &dateTime);
				HxLOG_Print("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HxLOG_Print("+\t\t * vodEmmData.contentViewingWindowDuration : %ld \n", vodEmmData.contentViewingWindowDuration);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentFirstVisualizationDate, &dateTime);
				HxLOG_Print("+\t\t * contentFirstVisualizationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentViewingWindowEndDate, &dateTime);
				HxLOG_Print("+\t\t * contentViewingWindowEndDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE)
				{
					HxLOG_Print("+\t\t * vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE \n");
				}
				else if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION)
				{
					HxLOG_Print("+\t\t * vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION \n");
				}
				else	// LAST_VIEWING_WINDOW_RELATIVITY
				{
					HxLOG_Print("+\t\t * vodEmmData.contentViewingWindowRelativity : LAST_VIEWING_WINDOW_RELATIVITY \n");
				}

				HxLOG_Print("+\t\t * vodEmmData.contentIsCiphered : %d \n", vodEmmData.contentIsCiphered);
				HxLOG_Print("+\t\t * vodEmmData.contentKeyIsProtected : %d \n", vodEmmData.contentKeyIsProtected);
				HxLOG_Print("+\t\t * vodEmmData.nuidValue : 0x%lx \n", vodEmmData.nuidValue);
			}
			break;

		case CREDENTIALS_TYPE_VOD_ECM:
			{
				TVodEcmData		vodEcmData;
				HxDATETIME_t	dateTime;

				HxLOG_Print("+\t\t CREDENTIALS_TYPE_VOD_ECM :	\n");
				HxSTD_memset(&vodEcmData, 0x00, sizeof(TVodEcmData));
				HxSTD_memcpy(&vodEcmData, &(pxCredentialsUnion->vodEcmData), sizeof(TVodEcmData));
				HxLOG_Print("+\t\t * vodEcmData.contentId : %ld\n", vodEcmData.contentId);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEcmData.contentCreationDate, &dateTime);
				HxLOG_Print("+\t\t * contentCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEcmData.contentExpirationDate, &dateTime);
				HxLOG_Print("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HxLOG_Print("+\t\t * vodEcmData.contentEmi : %ld \n", vodEcmData.contentEmi);
				HxLOG_Print("+\t\t * vodEmcData.contentKeyIsProtected : %d \n", vodEcmData.contentKeyIsProtected);
			}
			break;

 		case CREDENTIALS_TYPE_LCM:

			break;

		case CREDENTIALS_TYPE_HOME_NETWORK_LCM:

			break;

		default:

			break;
	}

	HxLOG_Print("\t xGenericMetadataSize : %d\n", xGenericMetadataSize);
#if 1
{
	HUINT32	i;

	for( i = 0; i < xGenericMetadataSize; i ++)
	{
		if(i != 0 && i%7 == 0)
		{
			HxLOG_Debug("\n");
		}
		HxLOG_Debug("[0x%02x]", pxGenericMetadata[i]);

	}
	HxLOG_Debug("\n");
}
#else
	HLIB_LOG_Dump(pxGenericMetadata, xGenericMetadataSize, 0, FALSE);
#endif

	HxLOG_Print("\t xSpecificMetadataSize : %d\n", xSpecificMetadataSize);
#if 1
{
	HUINT32 i;

	for( i = 0; i < xSpecificMetadataSize; i ++)
	{
		if(i != 0 && i%7 == 0)
		{
			HxLOG_Debug("\n");
		}
		HxLOG_Debug("[0x%02x]", pxSpecificMetadata[i]);

	}
	HxLOG_Debug("\n");
}
#else
	HLIB_LOG_Dump(pxSpecificMetadata, xSpecificMetadataSize, 0, FALSE);
#endif
	HxLOG_Print("###########################################################\n");
	HxLOG_Print("###########################################################\n");
}


static void _xmgr_cas_naprm_PrintCredentialInfomation(TSize tCredentialsSize, const TUnsignedInt8* pxCredentials)
{
	HERROR				hErr;
	TCredentialsType 	xCredentialsType;
	TCredentialsUnion	*pxCredentialsUnion;
	TSize				xGenericMetadataSize;
	TUnsignedInt8		*pxGenericMetadata;
	TSize			 	xSpecificMetadataSize;
	TUnsignedInt8		*pxSpecificMetadata;

	HxLOG_Print("tCredentialsSize:[%d], pxCredentials:[%p]\n", tCredentialsSize, pxCredentials);

	if(tCredentialsSize == 0 || pxCredentials == NULL)
	{
		HxLOG_Error("Error tCredentialsSize:[%d], pxCredentials:[%p]\n", tCredentialsSize, pxCredentials);
		return;
	}

	hErr = xmgr_cas_NadvlCredentialsInformationRequest(tCredentialsSize, (const TUnsignedInt8 *)pxCredentials, &xCredentialsType, (const TCredentialsUnion **)&pxCredentialsUnion, &xGenericMetadataSize, (const TUnsignedInt8 **)&pxGenericMetadata, &xSpecificMetadataSize, (const TUnsignedInt8 **)&pxSpecificMetadata);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\t Error in xmgr_cas_NadvlCredentialsInformationRequest, hErr : [%d]\n", hErr);
	}
	else
	{
		_xmgr_cas_naprm_PrintMetaInfo(xCredentialsType, pxCredentialsUnion, xGenericMetadataSize, pxGenericMetadata, xSpecificMetadataSize, pxSpecificMetadata);

		if(pxCredentialsUnion != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer((const TUnsignedInt8 *)pxCredentialsUnion);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}

		if(pxGenericMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxGenericMetadata);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}

		if(pxSpecificMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxSpecificMetadata);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}
	}
}

HUINT8 *_xmgr_cas_naprm_GetDvlStatusName(TDvlAccessStatus status)
{
	switch(status)
	{
		ENUM_TO_STR(DVL_ACCESS_GRANTED);
		/**<
		 *    The playback of the content is granted.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED);
		/**<
		*    The playback of the content is denied.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_CONTENT_EXPIRED);
		/**<
		*    The content's usage rules deny its playback because it is expired.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_INVALID_CREDENTIALS);
		/**<
		*    The provided content Credentials are invalid.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_WRONG_TSID);
		/**<
		*    The transport session identifier is unknown by the SCR at that time.
		*    This status only applies to a playback request.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_LIMIT_EXCEEDED);
		/**<
		*    Deprecated.
		*    This status is not returned by the DVL.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_INVALID_TIME);
		/**<
		*    The internal time reference is not valid. The DVR/VOD Library is not
		*    authorized to play back any content.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_UNKNOWN_STB);
		/**<
		*    The access to the content is denied because it has been
		*    recorded by an unknown set-top box.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_UNKNOWN_HOME_NETWORK);
		/**<
		*    The access to the content is denied because it has been
		*    recorded by a set-top box belonging to an unknow Home Network.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_UNKNOWN_NUID);
		/**<
		*    The access to the content is denied because it has been
		*    recorded by a set-top box with an unknown NUID.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_INVALID_EMI);
		/**<
		*    The EMI used to protect this content is not supported by the SCR
		*    implementation.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_INVALID_KEY_PROTECTION);
		/**<
		*    The keys needed to decipher the content have been protected using a
		*    mechanism not supported by the SCR implementation.
		*/
		ENUM_TO_STR(LAST_DVL_ACCESS_STATUS);
		/**<
		*    Last DVL Playback status value.
		*    This value is never returned by the DVR/VOD Library.
		*/
		default:
			return "Unknown Staus";
	}
}
#endif


/******************************************************************************************
*	Util
******************************************************************************************/
STATIC HINT32 _xmgr_cas_naprm_Util_atoi (HINT8 *pcPtr, HUINT32 ulSize)
{
	HINT32  lVal = 0 ;
	HUINT32 nCount =0 ;

	if (!pcPtr)
	{
		HxLOG_Error("pcPtr is NULL\n");
		return 0;
	}

	while (1)
	{
		if (*pcPtr=='\0' || nCount == ulSize)
		{
			break ;
		}
		if (*pcPtr>='0' && *pcPtr<='9')
		{
			lVal *= 10 ;
			lVal += *pcPtr - '0' ;
		}

		nCount++;
		pcPtr++ ;
	}

 	return lVal ;
}

STATIC void _xmgr_cas_naprm_ConvertHexDecimalStr(HUINT8 *pucSource, HUINT8 *pucDes, HUINT16 usTotalByte)
{
	HUINT16 usIdx;
	HUINT8 ch;
	HUINT8 tmpBuf[DRMAGENT_DRMMESSAGE_MAX_LEN*2];
	HUINT8 strBuf[DRMAGENT_DRMMESSAGE_MAX_LEN*2];

	if(!pucSource || !pucDes)
	{
		HxLOG_Error("pucSource or pucDes is NULL\n");
		return;
	}

	HxSTD_memset(&tmpBuf, 0, sizeof(tmpBuf));
	HxSTD_memset(&strBuf, 0, sizeof(strBuf));

	for(usIdx=0;usIdx<usTotalByte;usIdx++)
	{
		ch = pucSource[usIdx];
		HxSTD_snprintf(tmpBuf, sizeof(tmpBuf), "%02X", ch);
		HxSTD_StrCat(strBuf, tmpBuf);
	}

	HxSTD_memcpy(pucDes, strBuf, HxSTD_StrLen(strBuf));
}

STATIC void _xmgr_cas_naprm_ConvertDecimalStr(HUINT32 ulData, HUINT8 *pucDecStr)
{
	HUINT8 ucTmpBuf[MAX_TEMP_BUF_LENGTH];
	HUINT16 usLen = 0;

	if(!pucDecStr)
	{
		HxLOG_Error("pucDecStr is NULL\n");
		return;
	}
	HxSTD_memset(pucDecStr, 0, sizeof(ucTmpBuf));
	HxSTD_memset(ucTmpBuf, 0, sizeof(ucTmpBuf));
	HxSTD_snprintf(ucTmpBuf, sizeof(ucTmpBuf), "%lu", (HULONG)ulData);
	usLen = HxSTD_StrLen(ucTmpBuf);

	HxSTD_memcpy(pucDecStr, ucTmpBuf, usLen);
}

STATIC HERROR _xmgr_cas_naprm_ConvertDecimalToHex(HUINT8 *pucSource, HUINT32 ulSourceSize, HUINT8 *pucDes, HUINT32 *ulDesSize)
{
	HUINT32 ulIdx = 0;

	if(!pucSource || !pucDes)
	{
		HxLOG_Error("pucSource or pucDes is NULL\n");
		return ERR_FAIL;
	}

	if(ulSourceSize <= 0 )
	{
		HxLOG_Error("ulSourceSize < 0\n");
		return ERR_FAIL;
	}

	while(1)
	{
		pucDes[ulIdx] = MAKE_STROF2BYTE_2_NUM(pucSource);
		pucSource = pucSource + 2;
		ulIdx++;
		if(ulIdx >= (ulSourceSize/2))
			break;
	}

	*ulDesSize = ulSourceSize/2;

	return ERR_OK;
}

/******************************************************************************************
*
*	Manage SMD/GMD UsageRule
*
******************************************************************************************/
#ifdef SUPPORT_CREDENTIAL_GMD
static HERROR _xmgr_cas_naprm_CreateGenericMetadata(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle, HUINT32 ulSize, const HUINT8 *pucSource)
{
	HERROR hErr = ERR_OK;
	HUINT16 usIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr || !pucSource || ulSize<=0)
	{
		HxLOG_Error("pstCreHdlMgr or pucSource is NULL\n");
		return ERR_FAIL;
	}
	if(ulHandle<=INVALID_CREDENTIAL_HANDLE || ulHandle>MAX_CREDENTIAL_NUMBER)
	{
		HxLOG_Error("ulHandle is NULL\n");
		return ERR_FAIL;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle == ulHandle)
		{
			if(pCreInfo->pucGenericMetaData)
			{
				HLIB_STD_MemFree(pCreInfo->pucGenericMetaData);
				pCreInfo->pucGenericMetaData = NULL;
				pCreInfo->ulGenericMetaDataSize = 0;
			}
			pCreInfo->pucGenericMetaData = (HUINT8*)HLIB_STD_MemAlloc(ulSize+1);
			if(pCreInfo->pucGenericMetaData)
			{
				HxSTD_memset(pCreInfo->pucGenericMetaData, 0, ulSize+1);
				HxSTD_memcpy(pCreInfo->pucGenericMetaData, pucSource, ulSize);
				pCreInfo->ulGenericMetaDataSize = ulSize;
			}
			else
			{
				HxLOG_Error("pucGenericMetaData is NULL\n");
				hErr = ERR_FAIL;
			}
			break;
		}
	}
	return hErr;
}

static void xmgr_cas_naprm_CloseGenericMetadataByIndex(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT16 usIndex)
{
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL\n");
		return;
	}

	pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIndex];

	if(pCreInfo==NULL)
	{
		HxLOG_Error("pCreInfo is NULL\n");
		return;
	}

	if(pCreInfo->pucGenericMetaData)
	{
		HLIB_STD_MemFree(pCreInfo->pucGenericMetaData);
		pCreInfo->pucGenericMetaData = NULL;
		pCreInfo->ulGenericMetaDataSize = 0;
	}
}
#endif

#if defined(SUPPORT_USAGERULE)
#if defined(SUPPORT_USAGERULE_RECOVERY)
STATIC void xmgr_cas_naprm_RecoveryDefaultUsageRule(void)
{
	MgrCopyrightControl_OutputControlInfo_t	stCopyControl;
	HxSTD_memset(&stCopyControl, 0, sizeof(MgrCopyrightControl_OutputControlInfo_t));

	/* play stop시 usageRule을 default로 적용되게 한다 */
	(void)BUS_PostData(NULL, eMEVT_MEDIA_UPDATE_COPYCONTROL_INFO, HANDLE_NULL, 0, 0, (void*)&stCopyControl, sizeof(MgrCopyrightControl_OutputControlInfo_t));
}
#endif

STATIC void xmgr_cas_naprm_ProcessUsageRule(XmgrCas_NaPrmUsageRule_t *pstUR)
{
	MgrCopyrightControl_OutputControlInfo_t	stCopyControl;

	if(!pstUR)
	{
		HxLOG_Error("pstUR is NULL\n");
		return;
	}

	HxSTD_memset(&stCopyControl, 0, sizeof(MgrCopyrightControl_OutputControlInfo_t));
	/*
		anticipation_flag: This is a 1-bit field. Possible values are:
		0  The usage rules are related to current event.
		1  The usage rules are anticipated (related to next event).
		The  default  value  is  0.  This  flag  is  used  when  recording  a  broadcast  feed.  Usage  rules  can  be
		delivered  in  advance  to  the  event  (as  announced  in  the  EPG)  they  apply.  This  allows  the
		application to properly prepare the recording session.
	 */
	//pstUR->cAnticipation_flag;			/* NOT SUPPORT */

	/*
		recording_flag: This is a 1-bit field. Possible values are:
		0  Recording is not allowed.
		1  Protected recording is allowed.
		The default value is 1. This flag does not apply to the review buffer  and to transient storage for
		the  immediate  rendering,  which  are  governed  by  other  fields.  This  applies  to  whether  a
		persistent copy is allowed (e.g. after the device is shut-down).
	*/
 	//pstUR->cRecording_flag;				/* NOT SUPPORT */

	/*
		original_sub_required: This is a 1-bit field. Possible values are:
		0  At  playback  time,  no  valid  subscription  on  the  channel  content  has  been  recorded  is needed.
		1  At  playback  time,  a  valid  subscription  on  the  channel  content  has  been  recorded  is needed.
		The default value is 0.
	*/
	//pstUR->cOriginal_sub_required;		/* NOT SUPPORT */

	/*
		dvr_sub_required: This is a 1-bit field. Possible values are:
		0  At recording time, no valid DVR subscription is needed.
		1  At recording time, a valid DVR subscription is needed.
		The default value is 0.
	*/
	//pstUR->cOriginal_sub_required;		/* NOT SUPPORT */

	/*
		trans_coding_control: This is a 1-bit field. Possible values are:
		0  Trans-coding or re-encoding of content is not allowed.
		1  Trans-coding or re-encoding of content is allowed.
		The default value is 1.
	*/
 	//pstUR->cTrans_coding_control			/* NOT SUPPORT */

	/*
		trick_mode_control: This is a 4-bit field. Possible values are:
		0000  No restrictions.
		0001  No skipping, fast forward and fast backward limited to Speed 1.
		0010  No skipping, fast forward and fast backward limited to Speed 2.
		0011  No skipping, fast forward and fast backward limited to Speed 3.
		…
		1110  No skipping, fast forward and fast backward limited to Speed n.
		1111  No skipping/jumping/fast forward/fast backward. (see section 4.2 for details)
		Speeds 1 to n are those supported by the application on the device. The default value is 0000.

		< In case of SES Spec >
		0000	no restrictions
		1111 	Only those trick-mode functions are allowed which are initiated and controlled directly by the HbbTV application;
				all other trick-mode functions shall be disabled, see chapter 0 in the NAGRA documentation [11] for details.
	*/
	//pstUR->cTrick_mode_control;			/* NOT SUPPORT */

	/*
		playcount:  This  is  a  4-bit  field.  This  is  the  maximum  number  of  times  playback  of  content  is
		allowed. If set to 0, then the number of playbacks is not limited. Actual playcount value is stored
		separately. The default value is 0.
	*/
	//pstUR->cPlaycount;					/* NOT SUPPORT */

	/*
		concurrent_viewing: This is a 3-bit field. This is the maximum number of clients a gateway is
		allowed to concurrently stream to. Possible values are:
		000  Local viewing only
		001  Concurrent streaming to a maximum of 1 client is allowed
		010  Concurrent streaming to a maximum of 2 clients is allowed
		011  Concurrent streaming to a maximum of 3 clients is allowed
		100  Concurrent streaming to a maximum of 4 clients is allowed
		101  Concurrent streaming to a maximum of 8 clients is allowed
		110  Concurrent streaming to a maximum of 12 clients is allowed
		111  Concurrent streaming to an unlimited number of clients is allowed
		The default value is 110.
	*/
	//pstUR->cConcurrent_viewing;			/* NOT SUPPORT */

	/*
		digital_only_token: This is a 1-bit field. Possible values are:
		0  Output of decrypted content is allowed on Analog Outputs.
		1  Output of decrypted content is not allowed on Analog outputs.
		The default value is 0.
	*/


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	/* AS-10001_HD_SmartTV_STB_Requirements_v-1-0.pdf
	   Figure 10: Activity diagram showing the evaluation and processing of NAGRA Media PRM Content Usage Rules
	   에 의거해서 만들어짐.
	**/

	//pstUR->cRecording_flag				/* Not Support */
	//pstUR->cTrans_coding_control			/* Not Support */

	/* trick_mode_control
  	   0000 : no restrictions
       1111 : Only those trick-mode functions are allowed which are initiated and controlled directly by the HbbTV application;
              all other trick-mode functions shall be disabled, see chapter 0 in the NAGRA documentation [11] for details.
       Note: Any other bitmaps of the trick mode control bits (0001, 0010 etc.) are not used in the context of HD+ SmartTV.
    **/
	//pstUR->cTrick_mode_control			/* Not Support */

	/* unprotected_digital_output_token
	   0 : Output of content on unprotected digital outputs is not allowed.
       1 : Output of content on unprotected digital outputs is allowed.
       Note: If this bit is set to 1 then the hdcp_token, dtcp_token (and related cci, retention_state and concurrent_viewing bits),
             the WMDRM_token, PRM_token and others_token shall be ignored.
    **/
	if(pstUR->cUnprotected_digital_output_token == 0)
	{
		/* hdcp_token
		   0 : Output of content on HDCP is not allowed.
           1 : Output of content on HDCP is allowed.
           Note: An HDCP Token set to 1 in the context of HD+ SmartTV means that the digital output is only allowed via HDMI with HDCP activated.
                 Any other interfaces using HDCP for copy protection shall be disabled.
        **/
		if(pstUR->cHdcp_token == 1)
		{
			stCopyControl.bSetHdcp = TRUE;
			stCopyControl.bHdcp = TRUE;
		}
		else
		{
			stCopyControl.bSetHdcp = TRUE;
			stCopyControl.bHdcp = FALSE;
		}

		//pstUR->cWMDRM_token				/* Not Support */
		//pstUR->cPRM_token					/* Not Support */
		//pstUR->cOthers_token				/* Not Support */

		/* HD+ SmartTV does not require the implementation of DTCP into the STB.
		   An HD+ SmartTV STB which does not support DTCP shall therefore evaluate the dtcp_token bit as set to 0.
		**/
		//if(pstUR->cDtcp_token == 1)		/* Not Support */
		//{
		//	pstUR->cConcurrent_viewing		/* Not Support */
		//	pstUR->cCci						/* Not Support */
		//	pstUR->cRetention_state			/* Not Support */
		//}
		//else
		//{
		//	tUR->cConcurrent_viewing		/* Ignore */
		//	pstUR->cCci						/* Ignore */
		//	pstUR->cRetention_state			/* Ignore */
		//}
	}
	else
	{
		stCopyControl.bSetHdcp = TRUE;
		stCopyControl.bHdcp = FALSE;
		//pstUR->cHdcp_token				/* Ignore */
		//stUR->cDtcp_token					/* Ignore */
		//pstUR->cConcurrent_viewing		/* Ignore */
		//pstUR->cCci						/* Ignore */
		//pstUR->cRetention_state			/* Ignore */
		//pstUR->cWMDRM_token				/* Ignore */
		//pstUR->cPRM_token					/* Ignore */
		//pstUR->cOthers_token				/* Ignore */
	}

	/* digital_only_token
	   0 : Output of content is allowed on analogue outputs (e.g. RCA, SCART, YPbPr).
       1 : Output of content is not allowed on analogue outputs (e.g. RCA, SCART, YPbPr).
       Note: If the digital_only_token bit is set to 1 then the unprotected_analog_output_token and the image_constraint bit shall be ignored.
    **/
	if(pstUR->cDigital_only_token == 0)
	{
		stCopyControl.bDigitalOnlyConstraint = FALSE; /* analog output : enable */
		/* unprotected_analog_output_token
		   0 : Output of content on unprotected analogue outputs is not allowed.
		       In the context of HD+ SmartTV this means Macrovision shall be activated. If Macrovision is not implemented on the STB all analogue interfaces shall be switched off completely.
           1 : Output of content on unprotected analogue outputs is allowed.
           Note: In deviation from the requirements set forth in chapters 4.3 and 4.4 of [11] an HD+ SmartTV STB may not implement CGMS-A,
                 APS trigger bits and RCD on the analogue outputs.
                 If the digital_only_token is set to 1 the unprotected_analog_output_token shall be ignored.
        **/
		if(pstUR->cUnprotected_analog_output_token == 0)
		{
			stCopyControl.bSetMacrovision = TRUE;
			stCopyControl.eMacrovision = eMACROVISION_AGC;
		}
		else
		{
			stCopyControl.bSetMacrovision = TRUE;
			stCopyControl.eMacrovision = eMACROVISION_OFF;
		}
		//pstUR->cImage_constraint					/* Not Support */
	}
	else
	{
		stCopyControl.bDigitalOnlyConstraint = TRUE; /* analog output : disable */
		//pstUR->cUnprotected_analog_output_token	/* Ignore */
		//pstUR->cImage_constraint					/* Ignore */
	}

	HxLOG_Print("\n");
	HxLOG_Print("\t+ Unprotected_digital_output_token = 0x%x\n", pstUR->cUnprotected_digital_output_token);
	HxLOG_Print("\t+ Hdcp_token = %d\n", pstUR->cHdcp_token);
	HxLOG_Print("\t+ Digital_only_token = 0x%x\n", pstUR->cDigital_only_token);
	HxLOG_Print("\t+ Unprotected_analog_output_token = 0x%x\n", pstUR->cUnprotected_analog_output_token);

	HxLOG_Print("\n");
	HxLOG_Print("\t+ bSetHdcp(%d)\n", stCopyControl.bSetHdcp);
	HxLOG_Print("\t+ bHdcp(%d)\n", stCopyControl.bHdcp);
	HxLOG_Print("\t+ bSetMacrovision(%d)\n", stCopyControl.bSetMacrovision);
	HxLOG_Print("\t+ eMacrovision(%d)\n", stCopyControl.eMacrovision);
	HxLOG_Print("\t+ bDigitalOnlyConstraint(%d)\n", stCopyControl.bDigitalOnlyConstraint);
#else
	if(pstUR->cDigital_only_token == 0)
	{
		stCopyControl.bDigitalOnlyConstraint = FALSE;

		/*
			image_constraint: This is a 1-bit field. Possible values are:
			0  Downscaled from HD analog video to SD analog video on the component video interface is not required.
			1  Downscaled from HD analog video to SD analog video on the  component video interface is required.
			The default value is 1.
		*/
		//pstUR->cImage_constraint;		/* NOT SUPPORT : component를 지원하는 모델은 동작 확인 필요 */

		/*
			unprotected_analog_output_token: This is a 1-bit field. Possible values are:
			0  Output of decrypted content on Unprotected Analog Outputs is not allowed.
			1  Output of decrypted content on Unprotected Analog outputs is allowed.
			The default value is 0
		*/
		if(pstUR->cUnprotected_analog_output_token == 0)
		{
			stCopyControl.bSetMacrovision = TRUE;
			stCopyControl.eMacrovision = eMACROVISION_AGC;

			stCopyControl.bSetCgmsCopyRight = TRUE;
			/*
				cci: This is a 2-bit field. Possible values are those defined in [DTCP] for the same field and are:
				00  Copy-control_not_asserted
				01  No-more-copies
				10  Copy-one-generation
				11  Copy-Never
			*/
			switch(pstUR->cCci)
			{
				case 0b00:
					stCopyControl.eCgmsCopyRight = eCOPYRIGHT_COPY_FREE;
					break;

				case 0b01:
					stCopyControl.eCgmsCopyRight = eCOPYRIGHT_COPY_NOMORE;
					break;

				case 0b10:
					stCopyControl.eCgmsCopyRight = eCOPYRIGHT_COPY_ONCE;
					break;

				case 0b11:
					stCopyControl.eCgmsCopyRight = eCOPYRIGHT_COPY_NEVER;
					break;

				default:
					stCopyControl.eCgmsCopyRight = eCOPYRIGHT_COPY_FREE;
					break;
			}
		}
		else
		{
			stCopyControl.bSetMacrovision = TRUE;
			stCopyControl.eMacrovision = eMACROVISION_OFF;

			stCopyControl.bSetCgmsCopyRight = TRUE;
			stCopyControl.eCgmsCopyRight = eCOPYRIGHT_COPY_FREE;
		}
	}
	else
	{
		stCopyControl.bDigitalOnlyConstraint = TRUE;
	}

	/*
		unprotected_digital_output_token: This is a 1-bit field. Possible values are:
		0  Output of decrypted content on Unprotected Digital Outputs is not allowed.
		1  Output of decrypted content on Unprotected Digital outputs is allowed.
		The default value is 0
	*/
	if(pstUR->cUnprotected_digital_output_token == 0)
	{
		/*
			Uncompressed digital output
			No  uncompressed  digital  outputs  are  allowed  without  the  application  of  a  protection  technology
			unless  the  unprotected_digital_output_token  is  set  to  1.   HDCP  is  the  required  protection  on
			HDMI, DVI and DisplayPort.
		*/
		stCopyControl.bSetHdcp = TRUE;
		stCopyControl.bHdcp = TRUE;
	}
	else
	{
		stCopyControl.bSetHdcp = TRUE;
		stCopyControl.bHdcp = FALSE;
	}

	/*
		hdcp_token: This is a 1-bit field. Possible values are:
		0  Output of content on HDCP is not allowed
		1  Output of content on HDCP is allowed.
		The default value is 1.
	*/
	if(pstUR->cHdcp_token == 0)
	{
		//Uncompressed digital output
		//On HDMI, DVI and DisplayPort outputs, the device shall not output content if hdcp_token is set to 0.
		//HDMI 만 mute 시켜야 듯.
	}

	/*
		dtcp_token: This is a 1-bit field. Possible values are:
		0  Output of content on DTCP is not allowed.
		1  Output of content on DTCP is allowed.
		The default value is 1.
	*/
	//pstUR->cDtcp_token;				/* NOT SUPPORT */

	/*
		WMDRM_token: This is a 1-bit field. Possible values are:
		0  Output of content on [WMDRM] and future versions is not allowed.
		1  Output of content on [WMDRM] and future versions is allowed.
		The default value is 0.
	*/
	//pstUR->cWMDRM_token;			/* NOT SUPPORT */

	/*
		PRM_token: This is a 1-bit field. Possible values are:
		0  Output of content on PRM is not allowed.
		1  Output of content on PRM is allowed.
		The default value is 1. This field does not apply for the refurbishment functionality.
	*/
	//pstUR->cPRM_token;				/* NOT SUPPORT */

	/*
		others_token: This is a 1-bit field. Possible values are:
		0  Output of content on any other protection systems is not allowed.
		1  Output of content on all other protection systems is allowed.
		This token	constitutes the output	control for any and all other  content protection systems  other
		than HDCP, DTCP, PRM and WMDRM. The default value is 0.
	*/
	//pstUR->cOthers_token;			/* NOT SUPPORT */

	/*
		retention_state: This is a 3-bit field. Possible values are those defined in [DTCP] for the same
		field and are:
		000  Forever
		001  1 week
		010  2 days
		011  1 day
		100  12 hours
		101  6 hours
		110  3 hours
		111  90 minutes
		The default value is 111.
	*/
	//pstUR->cRetention_state;			/* NOT SUPPORT */
#endif

#ifdef SUPPORT_USAGERELE_EXT_DATA
	/*
		pc_threshold:  This is  a  4-bit  field.  It  denotes  the  parental  control  threshold  conveyed  into  the
		dataset. It has to be compared to the parental control threshold of the device. Access is denied if
		the conveyed value is greater than the value of the device.
	*/
	//pstUR->cPc_threshold;			/* NOT SUPPORT */

	/*
		pc_bitmap:  This  is  a  12-bit  field.  It  denotes  the  parental  control  bitmap  conveyed  into  the
		dataset. Bits that are set indicate the kind of content that is not allowed. Access is denied if the
		bitmap  of  the  ECM  intersects  the  bitmap  of  the  device.  The  semantics  of  each  bit  is  Operator
		dependent.
	*/
	//pstUR->sPc_bitmap;			/* NOT SUPPORT */
#endif

#ifdef CONFIG_DEBUG
	_xmgr_cas_naprm_PrintCopyControl(&stCopyControl);
#endif

	(void)BUS_PostData(NULL, eMEVT_MEDIA_UPDATE_COPYCONTROL_INFO, HANDLE_NULL, 0, 0, (void*)&stCopyControl, sizeof(MgrCopyrightControl_OutputControlInfo_t));

	return;
}

HERROR xmgr_cas_naprm_ParseUsageRule(HBOOL bDescriptorTagFlag, HUINT8 *pucSpecificMatadata, XmgrCas_NaPrmUsageRule_t *pstUR)
{
	HUINT8 ucTag = 0;
	HUINT8 ucLen = 0;
	HUINT16 usIdx = 0;
	XmgrCas_NaPrmUsageRule_t stUR;
	HUINT8 *pucSMD = pucSpecificMatadata;

	if(!pucSMD || !pstUR)
	{
		HxLOG_Error("pucSMD or pstUR is NULL\n");
		return ERR_FAIL;
	}
	if(bDescriptorTagFlag)
	{
		ucTag = pucSMD[usIdx++];
		ucLen = pucSMD[usIdx++];

#if defined(CONFIG_MW_CAS_NAGRA_APS_DVLTEST)
		MWNagraPrmData(DBG_PRINT, (C_CODE_F_YELLOW"[local_mw_nagra_prm_ParseUsageRule] skip 2byte (0xA50x) in the DVL Test"C_CODE_RESET"\n"));
		usIdx += 2;
#else
		if(ucTag != PRM_USAGE_RULE_TAG
		  || (ucLen != PRM_USAGE_RULE_LENGTH
#ifdef SUPPORT_USAGERELE_EXT_DATA
		  && ucLen != PRM_USAGE_RULE_EXT_LENGTH
#endif
		  ))
		{
			HxLOG_Error("ucTag(0x%x)(0x%x) or ucLen(%d)(%d) is Error\n", ucTag, PRM_USAGE_RULE_TAG, ucLen, PRM_USAGE_RULE_LENGTH);
			return ERR_FAIL;
		}
#endif
	}

	HxSTD_memset(&stUR, 0, sizeof(XmgrCas_NaPrmUsageRule_t));

	stUR.cVersion	= MAKE_USAGERULE_VAL(pucSMD[usIdx], 6, 2);
	stUR.cAnticipation_flag = MAKE_USAGERULE_VAL(pucSMD[usIdx], 5, 1);
	stUR.cRecording_flag = MAKE_USAGERULE_VAL(pucSMD[usIdx], 4, 1);
	stUR.cOriginal_sub_required = MAKE_USAGERULE_VAL(pucSMD[usIdx], 3, 1);
	stUR.cDvr_sub_required = MAKE_USAGERULE_VAL(pucSMD[usIdx], 2, 1);
	stUR.cTrans_coding_control = MAKE_USAGERULE_VAL(pucSMD[usIdx], 1, 1);
	stUR.cImage_constraint = MAKE_USAGERULE_VAL(pucSMD[usIdx], 0, 1);

	usIdx++;
	stUR.cTrick_mode_control = MAKE_USAGERULE_VAL(pucSMD[usIdx], 4, 4);
	stUR.cPlaycount = MAKE_USAGERULE_VAL(pucSMD[usIdx], 0, 4);

	usIdx++;
	stUR.cConcurrent_viewing = MAKE_USAGERULE_VAL(pucSMD[usIdx], 5, 3);
	stUR.cDigital_only_token = MAKE_USAGERULE_VAL(pucSMD[usIdx], 4, 1);
	stUR.cUnprotected_digital_output_token = MAKE_USAGERULE_VAL(pucSMD[usIdx], 3, 1);
	stUR.cHdcp_token = MAKE_USAGERULE_VAL(pucSMD[usIdx], 2, 1);
	stUR.cDtcp_token = MAKE_USAGERULE_VAL(pucSMD[usIdx], 1, 1);
	stUR.cWMDRM_token = MAKE_USAGERULE_VAL(pucSMD[usIdx], 0, 1);

	usIdx++;
	stUR.cPRM_token = MAKE_USAGERULE_VAL(pucSMD[usIdx], 7, 1);
	stUR.cOthers_token = MAKE_USAGERULE_VAL(pucSMD[usIdx], 6, 1);
	stUR.cCci = MAKE_USAGERULE_VAL(pucSMD[usIdx], 4, 2);
	stUR.cUnprotected_analog_output_token = MAKE_USAGERULE_VAL(pucSMD[usIdx], 3, 1);
	stUR.cRetention_state = MAKE_USAGERULE_VAL(pucSMD[usIdx], 0, 3);

#ifdef SUPPORT_USAGERELE_EXT_DATA
	if(stUR.cVersion==0x01)
	{
		HINT32 ucTmp = 0;
		usIdx++;
		stUR.cPc_threshold = (pucSMD[usIdx]>>4)&0x0f;
		/* UR_ext_data */
		ucTmp = pucSMD[usIdx]&0x0f;
		usIdx++;
		stUR.sPc_bitmap = ((ucTmp<<8) | pucSMD[usIdx])&0xfff;
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_APS_DVLTEST)
	return ERR_OK;
#endif

	HxSTD_memcpy(pstUR, &stUR, sizeof(XmgrCas_NaPrmUsageRule_t));

#ifdef CONFIG_DEBUG
	_xmgr_cas_naprm_PrintUsageRules(pstUR);
#endif

	return ERR_OK;
}

#if defined(SUPPORT_USAGERULE_RECOVERY_OUTPUT)
STATIC void xmgr_cas_naprm_RecoveryUsageRule(XmgrCas_NaPrmRecoveryUsageRule_t *pstRecoveryUR)
{
	// TODO : Recovery 해주어야함.
	if(!pstRecoveryUR)
	{
		HxLOG_Error("pstRecoveryUR is NULL\n");
		return;
	}

#if defined(SUPPORT_UR_HDCP)
	// HDCP Recovery
	if(pstRecoveryUR->stHdcp.ucActivate==TRUE)
	{
		//MW_OUTPUT_ChangeHdcpMode(pstRecoveryUR->stHdcp.eMode);
	}
#endif
	// Macrovision Recovery
	if(pstRecoveryUR->stMacrovision.ucActivate==TRUE)
	{
		//MW_OUTPUT_ChangeApsMode(pstRecoveryUR->stMacrovision.eMode);
	}
	// Analog Video/Audio Recovery
	if(pstRecoveryUR->stAVOutput.ucActivate==TRUE)
	{
		if(pstRecoveryUR->stAVOutput.eAVidOut==eOutFlag_Enable)
		{
			//PAL_OUT_EnableAnalogVideo();
			//PAL_OUT_EnableAnalogAudio();
		}
		else
		{
			//PAL_OUT_DisableAnalogVideo();
			//PAL_OUT_DisableAnalogAudio();
		}
	}

	HxSTD_memset(pstRecoveryUR, 0, sizeof(XmgrCas_NaPrmRecoveryUsageRule_t));
	return;
}
#endif

STATIC void xmgr_cas_naprm_CloseUsageRuleFromSpecificMetadataByIndex(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT16 usIndex)
{
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL\n");
		return;
	}
	pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIndex];
	if(pCreInfo==NULL)
	{
		HxLOG_Error("pCreInfo is NULL\n");
		return;
	}

#if defined(SUPPORT_USAGERULE_RECOVERY_OUTPUT)
	xmgr_cas_naprm_RecoveryUsageRule(&(pCreInfo->stUsageRules.stUR_Recovery));
#endif

	if(pCreInfo->pucSpecificMetaData)
	{
		HLIB_STD_MemFree(pCreInfo->pucSpecificMetaData);
		pCreInfo->pucSpecificMetaData = NULL;
		pCreInfo->ulSpecificMetaDataSize = 0;
	}
	return;
}

STATIC HERROR _xmgr_cas_naprm_CreageUsageRuleFromSpecificMetadata(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle, HUINT32 ulSize, const HUINT8 *pucSource)
{
	HERROR hErr = ERR_OK;
	HUINT16 usIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr || !pucSource || ulSize<=0)
	{
		HxLOG_Error("pstCreHdlMgr or pucSource is NULL, ulSize(%d)\n", ulSize);
		return ERR_FAIL;
	}
	if(ulHandle <= INVALID_CREDENTIAL_HANDLE || ulHandle > MAX_CREDENTIAL_NUMBER)
	{
		HxLOG_Error("ulHandle(ulHandle) is error\n", ulHandle);
		return ERR_FAIL;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle == ulHandle)
		{
			if(pCreInfo->pucSpecificMetaData)
			{
				HLIB_STD_MemFree(pCreInfo->pucSpecificMetaData);
				pCreInfo->pucSpecificMetaData = NULL;
				pCreInfo->ulSpecificMetaDataSize = 0;
			}
			pCreInfo->pucSpecificMetaData = (HUINT8*)HLIB_STD_MemAlloc(ulSize+1);
			if(pCreInfo->pucSpecificMetaData)
			{
				HxSTD_memset(pCreInfo->pucSpecificMetaData, 0, ulSize+1);
				HxSTD_memcpy(pCreInfo->pucSpecificMetaData, pucSource, ulSize);
				pCreInfo->ulSpecificMetaDataSize = ulSize;
#if defined(SUPPORT_USAGERULE)
				if(xmgr_cas_naprm_ParseUsageRule(TRUE, pCreInfo->pucSpecificMetaData, &pCreInfo->stUsageRules) == ERR_OK)
				{
					xmgr_cas_naprm_ProcessUsageRule(&pCreInfo->stUsageRules);
				}
#endif
			}
			else
			{
				HxLOG_Error("pucSpecificMetaData is NULL\n");
				hErr = ERR_FAIL;
			}
			break;
		}
	}
	return hErr;
}
#endif

/******************************************************************************************
*
*	Manage credential Handle
*
******************************************************************************************/
STATIC eXmgrCas_NaPrmResultCodeStatus_t xmgr_cas_naprm_CreateCredentialInfo(XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg, XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr,HUINT8 *pucData)
{
	HUINT16 usIdx = 0;
	HUINT16 suCredentialLen = 0;
	eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus = eCRES_RESULTCODE_ERROR_OTHER;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstDrmRstMsg || !pstCreHdlMgr)
	{
		HxLOG_Error("pstDrmRstMsg or pstCreHdlMgr is NULL\n");
		return eCRES_RESULTCODE_ERROR_OTHER;
	}
	if(!pucData)
	{
		HxLOG_Error("pucData is NULL\n");
		return eCRES_RESULTCODE_ERROR_INVALID_DATA;
	}
	suCredentialLen = HxSTD_StrLen(pucData);
	if(suCredentialLen<=0)
	{
		HxLOG_Error("suCredentialLen < 0\n");
		return eCRES_RESULTCODE_ERROR_INVALID_DATA;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle==INVALID_CREDENTIAL_HANDLE)
		{
			if(pCreInfo->pucData)
			{
				HLIB_STD_MemFree(pCreInfo->pucData);
				pCreInfo->pucData = NULL;
			}

			pCreInfo->pucData = (HUINT8*)HLIB_STD_MemAlloc(suCredentialLen+1);
			HxSTD_memset(pCreInfo->pucData, 0, suCredentialLen+1);

			if(pCreInfo->pucData)
			{
				pstDrmRstMsg->hCredetialHandle = pCreInfo->ulHandle = usIdx+1;
				pCreInfo->usSize = suCredentialLen;
				HxSTD_memcpy(pCreInfo->pucData, pucData, suCredentialLen);
				eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;
				break;
			}
			else
			{
				HxLOG_Error("eCRES_RESULTCODE_ERROR_MALLOC\n");
				eResultCodeStatus = eCRES_RESULTCODE_ERROR_MALLOC;
				break;
			}
		}
	}
	// [추후 주석 제거] 더이상 Credential Handle을 저장할수 없는겨우.
	if(usIdx>=MAX_CREDENTIAL_NUMBER)
	{
		eResultCodeStatus = eCRES_RESULTCODE_ERROR_OVERFLOW;
	}

	return eResultCodeStatus;
}

STATIC eXmgrCas_NaPrmResultCodeStatus_t xmgr_cas_naprm_DeleteCredentialInfoByHandle(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr,HUINT32 ulDeleteHandle)
{
	HUINT16 usIdx = 0;
	eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus = eCRES_RESULTCODE_ERROR_OTHER;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL\n");
		return eCRES_RESULTCODE_ERROR_OTHER;
	}
	if(ulDeleteHandle<=INVALID_CREDENTIAL_HANDLE || ulDeleteHandle>MAX_CREDENTIAL_NUMBER)
	{
		HxLOG_Error("ulDeleteHandle(%d) is error \n", ulDeleteHandle);
		return eCRES_RESULTCODE_ERROR_INVALID_HANDLE;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo= &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle == ulDeleteHandle)
		{
			if(pCreInfo->pucData)
			{
				HLIB_STD_MemFree(pCreInfo->pucData);
				pCreInfo->pucData = NULL;
			}
			pCreInfo->ulHandle = INVALID_CREDENTIAL_HANDLE;
			pCreInfo->usSize = 0;
			eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;
			break;
		}
	}
	// [추후 주석 제거] Delete할 Credential Info가 존재하지 않는경우.
	if(usIdx>=MAX_CREDENTIAL_NUMBER)
	{
		eResultCodeStatus = eCRES_RESULTCODE_ERROR_INVALID_HANDLE;
	}

	return eResultCodeStatus;
}

STATIC eXmgrCas_NaPrmResultCodeStatus_t xmgr_cas_naprm_DeleteAllCredentialInfo(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr)
{
	HUINT32 ulDeleteHandle = 0;
	eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus = eCRES_RESULTCODE_ERROR_OTHER;

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL \n");
		return eCRES_RESULTCODE_ERROR_OTHER;
	}

	for(ulDeleteHandle=1;ulDeleteHandle<=MAX_CREDENTIAL_NUMBER;ulDeleteHandle++)
	{
		eResultCodeStatus = xmgr_cas_naprm_DeleteCredentialInfoByHandle(pstCreHdlMgr, ulDeleteHandle);
	}
	// [추후 주석 제거] DALL인경우 eCRES_HANDLE_ERROR_INVALID_HANDLE를 check하지 않는다. */
	if(eResultCodeStatus==eCRES_RESULTCODE_ERROR_INVALID_HANDLE)
	{
		eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;
	}

	return eResultCodeStatus;
}

STATIC eXmgrCas_NaPrmResultCodeStatus_t _xmgr_cas_naprm_GetCredentialInfoFromCredentialHandle(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle, XmgrCas_NaPrmCredentialInfo_t *pCredentialInfo)
{
	HUINT16 usIdx = 0;
	eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus = eCRES_RESULTCODE_ERROR_OTHER;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr || !pCredentialInfo)
	{
		HxLOG_Error("pstCreHdlMgr or pCredentialInfo is NULL\n");
		return eCRES_RESULTCODE_ERROR_OTHER;
	}

	if(ulHandle<=INVALID_CREDENTIAL_HANDLE || ulHandle>MAX_CREDENTIAL_NUMBER)
	{
		HxLOG_Error("ulHandle(%d) is error\n", ulHandle);
		return eCRES_RESULTCODE_ERROR_INVALID_HANDLE;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle== ulHandle)
		{
			if(pCreInfo->pucData && pCreInfo->usSize > 0)
			{
				HxSTD_memcpy(pCredentialInfo, pCreInfo, sizeof(XmgrCas_NaPrmCredentialInfo_t));
				eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;
			}
			else
			{
				eResultCodeStatus = eCRES_RESULTCODE_ERROR_INVALID_HANDLE;
			}
			break;
		}
	}

	if(usIdx>=MAX_CREDENTIAL_NUMBER)
	{
		eResultCodeStatus = eCRES_RESULTCODE_ERROR_INVALID_HANDLE;
	}

	return eResultCodeStatus;
}

STATIC eXmgrCas_NaPrmResultCodeStatus_t _xmgr_cas_naprm_GetCredentialInfoFromPlaybackHandle(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle, XmgrCas_NaPrmCredentialInfo_t *pCredentialInfo)
{
	HUINT16 usIdx = 0;
	eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus = eCRES_RESULTCODE_ERROR_OTHER;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr || !pCredentialInfo)
	{
		HxLOG_Error("pstCreHdlMgr or pCredentialInfo is NULL\n");
		return eCRES_RESULTCODE_ERROR_OTHER;
	}

	if(ulHandle<=INVALID_PLAYBACK_HANDLE)
	{
		HxLOG_Error("ulHandle is error\n");
		return eCRES_RESULTCODE_ERROR_INVALID_HANDLE;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulPlayBackHandle == ulHandle)
		{
			if(pCreInfo->pucData && pCreInfo->usSize > 0)
			{
				HxSTD_memcpy(pCredentialInfo, pCreInfo, sizeof(XmgrCas_NaPrmCredentialInfo_t));
				eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;
			}
			else
			{
				eResultCodeStatus = eCRES_RESULTCODE_ERROR_INVALID_HANDLE;
			}
			break;
		}
	}

	if(usIdx>=MAX_CREDENTIAL_NUMBER)
	{
		eResultCodeStatus = eCRES_RESULTCODE_ERROR_INVALID_HANDLE;
	}

	return eResultCodeStatus;
}

STATIC void _xmgr_cas_naprm_SetPlaybackHandleByCredentialHandle(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle, HUINT32 ulPlaybackHandle)
{
	HUINT16 usIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL \n");
		return;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo= &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle == ulHandle)
		{
			pCreInfo->ulPlayBackHandle = ulPlaybackHandle;
			break;
		}
	}
}

STATIC HUINT8 xmgr_cas_naprm_IsActiveCredentialHandle(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle)
{
	HUINT16 usIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo= &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle == ulHandle)
		{
			if(pCreInfo->ulPlayBackHandle > INVALID_PLAYBACK_HANDLE)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

STATIC HUINT8 _xmgr_cas_naprm_IsExistActiveCredential(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 *ulPlayBackHandle)
{
	HUINT16 usIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo= &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulPlayBackHandle > INVALID_PLAYBACK_HANDLE)
		{
			if(ulPlayBackHandle)
			{
				*ulPlayBackHandle = pCreInfo->ulPlayBackHandle;
			}
			return TRUE;
		}
	}
	return FALSE;
}

STATIC void _xmgr_cas_naprm_SetMsgIDByCredentialHandle(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle, HUINT32 ulMsgID)
{
	HUINT16 usIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL \n");
		return;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo= &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle == ulHandle)
		{
			pCreInfo->ulMsgID = ulMsgID;
		}
	}

}


#if defined SUPPORT_REPLACE_CREDENTIAL
STATIC void _xmgr_cas_naprm_ReplaceCredential(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle, TSize xOutputCredentialsSize, const TUnsignedInt8 *pxOutputCredentials)
{
	HUINT16 usIdx = 0, usSize;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;
	HUINT8 strBuf[DRMAGENT_DRMMESSAGE_MAX_LEN*2];

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL \n");
		return;
	}

	HxLOG_Print("ulHandle:[0x%x], tCredentialsSize:[%d], pxCredentials:[%p]\n", ulHandle, xOutputCredentialsSize, pxOutputCredentials);

#if defined(CONFIG_DEBUG)
	//HLIB_LOG_Dump((HUINT8 *)pxOutputCredentials, xOutputCredentialsSize, 0, FALSE);
#endif

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo= &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle == ulHandle)
		{
			if(pCreInfo->pucData)
			{
				HLIB_STD_MemFree(pCreInfo->pucData);
				pCreInfo->pucData = NULL;
			}

			HxSTD_memset(strBuf, 0, sizeof(strBuf));
			_xmgr_cas_naprm_ConvertHexDecimalStr((HUINT8 *)pxOutputCredentials, strBuf, xOutputCredentialsSize);
			usSize = HxSTD_StrLen(strBuf);

			pCreInfo->pucData = (HUINT8*)HLIB_STD_MemAlloc(usSize + 1);

			if(pCreInfo->pucData)
			{
				HxSTD_memset(pCreInfo->pucData, 0, usSize + 1);
				HxSTD_memcpy(pCreInfo->pucData, strBuf, usSize);
				pCreInfo->usSize = usSize;
			}
			else
			{
				HxLOG_Critical("HLIB_STD_MemAlloc\n");
				break;
			}
			break;
		}
	}
}
#endif

#if defined (SUPPORT_TIME_SETTING) || defined (CONFIG_DEBUG)
HERROR _xmgr_ca_naprm_SetTime(HUINT16 year, HUINT8 month, HUINT8 day, HUINT8 hour, HUINT8 minute, HUINT8 second)
{
	HxDATETIME_t	stDateTime;
	HUINT32			ulUnixTime;

	stDateTime.stDate.usYear = year;
	stDateTime.stDate.ucMonth = month;
	stDateTime.stDate.ucDay = day;
	stDateTime.stTime.ucHour = hour;
	stDateTime.stTime.ucMinute = minute;
	stDateTime.stTime.ucSecond = second;
	stDateTime.stTime.usMillisecond = 0;

	if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &ulUnixTime) == ERR_OK)
	{
		HxLOG_Print("[Set time (%d.%d.%d, %d:%d:%d)]\n", year, month, day, hour, minute, second);
		if(TAPI_SetTime(TxSOURCE_SET_BY_USERINPUT, ulUnixTime) == ERR_OK)
			return ERR_OK;
	}

	return ERR_FAIL;
}
#endif

#if defined (SUPPORT_TIME_SETTING)
HERROR _xmgr_cas_naprm_SetTimeFromString(HUINT8 *szTime)
{
	HUINT8 ucTemp[10], *p;
	HUINT16 year;
	HUINT8 month, day, hour, minute, second;

	if(szTime == NULL)
	{
		HxLOG_Error("szTime is NULL\n");
		return ERR_FAIL;
	}
	p = szTime;

	HLIB_STD_StrNCpySafe(ucTemp, p, 5);
	year = (HUINT16)HLIB_STD_StrToUINT32(ucTemp);
	p += 4 + 1/* white space */;

	HLIB_STD_StrNCpySafe(ucTemp, p, 3);
	month = (HUINT8)HLIB_STD_StrToUINT32(ucTemp);
	p += 2 + 1/* white space */;

	HLIB_STD_StrNCpySafe(ucTemp, p, 3);
	day = (HUINT8)HLIB_STD_StrToUINT32(ucTemp);
	p += 2 + 1/* white space */;

	HLIB_STD_StrNCpySafe(ucTemp, p, 3);
	hour = (HUINT8)HLIB_STD_StrToUINT32(ucTemp);
	p += 2 + 1/* white space */;

	HLIB_STD_StrNCpySafe(ucTemp, p, 3);
	minute = (HUINT8)HLIB_STD_StrToUINT32(ucTemp);
	p += 2 + 1/* white space */;

	HLIB_STD_StrNCpySafe(ucTemp, p, 3);
	second = (HUINT8)HLIB_STD_StrToUINT32(ucTemp);

	if(_xmgr_ca_naprm_SetTime(year, month, day, hour, minute, second) != ERR_OK)
	{
		HxLOG_Print("[_xmgr_ca_naprm_SetTime] Error : (%d.%d.%d, %d:%d:%d)]\n", year, month, day, hour, minute, second);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

/******************************************************************************************
*
*	Make resultDRMMessage
*
******************************************************************************************/
STATIC HERROR _xmgr_cas_naprm_MakeFirstResultCode(XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg, XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg, XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr)
{
	eXmgrCas_NaPrmSendMsgType_t eMsgType = eMSGTYPE_MAX;
	eXmgrCas_NaPrmSendMsg_t eMsg = eMSG_MAX;
	eXmgrCas_NaPrmSendDRMSystemID_t eDRMSystemID = eSYSTEMID_MAX;
	eXmgrCas_NaPrmResultCode_t eResultCode = eMSG_RESULTCODE_0;

	if(pstDrmSendMsg == NULL || pstDrmRstMsg == NULL || pstCreHdlMgr == NULL)
	{
		HxLOG_Error("pstDrmSendMsg or pstDrmRstMsg or pstCreHdlMgr is NULL\n");
		return ERR_FAIL;
	}

	eMsgType = pstDrmSendMsg->eMsgType;
	eMsg = pstDrmSendMsg->eMsg;
	eDRMSystemID = pstDrmSendMsg->eDRMSystemID;

	if(eMsgType==eMSGTYPE_CSPG_HEXBIN)
	{
		if(eDRMSystemID != eSYSTEMID_6304)
		{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			eResultCode = eMSG_RESULTCODE_3;
#else
			eResultCode = eMSG_RESULTCODE_5;
#endif
		}
		else
		{
			if(eMsg != eMSG_PRMS_SYSINFO 	&& eMsg != eMSG_PRMS_CHINFO 	&&
			   eMsg != eMSG_CRES_CCRE 		&& eMsg != eMSG_PRMS_PSTART 	&&
			   eMsg != eMSG_PRMS_PSTOP 		&& eMsg != eMSG_PRMS_PSTOPALL 	&&
			   eMsg != eMSG_CRES_DCRE 		&& eMsg != eMSG_CRES_DALL 		&&
			   eMsg != eMSG_CRES_INFO 		&& eMsg != eMSG_CRES_UGMD 		&&
			   eMsg != eMSG_CRES_USMD 		&& eMsg	!= eMSG_CRES_RCRE		)
			{
				eResultCode = eMSG_RESULTCODE_2;
			}
		}
	}
	else if(eMsgType==eMSGTYPE_HDPLUSCA_SC)
	{
		if(eDRMSystemID != eSYSTEMID_6211)
		{
			eResultCode = eMSG_RESULTCODE_3;
		}
		else
		{
			if(eMsg != eMSG_SC_INSERTED && eMsg != eMSG_SC_PRODUCTVALIDATION)
			{
				eResultCode = eMSG_RESULTCODE_2;
			}
		}
	}
	else
	{
		eResultCode = eMSG_RESULTCODE_3;
	}

	// TODO : 광고인 경우에도 생각해야함.
	#if 0//def SUPPORT_IGNORE_ADVERTISEMENT
		if(eMsg == eMSG_PRMS_PSTART)
		{
			if(pstDrmSendMsg->ulTransportSessionId==0)
			{
				_xmgr_cas_naprm_HandleMangerInit(pstCreHdlMgr); // 여기서 handle init하면 추후 multi credential data인경우 광고 시점에서 credential data를 모두 지우므로 문제 발생. 막음.
				eResultCode = eMSG_RESULTCODE_1;
			}
		}
	#endif

	pstDrmRstMsg->eResultCode = eResultCode;

	if(pstDrmRstMsg->eResultCode != eMSG_RESULTCODE_0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC eXmgrCas_NaPrmResultCode_t _xmgr_cas_naprm_MakeSecondResultCode(eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus)
{
	eXmgrCas_NaPrmResultCode_t eResultCode = eMSG_RESULTCODE_0;

	switch(eResultCodeStatus)
	{
		case eCRES_RESULTCODE_SUCCESS:						/* CCRE or DCRE : success */
			eResultCode = eMSG_RESULTCODE_0;
			break;
		case eCRES_RESULTCODE_ERROR_OVERFLOW:				/* handle number > MAX_CREDENTIAL_NUMBER */
		case eCRES_RESULTCODE_ERROR_INVALID_HANDLE:
		case eCRES_RESULTCODE_ERROR_INVALID_DATA:			/* CCRE or DCRE : credential data is not valid */
		case eCRES_RESULTCODE_ERROR_ACTIVE_CREDENTIAL:		/* DCRE : active credential */
		case eCRES_RESULTCODE_ERROR_PSTOP_WITHOUT_PSTART:
			eResultCode = eMSG_RESULTCODE_2;
			break;
		case eCRES_RESULTCODE_ERROR_MALLOC:					/* CCRE : memory malloc error */
		case eCRES_RESULTCODE_ERROR_FREE:					/* DCRE : dredential data : free error */
		case eCRES_RESULTCODE_ERROR_OTHER: 					/* CCRE or DCRE : other error */
			eResultCode = eMSG_RESULTCODE_1;
			break;
		default:
			eResultCode = eMSG_RESULTCODE_1;
			break;
	}

	return eResultCode;
}

STATIC void _xmgr_cas_naprm_MakeDrmResultMessageOfField(HUINT8 *pucMsg, void *pvValue, HBOOL bNumeric, HBOOL bLastField, eXmgrCas_NaPrmResultFieldItem_t eField)
{
	HUINT8	szTempBuffer[20];
	HUINT32	ulTempStrLen = 0;
	HUINT32	*ulTempValue = NULL;

	if(pucMsg==NULL || pvValue==NULL)
	{
		HxLOG_Error("pucMsg or pvValue is NULL \n");
		return;
	}

	HxSTD_memset(szTempBuffer, 0x00, 20*sizeof(HUINT8));
	HxSTD_StrCat(pucMsg, stDrmResultMsgField[eField].ucStr);

	if((eField != eFIELD_ERROR) && (eField != eFIELD_CAID))
	{
		HxSTD_StrNCat(pucMsg, MESSAGE_VALUE_IDENTIFIER, MESSAGE_VALUE_IDENTIFIER_SIZE);
	}

	if(bNumeric == TRUE)
	{
		ulTempValue = (HUINT32*)(pvValue);
		HxSTD_snprintf(szTempBuffer, 20, "%X", *ulTempValue);
		ulTempStrLen = HxSTD_StrLen(szTempBuffer);
		HxSTD_StrNCat(pucMsg, szTempBuffer, ulTempStrLen);
	}
	else
	{
		HxSTD_StrCat(pucMsg, (HUINT8*)pvValue);
	}

	if(bLastField != TRUE)
	{
		HxSTD_StrNCat(pucMsg, MESSAGE_FIELD_IDENTIFIER, MESSAGE_FIELD_IDENTIFIER_SIZE);
	}

	return;
}

/* eMSG_RESULTCODE_0인경우 or Msg가 null인경우 강제로 Empty String을 만든다. */
STATIC void _xmgr_cas_naprm_MakeResultEmptyMessage(XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg)
{
	if(pstDrmRstMsg == NULL)
	{
		HxLOG_Error("pstDrmRstMsg is NULL\n");
		return;
	}

	if(pstDrmRstMsg->pucMsg)
	{
		HLIB_STD_MemFree(pstDrmRstMsg->pucMsg);
		pstDrmRstMsg->pucMsg = NULL;
		pstDrmRstMsg->ulMsgSize = 0;
	}

	pstDrmRstMsg->pucMsg = (HUINT8*)HLIB_STD_MemAlloc(2);

	if(pstDrmRstMsg->pucMsg)
	{
		pstDrmRstMsg->ulMsgSize = 2;
		HxSTD_memset(pstDrmRstMsg->pucMsg, 0, 2);
		//HxSTD_PrintToStr(pstDrmRstMsg->pucMsg, "%s", ""); // empty string
	}
	else
	{
		HxLOG_Error("pucMsg is NULL\n");
	}
}

STATIC void _xmgr_cas_naprm_MakeDrmResultMessage(XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg, HUINT8 *pucMsg)
{
	HUINT32	ulStringLen = 0;

	if(pstDrmRstMsg == NULL || pucMsg == NULL)
	{
		HxLOG_Error("pstDrmRstMsg or pucMsg is NULL \n");
		return;
	}

	ulStringLen = HxSTD_StrLen(pucMsg);

	pstDrmRstMsg->pucMsg = (HUINT8*)HLIB_STD_MemAlloc(ulStringLen * sizeof(HUINT8)+1);

	if(pstDrmRstMsg->pucMsg != NULL)
	{
		pstDrmRstMsg->ulMsgSize = ulStringLen * sizeof(HUINT8)+1;
		HxSTD_memset(pstDrmRstMsg->pucMsg, 0x00, ulStringLen * sizeof(HUINT8)+1);
		HxSTD_StrNCpy(pstDrmRstMsg->pucMsg, pucMsg, ulStringLen);
	}
	else
	{
		HxLOG_Error("pucMsg is NULL \n");
	}

	return;
}

/******************************************************************************************
*
*	Communicatie with dvl lib
*
******************************************************************************************/
STATIC void _xmgr_cas_naprm_SetPlaybackKeyInformation(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle, HUINT32 ulTransportSessionId, HUINT32 hPlayBackHandle)
{
	HUINT16 usIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL \n");
		return;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo= &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulHandle == ulHandle)
		{
			pCreInfo->stMgrMedia_DrmInformation.ulTransportSessionId = ulTransportSessionId;
			pCreInfo->stMgrMedia_DrmInformation.hPlayBackHandle = hPlayBackHandle;
			break;
		}
	}
}

STATIC HERROR _xmgr_cas_naprm_SendClearDrmInformation(void)
{
	MgrMedia_DrmInformation_t stDrmInfo;

	HxSTD_memset(&stDrmInfo, 0, sizeof(MgrMedia_DrmInformation_t));
	(void)BUS_PostData(NULL, eMEVT_MEDIA_SEND_DRM_INFORMATION, HANDLE_NULL, 0, 0, (void*) &stDrmInfo, sizeof(MgrMedia_DrmInformation_t));

	return ERR_OK;
}

STATIC BUS_Result_t _xmgr_cas_naprm_SendDrmInformation(XmgrCas_NaPrmContext_t *pstContext, NaPrmKeyInfo_t *pstNaPrmKeyInfo)
{

	XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr = NULL;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;
	HUINT16 usIdx = 0;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE) && !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	HUINT32	i;
#endif

	if(pstContext == NULL || pstNaPrmKeyInfo == NULL)
	{
		HxLOG_Error("pstContext or pstNaPrmKeyInfo is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}

	pstCreHdlMgr = pstContext->pstDrmCredentialHandleManager;

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL \n");
		return ERR_BUS_SOURCE_NULL;
	}

	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo= &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1) && !defined(__FORCED_USE_PRM_2_6__)
		if(pCreInfo->stMgrMedia_DrmInformation.hPlayBackHandle == (HUINT32)pstNaPrmKeyInfo->ulPlaybackHandle)
#else
		if(pCreInfo->stMgrMedia_DrmInformation.ulTransportSessionId == (HUINT32)pstNaPrmKeyInfo->xTSid)
#endif
		{
			pCreInfo->stMgrMedia_DrmInformation.eDRMType = MGR_MEDIA_DRMTYPE_NAGRAPRM;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			pCreInfo->stMgrMedia_DrmInformation.eEncryptionType = eMediaEncryptType_AES128;//eMediaEncryptType_NONE; // eMediaEncryptType_NONE으로 해야지 정상 play됨.
#else
			pCreInfo->stMgrMedia_DrmInformation.eEncryptionType = eMediaEncryptType_AES128;
#endif
			pCreInfo->stMgrMedia_DrmInformation.eStreamType = eMEDIA_Type_MPEG_TS;
			pCreInfo->stMgrMedia_DrmInformation.usEmi = pstNaPrmKeyInfo->usEmi;

			HxLOG_Print("============== Send To Imma ==================\n");
			HxLOG_Print("+\t usEmi = [0x%x]\n", pstNaPrmKeyInfo->usEmi);
			HxLOG_Print("+\t xTSid = [%d]\n", pstNaPrmKeyInfo->xTSid);
			HxLOG_Print("+\t ulPlaybackHandle = [%d]\n", pstNaPrmKeyInfo->ulPlaybackHandle);
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
			pCreInfo->stMgrMedia_DrmInformation.ulKeyHandle = pstNaPrmKeyInfo->ulKeyHandler;
			HxLOG_Print("+\t ulKeyHandler = [0x%x]\n", pstNaPrmKeyInfo->ulKeyHandler);
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
			pCreInfo->stMgrMedia_DrmInformation.ulKeyIdSize = pstNaPrmKeyInfo->ulKeyIdSize;
			if(pCreInfo->stMgrMedia_DrmInformation.ulKeyIdSize != 0)
			{
				for(i = 0; i < pCreInfo->stMgrMedia_DrmInformation.ulKeyIdSize; i ++)
				{
					pCreInfo->stMgrMedia_DrmInformation.aucKeyId[i] = pstNaPrmKeyInfo->pucKeyId[i];
				}
			}
			HxLOG_Print("+\t ulKeyIdSize = [%d]\n", pstNaPrmKeyInfo->ulKeyIdSize);
			HxLOG_Print("+\t aucKeyId = [%p]\n", pstNaPrmKeyInfo->pucKeyId);
#endif
#else
			pCreInfo->stMgrMedia_DrmInformation.ulKeySize = (pstNaPrmKeyInfo->ulKeyLength < MGR_MEDIA_DRMINFO_KEYSIZE)? pstNaPrmKeyInfo->ulKeyLength : MGR_MEDIA_DRMINFO_KEYSIZE;
			HxLOG_Print("+\t ulKeyIdSize = [%d]\n", pCreInfo->stMgrMedia_DrmInformation.ulKeySize);
			if(pCreInfo->stMgrMedia_DrmInformation.ulKeySize != 0)
			{
				HxSTD_memcpy(pCreInfo->stMgrMedia_DrmInformation.ucEncryptKey, pstNaPrmKeyInfo->ucKey, pCreInfo->stMgrMedia_DrmInformation.ulKeySize);
				HLIB_LOG_Dump(pCreInfo->stMgrMedia_DrmInformation.ucEncryptKey, pCreInfo->stMgrMedia_DrmInformation.ulKeySize, 0, FALSE);
			}

#endif
			break;
		}
	}

	if(usIdx >= MAX_CREDENTIAL_NUMBER)
	{
		HxLOG_Error("hPlayBackHandle is not exist \n");
		return ERR_BUS_NO_ERROR;
	}

#ifdef CONFIG_DEBUG
	_xmgr_cas_naprm_PrintDrmInformation(&pCreInfo->stMgrMedia_DrmInformation);
#endif

	(void)BUS_PostData(NULL, eMEVT_MEDIA_SEND_DRM_INFORMATION, HANDLE_NULL, 0, 0, (void*) &pCreInfo->stMgrMedia_DrmInformation, sizeof(MgrMedia_DrmInformation_t));

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR _xmgr_cas_naprm_ProcessCallbackEvent(HUINT32 xPlaybackHandle, TDvlAccessStatus xPlaybackStatus)
{
	XmgrCas_NaPrmContext_t	*pstContext = _xmgr_cas_naprm_GetContext();
	XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr = NULL;
	XmgrCas_NaPrmDRMResultMessage_t			*pstDrmRstMsg = NULL;
	XmgrCas_NaPrmCredentialInfo_t stCredentialInfo;
	//MgrDrmAgentSendDrmMessage_t stDrmMessage;
	HUINT8 szResultTempBuffer[DRMAGENT_DRMMESSAGE_MAX_LEN];
	HUINT8 ucDecimalStr[MAX_TEMP_BUF_LENGTH];
	HUINT8 ucSMD[DRMAGENT_DRMMESSAGE_MAX_LEN];

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return ERR_FAIL;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("\n\n\n\n"HxANSI_COLOR_PURPLE("############################################################################""\n"));
	HxLOG_Print(HxANSI_COLOR_PURPLE("[%s][%d] pxPlaybackHandle:[%d], xPlaybackStatus[%d]\n"), __FUNCTION__, __LINE__, (HUINT32)xPlaybackHandle, xPlaybackStatus);
	HxLOG_Print("\t"HxANSI_COLOR_PURPLE("xPlaybackStatus : [%s]""\n"), _xmgr_cas_naprm_GetDvlStatusName(xPlaybackStatus));
	HxLOG_Print(HxANSI_COLOR_PURPLE("############################################################################""\n\n\n\n"));
#endif

	pstCreHdlMgr = pstContext->pstDrmCredentialHandleManager;
	pstDrmRstMsg = pstContext->pstDrmResultMessage;

	if(pstCreHdlMgr == NULL || pstDrmRstMsg == NULL)
	{
		HxLOG_Error("pstContext or pstDrmRstMsg is NULL \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&szResultTempBuffer, 0x00, sizeof(szResultTempBuffer));
	HxSTD_memset(&ucSMD, 0, sizeof(ucSMD));
	HxSTD_memset(&stCredentialInfo, 0, sizeof(XmgrCas_NaPrmCredentialInfo_t));

	_xmgr_cas_naprm_ResultMsgInit(pstDrmRstMsg);

	if(_xmgr_cas_naprm_GetCredentialInfoFromPlaybackHandle(pstCreHdlMgr, xPlaybackHandle, &stCredentialInfo) != eCRES_RESULTCODE_SUCCESS)
	{
		HxLOG_Error(" invalid credential handle\n");
	}
	else
	{
#if 0 // DVL Library에서 호출 되는 Callback안에서 다시 DVL function을 부르면 call sequence가 꼬여 문제가 발생한다. Web Application으로 Status를 보내고 Web application에 의해서 Stop등의 요청을 받아 처리하는 것이 맞아 보인다.
		if(_xmgr_cas_naprm_StopPlaybackByPlaybackHandle(pstCreHdlMgr, (HINT32)pxPlaybackHandle)==ERR_FAIL)
		{
			HxLOG_Error(" stop fail\n");
		}
		if(xmgr_cas_naprm_DeleteAllCredentialInfo(pstCreHdlMgr)!=eCRES_RESULTCODE_SUCCESS)
		{
			HxLOG_Error(" credential data is not delete\n");
		}
#endif
	}

	pstDrmRstMsg->ucMsgID = stCredentialInfo.ulMsgID;

	//error code
	pstDrmRstMsg->hErrorCode = 0;
	_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
	_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_ERROR);

	// Playback handle
	_xmgr_cas_naprm_ConvertDecimalStr((HUINT32)xPlaybackHandle, ucDecimalStr);
	_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_PLAYH);

	// playback status
	_xmgr_cas_naprm_ConvertDecimalStr((HUINT32)xPlaybackStatus, ucDecimalStr);
	_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_STAT);

	// result message
	_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);

	pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_0;

#ifdef CONFIG_DEBUG
	_xmgr_cas_naprm_PrintDrmResultMessage(pstDrmRstMsg, pstCreHdlMgr);
#endif
	(void)BUS_PostDataToOM(NULL, eMEVT_DRMAGENT_COMMON_RESPONSEDRMMESSAGERESULT, HANDLE_NULL, pstDrmRstMsg->ucMsgID, pstDrmRstMsg->eResultCode, pstDrmRstMsg->pucMsg, pstDrmRstMsg->ulMsgSize);

	return ERR_OK;
}

STATIC void _xmgr_cas_naprm_PlayBackErrorCallback(TDvlHandle *pxPlaybackHandle, TDvlAccessStatus  xPlaybackStatus)
{

#ifdef CONFIG_DEBUG
	HxLOG_Print ("\n\n\n\n"HxANSI_COLOR_PURPLE("############################################################################""\n"));
	HxLOG_Print (HxANSI_COLOR_PURPLE("[%s][%d] pxPlaybackHandle:[%d], xPlaybackStatus[%d]\n"), __FUNCTION__, __LINE__, (HUINT32)pxPlaybackHandle, xPlaybackStatus);
	HxLOG_Print ("\t"HxANSI_COLOR_PURPLE("xPlaybackStatus : [%s]""\n"), _xmgr_cas_naprm_GetDvlStatusName(xPlaybackStatus));
	HxLOG_Print (HxANSI_COLOR_PURPLE("############################################################################""\n\n\n\n"));

#if 0 //DVL Integration Test 결과 report 작성시 사용하면 편함.
	{
		HCHAR alog[128] = {0,};

		HxSTD_sprintf(alog, "[xPlaybackErrorCB()], xPlaybackStatus: [%d, %s]\n", xPlaybackStatus, _xmgr_cas_naprm_GetDvlStatusName(xPlaybackStatus));
		filelogTest(alog);
	}
#endif

#endif

	BUS_PostMessage(NULL, eMEVT_CAS_UPDATE_STATUS, HANDLE_NULL, (HINT32)pxPlaybackHandle, (HINT32)xPlaybackStatus, 0);
}

STATIC HERROR _xmgr_cas_naprm_StopPlaybackByPlaybackHandle(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT32 ulHandle)
{
	HERROR hErr = ERR_OK;
	HUINT16 usIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr)
	{
		HxLOG_Error("pstCreHdlMgr is NULL\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_2_6) || defined(__FORCED_USE_PRM_2_6__)
	_xmgr_cas_naprm_SendClearDrmInformation();
	hErr = xmgr_cas_NadvlStopPlayback((TDvlHandle* )ulHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("ERROR in xmgr_cas_NadvlStopPlayback(), Err:[%d]\n", hErr);
	}
#elif defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
	hErr = xmgr_cas_NadvlPlaybackCloseSession((TDvlHandle* )ulHandle);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("ERROR in xmgr_cas_NadvlPlaybackCloseSession(), Err:[%d]\n", hErr);
	}
#endif
	for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
	{
		pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;

		if(pCreInfo->ulPlayBackHandle==ulHandle)
		{
			// TO DO
			// Handle Manager에서 Playback에 관련된 부분 초기화 한다.
			pCreInfo->ulMsgID = 0;
			pCreInfo->ulPlayBackHandle = INVALID_PLAYBACK_HANDLE;
#ifdef SUPPORT_CREDENTIAL_GMD
			xmgr_cas_naprm_CloseGenericMetadataByIndex(pstCreHdlMgr, usIdx);
#endif
			xmgr_cas_naprm_CloseUsageRuleFromSpecificMetadataByIndex(pstCreHdlMgr, usIdx);
#if defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
			if(xmgr_cas_RemoveTransportSessionIdDirect(pCreInfo->stMgrMedia_DrmInformation.ulTransportSessionId) != ERR_OK)
			{
				HxLOG_Error("Error in [xmgr_cas_RemoveTransportSessionIdDirect] TSID[%d]\n", pCreInfo->stMgrMedia_DrmInformation.ulTransportSessionId);
			}
#endif
#if defined(SUPPORT_USAGERULE_RECOVERY)
			xmgr_cas_naprm_RecoveryDefaultUsageRule();
#endif
			break;
		}
	}

	return hErr;
}

STATIC eXmgrCas_NaPrmResultCodeStatus_t _xmgr_cas_naprm_GetCredentialInfoFromCredentialIndex(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr, HUINT16 usIdx, XmgrCas_NaPrmCredentialInfo_t *pCredentialInfo)
{
	eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus = eCRES_RESULTCODE_ERROR_OTHER;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(!pstCreHdlMgr || !pCredentialInfo)
	{
		HxLOG_Error("pstCreHdlMgr or pCredentialInfo is NULL\n");
		return eCRES_RESULTCODE_ERROR_OTHER;
	}

	pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIdx];

	if(pCreInfo==NULL)
	{
		HxLOG_Error("pCreInfo is NULL\n");
		return eCRES_RESULTCODE_ERROR_OTHER;
	}

	if(pCreInfo->pucData && pCreInfo->usSize>0)
	{
		HxSTD_memcpy(pCredentialInfo, pCreInfo, sizeof(XmgrCas_NaPrmCredentialInfo_t));
		eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;
	}
	else
	{
		eResultCodeStatus = eCRES_RESULTCODE_ERROR_INVALID_HANDLE;
	}

	return eResultCodeStatus;
}


/******************************************************************************************
*
*	Process sendDRMMessage
*
******************************************************************************************/
STATIC HERROR _xmgr_cas_naprm_ProcessDrmResultMessage(XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg, XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg, XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr)
{
	HERROR hError = ERR_OK;
	eXmgrCas_NaPrmSendMsgType_t eMsgType = pstDrmSendMsg->eMsgType;
	eXmgrCas_NaPrmSendMsg_t eMsg = pstDrmSendMsg->eMsg;
	HUINT8	szResultTempBuffer[DRMAGENT_DRMMESSAGE_MAX_LEN];
	HUINT8 ucDecimalStr[MAX_TEMP_BUF_LENGTH];

	HxSTD_memset(szResultTempBuffer, 0x00, DRMAGENT_DRMMESSAGE_MAX_LEN*sizeof(HUINT8));

	if(eMsgType==eMSGTYPE_CSPG_HEXBIN)
	{
		switch(eMsg)
		{
		case eMSG_PRMS_SYSINFO:
			{
				XmgrCas_NaPrmSystemInfo_t *pLocalSysInfo = NULL;

				if(pstDrmRstMsg->pstSystemInformation == NULL)
				{
					HxLOG_Error("pstSystemInformation is NULL \n");
					pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
					break;
				}

				pLocalSysInfo = pstDrmRstMsg->pstSystemInformation;

				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_ERROR);

				// Version
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, pLocalSysInfo->szVersionStr, FALSE, FALSE, eFIELD_VERSION);

				// UNID String
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, pLocalSysInfo->szNuIdStr, FALSE, FALSE, eFIELD_SNUID);

				// NUID
				_xmgr_cas_naprm_ConvertDecimalStr(pLocalSysInfo->ulNuId, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_NUID);

				// STB Serial Number String
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, pLocalSysInfo->szIrdSerialNumberStr, FALSE, FALSE, eFIELD_SIRDSN);

				// STB Serial Number
				_xmgr_cas_naprm_ConvertDecimalStr(pLocalSysInfo->ulIrdSerialNumber, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_IRDSN);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		case eMSG_PRMS_CHINFO:
			{
				XmgrCas_NaPrmChipsetInfo_t *pLocalChipsetInfo = NULL;

				if(pstDrmRstMsg->pstChipsetInformation == NULL)
				{
					HxLOG_Error("pstChipsetInformation is NULL \n");
					pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
					break;
				}

				pLocalChipsetInfo = pstDrmRstMsg->pstChipsetInformation;

				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_ERROR);

				// Chipset Type
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, pLocalChipsetInfo->szChipsetTypeStr, FALSE, FALSE, eFIELD_CHTYPE);

				// UNID String
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, pLocalChipsetInfo->szNuIdStr, FALSE, FALSE, eFIELD_SNUID);

				// NUID
				_xmgr_cas_naprm_ConvertDecimalStr(pLocalChipsetInfo->ulNuId, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_NUID);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		case eMSG_PRMS_PSTART:
			{
				XmgrCas_NaPrmCredentialInfo_t CredentialInfo;
				eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;
				HUINT8 ucSMD[DRMAGENT_DRMMESSAGE_MAX_LEN];
#ifdef SUPPORT_CREDENTIAL_GMD
				HUINT8 ucGMD[DRMAGENT_DRMMESSAGE_MAX_LEN];
#endif
#if defined(SUPPORT_META_VARIATION)
				HBOOL bMetaData = FALSE;
#endif
				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_ERROR);

				// Playback Handle
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hPlayBackHandle, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_PLAYH);

				// Status
				HxSTD_memset(&CredentialInfo, 0, sizeof(XmgrCas_NaPrmCredentialInfo_t));
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hStat, ucDecimalStr);
#if defined(SUPPORT_META_VARIATION)
				if(CredentialInfo.ulSpecificMetaDataSize > 0
#ifdef SUPPORT_CREDENTIAL_GMD
					|| CredentialInfo.ulGenericMetaDataSize > 0
#endif
				)
				{
					bMetaData = TRUE;
				}

				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, (bMetaData == TRUE)? FALSE : TRUE, eFIELD_STAT);
#else
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_STAT);
#endif
				HxSTD_memset(&CredentialInfo, 0, sizeof(XmgrCas_NaPrmCredentialInfo_t));
				eResultCodeStatus = _xmgr_cas_naprm_GetCredentialInfoFromPlaybackHandle(pstCreHdlMgr, pstDrmRstMsg->hPlayBackHandle, &CredentialInfo);

				if(eResultCodeStatus==eCRES_RESULTCODE_SUCCESS)
				{
#ifdef SUPPORT_CREDENTIAL_GMD
					if(CredentialInfo.ulGenericMetaDataSize > 0)
					{
						// Generic Meta Data
						HxSTD_memset(&ucGMD, 0, sizeof(ucGMD));
						_xmgr_cas_naprm_ConvertHexDecimalStr(CredentialInfo.pucGenericMetaData, ucGMD, CredentialInfo.ulGenericMetaDataSize);
#if defined(SUPPORT_META_VARIATION)
						_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, CredentialInfo.pucGenericMetaData, FALSE, (CredentialInfo.ulSpecificMetaDataSize > 0)? FALSE : TRUE, eFIELD_GMD);
#else
						_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, CredentialInfo.pucGenericMetaData, FALSE, FALSE, eFIELD_GMD);
#endif
					}
					else
					{
#if !defined(SUPPORT_META_VARIATION)
						// Generic Meta Data
						_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, "", FALSE, FALSE, eFIELD_GMD);
#endif
					}
#endif

					if(CredentialInfo.ulSpecificMetaDataSize > 0)
					{
						// Specific Meta Data
						HxSTD_memset(&ucSMD, 0, sizeof(ucSMD));
						_xmgr_cas_naprm_ConvertHexDecimalStr(CredentialInfo.pucSpecificMetaData, ucSMD, CredentialInfo.ulSpecificMetaDataSize);
						_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, ucSMD, FALSE, TRUE, eFIELD_SMD);
					}
					else
					{
#if !defined(SUPPORT_META_VARIATION)
						// Specific Meta Data
						_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, "", FALSE, TRUE, eFIELD_SMD);
#endif
					}
				}
				else
				{
#if !defined(SUPPORT_META_VARIATION)

#ifdef SUPPORT_CREDENTIAL_GMD
					// Generic Meta Data
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, "", FALSE, FALSE, eFIELD_GMD);
#endif

					// Specific Meta Data
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, "", FALSE, TRUE, eFIELD_SMD);

#endif
				}
				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		case eMSG_PRMS_PSTOP:
			{
				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_ERROR);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		case eMSG_PRMS_PSTOPALL:
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_ERROR);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			break;
		case eMSG_CRES_CCRE:
			{
				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_ERROR);

				// Credential Handle
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hCredetialHandle, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)&ucDecimalStr, FALSE, TRUE, eFIELD_CREH);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		case eMSG_CRES_DCRE:
			{
				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_ERROR);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		case eMSG_CRES_INFO:
			{
				XmgrCas_NaPrmResultCredentialInfo_t *pLocalResultCredentialInfo= NULL;
				HUINT8 ucSMD[DRMAGENT_DRMMESSAGE_MAX_LEN];
#ifdef SUPPORT_CREDENTIAL_GMD
				HUINT8 ucGMD[DRMAGENT_DRMMESSAGE_MAX_LEN];
#endif
				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_ERROR);

				if(pstDrmRstMsg->pstResultCredentialInfo == NULL)
				{
					HxLOG_Error("pstResultCredentialInfo is NULL \n");
					pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
					break;
				}

				pLocalResultCredentialInfo = pstDrmRstMsg->pstResultCredentialInfo;
				if(pLocalResultCredentialInfo == NULL)
					break;

				if( pLocalResultCredentialInfo->ucCredentialsType == CREDENTIALS_TYPE_VOD_EMM)
				{
					TVodEmmData 		  *vodEmmData = NULL;

					vodEmmData = &pLocalResultCredentialInfo->stCredentialUnion.vodEmmData;

					// Credential Type
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, "VODEMMDATA", FALSE, FALSE, eFIELD_TYPE);

					//STB Serial Number
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->irdSerialNumber, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_IRDSN);

					// Content ID
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->contentId, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_CID);

					// Vod EMM ID
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->vodEmmId, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_EMMI);

					// Credentials Creation Date
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->credentialsCreationDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_CDA);

					// Content Expiration Date
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->contentExpirationDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_EDA);

					// Content First Visualization Date
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->contentFirstVisualizationDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_FVD);

					// Content Viewing Window Duration
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->contentViewingWindowDuration, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_VWD);

					// Content Viewing Window End Date
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->contentViewingWindowEndDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_VWED);

					// Content Viewing Window Relativity
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->contentViewingWindowRelativity, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_VWR);

					// Content Is Ciphered
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->contentIsCiphered, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_CIPH);

					// Content Key Is Protected
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->contentKeyIsProtected, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_CKPR);

					// NUID
					_xmgr_cas_naprm_ConvertDecimalStr(vodEmmData->nuidValue, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_NUID);
				}
				else if(pLocalResultCredentialInfo->ucCredentialsType == CREDENTIALS_TYPE_VOD_ECM)
				{
					TVodEcmData 		  *vodEcmData = NULL;

					vodEcmData = &pLocalResultCredentialInfo->stCredentialUnion.vodEcmData;

					// Credential Type
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, "VODECMDATA", FALSE, FALSE, eFIELD_TYPE);

					// Content ID
					_xmgr_cas_naprm_ConvertDecimalStr(vodEcmData->contentId, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_CID);

					// Credentials Creation Date
					_xmgr_cas_naprm_ConvertDecimalStr(vodEcmData->contentCreationDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_CDA);

					// Content Expiration Date
					_xmgr_cas_naprm_ConvertDecimalStr(vodEcmData->contentExpirationDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_EDA);

					// Content EMI
					_xmgr_cas_naprm_ConvertDecimalStr(vodEcmData->contentEmi, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_CEMI);

					// Content Key Is Protected
					_xmgr_cas_naprm_ConvertDecimalStr(vodEcmData->contentKeyIsProtected, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE,FALSE, eFIELD_CKPR);

					// TODO : 변수 인자로는 존재, Nagra 문서에는 없음..확인 필요..
//					// Profile ID
//					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, vodEmmData->nuidValue, FALSE, eFIELD_NUID);
				}
				else if(pLocalResultCredentialInfo->ucCredentialsType== CREDENTIALS_TYPE_LCM)
				{
					TLcmData			  *lcmData = NULL;

					lcmData = &pLocalResultCredentialInfo->stCredentialUnion.lcmData;

					// Credential Type
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, "LCMDATA", FALSE, FALSE, eFIELD_TYPE);

					//STB Serial Number
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->irdSerialNumber, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_IRDSN);

					// TODO : 변수 인자로는 존재, Nagra 문서에는 없음..확인 필요..
//					// Profile ID
//					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, lcmData->irdSerialNumber, FALSE, eFIELD_IRDSN);

					// Collection ID
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->collectionId, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_IRDSN);

					// Credentials Creation Date
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->credentialsCreationDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_CDA);

					// Content Expiration Date
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->contentExpirationDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_EDA);

					// Content First Visualization Date
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->contentFirstVisualizationDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_FVD);

					// Content Viewing Window Duration
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->contentViewingWindowDuration, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_VWD);

					// Content Viewing Window End Date
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->contentViewingWindowEndDate, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_VWED);

					// Content Viewing Window Relativity
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->contentViewingWindowRelativity, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_VWR);

					// Content EMI
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->contentEmi, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_CEMI);

					// NUID
					_xmgr_cas_naprm_ConvertDecimalStr(lcmData->nuidValue, ucDecimalStr);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_NUID);

					// Chipset Type
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, lcmData->chipsetType, FALSE, FALSE, eFIELD_CHTYPE);
				}
				else
				{

				}

#ifdef SUPPORT_CREDENTIAL_GMD
				if(pLocalResultCredentialInfo->pucGenericMetadata && pLocalResultCredentialInfo->ulGenericMetadataSize > 0)
				{
					HxSTD_memset(&ucGMD, 0, sizeof(ucGMD));
					_xmgr_cas_naprm_ConvertHexDecimalStr(pLocalResultCredentialInfo->pucGenericMetadata, ucGMD, pLocalResultCredentialInfo->ulGenericMetadataSize);
					// GMD
//					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, pLocalResultCredentialInfo->pucGenericMetadata, FALSE, TRUE, eFIELD_GMD);
#if !defined(SUPPORT_META_VARIATION)
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, ucGMD, FALSE,(pLocalResultCredentialInfo->ulSpecificMetadataSize > 0)? FALSE : TRUE, eFIELD_GMD);
#else
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, ucGMD, FALSE, FALSE, eFIELD_GMD);
#endif
				}
				else
				{
#if !defined(SUPPORT_META_VARIATION)
					// GMD
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, "", FALSE, FALSE, eFIELD_GMD);
#endif
				}
#endif

				if(pLocalResultCredentialInfo->pucSpecificMetadata && pLocalResultCredentialInfo->ulSpecificMetadataSize > 0)
				{
					HxSTD_memset(&ucSMD, 0, sizeof(ucSMD));
					_xmgr_cas_naprm_ConvertHexDecimalStr(pLocalResultCredentialInfo->pucSpecificMetadata, ucSMD, pLocalResultCredentialInfo->ulSpecificMetadataSize);
					// SMD
//					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, pLocalResultCredentialInfo->pucSpecificMetadata, FALSE, TRUE, eFIELD_SMD);
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, ucSMD, FALSE, TRUE, eFIELD_SMD);
				}
				else
				{
#if !defined(SUPPORT_META_VARIATION)
					// SMD
					_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, "", FALSE, TRUE, eFIELD_SMD);
#endif
				}

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		case eMSG_CRES_DALL:
				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_ERROR);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			break;
		case eMSG_CRES_USMD:
			{
				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_ERROR);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		case eMSG_CRES_UGMD:
			{
				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_ERROR);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		case eMSG_CRES_RCRE:
			{
				// Error Code
				_xmgr_cas_naprm_ConvertDecimalStr(pstDrmRstMsg->hErrorCode, ucDecimalStr);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, FALSE, eFIELD_ERROR);

				// DATA
				//_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, pLocalChipsetInfo->szChipsetTypeStr, TRUE, eFIELD_DATA);

				// result message
				_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
			}
			break;
		default:
			// ??
			break;

		}
	}
	else if(eMsgType==eMSGTYPE_HDPLUSCA_SC)
	{
		if(eMsg == eMSG_SC_INSERTED)
		{
			HBOOL	bHDPlusSC = FALSE;

			bHDPlusSC = (HBOOL)pstDrmRstMsg->hErrorCode;
			if(bHDPlusSC == TRUE)
			{
				// CA System ID
				HxSTD_memset(ucDecimalStr, 0, MAX_TEMP_BUF_LENGTH);
				HxSTD_snprintf(ucDecimalStr, sizeof(ucDecimalStr), "0x%x", pstDrmRstMsg->usEcmCasSystemID);
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)ucDecimalStr, FALSE, TRUE, eFIELD_CAID);
			}
			else
			{
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)MESSAGE_VALUE_FALSE, FALSE, TRUE, eFIELD_ERROR);
			}

			_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
		}
		else if(eMsg == eMSG_SC_PRODUCTVALIDATION)
		{
			HBOOL	bHDPlusSC = FALSE;
			// Error Code
			bHDPlusSC = (HBOOL)pstDrmRstMsg->hErrorCode;
			if(bHDPlusSC == TRUE)
			{
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)MESSAGE_VALUE_TRUE, FALSE, TRUE, eFIELD_ERROR);
			}
			else
			{
				_xmgr_cas_naprm_MakeDrmResultMessageOfField(szResultTempBuffer, (void*)MESSAGE_VALUE_FALSE, FALSE, TRUE, eFIELD_ERROR);
			}

			_xmgr_cas_naprm_MakeDrmResultMessage(pstDrmRstMsg, szResultTempBuffer);
		}
	}

	return hError;
}

STATIC void _xmgr_cas_naprm_ProcessCredentialHandleManager(XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg, XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg, XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr)
{
	eXmgrCas_NaPrmSendMsgType_t eMsgType = eMSGTYPE_MAX;
	eXmgrCas_NaPrmSendMsg_t eMsg = eMSG_MAX;
	eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;

	if(pstDrmSendMsg == NULL || pstDrmRstMsg == NULL || pstCreHdlMgr == NULL)
	{
		HxLOG_Error("pstDrmSendMsg or pstDrmRstMsg or pstCreHdlMgr is NULL\n");
		if (pstDrmRstMsg)
		{
			pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
		}
		return;
	}

	eMsgType = pstDrmSendMsg->eMsgType;
	eMsg = pstDrmSendMsg->eMsg;

	if(eMsgType==eMSGTYPE_CSPG_HEXBIN)
	{
		if(eMsg == eMSG_CRES_CCRE || eMsg == eMSG_CRES_DCRE || eMsg == eMSG_CRES_DALL || eMsg == eMSG_CRES_USMD || eMsg == eMSG_CRES_UGMD || eMsg == eMSG_CRES_RCRE)
		{
			switch(eMsg)
			{
				case eMSG_CRES_CCRE:
					eResultCodeStatus = xmgr_cas_naprm_CreateCredentialInfo(pstDrmRstMsg, pstCreHdlMgr, pstDrmSendMsg->pucCredentialData);
					break;
				case eMSG_CRES_DCRE:
					if(!xmgr_cas_naprm_IsActiveCredentialHandle(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle))
					{
						eResultCodeStatus = xmgr_cas_naprm_DeleteCredentialInfoByHandle(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle);
					}
					else
					{
						eResultCodeStatus = eCRES_RESULTCODE_ERROR_ACTIVE_CREDENTIAL;
					}
					break;
				case eMSG_CRES_DALL:
					_xmgr_cas_naprm_HandleMangerInit(pstCreHdlMgr);
					break;
				case eMSG_CRES_USMD:
					// not use
					break;
				case eMSG_CRES_UGMD:
					// not use
					break;
				case eMSG_CRES_RCRE:
					// not use
					break;
				default:
					break;
			}

			pstDrmRstMsg->eResultCode = _xmgr_cas_naprm_MakeSecondResultCode(eResultCodeStatus);
		}
	}

	return;
}

STATIC void _xmgr_cas_naprm_ProcessCommunicationWithDvlLib(XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg, XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg, XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr)
{
	eXmgrCas_NaPrmSendMsgType_t eMsgType = eMSGTYPE_MAX;
	eXmgrCas_NaPrmSendMsg_t eMsg = eMSG_MAX;
	eXmgrCas_NaPrmResultCodeStatus_t eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;
	HUINT32 hError = ERR_OK;

	if(pstDrmSendMsg == NULL || pstDrmRstMsg == NULL)
	{
		HxLOG_Error("pstDrmSendMsg or pstDrmRstMsg is NULL\n");
		if (pstDrmRstMsg)
		{
			pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
		}
		return;
	}

	eMsgType = pstDrmSendMsg->eMsgType;
	eMsg = pstDrmSendMsg->eMsg;

	if(eMsgType==eMSGTYPE_CSPG_HEXBIN)
	{
		if(eMsg == eMSG_PRMS_SYSINFO || eMsg == eMSG_PRMS_CHINFO || eMsg == eMSG_PRMS_PSTART || eMsg == eMSG_PRMS_PSTOP || eMsg == eMSG_PRMS_PSTOPALL || eMsg == eMSG_CRES_INFO)
		{
			switch(eMsg)
			{
				case eMSG_PRMS_SYSINFO:
					{
						XmgrCas_NaPrmSystemInfo_t *pLocalSysInfo = NULL;

						TDvlVersionString	xVersionString;
						TUnsignedInt32		xIrdSerialNumberValue;
						TDvlUniqueIdString	xIrdSerialNumberString;
						TUnsignedInt32		xNuidValue;
						TDvlUniqueIdString	xNuidString;

						hError = xmgr_cas_NadvlSystemInformationRequest(xVersionString,&xIrdSerialNumberValue,xIrdSerialNumberString,&xNuidValue,xNuidString);

						if(hError == ERR_OK)
						{
							pstDrmRstMsg->pstSystemInformation = (XmgrCas_NaPrmSystemInfo_t*)HLIB_STD_MemAlloc(sizeof(XmgrCas_NaPrmSystemInfo_t));
							if(pstDrmRstMsg->pstSystemInformation != NULL)
							{
								HxSTD_memset(pstDrmRstMsg->pstSystemInformation, 0x00, sizeof(XmgrCas_NaPrmSystemInfo_t));
								pLocalSysInfo = pstDrmRstMsg->pstSystemInformation;
								HxSTD_memcpy(pLocalSysInfo->szVersionStr, xVersionString, VERSION_STRING_LENGTH);
								pLocalSysInfo->ulIrdSerialNumber = (HUINT32) xIrdSerialNumberValue;
								HxSTD_memcpy(pLocalSysInfo->szIrdSerialNumberStr, xIrdSerialNumberString, UNIQUE_ID_STRING_LENGTH);
								pLocalSysInfo->ulNuId = (HUINT32) xNuidValue;
								HxSTD_memcpy(pLocalSysInfo->szNuIdStr, xNuidString, UNIQUE_ID_STRING_LENGTH);
							}
							else
							{
								// mem alloc 을 실패한 경우 unknown error로 처리(???)
								pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
								HxLOG_Error("pstSystemInformation is NULL \n");
							}
						}

						pstDrmRstMsg->hErrorCode = hError;
					}
					break;
				case eMSG_PRMS_CHINFO:
					{
						TDvlChipsetInfo *pxChipsetInformation = NULL;
						XmgrCas_NaPrmChipsetInfo_t *pLocalChipsetInfo = NULL;

						hError = xmgr_cas_NadvlChipsetInformationRequest((TDvlChipsetInfo**)&pxChipsetInformation);
						if(hError == ERR_OK)
						{
							pstDrmRstMsg->pstChipsetInformation= (XmgrCas_NaPrmChipsetInfo_t*)HLIB_STD_MemAlloc(sizeof(XmgrCas_NaPrmChipsetInfo_t));
							if(pstDrmRstMsg->pstChipsetInformation != NULL)
							{
								HxSTD_memset(pstDrmRstMsg->pstChipsetInformation, 0x00, sizeof(XmgrCas_NaPrmChipsetInfo_t));

								pLocalChipsetInfo = pstDrmRstMsg->pstChipsetInformation;

								pLocalChipsetInfo->ulNuId = (HUINT32)pxChipsetInformation->nuidValue;
								HxSTD_memcpy(pLocalChipsetInfo->szChipsetTypeStr, pxChipsetInformation->type, CHIPSET_TYPE_STRING_LENGTH);
								HxSTD_memcpy(pLocalChipsetInfo->szNuIdStr, pxChipsetInformation->nuidString, UNIQUE_ID_STRING_LENGTH);
							}
							else
							{
								// mem alloc 을 실패한 경우 unknown error로 처리(???)
								pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
								HxLOG_Error("pstChipsetInformation is NULL \n");
							}
						}

						pstDrmRstMsg->hErrorCode = hError;
					}
					break;
				case eMSG_PRMS_PSTART:
					{
						TSize               xOutputCredentialsSize;
						const TUnsignedInt8	*pxOutputCredentials = NULL;
						TDvlAccessStatus	xPlaybackStatus = 0;
						TSize				xGenericMetadataSize;
						const TUnsignedInt8	*pxGenericMetadata = NULL;
						TSize				xSpecificMetadataSize;
						const TUnsignedInt8	*pxSpecificMetadata=NULL;
						TDvlHandle			*pxPlaybackHandle = NULL;

						HUINT8	*pCredentialHex = NULL;
						HUINT32 ulCredentialHexSize = 0;
						HUINT32 ulPlaybackHandle = 0;
						XmgrCas_NaPrmCredentialInfo_t CredentialInfo;
#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1) && !defined(__FORCED_USE_PRM_2_6__)
						TDvlEntitlementsParameters	xEntitlementsParameters;
	 					TSize 						xKeyIdSize;
						TUnsignedInt8				*pxKeyId;
#endif

						HxSTD_memset(&CredentialInfo, 0, sizeof(CredentialInfo));
						eResultCodeStatus = _xmgr_cas_naprm_GetCredentialInfoFromCredentialHandle(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle, &CredentialInfo);
						if(eResultCodeStatus==eCRES_RESULTCODE_SUCCESS)
						{
							if(_xmgr_cas_naprm_IsExistActiveCredential(pstCreHdlMgr, &ulPlaybackHandle)) // "active"
							{
								//[추후 주석제거] PSTOP없이 두개이상의 PSTART가 들어온경우 STB는 자동으로 active credential을 deactive하고 가장 최신의 PSTART command를 사용한다.
								if(_xmgr_cas_naprm_StopPlaybackByPlaybackHandle(pstCreHdlMgr, ulPlaybackHandle)!=ERR_OK)
								{
									// TODO : Test Suite로 test시 Error Handling Repeated PSTART시 StopPlayback 문제일 경우 How ?
								}
							}

							if(hError==ERR_OK)
							{

								pCredentialHex = (HUINT8*)HLIB_STD_MemAlloc(CredentialInfo.usSize+2);
								if(pCredentialHex==NULL)
								{
									HxLOG_Error("pCredentialHex is NULL \n");
									eResultCodeStatus = eCRES_RESULTCODE_ERROR_MALLOC;
									break;
								}
								HxSTD_memset(pCredentialHex, 0, CredentialInfo.usSize+2);
								if(_xmgr_cas_naprm_ConvertDecimalToHex(CredentialInfo.pucData, CredentialInfo.usSize, pCredentialHex, &ulCredentialHexSize)==ERR_OK)
								{
#if defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
									hError |= xmgr_cas_AddTransportSessionIdDirect(pstDrmSendMsg->ulTransportSessionId);
									if(hError != ERR_OK)
									{
										HxLOG_Error("Error in [xmgr_cas_AddTransportSessionIdDirect] TSID[%d]\n", pstDrmSendMsg->ulTransportSessionId);
									}
#endif

#if 0//defined(CONFIG_DEBUG)
									_xmgr_cas_naprm_PrintCredentialInfomation(ulCredentialHexSize, pCredentialHex);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM_2_6) || defined (__FORCED_USE_PRM_2_6__)
									_xmgr_cas_naprm_SendClearDrmInformation();
									hError = xmgr_cas_NadvlStartPlayback(pstDrmSendMsg->ulTransportSessionId
																		, ulCredentialHexSize
																		, pCredentialHex
																		, _xmgr_cas_naprm_PlayBackErrorCallback
																		, &xOutputCredentialsSize
																		, &pxOutputCredentials
																		, &xPlaybackStatus
																		, &xGenericMetadataSize
																		, &pxGenericMetadata
																		, &xSpecificMetadataSize
																		, &pxSpecificMetadata
																		, &pxPlaybackHandle);
#elif defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
									if(hError == ERR_OK)
									{
										hError = xmgr_cas_NadvlPlaybackOpenSession(pstDrmSendMsg->ulTransportSessionId, &pxPlaybackHandle);
										if(hError == ERR_OK)
										{
											hError = xmgr_cas_NadvlPlaybackSetEntitlements(pxPlaybackHandle, &xEntitlementsParameters, &xPlaybackStatus, &xOutputCredentialsSize, &pxOutputCredentials, &xKeyIdSize, &pxKeyId);
											if(hError != ERR_OK)
											{
												HxLOG_Error("Error in [xmgr_cas_NadvlPlaybackSetEntitlements] pxPlaybackHandle[%p]\n", pxPlaybackHandle);
											}
										}
										else
										{
											HxLOG_Error("Error in [xmgr_cas_NadvlPlaybackOpenSession] TSID[%d]\n", pstDrmSendMsg->ulTransportSessionId);
										}
									}
#endif

									HxLOG_Print("xPlaybackStatus : [%d], pxPlaybackHandle(0x%08x)(%d)\n", xPlaybackStatus, pxPlaybackHandle, pxPlaybackHandle);

#if defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
									if((hError != ERR_OK) || (xPlaybackStatus != DVL_ACCESS_GRANTED))
									{
										if(xmgr_cas_RemoveTransportSessionIdDirect(pstDrmSendMsg->ulTransportSessionId) != ERR_OK)
										{
											HxLOG_Error("Error in [xmgr_cas_AddTransportSessionIdDirect] TSID[%d]\n", pstDrmSendMsg->ulTransportSessionId);
										}
									}
#endif

									//[추후 주석제거] StartPlayback : Success인경우.
									if(hError==ERR_OK)
									{
										pstDrmRstMsg->hPlayBackHandle = (HUINT32)pxPlaybackHandle;
										pstDrmRstMsg->hStat = (HUINT32)xPlaybackStatus;
										_xmgr_cas_naprm_SetMsgIDByCredentialHandle(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle, pstDrmRstMsg->ucMsgID);

										//[추후 주석제거] playback handle을 저장한다.
										_xmgr_cas_naprm_SetPlaybackHandleByCredentialHandle(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle, pstDrmRstMsg->hPlayBackHandle);

										_xmgr_cas_naprm_SetPlaybackKeyInformation(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle, pstDrmSendMsg->ulTransportSessionId, pstDrmRstMsg->hPlayBackHandle);
#if 0//defined(CONFIG_DEBUG)
										_xmgr_cas_naprm_PrintCredentialInfomation(xOutputCredentialsSize, pxOutputCredentials);
#endif

#if defined SUPPORT_REPLACE_CREDENTIAL
										if(xOutputCredentialsSize != 0 && pxOutputCredentials != NULL)
										{
											_xmgr_cas_naprm_ReplaceCredential(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle, xOutputCredentialsSize, pxOutputCredentials);
										}
#endif

										if(pxOutputCredentials)
										{
											xmgr_cas_NadvlReleaseBuffer(pxOutputCredentials);
										}

										if(pxGenericMetadata)
										{
#ifdef SUPPORT_CREDENTIAL_GMD
											//[추후 주석제거] GenericMatadata를 얻어온경우 resultMsg를위해 data를 저장한다.
											if(_xmgr_cas_naprm_CreateGenericMetadata(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle, xGenericMetadataSize, pxGenericMetadata)!=ERR_OK)
											{
												eResultCodeStatus = eCRES_RESULTCODE_ERROR_OTHER;
											}
#endif
											xmgr_cas_NadvlReleaseBuffer(pxGenericMetadata);
										}

										if(pxSpecificMetadata)
										{
#if defined(SUPPORT_USAGERULE)
											if(_xmgr_cas_naprm_CreageUsageRuleFromSpecificMetadata(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle, xSpecificMetadataSize, pxSpecificMetadata)!=ERR_OK)
											{
												eResultCodeStatus = eCRES_RESULTCODE_ERROR_OTHER;
											}
#endif
											xmgr_cas_NadvlReleaseBuffer(pxSpecificMetadata);
										}
									}
								}
								HLIB_STD_MemFree(pCredentialHex);
								pCredentialHex = NULL;
							}
						}

						pstDrmRstMsg->hErrorCode = hError;
					}
					break;
				case eMSG_PRMS_PSTOP:
					{
						XmgrCas_NaPrmCredentialInfo_t CredentialInfo;
						HxSTD_memset(&CredentialInfo, 0, sizeof(CredentialInfo));
						eResultCodeStatus = _xmgr_cas_naprm_GetCredentialInfoFromPlaybackHandle(pstCreHdlMgr, pstDrmSendMsg->hPlayBackHandle, &CredentialInfo);
						//[추후 주석제거] STOP할 Handle이 존재하는경우.
						if(eResultCodeStatus==eCRES_RESULTCODE_SUCCESS)
						{
							//[추후 주석제거] Playback Handle에의해 Play를 stop한다. 즉 "deactive"시킨다.
							hError = _xmgr_cas_naprm_StopPlaybackByPlaybackHandle(pstCreHdlMgr, pstDrmSendMsg->hPlayBackHandle);
						}

						pstDrmRstMsg->hErrorCode = hError;
					}
					break;
				case eMSG_PRMS_PSTOPALL:
					{
						HINT32 usIdx = 0;
						HBOOL bIsExistStopedPlayback = FALSE;
						XmgrCas_NaPrmCredentialInfo_t CredentialInfo;
						//[추후 주석제거] 모든 Credential Index에대해 PSTOP을 시도한다.
						for(usIdx=0;usIdx<MAX_CREDENTIAL_NUMBER;usIdx++)
						{
							HxSTD_memset(&CredentialInfo, 0, sizeof(XmgrCas_NaPrmCredentialInfo_t));
							eResultCodeStatus = _xmgr_cas_naprm_GetCredentialInfoFromCredentialIndex(pstCreHdlMgr, usIdx, &CredentialInfo);

							if(eResultCodeStatus==eCRES_RESULTCODE_SUCCESS)
							{
								//[추후 주석제거] STOP할 Playback Handle이 존재하는경우에만 Stop한다.
								if(CredentialInfo.ulPlayBackHandle>INVALID_PLAYBACK_HANDLE)
								{
									bIsExistStopedPlayback = TRUE;
									//[추후 주석제거] Playback Handle에의해 Play를 stop한다. 즉 "deactive"시킨다.
									hError = _xmgr_cas_naprm_StopPlaybackByPlaybackHandle(pstCreHdlMgr, CredentialInfo.ulPlayBackHandle);
								}
							}
						}
						//[추후 주석제거] Stoped된 Handle이 존재하지 않는경우 PSTART없이 PSTOP한것으로 일단 간주한다.
						if(bIsExistStopedPlayback==FALSE)
						{
							eResultCodeStatus = eCRES_RESULTCODE_ERROR_PSTOP_WITHOUT_PSTART;
						}
						else
						{
							// bIsExistStopedPlayback 이 존재하는경우 resultCode가 0이어야함.
							eResultCodeStatus = eCRES_RESULTCODE_SUCCESS;
						}
					}
					break;
				case eMSG_CRES_INFO:
					{
						TCredentialsType		xCredentialsType;
						const TCredentialsUnion	*pxCredentialsUnion=NULL;
						TSize					xGenericMetadataSize;
						const TUnsignedInt8		*pxGenericMetadata =NULL;
						TSize					xSpecificMetadataSize;
						const TUnsignedInt8		*pxSpecificMetadata = NULL;

						XmgrCas_NaPrmResultCredentialInfo_t	*pLocalResultCredentialInfo = NULL;

						HUINT8	*pCredentialHex = NULL;
						HUINT32 ulCredentialHexSize = 0;
						XmgrCas_NaPrmCredentialInfo_t CredentialInfo;

						HxSTD_memset(&CredentialInfo, 0, sizeof(CredentialInfo));
						eResultCodeStatus = _xmgr_cas_naprm_GetCredentialInfoFromCredentialHandle(pstCreHdlMgr, pstDrmSendMsg->hCredetialHandle, &CredentialInfo);
						if(eResultCodeStatus==eCRES_RESULTCODE_SUCCESS)
						{
							pCredentialHex = (HUINT8*)HLIB_STD_MemAlloc(CredentialInfo.usSize+2);
							if(pCredentialHex==NULL)
							{
								HxLOG_Error("pCredentialHex is NULL \n");
								eResultCodeStatus = eCRES_RESULTCODE_ERROR_MALLOC;
								break;
							}
							HxSTD_memset(pCredentialHex, 0, CredentialInfo.usSize+2);

							// HEX로 넘기는 경우에 사용됨 만일 사용하지 않을경우 아래 함수 제거 .
							if(_xmgr_cas_naprm_ConvertDecimalToHex(CredentialInfo.pucData, CredentialInfo.usSize, pCredentialHex, &ulCredentialHexSize)==ERR_OK)
							{
								hError = xmgr_cas_NadvlCredentialsInformationRequest(ulCredentialHexSize
																			, pCredentialHex
																			, &xCredentialsType
																			, &pxCredentialsUnion
																			, &xGenericMetadataSize
																			, &pxGenericMetadata
																			, &xSpecificMetadataSize
																			, &pxSpecificMetadata);
								// [추후 주석제거] DVL로부터 유효한 credential 정보를 얻어온경우.
								if(hError==ERR_OK)
								{
									pstDrmRstMsg->pstResultCredentialInfo = (XmgrCas_NaPrmResultCredentialInfo_t*)HLIB_STD_MemAlloc(sizeof(XmgrCas_NaPrmResultCredentialInfo_t));
									if(pstDrmRstMsg->pstResultCredentialInfo == NULL)
									{
										HxLOG_Error("pstResultCredentialInfo is NULL \n");
										HLIB_STD_MemFree(pCredentialHex);
										pCredentialHex = NULL;
										eResultCodeStatus = eCRES_RESULTCODE_ERROR_MALLOC;
										break;
									}
									pLocalResultCredentialInfo = pstDrmRstMsg->pstResultCredentialInfo;
#ifdef CONFIG_DEBUG
									_xmgr_cas_naprm_PrintCredentialInfo(pxCredentialsUnion, xCredentialsType);
#endif
									// TODO 추후 확인 필요.cerdential type (CREDENTIALS_TYPE_PROFILE, CREDENTIALS_TYPE_HOME_NETWORK_LCM)
									// 우선 3개 처리 한다..
									if(xCredentialsType == CREDENTIALS_TYPE_VOD_EMM
										|| xCredentialsType == CREDENTIALS_TYPE_VOD_ECM
										|| xCredentialsType== CREDENTIALS_TYPE_LCM)
									{
										pLocalResultCredentialInfo->ucCredentialsType = (HUINT8)xCredentialsType;

										if(xCredentialsType == CREDENTIALS_TYPE_VOD_EMM)
										{
											HxSTD_memcpy(&pLocalResultCredentialInfo->stCredentialUnion.vodEmmData, &pxCredentialsUnion->vodEmmData, sizeof(TVodEmmData));
										}
										else if(xCredentialsType == CREDENTIALS_TYPE_VOD_ECM)
										{
											HxSTD_memcpy(&pLocalResultCredentialInfo->stCredentialUnion.vodEcmData, &pxCredentialsUnion->vodEcmData, sizeof(TVodEcmData));
										}
										else if(xCredentialsType == CREDENTIALS_TYPE_LCM)
										{
											HxSTD_memcpy(&pLocalResultCredentialInfo->stCredentialUnion.lcmData, &pxCredentialsUnion->lcmData, sizeof(TLcmData));
										}

#ifdef SUPPORT_CREDENTIAL_GMD
										if(pxGenericMetadata && (HUINT32)xGenericMetadataSize > 0)
										{
											pLocalResultCredentialInfo->ulGenericMetadataSize = (HUINT32)xGenericMetadataSize;
											pLocalResultCredentialInfo->pucGenericMetadata = HLIB_STD_MemAlloc(sizeof(HUINT8) * pLocalResultCredentialInfo->ulGenericMetadataSize+1);
											if(pLocalResultCredentialInfo->pucGenericMetadata != NULL)
											{
												HxSTD_memset(pLocalResultCredentialInfo->pucGenericMetadata, 0, (sizeof(HUINT8) * pLocalResultCredentialInfo->ulGenericMetadataSize)+1);
												HxSTD_memcpy(pLocalResultCredentialInfo->pucGenericMetadata, pxGenericMetadata, pLocalResultCredentialInfo->ulGenericMetadataSize);
											}
											else
											{
												HxLOG_Error("pucGenericMetadata is NULL \n");
											}
										}
										else
										{
											pLocalResultCredentialInfo->ulGenericMetadataSize = 0;
											pLocalResultCredentialInfo->pucGenericMetadata = NULL;
										}
#endif

										if(pxSpecificMetadata && (HUINT32)xSpecificMetadataSize > 0)
										{
											pLocalResultCredentialInfo->ulSpecificMetadataSize = (HUINT32)xSpecificMetadataSize;
											pLocalResultCredentialInfo->pucSpecificMetadata = HLIB_STD_MemAlloc(sizeof(HUINT8) * pLocalResultCredentialInfo->ulSpecificMetadataSize+1);
											if(pLocalResultCredentialInfo->pucSpecificMetadata != NULL)
											{
												HxSTD_memset(pLocalResultCredentialInfo->pucSpecificMetadata, 0, (sizeof(HUINT8) * pLocalResultCredentialInfo->ulSpecificMetadataSize)+1);
												HxSTD_memcpy(pLocalResultCredentialInfo->pucSpecificMetadata, pxSpecificMetadata, pLocalResultCredentialInfo->ulSpecificMetadataSize);
											}
											else
											{
												HxLOG_Error("pucSpecificMetadata is NULL \n");
											}
										}
										else
										{
											pLocalResultCredentialInfo->ulSpecificMetadataSize = 0;
											pLocalResultCredentialInfo->pucSpecificMetadata = NULL;
										}
									}

									if(pxGenericMetadata)
									{
										xmgr_cas_NadvlReleaseBuffer(pxGenericMetadata);
									}
									if(pxSpecificMetadata)
									{
										xmgr_cas_NadvlReleaseBuffer(pxSpecificMetadata);
									}
								}
							}
							HLIB_STD_MemFree(pCredentialHex);
							pCredentialHex = NULL;
						}
					}
					break;
				default:
					break;
			}
			pstDrmRstMsg->eResultCode = _xmgr_cas_naprm_MakeSecondResultCode(eResultCodeStatus);
		}
	}

	return;
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
STATIC void _xmgr_cas_naprm_ProcessHdPlusSC(XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg, XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg)
{
	eXmgrCas_NaPrmSendMsgType_t eMsgType = eMSGTYPE_MAX;
	eXmgrCas_NaPrmSendMsg_t eMsg = eMSG_MAX;
	HUINT32 hError = ERR_OK;

	if(pstDrmSendMsg == NULL || pstDrmRstMsg == NULL)
	{
		HxLOG_Error("pstDrmSendMsg or pstDrmRstMsg is NULL\n");
#if !defined (CONFIG_OP_SES)
		pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
#endif
		return;
	}

	eMsgType = pstDrmSendMsg->eMsgType;
	eMsg = pstDrmSendMsg->eMsg;

	if(eMsgType == eMSGTYPE_HDPLUSCA_SC)
	{
		if(eMsg == eMSG_SC_INSERTED)
		{
			hError = CA_NA_CakGetEcmCaSystemId(0, &pstDrmRstMsg->usEcmCasSystemID);
			if(hError != ERR_OK)
			{
				pstDrmRstMsg->hErrorCode = (HUINT32)FALSE;
			}
			else
			{
				pstDrmRstMsg->hErrorCode = (HUINT32)TRUE;
			}
		}
		else if(eMsg == eMSG_SC_PRODUCTVALIDATION)
		{
			TBoolean	bExpired = TRUE;
			TCaDuration	ulDuration;
			HUINT8		*pucProductionParam= NULL, *pDescBuf = NULL, *pucPtr = NULL;
			HUINT32		ulBufferLen = 0, ulDesLen = 0;
			/*
				pucProductionParam 은 Merlin_Access_Criteria_Descriptor 의 Private_data_byte 의 내용임.
				그래서, MW_CA_NA_GetValidityHdProduct() 함수에 넘겨주기는 위해서는 아래와 같아야 함.

					- ulDesLen = 구해진 ulDesLen + 2byte(Private_data_length)
					- pucPtr = Private_data_length 값(2byte) + pDescBuf

				eSubscription 인 경우에는 ulDuration을 check 할 필요 없음..
			*/

			pucProductionParam = pstDrmSendMsg->pucProductionParam;
			ulBufferLen = HxSTD_StrLen(pucProductionParam);

			pDescBuf = (HUINT8*) MW_MEM_Alloc(sizeof(HUINT8) * ulBufferLen);
			if(pDescBuf == NULL)
			{
				pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
				HxLOG_Error("MW_MEM_Alloc Error\n");
				return;
			}

			pucPtr = pDescBuf;
			pucProductionParam ++;
			ulBufferLen --;

			// Private_data_length 를 넣어주기 위함.
			pDescBuf = pDescBuf+2;

			while(pucProductionParam[0] != 0 && pucProductionParam[0] != ')')
			{
				if(ulBufferLen == 0)
				{
					break;
				}

				*pDescBuf = MAKE_STROF2BYTE_2_NUM(pucProductionParam);

				pucProductionParam = pucProductionParam+2;
				ulBufferLen -=2;
				pDescBuf++;
				ulDesLen++;
			}

			// Private_data_length 를 넣어주기 위함.
			pucPtr[0] =(HUINT8) (((HUINT16) ulDesLen) & 0xff00);
			pucPtr[1] = (HUINT8)(((HUINT16) ulDesLen) & 0x00ff);

			// Private_data_length
			ulDesLen+=2;

#ifdef CONFIG_DEBUG
			_xmgr_cas_naprm_PrintParseSCProcuct(pucPtr, ulDesLen);
#endif
			hError = MW_CA_NA_GetValidityHdProduct(eSubscription, ulDesLen , pucPtr , &bExpired, &ulDuration);
			if(hError != ERR_OK)
			{
				hError = MW_CA_NA_GetValidityHdProduct(ePre_Paid, ulDesLen , pucPtr , &bExpired, &ulDuration);
				if(hError != ERR_OK)
				{
					pstDrmRstMsg->hErrorCode = (HUINT32) FALSE;
				}
				else
				{
					if(bExpired==FALSE && ulDuration>0) //만료 되지 않음, rentalDuration이 0보다 큰경우
					{
						pstDrmRstMsg->hErrorCode = (HUINT32) TRUE;
					}
					else
					{
						pstDrmRstMsg->hErrorCode = (HUINT32) FALSE;
					}
				}
			}
			else
			{
				if(bExpired == FALSE)
				{
					pstDrmRstMsg->hErrorCode = (HUINT32) TRUE;
				}
				else
				{
					pstDrmRstMsg->hErrorCode = (HUINT32) FALSE;
				}
			}

			if(pucPtr != NULL)
			{
				HLIB_STD_MemFree(pucPtr);
				pucPtr = NULL;
			}
		}

		pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_0;
	}

	return;
}
#endif

void _xmgr_cas_naprm_ProcessSendOnHdplusRedbutton(XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg, Handle_t hAction)
{
	HUINT32 ucHbbtvRedbutton = FALSE;

	if(pstDrmSendMsg == NULL)
	{
		HxLOG_Error("pstDrmSendMsg  is NULL\n");
		return;
	}

	if(pstDrmSendMsg->eMsgType == eMSGTYPE_CSPG_HEXBIN && pstDrmSendMsg->eMsg == eMSG_PRMS_SYSINFO)
	{
		ucHbbtvRedbutton = TRUE;
		svc_cas_SvcPortUtilPostMsgToAp(eMEVT_LIVE_SET_HBBTV_ON_HDPLUS_REDBUTTON, hAction, 0, 0, ucHbbtvRedbutton);
	}
}

STATIC void _xmgr_cas_naprm_ProcessSendDrmMsessage(XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg, XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg, XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr)
{
	eXmgrCas_NaPrmSendMsgType_t eMsgType = eMSGTYPE_MAX;

	if(pstDrmSendMsg == NULL || pstDrmRstMsg == NULL || pstCreHdlMgr == NULL)
	{
		HxLOG_Error("pstDrmSendMsg or pstDrmRstMsg or pstCreHdlMgr is NULL\n");
		if (pstDrmRstMsg)
		{
			pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
		}
		return;
	}

	eMsgType = pstDrmSendMsg->eMsgType;

	if(eMsgType==eMSGTYPE_CSPG_HEXBIN)
	{
		_xmgr_cas_naprm_ProcessCredentialHandleManager(pstDrmSendMsg, pstDrmRstMsg, pstCreHdlMgr);
		_xmgr_cas_naprm_ProcessCommunicationWithDvlLib(pstDrmSendMsg, pstDrmRstMsg, pstCreHdlMgr);
	}
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	else if(eMsgType==eMSGTYPE_HDPLUSCA_SC) // HD+ 즉 APS에 해당됨.
	{
		_xmgr_cas_naprm_ProcessHdPlusSC(pstDrmSendMsg, pstDrmRstMsg);
	}
#endif
	else
	{
		HxLOG_Error("eMsgType is error\n");
		pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_1;
	}
	return;
}

/******************************************************************************************
*
*	Parse sendDRMMessage
*
******************************************************************************************/
STATIC eXmgrCas_NaPrmSendMsg_t _xmgr_cas_naprm_ParseConvertMsg(HUINT8 * pucMsg, HUINT32 *pulReadSize, XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg)
{
	eXmgrCas_NaPrmSendMsg_t eMsg = eMSG_MAX;
	HUINT8	*pucPtr = NULL, *pucTemp = NULL;
	HUINT32	ulNumOfData = 0;

	pucPtr = pucMsg;

	if(pstDrmSendMsg->eMsgType == eMSGTYPE_CSPG_HEXBIN)
	{
		if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"PRMS", MESSAGE_SERVICE_NAME_LENGTH) == 0)
		{
			pucPtr = pucPtr + MESSAGE_SERVICE_NAME_LENGTH; //Service Name
			pucPtr = pucPtr + MESSAGE_FIELD_IDENTIFIER_SIZE; //"::"

			pucTemp = pucPtr;

			while ( pucPtr && (pucPtr[0] > 0x00))
			{
				if(HxSTD_StrNCmp ((const HUINT8 *)pucPtr, MESSAGE_FIELD_IDENTIFIER, MESSAGE_FIELD_IDENTIFIER_SIZE) == 0)
					break;

				pucPtr++;

				ulNumOfData++;
			}

			if(ulNumOfData >= MESSAGE_METHOD_MIN_LENGTH)
			{
				if(ulNumOfData == 5)
				{
					if(HxSTD_StrNCmp((const HUINT8 *)pucTemp, (const HUINT8 *)"PSTOP", ulNumOfData) == 0)
						eMsg = eMSG_PRMS_PSTOP;
#if defined(SUPPORT_TIME_SETTING)
					else if(HxSTD_StrNCmp((const HUINT8 *)pucTemp, (const HUINT8 *)"PTIME", ulNumOfData) == 0)
					{
						eMsg = eMSG_PRMS_PTIME;
					}
#endif
					else
						eMsg = eMSG_MAX;
				}
				else if(ulNumOfData == 6)
				{
					if(HxSTD_StrNCmp((const HUINT8 *)pucTemp, (const HUINT8 *)"PSTART", ulNumOfData) == 0)
						eMsg = eMSG_PRMS_PSTART;
					else if(HxSTD_StrNCmp((const HUINT8 *)pucTemp, (const HUINT8 *)"CHINFO", ulNumOfData) == 0)
						eMsg = eMSG_PRMS_CHINFO;
					else
						eMsg = eMSG_MAX;
				}
				else if(ulNumOfData == 7)
				{
					if(HxSTD_StrNCmp((const HUINT8 *)pucTemp, (const HUINT8 *)"SYSINFO", ulNumOfData) == 0)
						eMsg = eMSG_PRMS_SYSINFO;
					else
						eMsg = eMSG_MAX;
				}
				else if(ulNumOfData == 8)
				{
					if(HxSTD_StrNCmp((const HUINT8 *)pucTemp, (const HUINT8 *)"PSTOPALL", ulNumOfData) == 0)
						eMsg = eMSG_PRMS_PSTOPALL;
					else
						eMsg = eMSG_MAX;
				}
				else
				{
					eMsg = eMSG_MAX;
				}
			}
			else
			{
				eMsg = eMSG_MAX;
			}
		}
		else if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"CRES", MESSAGE_SERVICE_NAME_LENGTH) == 0)
		{
			pucPtr = pucPtr +MESSAGE_SERVICE_NAME_LENGTH; //Service Name
			pucPtr = pucPtr +MESSAGE_FIELD_IDENTIFIER_SIZE; //"::"

			if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"CCRE", 4) == 0)
				eMsg = eMSG_CRES_CCRE;
			else if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"DCRE", 4) == 0)
				eMsg = eMSG_CRES_DCRE;
			else if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"DALL", 4) == 0)
				eMsg = eMSG_CRES_DALL;
			else if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"PCRE", 4) == 0)
				eMsg = eMSG_CRES_RCRE;
			else if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"INFO", 4) == 0)
				eMsg = eMSG_CRES_INFO ;
			else if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"UGMD", 4) == 0)
				eMsg = eMSG_CRES_UGMD;
			else if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"USMD", 4) == 0)
				eMsg = eMSG_CRES_USMD;
			else
				eMsg = eMSG_MAX;

			ulNumOfData = 4;
		}
		else
		{
			eMsg = eMSG_MAX;
		}
	}
	else if(pstDrmSendMsg->eMsgType == eMSGTYPE_HDPLUSCA_SC)
	{
		if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"SC_inserted", 11) == 0)
		{
			eMsg = eMSG_SC_INSERTED;
			ulNumOfData = 11;
		}
		else if(HxSTD_StrNCmp((const HUINT8 *)pucPtr, (const HUINT8 *)"SC_productValidation", 20) == 0)
		{
			eMsg = eMSG_SC_PRODUCTVALIDATION;
			ulNumOfData = 20;
		}
		else
		{
			eMsg = eMSG_MAX;
		}
	}
	else
	{
		eMsg = eMSG_MAX;
	}

	*pulReadSize = ulNumOfData;

	return eMsg;
}

STATIC void _xmgr_cas_naprm_ParseValueOfField(XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg, HUINT8 * pucMsgField, HUINT32 ulMsgFieldSize)
{
	HUINT8	*pucFieldMsgPtr = NULL;
	HINT32	nRemainSize =0, nCerdentialDataSize = 0;
	HUINT32	ulReadSize=0;

	if(pstDrmSendMsg == NULL || pucMsgField == NULL)
	{
		HxLOG_Error("pstDrmSendMsg or pucMsgField is NULL\n");
		return;
	}

	nRemainSize = (HINT32)ulMsgFieldSize;
	pucFieldMsgPtr = pucMsgField;

	while(*pucFieldMsgPtr!=0x00)
	{
		if(nRemainSize < 0)
		{
			break;
		}

		if(HxSTD_StrNCmp((const HUINT8 *)pucFieldMsgPtr, MESSAGE_VALUE_IDENTIFIER , MESSAGE_VALUE_IDENTIFIER_SIZE) == 0)
		{
			pucFieldMsgPtr = pucFieldMsgPtr + MESSAGE_VALUE_IDENTIFIER_SIZE;

			if(HxSTD_StrNCmp((const HUINT8 *)pucMsgField, "DATA", ulReadSize) == 0)
			{
				nCerdentialDataSize = HxSTD_StrLen(pucMsgField);

				pstDrmSendMsg->pucCredentialData = (HUINT8*)HLIB_STD_MemAlloc(nCerdentialDataSize*sizeof(HUINT8));
				if(pstDrmSendMsg->pucCredentialData == NULL)
				{
					HxLOG_Error("pucCredentialData is NULL\n");
					break;
				}

				pstDrmSendMsg->ulCredentialDataSize = nCerdentialDataSize;
				HxSTD_StrNCpy(pstDrmSendMsg->pucCredentialData,pucFieldMsgPtr, nCerdentialDataSize);
			}
			else if(HxSTD_StrNCmp((const HUINT8 *)pucMsgField, "TSID", ulReadSize) == 0)
			{
				pstDrmSendMsg->ulTransportSessionId = (HUINT32) _xmgr_cas_naprm_Util_atoi(pucFieldMsgPtr, ulMsgFieldSize-ulReadSize-MESSAGE_VALUE_IDENTIFIER_SIZE);
			}
			else if(HxSTD_StrNCmp((const HUINT8 *)pucMsgField, "CREH", ulReadSize) == 0)
			{
				pstDrmSendMsg->hCredetialHandle = (HUINT32) _xmgr_cas_naprm_Util_atoi(pucFieldMsgPtr, ulMsgFieldSize-ulReadSize-MESSAGE_VALUE_IDENTIFIER_SIZE);
			}
			else if(HxSTD_StrNCmp((const HUINT8 *)pucMsgField, "PLAYH", ulReadSize) == 0)
			{
				pstDrmSendMsg->hPlayBackHandle = (HUINT32) _xmgr_cas_naprm_Util_atoi(pucFieldMsgPtr, ulMsgFieldSize-ulReadSize-MESSAGE_VALUE_IDENTIFIER_SIZE);
			}
			else
			{
				HxLOG_Error("pucMsgField is error\n");
			}
		}
		else
		{
			pucFieldMsgPtr++;
			nRemainSize--;
			ulReadSize++;
		}
	}

	return;
}

STATIC void _xmgr_cas_naprm_ParseDrmMessageType(HUINT8 * pucMsgType, XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg)
{
	HUINT16 i = 0;
	eXmgrCas_NaPrmSendMsgType_t eMsgType = eMSGTYPE_MAX;

	if(pucMsgType == NULL || pstDrmSendMsg == NULL)
	{
		HxLOG_Error("pucMsgType or pstDrmSendMsg is NULL\n");
		return;
	}

	for(i = 0; i < eMSGTYPE_MAX; i++)
	{
		if(HxSTD_StrCmp((const HUINT8 *)pucMsgType, (const HUINT8 *)stDrmSendMsgType[i].ucStr) == 0)
		{
			eMsgType = stDrmSendMsgType[i].eMsgType;
		}
	}

	if(eMsgType == eMSGTYPE_MAX)
	{
		HxLOG_Error("eMsgType is error\n");
	}

	pstDrmSendMsg->eMsgType = eMsgType;

	return;
}

STATIC void _xmgr_cas_naprm_ParseDrmMessage(HUINT8 * pucMsg, XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg)
{
	HUINT8	szMsgTempBuffer[DRMAGENT_DRMMESSAGE_MAX_LEN];

	HUINT8	*pucMsgPtr = NULL, *pucMsgTempPtr = NULL;
	HUINT32	ulReadSize = 0;
	HINT32	nRemainStrLen = 0 ;

	if(pucMsg == NULL || pstDrmSendMsg == NULL)
	{
		HxLOG_Error("pucMsg or pstDrmSendMsg is NULL\n");
		return;
	}

	pucMsgPtr = pucMsg;

	HxSTD_memset(szMsgTempBuffer, 0x00, sizeof(HUINT8)*DRMAGENT_DRMMESSAGE_MAX_LEN);

	pstDrmSendMsg->eMsg = _xmgr_cas_naprm_ParseConvertMsg(pucMsg, &ulReadSize, pstDrmSendMsg);

	if(pstDrmSendMsg->eMsg == eMSG_MAX)
	{
		HxLOG_Error("pucMsg or pstDrmSendMsg is NULL\n");
		return;
	}

	switch(pstDrmSendMsg->eMsg)
	{
		// PRMS 처리..
		case eMSG_PRMS_SYSINFO:
		case eMSG_PRMS_CHINFO:
		case eMSG_PRMS_PSTOPALL:
#if 0
// TODO
			if(pstDrmSendMsg->eMsg==eMSG_PRMS_CHINFO)
			{
				if(eNagraPRM_Message_Control == eMESSAGE_CONTROL_HIDE_READY)
				{
					MWC_UTIL_PostMsgToAp(EVT_DRM_NAGRA_PRM_MESSAGE_HIDE, 0, 0, 0, 0);
					eNagraPRM_Message_Control = eMESSAGE_CONTROL_SHOW_READY;
				}
#ifdef SUPPORT_ONLY_ONE_CHIPSYSINFORMATION
				local_mw_nagra_prm_BackUpChipSysInfo();
#endif
			}
#endif
			break;
		case eMSG_PRMS_PSTART:
		case eMSG_PRMS_PSTOP:
			{
				pucMsgPtr = pucMsgPtr+MESSAGE_SERVICE_NAME_LENGTH+MESSAGE_FIELD_IDENTIFIER_SIZE+ulReadSize+MESSAGE_FIELD_IDENTIFIER_SIZE;

				pucMsgTempPtr = pucMsgPtr;
				nRemainStrLen = HxSTD_StrLen(pucMsgPtr);
				ulReadSize = 0;

				while(pucMsgPtr)
				{
					if(nRemainStrLen<0)
						break;

					if(HxSTD_StrNCmp((const HUINT8 *)pucMsgPtr, MESSAGE_FIELD_IDENTIFIER , MESSAGE_FIELD_IDENTIFIER_SIZE) == 0
						|| (nRemainStrLen == 0))
					{
						HxSTD_StrNCpy(szMsgTempBuffer, pucMsgTempPtr, ulReadSize);

						_xmgr_cas_naprm_ParseValueOfField(pstDrmSendMsg, szMsgTempBuffer, ulReadSize);

						if(nRemainStrLen != 0)
						{
							HxSTD_memset(szMsgTempBuffer, 0x00, sizeof(HUINT8)*100);

							ulReadSize = 0;
							pucMsgPtr = pucMsgPtr+MESSAGE_FIELD_IDENTIFIER_SIZE;
							nRemainStrLen--;
							pucMsgTempPtr = pucMsgPtr;
						}
						else
						{
							break;
						}
					}
					else
					{
						pucMsgPtr++;
						nRemainStrLen--;
						ulReadSize++;
					}
				}
			}
			break;
#if defined (SUPPORT_TIME_SETTING)
		case eMSG_PRMS_PTIME:
				pucMsgPtr = pucMsgPtr+MESSAGE_SERVICE_NAME_LENGTH+MESSAGE_FIELD_IDENTIFIER_SIZE+ulReadSize+MESSAGE_FIELD_IDENTIFIER_SIZE;

				nRemainStrLen = HxSTD_StrLen(pucMsgPtr);
				ulReadSize = 0;

				/* "PRMS::PTIME::DATA=2013 03 27 01 02 03 " */
				HxLOG_Print("Time Data String : [%s]\n", pucMsgPtr);
				if((nRemainStrLen == 20 + 5) && HxSTD_StrNCmp((const HUINT8 *)pucMsgPtr, "DATA=" , 5) == 0)
				{
					HxSTD_StrNCpy(szMsgTempBuffer, pucMsgPtr + 5, 19);
					if(_xmgr_cas_naprm_SetTimeFromString(szMsgTempBuffer) != ERR_OK)
					{
						HxLOG_Error("Invalid Time Data String : [%s]\n", pucMsgPtr);
					}
					HxSTD_memset(szMsgTempBuffer, 0x00, sizeof(HUINT8)*100);
				}
				else
				{
					HxLOG_Error("Invalid Time Data String : [%s]\n", pucMsgPtr);
				}

			break;
#endif

		// CRES 처리..
		case eMSG_CRES_CCRE:
		case eMSG_CRES_DCRE:
		case eMSG_CRES_RCRE:
		case eMSG_CRES_INFO:
		case eMSG_CRES_UGMD:
		case eMSG_CRES_USMD:
			{
				pucMsgPtr = pucMsgPtr+MESSAGE_SERVICE_NAME_LENGTH+MESSAGE_FIELD_IDENTIFIER_SIZE+ulReadSize+MESSAGE_FIELD_IDENTIFIER_SIZE;

				pucMsgTempPtr = pucMsgPtr;
				nRemainStrLen = HxSTD_StrLen(pucMsgPtr);
				ulReadSize = 0;

				while(pucMsgPtr)
				{
					if(nRemainStrLen<0)
						break;

					if(HxSTD_StrNCmp((const HUINT8 *)pucMsgPtr, MESSAGE_FIELD_IDENTIFIER , MESSAGE_FIELD_IDENTIFIER_SIZE) == 0
						|| (nRemainStrLen == 0))
					{
						HxSTD_StrNCpy(szMsgTempBuffer, pucMsgTempPtr, ulReadSize);

						_xmgr_cas_naprm_ParseValueOfField(pstDrmSendMsg, szMsgTempBuffer, ulReadSize);

						if(nRemainStrLen != 0)
						{
							HxSTD_memset(szMsgTempBuffer, 0x00, sizeof(HUINT8)*100);

							ulReadSize = 0;
							pucMsgPtr = pucMsgPtr+MESSAGE_FIELD_IDENTIFIER_SIZE;
							nRemainStrLen--;
							pucMsgTempPtr = pucMsgPtr;
						}
						else
						{
							break;
						}
					}
					else
					{
						pucMsgPtr++;
						nRemainStrLen--;
						ulReadSize++;
					}
				}
			}
			break;
		case eMSG_CRES_DALL:
			break;

		// HD+ Smart Card
		case eMSG_SC_INSERTED:
			break;
		case eMSG_SC_PRODUCTVALIDATION:
			{
				pucMsgPtr = pucMsgPtr+ulReadSize;
				if(pucMsgPtr != NULL)
				{
					nRemainStrLen = HxSTD_StrLen(pucMsgPtr);
					if(nRemainStrLen > 0)
					{
						pstDrmSendMsg->pucProductionParam = (HUINT8*)HLIB_STD_MemAlloc(nRemainStrLen*sizeof(HUINT8));
						if(pstDrmSendMsg->pucProductionParam != NULL)
						{
							HxSTD_StrNCpy(pstDrmSendMsg->pucProductionParam, pucMsgPtr, nRemainStrLen);
						}
						else
						{
							HxLOG_Error("pucProductionParam is NULL\n");
						}
					}
			}
			}
			break;
		default :
			break;
	}
	return;
}

STATIC void _xmgr_cas_naprm_ParseDrmSystemId(HUINT8 * pucDRMSystemID, XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg)
{
	HUINT16 i = 0;
	eXmgrCas_NaPrmSendDRMSystemID_t eDrmSystemID = eSYSTEMID_MAX;

	if(pucDRMSystemID == NULL || pstDrmSendMsg == NULL)
	{
		HxLOG_Error("pucDRMSystemID or pstDrmSendMsg is NULL\n");
		return;
	}

	for(i = 0; i < eSYSTEMID_MAX; i++)
	{
		if(HxSTD_StrCmp((const char *)pucDRMSystemID, (const char *)stDrmSendDrmSystemId[i].ucStr) == 0)
		{
			eDrmSystemID = stDrmSendDrmSystemId[i].eDrmSystemID;
		}
	}

	if(eDrmSystemID == eSYSTEMID_MAX)
	{
		HxLOG_Error("eDrmSystemID is not nagra\n");
	}

	pstDrmSendMsg->eDRMSystemID = eDrmSystemID;

	return;
}


/******************************************************************************************
*
*	context Init
*
******************************************************************************************/
STATIC void _xmgr_cas_naprm_SendMsgInit(XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg)
{
	if(pstDrmSendMsg == NULL)
	{
		HxLOG_Error("pstDrmSendMsg error\n");
		return;
	}

	if(pstDrmSendMsg->pucCredentialData != NULL)
	{
		HLIB_STD_MemFree(pstDrmSendMsg->pucCredentialData);
		pstDrmSendMsg->pucCredentialData = NULL;
	}

	if(pstDrmSendMsg->pucProductionParam != NULL)
	{
		HLIB_STD_MemFree(pstDrmSendMsg->pucProductionParam);
		pstDrmSendMsg->pucProductionParam = NULL;
	}

	HxSTD_memset(pstDrmSendMsg, 0, sizeof(XmgrCas_NaPrmDRMSendMessage_t));

	pstDrmSendMsg->eMsgType = eSYSTEMID_MAX;
	pstDrmSendMsg->eMsg = eMSG_MAX;
	pstDrmSendMsg->eDRMSystemID = eMSGTYPE_MAX;
	pstDrmSendMsg->ulCredentialDataSize = 0;
	pstDrmSendMsg->hCredetialHandle = INVALID_CREDENTIAL_HANDLE;
	pstDrmSendMsg->ulTransportSessionId= 0xFFFFFFFF;
	pstDrmSendMsg->hPlayBackHandle= INVALID_CREDENTIAL_HANDLE;
	return;
}

STATIC void _xmgr_cas_naprm_ResultMsgInit(XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg)
{
	if(pstDrmRstMsg == NULL)
	{
		HxLOG_Error("pstDrmRstMsg error\n");
		return;
	}

	if(pstDrmRstMsg->pucMsg != NULL)
	{
		HLIB_STD_MemFree(pstDrmRstMsg->pucMsg);
		pstDrmRstMsg->pucMsg = NULL;
		pstDrmRstMsg->ulMsgSize = 0;
	}

	if(pstDrmRstMsg->pstChipsetInformation != NULL)
	{
		HLIB_STD_MemFree(pstDrmRstMsg->pstChipsetInformation);
		pstDrmRstMsg->pstChipsetInformation = NULL;
	}

	if(pstDrmRstMsg->pstSystemInformation != NULL)
	{
		HLIB_STD_MemFree(pstDrmRstMsg->pstSystemInformation);
		pstDrmRstMsg->pstSystemInformation = NULL;
	}

	if(pstDrmRstMsg->pstResultCredentialInfo != NULL)
	{
#ifdef SUPPORT_CREDENTIAL_GMD
		if(pstDrmRstMsg->pstResultCredentialInfo->pucGenericMetadata)
		{
			HLIB_STD_MemFree(pstDrmRstMsg->pstResultCredentialInfo->pucGenericMetadata);
			pstDrmRstMsg->pstResultCredentialInfo->pucGenericMetadata = NULL;
		}
#endif
		if(pstDrmRstMsg->pstResultCredentialInfo->pucSpecificMetadata)
		{
			HLIB_STD_MemFree(pstDrmRstMsg->pstResultCredentialInfo->pucSpecificMetadata);
			pstDrmRstMsg->pstResultCredentialInfo->pucSpecificMetadata = NULL;
		}
		HLIB_STD_MemFree(pstDrmRstMsg->pstResultCredentialInfo);
		pstDrmRstMsg->pstResultCredentialInfo = NULL;
	}

	HxSTD_memset(pstDrmRstMsg, 0, sizeof(XmgrCas_NaPrmDRMResultMessage_t));

	pstDrmRstMsg->eResultCode = eMSG_RESULTCODE_MAX;
	pstDrmRstMsg->hErrorCode = ERR_OK;
	pstDrmRstMsg->hStat = 0;
	pstDrmRstMsg->hCredetialHandle = INVALID_CREDENTIAL_HANDLE;

	return;
}

// Hbbtv application이 running중에 아래의 경우에 수행하도록 한다.
//	1. Standby Mode (Power button or Sleep time)
//	2. Exit Key
// 	3. New Application
//  4. Network disconnect
//  5. OTA 발생시
//  6. TV Portal 입력시
//	7. 그외 강제적으로 hbbtv operation이 강제 stop되는 경우.
STATIC void _xmgr_cas_naprm_HandleMangerInit(XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr)
{
	HINT16 usIdx = 0;
	XmgrCas_NaPrmCredentialInfo_t *pCreInfo = NULL;

	if(pstCreHdlMgr == NULL)
	{
		HxLOG_Error("pstCreHdlMgr error\n");
		return;
	}

	for(usIdx = 0; usIdx < MAX_CREDENTIAL_NUMBER; usIdx++)
	{
		pCreInfo = &pstCreHdlMgr->stCredentialInfo[usIdx];

		if(pCreInfo==NULL)
			continue;
		/* step
			1. deactivvate any active credential
			2. releae any set usage rule
			3. remove all credentials
			4. clear the parameter buffer
		*/
		// step 1 : deactivvate any active credential
		// step 2 : releae any set usage rule
		if(pCreInfo->ulPlayBackHandle > INVALID_PLAYBACK_HANDLE)
		{
			if(_xmgr_cas_naprm_StopPlaybackByPlaybackHandle(pstCreHdlMgr, pCreInfo->ulPlayBackHandle) != ERR_OK)
			{
				HxLOG_Error("_xmgr_cas_naprm_StopPlaybackByPlaybackHandle error\n");
			}
		}

		// step 3 : remove all credentials
		if(pCreInfo->pucData)
		{
			HLIB_STD_MemFree(pCreInfo->pucData);
			pCreInfo->pucData = NULL;
		}

		// step 4 : clear the parameter buffer
#ifdef SUPPORT_CREDENTIAL_GMD
		// Generic Metadata clear
		if(pCreInfo->pucGenericMetaData)
		{
			HLIB_STD_MemFree(pCreInfo->pucGenericMetaData);
			pCreInfo->pucGenericMetaData = NULL;
			pCreInfo->ulGenericMetaDataSize = 0;
		}
#endif
		// Credential Metadata clear
		if(pCreInfo->pucSpecificMetaData)
		{
			HLIB_STD_MemFree(pCreInfo->pucSpecificMetaData);
			pCreInfo->pucSpecificMetaData = NULL;
			pCreInfo->ulSpecificMetaDataSize = 0;
		}
	}

	HxSTD_memset(pstCreHdlMgr, 0, sizeof(XmgrCas_NaPrmCredentialHandleManager_t));

	return;
}

STATIC XmgrCas_NaPrmContext_t *_xmgr_cas_naprm_GetContext(void)
{
	return &stNaPrmContext;
}

STATIC BUS_Result_t _xmgr_cas_naprm_MsgBusCreate(XmgrCas_NaPrmContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}

	HxSTD_memset(pstContext, 0x00, sizeof(XmgrCas_NaPrmContext_t));

	pstContext->pstDrmSendMessage = (XmgrCas_NaPrmDRMSendMessage_t *)HLIB_STD_MemAlloc(sizeof(XmgrCas_NaPrmDRMSendMessage_t));
	if(pstContext->pstDrmSendMessage == NULL)
	{
		HxLOG_Error("pstDrmSendMessage is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}

	pstContext->pstDrmResultMessage = (XmgrCas_NaPrmDRMResultMessage_t *)HLIB_STD_MemAlloc(sizeof(XmgrCas_NaPrmDRMResultMessage_t));
	if(pstContext->pstDrmResultMessage == NULL)
	{
		HxLOG_Error("pstDrmResultMessage is NULL\n");
		if(pstContext->pstDrmSendMessage)
	{
			HLIB_STD_MemFree(pstContext->pstDrmSendMessage);
			pstContext->pstDrmSendMessage = NULL;
		}
		return ERR_BUS_SOURCE_NULL;
	}

	pstContext->pstDrmCredentialHandleManager = (XmgrCas_NaPrmCredentialHandleManager_t *)HLIB_STD_MemAlloc(sizeof(XmgrCas_NaPrmCredentialHandleManager_t));
	if(pstContext->pstDrmCredentialHandleManager == NULL)
	{
		HxLOG_Error("pstDrmSendMessage is NULL\n");
		if(pstContext->pstDrmSendMessage)
		{
			HLIB_STD_MemFree(pstContext->pstDrmSendMessage);
			pstContext->pstDrmSendMessage = NULL;
		}
		if(pstContext->pstDrmResultMessage)
		{
			HLIB_STD_MemFree(pstContext->pstDrmResultMessage);
			pstContext->pstDrmResultMessage = NULL;
		}
		return ERR_BUS_SOURCE_NULL;
	}

	HxSTD_memset(pstContext->pstDrmSendMessage, 0, sizeof(XmgrCas_NaPrmDRMSendMessage_t));
	HxSTD_memset(pstContext->pstDrmResultMessage, 0, sizeof(XmgrCas_NaPrmDRMResultMessage_t));
	HxSTD_memset(pstContext->pstDrmCredentialHandleManager, 0, sizeof(XmgrCas_NaPrmCredentialHandleManager_t));

	_xmgr_cas_naprm_SendMsgInit(pstContext->pstDrmSendMessage);
	_xmgr_cas_naprm_ResultMsgInit(pstContext->pstDrmResultMessage);
	_xmgr_cas_naprm_HandleMangerInit(pstContext->pstDrmCredentialHandleManager);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_naprm_MsgBusDestory(XmgrCas_NaPrmContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}

	_xmgr_cas_naprm_SendMsgInit(pstContext->pstDrmSendMessage);
	_xmgr_cas_naprm_ResultMsgInit(pstContext->pstDrmResultMessage);
	_xmgr_cas_naprm_HandleMangerInit(pstContext->pstDrmCredentialHandleManager);

#ifdef CONFIG_DEBUG
	_xmgr_cas_naprm_MemoryCheck(pstContext, eHDL_MEMORY_CHECK);
#endif

	if(pstContext->pstDrmSendMessage)
	{
		HLIB_STD_MemFree(pstContext->pstDrmSendMessage);
		pstContext->pstDrmSendMessage = NULL;

	}
	if(pstContext->pstDrmResultMessage)
	{
		HLIB_STD_MemFree(pstContext->pstDrmResultMessage);
		pstContext->pstDrmResultMessage = NULL;
	}
	if(pstContext->pstDrmCredentialHandleManager)
	{
		HLIB_STD_MemFree(pstContext->pstDrmCredentialHandleManager);
		pstContext->pstDrmCredentialHandleManager = NULL;
	}

#ifdef CONFIG_DEBUG
	_xmgr_cas_naprm_MemoryCheck(pstContext, eCXT_MEMORY_CHECK);
#endif

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_naprm_MsgDrmSendMessage(XmgrCas_NaPrmContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaPrmDRMSendMessage_t *pstDrmSendMsg = NULL;
	XmgrCas_NaPrmDRMResultMessage_t *pstDrmRstMsg = NULL;
	XmgrCas_NaPrmCredentialHandleManager_t *pstCreHdlMgr = NULL;
	MgrDrmAgentSendDrmMessage_t stDrmMessage;
	HUINT16 usMsgId = 0;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}

	if((void *)p3 == NULL)
	{
		HxLOG_Error("p3 is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}

	usMsgId = (HUINT16)p1;
	HxSTD_memcpy((void *)&stDrmMessage, (void *)p3, sizeof(MgrDrmAgentSendDrmMessage_t));

	pstDrmSendMsg = pstContext->pstDrmSendMessage;
	pstDrmRstMsg = pstContext->pstDrmResultMessage;
	pstCreHdlMgr = pstContext->pstDrmCredentialHandleManager;

	_xmgr_cas_naprm_SendMsgInit(pstDrmSendMsg);
	_xmgr_cas_naprm_ResultMsgInit(pstDrmRstMsg);
#ifdef CONFIG_DEBUG
	_xmgr_cas_naprm_MemoryCheck(pstContext, eMSG_MEMORY_CHECK);
#endif

	_xmgr_cas_naprm_ParseDrmMessageType(stDrmMessage.szDrmMsgType, pstDrmSendMsg);
	_xmgr_cas_naprm_ParseDrmMessage(stDrmMessage.szDrmMsg, pstDrmSendMsg);
	_xmgr_cas_naprm_ParseDrmSystemId(stDrmMessage.szDrmSystemId, pstDrmSendMsg);
#ifdef CONFIG_DEBUG
	_xmgr_cas_naprm_PrintDrmSendMessage(stDrmMessage.szDrmMsgType, stDrmMessage.szDrmMsg, stDrmMessage.szDrmSystemId, pstDrmSendMsg);
#endif

	pstDrmRstMsg->ucMsgID = usMsgId;

	if(_xmgr_cas_naprm_MakeFirstResultCode(pstDrmSendMsg, pstDrmRstMsg,  pstCreHdlMgr) == ERR_OK)
	{
		_xmgr_cas_naprm_ProcessSendOnHdplusRedbutton(pstDrmSendMsg, hAction);
		_xmgr_cas_naprm_ProcessSendDrmMsessage(pstDrmSendMsg, pstDrmRstMsg, pstCreHdlMgr);
		_xmgr_cas_naprm_ProcessDrmResultMessage(pstDrmSendMsg, pstDrmRstMsg, pstCreHdlMgr);
	}

	if( (pstDrmRstMsg->eResultCode != eMSG_RESULTCODE_0) || (pstDrmRstMsg->pucMsg == NULL) )
	{
		_xmgr_cas_naprm_MakeResultEmptyMessage(pstDrmRstMsg);
	}

#ifdef CONFIG_DEBUG
	_xmgr_cas_naprm_PrintDrmResultMessage(pstDrmRstMsg, pstCreHdlMgr);
#endif

	(void)BUS_PostDataToOM(NULL, eMEVT_DRMAGENT_COMMON_RESPONSEDRMMESSAGERESULT, HANDLE_NULL, pstDrmRstMsg->ucMsgID, pstDrmRstMsg->eResultCode, pstDrmRstMsg->pucMsg, pstDrmRstMsg->ulMsgSize);

	return ERR_BUS_MESSAGE_BREAK;
//	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_naprm_MsgReqDrmStatuesMessage(XmgrCas_NaPrmContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if !defined (CONFIG_OP_SES)		// not implemented yet.
	HCHAR	*pzDrmSystemId = NULL, *pzStr = (HCHAR *)p3;
	HUINT32	ulSize;//, ulDrmSystmeId;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}

	if(pzStr == NULL)
	{
		HxLOG_Error("pzStr is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}

	ulSize = HxSTD_StrLen(pzStr);
	pzDrmSystemId = HLIB_STD_MemAlloc(ulSize + 1);
	if (pzDrmSystemId == NULL)
	{
		HxLOG_Error("pzDrmSystemId is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}
	HxSTD_memset(pzDrmSystemId, 0, ulSize + 1);
	HxSTD_memcpy(pzDrmSystemId, pzStr, ulSize);

#if 0
	/* 1 */
	//Parse & Check DRM System ID : stDrmMessage.szDrmSystemId
	ulDrmSystmeId = ;

	if(ulDrmSystemId == NAGRA) /* CAS System ID가 Nagra에 해당된다면 다른 procedure 가 더이상 이 message를 처리할 필요가 없다 */
	{
		/* 2 */
		//Nothing to DO;
		return ERR_BUS_MESSAGE_BREAK;
	}
#endif
#endif
	return ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t _xmgr_cas_naprm_MsgDrmStatusChanged(XmgrCas_NaPrmContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	HCHAR			*pzDrmSystemId = NULL;
//	DxCAS_GroupId_e	eCasGrpId = (DxCAS_GroupId_e)p1;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_SOURCE_NULL;
	}

#if 0
	if(
	/* 1 */
	//Make a DRM System ID String Data

	/* 2 */
	//Send Drm System ID string to OM
	(void)BUS_PostDataToOM(NULL, eMEVT_DRMAGENT_COMMON_RESPONSEDRMSYSTEMSTATUS, HANDLE_NULL, 0, 0, pzDrmSystemID);
	return ERR_BUS_MESSAGE_BREAK;
#endif

	return ERR_BUS_NO_ERROR;
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____PUBLIC_FUNCTION______________________________________________________
BUS_Result_t 	xproc_cas_NaPrmMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eBusResult = ERR_BUS_NO_ERROR;
	XmgrCas_NaPrmContext_t	*pstContext = _xmgr_cas_naprm_GetContext();

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			eBusResult = _xmgr_cas_naprm_MsgBusCreate(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_TIMER :
			break;

#if defined(CONFIG_OP_SES)
		/* standby에서도 떠 있어야 하므로 그냥 PASS
		    stanby mode진입후 바로 live로 전환시 해당 proc이 distory되는 문제로인해 live 전환시 msg 처리 오류 발생.
		*/
		case eMEVT_BUS_READY_SHUTDOWN:
			return MESSAGE_PASS;
#endif

		case eMEVT_BUS_DESTROY :
			eBusResult = _xmgr_cas_naprm_MsgBusDestory(pstContext, hAction, p1, p2, p3);
			/* TODO */
			// mem free, recoveryUsageRule
			break;

		case eOEVT_DRMAGENT_SENDDRMMESSAGE_NAGRA_PRM:
			eBusResult = _xmgr_cas_naprm_MsgDrmSendMessage(pstContext, hAction, p1, p2, p3);
			break;

		case eOEVT_DRMAGENT_REQUESTDRMSYSTEMSTAUTS:
			eBusResult = _xmgr_cas_naprm_MsgDrmSendMessage(pstContext, hAction, p1, p2, p3);
			break;

#if 0
		case MESSAGE_FROM_IMMA:
			break;
#endif

#if 0
		case MESSAGE_FROM_DVL:
			break;
#endif
		case eMEVT_CAS_UPDATE_KEYINFO:
			{
				NaPrmKeyInfo_t stKeyDataInfo;
				HxSTD_MemSet(&stKeyDataInfo, 0, sizeof(NaPrmKeyInfo_t));
				HxSTD_memcpy(&stKeyDataInfo, (void *)p3, sizeof(NaPrmKeyInfo_t));
				eBusResult = _xmgr_cas_naprm_SendDrmInformation(pstContext, &stKeyDataInfo);
				break;
			}
		case eMEVT_CAS_UPDATE_STATUS:
			if(_xmgr_cas_naprm_ProcessCallbackEvent((HUINT32)p1, (TDvlAccessStatus)p2) != ERR_OK)
			{
				HxLOG_Error("Error in _xmgr_cas_naprm_ProcessCallbackEvent()\n");
			}
			break;
		default:
			break;
	}

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

void xmgr_cas_NaPrmCmdInit(HxCMD_Handle_t hCmdHandle)
{
#if defined (CONFIG_DEBUG)
	xmgr_cas_NaPrmCmd(hCmdHandle);
#endif
}

