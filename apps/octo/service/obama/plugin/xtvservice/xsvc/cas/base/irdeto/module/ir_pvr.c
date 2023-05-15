
#include "vd_humax.h"

#include "ir_svc.h"
#include "ir_msg_scell.h"
#include "ir_main.h"
#include "ir_evt.h"
#include "ir_pvr.h"

#include <svc_cas.h>
#include <pal_pipe.h>
//#include <mwc_util.h>
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>

#include <_svc_cas_mgr.h>
#include <_svc_cas_mgr_common.h>

/********************************************************************
*	Definitions (for this source file)
********************************************************************/






//---------------------------- Struct Define ----------------------------
/********************************************************************
*	Type Definition
********************************************************************/

#if	defined(IRPLAT_SYS_PVR)
//#define	PrintMSKMode(a)				HxLOG_Print("\t\t===========================\n\t\t     %s\n\t\t===========================\n", a)
#define	PrintMSKMode(...)

#define	SET_CRYPTOKEY_IN_VD_CALL


#if	defined(SET_CRYPTOKEY_IN_VD_CALL)
#define	STATE_CRYPTO_VD_OK			0x01
#define	STATE_CRYPTO_PVR_OK			0x02
#define	STATE_CRYPTO_OK				0x03

/********************************************************************
*	Internal variables
********************************************************************/

#if	1		//	key test
static	HUINT8		_secured_cssk_data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6};
static	HUINT8		*_secured_cssk = _secured_cssk_data;//NULL;
#else
static	HUINT8		_secured_cssk_data[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static	HUINT8		*_secured_cssk = NULL;
#endif


/********************************************************************
*	Global variables
********************************************************************/
extern	IR_SERVICE_INFO		g_IrSvcInfo[];
extern	IR_TP_INFO			g_IrTpInfo[];


/********************************************************************
*	Internal function prototypes
********************************************************************/

#if	defined(IRPLAT_PVR_DRM)
static void	local_cas_ir_Drv_UpdateDRMInfoDuringPlayback(Handle_t	hAction);
#endif


void	local_cas_ir_Drv_SendDMVSignal(Handle_t hIrSvc, HUINT8 state, HUINT32 ulDMVStatus)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo)
	{
		pstIrSvcInfo->pstPVRSvcInfo->stateCryptoKey |= state;
		if (pstIrSvcInfo->pstPVRSvcInfo->stateCryptoKey == STATE_CRYPTO_OK)
		{
			HxLOG_Info("CRYPTO OK --> Send DMV Signal\n");
			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, ulDMVStatus, (HINT32)NULL, (HINT32)NULL);
		}
	}
}
#endif

IR_PVRSERVICE_INFO	*CAS_IR_Drv_FindPVRSvcInfoBySCELL(HUINT16 usSCELLHandle)
{
	HINT32		i;
	IR_PVRSERVICE_INFO	*pstPVRSvcInfo;

	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrSvcInfo[i].pstPVRSvcInfo)
		{
			pstPVRSvcInfo = g_IrSvcInfo[i].pstPVRSvcInfo;
			if (pstPVRSvcInfo->ucIsSCELLReady && (pstPVRSvcInfo->usSCELL_SvcHandle == usSCELLHandle))
				return (IR_PVRSERVICE_INFO*)pstPVRSvcInfo;
		}
	}

	return NULL;
}

void	CAS_IR_Drv_GetPlaybackEncryptionMode(Handle_t hIrSvc, HUINT8 *pucEncryption)
{
	SvcMeta_Record_t		stRecordInfo;
	IR_SERVICE_INFO		*pstIrSvcInfo;

	*pucEncryption = 0;

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo)
	{
		//	find encryption info with ulContentsId...
		//	if contents id is not valid, we can set encryption flag to FALSE
		HxLOG_Print("Target ContentsID [%x]\n", pstIrSvcInfo->ulContentsId);

		if (SVC_META_GetRecordInfo(pstIrSvcInfo->ulContentsId, &stRecordInfo) == ERR_OK)
		{
			if (stRecordInfo.u1cEncryption)
				*pucEncryption = 1;
			else
				*pucEncryption = 0;

			HxLOG_Print("playback EncryptionMode (%x)\n", *pucEncryption);
			return;
		}
	}

	*pucEncryption = 0;
}

HERROR local_cas_ir_Drv_SetPlaybackEncryptKey(Handle_t hIrSvc, HUINT8 *encryptKey)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Info("Set Decryption Key hIrSvc(%x)\n", hIrSvc);
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Can't find irSvcInfo in IrSvc List!!\n");
		return HIR_NOT_OK;
	}

//	여기서 에러나는 상태.
//	Descryption키를 바꿀수 없음. 바꾸기 위해서는 Player Stop을 해야하는데...
//	Player는 한개!!
HxLOG_Info("Set Decryption -- \n");
{
	HINT32	i;

	for (i = 0; i < 16; i++)
		HxLOG_Info("%02X ", encryptKey[i]);
	HxLOG_Info("\n");
}

#if	defined(IRPLAT_PVR_DRM)
	if (pstIrSvcInfo->pstPVRSvcInfo->bIsSetCryptoKey)
	{
		HxLOG_Info("Crypto Key is already SET. Ignore Key.\n");
		return HIR_OK;
	}

	pstIrSvcInfo->pstPVRSvcInfo->bIsSetCryptoKey = TRUE;
#endif

HxLOG_Info("secure CSSK [%x]\n", (HINT32)_secured_cssk);

