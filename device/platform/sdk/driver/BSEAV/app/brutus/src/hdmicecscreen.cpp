/***************************************************************************
 *     (c)2007-2008 Broadcom Corporation
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
 * $brcm_Workfile: hdmicecscreen.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 5/11/09 7:41p $
 *
 * Module Description: HDMI CEC gui screen for BRUTUS demo app
 *		
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/hdmicecscreen.cpp $
 * 
 * 2   5/11/09 7:41p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 * 
 * PR53373_DeepColor/1   4/16/09 3:57p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 * 
 * 1   2/23/09 6:13p vle
 * PR 48785: Add HDMI CEC screen to BRUTUS
 * Merge to main branch
 * 
 * HDMI_TX_Plugfest11_200811/1   11/7/08 4:56p vle
 * PR48785: Add HDMI CEC screen to BRUTUS demo app
 * 
 ***************************************************************************/

#include "hdmicecscreen.h"
#include "cfgsettings.h"
#include "control.h"
#include "mmessagebox.h"
#include "bsettop_os.h"

#include "bstd.h"
BDBG_MODULE(brutus_hdmicecscreen);

// this comes from main.cpp which is built every time
extern const char *buildDate();

HdmiCecScreen::HdmiCecScreen(Control *acontrol) :
	GenericScreen(acontrol, NOTV), _timer(acontrol->fb()->app())
{
	int x = 60-38;
	int y = 10;

    new MLabel(this, MRect(x,y,700,30), MString("HDMI CEC Screen                        Built on ") + buildDate());

#define HDMICEC_BTNWIDTH 170
#define HDMICEC_BTNHEIGHT 40

	//////////////////////////////////////////
	// first column
	x = 25;
    y += HDMICEC_BTNHEIGHT+10;
	
	_imageViewOn = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "Image View On");
	_imageViewOn->addHandler(this);
	
	y += HDMICEC_BTNHEIGHT+10;
	_textViewOn = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "Text View On");
	_textViewOn->addHandler(this);

	y += HDMICEC_BTNHEIGHT+10;
	_activeSource = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "Active Source");
	_activeSource->addHandler(this);

	y += HDMICEC_BTNHEIGHT+10;
	_inactiveSource = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "Inactive Source");
	_inactiveSource->addHandler(this);

	y += HDMICEC_BTNHEIGHT+10;
	_oneTouchPlay = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "One Touch Play");
	_oneTouchPlay->addHandler(this);

	
	//////////////////////////////////////////
	// second column
	x += HDMICEC_BTNWIDTH + 35;
	y = 10+HDMICEC_BTNHEIGHT+10;

	_givePhysicalAddress = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "GivePhysAddr");
	_givePhysicalAddress->addHandler(this);

	y += HDMICEC_BTNHEIGHT+10;
	_reportPhysicalAddress = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "RprtPhysAddr");
	_reportPhysicalAddress->addHandler(this);

	y += HDMICEC_BTNHEIGHT+10;
	_setMenuLanguage = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "SetMenuLang");
	_setMenuLanguage->addHandler(this);

	y += HDMICEC_BTNHEIGHT+10;
	_cecVersion= new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "CEC Version");
	_cecVersion->addHandler(this);


	//////////////////////////////////////////
	// third column
	x += HDMICEC_BTNWIDTH + 35;
	y = 10+HDMICEC_BTNHEIGHT+10;

	_setOSDName = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "SetOSDName");
	_setOSDName->addHandler(this);

	y += HDMICEC_BTNHEIGHT+10;	
	_setOSDString = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "SetOSDString");
	_setOSDString->addHandler(this);
	
	y += HDMICEC_BTNHEIGHT+10;	
	_giveDevicePowerStatus = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "GiveDevPwrStat");
	_giveDevicePowerStatus->addHandler(this);

	y += HDMICEC_BTNHEIGHT+10;
	_reportPowerStatus= new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "RprtPwrStat");
	_reportPowerStatus->addHandler(this);

	y += HDMICEC_BTNHEIGHT+10;	
	_standby = new MPushButton(this, MRect(x,y,HDMICEC_BTNWIDTH,HDMICEC_BTNHEIGHT), "Standby");
	_standby->addHandler(this);


	/* Back button */
	x = 60-38;
	_back = new MPushButton(this, MRect(x,375,120,HDMICEC_BTNHEIGHT), "Back");
	_back->addHandler(this);

	_timer.addHandler(this);
	_timer.start(15 * 1000, MTimer::eForever);
}

