/**************************************************************
 *	@file		tr069.c
 *	TR-069 Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/20
 *	@author			humax
 *	@brief			TR-069 Application
 **************************************************************/
#include <ondk.h>
#include	<oapi.h>
#include	<hapi.h>

#include "trport_common.h"



#define __________define__________

#define TR069_SCREEN_WIDTH		1280
#define TR069_SCREEN_HEIGHT		720
#define JAPANESE_FONT_PATH 		"/usr/fonts/JP-ARIB-JGTR01B.ttf"


/**************************************************************
	internal variables
 **************************************************************/

static	ONDKWindowHandle	trportWindow;
static	ONDKSurfaceHandle 	trportSurface;

static     HUINT8               sysRebootMessageStr_utf8[] =
{
	0xEF, 0xBB, 0xBF, 0xE3, 0x82, 0xB7, 0xE3, 0x82, 0xB9, 0xE3, 0x83, 0x86, 0xE3, 0x83, 0xA0, 0xE3,
	0x81, 0xAE, 0xE6, 0x9B, 0xB4, 0xE6, 0x96, 0xB0, 0xE3, 0x81, 0xAE, 0xE3, 0x81, 0x9F, 0xE3, 0x82,
	0x81, 0xE3, 0x80, 0x81, 0x33, 0x30, 0xE7, 0xA7, 0x92, 0xE5, 0xBE, 0x8C, 0xE3, 0x80, 0x81, 0xE3,
	0x82, 0xB7, 0xE3, 0x82, 0xB9, 0xE3, 0x83, 0x86, 0xE3, 0x83, 0xA0, 0xE3, 0x81, 0x8C, 0xE5, 0x86,
	0x8D, 0xE8, 0xB5, 0xB7, 0xE5, 0x8B, 0x95, 0xE3, 0x81, 0x97, 0xE3, 0x81, 0xBE, 0x00,
};

void TRP_GFX_CreateWindow(void)
{
	trportWindow = ONDK_WINDOW_Create(eAWINDOW_TR069, 0, 0, TR069_SCREEN_WIDTH, TR069_SCREEN_HEIGHT, "ARGB8888", FALSE);

	ONDK_WINDOW_SetShow(trportWindow, TRUE);
	ONDK_WINDOW_Update(trportWindow, NULL);

	trportSurface = ONDK_SURFACE_GetFromWindow(trportWindow);
}

void TRP_GFX_DestroyWindow(void)
{
	ONDK_SURFACE_Destroy(trportSurface);
	ONDK_WINDOW_Destroy(trportWindow);
}

void TRP_GFX_DrawRebootDialog(void)
{
	ONDKFontHandle	hFont;

	hFont = ONDK_FONT_Create(JAPANESE_FONT_PATH, 26, 26);
	ONDK_DRAW_SystemNoticeDialog(trportSurface, hFont, sysRebootMessageStr_utf8);
	ONDK_WINDOW_Update(trportWindow, NULL);
	ONDK_FONT_Release(hFont);
}

void TRP_GFX_HideWindow(void)
{
	ONDK_WINDOW_SetShow(trportWindow, FALSE);
	ONDK_WINDOW_Update(trportWindow, NULL);
}

void TRP_GFX_ShowWindow(void)
{
	ONDK_WINDOW_SetShow(trportWindow, TRUE);
	ONDK_WINDOW_Update(trportWindow, NULL);
}
void TRP_GFX_ShowMsg(void)
{
	HxLOG_Debug("reboot msg[%s]\r\n",sysRebootMessageStr_utf8);
}

