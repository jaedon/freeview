/**************************************************************************************/
/**
 * @file opllocalsystem.h
 *
 * System information
 *
 **************************************************************************************
 **/

#ifndef __OPLLOCALSYSTEM_H__
#define __OPLLOCALSYSTEM_H__

#include "bpltype.h"
#include "opltype.h"
#include "oplavoutput.h"
#include "oplnetwork.h"
#include "oplnetservice.h"
#include "opltuner.h"
#include "oplfrontpanel.h"

//#include "oplswupdate.h"

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/
typedef enum {
	OPLLOCALSYSTEM_TV_STANDARD_UNKNOWN			= 0,
	OPLLOCALSYSTEM_TV_STANDARD_NTSC				= 1,
	OPLLOCALSYSTEM_TV_STANDARD_PAL				= 2,
	OPLLOCALSYSTEM_TV_STANDARD_SECAM			= 4
} eOplTVStandard;

typedef	struct
{
	HUINT32		ulValue;
	HCHAR		*pszValue;
} EnumStringPair_t;

typedef enum {
    OPLVERIMATRIX_LIB_IPTV  = 0,
    OPLVERIMATRIX_LIB_WEB,
    OPLVERIMATRIX_LIB_GATEWAY,
     OPLVERIMATRIX_LIB_DVB
}eOplVerimatirxLibType;


/******************************************************************
	Typedef
******************************************************************/
typedef void OplLocalSystem_t;

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern	HCHAR*		OPL_UTIL_GetStringFromValue(EnumStringPair_t *pstPairList, const HUINT32 ulValue);
extern	HUINT32		OPL_UTIL_GetValueFromString(EnumStringPair_t *pstPairList, const HCHAR *pszData);

BPLStatus OPL_LocalSystem_GetDeviceID( char *pszDevID);

BPLStatus OPL_LocalSystem_GetSystemReady( BPL_BOOL *pbReady);

BPLStatus OPL_LocalSystem_GetModelName( char *pszName);

BPLStatus OPL_LocalSystem_GetUpdatedList( char *pszList);

BPLStatus OPL_LocalSystem_GetVendorName( char *pszName);

BPLStatus OPL_LocalSystem_GetMicomVersion( char *pszName);

BPLStatus OPL_LocalSystem_GetRemoteCustomCode(int *pnCustomCode);

BPLStatus OPL_LocalSystem_GetWakeupReason( char *pszWakeupReason);

BPLStatus OPL_LocalSystem_GetSoftwareVersion( char *pszVersion);

BPLStatus OPL_LocalSystem_GetPackageVersion( char *pszVersion);

BPLStatus OPL_LocalSystem_GetHardwareVersion( char *pszVersion);

BPLStatus OPL_LocalSystem_GetSerialNumber( char *pszNumber);

BPLStatus OPL_LocalSystem_GetPvrEnabled( BPL_BOOL *pbEnabled);

BPLStatus OPL_LocalSystem_GetInternalHddEnabled(BPL_BOOL *pbEnabled);

BPLStatus OPL_LocalSystem_GetStandby( BPL_BOOL *pbStandby);

BPLStatus OPL_LocalSystem_SetStandby( BPL_BOOL bStandby);

BPLStatus OPL_LocalSystem_GetVolume( unsigned int *pnVolume);

BPLStatus OPL_LocalSystem_SetVolume( unsigned int nVolume);

BPLStatus OPL_LocalSystem_GetMute( BPL_BOOL *pbMute);

BPLStatus OPL_LocalSystem_SetMute( BPL_BOOL bMute);

BPLStatus OPL_LocalSystem_GetMicrophoneVolume( unsigned int *pnVolume);

BPLStatus OPL_LocalSystem_SetMicrophoneVolume( unsigned int nVolume);

BPLStatus OPL_LocalSystem_GetMicrophoneEnabled( BPL_BOOL *pbEnable);

BPLStatus OPL_LocalSystem_SetMicrophoneEnabled( BPL_BOOL bEnable);

BPLStatus OPL_LocalSystem_GetConnectedMicrophone(unsigned int *pnCnt);

BPLStatus OPL_LocalSystem_GetTVStandard( eOplTVStandard *eStandard);

BPLStatus OPL_LocalSystem_GetSystemID( char *pszID);

BPLStatus OPL_LocalSystem_GetScreenLanguage( char *pszLanguage);

BPLStatus OPL_LocalSystem_SetScreenLanguage( const char *pszLanguage);

BPLStatus OPL_LocalSystem_GetPreferredAudioLanuage( char *pszLanguage, int nIndex);

BPLStatus OPL_LocalSystem_SetPreferredAudioLanuage( const char *pszLanguage, int nIndex);

BPLStatus OPL_LocalSystem_GetPreferredSubtitleLanuage( char *pszLanguage, int nIndex);

BPLStatus OPL_LocalSystem_SetPreferredSubtitleLanuage( const char *pszLanguage, int nIndex);

BPLStatus OPL_LocalSystem_GetCountryID( char *pszID, unsigned int nLen);

BPLStatus OPL_LocalSystem_SetCountryID( const char *pszID);

BPLStatus OPL_LocalSystem_GetOperatorID( int *pnID);

BPLStatus OPL_LocalSystem_SetOperatorID( int nID);

BPLStatus OPL_LocalSystem_ExecuteCommand( int nID);

BPLStatus OPL_LocalSystem_ShowMsgbox( void);

BPLStatus OPL_LocalSystem_ShowKeyboard( int type, int pwmode, char *str);

BPLStatus OPL_LocalSystem_GetRegionID( int *pnRegionId);

BPLStatus OPL_LocalSystem_SetRegionID( int nRegionId);

BPLStatus OPL_LocalSystem_SetApplicationReady(void);

BPLStatus BPL_LocalSystem_GetAVOutputsCount( int *pnCount);

BPLStatus OPL_LocalSystem_GetChipId(char *pszID);

OplAVOutput_t *OPL_LocalSystem_GetAVOutputByIndex( int nIndex);

void OPL_LocalSystem_FreeAVOutput( OplAVOutput_t *ptOutput);

BPLStatus OPL_LocalSystem_GetNetInterfacesCount( int *pnCount);

BPLStatus OPL_LocalSystem_GetDefaultNetInterface(
							int *nIndex);
OplNetworkInterface_t *OPL_LocalSystem_GetNetInterfaceByIndex(
							int nIndex);
void	OPL_LocalSystem_FreeNetInterface(
							OplNetworkInterface_t *ptNetif);

BPLStatus OPL_LocalSystem_LoaderVersion( char *pszLoaderVer);

BPLStatus OPL_LocalSystem_UpgraderVersion( char *pszUpgraderVer);

BPLStatus OPL_LocalSystem_UpdateDate( char *pszUpdateDate);

BPLStatus OPL_LocalSystem_DoFctoryDefault( const char *Default_Values);

typedef void (*OplLoacalSystemCamStateChange_t)(unsigned char ucSlot, unsigned int ulState);
typedef void (*OplLoacalSystemSmartcardStateChange_t)(unsigned char ucSlot, unsigned int ulState);
typedef	void (*OplLocalSystemPowerStateChange_t)(int curState);
typedef	void (*OplLocalSystemPowerStateChangeError_t)(int nReason);
typedef void (*OplLoacalSystemUserActionIdle_t)(void);
typedef void (*OplLoacalSystemKeyboardResult_t)(char *data);
typedef void (*OplLocalSystemThermalProtection_t) (void);

typedef void (*OplLoacalSystemExtDevInfo_t)(int nDevType, int nNotiType, BPL_BOOL bConnected, unsigned int ulDeviceId, int nPortNum);

BPLStatus OPL_LocalSystem_GetPreferredMenuLanuage( char *pszLanguage);
BPLStatus OPL_LocalSystem_SetPreferredMenuLanuage( const char *pszLanguage);

