/**
	@file     va_sc_adapt.c
	@brief    va_sc_adapt.c (Viaccess SC)

	Description: . smartcard adaptation layer \n
	Module: MW/CAS/ Viaccess \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "va_def.h"

#include "va_sc_api.h"
#include "va_sc_adapt.h"

#include "pal_sc.h"

//#include <_svc_cas_mgr_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

//#define PRINT_DEBUG_SC_DATA

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define VA_PPS_EXCHANGE	// PPS exchange를 여기서 구현한다. ==> sc mgr이나 PAL_SC에서 해주는 것이 맞는 것 같은데...

/*****************************************************************************
							typedef
******************************************************************************/
typedef struct tVA_SC_INST
{
	PalSc_Handle_t 		hCardHandle;
	PalSc_Callback_t		pfnCallBack;

	unsigned char	ucSlotId;
	unsigned char	ucIsCardMarrying;
	unsigned char	dummy1;	// for 4 byte align
	unsigned char	dummy2;
} VA_SC_INST;

#ifdef VA_PPS_EXCHANGE
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

/*****************************************************************************
							external variable declaration
******************************************************************************/

/*****************************************************************************
							global/static variable declaration
******************************************************************************/
static VA_SC_INST	s_VaScInst[VA_SC_SLOT_NUM];

static PalSc_Param_t 			s_stDefaultAtrParam =
{
		ePAL_SC_PROTOCOL_T0,		// protocol
		0,			// Min Clock
		0,			// baudrate
		1,			// FI
		1,			// DI
		0,			// N
		10, 			//WI로 사용 	13,			// CWI
		4,			// BWI
};

#ifdef VA_PPS_EXCHANGE
static HUINT8 s_aucFiOrder[16] =
{	0xFF, 1, 3, 4, 7, 8, 10, 0xFF, 0xFF, 2, 5, 6, 9, 11, 0xFF, 0xFF };

