/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:		drv_channel_cxd2820.c
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
#include "htype.h"
#include "drv_gpio.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"

#include "drv_channel_t.h"
#include "drv_channel_cxd2820.h"
#include "drv_channel.h"
#include "drv_i2c.h"

#include "sony_dvb_cxd2820.h"
#include "sony_dvb_demod_monitor.h"
#include "sony_dvb_demod_monitorT.h"
#include "sony_dvb_demod_monitorT2.h"
#include "sony_dvb_demod_monitorC.h"
#include "sony_dvb_tuner.h"
#include "sony_dvb_i2c.h"
#include "drvi2c_feusb.h"  /* USB->I2C driver implementation. */

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define MAX_NUM_PLPS			(20)
#define GPIO_ANT_PWR_CHK		(0)
#define GPIO_ANT_PWR_EN			(1)
#define GPIO_READ				(0)
#define GPIO_WRITE				(1)
#define MAX_NUM_TER_UNITS		(2)
#define MAX_PLP_ID				(0xff)
#define FT2117_DVBT_REF_FREQ_HZ     (166667)      /* 166.667kHz */
#define FT2117_DVBT_IF_FREQ_MHZ		(36.0)
#define MASTER_CHANNEL			(0)

#define LOW_IF_8M_BW		(4.0)
#define LOW_IF_7M_BW		(3.5)

#define DBUV_TO_DBM			(-107)
#define CALIBRATION_TDA18273	(3.0)

#if defined(CONFIG_COMMON_SHORTCIRCUIT)  /* HDR-FOX T2 */
#define GPIO_ANT_EN				(2)
#endif
//#define MONITOR_CXD2820_STATUS
//#define MONITOR_CXD2820_ERROR_STATE
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
static sony_dvb_cxd2820_t 	s_stCxd2820[CONFIG_NUM_OF_TER_CHANNEL];
static sony_dvb_demod_t 	demod[CONFIG_NUM_OF_TER_CHANNEL];
static sony_dvb_tuner_t 	tuner[CONFIG_NUM_OF_TER_CHANNEL];
static sony_dvb_i2c_t 		demodI2C[CONFIG_NUM_OF_TER_CHANNEL];
static sony_dvb_i2c_t 		tunerI2C[CONFIG_NUM_OF_TER_CHANNEL];

static HUINT8 s_aucPlpID[CONFIG_NUM_OF_TER_CHANNEL][MAX_NUM_PLPS];
static HUINT8 s_ucNumberPlp[CONFIG_NUM_OF_TER_CHANNEL] = {0};

static CH_TER_AntStatus_t s_stAntState[MAX_NUM_TER_UNITS];

// End static variable

#define __T_T2_STRING__

char *pszCH_TER_BandWidth[]=
{
	"DI_CH_BW_8MHZ",
	"DI_CH_BW_7MHZ",
	"DI_CH_BW_6MHZ",
	"DI_CH_BW_5MHZ",
	"DI_CH_BW_10MHZ",
	"DI_CH_BW_1P7MHZ"
} ;

char *pszCH_TER_Moudulation[]=
{
	"DI_CH_TMOD_AUTO",
	"DI_CH_TMOD_QPSK",
	"DI_CH_TMOD_16QAM",
	"DI_CH_TMOD_64QAM",
	"DI_CH_TMOD_256QAM"
} ;

char *pszCH_TER_Hierarchy[]=
{
	"DI_CH_HIERARCHY_AUTO",
	"DI_CH_HIERARCHY_NONE",
	"DI_CH_HIERARCHY_1",
	"DI_CH_HIERARCHY_2",
	"DI_CH_HIERARCHY_4"
} ;

char *pszCH_TER_code_rate[]=
{
	"DI_CH_TCODERATE_AUTO",
	"DI_CH_TCODERATE_1_2",
	"DI_CH_TCODERATE_2_3",
	"DI_CH_TCODERATE_3_4",
	"DI_CH_TCODERATE_5_6",
	"DI_CH_TCODERATE_7_8",
	"DI_CH_T2CODERATE_3_5",
	"DI_CH_T2CODERATE_4_5"
} ;

char *pszCH_TER_TransMode[]=
{
	"DI_CH_TRANSMODE_AUTO",
	"DI_CH_TRANSMODE_2K",
	"DI_CH_TRANSMODE_8K",
	"DI_CH_T2TRANSMODE_4K",
	"DI_CH_T2TRANSMODE_1K",
	"DI_CH_T2TRANSMODE_16K",
	"DI_CH_T2TRANSMODE_32K"
} ;

char *pszCH_TER_GuardInterval[]=
{
	"DI_CH_GUARDINTERVAL_AUTO",
	"DI_CH_GUARDINTERVAL_1_32",
	"DI_CH_GUARDINTERVAL_1_16",
	"DI_CH_GUARDINTERVAL_1_8",
	"DI_CH_GUARDINTERVAL_1_4",
	"DI_CH_T2GUARDINTERVAL_1_128",
	"DI_CH_T2GUARDINTERVAL_19_128",
	"DI_CH_T2GUARDINTERVAL_19_256"
} ;

char *pszCH_TER_PreambleFormat[]=
{
	"DI_CH_T2SISO",
	"DI_CH_T2MISO"
} ;

char *pszCH_TER_PreambleMixed[]=
{
	"DI_CH_T2NOT_MIXED",
	"DI_CH_T2MIXED"
} ;

char *pszCH_TER_ExtendedCarrier[]=
{
	"DI_CH_T2NOT_USED",
	"DI_CH_T2USED"
} ;

char *pszCH_TER_PaprMode[]=
{
	"DI_CH_NONE_PAPR",
	"DI_CH_ACE_PAPR",
	"DI_CH_TR_PAPR",
	"DI_CH_ACETR_PAPR"
} ;

char *pszCH_TER_PilotPattern[]=
{
	"DI_CH_T2_PPAUTO",
	"DI_CH_T2_PP1",
	"DI_CH_T2_PP2",
	"DI_CH_T2_PP3",
	"DI_CH_T2_PP4",
	"DI_CH_T2_PP5",
	"DI_CH_T2_PP6",
	"DI_CH_T2_PP7",
	"DI_CH_T2_PP8"
} ;



/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:
#define __DRVCH_T_PRIVATE_FUNCITONS__

static const char* FormatResult (sony_dvb_result_t result)
{
    char* pErrorName = "UNKNOWN" ;
    switch (result) {
    case SONY_DVB_OK:
        pErrorName = "OK";
        break;
    case SONY_DVB_ERROR_TIMEOUT:
        pErrorName = "ERROR_TIMEOUT";
        break;
    case SONY_DVB_ERROR_UNLOCK:
        pErrorName = "ERROR_UNLOCK";
        break;
    case SONY_DVB_ERROR_CANCEL:
        pErrorName = "ERROR_CANCEL";
        break;
    case SONY_DVB_ERROR_ARG:
        pErrorName = "ERROR_ARG";
        break;
    case SONY_DVB_ERROR_I2C:
        pErrorName = "ERROR_I2C";
        break;
    case SONY_DVB_ERROR_SW_STATE:
        pErrorName = "ERROR_SW_STATE";
        break;
    case SONY_DVB_ERROR_HW_STATE:
        pErrorName = "ERROR_HW_STATE";
        break;
    case SONY_DVB_ERROR_RANGE:
        pErrorName = "ERROR_RANGE";
        break;
    case SONY_DVB_ERROR_NOSUPPORT:
        pErrorName = "ERROR_NOSUPPORT";
        break;
    case SONY_DVB_ERROR_OTHER:
        pErrorName = "ERROR_OTHER";
        break;
    default:
        pErrorName = "ERROR_UNKNOWN";
        break;
    }
    return pErrorName ;
}

static sony_dvb_result_t dummy_tuner_Initialize (sony_dvb_tuner_t * pTuner)
{
	int nRet = 0;
	int nUnitId = 0;
	HUINT8 ucDeviceAddr = 0;

	if( pTuner == NULL )
	{
		return DI_CH_PARAM_ERR;
	}

	ucDeviceAddr = pTuner->pI2c->gwAddress;

	if( ucDeviceAddr == SONY_DVB_CXD2820_DVBT_ADDRESS )
	{
		nUnitId = 0;
	}
	else
	{
		nUnitId = 1;
	}

	nRet = TDA18273_Initialize( nUnitId );
	if( nRet != 0 )
	{
		CH_DI_Print(0, ("\n Ooooooooooooooooops!!! TDA18273_Initialize fail!! Ooooooooooooooooops!!! \n"));
	}
    return SONY_DVB_OK ;
}

static sony_dvb_result_t dummy_tuner_Finalize (sony_dvb_tuner_t * pTuner)
{
	CH_UNUSED(pTuner);
    return SONY_DVB_OK ;
}
static sony_dvb_result_t dummy_tuner_Tune (sony_dvb_tuner_t * pTuner,
                                           uint32_t frequency,
                                           sony_dvb_system_t etSonySystem,
                                           uint8_t bandWidth)
{
	int	tuner_error;
	int nUnitId = 0;
	HUINT8 ucDeviceAddr = 0;

	if( pTuner == NULL )
	{
		return DI_CH_PARAM_ERR;
	}

	ucDeviceAddr = pTuner->pI2c->gwAddress;

	if( ucDeviceAddr == SONY_DVB_CXD2820_DVBT_ADDRESS )
	{
		nUnitId = 0;
	}
	else
	{
		nUnitId = 1;
	}

	CH_PrintFunc("Demod I2C[0x%x]", pTuner->pI2c->gwAddress);
	CH_PrintFunc("frequency(%d), system(%d), bandWidth(%d)", frequency, etSonySystem, bandWidth);

#if defined(CONFIG_PLATFORM_TN2117)
	tuner_error = FT2117_SetTune(pTuner->pI2c->gwAddress, frequency, etSonySystem, bandWidth);
#elif defined(CONFIG_PLATFORM_TN18273)
	TDA18273_SetTune(nUnitId,(frequency*KHZ_TO_HZ),(int)bandWidth);
#else
	FT3114_SetTune(frequency, etSonySystem, bandWidth);
#endif

    return SONY_DVB_OK ;
}

sony_dvb_result_t dummy_tuner_Sleep (sony_dvb_tuner_t * pTuner)
{
	CH_UNUSED(pTuner);
	VK_TASK_Sleep(20);
    return SONY_DVB_OK ;
}


static void ResetChannelHw( int nUnitId )
{
#if 0
	if(0)//if( nUnitId	== 1 )
	{
		DRV_GPIO_Write(GPIO_ID_nCH1_RST, GPIO_LOW);
		VK_TASK_Sleep(20);//min 5ms
		DRV_GPIO_Write(GPIO_ID_nCH1_RST, GPIO_HI);
		VK_TASK_Sleep(20);
	}
	else
	{
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_LOW);
		VK_TASK_Sleep(20);//min 5ms
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_HI);
		VK_TASK_Sleep(20);
	}
#else
	if( nUnitId	== 0 )
	{
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_LOW);
		VK_TASK_Sleep(20);//min 5ms
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_HI);
		VK_TASK_Sleep(20);
	}
#endif
	return;
}

