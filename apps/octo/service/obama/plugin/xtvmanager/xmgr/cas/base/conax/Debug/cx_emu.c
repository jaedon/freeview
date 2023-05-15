/**
	@file     cx_emul.c
	@brief    file_name & simple comment.

	Description: CONAX Pin code API  \n
	Module: Application(Layer), AP_CORE(module), CAS(module), CONAX(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/30		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>


#include <conax.h>
#include <cxcore_sc.h>
#include <cxcore_main.h>
#include "../local_include/_xmgr_cas_cx_util.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#if defined(WIN32)
#define	USE_CONAX_EMULATOR
#endif

#if defined(USE_CONAX_EMULATOR)

#define	CX_SC_NUMBER_OF_INSTANCE		1				// 일단 slot 1개만 고려 하지... 복잡혀..

#define	CX_SC_READ_BUFFER_SIZE		4096
#define	CX_SC_WRITE_BUFFER_SIZE		1024

#define	CX_SC_MAX_PARAM				10

#define	CX_SC_INTERFACE_VERSION			0x40		// version 4.1
//#define	CX_CORE_VERSION_STR				"01.00.00"


#define	CX_SC_MIN_SUBSCRIPTION_STATUS			40
#define	CX_SC_MIN_EVENT_STATUS					100
#define	CX_SC_MIN_DEBIT_STATUS					100
#define	CX_SC_MIN_CREDIT_STATUS					20


typedef struct {
	CxSc_OutputParam_t 	eType;
	HUINT32 				ulValue;
	HUINT8 				*pucData;
} CxParam_t;


typedef struct {
	HUINT32				ulCount;
	CxParam_t 			stParam[CX_SC_MAX_PARAM];
} CxParamList_t;


typedef struct {
	HUINT8					ucScSlot;
	HUINT8					ucCassVer;
	HUINT8					ucPlainBlkSize;
	HUINT8					ucCipherBlkSize;
	HUINT32					ulCardNumber;
	HUINT32					ulSeqNumber;
	HUINT16					usCaSysId;
	HUINT16					usCountry;
	HUINT8					ucMatRatLevel;
	HUINT8					ucSessionCount;
	HUINT8					ucSW1;
	HUINT8					ucSW2;
	HUINT16 					usReadBufLen;
	HUINT16					usWriteBufLen;
	HUINT8 					aucReadBuf[CX_SC_READ_BUFFER_SIZE];
	HUINT8					aucWriteBuf[CX_SC_WRITE_BUFFER_SIZE];
	Handle_t					hSession;
	CxSc_State_t				eScState;
	CxStatusSelectType_t		eStatusSelectType;
	CxStatusList_t				*pstStatusList;
	CxParamList_t 			stScParamList;
} CxScInstance_t;





/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HUINT8	s_nMatRatLevel = CX_MATURITY_RATE_X;
STATIC HUINT8	s_szCasPin[] = "1234";


CxScInstance_t 		s_pstCxEmulScInstance[CX_SC_NUMBER_OF_INSTANCE] =
{
	{ 0,						// ucScSlot
	  64,						// ucCassVer
	  0, 0,						// ucPlainBlkSize, ucCipherBlkSize
	  (HUINT32)1900001000L,		// ulCardNumber
	  0,						// ulSeqNumber
	  0x0B00,					// usCaSysId
	  0x2C,						// usCountry
	  8,						// ucMatRatLevel
	  3,						// ucSessionCount
	  0,						// ucSW1
	},
};


STATIC CxSubscriptionStatus_t s_stSubStatus[] =
{
	{
		0,														//	usSubscriptionRef;
		"Canal+",												//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2003, 04, 02, 0, 0,}, {2003, 04, 03, 0, 0,}},			//	*stStartDate[2]; YYYY,DD,MM, HH:MM
		{{2004, 15, 01, 0, 0,}, {2004, 01, 03, 0, 0,}},			//	*stEndDate[2];
		{{0xF4, 0x68, 0x95, 0xAC}, {0x00, 0x76, 0x4B, 0x23}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Fox Kids",												//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2003, 04, 02, 0, 0,}, {2003, 04, 03, 0, 0,}},			//	*stStartDate[2];
		{{2004, 15, 01, 0, 0,}, {2004, 01, 03, 0, 0,}},			//	*stEndDate[2];
		{{0xB2, 0x35, 0x8A, 0x49}, {0x10, 0xDE, 0x0A, 0x43}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels1",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels2",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels3",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels4",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels5",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels6",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels7",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels8",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels9",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
	{
		0,														//	usSubscriptionRef;
		"Conax Channels10",										//	szLabe[CX_MAX_LABEL_LENGTH]l;
		{{2002, 01, 12, 0, 0,}, {2002, 01, 11, 0, 0,}},			//	*stStartDate[2];
		{{2002, 31, 12, 0, 0,}, {2002, 30, 11, 0, 0,}},			//	*stEndDate[2];
		{{0x01, 0x00, 0x00, 0xFF}, {0x01, 0x00, 0x00, 0xFF}},	//	szEntitlemnet[2][CX_MAX_ENTITLEMENT_LENGTH];
	},
};


STATIC CxEventStatus_t s_stEventStatus[] =
{
	{
		0,							//	ulEventTag;
		"15897-NVOD product 2",		//	*szLabel;
		{2002, 17, 12, 06, 0},		//	*szStartTime;
		{2002, 19, 12, 06, 0},		//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Shine",					//	*szLabel;
		{2004,16, 8, 16, 30},		//	*szStartTime;
		{2004,21, 8, 22, 50},		//	*szEndTime;
		0x00,						//	ucAuthType;
		0x006D,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Pac-man",					//	*szLabel;
		{2004,16, 8, 23, 00},		//	*szStartTime;
		{2004,26, 8, 21, 40},			//	*szEndTime;
		0x02,						//	ucAuthType;
		0x0003,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V1",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x00,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V2",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V3",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V4",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V5",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V6",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V7",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V8",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V9",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
	{
		0,							//	ulEventTag;
		"Spirit-Hasten fran V10",	//	*szLabel;
		{2002,01,12, 0, 0},			//	*szStartTime;
		{2002,31,12, 0, 0},			//	*szEndTime;
		0x01,						//	ucAuthType;
		0x0100,						//	usLimitedLeft
	},
} ;


STATIC CxUserText_t	s_pstUserText[]=
{
	{
		"Message 1",				//szText
		1,							//ucDisplayed;
		0,							//usDuration;		// 이 값이 0이면 메시지를 표시하지는 않고 왔다고만 알려준다 (mailbox message). 단위 second.
		0,							//ucSeqNo;
		0,							//ucRepeat;		// 없으면 1회만 출력
		0,							//usInterval;		// 메시지를 표시하는 간격 (10초단위로 들어오나 *10하여 second단위로 저장하고 있음)
		0,							//stUnixTime;
	},
	{
		"Message 2",
		1,
		0,
		0,
		0,
		0,
		0,
	},

	{
		"Message 3",
		1,
		0,
		0,
		0,
		0,
		0,
	},
	{
		"Message 4",
		1,
		0,
		0,
		0,
		0,
		0,
	},
	{
		"Message 5",
		1,
		0,
		0,
		0,
		0,
		0,
	},
	{
		"Message 6",
		1,
		0,
		0,
		0,
		0,
		0,
	},
	{
		"Message 7",
		1,
		0,
		0,
		0,
		0,
		0,
	},
	{
		"Message 8",
		1,
		0,
		0,
		0,
		0,
		0,
	},
	{
		"Message 9",
		1,
		0,
		0,
		0,
		0,
		0,
	},
	{
		"Message 10",
		1,
		0,
		0,
		0,
		0,
		0,
	},
	{
		"Message 11",
		1,
		0,
		0,
		0,
		0,
		0,
	},
};


STATIC CxPurseStatus_t	s_stPurseStatus[] =
{
#if 0
	{
		HUINT16					usPurseRef;
		HUINT8					szLabel[CX_MAX_LABEL_LENGTH];
		HUINT32					ulBalance;
	},
#endif
	{
		1,
		"Purse Status 1",
		100
	},
	{
		2,
		"Purse Status 2",
		200
	},
	{
		3,
		"Purse Status 3",
		300
	},
	{
		4,
		"Purse Status 4",
		400
	},
	{
		5,
		"Purse Status 5",
		500
	},
	{
		15896,
		"Purse Status 6",
		600
	},


} ;


STATIC CxDebitStatus_t	s_stDebitStatus[] =
{
#if 0
	{
		HUINT32			ulEventTag;	//24 bit
		HUINT32			ulConsumedTokens;	//24 bit
		CxTime_t		stLastAccessTime;
		HUINT8			szLabel[CX_MAX_LABEL_LENGTH];
	}
#endif
	{
		0,
		458,
		{2003,07,18, 17, 30},
		"Serie A"
	},
	{
		0,
		42,
		{2003,07,04, 14, 50},
		"Matrix"
	},
	{
		0,
		274,
		{2003,06,20, 03, 00},
		"Formula 1"
	}
};

STATIC CxCreditStatus_t	s_stCreditStatus[] =
{
#if 0
	{
		HUINT32		ulCreditTokens;
		HUINT8		szLabel[CX_MAX_LABEL_LENGTH];
	},
#endif
	{
		750,
		"TestTrans1"
	},
	{
		200,
		"TestTrans2"
	}
};

STATIC HUINT8 s_szProgramLable[5][128] =
{
	"ProgramLable 1",
	"ProgramLable 2",
	"ProgramLable 3",
	"ProgramLable 4",
	"ProgramLable 5",
} ;


/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/



/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/


/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

// Pin code
HERROR CXAPI_UiEnterCaPin (HINT32 nScSlot, HUINT8 *pucPin)
{
	XMGR_CAS_CX_PARAM_UNUSED(nScSlot);
	HxLOG_Print("[Enter]\r\n");

	if (MWC_UTIL_DvbStrncmp(s_szCasPin, pucPin, 4))
		return	CX_ERR_UI_WRONG_PIN;

	return ERR_OK;
}


HERROR CXAPI_UiAcceptMaturityRatingOverride(Handle_t hStbHandle, HUINT8 *pucPin)
{
	XMGR_CAS_CX_PARAM_UNUSED(hStbHandle);

	HxLOG_Print("[Enter]\r\n");

	if (MWC_UTIL_DvbStrncmp(s_szCasPin, pucPin, 4))
		return	CX_ERR_UI_WRONG_PIN;

	return ERR_OK;
}


HERROR CXAPI_UiStartViewingPPV(Handle_t hStbHandle, HUINT8 *pucPin)
{
	XMGR_CAS_CX_PARAM_UNUSED(hStbHandle);

	HxLOG_Print("[Enter]\r\n");

	if (MWC_UTIL_DvbStrncmp(s_szCasPin, pucPin, 4))
		return	CX_ERR_UI_WRONG_PIN;

	return ERR_OK;
}


HERROR CXAPI_UiStartViewingTokenPPV(Handle_t hStbHandle, HUINT8 *pucPin)
{
	XMGR_CAS_CX_PARAM_UNUSED(hStbHandle);

	HxLOG_Print("[Enter]\r\n");

	if (MWC_UTIL_DvbStrncmp(s_szCasPin, pucPin, 4))
		return	CX_ERR_UI_WRONG_PIN;

	return ERR_OK;
}


HERROR		CXAPI_UiChangePin(HINT32 nScSlot, HUINT8 *pucOldPin, HUINT8 *pucNewPin, HUINT8 *pucNewPin2)
{
	HERROR nErr = ERR_OK;

	XMGR_CAS_CX_PARAM_UNUSED(nScSlot);
	XMGR_CAS_CX_PARAM_UNUSED(pucNewPin2);

	HxLOG_Print("[Enter]\r\n");

	if (pucOldPin == NULL || pucNewPin == NULL || pucNewPin2 == NULL)
	{
		return ERR_FAIL;
	}

	if (MWC_UTIL_DvbStrncmp(pucNewPin, pucNewPin2, 4))
	{
	 	return CX_ERR_UI_DIFFERENT_NEW_PIN;
	}

	if (MWC_UTIL_DvbStrncmp(s_szCasPin, pucOldPin, 4))
	{
		return	CX_ERR_UI_WRONG_PIN;
	}

	//nErr = CX_ERR_SC_PIN_LOCKED;

	if (nErr == ERR_OK)
	{
		MWC_UTIL_DvbStrncpy(s_szCasPin, pucNewPin, 4);
	}

	return ERR_OK;
}


HERROR CXAPI_UiGetMaturityRatingLevel(HINT32 nScSlot, HUINT8 *pucMaturityRating)
{
	XMGR_CAS_CX_PARAM_UNUSED(nScSlot);

	HxLOG_Print("[Enter]\r\n");

	if (pucMaturityRating)
	{
		*pucMaturityRating = s_nMatRatLevel;
		return ERR_OK;
	}

	return ERR_OK;
}


HERROR CXAPI_UiChangeMaturityRating(HINT32 nScSlot, HUINT8 ucMaturityRating, HUINT8 *pucPin)
{
	XMGR_CAS_CX_PARAM_UNUSED(nScSlot);

	HxLOG_Print("[Enter]\r\n");

	if (!pucPin)
		return 1;

	if (MWC_UTIL_DvbStrncmp(pucPin, s_szCasPin, 4))
		return CX_ERR_UI_WRONG_PIN;

	s_nMatRatLevel = ucMaturityRating;

	return ERR_OK;
}


HERROR		CXAPI_UiGetVersionInfo(HINT32 nScSlot, CxVersionInfo_t *pstInfo)
{
	CxScInstance_t pstInstance;

	if (pstInfo == NULL)
		return ERR_FAIL;

	HxLOG_Print("[Enter]\r\n");

	pstInstance = s_pstCxEmulScInstance[nScSlot];


	// 만약 카드가 없다면 아래 값들은 (-1)이 들어갈꺼다...
	pstInfo->nSessionCnt 	= pstInstance.ucSessionCount;
	pstInfo->nCountryInd 	= pstInstance.usCountry;
	pstInfo->nCaSysId 		= pstInstance.usCaSysId;

	pstInfo->ulCardNumber	= pstInstance.ulCardNumber;
	pstInfo->pszSwVer		= (HUINT8*)CX_CORE_VERSION_STR;

	pstInfo->ulChipId		= 0;
	pstInfo->nHostVer 		= CX_SC_INTERFACE_VERSION;

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}


HERROR	CXAPI_UiGetSCState(HINT32 lScSlot)
{
	CxScInstance_t pstInstance;

	HxLOG_Print("[Enter]\r\n");

	if (lScSlot < 0 || lScSlot > CX_SC_NUMBER_OF_INSTANCE)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstInstance = s_pstCxEmulScInstance[lScSlot];
	pstInstance.eScState = eCxSc_StateReady;

	// Ready가 되어야 Smart Card가 잘 된다고 생각 한다.
	if (pstInstance.eScState != eCxSc_StateReady)
	{
		HxLOG_Print(" == Invalid SC State : %d\r\n", pstInstance.eScState);
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}



CxStatusList_t *CXAPI_UiGetStatusInfo(HINT32 nScSlot, CxStatusSelectType_t eType, HUINT32 ulRef)
{
	HUINT16		usPurseRef;
	HINT32		i, lSize, lIndex;
	CxStatusList_t *pstList = NULL , *pstTmp = NULL;
	CxUiMsgType_t	eMsg;

	HxLOG_Print("[Enter]\r\n");

	XMGR_CAS_CX_PARAM_UNUSED(nScSlot);
	XMGR_CAS_CX_PARAM_UNUSED(ulRef);

	pstList = (CxStatusList_t *)OxAP_Malloc (sizeof(CxStatusList_t));

	pstTmp = pstList;
	//pstInstance.pstStatusList = &s_stSubStatus;
	switch (eType)
	{
	case eCxCaStatus_AllSubscription:
	case eCxCaStatus_Subscription:
		eMsg = eCxUi_GroupMenu_Subscription;
		lSize = sizeof(s_stSubStatus) / sizeof(CxSubscriptionStatus_t);

		lIndex = 0;
		for (i=0; i < CX_SC_MIN_SUBSCRIPTION_STATUS; i++)
		{
			lIndex = (lSize == lIndex) ? 0 : lIndex ;
			pstTmp->stStatus.stSubscription = s_stSubStatus[lIndex];
			pstTmp->pstNext= NULL;
			if (i != (CX_SC_MIN_SUBSCRIPTION_STATUS -1))
			{
				pstTmp->pstNext= (CxStatusList_t *)OxAP_Malloc (sizeof(CxStatusList_t));
				pstTmp = pstTmp->pstNext;
			}
			lIndex++;
		}
		pstTmp = NULL;
		break;

	case eCxCaStatus_AllPpvEvent:
	case eCxCaStatus_PpvEvent:
		eMsg = eCxUi_GroupMenu_Event;
		lSize = sizeof(s_stEventStatus) / sizeof(CxEventStatus_t);

		lIndex = 0;
		for (i=0; i < CX_SC_MIN_EVENT_STATUS; i++)
		{
			lIndex = (lSize == lIndex) ? 0 : lIndex ;
			pstTmp->stStatus.stEvent = s_stEventStatus[lIndex];
			pstTmp->pstNext= NULL;
			if (i != (CX_SC_MIN_EVENT_STATUS -1))
			{
				pstTmp->pstNext= (CxStatusList_t *)OxAP_Malloc (sizeof(CxStatusList_t));
				pstTmp = pstTmp->pstNext;
			}
			lIndex++;
		}
		pstTmp = NULL;
		break;

	case eCxCaStatus_AllPurse:
		eMsg = eCxUi_GroupMenu_Purse;
		lSize = sizeof(s_stPurseStatus)/sizeof(s_stPurseStatus[0]);

		for (i=0; i < lSize; i++)
		{
			pstTmp->stStatus.stPurse = s_stPurseStatus[i];
			pstTmp->pstNext= NULL;
			if (i != (lSize -1))
			{
				pstTmp->pstNext= (CxStatusList_t *)OxAP_Malloc (sizeof(CxStatusList_t));
				pstTmp = pstTmp->pstNext;
			}
		}
		pstTmp = NULL;
		break;

	case eCxCaStatus_Purse:
		eMsg = eCxUi_GroupMenu_Purse;
		lSize = sizeof(s_stPurseStatus)/sizeof(s_stPurseStatus[0]);
		usPurseRef = (HUINT16)ulRef;

		for (i=0; i < lSize; i++)
		{
			if (usPurseRef == s_stPurseStatus[i].usPurseRef)
			{
				pstTmp->stStatus.stPurse = s_stPurseStatus[i];
				pstTmp->pstNext= NULL;
				if (i != (lSize -1))
				{
					pstTmp->pstNext= (CxStatusList_t *)OxAP_Malloc (sizeof(CxStatusList_t));
					pstTmp = pstTmp->pstNext;
				}
			}
		}
		pstTmp = NULL;
		break;

	case eCxCaStatus_Debit:
		eMsg = eCxUi_GroupMenu_Debit;
		lSize = sizeof(s_stDebitStatus)/sizeof(s_stDebitStatus[0]);

		lIndex = 0;
		for (i=0; i < CX_SC_MIN_DEBIT_STATUS; i++)
		{
			lIndex = (lSize == lIndex) ? 0 : lIndex ;
			pstTmp->stStatus.stDebit = s_stDebitStatus[lIndex];
			pstTmp->pstNext= NULL;
			if (i != (CX_SC_MIN_DEBIT_STATUS -1))
			{
				pstTmp->pstNext= (CxStatusList_t *)OxAP_Malloc (sizeof(CxStatusList_t));
				pstTmp = pstTmp->pstNext;
			}
			lIndex++;
		}
		pstTmp = NULL;
		break;

	case eCxCaStatus_Credit:
		eMsg = eCxUi_GroupMenu_Credit;
		lSize = sizeof(s_stCreditStatus)/sizeof(s_stCreditStatus[0]);

		lIndex = 0;
		for (i=0; i < CX_SC_MIN_CREDIT_STATUS; i++)
		{
			lIndex = (lSize == lIndex) ? 0 : lIndex ;
			pstTmp->stStatus.stCredit = s_stCreditStatus[lIndex];
			pstTmp->pstNext= NULL;
			if (i != (CX_SC_MIN_CREDIT_STATUS -1))
			{
				pstTmp->pstNext= (CxStatusList_t *)OxAP_Malloc (sizeof(CxStatusList_t));
				pstTmp = pstTmp->pstNext;
			}
			lIndex++;
		}
		pstTmp = NULL;
		break;

	default:
		return NULL;
	}

	HxLOG_Print("[Exit]\r\n");

	return pstList;
}


HERROR CXAPI_UiGetUserText(HUINT32 *pulNumMsg, CxUserTextList_t *pstList)
{
	HUINT32 			i, ulCount;

	HxLOG_Print("[Enter]\r\n");

	if (pstList == NULL || *pulNumMsg == 0)
	{
		return ERR_FAIL;
	}

	if (pstList->pstUserText == NULL)
	{
		return ERR_FAIL;
	}

	ulCount = sizeof(s_pstUserText) / sizeof(CxUserText_t);

	if (ulCount > *pulNumMsg)
		ulCount = *pulNumMsg;

	pstList->pstUserText = (CxUserText_t *)OxAP_Malloc (sizeof(CxUserText_t)*ulCount);

	for (i = 0; i < ulCount; i++)
	{
		pstList->pstUserText[i] = s_pstUserText[i];
	}

	*pulNumMsg = ulCount;

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}



HERROR CXAPI_UiGetCountryIndicator(HINT32 nScSlot, HINT32 *nCountryIdx)
{
	CxScInstance_t pstInstance;

	HxLOG_Print("[Enter]\r\n");

	pstInstance = s_pstCxEmulScInstance[nScSlot];

	*nCountryIdx = pstInstance.usCountry;

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

HERROR CXAPI_UiFingerprintDisplayed(void)
{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	HxLOG_Print("[Enter]\r\n");

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}


HERROR CXAPI_UiUserTextDisplayed(HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	HxLOG_Print("[Enter]\r\n");

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}


#endif
// End of file

