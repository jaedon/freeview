/*******************************************************************
	File Description
*******************************************************************/
/*++
	@file   	 cx_sc.c
	@brief  	 Smart card Driver Adaptation Layer for Conax

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/
/*******************************************************************/
#define ________CX_SC_Private_Include_________________________________________
/*******************************************************************/
#include <octo_common.h>	// htype.h and hlib.h
#include <conax.h>
#include <_svc_cas_sub_dev_api.h>
#include "../local_include/cx_sc.h"
#include <cxmgr_action.h>


/*******************************************************************/
#define ________CX_SC_Golbal_Value____________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_SC_Private_Define__________________________________________
/*******************************************************************/
/* conax PT : SC-8.0/3.1.3F - Labwise on 20140812 */
/* PPS 응답이 없는 경우, 바로 deactivate 하고 reset 한다. 그리고, PPS를 다시 request 하지않고 진행한다. */
#define SUPPORT_RESET_AFTER_NO_RESPONSE_PPS

/* conax PT : SC-8.0/3.1.5G - Labwise on 20140812 */
/* PPS가 없는 경우에는 Fd(=372),Dd(=1) 값을 사용해야 한다. */
#define SUPPORT_CASE_PPS_IS_ABSENT
#define CX_SC_PPS_EXCHANGE

typedef struct
{
	SciHandle_t		hSci;
	SvcCas_SciCb_t	pfnCallBack;
	HUINT8			ucSlotId;
	HUINT8			ucIsCardMarrying;
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
	HUINT8			ucIsNoResponsePps;	//0; received PPS, 1;No response PPS
#else
	HUINT8			dummy1;	// for 4 byte align
#endif
	HUINT8			dummy2;
} CxScContext_t;

#if defined(CX_SC_PPS_EXCHANGE)
typedef struct
{
	HUINT32	ulProtocol;
	HUINT32	ulMinClock;
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
	HBOOL	bSpecificMode;
	HBOOL	bImplicitlyDefined;
	HBOOL	bPpsUnable;
} SCI_ATR_INFO;
#endif

/*******************************************************************/
#define ________CX_SC_Private_Static_Value____________________________________
/*******************************************************************/
static CxScContext_t			s_stCxScContext[CX_SC_SLOT_NUM];
static SvcCas_DevSciScParam_t	s_stDefaultAtrParam	=
{
	eCAS_SCI_T0,	// protocol
	0,				// Min Clock
	0,				// baudrate
	1,				// FI
	1,				// DI
	0,				// N
	10, 			// CWI
	4				// BWI
};

static Cx_SciEventNotifyCB_t	s_cbFunc			= NULL;

#if defined(CX_SC_PPS_EXCHANGE)
static HUINT8					s_aucFiOrder[16]	=
{
	0xFF, 1, 3, 4, 7, 8, 10, 0xFF, 0xFF, 2, 5, 6, 9, 11, 0xFF, 0xFF
};