static int InitCxd2820 ( int nUnitId )
{

    sony_dvb_result_t result = SONY_DVB_OK ;
	HUINT8	ucI2cTAddr = SONY_DVB_CXD2820_DVBT_ADDRESS;
	HUINT8	ucI2cCAddr = SONY_DVB_CXD2820_DVBC_ADDRESS;

	CH_PrintFunc(">>>>>>>>>>>>> nUnitId[%d]", nUnitId);

	if( nUnitId	!= MASTER_CHANNEL )
	{
		ucI2cTAddr = SONY_DVB_CXD2820_DVBT_SLAVE_ADDRESS;
		ucI2cCAddr = SONY_DVB_CXD2820_DVBC_SLAVE_ADDRESS;
	}
	else
	{
		//ucI2cTAddr = 0xD8;
		//ucI2cCAddr = 0xDA;
	}

    /* Setup I2C interfaces. */
    tunerI2C[nUnitId].gwAddress = ucI2cTAddr ;
    tunerI2C[nUnitId].gwSub = 0x09;  /* Connected via demod I2C gateway function. */
    tunerI2C[nUnitId].Read = drvi2c_feusb_ReadGw ;
    tunerI2C[nUnitId].Write = drvi2c_feusb_WriteGw ;
    tunerI2C[nUnitId].ReadRegister = dvb_i2c_CommonReadRegister;
    tunerI2C[nUnitId].WriteRegister = dvb_i2c_CommonWriteRegister;
    tunerI2C[nUnitId].WriteOneRegister = dvb_i2c_CommonWriteOneRegister;

    /* Setup demod I2C interfaces. */
    demodI2C[nUnitId].gwAddress = 0x00 ;
    demodI2C[nUnitId].gwSub = 0x00;  /* N/A */
    demodI2C[nUnitId].Read = drvi2c_feusb_Read ;
    demodI2C[nUnitId].Write = drvi2c_feusb_Write ;
    demodI2C[nUnitId].ReadRegister = dvb_i2c_CommonReadRegister;
    demodI2C[nUnitId].WriteRegister = dvb_i2c_CommonWriteRegister;
    demodI2C[nUnitId].WriteOneRegister = dvb_i2c_CommonWriteOneRegister;

    /* Setup tuner. */
    tuner[nUnitId].Initialize = dummy_tuner_Initialize ;
    tuner[nUnitId].Finalize = dummy_tuner_Finalize ;
    tuner[nUnitId].Tune = dummy_tuner_Tune ;
    tuner[nUnitId].Sleep = dummy_tuner_Sleep ;
    tuner[nUnitId].xtalFreq = 4 ;        /* 4MHz (dummy) */
    tuner[nUnitId].i2cAddress = 0xC0 ;   /* 0xC0 tuner I2C address. */
    tuner[nUnitId].pI2c = &tunerI2C[nUnitId];


    /* Setup demodulator. */
#if 0
    result =  dvb_cxd2820_Create (SONY_DVB_CXD2820_DVBT_ADDRESS,
                                  SONY_DVB_CXD2820_DVBC_ADDRESS,
                                  SONY_DVB_CXD2820_DVBT2_ADDRESS,
                                 &demodI2C[nUnitId],
                                 &demod[nUnitId],
                                 &tuner[nUnitId],
                                 &s_stCxd2820[nUnitId]);
#else
    result =  dvb_cxd2820_Create (ucI2cTAddr,
                                  ucI2cCAddr,
                                  ucI2cTAddr,
                                 &demodI2C[nUnitId],
                                 &demod[nUnitId],
                                 &tuner[nUnitId],
                                 &s_stCxd2820[nUnitId]);
#endif
	if(result == DI_CH_OK)
	{
		CH_PrintFunc("s_stCxd2820[%d][0x%x][0x%x]I2C", nUnitId, s_stCxd2820[nUnitId].pDemod->i2cAddressT, s_stCxd2820[nUnitId].pDemod->i2cAddressT2);
	}
	else
	{
		CH_PrintErr(" result[%d]", result);
	}
		/* Configure demod driver for specific tuner IF. */
		/* Please see specific tuner for details of IF. */
#if defined(CONFIG_PLATFORM_TN18273)
		demod[nUnitId].iffreq_config.config_DVBT6 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT (LOW_IF_7M_BW); 		/* 3.5MHz IF */
		demod[nUnitId].iffreq_config.config_DVBT7 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT (LOW_IF_7M_BW); 		/* 3.5MHz IF */
		demod[nUnitId].iffreq_config.config_DVBT8 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT (LOW_IF_8M_BW); 		/* 4MHz IF */
		demod[nUnitId].iffreq_config.config_DVBC = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBC (5.0);			/* 5MHz IF */

		/* T2 ITB setup. */
		demod[nUnitId].iffreq_config.config_DVBT2_5 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT2 (LOW_IF_7M_BW);		 /* 3.5MHz IF */
		demod[nUnitId].iffreq_config.config_DVBT2_6 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT2 (LOW_IF_7M_BW);		 /* 3.5MHz IF */
		demod[nUnitId].iffreq_config.config_DVBT2_7 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT2 (LOW_IF_7M_BW);		 /* 3.5MHz IF */
		demod[nUnitId].iffreq_config.config_DVBT2_8 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT2 (LOW_IF_8M_BW);		 /* 4MHz IF */
#else
		/* Supports High-If (Conventional MO-PLL) */
	demod[nUnitId].iffreq_config.config_DVBT6 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT (FT2117_DVBT_IF_FREQ_MHZ);	/* 36.0MHz IF */
	demod[nUnitId].iffreq_config.config_DVBT7 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT (FT2117_DVBT_IF_FREQ_MHZ);	/* 36.0MHz IF */
	demod[nUnitId].iffreq_config.config_DVBT8 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT (FT2117_DVBT_IF_FREQ_MHZ);	/* 36.0MHz IF */
	demod[nUnitId].iffreq_config.config_DVBC = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBC (36.125);		/* 36.125MHz IF */

	/* T2 ITB setup. */
	demod[nUnitId].iffreq_config.config_DVBT2_5 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT2 (FT2117_DVBT_IF_FREQ_MHZ); /* 36.0MHz IF */
	demod[nUnitId].iffreq_config.config_DVBT2_6 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT2 (FT2117_DVBT_IF_FREQ_MHZ); /* 36.0MHz IF */
	demod[nUnitId].iffreq_config.config_DVBT2_7 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT2 (FT2117_DVBT_IF_FREQ_MHZ); /* 36.0MHz IF */
	demod[nUnitId].iffreq_config.config_DVBT2_8 = DVB_DEMOD_MAKE_IFFREQ_CONFIG_DVBT2 (FT2117_DVBT_IF_FREQ_MHZ); /* 36.0MHz IF */

#endif


    /* IFAGC setup. Modify to suit connected tuner. */
    /* IFAGC postive, value = 0. */
#if 0
    result = dvb_demod_SetConfig (&demod, DEMOD_CONFIG_IFAGCNEG, 0);
    if (result != SONY_DVB_OK) {
        CH_PrintErr ("Error: Unable to configure IFAGCNEG. (status=%d, %s)\r\n", result, FormatResult(result));
        return -1 ;
    }
#endif

    /* Initialize the I2C */
    result = drvi2c_feusb_Initialize ();
    if (result != SONY_DVB_OK) {
        CH_PrintErr ("Error: Unable to initialise FEUSB I2C driver. (status=%d, %s)\r\n", result, FormatResult(result));
        return -1 ;
    }

    /* Initialize the system. */
    result = dvb_cxd2820_Initialize (&s_stCxd2820[nUnitId]);
    if (result != SONY_DVB_OK) {
        CH_PrintErr ("Error: Unable to initialise the CXD2820 driver. (status=%d, %s)\r\n", result, FormatResult(result));
        return -1 ;
    }

    /* -- Recommended -- */
    /* Enable the enhanced DVB-T2 long echo sequence. */
    result = dvb_demod_SetConfig (&demod[nUnitId], DEMOD_CONFIG_DVBT2_PP2_LONG_ECHO, 1);
    if (result != SONY_DVB_OK) {
        CH_PrintErr ("Error: Unable to configure DVBT2_PP2_LONG_ECHO. (status=%d, %s)\r\n", result, FormatResult(result));
        return -1 ;
    }

    result = dvb_demod_SetConfig (&demod[nUnitId], DEMOD_CONFIG_PARALLEL_SEL, 0);
    if (result != SONY_DVB_OK) {
        CH_PrintErr ("Error: Unable to configure DEMOD_CONFIG_PARALLEL_SEL. (status=%d, %s)\r\n", result, FormatResult(result));
        return -1 ;
    }

    result = dvb_demod_SetConfig (&demod[nUnitId], DEMOD_CONFIG_SCR_ENABLE, 1);
    if (result != SONY_DVB_OK) {
        CH_PrintErr ("Error: Unable to configure DEMOD_CONFIG_PARALLEL_SEL. (status=%d, %s)\r\n", result, FormatResult(result));
        return -1 ;
    }

	result = dvb_demod_SetConfig (&demod[nUnitId], DEMOD_CONFIG_TSCLK_CONT, 1);/* Continuous */
	if (result != SONY_DVB_OK)
	{
		CH_PrintErr ("Error: Unable to configure DEMOD_CONFIG_IFAGCNEG. (status=%d, %s)\r\n", result, FormatResult(result));
		return -1 ;
	}

    result = dvb_demod_SetConfig (&demod[nUnitId], DEMOD_CONFIG_IFAGCNEG, 0);
    if (result != SONY_DVB_OK) {
        CH_PrintErr ("Error: Unable to configure DEMOD_CONFIG_IFAGCNEG. (status=%d, %s)\r\n", result, FormatResult(result));
        return -1 ;
    }

	if( nUnitId	== MASTER_CHANNEL )
	{
		result = dvb_demod_GPIOSetting(&demod[nUnitId], GPIO_ANT_PWR_CHK, 1, GPIO_READ);
	    if (result != SONY_DVB_OK) {
	        CH_PrintErr ("Error: Unable to configure dvb_demod_GPIOSetting. (status=%d, %s)\r\n", result, FormatResult(result));
	        return -1 ;
	    }

		result = dvb_demod_GPIOSetting(&demod[nUnitId], GPIO_ANT_PWR_EN, 1, GPIO_WRITE);
	    if (result != SONY_DVB_OK) {
	        CH_PrintErr ("Error: Unable to configure dvb_demod_GPIOSetting. (status=%d, %s)\r\n", result, FormatResult(result));
	        return -1 ;
	    }
	}
	else
	{
		/* Do nothing */
	}


#if defined(CONFIG_COMMON_SHORTCIRCUIT)  /* HDR-FOX T2 */
	if( nUnitId	== MASTER_CHANNEL )
	{
		result = dvb_demod_GPIOSetting(&demod[nUnitId], GPIO_ANT_EN, 1, GPIO_WRITE);
	    if (result != SONY_DVB_OK) {
	        CH_PrintErr ("Error: Unable to configure dvb_demod_GPIOSetting. (status=%d, %s)\r\n", result, FormatResult(result));
	        return -1 ;
	    }
	}
	else
	{
		/* Do nothing */
	}
#endif

    /* Monitor the carrier offset. */
#if 0
    {
        int32sony_t offset ;
        result = dvb_demod_monitorT2_CarrierOffset(cxd2820.pDemod, &offset);
        if (result == SONY_DVB_OK) {
            CH_PrintDebug ("DVB-T2 TS lock achieved at at %dkHz, offset=%dkHz.\r\n", freqkHz, offset);
        }
    }

    {
        int32sony_t offset ;
        result = dvb_demod_monitorT_CarrierOffset(s_stCxd2820.pDemod, &offset);
        if (result == SONY_DVB_OK) {
            CH_PrintDebug ("DVB-T TS lock achieved at at %dkHz, offset=%dkHz.\r\n", freqkHz, offset);
        }
    }

#endif


#if 0
    /* Finalize the device. */
    result = dvb_cxd2820_Finalize (&cxd2820);
    if (result != SONY_DVB_OK) {
        CH_PrintErr ("Error: Unable to finalise the CXD2820 driver. (status=%d, %s)\r\n", result, FormatResult(result));
        return -1 ;
    }
#endif
    return 0 ;
}



