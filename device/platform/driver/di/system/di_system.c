/**
 * di_system.c
*/

/**
 * @defgroup		DI_SYSTEM :
 * @author			kyu-young Cho
 * @note
 * @brief
 * @file 			di_system.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include common utils first! */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/unistd.h>
#include "linden_trace.h"
#include "bstd.h"
#if defined(CONFIG_SUPPORT_CPU_TEMP)
#include "nexus_avs.h"
#include "nexus_platform_features.h"
#if defined(NEXUS_AVS_MONITOR) && (NEXUS_AVS_MONITOR > 0)
#else
#include "nexus_temp_monitor.h"
#endif
#endif

/* include drv headers */
#include "drv_micom.h"
#include "vkernel.h"

/* include di headers */
#include "di_err.h"
#include "drv_err.h"
#include "taskdef.h"
#include "drv_csd.h"
#include "drv_fan.h"
#if defined(CONFIG_CABLE_MODEM)
#if defined(CONFIG_CABLE_MODEM_BRCM_DSGCC)
#include "drv_cm_bdsgcc.h"
#endif
#if defined(CONFIG_CABLE_MODEM_BRCM_RPC)
#include "drv_cm_brpc.h"
#endif
#endif
#include "drv_smart.h"
#include "drv_flash.h"
#include "drv_gpio.h"
#include "di_system.h"
#include "drv_csd.h"
#if defined(CONFIG_OTP_LOCK_NORFLASH)
#include "flash_map.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE			TRACE_MODULE_DRV_MISC

#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
#ifndef DEBUG_MODULE
#ifdef DI_SYSTEM_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)		DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)		DI_UART_Print (fmt , ## __VA_ARGS__)
#endif
#endif
#endif

#define MAX_LENGTH_OF_PROCFS_FULLPATH 128
#define MAX_BUF_SIZE_FOR_PROC_STAT 512
#define MAX_BUF_SIZE_FOR_PROC_CPUINFO 1024

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct
{
	long long		llUser;
	long long		llSystem;
	long long		llNice;
	long long		llIdle;
	long long		llWait;
	long long		llHi;
	long long		llSi;
	long long		llZero;

	long long		llTotal;
} P_SYS_CPU_STAT_t;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
#if defined(CONFIG_MICOM) && (CONFIG_MICOM_UPGRADE)
int upgradeStatus = 0;
#endif


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
static DI_SYS_CALLBACK pfSysEventFanLockedCallback=NULL;
static DI_SYS_CALLBACK pfSysEventOverTemperatureCallback=NULL;
static HULONG ulSysProtectionMsgQueueId;
#endif
#if defined(CONFIG_MICOM_UPGRADE)
static DI_SYS_CALLBACK pfSysEventUpgradeCallback=NULL;
#endif

#if defined(CONFIG_QV940)
static DI_SYS_CALLBACK pfSysEventWiFiUpgradeCallback=NULL;
#endif

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
extern void DRV_MICOM_RegisterSystemProtectionCallback(pfnDRV_UCOM_SYSTEM_PROTECTION_Notify pfnNotify);

static void P_SYSTEM_MICOM_SystemProtectionNotify (MICOM_SYSTEM_PROTECTION_e eProtectionReason);
static void P_SYSTEM_Protection_Task(void *pvParam);
#endif
#if	defined(CONFIG_HDD_SMART)
static int P_SYSTEM_GetSmartTemp(int port_num, int *temperature);
#endif

#if defined(CONFIG_SUPPORT_CPU_TEMP)
#if defined(NEXUS_AVS_MONITOR) && (NEXUS_AVS_MONITOR > 0)
#else
#define NEXUS_NUM_TEMP_MONITORS 	1
static NEXUS_TempMonitorHandle	tempMonitorHandle[NEXUS_NUM_TEMP_MONITORS];
static HBOOL					curr_tempMonitorInit[NEXUS_NUM_TEMP_MONITORS];
static NEXUS_TempMonitorStatus	curr_tempMonitorStatus[NEXUS_NUM_TEMP_MONITORS];

static void P_SYSTEM_CpuTempCallback(void *context, int param)
{
	int rc;
	NEXUS_TempMonitorStatus tempMonitorStatus;
	NEXUS_TempMonitorHandle tempMonitor = context;

	if (param >= NEXUS_NUM_TEMP_MONITORS)
		return;

	rc = NEXUS_TempMonitor_GetStatus(tempMonitor, &tempMonitorStatus);
	if (!tempMonitorStatus.valid)
	{
		curr_tempMonitorInit[param] = FALSE;
	}
	else
	{
		curr_tempMonitorInit[param] = TRUE;
		curr_tempMonitorStatus[param] = tempMonitorStatus;
	//		VK_snprintf(tempString, 299, "CPU Temp(%d) = %d C\n", param, curr_tempMonitorStatus.onChipTemperature);
	}
}
#endif
#endif

#if defined(CONFIG_QV940)
extern DI_ERR_CODE DRV_WIFI_Update(char *pImage, unsigned int ulImgSize);
#endif

/********************************************************************
 Function   : DRV_SYSTEM_Init
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_SYSTEM_Init(void)
{
	DRV_Error	nDrvErr = DRV_OK;
#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
	HULONG		ulSysProtectTaskId;
#endif

	PrintEnter();

#if defined(CONFIG_CRYPTO)
	nDrvErr = DRV_CSDInitialize();
	if( nDrvErr !=  DRV_OK )
	{
		PrintError("error in DRV_CSDInitialize (%d)\n", nDrvErr);
		return	DRV_ERR;
	}
#endif

#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
	if(VK_MSG_Create(10, sizeof(MICOM_SYSTEM_PROTECTION_e),"SysProtectionQ",&ulSysProtectionMsgQueueId,VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		PrintError("[%s] Creating System Protection Msg Queue failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	if(VK_TASK_Create(P_SYSTEM_Protection_Task, SYSTEM_PROTECTION_TASK_PRIORITY, SYSTEM_PROTECTION_TASK_STACK_SIZE, "SysProtectionTask", (void *)0, &ulSysProtectTaskId, FALSE)!=VK_OK)
	{
		PrintError("[%s] Creating System Protection Task failed!\n",__FUNCTION__);
		return DI_ERR_ERROR;
	}
	VK_TASK_Start(ulSysProtectTaskId);

	DRV_MICOM_RegisterSystemProtectionCallback(P_SYSTEM_MICOM_SystemProtectionNotify);
#endif

#if defined(CONFIG_SUPPORT_CPU_TEMP)
#if defined(NEXUS_AVS_MONITOR) && (NEXUS_AVS_MONITOR > 0)
#else
	{
		NEXUS_Error					nexusError;
		NEXUS_TempMonitorSettings	tempMonitorSettings;
		HINT32						i;

		for (i = 0; i < NEXUS_NUM_TEMP_MONITORS; i++)
		{
			tempMonitorHandle[i] = NEXUS_TempMonitor_Open(i, NULL);
			if (tempMonitorHandle[i] == NULL)
			{
				PrintError("[%s] NEXUS_TempMonitor_Open error\n", __FUNCTION__);
			}

			NEXUS_TempMonitor_GetSettings(tempMonitorHandle[i], &tempMonitorSettings);
			tempMonitorSettings.enabled = true;
			tempMonitorSettings.dataReady.callback = P_SYSTEM_CpuTempCallback;
			tempMonitorSettings.dataReady.context = tempMonitorHandle[i];
			tempMonitorSettings.dataReady.param = i;
			tempMonitorSettings.measureInterval = 10000;	/* 10 seconds */
			nexusError = NEXUS_TempMonitor_SetSettings(tempMonitorHandle[i], &tempMonitorSettings);
			if (nexusError != 0)
			{
				PrintError("[%s] NEXUS_TempMonitor_SetSettings error(0x%x)\n", __FUNCTION__, nexusError);
			}
		}
	}
