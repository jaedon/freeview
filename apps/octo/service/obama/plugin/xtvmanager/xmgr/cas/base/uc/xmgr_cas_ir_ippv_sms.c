/********************************************************************
 Header Files
 ********************************************************************/
#include <stdio.h>
#include <bus.h>
//#include <vd_humax.h>
//#include <ir_svc.h>
#include <svc_si.h>
#include <svc_epg.h>
#include <util.h>
#include <mgr_action.h>

#include <s3_ippv_msg.h>
#include <s3_lppv_msg.h>
#include <s3_event_if_msg.h>

#include "local_include/_xmgr_cas_ir_ppv.h"

/********************************************************************
 Macro Definition
 ********************************************************************/




/*******************************************************************/
/******************      Definitions         ***********************/
/*******************************************************************/
#define BASE_35		35

#define MAX_TIME_VALUE			72
#define MINUTES_PER_HOUR		60
#define DIVIDE_20_MINUTES		20

#define CHANNEL_200				200
#define CHANNEL_1022			1022
#define CHANNEL_2				2

#define DYNAMIC_PART_MASK		0x000FFFFF
#define EVENT_DATE_MASK			0x000003FE
#define EVENT_DATA_SHIFT		1

#define CHANNEL_NUMBER_MASK		0x000FFC00
#define CHANNEL_NUMBER_SHIFT	10

#define PAYMENT_TYPE_MASK		0x00000001
#define FIRST_IRDETO_CARD_ID	(4023096199)
#define FIRST_CRYPTO_CARD_ID	(55840866560)

#define CHECKSUM_MASK			0x000001FF
#define CHECKSUM_CARDTYPE_MASK	0x00000200
#define RECOGNITION_CHARACTER	'T'


/*******************************************************************/
/*******************      Static Varaibles      ********************/
/*******************************************************************/
const STATIC HUINT8  baseDigits[35] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R'
									, 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};



/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
STATIC HUINT64 local_cas_ir_sms_power(HUINT64 ulX, HUINT64 ulN)
{
	HUINT64 i, ulResult = ulX;

	if(ulN == 0)
		return 1;

	for(i = 0; i < ulN - 1; i ++)
	{
		ulResult *= ulX;
	}

	return ulResult;
}

STATIC HUINT8 local_cas_ir_sms_ConvertAsciiCh2Decimal(HUINT8 parsedCh)
{
	if((parsedCh >= 0x30) && (parsedCh <= 0x39))
	{
		return parsedCh - 0x30;
	}
	else
	{
		return 0xff;
	}
}

STATIC HERROR local_cas_ir_sms_ConvertAsciiStr2Decimal(HUINT8 *pszAscii, HUINT64 *pulDec)
{
	HUINT64 ulDec, i, ulLen, ulTmpDec = 0;

	if((pszAscii == NULL) || (pulDec == NULL))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	ulLen = MWC_UTIL_DvbStrlenStrOnly(pszAscii);

	for(i = 0; i < ulLen ; i ++)
	{
		ulDec = local_cas_ir_sms_ConvertAsciiCh2Decimal(pszAscii[ulLen - 1 - i]);

		if(ulDec == 0xFF)
		{
			return ERR_FAIL;
		}

		ulTmpDec += (ulDec * local_cas_ir_sms_power(10, i));
	}

	*pulDec = ulTmpDec;

	return ERR_OK;
}

