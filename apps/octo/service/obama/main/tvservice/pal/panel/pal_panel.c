/**
	@file     pal_panel.c
	@brief    Front panel abstraction layer

	Description: VFD or LCD or 7 segment display, front key, LED 관련 기능을 제공하는 PAL/PANEL 모듈.
	Module: PAL/PANEL			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>


#include <di_led.h>
#include <di_front_panel.h>
#include <di_nvram.h>

#include <pal_panel.h>
#include "_pal_panel.h"
#include <pal_key.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

	//	DI에서 기능 지원으로 인해 해당 내용 삭제.

	//	Dimming의 조정은 Repeat키에 영향을 줄수 있음.
	//	Dimming으로 인해 Repeat이 제대로 동작하지 않을 경우 해당 디파인을 풀어줘야 함.
	//#define	CONFIG_IGNORE_SAME_DIMMING_DATA

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HUINT32	s_ulDiLedBits;

#if	defined(CONFIG_IGNORE_SAME_DIMMING_DATA)
static	HINT32		s_nLastDimmLevel = -1;
#endif
/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
void PAL_PANEL_ClearLed(void)
{
	DI_LED_ClearAll();
	s_ulDiLedBits = 0;
}

HERROR PAL_PANEL_SetLedOnOff(DxLedId_e eLedId, HBOOL bOnOff)
{
	DI_LED_ID	diLedId;

	HxLOG_Debug("(+)Enter - eLedId:0x%X bOnOff:0x%X\n",eLedId,bOnOff);

	diLedId = pal_panel_GetDiLedIndex(eLedId);
	if(diLedId != 0) // 0 means that ERROR !!!!
	{
		if(bOnOff == TRUE)
		{
			if( (s_ulDiLedBits & diLedId) == 0 )
			{
				HxLOG_Print("\t[%s]DI_LED_SetOn(%x)\n", __FUNCTION__, diLedId);
				DI_LED_SetOn(diLedId);
				s_ulDiLedBits |= diLedId;
			}
		}
		else
		{
			if( (s_ulDiLedBits & diLedId) != 0 )
			{
				HxLOG_Print("\t[%s]DI_LED_SetOff(%x)\n", __FUNCTION__, diLedId);
				DI_LED_SetOff(diLedId);
				s_ulDiLedBits &= ~diLedId;
			}
		}
	}

#if defined(CONFIG_MW_SYS_COLOR_LED)
	if ( eDxLED_ID_REC == eLedId )
	{
		diLedId = pal_panel_GetDiLedIndex(eDxLED_ID_RED_COLOR);
		HxLOG_Print("\t\t[%s:%d] eLED_ID_RED_COLOR - diLedId:0x%X\n",__FUNCTION__,__LINE__, diLedId);

		if(diLedId != 0) // 0 means that ERROR !!!!
		{
			if(bOnOff == TRUE)
			{
				if( (s_ulDiLedBits & diLedId) == 0 )
				{
					HxLOG_Print("\t\t[%s]DI_LED_SetOn(%x)\n", __FUNCTION__, diLedId);
					DI_LED_SetOn(diLedId);
					s_ulDiLedBits |= diLedId;
				}
#if 0
				else
					HLIB_CMD_Printf("\n On Red Led Id Fail\n");
#endif
			}
			else
			{
				if( (s_ulDiLedBits & diLedId) != 0 )
				{
					HxLOG_Print("\t\t[%s]DI_LED_SetOff(%x)\n", __FUNCTION__, diLedId);
					DI_LED_SetOff(diLedId);
					s_ulDiLedBits &= ~diLedId;
				}
			}
		}

	}
#endif

	HxLOG_Debug("(-)Leave\n");
	return ERR_OK;
}

HERROR PAL_PANEL_SetLedOnOffOnStandbyMode(DxLedId_e eLedId, HBOOL bOnOff)
{
	DI_LED_ID	diLedId;

	HxLOG_Debug("(+)Enter - eLedId:0x%X bOnOff:0x%X\n",eLedId,bOnOff);

	diLedId = pal_panel_GetDiLedIndex(eLedId);
	if(diLedId != 0) // 0 means that ERROR !!!!
	{
		if(bOnOff == TRUE)
		{
			if( (s_ulDiLedBits & diLedId) == 0 )
			{
				HxLOG_Print("\t[%s]DI_LED_SetOn(%x)\n", __FUNCTION__, diLedId);
				DI_LED_SetOn(diLedId);
				s_ulDiLedBits |= diLedId;
			}
		}
		else
		{
			if( (s_ulDiLedBits & diLedId) != 0 )
			{
				HxLOG_Print("\t[%s]DI_LED_SetOff(%x)\n", __FUNCTION__, diLedId);
				DI_LED_SetOff(diLedId);
				s_ulDiLedBits &= ~diLedId;
			}
		}
	}

	HxLOG_Debug("(-)Leave\n");
	return ERR_OK;
}


HERROR PAL_PANEL_ForceSetLedOnOff(DxLedId_e eLedId, HBOOL bOnOff)
{
	DI_LED_ID	diLedId;

	HxLOG_Debug("(+)Enter - eLedId:0x%X bOnOff:0x%X\n",eLedId,bOnOff);

	diLedId = pal_panel_GetDiLedIndex(eLedId);
	if(diLedId != 0) // 0 means that ERROR !!!!
	{
		if(bOnOff == TRUE)
		{
			HxLOG_Print("\t[%s]DI_LED_SetOn(%x)\n", __FUNCTION__, diLedId);
			DI_LED_SetOn(diLedId);
		}
		else
		{
			HxLOG_Print("\t[%s]DI_LED_SetOff(%x)\n", __FUNCTION__, diLedId);
			DI_LED_SetOff(diLedId);
		}
	}

#if defined(CONFIG_MW_SYS_COLOR_LED)
	if ( eDxLED_ID_REC == eLedId )
	{
		diLedId = pal_panel_GetDiLedIndex(eDxLED_ID_RED_COLOR);
		if(diLedId != 0) // 0 means that ERROR !!!!
		{
			if(bOnOff == TRUE)
			{
				HxLOG_Print("\t\t[%s]DI_LED_SetOn(%x)\n", __FUNCTION__, diLedId);
				DI_LED_SetOn(diLedId);
			}
			else
			{
				HxLOG_Print("\t\t[%s]DI_LED_SetOff(%x)\n", __FUNCTION__, diLedId);
				DI_LED_SetOff(diLedId);
			}
		}
	}
#endif

	HxLOG_Debug("(-)Leave\n");
	return ERR_OK;
}


HERROR PAL_PANEL_SetLedBlink(DxLedId_e eLedId, HUINT32 delay)
{
	DI_LED_ID	diLedId;

	diLedId = pal_panel_GetDiLedIndex(eLedId);
	if(diLedId != 0) // 0 means that ERROR !!!!
	{
		DI_LED_SetBlinkOn(diLedId, delay);
	}

#if defined(CONFIG_MW_SYS_COLOR_LED)
	if ( eDxLED_ID_REC == eLedId )
	{
		diLedId = pal_panel_GetDiLedIndex(eDxLED_ID_RED_COLOR);
		if(diLedId != 0) // 0 means that ERROR !!!!
		{
			DI_LED_SetBlinkOn(diLedId, delay);
		}
	}
#endif
	return ERR_OK;
}

HERROR PAL_PANEL_GetLedStatus(DxLedId_e eLedId, HBOOL *pbOnOff)
{
	DI_LED_ID	diLedId;

	if (pbOnOff == NULL)
	{
		return ERR_FAIL;
	}

	diLedId = pal_panel_GetDiLedIndex(eLedId);
	if(diLedId != 0) // 0 means that ERROR !!!!
	{
		*pbOnOff = (s_ulDiLedBits & diLedId) ? TRUE : FALSE;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR PAL_PANEL_SetPowerLedUse(HBOOL bOnOff)
{
	DI_LED_SetPowerLedUse(bOnOff);

	return ERR_OK;
}

#if (defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)) && defined(CONFIG_MW_SYS_COLOR_LED)
HERROR PAL_PANEL_SetPowerLedOnOff(PowerLedState_t eLedState)
{
	HERROR hErr = ERR_OK;

	switch(eLedState)
	{
		case ePOWER_LED_STANDBY:
			hErr = PAL_PANEL_SetLedOnOff(eDxLED_ID_BLUE_COLOR, FALSE);
			hErr = PAL_PANEL_SetLedOnOff(eDxLED_ID_RED_COLOR, TRUE);
			break;
		case ePOWER_LED_WATCHING:
			hErr = PAL_PANEL_SetLedOnOff(eDxLED_ID_RED_COLOR, FALSE);
			hErr = PAL_PANEL_SetLedOnOff(eDxLED_ID_BLUE_COLOR, TRUE);
			break;
		default:
			break;
	}

	return hErr;
}
#endif

HERROR PAL_PANEL_SetPowerLedState(PowerLedState_t eLedState)
{
	switch(eLedState)
	{
		case ePOWER_LED_STANDBY:
#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)
			PAL_PANEL_SetPowerLedOnOff(eLedState);
#elif defined(CONFIG_MW_SYS_LED_DIMMING_CTRL_ON_STBY)
			DI_LED_SetPowerLedDimmingLevel(ePANEL_DIM_LevelOff);
			DI_LED_SetPowerLedState (DI_LED_STANDBY_LEDLOW);
#else
			DI_LED_SetPowerLedState (DI_LED_STANDBY);
#endif
			break;

		case ePOWER_LED_WATCHING:
#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)
			PAL_PANEL_SetPowerLedOnOff(eLedState);
#elif defined(CONFIG_MW_SYS_LED_DIMMING_CTRL_ON_STBY)
			DI_LED_SetPowerLedDimmingLevel(ePANEL_DIM_Level100);
			DI_LED_SetPowerLedState (DI_LED_WATCHING);
#else
			DI_LED_SetPowerLedState (DI_LED_WATCHING);
#endif
#if defined(CONFIG_MW_SYS_COLOR_LED)
			if ( ( (s_ulDiLedBits & DI_LED_NUM19) != 0 ) && ( (s_ulDiLedBits & DI_LED_NUM8) != 0 ) ) // RED and RECORD LED
			{
				//HLIB_CMD_Printf("\n\n[%s:%d] On REC RED ID\n\n",__FUNCTION__,__LINE__);
				s_ulDiLedBits &= ~DI_LED_NUM19;
				s_ulDiLedBits &= ~DI_LED_NUM8;
				PAL_PANEL_SetLedOnOff(eDxLED_ID_REC, TRUE);
			}
#endif
			break;

		case ePOWER_LED_RECORDING:
#if defined(CONFIG_PROD_HGS1000S)
			DI_LED_SetPowerLedState (DI_LED_RECORDING);
#elif defined(CONFIG_MW_SYS_LED_DIMMING_CTRL_ON_STBY)
			DI_LED_SetPowerLedDimmingLevel(ePANEL_DIM_LevelOff);
			DI_LED_SetPowerLedState (DI_LED_STANDBY);
#endif
			break;


		default: // off
			DI_LED_SetPowerLedState (DI_LED_OFF);
			break;
	}

	return ERR_OK;
}

HERROR PAL_PANEL_SetDim(PanelDimmingLevel_t eDimLevel)
{
	DI_ERR_CODE	diErr;
	HUINT8		ucDimLevel;

	HxLOG_Print("[PAL_PANEL_SetDim] eDimLevel [%d]\n", eDimLevel);

	ucDimLevel = (HUINT8)eDimLevel;
	if (ucDimLevel > (HUINT8)DIM_LEVEL_MAX)
	{
		ucDimLevel = (HUINT8)DIM_LEVEL_MAX;
	}

#if	defined(CONFIG_IGNORE_SAME_DIMMING_DATA)
	if (ucDimLevel == s_nLastDimmLevel)
		return ERR_OK;
	s_nLastDimmLevel = ucDimLevel;
#endif

	HxLOG_Print("[PAL_PANEL_SetDim] DI_FRONT_PANEL_SetDimmingLevel(%d) \n", ucDimLevel);
	diErr = DI_FRONT_PANEL_SetDimmingLevel(ucDimLevel);
	if (diErr != DI_ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_PANEL_GetDimLevel(PanelDimmingLevel_t *peDimLevel)
{
	DI_ERR_CODE	diErr;
	HUINT8		ucDimLevel;

	if (peDimLevel == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	diErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_PANEL_DIMMING, 0, (void *)&ucDimLevel, sizeof(HUINT8));
	if (diErr != DI_ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (ucDimLevel > (HUINT8)DIM_LEVEL_MAX)
	{
		ucDimLevel = (HUINT8)DIM_LEVEL_MAX;
	}

	*peDimLevel= (PanelDimmingLevel_t)ucDimLevel;

	return ERR_OK;
}

HERROR PAL_PANEL_SetDimLevel(PanelDimmingLevel_t eDimLevel)
{
	DI_ERR_CODE	diErr;
	HUINT8		ucDimLevel;

	if (eDimLevel > ePANEL_DIM_LevelMax)
	{
		eDimLevel = ePANEL_DIM_LevelMax;
	}

	ucDimLevel = (HUINT8)eDimLevel;
	diErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_PANEL_DIMMING, 0, (void *)&ucDimLevel, sizeof(HUINT8));
	if (diErr != DI_ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


#if !defined(CONFIG_MW_SYS_VFD)
HERROR		PAL_PANEL_SetDimmingLevel(PanelDimmingLevel_t eDimLevel)
{
	DI_ERR_CODE	err = ERR_OK;
	HUINT8 uDimLevel = 0;

	/* set dimm level for LED */
	uDimLevel = (HUINT8)eDimLevel;
	err = DI_LED_SetPowerLedDimmingLevel(uDimLevel);