BPLStatus OPL_LocalSystem_GetStandByPowerMode( HINT32 *pnStandByPowerMode);
BPLStatus OPL_LocalSystem_SetStandByPowerMode( HINT32 nStandByPowerMode);
BPLStatus OPL_LocalSystem_SetSleepModeOnStandby(HINT32 eSleepMode);
BPLStatus OPL_LocalSystem_GetSleepModeOnStandby(HINT32 *pSleepMode);
BPLStatus OPL_LocalSystem_SetBootModeOnStandby(HINT32	eBootMode);
BPLStatus OPL_LocalSystem_GetBootModeOnStandby(HINT32 *pBootMode);
BPLStatus OPL_LocalSystem_SetWifiPowerOnStandby(HINT32 bOnOff);
BPLStatus OPL_LocalSystem_GetWifiPowerOnStandby(HINT32 *pOnOff);
BPLStatus OPL_LocalSystem_setPowerStateChangeListener(OplLocalSystemPowerStateChange_t listener);
BPLStatus OPL_LocalSystem_setPowerStateChangeErrorListener(OplLocalSystemPowerStateChangeError_t listener);

BPLStatus OPL_LocalSystem_GetAutomaticStandby( BPL_BOOL *pbAutoStandby);
BPLStatus OPL_LocalSystem_SetAutomaticStandby( BPL_BOOL bAutoStandby);

BPLStatus OPL_LocalSystem_GetAutoDelete( BPL_BOOL *pbAutoDelete);
BPLStatus OPL_LocalSystem_SetAutoDelete( BPL_BOOL bAutoDelete);

BPLStatus OPL_LocalSystem_GetAudioBleep( BPL_BOOL *pbAdioBleep);
BPLStatus OPL_LocalSystem_SetAudioBleep( BPL_BOOL bAudioBleep);

BPLStatus OPL_LocalSystem_GetSubtitleFontType( unsigned int *pnSubtitleFontType);
BPLStatus OPL_LocalSystem_SetSubtitleFontType( unsigned int nSubtitleFontType);

BPLStatus OPL_LocalSystem_GetInfoDisplayTimeout( unsigned int *pnInfiDispTimeout);
BPLStatus OPL_LocalSystem_SetInfoDisplayTimeout( unsigned int nInfiDispTimeout);

BPLStatus OPL_LocalSystem_GetInstantReplayTime( unsigned int *pnInstantReplayTime);
BPLStatus OPL_LocalSystem_SetInstantReplayTime( unsigned int nInstantReplayTime);
BPLStatus OPL_LocalSystem_GetSkipForwardTime( unsigned int *pnSkipForwardTime);
BPLStatus OPL_LocalSystem_SetSkipForwardTime( unsigned int nSkipForwardTime);
BPLStatus OPL_LocalSystem_GetTimeshiftEnabled( BPL_BOOL *bTSR);
BPLStatus OPL_LocalSystem_SetTimeshiftEnabled( BPL_BOOL bTSR);
BPLStatus OPL_LocalSystem_GetFavChListNumberingMode( BPL_BOOL *pbFavChListNumberingMode);
BPLStatus OPL_LocalSystem_SetFavChListNumberingMode( BPL_BOOL bFavChListNumberingMode);
BPLStatus OPL_LocalSystem_GetDescrambleOnOff( BPL_BOOL *pbOnOff);
BPLStatus OPL_LocalSystem_SetDescrambleOnOff( BPL_BOOL bOnOff);
BPLStatus OPL_LocalSystem_GetFirstTimeBoot( BPL_BOOL *pbFirstTimeBoot);
BPLStatus OPL_LocalSystem_SetFirstTimeBoot( BPL_BOOL bFirstTimeBoot);

BPLStatus OPL_LocalSystem_GetStartPaddingTime( int *pnTime);
BPLStatus OPL_LocalSystem_SetStartPaddingTime( int nTime);
BPLStatus OPL_LocalSystem_GetEndPaddingTime( int *pnTime);
BPLStatus OPL_LocalSystem_SetEndPaddingTime( int nTime);

BPLStatus OPL_LocalSystem_SetField( const char *field, const char *value);
BPLStatus OPL_LocalSystem_GetField( const char *field, char *value, unsigned int n);
BPLStatus OPL_LocalSystem_GetChannelSetupPIN( BPL_BOOL *pbChannelSetupPin);
BPLStatus OPL_LocalSystem_SetChannelSetupPIN( BPL_BOOL bChannelSetupPin);
BPLStatus OPL_LocalSystem_SetGuidancePolicy(HUINT32 guidancePolicy);
BPLStatus OPL_LocalSystem_GetGuidancePolicy(HUINT32 *guidancePolicy);

// for HMS
BPLStatus OPL_LocalSystem_GetTargetStorage( int *pnStorage);
BPLStatus OPL_LocalSystem_SetTargetStorage( int nStorage);

// For Freesat
BPLStatus OPL_LocalSystem_SetPostcode( const char *pszPostcode);
BPLStatus OPL_LocalSystem_GetPostcode( char *pszPostcode, unsigned int ulLen);

// For SES
//BPLStatus OPL_LocalSystem_SetSatRecordConfirmCode( const char *pszRegistrationcode);
BPLStatus OPL_LocalSystem_GetSatRecordConfirmCode( const char * pszRegistrationCode, const char *pszConfirmCode);

/* Common Interface */
BPLStatus OPL_LocalSystem_GetCIPlusEnabled( char *pszEnabled);
BPLStatus OPL_LocalSystem_GetCIPlusVersion( char *pszVersion);
BPLStatus OPL_LocalSystem_GetCISlotCount( char *pszCount);
BPLStatus OPL_LocalSystem_GetCamNameBySlot( unsigned char ucSlot, char *pszCamName);
BPLStatus OPL_LocalSystem_SetCamStateChangeListener(OplLoacalSystemCamStateChange_t listener);
BPLStatus OPL_LocalSystem_UnsetCamStateChangeListener(void);
BPLStatus OPL_LocalSystem_GetHdmiCecMode( BPL_BOOL *pHdmiCecMode);
BPLStatus OPL_LocalSystem_SetHdmiCecMode( BPL_BOOL HdmiCecMode);
BPLStatus OPL_LocalSystem_GetHdmiEdidInfo( char *pszEdidInfo);
BPLStatus OPL_LocalSystem_GetHdmiCecVolumeCtrlMode( BPL_BOOL *pHdmiCecMode);
BPLStatus OPL_LocalSystem_SetHdmiCecVolumeCtrlMode( BPL_BOOL HdmiCecMode);

BPLStatus OPL_LocalSystem_RegisterThermalProtectionOccuredListener(OplLocalSystemThermalProtection_t listener);

BPLStatus OPL_LocalSystem_SetUserActionIdleListener(OplLoacalSystemUserActionIdle_t listener);
BPLStatus OPL_LocalSystem_UserActionIdle(void);
BPLStatus OPL_LocalSystem_SetKeyboardResultListener(OplLoacalSystemKeyboardResult_t listener);
BPLStatus OPL_LocalSystem_KeyboardResult(char *resultStr);
BPLStatus OPL_LocalSystem_setExtDevInfoListener(OplLoacalSystemExtDevInfo_t listener);

BPLStatus OPL_LocalSystem_SetSmartcardStateChangedListener(OplLoacalSystemSmartcardStateChange_t listener);


/* Irdeto */
typedef void (*OplLocalSystemIrdetoInfoChange_t)(unsigned char ucSlot, unsigned int ulState);
typedef void (*OplLocalSystemIrdetoImportExport_t)(unsigned int type, unsigned int result);

/* Nagra */
typedef void (*OplLocalSystemNagraInfoChange_t)(unsigned char ucSlot, unsigned int ulState);
typedef void (*OplLocalSystemNagraSCState_t)(unsigned char ucSlot, unsigned int ulState);

/* LCN UPDATE */
typedef void (*OplLocalSystemLcnUpdate_t)(void *pstLcnInform);

/* Cold boot start -Me */
typedef void (*OplLocalSystemColdBootDone_t) (void);

typedef void (*OplLocalSystemCasActionIrEmmForcedOta_t) (unsigned int ulParam1, unsigned int ulParam2, unsigned int ulParam3);

