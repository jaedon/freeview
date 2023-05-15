#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "cmdtool.h"
#include "cmd_hwtest_interface.h"

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_util.h"
#include "di_channel_attr.h"

#include "drv_tspath.h"
#include "drv_gpio.h"

#if defined(CONFIG_FRONTEND_SAT)
#include "drv_channel_s.h"
#include "drv_channel_lnb.h"
#endif

#if defined(CONFIG_FRONTEND_CAB)
#include "drv_channel_c.h"
#endif

#if defined(CONFIG_FRONTEND_TER)
#include "drv_channel_t.h"
#endif

#if defined (CONFIG_PLATFORM_DP10023)
#include "drv_channel_dp10023.h"
#endif

#if defined (CONFIG_PLATFORM_DP10024)
#include "drv_channel_dp10024.h"
#endif


#if defined (CONFIG_PLATFORM_DT90507)
#include "drv_channel_dt90507.h"
#elif defined (CONFIG_PLATFORM_DT90517)
#include "drv_channel_dt90517.h"
#endif

#if defined (CONFIG_PLATFORM_MXL683)
#include "drv_channel_mxl683.h"
#endif

/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

//#define CONFIG_DEMOD_TSMF
#if defined(CONFIG_DEMOD_TSMF)
HBOOL bChannelLockSignal = FALSE;
extern HINT32 show_di_ch_tsmf;
#endif


/*extern variables */
extern HINT32 show_di_ch;

extern HUINT8  g_hwtest_lnb_off;

#if defined (CONFIG_PLATFORM_DP10023)
extern HUINT8  gucCABTopOn, gucRfMinC,gucRfMaxC, gucIfMinC, gucIfMaxC;
#endif

#if defined (CONFIG_PLATFORM_DT90517)
extern HUINT8  gucTERTopOn,  gucRfMinT,gucRfMaxT, gucIfMinT, gucIfMaxT;
#endif

/* static variables */
static HUINT32 ulRequestId= -1;

static HUINT8   ucIsInitDone = 0;

static HUINT8   ucIsDeepSleep = 0;


//JAMES...WA8500...Cab 4 channels + Ter 3 channels
#if 0
#if defined(CONFIG_FRONTEND_SAT)
#if (CONFIG_NUM_OF_SAT_CHANNEL == 4)
static HUINT8   ucIsLocked[CONFIG_NUM_OF_SAT_CHANNEL] = {0,0,0,0};
static HUINT8	s_CurrentType[CONFIG_NUM_OF_SAT_CHANNEL] = {0xff,0xff,0xff,0xff};
#elif (CONFIG_NUM_OF_SAT_CHANNEL == 2)
static HUINT8   ucIsLocked[CONFIG_NUM_OF_SAT_CHANNEL] = {0,0};
static HUINT8	s_CurrentType[CONFIG_NUM_OF_SAT_CHANNEL] = {0xff,0xff};
#elif (CONFIG_NUM_OF_SAT_CHANNEL == 1)
static HUINT8   ucIsLocked[CONFIG_NUM_OF_SAT_CHANNEL] = {0};
static HUINT8	s_CurrentType[CONFIG_NUM_OF_SAT_CHANNEL] = {0xff};
#else
#error CONFIG_FRONTEND_SAT is not defined.
#endif
#endif


#if defined(CONFIG_FRONTEND_TER)
#if (CONFIG_NUM_OF_TER_CHANNEL == 4)
static HUINT8	s_CurrentType[CONFIG_NUM_OF_TER_CHANNEL] = {0xff,0xff,0xff,0xff};
#elif (CONFIG_NUM_OF_TER_CHANNEL == 2)
static HUINT8	s_CurrentType[CONFIG_NUM_OF_TER_CHANNEL] = {0xff,0xff};
#elif (CONFIG_NUM_OF_TER_CHANNEL == 1)
static HUINT8	s_CurrentType[CONFIG_NUM_OF_TER_CHANNEL] = {0xff};
#else
#error CONFIG_FRONTEND_TER is not defined.
#endif
#endif

#if defined(CONFIG_FRONTEND_CAB)
#if (CONFIG_NUM_OF_CAB_CHANNEL == 4)
static HUINT8	s_CurrentType[CONFIG_NUM_OF_CAB_CHANNEL] = {0xff,0xff,0xff,0xff};
#elif (CONFIG_NUM_OF_CAB_CHANNEL == 2)
static HUINT8	s_CurrentType[CONFIG_NUM_OF_CAB_CHANNEL] = {0xff,0xff};
#elif (CONFIG_NUM_OF_CAB_CHANNEL == 1)
static HUINT8	s_CurrentType[CONFIG_NUM_OF_CAB_CHANNEL] = {0xff};
#else
#error CONFIG_FRONTEND_CAB is not defined.
#endif
#endif

#else //#if 0

#if (defined (CONFIG_FRONTEND_TER) && defined (CONFIG_FRONTEND_SAT))

#if (NUM_OF_CHANNEL == 4)
static HUINT8   ucIsLocked[NUM_OF_CHANNEL] = {0,0,0,0};
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 3)
static HUINT8   ucIsLocked[NUM_OF_CHANNEL] = {0,0,0};
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL == 2)
static HUINT8   ucIsLocked[NUM_OF_CHANNEL] = {0,0};
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff};
#elif (NUM_OF_CHANNEL == 1)
static HUINT8   ucIsLocked[NUM_OF_CHANNEL] = {0};
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff};
#else
#error CONFIG_FRONTEND_SAT & CONFIG_FRONTEND_TER is not defined.
#endif

#elif (defined (CONFIG_FRONTEND_TER) && defined (CONFIG_FRONTEND_CAB))

#if (NUM_OF_CHANNEL== 8)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 7)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff, 0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 6)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff, 0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 5)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff, 0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 4)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 3)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 2)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff};
#elif (NUM_OF_CHANNEL== 1)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff};
#else
#error CONFIG_FRONTEND_TER & CONFIG_FRONTEND_CAB is not defined.
#endif

#elif defined(CONFIG_FRONTEND_TER)

#if (NUM_OF_CHANNEL == 4)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL == 3)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL == 2)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff};
#elif (NUM_OF_CHANNEL == 1)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff};
#else
#error CONFIG_FRONTEND_TER is not defined.
#endif

#elif defined(CONFIG_FRONTEND_CAB)

#if (NUM_OF_CHANNEL== 8)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 7)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff, 0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 6)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff, 0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 5)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff, 0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 4)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL== 3)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL == 2)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff};
#elif (NUM_OF_CHANNEL == 1)
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff};
#else
#error CONFIG_FRONTEND_CAB is not defined.
#endif

#elif defined(CONFIG_FRONTEND_SAT)
#if (NUM_OF_CHANNEL == 4)
static HUINT8   ucIsLocked[NUM_OF_CHANNEL] = {0,0,0,0};
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff,0xff,0xff};
#elif (NUM_OF_CHANNEL == 2)
static HUINT8   ucIsLocked[NUM_OF_CHANNEL] = {0,0};
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff,0xff};
#elif (NUM_OF_CHANNEL == 1)
static HUINT8   ucIsLocked[NUM_OF_CHANNEL] = {0};
static HUINT8	s_CurrentType[NUM_OF_CHANNEL] = {0xff};
#else
#error CONFIG_FRONTEND_SAT is not defined.
#endif

#else
#if defined(CONFIG_CHANNEL)
#error CONFIG_FRONTEND_XXX is not defined.
#endif
#endif

#endif //#if 0
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)
/* static functions */
#if defined(CONFIG_FRONTEND_SAT)
static void HwTest_ChSatCallback ( HUINT32 ulInstanceId, HUINT32 unRequestID, DI_CH_Type_e eChannelType , DI_CH_SignalInfo_e etSignalInfo )
{
	int nRet = 0;
	CH_SAT_TuneParam_t stSatLockedP;
	//DI_CH_SignalQuality_t stSignalInfo;
	CH_UNUSED(unRequestID);
	CH_UNUSED(eChannelType);

	CH_DI_Print(4,("[SatCallback] ulInstanceId[%d],unRequestID[%d],eChannelType[%d]\n", ulInstanceId, unRequestID, eChannelType));

	switch ( etSignalInfo )
	{
		case DI_CH_SIGNAL_LOCKED:
		    if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("LOCK\n");
            }

			CH_DI_Print(1, ("\t\t\t >>>DI_CH_SIGNAL_LOCKED<<<\n" ));
			nRet = DI_CH_S_GetLockedInfo( ulInstanceId, &stSatLockedP );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, (" "));
			}
			else
			{
				ucIsLocked[ulInstanceId] = 1;
				CH_DI_Print(2, ("[%d] Locked... ucIsLocked(%d)\n", ulInstanceId, ucIsLocked[ulInstanceId] ));
				//PrintSatTuneParam( &stSatLockedP );
			}
			/*
			nRet = DI_CH_S_GetSignalInfo( ulInstanceId, &stSignalInfo);
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, (" ");
			}
			*/
			break;

		case DI_CH_SIGNAL_UNLOCK:
		    if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("UNLOCK\n");
            }

			CH_DI_Print(1, ("\t\t\t >>>DI_CH_SIGNAL_UNLOCK<<<\n"));
			ucIsLocked[ulInstanceId] = 0;
			CMD_Printf( "[%d] Unlocked... ucIsLocked(%d)\n", ulInstanceId, ucIsLocked[ulInstanceId] );
			break;

		case DI_CH_SIGNAL_ANTENNA_FAULT:
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("ANTENNA_FAULT\n");
            }

			CH_DI_Print(1, ( "DI_CH_SIGNAL_ANTENNA_FAULT\n" ));
			break;

		default:
			CH_DI_Print(3, ("etSignalInfo(%d) \n", etSignalInfo));
			break;
	}

}
#endif

#if defined(CONFIG_FRONTEND_CAB)
static void HwTest_ChCabCallback ( HUINT32 ulInstanceId, HUINT32 unRequestID, DI_CH_Type_e eChannelType , DI_CH_SignalInfo_e etSignalInfo )
{
	int nRet = 0;
	CH_CAB_TuneParam_t stCabLockedP;
	DI_CH_SignalQuality_t stSignalInfo;
	eChannelType = eChannelType;
	unRequestID = unRequestID;

	CH_DI_Print(4,("ulInstanceId[%d],unRequestID[%d],eChannelType[%d]\n", ulInstanceId, unRequestID, eChannelType ));

	switch ( etSignalInfo )
	{
		case DI_CH_SIGNAL_LOCKED:
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("LOCK\n");
            }
#if defined(CONFIG_DEMOD_TSMF)
			bChannelLockSignal = TRUE;
#endif
			CMD_Printf( "\t\t\t >>>CAB_SIGNAL_LOCKED<<<\n" );
			nRet = DI_CH_C_GetLockedInfo( ulInstanceId, &stCabLockedP );
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(1, ("[HwTest_ChCabCallback] ERR!\n "));
			}
			else
			{
				CMD_Printf("CH[%d] LOCK ~~! \n", ulInstanceId);
				PrintCabTuneParam( &stCabLockedP );
			}

			nRet = DI_CH_C_GetSignalInfo( ulInstanceId, &stSignalInfo);
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(1, ("[HwTest_ChCabCallback] ERR!\n "));
			}

			break;

		case DI_CH_SIGNAL_UNLOCK:
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("UNLOCK\n");
            }

			CMD_Printf( "\t\t\t >>>CAB_SIGNAL_UNLOCK<<<\n" );
			CMD_Printf("CH[%d] UNLOCK !! \n", ulInstanceId);
			break;

		case DI_CH_SIGNAL_ANTENNA_FAULT:
			CH_DI_Print(1, ( "DI_CH_SIGNAL_ANTENNA_FAULT\n" ));
			break;

		default:
			CH_DI_Print(0, ("etSignalInfo(%d) \n", etSignalInfo));
			break;
	}

}
#endif


#if defined(CONFIG_FRONTEND_TER)
static void HwTest_ChTerCallback ( HUINT32 ulInstanceId, HUINT32 unRequestID, DI_CH_Type_e eChannelType , DI_CH_SignalInfo_e etSignalInfo )
{

	eChannelType = eChannelType;
	unRequestID = unRequestID;

	CH_DI_Print(4,("ulInstanceId[%d],unRequestID[%d],eChannelType[%d]\n", ulInstanceId, unRequestID, eChannelType ));

	switch ( etSignalInfo )
	{
		case DI_CH_SIGNAL_LOCKED:
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("LOCK\n");
            }
			CMD_Printf( "\t\t\t >>>TER_SIGNAL_LOCKED<<<\n" );

			break;

		case DI_CH_SIGNAL_UNLOCK:
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("UNLOCK\n");
            }
			CMD_Printf( "\t\t\t >>>TER_SIGNAL_UNLOCK<<<\n" );
			break;

		case DI_CH_SIGNAL_ANTENNA_FAULT:
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Ter SIGNAL ANTENNA FAULT\n");
            }
			CMD_Printf( "\t\t\t >>>DI_CH_SIGNAL_ANTENNA_FAULT<<<\n" );
			break;

		case DI_CH_SIGNAL_ANTENNA_OK:
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Ter SIGNAL ANTENNA OK\n");
            }
			CMD_Printf( "\t\t\t >>>DI_CH_SIGNAL_ANTENNA_OK<<<\n" );
			break;

		default:
			break;
	}

}
#endif