//	HxLOG_Print("[%s:%d] level:%d DI_FRONT_PANEL_SetDimmingLevel(%d) -- DI_LED_SetPowerLedDimmingLevel(%d)\n",__FUNCTION__,__LINE__,ucDiLedDimLevel,ucDiLedDimLevel);

	return err;
}
#endif

HERROR PAL_PANEL_SetPower(HBOOL bOnOff)
{
	DI_ERR_CODE	diErr;

	diErr = DI_FRONT_PANEL_SetPanelPower(bOnOff);
	if(diErr == DI_ERR_OK)
	{
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}
}

HERROR PAL_PANEL_DisplayString(HUINT8 *pszStr)
{
	HUINT8		 szPanelString[32];

	HLIB_STD_StrUtf8NCpy (szPanelString, pszStr, 32);

	HxLOG_Info("PAL_PANEL_DisplayString(%s)\r\n", szPanelString);

	DI_FRONT_PANEL_Display(szPanelString);

	return ERR_OK;
}


HERROR PAL_PANEL_SetTimeDisplay(HBOOL bOnOff)
{
	return DI_FRONT_PANEL_SetTimeDisplay(bOnOff);
}

HERROR PAL_PANEL_SetColdBootCondition (HBOOL bDisplayVideo, HBOOL bViewPanel)
{
#if 0
	DI_NVRAM_FRONTPANEL_DISPLAY_E	ePannelDisplay;

	DI_NVRAM_GetField(DI_NVRAM_FIELD_PANEL_DISPLAY, 0, 0, &ePannelDisplay, sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E));
	if(bFrontDisplay == TRUE)
	{
		if(ePannelDisplay != DI_NVRAM_FRONTPANEL_DISPLAY_ON)
		{
			ePannelDisplay = DI_NVRAM_FRONTPANEL_DISPLAY_ON;
			DI_NVRAM_SetField(DI_NVRAM_FIELD_PANEL_DISPLAY, 0, &ePannelDisplay, sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E));
		}
	}
	else
	{
		if(ePannelDisplay != DI_NVRAM_FRONTPANEL_DISPLAY_OFF)
		{
			ePannelDisplay = DI_NVRAM_FRONTPANEL_DISPLAY_OFF;
			DI_NVRAM_SetField(DI_NVRAM_FIELD_PANEL_DISPLAY, 0, &ePannelDisplay, sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E));
		}
	}