/*
Strength Display specification

dBm		percent
-10		100
-15		100
-20		95
-25		90
-30		85
-35		80
-40		75
-45		70
-50		65
-55		60
-60		50
-65		40
-70		30
-75		25
-80		20
-85		10
-90		10
*/

static int CXD2820_AGC2Strength(int agc_indicator)
{
	int	strength_indicator ;
	int  IndexOfStrength[15]={105,97,90,84,79,75,71,66,51,44,39,33,26,15,9}; /* -15, -20, -25, -30, -35, -40, -45, -50, -55, -60, -65, -70, -75, -80, -85 value */

	if( agc_indicator >= IndexOfStrength[0] ) 		strength_indicator = 100;
	else if( agc_indicator >= IndexOfStrength[1] )	strength_indicator = 95 + (agc_indicator - IndexOfStrength[1]) * 5 / (IndexOfStrength[0] - IndexOfStrength[1]) ;
	else if( agc_indicator >= IndexOfStrength[2] )	strength_indicator = 90 + (agc_indicator - IndexOfStrength[2]) * 5 / (IndexOfStrength[1] - IndexOfStrength[2]) ;
	else if( agc_indicator >= IndexOfStrength[3] )	strength_indicator = 85 + (agc_indicator - IndexOfStrength[3]) * 5 / (IndexOfStrength[2] - IndexOfStrength[3]) ;
	else if( agc_indicator >= IndexOfStrength[4] )	strength_indicator = 80 + (agc_indicator - IndexOfStrength[4]) * 5 / (IndexOfStrength[3] - IndexOfStrength[4]) ;
	else if( agc_indicator >= IndexOfStrength[5] )	strength_indicator = 75 + (agc_indicator - IndexOfStrength[5]) * 5 / (IndexOfStrength[4] - IndexOfStrength[5]) ;
	else if( agc_indicator >= IndexOfStrength[6] )	strength_indicator = 70 + (agc_indicator - IndexOfStrength[6]) * 5 / (IndexOfStrength[5] - IndexOfStrength[6]) ;
	else if( agc_indicator >= IndexOfStrength[7] )	strength_indicator = 65 + (agc_indicator - IndexOfStrength[7]) * 5 / (IndexOfStrength[6] - IndexOfStrength[7]) ;
	else if( agc_indicator >= IndexOfStrength[8] )	strength_indicator = 60 + (agc_indicator - IndexOfStrength[8]) * 5 / (IndexOfStrength[7] - IndexOfStrength[8]) ;
	else if( agc_indicator >= IndexOfStrength[9] )	strength_indicator = 50 + (agc_indicator - IndexOfStrength[9]) * 10 / (IndexOfStrength[8] - IndexOfStrength[9]) ;
	else if( agc_indicator >= IndexOfStrength[10] )	strength_indicator = 40 + (agc_indicator - IndexOfStrength[10]) * 10 / (IndexOfStrength[9] - IndexOfStrength[10]) ;
	else if( agc_indicator >= IndexOfStrength[11] )	strength_indicator = 30 + (agc_indicator - IndexOfStrength[11]) * 10 / (IndexOfStrength[10] - IndexOfStrength[11]) ;
	else if( agc_indicator >= IndexOfStrength[12] )	strength_indicator = 25 + (agc_indicator - IndexOfStrength[12]) * 5 / (IndexOfStrength[11] - IndexOfStrength[12]) ;
	else if( agc_indicator >= IndexOfStrength[13] )	strength_indicator = 20 + (agc_indicator - IndexOfStrength[13]) * 5 / (IndexOfStrength[12] - IndexOfStrength[13]) ;
	else if( agc_indicator >= IndexOfStrength[14] )	strength_indicator = 10 + (agc_indicator - IndexOfStrength[14]) * 10 / (IndexOfStrength[13] - IndexOfStrength[14]) ;
	else strength_indicator = 10;

	return (strength_indicator);
}


#if defined( CONFIG_NORDIC_SSI_SQI )

static int GetRefSignalLevel( CH_TER_System_e etTerSystem, CH_TER_Moudulation_e	 etModulation, CH_TER_CodeRate_e etCodeRate )
{
	int nRefSignalLevel = 0;

	if( etTerSystem == DI_CH_DVBT )
	{
		if( etModulation == DI_CH_TMOD_QPSK )
		{
			if( etCodeRate == DI_CH_TCODERATE_1_2 )
			{
				nRefSignalLevel = -93;
			}
			else if( etCodeRate == DI_CH_TCODERATE_2_3 )
			{
				nRefSignalLevel = -91;
			}
			else if( etCodeRate == DI_CH_TCODERATE_3_4 )
			{
				nRefSignalLevel = -90;
			}
			else if( etCodeRate == DI_CH_TCODERATE_5_6 )
			{
				nRefSignalLevel = -89;
			}
			else
			{
				nRefSignalLevel = -88;
			}
		}
		else if( etModulation == DI_CH_TMOD_16QAM )
		{
			if( etCodeRate == DI_CH_TCODERATE_1_2 )
			{
				nRefSignalLevel = -87;
			}
			else if( etCodeRate == DI_CH_TCODERATE_2_3 )
			{
				nRefSignalLevel = -85;
			}
			else if( etCodeRate == DI_CH_TCODERATE_3_4 )
			{
				nRefSignalLevel = -84;
			}
			else if( etCodeRate == DI_CH_TCODERATE_5_6 )
			{
				nRefSignalLevel = -83;
			}
			else
			{
				nRefSignalLevel = -82;
			}
		}
		else // DI_CH_TMOD_64QAM
		{
			if( etCodeRate == DI_CH_TCODERATE_1_2 )
			{
				nRefSignalLevel = -82;
			}
			else if( etCodeRate == DI_CH_TCODERATE_2_3 )
			{
				nRefSignalLevel = -80;
			}
			else if( etCodeRate == DI_CH_TCODERATE_3_4 )
			{
				nRefSignalLevel = -78;
			}
			else if( etCodeRate == DI_CH_TCODERATE_5_6 )
			{
				nRefSignalLevel = -77;
			}
			else
			{
				nRefSignalLevel = -76;
			}
		}
	}
	else // DI_CH_DVBT2
	{
		if( etModulation == DI_CH_TMOD_QPSK )
		{
			if( etCodeRate == DI_CH_TCODERATE_1_2 )
			{
				nRefSignalLevel = -93;
			}
			else if( etCodeRate == DI_CH_T2CODERATE_3_5 )
			{
				nRefSignalLevel = -92;
			}
			else if( etCodeRate == DI_CH_TCODERATE_2_3 )
			{
				nRefSignalLevel = -91;
			}
			else if( etCodeRate == DI_CH_TCODERATE_3_4 )
			{
				nRefSignalLevel = -90;
			}
			else if( etCodeRate == DI_CH_T2CODERATE_4_5)
			{
				nRefSignalLevel = -90;
			}
			else // 5/6
			{
				nRefSignalLevel = -89;
			}
		}
		else if( etModulation == DI_CH_TMOD_16QAM )
		{
			if( etCodeRate == DI_CH_TCODERATE_1_2 )
			{
				nRefSignalLevel = -87;
			}
			else if( etCodeRate == DI_CH_T2CODERATE_3_5 )
			{
				nRefSignalLevel = -86;
			}
			else if( etCodeRate == DI_CH_TCODERATE_2_3 )
			{
				nRefSignalLevel = -85;
			}
			else if( etCodeRate == DI_CH_TCODERATE_3_4 )
			{
				nRefSignalLevel = -84;
			}
			else if( etCodeRate == DI_CH_T2CODERATE_4_5)
			{
				nRefSignalLevel = -84;
			}
			else // 5/6
			{
				nRefSignalLevel = -83;
			}
		}
		else if( etModulation == DI_CH_TMOD_64QAM )
		{
			if( etCodeRate == DI_CH_TCODERATE_1_2 )
			{
				nRefSignalLevel = -82;
			}
			else if( etCodeRate == DI_CH_T2CODERATE_3_5 )
			{
				nRefSignalLevel = -81;
			}
			else if( etCodeRate == DI_CH_TCODERATE_2_3 )
			{
				nRefSignalLevel = -80;
			}
			else if( etCodeRate == DI_CH_TCODERATE_3_4 )
			{
				nRefSignalLevel = -78;
			}
			else if( etCodeRate == DI_CH_T2CODERATE_4_5)
			{
				nRefSignalLevel = -78;
			}
			else // 5/6
			{
				nRefSignalLevel = -77;
			}
		}
		else // DI_CH_TMOD_256QAM
		{
			if( etCodeRate == DI_CH_TCODERATE_1_2 )
			{
				nRefSignalLevel = -76;
			}
			else if( etCodeRate == DI_CH_T2CODERATE_3_5 )
			{
				nRefSignalLevel = -75;
			}
			else if( etCodeRate == DI_CH_TCODERATE_2_3 )
			{
				nRefSignalLevel = -74;
			}
			else if( etCodeRate == DI_CH_TCODERATE_3_4 )
			{
				nRefSignalLevel = -73;
			}
			else if( etCodeRate == DI_CH_T2CODERATE_4_5)
			{
				nRefSignalLevel = -72;
			}
			else // 5/6
			{
				nRefSignalLevel = -71;
			}
		}
	}

	return nRefSignalLevel;
}



static HUINT8 CalculateSsi( int nReceivedDbm, int nReferenceDbm )
{
	float fTmpVal=0;
	HUINT8 ucSsi = 0;

	int nRelativeDbm=0;

	nRelativeDbm = nReceivedDbm - nReferenceDbm;
	//nRelativeDbm = nReceivedDbm + 80;

	CH_PrintData ("DEMOD[0] : nRelativeDbm (%d):(%d)-(%d) nReceivedDbm-nReferenceDbm\n", nRelativeDbm, nReceivedDbm, nReferenceDbm);

	if( nRelativeDbm < -15)
	{
		ucSsi = 0;
	}
	else if( nRelativeDbm >= -15 && nRelativeDbm < 0 )
	{
		fTmpVal = (2.0/3.0)*(nRelativeDbm + 15);
		ucSsi = (HUINT8)fTmpVal;
	}
	else if( nRelativeDbm >= 0 && nRelativeDbm < 20 )
	{
		fTmpVal = 4*nRelativeDbm + 10;
		ucSsi = (HUINT8)fTmpVal;
	}
	else if( nRelativeDbm >= 20 && nRelativeDbm < 35 )
	{
		fTmpVal = (2.0/3.0)*(nRelativeDbm - 20)+90;
		ucSsi = (HUINT8)fTmpVal;
	}
	else
	{
		ucSsi = 100;
	}
	CH_PrintData ("DEMOD[0] :  fTmpVal (%f):(%d) ucSsi\n", fTmpVal, ucSsi);
	return ucSsi;

}



