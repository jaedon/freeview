/**
 * unittest_di_channel.c
*/

/**
 * @author		Kevin Woo
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "unittest.h"

#include "vkernel.h"
#include "di_uart.h"

#include "di_channel_s.h"
#include "di_channel_t.h"
#include "di_channel_c.h"

#include "di_err.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE TRACE_MODULE_DI_CHANNEL

/* Set unit test channel locking here */

#if defined(CONFIG_FRONTEND_SAT)&&defined(CONFIG_FRONTEND_CAB)&&defined(CONFIG_FRONTEND_TER)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_SAT_CHANNEL+CONFIG_NUM_OF_CAB_CHANNEL+CONFIG_NUM_OF_TER_CHANNEL)
#elif defined(CONFIG_FRONTEND_SAT)&&defined(CONFIG_FRONTEND_CAB)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_SAT_CHANNEL+CONFIG_NUM_OF_CAB_CHANNEL)
#elif defined(CONFIG_FRONTEND_SAT)&&defined(CONFIG_FRONTEND_TER)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_SAT_CHANNEL+CONFIG_NUM_OF_TER_CHANNEL)
#elif defined(CONFIG_FRONTEND_CAB)&&defined(CONFIG_FRONTEND_TER)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_CAB_CHANNEL+CONFIG_NUM_OF_TER_CHANNEL)
#elif defined(CONFIG_FRONTEND_SAT)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_SAT_CHANNEL)
#elif defined(CONFIG_FRONTEND_CAB)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_CAB_CHANNEL)
#elif defined(CONFIG_FRONTEND_TER)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_TER_CHANNEL)
#else
#define NUM_OF_CHANNEL 0
#endif

#if defined(CONFIG_FRONTEND_SAT)

/* Fashion Tv Locking */
#define CH_ANT_TYPE DI_CH_ANT_TYPE_DISEQC

#define CH_FREQ 1405
#define CH_SYMBOL 2626

#define CH_CODE_RATE DI_CH_SCODERATE_AUTO
#define CH_POLAR_HOR DI_CH_POLAR_VER
#define CH_TRANS  DI_CH_TRANS_DVBS
#define CH_DISEQ_INPUT  DI_CH_DISEQC_INPUT_B
#define CH_TONE_BURST DI_CH_TONE_BURST_A
#define CH_22K_TONE TRUE

#define CH_MODULATION DI_CH_PSK_QPSK
#define CH_PILOT DI_CH_PILOT_AUTO
#define CH_ROLL_OFF DI_CH_ROLL_035
#define CH_LNB_VOLT DI_CH_LNB_VOLT_STD
#define CH_DISQ_VER DI_CH_DISEQC_VER_1_0

#endif

#if defined(CONFIG_FRONTEND_CAB)

#define CH_FREQ 522*1000
#define CH_SYMBOL 6875

#define CH_MODULATION DI_CH_CMOD_256QAM
#define CH_SPECTRUM DI_CH_INVERSION_AUTO 

#endif

#if defined(CONFIG_FRONTEND_TER)

#define CH_SYSTEM DI_CH_DVBT

#define CH_FREQ 610 * 1000 /*unit KHz*/
#define CH_BW  DI_CH_BW_8MHZ

#define CH_CODE_RATE DI_CH_TCODERATE_AUTO
#define CH_GUARD_INTERVAL DI_CH_GUARDINTERVAL_AUTO
#define CH_HIERARCHY DI_CH_HIERARCHY_AUTO

#define CH_MODULATION DI_CH_TMOD_64QAM
#define CH_OFFSET DI_CH_OFFSET_AUTO
#define CH_STREAM DI_CH_STREAM_HP
#define CH_TRANSMODE DI_CH_TRANSMODE_AUTO

#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/





/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
static unsigned long	ulRequestId = -1;


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static HBOOL test_channel_passed;
DI_CH_SignalInfo_e tunerStatus;


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

