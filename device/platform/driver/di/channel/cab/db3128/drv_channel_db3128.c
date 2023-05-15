/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:		drv_channel_db3128.c
// Original Author: Full Name!!
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2014 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/

/* chip headers */
#include "string.h"
#include "htype.h"
#include "vkernel.h"

/* di headers */
#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_c.h"
#include "di_channel_attr.h"

#include "drv_err.h"
//#include "drv_i2c.h"
#include "drv_gpio.h"

/* drv headers */
#include "drv_channel.h"
#include "drv_channel_c.h"
#include "drv_channel_db3128.h"

/* nexus headers */
#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_frontend_3128.h"
#include "nexus_input_band.h"



/****************************************************
	Define
******************************************************/
#define MASTER_CHANNEL			(0)
#define NUM_OF_CAB_CHANNEL	(CONFIG_NUM_OF_CAB_CHANNEL)

#define   FIRST_WAIT_TIME			(200)
#define   HAB_WAIT_TIME			(130)
#define	WAIT_QAM_LOCK		(150) //(50)//(100)
#define   MAX_REACQUIRE_COUNTS 	(5) //(30)//(2)//(10)//(4)

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
	HUINT8			ucRfAgc;			/**< RFAGC */
	HUINT8			ucIfAgc;			/**< IFAGC */
	HUINT32			ulCorrectedNo;		/**< ulCorrectedNo */
	HUINT32			ulUnCorrectedNo;	/**< ulUnCorrectedNo */
	HUINT32			ulUnErroredNo;		/**< ulUnErroredNo */
	HFLOAT32		fSignalPower;		/**< Signal strength dBm or dBmV or dBuV */
}DB3128_SignalQuality_t;

/****************************************************
	Static Variable
******************************************************/
static NEXUS_FrontendHandle s_st3128Frontend[NUM_OF_CHANNEL];
//static NEXUS_FrontendQamSettings s_stCableSettings[NUM_OF_CAB_CHANNEL];
static BKNI_EventHandle s_evStatusEvent[NUM_OF_CAB_CHANNEL];
static BKNI_EventHandle lockChangedEvent[NUM_OF_CAB_CHANNEL];

static DB3128_SignalQuality_t		s_stDb3128SignalStatus[NUM_OF_CHANNEL];
static HBOOL s_bInitDevice = false;
/****************************************************
	Extern Function
******************************************************/
extern	void DRV_CH_CallNotifyCallback ( int nChannelID, HUINT32 unRequestId, DI_CH_SignalInfo_e etSigalInfo );


static HUINT32 P_ConvertStrengthLevel(HINT32 nChannelId, HFLOAT32 fRfInputLevel)
{
	HFLOAT32 fStrength = 0;
	HINT32 nTmpRfLevel = 0;

	nTmpRfLevel = fRfInputLevel * 100.0;

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

	if (nTmpRfLevel >= 9200)
	{
		fStrength = 100;
	}
	else if (nTmpRfLevel >= 8200)
	{
		fStrength = 90 + (10.0 * (nTmpRfLevel - 8200)/(9200 - 8200));
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

	CH_DI_Print(CH_LOG_SIGINFO, ("[%s] : fStrength [%d] = %d\n",__FUNCTION__, nChannelId, (HUINT32)fStrength));

	return (HUINT32)fStrength;
}



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


    if (ulQuality > 100)
	ulQuality = 100;
    else if (ulQuality < 0)
        ulQuality = 0;

    CH_DI_Print(CH_LOG_SIGINFO, ("[%s] [%d] fQuality [%d] \n",__FUNCTION__,nChannelId, ulQuality));

    return (HUINT32)ulQuality;
}


static void lock_callback(void *context, int param)
{
#if 0
    int nUnitId = 0;
    NEXUS_FrontendHandle hFrontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendQamStatus QamStatus;

    CH_DI_Print( CH_LOG_ARGS, ("lock_callback is called...with context [0x%x]\n", hFrontend));

    if( lockChangedEvent[0] == hFrontend )
    {
        CH_DI_Print(CH_LOG_3, ("Master channel!!!\n"));
        nUnitId = 0;
    }
    else if( lockChangedEvent[1] == hFrontend )
    {
        CH_DI_Print(CH_LOG_3, ("Slave channel!!!\n"));
        nUnitId = 1;
    }
    else if( lockChangedEvent[2] == hFrontend )
    {
        CH_DI_Print(CH_LOG_3, ("Slave channel!!!\n"));
        nUnitId = 2;
    }
    else
    {
        CH_DI_Print(CH_LOG_3, ("Slave channel!!!\n"));
        nUnitId = 3;
    }

    CH_UNUSED(param);
    NEXUS_Frontend_GetQamAsyncStatus(s_st3128Frontend[nUnitId], &QamStatus);
    CH_DI_Print(CH_LOG_0, (">>>>> QAM lock_callback[%d] : QAM Lock callback: receiverLock(%d), fecLock(%d)\n",nUnitId, QamStatus.receiverLock, QamStatus.fecLock));
#else
    CH_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);

#endif

}

static void async_status_ready_callback(void *context, int param)
{
    CH_UNUSED(param);

    CH_DI_Print( CH_LOG_ARGS, ("async_status_ready_callback\n"));
    BKNI_SetEvent((BKNI_EventHandle)context);
}