static int CalculateDbm( HUINT32 ulRfAgc, HUINT32 ulIfAgc )
{
	float fTmpVal=0, fStep=0;
	HUINT8 ucSsi = 0;
	int i=0;
	int nDbm = 0;


	int anRfAgc[51]={0xe8,0xea,0xf0,0xf3,0xf7,0xfd,0x103,0x10a,0x111,0x118,0x123,0x12e,0x13b,0x147,0x158,0x169,
		0x17a,0x18c,0x199,0x1a8,0x1b6,0x1c4,0x1d1,0x1db,0x1e6,0x1f1,0x1fa,0x203,0x20b,0x213,0x21b,
		0x222,0x22a,0x230,0x237,0x23e,0x245,0x24b,0x250,0x256,0x25c,0x262,0x267,0x26e,0x273,0x27a,
		0x283,0x28d,0x299,0x2b5,0x39d};

	int anIfAgc[30]={0x562,0x583,0x5a2,0x5c3,0x5e6,0x608,0x62a,0x647,0x664,0x682,0x6a2,0x6c1,0x6de,0x6fb,
		0x71c,0x73c,0x75e,0x77e,0x7a1,0x7c6,0x7f1,0x81f,0x84b,0x87e,0x8b5,0x8f1,0x936,0x97d,0xff7,0xffd};

	if(ulRfAgc < 0x3fd)
	{
		for(i=0;i<51;i++)
		{
			if(ulRfAgc < anRfAgc[i])
			{
				break;
			}
		}

		nDbm = (i+1)*(-1);

	}
	else
	{
		for(i=0;i<30;i++)
		{
			if(ulIfAgc < anIfAgc[i])
			{
				break;
			}
		}

		if(ulIfAgc > 0xffd)
		{
			nDbm = -110;
		}
		else
		{
			nDbm = (i+52)*(-1);
		}

	}


	//return nDbm;
	return (nDbm+1);

}

#endif //#if defined( CONFIG_NORDIC_SSI_SQI )

static int CXD2820_CheckAgcBer( int nChannelId, HUINT32* strength_indicator, HUINT32* quality_indicator )
{
	int nRet = 0, nResult = 0;
	HUINT32 ulRfAgc = 0, ulIfAgc = 0, ulBer = 0, ulFer=0;
	HUINT32 Strength = 0, Quality = 0;
	CH_TER_TuneParam_t *pstTerTuneParam;
	float	fdBuV = 0.0;//, fdBm = 0.0;
#if defined( CONFIG_NORDIC_SSI_SQI )
    HUINT8 sqi = 0 ;
	HUINT8 ucSsi = 0;
	int nReceivedDbm = 0, nReferenceDbm = 0;
	sony_dvb_demod_mer_filter_t merFilter;
#endif

#if defined(MONITOR_CXD2820_STATUS)
	HINT32 nSnr = 0, nMer = 0;
#endif


	int			nUnitId = 0;
	nUnitId = GetDeviceUnitId( nChannelId );

	pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );

	nRet = dvb_demod_monitor_RFAGCOut (&demod[nUnitId], &ulRfAgc);
	if( nRet != DI_CH_OK )
	{
		CH_PrintErr(" ");
		nResult = DI_CH_ERR;
	}

	nRet = dvb_demod_monitor_IFAGCOut (&demod[nUnitId], &ulIfAgc);
	if( nRet != DI_CH_OK )
	{
		CH_PrintErr(" ");
		nResult = DI_CH_ERR;
	}

	CH_PrintData ("DEMOD[%d] : ulRfAgc = (0x%X) : (0x%X) = ulIfAgc \n", nUnitId, ulRfAgc, ulIfAgc);



#if defined( CONFIG_NORDIC_SSI_SQI )

#if defined(CONFIG_PLATFORM_TN18273)
	nRet = TDA18273_GetPowerLevel(nUnitId, &fdBuV);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : TDA18273_GetPowerLevel()... nRet(0x%x)... \n", nRet));
	}
	else
	{
		nReceivedDbm = fdBuV + DBUV_TO_DBM + CALIBRATION_TDA18273;
	}
#else
	nReceivedDbm = CalculateDbm( ulRfAgc, ulIfAgc );
#endif
	nReferenceDbm = GetRefSignalLevel(pstTerTuneParam->etTerSystem, pstTerTuneParam->etModulation, pstTerTuneParam->etCodeRate);

	ucSsi = CalculateSsi( nReceivedDbm, nReferenceDbm );

	*strength_indicator = ucSsi;
#else

	if(ulRfAgc > 0x2ff ) ulRfAgc = 0x2ff;
	if(ulIfAgc > 0x9ff ) ulIfAgc = 0x9ff;

	Strength = (int)(200-((float)ulRfAgc / 8 + (float) ulIfAgc / 27));
	*strength_indicator = CXD2820_AGC2Strength (Strength) ;
#endif

	if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
	{
		nRet = dvb_demod_monitorT2_PreLDPCBER( &demod[nUnitId], &ulBer );
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}

		nRet = dvb_demod_monitorT2_PostBCHFER( &demod[nUnitId], &ulFer );
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}

		if( ulFer == 0 ) {
			if(ulBer < 1300000)			Quality = 100;
			else if(ulBer < 1330000)    	Quality = 90;
			else if(ulBer < 1360000) 	Quality = 80;
			else if(ulBer < 1390000)		Quality = 70;
			else						Quality = 60;
			}
		else {
			if(ulFer < 100000 ) 			Quality = 50;
			else if(ulFer < 200000 )		Quality = 40;
			else if(ulFer < 400000 )		Quality = 30;
			else if(ulFer < 600000 )		Quality = 20;
			else if(ulFer < 800000 )		Quality = 10;
			else						Quality = 0;
			}
		//*quality_indicator = Quality;
	}

	else if( pstTerTuneParam->etTerSystem == DI_CH_DVBT )
	{
		nRet = dvb_demod_monitorT_PreRSBER( &demod[nUnitId], &ulBer );
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}

		nRet = dvb_demod_monitorT_RSError(&demod[nUnitId], &ulFer);
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}

		if(ulBer < 1000)				Quality = 100;
		else if(ulBer < 2500)    	Quality = 90;
		else if(ulBer < 5000) 		Quality = 80;
		else if(ulBer < 7500)		Quality = 70;
		else if(ulBer < 10000)		Quality = 60;
		else if(ulBer < 25000) 		Quality = 50;
		else if(ulBer < 50000)		Quality = 40;
		else if(ulBer < 75000)		Quality = 30;
		else if(ulBer < 100000)		Quality = 20;
		else if(ulBer < 500000)		Quality = 10;
		else						Quality = 0;

		//*quality_indicator = Quality;
	}

#if defined(MONITOR_CXD2820_STATUS)
	if(demod[nUnitId].system == SONY_DVB_SYSTEM_DVBT)
	{
		nRet = dvb_demod_monitorT_SNR( &demod[nUnitId], &nSnr );
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}

		nRet = dvb_demod_monitorT_MER( &demod[nUnitId], &nMer );
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}
	}
	else if(demod[nUnitId].system == SONY_DVB_SYSTEM_DVBT2)
	{
		nRet = dvb_demod_monitorT2_SNR( &demod[nUnitId], &nSnr );
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}

		nRet = dvb_demod_monitorT2_MER( &demod[nUnitId], &nMer );
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}
	}
	else
	{
		CH_DI_Print(0, ( "Error : Unknown system(%d) : demod[%d]\n", demod[nUnitId].system, nUnitId));
	}

	CH_PrintData ("DEMOD[%d] : SNR = (%f) MER = (%f) \n", nUnitId, ((float)nSnr)/1000.0, ((float)nMer)/1000.0);

#endif

#if defined(MONITOR_CXD2820_ERROR_STATE)
	//CH_PrintData ("DEMOD[%d] : RFAGC = (0x%x) IFAGC = (0x%x) \n", nUnitId, ulRfAgc, ulIfAgc);
	//CH_PrintData ("DEMOD[%d] : Quality = %d, Strength = %d <- %d \n", nUnitId, Quality, *strength_indicator, Strength);
	if(demod[nUnitId].system == SONY_DVB_SYSTEM_DVBT2)
	{
		CH_PrintData ("DEMOD[%d] : BER = (%d) FER = (%d) \n", nUnitId, ulBer, ulFer);
	}
	else
	{

		CH_PrintData ("DEMOD[%d] : BER = (%d) RS ERR = (%d) \n", nUnitId, ulBer, ulFer);
	}
#endif

#if defined( CONFIG_NORDIC_SSI_SQI )

    /* Reset and initialise MER filter. */
    nRet = dvb_demod_monitorT2_InitMERFilter(&merFilter);
    if (nRet != SONY_DVB_OK) {
        CH_PrintData ("Error: Unable to initialise MER filter. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
    }

    /*------------------------------------------------------------------------------
		SQI Monitor
    ------------------------------------------------------------------------------*/
    nRet = dvb_demod_monitor_QualityV2 (&demod[nUnitId], &merFilter, &sqi);
    if (nRet == SONY_DVB_OK)
	{
        CH_DI_Print(4, ("DEMOD[%d] : SSI = (%d) : (%d) = SQI : [%d]dBm\r\n", nUnitId, ucSsi, sqi, nReceivedDbm));
    }

	*quality_indicator = sqi;

#else

	*quality_indicator = Quality;
#endif

	return nResult;

}



