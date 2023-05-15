/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_cas.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_CAS_H__
#define	__SVC_CAS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <silib.h>
#include <octo_common.h>
#if defined(CONFIG_MW_CH_DVB_T2)
#include <db_svc.h>
#endif
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define CAS_MAX_NUM_OF_ES			64
#define CAS_MAX_NUM_OF_CAPID		64	/* descriptor ���� */
#define CAS_MAX_NUM_OF_DSC		16	// see CAS_MAX_NUM_OF_ES / MAX_NUM_OF_DSC_ID / MAX_DESC_CH_NUM (DI_DSC_SLOTNUM_MAXIMUM) !! -> org is 20 : ��ε��� ���� �������� 10�������� ��ũ������ ���� �� �ִ�. ���� �÷ο� ������ �ʿ���.

#if defined (CONFIG_MW_MM_PVR)
#define CAS_MAX_NUM_OF_TS			4	// CAS MGR���� ó�� ������ TS ����....CAT, SDT, NIT
#else
#define CAS_MAX_NUM_OF_TS			1	// CAS MGR���� ó�� ������ TS ����....CAT, SDT, NIT
#endif


// ��� ����.
#define CAS_MAX_NUM_OF_ACTION	NUM_MAX_ACTION								// octo_config�� ���ǵǾ� ����.

#ifdef CONFIG_MW_CI_NUM_SLOT
#define CAS_MAX_NUM_OF_CAM 		CONFIG_MW_CI_NUM_SLOT
#else
#define CAS_MAX_NUM_OF_CAM 		0
#endif

#ifdef CONFIG_MW_CAS_NUM_SC_SLOT
#define CAS_MAX_NUM_OF_SC			CONFIG_MW_CAS_NUM_SC_SLOT
#else
#define CAS_MAX_NUM_OF_SC			0
#endif

#define CAS_MAX_NUM_OF_SLOT		 max(CAS_MAX_NUM_OF_CAM, CAS_MAX_NUM_OF_SC)

#ifdef CONFIG_MW_CAS_NUM_EMBEDDED
#define CAS_MAX_NUM_OF_ECAS		CONFIG_MW_CAS_NUM_EMBEDDED				// Porting �Ǵ� EMBEDDED CAS�� ����, PAL_MAX_SMCSLOT_NUMBER�� �ƴϴ�.
#else
#define CAS_MAX_NUM_OF_ECAS		0 											// Porting �Ǵ� EMBEDDED CAS�� ����, PAL_MAX_SMCSLOT_NUMBER�� �ƴϴ�.
#endif

#define CAS_MAX_NUM_OF_INSTANCE	(CAS_MAX_NUM_OF_CAM + CAS_MAX_NUM_OF_ECAS)

#define MAX_CI_STR_LEN			40	// CI �� CI+ ���忡 ���� �ּ��� The host shall be able to display 40 characters and 5 lines in addition of title, subtitle and bottom line.

#define	SvcCas_UtilStrLen(pszStr)		((NULL == pszStr) ? 0 : HxSTD_StrLen(pszStr))

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/* @brief MW AV���� Application���� �����ϴ� event list */
enum
{
	eSEVT_CAS_MW = eSEVT_CAS_START,


	/* - Description : CAS Service�� Descramble����� �������� Application���� �˷��ִ� �޽���.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : cas group id (nagra, irdeto, conax  �� ), p2 : ����, p3 : ����
	*/
	eSEVT_CAS_OK,

	/* - Description : CAS Service�� Descramble����� �������� Application���� �˷��ִ� �޽���..
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : cas group id (nagra, irdeto, conax  �� ), p2 : void pointer(�� cas ���� ���ǵ� message), p3 : show type, cas���� video, audio, �� ������ �� ���� ����.
	*/
	eSEVT_CAS_FAIL,

	/* - Description : CAS Service�� Descramble�� �����ǰ�, Action�� Delete ���� �˷��ִ� �޼���.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : ��.
	*/
	eSEVT_CAS_STOPPED,

	/* - Description : CAS Service�� Descramble�� �� Action�� ��� ���� �߻��� CAS Message�� �����ϴ� �޼��� (ex, IRD Command).
	   - Parameters Usage :
		   handle : ����.
		   p1 : cas group id (nagra, irdeto, conax  �� ), p2 : void pointer(�� cas ���� ���ǵ� message), p3 : show type, cas���� video, audio, �� ������ �� ���� ����.
	*/
	eSEVT_CAS_CMD,

	/* - Description : PPV �� Ư�� OP/CAS���� Application�� Check�� �ʿ��� Messgae.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : cas group id (nagra, irdeto, conax  �� ), p2 : void pointer(�� cas ���� ���ǵ� message), p3 : show type, cas���� video, audio, �� ������ �� ���� ����.
	*/
	eSEVT_CAS_CHECK,

	/* MMI events */
	eSEVT_CAS_MMI_OPEN = (eSEVT_CAS_START + 100),
	eSEVT_CAS_MMI_CLOSE,
	eSEVT_CAS_MMI_TEXT,
	eSEVT_CAS_MMI_ENQ,
	eSEVT_CAS_MMI_MENU,
	eSEVT_CAS_MMI_LIST,

	/* cam state event */
	eSEVT_CAS_CAM_REMOVED = (eSEVT_CAS_START + 200),
	eSEVT_CAS_CAM_INSERTED,
	eSEVT_CAS_CAM_INITIALIZED,

	/* ci plus host tune event */
	eSEVT_CAS_CAM_TUNE,

	/* ci plus */
	eSEVT_CAS_CIPLUS_CC_REPORT,
	eSEVT_CAS_CIPLUS_CC_PIN_CAPA_INFO,
	eSEVT_CAS_CIPLUS_UPGRADE,
	eSEVT_CAS_CIPLUS_REC_PAUSE,
	eSEVT_CAS_CIPLUS_REC_START,

