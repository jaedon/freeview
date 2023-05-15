/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:		drv_channel_s.c
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
#include <memory.h>

#include "htype.h"
#include "vkernel.h"

/* di headers */
#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

/* drv headers */
#include "drv_channel.h"
#include "drv_channel_lnb.h"


// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define

#define MAX_NUM_SAT_UNITS		CONFIG_NUM_OF_SAT_CHANNEL
#define DISEQC_MSG_SIZE		4
#define SCD_MSG_SIZE			5

#define NUMBER_OF_RETRYS_TO_SCD	4

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
extern HINT32 show_di_ch;
// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee
static HULONG s_ulDiseqcSemID[MAX_NUM_SAT_UNITS];
static HULONG s_ulControlMsgSemID;
#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
#else
static int s_nMasterUnitId = 0;
#endif

static HUINT8 s_aucDiseqcMsg[DISEQC_MSG_SIZE];
static HUINT8 s_aucScdMsg[SCD_MSG_SIZE];

HUINT16 usScdShiftRegister[8] = {0x0021, 0x0042, 0x0063, 0x0084, 0x00A5, 0x00C6, 0x00E7, 0x0108};

// End static variable


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:
#define __CH_S_PRIVATE_FUNCITONS__



static HUINT32 GetScdRandomDelay(HUINT8 ucUserBand)
{
	unsigned long ulBit0, ulBit7, ulMsb;

	ulBit0 = usScdShiftRegister[ucUserBand] & 0x00000001;
	ulBit7 = (usScdShiftRegister[ucUserBand] >> 7) & 0x00000001;
	ulMsb = ulBit0 ^ ulBit7;
	usScdShiftRegister[ucUserBand] = usScdShiftRegister[ucUserBand] >> 1;
	usScdShiftRegister[ucUserBand] &= ~0x00000200;
	usScdShiftRegister[ucUserBand] |= ulMsb << 9;

 	return ((1000ul*usScdShiftRegister[ucUserBand])/1024ul);
}



 int MakeSwitchMsg( CH_SAT_DiseqcVer_e etVersion, CH_SAT_DiseqcInput_e etPosition, CH_SAT_Polarization_e etPolar, HBOOL bTone )
{
	int nMsgLength = 0;

	s_aucDiseqcMsg[0] = 0xE0; // Framing Byte (0xE0 for Type1, 0xE2 for Type2)
	s_aucDiseqcMsg[1] = 0x10; // Address Byte;
	s_aucDiseqcMsg[2] = 0x38; // Command Byte
	s_aucDiseqcMsg[3] = 0xf0; // Data Byte

    if( etVersion == DI_CH_DISEQC_VER_2_0 )
    {
		s_aucDiseqcMsg[0] |= 0x02;
    }

    switch(etPosition)
    {
        case DI_CH_DISEQC_INPUT_B:
            s_aucDiseqcMsg[3] |= 0x04;
            break;

        case DI_CH_DISEQC_INPUT_C:
            s_aucDiseqcMsg[3] |= 0x08;
            break;

        case DI_CH_DISEQC_INPUT_D :
            s_aucDiseqcMsg[3] |= 0x0C;
            break;

	 case DI_CH_DISEQC_INPUT_A :
        case DI_CH_DISEQC_INPUT_TB_A:
        case DI_CH_DISEQC_INPUT_TB_B:
        case DI_CH_DISEQC_INPUT_NONE :
        default:

            break;
    }

    if( etPosition !=DI_CH_DISEQC_INPUT_NONE )
    {
    	if( (etPolar == DI_CH_POLAR_HOR)||(etPolar == DI_CH_POLAR_LEFT) )
    	{
			s_aucDiseqcMsg[3] |= 0x02;
		}

	    if( bTone )
	    {
	    	s_aucDiseqcMsg[3] |= 0x01;
	    }
    }

	if( etPosition==DI_CH_DISEQC_INPUT_NONE )
	{
		//Diseqc clear command
		s_aucDiseqcMsg[0] = 0xE0;
		s_aucDiseqcMsg[1] = 0x10;
		s_aucDiseqcMsg[2] = 0x00;
		nMsgLength = 3;
	}
	else
	{
    		nMsgLength = 4;
	}

	CH_DI_Print(CH_LOG_ARGS, ("\n\n DiseqcMsg : [%x] [%x] [%x] [%x] \n\n", s_aucDiseqcMsg[0], s_aucDiseqcMsg[1], s_aucDiseqcMsg[2], s_aucDiseqcMsg[3]));

	return nMsgLength;
}


static int MakeScdMsg( CH_SAT_ScdCmd_e etScdCmd, HUINT32 ulUserBand, HUINT32 ulUserbandFreq, HUINT32 ulTuneFreq,
							  CH_SAT_DiseqcInput_e etPosition, CH_SAT_Polarization_e etPolar, HBOOL bTone )
{
	HUINT8	ucUserBand;
	HUINT32 ulTvalue;
	HUINT8 	ucUpperT;
	int i;

	ucUserBand = (HUINT8)ulUserBand;

	for( i=0; i<SCD_MSG_SIZE; i++ )
	{
		s_aucScdMsg[i]=0;
	}

	s_aucScdMsg[0] = 0xE0; // Framing Byte (0xE0 for Type1, 0xE2 for Type2)
	s_aucScdMsg[1] = 0x10; // Address Byte
	s_aucScdMsg[2] = 0x5A; // Command Byte

	switch( etScdCmd )
	{
		case ODU_Channel_change:

			//Set user band
			s_aucScdMsg[3] |= (ucUserBand << 5)&0xE0;

			//Set input bank
			if( etPolar == DI_CH_POLAR_HOR )
			{
				s_aucScdMsg[3] |= 0x08;
			}
			if( bTone == TRUE )
			{
				s_aucScdMsg[3] |= 0x04;
			}
			if( etPosition == DI_CH_DISEQC_INPUT_SCD_B )
			{
				s_aucScdMsg[3] |= 0x10;
			}

			/* Calculate T value : T = round(freq+userbandfreq)/Stepsize -350 */
			ulTvalue = ulTuneFreq;
			ulTvalue += ulUserbandFreq; // Mhz
			ulTvalue /= 4; // Stepsize == 4 MHz
			ulTvalue -= 350;

			ucUpperT = ( ulTvalue >> 8 ) & 0x03;
			//CH_DI_Print("ucUpperT(0x%x)", ucUpperT );
			s_aucScdMsg[3] |= ucUpperT;
			s_aucScdMsg[4] = ulTvalue & 0xff; // Data2 Byte

			break;

		case ODU_PowerOFF:
			//Set user band
			s_aucScdMsg[3] = 0;
			s_aucScdMsg[3] |= (ucUserBand << 5);
			s_aucScdMsg[4] = 0x00; // Data2 Byte

			break;

		case ODU_UbxSignal_ON:
		case ODU_Config:
		case ODU_LoFreq:
		default:
			break;
	}

	CH_DI_Print(CH_LOG_ARGS, ("\n\n ScdCmd[%d] DiseqcMsg : [%x] [%x] [%x] [%x] \n\n", etScdCmd, s_aucScdMsg[0], s_aucScdMsg[1], s_aucScdMsg[2], s_aucScdMsg[3], s_aucScdMsg[4]));

	return SCD_MSG_SIZE;

}



