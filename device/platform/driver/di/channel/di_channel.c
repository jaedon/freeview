/**
 * @file	di_channel.c
 * @brief	Channel DI Layer C source
 * @version $Revision: 1.0.0 $
 * @date 	$Date: 2008/07/26 11:00:00 $
 * @author 	Linus Lee
 */

/** @defgroup DI_CHANNEL
 *  This is CHANNEL module of DI layer.
 *  @{
 */

/******************************************************************************
 * Include Files
 ******************************************************************************/

#if defined(CONFIG_VK_STDLIB)
#else
	#include <stdio.h>
#endif
#include <memory.h>

#include "htype.h"
#include "vkernel.h"

/* di headers */
#include "di_err.h"

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

/* drv headers */
#include "drv_channel.h"

#include "drv_tspath.h"
#include "drv_channel_config.h"

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#include "drv_pm.h"
#endif

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/


/******************************************************************************
 * Data Types
 ******************************************************************************/

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/
HINT32 show_di_ch = 0;
#if defined(CONFIG_DEMOD_TSMF)
HINT32 show_di_ch_tsmf = 0;
#endif

/******************************************************************************
 * Global Variables
 ******************************************************************************/

CH_InfoTuner_t g_stTunerConfig[NUM_OF_CHANNEL];

CH_InfoChannel_t g_stChannelConfig[NUM_OF_CHANNEL];

/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/
//unsigned long s_ulSemID;
HULONG 			ulI2cSemID;
HULONG			ulReentrantSemID;

#if defined(CONFIG_TASKBASE_FW_DOWNLOAD)
static HULONG		s_ulChInitTaskId;
#endif

/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/
HBOOL CompareTuneParam( int nChannelId );


#define __DI_CH_PRIVATE_FUNCTIONS__

