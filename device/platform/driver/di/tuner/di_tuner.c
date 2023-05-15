/**
 * di_tuner.c
*/

/**
 * @defgroup		DI_TUNER DI_TUNER : DI_TUNER module
 * @author
 * @note			DI_TUNER APIs
 * @brief			Define DI_TUNER APIs
 * @file 			di_tuner.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils */
#include "linden_trace.h"

/* include  drv headers */

/* include  di headers */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif

#include "nexus_frontend.h"
#include "nexus_platform.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/



static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendSatelliteStatus status;

    param=param;    /* unused */

    VK_fprintf(VK_STDERR, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);
    VK_printf(">>>>>>>Lock callback, frontend 0x%08x\n", (unsigned)frontend);

    NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
    VK_fprintf(VK_STDERR, "Lock callback - state %d\n", status.demodLocked);
    VK_printf(">>>>>>>Lock callback - state %d\n", status.demodLocked);
}

static void GetSatelliteStatus(NEXUS_FrontendHandle frontend)
{
	NEXUS_FrontendSatelliteStatus status;

	NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
	VK_printf("\t\t status.tunerLocked (%d)\n",status.tunerLocked);
	VK_printf("\t\t status.demodLocked (%d)\n",status.demodLocked);
	VK_printf("\t\t status.bertLocked (%d)\n",status.bertLocked);
}


void TEST_TUNER_Tune(HUINT32 ulTuner, HUINT32 ulMode, HUINT32 ulFrequency, HUINT32 ulSymbolRate, HUINT32 ulCoderate, HUINT32 ulRollOff)
{
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendDiseqcSettings diseqcSettings;
	NEXUS_FrontendSatelliteStatus pStatus;
	HUINT32 i;

	HUINT8	ucDiseqcMsg[4];


    NEXUS_Platform_GetConfiguration(&platformConfig);

		for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
		{
			NEXUS_FrontendCapabilities capabilities;
			frontend = platformConfig.frontend[i];
			if (frontend) {
				NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
				/* Does this frontend support satellite? */
				if ( capabilities.satellite )
				{
					VK_printf("(%d th front end support satellite!!\n", i);
					break;
				}
			}
		}

		if (NULL == frontend )
		{
			VK_fprintf(VK_STDERR, "Unable to find Satellite-capable frontend\n");
			return 0;
		}

		NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
		satSettings.frequency = ulFrequency*1000*1000;
		satSettings.symbolRate = ulSymbolRate*1000;
		satSettings.mode = NEXUS_FrontendSatelliteMode_eDvb;
	//	satSettings.codeRate.numerator = 0;
	//	satSettings.codeRate.denominator = 0;
	//	satSettings.codeRate.bitsPerSymbol = 0;
		satSettings.lockCallback.callback = lock_callback;
		satSettings.lockCallback.context = frontend;
		NEXUS_Frontend_GetUserParameters(frontend, &userParams);

		NEXUS_Frontend_GetDiseqcSettings(frontend, &diseqcSettings);
#if 0
		diseqcSettings.toneEnabled = true;
		diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
#else
		diseqcSettings.toneEnabled = false;
		diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e18v;
#endif
		NEXUS_Frontend_SetDiseqcSettings(frontend, &diseqcSettings);

		for(i=0;i<4;i++)
		{
			ucDiseqcMsg[i] = 0;
		}

		ucDiseqcMsg[1]	= 0x10; // Address Byte
		ucDiseqcMsg[2]	= 0x38; // Command Byte
		ucDiseqcMsg[3]	= 0xF0;
//		if (g_stDiseqcParam.etSwitchOption == DDI_TN_SWITCH_OPTION_B)
//		ucDiseqcMsg[3] |= 0x08;
//		if (g_stDiseqcParam.etSatellitePosition == DDI_TN_SATELLITE_POSITION_B)
		ucDiseqcMsg[3] |= 0x04;
//		if (ucPolar == DDI_TN_SATELLITE_POL_LINEAR_HOR)
		ucDiseqcMsg[3] |= 0x02;
//		if (g_stDiseqcParam.uc22KhzTone == DDI_TN_22KHzTone_ON)
		ucDiseqcMsg[3] |= 0x01;

		NEXUS_Frontend_SendDiseqcMessage(frontend, ucDiseqcMsg, 4, NULL);

		NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
		BKNI_Sleep(1000);

		GetSatelliteStatus(frontend);

}