#if	defined(CONFIG_OP_DIGITURK)
#if defined(HMX_VD_STREAMCRYPTO_AES)
	if (CARM_PVR_PLAY_SetDecryption(0, PAL_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY, encryptKey, NULL, _secured_cssk) != ERR_OK)
#else
	if (CARM_PVR_PLAY_SetDecryption(0, PAL_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY, encryptKey, NULL, _secured_cssk) != ERR_OK)
#endif
#else
#if defined(HMX_VD_STREAMCRYPTO_AES)
	if (CARM_PVR_PLAY_SetDecryption(0, ePAL_PVR_ENCRYPTION_TYPE_AES, encryptKey, NULL) != ERR_OK)
#else
	if (CARM_PVR_PLAY_SetDecryption(0, ePAL_PVR_ENCRYPTION_TYPE_3DES, encryptKey, NULL) != ERR_OK)
#endif
#endif
	{
		HxLOG_Print("error : decryption fail. \n");
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

static HERROR local_cas_ir_Drv_SetRecordEncryptKey(Handle_t hIrSvc, HUINT8 *encryptKey)
{
	HINT32		nRecordId;
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Info("Set Encryption Key hIrSvc(%x)\n", hIrSvc);
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Can't find irSvcInfo in IrSvc List!!\n");
		return HIR_NOT_OK;
	}

//	여기서 에러나는 상태.
//	Encryption을 걸기 위해서는 record stop을 해야함.
//	아직 지원 안되어있음.

HxLOG_Info("Set Encryption -- \n");
{
	HINT32	i;

	for (i = 0; i < 16; i++)
		HxLOG_Info("%02X ", encryptKey[i]);
	HxLOG_Info("\n");
}
HxLOG_Info("secure CSSK [%x]\n", (HINT32)_secured_cssk);

#if	defined(IRPLAT_PVR_DRM)
	if (pstIrSvcInfo->pstPVRSvcInfo->bIsSetCryptoKey)
	{
		HxLOG_Info("Crypto Key is alreay SET. Ignore Key.\n");
		return HIR_OK;
	}
	pstIrSvcInfo->pstPVRSvcInfo->bIsSetCryptoKey = TRUE;
#endif

	//	Set key...
	if (svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(pstIrSvcInfo->hAction), eRxRSCTYPE_HDD_REC, &ulRecordId) != ERR_OK)
	{
		HxLOG_Print("error : invalid record Id \n");
		return HIR_NOT_OK;
	}

#if	defined(CONFIG_OP_DIGITURK)
#if	defined(HMX_VD_STREAMCRYPTO_AES)
	if (CARM_PVR_REC_SetEncryption((HUINT32)ulRecordId, PAL_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY, encryptKey, NULL, _secured_cssk) != ERR_OK)
#else
	if (CARM_PVR_REC_SetEncryption((HUINT32)ulRecordId, PAL_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY, encryptKey, NULL, _secured_cssk) != ERR_OK)
#endif
#else
#if	defined(HMX_VD_STREAMCRYPTO_AES)
	if (CARM_PVR_REC_SetEncryption((HUINT32)ulRecordId, ePAL_PVR_ENCRYPTION_TYPE_AES, encryptKey, NULL) != ERR_OK)
#else
	if (CARM_PVR_REC_SetEncryption((HUINT32)ulRecordId, ePAL_PVR_ENCRYPTION_TYPE_3DES, encryptKey, NULL) != ERR_OK)
#endif
#endif
	{
		HxLOG_Print("error : encryption fail. \n");
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HUINT8 tsrMainEncryptionFlag, tsrPipEncryptionFlag;

HUINT32 local_cas_ir_Drv_UpdateRecordEncryptionMode(Handle_t hIrSvc, HUINT8 recordMode)
{
	HERROR				err;
	SvcMeta_Record_t		stRecInfo;

	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Info("hIrSvc[%x] recordMode[%x]\n", hIrSvc, recordMode);
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Can't find IrSvcInfo with hIrSvc(%x)\n", hIrSvc);
		return HIR_NOT_OK;
	}
	if (pstIrSvcInfo->pstPVRSvcInfo == NULL && recordMode == IR_RECORD_TYPE_ENCRYPTION)
	{
		HxLOG_Print("Not PVR Service!!!\n");
		return HIR_NOT_OK;
	}

	if ((err = SVC_META_GetRecordInfo(pstIrSvcInfo->ulContentsId, &stRecInfo)) == ERR_OK)
	{
		HxLOG_Print("Record Encryption mode is changed : [%d --> ", stRecInfo.u1cEncryption);
		stRecInfo.u1cEncryption = ((recordMode == IR_RECORD_TYPE_ENCRYPTION) ? 1 : 0);
		HxLOG_Info("%d]\n", stRecInfo.u1cEncryption);

		err = SVC_META_SetRecordInfo(pstIrSvcInfo->ulContentsId, &stRecInfo);

		//	in Previous source code, add 'forbidden' and 'noMSK' attribute
	}

	return err;
}

void CAS_IR_DRV_Setup_Record(Handle_t hIrSvc, HUINT8 recordMode, HUINT16 errorIndex)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	CAS_CAI_IF_msgPVR* pMsg;

	HxLOG_Info("CAS_IR_DRV_Setup_Record : hIrSvc(0x%X)\n", hIrSvc);

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Can't find irSvcHandle(%x) in SvcList\n", hIrSvc);
		return;
	}

	pMsg = (CAS_CAI_IF_msgPVR *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPVR));
	if (pMsg == NULL)
	{
		HxLOG_Print("[CAS_IR_DRV_Setup_Record] memory allocation failed !!\n");
		return;
	}