static int CreateChannelObject( int nChannelID, DI_CH_Type_e type, int tunerID, int TypeOrder )
{
	int nResult = DI_CH_OK;
	void * pvTunePAddr 		= NULL;
	void * pvLockedPAddr 	= NULL;
	void *pvPreviousPAddr	= NULL;

	g_stChannelConfig[nChannelID].etChannelType 	= type;

	CH_DI_Print(4,("nChannelID[%d], tunerID [%d]\n", nChannelID, tunerID ));

#if defined (CONFIG_SUPPORT_CROSS_TUNER_ID)
	g_stChannelConfig[nChannelID].nTunerID 		= DRV_TS_PATH_GetDemodID(tunerID);
	CH_DI_Print(4,("nChannelID[%d], TS port ID[%d]\n", nChannelID, g_stChannelConfig[nChannelID].nTunerID ));
#elif defined (CONFIG_PLATFORM_DB7346)
	g_stChannelConfig[nChannelID].nTunerID 		= DRV_TS_PATH_GetDemodID(tunerID);
	CH_DI_Print(4,("nChannelID[%d], TS port ID[%d]\n", nChannelID, g_stChannelConfig[nChannelID].nTunerID ));
#elif defined (CONFIG_PLATFORM_DB7335)
	g_stChannelConfig[nChannelID].nTunerID 		= DRV_TS_PATH_GetDemodID(tunerID);
	CH_DI_Print(4,("nChannelID[%d], TS port ID[%d]\n", nChannelID, g_stChannelConfig[nChannelID].nTunerID ));
#else
	g_stChannelConfig[nChannelID].nTunerID		= tunerID;
#endif

	g_stChannelConfig[nChannelID].nUnitId			= TypeOrder;

	CH_DI_Print(3,("[CreateChannelObject] nChannelID[%d], etChannelType[0x%x], InstanceId[%d], UnitId[%d]\n", nChannelID, g_stChannelConfig[nChannelID].etChannelType , g_stChannelConfig[nChannelID].nTunerID, TypeOrder ));

	g_stChannelConfig[nChannelID].stChannelRsc.ulTaskID	= 0;
	g_stChannelConfig[nChannelID].stChannelRsc.ulQueueID	= 0;
	g_stChannelConfig[nChannelID].stChannelRsc.ulSemID	= 0;
	g_stChannelConfig[nChannelID].stChannelRsc.ulEventID	= 0;

	g_stChannelConfig[nChannelID].etChannelAction			= CH_ACTION_IDLE;

	g_stChannelConfig[nChannelID].stTsStatus.etTsOnOff 		= CH_TS_PATH_OFF;

	g_stChannelConfig[nChannelID].stChannelControl.etPreviousState 	= CH_STATUS_IDLE;
	g_stChannelConfig[nChannelID].stChannelControl.etCurrentState	= CH_STATUS_IDLE;

	g_stChannelConfig[nChannelID].SignalCallback			= NULL;

	g_stChannelConfig[nChannelID].stTuningInfo.ulAutoSearchMode = 0;

	g_stChannelConfig[nChannelID].etRfInputSource			= tunerID;

	if (g_stChannelConfig[nChannelID].etChannelType == DI_CH_TYPE_SAT)
	{
		pvPreviousPAddr	= DD_MEM_Alloc(sizeof(CH_SAT_TuneParam_t));
		if( pvPreviousPAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvPreviousParam	= pvPreviousPAddr;
		}

		pvTunePAddr = DD_MEM_Alloc(sizeof(CH_SAT_TuneParam_t));
		if( pvTunePAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam 	= pvTunePAddr;
		}

		pvLockedPAddr = DD_MEM_Alloc(sizeof(CH_SAT_TuneParam_t));
		if( pvLockedPAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvWorkingParam	= pvLockedPAddr;
		}
	}
	else if (g_stChannelConfig[nChannelID].etChannelType == DI_CH_TYPE_TER)
	{
		pvPreviousPAddr = DD_MEM_Alloc(sizeof(CH_TER_TuneParam_t));
		if( pvPreviousPAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvPreviousParam	= pvPreviousPAddr;
		}

		pvTunePAddr 	= DD_MEM_Alloc(sizeof(CH_TER_TuneParam_t));
		if( pvTunePAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam 	= pvTunePAddr;
		}

		pvLockedPAddr 	= DD_MEM_Alloc(sizeof(CH_TER_TuneParam_t));
		if( pvLockedPAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvWorkingParam 	= pvLockedPAddr;
		}
	}
	else if (g_stChannelConfig[nChannelID].etChannelType == DI_CH_TYPE_CAB)
	{
		pvPreviousPAddr	= DD_MEM_Alloc(sizeof(CH_CAB_TuneParam_t));
		if( pvPreviousPAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvPreviousParam	= pvPreviousPAddr;
		}

		pvTunePAddr 	= DD_MEM_Alloc(sizeof(CH_CAB_TuneParam_t));
		if( pvTunePAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam 	= pvTunePAddr;
		}

		pvLockedPAddr 	= DD_MEM_Alloc(sizeof(CH_CAB_TuneParam_t));
		if( pvLockedPAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvWorkingParam 	= pvLockedPAddr;
		}
	}
	else if (g_stChannelConfig[nChannelID].etChannelType == DI_CH_TYPE_ANA)
	{
		pvPreviousPAddr	= DD_MEM_Alloc(sizeof(CH_ANA_TuneParam_t));
		if( pvPreviousPAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvPreviousParam		= pvPreviousPAddr;
		}

		pvTunePAddr 	= DD_MEM_Alloc(sizeof(CH_ANA_TuneParam_t));
		if( pvTunePAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam 	= pvTunePAddr;
		}

		pvLockedPAddr 	= DD_MEM_Alloc(sizeof(CH_ANA_TuneParam_t));
		if( pvLockedPAddr == NULL )
		{
			CH_DI_Print(0, ("[CreateChannelObject] ERR! DD_MEM_Alloc\n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			g_stChannelConfig[nChannelID].stTuningInfo.pvWorkingParam 	= pvLockedPAddr;
		}
	}
	else
	{
		//unknown channel type
		CH_DI_Print(0, ("[CreateChannelObject] ERR! Unknown channel type\n"));
		nResult = DI_CH_ERR;
	}

	return nResult;
}

static int InitTunerObject(int nNumTuner)
{
	int i;
	int nNumSat = 0, nNumTer = 0, nNumCab = 0, nNumAna = 0;
	int nNumChann = 0;

	for(i=0; i<nNumTuner; i++)
	{
		g_stTunerConfig[i].unCapability = DRV_CH_GetTunerType(i);

		if((g_stTunerConfig[i].unCapability & DI_CH_TYPE_SAT) && (nNumChann < NUM_OF_CHANNEL))
		{
			CreateChannelObject(nNumChann++, DI_CH_TYPE_SAT, i, nNumSat++);
		}
		if((g_stTunerConfig[i].unCapability & DI_CH_TYPE_TER) && (nNumChann < NUM_OF_CHANNEL))
		{
			CreateChannelObject(nNumChann++, DI_CH_TYPE_TER, i, nNumTer++);
		}
		if((g_stTunerConfig[i].unCapability & DI_CH_TYPE_CAB) && (nNumChann < NUM_OF_CHANNEL))
		{
			CreateChannelObject(nNumChann++, DI_CH_TYPE_CAB, i, nNumCab++);
		}
		if((g_stTunerConfig[i].unCapability & DI_CH_TYPE_ANA) && (nNumChann < NUM_OF_CHANNEL))
		{
			CreateChannelObject(nNumChann++, DI_CH_TYPE_ANA, i, nNumAna++);
		}

	}

	CH_DI_Print(4,("Total Number of Channel(%d)\n", (nNumChann)));

	return 0;
}

#if defined(CONFIG_FRONTEND_SAT)
static void SAT_ManagerTask(void* pData)
{
	int 			nChannelID 	= 0;
	int				*pnChannelId;
	CH_Command_e 	etCommand 	= 0;
	int 			nReceivedID = 0;
	HUINT32			ulReceivedIdFromMsgQ = 0, unWorkingId = 0, ulCurrentRequestedId = 0;
	int 			nRet 		= VK_OK;

	HUINT32	aunReceivedMsg[4];

	pnChannelId = (int *)pData;
	nChannelID = *pnChannelId;

	CH_DI_Print(4, ("Created for nChannelID(%d)\n", nChannelID));

	while(1)
	{

		nRet = VK_MSG_ReceiveTimeout( g_stChannelConfig[nChannelID].stChannelRsc.ulQueueID, aunReceivedMsg, CONST_CH_Q_SIZE, CONST_CH_Q_TIMEOUT );

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(DRV_PM_GetStandbyStatus() == TRUE)
		{
			CH_DI_Print(5, ("[%s] Stanby mode: skip status checking..\n", __func__));
			continue;
		}
#endif
		if( nRet != VK_OK )
		{
			if ( nRet == VK_TIMEOUT )
			{
				ulCurrentRequestedId 	= GetRequestedId( nChannelID );
				CH_S_CheckStatus( nChannelID, ulCurrentRequestedId );
			}
			else
			{
				CH_DI_Print(0, ("[SAT_ManagerTask] nRet(0x%x)\n", nRet));
			}
			continue;
		}


		etCommand	= aunReceivedMsg[0];
		nReceivedID = aunReceivedMsg[1];
		ulReceivedIdFromMsgQ = aunReceivedMsg[2];

		if( nReceivedID != nChannelID )
		{
			CH_DI_Print(0, ("[SAT_ManagerTask] nReceivedID(0x%x)\n", nReceivedID));
			continue;
		}

		ulCurrentRequestedId 	= GetRequestedId( nChannelID );

		if( ulReceivedIdFromMsgQ != ulCurrentRequestedId )
		{
			CH_DI_Print(4, ("<<<<<<<<<<CH_CMD_IGNORED>>>>>>>>>> \n"));
			CH_DI_Print(5, ("ulReceivedIdFromMsgQ [%d]:[%d] ulCurrentRequestedId\n", ulReceivedIdFromMsgQ, ulCurrentRequestedId));
			continue;
		}
		else
		{
			if( etCommand == CH_CMD_LOCKING )
			{
				VK_memcpy( 	g_stChannelConfig[nChannelID].stTuningInfo.pvWorkingParam, \
							g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam, \
							sizeof(CH_SAT_TuneParam_t) );

				unWorkingId = ulReceivedIdFromMsgQ;
			}
		}

		CH_DI_Print(4, ("Msg for nChannelID[%d], etCommand[%d]\n", nChannelID, etCommand));

		switch(etCommand)
		{
			case CH_CMD_LOCKING:
				//PrintSatTuneParam(g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam);
				CH_DI_Print(2,(">>>>>>>>>>CH_CMD_LOCKING<<<<<<<<<< \n"));
				CH_DI_Print(3, ("ulReceivedIdFromMsgQ [%d]:[%d] ulCurrentRequestedId\n", ulReceivedIdFromMsgQ, ulCurrentRequestedId));
				SetTuneStatus( nChannelID, CH_STATUS_LOCKING );
				CH_S_SetTune( nChannelID, unWorkingId );

				break;

			case CH_CMD_AUTOSEARCH:
				CH_S_ProcessAutoSearch( nChannelID, unWorkingId );

				break;

			default:
				CH_DI_Print(0, ("[SAT_ManagerTask] etCommand(%d)\n", etCommand));
				break;
		}
	}

}
#endif

#if defined(CONFIG_FRONTEND_TER)
static void TER_ManagerTask(void* pData)
{
	int 			nChannelID 	= 0;
	int				*pnChannelId;
	CH_Command_e 	etCommand 	= 0;
	int 			nReceivedID = 0;
	HUINT32			ulReceivedIdFromMsgQ = 0, unWorkingId = 0, ulCurrentRequestedId = 0;
	int 			nRet 		= VK_OK;

	HUINT32			aunReceivedMsg[4];

	pnChannelId = (int *)pData;
	nChannelID = *pnChannelId;

	CH_DI_Print(4, (" Created for nChannelID(%d)\n", nChannelID));

	while(1)
	{
		nRet = VK_MSG_ReceiveTimeout( g_stChannelConfig[nChannelID].stChannelRsc.ulQueueID, aunReceivedMsg, CONST_CH_Q_SIZE, CONST_CH_Q_TIMEOUT );

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(DRV_PM_GetStandbyStatus() == TRUE)
		{
			CH_DI_Print(5, ("[%s] Stanby mode: skip status checking..\n", __func__));
			continue;
		}
#endif
		if( nRet != VK_OK )
		{
			if ( nRet == VK_TIMEOUT )
			{
				ulCurrentRequestedId 	= GetRequestedId( nChannelID );
				CH_T_CheckStatus( nChannelID, ulCurrentRequestedId );
			}
			else
			{
				CH_DI_Print(0, ("[TER_ManagerTask] nRet(0x%x)\n", nRet));
			}
			continue;
		}


		etCommand	= aunReceivedMsg[0];
		nReceivedID = aunReceivedMsg[1];
		ulReceivedIdFromMsgQ = aunReceivedMsg[2];

		if( nReceivedID != nChannelID )
		{
			CH_DI_Print(0, ("[TER_ManagerTask] nReceivedID(0x%x)\n", nReceivedID));
			continue;
		}

		ulCurrentRequestedId 	= GetRequestedId( nChannelID );

		if( ulReceivedIdFromMsgQ != ulCurrentRequestedId )
		{
			CH_DI_Print(4,("<<<<<<<<<<CH_CMD_IGNORED>>>>>>>>>> \n"));
			CH_DI_Print(5, ("ulReceivedIdFromMsgQ [%d]:[%d] ulCurrentRequestedId\n", ulReceivedIdFromMsgQ, ulCurrentRequestedId));
			continue;
		}
		else
		{
			if( etCommand == CH_CMD_LOCKING )
			{
			VK_memcpy( 	g_stChannelConfig[nChannelID].stTuningInfo.pvWorkingParam, \
						g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam, \
						sizeof(CH_TER_TuneParam_t) );

			unWorkingId = ulReceivedIdFromMsgQ;
		        }
		}

		switch(etCommand)
		{
			case CH_CMD_LOCKING:
				CH_DI_Print(3,(">>>>>>>>>>CH_CMD_LOCKING<<<<<<<<<< \n"));
				CH_DI_Print(3,("Msg for nChannelID[%d], unWorkingId[%d]\n", nChannelID, unWorkingId));
				/* PrintTerTuneParam(g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam); */

				SetTuneStatus( nChannelID, CH_STATUS_LOCKING );
				CH_T_SetTune( nChannelID, unWorkingId );

				break;
#if 0
			case TUNER_MSG_CHECK_STATUS:
				break;
#endif
			default:
				CH_DI_Print(0, ("[TER_ManagerTask] etCommand(%d)\n", etCommand));
				break;
		}
	}
}
#endif

#if defined(CONFIG_FRONTEND_CAB)
static void CAB_ManagerTask(void* pData)
{
	int 			nChannelID 	= 0;
	CH_Command_e 	etCommand 	= 0;
	int 			nReceivedID = 0;
	HUINT32			ulReceivedIdFromMsgQ = 0, unWorkingId = 0, ulCurrentRequestedId = 0;
	int 			nRet 		= VK_OK;

	HUINT32			aunReceivedMsg[4];

	nChannelID = *(int *)pData;

	CH_DI_Print(4, ("Created for nChannelID(%d)\n", nChannelID));

	while(1)
	{
		nRet = VK_MSG_ReceiveTimeout( g_stChannelConfig[nChannelID].stChannelRsc.ulQueueID, aunReceivedMsg, CONST_CH_Q_SIZE, CONST_CH_Q_TIMEOUT );

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(DRV_PM_GetStandbyStatus() == TRUE)
		{
			CH_DI_Print(5, ("[%s] Stanby mode: skip status checking..\n", __func__));
			continue;
		}
#endif
		if( nRet != VK_OK )
		{
			if ( nRet == VK_TIMEOUT )
			{
				ulCurrentRequestedId 	= GetRequestedId( nChannelID );
				CH_C_CheckStatus( nChannelID, ulCurrentRequestedId );
			}
			else
			{
				CH_DI_Print(0, ("[CAB_ManagerTask] nRet(0x%x) \n", nRet));
			}
			continue;
		}

		etCommand	= aunReceivedMsg[0];
		nReceivedID = aunReceivedMsg[1];
		ulReceivedIdFromMsgQ = aunReceivedMsg[2];

		if( nReceivedID != nChannelID )
		{
			CH_DI_Print(0, ("[CAB_ManagerTask] nReceivedID(0x%x) \n", nReceivedID));
			continue;
		}

		ulCurrentRequestedId 	= GetRequestedId( nChannelID );

		if( ulReceivedIdFromMsgQ != ulCurrentRequestedId )
		{
			CH_DI_Print(4, ("<<<<<<<<<<CH_CMD_IGNORED>>>>>>>>>> \n"));
			CH_DI_Print(5,("ulReceivedIdFromMsgQ [%d]:[%d] ulCurrentRequestedId \n", ulReceivedIdFromMsgQ, ulCurrentRequestedId));
			continue;
		}
		else
		{
			if( etCommand == CH_CMD_LOCKING )
			{
			VK_memcpy( 	g_stChannelConfig[nChannelID].stTuningInfo.pvWorkingParam, \
						g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam, \
							sizeof(CH_CAB_TuneParam_t) );

			unWorkingId = ulReceivedIdFromMsgQ;
		        }
		}

		switch(etCommand)
		{
			case CH_CMD_LOCKING:
				CH_DI_Print(3,(">>>>>>>>>>CH_CMD_LOCKING<<<<<<<<<< \n"));
				CH_DI_Print(3,("Msg for nChannelID[%d]\n", nChannelID));
				PrintCabTuneParam(g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam);

				SetTuneStatus( nChannelID, CH_STATUS_LOCKING );
				CH_C_SetTune( nChannelID, unWorkingId );
				break;

			case CH_CMD_AUTOSEARCH:
				CH_C_ProcessAutoSearch( nChannelID, unWorkingId );
				break;

			default:
				CH_DI_Print(0, ("[CAB_ManagerTask] etCommand(%d) \n", etCommand));
				break;
		}
	}
}
#endif

#if defined(CONFIG_FRONTEND_ANA)
static void ANA_ManagerTask(void* pData)
{
	int 			nChannelID 	= 0;
	CH_Command_e 	etCommand 	= 0;
	int 			nReceivedID = 0;
	HUINT32			unRequestID = 0, unWorkingId = 0;
	int 			nRet 		= VK_OK;
	HUINT32			aunReceivedMsg[4];
	HUINT32			ulTimeout = CH_A_TIMEOUT_IDLE;
	nChannelID = *(int *)pData;
	CH_ANA_TuneParam_t * pstAnaTuneParam;

	CH_DI_Print(4,("Created for nChannelID(%d)\n", nChannelID));

	while(1)
	{
		nRet = VK_MSG_ReceiveTimeout( g_stChannelConfig[nChannelID].stChannelRsc.ulQueueID, aunReceivedMsg, CONST_CH_Q_SIZE, ulTimeout);

		if( nRet != VK_OK )
		{
			if ( nRet == VK_TIMEOUT )
			{
				ulTimeout = CH_A_CheckStatus( nChannelID, unWorkingId);
				//CH_DI_Print ("ulTimeout(%d) !!!\n", ulTimeout);
			}
			else
			{
				//CH_DI_Print(0, ("nRet(0x%x)", nRet));
			}
			continue;
		}

		etCommand	= aunReceivedMsg[0];
		nReceivedID = aunReceivedMsg[1];
		unRequestID = aunReceivedMsg[2];

		if( nReceivedID != nChannelID )
		{
			CH_DI_Print(0, ("[ANA_ManagerTask] nReceivedID(0x%x)\n", nReceivedID));
			continue;
		}

		if( unRequestID != g_stChannelConfig[nChannelID].stTuningInfo.unRequestID )
		{
			CH_DI_Print(0, ("[ANA_ManagerTask] unRequestID(0x%x)\n", unRequestID));
			continue;
		}
		else
		{
			VK_memcpy( 	g_stChannelConfig[nChannelID].stTuningInfo.pvWorkingParam, \
			g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam, \
			sizeof(CH_ANA_TuneParam_t) );

			unWorkingId = unRequestID;
		}

		switch(etCommand)
		{
			case CH_CMD_IDLE:
				ulTimeout = CH_A_TIMEOUT_IDLE;
				break;

			case CH_CMD_LOCKING:
				CH_DI_Print(3,("Do request lock!, Msg for nChannelID[%d]\n", nChannelID));
				PrintAnaTuneParam(g_stChannelConfig[nChannelID].stTuningInfo.pvRequestedParam);
				(void)CH_A_SetTune (nChannelID, unWorkingId);
				pstAnaTuneParam = (CH_ANA_TuneParam_t *)GetWorkingTuneInfo(nChannelID);

				if (DRV_SCART_CheckPreviousRF() == TRUE)
				{
					ulTimeout = CH_A_TIMEOUT_SYNC_LOCK_INPUT;
					CH_DI_Print(3, ("Request Lock! sync check delay : input change!!\n"));
				}
				else
				{
					/*
					 * SECAM L search시 sync detection이 pal보다 100ms정도 느린 문제가 있어 workaround함.
					 */
					if (pstAnaTuneParam->etAnalogSystem >= DI_CH_SYSTEM_L)
					{
						ulTimeout = CH_A_TIMEOUT_SYNC_LOCK_SECAM;
					}
					else
					{
						ulTimeout = CH_A_TIMEOUT_SYNC_LOCK;
					}
					CH_DI_Print(3,("Request Lock! sync check delay : channel change!!\n"));
				}
				break;

			case CH_CMD_AUTOSEARCH:

				break;

			default:
				CH_DI_Print(0, ("[ANA_ManagerTask] etCommand(%d)\n", etCommand));
				break;
		}
	}
	CH_UNUSED(pData);
}
#endif


static int InitChnnelResource ( int nChannelID )
{
	int *pnChannelId;
	int nRet 	= VK_OK;
	int nResult = DI_CH_OK;

	char szSemName[NAME_LENGTH] 		= {'\0',};
	char szTaskName[NAME_LENGTH] 		= {'\0',};
	char szQueueName[NAME_LENGTH]		= {'\0',};
	char szEventName[NAME_LENGTH] 		= {'\0',};

	void (*pTaskFunc)(void* ) = NULL;

	CH_DI_Print(4, ("nChannelID = %d , etChannelType = %d \n)", nChannelID, g_stChannelConfig[nChannelID].etChannelType));

	if (g_stChannelConfig[nChannelID].etChannelType == DI_CH_TYPE_SAT)
	{
		VK_snprintf( szSemName, sizeof(szSemName), 	"SatSema%02X",		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szEventName, 	sizeof(szEventName), 	"SatEvent%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szQueueName, sizeof(szQueueName),	"SatQueue%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szTaskName, 	sizeof(szTaskName), 	"SatTask%02X", 		g_stChannelConfig[nChannelID].nUnitId );

#if defined(CONFIG_FRONTEND_SAT)
		pTaskFunc = SAT_ManagerTask;
#endif

	}
	else if (g_stChannelConfig[nChannelID].etChannelType == DI_CH_TYPE_TER)
	{
		VK_snprintf( szSemName, sizeof(szSemName),		"TerSema%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szEventName, sizeof(szEventName),	"TerEvent%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szQueueName, sizeof(szQueueName),	"TerQueue%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szTaskName, sizeof(szTaskName), 	"TerTask%02X", 		g_stChannelConfig[nChannelID].nUnitId );

#if defined(CONFIG_FRONTEND_TER)
		CH_DI_Print(4,("TER_ManagerTask \n"));
		pTaskFunc = TER_ManagerTask;
#endif

	}

	else if (g_stChannelConfig[nChannelID].etChannelType == DI_CH_TYPE_CAB)
	{
		VK_snprintf( szSemName,  sizeof(szSemName),	"CabSema%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szEventName, 	sizeof(szEventName),	"CabEvent%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szQueueName, sizeof(szQueueName),	"CabQueue%02X", 	g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szTaskName, sizeof(szTaskName), 	"CabTask%02X", 		g_stChannelConfig[nChannelID].nUnitId );

#if defined(CONFIG_FRONTEND_CAB)
		CH_DI_Print(4,("CAB_ManagerTask \n"));
		pTaskFunc = CAB_ManagerTask;
#endif

	}
	else if(g_stChannelConfig[nChannelID].etChannelType == DI_CH_TYPE_ANA)
	{
		VK_snprintf( szSemName,  sizeof(szSemName), 	"AnaSema%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szEventName, sizeof(szEventName), 	"AnaEvent%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szQueueName, sizeof(szQueueName),	"AnaQueue%02X", 		g_stChannelConfig[nChannelID].nUnitId );
		VK_snprintf( szTaskName, sizeof(szTaskName), 	"AnaTask%02X", 		g_stChannelConfig[nChannelID].nUnitId );

#if defined(CONFIG_FRONTEND_ANA)
		pTaskFunc = ANA_ManagerTask;
#endif

	}
	else
	{
		//unknown channel type
		CH_DI_Print(0, ("[InitChnnelResource] ERR! Unknown channel type\n"));
		nResult = DI_CH_ERR;
	}

	/* Create Semaphore */
	nRet = VK_SEM_Create( &g_stChannelConfig[nChannelID].stChannelRsc.ulSemID, szSemName, VK_SUSPENDTYPE_PRIORITY);

	if( nRet != VK_OK )
	{
		CH_DI_Print(0, ("[InitChnnelResource] ERR! Create Semaphore! nRet(0x%x)\n", nRet));
		return DI_CH_ERR;
	}

	/* Create Event */
	nRet = VK_EVENT_Create( &g_stChannelConfig[nChannelID].stChannelRsc.ulEventID, szEventName);
	if( nRet != VK_OK )
	{
		CH_DI_Print(0, ("[InitChnnelResource] ERR! Create Event ! nRet(0x%x)\n", nRet));
		nResult = DI_CH_ERR;
	}

	/* Create message queue */
	nRet = VK_MSG_Create( 	CH_MSGQ_SIZE,	\
							CONST_CH_Q_SIZE,	\
							szQueueName,		\
							&g_stChannelConfig[nChannelID].stChannelRsc.ulQueueID, \
							VK_SUSPENDTYPE_FIFO );

	if( nRet != VK_OK )
	{
		CH_DI_Print(0, ("[InitChnnelResource] ERR! Create message queue ! nRet(0x%x)\n", nRet));
		nResult = DI_CH_ERR;
	}

	/* Create Task */
	pnChannelId = (int *)DD_MEM_Alloc (sizeof(int));
        if(pnChannelId == NULL)
	{
		CH_DI_Print(0, ("[InitChnnelResource] ERR! can't allocate ChannelId!\n"));
		return DI_CH_ERR;
	}
	*pnChannelId = nChannelID;

	nRet = VK_TASK_Create( 	pTaskFunc,				\
							CH_TASK_PRIORITY,			\
							CH_TASK_STACK_SIZE,	\
							szTaskName, 					\
							pnChannelId, 					\
							&g_stChannelConfig[nChannelID].stChannelRsc.ulTaskID, \
							0 );

	if ((g_stChannelConfig[nChannelID].stChannelRsc.ulTaskID == 0) || (nRet != VK_OK))
	{
		CH_DI_Print(0, ("[InitChnnelResource] ERR! Create Task! nRet(0x%x)\n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		VK_TASK_Start(g_stChannelConfig[nChannelID].stChannelRsc.ulTaskID);
	}

	return nResult;

}

static int InitChannelDevice( void )
{
	int i;
	int nRet 	= DI_CH_OK;
	int nResult = DI_CH_OK;


	for (i=0; i< NUM_OF_CHANNEL; i++)
	{
		CH_DI_Print(3,("[InitChannelDevice]  (%d)Ch -> (%d) Type  \n", i, g_stChannelConfig[i].etChannelType));

		if ( g_stChannelConfig[i].etChannelType == DI_CH_TYPE_TER )
		{
#if defined(CONFIG_FRONTEND_TER)
			nRet = CH_T_InitDev( i );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("[InitChannelDevice] ERR!CH_T_InitDev() nRet(0x%x) \n", nRet));
				nResult = DI_CH_ERR;
			}
#else
			nResult = DI_CH_NOT_SUPPORTED;
			CH_DI_Print(0, ("[InitChannelDevice] ERR! Check DRV_CH_CAPABILITY in di_config.c !!! \n"));
#endif
		}
		 else if ( g_stChannelConfig[i].etChannelType == DI_CH_TYPE_SAT )
		{
#if defined(CONFIG_FRONTEND_SAT)
			nRet = CH_S_InitDev( i );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("[InitChannelDevice] ERR!CH_S_InitDev()  nRet(0x%x) \n", nRet));
				nResult = DI_CH_ERR;
			}
#else
			nResult = DI_CH_NOT_SUPPORTED;
			CH_DI_Print(0, ("[InitChannelDevice] ERR! Check DRV_CH_CAPABILITY in di_config.c !!! \n"));
#endif
		}
		else if ( g_stChannelConfig[i].etChannelType == DI_CH_TYPE_CAB )
		{

#if defined(CONFIG_FRONTEND_CAB)
			nRet = CH_C_InitDev( i );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("[InitChannelDevice] ERR! CH_C_InitDev() nRet(0x%x) \n", nRet));
				nResult = DI_CH_ERR;
			}
#else
			nResult = DI_CH_NOT_SUPPORTED;
			CH_DI_Print(0, ("[InitChannelDevice] ERR! Check DRV_CH_CAPABILITY in di_config.c !!! \n"));
#endif
		}

	}

	return nResult;

}

