/**
 * di_pannel.c
*/

/**
 * @defgroup		DI_PANNEL : PANNEL(LCD, VFD, 7Segment) device interface
 * @author			Kyu-Young Cho
 * @note			Pannel API file
 * @brief			Pannel Display apis
 * @file 			di_pannel.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "linden_trace.h"

#include "vkernel.h"
#include "di_front_panel.h"
#include "drv_err.h"
#include "drv_micom.h"
#if defined(CONFIG_STLED)
#include "drv_stled.h"
#endif
#if defined(CONFIG_PANEL_LED_PWM)
#include "nexus_pwm_init.h"
#include "nexus_pwm.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_LED

#if	defined(CONFIG_PANEL_7SEG)
#define FRONT_PANEL_TYPE DI_FRONT_PANEL_TYPE_7SEG
#define 	MAX_PANEL_LENGTH 			(4)		// 7-Segment display length max
#elif	defined(CONFIG_PANEL_11SEG)
#define FRONT_PANEL_TYPE DI_FRONT_PANEL_TYPE_11SEG
#define 	MAX_PANEL_LENGTH 			(5)		// 11-Segment display length max
#elif	defined(CONFIG_PANEL_LCD)
#define FRONT_PANEL_TYPE DI_FRONT_PANEL_TYPE_LCD
#define 	MAX_PANEL_LENGTH 			(16)		// LCD display length max
#elif	defined(CONFIG_PANEL_VFD)
#define FRONT_PANEL_TYPE DI_FRONT_PANEL_TYPE_VFD
#define MAX_PANEL_LENGTH CONFIG_PANEL_VFD_LENGTH 		/*  VFD display length max (default = 16) */
#else
#define FRONT_PANEL_TYPE DI_FRONT_PANEL_TYPE_NONE
#define 	MAX_PANEL_LENGTH 			(0)
#endif


#if	defined(CONFIG_PANEL_LCD) || defined(CONFIG_PANEL_VFD)
#define	MICOM_DIM_LEVEL_MAX (255)
#endif
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
HBOOL g_FrontTestFlag = FALSE;
#endif
#if defined(CONFIG_PANEL_LED_PWM)
static NEXUS_PwmChannelHandle PwmHandle;
#endif

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#if	defined(CONFIG_PANEL_LCD) || defined(CONFIG_PANEL_VFD)
unsigned char P_FRONT_PANEL_TransLevel(HUINT8 ucLevel)
{
	unsigned char ucTransLevel = 0;

	/*
		ucLevel		ucTransLevel
		~~~~~~      ~~~~~~~~~
		100			255
		99			252
		98			249
		...			...
		51			108
		50			105
		49			103
		...			...
		2			9
		1			7
		0			0
	*/
	if (ucLevel >= DIM_LEVEL_MAX)
		ucTransLevel = MICOM_DIM_LEVEL_MAX;
	else if (ucLevel == 0)
		ucTransLevel = 0;
	else if(ucLevel >= (DIM_LEVEL_MAX/2) )		// >50 && <100
		ucTransLevel = MICOM_DIM_LEVEL_MAX - 3*(DIM_LEVEL_MAX-ucLevel);
	else	// >0 && <50
		ucTransLevel = ucLevel*2+5;

	return ucTransLevel;
}
#endif

