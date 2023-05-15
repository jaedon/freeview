/*
 * (c) 2011-2012 Humax Co., Ltd.
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

#include "octo_common.h"
#include "UniversalClient_Common_SPI.h"
#include "UniversalClient_Common_API.h"


/* TODO 정리할 헤더들 ... ciyoon */
#include "ir_platform.h"
#include "ir_api.h"

/* Humax Hardware 관련 API Header들 */
#include "pal_sc.h"
#include "di_smartcard.h"


#ifdef UC_SC_IO_DEBUG
#define	PrintUcScATR	HxLOG_Print
#define	PrintUcScCom	HxLOG_Print
#else
#define	PrintUcScATR(...)
#define	PrintUcScCom(...)
#endif


/* ToDo 다른 곳으로 이동할 것 ...*/
#define _____UC_SMARTCARD________________
typedef struct
{
	PalSc_Handle_t				hIRSci;
	uc_uint32				SmartcardID;
	unsigned long			SemaphoreID;
	PalSc_CardStatus_e			CardPhysicalStatus;
	HUINT32					CardStatus;
	HUINT32					RetryCount;
	HUINT32					Protocol;
	HUINT32					ulAtrLen;
	HUINT8					aucATR[34];
	HUINT8					VersionFromATR[4];
#if defined(CONFIG_OP_DIGITURK)
	HUINT8					aucTMSUserData[IR_SC_INFO_TMS_DATA_LENGTH];
#endif
	HUINT8					szScID[IR_SC_INFO_ID_LENGTH]; //hashed value
	uc_sc_open_parameter	ScOpen;
	uc_smartcard_error_type	ErrorReason;
} UC_DRV_SC_INFO;


typedef struct tagUcScAtrInfo
{
	HUINT32		ulMinClock;
	HUINT32		ulBaudrate;

	HUINT8		ucK;
	HUINT8		ucFI;
	HUINT8		ucDI;
	HUINT8		ucN;
	HUINT8		ucWI;
	HUINT8		ucTFirst;
	HUINT8		ucT;
	HUINT8		ucIFSC;
	HUINT8		ucCWI;
	HUINT8		ucBWI;
	HUINT8		ucII;
	HUINT8		ucPI1;
	HUINT8		ucPI2;
	HUINT8		ucEDC;	//error detection code TC3

	HBOOL		bSpecificMode;
	HBOOL		bImplicitlyDefined;
	HBOOL		bPpsUnable;
	HBOOL		bExistTA1;
} UcScAtrInfo;


/* Humax Universial Client result*/
typedef enum
{
	eHUC_OK =0,
	eHUC_ERR,
	eHUC_ERR_NOT_IRDETOCARD,
	eHUC_ERR_INVALID_CARD,
}HUC_Result;


static PalSc_Param_t s_stUcScInParamT0 =
{
	ePAL_SC_PROTOCOL_T0_IR,		// protocol
	0,		// Min Clock
	0,		// baudrate
	1,		// FI
	1,		// DI
	0,		// N
	13,		// CWI		13
	4,		// BWI		4
};

static PalSc_Param_t s_stUcScInParamT14 =
{
	ePAL_SC_PROTOCOL_T14,		// protocol
	3000 , //6000,	// Min Clock
	0,		// baudrate
	2,		// FI
	1,		// DI
	0,		// N
	13,		// CWI		13
	5,		// BWI		4
};


static HUINT8 s_aucUcFiOrder[16] =
{ 0xFF, 1, 3, 4, 7, 8, 10, 0xFF, 0xFF, 2, 5, 6, 9, 11, 0xFF, 0xFF };

