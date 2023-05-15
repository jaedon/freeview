/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:		template.c
// Original Author: Full Name!!
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* chip headers */
#include "htype.h"
#include "vkernel.h"

/* di headers */
#include "di_channel.h"

#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"
#include "di_channel_mon.h"

#include "drv_tspath.h"

#include "drv_channel.h"
#include "drv_channel_config.h"

#if defined(CONFIG_FRONTEND_SAT)
#include "drv_channel_s.h"
#include "drv_channel_lnb.h"
#endif

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define

// End #define


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef


// End typedef


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee
extern CH_InfoChannel_t g_stChannelConfig[NUM_OF_CHANNEL];
extern HULONG 			ulI2cSemID;
extern HULONG			ulReentrantSemID;
extern HINT32 			show_di_ch;

#ifdef CH_PRINT_DI_LEVEL_ZAPPING_TIME
extern HUINT32 g_ulRequestLock[CONFIG_NUM_OF_TER_CHANNEL];
extern HUINT32 g_ulDiLevelLockedTime[CONFIG_NUM_OF_TER_CHANNEL] , g_ulDiLevelUnlockedTime[CONFIG_NUM_OF_TER_CHANNEL] ;
#endif
// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee
// End static variable


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:



#define __DRV_CH_PUBLIC_FUNCTIONS__

