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
#if defined(CONFIG_BCM_DOT_CONFIG)
#include <sys/mount.h>
#endif
#include <sys/times.h>
#include <string.h>

/* chip manufacture's  header files */

/* humax header files */
#include "htype.h"
#include "di_err.h"
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"
#include "fai_util.h"
#include "fai_flash.h"
#include "fai_system.h"
#include "fdi_flash.h"
#include "fdi_panel.h"
#include "fdi_ethernet.h"
#include "di_nvram.h"

/* model specific configuration header */
#include "factorysw_config.h"
#if defined(CONFIG_BCM_DOT_CONFIG)
#include "flash_map.h"
#endif

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_FLASH_TEST_RETRY_COUNT 10
#define FAI_FLASH_TEST_RETRY_SLEEP 1000

#define USB_FILE_PATH "/media/"

typedef enum
{
	FAI_FLASH_TMD_DATA_COMPARE,
	FAI_FLASH_TMD_FLASH_CRC,
	FAI_FLASH_TMD_NAND_DOWNLOAD,
	FAI_FLASH_TMD_FLASH_PATTERN,
	FAI_FLASH_TMD_BAD_BLOCK,
	FAI_FLASH_UI_DATA_COMPARE,
	FAI_FLASH_UI_FLASH_CRC,
	FAI_FLASH_UI_NAND_DOWNLOAD
} FAI_FLASH_TEST_TYPE;

typedef enum
{
	FAI_FLASH_NAND,
	FAI_FLASH_NOR,
	FAI_FLASH_UNKNOWN
} FAI_FLASH_TYPE_e;

typedef struct FAI_FLASH_CMD_DATACOMPARE
{
    char acFIleName[80];
} FLASH_CMD_DATACOMPARE_t;

typedef struct FAI_FLASH_CRC_CHECK
{
	unsigned char ucImageType;
} FLASH_CMD_CRC_CHECK_t;

typedef struct FAI_FLASH_CMD_ERASE_CHECK
{
    FAI_FLASH_TYPE_e FlashType;
    int nAddress;
    int nSize;
} FLASH_CMD_ERASE_CHECK_t;

typedef struct FAI_FLASH_CMD_PATTERN_TEST
{
    int nAddress;
    int nData;
} FLASH_CMD_PATTERN_TEST_t;

typedef struct FAI_FLASH_MSG
{
	int nCmd;
	FAI_TESTMENU_H hMenu;
    union
	{
		FLASH_CMD_DATACOMPARE_t	DataCompare;
		FLASH_CMD_CRC_CHECK_t	CrcCheck;
		FLASH_CMD_ERASE_CHECK_t EraseCheck;
		FLASH_CMD_PATTERN_TEST_t PatternTest;
	} cmd_arg;
} FAI_FLASH_MSG_t;

#if (USE_EMMC_FLASH==YES)
#define CRC_CHK_BUFF_SIZE (10*1024*1024)
typedef struct 
{
	unsigned int address;
	unsigned int size;
}part_info;

typedef struct
{
	unsigned int num_part;
	part_info part[2];
}crc_check_st;
#endif			

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */
extern const HUINT32		s_betaTable[];
extern const HUINT32 s_sizeBetaTable;

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
unsigned long g_ulFlashMsgId, g_ulFlashTaskId;

static unsigned char g_aucBlockBuf[HDF_DEFAULT_DATA_BLOCK_SIZE+MAX_HDF_DATA_BLOCK_SIZE];
static unsigned char g_aucFlashDataBlockBuf[HDF_DEFAULT_DATA_BLOCK_SIZE+MAX_HDF_DATA_BLOCK_SIZE];

FAI_ERR_CODE INTRN_FAI_FLASH_DataCompare(char *pcFileName, FAI_FLASH_TMD_ERR *pFlashError);
FAI_ERR_CODE INTRN_FAI_FLASH_CrcCheck(unsigned char ucFlashType,unsigned long ulAddr,unsigned long ulSize,int *pnCrc, FAI_FLASH_TMD_ERR *pFlashError);
FAI_ERR_CODE INTRN_FAI_FLASH_EraseCheck(FAI_FLASH_TYPE_e FlashType, int nAddress, int nSize, FAI_FLASH_TMD_ERR *pFlashError);
FAI_ERR_CODE INTRN_FAI_FLASH_PatternTest(int *pnAddress, int *pnData, FAI_FLASH_TMD_ERR *pFlashError);
FAI_ERR_CODE INTRN_FAI_FLASH_NandDownload(char *pcFileName, FAI_FLASH_TMD_ERR *pFlashError);
#if (EMMC_DOWNLOAD_ENABLE==YES)
FAI_ERR_CODE INTRN_FAI_FLASH_eMMCDownload(char *pcFileName, FAI_FLASH_TMD_ERR *pFlashError);
#endif
FAI_ERR_CODE INTRN_FAI_FLASH_NandBadBlockCheck( int *piBadStatus,FAI_FLASH_TMD_ERR *pFlashError);
void INTRN_FAI_FLASH_Task(void *pData);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FLASH_Init()
{
	FVK_ERR_CODE eFvkErr;

	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_FLASH_MSG_t), (char*)"FlsMsg", &g_ulFlashMsgId);
	if( eFvkErr	!= FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Init] : FVK_MSG_Create Error \n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Create(INTRN_FAI_FLASH_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "fls Task", NULL, &g_ulFlashTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Start(g_ulFlashTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
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
#if (FLASH_DATA_COMPARE_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_DataCompare(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	char szFileName[80], szRealFileName[80];
	unsigned char ucFileNameSize;
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_DataCompare] : Argument Count Error=%d\n", ucArgc));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 1;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	ucFileNameSize = FAI_TMD_GetParam(szFileName, pucArgList, 0);
	szFileName[ucFileNameSize] = '\0';

    tFlashMsg.nCmd = FAI_FLASH_TMD_DATA_COMPARE;   /* 0x01 means DataCompareTest */
	strcpy(tFlashMsg.cmd_arg.DataCompare.acFIleName, szFileName);
	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Check_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 1;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
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
#if (EMMC_DOWNLOAD_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_eMMCDownload(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	char szFileName[80], szRealFileName[80];
	unsigned char ucFileNameSize;
	
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;

	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_eMMCDownload] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 5;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	ucFileNameSize = FAI_TMD_GetParam(szFileName, pucArgList, 0);
	szFileName[ucFileNameSize] = '\0';

	tFlashMsg.nCmd = FAI_FLASH_TMD_NAND_DOWNLOAD;
	
	strcpy(tFlashMsg.cmd_arg.DataCompare.acFIleName, szFileName);

	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_eMMCDownload] : Error FVK_MSG_Send() : %d\n", eFvkErr));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 6;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
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

#if (NAND_DOWNLOAD_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_NandDownload(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	char szFileName[80], szRealFileName[80];
	unsigned char ucFileNameSize;
	
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;

	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_NandDownload] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 5;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	ucFileNameSize = FAI_TMD_GetParam(szFileName, pucArgList, 0);
	szFileName[ucFileNameSize] = '\0';

	tFlashMsg.nCmd = FAI_FLASH_TMD_NAND_DOWNLOAD;
	
	strcpy(tFlashMsg.cmd_arg.DataCompare.acFIleName, szFileName);

	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Check_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 6;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}