#endif
#endif


	PrintExit();

	return DRV_OK;
}

#if defined(CONFIG_SUPPORT_CPU_TEMP)
DRV_Error DRV_SYSTEM_GetCPUInfo(DI_SYS_CPU_INFO_t *pCPUInfo)
{
#if defined(NEXUS_AVS_MONITOR) && (NEXUS_AVS_MONITOR > 0)
	NEXUS_AvsStatus avsStatus;
	NEXUS_Error error;

	error = NEXUS_GetAvsStatus(&avsStatus);
	if (error != NEXUS_SUCCESS)
	{
		PrintError("[DRV_SYSTEM_GetCPUInfo] NEXUS_GetAvsStatus Error(0x%x)\n", error);

		return DRV_ERR;
	}

	pCPUInfo->ulVoltage = avsStatus.voltage;
	pCPUInfo->ulTemp	= (HINT32)avsStatus.temperature;
#else
	if (curr_tempMonitorInit[0] == TRUE)
	{
		pCPUInfo->ulVoltage = 0;
		pCPUInfo->ulTemp	= curr_tempMonitorStatus[0].onChipTemperature*1000;
	}
	else
	{
		pCPUInfo->ulVoltage = 0;
		pCPUInfo->ulTemp	= 0;
		return DRV_ERR;
	}
#endif
	return DRV_OK;
}
#endif

#if defined(CONFIG_OTP_LOCK_NORFLASH)
static DRV_Error P_SYSTEM_GetOPTBlock(HUINT8 *pucOTP)
{
#if defined(CONFIG_DI10)
	HUINT8 otp_flag[2];
	HUINT32 cfe_addr, config_addr;
	HUINT32 block_size, block_no, total_size;
	DRV_Error	nDrvErr = DRV_OK;


	PrintEnter();

	DRV_FLASH_GetCapabilities(&block_size, &block_no);
	total_size = block_size * block_no;

	/* CFE 위치는 마지막 4M에 위치함.. */
	cfe_addr 	= total_size - (4*1024*1024);
	config_addr = cfe_addr + CONFIG_BASE;

	nDrvErr =  DRV_FLASH_GetOTPBlock(cfe_addr,  &otp_flag[0]);
	nDrvErr |= DRV_FLASH_GetOTPBlock(config_addr,  &otp_flag[1]);
	if( nDrvErr !=  DRV_OK )
	{
		PrintError("error in DRV_FLASH_GetOTPBlock (%d)\n", nDrvErr);
		return	DRV_ERR;
	}

	*pucOTP = otp_flag[0] & otp_flag[1];

#else

	HINT32 otp_locked_block = 0;
	DRV_Error nDrvErr = DRV_OK;
	
	PrintEnter();
	nDrvErr = DRV_FLASH_GetOTPArea(NOR_OTP_ALL_OFFSET, NOR_OTP_ALL_SIZE, &otp_locked_block);
	if( nDrvErr !=  DRV_OK )
	{
		PrintError("error in DRV_FLASH_GetOTPArea (%d)\n", nDrvErr);
		return	DRV_ERR;
	}

	if(otp_locked_block == NOR_OTP_BLOCK_NUMBER)
	{
		*pucOTP = 1; 
	}
	else
	{
		*pucOTP = 0; 
	}
	PrintDebug("Number of OTP blocks : %d\n", otp_locked_block);
#endif
	PrintExit();
	return DRV_OK;
}


static DRV_Error P_SYSTEM_SetOPTBlock(void)
{
#if defined(CONFIG_DI10)
	HUINT32 cfe_addr, config_addr;
	HUINT32 block_size, block_no, total_size;
	DRV_Error	nDrvErr = DRV_OK;

	PrintEnter();

	DRV_FLASH_GetCapabilities(&block_size, &block_no);
	total_size = block_size * block_no;

	/* CFE 위치는 마지막 4M에 위치함.. */
	cfe_addr 	= total_size - (4*1024*1024);
	config_addr = cfe_addr + CONFIG_BASE;

	nDrvErr =  DRV_FLASH_SetOTPBlock(cfe_addr);
	nDrvErr |= DRV_FLASH_SetOTPBlock(config_addr);
	if( nDrvErr !=  DRV_OK )
	{
		PrintError("error in DRV_FLASH_SetOTPBlock (%d)\n", nDrvErr);
		return	DRV_ERR;
	}

#else

	HINT32 otp_locked_block = 0;
	HUINT32 i = 0;
	DRV_Error nDrvErr = DRV_OK;

	PrintEnter();

	for(i=0; i< NOR_OTP_AREA_MAX; i++){
		nDrvErr = DRV_FLASH_SetOTPArea(nor_otp_mtd[i].offset, nor_otp_mtd[i].size);
		if(nDrvErr != DRV_OK)
		{
			nDrvErr = DRV_FLASH_SetOTPArea(nor_otp_mtd[i].offset, nor_otp_mtd[i].size);	/* try to lock the flash otp one more  */
			if(nDrvErr != DRV_OK)
			{
				PrintError("set otp error\n");
				return DRV_ERR;
			}
		}
	}

	nDrvErr = DRV_FLASH_GetOTPArea(NOR_OTP_ALL_OFFSET, NOR_OTP_ALL_SIZE, &otp_locked_block);
	if(otp_locked_block != NOR_OTP_BLOCK_NUMBER)
	{
		PrintError("OTP block number should be %d.\n", NOR_OTP_BLOCK_NUMBER);
		return DRV_ERR;
	}
#endif
	PrintExit();
	return DRV_OK;
}
#endif

