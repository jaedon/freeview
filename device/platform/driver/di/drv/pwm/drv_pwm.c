/**
 * drv_pwm.c
*/

/**
 * @defgroup		DRV_PWM DRV_PWM : Pwm driver module
 * @author		Jin-hong Kim
 * @note			PWM driver APIs
 * @brief			Define broadcom dependent PWM Driver
 * @file			drv_pwm.c

*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include common utils headers */
#include "linden_trace.h"

/* include drv headers */
#include "drv_pwm.h"

/* include bcm nexus headers */
#include "nexus_pwm_init.h"
#include "nexus_pwm.h"



/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

#define DEBUG_MODULE TRACE_MODULE_DRV_PWM

#define DRV_PWM_MAX_CHANNEL_NUMBER	2 /* */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
#if 0
static NEXUS_ModuleHandle		s_PwmHandle = NULL;
#endif
static NEXUS_PwmChannelHandle	s_PwmChannelHandle[DRV_PWM_MAX_CHANNEL_NUMBER];


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

/**
 * @brief			Initialize PWM Module
 * @return		DRV_OK : ok\n
 *				DRV_ERR: fail\n
*/
DRV_Error DRV_PWM_Init(HUINT32 ulChannelNumber)
{
#if 0
	NEXUS_PwmModuleSettings		stPwmSettings;
#endif
	NEXUS_PwmChannelSettings	stPwmChnDefSettings;

	PrintEnter();

#if 0
	s_PwmHandle = NEXUS_PwmModule_Init(&stPwmSettings);
	if(s_PwmHandle == NULL)
	{
		TraceError(TRACE_MODULE_DRV_PWM, "%s->NEXUS_PwmModule_Init(): Error Null handle! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		return DRV_ERR;
	}
#endif

	if(ulChannelNumber >= DRV_PWM_MAX_CHANNEL_NUMBER)
	{
		return DRV_ERR;
	}

	/* Initialize Channel handle */
#if (NEXUS_VERSION < 750)
	s_PwmChannelHandle[ulChannelNumber]  = NULL;
	NEXUS_Pwm_GetDefaultChannelSettings(ulChannelNumber, &stPwmChnDefSettings);
	stPwmChnDefSettings.eChannelNumber = ulChannelNumber;
	stPwmChnDefSettings.bopenDrain = false;
	stPwmChnDefSettings.eFreqMode = NEXUS_PwmFreqModeType_eConstant;
	s_PwmChannelHandle[ulChannelNumber] = NEXUS_Pwm_OpenChannel(&stPwmChnDefSettings);
	if(s_PwmChannelHandle[ulChannelNumber] == NULL)
	{
		PrintError("%s->NEXUS_Pwm_OpenChannel(): Error Null handle! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug( "%s->NEXUS_Pwm_Start(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#else
	s_PwmChannelHandle[ulChannelNumber]  = NULL;
	NEXUS_Pwm_GetDefaultChannelSettings(&stPwmChnDefSettings);
	stPwmChnDefSettings.openDrain = false;
	stPwmChnDefSettings.eFreqMode = NEXUS_PwmFreqModeType_eConstant;
	s_PwmChannelHandle[ulChannelNumber] = NEXUS_Pwm_OpenChannel(ulChannelNumber,&stPwmChnDefSettings);
	if(s_PwmChannelHandle[ulChannelNumber] == NULL)
	{
		PrintError("%s->NEXUS_Pwm_OpenChannel(): Error Null handle! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Pwm_Start(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#endif
	PrintExit();

	return DRV_OK;
}

/**
 * @brief			Start PWM Control
 * @param[in]		ulChannelNumber	: PWM channel number
 * @return		DRV_OK : ok\n
 *				DRV_ERR: fail\n
*/
DRV_Error DRV_PWM_Start(HUINT32 ulChannelNumber)
{
	NEXUS_Error	eNexusError = 0;

	PrintEnter();

	eNexusError = NEXUS_Pwm_Start(s_PwmChannelHandle[ulChannelNumber]);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Pwm_Start(): eNexusError(0x%08x) Line(%d), %s\n", __FUNCTION__, eNexusError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Pwm_Start(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		return DRV_OK;
	}

}


/**
 * @brief			Stop PWM Control
 * @param[in]		ulChannelNumber	: PWM channel number
 * @return		DRV_OK : ok\n
 *				DRV_ERR: fail\n
*/
DRV_Error DRV_PWM_Stop(HUINT32 ulChannelNumber)
{
	NEXUS_Error eNexusError = 0;


	PrintEnter();


	eNexusError = NEXUS_Pwm_Stop(s_PwmChannelHandle[ulChannelNumber]);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Pwm_Stop(): eNexusError(0x%08x) Line(%d), %s\n", __FUNCTION__, eNexusError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Pwm_Stop(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		return DRV_OK;
	}
}

/**
 * @brief			Get PWM control word
 * @param[in]		ulChannelNumber	: PWM channel number
 * @param[out]	pulControlWord	: control word
 * @return		DRV_OK : ok\n
 *				DRV_ERR: fail\n
*/

DRV_Error DRV_PWM_GetControlWord(HUINT32 ulChannelNumber, HUINT32 *pulControlWord)
{
	NEXUS_Error eNexusError = 0;

	PrintEnter();

	eNexusError = NEXUS_Pwm_GetControlWord(s_PwmChannelHandle[ulChannelNumber], (uint16_t *)pulControlWord);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Pwm_SetControlWord(): eNexusError(0x%08x) Line(%d), %s\n", __FUNCTION__, eNexusError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Pwm_SetControlWord(): pulControlWord(%d) OK! Line(%d), %s\n", __FUNCTION__, *pulControlWord, __LINE__, __FILE__);
		return DRV_OK;
	}
}


/**
 * @brief			Set PWM control word
 * @param[in]		ulChannelNumber	: PWM channel number
 * @param[in]		ulControlWord		: control word
 * @return		DRV_OK : ok\n
 *				DRV_ERR: fail\n
*/
DRV_Error DRV_PWM_SetControlWord(HUINT32 ulChannelNumber, HUINT32 ulControlWord)
{
	NEXUS_Error eNexusError = 0;

	PrintEnter();

	eNexusError = NEXUS_Pwm_SetControlWord(s_PwmChannelHandle[ulChannelNumber], (uint16_t)ulControlWord);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Pwm_SetControlWord(): eNexusError(0x%08x) Line(%d), %s\n", __FUNCTION__, eNexusError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Pwm_SetControlWord(): ulControlWord(%d) OK! Line(%d), %s\n", __FUNCTION__, ulControlWord, __LINE__, __FILE__);
		return DRV_OK;
	}
}

