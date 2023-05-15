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
#include <string.h>

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"
#include "fai_hdd.h"
#include "fdi_hdd.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
unsigned long ulHddDstTimerId;
int nHddFd = 0;

#define HDD_SECTOR_SIZE		512
#define FAI_SMART_BUF_SIZE	516

#define ATA_FIRST_HDD_PORT_NUM 1
#define ATA_SECOND_HDD_PORT_NUM 2
#define ATA_THIRD_HDD_PORT_NUM 3

typedef enum
{
	INTERNAL_HDD,
	EXTERNAL_HDD,
	ESATA_HDD,
	HDD_POSITION_UNKNOWN
} FAI_HDD_POSITION_e;

typedef enum
{
	FORAT_FAT16,
    FORAT_FAT32,
    FORAT_NTFS,
    FORAT_EXT2,
    FORAT_EXT3,
    FORAT_HUMAX_FS,
    FORAT_CHIP_VENDOR_FS,
    HDD_FORMAT_TYPE_UNKNOWN
} FAI_HDD_FORMAT_TYPE_e;

typedef enum
{
	MODELNAME,
    SIZE,
    FIRMWARE_VERSION,
    SERIAL_NUMBER,
    PARTITION_INFO,
    HDD_INFO_UNKNOWN
} FAI_HDD_INFO_e;

typedef struct HDD_CMD_FORMAT
{
    FAI_HDD_POSITION_e HddPosition;
    FAI_HDD_FORMAT_TYPE_e HddFormatType;
} HDD_CMD_FORMAT_t;

typedef struct HDD_CMD_DATACOMP
{
    FAI_HDD_POSITION_e HddPosition;
} HDD_CMD_DATACOMP_t;

typedef struct HDD_CMD_GETINFO
{
    FAI_HDD_POSITION_e HddPosition;
    FAI_HDD_INFO_e HddInfo;
} HDD_CMD_GETINFO_t;

typedef struct HDD_CMD_SETPARK
{
    FAI_HDD_POSITION_e HddPosition;
    BOOL bParkOnOff;
} HDD_CMD_SETPARK_t;

typedef struct FAI_HDD_MSG
{
	int nCmd;
	int nResult;
    union
	{
		HDD_CMD_FORMAT_t Format;
		HDD_CMD_DATACOMP_t DataComp;
		HDD_CMD_GETINFO_t GetInfo;
		HDD_CMD_SETPARK_t SetPark;
	} cmd_arg;
} FAI_HDD_MSG_t;

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
unsigned long g_ulHddMsgId, g_ulHddTaskId;

/* sgkim4, Static variables and structures for encrypted filesystem */
#if (HDD_ENCRYPT == YES)
static BOOL s_bEncryptedPartitionOpened = FALSE;
#endif

void INTRN_FAI_HDD_Task(void *pData);
FAI_ERR_CODE INTRN_FAI_HDD_Format(FAI_HDD_POSITION_e HddPos, FAI_HDD_FORMAT_TYPE_e HddFormatType, FAI_HDD_TMD_ERR *pHddError);
FAI_ERR_CODE INTRN_FAI_HDD_DataCompare(FAI_HDD_POSITION_e HddPosition, FAI_HDD_TMD_ERR *pHddError);
#if ((CONFIG_PROD_HDR3000T==YES) || (CONFIG_PROD_HDR4000T==YES) || (PRODUCT_HMSLITE==YES))
FAI_ERR_CODE INTRN_FAI_HDD_GetInfo(FAI_HDD_POSITION_e HddPosition, FAI_HDD_INFO_e HddInfo, unsigned char *pucResult, FAI_HDD_TMD_ERR *pHddError, unsigned char *pucResultInfo);
#else
FAI_ERR_CODE INTRN_FAI_HDD_GetInfo(    FAI_HDD_POSITION_e HddPosition, FAI_HDD_INFO_e HddInfo, unsigned char *pucResult, FAI_HDD_TMD_ERR *pHddError);
#endif
FAI_ERR_CODE INTRN_FAI_HDD_SetParking(FAI_HDD_POSITION_e HddPosition, BOOL bParking_onoff, FAI_HDD_TMD_ERR *pHddError);
FAI_ERR_CODE INTRN_FAI_HDD_Defect(int *pnDefectCnt, FAI_HDD_TMD_ERR *pHddError);
FAI_ERR_CODE INTRN_FAI_HDD_ReadPOH(int *pnReadTime, FAI_HDD_TMD_ERR *pHddError);
FAI_ERR_CODE INTRN_FAI_HDD_SmartShortDST(FAI_HDD_TMD_ERR *pHddError);


