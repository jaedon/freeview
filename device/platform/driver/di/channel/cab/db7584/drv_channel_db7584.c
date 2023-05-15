/*************************************************************************************
 *
 *	DB7584.c
 *
 *	Copyright (c) 2014 by Humax	Corp.
 *
 *
 *************************************************************************************/
#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_c.h"
#include "di_channel_attr.h"

#include "drv_err.h"
//#include "drv_i2c.h"
//#include "drv_gpio.h"

#include "drv_channel_c.h"
#include "drv_channel_db7584.h"
#include "nexus_frontend.h"
#include "nexus_platform.h"

#include <string.h>

#include "bkni.h"

/****************************************************
	Define
******************************************************/
#define	WAIT_QAM_LOCKING		(100) //(50)//(100)
#define	LOCK_TRIAL_COUNT		(10) // (4)

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
	HINT32			netId;
	HINT32			tsId;
	HUINT8			tsmfStatus;
	HUINT8			emergencyFlag;
	BKNI_EventHandle	hStatusEvent;
}DB7584_SignalQuality_t;


/****************************************************
	Static Variable
******************************************************/
static NEXUS_FrontendHandle s_st7584Frontend[NUM_OF_CHANNEL];
static HUINT8	s_ucIsDb7584Lock[NUM_OF_CHANNEL];
static DB7584_SignalQuality_t s_stDb7584SignalStatus[NUM_OF_CHANNEL];

/****************************************************
	Extern Function
******************************************************/
static void P_async_status_ready_callback(void *context, int param)
{
	CH_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle)context);
}

static HUINT32 P_ConvertStrengthLevel(HINT32 nChannelId, HFLOAT32 fRfInputLevel)
{
	HFLOAT32 fStrength = 0;
	HINT32 nTmpRfLevel = 0;

	nTmpRfLevel = fRfInputLevel * 100.0;

	/*
		Strength table
		dBuV	/ %
		x > 75 		/ 100
		65 < x < 75	/ 90
		60 < x < 65	/ 80
		57 < x < 60	/ 70
		54 < x < 57	/ 60
		52 < x < 54	/ 50
		50 < x < 52	/ 40
		48 < x < 50	/ 30
		46 < x < 48	/ 20
		44 < x < 46	/ 10
		x < 44		/ 0
	*/

	if (nTmpRfLevel >= 7500)
	{
		fStrength = 100;
	}
	else if (nTmpRfLevel >= 6500)
	{
		fStrength = 90 + (10.0 * (nTmpRfLevel - 6500)/(7500 - 6500));
	}
	else if (nTmpRfLevel >= 6000)
	{
		fStrength = 80 + (10.0 * (nTmpRfLevel - 6000)/(6500 - 6000));
	}
	else if (nTmpRfLevel >= 5700)
	{
		fStrength = 70 + (10.0 * (nTmpRfLevel - 5700)/(6000 - 5700));
	}
	else if (nTmpRfLevel >= 5400)
	{
		fStrength = 60 + (10.0 * (nTmpRfLevel - 5400)/(5700 - 5400));
	}
	else if (nTmpRfLevel >= 5200)
	{
		fStrength = 50 + (10.0 * (nTmpRfLevel - 5200)/(5400 - 5200));
	}
	else if (nTmpRfLevel >= 5000)
	{
		fStrength = 40 + (10.0 * (nTmpRfLevel - 5000)/(5200 - 5000));
	}
	else if (nTmpRfLevel >= 4800)
	{
		fStrength = 30 + (10.0 * (nTmpRfLevel - 4800)/(5000 - 4800));
	}
	else if (nTmpRfLevel >= 4600)
	{
		fStrength = 20 + (10.0 * (nTmpRfLevel - 4600)/(4800 - 4600));
	}
	else if (nTmpRfLevel >= 4400)
	{
		fStrength = 10 + (10.0 * (nTmpRfLevel - 4400)/(4600 - 4400));
	}
	else
	{
		fStrength = 0;
	}

	CH_DI_Print(3, ("[%s] : fStrength [%d] = %d\n",__FUNCTION__, nChannelId, (HUINT32)fStrength));

	return (HUINT32)fStrength;
}

