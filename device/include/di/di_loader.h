#ifndef __DI_LOADER__H__
#include "htype.h"
#include "di_err.h"

typedef enum
{
	Image_Usage_NormalBoot,			/* main application Jump */
	Image_Usage_FacotryAppBoot,		/* Factory loader Jump */
	Image_Usage_LoaderInfo, 		/* Flash address Loader Stored */
	Image_Usage_HumaxTVInfo,		/* Flash address humaxTv Stored */
	Image_Usage_ShareInfo,			/* Flash address Share Stored */
	Image_Usage_RootfsInfo,			/* Flash address Rootfs Stored */
	Image_Usage_CrytKernelInfo,		/* Crypted kernel information */
	Image_Usage_CrytRootfsInfo,		/* Crypted rootfs information */
	Image_Usage_App01Info,			/* Application 01 information */
	Image_Usage_App02Info,			/* Application 02 information */
	Image_Usage_DecompHdfBufInfo,			/* OTA memory decompress buffer address information */
	Image_Usage_CompHdfBufInfo,			/* OTA memory compress buffer address information */
	Image_Usage_UpgraderBoot, 		/* OTA Jump */
	Image_Usage_UpgraderBoot2nd, 		/* OTA Jump */
	Image_Usage_MtdPhram,			/* Flash address MTD physical system ram Stored */
	Image_Usage_MAX
}Image_Usage_t;

typedef enum
{
	Image_Nagra_Info_Launcher,
	Image_Nagra_Info_Downloader,
	Image_Nagra_Info_Kernel,
	Image_Nagra_Info_Rootfs,
	Image_Nagra_Info_DA2,
	Image_Nagra_Info_PairingKey,
	Image_Nagra_Info_DA2Factory,
	Image_Nagra_Info_MAX
}Image_Nagra_Info_t;

typedef enum
{
	Image_Type_HDF,
	Image_Type_ELF,
	Image_Type_BINARY,
	Image_Type_BINARY_ZIPPED,
	Image_MAX
}Image_Type_t;

typedef enum
{
	Image_OS_Linux,
	Image_OS_Linux_Ramdisk,		/* need to return Ramdisk Size to DI */
//	Image_OS_Linux_LoadRamdisk,		/* need to return Ramdisk Size to DI */
	Image_OS_OS21,
	Image_OS_OS20,
	Image_OS_MAX
}Image_OS_t;

typedef enum
{
	Flash_Type_NOR,
	Flash_Type_NAND,
	Flash_Type_eMMC,
	Flash_Type_MAX
}Flash_Type_t;

typedef enum
{
	Image_Boot_Normal,
	Image_Boot_NormalBackup,
	Image_Boot_Factory,
	Image_Boot_Ota,
	Image_Boot_OtaBackup,
	Image_Boot_NormalInFactory,
	Image_Boot_MAX
}Image_Boot_t;

typedef struct
{
	HUINT32 		ulStoredAddress; /* normally Flash Address */
	HUINT32 		ulLoadingAddress;   /* normally RAM Address */
	HUINT32			ulSignatureCheckAddress; /* normally RAM cached Address */
	HUINT32 		ulRamdiskRootfsLoadingAddress; /* ramdisk's rootfs loading address */
	HUINT32			ulEntryAddress; /* Jump Address */
	HUINT32			defaultImageSize; /* Default Image Size */
	Image_Type_t 	Image_Type; /* Image Type */
	Image_OS_t		Image_OS; /*  necessary  for get Ramdisk size in loader*/
	Flash_Type_t		Flash_Type; /* Flash Partiton Type */
	HBOOL			bFeature_SecureExec; /* Secure Execution */
	HBOOL			bFeature_DivideSig; /* Divide Signature */
	HBOOL			bFeature_EncryptImg; /* Encrypt Image */
}ImageInfo_t;

typedef struct
{
	HUINT32 		ulJumpAddress;
	HUINT32 		ulRamdiskSize;   /* need for Image_OS_Linux_Ramdisk */
	Image_OS_t 		etImage_OS;
	Image_Boot_t	etImage_Boot;
}JumpInfo_t;

typedef struct
{
	Image_Usage_t			  	etImageUsage;
	ImageInfo_t					etImageInfo;
} Loader_ImageInfo_t;

typedef enum
{
	DI_LOADER_DEVICE_AUDIO,
	DI_LOADER_DEVICE_CHANNEL,
	DI_LOADER_DEVICE_CI,
	DI_LOADER_DEVICE_DEMUX,
	DI_LOADER_DEVICE_DRV,
	DI_LOADER_DEVICE_DSC,
	DI_LOADER_DEVICE_EEPROM,
	DI_LOADER_DEVICE_ETHERNET,
	DI_LOADER_DEVICE_FACTORY,
	DI_LOADER_DEVICE_FLASH,
	DI_LOADER_DEVICE_FRONT_PANEL,
	DI_LOADER_DEVICE_HDD,
	DI_LOADER_DEVICE_HDMI,
	DI_LOADER_DEVICE_IPTUNER,
	DI_LOADER_DEVICE_IRB,
	DI_LOADER_DEVICE_KEY,
	DI_LOADER_DEVICE_LED,
	DI_LOADER_DEVICE_MEDIA,
	DI_LOADER_DEVICE_MODEM,
	DI_LOADER_DEVICE_OSD,
	DI_LOADER_DEVICE_POWER,
	DI_LOADER_DEVICE_PVR,
	DI_LOADER_DEVICE_RTC,
	DI_LOADER_DEVICE_SCART,
	DI_LOADER_DEVICE_SMARTCARD,
	DI_LOADER_DEVICE_SYSTEM,
	DI_LOADER_DEVICE_TUNER,
	DI_LOADER_DEVICE_UART,
	DI_LOADER_DEVICE_USB,
	DI_LOADER_DEVICE_VIDEO,
	DI_LOADER_DEVICE_FS,
	DI_LOADER_DEVICE_DUMMY
} DI_LOADER_DEVICE_E;

