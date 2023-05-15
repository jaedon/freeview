/* $Header$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:
// Original Author: Full Name!!
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 										         */
/* All rights reserved.																	           */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files
#include "nexus_frontend.h"
#include "nexus_platform.h"

#include <memory.h>
#include "htype.h"
#include "vkernel.h"
#include "drv_gpio.h"

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "di_channel_util.h"

#include "drv_channel.h"
#include "drv_channel_t.h"
#include "drv_channel_db3461.h"

//TS Path Control
#include "nexus_input_band.h"
#include "drv_tspath.h"

#if defined(CONFIG_TEMP_HDR4000T)
#include "di_system.h"
#endif
// End Including Headers





/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define MASTER_CHANNEL			(0)
#define NUM_OF_TER_CHANNEL	(CONFIG_NUM_OF_TER_CHANNEL)

#define 	OFDM_UNLOCKED			0
#define 	OFDM_LOCKED				1
#define 	OFDM_CHECK_CANCELED		2

#define MAX_NUM_PLPS			(255)

#define FIRST_WAIT_TIME		(550)//(550)	//(150)
#define HAB_WAIT_TIME			(130)
#define MAX_REACQUIRE_COUNTS 	(7)//(30)//(2)//(10)//(4)
#define WAIT_OFDM_LOCKING	(150)	//(30)//(150)//(300)//(50)//(100)

//#define FIX_MANUAL_TPS        // Off Manual tps , DTG Impulse (35031), Echo Delay (12498)
//#define BCM_DEBUG_INFO
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
static NEXUS_FrontendOfdmSettings s_stOfdmSettings[NUM_OF_TER_CHANNEL];
static NEXUS_FrontendHandle s_hFrontend[NUM_OF_TER_CHANNEL] = {NULL,};
static BKNI_EventHandle s_evStatusEvent[NUM_OF_TER_CHANNEL];

static HUINT16 s_usCellId[NUM_OF_TER_CHANNEL] = {0};
static HUINT8 s_ucNumberPlp[NUM_OF_TER_CHANNEL] = {0};
static HUINT8 s_aucPlpID[NUM_OF_TER_CHANNEL][MAX_NUM_PLPS];

static CH_TER_AntStatus_t s_stAntState[NUM_OF_TER_CHANNEL];

#if defined(CONFIG_TEMP_HDR4000T)
static HUINT8 s_ucHwRev=0;
#endif

// End static variable


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:

#define __CH_T_PRIVATE_FUNCITONS__

static void lock_callback(void *context, int param)
{
    int nUnitId = 0;
    NEXUS_FrontendHandle hFrontend = (NEXUS_FrontendHandle)context;
#if defined(BCM_DEBUG_INFO)
    NEXUS_FrontendOfdmStatus ofdmStatus;
#endif
    CH_DI_Print(CH_LOG_4, ("lock_callback is called...with context [0x%x]\n", hFrontend));

    if( s_hFrontend[0] == hFrontend )
    {
        CH_DI_Print(CH_LOG_4, ("Master channel!!!\n"));
        nUnitId = 0;
    }
    else
    {
        CH_DI_Print(CH_LOG_4, ("Slave channel!!!\n"));
        nUnitId = 1;
    }

    CH_UNUSED(param);
#if defined(BCM_DEBUG_INFO)
    NEXUS_Frontend_GetOfdmStatus(s_hFrontend[nUnitId], &ofdmStatus);
    CH_DI_Print(CH_LOG_4, ("OFDM Lock callback: receiverLock=%d, fecLock=%d\n", ofdmStatus.receiverLock, ofdmStatus.fecLock));
#endif
}

static void async_status_ready_callback(void *context, int param)
{
    CH_UNUSED(param);

    CH_DI_Print(CH_LOG_4, ("async_status_ready_callback\n"));
    BKNI_SetEvent((BKNI_EventHandle)context);
}

#if defined(FIX_MANUAL_TPS)
static int ConvertBcmModulation( CH_TER_Moudulation_e etModulation )
{
	switch (etModulation)
	{
		case DI_CH_TMOD_QPSK:	return NEXUS_FrontendOfdmModulation_eQpsk;
		case DI_CH_TMOD_16QAM:	return NEXUS_FrontendOfdmModulation_eQam16;
		case DI_CH_TMOD_64QAM:	return NEXUS_FrontendOfdmModulation_eQam64;
		case DI_CH_TMOD_256QAM : 	return NEXUS_FrontendOfdmModulation_eQam256;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertBcmModulation() etModulation=(%d) \n", etModulation));
			return -1;
	}
}

static int ConvertBcmHierarchy( CH_TER_Hierarchy_e etHierarchy )
{
	switch (etHierarchy)
	{
		case DI_CH_HIERARCHY_NONE:	return NEXUS_FrontendOfdmHierarchy_e0;
		case DI_CH_HIERARCHY_1:	return NEXUS_FrontendOfdmHierarchy_e1;
		case DI_CH_HIERARCHY_2:	return NEXUS_FrontendOfdmHierarchy_e2;
		case DI_CH_HIERARCHY_4:	return NEXUS_FrontendOfdmHierarchy_e4;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertBcmHierarchy() etHierarchy=(%d) \n", etHierarchy));
			return -1;
	}
}


static int ConvertBcmCodeRate( CH_TER_CodeRate_e etCodeRate )
{
	switch (etCodeRate)
	{
		case DI_CH_TCODERATE_1_2:	return NEXUS_FrontendOfdmCodeRate_e1_2;
		case DI_CH_TCODERATE_2_3:	return NEXUS_FrontendOfdmCodeRate_e2_3;
		case DI_CH_TCODERATE_3_4:	return NEXUS_FrontendOfdmCodeRate_e3_4;
		case DI_CH_TCODERATE_5_6:	return NEXUS_FrontendOfdmCodeRate_e5_6;
		case DI_CH_TCODERATE_7_8:	return NEXUS_FrontendOfdmCodeRate_e7_8;
		case DI_CH_T2CODERATE_3_5 : 	return NEXUS_FrontendOfdmCodeRate_e3_5;
		case DI_CH_T2CODERATE_4_5 : 	return NEXUS_FrontendOfdmCodeRate_e4_5;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertBcmCodeRate() etCodeRate=(%d) \n", etCodeRate));
			return -1;
	}
}


static int ConvertBcmFftSize( CH_TER_TransMode_e etTransMode )
{
	switch (etTransMode)
	{
		case DI_CH_TRANSMODE_2K:	return NEXUS_FrontendOfdmTransmissionMode_e2k;
		case DI_CH_TRANSMODE_8K:	return NEXUS_FrontendOfdmTransmissionMode_e8k;
		case DI_CH_T2TRANSMODE_4K:		return NEXUS_FrontendOfdmTransmissionMode_e4k;
		case DI_CH_T2TRANSMODE_1K:		return NEXUS_FrontendOfdmTransmissionMode_e1k;
		case DI_CH_T2TRANSMODE_16K:		return NEXUS_FrontendOfdmTransmissionMode_e16k;
		case DI_CH_T2TRANSMODE_32K:		return NEXUS_FrontendOfdmTransmissionMode_e32k;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertBcmFftSize() etTransMode(%d) \n", etTransMode));
			return -1;
	}
}


static int ConvertBcmGuardInterval( CH_TER_GuardInterval_e etGuardInterval )
{
	switch (etGuardInterval)
	{
		case DI_CH_GUARDINTERVAL_1_32:	return NEXUS_FrontendOfdmGuardInterval_e1_32;
		case DI_CH_GUARDINTERVAL_1_16:	return NEXUS_FrontendOfdmGuardInterval_e1_16;
		case DI_CH_GUARDINTERVAL_1_8:	return NEXUS_FrontendOfdmGuardInterval_e1_8;
		case DI_CH_GUARDINTERVAL_1_4:	return NEXUS_FrontendOfdmGuardInterval_e1_4;
		case DI_CH_T2GUARDINTERVAL_1_128: 	return NEXUS_FrontendOfdmGuardInterval_e1_128;
		case DI_CH_T2GUARDINTERVAL_19_128:  	return NEXUS_FrontendOfdmGuardInterval_e19_128;
		case DI_CH_T2GUARDINTERVAL_19_256 :  	return NEXUS_FrontendOfdmGuardInterval_e19_256;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertBcmGuardInterval() etGuardInterval(%d) \n", etGuardInterval));
			return -1;
	}
}
#endif

#if 0 /* Not used */
static int ConvertTdaOffset( CH_TER_Offset_e etOffset )
{
	switch (etOffset)
	{
		case DI_CH_OFFSET_NONE:		return BCM3556_OFFSET_NONE;
		case DI_CH_OFFSET_1_6_P:	return BCM3556_OFFSET_1_6_P;
		case DI_CH_OFFSET_1_6_N:	return BCM3556_OFFSET_1_6_N;
		case DI_CH_OFFSET_2_6_P:	return BCM3556_OFFSET_2_6_P;
		case DI_CH_OFFSET_2_6_N:	return BCM3556_OFFSET_2_6_N;
		case DI_CH_OFFSET_3_6_P:	return BCM3556_OFFSET_3_6_P;
		case DI_CH_OFFSET_3_6_N:	return BCM3556_OFFSET_3_6_N;
		default:
			CH_DI_Print(0, ("Error : ConvertTdaOffset() etOffset=(%d) \n", etOffset));
			return -1;
	}
}
#endif

