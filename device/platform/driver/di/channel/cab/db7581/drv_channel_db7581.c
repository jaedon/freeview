 /*
 * (c) 2011-2014 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
 
 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  
 
 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
 
/******************************************************************************
File Name   : drv_channel_db7581.c
Original Author: Full Name!!
Description : 
Remarks:
******************************************************************************/

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_c.h"
#include "di_channel_attr.h"

#include "drv_err.h"
//#include "drv_i2c.h"
//#include "drv_gpio.h"

#include "drv_channel_c.h"
#include "drv_channel_db7581.h"
#include "nexus_frontend.h"
#include "nexus_platform.h"

#include <string.h>

#include "bkni.h"

/****************************************************
	Define
******************************************************/
#define NUM_OF_CAB_CHANNEL	(CONFIG_NUM_OF_CAB_CHANNEL)

#define	WAIT_QAM_LOCKING		(100) //(50)//(100)
#define	LOCK_TRIAL_COUNT		(10) // (4)


//#define CH_PRINT_LOW_LEVEL_ZAPPING_TIME

/****************************************************
	Extern  Variable
******************************************************/
extern HINT32 show_di_ch;

/****************************************************
	Groval Variable
******************************************************/
typedef struct
{
	HINT32			nStrength;			/**< Signal strength percentage Unit */
	HULONG			ulQuality;			/**< Signal quality percentage Unit */
	HUINT8			ucAgc;				/**< AGC */
	HULONG			ulBer;				/**< BER */
	HULONG			ulSnr;				/**< SNR */
#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
          HUINT32           ulSnrLevel;          /**< SNR percentage Unit */
          HUINT32           ulBerLevel;          /**< SNR percentage Unit */
#endif
	HUINT8			ucRfAgc;			/**< RFAGC */
	HUINT8			ucIfAgc;			/**< IFAGC */
	HUINT32			ulCorrectedNo;		/**< ulCorrectedNo */
	HUINT32			ulUnCorrectedNo;	/**< ulUnCorrectedNo */
	HUINT32			ulUnErroredNo;		/**< ulUnErroredNo */
	HFLOAT32		fSignalPower;		/**< Signal strength dBm or dBmV or dBuV */
#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
      HFLOAT32        fSignalPowerdBm;       /**< Signal strength dBm or dBmV or dBuV */
#endif
	BKNI_EventHandle	hStatusEvent;
}DB7581_SignalQuality_t;


/****************************************************
	Static Variable
******************************************************/
static NEXUS_FrontendHandle s_st7581Frontend[NUM_OF_CAB_CHANNEL];
static NEXUS_FrontendQamSettings s_stCableSettings[NUM_OF_CAB_CHANNEL];

static HUINT8	s_ucIsDb7581Lock[NUM_OF_CAB_CHANNEL];
static DB7581_SignalQuality_t s_stDb7581SignalStatus[NUM_OF_CAB_CHANNEL];
static BKNI_EventHandle lockChangedEvent[NUM_OF_CAB_CHANNEL];


#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
HUINT32 g_ulRequestTune[NUM_OF_CAB_CHANNEL];
HUINT32 g_ulLowLevelLockedTime[NUM_OF_CAB_CHANNEL] , g_ulLowLevelUnlockedTime[NUM_OF_CAB_CHANNEL] ;
#endif


