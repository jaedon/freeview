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
#include <fcntl.h>
#if !defined(CONFIG_OS_RTOS)
#include <sys/mount.h>
#endif

/* chip manufacture's  header files */

/* humax header files */
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"
#include "fdi_mmc.h"
#include "fai_mmc.h"
#include "fvk.h"

/* model specific configuration header */
#include "factorysw_config.h"

static BOOL s_bMMCInserted = 0;
static BOOL s_bMMCFormatted = 0;
static BOOL s_bMMCFormatCmd = FALSE;





/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
typedef enum
{
	FAI_MMC_TEST_TMD, 
	FAI_MMC_TEST_UI
} FAI_MMC_TEST_TYPE;

typedef struct FAI_MMC_MSG
{
	BOOL bFormatted;
} FAI_MMC_MSG_t;


#define FAI_MMC_SDCARD_SN 		0

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
    unsigned char g_checkMode;
	unsigned char g_aucResult[7];
	unsigned int g_ulNoParms, g_ulParmLength;
	bool g_protect_status;

	unsigned long g_ulMMCMsgId,g_ulMMCTaskId;
	static FAI_MMC_TEST_TYPE eTestType = FAI_MMC_TEST_TMD;

FAI_ERR_CODE INTRN_FAI_MMC_NotifyFunc(FDI_MMC_EVENT_e eEventType, FDI_MMC_INFO_t *ptMMCInfo);
FAI_ERR_CODE INTRN_FAI_MMC_SvcNotifyFunc(FDI_MMC_SVC_EVT_e eEventType);

