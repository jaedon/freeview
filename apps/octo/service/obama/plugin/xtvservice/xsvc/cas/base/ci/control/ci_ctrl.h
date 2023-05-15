/*
@file				ci_ctrl.h
@brief				ci_ctrl.h

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2011 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef CI_CTRL_H
#define CI_CTRL_H

// TODO: ci_plus.h 파일 참조 가능여부 coding convention 따져볼것.
//#include "/ci_plus.h"


/**************************************************************************************************/
#define ________Ci_ctrl_Private_Type_Define_____________________________________________
/**************************************************************************************************/
typedef enum tagControlTypeCi
{
	eCACTRL_CI_MMI_Open,
	eCACTRL_CI_MMI_Close,
	eCACTRL_CI_MMI_Select,
	eCACTRL_CI_MMI_Answer,
	eCACTRL_CI_MMI_GetData,
	eCACTRL_CI_GetSlotIdByActionId,
#ifdef CONFIG_MW_CI_PLUS
	eCACTRL_CI_GetHostId,
	eCACTRL_CI_GetBindedCamId,
	eCACTRL_CI_GetCcReport,
	eCACTRL_CI_InitAuthContexts,
	eCACTRL_CI_ReqFileOrData,
	eCACTRL_CI_ReqAppAbort,

	#if 0
	eCACTRL_CI_DownloadCiPlusData,
	eCACTRL_CI_AddNotifyFuncForDownloading,
	#endif

	eCACTRL_CI_ChangeHostCountrySetting,
	eCACTRL_CI_ChangeHostLanguageSetting,

	eCACTRL_CI_NotifyDrmInfoConsumed,

	eCACTRL_CI_GetUpgradeStatus,

	#if defined(CONFIG_OP_CANALREADY)
	eCACTRL_CI_CanalReadyGetInitInfo,
	eCACTRL_CI_CanalReadyMoralityLevelUnlock,
	#endif	// #if defined(CONFIG_OP_CANALREADY)

	#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
	eCACTRL_CI_NotifyTuneStatus,
	eCACTRL_CI_NotifyHciPmtUse,				// CI+에 의한 Tune 동작 중 HCI 로 받은 pmt를 사용해야 할 때 이를 noti 하기 위하여 사용
	#endif

	#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	eCACTRL_CI_SetParentalCtrl,			// CI+ CC resource에서 운용되는 Parental Control 관련 정보 set
	eCACTRL_CI_SetLicense,				// CI+ CC resource에서 운용되는 License 관련 정보 set
	eCACTRL_CI_SetPinCode,				// web app 로부터 입력받은 PIN code를 내부적으로 저장하기 위해 사용
	eCACTRL_CI_GetPinCapa,				// CI+ CC resource에 PIN code capability get
	#endif

	#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	eCACTRL_CI_OperatorPrepareStatus,
	eCACTRL_CI_SetOperatorTuneStatus,
	eCACTRL_CI_GetOpNetNum,
	eCACTRL_CI_GetOpNetInfo,
	eCACTRL_CI_GetOpTsNum,
	eCACTRL_CI_GetOpTsInfo,
	eCACTRL_CI_GetOpSvcNum,
	eCACTRL_CI_GetOpSvcInfo,
	eCACTRL_CI_GetOpName,
	eCACTRL_CI_GetOpRsvTime,
	eCACTRL_CI_GetOpSupportCis,
	eCACTRL_CI_GetOpErrStatus,
	#endif
#endif	// #ifdef CONFIG_MW_CI_PLUS
	eCACTRL_CI_MAX
} ControlTypeCi_t;

#if 0
typedef struct tagXXXParam
{
	XXXInParam_t	*pstInParam;
	XXXOutParam_t	*pstOutParam;
} XXXParam_t;
#endif

// SVC_CAS_GetHostId
#if 0
typedef struct tagCI_GetHostIdInParam_t
{
}CI_GetHostIdInParam_t;
#endif

typedef struct tagCI_MmiOpenInParam_t
{
	HUINT16	usSlotId;
}CI_MmiOpenInParam_t;

