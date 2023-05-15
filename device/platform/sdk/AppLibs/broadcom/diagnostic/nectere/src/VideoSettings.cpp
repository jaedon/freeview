/***************************************************************************
 *     (c)2008-2011 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: VideoSettings.cpp $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/2/11 1:06p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/diagnostic/nectere/src/VideoSettings.cpp $
 * 
 * 5   9/2/11 1:06p zqian
 * SWDTV-4106: Implement video settings menu.
 * 
 * 4   6/22/11 1:16p jliang
 * SW7425-760: check in code fixed by Alex Song.
 * 
 * 3   6/17/11 4:48p jliang
 * SWDTV-4106: Modifed to call DFB 1.4.5 code.
 * 
 * 2   6/17/11 1:42p jliang
 * SWDTV-4106: commented out calls that's no longer compatible with DBF
 *  1.4.5.
 * 
 * 1   6/16/11 3:59p jliang
 * SWDTV-4106: Initial checkin.
 * 
 *  VideoSettings.cpp
 *
 *  Created on: Jun 13, 2011
 ***************************************************************************/
#include "VideoSettings.h"
#include "nexus_platform_features.h"
#include "nexus_picture_ctrl.h"
#include "nexus_video_window.h"
#include "nexus_video_adj.h"
#include "dfb_platform.h"

#include <stdio.h>