#if defined(CONFIG_TASKBASE_FW_DOWNLOAD)
void InitChannelTask(void* pData)
{
	HINT32 err = DI_CH_OK;
	HULONG *pstChInitTaskId;
	HULONG ulCurrentTaskId;

	pstChInitTaskId = (HULONG*)pData;
	ulCurrentTaskId = *pstChInitTaskId;

	CH_DI_Print(4, ("[InitChannelTask] >>>>>>>>>>>Start...\n"));

	CH_DI_Print(0, ("+++[InitChannelTask] start!\n"));
	err = InitChannelDevice();
	if(err != 0)
	{
		CH_DI_Print(4, ("=== InitChannelTask &&&(%d) ===\n", err));
		return;
	}
	CH_DI_Print(0, ("---[InitChannelTask] done!\n"));

	CH_DI_Print(0, ("---[InitChannelTask] VK_TASK_Destroy start!\n"));
	err = VK_TASK_Destroy(ulCurrentTaskId);
	if(err != 0)
	{
		//CH_DI_Print(4, ("[InitChannelTask] === InitChannelTask &&&(%d) ===\n", err));
		return;
	}
	//CH_DI_Print(0, ("---[InitChannelTask] VK_TASK_Destroy done!\n"));

	while(1)	/* wait until self task detroy */
	{
		VK_TASK_Sleep(0xFFFFFFFF);
	}
}
#endif