/* Clock Recovery listener */
typedef void (*OplLocalClockRecovery_t) (unsigned int batchType, unsigned int batchProcType, unsigned int err);
typedef void (*OplLocalSystemTrdConflict_t)(void *pstTrdConflict);
typedef void (*OplLocalSystemNcdInfo_t)(void *pstNcdInfo);
BPLStatus OPL_HMXTrdConflictInfo(OplLocalSystemTrdConflict_t listener,unsigned int depth);
BPLStatus OPL_HMXSetTrdConflictInfo(unsigned int depth, unsigned int index);
BPLStatus OPL_HMXOperatorFeauture_GetNcdInfo(OplLocalSystemNcdInfo_t listener,unsigned int param);
BPLStatus OPL_HMXOperatorFeauture_SetNcdNotifyData(HUINT32 ulASONotifyFlag);

BPLStatus OPL_HMXIrdetoImportDBToUsb(OplLocalSystemIrdetoImportExport_t listener, BPL_BOOL bSync);
BPLStatus OPL_HMXIrdetoExportDBToUsb(OplLocalSystemIrdetoImportExport_t listener, BPL_BOOL bSync);
BPLStatus OPL_HMXColdBoot_GetColdBootDone(BPL_BOOL *pbColdBootDone, OplLocalSystemColdBootDone_t  listener);
BPLStatus OPL_HMXColdBoot_UnregisterNotifier(void);
BPLStatus OPL_HMXStartLcnUpdate(OplLocalSystemLcnUpdate_t listener, int searchMode);
BPLStatus OPL_HMXStopLcnUpdate(OplLocalSystemLcnUpdate_t listener);
BPLStatus OPL_HMXSaveLcnUpdate(OplLocalSystemLcnUpdate_t listener, int searchMode, int index);
BPLStatus OPL_HMXGetLcnUpdateInfo(OplLocalSystemLcnUpdate_t listener);
BPLStatus OPL_HMXRemoveLcnUpdateInfo(BPL_BOOL syncMode);
BPLStatus OPL_HMXClockRecovery(OplLocalClockRecovery_t listener);
BPLStatus OPL_HMXCasActionIrEmmForcedOta_RegisterNotifier(OplLocalSystemCasActionIrEmmForcedOta_t  listener);
BPLStatus OPL_HMXCasActionIrEmmForcedOta_UnregisterNotifier(OplLocalSystemCasActionIrEmmForcedOta_t  listener);
BPLStatus OPL_LocalSystem_SetIrdetoVirtualSecureCW( BPL_BOOL bVirtualSecureCW);
BPLStatus OPL_LocalSystem_GetIrdetoVirtualSecureCwMode(BPL_BOOL *pbCwMode);
BPLStatus OPL_LocalSystem_GetIrdetoClientVersion( char *pszVersion);
BPLStatus OPL_LocalSystem_GetIrdetoCWE(BPL_BOOL *pbCWE);
BPLStatus OPL_LocalSystem_GetIrdetoJTAG(BPL_BOOL *pbJTAG);
BPLStatus OPL_LocalSystem_GetIrdetoSBOOT(BPL_BOOL *pbSBOOT);
BPLStatus OPL_LocalSystem_GetIrdetoOTP(BPL_BOOL *pbOTP);
BPLStatus OPL_LocalSystem_GetIrdetoManCode( char *pszMancode);
BPLStatus OPL_LocalSystem_GetIrdetoHwCode( char *pszHWCode);
BPLStatus OPL_LocalSystem_GetIrdetoVariant( char *pszVariant);
BPLStatus OPL_LocalSystem_GetIrdetoSysId( char *pszSysId);
BPLStatus OPL_LocalSystem_GetIrdetoKeyVer( char *pszKeyVer);
BPLStatus OPL_LocalSystem_GetIrdetoSigVer( char *pszSigVer);
BPLStatus OPL_LocalSystem_GetControlByte(int *pControlByte);
BPLStatus OPL_LocalSystem_GetPrivateData(char *pszPrivateData);

BPLStatus OPL_LocalSystem_SetIrdetoInfoChangeListener(OplLocalSystemIrdetoInfoChange_t listener);
BPLStatus OPL_LocalSystem_UnsetIrdetoInfoChangeListener(void);

BPLStatus OPL_LocalSystem_GetNagraCWE(BPL_BOOL *pbCWE);
BPLStatus OPL_LocalSystem_GetNagraJTAG(BPL_BOOL *pbJTAG);
BPLStatus OPL_LocalSystem_GetNagraSBOOT(BPL_BOOL *pbSBOOT);
BPLStatus OPL_LocalSystem_GetNagraOTP(BPL_BOOL *pbOTP);
BPLStatus OPL_LocalSystem_GetNagraNUID( char *pszNUID);
BPLStatus OPL_LocalSystem_GetNagraCASN( char *pszCASN);
BPLStatus OPL_LocalSystem_NagraBoxFusingInfo(BPL_BOOL *IsSCS, BPL_BOOL *IsJTAG, BPL_BOOL *IsOTP, BPL_BOOL *IsCWE);

BPLStatus OPL_LocalSystem_SetNagraInfoChangeListener(OplLocalSystemNagraInfoChange_t listener);
BPLStatus OPL_LocalSystem_SetNagraSCStateListener(OplLocalSystemNagraSCState_t listener);

BPLStatus OPL_LocalSystem_GetNetworkIds(char *pszNetworkId, unsigned int ulLen);

/* URL */
BPLStatus OPL_LocalSystem_GetRpURL( char *pszUrl, unsigned int ulLen);
BPLStatus OPL_LocalSystem_GetPortalURL( char *pszUrl, unsigned int ulLen);
BPLStatus OPL_LocalSystem_GetIpOtaURL( char *pszUrl, unsigned int ulLen);
BPLStatus OPL_LocalSystem_GetHbbtvURL( char *pszUrl, unsigned int ulLen);
BPLStatus OPL_LocalSystem_CheckFileExist(char *pszUri, BPL_BOOL *IsExist);

/* OIPF Power Control */

BPLStatus OPL_LocalSystem_PowerControl( int OIPFValue);
BPLStatus OPL_LocalSystem_GetOipfPowerState( int *pnOipfPowerState);
BPLStatus OPL_LocalSystem_GettimeCurrentPowerState( int *pnOipfTimePowerState);
BPLStatus OPL_LocalSystem_SetOipfPowerState( int nOipfPowerState);
BPLStatus OPL_LocalSystem_GetPreviousOipfPowerState( int *pnOipfPreviousPowerState);

/* First Boot Status */
BPLStatus OPL_LocalSystem_SetFirstBootStatusOnProduction(BPL_BOOL IsFirstBoot);
BPLStatus OPL_LocalSystem_GetFirstBootStatusOnProduction(BPL_BOOL *IsFristBoot);
BPLStatus OPL_LocalSystem_GetLaunchAppMode(HUINT32 *pulLaunchAppMode);
BPLStatus OPL_LocalSystem_GetFusingInfo(BPL_BOOL *IsSCS, BPL_BOOL *IsJTAG, BPL_BOOL *IsOTP, BPL_BOOL *IsCWE, unsigned int *pulMarketID);
BPLStatus OPL_LocalSystem_GetFusingInfoExt(BPL_BOOL *IsSCS, BPL_BOOL *IsJTAG, BPL_BOOL *IsOTP, BPL_BOOL *IsCWE, unsigned int* pulOTPField1, unsigned int* pulOTPField2);

/* 지상파 antenna power */
BPLStatus OPL_LocalSystem_SetAntennaPower(BPL_BOOL bAntPower);
BPLStatus OPL_LocalSystem_GetAntennaPower(BPL_BOOL *pbAntPower);

/* Dynamic Update flag */
BPLStatus OPL_LocalSystem_SetServiceUpdateFlag(BPL_BOOL bOnOff);
BPLStatus OPL_LocalSystem_GetServiceUpdateFlag(BPL_BOOL *pbOnOff);
BPLStatus OPL_LocalSystem_GetNandBadBlocks(unsigned int *pulCntLdr, unsigned int *pulCntUBI, unsigned int *pulCntUBIFS);
BPLStatus OPL_LocalSystem_GetHCRData(char **pszbuffer);
BPLStatus OPL_LocalSystem_CheckHCRData();