/*******************************************************************************
* function : INTRN_FAI_FKEY_TestTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_MMC_Task(void *data)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned short usErrCode = FAI_TMD_DEFAULT_ERROR_CODE;
	FAI_MMC_MSG_t tMmcMsg;
	FVK_ERR_CODE eVkRet;
	unsigned char aucAgcResult[4];
	unsigned char ucTmdErrCode[2];
	
	int nIndex;
	while(1)
	{
	
		eVkRet = FVK_MSG_Receive(g_ulMMCMsgId, &tMmcMsg, sizeof(FAI_MMC_MSG_t));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_MMC_Task] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}
		

		if(s_bMMCFormatCmd==TRUE)
		{
			if( eTestType == FAI_MMC_TEST_TMD)
			{
				if(	tMmcMsg.bFormatted == FDI_MMC_SVC_EVT_FORMAT_DONE)
				{
					FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
						
					tTMDResult.ucCategory = 0x10;
					tTMDResult.ucModule 	= 0x11;
					tTMDResult.ucAction 	= 0x01;
					tTMDResult.eResult 	= FAI_TMD_ERR_OK;
								
					FAI_TMD_SetResult(&tTMDResult);	
				}
				else if(tMmcMsg.bFormatted == FDI_MMC_SVC_EVT_FORMAT_FAIL)
				{

					tTMDResult.ucCategory = 0x10;
					tTMDResult.ucModule = 0x11;
					tTMDResult.ucAction = 0x01;
					tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					
					ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
					ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
					FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
					
					FAI_TMD_SetResult(&tTMDResult);
					
				}
			}
			else
			{
				if( tMmcMsg.bFormatted == FDI_MMC_SVC_EVT_FORMAT_DONE)
				{
					FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
					FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SDCard Format", RGB_COLOR_BLACK);
					FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Complete!!", RGB_COLOR_BLACK);
					FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
				}
				else if(tMmcMsg.bFormatted == FDI_MMC_SVC_EVT_FORMAT_FAIL)
				{

					FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
					FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SDCard Format", RGB_COLOR_RED);
					FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
					FDI_PANEL_Display(FAI_MMC_TEST_FAIL_STR);
					FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : MMC Format error2\n"));
				}
			}
		}
		else
		{

		
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
FAI_ERR_CODE FAI_MMC_Init(void)
{
	FVK_ERR_CODE eFvkErr;
	
	FDI_MMC_Open();
	FDI_MMC_RegisterCallback((pfnFDI_MMC_NotifyFunc)INTRN_FAI_MMC_NotifyFunc);
	FDI_MMC_RegisterSvcCallback((pfnFDI_MMC_SvcCallbackFunc)INTRN_FAI_MMC_SvcNotifyFunc);

	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_MMC_MSG_t), (char*)"MmcMsg", &g_ulMMCMsgId);
	if( eFvkErr	!= FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Init] : FVK_MSG_Create Error \n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Create(INTRN_FAI_MMC_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "MMC Task", NULL, &g_ulMMCTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Start(g_ulMMCTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}
	return FAI_NO_ERR;
	
}

#if (MMC_ENABLE==YES)
/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_MMC_CheckStatus(unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char checkMode = 0;
	unsigned char aucResult[7];
	unsigned int ulNoParms, ulParmLength;
	
	FAI_TMD_StartTest(0);
	
	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDD_DataCompare] : Argument Count Error=%d\n", ucArgc));

		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&checkMode, pucArgList, 1);
	
	if(checkMode == 0)
	{
		if( s_bMMCInserted == TRUE )
		{
			aucResult[0] = 1;
			aucResult[1] = 1;
		
			ulNoParms = 1;
			ulParmLength = 2;
		}
		else
		{
			aucResult[0] = 1;
			aucResult[1] = 0;
		
			aucResult[2] = 2;	// length of param 
			aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high
			aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low
		
			ulNoParms = 1+1;
			ulParmLength = 2+FAI_TMD_ERROR_PARM_LENGTH;
		}
	}
	else if(checkMode == 1)
	{
		if( s_bMMCInserted == FALSE )
		{
			aucResult[0] = 1;
			aucResult[1] = 1;
		
			ulNoParms = 1;
			ulParmLength = 2;
		}
		else
		{
			aucResult[0] = 1;
			aucResult[1] = 0;

			aucResult[2] = 1;	// length of param
			aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high
			aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low
		
			ulNoParms = 1+1;
			ulParmLength = 2+FAI_TMD_ERROR_PARM_LENGTH;
		}

	}
	else
	{
		aucResult[0] = 1;
		aucResult[1] = 0;
		
		aucResult[2] = 1;	// length of param
		aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high
		aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low
		
		ulNoParms = 1+1;
		ulParmLength = 2+FAI_TMD_ERROR_PARM_LENGTH;
	}

	
	// Save result parameters
	FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_NO_ERROR, ulNoParms, aucResult, ulParmLength);

//	FDI_MMC_UnMount(1);

	return FAI_NO_ERR;
	
TEST_FAIL:
	aucResult[0] = 0x01;	// length of param
	aucResult[1] = 0x00;	// result - fail

	aucResult[2] = 1;	// length of param 
	aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high
	aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF;		// result - error code low

	FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_ERROR, 1+1, aucResult, 2+FAI_TMD_ERROR_PARM_LENGTH);
	
	return FAI_ERR;
}

FAI_ERR_CODE FAI_MMC_ReadTest(unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucBusWidth;
	int nSdFd;
	char *pcFileName;
	int nFileSize, nReadSize;
	unsigned char *pb;
	unsigned char aucResult[15];
	unsigned int ulNoParms, ulParmLength;
	
	unsigned long ulCrc;
	pcFileName = "/mnt/sd0/sd_test";
		
	FAI_TMD_StartTest(0);
	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SC_AtrCheck] : Argument Count Error=%d\n", ucArgc));
		goto TEST_FAIL;
	}

//	FAI_TMD_GetParam(&checkMode, pucArgList, 1);
	
	FDI_MMC_Mount(1, "/mnt/sd0");
	FVK_TASK_Sleep(500);
	
	if(FDI_NO_ERR == FDI_MMC_GetDataLineWidth(&ucBusWidth) && ucBusWidth == 4)
	{
		nSdFd = open(pcFileName, O_RDONLY | O_NONBLOCK);
		if( nSdFd < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_CheckStatus] : open Error!!\n"));
			goto TEST_FAIL;
		}
	
		nFileSize =lseek(nSdFd, 0, SEEK_END);
		if( nFileSize < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_CheckStatus] : lseek Error!!\n"));
			goto TEST_FAIL;
		}
	
		pb= FVK_MEM_Alloc(nFileSize);
		if( pb == NULL )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_CheckStatus] : FVK_MEM_Alloc Error!!\n"));
			goto TEST_FAIL;
		}
	
		 
		if( lseek(nSdFd, 0, SEEK_SET) != 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_CheckStatus] : lseek Error!!\n"));
			goto TEST_FAIL;
		}
			
		/* read file */
		nReadSize = read(nSdFd, pb, nFileSize);
		if( nReadSize != nFileSize )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_CheckStatus] : read Error!!\n"));
			goto TEST_FAIL;
		}

		ulCrc = FAI_UTIL_GetCRC32Fast((void *)pb, (unsigned long)nFileSize);

		FAI_PRINT(FAI_PRT_ERROR, ("CRC : %x\n", ulCrc));
		printf("junkh MMC CRC : %x\n", ulCrc);
	
		aucResult[0] = 1;
		aucResult[1] = 1;
		aucResult[2] = 4;	// length of param 
		aucResult[3] = (ulCrc>>24) & 0xFF;
		aucResult[4] = (ulCrc>>16) & 0xFF;
		aucResult[5] = (ulCrc>>8) & 0xFF;
		aucResult[6] = (ulCrc) & 0xFF;
			
		ulNoParms = 2;
		ulParmLength = 7;
	
		FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_NO_ERROR, ulNoParms, aucResult, ulParmLength);

		//FDI_MMC_UnMount(1);
		VK_SYSTEM_Command("umount /mnt/sd0");

		return FAI_NO_ERR;
	}
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_CheckStatus] : not mounted or bus width error\n"));
		goto TEST_FAIL;
	}
	