#define __DI_CH_PUBLIC_FUNCTIONS__



DI_CHANNEL_ERR MAIN_CH_ReInitChannel (void)
{
	int i;
	int nRet 	= DI_CH_OK;
	int nResult = DI_CH_OK;


	for (i=0; i< NUM_OF_CHANNEL; i++)
	{
		CH_DI_Print(3,("[InitChannelDevice]  (%d)Ch -> (%d) Type  \n", i, g_stChannelConfig[i].etChannelType));

		if ( g_stChannelConfig[i].etChannelType == DI_CH_TYPE_TER )
		{
#if defined(CONFIG_FRONTEND_TER)
			nRet = CH_T_ReInitDevice( i );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("[InitChannelDevice] ERR!CH_T_ReInitDev() nRet(0x%x)\n", nRet));
				nResult = DI_CH_ERR;
			}
#else
			nRet 	= DI_CH_NOT_SUPPORTED;
			CH_DI_Print(0, ("[InitChannelDevice] ERR! Check DRV_CH_CAPABILITY in di_config.c !!!\n"));
#endif
		}
		 else if ( g_stChannelConfig[i].etChannelType == DI_CH_TYPE_SAT )
		{
#if defined(CONFIG_FRONTEND_SAT)
			nRet = CH_S_ReInitDevice( i );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("[InitChannelDevice] ERR!CH_S_ReInitDev()  nRet(0x%x)\n", nRet));
				nResult = DI_CH_ERR;
			}
