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
 * $brcm_Workfile: VideoSettings.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/2/11 12:57p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/diagnostic/include/VideoSettings.h $
 * 
 * 2   9/2/11 12:57p zqian
 * SWDTV-4106: Implement video settings menu.
 * 
 * 1   6/16/11 3:47p jliang
 * SWDTV-4106: Initial check in.
 * 
 *  VideoSettings.h
 *
 *  Created on: Jun 13, 2011
 ***************************************************************************/

#ifndef VIDEOSETTINGS_H_
#define VIDEOSETTINGS_H_

#include <stdint.h>
/*
 *
 */
namespace broadcom { namespace broadband { namespace diagnostic {

#define SETTOP_MAX_PQ_LEVELS 6

#define SETTOP_ANR_DNR_OFF_LEVEL -99

#define DYCONTRAST_STEP 32

int32_t mnr_level[SETTOP_MAX_PQ_LEVELS] = {SETTOP_ANR_DNR_OFF_LEVEL,-75,-33,0,100,200};
int32_t bnr_level[SETTOP_MAX_PQ_LEVELS] = {SETTOP_ANR_DNR_OFF_LEVEL,-75,-33,0,100,500};
int32_t dcr_level[SETTOP_MAX_PQ_LEVELS] = {SETTOP_ANR_DNR_OFF_LEVEL,-90,-50,0,50,90};

int32_t anr_level[6] = {SETTOP_ANR_DNR_OFF_LEVEL,-12,-6,0,6,12};



typedef enum videosetting_dnr_level{

	dnr_level_off=0,
	dnr_level_low,
	dnr_level_medium_low,
	dnr_level_medium,
	dnr_level_medium_high,
	dnr_level_high
}videosetting_dnr_level;


class VideoSettings {
public:
	VideoSettings();
	virtual ~VideoSettings();

public:
	int16_t GetBrightness();
	bool SetBrightness(int16_t brightness);
	int16_t GetContrast();
	bool SetContrast(int16_t contrast);
	int16_t GetSaturation();
	bool SetSaturation(int16_t saturation);
	int16_t GetHue();
	bool SetHue(int16_t hue);
	int16_t GetSharpness();
	bool SetSharpness(int16_t sharpness);
	int16_t GetBNR();
	bool SetBNR(int16_t bnr);
	int16_t GetMNR();
	bool SetMNR(int16_t mnr);
	int16_t GetDCR();
	bool SetDCR(int16_t dcr);
	int16_t GetANR();
	bool SetANR(int16_t anr);
	int16_t GetDyContrast();
	bool SetDyContrast(int16_t dycontrast);
	int16_t GetDemoMode();
	bool SetDemoMode(int16_t demoMode);


};

}}}
#endif /* VIDEOSETTINGS_H_ */
