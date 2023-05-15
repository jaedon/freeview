/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
        
/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#if !defined(CONFIG_OS_RTOS)
#include <dirent.h>
#include <sys/mount.h>
#endif

/* humax header files */
#include "htype.h"
#include "di_err.h"
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_testparams.h"
#include "fai_app.h"
#include "fai_window.h"
#include "fai_msgbox.h"
#include "fai_util.h"
#include "fai_tmd.h"
#include "fai_system.h"
#include "fdi_flash.h"
#include "fdi_eeprom.h"
#include "fdi_ethernet.h"
#include "fdi_system.h"
#include "fdi_panel.h"
#include "fdi_key.h"
#include "fdi_wifi.h"
#include "fai_wifi.h"


#include "di_nvram.h"
/* model specific configuration header */
#include "factorysw_config.h"
#if !defined(CONFIG_PLATFORM_ALI)
#include "flash_map.h"
#endif

#if defined(FACTORY_USE_CAS_IR_UTIL)
#include "fdi_cas_ir_util.h"
#endif

#include "di_system.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
/* 이것들은 어떻게 처리할까.. */
#define USB_FILE_SYSTEM "vfat"
#define USB_FILE_PATH "/media/"

typedef void (*FDI_SYSTEM_ProgressFunc_t)(unsigned char ucProgressRate);

#define FONT_MIN_VALUE 0x20
#define FONT_MAX_VALUE 0x7A

#define FAI_DEF_SET_TMD_ERR_MES_DOWNLOAD		0x2001
#define FAI_DEF_SET_TMD_ERR_EEPROM_WRITE		0x2002
#define FAI_DEF_SET_TMD_ERR_CAM_PAIR_DATA		0x2003
#define FAI_DEF_SET_TMD_ERR_BSL_WRITE			0x2004
#define FAI_DEF_SET_TMD_ERR_SECURE_BOOT			0x2005
#define FAI_DEF_SET_TMD_ERR_JTAG				0x2006
#define FAI_DEF_SET_TMD_ERR_FLASH_OTP			0x2007
#define FAI_DEF_SET_TMD_ERR_FAC_SW_ERASE		0x2008
#define FAI_DEF_SET_TMD_ERR_CW_MODE				0x2009
#define FAI_DEF_SET_TMD_ERR_DEFAULT_DATA_WRITE	0x200A
#define FAI_DEF_SET_TMD_ERR_FAC_LDR_ERASE		0x200B
#define FAI_DEF_SET_TMD_ERR_CHIP_OTP			0x200C
#define FAI_DEF_SET_TMD_ERR_NO_DEFAULT_FILE		0x200D
#define FAI_DEF_SET_TMD_ERR_SYS_ID				0x200E
#define FAI_DEF_SET_TMD_ERR_CCB_ENCRYPT			0x200F
#define FAI_DEF_SET_TMD_ERR_NAND_CLEAR			0x2010
#define FAI_DEF_SET_TMD_ERR_NAND_COPY			0x2011
#define FAI_DEF_SET_TMD_ERR_MARKET_ID			0x2012
#define FAI_DEF_SET_TMD_ERR_REMUX_FUSING		0x2013
#define FAI_DEF_SET_TMD_ERR_SECTION_DATA		0x2014

const HUINT32		s_betaTable[] =
{
	DI_NVRAM_FIELD_SYSTEM_ID,		// 0
	DI_NVRAM_FIELD_SFLASH,				// 1
	DI_NVRAM_FIELD_SFLASH,				// 1
	DI_NVRAM_FIELD_SFLASH,				// 1
	DI_NVRAM_FIELD_SFLASH,				// 1
	DI_NVRAM_FIELD_SERIAL_NO,			// 5
	DI_NVRAM_FIELD_MAC_ADDR,			// 6
	DI_NVRAM_FIELD_HDCP_KEY,			// 7
	DI_NVRAM_FIELD_DTCP_KEY,			// 8
	DI_NVRAM_FIELD_CI_PLUS_KEY,			// 9
	DI_NVRAM_FIELD_PARING_DATA,			// A
	DI_NVRAM_FIELD_PRIVATE,				// B
	DI_NVRAM_FIELD_CSC_KEY,				// C
	DI_NVRAM_FIELD_CM_SERIAL_NO,		// D
	DI_NVRAM_FIELD_CM_MAC_ADDR,			// E
	DI_NVRAM_FIELD_CM_MAC_ADDR_ANOTHER,	// F
	DI_NVRAM_FIELD_IRDETO_BBCB,			// 10
	DI_NVRAM_FIELD_IRDETO_UC_SECURE_ID,		// 11
	DI_NVRAM_FIELD_IRDETO_UC_PRIVATE_DATA,	// 12
	DI_NVRAM_FIELD_APP_VERSION,					// 13
	DI_NVRAM_FIELD_IRDETO_CA_PARTITION,			// 14
	DI_NVRAM_FIELD_IRDETO_LOADER_PARTITION,		// 15
	DI_NVRAM_FIELD_IRDETO_IR_PARTITION,			// 16
	DI_NVRAM_FIELD_IRDETO_IR_BACKUP_PARTITION,	// 17
	DI_NVRAM_FIELD_IRDETO_CPCB,					// 18
	DI_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED, 		// 19
	DI_NVRAM_FIELD_DISPLAY_FACTORY_INFO,			// 1A
	DI_NVRAM_FIELD_GANG_CRC						// 1B
};

const HUINT32 s_sizeBetaTable = (sizeof(s_betaTable)/sizeof(s_betaTable[0]));

#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_SECTION_ENABLE)
extern unsigned char dataSectionData[DI_SYS_OTP_DATA_SECTION_LENGTH];
#endif
/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
static FAI_ERR_CODE INTRN_FAI_SYSTEM_ChangeSheet1(void);
static FAI_ERR_CODE INTRN_FAI_SYSTEM_ChangeSheet2(void);
static FAI_ERR_CODE INTRN_FAI_SYSTEM_ChangeSheet3(void);
static FAI_ERR_CODE INTRN_FAI_SYSTEM_CheckDefaultFile(void);
static FAI_ERR_CODE INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(unsigned char *pucFusingFlag, BOOL bIDCheckOnly);
static FAI_ERR_CODE INTRN_FAI_SYSTEM_GetDTCP_Validity(void);
static FAI_ERR_CODE INTRN_FAI_SYSTEM_GetCiPlus_Validity(void);
static FAI_ERR_CODE INTRN_FAI_SYSTEM_EncryptCCB(BOOL* bSuccess);
static FAI_ERR_CODE INTRN_FAI_SYSTEM_DeleteDBFiles(void);
static FAI_ERR_CODE INTRN_FAI_SYSTEM_CopyingDBFiles(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_Init(void)
{
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

#if defined(CONFIG_DI10)
FAI_ERR_CODE FAI_SYSTEM_WIFI_KO_Module_Load(void)
{
	FAI_PRINT(FAI_PRT_ERROR, ("\n\nFAI_WIFI_INIT\n\n"));

	VK_SYSTEM_Command("cp -rf /bcm_wifi_test /tmp");	
	VK_SYSTEM_Command("ln -s /tmp/bcm_wifi_test/bin/wl /usr/bin/wl");
	VK_SYSTEM_Command("chmod -R 777 /tmp/bcm_wifi_test");
//	VK_SYSTEM_Command("/tmp/bcm_wifi_test/env.sh");
	VK_SYSTEM_Command("/tmp/bcm_wifi_test/wifi_hmx.sh init mfg &");
//	VK_SYSTEM_Command("wl ver");	
	
	FAI_PRINT(FAI_PRT_ERROR, ("\n\n"));
	
	return FAI_NO_ERR;
}
#endif


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FAI_ERR_CODE FAI_SYSTEM_SetWIFIMacAddr(unsigned char *pucMacAddr)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;

	eFdiErr = FDI_SYSTEM_SetWifiMacAddress(pucMacAddr);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_SetMacAddr] FDI_SYSTEM_SetWifiMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_MAC_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_SetMacAddr] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetWifiMacAddress(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_SetMacAddr] FDI_SYSTEM_GetWifiMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucMacAddr, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_WIFI_SetMacAddr] MAC verification fail.\n"));
		return FAI_ERR;
	}
	
	FVK_MEM_Free(pucVerifyBuf);
	
	return FAI_NO_ERR;
}

static FAI_ERR_CODE INTRN_FAI_SYSTEM_GetFactoryVersion(unsigned char *pucVersion)
{
	sprintf(pucVersion, "%s %s.%s", MODEL_STRING, FACTORYSW_APP_VERSION, RELEASE_VERSION);
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SYSINFO_GET_SW_VERSION_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_GetSWVersion(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned long ulSystemId;
	unsigned char aucResult[20];
	unsigned char ucVersionType;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	/* get params */
	FAI_TMD_GetParam(&ucVersionType, pucArgList, 0);

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_SYSTEM_GetSWVersion] : type 0x%x\n", ucVersionType));

	memset(aucResult, 0, sizeof(aucResult));
	
	switch( ucVersionType )
	{
		case FAI_SYSTEM_SYSTEM_ID :
			FDI_SYSTEM_GetSystemId(&ulSystemId);
			sprintf(&aucResult[1], "%08X", ulSystemId);
			aucResult[0] = strlen(&aucResult[1]);					/* length of param */						
			break;
			
		case FAI_SYSTEM_LOADER :
			FDI_SYSTEM_GetLoaderVersion(&aucResult[1]);
			aucResult[0] = strlen(&aucResult[1]);					/* length of param */
			break;

		case FAI_SYSTEM_MICOM :
			FDI_SYSTEM_GetMicomVersion(&aucResult[1]);
			aucResult[0] = strlen(&aucResult[1]);					/* length of param */
			break;

		case FAI_SYSTEM_APPLICATION:
			FDI_SYSTEM_GetAppVersion(&aucResult[1]);
			aucResult[0] = strlen(&aucResult[1]);					/* length of param */
			break;

		case FAI_SYSTEM_ATV_APP:								/* sgkim4 : factory SW version */
			INTRN_FAI_SYSTEM_GetFactoryVersion(&aucResult[1]);
			aucResult[0] = strlen(&aucResult[1]);					/* length of param */
			break;	

		case FAI_SYSTEM_FACTORY_LOADER:
		case FAI_SYSTEM_MICOM_MODE:
		case FAI_SYSTEM_MODEL_NAME:
		case FAI_SYSTEM_ATV_LOADER:
		case FAI_SYSTEM_OTA:
		case FAI_SYSTEM_CHIP_INFO:
		case FAI_SYSTEM_SYSTEM:
		case FAI_SYSTEM_KERNEL:
		case FAI_SYSTEM_BOOTLOADER:
		case FAI_SYSTEM_UPDATE_DATE:
		case FAI_SYSTEM_MOCA_FW:
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_GetSWVersion] : Not Support feature 0x%x\n", ucVersionType));
			
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_NOT_SUPPORTED_FEATURE;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			FAI_TMD_SetResult(&tTMDResult);

			return FAI_FEATURE_NOT_SUPPORTED;

		default : 
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_GetSWVersion] : Not Support type 0x%x\n", ucVersionType));

			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			FAI_TMD_SetResult(&tTMDResult);

			return FAI_ERR_INVALID_PARAMETER;
	}

	//FAI_TMD_SetCurrentResult(ucVersionType, FAI_TMD_TEST_NO_ERROR, 1, aucResult, aucResult[0]+1);
	
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_AddResultParam(&tTMDResult, &aucResult[1], aucResult[0]);
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

#if (PRODUCT_YSR2000==YES)
#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)
FAI_ERR_CODE INTRN_FAI_SYSTEM_Get_QT_WIFI_State(int *uiState)
{
	FAI_ERR_CODE eFaiErr = FAI_ERR;
	unsigned char szSystemCommand[100];
	FILE *fp;
	unsigned char input[256];
	char	*pLine;
	int i=0;
	int nAccess;
	
	while(1)
	{
		access("/tmp/res",0);
	 	if(nAccess == 0)
	 	{
	 		VK_SYSTEM_Command("rm -rf /tmp/res");
	 	}
		FVK_MEM_Set(szSystemCommand, 0x00, sizeof(szSystemCommand));
		FVK_MEM_Set(input, 0x00, sizeof(input));
		sprintf(szSystemCommand, "call_qcsapi_sockrpc --host 169.254.119.1 get_bootcfg_param calstate > /tmp/res");
		VK_SYSTEM_Command(szSystemCommand);

		*uiState=0;

		//state or wifi connect가 가능한지 확인하기 위한 코드
		fp = fopen("/tmp/res","r");
		if(fp != NULL)
		{
			pLine = fgets(input, 255, fp);
			if(pLine != NULL)
			{			
				pLine = NULL;
				if((pLine = strstr(input,"RPC:")) != NULL)
				{
					eFaiErr = FAI_ERR;
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] wait for wifi connecting...[%d]\n",i);
				}
				else if(!strncmp(input,"0",1))
				{
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] invalid wifi calstate[0]...\n");
					*uiState=0;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}			
				else if(!strncmp(input,"1",1))
				{
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] invalid wifi calstate[1]...\n");
					*uiState=1;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}
				else if(!strncmp(input,"2",1))
				{
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] invalid wifi calstate[2]...\n");
					*uiState=2;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}
				else if(!strncmp(input,"3",1))
				{
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] valid wifi calstate...[3]\n");
					*uiState=3;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}						
				else
				{
					eFaiErr = FAI_NO_ERR;
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] invalid wifi calstate\n");
				}
			}
			fclose(fp);
		}

		if((eFaiErr==FAI_NO_ERR)||(*uiState>2))
		{
			break;
		}
		else
		{
			FVK_TASK_Sleep(500);
			if(i++>60)
			{
				/* max wait 25 seconds for wifi ic booting */
				break;
			}
			else
			{
				continue;
			}
		}		
	}
	return eFaiErr;
}
#endif
#endif

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SYSINFO_GET_UPLOAD_INFO_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_GetUploadInfo(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char aucResult[30];
	unsigned char ucInfoType;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FAI_ERR_CODE eFaiErr;
	unsigned char aucBuf[20];
	int i;
	int *ulState;			

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	/* get params */
	FAI_TMD_GetParam(&ucInfoType, pucArgList, 0);

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_SYSTEM_GetDownloadInfo] : type 0x%x\n", ucInfoType));

	memset(aucResult, 0, sizeof(aucResult));

	switch( ucInfoType )
	{
		case FAI_SYSTEM_CHIP_ID :
		aucResult[0] = 1;	/* length of param1 */
		aucResult[1] = 0;	/* 0:integer, 1:string */
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_NAGRA_CHIP_ID)
			aucResult[2] = FDI_SYSTEM_DATA_CHIPID_SIZE*2;				/* length of param */
			FDI_SYSTEM_GetChipId(&aucResult[3]);
			FDI_SYSTEM_GetChipIdCheckNumber(&aucResult[3+FDI_SYSTEM_DATA_CHIPID_SIZE]);
#else
			aucResult[2] = FDI_SYSTEM_DATA_CHIPID_SIZE; 			/* length of param */
			FDI_SYSTEM_GetChipId(&aucResult[3]);
#endif
			break;
			
		case FAI_SYSTEM_SERIAL_NO :
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 1;	/* 0:integer, 1:string */
			aucResult[2] = FDI_SYSTEM_DATA_SN_SIZE;	/* length of param */

			FDI_SYSTEM_GetSerialNumber(&aucResult[3]);
			break;

		case FAI_SYSTEM_MAC :   /* 1st Ethernet MAC */
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = FDI_SYSTEM_DATA_MAC_SIZE;			/* length of param2 */

			FDI_SYSTEM_GetMacAddress(&aucResult[3]);
			break;
#if defined(FACTORY_USE_BT)
		case FAI_SYSTEM_MAC2 : /* 2nd Ethernet MAC*/
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = FDI_SYSTEM_DATA_MAC_SIZE;			/* length of param2 */
			FDI_SYSTEM_GetBTMacAddress(&aucResult[3]);
			break;
#else
		case FAI_SYSTEM_MAC2 : /* 2nd Ethernet MAC*/
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = FDI_SYSTEM_DATA_MAC_SIZE;			/* length of param2 */
#if defined(CONFIG_DI10) /* only use KHD-1000T */
			FDI_SYSTEM_GetWifiMacAddress(&aucResult[3]);