//	pMsg->hAction = pstIrSvcInfo->hAction;
	pMsg->hIrSvc = hIrSvc;
	pMsg->recordMode = recordMode;
	pMsg->errorIndex = errorIndex;

	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_IR_SETUP_RECORD, (void *)pMsg);
}

void CAS_IR_DRV_Setup_Playback(Handle_t hIrSvc, HUINT8 recordMode, HUINT16 errorIndex, HUINT32 ulStatus)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	CAS_CAI_IF_msgPVR* pMsg;

	HxLOG_Info("CAS_IR_DRV_Setup_Playback : hIrSvc(0x%X)\n", hIrSvc);

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Can't find irSvcHandle(%x) in SvcList\n", hIrSvc);
		return;
	}

	pMsg = (CAS_CAI_IF_msgPVR *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPVR));
	if (pMsg == NULL)
	{
		HxLOG_Print("[CAS_IR_DRV_Setup_Playback] memory allocation failed !!\n");
		return;
	}

	pMsg->hIrSvc = pstIrSvcInfo->hIrSvc;
	pMsg->recordMode = recordMode;
	pMsg->errorIndex = errorIndex;
	pMsg->ulStatus   = ulStatus;

	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_IR_SETUP_PLAYBACK, (void *)pMsg);
}

HERROR CAS_IR_PVR_SetupRecord(CAS_CAI_IF_msgPVR *msgPVR)
{
	HMSG	msg;

	IR_SERVICE_INFO		*pstIrSvcInfo;
	//IR_PVRSERVICE_INFO	*pstIrPVRSvcInfo;

	HxLOG_Info("\n");

//	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByActionId(msgPVR->hAction);
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(msgPVR->hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Can't find IrSvcInfo by Action Handle (NULL)\n");
		return HIR_NOT_OK;
	}

#if	defined(IRPLAT_PVR_PHASE12)
	//In case DESCRAMBLE type, record descrambler pid should be set again.
	//when smartcard reinserted.
	if ((msgPVR->recordMode != IR_RECORD_TYPE_DESCRAMBLE) && (msgPVR->recordMode == pstIrSvcInfo->ucRecordMode))
		return HIR_OK;
#endif

	HxLOG_Print("[IR_DRV]Setup Record recordMode=0x%x\n", msgPVR->recordMode);

	//	DD 제약상 한번 Encryption하거나 Clear로 Recording Start시에 이를 변경하지 못한다. kimjh

	switch (msgPVR->recordMode)
	{
#if	defined(IRPLAT_PVR_DRM)
		case	IR_RECORD_TYPE_DRM:
			//	Start PVR DRM Module - IDX_NONE <-- descrambling
			if (msgPVR->errorIndex == AXI_STATUSCODE_IDX_None)
			{
				PrintMSKMode("PVR DRM Module");
				pstIrSvcInfo->ucRecordMode = msgPVR->recordMode;
				if (pstIrSvcInfo->pstPVRSvcInfo == NULL)
				{
					//	PVR is not registered.
					PrintMSKMode("Start PVR DRM Module\n");
					CAS_IR_DRV_Start_RecordService(pstIrSvcInfo->hIrSvc);
				}
			} else
			if ((msgPVR->errorIndex == AXI_STATUSCODE_IDX_D126_4) || (msgPVR->errorIndex == AXI_STATUSCODE_IDX_D100_27))
			{
				//	PVR DRM is OK.
				PrintMSKMode("PVR DRM is working");
//	Encryption 설정은 VD에서 하는게 맞음.
#if	defined(SET_CRYPTOKEY_IN_VD_CALL)
				local_cas_ir_Drv_SendDMVSignal(pstIrSvcInfo->hIrSvc, STATE_CRYPTO_PVR_OK, IRMV_STATUS_ENCRYPTION);
#else
				local_cas_ir_Drv_UpdateRecordEncryptionMode(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_ENCRYPTION);
				//	EVEN/ODD지원 못하자네.. 지원하면 변경을...
				local_cas_ir_Drv_SetRecordEncryptKey(pstIrSvcInfo->hIrSvc, pstIrSvcInfo->pstPVRSvcInfo->aucCryptoKey[0]);

				CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_ENCRYPTION, (HINT32)NULL, (HINT32)NULL);
#endif

				msg.usParam = msgPVR->errorIndex;
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_PVR;
				CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);
			} else
			{
				HxLOG_Info("ERROR : Check Message - errIndex[%d]\n", msgPVR->errorIndex);
			}
			break;

		case	IR_RECORD_TYPE_DRM_ERROR:
			//	PVR DRM is Error
			HxLOG_Info("*** PVR_DRM module is not working due to reason [%d]\n", msgPVR->errorIndex);

			msg.usParam = msgPVR->errorIndex;
			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_PVR;
			CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);


			//	Send DMV_SIGNALLED message for starting record
			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_NOMSK, (HINT32)NULL, (HINT32)NULL);
			break;