#endif	
#if (NAND_BAD_BLOCK_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_BadBlock(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	char szFileName[80], szRealFileName[80];
	unsigned char ucFileNameSize;	
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_NandDownload] : Argument Count Error=%d\n", ucArgc));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 6;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
	tFlashMsg.nCmd = FAI_FLASH_TMD_BAD_BLOCK;
	strcpy(tFlashMsg.cmd_arg.DataCompare.acFIleName, szFileName);
	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Check_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 6;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
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
#if (FLASH_CRC_CHECK_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_CrcCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char unImageType;
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
 
	if( ucArgc != 1 )
	{
	 FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",	__func__, ucArgc));
	 
	 tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	 ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
	 ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
	 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	 FAI_TMD_SetResult(&tTMDResult);
	 return FAI_ERR;
	}
	/* get params */
	FAI_TMD_GetParam(&unImageType, pucArgList, 0);
	tFlashMsg.cmd_arg.CrcCheck.ucImageType = unImageType;

	tFlashMsg.nCmd = FAI_FLASH_TMD_FLASH_CRC;   /* 0x03 means CRC Check Test */
	
	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Check_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 2;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);

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
#if (FLASH_PATTERN_TEST_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_PatternTest(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 3 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_CrcCheck] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 3;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

    tFlashMsg.nCmd = FAI_FLASH_TMD_FLASH_PATTERN;   /* 0x07 means Pattern Test */

	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Check_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 4;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
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
#if (FLASH_CHECK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_DataCompare_UI(void)
{
	char szFileName[80];
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Flash Compare", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	tFlashMsg.nCmd = FAI_FLASH_UI_DATA_COMPARE;
	FAI_TESTMENU_GetCurrentMenu(&tFlashMsg.hMenu);

	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Check_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));
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
#if (FLASH_CRC_UI_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_FlashCRC_UI(void)
{
	char szFileName[80];
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Flash CRC", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	tFlashMsg.nCmd = FAI_FLASH_UI_FLASH_CRC;
	FAI_TESTMENU_GetCurrentMenu(&tFlashMsg.hMenu);

	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_Check_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));
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
#if (EMMC_DOWNLOAD_UI_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_eMMCDownload_UI(void)
{
	char szFileName[80];
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "eMMC SW", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Download", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	tFlashMsg.nCmd = FAI_FLASH_UI_NAND_DOWNLOAD;
	FAI_TESTMENU_GetCurrentMenu(&tFlashMsg.hMenu);

	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_NandDownload_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));
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
#if (NAND_DOWNLOAD_UI_ENABLE==YES)
FAI_ERR_CODE FAI_FLASH_NandDownload_UI(void)
{
	char szFileName[80];
	FVK_ERR_CODE eFvkErr;
	FAI_FLASH_MSG_t tFlashMsg;

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "NAND SW", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Download", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	tFlashMsg.nCmd = FAI_FLASH_UI_NAND_DOWNLOAD;
	FAI_TESTMENU_GetCurrentMenu(&tFlashMsg.hMenu);

	eFvkErr = FVK_MSG_Send(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FLASH_NandDownload_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));
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
void INTRN_FAI_FLASH_Task(void *pData)
{
#if defined(CONFIG_BCM_DOT_CONFIG)
	FAI_FLASH_MSG_t tFlashMsg;
	FVK_ERR_CODE eFvkErr;
	FAI_ERR_CODE eFaiErr = FAI_NO_ERR;
	unsigned char aucTmdResult[5];
   	FAI_TMD_RESULT_T tTMDResult;
	unsigned char aucAgcResult[4];
	unsigned char ucTmdErrCode[2];
	char szFileName[80];
	FAI_FLASH_TMD_ERR stFlashError = FAI_FLASH_TMD_ERR_NO;
	unsigned char ucFlashType = FLASH_TYPE_NONE;
	unsigned long ulAddr = 0;
	unsigned long ulSize = 0;
	unsigned char *compare_buf = NULL;
	unsigned int calcCrc;
#if (USE_EMMC_FLASH==YES)
	crc_check_st stCrcChk;
	unsigned int currentAddr;
	unsigned int remained;
	unsigned int readThisTime;
	unsigned int inloopcnt, outloopcnt, inloopmax;
#endif
	
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_FLASH;

	while(1)
	{
		eFvkErr = FVK_MSG_Receive(g_ulFlashMsgId, &tFlashMsg, sizeof(FAI_FLASH_MSG_t));
		if (eFvkErr != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_FLASH_Task] : Error FVK_MSG_Receive() %d\n", eFvkErr));
			continue;
		}

        FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

        if( tFlashMsg.nCmd == FAI_FLASH_TMD_DATA_COMPARE )     /* 0x01 means DATA COMPARE command */
		{
            char szFileName[80];

			sprintf(szFileName, "%s%s", USB_FILE_PATH, tFlashMsg.cmd_arg.DataCompare.acFIleName);
			if( INTRN_FAI_FLASH_DataCompare(szFileName, &stFlashError) != FAI_NO_ERR )
			{
			     /* Fail 인 경우 */
                 tTMDResult.ucCategory = 0x08;
                 tTMDResult.ucModule = 0x01;
                 tTMDResult.ucAction = 0X01;
                 tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				ucTmdErrCode[1] = (unsigned char)stFlashError;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

                 FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
			     /* Pass 인 경우 */
                tTMDResult.ucCategory = 0x08;
                tTMDResult.ucModule = 0x01;
                tTMDResult.ucAction = 0X01;
                tTMDResult.eResult = FAI_TMD_ERR_OK;
                tTMDResult.ucNumOfParam = 0;

				stFlashError = FAI_FLASH_TMD_ERR_NO;
                FAI_TMD_SetResult(&tTMDResult);
			}
		}
        else if ( tFlashMsg.nCmd == FAI_FLASH_TMD_FLASH_CRC)     /* 0x02 means CRC Check command */
		{
			int nCrc =0x00;

			switch(tFlashMsg.cmd_arg.CrcCheck.ucImageType)
			{
				case FAI_FLASH_IMAGE_PRELOADER:
					ucFlashType = FLASH_CRC_CHECK_PRELOADER_TYPE;
					ulAddr = FLASH_CRC_CHECK_PRELOADER_ADDR;
					ulSize = FLASH_CRC_CHECK_PRELOADER_SIZE;
					break;
				case FAI_FLASH_IMAGE_LOADER:
					ucFlashType = FLASH_CRC_CHECK_LOADER_TYPE;
					ulAddr = FLASH_CRC_CHECK_LOADER_ADDR;
					ulSize = FLASH_CRC_CHECK_LOADER_SIZE;
					break;
				case FAI_FLASH_IMAGE_APPLICATION:
					ucFlashType = FLASH_CRC_CHECK_APP_TYPE;
					ulAddr = FLASH_CRC_CHECK_APP_ADDR;
					ulSize = FLASH_CRC_CHECK_APP_SIZE;
					break;
				case FAI_FLASH_IMAGE_HUMAXTV:
					ucFlashType = FLASH_CRC_CHECK_HUMAXTV_TYPE;
					ulAddr = FLASH_CRC_CHECK_HUMAXTV_ADDR;
					ulSize = FLASH_CRC_CHECK_HUMAXTV_SIZE;
					break;
				case FAI_FLASH_IMAGE_ROOTFS:
					ucFlashType = FLASH_CRC_CHECK_ROOTFS_TYPE;
					ulAddr = FLASH_CRC_CHECK_ROOTFS_ADDR;
					ulSize = FLASH_CRC_CHECK_ROOTFS_SIZE;
					break;
				case FAI_FLASH_IMAGE_LINUX:
					ucFlashType = FLASH_CRC_CHECK_LINUX_TYPE;
					ulAddr = FLASH_CRC_CHECK_LINUX_ADDR;
					ulSize = FLASH_CRC_CHECK_LINUX_SIZE;
					break;
				case FAI_FLASH_IMAGE_SHARE:
					ucFlashType = FLASH_CRC_CHECK_SHARE_TYPE;
					ulAddr = FLASH_CRC_CHECK_SHARE_ADDR;
					ulSize = FLASH_CRC_CHECK_SHARE_SIZE;
					break;
				case FAI_FLASH_IMAGE_PRELOADER2:
					ucFlashType = FLASH_CRC_CHECK_PRELOADER2_TYPE;
					ulAddr = FLASH_CRC_CHECK_PRELOADER2_ADDR;
					ulSize = FLASH_CRC_CHECK_PRELOADER2_SIZE;
					break;
				case FAI_FLASH_IMAGE_LOADER2:
					ucFlashType = FLASH_CRC_CHECK_LOADER2_TYPE;
					ulAddr = FLASH_CRC_CHECK_LOADER2_ADDR;
					ulSize = FLASH_CRC_CHECK_LOADER2_SIZE;
					break;
				case FAI_FLASH_IMAGE_APPLICATION2:
					ucFlashType = FLASH_CRC_CHECK_APP2_TYPE;
					ulAddr = FLASH_CRC_CHECK_APP2_ADDR;
					ulSize = FLASH_CRC_CHECK_APP2_SIZE;
					break;
				case FAI_FLASH_IMAGE_HUMAXTV2:
					ucFlashType = FLASH_CRC_CHECK_HUMAXTV2_TYPE;
					ulAddr = FLASH_CRC_CHECK_HUMAXTV2_ADDR;
					ulSize = FLASH_CRC_CHECK_HUMAXTV2_SIZE;
					break;
				case FAI_FLASH_IMAGE_ROOTFS2:
					ucFlashType = FLASH_CRC_CHECK_ROOTFS2_TYPE;
					ulAddr = FLASH_CRC_CHECK_ROOTFS2_ADDR;
					ulSize = FLASH_CRC_CHECK_ROOTFS2_SIZE;
					break;
				case FAI_FLASH_IMAGE_LINUX2:
					ucFlashType = FLASH_CRC_CHECK_LINUX2_TYPE;
					ulAddr = FLASH_CRC_CHECK_LINUX2_ADDR;
					ulSize = FLASH_CRC_CHECK_LINUX2_SIZE;
					break;
				case FAI_FLASH_IMAGE_SHARE2:
					ucFlashType = FLASH_CRC_CHECK_SHARE2_TYPE;
					ulAddr = FLASH_CRC_CHECK_SHARE2_ADDR;
					ulSize = FLASH_CRC_CHECK_SHARE2_SIZE;
					break;
				default:
					/* Fail 인 경우 */
	                 tTMDResult.ucCategory = 0x08;
	                 tTMDResult.ucModule = 0x01;
	                 tTMDResult.ucAction = 0x02;
	                 tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					 
					 ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
					 ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
					 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

	                 FAI_TMD_SetResult(&tTMDResult);
					 continue;
			}

			if(ucFlashType == FLASH_TYPE_NONE)
			{
				/* Fail 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x01;
				tTMDResult.ucAction = 0x02;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_NOT_SUPPORTED_FEATURE;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

				FAI_TMD_SetResult(&tTMDResult);
				continue;

			}
			//eFaiErr = INTRN_FAI_FLASH_CrcCheck(ucFlashType,ulAddr,ulSize,&nCrc, &stFlashError);

#if (USE_NAND_FLASH==YES)
			compare_buf = FVK_MEM_Alloc(N_UBI_ROOT_FS_SIZE + N_UBI_KERNEL_SIZE);
			eFaiErr = FDI_NANDFLASH_ReadForCRC(FLASH_UBI_AREA_START, compare_buf, N_UBI_ROOT_FS_SIZE + N_UBI_KERNEL_SIZE);
			if( eFaiErr != FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_Task] : FDI_NANDFLASH_ReadForCRC Error!!\n"));
				FVK_MEM_Free(compare_buf);				
			}

			calcCrc = FAI_UTIL_GetCRC32Fast(compare_buf, N_UBI_ROOT_FS_SIZE + N_UBI_KERNEL_SIZE);
            if(eFaiErr != FAI_NO_ERR)
			{
                 /* Fail 인 경우 */
                 tTMDResult.ucCategory = 0x08;
                 tTMDResult.ucModule = 0x01;
                 tTMDResult.ucAction = 0x02;
                 tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				 ucTmdErrCode[1] = (unsigned char)stFlashError;
				 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

                 FAI_TMD_SetResult(&tTMDResult);
				 continue;
            }
#elif (USE_EMMC_FLASH==YES)
			stCrcChk.num_part = 2;
			stCrcChk.part[0].address = 0x30000000;
			stCrcChk.part[0].size = 64*1024*1024;
			stCrcChk.part[1].address = 0x40000000;
			stCrcChk.part[1].size = 320*1024*1024;
			compare_buf = FVK_MEM_Alloc(CRC_CHK_BUFF_SIZE); //allocate 10MB buffer
			calcCrc = 0xFFFFFFFF;
			for(outloopcnt = 0; outloopcnt < stCrcChk.num_part; outloopcnt++)
			{
				inloopmax = stCrcChk.part[outloopcnt].size/CRC_CHK_BUFF_SIZE;
				if(stCrcChk.part[outloopcnt].size%CRC_CHK_BUFF_SIZE) inloopmax++;
				currentAddr = stCrcChk.part[outloopcnt].address;
				remained = stCrcChk.part[outloopcnt].size;
				for(inloopcnt=0; inloopcnt < inloopmax; inloopcnt++)
				{
					readThisTime = (remained>=CRC_CHK_BUFF_SIZE)?CRC_CHK_BUFF_SIZE:remained;
					FDI_EMMC_Read(currentAddr, compare_buf, readThisTime);
					calcCrc=FAI_UTIL_GetCRC32Fast_Partial(calcCrc, compare_buf,readThisTime);
					currentAddr+= CRC_CHK_BUFF_SIZE;
					remained-=readThisTime;
					
				}
			}
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_Task] CRC_calc = 0x%08x\n", calcCrc));
			FVK_MEM_Free(compare_buf);
#endif			
             /* Pass 인 경우 */
            tTMDResult.ucCategory = 0x08;
            tTMDResult.ucModule = 0x01;
            tTMDResult.ucAction = 0x02;
            tTMDResult.eResult = FAI_TMD_ERR_OK;

			stFlashError = FAI_FLASH_TMD_ERR_NO;

			aucAgcResult[0] = (calcCrc&0x000000FF);
			aucAgcResult[1] = (calcCrc&0x0000FF00)>>8;
			aucAgcResult[2] = (calcCrc&0x00FF0000)>>16;
			aucAgcResult[3] = (calcCrc&0xFF000000)>>24;
			FAI_TMD_AddResultParam(&tTMDResult, aucAgcResult, sizeof(aucAgcResult));

            FAI_TMD_SetResult(&tTMDResult);
        }
        else if ( tFlashMsg.nCmd == FAI_FLASH_TMD_FLASH_PATTERN)     /* 0x04 means Pattern Test command */
		{
            int nAddress =0x00;
            int nData =0x00;

            if(INTRN_FAI_FLASH_PatternTest(&nAddress,&nData, &stFlashError) != FAI_NO_ERR)
			{
                 /* Fail 인 경우 */
                 tTMDResult.ucCategory = 0x08;
                 tTMDResult.ucModule = 0x01;
                 tTMDResult.ucAction = 0x03;
                 tTMDResult.eResult = FAI_TMD_ERR_FAIL;
                 FAI_TMD_AddResultParam(&tTMDResult, &nAddress, sizeof(nAddress));
                 FAI_TMD_AddResultParam(&tTMDResult, &nData, sizeof(nData));

                 FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
                 /* Pass 인 경우 */
                tTMDResult.ucCategory = 0x08;
                tTMDResult.ucModule = 0x01;
                tTMDResult.ucAction = 0x03;
                tTMDResult.eResult = FAI_TMD_ERR_OK;
                nAddress =0x00;
                nData =0x00;

				stFlashError = FAI_FLASH_TMD_ERR_NO;

                FAI_TMD_AddResultParam(&tTMDResult, &nAddress, sizeof(nAddress));
                FAI_TMD_AddResultParam(&tTMDResult, &nData, sizeof(nData));

                FAI_TMD_SetResult(&tTMDResult);
			}
		}
		else if ( tFlashMsg.nCmd == FAI_FLASH_UI_DATA_COMPARE)
		{
			sprintf(szFileName, "%s%s", USB_FILE_PATH, FLASH_CHECK_UI_FILENAME);
			if( INTRN_FAI_FLASH_DataCompare(szFileName, &stFlashError) != FAI_NO_ERR )
			{
				FAI_TESTMENU_ChangeMenuColor(tFlashMsg.hMenu, RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_2, "Flash Compare", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
				FDI_PANEL_Display(FAI_FLASH_CHECK_FAIL_STR);
			}
			else
			{
				FAI_TESTMENU_ChangeMenuColor(tFlashMsg.hMenu, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_2, "Flash Compare", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
				FDI_PANEL_Display(FAI_FLASH_CHECK_PASS_STR);
			}

		}
#if (PRODUCT_YSR2000==YES)
		else if ( tFlashMsg.nCmd == FAI_FLASH_UI_FLASH_CRC)
		{
            int nCrc =0x00;
			char szCheckStr[20];

#if (USE_EMMC_FLASH==YES)
			stCrcChk.num_part = 2;
			stCrcChk.part[0].address = 0x30000000;
			stCrcChk.part[0].size = 64*1024*1024;
			stCrcChk.part[1].address = 0x40000000;
			stCrcChk.part[1].size = 320*1024*1024;
			compare_buf = FVK_MEM_Alloc(CRC_CHK_BUFF_SIZE); //allocate 10MB buffer
			calcCrc = 0xFFFFFFFF;
			for(outloopcnt = 0; outloopcnt < stCrcChk.num_part; outloopcnt++)
			{
				inloopmax = stCrcChk.part[outloopcnt].size/CRC_CHK_BUFF_SIZE;
				if(stCrcChk.part[outloopcnt].size%CRC_CHK_BUFF_SIZE) inloopmax++;
				currentAddr = stCrcChk.part[outloopcnt].address;
				remained = stCrcChk.part[outloopcnt].size;
				for(inloopcnt=0; inloopcnt < inloopmax; inloopcnt++)
				{
					readThisTime = (remained>=CRC_CHK_BUFF_SIZE)?CRC_CHK_BUFF_SIZE:remained;
					FDI_EMMC_Read(currentAddr, compare_buf, readThisTime);
					calcCrc=FAI_UTIL_GetCRC32Fast_Partial(calcCrc, compare_buf,readThisTime);
					currentAddr+= CRC_CHK_BUFF_SIZE;
					remained-=readThisTime;
					
				}
			}
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_Task] CRC_calc = 0x%08x\n", calcCrc));
#else

			compare_buf = FVK_MEM_Alloc(N_PART_GP0_SIZE);
			eFaiErr = FDI_NANDFLASH_ReadForCRC(N_PART_GP0_OFFSET, compare_buf, N_PART_GP0_SIZE);
			if( eFaiErr != FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_Task] : FDI_NANDFLASH_ReadForCRC Error!!\n"));
				FVK_MEM_Free(compare_buf);				
			}
			calcCrc = FAI_UTIL_GetCRC32Fast(compare_buf, N_PART_GP0_SIZE);					
#endif
			calcCrc = ((calcCrc>>24)&0xff) | ((calcCrc<<8)&0xff0000) | ((calcCrc>>8)&0xff00) | ((calcCrc<<24)&0xff000000); // Endian SWAP.
			if(eFaiErr != FAI_NO_ERR)
			{
				FAI_TESTMENU_ChangeMenuColor(tFlashMsg.hMenu, RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_2, "Flash CRC", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
				FDI_PANEL_Display(FAI_FLASH_CHECK_FAIL_STR);
			}
			else
			{
				sprintf(szCheckStr, "(0x%X)", calcCrc);
				FAI_TESTMENU_ChangeMenuColor(tFlashMsg.hMenu, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_2, "Flash CRC", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_3, szCheckStr, RGB_COLOR_BLACK);
				FDI_PANEL_Display(FAI_FLASH_CHECK_PASS_STR);
			}
			FVK_MEM_Free(compare_buf);


		}
#endif
#if (NAND_DOWNLOAD_ENABLE==YES)
		else if ( tFlashMsg.nCmd == FAI_FLASH_UI_NAND_DOWNLOAD)
		{
			sprintf(szFileName, "%s%s", USB_FILE_PATH, NAND_DOWNLOAD_UI_FILENAME);
			if( INTRN_FAI_FLASH_NandDownload(szFileName, &stFlashError) != FAI_NO_ERR )
			{
				FAI_TESTMENU_ChangeMenuColor(tFlashMsg.hMenu, RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_2, "NAND SW", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_3, "Download", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_4, "FAIL", RGB_COLOR_RED);
				FDI_PANEL_Display(FAI_FLASH_CHECK_FAIL_STR);
			}
			else
			{
				FAI_TESTMENU_ChangeMenuColor(tFlashMsg.hMenu, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_2, "NAND SW", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_3, "Download", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_4, "PASS", RGB_COLOR_BLACK);
				FDI_PANEL_Display(FAI_FLASH_CHECK_PASS_STR);
			}

		}
		else if(tFlashMsg.nCmd == FAI_FLASH_TMD_NAND_DOWNLOAD)
		{
			char szFileName[80];
			sprintf(szFileName, "%s%s", USB_FILE_PATH, tFlashMsg.cmd_arg.DataCompare.acFIleName);
			FAI_PRINT(FAI_PRT_DBGINFO, ("Nand download file : %s\n", szFileName));

			if( INTRN_FAI_FLASH_NandDownload(szFileName, &stFlashError) != FAI_NO_ERR )
			{
                 /* Fail 인 경우 */
                 tTMDResult.ucCategory = 0x08;
                 tTMDResult.ucModule = 0x01;
                 tTMDResult.ucAction = 0x05;
                 tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				 ucTmdErrCode[1] = (unsigned char)stFlashError;
				 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

                 FAI_TMD_SetResult(&tTMDResult);
				 continue;
            }
             /* Pass 인 경우 */
            tTMDResult.ucCategory = 0x08;
            tTMDResult.ucModule = 0x01;
            tTMDResult.ucAction = 0x05;
            tTMDResult.eResult = FAI_TMD_ERR_OK;
			tTMDResult.ucNumOfParam = 0;
			
			stFlashError = FAI_FLASH_TMD_ERR_NO;

			FAI_TMD_SetResult(&tTMDResult);
	
		}
#elif (EMMC_DOWNLOAD_ENABLE==YES)
		else if ( tFlashMsg.nCmd == FAI_FLASH_UI_NAND_DOWNLOAD)
		{
			sprintf(szFileName, "%s%s", USB_FILE_PATH, EMMC_DOWNLOAD_UI_FILENAME);
			if( INTRN_FAI_FLASH_eMMCDownload(szFileName, &stFlashError) != FAI_NO_ERR )
			{
				FAI_TESTMENU_ChangeMenuColor(tFlashMsg.hMenu, RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_2, "eMMC SW", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_3, "Download", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_4, "FAIL", RGB_COLOR_RED);
				FDI_PANEL_Display(FAI_FLASH_CHECK_FAIL_STR);
			}
			else
			{
				FAI_TESTMENU_ChangeMenuColor(tFlashMsg.hMenu, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_2, "eMMC SW", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_3, "Download", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tFlashMsg.hMenu, TESTMENU_LINE_4, "PASS", RGB_COLOR_BLACK);
				FDI_PANEL_Display(FAI_FLASH_CHECK_PASS_STR);
			}

		}
		else if(tFlashMsg.nCmd == FAI_FLASH_TMD_NAND_DOWNLOAD)
		{
			char szFileName[80];
			sprintf(szFileName, "%s%s", USB_FILE_PATH, tFlashMsg.cmd_arg.DataCompare.acFIleName);
			FAI_PRINT(FAI_PRT_ERROR, ("eMMC download file : %s\n", szFileName));

			if( INTRN_FAI_FLASH_eMMCDownload(szFileName, &stFlashError) != FAI_NO_ERR )
			{
				/* Fail 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x01;
				tTMDResult.ucAction = 0x05;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				ucTmdErrCode[1] = (unsigned char)stFlashError;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

				FAI_TMD_SetResult(&tTMDResult);
				continue;
            }
		/* Pass 인 경우 */
		tTMDResult.ucCategory = 0x08;
		tTMDResult.ucModule = 0x01;
		tTMDResult.ucAction = 0x05;
		tTMDResult.eResult = FAI_TMD_ERR_OK;
		tTMDResult.ucNumOfParam = 0;

		stFlashError = FAI_FLASH_TMD_ERR_NO;

		FAI_TMD_SetResult(&tTMDResult);

		}
#else
#error "must be define configuraton. NAND_DOWNLOAD_ENABLE or EMMC_DOWNLOAD_ENABLE"
#endif

#if (NAND_BAD_BLOCK_ENABLE==YES)		
		else if(tFlashMsg.nCmd == FAI_FLASH_TMD_BAD_BLOCK)
		{
			int nCntBadBlock = 0;
			unsigned char aucResult[4];
			if( INTRN_FAI_FLASH_NandBadBlockCheck(&nCntBadBlock,&stFlashError) != FAI_NO_ERR )
			{
				/* Fail 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x01;
				tTMDResult.ucAction = 0x06;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[1] = (unsigned char)stFlashError;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
				FAI_TMD_SetResult(&tTMDResult);
				continue;
			}
			/* Pass 인 경우 */
			tTMDResult.ucCategory = 0x08;
			tTMDResult.ucModule = 0x01;
			tTMDResult.ucAction = 0x06;
			tTMDResult.eResult = FAI_TMD_ERR_OK;
			aucResult[0] = (nCntBadBlock&0x0000FF00)>>8;
			aucResult[1] = (nCntBadBlock&0x000000FF);	
			FAI_TMD_AddResultParam(&tTMDResult, aucResult, 2);
			stFlashError = FAI_FLASH_TMD_ERR_NO;
			FAI_TMD_SetResult(&tTMDResult);
		}
#endif		
	}
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
FAI_ERR_CODE INTRN_FAI_FLASH_DataCompare(char *pcFileName, FAI_FLASH_TMD_ERR *pFlashError)
{
#if defined(CONFIG_BCM_DOT_CONFIG)

#if 0/* CONFIG_NAND_COMPARE_CRC */
	int nUsbFd, nUsbFileSize, nBlockIdx, nDataIdx, nReadSize, nUsbFilePos = 0;
	unsigned char ucDataType;
	unsigned char *pucUsbFileBuf;
	HDF_HEADER_BLOCK_T tHdfHeaderBlock;
	HDF_DATA_BLOCK_T tHdfDataBlock;
	int i;
	bool bNandRead = FALSE;
	unsigned long ulCompareCount = 0;
#else
	unsigned int calcCrc;
	unsigned char *compare_buf = NULL;
	unsigned int storedCrc;
#endif

	if( pcFileName == NULL )
	{
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_INVALID;

		return FAI_ERR;
	}
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : File Name : %s\n", pcFileName));

	FAI_SYSTEM_UnMountDBandDBBACKUP();
#if 0/* ! CONFIG_NAND_COMPARE_CRC */
	/* open usb file : wait for mount */
	for( i = 0; i < FAI_FLASH_TEST_RETRY_COUNT; i++ )
	{
	 	nUsbFd = open(pcFileName, O_RDONLY | O_NONBLOCK);
		if( nUsbFd >= 0 )
		{
			break;
		}
		FVK_TASK_Sleep(FAI_FLASH_TEST_RETRY_SLEEP);
	}

	if( nUsbFd < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : open Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_OPEN_FAIL;
		goto TEST_FAIL;
	}
#endif
	/* sync & buffer cache clear */
	if( FDI_NANDFLASH_Sync() != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : FDI_NANDFLASH_Sync Error!!\n"));
		goto TEST_FAIL;
	}
#if 0/* ! CONFIG_NAND_COMPARE_CRC */
	/* get file size */
	nUsbFileSize=lseek(nUsbFd, 0, SEEK_END);
	if( nUsbFileSize < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : lseek Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_SEEK_FAIL;
		goto TEST_FAIL;
	}

	/* alloc memory for file buffer */
	pucUsbFileBuf = FVK_MEM_Alloc(nUsbFileSize);
	if( pucUsbFileBuf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : FVK_MEM_Alloc Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}

	/* move to beginning of file */
	if( lseek(nUsbFd, 0, SEEK_SET) != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : lseek Error!!\n"));
		FVK_MEM_Free(pucUsbFileBuf);
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_SEEK_FAIL;
		goto TEST_FAIL;
	}

	/* read file */
	nReadSize = read(nUsbFd, pucUsbFileBuf, nUsbFileSize);
	if( nReadSize != nUsbFileSize )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : read Error!!\n"));
		FVK_MEM_Free(pucUsbFileBuf);
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_READ_FAIL;
		goto TEST_FAIL;
	}

	/* get hdf header block */
	FAI_UTIL_GetHdfHeaderBlock(pucUsbFileBuf, &tHdfHeaderBlock);
	tHdfDataBlock.aucData = g_aucBlockBuf;
	nUsbFilePos += HDF_HEADER_BLOCK_SIZE;

	for( nBlockIdx = 0; nBlockIdx < tHdfHeaderBlock.usNumOfDataBlock; nBlockIdx++ )
	{
		/* Get hdf data block */
		FAI_UTIL_GetHdfDataBlock(pucUsbFileBuf+nUsbFilePos, &tHdfDataBlock);
		nUsbFilePos += (tHdfDataBlock.usSize+2);

		/*************************
		*	Nand Check
		*************************/
		if((tHdfDataBlock.ulDataAddr&0x80000000) != 0x0)
		{
			bNandRead = TRUE;
			FAI_PRINT(FAI_PRT_DBGINFO, ("DataAddr indicates NAND flash memory!\n"));
		}
		else
		{
			bNandRead = FALSE;
		}

		ucDataType = tHdfDataBlock.usFlag&HDF_DATA_TYPE_MASK;

		if( ucDataType == HDF_DATA_TYPE_SIGNATURE )
		{
			continue;
		}

		/* Compare only loader, application area : loader must be flash type */
		if( ((tHdfDataBlock.ulDataAddr < (N_UBI_NVRAM_RO_OFFSET|0x80000000)) || 
			(tHdfDataBlock.ulDataAddr >= (FLASH_UBIFS_AREA_START|0x80000000))) &&
			ucDataType == HDF_DATA_TYPE_APPLICATION )
		{
		/* Read flash data */
		if(bNandRead == TRUE)
		{
			tHdfDataBlock.ulDataAddr &= 0x7fffffff;
			if( FDI_NANDFLASH_Read(tHdfDataBlock.ulDataAddr, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : FDI_FLASH_NAND_Read Error!!\n"));
				FVK_MEM_Free(pucUsbFileBuf);
				*pFlashError = FAI_FLASH_TMD_ERR_NAND_READ_FAIL;

				goto TEST_FAIL;
			}
		}
		else
		{
			if( FDI_FLASH_Read(tHdfDataBlock.ulDataAddr, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : FDI_FLASH_Read Error!!\n"));
				FVK_MEM_Free(pucUsbFileBuf);
				
				*pFlashError = FAI_FLASH_TMD_ERR_NOR_READ_FAIL;

				goto TEST_FAIL;
			}
		}

		/* Data compare */
		if( FVK_MEM_Compare(tHdfDataBlock.aucData, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : Data Compare Error!!\n"));
			FVK_MEM_Free(pucUsbFileBuf);
			*pFlashError = FAI_FLASH_TMD_ERR_VALID_FAIL;

			goto TEST_FAIL;
		}
		ulCompareCount++;
		FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : Data Compare OK!!\n"));			
            }
	}

	FVK_MEM_Free(pucUsbFileBuf);

	/* If there are no compare images, go to error */
	if( ulCompareCount == 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : There are no app images!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_NO_IMAGE;
		goto TEST_FAIL;
	}
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_DataCompare] : %d\n", ulCompareCount));
	close(nUsbFd);
#else
	DI_NVRAM_GetField(DI_NVRAM_FIELD_GANG_CRC, 0, &storedCrc, 4);

#if (USE_NAND_FLASH==YES)
	compare_buf = FVK_MEM_Alloc(N_UBI_ROOT_FS_SIZE + N_UBI_KERNEL_SIZE);

	if( FDI_NANDFLASH_ReadForCRC(FLASH_UBI_AREA_START, compare_buf, N_UBI_ROOT_FS_SIZE + N_UBI_KERNEL_SIZE) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : FDI_NANDFLASH_ReadForCRC Error!!\n"));
		FVK_MEM_Free(compare_buf);				
		goto TEST_FAIL;
	}

	calcCrc = FAI_UTIL_GetCRC32Fast(compare_buf, N_UBI_ROOT_FS_SIZE + N_UBI_KERNEL_SIZE);

	FVK_MEM_Free(compare_buf);

	if(storedCrc != calcCrc)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_DataCompare] : App Data Compare Error!![%08x][%08x]\n", storedCrc, calcCrc));
		goto TEST_FAIL;
	}	
#endif

#endif

	return FAI_NO_ERR;
	
TEST_FAIL:
#if 0
	close(nUsbFd);
#endif
	return FAI_ERR;
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
#if (NAND_DOWNLOAD_ENABLE==YES)
#define UBI_PART_ALIGN	(1*1024*1024)
#define LEB_BASED_SIZE(rSz)	(((rSz+(LOGICAL_BLOCK_SIZE-1))/LOGICAL_BLOCK_SIZE)*LOGICAL_BLOCK_SIZE)
#define PHY_ALLOCATED_SIZE(pSz) ((LEB_BASED_SIZE(pSz)+(UBI_PART_ALIGN-1))&(~(UBI_PART_ALIGN-1)))

FAI_ERR_CODE INTRN_FAI_FLASH_NandDownload(char *pcFileName, FAI_FLASH_TMD_ERR *pFlashError)
{
	int nUsbFd, nUsbFileSize, nBlockIdx, nDataIdx, nReadSize, nUsbFilePos = 0;
	unsigned char ucDataType;
	unsigned char *pucUsbFileBuf;
	HDF_HEADER_BLOCK_T tHdfHeaderBlock;
	HDF_DATA_BLOCK_T tHdfDataBlock;
	int i;
	bool bNandRead = FALSE;
	unsigned long ulDownCnt = 0;
	long long int leb_size;
	unsigned char *db_buf = NULL;
	unsigned char *dbbak_buf = NULL;
	unsigned char *dbuser_buf = NULL;

	unsigned int db_pos = 0;
	unsigned int dbbak_pos = 0;
	unsigned int dbuser_pos = 0;

	long long int nandubidb_leb = 0;
	long long int nandubidbbackup_leb = 0;
	long long int nandubidbuser_leb = 0;
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)	
	unsigned char *dbuserext_buf = NULL;
	unsigned int dbuserext_pos = 0;
	long long int nandubidbuserext_leb = 0;
#endif
	
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)	
	unsigned char *dbuserext2_buf = NULL;
	unsigned int dbuserext2_pos = 0;
	long long int nandubidbuserext2_leb = 0;
#endif
		
	unsigned char *unpack_buf = NULL;
	unsigned int unpack_pos = 0;
	HUINT32	result;

	unsigned int loader_pos = 0;
	unsigned char *loader_buf = NULL;
	unsigned char *pre_loader_buf = NULL;
	unsigned int pre_loader_pos = 0;

	if( pcFileName == NULL )
	{
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_INVALID;
		return FAI_ERR;
	}
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_NandDownload] : File Name : %s\n", pcFileName));


	FAI_SYSTEM_UnMountDBandDBBACKUP();

	/* open usb file : wait for mount */
	for( i = 0; i < FAI_FLASH_TEST_RETRY_COUNT; i++ )
	{
	 	nUsbFd = open(pcFileName, O_RDONLY | O_NONBLOCK);
		if( nUsbFd >= 0 )
		{
			break;
		}
		FVK_TASK_Sleep(FAI_FLASH_TEST_RETRY_SLEEP);
	}

	if( nUsbFd < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : open Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_OPEN_FAIL;
		goto TEST_FAIL;
	}

	/* get file size */
	nUsbFileSize=lseek(nUsbFd, 0, SEEK_END);
	if( nUsbFileSize < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : lseek Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_SEEK_FAIL;

		goto TEST_FAIL;
	}

	/* alloc memory for file buffer */
	pucUsbFileBuf = FVK_MEM_Alloc(nUsbFileSize);
	if( pucUsbFileBuf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;

		goto TEST_FAIL;
	}

	/* move to beginning of file */
	if( lseek(nUsbFd, 0, SEEK_SET) != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : lseek Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_SEEK_FAIL;

		goto TEST_FAIL;
	}

	/* read file */
	nReadSize = read(nUsbFd, pucUsbFileBuf, nUsbFileSize);
	if( nReadSize != nUsbFileSize )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : read Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_READ_FAIL;

		goto TEST_FAIL;
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
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc(loader_buf) Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}

	unpack_buf = FVK_MEM_Alloc(2 * 1024 * 1024);
	if( unpack_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}

	dbbak_buf = FVK_MEM_Alloc(PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE) + (1024 * 1024));
	if( dbbak_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}

	db_buf = FVK_MEM_Alloc(PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE) + (1024 * 1024));
	if( db_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}

	dbuser_buf = FVK_MEM_Alloc(PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE) + (1024 * 1024));
	if( dbuser_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}

