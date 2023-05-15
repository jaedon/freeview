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

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"
#include "fai_util.h"
#include "fai_eeprom.h"
#include "fdi_eeprom.h"
#include "fdi_flash.h"

/* model specific configuration header */
#include "factorysw_config.h"

typedef struct FAI_EEPROM_MSG
{
	int nCmd;

    int nAddress;
    int nSize;
} FAI_EEPROM_MSG_t;

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
unsigned long g_ulEepromMsgId, g_ulEepromTaskId;

void INTRN_FAI_EEPROM_Task(void *pData);
FAI_ERR_CODE INTRN_FAI_EEPROM_EraseAll(FAI_EEPROM_TMD_ERR *pEepromError);
FAI_ERR_CODE INTRN_FAI_EEPROM_Erase(int nAddress, int nSize, FAI_EEPROM_TMD_ERR *pEepromError);
FAI_ERR_CODE INTRN_FAI_EEPROM_Test(FAI_EEPROM_TMD_ERR *pEepromError);
FAI_ERR_CODE INTRN_FAI_EEPROM_Init(  FAI_EEPROM_TMD_ERR *pEepromError);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_EEPROM_Init(void)
{
	FVK_ERR_CODE eFvkErr;

	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_EEPROM_MSG_t), (char*)"FlsMsg", &g_ulEepromMsgId);
	if( eFvkErr	!= FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Init] : FVK_MSG_Create Error \n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Create(INTRN_FAI_EEPROM_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "eep Task", NULL, &g_ulEepromTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Start(g_ulEepromTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Init] : FVK_TASK_Start Error!!\n"));
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
#if (EEPROM_ERASE_ALL_ENABLE==YES)
FAI_ERR_CODE FAI_EEPROM_EraseAll(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_EEPROM_MSG_t tEepromMsg;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_EraseAll] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 2;
		tTMDResult.ucAction = 1;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

    tEepromMsg.nCmd = 0x01;   /* 0x01 means EEPROM ERase All Test */
	eFvkErr = FVK_MSG_Send(g_ulEepromMsgId, &tEepromMsg, sizeof(FAI_EEPROM_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_EraseAll] : Error FVK_MSG_Send() : %d\n", eFvkErr));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 2;
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
#if (EEPROM_INIT_ENABLE==YES)
FAI_ERR_CODE FAI_EEPROM_Erase(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_EEPROM_MSG_t tEepromMsg;

	unsigned char aucAddress[16],unAddressLen;
	unsigned char aucSize[16],unSizeLen;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Erase] : Argument Count Error=%d\n", ucArgc));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 2;
		tTMDResult.ucAction = 2;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);

		return FAI_ERR;
	}

#if 1
	unAddressLen = FAI_TMD_GetParam(aucAddress, pucArgList, 0);
	unSizeLen = FAI_TMD_GetParam(aucSize, pucArgList, 1);

//	FAI_ERR_CODE FAI_TMD_ArgToValue(unsigned long *pulValue, unsigned char *pucArg, int nArgSize)
	FAI_TMD_ArgToValue((unsigned long*)&tEepromMsg.nAddress, aucAddress, unAddressLen);
	FAI_TMD_ArgToValue((unsigned long*)&tEepromMsg.nSize, aucSize, unSizeLen);
#else
    FAI_TMD_GetParam(&tEepromMsg.nAddress, pucArgList, 0);
    FAI_TMD_GetParam(&tEepromMsg.nSize, pucArgList, 1);
