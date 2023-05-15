/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_snmp.c
// Original Author:
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved.												*/
/*******************************************************************/

/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

/*For Shared Memory*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*For Semapohre*/
#include <sys/sem.h>

#include "htype.h"
#include "vkernel.h"

/* di headers */
#include "di_snmp.h"

#include "di_err.h"
#include "di_channel.h"

#if defined (CONFIG_FRONTEND_SAT)
#include "di_channel_s.h"
#endif
#if defined (CONFIG_FRONTEND_CAB)
#include "di_channel_c.h"
#endif
#if defined (CONFIG_FRONTEND_TER)
#include "di_channel_t.h"
#endif
#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/



/* NEW DI DEBUG PRINT */

HINT32 show_di_snmp = 0;
#define SNMP_DI_Print(x, y)	( ((x) <= show_di_snmp) ? (DI_UART_Print("@DI_SNMP "),DI_UART_Print y) : 0 )

#define UNUSED_PARAM(x)  ((void)x)

/******************************************************************************
 * Data Types
 ******************************************************************************/

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/
DI_SNMP_Information_t 	g_stSnmpInfo;
/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/

static	DI_SNMP_Information_t 	*s_pstSnmpInfo; 	 /* Shared Memory Pointer*/

static int s_nSnmpShmID;	  /* Shared Memory Indicator */
static int s_nSnmpSemaID;	  /* Semaphore Indicator */
static unsigned long   s_ulSnmpTaskId;	/* SNMP task Indicator */

static struct sembuf stSemaOpen  = {0, -1, SEM_UNDO}; // Semaphore Open
static struct sembuf stSemaClose = {0, 1, SEM_UNDO};  // Semaphore Release


/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/

//////// For Debugging ////////////