#if (HDD_FORMAT_ENABLE==YES)

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FAI_ERR_CODE INTRN_FAI_HDD_Func_Fail(void)
{
	FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Format Fail\n",__FUNCTION__));
	
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
FAI_ERR_CODE INTRN_FAI_HDD_Func_OK(void)
{
	FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Format Ok\n",__FUNCTION__));

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
FAI_ERR_CODE FAI_HDD_Init(void)
{
	FVK_ERR_CODE eFvkErr;
	FAI_ERR_CODE eFdiErr;

	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_HDD_MSG_t), (char*)"FlsMsg", &g_ulHddMsgId);
	if( eFvkErr	!= FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Init] : FVK_MSG_Create Error \n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Create(INTRN_FAI_HDD_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "hdd Task", NULL, &g_ulHddTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Start(g_ulHddTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}
	
#if (HDD_FORMAT_ENABLE==YES)
	FDI_HDD_RegisterCallback(FDI_HDD_EVENT_FAIL, &INTRN_FAI_HDD_Func_Fail);
	FDI_HDD_RegisterCallback(FDI_HDD_EVENT_DONE, &INTRN_FAI_HDD_Func_OK);
#endif	
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
#if (HDD_FORMAT_ENABLE==YES)
FAI_ERR_CODE FAI_HDD_Format(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_HDD_MSG_t tHddMsg;
	unsigned char ucHddPosition;
	unsigned char ucHddInfo;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Format] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 1;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

    FAI_TMD_GetParam(&ucHddPosition, pucArgList, 0);
    FAI_TMD_GetParam(&ucHddInfo, pucArgList, 1);

	tHddMsg.cmd_arg.GetInfo.HddPosition = (FAI_HDD_POSITION_e)ucHddPosition;
	tHddMsg.cmd_arg.GetInfo.HddInfo		= (FAI_HDD_INFO_e)ucHddInfo;

    tHddMsg.nCmd = 0x01;   /* 0x01 means EEPROM ERase All Test */
	eFvkErr = FVK_MSG_Send(g_ulHddMsgId, &tHddMsg, sizeof(FAI_HDD_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Format] : Error FVK_MSG_Send() : %d\n", eFvkErr));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
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

#define PART_NUM	3
FAI_ERR_CODE FAI_HDD_Format_UI(void)
{
	FVK_ERR_CODE eFvkErr;
	FAI_HDD_MSG_t tHddMsg;
	unsigned char ucHddPosition;
	unsigned char ucHddInfo;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
     /* 여기 채워야함 */
	 FDI_FS ePartitiontype;
	 FAI_ERR_CODE eFdiErr;

	HUINT64 ulDefinedPartSize[PART_NUM];	
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( GET_STEP() == 0 )
	{
		SET_STEP(1);
		/* Display hdd test start */
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HDD", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Formating", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FVK_TASK_Sleep( (unsigned long)1000);

#if (HDD_FORMAT_ENABLE==YES)
#if (PRODUCT_YSR2000==YES)
		ulDefinedPartSize[0]=1044225;
		ulDefinedPartSize[1]=965209297;
		ulDefinedPartSize[2]=10482412;
		ePartitiontype=FDI_FS_EXT4_FILESYSTEM;
#else
#error "TODO: MUST DEFINE HDD PARTITON"
#endif
		FAI_PRINT(FAI_PRT_ERROR, ("[FDI_HDD_Format] : Format Start....\n"));
	
		eFdiErr = FDI_HDD_Format(0/* fix value*/, PART_NUM , ulDefinedPartSize ,ePartitiontype);
		SET_STEP(0);
		if(eFdiErr!=FDI_NO_ERR)
		{
			goto TEST_FAIL;
		}
#endif
	}
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HDD Format", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_HDD_TEST_PASS_STR);
	return FAI_NO_ERR;

	TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HDD Format", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_HDD_TEST_FAIL_STR);
	
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
#if (HDD_DATA_COMPARE_ENABLE==YES)
FAI_ERR_CODE FAI_HDD_DataCompare(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_HDD_MSG_t tHddMsg;
	unsigned char ucHddPosition;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_DataCompare] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 2;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

    FAI_TMD_GetParam(&ucHddPosition, pucArgList, 0);

	tHddMsg.cmd_arg.GetInfo.HddPosition = (FAI_HDD_POSITION_e)ucHddPosition;

    tHddMsg.nCmd = 0x02;   /* 0x02 means HDD Read/Write Compare Test */
	eFvkErr = FVK_MSG_Send(g_ulHddMsgId, &tHddMsg, sizeof(FAI_HDD_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_DataCompare] : Error FVK_MSG_Send() : %d\n", eFvkErr));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 2;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

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
#if ((CONFIG_PROD_HDR3000T==YES) || (CONFIG_PROD_HDR4000T==YES) || (PRODUCT_HMSLITE==YES))
#if (HDD_GET_INFO_ENABLE==YES)
FAI_ERR_CODE FAI_HDD_GetInfo(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_HDD_MSG_t tHddMsg;
	unsigned char ucHddPosition;
	unsigned char ucHddInfo;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_GetInfo] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 3;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

    FAI_TMD_GetParam(&ucHddPosition, pucArgList, 0);
    FAI_TMD_GetParam(&ucHddInfo, pucArgList, 1);

	tHddMsg.cmd_arg.GetInfo.HddPosition = (FAI_HDD_POSITION_e)ucHddPosition;
	tHddMsg.cmd_arg.GetInfo.HddInfo		= (FAI_HDD_INFO_e)ucHddInfo;
    tHddMsg.nCmd = 0x03;   /* 0x03 means FAI_HDD_GetInfo*/
	eFvkErr = FVK_MSG_Send(g_ulHddMsgId, &tHddMsg, sizeof(FAI_HDD_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_GetInfo] : Error FVK_MSG_Send() : %d\n", eFvkErr));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 3;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);

        return FAI_ERR;
    }

}
#endif
#else
#if (HDD_GET_INFO_ENABLE==YES)
FAI_ERR_CODE FAI_HDD_GetInfo(unsigned short usMsgId, unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_ERR_CODE eFdiErr;
	unsigned char ucHddPos, ucInfoType;
	unsigned char aucResult[FDI_HDD_INFO_MAX_SIZE+3] = {0,};
	FDI_HDD_INFO_t tHddInfo;
	unsigned char aucHDDSize[20];
	unsigned char numParam=2;
	unsigned char length;
	int cnt, currPos=0;
	unsigned int partStrSize;

	unsigned char tmpSN[255] = {0,};
	unsigned int totalLength = 0,count = 0, minuscount = 0,indicator = 0;

	FAI_TMD_StartTest(0);

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_DataCompare] : Argument Count Error=%d\n", ucArgc));
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&ucHddPos, pucArgList, 0);
	FAI_TMD_GetParam(&ucInfoType, pucArgList, 1);

	memset(&tHddInfo, 0x0, sizeof(FDI_HDD_INFO_t));

	if (ucHddPos == 0x0)
		eFdiErr = FDI_HDD_GetPartitonInfo(ATA_FIRST_HDD_PORT_NUM, &tHddInfo);
	else if (ucHddPos == 0x1)
		eFdiErr = FDI_HDD_GetPartitonInfo(ATA_SECOND_HDD_PORT_NUM, &tHddInfo);
	else if (ucHddPos == 0x2)
	{
		eFdiErr = FDI_HDD_GetPartitonInfo(ATA_THIRD_HDD_PORT_NUM, &tHddInfo);
	}

	if( eFdiErr != FAI_NO_ERR )
	{
		goto TEST_FAIL;
	}

	switch(ucInfoType)
	{
		case 0:		// model name
			aucResult[0] = 0x01;
			aucResult[1] = 0x01;

			totalLength = strlen(tHddInfo.aucModelName);
			
			for(count = 1; count < totalLength; count++)
			{
				if(tHddInfo.aucModelName[count]== 0x20)
				{
					tHddInfo.aucModelName[count] = 0;
					break;
				}
			}
			
			aucResult[2] = strlen(tHddInfo.aucModelName);
			strncpy(&aucResult[3], tHddInfo.aucModelName, strlen(tHddInfo.aucModelName));
			
			numParam=2;
			length = 3+aucResult[2];
			break;
		case 1:		// size
			aucResult[0] = 0x01;
			aucResult[1] = 0x00;
			aucResult[2] = 0x03;
			aucResult[3] = ((0xFF0000&tHddInfo.ulSizeMB)>>16);
			aucResult[4] = ((0x00FF00&tHddInfo.ulSizeMB)>>8);
			aucResult[5] = (0x0000FF&tHddInfo.ulSizeMB);
			numParam=2;
			length = 3+aucResult[2];
			break;
		case 2:		// firmware
			aucResult[0] = 0x01;
			aucResult[1] = 0x01;
			aucResult[2] = FDI_HDD_FIRMWARE_VER_SIZE;
			strncpy(&aucResult[3], tHddInfo.aucFirmwareVer, FDI_HDD_FIRMWARE_VER_SIZE);
			numParam=2;
			length = 3+aucResult[2];
			break;
		case 3:		// serial
			aucResult[0] = 0x01;
			aucResult[1] = 0x01;
			VK_MEM_Memset(tmpSN,0,255);
			strncpy(tmpSN, tHddInfo.aucSerialNo, FDI_HDD_SERIAL_NO_SIZE);
			length=strlen(tmpSN);
			for(count = 0; count < length; count++)
			{
				if(tmpSN[count] != 0x20)
				{
					indicator++;
					
				}
				else
				{
					break;
				}
			}
			if((indicator > 0)&&(length>indicator))/* find space */
			{
				aucResult[2] = indicator; /* length of param */
				VK_MEM_Memcpy(&aucResult[3],tmpSN,indicator);
			}
			else
			{
				aucResult[2] = length;
				strncpy(&aucResult[3], tmpSN, length);
			}
			
			numParam=2;
			length = 3+aucResult[2];
			break;
#if(PRODUCT_YSR2000==YES)
		case 4: /* partition info */
			aucResult[0] = 0x01;
			aucResult[1] = 0x01;
			//aucResult[2] = 0x04;
			//aucResult[3] = ((tHddInfo.ulNumPartitions & 0xFF000000)>>24);
			//aucResult[4] = ((tHddInfo.ulNumPartitions & 0x00FF0000)>>16);
			//aucResult[5] = ((tHddInfo.ulNumPartitions & 0x0000FF00)>>8);
			//aucResult[6] = ((tHddInfo.ulNumPartitions & 0x000000FF));
			numParam = 2;
			//length = 3 + aucResult[2];
			length = 3;
			currPos = 2;
		
			FDI_Print("Num partition = %u\n", tHddInfo.ulNumPartitions);
		
			for (cnt = 0; cnt < tHddInfo.ulNumPartitions; cnt++)
			{
				FDI_Print("[%s] ullPartitionSize(%d)\n", __func__, tHddInfo.ullPartitionSize[cnt]);
		
				memset(aucHDDSize, 0x0, 20);
				snprintf(aucHDDSize, 20, "%llu", tHddInfo.ullPartitionSize[cnt]);
				partStrSize = strlen(aucHDDSize);
				if ((currPos + (partStrSize + 1)) > FDI_HDD_INFO_MAX_SIZE)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_GetInfo] : Str buffer is full.\n"));
					break;
				}
				aucResult[currPos++] = partStrSize;
				strncpy(&aucResult[currPos], aucHDDSize, partStrSize);
				currPos += partStrSize;
				numParam++;
				length += (partStrSize + 1);
			}
			aucResult[currPos++] = 0x01;
			aucResult[currPos++] = 0x30;
			break;
#else			
#if ((CONFIG_PROD_HDR3000T==YES) || (CONFIG_PROD_HDR4000T==YES) || (PRODUCT_HMSLITE==YES))
		case 4: /* partition info */
			aucResult[0] = 0x01;
			aucResult[1] = 0x01;
			numParam = 1;
			length = 2;
			currPos = 2;
		
			FDI_Print("Num partition = %u\n", tHddInfo.ulNumPartitions);
		
			for (cnt = 0; cnt < tHddInfo.ulNumPartitions; cnt++)
			{
				FDI_Print("[%s] ullPartitionSize(%d)\n", __func__, tHddInfo.ullPartitionSize[cnt]);
		
				memset(aucHDDSize, 0x0, 20);
				snprintf(aucHDDSize, 20, "%llu", tHddInfo.ullPartitionSize[cnt]);
				partStrSize = strlen(aucHDDSize);
				if ((currPos + (partStrSize + 1)) > FDI_HDD_INFO_MAX_SIZE)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_GetInfo] : Str buffer is full.\n"));
					break;
				}
				aucResult[currPos++] = partStrSize;
				strncpy(&aucResult[currPos], aucHDDSize, partStrSize);
				currPos += partStrSize;
				numParam++;
				length += (partStrSize + 1);
			}
			break;
