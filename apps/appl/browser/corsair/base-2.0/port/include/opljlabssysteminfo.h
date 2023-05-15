/**************************************************************************************/
/**
 * @file opljlabssysteminfo.h
 *
 * System information
 *
 * @author  Sunghoon Lee(shlee3@humaxdigital.com)
 * @date    2012/07/25
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLJLABSSYSTEMINFO_H__
#define __OPLJLABSSYSTEMINFO_H__

#include "bpltype.h"
#include "opltype.h"

/******************************************************************
	Define
******************************************************************/

#define	OPL_JLABS_SYSTEMINFO_MAX_NUM_OF_GROUP_ID  7
#define	OPL_JLABS_SYSTEMINFO_MAX_LENGTH_OF_CARD_ID 20

/******************************************************************
	Enum
 ******************************************************************/


/******************************************************************
	Typedef
******************************************************************/

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

BPLStatus OPL_JlabsSystemInfo_GetDocsisModemStatus(HUINT32 *pulDocsisModemStatus);

BPLStatus OPL_JlabsSystemInfo_GetDocsisDownFrequency(HUINT32 *pulDocsisDownFrequency);
BPLStatus OPL_JlabsSystemInfo_SetDocsisDownFrequency(HUINT32 ulDownFreqency, HBOOL *pbSuccess);

