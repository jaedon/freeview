#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "htype.h"
#include "di_err.h"

#include "cmdtool.h"
#include "di_smartcard.h"
#include "cmd_hwtest_parser_sc.h"

/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1


#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define UNUSED(x) (void)(x)

/* global variables */

/* static variables */

/* static functions */

/* extern functions */


static HUINT8 atrbuf[256];

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST) /*Used only in HW_TEST mode*/

typedef enum {
	SCI_P_T0			= 0,
	SCI_P_T1			= 1,
	SCI_P_T14			= 14,
	SCI_P_T_END,
} hwtest_SCI_PROTOCOL;

typedef enum
{
	eSCM_Reset_None,
	eSCM_Reset_Cold,
	eSCM_Reset_Warm,
}hwtest_SCM_ResetType_t;

typedef struct _hwtest_SCI_SC_PARAM
{
	hwtest_SCI_PROTOCOL eProtocol;
	HUINT32 ulClock;
	HUINT32	ulBaudrate;
	HUINT8	ucFI;
	HUINT8	ucDI;
	HUINT8	ucN;
	HUINT8	ucCWI;
	HUINT8	ucBWI;
} hwtest_SCI_SC_PARAM;

typedef struct _hwtest_ScAtrIInfo
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

	HBOOL		bSpecificMode;
	HBOOL		bImplicitlyDefined;
	HBOOL		bPpsUnable;
} hwtest_scAtrInfo_t;

#if defined(CONFIG_HWTEST_SMARTCARD_T14)
static hwtest_SCI_SC_PARAM 			s_stDefaultAtrParam =
{
		SCI_P_T14,	// protocol
		6000,		// Min Clock
		0,			// baudrate
		1,			// FI
		1,			// DI
		0,			// N
		13, 			// CWI
		5,			// BWI
};
#else
static hwtest_SCI_SC_PARAM 			s_stDefaultAtrParam =
{
		SCI_P_T0,		// protocol
		0,			// Min Clock
		0,			// baudrate
		1,			// FI
		1,			// DI
		0,			// N
		13,			// CWI
		4,			// BWI
};
#endif




HUINT8	ucHwTestAtr[34];
HUINT8 	ucScExist = 0;
hwtest_scAtrInfo_t  gScAtrInfo;
static char parser_sc_Str[500];



static PFN_HWTest_SmartCardCallback fnSmartCardCallback = NULL;

void P_HwTest_RegisterSC_Callback(PFN_HWTest_SmartCardCallback scCallback)
{
    if ( scCallback != NULL )
    {
        fnSmartCardCallback = scCallback ;
    }
}

static HUINT8	lastAtr[34];
static HUINT32 lastAtrLen=0;

static void P_HwTest_SC_Callback( HUINT32 unDeviceId, DI_SC_STATUS nStatus )
{
	CMD_Printf("Smartcard(%ld) is %s\n", unDeviceId, ((nStatus == DI_SC_NOT_PRESENT)?"Removed":"Inserted"));
	if ( CMD_GetMsgUI() != NULL )
	{
		VK_sprintf(parser_sc_Str,"Smartcard(%u) is %s\n", unDeviceId, ((nStatus == DI_SC_NOT_PRESENT)?"Removed":"Inserted"));
		(int *)(CMD_GetMsgUI())(parser_sc_Str);
   		VK_MEM_Memset(parser_sc_Str, 0x00, 500);
	}
    if ( fnSmartCardCallback != 0 )
    {
        (int *)(fnSmartCardCallback)(unDeviceId, nStatus);
    }

	if(nStatus == DI_SC_NOT_PRESENT)
	{
		lastAtrLen = 0;
		VK_MEM_Memset(ucHwTestAtr, 0x00, 34);
	}

	if(nStatus == DI_SC_NOT_PRESENT)
		ucScExist = 0;
	else
		ucScExist = 1;
	return;
}