#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)	
	dbuserext_buf = FVK_MEM_Alloc(PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE) + (1024 * 1024));
	if( dbuserext_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)	
	dbuserext2_buf = FVK_MEM_Alloc(PHY_ALLOCATED_SIZE(UBIFS_PART5_SIZE) + (1024 * 1024));
	if( dbuserext2_buf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Alloc dbuserext2 Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}
#endif
	/* get hdf header block */
	FAI_UTIL_GetHdfHeaderBlock(pucUsbFileBuf, &tHdfHeaderBlock);
	tHdfDataBlock.aucData = g_aucBlockBuf;	
	nUsbFilePos += HDF_HEADER_BLOCK_SIZE;

	for( nBlockIdx = 0; nBlockIdx < tHdfHeaderBlock.usNumOfDataBlock; nBlockIdx++ )
	{
		/* Get hdf data block */
		FAI_UTIL_GetHdfDataBlock(pucUsbFileBuf+nUsbFilePos, &tHdfDataBlock);
		nUsbFilePos += (tHdfDataBlock.usSize+2);

		ucDataType = tHdfDataBlock.usFlag&HDF_DATA_TYPE_MASK;
		if( ucDataType == HDF_DATA_TYPE_BETACODE )
		{
			HUINT32		betaCode;
			HUINT32		systemId;
			
			betaCode = (tHdfDataBlock.ulDataAddr >> 16 & 0xFFFF);

			if (betaCode >= 0x8000)
			{
				FVK_MEM_Copy(unpack_buf + unpack_pos , tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
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
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error Invalid Beta Code (%d)\n", betaCode));
				goto TEST_FAIL;
			}

			if(DI_NVRAM_Reload() != DI_ERR_OK)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : DI_NVRAM_Reload Error!\n"));
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
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error NVRAM\n"));
				goto TEST_FAIL;
			}
		}
		else if(( ucDataType == HDF_DATA_TYPE_FLASH ) || ( ucDataType == HDF_DATA_TYPE_APPLICATION ))
		{
			if((tHdfDataBlock.ulDataAddr&0x80000000) == 0x0)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : Not nand area!!!\n"));
				continue;
			}

			if(tHdfDataBlock.ulDataAddr == (pre_loader_pos + 0x80000000))
			{
				FVK_MEM_Copy(pre_loader_buf + pre_loader_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				pre_loader_pos += tHdfDataBlock.usOrigDataSize;
			}
			else if(tHdfDataBlock.ulDataAddr == (loader_pos + 0x80000000 + 0x21000))
			{
				FVK_MEM_Copy(loader_buf + loader_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				loader_pos += tHdfDataBlock.usOrigDataSize;
				continue;
			}
			else if(tHdfDataBlock.ulDataAddr == (db_pos + 0x80000000 + FLASH_UBIFS_AREA_START + PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)))
			{
				FVK_MEM_Copy(db_buf + db_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				db_pos += tHdfDataBlock.usOrigDataSize;

				continue;
			}
			else if(tHdfDataBlock.ulDataAddr == (dbbak_pos + 0x80000000 + FLASH_UBIFS_AREA_START))
			{
				FVK_MEM_Copy(dbbak_buf + dbbak_pos , tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				dbbak_pos += tHdfDataBlock.usOrigDataSize;
				printf("%s:%d dbbak_pos = 0x%x\n", __FUNCTION__, __LINE__, dbbak_pos);

				continue;
			}
			else if(tHdfDataBlock.ulDataAddr == (dbuser_pos + 0x80000000 + FLASH_UBIFS_AREA_START + PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE) + PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)))
			{
				FVK_MEM_Copy(dbuser_buf + dbuser_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				dbuser_pos += tHdfDataBlock.usOrigDataSize;
				
				continue;
			}
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)	
			else if(tHdfDataBlock.ulDataAddr == (dbuserext_pos + 0x80000000 + FLASH_UBIFS_AREA_START + PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE) + PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE) + PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE) ))
			{
				FVK_MEM_Copy(dbuserext_buf + dbuserext_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				dbuserext_pos += tHdfDataBlock.usOrigDataSize;
				continue;
			}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)	
			else if(tHdfDataBlock.ulDataAddr == (dbuserext2_pos + 0x80000000 + FLASH_UBIFS_AREA_START + PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE) + PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE) + PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE) + PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE) ))
			{
				FVK_MEM_Copy(dbuserext2_buf + dbuserext2_pos, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				dbuserext2_pos += tHdfDataBlock.usOrigDataSize;
				continue;
			}
#endif
			else
			{
				tHdfDataBlock.ulDataAddr &= 0x7fffffff;

				/* read */
				if( FDI_NANDFLASH_Read(tHdfDataBlock.ulDataAddr, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FDI_FLASH_NAND_Read before compare Error!!\n"));
					*pFlashError = FAI_FLASH_TMD_ERR_NAND_READ_FAIL;

					goto TEST_FAIL;
				}

				ulDownCnt++;
				/* data compare: if same data, skip */
				if( FVK_MEM_Compare(tHdfDataBlock.aucData, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) == 0 )
				{
					FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_FLASH_NandDownload] : same data!\n"));
					*pFlashError = FAI_FLASH_TMD_ERR_VALID_FAIL;

					continue;
				}

				/* write */
				if( FDI_NANDFLASH_Write(tHdfDataBlock.ulDataAddr, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FDI_FLASH_NAND_Read Error!!\n"));
					*pFlashError = FAI_FLASH_TMD_ERR_NAND_WRITE_FAIL;

					goto TEST_FAIL;
				}

				/* read for verify */
				if( FDI_NANDFLASH_Read(tHdfDataBlock.ulDataAddr, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FDI_FLASH_NAND_Read for verify Error!!\n"));
					*pFlashError = FAI_FLASH_TMD_ERR_NAND_READ_FAIL;

					goto TEST_FAIL;
				}

				/* data compare for verify */
				if( FVK_MEM_Compare(tHdfDataBlock.aucData, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != 0 )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FVK_MEM_Compare Error!!!\n"));
					*pFlashError = FAI_FLASH_TMD_ERR_VALID_FAIL;

					goto TEST_FAIL;
				}
			}
		}
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

		if( FDI_NANDFLASH_Write(0x0, loader_buf, loader_pos) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
			FVK_MEM_Free(compare_buf);				
			goto TEST_FAIL;
		}
		
		if( FDI_NANDFLASH_Read(0x0, compare_buf, loader_pos) != FDI_NO_ERR )
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

	if(dbbak_pos != 0)
	{
		if(DI_NANDFLASH_UBIFS_Open(0, &nandubidbbackup_leb)!= DI_ERR_OK)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Open (%d)\n", 0));
			goto TEST_FAIL;
		}

		if( FDI_NANDFLASH_Write(FLASH_UBIFS_AREA_START , dbbak_buf, dbbak_pos) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
			goto TEST_FAIL;
		}

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
		
		dbbak_pos = 0;
	}
	
	if(db_pos != 0)
	{
		if(DI_NANDFLASH_UBIFS_Open(1, &nandubidb_leb) != DI_ERR_OK)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Open (%d)\n", 1));
			goto TEST_FAIL;
		}
	
		if( FDI_NANDFLASH_Write(FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE), db_buf, db_pos) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
			goto TEST_FAIL;
		}
	
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
		
		db_pos = 0;
	}
	
	if(dbuser_pos != 0)
	{
		if(DI_NANDFLASH_UBIFS_Open(2, &nandubidbuser_leb) != DI_ERR_OK)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Open (%d)\n", 2));
			goto TEST_FAIL;
		}
	
		if( FDI_NANDFLASH_Write(FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE), dbuser_buf, dbuser_pos) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
			goto TEST_FAIL;
		}
	
		if( FDI_NANDFLASH_Sync() != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FDI_NANDFLASH_Sync Error!!\n"));
			goto TEST_FAIL;
		}
	
		if(DI_NANDFLASH_UBIFS_Close(2) != DI_ERR_OK)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Close (%d)\n", 2));
			goto TEST_FAIL;
		}
	
		dbuser_pos = 0;
	}
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)	
	if(dbuserext_pos != 0)
	{
		if(DI_NANDFLASH_UBIFS_Open(3, &nandubidbuserext_leb) != DI_ERR_OK)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Final Error DI_NANDFLASH_UBIFS_Open (%d)\n", 3));
			goto TEST_FAIL;
		}
	
		if( FDI_NANDFLASH_Write(FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE), dbuserext_buf, dbuserext_pos) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
			goto TEST_FAIL;
		}
	
		if( FDI_NANDFLASH_Sync() != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FDI_NANDFLASH_Sync Error!!\n"));
			goto TEST_FAIL;
		}
	
		if(DI_NANDFLASH_UBIFS_Close(3) != DI_ERR_OK)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Close (%d)\n", 3));
			goto TEST_FAIL;
		}
	
		dbuserext_pos = 0;
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)	
	if(dbuserext2_pos != 0)
	{
		if(DI_NANDFLASH_UBIFS_Open(4, &nandubidbuserext2_leb) != DI_ERR_OK)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Final Error DI_NANDFLASH_UBIFS_Open (%d)\n", 4));
			goto TEST_FAIL;
		}
	
		if( FDI_NANDFLASH_Write(FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE), dbuserext2_buf, dbuserext2_pos) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SYSTEM_DefaultSettingUsingHDF] : FDI_NANDFLASH_Write Error!!\n"));
			goto TEST_FAIL;
		}
	
		if( FDI_NANDFLASH_Sync() != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : FDI_NANDFLASH_Sync Error!!\n"));
			goto TEST_FAIL;
		}
	
		if(DI_NANDFLASH_UBIFS_Close(4) != DI_ERR_OK)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] Error DI_NANDFLASH_UBIFS_Close (%d)\n", 4));
			goto TEST_FAIL;
		}
	
		dbuserext2_pos = 0;
	}