STATIC HERROR local_cas_ir_sms_ConvertDecimal2Base35(HUINT32 ulDecimal, HUINT8 *pucBase35, HUINT32 *pulBaseLen)
{
	HUINT32 ulTempDec, pulRem[50], i, ulLength;
	HUINT8	*pszTempBase35;

	if((pucBase35 == NULL) || (pulBaseLen == NULL))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	VK_MEM_Memset(pulRem, 0, sizeof(pulRem));

	ulTempDec = ulDecimal;
	i = 0;
	ulLength = 0;

	if(ulTempDec == 0)
	{
		ulLength = 1;
		pulRem[0] = 0;
	}
	else
	{
		while(ulTempDec > 0)
		{
			pulRem[i] = ulTempDec % BASE_35;
			ulTempDec = ulTempDec / BASE_35;
			i ++;
			ulLength ++;
		}
	}

	pszTempBase35 = (HUINT8 *)HLIB_STD_MemAlloc(ulLength);
	if(pszTempBase35 == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	VK_MEM_Memset(pszTempBase35, 0, ulLength);

	for(i = 0; i < ulLength ; i ++)
	{
		pszTempBase35[ulLength - 1 - i] = baseDigits[pulRem[i]];
	}

	*pulBaseLen = ulLength;
	VK_MEM_Memcpy(pucBase35, pszTempBase35, ulLength);
	pucBase35[ulLength] = '\0';

	return ERR_OK;
}

STATIC HERROR local_cas_ir_sms_ConvertBase352Decimal(HUINT8 *pucBase35, HUINT32 ulBaseLen, HUINT32 *pulDecimal)
{
	HUINT32 ulDigitValue = 0, ulTempNum = 0, i, j;

	if(pulDecimal == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for(i = 0; i < ulBaseLen; i ++)
	{
		j = 0;
		do
		{
			if(pucBase35[i] >= 'a' && pucBase35[i] <= 'z')
			{
				pucBase35[i] -= 'a' - 'A';
			}

			if(pucBase35[i] == 'O')
			{
				pucBase35[i] = '0';
			}

			if(baseDigits[j] == pucBase35[i])
			{
				ulDigitValue = j;
				break;
			}
			j ++;
		}while(j < BASE_35);

		if(j >= BASE_35)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		ulTempNum += (ulDigitValue * (HUINT32)local_cas_ir_sms_power(BASE_35, i));
	}

	*pulDecimal = ulTempNum;

	return ERR_OK;
}

STATIC HERROR local_cas_ir_sms_MakeDynamicPart(XmgrCas_IrIppvEitInfo_t *pstEITInfo, HUINT32 ulChannelNo, HUINT8 ucPaymentType, HUINT8 *pucDynamicPart, HUINT32 *pulDynamicPartLen)
{
	HERROR	hErr;
	HUINT8	ucWeekDay, ucTempWeekDay;
	HxDATETIME_Time_t stTime;
	HUINT32 ulTempDynamicPart;
	HUINT32	ulTimeValue, ulDayValue, ulEventDate, ulChRelNum;

	if((pstEITInfo == NULL) || (pucDynamicPart == NULL) || (pulDynamicPartLen == NULL))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = HLIB_DATETIME_ConvertUnixTimeToTime(pstEITInfo->startTime, &stTime);
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
	hErr = HLIB_DATETIME_ConvertUnixTimeToWeekday(pstEITInfo->startTime, &ucTempWeekDay);
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	if(ucTempWeekDay == 0)
	{
		ucWeekDay = 6;
	}
	else
	{
		ucWeekDay = ucTempWeekDay - 1;
	}

	ulTimeValue = ((stTime.ucHour * MINUTES_PER_HOUR) + stTime.ucMinute) / DIVIDE_20_MINUTES;
	ulDayValue = ucWeekDay * MAX_TIME_VALUE;
	ulEventDate = ulDayValue + ulTimeValue;

	ulChRelNum = ((ulChannelNo - CHANNEL_200 + CHANNEL_1022)%CHANNEL_1022) + 10;

	ulTempDynamicPart = DYNAMIC_PART_MASK & 												\
						((CHANNEL_NUMBER_MASK & (ulChRelNum << CHANNEL_NUMBER_SHIFT)) |		\
						 (EVENT_DATE_MASK & (ulEventDate << EVENT_DATA_SHIFT)) |			\
						 (PAYMENT_TYPE_MASK & ucPaymentType));


	hErr = local_cas_ir_sms_ConvertDecimal2Base35(ulTempDynamicPart, pucDynamicPart, pulDynamicPartLen);

	HxLOG_Print("WeekDay[%d] Time[%02d:%02d] CH[%d] Ptype[%d], DymamicPart[0x%08x][%s]\n",
								ucWeekDay, stTime.ucHour, stTime.ucMinute, ulChannelNo, ucPaymentType,
								*pulDynamicPartLen, pucDynamicPart));

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	return ERR_OK;
}

STATIC HERROR local_cas_ir_sms_DoFirstMixing(HUINT8 *pszSCIDPart, HUINT32 ulSCIDPartLen, HUINT8 *pucDynamicPart, HUINT32 ulDynmicPartLen, HUINT8 *pucResult)
{
	HERROR	hErr;
	HUINT8	pucTempMixed[10];
	HUINT32	i, ulTemp, ulSum = 0, ulBaseLen, pulDigitValue[10], pulSum[10], pulMixed[10];

	if((pucDynamicPart == NULL) || (pucResult == NULL))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	ulSum = 0;
	for(i = 0; i < ulDynmicPartLen; i ++)
	{
		pulDigitValue[i] = 0;
		hErr = local_cas_ir_sms_ConvertBase352Decimal(&pucDynamicPart[i], 1, &pulDigitValue[i]);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		ulSum += pulDigitValue[i];
	}

	for(i = 0; i < ulDynmicPartLen; i ++)
	{
		pulSum[i] = ((pulDigitValue[i] + ulSum) * (pulDigitValue[i] + 1)) % BASE_35;
	}

	for(i = 0; i < ulSCIDPartLen ; i ++)
	{
		ulTemp = 0;

		hErr = local_cas_ir_sms_ConvertBase352Decimal(&pszSCIDPart[i], 1, &ulTemp);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		pulMixed[i] = (ulTemp + pulSum[i%ulDynmicPartLen]) % BASE_35;

		hErr = local_cas_ir_sms_ConvertDecimal2Base35(pulMixed[i], &pucTempMixed[i], &ulBaseLen);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
	}

	VK_MEM_Memcpy(pucResult, pucTempMixed, ulSCIDPartLen);
	pucResult[ulSCIDPartLen] = '\0';

	HxLOG_Print("Result First Mixing[%s], Len[%d]\n", pucResult, ulSCIDPartLen);

	return ERR_OK;
}

STATIC HERROR local_cas_ir_sms_DoSecondMixing(HUINT8 *pucSCIDPart, HUINT32 ulSCIDPartLen, HUINT8 *pucDynamicPart, HUINT32 ulDynmicPartLen, HUINT8 *pucSCIDResult, HUINT8 *pucDynamicResult)
{
	HERROR	hErr;
	HUINT8	pucTempMixedDynamic[10], pucTempMixedSCID[10];
	HUINT32	i, ulTemp, ulSum = 0, ulBaseLen, pulDigitValue[10], pulSum[10], pulMixedDynamic[10], pulMixedSCID[10];

	if((pucDynamicPart == NULL) || (pucSCIDResult == NULL) || (pucDynamicResult == NULL))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	ulSum = 0;
	for(i = 0; i < ulSCIDPartLen; i ++)
	{
		pulDigitValue[i] = 0;
		hErr = local_cas_ir_sms_ConvertBase352Decimal(&pucSCIDPart[i], 1, &pulDigitValue[i]);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		ulSum += pulDigitValue[i];
	}

	for(i = 0; i < ulSCIDPartLen; i ++)
	{
		pulSum[i] = ((pulDigitValue[i] + ulSum) * (pulDigitValue[i] + 1)) % BASE_35;
	}

	for(i = 0; i < ulDynmicPartLen ; i ++)
	{
		ulTemp = 0;

		hErr = local_cas_ir_sms_ConvertBase352Decimal(&pucDynamicPart[i], 1, &ulTemp);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		pulMixedDynamic[i] = (ulTemp + pulSum[(ulSCIDPartLen - 1) - i%ulSCIDPartLen]) % BASE_35;

		hErr = local_cas_ir_sms_ConvertDecimal2Base35(pulMixedDynamic[i], &pucTempMixedDynamic[i], &ulBaseLen);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
	}

	for(i = 0; i < ulDynmicPartLen; i ++)
	{
		pulSum[i] = (pulMixedDynamic[i] % (BASE_35 - 1)) + 1;
	}

	for(i = 0; i < ulSCIDPartLen ; i ++)
	{
		ulTemp = 0;

		hErr = local_cas_ir_sms_ConvertBase352Decimal(&pucSCIDPart[i], 1, &ulTemp);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		pulMixedSCID[i] = (ulTemp + pulSum[i%ulDynmicPartLen]) % BASE_35;
		hErr = local_cas_ir_sms_ConvertDecimal2Base35(pulMixedSCID[i], &pucTempMixedSCID[i], &ulBaseLen);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
	}

	VK_MEM_Memcpy(pucDynamicResult, pucTempMixedDynamic, ulDynmicPartLen);
	pucDynamicResult[ulDynmicPartLen] = '\0';

	VK_MEM_Memcpy(pucSCIDResult, pucTempMixedSCID, ulSCIDPartLen);
	pucSCIDResult[ulSCIDPartLen] = '\0';

	HxLOG_Print("Result Second Mixing[%s-%s], Len[%d-%d]\n", pucSCIDResult, pucDynamicResult, ulSCIDPartLen, ulDynmicPartLen);

	return ERR_OK;
}
STATIC HUINT32	local_cas_ir_sms_CalculateCheckSum(HUINT8 *pszStr)
{
	HUINT32	ulSensitivity = 477;
	HINT32	length = strlen((const char*)pszStr);
	HUINT32	ulResultValue = pszStr[length - 1];
	HINT32	i = 0, ulValue;

	if(pszStr == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for(i = 0; i < length; i ++)
	{
		ulValue = ((i + 1)*(i + 2)) + (HUINT32)pszStr[length - 1];
		ulResultValue = (((HUINT32) pszStr[i]) * ulValue)^ulResultValue;
	}

	return (ulResultValue % ulSensitivity) + 35;
}

STATIC HERROR local_cas_ir_sms_MakeCheckSumPart(HUINT32 ulCheckSum, HUINT8 ucCardType, HUINT8 *pucCheckSumPart, HUINT32 *pulCheckSumLen)
{
	HERROR	hErr;
	HUINT32 ulCheckSumResult;

	if((pucCheckSumPart == NULL) || (pulCheckSumLen == NULL))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(ucCardType == 1/*IRDETO*/)
	{
		ulCheckSumResult = ((CHECKSUM_MASK & ulCheckSum) | CHECKSUM_CARDTYPE_MASK);
	}
	else
	{
		ulCheckSumResult = CHECKSUM_MASK & ulCheckSum;
	}

	hErr = local_cas_ir_sms_ConvertDecimal2Base35(ulCheckSumResult, pucCheckSumPart, pulCheckSumLen);

	HxLOG_Print("CheckSum[%d] CardType[%d], CheckSum Str[%s], Len[%d]\n", ulCheckSum, ucCardType, pucCheckSumPart, *pulCheckSumLen);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	return ERR_OK;
}

STATIC HERROR local_cas_ir_sms_GetShorteningSCID(HUINT8 *pszSCNumber, HUINT8 ucCardType,HUINT8 *pucShorteningSCID, HUINT32 *pulBaseLen)
{
	HERROR	hErr;
	HUINT64	ulSCNum;

	hErr = local_cas_ir_sms_ConvertAsciiStr2Decimal(pszSCNumber,&ulSCNum);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	if(ucCardType)
	{
		ulSCNum = ulSCNum - (HUINT64)FIRST_IRDETO_CARD_ID;
	}
	else
	{
		ulSCNum = ulSCNum - (HUINT64)FIRST_CRYPTO_CARD_ID;
	}

	hErr = local_cas_ir_sms_ConvertDecimal2Base35((HUINT32)ulSCNum, pucShorteningSCID, pulBaseLen);

	HxLOG_Print("Card No[%s], Shortening SCID[%s], Len[%d]\n", pszSCNumber, pucShorteningSCID, *pulBaseLen);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	return ERR_OK;
}



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR XMGR_CAS_IR_SMS_GetSMSCode(XmgrCas_IrIppvEitInfo_t *pstEITInfo, HUINT32 ulChannelNo, HUINT8 ucPaymentType,
										HUINT8 *pucSMSCode, HUINT32 *pulSMSCodeLen)
{
	HERROR			hErr;
	HUINT8			pucDynamicPart[10], pszSCNumber[IR_SC_INFO_NUMBER_LENGTH], pszShorteningSCID[10];
	HUINT8			pucSCIDPartFirstMixed[10], pucSCIDPartSecondMixed[10], pucDynamicPartMixed[10], pucCheckSumPart[10], pucTmp[20], pucFinelSMS[20];
	HUINT32			ulSCLen, ulSCIDLen, ulDynamicLen, ulCheckSumLen, ulCheckSum, ulSMSLen;
	HBOOL			bValidCard;
	IR_API_SC_INFO	stStbSCInfoSmartCardData;
	SvcCas_CtrlParam_t			stOutParam;
	IR_GetSCInfoOutParam_t	stSCInfo;

	if((pucSMSCode == NULL) || (pulSMSCodeLen == NULL))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

#if 0
	hErr = local_cas_ir_sms_GetShorteningSCID("55841370237", 0, pszShorteningSCID, &ulSCIDLen);
	//sprintf(pszShorteningSCID, "%s", "CDZI5K");
	//ulSCIDLen = 6;
	//sprintf(pucDynamicPart, "%s", "H2L");
	//ulDynamicLen = 3;
#else
	hErr = AP_CAR_IR_GetSCValidCardInserted(&bValidCard);
	if(hErr != ERR_OK || bValidCard == FALSE)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	stOutParam.ulControlType	= eCACTRL_IR_SC_Info;
	stOutParam.pvOut			= &stSCInfo;
	stSCInfo.ucScSlotId		= 0;
	stSCInfo.pstIrApiScInfo		= &stStbSCInfoSmartCardData;

	hErr = svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_SC_Info, &stOutParam);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pszSCNumber,0, sizeof(pszSCNumber));
	ulSCLen = SvcCas_UtilStrLen(stStbSCInfoSmartCardData.szNumber);
	VK_MEM_Memcpy(pszSCNumber, stStbSCInfoSmartCardData.szNumber, ulSCLen - 4);

	VK_MEM_Memset(pszShorteningSCID, 0, sizeof(pszShorteningSCID));

	hErr = local_cas_ir_sms_GetShorteningSCID(pszSCNumber, 1, pszShorteningSCID, &ulSCIDLen);
#endif

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pucDynamicPart, 0, sizeof(pucDynamicPart));
	hErr = local_cas_ir_sms_MakeDynamicPart(pstEITInfo, ulChannelNo, ucPaymentType, pucDynamicPart, &ulDynamicLen);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pucSCIDPartFirstMixed, 0, sizeof(pucSCIDPartFirstMixed));
	hErr = local_cas_ir_sms_DoFirstMixing(pszShorteningSCID, ulSCIDLen, pucDynamicPart, ulDynamicLen, pucSCIDPartFirstMixed);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pucSCIDPartSecondMixed, 0, sizeof(pucSCIDPartSecondMixed));
	VK_MEM_Memset(pucDynamicPartMixed, 0, sizeof(pucDynamicPartMixed));
	hErr = local_cas_ir_sms_DoSecondMixing(pucSCIDPartFirstMixed, ulSCIDLen, pucDynamicPart, ulDynamicLen, pucSCIDPartSecondMixed, pucDynamicPartMixed);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pucTmp, 0, sizeof(pucTmp));
	pucTmp[0] = RECOGNITION_CHARACTER;
	VK_MEM_Memcpy(&pucTmp[1], pucSCIDPartSecondMixed, ulSCIDLen);
	VK_MEM_Memcpy(&pucTmp[1 + ulSCIDLen], pucDynamicPartMixed, ulDynamicLen);

	ulCheckSum = local_cas_ir_sms_CalculateCheckSum(pucTmp);

	VK_MEM_Memset(pucCheckSumPart, 0, sizeof(pucCheckSumPart));
	hErr = local_cas_ir_sms_MakeCheckSumPart(ulCheckSum, 1/*IRDETO*/, pucCheckSumPart, &ulCheckSumLen);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pucFinelSMS, 0, sizeof(pucFinelSMS));
	sprintf((char *)pucFinelSMS, "%c%s-%s%s", RECOGNITION_CHARACTER, pucSCIDPartSecondMixed, pucDynamicPartMixed, pucCheckSumPart);

	ulSMSLen = SvcCas_UtilStrLen(pucFinelSMS);

	VK_MEM_Memcpy(pucSMSCode, pucFinelSMS, ulSMSLen);
	pucSMSCode[ulSMSLen] = '\0';

	*pulSMSCodeLen = ulSMSLen;

	return ERR_OK;
}