/*********************************************************************************/


static int ConvertDiModulation( NEXUS_FrontendOfdmModulation etModulation )
{

	CH_DI_Print(CH_LOG_4, (">>>>>>>>>>>>> ConvertDiModulation() etModulation=(%d) \n", etModulation));
	switch (etModulation)
	{
		case NEXUS_FrontendOfdmModulation_eQpsk:  	return DI_CH_TMOD_QPSK;
		case NEXUS_FrontendOfdmModulation_eQam16: 	return DI_CH_TMOD_16QAM;
		case NEXUS_FrontendOfdmModulation_eQam64: 	return DI_CH_TMOD_64QAM;
		case NEXUS_FrontendOfdmModulation_eQam256: 	return DI_CH_TMOD_256QAM;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertDiModulation() etModulation=(%d) \n", etModulation));
			return -1;
	}
}

static int ConvertDiHierarchy( NEXUS_FrontendOfdmHierarchy etHierarchy )
{
	switch (etHierarchy)
	{
		case NEXUS_FrontendOfdmHierarchy_e0: 	return DI_CH_HIERARCHY_NONE;
		case NEXUS_FrontendOfdmHierarchy_e1:    	return DI_CH_HIERARCHY_1;
		case NEXUS_FrontendOfdmHierarchy_e2:    	return DI_CH_HIERARCHY_2;
		case NEXUS_FrontendOfdmHierarchy_e4:    	return DI_CH_HIERARCHY_4;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertDiHierarchy() etHierarchy=(%d) \n", etHierarchy));
			return -1;
	}
}

static int ConvertDiCodeRate( NEXUS_FrontendOfdmCodeRate etCodeRate )
{
	switch (etCodeRate)
	{
		case NEXUS_FrontendOfdmCodeRate_e1_2: 	return DI_CH_TCODERATE_1_2;
		case NEXUS_FrontendOfdmCodeRate_e2_3: 	return DI_CH_TCODERATE_2_3;
		case NEXUS_FrontendOfdmCodeRate_e3_4: 	return DI_CH_TCODERATE_3_4;
		case NEXUS_FrontendOfdmCodeRate_e5_6: 	return DI_CH_TCODERATE_5_6;
		case NEXUS_FrontendOfdmCodeRate_e7_8: 	return DI_CH_TCODERATE_7_8;
		case NEXUS_FrontendOfdmCodeRate_e3_5: 	return DI_CH_T2CODERATE_3_5;
		case NEXUS_FrontendOfdmCodeRate_e4_5: 	return DI_CH_T2CODERATE_4_5;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertDiCodeRate() etCodeRate=(%d) \n", etCodeRate));
			return -1;
	}
}

static int ConvertDiFftSize( NEXUS_FrontendOfdmTransmissionMode etTransMode )
{
	switch (etTransMode)
	{
		case NEXUS_FrontendOfdmTransmissionMode_e2k:		return DI_CH_TRANSMODE_2K;
		case NEXUS_FrontendOfdmTransmissionMode_e8k:		return DI_CH_TRANSMODE_8K;
		case NEXUS_FrontendOfdmTransmissionMode_e4k:		return DI_CH_T2TRANSMODE_4K;
		case NEXUS_FrontendOfdmTransmissionMode_e1k:		return DI_CH_T2TRANSMODE_1K;
		case NEXUS_FrontendOfdmTransmissionMode_e16k:		return DI_CH_T2TRANSMODE_16K;
		case NEXUS_FrontendOfdmTransmissionMode_e32k:		return DI_CH_T2TRANSMODE_32K;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertDiFftSize() etTransMode=(%d) \n", etTransMode));
			return -1;
	}
}


static int ConvertDiGuardInterval( NEXUS_FrontendOfdmGuardInterval etGuardInterval )
{
	switch (etGuardInterval)
	{
		case NEXUS_FrontendOfdmGuardInterval_e1_32: 	return DI_CH_GUARDINTERVAL_1_32;
		case NEXUS_FrontendOfdmGuardInterval_e1_16: 	return DI_CH_GUARDINTERVAL_1_16;
		case NEXUS_FrontendOfdmGuardInterval_e1_8:  	return DI_CH_GUARDINTERVAL_1_8;
		case NEXUS_FrontendOfdmGuardInterval_e1_4:  	return DI_CH_GUARDINTERVAL_1_4;
		case NEXUS_FrontendOfdmGuardInterval_e1_128: 	return DI_CH_T2GUARDINTERVAL_1_128;
		case NEXUS_FrontendOfdmGuardInterval_e19_128:  	return DI_CH_T2GUARDINTERVAL_19_128;
		case NEXUS_FrontendOfdmGuardInterval_e19_256:  	return DI_CH_T2GUARDINTERVAL_19_256;
		default:
			CH_DI_Print(CH_LOG_0, ("Error : ConvertDiGuardInterval() etGuardInterval=(%d) \n", etGuardInterval));
			return -1;
	}
}


#if 0 /* Not used */
static int ConvertDiOffset( BCM3556_Offset_e etOffset )
{
	switch (etOffset)
	{
		case BCM3556_OFFSET_NONE:  	return DI_CH_OFFSET_NONE;
		case BCM3556_OFFSET_1_6_P: 	return DI_CH_OFFSET_1_6_P;
		case BCM3556_OFFSET_1_6_N: 	return DI_CH_OFFSET_1_6_N;
		case BCM3556_OFFSET_2_6_P: 	return DI_CH_OFFSET_2_6_P;
		case BCM3556_OFFSET_2_6_N: 	return DI_CH_OFFSET_2_6_N;
		case BCM3556_OFFSET_3_6_P: 	return DI_CH_OFFSET_3_6_P;
		case BCM3556_OFFSET_3_6_N:		return DI_CH_OFFSET_3_6_N;
		default:
			CH_DI_Print(0, ("Error : ConvertDiOffset() etOffset=(%d) \n", etOffset));
			return -1;
	}
}
#endif


static HBOOL WaitOfdmLocked( int nChannelId )
{
    int nRet = 0, nLoopCount=0;
    CH_Status_e etCurrentState;
    int nUnitId = 0;

    NEXUS_FrontendFastStatus status;

    nUnitId = GetDeviceUnitId( nChannelId );

    /*tune command approximately 430 msec after tuning (116 msec after commanding acquire)*/
    VK_TASK_Sleep(FIRST_WAIT_TIME);//(500ms);

    while(1)
    {
        
		etCurrentState = GetTuneStatus(nChannelId);
        if( ( etCurrentState == CH_STATUS_IDLE) || (etCurrentState == CH_STATUS_CANCLE) )
        {
            CH_DI_Print(CH_LOG_4, ("Cancel locking status..."));
            return OFDM_CHECK_CANCELED;
        }

        nRet = NEXUS_Frontend_GetFastStatus(s_hFrontend[nUnitId], &status);
        if( nRet != DI_CH_OK )
        {
            CH_DI_Print(0, ("Error : 0x%x", nRet));
        }

        CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
        CH_DI_Print(CH_LOG_4, ("NEXUS_Frontend_GetFastStatus: nLoopCount=[%d]\n", nLoopCount));
        CH_DI_Print(CH_LOG_4, ("NEXUS_Frontend_GetFastStatus: lockStatus=[%d]\n", status.lockStatus));
        CH_DI_Print(CH_LOG_4, ("NEXUS_Frontend_GetFastStatus: acquireInProgress=[%d]\n", status.acquireInProgress));

        if( status.lockStatus == NEXUS_FrontendLockStatus_eLocked)
        {
            CH_DI_Print(CH_LOG_4, ("NEXUS_Frontend_GetFastStatus: NEXUS_FrontendLockStatus_eLocked \n"));
            CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
            return OFDM_LOCKED;
        }
        else if(status.lockStatus == NEXUS_FrontendLockStatus_eNoSignal)
        {
            CH_DI_Print(CH_LOG_4, ("NEXUS_Frontend_GetFastStatus: NEXUS_FrontendLockStatus_eNoSignal \n"));
            CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
            //return OFDM_UNLOCKED;
        }
        else if(status.lockStatus == NEXUS_FrontendLockStatus_eUnlocked)
        {
            CH_DI_Print(CH_LOG_4, ("NEXUS_Frontend_GetFastStatus: NEXUS_FrontendLockStatus_eUnlocked \n"));
        }
        else
        {
            CH_DI_Print(CH_LOG_4, ("NEXUS_Frontend_GetFastStatus: NEXUS_FrontendLockStatus_eUnknown... Try one more... \n"));
        }
        CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));

        if( nLoopCount >= MAX_REACQUIRE_COUNTS )
        {
            CH_DI_Print(CH_LOG_4, ("########## nLoopCount=[%d] ########## \n", nLoopCount));
            return OFDM_UNLOCKED;
        }

        VK_TASK_Sleep(WAIT_OFDM_LOCKING);
        nLoopCount++;
     }
}


