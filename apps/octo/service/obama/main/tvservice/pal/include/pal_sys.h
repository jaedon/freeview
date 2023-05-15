/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_sys.h
	@brief	  Octo pal system information header

	Description: Product 정보를 제공하는 system information PAL의 헤더 파일.\n
	Module: PAL/SYS			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	_PAL_SYS_H_
#define	_PAL_SYS_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <stdio.h>
#include <stdarg.h>
#if defined(WIN32)
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#endif

#if defined(WIN32)
#define		DIR			char
#define		opendir(x)	x
#define		closedir(x)
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define PalSys_UPGRADE_PIECE_SIZE_FULL				(HUINT32)(-1)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if 0
typedef struct
{
	HINT8	sLoaderVersion[20];
	HINT8	sMcuLoaderVersion[20];
	HINT8	sMcuAppVersion[20];
	HINT8	sChipInfo[20];
	HINT8	sModelName[20];
	HINT8	sAppVersion[20];
#if defined(CONFIG_MW_CI_PLUS)
	HINT8	sAppVersionCIPLUS[20];
#endif
	HINT8	sSystemID[20];
	HINT8	sBootLoader[20];
} PAL_VERSION_INFO_t;
#endif

typedef enum
{
	PAL_SYS_ERR_OK = 0,
	PAL_SYS_ERR_FAIL = 1,
	PAL_SYS_ERR_CRC = 2,
	PAL_SYS_ERR_MAX
}PAL_SYS_ERR_CODE_t;

#if 0
typedef enum
{
	PAL_EEPROM_SD_STANDARD_PAL = 0,
	PAL_EEPROM_SD_STANDARD_NTSC,
	PAL_EEPROM_SD_STANDARD_SECAM
} PAL_VIDEO_SD_STANDARD;
#endif

typedef enum
{
	PAL_FLASH_FIELD_SERIAL_NO = 0,
	PAL_FLASH_FIELD_DTCP_KEY,
	PAL_FLASH_FIELD_CI_PLUS_KEY,
	PAL_FLASH_FIELD_MAC_ADDR,
	PAL_FLASH_FIELD_HDCP_KEY,
	PAL_FLASH_FIELD_PARING_DATA,
	PAL_FLASH_FIELD_SYSTEM_ID,
	PAL_FLASH_FIELD_LOADER_VER,
	PAL_FLASH_FIELD_APP_VER,
	PAL_FLASH_FIELD_APP_VERSION_NEXT,
	PAL_FLASH_FIELD_PRIVATE,

	PAL_FLASH_FIELD_CSC_KEY,
	PAL_FLASH_FIELD_DSTB_ID,

	PAL_FLASH_FIELD_IRDETO_BBCB,
	PAL_FLASH_FIELD_IRDETO_UC_SECURE_ID,
	PAL_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA,
	PAL_FLASH_FIELD_IRDETO_CPCB,
	PAL_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED,
	PAL_FLASH_FIELD_IRDETO_CA_PARTITION,
	PAL_FLASH_FIELD_IRDETO_LOADER_PARTITION,
	PAL_FLASH_FIELD_IRDETO_IR_PARTITION,
	PAL_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION,

	PAL_FLASH_FIELD_MAX
} PAL_FLASH_FIELD_t;

// for emergency ota write of new loader nvram write type
#define PAL_EMERGENCY_OTA_CHANNEL_NAME     "EMERGE_OTA_CHANNEL"

typedef enum
{
           PAL_EMERGENCY_OTA_CHANNEL_SAT        = 0x01,
           PAL_EMERGENCY_OTA_CHANNEL_CAB		= 0x02,
           PAL_EMERGENCY_OTA_CHANNEL_TER		= 0x04,
           PAL_EMERGENCY_OTA_CHANNEL_FTP		= 0x08,
           PAL_EMERGENCY_OTA_CHANNEL_BG         = 0x80, // SAT, CAT, TER, FTP와 같이 사용
} PAL_EMERGENCY_OTA_CHANNEL_t;


typedef enum
{
	PAL_EMERGENCY_FS_PARTITION_RO,
	PAL_EMERGENCY_FS_PARTITION_RW,
	PAL_EMERGENCY_FS_PARTITION_LOADER,
	PAL_EMERGENCY_PARTITION_W_RAWFS,
	PAL_EMERGENCY_PARTITION_MAX
} PAL_EMERGENCY_FS_PARTITION_t;

#if defined( CONFIG_MW_INET_SNMP )
typedef	enum
{
	PAL_STB_POWER_STATE_OFF = 0,
	PAL_STB_POWER_STATE_ON,
} PAL_SNMP_StbPowerStatus_e;