#else
#if (PRODUCT_YSR2000==YES) 
			eFaiErr = INTRN_FAI_SYSTEM_Get_QT_WIFI_State(&ulState);
			if((eFaiErr!=FAI_NO_ERR)||(ulState<1))
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_Get_QT_WIFI_State] WIFI Calstate Invalid state!!(%d)\n",ulState));
			}
			FAI_WIFI_QT3940_GetMacaddr(&aucResult[3]);
#else
			FDI_SYSTEM_GetSecondMacAddress(&aucResult[3]);
#endif
#endif
			break;
#endif
#if defined(CONFIG_DI20)
		case FAI_SYSTEM_MAC3 :  /* 3rd Ethernet MAC*/
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = FDI_SYSTEM_DATA_MAC_SIZE;			/* length of param2 */	
			FDI_SYSTEM_GetThirdMacAddress(&aucResult[3]);
			break;

#endif
#if defined(FACTORY_USE_CABLEMODEM)			
		case FAI_SYSTEM_MAC4 : /* CABLE MODEM MAC*/
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = FDI_SYSTEM_DATA_MAC_SIZE;			/* length of param2 */		
			FDI_SYSTEM_GetCmMacAddress(&aucResult[3]);
			break;
#endif			
#if defined(FACTORY_USE_WIFI)
		case FAI_SYSTEM_EXT_MAC1 : /* Wi-Fi MAC */
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = FDI_SYSTEM_DATA_MAC_SIZE;			/* length of param2 */		
			FDI_WIFI_GetMacAddr(0,&aucResult[3]);
			break;

#if (PRODUCT_YSR2000==YES)
		case FAI_SYSTEM_WIFIMAC1 :
			eFaiErr = INTRN_FAI_SYSTEM_Get_QT_WIFI_State(&ulState);
			if((eFaiErr!=FAI_NO_ERR)||(ulState<1))
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_Get_QT_WIFI_State] WIFI Calstate Invalid state!!(%d)\n",ulState));
			}			
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = 6;			/* length of param2 */		
			FAI_WIFI_QT3940_GetMacaddr(&aucResult[3]);
			break;
#else
		case FAI_SYSTEM_WIFIMAC1 :
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = 6;			/* length of param2 */		
			FDI_SYSTEM_GetWifiMacAddress(&aucResult[3]);
			break;
#endif	
#endif
		case FAI_SYSTEM_RF4CE_MAC:	/* RF4CE MAC */
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = 8;	/* length of param2 */		
			FDI_SYSTEM_GetIEEEAddr(&aucResult[3]);
			break;

		case FAI_STYTEM_IRDETO_PRIVATEDATA : /* IRDETO PRIVATE DATA */
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 0;	/* 0:integer, 1:string */			
			aucResult[2] = FDI_SYSTEM_DATA_IRDETO_PRIVATEDATA_SIZE;			/* length of param2 */		
			FDI_SYSTEM_GetPrivateData(&aucResult[3]);
			break;
		case FAI_SYSTEM_RECEIVER_NO:
		case FAI_SYSTEM_HDCP:
		case FAI_SYSTEM_DTCP:
			eFaiErr = INTRN_FAI_SYSTEM_GetDTCP_Validity();
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : DTCP Test Result =%d\n",  __func__, eFaiErr));
			if( eFaiErr == FAI_NO_ERR )
			{
				tTMDResult.eResult = FAI_TMD_ERR_OK;
			} 
			else
			{
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			}
			FAI_TMD_SetResult(&tTMDResult);
			return FAI_NO_ERR;
		case FAI_SYSTEM_NAGRA_CAID:
			aucResult[0] = 1;	/* length of param1 */
			aucResult[1] = 1;	/* 0:integer, 1:string */
			aucResult[2] = 0x0A;	/* length of param */
			eFaiErr = FDI_SYSTEM_GetCASerialNumber(&aucResult[3]);

			for(i=3; i<13; i++)
			{
				if(aucResult[i] == 0x20)
					aucResult[i] = 0x30;
			}	
			if( eFaiErr == FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetCASerialNumber Error\n"));
			}

			break;
		case FAI_SYSTEM_PARING_DATA:
		case FAI_SYSTEM_NDS_SVP:
		case FAI_SYSTEM_CERTIFICATION:
//		case FAI_SYSTEM_MAC2:
		case FAI_SYSTEM_DIRECTV_SN:
		case FAI_SYSTEM_MANUFACTURE_DATE:
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_GetDownloadInfo] : Not Support feature 0x%x\n", ucInfoType));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_NOT_SUPPORTED_FEATURE;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			FAI_TMD_SetResult(&tTMDResult);

			return FAI_FEATURE_NOT_SUPPORTED;
		default : 
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_GetDownloadInfo] : Not Support type 0x%x\n", ucInfoType));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			FAI_TMD_SetResult(&tTMDResult);

			return FAI_ERR_INVALID_PARAMETER;
	}

	//FAI_TMD_SetCurrentResult(ucInfoType, FAI_TMD_TEST_NO_ERROR, 2, aucResult, aucResult[0]+aucResult[2]+2);
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	//FAI_TMD_AddResultParam(&tTMDResult, aucResult, aucResult[0]+aucResult[2]+2);
	memcpy(&tTMDResult.aucParams[0], aucResult, aucResult[0]+aucResult[2]+2);
	tTMDResult.ucNumOfParam=2;
	FAI_TMD_SetResult(&tTMDResult);

	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SYSINFO_GET_SECURITY_INFO_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_GetSecurityInfo(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	BOOL bStatus=FALSE;


	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	if ((FDI_SYSTEM_GetSCSStatus(&bStatus)==FDI_NO_ERR && bStatus==TRUE)
		||
		(FDI_SYSTEM_GetJTAGStatus(&bStatus)==FDI_NO_ERR && bStatus==TRUE)
		||
		(FDI_SYSTEM_GetOTPStatus(&bStatus)==FDI_NO_ERR && bStatus==TRUE))
	{
		tTMDResult.eResult = FAI_TMD_ERR_OK;
		FAI_TMD_SetResult(&tTMDResult);
	}
	else
	{
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		FAI_TMD_SetResult(&tTMDResult);
	}

	return FAI_NO_ERR;
}
#endif



/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SYSINFO_OFF==YES)
FAI_ERR_CODE FAI_SYSTEM_InformationOff(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FAI_MSGBOX_H hCurMsgBox;
	unsigned char ucOnOff;
	static unsigned char sucStatus = 0 ;
	
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n", __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));		
		goto TEST_FAIL;
	}
	FAI_TMD_GetParam(&ucOnOff, pucArgList, 0);

	if(sucStatus != ucOnOff)
	{
		if(ucOnOff == 1)
		{
			FAI_SYSTEM_DisplayInfoMessageBox();
			sucStatus = 1;
		}
		else if(ucOnOff == 0)
		{		
			FAI_ERR_CHECK(FAI_MSGBOX_GetCurrentMsgBox(&hCurMsgBox));
			FAI_ERR_CHECK(FAI_MSGBOX_Destoy(hCurMsgBox));
			sucStatus = 0;
		}
		else
		{
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));		
			goto TEST_FAIL;

		}
	}
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);

	return FAI_NO_ERR;

TEST_FAIL:

	FAI_TMD_SetResult(&tTMDResult);
	
	return FAI_NO_ERR;
	
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (POWER_SW_RESET_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_SWReset(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];


	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	//----------------------------------
	//Send Result Success before reset.
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);

	FVK_TASK_Sleep(1000);
	//----------------------------------
	
	eFdiErr = FDI_SYSTEM_SetSwReset();
	
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_SWReset_UI] : SW reset error\n"));

		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (POWER_CONTROL_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_PowerCtrl(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];


	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetStandby();
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_SWReset_UI] : SW reset error\n"));

		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);

	return FAI_NO_ERR;
}
#endif

#if (CHANGE_SHEET_ENABLE==YES)
/*******************************************************************************
* function :
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_ChangeSheet(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucSheetNum;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Play] : Argument Count Error=%d\n", ucArgc));
 
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		FAI_TMD_SetResult(&tTMDResult);
		
		return FAI_ERR;
	}

	/* get params */
	FAI_TMD_GetParam(&ucSheetNum, pucArgList, 0);

	switch(ucSheetNum)
	{
		case 0:
			INTRN_FAI_SYSTEM_ChangeSheet1();
			break;
		case 1:
			INTRN_FAI_SYSTEM_ChangeSheet2();
			break;
		case 2:
			INTRN_FAI_SYSTEM_ChangeSheet3();
			break;
		default:
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_ETC;
			ucTmdErrCode[1] = FAI_ETC_TMD_ERR_INVALID_SHEET;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			FAI_TMD_SetResult(&tTMDResult);
			return FAI_ERR;
	}
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SYSINFO_DTCP_TEST_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_DTCPCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char aucResult[5];
	FAI_ERR_CODE eFaiErr;
	unsigned int ulNoParms, ulParmLength;
	
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	eFaiErr = INTRN_FAI_SYSTEM_GetDTCP_Validity();

	FAI_PRINT(FAI_PRT_ERROR, ("[%s] : DTCP Test Result =%d\n",  __func__, eFaiErr));
	
	if( eFaiErr == FAI_NO_ERR )
	{
		tTMDResult.eResult = FAI_TMD_ERR_OK;
	} 
	else
	{
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
	}
	
	

	TEST_FAIL:

	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SYSINFO_CHECK_CIPLUS_KEY_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_CiPlusCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char aucResult[5];
	FAI_ERR_CODE eFaiErr;
	unsigned int ulNoParms, ulParmLength;

	eFaiErr = INTRN_FAI_SYSTEM_GetCiPlus_Validity();
	if( eFaiErr == FAI_NO_ERR )
	{
		aucResult[0] = 1;		/* length of param */
		aucResult[1] = TRUE;	/* result */
		ulNoParms = 1;
		ulParmLength = aucResult[0]+1;
	}
	else
	{
		aucResult[0] = 1;		/* length of param */
		aucResult[1] = FALSE;	/* result */

		aucResult[2] = 2;	/* length of param */
		aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	/* result - error code high */
		aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF;		/* result - error code low */

		ulNoParms = 1+1;
		ulParmLength = aucResult[0]+1+FAI_TMD_ERROR_PARM_LENGTH;
	}

	FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_NO_ERROR, ulNoParms, aucResult, ulParmLength);
	return FAI_NO_ERR;
}
#endif

#if (DEFAULT_SETTING_ENABLE==YES)
/*******************************************************************************
* function : FAI_SYSTEM_DefaultSetting
* description : In old TMD task architecture, this function called in 
*               FAI_TMD_CommandParserTask directly.
* 				In the middle of this function, there is no response(no ack) from 
* 				FAI_TMD_CommandParserTask(STB). So it is need to delay enough time in TMD script.
*               Below is pseudo TMD script.
*               1) default setting start command
*				2) delay xxx time(about default setting time)
*				3) default setting get result command
*               
*               In new TMD task architecture(higher version 1.0), this function(all tmd test fuctions)
*               execute in FAI_TMD_CommandProcessorTask separately.
*               So it is NOT need to delay in TMD script.
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if(DEFAULT_SETTING_USB_LOG_SAVE_ENABLE==YES)

FAI_ERR_CODE FAI_SYSTEM_DefaultSetting(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_ERR_CODE eFaiErr;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucFusingFlag;
	int i;
	BOOL bMesDone, bSystemIDCheckOnly, bSuccess;
	unsigned char ucParamID, ucNumberOfParam, ucLength, aucResult[5];
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	
	int j=0, nLine=0;
	BOOL bValidSn=TRUE;
	unsigned char aucSerialNum[20];
	char cmdBuf[128];

#if (DEFAULT_SETTING_LIST&DEFAULT_STB_OWNER_ID)
	unsigned short usSTBOwnerId;
#endif	
#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_SECTION_ENABLE)
	BOOL bStatus = 0;
#endif

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		goto TEST_FAIL;
	}

	ucParamID = 0;

	VK_MEM_Memset(aucSerialNum,0x0, sizeof(aucSerialNum));
	aucSerialNum[FDI_SYSTEM_DATA_SN_SIZE-1]='\0';

	eFdiErr = FDI_SYSTEM_GetSerialNumber(aucSerialNum);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetSerialNumber Error\n"));
		goto TEST_FAIL;
	}
	for( j = 0; j < FDI_SYSTEM_DATA_SN_SIZE; j++ )
	{
		if( aucSerialNum[j] < FONT_MIN_VALUE || aucSerialNum[j] > FONT_MAX_VALUE )
		{
			bValidSn = FALSE;
		}
	}
	if( bValidSn == TRUE )
	{
		if((FDI_SYSTEM_DATA_SN_SIZE == 0x0C)||(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)||(FDI_SYSTEM_DATA_SN_SIZE == 0x10))
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : [%s] valid serial\n",__func__,aucSerialNum));
			VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: start default-setting... > /media/%s.log", nLine++,aucSerialNum);
			VK_SYSTEM_Command(cmdBuf);
		}
		else
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Invalid size serial\n",__func__));
			VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: start default-setting... > /media/no_serial.log", nLine++);
			VK_SYSTEM_Command(cmdBuf);
		}
	}
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : No serial\n",__func__));
		VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: start default-setting... > /media/no_serial.log", nLine++);
		VK_SYSTEM_Command(cmdBuf);
	}


#if (DEFAULT_SETTING_LIST&DEFAULT_CHECK_MES_DOWNLOAD)
	FDI_SYSTEM_CheckMesCompleted(&bMesDone);
	if( bMesDone == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DefaultSetting] : FDI_SYSTEM_CheckMesCompleted Error\n"));
		if( bValidSn == TRUE )
		{
			if((FDI_SYSTEM_DATA_SN_SIZE == 0x0C)||(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)||(FDI_SYSTEM_DATA_SN_SIZE == 0x10))
			{
				VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
				VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_CheckMesCompleted error... >> /media/%s.log", nLine++,aucSerialNum);
				VK_SYSTEM_Command(cmdBuf);
			}
			else
			{
				VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
				VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_CheckMesCompleted error... >> /media/no_serial.log", nLine++);
				VK_SYSTEM_Command(cmdBuf);
			}
		}
		else
		{
			VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_CheckMesCompleted error... >> /media/no_serial.log", nLine++);
			VK_SYSTEM_Command(cmdBuf);
		}

		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_MES_DOWNLOAD;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	

		goto TEST_FAIL;
	}
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_STB_OWNER_ID)
	/* check STB owner ID */
	FDI_SYSTEM_GetSTBOwnerId(&usSTBOwnerId);
	if(usSTBOwnerId != 0x2b)
	{
		eFdiErr = FDI_SYSTEM_SetSTBOwnerId(0x2b);
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DefaultSetting] : FDI_SYSTEM_SetSTBOwnerId Error\n"));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_SECURE_BOOT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
	}
#endif
#if 0///*removed from 40n*/ (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_USING_HDF)

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_SYSTEM_DefaultSetting] : Checking default file\n"));

	eFaiErr = INTRN_FAI_SYSTEM_CheckDefaultFile();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_CheckDefaultFile Error\n"));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_NO_DEFAULT_FILE;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL; 		
	}

	bSystemIDCheckOnly = TRUE;
	eFaiErr = INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(NULL, bSystemIDCheckOnly);		
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(System ID Error)\n"));

		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_SYS_ID;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_CCB_ENCRYPT)
	eFdiErr = FDI_SYSTEM_EncryptCCB(&bSuccess); 
	if(eFdiErr != FDI_NO_ERR || bSuccess != TRUE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EncryptCCB Error\n"));
		aucResult[2] = 0x02;		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_CCB_ENCRYPT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL; 		
	}

#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_NORMAL)
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_FACTORY_LOADER_ERASE)
		eFdiErr = FDI_SYSTEM_EraseFactoryLoader();
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EraseFactoryLoader\n"));

			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_FAC_LDR_ERASE;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_MAKE_SPCB)
#if defined(FACTORY_USE_CAS_IR_UTIL)
	eFdiErr = FDI_CAS_IR_MakeSPCB();
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DefaultSetting] : FDI_CAS_MakeSPCB Error\n"));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_MAKE_SPCB;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}
	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_SYSTEM_DefaultSetting] : Make SPCB completed.\n"));
