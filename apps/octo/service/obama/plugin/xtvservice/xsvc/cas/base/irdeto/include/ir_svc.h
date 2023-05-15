#ifndef __IR_SVC_H__
#define	__IR_SVC_H__


/********************************************************************
*   Including Headers
********************************************************************/
#include "octo_common.h"
//#include "ir_common.h"
#include "ir_platform.h"
//#include "vd_humax.h"
#include "s3_service.h"
#include "s3_cadrv.h"

#include "_svc_cas_mgr_common.h"
//#include "ir_api.h"
#include <pal_pipe.h>

/********************************************************************
*   define
********************************************************************/
//#define IRDRV_NULL_PID	(0x2000) // TODO: it will be erased after octo policry confirmed.

//#define GET_IR_SERVERITY_CHAR_FROM_STATUSCODE(statuscode)	(char) (statuscode>>24)

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1) && !defined(CONFIG_MW_CAS_IRDETO_SOFTCELL_3_11)
	#define	IR_SVC_CLOSE_USE_NULL_PMT
#endif

/********************************************************************
*   type definition
********************************************************************/
enum
{
	RESOURCE_GLOBAL_SOURCE = RESOURCE_HUMAX_INTERNAL, // by jichoi.  = RESOURCE_IPDC_DESCRAMBLER + 1,	// 20+1
	RESOURCE_INTERNAL_SOURCE,
	RESOURCE_ELEM_STREAM_SOURCE,
	RESOURCE_SESSION_SUB,
	RESOURCE_DRM_RIGHT,
	RESOURCE_ERROR,
	RESOURCE_MAX
};


typedef struct
{
	HUINT32			Valid;
	HUINT32			usResourceID;
	char			CharSeverity;
	ia_status_st	stStatus;
} IR_STATUS_INFO;

typedef struct
{
	HUINT32			Valid;
	HUINT16			usEcmPID;
	char			CharSeverity;
	ia_status_st	stStatus;
} IR_STATUS_ELEM_INFO;

typedef struct
{
	HUINT32			Valid;
	HUINT16			usResourceID;
	HUINT16			usEmmPID;
	char			CharSeverity;
	ia_status_st	stStatus;
} IR_STATUS_EMM_INFO;


typedef struct
{
	HUINT16		usPID;
	HUINT8		ucTypeFromPMT;
	HUINT8		ucTypeFromPlay;
	HUINT8		IsLive;
	HUINT8		IsRecord;

	IR_STATUS_ELEM_INFO	ElemRsc[CAS_MAX_NUM_OF_SC];
}IR_PID_INFO;

#if defined(IRPLAT_SYS_PVR)
#define		STREAM_CRYPTO_KEY		0
#define		STREAM_CRYPTO_TYPE_ODD	1
#define		STREAM_CRYPTO_TYPE_EVEN	2

#define		STREAM_CRYPTO_SAVE		0x01
#define		STREAM_CRYPTO_SET		0x02
#define		STREAM_CRYPTO_DONE		0x03

#if	defined(IRPLAT_PVR_DRM)
typedef	struct
{
	HUINT32		ulRightRecId;
	HUINT32		ulSeqRecId;
} IR_PVRDRM_BASIC_INFO;

typedef	struct
{
	HUINT32		ulCRID;

	HUINT32		eCopyControl;
	HUINT32		ulPlaybackStartTime;
	HUINT32		ulPlaybackDuration;
	HUINT8		NrOfPlayback;
} IR_PVRDRM_RIGHTREC_INFO;

typedef	struct
{
	HBOOL		bReady;
	HBOOL		fNonPVR;
	HBOOL		fMustExpired;
} IR_PVRDRM_RECFLAG_INFO;

typedef	struct
{
	HBOOL		bReady;			//	for Basic and RightRec
	IR_PVRDRM_BASIC_INFO	stDrmBasicInfo;
	IR_PVRDRM_RIGHTREC_INFO	stDrmRightRecInfo;
	IR_PVRDRM_RECFLAG_INFO	stDrmRecFlagInfo;
} IR_PVRDRM_INFO;
#endif

