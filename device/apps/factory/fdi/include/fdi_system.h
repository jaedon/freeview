/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_SYSTEM_H_
#define _FDI_SYSTEM_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_SYSTEM_DATA_SN,
	FDI_SYSTEM_DATA_MAC,
	FDI_SYSTEM_DATA_HDCPKEY,
	FDI_SYSTEM_DATA_CHIPID,
	FDI_SYSTEM_DATA_CIPLUSKEY,
	FDI_SYSTEM_DATA_DTCPKEY,
	FDI_SYSTEM_DATA_PAIRKEY,
	FDI_SYSTEM_DATA_DSTBID,
	FDI_SYSTEM_DATA_VAEXTRADATA,
	FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR,
	FDI_SYSTEM_DATA_HDD_SN,
	FDI_SYSTEM_IRDETO_PRIVATEDATA,
	FDI_SYSTEM_BC_NSC_DATA
} FDI_SYSTEM_DATA_e;

typedef enum
{
	FDI_SYSTEM_MICOM_INFO = 0,
	FDI_SYSTEM_MICOM_VER,
	FDI_SYSTEM_MICOM_STRVER,
	FDI_SYSTEM_MICOM_UPDATE,
	FDI_SYSTEM_CHIP_ID,
	FDI_SYSTEM_CHIP_CHECK_NO,
	FDI_SYSTEM_FUSING_INFO,
	FDI_SYSTEM_SYS_TICK_FOR_BOOT_TIME,
	FDI_SYSTEM_SMART_TEMP,
	FDI_SYSTEM_HW_REVISION,
	FDI_SYSTEM_CSC_CHECKNUMBER,
	FDI_SYSTEM_RESTORE_BEP,
	FDI_SYSTEM_ERASE_FACTORY_LOADER,
	FDI_SYSTEM_FLASH_OTP_LOCK,
	FDI_SYSTEM_SCS,
	FDI_SYSTEM_FUSE_JTAG,
	FDI_SYSTEM_FUSE_DSC,
	FDI_SYSTEM_CHIP_REVISION,
	FDI_SYSTEM_FUSE_OTP_DATA_SECTION,		/* Broadcom specific to set data section for secure boot on generic chipset */
	FDI_SYSTEM_CPU_INFO,					/* Broadcom specific to read CPU temp. and core voltage */
	FDI_SYSTEM_RF4CE_UPDATE,
	FDI_SYSTEM_STB_CA_SN,
	FDI_SYSTEM_BOOT_MODE,
	FDI_SYSTEM_IR_PATH,
	FDI_SYSTEM_DUMMY
}FDI_SYSTEM_INFO_e;

typedef enum
{
	FDI_SYSTEM_EVENT_CHECK_TEMP,			/**< smart temperature over shutdown threshold ( >68) */
	FDI_SYSTEM_EVENT_MICOM_UPDATE_PROGRESS,
	FDI_SYSTEM_EVENT_RF4CE_UPDATE_PROGRESS, /* RF4CE Firmware Update status */
	FDI_SYSTEM_EVENT_FAN_LOCKED,
	FDI_SYSTEM_EVENT_OVER_TEMPERATURE,
	FDI_SYSTEM_EVENT_CM_REBOOT,
	FDI_SYSTEM_EVENT_IR_EXTENDER,
	FDI_SYSTEM_EVENT_END
}FDI_SYSTEM_EVENT;

typedef enum
{
	FDI_SYSTEM_DOWNLOAD_NONE = 0,
	FDI_SYSTEM_DOWNLOAD_BY_OTA,			/* via Linear/Network/Flash (CAB, SAT, TER, FTP, Background)*/
	FDI_SYSTEM_DOWNLOAD_BY_USB,			/* via USB disk */
	FDI_SYSTEM_DOWNLOAD_BY_SERIAL,		/* via Serial */
	FDI_SYSTEM_DOWNLOAD_BY_EMERGENCY,	/* for Emergency */
	FDI_SYSTEM_DOWNLOAD_BY_TFTP,			/* via Network(TFTP), for debug only */
	FDI_SYSTEM_DOWNLOAD_BY_USBDUMP,		/* for dump2usb debug only */
	FDI_SYSTEM_DOWNLOAD_BY_FORCE_OTA_BY_KEY,		/* for force OTA by front key */
	FDI_SYSTEM_DOWNLOAD_DUMMY
}FDI_SYSTEM_DOWNLOAD_TYPE_e;

