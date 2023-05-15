/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_panel.h
	@brief	  Front panel PAL header

	Description: handle RCU input & front panel display		\n
	Module: mw/uio			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_PANEL_H__
#define	__PAL_PANEL_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/**
  @remark   Turn off all LEDs.
  @remark	type : Inline

  @return	NONE
*/
extern void PAL_PANEL_ClearLed(void);

/**
  @remark   Turn off each LEDs.
  @remark	type : Inline

  @return	NONE
*/
extern HERROR PAL_PANEL_SetLedOnOff(DxLedId_e eLedId, HBOOL bOnOff);

/**
  @remark   Turn off each LEDs On Standby Mode.
  @remark	type : Inline

  @return	NONE
*/

extern HERROR PAL_PANEL_SetLedOnOffOnStandbyMode(DxLedId_e eLedId, HBOOL bOnOff);

/**
  @remark   	Set Led on/off without led change status.
  @remark	type :
  @return	NONE
*/

extern HERROR PAL_PANEL_ForceSetLedOnOff(DxLedId_e eLedId, HBOOL bOnOff);

/**
  @remark   Set up LED blink. Turn off blink if delay is 0.
  @remark	type : Inline

  @return	NONE
*/
extern HERROR PAL_PANEL_SetLedBlink(DxLedId_e eLedId, HUINT32 delay);

/**
  @remark	Get LED on/off status.
  @remark	type
  @return
*/
extern HERROR PAL_PANEL_GetLedStatus(DxLedId_e eLedId, HBOOL *pbOnOff);

/**
  @remark   Decide to use or not of power LED.
  @remark	type : Inline

  @return	NONE
*/
extern HERROR PAL_PANEL_SetPowerLedUse(HBOOL bOnOff);

/**
  @remark   Set up power LED state.
  @remark	type : Inline

  @return	NONE
*/
extern HERROR PAL_PANEL_SetPowerLedState(PowerLedState_t eLedState);
extern HERROR PAL_PANEL_SetDim(PanelDimmingLevel_t eDimLevel);
extern HERROR PAL_PANEL_GetDimLevel(PanelDimmingLevel_t *peDimLevel);
extern HERROR PAL_PANEL_SetDimLevel(PanelDimmingLevel_t eDimLevel);

extern HERROR PAL_PANEL_SetPower(HBOOL bOnOff);
extern HERROR PAL_PANEL_DisplayString(HUINT8 *szStr);
extern HERROR PAL_PANEL_SetTimeDisplay(HBOOL bOnOff);
extern HERROR PAL_PANEL_SetColdBootCondition(HBOOL bFrontPower, HBOOL bFrontDisplay);
extern HERROR PAL_PANEL_ConvertStringToPanelString(HUINT8 *pszStr, DxPanelAlign_e eAlign);
extern HERROR PAL_PANEL_GetMaxStringLen(HUINT32 *pulMaxLen);
extern HERROR PAL_PANEL_SetPanelOnByOTA (void);

/* controll dimming level of both of VFD & LED */
extern HERROR PAL_PANEL_SetDimmingLevel(PanelDimmingLevel_t level);


HERROR PAL_PANEL_GetCapability(Panel_Capability_t *pstPanelCapa);

/* RGB Led Functions */
extern HERROR PAL_PANEL_SetRgbLed(DxLedId_e eLedId, HBOOL r, HBOOL g, HBOOL b);
extern HERROR PAL_PANEL_SetLedsDimmingLevel(DxLedId_e eLedId, HUINT8 ucDimLevel);

#endif /* __PAL_PANEL_H__ */

