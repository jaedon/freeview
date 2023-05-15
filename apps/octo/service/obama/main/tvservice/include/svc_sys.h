/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mw_sys.h
	@brief	  System resource I/F

	Description: Front panel, RCU, power, 등등 system 의 device resource를 i/f 해주는 모듈.\n
	Module: MW/SYS			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	_MW_SYS_H_
#define	_MW_SYS_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
//#include <pal_panel.h>
//#include <pal_key.h>

#include <svc_ch.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	SYS_ERR_OK = 0,
	SYS_ERR_FAIL = 1,
	SYS_ERR_CRC = 2,
	SYS_ERR_MAX
}SvcSys_ErrorCode_e;

typedef enum
{
	eSys_CustomCode_1 = 0, 	/* pal_key.h PalKey_CustomCode_t 요것과 같아야 한다. */
	eSys_CustomCode_2 = 1,
	eSys_CustomCode_3 = 2,
	eSys_CustomCode_4 = 3,
	eSys_CustomCode_5 = 4,
	eSys_CustomCode_6 = 5,
	eSys_CustomCode_MAX
} SvcSys_CustomCode_e;


typedef struct
{
	DxPanelCategory_e		category;			// 분류된 category
	HBOOL					bOnOff;				// led 상태
	HBOOL					bUsedByCustom;		// led 가 octo 가 아닌 다른곳에서 제어 되는지?
} SvcSys_LedInform_t;

#if defined( CONFIG_MW_INET_SNMP )

typedef	enum
{
	MW_STB_POWER_STATE_OFF = 0,
	MW_STB_POWER_STATE_ON,
}SvcSys_SnmpStbPowerStatus_e;

typedef enum
{
	MW_STB_PLAY_STATE_NO_WTV,
	MW_STB_PLAY_STATE_WTV,
	MW_STB_PLAY_STATE_HDD_PLAY,
	MW_STB_PLAY_STATE_VOD,
	MW_STB_PLAY_STATE_ETC
}SvcSys_SnmpStbPlayStatus_e;

typedef struct
{
	HUINT8 szSystemId[10];		/* SYSTEM ID, stbDevVerHardware */
	HUINT8 szApplicationVer[10];		/* APP Version, stbDevVerSoftVender */
	HUINT8 szMiddleWareVer[10];		/* M/W Version, stbDevVerSoftMiddle */
	HUINT8 szSoftBrowserVer[10];		/* Browser Version, stbDevVerSoftBrowser */

	HUINT8 szStbID[6];		/* StbID, stbDevIdStb */
	HUINT8 szBcasCardId[32];		/* B CAS Card ID, stbDevIdBcas */
	HUINT8 szCcasCardId[32];		/* C CAS Card ID, stbDevIdCcas */

	HUINT8 szMacAddress[6];		/* MAC Address, stbDevNetMacaddress */
	HUINT8 szIPAddress[4];		/* IP Address, stbDevNetIpaddress */
	HUINT8 szNetMask[4];		/* Net Mask, stbDevNetNetmask */

	HUINT32 ulDownFrequency;      /* Down Frequency, stbIfDownChannelRxFrequency */

	DxDeliveryType_e etWatchingChannelType; /* Watching Channel Type, for stbIfSignalQuailty MIB */
	HUINT32 ulWatchingTunerGroupId;	  /* Watching TunerGroupId, for stbIfSignalQuality MIB */

	HUINT32 ulWatchingChannelId;	/* Watching Channel ID, stbIfChannelIdService */
	HUINT32 ulWatchingNetworkId;/* Watching Netwrok ID, stbIfChannelIdOriginalnet */

	SvcSys_SnmpStbPowerStatus_e etPowerStatus;	/*	stbInfoPowerStatus*/
	SvcSys_SnmpStbPlayStatus_e etPlayStatus;	/*	stbInfoPlayStatus*/

	HUINT32 ulTuner1ChannelId; 		/*stbInfoTuner1ServiceId*/
	HUINT32 ulTuner1NetworkId;		/*stbInfoTuner1NetworkId*/
	HUINT32 ulTuner1Frequency; 		/*stbInfoTuner1Frequency*/

	HUINT32 ulTuner2ChannelId; 		/*stbInfoTuner2ServiceId*/
	HUINT32 ulTuner2NetworkId;		/*stbInfoTuner2NetworkId*/
	HUINT32 ulTuner2Frequency; 		/*stbInfoTuner2Frequency*/

}SvcSys_SnmpInformation_t;
#endif