#define FDI_SYSTEM_DATA_SN_SIZE 			FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_SN)
#define FDI_SYSTEM_DATA_MAC_SIZE 			FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_MAC)
#define FDI_SYSTEM_DATA_HDCPKEY_SIZE		FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_HDCPKEY)
#define FDI_SYSTEM_DATA_CHIPID_SIZE 		FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_CHIPID)
#define FDI_SYSTEM_DATA_CIPLUSKEY_SIZE		FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_CIPLUSKEY)
#define FDI_SYSTEM_DATA_DTCPKEY_SIZE		FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_DTCPKEY)
#define FDI_SYSTEM_DATA_PAIRKEY_SIZE		FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_PAIRKEY)
#define FDI_SYSTEM_DATA_DSTBID_SIZE			FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_DSTBID)
#define FDI_SYSTEM_DATA_VAEXTRADATA_SIZE	FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_VAEXTRADATA)
#define FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR_SIZE	FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR)
#define FDI_SYSTEM_DATA_HDD_SN_SIZE			FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_HDD_SN)
#define FDI_SYSTEM_DATA_IRDETO_PRIVATEDATA_SIZE			FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_IRDETO_PRIVATEDATA)
#define FDI_SYSTEM_DATA_VMX_BC_NSC_DATA_SIZE			FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_BC_NSC_DATA)
typedef void (*pfnFDI_SYSTEM_RF4CE_RSSI_Notify)(unsigned long ulTxRSSI, unsigned long ulRxRSSI);
typedef void (*FDI_SYSTEM_CALLBACK)(void*);

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_Open(void);

