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
/**
	@file     cx_core_sc.c
	@brief    cx_core_sc.c (CONAX SC)

	Description: . \n
	Module: MW/CAS/ CONAX \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <_svc_cas_mgr.h>
#include <util.h>
#include <dlib.h>

#include <cxmgr_action.h>
#include "../local_include/cxcore_sc.h"
#include "../local_include/cxcore_os.h"
#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_session.h"
#include "../local_include/cxcore_ecm.h"
#include "../local_include/cxcore_emm.h"
#include "../local_include/cxcore_nvm.h"
#include "../local_include/cxcore_ui.h"
#include "../local_include/cxcore_stream.h"
#if defined(CONFIG_MW_MM_PVR)
#include "../local_include/cxcore_pvr.h"
#endif
#if defined(CONFIG_MW_CAS_CONAX_CPM)
#include "../local_include/cxcore_cpm.h"
#include <pal_cipher.h>
#if defined(CONFIG_MW_CAS_CONAX_CPM_WITH_LPK)
#include <di_crypt.h>
#endif
#endif
#include <_svc_cas_sub_dev_api.h>

#include "../local_include/cx_sc.h"


/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines  	   *************************/
/*******************************************************************/
#define CX_APS_BIT_SHIFT					(14)
#define CX_EMI_BIT_SHIFT					(12)
#define CX_ICT_BIT_SHIFT					(11)
#define CX_REDIST_BIT_SHIFT					(10)
#define CX_RETENT_BIT_SHIFT					(7)
#define CX_TRICK_BIT_SHIFT					(4)
#define CX_MR_BIT_SHIFT						(1)
#define CX_DISABLE_ANALOG_BIT_SHIFT			(0)

#define CX_1BIT_MASK						(0x01)
#define CX_2BIT_MASK						(0x03)
#define CX_3BIT_MASK						(0x07)


#define CxSc_GetInParam(x)					(x)->aucWriteBuf[5]

#define CX_SC_NUMBER_OF_INSTANCE			1	// 일단 slot 1개만 고려 하지... 복잡혀..

// send command 중 error 발생시 최대 retry 회수
#define CX_SC_MAX_RETRY_SEND_CMD			3

// reset error시 최대 retry 수
#define CX_SC_MAX_RETRY_RESET				3

// Operation + (Query)
#if defined(CONFIG_MW_MM_PVR)
#define CX_SC_SEND_ECM_COUNT				2
#else
#define CX_SC_SEND_ECM_COUNT				1
#endif

// length of parameter
#define CX_PL_HostVer 						1
#define CX_PL_ResetSession 					1
#define CX_PL_CaPinIn 						5
#define CX_PL_CaPinChange			 		8
#define CX_PL_SignPinChange 				12
#define CX_PL_ReqPinStatus 					4
#define CX_PL_MatRatChange 					5
#define CX_PL_ReqOrderApdu 					10
#define CX_PL_ReqRetChanConfig 				0
#define CX_PL_ReqBlockSize					5
#define CX_PL_ReqCardNumber					0
#define CX_PL_ReqSequenceNumber				0
#define CX_PL_ReqRandomOctets				1
#define	CX_PL_PairingMech					2

// S/C 데이터를 읽어서 메뉴로 display 할때,
// 10개로 제한되어 있어서 100개로 늘려줌. spec 명시되어있음
#define CX_SC_MAX_PARAM						100
#define CX_SC_MAX_VALID_INPUT_PARAM			6

#define CX_ADDRESS_TYPE						0x23
#define CX_ADDRESS_LEN						0x7

#define CX_PRICE_CURRENCY_LEN				3


#if defined(CONFIG_DEBUG)
//#define DUMP_SMARTCARD_DATA
//#define DUMP_URI_DATA
//#define DUMP_PROTECTED_AND_PLAIN_TEXT
#endif


/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

typedef struct
{
	CxSc_OutputParam_t	eType;
	HUINT32				ulValue;
	HUINT8				aucData[256];	// PL이 1byte이므로 max value는 255이죠...
	HUINT32				ulIndex;
} CxParam_t;

typedef struct
{
	HUINT32		ulCount;
	CxParam_t	stParam[CX_SC_MAX_PARAM];
} CxParamList_t;

typedef struct
{
	HUINT8	ucPI;
	HUINT8	ucPL;		// 가변적인 경우는 minimum value로
	HUINT8	ucValidInParam[CX_SC_MAX_VALID_INPUT_PARAM];
} CxSc_ParamTable_t;

CxSc_ParamTable_t	s_stCxParamTable[]	=
{
	// 가변길이의 경우 0으로 표시함.
	{ eCxSc_CassVer, 				1, 	{ eCxSc_HostVer, 0,}}, 			//
	{ eCxSc_CaDescEmm,				0, 	{ eCxSc_Cat, 0,}},				// min 6
	{ eCxSc_SessionInfo,			1, 	{ eCxSc_HostVer, 0,}}, 			//
	{ eCxSc_Cw,						0, 	{ eCxSc_Ecm, 0,}},				// min 5
	{ eCxSc_CaSysId,				2, 	{ eCxSc_HostVer, 0,}}, 			//
	{ eCxSc_OrderReqApdu,			0, 	{ eCxSc_ReqOrderApdu, 0,}},		// min 0
	{ eCxSc_RetChanConfig,			0, 	{ eCxSc_ReqRetChanConfig, 0,}},	// min 2
	{ eCxSc_CountryIndicator,		2, 	{ eCxSc_HostVer, 0,}}, 			//
	{ eCxSc_MatRatLevel,			1, 	{ eCxSc_HostVer, 0,}}, 			//
	{ eCxSc_AccessStatus,			0, 	{ eCxSc_Ecm, 0,}},				// min 2
	{ eCxSc_SubscriptionStatus,		0, 	{ eCxSc_CaStatusSelect, 0,}},	// min 30
	{ eCxSc_OrderInfo,				0, 	{ eCxSc_Ecm, 0,}},				// min 6
	{ eCxSc_AcceptViewingInfo,		0, 	{ eCxSc_Ecm, 0,}},				// min 2
	{ eCxSc_EventStatus,			0, 	{ eCxSc_CaStatusSelect, 0,}},	// min 16
	{ eCxSc_TokenDebitInfo,			0, 	{ eCxSc_Ecm, 0,}},				// min 9
	{ eCxSc_PurseStatus,			0, 	{ eCxSc_CaStatusSelect, 0,}},	// min 6
	{ eCxSc_DebitStatus,			0, 	{ eCxSc_CaStatusSelect, 0,}},	// min 12
	{ eCxSc_CreditStatus,			0, 	{ eCxSc_CaStatusSelect, 0,}},	// min 4
	{ eCxSc_SignatureResp,			0, 	{ eCxSc_GenerateSign, 0,}},		// min 0
	{ eCxSc_EncryptedMsg,			0, 	{ eCxSc_EncryptMsg, 0,}},		// min 0
	{ eCxSc_DecryptedMsg,			0, 	{ eCxSc_DecryptMsg, 0,}},		// min 0
	{ eCxSc_CryptoBlockSize,		2, 	{ eCxSc_ReqBlockSize, 0,}}, 	//
	{ eCxSc_CardNumber,				4, 	{ eCxSc_ReqCardNumber, 0,}}, 	//
	{ eCxSc_SequenceNumber,			4, 	{ eCxSc_ReqSequenceNumber, 0,}},//
	{ eCxSc_RandomOctets,			0, 	{ eCxSc_ReqRandomOctets, 0,}},	// min 0
	{ eCxSc_PinStatus,				6, 	{ eCxSc_ReqPinStatus, 0,}},		//
	{ eCxSc_HostData,				0, 	{ eCxSc_Emm, eCxSc_Ecm, eCxSc_OrderRespApdu, 0,}},	// min 0
	{ eCxSc_ProtectedData, 			0,	{ eCxSc_Emm, eCxSc_Ecm, 0,}},	//	TODO: add..	min 3
	{ eCxSc_DebitLogData,			0, 	{ eCxSc_ReqDebitLog, 0,}},		// min 0


};

typedef enum
{
	eCxHostData_Padding			= 0x0,
	eCxHostData_UserText		= 0x10,
	eCxHostData_Fingerprint		= 0x20,
	eCxHostData_FingerprintId	= 0x21,
	eCxHostData_ChipsetPairing	= 0x40,
	eCxHostData_TokensReport	= 0x50,
	eCxHostData_BridgeControl	= 0x65	// for URI
} CxHostDataPI_t;

typedef enum
{
	eCxUserText_Text		= 0x1,
	eCxUserText_SeqNo		= 0x2,
	eCxUserText_Start		= 0x3,
	eCxUserText_Duration	= 0x4,
	eCxUserText_Repetition	= 0x5,
	eCxUserText_Interval	= 0x6,
} CxUserTextPI_t;

#define CX_USERTEXT_TEXT					0x00000001
#define CX_USERTEXT_SEQNO					0x00000010
#define CX_USERTEXT_START					0x00000100
#define CX_USERTEXT_DURATION				0x00001000
#define CX_USERTEXT_REPETITION				0x00010000
#define CX_USERTEXT_INTERVAL				0x00100000
#define CX_USERTEXT_MANDATORY_ALL			(CX_USERTEXT_TEXT | CX_USERTEXT_SEQNO | CX_USERTEXT_DURATION)
#define CX_USERTEXT_OPT_MANDATORY_ALL		(CX_USERTEXT_REPETITION | CX_USERTEXT_INTERVAL)

typedef enum
{
	eCxSc_AutomaticParsing,
	eCxSc_ManualParsing
} CxSc_ResponseParsingType_t;

#if defined(CONFIG_MW_CAS_CONAX_CPM) && defined(CONFIG_MW_CAS_CONAX_CPM_WITH_LPK)
//  [Chipset Pairing STB Intergration Kit 5.0-A 34p~35p]
/**
  *	 PROTECTED_DATA
  *
  *	PI : 0x81
  * 	PL : variable
  *	PV : protected_data() {
  *			algorithm_id				3	bslbf
  *  			mode_ind					2	bslbf
  *  			padding_ind				1	bslbf
  *  			reserved					2	bslbf
  *  			key_id					16	uimsbf
  *  			if (mode_ind == '10') {
  *  				init_vector_length		8	uimsbf
  *  				for (i=0;<N;i++){
  *  					init_vector_type	8	uimsbf
  *  				}
  *  			}
  *  			for (i=0;i<N;i++){
  *  				ciphertext_byte		8	bslbf
  *  			}
  *  		}
**/

typedef enum
{
	eCxSc_ProtectedData_AlgorithmID_3DES	= 0x00,	// '000' = 3DES
	eCxSc_ProtectedData_AlgorithmID_MAX			// other value = reseved for future
} CxSc_ProtectedData_AlgorithmID_t;

typedef enum
{
	eCxSc_ProtectedData_ModeInd_CBC_WO_IV	= 0x01,	// '01' = CBC without initialisation vector
	eCxSc_ProtectedData_ModeInd_CBC_WITH_IV	= 0x02,	// '10' = CBC with initialisation vector
	eCxSc_ProtectedData_ModeInd_MAX					// other value = reseved for future
} CxSc_ProtectedData_ModeInd_t;

typedef enum
{
	eCxSc_ProtectedData_PaddingInd_No		= 0x00,			// '0' = no padding
	eCxSc_ProtectedData_PaddingInd_RFC1423	= 0x01,		// '1' = RFC1423 padding
	eCxSc_ProtectedData_PaddingInd_MAX
} CxSc_ProtectedData_PaddingInd_t;
#endif

#define CX_SC_READ_BUFFER_SIZE				4096
#define CX_SC_WRITE_BUFFER_SIZE				1024

typedef struct
{
	HUINT8								ucScSlot;
	HUINT8								ucCassVer;
	HUINT8								ucPlainBlkSize;
	HUINT8								ucCipherBlkSize;
	HUINT32								ulCardNumber;
	HUINT32								ulSeqNumber;
	HUINT16								usCaSysId;
	HUINT16								usCountry;
	HUINT8								ucMatRatLevel;
	HUINT8								ucSessionCount;
	HUINT8								ucSW1;
	HUINT8								ucSW2;
	HUINT16								usReadBufLen;
	HUINT16								usWriteBufLen;
	HUINT8								aucReadBuf[CX_SC_READ_BUFFER_SIZE];
	HUINT8								aucWriteBuf[CX_SC_WRITE_BUFFER_SIZE];
	Handle_t							hSession;
	CxSc_State_t						eScState;
	CxStatusSelectType_t				eStatusSelectType;
	CxStatusList_t* 					pstStatusList;
	CxParamList_t						stScParamList;

	CxUserText_t						stUserText;
	CxFingerprint_t						stFingerprint;

	CxUiInfo_t							stUiInfo;

#if defined(CONFIG_MW_MM_PVR)
	HBOOL								bEcmQuery;
#endif

#if defined(CONFIG_MW_CAS_CONAX_CPM)
	CxErrorCode_t						eCpmCode;
#endif

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	HBOOL								bFPFromProtectedData;
#endif
	HBOOL								bBridgeControl;
	HUINT32								ulProtectedDataStartOffset;
	HUINT32								ulProtectedDataEndOffset;
#if defined(CONFIG_MW_CAS_CONAX_URI)
	DxCopyrightControl_ConaxDrmInfo_t	stDrmInfo;
	HBOOL								bFromProtectedData;
#endif

#if defined(CONFIG_DEBUG)
	Handle_t							hStbHandle;
#endif
	HBOOL								bRecoveryReset;
	HUINT8								ucRetryCount;
	HUINT32								ulTimerId;
} CxScInstance_t;

#define CX_SC_INTERFACE_VERSION				0x40		// version 4.1

/*******************************************************************/
/********************      Static Variables 	********************/
/*******************************************************************/
STATIC HULONG	s_ulCxScSemId;

STATIC CxScInstance_t* s_pstCxScInstance[CX_SC_NUMBER_OF_INSTANCE];

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/
STATIC HERROR local_cxsc_ProcessCommand(CxScInstance_t* pstInstance, Handle_t hParam);
STATIC void local_cxsc_MakeHeader(CxScInstance_t* pstInstance, HUINT8 ucSessionIdx, CxSc_Cmd_t eCmd, HUINT8 ucDataLen);
STATIC HERROR local_cxsc_DeleteStatusList(CxScInstance_t* pstInstance);
STATIC void local_cxsc_DeleteParam(CxScInstance_t* pstInstance);
STATIC HERROR local_cxsc_ScInserted(HINT32 nScSlot);
STATIC HERROR local_cxsc_ScExtracted(HINT32 nScSlot);

#if defined(CONFIG_DEBUG)
STATIC HUINT8* local_cxsc_DEBUG_GetPLStr(HUINT8 ucPL);
#endif

static HERROR local_cxsc_NotifyCb(Cx_SciEvent_e eSciEvent, HUINT8 ucSlotId)
{
	if (eSciEvent == eCx_SC_EXTRACTED)
	{
		return local_cxsc_ScExtracted((HINT32)ucSlotId);
	}

	return local_cxsc_ScInserted((HINT32)ucSlotId);
}


CxScInstance_t* local_cxsc_GetInstance(Handle_t hSession)
{
	HINT32	nSlot;

	nSlot = CXSESSION_GetScSlot(hSession);

	if (nSlot == CX_SC_NONE)
	{
		return NULL;
	}

	if (nSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return NULL;
	}

	return s_pstCxScInstance[nSlot];
}

HERROR CXSC_GetCurrentUri(Handle_t hAction, DxCopyrightControl_ConaxDrmInfo_t* pstDrmInfo)
{
	return CXACT_GetDrmInfo(hAction, pstDrmInfo);
}

HINT32 CXSC_GetCaSysId(HINT32 nScSlot)
{
	CxScInstance_t* pstInstance;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return CX_ERR_INVALID_PARAMETER;
	}

	pstInstance = s_pstCxScInstance[nScSlot];

	return pstInstance->usCaSysId;
}

HINT32 CXSC_GetSessionCount(HINT32 nScSlot)
{
	CxScInstance_t* pstInstance;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return CX_ERR_INVALID_PARAMETER;
	}

	pstInstance = s_pstCxScInstance[nScSlot];

	return pstInstance->ucSessionCount;
}

HINT32 CXSC_GetCountryIndicator(HINT32 nScSlot)
{
	CxScInstance_t* pstInstance;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return CX_ERR_INVALID_PARAMETER;
	}

	pstInstance = s_pstCxScInstance[nScSlot];

	return pstInstance->usCountry;
}

HINT32 CXSC_GetCardNumber(HINT32 nScSlot)
{
	CxScInstance_t* pstInstance;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return CX_ERR_INVALID_PARAMETER;
	}

	pstInstance = s_pstCxScInstance[nScSlot];

	return pstInstance->ulCardNumber;
}

HINT32 CXSC_GetMaturityRatingLevel(HINT32 nScSlot)
{
	CxScInstance_t* pstInstance;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return CX_ERR_INVALID_PARAMETER;
	}

	pstInstance = s_pstCxScInstance[nScSlot];

	return pstInstance->ucMatRatLevel;
}

HINT32 CXSC_GetHostVersion(void)
{
	return CX_SC_INTERFACE_VERSION;
}


HERROR CXSC_GetSCState(HINT32 lScSlot)
{
	CxScInstance_t* pstInstance;

	ENTER_FUNCTION;

	if (lScSlot < 0 || lScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		HxLOG_Error("Strange slot number");
		return ERR_FAIL;
	}

	pstInstance = s_pstCxScInstance[lScSlot];
	if (pstInstance == NULL)
	{
		HxLOG_Error("Can't get local context");
		return ERR_FAIL;
	}

	// Ready가 되어야 Smart Card가 잘 된다고 생각 한다.
	if (pstInstance->eScState != eCxSc_StateReady)
	{
		HxLOG_Error(" == Invalid SC State : %d\r\n", pstInstance->eScState);
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}


#define __BasicDataTypes____________________________________________________

// return the length of ascii text
STATIC HUINT8 local_cxsc_GetAsciiText(HUINT8* pucData, HUINT8* pszBuffer)
{
	HUINT8* pucSrc;
	HUINT8	ucLen;

	if (pucData == NULL || pszBuffer == NULL)
	{
		return 0;
	}

	pucSrc = pucData;

	if (*pucSrc++ != eCxData_AsciiText)
	{
		return 0;
	}

	ucLen = *pucSrc++;

	CX_memcpy(pszBuffer, pucSrc, ucLen);
	*(pszBuffer + ucLen) = 0x0;

	return ucLen;
}

STATIC HUINT8 local_cxsc_GetOctetStr(HUINT8* pucData, HUINT8* pszBuffer)
{
	HUINT8* pucSrc;
	HUINT8	ucLen;

	if (pucData == NULL || pszBuffer == NULL)
	{
		return 0;
	}

	pucSrc = pucData;

	if (*pucSrc++ != eCxData_OctetStr)
	{
		return 0;
	}

	ucLen = *pucSrc++;

	CX_memcpy(pszBuffer, pucSrc, ucLen);
	*(pszBuffer + ucLen) = 0x0;

	return ucLen;
}

STATIC HUINT8 local_cxsc_GetTime(HUINT8* pucData, CxTime_t* pstTime)
{
	HUINT8* pucSrc;
	HUINT8	ucLen;
	HUINT8	ucYoffDay, ucYearMonth;

	if (pucData == NULL || pstTime == NULL)
	{
		return 0;
	}

	pucSrc = pucData;

	if ((*pucSrc != eCxData_Time) && (*pucSrc != eCxUserText_Start))
	{
		return 0;
	}

	pucSrc++;

	ucLen = *pucSrc++;
	ucYoffDay = *pucSrc++;
	ucYearMonth = *pucSrc++;

	pstTime->usYear = (HUINT16)((ucYearMonth >> 4) + 1990 + (ucYoffDay >> 5) * 10);
	pstTime->ucMonth = (HUINT8)(ucYearMonth & 0x0F);
	pstTime->ucDay = (HUINT8)(ucYoffDay & 0x1F);

	if (ucLen == 4)
	{
		pstTime->ucHour = (HUINT8)(*pucSrc++ & 0x1F);
		pstTime->ucMinute = (HUINT8)(*pucSrc++ & 0x3F);
	}
	else
	{
		pstTime->ucHour = 0;
		pstTime->ucMinute = 0;
	}

	return ucLen;
}

STATIC HUINT8 local_cxsc_GetProductId(HUINT8* pucData, HUINT32* pulProdId)
{
	HUINT8* 		pucSrc;
	const HUINT8	ucLen	= 4;

	if (pucData == NULL || pulProdId == NULL)
	{
		return 0;
	}

	pucSrc = pucData;

	if (*pucSrc++ != eCxData_ProductId)
	{
		return 0;
	}

	if (*pucSrc++ != ucLen)	// length
	{
		return 0;
	}

	*pulProdId = get32bit(pucSrc);

	return ucLen;
}

STATIC HINT32 local_cxsc_SetProductId(HUINT8* pucData, HUINT32 ulProdId)
{
	HUINT8* pucTarget;

	if (pucData == NULL)
	{
		return 0;
	}

	pucTarget = pucData;

	*pucTarget++ = eCxData_ProductId;
	*pucTarget++ = 4;

	put32bit(pucTarget, ulProdId);

	return 6;
}

STATIC HUINT8 local_cxsc_GetPrice(HUINT8* pucData, CxPrice_t* pstPrice)
{
	HUINT8* 		pucSrc;
	const HUINT8	ucLen	= 6;

	if (pucData == NULL || pstPrice == NULL)
	{
		return 0;
	}

	pucSrc = pucData;

	if (*pucSrc++ != eCxData_Price)
	{
		return 0;
	}

	if (*pucSrc++ != ucLen)	// length
	{
		return 0;
	}

	CX_memcpy(pstPrice->aucCurrency, pucSrc, CX_PRICE_CURRENCY_LEN);
	pstPrice->aucCurrency[CX_PRICE_CURRENCY_LEN] = '\0';
	pstPrice->usPriceUnit = get16bit(pucSrc + 3);
	pstPrice->ucPriceDecimal = get8bit(pucSrc + 5);

	return ucLen;
}


#define __Initiailization______________________________________________

STATIC HERROR local_cxsc_ResetSession(CxScInstance_t* pstInstance, HUINT8 ucSessionIdx)
{
	HUINT8* pucCmd;
	HERROR	nErr;

	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	if (ucSessionIdx >= pstInstance->ucSessionCount)
	{
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, 0, eCxSc_InitCass, CX_PL_ResetSession + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_ResetSession;
	*pucCmd++ = CX_PL_ResetSession;
	*pucCmd = (HUINT8)ucSessionIdx;

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, HANDLE_NULL);

	LEAVE_FUNCTION;
	return nErr;
}


STATIC HERROR local_cxsc_PairingMech(CxScInstance_t* pstInstance)
{
	HERROR	nErr			= ERR_OK;

#if defined(CONFIG_MW_CAS_CONAX_CPM)
	HUINT16	usProtocolId;
	HUINT8* pucCmd;

	ENTER_FUNCTION;

	nErr = CXCPM_GetProtocolId(&usProtocolId);
	if (nErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, 0, eCxSc_InitCass, CX_PL_PairingMech + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;
	*pucCmd++ = (HUINT8)eCxSc_PairingMech;
	*pucCmd++ = CX_PL_PairingMech;
	*pucCmd++ = (HUINT8)((usProtocolId >> 8) & 0xFF);
	*pucCmd++ = (HUINT8)((usProtocolId >> 0) & 0xFF);

	nErr = local_cxsc_ProcessCommand(pstInstance, HANDLE_NULL);

	LEAVE_FUNCTION;
#endif
	return nErr;
}


// recoveryReset은 comm. error 발생시 s/c를 reset하는 경우를 말하는 것임. 이 때는 reset_session을 보내지 않음
// InitCass - HostVer / ResetSession
//STATIC HERROR local_cxsc_InitCass(CxScInstance_t *pstInstance, HBOOL bRecoveryReset)
STATIC HERROR local_cxsc_InitCass(CxScInstance_t* pstInstance)
{
	HUINT8* pucCmd;
	HERROR	nErr			= ERR_OK;

	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, 0, eCxSc_InitCass, CX_PL_HostVer + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_HostVer;
	*pucCmd++ = CX_PL_HostVer;
	*pucCmd = CX_SC_INTERFACE_VERSION;

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, HANDLE_NULL);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("local_cxsc_ProcessCommand() fail...\n");
		return nErr;
	}

	LEAVE_FUNCTION;
	return nErr;
}