/* HBBTV */
BPLStatus OPL_LocalSystem_SetHbbTvEnabled(BPL_BOOL bEanbled);
BPLStatus OPL_LocalSystem_GetHbbTvEnabled(BPL_BOOL *pbEnabled);

/* Freeview Connected (FVC)*/
BPLStatus OPL_LocalSystem_GetFvcNetworkUriPrefix(char *pszUrl, unsigned int ulLen);
BPLStatus OPL_LocalSystem_GetFvcBaseURL(char *pszUrl, unsigned int ulLen);
BPLStatus OPL_LocalSystem_SetFvcBaseURL(const char *pszUrl);
BPLStatus OPL_LocalSystem_GetFvcAuthHash(char *str, unsigned int ulLen);
BPLStatus OPL_LocalSystem_SetFvcAuthHash(const char *str);
BPLStatus OPL_LocalSystem_GetFvcAuthTimeStamp(char *str, unsigned int ulLen);
BPLStatus OPL_LocalSystem_SetFvcAuthTimeStamp(const char *str);

/* NTP */
BPLStatus OPL_LocalSystem_SetNtpServerURL(const char *pszUrl);

/* MyHD BAT Update */
BPLStatus OPL_LocalSystem_GetBatUpdateMode(HINT32 *pnBatUpdateMode);
BPLStatus OPL_LocalSystem_SetBatUpdateMode(HINT32 nBatUpdateMode);
/* MyHD NIT LCN Update */
BPLStatus OPL_LocalSystem_GetNitLcnUpdateMode(HINT32 *pnNitLcnUpdateMode);
BPLStatus OPL_LocalSystem_SetNitLcnUpdateMode(HINT32 nNitLcnUpdateMode);

/* Corsair Debug */
BPLStatus OPL_LocalSystem_getOipfGrab(int *value);


BPLStatus OPL_LocalSystem_GetLastServiceGroup( int *pnLastSvcGroup);
BPLStatus OPL_LocalSystem_SetLastServiceGroup( int nLastSvcGroup);

BPLStatus OPL_LocalSystem_GetAutomaticPowerdown(unsigned int *pnTimeout);
BPLStatus OPL_LocalSystem_SetAutomaticPowerdown(unsigned int nTimeout);

BPLStatus OPL_LocalSystem_GetPriorityHDMIOutput(unsigned int *priority_outputs);
BPLStatus OPL_LocalSystem_SetPriorityHDMIOutput(unsigned int priority_outputs);

BPLStatus OPL_LocalSystem_GetVerimatrixLibVersion(eOplVerimatirxLibType type, char * version, unsigned int max_len);

/* Netflix ESN ID */
BPLStatus OPL_LocalSystem_GetNflixESN( char *pszNetflixEsn);
#ifdef __cplusplus
};

/******************************************************************
	class OplLocalSystem
******************************************************************/
class OplLocalSystem {

public:
	OplLocalSystem()
	{
	}
	virtual ~OplLocalSystem()
	{
	}

	static bool getDeviceID(char *pszDevID)
	{ return OPL_LocalSystem_GetDeviceID( pszDevID) == BPL_STATUS_OK; }

	static bool getSystemReady(void)
	{
		BPL_BOOL bReady = FALSE;
		OPL_LocalSystem_GetSystemReady( &bReady);
		return bReady == TRUE;
	}

	static bool getVendorName(char *pszName)
	{ return OPL_LocalSystem_GetVendorName( pszName) == BPL_STATUS_OK; }

	static bool getMicomVersion(char *pszVersion)
	{ return OPL_LocalSystem_GetMicomVersion( pszVersion) == BPL_STATUS_OK; }

	static bool getRcuCustomCode(int *pnCustomCode)
	{ return OPL_LocalSystem_GetRemoteCustomCode(pnCustomCode) == BPL_STATUS_OK; }

	static bool getWakeupReason(char *pszWakeupReason)
	{ return OPL_LocalSystem_GetWakeupReason(pszWakeupReason) == BPL_STATUS_OK; }

	static bool getModelName(char *pszName)
	{ return OPL_LocalSystem_GetModelName( pszName) == BPL_STATUS_OK; }

	static bool getUpdatedList(char *pszList)
	{ return OPL_LocalSystem_GetUpdatedList( pszList) == BPL_STATUS_OK; }

	static bool getChipId(char *pszID)
	{ return OPL_LocalSystem_GetChipId( pszID) == BPL_STATUS_OK; }

	static bool getSoftwareVersion(char *pszVersion)
	{ return OPL_LocalSystem_GetSoftwareVersion( pszVersion) == BPL_STATUS_OK; }

	static bool getPackageVersion(char *pszVersion)
	{ return OPL_LocalSystem_GetPackageVersion( pszVersion) == BPL_STATUS_OK; }

	static bool getHardwareVersion(char *pszVersion)
	{ return OPL_LocalSystem_GetHardwareVersion( pszVersion) == BPL_STATUS_OK; }

	static bool getSerialNumber(char *pszNumber)
	{ return OPL_LocalSystem_GetSerialNumber( pszNumber) == BPL_STATUS_OK; }

	static bool getPvrEnabled(void)
	{
		BPL_BOOL bEnabled = FALSE;
		OPL_LocalSystem_GetPvrEnabled(&bEnabled);
		return bEnabled == TRUE;
	}

	static bool getInternalHddEnabled(void)
	{
		BPL_BOOL bEnabled = FALSE;
		OPL_LocalSystem_GetInternalHddEnabled(&bEnabled);
		return bEnabled == TRUE;
	}

	static bool getVolume(unsigned int *pnVolume)
	{ return OPL_LocalSystem_GetVolume( pnVolume) == BPL_STATUS_OK; }

	static bool setVolume(unsigned int nVolume)
	{ return OPL_LocalSystem_SetVolume( nVolume) == BPL_STATUS_OK; }

	static bool getMute(void)
	{
		BPL_BOOL bMute= FALSE;
		OPL_LocalSystem_GetMute( &bMute);
		return bMute == TRUE;
	}

	static bool setMute(BPL_BOOL bMute)
	{ return OPL_LocalSystem_SetMute( bMute) == BPL_STATUS_OK; }

	static bool getTVStandard(eOplTVStandard *eStandard)
	{ return OPL_LocalSystem_GetTVStandard( eStandard) == BPL_STATUS_OK; }

	static bool getSystemID(char *pszID)
	{ return OPL_LocalSystem_GetSystemID( pszID) == BPL_STATUS_OK; }

	static bool getScreenLanguage(char *pszLanguage)
	{ return OPL_LocalSystem_GetScreenLanguage( pszLanguage) == BPL_STATUS_OK; }

	static bool setScreenLanguage(const char *pszLanguage)
	{ return OPL_LocalSystem_SetScreenLanguage( pszLanguage) == BPL_STATUS_OK; }

	static bool getPreferredAudioLanguage(char *pszLanguage, int nIndex)
	{ return OPL_LocalSystem_GetPreferredAudioLanuage( pszLanguage, nIndex)\
			 == BPL_STATUS_OK; }

	static bool setPreferredAudioLanguage(const char *pszLanguage, int nIndex)
	{ return OPL_LocalSystem_SetPreferredAudioLanuage( pszLanguage, nIndex)\
			 == BPL_STATUS_OK; }

	static bool getPreferredSubtitleLanguage(char *pszLanguage, int nIndex)
	{ return OPL_LocalSystem_GetPreferredSubtitleLanuage( pszLanguage, nIndex)\
			 == BPL_STATUS_OK; }

	static bool setPreferredSubtitleLanguage(const char *pszLanguage, int nIndex)
	{ return OPL_LocalSystem_SetPreferredSubtitleLanuage( pszLanguage, nIndex)\
			 == BPL_STATUS_OK; }

	static bool getCountryID(char *pszID,unsigned int nLen)
	{ return OPL_LocalSystem_GetCountryID( pszID, nLen) == BPL_STATUS_OK; }