typedef struct tagCI_MmiCloseInParam_t
{
	Handle_t hMmi;
}CI_MmiCloseInParam_t;

typedef struct tagCI_MmiSelectInParam_t
{
	Handle_t	hMmi;
	HUINT8	ucItemNumber;
}CI_MmiSelectInParam_t;

typedef struct tagCI_MmiAnswerInParam_t
{
	Handle_t		hMmi;
	HUINT8		*pucString;
	HUINT16		usStringLength;
}CI_MmiAnswerInParam_t;

typedef struct tagCI_GetDataInParam_t
{
	HUINT16		usSlotId;
	HUINT32		ulDataVer;
}CI_GetDataInParam_t;

typedef struct tagCI_GetDataOutParam_t
{
	SvcCas_MmiData_t **ppMmiData;
}CI_GetDataOutParam_t;

typedef struct tagCI_GetSlotIdByActionIdInParam_t
{
	HUINT32		ulActionId;
}CI_GetSlotIdByActionIdInParam_t;

typedef struct tagCI_GetSlotIdByActionIdOutParam_t
{
	HUINT16		usSlotId;
}CI_GetSlotIdByActionIdOutParam_t;

typedef struct tagCI_GetHostIdOutParam_t
{
	HUINT8 *pucHost;
}CI_GetHostIdOutParam_t;

// MW_CAS_GetBindedCamId
typedef struct tagCI_GetBindedCamIdInParam_t
{
	HUINT16 usSlotId;
}CI_GetBindedCamIdInParam_t;

typedef struct tagCI_GetBindedCamIdOutParam_t
{
	HUINT8 *pucBindedCamId;
}CI_GetBindedCamIdOutParam_t;

typedef struct tagCI_GetCcReportOutParam_t
{
	HUINT16			usSlotId;
	HUINT32			ulEvtVersion;
	SvcCas_CcReport_t	*pData;
}CI_GetCcReportOutParam_t;

// SVC_CAS_InitAuthContexts
typedef struct tagCI_InitAuthContextsInParam_t
{
	HUINT16 usSlotId;
}CI_InitAuthContextsInParam_t;

// FileReq()'s RequestType
typedef enum
{
	CICtrlAmmiRequestType_File = 0x00,
	CICtrlAmmiRequestType_Data = 0x01
}CICtrlAmmiRequestType_t; // CI+ 스펙

typedef struct tagCI_RequestFileOrDataInParam_t
{
	unsigned short				usSessionId;
	CICtrlAmmiRequestType_t		enReqType;
	unsigned char					*pucReqName;
	unsigned long					ulReqNameLen;
}CI_RequestFileOrDataInParam_t;

// CI_AMMI_ABORT_REQUEST_CODE 와 같다. 어디어 넣어야 할지 좀더 고민 필요
typedef enum
{
	CICtrlAmmiAbortRequestCode_UserCancel = 0x01,
	CICtrlAmmiAbortRequestCode_SystemCancel = 0x02
}CICtrlAmmiAbortRequestCode_t;

typedef struct tagCI_RequestAppAbort_t
{
	unsigned short					usSessionId;
	CICtrlAmmiAbortRequestCode_t		enAbortReqCode;
}CI_RequestAppAbort_t;

#if 0
typedef struct tagCI_InitAuthContextsOutParam_t
{
}CI_InitAuthContextsOutParam_t;
#endif

// MW_CAS_DownloadCiPlusData
typedef struct tagCI_DownloadCiPlusDataInParam_t
{
	HUINT8 ucValue;
}CI_DownloadCiPlusDataInParam_t;
#if 0
typedef struct tagCI_DownloadCiPlusDataOutParam_t
{
}CI_DownloadCiPlusDataOutParam_t;
#endif

#if 0
// MW_CAS_AddNotifyFuncForDownloading
typedef struct tagCI_AddNotifyFuncForDownloadingInParam_t
{
	pfnDI_FACTORY_DOWNLOAD_Notify fnFacNotify;
}CI_AddNotifyFuncForDownloadingInParam_t;
#if 0
typedef struct tagCI_AddNotifyFuncForDownloadingOutParam_t
{
}CI_AddNotifyFuncForDownloadingOutParam_t;
#endif
#endif