#endif
	
	FVK_MEM_Free(loader_buf);
	FVK_MEM_Free(pucUsbFileBuf);
	FVK_MEM_Free(db_buf);
	FVK_MEM_Free(dbbak_buf);
	FVK_MEM_Free(dbuser_buf);
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)	
	FVK_MEM_Free(dbuserext_buf);
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)	
	FVK_MEM_Free(dbuserext2_buf);
#endif
	FVK_MEM_Free(unpack_buf);
	
	/* If there are no nand images, go to error */
	if( ulDownCnt == 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : There are no nand images!!\n"));
		goto TEST_FAIL;
	}

	if(DI_NVRAM_Reload() != DI_ERR_OK)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_NandDownload] : DI_NVRAM_Reload Error!\n"));
		goto TEST_FAIL;
	}
	close(nUsbFd);
	return FAI_NO_ERR;
	
TEST_FAIL:
	*pFlashError = FAI_FLASH_TMD_ERR_VALID_FAIL;
	FVK_MEM_Free(pucUsbFileBuf);
	FVK_MEM_Free(db_buf);
	FVK_MEM_Free(dbbak_buf);
	FVK_MEM_Free(dbuser_buf);
	FVK_MEM_Free(loader_buf);
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)	
	FVK_MEM_Free(dbuserext_buf);