static void P_HwTest_SC_DebugParsedAtr(hwtest_scAtrInfo_t *pstAtrInfo)
{
	CMD_Printf("____ SC PARSED ATR INFO ____\n");

	if (pstAtrInfo == NULL)
		return;

	if (pstAtrInfo->bSpecificMode)
		CMD_Printf("specific mode\n");
	else
		CMD_Printf("negotiable mode\n");

	if (pstAtrInfo->bSpecificMode)
	{
		if(pstAtrInfo->bPpsUnable)
			CMD_Printf("PPS Request unable\n");
		else
			CMD_Printf("PPS Request available\n");
	}

	if (pstAtrInfo->bSpecificMode)
	{

		if (pstAtrInfo->bSpecificMode && pstAtrInfo->bImplicitlyDefined)
			CMD_Printf("Parameter defined implicitly\n");
		else
			CMD_Printf("Parameter defined explicitly. Use FI, DI\n");
	}

	CMD_Printf("Historical bytes number (%d)\n", pstAtrInfo->ucK);

	CMD_Printf("T first (%d), T (%d)\n", pstAtrInfo->ucTFirst, pstAtrInfo->ucT);

	CMD_Printf("FI (%d), DI (%d), N(%d), WI (%d)\n", pstAtrInfo->ucFI, pstAtrInfo->ucDI, pstAtrInfo->ucN, pstAtrInfo->ucWI);

	if (pstAtrInfo->ucT == 1)
		CMD_Printf("IFSC (%d), CWI (%d), BWI(%d)\n", pstAtrInfo->ucIFSC, pstAtrInfo->ucCWI, pstAtrInfo->ucBWI);

	CMD_Printf("II (%d), PI1 (%d), PI2 (%d)\n", pstAtrInfo->ucII, pstAtrInfo->ucPI1, pstAtrInfo->ucPI2);

	CMD_Printf("\n________________________________\n");

	if ( CMD_GetMsgUI() != NULL )
	{
		VK_sprintf(parser_sc_Str,"FI (%d), DI (%d), N(%d), WI (%d)\n", pstAtrInfo->ucFI, pstAtrInfo->ucDI, pstAtrInfo->ucN, pstAtrInfo->ucWI);
		(int *)(CMD_GetMsgUI())(parser_sc_Str);
   		VK_MEM_Memset(parser_sc_Str, 0x00, 500);
		VK_sprintf(parser_sc_Str,"T first (%d), T (%d)\n", pstAtrInfo->ucTFirst, pstAtrInfo->ucT);
		(int *)(CMD_GetMsgUI())(parser_sc_Str);
   		VK_MEM_Memset(parser_sc_Str, 0x00, 500);
		if (pstAtrInfo->bSpecificMode)
			VK_sprintf(parser_sc_Str,"specific mode, ");
		else
			VK_sprintf(parser_sc_Str,"negotiable mode, ");
		VK_sprintf(parser_sc_Str,"%s Historical bytes (%d)\n",parser_sc_Str, pstAtrInfo->ucK);
		(int *)(CMD_GetMsgUI())(parser_sc_Str);
   		VK_MEM_Memset(parser_sc_Str, 0x00, 500);
		VK_sprintf(parser_sc_Str,"____ SC PARSED ATR INFO ____\n");
		(int *)(CMD_GetMsgUI())(parser_sc_Str);
   		VK_MEM_Memset(parser_sc_Str, 0x00, 500);
	}


}


