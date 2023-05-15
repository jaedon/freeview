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
// Start Including Header Files
#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "drv_channel.h"

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
extern HINT32 show_di_ch;

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

#define __DRV_CH_ATTRIBUTE_FUNCTIONS__

int FindChannelID ( HUINT32 ulInstanceId, DI_CH_Type_e etChannelType )
{
	int i;
	HBOOL bFind = FALSE;

	CH_DI_Print(6,("[FindChannelID] ulInstanceId[%d], etChannelType[0x%x]\n", ulInstanceId, etChannelType));

	for ( i=0; i<NUM_OF_CHANNEL; i++ )
	{
		if ( ((int)ulInstanceId == g_stChannelConfig[i].nTunerID) \
			&&(etChannelType == g_stChannelConfig[i].etChannelType) )
		{
			bFind = TRUE;
			break;
		}
	}

	if( bFind == TRUE )
	{
		CH_DI_Print(5, ("[FindChannelID] ChannelID[%d]\n", i));
		return i;
	}
	else
	{
		return DI_CH_ERR;
	}

}

int 	GetChannelType ( int nChannelId )
{
	return g_stChannelConfig[nChannelId].etChannelType;
}

int 	GetPreviousTuneStatus ( int nChannelId )
{
	CH_Status_e etCurrentStatus;

	DRV_CH_Lock( nChannelId );
	etCurrentStatus = g_stChannelConfig[nChannelId].stChannelControl.etPreviousState;
	DRV_CH_UnLock( nChannelId );

	return etCurrentStatus;
}

int 	GetTuneStatus ( int nChannelId )
{
	CH_Status_e etCurrentStatus;

	//DRV_CH_Lock( nChannelId );
	etCurrentStatus = g_stChannelConfig[nChannelId].stChannelControl.etCurrentState;
	//DRV_CH_UnLock( nChannelId );

	return etCurrentStatus;
}

void 	SetTuneStatus ( int nChannelId, CH_Status_e etStatus )
{
	//DRV_CH_Lock( nChannelId );

	if( etStatus == CH_STATUS_UNLOCK )
	{
		if( (g_stChannelConfig[nChannelId].stChannelControl.etCurrentState == CH_STATUS_IDLE) \
			|| (g_stChannelConfig[nChannelId].stChannelControl.etCurrentState == CH_STATUS_CANCLE) )
		{
			CH_DI_Print(5, ("[SetTuneStatus] Ignore changing status... \n"));

			//DRV_CH_UnLock( nChannelId );
			return;
		}
	}
	/* 주파수 변경중에 cancellock이 호출 되면 바로 semaphore를 얻지 못해 locked event 전달 후 DisableTsOutput 상태가 된다 */
	/* 이때 동일한 파라메터의 RequestLock이 오게되면 CompareTuneParam에서 alreadylock으로 간주하여 DisableTsOutput 상태로 남게 되는 문제가 있다 */
	/* 아래 코드는 현재 상태가 Locked 상태여도 직전상태가 CANCEL이면 status를 update하지 않게 한다.  */
	/* 이 조건은 MW에서 cancellock이 호출 되면 다음에는 RequestLock이 반드시 와야 한다.  */
	else if( etStatus == CH_STATUS_LOCKED )
	{
		if(g_stChannelConfig[nChannelId].stChannelControl.etCurrentState == CH_STATUS_CANCLE)
		{
			CH_DI_Print(5, ("[SetTuneStatus] Ignore changing status if CANCEL status(%d):(%d)... \n",
					g_stChannelConfig[nChannelId].stChannelControl.etPreviousState,
					g_stChannelConfig[nChannelId].stChannelControl.etCurrentState));

			return;
		}
	}

	/* Update previous state */
	g_stChannelConfig[nChannelId].stChannelControl.etPreviousState = \
					g_stChannelConfig[nChannelId].stChannelControl.etCurrentState;

	g_stChannelConfig[nChannelId].stChannelControl.etCurrentState = etStatus;

	CH_DI_Print(5, ("SetTuneStatus : nChannelId[%d ] SetTuneStatus (%d):(%d) \n",\
					nChannelId,
					g_stChannelConfig[nChannelId].stChannelControl.etPreviousState, \
					g_stChannelConfig[nChannelId].stChannelControl.etCurrentState));

	//DRV_CH_UnLock( nChannelId );

	return;
}