static HBOOL WaitQamLocked(int nChannelId)
{
    HUINT8 usLock = FALSE;
    int nUnitId=0, nLoopCount=0;
    CH_Status_e etCurrentState;
    NEXUS_FrontendFastStatus status;
    NEXUS_Error rc;

    nUnitId = GetDeviceUnitId( nChannelId );

    VK_TASK_Sleep(FIRST_WAIT_TIME);//(200);//(100);//first AGC time

    while(usLock != TRUE)
    {
        etCurrentState = GetTuneStatus(nChannelId);
        if( ( etCurrentState == CH_STATUS_IDLE) || (etCurrentState == CH_STATUS_CANCLE) )
        {
            CH_DI_Print(CH_LOG_ARGS, ("Cancel locking status..."));
            return FALSE;
        }

		if(s_bInitDevice == false)
		{
			return FALSE;
		}
        rc = NEXUS_Frontend_GetFastStatus(s_st3128Frontend[nUnitId], &status);
        if (rc == NEXUS_SUCCESS)
        {
            CH_DI_Print(CH_LOG_STATUS,  ("**********************************************************************\n"));
            CH_DI_Print(CH_LOG_STATUS, ("NEXUS_Frontend_GetFastStatus: nLoopCount=[%d]\n", nLoopCount));
            CH_DI_Print(CH_LOG_STATUS, ("NEXUS_Frontend_GetFastStatus: lockStatus=[%d]\n", status.lockStatus));
            CH_DI_Print(CH_LOG_STATUS, ("NEXUS_Frontend_GetFastStatus: acquireInProgress=[%d]\n", status.acquireInProgress));

            if( status.lockStatus == NEXUS_FrontendLockStatus_eLocked)
            {
                CH_DI_Print(CH_LOG_STATUS, ("NEXUS_Frontend_GetFastStatus: NEXUS_FrontendLockStatus_eLocked \n"));
                CH_DI_Print(CH_LOG_STATUS,  ("**********************************************************************\n"));
                return TRUE;
            }
            else if(status.lockStatus == NEXUS_FrontendLockStatus_eNoSignal)
            {
                CH_DI_Print(CH_LOG_STATUS, ("NEXUS_Frontend_GetFastStatus: NEXUS_FrontendLockStatus_eNoSignal \n"));
                CH_DI_Print(CH_LOG_STATUS,  ("**********************************************************************\n"));
            }
            else if(status.lockStatus == NEXUS_FrontendLockStatus_eUnlocked)
            {
                CH_DI_Print(CH_LOG_STATUS, ("NEXUS_Frontend_GetFastStatus: NEXUS_FrontendLockStatus_eUnlocked \n"));
            }
            else
            {
                CH_DI_Print(CH_LOG_STATUS, ("NEXUS_Frontend_GetFastStatus: NEXUS_FrontendLockStatus_eUnknown... Try one more... \n"));
            }
            CH_DI_Print(CH_LOG_STATUS,  ("**********************************************************************\n"));
        }
        else
        {
            CH_DI_Print(CH_LOG_DEFAULT, ("Error : 0x%x", rc));
        }

        if( nLoopCount >= MAX_REACQUIRE_COUNTS )
        {
            CH_DI_Print(CH_LOG_STATUS, ("########## nLoopCount=[%d] ########## \n", nLoopCount));
            return FALSE;
        }
        VK_TASK_Sleep(WAIT_QAM_LOCK);
        nLoopCount++;
    }

    return FALSE;
}


int ResetChannelHw( int nChannelId )
{
    CH_DI_Print(CH_LOG_ARGS, ("ResetChannelHw ...%d\n",nChannelId));
    if( nChannelId == MASTER_CHANNEL )
    {
        DRV_GPIO_Write(GPIO_ID_CH_RST_N, GPIO_LOW);
        //DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_LOW);
        VK_TASK_Sleep(25);//min 25ms
        DRV_GPIO_Write(GPIO_ID_CH_RST_N, GPIO_HI);
        //DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_HI);
        VK_TASK_Sleep(210);//min 210ms
    }
    return 0;
}

#define __CH_C_PUBLIC_FUNCITONS__