static int CXD2820_UpdateTunedParam(int nChannelId, CH_TER_TuneParam_t *pstTerTuneParam)
{
	int	nResult = DI_CH_OK;
	int	nRet = SONY_DVB_OK;
	//sony_dvb_demod_lock_result_t etLockStatus;

	sony_dvb_demod_dvbt_mode_t pMode;
	sony_dvb_demod_dvbt_guard_t pGuard;
	sony_dvb_demod_tpsinfo_t stTPSInfo;

	sony_dvb_dvbt2_l1pre_t stL1Pre;
	//sony_dvb_dvbt2_plp_t stPLPInfo;

	int			nUnitId = 0;
	nUnitId = GetDeviceUnitId( nChannelId );

	if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
	{
		/* Convert Sony_L1_Pre Parameter to DI_T2 Parameter */
		nRet = dvb_demod_monitorT2_L1Pre(&demod[nUnitId], &stL1Pre);
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}

		switch (stL1Pre.papr)
		{
			case SONY_DVB_DVBT2_PAPR_NONE:
				pstTerTuneParam->stT2Param.etPaprMode = DI_CH_NONE_PAPR; 	break;
			case SONY_DVB_DVBT2_PAPR_ACE:
				pstTerTuneParam->stT2Param.etPaprMode = DI_CH_ACE_PAPR; 	break;
			case SONY_DVB_DVBT2_PAPR_TR:
				pstTerTuneParam->stT2Param.etPaprMode = DI_CH_TR_PAPR; 		break;
			case SONY_DVB_DVBT2_PAPR_TR_ACE:
				pstTerTuneParam->stT2Param.etPaprMode = DI_CH_ACETR_PAPR; 	break;
			default:
				pstTerTuneParam->stT2Param.etPaprMode = DI_CH_NONE_PAPR;
		}

		switch (stL1Pre.s2 & 0x01)
		{
			case 0:
				pstTerTuneParam->stT2Param.etPreambleMixed = DI_CH_T2NOT_MIXED; 	break;
			case 1:
				pstTerTuneParam->stT2Param.etPreambleMixed = DI_CH_T2MIXED; 		break;
			default:
				pstTerTuneParam->stT2Param.etPreambleMixed = DI_CH_T2NOT_MIXED;
		}

		switch ( stL1Pre.s1 )
		{
			case SONY_DVB_DVBT2_S1_SISO:
				pstTerTuneParam->stT2Param.etPreambleFormat = DI_CH_T2SISO; 		break;
			case SONY_DVB_DVBT2_S1_MISO:
				pstTerTuneParam->stT2Param.etPreambleFormat = DI_CH_T2MISO; 		break;
			default:
				pstTerTuneParam->stT2Param.etPreambleFormat = DI_CH_T2SISO;
		}

		switch ( stL1Pre.fftMode )
		{
			case SONY_DVB_DVBT2_M2K:
				pstTerTuneParam->etTransMode = DI_CH_TRANSMODE_2K; 				break;
			case SONY_DVB_DVBT2_M8K:
				pstTerTuneParam->etTransMode = DI_CH_TRANSMODE_8K; 				break;
			case SONY_DVB_DVBT2_M4K:
				pstTerTuneParam->etTransMode = DI_CH_T2TRANSMODE_4K; 			break;
			case SONY_DVB_DVBT2_M1K:
				pstTerTuneParam->etTransMode = DI_CH_T2TRANSMODE_1K; 				break;
			case SONY_DVB_DVBT2_M16K:
				pstTerTuneParam->etTransMode = DI_CH_T2TRANSMODE_16K; 				break;
			case SONY_DVB_DVBT2_M32K:
				pstTerTuneParam->etTransMode = DI_CH_T2TRANSMODE_32K; 				break;
			default:
				pstTerTuneParam->etTransMode = DI_CH_TRANSMODE_8K;
		}

		switch ( stL1Pre.gi )
		{
			case SONY_DVB_DVBT2_G1_32:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_32; 		break;
			case SONY_DVB_DVBT2_G1_16:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_16; 		break;
			case SONY_DVB_DVBT2_G1_8:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_8; 		break;
			case SONY_DVB_DVBT2_G1_4:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_4; 		break;
			case SONY_DVB_DVBT2_G1_128:
				pstTerTuneParam->etGuardInterval = DI_CH_T2GUARDINTERVAL_1_128; 	break;
			case SONY_DVB_DVBT2_G19_128:
				pstTerTuneParam->etGuardInterval = DI_CH_T2GUARDINTERVAL_19_128; 	break;
			case SONY_DVB_DVBT2_G19_256:
				pstTerTuneParam->etGuardInterval = DI_CH_T2GUARDINTERVAL_19_256; 	break;
			default:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_8;
		}

		switch ( stL1Pre.pp )
		{
			case SONY_DVB_DVBT2_PP1:
				pstTerTuneParam->stT2Param.etPilotPattern = DI_CH_T2_PP1; 			break;
			case SONY_DVB_DVBT2_PP2:
				pstTerTuneParam->stT2Param.etPilotPattern = DI_CH_T2_PP2; 			break;
			case SONY_DVB_DVBT2_PP3:
				pstTerTuneParam->stT2Param.etPilotPattern = DI_CH_T2_PP3; 			break;
			case SONY_DVB_DVBT2_PP4:
				pstTerTuneParam->stT2Param.etPilotPattern = DI_CH_T2_PP4; 			break;
			case SONY_DVB_DVBT2_PP5:
				pstTerTuneParam->stT2Param.etPilotPattern = DI_CH_T2_PP5; 			break;
			case SONY_DVB_DVBT2_PP6:
				pstTerTuneParam->stT2Param.etPilotPattern = DI_CH_T2_PP6; 			break;
			case SONY_DVB_DVBT2_PP7:
				pstTerTuneParam->stT2Param.etPilotPattern = DI_CH_T2_PP7; 			break;
			default:
				pstTerTuneParam->stT2Param.etPilotPattern = DI_CH_T2_PP7;
		}

		switch ( stL1Pre.bwExt )
		{
			case 0:
				pstTerTuneParam->stT2Param.etExtendedCarrier = DI_CH_T2NOT_USED; 	break;
			case 1:
				pstTerTuneParam->stT2Param.etExtendedCarrier = DI_CH_T2USED; 		break;
			default:
				pstTerTuneParam->stT2Param.etPilotPattern = DI_CH_T2NOT_USED;
		}

		pstTerTuneParam->stT2Param.ulPlpNumBlocks = stL1Pre.numSymbols;

		pstTerTuneParam->etHierarchy = DI_CH_HIERARCHY_NONE;

		CH_PrintData ("**************************** Channel_Param_T2 ************************\n");
		CH_PrintData ("preamble mixed indicator	%s [%d] \n", pszCH_TER_PreambleMixed[pstTerTuneParam->stT2Param.etPreambleMixed],pstTerTuneParam->stT2Param.etPreambleMixed);
		CH_PrintData ("S1                       %s [%d] \n", pszCH_TER_PreambleFormat[pstTerTuneParam->stT2Param.etPreambleFormat],pstTerTuneParam->stT2Param.etPreambleFormat);
		CH_PrintData ("Mode                     %s [%d] \n", pszCH_TER_TransMode[pstTerTuneParam->etTransMode],pstTerTuneParam->etTransMode);
		CH_PrintData ("GI                       %s [%d] \n", pszCH_TER_GuardInterval[pstTerTuneParam->etGuardInterval],pstTerTuneParam->etGuardInterval);
		CH_PrintData ("PP                       %s [%d] \n", pszCH_TER_PilotPattern[pstTerTuneParam->stT2Param.etPilotPattern],pstTerTuneParam->stT2Param.etPilotPattern);
		CH_PrintData ("bwExtended               %s [%d] \n", pszCH_TER_ExtendedCarrier[pstTerTuneParam->stT2Param.etExtendedCarrier],pstTerTuneParam->stT2Param.etExtendedCarrier);
		CH_PrintData ("PAPR                     %s [%d] \n", pszCH_TER_PaprMode[pstTerTuneParam->stT2Param.etPaprMode],pstTerTuneParam->stT2Param.etPaprMode);
		CH_PrintData ("numSymbols               %d %d \n", stL1Pre.numSymbols,pstTerTuneParam->stT2Param.ulPlpNumBlocks);

	}
	else if( pstTerTuneParam->etTerSystem == DI_CH_DVBT )
	{
		nRet = dvb_demod_monitorT_ModeGuard(&demod[nUnitId], &pMode, &pGuard);
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}

		switch ( pMode )
		{
			case DVBT_MODE_2K:
				pstTerTuneParam->etTransMode = DI_CH_TRANSMODE_2K; 				break;
			case DVBT_MODE_8K:
				pstTerTuneParam->etTransMode = DI_CH_TRANSMODE_8K; 				break;
			default:
				pstTerTuneParam->etTransMode = DI_CH_TRANSMODE_8K;
		}

		switch ( pGuard )
		{
			case DVBT_GUARD_1_32:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_32; 		break;
			case DVBT_GUARD_1_16:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_16; 		break;
			case DVBT_GUARD_1_8:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_8; 		break;
			case DVBT_GUARD_1_4:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_4; 		break;
			default:
				pstTerTuneParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_8;
		}

		nRet = dvb_demod_monitorT_TPSInfo(&demod[nUnitId], &stTPSInfo);
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
			nResult = DI_CH_ERR;
		}

		switch ( stTPSInfo.hierarchy )
		{
			case DVBT_HIERARCHY_NON:
				pstTerTuneParam->etHierarchy = DI_CH_HIERARCHY_NONE; 			break;
			case DVBT_HIERARCHY_1:
				pstTerTuneParam->etHierarchy = DI_CH_HIERARCHY_1; 				break;
			case DVBT_HIERARCHY_2:
				pstTerTuneParam->etHierarchy = DI_CH_HIERARCHY_2; 				break;
			case DVBT_HIERARCHY_4:
				pstTerTuneParam->etHierarchy = DI_CH_HIERARCHY_4; 				break;
			default:
				pstTerTuneParam->etHierarchy = DI_CH_HIERARCHY_NONE;
		}

		switch ( stTPSInfo.constellation )
		{
			case DVBT_CONSTELLATION_QPSK:
				pstTerTuneParam->etModulation = DI_CH_TMOD_QPSK; 				break;
			case DVBT_CONSTELLATION_16QAM:
				pstTerTuneParam->etModulation = DI_CH_TMOD_16QAM; 				break;
			case DVBT_CONSTELLATION_64QAM:
				pstTerTuneParam->etModulation = DI_CH_TMOD_64QAM; 				break;
			default:
				pstTerTuneParam->etModulation = DI_CH_TMOD_64QAM;
		}

		if( pstTerTuneParam->etStream == DI_CH_STREAM_HP )
		{
			switch ( stTPSInfo.rateHP )
			{
				case DVBT_CODERATE_1_2:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_1_2; 				break;
				case DVBT_CODERATE_2_3:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_2_3; 				break;
				case DVBT_CODERATE_3_4:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_3_4; 				break;
				case DVBT_CODERATE_5_6:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_5_6; 				break;
				case DVBT_CODERATE_7_8:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_7_8; 				break;
				default:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_2_3;
			}
		}

		if( pstTerTuneParam->etStream == DI_CH_STREAM_LP )
		{
			switch ( stTPSInfo.rateLP )
			{
				case DVBT_CODERATE_1_2:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_1_2; 				break;
				case DVBT_CODERATE_2_3:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_2_3; 				break;
				case DVBT_CODERATE_3_4:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_3_4; 				break;
				case DVBT_CODERATE_5_6:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_5_6; 				break;
				case DVBT_CODERATE_7_8:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_7_8; 				break;
				default:
					pstTerTuneParam->etCodeRate = DI_CH_TCODERATE_2_3;
			}
		}

		CH_PrintData ("**************************** Channel_Param_T ************************\n");
		CH_PrintData ("Mode                     %s [%d] \n", pszCH_TER_TransMode[pstTerTuneParam->etTransMode],pstTerTuneParam->etTransMode);
		CH_PrintData ("GI                       %s [%d] \n", pszCH_TER_GuardInterval[pstTerTuneParam->etGuardInterval],pstTerTuneParam->etGuardInterval);
		CH_PrintData ("Hierarchy                %s [%d] \n", pszCH_TER_Hierarchy[pstTerTuneParam->etHierarchy],pstTerTuneParam->etHierarchy);
		CH_PrintData ("Modulation               %s [%d] \n", pszCH_TER_Moudulation[pstTerTuneParam->etModulation],pstTerTuneParam->etModulation);
		CH_PrintData ("Code Rate                %s [%d] \n", pszCH_TER_code_rate[pstTerTuneParam->etCodeRate],pstTerTuneParam->etCodeRate);
		CH_PrintData ("**********************************************************************\n");
	}

	return nResult;
}