TEST_FAIL:
	aucResult[0] = 0x01;	// length of param
	aucResult[1] = 0x00;	// result - fail
	
	aucResult[2] = 1;	// length of param 
	aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high
	aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low
	
	FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_ERROR, 1+1, aucResult, 2+FAI_TMD_ERROR_PARM_LENGTH);
	
	return FAI_NO_ERR;
}
FAI_ERR_CODE FAI_MMC_GetWriteLockState(unsigned char ucArgc, unsigned char *pucArgList)
{
/*	unsigned char checkMode = 0;
	unsigned char aucResult[7];
	unsigned int ulNoParms, ulParmLength;
	bool protect_status;*/

	FAI_TMD_StartTest(0);

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SC_AtrCheck] : Argument Count Error=%d\n", ucArgc));
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&g_checkMode, pucArgList, 1);

	FDI_MMC_GetWriteLockState(&g_protect_status);
	if(g_protect_status > 1)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FDI_MMC_GetWriteLockState] : Insert error =%d\n", g_protect_status));
		goto TEST_FAIL;
	}
	
	if(g_checkMode == 0)
	{
		if( g_protect_status == TRUE )
		{
			g_aucResult[0] = 1;
			g_aucResult[1] = 1;
		
			g_ulNoParms = 1;
			g_ulParmLength = 2;
		}
		else
		{
			g_aucResult[0] = 1;
			g_aucResult[1] = 0;
		
			g_aucResult[2] = 2;	// length of param
			g_aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high /
			g_aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low /
		
			g_ulNoParms = 1+1;
			g_ulParmLength = 2+FAI_TMD_ERROR_PARM_LENGTH;
		}

	}
	else if(g_checkMode == 1)
	{
		if( g_protect_status == FALSE )
		{
			g_aucResult[0] = 1;
			g_aucResult[1] = 1;
		
			g_ulNoParms = 1;
			g_ulParmLength = 2;
		}
		else
		{
			g_aucResult[0] = 1;
			g_aucResult[1] = 0;
		
			g_aucResult[2] = 1;	// length of param /
			g_aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high/
			g_aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low/
		
			g_ulNoParms = 1+1;
			g_ulParmLength = 2+FAI_TMD_ERROR_PARM_LENGTH;
		}

	}
	else
	{
		g_aucResult[0] = 1;
		g_aucResult[1] = 0;
		
		g_aucResult[2] = 1;	// length of param /
		g_aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high /
		g_aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	//result - error code low /
		
		g_ulNoParms = 1+1;
		g_ulParmLength = 2+FAI_TMD_ERROR_PARM_LENGTH;
	}

	// Save result parameters /
	FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_NO_ERROR, g_ulNoParms, g_aucResult, g_ulParmLength);

//	FDI_MMC_UnMount(1);
	
	return FAI_NO_ERR;
	
TEST_FAIL:
	g_aucResult[0] = 0x01;	// length of param /
	g_aucResult[1] = 0x00;	// result - fail /

	g_aucResult[2] = 1;	//length of param /
	g_aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high /
	g_aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF;		// result - error code low /

	FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_ERROR, 1+1, g_aucResult, 2+FAI_TMD_ERROR_PARM_LENGTH);
	
	return FAI_ERR;
}

FAI_ERR_CODE FAI_MMC_RESULT_WriteLockState(unsigned char ucArgc, unsigned char *pucArgList)
{
		unsigned char checkMode = 0;
		unsigned char aucResult[7];
		unsigned int ulNoParms, ulParmLength;
		bool protect_status;
		
		FAI_TMD_StartTest(0);
	
		if( ucArgc != 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SC_AtrCheck] : Argument Count Error=%d\n", ucArgc));
			goto TEST_FAIL;
		}
	
