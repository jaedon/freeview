/***************************************************************
* $Workfile:   ci_reshandler.c  $
* $Modtime:   13 Dec 2004 09:27:32  $
* Auther : Jaehee Cho / hmkim (CI+)
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include <vkernel.h>	// cipack_msg.h 에서 include 하고 있기 때문에 제거되어야 맞다.
#include <pal_ci.h>
#include "cipack_msg.h"

#include "ci.h"
#include "ci_reshandler.h"

#include "ci_rm.h"
#include "ci_ai.h"
#include "ci_cas.h"
#include "ci_dt.h"
#include "ci_mmi.h"

#ifdef CONFIG_MW_CI_PLUS
	#include "ci_plus.h"
	#include "ci_hci.h"
	#include "ci_ammi.h"
	#include "ci_cc_main.h"
	#include "ci_hlc.h"
	#include "ci_cu.h"
	#if defined(CONFIG_MW_CI_PLUS_OP_V_1)
		#include "ci_op.h"
	#endif
	#if defined(CONFIG_MW_CI_PLUS_LSC_V_3)
		#include "ci_lsc.h"
	#endif
#endif

#ifdef CONFIG_MW_CI_PLUS_SAS
	#include "ci_sas.h"
#endif

#ifdef CI_PLUS_PVR_ENABLE
	#include "ci_capvr.h"
#endif

#ifdef CONFIG_OP_CANALREADY
	#include "ci_auth.h"
#endif

#include "itc.h"
#include "../ci_pack/trans.h"

#include <_svc_cas_sub_dev_api.h>


/***************************************************************
* external variables and structures
***************************************************************/
extern trpt t_tab_tcid[MAXTCID];   /* tcid table */

/***************************************************************
* static variables and structures
***************************************************************/
static CI_RES_INST s_aCiHostRes[] = {
/*
		uResId,						usSrcId,		ucResIdType,	usResClass,		ucMaxSs,		ucSsCnt,
		cbList (CI_CALLBACK_INIT, CI_CALLBACK_SS_OPENED, CI_CALLBACK_SS_CLOSED, CI_CALLBACK_APDU)
*/
	{	// Resource Manager v2
		{CI_RM_RES_V2_ID},			CI_HOST_RES,		0x0,		CI_RM_CLASS,	MAX_CI_RM_SS,		0,
		{CiRm_Init,		CiRm_SsOpenedProc,		CiRm_SsClosedProc,		CiRm_ApduProc}
	},
	{	// Application Information v3
		{CI_AI_RES_V3_ID},			CI_HOST_RES,		0x0,		CI_AI_CLASS,	MAX_CI_AI_SS,		0,
		{CiAi_Init,		CiAi_SsOpenedProc,		CiAi_SsClosedProc,		CiAi_ApduProc}
	},
	{	// Conditional Access Support v1
		{CI_CAS_RES_V1_ID}, 		CI_HOST_RES,		0x0,		CI_CAS_CLASS,	MAX_CI_CAS_SS,		0,
		{CiCas_Init,		CiCas_SsOpenedProc,		CiCas_SsClosedProc,		CiCas_ApduProc}
	},
	{	// Date & Time v1
		{CI_DT_RES_V1_ID},			CI_HOST_RES,		0x0,		CI_DT_CLASS,	MAX_CI_DT_SS,		0,
		{CiDt_Init,		CiDt_SsOpenedProc,		CiDt_SsClosedProc,		CiDt_ApduProc}
	},
	{	// Man Machine Interface v1
		{CI_MMI_RES_V1_ID},			CI_HOST_RES,		0x0,		CI_MMI_CLASS,	MAX_CI_MMI_SS,		0,
		{CiMmi_Init,		CiMmi_SsOpenedProc,		CiMmi_SsClosedProc,		CiMmi_ApduProc}
	}
#ifdef CONFIG_MW_CI_PLUS
	,
	#ifdef CONFIG_MW_CI_PLUS_HCI_V_2 // 나중에 CI+ v1.3 이 default 가 되면 ifdef 제거하고 일반적으로 처리하도록 할 것
	{	// Host Control Interface v2
		{CI_HCI_RES_V2_ID},			CI_HOST_RES,		0x0,		CI_HCI_CLASS,	MAX_CI_HCI_SS,		0,
		{CiHci_Init,		CiHci_SsOpenedProc,		CiHci_SsClosedProc,		CiHci_ApduProc}
	},
	#else
	{	// Host Control Interface v1
		{CI_HCI_RES_V1_ID},			CI_HOST_RES,		0x0,		CI_HCI_CLASS,	MAX_CI_HCI_SS,		0,
		{CiHci_Init,		CiHci_SsOpenedProc,		CiHci_SsClosedProc,		CiHci_ApduProc}
	},
	#endif
	#ifdef CONFIG_MW_CI_PLUS_AMMI_V_2 // 나중에 CI+ v1.3 이 default 가 되면 ifdef 제거하고 일반적으로 처리하도록 할 것
	{	// CI+ Browser Application MMI v2
		{CI_AMMI_RES_V2_ID},		CI_HOST_RES,		0x0,		CI_AMMI_CLASS,	MAX_CI_AMMI_SS,		0,
		{CiAmmi_Init,		CiAmmi_SsOpenedProc,		CiAmmi_SsClosedProc,		CiAmmi_ApduProc}
	},
	#else
	{	// CI+ Browser Application MMI v1
		{CI_AMMI_RES_V1_ID},		CI_HOST_RES,		0x0,		CI_AMMI_CLASS,	MAX_CI_AMMI_SS,		0,
		{CiAmmi_Init,		CiAmmi_SsOpenedProc,		CiAmmi_SsClosedProc,		CiAmmi_ApduProc}
	},
	#endif
	#ifdef CONFIG_MW_CI_PLUS_CC_V_2 // 나중에 CI+ v1.3 이 default 가 되면 ifdef 제거하고 일반적으로 처리하도록 할 것
	{	// Content Control v2
		{CI_CC_RES_V2_ID},			CI_HOST_RES,		0x0,		CI_CC_CLASS,	MAX_CI_CC_SS,		0,
		{CiCc_Init,		CiCc_SsOpenedProc,		CiCc_SsClosedProc,		CiCc_ApduProc}
	},
	#endif
	{	// Content Control v1
		{CI_CC_RES_V1_ID},			CI_HOST_RES,		0x0,		CI_CC_CLASS,	MAX_CI_CC_SS,		0,
		{CiCc_Init,		CiCc_SsOpenedProc,		CiCc_SsClosedProc,		CiCc_ApduProc}
	},

	{	// Host Language & Country v1
		{CI_HLC_RES_V1_ID},			CI_HOST_RES,		0x0,		CI_HLC_CLASS,	MAX_CI_HLC_SS,		0,
		{CiHlc_Init,		CiHlc_SsOpenedProc,		CiHlc_SsClosedProc,		CiHlc_ApduProc}
	},
	{	// CAM Firmware Upgrade v1
		{CI_CU_RES_V1_ID},			CI_HOST_RES,		0x0,		CI_CU_CLASS,	MAX_CI_CU_SS,		0,
		{CiCu_Init,		CiCu_SsOpenedProc,		CiCu_SsClosedProc,		CiCu_ApduProc}
	}
	#ifdef CONFIG_MW_CI_PLUS_OP_V_1 // 나중에 CI+ v1.3 이 default 가 되면 ifdef 제거하고 일반적으로 처리하도록 할 것
	,
	{	// Operator Profile v1
		{CI_OP_RES_V1_ID},			CI_HOST_RES,		0x0,		CI_OP_CLASS,	MAX_CI_OP_SS,		0,
		{CiOp_Init,		CiOp_SsOpenedProc,		CiOp_SsClosedProc,		CiOp_ApduProc}
	}
	#endif
	#ifdef CONFIG_MW_CI_PLUS_LSC_V_3
	,
	{	// Low Speed Communication v3
		{CI_LSC_IP_RES_V3_ID},		CI_HOST_RES,		0x0,		CI_LSC_CLASS,	MAX_CI_LSC_SS,	0,
		{CiLsc_Init,	CiLsc_SsOpenedProc,	CiLsc_SsClosedProc,	CiLsc_ApduProc}
	}
	#endif
#endif
#ifdef CONFIG_MW_CI_PLUS_SAS
	,
	{	// Specific Application Support for MHP v1
		{CI_SAS_RES_V1_ID},			CI_HOST_RES,		0x0,		CI_SAS_CLASS,	MAX_CI_SAS_SS,		0,
		{CiSas_Init,		CiSas_SsOpenedProc,		CiSas_SsClosedProc,		CiSas_ApduProc}
	}
#endif
#ifdef CI_PLUS_PVR_ENABLE
	,
	{	// CA PVR v1
		{CI_CAPVR_RES_V1_ID},		CI_HOST_RES,		0x0,		CI_CAPVR_CLASS,	MAX_CI_CAPVR_SS,	0,
		{CiCaPvr_Init,	CiCaPvr_SsOpenedProc,	CiCaPvr_SsClosedProc,	CiCaPvr_ApduProc}
	}
#endif
#ifdef CONFIG_OP_CANALREADY
	,
	{	// Authentication
		{CI_AUTH_RES_V1_ID},		CI_HOST_RES,		0x0,		CI_AUTH_CLASS,	MAX_CI_AUTH_SS, 	0,
		{CiAuth_Init,		CiAuth_SsOpenedProc,		CiAuth_SsClosedProc,		CiAuth_ApduProc}
	}
#endif
#ifdef _INCLUDE_BD_CI_ /* 사용하지 않음. 필요하면 재작업할 것. */
	,
	{	// Host Control Interface
		{CI_HCI_RES_V1_ID},			CI_HOST_RES,		0x0,		CI_HCI_CLASS,	MAX_CI_HCI_SS,		0,
		{CiHci_Init,		CiHci_SsOpenedProc,		CiHci_SsClosedProc,		CiHci_ApduProc}
	},
	{	// Authentication
		{CI_AUTH_RES_V1_ID},		CI_HOST_RES,		0x0,		CI_AUTH_CLASS,	MAX_CI_AUTH_SS,		0,
		{CiAuth_Init,		CiAuth_SsOpenedProc,		CiAuth_SsClosedProc,		CiAuth_ApduProc}
	},
	{	// Copy Protection
		{CI_CP_RES_V2_ID},			CI_HOST_RES,		0x0,		CI_CP_CLASS,	MAX_CI_CP_SS,		0,
		{CiCp_Init,			CiCp_SsOpenedProc,			CiCp_SsClosedProc,			CiCp_ApduProc}
	},
	{	// BD Private Pin Management
		{BD_CI_PRI_PIN_RES_V1_ID},	CI_HOST_RES,		0x0,		0,				MAX_BD_CI_PIN_SS,	0,
		{BD_CiPin_Init,		BD_CiPin_SsOpenedProc,		BD_CiPin_SsClosedProc,		BD_CiPin_ApduProc}
	}
#endif	//_INCLUDE_BD_CI_
};