#endif
#if	defined(IRPLAT_PVR_PHASE12)
		case	IR_RECORD_TYPE_CLEAR:
			//	FTA방송일때 와야 하나 주는 곳이 없음.
			//	걍 무시임...
			PrintMSKMode("Record FTA");
			pstIrSvcInfo->ucRecordMode = msgPVR->recordMode;

			//	send DMV message
			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_FTA, (HINT32)NULL, (HINT32)NULL);
			break;

		case	IR_RECORD_TYPE_DESCRAMBLE:
			//	Phase 1,2 에서 MacroVision값을 보고 Encryption 할필요가 없는 Recording Service
			PrintMSKMode("Record DESCRAMBLE");
			pstIrSvcInfo->ucRecordMode = msgPVR->recordMode;
			local_cas_ir_Drv_UpdateRecordEncryptionMode(pstIrSvcInfo->hIrSvc, msgPVR->recordMode);

			//	send DMV_SIGNALLED message for starting record
			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_CLEAR, (HINT32)NULL, (HINT32)NULL);
			break;

		case	IR_RECORD_TYPE_CHECKMSK:
			//	Phase 1,2 에서 MacroVision값에 의해 Encryption필요 --> CheckMSK모드로 감.
			PrintMSKMode("Record CheckMSK");
			pstIrSvcInfo->ucRecordMode = msgPVR->recordMode;

			//	Check MSK --> PVR Service Start
			if (pstIrSvcInfo->pstPVRSvcInfo == NULL)
			{
				HxLOG_Print("Record PVR service is not defined --> Start Irdeto PVR service!!!\n");
				CAS_IR_DRV_Start_RecordService(pstIrSvcInfo->hIrSvc);
			} else
			{
				PrintMSKMode("MSK already checked\n");
			}
			break;

		case	IR_RECORD_TYPE_ENCRYPTION:
			//	Phase 1,2에서 Encryption해야 하는넘으로 판단. 저장된 Encryption키로 Encryption Start
			//	단 이미 Recording증일경우 무시함. <-- HW제약.
			PrintMSKMode("Record Encryption");
			if (pstIrSvcInfo->ucRecordMode == IR_RECORD_TYPE_CHECKMSK)
			{
				local_cas_ir_Drv_UpdateRecordEncryptionMode(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_ENCRYPTION);
				local_cas_ir_Drv_SetRecordEncryptKey(pstIrSvcInfo->hIrSvc, pstIrSvcInfo->pstPVRSvcInfo->aucCryptoKey[0]);
			}
			pstIrSvcInfo->ucRecordMode = msgPVR->recordMode;

			msg.usParam = msgPVR->errorIndex;
			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_PVR;
			CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

			//	Send DMV_SIGNALLED message for starting record.
			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_ENCRYPTION, (HINT32)NULL, (HINT32)NULL);
			break;

		case	IR_RECORD_TYPE_NOMSK:
			//	Phase 1,2에서 MSK가 없을 경우 Recording을 Pause시킴
			//	단 이미 녹화중이면 무시함 <-- HW제약.
			if (pstIrSvcInfo->ucRecordMode == IR_RECORD_TYPE_CHECKMSK)
			{
				PrintMSKMode("Record NO_MSK");
				if (pstIrSvcInfo->ucRecordMode == IR_RECORD_TYPE_ENCRYPTION)
					break;
			}
			pstIrSvcInfo->ucRecordMode = IR_RECORD_TYPE_NOMSK;

			msg.usParam = msgPVR->errorIndex;
			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_PVR;
			CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

			//	Send DMV_SIGNALLED message for starting record
			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_NOMSK, (HINT32)NULL, (HINT32)NULL);
			break;

		case	IR_RECORD_TYPE_FORBIDDEN:
			PrintMSKMode("Record FORBIDDEN");
			pstIrSvcInfo->ucRecordMode = msgPVR->recordMode;

#if	0		//	Application controlled message (ap_record_irdeto)
			msg.usParam = IR_EVT_Forbidden_Rec;
			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_PVR;
			CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);
#endif

			//	Send DMV_SIGNALLED message for NOT ALLOWED RECORDING
			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_FORBIDDEN, (HINT32)NULL, (HINT32)NULL);
			break;
#endif
	}

	return HIR_OK;
}

