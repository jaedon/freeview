/**************************************************************
 *	@file		xmgr_cas_ci.c
 *	common interface subcas main module
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/12/04
 *	@author			humax
 *	@brief
 **************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_cas.h>


#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_cas.h>
#include <xmgr_cas.h>

#include "./local_include/_xmgr_cas_ci.h"
#include "../../local_include/_xmgr_cas.h"

#if defined(CONFIG_MW_CI_PLUS)
#include "om_common.h"
#include <ci_plus.h>
#include <hlib.h>
#endif

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/


/*******************************************************************/
/********************      type define    **************************/
/*******************************************************************/
typedef enum
{
	eCiCasUi_FnMenuMmi = 0,
	eCiCasUi_FnZapMmi,
	eCiCasUi_FnSystemMessage,

#if defined (CONFIG_MW_CI_PLUS)
	eCiCasUi_FnCiPlusMessage,		// CI+CAM 관련 동작 중 MMI와 상관 없이 Host쪽에서 사용자에게 notify 하기 위한 UI 처리.
#endif

	eCiCasUi_FnMax
} XmgrCas_CiCasUiFn_e;

typedef enum
{
	eCiCasAct_FnCamTune = 0,
	eCiCasAct_FnCamUpgrade,
//#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	eCiCasAct_FnOperatorProfile,
//#endif

	eCiCasAct_FnMax
} XmgrCas_CiCasActFn_e;

typedef enum
{
	eCiCasUi_MmiNone,
	eCiCasUi_MmiForMenu,
	eCiCasUi_MmiForZap,

	eCiCasUi_MmiTypeMax
} XmgrCas_CiCasUiMmiType_e;

typedef struct
{
	XmgrCas_CiCasUiMmiType_e	eMmiType; //1 TODO:DELETE
	BUS_Callback_t				pfnUiCallback[eCiCasUi_FnMax];
} XmgrCas_CiCasUi_t;

typedef struct
{
	BUS_Callback_t			pfnActCallback[eCiCasAct_FnMax];
} XmgrCas_CiCasAct_t;


typedef struct
{
	HUINT32 					ulSlotNumber;
	XmgrCas_CiCamStatus_e 		eCamStatus;
	XmgrCas_CiCasUi_t			stCasCiUi;
	XmgrCas_CiCasAct_t			stCasCiAct;
} XmgrCas_CiCasMgrContext_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_CiCasMgrContext_t		_CiCasMgrContext[XMGR_CAS_CI_SLOT_MAX];

extern HERROR	OM_CAS_ACTION_RegisterCasProc(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasProc);
extern HERROR	OM_CAS_MMI_RegisterCasUiProc(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasUiProc);
/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____INLINE_FUNCTIONS______________________________________________________
INLINE XmgrCas_CiCasMgrContext_t	*xmgr_cas_CiGetCiCasMgrContext(HUINT32 ulSlotNumber)
{
	#if defined(CONFIG_DEBUG)
	HxLOG_Assert(ulSlotNumber < XMGR_CAS_CI_SLOT_MAX);
	#endif /* defined(CONFIG_DEBUG) */

	return &(_CiCasMgrContext[ulSlotNumber]);
}

#define _____LOCAL_UTIL_FUNCTIONS______________________________________________________
STATIC HERROR	xmgr_cas_CiGetActProcByMSG(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, BUS_Callback_t *ppReturnfnUiProc)
{
	BUS_Callback_t		pfnActProc = NULL;

	switch(message)
	{
		case eMEVT_CAS_ACTION_PREPARE_TUNE_FAIL:
			pfnActProc = pContext->stCasCiAct.pfnActCallback[eCiCasAct_FnOperatorProfile];
			break;
		default:
			HxLOG_Error("ERROR: check Message...\n");
			return ERR_FAIL;
	}

	(*ppReturnfnUiProc) = pfnActProc;

	return ERR_OK;
}

