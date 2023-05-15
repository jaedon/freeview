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
#define CAS_MAX_NUM_OF_CAPID		64	/* descriptor 개수 */
#define CAS_MAX_NUM_OF_DSC		16	// see CAS_MAX_NUM_OF_ES / MAX_NUM_OF_DSC_ID / MAX_DESC_CH_NUM (DI_DSC_SLOTNUM_MAXIMUM) !! -> org is 20 : 브로드컴 제약 사항으로 10개까지의 디스크램블러만 운용될 수 있다. 추후 플로우 수정도 필요함.

#if defined (CONFIG_MW_MM_PVR)
#define CAS_MAX_NUM_OF_TS			4	// CAS MGR에서 처리 가능한 TS 개수....CAT, SDT, NIT
#else
#define CAS_MAX_NUM_OF_TS			1	// CAS MGR에서 처리 가능한 TS 개수....CAT, SDT, NIT
#endif


// 사양 관리.
#define CAS_MAX_NUM_OF_ACTION	NUM_MAX_ACTION								// octo_config에 정의되어 있음.

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
#define CAS_MAX_NUM_OF_ECAS		CONFIG_MW_CAS_NUM_EMBEDDED				// Porting 되는 EMBEDDED CAS의 개수, PAL_MAX_SMCSLOT_NUMBER이 아니다.
#else
#define CAS_MAX_NUM_OF_ECAS		0 											// Porting 되는 EMBEDDED CAS의 개수, PAL_MAX_SMCSLOT_NUMBER이 아니다.
#endif

#define CAS_MAX_NUM_OF_INSTANCE	(CAS_MAX_NUM_OF_CAM + CAS_MAX_NUM_OF_ECAS)

#define MAX_CI_STR_LEN			40	// CI 및 CI+ 스펙에 따라 최소한 The host shall be able to display 40 characters and 5 lines in addition of title, subtitle and bottom line.

#define	SvcCas_UtilStrLen(pszStr)		((NULL == pszStr) ? 0 : HxSTD_StrLen(pszStr))

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/* @brief MW AV에서 Application으로 전송하는 event list */
enum
{
	eSEVT_CAS_MW = eSEVT_CAS_START,


	/* - Description : CAS Service에 Descramble결과가 성공됨을 Application에게 알려주는 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : cas group id (nagra, irdeto, conax  등 ), p2 : 없음, p3 : 없음
	*/
	eSEVT_CAS_OK,

	/* - Description : CAS Service에 Descramble결과가 실패임을 Application에게 알려주는 메시지..
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : cas group id (nagra, irdeto, conax  등 ), p2 : void pointer(각 cas 별로 정의된 message), p3 : show type, cas에서 video, audio, 등 보여줄 지 여부 선택.
	*/
	eSEVT_CAS_FAIL,

	/* - Description : CAS Service에 Descramble이 중지되고, Action이 Delete 됨을 알려주는 메세지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : 무.
	*/
	eSEVT_CAS_STOPPED,

	/* - Description : CAS Service에 Descramble이 중 Action에 상관 없이 발생한 CAS Message를 전달하는 메세지 (ex, IRD Command).
	   - Parameters Usage :
		   handle : 없음.
		   p1 : cas group id (nagra, irdeto, conax  등 ), p2 : void pointer(각 cas 별로 정의된 message), p3 : show type, cas에서 video, audio, 등 보여줄 지 여부 선택.
	*/
	eSEVT_CAS_CMD,

	/* - Description : PPV 및 특정 OP/CAS에서 Application의 Check가 필요한 Messgae.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : cas group id (nagra, irdeto, conax  등 ), p2 : void pointer(각 cas 별로 정의된 message), p3 : show type, cas에서 video, audio, 등 보여줄 지 여부 선택.
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
									// SC Insert / EMM Update 후 SC 및 Library의 System 관련 정보를  조회할 수 있는 상태가 되었을 경우 : Nagra PINCODE, Irdeto National Code 등
	eSEVT_CAS_DECODE_FORCE_TUNE,				//force tune이벤트	(p1: NW ID, p2:TS ID, p3:Svc Handle)

	eSEVT_CAS_DECODE_RESET_PIN,
	eSEVT_CAS_DECODE_RESET_IRD,
	eSEVT_CAS_DECODE_RESET_CH_ORDER,
	eSEVT_CAS_DECODE_RESCAN,
	eSEVT_CAS_DECODE_RESCAN_FROM_HOME,
	eSEVT_CAS_DECODE_RECONNECT,

	eSEVT_CAS_IR_FTA_BLOCK_CHECK,

	/* - Description : smart card event.
		           inserted가 발행되는 시점은 sub cas에서 smart card registration이 완료 된 후 cas info를 cas manager에 등록 한 후가 된다.
	   - Parameters Usage :
		   handle : HANDLE_NULL.
		   p1 : slot id.
	*/
	eSEVT_CAS_SC_REMOVED = (eSEVT_CAS_START + 300),
	eSEVT_CAS_SC_INSERTED,