static CI_TC_INST		s_aCiTc[MAXTCID];
static CI_SS_INST		s_aCiSs[MAXSSNB];
static CI_RES_INST		s_aCiRes[MAX_CI_RES_INST];
static unsigned char	s_ucCiResCnt;
static unsigned long	s_ulCiResSemId;

/***************************************************************
* static function prototypes
***************************************************************/
static void CiRh_ProcTcCreated(sr_creat tcCreated);
static void CiRh_ProcOpenSsReq(sr_open openSsReq);
static void CiRh_ProcSsOpened(sr_opened ssOpened);
static void CiRh_ProcApdu(sr_apdu ssApdu);
static void CiRh_ProcSsClosed(sr_closed ssClosed);
static void CiRh_ProcTcDeleted(sr_deleted tcDeleted);
static void CiRh_SendOpenSsRsp (unsigned short usSsNo, unsigned char ucState, unsigned char ucRTcId);
static CI_RES_HANDLE CiRh_GetNewRes(void);
static CI_TC_HANDLE CiRh_GetNewTc(void);
static CI_SS_HANDLE CiRh_GetNewSs(void);
static CI_TC_HANDLE CiRh_GetTc(unsigned char ucTcId);
static CI_SS_HANDLE CiRh_GetSs(unsigned short usSsNo);
static CI_STACK_SS_STATE CiRh_GetValidRes(sr_open openSsReq, CI_RES_HANDLE *phRes, unsigned long *ulSsResId);
static int CiRh_GetSlotIdByTcId(unsigned char ucTcId, CI_SLOT_ID *pusSlotId);
static void CiRh_LockRes(void);
static void CiRh_UnlockRes(void);

/***************************************************************
* function bodies
***************************************************************/

/***************************************************************
*
* global functions
*
***************************************************************/