typedef	struct
{
	HUINT8		ucIsSCELLReady;

	HUINT16		usSCELL_SvcType;
	HUINT16		usSCELL_SvcHandle;

	HUINT32		ulContentId;

	IR_SVCACTIONType_e	eIrSvcActionType;

	IR_STATUS_INFO		GlobalStatus;
	IR_STATUS_INFO		InternalStatus;
	IR_STATUS_INFO		SessionManageRscStatus;
	IR_STATUS_INFO		SessionSubStatus;
	IR_STATUS_INFO		CryptoOperationRscStatus;
	IR_STATUS_INFO		MacrovisionRscStatus;

#if	defined(IRPLAT_PVR_DRM)
	IR_PVRDRM_INFO		stPVRDrmInfo;

	HBOOL				bIsSetCryptoKey;		//	Set crypto key to encrypter or decrypter
#endif

	HUINT8				stateCryptoKey;			//	VD StreamCrypto보다 SCELL의 PVR OK가 먼저 올라 오는 경우가 존재함.
												//	Flag로의 관리가 필요할수도...
	HUINT8				aucCryptoKey[2][IR_CRYPTO_KEY_LEN];
}	IR_PVRSERVICE_INFO;
#endif

typedef	struct
{
	HUINT16 						ausEcmMonitorPID[MAX_NUM_ECM_MONITOR_PID];
	msg_dvb_ecm_monitor_data_st 	astEcmMonitoring[MAX_NUM_ECM_MONITOR_PID];
}IR_ECM_MONITOR_INFO;

typedef	struct
{
	Handle_t			hIrSvc;				//	Identifier in Irdeto Module
	Handle_t			hAction;			//	action handle from MW
	Handle_t			hSvc;				//	service handle from MW

	HUINT32				ulContentsId;		//	PVR base, same as recordId
	IR_SVCACTIONType_e	eIrSvcActionType;
	SvcCas_ActionType_e	eCasActionType;		//	CAS Action time (PB/REC/TSRPB/TSRREC)

	HUINT8				ucIsAllocated;		//	flag for slot
	HUINT8				ucIsSCELLReady;		//	flag for Open Reply from SCELL

	HUINT8				ucDemuxId;			//	DemuxId
	HUINT8				ucIsSelected;		//	current service is selected.

	HUINT8				ucRecordMode;
	HUINT8				ucIsValidPMT;
	HUINT8				*pucPMTRaw;

	HUINT8				ucStreamCount;
	IR_PID_INFO			astPidList[IR_SVC_MAX_PID];

	eIrDrvCaType		eSvcCasKind;

	HUINT16				usSCELL_SvcType;	//	Softcell Service Type
	HUINT16				usSCELL_SvcHandle;	//	Softcell Service Handle

	IR_STATUS_INFO		GlobalStatus;
	IR_STATUS_INFO		InternalStatus;
	IR_STATUS_INFO		DvbDescramblerRscStatus;
	IR_STATUS_INFO		EcmRscStatus;
	IR_STATUS_INFO		SmartcardRscStatus;
	IR_STATUS_INFO		MacrovisionRscStatus;

	IR_ECM_MONITOR_INFO		EcmMonitorInfo[CAS_MAX_NUM_OF_SC];

#if	defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	IR_STATUS_INFO		MaturityRatingRscStatus;
#endif

#if defined(IRPLAT_SYS_PVR)
	IR_PVRSERVICE_INFO		*pstPVRSvcInfo;
#endif
}	IR_SERVICE_INFO;

#if	defined(IRPLAT_SYS_EMM_BASED_ORGID)
typedef	enum
{
	eIRTP_State_NONE,
	eIRTP_State_Waiting,
	eIRTP_State_Started,
	eIRTP_State_NeedRestart,			//	information is changed in same action id
}	IRTP_State_e;
#endif

typedef struct
{
	Handle_t		hAction; //for emm filter.
	HINT32		nDmxId;

	HUINT8		ucIsAllocated;
	HUINT8		IsValidCat;
	HUINT16		usTsUniqueId;
#if	defined(IRPLAT_SYS_EMM_BASED_ORGID)
	HUINT16 			usTsId;
	HUINT16			usOrgNetId;
	IRTP_State_e	eState;
#endif

	HUINT8		*pCatRaw;
	HUINT32		nCatLen;

	HUINT8		ucIsSCELL_Ready;
	HUINT16		usSCELL_SvcType[CAS_MAX_NUM_OF_SC];
	HUINT16		usSCELL_SvcHandle[CAS_MAX_NUM_OF_SC];

	IR_STATUS_INFO			GlobalStatus[CAS_MAX_NUM_OF_SC];
	IR_STATUS_INFO			InternalStatus[CAS_MAX_NUM_OF_SC];

	IR_STATUS_EMM_INFO		EmmRscStatus[CAS_MAX_NUM_OF_SC];
	IR_STATUS_INFO			SmartcardRscStatus[CAS_MAX_NUM_OF_SC];

	msg_dvb_emm_monitor_data_st	EmmMonitoring[CAS_MAX_NUM_OF_SC];
} IR_TP_INFO;