static void CH_S_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId )
{
	int nRet;
	int nActiveState;
	int nAntennaState;

	nRet = CH_S_SetLnbOutput( nChannelId, TRUE );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ProcessRecoverAntenna] ERR!  CH_S_SetLnbOutput() nRet (%d) \n", nRet));
	}

	VK_TASK_Sleep(150);// wait 150ms

	nAntennaState = LNB_CheckAntState( nChannelId );
	CH_DI_Print(CH_LOG_ARGS	, ("nAntennaState(%d)", nAntennaState));

	if( nAntennaState == DI_CH_ANTENNA_FAULT )
	{
		//LNB output will be disabled...
		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT );
	}
	else if( nAntennaState == DI_CH_ERR )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ProcessRecoverAntenna] ERR! I2C read err...\n"));
	}
	else
	{
		CH_DI_Print(CH_LOG_ARGS	, ("Antenna recovered...\n"));

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_OK );

		nActiveState = GetActiveStatus( nChannelId );
		if( nActiveState == CH_ACTION_OFF )
		{
			SetTuneStatus( nChannelId, CH_STATUS_IDLE );
		}
		else if( nActiveState == CH_ACTION_ON )
		{
			SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );
		}
		else
		{
			//Do nothing
		}

	}

	return;

}


HBOOL CH_S_CheckLock ( int nChannelId )
{
	HBOOL bLocked;

	DRV_CH_Lock( nChannelId );

	bLocked = NIM_CheckLock( nChannelId );
	if( bLocked == FALSE )
	{
		VK_TASK_Sleep(100);//wait 100ms

		bLocked = NIM_CheckLock( nChannelId );
		if( bLocked == FALSE )
		{
			CH_DI_Print(CH_LOG_TUNE, ("[CH_S_CheckLock] Unlock!!! \n"));
		}
	}

	DRV_CH_UnLock( nChannelId );

	return(bLocked);

}



static DI_CHANNEL_ERR RetryScdSetTune( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	int nRet = DI_CH_OK;

	GetScdRandomDelay( pstSatTuneParam->ulScdUserBandNum );
	GetScdRandomDelay( pstSatTuneParam->ulScdUserBandNum );

	CH_DI_Print(CH_LOG_ARGS, ("\n ############ RetryScdSetTune[%d] ##################\n", nChannelId));

#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
	nRet = CH_S_ControlScd( nChannelId, ODU_Channel_change, pstSatTuneParam );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>CH_S_ControlScd<<<<<<<<<<<<<< \n"));
		CH_S_ControlScd( nChannelId, ODU_Channel_change, pstSatTuneParam );
	}
#else
	nRet = CH_S_ControlScd( s_nMasterUnitId, ODU_Channel_change, pstSatTuneParam );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>CH_S_ControlScd<<<<<<<<<<<<<< \n"));
		CH_S_ControlScd(  s_nMasterUnitId, ODU_Channel_change, pstSatTuneParam );
	}
#endif

	nRet = NIM_SetTune( nChannelId, pstSatTuneParam );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[RetryScdSetTune] NIM_SetTune Error (%d) \n",nRet));
		//nResult = DI_CH_ERR;
	}

	nRet = NIM_CheckTuneState( nChannelId, pstSatTuneParam->etTransSpec, pstSatTuneParam->ulSymbolRate );

	return nRet;

}


#if defined(WORKAROUND_AUTO_SEARCH)
static DI_CHANNEL_ERR RetryLnbSetTune( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	int nRet = DI_CH_OK;

	nRet = CH_S_ControlLnb( nChannelId, pstSatTuneParam);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>CH_S_ControlLnb<<<<<<<<<<<<<< \n"));
		nRet = CH_S_ControlLnb( nChannelId, pstSatTuneParam);
	}

	nRet = NIM_SetTune( nChannelId, pstSatTuneParam );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[RetryLnbSetTune] NIM_SetTune Error (%d) \n",nRet));
		//nResult = DI_CH_ERR;
	}

	nRet = NIM_CheckTuneState( nChannelId, pstSatTuneParam->etTransSpec, pstSatTuneParam->ulSymbolRate );

	return nRet;

}



static DI_CHANNEL_ERR RetrySwitchSetTune( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	int nRet = DI_CH_OK;

	nRet = CH_S_ControlSwitch( nChannelId, pstSatTuneParam);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>CH_S_ControlSwitch<<<<<<<<<<<<<< \n"));
		nRet = CH_S_ControlSwitch( nChannelId, pstSatTuneParam);
	}

	nRet = NIM_SetTune( nChannelId, pstSatTuneParam );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[RetrySwitchSetTune] NIM_SetTune Error (%d) \n",nRet));
		//nResult = DI_CH_ERR;
	}

	nRet = NIM_CheckTuneState( nChannelId, pstSatTuneParam->etTransSpec, pstSatTuneParam->ulSymbolRate );

	return nRet;

}
#endif


void LockDiseqc( int nChannelId )
{
	int nRet;

	nRet = VK_SEM_Get( s_ulDiseqcSemID[nChannelId] );
	if(nRet)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[LockDiseqc] ERR! VK_SEM_Get :: s_ulDiseqcSemID  nRet(%d) \n ", nRet));
	}
}

void UnLockDiseqc( int nChannelId )
{
	int nRet;

	nRet = VK_SEM_Release( s_ulDiseqcSemID[nChannelId] );
	if(nRet)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[UnLockDiseqc] ERR!VK_SEM_Release:: s_ulDiseqcSemID  nRet(%d) \n ", nRet));
	}
}

void LockControlMsg( void )
{
	int nRet;

	nRet = VK_SEM_Get( s_ulControlMsgSemID );
	if(nRet)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[LockControlMsg] ERR! VK_SEM_Get :: s_ulControlMsgSemID  nRet(%d) \n ", nRet));
	}
}

void UnLockControlMsg( void )
{
	int nRet;

	nRet = VK_SEM_Release( s_ulControlMsgSemID );
	if(nRet)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[UnLockControlMsg] ERR!VK_SEM_Release:: s_ulControlMsgSemID  nRet(%d) \n ", nRet));
	}
}

