/*
@file				ir_ctrl.h
@brief				ir_ctrl.h

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2011 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef IRDETO_CTRL_H
#define IRDETO_CTRL_H

#include <ir_msg.h>
#include <ir_api.h>
#include <iruc_api.h>
/**************************************************************************************************/
#define ________Irdeto_ctrl_Private_Type_Define_____________________________________________
/**************************************************************************************************/

typedef enum tagControlTypeIr
{
	eCACTRL_IR_LOADER_Init = 0,
	eCACTRL_IR_LOADER_GetManuCode,
	eCACTRL_IR_LOADER_GetHWCode,
	eCACTRL_IR_LOADER_GetClientVersion,
	eCACTRL_IR_LOADER_GetStatusInfo,
	eCACTRL_IR_LOADER_GetLoadSeqNum,
	eCACTRL_IR_LOADER_GetSignDate,
	eCACTRL_IR_LOADER_GetSignTime,
	eCACTRL_IR_LOADER_GetDnldDate,
	eCACTRL_IR_LOADER_GetDnldTime,
	eCACTRL_IR_LOADER_GetSerialNum,
	eCACTRL_IR_LOADER_GetRandomNum,
	eCACTRL_IR_LOADER_GetLoaderMajor,
	eCACTRL_IR_LOADER_GetLoaderMinor,
	eCACTRL_IR_LOADER_GetLoaderType,
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	eCACTRL_IR_LOADER_GetCssn,
#endif


	eCACTRL_IR_OTA_ReadOTAInfo,
	eCACTRL_IR_OTA_ClearOTAInfo,

	eCACTRL_IR_OTA_SetFlagUp,
	eCACTRL_IR_OTA_ReadChannelParam,
	eCACTRL_IR_OTA_SetChannelParam,

	eCACTRL_IR_GetMailList,
	eCACTRL_IR_GetMailBox,
	eCACTRL_IR_CheckValidOfMail,
	eCACTRL_IR_GetMailReadState,
	eCACTRL_IR_SetMailReadState,
	eCACTRL_IR_DoYouHaveUnreadMail,
	eCACTRL_IR_DeleteMail,
	eCACTRL_IR_AllClearExpiredNormalAttributeMail,

	eCACTRL_IR_DRV_GetStringIdxFromErrorCode,
	eCACTRL_IR_SC_Info,
	eCACTRL_IR_DRV_CheckValidEvtCode,
	eCACTRL_IR_DRV_GetGlobalStatusOfSC,
	eCACTRL_IR_DRV_GetDrvScInfo,

	eCACTRL_IR_SVCSTATUS_GetNumberOfServices,
	eCACTRL_IR_SVCSTATUS_GetInformationOfEachService,
	eCACTRL_IR_SVCSTATUS_GetInformationOfElement,

/* 공통으로 사용될수 있는 기능을 넣는다.   */
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	eCACTRL_IR_GetSolStatus,
#endif

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
	eCACTRL_IR_GetFTABlockStatus,
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	eCACTRL_IR_MR_UpdatePIN,
	eCACTRL_IR_MR_ReadAllUserPF,
	eCACTRL_IR_MR_SetUserPF,
	eCACTRL_IR_MR_DeleteUserPF,
#endif

/* 여기부터  OP관련된 기능을 넣는다.  */
#if defined(CONFIG_OP_ALMAJD)
#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
	eCACTRL_IR_GetAlmajdHomingChannel,
#endif
	eCACTRL_IR_GetRegionCode,
	eCACTRL_IR_GetFirstBoot,
	eCACTRL_IR_ResetFirstBoot,
#endif
#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
	eCACTRL_IRUC_ClientStatus,
	eCACTRL_IRUC_ProductStatus,
#endif
	eCACTRL_IR_MAX,
} ControlTypeIr_t;

#if 0
typedef struct tagXXXParam
{
	XXXInParam_t	*pstInParam;
	XXXOutParam_t	*pstOutParam;
} XXXParam_t;
#endif

//xsvc_cas_IrLoaderInit
#if 0
typedef struct tagIR_LOADER_InitInParam_t
{
} IR_LOADER_InitInParam_t;
typedef struct tagIR_LOADER_InitOutParam_t
{
} IR_LOADER_InitOutParam_t;
#endif

//xsvc_cas_IrLoaderGetManuCode
#if 0
typedef struct tagIR_LOADER_GetManuCodeInParam_t
{
} IR_LOADER_GetManuCodeInParam_t;
#endif
typedef struct tagIR_LOADER_GetManuCodeOutParam_t
{
	HUINT16	usManCode;
} IR_LOADER_GetManuCodeOutParam_t;