static int CXD2820_MonitorPLP(int nUnitId)
{
	int nRet = 0, i;
	sony_dvb_dvbt2_plp_t stPLPInfo;

	nRet = dvb_demod_monitorT2_DataPLPs(&demod[nUnitId], s_aucPlpID[nUnitId], &s_ucNumberPlp[nUnitId]);

	CH_PrintData ("***************************** PLP information ************************\n");
	CH_PrintData ("DEMOD[%d] : PLP s_ucNumberPlp = %d \n", nUnitId, s_ucNumberPlp[nUnitId]);
	for( i = 0; i<s_ucNumberPlp[nUnitId]; i++ )
	{
		CH_PrintData("s_aucPlpID[%d] = %d\n", i, s_aucPlpID[nUnitId][i]);
	}

	CH_PrintData ("**********************************************************************\n");

	nRet = dvb_demod_monitorT2_ActivePLP(&demod[nUnitId], SONY_DVB_DVBT2_PLP_DATA, &stPLPInfo );
	if( nRet != DI_CH_OK )
	{
		CH_PrintErr(" ");
	}
	CH_PrintData ("\n[ Active Data PLP information ] \n");
	CH_PrintData ("PLP ID                   [%d] \n", stPLPInfo.id);
	CH_PrintData ("PLP type                 [%d] \n", stPLPInfo.type);
	CH_PrintData ("PLP payload              [%d] \n", stPLPInfo.payload);
	CH_PrintData ("PLP constellation        [%d] \n", stPLPInfo.constell);
	CH_PrintData ("PLP code rate            [%d] \n", stPLPInfo.plpCr);
	CH_PrintData ("PLP rotation             [%d] \n", stPLPInfo.rot);
	CH_PrintData ("PLP groupId              [%d] \n", stPLPInfo.groupId);
	CH_PrintData ("**********************************************************************\n");

	if( s_ucNumberPlp[nUnitId] > 2 )
	{
		nRet = dvb_demod_monitorT2_ActivePLP(&demod[nUnitId], SONY_DVB_DVBT2_PLP_COMMON, &stPLPInfo );
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr(" ");
		}
		CH_PrintData ("\n[ Active Common PLP information ] \n");
		CH_PrintData ("PLP ID                   [%d] \n", stPLPInfo.id);
		CH_PrintData ("PLP type                 [%d] \n", stPLPInfo.type);
		CH_PrintData ("PLP payload              [%d] \n", stPLPInfo.payload);
		CH_PrintData ("PLP constellation        [%d] \n", stPLPInfo.constell);
		CH_PrintData ("PLP code rate            [%d] \n", stPLPInfo.plpCr);
		CH_PrintData ("PLP rotation             [%d] \n", stPLPInfo.rot);
		CH_PrintData ("PLP groupId              [%d] \n", stPLPInfo.groupId);
		CH_PrintData ("**********************************************************************\n");
	}
	else
	{
		CH_PrintFunc("Not multiple PLP system");
	}

	return nRet;

}



#define __DRVCH_T_PUBLIC_FUNCITONS__

int CXD2820_ReInitDevice( int nChannelId )
{
    int nResult = DI_CH_OK;

    CH_DI_Print(6,("CXD2820_ReInitDevice Enter ...\n"));

    /* TODO : to be implemented ... */
    CH_UNUSED(nChannelId);

    return nResult;
}

int CXD2820_UnInitDevice( int nChannelId )
{
    int nResult = DI_CH_OK;

    CH_DI_Print(6,("CXD2820_UnInitDevice Enter ...\n"));

    /* TODO : to be implemented ... */
    CH_UNUSED(nChannelId);

    return nResult;
}


void DRV_T_InstallApi( void )
{
	TER_InitDevice 				= &CXD2820_InitDevice;
	TER_SetTune					= &CXD2820_SetTune;
	TER_CheckLock 				= &CXD2820_CheckLock;

	TER_GetSignalInfo 		= &CXD2820_GetSignalInfo;
	TER_SetPowerState 			= &CXD2820_SetPowerState;
	TER_EnableTsOutput 			= &CXD2820_EnableTsOutput;
	TER_DisableTsOutput			= &CXD2820_DisableTsOutput;

	TER_CheckAntState 			= &CXD2820_CheckAntState;
	TER_SetAntState				= &CXD2820_SetAntState;
	TER_GetAntState				= &CXD2820_GetAntState;
	TER_ProcessRecoverAntenna 	= &CXD2820_ProcessRecoverAntenna;

	TER_GetDataPlpCount 		= &CXD2820_GetPlpCount;
	TER_GetDataPlpInfo 			= &CXD2820_GetPlpInfo;

	TER_ResetGpio 				= &DummyCXD2820_ResetGpio;
	TER_Initialize 				= &DummyCXD2820_Initialize;

    TER_ReInitDevice		= &CXD2820_ReInitDevice;
    TER_UnInitDevice              = &CXD2820_UnInitDevice;

 	CH_PrintDebug("DRV_T_InstallApi()  OK! \n");
}



int CXD2820_InitDevice( int nChannelId )
{
	int	nResult = DI_CH_OK;
	int nRet	= DI_CH_OK;
	HUINT32	ulI2cChannel = I2C_CHANNEL_CHANNEL;
	int			nUnitId = 0;
	nUnitId = GetDeviceUnitId( nChannelId );

	ResetChannelHw( nUnitId );

	if( nUnitId == MASTER_CHANNEL )
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
	else
	{
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
#else
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
#endif
	}

	DRV_I2c_SetClock( ulI2cChannel, DRV_I2C_CLOCK_100K );

	nRet = InitCxd2820( nUnitId );
	if( nRet != DI_CH_OK )
	{
		CH_PrintErr(" ");
		nResult = DI_CH_ERR;
	}

	return nResult;
}