static void P_SNMP_PrintMibInfo( DI_SNMP_Information_t *pstSnmpAllInfo)
{

	SNMP_DI_Print(1, ( "szSystemId : %s\n" , pstSnmpAllInfo->stSnmpMWInfo.szSystemId ));
	SNMP_DI_Print(1, ( "szApplicationVer : %s\n" , pstSnmpAllInfo->stSnmpMWInfo.szApplicationVer ));
	SNMP_DI_Print(1, ( "szMiddleWareVer : %s\n" , pstSnmpAllInfo->stSnmpMWInfo.szMiddleWareVer ));
	SNMP_DI_Print(1, ( "szSoftBrowserVer : %s\n" , pstSnmpAllInfo->stSnmpMWInfo.szSoftBrowserVer) );
	SNMP_DI_Print(1, ( "szStbID : %02X%02X%02X%02X%02X%02X\n" , pstSnmpAllInfo->stSnmpMWInfo.szStbID[0],
		pstSnmpAllInfo->stSnmpMWInfo.szStbID[1],pstSnmpAllInfo->stSnmpMWInfo.szStbID[2],pstSnmpAllInfo->stSnmpMWInfo.szStbID[3],
		pstSnmpAllInfo->stSnmpMWInfo.szStbID[4],pstSnmpAllInfo->stSnmpMWInfo.szStbID[5]));

	SNMP_DI_Print(1, ( "szBcasCardId : %s\n" , pstSnmpAllInfo->stSnmpMWInfo.szBcasCardId ));
	SNMP_DI_Print(1, ( "szCcasCardId : %s\n" , pstSnmpAllInfo->stSnmpMWInfo.szCcasCardId ));

	SNMP_DI_Print(1, ( "ulDownChannelPower : %d\n", pstSnmpAllInfo->stSnmpDIInfo.ulDownChannelPower));

	SNMP_DI_Print(1, ( "ulDownFrequency : %d\n", pstSnmpAllInfo->stSnmpMWInfo.ulDownFrequency ));

	SNMP_DI_Print(1, ( "etWatchingChannelType : %d\n",  pstSnmpAllInfo->stSnmpMWInfo.etWatchingChannelType ));
	SNMP_DI_Print(1, ( "ulWatchingTunerGroupId : %d\n",  pstSnmpAllInfo->stSnmpMWInfo.ulWatchingTunerGroupId ));
	SNMP_DI_Print(1, ( "ulWatchingChannelId : %d\n", pstSnmpAllInfo->stSnmpMWInfo.ulWatchingChannelId ));
	SNMP_DI_Print(1, ( "ulWatchingNetworkId : %d\n", pstSnmpAllInfo->stSnmpMWInfo.ulWatchingNetworkId ));

	SNMP_DI_Print(1, ( "ulUnErroredNo : %d\n", pstSnmpAllInfo->stSnmpDIInfo.ulUnErroredNo));
	SNMP_DI_Print(1, ( "ulCorrectedNo : %d\n", pstSnmpAllInfo->stSnmpDIInfo.ulCorrectedNo ));
	SNMP_DI_Print(1, ( "ulUnCorrectedNo : %d\n", pstSnmpAllInfo->stSnmpDIInfo.ulUnCorrectedNo));
	SNMP_DI_Print(1, ( "ulInbandBer: %d\n", pstSnmpAllInfo->stSnmpDIInfo.ulInbandBer));

	SNMP_DI_Print(1, ( "etPowerStatus : %d\n", (HINT32)pstSnmpAllInfo->stSnmpMWInfo.etPowerStatus ));
	SNMP_DI_Print(1, ( "etPlayStatus : %d\n", (HINT32)pstSnmpAllInfo->stSnmpMWInfo.etPlayStatus ));

	SNMP_DI_Print(1, ( "ulTuner1ChannelId : %d\n", pstSnmpAllInfo->stSnmpMWInfo.ulTuner1ChannelId ));
	SNMP_DI_Print(1, ( "ulTuner1NetworkId : %d\n", pstSnmpAllInfo->stSnmpMWInfo.ulTuner1NetworkId ));
	SNMP_DI_Print(1, ( "ulTuner1Frequency : %d\n", pstSnmpAllInfo->stSnmpMWInfo.ulTuner1Frequency ));

	SNMP_DI_Print(1, ( "ulTuner2ChannelId : %d\n", pstSnmpAllInfo->stSnmpMWInfo.ulTuner2ChannelId ));
	SNMP_DI_Print(1, ( "ulTuner2NetworkId : %d\n", pstSnmpAllInfo->stSnmpMWInfo.ulTuner2NetworkId ));
	SNMP_DI_Print(1, ( "ulTuner2Frequency : %d\n", pstSnmpAllInfo->stSnmpMWInfo.ulTuner2Frequency ));


}

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
static void P_SNMP_TestMibInfo( void)
{

	VK_sprintf(g_stSnmpInfo.stSnmpMWInfo.szSystemId, "%s",  "1234.5678");
	VK_sprintf(g_stSnmpInfo.stSnmpMWInfo.szApplicationVer, "%s", "1.01");
	VK_sprintf(g_stSnmpInfo.stSnmpMWInfo.szMiddleWareVer, "%s", "3.4.5");
	VK_sprintf(g_stSnmpInfo.stSnmpMWInfo.szSoftBrowserVer, "%s", "0.5");
	VK_sprintf(g_stSnmpInfo.stSnmpMWInfo.szStbID, "%s", "789");
	VK_sprintf(g_stSnmpInfo.stSnmpMWInfo.szBcasCardId, "%s", "bcas123");
	VK_sprintf(g_stSnmpInfo.stSnmpMWInfo.szCcasCardId, "%s", "ccas123");
	g_stSnmpInfo.stSnmpDIInfo.ulDownChannelPower = 55;
	g_stSnmpInfo.stSnmpMWInfo.etWatchingChannelType = 1;
	g_stSnmpInfo.stSnmpMWInfo.ulWatchingTunerGroupId = 1;
	g_stSnmpInfo.stSnmpMWInfo.ulWatchingChannelId = 1010;
	g_stSnmpInfo.stSnmpMWInfo.ulWatchingNetworkId = 5962;
	g_stSnmpInfo.stSnmpDIInfo.ulUnErroredNo = 100;
	g_stSnmpInfo.stSnmpDIInfo.ulCorrectedNo = 22;
	g_stSnmpInfo.stSnmpDIInfo.ulUnCorrectedNo = 13;
	g_stSnmpInfo.stSnmpDIInfo.ulInbandBer = 71;
	g_stSnmpInfo.stSnmpMWInfo.etPowerStatus = 1;
	g_stSnmpInfo.stSnmpMWInfo.etPlayStatus = 1;
	g_stSnmpInfo.stSnmpMWInfo.ulTuner1ChannelId = 100;
	g_stSnmpInfo.stSnmpMWInfo.ulTuner1NetworkId = 101;
	g_stSnmpInfo.stSnmpMWInfo.ulTuner1Frequency = 435;
	g_stSnmpInfo.stSnmpMWInfo.ulTuner2ChannelId = 200;
	g_stSnmpInfo.stSnmpMWInfo.ulTuner2NetworkId = 201;
	g_stSnmpInfo.stSnmpMWInfo.ulTuner2Frequency = 747;

	SNMP_DI_Print(0, ( "szBcasCardId : %s\n" , g_stSnmpInfo.stSnmpMWInfo.szBcasCardId ));
	SNMP_DI_Print(0, ( "szCcasCardId : %s\n" , g_stSnmpInfo.stSnmpMWInfo.szCcasCardId ));
	SNMP_DI_Print(0, ( "ulDownChannelPower : %d\n", g_stSnmpInfo.stSnmpDIInfo.ulDownChannelPower));

}