#define __CH_S_PUBLIC_FUNCITONS__


DI_CHANNEL_ERR CH_S_ReInitDevice( int nChannelId )
{
    int nResult = DI_CH_OK;
    int nRet;

   CH_DI_Print(CH_LOG_ARGS,("CH_S_ReInitDevice Enter ...\n"));

    nRet = LNB_InitDevice( nChannelId );
    if ( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! LNB_InitDevice() nRet (%d) \n ", nRet));
        /* Return DI_CH_OK, when reason of booting fail is LNB initialiaztion fail */
        nResult = DI_CH_OK;
    }

#if defined(CONFIG_PLATFORM_DB7346) && defined(CONFIG_PLATFORM_LA8296)
    //HDR-1003S has only configuration with both BCM7346 & A8296.
    if(nChannelId == 1)
    {
        nRet = NIM_SetInputSource(1, DI_CH_RF_INPUT1);
        if( nRet != DI_CH_OK )
        {
            CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! NIM_SetInputSource() nRet (%d) \n ", nRet));
            nResult = DI_CH_ERR;
        }
    }
#endif

#if defined(CONFIG_PLATFORM_DB45216) 
    nRet = NIM_ReInitDevice(nChannelId);
    if ( nRet != DI_CH_OK )
    {
	CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! NIM_ReInitDevice() nRet (%d) \n ", nRet));
	nResult = DI_CH_ERR;
    }
#endif

    /* After NIM is initialized, then Set default of the 22KHz tone value. */
    nRet = LNB_Set22KhzTone( nChannelId, FALSE );
    if ( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! LNB_Set22KhzTone() nRet (%d) \n ", nRet));
        nResult = DI_CH_ERR;
    }

    nRet = NIM_DisableTsOutput( nChannelId );
    if( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! NIM_DisableTsOutput() nRet (%d) \n ", nRet));
        nResult = DI_CH_ERR;
    }

    nRet = NIM_SetPowerState( nChannelId, CH_POWER_OFF );
    if ( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! NIM_SetPowerState() nRet (%d) \n ", nRet));
        nResult = DI_CH_ERR;
    }

	return nResult;
}



DI_CHANNEL_ERR CH_S_InitDev( int nChannelId )
{
	int nResult = DI_CH_OK;
	int nRet;
	HUINT8 unitId;

	SetTuneStatus ( nChannelId, CH_STATUS_IDLE );

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );
	if(unitId == 0)
		nRet = VK_SEM_Create( &s_ulDiseqcSemID[nChannelId], "di_chDiseqc_0", VK_SUSPENDTYPE_MAX);
	else if (unitId == 1)
		nRet = VK_SEM_Create( &s_ulDiseqcSemID[nChannelId], "di_chDiseqc_1", VK_SUSPENDTYPE_MAX);
	else if (unitId == 2)
		nRet = VK_SEM_Create( &s_ulDiseqcSemID[nChannelId], "di_chDiseqc_2", VK_SUSPENDTYPE_MAX);
	else if (unitId == 3)
		nRet = VK_SEM_Create( &s_ulDiseqcSemID[nChannelId], "di_chDiseqc_3", VK_SUSPENDTYPE_MAX);
	else
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! No Matched UnitId (%d) \n", unitId));
		return DI_CH_ERR;
	}

	if( nRet != VK_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! VK_SEM_Create() :: s_ulDiseqcSemID  nRet(0x%x)", nRet));
		return DI_CH_ERR;
	}

	nRet = VK_SEM_Create( &s_ulControlMsgSemID, "di_chControlMsg", VK_SUSPENDTYPE_MAX);
	if( nRet != VK_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! VK_SEM_Create() :: s_ulDiseqcSemID  nRet(0x%x)", nRet));
		return DI_CH_ERR;
	}

	DRV_S_InstallApi();

	nRet = LNB_InitDevice( nChannelId );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! LNB_InitDevice() nRet (%d) \n ", nRet));
		/* Return DI_CH_OK, when reason of booting fail is LNB initialiaztion fail */
		nResult = DI_CH_OK;
	}

	nRet = NIM_InitDevice( nChannelId );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! NIM_InitDevice() nRet (%d) \n ", nRet));
		nResult = DI_CH_ERR;
	}

#if defined(CONFIG_PLATFORM_DB7346) && defined(CONFIG_PLATFORM_LA8296)
	//HDR-1003S has only configuration with both BCM7346 & A8296.
	if(nChannelId == 1)
	{
		nRet = NIM_SetInputSource(1, DI_CH_RF_INPUT1);
		if( nRet != DI_CH_OK )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! NIM_SetInputSource() nRet (%d) \n ", nRet));
			nResult = DI_CH_ERR;
		}
	}
#endif

	/* After NIM is initialized, then Set default of the 22KHz tone value. */
	nRet = LNB_Set22KhzTone( nChannelId, FALSE );
	//nRet = LNB_Set22KhzTone( nChannelId, TRUE );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! LNB_Set22KhzTone() nRet (%d) \n ", nRet));
	    nResult = DI_CH_ERR;
	}

	nRet = NIM_DisableTsOutput( nChannelId );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! NIM_DisableTsOutput() nRet (%d) \n ", nRet));
		nResult = DI_CH_ERR;
	}

	nRet = NIM_SetPowerState( nChannelId, CH_POWER_OFF );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! NIM_SetPowerState() nRet (%d) \n ", nRet));
		nResult = DI_CH_ERR;
	}

	//NIM_SetInputMode(0,DI_CH_INPUT_ONE_SAME);//NIM_SetInputMode(0,DI_CH_INPUT_TWO_DIFF);

	return nResult;
}


DI_CHANNEL_ERR CH_S_UnInitDevice( int nChannelId )
{
    int nRet;
    int nResult = DI_CH_OK;

    nRet = LNB_SetOutput( nChannelId, FALSE );
    if ( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_UnInitDevice] ERR!  LNB_SetOutput() nRet(%d) \n", nRet));
        nResult = DI_CH_ERR;
    }

    nRet = NIM_DisableTsOutput( nChannelId );
    if( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_UnInitDevice] ERR! NIM_DisableTsOutput() nRet (%d) \n ", nRet));
        nResult = DI_CH_ERR;
    }

    nRet = NIM_SetPowerState( nChannelId, CH_POWER_OFF );
    if ( nRet != DI_CH_OK )
    {
    	CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_UnInitDevice] ERR! NIM_SetPowerState() nRet (%d) \n ", nRet));
    	nResult = DI_CH_ERR;
    }