	// ----------------------------------------
	// DRM 명령을 위한 CAS Message
	eSEVT_CAS_DRM_SIGNALLED = (eSEVT_CAS_START + 400),				// p1 : DrmModelType_t, p2, p3 : 각자 사양대로 가져 가기로 한다. (논의 필요)
	eSEVT_CAS_DRM_UNKNOWN,										// 특정 Action이 CAS에 Descramble 할 Session을 Start하였으나, Descramble 가능한 Instance가 존재하지 않아서, DRM Info를 전달하지 못할 때 발행.
	eSEVT_CAS_DRM_NONE,											// p1 : DrmModelType_t, DRM 정보가 없을 경우에 Notify
	eSEVT_CAS_DRM_AV_BLANK,										// CAS DRM 동작에 의하여 AV Blanking을 control 해야 하는 경우 사용.

	// ----------------------------------------
	// Digital MacroVision을 위한 CAS Message
	eSEVT_CAS_DMV_SIGNALLED = (eSEVT_CAS_START + 600),
	eSEVT_CAS_DMV_UNKNOWN,
	eSEVT_CAS_DMV_NONE,

	/* - Description : UI 등에서 CAS action이 start 되기 전, library 등으로부터의 update 된 데이터가 필요한 경우, noti를 주기 위함. 그럼 UI 에서 가져가도록.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : cas group id (nagra, irdeto, conax  등 ), p2 : 없음., p3 : 없음.
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
	eCAS_ActionType_Descramble_a_File,		// Descramble a file 시 recording 전용.
	eCAS_ActionType_Tsr_Play,
	eCAS_ActionType_Tsr_Rec,
	eCAS_ActionType_Data,
	eCAS_ActionType_EngineeringTS,			// japan arib 에서 사용하는, svc handle 없이 ts 만 존재하는 cas용 engineering ts
	eCAS_ActionType_RfVod,				// japan 에서 사용하는 svc handle 없는 vod 용 CAS
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
	eCasShowType_None = 0,							// Audio, Video 모두 보여주지 않는다.
	eCasShowType_AudioOnly = 1,					// Audio만 보여준다.
	eCasShowType_VideoOnly = 2,					// Video만 보여준다.
	eCasShowType_Both = 3,							// Audio, Video 모두 보여준다.
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
	HUINT8						ucStreamType;			// MPEG에서 정의한 Stream Type (see ISO_VIDEO_1 ...)
	EsType_t					eEsType;				// Humax에서 정의하는 Es의 Type
	HUINT16 					usPid;					// Element stream의 Pid
	HUINT8						bIsCurrent;
	HBOOL						bElementCaFlag;			// ES Level의 Ca Descriptor 가 존재하는 지.
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

// MMI 관련.
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
	HUINT8							ucBlindOnOff;		/* 사용자 입력의 화면 표시 방식 */
	HUINT8							ucAnswerLen;		/* 질의에 응답해야 할 스트링 길이 */
	HUINT8							*pszPrompt;			/* 질의하는 스트링 */
} SvcCas_MmiEnq_t;

