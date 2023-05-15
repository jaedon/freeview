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

/* model specific configuration header */
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"
#include "fai_cablemodem.h"
#include "fdi_video.h"
#include "fdi_panel.h"
#include "factorysw_config.h"


#define FAI_TMD_CATEGORY_PERIPHERAL	0x10
#define FAI_TMD_MODULE_CABLEMODEM		0x09
#define FAI_TMD_ACTION_TEST_CM 			0x01
#define FAI_CM_MODULATOR_CH_MAX 		4
#define FAI_CM_STATUS_ONLINE_START		6
#define FAI_CM_STATUS_ONLINE_END		13


/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

typedef struct
{
	BOOL bCMStart;	
} FAI_CM_MSG_t;

/*******************************************************************************
* Function Prototype
*******************************************************************************/
void INTRN_FAI_CM_TestTask(void *data);
FAI_ERR_CODE FAI_CABLEMODEM_Init();
FAI_ERR_CODE FAI_CABLEMODEM_Check(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */


/*******************************************************************************
* Global variables and structures
*******************************************************************************/
unsigned long g_ulCMMsgId, g_ulCMTaskId;
BOOL 	bCableModemStatus;

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CABLEMODEM_Init()
{
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_CM_MSG_t), (char*)"CMMsg", &g_ulCMMsgId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CABLEMODEM_Init] : FVK_MSG_Create \n"));
		return FAI_ERR;
	}

	if( FVK_TASK_Create(INTRN_FAI_CM_TestTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "CM Task", NULL, &g_ulCMTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CABLEMODEM_Init] : FVK_TASK_Create Error!!\n"));
		return FAI_ERR;
	}
	if( FVK_TASK_Start(g_ulCMTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CABLEMODEM_Init] : FVK_TASK_Start Error!!\n"));
		return FAI_ERR;
	}
	return FAI_NO_ERR;	
}