#if defined(CONFIG_PLATFORM_DB45216) 
    nRet = NIM_UnInitDevice(nChannelId);
    if ( nRet != DI_CH_OK )
    {
       CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_InitDev] ERR! NIM_UnInitDevice() nRet (%d) \n ", nRet));
       nResult = DI_CH_ERR;
    }
#endif 

    return nResult;
}


/**
 * Tuning process in driver layer.
 *
 * \param[in]	nChannelId			identity for channel
 *
 * \param[in]	ulWorkingId			sync for process
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     If unlocked, then return to orginal params requested by MW.
 *				Coution about managing channel status (CH_Status_e).
 *
 * \see
 */
DI_CHANNEL_ERR CH_S_SetTune( int nChannelId, HUINT32 ulWorkingId )
{
	int nUnitId;
	int nResult = DI_CH_OK;
	int nRet,nRet2;
	CH_SAT_TuneParam_t * pstSatTuneParam;
	CH_SAT_TuneParam_t * pstMWTuneParam;
	HUINT32 ulSearchMode = 0;
	int i;

	DRV_CH_Lock( nChannelId );

	CH_DI_Print(CH_LOG_ARGS, ("[CH_S_SetTune] ---- \n "));

	nUnitId = (HUINT8)GetDeviceUnitId( nChannelId );

	pstSatTuneParam = (CH_SAT_TuneParam_t *)GetWorkingTuneInfo( nChannelId );
	pstMWTuneParam	= (CH_SAT_TuneParam_t *)GetRequestedTuneInfo( nChannelId );

	//PrintSatTuneParam( pstSatTuneParam );

	CH_DI_Print(CH_LOG_ARGS, ("[CH_S_SetTune] nChannelId[%d]: ulFrequency = (%d)\n ", nChannelId, pstSatTuneParam->ulFrequency));
	if(pstSatTuneParam->ulFrequency == 0)
	{
		CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>Invalid frequency!!! goto IDLE status...\n"));
		SetTuneStatus( nChannelId, CH_STATUS_IDLE );
		DRV_CH_UnLock( nChannelId );
		return DI_CH_OK;
	}

	if(pstSatTuneParam->ulSymbolRate == 0)
	{
		CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>Invalid symbolrate!!! goto IDLE status...\n"));
		SetTuneStatus( nChannelId, CH_STATUS_IDLE );
		DRV_CH_UnLock( nChannelId );
		return DI_CH_OK;
	}

	switch ( pstSatTuneParam->etAntennaType )
	{
		case DI_CH_ANT_TYPE_LNB:
			nRet = CH_S_ControlLnb( nChannelId, pstSatTuneParam );
			if( nRet != DI_CH_OK )
			{
				CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>CH_S_ControlLnb<<<<<<<<<<<<<< \n"));
				nRet2 = CH_S_ControlLnb( nChannelId, pstSatTuneParam );
			}
			else
			{
				//OK
			}
			break;

		case DI_CH_ANT_TYPE_DISEQC:
			nRet = CH_S_ControlSwitch( nChannelId, pstSatTuneParam );
			if( nRet != DI_CH_OK )
			{
				CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>CH_S_ControlSwitch<<<<<<<<<<<<<< \n"));
				nRet2 = CH_S_ControlSwitch( nChannelId, pstSatTuneParam );
			}
			else
			{
				//OK
			}
			break;

		case DI_CH_ANT_TYPE_SCD:
#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
			nRet = CH_S_ControlScd( nChannelId, ODU_Channel_change, pstSatTuneParam );
			if( nRet != DI_CH_OK )
			{
				CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>CH_S_ControlScd<<<<<<<<<<<<<< \n"));
				nRet2 = CH_S_ControlScd( nChannelId, ODU_Channel_change, pstSatTuneParam );
			}
#else
			s_nMasterUnitId = pstSatTuneParam->ulScdMasterUnitId;
			CH_DI_Print(CH_LOG_TUNE, ("DI_CH_ANT_TYPE_SCD :  s_nMasterUnitId (%d) \n", s_nMasterUnitId));
			nRet = CH_S_ControlScd( s_nMasterUnitId, ODU_Channel_change, pstSatTuneParam );
			if( nRet != DI_CH_OK )
			{
				CH_DI_Print(CH_LOG_DEFAULT, (">>>>>>>>>>>CH_S_ControlScd<<<<<<<<<<<<<< \n"));
				nRet2 = CH_S_ControlScd(  s_nMasterUnitId, ODU_Channel_change, pstSatTuneParam );
			}
#endif
			else
			{
				//OK
			}
			break;

		case DI_CH_ANT_TYPE_SMATV:

			break;

		default:
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetTune] No Matched Antenna Type \n "));
			break;
	}

	nRet = NIM_SetTune( nChannelId, pstSatTuneParam );
	if( nRet == DI_CH_ERR )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetTune] ERR! NIM_SetTune() \n "));
		nResult = DI_CH_ERR;
	}
	else if( nRet == DI_CH_NOT_SUPPORTED )
	{
		DRV_CH_UnLock( nChannelId );

		CH_DI_Print(CH_LOG_TUNE, (">>> SAT CH_STATUS_UNLOCK\n"));
		SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );

		DRV_CH_CallNotifyCallback( nChannelId, ulWorkingId, DI_CH_SIGNAL_UNLOCK );
		return DI_CH_NOT_SUPPORTED;
	}
	else
	{
		/* Do Nothing */
	}

	nRet = NIM_CheckTuneState( nChannelId, pstSatTuneParam->etTransSpec, pstSatTuneParam->ulSymbolRate );

	if( nRet == DI_CH_ERR )
	{
		nResult = DI_CH_ERR;

		if( pstSatTuneParam->etAntennaType == DI_CH_ANT_TYPE_SCD )
		{
			for( i = 0; i < NUMBER_OF_RETRYS_TO_SCD; i++ )
			{
				CH_DI_Print(CH_LOG_TUNE, ("SCD channel is not changed...\n"));

				if( pstSatTuneParam->etPolarization == DI_CH_POLAR_AUTO )
				{
					pstSatTuneParam->etPolarization = DI_CH_POLAR_HOR;
				}

				nRet = RetryScdSetTune( nChannelId, pstSatTuneParam );
				if( nRet == DI_CH_OK || nRet == DI_CH_CANCEL_TUNE)
				{
					CH_DI_Print(CH_LOG_TUNE, ("TS is changed during the SCD retry...\n"));
					break;
				}
			}
		}
#if defined(WORKAROUND_AUTO_SEARCH)
		else if( pstSatTuneParam->etAntennaType == DI_CH_ANT_TYPE_LNB )
		{
			nRet = RetryLnbSetTune( nChannelId, pstSatTuneParam );
			if( nRet == DI_CH_OK )
			{
				CH_DI_Print(CH_LOG_TUNE, ("RetryLnbSetTune Locked \n"));
			}
			else
			{
				CH_DI_Print(CH_LOG_TUNE, ("RetryLnbSetTune Unlocked \n"));
			}
		}
		else if( pstSatTuneParam->etAntennaType == DI_CH_ANT_TYPE_DISEQC )
		{
			nRet = RetrySwitchSetTune( nChannelId, pstSatTuneParam );
			if( nRet == DI_CH_OK )
			{
				CH_DI_Print(CH_LOG_TUNE, ("RetrySwitchSetTune Locked \n"));
			}
			else
			{
				CH_DI_Print(CH_LOG_TUNE, ("RetrySwitchSetTune Unlocked \n"));
			}
		}
#endif
	}
	else
	{
		/* Do Nothing */
	}

	if( nRet == DI_CH_CANCEL_TUNE )
	{
		DRV_CH_UnLock( nChannelId );
		CH_DI_Print(CH_LOG_TUNE, (">>> SAT DI_CH_CANCEL_TUNE\n"));
		return DI_CH_CANCEL_TUNE;
	}

	DRV_CH_UnLock( nChannelId );

	ulSearchMode = GetSearchMode( nChannelId );

	if ( nRet == DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_TUNE, (">>> SAT CH_STATUS_LOCKED \n"));
		//SetTuneStatus( nChannelId, CH_STATUS_LOCKED );

		nRet = CH_S_EnableTsOutput(nChannelId);
		if( nRet != DI_CH_OK )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetTune] ERR! CH_S_EnableTsOutput() \n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			/* Do Nothing */
		}

		if( ulSearchMode == CH_S_POLAR )
		{
			SetSearchMode( nChannelId, ( ulSearchMode & (~CH_S_POLAR) ) );
		}

		NIM_UpdateTunedParam(  nChannelId, pstSatTuneParam );

		SetTuneStatus( nChannelId, CH_STATUS_LOCKED );
		CH_DI_Print(CH_LOG_TUNE, (">>> [%d] CH_STATUS_LOCKED (%d) \n",nChannelId, pstSatTuneParam->etPolarization));

		DRV_CH_CallNotifyCallback( nChannelId, ulWorkingId, DI_CH_SIGNAL_LOCKED );

	}
	else
	{
		if( (ulSearchMode == CH_S_POLAR) && (pstSatTuneParam->etPolarization == DI_CH_POLAR_HOR) )
		{
			nRet = SendChannelCommand( CH_CMD_AUTOSEARCH, nChannelId, ulWorkingId );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetTune] ERR!SendChannelCommand() nRet(0x%x) \n", nRet));
				nResult = DI_CH_ERR;
			}
		}
		else
		{
			/* return to original request tune param */
			{
				VK_memcpy( 	pstSatTuneParam, \
							pstMWTuneParam, \
							sizeof(CH_SAT_TuneParam_t) );

			}
			CH_DI_Print(CH_LOG_TUNE, (">>> SAT CH_STATUS_UNLOCK\n"));
			SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );
			CH_DI_Print(CH_LOG_TUNE, (">>> [%d] CH_STATUS_UNLOCK (%d) \n",nChannelId, pstSatTuneParam->etPolarization));
			DRV_CH_CallNotifyCallback( nChannelId, ulWorkingId, DI_CH_SIGNAL_UNLOCK );
		}
	}

	return nResult;
}


