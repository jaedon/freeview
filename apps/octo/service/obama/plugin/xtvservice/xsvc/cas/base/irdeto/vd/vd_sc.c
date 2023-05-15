#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "ir_svc.h"
#include "ir_main.h"

#include "di_smartcard.h"
//#include "pal_sc.h"

#include "s3_osdrv.h"
#include "s3_vd.h"
#include "s3_vdsc.h"



#include <_svc_cas_sub_dev_api.h>



#define	IRDETO_CARD_HISTORY_CHARS  	"IRDETO ACS" //but it shall be not adapted with ice-2 card

// 일반적으로 2번해서 안되면 안되는 것임. 3으로 괜히 늘려놓지 말것.
#define MAX_NUM_RETRY		2
#define T0_RETRY_COUNT		2

/* 생산용 SST TEST를 위해서 필요한 config */
#if defined (CONFIG_MW_CAS_IRDETO_SECURE)
#define T14_RETRY_COUNT	3
#else
#define T14_RETRY_COUNT	2
#endif

#define	CA_MAX_LENGTH_VDSC_SERIAL_NUMBER				11


typedef struct
{
	vd_rsc_id awResources[HUMAX_VD_SC_SLOT_COUNT];
} vd_sc_resource;


//extern HUINT8 g_ucScHardreset[HUMAX_VD_SC_SLOT_COUNT];

IR_DRV_SC_INFO		g_IrDrvScInfo[HUMAX_VD_SC_SLOT_COUNT];

typedef struct tagIrScAtrInfo
{
	HUINT32 	ulMinClock;
	HUINT32	ulBaudrate;

	HUINT8	ucK;
	HUINT8	ucFI;
	HUINT8	ucDI;
	HUINT8	ucN;
	HUINT8	ucWI;
	HUINT8	ucTFirst;
	HUINT8	ucT;
	HUINT8	ucIFSC;
	HUINT8	ucCWI;
	HUINT8	ucBWI;
	HUINT8	ucII;
	HUINT8	ucPI1;
	HUINT8	ucPI2;
	HUINT8	ucEDC;	//error detection code TC3

	HBOOL		bSpecificMode;
	HBOOL		bImplicitlyDefined;
	HBOOL		bPpsUnable;
	HBOOL		bExistTA1;
} IrScAtrInfo;


static SvcCas_DevSciScParam_t s_stIrScInParamT0 =
{
	eCAS_SCI_T_0_IR,		// protocol
	0,		// Min Clock
	0,		// baudrate
	1,		// FI
	1,		// DI
	0,		// N
	13,		// CWI		13
	4,		// BWI		4
};

static SvcCas_DevSciScParam_t s_stIrScInParamT14 =
{
	eCAS_SCI_T14,		// protocol
	3000 , //6000,	// Min Clock
	0,		// baudrate
	2,		// FI
	1,		// DI
	0,		// N
	13,		// CWI		13
	5,		// BWI		4
};
static IrScAtrInfo s_stIrAtrInfo;

static HUINT8 s_aucIrFiOrder[16] =
{ 0xFF, 1, 3, 4, 7, 8, 10, 0xFF, 0xFF, 2, 5, 6, 9, 11, 0xFF, 0xFF };

static HUINT8 s_aucIrDiOrder[16] =
{ 0xFF, 1, 2, 3, 4, 6, 8, 0xFF, 5, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static HUINT8 s_aucIrFiValue[16] =
{ 0xFF, 1, 9, 2, 3, 10, 11, 4, 5, 12, 6, 13, 0xFF, 0xFF, 0xFF, 0xFF };

static HUINT8 s_aucIrDiValue[16] =
{ 0xFF, 1, 2, 3, 4, 8, 5, 9, 6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


ia_result HUMAX_ResetCard( vd_ref_id wRefId );
static void local_VdSc_pfnCbScStatus0(SciHandle_t hSci, HUINT8 ucId, SvcCas_DevSciEvent_e eStatus);
static void local_VdSc_pfnCbScStatus1(SciHandle_t hSci, HUINT8 ucId, SvcCas_DevSciEvent_e eStatus);
ia_result local_VdSc_CommunicateCard( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
static HUINT32 local_VdSc_GetCardForm( vd_ref_id wRefId, ia_word32 *pwDataReadSize, void *pvDataRead );

#if defined(CAS_IR_ICE2_PROJECT)
static HUINT32 local_VdSc_CheckATR_ICE2( HUINT32 ulSlotId, HUINT8 *buf, HUINT32 nData, HUINT32 *pnProtocol );
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SMC_NEGOTIATE_MODE)
static HERROR local_VdSc_NegotiateByValueOrder(SciHandle_t hSci, IrScAtrInfo *pstAtrInfo, HUINT8 ucWI, HUINT8 ucBWI, SvcCas_DevSciScParam_t *pstDefaultParam);
static HERROR local_VdSc_RequestPps(SciHandle_t hSci, HUINT8 ucT, HUINT8 ucFi, HUINT8 ucDi, SvcCas_DevSciScParam_t *pstDefaultParam);
#else
static HUINT32 local_VdSc_CheckProtocol( HUINT8 *pbData, HUINT8 bLength );
static HUINT32 local_VdSc_CheckATR( HUINT32 ulSlotId, HUINT8 *buf, HUINT32 nData, HUINT32 *pnProtocol );
#endif

#if defined(CONFIG_DEBUG)
static void local_VdSc_DebugParsedAtr(IrScAtrInfo *pstAtrInfo);
#endif

#if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
extern HUINT32 CAS_IR_MGR_Notify_InstanceChanged(HUINT16 usSlotId, HUINT16 *pusCaSysId, HUINT8 ucNumCaSysId, HBOOL bAvailable);
#endif

#define ____________USED_BY_CA_TASK________________

HVD_Result  HUMAX_VD_SC_GetCardPhysicalStatus(HUINT8 ucSlotID, HBOOL *pSCInserted)
{
	HERROR	hErr;
	HxLOG_Info("slot id=%d\n", ucSlotID);

// Multi CAS 에서는 직접 읽지 않고 SC MGR 에 Notify 한 상태로 본다
#if (CONFIG_MW_CAS_NUM_EMBEDDED == 1)
	hErr =svc_cas_DevScGetCardStatus(g_IrDrvScInfo[ucSlotID].hIRSci, &(g_IrDrvScInfo[ucSlotID].CardPhysicalStatus) ) ;

	if(ERR_OK != hErr )
	{
		HxLOG_Print("svc_cas_DevScGetCardStatus, hErr=%d\n", hErr);
		return eHVD_ERR;
	}
#endif

	HxLOG_Print("g_IrDrvScInfo[ucSlotID].CardPhysicalStatus =%d\n", g_IrDrvScInfo[ucSlotID].CardPhysicalStatus);

	switch(g_IrDrvScInfo[ucSlotID].CardPhysicalStatus)
	{
		case eCAS_SCI_SC_NOT_PRESENT:
			*pSCInserted = FALSE;
			break;
		case eCAS_SCI_SC_PRESENT:
			*pSCInserted =TRUE;
			break;
		default:
			HxLOG_Print("CardPhysicalStatus=%d\n", g_IrDrvScInfo[ucSlotID].CardPhysicalStatus);
			return eHVD_ERR;
	}
	return eHVD_OK;
}

#define ____________USED_BY_SOFTCELL________________

ia_result HUMAX_VD_SC_Initialise(void)
{
	HUINT32		i;
	HERROR		hErr;


	HxLOG_Info("\n");

	HxSTD_memset(g_IrDrvScInfo, 0, sizeof(IR_DRV_SC_INFO) * HUMAX_VD_SC_SLOT_COUNT);
	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		g_IrDrvScInfo[i].ResourceID = g_IrDrvScInfo[i].ReferenceID = VD_REF_ID_ERROR;

		// TODO: jichoi. if other type used, adjust following line.
		g_IrDrvScInfo[i].eCardForm = VD_SC_CARD_FORM_NORMAL;
		g_IrDrvScInfo[i].CardStatus = 0xFFFFFFFF;

		if( VK_OK != VK_SEM_Create(&(g_IrDrvScInfo[i].SemaphoreID), "g_IrDrvScInfo", VK_SUSPEND_FIFO)   )
		{
			HxLOG_Print("semaphore create fail on %d slot\n", i);
			return IA_FAIL;
		}
	}

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if(i == 0)
			hErr = svc_cas_DevScRegisterCbFunc(0 ,eDxCAS_GROUPID_IR, local_VdSc_pfnCbScStatus0, &(g_IrDrvScInfo[0].hIRSci));
		else if(i == 1)
			hErr = svc_cas_DevScRegisterCbFunc(1, eDxCAS_GROUPID_IR, local_VdSc_pfnCbScStatus1, &(g_IrDrvScInfo[1].hIRSci));
		else
			hErr = ERR_FAIL;

		if( hErr != ERR_OK )
		{
			HxLOG_Print("SCI_RegisterCbFunc slot #%d\n", i);
			return IA_FAIL;
		}
	}

	return IA_SUCCESS;

}