HERROR CAS_IR_PVR_SetupPlayback(CAS_CAI_IF_msgPVR *msgPVR)
{
	HMSG			msg;
	HUINT8			ucRecordMode;
	IR_SERVICE_INFO	*pstIrSvcInfo;

	HxLOG_Print("recordMode (%x)\n", msgPVR->recordMode);
	ucRecordMode = msgPVR->recordMode;

//	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByActionId(msgPVR->hAction);
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(msgPVR->hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("Can't find IrSvcInfo by Action Handle (NULL)\n");
		return HIR_NOT_OK;
	}

#if	defined(IRPLAT_PVR_PHASE12)
	if (ucRecordMode == pstIrSvcInfo->ucRecordMode)
		return HIR_OK;
#endif

	HxLOG_Print("Setup Playback with hIrSvc(%x) recordMode (%x)\n", msgPVR->hIrSvc, ucRecordMode);
	switch (ucRecordMode)
	{
#if	defined(IRPLAT_PVR_DRM)
		case	IR_RECORD_TYPE_DRM:
			//	Start PVR DRM Module - IDX_NONE <-- descrambling
			if (msgPVR->errorIndex == AXI_STATUSCODE_IDX_None)
			{
				PrintMSKMode("Start PVRDRM PB Module");
				pstIrSvcInfo->ucRecordMode = msgPVR->recordMode;
				CAS_IR_DRV_Start_PlaybackService(pstIrSvcInfo->hIrSvc);
			} else
			if ((msgPVR->errorIndex == AXI_STATUSCODE_IDX_D126_4) || (msgPVR->errorIndex == AXI_STATUSCODE_IDX_D100_28))
			{
				PrintMSKMode("PVR DRM PB Working");
#if	defined(SET_CRYPTOKEY_IN_VD_CALL)
				local_cas_ir_Drv_SendDMVSignal(pstIrSvcInfo->hIrSvc, STATE_CRYPTO_PVR_OK, IRMV_STATUS_ENCRYPTION);
#else
				//	EVEN/ODD지원 못하자네.. 지원하면 변경을...
				local_cas_ir_Drv_SetPlaybackEncryptKey(pstIrSvcInfo->hIrSvc, pstIrSvcInfo->pstPVRSvcInfo->aucCryptoKey[0]);

				CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_ENCRYPTION, (HINT32)NULL, (HINT32)NULL);
#endif
				//	Send Event for Decrypt well
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_PVR;//IREVT_CLASS_CAS_DESCRAMBLED_WELL;
				msg.usParam = msgPVR->errorIndex;
				msg.ulParam = msgPVR->ulStatus; 		//	PVR Playback OK (D126-04)
				CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

				local_cas_ir_Drv_UpdateDRMInfoDuringPlayback(pstIrSvcInfo->hAction);

			} else
			{
				HxLOG_Info("PVR_DRM : Check error code\n");
			}
			break;

		case	IR_RECORD_TYPE_DRM_ERROR:
			HxLOG_Info("PVR_DRM module found error...\n");

			msg.usParam = msgPVR->errorIndex;
			msg.ulParam = msgPVR->ulStatus;
			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_PVR;
			CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_NOMSK, (HINT32)NULL, (HINT32)NULL);
			break;
#endif

#if	defined(IRPLAT_PVR_PHASE12)
		case	IR_RECORD_TYPE_CHECKMSK:
			PrintMSKMode("PLAYBACK CHECKMSK");
			pstIrSvcInfo->ucRecordMode = ucRecordMode;
			CAS_IR_DRV_Start_PlaybackService(pstIrSvcInfo->hIrSvc);
			break;

		case	IR_RECORD_TYPE_ENCRYPTION:
			PrintMSKMode("PLAYBACK ENCRYPTION");
			if (pstIrSvcInfo->pstPVRSvcInfo)
			{
				if (pstIrSvcInfo->ucRecordMode == IR_RECORD_TYPE_CHECKMSK)
				{
					HxLOG_Print("Set Playback Encryption KEY!!\n");
					local_cas_ir_Drv_SetPlaybackEncryptKey(pstIrSvcInfo->hIrSvc, pstIrSvcInfo->pstPVRSvcInfo->aucCryptoKey[0]);
				} else
				{
					HxLOG_Print("Find encryption key! but, we ignore current key due to h/w restriction\n");
				}

				//	Send Event for Decrypt well
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_PVR;//IREVT_CLASS_CAS_DESCRAMBLED_WELL;
				msg.usParam = msgPVR->errorIndex;
				msg.ulParam = msgPVR->ulStatus;			//	PVR Playback OK (D100-28)
				CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);


				pstIrSvcInfo->ucRecordMode = ucRecordMode;
				//	Send DMV_SIGNALLED message for starting playback.
				CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_ENCRYPTION, (HINT32)NULL, (HINT32)NULL);
			} else
			{
				HxLOG_Print("Can't find Playback Handle!!\n");
			}
			break;

		case	IR_RECORD_TYPE_NOMSK:
			PrintMSKMode("PLAYBACK NOMSK");

			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_PVR;//IREVT_CLASS_CAS_ERROR_FOR_GLOBAL;
			msg.usParam = msgPVR->errorIndex;
			msg.ulParam = msgPVR->ulStatus;
			CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

			pstIrSvcInfo->ucRecordMode = ucRecordMode;
			if (pstIrSvcInfo->ucRecordMode == IR_RECORD_TYPE_ENCRYPTION)
				break;

			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_NOMSK, (HINT32)NULL, (HINT32)NULL);
			break;

		case	IR_RECORD_TYPE_FORBIDDEN:
			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_FORBIDDEN, (HINT32)NULL, (HINT32)NULL);
			break;

		case	IR_RECORD_TYPE_CLEAR:
		case	IR_RECORD_TYPE_DESCRAMBLE:
		default:
			PrintMSKMode("PLAYBACK CLEAR/DESC");
			pstIrSvcInfo->ucRecordMode = ucRecordMode;

			//	send DMV_SIGNALLED message for starting playback
			CASMGR_UpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DMV_SIGNALLED, IRMV_STATUS_CLEAR, (HINT32)NULL, (HINT32)NULL);
			break;
#endif
	}

	return HIR_OK;
}


HERROR CAS_IR_Drv_CloseRecord(Handle_t hIrSvc)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Print("hIrSvc(%x)\n", hIrSvc);

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("ERROR - invalid Handle (%x)\n", hIrSvc);
		return 1;
	}

	CAS_IR_DRV_Stop_RecordService(hIrSvc);

	return HIR_OK;
}

HERROR CAS_IR_Drv_ClosePlayback(Handle_t hIrSvc)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Print("hIrSvc(%x)\n", hIrSvc);
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("ERROR - invalid Handle (%x)\n", hIrSvc);
		return 1;
	}

	CAS_IR_DRV_Stop_PlaybackService(hIrSvc);

	return HIR_OK;
}