#else			
		case 4: /* partition info */
			aucResult[0] = 0x01;
			aucResult[1] = 0x01;
			aucResult[2] = 0x04;
			aucResult[3] = ((tHddInfo.ulNumPartitions & 0xFF000000)>>24);
			aucResult[4] = ((tHddInfo.ulNumPartitions & 0x00FF0000)>>16);
			aucResult[5] = ((tHddInfo.ulNumPartitions & 0x0000FF00)>>8);
			aucResult[6] = ((tHddInfo.ulNumPartitions & 0x000000FF));
			numParam = 2;
			length = 3 + aucResult[2];
			currPos = 7;
		
			FDI_Print("Num partition = %u\n", tHddInfo.ulNumPartitions);
		
			for (cnt = 0; cnt < tHddInfo.ulNumPartitions; cnt++)
			{
				FDI_Print("[%s] ullPartitionSize(%d)\n", __func__, tHddInfo.ullPartitionSize[cnt]);
		
				memset(aucHDDSize, 0x0, 20);
				snprintf(aucHDDSize, 20, "%llu", tHddInfo.ullPartitionSize[cnt]);
				partStrSize = strlen(aucHDDSize);
				if ((currPos + (partStrSize + 1)) > FDI_HDD_INFO_MAX_SIZE)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_GetInfo] : Str buffer is full.\n"));
					break;
				}
				aucResult[currPos++] = partStrSize;
				strncpy(&aucResult[currPos], aucHDDSize, partStrSize);
				currPos += partStrSize;
				numParam++;
				length += (partStrSize + 1);
			}
			break;
#endif			
#endif
		default:
			break;
	}
	FAI_TMD_SetTestResult(0x08, 0x03, 0x03, ucHddPos, FAI_TMD_TEST_NO_ERROR, numParam, aucResult, length);
	return FAI_NO_ERR;