/********************************************************************
 Function   :	CIRH_Init
 Description :	initialize CIRH
				Before call CIRH_Init(), you must initialize CI Resource supported by Host.
				CI Initialization include registering callback to CIRH.
				But when module is inserted, CI Resource supported by module is initialized.
 Input      :
 Output     :
 Return     : 	0 for OK, -1 for Failure
********************************************************************/
int CIRH_Init(void)
{
	int i;

	HxSTD_memset(s_aCiTc, 0, sizeof(s_aCiTc));
	HxSTD_memset(s_aCiSs, 0, sizeof(s_aCiSs));
	HxSTD_memset(s_aCiRes, 0, sizeof(s_aCiRes));
	HxSTD_memcpy(s_aCiRes, s_aCiHostRes, sizeof(s_aCiHostRes));

	s_ucCiResCnt = sizeof(s_aCiHostRes) / sizeof(CI_RES_INST);

	for (i = 0; i < s_ucCiResCnt; i++)
		(*s_aCiRes[i].cbList.pfnCiInit)();

	s_ulCiResSemId = (unsigned long)NULL;
	if (VK_SEM_Create(&s_ulCiResSemId, "ci_res", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
	{
		HxLOG_Error("[CIRH]---> CIRH_Init()---> Error : init failure !!!\n");
		return -1;
	}

	//HLIB_LOG_SetLevel(HxLOG_DOMAIN, HxLOG_ERROR | HxLOG_PRINT | HxLOG_Print);


	/* Initialize CI stack : init and start task related CI stack and CIRH_Task */
	ItcKernelInit();

	return ERR_OK;
}

/********************************************************************
 Function   :	CIRH_GetRes
 Description :	Get Resources from CIRH
 Input      :
 Output     :
 Return     : 	CI Resource Array
 ********************************************************************/
CI_RES_INST * CIRH_GetRes(void)
{
	return s_aCiRes;
}

/********************************************************************
 Function   :	CIRH_AddRes
 Description :	Add(register) Resource to CIRH
 Input      :	ulResId - resource ID
 				usSrcId - Source ID
 				ucMaxSs - Max Number of Sessions
 				pCbList - pointer of callback function list
 Output     :
 Return     : 	CI Resource Handle allocated to CIRH (CI_RES_HANDLE)
 				NULL for Failure.
 ********************************************************************/
CI_RES_HANDLE CIRH_AddRes(unsigned long ulResId, CI_SLOT_ID usSrcId, unsigned char ucMaxSs, CI_RES_CB_LIST *pCbList)
{
	CI_RES_HANDLE hRes;

	CiRh_LockRes();

	hRes = CiRh_GetNewRes();	// get a new resource instance allocated by CIRH

	if (hRes == NULL)
	{
		HxLOG_Error("[CIRH]---> CIRH_AddRes()---> Error : don't have space to allocate !!!\n");
		CiRh_UnlockRes();
		return NULL;
	}

	hRes->uResId.ulId = ulResId;
	hRes->usSrcId = usSrcId;
	hRes->ucResIdType = (ulResId >> 30) & 0x03;

	if (hRes->ucResIdType != CI_RES_PRI_TYPE)
		hRes->usResClass = (ulResId >> 16) & 0x3fff;

	hRes->ucMaxSs = ucMaxSs;
	hRes->ucSsCnt = 0;
	if (pCbList != NULL)
		HxSTD_memcpy(&hRes->cbList, pCbList, sizeof(CI_RES_CB_LIST));

	s_ucCiResCnt++;

	CiRh_UnlockRes();

	return hRes;
}

/********************************************************************
 Function   :	CIRH_DeleteRes
 Description :	Delete(register) Resource from CIRH
 Input      :	hRes - resource handle
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CIRH_DeleteRes(CI_RES_HANDLE hRes)
{
	int i;

	CiRh_LockRes();

	for (i = 0; i < MAX_CI_RES_INST; i++)
	{
		if (&s_aCiRes[i] == hRes)
		{
			HxSTD_memset(hRes, 0, sizeof(CI_RES_INST));
			s_ucCiResCnt--;
			CiRh_UnlockRes();
			return ERR_OK;
		}
	}

	CiRh_UnlockRes();

	return -1;
}

/********************************************************************
 Function   :	CIRH_GetSlotIdBySsNo
 Description :	get a CI slot ID mapped usSsNo
 Input      :	usSsNo - Session Number
 Output     :	pusSlotId - pointer of CI slot ID
 Return     :	0 for OK, -1 for failure
 ********************************************************************/
int CIRH_GetSlotIdBySsNo(unsigned short usSsNo, CI_SLOT_ID *pusSlotId)
{
	CI_SS_HANDLE hSs = NULL;
	int i;

	for (i = 0; i < MAXSSNB; i++)
	{
		if (s_aCiSs[i].usSsNo == usSsNo)
		{
			hSs = &s_aCiSs[i];
			break;
		}
	}

	if (hSs == NULL || i == MAXSSNB)
	{
		HxLOG_Error("[CIRH]---> CIRH_GetSlotIdBySsNo()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo);
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		HxLOG_Error("[CIRH]---> CIRH_GetSlotIdBySsNo()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo);
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		HxLOG_Error("[CIRH]---> CIRH_GetSlotIdBySsNo()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo);
		return -1;
	}
	if (hSs->hTc->usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("[CIRH]---> CIRH_GetSlotIdBySsNo()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo);
		return -1;
	}

	*pusSlotId = hSs->hTc->usSlotId;

	return ERR_OK;
}

/********************************************************************
 Function   :	CIRH_GetSs
 Description :	Get CI Session Handle
 Input      :	usSsNo - Session Number
 Output     :
 Return     : 	CI Session Handle allocated to CIRH (CI_SS_HANDLE)
 				NULL for failure
 ********************************************************************/
CI_SS_HANDLE CIRH_GetSs(unsigned short usSsNo)
{
	return CiRh_GetSs(usSsNo);
}

/********************************************************************
 Function   :	CIRH_GetSsFromRes
 Description :	Get CI Session Handle
 Input      :	usSlotId - Slot ID
 				ulResId - Resource ID
 Output     :
 Return     : 	CI Session Handle allocated to CIRH (CI_SS_HANDLE)
 				NULL for failure
 ********************************************************************/
CI_SS_HANDLE CIRH_GetSsFromRes(CI_SLOT_ID usSlotId, unsigned long ulResId)
	/* 한 슬롯에 복수개의 세션을 갖을 수 있는 리소스의 경우 사용에 있어 주의해야 한다. CiAi_AddAppInfo() 에서 사용된 예처럼 특정 리소스가 존재하는가의 판별 용도로만 사용해야 한다. */
{
	int i;

	for (i = 0; i < MAXSSNB; i++)
	{
		if ((s_aCiSs[i].hTc->usSlotId == usSlotId) && (s_aCiSs[i].hRes->uResId.ulId == ulResId))
		{
			return &s_aCiSs[i];
		}
	}

	return NULL;
}

/********************************************************************
 Function   :	CIRH_GetTcInfo
 Description :	Get CI TC Information
 Input      :	ucTcId - TC ID
 Output     :
 Return     : 	pointer of CI TC Information
 				NULL for failure
 ********************************************************************/
CI_TC_HANDLE CIRH_GetTc(unsigned char ucTcId)
{
	return CiRh_GetTc(ucTcId);
}

/********************************************************************
 Function   :	CIRH_GetTcId
 Description :	get a CI TC ID mapped usSlotId
 Input      :	usSlotId - CI Slot ID
 Output     :	pucTcId - pointer of CI TC ID
 Return     :	0 for OK, -1 for failure
 ********************************************************************/
int CIRH_GetTcId(CI_SLOT_ID usSlotId, unsigned char *pucTcId)
{
	int i;

	for (i = 0; i < MAXTCID; i++)
	{
		if ((t_tab_tcid[i].slot_id) == usSlotId)
		{
			*pucTcId = (unsigned char)t_tab_tcid[i].tcid;
			return ERR_OK;
		}
	}

	*pucTcId = 0;

	HxLOG_Error("[CIRH]---> CIRH_GetTcId(%d)---> Error : No Matched TC Id !!!\n", usSlotId);

	return -1;
}

/********************************************************************
 Function   :	CIRH_GetResCnt
 Description :	Get Number of Registered Resource in CIRH
 Input      :
 Output     :
 Return     : 	Number of Resource registered to CIRH
 ********************************************************************/
unsigned char CIRH_GetResCnt(void)
{
	return s_ucCiResCnt;
}

/********************************************************************
 Function   :	CIRH_ReqCloseSs
 Description :	Request CIRH to close session
 Input      :	usSsNo - session number
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CIRH_ReqCloseSs(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;
	xmsg msg;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0) // CiRh_GetSs() 만 해도 무방할 듯...
	{
		HxLOG_Error("[CIRH]---> CIRH_ReqCloseSs()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	msg.message.header.trans = (CIRH << 4) | SESS;
	msg.message.header.type = RM_CLOSE_SS;
	msg.message.header.len = sizeof(sr_close);

	msg.message.body.close.ssnb = usSsNo;

	if (!ITC_MsgSend(msg.message.header.trans, (unsigned char *)&msg))
	{
		HxLOG_Error("[CIRH]---> CIRH_ReqCloseSs()---> Error : ITC_MsgSend() for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CIRH_SendApdu
 Description :	Send APDU to CIRH
 Input      :	usSsNo - session number
 				pucApdu - pointer of APDU
 						must be freed by caller ??
 				ulApduLen - APDU length
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CIRH_SendApdu(unsigned short usSsNo, unsigned char *pucApdu, unsigned long ulApduLen)
{
	CI_SLOT_ID usSlotId;
	unsigned char *pucData;
	xmsg msg;

#ifdef CONFIG_DEBUG
	int i;
	HxLOG_Info("CIRH_SendApdu()---> Sending APDU data: [ ");
	for ( i=0; i<ulApduLen; i++)
	{
		HxLOG_Print("%02x ", pucApdu[i]);
	}
	HxLOG_Print("]\n");
#endif


	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0) // CiRh_GetSs() 만 해도 무방할 듯...
	{
		HxLOG_Error("[CIRH]---> CIRH_SendApdu()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	pucData = ITC_DynAlloc(ulApduLen);
	if (pucData == NULL)
	{
		HxLOG_Error("[CIRH]---> CIRH_SendApdu(%d)---> Error : not enough memory !!!\n", usSsNo);
		return -1;
	}

	HxSTD_memcpy(pucData, pucApdu, ulApduLen);
	msg.message.header.trans = (CIRH << 4) | SESS;
	msg.message.header.type = APDU;
	msg.message.header.len = sizeof(sr_apdu);

	msg.message.body.apdu.ssnb = usSsNo;
	msg.message.body.apdu.papdu = pucData;
	msg.message.body.apdu.len = ulApduLen;

	if (!ITC_MsgSend(msg.message.header.trans, (unsigned char *)&msg))
	{
		HxLOG_Error("[CIRH]---> CIRH_SendApdu(%d)---> Error : ITC_MsgSend() !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CIRH_GetApduTag
 Description :	Get APDU tag from APDU
 Input      :	pucApdu - pointer of APDU
 Output     :
 Return     : 	APDU tag (apdu_tag in CI spec.)
 ********************************************************************/
unsigned long CIRH_GetApduTag(unsigned char *pucApdu)
{
	unsigned long ulTag;

	ulTag = (unsigned long)pucApdu[2] & 0x000000ff;
	ulTag |= ((unsigned long)pucApdu[1] << 8) & 0x0000ff00;
	ulTag |= ((unsigned long)pucApdu[0] << 16) & 0x00ff0000;

	return ulTag;
}

/********************************************************************
 Function   :	CIRH_GetApduDataLength
 Description :	Get length of APDU data from APDU
 Input      :	pucApdu - pointer of APDU
 Output     :
 Return     : 	length of APDU Data
 ********************************************************************/
unsigned long CIRH_GetApduDataLength(unsigned char *pucApdu)
{
	/* <TODO_CI/TODO_CI_PLUS> : 우선 65535 바이트 이상의 데이터를 받을 수 있도록 unsigned short 를 unsigned long 으로 수정하여 length_field_size 3, 4 인 경우에 대해 대응할 수 있도록 함.
								원칙적으로 스펙에 맞게 구현하려면 보다 근본적으로 수정해야 하는데...
								실제로 CI+ MHEG에서도 4294967295 바이트 이상의 데이터를 처리하지는 않을 것 같으므로 일단 이 정도로만 수정하도록 하자. */

	unsigned char	*pucData;
	unsigned long	ulLen;
	unsigned char	ucCnt;

	pucData = pucApdu;
	pucData += 3;

	if (*pucData & 0x80)
	{
		ucCnt = (*pucData++ & 0x7f);
		if (ucCnt > 4)
		{
			HxLOG_Error("[CIRH]---> CIRH_GetApduDataLength()---> Error : length_field_size (%d) > 4 !!!\n", ucCnt);
			return 0;
		}
		ulLen = 0;
		while(ucCnt--)
			ulLen = ((ulLen << 8) | *pucData++);
	}
	else
	{
		ulLen = *pucData;
	}

	return ulLen;
}

/********************************************************************
 Function   :	CIRH_GetApduData
 Description :	Get APDU Data from APDU
 Input      :	pucApdu - pointer of APDU
 Output     :
 Return     : 	pointer of APDU data
 ********************************************************************/
unsigned char * CIRH_GetApduData(unsigned char *pucApdu)
{
	unsigned char	*pucData;
	unsigned char	ucCnt;

	pucData = pucApdu;
	pucData += 3;

	if (*pucData & 0x80)
	{
		ucCnt = (*pucData++ & 0x7f);
		while(ucCnt--)
			pucData++;
	}
	else
		pucData++;

	return pucData;
}

/********************************************************************
 Function   :	CIRH_ConvertApduLengthField
 Description :	Convert length value to length_field() in CI spec.
 Input      :	ulLen - length to be converted
 Output     :	pucLenField - pointer of length_field()
 				pucFieldSize - pointer of Field Size(byte count)
 Return     :
 ********************************************************************/
void CIRH_ConvertApduLengthField(unsigned long ulLen, unsigned char *pucLenField, unsigned char *pucFieldSize)
{
	/*
		length_field() {
			size_indicator						1	bslbf
			if (size_indicator == 0)
				length_value					7	uimsbf
			else if (size_indicator == 1) {
				length_field_size				7	uimsbf
				for (i=0; i<length_field_size; i++) {
					length_value_byte			8	bslbf
				}
			}
		}
	*/

	/* <TODO_CI/TODO_CI_PLUS> : 우선 65535 바이트 이상의 데이터를 보낼 수 있도록 unsigned short 를 unsigned long 으로 수정하고 length_field_size 3, 4 인 경우에 대해 추가함.
								원칙적으로 스펙에 맞게 구현하려면 보다 근본적으로 수정해야 하는데...
								실제로 CI+ MHEG에서도 4294967295 바이트 이상의 데이터를 처리하지는 않을 것 같으므로 일단 이 정도로만 수정하도록 하자. */

	if (ulLen <= 127)
	{
		*pucLenField = (unsigned char)ulLen;
		*pucFieldSize = 1;
	}
	else if (ulLen <= 255)
	{
		*pucLenField++ = 0x81;
		*pucLenField = (unsigned char)ulLen;
		*pucFieldSize = 2;
	}
	else if (ulLen <= 65535)
	{
		*pucLenField++ = 0x82;
		*pucLenField++ = (unsigned char)((ulLen >> 8) & 0x00ff);
		*pucLenField = (unsigned char)(ulLen & 0x00ff);
		*pucFieldSize = 3;
	}
	else if (ulLen <= 16777215)
	{
		*pucLenField++ = 0x83;
		*pucLenField++ = (unsigned char)((ulLen >> 16) & 0x00ff);
		*pucLenField++ = (unsigned char)((ulLen >> 8) & 0x00ff);
		*pucLenField = (unsigned char)(ulLen & 0x00ff);
		*pucFieldSize = 4;
	}
	else // if (ulLen <= 4294967295)
	{
		*pucLenField++ = 0x84;
		*pucLenField++ = (unsigned char)((ulLen >> 24) & 0x00ff);
		*pucLenField++ = (unsigned char)((ulLen >> 16) & 0x00ff);
		*pucLenField++ = (unsigned char)((ulLen >> 8) & 0x00ff);
		*pucLenField = (unsigned char)(ulLen & 0x00ff);
		*pucFieldSize = 5;
	}
}

/********************************************************************
 Function   :	CIRH_DeleteTc
 Description :	Delete TC
 Input      :	usSlotId - Slot Id
 Output     :
 Return     :
 ********************************************************************/
int CIRH_DeleteTc(CI_SLOT_ID usSlotId)
{
	unsigned char ucTcId;
	xmsg msg;

	if (CIRH_GetTcId(usSlotId, &ucTcId) != ERR_OK)
		return -1;

	msg.message.header.trans = (CIRH << 4) | SESS;
	msg.message.header.type = RM_DELETE_TC;
	msg.message.header.len = sizeof(sr_delete);
	msg.message.body.sdel.tcid = ucTcId;

	if (!ITC_MsgSend(msg.message.header.trans, (unsigned char *)&msg))
	{
		HxLOG_Error("[CIRH]---> CIRH_DeleteTc()---> Error : ITC_MsgSend() for slot %d !!!\n", usSlotId);
		return -1;
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CIRH_Task
 Description :	CIRH Task
 Input      :	pvParam - currently not used
 Output     :
 Return     :
 ********************************************************************/
void CIRH_Task(void *pvParam)
{
	unsigned char	ucEvent;
	unsigned char	ucTrans;
	unsigned char	ucType;
	xmsg			rcvMsg;

	/* Not used */
	pvParam = NULL;

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
	ITC_SetTimer(CI_DT_TIMER, CI_DT_TIMER_PERIOD, CI_TASK_ID_RH);
#else
	ITC_SetTimer(CI_DT_TIMER, CI_DT_TIMER_PERIOD);
#endif

	for (;;)
	{
#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
		if (!ITC_WaitEvt(ITC_INFINITE, &ucEvent, &ucTrans, &rcvMsg, CI_TASK_ID_RH))
#else
		if (!ITC_WaitEvt(ITC_INFINITE, &ucEvent, &ucTrans, &rcvMsg))
#endif
		{
			HxLOG_Error("[CIRH]---> CIRH_Task()---> Error : ITC_WaitEvt !!!\n");

			#if 0 // org
			return;
			#else
			continue;
			#endif
		}

		if (ucEvent == MESSAGE)
		{
			ucType = rcvMsg.message.header.type;

			switch (ucType)
			{
			case RM_TC_CREATED:
				CiRh_ProcTcCreated(rcvMsg.message.body.screat);
				break;
			case RM_OPEN_SS_REQ:
				CiRh_ProcOpenSsReq(rcvMsg.message.body.open);
				break;
			case RM_SS_OPENED:
				CiRh_ProcSsOpened(rcvMsg.message.body.opened);
				break;
			case APDU:
				CiRh_ProcApdu(rcvMsg.message.body.apdu);
				break;
			case RM_SS_CLOSED:
				CiRh_ProcSsClosed(rcvMsg.message.body.closed);
				break;
			case RM_TC_DELETED:
				CiRh_ProcTcDeleted(rcvMsg.message.body.sdeleted);
				break;
			default:
				HxLOG_Error("[CIRH]---> CIRH_Task()---> Error : Received unexpected message[0x%x] !!!\n", ucType);
				break;
			}
		}
		else if (ucEvent == TIMER)
		{
#if 1
			switch (rcvMsg.timer)
			{
			case CI_DT_TIMER:
				CiDt_TimerProc();
				break;
			default:
				HxLOG_Error("[CIRH]---> CIRH_Task()---> Error : Received unexpected timerId[0x%x] !!!\n", rcvMsg.timer);
				break;
			}
#endif
		}
		else
		{
			HxLOG_Error("[CIRH]---> CIRH_Task()---> Error : Received unexpected event[0x%x] !!!\n", ucEvent);
		}
	}
}


/***************************************************************
*
* static functions
*
***************************************************************/

/********************************************************************
 Function   :	CiRh_ProcTcCreated
 Description :	Procedure to handle event that TC is created
 Input      :	tcCreated
 Output     :
 Return     :
 ********************************************************************/
static void CiRh_ProcTcCreated(sr_creat tcCreated)
{
	CI_SLOT_ID usSlotId;
	CI_TC_HANDLE hTc;

	if (CiRh_GetSlotIdByTcId(tcCreated.tcid, &usSlotId) != ERR_OK)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcTcCreated()---> Error : incorrect TC ID[%d] !!!\n", tcCreated.tcid);
		return;
	}

	// add TC Information
	hTc = CiRh_GetNewTc();
	if (hTc == NULL)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcTcCreated()---> Error : Can't add TC !!!\n");
		return;
	}

	hTc->ucTcId = tcCreated.tcid;
	hTc->usSlotId = usSlotId;

	HxLOG_Print("[CIRH]---> CiRh_ProcTcCreated()---> success TC ID[%d], slot ID[%d]\n", tcCreated.tcid, usSlotId);
}

/********************************************************************
 Function   :	CiRh_ProcOpenSsReq
 Description :	Procedure to handle event that module request to open session
 Input      :	openSsReq - information about session requested by module
 Output     :
 Return     :
 ********************************************************************/
static void CiRh_ProcOpenSsReq(sr_open openSsReq)
{
	CI_TC_HANDLE		hTc;
	CI_RES_HANDLE		hRes;
	CI_SS_HANDLE		hSs;

	CI_STACK_SS_STATE	ucSsState;
	unsigned char		ucRTcId;
	unsigned long		ulOpenedSsResId = 0;

	// check TC ID of Session
	hTc = CiRh_GetTc(openSsReq.tcid);
	if (hTc == NULL)
	{
		CiRh_SendOpenSsRsp(openSsReq.ssnb, SS_NOTALLOCATED, 0);
		HxLOG_Error("[CIRH]---> CiRh_ProcOpenSsReq()---> Error : unknown TC ID[0x%x] !!!\n", openSsReq.tcid);
		return;
	}

	// get a valid resource from CI resources in CIRH
	ucSsState = CiRh_GetValidRes(openSsReq, &hRes, &ulOpenedSsResId);
	if (hRes == NULL)
	{
		CiRh_SendOpenSsRsp(openSsReq.ssnb, ucSsState, 0);
		HxLOG_Error("[CIRH]---> CiRh_ProcOpenSsReq()---> Error : fail to get valid Resource !!!\n");
		return;
	}

	if (hRes->usSrcId != CI_HOST_RES)	// in case of external resource
	{
		if (CIRH_GetTcId(hRes->usSrcId, &ucRTcId) != ERR_OK)
		{
			CiRh_SendOpenSsRsp(openSsReq.ssnb, SS_NOTALLOCATED, 0);
			HxLOG_Error("[CIRH]---> CiRh_ProcOpenSsReq()---> Error : fail to get a routing TC ID !!!\n");
			return;
		}

		CiRh_SendOpenSsRsp(openSsReq.ssnb, SS_OK, ucRTcId);
		HxLOG_Print(("[CIRH]---> CiRh_ProcOpenSsReq()---> route TC...\n"));
		return;
	}

	// add Session instance
	hSs = CiRh_GetNewSs();
	if (hSs == NULL)
	{
		CiRh_SendOpenSsRsp(openSsReq.ssnb, SS_NOTALLOCATED, 0);
		HxLOG_Error("[CIRH]---> CiRh_ProcOpenSsReq()---> Error : fail to get a new SS !!!\n");
		return;
	}

	hSs->hTc = hTc;
	hSs->hRes = hRes;
	hSs->usSsNo = openSsReq.ssnb;
	hSs->nState = CI_SS_REQESTED_OPEN;
	hSs->ulOpendSsResId = ulOpenedSsResId;

	CiRh_SendOpenSsRsp(openSsReq.ssnb, SS_OK, 0);

	HxLOG_Print("[CIRH]---> CiRh_ProcOpenSsReq()---> SsNo[0x%x] OK\n", openSsReq.ssnb);
}

/********************************************************************
 Function   :	CiRh_ProcSsOpened
 Description :	Procedure to handle event that the session is opened
 Input      :	ssOpened - information about the opened session
 Output     :
 Return     :
 ********************************************************************/
static void CiRh_ProcSsOpened(sr_opened ssOpened)
{
	CI_SS_HANDLE	hSs;
	CI_RES_CB_LIST	*pCbList;

	hSs = CiRh_GetSs(ssOpened.ssnb);
	if (hSs == NULL)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcSsOpened()---> Error : CiRh_GetSs() SsNo[%d] - NULL hSs !!!\n", ssOpened.ssnb);
		return;
	}
	if (hSs->hTc == NULL)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcSsOpened()---> Error : CiRh_GetSs() SsNo[%d] - NULL hTc !!!\n", ssOpened.ssnb);
		return;
	}
	if (hSs->hTc->ucTcId == 0 || hSs->hTc->ucTcId != ssOpened.tcid)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcSsOpened()---> Error : CiRh_GetSs() SsNo[%d] - Invalid TcId[%d] !!!\n", ssOpened.ssnb, ssOpened.tcid);
		return;
	}
	if (hSs->hRes == NULL)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcSsOpened()---> Error : CiRh_GetSs() SsNo[%d] - NULL hRes !!!\n", ssOpened.ssnb);
		return;
	}

	hSs->nState = CI_SS_OPENED;
	hSs->hRes->ucSsCnt++;

	pCbList = &hSs->hRes->cbList;

	if (pCbList->pfnCiSsOpened != NULL)
		pCbList->pfnCiSsOpened(ssOpened.ssnb);

	HxLOG_Print("[CIRH]---> CiRh_ProcSsOpened()---> TcId[%d], SsNb[%d]\n", ssOpened.tcid, ssOpened.ssnb);
}

/********************************************************************
 Function   :	CiRh_ProcApdu
 Description :	Procedure to handle event that the APDU is received
 Input      :	ssApdu - information about the APDU
 Output     :
 Return     :
 ********************************************************************/
static void CiRh_ProcApdu(sr_apdu ssApdu)
{
	CI_SS_HANDLE	hSs;
	CI_RES_CB_LIST	*pCbList;
	unsigned long	ulCiApduTag;
	unsigned long	ulCiApduLen;
	unsigned char	*pucCiApduData;

	hSs = CiRh_GetSs(ssApdu.ssnb);
	if (hSs == NULL)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcApdu()---> Error : SsNo[%d] - NULL hSs !!!\n", ssApdu.ssnb);
		return;
	}
	if (hSs->hRes == NULL)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcApdu()---> Error : SsNo[%d] - NULL hRes !!!\n", ssApdu.ssnb);
		return;
	}

	ulCiApduTag = CIRH_GetApduTag(ssApdu.papdu);
	ulCiApduLen = CIRH_GetApduDataLength(ssApdu.papdu);
	pucCiApduData = CIRH_GetApduData(ssApdu.papdu);

	pCbList = &hSs->hRes->cbList;

	if (pCbList->pfnCiApdu != NULL)
		pCbList->pfnCiApdu(ssApdu.ssnb, ulCiApduTag, pucCiApduData, ulCiApduLen);

	ITC_DynFree(ssApdu.papdu);
}