/*****************************************************************************
* function: DRV_TS_PATH_GetDemodID
* description : Get Physical demodulator id from virtual ts id
* input:
*   ulDevice_id : virtual ts id
* return:
*    -1 : invalid device id
*    others : physical demodulator id
******************************************************************************/
void DRV_CH_CallNotifyCallback ( int nChannelID, HUINT32 unRequestId, DI_CH_SignalInfo_e etSigalInfo )
{
	CH_Status_e etCurrentTuneStatus;
	HUINT32		unRequestedByMw;
	HUINT32		ulInstanceId = 0;

	ulInstanceId 		= GetInstanceId( nChannelID );
	etCurrentTuneStatus = GetTuneStatus( nChannelID );
	unRequestedByMw 	= GetRequestedId( nChannelID );

	if ( g_stChannelConfig[nChannelID].SignalCallback != NULL )
	{
		CH_DI_Print(3, ( "nChannelID[%d],unRequestId[%d],etSigalInfo[%d]\n", nChannelID, unRequestId, etSigalInfo ));
		CH_DI_Print(3, ( "[DRV_CH_CallNotifyCallback] etCurrentTuneStatus[%d],unRequestedByMw[%d]\n", etCurrentTuneStatus, unRequestedByMw ));

		switch( etSigalInfo )
		{
			case DI_CH_SIGNAL_LOCKED:
				if ( (etCurrentTuneStatus == CH_STATUS_LOCKED) )
				{
					if( (unRequestId == unRequestedByMw) && (IsStateChanged( nChannelID )) )
					{
						g_stChannelConfig[nChannelID].SignalCallback( 	ulInstanceId, \
																		unRequestedByMw, \
																		g_stChannelConfig[nChannelID].etChannelType, \
																		etSigalInfo );

						CH_DI_Print(1, (">>>>>>>>>>CH_STATUS_LOCKED : [%d]<<<<<<<<<< \n", unRequestedByMw));


#ifdef CH_PRINT_DI_LEVEL_ZAPPING_TIME
                        g_ulDiLevelLockedTime[nChannelID] = VK_TIMER_GetSystemTick();
                        CH_DI_Print(0, ("###########################################\n" ));
                        CH_DI_Print(0, ("## [DRV_CH_CallNotifyCallback] [%d] Lock Time( %d )ms ## \n" ,nChannelID, g_ulDiLevelLockedTime[nChannelID] - g_ulRequestLock[nChannelID] ));
                        CH_DI_Print(0, ("###########################################\n" ));
#endif

						UpdatePreviousLockedParam( nChannelID );
					}
					else
					{
						/* requested ID mismatch, do nothing */
					}
				}
				else
				{
					/* Current status of channel is not related with this event */
				}

				break;

			case DI_CH_SIGNAL_UNLOCK:

				if ( (etCurrentTuneStatus == CH_STATUS_UNLOCK) )
				{
					if( (unRequestId == unRequestedByMw) && (IsStateChanged( nChannelID )) )
					{
						g_stChannelConfig[nChannelID].SignalCallback( 	ulInstanceId, \
																		unRequestedByMw, \
																		g_stChannelConfig[nChannelID].etChannelType, \
																		etSigalInfo );

						CH_DI_Print(1, ("<<<<<<<<<<CH_STATUS_UNLOCK : [%d]>>>>>>>>>> \n", unRequestedByMw));
#ifdef CH_PRINT_DI_LEVEL_ZAPPING_TIME
                         g_ulDiLevelUnlockedTime[nChannelID] = VK_TIMER_GetSystemTick();
                         CH_DI_Print(0, ("[DRV_CH_CallNotifyCallback] [%d] UnLock Time( %d )ms  \n", nChannelID, g_ulDiLevelUnlockedTime[nChannelID] - g_ulRequestLock[nChannelID] ));
#endif

					}
					else
					{
						/* requested ID mismatch, do nothing */
					}
				}
				else
				{
					/* Current status of channel is not related with this event */
				}

				break;

			case DI_CH_SIGNAL_ANTENNA_FAULT:

				if ( (etCurrentTuneStatus == CH_STATUS_ANT_FAULT) )
				{
					//if( (unRequestId == unRequestedByMw) && (IsStateChanged( nChannelID )) )
					{
						g_stChannelConfig[nChannelID].SignalCallback( 	ulInstanceId, \
																		unRequestedByMw, \
																		g_stChannelConfig[nChannelID].etChannelType, \
																		etSigalInfo );

						CH_DI_Print(1, ("<<<<<<<<<<CH_STATUS_ANT_FAULT>>>>>>>>>> \n"));

					}
					//else
					//{
						/* requested ID mismatch, do nothing */
					//}
				}
				else
				{
					/* Current status of channel is not related with this event */
				}
				break;

			case DI_CH_SIGNAL_ANTENNA_OK:

				if ( (etCurrentTuneStatus == CH_STATUS_ANT_FAULT) )
				{
					//if( (unRequestId == unRequestedByMw) && (IsStateChanged( nChannelID )) )
					{
						g_stChannelConfig[nChannelID].SignalCallback(	ulInstanceId, \
																		unRequestedByMw, \
																		g_stChannelConfig[nChannelID].etChannelType, \
																		etSigalInfo );

						CH_DI_Print(1, ("<<<<<<<<<<CH_STATUS_ANT_RECOVERED>>>>>>>>>> \n"));//virtual state

					}
					//else
					//{
						/* requested ID mismatch, do nothing */
					//}
				}
				else
				{
					/* Current status of channel is not related with this event */
				}
				break;

			case DI_CH_SIGNAL_TSMF_GOOD:
					g_stChannelConfig[nChannelID].SignalCallback( 	ulInstanceId, \
																unRequestedByMw, \
																g_stChannelConfig[nChannelID].etChannelType, \
																etSigalInfo );

					CH_DI_Print(1, ("<<<<<<<<<<SIGNAL_TSMF_GOOD>>>>>>>>>> \n"));
				break;

			case DI_CH_SIGNAL_TSMF_BAD:
					g_stChannelConfig[nChannelID].SignalCallback( 	ulInstanceId, \
																unRequestedByMw, \
																g_stChannelConfig[nChannelID].etChannelType, \
																etSigalInfo );

					CH_DI_Print(1, ("<<<<<<<<<<SIGNAL_TSMF_BAD>>>>>>>>>> \n"));
				break;

			case DI_CH_SIGNAL_TSMF_EWS_ON:
					g_stChannelConfig[nChannelID].SignalCallback( 	ulInstanceId, \
																unRequestedByMw, \
																g_stChannelConfig[nChannelID].etChannelType, \
																etSigalInfo );

					CH_DI_Print(1, ("<<<<<<<<<<SIGNAL_TSMF_EWS_ON>>>>>>>>>> \n"));
				break;

			case DI_CH_SIGNAL_TSMF_EWS_OFF:
					g_stChannelConfig[nChannelID].SignalCallback( 	ulInstanceId, \
																unRequestedByMw, \
																g_stChannelConfig[nChannelID].etChannelType, \
																etSigalInfo );

					CH_DI_Print(1, ("<<<<<<<<<<SIGNAL_TSMF_EWS_OFF>>>>>>>>>> \n"));
				break;

			default:
				CH_DI_Print(0, ("etSigalInfo(%d)", etSigalInfo));
				break;
		}


	}
	else
	{
		/* Callback function is not registered. Do nothing */
		if (etSigalInfo == DI_CH_SIGNAL_LOCKED)
		{
			CH_DI_Print(1, ("locked !!!!!\n"));
		}
	}

	return;
}

