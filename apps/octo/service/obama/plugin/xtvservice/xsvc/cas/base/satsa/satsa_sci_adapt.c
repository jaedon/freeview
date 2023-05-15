/* $Header:   //BASE/archives/Group_MHP/Premium_MHP/mw/cas/satsa/satsa_sci/satsa_sci_adapt.c-arc   1.5   Sep 16 2007 23:37:56   chlim  $ */
/******************************************************************************
 * $Workfile:   satsa_sci_adapt.c  $
 * $Modtime:   Sep 16 2007 23:30:44  $
 * Author : ChangHyun Lim(POOH)
 * description : SATSA interface
 *
 * Copyright (c) 2006 by Humax Co., Ltd.
 * All right reserved
 ******************************************************************************/

/*****************************************************************************
							include file
******************************************************************************/
#include "string.h"
#include <octo_common.h>
#include <svc_cas.h>
#include "_svc_cas_err.h"
#include "satsa_error.h"

#include "satsa_sci.h"
#include "satsa_sci_adapt.h"
#include "satsa_ui_adapt.h"

#include "satsa_sci_protocol_t1.h"

#include <_svc_cas_mgr.h>
#include <_svc_cas_mgr_common.h>

/**************************************************************************************************/
#define ________SATSA_SCI_Adapt_Golbal_Value________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________SATSA_SCI_adapt_Private_Define_______________________________________________________
/**************************************************************************************************/




const unsigned char SLOT_READER_INDEX[3] = "0H";

/*****************************************************************************
							typedef
******************************************************************************/

/*****************************************************************************
							external variable declaration
******************************************************************************/

/*****************************************************************************
							global/static variable declaration
******************************************************************************/

static SATSA_SC_MGR_INST		s_satsa_scmgrInst[MAX_SATSA_NUM_SC];
//static PalSc_Param_t		satsa_param_buf;
#define SATSA_PPS_EXCHANGE

SATSA_SCARD_CONTROL_BLOCK        scardControlBlock[SATSA_SCARD_MAX_DEVICES];
#if 1
static PalSc_Param_t 			s_stDefaultAtrParam =
{
		ePAL_SC_PROTOCOL_T0,		// protocol
		0,			// Min Clock
		0,			// baudrate
		1,			// FI
		1,			// DI
		0,			// N
		13,			// CWI
		4,			// BWI
};
#endif

#ifdef SATSA_PPS_EXCHANGE
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
							static function prototype
******************************************************************************/

/*****************************************************************************
							static function body
******************************************************************************/