TEST_FAIL:
	aucResult[0] = 0x01;
	aucResult[1] = 0x01;
	aucResult[2] = 0x00;
	aucResult[3] = 2;	/* length of param */
	aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	/* result - error code high */
	aucResult[5] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF;		/* result - error code low */
	FAI_TMD_SetTestResult(0x08, 0x03, 0x03, ucHddPos, FAI_TMD_TEST_ERROR, 1+1, aucResult, 3+FAI_TMD_ERROR_PARM_LENGTH);
	
	return FAI_ERR;
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
#if (HDD_SET_PARKING_ENABLE==YES)
FAI_ERR_CODE FAI_HDD_SetParking(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_HDD_MSG_t tHddMsg;
	unsigned char ucHddPosition;
	unsigned char ucHddInfo;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_SetParking] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 4;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	FAI_TMD_GetParam(&ucHddPosition, pucArgList, 0);
	FAI_TMD_GetParam(&ucHddInfo, pucArgList, 1);

	tHddMsg.cmd_arg.GetInfo.HddPosition = (FAI_HDD_POSITION_e)ucHddPosition;
	tHddMsg.cmd_arg.GetInfo.HddInfo 	= (FAI_HDD_INFO_e)ucHddInfo;

    tHddMsg.nCmd = 0x04;   /* 0x03 means HDD parking Test */
	eFvkErr = FVK_MSG_Send(g_ulHddMsgId, &tHddMsg, sizeof(FAI_HDD_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_SetParking] : Error FVK_MSG_Send() : %d\n", eFvkErr));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 4;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

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
#if (HDD_DEFECT_ENABLE==YES)
FAI_ERR_CODE FAI_HDD_Defect(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_HDD_MSG_t tHddMsg;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Defect] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 5;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

    tHddMsg.nCmd = 0x05;   /* 0x05 means HDD DEFECT Test */
	eFvkErr = FVK_MSG_Send(g_ulHddMsgId, &tHddMsg, sizeof(FAI_HDD_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Defect] : Error FVK_MSG_Send() : %d\n", eFvkErr));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 5;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}

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
#if (HDD_DEFECT_ENABLE==YES)
FAI_ERR_CODE FAI_HDD_ReadPOH(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_HDD_MSG_t tHddMsg;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_ReadPOH] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 6;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

    tHddMsg.nCmd = 0x06;   /* 0x06 means HDD DEFECT Test */
	eFvkErr = FVK_MSG_Send(g_ulHddMsgId, &tHddMsg, sizeof(FAI_HDD_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_ReadPOH] : Error FVK_MSG_Send() : %d\n", eFvkErr));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 6;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

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
#if (HDD_SMART_SHORTDST_ENABLE==YES)
FAI_ERR_CODE FAI_HDD_Smart_ShortDST(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_HDD_MSG_t tHddMsg;
	FAI_ERR_CODE eFaiErr;

	FAI_HDD_TMD_ERR stHddError = FAI_HDD_TMD_ERR_NO;

	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Smart_ShortDST] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 7;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
#if 0
	eFaiErr = INTRN_FAI_HDD_SmartShortDST(&stHddError);
	if(eFaiErr != FAI_NO_ERR)
	{
		 FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Smart_ShortDST] : Error FVK_MSG_Send() : %d\n", eFvkErr));

		 tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		 tTMDResult.ucModule = 3;
		 tTMDResult.ucAction = 7;
		 tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		 ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDD;
		 ucTmdErrCode[1] = (unsigned char)stHddError;
		 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		 FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;

	}
#else
    tHddMsg.nCmd = 0x07;   /* 0x07 means HDD Short DST Test */
	eFvkErr = FVK_MSG_Send(g_ulHddMsgId, &tHddMsg, sizeof(FAI_HDD_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Smart_ShortDST] : Error FVK_MSG_Send() : %d\n", eFvkErr));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 3;
		tTMDResult.ucAction = 7;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = 0;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
       return FAI_ERR;
   }
#endif
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
#if (HDD_UI_ENABLE==YES)
FAI_ERR_CODE FAI_HDD_Test_UI(void)
	{
		FAI_ERR_CODE eFdiErr;
		FDI_HDD_INFO_t tHddInfo;
		unsigned char aucHDDSizeStr[FDI_HDD_MODEL_NAME_SIZE];
		unsigned char aucHDDName[FDI_HDD_MODEL_NAME_SIZE];
		unsigned char aucHDDSNStr[FDI_HDD_SERIAL_NO_SIZE];
		unsigned char ucHDDNameLength = 0;
		unsigned char ucHDDSizeStrLength = 0;
		unsigned char ucHDDSNStrLength = 0;
		unsigned char i;
	

	if( GET_STEP() == 0 )
	{

		SET_STEP(1);
		/* Display hdd test start */
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HDD Test", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FVK_TASK_Sleep( (unsigned long)1000);
		FAI_PRINT(FAI_PRT_ERROR, ("Line(%d)\n", __LINE__));
	
		//eFdiErr = FDI_HDD_GetInfo(&tHddInfo);
		eFdiErr = FDI_HDD_GetPartitonInfo(ATA_FIRST_HDD_PORT_NUM, &tHddInfo);
		if( eFdiErr != FAI_NO_ERR )
		{
			goto TEST_FAIL;
		}
		
		ucHDDNameLength = strlen(tHddInfo.aucModelName);
	
		/* remove space of sn */
		for(i=0;i<FDI_HDD_SERIAL_NO_SIZE;i++)
		{
			if(tHddInfo.aucSerialNo[i] != 0x20)
			{
				break;
			}
		}
	
		tHddInfo.aucSerialNo[FDI_HDD_SERIAL_NO_SIZE] = '\0';
		
		ucHDDSNStrLength = strlen(tHddInfo.aucSerialNo) - i;
	
		memset(aucHDDName, 0x00, FDI_HDD_MODEL_NAME_SIZE);
		memset(aucHDDSizeStr, 0x00, FDI_HDD_MODEL_NAME_SIZE);
		memset(aucHDDSNStr, 0x00, FDI_HDD_SERIAL_NO_SIZE);
	
		strncpy(aucHDDName, (char *)(tHddInfo.aucModelName), ucHDDNameLength);
		strncpy(aucHDDSNStr, (char *)(&(tHddInfo.aucSerialNo[i])), ucHDDSNStrLength);
		sprintf(aucHDDSizeStr, "Size %d GB", (tHddInfo.ulSizeMB)/1000);
	
	
	
		if(ucHDDNameLength <FDI_HDD_MODEL_NAME_SIZE)
		{
			aucHDDName[ucHDDNameLength] ='\0';
		}
		else
		{
			aucHDDName[FDI_HDD_MODEL_NAME_SIZE - 1] ='\0';
		}

		aucHDDName[12] ='\0'; //consiering OSD Cell Size.
	
		ucHDDSizeStrLength = strlen(aucHDDSizeStr);
	
		if(ucHDDSizeStrLength < FDI_HDD_MODEL_NAME_SIZE)
		{
			aucHDDSizeStr[ucHDDSizeStrLength] ='\0';
		}
		else
		{
			aucHDDSizeStr[FDI_HDD_MODEL_NAME_SIZE - 1] ='\0';
		}
	
		if(ucHDDSNStrLength < FDI_HDD_SERIAL_NO_SIZE)
		{
			aucHDDSNStr[ucHDDSNStrLength] ='\0';
		}
		else
		{
			aucHDDSNStr[FDI_HDD_SERIAL_NO_SIZE] ='\0';
		}
	
		FAI_PRINT(FAI_PRT_ERROR, ("Line(%d), HDDName [%s], HDDSizeStr [%s], HDDSN [%s]\n", __LINE__, aucHDDName, aucHDDSizeStr, aucHDDSNStr));
		FAI_PRINT(FAI_PRT_ERROR, ("Line(%d), HDDNameLength [%d], HDDSizeStrLength [%d],  HDDSN_Len [%d]\n", __LINE__, ucHDDNameLength, ucHDDSizeStrLength, ucHDDSNStrLength));
	
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, (char*)aucHDDName, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, (char*)aucHDDSizeStr, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, (char*)aucHDDSNStr, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	
		FDI_PANEL_Display(FAI_HDD_TEST_PASS_STR);
		FAI_PRINT(FAI_PRT_ERROR, ("Line(%d)\n", __LINE__));
	}
	#if 0	//second step function need implimentation.
	else if( GET_STEP() == 1 )
	{
		FDI_HDD_INFO_t tHddInfo;
		char tempText[20];

		SET_STEP(0);

		memset(&tHddInfo, 0x0, sizeof(FDI_HDD_INFO_t));

		eFdiErr = FDI_HDD_GetPartitonInfo(ATA_FIRST_HDD_PORT_NUM, &tHddInfo);
		if( eFdiErr != FAI_NO_ERR )
		{
			goto TEST_FAIL;
		}

		memset(tempText, 0x0, 20);
		sprintf(tempText, "PartNum(%d)\0", (tHddInfo.ulNumPartitions));
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, (char *)tempText, RGB_COLOR_BLACK);
		memset(tempText, 0x0, 20);
		sprintf(tempText, "0: %06dMB\0", (tHddInfo.ullPartitionSize[0]/1000));

		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, (char *)tempText, RGB_COLOR_BLACK);
		memset(tempText, 0x0, 20);
		sprintf(tempText, "1: %06dMB\0", (tHddInfo.ullPartitionSize[1]/1000));
		
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, (char *)tempText, RGB_COLOR_BLACK);
		memset(tempText, 0x0, 20);
		sprintf(tempText, "2: %06dMB\0", (tHddInfo.ullPartitionSize[2]/1000));

		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, (char *)tempText, RGB_COLOR_BLACK);

		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	}
	#else
	SET_STEP(0);
	#endif
	
		return FAI_NO_ERR;
	
	TEST_FAIL:
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HDD Test", RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
		FDI_PANEL_Display(FAI_HDD_TEST_FAIL_STR);
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
void INTRN_FAI_HDD_Task(void *pData)
{
	FAI_HDD_MSG_t tHddMsg;
	FVK_ERR_CODE eFvkErr;
	FAI_ERR_CODE eFdiErr, eFaiErr;

   	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	BOOL bHDDConnect = FALSE;

	FAI_HDD_TMD_ERR stHddError = FAI_HDD_TMD_ERR_NO;

	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDD;

	while(1)
	{
		eFvkErr = FVK_MSG_Receive(g_ulHddMsgId, &tHddMsg, sizeof(FAI_HDD_MSG_t));
		if (eFvkErr != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_HDD_Task] : Error FVK_MSG_Receive() %d\n", eFvkErr));
			continue;
		}

		FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

		FDI_HDD_GetStatus(&bHDDConnect);
		if(bHDDConnect == FALSE)
		{
			tTMDResult.ucCategory = 0x08;
			tTMDResult.ucModule = 0x03;
			tTMDResult.ucAction = tHddMsg.nCmd;
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;

			ucTmdErrCode[1] = FAI_HDD_TMD_ERR_DISCONNECT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

			FAI_TMD_SetResult(&tTMDResult);
			continue;
		}
        if( tHddMsg.nCmd == 0x01 )     /* 0x01 means HDD FOrmat command */
		{
  			if( INTRN_FAI_HDD_Format(tHddMsg.cmd_arg.Format.HddPosition, tHddMsg.cmd_arg.Format.HddFormatType,&stHddError) != FAI_NO_ERR )
			{
			     /* Fail 인 경우 */
                 tTMDResult.ucCategory = 0x08;
                 tTMDResult.ucModule = 0x03;
                 tTMDResult.ucAction = tHddMsg.nCmd;
                 tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				 ucTmdErrCode[1] = (unsigned char)stHddError;

				 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

                 FAI_TMD_SetResult(&tTMDResult);
				 FAI_PRINT(FAI_PRT_ERROR, ("[HDD FORMAT FAIL]\n"));
			}
			else
			{
			     /* Pass 인 경우 */
                tTMDResult.ucCategory = 0x08;
                tTMDResult.ucModule = 0x03;
                tTMDResult.ucAction = tHddMsg.nCmd;
                tTMDResult.eResult = FAI_TMD_ERR_OK;

				stHddError = FAI_HDD_TMD_ERR_NO;

                FAI_TMD_SetResult(&tTMDResult);
				 FAI_PRINT(FAI_PRT_ERROR, ("[HDD FORMAT OK]\n"));
			}
		}
        else if( tHddMsg.nCmd == 0x02 )     /* 0x02 means HDD Read/Write Compare command */
		{
  			if( INTRN_FAI_HDD_DataCompare(tHddMsg.cmd_arg.DataComp.HddPosition,&stHddError) != FAI_NO_ERR )
			{
			     /* Fail 인 경우 */
                 tTMDResult.ucCategory = 0x08;
                 tTMDResult.ucModule = 0x03;
                 tTMDResult.ucAction = tHddMsg.nCmd;
                 tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				 ucTmdErrCode[1] = (unsigned char)stHddError;

				 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

                 FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
			     /* Pass 인 경우 */
                tTMDResult.ucCategory = 0x08;
                tTMDResult.ucModule = 0x03;
                tTMDResult.ucAction = tHddMsg.nCmd;
                tTMDResult.eResult = FAI_TMD_ERR_OK;

				stHddError = FAI_HDD_TMD_ERR_NO;

                FAI_TMD_SetResult(&tTMDResult);
			}
		}
        else if( tHddMsg.nCmd == 0x03 )     /* 0x03 means GetINfo command */
		{
             unsigned char aucResult[FDI_HDD_INFO_MAX_SIZE+3];
             unsigned char aucParamType =0;  /* 0 - Interger, 1- string */
			 unsigned char aucResultInfo[2];

#if ((CONFIG_PROD_HDR3000T==YES) || (CONFIG_PROD_HDR4000T==YES) || (PRODUCT_HMSLITE==YES))
			int n=0;
#if ((CONFIG_PROD_HDR3000T==YES) || (PRODUCT_HMSLITE==YES))
			if(tHddMsg.cmd_arg.GetInfo.HddInfo==PARTITION_INFO)
			{
				while(1)
				{
					if (s_bEncryptedPartitionOpened == FALSE)
					{
						/* Open encrypted partition */
						eFdiErr = FDI_HDD_Encrypted_Open(ATA_FIRST_HDD_PORT_NUM, 3);		/* 1st port, 4th partition */
						if (eFdiErr != FAI_NO_ERR)
						{
							FAI_PRINT(FAI_PRT_ERROR, ("[%s] FDI_HDD_Encrypted_Open error(0x%x)\n", __func__, eFdiErr));
						}
						else
						{
							FDI_Print("[%s] Open encrypted partition, portnum(ATA_FIRST_HDD_PORT_NUM) partIdx(3)\n", __func__);
							s_bEncryptedPartitionOpened = TRUE;
							break;
						}
						sleep(100);
					}
					else if (n > 100)
					{
						FDI_Print("[%s] Encrypted partition open time out!\n", __func__);
						break;
					}
					{
						FDI_Print("[%s] Encrypted partition alreay opened\n", __func__);
						break;
					}
					n++;
				}

			}
#endif
  			if( INTRN_FAI_HDD_GetInfo(tHddMsg.cmd_arg.GetInfo.HddPosition, tHddMsg.cmd_arg.GetInfo.HddInfo,aucResult,&stHddError,aucResultInfo) != FAI_NO_ERR )
  			{
				aucResult[0] = 0x01;
				aucResult[1] = 0x01;
				aucResult[2] = 0x00;
				aucResult[3] = 2;	/* length of param */
				aucResult[4] = 0x12;	/* result - error code high */
				aucResult[5] = 0x06; 	/* result - error code low */
				FAI_TMD_SetTestResult(0x08, 0x03, 0x03, tHddMsg.cmd_arg.GetInfo.HddPosition, FAI_TMD_TEST_ERROR, 1+1, aucResult, 3+FAI_TMD_ERROR_PARM_LENGTH);
  			}
			else
			{
				FAI_TMD_SetTestResult(0x08, 0x03, 0x03, tHddMsg.cmd_arg.GetInfo.HddPosition, FAI_TMD_TEST_NO_ERROR, aucResultInfo[0], aucResult, aucResultInfo[1]);
			}


#else
  			if( INTRN_FAI_HDD_GetInfo(tHddMsg.cmd_arg.GetInfo.HddPosition, tHddMsg.cmd_arg.GetInfo.HddInfo,aucResult,&stHddError) != FAI_NO_ERR )
			{
			     /* Fail 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = tHddMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				ucTmdErrCode[1] = (unsigned char)stHddError;

				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

				FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
				/* Pass 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = tHddMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_OK;

				stHddError = FAI_HDD_TMD_ERR_NO;

				//FAI_TMD_AddResultParam(&tTMDResult, &tHddMsg.cmd_arg.GetInfo.HddPosition, sizeof(tHddMsg.cmd_arg.GetInfo.HddPosition));
              			  //FAI_TMD_AddResultParam(&tTMDResult, &tHddMsg.cmd_arg.GetInfo.HddInfo, sizeof(tHddMsg.cmd_arg.GetInfo.HddInfo));
		                if(aucResult[0] == MODELNAME)
		                {
					aucParamType = 1;  /* 0 interger, 1 string */
					DI_UART_Dump(&aucResult[2], aucResult[1],16);
					FAI_TMD_AddResultParam(&tTMDResult, &aucParamType, sizeof(aucParamType));
					FAI_TMD_AddResultParam(&tTMDResult, &aucResult[2], aucResult[1]); /* aucResult[1] 은 aucRestul[2] 부터의 size */
		                }
		                else if(aucResult[0] == SIZE)
		                {
					aucParamType = 0;  /* 0 interger, 1 string */
					DI_UART_Dump(&aucResult[2], aucResult[1],16);
					FAI_TMD_AddResultParam(&tTMDResult, &aucParamType, sizeof(aucParamType));
					FAI_TMD_AddResultParam(&tTMDResult, &aucResult[2], aucResult[1]); /* aucResult[1] 은 aucRestul[2] 부터의 size */
		                }
		                else if(aucResult[0]== FIRMWARE_VERSION)
		                {
					aucParamType = 1;  /* 0 interger, 1 string */
					DI_UART_Dump(&aucResult[2], aucResult[1],16);
					FAI_TMD_AddResultParam(&tTMDResult, &aucParamType, sizeof(aucParamType));
					FAI_TMD_AddResultParam(&tTMDResult, &aucResult[2], aucResult[1]); /* aucResult[1] 은 aucRestul[2] 부터의 size */
		                }
				else if(aucResult[0] == SERIAL_NUMBER)
				{
					aucParamType = 1;  /* 0 interger, 1 string */
					DI_UART_Dump(&aucResult[2], aucResult[1],16);
					FAI_TMD_AddResultParam(&tTMDResult, &aucParamType, sizeof(aucParamType));
					FAI_TMD_AddResultParam(&tTMDResult, &aucResult[2], aucResult[1]); /* aucResult[1] 은 aucRestul[2] 부터의 size */
				}
				else if(aucResult[0] == PARTITION_INFO)
				{
					aucParamType = 1;  /* 0 interger, 1 string */
					DI_UART_Dump(&aucResult[2], aucResult[1],16);
					FAI_TMD_AddResultParam(&tTMDResult, &aucParamType, sizeof(aucParamType));
					FAI_TMD_AddResultParam(&tTMDResult, &aucResult[2], aucResult[1]); /* aucResult[1] 은 aucRestul[2] 부터의 size */
					if(aucResult[2+aucResult[1]])
					{
						FAI_TMD_AddResultParam(&tTMDResult, &aucResult[2+aucResult[1]+1], aucResult[2+aucResult[1]]);
					}

					if(aucResult[2+aucResult[1]+aucResult[2+aucResult[1]]+1])
					{
						FAI_TMD_AddResultParam(&tTMDResult, &aucResult[2+aucResult[1]+aucResult[2+aucResult[1]]+2], aucResult[2+aucResult[1]+aucResult[2+aucResult[1]]+1]);
					}
				}
		                FAI_TMD_SetResult(&tTMDResult);
			}
#endif
		}
        else if( tHddMsg.nCmd == 0x04 )     /* 0x04 means HDD Set Parking command */
		{
  			if( INTRN_FAI_HDD_SetParking(tHddMsg.cmd_arg.SetPark.HddPosition,tHddMsg.cmd_arg.SetPark.bParkOnOff,&stHddError) != FAI_NO_ERR )
			{
			     /* Fail 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = tHddMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[1] = (unsigned char)stHddError;

				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

				FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
			     /* Pass 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = tHddMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_OK;

				stHddError = FAI_HDD_TMD_ERR_NO;

				FAI_TMD_SetResult(&tTMDResult);
			}
		}
        else if( tHddMsg.nCmd == 0x05 )     /* 0x05 means HDD DEFECT command */
		{
		    int nDefectCnt =0;

  			if( INTRN_FAI_HDD_Defect(&nDefectCnt,&stHddError) != FAI_NO_ERR )
			{
			     /* Fail 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = tHddMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				ucTmdErrCode[1] = (unsigned char)stHddError;

				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

				FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
			     /* Pass 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = tHddMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_OK;

				stHddError = FAI_HDD_TMD_ERR_NO;

				FAI_TMD_AddResultParam(&tTMDResult, &nDefectCnt, sizeof(nDefectCnt));

				FAI_TMD_SetResult(&tTMDResult);
			}
		}
        else if( tHddMsg.nCmd == 0x06 )     /* 0x06 means HDD POH READ command */
		{
		    int nReadTime=0;

  			if( INTRN_FAI_HDD_ReadPOH(&nReadTime,&stHddError) != FAI_NO_ERR )
			{
			     /* Fail 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = tHddMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				ucTmdErrCode[1] = (unsigned char)stHddError;

				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

				FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
			     /* Pass 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = tHddMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_OK;

				stHddError = FAI_HDD_TMD_ERR_NO;

				FAI_TMD_AddResultParam(&tTMDResult, &nReadTime, sizeof(nReadTime));

				FAI_TMD_SetResult(&tTMDResult);
			}
		}
		else if( tHddMsg.nCmd == 0x07 )
		{
			if(INTRN_FAI_HDD_SmartShortDST(&stHddError) != FAI_NO_ERR)
			{
				 FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Smart_ShortDST] : Error FVK_MSG_Send() : %d\n", eFvkErr));
			
				 tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
				 tTMDResult.ucModule = 3;
				 tTMDResult.ucAction = tHddMsg.nCmd;
				 tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				 ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDD;
				 ucTmdErrCode[1] = (unsigned char)stHddError;
				 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			
				 FAI_TMD_SetResult(&tTMDResult);
				return;
			
			}
		}
        else if( tHddMsg.nCmd == 0x17 )     /* 0x17 means HDD SMART ShortDST command */
		{
			unsigned char ucHddDeviceName[10];
			VK_TIMER_Cancel(ulHddDstTimerId);
  			if( tHddMsg.nResult == 0 )
			{
			     /* Pass 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = 7;
				tTMDResult.eResult = FAI_TMD_ERR_OK;

				stHddError = FAI_HDD_TMD_ERR_NO;

				FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
				/* Fail 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x03;
				tTMDResult.ucAction = 7;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				ucTmdErrCode[1] = (unsigned char)FAI_HDD_TMD_ERR_DSTSTS_FAIL;

				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

				FAI_TMD_SetResult(&tTMDResult);
			}
			FDI_HDD_GetDeviceName(ucHddDeviceName);
			FDI_HDD_DSTClose(ucHddDeviceName);
		}
	}
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_HDD_Format(FAI_HDD_POSITION_e HddPos, FAI_HDD_FORMAT_TYPE_e HddFormatType, FAI_HDD_TMD_ERR *pHddError)
{
     /* 여기 채워야함 */
	 FDI_FS ePartitiontype;
	 FAI_ERR_CODE eFdiErr;
	#define PART_NUM	3
	HUINT64 ulDefinedPartSize[PART_NUM];

	
	 switch(HddPos)
 	{
 		case 0x00:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : HDD Type: Internal\n"));
			break;
		case 0x01:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : HDD Type: External\n"));
			break;
		case 0x02:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : HDD Type: Detattchable\n"));
			break;
		default:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : HDD Type: Unknown\n"));
			break;
 	}

	 switch(HddFormatType)
 	{
 		case 0x00:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : Format Type: FAT16\n"));
			ePartitiontype=4;
			break;
		case 0x01:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : Format Type: FAT32\n"));
			ePartitiontype=4;			
			break;
		case 0x02:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : Format Type: NTFS\n"));
			ePartitiontype=6;			
			break;			
		case 0x03:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : Format Type: EXT2\n"));
			ePartitiontype=4;
			break;			
		case 0x04:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : Format Type: EXT3\n"));
			ePartitiontype=1;
			break;			
		case 0x05:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : Format Type: HUMAX_FS\n"));
			ePartitiontype=0xFF; /* TODO: DI implementation for HUMAX_FS*/
			break;			
		case 0x06:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : Format Type: CHIP_VENDOR_FS\n"));
			ePartitiontype=0xFF; /* TODO: DI implementation for CHIP_VENDOR_FS*/
			break;			
		case 0x07:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : Format Type: EXT4\n"));
			ePartitiontype=2;
			break;			
		default:
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_Format] : Format Type: UNKNOWN\n"));
			ePartitiontype=0xFF;
			break;						
 	}	
#if (HDD_FORMAT_ENABLE==YES)
#if (PRODUCT_YSR2000==YES)
	ulDefinedPartSize[0]=1044225;
	ulDefinedPartSize[1]=965209297;
	ulDefinedPartSize[2]=10482412;	 
#else
	#error "TODO: MUST DEFINE HDD PARTITON"
#endif
	FAI_PRINT(FAI_PRT_ERROR, ("[FDI_HDD_Format] : Format Start....\n"));

	eFdiErr = FDI_HDD_Format(0/* fix value*/, PART_NUM , ulDefinedPartSize ,ePartitiontype);
	if(eFdiErr!=FDI_NO_ERR)
	{
		return FAI_ERR;
	}
#endif

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
FAI_ERR_CODE INTRN_FAI_HDD_DataCompare(FAI_HDD_POSITION_e HddPosition, FAI_HDD_TMD_ERR *pHddError)
{
	int i;
	unsigned long ulReadSize, ulWriteSize;
	unsigned char aucWriteBuf[HDD_SECTOR_SIZE], aucReadBuf[HDD_SECTOR_SIZE];

	/* generate write data */
	for( i = 0; i < HDD_SECTOR_SIZE; i++ )
	{
		aucWriteBuf[i] = i%10;
	}

	/* hdd open */
	if( FDI_HDD_Open() != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Test] : FDI_HDD_Open Error!!\n"));
		*pHddError = FAI_HDD_TMD_ERR_OPEN_FAIL;

		return FAI_ERR;
	}

	/* write data */
	ulWriteSize = HDD_SECTOR_SIZE;
	if( FDI_HDD_Write(0, aucWriteBuf, &ulWriteSize) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Test] : FDI_HDD_Write Error!!\n"));
		*pHddError = FAI_HDD_TMD_ERR_WRITE_FAIL;

		return FAI_ERR;
	}
	/* read data */
	ulReadSize = HDD_SECTOR_SIZE;
	if( FDI_HDD_Read(0, aucReadBuf, &ulReadSize) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Test] : FDI_HDD_Read Error!!\n"));
		*pHddError = FAI_HDD_TMD_ERR_READ_FAIL;

		return FAI_ERR;
	}

	/* verify */
	for( i = 0; i < HDD_SECTOR_SIZE; i++ )
	{
		if( aucWriteBuf[i] != aucReadBuf[i] )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Test] : Verification Error!!\n"));
			*pHddError = FAI_HDD_TMD_ERR_VALID_FAIL;

			return FAI_ERR;
		}
	}

	/* erase writed data */
	ulWriteSize = HDD_SECTOR_SIZE;
	FVK_MEM_Set( (void *)aucWriteBuf, 0x00, HDD_SECTOR_SIZE);
	if( FDI_HDD_Write(0, aucWriteBuf, &ulWriteSize) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Test] : FDI_HDD_Write Error!!\n"));
		*pHddError = FAI_HDD_TMD_ERR_WRITE_FAIL;

		return FAI_ERR;
	}

	/* close hdd */
	if( FDI_HDD_Close() != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_Test] : FDI_HDD_Close Error!!\n"));
		*pHddError = FAI_HDD_TMD_ERR_CLOSE_FAIL;

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
#if ((CONFIG_PROD_HDR3000T==YES) || (CONFIG_PROD_HDR4000T==YES) || (PRODUCT_HMSLITE==YES))
FAI_ERR_CODE INTRN_FAI_HDD_GetInfo(FAI_HDD_POSITION_e HddPosition, FAI_HDD_INFO_e HddInfo, unsigned char *pucResult, FAI_HDD_TMD_ERR *pHddError, unsigned char *pucResultInfo)
{
	FAI_ERR_CODE eFdiErr;
	FDI_HDD_INFO_t tHddInfo;
	unsigned char aucHDDSize[20];
	unsigned char numParam=2;
	unsigned char length;
	int cnt, currPos=0;
	unsigned int partStrSize;

	unsigned char tmpSN[255] = {0,};
	unsigned int totalLength = 0,count = 0, minuscount = 0,indicator = 0;

	if (HddPosition == 0x0)
		eFdiErr = FDI_HDD_GetPartitonInfo(ATA_FIRST_HDD_PORT_NUM, &tHddInfo);
	else if (HddPosition == 0x1)
		eFdiErr = FDI_HDD_GetPartitonInfo(ATA_SECOND_HDD_PORT_NUM, &tHddInfo);
	else if (HddPosition == 0x2)
	{
		eFdiErr = FDI_HDD_GetPartitonInfo(ATA_THIRD_HDD_PORT_NUM, &tHddInfo);
	}

	if( eFdiErr != FAI_NO_ERR )
	{
		return FAI_ERR;
	}

	switch(HddInfo)
	{
		case MODELNAME:		// model name
			pucResult[0] = 0x01;
			pucResult[1] = 0x01;

			totalLength = strlen(tHddInfo.aucModelName);
			
			for(count = 1; count < totalLength; count++)
			{
				if(tHddInfo.aucModelName[count]== 0x20)
				{
					tHddInfo.aucModelName[count] = 0;
					break;
				}
			}
			
			pucResult[2] = strlen(tHddInfo.aucModelName);
			strncpy(&pucResult[3], tHddInfo.aucModelName, strlen(tHddInfo.aucModelName));
			
			numParam=2;
			length = 3+pucResult[2];
			break;
		case SIZE:		// size
			pucResult[0] = 0x01;
			pucResult[1] = 0x00;
			pucResult[2] = 0x03;
			pucResult[3] = ((0xFF0000&tHddInfo.ulSizeMB)>>16);
			pucResult[4] = ((0x00FF00&tHddInfo.ulSizeMB)>>8);
			pucResult[5] = (0x0000FF&tHddInfo.ulSizeMB);
			numParam=2;
			length = 3+pucResult[2];
			break;
		case FIRMWARE_VERSION:		// firmware
			pucResult[0] = 0x01;
			pucResult[1] = 0x01;
			pucResult[2] = FDI_HDD_FIRMWARE_VER_SIZE;
			strncpy(&pucResult[3], tHddInfo.aucFirmwareVer, FDI_HDD_FIRMWARE_VER_SIZE);
			numParam=2;
			length = 3+pucResult[2];
			break;
		case SERIAL_NUMBER:		// serial
			pucResult[0] = 0x01;
			pucResult[1] = 0x01;
			VK_MEM_Memset(tmpSN,0,255);
			strncpy(tmpSN, tHddInfo.aucSerialNo, FDI_HDD_SERIAL_NO_SIZE);
			minuscount = FDI_HDD_SERIAL_NO_SIZE;
			for(count = 0; count < FDI_HDD_SERIAL_NO_SIZE; count++)
			{
				if(tmpSN[count] == 0x20)
				{
					minuscount--;
					indicator++;
				}
				else 
				{
					break;
				}
			}

			if(minuscount > 0)
			{
				pucResult[2] = minuscount; /* length of param */
				VK_MEM_Memcpy(&pucResult[3],&tmpSN[indicator],minuscount);
			}
			else
			{
				pucResult[2] = FDI_HDD_SERIAL_NO_SIZE;
				strncpy(&pucResult[3], tHddInfo.aucSerialNo, FDI_HDD_SERIAL_NO_SIZE);

			}

			numParam=2;
			length = 3+pucResult[2];
			break;
		case PARTITION_INFO: /* partition info */
			pucResult[0] = 0x01;
			pucResult[1] = 0x01;
			numParam = 1;
			length = 2;
			currPos = 2;
		
			FDI_Print("Num partition = %u\n", tHddInfo.ulNumPartitions);
#if ((CONFIG_PROD_HDR3000T==YES) || (CONFIG_PROD_HDR4000T==YES) || (PRODUCT_HMSLITE==YES))
			for (cnt = 0; cnt < 4; cnt++)
#else
			for (cnt = 0; cnt < tHddInfo.ulNumPartitions; cnt++)
#endif
			{
				FDI_Print("[%s] ullPartitionSize(%d)\n", __func__, tHddInfo.ullPartitionSize[cnt]);
		
				memset(aucHDDSize, 0x0, 20);
				snprintf(aucHDDSize, 20, "%llu", tHddInfo.ullPartitionSize[cnt]);
				partStrSize = strlen(aucHDDSize);
				if ((currPos + (partStrSize + 1)) > FDI_HDD_INFO_MAX_SIZE)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_GetInfo] : Str buffer is full.\n"));
					break;
				}
				pucResult[currPos++] = partStrSize;
				strncpy(&pucResult[currPos], aucHDDSize, partStrSize);
				currPos += partStrSize;
				numParam++;
				length += (partStrSize + 1);
			}
			break;
		default:
			break;
	}
	pucResultInfo[0]=numParam;
	pucResultInfo[1]=length;
//	FAI_TMD_SetTestResult(0x08, 0x03, 0x03, HddPosition, FAI_TMD_TEST_NO_ERROR, numParam, aucResult, length);
	return FAI_NO_ERR;
#if 0
TEST_FAIL:
	pucResult[0] = 0x01;
	pucResult[1] = 0x01;
	pucResult[2] = 0x00;
	pucResult[3] = 2;	/* length of param */
	pucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	/* result - error code high */
	pucResult[5] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF;		/* result - error code low */
	FAI_TMD_SetTestResult(0x08, 0x03, 0x03, HddPosition, FAI_TMD_TEST_ERROR, 1+1, pucResult, 3+FAI_TMD_ERROR_PARM_LENGTH);
	
	return FAI_ERR;
#endif
}
#else