#if !defined(CH_TAGGED_API)
void DRV_CH_Lock( int nChannelId )
{
	int nRet;
	nRet = VK_SEM_Get( g_stChannelConfig[nChannelId].stChannelRsc.ulSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[DRV_CH_Lock] ERR! VK_SEM_Get() \n"));
	}
}

void DRV_CH_UnLock( int nChannelId )
{
	int nRet;

	nRet = VK_SEM_Release( g_stChannelConfig[nChannelId].stChannelRsc.ulSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[DRV_CH_UnLock] ERR! VK_SEM_Release() \n"));
	}
}

#else

void DRV_CH_LockTagged(int nChannelId, const char *pcFilename, int nLineNum )
{
	int nRet;

	CH_DI_Print(5, ("[DRV_CH_LockTagged]  nChannelId[%d](%s:%d)\n", nChannelId, pcFilename, nLineNum ));
	nRet = VK_SEM_Get( g_stChannelConfig[nChannelId].stChannelRsc.ulSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[DRV_CH_LockTagged] ERR! VK_SEM_Get() :: g_stChannelConfig \n "));
	}
}

void DRV_CH_UnLockTagged( int nChannelId, const char *pcFilename, int nLineNum )
{
	int nRet;

	CH_DI_Print(5, ("nChannelId[%d](%s:%d)\n", nChannelId, pcFilename, nLineNum ));
	nRet = VK_SEM_Release( g_stChannelConfig[nChannelId].stChannelRsc.ulSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[DRV_CH_UnLockTagged] ERR! VK_SEM_Release() :: g_stChannelConfig \n "));
	}
}

#endif



void DRV_CH_LockI2c( int nChannelId )
{
	int nRet;
	CH_UNUSED(nChannelId);

	nRet = VK_SEM_Get( ulI2cSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[DRV_CH_LockI2c] ERR! VK_SEM_Get() :: ulI2cSemID \n "));
	}
}

void DRV_CH_UnLockI2c( int nChannelId )
{
	int nRet;
	CH_UNUSED(nChannelId);

	nRet = VK_SEM_Release( ulI2cSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[DRV_CH_UnLockI2c] ERR! VK_SEM_Release() :: ulI2cSemID \n "));
	}
}



void DRV_CH_LockReentrant( void )
{
	int nRet=DI_CH_OK;

	nRet = VK_SEM_Get( ulReentrantSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[DRV_CH_LockReentrant] ERR! VK_SEM_Get() :: ulReentrantSemID \n "));
	}
}

void DRV_CH_UnLockReentrant( void )
{
	int nRet=DI_CH_OK;

	nRet = VK_SEM_Release( ulReentrantSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[DRV_CH_UnLockReentrant] ERR! VK_SEM_Release() :: ulReentrantSemID \n "));
	}
}



#ifdef	CI_TEST_WORKAROUND
extern	unsigned char	f_humax_CI_card_insert;
#endif