//	Make PVR index for SoftCELL
/**********************************************************************************
*         Not used Function for 2010.05.07
*		CAS_IR_DRV_MakePVRSvcIndex, CAS_IR_DRV_GetBaseIrSvcIndexFromPVRIndex
***********************************************************************************/
HUINT16		CAS_IR_DRV_MakePVRSvcIndex(HUINT16 usSvcIndex)
{
	//	max svcIndex == IR_HUMAX_MAX_SERVICE(under 255)
	return ((usSvcIndex + 1) << 8);
}

HUINT16		CAS_IR_DRV_GetBaseIrSvcIndexFromPVRIndex(HUINT16 usSvcIndex)
{
	return ((usSvcIndex - 1) >> 8);
}

HUINT32 CAS_IR_DRV_SaveCryptoKey(HUINT16 usIrServiceHandle, HUINT8 type, HUINT8* CryptoKey, HUINT32 len)
{
	HINT32		i, numFound = 0;

	HxLOG_Info("SCELL_Handle[%x]\n", usIrServiceHandle);
	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if ((g_IrSvcInfo[i].usSCELL_SvcHandle == usIrServiceHandle) && (g_IrSvcInfo[i].pstPVRSvcInfo))
		{
			if (type == STREAM_CRYPTO_TYPE_EVEN)
			{
				if (CryptoKey != NULL)
				{
					HxSTD_memcpy(g_IrSvcInfo[i].pstPVRSvcInfo->aucCryptoKey[1], CryptoKey, len);
				}
			}
			else
			{
				if (CryptoKey != NULL)
				{
					HxSTD_memcpy(g_IrSvcInfo[i].pstPVRSvcInfo->aucCryptoKey[0], CryptoKey, len);
				}
			}

#if	defined(SET_CRYPTOKEY_IN_VD_CALL)
	#if	defined(IRPLAT_PVR_DRM)
			if (type == STREAM_CRYPTO_TYPE_ODD)
	#endif
	#if	defined(IRPLAT_PVR_PHASE12)
			if (type == STREAM_CRYPTO_KEY)
	#endif
			{
				if (g_IrSvcInfo[i].eIrSvcActionType == IR_SVC_PLAYBACK)
				{
					HxLOG_Info("Save Crypto Key in Playback\n");
					local_cas_ir_Drv_SetPlaybackEncryptKey(g_IrSvcInfo[i].hIrSvc, g_IrSvcInfo[i].pstPVRSvcInfo->aucCryptoKey[0]);
					local_cas_ir_Drv_SendDMVSignal(g_IrSvcInfo[i].hIrSvc, STATE_CRYPTO_VD_OK, IRMV_STATUS_ENCRYPTION);
				} else
				{
					HxLOG_Info("Save Crypto Key in Record\n");
					local_cas_ir_Drv_UpdateRecordEncryptionMode(g_IrSvcInfo[i].hIrSvc, IR_RECORD_TYPE_ENCRYPTION);
					local_cas_ir_Drv_SetRecordEncryptKey(g_IrSvcInfo[i].hIrSvc, g_IrSvcInfo[i].pstPVRSvcInfo->aucCryptoKey[0]);
					local_cas_ir_Drv_SendDMVSignal(g_IrSvcInfo[i].hIrSvc, STATE_CRYPTO_VD_OK, IRMV_STATUS_ENCRYPTION);
				}
			}
#endif

			numFound++;
		}
	}

	if (numFound == 0)
	{
		HxLOG_Print("Can't find matched IrSvcInfo with SCELL_Handle[%x]\n", usIrServiceHandle);
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HUINT32	CAS_IR_DRV_ClearSecuredKey(void)
{
	_secured_cssk = NULL;

	return 0;
}

HUINT32	CAS_IR_DRV_SaveSecureKey(HUINT8 *pucCSSK)
{
	HxSTD_memcpy(_secured_cssk_data, pucCSSK, 16);
	_secured_cssk = _secured_cssk_data;

	return 0;
}

void CAS_IR_DRV_Start_RecordService(Handle_t hIrSvc)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;
	IR_PVRSERVICE_INFO	*pstIrPVRSvcInfo;

HxLOG_Info("CALL CAS_IR_DRV_StartRecordService\n");
	HxLOG_Print("[IR_DRV] Start_RecordService\n");
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	// prevent fix
	if (pstIrSvcInfo == NULL)
		return;
	// the end of prevent fix

	if (pstIrSvcInfo->pstPVRSvcInfo == NULL)
	{
HxLOG_Info(" --> contents Id [%x]\n", pstIrSvcInfo->ulContentsId);
		if (pstIrSvcInfo->ulContentsId == IR_RECORD_ID_INVALID)
		{
			HxLOG_Info("\n\n--> Start Record --> Contents Id is not valid yet!!!\n\n");
			if (SVC_META_GetContentIdByhAction(pstIrSvcInfo->hAction, &pstIrSvcInfo->ulContentsId) != ERR_OK)
			{
				pstIrSvcInfo->ulContentsId = IR_RECORD_ID_INVALID;
				HxLOG_Info("\t --> Recording Fail --> PVR_REC_Base is not working yet!!\n");
			} else
			{
				HxLOG_Info("\t --> Contents ID is OK --> PVR_REC_Base is working...\n");
			}
		}

		pstIrPVRSvcInfo = (IR_PVRSERVICE_INFO*)OxCAS_Malloc(sizeof(IR_PVRSERVICE_INFO));
		HxSTD_memset(pstIrPVRSvcInfo, 0, sizeof(IR_PVRSERVICE_INFO));
		pstIrPVRSvcInfo->eIrSvcActionType = IR_SVC_RECORD;
		pstIrPVRSvcInfo->ulContentId = pstIrSvcInfo->ulContentsId;
		pstIrSvcInfo->pstPVRSvcInfo = pstIrPVRSvcInfo;
	}

#if	defined(IRPLAT_PVR_PHASE12)
	CAS_IR_SVC_OPEN_REQUEST_PVR(hIrSvc, SERVICE_PVR_RECORD);
#endif

#if	defined(IRPLAT_PVR_DRM)
	CAS_IR_SVC_OPEN_REQUEST_PVR(hIrSvc, SERVICE_PVR_DRM);
#endif
}