typedef enum
{
	PAL_STB_PLAY_STATE_NO_WTV,
	PAL_STB_PLAY_STATE_WTV,
	PAL_STB_PLAY_STATE_HDD_PLAY,
	PAL_STB_PLAY_STATE_VOD,
	PAL_STB_PLAY_STATE_ETC
} PAL_SNMP_StbPlayStatus_e;

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

	PAL_SNMP_StbPowerStatus_e etPowerStatus;	/*	stbInfoPowerStatus*/
	PAL_SNMP_StbPlayStatus_e etPlayStatus;	/*	stbInfoPlayStatus*/

	HUINT32 ulTuner1ChannelId; 		/*stbInfoTuner1ServiceId*/
	HUINT32 ulTuner1NetworkId;		/*stbInfoTuner1NetworkId*/
	HUINT32 ulTuner1Frequency; 		/*stbInfoTuner1Frequency*/

	HUINT32 ulTuner2ChannelId; 		/*stbInfoTuner2ServiceId*/
	HUINT32 ulTuner2NetworkId;		/*stbInfoTuner2NetworkId*/
	HUINT32 ulTuner2Frequency; 		/*stbInfoTuner2Frequency*/

} PAL_SNMP_MWInformation_t;
#endif

typedef enum
{
	PAL_CRYPT_AES,
	PAL_CRYPT_DES,
	PAL_CRYPT_TDES,
	PAL_CRYPT_AES_CTR,
	PAL_CRYPT_AES_CBC_MAC,
	PAL_CRYPT_TDES_DTCPIP,
	PAL_CRYPT_MAX
} PAL_CRYPT_CRYPTMODE_t;

typedef enum
{
	PAL_CRYPT_DEFAULT_SYSTEM_KEY,
	PAL_CRYPT_INTERNAL_CAS_KEY,
	PAL_CRYPT_USER_KEY,
} PAL_CRYPT_CRYPTKEYTYPE_t;

typedef struct
{
	PAL_CRYPT_CRYPTMODE_t		etCryptMode;
	PAL_CRYPT_CRYPTKEYTYPE_t	etCryptKey;
	HUINT8						*pKey;
	HUINT32						ulKeySize;
	const HUINT8				*pSrcBuffer;
	HUINT8						*pDstBuffer;
	HUINT32						ulBuffSize;
} PAL_Crypt_t;

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
typedef void (*PAL_OverTemperatureCallback_t)(void);
typedef void (*PAL_FanLockCallback_t)(void);
#endif

