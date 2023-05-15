/***************************************************************************
*     (c)2004-2009 Broadcom Corporation
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
 * $brcm_Workfile: hdmiscreen.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/20/12 3:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/hdmiscreen.cpp $
 * 
 * 2   3/20/12 3:52p erickson
 * SW7425-2664: protect against null handle
 * 
 * 1   5/11/09 7:42p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 * 
 * PR53373_DeepColor/2   5/11/09 6:30p vle
 * PR53373: Use boutput_hdmi_get to retrieve color depth settings.
 * 
 * PR53373_DeepColor/1   4/16/09 5:15p vle
 * PR53373: Add new HDMI screen to Brutus main menu.
 * 
 ***************************************************************************/
#include "hdmiscreen.h"
#include "hdmicecscreen.h"
#include "cfgsettings.h"
#include "control.h"
#include "mmessagebox.h"
#include "bsettop_os.h"

#include "bstd.h"
BDBG_MODULE(brutus_hdmiscreen);

// this comes from main.cpp which is built every time
extern const char *buildDate();

HdmiScreen::HdmiScreen(Control *acontrol) :
    GenericScreen(acontrol, NOTV), _timer(acontrol->fb()->app())
{
    int x = 0;
    int y = 0;
    new MLabel(this, MRect(x,y,700,20), MString("HDMI Tools & Config Screen                        Built on ") + buildDate());

#define HDMI_BTNWIDTH 120
#define HDMI_BTNHEIGHT 40

    //////////////////////////////////////////
    // left most column
    x = 15;
    y += HDMI_BTNHEIGHT + 10;

    //////// Color Depth Modes ///////////
    _colorDepthLabel = new MLabel(this, MRect(x,y,200,20), "Deep Color Mode");
    y += 25;
    _colorDepthMode = new MRadioButtonGroup(this, MRect(x,y,110,140));
    _colorDepthMode->add("std-8bit");
    _colorDepthMode->add("10bit");
    _colorDepthMode->add("12bit");
    // Add radio button but disable since 16bit deep color mode is not supported yet
    _colorDepthMode->add("16bit");
    _colorDepthMode->setEnabled(3,false); 

    // Get current color depth mode settings
    bdisplay_settings display_settings;
    
    control()->getOutput(&display_settings);
    if (display_settings.dvi) {
        boutput_hdmi_settings hdmi_settings;
        boutput_hdmi_get(display_settings.dvi, &hdmi_settings) ;
        _colorDepthMode->click((int)hdmi_settings.hdmi_color_depth);
    }
#ifndef HDMI_1_3_SUPPORT
    _colorDepthMode->setEnabled(false);
#endif

    //////////////////////////////////////////
    // Right most column 

#ifdef HDMI_CEC_SUPPORT 
    x += HDMI_BTNWIDTH + HDMI_BTNWIDTH + 265;
    y = HDMI_BTNHEIGHT + 10;
    _hdmiCec = new MPushButton(this, MRect(x,y,HDMI_BTNWIDTH,HDMI_BTNHEIGHT), "HDMI CEC");
    _hdmiCec->addHandler(this);
#endif

    /* Home button */
    x = 0;
    _back = new MPushButton(this, MRect(x,375,HDMI_BTNWIDTH,HDMI_BTNHEIGHT), "Home");
    _back->addHandler(this);

    /* Apply button */
    x += HDMI_BTNWIDTH + HDMI_BTNWIDTH + 280;;
    _apply = new MPushButton(this, MRect(x,375,HDMI_BTNWIDTH,HDMI_BTNHEIGHT), "Apply");
    _apply->addHandler(this);

    _timer.addHandler(this);
    _timer.start(15 * 1000, MTimer::eForever);
}

void HdmiScreen::show() {
    // This isn't in the constructor because main.cpp sets it
    // afterwards.
    GenericScreen::show();
}

void HdmiScreen::onTimeout(MTimer *self) {
    // do nothing
}

void HdmiScreen::onClick(MButton *button) {

    if (button == _back) {
        control()->showScreen(Control::eHome);
    }
    else if (button == _apply) {
        
        /* Get HDMI configurations */
        if (_colorDepthMode->enabled())
        {
            bdisplay_settings display_settings;

            control()->getOutput(&display_settings);
            if (display_settings.dvi) {
                boutput_hdmi_settings hdmi_settings;
                boutput_hdmi_get(display_settings.dvi, &hdmi_settings) ;
            
                switch (_colorDepthMode->checked()) {
                case 0: hdmi_settings.hdmi_color_depth = boutput_hdmi_color_depth_8bit; break;
                case 1: hdmi_settings.hdmi_color_depth = boutput_hdmi_color_depth_10bit; break;
                case 2: hdmi_settings.hdmi_color_depth = boutput_hdmi_color_depth_12bit; break;
                case 3: hdmi_settings.hdmi_color_depth = boutput_hdmi_color_depth_16bit; break;
                }
    
                /* Apply new settings */
                boutput_hdmi_set(display_settings.dvi, &hdmi_settings);
            }
        }
    }
    else if (button == _hdmiCec) {
        control()->showScreen(Control::eHdmiCec);
    }   
    else {
        GenericScreen::onClick(button);
    }
}