typedef struct
{
	HUINT8							ucNumOfItem;		/* 아이템의 개수 */
	HUINT8							*pszTitle;			/* 타이틀 영역에 표시될 스트링 */
	HUINT8							*pszSubTitle;		/* 서브타이틀 영역에 표시될 스트링 */
	HUINT8							*pszBottom;			/* 하단 영역에 표시된 스트링 */
	HUINT8							**ppszItem;			/* 각 아이템의 스트링 */
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

// pfnNotifyCcReport (for CC status or error reporting) : UI 처리 및 CiCc_DoActionForArcErr()에서 처리되는 것 외에 필요한 액션을 수행 ---
typedef enum
{
	// 라이선스 데이터가 훼손된 경우 (치명적 에러) -> "Can't load CI+ host license constants !" 표시할 것
	eCiPlus_CcReport_CanNotLoadHostLicenseConstants = 1,

	// 라이선스 데이터가 저장이 안된 경우 (치명적 에러) -> "CI+ host license constants not saved !" 표시할 것
	eCiPlus_CcReport_HostLicenseConstantsNotSaved,

	// 바인딩이 되지 않은 CI+ 캠이 삽입되어 바인딩 과정 시작시 (상태 표시) -> "Virgin CI+ CAM inserted. Authentication in progress. Please wait 1~2 minutes." 표시할 것
	eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress,

	// 캠과 세트 간의 바인딩 완료 (상태 표시) -> "Authentication success" 표시할 것 (또는 eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress 문구 닫거나...)
	eCiPlus_CcReport_AuthenticationSuccess, // 호스트 인증키가 만들어진 직후 시점.

	// 캠과 세트 간의 상호 인증 완료 (CAS/CI mgr 에서 사용하기 위한 용도)
	eCiPlus_CcReport_AuthenticationDoneAndStartSacEstablishment, // SAC Establishment 개시된 시점.

	// 캠과 세트 간의 URI Version 체크 완료 (CAS/CI mgr 에서 사용하기 위한 용도)
	eCiPlus_CcReport_UriVersionNegotiationDone, // URI Version Negotiation 된 직후 시점.

	// 캠과 세트 간의 인증 실패 (에러) -> "Authentication failed (error %d) !" 표시할 것 (여기서 %d 는 enArcErr 값)
	// 단, enArcErr 값이 CI_ARC_ERR_CAM_Cert_Expired 인 경우에는 "Authentication failed. Please check the time is correct on set and re-insert your CAM !" 표기할 것
	eCiPlus_CcReport_AuthenticationFailed,

	// 캠과 세트 간의 데이터 전송 또는 세트의 동작상 오류 (에러) -> "CI+ operation failed (error %d) !" 표시할 것 (여기서 %d 는 enArcErr 값)
	eCiPlus_CcReport_OperationFailed,

	// CC 프로토콜 상에서 캠이 응답하지 않는 경우 (에러) -> "No response from CI+ CAM (error %d) !" 표시할 것 (여기서 %d 는 enArcErr 값)
	eCiPlus_CcReport_NoResponseFromCam,

//#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	// 현재 삽입된 CAM이 CICAM PIN을 cache할 수 있어서 사용자로부터 PIN을 받는 과정을 수행하는 UI 출력
	// -> "To be able to record the pay-TV programmes, enter the CA PIN code. This CA PIN code will be saved in your set-top box" 표시 할 것 ( PIN code 입력 창 )
	eCiPlus_CcReport_RequireCicamPin,
	// 사용자로부터 잘못된 CICAM PIN을 입력 받았고, 더이상 PIN입력 받지 않을 때 -> "You have entered the wrong CA PIN code three times. Please try again later." 표시 할 것 ( banner에 표시)
	eCiPlus_CcReport_PinError,
	// 사용자로부터 잘못된 CICAM PIN을 입력 받았을 때 PIN code를 다시 입력받는 경우 -> "You have entered the wrong CA PIN code. Please try again." 표시 할 것 ( with PIN code 입력 창 )
	eCiPlus_CcReport_PinErrorRetry,
	// 사용자로부터 입력받은 PIN이 verify 된 경우 -> "The CA PIN code is saved." 표시 할 것 ( banner에 표시)
	eCiPlus_CcReport_PinVarified,
	// 사용자로부터 입력받은 PIN이 verify 되지 않은 경우 -> "The CA PIN code is saved, but the entered CA PIN code has not been verified." 표시 할 것 ( Box로 표시)
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

	// 이하 값들은 CI+ 스펙에 정의되어 있는 것으로 enum 값을 수정하거나 임의의 항목을 추가하지 말 것 !!

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
	unsigned int   bSavedFlag;					// 0 : not saved (이하 정보 유효하지 않음), 1 : saved
} SvcCas_CiAuthInfo_t;

typedef enum
{
	eCiPlus_CamUpgrade_Start,				// CAM firmware upgrade 진행 시작 처리 (현재로선 처리 사항 없음)
	eCiPlus_CamUpgrade_Processing,			// CAM firmware upgrade 진행 상태 처리 (진행률을 UI로 표시하고 user interaction 을 막을 것)
	eCiPlus_CamUpgrade_DoneOrCancelled,		// CAM firmware upgrade 진행 완료 처리 (UI로 표시하고 막혔던 user interaction 을 풀고 상황에 따라 캠 리셋할 것)
	eCiPlus_CamUpgrade_Failed,				// CAM firmware upgrade 진행 실패 처리 (UI로 표시하고 캠 리셋할 것)
	eCiPlus_CamUpgrade_Broken				// CAM firmware upgrade 진행 취소 처리 (유저에 의해 캠이 제거된 상황에 대한 처리)
} SvcCas_CiPlusCamUpgradeMode_e;

typedef enum
{
	eCiPlus_CamUpgradeReset_PCMCIA		= 0x00,		/* set RESET signal */ /* 일단 두 경우 모두 svc_cas_DevCiResetModule()을 호출하도록 한다... */
	eCiPlus_CamUpgradeReset_CAM			= 0x01,		/* set RS flag */
	eCiPlus_CamUpgradeReset_NO_RESET	= 0x02
} SvcCas_CiPlusCamUpgradeResetMode_e;		// CI_CU_RESET_REQUEST 값에 대응 : 이 값들은 CI+ 스펙에 정의되어 있는 것으로 enum 값을 수정하거나 임의의 항목을 추가하지 말 것 !!

typedef struct
{
	SvcCas_CiPlusCamUpgradeMode_e 		eCamUpgradeMode;
	unsigned char 						ucDownloadProgressInPercentage; /* 진행률(%) */
	SvcCas_CiPlusCamUpgradeResetMode_e 	eCamUpgradeResetMode;
	HUINT32								ulVer;
} SvcCas_CiPlusUpgrade_t;

// TODO: SvcCas_CiPlusPinCapability_e 및 SvcCas_CiPlusPinCapaInfo_t 정리.
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

	// bPinCapaSet은 capability_reply로 받는 정보는 아니지만, 현재 해당 CAM에 대하여 Pin Capability 가 setting 된 상태인지 파악하기 위해 추가함.
	HBOOL					bPinCapaSet;
} SvcCas_CiPlusPinCapaInfo_t;


