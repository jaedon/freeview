/* $Header:   //////di_system.h 1.0   July 28 2008 13:25:52   jhson  $ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_system.h
// Original Author: Son Jung Hee
// File Description: System Header File.
// Module:
// Remarks:
//
//     input:
//
//     output:
//
//     key position:
//
/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_SYSTEM_H__
#define __DI_SYSTEM_H__

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files

#include "di_err.h"
#include "htype.h"

// End Including Headers

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define DI_SYS_OTP_DATA_SECTION_LENGTH		32		/* Same value with NEXUS_OTP_DATASECTION_LEN, Broadcom specific */
#define DI_SYS_OTP_DATA_SECTION_CRC_LENGTH		4       /* Same value with NEXUS_OTP_DATASECTION_CRC_LEN, Broadcom specific */

#define MAX_PROCESS_STATUS_TABLE	100
#define MAX_PROCESS_STRCOMMAND_SIZE	256

#define MAX_PROCESSOR_ARCH_TABLE	16
#define MAX_PROCESSOR_ARCH_SIZE	256

#define DI_SYS_CHIPSET_EXTENTION_SIZE	(2)
#define DI_SYS_CHIPSET_CUT_SIZE	(2)
#define DI_SYS_CERT_REPORT_CHECK_NUM_SIZE (8)

// End #define

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef
typedef struct
{
	HBOOL	bUseMicom;
}DI_SYS_CAPABILITY_t;

typedef struct
{
	HUINT32	ulLoaderVersion;
	HUINT32	ulAppVersion;
	HUINT8	ucModelName[30];
}DI_SYS_MICOM_INFO_t;

typedef struct
{
	HUINT32	ulStartAddress;
	HUINT8	*ucBuffer;
	HUINT32	ulBufferLen;
}DI_SYS_MICOM_UPDATE_INFO_t;

typedef struct
{
	HUINT32	ulStartAddress;
	HUINT8	*ucBuffer;
	HUINT32	ulBufferLen;
}DI_SYS_RF4CE_UPDATE_INFO_t;

typedef struct
{
	HUINT32	ulChipID;
}DI_SYS_CHIP_ID_t;

typedef struct
{
	HUINT32	ulCheckNumber;
}DI_SYS_CHIP_CHECK_NO_t;

typedef struct
{
	HUINT8	ucSCS;
	HUINT8	ucJTAG;
	HUINT8	ucOTP;
	HUINT8	ucCWE;
	HUINT8	ucRAMSCR;
	HUINT8	ucMarketID;
	HUINT32	ulMarketID;
	HUINT32	ulParms[4];
	HUINT8	ucRemuxFusing;
	HUINT8	ucRemuxFusingByValue;
	HUINT16	usSTBOwnerID;
	HUINT8 	ucOTPChipset;
	HUINT8 	ucSCSVersioning;
	HUINT8 	ucFlashProtection;
	HUINT32	ulOTPField1;
	HUINT32	ulOTPField2;
}DI_SYS_FUSING_INFO_t;

typedef struct
{
	HUINT8		ucTemp;
	HUINT32 	ulFanCtrlVoltage;
	HUINT32 	ulFanFeedback_Hz;
}DI_SYS_FAN_INFO_t;

typedef struct
{
    HUINT8 *pucCSCBuffer;
    HUINT32 ulCSCCheckNumber;
}DI_SYS_CSC_CHECKNUM_INFO_t;

typedef struct
{
	HUINT8		ucIndex;
	HUINT8		ucDataSection[DI_SYS_OTP_DATA_SECTION_LENGTH];
	HUINT8		ucDataSectionCRC[DI_SYS_OTP_DATA_SECTION_CRC_LENGTH];
}DI_SYS_OTP_DATA_SECTION_INFO_t;

typedef struct
{
	HINT32		ulTemp;			/* Last measured voltage (in millivolts). To convert use: float V = voltage/1000 */
	HUINT32		ulVoltage;		/* Last measured temperature in thousands. To convert use: float celcius = temperature/1000 */
}DI_SYS_CPU_INFO_t;