void HdmiCecScreen::show() {
	// This isn't in the constructor because main.cpp sets it
	// afterwards.
	GenericScreen::show();
}

void HdmiCecScreen::onTimeout(MTimer *self) {
	// do nothing
}

void HdmiCecScreen::onClick(MButton *button) {

	/* TODO: Find the logical address of the attached HDMI device 
		Assume connecting to an HDMI TV (logical address 0) for now */
	control()->hdmiCecMessageData()->initiator_addr = 0;
	
	if (button == _back)
		control()->showScreen(Control::eHdmi);

	else if (button == _imageViewOn) {
		control()->hdmiCecImageViewOnMessage((void *) control());
		MMessageBox::info(fb(), "<Image View On> message sent");
	}
	else if (button == _textViewOn) {
		control()->hdmiCecTextViewOnMessage((void *) control());
		MMessageBox::info(fb(), "<Text View On> message sent");
	}
	else if (button == _activeSource) {
		control()->hdmiCecActiveSourceMessage((void *) control());
		MMessageBox::info(fb(), "<Active Source> message sent");
	}
	else if (button == _inactiveSource) {
		control()->hdmiCecInactiveSourceMessage((void *) control());
		MMessageBox::info(fb(), "<Inactive Source> message sent");		
	}
	else if (button == _oneTouchPlay) {
		control()->hdmiCecOneTouchPlay((void *) control());
		MMessageBox::info(fb(), "One Touch Play feature enabled");		
	}

	
	else if (button == _givePhysicalAddress) {
		control()->hdmiCecGivePhysicalAddressMessage((void *) control());
		MMessageBox::info(fb(), "<Give Physical Address> message sent");
	}
	else if (button == _reportPhysicalAddress) {
		control()->hdmiCecReportPhysicalAddressMessage((void *) control());
		MMessageBox::info(fb(), "<Report Physical Address> message sent");
	}
	else if (button == _setMenuLanguage) {
		control()->hdmiCecSetMenuLanguageMessage((void *) control());
		MMessageBox::info(fb(), "<Set Menu Language> message sent");
	}
	else if (button == _cecVersion)	{
		control()->hdmiCecVersionMessage((void *) control());
		MMessageBox::info(fb(), "<CEC Version> message sent");
	}

	
	else if (button == _setOSDName) {
		control()->hdmiCecSetOSDNameMessage((void *) control());
		MMessageBox::info(fb(), "<Set OSD Name> message sent");
	}
	else if (button == _setOSDString) {
		control()->hdmiCecSetOSDStringMessage((void *) control());
		MMessageBox::info(fb(), "<Set OSD String> message sent");
	}
	else if (button == _giveDevicePowerStatus) {
		control()->hdmiCecGiveDevicePowerStatusMessage((void *) control());
		MMessageBox::info(fb(), "<Give Device Power Status> message sent");		
	}
	else if (button == _reportPowerStatus) {
		control()->hdmiCecReportPowerStatusMessage((void *) control());
		MMessageBox::info(fb(), "<Report Power Status> message sent");		
	}
	else if (button == _standby) {
		control()->hdmiCecStandbyMessage((void *) control());
		MMessageBox::info(fb(), "<Standby> message sent");				
	}
	
	
	else {
		GenericScreen::onClick(button);
	}

}