void CAS_IR_DRV_Stop_RecordService(Handle_t hIrSvc)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Print("hIrSvc(%x)\n", hIrSvc);
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	// prevent fix
	if (pstIrSvcInfo == NULL)
		return;
	// the end of prevent fix

	if (pstIrSvcInfo->pstPVRSvcInfo == NULL)
	{
		HxLOG_Print("Record service is not started!!!\n");
		return;
	}

	CAS_IR_SVC_CLOSE_PVR(hIrSvc, SERVICE_PVR_RECORD);

	//	Remove PVRSVCInfo
	if (pstIrSvcInfo->pstPVRSvcInfo)
	{
		OxCAS_Free(pstIrSvcInfo->pstPVRSvcInfo);
		pstIrSvcInfo->pstPVRSvcInfo = NULL;
	}
}

void CAS_IR_DRV_Start_PlaybackService(Handle_t hIrSvc)
{
	HMSG				msg;
	IR_SERVICE_INFO		*pstIrSvcInfo;
	IR_PVRSERVICE_INFO	*pstIrPVRSvcInfo;

	HxLOG_Print("[IR_DRV] Start_Playback\n");
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	// prevent fix
	if (pstIrSvcInfo == NULL)
		return;
	// the end of prevent fix

	if (pstIrSvcInfo->pstPVRSvcInfo == NULL)
	{
		pstIrPVRSvcInfo = (IR_PVRSERVICE_INFO*)OxCAS_Malloc(sizeof(IR_PVRSERVICE_INFO));
		if (pstIrPVRSvcInfo == NULL)
		{
			return;
		}

		HxSTD_memset(pstIrPVRSvcInfo, 0, sizeof(IR_PVRSERVICE_INFO));
		pstIrPVRSvcInfo->eIrSvcActionType = IR_SVC_PLAYBACK;
		pstIrPVRSvcInfo->ulContentId = pstIrSvcInfo->ulContentsId;
		pstIrSvcInfo->pstPVRSvcInfo = pstIrPVRSvcInfo;

		//	Need to get CRID, RightRecId, SeqRecId from Metafile.
#if	defined(IRPLAT_PVR_DRM)
		{
			SvcMeta_Record_t		stRecInfo;

			SVC_META_GetRecordInfo(pstIrSvcInfo->ulContentsId, &stRecInfo);
			pstIrPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID		= stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.ulCRID;
			pstIrPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulRightRecId	= stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.ulRightsRecordId;
			pstIrPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulSeqRecId		= stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.ulSequenceRecordId;
		}
#endif
	}
	msg.usMsgClass = IREVT_CLASS_SVC_DESCRAMBLE_OPEN;
	CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

#if	defined(IRPLAT_PVR_PHASE12)
	CAS_IR_SVC_OPEN_REQUEST_PVR(hIrSvc, SERVICE_PVR_PLAYBACK);
#endif
#if	defined(IRPLAT_PVR_DRM)
	CAS_IR_SVC_OPEN_REQUEST_PVR(hIrSvc, SERVICE_PVR_DRM);
#endif
}

void CAS_IR_DRV_Stop_PlaybackService(Handle_t hIrSvc)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Print("[IR_DRV] Stop_PlaybackService\n");
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(hIrSvc);
	//prevent fix
	if (pstIrSvcInfo == NULL)
		return;
	//the end of prevent fix

	if (pstIrSvcInfo->pstPVRSvcInfo == NULL)
	{
		HxLOG_Print("Playback Service is not started!!!\n");
		return;
	}

#if	defined(IRPLAT_PVR_PHASE12)
	CAS_IR_SVC_CLOSE_PVR(hIrSvc, SERVICE_PVR_PLAYBACK);
#endif
#if	defined(IRPLAT_PVR_DRM)
	CAS_IR_SVC_CLOSE_PVR(hIrSvc, SERVICE_PVR_DRM);
#endif
	//	Remove PVRSVCInfo
	if (pstIrSvcInfo->pstPVRSvcInfo)
	{
		OxCAS_Free(pstIrSvcInfo->pstPVRSvcInfo);
		pstIrSvcInfo->pstPVRSvcInfo = NULL;
	}
}