/*********************************************** TV ****************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#if 0
extern HERROR PAL_SYS_GetModelName (HUINT8 *szModelName, HBOOL bPanelSizeOn);
#endif
extern HERROR PAL_SYS_GetSystemId(HUINT32 *pulSystemId);
extern HERROR PAL_SYS_SetSystemVersion(VersionType_t eSystemType, HUINT32 ulSystemVer);
extern HERROR PAL_SYS_MakeSystemVerString(VersionType_t eSystemType, HUINT32 ulSystemVer, HUINT8 *szSysVerStr);
extern HERROR PAL_SYS_GetTotalSystemVersionString(HUINT8 *szVerStr);
extern HERROR PAL_SYS_GetKernelVersionString(HUINT8 *szVerStr);
extern HERROR PAL_SYS_GetMicomVerString(HUINT8 *szVerStr);
extern HERROR PAL_SYS_GetChipId(HUINT32 *pulChipId );
extern HERROR PAL_SYS_GetChipId_x64(HUINT32 *pulFrontId, HUINT32 *pulRearId);
extern HERROR PAL_SYS_GetChipRevision(HUINT8 *pucChipRevision);
extern HERROR PAL_SYS_GetChipCheckNumber(HUINT32 *pulChipCheckNumber);
extern HERROR PAL_SYS_GetSysHWRevision(HUINT32 *pulHWRevision );
extern HERROR PAL_SYS_GetNandBadBlocks(HUINT32 *pulCntLdr, HUINT32 *pulCntUBI, HUINT32 *pulCntUBIFS);
#if defined(CONFIG_MW_CAS_CONAX_CPM)
extern HERROR PAL_SYS_GetFusingInfoForConax(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT8 *pucRemux, HUINT8 *pucMarketId);
#endif
#ifdef CONFIG_MW_CAS_VIACCESS
#if 0
HERROR PAL_SYS_SetFusingInfo(HUINT8 ucCWE);
#endif
HERROR PAL_SYS_EncryptRecKey(HUINT8 *pucPlain, HUINT8 *pucEncrypted);
#endif
#if 0
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
HERROR PAL_SYS_SetSecureBootFusingInfo(HUINT8 ucSB);
HERROR PAL_SYS_SetSecureCWFusingInfo(HUINT8 ucSC);
HERROR PAL_SYS_SetJtagFusingInfo(HUINT8 ucJtag);
#endif
#endif

extern HERROR PAL_SYS_GetOtaFlag (SWUP_InfoSlot_e eSlot, SWUP_Flag_e *peOtaFlag);
extern HERROR PAL_SYS_SetOtaFlag (SWUP_InfoSlot_e eSlot, SWUP_Flag_e eOtaFlag);
extern HERROR PAL_SYS_GetOtaType (SWUP_InfoSlot_e eSlot, SWUP_DownloadType_e *peOtaType);
extern HERROR PAL_SYS_SetOtaType (SWUP_InfoSlot_e eSlot, SWUP_DownloadType_e eOtaType);
extern HERROR PAL_SYS_SetChannelType (SWUP_InfoSlot_e eSlot, SWUP_ChannelType_e eChType);
extern HERROR PAL_SYS_SetOtaPid (SWUP_InfoSlot_e eSlot, HUINT16 usPid);
extern HERROR PAL_SYS_SetOtaTuningInfo (SWUP_InfoSlot_e eSlot, DxDeliveryType_e eDeliType, void *pvDdTuningInfo);
extern HERROR PAL_SYS_SetSwUpdateTuningInfoSat (SWUP_InfoSlot_e			eSlot,
											HUINT32 				ulFrequency,
											HUINT32 				ulSymbolRate,
											DxSat_CodeRate_e			fecCodeRate,
											DxSat_Polarization_e		polarization,
											DxSat_TransportSpec_e 	transSpec,
											DxSat_PskModulation_e 	pskMod,
											DxSat_Pilot_e 			pilot,
											DxAntennaType_e		antennaType,
											/* LNB && Common */
											HUINT32 				ulLnbFreq,
											DxSAT_LnbVoltage_e		lnbVoltage,
											HBOOL					b22kTone,				/**< 22KHz tone ON/OFF */
											/* DISQEC */
											DxSAT_DiseqcVer_e 		diseqcVersion,			/**< Diseqc Ver. 1.0, 1.1, 1.2, 1.3, 2.0 */
											DxSAT_DiseqcInput_e		diseqcInput,			/**< Diseqc A, B, C, D */
											DxSAT_ToneBurst_e 		toneBurst,				/**< NONE, A, B */
											HUINT32 				ucMotorPosition,		/**< for DiSEqC 1.2 */
											HINT32					sSatLonggitude, 		/**< for DiSEqC 1.3 */
											/* SCD */
											DxSAT_ScdInput_e			scdInput,				/**< SCD diseqc input A, B */
											HUINT32 				ulScdUserBandNum,		/**< Tuner 0용 SCD user band number. 설정에서 읽어서 넣어라. */
											HUINT32 				ulScdUserBandFreq,		/**< Tuner 0용 SCD user band frequency. 설정에서 읽어서 넣어라. */
											HUINT32 				ulScdUserBandNum1,		/**< Tuner 1용 SCD user band number. 설정에서 읽어서 넣어라. */
											HUINT32 				ulScdUserBandFreq1  	/**< Tuner 1용 SCD user band frequency. 설정에서 읽어서 넣어라. */
											);
extern HERROR PAL_SYS_SetSwUpdateTuningInfoTer (SWUP_InfoSlot_e			eSlot,
											HUINT32 				ulFrequency,
											DxTer_BandWidth_e 		bandWidth,
											DxTer_Constellation_e 	constellation,
											DxTer_Hierachy_e			hierachy,
											DxTer_CodeRate_e			codeRate,
											DxTer_TransMode_e 		transMode,
											DxTer_GuardInterval_e 	guardInterval,
											DxTer_Stream_e			stream,
											DxTer_Offset_e			offset,
											DxTer_System_e			system,
											HUINT32 				plpId,
											DxTer_PreambleFormat_e	preambleFormat,
											DxTer_Mixed_e 			mixed,
											DxTer_Pilot_Pattern_e 	pilotPattern,
											DxTer_ExtendedCarrier_e	extenedCarrier,
											DxTer_PAPR_Reduction_e	pAPRreduction,
											HUINT32 				numPlpBlocks);
extern HERROR PAL_SYS_SetSwUpdateTuningInfoCab (SWUP_InfoSlot_e			eSlot,
											HUINT32 				ulFrequency,
											HUINT32 				ulSymbolRate,
											DxCab_Constellation_e 	constellation,
											DxCab_Spectrum_e			spectrum);