#define	SvcSys_PARAM_KEY_INVALID		0xFFFFFFFF
typedef struct
{
	HUINT32		 ulKey;		//	PAL_PARAM_Item_e
	void 		*pvData;
	HUINT32		 ulSize;
} SvcSys_FactoryDefaultItem_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR SVC_SYS_Init(PostMsgToMgrCb_t pfncPostMsgCallback);
extern void SVC_SYS_PwrInit(void);


extern HERROR SVC_SYS_GetWakeUpReason(WakeUpReason_t *pWakeUpReason);
extern HERROR SVC_SYS_SetWakeUpReason(WakeUpReason_t wakeUpReason);
extern HERROR SVC_SYS_GetWakeUpSecondReason(WakeUpSecondReason_t *pWakeUpSecondReason);
extern HERROR SVC_SYS_SetWakeUpSecondReason(WakeUpSecondReason_t wakeUpSecondReason);
extern HERROR SVC_SYS_SetPanelTimeDisplay(HBOOL bOnOff);
extern HERROR SVC_SYS_GetPanelMaxStringLen(HUINT32 *pulMaxLen);
extern HERROR SVC_SYS_TurnOffMainBoard(HBOOL bPowerSaveMode);
extern HERROR SVC_SYS_SetRtcTime(HUINT32 ulUnixTime);
extern HERROR SVC_SYS_GetRtcTime(HUINT32 *pulUnixTime);
extern HERROR SVC_SYS_SetWakeUpTime(HUINT32 ulUnixTime);
extern HERROR SVC_SYS_EnableWakeUpTime(void);
extern HERROR SVC_SYS_DisableWakeUpTime(void);
extern HERROR SVC_SYS_Deactivate_SC(void);
extern HERROR SVC_SYS_WIFIPower(HBOOL bOnOff);
extern HERROR SVC_SYS_MountNandFlash(void);
extern HERROR SVC_SYS_UmountNandFlash(void);
extern HERROR SVC_SYS_Reboot(void);
extern HERROR SVC_SYS_SetWakeupRcuKey(HUINT32 keyGroup);
extern HERROR SVC_SYS_ClearWakeupRcuKey(void);
extern HERROR SVC_SYS_PrepareFastBootStandby(void);
extern HERROR SVC_SYS_SetFastBootStandby(HINT32 nSleepMode,HINT32 nBootMode,HBOOL bPowerSavingStandby);
extern HERROR SVC_SYS_CancelFastBootStandby(void);

extern HERROR SVC_SYS_SetColdBootPanelCondition (HBOOL bDisplayVideo, HBOOL bViewPanel);

extern HERROR SVC_SYS_SetWakeOnLAN(HBOOL bOnOff);
extern HERROR SVC_SYS_ClearWakeupRcuKey(void);

extern void SVC_SYS_ClearLed(void);
extern HERROR SVC_SYS_GetPanelCapability(Panel_Capability_t *pstPanelCapa);
extern HERROR SVC_SYS_SetLedOnOff(DxLedId_e ledId, HBOOL bOnOff);
extern HERROR SVC_SYS_SetLedOnOffOnStandbyMode(DxLedId_e ledId, HBOOL bOnOff);
extern HERROR SVC_SYS_GetLedStatus(DxLedId_e eLedId, HBOOL *pbOnOff);

extern HERROR SVC_SYS_SetLedOnOffByCustom(DxLedId_e ledId, HBOOL bOnOff);
extern HERROR SVC_SYS_SetLedAuto(void);
extern void	  SVC_SYS_ClearLedByCustom(void);
extern HERROR SVC_SYS_SetCategoryInform(DxPanelCategory_e eCategory);