/*		FAI_TMD_GetParam(&checkMode, pucArgList, 1);
	
		FDI_MMC_GetWriteLockState(&protect_status);
		if( (protect_status != 0 ) && (protect_status != 1))
		{
			printf("junkh protect :%x\n", protect_status);
			FAI_PRINT(FAI_PRT_ERROR, ("[FDI_MMC_GetWriteLockState] : Insert error =%d\n", protect_status));
			goto TEST_FAIL;
		}
		printf("junkh protect write :%x\n", protect_status);
		
		if(checkMode == 0)
		{
			if( protect_status == TRUE )
			{
				aucResult[0] = 1;
				aucResult[1] = 1;
			
				ulNoParms = 1;
				ulParmLength = 2;
			}
			else
			{
				aucResult[0] = 1;
				aucResult[1] = 0;
			
				aucResult[2] = 2;	// length of param
				aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high /
				aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low /
			
				ulNoParms = 1+1;
				ulParmLength = 2+FAI_TMD_ERROR_PARM_LENGTH;
			}
		}
		else if(checkMode == 1)
		{
			if( protect_status == FALSE )
			{
				aucResult[0] = 1;
				aucResult[1] = 1;
			
				ulNoParms = 1;
				ulParmLength = 2;
			}
			else
			{
				aucResult[0] = 1;
				aucResult[1] = 0;
			
				aucResult[2] = 1;	// length of param /
				aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high/
				aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low/
			
				ulNoParms = 1+1;
				ulParmLength = 2+FAI_TMD_ERROR_PARM_LENGTH;
			}
	
		}
		else
		{
			aucResult[0] = 1;
			aucResult[1] = 0;
			
			aucResult[2] = 1;	// length of param /
			aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high /
			aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	//result - error code low /
			
			ulNoParms = 1+1;
			ulParmLength = 2+FAI_TMD_ERROR_PARM_LENGTH;
		}
	
		
		// Save result parameters /
		FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_NO_ERROR, ulNoParms, aucResult, ulParmLength);*/
		FAI_TMD_SetTestResult(0x10, 0x18, 0x04, 0, FAI_TMD_TEST_NO_ERROR, 1, g_aucResult, 2);
		
		return FAI_NO_ERR;
		
	TEST_FAIL:
//		aucResult[0] = 0x01;	// length of param /
//		aucResult[1] = 0x00;	// result - fail /
	
//		aucResult[2] = 1;	//length of param /
//		aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high /
//		aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low /
	
		FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_ERROR, 1+1, g_aucResult, 2+FAI_TMD_ERROR_PARM_LENGTH);
		
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
FAI_ERR_CODE FAI_MMC_Format(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	unsigned char aucResult[15];
	eTestType = FAI_MMC_TEST_TMD;

	FDI_MMC_UnMount(1);
	FVK_TASK_Sleep(500);
	
	
	eFdiErr = FDI_MMC_Format(1);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : MMC Format error\n"));
		s_bMMCFormatCmd = FALSE;
		goto TEST_FAIL;
	}
	
	s_bMMCFormatCmd = TRUE;
	return FAI_NO_ERR;

	
	
TEST_FAIL:
	aucResult[0] = 0x01;	// length of param
	aucResult[1] = 0x00;	// result - fail
	
	aucResult[2] = 1;	// length of param 
	aucResult[3] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	// result - error code high
	aucResult[4] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF; 	// result - error code low
	
	FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_ERROR, 1+1, aucResult, 2+FAI_TMD_ERROR_PARM_LENGTH);
	
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
FAI_ERR_CODE FAI_MMC_File_Delete(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	eTestType = FAI_MMC_TEST_TMD;

	FDI_MMC_Mount(1, "/mnt/sd0");
	FVK_TASK_Sleep(1000);

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	

	eFdiErr = remove("/mnt/sd0/inittab");
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : file remove Error!!\n"));
		goto TEST_FAIL;
	}

	tTMDResult.ucCategory = 0x10;
    tTMDResult.ucModule = 0x11;
    tTMDResult.ucAction = 0x86;
    tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);
	//FDI_MMC_UnMount(1);
	return FAI_NO_ERR;
		
	