#endif

	HERROR							hErr	= ERR_FAIL;
	DI_NVRAM_OSD_DISPLAY_E		 	eVideo;
	DI_NVRAM_FRONTPANEL_DISPLAY_E	ePanel;


	// Video
	hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_OSD_DISPLAY, 0, &eVideo, sizeof(DI_NVRAM_OSD_DISPLAY_E));
	if (hErr != ERR_OK)
		return ERR_FAIL;

	if (bDisplayVideo == TRUE)
	{
		if (eVideo != DI_NVRAM_OSD_ON)
		{
			eVideo = DI_NVRAM_OSD_ON;
			hErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_OSD_DISPLAY, 0, &eVideo, sizeof(DI_NVRAM_OSD_DISPLAY_E));
			if (hErr != ERR_OK)
				return ERR_FAIL;
		}
	}
	else
	{
		if (eVideo != DI_NVRAM_OSD_OFF)
		{
			eVideo = DI_NVRAM_OSD_OFF;
			hErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_OSD_DISPLAY, 0, &eVideo, sizeof(DI_NVRAM_OSD_DISPLAY_E));
			if (hErr != ERR_OK)
				return ERR_FAIL;
		}
	}

	// Front Panel
	hErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_PANEL_DISPLAY, 0, &ePanel, sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E));
	if (hErr != ERR_OK)
		return ERR_FAIL;

	if (bViewPanel == TRUE)
	{
		if (ePanel != DI_NVRAM_FRONTPANEL_DISPLAY_ON)
		{
			ePanel = DI_NVRAM_FRONTPANEL_DISPLAY_ON;
			hErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_PANEL_DISPLAY, 0, &ePanel, sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E));
			if ( hErr != ERR_OK)
				return ERR_FAIL;
		}
	}
	else
	{
		if (ePanel != DI_NVRAM_FRONTPANEL_DISPLAY_OFF)
		{
			ePanel = DI_NVRAM_FRONTPANEL_DISPLAY_OFF;
			hErr = DI_NVRAM_SetField (DI_NVRAM_FIELD_PANEL_DISPLAY, 0, &ePanel, sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E));
			if (hErr != ERR_OK)
				return ERR_FAIL;
		}
	}

	return ERR_OK;
}