/*******************************************************************************
* function : FDI_SYSTEM_GetFieldSize
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned long FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_e eData);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetMicomVersion(unsigned char *pucVersion);

/*******************************************************************************
* function : FDI_SYSTEM_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetAppVersion(unsigned char *pucVersion);

/*******************************************************************************
* function : FDI_SYSTEM_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetLoaderVersion(unsigned char *pucVersion);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetSystemId(unsigned long *pulSystemId);

/*******************************************************************************
* function : FDI_SYSTEM_GetHWRevision
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetHWRevision(unsigned int *bStatus);

/*******************************************************************************
* function : FDI_SYSTEM_GetSCSStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetSCSStatus(BOOL *bStatus);

/*******************************************************************************
* function : FDI_SYSTEM_GetJTAGStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetJTAGStatus(BOOL *bStatus);

/*******************************************************************************
* function : FDI_SYSTEM_GetOTPStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetOTPStatus(BOOL *bStatus);

/*******************************************************************************
* function :	FDI_SYSTEM_GetMarketID
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetMarketID(unsigned int *punMarketID);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetSerialNumber(unsigned char *pucSN);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetSerialNumber(unsigned char *pucSN);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetMacAddress(unsigned char *pucMacAddr);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetMacAddress(unsigned char *pucMacAddr);

#if defined(FACTORY_USE_BT)
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetBTMacAddress(unsigned char *pucMacAddr);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetBTMacAddress(unsigned char *pucMacAddr);
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetWifiMacAddress(unsigned char *pucMacAddr);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetWifiMacAddress(unsigned char *pucMacAddr);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetHdcpKey(unsigned char *pucHdcpkey, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetHdcpKey(unsigned char *pucHdcpkey, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetChipId(unsigned char *pucChipId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetChipIdCheckNumber(unsigned char *pucChipIdChkNo);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetChipId(unsigned char *pucChipId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetIsoCode(unsigned char *pucIsoCode);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetIsoCode(unsigned char *pucIsoCode);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetDtcpKey(unsigned char *pucDtcpkey, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetDtcpKey(unsigned char *pucDtcpkey, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetCiPlusKeyData(unsigned char *pucCiPluskey, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetCiPlusKeyData(unsigned char *pucCiPluskey, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetPairKey(unsigned char *pucPairkey, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetPairKey(unsigned char *pucPairkey, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetDstbId(unsigned char *pucDstbId, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetDstbId(unsigned char *pucDstbId, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_CheckFactoryMode(BOOL *pbFlag);

/*******************************************************************************
* function : FDI_SYSTEM_CheckMesCompleted
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_CheckMesCompleted(BOOL *pbMesCompleted);


/*******************************************************************************
* function : FDI_SYSTEM_GetSCSStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetSCSStatus(BOOL *bStatus);

/*******************************************************************************
* function : FDI_SYSTEM_GetJTAGStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetJTAGStatus(BOOL *bStatus);

/*******************************************************************************
* function : FDI_SYSTEM_GetOTPStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetOTPStatus(BOOL *bStatus);

/*******************************************************************************
* function : FDI_SYSTEM_DefaultSet
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EraseFactoryLoader();

/*******************************************************************************
* function : FDI_SYSTEM_EnableSecureBoot
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableSecureBoot();

/*******************************************************************************
* function : FDI_SYSTEM_EnableJtag
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableJtag();

/*******************************************************************************
* function : FDI_SYSTEM_EnableCwMode
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableCwMode();

/*******************************************************************************
* function : FDI_SYSTEM_EnableOTPLock
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableOTPLock();

/*******************************************************************************
* function : FDI_SYSTEM_EnableOTPChipset
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableOTPChipset();

/*******************************************************************************
* function : FDI_SYSTEM_SetSwReset
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetSwReset(void);

/*******************************************************************************
* function : FDI_SYSTEM_FusingDSC
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_FusingDSC(void);

/*******************************************************************************
* function : FDI_SYSTEM_SetStanby
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetStandby(void);

/*******************************************************************************
* function : FDI_SYSTEM_GetCASerialNumber
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetCASerialNumber(unsigned char *pucSn);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetVAExtraData(unsigned char *pucVAExtraData);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetVAExtraData(unsigned char *pucVAExtraData);

/*******************************************************************************
* function : FDI_SYSTEM_EncryptCCB
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EncryptCCB(BOOL* bSuccess);

/*******************************************************************************
* function : FDI_SYSTEM_SetCSCKey
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetCSCKey(unsigned char *pucCSC, unsigned int csclength);

/*******************************************************************************
* function : FDI_SYSTEM_GetCpuTemperature
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetCpuTemperature(unsigned char ucDeviceId, unsigned long *pulTemp);

/*******************************************************************************
* function : FDI_SYSTEM_SetIEEEAddr
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetIEEEAddr(unsigned char *pucIEEEAddr);

/*******************************************************************************
* function : FDI_SYSTEM_GetIEEEAddr
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetIEEEAddr(unsigned char *pucIEEEAddr);

/*******************************************************************************
* function : FDI_SYSTEM_SetRf4ceFactoryMode
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetRf4ceFactoryMode(void);

/*******************************************************************************
* function : FDI_SYSTEM_RegisterEventNotify
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_RegisterEventNotify(FDI_SYSTEM_EVENT eSysEvent, FDI_SYSTEM_CALLBACK pfSysEventCallback);

/*******************************************************************************
* function : FDI_SYSTEM_SetSystemInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetSystemInfo(FDI_SYSTEM_INFO_e eSysInfo, void *pvInfo, unsigned int ulInfoSize);

/*******************************************************************************
* function : FDI_SYSTEM_RegisterRssiCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_RegisterRssiCallback(pfnFDI_SYSTEM_RF4CE_RSSI_Notify pfnNotify);


/*******************************************************************************
* function : FDI_SYSTEM_GetSTBOwnerId
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetSTBOwnerId(unsigned short *pusSTBOwnerId);

/*******************************************************************************
* function : FDI_SYSTEM_SetSTBOwnerId
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetSTBOwnerId(unsigned short usSTBOwnerId);

/*******************************************************************************
* function : FDI_SYSTEM_SetPrivateData
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetPrivateData(unsigned char *pucData);

/*******************************************************************************
* function : FDI_SYSTEM_GetPrivateData
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetPrivateData(unsigned char *pucData);

/*******************************************************************************
* function : FDI_SYSTEM_SetOtaDnType
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetOtaDnType(FDI_SYSTEM_DOWNLOAD_TYPE_e *peOtaDnType);

#if (VERIMATRIX_BETACRYPT_NSC_ENABLE==YES)
/*******************************************************************************
* function : FDI_SYSTEM_VMXBC_SetValueForInitNScComm
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_VMXBC_SetValueForInitNScComm(
					unsigned char *pucVendorId, unsigned short ucVendorIdLength,
					unsigned char *pucVendorData, unsigned short ucVendorDataLength,
					unsigned char *pucProvId, unsigned short ucProvIdLength,
					unsigned char *pucProvData, unsigned short ucProvDataLength);

/*******************************************************************************
* function : FDI_SYSTEM_GetNSc
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_VMXBC_GetNSc(unsigned char *pucData, unsigned short *pLength);

/*******************************************************************************
* function : FDI_SYSTEM_GetNSc
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_VMXBC_SetNSc(unsigned char *pucData, unsigned short pLength);
#endif

#endif