static HUINT8 s_aucUcDiOrder[16] =
{ 0xFF, 1, 2, 3, 4, 6, 8, 0xFF, 5, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static HUINT8 s_aucUcFiValue[16] =
{ 0xFF, 1, 9, 2, 3, 10, 11, 4, 5, 12, 6, 13, 0xFF, 0xFF, 0xFF, 0xFF };

static HUINT8 s_aucUcDiValue[16] ={ 0xFF, 1, 2, 3, 4, 8, 5, 9, 6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static UcScAtrInfo s_stUcAtrInfo;


/* 생산용 SST TEST를 위해서 필요한 config */
#if defined (CONFIG_MW_CAS_IRDETO_SECURE)
#define T14_RETRY_COUNT	3
#else
#define T14_RETRY_COUNT	2
#endif

// 일반적으로 2번해서 안되면 안되는 것임. 3으로 괜히 늘려놓지 말것.
#define MAX_NUM_RETRY		2
#define T0_RETRY_COUNT		2

#define	IRDETO_CARD_HISTORY_CHARS  	"IRDETO ACS" //but it shall be not adapted with ice-2 card


/* Definitions for reserved reference IDs. */
#define UC_REF_ID_MAX 		((uc_uint32)0xFFFEFFFF)				/* For return on UniversalClientSPI_Smartcard_Open(). */
#define UC_REF_ID_ERROR 	((uc_uint32)0xFFFFFFFE)				/* For return on UniversalClientSPI_Smartcard_Open(). */
#define UC_REF_ID_GLOBAL 	((uc_uint32)( UC_REF_ID_MAX + 1 ))	 /* For UniversalClientSPI_Smartcard_Open() */


/* global variable to store smart card status */
UC_DRV_SC_INFO		g_UcDrvScInfo[HUMAX_VD_SC_SLOT_COUNT]; /* VD에서 UC로 변경 ???*/


void CAS_IR_SendMessage(HUINT16 msg_channel, HUINT16 msg_type, void* pMsg) // external ToDO ciyoon 나중에 CAS_IR TASK를 포팅시 추가작업 할 것 ...
{
  // TO DO
}


static void local_uc_SC_Callback(HUINT8 ulSlotID, HUINT8 ucCardStatus, uc_buffer_st *pvInfo)
{
	if (ucCardStatus != UC_SC_CARD_IN)
	{
		g_UcDrvScInfo[ulSlotID].szScID[0] = '\0';
	}

	/* smartcardHandle에 softcell 값이 저장된다.  */
	(*(g_UcDrvScInfo[ulSlotID].ScOpen.pfnStatusNotify))(g_UcDrvScInfo[ulSlotID].SmartcardID, g_UcDrvScInfo[ulSlotID].ScOpen.smartcardHandle, ucCardStatus, (void *)pvInfo);
}


static HUINT32 local_UcSc_CheckProtocol( HUINT8 *pbData, HUINT8 bLength )
{
	HUINT32 ret_value;

	HxLOG_Print("\n") ;

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
		HxLOG_Print ("Check this ATR data \n") ;
		ret_value = -1;
	}

	return (ret_value);

}


static HUC_Result local_UcSc_CheckATR( HUINT32 ulSlotId, HUINT8 *buf, HUINT32 ulAtrLen, HUINT32 *pnProtocol )
{
	HUINT8	ret_value;
	HxLOG_Print("\n") ;

	if ( ulAtrLen > 0 )
	{
		*pnProtocol  = g_UcDrvScInfo[ulSlotId].Protocol  = local_UcSc_CheckProtocol(buf, (HUINT8)ulAtrLen);

        // validate the check-sum

		if ( *pnProtocol  == 14 )
		{
			 // Check the validity of smart card. card should contain the string "IRDETO ACS V1."
			if ( memcmp(buf + 4, IRDETO_CARD_HISTORY_CHARS, 10) == 0 )
			{
				HxLOG_Print("### T14 : VALID CARD\n");
				  // save the version of smart card
	       	       VK_memcpy( g_UcDrvScInfo[ulSlotId].VersionFromATR, buf + 15, sizeof(g_UcDrvScInfo[ulSlotId].VersionFromATR) );
	              	ret_value = eHUC_OK;
			}
			else
			{
				HxLOG_Print ("### T14 : INVALID CARD\n");
				ret_value = eHUC_ERR_NOT_IRDETOCARD;
			}
		}
		else if(*pnProtocol  == 0)
		{
			if ( memcmp(buf + 5, IRDETO_CARD_HISTORY_CHARS, 10) == 0 )
	       	{
				HxLOG_Print("### T0 : VALID CARD\n");
				// save the version of smart card
				VK_memcpy( g_UcDrvScInfo[ulSlotId].VersionFromATR, buf + 16, 4 );
				ret_value = eHUC_OK;
			}
			else
			{
				HxLOG_Print("### T0 : INVALID CARD\n");
				ret_value = eHUC_ERR_NOT_IRDETOCARD;
			}
	        }
	        else
	        {
			HxLOG_Print("### INVALID PROTOCOL\n");
			ret_value = eHUC_ERR_INVALID_CARD;
	       }
	}
	else
	{
		HxLOG_Print("NO ATR DATA\n");
		ret_value = eHUC_ERR_INVALID_CARD;
	}

	return ret_value ;
}


#if defined(CONFIG_DEBUG)
static void local_UcSc_DebugParsedAtr(UcScAtrInfo *pstAtrInfo)
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



static HERROR local_UcSc_ParseAtr(HUINT8 *pucAtr, HUINT32 ulAtrLen,UcScAtrInfo *pstAtrInfo)
{
	HUINT8 *pucData = NULL;
	HUINT8 ucIndex = 1;
	HUINT8 ucTS, ucT0, ucY, ucTAx, ucTBx, ucTCx, ucTDx;
	HUINT8 ucModeT = 0;
	HUINT8 ucSupportedProtocolTypes = 0;

	if ( (pucAtr == NULL) || (pstAtrInfo == NULL) )
	{
		HxLOG_Print("targets are null ...\n");
		return ERR_FAIL;
	}

	pucData = pucAtr;					// TS
	ucTS = *pucData;

	if ((ucTS != 0x3B) && (ucTS != 0x3F))
	{
		HxLOG_Print("Invalid TS value\n");
		return ERR_FAIL;
	}

	ucY = ucT0 = *(++pucData);
	pstAtrInfo->ucK = ucT0 & 0x0F;

	while (ucY)
	{
		HxLOG_Print("start ucIndex(%d)\t",ucIndex );
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
		HUINT8 ucCurIndex = 0;

		/* check historical byte size for checking Max ATR size validation */
		/* read the historical characters. */
		if (pstAtrInfo->ucK > 0)
		{
			pucData += pstAtrInfo->ucK;
			ucCurIndex = pucData - pucAtr +1;
		}

		HxLOG_Print("ATR Size atrlen %d,  curindex %d , K %d\n", ulAtrLen, ucCurIndex, pstAtrInfo->ucK);

	    /* TCK character should also be present only if list of supported protocol types is not t=0 by itself. */
		/* if in T = 1 mode, then TCK byte must be present and validated */
		if (ucSupportedProtocolTypes != 1)
		{
			HINT32	i = 1;
			HUINT8 	ucTCKCompare = 0;

			ucTck = *pucData;		// 사용안함.
			// Start from T0 to TCK.  Including historical bytes if they exist */
			for (i=1; i<=ucCurIndex; i++)
			{
				ucTCKCompare = ucTCKCompare ^ pucAtr[i];
				HxLOG_Print("In SmartCardValidateTCKByte inp_ucATR[%d] = %02x, 0x%2x\n", i, pucAtr[i] , ucTCKCompare);

			}
			ucCurIndex++;			// inc index

			if (ucTCKCompare != 0)
			{
				///* this is called with EDC/Lrc in ATR !!!
				HxLOG_Print("Invalid TCK. : in EDC, Lrc !!!!!!!!!!! \n");
				return ERR_FAIL;
			}
			else
			{
				/* TCK validated successfully */
				HxLOG_Print("Valid TCK.!! \n");
			}
		}
		HxLOG_Print("ucSupportedProtocolTypes %d, ucTck %d \n", ucSupportedProtocolTypes, ucTck);

		/* Check Validation of ATR size : Only extra chars are allowed in ATR */
		if (ulAtrLen < ucCurIndex)
		{
			/* ATR Size Error */
			HxLOG_Print("ATR Size Error\n");
			return ERR_FAIL;
		}
	}
#endif

#if defined(CONFIG_DEBUG)
	local_UcSc_DebugParsedAtr(pstAtrInfo);
#endif
	return ERR_OK;
}



static PalSc_Protocol_e local_UcSc_ConvertProtocol(DI_SC_PROTOCOL eInProtocol)
{
	PalSc_Protocol_e eScPotocol;

	switch(eInProtocol)
	{
		case DI_T_0:
			eScPotocol = ePAL_SC_PROTOCOL_T0;
			break;
		case DI_T_1:
			eScPotocol = ePAL_SC_PROTOCOL_T1;
			break;
		case DI_T_14:
			eScPotocol = ePAL_SC_PROTOCOL_T14;
			break;
		default:
			eScPotocol = ePAL_SC_PROTOCOL_END;
			break;
	}

	HxLOG_Print("[local_UcSc_ConvertProtocol] (0x%x, 0x%x)\n", eScPotocol, eInProtocol);
	return eScPotocol;
}

#if 1 // negotiation mode는 나중에 표팅 To do

static HERROR local_UcSc_RequestPps(PalSc_Handle_t hSci, HUINT8 ucT, HUINT8 ucFi, HUINT8 ucDi, PalSc_Param_t *pstDefaultParam)
{
	HERROR 	nErr = ERR_OK;
	HUINT8 	aPpsRequest[4];
	HUINT8 	aPpsResponse[4] = {0,0,0,0};
	HUINT8 	ucNumRead;

#if defined(CONFIG_DEBUG)
	HxLOG_Print("local_VdSc_RequestPps\n");
	HxLOG_Print ("pstDefaultParam->eProtocol = %d\n", pstDefaultParam->eProtocol);
	HxLOG_Print ("pstDefaultParam->ulClock = %d\n", pstDefaultParam->ulClock);
	HxLOG_Print ("pstDefaultParam->ulBaudrate = %d\n", pstDefaultParam->ulBaudrate);
	HxLOG_Print ("pstDefaultParam->ucFI = %d\n", pstDefaultParam->ucFI);
	HxLOG_Print ("pstDefaultParam->ucDI = %d\n", pstDefaultParam->ucDI);
	HxLOG_Print ("pstDefaultParam->ucN = %d\n", pstDefaultParam->ucN);
	HxLOG_Print ("pstDefaultParam->ucCWI = %d\n", pstDefaultParam->ucCWI);
	HxLOG_Print ("pstDefaultParam->ucBWI = %d\n", pstDefaultParam->ucBWI);
#endif

	nErr = PAL_SC_SetParams(hSci, pstDefaultParam);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("err<%x>\n", nErr);
	}

	aPpsRequest[0] = 0xFF;
	aPpsRequest[1] = 0x10 | (ucT & 0x0F);
	aPpsRequest[2] = ( (ucFi << 4) & 0xF0 ) | (ucDi & 0x0F );
	aPpsRequest[3] = aPpsRequest[0] ^ aPpsRequest[1] ^ aPpsRequest[2];

	nErr = PAL_SC_NegotiatePTS(hSci, aPpsRequest, 4, aPpsResponse, &ucNumRead);
	if (nErr != ERR_OK)
	{
		HxLOG_Error ("err<%x>\n", nErr);
	}

	HxLOG_Print ("Req : %x, %x, %x, %x\n", aPpsRequest[0], aPpsRequest[1], aPpsRequest[2], aPpsRequest[3]);
	HxLOG_Print ("Res : %x, %x, %x, %x (%d)\n", aPpsResponse[0], aPpsResponse[1], aPpsResponse[2], aPpsResponse[3], ucNumRead);

	if ( (aPpsResponse[0] | aPpsResponse[1] | aPpsResponse[2] | aPpsResponse[3]) == 0 )
	{
		HxLOG_Error ("err in data\n");
		return ERR_FAIL;
	}

	// skk
	// (1) response is same with request
	// (2) reponse(1-4) is same with request (1-4) and response(5-7) set to 0
//	if(memcmp(aPpsRequest, aPpsResponse, 4))
	if ( !((aPpsRequest[1] == aPpsResponse[1]) && (aPpsRequest[2] == aPpsResponse[2])) &&
		!((aPpsResponse[1] < 0x10) && ((aPpsRequest[1] & 0xF) == (aPpsResponse[1] & 0xF))) )
	{
		HxLOG_Error ("reject \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}




static HERROR local_UcSc_NegotiateByValueOrder(PalSc_Handle_t hSci, UcScAtrInfo *pstAtrInfo, HUINT8 ucWI, HUINT8 ucBWI, PalSc_Param_t *pstDefaultParam)
{
	HUINT8			i;
	HUINT8			ucT;
	HUINT8			ucF, ucD;
	HUINT8			ucCntF, ucCntD;
	HUINT8			ucOriCntD;
	UcScAtrInfo 	*p;
	PalSc_Param_t 	stScParam;
	HERROR 			nErr = ERR_OK;

	p = pstAtrInfo;

	ucF = p->ucFI;
	ucD = p->ucDI;

	ucCntF = s_aucUcFiOrder[ucF];
	ucCntD = ucOriCntD = s_aucUcDiOrder[ucD];

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
		HxLOG_Print ("UNIQUE T\n");
		while(ucCntF >= 1)		// ucCntF try
		{
			ucF = s_aucUcFiValue[ucCntF];
			while(ucCntD >= 1)	// ucCntD try
			{
				ucD = s_aucUcDiValue[ucCntD];
				HxLOG_Print ("PPS T(%d) F(%d) D(%d) Dcnt(%d)\n", ucT, ucF, ucD, ucCntD);
				nErr = local_UcSc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
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
					HxLOG_Print ("local_VdSc_NegotiateByValueOrder\n");
					HxLOG_Print ("stScParam.eProtocol = %d\n", stScParam.eProtocol);
					HxLOG_Print ("stScParam.ulClock = %d\n", stScParam.ulClock);
					HxLOG_Print ("stScParam.ulBaudrate = %d\n", stScParam.ulBaudrate);
					HxLOG_Print ("stScParam.ucFI = %d\n", stScParam.ucFI);
					HxLOG_Print ("stScParam.ucDI = %d\n", stScParam.ucDI);
					HxLOG_Print ("stScParam.ucN = %d\n", stScParam.ucN);
					HxLOG_Print ("stScParam.ucCWI = %d\n", stScParam.ucCWI);
					HxLOG_Print ("stScParam.ucBWI = %d\n", stScParam.ucBWI);
#endif

					nErr = PAL_SC_SetParams(hSci, &stScParam);
					if (nErr)
					{
						HxLOG_Print ("[local_cxsc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
					}
					HxLOG_Print ("T(%d) F(%d) D(%d)", ucT, ucF, ucD);
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
		HxLOG_Print ("MULTIPLE T\n");
		for (i=0; i<2; i++)
		{
			if (i == 0)
				ucT = p->ucTFirst;
			else
				ucT = p->ucT;

			while (ucCntF >= 1)		// ucCntF try
			{
				ucF = s_aucUcFiValue[ucCntF];
				while (ucCntD >= 1)	// ucCntD try
				{
					ucD = s_aucUcDiValue[ucCntD];
					HxLOG_Print ("PPS T(%d) F(%d) D(%d)\n", ucT, ucF, ucD);
					nErr = local_UcSc_RequestPps(hSci, ucT, ucF, ucD, pstDefaultParam);
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

						nErr = PAL_SC_SetParams(hSci, &stScParam);
						if (nErr)
						{
							HxLOG_Error ("[local_cxsc_NegotiateByValueOrder][DI_SC_SetParams] err<%x>\n", nErr);
						}
						HxLOG_Print ("T(%d) F(%d) D(%d)", ucT, ucF, ucD);
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


static HERROR local_UcSc_Negotiate(PalSc_Handle_t hSci, UcScAtrInfo *pstAtrInfo)
{
	HUINT8	ucWI;
	HUINT8	ucBWI;
	HUINT8			ucT;
	PalSc_Param_t	stScParam;
#if defined(CONFIG_MW_CAS_IRDETO_SMC_NEGOTIATE_MODE)
	PalSc_Param_t	*pstDefaultParams;
	HERROR			nErr = ERR_OK;
#endif
	if (pstAtrInfo == NULL)
	{
		HxLOG_Error("target is null ...\n");
		return ERR_FAIL;
	}

	VK_memset(&stScParam, 0x00, sizeof(PalSc_Param_t));

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

	stScParam.eProtocol = local_UcSc_ConvertProtocol(ucT);

#if defined(CONFIG_MW_CAS_IRDETO_SMC_NEGOTIATE_MODE)

	if(stScParam.eProtocol == ePAL_SC_PROTOCOL_T0)
	{
		pstDefaultParams = &s_stUcScInParamT0;
	}
	else
	{
		pstDefaultParams = &s_stUcScInParamT14;
	}

	if (pstAtrInfo->bSpecificMode)
	{
		HxLOG_Error("SPECIFIC\n");
		if (pstAtrInfo->bImplicitlyDefined)
		{
			HxLOG_Error("IMPLICIT\n");
			goto Atr_Parameters;
		}
		else
		{
			HxLOG_Error("EXPLICIT\n");
			goto Atr_Parameters;
		}
	}
	else	// negotiable mode
	{
		HxLOG_Print ("NEGOTIABLE\n");

		if (pstAtrInfo->bExistTA1 == TRUE) // there should be TA1 for starting PPS
		{
			// check if default Fd, Dd
			if ( (pstAtrInfo->ucT == pstAtrInfo->ucTFirst)
				&& ((pstAtrInfo->ucFI == 1 && pstAtrInfo->ucDI == 1)
				|| (pstAtrInfo->ucFI == 0 && pstAtrInfo->ucDI == 1)) )		// only 1 protocol & Fd Dd : no need to proceed PPS
					goto Atr_Parameters;

			HxLOG_Print ("START NEGOTIABLE PPS !!!!!!\n");
			nErr = local_UcSc_NegotiateByValueOrder(hSci, pstAtrInfo, ucWI, ucBWI, pstDefaultParams);
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
		HxLOG_Print ("SET Default FI DI\n");

		nErr = PAL_SC_SetParams(hSci, pstDefaultParams);
		if (nErr != ERR_OK)
		{
			HxLOG_Error ("err<%x>\n", nErr);
		}
		return ERR_OK;
	}

	Atr_Parameters:
#endif
	/* valid check F, D here because we need to compare with original Fn, Dn value above */
	if (pstAtrInfo->ucFI == 0 && pstAtrInfo->ucDI == 0)
	{
		HxLOG_Error("(stScParam.ucFI == 0 && stScParam.ucDI == 0 )\n\n\n");
		pstAtrInfo->ucFI = 1;
		pstAtrInfo->ucDI = 1;
	}

	if (pstAtrInfo->ucDI < 1 || pstAtrInfo->ucDI > 9)
	{
		HxLOG_Error("(stScParam.ucDI < 1 ||stScParam.ucDI > 9) stScParam.ucDI = %d\n\n\n", pstAtrInfo->ucDI);
		pstAtrInfo->ucDI = 1;
	}

	stScParam.ulClock		= pstAtrInfo->ulMinClock;
	stScParam.ulBaudrate	= pstAtrInfo->ulBaudrate;
	stScParam.ucFI			= pstAtrInfo->ucFI;
	stScParam.ucDI			= pstAtrInfo->ucDI;
	stScParam.ucN			= pstAtrInfo->ucN;
	stScParam.ucCWI 		= ucWI;
	stScParam.ucBWI 		= ucBWI;

	if (PAL_SC_SetParams(hSci, &stScParam) != ERR_OK)
	{
		HxLOG_Error("[local_VdSc_Negotiate] error in PAL_SC_SetParams\n");
		return ERR_FAIL;
	}
	return ERR_OK;

}

#else
static HERROR local_UcSc_Negotiate(PalSc_Handle_t hSci, UcScAtrInfo *pstAtrInfo)
{
	HUINT8 	ucWI;
	HUINT8 	ucBWI;
	HUINT8			ucT;
	PalSc_Param_t 	stScParam;
	PalSc_Param_t	*pstDefaultParams;
	HERROR			nErr = ERR_OK;

	if (pstAtrInfo == NULL)
	{
		HxLOG_Error("target is null ...\n");
		return ERR_FAIL;
	}

	VK_memset(&stScParam, 0x00, sizeof(PalSc_Param_t));

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

	stScParam.eProtocol = local_UcSc_ConvertProtocol(ucT);

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
		HxLOG_Error("SPECIFIC\n");
		if (pstAtrInfo->bImplicitlyDefined)
		{
			HxLOG_Error("IMPLICIT\n");
			goto Atr_Parameters;
		}
		else
		{
			HxLOG_Error("EXPLICIT\n");
			goto Atr_Parameters;
		}
	}
	else	// negotiable mode
	{
		HxLOG_Print ("NEGOTIABLE\n");

		if (pstAtrInfo->bExistTA1 == TRUE) // there should be TA1 for starting PPS
		{
			// check if default Fd, Dd
			if ( (pstAtrInfo->ucT == pstAtrInfo->ucTFirst)
				&& ((pstAtrInfo->ucFI == 1 && pstAtrInfo->ucDI == 1)
				|| (pstAtrInfo->ucFI == 0 && pstAtrInfo->ucDI == 1)) )		// only 1 protocol & Fd Dd : no need to proceed PPS
					goto Atr_Parameters;

			HxLOG_Print ("START NEGOTIABLE PPS !!!!!!\n");
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


		nErr = PAL_SC_SetParams(hSci, pstDefaultParams);
		if (nErr != ERR_OK)
		{
			HxLOG_Error(DBG_ASSERT, ("err<%x>\n", nErr);
		}
		return ERR_OK;
	}
#endif


Atr_Parameters:
	/* valid check F, D here because we need to compare with original Fn, Dn value above */
	if (pstAtrInfo->ucFI == 0 && pstAtrInfo->ucDI == 0)
	{
		HxLOG_Error("(stScParam.ucFI == 0 && stScParam.ucDI == 0 )\n\n\n");
		pstAtrInfo->ucFI = 1;
		pstAtrInfo->ucDI = 1;
	}

	if (pstAtrInfo->ucDI < 1 || pstAtrInfo->ucDI > 9)
	{
		HxLOG_Error("(stScParam.ucDI < 1 ||stScParam.ucDI > 9) stScParam.ucDI = %d\n\n\n", pstAtrInfo->ucDI);
		pstAtrInfo->ucDI = 1;
	}

	stScParam.ulClock 		= pstAtrInfo->ulMinClock;
	stScParam.ulBaudrate 	= pstAtrInfo->ulBaudrate;
	stScParam.ucFI 			= pstAtrInfo->ucFI;
	stScParam.ucDI 			= pstAtrInfo->ucDI;
	stScParam.ucN 			= pstAtrInfo->ucN;
	stScParam.ucCWI 		= ucWI;
	stScParam.ucBWI 		= ucBWI;

	if (PAL_SC_SetParams(hSci, &stScParam) != ERR_OK)
	{
		HxLOG_Error("[local_VdSc_Negotiate] error in PAL_SC_SetParams\n");
		return ERR_FAIL;
	}
	return ERR_OK;

}
#endif


static void local_UcSc_pfnCbScStatus0(PalSc_Handle_t hSci, HUINT8 ucSlotId, PalSc_CardEvent_e eStatus)
{
	CAS_IR_BASE_MSG 	*pMsg;
	HxLOG_Print ("ucSlotId(0x%X), eStatus(0x%X)\n", ucSlotId, eStatus) ;

	switch(eStatus)
	{
		case ePAL_SC_CARDEVENT_EXTRACTED:
			HxLOG_Print (" SmartCard Extracted 0 \n");
#ifdef	IRPLAT_SYS_SC_EXTRACT_MSG
			pMsg = (CAS_IR_BASE_MSG*)CA_MEM_Alloc(sizeof(CAS_IR_BASE_MSG));
			VK_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
			pMsg->msg.SC.ucSlotID	= ucSlotId;
			pMsg->msg.SC.ResourceID	= g_UcDrvScInfo[ucSlotId].SmartcardID;
			pMsg->msg.SC.ReferenceID= g_UcDrvScInfo[ucSlotId].SmartcardID;
			CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_CARD_OUT, (void*)pMsg);
#else
			if (g_UcDrvScInfo[ucSlotId].ScOpen.pfnStatusNotify != NULL)
			{
				if(g_UcDrvScInfo[ucSlotId].CardStatus != UC_SC_CARD_OUT)
				{
					local_uc_SC_Callback(ucSlotId, UC_SC_CARD_OUT, (uc_buffer_st *)NULL);
					g_UcDrvScInfo[ucSlotId].CardStatus = UC_SC_CARD_OUT;
#if 0 //defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
					CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT);
#endif
				}
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
				g_UcDrvScInfo[ucSlotId].CardPhysicalStatus = SCI_SC_NOT_PRESENT;
#endif
			}
#endif

#if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
			CAS_IRUC_MGR_Notify_InstanceChanged(ucSlotId, NULL, 0, FALSE);
#endif // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)

#if defined (CONFIG_OP_ZIGGO_RFVOD) || defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
			MWC_UTIL_PostMsgToAp(EVT_RFVOD_SC_OUT, 0, 0, 0, 0);
#endif
			break;

		case ePAL_SC_CARDEVENT_INSERTED:
			VK_Print("SC Inserted 0\n");
			HxLOG_Print ("Card Inserted(cb in)\n");
			pMsg = (CAS_IR_BASE_MSG *)CA_MEM_Alloc(sizeof(CAS_IR_BASE_MSG));
			VK_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
			pMsg->msg.SC.ucSlotID = ucSlotId;
			pMsg->msg.SC.ResourceID  = g_UcDrvScInfo[ucSlotId].SmartcardID;
			pMsg->msg.SC.ReferenceID = g_UcDrvScInfo[ucSlotId].SmartcardID;
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
			g_UcDrvScInfo[ucSlotId].CardPhysicalStatus = ePAL_SC_CARDSTATUS_PRESENT;
#endif
			CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_CARD_IN, (void *)pMsg);
#if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
			CAS_IRUC_MGR_Notify_InstanceChanged(ucSlotId, NULL, 1, TRUE);	
#endif // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
#if 0 //defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
			CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_IN);
#endif

#if defined (CONFIG_OP_ZIGGO_RFVOD) || defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
			MWC_UTIL_PostMsgToAp(EVT_RFVOD_SC_IN, 0, 0, 0, 0);
#endif
#if 1 // ciyoon 나중에 수정 ...
			UniversalClientSPI_Smartcard_Reset(0);
#endif

			break;
		default :
			HxLOG_Print ("Unknown Message, ucSlotId(0x%X), eStatus(0x%X)\n", ucSlotId, eStatus) ;
			break;
	}

}



/********************************************************************
*	function name :
*	input :
*	output :
*	description :
********************************************************************/
static void local_UcSc_pfnCbScStatus1(PalSc_Handle_t hSci, HUINT8 ucSlotId, PalSc_CardEvent_e eStatus)
{
	CAS_IR_BASE_MSG 	*pMsg;
	HUINT32				ulIdx = 1;
	HxLOG_Print("ucSlotId(0x%X), eStatus(0x%X)\n", ucSlotId, eStatus) ;

	switch(eStatus)
	{
		case ePAL_SC_CARDEVENT_EXTRACTED:
			HxLOG_Print (" SmartCard Extracted 1 \n");
#ifdef	IRPLAT_SYS_SC_EXTRACT_MSG
			pMsg = (CAS_IR_BASE_MSG*)CA_MEM_Alloc(sizeof(CAS_IR_BASE_MSG));
			VK_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
			pMsg->msg.SC.ucSlotID	= ucSlotId;
			pMsg->msg.SC.ResourceID	= g_UcDrvScInfo[ucSlotId].SmartcardID;
			pMsg->msg.SC.ReferenceID= g_UcDrvScInfo[ucSlotId].SmartcardID;
			CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_CARD_OUT, (void*)pMsg);
#else
			if (g_UcDrvScInfo[ucSlotId].ScOpen.pfnStatusNotify != NULL)
			{
				if(g_UcDrvScInfo[ucSlotId].CardStatus != UC_SC_CARD_OUT)
				{
					local_uc_SC_Callback(ucSlotId, UC_SC_CARD_OUT, (uc_buffer_st *)NULL);
					g_UcDrvScInfo[ucSlotId].CardStatus = UC_SC_CARD_OUT;
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
					CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT);
#endif
				}
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
				g_UcDrvScInfo[ucSlotId].CardPhysicalStatus = SCI_SC_NOT_PRESENT;
#endif
			}
#endif

#if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
			CAS_IRUC_MGR_Notify_InstanceChanged(ucSlotId, NULL, 0, FALSE);
#endif // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
			break;

		case ePAL_SC_CARDEVENT_INSERTED:
			VK_Print("SC Inserted 1\n");
			HxLOG_Print ("Card Inserted(cb in)\n");
			pMsg = (CAS_IR_BASE_MSG *)CA_MEM_Alloc(sizeof(CAS_IR_BASE_MSG));
			VK_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
			pMsg->msg.SC.ucSlotID 		= ucSlotId;
			pMsg->msg.SC.ResourceID 	= g_UcDrvScInfo[ucSlotId].SmartcardID;
			pMsg->msg.SC.ReferenceID 	= g_UcDrvScInfo[ucSlotId].SmartcardID;
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
			g_UcDrvScInfo[ucSlotId].CardPhysicalStatus = ePAL_SC_CARDSTATUS_PRESENT;
#endif
			CAS_IR_SendMessage(eIR_MSG_CLASS_CAS, IR_CATASK_CARD_IN, (void *)pMsg);

#if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
			CAS_IRUC_MGR_Notify_InstanceChanged(ucSlotId, NULL, 1, TRUE);
#endif // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
#if 0 //defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
			CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_IN);
#endif

#if 1 // ciyoon 나중에 수정 ...
			if(HUMAX_VD_SC_SLOT_COUNT > 1)
			{
					UniversalClientSPI_Smartcard_Reset(ulIdx);	// smartcard 1개
			}
#endif

			break;
		default :
			HxLOG_Print ("Unknown Message, ucSlotId(0x%X), eStatus(0x%X)\n", ucSlotId, eStatus);
			break;
	}

}


/* *
  *   Universial Client Smartcard를 사용하기전에 초기화시킨다.
  *   Humax MW에서 먼저 초기화 되어야 한다. // ?
  */
uc_result CAS_IRUC_SPI_SC_Init(void)
{
	HUINT32		i;
	HERROR		hErr;

	HxLOG_Print ("\n") ;

	VK_memset(g_UcDrvScInfo, 0, sizeof(UC_DRV_SC_INFO) * HUMAX_VD_SC_SLOT_COUNT);

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		g_UcDrvScInfo[i].SmartcardID	= UC_REF_ID_ERROR;
		g_UcDrvScInfo[i].CardStatus		= 0xFFFFFFFF;

		if( VK_OK != VK_SEM_Create(&(g_UcDrvScInfo[i].SemaphoreID), "g_UcDrvScInfo", VK_SUSPEND_FIFO)   )
		{
			HxLOG_Error("semaphore create fail on %d slot\n", i);
			return UC_ERROR_OS_FAILURE;
		}
	}

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if(i == 0)
			hErr = PAL_SC_RegisterCbFunc(0 ,eDxCAS_GROUPID_IR, local_UcSc_pfnCbScStatus0, &(g_UcDrvScInfo[0].hIRSci));
		else if(i == 1)
			hErr = PAL_SC_RegisterCbFunc(1, eDxCAS_GROUPID_IR, local_UcSc_pfnCbScStatus1, &(g_UcDrvScInfo[1].hIRSci));
		else
			hErr = ERR_FAIL;

		if( hErr != ERR_OK )
		{
			HxLOG_Error("SCI_RegisterCbFunc slot #%d\n", i);
			return UC_ERROR_OS_FAILURE;
		}
	}

	return UC_ERROR_SUCCESS;

}


/**
 * Smartcard method to initialize the smartcard device.
 *
 * This method is called during the initialization of Cloaked CA Agent.
 *
 * @param[out] pSmartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 * @param[in] pSCOpenData pointer of data transmitted to smartcard driver, the pointer
 *            points to structure ::uc_sc_open_parameter.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Smartcard_Open(uc_uint32 *pSmartcardID, uc_sc_open_parameter *pSCOpenData)
{// CCA로 SmartcardID를 보내준다. CCA에서는 이 ID를 가지고  resource id와 mapping 하여 사용된다.
// uc_sc_status_notify_fn을 넣어주고, uc_smartcard_handle도 넣어준다.
	HUINT32			i;
	uc_sc_open_parameter 	*pstScOpen;

	HxLOG_Print ("[%s] pSmartcardID(0x%X)\n",__FUNCTION__ , *((HUINT32 *)pSmartcardID) );

	pstScOpen = pSCOpenData;

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		// Resource 중에서 index가 앞인 것이 먼저 할당된다.
		if( g_UcDrvScInfo[i].SmartcardID == UC_REF_ID_ERROR ) // ciyoon To do VD_REF_ID_ERROR 이름 변경 ..
		{
			g_UcDrvScInfo[i].SmartcardID = i;	/*define reference id as resource id*/ // ciyoon  To do ref, res 를 smart card id로 ?...
			g_UcDrvScInfo[i].ScOpen.pfnStatusNotify = pstScOpen->pfnStatusNotify;
			g_UcDrvScInfo[i].ScOpen.smartcardHandle = pstScOpen->smartcardHandle; // stOpenData.smartcardHandle=ctx->wScellval; from tc_sc_operation.c by ciyoon
			break;
		}
	}

	if(HUMAX_VD_SC_SLOT_COUNT == i)
	{
		HxLOG_Print ("[%s]short of resources (0x%X) \n",__FUNCTION__ , (HUINT32)i );
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	*pSmartcardID = g_UcDrvScInfo[i].SmartcardID;
	HxLOG_Print ("[%s] UniversalClientSPI_Smartcard_Open success (0x%X) \n",__FUNCTION__ , (HUINT32)i );
	return UC_ERROR_SUCCESS;
}

/**
 * Smartcard method to close the smartcard device.
 *
 * This method is called when closing the Cloaked CA Agent.
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Smartcard_Close(uc_uint32 smartcardID)
{
	HUINT32		i = 0;

	HxLOG_Print ("\n") ;

	if ( smartcardID >= HUMAX_VD_SC_SLOT_COUNT) {
		HxLOG_Print ("our of ranges(0x%X) \n", (HUINT32)smartcardID );
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	g_UcDrvScInfo[i].SmartcardID = UC_REF_ID_ERROR;
	g_UcDrvScInfo[i].ScOpen.pfnStatusNotify = NULL;
	g_UcDrvScInfo[i].ScOpen.smartcardHandle = 0;

	HxLOG_Print("[%s] smartcardID = %d\n",__FUNCTION__, smartcardID);
	return UC_ERROR_SUCCESS;
}

/**
 * Smartcard method to reset the smartcard device.
 *
 * This method is called when Cloaked CA Agent wants to reset the smartcard device.
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 */
uc_result UniversalClientSPI_Smartcard_Reset(uc_uint32 smartcardID)
{
	/* the smartcard is a reference id. */
	uc_result		ret = UC_ERROR_DRIVER_FAILURE;
	HUINT8			i;
	HUINT32			ulSlotID, ulErr, resCheckATR;
	PalSc_Param_t	*pstInScParam;
	HUINT8			*pvInfo;

	HUINT8			ucResetTryCount=0;

	PalSc_ResetType_e	eScResetType = ePAL_SC_RESETTYPE_COLD;

	HxLOG_Print ("[%s] wRefId = 0x%X\n",__FUNCTION__ , (HUINT32)smartcardID);
	HxLOG_Print("[%s] smartcardID = %d\n",__FUNCTION__, smartcardID);

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if( g_UcDrvScInfo[i].SmartcardID == smartcardID )
		{
			break;
		}
	}

	if( i == HUMAX_VD_SC_SLOT_COUNT )
	{
		HxLOG_Print("[%s]No smartcardID(smartcardID= 0x%X)\n",__FUNCTION__ , (HUINT32)smartcardID);
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	ulSlotID = smartcardID; // ciyoon  Smartcard ID와 slot id는 동일 하다.

	// Multi CAS 에서는 SC MGR를 통해 업데이트 한다. Physical Status는 별도로 업데이트 된다.
#if (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	PAL_SC_GetCardStatus(g_UcDrvScInfo[ulSlotID].hIRSci, &(g_UcDrvScInfo[ulSlotID].CardPhysicalStatus));
#else
	// in case of multi-cas, it will be updated by its event from sc manager
#endif

	if( g_UcDrvScInfo[ulSlotID].CardPhysicalStatus != ePAL_SC_CARDSTATUS_PRESENT )
	{
		HxLOG_Error("[NO CARD FOR RESET!!]\n");

		if(g_UcDrvScInfo[ulSlotID].CardStatus != UC_SC_CARD_OUT)
		{
			local_uc_SC_Callback(ulSlotID, UC_SC_CARD_OUT, (uc_buffer_st *)NULL);
			g_UcDrvScInfo[ulSlotID].CardStatus = UC_SC_CARD_OUT;
		}
		return UC_ERROR_DRIVER_FAILURE;
	}

	HxLOG_Print("[%s] [CARD EXIST FOR RESET!!]\n",__FUNCTION__ );
	VK_SEM_Get(g_UcDrvScInfo[ulSlotID].SemaphoreID);

DO_RESET:
	VK_memset(g_UcDrvScInfo[ulSlotID].aucATR, 0, sizeof(g_UcDrvScInfo[ulSlotID].aucATR) );

#if defined (IRPLAT_SYS_SC_T14_FIRST) || defined(CONFIG_MW_CAS_IRDETO_SECURE) /* 생산용 SST TEST를 위해서 필요한 config */
		if(ucResetTryCount < T14_RETRY_COUNT)
		{
			HxLOG_Error("T=0, ucResetTryCount=%d\n", ucResetTryCount);
			pstInScParam = &s_stUcScInParamT14;
		}
		else
		{
			HxLOG_Error("T=14, ucResetTryCount=%d\n", ucResetTryCount);
			pstInScParam = &s_stUcScInParamT0;
		}
#else
		if(ucResetTryCount < T0_RETRY_COUNT)
		{
			HxLOG_Error("T=0, ucResetTryCount=%d\n", ucResetTryCount);
			pstInScParam = &s_stUcScInParamT0;
		}
		else
		{
			HxLOG_Error("T=14, ucResetTryCount=%d\n", ucResetTryCount);
			pstInScParam = &s_stUcScInParamT14;
		}
#endif

	ucResetTryCount++;

	ulErr = PAL_SC_Reset( g_UcDrvScInfo[ulSlotID].hIRSci,
						g_UcDrvScInfo[ulSlotID].aucATR,
						&(g_UcDrvScInfo[ulSlotID].ulAtrLen),
						eScResetType,
						pstInScParam);

	if(ulErr == ERR_OK)
	{
#if defined(CONFIG_DEBUG)
		PrintUcScATR ("\tATR DATA = ");
		for(i=0; i< g_UcDrvScInfo[ulSlotID].ulAtrLen; i++)
		{
			PrintUcScATR("%02X ", g_UcDrvScInfo[ulSlotID].aucATR[i] );
		}
		PrintUcScATR("\n");
#endif
		VK_memset32(&s_stUcAtrInfo, 0, sizeof(UcScAtrInfo));

		ulErr = local_UcSc_ParseAtr( g_UcDrvScInfo[ulSlotID].aucATR, g_UcDrvScInfo[ulSlotID].ulAtrLen, &s_stUcAtrInfo);

		if (ulErr == ERR_OK)
		{
			HxLOG_Error("Parse atr, Ok\n");
			ulErr = local_UcSc_Negotiate( g_UcDrvScInfo[ulSlotID].hIRSci, &s_stUcAtrInfo);
			if (ulErr == ERR_OK)
			{
				HxLOG_Error("Negotiate Ok \n");
			}
			else
			{
				HxLOG_Error("Negotiate error = %d\n", ulErr);
			}
		}
		else
		{
			HxLOG_Error("Parse atr, ulErr = %d\n", ulErr);
		}
	}
	else
	{
		HxLOG_Error("Reset error occured. (%d)\n", ulErr);
	}

	if( (ERR_OK != ulErr ) &&  (ucResetTryCount < (T0_RETRY_COUNT+ T14_RETRY_COUNT) ))
		goto DO_RESET;

	if(ERR_OK != ulErr )
	{
		HxLOG_Print("Reset : ulErr(0x%X)\n", ulErr);
		PAL_SC_GetCardStatus(g_UcDrvScInfo[ulSlotID].hIRSci, &(g_UcDrvScInfo[ulSlotID].CardPhysicalStatus));
		if( g_UcDrvScInfo[ulSlotID].CardPhysicalStatus == ePAL_SC_CARDSTATUS_PRESENT )
		{
			local_uc_SC_Callback(ulSlotID, UC_SC_CARD_IN_ERROR, (uc_buffer_st *)NULL);
			g_UcDrvScInfo[ulSlotID].CardStatus = UC_SC_CARD_IN_ERROR;
			g_UcDrvScInfo[ulSlotID].ErrorReason = UC_SC_HARDWARE_ERROR;
		}
		else
		{
			if(g_UcDrvScInfo[ulSlotID].CardStatus != UC_SC_CARD_OUT)
			{
				local_uc_SC_Callback(ulSlotID, UC_SC_CARD_OUT, (uc_buffer_st *)NULL);
				g_UcDrvScInfo[ulSlotID].CardStatus = UC_SC_CARD_OUT;
			}
			g_UcDrvScInfo[ulSlotID].ErrorReason = 0xFFFFFFFF;
		}
	}
	else
	{
	/* reset success */
	#if defined(CONFIG_MW_CAS_IRDETO_SMC_NEGOTIATE_MODE)
		resCheckATR = eHUC_OK;
	#else
		resCheckATR = local_UcSc_CheckATR( ulSlotID, g_UcDrvScInfo[ulSlotID].aucATR, g_UcDrvScInfo[ulSlotID].ulAtrLen, &g_UcDrvScInfo[ulSlotID].Protocol );
	#endif

		HxLOG_Print("Check ATR ret=%d\n", resCheckATR);

		if ( resCheckATR == eHUC_OK )  /* work is done by here */
		{
			uc_smartcard_ATR_info *tmpAtrInfo = NULL;
			/* Notify Function으로 CCA 보낼 Data를 만든다. uc_buffer_st data structure 이용 */
			/* 메모리 할당 및 size 표시 */
			pvInfo = CA_MEM_Alloc( sizeof(uc_buffer_st) + sizeof(uc_smartcard_ATR_info) + g_UcDrvScInfo[ulSlotID].ulAtrLen);
			/*1. uc_buffer_st 2. uc_smartcard_ATR_info 3. ATR payload*/

			/* 1. uc_buffer_st */
			((uc_buffer_st *)pvInfo)->bytes  = pvInfo + sizeof(uc_buffer_st);
			((uc_buffer_st *)pvInfo)->length = sizeof(uc_smartcard_ATR_info) + g_UcDrvScInfo[ulSlotID].ulAtrLen ;

			/* 2. uc_smartcard_ATR_info */
			tmpAtrInfo = (uc_smartcard_ATR_info *) ((uc_buffer_st *)pvInfo)->bytes;
			tmpAtrInfo->ATRLength = g_UcDrvScInfo[ulSlotID].ulAtrLen;
			tmpAtrInfo->ATRData	  = pvInfo + sizeof(uc_buffer_st) +  sizeof(uc_smartcard_ATR_info);

			/* 3. ATR payload  :: copy ATR payload  */
			VK_memcpy(tmpAtrInfo->ATRData, g_UcDrvScInfo[ulSlotID].aucATR, g_UcDrvScInfo[ulSlotID].ulAtrLen);

			if(g_UcDrvScInfo[ulSlotID].CardStatus != UC_SC_CARD_IN)
			{
				local_uc_SC_Callback(ulSlotID, UC_SC_CARD_IN, (uc_buffer_st *)pvInfo);
				g_UcDrvScInfo[ulSlotID].CardStatus = UC_SC_CARD_IN;
			}
			CA_MEM_Free(pvInfo);
			g_UcDrvScInfo[ulSlotID].ErrorReason = 0xFFFFFFFF;

			ret =  UC_ERROR_SUCCESS;
		}
		else if( resCheckATR == eHUC_ERR_INVALID_CARD )
		{
			local_uc_SC_Callback(ulSlotID, UC_SC_CARD_IN_ERROR, (uc_buffer_st *)NULL);
			g_UcDrvScInfo[ulSlotID].ErrorReason = UC_SC_ATR_INVALID_FORMAT;
			g_UcDrvScInfo[ulSlotID].CardStatus = UC_SC_CARD_IN_ERROR;
		}
		else if( resCheckATR == eHUC_ERR_NOT_IRDETOCARD )
		{
			local_uc_SC_Callback(ulSlotID, UC_SC_CARD_IN_ERROR, (uc_buffer_st *)NULL);
			g_UcDrvScInfo[ulSlotID].CardStatus = UC_SC_CARD_IN_ERROR;
			g_UcDrvScInfo[ulSlotID].ErrorReason = UC_SC_ATR_INVALID_HISTORY;
		}
		else
		{
			local_uc_SC_Callback(ulSlotID, UC_SC_CARD_IN_ERROR, (uc_buffer_st *)NULL);
			g_UcDrvScInfo[ulSlotID].CardStatus = UC_SC_CARD_IN_ERROR;
			g_UcDrvScInfo[ulSlotID].ErrorReason = UC_SC_HARDWARE_ERROR;
		}

	}

	VK_SEM_Release(g_UcDrvScInfo[ulSlotID].SemaphoreID);
	return ret /*UC_ERROR_SUCCESS*/;
}

/**
 * Smartcard method to communicate with the smartcard device
 *
 * This method is called when Cloaked CA Agent wants to read/write data from/to the smartcard device
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses this ID
 *            to map to a smartcard reader slot.
 * @param[in] headerLen header length of the APDU.
 * @param[in] payloadLen payload length of the APDU.
 * @param[in] pSendBuffer buffer for the  APDU.
 * @param[out] pRecvDataLen received buffer length.
 * @param[out] pRecvBuffer received buffer.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Smartcard_Communicate(uc_uint32 smartcardID, uc_uint32 headerLen, uc_uint32 payloadLen,uc_byte *pSendBuffer, uc_uint32 *pRecvDataLen, uc_byte *pRecvBuffer )
{// from tc_sc_operation VD_IO(VD_DRIVER_SMARTCARD,wRefId, VD_SC_COMMUNICATE, wWriteLength, bWriteBuffer, &wRxDataLength, &bRxData);
	HUINT8			i;
	HUINT32			ulSlotID, ulErr;
	uc_result		result = UC_ERROR_SUCCESS;
	HUINT8			*pWriteData;
	HUINT32			WriteSize;
	HUINT32			ReadSize;
//	uc_sc_write_st	*pSendBuffer;
	HUINT8			*p = (HUINT8 *)pSendBuffer;
// ciyoon	uc_sc_read_error_st	sc_read_error_st;  // <-- 사라짐 ... /*eError = ( (vd_sc_read_error_st *)bRxData )->eError;*/ from tc_sc_operation
	HINT32			writeRetry;

	HxLOG_Print ("\n") ;
	HxLOG_Print("[%s] smartcardID = %d\n",__FUNCTION__ , smartcardID);

	// prevent fix(added)
	if (pSendBuffer == NULL)
	{
		HxLOG_Critical("[%s] Error !!!!!!\n", __FUNCTION__,__LINE__);
		return UC_ERROR_NULL_PARAM;  // ciyoon
	}

#ifdef UC_SC_IO_DEBUG
		PrintUcScCom("\t\t[UC_SC] ++UniversalClientSPI_Smartcard_Communicate : smartcardID(0x%X), headerLen(0x%X), payloadLen(0x%X), pSendBuffer@(0x%X), pRecvDataLen@(0x%X), pRecvBuffer@(0x%X)\n", 
		            smartcardID, headerLen, payloadLen, pSendBuffer, pRecvDataLen, pRecvBuffer);
#endif

	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if( g_UcDrvScInfo[i].SmartcardID == smartcardID )
		{
			break;
		}
	}

	if( i >= HUMAX_VD_SC_SLOT_COUNT )
	{
		HxLOG_Error("SmartcardID\n");
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	ulSlotID = g_UcDrvScInfo[i].SmartcardID;
#ifdef VD_SC_IO_DEBUG
	HxLOG_Print("SLOT #%d\n", ulSlotID);
#endif

#if (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	if(ERR_OK != PAL_SC_GetCardStatus(g_UcDrvScInfo[ulSlotID].hIRSci, &(g_UcDrvScInfo[ulSlotID].CardPhysicalStatus) ) )
	{
		HxLOG_Error("NO CARD FOR COMMUNICATE!!\n");
		// sc_read_error_st.eError = UC_SC_HARDWARE_ERROR; // not used from tc_sc_operation by ciyoon
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}
#endif

	if( g_UcDrvScInfo[ulSlotID].CardPhysicalStatus == ePAL_SC_CARDSTATUS_PRESENT )
	{
#ifdef VD_SC_IO_DEBUG
		HxLOG_Print("[CARD EXIST FOR COMMUNICATE!!]\n");
#endif
		pWriteData = (HUINT8 *)CA_MEM_Alloc(headerLen + payloadLen);
		if (pWriteData == NULL)
		{
			HxLOG_Error("pWriteData == NULL \n");
			return UC_ERROR_OUT_OF_MEMORY;
		}
		VK_memcpy(pWriteData, (HUINT8 *)(p), headerLen + payloadLen); // bSkipHeadPayloadLength is zero from tc_sc_operation by ciyoon
		WriteSize = headerLen + payloadLen;

		PrintUcScCom("SC_SEM :TRY ---\n");
		VK_SEM_Get(g_UcDrvScInfo[ulSlotID].SemaphoreID);
		PrintUcScCom("SC_SEM :GOT\n");

#ifdef VD_SC_IO_DEBUG
	{
		HUINT8	j;

		PrintUcScCom("[WRITE DATA : SIZE(0x%X)]\n", WriteSize);
		for( j = 0; j < WriteSize; j++ )
		{
			PrintUcScCom("%02X ", pWriteData[j]);
			if( j % 16 == 15 )
				PrintUcScCom("\n");
		}
		PrintUcScCom("\n[END]\n");
	}
#endif
		writeRetry = 0;
		do
		{
			// TODO: jichoi. adjust ReadSize.
			ReadSize = 255;
			ulErr = PAL_SC_Transfer(g_UcDrvScInfo[ulSlotID].hIRSci, pWriteData, WriteSize, (HUINT8 *)pRecvBuffer, &ReadSize, ReadSize);
		} while ((ulErr || ReadSize == 0) && ++writeRetry < MAX_NUM_RETRY);

		if (g_UcDrvScInfo[ulSlotID].Protocol == 14)
		{
			/*Actual payload size + Header overload(size 8) + Check sum (size 1)*/
			/* data 넣는 방식이 이상하다. array 아니다 int 여야 한다.  */
			ReadSize = ((HUINT8*)pRecvBuffer)[7] + 8 + 1;
		}

#ifdef VD_SC_IO_DEBUG
	{
		HUINT8	j;

		PrintUcScCom("[READ DATA : SIZE(0x%X)]\n", ReadSize);
		for( j = 0; j < ReadSize; j++ )
		{
			PrintUcScCom("%02X ", ((HUINT8*)pRecvBuffer)[j]);
			if( j % 16 == 15 )
				PrintUcScCom("\n");
		}
		PrintUcScCom("\n[END]\n");
	}
#endif

		if(ulErr || ReadSize == 0)
		{
			HxLOG_Error("PAL_SC_Transfer (0x%X)\n", ulErr);
			// card가 빠진 경우와 timeout이 난 경우를 구별해줘야 할 필요가 있는지..
			// Multi CAS 에서는 SC MGR를 통해 업데이트 한다. Physical Status는 별도로 업데이트 된다.
#if (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
			PAL_SC_GetCardStatus(g_UcDrvScInfo[ulSlotID].hIRSci, &(g_UcDrvScInfo[ulSlotID].CardPhysicalStatus));
#endif
			if( g_UcDrvScInfo[ulSlotID].CardPhysicalStatus == ePAL_SC_CARDSTATUS_NOT_PRESENT )
			{
				//sc_read_error_st.eError = VD_SC_CARD_OUT_ERROR; // not used from tc_sc_operation by ciyoon
				result = UC_ERROR_WAIT_ABORTED;
			}
			else
			{
				HxLOG_Error("CardPhysicalStatus= %d\n", g_UcDrvScInfo[ulSlotID].CardPhysicalStatus);
				#if 0
				if (g_ucScHardreset[ulSlotID] == 0)
				{
					HxLOG_Error( DBG_ASSERT, ("코드 논리상 이론상 불가능한 에러 \n"));
					VdScAssert(1);
					g_ucScHardreset[ulSlotID] = 1;
					PAL_SC_Deactivate(g_IrDrvScInfo[ulSlotID].hIRSci);
				}
				#endif
				// sc_read_error_st.eError = VD_SC_TIME_OUT;  // not used from tc_sc_operation by ciyoon
				result = UC_ERROR_WAIT_ABORTED;
			}
		}
		else
		{
			*pRecvDataLen = (uc_uint32)ReadSize;
			result = UC_ERROR_SUCCESS;
		}

		CA_MEM_Free(pWriteData);
		VK_SEM_Release(g_UcDrvScInfo[ulSlotID].SemaphoreID);
	}
	else
	{
		HxLOG_Error("NO CARD FOR COMMUNICATE!!\n");
		//sc_read_error_st.eError = VD_SC_CARD_OUT_ERROR; // not used from tc_sc_operation by ciyoon
		result = UC_ERROR_WAIT_ABORTED;
	}

#ifdef VD_SC_IO_DEBUG
		PrintScCom("\t\t[VD_SC] --local_VdSc_CommunicateCard : result(0x%X)\n", result);
#endif

	return result;
}

/** @} */ /* End of smartcardspi */




