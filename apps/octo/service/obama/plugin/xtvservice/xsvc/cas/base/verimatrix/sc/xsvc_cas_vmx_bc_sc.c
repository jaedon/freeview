/* (c) 2011-2015 Humax Co., Ltd. 
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
/**
	@file     xsvc_cas_vmx_bc_sc.c
	@brief    xsvc_cas_vmx_bc_sc.c (Verimatrix SC)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "pal_sc.h"

#include "_xsvc_cas_vmx_bc_main.h"
#include "vmx_vr_bc_api.h"
#include "_xsvc_cas_vmx_bc_sc.h"
#include "_xsvc_cas_vmx_os.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define VMXBC_PPS_EXCHANGE	// PPS exchange를 여기서 구현한다. ==> sc mgr이나 PAL_SC에서 해주는 것이 맞는 것 같은데...
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

typedef struct tVmxBc_SC_Inst
{
	SCI_HANDLE 		hCardHandle;
	SCI_CB_FUNC		pfnCallBack;

	unsigned char		ucSlotId;
	unsigned char		ucIsCardMarrying;
	unsigned char		dummy1;	// for 4 byte align
	unsigned char		dummy2;
} VmxBc_SC_Inst;

#ifdef VMXBC_PPS_EXCHANGE
typedef struct
{
	HUINT32	ulProtocol;

	HUINT32 ulMinClock;
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
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC VmxBc_SC_Inst			s_VmxBcInst[VMXBC_SC_SLOT_NUM];

STATIC SCI_SC_PARAM 			s_stDefaultAtrParam =
{
		SCI_T0,				/* protocol : default - T = 0 Protocol  	*/
		0,					/* min clock : 확인해 봐야함 di에서 not use	*/
		0,					/* baudrate :  확인해 봐야함 					*/
		1,					/* FI : 372 	betacrypt default value 			*/
		1,					/* DI : 1 		betacrypt default value 			*/
		0,					/* N : 0		betacrypt default value 			*/
		13, 					/* CWI : 13   ISO-7816 default value  			*/
		4,					/* BWI : 4     ISO-7816 default value			*/
};

#ifdef VMXBC_PPS_EXCHANGE
STATIC HUINT8 s_aucFiOrder[16] =
{	0xFF, 1, 3, 4, 7, 8, 10, 0xFF, 0xFF, 2, 5, 6, 9, 11, 0xFF, 0xFF };