static HUINT8 s_aucDiOrder[16] =
{	0xFF, 1, 2, 3, 4, 6, 8, 0xFF, 5, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static HUINT8 s_aucFiValue[16] =
{	0xFF, 1, 9, 2, 3, 10, 11, 4, 5, 12, 6, 13, 0xFF, 0xFF, 0xFF, 0xFF };

static HUINT8 s_aucDiValue[16] =
{	0xFF, 1, 2, 3, 4, 8, 5, 9, 6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
#endif

/*****************************************************************************
							static function
******************************************************************************/
static void local_vasc_cbCard0(PalSc_Handle_t hSci, HUINT8 ucCardSlotId, PalSc_CardEvent_e nStatus);
static void local_vasc_cbCard1(PalSc_Handle_t hSci, HUINT8 ucCardSlotId, PalSc_CardEvent_e nStatus);

static PalSc_Handle_t local_vasc_GetSCIHandleWithSlotID(HUINT8 ucCardSlotId)
{
	int	i;

	for (i=0; i < VA_SC_SLOT_NUM; i++)
	{
		if (ucCardSlotId == s_VaScInst[i].ucSlotId)
			return s_VaScInst[i].hCardHandle;
	}

	return VA_SC_INVALID_HANDLE;
}

static int local_vasc_GetIndexWithSlotID(HUINT8 ucCardSlotId)
{
	int	idx;

	for (idx=0; idx < VA_SC_INVALID_HANDLE; idx++)
	{
		if (ucCardSlotId == s_VaScInst[idx].ucSlotId)
			return idx;
	}

	return -1;
}

static void local_vasc_Init()
{
	int i;

	for (i=0; i < VA_SC_SLOT_NUM; i++)
	{
		s_VaScInst[i].hCardHandle = VA_SC_INVALID_HANDLE;
		s_VaScInst[i].ucSlotId = VA_SC_INVALID_SLOT;

		if (i == 0)
			s_VaScInst[i].pfnCallBack = local_vasc_cbCard0;
#if (VA_SC_SLOT_NUM > 1)
		else
			s_VaScInst[i].pfnCallBack = local_vasc_cbCard1;
#endif
	}
}

static void local_vasc_cbCard0(PalSc_Handle_t hSci, HUINT8 ucCardSlotId, PalSc_CardEvent_e nStatus)
{
	int				idx;
	HUINT32			msg[4];

	HxLOG_Print("[Enter] Handle(0x%x) Slot(%d) Status(%d) \n", hSci, ucCardSlotId, nStatus);

	// sc_mgr에서 관리하는 handle과 slot ID를 mapping 시킨다.
	// CAS Module에서 read/write 요구시에 validity를 보장하기 위해서...
	for (idx=0; idx < VA_SC_SLOT_NUM; idx++)
	{
		if (s_VaScInst[idx].hCardHandle == hSci)
		{
			if (nStatus == ePAL_SC_CARDEVENT_EXTRACTED)
			{
				s_VaScInst[idx].ucSlotId = VA_SC_INVALID_SLOT;

				// Smart Card Task로 slot ID를 전송한다.
				msg[0] = VA_SC_MSG_CARD_OUT;
				msg[1] = ucCardSlotId;
				VA_PostMessage(VA_SC_GetMsgQueueId(), msg);

				#if 0 // hmkim : commented out (카드가 이미 제거된 상황이기 때문에 svc_cas_DevScDeactivate()을 호출하는 것은 의미가 없고 DI 쪽에 에러만 발생시킴)
				svc_cas_DevScDeactivate(hSci);
				#endif
			}
			else if (nStatus == ePAL_SC_CARDEVENT_INSERTED)
			{
				s_VaScInst[idx].ucSlotId = ucCardSlotId;

				msg[0] = VA_SC_MSG_CARD_IN;
				msg[1] = ucCardSlotId;
				VA_PostMessage(VA_SC_GetMsgQueueId(), msg);
			}
			break;
		}
	}

	HxLOG_Print("[Exit]\n");
}

#if (VA_SC_SLOT_NUM > 1)
static void local_vasc_cbCard1(PalSc_Handle_t hSci, HUINT8 ucCardSlotId, PalSc_CardEvent_e nStatus)
{
	int				idx;
	HUINT32			msg[4];

	HxLOG_Print("[Enter] Handle(0x%x), Slot(%d)\n", hSci, ucCardSlotId);

	// sc_mgr에서 관리하는 handle과 slot ID를 mapping 시킨다.
	// CAS Module에서 read/write 요구시에 validity를 보장하기 위해서...
	for (idx=0; idx < VA_SC_SLOT_NUM; idx++)
	{
		if (s_VaScInst[idx].hCardHandle == hSci)
		{
			if (nStatus == ePAL_SC_CARDEVENT_EXTRACTED)
			{
				s_VaScInst[idx].ucSlotId = VA_SC_INVALID_SLOT;

				// Smart Card Task로 slot ID를 전송한다.
				msg[0] = VA_SC_MSG_CARD_OUT;
				msg[1] = ucCardSlotId;
				VA_PostMessage(VA_SC_GetMsgQueueId(), msg);

				#if 0 // hmkim : commented out (카드가 이미 제거된 상황이기 때문에 svc_cas_DevScDeactivate()을 호출하는 것은 의미가 없고 DI 쪽에 에러만 발생시킴)
				svc_cas_DevScDeactivate(hSci);
				#endif
			}
			else if (nStatus == ePAL_SC_CARDEVENT_INSERTED)
			{
				s_VaScInst[idx].ucSlotId = ucCardSlotId;

				msg[0] = VA_SC_MSG_CARD_IN;
				msg[1] = ucCardSlotId;
				VA_PostMessage(VA_SC_GetMsgQueueId(), msg);
			}
			break;
		}
	}

	HxLOG_Print("[Exit]\n");
}
#endif

#ifdef VA_PPS_EXCHANGE
// PPS
HERROR local_vasc_ParseAtr(HUINT8 *pucAtr, SCI_ATR_INFO *pstAtrInfo)
{
	HUINT8	*pucP;
	HUINT8	Y;
	HUINT8	index = 1;

	pucP = pucAtr;		// TS
	HxLOG_Info("TS\t<%02x> (%d)\n", *pucP, *pucP);

	Y = *++pucP;		// T0
	HxLOG_Info("T0\t<%02x> (%d)\n", *pucP, *pucP);

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
			HxLOG_Info("TA%d\t<%02x> (%d)\n", index, *pucP, *pucP);
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
			HxLOG_Info("TB%d\t<%02x> (%d)\n", index, *pucP, *pucP);
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
			HxLOG_Info("TC%d\t<%02x> (%d)\n", index, *pucP, *pucP);
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
			HxLOG_Info("TD%d\t<%02x> (%d)\n", index, *pucP, *pucP);
		}
		else
		{
			Y = 0;
		}
		index++;
	}

	return ERR_OK;
}