typedef enum
{
	DI_SYS_MICOM_INFO = 0,
	DI_SYS_MICOM_VER,
	DI_SYS_MICOM_STRVER,
	DI_SYS_MICOM_UPDATE,
	DI_SYS_CHIP_ID,
	DI_SYS_CHIP_CHECK_NO,
	DI_SYS_FUSING_INFO,
	DI_SYS_SYS_TICK_FOR_BOOT_TIME,
	DI_SYS_SMART_TEMP,
	DI_SYS_HW_REVISION,
	DI_SYS_CSC_CHECKNUMBER,
	DI_SYS_RESTORE_BEP,
	DI_SYS_ERASE_FACTORY_LOADER,
	DI_SYS_FLASH_OTP_LOCK,
	DI_SYS_SCS,
	DI_SYS_FUSE_JTAG,
	DI_SYS_FUSE_DSC,
	DI_SYS_CHIP_REVISION,
	DI_SYS_FUSE_OTP_DATA_SECTION,		/* Broadcom specific to set data section for secure boot on generic chipset */
	DI_SYS_CPU_INFO,					/* Broadcom specific to read CPU temp. and core voltage */
	DI_SYS_RF4CE_UPDATE,
	DI_SYS_STB_CA_SN,
	DI_SYS_BOOT_MODE,
	DI_SYS_CHIP_ID_X64,
	DI_SYS_MICOM_RECOVERY,
	DI_SYS_WIFI_UPDATE,
	DI_SYS_CHIPSET_EXTENSION,
	DI_SYS_CHIPSET_CUT,
	DI_SYS_CERT_REPORT_CHECK_NUM,
	DI_SYS_DUMMY
}DI_SYS_INFO_e;

typedef union
{
	DI_SYS_MICOM_INFO_t		tMicomInfo;
	DI_SYS_CHIP_ID_t		tChipID;
	DI_SYS_CHIP_CHECK_NO_t	tChipCheckNo;
	DI_SYS_FUSING_INFO_t	tFusingInfo;
}DI_SYS_INFO_ID_t;

typedef	enum
{
	DI_SYS_EVENT_CHECK_TEMP,			/**< smart temperature over shutdown threshold ( >68) */
	DI_SYS_EVENT_MICOM_UPDATE_PROGRESS,
	DI_SYS_EVENT_RF4CE_UPDATE_PROGRESS,	/* RF4CE Firmware Update status */
	DI_SYS_EVENT_FAN_LOCKED,
	DI_SYS_EVENT_OVER_TEMPERATURE,
	DI_SYS_EVENT_CM_REBOOT,
	DI_SYS_EVENT_WIFI_UPDATE_PROGRESS,
	DI_SYS_EVENT_END
}DI_SYS_EVENT;

typedef enum
{
	PROCESS_STATE_RUNNING,
	PROCESS_STATE_SLEEPING_INTERRUPTABLE,
	PROCESS_STATE_SLEEPING,
	PROCESS_STATE_ZOMBIE,
	PROCESS_STATE_STOPPED,
	PROCESS_STATE_PAGING
} DI_SYS_PROCESS_STATE_e;

typedef enum
{
	DI_SYS_BOOT_NOT_SET = 0,
	DI_SYS_BOOT_FROM_NOR = 0x2,
	DI_SYS_BOOT_FROM_NAND,
	DI_SYS_BOOT_FROM_SPI,
	DI_SYS_BOOT_MODE_MAX
} DI_SYS_BOOT_MODE_e;

typedef struct
{
	HUINT32	unPid;
	HUINT32	unSize;		 /* KByte */
	HUINT32 unPriority;	 /* max 99, 0 is highset */
	HUINT32 unCPUTime;	 /* mili second */
	HUINT8 strCommand[MAX_PROCESS_STRCOMMAND_SIZE];
	DI_SYS_PROCESS_STATE_e eState;	 /* enum */
} DI_SYS_PROCESS_STATUS_t;