#if !defined(CONFIG_SUPPORT_ADVANCED_TUNING)
static void UpdateTunedOfdmParam( int nChannelId, CH_TER_TuneParam_t *pstTerTuneParam )
{
	int i=0, nNumPlp=0, nNumDataPlp=0;
	int nUnitId = 0;
	int nRet=0;
	NEXUS_FrontendOfdmStatus stOfdmStatus;

	nUnitId = GetDeviceUnitId( nChannelId );

	nRet = NEXUS_Frontend_GetOfdmStatus(s_hFrontend[nUnitId], &stOfdmStatus);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ("Error : NEXUS_Frontend_GetOfdmStatus(0x%x)", nRet));
	}

	CH_DI_Print(CH_LOG_4,  ("*********************** DB3461_GetLockedParam[%d] *************************\n", nChannelId));
	CH_DI_Print(CH_LOG_4,  (" modulation             [%d] \n", stOfdmStatus.modulation));
	CH_DI_Print(CH_LOG_4,  (" transmissionMode       [%d] \n", stOfdmStatus.transmissionMode));
	CH_DI_Print(CH_LOG_4,  (" guardInterval          [%d] \n", stOfdmStatus.guardInterval));
	CH_DI_Print(CH_LOG_4,  (" codeRate               [%d] \n", stOfdmStatus.codeRate));
	CH_DI_Print(CH_LOG_4,  (" hierarchy              [%d] \n", stOfdmStatus.hierarchy));
	CH_DI_Print(CH_LOG_4,  ("**************************************************************************\n"));

	pstTerTuneParam->etModulation 		= ConvertDiModulation( stOfdmStatus.modulation );
	pstTerTuneParam->etTransMode 		= ConvertDiFftSize( stOfdmStatus.transmissionMode );
	pstTerTuneParam->etGuardInterval 	       = ConvertDiGuardInterval( stOfdmStatus.guardInterval );
	pstTerTuneParam->etCodeRate 		= ConvertDiCodeRate( stOfdmStatus.codeRate );
	pstTerTuneParam->etHierarchy 		= ConvertDiHierarchy( stOfdmStatus.hierarchy );

	s_usCellId[nUnitId] = (HUINT16)stOfdmStatus.cellId;

      CH_DI_Print(CH_LOG_4,  (" s_usCellId[%d                [%d] \n", nUnitId, s_usCellId[nUnitId]));

	if(pstTerTuneParam->etTerSystem == DI_CH_DVBT2)
	{
		nNumPlp = stOfdmStatus.dvbt2Status.l1PlpStatus.numPlp;

		CH_DI_Print(CH_LOG_4,  ("***********************   DVB-T2 PLP Info.   *************************\n"));
		CH_DI_Print(CH_LOG_4,  (" numPlp               [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.numPlp));
		for(i=0;i<nNumPlp;i++)
		{
			CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
			CH_DI_Print(CH_LOG_4,  (" plpId                [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpId));
			CH_DI_Print(CH_LOG_4,  (" plpGroupId           [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpGroupId));
			CH_DI_Print(CH_LOG_4,  (" plpPayloadType       [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpPayloadType));
			CH_DI_Print(CH_LOG_4,  (" plpType              [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpType));
			if(stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpType != NEXUS_FrontendOfdmPlpType_eCommon)
			{
				s_aucPlpID[nChannelId][nNumDataPlp] = stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpId;
				nNumDataPlp++;
			}
		}
		CH_DI_Print(CH_LOG_4,  (" nNumDataPlp               [%d] \n", nNumDataPlp));
		s_ucNumberPlp[nChannelId] = nNumDataPlp;
		CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
	}

	return;
}
#endif

static void ResetChannelHw( int nUnitId )
{
	CH_UNUSED(nUnitId);
	//if( nUnitId	== 1 )
	{
		DRV_GPIO_Write(GPIO_ID_nCH1_RST, GPIO_LOW);
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_LOW);
#if defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)
    	DRV_GPIO_Write(GPIO_ID_nCH2_RST, GPIO_LOW);
#endif

		VK_TASK_Sleep(30);//min 5ms

		DRV_GPIO_Write(GPIO_ID_nCH1_RST, GPIO_HI);
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_HI);
#if defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)
    	DRV_GPIO_Write(GPIO_ID_nCH2_RST, GPIO_HI);
#endif

		VK_TASK_Sleep(250);//min 210ms
	}
	return;
}



static HUINT32 GetStrengthDb3461(int nReceivedDbuv)
{
	HUINT32	ulStrengthIndicator = 0;

	if( nReceivedDbuv > 55 ) ulStrengthIndicator = nReceivedDbuv + 10;
	else if( nReceivedDbuv > 35 ) ulStrengthIndicator = nReceivedDbuv;
	else if( nReceivedDbuv > 27 ) ulStrengthIndicator = nReceivedDbuv - 5;
	else ulStrengthIndicator = 10;

	if( ulStrengthIndicator > 100 )
	{
		ulStrengthIndicator = 100;
	}

	return ulStrengthIndicator;
}



static HUINT32 ConvertQualityLevel(
	CH_TER_System_e etTerSysterm,
	float fInSnr
)
{
	HUINT32 ulQuality = 0;

	CH_DI_Print(CH_LOG_4, ("[ConvertQualityLevel] ulInSnr: %3.1f \n", fInSnr));

	if( etTerSysterm == DI_CH_DVBT2 )
	{
		if( fInSnr > 19.5)
			ulQuality = 100;
		else if( fInSnr > 18.5)
			ulQuality = 80;
		else if( fInSnr > 17.5)
			ulQuality = 50;
		else
			ulQuality = 10;
	}
	else
	{
		if( fInSnr > 17.5)
			ulQuality = 100;
		else if( fInSnr > 16.5)
			ulQuality = 80;
		else if( fInSnr > 15.5)
			ulQuality = 70;
		else if( fInSnr > 14.5)
			ulQuality = 30;
		else
			ulQuality = 10;
	}

	CH_DI_Print(CH_LOG_4, ("[ConvertQualityLevel] => ulQuality: (%d)	\n", ulQuality ));

	return ulQuality;
}



#define __CH_T_PUBLIC_FUNCITONS__


void DRV_T_InstallApi( void )
{
	TER_InitDevice				= &DB3461_InitDevice;
	TER_SetTune 				= &DB3461_SetTune;
	TER_CheckLock				= &DB3461_CheckLock;
	TER_GetSignalInfo		= &DB3461_GetStrengthQuality;
	TER_SetPowerState			= &DB3461_SetPowerState;
	TER_EnableTsOutput			= &DB3461_EnableTsOutput;
	TER_DisableTsOutput 		= &DB3461_DisableTsOutput;
	TER_SetTsOutputMode			= &DB3461_SetTSOutputMode;
	TER_CheckAntState			= &DB3461_CheckAntState;
	TER_SetAntState				= &DB3461_SetAntState;
	TER_GetAntState				= &DB3461_GetAntState;
	TER_ProcessRecoverAntenna 	= &DB3461_ProcessRecoverAntenna;
	TER_GetDataPlpCount		= &DB3461_GetPlpCount;
	TER_GetDataPlpInfo		= &DB3461_GetPlpInfo;
	TER_GetCellId			= &DB3461_GetCellId;
	TER_GetLockedInfo		= &DB3461_GetLockedParam;
	TER_ReInitDevice		= &DB3461_ReInitDevice;
      TER_UnInitDevice              = &DB3461_UnInitDevice;

	CH_DI_Print(CH_LOG_4, ("[DB3461] DRV_T_InstallApi()  OK!\n"));

	return;
}


int DB3461_InitDevice( int nChannelId )
{
    int nUnitId=0, i=0, nRetryCount = 0;


    NEXUS_PlatformConfiguration platformConfig;

    NEXUS_FrontendHandle frontend[NUM_OF_TER_CHANNEL]={NULL,};
    NEXUS_FrontendDeviceHandle frontendDevice[NUM_OF_TER_CHANNEL];
    NEXUS_FrontendDeviceStatus frontendDeviceStatus[NUM_OF_TER_CHANNEL];

    CH_DI_Print(CH_LOG_4, ("DB3461_InitDevice ...S [%d] \n", nChannelId));

    nUnitId = GetDeviceUnitId( nChannelId );

    if(nChannelId == MASTER_CHANNEL)
    {
#if defined(CONFIG_TEMP_HDR4000T)
	 if(DI_ERR_OK == DI_SYSTEM_GetSystemInfo (DI_SYS_HW_REVISION, &s_ucHwRev, 1))
	 {
	 	CH_DI_Print(CH_LOG_0, ( "\n HW revision = 0x%x \n", s_ucHwRev));	
	 }
	 else
	 {
		 CH_DI_Print(CH_LOG_0, ( "\n Check HW revision error!! = 0x%x \n", s_ucHwRev));	 
		 s_ucHwRev = 0;
	 }
#endif	 
	 
        ResetChannelHw(nUnitId);
        NEXUS_Platform_OpenFrontend(nUnitId);

        for ( i = 0; i < NUM_OF_TER_CHANNEL; i++ )
        {
            NEXUS_FrontendAcquireSettings settings;
            NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
            settings.capabilities.ofdm = true;
            frontend[i] = NEXUS_Frontend_Acquire(&settings);
            if (!frontend[i])
            {
                CH_DI_Print(CH_LOG_0, ("Unable to find OFDM-capable frontend[%d]\n", i));
                return -1;
            }
        }

        frontendDevice[0] = NEXUS_Frontend_GetDevice(frontend[0]);
        frontendDevice[1] = NEXUS_Frontend_GetDevice(frontend[1]);
#if defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND) && defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)  /*TODO_SYKIM */
        frontendDevice[2] = NEXUS_Frontend_GetDevice(frontend[2]);
#endif
        while(1)
        {
            NEXUS_FrontendDevice_GetStatus(frontendDevice[0],&frontendDeviceStatus[0]);
            NEXUS_FrontendDevice_GetStatus(frontendDevice[1],&frontendDeviceStatus[1]);
#if defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND) && defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)  /*TODO_SYKIM */
            NEXUS_FrontendDevice_GetStatus(frontendDevice[2],&frontendDeviceStatus[2]);
#endif         
            if((frontendDeviceStatus[0].openFailed == false && frontendDeviceStatus[0].openPending == false) 
            && (frontendDeviceStatus[1].openFailed == false && frontendDeviceStatus[1].openPending == false)
#if defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND) && defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)  /*TODO_SYKIM */            
            && (frontendDeviceStatus[2].openFailed == false && frontendDeviceStatus[2].openPending == false)
#endif
            )
            {
                CH_DI_Print(CH_LOG_1, ("2 frontend open is okay!! \n"));
                break;
            }
		else
		{
			if (nRetryCount > NUM_OF_TER_CHANNEL* 10)
			{
                		CH_DI_Print(CH_LOG_0, (" >>>>>>>>>>>> frontend open fail!!! Rty Cnt[%d] <<<<<<<<<<<<<<<< \n", nRetryCount));
				CH_DI_Print(CH_LOG_0, (" frontend open is not okay!! 1st demod [%d][%d] \n", frontendDeviceStatus[0].openFailed, frontendDeviceStatus[0].openPending));
			if(NUM_OF_TER_CHANNEL >= 2) 
				CH_DI_Print(CH_LOG_0, (" frontend open is not okay!! 2nd demod [%d][%d] \n", frontendDeviceStatus[1].openFailed, frontendDeviceStatus[1].openPending));
			if(NUM_OF_TER_CHANNEL >= 3)
				CH_DI_Print(CH_LOG_0, (" frontend open is not okay!! 3rd demod [%d][%d] \n", frontendDeviceStatus[2].openFailed, frontendDeviceStatus[2].openPending));
			if (NUM_OF_TER_CHANNEL == 4)
				CH_DI_Print(CH_LOG_0, (" frontend open is not okay!! 4th demod [%d][%d] \n", frontendDeviceStatus[3].openFailed, frontendDeviceStatus[3].openPending));
				return DI_CH_ERR;
			}
			nRetryCount ++;
		}
		
            VK_TASK_Sleep(1000);
        }
    }

    NEXUS_Platform_GetConfiguration(&platformConfig);

    s_hFrontend[nUnitId] = platformConfig.frontend[nUnitId];
    if (NULL == s_hFrontend[nUnitId] )
    {
        CH_DI_Print(CH_LOG_0, ("Error : Unable to find OFDM-capable frontend s_hFrontend[%d]\n", nUnitId));
        return DI_CH_ERR;
    }
    else
    {
        CH_DI_Print(CH_LOG_1, ("Find OFDM-capable frontend s_hFrontend[%d]=[0x%x] \n", nUnitId, s_hFrontend[nUnitId]));
    }

    BKNI_CreateEvent(&s_evStatusEvent[nUnitId]);

    s_stOfdmSettings[nUnitId].lockCallback.callback = lock_callback;
    s_stOfdmSettings[nUnitId].lockCallback.context = s_hFrontend[nUnitId];

#if !defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND)
    DRV_I2c_SetClock( 0, 100);
#endif

    return DI_CH_OK;

}


int DB3461_UnInitDevice( int nChannelId )
{
    int nUnitId=0,  i=0;
    
    nUnitId = GetDeviceUnitId( nChannelId );

    SetTuneStatus( nChannelId, CH_STATUS_IDLE );

    if(nChannelId == MASTER_CHANNEL)
    {
         for ( i = 0; i < NUM_OF_TER_CHANNEL; i++ )
        {
            CH_DI_Print(CH_LOG_0, (" DB3461_UnInitDevice] NEXUS_Frontend_Untune [%d] !!!!!\n", i));
	    NEXUS_Frontend_Untune(s_hFrontend[i]);
	    VK_TASK_Sleep(HAB_WAIT_TIME);//(116ms);
        }
        CH_DI_Print(CH_LOG_0, ("[DB3461] DB3461_UnInitDevice(%d) NEXUS_Platform_UninitFrontend !!\n", nUnitId));
        NEXUS_Platform_UninitFrontend();
    }

    return DI_CH_OK;

}


int DB3461_ReInitDevice( int nChannelId )
{
    int nUnitId=0, i=0, nRetryCount = 0;

    NEXUS_PlatformConfiguration platformConfig;

    NEXUS_FrontendHandle frontend[NUM_OF_TER_CHANNEL]={NULL,};
    NEXUS_FrontendDeviceHandle frontendDevice[NUM_OF_TER_CHANNEL];
    NEXUS_FrontendDeviceStatus frontendDeviceStatus[NUM_OF_TER_CHANNEL];

    CH_DI_Print(CH_LOG_4, ("DB3461_ReInitDevice[%d] ...S\n", nChannelId));

    nUnitId = GetDeviceUnitId( nChannelId );

    if(nChannelId == MASTER_CHANNEL)
    {
#if defined(CONFIG_TEMP_HDR4000T)
	if(DI_ERR_OK == DI_SYSTEM_GetSystemInfo (DI_SYS_HW_REVISION, &s_ucHwRev, 1))
	{
	 	CH_DI_Print(CH_LOG_0, ( "\n HWrevision = 0x%x \n", s_ucHwRev));	
	}
	else
	{
		CH_DI_Print(CH_LOG_0, ( "\n Check HW revision error!! = 0x%x \n", s_ucHwRev));  
		s_ucHwRev = 0;
	}
#endif	 
	 
        ResetChannelHw(nUnitId);
        NEXUS_Platform_OpenFrontend(nUnitId);

        for ( i = 0; i < NUM_OF_TER_CHANNEL; i++ )
        {
            NEXUS_FrontendAcquireSettings settings;
            NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
            settings.capabilities.ofdm = true;
            frontend[i] = NEXUS_Frontend_Acquire(&settings);
            if (!frontend[i])
            {
                CH_DI_Print(CH_LOG_0, ("Unable to find OFDM-capable frontend\n"));
                return -1;
            }
        }

        frontendDevice[0] = NEXUS_Frontend_GetDevice(frontend[0]);
        frontendDevice[1] = NEXUS_Frontend_GetDevice(frontend[1]);
#if defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND) && defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)  /*TODO_SYKIM */
        frontendDevice[2] = NEXUS_Frontend_GetDevice(frontend[2]);
#endif

        while(1)
        {
            NEXUS_FrontendDevice_GetStatus(frontendDevice[0],&frontendDeviceStatus[0]);
            NEXUS_FrontendDevice_GetStatus(frontendDevice[1],&frontendDeviceStatus[1]);
#if defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND) && defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)  /*TODO_SYKIM */
            NEXUS_FrontendDevice_GetStatus(frontendDevice[2],&frontendDeviceStatus[2]);
#endif         
            if((frontendDeviceStatus[0].openFailed == false && frontendDeviceStatus[0].openPending == false)
             && (frontendDeviceStatus[1].openFailed == false && frontendDeviceStatus[1].openPending == false)
#if defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND) && defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)  /*TODO_SYKIM */            
             && (frontendDeviceStatus[2].openFailed == false && frontendDeviceStatus[2].openPending == false)
#endif
            )
            {
                CH_DI_Print(CH_LOG_1, ("2 frontend open is okay!! \n"));
                break;
            }
		else
		{
			if (nRetryCount > NUM_OF_TER_CHANNEL* 10)
			{
				CH_DI_Print(CH_LOG_0, (" >>>>>>>>>>>> frontend open fail!!! Rty Cnt[%d] <<<<<<<<<<<<<<<< \n", nRetryCount));
				CH_DI_Print(CH_LOG_0, (" frontend open is not okay!! 1st demod [%d][%d] \n", frontendDeviceStatus[0].openFailed, frontendDeviceStatus[0].openPending));
			if(NUM_OF_TER_CHANNEL >= 2) 
				CH_DI_Print(CH_LOG_0, (" frontend open is not okay!! 2nd demod [%d][%d] \n", frontendDeviceStatus[1].openFailed, frontendDeviceStatus[1].openPending));
#if defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND) && defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)  /*TODO_SYKIM */            
			if(NUM_OF_TER_CHANNEL >= 3)
				CH_DI_Print(CH_LOG_0, (" frontend open is not okay!! 3rd demod [%d][%d] \n", frontendDeviceStatus[2].openFailed, frontendDeviceStatus[2].openPending));
#endif			
				return DI_CH_ERR;
			}
			nRetryCount ++;
		}
            VK_TASK_Sleep(1000);
        }
    }

    NEXUS_Platform_GetConfiguration(&platformConfig);

    s_hFrontend[nUnitId] = platformConfig.frontend[nUnitId];
    if (NULL == s_hFrontend[nUnitId] )
    {
        CH_DI_Print(CH_LOG_0, ("Error : Unable to find OFDM-capable frontend s_hFrontend[%d]\n", nUnitId));
        return DI_CH_ERR;
    }
    else
    {
        CH_DI_Print(CH_LOG_1, ("Find OFDM-capable frontend s_hFrontend[%d]=[0x%x] \n", nUnitId, s_hFrontend[nUnitId]));
    }

    BKNI_CreateEvent(&s_evStatusEvent[nUnitId]);

    s_stOfdmSettings[nUnitId].lockCallback.callback = lock_callback;
    s_stOfdmSettings[nUnitId].lockCallback.context = s_hFrontend[nUnitId];

#if !defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND)
    DRV_I2c_SetClock( 0, 100);
#endif

    return DI_CH_OK;

}



int DB3461_SetTune( int nChannelId,  HUINT8 *nChecklock, HUINT32 unWorkingId )
{

    int nResult = DI_CH_OK;
    int nRet = DI_CH_OK;
    CH_TER_TuneParam_t *pstTerTuneParam;
    HUINT32 ulFreqKhz = 0;
    int nIsOfdmLocked = 0;
    int nUnitId = 0;

    nUnitId = GetDeviceUnitId( nChannelId );

    CH_UNUSED(unWorkingId);

    pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

    NEXUS_Frontend_GetDefaultOfdmSettings(&s_stOfdmSettings[nUnitId]);

    s_stOfdmSettings[nUnitId].cciMode = NEXUS_FrontendOfdmCciMode_eAuto;
    s_stOfdmSettings[nUnitId].terrestrial = true;
    s_stOfdmSettings[nUnitId].acquisitionMode = NEXUS_FrontendOfdmAcquisitionMode_eAuto;
    s_stOfdmSettings[nUnitId].spectrum = NEXUS_FrontendOfdmSpectrum_eAuto;
    s_stOfdmSettings[nUnitId].lockCallback.callback = lock_callback;
    s_stOfdmSettings[nUnitId].lockCallback.context = s_hFrontend[nUnitId];
    s_stOfdmSettings[nUnitId].asyncStatusReadyCallback.callback = async_status_ready_callback;
    s_stOfdmSettings[nUnitId].asyncStatusReadyCallback.context = s_evStatusEvent[nUnitId];

    CH_DI_Print(CH_LOG_4, ( "etStream(%d)", pstTerTuneParam->etTerSystem ));
    if(pstTerTuneParam->etTerSystem == DI_CH_DVBT2)
    {
        CH_DI_Print(CH_LOG_4, ( ">>>>> DI_CH_DVBT2 with ulPlpId(%d)\n", pstTerTuneParam->stT2Param.ulPlpId));
        s_stOfdmSettings[nUnitId].mode = NEXUS_FrontendOfdmMode_eDvbt2;
        //if(pstTerTuneParam->stT2Param.ulPlpId)
        if((pstTerTuneParam->stT2Param.ulPlpId)&&(pstTerTuneParam->stT2Param.ulPlpId<0xff))
        {
            s_stOfdmSettings[nUnitId].dvbt2Settings.plpMode = FALSE;
            s_stOfdmSettings[nUnitId].dvbt2Settings.plpId = pstTerTuneParam->stT2Param.ulPlpId;
        }
        else
        {
            s_stOfdmSettings[nUnitId].dvbt2Settings.plpMode = TRUE;
        }
    }
    else
    {
        s_stOfdmSettings[nUnitId].mode = NEXUS_FrontendOfdmMode_eDvbt;
        CH_DI_Print(CH_LOG_4, ( ">>>>> DI_CH_DVBT \n"));
    }

    ulFreqKhz = pstTerTuneParam->ulFrequency;
    s_stOfdmSettings[nUnitId].frequency = ulFreqKhz * KHZ_TO_HZ;

    if( pstTerTuneParam->etBandWidth == DI_CH_BW_8MHZ )
    {
        s_stOfdmSettings[nUnitId].bandwidth = NEXUS_FrontendOfdmBandwidth_e8Mhz;
    }
    else if( pstTerTuneParam->etBandWidth == DI_CH_BW_7MHZ )
    {
        s_stOfdmSettings[nUnitId].bandwidth = NEXUS_FrontendOfdmBandwidth_e7Mhz;
    }
    else
    {
        CH_DI_Print(CH_LOG_0, ( "Not Supported BandWidth(%d) \n", pstTerTuneParam->etBandWidth ));
    }

    if( pstTerTuneParam->etStream == DI_CH_STREAM_HP )
    {
        s_stOfdmSettings[nUnitId].priority = NEXUS_FrontendOfdmPriority_eHigh;//HP
    }
    else if( pstTerTuneParam->etStream == DI_CH_STREAM_LP )
    {
        s_stOfdmSettings[nUnitId].priority = NEXUS_FrontendOfdmPriority_eLow;//LP
    }
    else
    {
        CH_DI_Print(CH_LOG_0, ( "Not Supported Stream(%d) \n", pstTerTuneParam->etStream ));
    }
#if defined(FIX_MANUAL_TPS)
    if( (pstTerTuneParam->etTransMode == DI_CH_TRANSMODE_AUTO )\
    ||(pstTerTuneParam->etGuardInterval == DI_CH_GUARDINTERVAL_AUTO ) )
    {
        CH_DI_Print(CH_LOG_4, (">>> Auto mode...\n"));
        s_stOfdmSettings[nUnitId].manualModeSettings = FALSE;
    }
    else
    {
        CH_DI_Print(CH_LOG_4, (">>> Manual mode : etTransMode=%d, etGuardInterval=%d\n", pstTerTuneParam->etTransMode, pstTerTuneParam->etGuardInterval));

        s_stOfdmSettings[nUnitId].acquisitionMode = NEXUS_FrontendOfdmAcquisitionMode_eManual;
        s_stOfdmSettings[nUnitId].manualModeSettings = TRUE;
        s_stOfdmSettings[nUnitId].modeSettings.modeGuard = NEXUS_FrontendOfdmModeGuard_eManual;
        s_stOfdmSettings[nUnitId].modeSettings.mode 				= ConvertBcmFftSize(pstTerTuneParam->etTransMode);
        s_stOfdmSettings[nUnitId].modeSettings.guardInterval 	= ConvertBcmGuardInterval(pstTerTuneParam->etGuardInterval);
    }

    /* TPS setting */
    if( (pstTerTuneParam->etHierarchy == DI_CH_HIERARCHY_AUTO)\
    ||(pstTerTuneParam->etModulation == DI_CH_TMOD_AUTO)\
    ||(pstTerTuneParam->etCodeRate == DI_CH_TCODERATE_AUTO) )
    {
        CH_DI_Print(CH_LOG_4, (">>> Auto TPS...\n"));
        s_stOfdmSettings[nUnitId].manualTpsSettings = FALSE;
    }
    else
    {
        CH_DI_Print(CH_LOG_4, (">>> Manual TPS...\n"));
        s_stOfdmSettings[nUnitId].manualTpsSettings = TRUE;
        s_stOfdmSettings[nUnitId].tpsSettings.highPriorityCodeRate 	= ConvertBcmCodeRate(pstTerTuneParam->etCodeRate);
        s_stOfdmSettings[nUnitId].tpsSettings.lowPriorityCodeRate 	= ConvertBcmCodeRate(pstTerTuneParam->etCodeRate);
        s_stOfdmSettings[nUnitId].tpsSettings.hierarchy 				= ConvertBcmHierarchy(pstTerTuneParam->etHierarchy);
        s_stOfdmSettings[nUnitId].tpsSettings.modulation 			= ConvertBcmModulation(pstTerTuneParam->etModulation);
    }
#else
    s_stOfdmSettings[nUnitId].manualModeSettings = FALSE;
    s_stOfdmSettings[nUnitId].manualTpsSettings = FALSE;
#endif

    s_stOfdmSettings[nUnitId].pullInRange = NEXUS_FrontendOfdmPullInRange_eWide;/* +/-600kHz */
    //s_stOfdmSettings[nUnitId].pullInRange = NEXUS_FrontendOfdmPullInRange_eNarrow;/* +/-200kHz */

    CH_DI_Print(CH_LOG_4, ("s_stOfdmSettings[nUnitId].ifFrequency =%d \n", s_stOfdmSettings[nUnitId].ifFrequency));
    CH_DI_Print(CH_LOG_4,  ("*********************** TuneOfdm [%d] *************************\n", nChannelId));
    CH_DI_Print(CH_LOG_4,  (" manualTpsSettings        [%d] \n", s_stOfdmSettings[nUnitId].manualTpsSettings));
    CH_DI_Print(CH_LOG_4,  (" highPriorityCodeRate     [%d] \n", s_stOfdmSettings[nUnitId].tpsSettings.highPriorityCodeRate));
    CH_DI_Print(CH_LOG_4,  (" lowPriorityCodeRate      [%d] \n", s_stOfdmSettings[nUnitId].tpsSettings.lowPriorityCodeRate));
    CH_DI_Print(CH_LOG_4,  (" hierarchy                [%d] \n", s_stOfdmSettings[nUnitId].tpsSettings.hierarchy));
    CH_DI_Print(CH_LOG_4,  (" modulation               [%d] \n", s_stOfdmSettings[nUnitId].tpsSettings.modulation));
    CH_DI_Print(CH_LOG_4,  ("**************************************************************************\n"));
    CH_DI_Print(CH_LOG_4,  (" manualModeSettings       [%d] \n", s_stOfdmSettings[nUnitId].manualModeSettings));
    CH_DI_Print(CH_LOG_4,  (" modeGuard                [%d] \n", s_stOfdmSettings[nUnitId].modeSettings.modeGuard));
    CH_DI_Print(CH_LOG_4,  (" TransmissionMode         [%d] \n", s_stOfdmSettings[nUnitId].modeSettings.mode));
    CH_DI_Print(CH_LOG_4,  (" guardInterval            [%d] \n", s_stOfdmSettings[nUnitId].modeSettings.guardInterval));
    CH_DI_Print(CH_LOG_4,  ("**************************************************************************\n"));

    nRet = NEXUS_Frontend_TuneOfdm(s_hFrontend[nUnitId], &s_stOfdmSettings[nUnitId]);
    if( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_0, ("Error : 0x%x", nRet));
        nResult = DI_CH_ERR;
    }

    nIsOfdmLocked = WaitOfdmLocked( nChannelId );
    if( nIsOfdmLocked == OFDM_CHECK_CANCELED )
    {
        *nChecklock = OFDM_CHECK_CANCELED;
    }
    else if( nIsOfdmLocked == OFDM_LOCKED )
    {
#if !defined(CONFIG_SUPPORT_ADVANCED_TUNING)
        UpdateTunedOfdmParam( nChannelId, pstTerTuneParam );
#endif
        *nChecklock = OFDM_LOCKED;
    }
    else
    {
        *nChecklock = OFDM_UNLOCKED;
    }

    return nResult;

}



HBOOL DB3461_CheckLock( int nChannelId, HUINT32 ulWorkingId )
{
    CH_UNUSED(nChannelId);CH_UNUSED(ulWorkingId);
    int nUnitId = 0, nRet=0;
    NEXUS_FrontendFastStatus status;

    nUnitId = GetDeviceUnitId( nChannelId );

    CH_DI_Print(CH_LOG_4, ("DB3461_CheckLock[%d] ...S\n", nChannelId));

    nRet = NEXUS_Frontend_GetFastStatus(s_hFrontend[nUnitId], &status);
    if( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_0, ("Error : NEXUS_Frontend_GetFastStatus(0x%x)", nRet));
    }
    if(status.lockStatus == NEXUS_FrontendLockStatus_eLocked)
    {
#if defined(BCM_DEBUG_INFO)
        CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
        CH_DI_Print(CH_LOG_4,  (" status.lockStatus 					   [%d], [%d]:nUnitId\n", status.lockStatus, nUnitId));
        CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
#endif
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



int DB3461_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
    NEXUS_FrontendOfdmStatus stOfdmStatus;
    int nRet = 0;
    int nUnitId = 0;
    CH_TER_TuneParam_t *pstTerTuneParam;
    int rc=0;
    float fBer = 0.0, fSnr = 0.0, fInputPowerDbm =0.0;
    HUINT32		ulUnCorrectedNo =0;
    NEXUS_FrontendDvbt2StatusReady statusReady;
    NEXUS_FrontendDvbt2Status dvbt2Status;
    NEXUS_FrontendFastStatus status;


    nUnitId = GetDeviceUnitId( nChannelId );

    pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

    nRet = NEXUS_Frontend_GetFastStatus(s_hFrontend[nUnitId], &status);
    if( nRet != DI_CH_OK )
    {
        CH_DI_Print(CH_LOG_0, ("Error : NEXUS_Frontend_GetFastStatus(0x%x)", nRet));
    }
    
    if(status.lockStatus == NEXUS_FrontendLockStatus_eLocked)
    {
    	nRet = NEXUS_Frontend_GetOfdmStatus(s_hFrontend[nUnitId], &stOfdmStatus);
    	if( nRet != DI_CH_OK )
    	{
        	CH_DI_Print(CH_LOG_0, ("Error : NEXUS_Frontend_GetOfdmStatus(0x%x)", nRet));
    	}

        CH_DI_Print(CH_LOG_4, ("===Tuner[%d][%s]===================================================================\n", nUnitId, ((pstTerTuneParam->etTerSystem) ? "DVB-T2":"DVB-T")));
        CH_DI_Print(CH_LOG_4, (" signalStrength       =[%d] (1/10 dBm)\n", stOfdmStatus.signalStrength));
        CH_DI_Print(CH_LOG_4, (" signalLevelPercent   =[%d] \n", stOfdmStatus.signalLevelPercent));
        CH_DI_Print(CH_LOG_4, (" signalQualityPercent =[%d] \n", stOfdmStatus.signalQualityPercent));
        CH_DI_Print(CH_LOG_4, ("======================================================================\n"));

        if( pstTerTuneParam->etTerSystem == DI_CH_DVBT )
        {
            fBer = stOfdmStatus.viterbiErrorRate/2147483648.0;
            fSnr = stOfdmStatus.snr/100.0;
            ulUnCorrectedNo = stOfdmStatus.fecUncorrectedBlocks;

            CH_DI_Print(CH_LOG_4, ("===DVB-T===================================================================\n"));
            CH_DI_Print(CH_LOG_4, (" snr=%d\n", stOfdmStatus.snr));
            CH_DI_Print(CH_LOG_4, (" fecUncorrectedBlocks=%ul\n", stOfdmStatus.fecUncorrectedBlocks));
            CH_DI_Print(CH_LOG_4, (" viterbiErrorRate=(%3.2e)\n", stOfdmStatus.viterbiErrorRate/2147483648.0));
            CH_DI_Print(CH_LOG_4, ("======================================================================\n"));
        }
        else
        {
            rc = NEXUS_Frontend_RequestDvbt2AsyncStatus(s_hFrontend[nUnitId], NEXUS_FrontendDvbt2StatusType_eFecStatisticsPlpA);
            if(rc)
            {
                CH_DI_Print(CH_LOG_0, ("Error : NEXUS_Frontend_RequestDvbt2AsyncStatus\n"));
            }

            rc = BKNI_WaitForEvent(s_evStatusEvent[nUnitId], 1000);
            if(rc)
            {
                CH_DI_Print(CH_LOG_0, ("Error : BKNI_WaitForEvent\n"));
            }

            rc =  NEXUS_Frontend_GetDvbt2AsyncStatusReady(s_hFrontend[nUnitId], &statusReady);
            if(rc)
            {
                CH_DI_Print(CH_LOG_0, ("Error : NEXUS_Frontend_GetDvbt2AsyncStatusReady\n"));
            }

            rc = NEXUS_Frontend_GetDvbt2AsyncStatus(s_hFrontend[nUnitId], NEXUS_FrontendDvbt2StatusType_eFecStatisticsPlpA, &dvbt2Status);
            if(rc)
            {
                CH_DI_Print(CH_LOG_0, ("Error : NEXUS_Frontend_GetDvbt2AsyncStatus : NEXUS_FrontendDvbt2StatusType_eFecStatisticsPlpA \n"));
            }
            else
            {
                fSnr = stOfdmStatus.snr/100.0;
                fBer = dvbt2Status.status.fecStatistics.ldpcBER/4294967296.0;// 2^32
                ulUnCorrectedNo = dvbt2Status.status.fecStatistics.bchUncBlk;

                CH_DI_Print(CH_LOG_4, ("===DVB-T2===================================================================\n"));
                CH_DI_Print(CH_LOG_4, (" signalStrength=%d\n", dvbt2Status.status.basic.signalStrength));
                CH_DI_Print(CH_LOG_4, ("===statusReady.type===================================================================\n"));
                CH_DI_Print(CH_LOG_4, (" snr=%d\n", stOfdmStatus.snr));
                CH_DI_Print(CH_LOG_4, (" snrData=%d\n", dvbt2Status.status.fecStatistics.snrData));
                CH_DI_Print(CH_LOG_4, (" bchUncBlk=%ul\n", dvbt2Status.status.fecStatistics.bchUncBlk));
                CH_DI_Print(CH_LOG_4, (" ldpcBER=%ul\n", dvbt2Status.status.fecStatistics.ldpcBER));
                CH_DI_Print(CH_LOG_4, (" ldpcBER=(%3.2e)\n", dvbt2Status.status.fecStatistics.ldpcBER/4294967296.0 ));
                CH_DI_Print(CH_LOG_4, (" fBer   =(%3.2e)\n", fBer ));
                CH_DI_Print(CH_LOG_4, ("======================================================================\n"));
            }
        }

        //pstSignalInfo->ulStrength        = stOfdmStatus.signalLevelPercent;
        //pstSignalInfo->ulQuality         = stOfdmStatus.signalQualityPercent;
        pstSignalInfo->fSnr              = fSnr;
        pstSignalInfo->fBer              = fBer;
        if( pstTerTuneParam->etTerSystem == DI_CH_DVBT )
        {
            fInputPowerDbm                   = stOfdmStatus.signalStrength/10.0 - 40;
        }
        else
        {
            fInputPowerDbm                   = stOfdmStatus.signalStrength/10.0 - 37;
        }
        pstSignalInfo->fSignalInputPower = fInputPowerDbm;
        pstSignalInfo->ulUnCorrectedNo   = ulUnCorrectedNo;
        pstSignalInfo->ulStrength        = GetStrengthDb3461((int)fInputPowerDbm+107);
        pstSignalInfo->ulQuality         = ConvertQualityLevel(pstTerTuneParam->etTerSystem, fSnr);

        CH_DI_Print(CH_LOG_4, ("=== Tuner[%d] ============================================================\n",nUnitId));
        CH_DI_Print(CH_LOG_4, ("fSignalInputPower =[%3.1f]dBm\n", pstSignalInfo->fSignalInputPower));
        CH_DI_Print(CH_LOG_4, ("ulStrength        =[%d]%%\n", pstSignalInfo->ulStrength));
        CH_DI_Print(CH_LOG_4, ("ulQuality         =[%d]%%\n", pstSignalInfo->ulQuality));
        CH_DI_Print(CH_LOG_4, ("fSnr              =[%3.1f]dB\n", pstSignalInfo->fSnr));
        CH_DI_Print(CH_LOG_4, ("fBer              =[%3.2e]\n", pstSignalInfo->fBer));
        CH_DI_Print(CH_LOG_4, ("ulUnCorrectedNo   =[%u]\n", pstSignalInfo->ulUnCorrectedNo));
        CH_DI_Print(CH_LOG_4, ("======================================================================\n"));
    }
    else
    {
        pstSignalInfo->ulQuality = 0;
        pstSignalInfo->ulStrength = 0;
    }

    return nRet;
}



int DB3461_SetPowerState( int nChannelId, CH_Power_e etPower )
{
    int	nResult = DI_CH_OK;

    switch (etPower)
    {
        case CH_POWER_OFF:  //tmPowerStandby = 1
        SetActiveStatus( nChannelId, CH_ACTION_OFF );
        CH_DI_Print(CH_LOG_4, ("\n -------------CH_T_SetPowerState standby mode -------------\n"));
        break;

        case CH_POWER_ON: //tmPowerOn = 0
        SetActiveStatus( nChannelId, CH_ACTION_ON );
        CH_DI_Print(CH_LOG_4, ("\n -------------CH_T_SetPowerState Power on mode -------------\n"));
        break;
    }

    return nResult;
}



int DB3461_EnableTsOutput( int nChannelId )
{
	int nUnitId = 0;
	NEXUS_InputBand		inband;
	NEXUS_InputBandSettings inputBandSettings;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_4, (">>>>>>>>>>>> DB3461_EnableTsOutput !!!!!\n"));

	inband = DRV_TS_PATH_GetInbandID(nUnitId);
	NEXUS_InputBand_GetSettings(inband, &inputBandSettings);

#if defined(CONFIG_TEMP_HDR4000T) 
	if(s_ucHwRev >= 0x10)
	{
		if( inband == NEXUS_InputBand_e3)		// Forced vaiid option
		{
			CH_DI_Print(CH_LOG_4, (" Rev 1.2 - Forced vaiid !!\n"));
			inputBandSettings.validEnabled = false; 	/*forced Valid*/
			inputBandSettings.dataLsbFirst = false; 	/*MSB*/
			inputBandSettings.packetLength = 0xBC; 	/*188 byte*/
		}
		else // Master NEXUS_InputBand_e0 && Master NEXUS_InputBand_e1
		{
			CH_DI_Print(CH_LOG_4, (" Rev 1.2 - Used vaiid !!\n"));
			inputBandSettings.validActiveHigh = true; 	/*active high*/
		}
	}
	else
	{
		if(( inband == NEXUS_InputBand_e1) || ( inband == NEXUS_InputBand_e3))  // Forced vaiid option
		{
			CH_DI_Print(CH_LOG_4, (" Rev 1.1 - Forced vaiid !!\n"));
			inputBandSettings.validEnabled = false; 	/*forced Valid*/
			inputBandSettings.dataLsbFirst = false; 	/*MSB*/
			inputBandSettings.packetLength = 0xBC; 	/*188 byte*/
		}
		else // Master NEXUS_InputBand_e0
		{
			CH_DI_Print(CH_LOG_4, (" Rev 1.1 - Used vaiid !!\n"));
			inputBandSettings.validActiveHigh = true; 	/*active high*/
		}
	}
#else
	inputBandSettings.validActiveHigh = true; 	/*active high*/
#endif

	NEXUS_InputBand_SetSettings(inband, &inputBandSettings);

	return DI_CH_OK;
}



int DB3461_DisableTsOutput( int nChannelId )
{
	int nUnitId = 0;
	NEXUS_InputBand		inband;
	NEXUS_InputBandSettings inputBandSettings;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_4, (">>>>>>>>>>>> DB3461_DisableTsOutput !!!!!\n"));
	//NEXUS_Frontend_Untune(s_hFrontend[nUnitId]);
	inband = DRV_TS_PATH_GetInbandID(nUnitId);
	NEXUS_InputBand_GetSettings(inband, &inputBandSettings);

#if defined(CONFIG_TEMP_HDR4000T)
	if(s_ucHwRev >= 0x10)
	{
		if(inband == NEXUS_InputBand_e3) // Forced valid option
		{
			CH_DI_Print(CH_LOG_4, (" Rev 1.2 - Forced vaiid !!\n"));
			inputBandSettings.dataLsbFirst = true; 		/*LSB*/
			inputBandSettings.packetLength = 0x0; 		/*0 byte*/    
			inputBandSettings.validEnabled = true; 		/*Use input valid signal*/
			inputBandSettings.validActiveHigh = true; 	/*active high*/
		}
		else // Master NEXUS_InputBand_e0
		{
			CH_DI_Print(CH_LOG_4, (" Rev 1.2 - Used vaiid !!\n"));
			inputBandSettings.validActiveHigh = false; 	/*active Low*/
		}
	}
	else
	{
		if(( inband == NEXUS_InputBand_e1) || ( inband == NEXUS_InputBand_e3)) // Forced valid option
		{
			CH_DI_Print(CH_LOG_4, (" Rev 1.1 - Forced vaiid !!\n"));
			inputBandSettings.dataLsbFirst = true; 		/*LSB*/
			inputBandSettings.packetLength = 0x0; 		/*0 byte*/    
			inputBandSettings.validEnabled = true; 		/*Use input valid signal*/
			inputBandSettings.validActiveHigh = true; 	/*active high*/
		}
		else // Master NEXUS_InputBand_e0
		{
			CH_DI_Print(CH_LOG_4, (" Rev 1.1 - Used vaiid !!\n"));
			inputBandSettings.validActiveHigh = false; 	/*active Low*/
		}
	}
#else
	inputBandSettings.validActiveHigh = false;
#endif

	NEXUS_InputBand_SetSettings(inband, &inputBandSettings);

	return DI_CH_OK;
}



int DB3461_SetTSOutputMode(int nChannelId , CH_TsOutMode_e OutputMode)
{
    //Deprecated...
    CH_UNUSED(nChannelId);
    CH_UNUSED(OutputMode);
    return DI_CH_OK;
}



int DB3461_CheckAntState( int nChannelId )
{
    int nUnitId;
    int nRet=0;
    GPIO_STATE_t	gpioStat_t = GPIO_INVAILD;

    nUnitId = GetDeviceUnitId( nChannelId );

    if( s_stAntState[nUnitId].etDesiredAntState == DI_CH_ANT_POWER_ON )
    {
        nRet = DRV_GPIO_Read(GPIO_ID_ANT_PWR_CHK, &gpioStat_t);
        if(nRet < 0 )
        {
            CH_DI_Print(CH_LOG_0, ("DP10048_CheckAntState GPIO Read Error\n"));
            return DI_CH_ERR;
        }

        if( gpioStat_t == GPIO_LOW )  //if( gpioStat_t == GPIO_HI )
        {
            CH_DI_Print(CH_LOG_4, (">>>>>>>>>>>>Antenna short circuit\n"));
            s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANT_POWER_FAULT;
            return DI_CH_ANT_POWER_FAULT;
        }
        else if( gpioStat_t == GPIO_HI ) //else if( gpioStat_t == GPIO_LOW )
        {
            if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANT_POWER_FAULT )
            {
                nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_HI);
                if( nRet < 0 )
                {
                    CH_DI_Print(CH_LOG_0, ("DP10048_CheckAntState Gpio Write Error\n"));
                }
            }
            return DI_CH_OK;
        }
        else
        {
            CH_DI_Print(CH_LOG_0, ("Err!! DP10048_CheckAntState\n"));
            return DI_CH_ERR;
        }
    }
    else // DI_CH_ANT_POWER_OFF, DI_CH_ANT_POWER_FAULT
    {
        return DI_CH_OK;
    }
}


int DB3461_SetAntState( int nChannelId, CH_TER_AntStatus_e etAntState)
{
	int nUnitId;
	int nRet=0;
	HUINT32 unWorkingId;
	int nActiveState;

	nUnitId = GetDeviceUnitId( nChannelId );
	unWorkingId = GetRequestedId( nChannelId );

	s_stAntState[nUnitId].etDesiredAntState = etAntState;

	if( s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANT_POWER_FAULT )
	{
		if(etAntState < DI_CH_ANT_POWER_FAULT)
		{
			nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, etAntState);
			if( nRet < 0 )
			{
				CH_DI_Print(CH_LOG_0, ("DP10048_SetAntState Write fail\n"));
				return DI_CH_ERR;
			}
		}
		else
		{
			CH_DI_Print(CH_LOG_0, ("DP10048_SetAntState Unknown State\n"));
		}
		s_stAntState[nUnitId].etCurrentAntState = etAntState;
	}
	else if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANT_POWER_FAULT )
	{
		if( etAntState == DI_CH_ANT_POWER_OFF )
		{
			nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, etAntState);

			if( nRet < 0 )
			{
				CH_DI_Print(CH_LOG_0, ("DP10048_SetAntState Write fail\n"));
				return DI_CH_ERR;
			}

			s_stAntState[nUnitId].etCurrentAntState = etAntState;

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
		else
		{
			return DI_CH_HW_RESTRICTION;
		}
	}

	return DI_CH_OK;
}