#endif		
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_USING_HDF)
	bSystemIDCheckOnly = FALSE;
	eFaiErr = INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(&ucFusingFlag, bSystemIDCheckOnly);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_DefaultSetting\n"));
		if( bValidSn == TRUE )
		{
			if((FDI_SYSTEM_DATA_SN_SIZE == 0x0C)||(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)||(FDI_SYSTEM_DATA_SN_SIZE == 0x10))
			{
				VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
				VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: INTRN_FAI_SYSTEM_DefaultSettingUsingHDF error... >> /media/%s.log", nLine++,aucSerialNum);
				VK_SYSTEM_Command(cmdBuf);
			}
			else
			{
				VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
				VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: INTRN_FAI_SYSTEM_DefaultSettingUsingHDF error... >> /media/no_serial.log", nLine++);
				VK_SYSTEM_Command(cmdBuf);
			}
		}
		else
		{
			VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: INTRN_FAI_SYSTEM_DefaultSettingUsingHDF error... >> /media/no_serial.log", nLine++);
			VK_SYSTEM_Command(cmdBuf);
		}

		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_DEFAULT_DATA_WRITE;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}
	else
	{
		FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_SYSTEM_DefaultSetting]:default setting using HDF finished\n"));
	}
#else
	ucFusingFlag = TRUE;
#endif

	if( ucFusingFlag == TRUE )
	{
#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_SECTION_ENABLE)
		bStatus = 0;

		eFdiErr = FDI_SYSTEM_GetDataSectionStatus(dataSectionData,&bStatus);
		if(bStatus != 0)
		{
			eFdiErr = FDI_SYSTEM_EnableOTPDataSection(dataSectionData);
			if( eFdiErr != FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableOTPSectionData\n"));
				aucResult[2] = 0x02;
				aucResult[3] = (FAI_DEF_SET_TMD_ERR_SECTION_DATA>>8) & 0xFF;
				aucResult[4] = (FAI_DEF_SET_TMD_ERR_SECTION_DATA) & 0xFF;
				goto TEST_FAIL;
			}
		}
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_MARKET_ID)
		eFdiErr = FDI_SYSTEM_SetMarketID(DEFAULT_SETTING_MARKET_ID);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_SetMarketID\n"));
			if( bValidSn == TRUE )
			{
				if((FDI_SYSTEM_DATA_SN_SIZE == 0x0C)||(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)||(FDI_SYSTEM_DATA_SN_SIZE == 0x10))
				{
					VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
					VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_SetMarketID error... >> /media/%s.log", nLine++,aucSerialNum);
					VK_SYSTEM_Command(cmdBuf);
				}
				else
				{
					VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
					VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_SetMarketID error... >> /media/no_serial.log", nLine++);
					VK_SYSTEM_Command(cmdBuf);
				}
			}
			else
			{
				VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
				VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_SetMarketID error... >> /media/no_serial.log", nLine++);
				VK_SYSTEM_Command(cmdBuf);
			}
		
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_MARKET_ID;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_CW_MODE_ENABLE)
		eFdiErr = FDI_SYSTEM_EnableCwMode();
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableCwMode\n"));

			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_CW_MODE;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}

#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_OTP_LOCK_ENABLE)
		eFdiErr = FDI_SYSTEM_EnableOTPLock();
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableOTPLock\n"));
			if( bValidSn == TRUE )
			{
				if((FDI_SYSTEM_DATA_SN_SIZE == 0x0C)||(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)||(FDI_SYSTEM_DATA_SN_SIZE == 0x10))
				{
					VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
					VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_EnableOTPLock error... >> /media/%s.log", nLine++,aucSerialNum);
					VK_SYSTEM_Command(cmdBuf);
				}
				else
				{
					VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
					VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_EnableOTPLock error... >> /media/no_serial.log", nLine++);
					VK_SYSTEM_Command(cmdBuf);
				}
			}
			else
			{
				VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
				VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_EnableOTPLock error... >> /media/no_serial.log", nLine++);
				VK_SYSTEM_Command(cmdBuf);
			}

			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_FLASH_OTP;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_SECURE_BOOT_ENABLE)
		eFdiErr = FDI_SYSTEM_EnableSecureBoot();
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableSecureBoot\n"));
			if( bValidSn == TRUE )
			{
				if((FDI_SYSTEM_DATA_SN_SIZE == 0x0C)||(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)||(FDI_SYSTEM_DATA_SN_SIZE == 0x10))
				{
					VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
					VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_EnableSecureBoot error... >> /media/%s.log", nLine++,aucSerialNum);
					VK_SYSTEM_Command(cmdBuf);
				}
				else
				{
					VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
					VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_EnableSecureBoot error... >> /media/no_serial.log", nLine++);
					VK_SYSTEM_Command(cmdBuf);
				}
			}
			else
			{
				VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
				VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_EnableSecureBoot error... >> /media/no_serial.log", nLine++);
				VK_SYSTEM_Command(cmdBuf);
			}

			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_SECURE_BOOT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_JTAG_ENABLE)
		eFdiErr = FDI_SYSTEM_EnableJtag();			
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableJtag\n"));
			if( bValidSn == TRUE )
			{
				if((FDI_SYSTEM_DATA_SN_SIZE == 0x0C)||(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)||(FDI_SYSTEM_DATA_SN_SIZE == 0x10))
				{
					VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
					VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_EnableJtag error... >> /media/%s.log", nLine++,aucSerialNum);
					VK_SYSTEM_Command(cmdBuf);
				}
				else
				{
					VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
					VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_EnableJtag error... >> /media/no_serial.log", nLine++);
					VK_SYSTEM_Command(cmdBuf);
				}
			}
			else
			{
				VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
				VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: FDI_SYSTEM_EnableJtag error... >> /media/no_serial.log", nLine++);
				VK_SYSTEM_Command(cmdBuf);
			}

			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_JTAG;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif
	}


	ucNumberOfParam = 1;
	ucLength = 2;
	
	aucResult[0] = 0x01;	/* length of param 1 */
	aucResult[1] = 0x01;	/* result ok */
	//FAI_TMD_SetCurrentResult(ucParamID, FAI_TMD_TEST_NO_ERROR, ucNumberOfParam, aucResult, ucLength);

	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);

	if( bValidSn == TRUE )
	{
		if((FDI_SYSTEM_DATA_SN_SIZE == 0x0C)||(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)||(FDI_SYSTEM_DATA_SN_SIZE == 0x10))
		{
			VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: End Default-setting... >> /media/%s.log", nLine++,aucSerialNum);
			VK_SYSTEM_Command(cmdBuf);
			sync();
		}
		else
		{
			VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: End Default-setting... >> /media/no_serial.log", nLine++);
			VK_SYSTEM_Command(cmdBuf);
		}
	}
	else
	{
		VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: End Default-setting... >> /media/no_serial.log", nLine++);
		VK_SYSTEM_Command(cmdBuf);
	}
	sync();
	VK_SYSTEM_Command("umount /media");

	return FAI_NO_ERR;

TEST_FAIL:

	FAI_TMD_SetResult(&tTMDResult);
	
	if( bValidSn == TRUE )
	{
		if((FDI_SYSTEM_DATA_SN_SIZE == 0x0C)||(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)||(FDI_SYSTEM_DATA_SN_SIZE == 0x10))
		{
			VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: Fail Default-setting... >> /media/%s.log", nLine++,aucSerialNum);
			VK_SYSTEM_Command(cmdBuf);
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "mv /media/%s.log /media/%s_ng.log",aucSerialNum,aucSerialNum);
			VK_SYSTEM_Command(cmdBuf);
		}
		else
		{
			VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
			VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: Fail Default-setting... >> /media/no_serial.log", nLine++);
			VK_SYSTEM_Command(cmdBuf);
			VK_SYSTEM_Command("mv /media/no_serial.log /media/no_serial_ng.log");
		}
	}
	else
	{
		VK_MEM_Memset(cmdBuf,0x0, sizeof(cmdBuf));
		VK_snprintf(cmdBuf, sizeof(cmdBuf), "echo %d: Fail Default-setting... >> /media/no_serial.log", nLine++);
		VK_SYSTEM_Command(cmdBuf);
		VK_SYSTEM_Command("mv /media/no_serial.log /media/no_serial_ng.log");
	}
	sync();
	VK_SYSTEM_Command("umount /media");

	return FAI_ERR;
}
#else
FAI_ERR_CODE FAI_SYSTEM_DefaultSetting(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_ERR_CODE eFaiErr;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucFusingFlag;
	int i;
	BOOL bMesDone, bSystemIDCheckOnly, bSuccess;
	unsigned char ucParamID, ucNumberOfParam, ucLength, aucResult[5];
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	
#if (DEFAULT_SETTING_LIST&DEFAULT_STB_OWNER_ID)
	unsigned short usSTBOwnerId;
#endif	
#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_SECTION_ENABLE)
	BOOL bStatus = 0;
#endif

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		goto TEST_FAIL;
	}

	ucParamID = 0;
	

#if (DEFAULT_SETTING_LIST&DEFAULT_CHECK_MES_DOWNLOAD)
	FDI_SYSTEM_CheckMesCompleted(&bMesDone);
	if( bMesDone == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DefaultSetting] : FDI_SYSTEM_CheckMesCompleted Error\n"));

		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_MES_DOWNLOAD;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	

		goto TEST_FAIL;
	}
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_STB_OWNER_ID)
	/* check STB owner ID */
	FDI_SYSTEM_GetSTBOwnerId(&usSTBOwnerId);
	if(usSTBOwnerId != 0x2b)
	{
		eFdiErr = FDI_SYSTEM_SetSTBOwnerId(0x2b);
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DefaultSetting] : FDI_SYSTEM_SetSTBOwnerId Error\n"));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_SECURE_BOOT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
	}
#endif
#if 0///*removed from 40n*/ (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_USING_HDF)

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_SYSTEM_DefaultSetting] : Checking default file\n"));

	eFaiErr = INTRN_FAI_SYSTEM_CheckDefaultFile();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_CheckDefaultFile Error\n"));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_NO_DEFAULT_FILE;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL; 		
	}

	bSystemIDCheckOnly = TRUE;
	eFaiErr = INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(NULL, bSystemIDCheckOnly);		
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(System ID Error)\n"));

		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_SYS_ID;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_CCB_ENCRYPT)
	eFdiErr = FDI_SYSTEM_EncryptCCB(&bSuccess); 
	if(eFdiErr != FDI_NO_ERR || bSuccess != TRUE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EncryptCCB Error\n"));
		aucResult[2] = 0x02;		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_CCB_ENCRYPT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL; 		
	}

#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_NORMAL)
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_FACTORY_LOADER_ERASE)
		eFdiErr = FDI_SYSTEM_EraseFactoryLoader();
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EraseFactoryLoader\n"));

			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_FAC_LDR_ERASE;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_MAKE_SPCB)
#if defined(FACTORY_USE_CAS_IR_UTIL)
	eFdiErr = FDI_CAS_IR_MakeSPCB();
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DefaultSetting] : FDI_CAS_MakeSPCB Error\n"));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_MAKE_SPCB;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}
	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_SYSTEM_DefaultSetting] : Make SPCB completed.\n"));
#endif		
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_USING_HDF)
	bSystemIDCheckOnly = FALSE;
	eFaiErr = INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(&ucFusingFlag, bSystemIDCheckOnly);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_DefaultSetting\n"));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
		ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_DEFAULT_DATA_WRITE;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}
	else
	{
		FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_SYSTEM_DefaultSetting]:default setting using HDF finished\n"));
	}
#else
	ucFusingFlag = TRUE;
#endif

	if( ucFusingFlag == TRUE )
	{
#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_SECTION_ENABLE)
		bStatus = 0;

		eFdiErr = FDI_SYSTEM_GetDataSectionStatus(dataSectionData,&bStatus);
		if(bStatus != 0)
		{
			eFdiErr = FDI_SYSTEM_EnableOTPDataSection(dataSectionData);
			if( eFdiErr != FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableOTPSectionData\n"));
				aucResult[2] = 0x02;
				aucResult[3] = (FAI_DEF_SET_TMD_ERR_SECTION_DATA>>8) & 0xFF;
				aucResult[4] = (FAI_DEF_SET_TMD_ERR_SECTION_DATA) & 0xFF;
				goto TEST_FAIL;
			}
		}
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_MARKET_ID)
		eFdiErr = FDI_SYSTEM_SetMarketID(DEFAULT_SETTING_MARKET_ID);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_SetMarketID\n"));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_MARKET_ID;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_CW_MODE_ENABLE)
		eFdiErr = FDI_SYSTEM_EnableCwMode();
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableCwMode\n"));

			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_CW_MODE;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}

#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_OTP_LOCK_ENABLE)
		eFdiErr = FDI_SYSTEM_EnableOTPLock();
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableOTPLock\n"));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_FLASH_OTP;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_SECURE_BOOT_ENABLE)
		eFdiErr = FDI_SYSTEM_EnableSecureBoot();
		if( eFdiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableSecureBoot\n"));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_SECURE_BOOT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_JTAG_ENABLE)
		eFdiErr = FDI_SYSTEM_EnableJtag();			
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableJtag\n"));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_CATEGORY_SYSTEM;
			ucTmdErrCode[1] = FAI_FACTORY_DEFAULT_TMD_ERR_JTAG;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto TEST_FAIL;
		}
#endif
	}

	ucNumberOfParam = 1;
	ucLength = 2;
	
	aucResult[0] = 0x01;	/* length of param 1 */
	aucResult[1] = 0x01;	/* result ok */
	//FAI_TMD_SetCurrentResult(ucParamID, FAI_TMD_TEST_NO_ERROR, ucNumberOfParam, aucResult, ucLength);

	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);

	return FAI_NO_ERR;

TEST_FAIL:

	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}
#endif
#endif
/*******************************************************************************
* function : FAI_SYSTEM_GetTemperature
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (TEMPERATURE_SENSOR_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_GetTemperature(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	unsigned char ucDeviceType,ucDeviceId,ucTime;
	unsigned char aucResult;
	unsigned long ulResultValue = 0;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 3 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	FAI_TMD_GetParam(&ucDeviceType, pucArgList, 0);
	FAI_TMD_GetParam(&ucDeviceId, pucArgList, 1);
	FAI_TMD_GetParam(&ucTime, pucArgList, 2);	

	/************** ucDeviceType 0x00, ucDeviceId 0x01에 대해서만 구현.  ************/
	/************** ucDeviceType 0x00, ucDeviceId 0x00에 대해서도 구현.  ************/
	if(ucDeviceType == 0x00)	/* CPU */
	{
		if((ucDeviceId == 0x00) || (ucDeviceId == 0x01)) 	/* CPU 1 or 2 */
		{
			FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
			eFdiErr = FDI_SYSTEM_GetCpuTemperature(ucDeviceId,&ulResultValue);
			if(eFdiErr == FDI_NO_ERR )
			{
				aucResult = TRUE;						
				tTMDResult.eResult = FAI_TMD_ERR_OK;
				FAI_TMD_AddResultParam(&tTMDResult, &ulResultValue, 1);
				FAI_TMD_SetResult(&tTMDResult);	
				return FAI_NO_ERR;
			}
		}
		else
		{
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			FAI_TMD_SetResult(&tTMDResult);
			return FAI_ERR;
		}
	}
	else if(ucDeviceType == 0x01) /* HDD */
	{
		
	}
	else if(ucDeviceType == 0x02) /* Smart Card */
	{
	
	}
	
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
	ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_NOT_SUPPORTED_FEATURE;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;

}

#endif

#if (RF4CE_RSSI_CHECK_ENABLE==YES)
#define FAI_SYSTEM_CATEGORY_PERIPHERAL 	0x10
#define FAI_SYSTEM_MODULE_RF4CE 	0x10
#define FAI_SYSTEM_ACTION_CHECK_RSSI	0x06

#define FAI_SYSTEM_RF4CE_CHECK_TIME	500
typedef struct 
{
	unsigned long ulRssiSampleCnt;
	unsigned long ulCurSampleCnt;
	unsigned long ulSumTxRssi;
	unsigned long ulSumRxRssi;
	unsigned long ulMaxTxRssi;
	unsigned long ulMaxRxRssi;
	unsigned long ulMinTxRssi;
	unsigned long ulMinRxRssi;

	unsigned long ulRssiTimerId;
} FAI_SYSTEM_RF4CE_RSSI_Params_t;
FAI_SYSTEM_RF4CE_RSSI_Params_t g_Rf4ceRssi;