int CH_S_GetSignalInfo ( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo )
{
	int nRet = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nRet = NIM_GetSignalInfo( nChannelId, pstSignalInfo);
	if( nRet != DI_CH_OK )
	{
		nRet = NIM_GetSignalInfo( nChannelId, pstSignalInfo);
	}

	DRV_CH_UnLock( nChannelId );

	return nRet;
}

int CH_S_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	int nRet = DI_CH_OK;
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nRet = NIM_SetPowerState( nChannelId, etPower );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetPowerState] ERR! NIM_SetPowerState() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		if( etPower == CH_POWER_OFF )
		{
			SetActiveStatus( nChannelId, CH_ACTION_OFF );
		}
		else if( etPower == CH_POWER_ON )
		{
			SetActiveStatus( nChannelId, CH_ACTION_ON );
		}
		else
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetPowerState] Not matched.  etPower (%d) \n", etPower));
		}
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;

}



int CH_S_EnableTsOutput ( int nChannelId )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nRet = NIM_EnableTsOutput( nChannelId );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_EnableTsOutput] ERR! NIM_EnableTsOutput() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		SetTsStatus( nChannelId, CH_TS_PATH_ON );
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;
}


int CH_S_DisableTsOutput ( int nChannelId )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nRet = NIM_DisableTsOutput( nChannelId );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_DisableTsOutput] ERR! NIM_DisableTsOutput() nRet (%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		SetTsStatus( nChannelId, CH_TS_PATH_OFF );
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;
}



int CH_S_SendDiseqcMsg( int nChannelId, HUINT8 *pucSendBuf, HUINT8 ucNumOfBuf, CH_SAT_DiseqcInput_e etDiseqcInput )
{
	int nRet = DI_CH_OK;
	int nResult = DI_CH_OK;

	nRet = NIM_SendDiseqcMsg( nChannelId, pucSendBuf, ucNumOfBuf, etDiseqcInput );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SendDiseqcMsg] ERR!  NIM_SendDiseqcMsg() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}

	return nResult;
}


int CH_S_ReceiveDiseqcMsg( int nChannelId, HUINT8 *pucSendBuf, HUINT8 ucNumOfBuf, HUINT8 *pucReplyLength )
{
	int nRet = DI_CH_OK;
	int nResult = DI_CH_OK;

	if(NIM_ReceiveDiseqcMsg == NULL)
	{
		return DI_CH_NOT_SUPPORTED;
	}
	else
	{
		nRet = NIM_ReceiveDiseqcMsg( nChannelId, pucSendBuf, ucNumOfBuf, pucReplyLength );
		if( nRet != DI_CH_OK )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ReceiveDiseqcMsg] ERR!  NIM_ReceiveDiseqcMsg() nRet(%d) \n", nRet));
			nResult = DI_CH_ERR;
		}
	}
	return nResult;
}


int CH_S_SetLnbOutput( int nChannelId, HBOOL bEnable )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;

	nRet = LNB_SetOutput( nChannelId, bEnable );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetLnbOutput] ERR!  LNB_SetOutput() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}

	return nResult;
}


int CH_S_SetLnb22KhzTone( int nChannelId, HBOOL bEnable )
{
	int nRet = DI_CH_OK;
	int nResult = DI_CH_OK;

	nRet = LNB_Set22KhzTone( nChannelId, bEnable );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetLnb22KhzTone] ERR!  LNB_Set22KhzTone() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE	, ("Success : LNB_Set22KhzTone\n"));
	}

	return nResult;
}