// SVC_CAS_ChangeHostCountrySetting
#if 0
typedef struct tagCI_ChangeHostCountrySettingInParam_t
{
}CI_ChangeHostCountrySettingInParam_t;
typedef struct tagCI_ChangeHostCountrySettingOutParam_t
{
}CI_ChangeHostCountrySettingOutParam_t;
#endif

// SVC_CAS_ChangeHostLanguageSetting
#if 0
typedef struct tagCI_ChangeHostLanguageSettingInParam_t
{
}CI_ChangeHostLanguageSettingInParam_t;
typedef struct tagCI_ChangeHostLanguageSettingOutParam_t
{
}CI_ChangeHostLanguageSettingOutParam_t;
#endif

#if defined(CONFIG_MW_CI_PLUS)
typedef struct tagCI_NotifyDrmInfoConsumedInParam_t
{
	HUINT8 			ucDrmInfoVer;
	HERROR			hConsumedStatus;		// 필요시 enum으로 정의해도 됨!
}CI_NotifyDrmInfoConsumedInParam_t;
#endif

#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
typedef struct tagCI_NotifyTuneStatusInParam_t
{
	unsigned short	usSlotId;
	HUINT32 		ulTargetResource;
	HUINT8			ucTuneResult;
}CI_NotifyTuneStatusInParam_t;

typedef struct tagCI_NotifyHciPmtUseInParam_t
{
	unsigned short		usSlotId;
	Handle_t			hAction;
}CI_NotifyHciPmtUseInParam_t;
#endif

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
typedef struct tagCI_SetParentalCtrlInParam_t
{
	HUINT16			usSlotId;
	Handle_t		hPbAction;
	HUINT32			unMetaIndex;
	HUINT8			ucAgeLimit;		// 나이 제한 (HUINT8 이 적당하지 않다면 바꿔도 됨!)
	HUINT8			*pucPrivateData;
}CI_SetParentalCtrlInParam_t;

typedef struct tagCI_SetLicenseInParam_t
{
	Handle_t		hPbAction;
	HUINT32			unMetaIndex;
	HUINT16			usSlotId;
	HUINT16			usProgramNum;
	HUINT8			*pucLicenseData;
	HUINT16			usLicenseLength;
}CI_SetLicenseInParam_t;

typedef struct tagCI_SetPinByUserInParam_t
{
	HUINT16			usSlotId;
	HUINT32			unPinLength;
	HUINT8			*pucPinData;
}CI_SetPinByUserInParam_t;

typedef struct tagCI_GetPinCapabilityInParam_t
{
	HUINT16			usSlotId;
}CI_GetPinCapabilityInParam_t;

typedef struct tagCI_GetPinCapabilityOutParam_t
{
	SvcCas_CiPlusPinCapaInfo_t	stPinCapaInfo;
}CI_GetPinCapabilityOutParam_t;
#endif

typedef struct tagCI_OpCiCamNit
{
	HUINT16		usSlotId;
	HBOOL		bSearchAvailable;
}CI_OperatorPrepareStatus_t;

typedef struct tagCI_SetOperatorSearchStart_t
{
	HUINT16		usSlotId;
}CI_SetOperatorSearchStart_t;

typedef struct tagCI_SetOperatorTuneStatus_t
{
	HUINT16		usSlotId;
	HUINT8		ucStrength;
	HUINT8		ucQuality;
	HUINT8		ucStatus;
	HUINT8		ucDesLen;
	HUINT8		*pucDes;
	HUINT8		ucNextDesNum;
}CI_SetOperatorTuneStatus_t;

typedef struct tagCI_GetOperatorInParam
{
	HUINT16		usSlotId;
	HUINT16		usNetIdx;
	HUINT16		usTsIdx;
}CI_GetOperatorInParam_t;