static UT_RESULT ut_ChannelActivate(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;	
	int				i = 0;

	for(i=0;i<NUM_OF_CHANNEL;i++)
	{
#if defined(CONFIG_FRONTEND_SAT)
		result = DI_CH_S_ActivateChannel( i );	
		DONE_ERR(result != DI_ERR_OK);

#endif

		
#if defined(CONFIG_FRONTEND_CAB)
		result = DI_CH_C_ActivateChannel( i );		
		DONE_ERR(result != DI_ERR_OK);
#endif		

#if defined(CONFIG_FRONTEND_TER)
		result = DI_CH_T_ActivateChannel( i );		
		DONE_ERR(result != DI_ERR_OK);
#endif
	}

	ret = UT_OK;

	done:
		return ret; 

}


static UT_RESULT ut_ChannelDeactivate(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;	
	int				i = 0;

	for(i=0;i<NUM_OF_CHANNEL;i++)
	{
#if defined(CONFIG_FRONTEND_SAT)
		result = DI_CH_S_DeactivateChannel( i );		
		DONE_ERR(result != DI_ERR_OK);
#endif

		
#if defined(CONFIG_FRONTEND_CAB)
		result = DI_CH_C_DeactivateChannel( i );		
		DONE_ERR(result != DI_ERR_OK);
#endif		

#if defined(CONFIG_FRONTEND_TER)
		result = DI_CH_T_DeactivateChannel( i );		
		DONE_ERR(result != DI_ERR_OK);
#endif
	}

	ret = UT_OK;

	done:
		return ret; 

}

static UT_RESULT ut_GetNumOfInstance(void)
{
	DI_CHANNEL_ERR 	result;
    	UT_RESULT		ret = UT_FAIL;
	HUINT32			num;

	result = DI_CH_GetNumOfInstance(NULL);
	if(result != DI_CH_PARAM_ERR)
	{
		ret = UT_FAIL;
		goto done;
	}

	result = DI_CH_GetNumOfInstance(&num);
    	if(result != DI_CH_OK)
	{
		ret = UT_FAIL;
		goto done;

	}

	DI_UART_Print("Number of instance is %d \n", num);
    	if(num < 0)
	{
		ret = UT_FAIL;
		goto done;
	}

	ret = UT_OK;

	done:
		return ret;
}

static UT_RESULT ut_GetCapability(void)
{
	DI_CHANNEL_ERR 	result;
    	UT_RESULT		ret = UT_FAIL;
	HUINT32 			num;
	HUINT32			channelType;

	
	result = DI_CH_GetNumOfInstance(&num);
	if(result != DI_CH_OK)
	{
		ret = UT_FAIL;
		goto done;

	}

	/* instance id < 0 */
	result = DI_CH_GetCapability(-1, &channelType);
	if(result != DI_CH_PARAM_ERR)
	{
		ret = UT_FAIL;
		goto done;

	}

	/* instance id > DI_CH_GetNumOfInstance*/
	result = DI_CH_GetCapability(num + 1, &channelType);
	if(result != DI_CH_PARAM_ERR)
	{
		ret = UT_FAIL;
		goto done;

	}

	/* instance id < 0 */	
	result = DI_CH_GetCapability(0, NULL);
	if(result != DI_CH_PARAM_ERR)
	{
		ret = UT_FAIL;
		goto done;

	}

	for(int i = 0; i < num; i++)
	{
		result = DI_CH_GetCapability(i, &channelType);
		if(result == DI_CH_PARAM_ERR)
		{
			ret = UT_FAIL;
			goto done;
		}
		DI_UART_Print("i = %d : channelType = %d \n", i, channelType);
		if((channelType != DI_CH_TYPE_TER) && (channelType != DI_CH_TYPE_SAT)
			&& (channelType != DI_CH_TYPE_CAB) && (channelType != DI_CH_TYPE_ANA))
		{
			ret = UT_FAIL;
			goto done;
		}
	}

	ret = UT_OK;

	done:
		return ret;
}