namespace broadcom { namespace broadband { namespace diagnostic {

int window_offset = 0;

VideoSettings::VideoSettings() {
	if (!window_offset) {
		DFB_PlatformSettings platformSettings;
		DFB_Platform_GetSettings(&platformSettings);
		NEXUS_VideoWindowHandle window = NEXUS_VideoWindow_Open((NEXUS_DisplayHandle) platformSettings.display[0].handle, 0);
		if (window != NULL) {
			window_offset = (int) window - (int) platformSettings.display[0].handle;
			NEXUS_VideoWindow_Close(window);
			printf("platformSettings.display[0].handle to window0 offset is %x\n", window_offset);
		}
		else
			window_offset = 0xe0; // as of 6/16/2011
	}
}

VideoSettings::~VideoSettings() {
	// TODO Auto-generated destructor stub
}

int16_t VideoSettings::GetBrightness() {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
#ifdef NEXUS_DTV_PLATFORM
    int16_t brightness;
    NEXUS_VideoWindow_GetBrightness(window, &brightness);
    return brightness;
#else
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    return pictureCtrlCommonSettings.brightness;
#endif
}

bool VideoSettings::SetBrightness(int16_t brightness) {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
#ifdef NEXUS_DTV_PLATFORM
    printf("Setting brightness to %d\n", brightness);
    NEXUS_Error rc = NEXUS_VideoWindow_SetBrightness(window, brightness);
#else
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    pictureCtrlCommonSettings.brightness = brightness;
    NEXUS_Error rc = NEXUS_PictureCtrl_SetCommonSettings(window, &pictureCtrlCommonSettings);
    printf("Setting brightness to %d\n", pictureCtrlCommonSettings.brightness);
#endif
    if (rc != NEXUS_SUCCESS)
    	printf("NEXUS_PictureCtrl_SetCommonSettings error: %d", rc);
	return rc == NEXUS_SUCCESS;
}

int16_t VideoSettings::GetContrast() {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    return pictureCtrlCommonSettings.contrast;
}

bool VideoSettings::SetContrast(int16_t contrast) {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    pictureCtrlCommonSettings.contrast = contrast;
    NEXUS_Error rc = NEXUS_PictureCtrl_SetCommonSettings(window, &pictureCtrlCommonSettings);
    printf("Setting contrast to %d\n", pictureCtrlCommonSettings.contrast);
    if (rc != NEXUS_SUCCESS)
    	printf("NEXUS_PictureCtrl_SetCommonSettings error: %d", rc);
	return rc == NEXUS_SUCCESS;
}

int16_t VideoSettings::GetSaturation() {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    return pictureCtrlCommonSettings.saturation;
}

bool VideoSettings::SetSaturation(int16_t saturation) {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    pictureCtrlCommonSettings.saturation = saturation;
    NEXUS_Error rc = NEXUS_PictureCtrl_SetCommonSettings(window, &pictureCtrlCommonSettings);
    printf("Setting saturation to %d\n", pictureCtrlCommonSettings.saturation);
    if (rc != NEXUS_SUCCESS)
    	printf("NEXUS_PictureCtrl_SetCommonSettings error: %d", rc);
	return rc == NEXUS_SUCCESS;
}

int16_t VideoSettings::GetHue() {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    return pictureCtrlCommonSettings.hue;
}

bool VideoSettings::SetHue(int16_t hue) {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    pictureCtrlCommonSettings.hue=hue;
    NEXUS_Error rc = NEXUS_PictureCtrl_SetCommonSettings(window, &pictureCtrlCommonSettings);
    printf("Setting hue to %d\n", pictureCtrlCommonSettings.hue);
    if (rc != NEXUS_SUCCESS)
    	printf("NEXUS_PictureCtrl_SetCommonSettings error: %d", rc);
	return rc == NEXUS_SUCCESS;
}

int16_t VideoSettings::GetSharpness() {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    return pictureCtrlCommonSettings.sharpness;
}

bool VideoSettings::SetSharpness(int16_t sharpness) {
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_PictureCtrlCommonSettings pictureCtrlCommonSettings;
    NEXUS_PictureCtrl_GetCommonSettings(window, &pictureCtrlCommonSettings);
    pictureCtrlCommonSettings.sharpness=sharpness;
	pictureCtrlCommonSettings.sharpnessEnable = 1;
    NEXUS_Error rc = NEXUS_PictureCtrl_SetCommonSettings(window, &pictureCtrlCommonSettings);
    printf("Setting sharpness to %d\n", pictureCtrlCommonSettings.sharpness);
    if (rc != NEXUS_SUCCESS)
    	printf("NEXUS_PictureCtrl_SetCommonSettings error: %d", rc);
	return rc == NEXUS_SUCCESS;
}

int16_t VideoSettings::GetBNR() {
	
	/*
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_VideoWindowDnrSettings dnrSettings;
    NEXUS_VideoWindow_GetDnrSettings(window, &dnrSettings);
    return dnrSettings.bnr.level;
	*/

	return 0;
	
}

bool VideoSettings::SetBNR(int16_t bnr) {
	
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_VideoWindowDnrSettings dnrSettings;	
	NEXUS_VideoWindow_GetDnrSettings(window, &dnrSettings);
   
	/*printf("Setting BNR to %d\n", bnr);*/
	
	if( bnr <= 5)
	{
		if( bnr == 0 )
			dnrSettings.bnr.mode = NEXUS_VideoWindowFilterMode_eBypass;
		else
			  dnrSettings.bnr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		dnrSettings.bnr.level = bnr_level[bnr];
		
		NEXUS_Error rc = NEXUS_VideoWindow_SetDnrSettings(window, &dnrSettings);

		if (rc != NEXUS_SUCCESS)
    		printf("NEXUS_PictureCtrl_SetCommonSettings error: %d\n", rc);
		return rc == NEXUS_SUCCESS;
	
	}    

	return 0;
	
}

int16_t VideoSettings::GetMNR() {
/*
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_VideoWindowDnrSettings dnrSettings;
    NEXUS_VideoWindow_GetDnrSettings(window, &dnrSettings);
    return dnrSettings.bnr.level;
	*/

	return 0;
}

bool VideoSettings::SetMNR(int16_t mnr) {
	
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_VideoWindowDnrSettings dnrSettings;
	NEXUS_VideoWindow_GetDnrSettings(window, &dnrSettings);
   
	/*printf("Setting mnr to %d\n", mnr);*/
		
	if( mnr <= 5 )
	{
		if( mnr == 0 )
			dnrSettings.mnr.mode = NEXUS_VideoWindowFilterMode_eBypass;
		else
			dnrSettings.mnr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		dnrSettings.mnr.level = mnr_level[mnr];
		NEXUS_Error rc = NEXUS_VideoWindow_SetDnrSettings(window, &dnrSettings);
				
		if (rc != NEXUS_SUCCESS)
    		printf("NEXUS_PictureCtrl_SetCommonSettings error: %d", rc);
		return rc == NEXUS_SUCCESS;
	}
	return 0;
}

int16_t VideoSettings::GetDCR() {
/*
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_VideoWindowDnrSettings dnrSettings;
    NEXUS_VideoWindow_GetDnrSettings(window, &dnrSettings);
    return dnrSettings.bnr.level;
	*/

	return 0;
}

bool VideoSettings::SetDCR(int16_t dcr) {
	
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_VideoWindowDnrSettings dnrSettings;
	NEXUS_VideoWindow_GetDnrSettings(window, &dnrSettings);
    
	/*printf("Setting dcr to %d\n", dcr);*/

	if( dcr <= 5 )
	{
		if( dcr == 0 )
			dnrSettings.dcr.mode = NEXUS_VideoWindowFilterMode_eBypass;
		else
			dnrSettings.dcr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		dnrSettings.dcr.level = dcr_level[dcr];
		NEXUS_Error rc = NEXUS_VideoWindow_SetDnrSettings(window, &dnrSettings);
				
		if (rc != NEXUS_SUCCESS)
    		printf("NEXUS_PictureCtrl_SetCommonSettings error: %d", rc);
		return rc == NEXUS_SUCCESS;
	}
	return 0;

}


int16_t VideoSettings::GetANR() {
	/*
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_VideoWindowAnrSettings anrSettings;
    NEXUS_VideoWindow_GetAnrSettings(window, &anrSettings);
    return dnrSettings.bnr.level;
	*/

	return 0;
}

bool VideoSettings::SetANR(int16_t anr) {
	
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_VideoWindowAnrSettings anrSettings;
	NEXUS_VideoWindow_GetAnrSettings(window, &anrSettings);
    
	printf("Setting anr to %d\n", anr);

	if( anr <= 5 )
	{
		if( anr == 0 )
			anrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eBypass;
		else
			anrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		anrSettings.anr.level = anr_level[anr];
		NEXUS_Error rc = NEXUS_VideoWindow_SetAnrSettings(window, &anrSettings);
				
		if (rc != NEXUS_SUCCESS)
    		printf("NEXUS_PictureCtrl_SetCommonSettings error: %d", rc);
		return rc == NEXUS_SUCCESS;
	}
	return 0;

}


int16_t VideoSettings::GetDyContrast() {
	
	return 0;
}

bool VideoSettings::SetDyContrast(int16_t dycontrast) {
	
	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_PictureCtrlContrastStretch contrastStretchSettings;
	NEXUS_PictureCtrl_GetContrastStretch(window, &contrastStretchSettings);
	    
	/*printf("Setting contrast stretch to %d\n", dycontrast);*/

	if( dycontrast <= 8 )
	{

		contrastStretchSettings.gain = dycontrast * DYCONTRAST_STEP;
		contrastStretchSettings.gainShift = 8;
		if(dycontrast == 0)
			contrastStretchSettings.enabled = false;
		else
			contrastStretchSettings.enabled = true;
		NEXUS_Error rc = NEXUS_PictureCtrl_SetContrastStretch(window, &contrastStretchSettings);
						
		if (rc != NEXUS_SUCCESS)
    		printf("NEXUS_PictureCtrl_SetContrastStretch error: %d", rc);
		return rc == NEXUS_SUCCESS;
	}
	return 0;

}

int16_t VideoSettings::GetDemoMode() {
	
	return 0;
}

bool VideoSettings::SetDemoMode(int16_t demoMode) {

	DFB_PlatformSettings platformSettings;
	DFB_Platform_GetSettings(&platformSettings);
	NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle) ((int) platformSettings.display[0].handle + window_offset);
	NEXUS_VideoWindowSplitScreenSettings splitScreen;
	NEXUS_VideoWindow_GetSplitScreenSettings(window, &splitScreen);