int DB3128_ReInitDevice(int nChannelId)
{
    int nUnitId=0, i=0;

    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendHandle frontend[NUM_OF_CAB_CHANNEL]={NULL,};
    NEXUS_FrontendDeviceHandle frontendDevice[NUM_OF_CAB_CHANNEL]={NULL,};
    NEXUS_FrontendDeviceStatus frontendDeviceStatus[NUM_OF_CAB_CHANNEL];


    CH_DI_Print(CH_LOG_ARGS, ("DB3128_ReInitDevice[%d] ...S\n", nChannelId));

    nUnitId = GetDeviceUnitId( nChannelId );

     if(nChannelId == MASTER_CHANNEL)
    {
        //ResetChannelHw(nUnitId);
        NEXUS_Platform_OpenFrontend(nUnitId);

        for ( i = 0; i < NUM_OF_CAB_CHANNEL; i++ )
        {
            NEXUS_FrontendAcquireSettings settings;
            NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
            settings.capabilities.qam = true;
            frontend[i] = NEXUS_Frontend_Acquire(&settings);
            if (!frontend[i])
            {
                 CH_DI_Print(CH_LOG_DEFAULT, ("Unable to find CAB-capable frontend\n"));
                 return -1;
            }
        }

        frontendDevice[0] = NEXUS_Frontend_GetDevice(frontend[0]);
        frontendDevice[1] = NEXUS_Frontend_GetDevice(frontend[1]);
        frontendDevice[2] = NEXUS_Frontend_GetDevice(frontend[2]);
        frontendDevice[3] = NEXUS_Frontend_GetDevice(frontend[3]);

        while(1)
        {
            NEXUS_FrontendDevice_GetStatus(frontendDevice[0],&frontendDeviceStatus[0]);
            NEXUS_FrontendDevice_GetStatus(frontendDevice[1],&frontendDeviceStatus[1]);
            NEXUS_FrontendDevice_GetStatus(frontendDevice[2],&frontendDeviceStatus[2]);
            NEXUS_FrontendDevice_GetStatus(frontendDevice[3],&frontendDeviceStatus[3]);
            if((frontendDeviceStatus[0].openFailed == false && frontendDeviceStatus[0].openPending == false)
                && (frontendDeviceStatus[1].openFailed == false && frontendDeviceStatus[1].openPending == false)
                && (frontendDeviceStatus[2].openFailed == false && frontendDeviceStatus[2].openPending == false)
                && (frontendDeviceStatus[3].openFailed == false && frontendDeviceStatus[3].openPending == false)
            )
            {
                CH_DI_Print(CH_LOG_DEFAULT, (" frontend open is okay!! \n"));
				s_bInitDevice = true;
                break;
            }
            VK_TASK_Sleep(1000);
        }
    }

    NEXUS_Platform_GetConfiguration(&platformConfig);
    s_st3128Frontend[nUnitId] = platformConfig.frontend[nUnitId];

    if (NULL == s_st3128Frontend[nUnitId] )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[DB3128_InitDevice] ERR! Unable to find CABLE capable frontend\n"));
        return DI_CH_ERR;
    }
    else
    {
          CH_DI_Print(CH_LOG_DEFAULT, ("Find CABLE-capable frontend s_st3128Frontend[%d]=[0x%x] \n", nUnitId, s_st3128Frontend[nUnitId]));
    }

    BKNI_CreateEvent(&s_evStatusEvent[nUnitId]);
    BKNI_CreateEvent(&lockChangedEvent[nUnitId]);

    return DI_CH_OK;
}


int DB3128_UnInitDevice( int nChannelId )
{
    int nUnitId=0;
#if 0
	int  i=0;
#endif
	s_bInitDevice = false;

    nUnitId = GetDeviceUnitId( nChannelId );

    SetTuneStatus( nChannelId, CH_STATUS_IDLE );

    if(nChannelId == MASTER_CHANNEL)
    {
	#if 0
        for ( i = 0; i < NUM_OF_CAB_CHANNEL; i++ )
        {
		CH_DI_Print(CH_LOG_DEFAULT, (" DB3461_UnInitDevice] NEXUS_Frontend_Untune [%d] !!!!!\n", i));
		NEXUS_Frontend_Untune(s_st3128Frontend[i]);
		VK_TASK_Sleep(HAB_WAIT_TIME);
        }
	#endif
		CH_DI_Print(CH_LOG_DEFAULT, ("[DB3461] DB3461_UnInitDevice(%d) NEXUS_Platform_UninitFrontend !!\n", nUnitId));
        NEXUS_Platform_UninitFrontend();
		VK_TASK_Sleep(HAB_WAIT_TIME);
    }

    return DI_CH_OK;
}


void DRV_C_InstallApi( void )
{
    CAB_InitDevice 			= &DB3128_InitDevice;
    CAB_SetTune 			= &DB3128_SetTune;
    CAB_CheckLock 			= &DB3128_CheckLock;
    CAB_GetStrengthQuality 	= &DB3128_GetStrengthQuality;
    CAB_SetPowerState 		= &DB3128_SetPowerState;
    CAB_EnableTsOutput 		= &DB3128_EnableTsOutput;
    CAB_DisableTsOutput 	= &DB3128_DisableTsOutput;
    //      CAB_UpdateTunedParam	= &DB3128_UpdateTunedParam;
    //CAB_GetChannelStatus	= &DB3128_GetChannelStatus;
    CAB_ResetGpio = &ResetChannelHw;
    CAB_ReInitDevice	= &DB3128_ReInitDevice;
    CAB_UnInitDevice =  &DB3128_UnInitDevice;
    CAB_GetDeviceStatus = &DB3128_GetDeviceStatus;

    return;
}