#endif

    tEepromMsg.nCmd = 0x02;   /* 0x01 means EEPROM ERase All Test */
	eFvkErr = FVK_MSG_Send(g_ulEepromMsgId, &tEepromMsg, sizeof(FAI_EEPROM_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Erase] : Error FVK_MSG_Send() : %d\n", eFvkErr));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 2;
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
#if (EEPROM_INIT_ENABLE==YES)
FAI_ERR_CODE FAI_EEPROM_Test(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_EEPROM_MSG_t tEepromMsg;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Test] : Argument Count Error=%d\n", ucArgc));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 2;
		tTMDResult.ucAction = 3;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

    tEepromMsg.nCmd = 0x03;   /* 0x01 means EEPROM ERase All Test */
	eFvkErr = FVK_MSG_Send(g_ulEepromMsgId, &tEepromMsg, sizeof(FAI_EEPROM_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Test] : Error FVK_MSG_Send() : %d\n", eFvkErr));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 2;
		tTMDResult.ucAction = 3;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
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
#if (EEPROM_INIT_ENABLE==YES)
FAI_ERR_CODE FAI_EEPROM_Initialize(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FVK_ERR_CODE eFvkErr;
	FAI_EEPROM_MSG_t tEepromMsg;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Initialize] : Argument Count Error=%d\n", ucArgc));
		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 2;
		tTMDResult.ucAction = 4;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		FAI_TMD_SetResult(&tTMDResult);

		return FAI_ERR;
	}

    tEepromMsg.nCmd = 0x04;   /* 0x01 means EEPROM ERase All Test */
	eFvkErr = FVK_MSG_Send(g_ulEepromMsgId, &tEepromMsg, sizeof(FAI_EEPROM_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Initialize] : Error FVK_MSG_Send() : %d\n", eFvkErr));

		tTMDResult.ucCategory = FAI_CATEGORY_MEMORY;
		tTMDResult.ucModule = 2;
		tTMDResult.ucAction = 4;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
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
#if (EEPROM_INIT_UI_ENABLE==YES)
FAI_ERR_CODE FAI_EEPROM_Initialize_UI(void)
{
	int i, j;
	unsigned char *pucEepromData, *pucFlashData;

	/* Display EEPROM Test Start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "EEPROM Write", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Memory allocation */
	pucEepromData = (unsigned char *)FVK_MEM_Alloc(EEPROM_INIT_UI_DATA_SIZE);
	if( pucEepromData == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Write] : FVK_MEM_Alloc Error!!\n"));
		goto fail;
	}

	pucFlashData = (unsigned char *)FVK_MEM_Alloc(EEPROM_INIT_UI_DATA_SIZE);
	if( pucFlashData == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Write] : FVK_MEM_Alloc Error!!\n"));
		goto fail;
	}
	FVK_MEM_Set(pucEepromData, 0x00, EEPROM_INIT_UI_DATA_SIZE);
	FVK_MEM_Set(pucFlashData, 0x00, EEPROM_INIT_UI_DATA_SIZE);

	/* Read data from flash */
	FVK_MEM_Copy(pucFlashData, EEPROM_INIT_UI_DATA_FLASH_START_OFFSET, EEPROM_INIT_UI_DATA_SIZE);

	/* Read data from eeprom */
	if( FDI_EEPROM_Read(EEPROM_INIT_UI_DATA_START_OFFSET, pucEepromData, EEPROM_INIT_UI_DATA_SIZE) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Write] : FDI_EEPROM_Read Error!!\n"));
		goto fail;
	}

	/* Compare */
	for( i = 0; i < EEPROM_INIT_UI_DATA_SIZE; i++ )
	{
		if( pucEepromData[i] != pucFlashData[i] )
		{
			if( FDI_EEPROM_Write(EEPROM_INIT_UI_DATA_START_OFFSET, pucFlashData, EEPROM_INIT_UI_DATA_SIZE) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Write] : FDI_EEPROM_Read Error!!\n"));
				goto fail;
			}
			if( FDI_EEPROM_Read(EEPROM_INIT_UI_DATA_START_OFFSET, pucEepromData, EEPROM_INIT_UI_DATA_SIZE) != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Write] : FDI_EEPROM_Read Error!!\n"));
				goto fail;
			}
			/* Verify */
			for( j = 0; j < EEPROM_INIT_UI_DATA_SIZE; j++ )
			{
				if( pucEepromData[i] != pucFlashData[i] )
				{
					goto fail;
				}
			}
		}
	}

	/* Display pass */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "EEPROM Erase", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);

        #if 1
	/* [20140827] buf fixed : by jamin */
	FVK_MEM_Free(pucEepromData);
	FVK_MEM_Free(pucFlashData);
        #endif 

	return FAI_NO_ERR;