	/* ci plus operator profile event */
	eSEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ,
	eSEVT_CAS_CIPLUS_OPERATOR_TUNE,
	eSEVT_CAS_CIPLUS_OPERATOR_NIT,

	/* ppv event */
	eSEVT_CAS_PPV_REQUEST,
	eSEVT_CAS_PPV_REPLY,

	/* pincode action event */
	eSEVT_CAS_PINCODE_REQUEST,
	eSEVT_CAS_PINCODE_REPLY,

	/* Userdata Action Event */
	eSEVT_CAS_UDATAREAD_REQUEST,
	eSEVT_CAS_UDATAREAD_REPLY,

	eSEVT_CAS_SYSINFO_UPDATED,		// P1:DxCAS_GroupId_e
									// SC Insert / EMM Update �� SC �� Library�� System ���� ������  ��ȸ�� �� �ִ� ���°� �Ǿ��� ��� : Nagra PINCODE, Irdeto National Code ��
	eSEVT_CAS_DECODE_FORCE_TUNE,				//force tune�̺�Ʈ	(p1: NW ID, p2:TS ID, p3:Svc Handle)

	eSEVT_CAS_DECODE_RESET_PIN,
	eSEVT_CAS_DECODE_RESET_IRD,
	eSEVT_CAS_DECODE_RESET_CH_ORDER,
	eSEVT_CAS_DECODE_RESCAN,
	eSEVT_CAS_DECODE_RESCAN_FROM_HOME,
	eSEVT_CAS_DECODE_RECONNECT,

	eSEVT_CAS_IR_FTA_BLOCK_CHECK,

	/* - Description : smart card event.
		           inserted�� ����Ǵ� ������ sub cas���� smart card registration�� �Ϸ� �� �� cas info�� cas manager�� ��� �� �İ� �ȴ�.
	   - Parameters Usage :
		   handle : HANDLE_NULL.
		   p1 : slot id.
	*/
	eSEVT_CAS_SC_REMOVED = (eSEVT_CAS_START + 300),
	eSEVT_CAS_SC_INSERTED,

	// ----------------------------------------
	// DRM ����� ���� CAS Message
	eSEVT_CAS_DRM_SIGNALLED = (eSEVT_CAS_START + 400),				// p1 : DrmModelType_t, p2, p3 : ���� ����� ���� ����� �Ѵ�. (���� �ʿ�)
	eSEVT_CAS_DRM_UNKNOWN,										// Ư�� Action�� CAS�� Descramble �� Session�� Start�Ͽ�����, Descramble ������ Instance�� �������� �ʾƼ�, DRM Info�� �������� ���� �� ����.
	eSEVT_CAS_DRM_NONE,											// p1 : DrmModelType_t, DRM ������ ���� ��쿡 Notify
	eSEVT_CAS_DRM_AV_BLANK,										// CAS DRM ���ۿ� ���Ͽ� AV Blanking�� control �ؾ� �ϴ� ��� ���.

	// ----------------------------------------
	// Digital MacroVision�� ���� CAS Message
	eSEVT_CAS_DMV_SIGNALLED = (eSEVT_CAS_START + 600),
	eSEVT_CAS_DMV_UNKNOWN,
	eSEVT_CAS_DMV_NONE,

	/* - Description : UI ��� CAS action�� start �Ǳ� ��, library �����κ����� update �� �����Ͱ� �ʿ��� ���, noti�� �ֱ� ����. �׷� UI ���� ����������.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : cas group id (nagra, irdeto, conax  �� ), p2 : ����., p3 : ����.
	*/
	eSEVT_CAS_NOTIFICATION,

	eSEVT_CAS_UPDATE_KEYINFO,

	eSEVT_CAS_END
}; /* Message to GWM */

typedef enum
{
	eCAS_ActionType_None,
	eCAS_ActionType_Live,
	eCAS_ActionType_Playback,
	eCAS_ActionType_Recording,
	eCAS_ActionType_Descramble_a_File,		// Descramble a file �� recording ����.
	eCAS_ActionType_Tsr_Play,
	eCAS_ActionType_Tsr_Rec,
	eCAS_ActionType_Data,
	eCAS_ActionType_EngineeringTS,			// japan arib ���� ����ϴ�, svc handle ���� ts �� �����ϴ� cas�� engineering ts
	eCAS_ActionType_RfVod,				// japan ���� ����ϴ� svc handle ���� vod �� CAS
	eCAS_ActionType_Max
} SvcCas_ActionType_e;

typedef struct
{
	EsType_t 			esType;
	HUINT16 			usPid;
} SvcCas_PidInfo_t;

typedef struct
{
	HUINT32 				ulPidNum;
	SvcCas_PidInfo_t 		stPidInfo[CAS_MAX_NUM_OF_ES];
} SvcCas_SetPidInfo_t;

typedef enum
{
	eCasChannel_Unknown,
	eCasChannel_FTA,
	eCasChannel_Scramble
} SvcCas_ChannelType_e;

typedef enum
{
	eCasShowType_None = 0,							// Audio, Video ��� �������� �ʴ´�.
	eCasShowType_AudioOnly = 1,					// Audio�� �����ش�.
	eCasShowType_VideoOnly = 2,					// Video�� �����ش�.
	eCasShowType_Both = 3,							// Audio, Video ��� �����ش�.
	eCasShowType_Max
} SvcCas_ShowType_e;

typedef struct
{
	HUINT8	aucCAMID[8];
} SvcCas_CamStartData_t;

typedef struct
{
	DxCAS_GroupId_e			eCasGroupId;
	SIxTextEncoding_e		eTextEnc;
	SIxSpec_e			 	eSiSpec;

	union
	{
		SvcCas_CamStartData_t		stCasCamStartData;
	} uData;

} SvcCas_StartData_t;

#define _____CONTROL________________________________________________________________________________________________________