CH_TER_AntStatus_e DB3461_GetAntState( int nChannelId )
{
	int nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_4, (">>>>>>>>>>>> DB3461_GetAntState !!!!!\n"));

	return s_stAntState[nUnitId].etCurrentAntState;
}



void DB3461_ProcessRecoverAntenna( int nChannelId , HUINT32 unWorkingId)
{
	int nRet=0;
	int nUnitId;
	int nActiveState;
	GPIO_STATE_t	gpioStat_t = GPIO_INVAILD;

	nUnitId = GetDeviceUnitId( nChannelId );


	nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_HI);
	if( nRet < 0 )
	{
		CH_DI_Print(CH_LOG_0, ("DP10048_ProcessRecoverAntenna Write HI fail\n"));

	}
	VK_TASK_Sleep(100);// wait 100ms

	nRet = DRV_GPIO_Read(GPIO_ID_ANT_PWR_CHK, &gpioStat_t);
	if( nRet < 0 )
	{
		CH_DI_Print(CH_LOG_0, ("DP10048_ProcessRecoverAntenna Read fail\n"));
		return;
	}

	if( gpioStat_t == GPIO_LOW ) //if( gpioStat_t == GPIO_HI )
	{
		CH_DI_Print(CH_LOG_4, ("Antenna continue fault... Turn 5 Volt Off\n"));
		nRet = DRV_GPIO_Write(GPIO_ID_ANT_PWR_EN, GPIO_LOW);
		if( nRet < 0 )
		{
			CH_DI_Print(CH_LOG_0, ("DP10048_ProcessRecoverAntenna Write LOW fail\n"));
		}

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT);
		//s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANT_POWER_OFF;
	}
	else if( gpioStat_t == GPIO_HI ) //else if( gpioStat_t == GPIO_LOW )
	{
		CH_DI_Print(CH_LOG_4, ("Antenna recovered...\n"));

		s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANT_POWER_ON;

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
	else
	{
		CH_DI_Print(CH_LOG_0, ("DP10048_ProcessRecoverAntenna Read Unknown (%d)\n", gpioStat_t));
	}

	return;

}