/********************************************************************
*	function name :	 HUMAX_VD_SC_GetResources
*	input : ..
*	output : Available resource count/list & result.
*	description : 사용가능한 smartcard resource들을 전달.
*			   External Slot = 0, Internal Slot = 1;
********************************************************************/
HUINT32 HUMAX_VD_SC_GetResources(void **ppvList, ia_word16 *pwCount )
{
	HUINT8			i;
	vd_sc_resource	*List;

	HxLOG_Info("\n");

	*pwCount = HUMAX_VD_SC_SLOT_COUNT;

#if 1	// prevent fix
	List = OxCAS_Malloc(sizeof(vd_sc_resource));
#else
	List = (vd_sc_resource *)OxCAS_Malloc( *pwCount * sizeof(vd_rsc_id));
#endif
	for( i = 0; i < *pwCount ; i++ )
	{
		List->awResources[i] = i;
		g_IrDrvScInfo[i].ResourceID = i;
	}

	HxLOG_Info("Resource count = %d\n", *pwCount );

	*ppvList = (void*)List;
	return eHVD_OK;

}


vd_ref_id HUMAX_VD_SC_Open( vd_rsc_id wResourceID, void *pvData )
{
	HUINT32			i;
	vd_sc_open_st 	*pstScOpen;

	HxLOG_Info("wResourceID(0x%X)\n", (HUINT32)wResourceID);

	pstScOpen = (vd_sc_open_st *)pvData;

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if( g_IrDrvScInfo[i].ResourceID == wResourceID )
		{
			g_IrDrvScInfo[i].ReferenceID = wResourceID;	/*define reference id as resource id*/
			g_IrDrvScInfo[i].ScOpen.pfnStatusNotify = pstScOpen->pfnStatusNotify;
			g_IrDrvScInfo[i].ScOpen.wSoftCellVal = pstScOpen->wSoftCellVal;
			break;
		}
	}

	if(HUMAX_VD_SC_SLOT_COUNT == i)
	{
		HxLOG_Info("can not find resource id(0x%X) \n", (HUINT32)wResourceID );
		return VD_REF_ID_ERROR;
	}

	return g_IrDrvScInfo[i].ReferenceID;

}


ia_result HUMAX_VD_SC_Close( vd_ref_id wRefId )
{
	HUINT32		i;

	HxLOG_Info("\n");

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if( g_IrDrvScInfo[i].ReferenceID == wRefId )
		{
			g_IrDrvScInfo[i].ReferenceID = VD_REF_ID_ERROR;
			g_IrDrvScInfo[i].ScOpen.pfnStatusNotify = NULL;
			g_IrDrvScInfo[i].ScOpen.wSoftCellVal = 0;
		}
	}

	return IA_SUCCESS;

}

/********************************************************************
*	function name :	 HUMAX_VD_SC_IO
*	input : wRefId
*	output : result
*	description : Open할 때 등록된 callback함수를 close.
********************************************************************/
ia_result HUMAX_VD_SC_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	ia_result			result = IA_FAIL;
	HxLOG_Info("SC Transaction Start- wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X)\n", (HUINT32)wRefId, wOpCode, (HUINT32)wDataWriteSize);

	switch(wOpCode)
	{
		case VD_SC_COMMUNICATE:
			HxLOG_Info("\tVD_SC_COMMUNICATE\n");
			result = local_VdSc_CommunicateCard(wRefId, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
			break;

		case VD_SC_RESET:
			HxLOG_Info("\tVD_SC_RESET\n");
			result = HUMAX_ResetCard(wRefId);
			break;

		case VD_SC_GET_CARD_FORM:
			HxLOG_Info("\tVD_SC_GET_CARD_FORM\n");
			result =	local_VdSc_GetCardForm(wRefId, pwDataReadSize, pvDataRead);
			break;

		default:
			HxLOG_Info("unknown opcode (0x%x) \n", wOpCode);
			break;
	}

	HxLOG_Info("SC Transaction End - result(%s, 0x%X)\n", (result==IA_SUCCESS)?"SUCCESS":"FAIL", result );
	return result;

}


static void local_vd_SC_Callback(HUINT8 ulSlotID, HUINT8 ucCardStatus, void *pvInfo)
{
	if (ucCardStatus != VD_SC_CARD_IN)
	{
		g_IrDrvScInfo[ulSlotID].szScID[0] = '\0';
	}

	(*(g_IrDrvScInfo[ulSlotID].ScOpen.pfnStatusNotify))(g_IrDrvScInfo[ulSlotID].ReferenceID, g_IrDrvScInfo[ulSlotID].ScOpen.wSoftCellVal, ucCardStatus, (void *)pvInfo);
}


static void local_VdSc_pfnCbScStatus0(SciHandle_t hSci, HUINT8 ucSlotId, SvcCas_DevSciEvent_e eStatus)
{
	CAS_IR_BASE_MSG 	*pMsg;
	HxLOG_Info("ucSlotId(0x%X), eStatus(0x%X)\n", ucSlotId, eStatus);

	switch(eStatus)
	{
		case ePAL_SC_CARDEVENT_EXTRACTED:
			HxLOG_Print("Card Extracted\n");
			HxLOG_Print("SC Extracted 0\n");
#ifdef	IRPLAT_SYS_SC_EXTRACT_MSG
			pMsg = (CAS_IR_BASE_MSG*)OxCAS_Malloc(sizeof(CAS_IR_BASE_MSG));
			HxSTD_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
			pMsg->msg.SC.ucSlotID	= ucSlotId;
			pMsg->msg.SC.ResourceID	= g_IrDrvScInfo[ucSlotId].ResourceID;
			pMsg->msg.SC.ReferenceID= g_IrDrvScInfo[ucSlotId].ReferenceID;
			CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_CARD_OUT, (void*)pMsg);
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
			CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT);
#endif
#else
			if (g_IrDrvScInfo[ucSlotId].ScOpen.pfnStatusNotify != NULL)
			{
				if(g_IrDrvScInfo[ucSlotId].CardStatus != VD_SC_CARD_OUT)
				{
					local_vd_SC_Callback(ucSlotId, VD_SC_CARD_OUT, (void *)NULL);
					g_IrDrvScInfo[ucSlotId].CardStatus = VD_SC_CARD_OUT;
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
					CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT);
#endif
				}
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
				g_IrDrvScInfo[ucSlotId].CardPhysicalStatus = eCAS_SCI_SC_NOT_PRESENT;
#endif
			}
#endif

#if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
			CAS_IR_MGR_Notify_InstanceChanged(ucSlotId, NULL, 0, FALSE);	// ewlee 20101220
#endif // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)

#if defined (CONFIG_OP_ZIGGO_RFVOD) || defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
			svc_cas_MgrSubUpdateCasEtcEvent(0, HANDLE_NULL, eDxCAS_GROUPID_IR,
												EVT_RFVOD_SC_OUT, 0, 0, 0);
#endif
			break;

		case ePAL_SC_CARDEVENT_INSERTED:
			HxLOG_Print("SC Inserted 0\n");
			HxLOG_Print("Card Inserted(cb in)\n");
			pMsg = (CAS_IR_BASE_MSG *)OxCAS_Malloc(sizeof(CAS_IR_BASE_MSG));
			HxSTD_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
			pMsg->msg.SC.ucSlotID = ucSlotId;
			pMsg->msg.SC.ResourceID = g_IrDrvScInfo[ucSlotId].ResourceID;
			pMsg->msg.SC.ReferenceID = g_IrDrvScInfo[ucSlotId].ReferenceID;
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
			g_IrDrvScInfo[ucSlotId].CardPhysicalStatus = eCAS_SCI_SC_PRESENT;
#endif
			CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_CARD_IN, (void *)pMsg);
#if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
			CAS_IR_MGR_Notify_InstanceChanged(ucSlotId, NULL, 1, TRUE);	// ewlee 20101220
#endif // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
			CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_IN);
#endif

#if defined (CONFIG_OP_ZIGGO_RFVOD) || defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
			svc_cas_MgrSubUpdateCasEtcEvent(0, HANDLE_NULL, eDxCAS_GROUPID_IR,
												EVT_RFVOD_SC_IN, 0, 0, 0);
#endif

			break;
		default :
			HxLOG_Info("Unknown Message, ucSlotId(0x%X), eStatus(0x%X)\n", ucSlotId, eStatus);
			break;
	}

}

/********************************************************************
*	function name :
*	input :
*	output :
*	description :
********************************************************************/
static void local_VdSc_pfnCbScStatus1(SciHandle_t hSci, HUINT8 ucSlotId, SvcCas_DevSciEvent_e eStatus)
{
	CAS_IR_BASE_MSG 	*pMsg;
	HxLOG_Info("ucSlotId(0x%X), eStatus(0x%X)\n", ucSlotId, eStatus);

	switch(eStatus)
	{
		case ePAL_SC_CARDEVENT_EXTRACTED:
			HxLOG_Print("Card Extracted\n");
			HxLOG_Print("SC Extracted 1\n");
#ifdef	IRPLAT_SYS_SC_EXTRACT_MSG
			pMsg = (CAS_IR_BASE_MSG*)OxCAS_Malloc(sizeof(CAS_IR_BASE_MSG));
			HxSTD_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
			pMsg->msg.SC.ucSlotID	= ucSlotId;
			pMsg->msg.SC.ResourceID	= g_IrDrvScInfo[ucSlotId].ResourceID;
			pMsg->msg.SC.ReferenceID= g_IrDrvScInfo[ucSlotId].ReferenceID;
			CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_CARD_OUT, (void*)pMsg);
#else
			if (g_IrDrvScInfo[ucSlotId].ScOpen.pfnStatusNotify != NULL)
			{
				if(g_IrDrvScInfo[ucSlotId].CardStatus != VD_SC_CARD_OUT)
				{
					local_vd_SC_Callback(ucSlotId, VD_SC_CARD_OUT, (void *)NULL);
					g_IrDrvScInfo[ucSlotId].CardStatus = VD_SC_CARD_OUT;
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
					CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT);
#endif
				}
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
				g_IrDrvScInfo[ucSlotId].CardPhysicalStatus = eCAS_SCI_SC_NOT_PRESENT;
#endif
			}