/********************************************************************
 Function   :	CiRh_ProcSsClosed
 Description :	Procedure to handle event that the session is closed
 Input      :	ssClosed - information about the closed session
 Output     :
 Return     :
 ********************************************************************/
static void CiRh_ProcSsClosed(sr_closed ssClosed)
{
	CI_SS_HANDLE	hSs;
	CI_RES_CB_LIST	*pCbList;

	hSs = CiRh_GetSs(ssClosed.ssnb);
	if (hSs == NULL)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcSsClosed()---> Error : SsNo[%d] - NULL hSs !!!\n", ssClosed.ssnb);
		return;
	}
	if (hSs->hRes == NULL)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcSsClosed()---> Error : SsNo[%d] - NULL hRes !!!\n", ssClosed.ssnb);
		return;
	}

	pCbList = &hSs->hRes->cbList;

	if (pCbList->pfnCiSsClosed != NULL)
	{
		#ifdef _CI_PLUS_DEBUG_
		HxLOG_Print("[CIRH] resource ssnb %d is going to be closed...\n", ssClosed.ssnb);
		#endif
		pCbList->pfnCiSsClosed(ssClosed.ssnb);
	}

	hSs->hRes->ucSsCnt--;
	HxSTD_memset(hSs, 0, sizeof(CI_SS_INST));

	HxLOG_Print("[CIRH]---> CiRh_ProcSsClosed()---> SsNo:[%d]\n", ssClosed.ssnb);
}