#if defined(CONFIG_FRONTEND_SAT)
static void HwTest_ChTuneSat(unsigned long  ulInstanceId , unsigned char ucSDchannel)
{
	CH_SAT_TuneParam_t stSatTuneP;
	//static HUINT32	unSyncId = 0;

	stSatTuneP.etAntennaType = DI_CH_ANT_TYPE_DISEQC;
	g_hwtest_lnb_off = 0;

	if (ucSDchannel == 1) // SD
	{
		stSatTuneP.ulFrequency = 1150; //000; //unit KHz
		stSatTuneP.ulSymbolRate = 28125;
		stSatTuneP.etFecCodeRate = DI_CH_SCODERATE_3_4;
		stSatTuneP.etPolarization = DI_CH_POLAR_HOR;

		stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS;
		stSatTuneP.etModulation = DI_CH_PSK_QPSK;
		stSatTuneP.etPilot = DI_CH_PILOT_AUTO;
		stSatTuneP.etRollOff = DI_CH_ROLL_035;

		stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;

		stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
		stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_B;
		stSatTuneP.etToneBurst = DI_CH_TONE_BURST_NONE;
		stSatTuneP.b22kTone = TRUE;

		stSatTuneP.ulScdMasterUnitId = 0;
		stSatTuneP.ulScdUserBandNum = 0;
		stSatTuneP.ulScdUserBandFreq = 0;

		/*  s-10
		stSatTuneP.ulFrequency = 1500; //000; //unit KHz
		stSatTuneP.ulSymbolRate = 27500;
		stSatTuneP.etFecCodeRate = DI_CH_SCODERATE_3_4;
		stSatTuneP.etPolarization = DI_CH_POLAR_HOR;
		stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS;
		stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_D;
		stSatTuneP.b22kTone = FALSE;

		stSatTuneP.etModulation = DI_CH_PSK_QPSK;
		stSatTuneP.etPilot = DI_CH_PILOT_AUTO;
		stSatTuneP.etRollOff = DI_CH_ROLL_035;
		stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
		stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
		stSatTuneP.ulScdUserBandNum = 0;
		stSatTuneP.ulScdUserBandFreq = 0;
		*/

	}
	else // HD
	{
		stSatTuneP.ulFrequency = 1100; //000; //unit KHz
		stSatTuneP.ulSymbolRate = 27403;
		stSatTuneP.etFecCodeRate =  DI_CH_S2CODERATE_9_10;//DI_CH_SCODERATE_AUTO; //
		stSatTuneP.etPolarization = DI_CH_POLAR_HOR;
		stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS2;
		stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_D;
		stSatTuneP.etToneBurst = DI_CH_TONE_BURST_NONE;
		stSatTuneP.b22kTone = TRUE;

		stSatTuneP.etModulation = DI_CH_PSK_QPSK;
		stSatTuneP.etPilot = DI_CH_PILOT_OFF;
		stSatTuneP.etRollOff = DI_CH_ROLL_035;
		stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
		stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
		stSatTuneP.ulScdMasterUnitId = 0;
		stSatTuneP.ulScdUserBandNum = 0;
		stSatTuneP.ulScdUserBandFreq = 0;

		/* Fashion TV HD
		stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS2;
		stSatTuneP.etModulation = DI_CH_PSK_8PSK;
		stSatTuneP.ulFrequency = 1356;  //Freq:3794 ,LNB:5150
		stSatTuneP.ulSymbolRate = 4640;
		stSatTuneP.etPolarization = DI_CH_POLAR_HOR;
		stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_B;
		stSatTuneP.b22kTone = TRUE;

		stSatTuneP.etFecCodeRate = DI_CH_S2CODERATE_3_5;
		stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
		stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
		stSatTuneP.etRollOff = DI_CH_ROLL_035;
		stSatTuneP.etPilot = DI_CH_PILOT_OFF;
		stSatTuneP.ulScdUserBandNum = 0;
		stSatTuneP.ulScdUserBandFreq = 0;
		*/
	}

	ulRequestId++;

	DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);

}

static void HwTest_ChTuneTest(int nUnitId , unsigned char ucIsSDchannel)
{
	int nRet = 0;//, i;
	HUINT32 ulNumOfInstance = 0;
	//HUINT32 ulChannelType = 0;
	//HUINT8   ucChannelSD = 0;


	nRet = DI_CH_GetNumOfInstance(&ulNumOfInstance);
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ("[HwTest_ChTuneTest] ERR!\n "));
		return;
	}

	if ( nUnitId >= (int)ulNumOfInstance)
	{
		CH_DI_Print(0, ("[HwTest_ChTuneTest] ERR! nUnitId = %d \n ", nUnitId));
		return;
	}

	//DI_CH_S_ActivateChannel(nUnitId);
	HwTest_ChTuneSat(nUnitId, ucIsSDchannel);
	//DI_CH_S_DeactivateChannel(nUnitId);

	//VK_TASK_Sleep(1500);

}
#endif

static void HwTest_ChInit(void)
{
	int nRet = 0, i;
	HUINT32 ulNumOfInstance = 0;
	HUINT32 ulChannelType = 0;
	//HUINT8   ucChannelSD = 0;

	if(ucIsInitDone != 1)
	{
		nRet = DI_CH_GetNumOfInstance(&ulNumOfInstance);
		if ( nRet != DI_CH_OK )
		{
			CH_DI_Print(0, ("[HwTest_ChInit] ERR!DI_CH_GetNumOfInstance()  \n "));
			return;
		}

		for ( i=0; i < (int)ulNumOfInstance; i++ )
		{
			nRet = DI_CH_GetCapability( i, &ulChannelType);
			if ( nRet != DI_CH_OK )
			{
				CH_DI_Print(0, ("[HwTest_ChInit] ERR!DI_CH_GetCapability()  \n "));
				return;
			}
			else
			{
				CH_DI_Print(4, ("InstanceId[%d], Type[0x%x]\n", i,  ulChannelType ));
			}
#if defined(CONFIG_FRONTEND_SAT)
			DI_CH_S_RegisterCallback( i, HwTest_ChSatCallback);
#endif
#if defined(CONFIG_FRONTEND_CAB)
			DI_CH_C_RegisterCallback( i, HwTest_ChCabCallback);
#endif
#if defined(CONFIG_FRONTEND_TER)
			DI_CH_T_RegisterCallback( i, HwTest_ChTerCallback);
#endif

		}

		ucIsInitDone = 1;
	}
	else
	{
		CH_DI_Print(4, ("[HwTest_ChInit] HwTest_ChInit Done (%d) \n" , ucIsInitDone));
	}
}

#if defined(CONFIG_FRONTEND_SAT)
static int HwTest_ChInfoString (HUINT32 ulInstanceId, HINT32 ber, HUINT32 uncorrect, float power, char* pStr)
{
	int nRet = DI_CH_OK;
	CH_SAT_TuneParam_t stSatLockedP;
	char caTempStr[500];
    unsigned int tempStrIndex = 0;
    unsigned int strIndex= 0;
	VK_memset(caTempStr, 0x00, 500);

	nRet = DI_CH_S_GetLockedInfo(ulInstanceId, &stSatLockedP );
	if( nRet != DI_CH_OK)
	{
		CH_DI_Print(0, ("ERR!nRet(0x%x) UpdateLockedParam \n", nRet));
	}

	/* Trans Mode */
	if(stSatLockedP.etTransSpec == DI_CH_TRANS_DVBS)
	{
		CH_DI_Print(4, ("Mode : DVB-S \n"));
		VK_snprintf(caTempStr, 7, "DVB-S, ");
        tempStrIndex = 7;
	}
	else if(stSatLockedP.etTransSpec == DI_CH_TRANS_DVBS2)
	{
		CH_DI_Print(4, ("Mode : DVB-S2 \n"));
		VK_snprintf(caTempStr,8, "DVB-S2, ");
        tempStrIndex = 8;
	}
	else
    {
		CH_DI_Print(4, ("Mode : %d \n" , stSatLockedP.etTransSpec));
    	VK_snprintf(caTempStr,9, "UNKNWON, ");
        tempStrIndex = 9;
    }

    VK_MEM_Memcpy(&pStr[strIndex], caTempStr, tempStrIndex);
    strIndex = strIndex + tempStrIndex;

	/* Frequency */
	//CH_DI_Print("Frequency : %d MHz\n" , stSatLockedP.ulFrequency);
	VK_snprintf(caTempStr, 10, "%04d MHz, ", stSatLockedP.ulFrequency);
    tempStrIndex = 10;
    VK_MEM_Memcpy(&pStr[strIndex], caTempStr, tempStrIndex);
    strIndex = strIndex + tempStrIndex;

	/* Modulation */
	if(stSatLockedP.etModulation == DI_CH_PSK_QPSK)
	{
		CH_DI_Print(4, ("Modulation : QPSK \n"));
		VK_snprintf(caTempStr, 6, "QPSK, ");
        tempStrIndex = 6;
	}
	else if(stSatLockedP.etModulation == DI_CH_PSK_8PSK)
	{
		CH_DI_Print(4, ("Modulation : 8QPSK \n"));
		VK_snprintf(caTempStr, 7, "8QPSK, ");
        tempStrIndex = 7;
	}
	else
    {
		CH_DI_Print(4, ("%d, ", stSatLockedP.etModulation));
    	VK_snprintf(caTempStr,9, "UNKNWON, ");
        tempStrIndex = 9;
    }
    VK_MEM_Memcpy(&pStr[strIndex], caTempStr, tempStrIndex);
    strIndex = strIndex + tempStrIndex;

	/* Symbol rate*/
	CH_DI_Print(4, ("Symbol : %d Mbps\n" , stSatLockedP.ulSymbolRate));
	VK_snprintf(caTempStr, 12, "%05d Mbps, ", stSatLockedP.ulSymbolRate);
    tempStrIndex = 12;
    VK_MEM_Memcpy(&pStr[strIndex], caTempStr, tempStrIndex);
    strIndex = strIndex + tempStrIndex;

	/* Coderate */
	if(stSatLockedP.etFecCodeRate== DI_CH_SCODERATE_AUTO)
	 {
		CH_DI_Print(4, ("Coderate : AUTO \n"));
		VK_snprintf(caTempStr, 6, "AUTO, ");
        tempStrIndex = 6;
	 }
	else if(stSatLockedP.etFecCodeRate== DI_CH_SCODERATE_1_2)
	{	CH_DI_Print(4, ("Coderate : 1/2 \n"));
		VK_snprintf(caTempStr, 5, "%s, ", "1/2");
        tempStrIndex = 5;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_SCODERATE_2_3)
	{
		CH_DI_Print(4, ("Coderate : 2/3 \n"));
		VK_snprintf(caTempStr,5, "%s, ", "2/3");
        tempStrIndex = 5;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_SCODERATE_3_4)
	{
		CH_DI_Print(4, ("Coderate : 3/4 \n"));
		VK_snprintf(caTempStr, 5, "%s, ", "3/4");
        tempStrIndex = 5;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_SCODERATE_5_6)
	{
		CH_DI_Print(4, ("Coderate : 5/6 \n"));
		VK_snprintf(caTempStr, 5, "%s, ", "5/6");
        tempStrIndex = 5;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_SCODERATE_7_8)
	{
		CH_DI_Print(4, ("Coderate : 7/8 \n"));
		VK_snprintf(caTempStr, 5, "%s, ", "7/8");
        tempStrIndex = 5;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_S2CODERATE_3_5)
	{
		CH_DI_Print(4, ("Coderate : 3/5 \n"));
		VK_snprintf(caTempStr, 5, "%s, ", "3/5");
        tempStrIndex = 5;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_S2CODERATE_4_5)
	{
		CH_DI_Print(4, ("Coderate : 4/5 \n"));
		VK_snprintf(caTempStr, 5, "%s, ", "4/5");
        tempStrIndex = 5;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_S2CODERATE_8_9)
	{
		CH_DI_Print(4, ("Coderate : 8/9 \n"));
		VK_snprintf(caTempStr, 5, "%s, ", "8/9");
        tempStrIndex = 5;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_S2CODERATE_9_10)
	{
		CH_DI_Print(4, ("Coderate : 9/10 \n"));
		VK_snprintf(caTempStr, 6, "%s, ", "9/10");
        tempStrIndex = 6;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_SCODERATE_5_11)
	{
		CH_DI_Print(4, ("Coderate : 5/11 \n"));
		VK_snprintf(caTempStr, 6, "%s, ", "5/11");
        tempStrIndex = 6;
	}
	else if(stSatLockedP.etFecCodeRate== DI_CH_SCODERATE_6_7)
	{
		CH_DI_Print(4, ("Coderate : 6/7 \n"));
		VK_snprintf(caTempStr, 5, "%s, ", "6/7");
        tempStrIndex = 5;
	}
	else
    {
		CH_DI_Print(4, ("%d, ", stSatLockedP.etFecCodeRate));
    	VK_snprintf(caTempStr,9, "UNKNWON, ");
        tempStrIndex = 9;

    }

    VK_MEM_Memcpy(&pStr[strIndex], caTempStr, tempStrIndex);
    strIndex = strIndex + tempStrIndex;

	/* Pilot */
	if(stSatLockedP.etPilot == DI_CH_PILOT_AUTO)
	{
		CH_DI_Print(4, ("Pilot : AUTO \n"));
		VK_snprintf(caTempStr, 6, "AUTO, ");
        tempStrIndex = 6;
	}
	else if(stSatLockedP.etPilot == DI_CH_PILOT_OFF)
	{
		CH_DI_Print(4, ("Pilot : OFF \n"));
		VK_snprintf(caTempStr, 5, "OFF, ");
        tempStrIndex = 5;
    }
	else if(stSatLockedP.etPilot == DI_CH_PILOT_ON)
	{
		CH_DI_Print(4, ("Pilot : ON \n"));
		VK_snprintf(caTempStr, 4, "ON, ");
        tempStrIndex = 4;
    }
	else
    {
      	VK_snprintf(caTempStr,9, "UNKNWON, ");
        tempStrIndex = 9;
		CH_DI_Print(4, ("%d \n", stSatLockedP.etPilot));
    }

    VK_MEM_Memcpy(&pStr[strIndex], caTempStr, tempStrIndex);
    strIndex = strIndex + tempStrIndex;

	CH_DI_Print(4, ("SNR : %03d \n", ber));
	VK_snprintf(caTempStr, 12, "SNR : %03d , ", ber);
    tempStrIndex =12;
    VK_MEM_Memcpy(&pStr[strIndex], caTempStr, tempStrIndex);
    strIndex = strIndex + tempStrIndex;

	CH_DI_Print(4, ("AGC  : %2.2f \n", power));
	VK_snprintf(caTempStr, 16,  "Power : %2.2f , ", power);
    tempStrIndex =16;
    VK_MEM_Memcpy(&pStr[strIndex], caTempStr, tempStrIndex);
    strIndex = strIndex + tempStrIndex;

	CH_DI_Print(4, ("Uncorrected  : %d \n", uncorrect));
	VK_snprintf(caTempStr, 20, "Uncorrected : %03d \n", uncorrect);
    tempStrIndex =20;
    VK_MEM_Memcpy(&pStr[strIndex], caTempStr, tempStrIndex);
    strIndex = strIndex + tempStrIndex;
	//CMD_Printf("%s", pStr);

	return nRet;
}
#endif