void DRV_CH_CheckStatus( void )
{
	int i, nUnitId;

	CH_Status_e 	etCurrentStatus;
	DI_CH_Type_e 	etChannelType;
	CH_TsStatus_e 	etTsOnOff;
	CH_Active_e		etChannelAction;

	CH_TER_TuneParam_t *pstTerTuneParam;
	CH_SAT_TuneParam_t *pstSatTuneParam;
	CH_ANA_TuneParam_t *pstAnaTuneParam;
	CH_CAB_TuneParam_t *pstCabTuneParam;

	for( i=0; i<NUM_OF_CHANNEL; i++ )
	{

		nUnitId 		= GetDeviceUnitId( i );
		etChannelType 	= GetChannelType( i );

		etCurrentStatus = GetTuneStatus( i );
		etTsOnOff 		= GetTsStatus( i );
		etChannelAction = GetActiveStatus( i );

		CH_MON_PrintTypeUnitId( etChannelType, nUnitId);
		CH_MON_PrintAllState(  etCurrentStatus, etTsOnOff, etChannelAction );

		if( etCurrentStatus != CH_STATUS_IDLE )
		{
			switch( etChannelType )
			{
				case DI_CH_TYPE_SAT:
					pstSatTuneParam = GetWorkingTuneInfo( i );
#ifdef __CH_MONITOR__
					CH_MON_PrintSatTuneParam( pstSatTuneParam );
#else
					PrintSatTuneParam( pstSatTuneParam );
#endif
					break;

				case DI_CH_TYPE_TER:
					pstTerTuneParam = GetWorkingTuneInfo( i );
#ifdef __CH_MONITOR__
					CH_MON_PrintTerTuneParam( pstTerTuneParam );
#else
					PrintTerTuneParam( pstTerTuneParam );
#endif
					break;

				case DI_CH_TYPE_CAB:
					pstCabTuneParam = GetWorkingTuneInfo( i );
#ifdef __CH_MONITOR__
					CH_MON_PrintCabTuneParam( pstCabTuneParam );
#else
					PrintCabTuneParam( pstCabTuneParam );
#endif
					break;

				case DI_CH_TYPE_ANA:
					pstAnaTuneParam = GetWorkingTuneInfo( i );
#ifdef __CH_MONITOR__
					CH_MON_PrintAnaTuneParam( pstAnaTuneParam );
#else
					PrintAnaTuneParam( pstAnaTuneParam );
#endif
					break;

				default:
					CH_DI_Print(0, ("[DRV_CH_CheckStatus] ERR! etChannelType(%d) \n", etChannelType));
					break;
			}
		}

		if( etCurrentStatus == CH_STATUS_LOCKED )
		{
			CH_MON_PrintStrenthQuality( i, etChannelType );
		}


	}

}


int DRV_CH_GetDeviceStauts(int nChannelId, DI_CH_DeviceStatus * pstDeviceStatus)
{
	int nRet = DI_CH_OK;
	DI_CH_Type_e etChannelType=0;

	etChannelType = GetChannelType(nChannelId);
/* In case of combo model, It is need to control additionally */
#if defined(CONFIG_FRONTEND_CAB)
	if(etChannelType == DI_CH_TYPE_CAB)
	{
		nRet = CH_C_GetDeviceStatus(nChannelId, pstDeviceStatus);
	}
#elif defined(CONFIG_FRONTEND_SAT)
	if(etChannelType == DI_CH_TYPE_SAT)
	{
		nRet = CH_S_GetDeviceStatus(nChannelId, pstDeviceStatus);
	}
#elif defined(CONFIG_FRONTEND_TER)
	if(etChannelType == DI_CH_TYPE_TER)
	{
		nRet = CH_T_GetDeviceStatus(nChannelId ,pstDeviceStatus);
	}
#else
	CH_DI_Print(0, ("[DRV_CH_GetDeviceStauts] ERR! etChannelType(%d) \n", etChannelType));
	nRet = DI_CH_NOT_SUPPORTED;
#endif
	return nRet;
}

/*********************** End of File ******************************/