int CXD2820_SetTune( int nChannelId,  HUINT8 *bChecklock, HUINT32 unWorkingId )
{
	int	nResult = DI_CH_OK;
	int	nRet = DI_CH_OK;
	HUINT32		ulFreqKhz = 0, ulStepKhz = 0;
	int			nUnitId = 0;
	CH_TER_TuneParam_t *pstTerTuneParam;
	CH_TER_BandWidth_e etBandwidth;
	int nBandwidth = 8, nOffsetKhz = 0;
	CH_Status_e etCurrentState;

	sony_dvb_tune_params_t stTuneParams;
	CH_UNUSED(unWorkingId);

	CH_PrintDebug("Enter ...\n");

	//ulStepKhz = (FT2117_DVBT_REF_FREQ_HZ + 500) / 1000;
	DRV_CH_LockReentrant();

	nUnitId = GetDeviceUnitId( nChannelId );
	pstTerTuneParam = (CH_TER_TuneParam_t *)GetWorkingTuneInfo( nChannelId );
	//PrintTerTuneParam( pstTerTuneParam );

	/* Get the tuning data loaded from application to channel device */
	if( pstTerTuneParam->etOffset == DI_CH_OFFSET_1_6_N )
	{
		ulFreqKhz = pstTerTuneParam->ulFrequency - 167 ;
	}
	else if( pstTerTuneParam->etOffset == DI_CH_OFFSET_1_6_P )
	{
		ulFreqKhz = pstTerTuneParam->ulFrequency + 167 ;
	}
	else
	{
		ulFreqKhz = pstTerTuneParam->ulFrequency;
	}

	etBandwidth = pstTerTuneParam->etBandWidth;

	if( etBandwidth == DI_CH_BW_8MHZ )
	{
		nBandwidth = 8;
	}
	else if( etBandwidth == DI_CH_BW_7MHZ )
	{
		nBandwidth = 7;
	}
	else
	{
		nBandwidth = 8;
	}
#if 1 //TODO_ILEE : ASO TEST
	if( ((pstTerTuneParam->etTransMode != DI_CH_TRANSMODE_AUTO) \
		&& (pstTerTuneParam->etGuardInterval != DI_CH_GUARDINTERVAL_AUTO)&&( pstTerTuneParam->etTerSystem == DI_CH_DVBT ))
		|| ((pstTerTuneParam->stT2Param.ulPlpId > 0)&&(pstTerTuneParam->stT2Param.ulPlpId < MAX_PLP_ID)&&( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 ))) /* usePresets */
	{
		CH_PrintFunc(">>>>> 0. usePresets ...\n");
		stTuneParams.bwMHz = nBandwidth;
		stTuneParams.centreFreqkHz = ulFreqKhz;

		if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
		{
			CH_PrintFunc(">>>>> 1. ulPlpId[%d]", pstTerTuneParam->stT2Param.ulPlpId);

			stTuneParams.system = SONY_DVB_SYSTEM_DVBT2;

			stTuneParams.t2Params.dataPLPId = pstTerTuneParam->stT2Param.ulPlpId;

			stTuneParams.t2Params.tuneResult = 0;
#if 0 //sony_dvb_cxd2820_version = "1.04" do not support usePresets mode.
			stTuneParams.t2Params.usePresets = 1;

			stTuneParams.t2Params.presetInfo.mode = pstTerTuneParam->etTransMode - 1;

			stTuneParams.t2Params.presetInfo.gi = pstTerTuneParam->etGuardInterval - 1;

			stTuneParams.t2Params.presetInfo.bwExt = pstTerTuneParam->stT2Param.etExtendedCarrier;

			stTuneParams.t2Params.presetInfo.mixed = pstTerTuneParam->stT2Param.etPreambleMixed;

			stTuneParams.t2Params.presetInfo.numSymbols = pstTerTuneParam->stT2Param.ulPlpNumBlocks;

			stTuneParams.t2Params.presetInfo.papr = pstTerTuneParam->stT2Param.etPaprMode;

			stTuneParams.t2Params.presetInfo.pp = pstTerTuneParam->stT2Param.etPilotPattern -1;

			stTuneParams.t2Params.presetInfo.s1 = pstTerTuneParam->stT2Param.etPreambleFormat;
#endif

		}
		else if( pstTerTuneParam->etTerSystem == DI_CH_DVBT )
		{
			CH_PrintFunc(">>>>> 1. etStream : [%s]", (pstTerTuneParam->etStream ==DI_CH_STREAM_HP)? "HP":"LP");

			stTuneParams.system = SONY_DVB_SYSTEM_DVBT;

			stTuneParams.tParams.usePresets = 1;

			stTuneParams.tParams.profile = pstTerTuneParam->etStream;/* HP or LP */

			stTuneParams.tParams.mode = pstTerTuneParam->etTransMode - 1;

			stTuneParams.tParams.gi = pstTerTuneParam->etGuardInterval - 1;

		}
		else
		{
			stTuneParams.system = SONY_DVB_SYSTEM_DVBT;
			CH_PrintErr(" etTerSystem (%d)", pstTerTuneParam->etTerSystem);
		}

		nRet = dvb_cxd2820_Tune( &s_stCxd2820[nUnitId], &stTuneParams );
		if( nRet != SONY_DVB_OK )
		{
			CH_PrintErr (" dvb_cxd2820_Tune() at %dkHz. (status=%d, %s)", ulFreqKhz, nRet, FormatResult(nRet));
			*bChecklock = FALSE;
			//TODO_ILEE : return DI_CH_ERR;
		}
		else
		{
			if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
			{
				CH_PrintFunc(">>>>> 2. stTuneParams.t2Params.tuneResult[%d]", stTuneParams.t2Params.tuneResult);
			}
			else
			{

			}
		}

	}
	else /* BlindTune */
#endif
	{
		CH_DI_Print(3, (">>>>> BlindTune : nUnitId(%d),etTerSystem(%d), nBandwidth(%d), ulFreqKhz(%d)  \n",nUnitId, pstTerTuneParam->etTerSystem, nBandwidth, ulFreqKhz));

		if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
		{
			nRet = dvb_cxd2820_BlindTune(&s_stCxd2820[nUnitId], SONY_DVB_SYSTEM_DVBT2, ulFreqKhz, nBandwidth);
		}
		else if( pstTerTuneParam->etTerSystem == DI_CH_DVBT )
		{
			nRet = dvb_cxd2820_BlindTune(&s_stCxd2820[nUnitId], SONY_DVB_SYSTEM_DVBT, ulFreqKhz, nBandwidth);
		}
		else
		{
			CH_PrintErr(" etTerSystem (%d)", pstTerTuneParam->etTerSystem);
		}

		if( nRet != SONY_DVB_OK )
		{
			CH_PrintErr (" dvb_cxd2820_BlindTune(%d) at %dkHz. (status=%d, %s)", nUnitId, ulFreqKhz, nRet, FormatResult(nRet));
			*bChecklock = FALSE;
			DRV_CH_UnLockReentrant();
			return DI_CH_ERR;
		}
	}


	if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
	{
		s_stCxd2820[nUnitId].pDemod->system = SONY_DVB_SYSTEM_DVBT2;
	}
	else
	{
		s_stCxd2820[nUnitId].pDemod->system = SONY_DVB_SYSTEM_DVBT;
	}

	/* Monitor carrier offset. */
	nRet = dvb_demod_monitor_CarrierOffset( s_stCxd2820[nUnitId].pDemod, &nOffsetKhz );
	if (nRet != SONY_DVB_OK) {
		CH_PrintErr ("Error: Unable to monitor T2 carrier offset. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
		//TODO_ILEE : return -1 ;
	}

	//CH_DI_Print (3,("DVB-T2 carrier offset detected. Centre=%lukHz, offset=%dkHz, step=%lukHz.\r\n", tuneParams.centreFreqkHz, offsetkHz, stepkHz));


	if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
	{
		CH_PrintFunc ("DVB-T2 signal detected at %dkHz. Offset=%dKHz, step=%luKHz\n", ulFreqKhz, nOffsetKhz, ulStepKhz);
	}
	else
	{
		CH_PrintFunc ("DVB-T2 signal detected at %dkHz. Offset=%dKHz, step=%luKHz\n", ulFreqKhz, nOffsetKhz, ulStepKhz);
	}


	/* Carrier recovery loop locked (demod locked), compensate for the offset and retry tuning. */
	//ulStepKhz = (ulStepKhz + 1)/2 ;
	ulStepKhz = 90;
	if ((HUINT32) abs (nOffsetKhz) > ulStepKhz) {
		/* Tuners have only a fixed frequency step size (stepkHz), */
		/* therefore we must query the tuner driver to get the actual */
		/* centre frequency set by the tuner. */
		if( nOffsetKhz < 0 )
		{
			stTuneParams.centreFreqkHz = ulFreqKhz - 167 ;
			pstTerTuneParam->etOffset = DI_CH_OFFSET_1_6_N;
		}
		else
		{
			stTuneParams.centreFreqkHz = ulFreqKhz + 167 ;
			pstTerTuneParam->etOffset = DI_CH_OFFSET_1_6_P;
		}
		//stTuneParams.centreFreqkHz = ulFreqKhz + nOffsetKhz ;
		stTuneParams.bwMHz = nBandwidth;

		if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
		{
			stTuneParams.system = SONY_DVB_SYSTEM_DVBT2;
			stTuneParams.t2Params.tuneResult = 0;
		}
		else
		{
			stTuneParams.system = SONY_DVB_SYSTEM_DVBT;
			stTuneParams.tParams.usePresets = 0;

			stTuneParams.tParams.profile = pstTerTuneParam->etStream;/* HP or LP */
		}


		CH_PrintFunc ("Re-tuning to compensate offset. New centre=%lukHz.", stTuneParams.centreFreqkHz);
		nRet = dvb_cxd2820_Tune(&s_stCxd2820[nUnitId], &stTuneParams);
		if( nRet != SONY_DVB_OK )
		{
			CH_PrintErr (" dvb_cxd2820_Tune() at %dkHz. (status=%d, %s)", ulFreqKhz, nRet, FormatResult(nRet));
			*bChecklock = FALSE;
			DRV_CH_UnLockReentrant();
			return DI_CH_ERR;
		}
		else
		{
			if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
			{
				CH_PrintFunc(">>>>> 2. stTuneParams.t2Params.tuneResult[%d]", stTuneParams.t2Params.tuneResult);
			}
			else
			{

			}

			/* Monitor carrier offset. */
			nRet = dvb_demod_monitor_CarrierOffset(s_stCxd2820[nUnitId].pDemod, &nOffsetKhz);
			if (nRet != SONY_DVB_OK) {
				CH_PrintErr ("Error: Unable to monitor T2 carrier offset. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
				DRV_CH_UnLockReentrant();
				return -1 ;
			}
			else
			{
				CH_PrintFunc (">>>>>>>>>>> %dkHz. Offset=%dKHz, step=%luKHz\n", ulFreqKhz, nOffsetKhz, ulStepKhz);
			}
		}

	}

	etCurrentState =	GetTuneStatus(nChannelId);
	if( ( etCurrentState == CH_STATUS_IDLE) || (etCurrentState == CH_STATUS_CANCLE) )
	{
		CH_PrintFunc("Cancel locking status...");
		DRV_CH_UnLockReentrant();
		return DI_CH_CANCEL_TUNE;
	}

	/* Wait for TS lock. */
	nResult = dvb_cxd2820_WaitTSLock(&s_stCxd2820[nUnitId]);
	if (nResult != SONY_DVB_OK)
	{
		CH_PrintErr (" dvb_cxd2820_WaitTSLock() at %dkHz. (status=%d, %s)", ulFreqKhz, nResult, FormatResult(nResult));
		*bChecklock = FALSE;
		DRV_CH_UnLockReentrant();
		return DI_CH_ERR;
	}
	*bChecklock = TRUE;

	if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
	{
		CH_PrintFunc (">>>>> DVB-T2 : TS locked at %dkHz.\n", ulFreqKhz);
	}
	else
	{
		CH_PrintFunc (">>>>> DVB-T : TS locked at %dkHz.\n", ulFreqKhz);
	}

	CXD2820_UpdateTunedParam(nChannelId,pstTerTuneParam);

	if( pstTerTuneParam->etTerSystem == DI_CH_DVBT2 )
	{
		CXD2820_MonitorPLP( nUnitId );
	}
	DRV_CH_UnLockReentrant();
	return nResult;
}



HBOOL CXD2820_CheckLock( int nChannelId, HUINT32 ulWorkingId )
{
	//int	nResult = DI_CH_OK;
	int	nRet = SONY_DVB_OK;
	sony_dvb_demod_lock_result_t etLockStatus;
#if defined(MONITOR_CXD2820_PPM)
	int 	nPPM = 0;
#endif
#if defined(MONITOR_CXD2820_ERROR_STATE)
	HUINT32 strength_indicator,quality_indicator;
#endif
	int			nUnitId = 0;

	CH_UNUSED(ulWorkingId);

	DRV_CH_LockReentrant();

	nUnitId = GetDeviceUnitId( nChannelId );

	nRet = dvb_demod_CheckDemodLock( &demod[nUnitId], &etLockStatus);
	if (nRet != SONY_DVB_OK) {
		CH_PrintErr ("(status=%d, %s)", nRet, FormatResult(nRet));
		DRV_CH_UnLockReentrant();
		return FALSE;
	}
	else
	{
		if( etLockStatus != DEMOD_LOCK_RESULT_LOCKED )
		{
			if(etLockStatus == DEMOD_LOCK_RESULT_NOTDETECT )
			{
				CH_PrintFunc("\t\t DEMOD[%d] :>>>>>>>>>>>>>>>>>>>>>>>DEMOD_LOCK_RESULT_NOTDETECT", nUnitId);
				DRV_CH_UnLockReentrant();
				return FALSE;
			}
			else
			{
				CH_PrintFunc("DEMOD[%d] : etLockStatus(%d)", nUnitId, etLockStatus);
				DRV_CH_UnLockReentrant();
				return FALSE;
			}

		}
		else
		{
 			// for test only
#if defined(MONITOR_CXD2820_ERROR_STATE)
			CXD2820_CheckAgcBer( nChannelId,&strength_indicator,&quality_indicator);
#endif

#if defined(MONITOR_CXD2820_PPM)
			if(demod[nUnitId].system == SONY_DVB_SYSTEM_DVBT)
			{
				dvb_demod_monitorT_SamplingOffset(&demod[nUnitId],&nPPM);
			}
			else if(demod[nUnitId].system == SONY_DVB_SYSTEM_DVBT2)
			{
				dvb_demod_monitorT2_SamplingOffset(&demod[nUnitId],&nPPM);
			}
			else
			{
				CH_DI_Print(0, ( "Error : Unknown system(%d) : demod[%d]\n", demod[nUnitId].system, nUnitId));
			}

			CH_DI_Print(0, ( "DEMOD[%d]:[%d]:[%s] (nPPM : system)\n", nUnitId, nPPM, ((demod[nUnitId].system == SONY_DVB_SYSTEM_DVBT2) ? "DVBT2":"DVBT")));
#endif
		}

	}

	DRV_CH_UnLockReentrant();
	return TRUE;
}



int CXD2820_GetSignalInfo( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	int	nResult = DI_CH_OK;
	HUINT32 strength = 0, quality = 0;

	CXD2820_CheckAgcBer(nChannelId,&strength,&quality);

	pstSignalInfo->ulStrength = strength;
	pstSignalInfo->ulQuality = quality;

	return nResult;
}



int CXD2820_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	int	nResult = DI_CH_OK;

	switch (etPower)
	{
		case CH_POWER_OFF:  //tmPowerStandby = 1
			SetActiveStatus( nChannelId, CH_ACTION_OFF );
			CH_DI_Print(5, ("\n -------------CH_T_SetPowerState standby mode -------------\n"));
			break;

		case CH_POWER_ON: //tmPowerOn = 0
			SetActiveStatus( nChannelId, CH_ACTION_ON );
			CH_DI_Print(5, ("\n -------------CH_T_SetPowerState Power on mode -------------\n"));
			break;
	}

	return nResult;
}



int CXD2820_EnableTsOutput( int nChannelId )
{
	int	nResult = DI_CH_OK;
	CH_UNUSED(nChannelId);
	return nResult;
}



int CXD2820_DisableTsOutput( int nChannelId )
{
	int	nResult = DI_CH_OK;
	int nRet = DI_CH_OK;
	int			nUnitId = 0;
	nUnitId = GetDeviceUnitId( nChannelId );

	nRet = dvb_demod_Sleep( &demod[nUnitId] );
	if( nRet != DI_CH_OK )
	{
		CH_PrintErr(" ");
		nResult = DI_CH_ERR;
	}

	return nResult;
}



DI_CH_AntStatus_e CXD2820_GetAntState( int nChannelId )
{
	int nUnitId;

	nUnitId = GetDeviceUnitId( nChannelId );

	return s_stAntState[nUnitId].etCurrentAntState;

}



#if defined(CONFIG_COMMON_SHORTCIRCUIT)  /* HDR-FOX T2 */
int CXD2820_SetAntState( int nChannelId, DI_CH_AntStatus_e etAntState )
{
	int nUnitId;
	int nRet=0;
	HUINT32 unWorkingId;
	int nActiveState;

	nUnitId = GetDeviceUnitId( nChannelId );
	unWorkingId = GetRequestedId( nChannelId );

	if( nUnitId != MASTER_CHANNEL )
	{
		return DI_CH_HW_RESTRICTION;
	}

	s_stAntState[nUnitId].etDesiredAntState = etAntState;

	if( s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANTENNA_FAULT )
	{
		if(etAntState < DI_CH_ANTENNA_FAULT)
		{
			if((etAntState != DI_CH_ANTENNA_OFF) && s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANTENNA_ON)
			{
				CH_DI_Print(4, ("CXD2820_SetAntState (ON)\n"));

				/* Power On */
				/* Menu Power ON 시 ANT_PW_EN: LOW, 	ANT_EN : 한번만 3ms 동안 High 유지  */
				nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_PWR_EN, GPIO_HI);
				VK_TASK_Sleep(2);

				nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_EN, GPIO_HI);
				VK_TASK_Sleep(1);
				nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_EN, GPIO_LOW);
			}
			else
			{
				CH_DI_Print(4, ("CXD2820_SetAntState (OFF)\n"));
				/* Power Off */
				/* Menu Power Off 시 ANT_PW_EN: High  ,  ANT_EN : Low */
				nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_PWR_EN, GPIO_LOW);
				nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_EN, GPIO_LOW);
			}
			if( nRet < 0 )
			{
				CH_DI_Print(0, ("CXD2820_SetAntState Write fail\n"));
				return DI_CH_ERR;
			}
		}
		else
		{
			CH_DI_Print(0, ("CXD2820_SetAntState Unknown State\n"));
		}
		s_stAntState[nUnitId].etCurrentAntState = etAntState;

	}
	else if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANTENNA_FAULT )
	{

		if( etAntState == DI_CH_ANTENNA_OFF )
		{
			/* Power Off */
			nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_PWR_EN, GPIO_LOW);
			nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_EN, GPIO_LOW);
			if( nRet < 0 )
			{
				CH_DI_Print(0, ("CXD2820_SetAntState Write fail\n"));
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



int CXD2820_CheckAntState( int nChannelId )
{
	int nUnitId;
	int nRet=0;
	int nGpioVal = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( nUnitId != MASTER_CHANNEL )
	{
		return DI_CH_HW_RESTRICTION;
	}

	if( s_stAntState[nUnitId].etDesiredAntState == DI_CH_ANTENNA_ON )
	{
		nRet = dvb_demod_GPIORead(&demod[nUnitId], GPIO_ANT_PWR_CHK, &nGpioVal);
		if(nRet < 0 )
		{
			CH_DI_Print(0, ("CXD2820_CheckAntState GPIO Read Error\n"));
			return DI_CH_ERR;
		}

		if( nGpioVal == GPIO_LOW )
		{
			CH_DI_Print(5, (">>>>>>>>>>>>Antenna short circuit\n"));
			s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_FAULT;
			return DI_CH_ANTENNA_FAULT;
		}
		else if( nGpioVal == GPIO_HI)
		{
			if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANTENNA_FAULT )
			{
				CH_DI_Print(5, ("CXD2820_CheckAntState CHK(HIGH), Curr(FAULT)\n"));
				/* Power On */
				nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_PWR_EN, GPIO_HI);
				VK_TASK_Sleep(2);

				nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_EN, GPIO_HI);
				nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_EN, GPIO_LOW);

				if( nRet < 0 )
				{
					CH_DI_Print(0, ("CXD2820_CheckAntState Gpio Write Error\n"));
				}
			}
			return DI_CH_OK;

		}
		else
		{
			CH_DI_Print(0, ("CXD2820_CheckAntState\n"));
			return DI_CH_ERR;
		}
	}
	else // DI_CH_ANTENNA_OFF, DI_CH_ANTENNA_FAULT
	{
		return DI_CH_OK;
	}
}



