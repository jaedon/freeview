/**
	@file	  pal_sys_eeprominit.h
	@brief

	Description: \n
	Module: PAL/SYS			 	\n
	Remarks : 										\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __PAL_SYS_EEPROMINIT_H__
#define	__PAL_SYS_EEPROMINIT_H__

#include <di_nvram.h>
#include <di_video.h>
#include <di_scart.h>
#include <di_front_panel.h>

static DI_NVRAM_OSD_DISPLAY_E			s_panelStatus = DI_NVRAM_OSD_ON;
static DI_NVRAM_FRONTPANEL_DISPLAY_E			s_panelDisplay = DI_NVRAM_FRONTPANEL_DISPLAY_ON;
static unsigned char					s_panelDimLevel = DIM_LEVEL_MAX;
static unsigned int						s_OTA = DI_NVRAM_OTA_FLAG_NO_OTA;
static unsigned char					s_language[LANG_639_CODE_MAX] = { 'e', 'n', 'g', };
static DI_NVRAM_SCART_SETTING_VALUE	s_scartRGB = DI_NVRAM_SCART_OUTPUT_CVBS;
static DI_NVRAM_VIDEO_RESOLUTION_E		s_HDresolution = DI_NVRAM_RESOLUTION_720x576I;
static unsigned char					s_tunerId = 0;
static unsigned char					s_antennaPwr = 0;
#if 0 // #if 0 - temporally disblaed (for the warning removing)
static unsigned char					s_OadVer = 0;
static unsigned char					s_netDev = 0;	// 추후에 로더에 무선랜 들어가면 살릴 것.
#endif // #if 0
static unsigned char					s_netConf = 0;
static unsigned char					s_netaddr[INET_ADDR_LENGTH][INET_ADDR_LENGTH] = {
										 	{ 192, 0, 2, 200, },
											{ 255, 255, 255, 255, },
											{ 192, 0, 2, 1, },
											{ 192, 0, 2, 1, },
										};

static unsigned char					s_netPortalIp[INET_ADDR_LENGTH] = {192, 168, 0, 250, };
#if 0 // #if 0 - for the warning removing
static unsigned char					s_netDownPath[INET_DOWN_PATH_MAX] = "http://unkown";
#endif // #if 0

#endif /* __PAL_SYS_EEPROMINIT_H__ */

/* end of file */