#if defined(CONFIG_MICOM_UPGRADE)
DRV_Error DRV_SYSTEM_EventCallback(DI_SYS_EVENT eSysEvent, HUINT32 ulProgress)
{
	if (eSysEvent >= DI_SYS_EVENT_END)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if (pfSysEventUpgradeCallback == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	pfSysEventUpgradeCallback((void *)ulProgress);

	return DRV_OK;
}
#endif

#if defined(CONFIG_QV940)
DRV_Error DRV_SYSTEM_EventCallbackForWiFi(DI_SYS_EVENT eSysEvent, HUINT32 ulProgress)
{
	if (eSysEvent >= DI_SYS_EVENT_END)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	if (pfSysEventWiFiUpgradeCallback == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	pfSysEventWiFiUpgradeCallback((void *)ulProgress);

	return DRV_OK;
}
#endif

/********************************************************************
 Function   : DI_SYSTEM_GetCapability
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DI_ERR_CODE DI_SYSTEM_GetCapability(DI_SYS_CAPABILITY_t *pstSysCapa)
{

	PrintEnter();

#if defined(CONFIG_FRONT_MICOM_EU) || defined(CONFIG_FRONT_MICOM_JAPAN) || defined(CONFIG_FRONT_MICOM_TOUCH)
	pstSysCapa->bUseMicom = TRUE;
#else
	pstSysCapa->bUseMicom = FALSE;
#endif

	PrintExit();

	return DI_ERR_OK;
}


/********************************************************************
 Function   : DI_SYSTEM_GetSystemInfo
 Description :
 Input      : 	eSysInfo: 필요시 enum 추가,
 				pvInfo: struct, variable 등 다양하게 추가함.
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DI_ERR_CODE DI_SYSTEM_GetSystemInfo(DI_SYS_INFO_e eSysInfo, void *pvInfo, HUINT32 ulInfoSize)
{
	DI_SYS_MICOM_INFO_t		tMicomInfo;
	DI_SYS_FUSING_INFO_t	*pFusingInfo;
	unsigned int			ulMicomVer = 0;
	GPIO_ID_t 				eGpioId;
	GPIO_STATE_t			eGpioState;
	unsigned int 			unHwRev;
//    unsigned char           *pCSCBuf = NULL;
//    unsigned int            cscCheckNum = 0;
    DI_SYS_CSC_CHECKNUM_INFO_t *pCSCCheckNum=NULL;
#if	defined(CONFIG_FRONT_MICOM_JAPAN)
	unsigned int			ulBootTimeTick = 0;
#endif
	DRV_Error				nDrvErr = DRV_OK;
	unsigned char		ChipRev[2];
	unsigned char		StrVer[11];
	unsigned char		StrChipId_x64[8];
	DI_SYS_OTP_DATA_SECTION_INFO_t	*pOtpDataSectionInfo = NULL;
#if defined(CONFIG_SUPPORT_CPU_TEMP)
	DI_SYS_CPU_INFO_t *pCPUInfo = NULL;
#endif
#if defined(CONFIG_HDD_SMART)
	HINT32 ulHDDTemp = 0;
#endif
#if defined(CONFIG_CRYPTO) && defined(CONFIG_CAS_NA)
	DI_SYS_BOOT_MODE_e *pBootMode = NULL;
#endif

	PrintEnter();

	if( pvInfo == NULL )
	{
		PrintError("parm is NULL!!\n");
		return	DI_ERR_ERROR;
	}

	switch(eSysInfo)
	{
		case DI_SYS_MICOM_INFO:
			VK_memset(&tMicomInfo, 0x00, sizeof(DI_SYS_MICOM_INFO_t));

			//TODO :: Get Other Micom Info

			VK_memcpy(pvInfo, &tMicomInfo, sizeof(DI_SYS_MICOM_INFO_t));
			break;

		case DI_SYS_MICOM_VER:
			nDrvErr = DRV_MICOM_GetVersion(&ulMicomVer);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_MICOM_GetVersion() \n", nDrvErr);
				return	DI_ERR_ERROR;
			}
			VK_memcpy(pvInfo, &ulMicomVer, ulInfoSize);
			break;

		case DI_SYS_MICOM_STRVER:
			nDrvErr = DRV_MICOM_GetStrVersion(StrVer);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_MICOM_GetStrVersion() \n", nDrvErr);
				return DI_ERR_ERROR;
			}
			VK_memcpy(pvInfo, StrVer, ulInfoSize);
			break;

		case DI_SYS_CHIP_ID:
#if defined(CONFIG_CRYPTO)
			nDrvErr = DRV_CSDGetChipID((HUINT32 *)pvInfo);
#endif
			break;

		case DI_SYS_CHIP_ID_X64:
#if defined(CONFIG_CRYPTO)
			nDrvErr = DRV_CSDGetChipIDx64(StrChipId_x64);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_CSDGetChipIDx64() \n", nDrvErr);
				return	DI_ERR_ERROR;
			}
			VK_memcpy(pvInfo, StrChipId_x64, ulInfoSize);
#endif
			break;

		case DI_SYS_CHIP_CHECK_NO:
#if defined(CONFIG_CRYPTO)
			nDrvErr = DRV_CSDGetCheckNumber((HUINT32 *)pvInfo);
#endif
			break;

		case DI_SYS_FUSING_INFO:
#if defined(CONFIG_CRYPTO)
			pFusingInfo = (DI_SYS_FUSING_INFO_t *)pvInfo;

			VK_memset((void *)pFusingInfo, 0, sizeof(DI_SYS_FUSING_INFO_t));
			nDrvErr =  DRV_CSDGetSCS(&pFusingInfo->ucSCS);
			nDrvErr |= DRV_CSDGetJTAG(&pFusingInfo->ucJTAG);
			nDrvErr |= DRV_CSDGetCWE(&pFusingInfo->ucCWE);
			nDrvErr |= DRV_CSDGetRamScrambling(&pFusingInfo->ucRAMSCR);
			{
				HUINT8 ucMarketID32bit[4];
				nDrvErr |= DRV_CSDGetMarketID(ucMarketID32bit);
				pFusingInfo->ulMarketID = (ucMarketID32bit[0] << 24) + (ucMarketID32bit[1] << 16) + (ucMarketID32bit[2] << 8) + ucMarketID32bit[3];
				pFusingInfo->ucMarketID = ucMarketID32bit[3];
			}
			nDrvErr |= DRV_CSDGetSTBOwnerID(&pFusingInfo->usSTBOwnerID);
#if defined(CONFIG_OTP_LOCK_NORFLASH)
			nDrvErr |= P_SYSTEM_GetOPTBlock(&pFusingInfo->ucOTP);
#endif
#if defined(CONFIG_CAS_CX)
			nDrvErr |= DRV_CSDGetOTPChipset(&pFusingInfo->ucOTPChipset);
			nDrvErr |= DRV_CSDGetOTPField(&pFusingInfo->ulOTPField1, &pFusingInfo->ulOTPField2);
#endif
#endif
			nDrvErr |= DRV_CSDGetSCSVersioning(1, &pFusingInfo->ucSCSVersioning);
			nDrvErr |= DRV_CSDGetFlashProtection(&pFusingInfo->ucFlashProtection);
			break;

#if	defined(CONFIG_FRONT_MICOM_JAPAN)
		case DI_SYS_SYS_TICK_FOR_BOOT_TIME:
			DRV_MICOM_GetBootTick(&ulBootTimeTick);
			VK_memcpy(pvInfo, &ulBootTimeTick, ulInfoSize);
			break;
#endif

#if	defined(CONFIG_HDD_SMART)
		case DI_SYS_SMART_TEMP:
			if (P_SYSTEM_GetSmartTemp(ATA_FIRST_HDD_PORT_NUM, &ulHDDTemp) < 0) {
				PrintError("Error(0x%x) In P_SYSTEM_GetSmartTemp(-1) \n");
				return	DI_ERR_ERROR;
			}
			VK_memcpy(pvInfo, &ulHDDTemp, ulInfoSize);
			break;
#endif

		case DI_SYS_HW_REVISION:
			unHwRev = 0;
			for( eGpioId=GPIO_ID_HW_REV0; eGpioId<=GPIO_ID_HW_REV3; eGpioId++ )
			{
				nDrvErr = DRV_GPIO_Read(eGpioId, &eGpioState);
				if( nDrvErr == DRV_OK && eGpioState == GPIO_HI )
				{
					unHwRev += 1;
				}
				unHwRev <<= 1;
			}

			VK_memcpy(pvInfo, &unHwRev, ulInfoSize);
			nDrvErr = DRV_OK;
			break;

		case DI_SYS_CHIP_REVISION:
#if defined(CONFIG_CRYPTO)
			nDrvErr = DRV_CSDGetChipRev(ChipRev);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_CSDGetChipRev() \n", nDrvErr);
				return	DI_ERR_ERROR;
			}

			VK_memcpy(pvInfo, ChipRev, 2);
#endif
			break;

		case DI_SYS_CSC_CHECKNUMBER:
#if defined(CONFIG_CRYPTO)
			pCSCCheckNum =  (DI_SYS_CSC_CHECKNUM_INFO_t *)pvInfo;
			nDrvErr = DRV_CSDGetCSCCheckNumber((HUINT32 *)&pCSCCheckNum->ulCSCCheckNumber, pCSCCheckNum->pucCSCBuffer);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_CSDGetCSCCheckNumber() \n", nDrvErr);
				return	DI_ERR_ERROR;
			}
#endif
			break;
		case DI_SYS_STB_CA_SN:
#if defined(CONFIG_CAS_NA)
			{
				HUINT8 StbCaSn[4];
#if 1 // Read CA_SN data from flash 
				nDrvErr = DRV_CSDGetStbCaSnByPK(StbCaSn);
#else // Read CA_SN data from chipset
				nDrvErr = DRV_CSDGetStbCaSn(StbCaSn);
#endif
				if (nDrvErr != DRV_OK)
				{
					PrintError("Error(0x%x) In DRV_CSDGetStbCaSnByPK() \n", nDrvErr);
					return	DI_ERR_ERROR;
				}
				*(HUINT32 *)pvInfo = (StbCaSn[0] << 24) | (StbCaSn[1] << 16) | (StbCaSn[2] << 8) | StbCaSn[3];
			}
#endif
			break;
		case DI_SYS_FUSE_OTP_DATA_SECTION:
#if defined(CONFIG_CRYPTO)
			pOtpDataSectionInfo = (DI_SYS_OTP_DATA_SECTION_INFO_t *)pvInfo;
			nDrvErr = DRV_CSDGetOtpDataSection((unsigned char)pOtpDataSectionInfo->ucIndex, (unsigned char *)&pOtpDataSectionInfo->ucDataSection[0]);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_CSDGetOtpDataSection() \n", nDrvErr);
				return	DI_ERR_ERROR;
			}
#endif
			break;
		case DI_SYS_CPU_INFO:
#if defined(CONFIG_SUPPORT_CPU_TEMP)
			pCPUInfo = (DI_SYS_CPU_INFO_t *)pvInfo;
			nDrvErr = DRV_SYSTEM_GetCPUInfo(pCPUInfo);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_SYSTEM_GetCPUInfo() \n", nDrvErr);
				return	DI_ERR_ERROR;
			}
#endif
			break;
		case DI_SYS_BOOT_MODE:
#if defined(CONFIG_CRYPTO) && defined(CONFIG_CAS_NA)
			pBootMode = (DI_SYS_BOOT_MODE_e *)pvInfo;
			nDrvErr = DRV_CSDGetBootMode((DRV_CSD_BootMode *)pBootMode);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_CSDGetBootMode(-1) \n", nDrvErr);
				return	DI_ERR_ERROR;
			}
			VK_memcpy(pvInfo, pBootMode, 1);
#endif
			break;

		case DI_SYS_CHIPSET_EXTENSION:
#if defined(CONFIG_CRYPTO) && defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
			nDrvErr = DRV_CSDGetChipSetExtension((HUINT8 *)pvInfo);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_ChipSetExtention() \n", nDrvErr);
				return DI_ERR_ERROR;
			}
#endif
			break;

		case DI_SYS_CHIPSET_CUT:
#if defined(CONFIG_CRYPTO) && defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
			nDrvErr = DRV_CSDGetChipSetCut((HUINT8 *)pvInfo);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_ChipSetCut() \n", nDrvErr);
				return DI_ERR_ERROR;
			}
#endif
			break;

		case DI_SYS_CERT_REPORT_CHECK_NUM:
#if defined(CONFIG_CRYPTO) && defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
			nDrvErr = DRV_CSDGetCertCheckNumber((HUINT8 *)pvInfo);
			if (nDrvErr != DRV_OK)
			{
				PrintError("Error(0x%x) In DRV_dptGetCertCheckNumber() \n", nDrvErr);
				return DI_ERR_ERROR;
			}
#endif
			break;

		default:
			break;
	}

	PrintExit();

	return nDrvErr;
}

/********************************************************************
 Function   : DI_SYSTEM_SetSystemInfo
 Description :
 Input      : 	eMode: 필요시 enum 추가,
 				pvInfo: struct, variable 등 다양하게 추가함.
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DI_ERR_CODE DI_SYSTEM_SetSystemInfo(DI_SYS_INFO_e eSysInfo, void *pvInfo, HUINT32 ulInfoSize)
{
	DI_SYS_FUSING_INFO_t	*pFusingInfo = NULL;
	DRV_Error				nDrvErr = DRV_OK;
	DI_SYS_OTP_DATA_SECTION_INFO_t	*pOtpDataSectionInfo = NULL;

	PrintEnter();

	if( pvInfo == NULL )
	{
		PrintError("parm is NULL!!\n");
		return	DI_ERR_ERROR;
	}

	switch(eSysInfo)
	{
		case DI_SYS_MICOM_INFO:
			break;

		case DI_SYS_FUSING_INFO:
			pFusingInfo = (DI_SYS_FUSING_INFO_t *)pvInfo;

#if defined(CONFIG_CRYPTO)
			if( pFusingInfo->ucSCS != FALSE )
			{
				nDrvErr |= DRV_CSDSetSCS(pFusingInfo->ulParms[0], pFusingInfo->ulParms[1]);
			}

			if( pFusingInfo->ucJTAG != FALSE )
			{
				nDrvErr |= DRV_CSDSetJTAG();
			}

			if( pFusingInfo->ucCWE != FALSE )
			{
				nDrvErr |= DRV_CSDSetCWE();
			}

			if( pFusingInfo->ucRAMSCR != FALSE )
			{
				nDrvErr |= DRV_CSDSetRamScrambling();
			}

			if( pFusingInfo->usSTBOwnerID != 0 )
			{
				nDrvErr |= DRV_CSDSetSTBOwnerID(pFusingInfo->usSTBOwnerID);
			}
#if defined(CONFIG_OTP_LOCK_NORFLASH)
			if( pFusingInfo->ucOTP != FALSE )
			{
				nDrvErr |= P_SYSTEM_SetOPTBlock();
			}
#endif
			if( pFusingInfo->ucOTPChipset != FALSE)
			{
#if defined(CONFIG_CAS_CX)
				nDrvErr |= DRV_CSDSetOTPChipset();
#endif
			}
			if( pFusingInfo->ulMarketID != 0)
			{
				nDrvErr |= DRV_CSDSetMarketID(pFusingInfo->ulMarketID);
			}
#endif

			if( pFusingInfo->ucSCSVersioning != 0 )
			{
				nDrvErr |= DRV_CSDSetSCSVersioning(1, &pFusingInfo->ucSCSVersioning);
			}

			if( pFusingInfo->ucFlashProtection != FALSE )
			{
				nDrvErr |= DRV_CSDSetFlashProtection();
			}

			break;

		case DI_SYS_FUSE_OTP_DATA_SECTION:
#if defined(CONFIG_CRYPTO)
			pOtpDataSectionInfo = (DI_SYS_OTP_DATA_SECTION_INFO_t *)pvInfo;
			nDrvErr = DRV_CSDSetOtpDataSection((HUINT8)pOtpDataSectionInfo->ucIndex, (HUINT8 *)&pOtpDataSectionInfo->ucDataSection[0], (HUINT8 *)&pOtpDataSectionInfo->ucDataSectionCRC[0]);
#endif
			break;

#if defined(CONFIG_MICOM_UPGRADE)
		case DI_SYS_MICOM_UPDATE :
#if defined (CONFIG_MICOM)
			upgradeStatus = 1;
#endif
			nDrvErr = DRV_MICOM_Update(pvInfo, ulInfoSize);
#if defined (CONFIG_MICOM)
			upgradeStatus = 0;
#endif
			break;
#endif

		case DI_SYS_WIFI_UPDATE:
#if defined(CONFIG_QV940)
			nDrvErr = DRV_WIFI_Update(pvInfo, ulInfoSize);
#endif
			break;

		default:
			pvInfo = pvInfo;
			ulInfoSize = ulInfoSize;
			break;
	}

	PrintExit();

	return nDrvErr;
}

/********************************************************************
 Function   : DI_SYSTEM_RegisterEventNotify
 Description :
 Input      : 	eSysEvent: callback func. 등록하고자 하는 system event
 				pfSysEventCallback: 등록하고자 하는 event callback func.
 Output     : none
 Return     : DI_ERR_OK
 ********************************************************************/
DI_ERR_CODE DI_SYSTEM_RegisterEventNotify(DI_SYS_EVENT eSysEvent, DI_SYS_CALLBACK pfSysEventCallback)
{
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

#if	(defined(CONFIG_HDD_SMART) || defined(CONFIG_FAN)) && !defined(CONFIG_DI_SHARED_LIB)
	if (eSysEvent == DI_SYS_EVENT_CHECK_TEMP)
	{
		DRV_FAN_RegisterEventNotify(DRV_FAN_EVENT_CHECK_TEMP, (DRV_FAN_CALLBACK)pfSysEventCallback);
	}
	else if (eSysEvent == DI_SYS_EVENT_FAN_LOCKED)
	{
		DRV_FAN_RegisterEventNotify(DRV_FAN_EVENT_FAN_LOCKED, (DRV_FAN_CALLBACK)pfSysEventCallback);
	}
	else if (eSysEvent == DI_SYS_EVENT_OVER_TEMPERATURE)
	{
		DRV_FAN_RegisterEventNotify(DRV_FAN_EVENT_OVER_TEMPERATURE, (DRV_FAN_CALLBACK)pfSysEventCallback);
	}
	else if (eSysEvent == DI_SYS_EVENT_CM_REBOOT)
	{
#if defined(CONFIG_CABLE_MODEM)
		DRV_CM_RegisterEventNotify(DRV_CM_EVENT_REBOOT, (DRV_CM_CALLBACK)pfSysEventCallback);
#endif
	}
	else if (eSysEvent == DI_SYS_EVENT_WIFI_UPDATE_PROGRESS)
	{
#if defined(CONFIG_QV940)
		pfSysEventWiFiUpgradeCallback = pfSysEventCallback;
#endif
	}
	else
	{
		rc=DI_ERR_ERROR;
	}
#else
#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
	if (eSysEvent == DI_SYS_EVENT_FAN_LOCKED)
	{
		pfSysEventFanLockedCallback = pfSysEventCallback;
	}
	else if (eSysEvent == DI_SYS_EVENT_OVER_TEMPERATURE)
	{
		pfSysEventOverTemperatureCallback = pfSysEventCallback;
	}
	else if (eSysEvent == DI_SYS_EVENT_CM_REBOOT)
	{
#if defined(CONFIG_CABLE_MODEM)
		PrintError("[%s] CM rebooting callback is registered \n", __func__);
		DRV_CM_RegisterEventNotify(DRV_CM_EVENT_REBOOT, (DRV_CM_CALLBACK)pfSysEventCallback);
#endif
	}
	else
	{
		rc=DI_ERR_ERROR;
	}
#else
#if defined(CONFIG_MICOM_UPGRADE)
	if (eSysEvent == DI_SYS_EVENT_MICOM_UPDATE_PROGRESS)
	{
		pfSysEventUpgradeCallback = pfSysEventCallback;
	}
	else
	{
		rc=DI_ERR_ERROR;
	}
#else
	BSTD_UNUSED(eSysEvent);
	BSTD_UNUSED(pfSysEventCallback);

	PrintError("Not Supported Funtion for not using CONFIG_HDD_SMART define . Why did you call %s ??? \n", __func__);
#endif
#endif
#endif

	PrintExit();

	return rc;
}

#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
static void P_SYSTEM_MICOM_SystemProtectionNotify (MICOM_SYSTEM_PROTECTION_e eProtectionReason)
{
	HINT32 nVkRet = VK_OK;

	nVkRet = VK_MSG_Send(ulSysProtectionMsgQueueId, &eProtectionReason, sizeof(MICOM_SYSTEM_PROTECTION_e));
	if (nVkRet != VK_OK)
	{
		PrintError("P_SYSTEM_MICOM_SystemProtectionNotify() can not send msg (%d)!!\n", eProtectionReason);
	}
}

static void P_SYSTEM_Protection_Task(void *pvParam)
{
	int nVkRet;
	MICOM_SYSTEM_PROTECTION_e eProtectionReason;
	DI_SYS_EVENT eSystemEvent;

	HAPPY(pvParam);

	while(1)
	{
		nVkRet = VK_MSG_Receive(ulSysProtectionMsgQueueId, &eProtectionReason, sizeof(MICOM_SYSTEM_PROTECTION_e));
		if (nVkRet != VK_OK)
		{
			continue;
		}

		switch (eProtectionReason)
		{
			case FAN_LOCKED_DETECTION:
				PrintError("FATAL - FAN_LOCKED_DETECTION is detected!!!!! (%d)!!\n", eProtectionReason);
				if (pfSysEventFanLockedCallback != NULL)
				{
					eSystemEvent = DI_SYS_EVENT_FAN_LOCKED;
					(*pfSysEventFanLockedCallback)(&eSystemEvent);
				}
				break;

			case OVER_TEMPERATURE_DETECTION:
				PrintError("FATAL - OVER_TEMPERATURE_DETECTION is detected!!!!! (%d)!!\n", eProtectionReason);
				if (pfSysEventOverTemperatureCallback != NULL)
				{
					eSystemEvent = DI_SYS_EVENT_OVER_TEMPERATURE;
					(*pfSysEventOverTemperatureCallback)(&eSystemEvent);
				}
				break;

			default:
				break;
		}
	}
}
#endif

#if	defined(CONFIG_HDD_SMART)
static int P_SYSTEM_GetSmartTemp(int port_num, int *temperature)
{
#define SMART_CMD			(0x031F)
#define SMART_BUF_SIZE 		(516)
#define TEMP_INDEX 			(204)
#define SD_DEV_MAX			(8)
	char path[256] = {0,};
	char link[256] = {0,};
	int i;
	int len;
	int fd = -1;
	unsigned char buf[SMART_BUF_SIZE];
	int rc = -1;

	/* default value */
	*temperature = 51;

	DONE_ERR((port_num < 1));	

	for(i=0; i<SD_DEV_MAX; i++)
	{
		VK_MEM_Memset(path, 0, sizeof(path));
		VK_MEM_Memset(link, 0, sizeof(link));

		VK_snprintf(path, sizeof(path)-1, "/sys/block/sd%c/device", 'a'+i);
		len = readlink(path, link, sizeof(link)-1);

		if (len > 0 && (unsigned long)len < sizeof(link))
		{
			link[len] = '\0';
			if (VK_strstr(link, "ahci"))
			{
				break;
			}
		}

	}
	DONE_ERR((i == SD_DEV_MAX));

	VK_snprintf(path, sizeof(path)-1, "/dev/sd%c", 'a'+i);
	fd = open(path, O_RDONLY | O_NONBLOCK);
	DONE_ERR((fd < 0));

	VK_MEM_Memset(buf, 0, sizeof(buf));
	buf[0] = 0xb0;
	buf[1] = 0xe0;
	buf[2] = 0xd5;
	buf[3] = 1;
	buf[4] = 0x4f;
	buf[5] = 0xc2;

	rc = ioctl(fd, SMART_CMD, &buf);
	DONE_ERR((rc < 0));

	*temperature = (buf[TEMP_INDEX] & 0x80) ? (int)(0xFFFFFF00 | buf[TEMP_INDEX]) : (int)buf[TEMP_INDEX];

	rc = 0;
done:	
	if (fd >= 0) close(fd);
	return rc;
}
#endif

static HINT32 P_SYSTEM_IsSpaceChar(HUINT8 ucData)
{
	switch(ucData)
	{
		case ' ':
		case '\t':
		case '\n':
			return 1;
	}

	return 0;
}

static HINT32 P_SYSTEM_GetNumber64(char *pcData, long long *pnNum)
{
	HINT32 nIndex = 0;

	*pnNum = 0;

	while (P_SYSTEM_IsSpaceChar(pcData[nIndex]) == 1)
	{
		nIndex++;
	}
	while (P_SYSTEM_IsSpaceChar(pcData[nIndex]) == 0)
	{
		if (pcData[nIndex] < '0' || pcData[nIndex] > '9')
		{
			return 0;
		}

		*pnNum *= 10;
		*pnNum += pcData[nIndex] - '0';
		nIndex++;
	}

	return nIndex;
}

static DRV_Error P_SYSTEM_ReadCPUStat(P_SYS_CPU_STAT_t *pstCpuStat)
{
	HCHAR acData[64];
	HINT32 nReadSize = 0;
	HINT32 nIndex = 0;
	HINT32 nLen = 0;
	HINT32 nFd;

	if (pstCpuStat == NULL)
	{
		return DRV_ERR;
	}

	nFd = open("/proc/stat", O_RDONLY, 0);
	if  (nFd < 0)
	{
		return DRV_ERR;
	}

	do {
		nReadSize = read(nFd, acData, 64);
	} while (-1 == nReadSize && EINTR == errno);

	if ((nReadSize < 64) ||(VK_strncmp("cpu", &(acData[0]), 3) != 0))
	{
		close(nFd);
		return DRV_ERR;
	}

	nIndex = 3;

	nLen = P_SYSTEM_GetNumber64(&(acData[nIndex]), &(pstCpuStat->llUser));
	if(nLen == 0)
	{
		close(nFd);
		return DRV_ERR;
	}
	if(nIndex + nLen >= nReadSize)
	{
		close(nFd);
		return DRV_ERR;
	}
	nIndex += nLen;

	//
	nLen = P_SYSTEM_GetNumber64(&(acData[nIndex]), &(pstCpuStat->llNice));
	if(nLen == 0)
	{
		close(nFd);
		return DRV_ERR;
	}
	if(nIndex + nLen >= nReadSize)
	{
		close(nFd);
		return DRV_ERR;
	}
	nIndex += nLen;

	//
	nLen = P_SYSTEM_GetNumber64(&(acData[nIndex]), &(pstCpuStat->llSystem));
	if(nLen == 0)
	{
		close(nFd);
		return DRV_ERR;
	}
	if(nIndex + nLen >= nReadSize)
	{
		close(nFd);
		return DRV_ERR;
	}
	nIndex += nLen;

	//
	nLen = P_SYSTEM_GetNumber64(&(acData[nIndex]), &(pstCpuStat->llIdle));
	if(nLen == 0)
	{
		close(nFd);
		return DRV_ERR;
	}
	if(nIndex + nLen >= nReadSize)
	{
		close(nFd);
		return DRV_ERR;
	}
	nIndex += nLen;

	//
	nLen = P_SYSTEM_GetNumber64(&(acData[nIndex]), &(pstCpuStat->llWait));
	if(nLen == 0)
	{
		close(nFd);
		return DRV_ERR;
	}
	if(nIndex + nLen >= nReadSize)
	{
		close(nFd);
		return DRV_ERR;
	}
	nIndex += nLen;

	//
	nLen = P_SYSTEM_GetNumber64(&(acData[nIndex]), &(pstCpuStat->llHi));
	if(nLen == 0)
	{
		close(nFd);
		return DRV_ERR;
	}
	if(nIndex + nLen >= nReadSize)
	{
		close(nFd);
		return DRV_ERR;
	}
	nIndex += nLen;

	//
	nLen = P_SYSTEM_GetNumber64(&(acData[nIndex]), &(pstCpuStat->llSi));
	if(nLen == 0)
	{
		close(nFd);
		return DRV_ERR;
	}
	if(nIndex + nLen >= nReadSize)
	{
		close(nFd);
		return DRV_ERR;
	}
	nIndex += nLen;

	//
	nLen = P_SYSTEM_GetNumber64(&(acData[nIndex]), &(pstCpuStat->llZero));
	if(nLen == 0)
	{
		close(nFd);
		return DRV_ERR;
	}
	if(nIndex + nLen >= nReadSize)
	{
		close(nFd);
		return DRV_ERR;
	}
	nIndex += nLen;

	close(nFd);

	return DRV_OK;
}

static DRV_Error P_SYSTEM_GetCPUStat(P_SYS_CPU_STAT_t* pstCpuStat)
{
	static P_SYS_CPU_STAT_t s_stPrevStat;
	P_SYS_CPU_STAT_t stCurrStat;
	P_SYS_CPU_STAT_t stDiffStat;
	static HBOOL s_bValid = FALSE;
	HINT32 nErr;

	if (pstCpuStat == NULL)
	{
		return DRV_ERR;
	}

	nErr = P_SYSTEM_ReadCPUStat(&stCurrStat);
	if (nErr != DRV_OK)
	{
		return nErr;
	}

	if(s_bValid==FALSE)
	{
		VK_MEM_Memcpy(&s_stPrevStat, &stCurrStat, sizeof(P_SYS_CPU_STAT_t));
		s_bValid = TRUE;
		VK_TASK_Sleep(100);	 /* 100ms delay 이후 다시 한번 읽는다 */

		nErr = P_SYSTEM_ReadCPUStat(&stCurrStat);
		if (nErr != DRV_OK)
		{
			return nErr;
		}
	}

	stDiffStat.llUser = stCurrStat.llUser - s_stPrevStat.llUser;
	stDiffStat.llNice = stCurrStat.llNice - s_stPrevStat.llNice;
	stDiffStat.llSystem = stCurrStat.llSystem - s_stPrevStat.llSystem;
	stDiffStat.llIdle = stCurrStat.llIdle - s_stPrevStat.llIdle;
	stDiffStat.llWait = stCurrStat.llWait - s_stPrevStat.llWait;
	stDiffStat.llHi = stCurrStat.llHi - s_stPrevStat.llHi;
	stDiffStat.llSi = stCurrStat.llSi - s_stPrevStat.llSi;
	stDiffStat.llZero = stCurrStat.llZero - s_stPrevStat.llZero;

	stDiffStat.llTotal = stDiffStat.llUser + stDiffStat.llNice + stDiffStat.llSystem + stDiffStat.llIdle + stDiffStat.llWait + stDiffStat.llHi + stDiffStat.llSi + stDiffStat.llZero;

	VK_MEM_Memcpy(&s_stPrevStat, &stCurrStat, sizeof(P_SYS_CPU_STAT_t));

	pstCpuStat->llUser = stDiffStat.llUser * 10000 / stDiffStat.llTotal;
	pstCpuStat->llNice = stDiffStat.llNice * 10000 / stDiffStat.llTotal;
	pstCpuStat->llSystem = stDiffStat.llSystem * 10000 / stDiffStat.llTotal;
	pstCpuStat->llIdle = stDiffStat.llIdle * 10000 / stDiffStat.llTotal;
	pstCpuStat->llWait = stDiffStat.llWait * 10000 / stDiffStat.llTotal;
	pstCpuStat->llHi = stDiffStat.llHi * 10000 / stDiffStat.llTotal;
	pstCpuStat->llSi = stDiffStat.llSi * 10000 / stDiffStat.llTotal;
	pstCpuStat->llZero = stDiffStat.llZero * 10000 / stDiffStat.llTotal;

	return DRV_OK;
}

DRV_Error DRV_SYSTEM_GetCPUUsage(HUINT32 *punCpuUsage)		 /* *punCpuUsage is percentage */
{
	P_SYS_CPU_STAT_t cpuStat;
	DRV_Error nErr=DRV_OK;

	if(punCpuUsage == NULL)
	{
		PrintError("[%s %d] punCpuUsage is Null\n",__func__,__LINE__);
		return DRV_ERR;
	}

	nErr = P_SYSTEM_GetCPUStat(&cpuStat);	 /* percentage*100 으로 올라옴 */
	if(nErr != DRV_OK)
	{
		PrintError("[%s %d] P_SYSTEM_GetCPUStat() Fail, nErr= 0x%x\n",__func__,__LINE__,nErr);
		return DRV_ERR;
	}

	*punCpuUsage = (10000 - cpuStat.llIdle)/100;

#if defined(CONFIG_DEBUG)
	//PrintError("cpuStat.llTotal = %u \n", cpuStat.llTotal);
	PrintDebug("cpuStat.llUser = %u \n", cpuStat.llUser);
	PrintDebug("cpuStat.llSystem = %u \n", cpuStat.llSystem);
	PrintDebug("cpuStat.llIdle = %u \n", cpuStat.llIdle);
	PrintDebug("cpuStat.llNice = %u \n", cpuStat.llNice);
	PrintDebug("cpuStat.llWait = %u \n", cpuStat.llWait);
	PrintDebug("cpuStat.llHi = %u \n", cpuStat.llHi);
	PrintDebug("cpuStat.llSi = %u \n", cpuStat.llSi);
	PrintDebug("cpuStat.llZero = %u \n", cpuStat.llZero);

	PrintDebug("punCpuUsage = %u \n", *punCpuUsage);
#endif

	return DRV_OK;
}

DI_ERR_CODE DI_SYSTEM_GetCPUUsage(HUINT32 *punCpuUsage)
{
	DRV_Error nErr=DRV_OK;

	if(punCpuUsage == NULL)
	{
		PrintError("[%s %d] punCpuUsage is null\n",__func__,__LINE__);
		return DI_ERR_ERROR;
	}

	nErr = DRV_SYSTEM_GetCPUUsage(punCpuUsage);
	if(nErr != DRV_OK)
	{
		PrintError("[%s %d] P_SYSTEM_GetCPUStat() Fail, nErr= 0x%x\n",__func__,__LINE__,nErr);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

#if !defined(CONFIG_OS_UCOS)
DRV_Error P_SYSTEM_GetProcessStatus(DI_SYS_PROCESS_STATUS_TABLE_t *pProcessStatusTable)
{
	HINT32 nErr;

	DIR *proc_dir;
	struct dirent *file_entry;
	struct stat file_stat;

	HINT32 nPid;


	if(pProcessStatusTable == NULL)
	{
		PrintError("[%s %d] pProcessStatusTable is Null\n",__func__,__LINE__);
		return DRV_ERR;
	}
	else
	{
		pProcessStatusTable->unNumOfEntries = 0;

	}

	proc_dir = opendir("/proc");
	if(proc_dir == NULL)
	{
		PrintError("[%s %d] opendir error\n",__func__,__LINE__);

		closedir(proc_dir);

		return DRV_ERR;
	}

	while((file_entry = readdir(proc_dir)) != NULL)		 /* On success, readdir() returns a pointer to a dirent structure. (This structure may be statically allocated; do not attempt to free(3) it.) If the end of the directory stream is reached, NULL is returned and errno is not changed. If an error occurs, NULL is returned and errno is set appropriately */
	{
		HUINT8 strPathProcProcess[MAX_LENGTH_OF_PROCFS_FULLPATH];
		HUINT8 strPathProcIdStat[MAX_LENGTH_OF_PROCFS_FULLPATH];
		FILE *fd_proc_id_stat;
		HUINT8 bufStat[MAX_BUF_SIZE_FOR_PROC_STAT];

		HINT8 *pscToken = NULL;
		const HINT8 *c_pscSeparator = " ";
		HUINT32 unCount =0;
		HINT32 snTime;
		HINT32 snPriority;
		HINT32 snSize;

		VK_memset(strPathProcProcess,0x00,MAX_LENGTH_OF_PROCFS_FULLPATH);

		VK_snprintf(strPathProcProcess, MAX_LENGTH_OF_PROCFS_FULLPATH, "/proc/%s",file_entry->d_name);
		nErr = lstat(strPathProcProcess, &file_stat);
		if(nErr < 0)
		{
			PrintDebug("[%s %d] lstat error, file_entry->d_name = %s\n",__func__,__LINE__,file_entry->d_name);
			continue;
		}
#ifdef DEBUG
		else
		{
			PrintDebug(":%s:\n",strPathProcProcess);
		}
#endif
		if(!S_ISDIR(file_stat.st_mode))
		{
			PrintDebug("[%s %d] %s is not a directory. file_stat.st_mode = 0x%x\n",__func__,__LINE__,strPathProcProcess,file_stat.st_mode);
			continue;	 /* dir 이 아니라면, process 정보가 아님. */
		}

		nPid = VK_atoi(file_entry->d_name);
		if(nPid <=0)
		{
			PrintDebug("[%s %d] %s is not a process.\n",__func__,__LINE__,file_entry->d_name);
			continue;	 /* 숫자가 아니라면, process 정보가 아님 */
		}
		else
		{
			/* process 임 */
			pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].unPid = (HUINT32)nPid;
			 /* 초기화 */
			pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].unSize =0;
			pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].unPriority=0;
			pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].unCPUTime=0;
			pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].eState=0;
			VK_memset(pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].strCommand, 0x00, MAX_PROCESS_STRCOMMAND_SIZE);

			 /* 1. stat 을 읽는다. */
			 /* 1-1. pid ...  */
			 /* 1-2. Priority 확인 후 저장 */
			 /* 1-3. CPUTIME 계산.( user mdoe jiffies + kernel mode jiffies) */
			 /* 1-4. status 저장 */
			 /* 1-5. command 저장 */
			 /* 2.statm 읽는다 */
			 /* 2-1. size 저장 */
			VK_memset(strPathProcIdStat,0x00,MAX_LENGTH_OF_PROCFS_FULLPATH);
			VK_memset(bufStat,0x00,MAX_BUF_SIZE_FOR_PROC_STAT);

			VK_snprintf(strPathProcIdStat, MAX_LENGTH_OF_PROCFS_FULLPATH, "/proc/%s/stat",file_entry->d_name);

			fd_proc_id_stat = fopen(strPathProcIdStat,"r");
			if(fd_proc_id_stat == NULL)
			{
				 /* stat 을 읽을수 없다면, not available. */
				 PrintError("[%s %d] %s open fail \n",__func__,__LINE__,fd_proc_id_stat);
			}
			else
			{
				fgets(bufStat, MAX_BUF_SIZE_FOR_PROC_STAT-1, fd_proc_id_stat);

				pscToken = strtok(bufStat, c_pscSeparator);
				if(pscToken == NULL)
				{
					PrintError("[%s %d] End of stat\n",__func__,__LINE__);
				}
				else
				{
					PrintDebug(" 0 : %s \n", pscToken);
					unCount++;
				}
				while((pscToken = strtok(NULL, c_pscSeparator)) != NULL)
				{
					PrintDebug(" %d: %s \n", unCount, pscToken);
					unCount++;

					switch(unCount)
					{
					case 2:	 /* The filename of the executable, in parentheses. This is visible whether or not the executable is swapped out. */
						VK_strncpy(pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].strCommand, pscToken, 256);

						break;
					case 3:	 /* State - R (running), S(sleeping interruptable), D(Sleeing), Z(Zombie), T(Stopped on sa signal), W(Paging) */
						if(*pscToken=='R')
						{
							pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].eState = PROCESS_STATE_RUNNING;
						}
						else if(*pscToken=='S')
						{
							pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].eState = PROCESS_STATE_SLEEPING_INTERRUPTABLE;
						}
						else if(*pscToken=='D')
						{
							pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].eState = PROCESS_STATE_SLEEPING;
						}
						else if(*pscToken=='Z')
						{
							pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].eState = PROCESS_STATE_ZOMBIE;
						}
						else if(*pscToken=='T')
						{
							pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].eState = PROCESS_STATE_STOPPED;
						}
						else if(*pscToken=='W')
						{
							pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].eState = PROCESS_STATE_PAGING;
						}
						else
						{
							PrintError("[%s %d] Invalide state\n",__func__,__LINE__);
						}
						break;
					case 14:	 /* utime - The number of jiffies (process time) that this process has been scheduled in user mode */
					case 15:	 /* stime - The number of jiffies (process time) that this process has been scheduled in kernel mode */
						snTime = VK_atoi(pscToken);
						if(snTime<0)
						{
							PrintDebug("[%s %d] %s is not a number.\n",__func__,__LINE__,pscToken);
						}
						else
						{
							pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].unCPUTime += (HUINT32)snTime;
							/* ToDo. 단위 재확인 필요함!!!! */
						}
						break;
					case 18:	 /* priority - For processes running a real-time scheduling policy. */
						snPriority = VK_atoi(pscToken);
						if(snPriority<0)
						{
							PrintDebug("[%s %d] %s is not a number.\n",__func__,__LINE__,pscToken);
						}
						else
						{
							pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].unPriority= snPriority;
						}
						break;
					default:
						 /*  No Operation */
						break;
					}
				}
				fclose(fd_proc_id_stat);
			}


			unCount =0;
			VK_memset(strPathProcIdStat,0x00,MAX_LENGTH_OF_PROCFS_FULLPATH);
			VK_memset(bufStat,0x00,MAX_BUF_SIZE_FOR_PROC_STAT);
			VK_snprintf(strPathProcIdStat, MAX_LENGTH_OF_PROCFS_FULLPATH, "/proc/%s/statm",file_entry->d_name);	 /* FullPath로.. */

			fd_proc_id_stat = fopen(strPathProcIdStat,"r");
			if(fd_proc_id_stat == NULL)
			{
				 /* stat 을 읽을수 없다면, not available. */
				 PrintError("[%s %d] %s open fail \n",__func__,__LINE__,fd_proc_id_stat);
			}
			else
			{
				fgets(bufStat, MAX_BUF_SIZE_FOR_PROC_STAT-1, fd_proc_id_stat);

				pscToken = strtok(bufStat, c_pscSeparator);
				if(pscToken == NULL)
				{
					PrintError("[%s %d] End of stat\n",__func__,__LINE__);
				}
				else
				{
					PrintDebug(" M 0 : %s \n", pscToken);
					unCount++;
				}

				snSize = VK_atoi(pscToken);
				if(snSize<0)
				{
					PrintDebug("[%s %d] %s is not a number.\n",__func__,__LINE__,pscToken);
				}
				else
				{
					pProcessStatusTable->ProcessStatus[pProcessStatusTable->unNumOfEntries].unSize= (HUINT32)(snSize/1024);	 /* Kbyte */
				}

#if defined(CONFIG_DEBUG)
				while((pscToken = strtok(NULL, c_pscSeparator)) != NULL)
				{
					PrintDebug(" M %d: %s \n", unCount, pscToken);
					unCount++;
				}
#endif
				fclose(fd_proc_id_stat);
			}
			pProcessStatusTable->unNumOfEntries++;
		}
	}

	closedir(proc_dir);

	return DRV_OK;
}

