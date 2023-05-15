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
#include "fai_channel.h"
#include "fdi_channel.h"

/* model specific configuration header */
#include "factorysw_config.h"


/*******************************************************************************
* Debug level
*******************************************************************************/


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Global variables and structures
*******************************************************************************/
unsigned long g_ulNumofCHDevice = 0;
FAI_CH_TASK_t *g_pstCHTask = NULL;

static void INTRN_FAI_CHANNEL_Task(void *data);


/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_Init(void)
{
	int i;

	FDI_CHANNEL_GetNumOfDevice(&g_ulNumofCHDevice);

	if(g_ulNumofCHDevice == 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Error Number of Channel device = %d\n", __func__, g_ulNumofCHDevice));
		return FVK_ERR;
	}

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("Number of channel device = %d\n", g_ulNumofCHDevice));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	g_pstCHTask = (FAI_CH_TASK_t *)FVK_MEM_Alloc(g_ulNumofCHDevice*sizeof(FAI_CH_TASK_t));

	if(g_pstCHTask == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Error FVK_MEM_Alloc\n", __func__));
		return FVK_ERR;
	}

	for(i=0; i<g_ulNumofCHDevice; i++)
	{
		VK_MEM_Memset(&g_pstCHTask[i], 0x00, sizeof(FAI_CH_TASK_t));

		g_pstCHTask[i].ulCHID = i;
		sprintf(g_pstCHTask[i].aucMSGQName, "Ch%dMsg", i);
		sprintf(g_pstCHTask[i].aucTaskName, "Ch%dTask", i);

		FAI_PRINT(FAI_PRT_DBGINFO, ("ID=%d, %s & %s create\n", g_pstCHTask[i].ulCHID, g_pstCHTask[i].aucTaskName, g_pstCHTask[i].aucMSGQName));
		
		/* Create channel msg queue */
		if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_CH_MSG_t), (char*)(g_pstCHTask[i].aucMSGQName), &g_pstCHTask[i].ulMSGQID) != FVK_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FVK_MSG_Create Error!!\n", __func__));
			return FVK_ERR;
		}

		/* Create channel task */
		if( FVK_TASK_Create(INTRN_FAI_CHANNEL_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, (char*)(g_pstCHTask[i].aucTaskName), (void*)(&g_pstCHTask[i].ulCHID), &g_pstCHTask[i].ulTaskID) != FVK_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FVK_TASK_Create Error!!\n", __func__));
			return FVK_ERR;
		}

		/* start channel task */
		if( FVK_TASK_Start(g_pstCHTask[i].ulTaskID) != FVK_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : FVK_TASK_Start Error!!\n", __func__));
			return FVK_ERR;
		}
	}

#if defined(FACTORY_USE_CHANNEL_SAT)
	FAI_CHANNEL_S_Init();
#endif
#if defined(FACTORY_USE_CHANNEL_CAB)
	FAI_CHANNEL_C_Init();
#endif
#if defined(FACTORY_USE_CHANNEL_TER)
	FAI_CHANNEL_T_Init();
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
static void INTRN_FAI_CHANNEL_Task(void *data)
{
	unsigned long ulCHID;
	FAI_CH_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;

	ulCHID = *((unsigned long *)data);

	FAI_PRINT(FAI_PRT_DBGINFO, ("%s\n", g_pstCHTask[ulCHID].aucTaskName));
	
	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_pstCHTask[ulCHID].ulMSGQID, &tChMsg, sizeof(FAI_CH_MSG_t));
		if ( eVkRet != FVK_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[%s] : Error FVK_MSG_Receive() %d\n", __func__, eVkRet));
			continue;
		}
		
		if(tChMsg.ulTunerId >= g_ulNumofCHDevice)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[%s] : Error Invalid Tuner ID %d\n", __func__, tChMsg.ulTunerId));
			continue;
		}
		
		switch(tChMsg.eChTestType)
		{
#if (CHANNEL_SAT_LOCK_REQUEST_ENABLE==YES)
			case FAI_CHANNEL_TEST_TYPE_SAT_REQUEST_LOCK:
				FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : SAT lock test\n", g_pstCHTask[ulCHID].aucTaskName));
				if(g_pstCHTask[ulCHID].pfnSATRequestLock != NULL)
				{
					g_pstCHTask[ulCHID].pfnSATRequestLock(tChMsg.usMsgId, tChMsg.ulTunerId, tChMsg.tSatTuneParam);
				}
				break;
#endif
#if (CHANNEL_SAT_LOCK_FOR_PERFORMANCE_ENABLE==YES)
			case FAI_CHANNEL_TEST_TYPE_SAT_CHECK_PERFORMANCE:
				FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : SAT performance test\n", g_pstCHTask[ulCHID].aucTaskName));
				if(g_pstCHTask[ulCHID].pfnSATCheckPerformance != NULL)
				{
					g_pstCHTask[ulCHID].pfnSATCheckPerformance(tChMsg.usMsgId, tChMsg.ulTunerId, tChMsg.tSatTuneParam);
				}
				break;
#endif
#if (CHANNEL_CAB_LOCK_REQUEST_ENABLE==YES)
			case FAI_CHANNEL_TEST_TYPE_CAB_REQUEST_LOCK:
				FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : CAB lock test\n", g_pstCHTask[ulCHID].aucTaskName));
				if(g_pstCHTask[ulCHID].pfnCABRequestLock != NULL)
				{
					g_pstCHTask[ulCHID].pfnCABRequestLock(tChMsg.usMsgId, tChMsg.ulTunerId, tChMsg.tCabTuneParam);
				}
				break;
#endif
#if (CHANNEL_CAB_LOCK_FOR_PERFORMANCE_ENABLE==YES)
			case FAI_CHANNEL_TEST_TYPE_CAB_CHECK_PERFORMANCE:
				FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : CAB performance test\n", g_pstCHTask[ulCHID].aucTaskName));
				if(g_pstCHTask[ulCHID].pfnCABCheckPerformance != NULL)
				{
					g_pstCHTask[ulCHID].pfnCABCheckPerformance(tChMsg.usMsgId, tChMsg.ulTunerId, tChMsg.tCabTuneParam);
				}
				break;
#endif				
#if (CHANNEL_TER_LOCK_REQUEST_ENABLE==YES)
			case FAI_CHANNEL_TEST_TYPE_TER_REQUEST_LOCK:
				FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : TER lock test\n", g_pstCHTask[ulCHID].aucTaskName));
				if(g_pstCHTask[ulCHID].pfnTERRequestLock != NULL)
				{
					g_pstCHTask[ulCHID].pfnTERRequestLock(tChMsg.usMsgId, tChMsg.ulTunerId, tChMsg.tTerTuneParam);
				}
				break;
#endif				
#if (CHANNEL_TER_LOCK_FOR_PERFORMANCE_ENABLE==YES)
			case FAI_CHANNEL_TEST_TYPE_TER_CHECK_PERFORMANCE:
				FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : TER performance test\n", g_pstCHTask[ulCHID].aucTaskName));
				if(g_pstCHTask[ulCHID].pfnTERCheckPerformance != NULL)
				{
					g_pstCHTask[ulCHID].pfnTERCheckPerformance(tChMsg.usMsgId, tChMsg.ulTunerId, tChMsg.tTerTuneParam);
				}
				break;
#endif				
			default :
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Error Unknown channel test type() %d\n", g_pstCHTask[ulCHID].aucTaskName, tChMsg.eChTestType));
				break;
		}
 	}
}


