/**************************************************************************************/
/**
 * @file opljlabslocalsystem.h
 *
 * System information
 *
 * @author  Sunghoon Lee(shlee3@humaxdigital.com)
 * @date    2012/06/19
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLJLABSLOCALSYSTEM_H__
#define __OPLJLABSLOCALSYSTEM_H__

#include "bpltype.h"
#include "opltype.h"
#include "oplavoutput.h"
//#include "oplnetwork.h"
//#include "oplnetservice.h"
#include "opltuner.h"
//#include "oplfrontpanel.h"
//#include "oplswupdate.h"
#include "opldiscinfo.h"
#include <apk.h>

/******************************************************************
	Define
******************************************************************/
#define	OPL_JLABS_MAX_UDB_LEN		32

//#define DEBUG_JLS	1

#ifdef DEBUG_JLS
#define JLS_DBG(a)	a
#else
#define JLS_DBG(a)
#endif

#define	OPL_JLABS_LOCALSYSTEM_UNLIMITED_PRICE_STR	"unlimited"

/******************************************************************
	Enum
 ******************************************************************/
typedef enum{
	OPL_JLABS_LOCALSYSTEM_EVENT_STORAGE_PROGRESS 	= 0,
	OPL_JLABS_LOCALSYSTEM_EVENT_DISC_ATTACHED 		= 1,
	OPL_JLABS_LOCALSYSTEM_EVENT_DISC_DETACHED 		= 2,
	OPL_JLABS_LOCALSYSTEM_EVENT_RESULT_PAIRING 		= 3,
	OPL_JLABS_LOCALSYSTEM_EVENT_RESIDENT_APP_FINISH 		= 4,
	OPL_JLABS_LOCALSYSTEM_EVENT_DISC_LOAD_FINISHED 		= 5,
	OPL_JLABS_LOCALSYSTEM_EVENT_INSERT_SETUPINFO_FINISHED= 6,
	OPL_JLABS_LOCALSYSTEM_EVENT_UNKOWN,
}eOplJlabsLocalsystemEventType;

typedef enum{
	OPL_JLABS_LOCALSYSTEM_PAIRING_DONE =	0,
	OPL_JLABS_LOCALSYSTEM_PAIRING_OVER_MAX_HDD = 1,
	OPL_JLABS_LOCALSYSTEM_PAIRING_ABNORMAL_HDD = 2,
	OPL_JLABS_LOCALSYSTEM_PAIRING_UNKNOWN = 3,
}eOplJlabsLocalsystemPairingProgress;

typedef enum{
	OPL_JLABS_LOCALSYSTEM_STANDBYMODE_PWR_ON = 1,
	OPL_JLABS_LOCALSYSTEM_STANDBYMODE_PWR_OFF = 2,
	OPL_JLABS_LOCALSYSTEM_STANDBYMODE_UNKNOWN,
}eOplJlabsLocalSystemStandbyMode;
/******************************************************************
	Typedef
******************************************************************/
typedef void OplJlabsLocalSystem_t;
typedef void (*OplJlabsLocalListener_t)(eOplJlabsLocalsystemEventType eType, void *param1, void *param2, void *param3);

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