extern HERROR SVC_SYS_SetLedBlink(DxLedId_e ledId, HUINT32 delay);
extern HERROR SVC_SYS_StartBlink(HUINT32 ulKeyCode, KEY_PressMode_e eKeyPressMode, KEY_Device_t eKeyDevice);
extern HERROR SVC_SYS_GetBlinkingInfo(DxLedId_e *aulSupportLedList, HUINT32 *pulLedNum, KEY_Code_t *aulIgnoreKeyList, HUINT32 *pulKeyNum);
extern HERROR SVC_SYS_SetPowerLedUse(HBOOL bOnOff);
extern HERROR SVC_SYS_SetPowerLedState(PowerLedState_t ledState);
extern HERROR SVC_SYS_SetPanelDim(PanelDimmingLevel_t dimLevel);
extern HERROR SVC_SYS_GetPanelDimLevel(PanelDimmingLevel_t *peDimLevel);
extern HERROR SVC_SYS_SetPanelDimLevel(PanelDimmingLevel_t eDimLevel);
extern HERROR SVC_SYS_SetPanelPower(HBOOL bOnOff);
extern HERROR SVC_SYS_DisplayPanelString(HINT32 nPriority, HUINT8 *pszStr, DxPanelAlign_e eAlign);
extern HERROR SVC_SYS_DisablePanelAutoDisableOneTime(void);
extern HERROR SVC_SYS_ClearPanelString(HINT32 nPriority);
extern HERROR SVC_SYS_ClearAllPanelString(void);
extern HERROR SVC_SYS_DestroyAllPanelString(void);
extern HERROR SVC_SYS_SetLedVfdDimmLevel(DimmingLevel_e level);
extern HERROR SVC_SYS_SetLedDimmLevel(DimmingLevel_e level);

extern HERROR	SVC_SYS_RegisterKeyEvtCallback(void);
extern HERROR SVC_SYS_RegisterKeyPreProcessingEvtCallback(KEY_NotifyCallback_t pfnNotify);

#if 0
extern void 	SVC_SYS_OnOffKeyInput(HBOOL bOnOff);
#endif
extern HBOOL	SVC_SYS_IsWakeUpKey(HUINT32 ulKeyCode);
#if 0
extern HUINT8*	SVC_SYS_GetKeyString(HUINT32 ulKeyCode);
#endif
extern HERROR	SVC_SYS_GetCurrentCustomCode(HUINT16 *pusCustomCode);

extern HERROR SVC_SYS_GetModelName(HUINT8 *pszModelName, HUINT32 ulLength, HBOOL bPanelSizeOn);
extern HERROR SVC_SYS_GetSystemId (HUINT32 *pulSystemId);
extern HERROR SVC_SYS_GetDeviceId (HUINT8 *pszDeviceId, HUINT32 ulBufSize);
extern HERROR SVC_SYS_SetSystemVersion (VersionType_t eSystemType, HUINT32 ulSystemVer);
extern HERROR SVC_SYS_MakeSystemVerString (VersionType_t eSystemType, HUINT32 ulSystemVer, HUINT8 *szSysVerStr);
extern HERROR SVC_SYS_GetTotalSystemVersionString (HUINT8 *szVerStr);
extern HERROR SVC_SYS_GetKernelVersionString (HUINT8 *szVerStr);
extern HERROR SVC_SYS_GetMicomVersionString(HUINT8 *szVerStr);
extern HERROR SVC_SYS_GetChipId(HUINT32 *pulChipId);
extern HERROR SVC_SYS_GetChipId_x64(HUINT32 *pulFrontId, HUINT32 *pulRearId);
extern HERROR SVC_SYS_GetChipCheckNumber(HUINT32 *pulChipCheckNumber);
extern HERROR SVC_SYS_GetSysHWRevision(HUINT32 *pulHWRevision);
extern HERROR SVC_SYS_GetSystemSerialNumber(HUINT8 *szSerialNoStr, HUINT32 ulSize);
extern HERROR SVC_SYS_GetStbID(HUINT8 *szStbIdStr, HUINT32 ulSize);
// OTA >>>
extern HERROR SVC_SYS_GetOtaFlag (SWUP_InfoSlot_e eSlot, SWUP_Flag_e *peOtaFlag);
extern HERROR SVC_SYS_SetOtaFlag (SWUP_InfoSlot_e eSlot, SWUP_Flag_e eOtaFlag);
extern HERROR SVC_SYS_GetOtaType (SWUP_InfoSlot_e eSlot, SWUP_DownloadType_e *peOtaType);
extern HERROR SVC_SYS_SetOtaType (SWUP_InfoSlot_e eSlot, SWUP_DownloadType_e eOtaType);
HERROR SVC_SYS_SetChannelType(SWUP_InfoSlot_e eSlot, SWUP_ChannelType_e eChType);
extern HERROR SVC_SYS_SetOtaPid(SWUP_InfoSlot_e eSlot, HUINT16 usPid);
extern HERROR SVC_SYS_SetOtaTuningInfo(SWUP_InfoSlot_e eSlot, DxTuneParam_t *pstTuningInfo);