void INTRN_FAI_SYSTEM_RF4CE_RSSI_Notify(unsigned long ulTxRSSI, unsigned long ulRxRSSI)
{
	if(g_Rf4ceRssi.ulRssiSampleCnt <= g_Rf4ceRssi.ulCurSampleCnt)
	{
		return;
	}
	if(g_Rf4ceRssi.ulCurSampleCnt == 0)
	{
		g_Rf4ceRssi.ulMinTxRssi = ulTxRSSI;
		g_Rf4ceRssi.ulMaxTxRssi = ulTxRSSI;
		g_Rf4ceRssi.ulMinRxRssi = ulRxRSSI;
		g_Rf4ceRssi.ulMaxRxRssi = ulRxRSSI;
	}

	if(ulTxRSSI < g_Rf4ceRssi.ulMinTxRssi)
	{
		g_Rf4ceRssi.ulMinTxRssi = ulTxRSSI;
	}
	if(ulRxRSSI < g_Rf4ceRssi.ulMinRxRssi)
	{
		g_Rf4ceRssi.ulMinRxRssi = ulRxRSSI;
	}
	if(ulTxRSSI > g_Rf4ceRssi.ulMaxTxRssi)
	{
		g_Rf4ceRssi.ulMaxTxRssi = ulTxRSSI;
	}
	if(ulRxRSSI > g_Rf4ceRssi.ulMaxRxRssi)
	{
		g_Rf4ceRssi.ulMaxRxRssi = ulRxRSSI;
	}
	g_Rf4ceRssi.ulSumTxRssi += ulTxRSSI;
	g_Rf4ceRssi.ulSumRxRssi += ulRxRSSI;

	g_Rf4ceRssi.ulCurSampleCnt++;

	if(g_Rf4ceRssi.ulRssiSampleCnt <= g_Rf4ceRssi.ulCurSampleCnt)
	{
		FAI_TMD_RESULT_T tTMDResult;
		unsigned char ucAveTxRssi, ucAveRxRssi;

		FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

		tTMDResult.ucCategory = FAI_SYSTEM_CATEGORY_PERIPHERAL;
		tTMDResult.ucModule = FAI_SYSTEM_MODULE_RF4CE;
		tTMDResult.ucAction = FAI_SYSTEM_ACTION_CHECK_RSSI;
		tTMDResult.eResult = FAI_TMD_ERR_OK;
		ucAveTxRssi = (g_Rf4ceRssi.ulSumTxRssi-g_Rf4ceRssi.ulMinTxRssi-g_Rf4ceRssi.ulMaxTxRssi)/(g_Rf4ceRssi.ulRssiSampleCnt-2);
		ucAveRxRssi = (g_Rf4ceRssi.ulSumRxRssi-g_Rf4ceRssi.ulMinRxRssi-g_Rf4ceRssi.ulMaxRxRssi)/(g_Rf4ceRssi.ulRssiSampleCnt-2);
		FAI_TMD_AddResultParam(&tTMDResult, &ucAveTxRssi, sizeof(ucAveTxRssi));
		FAI_TMD_AddResultParam(&tTMDResult, &ucAveRxRssi, sizeof(ucAveRxRssi));
		FAI_TMD_SetResult(&tTMDResult);
		
		FVK_TIMER_Destory(g_Rf4ceRssi.ulRssiTimerId);
	}
}

/*******************************************************************************
* function : INTRN_FAI_HDMI_HDCPTimerCallback
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
FVK_ERR_CODE INTRN_FAI_SYSTEM_Rf4ceRssiTimerCallback(unsigned long ulTimerId, void *pvParams)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	tTMDResult.ucCategory = FAI_SYSTEM_CATEGORY_PERIPHERAL;
	tTMDResult.ucModule = FAI_SYSTEM_MODULE_RF4CE;
	tTMDResult.ucAction = FAI_SYSTEM_ACTION_CHECK_RSSI;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;

	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
	ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	

	FAI_TMD_SetResult(&tTMDResult);
}

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_CheckRf4ceRssi(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	if( ucArgc != 1 )
	{
		FAI_TMD_RESULT_T tTMDResult;
		unsigned char ucTmdErrCode[2];
		
		FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	FVK_MEM_Set(&g_Rf4ceRssi, 0x00, sizeof(g_Rf4ceRssi));

	FAI_TMD_GetParam(&g_Rf4ceRssi.ulRssiSampleCnt, pucArgList, 0);

	FDI_SYSTEM_RegisterRssiCallback(INTRN_FAI_SYSTEM_RF4CE_RSSI_Notify);

	FDI_SYSTEM_SetRf4ceFactoryMode();

	FVK_TIMER_CreateStart(g_Rf4ceRssi.ulRssiSampleCnt*FAI_SYSTEM_RF4CE_CHECK_TIME, 
				INTRN_FAI_SYSTEM_Rf4ceRssiTimerCallback, 
				NULL, 0, FVK_TIMER_ONCE, &g_Rf4ceRssi.ulRssiTimerId);

	return FAI_NO_ERR;
}
#endif

#if (INFORMATION_OSD_UI_ENABLE==YES)
/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_SYSTEM_DisplayInfoHandler(unsigned int unDeviceType, unsigned int unKeyCode)
{
	FAI_MSGBOX_H hCurMsgBox;

	if( unDeviceType == TEST_INPUT_TYPE_RCU)
	{
		FAI_ERR_CHECK(FAI_MSGBOX_GetCurrentMsgBox(&hCurMsgBox));
		FAI_ERR_CHECK(FAI_MSGBOX_Destoy(hCurMsgBox));
	}
	
	return FAI_NO_ERR;
}


/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_DisplayInfoMessageBox(void)
{
	FDI_ERR_CODE eFdiErr;
	FAI_ERR_CODE eFaiErr;
	FAI_MSGBOX_H hCurMsgBox;
	unsigned char aucBuf[20];
	unsigned int unmarket_id;
	unsigned long ulChipId;
	char acSystemInfoStr[16][FAI_MSG_STR_MAX];
	int i = 0, j;
	int		numOfInfoItem;
	BOOL bFusingStatus, bValidSn = TRUE;
	int *ulState;
	unsigned char aucSDCardSN[4];
	unsigned int ulSDCardSN;

#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_SDCARD_SN)
	FVK_MEM_Set(aucSDCardSN,0x00,sizeof(aucSDCardSN));
	FDI_MMC_Mount(1, "/mnt/sd0");
	FVK_TASK_Sleep(2000);
	eFdiErr = FDI_MMC_GetSerialNumber(aucSDCardSN);
	if( eFdiErr != FDI_NO_ERR )
	{
		sprintf(acSystemInfoStr[i++], "SD Card SN : %s","Not Detected");
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_MMC_GetSerialNumber Error\n"));
	}
	else
	{
		ulSDCardSN = (aucSDCardSN[0] << 24) | (aucSDCardSN[1] << 16)| (aucSDCardSN[2] << 8) | (aucSDCardSN[3]);
		sprintf(acSystemInfoStr[i++], "SD Card SN : 0x%08x",ulSDCardSN);
	}
#endif		
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_MAC_ADDRESS)
		eFdiErr = FDI_SYSTEM_GetMacAddress(aucBuf);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetMacAddress Error\n"));
			goto TEST_FAIL;
		}
		sprintf(acSystemInfoStr[i++], "WAN MAC Address : %02X.%02X.%02X.%02X.%02X.%02X",
									aucBuf[0], aucBuf[1], aucBuf[2], aucBuf[3], aucBuf[4], aucBuf[5]);
#endif
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_WIFI_MAC_ADDRESS)
#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)
#if (PRODUCT_YSR2000==YES)
		eFaiErr = INTRN_FAI_SYSTEM_Get_QT_WIFI_State(&ulState);
		if((eFaiErr!=FAI_NO_ERR)||(ulState<1))
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_Get_QT_WIFI_State] WIFI Calstate Invalid state!!(%d)\n",ulState));
			goto TEST_FAIL;
		}
		eFdiErr = FAI_WIFI_QT3940_GetMacaddr(aucBuf);
#endif		
#else
		eFdiErr = FDI_SYSTEM_GetWifiMacAddress(aucBuf);
#endif
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetWifiMacAddress Error\n"));
			goto TEST_FAIL;
		}
		sprintf(acSystemInfoStr[i++], "WIFI MAC Address : %02X.%02X.%02X.%02X.%02X.%02X",
									aucBuf[0], aucBuf[1], aucBuf[2], aucBuf[3], aucBuf[4], aucBuf[5]);
#endif	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_SERIAL_NUMBER)
		eFdiErr = FDI_SYSTEM_GetSerialNumber(aucBuf);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetMacAddress Error\n"));
			goto TEST_FAIL;
		}
		for( j = 0; j < FDI_SYSTEM_DATA_SN_SIZE; j++ )
		{
			if( aucBuf[j] < FONT_MIN_VALUE || aucBuf[j] > FONT_MAX_VALUE )
			{
				bValidSn = FALSE;
			}
		}
	
		if( bValidSn == TRUE )
		{
			if(FDI_SYSTEM_DATA_SN_SIZE == 0x0C)
			{
				sprintf(acSystemInfoStr[i++], "STB SN : %c%c%c%c%c%c%c%c%c%c%c%c",
										aucBuf[0], aucBuf[1], aucBuf[2], aucBuf[3], aucBuf[4], aucBuf[5],
										aucBuf[6], aucBuf[7], aucBuf[8], aucBuf[9], aucBuf[10], aucBuf[11]);
			}
			else if(FDI_SYSTEM_DATA_SN_SIZE == 0x0E)
			{
				sprintf(acSystemInfoStr[i++], "STB SN : %c%c%c%c%c%c%c%c%c%c%c%c%c%c",
										aucBuf[0], aucBuf[1], aucBuf[2], aucBuf[3], aucBuf[4], aucBuf[5],
										aucBuf[6], aucBuf[7], aucBuf[8], aucBuf[9], aucBuf[10], aucBuf[11],
										aucBuf[12], aucBuf[13]);
			}
			else if(FDI_SYSTEM_DATA_SN_SIZE == 0x10)
			{
				sprintf(acSystemInfoStr[i++], "STB SN : %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
										aucBuf[0], aucBuf[1], aucBuf[2], aucBuf[3], aucBuf[4], aucBuf[5],
										aucBuf[6], aucBuf[7], aucBuf[8], aucBuf[9], aucBuf[10], aucBuf[11],
										aucBuf[12], aucBuf[13], aucBuf[14], aucBuf[15]);
			}
			else
			{
				sprintf(acSystemInfoStr[i++], "STB SN : Invalid Serial Number Length. (should be 12 or 14)");
			}
		}
		else
		{
			sprintf(acSystemInfoStr[i++], "STB SN : Invalid Serial Number");
		}
#endif
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_CHIP_ID)
		eFdiErr = FDI_SYSTEM_GetChipId(aucBuf);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetChipId Error\n"));
			goto TEST_FAIL;
		}
#if defined(CONFIG_SUPPORT_CHIPID_64X)
		ulChipId = ((aucBuf[0]<<24)|(aucBuf[1]<<16)|(aucBuf[2]<<8)|(aucBuf[3]));
		sprintf(acSystemInfoStr[i++], "Chip ID : %u & 0x%02X%02X%02X%02X%02X%02X%02X%02X", ulChipId, aucBuf[0], aucBuf[1], aucBuf[2], aucBuf[3],aucBuf[4], aucBuf[5], aucBuf[6], aucBuf[7]);
#else
		ulChipId = ((aucBuf[0]<<24)|(aucBuf[1]<<16)|(aucBuf[2]<<8)|(aucBuf[3]));
		sprintf(acSystemInfoStr[i++], "Chip ID : %u & %02X%02X%02X%02X", ulChipId, aucBuf[0], aucBuf[1], aucBuf[2], aucBuf[3]);
#endif		
#endif
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_CHIP_ID_CHECKSUM)
		eFdiErr = FDI_SYSTEM_GetChipIdCheckNumber(aucBuf);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetChipIdCheckNumber Error\n"));
			goto TEST_FAIL;
		}
		sprintf(acSystemInfoStr[i++], "Chip ID Checksum : %02X%02X%02X%02X", aucBuf[0], aucBuf[1], aucBuf[2], aucBuf[3]);
#endif
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_MARKET_ID)
		eFdiErr = FDI_SYSTEM_GetMarketID(&unmarket_id);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetMarketID Error\n"));
			goto TEST_FAIL;
		}
		sprintf(acSystemInfoStr[i++], "Market ID : %08X", unmarket_id);
#endif
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_DSTB_ID)
		eFdiErr = FDI_SYSTEM_GetDstbId(aucBuf, FDI_SYSTEM_DATA_DSTBID_SIZE);

		FDI_SYSTEM_GetDstbId(aucBuf, FDI_SYSTEM_DATA_DSTBID_SIZE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetDstbId Error\n"));
			goto TEST_FAIL;
		}
		sprintf(acSystemInfoStr[i++], "DSTB ID : %02X%02X%02X%02X%02X%02X",
									aucBuf[0], aucBuf[1], aucBuf[2], aucBuf[3], aucBuf[4], aucBuf[5]);
#endif
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_NAGRA_PAIRING_DATA)
		eFdiErr = FDI_SYSTEM_GetCASerialNumber(aucBuf);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetCASerialNumber Error\n"));
			goto TEST_FAIL;
		}
		sprintf(acSystemInfoStr[i++], "NAGRA Paring data : %s", aucBuf);
#endif
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_NAGRA_CSC_VALIDITY)
		/* FDI_SYSTEM_GetCSCValidity need 11 bytes:
		   aucBuf[0] = validity : 0x00 - Fail 0x77 - Valide
		   aucBuf[1] - aucBuf[4] : nuid in CSC
		   aucBuf[5] - aucBuf[6] : CRC in CSC
		   aucBuf[7] - aucBuf[10] : CheckNum in CSC
		   aucBuf[11]- aucBuf[12]: Provider ID*/
		eFdiErr = FDI_SYSTEM_GetCSCValidity(aucBuf);
		if ( aucBuf[0] == 0x01 )
		{
			sprintf(acSystemInfoStr[i++], "NAGRA CSC   : Validation PASS");
		}
		else if (  ( eFdiErr != FDI_NO_ERR) || (aucBuf[0] != 0x01 ))
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetCSCValidity Error\n"));
			sprintf(acSystemInfoStr[i++], "NAGRA CSC   : Validation FAIL");
		}
#endif
	
		sprintf(acSystemInfoStr[i++], " ");
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_SCS)
		eFdiErr = FDI_SYSTEM_GetSCSStatus(&bFusingStatus);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetSCSStatus Error\n"));
			goto TEST_FAIL;
		}
		if( bFusingStatus )
		{
			sprintf(acSystemInfoStr[i++], "Secure Boot : Enable");
		}
		else
		{
			sprintf(acSystemInfoStr[i++], "Secure Boot : Disable");
		}
#endif
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_JTAG)
		eFdiErr = FDI_SYSTEM_GetJTAGStatus(&bFusingStatus);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetJTAGStatus Error\n"));
			goto TEST_FAIL;
		}
		if( bFusingStatus )
		{
			sprintf(acSystemInfoStr[i++], "JTAG Password Lock : Enable");
		}
		else
		{
			sprintf(acSystemInfoStr[i++], "JTAG Password Lock : Disable");
		}
#endif
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_OTP_LOCK)
		eFdiErr = FDI_SYSTEM_GetOTPStatus(&bFusingStatus);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetOTPStatus Error\n"));
			goto TEST_FAIL;
		}
		if( bFusingStatus )
		{
			sprintf(acSystemInfoStr[i++], "Flash OTP Lock : Enable");
		}
		else
		{
			sprintf(acSystemInfoStr[i++], "Flash OTP Lock : Disable");
		}
#endif
	
#if(INFORMATION_OSD_UI_DATA&INFORMATION_OSD_CW_MODE)
		eFdiErr = FDI_SYSTEM_GetCwMode(&bFusingStatus);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_DisplayInfoMessageBox] : FDI_SYSTEM_GetCwMode Error\n"));
			goto TEST_FAIL;
		}
		if( bFusingStatus )
		{
			sprintf(acSystemInfoStr[i++], "Secure CW : Enable");
		}
		else
		{
			sprintf(acSystemInfoStr[i++], "Secure CW : Disable");
		}