STATIC HERROR	xmgr_cas_CiGetSessionUIProc(XmgrCas_CiCasMgrContext_t *pContext, XmgrCas_CiCasUiFn_e eCiCasUiFn, BUS_Callback_t *ppReturnfnUiProc,
	HINT32 nVersion)
{
	HERROR				hError = ERR_FAIL;
	HCHAR				*pzProcName = NULL;
	HINT32				param1 = 0;
	HINT32				param2 = 0;
	HINT32				param3 = 0;
	BUS_Callback_t		pfnUiProc = NULL;
	BUS_Result_t		hBusResult = ERR_BUS_NO_ERROR;
	DxCAS_MmiSessionType_e eSessionType;

	HxLOG_Info("Func Enter...\n");

	switch(eCiCasUiFn)
	{
		case eCiCasUi_FnSystemMessage:
			eSessionType = eDxCAS_SESSIONTYPE_BANNER;
			pfnUiProc = pContext->stCasCiUi.pfnUiCallback[eCiCasUi_FnSystemMessage];
			if(pfnUiProc != NULL)
			{
				if(BUS_MGR_Get(pfnUiProc) == NULL)
				{
					pzProcName = "xproc_cas_CiUiSysMsg";
					param1 = (HINT32)pContext->ulSlotNumber;
					param2 = (HINT32)eSessionType;
					param3 = 0;
					hBusResult = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pfnUiProc, 0, param1, param2, param3);
					if(hBusResult != ERR_BUS_NO_ERROR)
					{
						HxLOG_Info("bus result %d\n", hBusResult);
					}
				}

				(*ppReturnfnUiProc) = pfnUiProc;
				hError = ERR_OK;
			}
			break;

		case eCiCasUi_FnMenuMmi:
			eSessionType = eDxCAS_SESSIONTYPE_MENU;
			pfnUiProc = pContext->stCasCiUi.pfnUiCallback[eCiCasUi_FnMenuMmi];
			if(pfnUiProc != NULL)
			{
				if(BUS_MGR_Get(pfnUiProc) == NULL)
				{
					pzProcName = "xproc_cas_CiMmi";
					param1 = (HINT32)pContext->ulSlotNumber;
					param2 = (HINT32)eSessionType;
					param3 = nVersion;
					hBusResult = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pfnUiProc, 0, param1, param2, param3);
					if(hBusResult != ERR_BUS_NO_ERROR)
					{
						HxLOG_Info("bus result %d\n", hBusResult);
					}
				}

				(*ppReturnfnUiProc) = pfnUiProc;

				hError = ERR_OK;
			}
			break;

		case eCiCasUi_FnZapMmi:
			eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
			pfnUiProc = pContext->stCasCiUi.pfnUiCallback[eCiCasUi_FnZapMmi];
			if(pfnUiProc != NULL)
			{
				if(BUS_MGR_Get(pfnUiProc) == NULL)
				{
					pzProcName = "xproc_cas_CiMmi";
					param1 = (HINT32)pContext->ulSlotNumber;
					param2 = (HINT32)eSessionType;
					param3 = nVersion;				// Version을 올려주기는 하나, create시 이 내용은 사용될 필요 없음.
					hBusResult = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pfnUiProc, 0, param1, param2, param3);
					if(hBusResult != ERR_BUS_NO_ERROR)
					{
						HxLOG_Info("bus result %d\n", hBusResult);
					}
				}

				(*ppReturnfnUiProc) = pfnUiProc;

				hError = ERR_OK;
			}
			break;

#if defined (CONFIG_MW_CI_PLUS)
		case eCiCasUi_FnCiPlusMessage:
			eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
			pfnUiProc = pContext->stCasCiUi.pfnUiCallback[eCiCasUi_FnCiPlusMessage];
			if(pfnUiProc != NULL)
			{
				if(BUS_MGR_Get(pfnUiProc) == NULL)
				{
					pzProcName = "xproc_cas_CiPlusSysMsg";
					param1 = (HINT32)pContext->ulSlotNumber;
					param2 = (HINT32)eSessionType;
					param3 = nVersion;
					hBusResult = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pfnUiProc, 0, param1, param2, param3);
					if(hBusResult != ERR_BUS_NO_ERROR)
					{
						HxLOG_Info("bus result %d\n", hBusResult);
					}
				}

				(*ppReturnfnUiProc) = pfnUiProc;

				hError = ERR_OK;
			}
			break;
#endif
		case eCiCasUi_FnMax:
		default:
			HxLOG_Error("\n\n");
			break;
	}

	return hError;
}

STATIC HERROR	xmgr_cas_CiTransitionCAMState(XmgrCas_CiCasMgrContext_t *pContext, XmgrCas_CiCamStatus_e eNextState)
{
	XmgrCas_CiCamStatus_e		eCurrentState = pContext->eCamStatus;

	HxLOG_Info("current[%d], next[%d]\n", eCurrentState, eNextState);

	switch(eCurrentState)
	{
		case eCiCas_CamRemoved:
			/* CAM이 꽂힌 상태에서 Booting시 Inserted message를 받지 못하는 경우가 발생한다. */
			if(eNextState != eCiCasUi_CamStatusMax)
			{
				pContext->eCamStatus = eNextState;
			}
			break;

		case eCiCas_CamInserted:
			switch(eNextState)
			{
				case eCiCas_CamRemoved:
					pContext->eCamStatus = eNextState;
					break;

				case eCiCas_CamInserted:
					break;

				case eCiCas_CamInitialized:
					pContext->eCamStatus = eNextState;
					break;

				case eCiCasUi_CamStatusMax:
				default:
					break;
			}
			break;

		case eCiCas_CamInitialized:
			if(eNextState == eCiCas_CamRemoved)
			{
				pContext->eCamStatus = eNextState;
			}
			break;

		case eCiCasUi_CamStatusMax:
		default:
			break;
	}

	return ERR_OK;
}








#if defined(CONFIG_MW_CI_PLUS)