#else
			nRet 	= DI_CH_NOT_SUPPORTED;
			CH_DI_Print(0, ("[InitChannelDevice] ERR! Check DRV_CH_CAPABILITY in di_config.c !!!\n"));
#endif
		}
		else if ( g_stChannelConfig[i].etChannelType == DI_CH_TYPE_CAB )
		{

#if defined(CONFIG_FRONTEND_CAB)
			nRet = CH_C_ReInitDevice( i );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("[InitChannelDevice] ERR! CH_C_ReInitDev() nRet(0x%x)\n", nRet));
				nResult = DI_CH_ERR;
			}
#else
			nRet 	= DI_CH_NOT_SUPPORTED;
			CH_DI_Print(0, ("[InitChannelDevice] ERR! Check DRV_CH_CAPABILITY in di_config.c !!!\n"));
#endif
		}

	}

	return nRet;
}



DI_CHANNEL_ERR MAIN_CH_InitChannel (void)
{
	int i;
	int nRet 	= DI_CH_OK;
	int nResult = DI_CH_OK;

	HUINT32 ulTunerNum = DRV_CH_GetMaxInstance();
	CH_DI_Print(5,("MaxInstance = %d \n", ulTunerNum));

	InitTunerObject( ulTunerNum );

	/* Create Semaphore */
	/* nRet = VK_SEM_Create( &ulI2cSemID, "CH_I2C_SEM", VK_SUSPENDTYPE_PRIORITY); */
	nRet = VK_SEM_Create( &ulI2cSemID, "CH_I2C_SEM", VK_SUSPENDTYPE_MAX);
	if( nRet != VK_OK )
	{
		CH_DI_Print(0, ("[MAIN_CH_InitChannel] ERR! Create Semaphore! nRet(0x%x)\n", nRet));
		return DI_CH_ERR;
	}

	nRet = VK_SEM_Create( &ulReentrantSemID, "CH_REENTRANT_SEM", VK_SUSPENDTYPE_MAX);
	if( nRet != VK_OK )
	{
		CH_DI_Print(0, ("[MAIN_CH_InitChannel] ERR! Create Semaphore! nRet(0x%x)\n", nRet));
		return DI_CH_ERR;
	}

	CH_DI_Print(3, ("NUM_OF_CHANNEL = %d \n", NUM_OF_CHANNEL));

	for (i=0; i< NUM_OF_CHANNEL; i++)
	{
		CH_DI_Print(3,("ChannelID[%d], Type[%d], TunerID[%d], PhysicalID[%d]\n", \
				i, g_stChannelConfig[i].etChannelType, g_stChannelConfig[i].nTunerID, g_stChannelConfig[i].nUnitId));

		nRet = InitChnnelResource(i);
		if( nRet != DI_CH_OK )
		{
			CH_DI_Print(0, ("[MAIN_CH_InitChannel] ERR! InitChnnelResource() nRet(0x%x)\n", nRet));
			nResult = DI_CH_ERR;
		}
	}

#if !defined(CONFIG_TASKBASE_FW_DOWNLOAD)
	nRet = InitChannelDevice();
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ("[InitChnnelResource] ERR! InitChannelDevice() nRet(0x%x)\n", nRet));
		nResult = DI_CH_ERR;
	}