fail:
	/* Display fail */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "EEPROM Write", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);

        #if 1
	/* [20140827] buf fixed : by jamin */
	FVK_MEM_Free(pucEepromData);
	FVK_MEM_Free(pucFlashData);
        #endif

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
#if (EEPROM_ERASE_ALL_UI_ENABLE==YES)
FAI_ERR_CODE FAI_EEPROM_EraseAll_UI(void)
{
	int i;
	unsigned char *pucBuf;

	/* Display EEPROM Test Start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "EEPROM Erase", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Memory allocation */
	pucBuf = (unsigned char *)FVK_MEM_Alloc(EEPROM_ERASE_ALL_UI_DATA_SIZE);
	if( pucBuf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Erase] : FVK_MEM_Alloc Error!!\n"));
		goto fail;
	}
	FVK_MEM_Set(pucBuf, 0x00, EEPROM_ERASE_ALL_UI_DATA_SIZE);
	if( FDI_EEPROM_Write(EEPROM_ERASE_ALL_UI_START_OFFSET, pucBuf, EEPROM_ERASE_ALL_UI_DATA_SIZE) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Erase] : FDI_EEPROM_Write Error!!\n"));
		goto fail;
	}
	FVK_MEM_Set(pucBuf, 0xff, EEPROM_ERASE_ALL_UI_DATA_SIZE);
	if( FDI_EEPROM_Read(EEPROM_ERASE_ALL_UI_START_OFFSET, pucBuf, EEPROM_ERASE_ALL_UI_DATA_SIZE) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Erase] : FDI_EEPROM_Read Error!!\n"));
		goto fail;
	}
	/* Verify */
	for( i = 0; i < EEPROM_ERASE_ALL_UI_DATA_SIZE; i++ )
	{
		if( pucBuf[i] != 0x00 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_EEPROM_Erase] : Verification Error!!\n"));
			goto fail;
		}
	}

	/* Display pass */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "EEPROM Erase", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);

	return FAI_NO_ERR;
fail:
	/* Display fail */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "EEPROM Erase", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);

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
void INTRN_FAI_EEPROM_Task(void *pData)
{
	FAI_EEPROM_MSG_t tEepromMsg;
	FVK_ERR_CODE eFvkErr;

   	FAI_TMD_RESULT_T tTMDResult;
	FAI_EEPROM_TMD_ERR stEepromError = FAI_EEPROM_TMD_ERR_NO;

	unsigned char ucTmdErrCode[2];

	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_EEPROM;

	while(1)
	{
		eFvkErr = FVK_MSG_Receive(g_ulEepromMsgId, &tEepromMsg, sizeof(FAI_EEPROM_MSG_t));
		if (eFvkErr != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_EEPROM_Task] : Error FVK_MSG_Receive() %d\n", eFvkErr));
			continue;
		}

        FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

        if( tEepromMsg.nCmd == 0x01 )     /* 0x01 means EEPROM ERASE All command */
		{
			if( INTRN_FAI_EEPROM_EraseAll(&stEepromError) != FAI_NO_ERR )
			{
				/* Fail 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x02;
				tTMDResult.ucAction = tEepromMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				ucTmdErrCode[1] = (unsigned char)stEepromError;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

				FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
				/* Pass 인 경우 */
				tTMDResult.ucCategory = 0x08;
				tTMDResult.ucModule = 0x02;
				tTMDResult.ucAction = tEepromMsg.nCmd;
				tTMDResult.eResult = FAI_TMD_ERR_OK;
				stEepromError =FAI_EEPROM_TMD_ERR_NO;

				FAI_TMD_SetResult(&tTMDResult);
			}
		}
        else if( tEepromMsg.nCmd == 0x02 )     /* 0x02 means EEPROM ERASE command */
		{
			if( INTRN_FAI_EEPROM_Erase(tEepromMsg.nAddress, tEepromMsg.nSize, &stEepromError) != FAI_NO_ERR )
			{
			     /* Fail 인 경우 */
                 tTMDResult.ucCategory = 0x08;
                 tTMDResult.ucModule = 0x02;
                 tTMDResult.ucAction = tEepromMsg.nCmd;
                 tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				 ucTmdErrCode[1] = (unsigned char)stEepromError;
				 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

                 FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{
			     /* Pass 인 경우 */
                tTMDResult.ucCategory = 0x08;
                tTMDResult.ucModule = 0x02;
                tTMDResult.ucAction = tEepromMsg.nCmd;
                tTMDResult.eResult = FAI_TMD_ERR_OK;
                stEepromError =FAI_EEPROM_TMD_ERR_NO;

                FAI_TMD_SetResult(&tTMDResult);
			}
        }
        else if( tEepromMsg.nCmd == 0x03 )     /* 0x03 means EEPROM Test command */
        {

             if( INTRN_FAI_EEPROM_Test(&stEepromError) != FAI_NO_ERR )
             {
                  /* Fail 인 경우 */
                  tTMDResult.ucCategory = 0x08;
                  tTMDResult.ucModule = 0x02;
                  tTMDResult.ucAction = tEepromMsg.nCmd;
                  tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				  ucTmdErrCode[1] = (unsigned char)stEepromError;
				  FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

                  FAI_TMD_SetResult(&tTMDResult);
             }
             else
             {
                  /* Pass 인 경우 */
                 tTMDResult.ucCategory = 0x08;
                 tTMDResult.ucModule = 0x02;
                 tTMDResult.ucAction = tEepromMsg.nCmd;
                 tTMDResult.eResult = FAI_TMD_ERR_OK;
                 stEepromError =FAI_EEPROM_TMD_ERR_NO;

                 FAI_TMD_SetResult(&tTMDResult);
             }
         }
        else if( tEepromMsg.nCmd == 0x04 )     /* 0x04 means EEPROM Init command */
         {
             if( INTRN_FAI_EEPROM_Init(&stEepromError) != FAI_NO_ERR )
             {
                  /* Fail 인 경우 */
                  tTMDResult.ucCategory = 0x08;
                  tTMDResult.ucModule = 0x02;
                  tTMDResult.ucAction = tEepromMsg.nCmd;
                  tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				  ucTmdErrCode[1] = (unsigned char)stEepromError;
				  FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

                  FAI_TMD_SetResult(&tTMDResult);
             }
             else
             {
                  /* Pass 인 경우 */
                 tTMDResult.ucCategory = 0x08;
                 tTMDResult.ucModule = 0x02;
                 tTMDResult.ucAction = tEepromMsg.nCmd;
                 tTMDResult.eResult = FAI_TMD_ERR_OK;
                 stEepromError =FAI_EEPROM_TMD_ERR_NO;

                 FAI_TMD_SetResult(&tTMDResult);
             }
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
FAI_ERR_CODE INTRN_FAI_EEPROM_EraseAll( FAI_EEPROM_TMD_ERR *pEepromError)
{
    unsigned long ulEepromSize =0;
    unsigned char *pucEepromData;
	int nCnt =0;

    if(FDI_EEPROM_GetCapability(&ulEepromSize) != FDI_NO_ERR)
    {
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_EraseAll] : FDI_EEPROM_GetCapability Error!!\n"));
		*pEepromError = FAI_EEPROM_TMD_ERR_CAPA_FAIL;
		goto TEST_FAIL;

    }

    pucEepromData = FVK_MEM_Alloc(ulEepromSize);
    if( pucEepromData == NULL )
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_EraseAll] : FVK_MEM_Alloc Error!!\n"));
        FVK_MEM_Free(pucEepromData);
		*pEepromError = FAI_EEPROM_TMD_ERR_MALLOC_FAIL;
        goto TEST_FAIL;
    }

    FVK_MEM_Set(pucEepromData, 0x00, ulEepromSize);

    if( FDI_EEPROM_Write(0x00, pucEepromData, ulEepromSize) != FDI_NO_ERR )
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_EraseAll] : FDI_EEPROM_Read Error!!\n"));
        FVK_MEM_Free(pucEepromData);
		*pEepromError = FAI_EEPROM_TMD_ERR_WR_FAIL;
        goto TEST_FAIL;
    }