static int P_HwTest_Sc_ParseATR(HUINT8 *pucAtr, hwtest_scAtrInfo_t *pstAtrInfo)
{
	HUINT8 *pucData;
	HUINT8 ucRawAtr;
	HUINT8 ucIndex = 1;

	if ( (pucAtr == NULL) || (pstAtrInfo == NULL) )
	{
		CMD_Printf("[P_HwTest_Sc_ParseATR] targets are null ...\n");
		return 1;
	}

	if( (pucAtr[0] !=0x3B) && (pucAtr[0] !=0x3F) )
	{
		CMD_Printf("[P_HwTest_Sc_ParseATR] Invalid ATR (TS=0x%02X \n", pucAtr[0] );
		if ( CMD_GetMsgUI() != NULL )
		{
			VK_sprintf(parser_sc_Str,"Invalid ATR (TS=0x%02X) \n", pucAtr[0] );
			(int *)(CMD_GetMsgUI())(parser_sc_Str);
	   		VK_MEM_Memset(parser_sc_Str, 0x00, 500);
		}
		return 1;
	}

	pucData = pucAtr;
	ucRawAtr = *++pucData;
	pstAtrInfo->ucK = *pucData & 0x0F;

	while (ucRawAtr)
	{
		if (ucRawAtr & 0x10)
		{
			if (ucIndex == 1)
			{
				pstAtrInfo->ucFI = (*++pucData >> 4) & 0x0F;
				pstAtrInfo->ucDI = *pucData & 0x0F;
			}
			else if (ucIndex == 2)
			{
				pstAtrInfo->bSpecificMode = TRUE;
				pstAtrInfo->bPpsUnable = ((*++pucData & 0x80) == 0x80);
				pstAtrInfo->bImplicitlyDefined = ((*pucData & 0x10) == 0x10);
			}
			else if (ucIndex > 2 && pstAtrInfo->ucT == 1)
			{
				pstAtrInfo->ucIFSC = *++pucData;
			}
		}
		if (ucRawAtr & 0x20)
		{
			if (ucIndex == 1)
			{
				pstAtrInfo->ucII = (*++pucData >> 5) & 0x07;
				pstAtrInfo->ucPI1 = *pucData & 0x1F;
			}
			else if (ucIndex == 2)
			{
				pstAtrInfo->ucPI2 = *++pucData;
			}
			else if (ucIndex > 2 && pstAtrInfo->ucT == 1)
			{
				pstAtrInfo->ucCWI = *++pucData & 0x0F;
				pstAtrInfo->ucBWI = (*pucData >> 4) & 0x0F;
			}
		}
		if (ucRawAtr & 0x40)
		{
			if (ucIndex == 1)
			{
				pstAtrInfo->ucN = *++pucData;
			}
			else if (ucIndex == 2)
			{
				pstAtrInfo->ucWI = *++pucData;
			}
		}
		if (ucRawAtr & 0x80)
		{
			if (ucIndex == 1)
			{
				pstAtrInfo->ucTFirst = *++pucData & 0x0F;
				pstAtrInfo->ucT = pstAtrInfo->ucTFirst;
			}
			else
			{
				pstAtrInfo->ucT = *++pucData & 0x0F;
			}
			ucRawAtr = *pucData;
		}
		else
		{
			ucRawAtr = 0;
		}
		ucIndex++;
	}

	P_HwTest_SC_DebugParsedAtr(pstAtrInfo);

	return ERR_OK;

}