/********************************************************************
 Function   :	CiRh_ProcTcDeleted
 Description :	Procedure to handle event that the TC is deleted
 Input      :	tcDeleted - information about the deleted TC
 Output     :
 Return     :
 ********************************************************************/
static void CiRh_ProcTcDeleted(sr_deleted tcDeleted)
{
	CI_RES_CB_LIST	*pCbList;
	CI_TC_HANDLE	hTc;
	unsigned char	ucTcId;
	CI_SLOT_ID		usSlotId;
	CI_APP_INFO		*pCiAppInfo;
	int				i;

	for (i = 0; i < MAXSSNB; i++)
	{
		if (s_aCiSs[i].nState == CI_SS_NONE)
			continue;

		if (s_aCiSs[i].hTc->ucTcId == tcDeleted.tcid)
		{
			pCbList = &s_aCiSs[i].hRes->cbList;
			if (pCbList->pfnCiSsClosed != NULL)
			{
				s_aCiSs[i].hRes->cbList.pfnCiSsClosed(s_aCiSs[i].usSsNo);
			}

			s_aCiSs[i].hRes->ucSsCnt--;
			HxSTD_memset(&s_aCiSs[i], 0, sizeof(CI_SS_INST));
		}
	}

	// delete TC Instance
	hTc = CiRh_GetTc(tcDeleted.tcid);
	if (hTc == NULL)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcTcDeleted()---> Error : unknown TC ID[0x%x] !!!\n", tcDeleted.tcid);
		return;
	}

	ucTcId = hTc->ucTcId;
	usSlotId = hTc->usSlotId;

	pCiAppInfo = CiAi_GetAppInfo(usSlotId);
	if (pCiAppInfo != NULL)
		pCiAppInfo->ucIsInitialized = 0;

	HxSTD_memset(hTc, 0, sizeof(CI_TC_INST));

	HxLOG_Print("[CIRH]---> CiRh_ProcTcDeleted()---> TcId[%d]:SlotId[%d]...\n", ucTcId, usSlotId);

	if (CARM_CI_CheckModule(usSlotId) != eCAS_DEV_CI_EXTRACTED)
	{
		HxLOG_Error("[CIRH]---> CiRh_ProcTcDeleted()---> svc_cas_DevCiResetModule(%d)...\n", usSlotId);
		if (svc_cas_DevCiResetModule(usSlotId) != ERR_OK)
		{
			HxLOG_Error("[CIRH]---> CiRh_ProcTcDeleted()---> Error : fail to reset slot %d !!!\n", usSlotId);
		}
	}
}