DRV_Error DRV_SYSTEM_GetCProcessStauts(DI_SYS_PROCESS_STATUS_TABLE_t *pProcessStatusTable)
{
	DRV_Error nErr=DRV_OK;

	if(pProcessStatusTable == NULL)
	{
		PrintError("[%s %d] pProcessStatusTable is Null\n",__func__,__LINE__);
		return DRV_ERR;
	}

	nErr = P_SYSTEM_GetProcessStatus(pProcessStatusTable);
	if(nErr != DRV_OK)
	{
		PrintError("[%s %d] P_SYSTEM_GetProcessStatus() Fail, nErr= 0x%x\n",__func__,__LINE__,nErr);
		return DRV_ERR;
	}

	return DRV_OK;
}

DI_ERR_CODE DI_SYSTEM_GetProcessStatus(DI_SYS_PROCESS_STATUS_TABLE_t *pProcessStatusTable)
{
	DRV_Error nErr=DRV_OK;

	if(pProcessStatusTable == NULL)
	{
		PrintError("[%s %d] pProcessStatusTable is null\n",__func__,__LINE__);
		return DI_ERR_ERROR;
	}

	nErr = DRV_SYSTEM_GetCProcessStauts(pProcessStatusTable);
	if(nErr != DRV_OK)
	{
		PrintError("[%s %d] DRV_SYSTEM_GetCProcessStauts() Fail, nErr= 0x%x\n",__func__,__LINE__,nErr);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DRV_Error DRV_SYSTEM_GetProcessorInfo(DI_SYS_PROCESSOR_ARCH_TABLE_t *pProcessorArchtecture)
{
	FILE *fd_proc_cpuinfo;
	HINT8 bufCpuinfo[MAX_BUF_SIZE_FOR_PROC_CPUINFO];

	if(pProcessorArchtecture == NULL)
	{
		PrintError("[%s %d] strArchtecture is Null\n",__func__,__LINE__);
		return DRV_ERR;
	}
	VK_memset(pProcessorArchtecture, 0x00, sizeof(DI_SYS_PROCESSOR_ARCH_TABLE_t));

	fd_proc_cpuinfo = fopen("/proc/cpuinfo","r");
	if(fd_proc_cpuinfo == NULL)
	{
		 /* cpuinfo 을 읽을수 없다면, not available. */
		 PrintError("[%s %d] %s open fail \n",__func__,__LINE__,fd_proc_cpuinfo);
		 return DRV_ERR;
	}
	VK_memset(bufCpuinfo, 0x00, MAX_BUF_SIZE_FOR_PROC_CPUINFO);

	/* line by line 으로 분석, used fgets() */
	while(!(feof(fd_proc_cpuinfo)))
	{
		if(fgets(bufCpuinfo, MAX_BUF_SIZE_FOR_PROC_CPUINFO, fd_proc_cpuinfo) == NULL)
		{
			if(!(feof(fd_proc_cpuinfo)))
			{
				PrintError("[%s %d] fgets Error \n", __func__,__LINE__);
				fclose(fd_proc_cpuinfo);
				return DRV_ERR;
			}
			else
			{
				PrintDebug("[%s %d] EOF\n", __func__,__LINE__);
				fclose(fd_proc_cpuinfo);
				return DRV_OK;
			}
		}
		if (VK_strncmp(bufCpuinfo, "cpu model", 9) != 0)
		{
			continue;
		}
		else
		{
			HINT8 *pStrCpuModel =NULL;

			pStrCpuModel = VK_strchr(bufCpuinfo,':');
			if(pStrCpuModel == NULL)
			{
				PrintError("[%s %d] invalid string \n",__func__,__LINE__);
			}
			else
			{
				VK_strncpy(pProcessorArchtecture->ProcessorArch[pProcessorArchtecture->unNumOfEntries].strArch, pStrCpuModel+2, MAX_PROCESSOR_ARCH_SIZE-1);
				pProcessorArchtecture->unNumOfEntries++;
			}
		}
	}

	fclose(fd_proc_cpuinfo);

	return DRV_OK;
}

DI_ERR_CODE DI_SYSTEM_GetProcessorInfo(DI_SYS_PROCESSOR_ARCH_TABLE_t *pProcessorArchtecture)
{
	if(pProcessorArchtecture == NULL)
	{
		PrintError("[%s %d] pProcessorArchtecture is Null\n",__func__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(DRV_SYSTEM_GetProcessorInfo(pProcessorArchtecture) != DRV_OK)
	{
		PrintError("[%s %d] DRV_SYSTEM_GetProcessorInfo Fail\n",__func__,__LINE__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif

#if defined(CONFIG_OS_UCOS)
DI_ERR_CODE DI_SYSTEM_CacheFlush(void)
{
	PrintError("[%s %d] Need to be implemented if we need\n",__func__,__LINE__);
	return DI_ERR_OK;
}
#endif