BPLStatus OPL_JlabsLocalSystem_GetLocalArea(HCHAR *pszLocalArea, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetAutoSWDownloadEnable( HBOOL *pbAutoDownEnable);

BPLStatus OPL_JlabsLocalSystem_GetPostcode( HCHAR *pszPostcode, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetChannelMaskEnable(HBOOL *pbChMaskEnable);

BPLStatus OPL_JlabsLocalSystem_GetSubtitleMode(HINT32 *pSubtitleMode);

BPLStatus OPL_JlabsLocalSystem_GetSuperImposeMode(HINT32 *pSuperImposeMode);

BPLStatus OPL_JlabsLocalSystem_GetLastChCCID(HCHAR *pszLastChCCID, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetSeamlessEnable( HBOOL *pbSeamlessEnabled);

BPLStatus OPL_JlabsLocalSystem_GetSetupConfigureLaunchMode(HUINT32 *pLaunchMode);

BPLStatus OPL_JlabsLocalSystem_GetAutoPPVPurchaseEnable(HBOOL *pbEnable);

BPLStatus OPL_JlabsLocalSystem_GetDisplayMode3D(HINT32 *pMode);

BPLStatus OPL_JlabsLocalSystem_GetThreeDMode(HBOOL *pMode);

BPLStatus OPL_JlabsLocalSystem_GetLimitPerPPV( HCHAR *pszLimitPrice, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetLimitPerPPM( HCHAR *pszLimitPrice, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetFriendlyName( HCHAR *pszFriendlyName, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetNaviURL( HCHAR *pszUrl, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetInteracTVURL( HCHAR *pszUrl, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetInternetBrowserURL( HCHAR *pszUrl, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetVodHomeURL( HCHAR *pszUrl, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetACSURL( HCHAR *pszUrl, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetTuningRange(HINT32 *pTuninRange);

BPLStatus OPL_JlabsLocalSystem_GetTimebarDispMode(HINT32 *pTimebarDispMode);

BPLStatus OPL_JlabsLocalSystem_GetPinUnlockExpireOption( HINT32 *pOption);

BPLStatus OPL_JlabsLocalSystem_GetAMPermissionLevel( HINT32 *pLevel);

BPLStatus OPL_JlabsLocalSystem_GetStandbyMode(HINT32 *pnMode);

BPLStatus OPL_JlabsLocalSystem_GetBmlInfoDispEnable( HBOOL *pbBmlInfoDispEnable);

BPLStatus OPL_JlabsLocalSystem_GetIplateInfoDispEnable( HBOOL *pbIplateInfoDispEnable);

BPLStatus OPL_JlabsLocalSystem_GetFrontDispEnable( HBOOL *pbFrontDispEnable);

BPLStatus OPL_JlabsLocalSystem_GetIsDMSPublic( HBOOL *pbIsDMSPublic);

BPLStatus OPL_JlabsLocalSystem_GetIsDMSDeliver( HBOOL *pbIsDMSDeliver);

BPLStatus OPL_JlabsLocalSystem_GetIsIPDubbing( HBOOL *pbIsIPDubbing);

BPLStatus OPL_JlabsLocalSystem_GetLastChCCIDByNetwork(HUINT32 netType, HCHAR *pszLastChCCID, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_GetChannelMaskPattern(HUINT32 *pChannelMaskPattern);

BPLStatus OPL_JlabsLocalSystem_GetSoftwareDownloadState(HUINT32 *pulSWDownloadState);

BPLStatus OPL_JlabsLocalSystem_GetDefaultFactoryForDap(HUINT32 *pnMode);

BPLStatus OPL_JlabsLocalSystem_GetDefaultDiscForDap(HCHAR *pszDiscId, HUINT32 ulLen);

BPLStatus OPL_JlabsLocalSystem_SetLocalArea( HCHAR *pszLocalArea);

BPLStatus OPL_JlabsLocalSystem_SetAutoSWDownloadEnable( HBOOL bAutoDownEnable);

BPLStatus OPL_JlabsLocalSystem_SetPostcode( HCHAR *pszPostcode);

BPLStatus OPL_JlabsLocalSystem_SetChannelMaskEnable(HBOOL bChMaskEnable);

BPLStatus OPL_JlabsLocalSystem_SetSubtitleMode(HINT32 subtitleMode);

BPLStatus OPL_JlabsLocalSystem_SetSuperImposeMode(HINT32 superImposeMode);

BPLStatus OPL_JlabsLocalSystem_SetSeamlessEnable( HBOOL bSeamlessEnabled);

BPLStatus OPL_JlabsLocalSystem_SetSetupConfigureLaunchMode(HINT32 launchMode);

BPLStatus OPL_JlabsLocalSystem_SetAutoPPVPurchaseEnable(HBOOL bEnable);

BPLStatus OPL_JlabsLocalSystem_SetDisplayMode3D(HINT32 bThreeDMode);

BPLStatus OPL_JlabsLocalSystem_SetThreeDMode(HBOOL bThreeDMode);

BPLStatus OPL_JlabsLocalSystem_SetNumOfTuners(HINT32 numOfTuner);

BPLStatus OPL_JlabsLocalSystem_SetLimitPerPPV( HCHAR *pszLimitOPrice);

BPLStatus OPL_JlabsLocalSystem_SetLimitPerPPM( HCHAR *pszLimitOPrice);

BPLStatus OPL_JlabsLocalSystem_SetNaviUrl( HCHAR *pszUrl);

BPLStatus OPL_JlabsLocalSystem_SetInteracTVUrl( HCHAR *pszUrl);

BPLStatus OPL_JlabsLocalSystem_SetInternetBrowserUrl( HCHAR *pszUrl);

BPLStatus OPL_JlabsLocalSystem_SetVodHomeUrl( HCHAR *pszUrl);

BPLStatus OPL_JlabsLocalSystem_SetAcsUrl( HCHAR *pszUrl);

BPLStatus OPL_JlabsLocalSystem_SetFriendlyName( HCHAR *pszFriendlyName);

BPLStatus OPL_JlabsLocalSystem_SetTuningRange(HINT32 TuningRange);

BPLStatus OPL_JlabsLocalSystem_SetTimebarDispMode(HINT32 mode);

BPLStatus OPL_JlabsLocalSystem_SetPinUnlockExpireOption( HINT32 option);

BPLStatus OPL_JlabsLocalSystem_SetAmPermissionLevel( HINT32 level);

BPLStatus OPL_JlabsLocalSystem_SetStandbyMode(HINT32 mode);

BPLStatus OPL_JlabsLocalSystem_SetBmlInfoDisoEnable( HBOOL bBmlInfoDispEnabled);

BPLStatus OPL_JlabsLocalSystem_SetIplateDispEnable( HBOOL bIplateDispEnable);

BPLStatus OPL_JlabsLocalSystem_SetFrontDispEnable( HBOOL bFrontDispEnable);

BPLStatus OPL_JlabsLocalSystem_SetIsDMSPublic( HBOOL bIsDMSPublic);

BPLStatus OPL_JlabsLocalSystem_SetDefaultFactoryForDap(HUINT32 mode);

BPLStatus OPL_JlabsLocalSystem_SetDefaultDiscForDap(HCHAR *pszDiscId);

BPLStatus OPL_JlabsLocalSystem_ResetUserInfo(void);

BPLStatus OPL_JlabsLocalSystem_GetChannelMaskResult(HBOOL *pChannelMaskResult);

BPLStatus OPL_JlabsLocalSystem_InsertSetupInfo(HINT32 *pnInsertResult);

#ifdef __cplusplus
};

/******************************************************************
	class OplJlabsLocalSystem
******************************************************************/
class OplJlabsLocalSystem {

public:
	OplJlabsLocalSystem()
	{
	}
	virtual ~OplJlabsLocalSystem()
	{
	}

	static bool getLocalArea( HCHAR *pszLocalArea, HUINT32 ulLen)
	{ return OPL_JlabsLocalSystem_GetLocalArea(pszLocalArea, ulLen) == BPL_STATUS_OK; }

	static bool getAutoSWDownloadEnable( HBOOL *pbAutoDownEnable)
	{ return OPL_JlabsLocalSystem_GetAutoSWDownloadEnable( pbAutoDownEnable) == BPL_STATUS_OK; }

	static bool getPostcode( HCHAR *pszPostcode, HUINT32 ulLen)
	{ return OPL_JlabsLocalSystem_GetPostcode(pszPostcode, ulLen) ==	BPL_STATUS_OK; }

	static bool getChannelMaskEnable(HBOOL *pbChMaskEnable)
	{ return OPL_JlabsLocalSystem_GetChannelMaskEnable( pbChMaskEnable) == BPL_STATUS_OK; }

	static bool getSubtitleMode(HINT32 *pSubtitleMode)
	{ return OPL_JlabsLocalSystem_GetSubtitleMode( pSubtitleMode) == BPL_STATUS_OK; }

	static bool getSuperImposeMode(HINT32 *pSuperImposeMode)
	{ return OPL_JlabsLocalSystem_GetSuperImposeMode( pSuperImposeMode) == BPL_STATUS_OK; }

	static bool getLastChCCID(HCHAR *pszLastChCCID, HUINT32 ulLen)
	{ return OPL_JlabsLocalSystem_GetLastChCCID( pszLastChCCID, ulLen) == BPL_STATUS_OK; }

	static bool getSeamlessEnable( HBOOL *pbSeamlessEnabled)
	{ return OPL_JlabsLocalSystem_GetSeamlessEnable( pbSeamlessEnabled) == BPL_STATUS_OK; }

	static bool getSetupConfigureLaunchMode(HUINT32 *pLaunchMode)
	{ return OPL_JlabsLocalSystem_GetSetupConfigureLaunchMode( pLaunchMode) == BPL_STATUS_OK; }

	static bool getAutoPPVPurchaseEnable(HBOOL *pbEnable)
	{ return OPL_JlabsLocalSystem_GetAutoPPVPurchaseEnable( pbEnable) == BPL_STATUS_OK; }

	static bool getThreeDMode(HBOOL *pbMode)
	{ return OPL_JlabsLocalSystem_GetThreeDMode( pbMode) == BPL_STATUS_OK; }

	static bool getDisplayMode3D(HINT32 *pMode)
	{ return OPL_JlabsLocalSystem_GetDisplayMode3D( pMode) == BPL_STATUS_OK; }

	static bool getLimitPerPPV(HCHAR *pszLimitPrice, HUINT32 ulLen)
	{ return OPL_JlabsLocalSystem_GetLimitPerPPV(pszLimitPrice, ulLen) ==	BPL_STATUS_OK; }

	static bool getLimitPerPPM(HCHAR *pszLimitPrice, HUINT32 ulLen)
	{ return OPL_JlabsLocalSystem_GetLimitPerPPM(pszLimitPrice, ulLen) ==	BPL_STATUS_OK; }

	static bool getFriendlyName(HCHAR *pszFriendlyName, HUINT32 ulLen)
	{ return OPL_JlabsLocalSystem_GetFriendlyName(pszFriendlyName, ulLen) == BPL_STATUS_OK; }

	static bool getNaviURL(HCHAR *pszUrl, HUINT32 ulLen)
	{return OPL_JlabsLocalSystem_GetNaviURL(pszUrl, ulLen) == BPL_STATUS_OK;}

	static bool getInteracTVURL(HCHAR *pszUrl, HUINT32 ulLen)
	{return OPL_JlabsLocalSystem_GetInteracTVURL(pszUrl, ulLen) == BPL_STATUS_OK;}

	static bool getInternetBrowserURL(HCHAR *pszUrl, HUINT32 ulLen)
	{return OPL_JlabsLocalSystem_GetInternetBrowserURL(pszUrl, ulLen) == BPL_STATUS_OK;}

	static bool getVodHomeURL(HCHAR *pszUrl, HUINT32 ulLen)
	{return OPL_JlabsLocalSystem_GetVodHomeURL(pszUrl, ulLen) == BPL_STATUS_OK;}

	static bool getACSURL(HCHAR *pszUrl, HUINT32 ulLen)
	{return OPL_JlabsLocalSystem_GetACSURL(pszUrl, ulLen) == BPL_STATUS_OK;}

	static bool getTuningRange(HINT32 *pTuningRange)
	{ return OPL_JlabsLocalSystem_GetTuningRange(pTuningRange) == BPL_STATUS_OK; }

	static bool getTimebarDisplayMode(HINT32 *pTimebarMode)
	{ return OPL_JlabsLocalSystem_GetTimebarDispMode(pTimebarMode) == BPL_STATUS_OK; }

	static bool getPinUnlockExpireOption( HINT32 *pOption)
	{ return OPL_JlabsLocalSystem_GetPinUnlockExpireOption( pOption) == BPL_STATUS_OK; }

	static bool getAMPermissionLevel( HINT32 *pLevel)
	{ return OPL_JlabsLocalSystem_GetAMPermissionLevel( pLevel) == BPL_STATUS_OK; }

	static bool getStandbyMode(HINT32 *pnStandbymode)
	{return OPL_JlabsLocalSystem_GetStandbyMode(pnStandbymode) == BPL_STATUS_OK;}

	static bool getBmlInfoDisplayEnable( HBOOL *pbBmlInfoDispEnable)
	{ return OPL_JlabsLocalSystem_GetBmlInfoDispEnable( pbBmlInfoDispEnable) == BPL_STATUS_OK; }

	static bool getIPlateDisplayEnable( HBOOL *pbIplateDispEnable)
	{ return OPL_JlabsLocalSystem_GetIplateInfoDispEnable( pbIplateDispEnable) == BPL_STATUS_OK; }

	static bool getFrontDisplayEnable( HBOOL *pbFronDispEnable)
	{ return OPL_JlabsLocalSystem_GetFrontDispEnable( pbFronDispEnable) == BPL_STATUS_OK; }

	static bool getIsDMSPublic( HBOOL *pbIsDMSPublic)
	{ return OPL_JlabsLocalSystem_GetIsDMSPublic( pbIsDMSPublic) == BPL_STATUS_OK; }

	static bool getIsDMSDeliver( HBOOL *pbIsDMSDeliver)
	{ return OPL_JlabsLocalSystem_GetIsDMSDeliver( pbIsDMSDeliver) == BPL_STATUS_OK; }

	static bool getIsIPDubbing( HBOOL *pbIsIPDubbing)
	{ return OPL_JlabsLocalSystem_GetIsIPDubbing( pbIsIPDubbing) == BPL_STATUS_OK; }

	static bool getLastChCCIDByNetwork(HUINT32 netType, HCHAR *pszLastChCCID, HUINT32 ulLen)
	{ return OPL_JlabsLocalSystem_GetLastChCCIDByNetwork(netType,  pszLastChCCID, ulLen) == BPL_STATUS_OK; }

	static bool getChannelMaskPattern(HUINT32 *pulChannelMaskPattern)
	{ return OPL_JlabsLocalSystem_GetChannelMaskPattern(pulChannelMaskPattern) == BPL_STATUS_OK; }

	static bool getSoftwareDownloadState(HUINT32 *pulSWDownloadState)
	{ return OPL_JlabsLocalSystem_GetSoftwareDownloadState(pulSWDownloadState) == BPL_STATUS_OK; }

	static bool setLocalArea( HCHAR *pszLocalArea)
	{ return OPL_JlabsLocalSystem_SetLocalArea( pszLocalArea) == BPL_STATUS_OK; }

	static bool setAutoSWDownloadEnable( HBOOL bAutoDownEnable)
	{ return OPL_JlabsLocalSystem_SetAutoSWDownloadEnable( bAutoDownEnable) == BPL_STATUS_OK; }

	static bool setPostcode( HCHAR *pszPostcode)
	{ return OPL_JlabsLocalSystem_SetPostcode(pszPostcode) ==	BPL_STATUS_OK; }

	static bool setChannelMaskEnable(HBOOL bChMaskEnable)
	{ return OPL_JlabsLocalSystem_SetChannelMaskEnable( bChMaskEnable) == BPL_STATUS_OK; }

	static bool setSubtitleMode(HINT32 subtitleMode)
	{ return OPL_JlabsLocalSystem_SetSubtitleMode( subtitleMode) == BPL_STATUS_OK; }

	static bool setSuperImposeMode(HINT32 superImposeMode)
	{ return OPL_JlabsLocalSystem_SetSuperImposeMode( superImposeMode) == BPL_STATUS_OK; }

	static bool setSeamlessEnable( HBOOL bSeamlessEnabled)
	{ return OPL_JlabsLocalSystem_SetSeamlessEnable( bSeamlessEnabled) == BPL_STATUS_OK; }

	static bool setSetupConfigureLaunchMode(HINT32 launchMode)
	{ return OPL_JlabsLocalSystem_SetSetupConfigureLaunchMode( launchMode) == BPL_STATUS_OK; }

	static bool setDisplayMode3D(HINT32 bMode)
	{ return OPL_JlabsLocalSystem_SetDisplayMode3D( bMode) == BPL_STATUS_OK; }

	static bool setThreeDMode(HBOOL bMode)
	{ return OPL_JlabsLocalSystem_SetThreeDMode( bMode) == BPL_STATUS_OK; }

	static bool setAutoPPVPurchaseEnable(HBOOL bEnable)
	{ return OPL_JlabsLocalSystem_SetAutoPPVPurchaseEnable( bEnable) == BPL_STATUS_OK; }

	static bool setNumOfTuners(HINT32 numOfTuner)
	{ return OPL_JlabsLocalSystem_SetNumOfTuners( numOfTuner) == BPL_STATUS_OK; }

	static bool setLimitPerPPV(HCHAR *pszLimitPrice)
	{ return OPL_JlabsLocalSystem_SetLimitPerPPV(pszLimitPrice) ==	BPL_STATUS_OK; }

	static bool setLimitPerPPM(HCHAR *pszLimitPrice)
	{ return OPL_JlabsLocalSystem_SetLimitPerPPM(pszLimitPrice) ==	BPL_STATUS_OK; }

	static bool setNaviUrl(HCHAR *pszUrl)
	{ return OPL_JlabsLocalSystem_SetNaviUrl(pszUrl) ==	BPL_STATUS_OK; }

	static bool setInteracTVUrl(HCHAR *pszUrl)
	{ return OPL_JlabsLocalSystem_SetInteracTVUrl(pszUrl) ==	BPL_STATUS_OK; }

	static bool setInternetBrowserUrl(HCHAR *pszUrl)
	{ return OPL_JlabsLocalSystem_SetInternetBrowserUrl(pszUrl) ==	BPL_STATUS_OK; }

	static bool setVodHomeUrl(HCHAR *pszUrl)
	{ return OPL_JlabsLocalSystem_SetVodHomeUrl(pszUrl) ==	BPL_STATUS_OK; }

	static bool setAcsUrl(HCHAR *pszUrl)
	{ return OPL_JlabsLocalSystem_SetAcsUrl(pszUrl) ==	BPL_STATUS_OK; }

	static bool setFriendlyName(HCHAR *pszFriendlyName)
	{ return OPL_JlabsLocalSystem_SetFriendlyName(pszFriendlyName) == BPL_STATUS_OK; }

	static bool setTuningRange(HINT32 tuningRange)
	{ return OPL_JlabsLocalSystem_SetTuningRange(tuningRange) == BPL_STATUS_OK; }

	static bool setTimebarDisplayMode(HINT32 timebarMode)
	{ return OPL_JlabsLocalSystem_SetTimebarDispMode(timebarMode) == BPL_STATUS_OK; }

	static bool setPinUnlockExpireOption( HINT32 option)
	{ return OPL_JlabsLocalSystem_SetPinUnlockExpireOption( option) == BPL_STATUS_OK; }

	static bool setAmPermissionLevel ( HINT32 level)
	{ return OPL_JlabsLocalSystem_SetAmPermissionLevel( level) == BPL_STATUS_OK; }

	static bool setStandbyMode( HINT32 mode)
	{ return OPL_JlabsLocalSystem_SetStandbyMode( mode) == BPL_STATUS_OK; }

	static bool setBmlInfoDisplayEnable( HBOOL bBmlInfoDispEnable)
	{ return OPL_JlabsLocalSystem_SetBmlInfoDisoEnable( bBmlInfoDispEnable) == BPL_STATUS_OK; }

	static bool setIPlateDisplayEnable( HBOOL bIplateDispEnable)
	{ return OPL_JlabsLocalSystem_SetIplateDispEnable( bIplateDispEnable) == BPL_STATUS_OK; }

	static bool setFrontDisplayEnable( HBOOL bFronDispEnable)
	{ return OPL_JlabsLocalSystem_SetFrontDispEnable( bFronDispEnable) == BPL_STATUS_OK; }

	static bool setIsDMSPublic( HBOOL bIsDMSPublic)
	{ return OPL_JlabsLocalSystem_SetIsDMSPublic( bIsDMSPublic) == BPL_STATUS_OK; }

	static bool resetUserInfo(void)
	{ return OPL_JlabsLocalSystem_ResetUserInfo() == BPL_STATUS_OK; }

	static bool doChannelMask( HBOOL *pbChannelMaskResult)
	{ return OPL_JlabsLocalSystem_GetChannelMaskResult(pbChannelMaskResult) == BPL_STATUS_OK; }

	static bool insertSetupInfo(HINT32 *pnInsertResult)
	{ return OPL_JlabsLocalSystem_InsertSetupInfo(pnInsertResult) == BPL_STATUS_OK;}

	// HDD 관리 관련 기능
	static bool setStoragePairing(const char *storageId, const char *storageName);
	static bool setDefaultStorage(const char *storageId);
	static bool setStorageName(const char *storageId, const char *storageName);

	static void setEventListener(OplJlabsLocalListener_t listener);
	static void DeviceInfoNotifier(APKE_MS_NOTIFY_Type_e eType, void *pvData);
	static void DeviceListNotifier(APKE_MS_NOTIFY_Type_e eType, void *pvData);
	static void updateDeviceInfoList();
	static OplDiscInfo* makeDiscInfo(const APKS_MS_DEVINFO_t &devInfo);
	static void printDeviceInfo();
	static bool DetachDisc(const char *storageId, bool force);
	static bool startPairing();
	static bool abortPairing();
	static bool startResidentApp(const char *appName, const char *exInfo);
	static bool ResidentAppFinish(const char *appName);
	static void ResidentAppFinishNotifier(HCHAR *appName);
	static void DiscScanNotifier(APKE_MS_NOTIFY_Type_e eType, void *pvData);
	static void insertSetupInfoNotifier(HUINT32 p1, HUINT32 p2, HUINT32 p3);
private:
	static	HUINT32 mCountDeviceInfoList;
	static 	APKS_MS_DEVINFO_t* mDeviceInfoList;
	static	HCHAR *mDetachDiscId;
};

#endif

#endif