void CXD2820_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId )
{
	int nRet=0;
	int nUnitId;
	int nActiveState;
	int nGpioVal = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( nUnitId != MASTER_CHANNEL )
	{
		return;
	}

	CH_DI_Print(5, ("CXD2820_ProcessRecoverAntenna\n"));
	nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_EN, GPIO_HI);
	VK_TASK_Sleep(1);
	nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_EN, GPIO_LOW);
	if( nRet < 0 )
	{
		CH_DI_Print(0, ("CXD2820_ProcessRecoverAntenna Write HI fail\n"));

	}

	VK_TASK_Sleep(100);// wait 100ms

	nRet = dvb_demod_GPIORead(&demod[nUnitId], GPIO_ANT_PWR_CHK, &nGpioVal);
	if( nRet < 0 )
	{
		CH_DI_Print(0, ("CXD2820_ProcessRecoverAntenna Read fail\n"));
		return;
	}

	if( nGpioVal == GPIO_LOW)
	{
		CH_DI_Print(5, ("Antenna continue fault... Turn 5 Volt Off\n"));
		/* Power Off */

		nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_EN, GPIO_LOW);
		if( nRet < 0 )
		{
			CH_DI_Print(0, ("CXD2820_ProcessRecoverAntenna Write LOW fail\n"));
		}

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT);
		//s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_OFF;

	}
	else if( nGpioVal == GPIO_HI )
	{
		CH_DI_Print(5, ("Antenna recovered...\n"));

		s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_ON;

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
		CH_DI_Print(0, ("CXD2820_ProcessRecoverAntenna Read Unknown (%d)\n", nGpioVal));
	}

	return;

}



#else /* HD-FOX T2 */



int CXD2820_SetAntState( int nChannelId, DI_CH_AntStatus_e etAntState)
{
	int nUnitId;
	int nRet = DI_CH_OK;
	HUINT32 unWorkingId;
	int nActiveState;

	DRV_CH_Lock( nChannelId );

	CH_PrintFunc("[%s]", (etAntState == DI_CH_ANTENNA_OFF)? "Off":"On");

	nUnitId = GetDeviceUnitId( nChannelId );
	unWorkingId = GetRequestedId( nChannelId );

	s_stAntState[nUnitId].etDesiredAntState = etAntState;

	if( s_stAntState[nUnitId].etCurrentAntState != DI_CH_ANTENNA_FAULT )
	{
		if(etAntState < DI_CH_ANTENNA_FAULT)
		{
			nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_PWR_EN, etAntState);
			if( nRet != DI_CH_OK )
			{
				CH_PrintErr ("dvb_demod_GPIOWrite. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
				DRV_CH_UnLock( nChannelId );
				return DI_CH_ERR;
			}
		}
		else
		{
			CH_PrintErr("");
		}
		s_stAntState[nUnitId].etCurrentAntState = etAntState;

	}
	else if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANTENNA_FAULT )
	{

		if( etAntState == DI_CH_ANTENNA_OFF )
		{
			nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_PWR_EN, etAntState);
			if( nRet != DI_CH_OK )
			{
				CH_PrintErr ("dvb_demod_GPIOWrite. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
				DRV_CH_UnLock( nChannelId );
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
			DRV_CH_UnLock( nChannelId );
			return DI_CH_HW_RESTRICTION;
		}
	}

	DRV_CH_UnLock( nChannelId );
	return DI_CH_OK;

}



int CXD2820_CheckAntState( int nChannelId )
{
	int nGpioVal = 0;
	int nUnitId = 0;
	int nRet = DI_CH_OK;

	DRV_CH_Lock( nChannelId );

	nUnitId = GetDeviceUnitId( nChannelId );

	//CH_PrintFunc("etDesiredAntState[%d]:[%d]etCurrentAntState\n", s_stAntState[nUnitId].etDesiredAntState, s_stAntState[nUnitId].etCurrentAntState);

	if( s_stAntState[nUnitId].etDesiredAntState == DI_CH_ANTENNA_ON )
	{
		nRet = dvb_demod_GPIORead(&demod[nUnitId], GPIO_ANT_PWR_CHK, &nGpioVal);
	    if ( nRet != DI_CH_OK)
	    {
	        CH_PrintErr ("Error: dvb_demod_GPIORead. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
	    }

		if( nGpioVal == GPIO_LOW )
		{
			CH_PrintFunc(">>>>>>>>>>>>Antenna short circuit\n");
			s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_FAULT;
			DRV_CH_UnLock( nChannelId );
			return DI_CH_ANTENNA_FAULT;
		}
		else if( nGpioVal == GPIO_HI )
		{
			if( s_stAntState[nUnitId].etCurrentAntState == DI_CH_ANTENNA_FAULT )
			{
				nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_PWR_EN, GPIO_HI);
				if( nRet != DI_CH_OK )
				{
					CH_PrintErr ("Error: dvb_demod_GPIOWrite. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
				}
			}

			DRV_CH_UnLock( nChannelId );
			return DI_CH_OK;

		}
		else
		{
			CH_PrintErr(" ");
			DRV_CH_UnLock( nChannelId );
			return DI_CH_ERR;
		}
	}
	else // DI_CH_ANTENNA_OFF, DI_CH_ANTENNA_FAULT
	{
		DRV_CH_UnLock( nChannelId );
		return DI_CH_OK;
	}

}



void CXD2820_ProcessRecoverAntenna( int nChannelId , HUINT32 unWorkingId)
{
	int nRet=0;
	int nUnitId;
	int nActiveState;
	int nGpioVal = 0;

	DRV_CH_Lock( nChannelId );

	nUnitId = GetDeviceUnitId( nChannelId );

	nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_PWR_EN, GPIO_HI);
	if( nRet != DI_CH_OK )
	{
		CH_PrintErr ("dvb_demod_GPIOWrite. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
	}

	VK_TASK_Sleep(100);// wait 100ms

	nRet = dvb_demod_GPIORead(&demod[nUnitId], GPIO_ANT_PWR_CHK, &nGpioVal);
	if( nRet != DI_CH_OK )
	{
		CH_PrintErr ("dvb_demod_GPIORead. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
	}

	if( nGpioVal == GPIO_LOW )
	{
		CH_PrintFunc("Antenna continue fault... Turn 5 Volt Off");

		nRet = dvb_demod_GPIOWrite(&demod[nUnitId], GPIO_ANT_PWR_EN, GPIO_LOW);
		if( nRet != DI_CH_OK )
		{
			CH_PrintErr ("dvb_demod_GPIOWrite. (status=%d, %s)\r\n", nRet, FormatResult(nRet));
		}

		DRV_CH_CallNotifyCallback( nChannelId, unWorkingId, DI_CH_SIGNAL_ANTENNA_FAULT);
		//s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_OFF;

	}
	else if( nGpioVal == GPIO_HI )
	{
		CH_PrintFunc("Antenna recovered...");

		s_stAntState[nUnitId].etCurrentAntState = DI_CH_ANTENNA_ON;

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
		CH_PrintErr("");
	}

	DRV_CH_UnLock( nChannelId );
	return;
}
#endif



void DummyCXD2820_ResetGpio(int nChannelId)
{
	CH_UNUSED(nChannelId);
}



void DummyCXD2820_Initialize(int nChannelId)
{
	CH_UNUSED(nChannelId);
}



int CXD2820_GetPlpCount( int nChannelId, HUINT8 * pucNumPlps )
{
	int nResult = DI_CH_OK;
	int			nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_PrintData("DEMOD[%d] : s_ucNumberPlp : %d\n", nUnitId, s_ucNumberPlp[nUnitId]);
	*pucNumPlps = s_ucNumberPlp[nUnitId];

	return nResult;
}


int CXD2820_GetPlpInfo( int nChannelId, HUINT8 * pucPlpIds, HUINT8 * pucNumPlps )
{
	int nResult = DI_CH_OK;
	int			nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( s_ucNumberPlp[nUnitId] < MAX_NUM_PLPS )
	{
		VK_MEM_Memcpy( pucPlpIds, s_aucPlpID[nUnitId], s_ucNumberPlp[nUnitId] );
		*pucNumPlps = s_ucNumberPlp[nUnitId];
	}
	else
	{
		CH_PrintErr(" s_ucNumberPlp (%d) ", s_ucNumberPlp[nUnitId]);
		nResult = DI_CH_ERR;
	}

	return nResult;
}
/*********************** End of File ******************************/