int P_HwTest_SC_Reset(
						HUINT32 ulSlotId,
						HUINT8 *pucAtr,
						HUINT32 *pulLen,
						hwtest_SCM_ResetType_t eResetType,
						hwtest_SCI_SC_PARAM *pstDefaultParam
						)
{
	DI_ERR_CODE nResult = 0;
	DI_SC_STATUS eStatus;
	HUINT8	ucXOR = 0;
	HUINT32 ulCnt;

	nResult = DI_SC_GetCardStatus(ulSlotId, &eStatus);
	if(nResult != DI_ERR_OK || eStatus != DI_SC_PRESENT)
	{
		return 1;
	}

	nResult = DI_SC_SetParams(ulSlotId, pstDefaultParam->eProtocol, pstDefaultParam->ulClock,
							pstDefaultParam->ulBaudrate, 	pstDefaultParam->ucFI, pstDefaultParam->ucDI,
							pstDefaultParam->ucN, pstDefaultParam->ucCWI, pstDefaultParam->ucBWI );
	if(nResult != DI_ERR_OK)
	{
		VK_printf("ERROR:  set param \n");
		return ERR_FAIL;
	}

	if(eResetType == eSCM_Reset_Cold)
	{
		nResult = DI_SC_DownUpVCC(ulSlotId, 20);
		if(nResult != DI_ERR_OK)
		{
			VK_printf("ERROR: down up vcc. \n");
			return 1;
		}
	}

	nResult = DI_SC_Reset(ulSlotId, pucAtr, pulLen);
	if(nResult != DI_ERR_OK)
	{
		VK_printf("ERROR: reset \n");
		return 1;
	}

	for(ulCnt=1;ulCnt< *pulLen;ulCnt++)
	{
		ucXOR ^= pucAtr[ulCnt];
	}
	VK_printf("Check result = 0x%02x\n", ucXOR);

	if(lastAtrLen == 0)
	{
		lastAtrLen = *pulLen;
		VK_memcpy( lastAtr, pucAtr, lastAtrLen );
	}
	else
	{
		if(lastAtrLen != pulLen)
		{
			VK_sprintf(parser_sc_Str,"ERROR :: last length (%u) != current length(%u)\n",lastAtrLen, pulLen);
			(int *)(CMD_GetTimeoutInfoUI())(parser_sc_Str,VK_strlen(parser_sc_Str),5*60);
			VK_MEM_Memset(parser_sc_Str, 0x00, 500);
		}
		else
		{
			for(ulCnt=0;ulCnt< *pulLen;ulCnt++)
			{
				if(lastAtr[ulCnt] != pucAtr[ulCnt])
				{
					VK_sprintf(parser_sc_Str,"ERROR :: %d [ %x : %x ]",ulCnt,lastAtr[ulCnt] , pucAtr[ulCnt]);

					(int *)(CMD_GetTimeoutInfoUI())(parser_sc_Str,VK_strlen(parser_sc_Str),5*60);
					VK_MEM_Memset(parser_sc_Str, 0x00, 500);
				}
			}

		}
	}

	P_HwTest_Sc_ParseATR(pucAtr, &gScAtrInfo);
	return 0;
}

int P_hwtest_SC_GetSCInfo(HUINT32 deviceId)
{
	HUINT32 ulAtrLen;

	P_HwTest_SC_Reset(deviceId, ucHwTestAtr, &ulAtrLen, eSCM_Reset_Cold,  &s_stDefaultAtrParam);

	return 0;
}

#endif

int CMD_HwTest_Parser_SC(void *szArgs)
{
//	int iResult=CMD_ERR;
	DI_ERR_CODE errorCode = DI_ERR_ERROR;
	HUINT32 atrlen;
	HUINT32 unDeviceId=0;
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)  /*Used only in HW_TEST mode*/
	HUINT32 ulCnt;
	hwtest_scAtrInfo_t stAtrInfo;
#endif
	GET_ARGS;

	UNUSED(szParam1);
	UNUSED(szParam2);

	if CMD_IS("atr0")
	{
		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)  /*Used only in HW_TEST mode*/
		if ( CMD_GetMsgUI() != NULL )
		{
			VK_sprintf(parser_sc_Str,"Getting ATR(T0) for SC(%d) \n", unDeviceId );
			(int *)(CMD_GetMsgUI())(parser_sc_Str);
	   		VK_MEM_Memset(parser_sc_Str, 0x00, 500);
		}
#endif
		errorCode = DI_SC_SetParams( unDeviceId, 0,
						0, 0,
						1, 1, 0,
						13, 4);

		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_SetParams \r\n");
			return iResult;
		}

		errorCode = DI_SC_Reset (unDeviceId, atrbuf, &atrlen);
		if (errorCode != DI_ERR_OK)
		{
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)  /*Used only in HW_TEST mode*/
			VK_sprintf(parser_sc_Str,"ERROR :: Smart Card Reset Fail #");
			(int *)(CMD_GetTimeoutInfoUI())(parser_sc_Str,32,5*60);
			VK_MEM_Memset(parser_sc_Str, 0x00, 500);