/*******************************************************************************
* function : INTRN_FAI_CM_TestTask
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_CM_TestTask(void *data)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_CM_MSG_t tCMMsg;
	FVK_ERR_CODE eVkRet;
	unsigned char ucTmdErrCode[2];
	int ret, i=0;
	unsigned int Power[4]={0,};
	unsigned int Snr[4]={0,};
	unsigned char aucResult[24]={0,};
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
	tTMDResult.ucCategory = FAI_TMD_CATEGORY_PERIPHERAL;
	tTMDResult.ucModule = FAI_TMD_MODULE_CABLEMODEM;
	tTMDResult.ucAction = FAI_TMD_ACTION_TEST_CM;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;

	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulCMMsgId, &tCMMsg, sizeof(FAI_CM_MSG_t));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_CM_TestTask] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}		

		if( tCMMsg.bCMStart == TRUE)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CM_TestTask] : CM Test start msg received!\n"));
			FDI_CABLEMODEM_Scan();

			ret=FDI_CABLEMODEM_Status();	/* online status checking wait time about. 15 secs */
			if((ret>FAI_CM_STATUS_ONLINE_START)&&(ret<FAI_CM_STATUS_ONLINE_END))
			{
				bCableModemStatus=TRUE;

				for(i=0; i<FAI_CM_MODULATOR_CH_MAX; i++)
				{
					FDI_CABLEMODEM_GetSNR(i, (unsigned int *)&Snr[i]);
			
					if(Snr[i]>=(0xFFFFFFFF-1)) /* DI_CM.c default value : 0xFFFFFFFF-1*/
					{
						Snr[i]=0x0; 	/* NPI 협의. default snr value = 0*/
					}
					aucResult[0+i*3] = (Snr[i]&0xFF0000)>>16;
					aucResult[1+i*3]= (Snr[i]&0xFF00)>>8;
					aucResult[2+i*3]= (Snr[i]&0xFF);
				}
#if 0	/* TX POWER는 NPI에서 삭제 요청함. 나중에 필요하면 추가*/
				for(i=0; i<FAI_CM_MODULATOR_CH_MAX; i++)
				{
					FDI_CABLEMODEM_GetPWR(i, (unsigned int *)&Power[i]);
			
					if(Power[i]>=(0xFFFFFFFF-1)) /* DI_CM.c default value : 0xFFFFFFFF-1*/
					{
						Power[i]=0x0; 	/* NPI 협의. default power value = 0*/
					}
					aucResult[12+i*3] = (Power[i]&0xFF0000)>>16;
					aucResult[13+i*3]= (Power[i]&0xFF00)>>8;
					aucResult[14+i*3]= (Power[i]&0xFF);
				}				
#endif
				bCableModemStatus=TRUE;
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CM_TestTask] : CM STATUS ONLINE detect!\n"));
			}
			else
			{
				bCableModemStatus=FALSE;
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CM_TestTask] : CM STATUS OFFLINE detect!\n"));
			}
			
			if( bCableModemStatus == TRUE )
			{
				FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
				tTMDResult.ucCategory = FAI_TMD_CATEGORY_PERIPHERAL;
				tTMDResult.ucModule = FAI_TMD_MODULE_CABLEMODEM;
				tTMDResult.ucAction = FAI_TMD_ACTION_TEST_CM;
				tTMDResult.eResult = FAI_TMD_ERR_OK;
				FAI_TMD_AddResultParam(&tTMDResult, &aucResult[0], 3); /* result 3bytes -Modulator Ch 1 SNR */
				FAI_TMD_AddResultParam(&tTMDResult, &aucResult[3], 3); /* result 3bytes -Modulator Ch 2 SNR */
				FAI_TMD_AddResultParam(&tTMDResult, &aucResult[6], 3); /* result 3bytes -Modulator Ch 3 SNR */
				FAI_TMD_AddResultParam(&tTMDResult, &aucResult[9], 3); /* result 3bytes -Modulator Ch 4 SNR */
				FAI_TMD_SetResult(&tTMDResult);
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CM_TestTask] : result: ONLINE\n"));
			}
			else
			{
				FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
				tTMDResult.ucCategory = FAI_TMD_CATEGORY_PERIPHERAL;
				tTMDResult.ucModule = FAI_TMD_MODULE_CABLEMODEM;
				tTMDResult.ucAction = FAI_TMD_ACTION_TEST_CM;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_CABLEMODEM;
				ucTmdErrCode[1] = FAI_TMD_ERR_OK;
				
				FVK_MEM_Set(aucResult, 0x00, sizeof(aucResult));
				FAI_TMD_AddResultParam(&tTMDResult, &aucResult[0], 3); /* SNR = 0 */
				FAI_TMD_AddResultParam(&tTMDResult, &aucResult[3], 3); /* SNR = 0 */
				FAI_TMD_AddResultParam(&tTMDResult, &aucResult[6], 3); /* SNR = 0 */
				FAI_TMD_AddResultParam(&tTMDResult, &aucResult[9], 3); /* SNR = 0 */				
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));			
				FAI_TMD_SetResult(&tTMDResult);
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CM_TestTask] : result: OFFLINE\n"));
			}
		}
	}
}


/*******************************************************************************
* function : FAI_CABLEMODEM_Check
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CABLEMODEM_Check(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucOnOff;
	unsigned char ucTmdErrCode[2];
	FAI_CM_MSG_t tCMMsg;
	FVK_ERR_CODE eVkRet;
	
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CABLEMODEM_Check] : Wrong parameter count=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_CABLEMODEM;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}
	tCMMsg.bCMStart = TRUE;
	eVkRet = FVK_MSG_Send(g_ulCMMsgId, &tCMMsg, sizeof(FAI_CM_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CM_CheckStartCallback] : Error FVK_MSG_Send() : %d\n", eVkRet));
		eTmdErr=FAI_TMD_ERR_FAIL;
		goto END;
	}

	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CABLEMODEM_Check] : Check Start!\n"));
	FDI_PANEL_Display(FAI_TESTMODE_STR);
	return FAI_NO_ERR;

END:
	
	tTMDResult.ucCategory = FAI_CATEGORY_PERIPHERAL;
	tTMDResult.ucModule = FAI_TMD_MODULE_CABLEMODEM;
	tTMDResult.ucAction = FAI_TMD_ACTION_TEST_CM;
	tTMDResult.eResult = eTmdErr;
	
	FAI_TMD_SetResult(&tTMDResult);
	FDI_PANEL_Display(FAI_CM_FAIL_STR);

	return FAI_NO_ERR;
}