	static bool setCountryID(const char *pszID)
	{ return OPL_LocalSystem_SetCountryID( pszID) == BPL_STATUS_OK; }

	static bool getOperatorID(int *pnID)
	{ return OPL_LocalSystem_GetOperatorID( pnID) == BPL_STATUS_OK; }

	static bool setOperatorID(int nID)
	{ return OPL_LocalSystem_SetOperatorID( nID) == BPL_STATUS_OK; }

	static bool getRegionID(int *pnRegionId)
	{ return OPL_LocalSystem_GetRegionID( pnRegionId) == BPL_STATUS_OK; }

	static bool setRegionID(int nRegionId)
	{ return OPL_LocalSystem_SetRegionID( nRegionId) == BPL_STATUS_OK; }

	static bool getLoaderVersion(char *pszLoaderVer)
	{ return OPL_LocalSystem_LoaderVersion( pszLoaderVer) == BPL_STATUS_OK; }

	static bool getUpgraderVersion(char *pszUpgraderVer)
	{ return OPL_LocalSystem_UpgraderVersion( pszUpgraderVer) == BPL_STATUS_OK; }

	static bool getUpdateDate(char *pszUpdateDate)
	{ return OPL_LocalSystem_UpdateDate( pszUpdateDate) == BPL_STATUS_OK; }

	static bool getStartPaddingTime(int *pnTime)
	{ return OPL_LocalSystem_GetStartPaddingTime( pnTime)\
			 == BPL_STATUS_OK; }

	static bool setStartPaddingTime(int nTime)
	{ return OPL_LocalSystem_SetStartPaddingTime( nTime)\
			 == BPL_STATUS_OK; }

	static bool getEndPaddingTime(int *pnTime)
	{ return OPL_LocalSystem_GetEndPaddingTime( pnTime)\
			 == BPL_STATUS_OK; }

	static bool setEndPaddingTime(int nTime)
	{ return OPL_LocalSystem_SetEndPaddingTime( nTime)\
			 == BPL_STATUS_OK; }

	static bool getTargetStorage(int *pnStorage)
	{ return OPL_LocalSystem_GetTargetStorage( pnStorage)\
			 == BPL_STATUS_OK; }

	static bool setTargetStorage(int nStorage)
	{ return OPL_LocalSystem_SetTargetStorage( nStorage)\
			 == BPL_STATUS_OK; }

	static bool setApplicationReady(void)
	{ return OPL_LocalSystem_SetApplicationReady()\
			 == BPL_STATUS_OK; }

	static bool setHbbTvEnabled(BPL_BOOL bEnabled)
	{ return OPL_LocalSystem_SetHbbTvEnabled( bEnabled)\
			 == BPL_STATUS_OK; }

	static bool getHbbTvEnabled(BPL_BOOL *pbEnabled)
	{ return OPL_LocalSystem_GetHbbTvEnabled( pbEnabled)\
			 == BPL_STATUS_OK; }

	 static bool setLastServiceGroup(int nLastSvcGroup)
	{ return OPL_LocalSystem_SetLastServiceGroup( nLastSvcGroup)\
			 == BPL_STATUS_OK; }

	static bool getLastServiceGroup(int *pnLastSvcGroup)
	{ return OPL_LocalSystem_GetLastServiceGroup( pnLastSvcGroup)\
			 == BPL_STATUS_OK; }

	static bool setAutomaticPowerdown(unsigned int nTimeout)
	{ return OPL_LocalSystem_SetAutomaticPowerdown(nTimeout)\
			 == BPL_STATUS_OK; }

	static bool getAutomaticPowerdown(unsigned  int *pnTimeout)
	{ return OPL_LocalSystem_GetAutomaticPowerdown(pnTimeout)\
			 == BPL_STATUS_OK; }

	static bool setPriorityHDMIOutputs(unsigned int priority_outputs)
	{ return OPL_LocalSystem_SetPriorityHDMIOutput(priority_outputs)\
			 == BPL_STATUS_OK; }

	static bool getPriorityHDMIOutputs(unsigned int *priority_outputs)
	{ return OPL_LocalSystem_GetPriorityHDMIOutput(priority_outputs)\
				 == BPL_STATUS_OK; }

	static bool getVerimatrixLibVersion(eOplVerimatirxLibType type, char * version, unsigned int max_len)
	{ return OPL_LocalSystem_GetVerimatrixLibVersion(type, version, max_len) == BPL_STATUS_OK; }

	static bool getNflixESN(char *pszNetflixEsn)
	{ return OPL_LocalSystem_GetNflixESN( pszNetflixEsn) == BPL_STATUS_OK; }

	static int getAVOutputsCount(void);
	static OplAVOutput *getAVOutputByIndex(int nIndex);

	static int getNetworkInterfacesCount(void);
	static OplNetworkInterface *getNetworkInterfaceByIndex(int nIndex);

#if defined(CONFIG_OP_JAPAN)
	static int getNetworkServiceCount(void);
	static OplNetworkService *getNetworkService(int index);
	static int getWiFiAccessPointCount(void);
	static OplNetworkAccessPoint *getWiFiAccessPoint(int index);
#endif

	static bool startTVBrowser(const char *exInfo) { return true; };
	static void notifyToStopTVBrowser();
	static bool getStatusOfTVBrowser() { return true; };

	/* Common Interface */
	static bool getCIPlusEnabled(char *pszEnabled)
	{ return OPL_LocalSystem_GetCIPlusEnabled( pszEnabled) == BPL_STATUS_OK; }
	static bool getCIPlusVersion(char *pszVersion)
	{ return OPL_LocalSystem_GetCIPlusVersion( pszVersion) == BPL_STATUS_OK; }
	static bool getCISlotCount(char *pszCount)
	{ return OPL_LocalSystem_GetCISlotCount( pszCount) == BPL_STATUS_OK; }
	static bool getCICamNameBySlot(unsigned char ucSlot, char *pszCamName)
	{ return OPL_LocalSystem_GetCamNameBySlot( ucSlot, pszCamName) == BPL_STATUS_OK; }
	static void setCamStateChangeListener(OplLoacalSystemCamStateChange_t listener)
	{ OPL_LocalSystem_SetCamStateChangeListener(listener);}
	static void unsetCamStateChangeListener(void)
	{ OPL_LocalSystem_UnsetCamStateChangeListener();}
	static void setSmartcardStateChangedListener(OplLoacalSystemSmartcardStateChange_t listener)
	{ OPL_LocalSystem_SetSmartcardStateChangedListener(listener);}
	static void setThermalProtectionOccuredListener(OplLocalSystemThermalProtection_t listener)
	{ OPL_LocalSystem_RegisterThermalProtectionOccuredListener(listener);}

	static void setUserActionIdleListener(OplLoacalSystemUserActionIdle_t listener)
	{ OPL_LocalSystem_SetUserActionIdleListener(listener);}

	static void userActionIdle(void)
	{ OPL_LocalSystem_UserActionIdle();}

	static void setKeyboardResultListener(OplLoacalSystemKeyboardResult_t listener)
	{ OPL_LocalSystem_SetKeyboardResultListener(listener);}

	static void keyboardResult(char *data)
	{ OPL_LocalSystem_KeyboardResult(data);}

	static void setExtDevInfoListener(OplLoacalSystemExtDevInfo_t listener)
	{ OPL_LocalSystem_setExtDevInfoListener(listener); }