#if 1
	/* pucEepromData 을 그데로 이용해서 비교하자 */
	if( FDI_EEPROM_Read(0x00, pucEepromData, ulEepromSize) != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucEepromData);
		*pEepromError = FAI_EEPROM_TMD_ERR_RD_FAIL;
		goto TEST_FAIL;
	}

	 /* Validation */
	for(nCnt = 0; nCnt < ulEepromSize ; nCnt++)
	{
//		printf("%02x ", pucEepromData[nCnt]);
#if 1
		if(pucEepromData[nCnt] != 0x00)
		{
        	FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_EraseAll] : nCnt is 0x%x Error!!\n",nCnt));
			FVK_MEM_Free(pucEepromData);
			*pEepromError = FAI_EEPROM_TMD_ERR_WR_FAIL;
			goto TEST_FAIL;
		}
#endif
	}
#endif
    FVK_MEM_Free(pucEepromData);

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
FAI_ERR_CODE INTRN_FAI_EEPROM_Erase(int nAddress, int nSize, FAI_EEPROM_TMD_ERR *pEepromError)
{
	int nCnt =0;
    unsigned char *pucEepromData;

	pucEepromData = FVK_MEM_Alloc(nSize);
	if( pucEepromData == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Erase] : FVK_MEM_Alloc Error!!\n"));
		FVK_MEM_Free(pucEepromData);
		*pEepromError = FAI_EEPROM_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}

    FVK_MEM_Set(pucEepromData, 0x00, nSize);

    if( FDI_EEPROM_Write(nAddress, pucEepromData, nSize) != FDI_NO_ERR )
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Erase] : FDI_EEPROM_Write Error!!\n"));
		FVK_MEM_Free(pucEepromData);
		*pEepromError = FAI_EEPROM_TMD_ERR_WR_FAIL;
        goto TEST_FAIL;
    }

	/* pucEepromData 을 그데로 이용해서 비교하자 */
	if( FDI_EEPROM_Read(nAddress, pucEepromData, nSize) != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucEepromData);
		*pEepromError = FAI_EEPROM_TMD_ERR_RD_FAIL;
		goto TEST_FAIL;
	}

	  /* Validation */
	 for(nCnt = 0; nCnt < nSize ; nCnt++)
	 {
		 if(pucEepromData[nCnt] != 0x00)
		 {
			 FVK_MEM_Free(pucEepromData);
			 *pEepromError = FAI_EEPROM_TMD_ERR_WR_FAIL;
			 goto TEST_FAIL;
		 }
	 }

    FVK_MEM_Free(pucEepromData);

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
FAI_ERR_CODE INTRN_FAI_EEPROM_Test(FAI_EEPROM_TMD_ERR *pEepromError)
{
    /*
        1. eeprom backup
        2.eeprom write adn read and verify,
        3. write from backup
    */

    unsigned char *pucEepromData_backup;
    unsigned char *pucEepromData;
    unsigned char *pucEepromData_Comp;

    unsigned long ulEepromSize =0;
    unsigned int unCnt =0;

	int nResult=0x00;

    if(FDI_EEPROM_GetCapability(&ulEepromSize) != FDI_NO_ERR)
    {
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Test] : FDI_EEPROM_GetCapability Error!!\n"));
		*pEepromError = FAI_EEPROM_TMD_ERR_CAPA_FAIL;
		goto TEST_FAIL;
    }

	pucEepromData_backup = FVK_MEM_Alloc(ulEepromSize);
	if( pucEepromData_backup == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Test] : FVK_MEM_Alloc Error!!\n"));
		FVK_MEM_Free(pucEepromData_backup);
		*pEepromError = FAI_EEPROM_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}

	if( FDI_EEPROM_Read(0x00, pucEepromData_backup, ulEepromSize) != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucEepromData_backup);
		*pEepromError = FAI_EEPROM_TMD_ERR_RD_FAIL;
		goto TEST_FAIL;
	}

    pucEepromData = FVK_MEM_Alloc(ulEepromSize);
    if( pucEepromData == NULL )
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Test] : FVK_MEM_Alloc Error!!\n"));
        FVK_MEM_Free(pucEepromData);
		FVK_MEM_Free(pucEepromData_backup);
		*pEepromError = FAI_EEPROM_TMD_ERR_MALLOC_FAIL;
        goto TEST_FAIL;
    }

     /* any special char for comp */
    for(unCnt =0; unCnt < ulEepromSize; unCnt++)
    {
        pucEepromData[unCnt] = unCnt%0xFF;
    }

    if( FDI_EEPROM_Write(0x00, pucEepromData, ulEepromSize) != FDI_NO_ERR )
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Test] : FDI_EEPROM_Write Error!!\n"));
        FVK_MEM_Free(pucEepromData);
		FVK_MEM_Free(pucEepromData_backup);
		*pEepromError = FAI_EEPROM_TMD_ERR_WR_FAIL;
		goto TEST_FAIL;
    }

	pucEepromData_Comp = FVK_MEM_Alloc(ulEepromSize);
	if( pucEepromData_Comp == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Test] : FVK_MEM_Alloc Error!!\n"));
		FVK_MEM_Free(pucEepromData_Comp);
        FVK_MEM_Free(pucEepromData);
		FVK_MEM_Free(pucEepromData_backup);
		*pEepromError = FAI_EEPROM_TMD_ERR_MALLOC_FAIL;
		goto TEST_FAIL;
	}

	if( FDI_EEPROM_Read(0x00, pucEepromData_Comp, ulEepromSize) != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucEepromData_Comp);
        FVK_MEM_Free(pucEepromData);
		FVK_MEM_Free(pucEepromData_backup);

		*pEepromError = FAI_EEPROM_TMD_ERR_RD_FAIL;
		goto TEST_FAIL;
	}

    if( (nResult = FVK_MEM_Compare(pucEepromData_Comp, pucEepromData, ulEepromSize)) != 0 )
    {
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Test]0 : FVK_MEM_Compare Error!!nResult = 0x%x\n",nResult));
		FVK_MEM_Free(pucEepromData_Comp);
        FVK_MEM_Free(pucEepromData);
		FVK_MEM_Free(pucEepromData_backup);
		*pEepromError = FAI_EEPROM_TMD_ERR_WR_FAIL;
        goto TEST_FAIL;
    }

    if( FDI_EEPROM_Write(0x00, pucEepromData_backup, ulEepromSize) != FDI_NO_ERR )
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Test] : FDI_EEPROM_Read Error!!\n"));
		FVK_MEM_Free(pucEepromData_Comp);
        FVK_MEM_Free(pucEepromData);
		FVK_MEM_Free(pucEepromData_backup);

		*pEepromError = FAI_EEPROM_TMD_ERR_WR_FAIL;

        goto TEST_FAIL;
    }