#else
	nRet = VK_TASK_Create(	InitChannelTask,	\
							(unsigned long)CH_TASK_PRIORITY,	\
							(unsigned long)CH_TASK_STACK_SIZE, \
							(char *)"Ch_Init", 			\
							&s_ulChInitTaskId,				\
							&s_ulChInitTaskId, 	\
							TRUE );

	if ((s_ulChInitTaskId == 0) || (nRet != VK_OK))
	{
		CH_DI_Print(0, ("[InitChnnelResource] ERR! InitChannelTask Create !! nRet(0x%x)\n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		VK_TASK_Start(s_ulChInitTaskId);
	}
#endif

#if defined(CONFIG_DEMOD_TSMF)
	nRet = DRV_CH_TSMF_Init();
	if(nRet != DI_CH_OK)
	{
		nResult = DI_CH_ERR;
	}
#endif /* 0 */

	return nResult;

}


DI_CHANNEL_ERR MAIN_CH_UnInitChannel (void)
{
    int i;
    int nRet 	= DI_CH_OK;
    int nResult = DI_CH_OK;

    CH_DI_Print(4, ("[MAIN_CH_UnInitChannel] \n"));

    for (i=0; i< NUM_OF_CHANNEL; i++)
    {
        CH_DI_Print(3,("[MAIN_CH_UnInitChannel]  (%d)Ch -> (%d) Type  \n", i, g_stChannelConfig[i].etChannelType));

        if ( g_stChannelConfig[i].etChannelType == DI_CH_TYPE_TER )
        {
#if defined(CONFIG_FRONTEND_TER)
            nRet = CH_T_UnInitDevice( i );
            if ( nRet != DI_CH_OK )
            {
                CH_DI_Print(0, ("[MAIN_CH_UnInitChannel] ERR!CH_T_UnInitDevice() nRet(0x%x)\n", nRet));
                nResult = DI_CH_ERR;
            }
#else
            nRet 	= DI_CH_NOT_SUPPORTED;
            CH_DI_Print(0, ("[MAIN_CH_UnInitChannel] ERR! Check DRV_CH_CAPABILITY in di_config.c !!!\n"));
#endif
        }
        else if ( g_stChannelConfig[i].etChannelType == DI_CH_TYPE_SAT )
        {
#if defined(CONFIG_FRONTEND_SAT)
            nRet = CH_S_UnInitDevice( i );
            if ( nRet != DI_CH_OK )
            {
                CH_DI_Print(0, ("[MAIN_CH_UnInitChannel] ERR!CH_S_UnInitDevice() nRet(0x%x)\n", nRet));
                nResult = DI_CH_ERR;
            }
#else
            nRet    = DI_CH_NOT_SUPPORTED;
            CH_DI_Print(0, ("[MAIN_CH_UnInitChannel] ERR! Check DRV_CH_CAPABILITY in di_config.c !!!\n"));
#endif
        }
        else if ( g_stChannelConfig[i].etChannelType == DI_CH_TYPE_CAB )
        {
#if defined(CONFIG_FRONTEND_CAB)
            nRet = CH_C_UnInitDevice( i );
            if ( nRet != DI_CH_OK )
            {
                CH_DI_Print(0, ("[MAIN_CH_UnInitChannel] ERR!CH_S_UnInitDevice() nRet(0x%x)\n", nRet));
                nResult = DI_CH_ERR;
    		}
#else
            nRet    = DI_CH_NOT_SUPPORTED;
            CH_DI_Print(0, ("[MAIN_CH_UnInitChannel] ERR! Check DRV_CH_CAPABILITY in di_config.c !!!\n"));
#endif
        }
    }

    return nRet;
}


DI_CHANNEL_ERR DI_CH_GetNumOfInstance( HUINT32 * pulNumInstance )
{
	int nResult = DI_CH_OK;

	if ( pulNumInstance == NULL )
	{
		CH_DI_Print(0, ("[DI_CH_GetNumOfInstance] ERR! NumInstance NULL!\n"));
		nResult = DI_CH_PARAM_ERR;
	}
	else
	{
		*pulNumInstance = DRV_CH_GetMaxInstance();
	}

	return nResult;
}


DI_CHANNEL_ERR DI_CH_GetCapability( HUINT32 ulInstanceId, HUINT32 *pulChannelType )
{
	int nResult = DI_CH_OK;

	if ( ulInstanceId > DRV_CH_GetMaxInstance() )
	{
		CH_DI_Print(0, ("[DI_CH_GetCapability] ERR! InstanceId (%d) \n ", ulInstanceId));
		return DI_CH_PARAM_ERR;
	}

	if ( pulChannelType == NULL )
	{
		CH_DI_Print(0, ("[DI_CH_GetCapability] ERR! ChannelType (%d) \n ", pulChannelType));
		return DI_CH_PARAM_ERR;
	}
	else
	{
            /* Modify Prevent CR Defects(Out-of-bounds read) */
            if(ulInstanceId < NUM_OF_CHANNEL)
            {
                *pulChannelType = g_stTunerConfig[ulInstanceId].unCapability;
            }
	}

	return nResult;
}



/** @} */ // end of DI_CHANNEL