BPLStatus OPL_JlabsSystemInfo_GetDocsisQamType(HCHAR *pszDocsisQamType, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetDocsisDownSignalLevel(HUINT32 *pulDocsisDownSignalLevel);

BPLStatus OPL_JlabsSystemInfo_GetDocsisDownBer(HUINT32 *pulDocsisDownBer);

BPLStatus OPL_JlabsSystemInfo_GetDocsisDownSnr(HUINT32 *pulDocsisDownSnr);

BPLStatus OPL_JlabsSystemInfo_GetDocsisUpFrequency(HUINT32 *pulDocsisUpFrequency);

BPLStatus OPL_JlabsSystemInfo_GetDocsisUpChannelId(HUINT32 *pulDocsisUpChannelId);

BPLStatus OPL_JlabsSystemInfo_GetDocsisUpBandWidth(HUINT32 *pulDocsisUpBandWidth);

BPLStatus OPL_JlabsSystemInfo_GetDocsisUpSignalLevel(HUINT32 *pulDocsisUpSignalLevel);

BPLStatus OPL_JlabsSystemInfo_GetCardIdBCAS(HCHAR *pszCardIdBCAS, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetDiscernBCAS(HCHAR *pszDiscernBCAS, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetGroupIdBCAS(HCHAR **ppacGroupIdBCAS, HUINT32 *pNumOfId, HUINT32 maxBufLen, HUINT32 maxNumOfBuf);

BPLStatus OPL_JlabsSystemInfo_GetCardIdCCAS( HCHAR *pszCardIdCCAS, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetDiscernCCAS(HCHAR *pszDiscernCCAS, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetGroupIdCCAS(HCHAR **ppacGroupIdCCAS, HUINT32 *pNumOfId, HUINT32 maxBufLen, HUINT32 maxNumOfBuf);

BPLStatus OPL_JlabsSystemInfo_GetDeviceId( HCHAR *pszDeviceId, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetDeviceModel( HCHAR *pszDeviceModel, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetDeviceSerialNumber( HCHAR *pszDeviceSerialNumber, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetDeviceSoftwareVersion( HCHAR *pszDeviceSoftwareVersion, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetDeviceHardwareVersion( HCHAR *pszDeviceHardwareVersion, HUINT32 ulLen);

BPLStatus OPL_JlabsSystemInfo_GetSystemUpdateDate( HUINT32 *pulSystemUpdateDate);

BPLStatus OPL_JlabsSystemInfo_GetCardStatusBCAS(HBOOL *pbIsBCasStatusOK);

BPLStatus OPL_JlabsSystemInfo_GetCardStatusCCAS(HBOOL *pbIsCCasStatusOK);

BPLStatus OPL_JlabsSystemInfo_StartSettingInfo(HUINT32 ulSettingInfoOpt, HBOOL *pbSuccess);


#ifdef __cplusplus
};

/******************************************************************
	class OplJlabsSystemInfo
******************************************************************/
class OplJlabsSystemInfo {

public:
	OplJlabsSystemInfo()
	{
	}
	virtual ~OplJlabsSystemInfo()
	{
	}

	static bool getDocsisModemStatus(HUINT32 *pulDocsisModemStatus)
	{return OPL_JlabsSystemInfo_GetDocsisModemStatus(pulDocsisModemStatus) == BPL_STATUS_OK;	}

	static bool getDocsisDownFrequency(HUINT32 *pulDocsisDownFrequency)
	{return OPL_JlabsSystemInfo_GetDocsisDownFrequency(pulDocsisDownFrequency) == BPL_STATUS_OK;	}

	static bool setDocsisDownFrequency(HUINT32 ulDownFreqency, HBOOL *pbSuccess)
	{return OPL_JlabsSystemInfo_SetDocsisDownFrequency(ulDownFreqency, pbSuccess) == BPL_STATUS_OK;	}

	static bool getDocsisQamType(HCHAR *pszDocsisQamType, HUINT32 ulLen)
	{return OPL_JlabsSystemInfo_GetDocsisQamType(pszDocsisQamType, ulLen) == BPL_STATUS_OK;	}

	static bool getDocsisDownSignalLevel(HUINT32 *pulDocsisDownSignalLevel)
	{return OPL_JlabsSystemInfo_GetDocsisDownSignalLevel(pulDocsisDownSignalLevel) == BPL_STATUS_OK;	}

	static bool getDocsisDownBer(HUINT32 *pulDocsisDownBer)
	{return OPL_JlabsSystemInfo_GetDocsisDownBer(pulDocsisDownBer) == BPL_STATUS_OK;	}

	static bool getDocsisDownSnr(HUINT32 *pulDocsisDownSnr)
	{return OPL_JlabsSystemInfo_GetDocsisDownSnr(pulDocsisDownSnr) == BPL_STATUS_OK;	}

	static bool getDocsisUpFrequency(HUINT32 *pulDocsisUpFrequency)
	{return OPL_JlabsSystemInfo_GetDocsisUpFrequency(pulDocsisUpFrequency) == BPL_STATUS_OK;	}

	static bool getDocsisDocsisUpChannelId(HUINT32 *pulDocsisUpChannelId)
	{return OPL_JlabsSystemInfo_GetDocsisUpChannelId(pulDocsisUpChannelId) == BPL_STATUS_OK;	}

	static bool getDocsisUpBandWidth(HUINT32 *pulDocsisUpBandWidth)
	{return OPL_JlabsSystemInfo_GetDocsisUpBandWidth(pulDocsisUpBandWidth) == BPL_STATUS_OK;	}

	static bool getDocsisUpSignalLevel(HUINT32 *pulDocsisUpSignalLevel)
	{return OPL_JlabsSystemInfo_GetDocsisUpSignalLevel(pulDocsisUpSignalLevel) == BPL_STATUS_OK;	}

	static bool getCardIdBCAS(HCHAR *pszCardIdBCAS, HUINT32 ulLen)
	{return OPL_JlabsSystemInfo_GetCardIdBCAS(pszCardIdBCAS, ulLen) == BPL_STATUS_OK;	}

	static bool getDiscernBCAS(HCHAR *pszDiscernBCAS, HUINT32 ulLen)
	{return OPL_JlabsSystemInfo_GetDiscernBCAS(pszDiscernBCAS, ulLen) == BPL_STATUS_OK;	}

	static bool getGroupIdBCAS(HCHAR **ppacGroupIdBCAS, HUINT32 *pNumOfId, HUINT32 maxBufLen, HUINT32 maxNumOfBuf)
	{return OPL_JlabsSystemInfo_GetGroupIdBCAS(ppacGroupIdBCAS, pNumOfId, maxBufLen, maxNumOfBuf) == BPL_STATUS_OK;	}

	static bool getCardIdCCAS( HCHAR *pszCardIdCCAS, HUINT32 ulLen)
	{ return OPL_JlabsSystemInfo_GetCardIdCCAS( pszCardIdCCAS, ulLen) == BPL_STATUS_OK; }

	static bool getDiscernCCAS(HCHAR *pszDiscernCCAS, HUINT32 ulLen)
	{return OPL_JlabsSystemInfo_GetDiscernCCAS(pszDiscernCCAS, ulLen) == BPL_STATUS_OK;	}

	static bool getGroupIdCCAS(HCHAR **ppacGroupIdCCAS, HUINT32 *pNumOfId, HUINT32 maxBufLen, HUINT32 maxNumOfBuf)
	{return OPL_JlabsSystemInfo_GetGroupIdCCAS(ppacGroupIdCCAS, pNumOfId, maxBufLen, maxNumOfBuf) == BPL_STATUS_OK;	}

	static bool getDeviceId( HCHAR *pszDeviceId, HUINT32 ulLen)
	{ return OPL_JlabsSystemInfo_GetDeviceId( pszDeviceId, ulLen) ==	BPL_STATUS_OK; }

	static bool getDeviceModel( HCHAR *pszDeviceModel, HUINT32 ulLen)
	{ return OPL_JlabsSystemInfo_GetDeviceModel( pszDeviceModel, ulLen) == BPL_STATUS_OK; }

	static bool getDeviceSerialNumber( HCHAR *pszDeviceSerialNumber, HUINT32 ulLen)
	{ return OPL_JlabsSystemInfo_GetDeviceSerialNumber( pszDeviceSerialNumber, ulLen) == BPL_STATUS_OK; }

	static bool getDeviceSoftwareVersion( HCHAR *pszDeviceSoftwareVersion, HUINT32 ulLen)
	{ return OPL_JlabsSystemInfo_GetDeviceSoftwareVersion( pszDeviceSoftwareVersion,  ulLen) == BPL_STATUS_OK; }

	static bool getDeviceHardwareVersion( HCHAR *pszDeviceHardwareVersion, HUINT32 ulLen)
	{ return OPL_JlabsSystemInfo_GetDeviceHardwareVersion( pszDeviceHardwareVersion,  ulLen) == BPL_STATUS_OK; }

	static bool getSystemUpdateDate( HUINT32 *pulSystemUpdateDate)
	{ return OPL_JlabsSystemInfo_GetSystemUpdateDate( pulSystemUpdateDate) == BPL_STATUS_OK; }

	static bool getCardStatusBCAS(HBOOL *pbIsBCasStatusOK)
	{ return OPL_JlabsSystemInfo_GetCardStatusBCAS(pbIsBCasStatusOK) == BPL_STATUS_OK; }

	static bool getCardStatusCCAS(HBOOL *pbIsCCasStatusOK)
	{ return OPL_JlabsSystemInfo_GetCardStatusCCAS(pbIsCCasStatusOK) == BPL_STATUS_OK; }

	static bool startSettingInfo(HUINT32 ulSettingInfoOpt, HBOOL *pbSuccess)
	{ return OPL_JlabsSystemInfo_StartSettingInfo(ulSettingInfoOpt, pbSuccess) == BPL_STATUS_OK;}

};

#endif

#endif


