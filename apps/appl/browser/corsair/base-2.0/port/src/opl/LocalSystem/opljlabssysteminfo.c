/**************************************************************************************/
/**
 * @file opljlabssysteminfo.c
 *
 * System information
 *
 * @author  Sunghoon Lee(shlee3@humaxdigital.com)
 * @date    2012/07/25
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "opljlabssysteminfo.h"

#include "oapi.h"
#include "hapi.h"
#include <apk.h>
#include <stdlib.h>
#include <string.h>
#include <hlib.h>


/******************************************************************
	Define
******************************************************************/
#define	CHECK_BPL_PARAM_PTR(a)		{if (a == NULL) {HxLOG_Error("Input argument is NULL\n"); return BPL_STATUS_ERROR;}}

/******************************************************************
	Static variables
******************************************************************/

/******************************************************************
	Static functions
******************************************************************/
/**
0 : 정지
1 : 초기화 중
2 : Scan 중
3 : Ranging 중 (Cable modem 상향신호(upstream)를 CMTS와 주고 받는 동작)
4 : IP 취득 중 (CM)
5 : IP 취득 중 (CPE)
6 : 정상 동작 중
9 : 장해 발생 중
*/
BPLStatus OPL_JlabsSystemInfo_GetDocsisModemStatus(HUINT32 *pulDocsisModemStatus)
{
	APKE_NETCM_Status_e eStatus;

	CHECK_BPL_PARAM_PTR(pulDocsisModemStatus);

	*pulDocsisModemStatus = 0;

	APK_NETWORK_CM_GetStatus(&eStatus);
	switch(eStatus)
	{
		case eNETCM_NOT_READY:
		case eNETCM_NOT_SYNC:
			*pulDocsisModemStatus = 2;	//
			break;
		case eNETCM_PHY_SYNC_DONE:
		case eNETCM_PARAM_ACQUIRED:
			*pulDocsisModemStatus = 3;
			break;
		case eNETCM_RANGING_COMPLETE:
		case eNETCM_IP_COMPLETE:
		case eNETCM_TOD_CERTIFY:
		case eNETCM_SECURITY_CERTIFY:
		case eNETCM_PARAM_TRAANSE_COMPLETE:
		case eNETCM_REGIST_COMPLETE:
			*pulDocsisModemStatus = 4;
			break;
		case eNETCM_OPERATION:
			*pulDocsisModemStatus = 6;
			break;
		case eNETCM_ACCESS_DENIED:
			*pulDocsisModemStatus = 9;
			break;

		default:
			break;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDocsisDownFrequency(HUINT32 *pulDocsisDownFrequency)
{
	HINT32 nFreq = 0;
	CHECK_BPL_PARAM_PTR(pulDocsisDownFrequency);

	APK_NETWORK_CM_GetDownStreamInfo( eNETCM_DOWN_FREQ, &nFreq);
	*pulDocsisDownFrequency = nFreq ? (nFreq / 1000) : 0;

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_SetDocsisDownFrequency(HUINT32 ulDownFrequency, HBOOL *pbSuccess)
{
	HERROR	hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(pbSuccess);
	pbSuccess = FALSE;

	HxLOG_Print("[%s] ulDownFrequency %d\n",__FUNCTION__,ulDownFrequency);
	hError = APK_NETWORK_CM_SetDownStreamInfo( eNETCM_DOWN_FREQ, ulDownFrequency * 1000);

	if(hError == ERR_OK)
	{
		pbSuccess = TRUE;
		return BPL_STATUS_OK;
	}
	return BPL_STATUS_ERROR;
}


BPLStatus OPL_JlabsSystemInfo_GetDocsisQamType(HCHAR *pszDocsisQamType, HUINT32 ulLen)
{
	APKE_NETCM_QAMType_e	eQamType = eNETCM_DOWN_MOD_UNKNOWN;

	CHECK_BPL_PARAM_PTR(pszDocsisQamType);

	HxSTD_MemSet(pszDocsisQamType, 0, sizeof(HCHAR)*ulLen);

	APK_NETWORK_CM_GetDownStreamInfo( eNETCM_DOWN_QAM, (HINT32 *)&eQamType);
	switch(eQamType)
	{
		case eNETCM_DOWN_MOD_QAM64:
			HxSTD_StrNCpy(pszDocsisQamType, "64QAM", ulLen-1);
			break;

		case eNETCM_DOWN_MOD_QAM256:
			HxSTD_StrNCpy(pszDocsisQamType, "256QAM", ulLen-1);
			break;

		case eNETCM_DOWN_MOD_OTHER:
		case eNETCM_DOWN_MOD_UNKNOWN:
		default:
			break;
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDocsisDownSignalLevel(HUINT32 *pulDocsisDownSignalLevel)
{
	HINT32 nSignalLevel = 0;

	CHECK_BPL_PARAM_PTR(pulDocsisDownSignalLevel);

	APK_NETWORK_CM_GetDownStreamInfo( eNETCM_DOWN_SIGLEVEL, &nSignalLevel );
	*pulDocsisDownSignalLevel = nSignalLevel;
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDocsisDownBer(HUINT32 *pulDocsisDownBer)
{
	HINT32 nBer = 0;

	CHECK_BPL_PARAM_PTR(pulDocsisDownBer);

	APK_NETWORK_CM_GetDownStreamInfo( eNETCM_DOWN_BER, &nBer);
	*pulDocsisDownBer = nBer;

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDocsisDownSnr(HUINT32 *pulDocsisDownSnr)
{
	HINT32 nSnr = 0;

	CHECK_BPL_PARAM_PTR(pulDocsisDownSnr);

	APK_NETWORK_CM_GetDownStreamInfo( eNETCM_DOWN_SNR, &nSnr);
	/* in dB*/
	*pulDocsisDownSnr = nSnr ? (nSnr/10) : 0;

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDocsisUpFrequency(HUINT32 *pulDocsisUpFrequency)
{
	HINT32 nFreq = 0;

	CHECK_BPL_PARAM_PTR(pulDocsisUpFrequency);

	APK_NETWORK_CM_GetUpStreamInfo( eNETCM_UP_FREQ, &nFreq );
	/* in KHz */
	*pulDocsisUpFrequency = nFreq ? (nFreq / 1000) : 0;

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDocsisUpChannelId(HUINT32 *pulDocsisUpChannelId)
{
	HINT32 nChannelId = 0;

	CHECK_BPL_PARAM_PTR(pulDocsisUpChannelId);

	APK_NETWORK_CM_GetUpStreamInfo( eNETCM_UP_CHID, &nChannelId );
	*pulDocsisUpChannelId = nChannelId;

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDocsisUpBandWidth(HUINT32 *pulDocsisUpBandWidth)
{
	HINT32 nBandWidth = 0;

	CHECK_BPL_PARAM_PTR(pulDocsisUpBandWidth);

	APK_NETWORK_CM_GetUpStreamInfo( eNETCM_UP_BANDWIDTH, &nBandWidth);
	/* in KHz */
	*pulDocsisUpBandWidth = nBandWidth ? (nBandWidth/1000) : 0;

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDocsisUpSignalLevel(HUINT32 *pulDocsisUpSignalLevel)
{
	HINT32 nSignalLevel = 0;

	CHECK_BPL_PARAM_PTR(pulDocsisUpSignalLevel);

	APK_NETWORK_CM_GetUpStreamInfo( eNETCM_UP_SIGLEVEL, &nSignalLevel );
	*pulDocsisUpSignalLevel = nSignalLevel;

	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetCardIdBCAS(HCHAR *pszCardIdBCAS, HUINT32 ulLen)
{
	HERROR hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(pszCardIdBCAS);
	HxSTD_MemSet(pszCardIdBCAS, 0, sizeof(HCHAR)*ulLen);
	hError = APK_CAS_BC_GetCardInfoBCCas(BCAS_CARDINFO_TYPE_BCAS_CARDID, pszCardIdBCAS, ulLen);
	if(hError == ERR_OK)
		return BPL_STATUS_OK;
	return BPL_STATUS_ERROR;
}

BPLStatus OPL_JlabsSystemInfo_GetDiscernBCAS(HCHAR *pszDiscernBCAS, HUINT32 ulLen)
{
	HERROR hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(pszDiscernBCAS);
	HxSTD_MemSet(pszDiscernBCAS, 0, sizeof(HCHAR)*ulLen);
	hError = APK_CAS_BC_GetCardInfoBCCas(BCAS_CARDINFO_TYPE_BCAS_DISCERN, pszDiscernBCAS, ulLen);
	if(hError == ERR_OK)
		return BPL_STATUS_OK;
	return BPL_STATUS_ERROR;
}

BPLStatus OPL_JlabsSystemInfo_GetGroupIdBCAS(HCHAR **ppacGroupIdBCAS, HUINT32 *pNumOfId, HUINT32 maxBufLen, HUINT32 maxNumOfBuf)
{
	HERROR hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(ppacGroupIdBCAS);
	CHECK_BPL_PARAM_PTR(pNumOfId);
	hError = APK_CAS_BC_GetGroupIdBCCas(BCAS_CARDINFO_TYPE_BCAS_GROUPID, ppacGroupIdBCAS, pNumOfId, maxBufLen, maxNumOfBuf);
	if(hError == ERR_OK)
		return BPL_STATUS_OK;
	return BPL_STATUS_ERROR;
}

BPLStatus OPL_JlabsSystemInfo_GetCardIdCCAS( HCHAR *pszCardIdCCAS, HUINT32 ulLen)
{
	HERROR hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(pszCardIdCCAS);
	HxSTD_MemSet(pszCardIdCCAS, 0, sizeof(HCHAR)*ulLen);
	hError = APK_CAS_BC_GetCardInfoBCCas(BCAS_CARDINFO_TYPE_CCAS_CARDID, pszCardIdCCAS, ulLen);
	if(hError == ERR_OK)
		return BPL_STATUS_OK;
	return BPL_STATUS_ERROR;
}

BPLStatus OPL_JlabsSystemInfo_GetDiscernCCAS(HCHAR *pszDiscernCCAS, HUINT32 ulLen)
{
	HERROR hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(pszDiscernCCAS);
	HxSTD_MemSet(pszDiscernCCAS, 0, sizeof(HCHAR)*ulLen);
	hError = APK_CAS_BC_GetCardInfoBCCas(BCAS_CARDINFO_TYPE_CCAS_DISCERN, pszDiscernCCAS, ulLen);
	if(hError == ERR_OK)
		return BPL_STATUS_OK;
	return BPL_STATUS_ERROR;
}

BPLStatus OPL_JlabsSystemInfo_GetGroupIdCCAS(HCHAR **ppacGroupIdCCAS, HUINT32 *pNumOfId, HUINT32 maxBufLen, HUINT32 maxNumOfBuf)
{
	HERROR hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(ppacGroupIdCCAS);
	CHECK_BPL_PARAM_PTR(pNumOfId);
	hError = APK_CAS_BC_GetGroupIdBCCas(BCAS_CARDINFO_TYPE_CCAS_GROUPID, ppacGroupIdCCAS, pNumOfId, maxBufLen, maxNumOfBuf);
	if(hError == ERR_OK)
		return BPL_STATUS_OK;
	return BPL_STATUS_ERROR;
}

BPLStatus OPL_JlabsSystemInfo_GetDeviceId( HCHAR *pszDeviceId, HUINT32 ulLen)
{
	CHECK_BPL_PARAM_PTR(pszDeviceId);
	HxSTD_MemSet(pszDeviceId, 0, sizeof(HCHAR)*ulLen);
	APK_SYSINFO_STB_GetDeviceId(pszDeviceId, ulLen);
	return BPL_STATUS_OK;
}


BPLStatus OPL_JlabsSystemInfo_GetDeviceModel( HCHAR *pszDeviceModel, HUINT32 ulLen)
{
	CHECK_BPL_PARAM_PTR(pszDeviceModel);
	HxSTD_MemSet(pszDeviceModel, 0, sizeof(HCHAR)*ulLen);
	APK_SYSINFO_STB_GetModelName(pszDeviceModel, ulLen);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDeviceSerialNumber( HCHAR *pszDeviceSerialNumber, HUINT32 ulLen)
{
	CHECK_BPL_PARAM_PTR(pszDeviceSerialNumber);
	HxSTD_MemSet(pszDeviceSerialNumber, 0, sizeof(HCHAR)*ulLen);
	APK_SYSINFO_STB_GetSerialNumber(pszDeviceSerialNumber, ulLen);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDeviceSoftwareVersion( HCHAR *pszDeviceSoftwareVersion, HUINT32 ulLen)
{
	CHECK_BPL_PARAM_PTR(pszDeviceSoftwareVersion);
	HxSTD_MemSet(pszDeviceSoftwareVersion, 0, sizeof(HCHAR)*ulLen);
	APK_SYSINFO_STB_GetSoftwareVersion(pszDeviceSoftwareVersion, ulLen);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetDeviceHardwareVersion( HCHAR *pszDeviceHardwareVersion, HUINT32 ulLen)
{
	CHECK_BPL_PARAM_PTR(pszDeviceHardwareVersion);
	HxSTD_MemSet(pszDeviceHardwareVersion, 0, sizeof(HCHAR)*ulLen);
	APK_SYSINFO_STB_GetHardwareVersion(pszDeviceHardwareVersion, ulLen);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetSystemUpdateDate( HUINT32 *pulSystemUpdateDate)
{
	CHECK_BPL_PARAM_PTR(pulSystemUpdateDate);
	APK_SQCSCENARIOJAPAN_GetSystemUpdateDate(pulSystemUpdateDate);
	return BPL_STATUS_OK;
}

BPLStatus OPL_JlabsSystemInfo_GetCardStatusBCAS(HBOOL *pbIsBCasStatusOK)
{
	HERROR hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(pbIsBCasStatusOK);
	hError = APK_CAS_BC_ISCardStatusBCCasOK(pbIsBCasStatusOK, NULL);
	if(hError == ERR_OK)
		return BPL_STATUS_OK;
	return BPL_STATUS_ERROR;
}

BPLStatus OPL_JlabsSystemInfo_GetCardStatusCCAS(HBOOL *pbIsCCasStatusOK)
{
	HERROR hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(pbIsCCasStatusOK);
	hError = APK_CAS_BC_ISCardStatusBCCasOK(NULL, pbIsCCasStatusOK);
	if(hError == ERR_OK)
		return BPL_STATUS_OK;
	return BPL_STATUS_ERROR;
}

BPLStatus OPL_JlabsSystemInfo_StartSettingInfo(HUINT32 ulSettingInfoOpt, HBOOL *pbSuccess)
{
	HERROR	hError = ERR_FAIL;
	CHECK_BPL_PARAM_PTR(pbSuccess);
	HxLOG_Print("[%s] ulSettingInfoOpt %d\n",__FUNCTION__,ulSettingInfoOpt);
	hError = APK_SQCSCENARIOJAPAN_StartSettingInfo(ulSettingInfoOpt, pbSuccess);
	if(hError == ERR_OK)
		return BPL_STATUS_OK;
	return BPL_STATUS_ERROR;
}

/* end of file */