/* For Pre-Loader, Launcher, Downloader. Except Application */
typedef enum
{
	DI_LOADER_DOWNLOAD_NONE = 0,
	DI_LOADER_DOWNLOAD_BY_OTA,			/* via Linear/Network/Flash (CAB, SAT, TER, FTP, Background)*/
	DI_LOADER_DOWNLOAD_BY_USB,			/* via USB disk */
	DI_LOADER_DOWNLOAD_BY_SERIAL,		/* via Serial */
	DI_LOADER_DOWNLOAD_BY_EMERGENCY,	/* for Emergency */
	DI_LOADER_DOWNLOAD_BY_TFTP,			/* via Network(TFTP), for debug only */
	DI_LOADER_DOWNLOAD_BY_USBDUMP,		/* for dump2usb debug only */
	DI_LOADER_DOWNLOAD_BY_FORCE_OTA_BY_KEY,		/* for force OTA by front key */
	DI_LOADER_DOWNLOAD_DUMMY
}DI_LOADER_DOWNLOAD_TYPE_E;

typedef enum
{
	DI_LOADER_SCRATCH_NONE = 0,
	DI_LOADER_SCRATCH_OTA,				/* always on - for OTA*/
	DI_LOADER_SCRATCH_SBP_RESULT1,		/* SBP_RESULT1 */
	DI_LOADER_SCRATCH_SBP_RESULT2,		/* SBP_RESULT2 */
	DI_LOADER_SCRATCH_WAKEUP_REASON,	/* Wakeup Reason */
	DI_LOADER_SCRATCH_DUMMY
}DI_LOADER_SCRATCH_TYPE_E;

DI_ERR_CODE DI_LOADER_GetImageInfo(Image_Usage_t etImageUsage, ImageInfo_t *pImageInfo);
DI_ERR_CODE DI_LOADER_GetNagraImageInfo(Image_Nagra_Info_t etImageUsage, ImageInfo_t *pImageInfo);

DI_ERR_CODE DI_LOADER_Jump(JumpInfo_t stJumpInfo);
DI_ERR_CODE DI_LOADER_InitDevice(DI_LOADER_DEVICE_E type);

DI_ERR_CODE DI_LOADER_GetScratch(DI_LOADER_SCRATCH_TYPE_E eSrType, unsigned int *eDnType);
DI_ERR_CODE DI_LOADER_SetScratch(DI_LOADER_SCRATCH_TYPE_E eSrType, unsigned int eDnType);

DI_ERR_CODE DI_LOADER_GetDownloadType(DI_LOADER_DOWNLOAD_TYPE_E *eDnType);
DI_ERR_CODE DI_LOADER_SetDownloadType(DI_LOADER_DOWNLOAD_TYPE_E eDnType);
DI_ERR_CODE DI_LOADER_LoadImage(unsigned int *pLoadAddr);
DI_ERR_CODE DI_LOADER_LoadInit(void);
DI_ERR_CODE DI_LOADER_GetSignKey(HUINT8 *pKeyBuf);

#if defined(CONFIG_DEBUG)
HBOOL DI_LOADER_IsBootActionWait(void);
#endif


typedef enum tagDI_LOADER_BootOption_e
{
	eDI_BOOTOPTION_None			= 0x000000,

	/*
	 * add BootParam
	 * CableModem MAC Addresss
	 */
	eDI_BOOTOPTION_CM_MACADDR	= 0x000001,
	/* External RSA public key */
	eDI_BOOTOPTION_RSA_KEY		= 0x000002,
	/* External Application */
	eDI_BOOTOPTION_EXT_APP		= 0x000004,

	/* ex */
	eDI_BOOTOPTION_xxxx3		= 0x000008,
	eDI_BOOTOPTION_xxxx4		= 0x000010,

	/* all option */
	eDI_BOOTOPTION_Max			= 0xFFFFFF
} DI_LOADER_BootOption_e;

#define DEF_MAX_ETHNAME_LENGTH 128
#define DEF_MAX_EXTAPPNAME_LENGTH 16

typedef struct tagDI_LOADER_BootOption
{
	HINT8 					ethernetName[DEF_MAX_ETHNAME_LENGTH];
	HINT8 					extAppName[DEF_MAX_EXTAPPNAME_LENGTH];
	DI_LOADER_BootOption_e 	eOption;
} DI_LOADER_BootOption_t;

void CMD_DI_LOADER_Init(void *arg);
void DI_LOADER_Init(void *arg);
DI_ERR_CODE DI_LOADER_SetBootOption(DI_LOADER_BootOption_t *pstOption);
DI_ERR_CODE DI_LOADER_GetBootOption(DI_LOADER_BootOption_t *pstOption);
HBOOL DI_LOADER_IsBootwait(void);


#endif /* __DI_LOADER__H__ */