#if defined(CONFIG_FRONTEND_SAT)
int CMD_HwTest_Parser_ChLock_QPSK(void *szArgs)
{
	int iResult=CMD_ERR;

	HUINT32 ulInstanceId;
	int iUnitId = 0;

	CH_SAT_TuneParam_t stSatTuneP;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0))
		{

			/* [unit] */
			VK_sscanf(szUnitId, "%d", &iUnitId);

			ulInstanceId = iUnitId;
			if(ulInstanceId > NUM_OF_CHANNEL)
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
				return iResult;
			}

			g_hwtest_lnb_off = 0;

			/* Ch Type : SAT */
			stSatTuneP.etAntennaType = DI_CH_ANT_TYPE_DISEQC;
			/* [trans] */
			stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS;
			/* [modu] */
			stSatTuneP.etModulation = DI_CH_PSK_QPSK;
			/* [freq] */
			stSatTuneP.ulFrequency = 1700;  //Freq:12300, LNB:10600
			/* [sr] */
			stSatTuneP.ulSymbolRate = 27500;
			/* [polar] */
			stSatTuneP.etPolarization = DI_CH_POLAR_HOR;
			/* [diseqc] */
			stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_D;

			/* [tone] */
			stSatTuneP.etToneBurst = DI_CH_TONE_BURST_NONE;
			stSatTuneP.b22kTone = FALSE;

			/* common param */
			stSatTuneP.etFecCodeRate = DI_CH_SCODERATE_AUTO;
			stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
			stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
			stSatTuneP.etRollOff = DI_CH_ROLL_035;
			stSatTuneP.etPilot = DI_CH_PILOT_OFF;

			stSatTuneP.ulScdMasterUnitId = 0;
			stSatTuneP.ulScdUserBandNum = 0;
			stSatTuneP.ulScdUserBandFreq = 0;

			ulRequestId++;

			DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);

			/*
			VK_TASK_Sleep(500); 	//0.5Sec sleep
			if (bTunelocked != TRUE )
			{
				CMD_Printf("CH[%d] UNLOCK !! \n", ulInstanceId);
			}
			else
			{
				CMD_Printf("CH[%d] LOCK ~~! \n", ulInstanceId);
			}
			*/
			iResult=CMD_OK;
		}
	}
	else
	{
		CMD_Printf("ERR! Bad Command Iput. \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_ChLock_VER(void *szArgs)
{
	int iResult=CMD_ERR;

	HUINT32 ulInstanceId;
	int iUnitId = 0, isSDVertical = 0;

	CH_SAT_TuneParam_t stSatTuneP;
	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"sd")==0)
	{
		isSDVertical = 1;
	}
	else if (szCmd!=NULL && VK_strcmp(szCmd,"hd")==0)
	{
		isSDVertical = 0;
	}
	else
	{
		isSDVertical= -1;
		CMD_Printf("ERR! Bad Input Param \n");
		return iResult;
	}

	char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
	if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0))
	{
		/* [unit] */
        if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
        {
            return CMD_ERR;
        }



		ulInstanceId = iUnitId;
		if(ulInstanceId > NUM_OF_CHANNEL)
    	{
			CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
			return iResult;
		}

		g_hwtest_lnb_off = 0;

		/* Ch Type : SAT */
		stSatTuneP.etAntennaType = DI_CH_ANT_TYPE_DISEQC;

		if (isSDVertical == 1)
		{
			/* [trans] */
			stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS;
			/* [modu] */
			stSatTuneP.etModulation = DI_CH_PSK_QPSK;
			/* [freq] */
			stSatTuneP.ulFrequency = 1130;  //Freq:4020 ,LNB:5150
			/* [sr] */
			stSatTuneP.ulSymbolRate = 27250;
			/* [polar] */
			stSatTuneP.etPolarization = DI_CH_POLAR_VER;
			/* [diseqc] */
			stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_B;
			/* [tone] */
			stSatTuneP.etToneBurst = DI_CH_TONE_BURST_NONE;
			stSatTuneP.b22kTone = TRUE;
			/* [coderate] */
			stSatTuneP.etFecCodeRate = DI_CH_SCODERATE_3_4;
			/* common param */
			stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
			stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
			stSatTuneP.etRollOff = DI_CH_ROLL_035;
			stSatTuneP.etPilot = DI_CH_PILOT_OFF;
			stSatTuneP.ulScdMasterUnitId = 0;
			stSatTuneP.ulScdUserBandNum = 0;
			stSatTuneP.ulScdUserBandFreq = 0;
		}
		else if (isSDVertical == 0) // HD VER
		{
			/* Fashion TV HD */
			stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS2;
			stSatTuneP.etModulation = DI_CH_PSK_8PSK;
			stSatTuneP.ulFrequency = 1356;	//Freq:3794 ,LNB:5150
			stSatTuneP.ulSymbolRate = 4640;
			stSatTuneP.etPolarization = DI_CH_POLAR_HOR;
			stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_B;
			stSatTuneP.etToneBurst = DI_CH_TONE_BURST_NONE;
			stSatTuneP.b22kTone = TRUE;
			stSatTuneP.etFecCodeRate = DI_CH_S2CODERATE_3_5;
			stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
			stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
			stSatTuneP.etRollOff = DI_CH_ROLL_035;
			stSatTuneP.etPilot = DI_CH_PILOT_OFF;
			stSatTuneP.ulScdMasterUnitId = 0;
			stSatTuneP.ulScdUserBandNum = 0;
			stSatTuneP.ulScdUserBandFreq = 0;
		}
		else
		{
			CMD_Printf("ERR! Bad Input...  isSDVertical [%d] \n", isSDVertical);
			return iResult;
		}
		ulRequestId++;

		DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);

		/*
		VK_TASK_Sleep(500); 	//0.5Sec sleep
		if (bTunelocked != TRUE )
		{
			CMD_Printf("CH[%d] UNLOCK !! \n", ulInstanceId);
		}
		else
		{
			CMD_Printf("CH[%d] LOCK ~~! \n", ulInstanceId);
		}
		*/
		iResult=CMD_OK;

	}
	else
	{
		CMD_Printf("ERR! Bad Command Iput. \n");
	}

	return iResult;
}

int  CMD_HwTest_Parser_ChLock_SD(void *szArgs)
{
	int iResult=CMD_ERR;

	HUINT32 ulInstanceId;
	int iUnitId;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"lock")==0)
	{

		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);

		if (szUnitId!=NULL )
		{
            if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return CMD_ERR;
            }


			ulInstanceId = iUnitId;
			if(ulInstanceId > NUM_OF_CHANNEL)
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
				return iResult;
			}

			HwTest_ChInit();

			if(ulInstanceId == 0)
			{
				CMD_Printf(" [0]Ch SD Locking... \n");
				HwTest_ChTuneTest(0, 1);
			}
			else
			{
				CMD_Printf(" [1]Ch SD Locking... \n");
				HwTest_ChTuneTest(1, 1);
			}
			/*
			if(ucIsLocked[ulInstanceId] == 1)
				CMD_Printf("CH[%d] LOCK ~~! \n", ulInstanceId);
			else
				CMD_Printf("CH[%d] UNLOCK !!! \n", ulInstanceId);
			*/
		}

		iResult=CMD_OK;
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

int  CMD_HwTest_Parser_ChLock_HD(void *szArgs)
{
	int iResult=CMD_ERR;

	HUINT32 ulInstanceId;

	int iUnitId;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"lock")==0)
	{

		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);

		if (szUnitId!=NULL )
		{
            if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return CMD_ERR;
            }

			ulInstanceId = iUnitId;
			if(ulInstanceId > NUM_OF_CHANNEL)
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
				return iResult;
			}

			HwTest_ChInit();

			if(ulInstanceId == 0)
			{
				CMD_Printf(" [0]Ch HD Locking... \n");
				HwTest_ChTuneTest(0, 0);
			}
			else
			{
				CMD_Printf(" [1]Ch HD Locking... \n");
				HwTest_ChTuneTest(1, 0);
			}
			/*
			if(ucIsLocked[ulInstanceId] == 1)
				CMD_Printf("CH[%d] LOCK ~~! \n", ulInstanceId);
			else
				CMD_Printf("CH[%d] UNLOCK !!! \n", ulInstanceId);
			*/

		}

		iResult=CMD_OK;
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}
#endif

#if defined(CONFIG_FRONTEND_CAB)
int CMD_HwTest_Parser_Chlock_CAB(void *szArgs)
{
	int iResult=CMD_ERR;
	HUINT32 ulInstanceId;
	int iUnitId;

	CH_CAB_TuneParam_t stCabChannelParam;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	 if(szCmd!=NULL && VK_strcmp(szCmd,"lock")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if (szUnitId!=NULL)
		{
            if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return CMD_ERR;
            }

			ulInstanceId = iUnitId;
			if(ulInstanceId > NUM_OF_CHANNEL)
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
				return CMD_ERR;
			}

			HwTest_ChInit();

			/* CAB */
			stCabChannelParam.ulFrequency = 674 * 1000;  /*unit KHz*/

			stCabChannelParam.ulSymbolRate= 5273;

			stCabChannelParam.etSpectrum = DI_CH_INVERSION_AUTO;

			stCabChannelParam.etModulation = DI_CH_CMOD_64QAM;

			ulRequestId++;

			if(s_CurrentType[ulInstanceId] != DI_CH_TYPE_CAB)
			{
#if defined(CONFIG_FRONTEND_TER)
				DI_CH_T_CancelLock( ulInstanceId );
				DI_CH_T_DeactivateChannel( ulInstanceId );
#endif

#if defined (CONFIG_PLATFORM_DP10023)
				DI_CH_C_ResetGpio(0);
#endif
				DI_CH_C_ActivateChannel( ulInstanceId );

#if defined (CONFIG_PLATFORM_DP10023)
				DI_CH_C_Init(0);
#endif
			}
			s_CurrentType[ulInstanceId] = DI_CH_TYPE_CAB;

			DI_CH_C_RequestLock( ulInstanceId, ulRequestId, &stCabChannelParam);

			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("ERR! Bad Param Iput. \n");
		}
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}


int CMD_HwTest_Parser_ClearErrCount(void *szArgs)
{
	int iResult=CMD_ERR;
	int iUnitId;
	HUINT32 ulInstanceId;
/*
#if defined (CONFIG_PLATFORM_DP10024)
	HINT32 nSnr;
	HINT32 nber_decimal;
	HINT32 nber_float;
	HINT32 nber_exp;
	HINT32 nUncorrected;
	HINT32 nSignallevel;
#endif
*/
	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		CMD_Printf("SAT : Not support. \n");

	}
	else if (szCmd!=NULL && VK_strcmp(szCmd,"cab")==0)
	{

		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if (szUnitId!=NULL)
		{
            if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return CMD_ERR;
            }
			ulInstanceId = iUnitId;
			if(ulInstanceId > NUM_OF_CHANNEL)
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
				return CMD_ERR;
			}

			CMD_Printf(" Clear Error Count[%d] \n", ulInstanceId);
/*
#if defined (CONFIG_PLATFORM_DP10024)
			DP10024_ClearErrCount(ulInstanceId);
			DI_CH_C_GetLockInfo(ulInstanceId, &nSnr, &nber_decimal, &nber_float, &nber_exp, &nUncorrected, &nSignallevel);
#endif
*/
			iResult=CMD_OK;
		}
		else if (szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
		{
			CMD_Printf("TER : Not support. \n");
		}
		else
		{
			CMD_Printf("ERR! Bad Param Input. \n");
		}

	}
	else
	{
		CMD_Printf("ERR! Bad Comman Input. \n");
	}


	return iResult;

}


#endif

#if defined(CONFIG_FRONTEND_TER)
int CMD_HwTest_Parser_Chlock_TER(void *szArgs)
{
	int iResult=CMD_ERR;

	HUINT32 ulInstanceId=0;

	CH_TER_TuneParam_t stTerChannelParam;

	int iUnitId=0;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	 if(szCmd!=NULL && VK_strcmp(szCmd,"lock")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);

		if (szUnitId!=NULL)
		{
            if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return CMD_ERR;
            }

			ulInstanceId = iUnitId;
			if(ulInstanceId > NUM_OF_CHANNEL)
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
				return CMD_ERR;
			}

			HwTest_ChInit();

			/* TER */
			stTerChannelParam.ulFrequency = 767 * 1000; /*unit KHz*/

			stTerChannelParam.etBandWidth =   DI_CH_BW_6MHZ;
			stTerChannelParam.etCodeRate = DI_CH_TCODERATE_AUTO;
			stTerChannelParam.etGuardInterval = DI_CH_GUARDINTERVAL_AUTO;
			stTerChannelParam.etHierarchy = DI_CH_HIERARCHY_AUTO;

			stTerChannelParam.etModulation =DI_CH_TMOD_64QAM ;

			stTerChannelParam.etOffset = DI_CH_OFFSET_AUTO;
			stTerChannelParam.etStream = DI_CH_STREAM_HP;
			stTerChannelParam.etTransMode = DI_CH_TRANSMODE_AUTO;

			ulRequestId++;

			if(s_CurrentType[ulInstanceId] != DI_CH_TYPE_TER)
			{
#if defined(CONFIG_FRONTEND_CAB)
				DI_CH_C_CancelLock( ulInstanceId );
				DI_CH_C_DeactivateChannel( ulInstanceId );
#endif

#if defined (CONFIG_PLATFORM_DT90507) || defined (CONFIG_PLATFORM_DT90517)
				DI_CH_T_ResetGpio(0);
#endif
				DI_CH_T_ActivateChannel( ulInstanceId );

#if defined (CONFIG_PLATFORM_DT90507) || defined (CONFIG_PLATFORM_DT90517)
				DI_CH_T_Init(0);
#endif
			}

			s_CurrentType[ulInstanceId] = DI_CH_TYPE_TER;

			DI_CH_T_RequestLock( ulInstanceId, ulRequestId, &stTerChannelParam);
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("ERR! Bad Param Iput. \n");
		}
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}
//JAMES
int CMD_HwTest_Parser_Dump_TER(void *szArgs)
{
	int iResult=CMD_ERR;
	int nUnitID = 0;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd != NULL)
	{
		if ( ( READABLE_IN_DEC(szCmd, nUnitID) != 1 ) )
		{
		    return 1;
		}

		CMD_Printf("[CMD_HwTest_Parser_Dump_TER] nUnitID = %d  \n", nUnitID);

		DI_CH_T_Requestdump(nUnitID);
	}

	return CMD_OK;
}

int CMD_HwTest_Parser_READ_TER(void *szArgs)
{
	int iResult=CMD_ERR;
	int nUnitID = 0;
	int nAddr = 0;
	int nData = 0;

	char *szChID=(char *)CMD_NextArg((char **)&szArgs);
	char *szAddr=(char *)CMD_NextArg((char **)&szArgs);

	if ( (szChID != NULL) && (szAddr != NULL) )
	{
		if ( ( READABLE_IN_DEC(szChID, nUnitID) != 1 ) ||
			( READABLE_IN_HEX(szAddr, nAddr) != 1 ) )
		{
		    return 1;
		}

		CMD_Printf("[CMD_HwTest_Parser_WRITE_TER] nUnitID = %d, Addr = %x,  \n", nUnitID, nAddr);

		DI_CH_T_READ(nUnitID, nAddr);
	}

	return CMD_OK;
}

int CMD_HwTest_Parser_WRITE_TER(void *szArgs)
{
	int iResult=CMD_ERR;
	int nUnitID = 0;
	int nAddr = 0;
	int nData = 0;

	char *szChID=(char *)CMD_NextArg((char **)&szArgs);
	char *szAddr=(char *)CMD_NextArg((char **)&szArgs);
	char *szData=(char *)CMD_NextArg((char **)&szArgs);

	if ( (szChID != NULL) && (szAddr != NULL) && (szData != NULL) )
	{
		if ( ( READABLE_IN_DEC(szChID, nUnitID) != 1 ) ||
			( READABLE_IN_HEX(szAddr, nAddr) != 1 ) ||
			( READABLE_IN_HEX(szData, nData) != 1 ) )
		{
		    return 1;
		}

		CMD_Printf("[CMD_HwTest_Parser_WRITE_TER] nUnitID = %d, Addr = %x, Data = %x  \n", nUnitID, nAddr, nData);

		DI_CH_T_WRITE(nUnitID, nAddr, nData);
	}

	return CMD_OK;
}