int DB3461_GetPlpCount( int nChannelId, HUINT8 * pucNumPlps )
{
	NEXUS_FrontendOfdmStatus stOfdmStatus;
	int i=0, nNumPlp=0, nNumDataPlp=0;
	int nUnitId = 0;
	int nRet=0;

	if(pucNumPlps == NULL)
	{
		return DI_CH_PARAM_ERR;
	}

	nUnitId = GetDeviceUnitId( nChannelId );
	nRet = NEXUS_Frontend_GetOfdmStatus(s_hFrontend[nUnitId], &stOfdmStatus);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_0, ("Error : NEXUS_Frontend_GetOfdmStatus(0x%x)", nRet));
	}

	//if(pstTerTuneParam->etTerSystem == DI_CH_DVBT2)
	{
		nNumPlp = stOfdmStatus.dvbt2Status.l1PlpStatus.numPlp;

		CH_DI_Print(CH_LOG_4,  ("***********************   DVB-T2 PLP Info.   *************************\n"));
		CH_DI_Print(CH_LOG_4,  (" numPlp               [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.numPlp));
		for(i=0;i<nNumPlp;i++)
		{
			CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
			CH_DI_Print(CH_LOG_4,  (" plpId                [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpId));
			CH_DI_Print(CH_LOG_4,  (" plpGroupId           [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpGroupId));
			CH_DI_Print(CH_LOG_4,  (" plpPayloadType       [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpPayloadType));
			CH_DI_Print(CH_LOG_4,  (" plpType              [%d] \n", stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpType));
#if (NEXUS_VERSION >= 1502)
			if(stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpType != NEXUS_FrontendDvbt2PlpType_eCommon)
#else			
			if(stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpType != NEXUS_FrontendOfdmPlpType_eCommon)
#endif				
			{
				s_aucPlpID[nChannelId][nNumDataPlp] = stOfdmStatus.dvbt2Status.l1PlpStatus.plp[i].plpId;
				nNumDataPlp++;
			}
		}
		CH_DI_Print(CH_LOG_4,  (" nNumDataPlp               [%d] \n", nNumDataPlp));
		s_ucNumberPlp[nChannelId] = nNumDataPlp;
		CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
	}

	*pucNumPlps = s_ucNumberPlp[nChannelId];
	return DI_CH_OK;
}



int DB3461_GetPlpInfo( int nChannelId, HUINT8 * pucPlpIds, HUINT8 * pucNumPlps )
{
	int nResult = DI_CH_OK;
	int	nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	if((pucPlpIds == NULL)||(pucNumPlps == NULL))
	{
		return DI_CH_PARAM_ERR;
	}

	if( s_ucNumberPlp[nUnitId] < MAX_NUM_PLPS )
	{
		VK_MEM_Memcpy( pucPlpIds, s_aucPlpID[nUnitId], s_ucNumberPlp[nUnitId] );
		*pucNumPlps = s_ucNumberPlp[nUnitId];
	}
	else
	{
		CH_DI_Print(CH_LOG_0, (" s_ucNumberPlp (%d) ", s_ucNumberPlp[nUnitId]));
		nResult = DI_CH_ERR;
	}

	return nResult;
}



int DB3461_GetCellId(int nChannelId, HUINT16 *pusCellId)
{
	int nResult = DI_CH_OK;
	int			nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	*pusCellId = s_usCellId[nUnitId];

	CH_DI_Print(CH_LOG_4,("DEMOD[%d] : *pusCellId : (0x%x)\n", nUnitId, *pusCellId));

	return nResult;
}


int DB3461_GetLockedParam(int nChannelId, CH_TER_TuneParam_t * pstTerTuneParam)
{
	int nRet=0;
	int nUnitId = 0;
	CH_TER_TuneParam_t * pvSource;

	NEXUS_FrontendOfdmStatus stOfdmStatus;

	nUnitId = GetDeviceUnitId( nChannelId );

	pvSource = GetWorkingTuneInfo( nChannelId );
	VK_memcpy( pstTerTuneParam, pvSource, sizeof(CH_TER_TuneParam_t));

	nRet = NEXUS_Frontend_GetOfdmStatus(s_hFrontend[nUnitId], &stOfdmStatus);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ("Error : NEXUS_Frontend_GetOfdmStatus(0x%x)", nRet));
	}

	CH_DI_Print(CH_LOG_4,  ("*********************** DB3461_GetLockedParam[%d] *************************\n", nChannelId));
	CH_DI_Print(CH_LOG_4,  (" modulation             [%d] \n", stOfdmStatus.modulation));
	CH_DI_Print(CH_LOG_4,  (" transmissionMode       [%d] \n", stOfdmStatus.transmissionMode));
	CH_DI_Print(CH_LOG_4,  (" guardInterval          [%d] \n", stOfdmStatus.guardInterval));
	CH_DI_Print(CH_LOG_4,  (" codeRate               [%d] \n", stOfdmStatus.codeRate));
	CH_DI_Print(CH_LOG_4,  (" hierarchy              [%d] \n", stOfdmStatus.hierarchy));
	CH_DI_Print(CH_LOG_4,  ("**************************************************************************\n"));

	pstTerTuneParam->etModulation 		= ConvertDiModulation( stOfdmStatus.modulation );
	pstTerTuneParam->etTransMode 		= ConvertDiFftSize( stOfdmStatus.transmissionMode );
	pstTerTuneParam->etGuardInterval 	= ConvertDiGuardInterval( stOfdmStatus.guardInterval );
	pstTerTuneParam->etCodeRate 		= ConvertDiCodeRate( stOfdmStatus.codeRate );
	pstTerTuneParam->etHierarchy 		= ConvertDiHierarchy( stOfdmStatus.hierarchy );

	s_usCellId[nUnitId] = (HUINT16)stOfdmStatus.cellId;

      CH_DI_Print(CH_LOG_4,  (" s_usCellId[%d                [%d] \n", nUnitId, s_usCellId[nUnitId]));

	if(pstTerTuneParam->etTerSystem == DI_CH_DVBT2)
	{
		CH_DI_Print(CH_LOG_4,  (" plpId                [%d] \n", pstTerTuneParam->stT2Param.ulPlpId));
		CH_DI_Print(CH_LOG_4,  ("**********************************************************************\n"));
	}

	return nRet;
}

/*********************** End of File ******************************/