#endif


	numOfInfoItem = i;
	FAI_ERR_CHECK(FAI_MSGBOX_Create(&hCurMsgBox, 
									"System Info", 
									acSystemInfoStr,
									numOfInfoItem, 
									INTRN_FAI_SYSTEM_DisplayInfoHandler));
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Infomation", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "fail", RGB_COLOR_RED);

	return FAI_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_DisplayInfo_UI(void)
{
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Information", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_SYSTEM_INFORMATION_STR);
	
	FAI_SYSTEM_DisplayInfoMessageBox();
	
	return FAI_NO_ERR;
}
#endif

#if (DEFAULT_SETTING_UI_ENABLE==YES)
BOOL g_IsPassDefaultSet;
/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_SYSTEM_DefaultSetHandler(unsigned int unDeviceType, unsigned int unKeyCode)
{
	FAI_MSGBOX_H hCurMsgBox;

	if( unDeviceType == TEST_INPUT_TYPE_RCU)
	{
		FAI_ERR_CHECK(FAI_MSGBOX_GetCurrentMsgBox(&hCurMsgBox));
		FAI_ERR_CHECK(FAI_MSGBOX_Destoy(hCurMsgBox));
		
		if( g_IsPassDefaultSet==TRUE )
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Default", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Setting", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Pass", RGB_COLOR_BLACK);			
		}
		else
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Default", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Setting", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Fail", RGB_COLOR_RED);
		}
		FAI_SYSTEM_DisplayInfoMessageBox();
	}
	
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_SYSTEM_DefaultSetPrepare(unsigned int unDeviceType, unsigned int unKeyCode)
{
	FAI_ERR_CODE eFaiErr;
	FDI_ERR_CODE eFdiErr;
	FAI_MSGBOX_H hCurMsgBox;
	unsigned char ucFusingFlag;
	unsigned long ulNumOfDefault = 0;
	int i;
	BOOL bMesDone, bSystemIDCheckOnly, bSuccess;
#if (DEFAULT_SETTING_LIST&DEFAULT_STB_OWNER_ID)
	unsigned short usSTBOwnerId;
#endif	
#if defined(FACTORY_USE_USB2ETHERNET)
	while(!FDI_ETHERNET_Check_USBtoEthernet_PowerCntl())
	{
		FVK_TASK_Sleep(1000);
	}
#endif

	if( unDeviceType == TEST_INPUT_TYPE_FKEY && unKeyCode == FRONT_KEY_POWER )
	{
		FAI_ERR_CHECK(FAI_MSGBOX_GetCurrentMsgBox(&hCurMsgBox));
		FAI_ERR_CHECK(FAI_MSGBOX_Destoy(hCurMsgBox));

		FDI_PANEL_Display(FAI_SYSTEM_DEFUALT_SETTING_START_STR);
		
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Default", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Setting", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

#if (DEFAULT_SETTING_LIST&DEFAULT_CHECK_MES_DOWNLOAD)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_USING_HDF)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_CCB_ENCRYPT)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_FACTORY_LOADER_ERASE)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_NORMAL)
		ulNumOfDefault++;
#endif		
#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_USING_HDF)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_CW_MODE_ENABLE)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_OTP_LOCK_ENABLE)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_SECURE_BOOT_ENABLE)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_JTAG_ENABLE)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_NAND_DB_COPY)
		ulNumOfDefault++;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_MAKE_SPCB)
		ulNumOfDefault++;
#endif

		FAI_ERR_CHECK(FAI_MSGBOX_Create(&hCurMsgBox, 
										"Default Setting Result", 
										NULL,
										ulNumOfDefault+3, 
										INTRN_FAI_SYSTEM_DefaultSetHandler));

		ulNumOfDefault = 0;
#if (DEFAULT_SETTING_LIST&DEFAULT_CHECK_MES_DOWNLOAD)
		FDI_SYSTEM_CheckMesCompleted(&bMesDone);
		if( bMesDone == FALSE )
		{
#if 0 /* NPI 요청에 의해 메시지 삭제 */
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "MES Download failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);			
#endif
			goto TEST_FAIL;
		}
#if 0 /* NPI 요청에 의해 메시지 삭제 */
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "MES Download completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_STB_OWNER_ID)
		/* check STB owner ID */
		FDI_SYSTEM_GetSTBOwnerId(&usSTBOwnerId);
		if(usSTBOwnerId != 0x2b)
		{
			eFdiErr = FDI_SYSTEM_SetSTBOwnerId(0x2b);
			if( eFdiErr != FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_GetSTBOwnerId(STB Owner ID Error)\n"));
				FAI_MSGBOX_ChangeMsg(hCurMsgBox, "STB Owner ID is different.", ++ulNumOfDefault, RGB_COLOR_BLACK);
				goto TEST_FAIL;
			}
		}
#endif

#if 0 ///*removed from 40n*/(DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_USING_HDF)
		eFaiErr = INTRN_FAI_SYSTEM_CheckDefaultFile();
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_CheckDefaultFile\n"));
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "There is no default file.", ++ulNumOfDefault, RGB_COLOR_BLACK);
			goto TEST_FAIL;			
		}
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Default file found.", ++ulNumOfDefault, RGB_COLOR_BLACK);

		bSystemIDCheckOnly = TRUE;
		eFaiErr = INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(NULL, bSystemIDCheckOnly);		
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(System ID Error)\n"));
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "System ID is different.", ++ulNumOfDefault, RGB_COLOR_BLACK);
			goto TEST_FAIL;
		}
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "System ID is OK.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_CCB_ENCRYPT)
		eFdiErr = FDI_SYSTEM_EncryptCCB(&bSuccess);	
		if(eFdiErr != FDI_NO_ERR || bSuccess != TRUE)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EncryptCCB\n"));
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Encrypting CCB failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
			goto TEST_FAIL;			
		}
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Encrypting CCB completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);		
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_NORMAL)
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Default Data Write completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_FACTORY_LOADER_ERASE)
		eFdiErr = FDI_SYSTEM_EraseFactoryLoader();
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EraseFactoryLoader\n"));
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Factory Loader Erase failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
			goto TEST_FAIL;
		}
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Factory Loader Erase completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_MAKE_SPCB)
#if defined(FACTORY_USE_CAS_IR_UTIL)
		eFdiErr = FDI_CAS_IR_MakeSPCB();
		if(eFdiErr != FDI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_CAS_MakeSPCB Error\n"));
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Make SPCB Failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
			goto TEST_FAIL;
		}
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Make SPCB completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif		
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_DATA_WRITE_USING_HDF)
		bSystemIDCheckOnly = FALSE;
		eFaiErr = INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(&ucFusingFlag, bSystemIDCheckOnly);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_DefaultSetting\n"));
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Default Data Write failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
			goto TEST_FAIL;
		}
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Default Data Write completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#else
		ucFusingFlag = TRUE;
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_MARKET_ID)
		eFdiErr = FDI_SYSTEM_SetMarketID(DEFAULT_SETTING_MARKET_ID);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_SetMarketID\n"));
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Market ID write failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
			goto TEST_FAIL;
		}
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Market ID Write completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_NAND_DB_CLEAR)
		eFaiErr = INTRN_FAI_SYSTEM_DeleteDBFiles();
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_DeleteDBFiles\n"));
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Nand DB Clear failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
			goto TEST_FAIL;
		}
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Nand DB Clear completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif

#if (DEFAULT_SETTING_LIST&DEFAULT_NAND_DB_COPY)
		eFaiErr = INTRN_FAI_SYSTEM_CopyingDBFiles();
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : INTRN_FAI_SYSTEM_CopyingDBFiles\n"));
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Nand DB Copy failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
			goto TEST_FAIL;
		}
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Nand DB Copy completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif
		if( ucFusingFlag == TRUE )
		{
#if (DEFAULT_SETTING_LIST&DEFAULT_CW_MODE_ENABLE)
			eFdiErr = FDI_SYSTEM_EnableCwMode();
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableCwMode\n"));
				FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Secure CW Enable failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
				goto TEST_FAIL;
			}
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Secure CW Enable completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_OTP_LOCK_ENABLE)
			eFdiErr = FDI_SYSTEM_EnableOTPLock();
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableOTPLock\n"));
				FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Flash OTP Lock failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
				goto TEST_FAIL;
			}
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Flash OTP Lock completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_SECURE_BOOT_ENABLE)
			eFdiErr = FDI_SYSTEM_EnableSecureBoot();
			if( eFdiErr != FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableSecureBoot\n"));
				FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Secure Boot Enable failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
				goto TEST_FAIL;
			}
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "Secure Boot Enable completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif
#if (DEFAULT_SETTING_LIST&DEFAULT_JTAG_ENABLE)
			eFdiErr = FDI_SYSTEM_EnableJtag();
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSetPrepare] : FDI_SYSTEM_EnableJtag\n"));
				FAI_MSGBOX_ChangeMsg(hCurMsgBox, "JTAG Password Lock Enable failed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
				goto TEST_FAIL;
			}
			FAI_MSGBOX_ChangeMsg(hCurMsgBox, "JTAG Password Lock Enable completed.", ++ulNumOfDefault, RGB_COLOR_BLACK);
#endif
		}

		FDI_PANEL_Display(FAI_SYSTEM_DEFUALT_SETTING_PASS_STR);
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, " ", ++ulNumOfDefault, RGB_COLOR_BLACK);
		FAI_MSGBOX_ChangeMsg(hCurMsgBox, "\"Default setting is completed 100%.\"", ++ulNumOfDefault, RGB_COLOR_RED);
		g_IsPassDefaultSet = TRUE;
	}
	else if( unDeviceType == TEST_INPUT_TYPE_RCU && (unKeyCode == RCU_KEY_EXIT||unKeyCode == RCU_KEY_BACK) )
	{
		FDI_PANEL_Display(FAI_SYSTEM_DEFUALT_SETTING_EXIT_STR);
		FAI_ERR_CHECK(FAI_MSGBOX_GetCurrentMsgBox(&hCurMsgBox));
		FAI_ERR_CHECK(FAI_MSGBOX_Destoy(hCurMsgBox));
	}

	return FAI_NO_ERR;