int DB3128_InitDevice(int nChannelId)
{
    int nUnitId=0, i=0;

    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendHandle frontend[NUM_OF_CAB_CHANNEL]={NULL,};
    NEXUS_FrontendDeviceHandle frontendDevice[NUM_OF_CAB_CHANNEL]={NULL,};
    NEXUS_FrontendDeviceStatus frontendDeviceStatus[NUM_OF_CAB_CHANNEL];


    CH_DI_Print(CH_LOG_ARGS, ("DB3128_InitDevice[%d] ...S\n", nChannelId));

    nUnitId = GetDeviceUnitId( nChannelId );

    if(nChannelId == MASTER_CHANNEL)
    {
        //ResetChannelHw(nUnitId);
        NEXUS_Platform_OpenFrontend(nUnitId);

        for ( i = 0; i < NUM_OF_CAB_CHANNEL; i++ )
        {
            NEXUS_FrontendAcquireSettings settings;
            NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
            settings.capabilities.qam = true;
            frontend[i] = NEXUS_Frontend_Acquire(&settings);
            if (!frontend[i])
            {
                 CH_DI_Print(CH_LOG_DEFAULT, ("Unable to find CAB-capable frontend\n"));
                 return -1;
            }
        }

        frontendDevice[0] = NEXUS_Frontend_GetDevice(frontend[0]);
        frontendDevice[1] = NEXUS_Frontend_GetDevice(frontend[1]);
        frontendDevice[2] = NEXUS_Frontend_GetDevice(frontend[2]);
        frontendDevice[3] = NEXUS_Frontend_GetDevice(frontend[3]);

        while(1)
        {
            NEXUS_FrontendDevice_GetStatus(frontendDevice[0],&frontendDeviceStatus[0]);
            NEXUS_FrontendDevice_GetStatus(frontendDevice[1],&frontendDeviceStatus[1]);
            NEXUS_FrontendDevice_GetStatus(frontendDevice[2],&frontendDeviceStatus[2]);
            NEXUS_FrontendDevice_GetStatus(frontendDevice[3],&frontendDeviceStatus[3]);
            if((frontendDeviceStatus[0].openFailed == false && frontendDeviceStatus[0].openPending == false)
                && (frontendDeviceStatus[1].openFailed == false && frontendDeviceStatus[1].openPending == false)
                && (frontendDeviceStatus[2].openFailed == false && frontendDeviceStatus[2].openPending == false)
                && (frontendDeviceStatus[3].openFailed == false && frontendDeviceStatus[3].openPending == false)
            )
            {
                CH_DI_Print(CH_LOG_DEFAULT, (" frontend open is okay!! \n"));
				s_bInitDevice = true;
                break;
            }
            VK_TASK_Sleep(1000);
        }
    }

    NEXUS_Platform_GetConfiguration(&platformConfig);
    s_st3128Frontend[nUnitId] = platformConfig.frontend[nUnitId];

    if (NULL == s_st3128Frontend[nUnitId] )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("[DB3128_InitDevice] ERR! Unable to find CABLE capable frontend\n"));
        return DI_CH_ERR;
    }
    else
    {
    	  CH_DI_Print(CH_LOG_DEFAULT, ("Find CABLE-capable frontend s_st3128Frontend[%d]=[0x%x] \n", nUnitId, s_st3128Frontend[nUnitId]));
    }

    BKNI_CreateEvent(&s_evStatusEvent[nUnitId]);
    BKNI_CreateEvent(&lockChangedEvent[nUnitId]);

    return DI_CH_OK;
}


int DB3128_SetTune(int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
    HINT32 nUnitId = 0, nResult = DI_CH_OK;
    int nIsQamLocked = 0;
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_Error rc;

    CH_UNUSED(ulWorkingId);
	if(s_bInitDevice == false)
	{
		return DI_CH_NOT_SUPPORTED;
	}

    DRV_CH_LockReentrant();
    CH_DI_Print(CH_LOG_ARGS,("DB3128_SetTune[%d]\n",nChannelId));

    nUnitId = GetDeviceUnitId( nChannelId );

    NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
    qamSettings.frequency = pstCabTuneParam->ulFrequency * KHZ_TO_HZ;
    qamSettings.symbolRate = pstCabTuneParam->ulSymbolRate * KHZ_TO_HZ;
    qamSettings.annex = NEXUS_FrontendQamAnnex_eA;
    qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e8Mhz;

    qamSettings.acquisitionMode = NEXUS_FrontendQamAcquisitionMode_eScan; //NEXUS_FrontendQamAcquisitionMode_eAuto;
	/* spectrumMode : spectrum normal or inverted */
	qamSettings.spectrumMode = NEXUS_FrontendQamSpectrumMode_eAuto;

	if((qamSettings.acquisitionMode == NEXUS_FrontendQamAcquisitionMode_eScan)||(qamSettings.acquisitionMode == NEXUS_FrontendQamAcquisitionMode_eAuto))
	{
		qamSettings.scan.upperBaudSearch = qamSettings.symbolRate + KHZ_TO_HZ;
		qamSettings.scan.lowerBaudSearch = qamSettings.symbolRate - KHZ_TO_HZ;
	}

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
        qamSettings.symbolRate = pstCabTuneParam->ulSymbolRate * KHZ_TO_HZ;
        break;

    }

    qamSettings.autoAcquire = true; // use NEXUS to auto acquire if loss of lock
    qamSettings.lockCallback.callback = lock_callback;
    qamSettings.lockCallback.context = lockChangedEvent[nUnitId];
    qamSettings.asyncStatusReadyCallback.callback = async_status_ready_callback;
    qamSettings.asyncStatusReadyCallback.context = s_evStatusEvent[nUnitId];

    rc = NEXUS_Frontend_TuneQam(s_st3128Frontend[nUnitId], &qamSettings);
    if( rc != NEXUS_SUCCESS )
    {
        CH_DI_Print(CH_LOG_DEFAULT,("DB3128_SetTune[%d] :: NEXUS_Frontend_TuneQam.. Fail.. nRet(0x%x)", nUnitId, rc));
        nResult = DI_CH_ERR;
    }

    nIsQamLocked = WaitQamLocked( nChannelId );
    if( nIsQamLocked == TRUE )
    {
        //CH_DI_Print(CH_LOG_2,("DB3128_SetTune[%d]... carrierFreqOffset=%d, ifAgcLevel=%d, rfAgcLevel=%d\n",nUnitId, stQamStatus.carrierFreqOffset, stQamStatus.ifAgcLevel, stQamStatus.rfAgcLevel));
        CH_DI_Print(CH_LOG_DEFAULT,("DB3128_SetTune[%d] :: Locked \n",nUnitId));
        nResult = DI_CH_OK;
    }
    else
    {
        CH_DI_Print(CH_LOG_DEFAULT,("DB3128_SetTune[%d] :: Unlocked \n",nUnitId));
        nResult = DI_CH_ERR;
    }

	CH_DI_Print(CH_LOG_ARGS,("DB3128_SetTune[%d].. End \n",nUnitId));
	DRV_CH_UnLockReentrant();

    return nResult;
}