#endif

int CMD_HwTest_Parser_ChLock(void *szArgs)
{
	int iResult=CMD_ERR;

	HUINT32 ulInstanceId;

#if defined(CONFIG_FRONTEND_SAT)
	CH_SAT_TuneParam_t stChannelParam;
#endif

#if defined(CONFIG_FRONTEND_CAB)
	CH_CAB_TuneParam_t stCabChannelParam;
#endif

#if defined(CONFIG_FRONTEND_TER)
	CH_TER_TuneParam_t stTerChannelParam;
#endif

#if defined(CONFIG_DEMOD_TSMF)
	int nTsid;
	int nNetid;
#endif
	int iUnitId;
	int iModulation;
	int iFreq;
	int iSr;

#if defined(CONFIG_FRONTEND_SAT)
	int iFec;
	int iDVBMode;
	int iPolar;
	int iDiseqc;
	int iTone;
	int iPilot;
#endif

#if defined(CONFIG_FRONTEND_TER)
	int iBandWidth;
	int iSystem;
#endif
	//char bTunelocked=0;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

#if defined(CONFIG_FRONTEND_SAT)
	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		char *szDVBMode=(char *)CMD_NextArg((char **)&szArgs);
		char *szModulation=(char *)CMD_NextArg((char **)&szArgs);
		char *szFreq=(char *)CMD_NextArg((char **)&szArgs);
		char *szSr=(char *)CMD_NextArg((char **)&szArgs);
		char *szFec=(char *)CMD_NextArg((char **)&szArgs);
		char *szPolar=(char *)CMD_NextArg((char **)&szArgs);
		char *szDiseqc=(char *)CMD_NextArg((char **)&szArgs);
		char *szTone=(char *)CMD_NextArg((char **)&szArgs);
		char *szPilot=(char *)CMD_NextArg((char **)&szArgs);

		if (szUnitId!=NULL && szDVBMode!=NULL && szModulation!=NULL && szFreq!=NULL
			&& szSr!=NULL && szPolar!=NULL && szDiseqc!=NULL && szTone!=NULL )
		{
            if ( (READABLE_IN_DEC(szUnitId, iUnitId) != 1 ) ||
                 (READABLE_IN_DEC(szDVBMode, iDVBMode) != 1 ) ||
                 (READABLE_IN_DEC(szModulation, iModulation) != 1 ) ||
                 (READABLE_IN_DEC(szFreq, iFreq) != 1 ) ||
                 (READABLE_IN_DEC(szSr, iSr) != 1 ) ||
                 (READABLE_IN_DEC(szFec, iFec) != 1 ) ||
                 (READABLE_IN_DEC(szPolar, iPolar) != 1 ) ||
                 (READABLE_IN_DEC(szDiseqc, iDiseqc) != 1 ) ||
                 (READABLE_IN_DEC(szTone, iTone) != 1 ) )
            {
                return CMD_ERR;
            }



			if(szPilot == NULL)
			{
				iPilot = 0;
				CMD_Printf("No Input Pilot: Default  Pilot-Auto \n ");
			}
			else
			{
				VK_sscanf(szPilot, "%d", &iPilot);
				CMD_Printf(" Input Pilot: iPilot = %d \n ", iPilot);
			}

			ulInstanceId = iUnitId;
			if(ulInstanceId > NUM_OF_CHANNEL)
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
				return CMD_ERR;
			}

			HwTest_ChInit();

			g_hwtest_lnb_off = 0;

			/* SAT */
			stChannelParam.etAntennaType = DI_CH_ANT_TYPE_DISEQC;

			if(iDVBMode ==0)
				stChannelParam.etTransSpec = DI_CH_TRANS_DVBS;
			else if (iDVBMode== 1)
				stChannelParam.etTransSpec = DI_CH_TRANS_DVBS2;

			stChannelParam.ulFrequency = (unsigned long)iFreq;  /*unit MHz*/

			stChannelParam.ulSymbolRate = (unsigned short)iSr;

			if(iPolar == 0)
				stChannelParam.etPolarization = DI_CH_POLAR_HOR;
			else if(iPolar == 1)
				stChannelParam.etPolarization = DI_CH_POLAR_VER;
			else
				stChannelParam.etPolarization = DI_CH_POLAR_AUTO;

			if(iModulation == 0)
				stChannelParam.etModulation = DI_CH_PSK_AUTO;
			else if(iModulation == 1)
				stChannelParam.etModulation = DI_CH_PSK_QPSK;
			else if (iModulation == 2)
				stChannelParam.etModulation = DI_CH_PSK_8PSK;

			if(iFec == 0)
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_AUTO;
			else if(iFec == 1)
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_1_2;
			else if(iFec == 2)
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_2_3;
			else if(iFec == 3)
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_3_4;
			else if(iFec == 4)
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_5_6;
			else if(iFec == 5)
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_7_8;
			///// DVB S2 ////////////////////////////////////////////
			else if(iFec == 6)
				stChannelParam.etFecCodeRate = DI_CH_S2CODERATE_3_5;
			else if(iFec == 7)
				stChannelParam.etFecCodeRate = DI_CH_S2CODERATE_4_5;
			else if(iFec == 8)
				stChannelParam.etFecCodeRate = DI_CH_S2CODERATE_8_9;
			else if(iFec == 9)
				stChannelParam.etFecCodeRate = DI_CH_S2CODERATE_9_10;
			////////////////////////////////////////////////////////
			else if(iFec == 10)
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_5_11;
			else if(iFec == 11)
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_6_7;
			else
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_AUTO;

			stChannelParam.etDiseqcInput = (unsigned char)iDiseqc;
			stChannelParam.b22kTone = (unsigned char)iTone;
			stChannelParam.etToneBurst = DI_CH_TONE_BURST_NONE;
			stChannelParam.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
			stChannelParam.etLnbVoltage = DI_CH_LNB_VOLT_STD;
			stChannelParam.etRollOff = DI_CH_ROLL_035;

			if(iPilot == 0)
				stChannelParam.etPilot = DI_CH_PILOT_AUTO;
			else if(iPilot == 1)
				stChannelParam.etPilot = DI_CH_PILOT_OFF;
			else if(iPilot == 2)
				stChannelParam.etPilot = DI_CH_PILOT_ON;
			else
				stChannelParam.etPilot = DI_CH_PILOT_AUTO;

			stChannelParam.ulScdMasterUnitId = 0;
			stChannelParam.ulScdUserBandNum = 0;
			stChannelParam.ulScdUserBandFreq = 0;

			ulRequestId++;

			DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stChannelParam);

			/*
			if(ucIsLocked[ulInstanceId] == 1)
				CMD_Printf("CH[%d] LOCK ~~! \n", ulInstanceId);
			else
				CMD_Printf("CH[%d] UNLOCK !!! \n", ulInstanceId);
			*/
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("ERR! Bad Param Iput. \n");
		}
	}
#endif

#if defined(CONFIG_FRONTEND_CAB)
	if(szCmd!=NULL && VK_strcmp(szCmd,"cab")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		char *szFreq=(char *)CMD_NextArg((char **)&szArgs);
		char *szSr=(char *)CMD_NextArg((char **)&szArgs);
		char *szModulation=(char *)CMD_NextArg((char **)&szArgs);
#if defined(CONFIG_DEMOD_TSMF)
		char *szTsid=(char *)CMD_NextArg((char **)&szArgs);
		char *szNetid=(char *)CMD_NextArg((char **)&szArgs);
#endif

#if defined(CONFIG_DEMOD_TSMF)
		if (szUnitId!=NULL && szFreq!=NULL && szModulation!=NULL && szSr!=NULL && szTsid!=NULL && szNetid!=NULL)
#else
		if (szUnitId!=NULL && szFreq!=NULL && szModulation!=NULL && szSr!=NULL)
#endif
		{
			VK_sscanf(szUnitId, "%d", &iUnitId);
			VK_sscanf(szFreq, "%d", &iFreq);
			VK_sscanf(szSr, "%d", &iSr);
			VK_sscanf(szModulation, "%d", &iModulation);
#if defined(CONFIG_DEMOD_TSMF)
			VK_sscanf(szTsid, "%d", &nTsid);
			VK_sscanf(szNetid, "%d", &nNetid);
#endif
			ulInstanceId = iUnitId;
			if(ulInstanceId > NUM_OF_CHANNEL)
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
				return CMD_ERR;
			}

			HwTest_ChInit();

			/* CAB */
			stCabChannelParam.ulFrequency = (unsigned long)iFreq * 1000;  /*unit KHz*/

			stCabChannelParam.ulSymbolRate= (unsigned long)iSr;

			stCabChannelParam.etSpectrum = DI_CH_INVERSION_AUTO;

			if(iModulation == 0)
				stCabChannelParam.etModulation = DI_CH_CMOD_AUTO;
			else if(iModulation == 16)
				stCabChannelParam.etModulation = DI_CH_CMOD_16QAM;
			else if(iModulation == 32)
				stCabChannelParam.etModulation = DI_CH_CMOD_32QAM;
			else if(iModulation == 64)
				stCabChannelParam.etModulation = DI_CH_CMOD_64QAM;
			else if(iModulation == 128)
				stCabChannelParam.etModulation = DI_CH_CMOD_128QAM;
			else if(iModulation == 256)
				stCabChannelParam.etModulation = DI_CH_CMOD_256QAM;
			else
				stCabChannelParam.etModulation = DI_CH_CMOD_64QAM;

			ulRequestId++;

			if(s_CurrentType[ulInstanceId] != DI_CH_TYPE_CAB)
			{
#if defined(CONFIG_FRONTEND_TER)
				DI_CH_T_CancelLock( ulInstanceId );
				DI_CH_T_DeactivateChannel( ulInstanceId );
#endif

#if defined (CONFIG_PLATFORM_DP10023)
				DI_CH_C_ResetGpio(0);
#endif
				DI_CH_C_ActivateChannel( ulInstanceId );
			}

			s_CurrentType[ulInstanceId] = DI_CH_TYPE_CAB;

#if defined(CONFIG_DEMOD_TSMF)
			stCabChannelParam.nTsId = nTsid;
			stCabChannelParam.nNetId = nNetid;
			VK_TASK_Sleep(1000);
#endif
			DI_CH_C_RequestLock( ulInstanceId, ulRequestId, &stCabChannelParam);

			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("ERR! Bad Param Iput. \n");
		}
	}
		else if(szCmd!=NULL && VK_strcmp(szCmd,"cancle")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);

			ulInstanceId = iUnitId;
		VK_sscanf(szUnitId, "%d", &iUnitId);
		DI_CH_C_CancelLock(ulInstanceId);

			iResult=CMD_OK;
}

#endif

#if defined(CONFIG_FRONTEND_TER)

	if(szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		char *szFreq=(char *)CMD_NextArg((char **)&szArgs);
		char *szBandWidth=(char *)CMD_NextArg((char **)&szArgs);
		char *szModulation=(char *)CMD_NextArg((char **)&szArgs);
		char *szSystem=(char *)CMD_NextArg((char **)&szArgs);

		CH_UNUSED(iSr);
		if (szUnitId!=NULL && szFreq!=NULL && szModulation!=NULL && szBandWidth!=NULL)
		{
			VK_sscanf(szUnitId, "%d", &iUnitId);
			VK_sscanf(szFreq, "%d", &iFreq);
			VK_sscanf(szBandWidth, "%d", &iBandWidth);
			VK_sscanf(szModulation, "%d", &iModulation);

			if( szSystem != NULL )
			{
				VK_sscanf(szSystem, "%d", &iSystem);
			}
			else
			{
				iSystem = 0;
			}

			ulInstanceId = iUnitId;
			if(ulInstanceId > NUM_OF_CHANNEL)
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", ulInstanceId);
				return CMD_ERR;
			}

			HwTest_ChInit();

			/* TER */

			stTerChannelParam.ulFrequency = (unsigned long)iFreq * 1000;  /*unit KHz*/

			/* System */
			if(iSystem == 1)
				stTerChannelParam.etTerSystem = DI_CH_DVBT2;
			else
				stTerChannelParam.etTerSystem = DI_CH_DVBT;

			/* BandWidth */
			if(iBandWidth== 8)
				stTerChannelParam.etBandWidth = DI_CH_BW_8MHZ;
			else if(iBandWidth== 7)
				stTerChannelParam.etBandWidth = DI_CH_BW_7MHZ;
			else if(iBandWidth== 6)
				stTerChannelParam.etBandWidth = DI_CH_BW_6MHZ;
			else
				stTerChannelParam.etBandWidth = DI_CH_BW_6MHZ;


			/* Modulation */
			if(iModulation == 0)
				stTerChannelParam.etModulation = DI_CH_TMOD_AUTO;
			else if(iModulation == 1)
				stTerChannelParam.etModulation = DI_CH_TMOD_QPSK;
			else if(iModulation == 16)
				stTerChannelParam.etModulation = DI_CH_TMOD_16QAM;
			else if(iModulation == 64)
				stTerChannelParam.etModulation = DI_CH_TMOD_64QAM;
			else
				stTerChannelParam.etModulation = DI_CH_TMOD_64QAM;

			/* AUTO */
			stTerChannelParam.etCodeRate = DI_CH_TCODERATE_AUTO;
			stTerChannelParam.etGuardInterval = DI_CH_GUARDINTERVAL_AUTO;
			stTerChannelParam.etHierarchy = DI_CH_HIERARCHY_AUTO;

			stTerChannelParam.etOffset = DI_CH_OFFSET_AUTO;
			stTerChannelParam.etStream = DI_CH_STREAM_HP;
			stTerChannelParam.etTransMode = DI_CH_TRANSMODE_AUTO;

			if(s_CurrentType[ulInstanceId] != DI_CH_TYPE_TER)
			{
#if defined(CONFIG_FRONTEND_CAB)
				DI_CH_C_CancelLock( ulInstanceId );
				DI_CH_C_DeactivateChannel( ulInstanceId );
#endif

#if defined (CONFIG_PLATFORM_DT90507) || defined (CONFIG_PLATFORM_DT90517)
				DI_CH_T_ResetGpio(0);
#endif
				DI_CH_T_ActivateChannel( ulInstanceId );
			}

			s_CurrentType[ulInstanceId] = DI_CH_TYPE_TER;

			ulRequestId++;

			DI_CH_T_RequestLock( ulInstanceId, ulRequestId, &stTerChannelParam);

			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("ERR! Bad Param Iput. \n");
		}
	}
#endif

	if (szCmd ==NULL || (VK_strcmp(szCmd,"sat") !=0 && VK_strcmp(szCmd,"cab") !=0 && VK_strcmp(szCmd,"cancle") !=0 && VK_strcmp(szCmd,"ter") !=0))
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}


	if (iResult==CMD_ERR)
    {
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())("FAIL : RequestLock\n");
        }
    }
	return iResult;
}