extern HERROR PAL_SYS_GetSystemUpdateDate( HUINT8 *pucString );
extern HERROR PAL_SYS_GetSystemUpdatedList( HUINT8 *pucString );
extern HERROR PAL_SYS_GetHwRevision( HUINT8 *pucString );

extern PAL_SYS_ERR_CODE_t PAL_SYS_GetSystemVersion (VersionType_t eSystemType, HUINT32 *pulSystemVer);

extern HERROR PAL_SYS_SetEeprom4LoaderFactoryReset(void);
extern HERROR PAL_SYS_SetLanguage(HUINT8 *pucLangCode);
extern HERROR PAL_SYS_SetScartRGB(DxScartFormat_b eScartRGB);
extern HERROR PAL_SYS_SetResolution(HUINT32 ulDiResolution);
extern HERROR PAL_SYS_SetFrameRate(HUINT32 ulDiFrameRate);
extern HERROR PAL_SYS_SetAspectRatio(DxAspectRatio_e eAspectRatio);
extern HERROR PAL_SYS_SetTunerId(SWUP_InfoSlot_e eSlot, HUINT8 ucTunerId);
extern HERROR PAL_SYS_SetAntennaPower(SWUP_InfoSlot_e eSlot, HUINT8 ucAntennaPower);
extern HERROR PAL_SYS_SetPortalIP(HUINT8 *pucIp);
extern HERROR PAL_SYS_SetDownloadPath(HUINT8 *pucPath);
extern HERROR PAL_SYS_SetFTPInfo(HUINT8 *pFTPInfo );

extern HERROR PAL_SYS_SetNetworkDeviceId(HUINT8 ucEthernetId);
extern HERROR PAL_SYS_SetNetworkIpConfig(INET_IP_CONFIG_e eIpConfig);
extern HERROR PAL_SYS_SetNetworkIp(HULONG ipv4Addr, HULONG netmask, HULONG gatewayAddr, HULONG DNSaddr);
extern HERROR PAL_SYS_SetFirstBoot(HINT32 nFirstBoot);
extern HERROR PAL_SYS_GetFirstBoot(HINT32 *pnFirstBoot);
extern HERROR PAL_SYS_GetLaunchAppMode(HUINT8 *pnLaunchAppMode);
extern HERROR PAL_SYS_SetLaunchAppMode(HUINT8 nLaunchAppMode);
extern HERROR PAL_SYS_SetOadVersion(HUINT8 *pucOadVersion);
extern HERROR PAL_SYS_SetField(PAL_FLASH_FIELD_t eType, void *pData, HUINT32 ulSize);
extern HERROR PAL_SYS_GetField(PAL_FLASH_FIELD_t eType, void *pData, HUINT32 ulSize);
extern HERROR PAL_SYS_GetFieldOffset(PAL_FLASH_FIELD_t eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize);
extern HERROR PAL_SYS_SetFieldOffset(PAL_FLASH_FIELD_t eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize);

extern HERROR PAL_SYS_SetCountryCode (HUINT8  *pucCountryCode);
extern HERROR PAL_SYS_SetUsageId (HUINT8 ucUsageId);
extern HERROR PAL_SYS_GetUsageId (HUINT8 *pucUsageId);
extern HERROR PAL_SYS_WriteEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_t partition,
												HUINT8 *pFileName, HUINT32 offset, void *data, HINT32 nDataSize);
extern HERROR PAL_SYS_ReadEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_t partition,
												HUINT8 *pFileName, HUINT32 offset, void *data, HINT32 nDataSize);
extern HERROR PAL_SYS_GetCPUTemperature(HUINT32 *pulCpuTemp );

extern HERROR PAL_SYS_InitEncryptionKey (void);
extern HUINT8* PAL_SYS_GetEncryptionKey (void);
extern void *PAL_SYS_CRYPT_MemAlloc (HUINT32 ulSize);
extern HUINT32 PAL_SYS_GetEncryptionKeySize (void);
extern HUINT8 *PAL_SYS_GetEncryptionIV (void);
extern void PAL_SYS_SetPvrEncryptMode( HUINT32 nEncType );
extern HUINT32 PAL_SYS_GetPvrEncryptMode( void );


#if defined (CONFIG_MW_CI_PLUS)
HERROR PAL_SYS_GetCiPlusCoreVersion(HUINT8 *pucString);
#endif