TEST_FAIL:
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x11;
	tTMDResult.ucAction = 0x86;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
	ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	
	FAI_TMD_SetResult(&tTMDResult);

	
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
FAI_ERR_CODE FAI_MMC_UploadInfo(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	unsigned char aucSDCardSN[4];
	unsigned char aucResult[16];
	unsigned char ucInfoType;
	unsigned char ucDataType;
	
	FVK_MEM_Set(aucSDCardSN,0x00,sizeof(aucSDCardSN));
	FVK_MEM_Set(aucResult,0x00,sizeof(aucResult));

#if 0
	if( ucArgc != 2 ) /* slotID, InfoType */
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}
#endif
	/* get params */
	FAI_TMD_GetParam(&ucDataType, pucArgList, 0);
	FAI_TMD_GetParam(&ucInfoType, pucArgList, 0);

	FAI_PRINT(FAI_PRT_ERROR, ("[%s] : DataType=0x%x, Infotype 0x%x\n", __func__, ucDataType, ucInfoType));

	memset(aucResult, 0, sizeof(aucResult));

	FDI_MMC_Mount(1, "/mnt/sd0");
	FVK_TASK_Sleep(2000);

	switch( ucInfoType )
	{
		case FAI_MMC_SDCARD_SN:
			eFdiErr = FDI_MMC_GetSerialNumber(aucSDCardSN);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FDI_MMC_GetSerialNumber Error\n",__func__));
				goto TEST_FAIL;
			}
			else
			{
				aucResult[0] = 0x01; /* len1 */
				aucResult[1] = 0x00; /* 0: Interger, 1:string */
				aucResult[2] = 0x04; /*len2 */
				aucResult[3] = aucSDCardSN[0];
				aucResult[4] = aucSDCardSN[1];
				aucResult[5] = aucSDCardSN[2];
				aucResult[6] = aucSDCardSN[3];
			}
			break;
			
		default:
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Not support Type:%d Error\n",__func__,ucInfoType));
			goto TEST_FAIL;
			break;
			
	}
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x11;
	tTMDResult.ucAction = 0x87; /* SDCARD SN */
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	memcpy(&tTMDResult.aucParams[0], aucResult, aucResult[0]+aucResult[2]+2);
	tTMDResult.ucNumOfParam=2;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;


	TEST_FAIL:
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x11;
	tTMDResult.ucAction = 0x87; /* SDCARD SN */
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;

	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
	ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);


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
FAI_ERR_CODE FAI_MMC_File_Write(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;

	unsigned char ucBusWidth;
	char *pcSourceFile, *pcTargetFile;
	int iReadn;
	char *pcFileName;
	unsigned char *pb;
	char buf[80];
	unsigned char ucTmdErrCode[2];

	eTestType = FAI_MMC_TEST_TMD;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	

	unsigned char string[10];
	unsigned long ulCrc;
	pcFileName = "/mnt/sd0/inittab";

	FDI_MMC_UnMount(1);
	FDI_MMC_Mount(1, "/mnt/sd0");
	FVK_TASK_Sleep(2000);
	if(FDI_NO_ERR == FDI_MMC_GetDataLineWidth(&ucBusWidth) && ucBusWidth == 4)
	{
#if 0	
		pcSourceFile = open("/etc/inittab", O_RDONLY | O_NONBLOCK);
		if( pcSourceFile == NULL )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : open Error!!\n"));
			goto TEST_FAIL;
		}
				
		pcTargetFile = open(pcFileName,  O_WRONLY|O_CREAT);
		if( pcTargetFile == NULL )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : open Error!!\n"));
			goto TEST_FAIL;
		}
				
		while((iReadn = read(pcSourceFile, buf, 80)) > 0)
		{
			write(pcTargetFile, buf, iReadn);
		}
		
		close(pcSourceFile);
		close(pcTargetFile);
#endif

		VK_SYSTEM_Command("cp /etc/inittab /mnt/sd0/");
	    tTMDResult.ucCategory = 0x10;
        tTMDResult.ucModule = 0x11;
        tTMDResult.ucAction = 0x85;
        tTMDResult.eResult = FAI_TMD_ERR_OK;
		FAI_TMD_SetResult(&tTMDResult);
		//FDI_MMC_UnMount(1);
		return FAI_NO_ERR;

	
	}
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : not mounted or bus width error\n"));
		goto TEST_FAIL;
	}

TEST_FAIL:
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x11;
	tTMDResult.ucAction = 0x85;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
	ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	
	FAI_TMD_SetResult(&tTMDResult);

	
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