TEST_FAIL:
	FDI_PANEL_Display(FAI_SYSTEM_DEFUALT_SETTING_FAIL_STR);	
	FAI_MSGBOX_ChangeMsg(hCurMsgBox, " ", ++ulNumOfDefault, RGB_COLOR_BLACK);
	FAI_MSGBOX_ChangeMsg(hCurMsgBox, "\"Default setting failed.\"", ++ulNumOfDefault, RGB_COLOR_RED);
	g_IsPassDefaultSet = FALSE;
	
	return FAI_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_DefaultSetting_UI(void)
{
	FAI_MSGBOX_H hCurMsgBox;
	char szDefaultSetPrepareInfo[3][FAI_MSG_STR_MAX];

	sprintf(szDefaultSetPrepareInfo[0], "Do you want Default Setting??");
	sprintf(szDefaultSetPrepareInfo[1], "Please, Push \"%s\" key from Front Panel.", FKeyCode2Str(DEFAULT_SETTING_UI_FRONT_INPUT));
	sprintf(szDefaultSetPrepareInfo[2], "Otherwise, Push \"EXIT/BACK\" key from RCU.");
		
	FDI_PANEL_Display(FAI_SYSTEM_DEFUALT_SETTING_READY_STR);
	
  	FAI_ERR_CHECK(FAI_MSGBOX_Create(&hCurMsgBox,
									"DEFAULT SETTING",
									szDefaultSetPrepareInfo,
									3,
									INTRN_FAI_SYSTEM_DefaultSetPrepare));

	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SW_RESET_UI_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_SWReset_UI(void)
{
	FDI_ERR_CODE eFdiErr ;
	
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "S/W Reset", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	FVK_TASK_Sleep(1000);
	
	eFdiErr = FDI_SYSTEM_SetSwReset();
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_SWReset_UI] : SW reset error\n"));
		return FAI_ERR;
	}
	
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (POWER_CONTROL_UI_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_PowerCtrl_UI(void)
{
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function : FAI_SYSTEM_DTCPCheck_UI
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
#if (DTCP_UI_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_DTCPCheck_UI(void)
{
	FAI_ERR_CODE eFaiErr;

	/* Display DTCP test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "DTCP Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);	

	eFaiErr = INTRN_FAI_SYSTEM_GetDTCP_Validity();
	if( eFaiErr == FAI_NO_ERR )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "DTCP Test", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_DTCP_PASS_STR);
	}
	else
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "DTCP Test", RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED); 
		FDI_PANEL_Display(FAI_DTCP_FAIL_STR);
	}

	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function : FAI_SYSTEM_CiPlusCheck_UI
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CIPLUS_UI_ENABLE==YES)
FAI_ERR_CODE FAI_SYSTEM_CiPlusCheck_UI(void)
{
	FAI_ERR_CODE eFaiErr;

	/* Display CI+ test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI+ Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	eFaiErr = INTRN_FAI_SYSTEM_GetCiPlus_Validity();
	if( eFaiErr == FAI_NO_ERR )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI+ Test", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_CIPLUS_PASS_STR);
	}
	else
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI+ Test", RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
		FDI_PANEL_Display(FAI_CIPLUS_FAIL_STR);
	}

	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_SYSTEM_ChangeSheet1(void)
{
	FAI_WINDOW_H hCurWin, hCurSheetWin, hSheet1Win;
	FAI_WIDGET_H hCurWidget;
	FAI_PP_SHEET eCurSheet;

	FAI_ERR_CHECK(FAI_WINDOW_GetCurrentWindow(&hCurWin));
	FAI_ERR_CHECK(FAI_WINDOW_GetFocusWidget(hCurWin, &hCurWidget));
	FAI_ERR_CHECK(FAI_APP_GetCurrentSheet(&eCurSheet));
	FAI_ERR_CHECK(FAI_APP_GetSheetWindowHandle(eCurSheet, &hCurSheetWin));
	FAI_ERR_CHECK(FAI_APP_GetSheetWindowHandle(FAI_PP_SHEET_1, &hSheet1Win));
	
	if( eCurSheet == FAI_PP_SHEET_1 )
	{
		return FAI_NO_ERR;
	}
	else if( eCurSheet == FAI_PP_SHEET_2 )
	{
		FAI_ERR_CHECK(FAI_WINDOW_MoveWidget(hCurWin, hCurWidget, hCurWidget->ptLeft));
		FAI_ERR_CHECK(FAI_WIDGET_Move(&hCurSheetWin->tWidget, &hSheet1Win->tWidget));
		FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_1));
	}
	else if( eCurSheet == FAI_PP_SHEET_3 )
	{
		FAI_ERR_CHECK(FAI_WINDOW_MoveWidget(hCurWin, hCurWidget, hCurWidget->ptLeft->ptLeft));
		FAI_ERR_CHECK(FAI_WIDGET_Move(&hCurSheetWin->tWidget, &hSheet1Win->tWidget));
		FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_1));
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_SYSTEM_ChangeSheet2(void)
{
	FAI_WINDOW_H hCurWin, hCurSheetWin, hSheet2Win;
	FAI_WIDGET_H hCurWidget;
	FAI_PP_SHEET eCurSheet;

	FAI_ERR_CHECK(FAI_WINDOW_GetCurrentWindow(&hCurWin));
	FAI_ERR_CHECK(FAI_WINDOW_GetFocusWidget(hCurWin, &hCurWidget));
	FAI_ERR_CHECK(FAI_APP_GetCurrentSheet(&eCurSheet));
	FAI_ERR_CHECK(FAI_APP_GetSheetWindowHandle(eCurSheet, &hCurSheetWin));
	FAI_ERR_CHECK(FAI_APP_GetSheetWindowHandle(FAI_PP_SHEET_2, &hSheet2Win));
	
	if( eCurSheet == FAI_PP_SHEET_1 )
	{
		FAI_ERR_CHECK(FAI_WINDOW_MoveWidget(hCurWin, hCurWidget, hCurWidget->ptRight));
		FAI_ERR_CHECK(FAI_WIDGET_Move(&hCurSheetWin->tWidget, &hSheet2Win->tWidget));
		FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_2));
	}
	else if( eCurSheet == FAI_PP_SHEET_2 )
	{
		return FAI_NO_ERR;
	}
	else if( eCurSheet == FAI_PP_SHEET_3 )
	{
		FAI_ERR_CHECK(FAI_WINDOW_MoveWidget(hCurWin, hCurWidget, hCurWidget->ptLeft));
		FAI_ERR_CHECK(FAI_WIDGET_Move(&hCurSheetWin->tWidget, &hSheet2Win->tWidget));
		FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_2));
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_SYSTEM_ChangeSheet3(void)
{
	FAI_WINDOW_H hCurWin, hCurSheetWin, hSheet1Win;
	FAI_WIDGET_H hCurWidget;
	FAI_PP_SHEET eCurSheet;

	FAI_ERR_CHECK(FAI_WINDOW_GetCurrentWindow(&hCurWin));
	FAI_ERR_CHECK(FAI_WINDOW_GetFocusWidget(hCurWin, &hCurWidget));
	FAI_ERR_CHECK(FAI_APP_GetCurrentSheet(&eCurSheet));
	FAI_ERR_CHECK(FAI_APP_GetSheetWindowHandle(eCurSheet, &hCurSheetWin));
	FAI_ERR_CHECK(FAI_APP_GetSheetWindowHandle(FAI_PP_SHEET_3, &hSheet1Win));
	
	if( eCurSheet == FAI_PP_SHEET_1 )
	{
		FAI_ERR_CHECK(FAI_WINDOW_MoveWidget(hCurWin, hCurWidget, hCurWidget->ptRight->ptRight));
		FAI_ERR_CHECK(FAI_WIDGET_Move(&hCurSheetWin->tWidget, &hSheet1Win->tWidget));
		FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_3));
	}
	else if( eCurSheet == FAI_PP_SHEET_2 )
	{
		FAI_ERR_CHECK(FAI_WINDOW_MoveWidget(hCurWin, hCurWidget, hCurWidget->ptRight));
		FAI_ERR_CHECK(FAI_WIDGET_Move(&hCurSheetWin->tWidget, &hSheet1Win->tWidget));
		FAI_ERR_CHECK(FAI_APP_SetCurrentSheet(FAI_PP_SHEET_3));
	}
	else if( eCurSheet == FAI_PP_SHEET_3 )
	{
		return FAI_NO_ERR;
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_SYSTEM_GetDTCP_Validity(void)
{
	int i;
	FDI_ERR_CODE eFdiErr;
	unsigned char szDtcpKey[FDI_SYSTEM_DATA_DTCPKEY_SIZE];

	eFdiErr = FDI_SYSTEM_GetDtcpKey(szDtcpKey, FDI_SYSTEM_DATA_DTCPKEY_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		return FAI_ERR;
	}

	/* check empry or not */
	for(i=0; i<FDI_SYSTEM_DATA_DTCPKEY_SIZE; i++)
	{
		if( (szDtcpKey[i]!=0x00) && (szDtcpKey[i]!=0xff) )
			break;
	}

	if( i>= FDI_SYSTEM_DATA_DTCPKEY_SIZE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_GetDTCP_Validity] : DTCP KEY is empty!!\n"));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_SYSTEM_GetCiPlus_Validity(void)
{
	int i;
	FDI_ERR_CODE eFdiErr;
	unsigned char szCiPlusKey[FDI_SYSTEM_DATA_CIPLUSKEY_SIZE];

	eFdiErr = FDI_SYSTEM_GetCiPlusKeyData(szCiPlusKey, FDI_SYSTEM_DATA_CIPLUSKEY_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		return FAI_ERR;
	}

	/* check empry or not */
	for(i=0; i<FDI_SYSTEM_DATA_CIPLUSKEY_SIZE; i++)
	{
		if( (szCiPlusKey[i]!=0x00) && (szCiPlusKey[i]!=0xff) )
			break;
	}

	if( i>= FDI_SYSTEM_DATA_CIPLUSKEY_SIZE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_GetCiPlus_Validity] : CI+ KEY is empty!!\n"));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_SYSTEM_CheckDefaultFile(void)
{
	int nUsbFd;
	unsigned char szFileName[80], *szConvFileNamePtr;


	/* open usb file */
	sprintf(szFileName, "%s%s", USB_FILE_PATH, DEFAULT_SETTING_FILENAME);
	if( (nUsbFd=open(szFileName, O_RDWR | O_NONBLOCK)) < 0 )
	{
		/* check no fusing data */
		szConvFileNamePtr = strstr(szFileName, ".hdf");
		if( szConvFileNamePtr != NULL )
		{
			strcpy(szConvFileNamePtr, "_no_fusing.hdf");
		}

		if( (nUsbFd=open(szFileName, O_RDWR | O_NONBLOCK)) < 0 )
		{
#if !defined(CONFIG_OS_RTOS)			
			perror("open : ");
#endif
			close(nUsbFd);
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_CheckDefaultFile] : open Error!!\n"));
			return FAI_ERR;
		}
	}

	close(nUsbFd);
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (USE_NAND_FLASH==YES)
#define UBI_PART_ALIGN	(1*1024*1024)
#define LEB_BASED_SIZE(rSz)	(((rSz+(LOGICAL_BLOCK_SIZE-1))/LOGICAL_BLOCK_SIZE)*LOGICAL_BLOCK_SIZE)
#define PHY_ALLOCATED_SIZE(pSz) ((LEB_BASED_SIZE(pSz)+(UBI_PART_ALIGN-1))&(~(UBI_PART_ALIGN-1)))
#endif

static unsigned char g_aucBlockBuf[HDF_DEFAULT_DATA_BLOCK_SIZE+MAX_HDF_DATA_BLOCK_SIZE];
static unsigned char g_aucReadBlockBuf[HDF_DEFAULT_DATA_BLOCK_SIZE+MAX_HDF_DATA_BLOCK_SIZE];
static FAI_ERR_CODE INTRN_FAI_SYSTEM_DefaultSettingUsingHDF(unsigned char *pucFusingFlag, BOOL bIDCheckOnly)
{
#if (USE_NAND_FLASH==YES)
	FAI_ERR_CODE eFaiErr;	
	int nUsbFd, nUsbFileSize, nBlockIdx, nDataIdx, nReadSize, nUsbFilePos = 0;
	unsigned char *pucUsbFileBuf;
	HDF_HEADER_BLOCK_T tHdfHeaderBlock;
	HDF_DATA_BLOCK_T tHdfDataBlock;
	unsigned char ucDataType;
	unsigned char szFileName[80], *szConvFileNamePtr;
	unsigned char ucProgressRate;
	unsigned long ulSystemId;
	unsigned char *loader_buf = NULL;
	unsigned int loader_pos = 0;
	unsigned char *pre_loader_buf = NULL;
	unsigned int pre_loader_pos = 0;

	unsigned char *db_buf = NULL;
	unsigned int db_pos = 0;
	unsigned char *dbbackup_buf = NULL;
	unsigned int dbback_pos = 0;
	unsigned char *dbuser_buf = NULL;
	unsigned int dbuser_pos = 0;
	long long int nandubidb_leb = 0;
	long long int nandubidbbackup_leb = 0;
	long long int nandubidbuser_leb = 0;
	unsigned char *unpack_buf = NULL;
	unsigned int unpack_pos = 0;
	HUINT32	result;

    if ( pucFusingFlag != NULL )
    {
	*pucFusingFlag = TRUE;
    }

	FAI_SYSTEM_UnMountDBandDBBACKUP();
	FAI_SYSTEM_UBIFSInit();

	/* open usb file */
	sprintf(szFileName, "%s%s", USB_FILE_PATH, DEFAULT_SETTING_FILENAME);
	if( (nUsbFd=open(szFileName, O_RDWR | O_NONBLOCK)) < 0 )
	{
		/* check no fusing data */
		szConvFileNamePtr = strstr(szFileName, ".hdf");
		if( szConvFileNamePtr != NULL )
		{
			strcpy(szConvFileNamePtr, "_no_fusing.hdf");
		}

		if( (nUsbFd=open(szFileName, O_RDWR | O_NONBLOCK)) > 0 )
		{
			if( pucFusingFlag != NULL )
			{
				*pucFusingFlag = FALSE;
			}
		}
		else
		{
#if !defined(CONFIG_OS_RTOS)
			perror("open : ");
#endif
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : open Error!!\n"));
			return FAI_ERR;
		}
	}

	/* get file size */
	if( (nUsbFileSize=lseek(nUsbFd, 0, SEEK_END)) < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : lseek Error!!\n"));
		close(nUsbFd);
		return FAI_ERR;
	}
	FAI_PRINT(0, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : file name %s, file size=%d\n", szFileName, nUsbFileSize));

	/* alloc memory for data buffer */
	pucUsbFileBuf = FVK_MEM_Alloc(nUsbFileSize);
	if( pucUsbFileBuf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc(pucUsbFileBuf) Error!!\n"));
		close(nUsbFd);		
		return FAI_ERR;
	}

	pre_loader_buf = FVK_MEM_Alloc(4*1024*1024);
	if( pre_loader_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc(loader_buf) Error!!\n"));
		close(nUsbFd);
		FVK_MEM_Free(pucUsbFileBuf);
		return FAI_ERR;
	}

	loader_buf = FVK_MEM_Alloc(4*1024*1024);
	if( loader_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc(loader_buf) Error!!\n"));
		close(nUsbFd);
		FVK_MEM_Free(pucUsbFileBuf);
		return FAI_ERR;
	}
	unpack_buf = FVK_MEM_Alloc(2*1024*1024);
	if( unpack_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc(unpack_buf) Error!!\n"));
		close(nUsbFd);
		FVK_MEM_Free(pucUsbFileBuf);
		FVK_MEM_Free(loader_buf);
		return FAI_ERR;
	}

	/* move to beginning of file */
	if( lseek(nUsbFd, 0, SEEK_SET) != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : lseek Error!!\n"));
		goto TEST_FAIL;
	}

	/* read file */
	nReadSize = read(nUsbFd, pucUsbFileBuf, nUsbFileSize);
	if( nReadSize != nUsbFileSize )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : read Error!!\n"));
		goto TEST_FAIL;
	}

	FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : get header!!\n"));
	/* get hdf header block */
	eFaiErr = FAI_UTIL_GetHdfHeaderBlock(pucUsbFileBuf, &tHdfHeaderBlock);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FAI_UTIL_GetHdfHeaderBlock Error!!\n"));
		goto TEST_FAIL;
	}
	
	nUsbFilePos += HDF_HEADER_BLOCK_SIZE;

	/* check System ID */
	FDI_SYSTEM_GetSystemId(&ulSystemId);
	if( (ulSystemId<tHdfHeaderBlock.ulStartID) || (ulSystemId>tHdfHeaderBlock.ulEndID) )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : Invalid System ID 0x%x (0x%x~0x%x)\n",
				ulSystemId, tHdfHeaderBlock.ulStartID, tHdfHeaderBlock.ulEndID));
		goto TEST_FAIL;
	}

	dbbackup_buf = FVK_MEM_Alloc(PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE) + 1024*1024);
	if( dbbackup_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc Error!!\n"));
		goto TEST_FAIL;
	}

	db_buf = FVK_MEM_Alloc(PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE) + 1024*1024);
	if( db_buf == NULL )
		{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc Error!!\n"));
		goto TEST_FAIL;
	}

	dbuser_buf = FVK_MEM_Alloc(PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE) + 1024*1024);
	if( dbuser_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc Error!!\n"));
		goto TEST_FAIL;
	}

	if(bIDCheckOnly !=TRUE)
	{		
		tHdfDataBlock.aucData = g_aucBlockBuf;
		for( nBlockIdx = 0; nBlockIdx < tHdfHeaderBlock.usNumOfDataBlock; nBlockIdx++ )
		{
			/* get hdf data block */
			eFaiErr = FAI_UTIL_GetHdfDataBlock(pucUsbFileBuf+nUsbFilePos, &tHdfDataBlock);
			if( eFaiErr != FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FAI_UTIL_GetHdfDataBlock Error!!\n"));
				goto TEST_FAIL;
			}
			
			nUsbFilePos += (tHdfDataBlock.usSize+2);

			ucDataType = tHdfDataBlock.usFlag&HDF_DATA_TYPE_MASK;
			if( ucDataType == HDF_DATA_TYPE_SIGNATURE ||
				ucDataType == HDF_DATA_TYPE_LOADER 
				)
			{
				continue;
			}

#if !defined(FACTORY_USE_DI20) || (defined(FACTORY_USE_DI20) && defined(FACTORY_USE_EEPROM))
			if( ucDataType == HDF_DATA_TYPE_EEPROM )
			{
				if( FDI_EEPROM_Write(tHdfDataBlock.ulDataAddr, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_EEPROM_Write Error!!\n"));
					goto TEST_FAIL;
				}
				/* Read flash data for verification */
				if( FDI_EEPROM_Read(tHdfDataBlock.ulDataAddr, g_aucReadBlockBuf, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_EEPROM_Read Error!!\n"));
					goto TEST_FAIL;
				}
				/* Data compare */
				if( FVK_MEM_Compare(tHdfDataBlock.aucData, g_aucReadBlockBuf, tHdfDataBlock.usOrigDataSize) != 0 )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : EEPROM Data Compare Error!!\n"));
					goto TEST_FAIL;
				}
			}
			else
#endif
			if( ucDataType == HDF_DATA_TYPE_BETACODE )
			{
				HUINT32		betaCode;
				HUINT32		systemId;
				
				betaCode = (tHdfDataBlock.ulDataAddr >> 16 & 0xFFFF);
				if (betaCode >= 0x8000)
				{
					FVK_MEM_Copy(unpack_buf + unpack_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
					unpack_pos += tHdfDataBlock.usOrigDataSize;

					continue;
				}

				if (unpack_pos != 0)
				{
					result = FDI_NVRAM_UnpackNvramCfg(unpack_buf, unpack_pos);
					if (result != ERR_OK)
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] NVRAM unpack error(0x%x)\n", result));
						goto TEST_FAIL;
					}
					
					unpack_pos = 0;
				}

				if (betaCode >= s_sizeBetaTable)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] Error Invalid Beta Code (%d)\n", betaCode));
					goto TEST_FAIL;
				}
				if (s_betaTable[betaCode] == DI_NVRAM_FIELD_SYSTEM_ID)
				{
					systemId = (tHdfDataBlock.aucData[0] << 24) | (tHdfDataBlock.aucData[1] << 16) 
							| (tHdfDataBlock.aucData[2] << 8) | (tHdfDataBlock.aucData[3]);
					result = DI_NVRAM_SetField(DI_NVRAM_FIELD_SYSTEM_ID, 0, &systemId, sizeof(HUINT32));
				}
				else
				{
					result = DI_NVRAM_SetField((DI_NVRAM_FIELD_T)s_betaTable[betaCode], 0, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				}
				if (result)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("Error NVRAM\n"));
					goto TEST_FAIL;
				}
			}
			else if(( ucDataType == HDF_DATA_TYPE_FLASH ) || ( ucDataType == HDF_DATA_TYPE_APPLICATION ))
			{
				if(tHdfDataBlock.ulDataAddr == (pre_loader_pos + 0x80000000))
				{
//					if(pre_loader_pos <= 131072)
//					{
						FVK_MEM_Copy(pre_loader_buf + pre_loader_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
						pre_loader_pos += tHdfDataBlock.usOrigDataSize;
//					}
				}
				else if(tHdfDataBlock.ulDataAddr == (loader_pos + 0x80000000 + 0x21000))
				{
					FVK_MEM_Copy(loader_buf + loader_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
					loader_pos += tHdfDataBlock.usOrigDataSize;
				}
				else if(tHdfDataBlock.ulDataAddr == (dbback_pos + 0x80000000 + FLASH_UBIFS_AREA_START))
				{
					FVK_MEM_Copy(dbbackup_buf + dbback_pos , tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
					dbback_pos += tHdfDataBlock.usOrigDataSize;
				}
				else if(tHdfDataBlock.ulDataAddr == (db_pos + 0x80000000 + FLASH_UBIFS_AREA_START + PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)))
				{
					FVK_MEM_Copy(db_buf + db_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
					db_pos += tHdfDataBlock.usOrigDataSize;
				}
				else if(tHdfDataBlock.ulDataAddr == (dbuser_pos + 0x80000000 + FLASH_UBIFS_AREA_START + PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE) + PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)))
				{
					FVK_MEM_Copy(dbuser_buf + dbuser_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
					dbuser_pos += tHdfDataBlock.usOrigDataSize;
				}
				
#if !defined(FACTORY_USE_DI20) || (defined(FACTORY_USE_DI20) && defined(FACTORY_USE_NOR_FLASH))				
				else
				{
				if( FDI_FLASH_WriteAfterErase(tHdfDataBlock.ulDataAddr, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_FLASH_WriteAfterErase Error!!\n"));
						goto TEST_FAIL;
				}

				/* Read flash data for verification */
					if( FDI_FLASH_Read(tHdfDataBlock.ulDataAddr, g_aucReadBlockBuf, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_FLASH_Read Error!!\n"));
						goto TEST_FAIL;
					}

					/* Data compare */
					if( FVK_MEM_Compare(tHdfDataBlock.aucData, g_aucReadBlockBuf, tHdfDataBlock.usOrigDataSize) != 0 )
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : Flash Data Compare Error!!\n"));
						goto TEST_FAIL;
					}
				}
#endif				
			}
		}
#if 0
		if (unpack_pos != 0)
		{
			result = FDI_NVRAM_UnpackNvramCfg(unpack_buf, unpack_pos);
			if (result != ERR_OK)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] NVRAM unpack error(0x%x)\n", result));
				goto TEST_FAIL;
			}

			unpack_pos = 0;
		}