#endif

static void P_SNMP_SnmpTask(void *param)
{
	int nRet = DI_CH_OK;
	DI_CH_SignalQuality_t stSignalInfo;

	UNUSED_PARAM(param);

    VK_memset(&stSignalInfo,0, sizeof(DI_CH_SignalQuality_t));

	while (1)
	{
		SNMP_DI_Print(3, (" P_SNMP_SnmpTask \n"));
		semop(s_nSnmpSemaID, &stSemaOpen, 1);

		if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_TER )
		{
#if defined (CONFIG_FRONTEND_TER)
			CH_TER_TuneParam_t tDiTerParam;
			if(DI_CH_T_GetLockedInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &tDiTerParam) == DI_CH_OK)
			{
			nRet = DI_CH_T_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
			if (nRet != DI_CH_OK)
			{
				SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
			}
				else
				{
					s_pstSnmpInfo->stSnmpDIInfo.ulDownChannelPower = (HUINT32) (stSignalInfo.fSignalInputPower*10) - 600;
					s_pstSnmpInfo->stSnmpDIInfo.ulUnErroredNo =  stSignalInfo.ulUnErroredNo;
					s_pstSnmpInfo->stSnmpDIInfo.ulCorrectedNo = stSignalInfo.ulCorrectedNo;
					s_pstSnmpInfo->stSnmpDIInfo.ulUnCorrectedNo = stSignalInfo.ulUnCorrectedNo;
					s_pstSnmpInfo->stSnmpDIInfo.ulInbandBer = (HUINT32)stSignalInfo.fBer;

					g_stSnmpInfo.stSnmpDIInfo.ulDownChannelPower = (HUINT32) (stSignalInfo.fSignalInputPower*10) - 600;
					g_stSnmpInfo.stSnmpDIInfo.ulUnErroredNo =  stSignalInfo.ulUnErroredNo;
					g_stSnmpInfo.stSnmpDIInfo.ulCorrectedNo = stSignalInfo.ulCorrectedNo;
					g_stSnmpInfo.stSnmpDIInfo.ulUnCorrectedNo = stSignalInfo.ulUnCorrectedNo;
					g_stSnmpInfo.stSnmpDIInfo.ulInbandBer = (HUINT32)stSignalInfo.fBer;
				}
			}
			else
			{
				s_pstSnmpInfo->stSnmpDIInfo.ulDownChannelPower = 0;
				s_pstSnmpInfo->stSnmpDIInfo.ulUnErroredNo =  0;
				s_pstSnmpInfo->stSnmpDIInfo.ulCorrectedNo = 0;
				s_pstSnmpInfo->stSnmpDIInfo.ulUnCorrectedNo = 0;
				s_pstSnmpInfo->stSnmpDIInfo.ulInbandBer = 0;

				g_stSnmpInfo.stSnmpDIInfo.ulDownChannelPower = 0;
				g_stSnmpInfo.stSnmpDIInfo.ulUnErroredNo =  0;
				g_stSnmpInfo.stSnmpDIInfo.ulCorrectedNo = 0;
				g_stSnmpInfo.stSnmpDIInfo.ulUnCorrectedNo = 0;
				g_stSnmpInfo.stSnmpDIInfo.ulInbandBer = 0;
			}
#endif
		}
		else if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_CAB )
		{
#if defined (CONFIG_FRONTEND_CAB)
			CH_CAB_TuneParam_t tDiCabParam;
			if(DI_CH_C_GetLockedInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &tDiCabParam) == DI_CH_OK)
			{
				nRet = DI_CH_C_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
				if (nRet != DI_CH_OK)
				{
					SNMP_DI_Print(0, (" DI_CH_C_GetSignalInfo Error(%d)\n", nRet));
				}
				else
				{
					s_pstSnmpInfo->stSnmpDIInfo.ulDownChannelPower = (HUINT32) (stSignalInfo.fSignalInputPower*10) - 600;
					s_pstSnmpInfo->stSnmpDIInfo.ulUnErroredNo =  stSignalInfo.ulUnErroredNo;
					s_pstSnmpInfo->stSnmpDIInfo.ulCorrectedNo = stSignalInfo.ulCorrectedNo;
					s_pstSnmpInfo->stSnmpDIInfo.ulUnCorrectedNo = stSignalInfo.ulUnCorrectedNo;
					s_pstSnmpInfo->stSnmpDIInfo.ulInbandBer = (HUINT32)stSignalInfo.fBer;

					g_stSnmpInfo.stSnmpDIInfo.ulDownChannelPower = (HUINT32) (stSignalInfo.fSignalInputPower*10) - 600;
					g_stSnmpInfo.stSnmpDIInfo.ulUnErroredNo =  stSignalInfo.ulUnErroredNo;
					g_stSnmpInfo.stSnmpDIInfo.ulCorrectedNo = stSignalInfo.ulCorrectedNo;
					g_stSnmpInfo.stSnmpDIInfo.ulUnCorrectedNo = stSignalInfo.ulUnCorrectedNo;
					g_stSnmpInfo.stSnmpDIInfo.ulInbandBer = (HUINT32)stSignalInfo.fBer;
				}
			}
			else
			{
				s_pstSnmpInfo->stSnmpDIInfo.ulDownChannelPower = 0;
				s_pstSnmpInfo->stSnmpDIInfo.ulUnErroredNo =  0;
				s_pstSnmpInfo->stSnmpDIInfo.ulCorrectedNo = 0;
				s_pstSnmpInfo->stSnmpDIInfo.ulUnCorrectedNo = 0;
				s_pstSnmpInfo->stSnmpDIInfo.ulInbandBer = 0;

				g_stSnmpInfo.stSnmpDIInfo.ulDownChannelPower = 0;
				g_stSnmpInfo.stSnmpDIInfo.ulUnErroredNo =  0;
				g_stSnmpInfo.stSnmpDIInfo.ulCorrectedNo = 0;
				g_stSnmpInfo.stSnmpDIInfo.ulUnCorrectedNo = 0;
				g_stSnmpInfo.stSnmpDIInfo.ulInbandBer = 0;
			}