STATIC HUINT8 s_aucDiOrder[16] =
{	0xFF, 1, 2, 3, 4, 6, 8, 0xFF, 5, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

STATIC HUINT8 s_aucFiValue[16] =
{	0xFF, 1, 9, 2, 3, 10, 11, 4, 5, 12, 6, 13, 0xFF, 0xFF, 0xFF, 0xFF };

STATIC HUINT8 s_aucDiValue[16] =
{	0xFF, 1, 2, 3, 4, 8, 5, 9, 6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
#endif

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*	
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/
STATIC void vmx_bc_sc_cbCard0(SCI_HANDLE hSci, HUINT8 ucCardSlotId, SCI_SC_EVENT nStatus);
#if (VMXBC_SC_SLOT_NUM > 1)
STATIC void vmx_bc_sc_cbCard1(SCI_HANDLE hSci, HUINT8 ucCardSlotId, SCI_SC_EVENT nStatus);
#endif

STATIC VmxBc_SC_Inst * vmx_bc_GetVmxBcInst(HUINT8 usSlot)
{
	return &s_VmxBcInst[usSlot];
}

STATIC SCI_HANDLE vmx_bc_sc_GetSCIHandleWithSlotID(HUINT8 ucCardSlotId)
{
	int	i;
	VmxBc_SC_Inst *pBcScInst = NULL;

	for (i=0; i < VMXBC_SC_SLOT_NUM; i++)
	{
		pBcScInst = vmx_bc_GetVmxBcInst(i);
		if(pBcScInst == NULL)
			continue;

		if (ucCardSlotId == pBcScInst->ucSlotId)
		{
			return pBcScInst->hCardHandle;
		}
	}

	HxLOG_Error(DBG_ASSERT, ("VMXBC_SC_INVALID_HANDLE\n");

	return VMXBC_SC_INVALID_HANDLE;
}

STATIC int vmx_bc_sc_GetIndexWithSlotID(HUINT8 ucCardSlotId)
{
	int	idx;
	VmxBc_SC_Inst *pBcScInst = NULL;

	for (idx=0; idx < VMXBC_SC_INVALID_HANDLE; idx++)
	{
		pBcScInst = vmx_bc_GetVmxBcInst(idx);
		if(pBcScInst == NULL)
			continue;

		if (ucCardSlotId == pBcScInst->ucSlotId)
		{
			return idx;
		}
	}

	HxLOG_Error(DBG_ASSERT, ("SlotId error\n");
	return -1;
}

STATIC void vmx_bc_sc_BcInstInit(void)
{
	int i;
	VmxBc_SC_Inst *pBcScInst = NULL;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	for (i=0; i < VMXBC_SC_SLOT_NUM; i++)
	{
		pBcScInst = vmx_bc_GetVmxBcInst(i);
		if(pBcScInst == NULL)
			continue;

		HxLOG_Print(DBG_PRINT, ("\tInstance[%d] is exist\n", i);

		pBcScInst->hCardHandle = VMXBC_SC_INVALID_HANDLE;
		pBcScInst->ucSlotId = VMXBC_SC_INVALID_SLOT;

		if (i == 0)
			pBcScInst->pfnCallBack = vmx_bc_sc_cbCard0;
#if (VMXBC_SC_SLOT_NUM > 1)
		else
			pBcScInst->pfnCallBack = vmx_bc_sc_cbCard1;
#endif
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
}

STATIC void vmx_bc_sc_cbCard0(SCI_HANDLE hSci, HUINT8 ucCardSlotId, SCI_SC_EVENT nStatus)
{
	int				idx;
	Vmx_Message_t stMsg;
	VmxBc_SC_Inst *pBcScInst = NULL;

	HxLOG_Print(DBG_PRINT, ("[Enter] Handle(0x%x) Slot(%d) Status(%d) \n", hSci, ucCardSlotId, nStatus);

	// sc_mgr에서 관리하는 handle과 slot ID를 mapping 시킨다.
	// CAS Module에서 read/write 요구시에 validity를 보장하기 위해서...
	for (idx=0; idx < VMXBC_SC_SLOT_NUM; idx++)
	{
		pBcScInst = vmx_bc_GetVmxBcInst(idx);
		if(pBcScInst == NULL)
			continue;

		if (pBcScInst->hCardHandle == hSci)
		{
			break;
		}
	}

	// for removing prevent issue
	if (idx < 0 || idx >= VMXBC_SC_SLOT_NUM)
	{
		HxLOG_Error(DBG_ASSERT, ("ScInst Idx(%d)\n", idx);
		return;
	}

	HxLOG_Print(DBG_PRINT, ("ScInst Idx(%d)\n", idx);

	if (nStatus == SCI_SC_EXTRACTED)
	{
		pBcScInst->ucSlotId = VMXBC_SC_INVALID_SLOT;
		// Smart Card Task로 slot ID를 전송한다.
		stMsg.param1 = eVmxBc_MsgType_ScExtracted;
		stMsg.param2 = ucCardSlotId;
		
		HxLOG_Print(DBG_PRINT, ("- SCI_SC_EXTRACTED\n");
		
		VMX_MSG_Send(vmx_vr_bc_sc_GetMsgQueueId(), &stMsg, sizeof(Vmx_Message_t));
	}
	else if (nStatus == SCI_SC_INSERTED)
	{
		pBcScInst->ucSlotId = ucCardSlotId;
		stMsg.param1 = eVmxBc_MsgType_ScInserted;
		stMsg.param2 = ucCardSlotId;

		HxLOG_Print(DBG_PRINT, ("- SCI_SC_INSERTED, ucCardSlotId(%d)\n", ucCardSlotId);

		VMX_MSG_Send(vmx_vr_bc_sc_GetMsgQueueId(), &stMsg, sizeof(Vmx_Message_t));
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
}

#if (VMXBC_SC_SLOT_NUM > 1)
STATIC void vmx_bc_sc_cbCard1(SCI_HANDLE hSci, HUINT8 ucCardSlotId, SCI_SC_EVENT nStatus)
{
	int				idx;
	VmxBc_SC_Inst *pBcScInst = NULL;

	HxLOG_Print(DBG_PRINT, ("[Enter] Handle(0x%x) Slot(%d) Status(%d) \n", hSci, ucCardSlotId, nStatus);

	// sc_mgr에서 관리하는 handle과 slot ID를 mapping 시킨다.
	// CAS Module에서 read/write 요구시에 validity를 보장하기 위해서...
	for (idx=0; idx < VMXBC_SC_SLOT_NUM; idx++)
	{
		pBcScInst = vmx_bc_GetVmxBcInst(idx);
		if(pBcScInst == NULL)
			continue;

		if (pBcScInst->hCardHandle == hSci)
		{
			break;
		}
	}

	// for removing prevent issue
	if (idx < 0 || idx >= VMXBC_SC_SLOT_NUM)
	{
		return;
	}

	if (nStatus == SCI_SC_EXTRACTED)
	{
		// Smart Card Task로 slot ID를 전송한다.		
		pBcScInst->ucSlotId = VMXBC_SC_INVALID_SLOT;
		stMsg.param1 = eVmxBc_MsgType_ScExtracted;
		stMsg.param2 = ucCardSlotId;		
		VMX_MSG_Send(vmx_vr_bc_sc_GetMsgQueueId(), &stMsg, sizeof(Vmx_Message_t));
	}
	else if (nStatus == SCI_SC_INSERTED)
	{
		pBcScInst->ucSlotId = ucCardSlotId;
		stMsg.param1 = eVmxBc_MsgType_ScInserted;
		stMsg.param2 = ucCardSlotId;		
		VMX_MSG_Send(vmx_vr_bc_sc_GetMsgQueueId(), &stMsg, sizeof(Vmx_Message_t));
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
}
#endif

#ifdef VMXBC_PPS_EXCHANGE
STATIC HERROR vmx_bc_sc_ParseAtr(HUINT8 *pucAtr, SCI_ATR_INFO *pstAtrInfo)
{
	HUINT8	*pucP;
	HUINT8	Y;
	HUINT8	index = 1;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	pucP = pucAtr;		// TS
	VmxBcScData(DBG_DATA, ("TS\t<%02x> (%d)\n", *pucP, *pucP));

	Y = *++pucP;		// T0
	VmxBcScData(DBG_DATA, ("T0\t<%02x> (%d)\n", *pucP, *pucP));

	while(Y)
	{
		if(Y & 0x10)	// TA
		{
			if(index == 1)			// TA1
			{
				pstAtrInfo->ucFI = (*++pucP >> 4) & 0x0F;
				pstAtrInfo->ucDI = *pucP & 0x0F;
			}
			else if(index == 2)		// TA2
			{
				pstAtrInfo->bSpecificMode = TRUE;
				pstAtrInfo->bPpsUnable = ((*++pucP & 0x80) == 0x80);
				pstAtrInfo->bImplicitlyDefined = ((*pucP & 0x10) == 0x10);
			}
			else if(index > 2 && pstAtrInfo->ucT == 1)	// TA3 ~
			{
				pstAtrInfo->ucIFSC = *++pucP;
			}
			VmxBcScData(DBG_DATA, ("TA%d\t<%02x> (%d)\n", index, *pucP, *pucP));
		}
		if(Y & 0x20)	// TB
		{
			if(index == 1)			// TB1
			{
				pstAtrInfo->ucII = (*++pucP >> 5) & 0x07;
				pstAtrInfo->ucPI1 = *pucP & 0x1F;
			}
			else if(index == 2)		// TB2
			{
				pstAtrInfo->ucPI2 = *++pucP;
			}
			else if(index > 2 && pstAtrInfo->ucT == 1)	// TB3 ~
			{
				pstAtrInfo->ucCWI = *++pucP & 0x0F;
				pstAtrInfo->ucBWI = (*pucP >> 4) & 0x0F;
			}
			VmxBcScData(DBG_DATA, ("TB%d\t<%02x> (%d)\n", index, *pucP, *pucP));
		}
		if(Y & 0x40)	// TC
		{
			if(index == 1)			// TC1
			{
				pstAtrInfo->ucN = *++pucP;
			}
			else if(index == 2)		// TC2
			{
				pstAtrInfo->ucWI = *++pucP;
			}
			VmxBcScData(DBG_DATA, ("TC%d\t<%02x> (%d)\n", index, *pucP, *pucP));
		}
		if(Y & 0x80)	// TD
		{
			if(index == 1)			// TD1
			{
				pstAtrInfo->ucTFirst = *++pucP & 0x0F;
				pstAtrInfo->ucT = pstAtrInfo->ucTFirst;
			}
			else
			{
				pstAtrInfo->ucT = *++pucP & 0x0F;
			}
			Y = *pucP;
			VmxBcScData(DBG_DATA, ("TD%d\t<%02x> (%d)\n", index, *pucP, *pucP));
		}
		else
		{
			Y = 0;
		}
		index++;
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;
}

STATIC HERROR vmx_bc_sc_RequestPps(SCI_HANDLE hSci, HUINT8 ucT, HUINT8 ucFi, HUINT8 ucDi, SCI_SC_PARAM *pstDefaultParam)
{
	HUINT8	aPpsRequest[4];
	HUINT8	aPpsResponse[4] = {0,0,0,0};
	HUINT8	ucNumRead;
	HERROR nErr = ERR_OK;

	VmxBcScData(DBG_DATA, ("[++vmx_bc_sc_RequestPps]\n"));

	nErr = PAL_SC_SetParams(hSci, pstDefaultParam);

	if(nErr)
	{
		HxLOG_Error(DBG_ASSERT, ("[vmx_bc_sc_RequestPps][DI_SC_SetParams] err<%x>\n", nErr);
	}

	aPpsRequest[0] = 0xFF;
	aPpsRequest[1] = 0x10 | (ucT & 0x0F);
	aPpsRequest[2] = ( (ucFi << 4) & 0xF0 ) | (ucDi & 0x0F );
	aPpsRequest[3] = aPpsRequest[0] ^ aPpsRequest[1] ^ aPpsRequest[2];

	nErr = PAL_SC_NegotiatePTS(hSci, aPpsRequest, 4, aPpsResponse, &ucNumRead);
	if(nErr)
	{
		HxLOG_Error(DBG_ASSERT, ("[SCI_INT_RequestPps][DDI_SC_NegotiatePTS] err<%x>\n", nErr);
	}

	VmxBcScData(DBG_DATA, ("Req : %x, %x, %x, %x\n", aPpsRequest[0], aPpsRequest[1], aPpsRequest[2], aPpsRequest[3]));
	VmxBcScData(DBG_DATA, ("Res : %x, %x, %x, %x (%d)\n", aPpsResponse[0], aPpsResponse[1], aPpsResponse[2], aPpsResponse[3], ucNumRead));

	if( (aPpsResponse[0] | aPpsResponse[1] | aPpsResponse[2] | aPpsResponse[3]) == 0 )
	{
		HxLOG_Error(DBG_ASSERT, ("[vmx_bc_sc_RequestPps][DI_SC_NegotiatePTS] err<%x>\n", nErr);
		return ERR_FAIL;
	}

	// skk
	// (1) response is same with request
	// (2) reponse(1-4) is same with request (1-4) and response(5-7) set to 0
//	if(memcmp(aPpsRequest, aPpsResponse, 4))
	if (!((aPpsRequest[1] == aPpsResponse[1]) && (aPpsRequest[2] == aPpsResponse[2])) &&
		!((aPpsResponse[1] < 0x10) && ((aPpsRequest[1] & 0xF) == (aPpsResponse[1] & 0xF))))
	{
		HxLOG_Error(DBG_ASSERT, ("[vmx_bc_sc_RequestPps] reject \n");
		return ERR_FAIL;
	}

	#if 0
	if ((aPpsResponse[1] & 0x10) == 0)
	{
		*pucPpsValue = 0x11; //default Fd, Dd
	}
	else
	{
		*pucPpsValue = aPpsResponse[2];
	}
	#endif

	VmxBcScData(DBG_DATA, ("[--vmx_bc_sc_RequestPps]\n"));
	return ERR_OK;
}

STATIC HERROR vmx_bc_sc_NegotiateByValueOrder(SCI_HANDLE hSci, SCI_ATR_INFO *pstAtrInfo, HUINT8 ucWI, HUINT8 ucBWI, SCI_SC_PARAM *pstDefaultParam)
{
	int ulErr = ERR_OK;
	HUINT8	ucT;
	HUINT8	ucF, ucD;
	HUINT8	ucCntF, ucCntD;
	SCI_ATR_INFO *p;
	HERROR nErr = ERR_OK;
	SCI_SC_PARAM stScParam;
	int i;

	p = pstAtrInfo;

#if 1
	ucF = p->ucFI;
	ucD = p->ucDI;
#else
	// vd인경우. 아래에 해당 문제 발생하면 아래내용 확인
	ucFIMax = (ucPpsMax & 0xF0) >> 4;
	ucDIMax = ucPpsMax & 0x0F;

	ucF = (s_aucFiOrder[p->ucFI] > s_aucFiOrder[ucFIMax]) ? ucFIMax : p->ucFI;
	ucD = (s_aucDiOrder[p->ucDI] > s_aucDiOrder[ucDIMax]) ? ucDIMax : p->ucDI;	//1 ucD = ucDIMax;
#endif

	ucCntF = s_aucFiOrder[ucF];
	ucCntD = s_aucDiOrder[ucD];

	VmxBcScData(DBG_DATA, ("[T=%x, %x]\n", p->ucT, p->ucTFirst));
	if(p->ucT == p->ucTFirst)
	{
		ucT = p->ucT;
		VmxBcScData(DBG_DATA, ("[vmx_bc_sc_NegotiateByValueOrder] UNIQUE T\n"));
		while(ucCntF >= 1)		// ucCntF try
		{
			ucF = s_aucFiValue[ucCntF];
			while(ucCntD >= 1)	// ucCntD try
			{
				ucD = s_aucDiValue[ucCntD];
				VmxBcScData(DBG_DATA, ("[vmx_bc_sc_NegotiateByValueOrder] PPS T(%d) F(%d) D(%d) N(%d)\n", ucT, ucF, ucD, p->ucN));
				{
					ulErr = vmx_bc_sc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
				}
				if(ulErr == ERR_OK)
				{
					stScParam.eProtocol = (SCI_PROTOCOL)ucT;
					stScParam.ulClock= p->ulMinClock;
					stScParam.ulBaudrate = p->ulBaudrate;
					stScParam.ucFI = ucF;
					stScParam.ucDI =ucD;
					stScParam.ucN = p->ucN;
					stScParam.ucCWI = ucWI;
					stScParam.ucBWI = ucBWI;

					nErr = PAL_SC_SetParams(hSci, &stScParam);
					if(nErr)
					{
						HxLOG_Error(DBG_ASSERT, ("[vmx_bc_sc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
					}
					VmxBcScData(DBG_DATA, ("[vmx_bc_sc_NegotiateByValueOrder ]T(%d) F(%d) D(%d)", ucT, ucF, ucD));
					return ERR_OK;
				}
				ucCntD--;
			}
			ucCntF--;
		}
		return ERR_FAIL;
	}
	else
	{
		VmxBcScData(DBG_DATA, ("[vmx_bc_sc_NegotiateByValueOrder] MULTIPLE T\n"));
		for(i=0; i<2; i++)
		{
			if(i == 0)
				ucT = p->ucTFirst;
			else
				ucT = p->ucT;

			while(ucCntF >= 1)		// ucCntF try
			{
				ucF = s_aucFiValue[ucCntF];
				while(ucCntD >= 1)	// ucCntD try
				{
					ucD = s_aucDiValue[ucCntD];
					VmxBcScData(DBG_DATA, ("[vmx_bc_sc_NegotiateByValueOrder] PPS T(%d) F(%d) D(%d) N(%d)\n", ucT, ucF, ucD, p->ucN));
					{
						ulErr = vmx_bc_sc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
					}
					if(ulErr == ERR_OK)
					{
						stScParam.eProtocol = ucT;
						stScParam.ulClock= p->ulMinClock;
						stScParam.ulBaudrate = p->ulBaudrate;
						stScParam.ucFI = ucF;
						stScParam.ucDI =ucD;
						stScParam.ucN = p->ucN;
						stScParam.ucCWI = ucWI;
						stScParam.ucBWI = ucBWI;

						nErr = PAL_SC_SetParams(hSci, &stScParam);
						if(nErr)
						{
							HxLOG_Error(DBG_ASSERT, ("[vmx_bc_sc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
						}
						VmxBcScData(DBG_DATA, ("[vmx_bc_sc_NegotiateByValueOrder] T(%d) F(%d) D(%d)", ucT, ucF, ucD));
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

STATIC HERROR vmx_bc_sc_Negotiate(SCI_HANDLE hSci, SCI_ATR_INFO *pstAtrInfo, SCI_SC_PARAM *pstDefaultParam)
{
	int ulErr;
	HUINT8	ucWI, ucBWI;
	SCI_ATR_INFO *p;
	HERROR nErr = ERR_OK;
	SCI_SC_PARAM stScParam;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	p = pstAtrInfo;

	/* betacrypt : T = 1 Only */
#if 1	
	ucWI = p->ucCWI;
	ucBWI = p->ucBWI;
#else
	if(p->ucT == SCI_T0)
		ucWI = p->ucWI;
	else
		ucWI = p->ucCWI;

	if(p->ucT == SCI_T14 && p->ucBWI < 5)
		ucBWI = 5;
	else
		ucBWI = p->ucBWI;
#endif

	if(p->bSpecificMode)
	{
		VmxBcScData(DBG_DATA, ("[vmx_bc_sc_Negotiate] SPECIFIC\n"));

		if(p->bImplicitlyDefined)
		{
			VmxBcScData(DBG_DATA, ("[vmx_bc_sc_Negotiate] IMPLICIT\n"));

		//	Initiate a warm reset to invoke the negotiable mode
		//	if(SCI_INT_IsFirstWarmReset())
		//	{
		//		;//Initiate warm reset
		//	}
		//	else
			{
				goto Atr_Parameters;
			}

		}
		else
		{
			VmxBcScData(DBG_DATA, ("[vmx_bc_sc_Negotiate] EXPLICIT\n"));
			goto Atr_Parameters;
		}

	}
	else	// negotiable mode
	{
		SCI_SC_PARAM stDefaultParam;

		VmxBcScData(DBG_DATA, ("[vmx_bc_sc_Negotiate] NEGOTIABLE\n"));

		if(p->ucFI == 0 && p->ucDI == 0)
			goto Atr_Parameters;

		VK_memcpy(&stDefaultParam, pstDefaultParam, sizeof(SCI_SC_PARAM));
		stDefaultParam.ucN = p->ucN;

		ulErr = vmx_bc_sc_NegotiateByValueOrder(hSci, p, ucWI, ucBWI, &stDefaultParam);
		if(ulErr)
		{
			goto Default_Parameters;
		}
		else
		{
			return ERR_OK;
		}
	}

Default_Parameters:

	VmxBcScData(DBG_DATA, ("[vmx_bc_sc_Negotiate] SET Default FI DI\n"));

	// Comments :: Conax의 경우 Clock을 typically 3.6Mhz이고, 4.5Mhz보다 적은 clock을 사용해야 함.
	//				4.5Mhz를 쓸 경우 동작하지 않는 card가 있음.
	//	=-> baudrate을 9600으로 설정하면 ST, CNXT DD 모두 3.6Mhz로 clock을 설정함.
	pstDefaultParam->ulBaudrate = 9600;

	nErr = PAL_SC_SetParams(hSci, pstDefaultParam);

	if(nErr)
	{
		HxLOG_Error(DBG_ASSERT, ("[vmx_bc_sc_Negotiate][DI_SC_SetParams] err<%x>\n", nErr);
	}

	return ERR_OK;

Atr_Parameters:

	VmxBcScData(DBG_DATA, ("[vmx_bc_sc_Negotiate] SET ATR FI DI\n"));

	stScParam.eProtocol		= p->ucT;
	stScParam.ulClock		= p->ulMinClock;
	stScParam.ulBaudrate	= p->ulBaudrate;
	stScParam.ucN			= p->ucN;
	stScParam.ucCWI		= p->ucWI;
	stScParam.ucBWI		= p->ucBWI;

	if(p->ucFI == 0 && p->ucDI == 0)
	{
		stScParam.ucFI		= 0x1;
		stScParam.ucDI		= 0x1;
	}
	else
	{
		stScParam.ucFI		= p->ucFI;
		stScParam.ucDI		= p->ucDI;
	}

	nErr = PAL_SC_SetParams(hSci, &stScParam);
	if(nErr)
	{
		HxLOG_Error(DBG_ASSERT, ("[vmx_bc_sc_Negotiate][DI_SC_SetParams] err<%x>\n", nErr));
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;
}
#endif

/*
 * Initialize Smart Card DD
 * Register Callback function that notifies smart card insert/extract
 * Reset card if card presents
 */
STATIC HERROR vmx_bc_sc_Init(void)
{
	int 	ulErr, i;
	HUINT32	ulNumSlot;
	VmxBc_SC_Inst *pBcScInst = NULL;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	vmx_bc_sc_BcInstInit();

	ulNumSlot = PAL_SC_GetNumberOfModule();

	HxLOG_Print(DBG_PRINT, ("ulNumSlot = %d\n", ulNumSlot);

	for (i=0; i < ulNumSlot && i < VMXBC_SC_SLOT_NUM; i++)
	{
		pBcScInst = vmx_bc_GetVmxBcInst(i);
		if(pBcScInst == NULL)
			continue;

		ulErr = PAL_SC_RegisterCbFunc(i, eGroupID_VMXBC, pBcScInst->pfnCallBack, &(pBcScInst->hCardHandle));
		if(ulErr)
		{
			HxLOG_Error(DBG_TRACE, ("[PAL_SC_RegisterCbFunc]Err:%x\n", ulErr);
		}

		HxLOG_Print(DBG_PRINT, ("Callback Register hSci[%d], handle(0x%x)\n",  i, pBcScInst->hCardHandle);
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");

	return ulErr;
}

STATIC HERROR vmx_bc_sc_Reset(HINT32 nScSlot, HUINT8 *pucAtr, HUINT32 *pulAtrLen, HUINT8 *pucProtocol)
{
	int 				ulErr;
	SCI_HANDLE		hSci;

#if !defined(VMXBC_PPS_EXCHANGE)
	SCI_SC_PARAM	stSCParam;
#endif

	HxLOG_Print(DBG_PRINT, ("[Enter] Slot(%d)\n", nScSlot);

	hSci = vmx_bc_sc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VMXBC_SC_INVALID_HANDLE)
	{
		HxLOG_Error(DBG_TRACE, ("wrong handle\n");
		return ERR_FAIL;
	}

	ulErr = PAL_SC_Reset(hSci, pucAtr, pulAtrLen, eSCM_ResetType_Cold, &s_stDefaultAtrParam);
	if(ulErr)
	{
		HxLOG_Error(DBG_TRACE, ("[PAL_SC_Reset]Err:%x\n", ulErr);
		return ulErr;
	}
	else
	{
		HxLOG_Print(DBG_PRINT, ("[PAL_SC_Reset] OK.OK.OK.\n");
	}

#if defined(VMXBC_PPS_EXCHANGE)
	// PPS negotiation
	{
		SCI_ATR_INFO	stAtrInfo;
		SCI_SC_STATUS	eStatus;

		memset(&stAtrInfo, 0, sizeof(SCI_ATR_INFO));

		stAtrInfo.ulProtocol	= s_stDefaultAtrParam.eProtocol;
		stAtrInfo.ulMinClock	= s_stDefaultAtrParam.ulClock;
		stAtrInfo.ulBaudrate	= s_stDefaultAtrParam.ulBaudrate;
		stAtrInfo.ucN			= s_stDefaultAtrParam.ucN;

		stAtrInfo.ucWI			= s_stDefaultAtrParam.ucCWI;
		stAtrInfo.ucBWI			= s_stDefaultAtrParam.ucBWI;

		vmx_bc_sc_ParseAtr(pucAtr, &stAtrInfo);
		vmx_bc_sc_Negotiate(hSci, &stAtrInfo, &s_stDefaultAtrParam);
#if 1
		/*
			work around
			카드가 빠진 상태에서 PPS가 계속 진행되면서 오동작이 되네요..
			initcass() 전에 카드가 정상적으로 있는지 확인해 보죠.
		*/
		PAL_SC_GetCardStatus(hSci, &eStatus);
		if(eStatus != SCI_SC_PRESENT)
		{
			return ERR_FAIL;
		}
#endif
		*pucProtocol = stAtrInfo.ulProtocol;
	}
#else
	*pucProtocol = (unsigned char)stSCParam.eProtocol;
#endif	//End of VMXBC_PPS_EXCHANGE

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;
}

HERROR vmx_bc_sc_Write(HINT32 nScSlot, HUINT8 *pucSndData, HUINT32 ulSndLen, HUINT8 *pucRcvData, HUINT32 *pulNumRead)
{
	unsigned long		ulTotSndLen;
	int				nRet;
	HUINT32			ulReadNum = 0;
#if defined(CONFIG_DEBUG)
	int				i;
#endif
	SCI_HANDLE		hSci;

	HxLOG_Print(DBG_PRINT, ("[Enter] Slot(%d)[%d]\n", nScSlot, ulSndLen);

	hSci = vmx_bc_sc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VMXBC_SC_INVALID_HANDLE)
	{
		HxLOG_Error(DBG_TRACE, ("wrong handle\n");
		return ERR_FAIL;
	}

	ulTotSndLen = ulSndLen;

#if 0//defined(CONFIG_DEBUG)
	xsvc_vmx_main_PrintDump(VMX_DBG_SC, "SC_Write To SC", pucSndData, ulTotSndLen);
#endif

	nRet = PAL_SC_Transfer(hSci, pucSndData, ulTotSndLen, pucRcvData, &ulReadNum, 0);
	if (nRet != ERR_OK)
	{
		HxLOG_Error(DBG_TRACE, ("PAL_SC_Write() Error\n");
		return ERR_FAIL;
	}

	* pulNumRead = ulReadNum;
#if 0//defined(CONFIG_DEBUG)
		xsvc_vmx_main_PrintDump(VMX_DBG_SC, "SC_Read read SC", pucRcvData, ulReadNum);
#endif

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return 0;
}

#if 0
HERROR vmx_bc_sc_Read(HINT32 nScSlot, HUINT8 *pucSndData, HUINT8 *pucRcvData, HUINT32 * pulNumRead)
{
	int				nRet;
#ifdef PRINT_DEBUG_SC_DATA
	int				i;
#endif
	SCI_HANDLE		hSci;

	HxLOG_Print(DBG_PRINT, ("[Enter] Slot(%d,%d)\n", nScSlot, *pulNumRead);

	hSci = vmx_bc_sc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VMXBC_SC_INVALID_HANDLE)
	{
		HxLOG_Error(DBG_TRACE, ("wrong handle\n");
		return ERR_FAIL;
	}

//	nRet = PAL_SC_Read(hSci, pucSndData, T0_COMMAND_LEN, pucRcvData, pulNumRead);
	nRet = PAL_SC_Transfer(hSci, pucSndData, T0_COMMAND_LEN, pucRcvData, pulNumRead, 0);
	if (nRet != ERR_OK)
	{
		HxLOG_Error(DBG_TRACE, ("PAL_SC_Transfer() Error:%x\n", nRet);
		return ERR_FAIL;
	}

#ifdef PRINT_DEBUG_SC_DATA
	VK_Print("Read - Send Data\n[");
	for (i=0; i < T0_COMMAND_LEN; i++)
	{
		if (((i%16) == 0) && (i != 0))
			VK_Print("\n");
		VK_Print("%2x ", pucSndData[i]);
	}
	VK_Print("]\n");
#endif

#ifdef PRINT_DEBUG_SC_DATA
	VK_Print("Read - Receive Data\n[");
	for (i=0; i < *pulNumRead; i++)
	{
		if (((i%16) == 0) && (i != 0))
			VK_Print("\n");
		VK_Print("%2x ", pucRcvData[i]);
	}
	VK_Print("]\n");
#endif

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return 0;
}
#endif

HERROR vmx_bc_sc_ValidCard(HINT32 nScSlot, HBOOL bIsValid, HUINT16 usCaSysId, HUINT8 ucNumSession)
{
	SCI_HANDLE		hSci;

	HxLOG_Print(DBG_PRINT, ("[Enter] Slot(%d)\n", nScSlot);

	hSci = vmx_bc_sc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VMXBC_SC_INVALID_HANDLE)
	{
		HxLOG_Error(DBG_TRACE, ("wrong handle\n");
		return ERR_FAIL;
	}

//	local_cxsc_SetMarryingStatus(nScSlot, 0);
	PAL_SC_NotifyScValidity(hSci, (HBOOL)bIsValid);

	// casMgr에게 알려준다.
	if (bIsValid)
	{
	//	CXACT_UpdateCasInfo((HUINT16)nScSlot, &usCaSysId, (HUINT32)1, (HUINT32)ucNumSession, bIsValid);
	// TODO_VMX
	}
	else
	{
//		CXACT_UpdateCasInfo((HUINT16)nScSlot, NULL, (HUINT32)0, (HUINT32)0, bIsValid);
	// TODO_VMX
	}
	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;
}

HERROR vmx_bc_sc_Deactivate(HINT32 nScSlot)
{
	HERROR 			ulErr;
	SCI_HANDLE		hSci;

	HxLOG_Print(DBG_PRINT, ("[Enter] Slot(%d)\n", nScSlot);

	hSci = vmx_bc_sc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VMXBC_SC_INVALID_HANDLE)
	{
		HxLOG_Error(DBG_ASSERT, ("wrong handle\n");
		return ERR_FAIL;
	}

	ulErr = PAL_SC_Deactivate(hSci);
	if(ulErr)
	{
		HxLOG_Error(DBG_ASSERT, ("[PAL_SC_Deactivate]Err:%x\n", ulErr);
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");

	return ulErr;
}

HERROR xsvc_cas_vmx_bc_sc_Write(HINT32 nScSlot, HUINT8 *pucSndData, HUINT32 ulSndLen, HUINT8 *pucRcvData, HUINT32 *pulNumRead)
{
	return vmx_bc_sc_Write(nScSlot, pucSndData, ulSndLen, pucRcvData, pulNumRead);
}

HERROR xsvc_cas_vmx_bc_sc_Reset(HINT32 nScSlot, HUINT8 *pucAtr, HUINT32 *pulAtrLen, HUINT8 *pucProtocol)
{
	return vmx_bc_sc_Reset(nScSlot, pucAtr, pulAtrLen, pucProtocol);
}

HERROR xsvc_cas_vmx_bc_sc_Init(void)
{
	if(vmx_vr_bc_sc_Init() != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("vmx_vr_bc_sc_Init : error\n");
		return ERR_FAIL;
	}

	return vmx_bc_sc_Init();
}