int CMD_HwTest_Parser_ChLockStatus(void *szArgs)
{
	int iResult=CMD_ERR;
	int nRet = DI_CH_OK;
	int nIndex = 0;
#if defined(CONFIG_FRONTEND_SAT)
	CH_SAT_TuneParam_t stSatTune;
#endif
#if defined(CONFIG_FRONTEND_CAB)
	CH_CAB_TuneParam_t stCabTune;
#endif
#if defined(CONFIG_FRONTEND_TER)
	CH_TER_TuneParam_t stTerTune;
#endif

	for (nIndex = 0 ; nIndex < NUM_OF_CHANNEL ; nIndex++)
	{
		if(
#if defined(CONFIG_FRONTEND_SAT)
		(DI_CH_S_GetLockedInfo(nIndex, &stSatTune) == DI_CH_OK) ||
#endif
#if defined(CONFIG_FRONTEND_CAB)
		(DI_CH_C_GetLockedInfo(nIndex, &stCabTune) == DI_CH_OK) ||
#endif
#if defined(CONFIG_FRONTEND_TER)
		(DI_CH_T_GetLockedInfo(nIndex, &stTerTune) == DI_CH_OK) ||
#endif
			0)
		{
			CMD_Printf("Tuner [%d] = Locked\n", nIndex);
		}
		else
		{
			CMD_Printf("Tuner [%d] = Unlocked\n", nIndex);
		}
	}

	iResult=CMD_OK;

	return iResult;
}



int CMD_HwTest_Parser_ChGetDevStatus(void *szArgs)
{
	int iResult=CMD_ERR;
	int nRet = DI_CH_OK;
	int nIndex = 0;

	DI_CH_DeviceStatus stDevStatus;

#if defined(CONFIG_FRONTEND_SAT)
		return CMD_OK;
#endif

#if defined(CONFIG_FRONTEND_TER)
		return CMD_OK;
#endif

		if(
#if defined(CONFIG_FRONTEND_CAB)
		(DI_CH_C_GetDeviceStatus(0, &stDevStatus) == DI_CH_OK) ||
#endif
			0)
		{
			CMD_Printf("temperature = [%5.1f]degrees C\n", stDevStatus.temperature/1000.0);
		}
		else
		{
			CMD_Printf("Error : DI_CH_C_GetDeviceStatus()\n");
		}


	iResult=CMD_OK;

	return iResult;
}



int CMD_HwTest_Parser_ChStatus(void *szArgs)
{
	int iResult=CMD_ERR;
	int nRet = DI_CH_OK;

	HUINT32 ulInstanceId;
	int iUnitId;
/*
#if defined(CONFIG_FRONTEND_SAT)
	float power=0;	// Power estimation
	HINT32 ber=0;	// SNR estimation
	HUINT32 rfagc=0, ifagc=0, uncorrected=0;
#endif
*/
//JAMES
#if 0
#if defined(CONFIG_FRONTEND_TER)
	CH_TER_TuneParam_t   stTerLockedInfo;
	HBOOL bLocked = 0;
#elif defined(CONFIG_FRONTEND_CAB)
	CH_CAB_TuneParam_t	stCabLockedInfo;
	HBOOL bLocked = 0;
#endif
#else
#if defined(CONFIG_FRONTEND_TER)
	CH_TER_TuneParam_t   stTerLockedInfo;
#endif

#if defined(CONFIG_FRONTEND_CAB)
	CH_CAB_TuneParam_t	stCabLockedInfo;
#endif
	HBOOL bLocked = 0;

#endif

//#if !defined(CONFIG_PLATFORM_DP10024)
	DI_CH_SignalQuality_t stSignalInfo;
//#endif



	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

#if defined(CONFIG_FRONTEND_SAT)
	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
		    if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return CMD_ERR;
            }

			ulInstanceId = iUnitId;
			//CMD_Printf("ulInstanceId (%d) \n" , ulInstanceId);

			if(ulInstanceId <= NUM_OF_CHANNEL)
			{
				/* SAT */
//				nRet = DI_CH_S_GetChannelStatus(ulInstanceId, &ber, &rfagc, &ifagc, &uncorrected, &power);
				nRet = DI_CH_S_GetSignalInfo(ulInstanceId, &stSignalInfo);
				if( nRet != DI_CH_ERR )
				{
					static char parser_ch_status[500];
					VK_MEM_Memset(parser_ch_status, 0x00, 500);
					nRet = HwTest_ChInfoString(ulInstanceId, stSignalInfo.fBer, stSignalInfo.ulUnCorrectedNo, stSignalInfo.fSignalInputPower, &parser_ch_status[0]);
                    if ( CMD_GetMsgUI() != NULL )
                    {
                        (int *)(CMD_GetMsgUI())(parser_ch_status);
                    }
					iResult=CMD_OK;
				}
				else
				{
					CMD_Printf("CH[%d] UNLOCK... \n", iUnitId);
                    if ( CMD_GetMsgUI() != NULL )
                    {
                        (int *)(CMD_GetMsgUI())("UNLOCK\n");
                    }
					iResult=CMD_OK;
				}
			}
			else
			{
				CMD_Printf("ERR! Bad Unit ID...  CH[%d] \n", iUnitId);
			}

		}
		else
		{
			CMD_Printf("ERR! Bad Param Iput. \n");
		}

	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}
#endif

#if defined(CONFIG_FRONTEND_CAB)
/*
#if defined (CONFIG_PLATFORM_DP10024)
	 HINT32 nSnr;
	 HINT32 nber_decimal;
	 HINT32 nber_float;
	 HINT32 nber_exp;
	 HINT32 nUncorrected;
	 HINT32 nSignallevel;
#endif
*/
	 if(szCmd!=NULL && VK_strcmp(szCmd,"cab")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0))
		{
		    if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return CMD_ERR;
            }

			ulInstanceId = iUnitId;
			//CMD_Printf("ulInstanceId (%d)	\n", ulInstanceId );

			if(ulInstanceId <= NUM_OF_CHANNEL)
			{
				/* CAB */
				if(s_CurrentType[ulInstanceId] == DI_CH_TYPE_CAB)
				{
					bLocked = CH_C_CheckLock( ulInstanceId);

					if(bLocked == TRUE)
					{
						nRet= DI_CH_C_GetLockedInfo(ulInstanceId, &stCabLockedInfo);
						if( nRet != DI_CH_ERR )
						{
							CMD_Printf(" Frequency =%d , Modulation = %d, SymbolRate = %d\n ", stCabLockedInfo.ulFrequency, stCabLockedInfo.etModulation, stCabLockedInfo.ulSymbolRate);
						}
						nRet = DI_CH_C_GetSignalInfo(ulInstanceId, &stSignalInfo);
						if( nRet != DI_CH_ERR )
						{
							CMD_Printf(" Strength =%d , Agc = %d, Quality = %d, Snr = %f \n ", stSignalInfo.ulStrength, stSignalInfo.ulAgc, stSignalInfo.ulQuality, stSignalInfo.fSnr);
							iResult=CMD_OK;
						}
					}
					else
					{
						CMD_Printf(" CAB Unlock ! Retry Lock cmd \n");
						iResult = CMD_OK;
					}
				}
				else
				{
					CMD_Printf(" First Try CAB Lock cmd \n");
					iResult=CMD_OK;
				}
			}
		}
	}
#endif

#if defined(CONFIG_FRONTEND_TER)
		 if(szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
		{
			char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
			if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0))
			{
    		    if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
                {
                    return CMD_ERR;
                }

				ulInstanceId = iUnitId;
				CMD_Printf("ulInstanceId (%d) \n", ulInstanceId );

				if(ulInstanceId <= NUM_OF_CHANNEL)
				{
					/* TER */
					if(s_CurrentType[ulInstanceId] == DI_CH_TYPE_TER)
					{

						bLocked = CH_T_CheckLock( ulInstanceId, iUnitId);
						CMD_Printf("bLocked = (%d) \n", bLocked );

						if(bLocked == TRUE)
						{
							nRet= DI_CH_T_GetLockedInfo(ulInstanceId, &stTerLockedInfo);
							if( nRet != DI_CH_ERR )
							{
								CMD_Printf(" Frequency =%d , Modulation = %d, BandWidth = %d\n ", stTerLockedInfo.ulFrequency, stTerLockedInfo.etModulation, stTerLockedInfo.etBandWidth);
							}

							nRet = DI_CH_T_GetSignalInfo(ulInstanceId, &stSignalInfo);
							if( nRet != DI_CH_ERR )
							{
								CMD_Printf(" Strength =%d , Agc = %d, Quality = %d\n ", stSignalInfo.ulStrength, stSignalInfo.ulAgc, stSignalInfo.ulQuality);
								iResult=CMD_OK;
							}
						}
						else
						{
							CMD_Printf(" TER Unlock ! Retry Lock cmd \n");
							iResult=CMD_OK;
						}
					}
					else
					{
						CMD_Printf(" First Try TER Lock cmd \n");
						iResult=CMD_OK;
					}
				}
			}
	}
#endif

	return iResult;
}

int CMD_HwTest_Parser_ChTSEnable(void *szArgs)
{
	int iResult=CMD_ERR;
	HUINT32 ulInstanceId;

	int iUnitId, nChannelId;
	int nRet = DI_CH_OK;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			ulInstanceId = iUnitId;

#if defined(CONFIG_FRONTEND_SAT)
			nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );
			nRet = CH_S_EnableTsOutput(nChannelId);
#endif
			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! SAT TS OUTPUT Enable \n");

			}
			else
			{
				CMD_Printf("Fail! SAT TS OUTPUT Enable \n");
			}
			iResult=CMD_OK;
		}
	}
	else if (szCmd!=NULL && VK_strcmp(szCmd,"cab")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			ulInstanceId = iUnitId;

#if defined(CONFIG_FRONTEND_CAB)
			nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );
			nRet = CH_C_EnableTsOutput(nChannelId);
#endif
			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! CAB TS OUTPUT Enable \n");

			}
			else
			{
				CMD_Printf("Fail! CAB TS OUTPUT Enable \n");
			}
			iResult=CMD_OK;
		}
	}
	else
	{
		CH_UNUSED(nChannelId);
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_ChTSDisable(void *szArgs)
{
	int iResult=CMD_ERR;
	HUINT32 ulInstanceId;

	int iUnitId, nChannelId;
	int nRet = DI_CH_OK;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			ulInstanceId = iUnitId;

#if defined(CONFIG_FRONTEND_SAT)
			nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_SAT );
			nRet = CH_S_DisableTsOutput(nChannelId);
#endif
			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! SAT TS OUTPUT Disable \n");

			}
			else
			{
				CMD_Printf("Fail! SAT TS OUTPUT Disable \n");
			}
			iResult=CMD_OK;
		}
	}
	else if (szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			ulInstanceId = iUnitId;

#if defined(CONFIG_FRONTEND_TER)
			nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_TER );
			nRet = CH_T_DisableTsOutput(nChannelId);