HERROR local_vasc_RequestPps(PalSc_Handle_t hSci, HUINT8 ucT, HUINT8 ucFi, HUINT8 ucDi, PalSc_Param_t *pstDefaultParam, HUINT8 *pucPpsValue)
{
	int ulErr = ERR_OK;
	HUINT8	aPpsRequest[4];
	HUINT8	aPpsResponse[4] = {0,0,0,0};
	HUINT8	ucNumRead;
	HERROR nErr = ERR_OK;

	HxLOG_Print("[++local_vasc_RequestPps]\n");

	nErr = svc_cas_DevScSetParams(hSci, pstDefaultParam);
	if(nErr)
		HxLOG_Error("svc_cas_DevScSetParams() err<%x>\n", ulErr);

	aPpsRequest[0] = 0xFF;
	aPpsRequest[1] = 0x10 | (ucT & 0x0F);
	aPpsRequest[2] = ( (ucFi << 4) & 0xF0 ) | (ucDi & 0x0F );
	aPpsRequest[3] = aPpsRequest[0] ^ aPpsRequest[1] ^ aPpsRequest[2];

	nErr = svc_cas_DevScNegotiatePTS(hSci, aPpsRequest, 4, aPpsResponse, &ucNumRead);
	if(nErr)
		HxLOG_Error("svc_cas_DevScNegotiatePTS() err<%x>\n", ulErr);

	HxLOG_Info("Req : %x, %x, %x, %x\n", aPpsRequest[0], aPpsRequest[1], aPpsRequest[2], aPpsRequest[3]);
	HxLOG_Info("Res : %x, %x, %x, %x\n", aPpsResponse[0], aPpsResponse[1], aPpsResponse[2], aPpsResponse[3]);

	// skk
	// (1) response is same with request
	// (2) reponse(1-4) is same with request (1-4) and response(5-7) set to 0
//	if(VA_memcmp(aPpsRequest, aPpsResponse, 4))
	if (!((aPpsRequest[1] == aPpsResponse[1]) && (aPpsRequest[2] == aPpsResponse[2])) &&
		!((aPpsResponse[1] < 0x10) && ((aPpsRequest[1] & 0xF) == (aPpsResponse[1] & 0xF))))
	{
		HxLOG_Error("reject \n");
		return ERR_FAIL;
	}

	if ((aPpsResponse[1] & 0x10) == 0)
	{
		*pucPpsValue = 0x11; //default Fd, Dd
	}
	else
	{
		*pucPpsValue = aPpsResponse[2];
	}

	HxLOG_Print("[--local_vasc_RequestPps(pps:%x)]\n", *pucPpsValue);
	return ERR_OK;

}