#endif


			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_Reset \r\n");
			return iResult;
		}
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)  /*Used only in HW_TEST mode*/
		VK_printf("call atr0\r\n");

		if(lastAtrLen == 0)
		{
			lastAtrLen = atrlen;
			VK_memcpy( lastAtr, atrbuf, lastAtrLen );
		}
		else
		{
			if(lastAtrLen != atrlen)
			{
				VK_sprintf(parser_sc_Str,"ERROR :: last length (%d) != current length(%d)#",lastAtrLen, atrlen);
				VK_printf("%s : length = %d\n",parser_sc_Str, VK_strlen(parser_sc_Str));
				(int *)(CMD_GetTimeoutInfoUI())(parser_sc_Str,VK_strlen(parser_sc_Str),5*60);
				VK_MEM_Memset(parser_sc_Str, 0x00, 500);
			}
			else
			{
				for(ulCnt=0;ulCnt< atrlen;ulCnt++)
				{
					if(lastAtr[ulCnt] != atrbuf[ulCnt])
					{
						VK_sprintf(parser_sc_Str,"ERROR :: last length (%d)  current length(%d)\n\r# %dth  [%x] != [%x] # "
							,lastAtrLen, atrlen,ulCnt ,lastAtr[ulCnt] ,atrbuf[ulCnt]);

						(int *)(CMD_GetTimeoutInfoUI())(parser_sc_Str,VK_strlen(parser_sc_Str),5*60);
						VK_MEM_Memset(parser_sc_Str, 0x00, 500);
						break;
					}
				}
			}
		}

		P_HwTest_Sc_ParseATR(atrbuf, &stAtrInfo);
#endif
		iResult = CMD_OK;
	}
	else 	if CMD_IS("atr14")
	{
		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)  /*Used only in HW_TEST mode*/
		if ( CMD_GetMsgUI() != NULL )
		{
			VK_sprintf(parser_sc_Str,"Getting ATR(T14) for SC(%d) \n", unDeviceId );
			(int *)(CMD_GetMsgUI())(parser_sc_Str);
	   		VK_MEM_Memset(parser_sc_Str, 0x00, 500);
		}
#endif

		errorCode = DI_SC_SetParams( unDeviceId, 14,
						3000, 0,
						2, 1, 0,
						13, 5);

		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_SetParams \r\n");
			return iResult;
		}

		errorCode = DI_SC_Reset (unDeviceId, atrbuf, &atrlen);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_Reset \r\n");
			return iResult;
		}
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)  /*Used only in HW_TEST mode*/
		VK_printf("call atr14\r\n");

		P_HwTest_Sc_ParseATR(atrbuf, &stAtrInfo);
#endif
		iResult = CMD_OK;
	}
	else 	if CMD_IS("coldreset0")	// only T0 T1
	{
		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}

//		DI_SC_Deactivate( unDeviceId );
		DI_SC_DownUpVCC(unDeviceId, 20);

		errorCode = DI_SC_SetParams( unDeviceId, 0,
						0, 0,
						1, 1, 0,
						13, 4);

		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_SetParams \r\n");
			return iResult;
		}

		errorCode = DI_SC_Reset (unDeviceId, atrbuf, &atrlen);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_Reset \r\n");
			return iResult;
		}
		iResult = CMD_OK;
	}
	else	if CMD_IS("coldreset14")
	{
		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}