#endif

#if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
			CAS_IR_MGR_Notify_InstanceChanged(ucSlotId, NULL, 0, FALSE);
#endif // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)

			break;

		case ePAL_SC_CARDEVENT_INSERTED:
			HxLOG_Print("SC Inserted 1\n");
			HxLOG_Print("Card Inserted(cb in)\n");
			pMsg = (CAS_IR_BASE_MSG *)OxCAS_Malloc(sizeof(CAS_IR_BASE_MSG));
			HxSTD_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
			pMsg->msg.SC.ucSlotID = ucSlotId;
			pMsg->msg.SC.ResourceID = g_IrDrvScInfo[ucSlotId].ResourceID;
			pMsg->msg.SC.ReferenceID = g_IrDrvScInfo[ucSlotId].ReferenceID;
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
			g_IrDrvScInfo[ucSlotId].CardPhysicalStatus = eCAS_SCI_SC_PRESENT;
#endif
			CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_CARD_IN, (void *)pMsg);

#if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
			CAS_IR_MGR_Notify_InstanceChanged(ucSlotId, NULL, 1, TRUE);
#endif // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
			CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_IN);
#endif
			break;
		default :
			HxLOG_Info("Unknown Message, ucSlotId(0x%X), eStatus(0x%X)\n", ucSlotId, eStatus);
			break;
	}

}

/********************************************************************
*	function name :
*	input :
*	output :
*	description :
********************************************************************/
ia_result local_VdSc_CommunicateCard( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	HUINT8			i;
	HUINT32			ulSlotID, ulErr;
	ia_result		result = IA_SUCCESS;
	HUINT8			*pWriteData;
	HUINT32			WriteSize;
	HUINT32			ReadSize;
	vd_sc_write_st	*p_sc_write_st;
	HUINT8			*p = (HUINT8 *)pvDataWrite;
	vd_sc_read_error_st	sc_read_error_st;
	HINT32			writeRetry;

	HxLOG_Info("\n");


	// prevent fix(added)
	if ( (pwDataReadSize == NULL) || (pvDataRead == NULL))
	{
		HxLOG_Critical("");

		return IA_INVALID_PARAMETER;
	}

#ifdef VD_SC_IO_DEBUG
	HxLOG_Info("\t\t[VD_SC] ++local_VdSc_CommunicateCard : wRefId(0x%X), wDataWriteSize(0x%X), pvDataWrite@(0x%X), pwDataReadSize(0x%X), pvDataRead@(0x%X)\n",
											wRefId, wDataWriteSize, pvDataWrite, *pwDataReadSize, pvDataRead);
#endif

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if( g_IrDrvScInfo[i].ReferenceID == wRefId )
		{
			break;
		}
	}

	if( i == HUMAX_VD_SC_SLOT_COUNT )
	{
		HxLOG_Print("no ReferenceID\n");
		return VD_WRONG_REF_ID;
	}

	ulSlotID = g_IrDrvScInfo[i].ResourceID;
	HxLOG_Info("SLOT #%d\n", ulSlotID);


#if (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	if(ERR_OK != svc_cas_DevScGetCardStatus(g_IrDrvScInfo[ulSlotID].hIRSci, &(g_IrDrvScInfo[ulSlotID].CardPhysicalStatus) ) )
	{
		HxLOG_Print("NO CARD FOR COMMUNICATE!!\n");
		sc_read_error_st.eError = VD_SC_HARDWARE_ERROR;
		return VD_HARDWARE_ERROR;
	}
#endif

	if( g_IrDrvScInfo[ulSlotID].CardPhysicalStatus == eCAS_SCI_SC_PRESENT )
	{
		HxLOG_Info("[CARD EXIST FOR COMMUNICATE!!]\n");

		p_sc_write_st = (vd_sc_write_st *)pvDataWrite;

		pWriteData = (HUINT8 *)OxCAS_Malloc(p_sc_write_st->bScHeaderLen + p_sc_write_st->bScPayloadLen);
		if (pWriteData == NULL)
		{
			HxLOG_Print("pWriteData == NULL \n");
			return VD_OS_ERROR;
		}
		HxSTD_memcpy(pWriteData, (HUINT8 *)(p+sizeof(vd_sc_write_st)), p_sc_write_st->bScHeaderLen + p_sc_write_st->bScPayloadLen);
		WriteSize = p_sc_write_st->bScHeaderLen + p_sc_write_st->bScPayloadLen;

		HxLOG_Info("SC_SEM :TRY ---\n");
		VK_SEM_Get(g_IrDrvScInfo[ulSlotID].SemaphoreID);
		HxLOG_Info("SC_SEM :GOT\n");
#ifdef VD_SC_IO_DEBUG
{
	HUINT8	j;

	HxLOG_Info("[WRITE DATA : SIZE(0x%X)]\n", WriteSize);
	for( j = 0; j < WriteSize; j++ )
	{
		HxLOG_Info("%02X ", pWriteData[j]);
		if( j % 16 == 15 )
			HxLOG_Info("\n");
	}
	HxLOG_Info("\n[END]\n");
}
#endif
		writeRetry = 0;
		do
		{
			// TODO: jichoi. adjust ReadSize.
			ReadSize = 255;
			ulErr = svc_cas_DevScTransfer(g_IrDrvScInfo[ulSlotID].hIRSci, pWriteData, WriteSize, (HUINT8 *)pvDataRead, &ReadSize, ReadSize);
		} while ((ulErr || ReadSize == 0) && ++writeRetry < MAX_NUM_RETRY);

		if (g_IrDrvScInfo[ulSlotID].Protocol == 14)
		{
			/*Actual payload size + Header overload(size 8) + Check sum (size 1)*/
			ReadSize = ((HUINT8*)pvDataRead)[7] + 8 + 1;
		}

#ifdef VD_SC_IO_DEBUG
{
	HUINT8	j;

	HxLOG_Info("[READ DATA : SIZE(0x%X)]\n", ReadSize);
	for( j = 0; j < ReadSize; j++ )
	{
		HxLOG_Info("%02X ", ((HUINT8*)pvDataRead)[j]);
		if( j % 16 == 15 )
			HxLOG_Info("\n");
	}
	HxLOG_Info("\n[END]\n");
}
#endif

		if(ulErr || ReadSize == 0)
		{
			HxLOG_Print("svc_cas_DevScTransfer (0x%X)\n", ulErr);
			// card가 빠진 경우와 timeout이 난 경우를 구별해줘야 할 필요가 있는지..
// Multi CAS 에서는 SC MGR를 통해 업데이트 한다. Physical Status는 별도로 업데이트 된다.
#if (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
			svc_cas_DevScGetCardStatus(g_IrDrvScInfo[ulSlotID].hIRSci, &(g_IrDrvScInfo[ulSlotID].CardPhysicalStatus));
#endif
			if( g_IrDrvScInfo[ulSlotID].CardPhysicalStatus == eCAS_SCI_SC_NOT_PRESENT )
			{
				sc_read_error_st.eError = VD_SC_CARD_OUT_ERROR;
				result = VD_WRITE_INCOMPLETE;
			}
			else
			{
				HxLOG_Print("CardPhysicalStatus= %d\n", g_IrDrvScInfo[ulSlotID].CardPhysicalStatus);
				#if 0
				if (g_ucScHardreset[ulSlotID] == 0)
				{
					HxLOG_Error("코드 논리상 이론상 불가능한 에러 \n");
					HxLOG_Critical("\n\n");
					g_ucScHardreset[ulSlotID] = 1;
					svc_cas_DevScDeactivate(g_IrDrvScInfo[ulSlotID].hIRSci);
				}
				#endif
				sc_read_error_st.eError = VD_SC_TIME_OUT;
				result = VD_WRITE_INCOMPLETE;
			}
		}
		else
		{
			*pwDataReadSize = (ia_word32)ReadSize;
			result = IA_SUCCESS;
		}

		OxCAS_Free(pWriteData);
		VK_SEM_Release(g_IrDrvScInfo[ulSlotID].SemaphoreID);
	}
	else
	{
		HxLOG_Print("NO CARD FOR COMMUNICATE!!\n");
		sc_read_error_st.eError = VD_SC_CARD_OUT_ERROR;
		result = VD_WRITE_INCOMPLETE;
	}

#ifdef VD_SC_IO_DEBUG
	HxLOG_Info("\t\t[VD_SC] --local_VdSc_CommunicateCard : result(0x%X)\n", result);
#endif

	if( result != IA_SUCCESS )
	{
		HxSTD_memcpy(pvDataRead, (HUINT8 *)&(sc_read_error_st), sizeof(sc_read_error_st));
		*pwDataReadSize = sizeof(sc_read_error_st);
	}

	return result;
}