#endif

	FVK_MEM_Free(unpack_buf);
	close(nUsbFd);
	return FAI_ERR;		
}
#endif

#if (EMMC_DOWNLOAD_ENABLE==YES)
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FAI_ERR_CODE INTRN_FAI_FLASH_eMMCDownload(char *pcFileName, FAI_FLASH_TMD_ERR *pFlashError)
{
	int nUsbFd, nUsbFileSize, nBlockIdx, nDataIdx, nReadSize, nUsbFilePos = 0;
	unsigned char ucDataType;
	unsigned char *pucUsbFileBuf;
	HDF_HEADER_BLOCK_T tHdfHeaderBlock;
	HDF_DATA_BLOCK_T tHdfDataBlock;
	int i;
	bool bNandRead = FALSE;
	unsigned long ulDownCnt = 0;
	FDI_ERR_CODE eFdiErr;

	#define EMMC_PART_MAX_NUM		20
	HUINT8 *pwrite_buf;
	HUINT16 data_pos=0;
	HUINT32 ulDummyByte=0;
	HUINT32 ulShowAddr=0;	
	int nCrc =0x00;
	int nCrc2 =0x00;	
	HUINT16 nHdfBlockSize=0x8000;

	pwrite_buf= FVK_MEM_Alloc(nHdfBlockSize);/* buffer for emmc write*/
	if(pwrite_buf!= NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Allocated...\n",__func__));
	}
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Allocation Failed...\n",__func__));
	}	

	if( pcFileName == NULL )
	{
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_INVALID;
		return FAI_ERR;
	}
	FAI_PRINT(FAI_PRT_ERROR, ("[%s] : File Name : %s\n", __func__, pcFileName));

	/* open usb file : wait for mount */
	for( i = 0; i < FAI_FLASH_TEST_RETRY_COUNT; i++ )
	{
	 	nUsbFd = open(pcFileName, O_RDONLY | O_NONBLOCK);
		if( nUsbFd >= 0 )
		{
			break;
		}
		FVK_TASK_Sleep(FAI_FLASH_TEST_RETRY_SLEEP);
	}

	if( nUsbFd < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : open Error!!\n", __func__));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_OPEN_FAIL;
		goto TEST_FAIL;
	}

	/* get file size */
	nUsbFileSize=lseek(nUsbFd, 0, SEEK_END);
	if( nUsbFileSize < 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : lseek Error!!\n",__func__));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_SEEK_FAIL;

		goto TEST_FAIL;
	}

	/* alloc memory for file buffer */
	pucUsbFileBuf = FVK_MEM_Alloc(nUsbFileSize);
	if( pucUsbFileBuf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FVK_MEM_Alloc Error!!\n",__func__));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;

		goto TEST_FAIL;
	}

	/* move to beginning of file */
	if( lseek(nUsbFd, 0, SEEK_SET) != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : lseek Error!!\n",__func__));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_SEEK_FAIL;

		goto TEST_FAIL;
	}

	/* read file */
	nReadSize = read(nUsbFd, pucUsbFileBuf, nUsbFileSize);
	if( nReadSize != nUsbFileSize )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : read Error!!\n",__func__));
		*pFlashError = FAI_FLASH_TMD_ERR_FILE_READ_FAIL;

		goto TEST_FAIL;
	}

	/* get hdf header block */
	FAI_UTIL_GetHdfHeaderBlock(pucUsbFileBuf, &tHdfHeaderBlock);

	tHdfDataBlock.aucData = g_aucBlockBuf;	
	nUsbFilePos += HDF_HEADER_BLOCK_SIZE;
	ulShowAddr=0;
	for( nBlockIdx = 0; nBlockIdx < tHdfHeaderBlock.usNumOfDataBlock; nBlockIdx++ )
	{
		/* Get hdf data block */
		FAI_UTIL_GetHdfDataBlock(pucUsbFileBuf+nUsbFilePos, &tHdfDataBlock);
		nUsbFilePos += (tHdfDataBlock.usSize+2);

		ucDataType = tHdfDataBlock.usFlag&HDF_DATA_TYPE_MASK;
		if( ucDataType == HDF_DATA_TYPE_BETACODE )
		{
			HUINT32		betaCode;
			HUINT32		systemId;
			
			betaCode = (tHdfDataBlock.ulDataAddr >> 16 & 0xFFFF);

			if (betaCode >= 0x8000)
			{
				FVK_MEM_Copy(pwrite_buf , tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				data_pos += tHdfDataBlock.usOrigDataSize;				
				continue;
			}

			if (data_pos != 0)
			{
				eFdiErr = FDI_NVRAM_UnpackNvramCfg(pwrite_buf, data_pos);
				if (eFdiErr != ERR_OK)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] NVRAM unpack error(0x%x)\n", __func__,eFdiErr));
					goto TEST_FAIL;
				}
				
				data_pos = 0;
			}

			if (betaCode >= s_sizeBetaTable)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] Error Invalid Beta Code (%d)\n", __func__,betaCode));
				goto TEST_FAIL;
			}

			if(DI_NVRAM_Reload() != DI_ERR_OK)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] : DI_NVRAM_Reload Error!\n",__func__));
				goto TEST_FAIL;
			}
			if (s_betaTable[betaCode] == DI_NVRAM_FIELD_SYSTEM_ID)
			{
				systemId = (tHdfDataBlock.aucData[0] << 24) | (tHdfDataBlock.aucData[1] << 16) 
						| (tHdfDataBlock.aucData[2] << 8) | (tHdfDataBlock.aucData[3]);
				eFdiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_SYSTEM_ID, 0, &systemId, sizeof(HUINT32));
			}
			else
			{
				eFdiErr = DI_NVRAM_SetField((DI_NVRAM_FIELD_T)s_betaTable[betaCode], 0, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
			}
			if (eFdiErr)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] Error NVRAM\n",__func__));
				goto TEST_FAIL;
			}
		}
		else if(( ucDataType == HDF_DATA_TYPE_FLASH ) || ( ucDataType == HDF_DATA_TYPE_APPLICATION ))
		{
			if(tHdfDataBlock.ulDataAddr>=0x200000) /* eMMC */
			{
				switch(tHdfDataBlock.ulDataAddr)
				{
					case 0xFFFFFFFF: 
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] MICOM S/W (0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;
					case 0xFFFFFFFE: 
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] RF4CE S/W (0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;
					case 0xFFFFFFFD:
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] CABLEMODEM S/W (0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;
					case 0xFFFFFFFC: 
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] WIFI S/W (0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;
					case N_PART_GP3_OFFSET: /* 64 MB */
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] GP3 (0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;
					case N_PART_GP2_OFFSET: /* 64 MB */
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] GP2 (0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;
					case N_PART_GP1_OFFSET: 
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] GP1 (0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;
					case N_PART_GP0_OFFSET: 
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] GP0 (0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;
					case (N_PART_GP0_OFFSET+N_IMAGE_OTALDR_OFFSET): 	/*uocto 32MB */
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] OTALDR(0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;
					case (N_PART_GP0_OFFSET+N_IMAGE_OTA2NDLDR_OFFSET): /*uocto 32MB */
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] OTA2nLDR(0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;					
					case N_PART_GP1_KERNEL_OFFSET: /* 6MB */
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] KERNEL(0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));
						break;					
					case N_PART_GP1_ROOTFS_OFFSET: /* 250MB */
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] ROOTFS(0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));					
						break;					
					case N_PART_GP1_APP_OFFSET: /* 64MB */
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] APPLICATION0(0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));					
						break;					
					case N_PART_FSAREA_OFFSET: /* 972 MB */
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] FS AREA(0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));					
						break;
					case N_PART_BOOT1_OFFSET: /* 2MB */
						FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] CFE ENV(0x%X)!!! type=0x%x\n",__func__,tHdfDataBlock.ulDataAddr,ucDataType));		
						break;
				}
				
				ulDownCnt++;
				FVK_MEM_Copy(pwrite_buf, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);
				//data_pos+= tHdfDataBlock.usOrigDataSize;

				/* write */
				if( FDI_EMMC_Write(tHdfDataBlock.ulDataAddr, pwrite_buf, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FDI_EMMC_Write Error!!\n",__func__));
					*pFlashError = FAI_FLASH_TMD_ERR_NAND_WRITE_FAIL;

					goto TEST_FAIL;
				}
				/* read for verify */
				FVK_MEM_Set(g_aucFlashDataBlockBuf,0x00, sizeof(g_aucFlashDataBlockBuf));
				
				if( FDI_EMMC_Read(tHdfDataBlock.ulDataAddr, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FDI_EMMC_Read for verify Error!!\n",__func__));
					*pFlashError = FAI_FLASH_TMD_ERR_NAND_READ_FAIL;

					goto TEST_FAIL;
				}
				nCrc= (int)FAI_UTIL_GetCRC16(0x0, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize);
				nCrc2= (int)FAI_UTIL_GetCRC16(0x0, pwrite_buf, tHdfDataBlock.usOrigDataSize);			
				if(nCrc!=nCrc2)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] : CRC16 Error!(Addr=0x%08X, nCrc=0x%04X, readCrc=0x%04X)!\n",__func__,tHdfDataBlock.ulDataAddr, nCrc,nCrc2));
					goto TEST_FAIL;
				}
			
				/* data compare for verify */
				if( FVK_MEM_Compare(pwrite_buf, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != 0 )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FVK_MEM_Compare Error!!!\n",__func__));
					*pFlashError = FAI_FLASH_TMD_ERR_VALID_FAIL;

					goto TEST_FAIL;
				}
				FAI_PRINT(FAI_PRT_ERROR, ("."));
			}
			else
			{
				ulShowAddr = tHdfDataBlock.ulDataAddr + (ulDummyByte*0xA);
				if((ulShowAddr%0x8000)==0)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("\n[%s] NOR flash (0x%X)!!! type=0x%x\n",__func__,ulShowAddr, ucDataType));
					ulDummyByte++;
				}
				if( FDI_FLASH_WriteAfterErase(tHdfDataBlock.ulDataAddr, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FDI_FLASH_WriteAfterErase Error!!\n",__func__));
					goto TEST_FAIL;
				}
				/* Read flash data for verification */
				FVK_MEM_Set(g_aucFlashDataBlockBuf,0x00, sizeof(g_aucFlashDataBlockBuf));
				if( FDI_FLASH_Read(tHdfDataBlock.ulDataAddr, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FDI_FLASH_Read Error!!\n",__func__));
					goto TEST_FAIL;
				}
				nCrc= (int)FAI_UTIL_GetCRC16(0x0, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize);
				nCrc2= (int)FAI_UTIL_GetCRC16(0x0, tHdfDataBlock.aucData, tHdfDataBlock.usOrigDataSize);			
				if(nCrc!=nCrc2)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] : CRC16 Error!(Addr=0x%08X, nCrc=0x%04X, readCrc=0x%04X)!\n",__func__,tHdfDataBlock.ulDataAddr, nCrc,nCrc2));
					goto TEST_FAIL;
				}

				/* Data compare */
				if( FVK_MEM_Compare(tHdfDataBlock.aucData, g_aucFlashDataBlockBuf, tHdfDataBlock.usOrigDataSize) != 0 )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Flash Data Compare Error!!\n",__func__));
					goto TEST_FAIL;
				}
				FAI_PRINT(FAI_PRT_ERROR, ("."));
			}			
		}
	}
	FAI_PRINT(FAI_PRT_ERROR, ("\n"));
	/* If there are no nand images, go to error */
	if( ulDownCnt == 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : There are no eMMC images!!\n",__func__));
		goto TEST_FAIL;
	}
	if(DI_NVRAM_Reload() != DI_ERR_OK)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : DI_NVRAM_Reload Error!\n",__func__));
		goto TEST_FAIL;
	}
	if(pucUsbFileBuf)
	{
		FVK_MEM_Free(pucUsbFileBuf);
	}
	if(pwrite_buf)
	{
		FVK_MEM_Free(pwrite_buf);
	}
	close(nUsbFd);
	FAI_PRINT(FAI_PRT_ERROR, ("%s End\n",__func__));	
	return FAI_NO_ERR;
	