//		DI_SC_Deactivate( unDeviceId );
		DI_SC_DownUpVCC(unDeviceId, 20);

		errorCode = DI_SC_SetParams( unDeviceId, 14,
						6000, 0,
						2, 1, 0,
						13, 5);

		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_SetParams \r\n");
			return iResult;
		}

		errorCode = DI_SC_Reset (unDeviceId, atrbuf, &atrlen);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_Reset \r\n");
			return iResult;
		}
		iResult = CMD_OK;
	}
	else 	if CMD_IS("trans1")
	{
		unsigned char aucWriteBuf[256];
		unsigned char aucReadBuf[256] ; //= {0x00, 0xe1, 0x01, 0xfe, 0x1e};
		int i, ucReadNum = 0;

		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}

		aucWriteBuf[0] = 0x00;
		aucWriteBuf[1] = 0xC1;
		aucWriteBuf[2] = 0x01;
		aucWriteBuf[3] = 0xFE;
		aucWriteBuf[4] = 0x3E;


#if 0
//nagra other etu smartcard test
//Protocol(1), MinClock(0), SrcBaudrate(0), FI(9), DI(5), N(255), CWI(7), BWI(4)	//etu32
//		errorCode = DI_SC_SetParams( unDeviceId, 1,0, 0, 9, 5, 255, 7, 4);

//Protocol(1), MinClock(0), SrcBaudrate(0), FI(10), DI(6), N(255), CWI(7), BWI(4)	//etu24
//		errorCode = DI_SC_SetParams( unDeviceId, 1,0, 0, 10, 6, 255, 7, 4);

//Protocol(1), MinClock(0), SrcBaudrate(0), FI(9), DI(6), N(255), CWI(7), BWI(4)	//etu16
//		errorCode = DI_SC_SetParams( unDeviceId, 1,0, 0, 9, 6, 255, 7, 4);

#else
//arib
		errorCode = DI_SC_SetParams( unDeviceId, 1,
						0, 21504,
						1, 2, 0,
						5, 4);
#endif
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_SetParams \r\n");
			return iResult;
		}

		ucReadNum = 0;
		errorCode = DI_SC_TransferData(unDeviceId, aucWriteBuf, 5, aucReadBuf, (unsigned int *)&ucReadNum);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_TransferData \r\n");
			return iResult;
		}
		VK_printf("Read Data : \r\n");
		for( i = 0; i < ucReadNum;i++ )
		{
			VK_printf("%02x ", aucReadBuf[i]);
		}
		VK_printf("\r\n");

		iResult = CMD_OK;

	}
	else 	if CMD_IS("trans0")
	{
		unsigned char aucWriteBuf[256];
		unsigned char aucReadBuf[256] ; //= {0x00, 0xe1, 0x01, 0xfe, 0x1e};
		int i, ucReadNum = 0;

		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}

		aucWriteBuf[0] = 0xd2;
		aucWriteBuf[1] = 0x42;
		aucWriteBuf[2] = 0x00;
		aucWriteBuf[3] = 0x00;
		aucWriteBuf[4] = 0x01;
		aucWriteBuf[5] = 0x1d;

#if 0
		errorCode = DI_SC_SetParams( unDeviceId, 0,
						0, 0,
						1, 1, 0,
						0, 0);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_SetParams \r\n");
			return iResult;
		}
#endif
		ucReadNum = 0;
		errorCode = DI_SC_TransferData(unDeviceId, aucWriteBuf, 6, aucReadBuf, (unsigned int *)&ucReadNum);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_TransferData \r\n");
			return iResult;
		}
		VK_printf("Read Data : \r\n");
		for( i = 0; i < ucReadNum;i++ )
		{
			VK_printf("%02x ", aucReadBuf[i]);
		}
		VK_printf("\r\n");

		iResult = CMD_OK;


	}
	else	if CMD_IS("transna14")
	{
		unsigned char aucWriteBuf[256];
		unsigned char aucReadBuf[256] ; //17bytes
		//01 00 90 00 00 00 00 08 95 06 1F 5D 5D 60 13 5D 04
		int i, ucReadNum = 0;

		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}
		//01 A0 CA 00 00 03 15 00 08 4A
		aucWriteBuf[0] = 0x01;
		aucWriteBuf[1] = 0xa0;
		aucWriteBuf[2] = 0xca;
		aucWriteBuf[3] = 0x00;
		aucWriteBuf[4] = 0x00;
		aucWriteBuf[5] = 0x03;
		aucWriteBuf[6] = 0x15;
		aucWriteBuf[7] = 0x00;
		aucWriteBuf[8] = 0x08;
		aucWriteBuf[9] = 0x4a;