HBOOL DB3128_CheckLock(int nChannelId)
{

    int nUnitId = 0;
    NEXUS_Error rc;
    NEXUS_FrontendFastStatus status;

	if(s_bInitDevice == false)
	{
		return DI_CH_NOT_SUPPORTED;
	}

    nUnitId = GetDeviceUnitId( nChannelId );

    rc = NEXUS_Frontend_GetFastStatus(s_st3128Frontend[nUnitId], &status);
    if( rc != NEXUS_SUCCESS )
    {
        CH_DI_Print(CH_LOG_DEFAULT, ("Error : NEXUS_Frontend_GetFastStatus(0x%x)", rc));
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


int  DB3128_GetStrengthQuality(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
    HINT32 nUnitId = 0;
    int rc=0;
    NEXUS_FrontendQamStatus stQamStatus;
    HFLOAT32    fPreRsBer = 0;
    HFLOAT32    fPostRsBer = 0;
    HFLOAT32    fSignalPowerdBuV = 0;
	HFLOAT32 	fBerRateUnit = 0;
    CH_CAB_TuneParam_t *pstCabTuneParam;

	if(s_bInitDevice == false)
	{
		return DI_CH_NOT_SUPPORTED;
	}

    nUnitId = GetDeviceUnitId( nChannelId );

    DRV_CH_LockReentrant();

    rc = NEXUS_Frontend_RequestQamAsyncStatus(s_st3128Frontend[nUnitId]);
    if(rc == NEXUS_SUCCESS)
    {
        rc = BKNI_WaitForEvent( s_evStatusEvent[nUnitId], 1000);
        if (rc) {
            CH_DI_Print(CH_LOG_DEFAULT,("DB3128_GetStrengthQuality] Status not returned [%x]\n",rc));
        }
        rc = NEXUS_Frontend_GetQamAsyncStatus(s_st3128Frontend[nUnitId] , &stQamStatus);
        if(rc != NEXUS_SUCCESS)
        {
            CH_DI_Print(CH_LOG_DEFAULT,("NEXUS_Frontend_GetQamAsyncStatus] Status not returned [%x]\n",rc));
			DRV_CH_UnLockReentrant();
            return DI_CH_ERR;
        }
    }
    else
    {
        CH_DI_Print(CH_LOG_DEFAULT,("NEXUS_Frontend_RequestQamAsyncStatus] Status not returned [%x]\n",rc));
	DRV_CH_UnLockReentrant();
        return DI_CH_ERR;
    }
    DRV_CH_UnLockReentrant();


    if(stQamStatus.fecLock)
   {
        pstCabTuneParam = (CH_CAB_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

        if(stQamStatus.settings.annex == NEXUS_FrontendQamAnnex_eB)
        {
            /* PreRsBer = (correctedBits * 128.0) / ((uncorrectedBlocks + correctedBlocks + cleanBlocks) * 7 * 122 * 127);*/
            /* PostRsBer = (uncorrectedBlocks * 11.224) / ((uncorrectedBlocks + correctedBlocks + cleanBlocks) * 7 * 122);*/
            fPreRsBer = (stQamStatus.bitErrCorrected * 128.0) / ((stQamStatus.fecUncorrected + stQamStatus.fecCorrected + stQamStatus.fecClean) * 7 * 122 * 127);
            fPostRsBer = (stQamStatus.fecUncorrected * 11.224) / ((stQamStatus.fecUncorrected + stQamStatus.fecCorrected + stQamStatus.fecClean) * 7 * 122);
        }
        else
        {
            /* PreRsBer = (correctedBits * 204.0) / ((uncorrectedBlocks + correctedBlocks + cleanBlocks) * 8 * 187 * 203);*/
            /* PostRsBer = (uncorrectedBlocks * 11.224) / ((uncorrectedBlocks + correctedBlocks + cleanBlocks) * 8 * 187);*/

            // fPreRsBer = (stQamStatus.bitErrCorrected * 204.0) / ((stQamStatus.fecUncorrected + stQamStatus.fecCorrected + stQamStatus.fecClean) * 8.0 * 187.0 * 203.0);
            fPreRsBer =  (stQamStatus.viterbiErrorRate * 204.0) / ((stQamStatus.viterbiTotalBits) * 8.0 * 187.0 * 203.0);
            fPostRsBer = (stQamStatus.fecUncorrected * 11.224) / ((stQamStatus.fecUncorrected + stQamStatus.fecCorrected + stQamStatus.fecClean) * 8.0 * 187.0);
        }

        // fSignalPowerdBuV = (stQamStatus.dsChannelPower/10.0 -48.0) + 107.0;
        fSignalPowerdBuV = (stQamStatus.dsChannelPower/10.0 -56.0) + 107.0;


        CH_DI_Print(CH_LOG_SIGINFO,("DB3128_CheckLock[%d] : Locked \n",nChannelId));
        CH_DI_Print(CH_LOG_SIGINFO,("====================== DB3128_CheckLock[%d] ======================\n",nChannelId));
        CH_DI_Print(CH_LOG_SIGINFO,("  Annex_%s\n", stQamStatus.settings.annex?"B":"A"));
        CH_DI_Print(CH_LOG_SIGINFO,("  receiverLock             = %u\n", stQamStatus.receiverLock));
        CH_DI_Print(CH_LOG_SIGINFO,("  fecLock                  = %u\n", stQamStatus.fecLock));
        CH_DI_Print(CH_LOG_SIGINFO,("  opllLock                 = %u\n", stQamStatus.opllLock));
        CH_DI_Print(CH_LOG_SIGINFO,("  spectrumInverted         = %u\n", stQamStatus.spectrumInverted));

        CH_DI_Print(CH_LOG_SIGINFO,("  symbolRate               = %u [Baud]\n", stQamStatus.symbolRate));
        CH_DI_Print(CH_LOG_SIGINFO,("  symbolRateError          = %d [Baud]\n", stQamStatus.symbolRateError));
        CH_DI_Print(CH_LOG_SIGINFO,("  berEstimate              = %d\n", stQamStatus.berEstimate));
        CH_DI_Print(CH_LOG_SIGINFO,("  snrEstimate              = %2.2f [dB]\n", stQamStatus.snrEstimate/100.0));

        CH_DI_Print(CH_LOG_SIGINFO,("  fecCorrected             = %u\n", stQamStatus.fecCorrected));
        CH_DI_Print(CH_LOG_SIGINFO,("  fecUncorrected           = %u\n", stQamStatus.fecUncorrected));
        CH_DI_Print(CH_LOG_SIGINFO,("  fecClean                 = %u\n", stQamStatus.fecClean));
        CH_DI_Print(CH_LOG_SIGINFO,("  bitErrCorrected          = %u\n", stQamStatus.bitErrCorrected));
        CH_DI_Print(CH_LOG_SIGINFO,("  reacquireCount           = %u\n", stQamStatus.reacquireCount));

        CH_DI_Print(CH_LOG_SIGINFO,("  PreRsBer                 = %3.2e\n", fPreRsBer));
        CH_DI_Print(CH_LOG_SIGINFO,("  PostRsBer                = %3.2e\n", fPostRsBer));

        CH_DI_Print(CH_LOG_SIGINFO,("  viterbiUncorrectedBits   = %u\n", stQamStatus.viterbiUncorrectedBits));
        CH_DI_Print(CH_LOG_SIGINFO,("  viterbiTotalBits         = %u\n", stQamStatus.viterbiTotalBits));
        CH_DI_Print(CH_LOG_SIGINFO,("  errorRateUnits           = %d\n", stQamStatus.errorRateUnits));
		CH_DI_Print(CH_LOG_SIGINFO,("  viterbiErrorRate         = %3.2e\n", stQamStatus.viterbiErrorRate));
		fBerRateUnit = (float)stQamStatus.viterbiErrorRate / 2147483648.0;
		CH_DI_Print(CH_LOG_SIGINFO,("  fBerRateUnit             = %3.2e\n", fBerRateUnit));
        CH_DI_Print(CH_LOG_SIGINFO,("  carrierFreqOffset        = %.3f [Hz]\n", stQamStatus.carrierFreqOffset/1000.0));
        CH_DI_Print(CH_LOG_SIGINFO,("  carrierPhaseOffset       = %.3f [Hz]\n", stQamStatus.carrierPhaseOffset/1000.0));

        CH_DI_Print(CH_LOG_SIGINFO,("  goodRsBlockCount         = %u\n", stQamStatus.goodRsBlockCount));
        CH_DI_Print(CH_LOG_SIGINFO,("  berRawCount              = %u\n", stQamStatus.berRawCount));
        CH_DI_Print(CH_LOG_SIGINFO,("  dsChannelPower           = %3.2f [dBm] %3.2f [dBuV]\n", (stQamStatus.dsChannelPower/10.0 -56.0), fSignalPowerdBuV));
        CH_DI_Print(CH_LOG_SIGINFO,("  mainTap                  = %u\n", stQamStatus.mainTap));
        CH_DI_Print(CH_LOG_SIGINFO,("  equalizerGain            = %u [dBm]\n", stQamStatus.equalizerGain));
        CH_DI_Print(CH_LOG_SIGINFO,("  postRsBer                = %u\n", stQamStatus.postRsBer));
        CH_DI_Print(CH_LOG_SIGINFO,("  postRsBerElapsedTime     = %u [s]\n", stQamStatus.postRsBerElapsedTime));
        CH_DI_Print(CH_LOG_SIGINFO,("  interleaveDepth          = %u\n", stQamStatus.interleaveDepth));
        CH_DI_Print(CH_LOG_SIGINFO,("  lnaAgcLevel              = %2.1f [%%]\n", stQamStatus.lnaAgcLevel/10.0));
        CH_DI_Print(CH_LOG_SIGINFO,("=================================================================\n"));

        /* TODO_JDCHO Strength & Qaulity */
        s_stDb3128SignalStatus[nChannelId].fSignalPower = fSignalPowerdBuV;
        s_stDb3128SignalStatus[nChannelId].ucAgc = 0;		// Ignore
        s_stDb3128SignalStatus[nChannelId].ulBer = stQamStatus.viterbiErrorRate;
        s_stDb3128SignalStatus[nChannelId].ulSnr = stQamStatus.snrEstimate;
        s_stDb3128SignalStatus[nChannelId].ucRfAgc = stQamStatus.rfAgcLevel;
        s_stDb3128SignalStatus[nChannelId].ucIfAgc = stQamStatus.ifAgcLevel;
        s_stDb3128SignalStatus[nChannelId].ulCorrectedNo = stQamStatus.fecCorrected;
        s_stDb3128SignalStatus[nChannelId].ulUnCorrectedNo = stQamStatus.fecUncorrected;
        s_stDb3128SignalStatus[nChannelId].ulUnErroredNo = stQamStatus.fecClean;
        s_stDb3128SignalStatus[nChannelId].nStrength = P_ConvertStrengthLevel(nChannelId,fSignalPowerdBuV);
        // s_stDb3128SignalStatus[nChannelId].ulQuality = P_ConvertQualityLevel(nChannelId,stQamStatus.snrEstimate);
        s_stDb3128SignalStatus[nChannelId].ulQuality = P_ConvertQualityLevel(nChannelId, pstCabTuneParam->etModulation, stQamStatus.snrEstimate);


        pstSignalInfo->ulStrength 			= s_stDb3128SignalStatus[nChannelId].nStrength;
        pstSignalInfo->fSignalInputPower        = s_stDb3128SignalStatus[nChannelId].fSignalPower;
        pstSignalInfo->ulQuality 			= s_stDb3128SignalStatus[nChannelId].ulQuality;
        pstSignalInfo->ulAgc 				= s_stDb3128SignalStatus[nChannelId].ucAgc;
		/* NEXUS_FrontendErrorRateUnits errorRateUnits;  units for viterbiErrorRate and postRsBer. Default is NEXUS_FrontendErrorRateUnits_eLinear. */
		/* ErrorRateUnits_eLinear : 1/2147483648 or 1/(2^31) units. Convert to floating point: float fBer = (float)ber / 2147483648.0 */
		/* ErrorRateUnits_eNaturalLog : (log(ber) * 1000000.0) units. Convert to floating point: float fBer = exp((float)ber/1000000.0) */
		//pstSignalInfo->fBer 				= s_stDb3128SignalStatus[nChannelId].ulBer;
		pstSignalInfo->fBer 				= fBerRateUnit;
        pstSignalInfo->fSnr 				= s_stDb3128SignalStatus[nChannelId].ulSnr/100.0;

        /* TODO_JDCHO - Activate when the latest DI is ported
        pstSignalInfo->ulRfAgc 				= s_stDb3128SignalStatus[nChannelId].ucRfAgc;
        pstSignalInfo->ulIfAgc 				= s_stDb3128SignalStatus[nChannelId].ucIfAgc;
        */
        pstSignalInfo->ulCorrectedNo 		= s_stDb3128SignalStatus[nChannelId].ulCorrectedNo;
        pstSignalInfo->ulUnCorrectedNo 		= s_stDb3128SignalStatus[nChannelId].ulUnCorrectedNo;
        pstSignalInfo->ulUnErroredNo 		= s_stDb3128SignalStatus[nChannelId].ulUnErroredNo;

        /* TODO_JDCHO - Activate when the latest DI is ported
        pstSignalInfo->fSignalInputPower	= s_stDb7580SignalStatus[nChannelId].fSignalPower;
        */
        CH_DI_Print(CH_LOG_SIGINFO,("=================== DB3128_GetStrengthQuality[%d] ===================\n",nChannelId));
        CH_DI_Print(CH_LOG_SIGINFO,("  ulStrength               = %d\n", pstSignalInfo->ulStrength));
        CH_DI_Print(CH_LOG_SIGINFO,("  ulQuality                = %d\n", pstSignalInfo->ulQuality));
        CH_DI_Print(CH_LOG_SIGINFO,("  fBer                     = %3.2e\n", pstSignalInfo->fBer));
        CH_DI_Print(CH_LOG_SIGINFO,("  fSnr                     = %f\n", pstSignalInfo->fSnr));

        /* TODO_JDCHO - Activate when the latest DI is ported
        CH_DI_Print(CH_LOG_1,("  ulIfAgc                  = %u\n", pstSignalInfo->ulIfAgc));
        CH_DI_Print(CH_LOG_1,("  ulRfAgc                  = %u\n", pstSignalInfo->ulRfAgc));
        */
        CH_DI_Print(CH_LOG_SIGINFO,("  ulCorrectedNo            = %u\n", pstSignalInfo->ulCorrectedNo));
        CH_DI_Print(CH_LOG_SIGINFO,("  ulUnCorrectedNo          = %u\n", pstSignalInfo->ulUnCorrectedNo));
        CH_DI_Print(CH_LOG_SIGINFO,("  ulUnErroredNo            = %u\n", pstSignalInfo->ulUnErroredNo));

        /* TODO_JDCHO - Activate when the latest DI is ported
        CH_DI_Print(CH_LOG_1,("  fSignalInputPower        = %u\n", pstSignalInfo->fSignalInputPower));
        */
        CH_DI_Print(CH_LOG_SIGINFO,("======================================================================\n"));
    }
    else
    {
        s_stDb3128SignalStatus[nChannelId].nStrength = 0;
        s_stDb3128SignalStatus[nChannelId].ulQuality = 0;
        s_stDb3128SignalStatus[nChannelId].ucAgc = 0;		// Ignore
        s_stDb3128SignalStatus[nChannelId].ulBer = 0;
        s_stDb3128SignalStatus[nChannelId].ulSnr = 0;
        s_stDb3128SignalStatus[nChannelId].ucRfAgc = 0;
        s_stDb3128SignalStatus[nChannelId].ucIfAgc = 0;
        s_stDb3128SignalStatus[nChannelId].ulCorrectedNo = 0;
        s_stDb3128SignalStatus[nChannelId].ulUnCorrectedNo = 0;
        s_stDb3128SignalStatus[nChannelId].ulUnErroredNo = 0;
        s_stDb3128SignalStatus[nChannelId].fSignalPower = 0;

        pstSignalInfo->ulStrength			= 0;
        pstSignalInfo->ulQuality			= 0;
        pstSignalInfo->ulAgc				= 0;		// Ignore
        pstSignalInfo->fBer					= 0;
        pstSignalInfo->fSnr					= 0;

        /* TODO_JDCHO - Activate when the latest DI is ported
        pstSignalInfo->ulRfAgc				= 0;
        pstSignalInfo->ulIfAgc				= 0;
        */
        pstSignalInfo->ulCorrectedNo		= 0;
        pstSignalInfo->ulUnCorrectedNo		= 0;
        pstSignalInfo->ulUnErroredNo		= 0;

        /* TODO_JDCHO - Activate when the latest DI is ported
        pstSignalInfo->fSignalInputPower	= 0;
        */
        CH_DI_Print(CH_LOG_DEFAULT,("DB3128_GetStrengthQuality[%d] : Unlocked \n",nChannelId));
    }

    return DI_CH_OK;
}


int  DB3128_SetPowerState( int nChannelId, CH_Power_e etPower )
{
    int nUnitId;
    int	nResult = DI_CH_OK;

    CH_DI_Print(CH_LOG_STATUS, ("[DB3128_SetPowerState[%d]] ----\n",nChannelId));

    nUnitId = GetDeviceUnitId( nChannelId );

    switch (etPower)
    {
        case CH_POWER_OFF:  //tmPowerStandby = 1
        SetActiveStatus( nChannelId, CH_ACTION_OFF );
        CH_DI_Print(CH_LOG_STATUS, ("\n -------------CH_C_SetPowerState standby mode -------------\n"));
        break;

        case CH_POWER_ON: //tmPowerOn = 0
        SetActiveStatus( nChannelId, CH_ACTION_ON );
        CH_DI_Print(CH_LOG_STATUS, ("\n -------------CH_C_SetPowerState Power on mode -------------\n"));
        break;
    }

    return nResult;
}

int DB3128_EnableTsOutput ( int nChannelId )
{
    int nUnitId = 0;
#if (NEXUS_VERSION >= 1404)
#else
    NEXUS_InputBand		inband;
    NEXUS_InputBandSettings inputBandSettings;
#endif
	if(s_bInitDevice == false)
	{
		return DI_CH_NOT_SUPPORTED;
	}

    nUnitId = GetDeviceUnitId( nChannelId );

    CH_DI_Print(CH_LOG_ARGS, (">>>>>>>>>>>> DB3128_EnableTsOutput !!!!!\n"));

#if (NEXUS_VERSION >= 1404)
    NEXUS_Frontend_SetInputBandEnable(s_st3128Frontend[nUnitId], true);
#else
    inband = DRV_TS_PATH_GetInbandID(nUnitId);
    NEXUS_InputBand_GetSettings(inband, &inputBandSettings);
    inputBandSettings.validActiveHigh = true;
    NEXUS_InputBand_SetSettings(inband, &inputBandSettings);
#endif

    return DI_CH_OK;
}

int DB3128_DisableTsOutput ( int nChannelId )
{
    int nUnitId = 0;
#if (NEXUS_VERSION >= 1404)
#else
    NEXUS_InputBand		inband;
    NEXUS_InputBandSettings inputBandSettings;
#endif

	if(s_bInitDevice == false)
	{
		return DI_CH_NOT_SUPPORTED;
	}

    nUnitId = GetDeviceUnitId( nChannelId );

    CH_DI_Print(CH_LOG_ARGS, (">>>>>>>>>>>> DB3128_DisableTsOutput !!!!!\n"));

#if (NEXUS_VERSION >= 1404)
    NEXUS_Frontend_SetInputBandEnable(s_st3128Frontend[nUnitId], false);
#else
   // NEXUS_Frontend_Untune(s_st3128Frontend[nUnitId]);
    inband = DRV_TS_PATH_GetInbandID(nUnitId);
    NEXUS_InputBand_GetSettings(inband, &inputBandSettings);
    inputBandSettings.validActiveHigh = false;
    NEXUS_InputBand_SetSettings(inband, &inputBandSettings);
#endif

    return DI_CH_OK;
}


int DB3128_GetDeviceStatus ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus )
{
    int nUnitId = 0;
    NEXUS_Error nRet = NEXUS_SUCCESS;
    NEXUS_FrontendDeviceStatus stDevStatus;
    NEXUS_FrontendDeviceHandle deviceHandle = NULL;

	if(s_bInitDevice == false)
	{
		return DI_CH_NOT_SUPPORTED;
	}
    nUnitId = GetDeviceUnitId( nChannelId );

    deviceHandle = NEXUS_Frontend_GetDevice(s_st3128Frontend[nUnitId]);
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

    CH_DI_Print(CH_LOG_SIGINFO, (">>>>>>>>>>>> DB3128_GetDeviceStatus !!!!!\n"));
    CH_DI_Print(CH_LOG_SIGINFO, ("avs.enabled : [%d] \n",stDevStatus.avs.enabled));
    CH_DI_Print(CH_LOG_SIGINFO, ("avs.voltage : [%d] \n",stDevStatus.avs.voltage));
    CH_DI_Print(CH_LOG_SIGINFO, ("temperature : [%d] \n",stDevStatus.temperature));
    CH_DI_Print(CH_LOG_SIGINFO, (">>>>>>>>>>>> DB3128_GetDeviceStatus !!!!!\n"));
    pstDeviceStatus->avs.enabled = stDevStatus.avs.enabled;
    pstDeviceStatus->avs.voltage = stDevStatus.avs.voltage;
    pstDeviceStatus->temperature = stDevStatus.temperature;

    return DI_CH_OK;

}