TEST_FAIL:
	FAI_PRINT(FAI_PRT_ERROR, ("%s Error\n",__func__));	
	*pFlashError = FAI_FLASH_TMD_ERR_VALID_FAIL;

	if(pucUsbFileBuf)
	{
		FVK_MEM_Free(pucUsbFileBuf);
	}
	if(pwrite_buf)
	{
		FVK_MEM_Free(pwrite_buf);
	}	
	close(nUsbFd);
	return FAI_ERR;		
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
FAI_ERR_CODE INTRN_FAI_FLASH_CrcCheck(unsigned char ucFlashType,unsigned long ulAddr,unsigned long ulSize,int *pnCrc, FAI_FLASH_TMD_ERR *pFlashError)
{
    unsigned char *aucFlashData;
	int nCnt = 0;
    int nCrc =0x00,nRemainSize = 0;
	int i;
	
	aucFlashData = FVK_MEM_Alloc(0x100000);
	if( aucFlashData == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_CrcCheck] : FVK_MEM_Alloc Error!!\n"));
		*pFlashError = FAI_FLASH_TMD_ERR_MALLOC_FAIL;

		goto TEST_FAIL;
	}

	nCnt = ulSize/0x100000;
	nRemainSize = ulSize%0x100000;

    /* Read flash data */
    if(ucFlashType == FLASH_TYPE_NAND)
    {
	    	for( i = 0; i < nCnt ; i++ )
	    	{
			if( FDI_NANDFLASH_Read(ulAddr+0x100000*i, aucFlashData, 0x100000) != FDI_NO_ERR )
        {
            FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_CrcCheck] : FDI_FLASH_NAND_Read Error!!\n"));
            FVK_MEM_Free(aucFlashData);
			*pFlashError = FAI_FLASH_TMD_ERR_NAND_READ_FAIL;
             goto TEST_FAIL;
        }
			nCrc= (int)FAI_UTIL_GetCRC16(nCrc, aucFlashData, 0x100000);
	    	}
		if(nRemainSize>0)
    	{
			if( FDI_NANDFLASH_Read(ulAddr+0x100000*i, aucFlashData, nRemainSize) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_CrcCheck] : FDI_FLASH_NAND_Read Error!!\n"));
				FVK_MEM_Free(aucFlashData);
				*pFlashError = FAI_FLASH_TMD_ERR_NAND_READ_FAIL;
				goto TEST_FAIL;
			}
			nCrc= (int)FAI_UTIL_GetCRC16(nCrc, aucFlashData, nRemainSize);
    	}
	}