HERROR PAL_PANEL_SetPanelOnByOTA (void)
{
	DI_NVRAM_FRONTPANEL_DISPLAY_E	 ePanel;

	ePanel = DI_NVRAM_FRONTPANEL_DISPLAY_ON;
	DI_NVRAM_SetField (DI_NVRAM_FIELD_PANEL_DISPLAY, 0, &ePanel, sizeof(DI_NVRAM_FRONTPANEL_DISPLAY_E));

	return ERR_OK;
}

HERROR PAL_PANEL_GetCapability(Panel_Capability_t *pstPanelCapa)
{
	HUINT32 ulMaxLen,idx;
	HUINT32 ulLedCapaMask;

	// 현재로서는 DI의 아래 함수 사용 불가능.
#if 0
	DI_FRONT_PANEL_CAP stCapability;

	DI_FRONT_PANEL_GetCapability(&stCapability);
#endif

	DI_LED_GetCapability(&ulLedCapaMask);

	// 이 길이는 시간표시를 제외하고 Display가능한 글자이다.
	PAL_PANEL_GetMaxStringLen(&ulMaxLen);

#if defined (CONFIG_MW_SYS_VFD)
	pstPanelCapa->type = ePanelType_VFD;
#elif defined (CONFIG_MW_SYS_LCD)
	pstPanelCapa->type = ePanelType_LCD;
#elif defined (CONFIG_MW_SYS_7SEG)
	pstPanelCapa->type = ePanelType_7SEG;
#endif

	pstPanelCapa->ulLen = ulMaxLen;

	for (idx = 0 ; idx < NUM_MAX_DxLED; idx++ )
	{
		if (pal_panel_GetDiLedIndex(idx) & ulLedCapaMask)
			pstPanelCapa->abLEDs[idx] = TRUE;
	}

#if (defined(CONFIG_MW_SYS_VFD) || defined(CONFIG_MW_SYS_LCD))
	pstPanelCapa->bSupportDim = TRUE;
#else
	pstPanelCapa->bSupportDim = FALSE;
#endif

	HxLOG_Print("[%s:%d] type:(0x%X) len:(%d)\n",__FUNCTION__,__LINE__,
			pstPanelCapa->type, pstPanelCapa->ulLen);

	return ERR_OK;
}