static HUINT32 P_ConvertQualityLevel(HINT32 nChannelId, HUINT32 ulSnr)
{
	HUINT32 ulQuality = 0;

	ulQuality = ulSnr/10 - 230;
	if (ulQuality >= 100)
	{
		ulQuality = 100;
	}
	else if (ulQuality <= 0)
	{
		ulQuality = 0;
	}

	CH_DI_Print(3, ("[%s] : fQuality [%d] = %d\n",__FUNCTION__,nChannelId, ulQuality));

	return ulQuality;
}

static HBOOL WaitQamLocked(int nChannelId)
{
	NEXUS_FrontendQamStatus stQamStatus;
	HUINT8 usLock = FALSE;
	int nUnitId=0, nCount=0;

	nUnitId = GetDeviceUnitId( nChannelId );

	while(usLock != TRUE)
	{
		NEXUS_Frontend_GetQamStatus(s_st7584Frontend[nUnitId], &stQamStatus);

		if ( (stQamStatus.fecLock == 1 )&&(stQamStatus.receiverLock == 1))
        //        if ( stQamStatus.receiverLock == 1 )
		{
			usLock = TRUE;
		}
		else
		{
			usLock = FALSE;
		}

		if(usLock == TRUE)
		{
			s_ucIsDb7584Lock[nUnitId] = TRUE;
			CH_DI_Print(0, ("WaitQamLocked[%d] : Locked!(nCount:%d) fec(%d)\n", nUnitId, nCount, stQamStatus.fecLock));
			return TRUE;
		}
		else
		{
			VK_TASK_Sleep(WAIT_QAM_LOCKING);
			if(nCount == LOCK_TRIAL_COUNT)
			{
				s_ucIsDb7584Lock[nUnitId] = FALSE;
				CH_DI_Print(1, ("WaitQamLocked[%d] : Lock trial expired fec(%d)\n", nUnitId, stQamStatus.fecLock));
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


#if __COMNET__
#define	_____API_____
#endif

void DRV_C_InstallApi( void )
{
	CAB_InitDevice 			= &DB7584_InitDevice;
	CAB_SetTune 			= &DB7584_SetTune;
	CAB_CheckLock 			= &DB7584_CheckLock;
	CAB_GetStrengthQuality 	= &DB7584_GetStrengthQuality;
	CAB_SetPowerState 		= &DB7584_SetPowerState;
	CAB_EnableTsOutput		= &DB7584_EnableTsOutput;
	CAB_DisableTsOutput		= &DB7584_DisableTsOutput;
	CAB_ResetGpio 			= &DB7584_ResetGpio;
	CAB_Initialize			= &DB7584_Initialize;
	CAB_ReInitDevice		= &DB7584_ReInitDevice;

	CH_DI_Print(5, ("DRV_C_InstallApi()  OK! \n"));

	return;
}

int DB7584_InitDevice(int nChannelId)
{
	 int nUnitId;
	 HINT32 nRet = DI_CH_OK;
	 NEXUS_PlatformConfiguration platformConfig;
	 NEXUS_FrontendAcquireSettings settings;

	 nUnitId = GetDeviceUnitId( nChannelId );

	 NEXUS_Platform_GetConfiguration(&platformConfig);
	 if(platformConfig.frontend[nUnitId] != NULL)
	 {
		s_st7584Frontend[nUnitId] = platformConfig.frontend[nUnitId];
	 }
	 else
	 {
		nRet = NEXUS_Platform_InitFrontend();
		if(nRet == DI_CH_OK)
		{
			NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
			settings.mode = NEXUS_FrontendAcquireMode_eByIndex;
        		settings.index = nUnitId;
        		s_st7584Frontend[nUnitId] = NEXUS_Frontend_Acquire(&settings);
		}        	
	 }

	 if (NULL == s_st7584Frontend[nUnitId] )
	 {
		 CH_DI_Print(0, ("[DB7584_InitDevice] ERR! Unable to find CABLE capable frontend\n"));
		 return DI_CH_ERR;
	 }

	BKNI_CreateEvent(&(s_stDb7584SignalStatus[nChannelId].hStatusEvent));

	 return DI_CH_OK;
 }

int  DB7584_ResetGpio(int nChannelId)
{
	HUINT32 	nUnitId = 0;

	CH_DI_Print(3, ("[DB7584_ResetGpio[%d]] ----\n",nChannelId));

	nUnitId = GetDeviceUnitId( nChannelId );

	return DI_CH_OK;
}

int DB7584_SetTune(int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
	HINT32 nUnitId = 0, nRet = DI_CH_OK, nResult = DI_CH_OK;
	NEXUS_FrontendQamSettings qamSettings;

	CH_UNUSED(ulWorkingId);
	nUnitId = GetDeviceUnitId( nChannelId );

	 CH_DI_Print(0,("DB7584_SetTune[%d][%dMHz]\n", nChannelId,  pstCabTuneParam->ulFrequency/1000));

	NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
	qamSettings.frequency = pstCabTuneParam->ulFrequency * KHZ_TO_HZ;
	qamSettings.annex = NEXUS_FrontendQamAnnex_eA;
	qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e8Mhz;
#if 0
	qamSettings.spectrumMode = NEXUS_FrontendQamSpectrumMode_eManual;
	qamSettings.frequencyOffset = NEXUS_FrontendQamFrequencyOffset_e180Khz;
#endif		

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

	qamSettings.symbolRate = pstCabTuneParam->ulSymbolRate * KHZ_TO_HZ;

	qamSettings.autoAcquire = true; // use NEXUS to auto acquire if loss of lock
	//qamSettings[nUnitId].lockCallback.callback = P_lock_callback;
	//qamSettings[nUnitId].lockCallback.context = s_stDb7584SignalStatus[nChannelId].hlockEvent;
	qamSettings.asyncStatusReadyCallback.callback = P_async_status_ready_callback;
	qamSettings.asyncStatusReadyCallback.context = s_stDb7584SignalStatus[nChannelId].hStatusEvent;

	nRet = NEXUS_Frontend_TuneQam(s_st7584Frontend[nUnitId], &qamSettings);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(0,("DB7584_SetTune[%d] :: NEXUS_Frontend_TuneQam.. Fail.. nRet(0x%x)", nUnitId, nRet));
		nResult = DI_CH_ERR;
	}
	
	nRet = WaitQamLocked( nChannelId );
	if( nRet == TRUE )
	{
		CH_DI_Print(0,("\n\n\n-------------------------------------------------------\n"));
		CH_DI_Print(0,("DB7584_SetTune[%d][%d] :: Locked!!! TSID(0x%x), NETID(0x%x) \n",nUnitId, pstCabTuneParam->ulFrequency/1000 ,s_stDb7584SignalStatus[nChannelId].tsId, s_stDb7584SignalStatus[nChannelId].netId));
                CH_DI_Print(0,("---------------------------------------------------------\n\n\n"));

//		DB7584_CheckLock(nChannelId);
		s_ucIsDb7584Lock[nUnitId] = TRUE;
		nResult = DI_CH_OK;
	}
	else
	{
		CH_DI_Print(0,("DB7584_SetTune[%d] :: Unlocked \n",nUnitId));
		s_ucIsDb7584Lock[nUnitId] = FALSE;
		nResult = DI_CH_ERR;
	}

	return nResult;
}

HBOOL DB7584_CheckLock(int nChannelId)
{
	HINT32 nUnitId = 0;
	NEXUS_Error nResult;

	NEXUS_FrontendQamStatus stQamStatus;
	HFLOAT32	fSignalPowerdBuV = 0;
	HBOOL	Locked=0;
	HINT32  nQuality=0, nStrength=0;

	nUnitId = GetDeviceUnitId( nChannelId );

#if 1
	nResult = NEXUS_Frontend_RequestQamAsyncStatus(s_st7584Frontend[nUnitId]);
	if(nResult == NEXUS_SUCCESS)
	{
		nResult = BKNI_WaitForEvent(s_stDb7584SignalStatus[nChannelId].hStatusEvent, 1000);
		if (nResult)
		{
			CH_DI_Print(0,("%s[%d] BKNI_WaitForEvent() Status not returned\n",__FUNCTION__, nUnitId));
		}
		NEXUS_Frontend_GetQamAsyncStatus(s_st7584Frontend[nUnitId] , &stQamStatus);
	}
	else
	{
		CH_DI_Print(0,("%s[%d] NEXUS_Frontend_RequestQamAsyncStatus return error, Call NEXUS_Frontend_GetQamStatus \n",__FUNCTION__, nUnitId));
		NEXUS_Frontend_GetQamStatus(s_st7584Frontend[nUnitId], &stQamStatus);
	}
#else
	NEXUS_Frontend_GetQamStatus(s_st7584Frontend[nUnitId], &stQamStatus);
#endif

	Locked = stQamStatus.receiverLock;
	CH_DI_Print(1,("DB7584_CheckLock[%d]... NEXUS_Frontend_GetQamStatus: %d\n",nChannelId, Locked));

	if(Locked)
	{
		CH_DI_Print(1,("DB7584_CheckLock[%d] receiverLock=%d, fecLock=%d, CB=%d, UCB=%d\n",nChannelId, stQamStatus.receiverLock, stQamStatus.fecLock,stQamStatus.fecCorrected, stQamStatus.fecUncorrected));

#if 1
		fSignalPowerdBuV = (float)(stQamStatus.dsChannelPower/10.0)+60;
#else
		fSignalPowerdBuV = (float)(stQamStatus.equalizerGain/1000.0);
#endif

#if 1
		CH_DI_Print(6,("DB7584_CheckLock[%d] : Locked \n",nChannelId));
		CH_DI_Print(6,("====================== DB7584_CheckLock[%d] ======================\n",nChannelId));
		CH_DI_Print(6,("  freq			  = %d [MHz]\n",stQamStatus.settings.frequency / 1000000));
		CH_DI_Print(6,("  receiverLock             = %u\n", stQamStatus.receiverLock));
		CH_DI_Print(6,("  fecLock                  = %u\n", stQamStatus.fecLock));

		CH_DI_Print(6,("  symbolRate               = %u [Baud]\n", stQamStatus.symbolRate));
		CH_DI_Print(6,("  symbolRateError          = %d [Baud]\n", stQamStatus.symbolRateError));

//		CH_DI_Print(6,("  lnaAgcLevel              = %2.1f [%%]\n", stQamStatus.lnaAgcLevel/10.0));
		CH_DI_Print(6,("  ifAgcLevel               = %2.1f [%%]\n", stQamStatus.ifAgcLevel/10.0));
//		CH_DI_Print(6,("  rfAgcLevel               = %2.1f [%%]\n", stQamStatus.rfAgcLevel/10.0));
//		CH_DI_Print(6,("  intAgcLevel              = %2.1f [%%]\n", stQamStatus.intAgcLevel/10.0));
		CH_DI_Print(6,("  snrEstimate              = %2.2f [dB]\n", stQamStatus.snrEstimate/100.0));

		CH_DI_Print(6,("  fecUnerrored         = %u\n", stQamStatus.goodRsBlockCount));
		CH_DI_Print(6,("  fecCorrected             = %u\n", stQamStatus.fecCorrected));
		CH_DI_Print(6,("  fecUncorrected           = %u\n", stQamStatus.fecUncorrected));
		CH_DI_Print(6,("  berEstimate              = %3.2e\n", stQamStatus.postRsBer/1000000.0));

		CH_DI_Print(6,("  dsChannelPower           = %3.2f [dBuV]\n", fSignalPowerdBuV));
		CH_DI_Print(6,("=================================================================\n"));


#else
		CH_DI_Print(6,("DB7584_CheckLock[%d] : Locked \n",nChannelId));
		CH_DI_Print(6,("====================== DB7584_CheckLock[%d] ======================\n",nChannelId));
		CH_DI_Print(6,("  receiverLock             = %u\n", stQamStatus.receiverLock));
		CH_DI_Print(6,("  fecLock                  = %u\n", stQamStatus.fecLock));
		CH_DI_Print(6,("  opllLock                 = %u\n", stQamStatus.opllLock));
		CH_DI_Print(6,("  spectrumInverted         = %u\n", stQamStatus.spectrumInverted));

		CH_DI_Print(6,("  symbolRate               = %u [Baud]\n", stQamStatus.symbolRate));
		CH_DI_Print(6,("  symbolRateError          = %d [Baud]\n", stQamStatus.symbolRateError));
		CH_DI_Print(6,("  berEstimate              = %d\n", stQamStatus.berEstimate));

		CH_DI_Print(6,("  ifAgcLevel               = %2.1f [%%]\n", stQamStatus.ifAgcLevel/10.0));
		CH_DI_Print(6,("  rfAgcLevel               = %2.1f [%%]\n", stQamStatus.rfAgcLevel/10.0));
		CH_DI_Print(6,("  intAgcLevel              = %2.1f [%%]\n", stQamStatus.intAgcLevel/10.0));
		CH_DI_Print(6,("  snrEstimate              = %2.2f [dB]\n", stQamStatus.snrEstimate/100.0));

		CH_DI_Print(6,("  fecCorrected             = %u\n", stQamStatus.fecCorrected));
		CH_DI_Print(6,("  fecUncorrected           = %u\n", stQamStatus.fecUncorrected));
		CH_DI_Print(6,("  fecClean                 = %u\n", stQamStatus.fecClean));
		CH_DI_Print(6,("  bitErrCorrected          = %u\n", stQamStatus.bitErrCorrected));
		CH_DI_Print(6,("  reacquireCount           = %u\n", stQamStatus.reacquireCount));

		CH_DI_Print(6,("  PreRsBer                 = %3.2e\n", fPreRsBer));
		CH_DI_Print(6,("  PostRsBer                = %3.2e\n", fPostRsBer));

		CH_DI_Print(6,("  viterbiUncorrectedBits   = %u\n", stQamStatus.viterbiUncorrectedBits));
		CH_DI_Print(6,("  viterbiTotalBits         = %u\n", stQamStatus.viterbiTotalBits));
		CH_DI_Print(6,("  viterbiErrorRate         = %3.2e\n", stQamStatus.viterbiErrorRate));

		CH_DI_Print(6,("  carrierFreqOffset        = %.3f [Hz]\n", stQamStatus.carrierFreqOffset/1000.0));
		CH_DI_Print(6,("  carrierPhaseOffset       = %.3f [Hz]\n", stQamStatus.carrierPhaseOffset/1000.0));

		CH_DI_Print(6,("  goodRsBlockCount         = %u\n", stQamStatus.goodRsBlockCount));
		CH_DI_Print(6,("  berRawCount              = %u\n", stQamStatus.berRawCount));

		CH_DI_Print(6,("  dsChannelPower           = %3.2f [dBuV]\n", fSignalPowerdBuV));
		CH_DI_Print(6,("  mainTap                  = %u\n", stQamStatus.mainTap));
		CH_DI_Print(6,("  equalizerGain            = %u [dBm]\n", stQamStatus.equalizerGain));
		CH_DI_Print(6,("  postRsBer                = %u\n", stQamStatus.postRsBer));
		CH_DI_Print(6,("  postRsBerElapsedTime     = %u [s]\n", stQamStatus.postRsBerElapsedTime));
		CH_DI_Print(6,("  interleaveDepth          = %u\n", stQamStatus.interleaveDepth));
		CH_DI_Print(6,("  lnaAgcLevel              = %2.1f [%%]\n", stQamStatus.lnaAgcLevel/10.0));
		CH_DI_Print(6,("=================================================================\n"));
#endif

#if 1
		nStrength = P_ConvertStrengthLevel(nChannelId,fSignalPowerdBuV);
		nQuality = P_ConvertQualityLevel(nChannelId,stQamStatus.snrEstimate);
#else
/* TODO_JDCHO Strength & Qaulity */
		nQuality = stQamStatus.snrEstimate/10 - 230;
		if (nQuality >=100)
		{
			nQuality = 100;
		}
		else if (nQuality <= 0)
		{
			nQuality = 0;
		}

		nStrength = 93 - stQamStatus.ifAgcLevel/10;

		if (nStrength >= 80)
		{
			nStrength = 100;
		}
		else if (nStrength <= 10)
		{
			nStrength = 10;
		}
#endif

		s_stDb7584SignalStatus[nChannelId].nStrength = (HUINT32)nStrength;
		s_stDb7584SignalStatus[nChannelId].ulQuality = (HUINT32)nQuality;
		s_stDb7584SignalStatus[nChannelId].ucAgc = 0;		// Ignore

		s_stDb7584SignalStatus[nChannelId].ulBer = stQamStatus.postRsBer;
		s_stDb7584SignalStatus[nChannelId].ulSnr = stQamStatus.snrEstimate;
		s_stDb7584SignalStatus[nChannelId].ucRfAgc = stQamStatus.rfAgcLevel/10;
		s_stDb7584SignalStatus[nChannelId].ucIfAgc = stQamStatus.ifAgcLevel/10;
		s_stDb7584SignalStatus[nChannelId].ulCorrectedNo = stQamStatus.fecCorrected;
		s_stDb7584SignalStatus[nChannelId].ulUnCorrectedNo = stQamStatus.fecUncorrected;
		s_stDb7584SignalStatus[nChannelId].ulUnErroredNo = stQamStatus.goodRsBlockCount ;
		s_stDb7584SignalStatus[nChannelId].fSignalPower = fSignalPowerdBuV;
		s_ucIsDb7584Lock[nUnitId] = TRUE;

		return TRUE;
	}
	else
	{
		s_stDb7584SignalStatus[nChannelId].nStrength = 0;
		s_stDb7584SignalStatus[nChannelId].ulQuality = 0;
		s_stDb7584SignalStatus[nChannelId].ucAgc = 0;		// Ignore

		s_stDb7584SignalStatus[nChannelId].ulBer = 0;
		s_stDb7584SignalStatus[nChannelId].ulSnr = 0;
		s_stDb7584SignalStatus[nChannelId].ucRfAgc = 0;
		s_stDb7584SignalStatus[nChannelId].ucIfAgc = 0;
		s_stDb7584SignalStatus[nChannelId].ulCorrectedNo = 0;
		s_stDb7584SignalStatus[nChannelId].ulUnCorrectedNo = 0;
		s_stDb7584SignalStatus[nChannelId].ulUnErroredNo = 0;
		s_stDb7584SignalStatus[nChannelId].fSignalPower = 0;
		s_ucIsDb7584Lock[nUnitId] = FALSE;

		return FALSE;
	}


}

int  DB7584_GetStrengthQuality(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	if(s_ucIsDb7584Lock[nChannelId])
	{
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
		DB7584_CheckLock(nChannelId);
#endif

		pstSignalInfo->ulStrength 			= s_stDb7584SignalStatus[nChannelId].nStrength;
		pstSignalInfo->ulQuality 			= s_stDb7584SignalStatus[nChannelId].ulQuality;
		pstSignalInfo->ulAgc 				= s_stDb7584SignalStatus[nChannelId].ucAgc;

		pstSignalInfo->fBer 				= s_stDb7584SignalStatus[nChannelId].ulBer / 1000000.0;
		pstSignalInfo->fSnr 				= s_stDb7584SignalStatus[nChannelId].ulSnr / 100.0;

		pstSignalInfo->ulRfAgc 				= s_stDb7584SignalStatus[nChannelId].ucRfAgc;
		pstSignalInfo->ulIfAgc 				= s_stDb7584SignalStatus[nChannelId].ucIfAgc;

		pstSignalInfo->ulCorrectedNo 		= s_stDb7584SignalStatus[nChannelId].ulCorrectedNo;
		pstSignalInfo->ulUnCorrectedNo 		= s_stDb7584SignalStatus[nChannelId].ulUnCorrectedNo;
		pstSignalInfo->ulUnErroredNo 		= s_stDb7584SignalStatus[nChannelId].ulUnErroredNo;
		pstSignalInfo->fSignalInputPower		= s_stDb7584SignalStatus[nChannelId].fSignalPower;

/* TODO_JDCHO - Activate when the latest DI is ported
		pstSignalInfo->fSignalInputPower	= s_stDb7580SignalStatus[nChannelId].fSignalPower;
*/
		CH_DI_Print(1,("DB7584_GetStrengthQuality[%d] : Locked \n",nChannelId ));
		CH_DI_Print(1,("=================== DB7584_GetStrengthQuality[%d] ===================\n",nChannelId));
		CH_DI_Print(1,("  ulStrength               = %d\n", pstSignalInfo->ulStrength));
		CH_DI_Print(1,("  ulQuality                = %d\n", pstSignalInfo->ulQuality));
		CH_DI_Print(1,("  fBer                     = %f\n", pstSignalInfo->fBer));
		CH_DI_Print(1,("  fSnr                     = %3.1f\n", pstSignalInfo->fSnr));
		CH_DI_Print(1,("  Signal Power [dBuV]      = %4.1f\n", pstSignalInfo->fSignalInputPower));
		CH_DI_Print(1,("  ulIfAgc                  = %u\n", pstSignalInfo->ulIfAgc));
//		CH_DI_Print(0,("  ulRfAgc                  = %u\n", pstSignalInfo->ulRfAgc));
		CH_DI_Print(1,("  ulCorrectedNo            = %u\n", pstSignalInfo->ulCorrectedNo));
		CH_DI_Print(1,("  ulUnCorrectedNo          = %u\n", pstSignalInfo->ulUnCorrectedNo));
		CH_DI_Print(1,("  ulUnErroredNo            = %u\n", pstSignalInfo->ulUnErroredNo));

/* TODO_JDCHO - Activate when the latest DI is ported
		CH_DI_Print(2,("  fSignalInputPower        = %u\n", pstSignalInfo->fSignalInputPower));
*/
		CH_DI_Print(1,("======================================================================\n"));
	}
	else
	{
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
		CH_DI_Print(0,("DB7584_GetStrengthQuality[%d] : Unlocked \n",nChannelId));
	}

	return DI_CH_OK;
}

int  DB7584_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	int nUnitId;

	CH_UNUSED(etPower);

	CH_DI_Print(6, ("[DB7584_SetPowerState[%d]] ----\n",nChannelId));

	nUnitId = GetDeviceUnitId( nChannelId );	

	return DI_CH_OK;
}

int DB7584_EnableTsOutput ( int nChannelId )
{
	int nUnitId;

	CH_DI_Print(3, ("[DB7584_DisableTsOutput[%d]] ----\n",nChannelId));

	nUnitId = GetDeviceUnitId( nChannelId );

	return DI_CH_OK;
}

int DB7584_DisableTsOutput ( int nChannelId )
{
	int nUnitId;

	CH_DI_Print(3, ("[DB7584_DisableTsOutput[%d]] ----\n",nChannelId));
	
	nUnitId = GetDeviceUnitId( nChannelId );

	return DI_CH_OK;
}

int DB7584_Initialize ( int nChannelId )
{
	/* Don't need antthing*/
	return DI_CH_OK;
}

int DB7584_ReInitDevice ( int nChannelId )
{
	/* Don't need antthing*/
	return DI_CH_OK;
}