extern HERROR SVC_SYS_GetSystemUpdateDate(HUINT8 *pucString);
extern HERROR SVC_SYS_GetSystemUpdatedList(HUINT8 *pucString);
extern HERROR SVC_SYS_GetHwRevision(HUINT8 *pucString);
extern SvcSys_ErrorCode_e SVC_SYS_GetSystemVersion(VersionType_t eSystemType, HUINT32 *pulSystemVer);

extern HERROR SVC_SYS_GetEeprom4LoaderDefaultItemList(SvcSys_FactoryDefaultItem_t **pastDefaultItem, HUINT32 *pulItemNum);
extern HERROR SVC_SYS_SetEeprom4LoaderFactoryReset(SvcSys_FactoryDefaultItem_t *astDefaultItem, HUINT32 ulItemNum);
extern HERROR SVC_SYS_SetLanguage(HUINT8 *pucLanguage);
extern HERROR SVC_SYS_SetScartRGB(HUINT32 ulScartRGB);
extern HERROR SVC_SYS_SetResolution(DxResolutionSelect_e eResolution);
extern HERROR SVC_SYS_SetAspectRatio(DxAspectRatio_e eAspectRatio);
extern HERROR SVC_SYS_SetTunerId(SWUP_InfoSlot_e eSlot, HUINT8 ucTunerId);
extern HERROR SVC_SYS_SetAntennaPower(SWUP_InfoSlot_e eSlot, HUINT8 ucAntennaPower);
extern HERROR SVC_SYS_SetFirstBoot(HBOOL bFirstBoot);
extern HERROR SVC_SYS_GetFirstBoot(HBOOL *pbFirstBoot);
extern HERROR SVC_SYS_GetLaunchAppMode(LaunchAppMode_t *pLaunchAppMode);
extern HERROR SVC_SYS_SetLaunchAppMode(LaunchAppMode_t eLaunchAppMode);


extern HERROR SVC_SYS_GetFusingInfo(Sys_FusingInfo_t *pstFusingInfo);
extern HERROR SVC_SYS_SetFusingInfoJTAG(HUINT8 ucJTAG);
extern HERROR SVC_SYS_SetFusingInfoCW(HUINT8 ucCWE);
extern HERROR SVC_SYS_SetFusingInfoSecureBoot(HUINT8 ucSCS);


#if defined (CONFIG_MW_CI_PLUS)
HERROR SVC_SYS_GetCiPlusVersionNumber(HUINT8 *pucString);
#endif

HERROR SVC_SYS_GetNandBadBlocks(HUINT32 *pulCntLdr, HUINT32 *pulCntUBI, HUINT32 *pulCntUBIFS);

HERROR SVC_SYS_SetPanelOnByOTA(void);

#if defined( CONFIG_MW_INET_SNMP )
extern void	SVC_SYS_SetMWPrivateMIBInfo(SvcSys_SnmpInformation_t *pstInfo);
#endif