FAI_ERR_CODE FAI_MMC_File_Read(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucBusWidth;
	int nSdFd;
	char *pcFileName;
	int nFileSize, nReadSize;
	unsigned char *pb;
	unsigned char aucAgcResult[4]={0,};
	unsigned int ulNoParms, ulParmLength;
	unsigned char ucTmdErrCode[2];

	unsigned char string[10];
	unsigned long ulCrc;
	pcFileName = "/mnt/sd0/inittab";
	eTestType = FAI_MMC_TEST_TMD;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	FDI_MMC_Mount(1, "/mnt/sd0");
	FVK_TASK_Sleep(500);

	if(FDI_NO_ERR == FDI_MMC_GetDataLineWidth(&ucBusWidth) && ucBusWidth == 4)
	{
		nSdFd = open(pcFileName, O_RDONLY | O_NONBLOCK);
		if( nSdFd < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : open Error!!\n"));
			goto TEST_FAIL;
		}

		nFileSize =lseek(nSdFd, 0, SEEK_END);
		if( nFileSize < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : lseek Error!!\n"));
			goto TEST_FAIL;
		}


		 
		pb= FVK_MEM_Alloc(nFileSize);
		if( pb == NULL )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : FVK_MEM_Alloc Error!!\n"));
			goto TEST_FAIL;
		}

	 
		if( lseek(nSdFd, 0, SEEK_SET) != 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : lseek Error!!\n"));
			goto TEST_FAIL;
		}
		
		/* read file */
		nReadSize = read(nSdFd, pb, nFileSize);
		if( nReadSize != nFileSize )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : read Error!!\n"));
			goto TEST_FAIL;
		}

		close(nSdFd);

		ulCrc = FAI_UTIL_GetCRC32Fast((void *)pb, (unsigned long)nFileSize);

		FAI_PRINT(FAI_PRT_ERROR, ("CRC : %x\n", ulCrc));
		
		VK_MEM_Memset(string, 0x0, 10);

		snprintf(string, 10, "%X", ulCrc);

	    tTMDResult.ucCategory = 0x10;
        tTMDResult.ucModule = 0x11;
        tTMDResult.ucAction = 0x84;
        tTMDResult.eResult = FAI_TMD_ERR_OK;

		aucAgcResult[3] = (ulCrc&0x000000FF);
		aucAgcResult[2] = (ulCrc&0x0000FF00)>>8;
		aucAgcResult[1] = (ulCrc&0x00FF0000)>>16;
		aucAgcResult[0] = (ulCrc&0xFF000000)>>24;
		FAI_TMD_AddResultParam(&tTMDResult, aucAgcResult, sizeof(aucAgcResult));

        FAI_TMD_SetResult(&tTMDResult);
		FVK_MEM_Free(pb);
		//FDI_MMC_UnMount(1);
		return FAI_NO_ERR;
	
	}
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : not mounted or bus width error\n"));
		goto TEST_FAIL;
	}