#if !defined(FACTORY_USE_DI20) || (defined(FACTORY_USE_DI20) && defined(FACTORY_USE_NOR_FLASH))		
	else if(ucFlashType == FLASH_TYPE_NOR)
	
		{
		for( i = 0; i < nCnt ; i++ )
		{
			if( FDI_FLASH_Read(ulAddr+0x100000*i, aucFlashData, 0x100000) != FDI_NO_ERR )
	        {
	            FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_CrcCheck] : FDI_FLASH_NAND_Read Error!!\n"));
	            FVK_MEM_Free(aucFlashData);
				*pFlashError = FAI_FLASH_TMD_ERR_NOR_READ_FAIL;
	             goto TEST_FAIL;
	        }
			nCrc= (int)FAI_UTIL_GetCRC16(nCrc, aucFlashData, 0x100000);
		}
		if(nRemainSize>0)
    {
			if( FDI_FLASH_Read(ulAddr+0x100000*i, aucFlashData, nRemainSize) != FDI_NO_ERR )
        {
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_CrcCheck] : FDI_FLASH_NAND_Read Error!!\n"));
            FVK_MEM_Free(aucFlashData);
			*pFlashError = FAI_FLASH_TMD_ERR_NOR_READ_FAIL;
            goto TEST_FAIL;
        }
			nCrc= (int)FAI_UTIL_GetCRC16(nCrc, aucFlashData, nRemainSize);
		}		
    }