#endif
			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! TER TS OUTPUT Disable \n");

			}
			else
			{
				CMD_Printf("Fail! TER TS OUTPUT Disable \n");
			}
			iResult=CMD_OK;
		}
	}
	if (szCmd!=NULL && VK_strcmp(szCmd,"cab")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			ulInstanceId = iUnitId;

#if defined(CONFIG_FRONTEND_CAB)
			nChannelId = FindChannelID( ulInstanceId, DI_CH_TYPE_CAB );
			nRet = CH_C_DisableTsOutput(nChannelId);
#endif
			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! CAB TS OUTPUT Disable \n");

			}
			else
			{
				CMD_Printf("Fail! CAB TS OUTPUT Disable \n");
			}
			iResult=CMD_OK;
		}
	}
	else
	{
		CH_UNUSED(nChannelId);
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_ChReInitDev(void *szArgs)
{
	CH_UNUSED(szArgs);
	MAIN_CH_ReInitChannel();
	return CMD_OK;
}

int CMD_HwTest_Parser_ChUnInitDev(void *szArgs)
{
	CH_UNUSED(szArgs);
	MAIN_CH_UnInitChannel();
	return CMD_OK;
}

int CMD_HwTest_Parser_ChDeepSleep(void *szArgs)
{
	int iSleepMode;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

    if ((szCmd != NULL  && VK_strcmp(szCmd,"0")==0) || (szCmd != NULL  && VK_strcmp(szCmd,"1")==0))
    {
        if ( READABLE_IN_DEC(szCmd, iSleepMode) != 1 )
        {
            return CMD_ERR;
        }

        CMD_Printf(" [CMD_CH] %s \n", iSleepMode? "Deep sleep":"Wake up");

        if(( iSleepMode == 0) && (ucIsDeepSleep != 0))    // Wake up
        {
            DRV_GPIO_Write(GPIO_ID_LOOPTHROUGH, GPIO_HI);
            VK_TASK_Sleep(1000);
            MAIN_CH_ReInitChannel();
            ucIsDeepSleep = 0;
        }
        else if(( iSleepMode == 1) && (ucIsDeepSleep != 1))  // Deep Sleep
        {
            MAIN_CH_UnInitChannel();
            VK_TASK_Sleep(1000);
            DRV_GPIO_Write(GPIO_ID_LOOPTHROUGH, GPIO_LOW);
            ucIsDeepSleep = 1;
        }
        else
            CMD_Printf(" That is already setting!! \n");

    }


    return CMD_OK;
}


#if defined(CONFIG_DEMOD_TSMF)
typedef struct INTRN_TSMF_TSID
{
#if 1
	HUINT8	ts_id0;
	HUINT8	ts_id1;
	HUINT8	original_network_id0;
	HUINT8	original_network_id1;
#else
	HUINT16 ts_id;
	HUINT16 original_network_id;

#endif /* 0 */

}INTRN_TSMF_TSID_t;


typedef struct INTRN_TSMF_TS_STATUS
{
#if 1	// Little Endian
	HUINT8	ts_status8:1,
			ts_status7:1,
			ts_status6:1,
			ts_status5:1,
			ts_status4:1,
			ts_status3:1,
			ts_status2:1,
			ts_status1:1;
	HUINT8	reserved:1,
			ts_status15:1,
			ts_status14:1,
			ts_status13:1,
			ts_status12:1,
			ts_status11:1,
			ts_status10:1,
			ts_status9:1;
#else	//Big Endian
	HUINT8	ts_status1:1,
			ts_status2:1,
			ts_status3:1,
			ts_status4:1,
			ts_status5:1,
			ts_status6:1,
			ts_status7:1,
			ts_status8:1;
	HUINT8	ts_status9:1,
			ts_status10:1,
			ts_status11:1,
			ts_status12:1,
			ts_status13:1,
			ts_status14:1,
			ts_status15:1,
			reserved:1;
#endif /* 1 */

}INTRN_TSMF_TS_STATUS_t;

typedef struct INTRN_TSMF_RECEIVE_STATUS
{
#if 1 // Little Endian
	HUINT8	receive_status4:2,
			receive_status3:2,
			receive_status2:2,
			receive_status1:2;
	HUINT8	receive_status9:2,
			receive_status8:2,
			receive_status7:2,
			receive_status6:2;
	HUINT8	receive_status5:2,
			receive_status15:2,
			receive_status14:2,
			receive_status13:2;
	HUINT8	receive_status12:2,
			receive_status11:2,
			receive_status10:2,
			emergency_indicator:1,
			reserved:1;

#else	//Big Endian
	HUINT8	receive_status1:2,
			receive_status2:2,
			receive_status3:2,
			receive_status4:2;
	HUINT8	receive_status5:2,
			receive_status6:2,
			receive_status7:2,
			receive_status8:2;
	HUINT8	receive_status9:2,
			receive_status10:2,
			receive_status11:2,
			receive_status12:2;
	HUINT8	receive_status13:2,
			receive_status14:2,
			receive_status15:2,
			reserved:1,
			emergency_indicator:1;
#endif /* 1 */
}INTRN_TSMF_RECEIVE_STATUS_t;

typedef struct INTRN_TSMF_RELATIVE_TS_NUMBER
{
	HUINT8	relative_ts_number2nd:4,
			relative_ts_number1st:4;
}INTRN_TSMF_RELATIVE_TS_NUMBER_t;


typedef struct INTRN_CH_TSMF_HEADER
{
	HUINT8 TSHdr0;
	HUINT8 TSHdr1;
	HUINT8 TSHdr2;
	HUINT8 TSHdr3;
#if 1	//Little Endian
	HUINT8 frame_sync0:5,
		   reserved0:3;
	HUINT8 frame_sync1;
	HUINT8 frame_type:4,
		   relative_ts_number_mode:1,
		   version_number:3;
#else	// Big Endian
	HUINT8 reserved0:3,
		   frame_sync0:5;
	HUINT8 frame_sync1;
	HUINT8 version_number:3,
		   relative_ts_number_mode:1,
		   frame_type:4;

#endif /* 1 */

#if 1
	INTRN_TSMF_TS_STATUS_t ts_status;
	INTRN_TSMF_TSID_t aTSID[15];
	INTRN_TSMF_RECEIVE_STATUS_t receive_status;

	INTRN_TSMF_RELATIVE_TS_NUMBER_t relative_ts_number[26];
#endif /* 0 */

}INTRN_CH_TSMF_HEADER_t;

static INTRN_CH_TSMF_HEADER_t sTSMF_HEADER;

void hwtest_CB_CH_TSMF_Notify(HUINT32 ulDemodId, HUINT8 *pucBuf)
{
	int i;
	INTRN_CH_TSMF_HEADER_t* pTSMF_HEADER;

	CMD_Printf("[%s] Demod ID : (%d)", __FUNCTION__, ulDemodId);

	VK_memcpy(&sTSMF_HEADER, pucBuf, sizeof(INTRN_CH_TSMF_HEADER_t));

	DD_MEM_Free(pucBuf);

	CMD_Printf("TS_HDR : %02x %02x %02x %02x\n", sTSMF_HEADER.TSHdr0,sTSMF_HEADER.TSHdr1, sTSMF_HEADER.TSHdr2, sTSMF_HEADER.TSHdr3);
	CMD_Printf("reserved : 0x%x\n", sTSMF_HEADER.reserved0);
	CMD_Printf("FRAME_SYNC : 0x%x, 0x%x\n", sTSMF_HEADER.frame_sync0, sTSMF_HEADER.frame_sync1);
	CMD_Printf("VERSION_NUM : 0x%x\n", sTSMF_HEADER.version_number);
	CMD_Printf("relative_ts_number_mode : %02x\n", sTSMF_HEADER.relative_ts_number_mode);
	CMD_Printf("frame_type : 0x%x\n", sTSMF_HEADER.frame_type);

	//for(i=0; i<15; i++)
	{
		CMD_Printf("ts_status[1] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status1);
		CMD_Printf("ts_status[2] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status2);
		CMD_Printf("ts_status[3] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status3);
		CMD_Printf("ts_status[4] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status4);
		CMD_Printf("ts_status[5] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status5);
		CMD_Printf("ts_status[6] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status6);
		CMD_Printf("ts_status[7] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status7);
		CMD_Printf("ts_status[8] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status8);
		CMD_Printf("ts_status[9] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status9);
		CMD_Printf("ts_status[10] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status10);
		CMD_Printf("ts_status[11] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status11);
		CMD_Printf("ts_status[12] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status12);
		CMD_Printf("ts_status[13] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status13);
		CMD_Printf("ts_status[14] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status14);
		CMD_Printf("ts_status[15] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status15);
		CMD_Printf("ts_status[reserved] : 0x%x\n", sTSMF_HEADER.ts_status.reserved);
	}

	for(i=0; i<15; i++)
	{
#if 1
		CMD_Printf("ts_id[%d] : 0x%x, original_network_id[i+1] : 0x%x \n", i+1, ((sTSMF_HEADER.aTSID[i].ts_id0<<8) | sTSMF_HEADER.aTSID[i].ts_id1), ((sTSMF_HEADER.aTSID[i].original_network_id0<<8) | sTSMF_HEADER.aTSID[i].original_network_id1) );
#else
		PrintError("ts_id[%d] : 0x%x, original_network_id[i+1] : 0x%x \n", i+1, sTSMF_HEADER.aTSID[i].ts_id, sTSMF_HEADER.aTSID[i].original_network_id);

#endif /* 0 */
	}


	//for(i=0; i<15; i++)
	{
		CMD_Printf("receive_status[1] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status1);
		CMD_Printf("receive_status[2] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status2);
		CMD_Printf("receive_status[3] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status3);
		CMD_Printf("receive_status[4] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status4);
		CMD_Printf("receive_status[5] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status5);
		CMD_Printf("receive_status[6] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status6);
		CMD_Printf("receive_status[7] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status7);
		CMD_Printf("receive_status[8] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status8);
		CMD_Printf("receive_status[9] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status9);
		CMD_Printf("receive_status[10] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status10);
		CMD_Printf("receive_status[11] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status11);
		CMD_Printf("receive_status[12] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status12);
		CMD_Printf("receive_status[13] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status13);
		CMD_Printf("receive_status[14] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status14);
		CMD_Printf("receive_status[15] : 0x%x\n", sTSMF_HEADER.receive_status.receive_status15);
		CMD_Printf("receive_status[emergency_indicator] : 0x%x\n", sTSMF_HEADER.receive_status.emergency_indicator);

	}

	for(i=0; i<26; i++)
	{
		CMD_Printf("relative_ts_number[slot-%d] : 0x%x\n", i*2, sTSMF_HEADER.relative_ts_number[i].relative_ts_number1st);
		CMD_Printf("relative_ts_number[slot-%d] : 0x%x\n", i*2+1, sTSMF_HEADER.relative_ts_number[i].relative_ts_number2nd);
	}


	return DI_ERR_OK;
}

int CMD_HwTest_Parser_ChTSMFTest(void *szArgs)
{
	GET_ARGS;
	BSTD_UNUSED(HWTEST_PARAM2);
	DI_ERR_CODE diErr = DI_ERR_OK;
	int nDemodId;
	DRV_CH_TSMF_STATUS_e eStatus;
	if CMD_IS("testcb")
	{

		DRV_CH_TSMF_RegisterTSMFCallback(hwtest_CB_CH_TSMF_Notify);
		iResult=CMD_OK;

	}
	else if CMD_IS("stop")
	{
		DRV_CH_TSMF_RegisterTSMFCallback(NULL);
		iResult=CMD_OK;
	}
	else if CMD_IS("get")
	{
		DRV_CH_TSMF_GetStatus(0, &eStatus);
		DRV_CH_TSMF_GetStatus(1, &eStatus);
		DRV_CH_TSMF_GetStatus(2, &eStatus);
		DRV_CH_TSMF_GetStatus(3, &eStatus);
		iResult=CMD_OK;
	}
	else if CMD_IS("req")
	{
		nDemodId =  VK_atoi((char *)szParam);
		DRV_CH_TSMF_RequestSearch(nDemodId);
		iResult=CMD_OK;
	}
	else
	{
		CMD_Printf(" ERR!Null or Bad Input \n");
		iResult=CMD_OK;
	}

	return iResult;
}

int CMD_HwTest_Parser_ChPathInfo(void *szArgs)
{
	GET_ARGS;
	BSTD_UNUSED(HWTEST_PARAM2);
	iResult=CMD_OK;

#if defined(CONFIG_FRONTEND_CAB)
	CH_C_GetPathInfo();
#endif

	return iResult;
}

#endif /* CONFIG_DEMOD_TSMF */



#if defined(CONFIG_FRONTEND_SAT)
int CMD_HwTest_Parser_ChLnb(void *szArgs)
{
	int iResult=CMD_ERR;

	int iIndex;

	unsigned long ulInstanceId;
	CH_SAT_TuneParam_t stSatTuneP;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		/* SAT */
		stSatTuneP.etAntennaType = DI_CH_ANT_TYPE_DISEQC;
		stSatTuneP.ulFrequency = 1150; //000; //unit KHz
		stSatTuneP.ulSymbolRate = 28125;
		stSatTuneP.etFecCodeRate = DI_CH_SCODERATE_3_4;
		stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS;
		stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_B;
		stSatTuneP.etModulation = DI_CH_PSK_QPSK;
		stSatTuneP.etPilot = DI_CH_PILOT_AUTO;
		stSatTuneP.etRollOff = DI_CH_ROLL_035;
		stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
		stSatTuneP.ulScdMasterUnitId = 0;
		stSatTuneP.ulScdUserBandNum = 0;
		stSatTuneP.ulScdUserBandFreq = 0;

		/*22k Tone OFF*/
		stSatTuneP.b22kTone = FALSE;
		stSatTuneP.etToneBurst = DI_CH_TONE_BURST_NONE;

		char *szLLCCmd=(char *)CMD_NextArg((char **)&szArgs);
		/* LLC ON */
		if (szLLCCmd!=NULL && VK_strcmp(szLLCCmd,"on")==0)
		{
			/* High */
			stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_HIGH;
		}

		/* LLC OFF */
		else if (szLLCCmd!=NULL && VK_strcmp(szLLCCmd,"off")==0)
		{
			/* Standard */
			stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
		}
		else
		{
			CMD_Printf("Bad LLC Input! (default: LLC off) \n");
			stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
		}


		char *szIndexId=(char *)CMD_NextArg((char **)&szArgs);

		VK_sscanf(szIndexId, "%d", &iIndex);
		if (iIndex > -1 && iIndex <= 4)
		{
			if(iIndex == 0)
			{
				g_hwtest_lnb_off = 1;
				DRV_CH_LNB_SetOutput(0, FALSE);
				DRV_CH_LNB_SetOutput(1, FALSE);
				CMD_Printf("ALL LNB Out Off \n" );
			}
			else
			{
				g_hwtest_lnb_off = 0;
				if(iIndex == 1)
				{
					stSatTuneP.etPolarization = DI_CH_POLAR_HOR;
					ulInstanceId = 0;
					DRV_CH_LNB_SetOutput(0, TRUE);

					ulRequestId++;
					DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);
					CMD_Printf("Hor Tuner 1 \n" );
				}
				else if(iIndex == 2)
				{
					stSatTuneP.etPolarization = DI_CH_POLAR_VER;
					ulInstanceId = 0;
					DRV_CH_LNB_SetOutput(0, TRUE);

					ulRequestId++;
					DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);
					CMD_Printf("Ver Tuner 1 \n" );
				}
				else if(iIndex == 3)
				{
					stSatTuneP.etPolarization = DI_CH_POLAR_HOR;
					ulInstanceId = 1;
					DRV_CH_LNB_SetOutput(1, TRUE);

					ulRequestId++;
					DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);
					CMD_Printf("Hor Tuner 2 \n" );
				}
				else if(iIndex == 4)
				{
					stSatTuneP.etPolarization = DI_CH_POLAR_VER;
					ulInstanceId = 1;
					DRV_CH_LNB_SetOutput(1, TRUE);

					ulRequestId++;
					DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);
					CMD_Printf("Ver Tuner 2 \n" );
				}
				else
					CMD_Printf("Err! Input Parameter (iIndex = %d) \n", iIndex);
			}

			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("ERR! Bad Input index [%d] \n", iIndex);
		}
	}
	else
	{
		CMD_Printf(" ERR!Null or Bad Input \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_Ch22kTone(void *szArgs)
{
	int iResult=CMD_ERR;

	int iIndex;
	unsigned long ulInstanceId;
	CH_SAT_TuneParam_t stSatTuneP;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szIndexId=(char *)CMD_NextArg((char **)&szArgs);
		if (szIndexId != NULL)
		{
			if ( READABLE_IN_DEC(szIndexId, iIndex) != 1 )
            {
                return 1;
            }
			//CMD_Printf(" Input index(%d) \n", iIndex);

			if(iIndex < 0 || iIndex >4)
			{
				CMD_Printf("ERR! Bad Input index [%d] \n", iIndex);
				return CMD_ERR;
			}

			/* SAT */
			stSatTuneP.etAntennaType = DI_CH_ANT_TYPE_DISEQC;
			stSatTuneP.ulFrequency = 1150; //000; //unit KHz
			stSatTuneP.ulSymbolRate = 28125;
			stSatTuneP.etFecCodeRate = DI_CH_SCODERATE_3_4;
			stSatTuneP.etTransSpec = DI_CH_TRANS_DVBS;
			stSatTuneP.etDiseqcInput = DI_CH_DISEQC_INPUT_B;

			stSatTuneP.etModulation = DI_CH_PSK_QPSK;
			stSatTuneP.etPilot = DI_CH_PILOT_AUTO;
			stSatTuneP.etRollOff = DI_CH_ROLL_035;

			stSatTuneP.etToneBurst = DI_CH_TONE_BURST_NONE;
			stSatTuneP.etLnbVoltage = DI_CH_LNB_VOLT_STD;
			stSatTuneP.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;
			stSatTuneP.ulScdMasterUnitId = 0;
			stSatTuneP.ulScdUserBandNum = 0;
			stSatTuneP.ulScdUserBandFreq = 0;

			DRV_CH_LNB_SetOutput(0, TRUE);
			//stSatTuneP.etPolarization = DI_CH_POLAR_HOR;

			if(iIndex == 1)
			{
				/*22k Tone ON*/
				stSatTuneP.b22kTone = TRUE;
				ulInstanceId = 0;
				ulRequestId++;
				DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);
				CMD_Printf("22k On Tuenr 1 \n" );
			}
			else if(iIndex == 2)
			{
				stSatTuneP.b22kTone = FALSE;
				ulInstanceId = 0;
				ulRequestId++;
				DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);
				CMD_Printf("22k Off Tuenr 1 \n" );
			}
			else if(iIndex == 3)
			{
				stSatTuneP.b22kTone = TRUE;
				ulInstanceId = 1;
				ulRequestId++;
				DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);
				CMD_Printf("22k On Tuenr 2 \n" );
			}
			else if(iIndex == 4)
			{
				stSatTuneP.b22kTone = FALSE;
				ulInstanceId = 1;
				ulRequestId++;
				DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stSatTuneP);
				CMD_Printf("22k Off Tuenr 2 \n" );
			}
			else
				CMD_Printf("Err! Input Parameter (iIndex = %d) \n", iIndex);

			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf(" ERR!Null or Bad Input \n");
		}

	}
	else
	{
		CMD_Printf(" ERR!Null or Bad Input \n");
	}

	return iResult;
}


int CMD_HwTest_Parser_ChDiseqc(void *szArgs)
{
	int iResult=CMD_ERR;

	HUINT32 ulInstanceId;

	int iUnitId;
	int iDiseqcInput;

	CH_SAT_TuneParam_t stChannelParam;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
        {
            return 1;
        }

		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0))
		{
			char *szInput=(char *)CMD_NextArg((char **)&szArgs);
			if ( READABLE_IN_DEC(szInput, iDiseqcInput) != 1 )
            {
                return 1;
            }

			ulInstanceId = iUnitId;
			CMD_Printf("ulInstanceId (%d), iDiseqcInput (%d)\n", ulInstanceId ,  iDiseqcInput);

			if( (iDiseqcInput == 1) || (iDiseqcInput == 2) || (iDiseqcInput == 3) || (iDiseqcInput == 4) )
			{
				/* SAT */
				stChannelParam.etAntennaType = DI_CH_ANT_TYPE_DISEQC;
				stChannelParam.ulFrequency = 1150;
				stChannelParam.ulSymbolRate = 28125;
				stChannelParam.etFecCodeRate = DI_CH_SCODERATE_AUTO;
				stChannelParam.etPolarization = DI_CH_POLAR_HOR;
				stChannelParam.etTransSpec = DI_CH_TRANS_DVBS;
				stChannelParam.b22kTone =TRUE;

				stChannelParam.etModulation = DI_CH_PSK_QPSK;
				stChannelParam.etPilot = DI_CH_PILOT_OFF;
				stChannelParam.etRollOff = DI_CH_ROLL_035;
				stChannelParam.etToneBurst = DI_CH_TONE_BURST_NONE;
				stChannelParam.etLnbVoltage = DI_CH_LNB_VOLT_STD;
				stChannelParam.etDiseqcVersion = DI_CH_DISEQC_VER_1_0;

				stChannelParam.ulScdMasterUnitId = 0;
				stChannelParam.ulScdUserBandNum = 0;
				stChannelParam.ulScdUserBandFreq = 0;

				if(iDiseqcInput == 1)
					stChannelParam.etDiseqcInput = DI_CH_DISEQC_INPUT_A;
				else if (iDiseqcInput == 2)
					stChannelParam.etDiseqcInput = DI_CH_DISEQC_INPUT_B;
				else if (iDiseqcInput == 3)
					stChannelParam.etDiseqcInput = DI_CH_DISEQC_INPUT_C;
				else if (iDiseqcInput == 4)
					stChannelParam.etDiseqcInput = DI_CH_DISEQC_INPUT_D;

				ulRequestId++;

				DI_CH_S_RequestLock( ulInstanceId, ulRequestId, &stChannelParam);
				iResult=CMD_OK;
			}
			else
			{
				CMD_Printf("ERR! Bad Input Param : iDiseqcInput(%d)\n", iDiseqcInput);
			}
		}
		else
		{
			CMD_Printf("ERR! Bad Unit Id (%d)\n", iUnitId);
		}
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_ChSetInputMode(void *szArgs)
{
	int iResult=CMD_ERR;
	CH_SAT_InputMode_e etInputMode = DI_CH_INPUT_ONE_SAME;

	int iUnitId, nChannelId;
	int nRet = DI_CH_OK;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd!=NULL && VK_strcmp(szCmd,"mode")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			etInputMode = iUnitId;

			nRet = DI_CH_S_SetInputMode(0, etInputMode);

			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! SAT Input mode is changed \n");

			}
			else
			{
				CMD_Printf("Fail! SAT Input mode is not changed \n");
			}
			iResult=CMD_OK;
		}
	}
	else
	{
		CH_UNUSED(nChannelId);
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_ChSetInput(void *szArgs)
{
	int iResult=CMD_ERR;
	int iUnitId, nInputSource = 0;
	int nRet = DI_CH_OK;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			char *szInputSource=(char *)CMD_NextArg((char **)&szArgs);
			if (szInputSource != NULL)
			{
				if ( READABLE_IN_DEC(szInputSource, nInputSource) != 1 )
				{
                	return CMD_ERR;
            	}
			}

			nRet = DI_CH_S_SetInputSource(iUnitId, nInputSource);

			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! SAT Input mode is changed \n");

			}
			else
			{
				CMD_Printf("Fail! SAT Input mode is not changed \n");
			}
			iResult=CMD_OK;
		}
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}



int CMD_HwTest_Parser_ChGetInput(void *szArgs)
{
	int iResult=CMD_ERR;
	DI_CH_TunerCapacity_t *pstTunerCap;

	int iUnitId;
	HUINT32 i;
	int nRet = DI_CH_OK;

	pstTunerCap = DD_MEM_Alloc(sizeof(DI_CH_TunerCapacity_t));

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			nRet = DI_CH_S_GetTunerCapability(iUnitId, pstTunerCap);
			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! DI_CH_S_GetTunerCapability() \n");
				CMD_Printf("ulNumOfInput =(%d)\n", pstTunerCap->ulNumOfInput);
				for( i=0; i<pstTunerCap->ulNumOfInput; i++)
				{
					CMD_Printf("anInputId[%d] =(%d)\n", i, pstTunerCap->anInputId[i]);
				}
				CMD_Printf("bLoopThroughOut =(%d)\n", pstTunerCap->bLoopThroughOut);

			}
			else
			{
				CMD_Printf("Fail! DI_CH_S_GetTunerCapability() \n");
			}
			iResult=CMD_OK;
		}
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_ChSendDiseqc(void *szArgs)
{
	int iResult=CMD_ERR;
	int iUnitId, nSendCmd = 0;
	int nRet = DI_CH_OK;

	HUINT8 aucData[3]={0xe2,0x10,0x50};

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			char *szInputSource=(char *)CMD_NextArg((char **)&szArgs);
			if (szInputSource != NULL)
			{
				if ( READABLE_IN_DEC(szInputSource, nSendCmd) != 1 )
				{
                	return CMD_ERR;
            	}
			}

			aucData[2]= nSendCmd;

			nRet = DI_CH_S_SendDiseqcMsg(iUnitId, aucData, sizeof(aucData));

			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! DI_CH_S_SendDiseqcMsg \n");

			}
			else
			{
				CMD_Printf("Fail! DI_CH_S_SendDiseqcMsg \n");
			}
			iResult=CMD_OK;
		}
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_ChReceiveDiseqc(void *szArgs)
{
	int iResult=CMD_ERR;
	int iUnitId = 0;
	int nRet = DI_CH_OK;
	int i=0;

	static HUINT8 aucData[4]={0x0,0x0,0x0,0x0};
	HUINT8 ucReplyLength=0;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		if ((szUnitId != NULL  && VK_strcmp(szUnitId,"0")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"1")==0) ||
			(szUnitId != NULL  && VK_strcmp(szUnitId,"2")==0) || (szUnitId != NULL  && VK_strcmp(szUnitId,"3")==0))
		{
			if ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 )
            {
                return 1;
            }

			nRet = DI_CH_S_ReceiveDiseqcMsg(iUnitId,aucData,sizeof(aucData), &ucReplyLength);

			if(nRet == DI_CH_OK)
			{
				CMD_Printf("OK! DI_CH_S_ReceiveDiseqcMsg \n");
				for(i=0;i<ucReplyLength;i++)
				{
					CMD_Printf("aucData[%d]=[0x%x] \n",i,aucData[i]);
				}

			}
			else
			{
				CMD_Printf("Fail! DI_CH_S_ReceiveDiseqcMsg \n");
			}
			iResult=CMD_OK;
		}
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

#endif


int CMD_HwTest_Parser_ChWrite(void *szArgs)
{
	int iResult=CMD_ERR;

#if defined(CONFIG_FRONTEND_CAB)||defined(CONFIG_FRONTEND_TER)
	HINT32 ucIndex, ucData;
	HINT32	iUnitId;
#endif
	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

#if defined(CONFIG_FRONTEND_CAB)||defined(CONFIG_FRONTEND_TER)
	char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
	char *szIndex=(char *)CMD_NextArg((char **)&szArgs);
	char *szData =(char *)CMD_NextArg((char **)&szArgs);
#endif

	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		CMD_Printf("SAT(Demod) Register Data Read \n");
		iResult=CMD_OK;
	}

#if defined(CONFIG_FRONTEND_CAB)
	else if (szCmd!=NULL && VK_strcmp(szCmd,"cab")==0)
	{
        if ( ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 ) ||
             ( READABLE_IN_DEC(szIndex, ucIndex) != 1 ) ||
             ( READABLE_IN_DEC(szData, ucData) != 1 ) )
        {
            return 1;
        }
		CMD_Printf("CAB (Demod) Register Write Register \n");
		if (szIndex!=NULL && szData!=NULL)
		{
			CMD_Printf("Index [0x%02x] Write Data = 0x%02x \n",ucIndex, ucData);

			if(s_CurrentType[iUnitId] == DI_CH_TYPE_CAB)
			{
#if defined (CONFIG_PLATFORM_DP10023)
				Drv_Ch_I2C_DemodWrite(iUnitId, TDA10023_DEMOD_ADDRESS, ucIndex, &ucData, 1);
#endif
			}
			else
			{
				CMD_Printf("ERR! It is not CAB mode. Try CAB Lock cmd \n");
			}
		}

		iResult=CMD_OK;
	}
#endif

#if defined(CONFIG_FRONTEND_TER)
	else if (szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
	{
        if ( ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 ) ||
             ( READABLE_IN_DEC(szIndex, ucIndex) != 1 ) ||
             ( READABLE_IN_DEC(szData, ucData) != 1 ) )
        {
            return 1;
        }

		CMD_Printf("TER(Demod) Register Write Register \n");
		if (szIndex!=NULL && szData!=NULL)
		{
			CMD_Printf("Index [0x%02x]  Write Data = 0x%02x \n", ucIndex, ucData);

			if(s_CurrentType[iUnitId] == DI_CH_TYPE_TER)
			{
#if defined (CONFIG_PLATFORM_DT90517)
				Drv_Ch_I2C_DemodWrite(iUnitId, DEMOD_TC90517_ADDRESS, ucIndex, &ucData, 1);
#endif
			}
			else
			{
				CMD_Printf("ERR! It is not TER mode. Try TER Lock cmd \n");
			}
		}

		iResult=CMD_OK;
	}
#endif
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_ChRead(void *szArgs)
{
	int iResult=CMD_ERR;

#if defined(CONFIG_FRONTEND_CAB)||defined(CONFIG_FRONTEND_TER)
	HINT32	iUnitId, ucIndex;
	HUINT8	ucData = 0;
#endif

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
		CMD_Printf("SAT(Demod) Register Data Read \n");
		iResult=CMD_OK;
	}

#if defined(CONFIG_FRONTEND_CAB)
	else if (szCmd!=NULL && VK_strcmp(szCmd,"cab")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		char *szIndex=(char *)CMD_NextArg((char **)&szArgs);

		if (szUnitId !=NULL && szIndex !=NULL )
		{

            if ( ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 ) ||
                 ( READABLE_IN_DEC(szIndex, ucIndex) != 1 ) )
            {
                return 1;
            }

			if(s_CurrentType[iUnitId] == DI_CH_TYPE_CAB)
			{
#if defined (CONFIG_PLATFORM_DP10023)
				Drv_Ch_I2C_DemodRead(iUnitId, TDA10023_DEMOD_ADDRESS, ucIndex, &ucData, 1);
#endif
				CMD_Printf("Index [0x%02x]	Read Data = 0x%02x \n", ucIndex, ucData);
				iResult=CMD_OK;
			}
			else
			{
				CMD_Printf(" First Try CAB Lock cmd \n");
				iResult=CMD_OK;
			}
		}
		else
		{
			CMD_Printf("CAB_Dump Register  \n");
			if(s_CurrentType[iUnitId] == DI_CH_TYPE_CAB)
			{
#if defined (CONFIG_PLATFORM_DP10023)
		DP10023_DumpRegister(0);
#endif
#if defined (CONFIG_PLATFORM_DP10024)
		DP10024_DumpRegister(0);
#endif
			}
			else
			{
				CMD_Printf(" First Try CAB Lock cmd \n");
			}
			iResult=CMD_OK;
		}
	}
#endif
#if defined(CONFIG_FRONTEND_TER)
	else if (szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
	{
		char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
		char *szIndex=(char *)CMD_NextArg((char **)&szArgs);

		CMD_Printf("TER(Demod) Register Write Register \n");
		if (szUnitId !=NULL && szIndex !=NULL )
		{
            if ( ( READABLE_IN_DEC(szUnitId, iUnitId) != 1 ) ||
                 ( READABLE_IN_DEC(szIndex, ucIndex) != 1 ) )
            {
                return 1;
            }

			if(s_CurrentType[iUnitId] == DI_CH_TYPE_TER)
			{
#if defined (CONFIG_PLATFORM_DT90517)
				Drv_Ch_I2C_DemodRead(iUnitId, DEMOD_TC90517_ADDRESS, ucIndex, &ucData, 1);
#endif
				CMD_Printf("Index [0x%02x]  Read Data = 0x%02x \n", ucIndex, ucData);
				iResult=CMD_OK;
			}
			else
			{
				CMD_Printf(" First Try TER Lock cmd \n");
				iResult=CMD_OK;
			}
		}
		else
		{
			CMD_Printf("TER_Dump Register  \n");
			if(s_CurrentType[iUnitId] == DI_CH_TYPE_TER)
			{
#if defined (CONFIG_PLATFORM_DT90507)
		DT90507_DumpRegister(0);
#elif defined (CONFIG_PLATFORM_DT90517)
				DT90517_DumpRegister(0);
#endif
			}
			else
			{
				CMD_Printf(" First Try TER Lock cmd \n");
			}

			iResult=CMD_OK;
		}

	}
#endif
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}


int CMD_HwTest_Parser_ChTop(void *szArgs)
{
		int iResult=CMD_ERR;


		char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

#if defined(CONFIG_FRONTEND_CAB)||defined(CONFIG_FRONTEND_TER)
		HINT32 RfMinC,RfMaxC, IfMinC, IfMaxC;
		HUINT32 ucTopEnable=0;
#endif
#if defined(CONFIG_FRONTEND_TER) && defined (CONFIG_PLATFORM_DT90517)
		HINT32 RfMinT,RfMaxT, IfMinT, IfMaxT;
#endif

		if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
		{
			CMD_Printf("CMD_HwTest_Parser_ChTop[SAT]  \n");
			iResult=CMD_OK;
		}

#if defined(CONFIG_FRONTEND_CAB)
		else if (szCmd!=NULL && VK_strcmp(szCmd,"cab")==0)
		{
			char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
			char *szRfMin=(char *)CMD_NextArg((char **)&szArgs);
			char *szRfMax =(char *)CMD_NextArg((char **)&szArgs);
			char *szIfMin=(char *)CMD_NextArg((char **)&szArgs);
			char *szIfMax =(char *)CMD_NextArg((char **)&szArgs);
			char *szTopEnable =(char *)CMD_NextArg((char **)&szArgs);

			if (szUnitId!=NULL && szRfMin!=NULL && szRfMax!=NULL && szIfMin!=NULL && szIfMin!=NULL && szTopEnable!=NULL  )
			{
                if ( ( READABLE_IN_DEC(szRfMin, RfMinC) != 1 ) ||
                     ( READABLE_IN_DEC(szRfMax, RfMaxC) != 1 ) ||
                     ( READABLE_IN_DEC(szIfMin, IfMinC) != 1 ) ||
                     ( READABLE_IN_DEC(szIfMax, IfMaxC) != 1 ) ||
                     ( READABLE_IN_DEC(szTopEnable, ucTopEnable) != 1 ) )
                {
                        return 1;
                }

#if defined (CONFIG_PLATFORM_DP10023)

				gucRfMinC = (HUINT8)RfMinC;
				gucRfMaxC = (HUINT8)RfMaxC;
				gucIfMinC = (HUINT8)IfMinC;
				gucIfMaxC = (HUINT8)IfMaxC;

				 if(szTopEnable != NULL  && VK_strcmp(szTopEnable,"0")==0)
				 {
					gucCABTopOn = 0;
				 }
				 else
				 {
					 gucCABTopOn = 1;

				 }

				CMD_Printf("CMD_HwTest_Parser_ChTop[CAB] Changed [%x], [%x], [%x], [%x].\n",gucRfMinC, gucRfMaxC, gucIfMinC, gucIfMaxC);
#endif
				iResult=CMD_OK;
			}
			else
			{
				iResult=CMD_ERR;
				CMD_Printf("ERR! Bad Param Iput. \n");

			}

		}
#endif

#if defined(CONFIG_FRONTEND_TER)
		else if (szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
		{
			char *szUnitId=(char *)CMD_NextArg((char **)&szArgs);
			char *szRfMin=(char *)CMD_NextArg((char **)&szArgs);
			char *szRfMax =(char *)CMD_NextArg((char **)&szArgs);
			char *szIfMin=(char *)CMD_NextArg((char **)&szArgs);
			char *szIfMax =(char *)CMD_NextArg((char **)&szArgs);
			char *szTopEnable =(char *)CMD_NextArg((char **)&szArgs);

			if (szUnitId!=NULL && szRfMin!=NULL && szRfMax!=NULL && szIfMin!=NULL && szIfMin!=NULL && szTopEnable!=NULL  )
			{
                if ( ( READABLE_IN_DEC(szRfMin, RfMinC) != 1 ) ||
                     ( READABLE_IN_DEC(szRfMax, RfMaxC) != 1 ) ||
                     ( READABLE_IN_DEC(szIfMin, IfMinC) != 1 ) ||
                     ( READABLE_IN_DEC(szIfMax, IfMaxC) != 1 ) ||
                     ( READABLE_IN_DEC(szTopEnable, ucTopEnable) != 1 ) )
                {
                    return 1;
                }

#if defined (CONFIG_PLATFORM_DT90517)

				gucRfMinT= (HUINT8)RfMinT;
				gucRfMaxT = (HUINT8)RfMaxT;
				gucIfMinT = (HUINT8)IfMinT;
				gucIfMaxT = (HUINT8)IfMaxT;

				 if(szTopEnable != NULL  && VK_strcmp(szTopEnable,"0")==0)
				 {
					gucTERTopOn = 0;
				 }
				 else
				 {
					 gucTERTopOn = 1;

				 }

				CMD_Printf("CMD_HwTest_Parser_ChTop[TER] Changed [%x], [%x], [%x], [%x].\n",gucRfMinT, gucRfMaxT, gucIfMinT, gucIfMaxT);

#endif
				iResult=CMD_OK;
			}
			else
			{
				iResult=CMD_ERR;
				CMD_Printf("ERR! Bad Param Iput. \n");

			}
		}
#endif
		else
		{
			CMD_Printf("ERR! Bad Comman Iput. \n");
		}

		return iResult;
}


int CMD_HwTest_Parser_ChAntPower(void *szArgs)
{
	int iResult=CMD_ERR;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
	{
		char *szIndexId=(char *)CMD_NextArg((char **)&szArgs);
		if(szIndexId != NULL  && VK_strcmp(szIndexId,"1")==0)
		{
			CMD_Printf("OFDM Active Antena On \n");
#if defined(CONFIG_FRONTEND_TER)
			DI_CH_T_SetAntennaPower(0, DI_CH_ANTENNA_ON);
#endif
			iResult=CMD_OK;
		}
		else if(szIndexId != NULL  && VK_strcmp(szIndexId,"0")==0)
		{
			CMD_Printf("OFDM Active Antena Off\n");
#if defined(CONFIG_FRONTEND_TER)
			DI_CH_T_SetAntennaPower(0, DI_CH_ANTENNA_OFF);
#endif
			iResult=CMD_OK;
		}
		else
			CMD_Printf("ERR! Bad Comman Iput. \n");

	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

int CMD_HwTest_Parser_ChAntPowerEx(void *szArgs)
{
	int iResult=CMD_ERR;
	int iTunerId=0xffff;
	int iOnOff=0xffff;

	CMD_Printf("%s\n", (char *)szArgs);
	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	//VK_printf("%08X\n", szCmd);
	CMD_Printf("%s\n", szCmd);
	CMD_Printf("%s\n", (char *)szArgs);
	CHECK;
	if (szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
	{
CHECK;
		szCmd = (char *)CMD_NextArg((char **)&szArgs);
		if(szCmd == NULL)
		{
			CMD_Printf("ERR! Bad Comman Iput. \n");
			return iResult;
		}

		CHECK;
		iResult = CMD_ReadAsDec(szCmd, &iTunerId);
		if(iResult != 1)
		{
			CMD_Printf("ERR! CMD_ReadAsDec error \n");
			return iResult;
		}

		CHECK;
		szCmd = (char *)CMD_NextArg((char **)&szArgs);
		if(szCmd == NULL)
		{
			CMD_Printf("ERR! Bad Comman Iput. \n");
			return iResult;
		}

		CHECK;
		iResult = CMD_ReadAsDec(szCmd, &iOnOff);
		if(iResult != 1)
		{
			CMD_Printf("ERR! CMD_ReadAsDec error \n");
			return iResult;
		}

#if defined(CONFIG_FRONTEND_TER)
		DI_CH_T_SetAntennaPower(iTunerId, DI_CH_ANTENNA_OFF+iOnOff);
#endif
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}


/*******************************************************************************
 *	FUNCTION NAME	: CMD_HwTest_Parser_ChSetLogLevel
 *	DESCRIPTION   	: DI Channel Debug Log level 설정
 * 	INPUT         	: level
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 *******************************************************************************/
int CMD_HwTest_Parser_ChSetLogLevel(void *szArgs)
{
	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd != NULL)
	{
		if ((VK_strcmp(szCmd,"enable")!=0) && (VK_strcmp(szCmd,"disable")!=0) && (VK_strcmp(szCmd,"show")!=0))
		{
			return CMD_ERR;
		}

		if (VK_strcmp(szCmd,"show")==0)
		{
			CMD_Printf("[%s] Currently channel log mask = 0x%x \n", __FUNCTION__, show_di_ch);
			CMD_Printf("[%s] Mask = [opt3|opt2|opt1|tsinfo|args|status|siginfo|tune] \n", __FUNCTION__);

			return CMD_OK;
		}
		else
		{
			char *szInfoType=(char *)CMD_NextArg((char **)&szArgs);

			if (szInfoType != NULL)
			{
				HINT32 nMask = CH_LOG_DEFAULT;

				if (VK_strcmp(szInfoType,"tune")==0)
				{
					nMask = CH_LOG_TUNE;
				}
				else if (VK_strcmp(szInfoType,"siginfo")==0)
				{
					nMask = CH_LOG_SIGINFO;
				}
				else if (VK_strcmp(szInfoType,"status")==0)
				{
					nMask = CH_LOG_STATUS;
				}
				else if (VK_strcmp(szInfoType,"args")==0)
				{
					nMask = CH_LOG_ARGS;
				}
				else if (VK_strcmp(szInfoType,"tsinfo")==0)
				{
					nMask = CH_LOG_TSINFO;
				}
				else if (VK_strcmp(szInfoType,"opt1")==0)
				{
					nMask = CH_LOG_OPT1;
				}
				else if (VK_strcmp(szInfoType,"opt2")==0)
				{
					nMask = CH_LOG_OPT2;
				}
				else if (VK_strcmp(szInfoType,"opt3")==0)
				{
					nMask = CH_LOG_OPT3;
				}
				else if (VK_strcmp(szInfoType,"all")==0)
				{
					nMask = CH_LOG_ALL;
				}
				else
				{
					return CMD_ERR;
				}

				if (VK_strcmp(szCmd,"enable")==0)
				{
					show_di_ch |= nMask;
					CMD_Printf("[%s] Enabled channel log mask = 0x%x \n", __FUNCTION__, nMask);
				}
				else
				{
					show_di_ch &= (~nMask);
					CMD_Printf("[%s] Disabled channel log mask = 0x%x \n", __FUNCTION__, nMask);
				}
			}
			else
			{
				return CMD_ERR;
			}
		}
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");

		return CMD_ERR;
	}

	return CMD_OK;
}

#if defined(CONFIG_DEMOD_TSMF)
/*******************************************************************************
 *	FUNCTION NAME	: CMD_HwTest_Parser_ChTSMFSetLogLevel
 *	DESCRIPTION   	: DI Channel TSMF Debug Log level 설정
 * 	INPUT         	: level
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 *******************************************************************************/
int CMD_HwTest_Parser_ChTSMFSetLogLevel(void *szArgs)
{
	int iResult=CMD_ERR;
	int nLevel = 0;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if (szCmd != NULL)
	{
        if ( ( READABLE_IN_DEC(szCmd, nLevel) != 1 ) )
        {
                return 1;
        }

		CMD_Printf("[CMD_HwTest_Parser_ChTSMFSetLogLevel] nLevel = %d  \n", nLevel);
		show_di_ch_tsmf = nLevel;
		iResult=CMD_OK;
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}
#endif

/*******************************************************************************
 *	FUNCTION NAME	: CMD_HwTest_Parser_ChSetLogLevel
 *	DESCRIPTION   	: DI Channel Debug Log level 설정
 * 	INPUT         	: level
 *	OUTPUT        	: NONE
 *	RETURN        	: NONE
 *	REMARK        	: NONE
 *******************************************************************************/
int CMD_HwTest_Parser_ChSetLockingChType(void *szArgs)
{
	int iResult=CMD_ERR;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);
	if (szCmd!=NULL && VK_strcmp(szCmd,"ter")==0)
	{
#if defined(CONFIG_FRONTEND_TER)
		CMD_Printf("[CMD_HwTest_Parser_ChSetLockingChType] DI_CH_TYPE_TER  \n");
		s_CurrentType[0] = DI_CH_TYPE_TER;
#endif
		iResult=CMD_OK;
	}
	else if (szCmd!=NULL && VK_strcmp(szCmd,"cab")==0)
	{
#if defined(CONFIG_FRONTEND_CAB)
		CMD_Printf("[CMD_HwTest_Parser_ChSetLockingChType] DI_CH_TYPE_CAB  \n");
		s_CurrentType[0] = DI_CH_TYPE_CAB;
#endif
		iResult=CMD_OK;
	}
	else if (szCmd!=NULL && VK_strcmp(szCmd,"sat")==0)
	{
#if defined(CONFIG_FRONTEND_SAT)
		CMD_Printf("[CMD_HwTest_Parser_ChSetLockingChType] DI_CH_TYPE_SAT  \n");
		s_CurrentType[0] = DI_CH_TYPE_SAT;
#endif
		iResult=CMD_OK;
	}
	else
	{
		CMD_Printf("ERR! Bad Comman Iput. \n");
	}

	return iResult;
}

#if defined(CONFIG_SUPPORT_CPU_TEMP)
static void Cmd_ChTempTimer(unsigned long tid, void *arg)
{
	DI_CH_DeviceStatus stDeviceStatus;
	static int cpuMax = 0;

	BSTD_UNUSED(tid);
	BSTD_UNUSED(arg);

	memset(&stDeviceStatus, 0, sizeof(DI_CH_DeviceStatus));
#if defined(CONFIG_FRONTEND_CAB)	
	DI_CH_C_GetDeviceStatus(0, &stDeviceStatus);
#endif
	if(stDeviceStatus.temperature > cpuMax)
	{
		cpuMax = stDeviceStatus.temperature;
	}
	CMD_Printf("Demod Temp(%d) Max(%d)\n",stDeviceStatus.temperature/1000,cpuMax/1000);

}
#endif

int CMD_HwTest_Parser_ChTemp(void *szArgs)
{
	DI_CH_DeviceStatus stDeviceStatus;

	GET_ARGS;
	
	if CMD_IS("temp")
	{
#if defined(CONFIG_SUPPORT_CPU_TEMP)
		if (HWTEST_PARAM)
		{
			static HUINT32 snCmdTmpTimer = 0;
			if PARAM1_IS("on")
			{
				CMD_Printf("Temp aging is on\n");
				if(snCmdTmpTimer ==0)
				{
					VK_TIMER_EventEvery((unsigned long)1000, Cmd_ChTempTimer,NULL,0,(unsigned long *)&snCmdTmpTimer);
				}

			}
			else
			{
				if(snCmdTmpTimer != 0)
				{
					CMD_Printf("Temp aging is off\n");
					VK_TIMER_Cancel(snCmdTmpTimer);
					snCmdTmpTimer = 0;
					
				}
			}
				iResult=CMD_OK;
		}
		else
		{
			memset(&stDeviceStatus, 0, sizeof(DI_CH_DeviceStatus));
#if defined(CONFIG_FRONTEND_CAB)	
			DI_CH_C_GetDeviceStatus(0, &stDeviceStatus);
#endif
			CMD_Printf("[%s] Demod Temp = %d \n", __FUNCTION__, stDeviceStatus.temperature/1000);
			iResult=CMD_OK;
		}
#endif		
	}
    return 0;
}