void CallbackFunc ( HUINT32 ulInstanceId, HUINT32 unRequestID, DI_CH_Type_e eChannelType , DI_CH_SignalInfo_e etSignalInfo )
{	
	DI_UART_Print("\n===> Go to callback function <===\n");

	tunerStatus = etSignalInfo;

	if (tunerStatus == DI_CH_SIGNAL_LOCKED)
	{
		DI_UART_Print("\n\t\t===> DI_CH_SIGNAL_LOCKED <===\t\t\n");
		test_channel_passed = TRUE;
	}
	else if(tunerStatus == DI_CH_SIGNAL_UNLOCK)
	{
		DI_UART_Print("\n\t\t===> DI_CH_SIGNAL_UNLOCK <===\t\t\n");
		test_channel_passed = FALSE;
	}else
	{
		test_channel_passed = FALSE;
	}
	return;
}

static UT_RESULT ut_RegisterCallback(void)
{
	DI_CHANNEL_ERR 	result;
    	UT_RESULT		ret = UT_FAIL;
	HUINT32 			num;
	HBOOL 			bLocked = 0;
	ulRequestId++;

	result = DI_CH_GetNumOfInstance(&num);
	if(result != DI_CH_OK)
	{
		ret = UT_FAIL;
		goto done;

	}
#if defined(CONFIG_FRONTEND_TER)
	/* instance id < 0 */
	result = DI_CH_T_RegisterCallback(-1, CallbackFunc);
	if(result != DI_CH_PARAM_ERR)
	{
		ret = UT_FAIL;
		goto done;

	}

	/* instance id > DI_CH_GetNumOfInstance*/
	result = DI_CH_T_RegisterCallback(num + 1, CallbackFunc);
	if(result != DI_CH_PARAM_ERR)
	{
		ret = UT_FAIL;
		goto done;

	}

	/* callback function = NULL */	
	result = DI_CH_T_RegisterCallback(0, NULL);
	if(result != DI_CH_OK)
	{
		ret = UT_FAIL;
		goto done;

	}

	CH_TER_TuneParam_t stTerChannelParam;
	CH_TER_TuneParam_t stTerLockedInfo;

	stTerChannelParam.etTerSystem = CH_SYSTEM;

	stTerChannelParam.ulFrequency = CH_FREQ; /*unit KHz*/
	
	stTerChannelParam.etBandWidth =   CH_BW;
	stTerChannelParam.etCodeRate = CH_CODE_RATE;
	stTerChannelParam.etGuardInterval = CH_GUARD_INTERVAL;
	stTerChannelParam.etHierarchy = CH_HIERARCHY;
	
	stTerChannelParam.etModulation =CH_MODULATION ;
	
	stTerChannelParam.etOffset = CH_OFFSET;
	stTerChannelParam.etStream = CH_STREAM;
	stTerChannelParam.etTransMode = CH_TRANSMODE;

	for(int i = 0; i < num; i++)
	{
		/* callback function = NULL */	
		result = DI_CH_T_RegisterCallback(i, NULL);
		if(result != DI_CH_OK)
		{
			ret = UT_FAIL;
			goto done;

		}
	
		result = DI_CH_T_RegisterCallback(i, CallbackFunc);
		if(result == DI_CH_PARAM_ERR)
		{
			ret = UT_FAIL;
			goto done;
		}
		test_channel_passed = FALSE;


		result = DI_CH_T_RequestLock( i, ulRequestId, &stTerChannelParam);
		DONE_ERR(result != DI_ERR_OK);

		VK_TASK_Sleep(3000);

		result= DI_CH_T_GetLockedInfo(i, &stTerLockedInfo);
		DONE_ERR(result != DI_ERR_OK);
		

		if(test_channel_passed == FALSE)
		{
			ret = UT_FAIL;
			goto done;
		}

	}

	

#endif

#if defined(CONFIG_FRONTEND_CAB)
		/* instance id < 0 */
		result = DI_CH_C_RegisterCallback(-1, CallbackFunc);
		if(result != DI_CH_PARAM_ERR)
		{
			ret = UT_FAIL;
			goto done;
	
		}
	
		/* instance id > DI_CH_GetNumOfInstance*/
		result = DI_CH_C_RegisterCallback(num + 1, CallbackFunc);
		if(result != DI_CH_PARAM_ERR)
		{
			ret = UT_FAIL;
			goto done;
	
		}
	
		/* callback function = NULL */	
		result = DI_CH_C_RegisterCallback(0, NULL);
		if(result != DI_CH_OK)
		{
			ret = UT_FAIL;
			goto done;
	
		}
	
		CH_CAB_TuneParam_t stCabChannelParam;
	
		stCabChannelParam.ulFrequency = CH_FREQ;
		stCabChannelParam.ulSymbolRate = CH_SYMBOL;
		stCabChannelParam.etModulation = CH_MODULATION;
		stCabChannelParam.etSpectrum = CH_SPECTRUM;
	
		for(int i = 0; i < num; i++)
		{
			/* callback function = NULL */	
			result = DI_CH_C_RegisterCallback(0, NULL);
			if(result != DI_CH_OK)
			{
				ret = UT_FAIL;
				goto done;
	
			}
		
			result = DI_CH_C_RegisterCallback(i, CallbackFunc);
			if(result == DI_CH_PARAM_ERR)
			{
				ret = UT_FAIL;
				goto done;
			}
			test_channel_passed = FALSE;
			result = DI_CH_C_RequestLock( i, 0, &stCabChannelParam);
			DI_UART_Print("Finished request lock result = %d", result);
			DONE_ERR(result != DI_ERR_OK);
	
			VK_TASK_Sleep(3000);
			if(test_channel_passed == FALSE)
			{
				ret = UT_FAIL;
				goto done;
			}
	
			result = DI_CH_C_CancelLock(i);
			DI_UART_Print("Cancel request lock result = %d", result);
			DONE_ERR(result != DI_ERR_OK);
		}
	
		result = DI_CH_C_RegisterCallback(0, NULL);
			if(result != DI_CH_OK)
			{
				ret = UT_FAIL;
				goto done;
	
			}
	
#endif

#if defined(CONFIG_FRONTEND_SAT)
		/* instance id < 0 */
		result = DI_CH_S_RegisterCallback(-1, CallbackFunc);
		if(result != DI_CH_PARAM_ERR)
		{
			ret = UT_FAIL;
			goto done;
	
		}
	
		/* instance id > DI_CH_GetNumOfInstance*/
		result = DI_CH_S_RegisterCallback(num + 1, CallbackFunc);
		if(result != DI_CH_PARAM_ERR)
		{
			ret = UT_FAIL;
			goto done;
	
		}
	
		/* callback function = NULL */	
		result = DI_CH_S_RegisterCallback(0, NULL);
		if(result != DI_CH_OK)
		{
			ret = UT_FAIL;
			goto done;
	
		}
	
		CH_SAT_TuneParam_t stSatChannelParam;
	
		
		stSatChannelParam.etAntennaType = CH_ANT_TYPE;
		
		stSatChannelParam.ulFrequency = CH_FREQ; //000; //unit KHz
		stSatChannelParam.ulSymbolRate = CH_SYMBOL;
		stSatChannelParam.etFecCodeRate =  CH_CODE_RATE;//DI_CH_SCODERATE_AUTO; //
		stSatChannelParam.etPolarization = CH_POLAR_HOR;
		stSatChannelParam.etTransSpec = CH_TRANS;
		stSatChannelParam.etDiseqcInput = CH_DISEQ_INPUT;
		stSatChannelParam.etToneBurst = CH_TONE_BURST;
		stSatChannelParam.b22kTone = CH_22K_TONE;

		stSatChannelParam.etModulation = CH_MODULATION;
		stSatChannelParam.etPilot = CH_PILOT;
		stSatChannelParam.etRollOff = CH_ROLL_OFF;
		stSatChannelParam.etLnbVoltage = CH_LNB_VOLT;
		stSatChannelParam.etDiseqcVersion = CH_DISQ_VER;
		stSatChannelParam.ulScdMasterUnitId = 0;
		stSatChannelParam.ulScdUserBandNum = 0;
		stSatChannelParam.ulScdUserBandFreq = 0;
		
	
		for(int i = 0; i < num; i++)
		{
			/* callback function = NULL */	
			result = DI_CH_S_RegisterCallback(0, NULL);
			if(result != DI_CH_OK)
			{
				ret = UT_FAIL;
				goto done;
	
			}
		
			result = DI_CH_S_RegisterCallback(i, CallbackFunc);
			if(result == DI_CH_PARAM_ERR)
			{
				ret = UT_FAIL;
				goto done;
			}
			test_channel_passed = FALSE;
			result = DI_CH_S_RequestLock( i, 0, &stSatChannelParam);
			DI_UART_Print("Finished request lock result = %d", result);
			DONE_ERR(result != DI_ERR_OK);
	
			VK_TASK_Sleep(3000);
			if(test_channel_passed == FALSE)
			{
				ret = UT_FAIL;
				goto done;
			}
	
			result = DI_CH_S_CancelLock(i);
			DI_UART_Print("Cancel request lock result = %d", result);
			DONE_ERR(result != DI_ERR_OK);
		}
	
		result = DI_CH_S_RegisterCallback(0, NULL);
			if(result != DI_CH_OK)
			{
				ret = UT_FAIL;
				goto done;
	
			}
	
#endif


	ret = UT_OK;

	done:
		return ret;
}