static HUINT8					s_aucDiOrder[16]	=
{
	0xFF, 1, 2, 3, 4, 6, 8, 0xFF, 5, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static HUINT8					s_aucFiValue[16]	=
{
	0xFF, 1, 9, 2, 3, 10, 11, 4, 5, 12, 6, 13, 0xFF, 0xFF, 0xFF, 0xFF
};

static HUINT8					s_aucDiValue[16]	=
{
	0xFF, 1, 2, 3, 4, 8, 5, 9, 6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
#endif

/*******************************************************************/
#define ________CX_SC_Global_Value____________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_SC_Private_Static_Prototype________________________________
/*******************************************************************/
static void local_cxDalSc_Init(Cx_SciEventNotifyCB_t cbFunc);

static void local_cxDalSc_InitContext(void);
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
static void local_cxDalSc_SetIsNoReponsePpsWithSCIHandle(SciHandle_t hSci, HUINT8 ucIsNoResponsePps);
static HBOOL local_cxDalSc_IsNoReponsePpsWithSCIHandle(SciHandle_t hSci);
#endif
static SciHandle_t local_cxDalSc_GetSciHandleBySlotId(HUINT8 ucSlotId);
static void local_cxDalSc_cbCard(SciHandle_t hSci, HUINT8 ucSlotId, SvcCas_DevSciEvent_e eStatus);
static HERROR local_cxDalSc_Reset(HINT32 nScSlot, HUINT8* pucAtr, HUINT32* pulAtrLen, HUINT8* pucProtocol);
static HERROR local_cxDalSc_Write(HINT32 nScSlot, HUINT8* pucSndData, HUINT32 ulSndLen, HUINT8* pucRcvData, HUINT32* pulNumRead);
static HERROR local_cxDalSc_Read(HINT32 nScSlot, HUINT8* pucSndData, HUINT8* pucRcvData, HUINT32* pulNumRead);
static HERROR local_cxDalSc_ValidCard(HINT32 nScSlot, HBOOL bIsValid, HUINT16 usCaSysId, HUINT8 ucNumSession);
static HERROR local_cxDalSc_Deactivate(HINT32 nScSlot);

#if defined(CX_SC_PPS_EXCHANGE)
static HERROR local_cxDalSc_ParseAtr(HUINT8* pucAtr, SCI_ATR_INFO* pstAtrInfo);
#if defined(SUPPORT_CASE_PPS_IS_ABSENT)
static HERROR local_cxDalSc_RequestPps(SciHandle_t hSci, HUINT8 ucT, HUINT8* pucFi, HUINT8* pucDi, SvcCas_DevSciScParam_t* pstDefaultParam);
#else
static HERROR local_cxDalSc_RequestPps(SciHandle_t hSci, HUINT8 ucT, HUINT8 ucFi, HUINT8 ucDi, SvcCas_DevSciScParam_t* pstDefaultParam);
#endif
static HERROR local_cxDalSc_NegotiateByValueOrder(SciHandle_t hSci, SCI_ATR_INFO* pstAtrInfo, HUINT8 ucWI, HUINT8 ucBWI, SvcCas_DevSciScParam_t* pstDefaultParam);
static HERROR local_cxDalSc_Negotiate(SciHandle_t hSci, SCI_ATR_INFO* pstAtrInfo, SvcCas_DevSciScParam_t* pstDefaultParam);
#endif

/*******************************************************************/
#define ________CX_SC_Private_Static_Prototype_Body___________________________
/*******************************************************************/
static void local_cxDalSc_Init(Cx_SciEventNotifyCB_t cbFunc)
{
	int		i;
	HERROR	nResult			= ERR_FAIL;
	HUINT32	ulNumSlot;

	ENTER_FUNCTION;
	// Initialize local context
	local_cxDalSc_InitContext();

	if (cbFunc != NULL)
	{
		s_cbFunc = cbFunc;
	}

#if 0
	// PAL과 연동을 취한 초기화 OBAMA_Init에서 먼저 부르면 부르지 않아야 한다.
	nResult = svc_cas_DevScInit();
	if (nResult != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevScInit() returns fail.\n");
		LEAVE_FUNCTION;
		return;
	}
#endif

	ulNumSlot = svc_cas_DevScGetNumberOfModule();
	for (i = 0; i < ulNumSlot && i < CX_SC_SLOT_NUM; i++)
	{
		nResult = svc_cas_DevScRegisterCbFunc(i, eDxCAS_GROUPID_CX, s_stCxScContext[i].pfnCallBack, &(s_stCxScContext[i].hSci));

		if (nResult != ERR_OK)
		{
			HxLOG_Info("svc_cas_DevScRegisterCbFunc() returns fails.\n");
		}
		HxLOG_Print("s_stCxScContext[%d].hSci = 0x%08x\n", i, s_stCxScContext[i].hSci);

	}

	LEAVE_FUNCTION;
	return;
}
static void local_cxDalSc_InitContext(void)
{
	int	i;

	for (i = 0; i < CX_SC_SLOT_NUM; i++)
	{
		s_stCxScContext[i].hSci = (SciHandle_t)CX_SC_INVALID_HANDLE;
		s_stCxScContext[i].ucSlotId = (HUINT8)CX_SC_INVALID_SLOT;
		s_stCxScContext[i].pfnCallBack = local_cxDalSc_cbCard;
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
		s_stCxScContext[i].ucIsNoResponsePps = 0;	//set to received PPS
#endif
	}
}

#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
static void local_cxDalSc_SetIsNoReponsePpsWithSCIHandle(SciHandle_t hSci, HUINT8 ucIsNoResponsePps)
{
	int	i;

	for (i = 0; i < CX_SC_SLOT_NUM; i++)
	{
		if (s_stCxScContext[i].hSci == hSci)
		{
			s_stCxScContext[i].ucIsNoResponsePps = ucIsNoResponsePps;
			break;
		}
	}
}

static HBOOL local_cxDalSc_IsNoReponsePpsWithSCIHandle(SciHandle_t hSci)
{
	int		i;
	HBOOL	bIsNoResponsePps	= FALSE;

	for (i = 0; i < CX_SC_SLOT_NUM; i++)
	{
		if (s_stCxScContext[i].hSci == hSci)
		{
			if (s_stCxScContext[i].ucIsNoResponsePps)
			{
				bIsNoResponsePps = TRUE;
			}
			break;
		}
	}
	return bIsNoResponsePps;
}
#endif

static SciHandle_t local_cxDalSc_GetSciHandleBySlotId(HUINT8 ucSlotId)
{
	int			i;
	SciHandle_t	hSci	= (SciHandle_t)CX_SC_INVALID_HANDLE;

	for (i = 0; i < CX_SC_SLOT_NUM; i++)
	{
		if (s_stCxScContext[i].ucSlotId == ucSlotId)
		{
			hSci = s_stCxScContext[i].hSci;
			break;
		}
	}

	HxLOG_Info("SC Handle : 0x%08x\n", __FUNCTION__, __LINE__, hSci);
	return hSci;
}

static void local_cxDalSc_cbCard(SciHandle_t hSci, HUINT8 ucSlotId, SvcCas_DevSciEvent_e eStatus)
{
	int	i;

	ENTER_FUNCTION;

	for (i = 0; i < CX_SC_SLOT_NUM; i++)
	{
		if (s_stCxScContext[i].hSci == hSci)
		{
			break;
		}
	}

	if (i < 0 || i >= CX_SC_SLOT_NUM)
	{
		return;
	}

	if (eStatus == eCAS_SCI_SC_EXTRACTED)
	{
		if (s_cbFunc != NULL)
		{
			s_cbFunc(eCx_SC_EXTRACTED, ucSlotId);
		}
		//s_stCxScContext[i].ucSlotId = (HUINT8)CX_SC_INVALID_SLOT;
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
		s_stCxScContext[i].ucIsNoResponsePps = 0;	//set to received PPS
#endif

		HxLOG_Print("eCAS_SCI_SC_EXTRACTED\r\n");
	}
	else if (eStatus == eCAS_SCI_SC_INSERTED)
	{
		s_stCxScContext[i].ucSlotId = ucSlotId;
		HxLOG_Print("eCAS_SCI_SC_INSERTED\r\n");
		if (s_cbFunc != NULL)
		{
			s_cbFunc(eCx_SC_INSERTED, ucSlotId);
		}
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
		s_stCxScContext[i].ucIsNoResponsePps = 0;	//set to received PPS
#endif
	}

	LEAVE_FUNCTION;
}

static HERROR local_cxDalSc_Reset(HINT32 nScSlot, HUINT8* pucAtr, HUINT32* pulAtrLen, HUINT8* pucProtocol)
{
	SciHandle_t				hSci;
	HERROR					hErr			= ERR_FAIL;
#if defined(CX_SC_PPS_EXCHANGE)
	SCI_ATR_INFO			stAtrInfo;
	SvcCas_DevSciScStatus_e	eStatus;
#endif
	ENTER_FUNCTION;

	hSci = local_cxDalSc_GetSciHandleBySlotId((HUINT8)nScSlot);
	if (hSci == CX_SC_INVALID_HANDLE)
	{
		HxLOG_Info("Wrong SciHandle !!!\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	hErr = svc_cas_DevScReset(hSci, pucAtr, pulAtrLen, eCAS_SCM_RESET_TYPE_COLD, &s_stDefaultAtrParam);
	if (hErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevScReset() Error : 0x%08x\n", hErr);
		LEAVE_FUNCTION;
		return hErr;
	}
	if (*pulAtrLen < 4)
	{
		HxLOG_Info("svc_cas_DevScReset() Atr length (%d)\n", *pulAtrLen);
		LEAVE_FUNCTION;
		return hErr;
	}

#if defined(CX_SC_PPS_EXCHANGE)
	HxSTD_memset(&stAtrInfo, 0x00, sizeof(SCI_ATR_INFO));
	stAtrInfo.ulProtocol = s_stDefaultAtrParam.eProtocol;
	stAtrInfo.ulMinClock = s_stDefaultAtrParam.ulClock;
	stAtrInfo.ulBaudrate = s_stDefaultAtrParam.ulBaudrate;
	stAtrInfo.ucN = s_stDefaultAtrParam.ucN;

	stAtrInfo.ucWI = s_stDefaultAtrParam.ucCWI;
	stAtrInfo.ucBWI = s_stDefaultAtrParam.ucBWI;

#if 0	// PPS Nego test 위한 ATR 값...
	HxSTD_memset(pucAtr, 0, 32);

	pucAtr[0] = 0x3B;
	pucAtr[1] = 0x54;
	pucAtr[2] = 0x14;
	pucAtr[3] = 0x04;
	pucAtr[4] = 0x30;
	pucAtr[5] = 0x42;
	pucAtr[6] = 0x30;
	pucAtr[7] = 0x30;
#endif
	local_cxDalSc_ParseAtr(pucAtr, &stAtrInfo);
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
	if (local_cxDalSc_Negotiate(hSci, &stAtrInfo, &s_stDefaultAtrParam) != ERR_OK)
	{
		if (local_cxDalSc_IsNoReponsePpsWithSCIHandle(hSci) == TRUE)
		{
			HxLOG_Info("local_cxDalSc_Negotiate is failed[CX_ERR_SC_NO_RESPONSE_PPS]\n");
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}
#else
	local_cxDalSc_Negotiate(hSci, &stAtrInfo, &s_stDefaultAtrParam);
#endif



	/*
		work around
		카드가 빠진 상태에서 PPS가 계속 진행되면서 오동작이 되네요..
		initcass() 전에 카드가 정상적으로 있는지 확인해 보죠.
	*/
	svc_cas_DevScGetCardStatus(hSci, &eStatus);
	if (eStatus != eCAS_SCI_SC_PRESENT)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	*pucProtocol = stAtrInfo.ulProtocol;
#else
	*pucProtocol = (HUINT8)s_stDefaultAtrParam.eProtocol;
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalSc_Write(HINT32 nScSlot, HUINT8* pucSndData, HUINT32 ulSndLen, HUINT8* pucRcvData, HUINT32* pulNumRead)
{
	HUINT32		ulTotSndLen;
	HERROR		nRet;
	SciHandle_t	hSci;


	ENTER_FUNCTION;
	HxLOG_Print("[Enter] Slot(%d)[%d]\n", nScSlot, ulSndLen);

	hSci = local_cxDalSc_GetSciHandleBySlotId((HUINT8)nScSlot);
	if (hSci == CX_SC_INVALID_HANDLE)
	{
		HxLOG_Info("wrong handle\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	ulTotSndLen = ulSndLen; //CX_SC_T0_COMMAND_LEN + pucSndData[4];

	nRet = svc_cas_DevScTransfer(hSci, pucSndData, ulTotSndLen, pucRcvData, pulNumRead, 0);
	if (nRet != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevScWrite() Error\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalSc_Read(HINT32 nScSlot, HUINT8* pucSndData, HUINT8* pucRcvData, HUINT32* pulNumRead)
{
	HERROR		nRet;
	SciHandle_t	hSci;
	ENTER_FUNCTION;

	HxLOG_Print("[Enter] Slot(%d,%d)\n", nScSlot, *pulNumRead);

	hSci = local_cxDalSc_GetSciHandleBySlotId(nScSlot);
	if (hSci == CX_SC_INVALID_HANDLE)
	{
		HxLOG_Info("wrong handle\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	nRet = svc_cas_DevScTransfer(hSci, pucSndData, CX_SC_T0_COMMAND_LEN, pucRcvData, pulNumRead, 0);
	if (nRet != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevScTransfer() Error:%x\n", nRet);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalSc_ValidCard(HINT32 nScSlot, HBOOL bIsValid, HUINT16 usCaSysId, HUINT8 ucNumSession)
{
	SciHandle_t	hSci;
	ENTER_FUNCTION;
	HxLOG_Print("[Enter] Slot(%d)\n", nScSlot);

	hSci = local_cxDalSc_GetSciHandleBySlotId(nScSlot);
	if (hSci == CX_SC_INVALID_HANDLE)
	{
		HxLOG_Info("wrong handle\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	svc_cas_DevScNotifyScValidity(hSci, (HBOOL)bIsValid);

	// casMgr에게 알려준다.
	if (bIsValid)
	{
		HxLOG_Error("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
		HxLOG_Error("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
		HxLOG_Error("++++++++++++++++++++  Valid Smart Card  ++++++++++++++++++++\r\n");
		HxLOG_Error("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
		HxLOG_Error("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
		CXACT_UpdateCasInfo((HUINT16)nScSlot, &usCaSysId, (HUINT32)1, (HUINT32)ucNumSession, bIsValid);
	}
	else
	{
		HxLOG_Error("------------------------------------------------------------\r\n");
		HxLOG_Error("------------------------------------------------------------\r\n");
		HxLOG_Error("------------------- Invalid Smart Card ---------------------\r\n");
		HxLOG_Error("------------------------------------------------------------\r\n");
		HxLOG_Error("------------------------------------------------------------\r\n");
		CXACT_UpdateCasInfo((HUINT16)nScSlot, NULL, (HUINT32)0, (HUINT32)0, bIsValid);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalSc_Deactivate(HINT32 nScSlot)
{
	HERROR		hErr;
	SciHandle_t	hSci;

	HxLOG_Print("[Enter] Slot(%d)\n", nScSlot);

	hSci = local_cxDalSc_GetSciHandleBySlotId(nScSlot);
	if (hSci == CX_SC_INVALID_HANDLE)
	{
		HxLOG_Error("wrong handle\n");
		return ERR_FAIL;
	}

	hErr = svc_cas_DevScDeactivate(hSci);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevScDeactivate() returns fail (0x%08x)\r\n", hErr);
	}

	return hErr;
}

#if defined(CX_SC_PPS_EXCHANGE)
static HERROR local_cxDalSc_ParseAtr(HUINT8* pucAtr, SCI_ATR_INFO* pstAtrInfo)
{
	HUINT8* pucP;
	HUINT8	Y;
	HUINT8	index	= 1;

	pucP = pucAtr;		// TS
	HxLOG_Print("TS\t<%02x> (%d)\n", *pucP, *pucP);

	Y = *++pucP;		// T0
	HxLOG_Print("T0\t<%02x> (%d)\n", *pucP, *pucP);

	while (Y)
	{
		if (Y & 0x10)	// TA
		{
			if (index == 1)			// TA1
			{
				pstAtrInfo->ucFI = (*++pucP >> 4) & 0x0F;
				pstAtrInfo->ucDI = *pucP & 0x0F;
			}
			else if (index == 2)		// TA2
			{
				pstAtrInfo->bSpecificMode = TRUE;
				pstAtrInfo->bPpsUnable = ((*++pucP & 0x80) == 0x80);
				pstAtrInfo->bImplicitlyDefined = ((*pucP & 0x10) == 0x10);
			}
			else if (index > 2 && pstAtrInfo->ucT == 1)	// TA3 ~
			{
				pstAtrInfo->ucIFSC = *++pucP;
			}
			HxLOG_Print("TA%d\t<%02x> (%d)\n", index, *pucP, *pucP);
		}
		if (Y & 0x20)	// TB
		{
			if (index == 1)			// TB1
			{
				pstAtrInfo->ucII = (*++pucP >> 5) & 0x07;
				pstAtrInfo->ucPI1 = *pucP & 0x1F;
			}
			else if (index == 2)		// TB2
			{
				pstAtrInfo->ucPI2 = *++pucP;
			}
			else if (index > 2 && pstAtrInfo->ucT == 1)	// TB3 ~
			{
				pstAtrInfo->ucCWI = *++pucP & 0x0F;
				pstAtrInfo->ucBWI = (*pucP >> 4) & 0x0F;
			}
			HxLOG_Print("TB%d\t<%02x> (%d)\n", index, *pucP, *pucP);
		}
		if (Y & 0x40)	// TC
		{
			if (index == 1)			// TC1
			{
				pstAtrInfo->ucN = *++pucP;
			}
			else if (index == 2)		// TC2
			{
				pstAtrInfo->ucWI = *++pucP;
			}
			HxLOG_Print("TC%d\t<%02x> (%d)\n", index, *pucP, *pucP);
		}
		if (Y & 0x80)	// TD
		{
			if (index == 1)			// TD1
			{
				pstAtrInfo->ucTFirst = *++pucP & 0x0F;
				pstAtrInfo->ucT = pstAtrInfo->ucTFirst;
			}
			else
			{
				pstAtrInfo->ucT = *++pucP & 0x0F;
			}
			Y = *pucP;
			HxLOG_Print("TD%d\t<%02x> (%d)\n", index, *pucP, *pucP);
		}
		else
		{
			Y = 0;
		}
		index++;
	}

	return ERR_OK;
}

#if defined(SUPPORT_CASE_PPS_IS_ABSENT)
static HERROR local_cxDalSc_RequestPps(SciHandle_t hSci, HUINT8 ucT, HUINT8* pucFi, HUINT8* pucDi, SvcCas_DevSciScParam_t* pstDefaultParam)
#else
static HERROR local_cxDalSc_RequestPps(SciHandle_t hSci, HUINT8 ucT, HUINT8 ucFi, HUINT8 ucDi, SvcCas_DevSciScParam_t* pstDefaultParam)
#endif
{
	HUINT8	aPpsRequest[4];
	HUINT8	aPpsResponse[4]	=
	{
		0, 0, 0, 0
	};
	HUINT8	ucNumRead;
	HERROR	nErr			= ERR_OK;
#if defined(SUPPORT_CASE_PPS_IS_ABSENT)
	HUINT8	ucFi			= *pucFi, ucDi = *pucDi;
#endif
	ENTER_FUNCTION;
	nErr = svc_cas_DevScSetParams(hSci, pstDefaultParam);

	if (nErr)
	{
		HxLOG_Error("[local_cxDalSc_RequestPps][DI_SC_SetParams] err<%x>\n", nErr);
	}

	aPpsRequest[0] = 0xFF;
	aPpsRequest[1] = 0x10 | (ucT & 0x0F);
	aPpsRequest[2] = ((ucFi << 4) & 0xF0) | (ucDi & 0x0F);
	aPpsRequest[3] = aPpsRequest[0] ^ aPpsRequest[1] ^ aPpsRequest[2];

	nErr = svc_cas_DevScNegotiatePTS(hSci, aPpsRequest, 4, aPpsResponse, &ucNumRead);
	if (nErr)
	{
		HxLOG_Error("[local_cxDalSc_RequestPps][svc_cas_DevScNegotiatePTS] err<%x>\n", nErr);
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
		local_cxDalSc_SetIsNoReponsePpsWithSCIHandle(hSci, 1);	//set to no response PPS
		return ERR_FAIL;
#endif
	}

	HxLOG_Print("Req : %x, %x, %x, %x\n", aPpsRequest[0], aPpsRequest[1], aPpsRequest[2], aPpsRequest[3]);
	HxLOG_Print("Res : %x, %x, %x, %x (%d)\n", aPpsResponse[0], aPpsResponse[1], aPpsResponse[2], aPpsResponse[3], ucNumRead);


#if defined(SUPPORT_CASE_PPS_IS_ABSENT)
	if (aPpsResponse[2] == 0xFF)	//PPS1 is absent.
	{
		*pucFi = 0;
		*pucDi = 1;
	}
#endif

	if ((aPpsResponse[0] | aPpsResponse[1] | aPpsResponse[2] | aPpsResponse[3]) == 0)
	{
		HxLOG_Error("[local_cxDalSc_RequestPps][DI_SC_NegotiatePTS] err<%x>\n", nErr);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// skk
	// (1) response is same with request
	// (2) reponse(1-4) is same with request (1-4) and response(5-7) set to 0
	if (!((aPpsRequest[1] == aPpsResponse[1]) && (aPpsRequest[2] == aPpsResponse[2])) &&	//
		!((aPpsResponse[1] < 0x10) && ((aPpsRequest[1] & 0xF) == (aPpsResponse[1] & 0xF))))
	{
		HxLOG_Error("[local_cxDalSc_RequestPps] reject \n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalSc_NegotiateByValueOrder(SciHandle_t hSci, SCI_ATR_INFO* pstAtrInfo, HUINT8 ucWI, HUINT8 ucBWI, SvcCas_DevSciScParam_t* pstDefaultParam)
{
	HUINT8					ucT;
	HUINT8					ucF, ucD;
	HUINT8					ucCntF, ucCntD;
	SCI_ATR_INFO* 			p;
	HERROR					nErr	= ERR_OK;
	SvcCas_DevSciScParam_t	stScParam;
	int						i;

	p = pstAtrInfo;

	ucF = p->ucFI;
	ucD = p->ucDI;

	ucCntF = s_aucFiOrder[ucF];
	ucCntD = s_aucDiOrder[ucD];

	ENTER_FUNCTION;
	HxLOG_Print("[T=%x, %x]\n", p->ucT, p->ucTFirst);
	if (p->ucT == p->ucTFirst)
	{
		ucT = p->ucT;
		HxLOG_Print("[local_cxDalSc_NegotiateByValueOrder] UNIQUE T\n");
		while (ucCntF >= 1)		// ucCntF try
		{
			ucF = s_aucFiValue[ucCntF];
			while (ucCntD >= 1)	// ucCntD try
			{
				ucD = s_aucDiValue[ucCntD];
				HxLOG_Print("[local_cxDalSc_NegotiateByValueOrder] PPS T(%d) F(%d) D(%d)\n", ucT, ucF, ucD);
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
				if (local_cxDalSc_IsNoReponsePpsWithSCIHandle(hSci) == FALSE)
#endif
				{
#if defined(SUPPORT_CASE_PPS_IS_ABSENT)
					nErr = local_cxDalSc_RequestPps(hSci, ucT, &ucF, &ucD, pstDefaultParam);
#else
					nErr = local_cxDalSc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
#endif
				}
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
				else
				{
					ucF = pstDefaultParam->ucFI;
					ucD = pstDefaultParam->ucDI;
					nErr = ERR_OK;
				}
#endif
				if (nErr == ERR_OK)
				{
					stScParam.eProtocol = ucT;
					stScParam.ulClock = p->ulMinClock;
					stScParam.ulBaudrate = p->ulBaudrate;
					stScParam.ucFI = ucF;
					stScParam.ucDI = ucD;
					stScParam.ucN = p->ucN;
					stScParam.ucCWI = ucWI;
					stScParam.ucBWI = ucBWI;

					nErr = svc_cas_DevScSetParams(hSci, &stScParam);
					if (nErr)
					{
						HxLOG_Error("[local_cxDalSc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
					}
					HxLOG_Print("[local_cxDalSc_NegotiateByValueOrder ]T(%d) F(%d) D(%d)", ucT, ucF, ucD);

					LEAVE_FUNCTION;
					return ERR_OK;
				}
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
				else
				{
					if (local_cxDalSc_IsNoReponsePpsWithSCIHandle(hSci) == TRUE)
					{
						HxLOG_Error("[local_cxDalSc_IsNoReponsePpsWithSCIHandle][local_cxDalSc_RequestPps] err<%x>\n", nErr);
						LEAVE_FUNCTION;
						return ERR_FAIL;
					}
				}
#endif
				ucCntD--;
			}
			ucCntF--;
		}
	}
	else
	{
		HxLOG_Print("[local_cxDalSc_NegotiateByValueOrder] MULTIPLE T\n");
		for (i = 0; i < 2; i++)
		{
			if (i == 0)
			{
				ucT = p->ucTFirst;
			}
			else
			{
				ucT = p->ucT;
			}

			while (ucCntF >= 1)		// ucCntF try
			{
				ucF = s_aucFiValue[ucCntF];
				while (ucCntD >= 1)	// ucCntD try
				{
					ucD = s_aucDiValue[ucCntD];
					HxLOG_Print("[local_cxDalSc_NegotiateByValueOrder] PPS T(%d) F(%d) D(%d)\n", ucT, ucF, ucD);
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
					if (local_cxDalSc_IsNoReponsePpsWithSCIHandle(hSci) == FALSE)
#endif
					{
#if defined(SUPPORT_CASE_PPS_IS_ABSENT)
						nErr = local_cxDalSc_RequestPps(hSci, ucT, &ucF, &ucD, pstDefaultParam);
#else
						nErr = local_cxDalSc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
#endif
					}
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
					else
					{
						ucF = pstDefaultParam->ucFI;
						ucD = pstDefaultParam->ucDI;
						nErr = ERR_OK;
					}
#endif
					if (nErr == ERR_OK)
					{
						stScParam.eProtocol = ucT;
						stScParam.ulClock = p->ulMinClock;
						stScParam.ulBaudrate = p->ulBaudrate;
						stScParam.ucFI = ucF;
						stScParam.ucDI = ucD;
						stScParam.ucN = p->ucN;
						stScParam.ucCWI = ucWI;
						stScParam.ucBWI = ucBWI;

						nErr = svc_cas_DevScSetParams(hSci, &stScParam);
						if (nErr)
						{
							HxLOG_Error("[local_cxDalSc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
						}
						HxLOG_Print("[local_cxDalSc_NegotiateByValueOrder] T(%d) F(%d) D(%d)", ucT, ucF, ucD);
						LEAVE_FUNCTION;
						return ERR_OK;
					}
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
					else
					{
						if (local_cxDalSc_IsNoReponsePpsWithSCIHandle(hSci) == TRUE)
						{
							HxLOG_Error("[local_cxDalSc_NegotiateByValueOrder][local_cxDalSc_RequestPps] err<%x>\n", nErr);
							return ERR_FAIL;
						}
					}
#endif
					ucCntD--;
				}
				ucCntF--;
			}
		}
	}

	LEAVE_FUNCTION;
	return ERR_FAIL;
}

static HERROR local_cxDalSc_Negotiate(SciHandle_t hSci, SCI_ATR_INFO* pstAtrInfo, SvcCas_DevSciScParam_t* pstDefaultParam)
{
	HUINT8					ucWI, ucBWI;
	SCI_ATR_INFO* 			p;
	HERROR					nErr			= ERR_OK;
	SvcCas_DevSciScParam_t	stScParam;
	SvcCas_DevSciScParam_t	stDefaultParam;

	ENTER_FUNCTION;
	HxLOG_Print("[++local_cxDalSc_Negotiate]\n");

	p = pstAtrInfo;

	if (p->ucT == eCAS_SCI_T0)
	{
		ucWI = p->ucWI;
	}
	else
	{
		ucWI = p->ucCWI;
	}

	if (p->ucT == eCAS_SCI_T14 && p->ucBWI < 5)
	{
		ucBWI = 5;
	}
	else
	{
		ucBWI = p->ucBWI;
	}

	if (p->bSpecificMode)
	{
		HxLOG_Print("[local_cxDalSc_Negotiate] SPECIFIC\n");

		if (p->bImplicitlyDefined)
		{
			HxLOG_Print("[local_cxDalSc_Negotiate] IMPLICIT\n");
			goto Atr_Parameters;
		}
		else
		{
			HxLOG_Print("[local_cxDalSc_Negotiate] EXPLICIT\n");
			goto Atr_Parameters;
		}
	}
	else	// negotiable mode
	{
		HxLOG_Print("[local_cxDalSc_Negotiate] NEGOTIABLE\n");

		if (p->ucFI == 0 && p->ucDI == 0)
		{
			goto Atr_Parameters;
		}
		HxSTD_memcpy(&stDefaultParam, pstDefaultParam, sizeof(SvcCas_DevSciScParam_t));
		stDefaultParam.ucN = p->ucN;

		nErr = local_cxDalSc_NegotiateByValueOrder(hSci, p, ucWI, ucBWI, &stDefaultParam);
		if (nErr)
		{
#if defined(SUPPORT_RESET_AFTER_NO_RESPONSE_PPS)
			if (local_cxDalSc_IsNoReponsePpsWithSCIHandle(hSci) == TRUE)
			{
				LEAVE_FUNCTION;
				return ERR_FAIL;
			}
			else
#endif
			{
				goto Default_Parameters;
			}
		}
		else
		{
			LEAVE_FUNCTION;
			return ERR_OK;
		}
	}

Default_Parameters :
	HxLOG_Print("[local_cxDalSc_Negotiate] SET Default FI DI\n");

	// Comments :: Conax의 경우 Clock을 typically 3.6Mhz이고, 4.5Mhz보다 적은 clock을 사용해야 함.
	//				4.5Mhz를 쓸 경우 동작하지 않는 card가 있음.
	//	=-> baudrate을 9600으로 설정하면 ST, CNXT DD 모두 3.6Mhz로 clock을 설정함.
	pstDefaultParam->ulBaudrate = 9600;

	nErr = svc_cas_DevScSetParams(hSci, pstDefaultParam);

	if (nErr)
	{
		HxLOG_Error("[local_cxDalSc_Negotiate][DI_SC_SetParams] err<%x>\n", nErr);
	}

	LEAVE_FUNCTION;
	return ERR_OK;

Atr_Parameters :
	HxLOG_Print("[local_cxDalSc_Negotiate] SET ATR FI DI\n");

	stScParam.eProtocol = p->ucT;
	stScParam.ulClock = p->ulMinClock;
	stScParam.ulBaudrate = p->ulBaudrate;
	stScParam.ucN = p->ucN;
	stScParam.ucCWI = p->ucWI;
	stScParam.ucBWI = p->ucBWI;

	if (p->ucFI == 0 && p->ucDI == 0)
	{
		stScParam.ucFI = 0x1;
		stScParam.ucDI = 0x1;
	}
	else
	{
		stScParam.ucFI = p->ucFI;
		stScParam.ucDI = p->ucDI;
	}

	nErr = svc_cas_DevScSetParams(hSci, &stScParam);
	if (nErr)
	{
		HxLOG_Error("[local_cxDalSc_Negotiate][DI_SC_SetParams] err<%x>\n", nErr);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}
#endif


/*******************************************************************/
#define ________CX_SC_Public_Functions_Body___________________________________
/*******************************************************************/
void CXSC_Init(Cx_SciEventNotifyCB_t cbFunc)
{
	local_cxDalSc_Init(cbFunc);
}

HERROR CXSC_Reset(HINT32 nScSlot, HUINT8* pucAtr, HUINT32* pulAtrLen, HUINT8* pucProtocol)
{
	return local_cxDalSc_Reset(nScSlot, pucAtr, pulAtrLen, pucProtocol);
}

HERROR CXSC_Write(HINT32 nScSlot, HUINT8* pucSndData, HUINT32 ulSndLen, HUINT8* pucRcvData, HUINT32* pulNumRead)
{
	return local_cxDalSc_Write(nScSlot, pucSndData, ulSndLen, pucRcvData, pulNumRead);
}

HERROR CXSC_Read(HINT32 nScSlot, HUINT8* pucSndData, HUINT8* pucRcvData, HUINT32* pulNumRead)
{
	return local_cxDalSc_Read(nScSlot, pucSndData, pucRcvData, pulNumRead);
}

HERROR CXSC_ValidCard(HINT32 nScSlot, HBOOL bIsValid, HUINT16 usCaSysId, HUINT8 ucNumSession)
{
	return local_cxDalSc_ValidCard(nScSlot, bIsValid, usCaSysId, ucNumSession);
}

HERROR CXSC_Deactivate(HINT32 nScSlot)
{
	return local_cxDalSc_Deactivate(nScSlot);
}