int CH_S_SetLnbPolarisation( int nChannelId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage )
{
	int nRet = DI_CH_OK;
	int nResult = DI_CH_OK;

	nRet = LNB_SetPolarisation( nChannelId, etPolar, etVoltage );//M_HORIZONTAL =1
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetLnbPolarisation] ERR!  LNB_SetPolarisation() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE	, ("Success : LNB_SetPolarisation\n"));
	}

	return nResult;
}


int CH_S_SetToneBurst(int nChannelId, HBOOL bToneOn, CH_SAT_DiseqcInput_e etInput, CH_SAT_AntennaType_e etAntennaType, CH_SAT_DiseqcVer_e etDiseqcVer )
{
	int nRet = DI_CH_OK;
	int nResult = DI_CH_OK;

	nRet = NIM_SetToneBurst( nChannelId, bToneOn, etInput, etAntennaType, etDiseqcVer );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetToneBurst] ERR!  NIM_SetToneBurst() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE	, ("CH_S_SetToneBurst : NIM_SetToneBurst\n"));
	}

	return nResult;
}


int CH_S_ResetDiseqc( int nChannelId )
{
	int nRet = DI_CH_OK;
	int nResult = DI_CH_OK;

	nRet = NIM_ResetDiseqc( nChannelId );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ResetDiseqc] ERR!  NIM_ResetDiseqc() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}

	return nResult;
}


int CH_S_ControlLnb( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	int nRet	= DI_CH_OK;
	int nResult = DI_CH_OK;
	HUINT32 ulSearchMode = 0;
	HBOOL bToneOn;

	ulSearchMode = GetSearchMode( nChannelId );

	//1. Set polarization
	if( pstSatTuneParam->etPolarization == DI_CH_POLAR_AUTO )
	{
		ulSearchMode |= CH_S_POLAR;
		SetSearchMode( nChannelId, ulSearchMode );
		pstSatTuneParam->etPolarization = DI_CH_POLAR_HOR;
	}

	nRet = CH_S_SetLnbPolarisation( nChannelId, pstSatTuneParam->etPolarization, pstSatTuneParam->etLnbVoltage );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlLnb] ERR!  CH_S_SetLnbPolarisation() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_ARGS, ("Success : CH_S_SetLnbPolarisation\n"));
	}

	//2. Set 22KHz tone
	bToneOn = LNB_Get22KhzTone( nChannelId );
	if( bToneOn == pstSatTuneParam->b22kTone )
	{
		/* Do nothing */
		CH_DI_Print(CH_LOG_ARGS, ("Desired tone status...\n"));
	}
	else
	{
		nRet = CH_S_SetLnb22KhzTone( nChannelId, pstSatTuneParam->b22kTone );
		if( nRet == DI_CH_ERR )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlLnb] ERR!  CH_S_SetLnb22KhzTone() \n"));
			nResult = DI_CH_ERR;
		}
		else
		{
			CH_DI_Print(CH_LOG_TUNE, ("Success : CH_S_SetLnb22KhzTone\n"));
		}

	}

	return nResult;

}


int CH_S_ControlSwitch( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	int nRet;
	int nResult = DI_CH_OK;
	HUINT32 ulSearchMode;
	int nMsgLength = 0;

	LockControlMsg();

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_LockReentrant();
	VK_TASK_Sleep( 40 );
#endif

	ulSearchMode = GetSearchMode( nChannelId );

#if 0//TODO_ILEE : defined (PLATFORM_DB7335)
	if(nChannelId == 0)
	{
		/* Delay for Connection Detection Issue */
		VK_TASK_Sleep(50);
	}
#endif

#if defined(CONFIG_PLATFORM_DB7362) || defined(CONFIG_PLATFORM_DB45216)  
	//reset diseqc
	nRet = CH_S_ResetDiseqc(nChannelId);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlSwitch] ERR!	CH_S_ResetDiseqc() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE, ("Success : CH_S_ResetDiseqc()\n"));
	}
#endif

	//0. Tone off and set tone burst and select diseqc
#if defined(CONFIG_PLATFORM_DB4538)|| defined(CONFIG_PLATFORM_DB7362)
	nRet = CH_S_SetToneBurst(nChannelId, pstSatTuneParam->b22kTone, pstSatTuneParam->etDiseqcInput, pstSatTuneParam->etAntennaType, pstSatTuneParam->etDiseqcVersion);
#else
	nRet = CH_S_SetToneBurst(nChannelId, FALSE, pstSatTuneParam->etDiseqcInput, pstSatTuneParam->etAntennaType, pstSatTuneParam->etDiseqcVersion);
#endif
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlSwitch] ERR!	CH_S_SetToneBurst() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE, ("Success : CH_S_SetToneBurst()\n"));
	}

	//1. Set polarization
	if( pstSatTuneParam->etPolarization == DI_CH_POLAR_AUTO )
	{
		ulSearchMode |= CH_S_POLAR;
		SetSearchMode( nChannelId, ulSearchMode );
		pstSatTuneParam->etPolarization = DI_CH_POLAR_HOR;
	}

	nRet = CH_S_SetLnbPolarisation( nChannelId, pstSatTuneParam->etPolarization, pstSatTuneParam->etLnbVoltage );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlSwitch] ERR!	CH_S_SetLnbPolarisation() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE, ("Success : CH_S_SetLnbPolarisation\n"));
	}

	VK_TASK_Sleep( 20 );

	if( pstSatTuneParam->etDiseqcInput != DI_CH_DISEQC_INPUT_NONE )
	{
		//2. Make DiSEqC msg
		nMsgLength = MakeSwitchMsg( pstSatTuneParam->etDiseqcVersion, \
								 pstSatTuneParam->etDiseqcInput, \
								 pstSatTuneParam->etPolarization, \
								 pstSatTuneParam->b22kTone);

		//3. Send DiSEqC msg
		nRet = CH_S_SendDiseqcMsg( nChannelId, s_aucDiseqcMsg, nMsgLength, pstSatTuneParam->etDiseqcInput );
		if( nRet != DI_CH_OK )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlSwitch] ERR! Cannot send disecq msg \n"));
			nResult = DI_CH_ERR;
		}


	}

	VK_TASK_Sleep( 10 );//The minimum delay of BCM7358  is 10ms.

#ifndef CONFIG_PLATFORM_DB4538
	//5. Enable 22KHz tone if required
	nRet = CH_S_SetLnb22KhzTone( nChannelId, pstSatTuneParam->b22kTone );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlSwitch] ERR!	CH_S_SetLnb22KhzTone() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE, ("Success : CH_S_SetLnb22KhzTone\n"));
	}