// TODO: 현재는 여기서 cb을 꼽고, cb에 올라온 data를 bus에 달아 보내지만,
// TODO: mheg 엔진에 갈 데이터는 bus thread를 탈 필요도 없고, SERIALIZER 사용하기도 복잡하며, data 전달의 부담이 있으므로,
// TODO: xom_cas_ci에서 바로 cb을 꼽아 사용해도 무관할 듯 하다.
STATIC int xmgr_cas_Cb_appMmiMessage(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT *pustContent)
{
	const HCHAR *szText;
	HUINT32		ulSize;
	void 		*pvData;
	void		*outbuf = NULL;
	HINT32		nSrzr;
#if 0		// not used
	HINT32		nSrzrParam;
#endif

	HxLOG_Trace();

	switch(enMsgType)
	{
		case CI_AMMI_MSG_REQ_START:
			nSrzr = HLIB_SERIALIZER_Open();
			HLIB_SERIALIZER_MakeSerializedData(nSrzr,
										"ibb",
										(HINT32)pustContent->typeReqStart.usSessionId,
										pustContent->typeReqStart.pucAppDomainId,
										(HINT32)pustContent->typeReqStart.ucAppDomainIdLen,
										pustContent->typeReqStart.pucInitialObject,
										(HINT32)pustContent->typeReqStart.ucInitialObjectLen);

			if ((szText = HLIB_SERIALIZER_Error(nSrzr)) != NULL)
			{
				HxLOG_Error("[%s:%d] Serialize failed : %s\n", szText);

				return 0;
			}
			pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);
			outbuf = HLIB_STD_MemDup(pvData, ulSize);
			HLIB_SERIALIZER_Close(nSrzr);
			BUS_PostDataToOM(NULL, eMEVT_XMGR_CAS_CI_AMMI_REQ_START, HANDLE_NULL, enMsgType, ulSize, outbuf, ulSize);

			break;

		case CI_AMMI_MSG_FILE_ACK:
			nSrzr = HLIB_SERIALIZER_Open();
			// unsinged long이나 unsigned int 나 둘다 32 bit.
			HLIB_SERIALIZER_MakeSerializedData(nSrzr,
										"iiib",
										(HINT32)pustContent->typeFileAck.usSessionId,
										(HINT32)pustContent->typeFileAck.ucFileOK,
										(HINT32)pustContent->typeFileAck.enReqType,
										pustContent->typeFileAck.pucBytes,
										(HINT32)pustContent->typeFileAck.ulBytesLen);

			if ((szText = HLIB_SERIALIZER_Error(nSrzr)) != NULL)
			{
				HxLOG_Error("Serialize failed : %s\n", szText);

				return 0;
			}
			pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);
			outbuf = HLIB_STD_MemDup(pvData, ulSize);
			HLIB_SERIALIZER_Close(nSrzr);
			BUS_PostDataToOM(NULL, eMEVT_XMGR_CAS_CI_AMMI_FILE_ACK, HANDLE_NULL, enMsgType, ulSize, outbuf, ulSize);
			break;

		case CI_AMMI_MSG_REQ_APP_ABORT:
			nSrzr = HLIB_SERIALIZER_Open();
			HLIB_SERIALIZER_MakeSerializedData(nSrzr,
										"ii",
										(HINT32)pustContent->typeReqAppAbort.usSessionId,
										(HINT32)pustContent->typeReqAppAbort.enAbortReqCode);

			if ((szText = HLIB_SERIALIZER_Error(nSrzr)) != NULL)
			{
				HxLOG_Error("Serialize failed : %s\n", szText);

				return 0;
			}
			pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);
			outbuf = HLIB_STD_MemDup(pvData, ulSize);
			HLIB_SERIALIZER_Close(nSrzr);
			BUS_PostDataToOM(NULL, eMEVT_XMGR_CAS_CI_AMMI_REQ_APP_ABORT, HANDLE_NULL, enMsgType, ulSize, outbuf, ulSize);
			break;

		case CI_AMMI_MSG_APP_ABORT_ACK:
			nSrzr = HLIB_SERIALIZER_Open();
			HLIB_SERIALIZER_MakeSerializedData(nSrzr,
										"i",
										(HINT32)pustContent->typeAppAbortAck.usSessionId);
			if ((szText = HLIB_SERIALIZER_Error(nSrzr)) != NULL)
			{
				HxLOG_Error("Serialize failed : %s\n", szText);

				return 0;
			}
			pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);
			outbuf = HLIB_STD_MemDup(pvData, ulSize);
			HLIB_SERIALIZER_Close(nSrzr);
			BUS_PostDataToOM(NULL, eMEVT_XMGR_CAS_CI_AMMI_APP_ABORT_ACK, HANDLE_NULL, enMsgType, ulSize, outbuf, ulSize);
			break;

		default:
			HxLOG_Error("ERROR : invalid message type (%d) \n", enMsgType);
			return 0;
	}

	if(outbuf != NULL)
	{
		HLIB_STD_MemFree(outbuf);
		outbuf = NULL;
	}

	//	return CI_AMMI_REQUEST_START_ACK_CODE_OK;
	return 1;
}
#endif