DI_ERR_CODE DI_FRONT_PANEL_GetCapability(DI_FRONT_PANEL_CAP *pstCapability)
{
	pstCapability->max_length = MAX_PANEL_LENGTH;
	pstCapability->type       = FRONT_PANEL_TYPE;

#if defined(CONFIG_PANEL_DISPLAY_ON)
	pstCapability->displayOnOff = TRUE;
#else
	pstCapability->displayOnOff = FALSE;
#endif

#if defined(CONFIG_PANEL_DIMMING_LEVEL_ON)
	pstCapability->dimmingLevelOnOff = TRUE;
#else
	pstCapability->dimmingLevelOnOff = FALSE;
#endif

	PrintDebug("displayOnOff = %s\n", pstCapability->displayOnOff ? "TRUE" : "FALSE");
	PrintDebug("dimmingLevelOnOff = %s\n", pstCapability->dimmingLevelOnOff ? "TRUE" : "FALSE");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FRONT_PANEL_Display (HUINT8 *pstString)
{
	unsigned char ucStr[MAX_PANEL_LENGTH + 1] = {0, };
	HUINT32 ulStrLen = 0;
#if defined(CONFIG_STLED)
	int nIsTime = 0;
#endif

	ulStrLen = VK_strlen((char*) pstString);
	if(ulStrLen > MAX_PANEL_LENGTH)
	{
		VK_memcpy(ucStr, pstString, MAX_PANEL_LENGTH);
		ucStr[MAX_PANEL_LENGTH] = 0x00;
	}
	else
	{
		VK_strncpy((char*)ucStr, (char*)pstString, ulStrLen);
		ucStr[ulStrLen] = 0x00;
	}

#if defined(CONFIG_FRONT_MICOM_TOUCH) && defined(CONFIG_DEBUG)
	if( g_FrontTestFlag == TRUE )
		return DI_ERR_OK;
#endif

	if(DRV_MICOM_WriteString(ucStr, VK_strlen((const char *)ucStr)) != DRV_OK)
	{
		PrintError("DI_PANNEL_Display(), DRV_MICOM_WriteStrLcd() returns error.\n");
		return DI_ERR_ERROR;
	}

#if defined(CONFIG_STLED)
	nIsTime = 0;
	if(DRV_STLED_WriteString(ucStr, VK_strlen((const char *)ucStr), nIsTime)
			!= DRV_OK)
	{
		PrintError("DI_FRONT_PANEL_Display(), DRV_STLED_WriteString() returns "
			   "error.\n");
		return DI_ERR_ERROR;
	}
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FRONT_PANEL_SetTimeDisplay(HBOOL bOn_nOff)
{
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_DisplayTime((unsigned char)bOn_nOff);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_PANNEL_DisplayTime() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}

	PrintDebug("[%s] PANNEL_DisplayTime is %s \r\n", __func__, bOn_nOff?"ON":"OFF");

	return DI_ERR_OK;
}

#if	defined(CONFIG_FRONT_MICOM_JAPAN)
DI_ERR_CODE DI_FRONT_PANEL_SetDimmer(HBOOL bOn_Off)
{
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_SetDimmer(bOn_Off);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_PANNEL_SetDimmer() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}

	PrintDebug("[%s] DI_PANNEL_SetDimmer is %s \r\n", __func__, bOn_Off?"ON":"OFF");

	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_FRONT_PANEL_SetPanelPower(HBOOL bOn_nOff)
{
#if	defined(CONFIG_PANEL_LCD) || defined (CONFIG_PANEL_VFD)
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_SetLCDPower(bOn_nOff);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_SetLCDPower() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}

	PrintDebug("[%s] LCD Power is %s\n", __func__, bOn_nOff?"ON":"OFF");
#else
	bOn_nOff = bOn_nOff;
#endif
	return DI_ERR_OK;
}

DI_ERR_CODE DI_FRONT_PANEL_SetDimmingLevel(HUINT8 ucDimLevel)
{
#if	defined(CONFIG_PANEL_LCD) || defined(CONFIG_PANEL_VFD)
	DRV_Error	nDrvErr = DRV_OK;
	unsigned char	ucMicomDimLevel = 0;
	static HUINT8	s_ucDimLevel = 255;

	if (s_ucDimLevel == ucDimLevel)
	{
		return	DI_ERR_OK;
	}

	s_ucDimLevel = ucDimLevel;
	ucMicomDimLevel = P_FRONT_PANEL_TransLevel(ucDimLevel);

	PrintDebug("[%s] ucDimLevel(%d), ucMicomDimLevel(%d)\n", __func__, ucDimLevel, ucMicomDimLevel);

	nDrvErr = DRV_MICOM_SetLcdDimmingLevel(ucMicomDimLevel);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_SetLcdDimmingLevel() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}
#elif defined(CONFIG_FRONT_MICOM_JAPAN)
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_Set7SegDimmingLevel(ucDimLevel);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_Set7SegDimmingLevel() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}
	PrintDebug("[%s] ucDimLevel is 0x%x\n", __func__, ucDimLevel);
#else
	ucDimLevel = ucDimLevel;
#endif
	return DI_ERR_OK;
}

DI_ERR_CODE DI_FRONT_PANEL_SetDisplayAni(HBOOL bOn_nOff)
{
	DRV_Error nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_SetDisplayAni(bOn_nOff);
	if(nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_SetDisplayAni() \n", nDrvErr);
		return DI_ERR_ERROR;
	}

	PrintDebug("[%s] Display Animation is %s\n", __func__, bOn_nOff?"ON":"OFF");

	return DI_ERR_OK;
}