TEST_FAIL:
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule = 0x11;
	tTMDResult.ucAction = 0x84;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
	ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	
	FAI_TMD_SetResult(&tTMDResult);

	
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
#if (MMC_UI_ENABLE==YES)
#if (MMC_FACTORY_FORMAT_ENABLE==YES)
FAI_ERR_CODE FAI_MMC_Test_UI(void)
{
#if 0
	/* Display usb test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD Card Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	if( s_bMMCInserted == TRUE )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD Card Test", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_MMC_TEST_PASS_STR);
	}
	else
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD Card Test", RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
		FDI_PANEL_Display(FAI_MMC_TEST_FAIL_STR);
	}

	return FAI_NO_ERR;
#endif
	FDI_ERR_CODE eFdiErr;

	unsigned char ucBusWidth;
	int nSdFd;
	char *pcSourceFile, *pcTargetFile;
	int iReadn;
	char *pcFileName;
	int nFileSize, nReadSize;
	unsigned char *pb;
	char buf[200];
	eTestType = FAI_MMC_TEST_UI;

	unsigned char string[10];
	unsigned long ulCrc;
	pcFileName = "/mnt/sd0/inittab";
	
	/* Display usb test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD Card Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "&Fortmat", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_MMC_UnMount(1);
	FVK_TASK_Sleep(500);

	

	if( GET_STEP() == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SDCard Format", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		eFdiErr = FDI_MMC_Format(1);
		SET_STEP(1);
		if(eFdiErr != FDI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : MMC Format error\n"));
			s_bMMCFormatCmd = FALSE;
			goto FORMAT_FAIL;
		}
		s_bMMCFormatCmd = TRUE;
		return FAI_NO_ERR;
		
		
	}
	else if( GET_STEP() == 1 )
	{

		FDI_MMC_Mount(1, "/mnt/sd0");
		FVK_TASK_Sleep(500);

		if(FDI_NO_ERR == FDI_MMC_GetDataLineWidth(&ucBusWidth) && ucBusWidth == 4)
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "BUSWIDTH 4", RGB_COLOR_BLACK);

#if 0
			
			pcSourceFile = open("/etc/inittab", O_RDONLY | O_NONBLOCK);
			if( pcSourceFile == NULL )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : open Error!!\n"));
				goto TEST_FAIL;
			}
					
			pcTargetFile = open(pcFileName,  O_WRONLY|O_CREAT);
			if( pcTargetFile == NULL )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : open Error!!\n"));
				goto TEST_FAIL;
			}
					
			while((iReadn = read(pcSourceFile, buf, 200)) > 0)
		    {
		        write(pcTargetFile, buf, iReadn);
		    }
	 		
			close(pcSourceFile);
			close(pcTargetFile);
#endif			
			VK_SYSTEM_Command("cp /etc/inittab /mnt/sd0/");
			nSdFd = open(pcFileName, O_RDONLY | O_NONBLOCK);
			if( nSdFd < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : open Error!!\n"));
				goto TEST_FAIL;
			}

			nFileSize =lseek(nSdFd, 0, SEEK_END);
			if( nFileSize < 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : lseek Error!!\n"));
				goto TEST_FAIL;
			}


			 
			pb= FVK_MEM_Alloc(nFileSize);
			if( pb == NULL )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : FVK_MEM_Alloc Error!!\n"));
				goto TEST_FAIL;
			}

		 
			if( lseek(nSdFd, 0, SEEK_SET) != 0 )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : lseek Error!!\n"));
				goto TEST_FAIL;
			}
			
			/* read file */
			nReadSize = read(nSdFd, pb, nFileSize);
			if( nReadSize != nFileSize )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : read Error!!\n"));
				goto TEST_FAIL;
			}

			close(nSdFd);

			ulCrc = FAI_UTIL_GetCRC32Fast((void *)pb, (unsigned long)nFileSize);

			FAI_PRINT(FAI_PRT_DBGINFO, ("CRC : %x\n", ulCrc));
			
			VK_MEM_Memset(string, 0x0, 10);
			snprintf(string, 10, "%X", ulCrc);

			eFdiErr = remove("/mnt/sd0/inittab");
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : file remove Error!!\n"));
				goto TEST_FAIL;
			}

			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "CRC", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, string, RGB_COLOR_BLACK);
			FVK_MEM_Free(pb);
			//FDI_MMC_UnMount(1);
			SET_STEP(0);

			return FAI_NO_ERR;
		}
		else
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : not mounted or bus width error\n"));
			goto TEST_FAIL;
		}
	}

	
FORMAT_FAIL:
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SDCard Format", RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
		FDI_PANEL_Display(FAI_MMC_TEST_FAIL_STR);
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : MMC Format error2\n"));
	}
	return FAI_NO_ERR;



TEST_FAIL:
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD Card Test", RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
		FDI_PANEL_Display(FAI_MMC_TEST_FAIL_STR);
		SET_STEP(0);
	}

	return FAI_NO_ERR;

	
}
#else
FAI_ERR_CODE FAI_MMC_Test_UI(void)
{
	unsigned char ucBusWidth;
	int nSdFd;
	char *pcFileName;
	int nFileSize, nReadSize;
	unsigned char *pb;

	unsigned char string[10];
	unsigned long ulCrc;
	pcFileName = "/mnt/sd0/sd_test";
	
	/* Display usb test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD Card Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);


	FDI_MMC_Mount(1, "/mnt/sd0");
	FVK_TASK_Sleep(500);

	if(FDI_NO_ERR == FDI_MMC_GetDataLineWidth(&ucBusWidth) && ucBusWidth == 4)
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "BUSWIDTH 4", RGB_COLOR_BLACK);
		
		nSdFd = open(pcFileName, O_RDONLY | O_NONBLOCK);
		if( nSdFd < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : open Error!!\n"));
			goto TEST_FAIL;
		}

		nFileSize =lseek(nSdFd, 0, SEEK_END);
		if( nFileSize < 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : lseek Error!!\n"));
			goto TEST_FAIL;
		}


		 
		pb= FVK_MEM_Alloc(nFileSize);
		if( pb == NULL )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : FVK_MEM_Alloc Error!!\n"));
			goto TEST_FAIL;
		}

	 
		if( lseek(nSdFd, 0, SEEK_SET) != 0 )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : lseek Error!!\n"));
			goto TEST_FAIL;
		}
		
		/* read file */
		nReadSize = read(nSdFd, pb, nFileSize);
		if( nReadSize != nFileSize )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : read Error!!\n"));
			goto TEST_FAIL;
		}


		ulCrc = FAI_UTIL_GetCRC32Fast((void *)pb, (unsigned long)nFileSize);

		FAI_PRINT(FAI_PRT_ERROR, ("CRC : %x\n", ulCrc));
		
		VK_MEM_Memset(string, 0x0,			10);
		snprintf(string, 10, "%X", ulCrc);

		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "CRC", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, string, RGB_COLOR_BLACK);