STATIC BUS_Result_t	xmgr_cas_CiMsgBusCreate(XmgrCas_CiCasMgrContext_t *pContext, HUINT32 ulSlotNumber)
{
	HxSTD_MemSet(pContext, 0x00, sizeof(XmgrCas_CiCasMgrContext_t));

	pContext->ulSlotNumber = ulSlotNumber;
	pContext->eCamStatus = eCiCas_CamRemoved;
	pContext->stCasCiUi.eMmiType = eCiCasUi_MmiNone;

	pContext->stCasCiUi.pfnUiCallback[eCiCasUi_FnMenuMmi] = (BUS_Callback_t)xproc_cas_CiMmi;
	pContext->stCasCiUi.pfnUiCallback[eCiCasUi_FnZapMmi] = (BUS_Callback_t)xproc_cas_CiMmi;
	pContext->stCasCiUi.pfnUiCallback[eCiCasUi_FnSystemMessage] = (BUS_Callback_t)xproc_cas_CiUiSysMsg;
#if defined(CONFIG_MW_CI_PLUS)
	pContext->stCasCiUi.pfnUiCallback[eCiCasUi_FnCiPlusMessage] = (BUS_Callback_t)xproc_cas_CiPlusSysMsg; // CI+ 관련 동작 사항에 대해 Host쪽에서 사용자에게 notify 하기 위한 UI 처리.
	pContext->stCasCiAct.pfnActCallback[eCiCasAct_FnCamTune] = (BUS_Callback_t)xproc_cas_CiTuneMgr;
	pContext->stCasCiAct.pfnActCallback[eCiCasAct_FnCamUpgrade] = (BUS_Callback_t)xproc_cas_CiCamUpgrade;
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	pContext->stCasCiAct.pfnActCallback[eCiCasAct_FnOperatorProfile] = (BUS_Callback_t)xproc_cas_CiOperatorProfile;
#endif
#endif

#if defined(CONFIG_MW_CI_PLUS)
	CI_RegisterAppMmiCallback(xmgr_cas_Cb_appMmiMessage);
#endif

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CiEvtCiCamRemoved(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		hBusResult = ERR_BUS_NO_ERROR;
	XmgrCas_CiCasUiFn_e		eFnIdxLoop;
	BUS_Callback_t		pfnUiCallback = NULL;

	HxLOG_Info("slotNumber %d previous cam status %d\n", pContext->ulSlotNumber, pContext->eCamStatus);

	xmgr_cas_CiTransitionCAMState(pContext, eCiCas_CamRemoved);

	for(eFnIdxLoop = eCiCasUi_FnMenuMmi;eFnIdxLoop < eCiCasUi_FnMax;eFnIdxLoop++)
	{
		pfnUiCallback = pContext->stCasCiUi.pfnUiCallback[eFnIdxLoop];
		if((pfnUiCallback != NULL) && (BUS_MGR_Get(pfnUiCallback) != NULL))
		{
			hBusResult = BUS_SendMessage(pfnUiCallback, message, hAction, (HINT32)pContext->ulSlotNumber, (HINT32)pContext->eCamStatus, p3);
			if(hBusResult != ERR_BUS_NO_ERROR)
			{
				HxLOG_Info("bus result %d\n", hBusResult);
			}

			//1 TODO : BUS disconnect
			/* BUS_MGR_Destroy(pfnUiCallback) */
		}
	}

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_SYSCTL_CI_CAM_STATUS_EXTRACTED, HANDLE_NULL, (HINT32)pContext->ulSlotNumber, (HINT32)pContext->eCamStatus, 0);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Info("bus result %d\n", hBusResult);
	}

	return MESSAGE_BREAK;
}