	/* Irdeto */
	static bool setIrdetoVirtualSecureCW(BPL_BOOL bVirtualSecureCW)
	{ return OPL_LocalSystem_SetIrdetoVirtualSecureCW( bVirtualSecureCW) == BPL_STATUS_OK; }
	static bool getIrdetoVirtualSecureCW(BPL_BOOL *bVirtualSecureCW)
	{ return OPL_LocalSystem_GetIrdetoVirtualSecureCwMode( bVirtualSecureCW) == BPL_STATUS_OK; }
	static bool getIrdetoClientVersion(char *pszVersion)
	{ return OPL_LocalSystem_GetIrdetoClientVersion( pszVersion) == BPL_STATUS_OK; }
	static bool getIrdetoCWE(BPL_BOOL *pbCwe)
	{ return OPL_LocalSystem_GetIrdetoCWE( pbCwe) == BPL_STATUS_OK; }
	static bool getIrdetoJTAG(BPL_BOOL *pbJtag)
	{ return OPL_LocalSystem_GetIrdetoJTAG( pbJtag) == BPL_STATUS_OK; }
	static bool getIrdetoSBOOT(BPL_BOOL *pbSboot)
	{ return OPL_LocalSystem_GetIrdetoSBOOT( pbSboot) == BPL_STATUS_OK; }
	static bool getIrdetoOTP(BPL_BOOL *pbOtp)
	{ return OPL_LocalSystem_GetIrdetoOTP( pbOtp) == BPL_STATUS_OK; }
	static bool getIrdetoManCode(char *pszMancode)
	{ return OPL_LocalSystem_GetIrdetoManCode( pszMancode) == BPL_STATUS_OK; }
	static bool getIrdetoHwCode(char *pszHWCode)
	{ return OPL_LocalSystem_GetIrdetoHwCode( pszHWCode) == BPL_STATUS_OK; }
	static bool getIrdetoVariant(char *pszVariant)
	{ return OPL_LocalSystem_GetIrdetoVariant( pszVariant) == BPL_STATUS_OK; }
	static bool getIrdetoSysId(char *pszSysId)
	{ return OPL_LocalSystem_GetIrdetoSysId( pszSysId) == BPL_STATUS_OK; }
	static bool getIrdetoKeyVer(char *pszKeyVer)
	{ return OPL_LocalSystem_GetIrdetoKeyVer( pszKeyVer) == BPL_STATUS_OK; }
	static bool getIrdetoSigVer(char *pszSigVer)
	{ return OPL_LocalSystem_GetIrdetoSigVer( pszSigVer) == BPL_STATUS_OK; }
	static bool getIrdetoControlByte(int *pszControlByte)
	{ return OPL_LocalSystem_GetControlByte(pszControlByte) == BPL_STATUS_OK; }
	static bool getIrdetoPrivateData(char *pszPrivateData)
	{ return OPL_LocalSystem_GetPrivateData(pszPrivateData) == BPL_STATUS_OK; }

	static void setIrdetoInfoChangeListener(OplLoacalSystemCamStateChange_t listener)
	{ OPL_LocalSystem_SetIrdetoInfoChangeListener(listener);}
	static void unsetIrdetoInfoChangeListener(void)
	{ OPL_LocalSystem_UnsetIrdetoInfoChangeListener();}

	/* Nagra */
	static bool getNagraCWE(BPL_BOOL *pbCwe)
	{ return OPL_LocalSystem_GetNagraCWE( pbCwe) == BPL_STATUS_OK; }
	static bool getNagraJTAG(BPL_BOOL *pbJtag)
	{ return OPL_LocalSystem_GetNagraJTAG( pbJtag) == BPL_STATUS_OK; }
	static bool getNagraSBOOT(BPL_BOOL *pbSboot)
	{ return OPL_LocalSystem_GetNagraSBOOT( pbSboot) == BPL_STATUS_OK; }
	static bool getNagraOTP(BPL_BOOL *pbOtp)
	{ return OPL_LocalSystem_GetNagraOTP( pbOtp) == BPL_STATUS_OK; }
	static bool getNagraNUID(char *pszNUID)
	{ return OPL_LocalSystem_GetNagraNUID( pszNUID) == BPL_STATUS_OK; }
	static bool getNagraCASN(char *pszCASN)
	{ return OPL_LocalSystem_GetNagraCASN( pszCASN) == BPL_STATUS_OK; }
	static bool getNagraBoxFusingInfo(BPL_BOOL *IsSCS, BPL_BOOL *IsJTAG, BPL_BOOL *IsOTP, BPL_BOOL *IsCWE)
	{ return OPL_LocalSystem_NagraBoxFusingInfo(IsSCS, IsJTAG, IsOTP, IsCWE ) == BPL_STATUS_OK; }

	static void setNagraInfoChangeListener(OplLoacalSystemCamStateChange_t listener)
	{ OPL_LocalSystem_SetNagraInfoChangeListener(listener);}

	static bool getPreferredMenuLanguage(char *pszLanguage)
	{ return OPL_LocalSystem_GetPreferredMenuLanuage( pszLanguage)\
			 == BPL_STATUS_OK; }

	static bool setPreferredMenuLanguage(const char *pszLanguage)
	{ return OPL_LocalSystem_SetPreferredMenuLanuage( pszLanguage)\
			 == BPL_STATUS_OK; }

	static bool getAutomaticStandby(void)
	{
		BPL_BOOL bAutoStandby = FALSE;
		OPL_LocalSystem_GetAutomaticStandby( &bAutoStandby);
		return bAutoStandby == TRUE;
	}

	static bool setAutomaticStandby(BPL_BOOL bAutoStandby)
	{ return OPL_LocalSystem_SetAutomaticStandby( bAutoStandby) == BPL_STATUS_OK; }

	static bool getAutoDelete(void)
	{
		BPL_BOOL bAutoDelete = FALSE;
		OPL_LocalSystem_GetAutoDelete( &bAutoDelete);
		return bAutoDelete == TRUE;
	}

	static bool setAutoDelete(BPL_BOOL bAutoDelete)
	{ return OPL_LocalSystem_SetAutoDelete( bAutoDelete) == BPL_STATUS_OK; }

	static bool getAudioBleep(void)
	{
		BPL_BOOL bAdioBleep = FALSE;
		OPL_LocalSystem_GetAudioBleep( &bAdioBleep);
		return bAdioBleep == TRUE;
	}

	static bool setAudioBleep(BPL_BOOL bAdioBleep)
	{ return OPL_LocalSystem_SetAudioBleep( bAdioBleep) == BPL_STATUS_OK; }

	static bool getSubtitleFontType(unsigned int *pnSubtitleFontType)
	{ return OPL_LocalSystem_GetSubtitleFontType( pnSubtitleFontType) == BPL_STATUS_OK; }

	static bool setSubtitleFontType(unsigned int nSubtitleFontType)
	{ return OPL_LocalSystem_SetSubtitleFontType( nSubtitleFontType) == BPL_STATUS_OK; }

	static bool getInfoDisplayTimeout(unsigned int *pnInfiDispTimeout)
	{ return OPL_LocalSystem_GetInfoDisplayTimeout( pnInfiDispTimeout) == BPL_STATUS_OK; }

	static bool setInfoDisplayTimeout(unsigned int nInfiDispTimeout)
	{ return OPL_LocalSystem_SetInfoDisplayTimeout( nInfiDispTimeout) == BPL_STATUS_OK; }

	static bool getInstantReplayTime(unsigned int *pnInstantReplayTime)
	{ return OPL_LocalSystem_GetInstantReplayTime( pnInstantReplayTime) == BPL_STATUS_OK; }

	static bool setInstantReplayTime(unsigned int nInstantReplayTime)
	{ return OPL_LocalSystem_SetInstantReplayTime( nInstantReplayTime) == BPL_STATUS_OK; }

	static bool getSkipForwardTime(unsigned int *pnSkipForwardTime)
	{ return OPL_LocalSystem_GetSkipForwardTime( pnSkipForwardTime) == BPL_STATUS_OK; }

	static bool setSkipForwardTime(unsigned int nSkipForwardTime)
	{ return OPL_LocalSystem_SetSkipForwardTime( nSkipForwardTime) == BPL_STATUS_OK; }

	static bool getFirstTimeBoot(BPL_BOOL *pbFirstTimeBoot)
	{ return OPL_LocalSystem_GetFirstTimeBoot( pbFirstTimeBoot) == BPL_STATUS_OK; }

	static bool setFirstTimeBoot(BPL_BOOL bFirstTimeBoot)
	{ return OPL_LocalSystem_SetFirstTimeBoot( bFirstTimeBoot) == BPL_STATUS_OK; }

	static bool getTimeshiftEnabled(void)
	{
		BPL_BOOL bTSR = FALSE;
		OPL_LocalSystem_GetTimeshiftEnabled( &bTSR);
		return bTSR == TRUE;
	}