extern HUINT8*	SVC_SYS_GetEncryptionKey (void);
extern HUINT32	SVC_SYS_GetEncryptionKeySize (void);
extern void*	SVC_SYS_CryptMemAlloc (HUINT32 ulSize);
extern HUINT8*	SVC_SYS_GetIV( void );
extern HUINT32 SVC_SYS_GetPvrEncryptMode( void );
extern void SVC_SYS_SetPvrEncryptMode( HUINT32 nEncType );

extern HERROR SVC_SYS_Encrypt(HUINT8 *pucKey, HBOOL bBlock, /* Block:TRUE or Stream:FALSE */ HUINT8 *pucPlain, HUINT8 *pucEncrypted, HUINT32 ulSize);
extern HERROR SVC_SYS_Decrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucDecrypted, HUINT32 ulSize);

#if 0
typedef enum
{
	SVCSYS_NVRAM_FS_PARTITION_RO,
	SVCSYS_NVRAM_FS_PARTITION_RW,
	SVCSYS_NVRAM_FS_PARTITION_LOADER,
	SVCSYS_NVRAM_PARTITION_W_RAWFS
} SvcSysNvramFsPartition_e;

extern HERROR SVC_SYS_NvramRead(SvcSysNvramFsPartition_e ePartition, HUINT8 *pFileName, HUINT32 offset, void *data, HINT32 nDataSize);
extern HERROR SVC_SYS_NvramGetSize(SvcSysNvramFsPartition_e ePartition, HUINT8 *pFileName, HINT32 *pnDataSize);
#endif


// RGB LED Related
typedef struct
{
	DxRgbLedColor_e		eLedColor;
	DxRgbLedDimmLevel_e	eDimmLevel;
}SvcSys_RgbLedState_t;

typedef struct
{
	DxLedId_e 				eLedId;
	SvcSys_RgbLedState_t 	stLedState;
	HBOOL					bBlink;
}SvcSys_RgbLedInfo_t;

extern HERROR SVC_SYS_SetRGBLedNone(DxLedId_e eLedId, HBOOL bBlink);
extern HERROR SVC_SYS_SetRGBLedRed(DxLedId_e eLedId, HBOOL bBlink);
extern HERROR SVC_SYS_SetRGBLedGreen(DxLedId_e eLedId, HBOOL bBlink);
extern HERROR SVC_SYS_SetRGBLedBlue(DxLedId_e eLedId, HBOOL bBlink);
extern HERROR SVC_SYS_SetRGBLedYellow(DxLedId_e eLedId, HBOOL bBlink);
extern HERROR SVC_SYS_SetRGBLedByColor(DxLedId_e eLedId, DxRgbLedColor_e eColor, HBOOL bBlink);
extern HERROR SVC_SYS_SetRGBLedOff(DxLedId_e eLedId);
extern HERROR SVC_SYS_SetRGBLedDimmingLevel(DxLedId_e eLedId, DxRgbLedDimmLevel_e eDimmLevel);
extern HERROR SVC_SYS_SetRGBLedDimmingValue(DxLedId_e eLedId, HUINT8 ucDimmValue);
extern HERROR SVC_SYS_SetRGBLedBlinkOn(DxLedId_e eLedId);
extern HERROR SVC_SYS_SetRGBLedBlinkOff(DxLedId_e eLedId);
extern HERROR SVC_SYS_SetRGBLedBlinkInterval(HULONG ulTime);

#if defined(CONFIG_DEBUG_AGING_TEST) && defined(CONFIG_PROD_DEVICE_STORAGE_USB) && !defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_SUPPORT_UMMA)
extern void SVC_SYS_AgingLog_UsbInit(void);
extern HERROR SVC_SYS_AgingLog_GetPath(HCHAR *pszPath);
#endif

#if defined(CONFIG_NETFLIX_APP)
extern HERROR SVC_SYS_SetNetflixESN(HUINT8 *szESN, HUINT32 ulSize);
extern HERROR SVC_SYS_GetNetflixESN(HUINT8 *szESN, HUINT32 ulSize);
#endif

extern HERROR SVC_SYS_GetCPUTemperature(HUINT32 *pulCpuTemp);

#endif /* !_MW_SYS_H_ */