#if	defined(IRPLAT_PVR_DRM)
void	CAS_IR_DRV_UpdateDRMInfo(Handle_t	hAction)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;
	SvcMeta_Record_t		stRecInfo;
	HxDATETIME_t 		stDateTime;
	UNIXTIME			ulUnixTime = 0;

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByActionId(hAction);
	if (SVC_META_GetRecordInfo(pstIrSvcInfo->ulContentsId, &stRecInfo) == ERR_OK)
	{
		stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.ulCRID = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID;
		stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.ulRightsRecordId = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulRightRecId;
		stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.ulSequenceRecordId = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulSeqRecId;

		stRecInfo.stDrmInfo.eDrmType = eDxCopyrightControl_CasCcType_Irdeto;
		stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulCRID = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID;
		stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.eCopyControl = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.eCopyControl;
		stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulPlaybackStartTime;
		stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackDuration = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulPlaybackDuration;
		stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ucNrOfPlayback = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.NrOfPlayback;

		/* start time 이 0이면 무제한, 0xFFFFFFFF이면 play시 결정되므로 play시 update필요. */
		if((stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime != 0)
			&& (stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime != 0xFFFFFFFF))
		{
			/*in seconds since 1, Jan, 2000, 00:00 UTC */
			stDateTime.stDate.usYear = 2000;
			stDateTime.stDate.ucMonth = 1;
			stDateTime.stDate.ucDay = 1;

			stDateTime.stTime.ucHour = 0;
			stDateTime.stTime.ucMinute = 0;
			stDateTime.stTime.ucSecond = 0;
			stDateTime.stTime.usMillisecond = 0;

			if(HLIB_DATETIME_ConvertDateTimeToUnixTime (&stDateTime, &ulUnixTime) != ERR_OK)
			{
				HxLOG_Critical("\n\n");
			}
			/* Changnig to Unixtime based */
			stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime += ulUnixTime;
		}

#if defined(CONFIG_DEBUG)
		HxLOG_Print("\n\n\n====>PVR DRM Info CRID[0x%x], PbCC[%d], PbStarTime[%u], PbDuration[%u], PbNuber[%d]\n\n\n",
					stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulCRID,
					stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.eCopyControl,
					stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime,
					stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackDuration,
					stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ucNrOfPlayback));

		if(stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime)
		{
			HLIB_DATETIME_ConvertUnixTimeToDateTime(stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime + stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackDuration, &stDateTime);
			HxLOG_Error("====Expiration Info[%d, %d, %d][%d:%d:%d] Nr[%d]====\n", stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay,
														stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond,
														stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ucNrOfPlayback));
		}
#endif

		SVC_META_SetRecordInfo(pstIrSvcInfo->ulContentsId, &stRecInfo);
	}
}


void	local_cas_ir_Drv_UpdateDRMInfoDuringPlayback(Handle_t	hAction)
{
	HBOOL				bUpdated = FALSE;
	IR_SERVICE_INFO		*pstIrSvcInfo;
	SvcMeta_Record_t		stRecInfo;
	HxDATETIME_t 		stDateTime;
	UNIXTIME			ulUnixTime = 0;

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByActionId(hAction);

	if (SVC_META_GetRecordInfo(pstIrSvcInfo->ulContentsId, &stRecInfo) == ERR_OK)
	{
		if(stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime == 0xFFFFFFFF)
		{
			stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulPlaybackStartTime;
			stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackDuration = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulPlaybackDuration;
			/* start time 이 0이면 무제한 */
			if((stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime != 0)
				&& (stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime != 0xFFFFFFFF))
			{
				/*in seconds since 1, Jan, 2000, 00:00 UTC */
				stDateTime.stDate.usYear = 2000;
				stDateTime.stDate.ucMonth = 1;
				stDateTime.stDate.ucDay = 1;

				stDateTime.stTime.ucHour = 0;
				stDateTime.stTime.ucMinute = 0;
				stDateTime.stTime.ucSecond = 0;
				stDateTime.stTime.usMillisecond = 0;

				if(HLIB_DATETIME_ConvertDateTimeToUnixTime (&stDateTime, &ulUnixTime) != ERR_OK)
				{
					HxLOG_Critical("\n\n");
				}
				/* Changnig to Unixtime based */
				stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime += ulUnixTime;
			}

			if(stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime != 0xFFFFFFFF)
			{
				bUpdated = TRUE;
			}
		}

		if(stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ucNrOfPlayback != pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.NrOfPlayback)
		{
			stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ucNrOfPlayback = pstIrSvcInfo->pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.NrOfPlayback;
			bUpdated = TRUE;
		}

		if(bUpdated)
		{
#if defined(CONFIG_DEBUG)
			HxLOG_Print("\n\n\n====>PVR DRM Info CRID[0x%x], PbCC[%d], PbStarTime[%u], PbDuration[%u], PbNuber[%d]\n\n\n",
						stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulCRID,
						stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.eCopyControl,
						stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime,
						stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackDuration,
						stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ucNrOfPlayback));

			if((stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime != 0)
				&& (stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime != 0xFFFFFFFF))
			{
				HLIB_DATETIME_ConvertUnixTimeToDateTime(stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackStartTime + stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ulPlaybackDuration, &stDateTime);
				HxLOG_Error("====Expiration Info[%d, %d, %d][%d:%d:%d] Nr[%d]====\n", stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay,
															stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond,
															stRecInfo.stDrmInfo.info.stIrdetoDrmInfo.stIrPvrDrmInfo.ucNrOfPlayback));
			}
#endif
			SVC_META_SetRecordInfo(pstIrSvcInfo->ulContentsId, &stRecInfo);
		}
	}
	else
	{
		HxLOG_Critical("\n\n");
	}
}
#endif

#endif

