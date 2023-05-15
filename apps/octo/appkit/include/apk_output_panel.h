#ifndef __APK_OUTPUT_PANEL_H__
#define __APK_OUTPUT_PANEL_H__

typedef enum
{
	eApkPANEL_PRIORITY_UPPER	= 1,
	eApkPANEL_PRIORITY_MIDDLE	= 0,
	eApkPANEL_PRIORITY_LOWER	= -1
} ApkPanel_Priority_e;

typedef enum
{
	eApkPANEL_TYPE_STRING,
	eApkPANEL_TYPE_LED,
	eApkPANEL_TYPE_ALL
} ApkPanel_Type_e;

HERROR	APK_OUTPUT_PANEL_Init(void);
HERROR	APK_OUTPUT_PANEL_DeInit(void);
HERROR	APK_OUTPUT_PANEL_SetAppPriority(HINT32 nPriority);
HERROR	APK_OUTPUT_PANEL_SetLed(DxLedId_e eLedId, DxLedStatus_e eStatus);
HERROR	APK_OUTPUT_PANEL_SetLedAuto(void);
HERROR	APK_OUTPUT_PANEL_SetLedCategory(DxPanelCategory_e eCategory);
HERROR	APK_OUTPUT_PANEL_ClearLed(void);
HERROR	APK_OUTPUT_PANEL_SetPanel(HINT32 ePriority, const HCHAR *pszString, DxPanelAlign_e ePanelAlign);
HERROR	APK_OUTPUT_PANEL_SetPanelAuto(HINT32 nPriority);
HERROR	APK_OUTPUT_PANEL_SetPanelDimLevel(HINT32 nPriority, HINT32 nDimLevel);
HERROR	APK_OUTPUT_PANEL_SetPanelAnimation(HINT32 nPriority);
HERROR	APK_OUTPUT_PANEL_SetRgbLed(DxLedId_e eLedId, DxRgbLedColor_e eLedColor, HBOOL bBlink);
HERROR	APK_OUTPUT_PANEL_SetRgbLedOff(DxLedId_e eLedId);
HERROR	APK_OUTPUT_PANEL_SetRgbLedDimm(DxLedId_e eLedId, DxRgbLedDimmLevel_e eDimmLevel);
HERROR	APK_OUTPUT_PANEL_SetRgbLedBlink(DxLedId_e eLedId, HBOOL bBlink);
HERROR	APK_OUTPUT_PANEL_SetPowerLedState(DxPowerLedStatus_e eLedStatus);

#endif // __APK_OUTPUT_PANEL_H__