/**
 * @brief			Get PWM Period Intervall
 * @param[in]		ulChannelNumber	: PWM channel number
 * @param[out]	*pulPeriodInterval	: period interval
 * @return		DRV_OK : ok\n
 *				DRV_ERR: fail\n
*/
DRV_Error DRV_PWM_GetPeriodInterval(HUINT32 ulChannelNumber, HUINT32 *pulPeriodInterval)
{
	NEXUS_Error eNexusError = 0;

	PrintEnter();

	eNexusError = NEXUS_Pwm_GetPeriodInterval(s_PwmChannelHandle[ulChannelNumber], (uint16_t *)pulPeriodInterval);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Pwm_SetPeriodInterval(): eNexusError(0x%08x) Line(%d), %s\n", __FUNCTION__, eNexusError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Pwm_SetPeriodInterval(): pulPeriodInterval(%d) OK! Line(%d), %s\n", __FUNCTION__, *pulPeriodInterval, __LINE__, __FILE__);
		return DRV_OK;
	}
}

/**
 * @brief			Set PWM Period Intervall
 * @param[in]		ulChannelNumber	: PWM channel number\n
				ulPeriodInterval	: period interval\n
 * @return		DRV_OK : ok\n
 *				DRV_ERR: fail\n
*/
DRV_Error DRV_PWM_SetPeriodInterval(HUINT32 ulChannelNumber, HUINT32 ulPeriodInterval)
{
	NEXUS_Error eNexusError = 0;

	PrintEnter();

	eNexusError = NEXUS_Pwm_SetPeriodInterval(s_PwmChannelHandle[ulChannelNumber], (uint16_t)ulPeriodInterval);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Pwm_SetPeriodInterval(): eNexusError(0x%08x) Line(%d), %s\n", __FUNCTION__, eNexusError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Pwm_SetPeriodInterval(): ulPeriodInterval(%d) OK! Line(%d), %s\n", __FUNCTION__, ulPeriodInterval, __LINE__, __FILE__);
		return DRV_OK;
	}
}

/**
 * @brief			Get PWM On Intervall
 * @param[in]		ulChannelNumber	: PWM channel number
 * @param[out]	*pulOnInterval	: on interval
 * @return		DRV_OK : ok\n
 *				DRV_ERR: fail\n
*/
DRV_Error DRV_PWM_GetOnInterval(HUINT32 ulChannelNumber, HUINT32 *pulOnInterval)
{
	NEXUS_Error eNexusError = 0;

	PrintEnter();

	eNexusError = NEXUS_Pwm_GetOnInterval(s_PwmChannelHandle[ulChannelNumber], (uint16_t *)pulOnInterval);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->DRV_PWM_SetOnInterval(): eNexusError(0x%08x) Line(%d), %s\n", __FUNCTION__, eNexusError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->DRV_PWM_SetOnInterval(): ulOnInterval(%d) OK! Line(%d), %s\n", __FUNCTION__, *pulOnInterval, __LINE__, __FILE__);
		return DRV_OK;
	}
}

/**
 * @brief			Set PWM On Intervall
 * @param[in]		ulChannelNumber	: PWM channel number
 * @param[in]		ulOnInterval	: on interval
 * @return		DRV_OK : ok\n
 *				DRV_ERR: fail\n
*/
DRV_Error DRV_PWM_SetOnInterval(HUINT32 ulChannelNumber, HUINT32 ulOnInterval)
{
	NEXUS_Error eNexusError = 0;

	PrintEnter();

	eNexusError = NEXUS_Pwm_SetOnInterval(s_PwmChannelHandle[ulChannelNumber], (uint16_t)ulOnInterval);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->DRV_PWM_SetOnInterval(): eNexusError(0x%08x) Line(%d), %s\n", __FUNCTION__, eNexusError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->DRV_PWM_SetOnInterval(): ulOnInterval(%d) OK! Line(%d), %s\n", __FUNCTION__, ulOnInterval, __LINE__, __FILE__);
		return DRV_OK;
	}
}