HERROR CXSC_ResetSession(HINT32 nScSlot, HUINT8 ucSessionIdx)
{
	CxScInstance_t* pstInstance;
	HERROR			nErr			= ERR_OK;

	ENTER_FUNCTION;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = s_pstCxScInstance[nScSlot];

	nErr = local_cxsc_ResetSession(pstInstance, ucSessionIdx);	// header 만들기

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

HERROR CXSC_AllSessionReset(void)
{
	CxScInstance_t* pstInstance;
	HERROR			nErr			= ERR_OK;
	int				i, j;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);

	for (i = 0 ; i < CX_SC_NUMBER_OF_INSTANCE ; i++)
	{
		pstInstance = s_pstCxScInstance[i];

		for (j = 0 ; j < pstInstance->ucSessionCount ; j++)
		{
			nErr |= local_cxsc_ResetSession(pstInstance, j);	// header 만들기
		}
	}

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

#define __EMM______________________________________________

// InitOaa - Cat
HERROR CXSC_SendCat(HINT32 nScSlot, Handle_t hEmm, HUINT8* pucData)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	HUINT16			usLen, usDescLen, usCopiedLen, usIdx;
	HUINT16			usCaSysId;
	HINT32			nRet;

	ENTER_FUNCTION;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);

	pstInstance = s_pstCxScInstance[nScSlot];

	if (pstInstance->eScState == eCxSc_StateCommError)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	usLen = get12bit(pucData + 1) + 3 - 4; // crc 부분 제외한 길이

	//	if (usLen > 254)
	//	{
	//		// usLen이 너무크면 smartcard로 넘겨줄 수가 없음
	//		return ERR_FAIL;
	//	}

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_Cat;
	*pucCmd++ = (HUINT8)usLen;			// 마지막에 다시 update할 것임
	//	CX_memcpy(pucCmd, pucData, usLen);

	//## CAT에서 ca_sys_id 같은 ca_descriptor만 걸러내서 저장하고 sec_len 조정해준다. crc는 틀려도 상관 없삼
	nRet = CXSC_GetCaSysId(nScSlot);
	if (nRet == CX_ERR_INVALID_PARAMETER)
	{
		CX_SEM_Release(s_ulCxScSemId);
		HxLOG_Error("fail to get ca_sys_id\r\n");
		return ERR_FAIL;
	}
	usCaSysId = (HUINT16)nRet;

	CX_memcpy(pucCmd, pucData, 8);
	usCopiedLen = 8;
	usIdx = 8;

	while (usLen > usIdx)
	{
		usDescLen = *(pucData + usIdx + 1) + 2;
		if (*(pucData + usIdx) == 0x09)	// ca_descriptor
		{
			if (get16bit(pucData + usIdx + 2) == usCaSysId)
			{
				CX_memcpy(pucCmd + usCopiedLen, pucData + usIdx, usDescLen);
				usCopiedLen += usDescLen;
			}
		}

		usIdx += usDescLen;
	}

	// crc 부분 copy
	CX_memcpy(pucCmd + usCopiedLen, pucData + usIdx, 4);
	usCopiedLen += 4; // + crc


	// len 값 다시 쓰기
	pucCmd = pstInstance->aucWriteBuf + 5 + 1;	//PI
	//*pucCmd = usCopiedLen - 3;	// - (table_id & sec_len)
	*pucCmd = usCopiedLen;	// PL

	// sec_len update
	*(pucCmd + 2) = (*(pucCmd + 2) & 0xF0) | (((usCopiedLen - 3) >> 8) & 0x0F);	// 12bit 앞부분 값은 그대로 쓰려고..
	*(pucCmd + 3) = (usCopiedLen - 3) & 0xFF;

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, 0, eCxSc_InitOaa, usCopiedLen + 2);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, (Handle_t)hEmm);
	if(nErr != ERR_OK)
	{
		HxLOG_Error("CXSC_SendCat() error\n");
	}

	CXCORE_SendMsg(eCxMsg_Sc_CatReceived, HANDLE_NULL, nScSlot, (HUINT32)0);

	CX_SEM_Release(s_ulCxScSemId);

	LEAVE_FUNCTION;
	return nErr;
}

// EmmCom - Emm
HERROR CXSC_SendEmm(HINT32 nScSlot, HUINT8* pucData)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HUINT16			usLen;
	HERROR			nErr;

	ENTER_FUNCTION;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	usLen = get12bit(pucData + 1) + 3;
	if (usLen > 254)
	{
		// usLen이 너무크면 smartcard로 넘겨줄 수가 없음
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = s_pstCxScInstance[nScSlot];

	if (pstInstance->eScState != eCxSc_StateReady)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, 0, eCxSc_EmmCom, usLen + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_Emm;
	*pucCmd++ = (HUINT8)usLen;
	CX_memcpy(pucCmd, pucData, usLen);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, (Handle_t)nScSlot);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParseCaDescEmm(Handle_t hEmm, HUINT32 ulLen, HUINT8* pucData)
{
	HUINT16		usPid;
	HUINT16		usAddrCnt;
	HUINT32		ulIdx;
	CxEmmAddr_t* 					pstAddr;

	/*
	tag
	len
	desc_tag
	desc_len
	ca_sys_id
	ca_pid
	private_data
	emm_addr (n * (2+7))
	*/
	ENTER_FUNCTION;

	ulIdx = 2 + *(pucData + 1);	// tag, len, desc_tag, desc_len + value of desc_len

	usPid = get13bit(pucData + 4);

	usAddrCnt = (HUINT16)(ulLen - ulIdx) / (CX_ADDRESS_LEN + 2);

	pstAddr = (CxEmmAddr_t *)CX_MEM_Alloc(sizeof(CxEmmAddr_t) * usAddrCnt);
	if (pstAddr == NULL)
	{
		return ERR_FAIL;
	}

	usAddrCnt = 0;
	while (ulIdx < ulLen)
	{
		// tag = 0x23 (ADDRESS), length = 0x7
		if (*(pucData + ulIdx) == CX_ADDRESS_TYPE && *(pucData + ulIdx + 1) == CX_ADDRESS_LEN)
		{
			// 맨앞에는 0x82가 들어가야 ???
			// The Default Table ID value for EMMs is 0x82.
			pstAddr[usAddrCnt].aucFilter[0] = 0x82;
			CX_memcpy(&pstAddr[usAddrCnt].aucFilter[1], pucData + ulIdx + 2, CX_ADDRESS_LEN);
			CX_memset(pstAddr[usAddrCnt].aucMask, 0xFF, CX_ADDRESS_LEN + 1);
			usAddrCnt++;
		}
		ulIdx += CX_ADDRESS_LEN + 2;
	}

	// conax 6.0 [CT 4.1.2-F. page - 20]
	// If the number of EMM addresses returned by the smart card is greater than 4,
	// the STB shall reserve EMM filters for the first four addresses only.
	if (usAddrCnt > 4)
	{
		usAddrCnt = 4;
	}

	// emm address 들이 돌아온다.. emm쪽으로 넘겨줘서 거기서 filter 거는게 좋을 듯하다..
	// ca_pid 랑 emmaddr만 넘겨주자.
	CXEMM_Add(hEmm, usPid, usAddrCnt, pstAddr);

	LEAVE_FUNCTION;
	return ERR_OK;
}

#define __ECM______________________________________________

STATIC HERROR local_cxsc_UpdateURI(CxScInstance_t* pstInstance, Handle_t hEcm)
{
#if defined(CONFIG_MW_MM_PVR) && defined(CONFIG_MW_CAS_CONAX_URI)
	Handle_t hStbHandle;
	HUINT8 ucProgMrLevel;

	if(CxSc_GetInParam(pstInstance) != eCxSc_Ecm)
	{
		return ERR_OK;
	}

	hStbHandle = CXECM_GetStbHandle(hEcm);
	if(hStbHandle == HANDLE_NULL)
	{
		HxLOG_Error("hStbHandle HANDLE_NULL\n");
		return ERR_FAIL;
	}

	if(pstInstance->bEcmQuery == TRUE)
	{
		// 해당 조건으로는 TSR or Record session만 가능하다.
		if(pstInstance->bBridgeControl != TRUE)
		{
			CXSC_GetDefaultUri(&pstInstance->stDrmInfo);
		}

		CXECM_GetAccessStatus(hEcm, &ucProgMrLevel);
		pstInstance->stDrmInfo.eMR = CXSC_ConvertScMrToUriMr(ucProgMrLevel);
	}

	CXSESSION_UriReceived(CXSESSION_GetHandleByStbHandle(hStbHandle), &pstInstance->stDrmInfo);
#endif

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_CONAX_CPM)
STATIC HINT32 local_cxsc_CheckErrorByAccessDenied(void)
{
	HUINT32	ulCWPK;
	HUINT32	ulLPK;

	ulCWPK = CXCPM_CountKey(eCxCpm_CWPK);
	ulLPK = CXCPM_CountKey(eCxCpm_LPK);

	if (ulCWPK == 0)
	{
		return eCxECode_4;
	}

	/* keystore에 LPK 와 CWPK 모두 있을 경우 : eCxECode_6 */
	/* keystore에 CWPK 가 있을 경우 : eCxECode_5 */
	return (ulLPK > 0) ? eCxECode_6 : eCxECode_5;
}

HINT32 CXSC_CheckErrorByAccessDenied(void)
{
	return local_cxsc_CheckErrorByAccessDenied();
}
#endif

//EcmCom - Ecm
HERROR CXSC_SendEcm(Handle_t hSession, Handle_t hEcm, HUINT8* pucData)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HUINT16			usLen;
	HERROR			nErr;
#if defined(CONFIG_MW_MM_PVR)
	HBOOL			bBridgeControl = FALSE;
#endif
	int i;

	ENTER_FUNCTION;

	usLen = get12bit(pucData + 1) + 3 + 1;	// +1은 mode_ind/pairing_ind의 1byte
	if (usLen > 254)
	{
		// usLen이 너무크면 smartcard로 넘겨줄 수가 없음
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);

	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	pstInstance->bEcmQuery = FALSE;

	for(i = 0 ; i < CX_SC_SEND_ECM_COUNT ; i++)
	{
		if (pstInstance->eScState != eCxSc_StateReady)
		{
			CX_SEM_Release(s_ulCxScSemId);
			return ERR_FAIL;
		}

		// header 만들기
		local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_EcmCom, usLen + 2);

		// parameter 만들기
		pucCmd = pstInstance->aucWriteBuf + 5;

		*pucCmd++ = (HUINT8)eCxSc_Ecm;
		*pucCmd++ = (HUINT8)usLen;
		//## pairing 모드이냐 아니냐 에 따라서 값이 달라짐
		*pucCmd = 0x00;	// normal
#if defined(CONFIG_MW_CAS_CONAX_CPM)
		if (CXCPM_IsEncryptedCwMode())
		{
			;//*pucCmd = 0x04;	// chipset pairing
		}
#endif
#if defined(CONFIG_MW_MM_PVR)
		if (pstInstance->bEcmQuery == TRUE)
		{
			*pucCmd |= 0x01;
		}
#endif
		pucCmd++;

		CX_memcpy(pucCmd, pucData, usLen);

		// 보내기
		nErr = local_cxsc_ProcessCommand(pstInstance, hEcm);

#if defined(CONFIG_MW_MM_PVR)
		if(pstInstance->bEcmQuery != TRUE)
		{
			bBridgeControl = pstInstance->bBridgeControl;

			if(pstInstance->bBridgeControl == TRUE)
			{
				if(pstInstance->stDrmInfo.eMR && (pstInstance->stDrmInfo.eMR <= eDxCopyrightControl_ConaxMaturityRating_XXX))
				{
					break;
				}
			}
		}

	 	switch(CXACT_GetActionTypeFromStbHandle(CXECM_GetStbHandle(hEcm)))
		{
			case eCAS_ActionType_Recording:
			case eCAS_ActionType_Tsr_Rec:
				pstInstance->bEcmQuery = TRUE;
				break;

			default:
				break;
	 	}

		if(pstInstance->bEcmQuery != TRUE)
			break;
#endif
	}

#if defined(CONFIG_MW_MM_PVR)
	pstInstance->bBridgeControl = bBridgeControl;
#endif

	local_cxsc_UpdateURI(pstInstance, hEcm);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

#if defined(CONFIG_MW_CAS_CONAX_CPM)
STATIC HINT32 local_cxsc_GetProtectKey(HUINT16 usKeyId, CxCpmKey_t *pstCpmKey)
{
	HERROR		nErr;
	HUINT32		ulCWPK;
	HUINT32		ulLPK;

	ulCWPK = CXCPM_CountKey(eCxCpm_CWPK);
	ulLPK = CXCPM_CountKey(eCxCpm_LPK);

	if (usKeyId == CX_CPM_INVALID_KEYID)	// if Control word is not encrypted
	{
		if (ulCWPK == 0)
		{
			return eCxECode_1;	// error code <1>
		}
		return (ulLPK != 0) ? eCxECode_3 : eCxECode_2;
	}

	if (ulCWPK == 0)
	{
		return eCxECode_9;  		 // keystore에 CWPK가 하나도 없으면 '9'
	}

	nErr = CXCPM_GetKey(usKeyId, pstCpmKey);
	if (nErr != ERR_OK)
	{
		return eCxECode_7;  		 //keystore의 key_id와 CWPK의 key_id가 다르면 '7'
	}

	if (CXCPM_HasValue(pstCpmKey) != TRUE)	// key_id가 있지만 key가 비어있다?
	{
		return eCxECode_7;
	}

	//if (key.ucType != (HUINT8)eCxCpm_CWPK && CXCPM_HasValue(&key) == FALSE)
	if (pstCpmKey->ucType != (HUINT8)eCxCpm_CWPK)
	{
		/*
		 * If the key with the specified key_id is found, but is not a CWPK.
		 * The user shall be presented with a status message corresponding to status code 14
		*/

		return eCxECode_14; 		 // '5.3' C.P spec. 24 page.
	}

	return 0;
}
#endif

STATIC HERROR local_cxsc_ParseAccessStatus(CxScInstance_t* pstInstance, Handle_t hEcm, HUINT8* pucData)
{
	CxAccessStatus_t *pstAccess;
	int i;

	pstAccess = (CxAccessStatus_t *)pucData;

	pstAccess->ucExt3 = pstInstance->eCpmCode;
	pstAccess->ucExt4 = (HUINT8)FALSE;

	for (i = 0; i < pstInstance->stScParamList.ulCount ; i++)
	{
		switch(pstInstance->stScParamList.stParam[i].eType)
		{
			case eCxSc_OrderInfo:
			case eCxSc_AcceptViewingInfo:
			case eCxSc_TokenDebitInfo:
				pstAccess->ucExt4 = (HUINT8)TRUE;
				break;

			default:
				break;
		}
	}

	return CXUI_DisplayAccessStatus(hEcm, pstAccess, 0);
}