typedef enum
{
	eCasCtrl_None		= 0x00,
	eCasCtrl_Control	= 0x01,
	eCasCtrl_Get		= 0x02,
	eCasCtrl_Set		= 0x04,
	eCasCtrl_Req		= 0x08,
	eCasCtrl_Noti		= 0x10,
} SvcCas_CtrlFlag_e;

typedef struct tagCAS_Ctrl_Param_t
{
	HUINT32		ulControlType;

	void		*pvIn;
	void		*pvOut;
} SvcCas_CtrlParam_t;

typedef HERROR (* SvcCas_CtrlCb_t)(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
typedef HERROR (* SvcCas_GetCb_t)(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
typedef HERROR (* SvcCas_SetCb_t)(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
typedef HERROR (* SvcCas_ReqCb_t)(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
typedef HERROR (* SvcCas_NotiCb_t)(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);

typedef struct tagCtrl
{
	HUINT32				ulControlType;
	SvcCas_CtrlFlag_e	eCtrlFlag;
	SvcCas_CtrlCb_t		pfnCtrlFunc;
	SvcCas_GetCb_t		pfnGetFunc;
	SvcCas_SetCb_t		pfnSetFunc;
	SvcCas_ReqCb_t		pfnReqFunc;
	SvcCas_NotiCb_t		pfnNotiFunc;
} SvcCas_Ctrl_t;

typedef struct
{
	HUINT32 ulMsg;
	HUINT8 *pvData;
} SvcCas_SmartcardReplyData_t;

#define _____SI________________________________________________________________________________________________________


typedef enum
{
	eCASSI_Table_Skipped = 0,
	eCASSI_Table_Waiting,
	eCASSI_Table_Received,
	eCASSI_Table_Timeout,
	eCASSI_Table_Running,
	eCASSI_Table_Fail
} SvcCas_SiTableState_e;

typedef struct
{
	HUINT16 					usCaSysId;				/* CA system ID							*/
	HUINT16 					usCaPid;				/* CA PID								*/
	HUINT8 						*pucRaw;				/* pointer to raw descriptor byte-data	*/
	HUINT8 						ucRawLen;				/* length of raw descriptor byte-data	*/
} SvcCas_CaDescriptor_t;

typedef struct
{
	HUINT8						ucStreamType;			// MPEG���� ������ Stream Type (see ISO_VIDEO_1 ...)
	EsType_t					eEsType;				// Humax���� �����ϴ� Es�� Type
	HUINT16 					usPid;					// Element stream�� Pid
	HUINT8						bIsCurrent;
	HBOOL						bElementCaFlag;			// ES Level�� Ca Descriptor �� �����ϴ� ��.
	HUINT32						ulNumOfCaDescriptor;
	SvcCas_CaDescriptor_t		stElementCaDescriptor[CAS_MAX_NUM_OF_CAPID];
} SvcCas_PmtElement_t;

typedef struct
{
	HUINT32						ulSectionState;

	/* Program Information */
	HUINT16						usServiceId;
	HUINT16						usPcrPid;
	HUINT8						ucVerNum;
	HUINT8						bCurNextInd;
	HUINT32						ulCrc;

	/* Global CA Descriptors */
	HBOOL						bProgramCaFlag;
	HUINT32						ulNumOfProgramCaDescriptor;
	SvcCas_CaDescriptor_t		stProgramCaDescriptor[CAS_MAX_NUM_OF_CAPID];

	/* Element Information */
	HUINT32						ulNumOfElement;
	SvcCas_PmtElement_t			stElementInfo[CAS_MAX_NUM_OF_ES];
} SvcCas_PmtInfo_t;

typedef HERROR (* SvcCas_SiGetTimeOutCb_t)(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout);
typedef void (* SvcCas_SiParseCb_t)(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
typedef void (* SvcCas_SiFreeCb_t)(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);

#define _____CI________________________________________________________________________________________________________


typedef enum
{
	eCasInst_None,
	eCasInst_Embedded,
	eCasInst_CI,
	eCasInst_Max
} SvcCas_InstanceType_e;


#define _____MMI________________________________________________________________________________________________________

// MMI ����.
typedef enum
{
	eCasMmiType_None,
	eCasMmiType_Open,
	eCasMmiType_Close,
	eCasMmiType_Menu,
	eCasMmiType_List,
	eCasMmiType_Text,
	eCasMmiType_Enq,
	eCasMmiType_Max
} SvcCas_MmiDataType_e;

typedef struct
{
	HUINT32						 ulVersion;
	SvcCas_InstanceType_e		 eCasInstType;
	HUINT16						 usSlotId;
	Handle_t					 hMmi;
	SvcCas_MmiDataType_e		 eDataType;
	void						*pvMmiData;
	HUINT32						 ulMmiDataSize;
	HUINT8						 ucAnswer[MAX_CI_STR_LEN + 1];
} SvcCas_MmiData_t;

typedef struct
{
	HUINT8							ucBlindOnOff;		/* ����� �Է��� ȭ�� ǥ�� ��� */
	HUINT8							ucAnswerLen;		/* ���ǿ� �����ؾ� �� ��Ʈ�� ���� */
	HUINT8							*pszPrompt;			/* �����ϴ� ��Ʈ�� */
} SvcCas_MmiEnq_t;

typedef struct
{
	HUINT8							ucNumOfItem;		/* �������� ���� */
	HUINT8							*pszTitle;			/* Ÿ��Ʋ ������ ǥ�õ� ��Ʈ�� */
	HUINT8							*pszSubTitle;		/* ����Ÿ��Ʋ ������ ǥ�õ� ��Ʈ�� */
	HUINT8							*pszBottom;			/* �ϴ� ������ ǥ�õ� ��Ʈ�� */
	HUINT8							**ppszItem;			/* �� �������� ��Ʈ�� */
} SvcCas_MmiMenuList_t;

typedef enum
{
	eCasMmiCmd_Open,
	eCasMmiCmd_Close,
	eCasMmiCmd_Answer,
	eCasMmiCmd_Select,
	eCasMmiCmd_Max
} SvcCas_MmiCmd_e;

//#if defined(CONFIG_MW_CI_PLUS)

// pfnNotifyCcReport (for CC status or error reporting) : UI ó�� �� CiCc_DoActionForArcErr()���� ó���Ǵ� �� �ܿ� �ʿ��� �׼��� ���� ---
typedef enum
{
	// ���̼��� �����Ͱ� �Ѽյ� ��� (ġ���� ����) -> "Can't load CI+ host license constants !" ǥ���� ��
	eCiPlus_CcReport_CanNotLoadHostLicenseConstants = 1,

	// ���̼��� �����Ͱ� ������ �ȵ� ��� (ġ���� ����) -> "CI+ host license constants not saved !" ǥ���� ��
	eCiPlus_CcReport_HostLicenseConstantsNotSaved,

	// ���ε��� ���� ���� CI+ ķ�� ���ԵǾ� ���ε� ���� ���۽� (���� ǥ��) -> "Virgin CI+ CAM inserted. Authentication in progress. Please wait 1~2 minutes." ǥ���� ��
	eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress,

	// ķ�� ��Ʈ ���� ���ε� �Ϸ� (���� ǥ��) -> "Authentication success" ǥ���� �� (�Ǵ� eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress ���� �ݰų�...)
	eCiPlus_CcReport_AuthenticationSuccess, // ȣ��Ʈ ����Ű�� ������� ���� ����.

	// ķ�� ��Ʈ ���� ��ȣ ���� �Ϸ� (CAS/CI mgr ���� ����ϱ� ���� �뵵)
	eCiPlus_CcReport_AuthenticationDoneAndStartSacEstablishment, // SAC Establishment ���õ� ����.

	// ķ�� ��Ʈ ���� URI Version üũ �Ϸ� (CAS/CI mgr ���� ����ϱ� ���� �뵵)
	eCiPlus_CcReport_UriVersionNegotiationDone, // URI Version Negotiation �� ���� ����.

	// ķ�� ��Ʈ ���� ���� ���� (����) -> "Authentication failed (error %d) !" ǥ���� �� (���⼭ %d �� enArcErr ��)
	// ��, enArcErr ���� CI_ARC_ERR_CAM_Cert_Expired �� ��쿡�� "Authentication failed. Please check the time is correct on set and re-insert your CAM !" ǥ���� ��
	eCiPlus_CcReport_AuthenticationFailed,

	// ķ�� ��Ʈ ���� ������ ���� �Ǵ� ��Ʈ�� ���ۻ� ���� (����) -> "CI+ operation failed (error %d) !" ǥ���� �� (���⼭ %d �� enArcErr ��)
	eCiPlus_CcReport_OperationFailed,

	// CC �������� �󿡼� ķ�� �������� �ʴ� ��� (����) -> "No response from CI+ CAM (error %d) !" ǥ���� �� (���⼭ %d �� enArcErr ��)
	eCiPlus_CcReport_NoResponseFromCam,

//#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	// ���� ���Ե� CAM�� CICAM PIN�� cache�� �� �־ ����ڷκ��� PIN�� �޴� ������ �����ϴ� UI ���
	// -> "To be able to record the pay-TV programmes, enter the CA PIN code. This CA PIN code will be saved in your set-top box" ǥ�� �� �� ( PIN code �Է� â )
	eCiPlus_CcReport_RequireCicamPin,
	// ����ڷκ��� �߸��� CICAM PIN�� �Է� �޾Ұ�, ���̻� PIN�Է� ���� ���� �� -> "You have entered the wrong CA PIN code three times. Please try again later." ǥ�� �� �� ( banner�� ǥ��)
	eCiPlus_CcReport_PinError,
	// ����ڷκ��� �߸��� CICAM PIN�� �Է� �޾��� �� PIN code�� �ٽ� �Է¹޴� ��� -> "You have entered the wrong CA PIN code. Please try again." ǥ�� �� �� ( with PIN code �Է� â )
	eCiPlus_CcReport_PinErrorRetry,
	// ����ڷκ��� �Է¹��� PIN�� verify �� ��� -> "The CA PIN code is saved." ǥ�� �� �� ( banner�� ǥ��)
	eCiPlus_CcReport_PinVarified,
	// ����ڷκ��� �Է¹��� PIN�� verify ���� ���� ��� -> "The CA PIN code is saved, but the entered CA PIN code has not been verified." ǥ�� �� �� ( Box�� ǥ��)
	eCiPlus_CcReport_PinNotVarified,
//#endif
	eCiPlus_CcReport_Max
} SvcCas_CiPlusCcReportMode_e;

typedef enum
{
	CI_ARC_ERR_Get_Instance = -16,

	CI_ARC_ERR_Apdu_Tag = -15,
	CI_ARC_ERR_Apdu_Len1 = -14,
	CI_ARC_ERR_Apdu_Len2 = -13,
	CI_ARC_ERR_Send_Apdu = -12,

	CI_ARC_ERR_SAC_MSG_Counter = -11,
	CI_ARC_ERR_SAC_MSG_Header1 = -10,
	CI_ARC_ERR_SAC_MSG_Header2 = -9,
	CI_ARC_ERR_SAC_MSG_PayloadLen = -8,
	CI_ARC_ERR_SAC_MSG_Authentication = -7,

	CI_ARC_ERR_Send_data_nbr = -6,
	CI_ARC_ERR_Request_data_nbr = -5,

	CI_ARC_ERR_uri_message = -4,
	CI_ARC_ERR_program_number = -3,

	CI_ARC_ERR_Root_Cert = -2,
	CI_ARC_ERR_Not_Defined = -1,

	// ���� ������ CI+ ���忡 ���ǵǾ� �ִ� ������ enum ���� �����ϰų� ������ �׸��� �߰����� �� �� !!

	CI_ARC_ERR_None = 0,						//   0 detect : None		Host action : None

	CI_ARC_ERR_Module_Revoked,					//   1 detect : CAM			Host action : None
	CI_ARC_ERR_Host_Revoked,					//   2 detect : CAM			Host action : None

	CI_ARC_ERR_SAC_Failed,						//   3 detect : CAM/Host	Host action : Host stops the CICAM -> None (CI+ v1.3)
	CI_ARC_ERR_CCK_Failed,						//   4 detect : CAM/Host	Host action : Host stops the CICAM -> None (CI+ v1.3)

	CI_ARC_ERR_CAM_Upgrade_Bootloader_Failed,	//   5 detect : CAM			Host action : None
	CI_ARC_ERR_CAM_Upgrade_Location_Failed,		//   6 detect : CAM			Host action : None
	CI_ARC_ERR_CAM_Upgrade_Signature_Failed,	//   7 detect : CAM			Host action : None

	CI_ARC_ERR_Auth_Retries_Exhausted,			//   8 detect : CAM			Host action : None -> CICAM goes to pass-through mode when Authentication Failed -> None (CI+ v1.3)
	CI_ARC_ERR_Auth_Signature_Failed,			//   9 detect : CAM/Host	Host action : Host stops the CICAM
	CI_ARC_ERR_Auth_Key_Verification_Failed,	// 10 detect : CAM/Host		Host action : Host stops the CICAM
	CI_ARC_ERR_Auth_Key_Computation_Failed,		// 11 detect : CAM/Host		Host action : Host stops the CICAM
	CI_ARC_ERR_Auth_DH_Failed,					// 12 detect : CAM/Host		Host action : Host stops the CICAM

	CI_ARC_ERR_CAM_Cert_Syntax_Incorrect,		// 13 detect : Host			Host action : Host stops the CICAM
	CI_ARC_ERR_CAM_Cert_Expired,				// 14 detect : Host			Host action : Host goes to DVB-CI mode (The host behaves like a DVB-CI compliant host)
	CI_ARC_ERR_CAM_Cert_Signature_Failed,		// 15 detect : Host			Host action : Host stops the CICAM

	CI_ARC_ERR_Host_Cert_Syntax_Incorrect,		// 16 detect : CAM			Host action : None
	CI_ARC_ERR_Host_Cert_Expired,				// 17 detect : CAM			Host action : None
	CI_ARC_ERR_Host_Cert_Signature_Failed,		// 18 detect : CAM			Host action : None

	CI_ARC_ERR_OP_Cert_Syntax_Incorrect,		// 19 detect : CAM			Host action : None
	CI_ARC_ERR_OP_Cert_Expired,					// 20 detect : CAM			Host action : None
	CI_ARC_ERR_OP_Cert_Signature_Failed,		// 21 detect : CAM			Host action : None

	CI_ARC_ERR_CAM_Requires_Update,				// 22 detect : CAM			Host action : None
	CI_ARC_ERR_Reserved_for_CI_Plus = 23,		// 23 detect : CAM			Host action : None
	CI_ARC_ERR_Private_Use_for_OP = 128			//128 detect : CAM			Host action : None
} SvcCas_CiArcErr_e;

typedef struct
{
	SvcCas_CiPlusCcReportMode_e 	eCcReportType;
	SvcCas_CiArcErr_e 				enArcErrType;
	HUINT32							ulVer;
} SvcCas_CcReport_t;

typedef struct
{
	unsigned int   bSavedFlag;					// 0 : not saved (���� ���� ��ȿ���� ����), 1 : saved
} SvcCas_CiAuthInfo_t;

typedef enum
{
	eCiPlus_CamUpgrade_Start,				// CAM firmware upgrade ���� ���� ó�� (����μ� ó�� ���� ����)
	eCiPlus_CamUpgrade_Processing,			// CAM firmware upgrade ���� ���� ó�� (������� UI�� ǥ���ϰ� user interaction �� ���� ��)
	eCiPlus_CamUpgrade_DoneOrCancelled,		// CAM firmware upgrade ���� �Ϸ� ó�� (UI�� ǥ���ϰ� ������ user interaction �� Ǯ�� ��Ȳ�� ���� ķ ������ ��)
	eCiPlus_CamUpgrade_Failed,				// CAM firmware upgrade ���� ���� ó�� (UI�� ǥ���ϰ� ķ ������ ��)
	eCiPlus_CamUpgrade_Broken				// CAM firmware upgrade ���� ��� ó�� (������ ���� ķ�� ���ŵ� ��Ȳ�� ���� ó��)
} SvcCas_CiPlusCamUpgradeMode_e;

typedef enum
{
	eCiPlus_CamUpgradeReset_PCMCIA		= 0x00,		/* set RESET signal */ /* �ϴ� �� ��� ��� svc_cas_DevCiResetModule()�� ȣ���ϵ��� �Ѵ�... */
	eCiPlus_CamUpgradeReset_CAM			= 0x01,		/* set RS flag */
	eCiPlus_CamUpgradeReset_NO_RESET	= 0x02
} SvcCas_CiPlusCamUpgradeResetMode_e;		// CI_CU_RESET_REQUEST ���� ���� : �� ������ CI+ ���忡 ���ǵǾ� �ִ� ������ enum ���� �����ϰų� ������ �׸��� �߰����� �� �� !!

typedef struct
{
	SvcCas_CiPlusCamUpgradeMode_e 		eCamUpgradeMode;
	unsigned char 						ucDownloadProgressInPercentage; /* �����(%) */
	SvcCas_CiPlusCamUpgradeResetMode_e 	eCamUpgradeResetMode;
	HUINT32								ulVer;
} SvcCas_CiPlusUpgrade_t;

// TODO: SvcCas_CiPlusPinCapability_e �� SvcCas_CiPlusPinCapaInfo_t ����.
typedef enum
{
	eCiPlus_PinCapability_NONE = 0x00,						// CICAM has no capability to handle PIN
	eCiPlus_PinCapability_ONLY_CAS = 0x01,				// CICAM only handles CAS controlled content PIN
	eCiPlus_PinCapability_BOTH = 0x02,						// CICAM can handle both CAS controlled content PIN and non CAS controlled content PIN
	eCiPlus_PinCapability_ONLY_CAS_WITH_CACHE = 0x03,	// CICAM only handles CAS controlled content PIN (with CICAM cached PINs)
	eCiPlus_PinCapability_BOTH_WITH_CACHE = 0x04		// CICAM can handle both CAS controlled content PIN and non CAS controlled content PIN (with CICAM cached PINs)
} SvcCas_CiPlusPinCapability_e;

typedef struct
{
	// "capability_field"
	SvcCas_CiPlusPinCapability_e	enPinCapa;

	// "pin_change_time_utc"
	// the time when the CICAM PIN was last changed.
	// This is a 40-bit field which specifies the date and time in MJD and UTC when the PIN was last changed (See start_time field of the EIT in EN 300 468).
	// This field is encoded as 40-bits giving the 16 LSBs MJD followed by 24-bits coded as 6 digits in 4-bit BCD.
	// This field shall be specified as zero if the PIN is not handled or when it has never been changed.
	// The Host may use the this to warn the end-user that any unattended recording programmed may fail when it was programmed before and scheduled after the time indicated by the pin_change_time_utc field.
	unsigned char			ucPinChangeTimeUtc[5];

	// "rating"
	// This 8-bit field is coded as DVB rating (3+years).
	// Rating is defined in EN 300 468 parental rating descriptor.
	// This is the current rating as set in the CICAM.
	// This field allows the Host to exert parental control when the Host rating is set at a lower level than the CICAM rating.
	// The Host may use the cc_PIN_MMI_req() APDU for this purpose depending on the CICAM PIN capabilities.
	// The CICAM shall not request a PIN entry for an age rating less than this value.
	unsigned char			ucRating;

	// bPinCapaSet�� capability_reply�� �޴� ������ �ƴ�����, ���� �ش� CAM�� ���Ͽ� Pin Capability �� setting �� �������� �ľ��ϱ� ���� �߰���.
	HBOOL					bPinCapaSet;
} SvcCas_CiPlusPinCapaInfo_t;


/*-----------------------------------------------------------------------------------------------------------*/
/*  CAS Tune ���� - ����� CI Tune���� ���������, CAS Tune ������ �ʿ��� ��쿡�� �� �κ��� Ȯ���Ͽ� ���   */
/*-----------------------------------------------------------------------------------------------------------*/

typedef enum
{
	eCasTuneMode_Start_v1_TuneByTripleId,		// HCI v1 �� ���� tune (��, CI_TUNE �� ����)
	eCasTuneMode_Start_v2_TuneByTripleId,		// HCI v2 �� ���� tune (��, CI_TUNE_BROADCAST_REQ �� ����), CI+ v1.3
	eCasTuneMode_Start_v2_TuneByLocation,		// HCI v2 �� ���� tune
	eCasTuneMode_Cancel
} SvcCas_TuneMode_e;

typedef struct
{
	HUINT32						ulFrequency;
	HUINT32						ulSymbolRate;
	DxSat_Polarization_e		ePolarization;
	DxSat_CodeRate_e			eFecCodeRate;

	/** @brief DVB-S2 modulation system's parameters */
	DxSat_TransportSpec_e		eTransSpec;
	DxSat_PskModulation_e		ePskMod;
	DxSat_Pilot_e				ePilot;
	DxSat_RollOff_e				eRollOff;
} SvcCas_SatTuningInfo_t;

typedef struct
{
	/* not properly defined at this time */
	HUINT32						ulFreq;
	HUINT16						usSR;
	HUINT16						usNetworkId;
	DxCab_Constellation_e		ucCon;
	DxCab_Spectrum_e			ucSpec;
} SvcCas_CabTuningInfo_t;

#if defined(CONFIG_MW_CH_DVB_T2)
typedef struct
{
	HUINT32						ulPlpId;
	DxTer_PreambleFormat_e		ucPreambleFormat;
	DxTer_Mixed_e				ucMixed;
	DxTer_Pilot_Pattern_e		ucPilotPattern;
	DxTer_ExtendedCarrier_e		ucExtenedCarrier;
	DxTer_PAPR_Reduction_e		ucPAPRreduction;
	HUINT32						ulNumPlpBlocks;
} SvcCas_TerT2TuningInfo_t;
#endif

typedef struct
{
	HUINT32						ulFreq;				/**< centre_frequency (mutiples of 10Hz) */
	DxTer_BandWidth_e			ucBandwidth;		/**< bandwidth     : 8/7/6 MHz */
	DxTer_Constellation_e		ucConstellation;	/**< constellation : QPSK/16-QAM/64-QAM */
	DxTer_Hierachy_e			ucHierachy;			/**< hierachy information : signalling format for the alpha values */
	DxTer_Stream_e				ucStream;			/**< HP/LP */
	DxTer_CodeRate_e			ucCodeRate;			/**< FEC */
	DxTer_GuardInterval_e		ucGuardInterval;	/**< 1/32, 1/16, 1/8, 1/4 */
	DxTer_TransMode_e			ucTransMode;		/**< 2k/8k */
	DxTer_Offset_e				ucOffset;
#if defined(CONFIG_MW_CH_DVB_T2)
	DxTer_System_e				ucSystem;			/**< T/T2 */
	DbSvc_TerT2TuningInfo_t		stT2param;
#endif
} SvcCas_TerTuningInfo_t;

typedef union
{
	SvcCas_SatTuningInfo_t		sat;
	SvcCas_CabTuningInfo_t		cab;
	SvcCas_TerTuningInfo_t		ter;
} SvcCas_TuningDesInfo_t;

typedef struct
{
	HUINT8					ucstream_type;
	HUINT16					usElePid;
	SvcCas_CaDescriptor_t	stElementCaDescriptor[CAS_MAX_NUM_OF_CAPID];
} SvcCas_ElementaryInfo_t;

#define MAX_ES_NUM_FOR_TUNING 5

typedef struct tagCASPMT_INFO
{
	HUINT16					usServiceId;						/** < Service ID */
	HUINT16					usPcrPid;							/** < PCR PID */

	HUINT16					usEleNum;							/** < number of VIDEO */
	SvcCas_ElementaryInfo_t	stEleInfo[MAX_ES_NUM_FOR_TUNING];
} SvcCas_PmtInfoForTune_t;

typedef struct
{
	SvcCas_TuneMode_e eMode;

	union
	{
		struct
		{
			// CI_TUNE �� ���� ����
			// eMode �� eCasTuneMode_Start_v1_TuneByTripleId & eCasTuneMode_Start_v2_TuneByTripleId �� �� ���ȴ�.
			// HCI v2�� ��쿡 tuning ����� MW_CAS_TuneReply() �� ���� CAM �� �˷��־�� �Ѵ�.
			HUINT16	usNetId;
			HUINT16	usOnId;
			HUINT16	usTsId;
			HUINT16	usSvcId;
		} stTripleId;

		struct
		{
			// CI_TUNE_BROADCAST_REQ �� ���� ���� (CI+ v1.3)
			// eMode �� eCasTuneMode_Start_v2_TuneByLocation �� �� ���ȴ�.
			// tuning ����� MW_CAS_TuneReply() �� ���� CAM �� �˷��־�� �Ѵ�.
			// ���� �����͵��� OxCAS_Malloc() �Ǿ� ���޵Ǵ� ������ �޴� �ʿ��� �ݵ�� OxCAS_Free() ����� �Ѵ�.
			DxDeliveryType_e  			eDeliType;
			HBOOL						bPmtFlag;
			HUINT16						usServiceId; // usServiceId �� 0 �� ��� bPmtFlag �� FALSE �̰� pucDescriptors �� �����Ͽ� �ش� TP �� tuning �� �Ѵ� (�� ��� ca_pmt ������ ����). usServiceId �� 0 �� �ƴϸ� PMT ���� program_number �� �����ϴ�.
			HUINT16						usDescriptorsLen;

			SvcCas_TuningDesInfo_t		stTuningDesInfo;
			SvcCas_PmtInfoForTune_t		stPmtInfo;
		} stBroadcastReq;
	} utInfo;
} SvcCas_TuneInfo_t;

typedef struct
{
	DxDeliveryType_e  		 eDeliType;
	SvcCas_TuningDesInfo_t	 stTuningDesInfo;
	HUINT8					*pucDes;
	HUINT8					 ucDesLen;
	HUINT8					 ucNextDesNum;
} SvcCas_OperatorTune_t;

typedef struct
{
	HUINT16		usNetId;
	HUINT8		szNetName[256];
} SvcCas_OperatorNit_t;

typedef struct
{
	HUINT16					usTsId;
	HUINT16					usOnId;
	DxDeliveryType_e		eDeliType;
	SvcCas_TuningDesInfo_t	stTuningDes;
} SvcCas_OperatorNitTs_t;

typedef struct
{
	HUINT16		usSvcId;
	HUINT8		ucSvcType;
	HBOOL		bVisible;
	HBOOL		bSelectable;
	HUINT16		usLcn;
	HUINT8		szSvcPrvName[256];
	HUINT8		szSvcName[256];
} SvcCas_OperatorSvc_t;


/*--------------------------------------*/
/*  Usage Rules Information (URI) ����  */
/*--------------------------------------*/

#define CI_CC_URI_MODE_DEFAULT		1
#define CI_CC_URI_MODE_TIMEOUT		2
#define CI_CC_URI_MODE_FROMCAM		3

// Analogue Protection System (APS) for Macrovision on/off : 2 bits
#define CI_CC_URI_APS_OFF			0x0					// Copy Protection Encoding Off
#define CI_CC_URI_APS_ON_1			0x1					// AGC Process On, Split Burst Off
#define CI_CC_URI_APS_ON_2			0x2					// AGC Process On, 2 line Split Burst On
#define CI_CC_URI_APS_ON_3			0x3					// AGC Process On, 4 line Split Burst On
#define CI_CC_URI_APS_DEFAULT		CI_CC_URI_APS_OFF

// Encryption Mode Indicator (EMI) or Copy Control Information (CCI) : 2 bits
#define CI_CC_URI_EMI_COPY_FREE		0x0					// Copying not restricted
#define CI_CC_URI_EMI_COPY_NO_MORE	0x1					// No further copying is permitted
#define CI_CC_URI_EMI_COPY_ONCE		0x2					// One generation copy is permitted
#define CI_CC_URI_EMI_COPY_NEVER	0x3					// Copying is prohibited
#define CI_CC_URI_EMI_DEFAULT		CI_CC_URI_EMI_COPY_NEVER

// Image Constrained Trigger (ICT) for HD downscale on/off : 1 bit
#define CI_CC_URI_ICT_OFF			0x0					// No Image Constraint asserted
#define CI_CC_URI_ICT_ON			0x1					// Image Constraint required
#define CI_CC_URI_ICT_DEFAULT		CI_CC_URI_ICT_OFF
#define CI_CC_URI_ICT_TIMEOUT		CI_CC_URI_ICT_ON	// CI_CC_URI_TIMEOUT �� ICT ���밪

// Redistribution Control Trigger (RCT) for Encryption Plus Non-assert : 1 bit (for EMI ���� �߰� �����)
#define CI_CC_URI_RCT_OFF			0x0					// No Redistribution Control asserted. Default.
#define CI_CC_URI_RCT_ON			0x1					// Redistribution Control asserted
#define CI_CC_URI_RCT_DEFAULT		CI_CC_URI_RCT_OFF

// Retention Limit (RL) : 6 bits (0x00, 0x01, 0x02 or 0x03..0x3F for EMI ���� �߰� �����)
#define CI_CC_URI_RL_DEFAULT		0x00				// Default retention limit of 90 minutes applies

//#endif

// TODO: ��ġ �̵�.

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	SVC_CAS_Init(PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData);
HERROR	SVC_CAS_SiRegisterTimeoutCallback(SvcCas_SiGetTimeOutCb_t fnSiTimeout);
HERROR	SVC_CAS_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActionType, SvcCas_StartData_t *pstCasStartData);
HERROR	SVC_CAS_StopService(Handle_t		hAction,
							Handle_t		hNextSvc,		/**< [in] ������ start�� service handle ����. ����� ���ٸ� demux filter�� ������.		HANDLE_NULL(-1 or 0xffffffff)�̸� ���������� ������ stop. */
							ApiSyncMode_t	ulSyncMode
							);

HERROR	SVC_CAS_SetRecPid(Handle_t hAction, SvcCas_ActionType_e eActType, SvcCas_SetPidInfo_t *pstCaDscInfo);
HERROR	SVC_CAS_SetDataPid(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo);
HERROR	SVC_CAS_SetPid(	Handle_t			hAction,
						SvcCas_ActionType_e	eActType,
						HUINT16				usVideoPid,
						HUINT16				usAudioPid,
						HUINT16				usAuxPid,
						HUINT16				usDolbyPid,
						HUINT16				usTeletext_Super_Pid,
						HUINT16				usSubtitle_CC_Pid);

HERROR	SVC_CAS_Pause(Handle_t hAction);
HERROR	SVC_CAS_Resume(Handle_t hAction);

// CI
#ifdef CONFIG_MW_CI
HERROR	SVC_CAS_GetNumOfCISlot(HUINT32 *ulMaxSlot);
HERROR	SVC_CAS_GetMaxNumOfCISlot(HUINT32 *ulMaxSlot); // OCTO�� ������ �� �ִ� �ִ� CI Slot ����
HERROR	SVC_CAS_GetCamState(HUINT16 usSlotId, HUINT32 *pCamState);
HERROR	SVC_CAS_GetCamName(HUINT16 usSlotId, HUINT8 **ppszCamInfo);
HERROR	SVC_CAS_CiGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName);
#endif

HERROR 	SVC_CAS_GetUiData(HUINT8 *pucUiData, DxCAS_UiDataType_e eType);


//#if defined(CONFIG_MW_CAS) // CONFIG_MW_CAS_NAGRA, CONFIG_MW_CAS_IRDETO, CONFIG_MW_CAS_CONAX ...
HERROR	SVC_CAS_GetNumOfSCSlot(HUINT32 *ulMaxSlot);
HBOOL	SVC_CAS_CheckSmartCardValidity(DxCAS_GroupId_e eCasId);
HERROR	SVC_CAS_GetSCCasSysIdList (HUINT32 *pulNumCaSysId, HUINT16 **ppusCaSysIdList);
HERROR	SVC_CAS_FreeSCCasSysIdList (HUINT16 *pusCaSysIdList);
//#endif

HERROR	SVC_CAS_GetChannelType(Handle_t hAction, SvcCas_ChannelType_e *pType);
HERROR	SVC_CAS_PrintCaSysId(Handle_t hAction);


/* Debug Function */
void	SVC_CAS_PrintInfoAll(void);
void	SVC_CAS_PrintCaDeviceInfo(void);
void	SVC_CAS_PrintCaInstTableById(HUINT32 ulCaInstanceId);
void	SVC_CAS_PrintContextList(void);
void	SVC_CAS_PrintContextByActionId(HUINT32 ulActionId);

void	SVC_CAS_PrintCaInstTable(void);

void	SVC_CAS_PrintCatInfo(HUINT32 ulActionId);
void	SVC_CAS_PrintPmtInfo(HUINT32 ulActionId);

#ifdef CONFIG_MW_CI
void	SVC_CAS_PrintMmiStatus(void);
void	SVC_CAS_PrintMmiList(HUINT16 usSlotId);
#endif

void	SVC_CAS_PrintRawTable(void);

#ifdef CONFIG_MW_CI_PLUS
HERROR	SVC_CAS_GetHostId(HUINT8 *pucHost);
HERROR	SVC_CAS_InitAuthContexts(HUINT16 usSlotId);

HERROR	SVC_CAS_GetCcReport(HUINT16 usSlotId, HUINT32 ulEvtVersion, SvcCas_CcReport_t **pData);
HERROR	SVC_CAS_GetUpdateStatus(HUINT16 usSlotId, HUINT32 ulEvtVersion, SvcCas_CiPlusUpgrade_t **pData);

HERROR	SVC_CAS_ChangeHostCountrySetting(void);
HERROR	SVC_CAS_ChangeHostLanguageSetting(void);
#endif	// #ifdef CONFIG_MW_CI_PLUS

HERROR	SVC_CAS_CtrlCall(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
HERROR	SVC_CAS_CtrlGet(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
HERROR	SVC_CAS_CtrlSet(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
HERROR	SVC_CAS_CtrlReq(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
HERROR	SVC_CAS_CtrlNoti(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);

#if defined(CONFIG_DEBUG)
void	SVC_CAS_CmdInit(HxCMD_Handle_t hCmdHandle);
#endif

#endif /* !__SVC_CAS_H__ */