FAI_ERR_CODE INTRN_FAI_HDD_GetInfo(    FAI_HDD_POSITION_e HddPosition, FAI_HDD_INFO_e HddInfo, unsigned char *pucResult, FAI_HDD_TMD_ERR *pHddError)
	{
		FAI_ERR_CODE eFdiErr;
	
		FDI_HDD_INFO_t tHddInfo;
		unsigned char aucHDDSizeStr[FDI_HDD_MODEL_NAME_SIZE];
		unsigned char aucHDDName[FDI_HDD_MODEL_NAME_SIZE];
		unsigned char aucHDDSNStr[FDI_HDD_SERIAL_NO_SIZE];
		unsigned char ucHDDNameLength = 0;
		unsigned char ucHDDSizeStrLength = 0;
		unsigned char ucHDDSNStrLength = 0;
		unsigned char i;
#if(1)  /*sgkim4, for HDD partition info*/
		int nCurrPos=0;
		unsigned int unPartitionStrSize=0;
#endif
	
		if (HddPosition == INTERNAL_HDD)
			eFdiErr = FDI_HDD_GetPartitonInfo(ATA_FIRST_HDD_PORT_NUM, &tHddInfo);
		else if (HddPosition == EXTERNAL_HDD)
			eFdiErr = FDI_HDD_GetPartitonInfo(ATA_SECOND_HDD_PORT_NUM, &tHddInfo);
		else if (HddPosition == ESATA_HDD)
			eFdiErr = FDI_HDD_GetPartitonInfo(ATA_THIRD_HDD_PORT_NUM, &tHddInfo);

		if( eFdiErr != FAI_NO_ERR )
		{
			*pHddError = FAI_HDD_TMD_ERR_GETINFO_FAIL;
			goto TEST_FAIL;
		}
	
		
		ucHDDNameLength = strlen(tHddInfo.aucModelName);
	
		/* remove space of sn */
		for(i=0;i<FDI_HDD_SERIAL_NO_SIZE;i++)
		{
			if(tHddInfo.aucSerialNo[i] != 0x20)
			{
				break;
			}
		}
		
		tHddInfo.aucSerialNo[FDI_HDD_SERIAL_NO_SIZE] = '\0';
		
		ucHDDSNStrLength = strlen(tHddInfo.aucSerialNo) - i;
	
		memset(aucHDDName, 0x00, FDI_HDD_MODEL_NAME_SIZE);
		strncpy(aucHDDName, (char *)(tHddInfo.aucModelName), ucHDDNameLength);
		strncpy(aucHDDSNStr, (char *)(tHddInfo.aucSerialNo+i), ucHDDSNStrLength);
		sprintf(aucHDDSizeStr, "Size %dGB", (tHddInfo.ulSizeMB)/1000);
	
	
	
		if(ucHDDNameLength <FDI_HDD_MODEL_NAME_SIZE)
		{
			aucHDDName[ucHDDNameLength] ='\0';
		}
		else
		{
			aucHDDName[FDI_HDD_MODEL_NAME_SIZE - 1] ='\0';
		}
	
		ucHDDSizeStrLength = strlen(aucHDDSizeStr);
	
		if(ucHDDSizeStrLength < FDI_HDD_MODEL_NAME_SIZE)
		{
			aucHDDSizeStr[ucHDDSizeStrLength] ='\0';
		}
		else
		{
			aucHDDSizeStr[FDI_HDD_MODEL_NAME_SIZE - 1] ='\0';
		}
	
		if(ucHDDSNStrLength < FDI_HDD_SERIAL_NO_SIZE)
		{
			aucHDDSNStr[ucHDDSNStrLength] ='\0';
		}
		else
		{
			aucHDDSNStr[FDI_HDD_SERIAL_NO_SIZE] ='\0';
		}
	
	
		switch(HddInfo)
		{
			case MODELNAME: 	// model name
				pucResult[0] = HddInfo;
				pucResult[1] = ucHDDNameLength;
				strncpy(&pucResult[2], aucHDDName, ucHDDNameLength);
				break;
				
			case SIZE:		// size
				pucResult[0] = HddInfo;
				pucResult[1] = sizeof(tHddInfo.ulSizeMB);
				pucResult[2] = (unsigned char)((0xFF000000&tHddInfo.ulSizeMB)>>24);;
				pucResult[3] = (unsigned char)((0xFF0000&tHddInfo.ulSizeMB)>>16);
				pucResult[4] = (unsigned char)((0x00FF00&tHddInfo.ulSizeMB)>>8);
				pucResult[5] = (unsigned char)(0x0000FF&tHddInfo.ulSizeMB);
				break;
	
			case FIRMWARE_VERSION:		// firmware
				pucResult[0] = HddInfo;
				pucResult[1] = FDI_HDD_FIRMWARE_VER_SIZE;
				strncpy(&pucResult[2], tHddInfo.aucFirmwareVer, FDI_HDD_FIRMWARE_VER_SIZE);
				break;
	
	
			case SERIAL_NUMBER:
				pucResult[0] = HddInfo;
				pucResult[1] = ucHDDSNStrLength;
				strncpy(&pucResult[2], aucHDDSNStr, ucHDDSNStrLength);
				break;
			case PARTITION_INFO:		/* partition info */
				pucResult[0] = HddInfo;
	
				nCurrPos = 1;
	
				FDI_Print("Num partition = %u\n", tHddInfo.ulNumPartitions);
			
				for (i = 0; i < tHddInfo.ulNumPartitions; i++)
				{
					FDI_Print("[%s] ullPartitionSize(%llu)\n", __func__, tHddInfo.ullPartitionSize[i]);
			
					VK_MEM_Memset(aucHDDSizeStr, 0x0, 20);
					snprintf(aucHDDSizeStr, 20, "%llu", tHddInfo.ullPartitionSize[i]);
					unPartitionStrSize = strlen(aucHDDSizeStr);
					if ((nCurrPos + (unPartitionStrSize + 1)) > FDI_HDD_INFO_MAX_SIZE)
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_GetInfo] : Str buffer is full.\n"));
						break;
					}
					pucResult[nCurrPos++] = unPartitionStrSize;
					strncpy(&pucResult[nCurrPos], aucHDDSizeStr, unPartitionStrSize);
					nCurrPos += unPartitionStrSize;
				}
				break;
			default:
				break;
		}
	
		return FAI_NO_ERR;
	
	TEST_FAIL:
	
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
FAI_ERR_CODE INTRN_FAI_HDD_SetParking(FAI_HDD_POSITION_e HddPosition, BOOL bParking_onoff, FAI_HDD_TMD_ERR *pHddError)
{
	FDI_ERR_CODE eFdiErr;
#if 0
	eFdiErr = FDI_HDD_SetParking(bParking_onoff);
	if( eFdiErr != FDI_NO_ERR )
	{
        return FAI_ERR;
	}
#endif
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
FAI_ERR_CODE INTRN_FAI_HDD_Defect(int *pnDefectCnt, FAI_HDD_TMD_ERR *pHddError)
{
#if 0
	eFdiErr = FDI_HDD_DefectTest(pnDefectCnt);
	if( eFdiErr != FDI_NO_ERR )
	{
        return FAI_ERR;
	}
#endif
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
FAI_ERR_CODE INTRN_FAI_HDD_ReadPOH(int *pnReadTime, FAI_HDD_TMD_ERR *pHddError)
{
#if 0
        eFdiErr = FDI_HDD_ReadPOH(pnReadTime);
        if( eFdiErr != FDI_NO_ERR )
        {
            return FAI_ERR;
        }
#endif
return FAI_NO_ERR;

}

static void INTRN_FAI_HDD_Dst_TimeNotify(unsigned long tid, void *arg)
{
	int ret;
	FAI_HDD_MSG_t tHddMsg;
	FVK_ERR_CODE eFvkErr;

	printf("[INTRN_FAI_HDD_Dst_TimeNotify]\n");
	ret=FDI_HDD_DstStatus(nHddFd);
	if(ret != 15)
	{
	    tHddMsg.nCmd = 0x17;   /* 0x17 means HDD Short DST Test */
		if(ret == 0)
		{
			tHddMsg.nResult = 0;/*ok*/
		}
		else
		{
			tHddMsg.nResult = 1;/*fail*/
		}
		eFvkErr = FVK_MSG_SendTimeout(g_ulHddMsgId, &tHddMsg, sizeof(FAI_HDD_MSG_t),0);
	}
	printf("[INTRN_FAI_HDD_Dst_TimeNotify] Wait... %d\n",ret);
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_HDD_SmartShortDST(FAI_HDD_TMD_ERR *pHddError)
{

	unsigned char ucHddDeviceName[10];

	FDI_ERR_CODE eFdiErr;
	int nStatus = 0;
	unsigned char buf[FAI_SMART_BUF_SIZE];

	eFdiErr = FDI_HDD_GetDeviceName(ucHddDeviceName);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_SmartShortDST] : FDI_HDD_GetDeviceName Error!!\n"));
		*pHddError = FAI_HDD_TMD_ERR_GET_DEVNAME_FAIL;

		return FAI_ERR;
	}

	eFdiErr = FDI_HDD_DSTOpen(ucHddDeviceName,&nHddFd);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_SmartShortDST] : FDI_HDD_DSTOpen Error!!\n"));
		*pHddError = FAI_HDD_TMD_ERR_DSTOPEN_FAIL;

		return FAI_ERR;
	}

	nStatus = FDI_HDD_DstStatus(nHddFd);
	if(nStatus < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDD_SmartShortDST] : FDI_HDD_DstStatus Error to read DstStatus %d\n",nStatus));
		*pHddError = FAI_HDD_TMD_ERR_DSTSTS_FAIL;

		goto err;
	}
	FAI_PRINT(FAI_PRT_ERROR, ("FDI_HDD_DstStatus] 00 nStatus %d \n",nStatus));

	if(nStatus == 15)
	{
		//goto doing;
		*pHddError = FAI_HDD_TMD_ERR_DSTSTS_FAIL;
		goto err;
	}

	memset(buf,0x00,FAI_SMART_BUF_SIZE);
	nStatus = FDI_HDD_AtaCmd(nHddFd, buf, FDI_ATA_SMART_ENABLE);
	if(nStatus < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%d]%s Error to read Attr values \n",__LINE__,__func__));
		*pHddError = FAI_HDD_TMD_ERR_ATASMART_EN_FAIL;
		goto err;
	}
	memset(buf,0x00,FAI_SMART_BUF_SIZE);
	buf[1] = 1; //short  /* buf[1] = 2 //long*/
	nStatus = FDI_HDD_AtaCmd(nHddFd, buf, FDI_ATA_SMART_EXECUTE_OFFLINE_IMMEDIATE);
	if(nStatus < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%d]%s Error to do SMART EXECUTE OFFLINE IMMEDIATE \n",__LINE__,__func__));
		*pHddError = FAI_HDD_TMD_ERR_ATASMART_OFFLINE_FAIL;
		goto err;
	}

	memset(buf,0x00,FAI_SMART_BUF_SIZE);
	nStatus = FDI_HDD_AtaCmd(nHddFd, buf, FDI_ATA_SMART_READ_ATTR_VALUE);
	if(nStatus < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%d]%s Error to read Attr values \n",__LINE__,__func__));
		*pHddError = FAI_HDD_TMD_ERR_ATASMART_RD_ATTR_FAIL;
		goto err;
	}

	if(!(buf[367] & 0xF0))
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%d]%s Error to do LongDST/ShortDST or not Start\n",__LINE__,__func__));
		*pHddError = FAI_HDD_TMD_ERR_DST_NOT_STARTED_FAIL;
		goto err;
	}

	VK_TIMER_EventEvery(1000, INTRN_FAI_HDD_Dst_TimeNotify,NULL,0,&ulHddDstTimerId);

	return FAI_NO_ERR;
err:
	FDI_HDD_DSTClose(ucHddDeviceName);
	return FAI_ERR;
}