#endif

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_UnLockReentrant();
#endif

	UnLockControlMsg();

	return nResult;

}


int CH_S_ControlScd( int nChannelId, CH_SAT_ScdCmd_e etScdCmd, CH_SAT_TuneParam_t *pstSatTuneParam )
{
	int nRet;
	int nResult = DI_CH_OK;
	HUINT32 ulUserBand;
	HUINT32 ulUserbandFreq;
	HUINT32 ulTuneFreq;
	int nMsgLength = 0;
	static HBOOL s_bFirstTime = TRUE;
	HUINT32 ulMsDelay;

	LockControlMsg();

	if( s_bFirstTime == TRUE )
	{
		GetScdRandomDelay( pstSatTuneParam->ulScdUserBandNum );
		GetScdRandomDelay( pstSatTuneParam->ulScdUserBandNum );
		s_bFirstTime = FALSE;
	}

	ulMsDelay = GetScdRandomDelay(pstSatTuneParam->ulScdUserBandNum);
	CH_DI_Print(CH_LOG_TUNE	, ("[%d] CH_S_ControlScd ulMsDelay (%d) \n", nChannelId, ulMsDelay));
	VK_TASK_Sleep(ulMsDelay);

	ulUserBand 		= pstSatTuneParam->ulScdUserBandNum;
	ulUserbandFreq 	= pstSatTuneParam->ulScdUserBandFreq;
	ulTuneFreq 		= pstSatTuneParam->ulFrequency;

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_LockReentrant();
#endif

	//0. Tone off and set tone burst
	nRet = CH_S_SetToneBurst(nChannelId, FALSE, DI_CH_DISEQC_INPUT_NONE, pstSatTuneParam->etAntennaType, pstSatTuneParam->etDiseqcVersion);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlScd] ERR!  CH_S_SetToneBurst() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE	, ("Success : CH_S_SetToneBurst()\n"));
	}

	//1. Set polarization VER
	nRet = CH_S_SetLnbPolarisation( nChannelId, DI_CH_POLAR_VER, pstSatTuneParam->etLnbVoltage );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlScd] ERR! CH_S_SetLnbPolarisation() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE	, ("Success : CH_S_SetLnbPolarisation to VER...\n"));
	}

	//2. Set polarization VER -> HOR
	nRet = CH_S_SetLnbPolarisation( nChannelId, DI_CH_POLAR_HOR, pstSatTuneParam->etLnbVoltage );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlScd] ERR! CH_S_SetLnbPolarisation() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE	, ("Success : CH_S_SetLnbPolarisation to HOR...\n"));
	}

	//3. Make SCD msg
	nMsgLength = MakeScdMsg( etScdCmd, ulUserBand, ulUserbandFreq, ulTuneFreq, \
									pstSatTuneParam->etDiseqcInput, \
									pstSatTuneParam->etPolarization, \
									pstSatTuneParam->b22kTone);

	//4. Send DiSEqC msg
	nRet = CH_S_SendDiseqcMsg( nChannelId, s_aucScdMsg, nMsgLength, DI_CH_DISEQC_INPUT_NONE );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlScd] ERR! Cannot send disecq msg \n"));
		nResult = DI_CH_ERR;
	}

#if defined(CONFIG_PLATFORM_DB4506)
	VK_TASK_Sleep( 65 + 20 );
#else
	//VK_TASK_Sleep( 20 );// 2 < delay ms <60
	VK_TASK_Sleep( 65 + 20 + 30 );// 2 < delay ms <60
#endif

	//5. Set polarization VER
	nRet = CH_S_SetLnbPolarisation( nChannelId, DI_CH_POLAR_VER, pstSatTuneParam->etLnbVoltage );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlScd] ERR! CH_S_SetLnbPolarisation() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE	, ("Success : CH_S_SetLnbPolarisation to VER...\n"));
	}

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_UnLockReentrant();
#endif

	UnLockControlMsg();

	return nResult;

}



int CH_S_ControlMotor( int nChannelId, HUINT8 *pucData, HUINT8 ucLength )
{
	int nRet;
	int nResult = DI_CH_OK;
	CH_SAT_TuneParam_t * pstSatTuneParam;

	LockControlMsg();

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_LockReentrant();
#endif

	pstSatTuneParam = (CH_SAT_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

#if 0//TODO_ILEE : defined (PLATFORM_DB7335)
	if(nChannelId == 0)
	{
		/* Delay for Connection Detection Issue */
		VK_TASK_Sleep(50);
	}
#endif

	//0. Tone off and set tone burst
	nRet = CH_S_SetToneBurst(nChannelId, FALSE, DI_CH_DISEQC_INPUT_NONE, pstSatTuneParam->etAntennaType, pstSatTuneParam->etDiseqcVersion);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlMotor] ERR!  CH_S_SetToneBurst() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE, ("Success : CH_S_SetToneBurst()\n"));
	}

	//1. Send DiSEqC msg
	nRet = CH_S_SendDiseqcMsg( nChannelId, pucData, ucLength, DI_CH_DISEQC_INPUT_NONE );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlMotor] ERR! Cannot send disecq msg \n"));
		nResult = DI_CH_ERR;
	}

	VK_TASK_Sleep( 20 );//Delay over 15ms to enable 22Khz tone after ToneBurst signal
#if (BCHP_CHIP ==  7325) //More delay on 7325
	VK_TASK_Sleep( 80 );
#endif

	//2. Enable 22KHz tone if required
	nRet = CH_S_SetLnb22KhzTone( nChannelId, pstSatTuneParam->b22kTone );
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_ControlMotor] ERR! CH_S_SetLnb22KhzTone() nRet(%d) \n", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE, ("Success : CH_S_SetLnb22KhzTone\n"));
	}

#if defined(CONFIG_PLATFORM_DB4506)
	DRV_CH_UnLockReentrant();
#endif

	UnLockControlMsg();

	return nResult;

}