typedef struct tagIR_LOADER_GetHwCodeOutParam_t
{
	HUINT16	usHwCode;
} IR_LOADER_GetHwCodeOutParam_t;

typedef struct tagIR_LOADER_GetClientVersionOutParam_t
{
	HUINT8 	ucClientVersion[UC_BYTES_GENERIC_SIZE];
} IR_LOADER_GetClientVersionOutParam_t;


typedef struct tagIR_LOADER_GetStatusOutParam_t
{
	IHL_STATUS	stIRStatus;
} IR_LOADER_GetStatusOutParam_t;

typedef struct tagIR_LOADER_GetLoadSeqNumOutParam_t
{
	HUINT16	usLoadSeqNum;
} IR_LOADER_GetLoadSqeNumOutParam_t;

typedef struct tagIR_LOADER_GetSignDateOutParam_t
{
	HUINT16 	usMJD;
} IR_LOADER_GetSignDateOutParam_t;

typedef struct tagIR_LOADER_GetSignTimeOutParam_t
{
	HxDATETIME_Time_t 	stTime;
} IR_LOADER_GetSignTimeOutParam_t;


typedef struct tagIR_LOADER_GetDnldDateOutParam_t
{
	HUINT16	usMJD;
} IR_LOADER_GetDnldDateOutParam_t;

typedef struct tagIR_LOADER_GetDnldTimeOutParam_t
{
	HxDATETIME_Time_t 	stTime;
} IR_LOADER_GetDnldTimeOutParam_t;

typedef struct tagIR_LOADER_GetSerialNumOutParam_t
{
	HUINT32	unSerialNum;
} IR_LOADER_GetSerialNumOutParam_t;

typedef struct tagIR_LOADER_GetRandomNumOutParam_t
{
	HUINT32	unRandomNum;
} IR_LOADER_GetRandomNumOutParam_t;

typedef struct tagIR_LOADER_GetLoaderMajOutParam_t
{
	HUINT8	ucLoaderMajor;
} IR_LOADER_GetLoaderMajOutParam_t;

typedef struct tagIR_LOADER_GetLoaderMinorOutParam_t
{
	HUINT8	ucLoaderMinor;
} IR_LOADER_GetLoaderMinorOutParam_t;

typedef struct tagIR_LOADER_GetLoaderTypeOutParam_t
{
	HUINT8	ucLoaderType;
} IR_LOADER_GetLoaderTypeOutParam_t;

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
typedef struct tagIR_LOADER_GetCssnOutParam_t
{
	long int	ulCssn;
} IR_LOADER_GetCssnOutParam_t;
#endif

typedef struct tagIR_OTA_GetOtaInfoOutParam_t
{
	HUINT8	ucTableID;
	HUINT16	usDownloadServicePID;
} IR_OTA_GetOtaInfoOutParam_t;

typedef struct tagIR_OTA_SetOtaFlagUpInParam_t
{
	HBOOL	bflagDoOTA;
	HUINT16	usDownloadServicePID;
	HUINT8	ucTableID;
} IR_OTA_SetOtaFlagUpInParam_t;

typedef struct tagIR_OTA_GetChannelParamOutParam_t
{
	HUINT32		unTunerId;
	void		*pvDiChannel_param;
} IR_OTA_GetChannelParamOutParam_t;

typedef struct tagIR_OTA_SetChannelParamInParam_t
{
	HUINT32		unTunerId;
	void			*pvDiChannel_param;
} IR_OTA_SetChannelParamInParam_t;

typedef struct tagIR_GetMailListOutParam_t
{
	HUINT8	ucNumberOfSavedMail;
	HUINT8	*paucMailIndexList;
} IR_GetMailListOutParam_t;

typedef struct tagIR_GetMailBoxOutParam_t
{
	HUINT8			ucMailIndex;
	MAILBOX_TABLE	*pstMailBox;
} IR_GetMailBoxOutParam_t;

typedef struct tagIR_GetCheckMailOutParam_t
{
	HBOOL			bCheckMail;
	HUINT8			ucMailIndex;
} IR_GetCheckMailOutParam_t;

typedef struct tagIR_GetMailStateOutParam_t
{
	HBOOL			bReadMail;
	HUINT8			ucMailIndex;
} IR_GetMailStateOutParam_t;

typedef struct tagIR_SetMailStateInParam_t
{
	HBOOL			bReadMail;
	HUINT8			ucMailIndex;
} IR_SetMailStateInParam_t;

typedef struct tagIR_DoUHaveUnReadMailOutParam_t
{
	HUINT8			ucTypeFlag;
} IR_DoUHaveUnReadMailOutParam_t;