//		FDI_MMC_UnMount(1);

		return FAI_NO_ERR;
	}
	else
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Test_UI] : not mounted or bus width error\n"));
		goto TEST_FAIL;
	}

TEST_FAIL:
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD Card Test", RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
		FDI_PANEL_Display(FAI_MMC_TEST_FAIL_STR);
	}

	return FAI_NO_ERR;
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
FAI_ERR_CODE INTRN_FAI_MMC_NotifyFunc(FDI_MMC_EVENT_e eEventType, FDI_MMC_INFO_t *ptMMCInfo)
{
	FAI_TESTMENU_H hMenu;
	FDI_ERR_CODE eFdiErr=0;
	unsigned char ucSDCardSN[8];
	unsigned char string[8];
	unsigned long ulSDCardSN=0;
	
	FVK_TASK_Sleep(3000);
	
#if (SDCARD_DETECT_UI_ENABLE==YES)
	FAI_APP_GetTestMenuHandle(SDCARD_DETECT_UI_SHEET, SDCARD_DETECT_UI_CELL, &hMenu);
#endif

	if(eEventType == FDI_MMC_EVENT_INSERT )
	{
		printf("FDI_MMC_EVENT_INSERT\n");
		s_bMMCInserted = TRUE;
#if (SDCARD_DETECT_UI_ENABLE==YES)		
		FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "SD CARD", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "Inserted", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);	
#endif		
#if (MMC_SD_CARD_GET_SN_ENABLE==YES)
		FVK_MEM_Set(ucSDCardSN,0x0,sizeof(ucSDCardSN));
		FVK_MEM_Set(string,0x0,sizeof(string));
		FDI_MMC_Mount(1, "/mnt/sd0");
		FVK_TASK_Sleep(2000);
		eFdiErr = FDI_MMC_GetSerialNumber(&ucSDCardSN[0]);
		FAI_PRINT(FAI_PRT_ERROR, ("FDI_MMC_GetSerialNumber eFdiErr=0x%x, SN=0x%x%x%x%x \n",eFdiErr, ucSDCardSN[0],ucSDCardSN[1],ucSDCardSN[2],ucSDCardSN[3]));
		if(eFdiErr==FDI_NO_ERR)
		{
			ulSDCardSN = (ucSDCardSN[0] << 24) | (ucSDCardSN[1] << 16)| (ucSDCardSN[2] << 8) | (ucSDCardSN[3]);
			sprintf(string, "S/N:%08X",ulSDCardSN);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, string, RGB_COLOR_BLACK);
		}
		else
		{
			FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_RED);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "SD CARD", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "S/N Read", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_4, "Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);				
		}
#endif

	}
	else if( eEventType == FDI_MMC_EVENT_EXTRACT )
	{
		printf("FDI_MMC_EVENT_EXTRACT\n");
		s_bMMCInserted = FALSE;
#if (SDCARD_DETECT_UI_ENABLE==YES)		
		FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "SD CARD", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "Removed", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);		
#endif		
	}
	
	return FDI_NO_ERR;
}

FAI_ERR_CODE INTRN_FAI_MMC_SvcNotifyFunc(FDI_MMC_SVC_EVT_e eEventType)
{
		
	FVK_ERR_CODE eVkRet;
	FAI_MMC_MSG_t tMmcMsg;
    printf("[hee] SVC event received. (%d)\n", eEventType);
	if( eEventType == FDI_MMC_SVC_EVT_FORMAT_DONE )
	{
		s_bMMCFormatted = TRUE;
	}
	else if(eEventType == FDI_MMC_SVC_EVT_FORMAT_FAIL )
	{
		s_bMMCFormatted = FALSE;
	}

	tMmcMsg.bFormatted= eEventType;

	eVkRet = FVK_MSG_SendTimeout(g_ulMMCMsgId, &tMmcMsg, sizeof(FAI_MMC_MSG_t),0);
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MMC_SvcNotifyFunc] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FDI_ERR;
	}
    return FDI_NO_ERR;
}