HERROR PAL_PANEL_SetRgbLed(DxLedId_e eLedId, HBOOL r, HBOOL g, HBOOL b)
{
	HERROR err = ERR_FAIL;
	DI_ERR_CODE	diErr = DI_ERR_ERROR;
	DI_LED_ID	diLedId = 0;

	diLedId = pal_panel_GetDiLedIndex(eLedId);
	if(diLedId != 0)
	{
		diErr = DI_LED_SetRGBOn(diLedId, (HUINT8)r, (HUINT8)g, (HUINT8)b);
		if(diErr == DI_ERR_OK)
		{
			err = ERR_OK;
		}
		else
		{
			HxLOG_Warning("Fail to DI_LED_SetRGBOn(0x%08x, %d, %d, %d) - ret( %d )\n", diLedId, r, g, b, diErr);
		}
	}

	return err;
}

HERROR PAL_PANEL_SetLedsDimmingLevel(DxLedId_e eLedId, HUINT8 ucDimLevel)
{
	HERROR err = ERR_OK;
	DI_ERR_CODE	diErr = DI_ERR_ERROR;
	DI_LED_ID	diLedId;

	diLedId = pal_panel_GetDiLedIndex(eLedId);
	if(diLedId != 0) // 0 means that ERROR !!!!
	{
		diErr = DI_LED_SetLedsDimmingLevel(diLedId, ucDimLevel);
	}

	if(diErr == DI_ERR_OK)
	{
		err = ERR_OK;
	}
	else
	{
		HxLOG_Warning("Fail to DI_LED_SetLedsDimmingLevel(0x%08x, %d) - ret( %d )\n", diLedId, ucDimLevel, diErr);
	}

	return err;
}