/*-----------------------------------------------------------------------------------------------------------*/
/*  CAS Tune 관련 - 현재는 CI Tune으로 사용하지만, CAS Tune 동작이 필요할 경우에는 이 부분을 확장하여 사용   */
/*-----------------------------------------------------------------------------------------------------------*/

typedef enum
{
	eCasTuneMode_Start_v1_TuneByTripleId,		// HCI v1 에 의한 tune (즉, CI_TUNE 에 의한)
	eCasTuneMode_Start_v2_TuneByTripleId,		// HCI v2 에 의한 tune (즉, CI_TUNE_BROADCAST_REQ 에 의한), CI+ v1.3
	eCasTuneMode_Start_v2_TuneByLocation,		// HCI v2 에 의한 tune
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
			// CI_TUNE 에 의한 동작
			// eMode 가 eCasTuneMode_Start_v1_TuneByTripleId & eCasTuneMode_Start_v2_TuneByTripleId 일 때 사용된다.
			// HCI v2일 경우에 tuning 결과를 MW_CAS_TuneReply() 를 통해 CAM 에 알려주어야 한다.
			HUINT16	usNetId;
			HUINT16	usOnId;
			HUINT16	usTsId;
			HUINT16	usSvcId;
		} stTripleId;

		struct
		{
			// CI_TUNE_BROADCAST_REQ 에 의한 동작 (CI+ v1.3)
			// eMode 가 eCasTuneMode_Start_v2_TuneByLocation 일 때 사용된다.
			// tuning 결과는 MW_CAS_TuneReply() 를 통해 CAM 에 알려주어야 한다.
			// 이하 포인터들은 OxCAS_Malloc() 되어 전달되는 것으로 받는 쪽에서 반드시 OxCAS_Free() 해줘야 한다.
			DxDeliveryType_e  			eDeliType;
			HBOOL						bPmtFlag;
			HUINT16						usServiceId; // usServiceId 가 0 인 경우 bPmtFlag 는 FALSE 이고 pucDescriptors 를 참고하여 해당 TP 로 tuning 만 한다 (이 경우 ca_pmt 보내지 않음). usServiceId 가 0 이 아니면 PMT 내의 program_number 과 동일하다.
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
/*  Usage Rules Information (URI) 관련  */
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
#define CI_CC_URI_ICT_TIMEOUT		CI_CC_URI_ICT_ON	// CI_CC_URI_TIMEOUT 시 ICT 적용값

// Redistribution Control Trigger (RCT) for Encryption Plus Non-assert : 1 bit (for EMI 관련 추가 제어용)
#define CI_CC_URI_RCT_OFF			0x0					// No Redistribution Control asserted. Default.
#define CI_CC_URI_RCT_ON			0x1					// Redistribution Control asserted
#define CI_CC_URI_RCT_DEFAULT		CI_CC_URI_RCT_OFF

// Retention Limit (RL) : 6 bits (0x00, 0x01, 0x02 or 0x03..0x3F for EMI 관련 추가 제어용)
#define CI_CC_URI_RL_DEFAULT		0x00				// Default retention limit of 90 minutes applies

//#endif

// TODO: 위치 이동.

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	SVC_CAS_Init(PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData);
HERROR	SVC_CAS_SiRegisterTimeoutCallback(SvcCas_SiGetTimeOutCb_t fnSiTimeout);
HERROR	SVC_CAS_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActionType, SvcCas_StartData_t *pstCasStartData);
HERROR	SVC_CAS_StopService(Handle_t		hAction,
							Handle_t		hNextSvc,		/**< [in] 다음에 start할 service handle 예고. 현재와 같다면 demux filter를 유지함.		HANDLE_NULL(-1 or 0xffffffff)이면 미지정으로 무조건 stop. */
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
HERROR	SVC_CAS_GetMaxNumOfCISlot(HUINT32 *ulMaxSlot); // OCTO가 지원할 수 있는 최대 CI Slot 개수
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