#ifdef SATSA_PPS_EXCHANGE
// PPS
HERROR local_Satsa_SC_ParseAtr(HUINT8 *pucAtr, SCI_ATR_INFO *pstAtrInfo)
{
	HUINT8	*pucP;
	HUINT8	Y;
	HUINT8	index = 1;

	pucP = pucAtr;		// TS
	HxLOG_Print("TS\t<%02x> (%d)\n", *pucP, *pucP);

	Y = *++pucP;		// T0
	HxLOG_Print("T0\t<%02x> (%d)\n", *pucP, *pucP);

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
			HxLOG_Print("TA%d\t<%02x> (%d)\n", index, *pucP, *pucP);
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
			HxLOG_Print("TB%d\t<%02x> (%d)\n", index, *pucP, *pucP);
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
			HxLOG_Print("TC%d\t<%02x> (%d)\n", index, *pucP, *pucP);
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

HERROR local_Satsa_SC_RequestPps(PalSc_Handle_t hSci, HUINT8 ucT, HUINT8 ucFi, HUINT8 ucDi, PalSc_Param_t *pstDefaultParam)
{
	int ulErr = ERR_OK;
	HUINT8	aPpsRequest[4];
	HUINT8	aPpsResponse[4] = {0,0,0,0};
	HUINT8	ucNumRead;
	HERROR nErr = ERR_OK;

	HxLOG_Print("[++local_Satsa_SC_RequestPps]\n");

	nErr = svc_cas_DevScSetParams(hSci, pstDefaultParam);

	if(nErr)
		HxLOG_Error("[local_Satsa_SC_RequestPps][DI_SC_SetParams] err<%x>\n", ulErr);

	aPpsRequest[0] = 0xFF;
	aPpsRequest[1] = 0x10 | (ucT & 0x0F);
	aPpsRequest[2] = ( (ucFi << 4) & 0xF0 ) | (ucDi & 0x0F );
	aPpsRequest[3] = aPpsRequest[0] ^ aPpsRequest[1] ^ aPpsRequest[2];

	nErr = svc_cas_DevScNegotiatePTS(hSci, aPpsRequest, 4, aPpsResponse, &ucNumRead);
	if(nErr)
		HxLOG_Error("[SCI_INT_RequestPps][DDI_SC_NegotiatePTS] err<%x>\n", ulErr);

	HxLOG_Print("Req : %x, %x, %x, %x\n", aPpsRequest[0], aPpsRequest[1], aPpsRequest[2], aPpsRequest[3]);
	HxLOG_Print("Res : %x, %x, %x, %x (%d)\n", aPpsResponse[0], aPpsResponse[1], aPpsResponse[2], aPpsResponse[3], ucNumRead);


	if( (aPpsResponse[0] | aPpsResponse[1] | aPpsResponse[2] | aPpsResponse[3]) == 0 )
	{
		HxLOG_Error("[local_Satsa_SC_RequestPps][DI_SC_NegotiatePTS] err<%x>\n", ulErr);
		return ERR_FAIL;
	}

	// skk
	// (1) response is same with request
	// (2) reponse(1-4) is same with request (1-4) and response(5-7) set to 0
//	if(memcmp(aPpsRequest, aPpsResponse, 4))
	if (!((aPpsRequest[1] == aPpsResponse[1]) && (aPpsRequest[2] == aPpsResponse[2])) &&
		!((aPpsResponse[1] < 0x10) && ((aPpsRequest[1] & 0xF) == (aPpsResponse[1] & 0xF))))
	{
		HxLOG_Error("[local_Satsa_SC_RequestPps] reject \n");
		return ERR_FAIL;
	}

	HxLOG_Print("[--local_Satsa_SC_RequestPps]\n");
	return ERR_OK;

}

HERROR local_Satsa_SC_NegotiateByValueOrder(PalSc_Handle_t hSci, SCI_ATR_INFO *pstAtrInfo, HUINT8 ucWI, HUINT8 ucBWI, PalSc_Param_t *pstDefaultParam)
{
	int ulErr;
	HUINT8	ucT;
	HUINT8	ucF, ucD;
	HUINT8	ucCntF, ucCntD;
	SCI_ATR_INFO *p;
	HERROR nErr = ERR_OK;
	PalSc_Param_t stScParam;
	int i;

	p = pstAtrInfo;

	ucF = p->ucFI;
	ucD = p->ucDI;

	ucCntF = s_aucFiOrder[ucF];
	ucCntD = s_aucDiOrder[ucD];

	HxLOG_Print("[T=%x, %x]\n", p->ucT, p->ucTFirst);
	if(p->ucT == p->ucTFirst)
	{
		ucT = p->ucT;
		HxLOG_Print("[local_Satsa_SC_NegotiateByValueOrder] UNIQUE T\n");
		while(ucCntF >= 1)		// ucCntF try
		{
			ucF = s_aucFiValue[ucCntF];
			while(ucCntD >= 1)	// ucCntD try
			{
				ucD = s_aucDiValue[ucCntD];
				HxLOG_Print("[local_Satsa_SC_NegotiateByValueOrder] PPS T(%d) F(%d) D(%d)\n", ucT, ucF, ucD);
				ulErr = local_Satsa_SC_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
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

					nErr = svc_cas_DevScSetParams(hSci, &stScParam);
					if(nErr)
						HxLOG_Error("[local_Satsa_SC_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", ulErr);
					HxLOG_Print("[local_Satsa_SC_NegotiateByValueOrder ]T(%d) F(%d) D(%d)", ucT, ucF, ucD);
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
		HxLOG_Print("[local_Satsa_SC_NegotiateByValueOrder] MULTIPLE T\n");
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
					HxLOG_Print("[local_Satsa_SC_NegotiateByValueOrder] PPS T(%d) F(%d) D(%d)\n", ucT, ucF, ucD);
					ulErr = local_Satsa_SC_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
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

						nErr = svc_cas_DevScSetParams(hSci, &stScParam);
						if(nErr)
							HxLOG_Error("[local_Satsa_SC_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", ulErr);
						HxLOG_Print("[local_Satsa_SC_NegotiateByValueOrder] T(%d) F(%d) D(%d)", ucT, ucF, ucD);
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

HERROR local_Satsa_SC_Negotiate(PalSc_Handle_t hSci, SCI_ATR_INFO *pstAtrInfo, PalSc_Param_t *pstDefaultParam)
{
	int ulErr;
	HUINT8	ucWI, ucBWI;
	SCI_ATR_INFO *p;
	HERROR nErr = ERR_OK;
	PalSc_Param_t stScParam;

	HxLOG_Print("[++local_Satsa_SC_Negotiate]\n");

	p = pstAtrInfo;

	if(p->ucT == ePAL_SC_PROTOCOL_T0)
		ucWI = p->ucWI;
	else
		ucWI = p->ucCWI;

	if(p->ucT == ePAL_SC_PROTOCOL_T14 && p->ucBWI < 5)
		ucBWI = 5;
	else
		ucBWI = p->ucBWI;

	if(p->bSpecificMode)
	{
		HxLOG_Print("[local_Satsa_SC_Negotiate] SPECIFIC\n");

		if(p->bImplicitlyDefined)
		{
			HxLOG_Print("[local_Satsa_SC_Negotiate] IMPLICIT\n");

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
			HxLOG_Print("[local_Satsa_SC_Negotiate] EXPLICIT\n");
			goto Atr_Parameters;
		}

	}
	else	// negotiable mode
	{
		HxLOG_Print("[local_Satsa_SC_Negotiate] NEGOTIABLE\n");

		if(p->ucFI == 1 && p->ucDI == 1)
			goto Atr_Parameters;

		ulErr = local_Satsa_SC_NegotiateByValueOrder(hSci, p, ucWI, ucBWI, pstDefaultParam);
		if(ulErr)
			goto Default_Parameters;
		else
			return ERR_OK;
	}

Default_Parameters:

	HxLOG_Print("[local_Satsa_SC_Negotiate] SET Default FI DI\n");

	// Comments :: Conax의 경우 Clock을 typically 3.6Mhz이고, 4.5Mhz보다 적은 clock을 사용해야 함.
	//				4.5Mhz를 쓸 경우 동작하지 않는 card가 있음.
	//	=-> baudrate을 9600으로 설정하면 ST, CNXT DD 모두 3.6Mhz로 clock을 설정함.
	pstDefaultParam->ulBaudrate = 9600;

	nErr = svc_cas_DevScSetParams(hSci, pstDefaultParam);

	if(nErr)
		HxLOG_Error("[local_Satsa_SC_Negotiate][DI_SC_SetParams] err<%x>\n", ulErr);

	return ERR_OK;

Atr_Parameters:

	HxLOG_Print("[local_Satsa_SC_Negotiate] SET ATR FI DI\n");

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
		HxLOG_Error("[local_Satsa_SC_Negotiate][DI_SC_SetParams] err<%x>\n", nErr);

	HxLOG_Print("[--local_Satsa_SC_Negotiate]\n");

	return ERR_OK;
}
#endif

static void local_Satsa_SciAdapt_cbCard0(  PalSc_Handle_t hSci, HUINT8 ucCardSlotId, PalSc_CardEvent_e nStatus )
{
	SATSA_MSG msg;
//	HUINT32 	res;
	int errRet;
	int 		idx;
	//HUINT32			msg[4];

	HxLOG_Print("[+Satsa_SciAdapt_cbCard0] hSci(0x%x), Slot(%d), Status(%d)\n", hSci, ucCardSlotId, nStatus);

	for ( idx=0; idx < MAX_SATSA_NUM_SC; idx++ )
	{
		if ( s_satsa_scmgrInst[idx].hCardHandle == hSci )
			break;
	}

	msg.wMsg = MSG_SCI_FROM_SC;
	msg.lParam1 = ucCardSlotId;
	msg.lParam2 = 0;
	msg.lParam3 = 0;
	msg.lParam4 = 0;

	if (( nStatus == ePAL_SC_CARDEVENT_EXTRACTED ) && ( s_satsa_scmgrInst[0].state == CARD_OTHER_SC_ACTION ))
	{
		CASMGR_UpdateCasEtcEvent(0, 0, eDxCAS_GROUPID_SATSA, eSEVT_CAS_SC_REMOVED, ucCardSlotId, eDxCAS_GROUPID_SATSA, 0);
		s_satsa_scmgrInst[idx].ucSlotId = SATSA_SC_INVALID_SLOT;
		s_satsa_scmgrInst[idx].CardStatus= CARD_OUT;
		s_satsa_scmgrInst[idx].state = CARD_OTHER_SC_ACTION;

		HxLOG_Print("\r\n\n\r\t [Satsa_SciAdapt_cbCard0] DDI_SC_EXTRACTED CARD_OTHER_SC_ACTION\n\r\n\r");
//		msg.nMsg = SCI_FROM_SCMGR_INVALID;
		msg.nMsg = SCI_FROM_SCMGR_EXTRACT;
	}
	else if ( nStatus == ePAL_SC_CARDEVENT_EXTRACTED )
	{
		CASMGR_UpdateCasEtcEvent(0, 0, eDxCAS_GROUPID_SATSA, eSEVT_CAS_SC_REMOVED, ucCardSlotId, eDxCAS_GROUPID_SATSA, 0);
		s_satsa_scmgrInst[idx].ucSlotId = SATSA_SC_INVALID_SLOT;
		s_satsa_scmgrInst[idx].CardStatus= CARD_OUT;
		s_satsa_scmgrInst[idx].state = CARD_EXTRACTED;

		HxLOG_Print("\r\n\n\r\t [Satsa_SciAdapt_cbCard0] DDI_SC_EXTRACTED \r\n\n\r");
		msg.nMsg = SCI_FROM_SCMGR_EXTRACT;
	}
	else if ( nStatus == ePAL_SC_CARDEVENT_INSERTED )
	{
		errRet = svc_cas_DevScReset(hSci, s_satsa_scmgrInst[idx].atr, &s_satsa_scmgrInst[idx].atrLen, ePAL_SC_RESETTYPE_COLD, &s_stDefaultAtrParam);
		if(errRet)
		{
			HxLOG_Info("Err:%x\n", errRet);
			s_satsa_scmgrInst[idx].hCardHandle = hSci;
			s_satsa_scmgrInst[idx].ucSlotId = ucCardSlotId;
			s_satsa_scmgrInst[idx].CardStatus= CARD_MUTE;
			s_satsa_scmgrInst[idx].state = CARD_INSERTED;

			HxLOG_Print("\r\n\n\r\t [Satsa_SciAdapt_cbCard0] SCI_FROM_SCMGR_MUTE \r\n\n\r");
			msg.nMsg = SCI_FROM_SCMGR_MUTE;
			CASMGR_UpdateCasEtcEvent(0, 0, eDxCAS_GROUPID_SATSA, eSEVT_CAS_SC_INSERTED, ucCardSlotId, eDxCAS_GROUPID_SATSA, 0);

		}
		else
		{
			s_satsa_scmgrInst[idx].hCardHandle = hSci;
			s_satsa_scmgrInst[idx].ucSlotId = ucCardSlotId;
			s_satsa_scmgrInst[idx].CardStatus= CARD_IN;
			s_satsa_scmgrInst[idx].state = CARD_INSERTED;

			HxLOG_Print("\r\n\n\r\t [Satsa_SciAdapt_cbCard0] DDI_SC_INSERTED \r\n\n\r");
			msg.nMsg = SCI_FROM_SCMGR_INSERT;
			CASMGR_UpdateCasEtcEvent(0, 0, eDxCAS_GROUPID_SATSA, eSEVT_CAS_SC_INSERTED, ucCardSlotId, eDxCAS_GROUPID_SATSA, 0);
		}
	}

	SATSA_MSG_Send(Get_satsaQueueId(), (void *)&msg, sizeof(SATSA_MSG));

	HxLOG_Print("\t[-Satsa_SciAdapt_cbCard0]\n");
}

static void local_SciAdapt_Init( )
{
	int i;

	for ( i=0; i < MAX_SATSA_NUM_SC; i++ )
	{
		s_satsa_scmgrInst[i].hCardHandle 		= 0;
		s_satsa_scmgrInst[i].ucSlotId 			= 0;
		s_satsa_scmgrInst[i].ucIsCardMarrying	= 0;
		s_satsa_scmgrInst[i].atrLen				= 0;
		if ( i == 0 )
		{
			s_satsa_scmgrInst[i].pfnCallBack 		= (PalSc_Callback_t)local_Satsa_SciAdapt_cbCard0;
		}
	}
}

static PalSc_Handle_t local_SciAdapt_GetSCIHandleWithSlotID( HUINT8 ucCardSlotId )
{
	int i;

	for(i=0; i < MAX_SATSA_NUM_SC; i++)
	{
		HxLOG_Print("[INT_SciAdapt_GetSCIHandleWithSlotID] ucSlotId(0x%x), ucDardSlotId(0x%x), handle(0x%x) \n",
								s_satsa_scmgrInst[i].ucSlotId, ucCardSlotId, s_satsa_scmgrInst[i].hCardHandle));

		if(ucCardSlotId == s_satsa_scmgrInst[i].ucSlotId)
			return s_satsa_scmgrInst[i].hCardHandle;
	}

	return SATSA_SC_INVALID_HANDLE;
}

/*****************************************************************************
							global function body
******************************************************************************/
SATSA_SCARD_RESULT_CODE local_Satsa_Sci_Adapt_ProcessProtocol(SATSA_SCARD_DEV_NUM devNum)
{
	PalSc_Handle_t	handle;
	unsigned long ret;

	SATSA_SCARD_RESULT_CODE scardResult = SATSA_SCARD_OK;
	HBOOL endSequence;
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	SATSA_SCARD_TRANS_IOPB transIOPB;

	SATSA_SCARD_EVENT			eventCode;

	int 		i;



	handle = s_satsa_scmgrInst[devNum].hCardHandle;

	scb = &scardControlBlock[ devNum ];
	t1Block = &scb->t1CntrlBlock;

	if ((scb->transactionStatus == SATSA_SCARD_T1_TRANSACTION) &&
		(scb->t1CntrlBlock.state != SATSA_SCARD_T1_IDLE))
	{
		/*
		** Note that values of 'endSequence' & 'scardResult' form
		** a state machine as follows:
		**
		** endSequence	FALSE	FALSE	TRUE	TRUE
		** scardResult	OK		FAIL		OK		FAIL
		** result			R/W 	R/W 	OK_END	ERROR_END
		*/
		endSequence = SATSA_FALSE;
		do
		{
//			if ((scb->transactionStatus == SATSA_SCARD_T1_TRANSACTION) &&
//				(scb->t1CntrlBlock.state != SATSA_SCARD_T1_IDLE))
			{
				scardResult = MW_SATSA_SCARD_T1_SendBlock(devNum, &endSequence);

				transIOPB.deviceNumber	= devNum;
				transIOPB.pucWriteBuf	= t1Block->blockOut;
				transIOPB.numWrite	= t1Block->blockOutLen;
				transIOPB.pucReadBuf	= t1Block->blockIn;
				transIOPB.pucNumRead	= t1Block->blockInLen;

				transIOPB.pucNumRead = 0;

				SATSA_SEM_Get(Get_satsaSemId());

				ret = svc_cas_DevScTransfer(handle,
								(unsigned char*) (transIOPB.pucWriteBuf),
								(int)(transIOPB.numWrite),
								(unsigned char*) (transIOPB.pucReadBuf),
								(int*)&(transIOPB.pucNumRead),
								0);

				SATSA_SEM_Release(Get_satsaSemId());

				if (ret != ERR_OK)
				{
					scb->error |= SATSA_SCARD_OTHER_ERROR;
					scardResult = SATSA_SCARD_FAIL;

					t1Block->errorCode = SATSA_SCARD_T1_ERROR_OTHER;

					return scardResult;
				}

				HxLOG_Print("[local_Satsa_Sci_Adapt_ProcessProtocol] send length = %d,\n",transIOPB.numWrite);
				HxLOG_Print("--------------------------Send--------------------------\n");
				for(i=0; i<transIOPB.numWrite; i++)
				{
					if(i%16 || i==0)
						HxLOG_Print(" %02x", transIOPB.pucWriteBuf[i]);
					else
						HxLOG_Print("\n %02x", transIOPB.pucWriteBuf[i]);
				}
				HxLOG_Print("\n");

				HxLOG_Print("\n--------------------------Read--------------------------\n");
				for(i=0; i<transIOPB.pucNumRead; i++)
				{
					if(i%16 || i == 0)
					HxLOG_Print(" %02x", transIOPB.pucReadBuf[i]);
					else
					HxLOG_Print("\n %02x", transIOPB.pucReadBuf[i]);
				}
				HxLOG_Print("\n");

				if((scb->transInfo.lengthMaxIn) < (transIOPB.pucNumRead))
				{
					HxLOG_Info("[local_Satsa_Sci_Adapt_ProcessProtocol] Over Max Length \n");
					return SATSA_ERROR;
				}

				t1Block->blockInLen = transIOPB.pucNumRead;

				scardResult = MW_SATSA_SCARD_T1_ReceiveBlock(devNum, &endSequence);
			}

		} while(endSequence == SATSA_FALSE);

		/*
		** T1 transaction close sequence
		*/
		if (scardResult == SATSA_SCARD_OK)
		{
			scb->error = SATSA_SCARD_NO_ERROR;
			eventCode = SATSA_SCARD_EVENT_TRANSACTION_COMPLETED;
			if (scb->flags.abortRequested == SATSA_TRUE)
			{
				eventCode = SATSA_SCARD_EVENT_ABORT_COMPLETED;
			}

//			scardResult == SATSA_SCARD_OK;
		}
		else if(scardResult == SATSA_SCARD_IFS_COMPLETE )
		{
			scb->error = SATSA_SCARD_NO_ERROR;
			eventCode = SATSA_SCARD_EVENT_IFS_COMPLETED;
		}
		else
		{
			scb->error |= SATSA_SCARD_OTHER_ERROR;
			eventCode = SATSA_SCARD_EVENT_ERROR;

			HxLOG_Info("[ local_Satsa_Sci_Adapt_ProcessProtocol ] scardResult = %d \r\n",scardResult);
			scardResult = SATSA_SCARD_FAIL;

//			return scardResult;
		}

		scb->t1CntrlBlock.state = SATSA_SCARD_T1_IDLE;
		scb->transactionStatus = SATSA_SCARD_IDLE;

		scb->flags.abortRequested = SATSA_FALSE;
		scb->t1CntrlBlock.cardAbortRequested = SATSA_FALSE;
		scb->cardStatus = SATSA_SCARD_INSERTED;

		if ( eventCode != SATSA_SCARD_EVENT_ERROR)
		{
			scb->eventData.data = scb->transInfo.dataIn;
			scb->eventData.dataLength = scb->numRAPDUBytesReceived;
		}
		else
		{
			scb->eventData.dataLength = 0;
			scb->numRAPDUBytesReceived = 0;
			SATSA_memset(scb->eventData.data,0,sizeof(scb->eventData.data));
			SATSA_memset(scb->transInfo.dataIn,0,sizeof(scb->transInfo.dataIn));
		}

		/* notify application about the results */

		endSequence = SATSA_FALSE;

	}
	else
	{
		scardResult = SATSA_SCARD_INVALID_PARAM; /* invalid data length parameters */
	}

	return scardResult;
}

HUINT32 MW_Satsa_Sci_Adapt_TransferData (
							HUINT16 slot,
							HUINT8 *sendBuf,
							HUINT16 lenSendBuf,
							HUINT8 *recvBuf,
							HUINT8 *lenRecvBuf,
							HUINT16 recvMaxLength)
{
	int index;
	int i;

	SATSA_SCARD_RESULT_CODE satsaResult = SATSA_SCARD_OK;
	SATSA_SCARD_CONTROL_BLOCK *scb;
//	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;
	SATSA_SCARD_DEV_NUM devNum;

//	HUINT32 read_length=0;

	HxLOG_Print("[SATSA_Sci_Adapt_TransferData]+\n");

	index = 0;

//	*lenRecvBuf = 0;
	HxLOG_Print("[SATSA_Sci_Adapt_TransferData] send length = %d, read length = %d max len = %d\n",lenSendBuf, *lenRecvBuf, recvMaxLength);

	if (slot > MAX_VALID_SLOT_NUM)
	{
		HxLOG_Info("[SATSA_Sci_Adapt_TransferData] Invalid Session ID\n");
		return SATSA_ERROR_SESSION_ID;
	}
	if (sendBuf == NULL)
	{
		HxLOG_Info("[SATSA_Sci_Adapt_TransferData] pxSendBlock is NULL\n");
		return SATSA_ERROR;
	}
	if (recvMaxLength == 0)
	{
		HxLOG_Info("[SATSA_Sci_Adapt_TransferData] xReplyMaxLen is 0\n");
		return SATSA_ERROR;
	}
	if (lenSendBuf == 0)
	{
		HxLOG_Info("[SATSA_Sci_Adapt_TransferData] xSendLen is 0\n");
		return SATSA_ERROR;
	}

	if (s_satsa_scmgrInst[index].CardStatus == CARD_OUT)
	{
		HxLOG_Info("[SATSA_Sci_Adapt_TransferData] CARD_REMOVED\n");
		return SATSA_ERROR_CARD_REMOVED;
	}
	if (s_satsa_scmgrInst[index].CardStatus == CARD_MUTE)
	{
		HxLOG_Info("[SATSA_Sci_Adapt_TransferData] CARD_REMOVED\n");
		return SATSA_ERROR_CARD_MUTE ;
	}

	HxLOG_Print("[SATSA_Sci_Adapt_TransferData] send length = %d, pxReplyLen = %d max len = %d session Id = %d\n",lenSendBuf, *lenRecvBuf, recvMaxLength, slot);
	HxLOG_Print("--------------------------S--------------------------\n");
	for(i=0; i<lenSendBuf; i++)
	{
		if(i%16 || i==0)
			HxLOG_Print(" %02x", sendBuf[i]);
		else
			HxLOG_Print(" %02x\n", sendBuf[i]);
	}

	devNum = index;
	 scb = &scardControlBlock[devNum];
//	 t1Block = &scb->t1CntrlBlock;

	/*
	** store internally input parameters
	*/
	scb->transInfo.dataOut 	= sendBuf;
	scb->transInfo.lengthOut 	= lenSendBuf;
	scb->transInfo.dataIn 		= recvBuf;
	scb->transInfo.lengthIn 	= lenRecvBuf;   /* Maximum length to be received */
	scb->transInfo.lengthMaxIn	= recvMaxLength;

	 /* Clear current error*/
	 scb->error = SATSA_SCARD_NO_ERROR;

	if ((scb->transInfo.lengthOut >= SATSA_SCARD_MIN_C_APDU_LEN)&&
		(scb->transInfo.lengthMaxIn >= SATSA_SCARD_MIN_R_APDU_LEN))
	{
		/* initilaise data transfer arguments */
		if (MW_SATSA_SCARD_T1_Initialise(devNum) == SATSA_SCARD_OK)
		{
			/*
			** Compose the first block to send
			*/
			MW_SATSA_SCARD_T1_Compose_I_Block(devNum);
			/*
			** Note: If it becomes necessary for an application to initially
			** send an S-block, the application needs to pass
			** a block id as a parameter.
			*/

			/*
			** kick off T=1 transaction
			*/
			scb->transactionStatus = SATSA_SCARD_T1_TRANSACTION;
			scb->t1CntrlBlock.state = SATSA_SCARD_T1_SENDING_BLOCK;

			satsaResult = local_Satsa_Sci_Adapt_ProcessProtocol(devNum);
			if ((satsaResult == SATSA_SCARD_IFS_COMPLETE) || (satsaResult == SATSA_SCARD_OK))
			{
				satsaResult = SATSA_NO_ERROR;
				HxLOG_Print(" scb->error:0x%X, scb->cardStatus:%d\r\n", scb->error, scb->cardStatus);
				HxLOG_Print("[SATSA_Sci_Adapt_TransferData] SATSA_NO_ERROR\n");
			}
			else
			{

			//deactivation


				*lenRecvBuf = 0;
				HxLOG_Info(" scb->error:0x%X, scb->cardStatus:%d\r\n", scb->error, scb->cardStatus);
				HxLOG_Info("[SATSA_Sci_Adapt_TransferData] SATSA_ERROR\n");

				return SATSA_ERROR;
			}
		}
		else
		{
			satsaResult = SATSA_SCARD_FAIL;
			HxLOG_Info(" scb->error:0x%X, scb->cardStatus:%d\r\n", scb->error, scb->cardStatus);
			HxLOG_Info("[SATSA_Sci_Adapt_TransferData] satsaResult = %d\n",satsaResult);

			return SATSA_ERROR;
		}
	}
	else
	{

		satsaResult = SATSA_SCARD_INVALID_PARAM; /* invalid data length parameters */
		HxLOG_Info(" scb->error:0x%X, scb->cardStatus:%d\r\n", scb->error, scb->cardStatus);
		HxLOG_Info("[SATSA_Sci_Adapt_TransferData] satsaResult = %d\n",satsaResult);

		return SATSA_ERROR;
	}

	*lenRecvBuf = scb->eventData.dataLength;
//	*recvBuf =  scb->eventData.data;
	SATSA_memcpy(recvBuf,scb->eventData.data,scb->eventData.dataLength);

#if 1	//#if DEBUG_SATSA_TRANSFER

	HxLOG_Print("[SATSA_Sci_Adapt_TransferData] scb->eventData.dataLength length = %d\n",scb->eventData.dataLength);
	HxLOG_Print("[SATSA_Sci_Adapt_TransferData] *lenRecvBuf  length = %d\n",*lenRecvBuf );

	HxLOG_Print("\n----------------------READ ----------------------\n");

	for(i=0; i<scb->eventData.dataLength; i++)
	{
		if(i%16 || i == 0)
			HxLOG_Print(" %02x", scb->eventData.data[i]);
		else
			HxLOG_Print("\n %02x", scb->eventData.data[i]);
	}
	HxLOG_Print("\n");

#endif
	HxLOG_Print("[SATSA_Sci_Adapt_TransferData]-\n");

	return SATSA_NO_ERROR;
}


HUINT32 local_Satsa_Sci_Adapt_TransferSBlock (HUINT16 slot)
{
	int index;

	HUINT8 sendBuf[5];
	HUINT8 recvBuf[5];
	HUINT16 lenSendBuf =5;
	HUINT16 recvMaxLength = 5;
	HUINT8 lenRecvBuf =0;

	SATSA_SCARD_RESULT_CODE satsaResult = SATSA_SCARD_OK;
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_DEV_NUM devNum;


	HxLOG_Print("[Satsa_Sci_Adapt_TransferSBlock]+\n");

	index = 0;

	if (slot > MAX_VALID_SLOT_NUM)
	{
		HxLOG_Info("[Satsa_Sci_Adapt_TransferSBlock] Invalid Session ID\n");
		return SATSA_ERROR_SESSION_ID;
	}

	if (s_satsa_scmgrInst[index].CardStatus == CARD_OUT)
	{
		HxLOG_Info("[Satsa_Sci_Adapt_TransferSBlock] CARD_REMOVED\n");
		return SATSA_ERROR_CARD_REMOVED;
	}
	if (s_satsa_scmgrInst[index].CardStatus == CARD_MUTE)
	{
		HxLOG_Info("[Satsa_Sci_Adapt_TransferSBlock] CARD_REMOVED\n");
		return SATSA_ERROR_CARD_MUTE ;
	}

	devNum = index;
	 scb = &scardControlBlock[devNum];
//	 t1Block = &scb->t1CntrlBlock;

	/*
	** store internally input parameters
	*/
	scb->transInfo.dataOut 	= sendBuf;
	scb->transInfo.lengthOut 	= lenSendBuf;
	scb->transInfo.dataIn 		= recvBuf;
	scb->transInfo.lengthIn 	= &lenRecvBuf;   /* Maximum length to be received */
	scb->transInfo.lengthMaxIn	= recvMaxLength;

	 /* Clear current error*/
	 scb->error = SATSA_SCARD_NO_ERROR;

	if ((scb->transInfo.lengthOut >= SATSA_SCARD_MIN_C_APDU_LEN)&&
		(scb->transInfo.lengthMaxIn >= SATSA_SCARD_MIN_R_APDU_LEN))
	{
		/* initilaise data transfer arguments */
		if (MW_SATSA_SCARD_T1_Initialise(devNum) == SATSA_SCARD_OK)
		{
			/*
			** Compose the first block to send
			*/
			MW_SATSA_SCARD_T1_Compose_S_Block(devNum);
			/*
			** Note: If it becomes necessary for an application to initially
			** send an S-block, the application needs to pass
			** a block id as a parameter.
			*/

			/*
			** kick off T=1 transaction
			*/
			scb->transactionStatus = SATSA_SCARD_T1_TRANSACTION;
			scb->t1CntrlBlock.state = SATSA_SCARD_T1_SENDING_BLOCK;

			satsaResult = local_Satsa_Sci_Adapt_ProcessProtocol(devNum);
			if ((satsaResult == SATSA_SCARD_IFS_COMPLETE) || (satsaResult == SATSA_SCARD_OK))
			{
				satsaResult = SATSA_NO_ERROR;
				HxLOG_Print(" scb->error:0x%X, scb->cardStatus:%d\r\n", scb->error, scb->cardStatus);
				HxLOG_Print("[Satsa_Sci_Adapt_TransferSBlock] SATSA_NO_ERROR\n");
			}
			else
			{

			//deactivation
				lenRecvBuf = 0;
				HxLOG_Info(" scb->error:0x%X, scb->cardStatus:%d\r\n", scb->error, scb->cardStatus);
				HxLOG_Info("[Satsa_Sci_Adapt_TransferSBlock] SATSA_ERROR\n");

				return SATSA_ERROR;
			}
		}
		else
		{
			satsaResult = SATSA_SCARD_FAIL;
			HxLOG_Info(" scb->error:0x%X, scb->cardStatus:%d\r\n", scb->error, scb->cardStatus);
			HxLOG_Info("[Satsa_Sci_Adapt_TransferSBlock] satsaResult = %d\n",satsaResult);

			return SATSA_ERROR;
		}
	}
	else
	{

		satsaResult = SATSA_SCARD_INVALID_PARAM; /* invalid data length parameters */
		HxLOG_Info(" scb->error:0x%X, scb->cardStatus:%d\r\n", scb->error, scb->cardStatus);
		HxLOG_Info("[Satsa_Sci_Adapt_TransferSBlock] satsaResult = %d\n",satsaResult);

		return SATSA_ERROR;
	}

	lenRecvBuf = scb->eventData.dataLength;
//	*recvBuf =  scb->eventData.data;
	SATSA_memcpy(recvBuf,scb->eventData.data,scb->eventData.dataLength);

	HxLOG_Print("[Satsa_Sci_Adapt_TransferSBlock]-\n");

	return SATSA_NO_ERROR;
}

int MW_Satsa_Sci_Adapt_getATR(HUINT16 slotId, HUINT8 *atrBuf,HUINT16 *atrLength)
{
	int errRet;
	PalSc_CardStatus_e	eStatus;
	SATSA_SCARD_CONTROL_BLOCK *scb;
	PalSc_Handle_t		hSci;

	*atrLength = 0;
	HxLOG_Print("[Satsa_Sci_Adapt_getATR]+\n");

	scb = &scardControlBlock[slotId];

	//SATSA_memset(&(scb->satsa_param_buf),0,sizeof(PalSc_Param_t));


	hSci = local_SciAdapt_GetSCIHandleWithSlotID(slotId);
	if (hSci == SATSA_SC_INVALID_HANDLE)
	{
		HxLOG_Info("wrong handle\n");
		return ERR_FAIL;
	}

	errRet = svc_cas_DevScGetCardStatus(s_satsa_scmgrInst[slotId].hCardHandle, &eStatus);
	if(errRet)
	{
		HxLOG_Info("Err:%x\n", errRet);
		return errRet;
	}

	if(eStatus != ePAL_SC_CARDEVENT_INSERTED)
	{
		HxLOG_Info("eStatus:%d\n", eStatus);
		return SATSA_ERROR_CARD_REMOVED;
	}

	errRet = svc_cas_DevScReset(hSci, s_satsa_scmgrInst[slotId].atr, &s_satsa_scmgrInst[slotId].atrLen, ePAL_SC_RESETTYPE_COLD, &s_stDefaultAtrParam);
	if(errRet)
	{
		HxLOG_Info("Err:%x\n", errRet);
		return errRet;
	}
#ifdef SATSA_PPS_EXCHANGE
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

		local_Satsa_SC_ParseAtr(s_satsa_scmgrInst[slotId].atr, &stAtrInfo);

		local_Satsa_SC_Negotiate(hSci, &stAtrInfo, &s_stDefaultAtrParam);

	}
#endif

	local_Satsa_Sci_Adapt_TransferSBlock (slotId);

#ifdef DEBUG_SATSA_TRANSFER
	{
		int k;

		HxLOG_Print("----------- 	SATSA A T R	  -----------\n");
		HxLOG_Print("\t\t atrLen = %d \r\n", s_satsa_scmgrInst[slotId].atrLen);

		for(k=0; k<s_satsa_scmgrInst[slotId].atrLen; k++)
		{
			HxLOG_Print("\n");
			HxLOG_Print("0x%02x ", s_satsa_scmgrInst[slotId].atr[k]);
		}
		HxLOG_Print("\n");
		HxLOG_Print("------------------------------------------\n");
	}
#endif

	HxLOG_Print("[Satsa_Sci_Adapt_getATR] Protocol = %d \r\n",scb->satsa_param_buf.eProtocol);

	if ( scb->satsa_param_buf.eProtocol == ePAL_SC_PROTOCOL_T1)	//20060905	TestByPooh : 일단 처리를 할것인지 결정해야됨. TEMP
	{
		HxLOG_Print("[Satsa_Sci_Adapt_cardStaus] CARD_IN : T1 Protocol\n\r");
	}
	else
	{
		HxLOG_Print("[Satsa_Sci_Adapt_cardStaus] CARD_IN : %s protocol\n\r",(scb->satsa_param_buf.eProtocol ==ePAL_SC_PROTOCOL_T0) ? "T0":((scb->satsa_param_buf.eProtocol ==ePAL_SC_PROTOCOL_T14) ? "T14" : "ERR"));
	}

	SATSA_memcpy(atrBuf, s_satsa_scmgrInst[slotId].atr, s_satsa_scmgrInst[slotId].atrLen);
	*atrLength = s_satsa_scmgrInst[slotId].atrLen;

	HxLOG_Print("[Satsa_Sci_Adapt_getATR]-\n");

	return SATSA_NO_ERROR;
}

void MW_Satsa_Sci_Adapt_cardStatus(HUINT8 slotId,HUINT16 cardStatus)
{
	HERROR	hError = ERR_OK;

	if(cardStatus == SCI_FROM_SCMGR_INSERT)
	{
		hError = svc_cas_DevScNotifyScValidity(s_satsa_scmgrInst[slotId].hCardHandle, TRUE);
	}
	else
	if(cardStatus == SCI_FROM_SCMGR_EXTRACT)
	{
		hError = svc_cas_DevScNotifyScValidity(s_satsa_scmgrInst[slotId].hCardHandle, FALSE);
	}

	if(ERR_OK != hError)
	{
		HxLOG_Info("[Satsa_Sci_Adapt_cardStaus] SCI_NotifyScValidity return error(%x)\n", hError);
		return;
	}

	switch (cardStatus)
	{
		case SCI_FROM_SCMGR_INSERT:
			HxLOG_Print("[Satsa_Sci_Adapt_cardStaus] CARD_IN : T1 Protocol\n\r");
			MW_Satsa_Ui_ConvertEvent2Gule(slotId, CARD_IN);
			break;

		case SCI_FROM_SCMGR_MUTE:
			HxLOG_Print("[Satsa_Sci_Adapt_cardStaus] SCI_FROM_SCMGR_MUTE\n\r");
			MW_Satsa_Ui_ConvertEvent2Gule(slotId, CARD_MUTE);
			break;

		case SCI_FROM_SCMGR_EXTRACT:
			HxLOG_Print("[Satsa_Sci_Adapt_cardStaus] SCI_FROM_SCMGR_EXTRACT\n\r");
			MW_Satsa_Ui_ConvertEvent2Gule(slotId, CARD_OUT);
			break;

		case SCI_FROM_SCMGR_INVALID:
			HxLOG_Print("[Satsa_Sci_Adapt_cardStaus] SCI_FROM_SCMGR_INVALID\n\r");
			MW_Satsa_Ui_ConvertEvent2Gule(slotId, CARD_OTHER_SC_ACTION);
			break;

		case SCI_FROM_SCMGR_DENIED:
			HxLOG_Print("[Satsa_Sci_Adapt_cardStaus] SCI_FROM_SCMGR_DENIED\n\r");
			MW_Satsa_Ui_ConvertEvent2Gule(slotId, CARD_ACCESS_DENIED);
			break;

		default:
			break;

	}

}

HUINT8 * MW_Satsa_Sci_Adapt_getSCList (void)
{
	return (HUINT8*)SLOT_READER_INDEX;
}

int MW_Satsa_Sci_Adapt_StartCard( )
{
	int 		ulErr, i;
	HUINT32		ulNumSlot;

	HxLOG_Print("\t[+Satsa_Sci_Adapt_StartCard]\n");

	local_SciAdapt_Init();

	ulNumSlot = svc_cas_DevScGetNumberOfModule();

	for ( i=0; i < ulNumSlot && i < MAX_SATSA_NUM_SC; i++ )
	{
		ulErr = svc_cas_DevScRegisterCbFunc(i, eDxCAS_GROUPID_SATSA, s_satsa_scmgrInst[i].pfnCallBack, &(s_satsa_scmgrInst[i].hCardHandle));
		if(ulErr)
			HxLOG_Info("CX_ERR<%x> [Satsa_Sci_Adapt_StartCard][SCI_RegisterCbFunc]\n", ulErr);

		HxLOG_Print("[Satsa_Sci_Adapt_StartCard] hSci[%d], handle <%x>\n\n", i, s_satsa_scmgrInst[i].hCardHandle);
	}

	HxLOG_Print("\t[-Satsa_Sci_Adapt_StartCard]\n");

	return 0;
}