// Front Panel에 아무 것도 없는 Model일 경우를 위한 Dummy 함수들.
#if (!defined(CONFIG_MW_SYS_VFD) && !defined(CONFIG_MW_SYS_7SEG) && !defined(CONFIG_MW_SYS_VFD))
DI_LED_ID pal_panel_GetDiLedIndex(DxLedId_e eLedId)
{
#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)
	HxLOG_Print("\t\t[%s:%d] pal_panel_GetDiLedIndex - eLedId:0x%X\n",__FUNCTION__,__LINE__, eLedId);

	switch(eLedId)
	{
#if 0
		case eDxLED_ID_TV: return DI_LED_NUM0;
		case eDxLED_ID_RADIO: return DI_LED_NUM1;

/* PVR */
		case eDxLED_ID_REC: return DI_LED_NUM8;
		case eDxLED_ID_PLAY: return DI_LED_NUM9;
		case eDxLED_ID_PAUSE: return DI_LED_NUM10;
		case eDxLED_ID_RESERVE: return DI_LED_NUM11;

/* Audio */
		case eDxLED_ID_DOLBY: return DI_LED_NUM4;
		case eDxLED_ID_DOLBY_PLUS: return DI_LED_NUM5;
		case eDxLED_ID_DOLBY_PULSE: return DI_LED_NUM6;

/* Video */
		case eDxLED_ID_HD: return DI_LED_NUM3;
		case eDxLED_ID_1080P: return DI_LED_NUM2;

/* USB */
		case eDxLED_ID_USB: return DI_LED_NUM7;

/* WIFI */
		case eDxLED_ID_WIFI: return DI_LED_NUM12;

/* HDD Capacity */
		case eDxLED_ID_HDD_25_FULL: return DI_LED_NUM15;
		case eDxLED_ID_HDD_50_FULL: return DI_LED_NUM16;
		case eDxLED_ID_HDD_75_FULL: return DI_LED_NUM17;
		case eDxLED_ID_HDD_100_FULL: return DI_LED_NUM18;

#endif
/* LED Color */
		case eDxLED_ID_RED_COLOR: return DI_LED_NUM8;
		case eDxLED_ID_BLUE_COLOR: return DI_LED_NUM20;
		case eDxLED_ID_ORANGE_COLOR: return DI_LED_NUM21;

		default:
			VK_Print("\n\t!!!!!!!!! ERROR! unknown LED ID[%d] !!!!!!!!!\n", eLedId);
			break;
	}
#endif

#if defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_YS1000)
	HxLOG_Print("\t\t[%s:%d] pal_panel_GetDiLedIndex - eLedId:0x%X\n",__FUNCTION__,__LINE__, eLedId);

	switch(eLedId)
	{
		case eDxLED_ID_RGB_REC: return DI_LED_NUM2;
		case eDxLED_ID_RGB_NET: return DI_LED_NUM3;

		default:
			HxLOG_Print("\n\t!!!!!!!!! ERROR! unknown LED ID[%d] !!!!!!!!!\n", eLedId);
			break;
	}
#endif

	return 0;
}

DxLedId_e pal_panel_GetOctoLedIndex(DI_LED_ID eDiLedId)
{
	return 0;
}

HERROR PAL_PANEL_ConvertStringToPanelString(HUINT8 *pszStr, DxPanelAlign_e eAlign)
{
	return ERR_OK;
}

HERROR PAL_PANEL_GetMaxStringLen(HUINT32 *pulMaxLen)
{
	DI_FRONT_PANEL_CAP stCapability;

	DI_FRONT_PANEL_GetCapability(&stCapability);

	*pulMaxLen = stCapability.max_length;

	return ERR_OK;
}
#endif
/* end of file */