#if 1
	 /* validation pucEepromData_Comp 사용 .*/
	if( FDI_EEPROM_Read(0x00, pucEepromData_Comp, ulEepromSize) != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucEepromData_Comp);
        FVK_MEM_Free(pucEepromData);
		FVK_MEM_Free(pucEepromData_backup);

		*pEepromError = FAI_EEPROM_TMD_ERR_RD_FAIL;
		goto TEST_FAIL;
	}

	if( (nResult = FVK_MEM_Compare(pucEepromData_Comp, pucEepromData_backup, ulEepromSize)) != 0 )
    {
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Test]1 : FVK_MEM_Compare Error!!nResult = 0x%x\n",nResult));
		FVK_MEM_Free(pucEepromData_Comp);
        FVK_MEM_Free(pucEepromData);
		FVK_MEM_Free(pucEepromData_backup);
		*pEepromError = FAI_EEPROM_TMD_ERR_WR_FAIL;
        goto TEST_FAIL;
    }

#endif
    FVK_MEM_Free(pucEepromData_Comp);
    FVK_MEM_Free(pucEepromData);
    FVK_MEM_Free(pucEepromData_backup);

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
FAI_ERR_CODE INTRN_FAI_EEPROM_Init(FAI_EEPROM_TMD_ERR *pEepromError)
{
    unsigned char *pucEepromInitData;
    unsigned char *pucEepromWritedData;

    unsigned long ulEepromSize =0;

    if(FDI_EEPROM_GetCapability(&ulEepromSize) != FDI_NO_ERR)
    {
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Init] : FDI_EEPROM_GetCapability Error!!\n"));
		goto TEST_FAIL;
    }

	pucEepromInitData = FVK_MEM_Alloc(ulEepromSize);
	if( pucEepromInitData == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Init] : FVK_MEM_Alloc Error!!\n"));
		FVK_MEM_Free(pucEepromInitData);
		goto TEST_FAIL;
	}

    if(FDI_EEPROM_Read(EEPROM_INIT_DATA_START_FLASH_ADDR, pucEepromInitData, ulEepromSize) != FDI_NO_ERR )
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Init] : FDI_FLASH_Read Error!!\n"));
        FVK_MEM_Free(pucEepromInitData);
        goto TEST_FAIL;
    }

    if( FDI_EEPROM_Write(0x00, pucEepromInitData, ulEepromSize) != FDI_NO_ERR )
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Init] : FDI_EEPROM_Read Error!!\n"));
		FVK_MEM_Free(pucEepromInitData);

        goto TEST_FAIL;
    }

	pucEepromWritedData = FVK_MEM_Alloc(ulEepromSize);
	if( pucEepromWritedData == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Init] : FVK_MEM_Alloc Error!!\n"));
        FVK_MEM_Free(pucEepromInitData);
        FVK_MEM_Free(pucEepromWritedData);
		goto TEST_FAIL;
	}
#if 1  /* validation */
	if( FDI_EEPROM_Read(0x00, pucEepromWritedData, ulEepromSize) != FDI_NO_ERR )
	{
        FVK_MEM_Free(pucEepromInitData);
        FVK_MEM_Free(pucEepromWritedData);
		goto TEST_FAIL;
	}

    if( FVK_MEM_Compare(pucEepromInitData, pucEepromWritedData, ulEepromSize) != 0 )
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_EEPROM_Init] : FVK_MEM_Alloc Error!!\n"));
        FVK_MEM_Free(pucEepromInitData);
        FVK_MEM_Free(pucEepromWritedData);
		goto TEST_FAIL;
    }
#endif

    FVK_MEM_Free(pucEepromInitData);
    FVK_MEM_Free(pucEepromWritedData);

    return FAI_NO_ERR;

TEST_FAIL:
    return FAI_ERR;

}