#endif
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_CrcCheck] : FlashType (0x%x)ERROR!!\n",ucFlashType));
		FVK_MEM_Free(aucFlashData);
		*pFlashError = FAI_FLASH_TMD_ERR_INVALID_FLASH_TYPE;
		goto TEST_FAIL;
	}

	*pnCrc = nCrc;
    FVK_MEM_Free(aucFlashData);
    return FAI_NO_ERR;

TEST_FAIL:

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
FAI_ERR_CODE INTRN_FAI_FLASH_EraseCheck(FAI_FLASH_TYPE_e FlashType, int nAddress, int nSize, FAI_FLASH_TMD_ERR *pFlashError)
{
    FDI_ERR_CODE eFDiErr;
    bool bIsNand = FALSE;
    unsigned long ulStartBlock, ulEndBlock, ulBlockIdx;
    unsigned long ulBlockSize=0, ulNumOfBlock=0;

    if(FlashType == FAI_FLASH_NAND)
    {
        bIsNand = TRUE;
    }
    else
    {
        bIsNand = FALSE;
    }

    FDI_FLASH_GetInfo(&ulNumOfBlock,&ulBlockSize);

    ulStartBlock = nAddress/ulBlockSize;
    ulEndBlock = (nAddress+nSize-1)/ulBlockSize;


    /* loop blocks */
    for( ulBlockIdx = ulStartBlock; ulBlockIdx <= ulEndBlock; ulBlockIdx++)
    {
        /* erase block */

        if(bIsNand == TRUE)  /* NAND Erase */
        {
             /* NAND erase 추가 구현 필요 */
//            eFDiErr = FDI_FLASH_EraseBlock(ulBlockIdx);
        }
        else                 /* NOR Erase */
        {
 #if !defined(FACTORY_USE_DI20) || (defined(FACTORY_USE_DI20) && defined(FACTORY_USE_NOR_FLASH))       
            eFDiErr = FDI_FLASH_EraseBlock(ulBlockIdx);
 #else
 			eFDiErr = FDI_NO_ERR;
 #endif
        }

        if( eFDiErr != FDI_NO_ERR )
        {
            FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_FLASH_EraseCheck] : DI_FLASH_EraseBlock(%x) Error\n", ulBlockIdx));
			*pFlashError = FAI_FLASH_TMD_ERR_ERASE_FAIL;

            return FAI_ERR;
        }

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
FAI_ERR_CODE INTRN_FAI_FLASH_PatternTest(int *pnAddress, int *pnData, FAI_FLASH_TMD_ERR *pFlashError)
{
    /* 여기 채워야 함. */
    return FAI_FEATURE_NOT_SUPPORTED;
}

#if (NAND_BAD_BLOCK_ENABLE==YES)
FAI_ERR_CODE INTRN_FAI_FLASH_NandBadBlockCheck( int *piBadStatus,FAI_FLASH_TMD_ERR *pFlashError )
{
	FDI_ERR_CODE eFdi = 0;
	unsigned char index = 0;
	unsigned int ulAddr = 0,ulSize = 0;
	int nandBadStatus = 0;
	unsigned int mounted1 = 0, mounted2 = 0, mounted3 = 0;

	*piBadStatus = 0;

	for(index = 1; index < 3; index++)
	{
		if(index == 0)
		{
			/* Loader 는 체크 하지 않는데.. */
			ulAddr = N_BL_OFFSET;
			ulSize = N_BL_SIZE;
		}
		else if(index == 1)
		{
			ulAddr = FLASH_UBI_AREA_START;
			ulSize = FLASH_UBI_AREA_SIZE;
		}
		else if(index == 2)
		{
			//FAI_SYSTEM_UnMountDBandDBBACKUP();
			//VK_SYSTEM_Command("/usr/sbin/ubidetach -m 5");
		
			ulAddr = FLASH_UBIFS_AREA_START;
			ulSize = FLASH_UBIFS_AREA_SIZE;
		}
		else
		{
			ulAddr = N_BL_OFFSET;
			ulSize = N_BL_SIZE;
		}

		eFdi =  FDI_NANDFLASH_CheckBadBlock(ulAddr,ulSize,&nandBadStatus);
		if(eFdi != 0)
		{
			if(index == 2)
			{
				//VK_SYSTEM_Command("/usr/sbin/ubiattach -m 5");
				//FAI_SYSTEM_MountDBandDBBACKUP(&mounted1, &mounted2,&mounted3);
			}
			*pFlashError = FAI_FLASH_TMD_ERR_UNKNOWN;
			return FAI_ERR;
		}
		else
		{

			if(nandBadStatus < 0)
			{
				*pFlashError = FAI_FLASH_TMD_ERR_UNKNOWN;
				return FAI_ERR;
			}
			
			*piBadStatus += nandBadStatus;
		}
		
		VK_TASK_Sleep(10);
		
	}

	//VK_SYSTEM_Command("/usr/sbin/ubiattach -m 5");
	//FAI_SYSTEM_MountDBandDBBACKUP(&mounted1, &mounted2,&mounted3);
	return FAI_NO_ERR;
}
#endif