/********************************************************************
 Function   :	CiRh_SendOpenSsRsp
 Description :	Send the module response to the request to open session
 Input      :	usSsNo - session number
 				ucState - response to the request to open session.
 						SS_OK/SS_NOTALLOCATED/SS_NOTAVAILABLE
 						/SS_BADVERSION/SS_BUSY
 				ucRTcId - routing TC ID for using external resource
 Output     :
 Return     :
 ********************************************************************/
static void CiRh_SendOpenSsRsp (unsigned short usSsNo, unsigned char ucState, unsigned char ucRTcId)
{
	xmsg msg;

	msg.message.header.trans = (CIRH << 4) | SESS;
	msg.message.header.type = RM_OPEN_SS_RSP;
	msg.message.header.len = sizeof(sr_openr);
	msg.message.body.openrsp.ssnb = usSsNo;
	msg.message.body.openrsp.status = ucState;
	msg.message.body.openrsp.rtcid = ucRTcId;

	if (!ITC_MsgSend(msg.message.header.trans, (unsigned char *)&msg))
	{
		HxLOG_Error("[CIRH]---> CiRh_SendOpenSsRsp()---> Error : ITC_MsgSend() for SsNo[%d] !!!\n", usSsNo);
		return;
	}
}

/********************************************************************
 Function   :	CiRh_GetNewRes
 Description :	get new CI resource
 Input      :
 Output     :
 Return     :	new CI resource handle allocated by CIRH
 ********************************************************************/