static UT_RESULT ut_ChannelRequestLock(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;
	int				i = 0;

	ulRequestId++;

	for(i=0;i<NUM_OF_CHANNEL;i++)
	{
	
#if defined(CONFIG_FRONTEND_SAT)
		CH_SAT_TuneParam_t stSatChannelParam;
		CH_SAT_TuneParam_t   stSatLockedInfo;
		CH_SAT_TuneParam_t   stSatLockedInfo0;
#endif

#if defined(CONFIG_FRONTEND_CAB)
		CH_CAB_TuneParam_t stCabChannelParam;
		CH_CAB_TuneParam_t   stCabLockedInfo;
		CH_CAB_TuneParam_t   stCabLockedInfo0;
#endif

#if defined(CONFIG_FRONTEND_TER)
		CH_TER_TuneParam_t stTerChannelParam;
		CH_TER_TuneParam_t   stTerLockedInfo;
		CH_TER_TuneParam_t   stTerLockedInfo0;
#endif


#if defined(CONFIG_FRONTEND_SAT)

		stSatChannelParam.etAntennaType = CH_ANT_TYPE;

		stSatChannelParam.ulFrequency = CH_FREQ; //000; //unit KHz
		stSatChannelParam.ulSymbolRate = CH_SYMBOL;
		stSatChannelParam.etFecCodeRate =  CH_CODE_RATE;//DI_CH_SCODERATE_AUTO; //
		stSatChannelParam.etPolarization = CH_POLAR_HOR;
		stSatChannelParam.etTransSpec = CH_TRANS;
		stSatChannelParam.etDiseqcInput = CH_DISEQ_INPUT;
		stSatChannelParam.etToneBurst = CH_TONE_BURST;
		stSatChannelParam.b22kTone = CH_22K_TONE;

		stSatChannelParam.etModulation = CH_MODULATION;
		stSatChannelParam.etPilot = CH_PILOT;
		stSatChannelParam.etRollOff = CH_ROLL_OFF;
		stSatChannelParam.etLnbVoltage = CH_LNB_VOLT;
		stSatChannelParam.etDiseqcVersion = CH_DISQ_VER;
		stSatChannelParam.ulScdMasterUnitId = 0;
		stSatChannelParam.ulScdUserBandNum = 0;
		stSatChannelParam.ulScdUserBandFreq = 0;

		
		result = DI_CH_S_CancelLock( i);
		DONE_ERR(result != DI_ERR_OK);
		
		result= DI_CH_S_GetLockedInfo(i, &stSatLockedInfo0);
		DONE_ERR(result == DI_ERR_OK);
		
		VK_TASK_Sleep(3000);

		result = DI_CH_S_RequestLock( i, ulRequestId, &stSatChannelParam);
		DONE_ERR(result != DI_ERR_OK);

		VK_TASK_Sleep(3000);

		if(tunerStatus == DI_CH_SIGNAL_LOCKED)
		{

			result= DI_CH_S_GetLockedInfo(i, &stSatLockedInfo);
			DONE_ERR(result != DI_ERR_OK);

			if(	stSatLockedInfo.ulFrequency != CH_FREQ ||
				stSatLockedInfo.etModulation != CH_MODULATION)
			{
				DI_UART_Print(" Get wrong tuner parameter \n");
				goto done;
			}
		}
		else
		{
			DI_UART_Print(" SAT Unlock ! Retry Lock cmd \n");
			goto done;
		}

#endif

#if defined(CONFIG_FRONTEND_CAB)

		stCabChannelParam.ulFrequency = CH_FREQ;
		stCabChannelParam.ulSymbolRate = CH_SYMBOL;
		stCabChannelParam.etModulation = CH_MODULATION;
		stCabChannelParam.etSpectrum = CH_SPECTRUM;

		result = DI_CH_C_CancelLock( i);
		DONE_ERR(result != DI_ERR_OK);
		
		result= DI_CH_C_GetLockedInfo(i, &stCabLockedInfo0);
		DONE_ERR(result == DI_ERR_OK);
		
		VK_TASK_Sleep(3000);
		
		result = DI_CH_C_RequestLock( i, ulRequestId, &stCabChannelParam);
		DONE_ERR(result != DI_ERR_OK);

		VK_TASK_Sleep(3000);

		if(tunerStatus == DI_CH_SIGNAL_LOCKED)
		{

			result= DI_CH_C_GetLockedInfo(i, &stCabLockedInfo);
			DONE_ERR(result != DI_ERR_OK);

			if(	stCabLockedInfo.ulFrequency != CH_FREQ ||
				stCabLockedInfo.etModulation != CH_MODULATION)
			{
				DI_UART_Print(" Get wrong tuner parameter \n");
				goto done;
			}
		}
		else
		{
			DI_UART_Print(" CAB Unlock ! Retry Lock cmd \n");
			goto done;
		}

#endif

#if defined(CONFIG_FRONTEND_TER)

		stTerChannelParam.etTerSystem = CH_SYSTEM;
		stTerChannelParam.ulFrequency = CH_FREQ; /*unit KHz*/		
		stTerChannelParam.etBandWidth =   CH_BW;
		stTerChannelParam.etCodeRate = CH_CODE_RATE;
		stTerChannelParam.etGuardInterval = CH_GUARD_INTERVAL;
		stTerChannelParam.etHierarchy = CH_HIERARCHY;		
		stTerChannelParam.etModulation =CH_MODULATION;		
		stTerChannelParam.etOffset = CH_OFFSET;
		stTerChannelParam.etStream = CH_STREAM;
		stTerChannelParam.etTransMode = CH_TRANSMODE;

		result = DI_CH_T_CancelLock( i);
		DONE_ERR(result != DI_ERR_OK);

		result= DI_CH_T_GetLockedInfo(i, &stTerLockedInfo0);
		DONE_ERR(result == DI_ERR_OK);

		VK_TASK_Sleep(3000);

		result = DI_CH_T_RequestLock( i, ulRequestId, &stTerChannelParam);
		DONE_ERR(result != DI_ERR_OK);

		VK_TASK_Sleep(3000);

		if(tunerStatus == DI_CH_SIGNAL_LOCKED)
		{

			result= DI_CH_T_GetLockedInfo(i, &stTerLockedInfo);
			DONE_ERR(result != DI_ERR_OK);
#if 0
			printf ("\n stTerLockedInfo.etTerSystem = %d \n", stTerLockedInfo.etTerSystem);
			printf ("\n stTerLockedInfo.ulFrequency = %d \n", stTerLockedInfo.ulFrequency);
			printf ("\n stTerLockedInfo.etBandWidth = %d \n", stTerLockedInfo.etBandWidth);
			printf ("\n stTerLockedInfo.etCodeRate = %d \n", stTerLockedInfo.etCodeRate);
			printf ("\n stTerLockedInfo.etGuardInterval = %d \n",stTerLockedInfo.etGuardInterval);
			printf ("\n stTerLockedInfo.etHierarchy = %d \n",stTerLockedInfo.etHierarchy);
			printf ("\n stTerLockedInfo.etModulation = %d \n", stTerLockedInfo.etModulation);
			printf ("\n stTerLockedInfo.etOffset = %d \n",stTerLockedInfo.etOffset);
			printf ("\n stTerLockedInfo.etStream = %d \n",stTerLockedInfo.etStream);
			printf ("\n stTerLockedInfo.etTransMode = %d \n", stTerLockedInfo.etTransMode); 
#endif

			if(stTerLockedInfo.etTerSystem != CH_SYSTEM ||
				stTerLockedInfo.ulFrequency != CH_FREQ ||
				stTerLockedInfo.etBandWidth != CH_BW ||
				stTerLockedInfo.etModulation != CH_MODULATION ||
				stTerLockedInfo.etStream != CH_STREAM)
			{
				DI_UART_Print(" Get wrong tuner parameter \n");
				goto done;
			}
		}
		else
		{
			DI_UART_Print(" TER Unlock ! Retry Lock cmd \n");
			goto done;
		}
#endif

	}

	ret = UT_OK;

	done:
		return ret; 
	
}