STATIC BUS_Result_t xmgr_cas_CiEvtCiChangeState(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	BUS_Callback_t			pfnUiProc = NULL;
	XmgrCas_CiCasUiFn_e 	eCiCasUiFn = eCiCasUi_FnSystemMessage;
	HINT32					event = 0;

	HxLOG_Info("slotNumber %d previous cam status %d message %d\n", pContext->ulSlotNumber, pContext->eCamStatus, message);

	switch(message)
	{
		case eMEVT_CAS_CAM_INSERTED:
			xmgr_cas_CiTransitionCAMState(pContext, eCiCas_CamInserted);
			event = eMEVT_SYSCTL_CI_CAM_STATUS_INSERTED;
			break;

		case eMEVT_CAS_CAM_INITIALIZED:
			xmgr_cas_CiTransitionCAMState(pContext, eCiCas_CamInitialized);
			event = eMEVT_SYSCTL_CI_CAM_STATUS_INITIALIZED;
			break;

		default:
			HxLOG_Error("\n\n");
			return ERR_BUS_OUT_OF_RANGE;
	}

	HxLOG_Info("pContext->eCamStatus: [%d]\n", pContext->eCamStatus);

	xmgr_cas_CiGetSessionUIProc(pContext, eCiCasUiFn, &pfnUiProc, p3);

	if(pfnUiProc != NULL)
	{
		hBusResult = BUS_SendMessage(pfnUiProc, message, hAction, (HINT32)pContext->ulSlotNumber, (HINT32)pContext->eCamStatus, p3);
		if(hBusResult != ERR_BUS_NO_ERROR)
		{
			HxLOG_Info("bus result %d\n", hBusResult);
		}
	}

	hBusResult = BUS_PostMessageToOM(NULL, event, HANDLE_NULL, (HINT32)pContext->ulSlotNumber, (HINT32)pContext->eCamStatus, 0);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Info("bus result %d\n", hBusResult);
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CiEvtCiMmi(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t		pfnUiCallback = NULL;
	BUS_Result_t		hRes = ERR_BUS_MESSAGE_BREAK;
	XmgrCas_CiCasUiFn_e 	eCiCasUiFn = eCiCasUi_FnMax;

	HxLOG_Print("(message : %08x), (slot id : %d), (Group ID : 0x%04x), (MMI Ver : %d)\n",
		message, p1, p2, p3);

	switch(pContext->stCasCiUi.eMmiType)
	{
		case eCiCasUi_MmiForMenu:
			eCiCasUiFn = eCiCasUi_FnMenuMmi;
			break;

		default:
			eCiCasUiFn = eCiCasUi_FnZapMmi;
			break;
	}

	xmgr_cas_CiGetSessionUIProc(pContext, eCiCasUiFn, &pfnUiCallback, p3);
	if(pfnUiCallback != NULL)
	{
		hRes = BUS_SendMessage(pfnUiCallback, message, hAction, p1, p2, p3);
		return hRes;
	}

	return ERR_BUS_MESSAGE_BREAK;
}

#if defined(CONFIG_MW_CI_PLUS)
STATIC BUS_Result_t xmgr_cas_CiEvtCasCiPlusCamTune(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	DxCAS_MmiSessionType_e		eSessionType;
	BUS_Callback_t			pfnActProc = NULL;
	HCHAR					*pzProcName = NULL;

	BUS_Result_t			hRes = ERR_BUS_MESSAGE_BREAK;

	pfnActProc = pContext->stCasCiAct.pfnActCallback[eCiCasAct_FnCamTune];
	if(pfnActProc != NULL)
	{
		if(BUS_MGR_Get(pfnActProc) == NULL)
		{
			pzProcName = "APCASACT_MGRPROC_CamTuneMessage";
			hRes = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pfnActProc, 0, p1, 0, 0); // p1은 slot number 이고 (HINT32)pContext->ulSlotNumber 값과 같아야 함.
			if(hRes != ERR_BUS_NO_ERROR)
			{
				HxLOG_Info("bus result %d\n", hRes);
			}
		}

		hRes = BUS_SendMessage(pfnActProc, message, hAction, p1, (HINT32)pContext->eCamStatus, p3);
		// p1은 slot number 이고 (HINT32)pContext->ulSlotNumber 값과 같아야 함.
		// 두번째 인자 값 (HINT32)pContext->eCamStatus는 보내긴 하지만 사실 쓰이지는 않음.
		// p3는 svc_cas.h 파일에 정의되어 있는 SvcCas_TuneInfo_t 구조체의 주소 임.
		if(hRes != ERR_BUS_NO_ERROR)
		{
			HxLOG_Info("bus result %d\n", hRes);
		}
	}
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_CiEvtCasCiPlusCcReport(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_CiCasUiFn_e		eCiCasUiFn;
	BUS_Callback_t			pfnUiProc = NULL;
	BUS_Result_t			hRes = ERR_BUS_MESSAGE_BREAK;

	eCiCasUiFn = eCiCasUi_FnCiPlusMessage;

	xmgr_cas_CiGetSessionUIProc(pContext, eCiCasUiFn, &pfnUiProc, p3);
	if(pfnUiProc != NULL)
	{
		hRes = BUS_SendMessage(pfnUiProc, message, hAction, (HINT32)pContext->ulSlotNumber, (HINT32)pContext->eCamStatus, p3);
		if(hRes != ERR_BUS_NO_ERROR)
		{
			HxLOG_Info("bus result %d\n", hRes);
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_CiEvtCasCiPlusCamUpgrade(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t			pfnActProc = NULL;
	HCHAR					*pzProcName = NULL;

	BUS_Result_t			hRes = ERR_BUS_MESSAGE_BREAK;

	pfnActProc = pContext->stCasCiAct.pfnActCallback[eCiCasAct_FnCamUpgrade];
	if(pfnActProc != NULL)
	{
		if(BUS_MGR_Get(pfnActProc) == NULL)
		{
			pzProcName = "APCASACT_MGRPROC_CamUpgradeMessage";
			hRes = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pfnActProc, 0, p1, 0, 0); // p1은 slot number 이고 (HINT32)pContext->ulSlotNumber 값과 같아야 함.
			if(hRes != ERR_BUS_NO_ERROR)
			{
				HxLOG_Info("bus result %d\n", hRes);
			}
		}

		hRes = BUS_SendMessage(pfnActProc, message, hAction, p1, (HINT32)pContext->eCamStatus, p3);
		// p1은 slot number 이고 (HINT32)pContext->ulSlotNumber 값과 같아야 함.
		// 두번째 인자 값 (HINT32)pContext->eCamStatus는 보내긴 하지만 사실 쓰이지는 않음.
		// p3는 svc_cas.h 파일에 정의되어 있는 SvcCas_TuneInfo_t 구조체의 주소 임.
		if(hRes != ERR_BUS_NO_ERROR)
		{
			HxLOG_Info("bus result %d\n", hRes);
		}
	}
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_CiEvtCasCiPlusOperatorProfile(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t			pfnActProc = NULL;
	HCHAR					*pzProcName = NULL;

	BUS_Result_t			hRes = ERR_BUS_MESSAGE_BREAK;

	switch(message)
	{
		case eMEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ:
			pfnActProc = pContext->stCasCiAct.pfnActCallback[eCiCasAct_FnOperatorProfile];
			break;

		case eMEVT_CAS_CIPLUS_OPERATOR_TUNE:
		case eMEVT_CAS_CIPLUS_OPERATOR_NIT:
			if(p3 == TRUE)
			{
				hRes = BUS_MGR_Create("APCASACT_MGRPROC_CiOpSearchOperatorProfile", APP_CAS_PRIORITY, xproc_cas_CiOpSearchOperatorProfile, hAction, p1, p2, TRUE);
				if(hRes != ERR_BUS_NO_ERROR)
				{
					HxLOG_Info("bus result %d\n", hRes);
				}
			}

			pfnActProc = xproc_cas_CiOpSearchOperatorProfile;
			if(BUS_MGR_Get(pfnActProc) == NULL)
			{
				HxLOG_Error("BUS_MGR_Get() error\n");
				return ERR_BUS_MESSAGE_BREAK;
			}
			break;

		default:
			return ERR_BUS_MESSAGE_PASS;
	}

	if(pfnActProc != NULL)
	{
		if(BUS_MGR_Get(pfnActProc) == NULL)
		{
			pzProcName = "APCASACT_MGRPROC_CiOperatorProfile";
			hRes = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pfnActProc, hAction, p1, p2, p3);
			if(hRes != ERR_BUS_NO_ERROR)
			{
				HxLOG_Info("bus result %d\n", hRes);
			}
		}

		hRes = BUS_SendMessage(pfnActProc, message, hAction, p1, p2, p3);
		if(hRes != ERR_BUS_NO_ERROR)
		{
			HxLOG_Info("bus result %d\n", hRes);
		}
	}

	return ERR_BUS_NO_ERROR;
}

#endif

STATIC BUS_Result_t xmgr_cas_CiEvtCiPlus(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;

	switch(message)
	{
		case eMEVT_CAS_CAM_TUNE:
#if defined(CONFIG_MW_CI_PLUS)
			hBusResult = xmgr_cas_CiEvtCasCiPlusCamTune(pContext, message, hAction, p1, p2, p3);
			if (hBusResult != ERR_BUS_NO_ERROR)
			{
				HxLOG_Error ("MESSAGE ERROR : eMEVT_CAS_CAM_TUNE(slot id : %d), (CAS ID : 0x%04x), (Ver : %d)\n", p1, p2, p3);
			}
#endif
			HxLOG_Print("MESSAGE : eMEVT_CAS_CAM_TUNE(slot id : %d), (CAS ID : 0x%04x), , (Tune Data : 0x%x)\n", p1, p2, p3);
			break;

		case eMEVT_CAS_CIPLUS_CC_REPORT:
#if defined(CONFIG_MW_CI_PLUS)
			hBusResult = xmgr_cas_CiEvtCasCiPlusCcReport(pContext, message, hAction, p1, p2, p3);
			if (hBusResult != ERR_BUS_NO_ERROR)
			{
				HxLOG_Error ("MESSAGE ERROR : eMEVT_CAS_CIPLUS_CC_REPORT(slot id : %d), (CAS ID : 0x%04x), (Ver : %d)\n", p1, p2, p3);
			}
#endif
			HxLOG_Print("MESSAGE : eMEVT_CAS_CIPLUS_CC_REPORT(slot id : %d), (CAS ID : 0x%04x), (Ver : %d)\n", p1, p2, p3);
			break;

		case eMEVT_CAS_CIPLUS_UPGRADE:
#if defined(CONFIG_MW_CI_PLUS)
			hBusResult = xmgr_cas_CiEvtCasCiPlusCamUpgrade(pContext, message, hAction, p1, p2, p3);
			if (hBusResult != ERR_BUS_NO_ERROR)
			{
				HxLOG_Error ("MESSAGE ERROR : eMEVT_CAS_CIPLUS_UPGRADE(slot id : %d), (CAS ID : 0x%04x), (Ver : %d)\n", p1, p2, p3);
			}
#endif

			HxLOG_Print("MESSAGE : eMEVT_CAS_CIPLUS_UPGRADE(slot id : %d), (CAS ID : 0x%04x), (Ver : %d)\n", p1, p2, p3);
			break;

		case eMEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ:
		case eMEVT_CAS_CIPLUS_OPERATOR_TUNE:
		case eMEVT_CAS_CIPLUS_OPERATOR_NIT:
			hBusResult = xmgr_cas_CiEvtCasCiPlusOperatorProfile(pContext, message, hAction, p1, p2, p3);
			if (hBusResult != ERR_BUS_NO_ERROR)
			{
				;
			}
			break;

		default:
			break;
	}

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CiMsgOpenSession(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, OxMgrCasSessionEvent_t *pstSession)
{
	BUS_Callback_t		pfnUiCallback = NULL;

	HxLOG_Info("Func Enter...\n");

	if (pstSession->ulUiHandle == 0)
	{
		switch(pstSession->eSessionType)
		{
			case eDxCAS_SESSIONTYPE_BANNER:
				xmgr_cas_CiGetSessionUIProc(pContext, eCiCasUi_FnSystemMessage, &pfnUiCallback, 0);
				break;
			case eDxCAS_SESSIONTYPE_MENU:
				xmgr_cas_CiGetSessionUIProc(pContext, eCiCasUi_FnMenuMmi, &pfnUiCallback, 0);
				break;
			case eDxCAS_SESSIONTYPE_EXTENSION:
				HxLOG_Critical("Error - Cannot get callback function!! It must be fixed!!\n");
				HxLOG_Assert(FALSE);
				break;
			default:
				HxLOG_Error("Error - Wrong Session Type... [%d]!!\n", pstSession->eSessionType);
				pfnUiCallback = NULL;
				break;
		}
	}
	else
	{
		pfnUiCallback = (BUS_Callback_t)pstSession->ulUiHandle;
	}

	if(pfnUiCallback != NULL)
	{
		BUS_SendMessage(pfnUiCallback, message, hAction, 0, 0, (HINT32)pstSession);
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t	xmgr_cas_CiMsgCloseSession(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, OxMgrCasSessionEvent_t *pstSession)
{
	BUS_Callback_t		pfnUiCallback = NULL;

	pfnUiCallback = (BUS_Callback_t)pstSession->ulUiHandle;
	if(pfnUiCallback != NULL)
	{
		BUS_SendMessage(pfnUiCallback, message, hAction, 0, 0, (HINT32)pstSession);
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CiMsgCasUiInputNotify(XmgrCas_CiCasMgrContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t				pfnUiCallback = NULL;
//	DxCAS_MmiSessionType_e		eSessionType = (DxCAS_MmiSessionType_e)p2;
	OxMgrCasUiUserInputNotify_t	*pstUserInputNotify = NULL;

	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	pfnUiCallback = (BUS_Callback_t)pstUserInputNotify->ulUiHandle;
	if(pfnUiCallback != NULL)
	{
		BUS_SendMessage(pfnUiCallback, message, hAction, p1, p2, p3);
	}

	return ERR_BUS_MESSAGE_BREAK;
}


#define _____LOCAL_EVENT_PROC_FUNCTIONS______________________________________________________
STATIC BUS_Result_t xproc_cas_CiMsgBus(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hError = ERR_OK;
	HUINT32				ulMaxSlotNumber = 0, i;
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	XmgrCas_CiCasMgrContext_t 	*pContext = (XmgrCas_CiCasMgrContext_t *)NULL;

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("eMEVT_BUS_CREATE...\n");
			SVC_CAS_GetMaxNumOfCISlot(&ulMaxSlotNumber);
			for(i = 0;i < ulMaxSlotNumber;i++)
			{
				pContext = xmgr_cas_CiGetCiCasMgrContext(i);
				eBusResult = xmgr_cas_CiMsgBusCreate(pContext, i);
			}

			hError = OM_CAS_MMI_RegisterCasUiProc(eDxCAS_GROUPID_CAM, (BUS_Callback_t)xproc_cas_CiMain);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
			hError = OM_CAS_ACTION_RegisterCasProc(eDxCAS_GROUPID_CAM, (BUS_Callback_t)xproc_cas_CiMain);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Debug("eMEVT_BUS_DESTROY(0x%x)\n", message);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			return ERR_BUS_MESSAGE_PASS;

		default:
			break;
	}

	return eBusResult;
}

STATIC BUS_Result_t xproc_cas_CiEvt(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				ulSlotNumber = 0;
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	XmgrCas_CiCasMgrContext_t 	*pContext = (XmgrCas_CiCasMgrContext_t *)NULL;

	switch(message)
	{
		case eMEVT_CAS_CAM_REMOVED:
			ulSlotNumber = (HUINT32)p1;
			pContext = xmgr_cas_CiGetCiCasMgrContext(ulSlotNumber);
			eBusResult = xmgr_cas_CiEvtCiCamRemoved(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CAM_INSERTED:
		case eMEVT_CAS_CAM_INITIALIZED:
			ulSlotNumber = (HUINT32)p1;
			pContext = xmgr_cas_CiGetCiCasMgrContext(ulSlotNumber);
			eBusResult = xmgr_cas_CiEvtCiChangeState(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_MMI_OPEN:
		case eMEVT_CAS_MMI_CLOSE:
		case eMEVT_CAS_MMI_TEXT:
		case eMEVT_CAS_MMI_ENQ:
		case eMEVT_CAS_MMI_MENU:
		case eMEVT_CAS_MMI_LIST:
			ulSlotNumber = (HUINT32)p1;
			pContext = xmgr_cas_CiGetCiCasMgrContext(ulSlotNumber);
			eBusResult = xmgr_cas_CiEvtCiMmi(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CAM_TUNE:
		case eMEVT_CAS_CIPLUS_CC_REPORT:
		case eMEVT_CAS_CIPLUS_UPGRADE:
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		case eMEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ:
		case eMEVT_CAS_CIPLUS_OPERATOR_TUNE:
		case eMEVT_CAS_CIPLUS_OPERATOR_NIT:
#endif
			ulSlotNumber = (HUINT32)p1;
			pContext = xmgr_cas_CiGetCiCasMgrContext(ulSlotNumber);
			eBusResult = xmgr_cas_CiEvtCiPlus(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_FAIL :
			HxLOG_Debug("eMEVT_CAS_FAIL(0x%x)\n", message);
			break;
		case eMEVT_CAS_CMD :
		case eMEVT_CAS_CHECK :
		case eMEVT_CAS_OK :
			HxLOG_Debug("MGR_EVT_CAS(0x%x)\n", message);
			break;

		default:
			break;
	}

	return eBusResult;
}

STATIC BUS_Result_t xproc_cas_CiMsgMgr(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				ulSlotNumber = 0;
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	XmgrCas_CiCasMgrContext_t 	*pContext = (XmgrCas_CiCasMgrContext_t *)NULL;
	BUS_Callback_t		pfnCallback;
	HERROR				hErr = ERR_OK;
	OxMgrCasSessionEvent_t *pstSessionEvent = NULL;

	switch(message)
	{
		case eMEVT_CAS_ACTION_PREPARE_TUNE_FAIL:
			ulSlotNumber = (HUINT32)p1;
			pContext = xmgr_cas_CiGetCiCasMgrContext(ulSlotNumber);
			hErr = xmgr_cas_CiGetActProcByMSG(pContext, message, &pfnCallback);
			if(hErr != ERR_OK)
			{
				HxLOG_Critical("Fail to get CI action Proc... xmgr_cas_CiGetActProcByMSG...\n\n");
				return ERR_BUS_MESSAGE_BREAK;
			}
			BUS_SendMessage(pfnCallback, message, hAction, ulSlotNumber, 0, 0);
			break;
		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_OPEN_SESSION...\n");
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			pContext = xmgr_cas_CiGetCiCasMgrContext(pstSessionEvent->ulCasSlotNum);
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_CiMsgOpenSession(pContext, message, hAction, pstSessionEvent);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_CLOSE_SESSION...\n");
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			pContext = xmgr_cas_CiGetCiCasMgrContext(pstSessionEvent->ulCasSlotNum);
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_CiMsgCloseSession(pContext, message, hAction, pstSessionEvent);
			}
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_USERINPUTDATA...\n");
			ulSlotNumber = (HUINT32)p1;
			pContext = xmgr_cas_CiGetCiCasMgrContext(ulSlotNumber);
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_CiMsgCasUiInputNotify(pContext, message, hAction, p1, p2, p3);
			}
			break;
/*
		case MSG_OAPI_CASUI_CHANGE_LANGUAGE:
			HxLOG_Debug("MSG_OAPI_CASUI_CHANGE_LANGUAGE(0x%x)\n", message);
			break;
*/
		default:
			break;
	}

	return eBusResult;
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_CI_PUBLIC_FUNCTION______________________________________________________

HERROR			xproc_cas_CiDestroyCamTune(HUINT32 unSlotNumber)
{
	XmgrCas_CiCasMgrContext_t 	*pContext = (XmgrCas_CiCasMgrContext_t *)NULL;
	BUS_Callback_t		pfnActProc = NULL;

	pContext = xmgr_cas_CiGetCiCasMgrContext(unSlotNumber);
	pfnActProc = pContext->stCasCiAct.pfnActCallback[eCiCasAct_FnCamTune];

	if(BUS_MGR_Get(pfnActProc) != NULL)
	{
		BUS_MGR_Destroy(pfnActProc);
	}

	return ERR_OK;
}

BUS_Result_t 	xproc_cas_CiMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;

	switch(message&(OBAMA_EVENT_MANAGER_START|0x00F00000))
	{
		case eMEVT_BUS_START:
			eBusResult = xproc_cas_CiMsgBus(message, hAction, p1, p2, p3);
			break;

		case eMEVT_MGR_START:
			if (message > eMEVT_CAS_CASUI)
				eBusResult = xproc_cas_CiMsgMgr(message, hAction, p1, p2, p3);
			else
				eBusResult = xproc_cas_CiEvt(message, hAction, p1, p2, p3);
			break;

		default:
			break;
	}

	switch(message)
	{
		case eOEVT_CAS_CI_REQUESTFILEORDATA:
			HxLOG_Debug("eOEVT_CAS_CI_REQUESTFILEORDATA \n");
			CI_RequestFileorData((unsigned short)p1, (CI_AMMI_REQUEST_TYPE)p2, (unsigned char *)p3, (unsigned long)HxSTD_StrLen((const char *)p3));
			break;

		case eOEVT_CAS_CI_REQUESTAPPABORT:
			HxLOG_Debug("eOEVT_CAS_CI_REQUESTAPPABORT \n");
			CI_RequestAppAbort((unsigned short)p1, (CI_AMMI_ABORT_REQUEST_CODE)p2);
			break;

		default:
			break;

	}

	if(eBusResult == ERR_BUS_NO_ERROR)
	{
		eBusResult = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return eBusResult;
}

/* EOF */