typedef struct tagCI_GetOperatorNameInParam
{
	HUINT16		usSlotId;
}CI_GetOperatorNameInParam_t;

typedef struct tagCI_GetOperatorNameOutParam
{
	HUINT8 *pucOpName;
}CI_GetOperatorNameOutParam_t;

union tagCi_InParam_t
{
	CI_MmiOpenInParam_t							stMmiOpen;
	CI_MmiCloseInParam_t						stMmiClose;
	CI_MmiAnswerInParam_t						stMmiAnswer;
	CI_MmiSelectInParam_t						stMmiSelect;
	CI_GetDataInParam_t							stGetDataIn;
	CI_GetSlotIdByActionIdInParam_t				stGetSlotIdByActionId;
	CI_GetBindedCamIdInParam_t					stGetBind;
	CI_InitAuthContextsInParam_t				stInitAuthContexts;
	CI_RequestFileOrDataInParam_t				stReqFileOrData;
	CI_RequestAppAbort_t						stReqAppAbort;
	CI_DownloadCiPlusDataInParam_t				stDownloadCiPlusData;
#if 0
	CI_AddNotifyFuncForDownloadingInParam_t		stAddNotifyFuncForDownloading;
#endif
//	CI_ChangeHostCountrySettingInParam_t		stChangeHostCountrySetting;
//	CI_ChangeHostLanguageSettingInParam_t		stChangeHostLanguageSetting;
#if defined(CONFIG_MW_CI_PLUS)
	CI_NotifyDrmInfoConsumedInParam_t			stNotifyDrmInfoConsumed;
#endif
#if defined(CONFIG_OP_CANALREADY)
	CI_CanalReadyGetInitInfoInParam_t			stCanalReadyGetInitInfo;
	CI_CanalReadyMoralityLevelUnlockInParam_t	stCanalReadyMoralityLevelUnlock;
#endif
#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
	CI_NotifyTuneStatusInParam_t				stNotifyTuneStatus;
#endif
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	CI_GetOperatorInParam_t						stGetOperator;
	CI_GetOperatorNameInParam_t					stGetOperatorName;
	CI_GetPinCapabilityInParam_t				stGetPinCapa;
#endif
};

union tagCi_OutParam_t
{
	CI_GetDataOutParam_t						stGetDataOut;
	CI_GetSlotIdByActionIdOutParam_t			stGetSlotIdByActionId;
	CI_GetHostIdOutParam_t						stGetHostId;
	CI_GetBindedCamIdOutParam_t					stGetBindedCamId;
	CI_GetCcReportOutParam_t					stGetCcReport;
//	CI_InitAuthContextsOutParam_t				stInitAuthContexts;
//	CI_DownloadCiPlusDataOutParam_t			stDownloadCiPlusData;
//	CI_AddNotifyFuncForDownloadingOutParam_t	stAddNotifyFuncForDownloading;
//	CI_ChangeHostCountrySettingOutParam_t		stChangeHostCountrySetting;
//	CI_ChangeHostLanguageSettingOutParam_t	stChangeHostLanguageSetting;
#if defined(CONFIG_OP_CANALREADY)
	CI_CanalReadyGetInitInfoOutParam_t			CI_CanalReadyGetInitInfo;
//	CI_CanalReadyMoralityLevelUnlockOutParam_t	stCanalReadyMoralityLevelUnlock;
#endif
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	CI_GetOperatorNameOutParam_t				stGetOperatorName;
	CI_GetPinCapabilityOutParam_t				stGetPinCapa;
#endif
};

typedef union tagCi_InParam_t	Ci_InParam_t;
typedef union tagCi_OutParam_t	Ci_OutParam_t;

#if 0
#define CI_Param_t		CACTRL_CI_Param_t
struct CI_Param_t
{
	Ci_InParam_t		*putIn;
	Ci_OutParam_t		*putOut;
};
#endif

/********************************************************************
  Extern Function declaration.
 ********************************************************************/
extern	HERROR CI_Ctrl_RegisterCtrlList(void);

#endif /* CI_CTRL_H */