/****************************************************
	Extern Function
******************************************************/
static void async_status_ready_callback(void *context, int param)
{
    CH_UNUSED(param);

    CH_DI_Print(CH_LOG_SIGINFO, ("async_status_ready_callback\n"));
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void lock_callback(void *context, int param)
{
    #if 0
    int nUnitId = 0;
    NEXUS_FrontendHandle hFrontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendQamStatus QamStatus;

    CH_DI_Print(CH_LOG_DEFAULT, ("lock_callback is called...with context [0x%x]\n", hFrontend));

    CH_UNUSED(param);

    if( lockChangedEvent[0] == hFrontend )
    {
        nUnitId = 0;
        NEXUS_Frontend_GetQamAsyncStatus(s_st7581Frontend[nUnitId], &QamStatus);
        CH_DI_Print(CH_LOG_DEFAULT, (">>>>> QAM lock_callback[%d] : QAM Lock callback: receiverLock(%d), fecLock(%d)\n",nUnitId, QamStatus.receiverLock, QamStatus.fecLock));
    }
    else
    {
        CH_DI_Print(CH_LOG_DEFAULT, (" Frontend handle Err![0x%x][0x%x]!\n", s_st7581Frontend[0], hFrontend));
    }
    #else
    CH_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
    #endif
}


#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
static HUINT32 P_ConvertStrengthLevel(HINT32 nChannelId, HFLOAT32 fRfInputLevel)
{
	HFLOAT32 fStrength = 0;
	HINT32 nTmpRfLevel = 0;
	HINT32 i;
	nTmpRfLevel = fRfInputLevel * 100.0;

	CH_DI_Print(CH_LOG_SIGINFO, ("fRfInputLevel [%f] \n", fRfInputLevel));

	/*
		Strength table
		dBuV	/ %
		x > 92 		/ 100
		87 < x < 92	/ 95
		82 < x < 87	/ 90
		77 < x < 82	/ 85
		72 < x < 77	/ 80
		67 < x < 72	/ 75
		62 < x < 67	/ 70
		57 < x < 62	/ 65
		52 < x < 57	/ 60
		47 < x < 52	/ 50
		42 < x < 47	/ 40
		32 < x < 42	/ 30
		27 < x < 32	/ 20
		17 < x < 27	/ 10
		x < 17		/ 0
	*/
#if 0
	if (nTmpRfLevel >= 1490)
	{
		fStrength = 100;
	}
	else if (nTmpRfLevel >= 1190)
	{
		fStrength = 90 + (10.0 * (nTmpRfLevel - 1190)/(1490 - 1190));
	}
	else if (nTmpRfLevel >= 7200)
	{
		fStrength = 80 + (10.0 * (nTmpRfLevel - 7200)/(8200 - 7200));
	}
	else if (nTmpRfLevel >= 6200)
	{
		fStrength = 70 + (10.0 * (nTmpRfLevel - 6200)/(7200 - 6200));
	}
	else if (nTmpRfLevel >= 5200)
	{
		fStrength = 60 + (10.0 * (nTmpRfLevel - 5200)/(6200 - 5200));
	}
	else if (nTmpRfLevel >= 4700)
	{
		fStrength = 50 + (10.0 * (nTmpRfLevel - 4700)/(5200 - 4700));
	}
       else if (nTmpRfLevel >= 4200)
	{
		fStrength = 40 + (10.0 * (nTmpRfLevel - 4200)/(4700 - 4200));
	}
       else if (nTmpRfLevel >= 3200)
	{
		fStrength = 30 + (10.0 * (nTmpRfLevel - 3200)/(4200 - 3200));
	}
       else if (nTmpRfLevel >= 2700)
	{
		fStrength = 20 + (10.0 * (nTmpRfLevel - 2700)/(3200 - 2700));
	}
       else if (nTmpRfLevel >= 1700)
	{
		fStrength = 10 + (10.0 * (nTmpRfLevel - 1700)/(2700 - 1700));
	}
	else
	{
		fStrength = 0;
	}
#else
	for (i=0;i<100;i++)
	{
		if (nTmpRfLevel < (-1480 + 30*i))
			break;
	}
	
	fStrength = i;

#endif
	CH_DI_Print(CH_LOG_SIGINFO, ("[%s] : fStrength [%d] = %d\n",__FUNCTION__, nChannelId, (HUINT32)fStrength));

	return (HUINT32)fStrength;
}

#else
static HUINT32 P_ConvertStrengthLevel(HINT32 nChannelId, HINT32 nRfInputLevel)
{
	HINT32 nStrength = 0;
	HINT32 nTmpRfLevel = 0;

	nTmpRfLevel = (HINT32)(nRfInputLevel * 100);

      CH_DI_Print(CH_LOG_SIGINFO, (" P_ConvertStrengthLevel] RfInputLevel[%d][%d] -------------\n", nRfInputLevel, nTmpRfLevel));

	/*
		Strength table
		dBuV	/ %
		x > 92 		/ 100
		82 < x < 92	/ 90
		72 < x < 82	/ 80
		62 < x < 72	/ 70
		57 < x < 62	/ 60
		52 < x < 57	/ 50
		47 < x < 52	/ 40
		42 < x < 47	/ 30
		37 < x < 42	/ 20
		32 < x < 37	/ 10
		x < 32		/ 0
	*/

	if (nTmpRfLevel >= 12600)
	{
		nStrength = 100;
	}
	else if (nTmpRfLevel >= 11100)
	{
		nStrength = 90 + (10 * (nTmpRfLevel - 11100)/(12600 - 11100));
	}
	else if (nTmpRfLevel >= 8700)
	{
		nStrength = 80 + (10 * (nTmpRfLevel - 8700)/(11100 - 8700));
	}
	else if (nTmpRfLevel >= 6100)
	{
		nStrength = 70 + (10 * (nTmpRfLevel - 6100)/(8700 - 6100));
	}
	else if (nTmpRfLevel >= 5100)
	{
		nStrength = 60 + (10 * (nTmpRfLevel - 5100)/(6100 - 5100));
	}
	else if (nTmpRfLevel >= 4300)
	{
		nStrength = 50 + (10 * (nTmpRfLevel - 4300)/(5100 - 4300));
	}
	else if (nTmpRfLevel >= 3300)
	{
		nStrength = 40 + (10 * (nTmpRfLevel - 3300)/(4300 - 3300));
	}
	else if (nTmpRfLevel >= 2300)
	{
		nStrength = 30 + (10 * (nTmpRfLevel - 2300)/(3300 - 2300));
	}
	else if (nTmpRfLevel >= 1300)
	{
		nStrength = 20 + (10 * (nTmpRfLevel - 1300)/(2300 - 1300));
	}
	else if (nTmpRfLevel >= 300)
	{
		nStrength = 10 + (10 * (nTmpRfLevel - 300)/(1300 - 300));
	}
	else
	{
		nStrength = 0;
	}

	CH_DI_Print(CH_LOG_SIGINFO, ("[%s] : nStrength [%d] = %d\n",__FUNCTION__, nChannelId, (HUINT32)nStrength));

	return (HUINT32)nStrength;
}

#endif

static HUINT32 P_ConvertQualityLevel(HINT32 nChannelId, CH_CAB_Modulation_e  etModulation, HUINT32 ulSnr)
{
    HINT32 ulQuality = 0;

    
    CH_DI_Print(CH_LOG_SIGINFO, (" P_ConvertQualityLevel] etModulation[%d] snrEstimate[%d] -------------\n", etModulation, ulSnr));

    if( etModulation == DI_CH_CMOD_256QAM )
    {
        ulQuality = ulSnr/10 - 200;     // SNR >= 30 -> 100%
    }
    else if( etModulation == DI_CH_CMOD_128QAM )
    {
        ulQuality = ulSnr/10 - 165;     // SNR >= 27 -> 100%
    }
    else if( etModulation == DI_CH_CMOD_64QAM )
    {
        ulQuality = ulSnr/10 - 130;      // SNR >= 24 -> 100%
    }
    else if( etModulation == DI_CH_CMOD_32QAM )
    {
        ulQuality = ulSnr/10 - 90;      // SNR >= 20 -> 100%
    }
    else if( etModulation == DI_CH_CMOD_16QAM )
    {
        ulQuality = ulSnr/10 - 70;      // SNR >= 17 -> 100%
    }
    else
         ulQuality = 0;
    

	if (ulQuality >= 100)
	{
		ulQuality = 100;
	}
	else if (ulQuality <= 0)
	{
		ulQuality = 0;
	}
      CH_DI_Print(CH_LOG_SIGINFO, ("[%s] [%d] fQuality [%d] \n",__FUNCTION__,nChannelId, ulQuality));

    return (HUINT32)ulQuality;
}


#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
static HUINT32 P_ConvertBERLevel(HINT32 nChannelId, HUINT32 ulBer)
{
    HINT32 ulBerLevel = 0;
    HINT32 i;
    HINT32 j = 0;

    
    CH_DI_Print(CH_LOG_SIGINFO, (" P_ConvertSNRLevel] ulBer[%d] -------------\n", ulBer));

if (ulBer >= 100000) //E-03
{
	if(( i == 3) || ( i == 7) || ( i == 10) || ( i == 13))
		j++;
	
	for (i=0;i<100;i++)
	{
		if (ulBer >= (970000 - (60000*i +10000*j)))
			break;
	}
	ulBerLevel =  i ;
}
else if (ulBer >= 99000) //E-04
{
	ulBerLevel = 14;
}
else if (ulBer >= 93000) //E-04
{
	ulBerLevel = 15;
}
else if (ulBer >= 10000) //E-04
{
	if(( i == 2) || ( i == 5) || ( i == 8) || ( i == 12))
		j++;

	for (i=0;i<100;i++)
	{
		if (ulBer >= (93000 - (6000*i +1000*j)))
			break;
	}
	ulBerLevel = 15 + i ;
}
else if (ulBer >= 9500) //E-05
{
	ulBerLevel = 29;
}
else if (ulBer >= 1000) //E-05
{
	if(( i == 1) || ( i == 4) || ( i == 8) || ( i == 11))
		j++;
	
	for (i=0;i<100;i++)
	{
		if (ulBer < (9500 - (600*i +100*j)))
			break;
	}
	ulBerLevel =  29 + i ;
}
else if (ulBer >= 960) //E-06
{
	ulBerLevel = 43;
}
else if (ulBer >= 100) //E-06
{
	if(( i == 4) || ( i == 7) || ( i == 10) )
		j++;
	
	for (i=0;i<100;i++)
	{
		if (ulBer < (960 - (60*i +10*j)))
			break;
	}
	ulBerLevel =  43 + i ;
}
else if (ulBer >= 98) //E-07
{
	ulBerLevel = 57;
}
else if (ulBer >= 10) //E-07
{
	if(( i == 3) || ( i == 6) || ( i == 9) || ( i == 13) )
		j++;
	
	for (i=0;i<100;i++)
	{
		if (ulBer < (98 - (6*i +j)))
			break;
	}
	ulBerLevel =  57 + i ;
}
else 
{
	ulBerLevel =  100 ;
}

    CH_DI_Print(CH_LOG_SIGINFO, ("[%s] [%d] ulBerLevel [%d] \n",__FUNCTION__,nChannelId, ulBerLevel));

    return (HUINT32)ulBerLevel;
}


static HUINT32 P_ConvertSNRLevel(HINT32 nChannelId, HUINT32 ulSnr)
{
    HINT32 ulSnrLevel = 0;
    HINT32 i;
    HINT32 j = 0;

    CH_DI_Print(CH_LOG_SIGINFO, (" P_ConvertQualityLevel] snrEstimate[%d] -------------\n", ulSnr));

	for (i=0;i<100;i++)
	{
		if( i ==  (3 + 4*j))
			j++;
			
		if (ulSnr >= (4000 - 50*i))
			break;
	}

	ulSnrLevel = 100 - i - j;
	

    CH_DI_Print(CH_LOG_SIGINFO, ("[%s] [%d] ulSnrLevel [%d] \n",__FUNCTION__,nChannelId, ulSnrLevel));

    return (HUINT32)ulSnrLevel;
}
#endif

static HBOOL WaitQamLocked(int nChannelId)
{
    NEXUS_FrontendFastStatus status;
    HUINT8 usLock = FALSE;
    int nUnitId=0, nCount=0;
    NEXUS_Error rc;    

    nUnitId = GetDeviceUnitId( nChannelId );

    while(usLock != TRUE)
    {
        rc = NEXUS_Frontend_GetFastStatus(s_st7581Frontend[nUnitId] , &status);
        if (rc == NEXUS_SUCCESS) 
        { 
            if (status.lockStatus == NEXUS_FrontendLockStatus_eLocked) 
            {
                CH_DI_Print(CH_LOG_STATUS, ("frontend locked \n"));
            usLock = TRUE;
            }
            else if (status.lockStatus == NEXUS_FrontendLockStatus_eUnlocked) 
            {
                CH_DI_Print(CH_LOG_STATUS, ("frontend unlocked (acquireInProgress=%d) \n", status.acquireInProgress));
                /* Wait for  MAX_ACQUIRE_TIME when unlocked*/
            }
            else if (status.lockStatus == NEXUS_FrontendLockStatus_eNoSignal) 
            {                      
                CH_DI_Print(CH_LOG_STATUS, ("No signal at the tuned frequency. \n"));
            }
            else
                CH_DI_Print(CH_LOG_STATUS, ("No signal at the tuned frequency. Waiting till the application timesout to allow auto acquire to try again. \n"));
       }
        else
       {
            CH_DI_Print(CH_LOG_STATUS, ("unknown status: %d \n", status.lockStatus));
       }                

        if(usLock == TRUE)
        {
            s_ucIsDb7581Lock[nUnitId] = TRUE;
            CH_DI_Print(CH_LOG_DEFAULT, ("WaitQamLocked[%d] : Locked!(nCount:%d) lockStatus(%d)\n", nUnitId, nCount, status.lockStatus));
            
#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
            g_ulLowLevelLockedTime[nChannelId] = VK_TIMER_GetSystemTick();
            CH_DI_Print(CH_LOG_DEFAULT, ("[WaitQamLocked] Lock Time( %d )ms  \n" , g_ulLowLevelLockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
#endif
            return TRUE;
        }
        else
        {
            VK_TASK_Sleep(WAIT_QAM_LOCKING);
            if(nCount == LOCK_TRIAL_COUNT)
            {
                s_ucIsDb7581Lock[nUnitId] = FALSE;
                CH_DI_Print(CH_LOG_STATUS, ("WaitQamLocked[%d] : Lock trial expired lockStatus(%d)\n", nUnitId, status.lockStatus));
#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
                g_ulLowLevelUnlockedTime[nChannelId] = VK_TIMER_GetSystemTick();
                CH_DI_Print(CH_LOG_DEFAULT, ("[WaitQamLocked] UnLock Time( %d )ms  \n",  g_ulLowLevelUnlockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
#endif
                return FALSE;
            }
            else
            {
                nCount++;
            }
        }
    }

    return TRUE;
}


#define __CH_C_PUBLIC_FUNCITONS__

void DRV_C_InstallApi( void )
{
    CAB_InitDevice 			= &DB7581_InitDevice;
    CAB_SetTune 			= &DB7581_SetTune;
    CAB_CheckLock 			= &DB7581_CheckLock;
    CAB_GetStrengthQuality 	= &DB7581_GetStrengthQuality;
    CAB_SetPowerState 		= &DB7581_SetPowerState;
    CAB_EnableTsOutput		= &DB7581_EnableTsOutput;
    CAB_DisableTsOutput		= &DB7581_DisableTsOutput;
    CAB_ResetGpio 			= &DB7581_ResetGpio;
    CAB_Initialize			= &DB7581_Initialize;
    CAB_ReInitDevice		= &DB7581_ReInitDevice;
    CAB_GetDeviceStatus         = &DB7581_GetDeviceStatus;

    CH_DI_Print(CH_LOG_ARGS, ("DRV_C_InstallApi()  OK! \n"));

    return;
}

int DB7581_InitDevice(int nChannelId)
{
    int nUnitId;
    HINT32 nRet = DI_CH_OK;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendAcquireSettings settings;

    nUnitId = GetDeviceUnitId( nChannelId );

    NEXUS_Platform_GetConfiguration(&platformConfig);
    if(platformConfig.frontend[nUnitId] != NULL)
    {
        s_st7581Frontend[nUnitId] = platformConfig.frontend[nUnitId];
    }
    else
    {
        nRet = NEXUS_Platform_InitFrontend();
        if(nRet == DI_CH_OK)
        {
            NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
            settings.capabilities.qam = true;
            s_st7581Frontend[nUnitId] = NEXUS_Frontend_Acquire(&settings);
        }        	
    }

    if (NULL == s_st7581Frontend[nUnitId] )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[DB7581_InitDevice] ERR! Unable to find CABLE capable frontend\n"));
        return DI_CH_ERR;
    }

    BKNI_CreateEvent(&(s_stDb7581SignalStatus[nChannelId].hStatusEvent));
    BKNI_CreateEvent(&lockChangedEvent[nChannelId]);

    return DI_CH_OK;
}


int  DB7581_ResetGpio(int nChannelId)
{
    HUINT32 	nUnitId = 0;

    CH_DI_Print(CH_LOG_TSINFO, ("[DB7581_ResetGpio[%d]] ----\n",nChannelId));

    nUnitId = GetDeviceUnitId( nChannelId );

    return DI_CH_OK;
}


int DB7581_SetTune(int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
    HINT32 nUnitId = 0, nRet = DI_CH_OK, nResult = DI_CH_OK;
    NEXUS_FrontendQamSettings qamSettings;

    CH_UNUSED(ulWorkingId);
    nUnitId = GetDeviceUnitId( nChannelId );

    CH_DI_Print(CH_LOG_DEFAULT,("DB7581_SetTune[%d][%dMHz]\n", nChannelId,  pstCabTuneParam->ulFrequency/1000));

    NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
    qamSettings.frequency = pstCabTuneParam->ulFrequency * KHZ_TO_HZ;
    
#if defined(CONFIG_SUPPORT_QAM_ANNEX_B)
    qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
    qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e6Mhz;
#else  // ANNEX_A
    qamSettings.annex = NEXUS_FrontendQamAnnex_eA;
    qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e8Mhz;
    qamSettings.frequencyOffset = NEXUS_FrontendQamFrequencyOffset_e250Khz;
#endif

#if defined(CONFIG_SUPPORT_QAM_ANNEX_B)
    switch(pstCabTuneParam->etModulation)
    {
        case DI_CH_CMOD_64QAM:
            qamSettings.mode = NEXUS_FrontendQamMode_e64;
            break;
        case DI_CH_CMOD_256QAM:
            qamSettings.mode = NEXUS_FrontendQamMode_e256;
            break;
        default:
            qamSettings.mode = NEXUS_FrontendQamMode_eAuto_64_256;
            break;
    }
#else
    switch(pstCabTuneParam->etModulation)
    {
        case DI_CH_CMOD_16QAM:
            qamSettings.mode = NEXUS_FrontendQamMode_e16;
            break;
        case DI_CH_CMOD_32QAM:
            qamSettings.mode = NEXUS_FrontendQamMode_e32;
            break;
        case DI_CH_CMOD_64QAM:
            qamSettings.mode = NEXUS_FrontendQamMode_e64;
            break;
        case DI_CH_CMOD_128QAM:
            qamSettings.mode = NEXUS_FrontendQamMode_e128;
            break;			
        case DI_CH_CMOD_256QAM:
            qamSettings.mode = NEXUS_FrontendQamMode_e256;
            break;
        default:
            qamSettings.mode = NEXUS_FrontendQamMode_eAuto_64_256;
            break;

    }
#endif

    qamSettings.symbolRate = pstCabTuneParam->ulSymbolRate * KHZ_TO_HZ;
    //qamSettings.autoAcquire = true; // use NEXUS to auto acquire if loss of lock
    qamSettings.autoAcquire = false;

    qamSettings.lockCallback.callback = lock_callback;
    qamSettings.lockCallback.context = lockChangedEvent[nUnitId];
    qamSettings.asyncStatusReadyCallback.callback = async_status_ready_callback;
    qamSettings.asyncStatusReadyCallback.context = s_stDb7581SignalStatus[nChannelId].hStatusEvent;

#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
            g_ulRequestTune[nUnitId] = VK_TIMER_GetSystemTick();
#endif

    nRet = NEXUS_Frontend_TuneQam(s_st7581Frontend[nUnitId], &qamSettings);
    if( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_DEFAULT,("DB7581_SetTune[%d] :: NEXUS_Frontend_TuneQam.. Fail.. nRet(0x%x)", nUnitId, nRet));
        nResult = DI_CH_ERR;
    }

    nRet = WaitQamLocked( nChannelId );
    if( nRet == TRUE )
    {
        CH_DI_Print(CH_LOG_DEFAULT,("\n\n\n-------------------------------------------------------\n"));
        CH_DI_Print(CH_LOG_DEFAULT,("DB7581_SetTune[%d] :: Locked \n",nUnitId));
        CH_DI_Print(CH_LOG_DEFAULT,("---------------------------------------------------------\n\n\n"));
        s_ucIsDb7581Lock[nUnitId] = TRUE;
        nResult = DI_CH_OK;
    }
    else
    {
        CH_DI_Print(CH_LOG_DEFAULT,("DB7581_SetTune[%d] :: Unlocked \n",nUnitId));
        s_ucIsDb7581Lock[nUnitId] = FALSE;
        nResult = DI_CH_ERR;
    }

    return nResult;
}


HBOOL DB7581_CheckLock(int nChannelId)
{
    HINT32 nUnitId = 0;
    NEXUS_Error nRet;
    NEXUS_FrontendFastStatus status;

    nUnitId = GetDeviceUnitId( nChannelId );

    nRet = NEXUS_Frontend_GetFastStatus(s_st7581Frontend[nUnitId], &status);
    if( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("Error : NEXUS_Frontend_GetFastStatus(0x%x)", nRet));
    }

    if(status.lockStatus == NEXUS_FrontendLockStatus_eLocked)
    {
        CH_DI_Print(CH_LOG_STATUS,  ("**********************************************************************\n"));
        CH_DI_Print(CH_LOG_STATUS,  (" status.lockStatus 					   [%d], [%d]:nUnitId\n", status.lockStatus, nUnitId));
        CH_DI_Print(CH_LOG_STATUS,  ("**********************************************************************\n"));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


int  DB7581_GetStrengthQuality(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{

    HINT32 nUnitId = 0;
    NEXUS_FrontendQamStatus stQamStatus;    
#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
    HFLOAT32    fSignalPowerdBmV = 0;
#else
    HFLOAT32    fSignalPowerdBuV = 0;
#endif
    HFLOAT32    fPreRsBer = 0;
    HFLOAT32    fPostRsBer = 0;
    HINT32  nQuality=0, nStrength=0;
#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
    HINT32  nSNR=0, nBER=0;
#endif
    HINT32 	nSignalPower = 0;
    CH_CAB_TuneParam_t *pstCabTuneParam;

    nUnitId = GetDeviceUnitId( nChannelId );

#if !defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
    if(s_ucIsDb7581Lock[nUnitId])
    {
#endif        
        pstCabTuneParam = (CH_CAB_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

        NEXUS_Frontend_GetQamStatus(s_st7581Frontend[nUnitId], &stQamStatus );

#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
        fSignalPowerdBmV = (stQamStatus.dsChannelPower/10.0) + 47.0 + 2.5 ;
#else   // cdig-1000c
        fSignalPowerdBuV = (stQamStatus.dsChannelPower/256.0) + 107.0;
#endif        
        nSignalPower = (stQamStatus.dsChannelPower + 787)/5;
        
#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
        nStrength = P_ConvertStrengthLevel(nChannelId,fSignalPowerdBmV);
        nSNR = P_ConvertSNRLevel(nChannelId, stQamStatus.snrEstimate);
        nBER = P_ConvertBERLevel(nChannelId, stQamStatus.viterbiErrorRate);
#else        
        nStrength = P_ConvertStrengthLevel(nChannelId,nSignalPower);
#endif
        nQuality = P_ConvertQualityLevel(nChannelId, pstCabTuneParam->etModulation, stQamStatus.snrEstimate);
      
        pstSignalInfo->ulStrength = s_stDb7581SignalStatus[nChannelId].nStrength = nStrength;
        pstSignalInfo->ulQuality 	 = s_stDb7581SignalStatus[nChannelId].ulQuality = nQuality;
        pstSignalInfo->ulAgc 	 = s_stDb7581SignalStatus[nChannelId].ucAgc = 0; //ignore

        //pstSignalInfo->fBer 		 = s_stDb7581SignalStatus[nChannelId].ulBer = stQamStatus.postRsBer / 1000000.0;
        pstSignalInfo->fBer          = s_stDb7581SignalStatus[nChannelId].ulBer = stQamStatus.viterbiErrorRate;
#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO) //JAMES..151121...fSnr must be float type
        s_stDb7581SignalStatus[nChannelId].ulSnr = stQamStatus.snrEstimate / 100.0;
	pstSignalInfo->fSnr 		 = (float)stQamStatus.snrEstimate / 100.0;
#else
        pstSignalInfo->fSnr 		 = s_stDb7581SignalStatus[nChannelId].ulSnr = (float)stQamStatus.snrEstimate / 100.0;
#endif
        pstSignalInfo->ulRfAgc 	 = s_stDb7581SignalStatus[nChannelId].ucRfAgc = stQamStatus.rfAgcLevel;
        pstSignalInfo->ulIfAgc 	 = s_stDb7581SignalStatus[nChannelId].ucIfAgc = stQamStatus.ifAgcLevel;

        pstSignalInfo->ulCorrectedNo 		= s_stDb7581SignalStatus[nChannelId].ulCorrectedNo = stQamStatus.fecCorrected;
        pstSignalInfo->ulUnCorrectedNo 	= s_stDb7581SignalStatus[nChannelId].ulUnCorrectedNo = stQamStatus.fecUncorrected;
        pstSignalInfo->ulUnErroredNo 		= s_stDb7581SignalStatus[nChannelId].ulUnErroredNo= stQamStatus.goodRsBlockCount;
#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
        pstSignalInfo->fSignalInputPowerdBm    = s_stDb7581SignalStatus[nChannelId].fSignalPowerdBm = (stQamStatus.dsChannelPower/10.0 + 4.0);    //dBm
        pstSignalInfo->ulSnrLevel               = s_stDb7581SignalStatus[nChannelId].ulSnrLevel = nSNR;
        pstSignalInfo->ulBerLevel               = s_stDb7581SignalStatus[nChannelId].ulBerLevel = nBER;
        pstSignalInfo->fSignalInputPower	= s_stDb7581SignalStatus[nChannelId].fSignalPower = fSignalPowerdBmV;
#else
        pstSignalInfo->fSignalInputPower	= s_stDb7581SignalStatus[nChannelId].fSignalPower = fSignalPowerdBuV;
#endif

#if 0
        CH_DI_Print(0,("  Signal Power	: SDK[%d]	DI[%2.2f]	UI[%d]\n", stQamStatus.dsChannelPower, pstSignalInfo->fSignalInputPower, pstSignalInfo->ulStrength));
        CH_DI_Print(0,("  SNR		 : SDK[%d]	DI[%2.2f]	UI[%d]\n", stQamStatus.snrEstimate, pstSignalInfo->fSnr, pstSignalInfo->ulSnrLevel ));
        CH_DI_Print(0,("  BER		 : SDK[%d]	DI[%2.2f]	UI[%d]\n", stQamStatus.viterbiErrorRate, pstSignalInfo->fBer, pstSignalInfo->ulBerLevel ));
        CH_DI_Print(0,("  \n\n" ));
#endif



        CH_DI_Print(CH_LOG_SIGINFO,("====================== DB7581 Signal Status[%d] ======================\n",nChannelId));
        CH_DI_Print(CH_LOG_SIGINFO,("  DB7581_CheckLock : Locked \n"));
        CH_DI_Print(CH_LOG_SIGINFO,("  Annex_%s\n", stQamStatus.settings.annex?"B":"A"));
        CH_DI_Print(CH_LOG_SIGINFO,("  receiverLock             = %u\n", stQamStatus.receiverLock));
        CH_DI_Print(CH_LOG_SIGINFO,("  fecLock                  = %u\n", stQamStatus.fecLock));
        CH_DI_Print(CH_LOG_SIGINFO,("  opllLock                 = %u\n", stQamStatus.opllLock));
        CH_DI_Print(CH_LOG_SIGINFO,("  spectrumInverted         = %u\n", stQamStatus.spectrumInverted));

        CH_DI_Print(CH_LOG_SIGINFO,("  symbolRate               = %u [Baud]\n", stQamStatus.symbolRate));
        CH_DI_Print(CH_LOG_SIGINFO,("  symbolRateError          = %d [Baud]\n", stQamStatus.symbolRateError));
        CH_DI_Print(CH_LOG_SIGINFO,("  berEstimate              = %d\n", stQamStatus.berEstimate));

        CH_DI_Print(CH_LOG_SIGINFO,("  ifAgcLevel               = %2.1f [%%]\n", stQamStatus.ifAgcLevel/10.0));
        CH_DI_Print(CH_LOG_SIGINFO,("  rfAgcLevel               = %2.1f [%%]\n", stQamStatus.rfAgcLevel/10.0));
        CH_DI_Print(CH_LOG_SIGINFO,("  intAgcLevel              = %2.1f [%%]\n", stQamStatus.intAgcLevel/10.0));
        CH_DI_Print(CH_LOG_SIGINFO,("  snrEstimate              = %2.2f [dB]\n", stQamStatus.snrEstimate/100.0));

        CH_DI_Print(CH_LOG_SIGINFO,("  ulStrength               = %d\n", pstSignalInfo->ulStrength));
        CH_DI_Print(CH_LOG_SIGINFO,("  ulQuality                = %d\n", pstSignalInfo->ulQuality));

        CH_DI_Print(CH_LOG_SIGINFO,("  fecCorrected             = %u\n", stQamStatus.fecCorrected));
        CH_DI_Print(CH_LOG_SIGINFO,("  fecUncorrected           = %u\n", stQamStatus.fecUncorrected));
        CH_DI_Print(CH_LOG_SIGINFO,("  fecClean                 = %u\n", stQamStatus.fecClean));
        CH_DI_Print(CH_LOG_SIGINFO,("  bitErrCorrected          = %u\n", stQamStatus.bitErrCorrected));
        CH_DI_Print(CH_LOG_SIGINFO,("  reacquireCount           = %u\n", stQamStatus.reacquireCount));

//        CH_DI_Print(CH_LOG_SIGINFO,("  fPreRsBer                 = %3.2e\n", fPreRsBer));
//        CH_DI_Print(CH_LOG_SIGINFO,("  fPostRsBer                = %3.2e\n", fPostRsBer));

        CH_DI_Print(CH_LOG_SIGINFO,("  viterbiUncorrectedBits   = %u\n", stQamStatus.viterbiUncorrectedBits));
        CH_DI_Print(CH_LOG_SIGINFO,("  viterbiTotalBits         = %u\n", stQamStatus.viterbiTotalBits));
        CH_DI_Print(CH_LOG_SIGINFO,("  viterbiErrorRate         = %3.2e\n", stQamStatus.viterbiErrorRate));
#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
        CH_DI_Print(CH_LOG_SIGINFO,("  SNR Level         = %d\n", pstSignalInfo->ulSnrLevel));  
        CH_DI_Print(CH_LOG_SIGINFO,("  BER Level         = %d\n", pstSignalInfo->ulBerLevel));  
#endif
        CH_DI_Print(CH_LOG_SIGINFO,("  carrierFreqOffset        = %.3f [Hz]\n", stQamStatus.carrierFreqOffset/1000.0));
        CH_DI_Print(CH_LOG_SIGINFO,("  carrierPhaseOffset       = %.3f [Hz]\n", stQamStatus.carrierPhaseOffset/1000.0));

        CH_DI_Print(CH_LOG_SIGINFO,("  goodRsBlockCount         = %u\n", stQamStatus.goodRsBlockCount));
        CH_DI_Print(CH_LOG_SIGINFO,("  berRawCount              = %u\n", stQamStatus.berRawCount));
#if defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
        CH_DI_Print(CH_LOG_SIGINFO,("  dsChannelPower           = %3.2f [dBm] %3.2f [dBuV]\n", (stQamStatus.dsChannelPower/10.0 + 4.0), fSignalPowerdBmV));
#else
        CH_DI_Print(CH_LOG_SIGINFO,("  dsChannelPower           = %3.2f [dBuV]\n", fSignalPowerdBuV));
#endif
        CH_DI_Print(CH_LOG_SIGINFO,("  mainTap                  = %u\n", stQamStatus.mainTap));
        CH_DI_Print(CH_LOG_SIGINFO,("  equalizerGain            = %u [dBm]\n", stQamStatus.equalizerGain));
        CH_DI_Print(CH_LOG_SIGINFO,("  postRsBer                = %u\n", stQamStatus.postRsBer));
        CH_DI_Print(CH_LOG_SIGINFO,("  postRsBerElapsedTime     = %u [s]\n", stQamStatus.postRsBerElapsedTime));
        CH_DI_Print(CH_LOG_SIGINFO,("  interleaveDepth          = %u\n", stQamStatus.interleaveDepth));
        CH_DI_Print(CH_LOG_SIGINFO,("  lnaAgcLevel              = %2.1f [%%]\n", stQamStatus.lnaAgcLevel/10.0));
        CH_DI_Print(CH_LOG_SIGINFO,("=================================================================\n"));
        
#if !defined(CONFIG_REALTIME_GETTING_SIGNALINFO)
    }
    else
    {
        pstSignalInfo->ulStrength			= 0;
        pstSignalInfo->ulQuality			= 0;
        pstSignalInfo->ulAgc				= 0;		// Ignore
        pstSignalInfo->fBer					= 0;
        pstSignalInfo->fSnr					= 0;
        pstSignalInfo->ulCorrectedNo		= 0;
        pstSignalInfo->ulUnCorrectedNo		= 0;
        pstSignalInfo->ulUnErroredNo		= 0;
        CH_DI_Print(CH_LOG_DEFAULT,("DB7581_GetStrengthQuality[%d] : Unlocked \n",nChannelId));
    }
#endif

    return DI_CH_OK;
}


int  DB7581_SetPowerState( int nChannelId, CH_Power_e etPower )
{
    int nUnitId;
    int nResult = DI_CH_OK;

    CH_DI_Print(CH_LOG_ARGS, ("[DB7581_SetPowerState[%d]] ----\n",nChannelId));

    nUnitId = GetDeviceUnitId( nChannelId );

    switch (etPower)
    {
        case CH_POWER_OFF:  //tmPowerStandby = 1
            SetActiveStatus( nChannelId, CH_ACTION_OFF );
            CH_DI_Print(CH_LOG_ARGS, ("\n -------------CH_C_SetPowerState standby mode -------------\n"));
            break;

        case CH_POWER_ON: //tmPowerOn = 0
            SetActiveStatus( nChannelId, CH_ACTION_ON );
            CH_DI_Print(CH_LOG_ARGS, ("\n -------------CH_C_SetPowerState Power on mode -------------\n"));
            break;
    }

    return nResult;
}


int DB7581_EnableTsOutput ( int nChannelId )
{
	int nUnitId;

	CH_DI_Print(CH_LOG_TSINFO, ("[DB7581_DisableTsOutput[%d]] ----\n",nChannelId));

	nUnitId = GetDeviceUnitId( nChannelId );

	return DI_CH_OK;
}

int DB7581_DisableTsOutput ( int nChannelId )
{
	int nUnitId;
    
	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_TSINFO, ("[DB7581_DisableTsOutput[%d]] ----\n",nChannelId));
       NEXUS_Frontend_Untune(s_st7581Frontend[nUnitId]);

	return DI_CH_OK;
}


int DB7581_Initialize ( int nChannelId )
{
	/* Don't need antthing*/
	return DI_CH_OK;
}


int DB7581_ReInitDevice ( int nChannelId )
{
    int nUnitId;
    HINT32 nRet = DI_CH_OK;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendAcquireSettings settings;

    nUnitId = GetDeviceUnitId( nChannelId );

    NEXUS_Platform_GetConfiguration(&platformConfig);
    if(platformConfig.frontend[nUnitId] != NULL)
    {
        s_st7581Frontend[nUnitId] = platformConfig.frontend[nUnitId];
    }
    else
    {
        nRet = NEXUS_Platform_InitFrontend();
        if(nRet == DI_CH_OK)
        {
            NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
            //settings.mode = NEXUS_FrontendAcquireMode_eByIndex;
            settings.capabilities.qam = true;
            //settings.index = nUnitId;
            s_st7581Frontend[nUnitId] = NEXUS_Frontend_Acquire(&settings);
        }           
    }

    if (NULL == s_st7581Frontend[nUnitId] )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[DB7581_InitDevice] ERR! Unable to find CABLE capable frontend\n"));
        return DI_CH_ERR;
    }

    BKNI_CreateEvent(&(s_stDb7581SignalStatus[nChannelId].hStatusEvent));

#if 0 // QamAsyncStatus  ????????????
    s_stCableSettings[nUnitId].lockCallback.callback = lock_callback;
    s_stCableSettings[nUnitId].lockCallback.context = s_st7581Frontend[nUnitId];
#endif        

    return DI_CH_OK;
}


int DB7581_GetDeviceStatus ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus )
{
    int nUnitId = 0;
    NEXUS_Error nRet = NEXUS_SUCCESS;
    NEXUS_FrontendDeviceStatus stDevStatus;
    NEXUS_FrontendDeviceHandle deviceHandle = NULL;

    nUnitId = GetDeviceUnitId( nChannelId );

    deviceHandle = NEXUS_Frontend_GetDevice(s_st7581Frontend[nUnitId]);
    if(deviceHandle == NULL)
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("Error : NEXUS_Frontend_GetDevice() deviceHandle is null... \n"));
        return DI_CH_ERR;
    }

    nRet = NEXUS_FrontendDevice_GetStatus(deviceHandle, &stDevStatus);
    if(nRet != NEXUS_SUCCESS)
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("Error : NEXUS_FrontendDevice_GetStatus() nRet =[0x%x] \n",nRet));
        return DI_CH_ERR;
    }

    CH_DI_Print(CH_LOG_SIGINFO, (">>>>>>>>>>>> DB7581_GetDeviceStatus !!!!!\n"));
    CH_DI_Print(CH_LOG_SIGINFO, ("avs.enabled : [%d] \n",stDevStatus.avs.enabled));
    CH_DI_Print(CH_LOG_SIGINFO, ("avs.voltage : [%d] \n",stDevStatus.avs.voltage));
    CH_DI_Print(CH_LOG_SIGINFO, ("temperature : [%d] \n",stDevStatus.temperature));
    CH_DI_Print(CH_LOG_SIGINFO, (">>>>>>>>>>>> DB7581_GetDeviceStatus !!!!!\n"));
    pstDeviceStatus->avs.enabled = stDevStatus.avs.enabled;
    pstDeviceStatus->avs.voltage = stDevStatus.avs.voltage;
    pstDeviceStatus->temperature = stDevStatus.temperature;

    return DI_CH_OK;

}


/*EOF*/