HBOOL	IsStateChanged ( int nChannelId )
{

#if 0
	CH_PrintFunc("(%d):(%d)",\
					g_stChannelConfig[nChannelId].stChannelControl.etPreviousState, \
					g_stChannelConfig[nChannelId].stChannelControl.etCurrentState);
#endif

	if( g_stChannelConfig[nChannelId].stChannelControl.etPreviousState != \
					g_stChannelConfig[nChannelId].stChannelControl.etCurrentState)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HUINT32 GetRequestedId ( int nChannelId )
{
	return g_stChannelConfig[nChannelId].stTuningInfo.unRequestID;
}

HUINT32 GetSearchMode ( int nChannelId )
{
	return g_stChannelConfig[nChannelId].stTuningInfo.ulAutoSearchMode;
}

int 	SetSearchMode ( int nChannelId, HUINT32 ulMode )
{
	g_stChannelConfig[nChannelId].stTuningInfo.ulAutoSearchMode = ulMode;
	return 0;
}

HUINT32 	GetInstanceId ( int nChannelId )
{
	return (HUINT32)g_stChannelConfig[nChannelId].nTunerID;
}

int 	GetDeviceUnitId ( int nChannelId )
{
	return g_stChannelConfig[nChannelId].nUnitId;
}

void * 	GetWorkingTuneInfo( int nChannelId )
{
	return g_stChannelConfig[nChannelId].stTuningInfo.pvWorkingParam;
}

void * 	GetRequestedTuneInfo( int nChannelId )
{
	return g_stChannelConfig[nChannelId].stTuningInfo.pvRequestedParam;
}

void * 	GetPreviousLockedTuneInfo( int nChannelId )
{
	return g_stChannelConfig[nChannelId].stTuningInfo.pvPreviousParam;
}

int 	GetTsOutputMode ( int nChannelId )
{
	return g_stChannelConfig[nChannelId].stTsStatus.etTsOutMode;
}

int 	SetTsOutputMode ( int nChannelId, CH_TsOutMode_e etTsMode )
{
	g_stChannelConfig[nChannelId].stTsStatus.etTsOutMode = etTsMode;
	return 0;
}

int 	GetTsStatus ( int nChannelId )
{
	return g_stChannelConfig[nChannelId].stTsStatus.etTsOnOff;
}

int 	SetTsStatus ( int nChannelId, CH_TsStatus_e TsOnOff )
{
	g_stChannelConfig[nChannelId].stTsStatus.etTsOnOff = TsOnOff;
	return 0;
}

int 	GetActiveStatus ( int nChannelId )
{
	return g_stChannelConfig[nChannelId].etChannelAction;
}

int 	SetActiveStatus ( int nChannelId, CH_Active_e etAction )
{
	g_stChannelConfig[nChannelId].etChannelAction = etAction;
	return 0;
}

int		GetRfInputId ( int nChannelId )
{
	return g_stChannelConfig[nChannelId].etRfInputSource;
}

int		SetRfInputId ( int nChannelId, DI_CH_InputSource_e etInputSource )
{
	if (etInputSource == DI_CH_RF_INPUTLT)
	{
		g_stChannelConfig[nChannelId].etRfInputSource = DI_CH_RF_INPUT1;
	}
	else
	{
#if defined (CONFIG_SUPPORT_2TUNER_1RFINPUT)
		g_stChannelConfig[nChannelId].etRfInputSource = 2*etInputSource;
#else
		g_stChannelConfig[nChannelId].etRfInputSource = etInputSource;
#endif
	}
	return 0;
}

/*********************** End of File ******************************/
