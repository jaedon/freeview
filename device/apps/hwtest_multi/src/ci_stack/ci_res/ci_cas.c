/* $Header:   //Mars/source/pvcs/VM/BASE/archives/PDR_LIGHT/mw/cas/ci/ci_res/ci_cas.c-arc   1.4   Aug 21 2004 08:31:50   wskim  $ */
/***************************************************************
* $Workfile:   ci_cas.c  $
* $Modtime:   Aug 21 2004 07:31:20  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include "cb_list.h"

#include "di_ci.h"
#include "ci.h"
#include "cipack_msg.h"
#include "ci_reshandler.h"
#include "ci_ai.h"
#include "ci_cas.h"
#include "ci_hwtest.h"
#include "pal_ci.h"
/***************************************************************
* definition
***************************************************************/



/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE	g_hCbNotifyCamState;
extern CALLBACK_HANDLE	g_hCbNotifyCaSysId;


/***************************************************************
* static variables and structures
***************************************************************/
static CI_CAS_INST		s_aCiCas[PAL_MAX_CISLOT_NUMBER];
static unsigned char	s_ucCiCasCnt;
static unsigned short	s_usCiCasSsNo[PAL_MAX_CISLOT_NUMBER];


/***************************************************************
* static function prototypes
***************************************************************/
static int CiCas_AddCaInst(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCas_DeleteCaInst(unsigned short usSsNo);


/***************************************************************
* function bodies
***************************************************************/
int CiCas_Init(void)
{
	VK_memset(s_aCiCas, 0, sizeof(CI_CAS_INST) * PAL_MAX_CISLOT_NUMBER);
	VK_memset(s_usCiCasSsNo, 0, sizeof(s_usCiCasSsNo));

	s_ucCiCasCnt = 0;

	return ERR_OK;
}

int CiCas_SsOpenedProc(unsigned short usSsNo)
{
	CI_SS_HANDLE hSs;
	unsigned char ucMsg[4];

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CICA]---> CiCas_SsOpenedProc()---> Error : CIRH_GetSs() SsNo[%d] - NULL hSs\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CICA]---> CiCas_SsOpenedProc()---> Error : CIRH_GetSs() SsNo[%d] - NULL hTc\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CICA]---> CiCas_SsOpenedProc()---> Error : CIRH_GetSs() SsNo[%d] - Invalid TcId\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CICA]---> CiCas_SsOpenedProc()---> Error : CIRH_GetSs() SsNo[%d] - Invalid SlotId\n", usSsNo));
		return -1;
	}

	s_usCiCasSsNo[hSs->hTc->usSlotId] = usSsNo;

	ucMsg[0] = (unsigned char)((CI_CA_INFO_ENQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CA_INFO_ENQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CA_INFO_ENQ & 0xff);
	ucMsg[3] = 0x00;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		PrintErr(("[CICA]---> CiCas_SsOpenedProc()---> Error : send APDU msg. SsNo[%d]\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CICA]---> CiCas_SsOpenedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiCas_SsClosedProc(unsigned short usSsNo)
{
	CI_SS_HANDLE hSs;

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CICA]---> CiCas_SsClosedProc()---> Error : CIRH_GetSs() SsNo[%d] - NULL hSs\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CICA]---> CiCas_SsClosedProc()---> Error : CIRH_GetSs() SsNo[%d] - NULL hTc\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CICA]---> CiCas_SsClosedProc()---> Error : CIRH_GetSs() SsNo[%d] - Invalid TcId\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CICA]---> CiCas_SsClosedProc()---> Error : CIRH_GetSs() SsNo[%d] - Invalid SlotId\n", usSsNo));
		return -1;
	}

	s_usCiCasSsNo[hSs->hTc->usSlotId] = 0;

	if (CiCas_DeleteCaInst(usSsNo) < 0)
	{
		PrintErr(("[CICA]---> CiCas_SsClosedProc()---> Error : delete Ca info.SsNo[%d]\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CICA]---> CiCas_SsClosedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiCas_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_CA_INFO_REPLY:
		#if 0 /* #ifdef CI_WORKAROUND_FOR_DOUGLAS */ /* CI_APPL_INFO_REPLY 시점으로 당김. CI+ 캠의 경우 이것이 한참 후에 올라오기 때문임. 주의할 것! */
		{
			extern void CiAdapt_DouglasWorkAroundTimerCallbackCancel(void);
			CiAdapt_DouglasWorkAroundTimerCallbackCancel();
		}
		#endif
		nRtnCode = CiCas_AddCaInst(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CA_PMT_REPLY:
		PrintErr(("[CICA]---> CiCas_ApduProc()---> received CI_CAS_PMT_REPLY (len=%ld) !!!\n", ulApduLen));
		#if 0
		{
			int i;
			for (i = 0; i < ulApduLen; i++)
			{
				VK_Print(" %02x", pucApduData[i]);
			}
			VK_Print("\n");
		}
		#endif
		break;

	default:
		nRtnCode = -1;
		PrintErr(("[CICA]---> CiCas_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

	PrintDbg(("[CICA]---> CiCas_ApduProc()---> SsNo[%d], tag[0x%lx]...\n", usSsNo, ulApduTag));

	return nRtnCode;
}

int CiCas_SendCaPmt(unsigned short usSsNo, unsigned char *pucCaPmt, unsigned short usCaPmtLen)
{

	if(pucCaPmt == NULL || usCaPmtLen == 0)
	{
		PrintErr(("[CICA]---> CiCas_SendCaPmt()---> Error : No PMT data. SsNo[%d]\n", usSsNo));
		return -1;
	}

	if (CIRH_SendApdu(usSsNo, pucCaPmt, usCaPmtLen) != ERR_OK)
	{
		PrintErr(("[CICA]---> CiCas_SendCaPmt()---> Error : send APDU msg.SsNo[%d]\n", usSsNo));
		return -1;
	}

	/******************************************
	 notify cas state
	 ******************************************/

	return ERR_OK;
}

unsigned short CiCas_GetSsNo(CI_SLOT_ID usSlotId)
{
	if (usSlotId >= PAL_CI_GetNumberOfModule())
		return 0;

	return s_usCiCasSsNo[usSlotId];
}

CI_CAS_INST *CiCas_GetCaSysID(CI_SLOT_ID usSlotId) //for HW_TEST only
{
	if (usSlotId >= PAL_CI_GetNumberOfModule())
		return 0;

	return &s_aCiCas[usSlotId];
}
static int CiCas_AddCaInst(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_SS_HANDLE hSs;
	unsigned short *pusCaSysId = NULL;
	unsigned char ucCaSysIdCnt;
	CI_NOTIFY_CA_SYS_ID_FUNC	pfnNotifyCaSysId;
//	CI_NOTIFY_CAM_STATE_FUNC	pfnNotifyCamState;
//	CI_APP_INFO *	pCiAppInfo;
	int i;

#if 0
	if (pucApduData == NULL || ulApduLen == 0)
	{
		PrintErr(("[CICA]---> CiCas_AddCaInst()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}
#endif

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CICA]---> CiCas_AddCaInst()---> Error : CIRH_GetSs(%d) - NULL hSs\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CICA]---> CiCas_AddCaInst()---> Error : CIRH_GetSs(%d) - NULL hTc\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CICA]---> CiCas_AddCaInst()---> Error : CIRH_GetSs(%d) - Invalid TcId\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CICA]---> CiCas_AddCaInst()---> Error : CIRH_GetSs(%d) - Invalid SlotId\n", usSsNo));
		return -1;
	}

	pusCaSysId = (unsigned short *)CA_MEM_Alloc(ulApduLen);
	if(pusCaSysId == NULL)
	{
		PrintErr(("[CICA]---> CiCas_AddCaInst()---> Error : Can't alloc memory(%d)\n", usSsNo));
		return -1;
	}

	for (i=0; i<ulApduLen; i+=2)
	{
		pusCaSysId[i/2] = ((unsigned short)pucApduData[i] << 8) & 0xff00;
		pusCaSysId[i/2] |= ((unsigned short)pucApduData[i+1] & 0x00ff);
	}

	ucCaSysIdCnt = (unsigned char)(ulApduLen / sizeof(unsigned short));
	s_aCiCas[hSs->hTc->usSlotId].pusCaSysId = pusCaSysId;
	s_aCiCas[hSs->hTc->usSlotId].ucCaSysIdCnt = ucCaSysIdCnt;
	s_ucCiCasCnt++;

	/******************************************
	 for debugging
	 ******************************************/
#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
{
	VK_Print("\n --- # of CA_system_id = %d ---\n\n", ucCaSysIdCnt);
	for (i = 0; i < ucCaSysIdCnt; i++)
	{
		VK_Print(" %02X", pusCaSysId[i]);
	}
	VK_Print("\n\n");
}
#endif

	/******************************************
	 notify ca_system_id
	 ******************************************/
	/* get notify function list and notify CAM state */
	Callback_Reset(g_hCbNotifyCaSysId);
    while ((pfnNotifyCaSysId = (CI_NOTIFY_CA_SYS_ID_FUNC)Callback_GetNextFunc(g_hCbNotifyCaSysId)) != NULL)
    {
    	(*pfnNotifyCaSysId)(hSs->hTc->usSlotId, pusCaSysId, ucCaSysIdCnt);
    }

#if 0
    /******************************************
	 notify CAM state
	 ******************************************/
	/* get notify function list and notify CAM state */
	Callback_Reset(g_hCbNotifyCamState);
    while ((pfnNotifyCamState = (CI_NOTIFY_CAM_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCamState)) != NULL)
    {
    	(*pfnNotifyCamState)(hSs->hTc->usSlotId, CI_CAM_INITIALIZED);
    }
#endif

	CI_HWTEST_AddCaSysID();
	return ERR_OK;
}

static int CiCas_DeleteCaInst(unsigned short usSsNo)
{
	CI_SS_HANDLE hSs;
	CI_NOTIFY_CA_SYS_ID_FUNC	pfnNotifyCaSysId;

	CI_HWTEST_DeleteCaSysID();
	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		return -1;
	}

	s_ucCiCasCnt--;

	/******************************************
	 notify ca_system_id
	 ******************************************/
	/* get notify function list and notify CAM state */
	Callback_Reset(g_hCbNotifyCaSysId);
    while ((pfnNotifyCaSysId = (CI_NOTIFY_CA_SYS_ID_FUNC)Callback_GetNextFunc(g_hCbNotifyCaSysId)) != NULL)
    {
    	(*pfnNotifyCaSysId)(hSs->hTc->usSlotId, (unsigned short *)NULL, 0);
    }

	if(s_aCiCas[hSs->hTc->usSlotId].pusCaSysId != NULL)
	{
		CA_MEM_Free(s_aCiCas[hSs->hTc->usSlotId].pusCaSysId);
	}
	VK_memset(&s_aCiCas[hSs->hTc->usSlotId], 0, sizeof(CI_CAS_INST));

	return ERR_OK;
}