HERROR XMGR_CAS_IR_SMS_GetSMSCodeWithDynamicPart(HUINT8 *pszDynamicPart, HUINT32 ulDynamicLen, 	HUINT8 *pucSMSCode, HUINT32 *pulSMSCodeLen)
{
	HERROR			hErr = ERR_OK;
	HUINT8			pucDynamicPart[10], pszSCNumber[IR_SC_INFO_NUMBER_LENGTH], pszShorteningSCID[10];
	HUINT8			pucSCIDPartFirstMixed[10], pucSCIDPartSecondMixed[10], pucDynamicPartMixed[10], pucCheckSumPart[10], pucTmp[20], pucFinelSMS[20];
	HUINT32			ulSCLen, ulSCIDLen, ulCheckSumLen, ulCheckSum, ulSMSLen;
	HBOOL			bValidCard;
	IR_API_SC_INFO	stStbSCInfoSmartCardData;
	SvcCas_CtrlParam_t						stOutParam;
	IR_GetSCInfoOutParam_t				stSCInfo;

	if((pucSMSCode == NULL) || (pulSMSCodeLen == NULL))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

#if 0
	//hErr = local_cas_ir_sms_GetShorteningSCID("55841370237", 0, pszShorteningSCID, &ulSCIDLen);
	sprintf(pszShorteningSCID, "%s", "L2F3");
	ulSCIDLen = 4;
	//sprintf(pucDynamicPart, "%s", "H2L");
	//ulDynamicLen = 3;
#else
	hErr = AP_CAR_IR_GetSCValidCardInserted(&bValidCard);
	if(hErr != ERR_OK || bValidCard == FALSE)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	stOutParam.ulControlType	= eCACTRL_IR_SC_Info;
	stOutParam.pvOut			= &stSCInfo;
	stSCInfo.ucScSlotId		= 0;
	stSCInfo.pstIrApiScInfo		= &stStbSCInfoSmartCardData;

	hErr = svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_SC_Info, &stOutParam);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pszSCNumber,0, sizeof(pszSCNumber));
	ulSCLen = SvcCas_UtilStrLen(stStbSCInfoSmartCardData.szNumber);
	VK_MEM_Memcpy(pszSCNumber, stStbSCInfoSmartCardData.szNumber, ulSCLen - 4);

	VK_MEM_Memset(pszShorteningSCID, 0, sizeof(pszShorteningSCID));

	hErr = local_cas_ir_sms_GetShorteningSCID(pszSCNumber, 1, pszShorteningSCID, &ulSCIDLen);