#if 1//!defined(CONFIG_DROP_ROOT_PRIVILEGES)
extern HINT32 PAL_SYS_Ioctl(HINT32 nId, HINT32 nRequest, HUINT8 ucCmdNum, ...);
#else
//#include "secapi.h"
#define PAL_SYS_Ioctl(x, y, z, ...) SECAPI_Ioctl(x, y, z, ## __VA_ARGS__)
extern HINT32 PAL_SYS_Access(const HINT8 *pucPathname, HINT32 nMode);
extern HINT32 PAL_SYS_Remove(const HINT8 *pucPathname);
extern HINT32 PAL_SYS_Mknod(const HINT8 *pucPathname, HUINT32 ulMode, HUINT32 ulDev);
extern HINT32 PAL_SYS_Open(const HINT8 *pucPathname, HINT32 nFlags);
extern HINT32 PAL_SYS_Open2(const HINT8 *pucPathname, HINT32 nFlags, HUINT32 ulMode);
extern DIR 	*PAL_SYS_OpenDir(const HINT8 *ucName);
extern HINT32 PAL_SYS_ChDir(const HINT8 *pucPathname);
extern HINT32 PAL_SYS_MkDir(const HINT8 *pucPathname, HUINT32 ulMode);
extern HINT32 PAL_SYS_Stat(const HINT8 *pucPath, struct stat *pstBuf);
extern HINT32 PAL_SYS_Lstat(const HINT8 *pucPath, struct stat *pstBuf);
extern HINT32 PAL_SYS_Rename(const HINT8 *pucPath, const HINT8 *pucNewPath);
extern HINT32 PAL_SYS_Socket(HINT32 nDomain, HINT32 nType, HINT32 nProtocol);
extern HINT32 PAL_SYS_Close(HINT32 nFd);
extern HINT32 PAL_SYS_Select(HINT32 nFd, fd_set *pstReadFds, fd_set *pstWriteFds, fd_set *pstExceptFds, struct timeval *pstTimeout);
extern HLONG PAL_SYS_Write(HINT32 nFd, const void *pvBuf, HLONG lCount);
extern HINT32 PAL_SYS_Chown(const HINT8 *pucPathname);
extern HINT32 PAL_SYS_Open_BySecHandler(const HINT8 *pucPathname, HINT32 nFlags);
extern HINT32 PAL_SYS_Lseek(HINT32 fd, off_t pos, HINT32 origin);
extern HINT32 PAL_SYS_Read(HINT32 fd, void *buf, HUINT32  ulSize);
extern FILE 	*PAL_SYS_Fopen(const HINT8 *pucPath, const HINT8 *pucMode);
#endif

#if defined( CONFIG_MW_INET_SNMP )
extern void	PAL_SYS_SetMWPrivateMIBInfo(PAL_SNMP_MWInformation_t *pstInfo);
#endif

extern HERROR PAL_SYS_Encrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucEncrypted, HUINT32 ulSize);
extern HERROR PAL_SYS_Decrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucDecrypted, HUINT32 ulSize);

extern HERROR PAL_SYS_EncryptData(PAL_Crypt_t *pstPalCrypt);
extern HERROR PAL_SYS_DecryptData(PAL_Crypt_t *pstPalCrypt);

extern HERROR PAL_SYS_NANDFLASH_Read(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize);
HERROR PAL_SYS_FLASH_Read(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize);
/*	Upgrade
 */
extern HERROR PAL_SYS_UpgradeInit(HUINT32 ulUpgradeMaxSize);

extern HERROR PAL_SYS_Upgrade_Setup(HUINT32		 ulDataSize, 		// In Bytes
									HUINT32		 ulPieceSize, 		// In Bytes : Default 128 KB
									HUINT32		*pulPieceNum,
									HUINT32		*pulPieceSize);

extern HINT32 PAL_SYS_Upgrade_ReadData(HUINT32		 ulPieceIdx,
										HUINT8		*pucData,
										HUINT32		 ulSize);

extern HINT32 PAL_SYS_Upgrade_WriteData(HUINT32			 ulPieceIdx,
										const HUINT8	*pucData,
										HUINT32			 ulSize);

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
extern void	PAL_SYS_RegistOverTemperatureCallback(PAL_OverTemperatureCallback_t pfncOverTempCallback);
extern void	PAL_SYS_RegistFanLockCallback(PAL_FanLockCallback_t pfncFanLockCallback);
#endif

#if defined(CONFIG_DEBUG_AGING_TEST) && defined(CONFIG_PROD_DEVICE_STORAGE_USB) && !defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_SUPPORT_UMMA)
extern void PAL_SYS_AgingLog_UsbInit(void);
extern HERROR PAL_SYS_AgingLog_GetPath(HCHAR *pszPath);
#endif

#endif	//!_PAL_SYS_H_