typedef struct tagIR_DeleteMailInParam_t
{
	HUINT8			ucMailIndex;
} IR_DeleteMailInParam_t;

typedef struct tagIR_GetStrIdxFromErrCodeOutParam_t
{
	// input value
	IR_STATE 			*pstIrStatus;

	// return value
	AXI_STRING_IDX		unStrIdx;
} IR_GetStrIdxFromErrCodeOutParam_t;

typedef struct tagIR_GetSCInfoOutParam_t
{
	// input value
	HUINT8 				ucScSlotId;

	// return value
	IR_API_SC_INFO 		*pstIrApiScInfo;
} IR_GetSCInfoOutParam_t;
// hjlee3
typedef struct tagIR_GetClientInfoOutParam_t
{
	// input value
	HUINT8 				ucScSlotId;

	// return value
	IRUC_CLIENT_STATUS 		*pstIrApiClientInfo;
} IR_GetClientInfoOutParam_t;

typedef struct tagIR_GeProductInfoOutParam_t
{
	// input value
	HUINT8 				ucScSlotId;
	HUINT32				productCnt;

	// return value
	IRUC_PRODUCT_STATUS 		*pstIrApiProductInfo;
} IR_GetProductInfoOutParam_t;

typedef struct tagIR_CheckValidEvtCodeOutParam_t
{
	// input value
	HINT32 				nEvtCode;

	// return value
	HUINT8 				ucEvtCode;
} IR_CheckValidEvtCodeOutParam_t;

typedef struct tagIR_GetGlobalStatusOfScOutParam_t
{
	// input value
	HUINT8 				ucSlotId;

	// return value
	HUINT32 				ulStatus;
	HBOOL				bGetStatus;
} IR_GetGlobalStatusOfScOutParam_t;

typedef struct tagIR_GetDrvSmartCardInfoOutParam_t
{
	// input value
	HUINT8 				ucSlotId;

	// return value
//	IR_DRV_SC_INFO		*pstIrDrvScInfo;			참조 에러가 나니 실제 쓰는 항목은 하나 뿐인데 그것만 뽑아서 가지고 오도록 변경이 필요함!!
	HBOOL				bGetStatus;
} IR_GetDrvSmartCardInfoOutParam_t;

typedef struct tagIR_GetNumberOfServicesOutParam_t
{
	// input value

	// return value
	IR_SVC_STATUS_INFO_MAIN	*pstSvcStatusInfo;
	HUINT8 						ucNumberOfServices;
	HBOOL						bGetStatus;
} IR_GetNumberOfServicesOutParam_t;

typedef struct tagIR_GetInformationOfEachServiceOutParam_t
{
	// input value
	HUINT32 						eSvcType;
	HUINT16 						usHandle;

	// return value
	IR_SVC_STATUS_INFO_EACH	*pstSvcStatusInfoEach;
	HBOOL						bGetStatus;
} IR_GetInformationOfEachServiceOutParam_t;

typedef struct tagIR_GetInformationOfElementOutParam_t
{
	// input value
	HUINT32 						ulResourceId;
	HUINT16 						usHandle;

	// return value
	IR_ELEM_STATE				*pstSvcStatusInfoElem;
	HUINT8						ucNumOfElem;
} IR_GetInformationOfElementOutParam_t;


#if defined(CONFIG_MW_CAS_IRDETO_SOL)

typedef struct tagIR_GetSolStatusOutParam_t
{
	// input value
	Handle_t		hAction;
	HUINT16		ONID;
	HUINT16		TSID;

	// return value
	SOL_SERVICE_STATE	eSolState;
} IR_GetsolStatusOutParam_t;

#endif

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)

typedef struct tagIR_GetFtaBlockStatusOutParam_t
{
	// input value
	Handle_t				hAction;

	// return value
	HUINT32				unFtaBlockStatus;
} IR_GetFtaBlockStatusOutParam_t;

#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)

typedef struct tagIR_MR_UpdatePinInParam_t
{
	Handle_t		hAction;
	HUINT8		aucDecimalPIN[4];
} IR_MR_UpdatePinInParam_t;

typedef struct tagIR_GetReadAllUserPFOutParam_t
{
	HUINT32		resourceId;
	HUINT8		aucDecimalPIN[4];
} IR_GetReadAllUserPFOutParam_t;

typedef struct tagIR_MR_SetUserPFInParam_t
{
	HUINT8	ucUserIdx;
	HUINT8	ucAge;
	HUINT8	aucDecimalPIN[4];
} IR_MR_SetUserPFInParam_t;