HERROR local_vasc_NegotiateByValueOrder(PalSc_Handle_t hSci, SCI_ATR_INFO *pstAtrInfo, HUINT8 ucWI, HUINT8 ucBWI, PalSc_Param_t *pstDefaultParam, HUINT8 ucPpsMax, HUINT8 *pucPpsValue)
{
	int ulErr;
	HUINT8	ucT;
	HUINT8	ucF, ucD;
	HUINT8	ucFIMax, ucDIMax;
	HUINT8	ucCntF, ucCntD;
	SCI_ATR_INFO *p;
	HERROR nErr = ERR_OK;
	PalSc_Param_t stScParam;
	int i;

	p = pstAtrInfo;

//skk 2009-03-15 pps value와 ATR의 FD 값을 비교하여 더 작은 값으로 nego 시작...!!
#if 1
	ucFIMax = (ucPpsMax & 0xF0) >> 4;
	ucDIMax = ucPpsMax & 0x0F;

	ucF = (s_aucFiOrder[p->ucFI] > s_aucFiOrder[ucFIMax]) ? ucFIMax : p->ucFI;
	ucD = (s_aucDiOrder[p->ucDI] > s_aucDiOrder[ucDIMax]) ? ucDIMax : p->ucDI;	//1 ucD = ucDIMax;
#else
	ucF = p->ucFI;
	ucD = p->ucDI;
#endif

	ucCntF = s_aucFiOrder[ucF];
	ucCntD = s_aucDiOrder[ucD];

	HxLOG_Info("[T=%x, %x]\n", p->ucT, p->ucTFirst);
	if(p->ucT == p->ucTFirst)
	{
		ucT = p->ucT;
		HxLOG_Info("UNIQUE T\n");
		while(ucCntF >= 1)		// ucCntF try
		{
			ucF = s_aucFiValue[ucCntF];
			while(ucCntD >= 1)	// ucCntD try
			{
				ucD = s_aucDiValue[ucCntD];
				HxLOG_Info("PPS T(%d) F(%d) D(%d)\n", ucT, ucF, ucD);
				ulErr = local_vasc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam, pucPpsValue);
				if(ulErr == ERR_OK)
				{
					ucF = *pucPpsValue >> 4;
					ucD = *pucPpsValue & 0xF;

					stScParam.eProtocol = ucT;
					stScParam.ulClock= p->ulMinClock;
					stScParam.ulBaudrate = p->ulBaudrate;
					stScParam.ucFI = ucF;
					stScParam.ucDI =ucD;
					stScParam.ucN = p->ucN;
					stScParam.ucCWI = ucWI;
					stScParam.ucBWI = ucBWI;

					nErr = svc_cas_DevScSetParams(hSci, &stScParam);
					if(nErr)
						HxLOG_Error("svc_cas_DevScSetParams() err<%x>\n", ulErr);
					HxLOG_Info("T(%d) F(%d) D(%d), pps(%x)", ucT, ucF, ucD, *pucPpsValue);
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
		HxLOG_Info("MULTIPLE T\n");
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
					HxLOG_Info("PPS T(%d) F(%d) D(%d)\n", ucT, ucF, ucD);
					ulErr = local_vasc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam, pucPpsValue);
					if(ulErr == ERR_OK)
					{
						ucF = *pucPpsValue >> 4;
						ucD = *pucPpsValue & 0xF;

						stScParam.eProtocol = ucT;
						stScParam.ulClock= p->ulMinClock;
						stScParam.ulBaudrate = p->ulBaudrate;
						stScParam.ucFI = ucF;
						stScParam.ucDI =ucD;
						stScParam.ucN = p->ucN;
						stScParam.ucCWI = ucWI;
						stScParam.ucBWI = ucBWI;

						nErr = svc_cas_DevScSetParams(hSci, &stScParam);
						if(nErr)
							HxLOG_Error("svc_cas_DevScSetParams() err<%x>\n", ulErr);
						HxLOG_Info("T(%d) F(%d) D(%d), pps(%x)", ucT, ucF, ucD, *pucPpsValue);
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

HERROR local_vasc_Negotiate(PalSc_Handle_t hSci, SCI_ATR_INFO *pstAtrInfo, PalSc_Param_t *pstDefaultParam, HUINT8 ucPpsMax, HUINT8 *pucPpsValue)
{
	int ulErr;
	HUINT8	ucWI, ucBWI;
	SCI_ATR_INFO *p;
	HERROR nErr = ERR_OK;
	PalSc_Param_t stScParam;

	HxLOG_Print("[++local_vasc_Negotiate]\n");

	p = pstAtrInfo;

	/* only Viaccess T0 */
#if 1
	ucWI = p->ucWI;
	ucBWI = p->ucBWI;

	if(ucPpsMax == 0)
	{
		p->ucDI = 1;
		p->ucN += 2;
		goto Atr_Parameters;
	}

#else
	if(p->ucT == ePAL_SC_PROTOCOL_T0)
		ucWI = p->ucWI;
	else
		ucWI = p->ucCWI;

	if(p->ucT == ePAL_SC_PROTOCOL_T14 && p->ucBWI < 5)
		ucBWI = 5;
	else
		ucBWI = p->ucBWI;
#endif

	if(p->bSpecificMode)
	{
		HxLOG_Info("SPECIFIC\n");

		if(p->bImplicitlyDefined)
		{
			HxLOG_Info("IMPLICIT\n");

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
			HxLOG_Info("EXPLICIT\n");
			goto Atr_Parameters;
		}

	}
	else	// negotiable mode
	{
		HxLOG_Info("NEGOTIABLE\n");

		if(p->ucFI == 1 && p->ucDI == 1)
			goto Atr_Parameters;

		ulErr = local_vasc_NegotiateByValueOrder(hSci, p, ucWI, ucBWI, pstDefaultParam, ucPpsMax, pucPpsValue);
		if(ulErr)
			goto Default_Parameters;
		else
			return ERR_OK;
	}

Default_Parameters:

	HxLOG_Info("SET Default FI DI\n");

	// Comments :: Conax의 경우 Clock을 typically 3.6Mhz이고, 4.5Mhz보다 적은 clock을 사용해야 함.
	//				4.5Mhz를 쓸 경우 동작하지 않는 card가 있음.
	//	=-> baudrate을 9600으로 설정하면 ST, CNXT DD 모두 3.6Mhz로 clock을 설정함.
	pstDefaultParam->ulBaudrate = 9600;

	nErr = svc_cas_DevScSetParams(hSci, pstDefaultParam);
	if(nErr)
		HxLOG_Error("svc_cas_DevScSetParams() err<%x>\n", ulErr);

	return ERR_OK;

Atr_Parameters:

	HxLOG_Info("SET ATR FI DI\n");

	*pucPpsValue = (p->ucFI << 4) | p->ucDI;

	stScParam.eProtocol = p->ucT;
	stScParam.ulClock= p->ulMinClock;
	stScParam.ulBaudrate = p->ulBaudrate;
	stScParam.ucFI = p->ucFI;
	stScParam.ucDI = p->ucDI;
	stScParam.ucN = p->ucN;
	stScParam.ucCWI = p->ucWI;
	stScParam.ucBWI = p->ucBWI;

	nErr = svc_cas_DevScSetParams(hSci, &stScParam);
	if(nErr)
		HxLOG_Error("svc_cas_DevScSetParams() err<%x>\n", nErr);

	HxLOG_Print("[--local_vasc_Negotiate]\n");

	return ERR_OK;
}
#endif


/*****************************************************************************
							global function body
******************************************************************************/
/*
 * Initialize Smart Card DD
 * Register Callback function that notifies smart card insert/extract
 * Reset card if card presents
 */
HERROR VA_SC_Adapt_Init()
{
	int 	ulErr = 0, i;
	HUINT32	ulNumSlot;

	HxLOG_Print("[Enter]\n");

	// Initialize Internal(CAS Module에서 사용하는) data
	local_vasc_Init();

#if 0 // hmkim : commented out (OBAMA_Init() 에서 이미 호출됨)
	ulErr = svc_cas_DevScInit();
	if(ulErr)
		HxLOG_Error("svc_cas_DevScInit() Err:%x\n", ulErr);
#endif

	ulNumSlot = svc_cas_DevScGetNumberOfModule();

	for (i = 0; i < ulNumSlot && i < VA_SC_SLOT_NUM; i++)
	{
		ulErr = svc_cas_DevScRegisterCbFunc(i, eDxCAS_GROUPID_VA, s_VaScInst[i].pfnCallBack, &(s_VaScInst[i].hCardHandle));
		if(ulErr)
			HxLOG_Error("svc_cas_DevScRegisterCbFunc() Err:%x\n", ulErr);

		HxLOG_Info("hSci[%d], handle <%x>\n", i, s_VaScInst[i].hCardHandle);
	}

	HxLOG_Print("[Exit]\n");
	return ulErr;
}

HERROR VA_SC_Adapt_Reset(HINT32 nScSlot, HUINT8 *pucAtr, HUINT32 *pulAtrLen, HUINT8 *pucProtocol, HUINT8 ucPpsMax, HUINT8 *pucPpsData)
{
	int 				ulErr;
	PalSc_Handle_t		hSci;
#ifndef VA_PPS_EXCHANGE
	PalSc_Param_t	stSCParam;
#endif

	HxLOG_Print("[Enter] Slot(%d), pps(%x)\n", nScSlot, ucPpsMax);

	hSci = local_vasc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VA_SC_INVALID_HANDLE)
	{
		HxLOG_Error("wrong handle\n");
		return ERR_FAIL;
	}

	ulErr = svc_cas_DevScReset(hSci, pucAtr, pulAtrLen, ePAL_SC_RESETTYPE_COLD, &s_stDefaultAtrParam);
	if(ulErr)
	{
		HxLOG_Info("svc_cas_DevScReset() Err:%x\n", ulErr);
		return ulErr;
	}

#ifdef VA_PPS_EXCHANGE
	// PPS negotiation
	{
		SCI_ATR_INFO	stAtrInfo;

		memset(&stAtrInfo, 0, sizeof(SCI_ATR_INFO));

		stAtrInfo.ulProtocol 	= s_stDefaultAtrParam.eProtocol;
		stAtrInfo.ulMinClock 	= s_stDefaultAtrParam.ulClock;
		stAtrInfo.ulBaudrate 	= s_stDefaultAtrParam.ulBaudrate;
		stAtrInfo.ucN 			= s_stDefaultAtrParam.ucN;

		stAtrInfo.ucWI 		= s_stDefaultAtrParam.ucCWI;
		stAtrInfo.ucBWI 		= s_stDefaultAtrParam.ucBWI;

		stAtrInfo.ucFI 			= 0x1;
		stAtrInfo.ucDI 		= 0x1;

		local_vasc_ParseAtr(pucAtr, &stAtrInfo);

		// ucPpsMax > 0 일경우에만 PPS 진행
		if (ucPpsMax)
		local_vasc_Negotiate(hSci, &stAtrInfo, &s_stDefaultAtrParam, ucPpsMax, pucPpsData);

		*pucProtocol = stAtrInfo.ulProtocol;
	}
#else
	*pucProtocol = (unsigned char)stSCParam.eProtocol;
#endif	//End of VA_PPS_EXCHANGE

	HxLOG_Print("[Exit(pps result:%x]\n", *pucPpsData);
	return 0;
}


HERROR VA_SC_Adapt_Write(HINT32 nScSlot, HUINT8 *pucSndData, HUINT32 ulSndLen, HUINT8 *pucRcvData, HUINT32 *pulNumRead)
{
	unsigned long		ulTotSndLen;
	int				nRet;
#ifdef PRINT_DEBUG_SC_DATA
	int				i;
#endif
	PalSc_Handle_t		hSci;

	HxLOG_Print("[Enter] Slot(%d)[%d/%d]\n", nScSlot, ulSndLen, *pulNumRead);

	hSci = local_vasc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VA_SC_INVALID_HANDLE)
	{
		HxLOG_Error("wrong handle\n");
		return ERR_FAIL;
	}

	ulTotSndLen = ulSndLen; //T0_COMMAND_LEN + pucSndData[4];

#ifdef PRINT_DEBUG_SC_DATA
	HxLOG_Print("Write - Send Data\n[");
	for (i=0; i < ulTotSndLen; i++)
	{
		if (((i%16) == 0) && (i != 0))
			HxLOG_Print("\n");
		HxLOG_Print("%2x ", pucSndData[i]);
	}
	HxLOG_Print("]\n");
#endif

//	nRet = svc_cas_DevScWrite(hSci, pucSndData, ulTotSndLen, pucRcvData, pulNumRead);
	nRet = svc_cas_DevScTransfer(hSci, pucSndData, ulTotSndLen, pucRcvData, pulNumRead, 0);
	if (nRet != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevScTransfer() Error:%x\n", nRet);
		return ERR_FAIL;
	}

#ifdef PRINT_DEBUG_SC_DATA
	HxLOG_Print("Write - Receive Data\n[");
	for (i=0; i < *pulNumRead; i++)
	{
		if (((i%16) == 0) && (i != 0))
			HxLOG_Print("\n");
		HxLOG_Print("%2x ", pucRcvData[i]);
	}
	HxLOG_Print("]\n");
#endif

	HxLOG_Print("[Exit]\n");
	return 0;
}


HERROR VA_SC_Adapt_Read(HINT32 nScSlot, HUINT8 *pucSndData, HUINT8 *pucRcvData, HUINT32 * pulNumRead)
{
	int				nRet;
#ifdef PRINT_DEBUG_SC_DATA
	int				i;
#endif
	PalSc_Handle_t		hSci;

	HxLOG_Print("[Enter] Slot(%d,%d)\n", nScSlot, *pulNumRead);

	hSci = local_vasc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VA_SC_INVALID_HANDLE)
	{
		HxLOG_Error("wrong handle\n");
		return ERR_FAIL;
	}

//	nRet = svc_cas_DevScRead(hSci, pucSndData, T0_COMMAND_LEN, pucRcvData, pulNumRead);
	nRet = svc_cas_DevScTransfer(hSci, pucSndData, T0_COMMAND_LEN, pucRcvData, pulNumRead, 0);
	if (nRet != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevScTransfer() Error:%x\n", nRet);
		return ERR_FAIL;
	}

#ifdef PRINT_DEBUG_SC_DATA
	HxLOG_Print("Read - Send Data\n[");
	for (i=0; i < T0_COMMAND_LEN; i++)
	{
		if (((i%16) == 0) && (i != 0))
			HxLOG_Print("\n");
		HxLOG_Print("%2x ", pucSndData[i]);
	}
	HxLOG_Print("]\n");
#endif

#ifdef PRINT_DEBUG_SC_DATA
	HxLOG_Print("Read - Receive Data\n[");
	for (i=0; i < *pulNumRead; i++)
	{
		if (((i%16) == 0) && (i != 0))
			HxLOG_Print("\n");
		HxLOG_Print("%2x ", pucRcvData[i]);
	}
	HxLOG_Print("]\n");
#endif

	HxLOG_Print("[Exit]\n");
	return 0;
}

HERROR VA_SC_Adapt_ValidCard(HINT32 nScSlot, HBOOL bIsValid, HUINT16 usCaSysId, HUINT8 ucNumSession)
{
	PalSc_Handle_t		hSci;

	HxLOG_Print("[Enter] Slot(%d)\n", nScSlot);

	hSci = local_vasc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VA_SC_INVALID_HANDLE)
	{
		HxLOG_Error("wrong handle\n");
		return ERR_FAIL;
	}

//	local_vasc_SetMarryingStatus(nScSlot, 0);
	svc_cas_DevScNotifyScValidity(hSci, (HBOOL)bIsValid);

	// casMgr에게 알려준다.

	if (bIsValid)
		VAACT_UpdateCasInfo((HUINT16)nScSlot, &usCaSysId, (HUINT32)1, (HUINT32)ucNumSession, bIsValid);
	else
		VAACT_UpdateCasInfo((HUINT16)nScSlot, NULL, (HUINT32)0, (HUINT32)0, bIsValid);

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST
int VA_SC_Adapt_Activate(HINT32 nScSlot)
{
	HERROR 			nErr;
	PalSc_Handle_t		hSci;

	HxLOG_Print("[Enter] Slot(%d)\n", nScSlot);

	hSci = local_vasc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VA_SC_INVALID_HANDLE)
	{
		HxLOG_Error("wrong handle\n");
		return ERR_FAIL;
	}

	nErr = svc_cas_DevScActivate(hSci);
	if(nErr)
		HxLOG_Error("svc_cas_DevScActivate() Err:%x\n", nErr);

	HxLOG_Print("[Exit]\n");
	return nErr;
}
#endif

HERROR VA_SC_Adapt_Deactivate(HINT32 nScSlot)
{
	HERROR 			nErr;
	PalSc_Handle_t		hSci;

	HxLOG_Print("[Enter] Slot(%d)\n", nScSlot);

	hSci = local_vasc_GetSCIHandleWithSlotID(nScSlot);
	if (hSci == VA_SC_INVALID_HANDLE)
	{
		HxLOG_Error("wrong handle\n");
		return ERR_FAIL;
	}

	nErr = svc_cas_DevScDeactivate(hSci);
	if(nErr)
		HxLOG_Error("svc_cas_DevScDeactivate() Err:%x\n", nErr);

	HxLOG_Print("[Exit]\n");

	return nErr;
}