	NEXUS_VideoWindowSplitScreenMode demoScreenSetting;

	switch(demoMode){
	case 0:
		demoScreenSetting = NEXUS_VideoWindowSplitScreenMode_eDisable;
		break;
	case 1:
		demoScreenSetting = NEXUS_VideoWindowSplitScreenMode_eLeft;
		break;
	case 2:
		demoScreenSetting = NEXUS_VideoWindowSplitScreenMode_eRight;
		break;
	}
    
	/*printf("Setting Demo mode to %d\n", demoMode);*/

	if( demoMode <= 2 )
	{

		splitScreen.hue = demoScreenSetting;
		splitScreen.contrast = demoScreenSetting;
		splitScreen.brightness = demoScreenSetting;
		splitScreen.colorTemp = demoScreenSetting;
		splitScreen.sharpness = demoScreenSetting;
		splitScreen.contrastStretch = demoScreenSetting;
		splitScreen.dnr = demoScreenSetting;
		splitScreen.anr = demoScreenSetting;
		splitScreen.autoFlesh = demoScreenSetting;
		splitScreen.blueBoost = demoScreenSetting;
		splitScreen.greenBoost = demoScreenSetting;
		splitScreen.blueStretch = demoScreenSetting;
		splitScreen.dejagging = demoScreenSetting;
		splitScreen.deringing = demoScreenSetting;

		NEXUS_Error rc =  NEXUS_VideoWindow_SetSplitScreenSettings(window, &splitScreen);
						
		if (rc != NEXUS_SUCCESS)
    		printf("NEXUS_PictureCtrl_SetSplitScreenSettings error: %d", rc);
		return rc == NEXUS_SUCCESS;
	}
	return 0;
	

}



      

}}}