static SvcCas_DevSciProtocol_e local_VdSc_ConvertProtocol(DI_SC_PROTOCOL eInProtocol)
{
	SvcCas_DevSciProtocol_e eScPotocol;

	switch(eInProtocol)
	{
		case DI_T_0:
			eScPotocol = eCAS_SCI_T0;
			break;
		case DI_T_1:
			eScPotocol = eCAS_SCI_T1;
			break;
		case DI_T_14:
			eScPotocol = eCAS_SCI_T14;
			break;
		default:
			eScPotocol = eCAS_SCI_T_END;
			break;
	}

	HxLOG_Print("[local_VdSc_ConvertProtocol] (0x%x, 0x%x)\n", eScPotocol, eInProtocol);
	return eScPotocol;
}

static HERROR local_VdSc_ParseAtr(HUINT8 *pucAtr, HUINT32 ulAtrLen,IrScAtrInfo *pstAtrInfo)
{
	HUINT8 *pucData;
	HUINT8 ucIndex = 1;
	HUINT8 ucTS, ucT0, ucY, ucTAx, ucTBx, ucTCx, ucTDx;
	HUINT8 ucModeT = 0;
	HUINT8 ucCurIndex = 0;
	HUINT8 ucSupportedProtocolTypes = 0;

	if ( (pucAtr == NULL) || (pstAtrInfo == NULL) )
	{
		HxLOG_Error("targets are null ...\n");
		return ERR_FAIL;
	}

	pucData = pucAtr;					// TS
	ucTS = *pucData;

	if ((ucTS != 0x3B) && (ucTS != 0x3F))
	{
		HxLOG_Error("Invalid TS value\n");
		return ERR_FAIL;
	}

	ucY = ucT0 = *(++pucData);
	pstAtrInfo->ucK = ucT0 & 0x0F;

	while (ucY)
	{
		HxLOG_Info("start ucIndex(%d)\t",ucIndex );
		if (ucY & 0x10) 		// TA
		{
			ucTAx = *(++pucData);
			if (ucIndex == 1)			// TA1
			{
				pstAtrInfo->ucFI = (ucTAx >> 4) & 0x0F;
				pstAtrInfo->ucDI = ucTAx & 0x0F;
				pstAtrInfo->bExistTA1 = TRUE;
			}
			else if (ucIndex == 2)		// TA2
			{
				pstAtrInfo->bSpecificMode = TRUE;
				pstAtrInfo->bPpsUnable = ((ucTAx & 0x80) == 0x80);
				pstAtrInfo->bImplicitlyDefined = ((ucTAx & 0x10) == 0x10);
			}
			else if (ucIndex > 2 && pstAtrInfo->ucT == 1) // TA3
			{
				pstAtrInfo->ucIFSC = ucTAx;
			}
		}
		else
		{
			if (ucIndex == 1)
			{
				/* If TA1 is absent, use ucFfactor = 1, ucDFactor = 1 */
				pstAtrInfo->ucFI = 1;
				pstAtrInfo->ucDI = 1;
				pstAtrInfo->bExistTA1 = FALSE;
			}
		}

		if (ucY & 0x20) 			// TB
		{
			ucTBx = *(++pucData);
			if (ucIndex == 1)				// TB1
			{
				pstAtrInfo->ucII = (ucTBx >> 5) & 0x07;
				pstAtrInfo->ucPI1 = ucTBx & 0x1F;
			}
			else if (ucIndex == 2)			// TB2
			{
				pstAtrInfo->ucPI2 = ucTBx;
			}
			else if (ucIndex > 2 && pstAtrInfo->ucT == 1) // TB3
			{
				pstAtrInfo->ucCWI = ucTBx & 0x0F;
				pstAtrInfo->ucBWI = (ucTBx >> 4) & 0x0F;
			}
		}

		if (ucY & 0x40) 		// TC
		{
			ucTCx = *(++pucData);
			if (ucIndex == 1)			// TC1
			{
				/* if mode is T1, TC1(FF) should be transfered with original FF value for setting mininum 11etu */
				pstAtrInfo->ucN = ucTCx;
			}
			else if (ucIndex == 2)		// TC2
			{
				pstAtrInfo->ucWI = ucTCx;
			}
			else if (ucIndex > 2 && pstAtrInfo->ucT == 1) // TC3
			{
				pstAtrInfo->ucEDC = ucTCx;
			}
		}

		if (ucY & 0x80) 		// TD
		{
			ucTDx = *(++pucData);
			ucModeT = (ucTDx & 0x0F);
			if (ucIndex == 1)			// TD1
			{
				pstAtrInfo->ucTFirst = ucTDx & 0x0F;
				pstAtrInfo->ucT = pstAtrInfo->ucTFirst;
			}
			else
			{
				pstAtrInfo->ucT = ucTDx & 0x0F;
			}
			ucY = ucTDx;	// next loop
		}
		else
		{
			ucY = 0;		// last loop
			if (ucIndex == 1)
			{
				/* default mode is T0 when TD1 is absent */
				ucModeT = 0;
			}
		}

		ucSupportedProtocolTypes |= (1 << ucModeT);
		ucIndex++;
	}

#if defined(CONFIG_MW_CAS_IRDETO_SMC_NEGOTIATE_MODE)
	{
		HUINT8 ucTck = 0;

		/* check historical byte size for checking Max ATR size validation */
		/* read the historical characters. */
		if (pstAtrInfo->ucK > 0)
		{
			pucData += pstAtrInfo->ucK;
			ucCurIndex = pucData - pucAtr +1;
		}

		HxLOG_Info("ATR Size atrlen %d,  curindex %d , K %d\n", ulAtrLen, ucCurIndex, pstAtrInfo->ucK);

	    /* TCK character should also be present only if list of supported protocol types is not t=0 by itself. */
		/* if in T = 1 mode, then TCK byte must be present and validated */
		if (ucSupportedProtocolTypes != 1)
		{
			HINT32 			i = 1;
			HUINT8 			ucTCKCompare = 0;

			ucTck = *pucData;		// 사용안함.
			// Start from T0 to TCK.  Including historical bytes if they exist */
			for (i=1; i<=ucCurIndex; i++)
			{
				ucTCKCompare = ucTCKCompare ^ pucAtr[i];
				HxLOG_Info("In SmartCardValidateTCKByte inp_ucATR[%d] = %02x, 0x%2x\n", i, pucAtr[i] , ucTCKCompare);

			}
			ucCurIndex++;			// inc index

			if (ucTCKCompare != 0)
			{
				///* this is called with EDC/Lrc in ATR !!!
				HxLOG_Info("Invalid TCK. : in EDC, Lrc !!!!!!!!!!! \n");
				return ERR_FAIL;
			}
			else
			{
				/* TCK validated successfully */
				HxLOG_Info("Valid TCK.!! \n");
			}
		}
		HxLOG_Info("ucSupportedProtocolTypes %d, ucTck %d \n", ucSupportedProtocolTypes, ucTck);

		/* Check Validation of ATR size : Only extra chars are allowed in ATR */
		if (ulAtrLen < ucCurIndex)
		{
			/* ATR Size Error */
			HxLOG_Info("ATR Size Error\n");
			return ERR_FAIL;
		}
	}
#endif

#if defined(CONFIG_DEBUG)
	local_VdSc_DebugParsedAtr(pstAtrInfo);
#endif
	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
static void local_VdSc_DebugParsedAtr(IrScAtrInfo *pstAtrInfo)
{
	HxLOG_Print("____ SC PARSED ATR INFO ____\n");

	if (pstAtrInfo == NULL)
		return;

	if (pstAtrInfo->bSpecificMode)
		HxLOG_Print("specific mode\n");
	else
		HxLOG_Print("negotiable mode\n");

	if (pstAtrInfo->bSpecificMode)
	{
		if(pstAtrInfo->bPpsUnable)
			HxLOG_Print("PPS Request unable\n");
		else
			HxLOG_Print("PPS Request available\n");
	}

	if (pstAtrInfo->bSpecificMode)
	{

		if (pstAtrInfo->bSpecificMode && pstAtrInfo->bImplicitlyDefined)
			HxLOG_Print("Parameter defined implicitly\n");
		else
			HxLOG_Print("Parameter defined explicitly. Use FI, DI\n");
	}

	HxLOG_Print("Historical bytes number (%d)\n", pstAtrInfo->ucK);

	HxLOG_Print("T first (%d), T (%d)\n", pstAtrInfo->ucTFirst, pstAtrInfo->ucT);

	HxLOG_Print("FI (%d), DI (%d), N(%d), WI (%d)\n", pstAtrInfo->ucFI, pstAtrInfo->ucDI, pstAtrInfo->ucN, pstAtrInfo->ucWI);

	if (pstAtrInfo->ucT == 1)
		HxLOG_Print("IFSC (%d), CWI (%d), BWI(%d)\n", pstAtrInfo->ucIFSC, pstAtrInfo->ucCWI, pstAtrInfo->ucBWI);

	HxLOG_Print("II (%d), PI1 (%d), PI2 (%d)\n", pstAtrInfo->ucII, pstAtrInfo->ucPI1, pstAtrInfo->ucPI2);

	HxLOG_Print("H:");

	HxLOG_Print("\n________________________________\n");
}
#endif

static HERROR local_VdSc_Negotiate(SciHandle_t hSci, IrScAtrInfo *pstAtrInfo)
{
	HUINT8 						ucWI;
	HUINT8 						ucBWI;
	HUINT8						ucT;
	SvcCas_DevSciScParam_t 		stScParam;
	SvcCas_DevSciScParam_t			*pstDefaultParams;
	HERROR						nErr = ERR_OK;

	if (pstAtrInfo == NULL)
	{
		HxLOG_Print("target is null ...\n");
		return ERR_FAIL;
	}

	HxSTD_memset(&stScParam, 0x00, sizeof(SvcCas_DevSciScParam_t));

#if defined(CONFIG_MW_CAS_IRDETO_SMC_NEGOTIATE_MODE)
	ucT = pstAtrInfo->ucTFirst;
#else
	ucT = pstAtrInfo->ucT;
#endif

	if (ucT == DI_T_0)
	{
		ucWI = pstAtrInfo->ucWI;
	}
	else
	{
		ucWI = pstAtrInfo->ucCWI;
	}

	if ( (ucT == DI_T_14) && (pstAtrInfo->ucBWI < 5) )
	{
		ucBWI = 5;
	}
	else
	{
		ucBWI = pstAtrInfo->ucBWI;
	}

	stScParam.eProtocol = local_VdSc_ConvertProtocol(ucT);

#if defined(CONFIG_MW_CAS_IRDETO_SMC_NEGOTIATE_MODE)


	if(stScParam.eProtocol == eCAS_SCI_T0)
	{
		pstDefaultParams = &s_stIrScInParamT0;
	}
	else
	{
		pstDefaultParams = &s_stIrScInParamT14;
	}

	if (pstAtrInfo->bSpecificMode)
	{
		HxLOG_Print("SPECIFIC\n");
		if (pstAtrInfo->bImplicitlyDefined)
		{
			HxLOG_Print("IMPLICIT\n");
			goto Atr_Parameters;
		}
		else
		{
			HxLOG_Print("EXPLICIT\n");
			goto Atr_Parameters;
		}
	}
	else	// negotiable mode
	{
		HxLOG_Print("NEGOTIABLE\n");

		if (pstAtrInfo->bExistTA1 == TRUE) // there should be TA1 for starting PPS
		{
			// check if default Fd, Dd
			if ( (pstAtrInfo->ucT == pstAtrInfo->ucTFirst)
				&& ((pstAtrInfo->ucFI == 1 && pstAtrInfo->ucDI == 1)
				|| (pstAtrInfo->ucFI == 0 && pstAtrInfo->ucDI == 1)) )		// only 1 protocol & Fd Dd : no need to proceed PPS
					goto Atr_Parameters;

			HxLOG_Print("START NEGOTIABLE PPS !!!!!!\n");
			nErr = local_VdSc_NegotiateByValueOrder(hSci, pstAtrInfo, ucWI, ucBWI, pstDefaultParams);
			if (nErr != ERR_OK)
				goto Default_Parameters;
			else
				return ERR_OK;
		}
		else
		{
			goto Atr_Parameters;
		}
	}

Default_Parameters:
	{
		HxLOG_Print("SET Default FI DI\n");


		nErr = svc_cas_DevScSetParams(hSci, pstDefaultParams);
		if (nErr != ERR_OK)
		{
			HxLOG_Error("err<%x>\n", nErr);
		}
		return ERR_OK;
	}
#endif


Atr_Parameters:
	/* valid check F, D here because we need to compare with original Fn, Dn value above */
	if (pstAtrInfo->ucFI == 0 && pstAtrInfo->ucDI == 0)
	{
		HxLOG_Print("(stScParam.ucFI == 0 && stScParam.ucDI == 0 )\n\n\n");
		pstAtrInfo->ucFI = 1;
		pstAtrInfo->ucDI = 1;
	}

	if (pstAtrInfo->ucDI < 1 || pstAtrInfo->ucDI > 9)
	{
		HxLOG_Print("(stScParam.ucDI < 1 ||stScParam.ucDI > 9) stScParam.ucDI = %d\n\n\n", pstAtrInfo->ucDI);
		pstAtrInfo->ucDI = 1;
	}

	stScParam.ulClock 		= pstAtrInfo->ulMinClock;
	stScParam.ulBaudrate 	= pstAtrInfo->ulBaudrate;
	stScParam.ucFI 			= pstAtrInfo->ucFI;
	stScParam.ucDI 			= pstAtrInfo->ucDI;
	stScParam.ucN 			= pstAtrInfo->ucN;
	stScParam.ucCWI 		= ucWI;
	stScParam.ucBWI 		= ucBWI;

	if (svc_cas_DevScSetParams(hSci, &stScParam) != ERR_OK)
	{
		HxLOG_Print("[local_VdSc_Negotiate] error in svc_cas_DevScSetParams\n");
		return ERR_FAIL;
	}
	return ERR_OK;

}

#if defined(CONFIG_MW_CAS_IRDETO_SMC_NEGOTIATE_MODE)
static HERROR local_VdSc_NegotiateByValueOrder(SciHandle_t hSci, IrScAtrInfo *pstAtrInfo, HUINT8 ucWI, HUINT8 ucBWI, SvcCas_DevSciScParam_t *pstDefaultParam)
{
	HUINT8			i;
	HUINT8			ucT;
	HUINT8			ucF, ucD;
	HUINT8			ucCntF, ucCntD;
	HUINT8			ucOriCntD;
	IrScAtrInfo 	*p;
	SvcCas_DevSciScParam_t 	stScParam;
	HERROR 			nErr = ERR_OK;

	p = pstAtrInfo;

	ucF = p->ucFI;
	ucD = p->ucDI;

	ucCntF = s_aucIrFiOrder[ucF];
	ucCntD = ucOriCntD = s_aucIrDiOrder[ucD];

	/* check if Fi and Di have valid value*/
	if (ucCntF == 0xFF)
	{
		ucCntF = 1;		// try once with default value
	}

	if (ucCntD == 0xFF)
	{
		ucCntD = 1;		// try once with default value
	}

	HxLOG_Print("[T=%x, %x]\n", p->ucT, p->ucTFirst);
	if (p->ucT == p->ucTFirst)
	{
		ucT = p->ucT;
		HxLOG_Print("UNIQUE T\n");
		while(ucCntF >= 1)		// ucCntF try
		{
			ucF = s_aucIrFiValue[ucCntF];
			while(ucCntD >= 1)	// ucCntD try
			{
				ucD = s_aucIrDiValue[ucCntD];
				HxLOG_Print("PPS T(%d) F(%d) D(%d) Dcnt(%d)\n", ucT, ucF, ucD, ucCntD);
				nErr = local_VdSc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
				if (nErr == ERR_OK)
				{
					stScParam.eProtocol = (PalSc_Protocol_e) ucT;
					stScParam.ulClock= (HUINT32) p->ulMinClock;
					stScParam.ulBaudrate = (HUINT32) p->ulBaudrate;
					stScParam.ucFI = ucF;
					stScParam.ucDI =ucD;
					stScParam.ucN = p->ucN;
					stScParam.ucCWI = ucWI;
					stScParam.ucBWI = ucBWI;
#if defined(CONFIG_DEBUG)
					HxLOG_Print("local_VdSc_NegotiateByValueOrder\n");
					HxLOG_Print("stScParam.eProtocol = %d\n", stScParam.eProtocol);
					HxLOG_Print("stScParam.ulClock = %d\n", stScParam.ulClock);
					HxLOG_Print("stScParam.ulBaudrate = %d\n", stScParam.ulBaudrate);
					HxLOG_Print("stScParam.ucFI = %d\n", stScParam.ucFI);
					HxLOG_Print("stScParam.ucDI = %d\n", stScParam.ucDI);
					HxLOG_Print("stScParam.ucN = %d\n", stScParam.ucN);
					HxLOG_Print("stScParam.ucCWI = %d\n", stScParam.ucCWI);
					HxLOG_Print("stScParam.ucBWI = %d\n", stScParam.ucBWI);
#endif

					nErr = svc_cas_DevScSetParams(hSci, &stScParam);
					if (nErr)
					{
						HxLOG_Error("[local_cxsc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
					}
					HxLOG_Print("T(%d) F(%d) D(%d)", ucT, ucF, ucD);
					return ERR_OK;
				}
				ucCntD--;
			}
			ucCntF--;
			ucCntD = ucOriCntD;
		}
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Print("MULTIPLE T\n");
		for (i=0; i<2; i++)
		{
			if (i == 0)
				ucT = p->ucTFirst;
			else
				ucT = p->ucT;

			while (ucCntF >= 1)		// ucCntF try
			{
				ucF = s_aucIrFiValue[ucCntF];
				while (ucCntD >= 1)	// ucCntD try
				{
					ucD = s_aucIrDiValue[ucCntD];
					HxLOG_Print("PPS T(%d) F(%d) D(%d)\n", ucT, ucF, ucD);
					nErr = local_VdSc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
					if (nErr == ERR_OK)
					{
						stScParam.eProtocol = ucT;
						stScParam.ulClock= p->ulMinClock;
						stScParam.ulBaudrate = p->ulBaudrate;
						stScParam.ucFI = ucF;
						stScParam.ucDI =ucD;
						stScParam.ucN = p->ucN;
						stScParam.ucCWI = ucWI;
						stScParam.ucBWI = ucBWI;

						nErr = svc_cas_DevScSetParams(hSci, &stScParam);
						if (nErr)
						{
							HxLOG_Error("[local_cxsc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
						}
						HxLOG_Print("T(%d) F(%d) D(%d)", ucT, ucF, ucD);
						return ERR_OK;
					}
					ucCntD--;
				}
				ucCntF--;
			}
		}
		return ERR_FAIL;
	}
}

static HERROR local_VdSc_RequestPps(SciHandle_t hSci, HUINT8 ucT, HUINT8 ucFi, HUINT8 ucDi, SvcCas_DevSciScParam_t *pstDefaultParam)
{
	HERROR 	nErr = ERR_OK;
	HUINT8 	aPpsRequest[4];
	HUINT8 	aPpsResponse[4] = {0,0,0,0};
	HUINT8 	ucNumRead;

#if defined(CONFIG_DEBUG)
	HxLOG_Print("local_VdSc_RequestPps\n");
	HxLOG_Print("pstDefaultParam->eProtocol = %d\n", pstDefaultParam->eProtocol);
	HxLOG_Print("pstDefaultParam->ulClock = %d\n", pstDefaultParam->ulClock);
	HxLOG_Print("pstDefaultParam->ulBaudrate = %d\n", pstDefaultParam->ulBaudrate);
	HxLOG_Print("pstDefaultParam->ucFI = %d\n", pstDefaultParam->ucFI);
	HxLOG_Print("pstDefaultParam->ucDI = %d\n", pstDefaultParam->ucDI);
	HxLOG_Print("pstDefaultParam->ucN = %d\n", pstDefaultParam->ucN);
	HxLOG_Print("pstDefaultParam->ucCWI = %d\n", pstDefaultParam->ucCWI);
	HxLOG_Print("pstDefaultParam->ucBWI = %d\n", pstDefaultParam->ucBWI);
#endif

	nErr = svc_cas_DevScSetParams(hSci, pstDefaultParam);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("err<%x>\n", nErr);
	}

	aPpsRequest[0] = 0xFF;
	aPpsRequest[1] = 0x10 | (ucT & 0x0F);
	aPpsRequest[2] = ( (ucFi << 4) & 0xF0 ) | (ucDi & 0x0F );
	aPpsRequest[3] = aPpsRequest[0] ^ aPpsRequest[1] ^ aPpsRequest[2];

	nErr = svc_cas_DevScNegotiatePTS(hSci, aPpsRequest, 4, aPpsResponse, &ucNumRead);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("err<%x>\n", nErr);
	}

	HxLOG_Print("Req : %x, %x, %x, %x\n", aPpsRequest[0], aPpsRequest[1], aPpsRequest[2], aPpsRequest[3]);
	HxLOG_Print("Res : %x, %x, %x, %x (%d)\n", aPpsResponse[0], aPpsResponse[1], aPpsResponse[2], aPpsResponse[3], ucNumRead);

	if ( (aPpsResponse[0] | aPpsResponse[1] | aPpsResponse[2] | aPpsResponse[3]) == 0 )
	{
		HxLOG_Error("err in data\n");
		return ERR_FAIL;
	}

	// skk
	// (1) response is same with request
	// (2) reponse(1-4) is same with request (1-4) and response(5-7) set to 0
//	if(memcmp(aPpsRequest, aPpsResponse, 4))
	if ( !((aPpsRequest[1] == aPpsResponse[1]) && (aPpsRequest[2] == aPpsResponse[2])) &&
		!((aPpsResponse[1] < 0x10) && ((aPpsRequest[1] & 0xF) == (aPpsResponse[1] & 0xF))) )
	{
		HxLOG_Error("reject \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#else

static HUINT32 local_VdSc_CheckProtocol( HUINT8 *pbData, HUINT8 bLength )
{
	HUINT32 ret_value;

	HxLOG_Info("\n")) ;

	/* For T= 14 as 710624.2.35 */
	if ( (pbData[0] == 0x3b) && ((pbData[1] & 0xf0) == 0x90)  && (pbData[2] == 0x21) && (pbData[3] == 0x0e) )
	{
	    ret_value = 14;
	}
	/* For card Version V4.1 and below as 710624.2.35 */
	else if ( (pbData[0] == 0x3b) && ((pbData[1] & 0xf0) == 0x90)	&& (pbData[3] == 0x40) )
	{
	    ret_value = 0;
	}
	/* Dircet convention card for compling ISO 7816  */
	else if ( (pbData[0] == 0x3b) && ((pbData[1] & 0x80) != 0x80)  )
	{
		HxLOG_Print("TD1 is not preset\n");
 		ret_value = 0;
	}
	else
	{
		HxLOG_Info("Check this ATR data \n")) ;
		ret_value = -1;
	}

	return (ret_value);

}

static HVD_Result local_VdSc_CheckATR( HUINT32 ulSlotId, HUINT8 *buf, HUINT32 ulAtrLen, HUINT32 *pnProtocol )
{
	HUINT8	ret_value;
	HxLOG_Info("\n")) ;

	if ( ulAtrLen > 0 )
	{
		*pnProtocol  = g_IrDrvScInfo[ulSlotId].Protocol  = local_VdSc_CheckProtocol(buf, (HUINT8)ulAtrLen);

            // validate the check-sum

		if ( *pnProtocol  == 14 )
		{
			 // Check the validity of smart card. card should contain the string "IRDETO ACS V1."
			if ( memcmp(buf + 4, IRDETO_CARD_HISTORY_CHARS, 10) == 0 )
			{
				HxLOG_Print("### T14 : VALID CARD\n");
				  // save the version of smart card
	       	       HxSTD_memcpy( g_IrDrvScInfo[ulSlotId].VersionFromATR, buf + 15, sizeof(g_IrDrvScInfo[ulSlotId].VersionFromATR) );
	              	ret_value = eHVD_OK;
			}
			else
			{
				HxLOG_Print("### T14 : INVALID CARD\n");
				ret_value = eHVD_ERR_NOT_IRDETOCARD;
			}
		}
		else if(*pnProtocol  == 0)
		{
			if ( memcmp(buf + 5, IRDETO_CARD_HISTORY_CHARS, 10) == 0 )
	       	{
				HxLOG_Print("### T0 : VALID CARD\n");
				// save the version of smart card
				HxSTD_memcpy( g_IrDrvScInfo[ulSlotId].VersionFromATR, buf + 16, 4 );
				ret_value = eHVD_OK;
			}
			else
			{
				HxLOG_Print("### T0 : INVALID CARD\n");
				ret_value = eHVD_ERR_NOT_IRDETOCARD;
			}
	        }
	        else
	        {
			HxLOG_Print("### INVALID PROTOCOL\n");
			ret_value = eHVD_ERR_INVALID_CARD;
	       }
	}
	else
	{
		HxLOG_Print("NO ATR DATA\n");
		ret_value = eHVD_ERR_INVALID_CARD;
	}

	return ret_value ;

}
#endif

ia_result HUMAX_ResetCard( vd_ref_id wRefId )
{
	ia_result					ret= IA_FAIL;
	HUINT8					i;
	HUINT32					ulSlotID, ulErr, resCheckATR;
	SvcCas_DevSciScParam_t		*pstInScParam;
	HUINT8					*p, *pvInfo;
	HUINT8					ucResetTryCount=0 ;
	SvcCas_DevScmResetType_e	eScResetType =eCAS_SCM_RESET_TYPE_COLD ;

	HxLOG_Print("wRefId = 0x%X\n", (HUINT32)wRefId);

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if( g_IrDrvScInfo[i].ReferenceID == wRefId )
		{
			break;
		}
	}

	if( i == HUMAX_VD_SC_SLOT_COUNT )
	{
		HxLOG_Print("No ReferenceID(wRefId= 0x%X)\n", (HUINT32)wRefId);
		return VD_WRONG_REF_ID;
	}

	ulSlotID = g_IrDrvScInfo[i].ResourceID;

	HxLOG_Print("[SLOT #%d]\n", ulSlotID);

	if( ulSlotID >= HUMAX_VD_SC_SLOT_COUNT )
	{
		HxLOG_Print("Invalid slotID\n");
		return VD_WRONG_REF_ID;
	}

// Multi CAS 에서는 SC MGR를 통해 업데이트 한다. Physical Status는 별도로 업데이트 된다.
#if (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	svc_cas_DevScGetCardStatus(g_IrDrvScInfo[ulSlotID].hIRSci, &(g_IrDrvScInfo[ulSlotID].CardPhysicalStatus));
#else
	// in case of multi-cas, it will be updated by its event from sc manager
#endif

	if( g_IrDrvScInfo[ulSlotID].CardPhysicalStatus != eCAS_SCI_SC_PRESENT )
	{
		HxLOG_Print("[NO CARD FOR RESET!!]\n");

		if(g_IrDrvScInfo[ulSlotID].CardStatus != VD_SC_CARD_OUT)
		{
			local_vd_SC_Callback(ulSlotID, VD_SC_CARD_OUT, (void *)NULL);
			g_IrDrvScInfo[ulSlotID].CardStatus = VD_SC_CARD_OUT;
		}
		return IA_FAIL;
	}

	HxLOG_Print("[CARD EXIST FOR RESET!!]\n");
	VK_SEM_Get(g_IrDrvScInfo[ulSlotID].SemaphoreID);

DO_RESET:
	HxSTD_memset(g_IrDrvScInfo[ulSlotID].aucATR, 0, sizeof(g_IrDrvScInfo[ulSlotID].aucATR) );

#if defined (IRPLAT_SYS_SC_T14_FIRST) || defined(CONFIG_MW_CAS_IRDETO_SECURE) /* 생산용 SST TEST를 위해서 필요한 config */
	if(ucResetTryCount < T14_RETRY_COUNT)
	{
		HxLOG_Print("T=0, ucResetTryCount=%d\n", ucResetTryCount);
		pstInScParam = &s_stIrScInParamT14;
	}
	else
	{
		HxLOG_Print("T=14, ucResetTryCount=%d\n", ucResetTryCount);
		pstInScParam = &s_stIrScInParamT0;
	}
#else
	if(ucResetTryCount < T0_RETRY_COUNT)
	{
		HxLOG_Print("T=0, ucResetTryCount=%d\n", ucResetTryCount);
		pstInScParam = &s_stIrScInParamT0;
	}
	else
	{
		HxLOG_Print("T=14, ucResetTryCount=%d\n", ucResetTryCount);
		pstInScParam = &s_stIrScInParamT14;
	}
#endif

	ucResetTryCount++;

	ulErr = svc_cas_DevScReset( g_IrDrvScInfo[ulSlotID].hIRSci,
						g_IrDrvScInfo[ulSlotID].aucATR,
						&(g_IrDrvScInfo[ulSlotID].ulAtrLen),
						eScResetType,
						pstInScParam);
	if(ulErr == ERR_OK)
	{
#if defined(CONFIG_DEBUG)
		HxLOG_Print ("\tATR DATA = ");
		for(i=0; i< g_IrDrvScInfo[ulSlotID].ulAtrLen; i++)
		{
			HxLOG_Print("%02X ", g_IrDrvScInfo[ulSlotID].aucATR[i] );
		}
		HxLOG_Print("\n");
#endif
		VK_memset32(&s_stIrAtrInfo, 0, sizeof(IrScAtrInfo));

		ulErr = local_VdSc_ParseAtr( g_IrDrvScInfo[ulSlotID].aucATR, g_IrDrvScInfo[ulSlotID].ulAtrLen, &s_stIrAtrInfo);

	 	if (ulErr == ERR_OK)
		{
			HxLOG_Print("Parse atr, Ok\n");
			ulErr = local_VdSc_Negotiate( g_IrDrvScInfo[ulSlotID].hIRSci, &s_stIrAtrInfo);
		 	if (ulErr == ERR_OK)
			{
				HxLOG_Print("Negotiate Ok \n");
		 	}
			else
			{
				HxLOG_Print("Negotiate error = %d\n", ulErr);
			}
		}
		else
		{
			HxLOG_Print("Parse atr, ulErr = %d\n", ulErr);
		}
	}
	else
	{
		HxLOG_Print("Reset error occured. (%d)\n", ulErr);
	}

	if( (ERR_OK != ulErr ) &&  (ucResetTryCount < (T0_RETRY_COUNT+ T14_RETRY_COUNT) ))
		goto DO_RESET;

	if(ERR_OK != ulErr )
	{
		HxLOG_Print("Reset : ulErr(0x%X)\n", ulErr);
		svc_cas_DevScGetCardStatus(g_IrDrvScInfo[ulSlotID].hIRSci, &(g_IrDrvScInfo[ulSlotID].CardPhysicalStatus));
		if( g_IrDrvScInfo[ulSlotID].CardPhysicalStatus == eCAS_SCI_SC_PRESENT )
		{
			pvInfo = OxCAS_Malloc(sizeof(vd_sc_card_in_error_st));
			((vd_sc_card_in_error_st *)pvInfo)->eCardError = VD_SC_HARDWARE_ERROR;
			local_vd_SC_Callback(ulSlotID, VD_SC_CARD_IN_ERROR, (void *)pvInfo);
			g_IrDrvScInfo[ulSlotID].CardStatus = VD_SC_CARD_IN_ERROR;
			g_IrDrvScInfo[ulSlotID].ErrorReason = VD_SC_HARDWARE_ERROR;
			OxCAS_Free(pvInfo);
		}
		else
		{
			if(g_IrDrvScInfo[ulSlotID].CardStatus != VD_SC_CARD_OUT)
			{
				local_vd_SC_Callback(ulSlotID, VD_SC_CARD_OUT, (void *)NULL);
				g_IrDrvScInfo[ulSlotID].CardStatus = VD_SC_CARD_OUT;
			}
			g_IrDrvScInfo[ulSlotID].ErrorReason = 0xFFFFFFFF;
		}
	}
#if defined(CAS_IR_ICE2_PROJECT)
	else
	{
		svc_cas_DevScNotifyScValidity(g_IrDrvScInfo[ulSlotID].hIRSci, TRUE);

		g_IrDrvScInfo[ulSlotID].AtrLen = (HUINT16)ucAtrLen;
		resCheckATR = local_VdSc_CheckATR_ICE2( ulSlotID, g_IrDrvScInfo[ulSlotID].aucATR, (HUINT32)ucAtrLen, &g_IrDrvScInfo[ulSlotID].Protocol );

		HxLOG_Print(("[VD_SC] ATR ret=%d\n", resCheckATR);

		if ( resCheckATR == HIR_OK )
		{
			pvInfo = OxCAS_Malloc(sizeof(vd_sc_atr_st) + g_IrDrvScInfo[ulSlotID].AtrLen);
			((vd_sc_atr_st *)pvInfo)->wAtrLength = g_IrDrvScInfo[ulSlotID].AtrLen;
			p = pvInfo + sizeof(vd_sc_atr_st);
			HxSTD_memcpy(p, g_IrDrvScInfo[ulSlotID].aucATR, g_IrDrvScInfo[ulSlotID].AtrLen);

			if(g_IrDrvScInfo[ulSlotID].CardStatus != VD_SC_CARD_IN)
			{
				local_vd_SC_Callback(ulSlotID, VD_SC_CARD_IN, (void *)pvInfo);
				g_IrDrvScInfo[ulSlotID].CardStatus = VD_SC_CARD_IN;
			}
			OxCAS_Free(pvInfo);

			ret = IA_SUCCESS;
		}
		else
		{
			pvInfo = OxCAS_Malloc(sizeof(vd_sc_card_in_error_st));
			((vd_sc_card_in_error_st *)pvInfo)->eCardError = VD_SC_HARDWARE_ERROR;
			local_vd_SC_Callback(ulSlotID, VD_SC_CARD_IN_ERROR, (void *)pvInfo);
			g_IrDrvScInfo[ulSlotID].CardStatus = VD_SC_CARD_IN_ERROR;
			OxCAS_Free(pvInfo);
		}
	}
#else
	else
	{
		/* reset success */
	#if defined(CONFIG_MW_CAS_IRDETO_SMC_NEGOTIATE_MODE)
		resCheckATR = eHVD_OK;
	#else
		resCheckATR = local_VdSc_CheckATR( ulSlotID, g_IrDrvScInfo[ulSlotID].aucATR, g_IrDrvScInfo[ulSlotID].ulAtrLen, &g_IrDrvScInfo[ulSlotID].Protocol );
	#endif

		HxLOG_Print("Check ATR ret=%d\n", resCheckATR);

		if ( resCheckATR == eHVD_OK )
		{
			pvInfo = OxCAS_Malloc(sizeof(vd_sc_atr_st) + g_IrDrvScInfo[ulSlotID].ulAtrLen);
			((vd_sc_atr_st *)pvInfo)->wAtrLength = g_IrDrvScInfo[ulSlotID].ulAtrLen;
			p = pvInfo + sizeof(vd_sc_atr_st);
			HxSTD_memcpy(p, g_IrDrvScInfo[ulSlotID].aucATR, g_IrDrvScInfo[ulSlotID].ulAtrLen);

			if(g_IrDrvScInfo[ulSlotID].CardStatus != VD_SC_CARD_IN)
			{
				local_vd_SC_Callback(ulSlotID, VD_SC_CARD_IN, (void *)pvInfo);
				g_IrDrvScInfo[ulSlotID].CardStatus = VD_SC_CARD_IN;
			}
			OxCAS_Free(pvInfo);
			g_IrDrvScInfo[ulSlotID].ErrorReason = 0xFFFFFFFF;

			ret =  IA_SUCCESS;
		}
		else if( resCheckATR == eHVD_ERR_INVALID_CARD )
		{
			pvInfo = OxCAS_Malloc(sizeof(vd_sc_card_in_error_st));
			((vd_sc_card_in_error_st *)pvInfo)->eCardError = VD_SC_ATR_INVALID_FORMAT;
			local_vd_SC_Callback(ulSlotID, VD_SC_CARD_IN_ERROR, (void *)pvInfo);
			g_IrDrvScInfo[ulSlotID].ErrorReason = VD_SC_ATR_INVALID_FORMAT;
			g_IrDrvScInfo[ulSlotID].CardStatus = VD_SC_CARD_IN_ERROR;

			OxCAS_Free(pvInfo);
		}
		else if( resCheckATR == eHVD_ERR_NOT_IRDETOCARD )
		{
			pvInfo = OxCAS_Malloc(sizeof(vd_sc_card_in_error_st));
			((vd_sc_card_in_error_st *)pvInfo)->eCardError = VD_SC_ATR_INVALID_HISTORY;
			local_vd_SC_Callback(ulSlotID, VD_SC_CARD_IN_ERROR, (void *)pvInfo);
		        g_IrDrvScInfo[ulSlotID].CardStatus = VD_SC_CARD_IN_ERROR;

			g_IrDrvScInfo[ulSlotID].ErrorReason = VD_SC_ATR_INVALID_HISTORY;

			OxCAS_Free(pvInfo);
		}
		else
		{
			pvInfo = OxCAS_Malloc(sizeof(vd_sc_card_in_error_st));
			((vd_sc_card_in_error_st *)pvInfo)->eCardError = VD_SC_HARDWARE_ERROR;
			(*(g_IrDrvScInfo[ulSlotID].ScOpen.pfnStatusNotify))(wRefId, g_IrDrvScInfo[ulSlotID].ScOpen.wSoftCellVal, VD_SC_CARD_IN_ERROR, pvInfo);
		        g_IrDrvScInfo[ulSlotID].CardStatus = VD_SC_CARD_IN_ERROR;
			g_IrDrvScInfo[ulSlotID].ErrorReason = VD_SC_HARDWARE_ERROR;

			OxCAS_Free(pvInfo);
		}

	}
#endif

	VK_SEM_Release(g_IrDrvScInfo[ulSlotID].SemaphoreID);

	return ret;

}


#ifdef	IRPLAT_SYS_SC_EXTRACT_MSG
void	HUMAX_ExtractCard(HUINT8 ucSlotId)
{
	if (g_IrDrvScInfo[ucSlotId].ScOpen.pfnStatusNotify)
	{
		if(g_IrDrvScInfo[ucSlotId].CardStatus != VD_SC_CARD_OUT)
		{
			local_vd_SC_Callback(ucSlotId, VD_SC_CARD_OUT, (void *)NULL);
			g_IrDrvScInfo[ucSlotId].CardStatus = VD_SC_CARD_OUT;
		}
	}
}
#endif

#if defined(CAS_IR_ICE2_PROJECT)
static HUINT32 local_VdSc_CheckATR_ICE2( HUINT32 ulSlotId, HUINT8 *buf, HUINT32 nData, HUINT32 *pnProtocol )
{
	if ( nData > 0 )
	{
		s_iCardType = local_VdSc_CheckProtocol(buf, (HUINT8)nData);
		*pnProtocol = s_iCardType;
            // validate the check-sum

		if ( s_iCardType == 14 )
		{
			HxLOG_Print("### T14 : VALID CARD\n");
			HxSTD_memcpy( g_IrDrvScInfo[ulSlotId].VersionFromATR, buf + 15, 4 );

		}
		else if(s_iCardType == 0)
		{
			HxLOG_Print("### T0 : VALID CARD\n");
				// save the version of smart card
			HxSTD_memcpy( g_IrDrvScInfo[ulSlotId].VersionFromATR, buf + 16, 4 );
#if defined(SUPPORT_IRDETO_ZETA_INFINEON)
			if (!memcmp(g_IrDrvScInfo[ulSlotId].VersionFromATR, "V5.4", 4))
			{
				SCI_SetCardVersion(g_IrDrvScInfo[ulSlotId].hIRSci, 54);
			}
			else
			{
				SCI_SetCardVersion(g_IrDrvScInfo[ulSlotId].hIRSci, 0);
			}
#endif
		}
		else
		{
			HxLOG_Print("Non-T=0, T=14 protocol smartcard is inserted\n");
		}
	}
	else
	{
		int i;
		for(i=0;i<100; i++)
		{
			HxLOG_Print("jichoi:local_VdSc_CheckATR_ICE2: ATR length is zero\n");
		}
		return HIR_NOT_OK;
	}

	//always return HIR_OK.
	return HIR_OK;
}

#endif
/* end of file */

HUINT32 local_VdSc_GetCardForm( vd_ref_id wRefId, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_sc_get_card_form_st		sc_get_card_form_st;

	HxLOG_Info("\n");

	if ((pwDataReadSize == NULL) || (pvDataRead == NULL) ) //fixed a prevent issue
	{
		HxLOG_Critical("");

		return IA_INVALID_PARAMETER;
	}
	sc_get_card_form_st.eScForm = g_IrDrvScInfo[wRefId].eCardForm;
	HxSTD_memcpy( pvDataRead, &sc_get_card_form_st, sizeof(vd_sc_get_card_form_st) );
	*pwDataReadSize = sizeof(vd_sc_get_card_form_st);

	return 	IA_SUCCESS;

}

// ewlee 20100817
void local_VdSc_HashCodeConvert(char* szInSCNumber, char* szOutHashedCode )
{
	/* szInSCNumber input format: "4016372592 (8)" */
	char paucConversionLetterTable[20];
	char paucSmartCardNum[12];
	int i,j;

	/*step 1. make fingerprint hashcode reverse-conversion table & initialize string variables*/
	for(i=0;i<20; i++)
	{
		paucConversionLetterTable[i]= 'B' + i;
	}

	memcpy( paucSmartCardNum, szInSCNumber, 10);
	paucSmartCardNum[10] = szInSCNumber[12];
	paucSmartCardNum[11] = 0;

	/*step 2. Convert from lowest digit */
	for(i=0;i<CA_MAX_LENGTH_VDSC_SERIAL_NUMBER; i++)
	{
		/* loop digit */
		for(j=0;j<10;j++)
		{
			if( paucSmartCardNum[10- i] == ('0' + j) )
			{
				szOutHashedCode[i] = paucConversionLetterTable[9-j+i] ;
				break;
			}
		}
	}
}

void local_VdSc_MSR_DRV_HashCodeConvert(char* szInSCNumber, char* szOutHashedCode )
{
	/* szInSCNumber input format: "4016372592 (8)" */
	char paucConversionLetterTable[20];
	char paucSmartCardNum[12];
	int i,j;

	/*step 1. make fingerprint hashcode reverse-conversion table & initialize string variables*/
	for(i=0;i<20; i++)
	{
		paucConversionLetterTable[i]= 'B' + i;
	}

	memcpy( paucSmartCardNum, szInSCNumber, 10);
	paucSmartCardNum[10] = szInSCNumber[12];
	paucSmartCardNum[11] = 0;

	/*step 2. Convert from lowest digit */
	for(i=0;i<CA_MAX_LENGTH_VDSC_SERIAL_NUMBER; i++)
	{
		/* loop digit */
		for(j=0;j<10;j++)
		{
			if( paucSmartCardNum[10- i] == ('0' + j) )
			{
				szOutHashedCode[i] = paucConversionLetterTable[9-j+i] ;
				break;
			}
		}
	}
}

// ewlee 20100817
void HUMAX_VdSc_HashCodeConvert(void)
{
	IR_DRV_SC_INFO 	stIrSCInfo;

	if( HIR_OK != XSVC_CAS_IR_DRV_GetScInfo(0 , &stIrSCInfo) )
	{
		return;
	}

	/* if 2 smartcard slot, it should be changed into getting resource */
	/* following case means that just once ECM fingerprint case(MSG_SMARTCARD_ID) was occured. */
	if(g_IrDrvScInfo[0].szScID[0] == '\0')
	{
		local_VdSc_MSR_DRV_HashCodeConvert(stIrSCInfo.szScID,g_IrDrvScInfo[0].szScID);
	}
	HxLOG_Info("g_IrDrvScInfo[0].szScID: %s \n", g_IrDrvScInfo[0].szScID);
}

#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER)
HBOOL HUMAX_VdSc_IsSameWithSavedSmartcardNumber()
{
	char		szSavedNumber[IR_SC_INFO_NUMBER_LENGTH];
	HUINT8		szCurrentNumber[IR_PARTION_CUSTOM_SMARTCARD_NUMBER_SIZE];
	HIR_ERR	err;

	// 일단 multi card 는 복잡하니 card 가 하나인 경우만 하자
	if (g_IrDrvScInfo[0].CardStatus != VD_SC_CARD_IN)
	{
		return FALSE;
	}

	err = CAS_IR_NVR_ReadSmartcardNumber (szSavedNumber, IR_SC_INFO_NUMBER_LENGTH);
	if (err != HIR_OK)
	{
		return FALSE;
	}

	// irdeto status 화면에 맞추어 0000000000(0) 으로 되어 있다
	HxSTD_memcpy (szCurrentNumber, g_IrDrvScInfo[0].stIrScApiInfo.szNumber, IR_SC_INFO_NUMBER_LENGTH);
	szCurrentNumber[10] = szCurrentNumber[12];
	szCurrentNumber[11] = '\0';

	if (VK_strcmp (szCurrentNumber, szSavedNumber) == 0)
	{
		return TRUE;
	}

	return 	FALSE;
}

void HUMAX_VdSc_SaveSmartcardNumber ()
{
	HUINT8		szNumber[IR_PARTION_CUSTOM_SMARTCARD_NUMBER_SIZE];

	// 일단 multi card 는 복잡하니 card 가 하나인 경우만 하자
	// irdeto status 화면에 맞추어 0000000000(0) 으로 되어 있다
	HxSTD_memcpy (szNumber, g_IrDrvScInfo[0].stIrScApiInfo.szNumber, IR_SC_INFO_NUMBER_LENGTH);
	szNumber[10] = szNumber[12];
	szNumber[11] = '\0';

	CAS_IR_NVR_SaveSmartcardNumber (szNumber, IR_SC_INFO_NUMBER_LENGTH);
}
#endif