	static bool setTimeshiftEnabled(BPL_BOOL bTSR)
	{ return OPL_LocalSystem_SetTimeshiftEnabled( bTSR) == BPL_STATUS_OK; }

	static bool getFavChListNumberingMode(void)
	{
		BPL_BOOL bMode = FALSE;
		OPL_LocalSystem_GetFavChListNumberingMode( &bMode);
		return bMode == TRUE;
	}

	static bool setFavChListNumberingMode(BPL_BOOL bFavChListNumberingMode)
	{ return OPL_LocalSystem_SetFavChListNumberingMode( bFavChListNumberingMode) == BPL_STATUS_OK; }

	static bool getDescrambleOnOff(void)
	{
		BPL_BOOL bOnOff = FALSE;
		OPL_LocalSystem_GetDescrambleOnOff( &bOnOff);
		return bOnOff == TRUE;
	}

	static bool setDescrambleOnOff(BPL_BOOL bOnOff)
	{ return OPL_LocalSystem_SetDescrambleOnOff( bOnOff) == BPL_STATUS_OK; }

	static void doFactoryDefault(const char *Default_Values)
	{
		OPL_LocalSystem_DoFctoryDefault(Default_Values);
	}

	static bool setField (const char *field, const char *value)
	{ return OPL_LocalSystem_SetField( field, value) == BPL_STATUS_OK; }

	static bool getField (const char *field, char *value, unsigned int n)
	{ return OPL_LocalSystem_GetField( field, value, n) == BPL_STATUS_OK; }

	/* Freesat Extention */
	static bool getPostcode(char *pszPostcode, unsigned int nLen)
	{ return OPL_LocalSystem_GetPostcode( pszPostcode, nLen) == BPL_STATUS_OK; }

	static bool setPostcode(const char *pszPostcode)
	{ return OPL_LocalSystem_SetPostcode( pszPostcode) == BPL_STATUS_OK; }

//#if defined(CONFIG_OP_SES)
	/* SES Extention */
	static bool getSatRecordConfirmCode(const char *pszRegistrationCode, const char *pszConfirmCode)
	{ return OPL_LocalSystem_GetSatRecordConfirmCode( pszRegistrationCode, pszConfirmCode) == BPL_STATUS_OK; }
/*
	static bool setSatRecordConfirmCode(const char *pszRegistrationcode)
	{ return OPL_LocalSystem_SetSatRecordConfirmCode( pszRegistrationcode) == BPL_STATUS_OK; }
*/
//#endif


	static bool setChannelSetupPIN(BPL_BOOL bChannelChangePIN)
	{ return OPL_LocalSystem_SetChannelSetupPIN( bChannelChangePIN) == BPL_STATUS_OK; }

	static bool getChannelSetupPIN(void)
	{
		BPL_BOOL bChannelChangePIN = FALSE;
		OPL_LocalSystem_GetChannelSetupPIN( &bChannelChangePIN);
		return bChannelChangePIN == TRUE;
	}
	static bool getGuidancePolicy(unsigned int *pnGuidancePolicy)
	{ return OPL_LocalSystem_GetGuidancePolicy( pnGuidancePolicy) == BPL_STATUS_OK; }

	static bool setGuidancePolicy(unsigned int nGuidancePolicy)
	{ return OPL_LocalSystem_SetGuidancePolicy( nGuidancePolicy) == BPL_STATUS_OK; }

	static bool getHdmiCecMode( BPL_BOOL *pHdmiCecMode)
	{ return OPL_LocalSystem_GetHdmiCecMode( pHdmiCecMode) == BPL_STATUS_OK; }

	static bool setHdmiCecMode( BPL_BOOL HdmiCecMode)
	{ return OPL_LocalSystem_SetHdmiCecMode( HdmiCecMode) == BPL_STATUS_OK; }

	static bool getHdmiEdidInfo( char *pszEdidInfo)
	{ return OPL_LocalSystem_GetHdmiEdidInfo( pszEdidInfo) == BPL_STATUS_OK; }

	static bool getHdmiCecVolumeCtrlMode( BPL_BOOL *pHdmiCecVolMode)
	{ return OPL_LocalSystem_GetHdmiCecVolumeCtrlMode( pHdmiCecVolMode) == BPL_STATUS_OK; }

	static bool setHdmiCecVolumeCtrlMode( BPL_BOOL HdmiCecVolMode)
	{ return OPL_LocalSystem_SetHdmiCecVolumeCtrlMode( HdmiCecVolMode) == BPL_STATUS_OK; }

	static bool getRpURL(char *pszUrl, unsigned int ulLen)
	{ return OPL_LocalSystem_GetRpURL(pszUrl, ulLen) == BPL_STATUS_OK; }

	static bool getPortalURL(char *pszUrl, unsigned int ulLen)
	{ return OPL_LocalSystem_GetPortalURL(pszUrl, ulLen) == BPL_STATUS_OK; }

	static bool getIpOtaURL(char *pszUrl, unsigned int ulLen)
	{ return OPL_LocalSystem_GetIpOtaURL(pszUrl, ulLen) == BPL_STATUS_OK; }

	static bool getHbbtvURL(char *pszUrl, unsigned int ulLen)
	{ return OPL_LocalSystem_GetHbbtvURL(pszUrl, ulLen) == BPL_STATUS_OK; }

	static bool checkFileExist(char *pszUri, BPL_BOOL *IsExist)
	{ return OPL_LocalSystem_CheckFileExist(pszUri, IsExist) == BPL_STATUS_OK; }

	// box frist boot status
	static bool setBoxFirstBootStatus(BPL_BOOL status)
	{ return OPL_LocalSystem_SetFirstBootStatusOnProduction(status) == BPL_STATUS_OK; }

	static bool getBoxFirstBootStatus(BPL_BOOL *status)
	{ return OPL_LocalSystem_GetFirstBootStatusOnProduction(status) == BPL_STATUS_OK; }

	static bool getBoxLaunchAppMode(unsigned int *status)
	{ return OPL_LocalSystem_GetLaunchAppMode(status) == BPL_STATUS_OK; }

	static bool getBoxFusingInfo(BPL_BOOL *IsSCS, BPL_BOOL *IsJTAG, BPL_BOOL *IsOTP, BPL_BOOL *IsCWE, unsigned int *pulMarketID)
	{ return OPL_LocalSystem_GetFusingInfo(IsSCS, IsJTAG, IsOTP, IsCWE, pulMarketID) == BPL_STATUS_OK; }
	static bool getBoxFusingInfoExt(BPL_BOOL *IsSCS, BPL_BOOL *IsJTAG, BPL_BOOL *IsOTP, BPL_BOOL *IsCWE, unsigned int* pulOTPField1, unsigned int* pulOTPField2)
	{ return OPL_LocalSystem_GetFusingInfoExt(IsSCS, IsJTAG, IsOTP, IsCWE, pulOTPField1, pulOTPField2 ) == BPL_STATUS_OK; }


	// 지상파 antennna power
	static bool setAntennaPowerStatus(BPL_BOOL status)
	{ return OPL_LocalSystem_SetAntennaPower(status) == BPL_STATUS_OK; }

	static bool getAntennaPowerStatus(BPL_BOOL *status)
	{ return OPL_LocalSystem_GetAntennaPower(status) == BPL_STATUS_OK; }


	// Dynamic update mgr
	static bool setServiceUpdateFlag(BPL_BOOL status)
	{ return OPL_LocalSystem_SetServiceUpdateFlag(status) == BPL_STATUS_OK; }

	static bool getServiceUpdateFlag(BPL_BOOL *status)
	{ return OPL_LocalSystem_GetServiceUpdateFlag(status) == BPL_STATUS_OK; }

	static bool getNandBadBlocks(unsigned int *pulCntLdr, unsigned int *pulCntUBI, unsigned int *pulCntUBIFS)
	{ return OPL_LocalSystem_GetNandBadBlocks(pulCntLdr, pulCntUBI, pulCntUBIFS) == BPL_STATUS_OK; }