#endif
		if(pre_loader_pos != 0)
		{
			unsigned char *compare_buf = NULL;
			HUINT32		nGangWrited = 0;
			HUINT32		result;

			compare_buf = FVK_MEM_Alloc(pre_loader_pos);
			if( compare_buf == NULL )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc Error!!\n"));
				goto TEST_FAIL;
			}
	
			if( FDI_NANDFLASH_Write(0x0, pre_loader_buf, pre_loader_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			
			if( FDI_NANDFLASH_Read(0x0, compare_buf, pre_loader_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_FLASH_Read Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}

			/* Data compare */
			if( FVK_MEM_Compare(pre_loader_buf, compare_buf, pre_loader_pos) != 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : Flash Data Compare Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			
			FVK_MEM_Free(compare_buf);			
#if 0
			result = DI_NVRAM_SetField(DI_NVRAM_FIELD_FACTORY_GANG_WRITED, 0, &nGangWrited, sizeof(HUINT32));
			if (result)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("Error NVRAM\n"));
				goto TEST_FAIL;
			}
#endif
		}
		if(loader_pos != 0)
		{
			unsigned char *compare_buf = NULL;
			HUINT32		nGangWrited = 0;
			HUINT32		result;

			compare_buf = FVK_MEM_Alloc(loader_pos);
			if( compare_buf == NULL )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc Error!!\n"));
				goto TEST_FAIL;
			}
	
			if( FDI_NANDFLASH_Write(0x21000, loader_buf, loader_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			
			if( FDI_NANDFLASH_Read(0x21000, compare_buf, loader_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_FLASH_Read Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}

			/* Data compare */
			if( FVK_MEM_Compare(loader_buf, compare_buf, loader_pos) != 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : Flash Data Compare Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			
			FVK_MEM_Free(compare_buf);			
#if 0
			result = DI_NVRAM_SetField(DI_NVRAM_FIELD_FACTORY_GANG_WRITED, 0, &nGangWrited, sizeof(HUINT32));
			if (result)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("Error NVRAM\n"));
				goto TEST_FAIL;
			}
#endif
		}

		if(dbback_pos != 0)
		{
			unsigned char *compare_buf = NULL;

			if(DI_NANDFLASH_UBIFS_Open(0, &nandubidbbackup_leb)!= DI_ERR_OK)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Open (%d)\n", 1));
				goto TEST_FAIL;
			}

			compare_buf = FVK_MEM_Alloc(dbback_pos);
			if( compare_buf == NULL )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc Error!!\n"));
				goto TEST_FAIL;
			}
	
			if( FDI_NANDFLASH_Write(FLASH_UBIFS_AREA_START, dbbackup_buf, dbback_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			
			if( FDI_NANDFLASH_Read(FLASH_UBIFS_AREA_START, compare_buf, dbback_pos) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_FLASH_Read Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}

			/* Data compare */
			if( FVK_MEM_Compare(dbbackup_buf, compare_buf, dbback_pos) != 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : Flash Data Compare Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			
			FVK_MEM_Free(compare_buf);			

			if( FDI_NANDFLASH_Sync() != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FDI_NANDFLASH_Sync Error!!\n"));
				goto TEST_FAIL;
			}

			if(DI_NANDFLASH_UBIFS_Close(0) != DI_ERR_OK)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Close (%d)\n", 0));
				goto TEST_FAIL;
			}

			nandubidbbackup_leb = 0;
			
		}

		if(db_pos != 0)
		{
			unsigned char *compare_buf = NULL;

			if(DI_NANDFLASH_UBIFS_Open(1, &nandubidb_leb) != DI_ERR_OK)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Open (%d)\n", 0));
				goto TEST_FAIL;
			}

			compare_buf = FVK_MEM_Alloc(db_pos);
			if( compare_buf == NULL )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc Error!!\n"));
				goto TEST_FAIL;
			}
	
			if( FDI_NANDFLASH_Write(FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE), db_buf, db_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			
			if( FDI_NANDFLASH_Read(FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE), compare_buf, db_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_FLASH_Read Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
				}

				/* Data compare */
			if( FVK_MEM_Compare(db_buf, compare_buf, db_pos) != 0 )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : Flash Data Compare Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			
			FVK_MEM_Free(compare_buf);			

			if( FDI_NANDFLASH_Sync() != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FDI_NANDFLASH_Sync Error!!\n"));
				goto TEST_FAIL;
			}

			if(DI_NANDFLASH_UBIFS_Close(1) != DI_ERR_OK)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Close (%d)\n", 1));
				goto TEST_FAIL;
			}

			nandubidb_leb = 0;
			
				}

		if(dbuser_pos != 0)
		{
			unsigned char *compare_buf = NULL;

			if(DI_NANDFLASH_UBIFS_Open(2, &nandubidbuser_leb) != DI_ERR_OK)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Open (%d)\n", 0));
				goto TEST_FAIL;
			}

			compare_buf = FVK_MEM_Alloc(dbuser_pos);
			if( compare_buf == NULL )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc Error!!\n"));
				goto TEST_FAIL;
			}
	
			if( FDI_NANDFLASH_Write(FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE), dbuser_buf, dbuser_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
		}		
			
			if( FDI_NANDFLASH_Read(FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE), compare_buf, dbuser_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_FLASH_Read Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}

			/* Data compare */
			if( FVK_MEM_Compare(dbuser_buf, compare_buf, dbuser_pos) != 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : Flash Data Compare Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			
			FVK_MEM_Free(compare_buf);			

			if( FDI_NANDFLASH_Sync() != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FDI_NANDFLASH_Sync Error!!\n"));
				goto TEST_FAIL;
			}

			if(DI_NANDFLASH_UBIFS_Close(2) != DI_ERR_OK)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Close (%d)\n", 1));
				goto TEST_FAIL;
			}
			nandubidbuser_leb = 0;
			
	}

	}

	FVK_MEM_Free(pre_loader_buf);
	FVK_MEM_Free(loader_buf);
	FVK_MEM_Free(unpack_buf);

	FVK_MEM_Free(dbbackup_buf);
	FVK_MEM_Free(db_buf);
	FVK_MEM_Free(dbuser_buf);
	
	FVK_MEM_Free(pucUsbFileBuf);
	close(nUsbFd);	

	return FAI_NO_ERR;

TEST_FAIL:
	FVK_MEM_Free(pre_loader_buf);
	FVK_MEM_Free(loader_buf);
	FVK_MEM_Free(unpack_buf);
	FVK_MEM_Free(dbbackup_buf);
	FVK_MEM_Free(db_buf);
	FVK_MEM_Free(dbuser_buf);
	FVK_MEM_Free(pucUsbFileBuf);	
	close(nUsbFd);
	if(nandubidbbackup_leb > 0)
		DI_NANDFLASH_UBIFS_Close(0);
	if(nandubidb_leb > 0)
		DI_NANDFLASH_UBIFS_Close(1);
	if(nandubidbuser_leb > 0)
		DI_NANDFLASH_UBIFS_Close(2);
#elif (USE_NOR_FLASH==YES && USE_EMMC_FLASH==YES) /*when both NOR & EMMC used*/
	FAI_ERR_CODE eFaiErr;	
	int nUsbFd, nUsbFileSize, nBlockIdx, nDataIdx, nReadSize, nUsbFilePos = 0;
	unsigned char *pucUsbFileBuf;
	HDF_HEADER_BLOCK_T tHdfHeaderBlock;
	HDF_DATA_BLOCK_T tHdfDataBlock;
	unsigned char ucDataType;
	unsigned char szFileName[80], *szConvFileNamePtr;
	unsigned char ucProgressRate;
	unsigned long ulSystemId;
	unsigned char *loader_buf = NULL;
	unsigned int loader_pos = 0;
	unsigned char *unpack_buf = NULL;
	unsigned int unpack_pos = 0;
	HUINT32	result;
	
    if ( pucFusingFlag != NULL )
    {
	*pucFusingFlag = TRUE; /*no fusing allowed yet*/
    }

//	FAI_SYSTEM_UnMountDBandDBBACKUP();
//	FAI_SYSTEM_UBIFSInit();

	/* open usb file */
	sprintf(szFileName, "%s%s", USB_FILE_PATH, DEFAULT_SETTING_FILENAME);
	if( (nUsbFd=open(szFileName, O_RDWR | O_NONBLOCK)) < 0 )
	{
		/* check no fusing data */
		szConvFileNamePtr = strstr(szFileName, ".hdf");
		if( szConvFileNamePtr != NULL )
		{
			strcpy(szConvFileNamePtr, "_no_fusing.hdf");
		}

		if( (nUsbFd=open(szFileName, O_RDWR | O_NONBLOCK)) > 0 )
		{
			if( pucFusingFlag != NULL )
			{
				*pucFusingFlag = FALSE;
			}
		}
		else
		{
#if !defined(CONFIG_OS_RTOS)
			perror("open : ");
#endif
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : open Error!!\n"));
			return FAI_ERR;
		}
	}

	/* get file size */
	if( (nUsbFileSize=lseek(nUsbFd, 0, SEEK_END)) < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : lseek Error!!\n"));
		close(nUsbFd);
		return FAI_ERR;
	}
	FAI_PRINT(0, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : file name %s, file size=%d\n", szFileName, nUsbFileSize));

	/* alloc memory for data buffer */
	pucUsbFileBuf = FVK_MEM_Alloc(nUsbFileSize);
	if( pucUsbFileBuf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc(pucUsbFileBuf) Error!!\n"));
		close(nUsbFd);		
		return FAI_ERR;
	}

	loader_buf = FVK_MEM_Alloc(4*1024*1024);
	if( loader_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc(loader_buf) Error!!\n"));
		close(nUsbFd);
		FVK_MEM_Free(pucUsbFileBuf);
		return FAI_ERR;
	}
	unpack_buf = FVK_MEM_Alloc(2*1024*1024);
	if( unpack_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc(unpack_buf) Error!!\n"));
		close(nUsbFd);
		FVK_MEM_Free(pucUsbFileBuf);
		FVK_MEM_Free(loader_buf);
		return FAI_ERR;
	}

	/* move to beginning of file */
	if( lseek(nUsbFd, 0, SEEK_SET) != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : lseek Error!!\n"));
		goto TEST_FAIL;
	}

	/* read file */
	nReadSize = read(nUsbFd, pucUsbFileBuf, nUsbFileSize);
	if( nReadSize != nUsbFileSize )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : read Error!!\n"));
		goto TEST_FAIL;
	}

	FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : get header!!\n"));
	/* get hdf header block */
	eFaiErr = FAI_UTIL_GetHdfHeaderBlock(pucUsbFileBuf, &tHdfHeaderBlock);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FAI_UTIL_GetHdfHeaderBlock Error!!\n"));
		goto TEST_FAIL;
	}
	
	nUsbFilePos += HDF_HEADER_BLOCK_SIZE;

	/* check System ID */
	FDI_SYSTEM_GetSystemId(&ulSystemId);
	if( (ulSystemId<tHdfHeaderBlock.ulStartID) || (ulSystemId>tHdfHeaderBlock.ulEndID) )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : Invalid System ID 0x%x (0x%x~0x%x)\n",
				ulSystemId, tHdfHeaderBlock.ulStartID, tHdfHeaderBlock.ulEndID));
		goto TEST_FAIL;
	}

	if(bIDCheckOnly !=TRUE)
	{		
		tHdfDataBlock.aucData = g_aucBlockBuf;
		for( nBlockIdx = 0; nBlockIdx < tHdfHeaderBlock.usNumOfDataBlock; nBlockIdx++ )
		{
			/* get hdf data block */
			eFaiErr = FAI_UTIL_GetHdfDataBlock(pucUsbFileBuf+nUsbFilePos, &tHdfDataBlock);
			if( eFaiErr != FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FAI_UTIL_GetHdfDataBlock Error!!\n"));
				goto TEST_FAIL;
			}
			
			nUsbFilePos += (tHdfDataBlock.usSize+2);

			ucDataType = tHdfDataBlock.usFlag&HDF_DATA_TYPE_MASK;
			if( ucDataType == HDF_DATA_TYPE_SIGNATURE ||
				ucDataType == HDF_DATA_TYPE_LOADER 
				)
			{
				continue;
			}

			if( ucDataType == HDF_DATA_TYPE_BETACODE )
			{
				HUINT32		betaCode;
				HUINT32		systemId;
				
				betaCode = (tHdfDataBlock.ulDataAddr >> 16 & 0xFFFF);
				if (betaCode >= 0x8000)
				{
					FVK_MEM_Copy(unpack_buf + unpack_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
					unpack_pos += tHdfDataBlock.usOrigDataSize;

					continue;
				}

				if (unpack_pos != 0)
				{
					result = FDI_NVRAM_UnpackNvramCfg(unpack_buf, unpack_pos);
					if (result != ERR_OK)
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] NVRAM unpack error(0x%x)\n", result));
						goto TEST_FAIL;
					}
					
					unpack_pos = 0;
				}

				if (betaCode >= s_sizeBetaTable)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] Error Invalid Beta Code (%d)\n", betaCode));
					goto TEST_FAIL;
				}
				if (s_betaTable[betaCode] == DI_NVRAM_FIELD_SYSTEM_ID)
				{
					systemId = (tHdfDataBlock.aucData[0] << 24) | (tHdfDataBlock.aucData[1] << 16) 
							| (tHdfDataBlock.aucData[2] << 8) | (tHdfDataBlock.aucData[3]);
					result = DI_NVRAM_SetField(DI_NVRAM_FIELD_SYSTEM_ID, 0, &systemId, sizeof(HUINT32));
				}
				else
				{
					result = DI_NVRAM_SetField((DI_NVRAM_FIELD_T)s_betaTable[betaCode], 0, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				}
				if (result)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("Error NVRAM\n"));
					goto TEST_FAIL;
				}
			}
			else if(( ucDataType == HDF_DATA_TYPE_FLASH ) || ( ucDataType == HDF_DATA_TYPE_APPLICATION ))
			{		
				if(tHdfDataBlock.ulDataAddr == (loader_pos+0x00000000))
				{
					printf("[hee] Update loader area.\n");
					FVK_MEM_Copy(loader_buf + loader_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
					loader_pos += tHdfDataBlock.usOrigDataSize;
				}
			}
		}


		if(loader_pos != 0)
		{
			unsigned char *compare_buf = NULL;
			HUINT32 	nGangWrited = 0;
			HUINT32 	result; 
			int heei=0;
			int heemax=0;
			printf("allocate compare buffer of size %d\n", loader_pos);
			compare_buf = FVK_MEM_Alloc(loader_pos);
			if( compare_buf == NULL )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FVK_MEM_Alloc Error!!\n"));
				goto TEST_FAIL;
			}

			if( FDI_FLASH_WriteAfterErase(0x0, loader_buf, loader_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}
			if( FDI_FLASH_Read(0x0, compare_buf, loader_pos) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_FLASH_Read Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}

			/* Data compare */
			if( FVK_MEM_Compare(loader_buf, compare_buf, loader_pos) != 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : Flash Data Compare Error!!\n"));
				FVK_MEM_Free(compare_buf);				
				goto TEST_FAIL;
			}

			FVK_MEM_Free(compare_buf);					
		}
	

	}

	FVK_MEM_Free(loader_buf);
	FVK_MEM_Free(unpack_buf);
	FVK_MEM_Free(pucUsbFileBuf);
	close(nUsbFd);	

	return FAI_NO_ERR;