typedef struct tagIR_MR_DeleteUserPFInParam_t
{
	HUINT8	ucUserIdx;
	HUINT8	aucDecimalPIN[4];
} IR_MR_DeleteUserPFInParam_t;

#endif

#if defined(CONFIG_OP_ALMAJD)

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)

typedef struct tagIR_GetAlmajdHomingChannelOutParam_t
{
	HUINT8						ucSlot;
	IR_SC_HOME_CHANNEL_INFO	stHomeChannel;
} IR_GetAlmajdHomingChannelOutParam_t;

#endif

typedef struct tagIR_GetRegionCodeOutParam_t
{
	HUINT16 ausRegionCode[2];
} IR_GetRegionCodeOutParam_t;

typedef struct tagIR_GetFirstBootOutParam_t
{
	HUINT8 ucFirstBoot;
} IR_GetFirstBootOutParam_t;

#endif


union tagIr_InParam_t
{
	IR_OTA_SetOtaFlagUpInParam_t		stSetOtaFlag;
	IR_OTA_SetChannelParamInParam_t	stSetChannelParam;
	IR_SetMailStateInParam_t			stSetMailState;
	IR_DeleteMailInParam_t				stDeleteMail;

	#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	IR_MR_UpdatePinInParam_t			stUpdatePin;
	IR_MR_SetUserPFInParam_t			stSetUserPF;
	IR_MR_DeleteUserPFInParam_t		stDeleteUserPF;
	#endif
};

union tagIr_OutParam_t
{
	IR_LOADER_GetManuCodeOutParam_t		stGetManuCode;
	IR_LOADER_GetHwCodeOutParam_t		stGetHwCode;
	IR_LOADER_GetStatusOutParam_t		stGetSatus;
	IR_LOADER_GetLoadSqeNumOutParam_t	stGetLoadSqeNum;
	IR_LOADER_GetSignDateOutParam_t		stGetSignDate;
	IR_LOADER_GetSignTimeOutParam_t		stGetSignTime;
	IR_LOADER_GetDnldDateOutParam_t		stGetDnldDate;
	IR_LOADER_GetDnldTimeOutParam_t		stGetDnldTime;
	IR_LOADER_GetSerialNumOutParam_t		stGetSerialNum;
	IR_LOADER_GetRandomNumOutParam_t	stGetRandomNum;
	IR_LOADER_GetLoaderMajOutParam_t		stGetLoaderMaj;
	IR_LOADER_GetLoaderMinorOutParam_t	stGetLoaderMinor;
	IR_LOADER_GetLoaderTypeOutParam_t	stGetLoaderType;
	IR_OTA_GetOtaInfoOutParam_t			stGetOtaInfo;
	IR_OTA_GetChannelParamOutParam_t		stGetChannelParam;
	IR_GetMailListOutParam_t				stGetMailList;
	IR_GetMailBoxOutParam_t				stGetMailBox;
	IR_GetCheckMailOutParam_t				stGetCheckMail;
	IR_GetMailStateOutParam_t				stGetMailState;
	IR_DoUHaveUnReadMailOutParam_t		stDoUHaveUnReadMail;
	IR_GetStrIdxFromErrCodeOutParam_t		stGetStrIdxFromErrCode;
	IR_GetSCInfoOutParam_t				stGetSCInfo;
	IR_CheckValidEvtCodeOutParam_t		stCheckValidEvtCode;

	#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	IR_GetsolStatusOutParam_t				stGetsolStatus;
	#endif

	#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
	IR_GetFtaBlockStatusOutParam_t			stGetFtaBlockStatus;
	#endif

	#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	IR_GetReadAllUserPFOutParam_t			stGetReadAllUserPF;
	#endif

	#if defined(CONFIG_OP_ALMAJD)
	#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
	IR_GetAlmajdHomingChannelOutParam_t	stGetAlmajdHomingChannel;
	#endif
	IR_GetRegionCodeOutParam_t			stGetRegionCode;
	IR_GetFirstBootOutParam_t			stGetFirstBoot;
	#endif

// hjlee3
	IR_GetClientInfoOutParam_t			stGetClientInfo;
};


typedef union tagIr_InParam_t		Ir_InParam_t;
typedef union tagIr_OutParam_t	Ir_OutParam_t;

#if 0
#define IR_Param_t		CACTRL_IR_Param_t
struct IR_Param_t
{
	Ir_InParam_t			*putIn;
	Ir_OutParam_t		*putOut;
};
#endif


/********************************************************************
  Extern Function declaration.
 ********************************************************************/
extern	HERROR CA_IR_RegisterCtrlList(void);

#endif /* IRDETO_CTRL_H */