#if 1
		errorCode = DI_SC_SetParams( unDeviceId, 14,
						3000, 0,
						2, 1, 0,
						13, 5);

		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_SetParams \r\n");
			return iResult;
		}
#endif
		ucReadNum = 0;
		errorCode = DI_SC_TransferData(unDeviceId, aucWriteBuf, 10, aucReadBuf, (unsigned int *)&ucReadNum);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_TransferData \r\n");
			return iResult;
		}
		VK_printf("Read Data : \r\n");
		for( i = 0; i < ucReadNum;i++ )
		{
			VK_printf("%02x ", aucReadBuf[i]);
		}
		VK_printf("\r\n");

		iResult = CMD_OK;


	}
	else	if CMD_IS("trans14")
	{
		unsigned char aucWriteBuf[256];
		unsigned char aucReadBuf[256] ; //29bytes
		//01 02 00 00 00 00 00 14 34 30 31 36 33 37 32 35 39 33 54 36 32 32 30 34 41 00 00 00 05
		int i, ucReadNum = 0;

		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}
		// 01 02 00 00 00 00 3C
		aucWriteBuf[0] = 0x01;
		aucWriteBuf[1] = 0x02;
		aucWriteBuf[2] = 0x00;
		aucWriteBuf[3] = 0x00;
		aucWriteBuf[4] = 0x00;
		aucWriteBuf[5] = 0x00;
		aucWriteBuf[6] = 0x3c;

#if 1
		errorCode = DI_SC_SetParams( unDeviceId, 14,
						3000, 0,
						2, 1, 0,
						13, 5);

		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_SetParams \r\n");
			return iResult;
		}
#endif
		ucReadNum = 0;
		errorCode = DI_SC_TransferData(unDeviceId, aucWriteBuf, 7, aucReadBuf, (unsigned int *)&ucReadNum);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_TransferData \r\n");
			return iResult;
		}
		VK_printf("Read Data : \r\n");
		for( i = 0; i < ucReadNum;i++ )
		{
			VK_printf("%02x ", aucReadBuf[i]);
		}
		VK_printf("\r\n");

		iResult = CMD_OK;


	}
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)  /*Used only in HW_TEST mode*/
	else if CMD_IS("parseatr")
	{
		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}
		VK_printf("call parseatr\r\n");

		P_hwtest_SC_GetSCInfo(unDeviceId);

		iResult = CMD_OK;
	}
	else if CMD_IS("hwtest_regcb")
	{
		HUINT32 iIndex;
		HUINT32 iMax;

		DI_SC_GetCapability(&iMax);
		for (iIndex=0 ; iIndex<iMax ; iIndex++)
			DI_SC_RegisterCallback(iIndex, P_HwTest_SC_Callback);
		iResult = CMD_OK;
	}
#endif
	else if CMD_IS("status")
	{
		DI_SC_STATUS eStatus = DI_SC_NOT_PRESENT;
		HUINT32 ulDeviceId = 0;

		if PARAM_IS("0")
		{
			ulDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			ulDeviceId = 1;
		}

		errorCode = DI_SC_GetCardStatus(ulDeviceId, &eStatus);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_SC] ERROR: DI_SC_GetCardStatus \r\n");
			return iResult;
		}

		VK_printf("Smartcard(%u) : %s\n", ulDeviceId, ((eStatus == DI_SC_PRESENT)?"PRESENT":"NOT_PRESENT"));

		iResult = CMD_OK;
	}
	else
	{
		VK_printf("ERR! Bad Comman Input. \n");
	}

	return iResult;
}