/********************************************************************
*   type definition
********************************************************************/
IR_TP_INFO			*CAS_IR_DRV_FindIrTpInfo(Handle_t hAction);
IR_TP_INFO			*CAS_IR_DRV_FindIrTpInfoBySCELL(HUINT16 usSCELLHandle);
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfo(Handle_t hIrSvc);
HINT32				CAS_IR_DRV_GetSelectedIrSvcCount(void);
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfoByContentsId(HUINT32 ulContentId);
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfoByPvrInfo(void *pvrInfo);
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfoBySCELLHandle(HUINT16 usSCELLHandle);
IR_SERVICE_INFO		*CAS_IR_DRV_FindIrSvcInfoByActionId(Handle_t hAction);
IR_SERVICE_INFO		*CAS_IR_DRV_FindEmptySvcInfo(void);
IR_SERVICE_INFO		*CAS_IR_DRV_GetIrSvcInfobyActHandle(Handle_t hAction);


HINT32		CAS_IR_DRV_GetIrSvcIndexFromPtr(IR_SERVICE_INFO *pstIrSvcInfo);
HIR_ERR		CAS_IR_DRV_GetSvcCasKind(Handle_t hAction, eIrDrvCaType *eSvcCasKind);
HIR_ERR		CAS_IR_DRV_GetSvcRecordMode (Handle_t hAction, HUINT8 *pucRecordMode);
HUINT32		XSVC_CAS_IR_DRV_GetScInfo(HUINT8 ucSlotID, IR_DRV_SC_INFO *IrScInfo);
HUINT32		XSVC_CAS_IR_DRV_CopyScInfo(HUINT8 ucSlotID, HUINT8 *pData);	// ewlee 20100817
#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER)
extern HBOOL	XSVC_CAS_IR_DRV_IsSameWithSavedSmartcardNumber();
extern HUINT32	XSVC_CAS_IR_DRV_GetSavedSmartcardNumber (HUINT8 *pData);
extern HBOOL	XSVC_CAS_IR_DRV_SavedNumberIsInitial ();
#endif

HBOOL		XSVC_CAS_IR_DRV_MakeNumericIRStatus(HUINT32 ulIRMessage, HUINT16 *pusStatus);
HBOOL		XSVC_CAS_IR_DRV_GetGlobalStatusOfSC(HUINT8 ucSlotId, HUINT32 *pucStatus);
ia_result		XSVC_CAS_IR_DRV_PinCodeRequest(HUINT8 ucSlotId, ia_pin_action_e ePinAction, ia_pin_type_e ePinType, HUINT16 usOldPin, HUINT16 usNewPin);

HUINT32		CAS_IR_DRV_GetIrSvcType(ia_word16 irSvcHandle);
HUINT8		CAS_IR_DRV_GetIrSvcInfoIDXbyActHandle(Handle_t hAction);
HIR_ERR		CAS_IR_DRV_GetActionHandleByDemuxId(HUINT8 in_ucDemuxId, Handle_t *out_hAct);
HIR_ERR		CAS_IR_DRV_GetDemuxIdByActionHandle(Handle_t in_hAct, HUINT8 *out_ucDemuxId);

void			CAS_IR_SVC_StartService(CAS_CAI_IF_msgPID	*pMsgPID);
void			CAS_IR_SVC_IamSelected(CAS_CAI_IF_msgCAINST_SELECT *pMsgSelect);
void			CAS_IR_SVC_IamDeSelected(CAS_CAI_IF_msgCAINST_SELECT *pMsgSelect);
void			CAS_IR_SVC_StopService(CAS_CAI_IF_msgPID	*pMsgPID);

void			CAS_IR_SVC_SetPID(CAS_CAI_IF_msgPIDList	*pMsgPIDList);
void			CAS_IR_SVC_SetPIDforRecording(IR_SERVICE_INFO *pstIrSvcInfo);
void			CAS_IR_SVC_StartTS(CAS_CAI_IF_msgTS	*pMsgTS);
void			CAS_IR_SVC_StopTS(CAS_CAI_IF_msgTS	*pMsgTS);

IR_SERVICE_INFO*	CAS_IR_DRV_GetLiveSvcInfo (void);

#endif