// CW Parameter
STATIC HERROR local_cxsc_ParseCw(CxScInstance_t* pstInstance, Handle_t hEcm, HUINT8* pucData)
{
	HUINT8		ucKeyId;
	HUINT8		ucEvenOdd;
	HUINT8* 	pucCW;
	HERROR		nErr			= ERR_OK;
	HUINT8* 	pucCWPK			= NULL;
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	CxCpmKey_t	stCpmKey;
#endif

	// CW를 전달한다
	ENTER_FUNCTION;

	ucKeyId = *(pucData + 1);	// key_id of CWPK
	ucEvenOdd = *(pucData + 2);
	pucCW = pucData + 5;

#if defined(CONFIG_MW_CAS_CONAX_CPM)
	pstInstance->eCpmCode = 0;

	if(CXCPM_IsEncryptedCwMode() || (ucKeyId != CX_CPM_INVALID_KEYID))
	{
		pstInstance->eCpmCode = local_cxsc_GetProtectKey((HUINT16)ucKeyId, &stCpmKey);
		if (pstInstance->eCpmCode != 0)
		{
			HxLOG_Error("Can not set CW for some reason! ErrorCode(%d), keyid(0x%x)\r\n", pstInstance->eCpmCode, ucKeyId);
			return ERR_FAIL;
		}

		pucCWPK = stCpmKey.aucValue;
	}
#endif

	// ucEvenOdd == 1, odd period
	if (ucEvenOdd == 0x1)
	{
		nErr = CXSTREAM_SetCW(hEcm, CX_CW_LENGTH, 0, pucCW, NULL, pucCWPK, (HUINT16)ucKeyId);
	}
	else if (ucEvenOdd == 0x0)
	{
		nErr = CXSTREAM_SetCW(hEcm, 0, CX_CW_LENGTH, NULL, pucCW, pucCWPK, (HUINT16)ucKeyId);
	}
	else
	{
		// unknown value - ignore
	}

	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParseOrderInfo(CxScInstance_t* pstInstance, Handle_t hEcm, HUINT32 ulLen, HUINT8* pucData)
{
	HUINT8	ucRetLen, ucPriceLen	= 0, ucLabelLen	= 0;
	HUINT8* pucSrc;

	ENTER_FUNCTION;

	pucSrc = pucData;
	ucRetLen = local_cxsc_GetProductId(pucSrc, &pstInstance->stUiInfo.stOrder.ulProdRef);
	pucSrc += ucRetLen + 2;

	ucPriceLen = local_cxsc_GetPrice(pucSrc, &pstInstance->stUiInfo.stOrder.stPrice);
	if (ucPriceLen > 0)
	{
		pucSrc += ucPriceLen + 2;
	}
	else
	{
		CX_memset(&pstInstance->stUiInfo.stOrder.stPrice, 0, sizeof(CxPrice_t));
	}

	if (pucSrc < pucData + ulLen)
	{
		ucLabelLen = local_cxsc_GetAsciiText(pucSrc, pstInstance->stUiInfo.stOrder.szLabel);
	}
	else
	{
		pstInstance->stUiInfo.stOrder.szLabel[0] = 0;
	}

	// 데이터는 다 얻었는데 뭘 하지? 화면에 보여줘야 겠지..
	CXUI_ProcessOrderInfo(hEcm, &pstInstance->stUiInfo.stOrder);

	(void)ucLabelLen;

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ParseAcceptViewingInfo(CxScInstance_t* pstInstance, Handle_t hEcm, HUINT32 ulLen, HUINT8* pucData)
{
	HUINT8	ucLabelLen		= 0, ucProdRefLen = 0;
	HUINT8* pucSrc;

	ENTER_FUNCTION;

	pucSrc = pucData;
	pstInstance->stUiInfo.stAcceptViewing.usMinuteLeft = get16bit(pucSrc);
	pucSrc += 2;

	ucLabelLen = local_cxsc_GetAsciiText(pucSrc, pstInstance->stUiInfo.stAcceptViewing.szLabel);
	pucSrc += ucLabelLen + 2;

	if (pucSrc < pucData + ulLen)
	{
		ucProdRefLen = local_cxsc_GetProductId(pucSrc, &pstInstance->stUiInfo.stAcceptViewing.ulProdRef);
		if (ucProdRefLen == 0)
		{
			pstInstance->stUiInfo.stAcceptViewing.ulProdRef = CX_INVALID_PRODUCT_REF;
		}
	}

	// 데이터는 다 얻었는데 뭘 하지? 화면에 보여줘야 겠지..
	CXUI_ProcessAcceptViewingInfo(hEcm, &pstInstance->stUiInfo.stAcceptViewing);
	(void)ucLabelLen;
	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ParseTokenDebitInfo(CxScInstance_t* pstInstance, Handle_t hEcm, HUINT32 ulLen, HUINT8* pucData)
{
	HUINT8* pucSrc;
	HUINT8	ucLabelLen		= 0;

	ENTER_FUNCTION;

	pucSrc = pucData;

	pstInstance->stUiInfo.stTokenDebit.usPurseRef = get16bit(pucSrc);
	pucSrc += 2;

	pstInstance->stUiInfo.stTokenDebit.ulEventTag = get24bit(pucSrc);
	pucSrc += 3;

	pstInstance->stUiInfo.stTokenDebit.ucInsufficientTokens = (*pucSrc & 0x2) >> 1;
	pstInstance->stUiInfo.stTokenDebit.ucTokenMode = (*pucSrc & 0x1) ? eCxTokenPPV_Time : eCxTokenPPV_Event;
	pucSrc++;

	pstInstance->stUiInfo.stTokenDebit.ulEventCost = get24bit(pucSrc);
	pucSrc += 3;

	if (pucSrc < pucData + ulLen)
	{
		ucLabelLen = local_cxsc_GetAsciiText(pucSrc, pstInstance->stUiInfo.stTokenDebit.szLabel);
	}
	else
	{
		pstInstance->stUiInfo.stTokenDebit.szLabel[0] = 0;
	}

	// 데이터는 다 얻었는데 뭘 하지? 화면에 보여줘야 겠지..
	CXUI_ProcessTokenDebitInfo(hEcm, &pstInstance->stUiInfo.stTokenDebit);
	(void)ucLabelLen;
	LEAVE_FUNCTION;
	return ERR_OK;
}

#define __UI______________________________________________

//CaStatusCom - CaStatusSelect
CxStatusList_t* CXSC_GetCaStatus(HINT32 nScSlot, CxStatusSelectType_t eType, HUINT32 ulRef)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HUINT8			ucCmdLen;
	HERROR			nErr;

	ENTER_FUNCTION;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		//DDI_UART_Print("CXSC_GetCaStatus error (%d)\n", nScSlot);
		return NULL;
	}

	CX_SEM_Get(s_ulCxScSemId);

	pstInstance = s_pstCxScInstance[nScSlot];

	switch (eType)
	{
	case eCxCaStatus_Subscription:
	case eCxCaStatus_Purse:
	case eCxCaStatus_Debit:
	case eCxCaStatus_Credit:
		ucCmdLen = 3;
		break;

	case eCxCaStatus_PpvEvent:
		ucCmdLen = 4;
		break;

	default:
		ucCmdLen = 1;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, 0, eCxSc_CaStatusCom, ucCmdLen + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_CaStatusSelect;
	*pucCmd++ = ucCmdLen;
	*pucCmd++ = (HUINT8)eType;

	if (ucCmdLen == 3)
	{
		*pucCmd++ = (HUINT8)((ulRef >> 8) & 0xFF);
		*pucCmd = (HUINT8)(ulRef & 0xFF);
	}
	else if (ucCmdLen == 4)
	{
		*pucCmd++ = (HUINT8)((ulRef >> 16) & 0xFF);
		*pucCmd++ = (HUINT8)((ulRef >> 8) & 0xFF);
		*pucCmd = (HUINT8)(ulRef & 0xFF);
	}
	else
	{
		// do nothing
	}

	// 기존에 있던 데이터는 삭제
	local_cxsc_DeleteStatusList(pstInstance);

	// 이번에 받을 data type 기록해둠.. 나중에 지울때 참조할 거임
	pstInstance->eStatusSelectType = eType;

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, HANDLE_NULL);
	if (nErr != ERR_OK)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return NULL;
	}

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;

	return pstInstance->pstStatusList;
}

//PinCom - CaPinIn
// pcuPin = ascii char
// nScSlot을 정확히 줘야 한다.. hSession은 옵션..
HERROR CXSC_EnterCaPin(Handle_t hSession, HINT32 nScSlot, CxPinCheckType_t eType, HUINT8* pucPin)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HUINT8			ucSessIdx;
	HERROR			nErr;

	ENTER_FUNCTION;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = s_pstCxScInstance[nScSlot];

	if (eType == eCxPinCheck_General)
	{
		ucSessIdx = 0;
	}
	else
	{
		ucSessIdx = CXSESSION_GetSessionIdx(hSession);
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, ucSessIdx, eCxSc_PinCom, CX_PL_CaPinIn + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_CaPinIn;
	*pucCmd++ = CX_PL_CaPinIn;
	*pucCmd++ = (HUINT8)eType;
	CX_memcpy(pucCmd, pucPin, 4);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	if (nErr == ERR_OK)
	{
		switch(eType)
		{
			case eCxPinCheck_General:
				break;

			default:
				// MR 풀거나 ppv 본다고 하면, 바로 화면 볼 수 있도록 ecm을 다시 보낸다. (ecm filter를 다시 건다)
				CXCORE_SendMsg(eCxMsg_Ecm_FilterRestart, hSession, (HUINT32)0, (HUINT32)0);
				break;
		}
	}

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//PinCom - CaPinChange
HERROR CXSC_ChangeCaPin(HINT32 nScSlot, HUINT8* pucOldPin, HUINT8* pucNewPin)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	ENTER_FUNCTION;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = s_pstCxScInstance[nScSlot];

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, 0, eCxSc_PinCom, CX_PL_CaPinChange + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_CaPinChange;
	*pucCmd++ = CX_PL_CaPinChange;
	CX_memcpy(pucCmd, pucOldPin, 4);
	CX_memcpy(pucCmd + 4, pucNewPin, 4);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, HANDLE_NULL);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//PinCom - SignPinChange
HERROR CXSC_ChangeSignPin(Handle_t hSession, HUINT32 ulKeyId, HUINT8* pucOldSignPin, HUINT8* pucNewSignPin)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_PinCom, CX_PL_SignPinChange + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_SignPinChange;
	*pucCmd++ = CX_PL_SignPinChange;

	put32bit(pucCmd, ulKeyId);

	CX_memcpy(pucCmd + 4, pucOldSignPin, 4);
	CX_memcpy(pucCmd + 8, pucNewSignPin, 4);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//PinCom - ReqPinStatus
HERROR CXSC_GetSignPinStatus(Handle_t hSession, HUINT32 ulKeyId)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_PinCom, CX_PL_ReqPinStatus + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_ReqPinStatus;
	*pucCmd++ = CX_PL_ReqPinStatus;

	put32bit(pucCmd, ulKeyId);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//PinCom - MatRatChange