static CI_RES_HANDLE CiRh_GetNewRes(void)
{
	int i;

	for (i = 0; i < MAX_CI_RES_INST; i++)
	{
		if (s_aCiRes[i].usResClass == 0)
			return &s_aCiRes[i];
	}

	return NULL;
}

/********************************************************************
 Function   :	CiRh_GetNewTc
 Description :	get new TC Instance
 Input      :
 Output     :
 Return     :	new CI TC allocated by CIRH
 ********************************************************************/
static CI_TC_HANDLE CiRh_GetNewTc(void)
{
	int i;

	for (i = 0; i < MAXTCID; i++)
	{
		if (s_aCiTc[i].ucTcId == 0)
			return &s_aCiTc[i];
	}

	return NULL;
}

/********************************************************************
 Function   :	CiRh_GetNewSs
 Description :	get new CI session
 Input      :
 Output     :
 Return     :	new CI session handle allocated by CIRH
 ********************************************************************/
static CI_SS_HANDLE CiRh_GetNewSs(void)
{
	int i;

	for (i = 0; i < MAXSSNB; i++)
	{
		if (s_aCiSs[i].usSsNo == 0)
			return &s_aCiSs[i];
	}

	return NULL;
}

/********************************************************************
 Function   :	CiRh_GetTc
 Description :	get TC handle including ucTcId
 Input      :	ucTcId - TC ID
 Output     :
 Return     :	CI TC handle including ucTcId
 ********************************************************************/
static CI_TC_HANDLE CiRh_GetTc(unsigned char ucTcId)
{
	int i;

	for (i = 0; i < MAXTCID; i++)
	{
		if (s_aCiTc[i].ucTcId == ucTcId)
			return &s_aCiTc[i];
	}

	return NULL;
}

/********************************************************************
 Function   :	CiRh_GetSs
 Description :	Get CI Session Handle
 Input      :	usSsNo - Session Number
 Output     :
 Return     : 	CI Session Handle allocated to CIRH (CI_SS_HANDLE)
 				NULL for failure
 ********************************************************************/
static CI_SS_HANDLE CiRh_GetSs(unsigned short usSsNo)
{
	int i;

	for (i = 0; i < MAXSSNB; i++)
	{
		if (s_aCiSs[i].usSsNo == usSsNo)
			return &s_aCiSs[i];
	}

	return NULL;
}

/********************************************************************
 Function   :	CiRh_GetValidRes
 Description :	get a valid resource to support openSsReq
 Input      :	openSsReq - information about the request to open session
 Output     :	phRes - pointer of the handle of valid resource
 Return     :	CI_STACK_SS_STATE
						 SS_OK/SS_NOTALLOCATED/SS_NOTAVAILABLE
 						/SS_BADVERSION/SS_BUSY
 ********************************************************************/
static CI_STACK_SS_STATE CiRh_GetValidRes(sr_open openSsReq, CI_RES_HANDLE *phRes, unsigned long *ulSsResId)
{
	CI_RES_ID			uResId;
	CI_RES_HANDLE		hRes = NULL;
	CI_STACK_SS_STATE	ucSsState = SS_NOTALLOCATED; // = SS_OK;
	int i;

	uResId.ulId = ((unsigned long)openSsReq.ress_id[0] & 0xff) << 24;
	uResId.ulId |= ((unsigned long)openSsReq.ress_id[1] & 0xff) << 16;
	uResId.ulId |= ((unsigned long)openSsReq.ress_id[2] & 0xff) << 8;
	uResId.ulId |= ((unsigned long)openSsReq.ress_id[3] & 0xff);

#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
	switch(uResId.ulId)
	{
		case CI_RM_RES_V1_ID	: HxLOG_Print("[CIRH] resource Resource Manager v1 requested with ssnb %d\n", openSsReq.ssnb); break;
		case CI_RM_RES_V2_ID	: HxLOG_Print("[CIRH] resource Resource Manager v2 requested with ssnb %d\n", openSsReq.ssnb); break;
		case CI_AI_RES_V1_ID	: HxLOG_Print("[CIRH] resource Application Information v1 requested with ssnb %d\n", openSsReq.ssnb); break;
		case CI_AI_RES_V2_ID	: HxLOG_Print("[CIRH] resource Application Information v2 requested with ssnb %d\n", openSsReq.ssnb); break;
		case CI_AI_RES_V3_ID	: HxLOG_Print("[CIRH] resource Application Information v3 requested with ssnb %d\n", openSsReq.ssnb); break;
		case CI_CAS_RES_V1_ID	: HxLOG_Print("[CIRH] resource Conditional Access Support requested with ssnb %d\n", openSsReq.ssnb); break;
		case CI_HCI_RES_V1_ID	: HxLOG_Print("[CIRH] resource Host Control Interface v1 requested with ssnb %d\n", openSsReq.ssnb); break;
#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
		case CI_HCI_RES_V2_ID	: HxLOG_Print("[CIRH] resource Host Control Interface v2 requested with ssnb %d\n", openSsReq.ssnb); break;
#endif
		case CI_DT_RES_V1_ID	: HxLOG_Print("[CIRH] resource Date-Time requested with ssnb %d\n", openSsReq.ssnb); break;
		case CI_MMI_RES_V1_ID	: HxLOG_Print("[CIRH] resource MMI requested with ssnb %d\n", openSsReq.ssnb); break;
		case CI_AMMI_RES_V1_ID	: HxLOG_Print("[CIRH] resource CI+ Browser Application MMI v1 requested with ssnb %d\n", openSsReq.ssnb); break;
#if defined(CONFIG_MW_CI_PLUS_AMMI_V_2)
		case CI_AMMI_RES_V2_ID	: HxLOG_Print("[CIRH] resource CI+ Browser Application MMI v2 requested with ssnb %d\n", openSsReq.ssnb); break;
#endif
		case CI_CC_RES_V1_ID	: HxLOG_Print("[CIRH] resource Content Control v1 requested with ssnb %d\n", openSsReq.ssnb); break;
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
		case CI_CC_RES_V2_ID	: HxLOG_Print("[CIRH] resource Content Control v2 requested with ssnb %d\n", openSsReq.ssnb); break;
#endif
		case CI_HLC_RES_V1_ID	: HxLOG_Print("[CIRH] resource Host Language & Country requested with ssnb %d\n", openSsReq.ssnb); break;
		case CI_CU_RES_V1_ID	: HxLOG_Print("[CIRH] resource CAM Firmware Upgrade requested with ssnb %d\n", openSsReq.ssnb); break;
#if defined(CONFIG_MW_CI_PLUS_OP_V_1)
		case CI_OP_RES_V1_ID	: HxLOG_Print("[CIRH] resource Operator Profile requested with ssnb %d\n", openSsReq.ssnb); break;
#endif
#if defined(CONFIG_MW_CI_PLUS_SAS)
		case CI_SAS_RES_V1_ID	: HxLOG_Print("[CIRH] resource Specific Application Support requested with ssnb %d\n", openSsReq.ssnb); break;
#else
		case CI_SAS_RES_V1_ID	: HxLOG_Print("[CIRH] resource Specific Application Support requested with ssnb %d -> but not supported\n", openSsReq.ssnb); break;
#endif
#if defined(CI_PLUS_PVR_ENABLE)
		case CI_CAPVR_RES_V1_ID	: HxLOG_Print("[CIRH] resource CA PVR requested with ssnb %d\n", openSsReq.ssnb); break;
#else
		case CI_CAPVR_RES_V1_ID	: HxLOG_Print("[CIRH] resource CA PVR requested with ssnb %d -> but not supported\n", openSsReq.ssnb); break;
#endif
#if defined(CONFIG_OP_CANALREADY)
		case CI_AUTH_RES_V1_ID	: HxLOG_Print("[CIRH] resource Authentication requested with ssnb %d\n", openSsReq.ssnb); break;
#else
		case CI_AUTH_RES_V1_ID	: HxLOG_Print("[CIRH] resource Authentication requested with ssnb %d -> but not supported\n", openSsReq.ssnb); break;
#endif
		case CI_CP_RES_V2_ID	: HxLOG_Print("[CIRH] resource Copy Protection requested with ssnb %d -> but not supported\n", openSsReq.ssnb); break;
#if defined( CONFIG_MW_CI_PLUS_LSC_V_3)
		case CI_LSC_IP_RES_V3_ID: HxLOG_Print("[CIRH] resource Low Speed Communication Ver3 with IP requested with ssnb %d\n", openSsReq.ssnb); break;
#else
		case CI_LSC_IP_RES_V3_ID: HxLOG_Print("[CIRH] resource Low Speed Communication Ver3 with IP requested with ssnb %d -> but not supported\n", openSsReq.ssnb); break;
#endif
		default					: HxLOG_Error("[CIRH] unknown resource 0x%08lx requested with ssnb %d !!!\n", uResId.ulId, openSsReq.ssnb); break;
	}
	/*
		SmarDTV CI+ old prototype 캠의 경우 : RMv1 -> AMMI -> AIv3 -> HLC -> CC -> DT -> MMI -> HCI -> CAS ... (CC -> CAS 순에 유의)
		SmarDTV CI+ new prototype 캠의 경우 (SmarCAM-3) : RMv1 -> HLC -> DT -> AIv3 -> CC -> HCI -> CAS -> MMI ... (CC -> CAS 순이나 리소스간 병렬 처리함)
		Neotion CI+ prototype 캠의 경우 : RMv1 -> AIv3 -> CAS -> DT -> CC -> MMI -> HLC ... (CAS -> CC 순이나 리소스간 병렬 처리함)
		SMiT    CI+ prototype 캠의 경우 : RMv1 -> AIv3 -> DT -> HLC -> CC -> MMI -> CAS ... (CC -> CAS 순에 유의)
	*/
#endif

//	hRes = NULL;

	// check whether uResId is private resource or not
	if (((uResId.ulId >> 30) & 0x03) == CI_RES_PRI_TYPE) // in case of Private Resource ID
	{
		for (i = 0; i < MAX_CI_RES_INST; i++)
		{
			if (s_aCiRes[i].uResId.ulId == uResId.ulId)
			{
				hRes = &s_aCiRes[i];
				if (hRes->ucSsCnt < hRes->ucMaxSs)
				{
					ucSsState = SS_OK;
					break;
				}
			}
		}

//		if (i >= MAX_CI_RES_INST)
//		{
//			ucSsState = SS_NOTALLOCATED;
//		}
	}
	else	// in case of Public Resource ID
	{
		// check whether CIRH support uResId's class & type or not
		for (i = 0; i < MAX_CI_RES_INST; i++)
		{
			if (((uResId.ulId >> 6) & 0x03ffffff) == ((s_aCiRes[i].uResId.ulId >> 6) & 0x03ffffff))
			{
				hRes = &s_aCiRes[i];
				*ulSsResId = uResId.ulId;
				#if 0
				// check whether CIRH support uResId's version or not
				if (hRes->uResId.ulPubIdV1.bitVer >= uResId.ulPubIdV1.bitVer)
				{
					if (hRes->ucSsCnt < hRes->ucMaxSs)
					{
						ucSsState = SS_OK;
						break;
					}
				}
				#else
				if (hRes->ucSsCnt < hRes->ucMaxSs)
				{
					ucSsState = SS_OK;
					break;
				}
				#endif
			}
		}

//		if (i >= MAX_CI_RES_INST)
//		{
//			ucSsState = SS_NOTALLOCATED;
//		}
	}

	*phRes = hRes;

	return ucSsState;
}