	/// Power Control Area...
	// standbyPowerMode in HConfiguration (getter)
	static bool getStandByPowerMode(int *pnStandByPowerMode)
	{ return (OPL_LocalSystem_GetStandByPowerMode( pnStandByPowerMode) == BPL_STATUS_OK); }

	// standbyPowerMode in HConfiguration (setter)
	static bool setStandByPowerMode(int nStandByPowerMode)
	{ return OPL_LocalSystem_SetStandByPowerMode( nStandByPowerMode) == BPL_STATUS_OK; }

	// standbyPowerMode in HConfiguration (getter)
	static bool getStandBySleepMode(int *pnStandBySleepMode)
	{ return (OPL_LocalSystem_GetSleepModeOnStandby( pnStandBySleepMode) == BPL_STATUS_OK); }

	// standbyPowerMode in HConfiguration (setter)
	static bool setStandBySleepMode(int nStandBySleepMode)
	{ return OPL_LocalSystem_SetSleepModeOnStandby( nStandBySleepMode) == BPL_STATUS_OK; }

	// standbyPowerMode in HConfiguration (getter)
	static bool getStandByBootMode(int *pnStandByBootMode)
	{ return (OPL_LocalSystem_GetBootModeOnStandby( pnStandByBootMode) == BPL_STATUS_OK); }

	// standbyPowerMode in HConfiguration (setter)
	static bool setStandByBootMode(int nStandByBootMode)
	{ return OPL_LocalSystem_SetBootModeOnStandby( nStandByBootMode) == BPL_STATUS_OK; }

	// standbyPowerMode in HConfiguration (getter)
	static bool getStandByWifiPower(int *pnStandByWifiPower)
	{ return (OPL_LocalSystem_GetWifiPowerOnStandby( pnStandByWifiPower) == BPL_STATUS_OK); }

	// standbyPowerMode in HConfiguration (setter)
	static bool setStandByWifiPower(int nStandByWifiPower)
	{ return OPL_LocalSystem_SetWifiPowerOnStandby( nStandByWifiPower) == BPL_STATUS_OK; }

	// standbyState in HLocalSystem (getter)
	static bool getStandby(void)
	{
		BPL_BOOL bStandby = FALSE;
		OPL_LocalSystem_GetStandby( &bStandby);
		return bStandby == TRUE;
	}

	// standbyState in HLocalSystem (setter)
	static bool setStandby(BPL_BOOL bStandby)
	{ return OPL_LocalSystem_SetStandby( bStandby) == BPL_STATUS_OK; }

	// setPowerState() in HBBTV/OIPF/HLocalSystem
	static bool controlPower(int OIPFValue)
	{ return OPL_LocalSystem_PowerControl( OIPFValue) == BPL_STATUS_OK; }

	// onPowerStateChangeEvent listener setting (Whole Part)
	static void setPowerStateChangeListener(OplLocalSystemPowerStateChange_t listener)
	{ OPL_LocalSystem_setPowerStateChangeListener(listener);};

	// onPowerStateChangeError listener setting (JLAB Only)
	static void setPowerStateChangeErrorListener(OplLocalSystemPowerStateChangeError_t listener)
	{ OPL_LocalSystem_setPowerStateChangeErrorListener(listener);};

	static bool getOipfPowerState(int *pnOipfPowerState)
	{ return OPL_LocalSystem_GetOipfPowerState( pnOipfPowerState) == BPL_STATUS_OK; }

	static bool getOipfPreviousPowerState(int *pnOipfPreviousPowerState)
	{ return OPL_LocalSystem_GetPreviousOipfPowerState( pnOipfPreviousPowerState) == BPL_STATUS_OK; }

	static bool getOipfTimeCurrentPowerState(int *pnOipfPreviousPowerState)
	{ return OPL_LocalSystem_GettimeCurrentPowerState( pnOipfPreviousPowerState) == BPL_STATUS_OK; }

	static bool showMsgbox(void)
	{	return OPL_LocalSystem_ShowMsgbox() == BPL_STATUS_OK; }

	static bool showKeyboard(int type, int pwmode, char *str)
	{	return OPL_LocalSystem_ShowKeyboard(type, pwmode, str) == BPL_STATUS_OK; }

	static bool getHCRData(char **pszbuffer)
	{	return OPL_LocalSystem_GetHCRData(pszbuffer) == BPL_STATUS_OK; }

	static bool checkHCRData()
	{	return OPL_LocalSystem_CheckHCRData() == BPL_STATUS_OK; }

	static bool getMicrophoneVolume(unsigned int *pnVolume)
	{ return OPL_LocalSystem_GetMicrophoneVolume(pnVolume) == BPL_STATUS_OK; }

	static bool setMicrophoneVolume(unsigned int nVolume)
	{ return OPL_LocalSystem_SetMicrophoneVolume(nVolume) == BPL_STATUS_OK; }

	static bool getMicrophoneEnabled(void)
	{
		BPL_BOOL bEnable = FALSE;
		OPL_LocalSystem_GetMicrophoneEnabled(&bEnable);
		return bEnable == TRUE;
	}

	static bool setMicrophoneEnabled(BPL_BOOL bEnable)
	{ return OPL_LocalSystem_SetMicrophoneEnabled(bEnable) == BPL_STATUS_OK; }

	static bool getConnectedMicrophone(unsigned int *pnCnt)
	{ return OPL_LocalSystem_GetConnectedMicrophone(pnCnt) == BPL_STATUS_OK; }


	static bool getNetworkId( char *pszNetworkId, unsigned int ulLen)
	{ return OPL_LocalSystem_GetNetworkIds( pszNetworkId,ulLen) == BPL_STATUS_OK; }

	/* Freeview Connected (FVC) */
	static bool getFvcNetworkUriPrefix(char *pszUrl, unsigned int ulLen)
	{ return OPL_LocalSystem_GetFvcNetworkUriPrefix(pszUrl, ulLen) == BPL_STATUS_OK; }

	static bool getFvcBaseURL(char *pszUrl, unsigned int ulLen)
	{ return OPL_LocalSystem_GetFvcBaseURL(pszUrl, ulLen) == BPL_STATUS_OK; }

	static bool setFvcBaseURL(const char *pszUrl)
	{ return OPL_LocalSystem_SetFvcBaseURL(pszUrl) == BPL_STATUS_OK; }

	static bool getFvcAuthHash(char *str, unsigned int ulLen)
	{ return OPL_LocalSystem_GetFvcAuthHash(str, ulLen) == BPL_STATUS_OK; }

	static bool setFvcAuthHash(const char *str)
	{ return OPL_LocalSystem_SetFvcAuthHash(str) == BPL_STATUS_OK; }

	static bool getFvcAuthTimeStamp(char *str, unsigned int ulLen)
	{ return OPL_LocalSystem_GetFvcAuthTimeStamp(str, ulLen) == BPL_STATUS_OK; }

	static bool setFvcAuthTimeStamp(const char *str)
	{ return OPL_LocalSystem_SetFvcAuthTimeStamp(str) == BPL_STATUS_OK; }

	static bool setNtpServerURL(const char *pszUrl)
	{ return OPL_LocalSystem_SetNtpServerURL(pszUrl) == BPL_STATUS_OK; }

	static bool getBatUpdateMode(int *pnBatUpdateMode)
	{ return (OPL_LocalSystem_GetBatUpdateMode( pnBatUpdateMode) == BPL_STATUS_OK); }

	static bool setBatUpdateMode(int nBatUpdateMode)
	{ return OPL_LocalSystem_SetBatUpdateMode( nBatUpdateMode) == BPL_STATUS_OK; }

	static bool getNitLcnUpdateMode(int *pnNitLcnUpdateMode)
	{ return (OPL_LocalSystem_GetNitLcnUpdateMode( pnNitLcnUpdateMode) == BPL_STATUS_OK); }

	static bool setNitLcnUpdateMode(int nNitLcnUpdateMode)
	{ return OPL_LocalSystem_SetNitLcnUpdateMode( nNitLcnUpdateMode) == BPL_STATUS_OK; }
};

#endif

#endif