HERROR CXSC_ChangeMaturityRating(HINT32 nScSlot, HUINT8 ucMatRatValue, HUINT8* pucPin)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	HxLOG_Error("[Enter(%d)]\r\n", nScSlot);

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);

	pstInstance = s_pstCxScInstance[nScSlot];

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, 0, eCxSc_PinCom, CX_PL_MatRatChange + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_MatRatChange;
	*pucCmd++ = CX_PL_MatRatChange;

	CX_memcpy(pucCmd, pucPin, 4);
	*(pucCmd + 4) = ucMatRatValue;

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, HANDLE_NULL);

	if (nErr == ERR_OK)
	{
		// 바뀐 maturity rating level을 기억해둔다.
		pstInstance->ucMatRatLevel = ucMatRatValue;
	}

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_AddStatusList(CxScInstance_t* pstInstance, CxStatusList_t* pStatus)
{
	CxStatusList_t* pNode;

	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	pStatus->pstNext = NULL;

	if (pstInstance->pstStatusList == NULL)
	{
		pstInstance->pstStatusList = pStatus;
	}
	else
	{
		pNode = pstInstance->pstStatusList;

		while (pNode->pstNext != NULL)
		{
			pNode = pNode->pstNext;
		}

		pNode->pstNext = pStatus;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_DeleteStatusList(CxScInstance_t* pstInstance)
{
	CxStatusList_t* pNode, * pNextNode;

	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	pNode = pstInstance->pstStatusList;
	pstInstance->pstStatusList = NULL;

	while (pNode != NULL)
	{
		pNextNode = pNode->pstNext;

		CX_MEM_Free(pNode);
		pNode = pNextNode;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ParseSubscriptionStatus(CxScInstance_t* pstInstance, HUINT32 ulLen, HUINT8* pucData)
{
	CxStatusList_t* pStatus;
	HUINT8* 		pucSrc;
	HUINT8			ucRetLen		= 0;
	HERROR			nErr;
	int				i;

	ENTER_FUNCTION;
	pStatus = CX_MEM_Alloc(sizeof(CxStatusList_t));
	if (pStatus == NULL)
	{
		return ERR_FAIL;
	}

	pucSrc = pucData;

	pStatus->stStatus.stSubscription.usSubscriptionRef = get16bit(pucSrc);
	pucSrc += 2;

	ucRetLen = local_cxsc_GetAsciiText(pucSrc, pStatus->stStatus.stSubscription.szLabel);
	pucSrc += ucRetLen + 2;

	for (i = 0; i < 2; i++)
	{
		ucRetLen = local_cxsc_GetTime(pucSrc, &pStatus->stStatus.stSubscription.stStartDate[i]);
		pucSrc += ucRetLen + 2;

		ucRetLen = local_cxsc_GetTime(pucSrc, &pStatus->stStatus.stSubscription.stEndDate[i]);
		pucSrc += ucRetLen + 2;

		ucRetLen = local_cxsc_GetOctetStr(pucSrc, pStatus->stStatus.stSubscription.szEntitlemnet[i]);
		pucSrc += ucRetLen + 2;
	}

	nErr = local_cxsc_AddStatusList(pstInstance, pStatus);

	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParseEventStatus(CxScInstance_t* pstInstance, HUINT32 ulLen, HUINT8* pucData)
{
	CxStatusList_t* pStatus;
	HUINT8* 		pucSrc;
	HUINT8			ucRetLen		= 0;
	HERROR			nErr;

	ENTER_FUNCTION;

	pStatus = CX_MEM_Alloc(sizeof(CxStatusList_t));
	if (pStatus == NULL)
	{
		return ERR_FAIL;
	}

	pucSrc = pucData;

	pStatus->stStatus.stEvent.ulEventTag = get24bit(pucSrc);
	pucSrc += 3;

	ucRetLen = local_cxsc_GetAsciiText(pucSrc, pStatus->stStatus.stEvent.szLabel);
	pucSrc += ucRetLen + 2;

	ucRetLen = local_cxsc_GetTime(pucSrc, &pStatus->stStatus.stEvent.stStartDate);
	pucSrc += ucRetLen + 2;

	ucRetLen = local_cxsc_GetTime(pucSrc, &pStatus->stStatus.stEvent.stEndDate);
	pucSrc += ucRetLen + 2;

	pStatus->stStatus.stEvent.ucAuthType = get8bit(pucSrc);
	pucSrc ++;

	if (pStatus->stStatus.stEvent.ucAuthType == eCxAuth_TimeLimitedAccess ||	//
		pStatus->stStatus.stEvent.ucAuthType == eCxAuth_CreditBasedAccess)
	{
		pStatus->stStatus.stEvent.usLeft = get16bit(pucSrc);
	}
	else
	{
		pStatus->stStatus.stEvent.usLeft = 0;
	}

	nErr = local_cxsc_AddStatusList(pstInstance, pStatus);

	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParsePurseStatus(CxScInstance_t* pstInstance, HUINT32 ulLen, HUINT8* pucData)
{
	CxStatusList_t* pStatus;
	HUINT8* 		pucSrc;
	HUINT8			ucRetLen		= 0;
	HERROR			nErr;

	ENTER_FUNCTION;

	pStatus = CX_MEM_Alloc(sizeof(CxStatusList_t));
	if (pStatus == NULL)
	{
		return ERR_FAIL;
	}

	pucSrc = pucData;

	pStatus->stStatus.stPurse.usPurseRef = get16bit(pucSrc);
	pucSrc += 2;

	ucRetLen = local_cxsc_GetAsciiText(pucSrc, pStatus->stStatus.stPurse.szLabel);
	pucSrc += ucRetLen + 2;

	pStatus->stStatus.stPurse.ulBalance = get32bit(pucSrc);

	nErr = local_cxsc_AddStatusList(pstInstance, pStatus);

	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParseDebitStatus(CxScInstance_t* pstInstance, HUINT32 ulLen, HUINT8* pucData)
{
	CxStatusList_t* pStatus;
	HUINT8* 		pucSrc;
	HUINT8			ucRetLen		= 0;
	HERROR			nErr;

	ENTER_FUNCTION;

	pStatus = CX_MEM_Alloc(sizeof(CxStatusList_t));
	if (pStatus == NULL)
	{
		return ERR_FAIL;
	}

	pucSrc = pucData;

	pStatus->stStatus.stDebit.ulEventTag = get24bit(pucSrc);
	pucSrc += 3;

	pStatus->stStatus.stDebit.ulConsumedTokens = get24bit(pucSrc);
	pucSrc += 3;

	ucRetLen = local_cxsc_GetTime(pucSrc, &pStatus->stStatus.stDebit.stLastAccessTime);
	pucSrc += ucRetLen + 2;

	if (pucSrc < pucData + ulLen)
	{
		ucRetLen = local_cxsc_GetAsciiText(pucSrc, pStatus->stStatus.stDebit.szLabel);
	}
	else
	{
		pStatus->stStatus.stDebit.szLabel[0] = 0;
	}

	nErr = local_cxsc_AddStatusList(pstInstance, pStatus);

	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParseCreditStatus(CxScInstance_t* pstInstance, HUINT32 ulLen, HUINT8* pucData)
{
	CxStatusList_t* pStatus;
	HUINT8* 		pucSrc;
	HUINT8			ucRetLen		= 0;
	HERROR			nErr;

	ENTER_FUNCTION;

	pStatus = CX_MEM_Alloc(sizeof(CxStatusList_t));
	if (pStatus == NULL)
	{
		return ERR_FAIL;
	}

	pucSrc = pucData;

	pStatus->stStatus.stCredit.ulCreditTokens = get32bit(pucSrc);
	pucSrc += 4;

	if (pucSrc < pucData + ulLen)
	{
		ucRetLen = local_cxsc_GetAsciiText(pucSrc, pStatus->stStatus.stCredit.szLabel);
	}
	else
	{
		pStatus->stStatus.stCredit.szLabel[0] = 0;
	}

	nErr = local_cxsc_AddStatusList(pstInstance, pStatus);

	(void)ucRetLen;
	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParsePinStatus(Handle_t hSession, HUINT32 ulLen, HUINT8* pucData)
{
#if 1
	HxLOG_Error("[To be implemented]\r\n");
	return ERR_FAIL;
#else
	CxPinStatus_t* 	pStatus;
	HUINT8* 		pucSrc;
	HERROR			nErr;

	ENTER_FUNCTION;

	pStatus = CX_MEM_Alloc(sizeof(CxPinStatus_t));
	if (pStatus == NULL)
	{
		return ERR_FAIL;
	}

	pucSrc = pucData;

	pStatus->ulKeyId = get32bit(pucSrc);
	pucSrc += 4;

	pStatus->ucPresentationCounter = (*pucSrc & 0x80);
	pStatus->ucPinChosenByUser = (*pucSrc & 0x40);
	pStatus->ucPinSet = (*pucSrc & 0x20);

	LEAVE_FUNCTION;
	return nErr;
#endif
}


#define __ReturnChannel______________________________________________

//RetChan - ReqOrderApdu
HERROR CXSC_RequestOrderApdu(Handle_t hSession, HUINT8* pucPin, HUINT32 ulProdId)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_RetChan, CX_PL_ReqOrderApdu + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_ReqOrderApdu;
	*pucCmd++ = CX_PL_ReqOrderApdu;

	CX_memcpy(pucCmd, pucPin, 4);

	local_cxsc_SetProductId(pucCmd + 4, ulProdId);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//RetChan - OrderRespApdu
HERROR CXSC_OrderResponseApdu(Handle_t hSession, HUINT16 usLen, HUINT8* pucApdu)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_RetChan, usLen + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_OrderRespApdu;
	*pucCmd++ = usLen;

	CX_memcpy(pucCmd, pucApdu, usLen);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//RetChan - ReqRetChanConfig
HERROR CXSC_ReturnChannelConfig(Handle_t hSession)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_RetChan, CX_PL_ReqRetChanConfig + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_ReqRetChanConfig;
	*pucCmd = CX_PL_ReqRetChanConfig;

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//RetChan - ReqDebitLog
HERROR CXSC_RequestDebitLog(Handle_t hSession, HUINT16 usPurseRef, HUINT32 ulCutOffDate)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HUINT8			ucCmdLen;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	if (ulCutOffDate)
	{
		ucCmdLen = 6;
	}
	else
	{
		ucCmdLen = 2;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_RetChan, ucCmdLen + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_ReqDebitLog;
	*pucCmd++ = ucCmdLen;

	put16bit(pucCmd, usPurseRef);

	if (ulCutOffDate)
	{
		put32bit(pucCmd + 2, ulCutOffDate);
	}

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParseOrderReqApdu(Handle_t hSession, HUINT32 ulLen, HUINT8* pucData)
{
	ENTER_FUNCTION;

	//## pucData가 바로 return channel로 돌려보낼 APDU이니 요걸 넘겨서 처리하면 된다.
	// 이 과정은 task 통해서 하자.. 시간이 오래걸릴 수도 있으니까...
	// s/c에서 ORDER_REQ_APDU를 받으면 즉시 connection 시도하고.. 성공하면 nothing, 실패하면 order failed
	// order를 보내는데, 성공하면 nothing, 실패하면 order failed
	// order 접수되었다는 응답을 받으면, order accepted, 실패하면 order failed

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ParseRetChanConfig(Handle_t hSession, HUINT32 ulLen, HUINT8* pucData)
{
	ENTER_FUNCTION;

	//## return channel 종류 마다 다르다... 나중에 필요하면 그때 작업

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ParseDebitLogData(Handle_t hSession, HUINT32 ulLen, HUINT8* pucData)
{
	ENTER_FUNCTION;

	//## pucData가 바로 return channel로 돌려보낼 Debit log data이니 요걸 넘겨서 처리하면 된다.

	LEAVE_FUNCTION;
	return ERR_OK;
}

#define __Security______________________________________________

STATIC HUINT32 local_cxsc_GetSecuDataLen(CxSecuData_t* pstMsg)
{
	HUINT32	ulLen;

	ulLen = pstMsg->ucDataLen + 2;

	if (pstMsg->eDataTag != eCxData_PlainText && pstMsg->eDataTag != eCxData_SignPin)
	{
		ulLen += 1;
	}

	return ulLen;
}

STATIC void local_cxsc_SetSecuData(HUINT8* pucBuf, CxSecuData_t* pstMsg)
{
	HUINT8* pucDest;

	pucDest = pucBuf;

	*pucDest++ = (HUINT8)pstMsg->eDataTag;

	if (pstMsg->eDataTag == eCxData_PlainText)
	{
		*pucDest++ = pstMsg->ucDataLen;
	}
	else if (pstMsg->eDataTag == eCxData_SignPin)
	{
		*pucDest++ = pstMsg->ucDataLen;
	}
	else
	{
		*pucDest++ = pstMsg->ucDataLen + 1;
		*pucDest++ = pstMsg->ucAlgorithm;
	}

	CX_memcpy(pucDest, pstMsg->pucData, pstMsg->ucDataLen);
}

//SecurityOp - GenerateSign
HERROR CXSC_GenerateSign(Handle_t hSession, HUINT8 ucAlgorithm, HUINT32 ulKeyId, CxSecuData_t* pstDigest, CxSecuData_t* pstSignPin)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HUINT8			ucCmdLen;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	if (ulKeyId == 0xFFFFFFFF)
	{
		ucCmdLen = local_cxsc_GetSecuDataLen(pstDigest) + local_cxsc_GetSecuDataLen(pstSignPin) + 1 + 4;
	}
	else
	{
		ucCmdLen = local_cxsc_GetSecuDataLen(pstDigest) + 1 + 4;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_SecurityOp, ucCmdLen + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_GenerateSign;
	*pucCmd++ = ucCmdLen;
	*pucCmd++ = ucAlgorithm;
	put32bit(pucCmd, ulKeyId);
	local_cxsc_SetSecuData(pucCmd + 4, pstDigest);

	if (ulKeyId == 0xFFFFFFFF)
	{
		local_cxsc_SetSecuData(pucCmd + local_cxsc_GetSecuDataLen(pstDigest) + 4, pstSignPin);
	}

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//SecurityOp - VerifySign
HERROR CXSC_VerifySign(Handle_t hSession, HUINT32 ulKeyId, CxSecuData_t* pstDigest, CxSecuData_t* pstSignature)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HUINT8			ucCmdLen;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	ucCmdLen = local_cxsc_GetSecuDataLen(pstDigest) + local_cxsc_GetSecuDataLen(pstSignature) + 4;

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_SecurityOp, ucCmdLen + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_VerifySign;
	*pucCmd++ = ucCmdLen;
	put32bit(pucCmd, ulKeyId);
	local_cxsc_SetSecuData(pucCmd + 4, pstDigest);
	local_cxsc_SetSecuData(pucCmd + local_cxsc_GetSecuDataLen(pstDigest) + 4, pstSignature);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//SecurityOp - EncryptMsg
HERROR CXSC_EncryptMsg(Handle_t hSession, HUINT8 ucAlgorithm, HUINT32 ulKeyId, CxSecuData_t* pstPlainText)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HUINT8			ucCmdLen;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	ucCmdLen = local_cxsc_GetSecuDataLen(pstPlainText) + 1 + 4;

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_SecurityOp, ucCmdLen + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_EncryptMsg;
	*pucCmd++ = ucCmdLen;
	*pucCmd++ = ucAlgorithm;
	put32bit(pucCmd, ulKeyId);
	local_cxsc_SetSecuData(pucCmd + 4, pstPlainText);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//SecurityOp - DecryptMsg
HERROR CXSC_DecryptMsg(Handle_t hSession, HUINT32 ulKeyId, CxSecuData_t* pstCipherText, CxSecuData_t* pstSignPin)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HUINT8			ucCmdLen;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	if (ulKeyId == 0xFFFFFFFF)
	{
		ucCmdLen = local_cxsc_GetSecuDataLen(pstCipherText) + local_cxsc_GetSecuDataLen(pstSignPin) + 1 + 4;
	}
	else
	{
		ucCmdLen = local_cxsc_GetSecuDataLen(pstCipherText) + 1 + 4;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_SecurityOp, ucCmdLen + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_DecryptMsg;
	*pucCmd++ = ucCmdLen;
	put32bit(pucCmd, ulKeyId);
	local_cxsc_SetSecuData(pucCmd + 4, pstCipherText);

	if (ulKeyId == 0xFFFFFFFF)
	{
		local_cxsc_SetSecuData(pucCmd + local_cxsc_GetSecuDataLen(pstCipherText) + 4, pstSignPin);
	}

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//SecurityOp - ReqBlockSize
HERROR CXSC_RequestBlockSize(Handle_t hSession, HUINT8 ucAlgorithm, HUINT32 ulKeyId)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_SecurityOp, CX_PL_ReqBlockSize + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_ReqBlockSize;
	*pucCmd++ = CX_PL_ReqBlockSize;
	*pucCmd++ = ucAlgorithm;
	put32bit(pucCmd, ulKeyId);

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//SecurityOp - ReqCardNumber
HERROR CXSC_RequestCardNumber(HINT32 nScSlot)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	HxLOG_Error("[Enter(%d)]\r\n", nScSlot);

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);

	pstInstance = s_pstCxScInstance[nScSlot];

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, 0, eCxSc_SecurityOp, CX_PL_ReqCardNumber + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_ReqCardNumber;
	*pucCmd = CX_PL_ReqCardNumber;

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, HANDLE_NULL);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//SecurityOp - ReqSequenceNumber
HERROR CXSC_RequestSequenceNumber(Handle_t hSession)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_SecurityOp, CX_PL_ReqSequenceNumber + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_ReqSequenceNumber;
	*pucCmd = CX_PL_ReqSequenceNumber;

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

//SecurityOp - ReqRandomOctets
HERROR CXSC_RequestRandomOctets(Handle_t hSession, HUINT8 ucRandomLength)
{
	CxScInstance_t* pstInstance;
	HUINT8* 		pucCmd;
	HERROR			nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxScSemId);
	pstInstance = local_cxsc_GetInstance(hSession);
	if (pstInstance == NULL)
	{
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
	}

	// header 만들기
	local_cxsc_MakeHeader(pstInstance, CXSESSION_GetSessionIdx(hSession), eCxSc_SecurityOp, CX_PL_ReqRandomOctets + 2);

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	*pucCmd++ = (HUINT8)eCxSc_ReqRandomOctets;
	*pucCmd++ = CX_PL_ReqRandomOctets;
	*pucCmd = ucRandomLength;

	// 보내기
	nErr = local_cxsc_ProcessCommand(pstInstance, hSession);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParseSignatureResp(Handle_t hSession, HUINT32 ulLen, HUINT8* pucData)
{
	ENTER_FUNCTION;

	//## pucData가 바로 signature인데 어떻게 써먹나???

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ParseEncryptedMsg(Handle_t hSession, HUINT32 ulLen, HUINT8* pucData)
{
	ENTER_FUNCTION;

	//## pucData가 바로 cipher text 인데 어떻게 써먹나???

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ParseDecryptedMsg(Handle_t hSession, HUINT32 ulLen, HUINT8* pucData)
{
	ENTER_FUNCTION;

	//## pucData가 바로 plain text 인데 어떻게 써먹나???

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ParseRandomOctets(Handle_t hSession, HUINT32 ulLen, HUINT8* pucData)
{
	ENTER_FUNCTION;

	//## pucData가 바로 random octet bytes 인데 어떻게 써먹나???

	LEAVE_FUNCTION;
	return ERR_OK;
}

#define __HostData____________________________________________________________
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
STATIC HERROR local_cxsc_ParseFingerprint(CxScInstance_t* pstInstance, HUINT32 ulLen, HUINT8* pucData, CxHostDataPI_t ucHostDataId, Handle_t hEcm)
{
	Handle_t			hStbHandle = HANDLE_NULL, hSession = HANDLE_NULL;
	SvcCas_ActionType_e	eActType;

	if (ucHostDataId == eCxHostData_Fingerprint)
	{
		HUINT8 *pCTemp = pucData;

		// conax 6.0 chipset pairing 136p - all fingerprints received through ECMs shall be imposed immediately
		if (CxSc_GetInParam(pstInstance) == eCxSc_Ecm)
		{
			pstInstance->stFingerprint.ulTime = 0xffffff;
		}
		else
		{
			pstInstance->stFingerprint.ulTime = get24bit(pucData);
		}
		pstInstance->stFingerprint.ucCentTime[0] = ((*pCTemp) >> 3);										// bits(23 - 19) : hour(0-23)
		pstInstance->stFingerprint.ucCentTime[1] = (((*pCTemp)&0x07) << 3 | ((*(pCTemp + 1)) >> 5));		// bits(18 - 13) : min(0-59)
		pstInstance->stFingerprint.ucCentTime[2] = (((*(pCTemp + 1)) & 0x1F) << 1 | ((*(pCTemp + 2)) >> 7));	// bits(12 -  7) : sec(0-59)
		pstInstance->stFingerprint.ucCentTime[3] = ((*(pCTemp + 2)) & 0x7F);								// bits( 6 -  0) : cent(0-99)

		pstInstance->stFingerprint.usDuration = get16bit(pucData + 3);
		pstInstance->stFingerprint.usPosX = get16bit(pucData + 3 + 2);
		pstInstance->stFingerprint.usPosY = get16bit(pucData + 3 + 2 + 2);
		pstInstance->stFingerprint.usHeight = get8bit(pucData + 3 + 2 + 2 + 2);

		// Protected Data인지 확인용
		pstInstance->stFingerprint.bFPFromProtectedData = pstInstance->bFPFromProtectedData;

		HxLOG_Print(" Full time info(0x%x), CentTime(%x:%x:%x:%x)\n",
										pstInstance->stFingerprint.ulTime,
										pstInstance->stFingerprint.ucCentTime[0],
										pstInstance->stFingerprint.ucCentTime[1],
										pstInstance->stFingerprint.ucCentTime[2],
										pstInstance->stFingerprint.ucCentTime[3]);

		HxLOG_Print("[FP Info] dur(%d), Height(%d), bFPFromProtectedData(%d)\n",
										pstInstance->stFingerprint.usDuration,
										pstInstance->stFingerprint.usHeight,
										pstInstance->stFingerprint.bFPFromProtectedData);
	}
	else if (ucHostDataId == eCxHostData_FingerprintId)
	{
		pstInstance->stFingerprint.ucDisplayed = FALSE;
		CX_memcpy(pstInstance->stFingerprint.szText, pucData, ulLen);
		pstInstance->stFingerprint.szText[ulLen] = '\0';
	}

	if (pstInstance->stFingerprint.usDuration == 0)
		return ERR_OK;

	switch(CxSc_GetInParam(pstInstance))
	{
		case eCxSc_Ecm:
#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_SUPPORT_FCC)
			hStbHandle = CXECM_GetStbHandle(hEcm);
			if (hStbHandle == HANDLE_NULL)
			{
				HxLOG_Error("CXECM_GetStbHandle() failed!! ! \n");
				return ERR_FAIL;
			}

			eActType = CXACT_GetActionTypeFromStbHandle(hStbHandle);
#else
			hStbHandle = HANDLE_NULL;
			eActType = eCAS_ActionType_Live;
#endif
			break;

		case eCxSc_Emm:
			hStbHandle = HANDLE_NULL;
			eActType = eCAS_ActionType_Live;
			break;

		default:
			eActType = eCAS_ActionType_None;
			break;
	}

	switch(eActType)
	{
		case eCAS_ActionType_Live:
			if (pstInstance->stFingerprint.ulTime != CX_FINGER_PRINT_IMMEDIATELY_TIME)	// immediate인 경우에는 저장하지 않는다.
			{
				if (CXNVM_WriteFingerprint(&pstInstance->stFingerprint) != ERR_OK)
				{
					HxLOG_Error("CXNVM_WriteFingerprint() error\n");
					return ERR_FAIL;
				}
			}

			if(hStbHandle != HANDLE_NULL)
				hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);
			else
				hSession = HANDLE_NULL;

			// 화면에도 뿌리자.. pstFingerprint가 바로 free되는 것에 주의
			CXUI_FingerprintReceived(&pstInstance->stFingerprint, hSession);
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eCAS_ActionType_Recording:
		case eCAS_ActionType_Tsr_Rec:
			if(hStbHandle != HANDLE_NULL)
			{
				CXAPI_PVR_UpdateFingerprintToMetaFile(CXACT_GetActionHandle(hStbHandle), &pstInstance->stFingerprint);
			}
			break;
#endif
		default:
			break;
	}

	return ERR_OK;
}
#endif
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
STATIC HERROR local_cxsc_ParseUserText(CxScInstance_t* pstInstance, HUINT32 ulLen, HUINT8* pucData)
{
	HUINT8* 	pucApdu, * pucPV;
	HUINT8		ucPI, ucPL;
	HUINT8		ucPreviousPI	= 0xff;			/*중복 PI를 처리하기 위함*/
	HUINT32		ulPiRecvState	= 0;				/* 받은 pi를 여기에 기록해 둔다 */
	HUINT32		ulIdx;
	CxTime_t	stStartTime;
	HBOOL		bStartTimeOmit	= TRUE;
#if defined(CONFIG_DEBUG)
	HUINT32		i, j			= 1;
#endif
	UNIXTIME	ulUnixtime		= 0;
	HERROR		hErr			= ERR_OK;

	ENTER_FUNCTION;

	// clear all data
	CX_memset(&pstInstance->stUserText, 0x0, sizeof(CxUserText_t));

	pucApdu = pucData;
	ulIdx = 0;

	while (ulIdx < ulLen)
	{
		ucPI = *pucApdu;
		ucPL = *(pucApdu + 1);
		pucPV = pucApdu + 2;
#if defined(CONFIG_DEBUG)
		HxLOG_Error("\nUSER_TEXT parameters -> PI [%02x], PL [%02x]\n\tPV >>> ", ucPI, ucPL);
		for (i = 0; i < 16; i++)
		{
			HxLOG_Print("%02x ", i);
		}
		HxLOG_Print("\n\t======================================================\n\t[00] : ");
		for (i = 0; i < ucPL; i++)
		{
			HxLOG_Print("%02x", *(pucPV + i));
			if (i < ucPL - 1)
			{
				HxLOG_Print(" ");
			}
			if (((i + 1) % 16) == 0)
			{
				HxLOG_Print("\n\t[%02x] : ", j++);
			}
		}
		HxLOG_Print("\n");
#endif
		if (ucPI == 0)
		{
			// NULL 로 padding 되어있는 경우, (ucPI==0x0 인 case를 위함).
			break;
		}

		if (ucPI == ucPreviousPI)
		{
			HxLOG_Print("\n==>PI_PARAMETER is equal OldParam(0x%x) with NewParam(0x%x).\r\n", ucPreviousPI, ucPI);
			return ERR_FAIL;
		}

		ucPreviousPI = ucPI;

		switch (ucPI)
		{
		case eCxUserText_Text:
			/* ucPL은 HUINT8 이므로 어차피 255를 넘을 수 없음
			if (ucPL > CX_MAX_USER_TEXT_LEN)
			{
				HxLOG_Print("\n==> Invliad user text length(%d) -> set max len(%d) \n", ucPL, CX_MAX_USER_TEXT_LEN);
				ucPL = CX_MAX_USER_TEXT_LEN;
			}
			*/
			CX_memcpy(pstInstance->stUserText.szText, pucPV, ucPL);
			pstInstance->stUserText.szText[ucPL] = '\0';
			ulPiRecvState |=	CX_USERTEXT_TEXT;
			break;

		case eCxUserText_SeqNo:
			pstInstance->stUserText.ucSeqNo = get8bit(pucPV);
			ulPiRecvState |=	CX_USERTEXT_SEQNO;
			break;

		case eCxUserText_Start:
			//ulPiRecvState |= CX_USERTEXT_START;
			if (local_cxsc_GetTime(pucApdu, &stStartTime))	// local_cxsc_GetTime는 PI/PL도 같이 넘겨야 해서 pucApdu를 쓴다
			{
				// Modify by Jhlee
				// STB shall use the clock. Start Time omitted, the STB shall display the text immediately.
				pstInstance->stUserText.stStartDataTime.stDate.usYear = stStartTime.usYear;
				pstInstance->stUserText.stStartDataTime.stDate.ucMonth = stStartTime.ucMonth;
				pstInstance->stUserText.stStartDataTime.stDate.ucDay = stStartTime.ucDay;
				pstInstance->stUserText.stStartDataTime.stTime.ucHour = stStartTime.ucHour;
				pstInstance->stUserText.stStartDataTime.stTime.ucMinute = stStartTime.ucMinute;
				pstInstance->stUserText.stStartDataTime.stTime.ucSecond = 0;
				pstInstance->stUserText.stStartDataTime.stTime.usMillisecond = 0;

				bStartTimeOmit = FALSE;

				// add offset. - schedule mailbox
				HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&pstInstance->stUserText.stStartDataTime, &ulUnixtime);
				HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixtime, &pstInstance->stUserText.stStartDataTime);
				HxLOG_Print("\n==> eCxUserText_Start Year(%4d), Month(%2d), Day(%2d)\n", stStartTime.usYear, stStartTime.ucMonth, stStartTime.ucDay);
			}
			break;

		case eCxUserText_Duration:
			pstInstance->stUserText.usDuration = get16bit(pucPV);
			ulPiRecvState |=	CX_USERTEXT_DURATION;
			break;

		case eCxUserText_Repetition:
			pstInstance->stUserText.ucRepeat = get8bit(pucPV);
			ulPiRecvState |=	CX_USERTEXT_REPETITION;
			break;

		case eCxUserText_Interval:
			pstInstance->stUserText.usInterval = get8bit(pucPV) * 10;
			ulPiRecvState |=	CX_USERTEXT_INTERVAL;
			break;

		default:
			// format이 정해져 있으므로 모르는 pi만 무시하고 다음 pi를 보자
			break;
		}
		pucApdu += ucPL + 2;
		ulIdx += ucPL + 2;
	}

#ifdef MAKE_TEST_DATA_FOR_PT
	// ts6a.trp Conax 1. 여기에 PI_REPETITION, PI_INTERVAL을 강제로 할당하면 Immediate UserText with REPETITION and INTERVAL STBY 테스트 가능
	pstInstance->stUserText.ucRepeat = 3;
	ulPiRecvState |=	CX_USERTEXT_REPETITION;
	pstInstance->stUserText.usInterval = 2 * 10;
	ulPiRecvState |=	CX_USERTEXT_INTERVAL;
#endif

	// check mandatory PI
	if ((ulPiRecvState & CX_USERTEXT_MANDATORY_ALL) != CX_USERTEXT_MANDATORY_ALL)
	{
		HxLOG_Print("\n==> Invliad user text state. ulPiRecvState(0x%X)\n", ulPiRecvState);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// check optional madatory PI
	if ((ulPiRecvState & CX_USERTEXT_REPETITION) == CX_USERTEXT_REPETITION)
	{
		if ((ulPiRecvState & CX_USERTEXT_INTERVAL) != CX_USERTEXT_INTERVAL)
		{
			// 항상 pair로 들어와야 하는 PI들..
			HxLOG_Print(" ==> Invliad user text state.. ulPiRecvState(0x%X)\n", ulPiRecvState);
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}
	else
	{
		if ((ulPiRecvState & CX_USERTEXT_INTERVAL) == CX_USERTEXT_INTERVAL)
		{
			// 항상 pair로 들어와야 하는 PI들..
			HxLOG_Print(" ==> Invliad user text state.. ulPiRecvState(0x%X)\n", ulPiRecvState);
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}
	// check duration and interval
	/**
	  *	Basic STB IK 5.0, Spec and Guidelines, 80page
	  *
	  *	 Hence, the value of PI_INTERVAL should be greater than the value of PI_DURATION,
	  * 	otherwise the text will be displayed continuosly until the las repetition is completed.
	  *	...
	  *	 If PI_INTERVAL is omitted, the STB shall display the text message only once
	**/
	if ((pstInstance->stUserText.usInterval > 0)	//
		&& (pstInstance->stUserText.usDuration >= pstInstance->stUserText.usInterval))
	{
		HxLOG_Print(" ==> Invliad duration! usDuration(%d) >=  usInterval(%d)\n", pstInstance->stUserText.usDuration, pstInstance->stUserText.usInterval);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pstInstance->stUserText.ucRepeat == 0)
	{
		pstInstance->stUserText.ucRepeat = 1;
	}

	// Start Time 이 생략된 UserText
	if (bStartTimeOmit == TRUE)
	{
		/* Immediate message init */
		UNIXTIME	ulStartTime	= 0;
		HINT32		nStandbyMode = 0;
		// PI_START 없는 immediate message에서 repeat이 들어오는 경우 무시하도록함.
		HERROR		hError		= ERR_FAIL;

		PAPI_GetLastStandby(&nStandbyMode);

		if(nStandbyMode || CXUI_GetUiDisplayedState((HINT32)pstInstance->ucScSlot))
		{
			HxLOG_Print("In the Standby Mode or Ui displayed...\n");
			return ERR_OK;
		}

		// basic STB integration kit 6.0 70p - PI_INTERVAL / PI_REPETITION shall be ignored if PI_START is omitted.
		if ((ulPiRecvState & CX_USERTEXT_REPETITION) == CX_USERTEXT_REPETITION)
		{
			pstInstance->stUserText.ucRepeat = 1;
		}
		if ((ulPiRecvState & CX_USERTEXT_INTERVAL) == CX_USERTEXT_INTERVAL)
		{
			pstInstance->stUserText.usInterval = 0;
		}

		HxLOG_Error(" ==> StartTime omitted -> ucRepeat:%d , usInterval:%d\n", pstInstance->stUserText.ucRepeat, pstInstance->stUserText.usInterval);
		hError = CXNVM_CompareUserTextWithoutStartTime(&pstInstance->stUserText);
		if (hError == ERR_OK)
		{
			HxLOG_Error("Same MSG is already stored\r\n");
			// 같은 것이 있으면 Immediate message가 display 된 적이 있는지 체크.
			hError = CXNVM_IsAlreadyDisplayed(&pstInstance->stUserText);
			if (hError == ERR_OK)
			{
				// Immediate message가 display 된 적이 있으면 아무것도 하지 않는다.
				HxLOG_Error("The MSG already have been displayed.\r\n");
				LEAVE_FUNCTION;
				return ERR_OK;
			}
		}

		// OP 요청으로 Start Time 이 없을 경우 STB Time 출력 한다.
		// Start Time에 STB Time으로 setting하는 이유는 Mail Message인 경우 메시지 받은 시각을 출력해야 하기 때문이다.
		// 따라서 Mail Message인 경우만 GetTime을 하여 Setting 하고, Immediate Conax Message인 경우는 시간 설정을 하지 않도록 한다.
		// 그 이유는 VK GetTime을 해서 던진 event가 xmgr_cas쪽에서 처리할 때 VK GetTime을 다시하고 시간을 비교하게 되는데,
		// event 처리과정이서 delay된 시간에 의하여 초단위 시간이 바뀌었다면 UI가 출력되지 않을 가능성이 있다.
		// 또한 Start Time이 없을 경우는 Immediate Message 이므로 현재시간을 Start Data Time으로 설정할 필요 없이 0으로 설정하여 xmgr_cas쪽에서 즉시 출력하도록 하는것이 올바르다.
		if(pstInstance->stUserText.usDuration == 0)	// Mail Message를 나타냄.
		{
			hError = VK_CLOCK_GetTime((HULONG *)&ulStartTime);
			if (hError != ERR_OK)
			{
				// Mail Message인 경우 꼭 시간이 설정 되어있어야 정상 출력이 된다.
				HxLOG_Critical("Critical Error for Mail message...\n");
				ulStartTime = 0;
				HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulStartTime, &(pstInstance->stUserText.stStartDataTime));
			}
			else
			{
				HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &(pstInstance->stUserText.stStartDataTime));
			}
		}
		else
		{
			ulStartTime = 0;
			HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulStartTime, &(pstInstance->stUserText.stStartDataTime));
		}
	}
	else
	{
		hErr = CXUI_CheckScheduledMsgAvailable(&pstInstance->stUserText);
		if (hErr != ERR_OK)
		{
			// 지나간 scheduled msg는 display 하지 않는다.
			LEAVE_FUNCTION;
			return ERR_OK;
		}
	}

	// Repeat Immediate Message 를 위한 Current Time 초기화
	pstInstance->stUserText.ulImmediateStartTime = 0;

	// 저장하자..
	if (CXNVM_WriteUserText(&pstInstance->stUserText, bStartTimeOmit) == ERR_OK)
	{
		// 화면에도 뿌리자..
		CXUI_UserTextReceived(&pstInstance->stUserText, eCxUTFlagFromTS);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_CONAX_CPM)
static HERROR local_cxsc_UpdateKey(CxScInstance_t* pstInstance, const HUINT8* pucPV, HUINT8 ucPL)
{
	HERROR	err;
	HUINT32	ulChipId;
	HUINT16	usKeyId;
	HUINT8	ucType;

	ulChipId = get32bit(pucPV);
	pucPV += 4;
	ucType = get8bit(pucPV) & 0x07;
	pucPV++;
	usKeyId = get16bit(pucPV);
	pucPV += 2;

	err = CXCPM_Process_UPDATE_KEY(
				NULL/*Reserved*/,
				ulChipId,
				(CxCpmKeyType_t)ucType,
				usKeyId,
				pucPV,
				ucPL
			);

#ifdef DUMP_SMARTCARD_DATA
	VK_Print("> UPDATE_KEY:0X%X \n", usKeyId);
#endif
	if (err == ERR_OK && ucType == (CxCpmKeyType_t)eCxCpm_CWPK)
	{
		// TODO: CWPK Updated!!
		// TODO: DVB Chipset에 Load 하는것인지, DSCR 에 CW와 Set 한것으로 끝인지...모르겠다..
	}
	return err;
}

static HERROR local_cxsc_RemoveKey(CxScInstance_t* pstInstance, const HUINT8* pucPV)
{
	HERROR	err;
	HUINT32	ulChipId;
	HUINT16	usKeyId;

	ulChipId = get32bit(pucPV);
	pucPV += 4;
	usKeyId = get16bit(pucPV);
	pucPV += 2;

	err = CXCPM_Process_REMOVE_KEY(
				NULL/*Reserved*/,
				ulChipId,
				usKeyId
			);

#ifdef DUMP_SMARTCARD_DATA
	VK_Print("> REMOVE_KEY:0X%X \n", usKeyId);
#endif
	return err;
}

static HERROR local_cxsc_ParseChipsetPairing(CxScInstance_t* pstInstance, const HUINT8* pucData, HUINT8 ucLen)
{
	HUINT8	ucPI			= 0;
	HUINT8	ucPL			= 0;
	HUINT8* pucPV			= NULL;

	HUINT8	ucPos;
	HERROR	err;

	ENTER_FUNCTION;

	ucPos = 0;
	while (ucPos < ucLen)
	{
		ucPI = *pucData++;
		ucPL = *pucData++;
		pucPV = (HUINT8 *)pucData;

		switch (ucPI)
		{
		case 0x01:
			// UPDATE_KEY
			err = local_cxsc_UpdateKey(pstInstance, pucPV, ucPL);
			break;

		case 0x02:
			// REMOVE_KEY
			err = local_cxsc_RemoveKey(pstInstance, pucPV);
			break;

		default:
			err = ERR_FAIL;
			break;
		}
		pucData += ucPL;
		ucPos += (ucPL + 2);// 2 = PI & PL index
	}


	NOT_USED_PARAM(err);
	LEAVE_FUNCTION;

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_CONAX_URI)
HUINT8 CXSC_ConvertUriMrToScMr(eDxCopyrightControl_ConaxMaturityRating_e eUriMr)
{
	HUINT8 ucScMr;

	switch(eUriMr)
	{
		case eDxCopyrightControl_ConaxMaturityRating_Off:
			ucScMr = CX_MATURITY_RATE_OFF;
			break;

		case eDxCopyrightControl_ConaxMaturityRating_G:
			ucScMr = CX_MATURITY_RATE_G;
			break;

		case eDxCopyrightControl_ConaxMaturityRating_PG:
			ucScMr = CX_MATURITY_RATE_PG;
			break;

		case eDxCopyrightControl_ConaxMaturityRating_A:
			ucScMr = CX_MATURITY_RATE_A;
			break;

		case eDxCopyrightControl_ConaxMaturityRating_X:
			ucScMr = CX_MATURITY_RATE_X;
			break;

		case eDxCopyrightControl_ConaxMaturityRating_XXX:
			ucScMr = CX_MATURITY_RATE_XXX;
			break;

		default:
			ucScMr = CX_MATURITY_RATE_G;
			break;
	}

	return ucScMr;
}

eDxCopyrightControl_ConaxMaturityRating_e CXSC_ConvertScMrToUriMr(HUINT8 ucScMr)
{
	eDxCopyrightControl_ConaxMaturityRating_e eUriMr;

	switch(ucScMr)
	{
		case CX_MATURITY_RATE_OFF:
			eUriMr = eDxCopyrightControl_ConaxMaturityRating_Off;
			break;

		case CX_MATURITY_RATE_G:
			eUriMr = eDxCopyrightControl_ConaxMaturityRating_G;
			break;

		case CX_MATURITY_RATE_PG:
			eUriMr = eDxCopyrightControl_ConaxMaturityRating_PG;
			break;

		case CX_MATURITY_RATE_A:
			eUriMr = eDxCopyrightControl_ConaxMaturityRating_A;
			break;

		case CX_MATURITY_RATE_X:
			eUriMr = eDxCopyrightControl_ConaxMaturityRating_X;
			break;

		case CX_MATURITY_RATE_XXX:
			eUriMr = eDxCopyrightControl_ConaxMaturityRating_XXX;
			break;

		default:
			eUriMr = eDxCopyrightControl_ConaxMaturityRating_G;
			break;
	}

	return eUriMr;
}

HERROR CXSC_GetDefaultUri(DxCopyrightControl_ConaxDrmInfo_t* pstDrmInfo)
{
	if (pstDrmInfo == NULL)
	{
		HxLOG_Error("pstDrmInfo NULL !! \n");
		return ERR_FAIL;
	}

	CX_memset(pstDrmInfo, 0, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

	// see Chipset pairing STB Integration Kit 5.0-A PRE, URI Processing Extensions
	pstDrmInfo->eApsCopyCtrl = eDxCopyrightControl_ConaxApsCopyCtrl_Off;
	pstDrmInfo->eEmiCopyCtrl = eDxCopyrightControl_ConaxEmiCopyCtrl_Once;
	pstDrmInfo->eICT = eDxCopyrightControl_ConaxICT_Off;
	pstDrmInfo->eRedistributionCtrl = eDxCopyrightControl_ConaxRedistributionCtrl_Off;
	pstDrmInfo->eRetentionLimit = eDxCopyrightControl_ConaxRetentionLimit_NoExpLimit;
	pstDrmInfo->eTrickPlayCtrl = eDxCopyrightControl_ConaxTrickPlayCtrl_NoRestriction;
	pstDrmInfo->eMR = eDxCopyrightControl_ConaxMaturityRating_Off;
	pstDrmInfo->eDisableAnalOutput = eCxDisableAnalOutput_Off;

	return ERR_OK;
}

static HERROR local_cxsc_ParseBridgeControl(CxScInstance_t* pstInstance, const HUINT8* pucData, HUINT8 ucLen, Handle_t hEcm)
{
	HUINT8		*pucPv = NULL, ucExportControlLen, ucExportConditionslLen, ucUriLen;
	HUINT16		usUri;
	Handle_t	hStbHandle;
	DxCopyrightControl_ConaxDrmInfo_t* 	pstDrmInfo;

	ENTER_FUNCTION;

	pucPv = (HUINT8 *)pucData;
	if (pucPv == NULL)
	{
		HxLOG_Error("pucPv is NULL !! \n");
		return ERR_FAIL;
	}

	ucExportControlLen = get8bit(pucPv++);

	// conax 6.0 - uri spec
	if (ucExportControlLen > 0)
	{
		pucPv += ucExportControlLen;
		HxLOG_Print("** ucExportControlLen(0x%X) \n", ucExportControlLen);
	}

	ucExportConditionslLen = get8bit(pucPv++);
	if (ucExportConditionslLen > 0)
	{
		pucPv += ucExportConditionslLen;
		HxLOG_Print("** ucExportConditionslLen(0x%X) \n", ucExportConditionslLen);
	}

	ucUriLen = get8bit(pucPv++);
	pstDrmInfo = &pstInstance->stDrmInfo;

	CX_memset(pstDrmInfo, 0, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

	if ((ucUriLen == 0) || (pstInstance->bFromProtectedData == FALSE))
	{
		// no URI, set default URI
		HxLOG_Warning("UriLen (%d) FromProtectedData (%d), default URI !!\n", ucUriLen, pstInstance->bFromProtectedData);

		if (CXSC_GetDefaultUri(pstDrmInfo) != ERR_OK)
		{
			HxLOG_Error("CXSC_GetDefaultUri() error \n");
			return ERR_FAIL;
		}
	}
	else
	{
		// URI exist~
		usUri = get16bit(pucPv);

		pstDrmInfo->eApsCopyCtrl = (usUri >> CX_APS_BIT_SHIFT) & CX_2BIT_MASK;
		pstDrmInfo->eEmiCopyCtrl = (usUri >> CX_EMI_BIT_SHIFT) & CX_2BIT_MASK;
		pstDrmInfo->eICT = (usUri >> CX_ICT_BIT_SHIFT) & CX_1BIT_MASK;
		pstDrmInfo->eRedistributionCtrl = (usUri >> CX_REDIST_BIT_SHIFT) & CX_1BIT_MASK;
		pstDrmInfo->eRetentionLimit = (usUri >> CX_RETENT_BIT_SHIFT) & CX_3BIT_MASK;
		pstDrmInfo->eTrickPlayCtrl = (usUri >> CX_TRICK_BIT_SHIFT) & CX_3BIT_MASK;
		pstDrmInfo->eMR = (usUri >> CX_MR_BIT_SHIFT) & CX_3BIT_MASK;
		pstDrmInfo->eDisableAnalOutput = (usUri >> CX_DISABLE_ANALOG_BIT_SHIFT) & CX_1BIT_MASK;

		if (pstDrmInfo->eRetentionLimit > 0x07)
		{
			// max 0b111 이므로 여기서는 0~7 사이의 값이어야 한다
			HxLOG_Error("invalid eRetentionLimit(0x%X) \n", pstDrmInfo->eRetentionLimit);
			return ERR_FAIL;
		}

		switch (pstDrmInfo->eEmiCopyCtrl)
		{
		case eDxCopyrightControl_ConaxEmiCopyCtrl_Freely:
			pstDrmInfo->eRetentionLimit = eDxCopyrightControl_ConaxRetentionLimit_CopyFree_NoLimit;
			break;

		case eDxCopyrightControl_ConaxEmiCopyCtrl_Never:
			/* fall through */
		case eDxCopyrightControl_ConaxEmiCopyCtrl_NoMore:
			pstDrmInfo->eRetentionLimit = eDxCopyrightControl_ConaxRetentionLimit_NoLimit + pstDrmInfo->eRetentionLimit;
			break;

		case eDxCopyrightControl_ConaxEmiCopyCtrl_Once:
			pstDrmInfo->eRetentionLimit = eDxCopyrightControl_ConaxRetentionLimit_NoExpLimit + pstDrmInfo->eRetentionLimit;
			break;

		default:
			HxLOG_Error("invalid eEmiCopyCtrl(0x%X) \n", pstDrmInfo->eEmiCopyCtrl);
			return ERR_FAIL;
		}
	}

	LEAVE_FUNCTION;

	return ERR_OK;
}
#endif	// #if defined(CONFIG_MW_CAS_CONAX_URI)
#endif	// #if defined(CONFIG_MW_CAS_CONAX_CPM)

STATIC HERROR local_cxsc_ParseHostData(Handle_t hEcm, CxScInstance_t* pstInstance, HUINT32 ulLen, HUINT8* pucData, HUINT32 ulIndex)
{
	HUINT8* pucApdu, * pucPV;
	HUINT8	ucPI, ucPL;
	HUINT32	ulIdx;

	ENTER_FUNCTION;

	// pucData가 host data임..
	pucApdu = pucData;
	ulIdx = 0;

	pstInstance->stFingerprint.usDuration = 0;
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	pstInstance->bFPFromProtectedData = FALSE;
#endif
	while (ulIdx < ulLen)
	{
		if (*pucApdu == eCxHostData_Padding)
		{
			pucApdu++;
			ulIdx++;

			continue;
		}
		ucPI = *pucApdu;
		ucPL = *(pucApdu + 1);
		pucPV = pucApdu + 2;

		switch (ucPI)
		{
		case eCxHostData_UserText:
			// APDU_ID, APDU_L은 떼어내고 준다
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
			local_cxsc_ParseUserText(pstInstance, ucPL, pucPV);
#endif
			break;

		case eCxHostData_Fingerprint:
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT) && defined(CONFIG_MW_CAS_CONAX_URI)
			if (pstInstance->bFromProtectedData && ((ulIndex >= pstInstance->ulProtectedDataStartOffset) && (ulIndex < pstInstance->ulProtectedDataEndOffset)))
			{
				pstInstance->bFPFromProtectedData = TRUE;
			}
#endif
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
			local_cxsc_ParseFingerprint(pstInstance, ucPL, pucPV, eCxHostData_Fingerprint, hEcm);
#endif
			break;
		case eCxHostData_FingerprintId:
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
			local_cxsc_ParseFingerprint(pstInstance, ucPL, pucPV, eCxHostData_FingerprintId, hEcm);
#endif
			break;

		case eCxHostData_ChipsetPairing:
#if defined(CONFIG_MW_CAS_CONAX_CPM)
			(void)local_cxsc_ParseChipsetPairing(pstInstance, pucPV, ucPL);
#endif
			break;

		case eCxHostData_TokensReport:
			// 설명이 부족해요.. 처리안함..
			break;

		case eCxHostData_BridgeControl:
#if defined(CONFIG_MW_CAS_CONAX_URI)
			if (pstInstance->bFromProtectedData && ((ulIndex >= pstInstance->ulProtectedDataStartOffset) && (ulIndex < pstInstance->ulProtectedDataEndOffset)))
			{
				pstInstance->bBridgeControl = TRUE;
				(void)local_cxsc_ParseBridgeControl(pstInstance, pucPV, ucPL, hEcm);
			}
#endif
			break;

		default:
			// 새로 추가되는 apdu parameter의 format이 어떨지 예상할 수 없으므로 이 뒤로 나오는 데이터는 전부 해석을 포기한다.
			HxLOG_Error("Unknown HostData APDU PI (%x)\n", *pucApdu);
			return ERR_OK;
		}
		ulIdx += ucPL + 2;
		pucApdu += ucPL + 2;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}
#define __ProtectedData____________________________________________________________

#if defined(CONFIG_MW_CAS_CONAX_CPM) && defined(CONFIG_MW_CAS_CONAX_CPM_WITH_LPK)
/*
	parameter check 부분에서 error이 발생할 때 아무것도 출력해 주지 않는다... 먼저 keystore를 체크하도록 하자...
	local_cxsc_ParseResponse()에서 error code 값에 따라서 ui에 출력을 해 줌...
	ERR_FAIL일 경우 keystore 검사가 끝났기에  무조건 error code 11로 처리함.
*/
STATIC HERROR local_cxsc_GetProtectedPlaneText(CxScInstance_t* pstInstance, const HUINT8* pucData, CxParam_t* pPlaneText)
{
	HERROR		err				= ERR_OK;
	HUINT8		ucAlgorithm;
	HUINT8		ucMode;
	HUINT8		ucPadding;
	HUINT16		usKeyId;
	HUINT32		ulIdx;
	HUINT32		ulLen;
	CxCpmKey_t	key;
	HUINT8		ucIvLen;
	HUINT8		aucIv[256]		=
	{
		0,
	};

	HUINT8		ucTextLen;
	HUINT8		aucCipherText[256];
	//	HUINT8	aucCipherTextHash[6];
	HUINT8		aucHashValue[SHA_1_DIGEST_SIZE];
	HUINT8		aucPlainText[256];
	//	HUINT8	ucCipherTextLen;

	stCrypt_t	CxLpkData;

	if (pucData == NULL || *pucData != eCxSc_ProtectedData)
	{
		HxLOG_Print("pucData NULL or not protected data...\n");
		return ERR_FAIL;
	}

#ifdef DUMP_PROTECTED_AND_PLAIN_TEXT
	CX_MemDump("Protected DATA", pucData, *(pucData + 1) + 2);
#endif

	ulIdx = 0;
	CX_memset(&CxLpkData, 0, sizeof(CxLpkData));
	CX_memset(aucPlainText, 0, sizeof(aucPlainText));

	pucData++;

	ulLen = (HUINT32)*pucData++;
	if (1 >= ulLen)
	{
		return ERR_FAIL;
	}

	ucAlgorithm = (*pucData >> 5) & 0x07;
	ucMode  	= (*pucData >> 3) & 0x03;
	ucPadding   = (*pucData >> 2) & 0x01;
	pucData++;
	ulIdx++;

	/** [Chipset Pairing STB Intergration Kit 5.0-A 22p.]
	 * 3.4 Compare the key_id signalled in the PROTECTED_DATA parameter to
	 *     the key_id of the LPK currently loaded in the DVB chipset.
	 *     if they are identical and the key is valid, jump to step 3.8
	 *     3.8 -> Decrypt the received data using chipset decryption routines.
				  The data is encrypted in CBC mode using techniques indicated in
				  the PROTECTED_DATA mode_ind and padding_ind elements.
	*/

	usKeyId	= get16bit(pucData);
	if(usKeyId == CX_CPM_INVALID_KEYID)
	{
		return eCxECode_10;
	}

	 /* 3.5 Retrieve the key with the specified key_id from the keystore.
	 *     Reject the response if the key is not found.
	 */
	pucData += 2;
	ulIdx += 2;

	err = CXCPM_GetKey(usKeyId, &key);
	if (err != ERR_OK)
	{
		if (CXCPM_CountKey(eCxCpm_LPK) == 0)
			return eCxECode_12;
		else
			return eCxECode_10;
	}

	// 3.6 Reject the response if the key with the specified key_id is found, but is not an LPK.
	if (key.ucType != (HUINT8)eCxCpm_LPK)
	{
		return eCxECode_13;
	}

	/** [Chipset Pairing STB Intergration Kit 5.0-A 22p.]
	 * 3.3 Reject the response if the algorithm_id or mode_id signalled in PROTECTED_DATA
	 *     is not supported by the STB.
	 */
	if (ucAlgorithm > eCxSc_ProtectedData_AlgorithmID_3DES)
	{
		return ERR_FAIL;
	}

	if ( (ucMode != eCxSc_ProtectedData_ModeInd_CBC_WO_IV)
		&& (ucMode != eCxSc_ProtectedData_ModeInd_CBC_WITH_IV))
	{
		return ERR_FAIL;
	}

	if(ucMode == eCxSc_ProtectedData_ModeInd_CBC_WITH_IV)
	{
		int i;

		ucIvLen = *pucData;
		pucData++;
		ulIdx++;

		for(i = 0 ; i < ucIvLen ; i++)
		{
			aucIv[i] = *pucData;
			pucData++;
			ulIdx++;
		}
	}

	/*
	// DONE: [Decryption]
	// DONE: Set LPK (is encrypted in ECB) : key(CxCpmKey_t)
	// DONE: mode에 따라 init vector를 사용할 수도/않을 수도 있음
	// DONE: cyphertext_byte를 decrypt 한 후, hash를 뺀 나머지 부분을 pPlaneText(CxParam_t).
	// DONE: pPlaneText->ulValue = protected_planetext() - hash;
	// DONE: Cx_memcpy(pPlaneText->aucData, protected_planetext(), pPlaneText->ulValue);
	*/

	// cipher text만 따로 복사하여 수행!
	ucTextLen = ulLen - ulIdx;
	CX_memcpy(aucCipherText, pucData, ucTextLen);

	// DI에 넘어가야 하는 Parameter
	if(ucAlgorithm == 0)
	{
		CxLpkData.etCryptMode = DI_CRYPT_TDES;
	}
	CxLpkData.etCryptKey = DI_CRYPT_INTERNAL_M2M_LPK_KEY;
	CxLpkData.etCryptData = DI_CRYPT_BLOCK;
	CxLpkData.pKey = key.aucValue;  		// M2M(LPK)
	CxLpkData.ulKeySize = (HUINT32)key.ucLength;
	CxLpkData.pSrcBuffer = aucCipherText;   // ciphertext
	CxLpkData.pDstBuffer = aucPlainText;
	CxLpkData.pIvKey = aucIv;   		// Iv key == mode ??

	CxLpkData.ulBuffSize = ucTextLen;

	err = DI_CRYPT_Decrypt(&CxLpkData);
	if(err != ERR_OK)
	{
		HxLOG_Error("Decryption is not complete!\n");
		return eCxECode_11;
	}

	/*
	 * aucPlainText 는 DI 에서 M2M(LPK) 를 이용해서,
	 * LPK 데이터를 decryption 한 data -> CWPK(CW) 이다.

	 * PainText에 포함된 HASH값(6bytes)는 HASH 수행하지 않고...
	 * PlainText에 포함된 HASH값과 SHA1 수행후 뒤에 6바이트를 비교하자.
	 */

	svc_cas_DevPortChiperSha1(aucPlainText, ucTextLen - 6, aucHashValue);

#ifdef DUMP_PROTECTED_AND_PLAIN_TEXT
	CX_MemDump("Plain DATA", aucPlainText, ucTextLen);
	CX_MemDump("Plain HASH", aucHashValue, SHA_1_DIGEST_SIZE);
#endif

	if(CX_memcmp(aucPlainText + (ucTextLen - 6), aucHashValue + 14, 6) != 0)
	{
		HxLOG_Print("different hash value.\n");
		return eCxECode_11;
	}

	CX_memcpy(pPlaneText->aucData, aucPlainText, ucTextLen - 6);
	pPlaneText->ulValue = ucTextLen - 6;

	NOT_USED_PARAM(ucPadding);

	LEAVE_FUNCTION;

	return ERR_OK;
}
#endif

#define ______________________________________________________________

HERROR CXSC_InitCore(void)
{
	register int	i;
	HINT32			nResult			= 0;
	//## s/c에 callback 등록하고.. 필요하면 뭐 init 다 해주고..
	// standby도 power on/off 처럼 smartcard reset해주란다.
	ENTER_FUNCTION;

	nResult = CX_SEM_Create("CxCScSem", &s_ulCxScSemId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	for (i = 0; i < CX_SC_NUMBER_OF_INSTANCE; i++)
	{
		s_pstCxScInstance[i] = CX_MEM_Alloc(sizeof(CxScInstance_t));
		if (s_pstCxScInstance[i] == NULL)
		{
			return ERR_FAIL;
		}

		// clear instance
		CX_memset(s_pstCxScInstance[i], 0x0, sizeof(CxScInstance_t));
		s_pstCxScInstance[i]->eScState = eCxSc_StateNone;
		s_pstCxScInstance[i]->ucScSlot = i;
#if defined(CONFIG_MW_CAS_CONAX_URI)
		s_pstCxScInstance[i]->bFromProtectedData = FALSE;
#endif
	}

	CXSC_Init(local_cxsc_NotifyCb);

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_Ready(CxScInstance_t* pstInstance, HBOOL* pbReset)
{
	HERROR	hError	= ERR_FAIL;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	// reset 중에 카드가 빠질 수도 있으니 확인하고..
	if (pstInstance->eScState != eCxSc_StateNone)
	{
		if (pstInstance->eScState != eCxSc_StateCassVerReceived)
		{
			//CassVer를 못받았다.. conax card가 아닐거 같음
			pstInstance->eScState = eCxSc_StateInvalid;
			*pbReset = TRUE;
			hError = CX_ERR_SC_INCORRECT_CARD;
		}
		else if (pstInstance->ucCassVer != CX_SC_INTERFACE_VERSION)
		{
			// CassVer이 다르다..
			pstInstance->eScState = eCxSc_StateDifferentVer;
			*pbReset = TRUE;
			hError = CX_ERR_SC_INCORRECT_CARD;
		}
		else if (pstInstance->usCaSysId < 0x0B00 || pstInstance->usCaSysId > 0x0BFF)
		{
			// conax ca sys id가 아니다..
			pstInstance->eScState = eCxSc_StateInvalid;

			// Basic STB Integration Kit 6.0 (page 16)[4.5:CA_SYS_ID in range 0x0B00 to 0x0BFF --> reset & retry] pn_jdkim
			*pbReset = TRUE;
			hError = CX_ERR_SC_INCORRECT_CARD;
		}
		else if (pstInstance->ucSessionCount > 0)
		{
			// CassVer 받고, CaSysId도 받고, session 개수도 얻어오면 card 준비 된 것으로 봅시다
			hError = ERR_OK;

			// 카드가 준비 된 것을 알려줍니다.
			//CXCORE_SendMsg(eCxMsg_Sc_Ready, HANDLE_NULL, pstInstance->ucScSlot, (HUINT32)0);
		}
		else
		{
			// usCaSysId나 ucSessionCount가 안들어오는 경우가 생긴다면 어떻게 할까??? 둘중 하나를 해야할 것.
			// 1. default 값을 써준다.
			// 	pstInstance->usCaSysId = 0xB00;
			// 	pstInstance->ucSessionCount = 1;
			// 2. error 처리해버린다. invalid? comm error?
			pstInstance->eScState = eCxSc_StateCommError;
		}

		// ui에 알려줌
		//CXUI_ScStateChanged((HINT32)pstInstance->ucScSlot, pstInstance->eScState);
	}

	LEAVE_FUNCTION;

	return hError;
}

STATIC HERROR local_cxsc_cbResetTimer(unsigned long ulTimerId, void* pvParam)
{
	HINT32			nScSlot;
	CxMsgType_t		eMsgType;
	CxScInstance_t* pstInstance;

	HxLOG_Print("[Enter]\n");

	nScSlot = *((HINT32 *)pvParam);

	//CX_SEM_Get(s_ulCxScSemId);

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		HxLOG_Error("Invalid ScSclot (%d)\n", nScSlot);
		return ERR_FAIL;
	}

	pstInstance = s_pstCxScInstance[nScSlot];

	if (ulTimerId != pstInstance->ulTimerId)
	{
		//CX_SEM_Release(s_ulCxScSemId);
		HxLOG_Error("timer id mismatch\n");
		return ERR_FAIL;
	}

	pstInstance->ulTimerId = 0;

	if(pstInstance->bRecoveryReset)
		eMsgType = eCxMsg_Sc_RecoveryReset;
	else
		eMsgType = eCxMsg_Sc_Reset;

	CXCORE_SendMsg(eMsgType, HANDLE_NULL, nScSlot, (HUINT32)0);

	//CX_SEM_Release(s_ulCxScSemId);

	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}

STATIC HERROR local_cxsc_ScInserted(HINT32 nScSlot)
{
	CxScInstance_t* pstInstance;

	HxLOG_Error("[Enter(%d)]\r\n", nScSlot);

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);

	pstInstance = s_pstCxScInstance[nScSlot];

	if(pstInstance->eScState != eCxSc_StateNone)
	{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
		// insert 상태에서 또 insert가 오면 무슨 일이 생길지 모른다... extract message로 session을 정리하도록 하자.
		CXCORE_SendMsg(eCxMsg_Sc_Extracted, HANDLE_NULL, nScSlot, (HUINT32)0);
#else
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_FAIL;
#endif
	}

	pstInstance->eScState = eCxSc_StateInserted;
	CXUI_ScStateChanged(nScSlot, pstInstance->eScState, FALSE);

	//## reset 하고 필요하면 UI에도 통보해주고..
	CXCORE_SendMsg(eCxMsg_Sc_Reset, HANDLE_NULL, nScSlot, (HUINT32)0);

	CX_SEM_Release(s_ulCxScSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ScExtracted(HINT32 nScSlot)
{
	CxScInstance_t* pstInstance;

	HxLOG_Error("[Enter(%d)]\r\n", nScSlot);

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);

	pstInstance = s_pstCxScInstance[nScSlot];

	pstInstance->ucRetryCount = 0;

	if (pstInstance->ulTimerId != 0)
	{
		VK_TIMER_Cancel(pstInstance->ulTimerId);
		pstInstance->ulTimerId = 0;
	}

	// extract 상태인데 또 extract가 오면 무시해버리자.. 어디서 뭐가 꼬일지도 몰라
	if (pstInstance->eScState == eCxSc_StateNone)
	{
		// 부팅시 SC가 없을 경우 init state가 none이기에 UI만이라도 표시하도록 해야 함.
		CXUI_ScStateChanged(nScSlot, pstInstance->eScState, FALSE);
		CX_SEM_Release(s_ulCxScSemId);
		return ERR_OK;
	}

	pstInstance->eScState = eCxSc_StateNone;
	CXUI_ScStateChanged(nScSlot, pstInstance->eScState, FALSE);

	/* BCM 6.5에서는 사용하지 않음. --> deactivate 안하면 인증받을 때는 문제가 될수도 있으니 확인 꼭 해보세요!! */
	CXSC_Deactivate(nScSlot);

	//## deactivate 하고 다른곳에도 통보해주고
	CXCORE_SendMsg(eCxMsg_Sc_Extracted, HANDLE_NULL, nScSlot, (HUINT32)0);

	//## 필요하다면 instance 초기화 해주고...
	local_cxsc_DeleteStatusList(pstInstance);
	local_cxsc_DeleteParam(pstInstance);

	CX_memset(pstInstance, 0x0, sizeof(CxScInstance_t));
	pstInstance->ucScSlot = (HUINT8)nScSlot;

	CX_SEM_Release(s_ulCxScSemId);

	// casMgr에게 알려주기위해서리..
	CXSC_ValidCard(nScSlot, FALSE, 0, 0);

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_Reset(HINT32 nScSlot, HBOOL bRecoveryReset)
{
	CxScInstance_t* pstInstance;
	HUINT8			ucProtocol;
	HUINT32			ulAtrLen		= 0;
	HUINT8			aucAtr[33]		= {0, };
	int				nErr, i;
	HBOOL			bReset			= FALSE;

	ENTER_FUNCTION;

	pstInstance = s_pstCxScInstance[nScSlot];

	if (pstInstance->eScState == eCxSc_StateNone)
	{
		HxLOG_Error("smartcard state none!!!");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pstInstance->bRecoveryReset = bRecoveryReset;

	//## reset s/c
	nErr = CXSC_Reset(nScSlot, aucAtr, &ulAtrLen, &ucProtocol);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("CXSC_Reset() fail...\n");
		goto _retry;
	}

	pstInstance->eScState = eCxSc_StateReset;

	nErr = local_cxsc_InitCass(pstInstance);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("local_cxsc_InitCass() fail...\n");
		goto _retry;
	}

	nErr = local_cxsc_Ready(pstInstance, &bReset);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("local_cxsc_Ready(%d) fail...\n", bReset);
		if (bReset == TRUE)
		{
			goto _retry;
		}
		else
		{
			goto _fail;
		}
	}

	nErr = local_cxsc_PairingMech(pstInstance);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("local_cxsc_PairingMech() fail...\n");
		goto _fail;
	}

	if (bRecoveryReset != TRUE)
	{
		for (i = 0; i < pstInstance->ucSessionCount; i++)
		{
			nErr = local_cxsc_ResetSession(pstInstance, i);

			// 에러 났으면 더 할 필요 없자나?
			if (nErr != ERR_OK)
			{
				HxLOG_Error("local_cxsc_ResetSession() (%d) fail...\n", i);
				goto _fail;
			}
		}
	}

	pstInstance->ucRetryCount = 0;
	pstInstance->eScState = eCxSc_StateReady;
	CXCORE_SendMsg(eCxMsg_Sc_Ready, HANDLE_NULL, pstInstance->ucScSlot, (HUINT32)0);

	CXSC_ValidCard(nScSlot, TRUE, pstInstance->usCaSysId, pstInstance->ucSessionCount);
	CXUI_ScStateChanged(nScSlot, eCxSc_StateReady, TRUE);

	LEAVE_FUNCTION;
	return ERR_OK;

_retry:
	if (pstInstance->ucRetryCount < CX_SC_MAX_RETRY_RESET)
	{
		VK_TIMER_EventAfter(1500, (void*)local_cxsc_cbResetTimer, (void*)&nScSlot, sizeof(HINT32), (HULONG *)&pstInstance->ulTimerId);
		pstInstance->ucRetryCount++;
	}
	else
	{
		pstInstance->ucRetryCount = 0;
	}

_fail:
	if (CXSC_Deactivate(pstInstance->ucScSlot) != ERR_OK)
	{
		HxLOG_Error("CXSC_Deactivate() fail...\n");
	}

	if (nErr == CX_ERR_SC_INCORRECT_CARD)
	{
		if (pstInstance->eScState != eCxSc_StateDifferentVer)
		{
			pstInstance->eScState = eCxSc_StateInvalid;
		}
	}
	else
	{
		pstInstance->eScState = eCxSc_StateCommError;
	}

	CXUI_ScStateChanged(nScSlot, pstInstance->eScState, FALSE);
	CXSC_ValidCard(nScSlot, FALSE, 0, 0);

	LEAVE_FUNCTION;

	return ERR_FAIL;
}


HERROR CXSC_ColdReset(HINT32 nScSlot)
{
	ENTER_FUNCTION;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);

	if(local_cxsc_Reset(nScSlot, FALSE) != ERR_OK)
	{
		HxLOG_Error("local_cxsc_Reset(%d, FALSE) error\n", nScSlot);
	}

	CX_SEM_Release(s_ulCxScSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXSC_RecoveryReset(HINT32 nScSlot, HBOOL bUiDisplay)
{
	CxScInstance_t *pstInstance;

	ENTER_FUNCTION;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	// 통신 중 error 등이 발생했다.

	CX_SEM_Get(s_ulCxScSemId);

	pstInstance = s_pstCxScInstance[nScSlot];

	// 필요하면 왜 발생했는지 UI도 알려주고...
	if(bUiDisplay)
	{
		CXUI_ScStateChanged(nScSlot, pstInstance->eScState, TRUE);
	}

	// ECM & EMM Queue를 비운 후 filter를 reset 시키고...
	CXSESSION_ScErrorHandler(nScSlot);

	// reset도 하고
	if(local_cxsc_Reset(nScSlot, TRUE) != ERR_OK)
	{
		HxLOG_Error("local_cxsc_Reset(%d, TRUE) error\n", nScSlot);
	}

	//## ui에서 뭔가 interaction이 있던 와중이라면 어떻게 하는게 좋을까?

	CX_SEM_Release(s_ulCxScSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_Read(CxScInstance_t* pstInstance)
{
	HUINT8	aucCmd[5];
	HUINT32	ulToReadLen;
	HUINT32	ulReadLen;
	HUINT8	aucReadBuf[256+2]; // SW bytes
	HERROR	nRet;

	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	// read data
	aucCmd[0] = 0xDD;
	aucCmd[1] = (HUINT8)eCxSc_Read;
	aucCmd[2] = 0x00;
	aucCmd[3] = pstInstance->aucWriteBuf[3];	// session index: write했던 session과 같은 것을 쓴다
	aucCmd[4] = pstInstance->ucSW2;


	// write data
	ulToReadLen = pstInstance->ucSW2 + 2;	// 읽어올 data + sw 2byte
	nRet = CXSC_Read(pstInstance->ucScSlot, aucCmd, aucReadBuf, &ulToReadLen);

	if (nRet == ERR_OK)
	{
		ulReadLen = pstInstance->usReadBufLen + ulToReadLen - 2;
		if (ulReadLen > CX_SC_READ_BUFFER_SIZE)
		{
			//## 4096 byte 넘었으니 에러다!!! 읽어서 버퍼에 두는데 4K 넘으면 error! -> reset
			pstInstance->eScState = eCxSc_StateCommError;
			CXCORE_SendMsg(eCxMsg_Sc_RecoveryReset, HANDLE_NULL, (HUINT32)pstInstance->ucScSlot, (HUINT32)0);
			return ERR_FAIL;
		}

		if (pstInstance->ucSW2 != (HUINT8)(ulToReadLen - 2))
		{
			/* if not match the expected number of bytes (P3/LNG in the READ command), reject the smart card output and reset the card.*/
			pstInstance->eScState = eCxSc_StateCommError;
			CXCORE_SendMsg(eCxMsg_Sc_RecoveryReset, HANDLE_NULL, (HUINT32)pstInstance->ucScSlot, (HUINT32)0);
			return ERR_FAIL;
		}

		pstInstance->ucSW1 = aucReadBuf[ulToReadLen - 2];
		pstInstance->ucSW2 = aucReadBuf[ulToReadLen - 1];

		CX_memcpy(pstInstance->aucReadBuf + pstInstance->usReadBufLen, aucReadBuf, ulToReadLen - 2);
		pstInstance->usReadBufLen += (HUINT16)ulToReadLen - 2;
	}
	else
	{
		if (pstInstance->eScState != eCxSc_StateReady)
		{
			CXUI_ScStateChanged(pstInstance->ucScSlot, eCxSc_StateCommError, FALSE);
		}
		pstInstance->eScState = eCxSc_StateCommError;
		CXCORE_SendMsg(eCxMsg_Sc_RecoveryReset, HANDLE_NULL, (HUINT32)pstInstance->ucScSlot, (HUINT32)0);
	}

	LEAVE_FUNCTION;
	return nRet;
}

STATIC HERROR local_cxsc_Write(CxScInstance_t* pstInstance)
{
	HERROR	nRet;
	HUINT32	ulReadLen;

	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	// write data
	ulReadLen = 2;	// sw 2byte
	nRet = CXSC_Write(pstInstance->ucScSlot, pstInstance->aucWriteBuf, pstInstance->usWriteBufLen, pstInstance->aucReadBuf, &ulReadLen);
	pstInstance->usReadBufLen = (HUINT16)ulReadLen;

	if (nRet == ERR_OK)
	{
		pstInstance->ucSW1 = pstInstance->aucReadBuf[pstInstance->usReadBufLen - 2];
		pstInstance->ucSW2 = pstInstance->aucReadBuf[pstInstance->usReadBufLen - 1];
	}
	else
	{
		if (pstInstance->eScState != eCxSc_StateReady)
		{
			CXUI_ScStateChanged(pstInstance->ucScSlot, eCxSc_StateCommError, FALSE);
		}
		pstInstance->eScState = eCxSc_StateCommError;
		CXCORE_SendMsg(eCxMsg_Sc_RecoveryReset, HANDLE_NULL, (HUINT32)pstInstance->ucScSlot, (HUINT32)0);
	}

	LEAVE_FUNCTION;
	return nRet;
}

//////////////////////////////////////////////////////////////////////////////////

STATIC HERROR local_cxsc_IsValidResponse(CxSc_InputParam_t eInParam, CxSc_ParamTable_t* pstOutParam)
{
	int	i;

	for (i = 0; i < CX_SC_MAX_VALID_INPUT_PARAM; i++)
	{
		if (pstOutParam->ucValidInParam[i])
		{
			if (eInParam == pstOutParam->ucValidInParam[i])
			{
				// input param is matched
				return ERR_OK;
			}
		}
		else
		{
			// compairing is completed but input param is not matched - ignore this parameter
			break;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR local_cxsc_IsValidLength(HUINT8* pucParam, HUINT8 ucParamLen, CxSc_ParamTable_t* pstOutParam)
{
	if (pstOutParam->ucPL != 0)
	{
		if (ucParamLen != pstOutParam->ucPL)
		{
			return ERR_FAIL;
		}
	}
	else
	{
		if (pstOutParam->ucPI == eCxSc_HostData)
		{
			CxParam_t	stParam;
			HUINT32		unIdx	= 0;

			stParam.ulValue = (HUINT32)get8bit(pucParam + 1);
			CX_memcpy(stParam.aucData, pucParam + 2, stParam.ulValue);

			while (unIdx < stParam.ulValue)
			{
				if (stParam.aucData[unIdx] == eCxHostData_Padding)
				{
					unIdx++;
					continue;
				}

				switch (stParam.aucData[unIdx])
				{
				case eCxHostData_Fingerprint:
					if (stParam.aucData[unIdx + 1] != 10)
					{
						HxLOG_Error("invalid length... HostData - Fingerprint\n");
						return ERR_FAIL;
					}
					break;
				}

				unIdx += (stParam.aucData[unIdx + 1] + 2);
			}
		}
		else
		{
			HUINT8	ucMinLen;

			switch (pstOutParam->ucPI)
			{
			case eCxSc_CaDescEmm:
				ucMinLen = 6;		break;
			case eCxSc_Cw:
				ucMinLen = 5;		break;
			case eCxSc_RetChanConfig:
				ucMinLen = 2;		break;
			case eCxSc_AccessStatus:
				ucMinLen = 2;		break;
			case eCxSc_SubscriptionStatus:
				ucMinLen = 30;		break;
			case eCxSc_OrderInfo:
				ucMinLen = 6;		break;
			case eCxSc_AcceptViewingInfo:
				ucMinLen = 2;		break;
			case eCxSc_EventStatus:
				ucMinLen = 16;		break;
			case eCxSc_TokenDebitInfo:
				ucMinLen = 9;		break;
			case eCxSc_PurseStatus:
				ucMinLen = 6;		break;
			case eCxSc_DebitStatus:
				ucMinLen = 12;		break;
			case eCxSc_CreditStatus:
				ucMinLen = 4;		break;
			case eCxSc_ProtectedData:
				ucMinLen = 3;		break;
			default:
				ucMinLen = 1;		break;
			}

			if (ucMinLen > ucParamLen)
			{
				return ERR_FAIL;
			}
		}
	}

	return ERR_OK;
}

STATIC void local_cxsc_DeleteParam(CxScInstance_t* pstInstance)
{
	ENTER_FUNCTION;

	pstInstance->stScParamList.ulCount = 0;

	LEAVE_FUNCTION;
}

STATIC HERROR local_cxsc_GetParam(HUINT8* pucParam, CxParam_t* pstParam)
{
	ENTER_FUNCTION;

	pstParam->eType = (CxSc_OutputParam_t)*pucParam;

	switch (*pucParam)
	{
	case eCxSc_CassVer:
	case eCxSc_SessionInfo:
	case eCxSc_MatRatLevel:
		pstParam->ulValue = (HUINT32)get8bit(pucParam + 2);
		break;

	case eCxSc_CaSysId:
	case eCxSc_CountryIndicator:
	case eCxSc_CryptoBlockSize:
		pstParam->ulValue = (HUINT32)get16bit(pucParam + 2);
		break;

	case eCxSc_CardNumber:
	case eCxSc_SequenceNumber:
		pstParam->ulValue = (HUINT32)get32bit(pucParam + 2);
		break;

	case eCxSc_CaDescEmm:
	case eCxSc_HostData:
	case eCxSc_ProtectedData:
		// CPM
	case eCxSc_AccessStatus:
	case eCxSc_Cw:
	case eCxSc_OrderInfo:
	case eCxSc_AcceptViewingInfo:
	case eCxSc_TokenDebitInfo:
	case eCxSc_SubscriptionStatus:
	case eCxSc_EventStatus:
	case eCxSc_PurseStatus:
	case eCxSc_DebitStatus:
	case eCxSc_CreditStatus:
	case eCxSc_PinStatus:
	case eCxSc_OrderReqApdu:
	case eCxSc_RetChanConfig:
	case eCxSc_DebitLogData:
	case eCxSc_SignatureResp:
	case eCxSc_EncryptedMsg:
	case eCxSc_DecryptedMsg:
	case eCxSc_RandomOctets:
		pstParam->ulValue = (HUINT32)get8bit(pucParam + 1);

		CX_memcpy(pstParam->aucData, pucParam + 2, pstParam->ulValue);
		break;

	default:
		//## unknown parameter - ignore
		return ERR_FAIL;
	}

	HxLOG_Print("[PARAM:%x,%x]", *pucParam, pstParam->ulValue);

	LEAVE_FUNCTION;
	return ERR_OK;
}


STATIC HERROR local_cxsc_GetParamList(CxScInstance_t* pstInstance)
{
	//## 각 paramter를 structure로 만들어준다.
	// 아래의 경우에는 response를 무시한다.
	// reject the smartcard response if
	//	1. a PL value points beyond the total length of the smart card response
	//	2. the last PL is missing
	//	3. a PL value is less than what it is allowed to be according to specifications
	//	4. any parameter or data element has illegal value accroding to specifications

	HUINT16				usIdx, usParamLen;
	HUINT8* 			pucParam;

	HUINT8* 			pucResp;
	HUINT16				usLen;
	CxSc_InputParam_t	eInParam;
	int					nParamIdx;

	ENTER_FUNCTION;

	usIdx = 0;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	pucResp = pstInstance->aucReadBuf;
	usLen = pstInstance->usReadBufLen;
	eInParam = CxSc_GetInParam(pstInstance);

	while (usIdx < usLen)
	{
		pucParam = pucResp + usIdx;

		if (usIdx + 1 >= usLen)
		{
			//## case 2 - reject : the last PL is missing
			local_cxsc_DeleteParam(pstInstance);
			return ERR_FAIL;
		}

		usParamLen = *(pucParam + 1);

		nParamIdx = 0;

#if defined(CONFIG_DEBUG)
		HxLOG_Print("[PI:%s, PL:%02X, idx=%d, len=%d]\r\n", local_cxsc_DEBUG_GetPLStr(*pucParam), usParamLen, usIdx, usLen);
#endif

		while (s_stCxParamTable[nParamIdx].ucPI)
		{
			if (s_stCxParamTable[nParamIdx].ucPI == *pucParam)
			{
/*
				// PT 이슈로 인해 length 체크는 아래 local_cxsc_IsValidLength() 에서 한다.
				if (s_stCxParamTable[nParamIdx].ucPL > usParamLen)
				{
					//## case 3 - reject : a PL value is less than what it is allowed to be according to specifications
					local_cxsc_DeleteParam(pstInstance);
					return ERR_FAIL;
				}
*/
				if (usIdx + usParamLen + 2 > usLen)
				{
					//## case 1 - reject : a PL value points beyond the total length of the smart card response
					local_cxsc_DeleteParam(pstInstance);
					return ERR_FAIL;
				}

				if (local_cxsc_IsValidResponse(eInParam, &s_stCxParamTable[nParamIdx]) == ERR_OK)
				{
					if (local_cxsc_IsValidLength(pucParam, usParamLen, &s_stCxParamTable[nParamIdx]) != ERR_OK)
					{
						//## case 3 - reject : a PL value is less than what it is allowed to be according to specifications
						local_cxsc_DeleteParam(pstInstance);
						return ERR_FAIL;
					}

					// input parameter에 맞는 response인 경우만 처리  - 아니면 skip
					if (pstInstance->stScParamList.ulCount < CX_SC_MAX_PARAM)
					{
						if (local_cxsc_GetParam(pucParam, &pstInstance->stScParamList.stParam[pstInstance->stScParamList.ulCount]) == ERR_OK)
						{
							// 189358 , pn_jakim, 140702
							pstInstance->stScParamList.stParam[pstInstance->stScParamList.ulCount].ulIndex = usIdx;
							pstInstance->stScParamList.ulCount++;
						}
					}
				}
#if defined(CONFIG_DEBUG)
				else
				{
					if (s_stCxParamTable[nParamIdx].ucPI == eCxSc_ProtectedData)
					{
						// 이 프린트에 찍히는 InParam은 Param Table에... 넣어줘야 할 것 같음
						HxLOG_Error("\t>>>[CX:SC:CPM] PI: Protected Data, InParam:?? %d\n", eInParam);
					}
				}
#endif
				break;
			}
			nParamIdx++;
		}

		usIdx += usParamLen + 2;	// PI, PL, PV = 2 + PL
	}

	if (usIdx != usLen)
	{
		// total length mismatch...
		local_cxsc_DeleteParam(pstInstance);
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_CONAX_CPM) && defined(CONFIG_MW_CAS_CONAX_CPM_WITH_LPK)

/**
 * LPK 지원하는 모델만 이 함수 사용 (나머지에서는 의미 없음)
 *
 * Encrypt되어 있는 Protected Data를 Decrypt해서 Response Data를 재구성~!!
 */
STATIC HERROR local_cxsc_DecryptParam(CxScInstance_t* pstInstance)
{
	HERROR		err				= ERR_OK;
	HINT32		nRespLen, nOffset;
	HUINT8* 	pucResp;
	HUINT8* 	pucParam;
	HUINT8* 	pucProtParam;
	HINT32		nProtOffset		= 0, nRemain , nDiff ;
	HUINT8		ucPI, ucPL;
	HINT32		nNumHostData	= 0;
	HINT32		nNumProtData	= 0;
	CxParam_t	planeTxt;

	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	nOffset = 0;
	pucResp = pstInstance->aucReadBuf;
	nRespLen = (HINT32)pstInstance->usReadBufLen;

#if defined(CONFIG_MW_CAS_CONAX_URI)
	pstInstance->bFromProtectedData = FALSE;
#endif
	pstInstance->bBridgeControl = FALSE;
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	pstInstance->bFPFromProtectedData = FALSE;
#endif
	pucProtParam = NULL;

	while (nOffset < nRespLen)
	{
		pucParam = pucResp + nOffset;

		if (nOffset + 1 >= nRespLen)
		{
			return ERR_FAIL;
		}

		ucPI = *pucParam;
		ucPL = *(pucParam + 1);

		if (ucPI == (HUINT8)eCxSc_HostData)
		{
			nNumHostData++;
		}
		else if (ucPI == (HUINT8)eCxSc_ProtectedData)
		{
			nNumProtData++;
			pucProtParam = pucParam;
			nProtOffset = nOffset;
		}
		nOffset += (HINT32)ucPL + 2; // PI, PL, PV = 2 + PL
	}

	/** [Chipset Pairing STB Intergration Kit 5.0-A 22p.]
	 * 3.2 Reject the smart card response if it contains a plaintext HOST_DATA parameter
	 * and a PROTECTED_DATA parameter.
	 */
#if defined(CONFIG_MW_CAS_CONAX_URI)
	// conax 6.0 - delete 3.2
	if (nNumProtData > 1)
#else
	if ((nNumHostData > 0 && nNumProtData > 0) || nNumProtData > 1)
#endif
	{
		HxLOG_Error("plain HOST_DATA (%d), PROTECTED_DATA (%d)\n", nNumHostData, nNumProtData);
		return ERR_FAIL;
	}

	if (pucProtParam != NULL)
	{
		ucPL = (HUINT8)*(pucProtParam + 1);
		err = local_cxsc_GetProtectedPlaneText(pstInstance, pucProtParam, &planeTxt);
		if (err == ERR_OK)
		{
			/**
			 * 기존 Protected Data 영역에 Decrypt된 PlaneText를 복사하고,
			 * 그 뒤에 딸려 오는 Param들을 앞으로 땡겨준다~
			 */

			if (ucPL + 2 < (HUINT8)planeTxt.ulValue)
			{
				HxLOG_Critical("\n\n");
				return ERR_FAIL;
			}

			CX_memcpy(pucProtParam, planeTxt.aucData, planeTxt.ulValue);

			nDiff = (HINT32)(ucPL + 2) - (HINT32)planeTxt.ulValue;
			nRemain = nRespLen - nProtOffset - (HINT32)(ucPL + 2);
			pucParam = pucProtParam + planeTxt.ulValue;
			while (nRemain--)
			{
				*pucParam = *(pucParam + nDiff);
				pucParam++;
			}
			pstInstance->usReadBufLen = (HUINT16)(nRespLen - nDiff);

#if defined(CONFIG_MW_CAS_CONAX_URI)
			pstInstance->bFromProtectedData = TRUE;
#endif
			// conax 6.0 vt 7.3.4
			pstInstance->ulProtectedDataStartOffset = nProtOffset;
			pstInstance->ulProtectedDataEndOffset = nProtOffset + (HINT32)planeTxt.ulValue;
#if defined(CONFIG_MW_CAS_CONAX_URI)
			HxLOG_Info("** received Protected Data: %d , startOffset: %d , endOffset: %d\n", pstInstance->bFromProtectedData, pstInstance->ulProtectedDataStartOffset, pstInstance->ulProtectedDataEndOffset);
#else
			HxLOG_Info("** startOffset: %d , endOffset: %d\n", pstInstance->ulProtectedDataStartOffset, pstInstance->ulProtectedDataEndOffset);
#endif
		}
		else
		{
			if (CxSc_GetInParam(pstInstance) == eCxSc_Ecm)
			{
				if(err != ERR_FAIL)
				{
					CXUI_DisplayPairingError(pstInstance->hSession, err);
				}
			}
		}
	}

	LEAVE_FUNCTION;

	return err;
}
#endif

HERROR CXSC_SendEcmForQuerymode(CxScInstance_t* pstInstance, Handle_t hEcm)
{
	HUINT8* pucCmd;
	HERROR	nErr;

	if (pstInstance->eScState != eCxSc_StateReady)
	{
		return ERR_FAIL;
	}

	// paramList count 초기화.
	pstInstance->stScParamList.ulCount = 0;

	// parameter 만들기
	pucCmd = pstInstance->aucWriteBuf + 5;

	pucCmd++;
	pucCmd++;
	*pucCmd = 0x01;

	nErr = local_cxsc_ProcessCommand(pstInstance, hEcm);

	return nErr;
}

STATIC HERROR local_cxsc_ProcessParamQuery(CxScInstance_t* pstInstance, Handle_t hParam)
{
	CxParam_t* 		pstParam;
	CxAccessStatus_t* pstAccess;
	unsigned int	i;

	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		HxLOG_Error("Error! pstInstance NULL \r\n");
		return ERR_FAIL;
	}

	for (i = 0; i < pstInstance->stScParamList.ulCount; i++)
	{
		pstParam = &pstInstance->stScParamList.stParam[i];

		switch (pstParam->eType)
		{
		case eCxSc_AccessStatus:
			// QueryMode로 얻은 MR값을 ECM handle에 저장한다.
			pstAccess = (CxAccessStatus_t *)pstParam->aucData;
			if(pstAccess->ucStatus1 & 0x2)
			{
				//maturity lock
				CXECM_SetAccessStatus(hParam, pstAccess->ucExt1);
			}
			break;

		default:
			break;
		}
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}


STATIC HERROR local_cxsc_ProcessParamNormal(CxScInstance_t* pstInstance, Handle_t hParam)
{
	CxParam_t* 		pstParam;
	unsigned int	i;

	if (pstInstance == NULL)
	{
		HxLOG_Error("Error! pstInstance NULL \r\n");
		return ERR_FAIL;
	}
	// prevent #232362
	HxLOG_Print("[Enter](%d)\r\n", pstInstance->stScParamList.ulCount);

#if defined(CONFIG_DEBUG)
	pstInstance->hStbHandle = hParam;
#endif

	for (i = 0; i < pstInstance->stScParamList.ulCount; i++)
	{
		pstParam = &pstInstance->stScParamList.stParam[i];

		switch (pstParam->eType)
		{
		case eCxSc_CassVer:
			pstInstance->eScState = eCxSc_StateCassVerReceived;
			pstInstance->ucCassVer = (HUINT8)pstParam->ulValue;
			break;

		case eCxSc_SessionInfo:
			pstInstance->ucSessionCount = (HUINT8)pstParam->ulValue;
			break;

		case eCxSc_MatRatLevel:
			pstInstance->ucMatRatLevel = (HUINT8)pstParam->ulValue;
			break;

		case eCxSc_CaSysId:
			pstInstance->usCaSysId = (HUINT16)pstParam->ulValue;
			break;

		case eCxSc_CountryIndicator:
			pstInstance->usCountry = (HUINT16)pstParam->ulValue;
			break;

		case eCxSc_CaDescEmm:
			// emm filter 거는데 쓰도록 전달해야되요
			// 얻어온 정보는 잘 보내서 emm filter 걸도록 한다. (바로 할까? 아니면 mgr task로 던질까?)
			local_cxsc_ParseCaDescEmm((HINT32)hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_AccessStatus:
			// access status를 ui로 던진다
			local_cxsc_ParseAccessStatus(pstInstance, hParam, pstParam->aucData);
			break;

		case eCxSc_Cw:
			// CW를 전달한다
			local_cxsc_ParseCw(pstInstance, hParam, pstParam->aucData);
			break;

		case eCxSc_OrderInfo:
			local_cxsc_ParseOrderInfo(pstInstance, hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_AcceptViewingInfo:
			local_cxsc_ParseAcceptViewingInfo(pstInstance, hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_TokenDebitInfo:
			local_cxsc_ParseTokenDebitInfo(pstInstance, hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_SubscriptionStatus:
			local_cxsc_ParseSubscriptionStatus(pstInstance, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_EventStatus:
			local_cxsc_ParseEventStatus(pstInstance, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_PurseStatus:
			local_cxsc_ParsePurseStatus(pstInstance, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_DebitStatus:
			local_cxsc_ParseDebitStatus(pstInstance, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_CreditStatus:
			local_cxsc_ParseCreditStatus(pstInstance, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_PinStatus:
			local_cxsc_ParsePinStatus(hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_HostData:
			local_cxsc_ParseHostData(hParam, pstInstance, pstParam->ulValue, pstParam->aucData, pstParam->ulIndex);
			break;

		case eCxSc_ProtectedData:
			// 여기 오면 안됨!! local_cxsc_DecryptParam()에서 처리된 상태여야 함
			HxLOG_Critical("\n\n");
			break;

		case eCxSc_OrderReqApdu:
			// pstParam->aucData가 바로 return channel로 돌려보낼 APDU이니 요걸 넘겨서 처리하면 된다.
			local_cxsc_ParseOrderReqApdu(hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_RetChanConfig:
			// return channel 종류 마다 다르다... 나중에 필요하면 그때 작업
			local_cxsc_ParseRetChanConfig(hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_DebitLogData:
			// pstParam->aucData가 바로 return channel로 돌려보낼 Debit log data이니 요걸 넘겨서 처리하면 된다.
			local_cxsc_ParseDebitLogData(hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_SignatureResp:
			local_cxsc_ParseSignatureResp(hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_EncryptedMsg:
			local_cxsc_ParseEncryptedMsg(hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_DecryptedMsg:
			local_cxsc_ParseDecryptedMsg(hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_RandomOctets:
			local_cxsc_ParseRandomOctets(hParam, pstParam->ulValue, pstParam->aucData);
			break;

		case eCxSc_CryptoBlockSize:
			pstInstance->ucPlainBlkSize = (HUINT8)((pstParam->ulValue >> 8) & 0xFF);
			pstInstance->ucCipherBlkSize = (HUINT8)(pstParam->ulValue & 0xFF);
			break;

		case eCxSc_CardNumber:
			pstInstance->ulCardNumber = (HUINT32)pstParam->ulValue;
			break;

		case eCxSc_SequenceNumber:
			pstInstance->ulSeqNumber = (HUINT32)pstParam->ulValue;
			break;

		default:
			//## unknown parameter - ignore
			break;
		}
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxsc_ProcessParam(CxScInstance_t* pstInstance, Handle_t hParam)
{
	if(CxSc_GetInParam(pstInstance) == eCxSc_Ecm)
	{
		if(pstInstance->bEcmQuery == TRUE)
		{
			return local_cxsc_ProcessParamQuery(pstInstance, hParam);
		}
	}

	return local_cxsc_ProcessParamNormal(pstInstance, hParam);
}

STATIC HERROR local_cxsc_ParseResponse(CxScInstance_t* pstInstance, Handle_t hParam)
{
	HERROR	nErr;

	// sc에서 데이터를 다 읽었으면 parsing한다. 얻어온 정보를 structure로 돌려주는게 좋지 않겠나? viaccess 처럼 union으로 만들면 어때?
	// 모르는 PI 무시, reserved 데이터 무시, ..
	ENTER_FUNCTION;

#if defined(CONFIG_MW_CAS_CONAX_CPM) && defined(CONFIG_MW_CAS_CONAX_CPM_WITH_LPK)
	/*
		For LPK, Process Protected Data from S/C
	*/
	nErr = local_cxsc_DecryptParam(pstInstance);
	if (nErr != ERR_OK)
	{
		HxLOG_Error("local_cxsc_DecryptParam() fail...\n");
		return ERR_FAIL;
	}
#endif

	// parsing 도중 오류 발생할 가능성 있으므로 parsing 다 한 후에 parameter들 처리하도록 분리
	nErr = local_cxsc_GetParamList(pstInstance);

	if (nErr == ERR_OK)
	{
		nErr = local_cxsc_ProcessParam(pstInstance, hParam);
		local_cxsc_DeleteParam(pstInstance);
	}

	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxsc_ParseStatusWord(CxScInstance_t* pstInstance)
{
	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	HxLOG_Print("[SW][%02X %02X]\n", pstInstance->ucSW1, pstInstance->ucSW2);

	//## SW 처리
	if (pstInstance->ucSW1 == 0x90)
	{
		switch (pstInstance->ucSW2)
		{
		case 0x0:
			//	0x90 00: OK
			return ERR_OK;

		case 0x11:
			// 	0x90 11: bad or missing parameter -> reset card
			if (pstInstance->eScState == eCxSc_StateReady)
			{
				pstInstance->eScState = eCxSc_StateCommError;
				CXCORE_SendMsg(eCxMsg_Sc_RecoveryReset, HANDLE_NULL, (HUINT32)pstInstance->ucScSlot, (HUINT32)0);
			}
			break;

		case 0x12:
			//	0x90 12: access denied
			return CX_ERR_SC_ACCESS_DENIED;

		case 0x13:
			// 	0x90 13: bit error detected -> retreive new message and try again 이라는데?
			//  PT에서 bit error 발생시에 다시 처리하지 말고 버리라고 하네요...
			return CX_ERR_SC_BIT_ERROR;

		case 0x14:
			//	0x90 14: incompatible protocol
			break;

		case 0x16:
			//	0x90 16: insufficient information given
			break;

		case 0x17:
			//	0x90 17: Wrong PIN이라는군
			return CX_ERR_SC_WRONG_PIN;

		case 0x19:
			//	0x90 19: function is locked (pin 너무 많이 틀렸어)
			return CX_ERR_SC_PIN_LOCKED;

		case 0x20:
			//	0x90 20: session id invalid
			break;

		case 0x30:
			//	0x90 30: function is currently not available
			return CX_ERR_SC_CHANGE_PIN_FAILED;

		case 0x31:
			//	0x90 31: sign pin has not been set
			break;

		case 0x40:
			//	0x90 40: unsupported alogrithm
			break;

		case 0x41:
			//	0x90 41: invalid key selected
			break;

		case 0x42:
			//	0x90 42: signature verification failed
			break;

		case 0x43:
			//	0x90 43: decryption failed
			break;

		case 0x44:
			//	0x90 44: cryptographic operation failed
			break;

		default:
			//unknown - ignore it
			if (pstInstance->eScState == eCxSc_StateReady)
			{
				pstInstance->eScState = eCxSc_StateCommError;
				CXCORE_SendMsg(eCxMsg_Sc_RecoveryReset, HANDLE_NULL, (HUINT32)pstInstance->ucScSlot, (HUINT32)0);
			}
			else
			{
				if (pstInstance->ucSW2 == 0x09)
				{
					return CX_ERR_SC_INCORRECT_CARD;
				}
			}
			break;
		}
	}
	else if (pstInstance->ucSW1 == 0x98)
	{
		return CX_ERR_SC_READ;
	}
	else
	{
		switch (pstInstance->ucSW1)
		{
		case 0x60:
			// 0x60 60 : it means NULL byte, it requests no action on data transfer. shall wait for a character conveying a procedure byte.....( 뭔말인지..)
			return CX_ERR_SC_READ_NULL_BYTE;

		case 0x67:
			//0x67 00: invalid length in iso header -> reset card
		case 0x6B:
			//0x6B 00: reference is incorrect
		case 0x6D:
			//0x6D 00: invalid command -> display "incorrect card"
		case 0x6E:
			//0x6E 00: invalid instruction class -> display "incorrect card" and reset card
		case 0x6F:
			//0x6F 00: no precise diagnosis is given
			if (pstInstance->eScState == eCxSc_StateReady)
			{
				pstInstance->eScState = eCxSc_StateInvalid;
				/*
					SESSION -> EMM -> SC -> UI -> SESSION으로 움직이면서 semaphore deadlock이 발생하는 구조가 있다. (CAT 전달 등)
					Recovery Reset전에 필요하면 UI를 그리는 구조로 변경...
				*/
				//CXUI_ScStateChanged(pstInstance->ucScSlot, pstInstance->eScState, TRUE);
				CXCORE_SendMsg(eCxMsg_Sc_RecoveryReset, HANDLE_NULL, (HUINT32)pstInstance->ucScSlot, (HUINT32)TRUE);
			}
			else
			{
				if (pstInstance->ucSW1 == 0x6D || pstInstance->ucSW1 == 0x6E)
				{
					return CX_ERR_SC_INCORRECT_CARD;
				}
			}
			break;

		default:
			if (pstInstance->eScState == eCxSc_StateReady)
			{
				pstInstance->eScState = eCxSc_StateCommError;
				CXCORE_SendMsg(eCxMsg_Sc_RecoveryReset, HANDLE_NULL, (HUINT32)pstInstance->ucScSlot, (HUINT32)0);
			}
			break;
		}
	}

	LEAVE_FUNCTION;
	return ERR_FAIL;
}

STATIC HERROR local_cxsc_ProcessCommand(CxScInstance_t* pstInstance, Handle_t hParam)
{
	HERROR	nRet;
	HINT32	nRetry			= 0;
	HBOOL	bReRead			= FALSE;

	ENTER_FUNCTION;

	if (pstInstance == NULL)
	{
		return ERR_FAIL;
	}

	do
	{
		if (bReRead == FALSE)
		{
#ifdef DUMP_SMARTCARD_DATA
			CX_MemDump("Write to S/C", pstInstance->aucWriteBuf, pstInstance->usWriteBufLen);
#endif
			// s/c에 command를 보낸다
			nRet = local_cxsc_Write(pstInstance);
			if (nRet == ERR_FAIL)
			{
				//## 보내는데 문제 있었나? error 처리
				return ERR_FAIL;
			}

			// sw 처리
			nRet = local_cxsc_ParseStatusWord(pstInstance);

			//read하기 전에 read buffer 클리어 한다
			pstInstance->usReadBufLen = 0;
		}
		else
		{
			nRet = CX_ERR_SC_READ;
		}

		while (nRet == CX_ERR_SC_READ)
		{
			//	0x98 nn이면 nn만큼 READ
			nRet = local_cxsc_Read(pstInstance);
			if (nRet == ERR_OK)
			{
				nRet = local_cxsc_ParseStatusWord(pstInstance);
			}
		}

		if (nRet == CX_ERR_SC_READ_NULL_BYTE)
		{
			bReRead = TRUE;
		}
		else
		{
			bReRead = FALSE;
		}

		// NULL byte 인 경우만 command 다시 보낸다
		if (nRet != CX_ERR_SC_READ_NULL_BYTE)
		{
			break;
		}
	}
	while (++nRetry < CX_SC_MAX_RETRY_SEND_CMD);

	// 읽어온 데이터가 있다면 처리
	if (nRet != ERR_FAIL && pstInstance->usReadBufLen)
	{
#ifdef DUMP_SMARTCARD_DATA
		CX_MemDump("Read from S/C", pstInstance->aucReadBuf, pstInstance->usReadBufLen);
#endif
		local_cxsc_ParseResponse(pstInstance, hParam);
	}

	LEAVE_FUNCTION;
	return nRet;
}

STATIC void local_cxsc_MakeHeader(CxScInstance_t* pstInstance, HUINT8 ucSessionIdx, CxSc_Cmd_t eCmd, HUINT8 ucDataLen)
{
	pstInstance->aucWriteBuf[0] = 0xDD;
	pstInstance->aucWriteBuf[1] = (HUINT8)eCmd;
	pstInstance->aucWriteBuf[2] = 0x00;

	pstInstance->aucWriteBuf[3] = ucSessionIdx;
	pstInstance->aucWriteBuf[4] = ucDataLen;

	// 전체 command의 길이를 기록해둔다
	pstInstance->usWriteBufLen = ucDataLen + 5;		//5 = header length

	HxLOG_Print("[SC Header][%02X %02X %02X %02X %02X]\n", pstInstance->aucWriteBuf[0],				//
																	pstInstance->aucWriteBuf[1],	//
																	pstInstance->aucWriteBuf[2],	//
																	pstInstance->aucWriteBuf[3],	//
																	pstInstance->aucWriteBuf[4]);	//
}

#define ______________________________________________________________
HERROR CXAPI_ScGetResetReason(HINT32 nScSlot, CxScReset_Reason_t *peReason)
{
	CxScInstance_t *pstInstance;

	if (nScSlot < 0 || nScSlot >= CX_SC_NUMBER_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	if(peReason == NULL)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxScSemId);

	pstInstance = s_pstCxScInstance[nScSlot];

	if(pstInstance->bRecoveryReset)
		*peReason = eScReset_By_Recovery;
	else
		*peReason = eScReset_By_ISR;

	CX_SEM_Release(s_ulCxScSemId);

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
STATIC HUINT8* local_cxsc_DEBUG_GetPLStr(HUINT8 ucPL)
{
	HUINT8* pszName;

	switch (ucPL)
	{
		ENUM_TO_STR(eCxSc_CassVer)
		ENUM_TO_STR(eCxSc_CaDescEmm)
		ENUM_TO_STR(eCxSc_SessionInfo)
		ENUM_TO_STR(eCxSc_Cw)
		ENUM_TO_STR(eCxSc_CaSysId)
		ENUM_TO_STR(eCxSc_OrderReqApdu)
		ENUM_TO_STR(eCxSc_RetChanConfig)
		ENUM_TO_STR(eCxSc_CountryIndicator)
		ENUM_TO_STR(eCxSc_MatRatLevel)
		ENUM_TO_STR(eCxSc_AccessStatus)
		ENUM_TO_STR(eCxSc_SubscriptionStatus)
		ENUM_TO_STR(eCxSc_OrderInfo)
		ENUM_TO_STR(eCxSc_AcceptViewingInfo)
		ENUM_TO_STR(eCxSc_EventStatus)
		ENUM_TO_STR(eCxSc_TokenDebitInfo)
		ENUM_TO_STR(eCxSc_PurseStatus)
		ENUM_TO_STR(eCxSc_DebitStatus)
		ENUM_TO_STR(eCxSc_CreditStatus)
		ENUM_TO_STR(eCxSc_SignatureResp)
		ENUM_TO_STR(eCxSc_EncryptedMsg)
		ENUM_TO_STR(eCxSc_DecryptedMsg)
		ENUM_TO_STR(eCxSc_CryptoBlockSize)
		ENUM_TO_STR(eCxSc_CardNumber)
		ENUM_TO_STR(eCxSc_SequenceNumber)
		ENUM_TO_STR(eCxSc_RandomOctets)
		ENUM_TO_STR(eCxSc_PinStatus)
		ENUM_TO_STR(eCxSc_HostData)
		ENUM_TO_STR(eCxSc_ProtectedData)
		ENUM_TO_STR(eCxSc_DebitLogData)

	default:
		pszName = (HUINT8 *)"Unknown";
		break;
	}

	return pszName;
}

STATIC HUINT8* local_cxsc_DEBUG_GetScStateStr(CxSc_State_t eScState)
{
	HUINT8* pszName;

	switch (eScState)
	{
		ENUM_TO_STR(eCxSc_StateNone)
		ENUM_TO_STR(eCxSc_StateInserted)
		ENUM_TO_STR(eCxSc_StateReset)
		ENUM_TO_STR(eCxSc_StateCassVerReceived)
		ENUM_TO_STR(eCxSc_StateReady)
		ENUM_TO_STR(eCxSc_StateDifferentVer)
		ENUM_TO_STR(eCxSc_StateInvalid)
		ENUM_TO_STR(eCxSc_StateCommError)

	default:
		pszName = (HUINT8 *)"Unknown";
		break;
	}

	return pszName;
}

STATIC HUINT8* local_cxsc_DEBUG_GetStatusSelectType(CxStatusSelectType_t eStatusSelectType)
{
	HUINT8* pszName;

	switch (eStatusSelectType)
	{
		ENUM_TO_STR(eCxCaStatus_AllSubscription)
		ENUM_TO_STR(eCxCaStatus_AllPpvEvent)
		ENUM_TO_STR(eCxCaStatus_AllPurse)
		ENUM_TO_STR(eCxCaStatus_Subscription)
		ENUM_TO_STR(eCxCaStatus_PpvEvent)
		ENUM_TO_STR(eCxCaStatus_Purse)
		ENUM_TO_STR(eCxCaStatus_Debit)
		ENUM_TO_STR(eCxCaStatus_Credit)

	default:
		pszName = (HUINT8 *)"Unknown";
		break;
	}

	return pszName;
}

static HUINT8* local_cxsc_GetRetentionStr(eDxCopyrightControl_ConaxRetentionLimit_e eRetentionLimit)
{
	switch (eRetentionLimit)
	{
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_CopyFree_NoLimit);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_NoLimit);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_1Week);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_2Days);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_1Day);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_12Hours);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_6Hours);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_3Hours);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_90Min);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_NoExpLimit);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_ExpDays_360);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_ExpDays_90);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_ExpDays_30);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_ExpDays_14);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_ExpDays_7);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_ExpDays_2);
		ENUM_TO_STR(eDxCopyrightControl_ConaxRetentionLimit_ExpDays_1);
	}
	return "Invalid Value";
}

HERROR CXSC_PrintUriValue(DxCopyrightControl_ConaxDrmInfo_t* pstDrmInfo)
{
	if (pstDrmInfo == NULL)
	{
		HxLOG_Print("pstDrmInfo is NULL !! \n");
		return ERR_FAIL;
	}

	HxLOG_Print("==================================================== \n");
	HxLOG_Print(" eApsCopyCtrl (0x%X) : ", pstDrmInfo->eApsCopyCtrl);
	switch (pstDrmInfo->eApsCopyCtrl)
	{
	case eDxCopyrightControl_ConaxApsCopyCtrl_Off:
		HxLOG_Print("eDxCopyrightControl_ConaxApsCopyCtrl_Off \n");		break;
	case eDxCopyrightControl_ConaxApsCopyCtrl_BurstOff:
		HxLOG_Print("eDxCopyrightControl_ConaxApsCopyCtrl_BurstOff \n");	break;
	case eDxCopyrightControl_ConaxApsCopyCtrl_Burst2Line:
		HxLOG_Print("eDxCopyrightControl_ConaxApsCopyCtrl_Burst2Line \n");	break;
	case eDxCopyrightControl_ConaxApsCopyCtrl_Burst4Line:
		HxLOG_Print("eDxCopyrightControl_ConaxApsCopyCtrl_Burst4Line \n");	break;
	default:
		HxLOG_Print("Invalid value \n");				break;
	}

	HxLOG_Print(" eEmiCopyCtrl (0x%X) : ", pstDrmInfo->eEmiCopyCtrl);
	switch (pstDrmInfo->eEmiCopyCtrl)
	{
	case eDxCopyrightControl_ConaxEmiCopyCtrl_Freely:
		HxLOG_Print("eDxCopyrightControl_ConaxEmiCopyCtrl_Freely \n");		break;
	case eDxCopyrightControl_ConaxEmiCopyCtrl_NoMore:
		HxLOG_Print("eDxCopyrightControl_ConaxEmiCopyCtrl_NoMore \n");	break;
	case eDxCopyrightControl_ConaxEmiCopyCtrl_Once:
		HxLOG_Print("eDxCopyrightControl_ConaxEmiCopyCtrl_Once \n");		break;
	case eDxCopyrightControl_ConaxEmiCopyCtrl_Never:
		HxLOG_Print("eDxCopyrightControl_ConaxEmiCopyCtrl_Never \n");		break;
	default:
		HxLOG_Print("Invalid value \n");				break;
	}

	HxLOG_Print(" eICT (0x%X) : ", pstDrmInfo->eICT);
	switch (pstDrmInfo->eICT)
	{
	case eDxCopyrightControl_ConaxICT_Off:
		HxLOG_Print("eDxCopyrightControl_ConaxICT_Off \n");	break;
	case eDxCopyrightControl_ConaxICT_On:
		HxLOG_Print("eDxCopyrightControl_ConaxICT_On \n");	break;
	default:
		HxLOG_Print("Invalid value \n");	break;
	}

	HxLOG_Print(" eRedistributionCtrl (0x%X) : ", pstDrmInfo->eRedistributionCtrl);
	switch (pstDrmInfo->eRedistributionCtrl)
	{
	case eDxCopyrightControl_ConaxRedistributionCtrl_Off:
		HxLOG_Print("eDxCopyrightControl_ConaxRedistributionCtrl_Off \n");	break;
	case eDxCopyrightControl_ConaxRedistributionCtrl_On:
		HxLOG_Print("eDxCopyrightControl_ConaxRedistributionCtrl_On \n");	break;
	default:
		HxLOG_Print("Invalid value \n");				break;
	}

	HxLOG_Print(" eRetentionLimit (0x%X) : %s \n", pstDrmInfo->eRetentionLimit, local_cxsc_GetRetentionStr(pstDrmInfo->eRetentionLimit));

	HxLOG_Print(" eTrickPlayCtrl (0x%X) : ", pstDrmInfo->eTrickPlayCtrl);
	switch (pstDrmInfo->eTrickPlayCtrl)
	{
	case eDxCopyrightControl_ConaxTrickPlayCtrl_NoRestriction:
		HxLOG_Print("eDxCopyrightControl_ConaxTrickPlayCtrl_NoRestriction \n");	break;
	case eDxCopyrightControl_ConaxTrickPlayCtrl_FF2X:
		HxLOG_Print("eDxCopyrightControl_ConaxTrickPlayCtrl_FF2X \n");	break;
	case eDxCopyrightControl_ConaxTrickPlayCtrl_FF4X:
		HxLOG_Print("eDxCopyrightControl_ConaxTrickPlayCtrl_FF4X \n");	break;
	case eDxCopyrightControl_ConaxTrickPlayCtrl_FF8X:
		HxLOG_Print("eDxCopyrightControl_ConaxTrickPlayCtrl_FF8X \n");	break;
	case eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpFF:
		HxLOG_Print("eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpFF \n");		break;
	case eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpPauseFF:
		HxLOG_Print("eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpPauseFF \n");	break;
	default:
		HxLOG_Print("Invalid value \n");					break;
	}

	HxLOG_Print(" CxMR_e (0x%X) : ", pstDrmInfo->eMR);
	switch (pstDrmInfo->eMR)
	{
	case eDxCopyrightControl_ConaxMaturityRating_Off:
		HxLOG_Print("eDxCopyrightControl_ConaxMaturityRating_Off \n");	break;
	case eDxCopyrightControl_ConaxMaturityRating_G:
		HxLOG_Print("eDxCopyrightControl_ConaxMaturityRating_G \n");	break;
	case eDxCopyrightControl_ConaxMaturityRating_PG:
		HxLOG_Print("eDxCopyrightControl_ConaxMaturityRating_PG \n");	break;
	case eDxCopyrightControl_ConaxMaturityRating_A:
		HxLOG_Print("eDxCopyrightControl_ConaxMaturityRating_A \n");	break;
	case eDxCopyrightControl_ConaxMaturityRating_X:
		HxLOG_Print("eDxCopyrightControl_ConaxMaturityRating_X \n");	break;
	case eDxCopyrightControl_ConaxMaturityRating_XXX:
		HxLOG_Print("eDxCopyrightControl_ConaxMaturityRating_XXX \n");	break;
	default:
		HxLOG_Print("Invalid value \n");	break;
	}

	HxLOG_Print(" CxDisableAnalOutput_e (0x%X) : ", pstDrmInfo->eDisableAnalOutput);
	switch (pstDrmInfo->eDisableAnalOutput)
	{
	case eCxDisableAnalOutput_Off:
		HxLOG_Print("eCxDisableAnalOutput_Off \n");	break;
	case eCxDisableAnalOutput_On:
		HxLOG_Print("eCxDisableAnalOutput_On \n");	break;
	default:
		HxLOG_Print("Invalid value \n");				break;
	}

	HxLOG_Print("==================================================== \n");

	return ERR_OK;
}

void CMD_CXSC_PrintScContext(void)
{
	int				i;
	CxScInstance_t* pstContext;

	for (i = 0; i < CX_SC_NUMBER_OF_INSTANCE; i++)
	{
		pstContext = s_pstCxScInstance[i];

		HxLOG_Warning(HxANSI_COLOR_GREEN("CONAX SC (%d)\n"), i);

		HxLOG_Warning("\t ucScSlot (%d)\n", pstContext->ucScSlot);
		HxLOG_Warning("\t ucCassVer (0x%x)\n", pstContext->ucCassVer);
		HxLOG_Warning("\t ucPlainBlkSize (%d)\n", pstContext->ucPlainBlkSize);
		HxLOG_Warning("\t ucCipherBlkSize (%d)\n", pstContext->ucCipherBlkSize);
		HxLOG_Warning("\t ulCardNumber (0x%08x, %d)\n", pstContext->ulCardNumber, pstContext->ulCardNumber);
		HxLOG_Warning("\t ulSeqNumber (%d)\n", pstContext->ulSeqNumber);
		HxLOG_Warning("\t usCaSysId (0x%x)\n", pstContext->usCaSysId);

		HxLOG_Warning("\t usCountry (0x%x)\n", pstContext->usCountry);
		HxLOG_Warning("\t ucMatRatLevel (%d)\n", pstContext->ucMatRatLevel);
		HxLOG_Warning("\t ucSessionCount (%d)\n", pstContext->ucSessionCount);
		HxLOG_Warning("\t ucSW1 (0x%02x)\n", pstContext->ucSW1);
		HxLOG_Warning("\t ucSW2 (0x%02x)\n", pstContext->ucSW2);
		HxLOG_Warning("\t usReadBufLen (%d)\n", pstContext->usReadBufLen);
		HxLOG_Warning("\t usWriteBufLen (%d)\n", pstContext->usWriteBufLen);

		HxLOG_Warning("\t eScState (%s)\n", local_cxsc_DEBUG_GetScStateStr(pstContext->eScState));
		HxLOG_Warning("\t ucRetryCount (%d)\n", pstContext->ucRetryCount);
		HxLOG_Warning("\t ECM message (%d)\n", CxSc_GetInParam(pstContext) == eCxSc_Ecm ? TRUE : FALSE);
		HxLOG_Warning("\t bBridgeControl (%d)\n", pstContext->bBridgeControl);
		HxLOG_Warning("\t ulProtectedDataStartOffset (%d)\n", pstContext->ulProtectedDataStartOffset);
		HxLOG_Warning("\t ulProtectedDataEndOffset (%d)\n", pstContext->ulProtectedDataEndOffset);

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
		HxLOG_Warning("\t bFPFromProtectedData (%d)\n", pstContext->bFPFromProtectedData);
#endif
#if defined(CONFIG_MW_CAS_CONAX_URI)
		HxLOG_Warning("\t bFromProtectedData (%d)\n", pstContext->bFromProtectedData);
#endif
	}
}
#endif


/*********************** End of File ******************************/