#endif
		}
		switch (s_pstSnmpInfo->stSnmpDIInfo.etSnmpEvent)
		{
			case PRIV_stbIfDownChannelPower:						//DI setting
			{
				if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_TER )
				{
#if defined (CONFIG_FRONTEND_TER)

					nRet = DI_CH_T_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}
#endif
				}
				else if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_CAB )
				{
#if defined (CONFIG_FRONTEND_CAB)

					nRet = DI_CH_C_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}
#endif
				}
				s_pstSnmpInfo->stSnmpDIInfo.ulDownChannelPower = (HUINT32) (stSignalInfo.fSignalInputPower*10) - 600;
				s_pstSnmpInfo->stSnmpDIInfo.etSnmpEvent=0;
				break;
			}
			case PRIV_stbIfSigQUnerroreds:							//DI setting
			{
				if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_TER )
				{
#if defined (CONFIG_FRONTEND_TER)

					nRet = DI_CH_T_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}

#endif
				}
				else if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_CAB )
				{
#if defined (CONFIG_FRONTEND_CAB)

					nRet = DI_CH_C_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}
#endif
				}

				s_pstSnmpInfo->stSnmpDIInfo.ulUnErroredNo =  stSignalInfo.ulUnErroredNo;
				s_pstSnmpInfo->stSnmpDIInfo.etSnmpEvent=0;
				break;
			}
			case PRIV_stbIfSigQCorrecteds:							//DI setting
			{
				if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_TER )
				{
#if defined (CONFIG_FRONTEND_TER)

					nRet = DI_CH_T_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}