#endif

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pucDynamicPart, 0, sizeof(pucDynamicPart));
	VK_MEM_Memcpy(pucDynamicPart, pszDynamicPart, ulDynamicLen);

	VK_MEM_Memset(pucSCIDPartFirstMixed, 0, sizeof(pucSCIDPartFirstMixed));
	hErr = local_cas_ir_sms_DoFirstMixing(pszShorteningSCID, ulSCIDLen, pucDynamicPart, ulDynamicLen, pucSCIDPartFirstMixed);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pucSCIDPartSecondMixed, 0, sizeof(pucSCIDPartSecondMixed));
	VK_MEM_Memset(pucDynamicPartMixed, 0, sizeof(pucDynamicPartMixed));
	hErr = local_cas_ir_sms_DoSecondMixing(pucSCIDPartFirstMixed, ulSCIDLen, pucDynamicPart, ulDynamicLen, pucSCIDPartSecondMixed, pucDynamicPartMixed);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pucTmp, 0, sizeof(pucTmp));
	pucTmp[0] = RECOGNITION_CHARACTER;
	VK_MEM_Memcpy(&pucTmp[1], pucSCIDPartSecondMixed, ulSCIDLen);
	VK_MEM_Memcpy(&pucTmp[1 + ulSCIDLen], pucDynamicPartMixed, ulDynamicLen);

	ulCheckSum = local_cas_ir_sms_CalculateCheckSum(pucTmp);

	VK_MEM_Memset(pucCheckSumPart, 0, sizeof(pucCheckSumPart));
	hErr = local_cas_ir_sms_MakeCheckSumPart(ulCheckSum, 1/*IRDETO*/, pucCheckSumPart, &ulCheckSumLen);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	VK_MEM_Memset(pucFinelSMS, 0, sizeof(pucFinelSMS));
	sprintf((char *)pucFinelSMS, "%c%s-%s%s", RECOGNITION_CHARACTER, pucSCIDPartSecondMixed, pucDynamicPartMixed, pucCheckSumPart);

	ulSMSLen = SvcCas_UtilStrLen(pucFinelSMS);

	VK_MEM_Memcpy(pucSMSCode, pucFinelSMS, ulSMSLen);
	pucSMSCode[ulSMSLen] = '\0';

	*pulSMSCodeLen = ulSMSLen;

	return ERR_OK;
}
#if defined(CONFIG_DEBUG)
HERROR AP_DEBUG_CAS_IR_SMS_MakeSMSCode(void)
{
	HERROR hErr;
	XmgrCas_IrIppvEitInfo_t stEITInfo;
	HUINT32	ulChannelNo = 201, ulSMSCodeLen;
	HUINT8	ucPaymentType = 0, pucSMSCode[20];
	HxDATETIME_t stDateTime;

	stDateTime.stDate.usYear = 2010;
	stDateTime.stDate.ucMonth = 10;
	stDateTime.stDate.ucDay = 10;

	stDateTime.stTime.ucHour = 19;
	stDateTime.stTime.ucMinute = 38;
	stDateTime.stTime.ucSecond = 0;
	stDateTime.stTime.usMillisecond = 0;

	hErr = HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &stEITInfo.startTime);
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	VK_MEM_Memset(pucSMSCode, 0, sizeof(pucSMSCode));
	hErr = XMGR_CAS_IR_SMS_GetSMSCode(&stEITInfo, ulChannelNo, ucPaymentType, pucSMSCode, &ulSMSCodeLen);

	HxLOG_Print("SMS Code:[%s]\n", pucSMSCode);

	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	return hErr;
}
#endif


/* end of file */