void CH_S_CheckStatus ( int nChannelId, HUINT32 unWorkingId )
{

	HUINT8	unitId;
	CH_Status_e		etStatusOfChannel;
	HBOOL	bLocked = FALSE;
	int nAntennaState;
#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
	int nRfInputId=0;
#endif

	//CH_DI_Print("Enter ...\n");

	unitId = (HUINT8)GetDeviceUnitId( nChannelId );
	etStatusOfChannel = GetTuneStatus( nChannelId );

	CH_DI_Print(CH_LOG_TUNE, ("[%d, %d] Current status (%d)\n", unitId , nChannelId,  etStatusOfChannel));
#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
	nRfInputId = GetRfInputId( nChannelId );
	CH_DI_Print(CH_LOG_TUNE, ("	ChannelId(%d), nRfInputId(%d) \n", nChannelId, nRfInputId));
	nAntennaState = LNB_CheckAntState( nRfInputId );
#else
	nAntennaState = LNB_CheckAntState( nChannelId );
#endif
	//CH_DI_Print("nAntennaState(%d)", nAntennaState);
	if( nAntennaState == DI_CH_ANTENNA_FAULT )
	{
		CH_DI_Print(CH_LOG_TUNE, ("DI_CH_ANTENNA_FAULT : nAntennaState (%d)\n", nAntennaState));
		SetTuneStatus( nChannelId, CH_STATUS_ANT_FAULT );
		etStatusOfChannel = GetTuneStatus( nChannelId );
		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT );
	}
	else if( nAntennaState == DI_CH_ERR )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_CheckStatus] ERR! I2C read err...\n"));
	}
	else
	{
		//Do nothing...
	}

	switch ( etStatusOfChannel )
	{
		case CH_STATUS_IDLE:
			break;

		case CH_STATUS_LOCKING:
			break;

		case CH_STATUS_LOCKED:
			CH_DI_Print(CH_LOG_TUNE, (" >>> CH_STATUS_LOCKED [%d] \n",nChannelId));

			bLocked = CH_S_CheckLock ( nChannelId );
			if( bLocked != TRUE )
			{
				SetTuneStatus( nChannelId, CH_STATUS_UNLOCK );
				DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_UNLOCK );
			}
			else
			{
				//do nothing...
				//DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_LOCKED );

				// check tuned status ..
				//CH_S_GetSignalStatus(nChannelId, &ber, &rfagc, &ifagc, &uncorrected, &power);
			}

			break;

		case CH_STATUS_CANCLE:
			break;

		case CH_STATUS_UNLOCK:
			CH_S_SetTune( nChannelId, unWorkingId );
			break;

		case CH_STATUS_ANT_FAULT:
			CH_S_ProcessRecoverAntenna( nChannelId, unWorkingId );
			break;

		default:
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_CheckStatus] Not Matched. etStatusOfChannel(%d)", etStatusOfChannel));
			break;

	}

	return;

}



/**
 * Process for auto search mode.
 *
 * \param[in]	nChannelId			identity for channel
 *
 * \param[in]	ulWorkingId			sync for process
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     First CH_S_SetTune set auto search mode and this func control params.
 *				Finally, clear auto search mode here.
 *
 * \see         CH_S_SetTune
 */

void CH_S_ProcessAutoSearch( int nChannelId, HUINT32 ulWorkingId )
{
	HUINT32			ulSearchMode;
	CH_S_AutoMode_e etSearchType = 0;
	CH_SAT_TuneParam_t * pstSatTuneParam;

	pstSatTuneParam = (CH_SAT_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	ulSearchMode = GetSearchMode( nChannelId );

	if( ulSearchMode & CH_S_POLAR )
	{
		etSearchType = CH_S_POLAR;
	}
	else if( ulSearchMode & CH_S_CODERATE )
	{
		etSearchType = CH_S_CODERATE;
	}
	else if( ulSearchMode & CH_S_MODULATION )
	{
		etSearchType = CH_S_MODULATION;
	}
	else if( ulSearchMode & CH_S_PILOT )
	{
		etSearchType = CH_S_PILOT;
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE, ("[CH_S_ProcessAutoSearch] Auto Search is finished!!! \n"));
	}

	switch( etSearchType )
	{
		case CH_S_POLAR:
			if( pstSatTuneParam->etPolarization == DI_CH_POLAR_HOR )
			{
				pstSatTuneParam->etPolarization = DI_CH_POLAR_VER;
				SetSearchMode( nChannelId, ( ulSearchMode & (~CH_S_POLAR) ) );
			}
			else
			{
				CH_DI_Print(CH_LOG_TUNE, ("[CH_S_ProcessAutoSearch] Already finished!!! \n"));
			}

			break;

		case CH_S_CODERATE:
			/* Current not used */
			if( pstSatTuneParam->etFecCodeRate == DI_CH_SCODERATE_1_2 )
			{
				pstSatTuneParam->etFecCodeRate += 1;

				if( (pstSatTuneParam->etTransSpec == DI_CH_TRANS_DVBS) \
					&&( pstSatTuneParam->etFecCodeRate == DI_CH_SCODERATE_7_8 ) )
				{
					SetSearchMode( nChannelId, ( ulSearchMode & (~CH_S_CODERATE) ) );
				}
				else
				{
					/* TODO_ILEE : for DVBS2 */
				}
			}
			else
			{
				CH_DI_Print(CH_LOG_TUNE, ("[CH_S_ProcessAutoSearch] Already finished!!!\n"));
			}

			break;

		case CH_S_MODULATION:
			break;

		case CH_S_PILOT:
			break;

		default:
			CH_DI_Print(CH_LOG_DEFAULT, ( "[CH_S_ProcessAutoSearch] Not Matched. etSearchType(%d) \n", etSearchType ));
			break;
	}

	CH_S_SetTune( nChannelId, ulWorkingId );

}



int CH_S_SetInputMode( int nChannelId, CH_SAT_InputMode_e etInputMode )
{
	int nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

    if ( NULL == NIM_SetInputMode )
    {
		return DI_CH_NOT_SUPPORTED;
    }
    else
    {
		nRet = NIM_SetInputMode( nChannelId, etInputMode );
		if( nRet != DI_CH_OK )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetInputMode] ERR!  NIM_SetInputMode() nRet(%d) \n", nRet));
			nResult = DI_CH_ERR;
		}
    }

	return nResult;
}



int CH_S_SetInputSource( int nChannelId, DI_CH_InputSource_e etInputSource )
{
	int nRet = DI_CH_OK;
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

    if ( NULL == NIM_SetInputSource )
    {
		return DI_CH_NOT_SUPPORTED;
    }
    else
    {
		nRet = NIM_SetInputSource( nChannelId, etInputSource );
		if( nRet != DI_CH_OK )
		{
			CH_DI_Print(CH_LOG_DEFAULT, ("[CH_S_SetInputSource] ERR!  NIM_SetInputSource() nRet(%d) \n", nRet));
		}
    }

	return nRet;
}


int CH_S_GetDeviceStatus(int nChannelId,DI_CH_DeviceStatus * pstDeviceStatus)
{
	int nResult = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	if(NIM_GetDeviceStatus == NULL)
	{
		DRV_CH_UnLock( nChannelId );
		return DI_CH_NOT_SUPPORTED;
	}
	else
	{
		nResult = NIM_GetDeviceStatus( nChannelId, pstDeviceStatus);
	}

	DRV_CH_UnLock( nChannelId );

	return nResult;
}


/*********************** End of File ******************************/