typedef struct
{
	HUINT32	unNumOfEntries;
	DI_SYS_PROCESS_STATUS_t ProcessStatus[MAX_PROCESS_STATUS_TABLE];	 /* 일단 100 개를 max 로 고려 */
} DI_SYS_PROCESS_STATUS_TABLE_t;

typedef struct
{
	HUINT8 strArch[MAX_PROCESSOR_ARCH_SIZE];
} DI_SYS_PROCESSOR_ARCH_t;

typedef struct
{
	HUINT32	unNumOfEntries;
	DI_SYS_PROCESSOR_ARCH_t ProcessorArch[MAX_PROCESSOR_ARCH_TABLE];	 /* 일단 16 개를 max 로 고려 */
} DI_SYS_PROCESSOR_ARCH_TABLE_t;

typedef enum
{
	DI_WIFI_NOTIFY_UPGRADE_EVT_WRITE_START,
	DI_WIFI_NOTIFY_UPGRADE_EVT_WRITE_COMPLETE,
	DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_START,
	DI_WIFI_NOTIFY_UPGRADE_EVT_TRANSFER_COMPLETE,
	DI_WIFI_NOTIFY_UPGRADE_EVT_DOWNLOAD_START,
	DI_WIFI_NOTIFY_UPGRADE_EVT_DOWNLOAD_COMPLETE,
	DI_WIFI_NOTIFY_UPGRADE_EVT_MAX
} DI_WIFI_NOTIFY_UPGRADE_EVT_e;

typedef enum
{
	DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_NONE,
	DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_WRITE_FAIL,
	DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_TRANSFER_FAIL,
	DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_DOWNLOAD_FAIL,
	DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_MAX
} DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_e;

typedef	struct
{
	HUINT32								ulDevId;
	DI_WIFI_NOTIFY_UPGRADE_EVT_e		eEvent;
	HUINT32								ulProgress;
	DI_WIFI_NOTIFY_UPGRADE_ERROR_CODE_e	eErrorCode;
}DI_WIFI_UPGRADE_CALLBACK_DATA_t;

typedef void (*DI_SYS_CALLBACK)(void*);
typedef void (*pfnWIFI_UPGRADE_CALLBACK )(void  *pEventData);
DI_ERR_CODE DRV_WIFI_Update(char *pImage, unsigned int ulImgSize);

// End typedef
/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee


// End global variable

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee

// End static variable


/*******************************************************************/
/*********************** Function Prototype ***********************/
/*******************************************************************/
// Start Function Prototype

DI_ERR_CODE DI_SYSTEM_GetCapability(DI_SYS_CAPABILITY_t *pstSysCapa);
DI_ERR_CODE DI_SYSTEM_GetSystemInfo(DI_SYS_INFO_e eSysInfo, void *pvInfo, HUINT32 ulInfoSize);
DI_ERR_CODE DI_SYSTEM_SetSystemInfo(DI_SYS_INFO_e eSysInfo, void *pvInfo, HUINT32 ulInfoSize);
DI_ERR_CODE DI_SYSTEM_RegisterEventNotify(DI_SYS_EVENT eSysEvent, DI_SYS_CALLBACK pfSysEventCallback);
DI_ERR_CODE DI_SYSTEM_CacheFlush(void);
DI_ERR_CODE DI_SYSTEM_GetCPUUsage(HUINT32 *punCpuUsage);
DI_ERR_CODE DI_SYSTEM_GetProcessStatus(DI_SYS_PROCESS_STATUS_TABLE_t *pProcessStatusTable);
DI_ERR_CODE DI_SYSTEM_GetProcessorInfo(DI_SYS_PROCESSOR_ARCH_TABLE_t *pProcessorArchtecture);

// End Function Prototype

#endif	/* __DI_SYSTEM_H__ */