TEST_FAIL:
	
	FVK_MEM_Free(loader_buf);
	FVK_MEM_Free(unpack_buf);
	FVK_MEM_Free(pucUsbFileBuf);
	close(nUsbFd);	

#endif
	
	return FAI_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void FAI_SYSTEM_MountDBandDBBACKUP(int *mounted1, int *mounted2, int *mounted3)
{
	if(*mounted1 == 0)
	{
		VK_SYSTEM_Command("mount -t ubifs ubi1:dbdata /var/lib/humaxtv");
		*mounted1 = 1;
	}
	if(*mounted2 == 0)
	{
		VK_SYSTEM_Command("mount -t ubifs ubi1:dbbackup /var/lib/humaxtv_backup");
		*mounted2 = 1;
	}
	if(*mounted3 == 0)
	{
		VK_SYSTEM_Command("mount -t ubifs ubi1:dbuser /var/lib/humaxtv_user");
		*mounted3 = 1;
	}
}

void FAI_SYSTEM_UnMountDBandDBBACKUP(void)
{
	VK_SYSTEM_Command("umount /var/lib/humaxtv");
	VK_SYSTEM_Command("umount /var/lib/humaxtv_backup");
	VK_SYSTEM_Command("umount /var/lib/humaxtv_user");
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)	
	VK_SYSTEM_Command("umount /var/lib/humaxtv_data");
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)	
	VK_SYSTEM_Command("umount /var/lib/humaxtv_ext2");
#endif
}

void FAI_SYSTEM_UBIFSInit(void)
{
//	VK_SYSTEM_Command("/usr/sbin/ubifsinit");
#if(UBIMTDNUM == 4)
	VK_SYSTEM_Command("/usr/sbin/ubiattach -m 4"); //ramfs use case ex)HDR2000T
#else
	VK_SYSTEM_Command("/usr/sbin/ubiattach -m 5");
#endif
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (DEFAULT_SETTING_LIST&DEFAULT_NAND_DB_COPY) || (DEFAULT_SETTING_LIST&DEFAULT_NAND_DB_CLEAR) 
#define LINEBUFSIZE 1000
#define DB_PATH	"/var/lib/humaxtv"
#define DB_BACKUP_PATH "/var/lib/humaxtv_backup"
#define DB_USER_PATH "/var/lib/humaxtv_user"
#define DB_SRC_PATH "/media/"
	
#define ISSLASH(_c)		('/' == (_c))
#define DOT_OR_DOTDOT(_basename)	((_basename[0] == '.') && (_basename[1] == '\0' || (_basename[1] == '.' && _basename[2] == '\0')))

static char *get_fullpath(const char *s1, const char *s2)
{
	char *fullpath, *p;
	size_t len;

	len = strlen(s1) + strlen(s2) + 2;

	if (fullpath = malloc(len), NULL == fullpath)
		return NULL;

	strncpy(fullpath, s1, strlen(s1));

	p = fullpath + strlen(s1) - 1;
	while (ISSLASH(*p))
		p--;
	*++p = '/';
	*++p = '\0';

	strcpy(p, s2);

	return fullpath;
}

static int delete_files_in_dir(const char *pathname, unsigned int depth)
{
	struct stat st;
	DIR *dp;
	struct dirent *dirent;
	char *fullpath;

	printf("delete_files_in_dir() called. path=%s\n", pathname);
	if(lstat(pathname, &st) == -1)
	{
		printf("lstat error.\n");
		return -1;
	}
	if(S_ISDIR(st.st_mode))
	{
		dp = opendir(pathname);
		if(dp == NULL)
		{
			printf("dir open error.\n");
			return -1;
		}
		while((dirent = readdir(dp)) != NULL)
		{
			if (DOT_OR_DOTDOT(dirent->d_name))
			{
				continue;
			}
			fullpath = get_fullpath(pathname, dirent->d_name);
			if (fullpath == NULL)
			{
				continue;
			}
			delete_files_in_dir(fullpath,(depth+1));
			free(fullpath);
		}
		closedir(dp);
	}

	if(depth > 0)
	{
		printf("try to remove %s\n", pathname);
		if(remove(pathname) == -1)
		{
			printf("remove error. (%s)\n", pathname);
			return -1;
		}
	}
	return 0;
}

static int copyfile(const char *src, const char *dst, const struct stat *st, int exist)
{
	char *buf, *p;
	size_t buf_size;
	int src_fd, dst_fd;
	int flags = O_WRONLY;
	unsigned long long curr_num_bytes = 0;
	ssize_t n_read, n_written;
	int rv = 0;

	if (src_fd = open(src, O_RDONLY), src_fd < 0) {
		printf("cannot open \"%s\": %s\n", src, strerror(errno));
		return -1;
	}
	flags |= exist ? O_TRUNC : O_CREAT;
	if (dst_fd = open(dst, flags, st->st_mode), dst_fd < 0) {
		printf("cannot open \"%s\": %s\n", dst, strerror(errno));
		rv = -1;
		goto close_src_fd;
	}
	buf_size = getpagesize();
	if (buf = FVK_MEM_Alloc(buf_size), NULL == buf) {
		printf("cannot alloc memory: %s\n", strerror(errno));
		rv = -1;
		goto close_dst_fd_src_fd;
	}
	for (;;)
	{
		n_read = read(src_fd, buf, buf_size);
		if (n_read < 0) {
			if (EINTR == errno)
				continue;

			printf("cannot read \"%s\": %s\n", src, strerror(errno));
			rv = -1;
			goto free_buf;
		}
		if (0 == n_read)
			break;

		curr_num_bytes += n_read;

		/* for async. but not use at this time :) */
		p = buf;
		while (n_read > 0)
		{
			n_written = write(dst_fd, p, (size_t)n_read);
			if (n_written < 0)
			{
				if (EINTR == errno)
					continue;

				printf("cannot write \"%s\": %s\n", dst, strerror(errno));
				rv = -1;
				goto free_buf;
			}

			p += n_written;
			n_read -= n_written;
		}


	}
free_buf:
	FVK_MEM_Free(buf);
close_dst_fd_src_fd:
	close(dst_fd);
close_src_fd:
	close(src_fd);
	return rv;
}

static int copydir(const char *src, const char *dst)
{
	struct stat src_st, dst_st;
	int r, ret;
	if (-1 == lstat(src, &src_st)) {
		printf("cannot stat \"%s\": %s\n", src, strerror(errno));
		return -1;
	}
	r = lstat(dst, &dst_st);
	if (-1 == r && ENOENT != errno) {
		printf("cannot stat \"%s\": %s\n", dst, strerror(errno));
		return -1;
	}
	if (-1 == r && S_ISDIR(src_st.st_mode)) {
		if (-1 == mkdir(dst, src_st.st_mode)) {
			printf("cannot mkdir \"%s\": %s\n", dst, strerror(errno));
			return -1;
		}
	}
	switch (src_st.st_mode & S_IFMT)
	{
		case S_IFDIR:
		{
			DIR *dp;
			struct dirent *dirent;
			char *nsrc, *ndst;

			if (dp = opendir(src), NULL == dp)
				return -1;

			while ((dirent = readdir(dp)) != NULL) {
				if (DOT_OR_DOTDOT(dirent->d_name))
					continue;

				if (nsrc = get_fullpath(src, dirent->d_name),
					NULL == nsrc)
					continue;

				if (ndst = get_fullpath(dst, dirent->d_name),
					NULL == ndst) {
					free(nsrc);
					continue;
				}

				copydir(nsrc, ndst);

				free(nsrc);
				free(ndst);
			}

			closedir(dp);
			break;
		}
		case S_IFREG:
			ret = copyfile(src, dst, &src_st, !r);
			if (ret < 0)
			{
					return -1;
			}
			break;
		case S_IFLNK:
		{
			int len;
			char llink[PATH_MAX];

			if (-1 == (len = readlink(src, llink, sizeof(llink) - 1))) {
				printf("cannot readlink \"%s\": %s\n", src, strerror(errno));
				return -1;
			}
			llink[len] = '\0';
			if (0 == r && -1 == remove(dst)) {
				printf("cannot remove \"%s\": %s\n", dst, strerror(errno));
				return -1;
			}
			if (-1 == symlink(llink, dst)) {
				printf("cannot symlink \"%s\": %s\n", dst, strerror(errno));
				return -1;
			}
			break;
		}
		case S_IFBLK:
		case S_IFCHR:
			if (0 == r && -1 == remove(dst)) {
				printf("cannot remove \"%s\": %s\n", dst, strerror(errno));
				return -1;
			}
			if (-1 == mknod(dst, src_st.st_mode, src_st.st_rdev)) {
				printf("cannot mknod \"%s\": %s\n", dst, strerror(errno));
				return -1;
			}
			break;
		case S_IFIFO:
			if (0 == r && -1 == remove(dst)) {
				printf("cannot remove \"%s\": %s\n", dst, strerror(errno));
				return -1;
			}
			if (-1 == mkfifo(dst, src_st.st_mode)) {
				printf("cannot mkfifo \"%s\": %s\n", dst, strerror(errno));
				return -1;
			}
			break;
		case S_IFSOCK:
			printf("%s is a socket (not copied)\n", src);
			break;
		default:
			printf("unrecognized file type \"%s\"\n", src);
			return -1;
	}
	sync();

	return 0;
}

static FAI_ERR_CODE INTRN_FAI_SYSTEM_CopyingDBFiles(void)
{
	FAI_ERR_CODE faiError = FAI_NO_ERR;
	FILE *fmount = NULL;
	char *pbuf = NULL;
	char *pstr;
	char *str1, *str2, *str3;
	unsigned char mounted1=0, mounted2=0, mounted3 = 0;
	int checkResult = 0;
	char srcPath[256] = {0,};
	
	fmount = fopen("/proc/mounts", "r");
	if(fmount == NULL)
	{
		printf("error opening file /proc/mounts\n");
		return FAI_ERR;
	}

	pbuf = FVK_MEM_Alloc(LINEBUFSIZE);
	if(pbuf == NULL)
	{
		fclose(fmount);
		printf("failed allocating buffer.\n");
		return FAI_ERR;
	}
	memset(pbuf, 0x0, LINEBUFSIZE);


	while(1)
	{
		pstr = fgets(pbuf, LINEBUFSIZE, fmount);
		if(pstr == NULL)
		{
			printf("end of file\n");
			break;
		}
		printf("%s \n", pstr);
		str1 = str2 = str3 = NULL;
		str1 = strstr(pstr, "dbdata");
		str2 = strstr(pstr, "/var/lib/humaxtv");
		str3 = strstr(pstr, "ubifs");
		if(str1 != NULL && str2 != NULL && str3 != NULL)
		{
			mounted1++;
		}
		str1 = str2 = str3 = NULL;
		str1 = strstr(pstr, "dbbackup");
		str2 = strstr(pstr, "/var/lib/humaxtv_backup");
		str3 = strstr(pstr, "ubifs");
		if(str1 != NULL && str2 != NULL && str3 != NULL)
		{
			mounted2++;
		}

		str1 = str2 = str3 = NULL;
		str1 = strstr(pstr, "dbuser");
		str2 = strstr(pstr, "/var/lib/humaxtv_user");
		str3 = strstr(pstr, "ubifs");
		if(str1 != NULL && str2 != NULL && str3 != NULL)
		{
			mounted3++;
		}
		
		memset(pbuf, 0x0, LINEBUFSIZE);
	}
	printf(" mounted1 = %d mounted2 = %d mounted3 = %d\n", mounted1, mounted2, mounted3);
	FVK_MEM_Free(pbuf);
	fclose(fmount);

	FAI_SYSTEM_MountDBandDBBACKUP(&mounted1, &mounted2,&mounted3);

	if(mounted1 == 1 && mounted2 == 1 && mounted3 == 1) //both f/s are successfully mounted.
	{
		//remove files in humaxtv
		checkResult = delete_files_in_dir(DB_PATH, 0);
		if(checkResult != 0)
			return FAI_ERR;
		//remove files in humaxtv_backup
		checkResult = delete_files_in_dir(DB_BACKUP_PATH, 0);
		if(checkResult != 0)
			return FAI_ERR;
		//remove files in humaxtv_user
		checkResult = delete_files_in_dir(DB_USER_PATH, 0);
		if(checkResult != 0)
			return FAI_ERR;

		strncpy(srcPath, DB_SRC_PATH, strlen(DB_SRC_PATH));
		strncat(srcPath, DEFAULT_SETTING_NAND_DB_DIR, strlen(DEFAULT_SETTING_NAND_DB_DIR));
		//copy db files to humaxtv & humaxtv_backup &humaxtv_user
		checkResult = copydir(srcPath,DB_PATH);
		if(checkResult != 0)
			return FAI_ERR;
		checkResult = copydir(srcPath,DB_BACKUP_PATH);
		if(checkResult != 0)
			return FAI_ERR;
		checkResult = copydir(srcPath,DB_USER_PATH);
		if(checkResult != 0)
			return FAI_ERR;
	}
	else
	{
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}

static FAI_ERR_CODE INTRN_FAI_SYSTEM_DeleteDBFiles(void)
{
	FAI_ERR_CODE faiError = FAI_NO_ERR;
	FILE *fmount = NULL;
	char *pbuf = NULL;
	char *pstr;
	char *str1, *str2, *str3;
	unsigned char mounted1=0, mounted2=0, mounted3=0;
	int checkResult = 0;
	char srcPath[256] = {0,};
	
	fmount = fopen("/proc/mounts", "r");
	if(fmount == NULL)
	{
		printf("error opening file /proc/mounts\n");
		return FAI_ERR;
	}

	pbuf = FVK_MEM_Alloc(LINEBUFSIZE);
	if(pbuf == NULL)
	{
		fclose(fmount);
		printf("failed allocating buffer.\n");
		return FAI_ERR;
	}
	memset(pbuf, 0x0, LINEBUFSIZE);


	while(1)
	{
		pstr = fgets(pbuf, LINEBUFSIZE, fmount);
		if(pstr == NULL)
		{
			printf("end of file\n");
			break;
		}
		printf("%s \n", pstr);
		str1 = str2 = str3 = NULL;
		str1 = strstr(pstr, "dbdata");
		str2 = strstr(pstr, "/var/lib/humaxtv");
		str3 = strstr(pstr, "ubifs");
		if(str1 != NULL && str2 != NULL && str3 != NULL)
		{
			mounted1++;
		}
		str1 = str2 = str3 = NULL;
		str1 = strstr(pstr, "dbbackup");
		str2 = strstr(pstr, "/var/lib/humaxtv_backup");
		str3 = strstr(pstr, "ubifs");
		if(str1 != NULL && str2 != NULL && str3 != NULL)
		{
			mounted2++;
		}
		str1 = str2 = str3 = NULL;
		str1 = strstr(pstr, "dbuser");
		str2 = strstr(pstr, "/var/lib/humaxtv_user");
		str3 = strstr(pstr, "ubifs");
		if(str1 != NULL && str2 != NULL && str3 != NULL)
		{
			mounted3++;
		}
		
		memset(pbuf, 0x0, LINEBUFSIZE);
	}
	printf(" mounted1 = %d mounted2 = %d mounted3 = %d\n", mounted1, mounted2, mounted3);
	FVK_MEM_Free(pbuf);
	fclose(fmount);

	FAI_SYSTEM_MountDBandDBBACKUP(&mounted1, &mounted2, &mounted3);

	if(mounted1 == 1 && mounted2 == 1 && mounted3 == 1) //both f/s are successfully mounted.
	{
		//remove files in humaxtv
		checkResult = delete_files_in_dir(DB_PATH, 0);
		if(checkResult != 0)
			return FAI_ERR;
		//remove files in humaxtv_backup
		checkResult = delete_files_in_dir(DB_BACKUP_PATH, 0);
		if(checkResult != 0)
			return FAI_ERR;
		//remove files in humaxtv_user
		checkResult = delete_files_in_dir(DB_USER_PATH, 0);
		if(checkResult != 0)
			return FAI_ERR;
	}
	else
	{
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}
#endif