/********************************************************************
 Function   :	CiRh_GetSlotIdByTcId
 Description :	get a CI slot ID mapped ucTcId
 Input      :	ucTcId - TC ID
 Output     :	pusSlotId - pointer of CI slot ID
 Return     :	0 for OK, -1 for failure
 ********************************************************************/
static int CiRh_GetSlotIdByTcId(unsigned char ucTcId, CI_SLOT_ID *pusSlotId)
{
	int i;

	for (i = 0; i < MAXTCID; i++)
	{
		if (t_tab_tcid[i].tcid == ucTcId)
		{
			*pusSlotId = t_tab_tcid[i].slot_id;
			return ERR_OK;
		}
	}

	HxLOG_Error("[CIRH]---> CiRh_GetSlotIdByTcId()---> Error : No Matched Slot Id !!!\n");

	return -1;
}

static void	CiRh_LockRes(void)
{
	if (s_ulCiResSemId != (unsigned long)NULL)
		VK_SEM_Get(s_ulCiResSemId);
}

static void	CiRh_UnlockRes(void)
{
	if (s_ulCiResSemId != (unsigned long)NULL)
		VK_SEM_Release(s_ulCiResSemId);
}

const char * CiRh_GetResIdString(unsigned long ulId)
{
	switch(ulId)
	{
		case CI_RM_RES_V1_ID		: return (char *)"Resource Manager v1";
		case CI_RM_RES_V2_ID		: return (char *)"Resource Manager v2";
		case CI_AI_RES_V1_ID		: return (char *)"Application Information v1";
		case CI_AI_RES_V2_ID		: return (char *)"Application Information v2";
		case CI_AI_RES_V3_ID		: return (char *)"Application Information v3";
		case CI_CAS_RES_V1_ID		: return (char *)"Conditional Access Support";
		case CI_HCI_RES_V1_ID		: return (char *)"Host Control Interface";
		case CI_DT_RES_V1_ID		: return (char *)"Date-Time";
		case CI_MMI_RES_V1_ID		: return (char *)"MMI";
		case CI_AMMI_RES_V1_ID		: return (char *)"CI+ Browser Application MMI";
		case CI_CC_RES_V1_ID		: return (char *)"Content Control";
		case CI_HLC_RES_V1_ID		: return (char *)"Host Language & Country";
		case CI_CU_RES_V1_ID		: return (char *)"CAM Firmware Upgrade";
		case CI_SAS_RES_V1_ID		: return (char *)"Specific Application Support";
		case CI_CAPVR_RES_V1_ID	: return (char *)"CA PVR";
		case CI_AUTH_RES_V1_ID		: return (char *)"Authentication";
		case CI_CP_RES_V2_ID		: return (char *)"Copy Protection";
		default						: return (char *)"unknown resource";
	}
}

void CIRH_PrintRes(void)
{
	int				i;
	const char		*pResIdStr;

	for(i = 0; i < MAXSSNB; i++)
	{
		if(s_aCiSs[i].nState == CI_SS_OPENED)
		{
			pResIdStr = CiRh_GetResIdString(s_aCiSs[i].hRes->uResId.ulId);
			HxLOG_Print("[CIRH] %s, %d\n", pResIdStr, s_aCiSs[i].hRes->usSrcId);
		}
	}

	return;
}

/* end of file */