static UT_RESULT ut_ChannelStatusCheck(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;
	int				i=0;

#if defined(CONFIG_FRONTEND_TER)
	CH_TER_TuneParam_t   stTerLockedInfo;
#endif

#if defined(CONFIG_FRONTEND_CAB)
	CH_CAB_TuneParam_t	stCabLockedInfo;
#endif
	HBOOL bLocked = 0;

	DI_CH_SignalQuality_t stSignalInfo;

	VK_TASK_Sleep(1000);

#if defined(CONFIG_FRONTEND_SAT)

	for(i=0;i<NUM_OF_CHANNEL;i++)
	{
		bLocked = CH_S_CheckLock( i);

		if(bLocked == TRUE)
		{
			result = DI_CH_S_GetSignalInfo(i, &stSignalInfo);
			DONE_ERR(result != DI_ERR_OK);
		}
		else
		{
			DI_UART_Print(" SAT Unlock ! Retry Lock cmd \n");
			goto done;
		}
	}
	
#endif

#if defined(CONFIG_FRONTEND_CAB)

	for(i=0;i<NUM_OF_CHANNEL;i++)
	{
		bLocked = CH_C_CheckLock( i);

		if(bLocked == TRUE)
		{
			result= DI_CH_C_GetLockedInfo(i, &stCabLockedInfo);
			DONE_ERR(result != DI_ERR_OK);
			result = DI_CH_C_GetSignalInfo(i, &stSignalInfo);
			DONE_ERR(result != DI_ERR_OK);
		}
		else
		{
			DI_UART_Print(" CAB Unlock ! Retry Lock cmd \n");
			goto done;
		}
	}
	
#endif

#if defined(CONFIG_FRONTEND_TER)
		for(i=0;i<NUM_OF_CHANNEL;i++)
		{
			bLocked = CH_T_CheckLock( i);
		 
			if(bLocked == TRUE)
			{
				result= DI_CH_T_GetLockedInfo(i, &stTerLockedInfo);
				DONE_ERR(result != DI_ERR_OK);
				result = DI_CH_T_GetSignalInfo(i, &stSignalInfo);
				DONE_ERR(result != DI_ERR_OK);
			 }
			 else
			{
				DI_UART_Print(" TER Unlock ! Retry Lock cmd \n");
				goto done;
			}
		}
#endif

	ret = UT_OK;

	done:
		return ret; 
}

static UT_MENU_ITEM_T	menu_channel[] = 
{
	MENU_START,
	MENU_FUNC(ut_RegisterCallback, 5),
	MENU_FUNC(ut_GetNumOfInstance, 1),
	MENU_FUNC(ut_GetCapability, 1),	
	MENU_FUNC(ut_ChannelRequestLock, 5),
	MENU_FUNC(ut_ChannelActivate, 5),		
	MENU_FUNC(ut_ChannelStatusCheck, 9), /* test after channel locking */ 
	MENU_FUNC(ut_ChannelDeactivate, 5),	
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_channel[] = 
{	
	MENU_SUB(menu_channel),
};

void UT_CHANNEL_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_channel)/sizeof(menu_main_channel[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_channel[i]);
	}
	array_size = sizeof(menu_channel)/sizeof(menu_channel[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_channel[i]);
	}
}