#endif
				}
				else if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_CAB )
				{
#if defined (CONFIG_FRONTEND_CAB)

					nRet = DI_CH_C_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}
#endif
				}

				s_pstSnmpInfo->stSnmpDIInfo.ulCorrectedNo = stSignalInfo.ulCorrectedNo;
				s_pstSnmpInfo->stSnmpDIInfo.etSnmpEvent=0;
				break;
			}
			case PRIV_stbIfSigQUncorrecteds:						//DI setting
			{
				if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_TER )
				{
#if defined (CONFIG_FRONTEND_TER)

					nRet = DI_CH_T_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}

#endif
				}
				else if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_CAB )
				{
#if defined (CONFIG_FRONTEND_CAB)

					nRet = DI_CH_C_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}
#endif
				}

				s_pstSnmpInfo->stSnmpDIInfo.ulUnCorrectedNo = stSignalInfo.ulUnCorrectedNo;
				s_pstSnmpInfo->stSnmpDIInfo.etSnmpEvent=0;
				break;
			}
			case PRIV_stbIfSigQBER:									//DI setting
			{
				if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_TER )
				{
#if defined (CONFIG_FRONTEND_TER)

					nRet = DI_CH_T_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}

#endif
				}
				else if(s_pstSnmpInfo->stSnmpMWInfo.etWatchingChannelType == DI_CH_TYPE_CAB )
				{
#if defined (CONFIG_FRONTEND_CAB)

					nRet = DI_CH_C_GetSignalInfo(s_pstSnmpInfo->stSnmpMWInfo.ulWatchingTunerGroupId, &stSignalInfo);
					if (nRet != DI_CH_OK)
					{
						SNMP_DI_Print(0, (" DI_CH_T_GetSignalInfo Error(%d)\n", nRet));
					}
#endif
				}

				s_pstSnmpInfo->stSnmpDIInfo.ulInbandBer = (HUINT32)stSignalInfo.fBer;
				s_pstSnmpInfo->stSnmpDIInfo.etSnmpEvent=0;
				break;
			}
			default:
				break;
		}
		semop(s_nSnmpSemaID, &stSemaClose, 1);

		VK_TASK_Sleep(2000);

	}
}


/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/******************************************************************************
 * Create Shared Memory Segment, Semaphore, and task for date processing
 * * \param[out]
 * * \return
 * * \remarks
 * * \see
 ******************************************************************************/

DI_ERR_CODE	DI_SNMP_Init(void)
{
	int nResult = DI_ERR_OK;

	key_t		 stKeyShared;	   /* Shared Memory Key */
	key_t		 stKeySema;	   /* Semaphore Key */
	union semun        utSema;


	stKeyShared = ftok( SHMKEYPATH, SHMKEYID );		/* Generate Shared Memory Key */
	if( (s_nSnmpShmID = shmget( stKeyShared, SIZEOFSHMSEG, IPC_CREAT | IPC_EXCL | 0666 )) == -1 ) 	/* Open Share Memory Segment */
	{
		SNMP_DI_Print(0, ("Shared memory segment exist - opening as client\n"));
		if( (s_nSnmpShmID = shmget( stKeyShared, SIZEOFSHMSEG, 0 )) == -1 ) 		/* Segment probably already exists - try as a client */
		{
			SNMP_DI_Print(0, ("Trying as client failed!!\n"));
			return DI_ERR_ERROR;
		}
	}
	else
	{
		SNMP_DI_Print(0, ("Creating new shared memory segment\n"));
	}

	if( (s_pstSnmpInfo = (DI_SNMP_Information_t *)shmat(s_nSnmpShmID, 0, 0)) == NULL )	/* Connect to Shared Memory Segment */
	{
		SNMP_DI_Print(0, ("Connecting to Shared memory Segment failed!!\n"));
		return DI_ERR_ERROR;
	}

	stKeySema = ftok( SEMKEYPATH, SEMKEYID );	 /* Generate Semaphore Key */
	if( (s_nSnmpSemaID = semget( stKeySema, 1, IPC_CREAT | IPC_EXCL | 0666 )) == -1 )	/* Open Semaphore Segment */
	{
		SNMP_DI_Print(0, ("Semaphore segment exist - opening as client\n"));
		if( (s_nSnmpSemaID = semget( stKeySema, 0, 0 )) == -1 )				/* Segment probably already exists - try as a client */
		{
			SNMP_DI_Print(0, ("Trying as client failed!!\n"));
			return DI_ERR_ERROR;
		}
	}
	else
	{
		SNMP_DI_Print(0, ("Creating new Semaphore segment\n"));
	}
	utSema.val = 1;
	semctl( s_nSnmpSemaID, 0, SETVAL, utSema );		/* Semaphore Initialization */


 	////////////////////////////////////////////////////////////
	/* Task Create*/
	nResult = VK_TASK_Create( P_SNMP_SnmpTask, SNMP_TASK_PRIORITY, SNMP_TASK_STACK_SIZE, "SnmpTask", NULL, &s_ulSnmpTaskId, FALSE);
	if(nResult != VK_OK)
	{
		SNMP_DI_Print(0, ("Error(0x%x) in VK_TASK_Create()\n\r", nResult));
		return DI_ERR_ERROR;
	}

	/* Task Start*/
	nResult = VK_TASK_Start(s_ulSnmpTaskId);
	if(nResult != VK_OK)
	{
		SNMP_DI_Print(0, ("Error(0x%x) in VK_TASK_Start()\n\r", nResult));
		return DI_ERR_ERROR;
	}

	/* Initialization */
	VK_memset (s_pstSnmpInfo, 0x00, sizeof(DI_SNMP_Information_t));

	SNMP_DI_Print(0, ("=============[INIT] Private MIB Info================\n\n\n"));
	P_SNMP_PrintMibInfo(s_pstSnmpInfo);

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	P_SNMP_TestMibInfo();
#endif
	return nResult;
}



/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/******************************************************************************
 * MW set privateMIB Info
 * \param[out]	DI_SNMP_MWInformation_t
 * \return
 * \remarks
 * \see
 ******************************************************************************/
DI_ERR_CODE DI_SNMP_SetMWPrivateMIBInfo( DI_SNMP_MWInformation_t *stSnmpMWInfo )
{
	int nResult =DI_ERR_OK;

	semop(s_nSnmpSemaID, &stSemaOpen, 1);

	//////Check For MW Setting /////////
	//SNMP_DI_Print(0, ("==============Before Setting================\n\n\n");
	//P_SNMP_PrintMibInfo(s_pstSnmpInfo);

	VK_memcpy( &s_pstSnmpInfo->stSnmpMWInfo, stSnmpMWInfo, sizeof(DI_SNMP_MWInformation_t));
	VK_memcpy( &g_stSnmpInfo.stSnmpMWInfo, stSnmpMWInfo, sizeof(DI_SNMP_MWInformation_t));


	//////Check For MW Setting /////////
	SNMP_DI_Print(1, ("==============After Setting================\n\n\n"));
	P_SNMP_PrintMibInfo(s_pstSnmpInfo);

	semop(s_nSnmpSemaID, &stSemaClose, 1);

	return nResult;

}

