/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ipstbconfigscreen.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 8/25/06 11:09a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/ipstbconfigscreen.cpp $
 * 
 * 3   8/25/06 11:09a jjordan
 * PR23128: remove managed network buffer on non-7411 chips
 * 
 * 2   3/1/06 3:25p jjordan
 * PR18981: fix "live playback" and add config option IP_USE_LIVE_PLAYBACK
 * 
 * 1   8/12/05 10:36a jjordan
 * PR16668: add IP STB config screen to IP STB GUI
 * 
 ***************************************************************************/

#include "control.h"
#include "ipstbconfigscreen.h"
#include "bstd.h"
#include "mmessagebox.h"
#include "mlabel.h"
#include "cfgsettings.h"

BDBG_MODULE(brutus_ipstbconfigscreen);

IpStbConfigScreen::IpStbConfigScreen(Control *acontrol) :
	GenericScreen(acontrol, NOTV)
{
	int x = 60-38;
	int y = 25;

	new MLabel(this, MRect(x,y,700,30), MString("IP STB Configuration Screen"));

    _control = acontrol;
	_back = new MPushButton(this, MRect(22,355,120,40), "Back");
	_back->addHandler(this);

	y += 30;
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
	_manageNetworkBuffer = new MCheckBox(this, MRect(x, y, 300, 30), "Use Managed Network Buffer");
	_manageNetworkBuffer->addHandler(this);
#endif
	_useLivePlayback = new MCheckBox(this, MRect(x, y, 300, 90), "Use Live Playback Mode");
	_useLivePlayback->addHandler(this);
}


void IpStbConfigScreen::show() {
	// This isn't in the constructor because main.cpp sets it
	// afterwards.
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
	_manageNetworkBuffer->setChecked(control()->cfg()->getBool(Config::MANAGE_IP_NETWORK_BUFFER));
#endif
	_useLivePlayback->setChecked(control()->cfg()->getBool(Config::IP_USE_LIVE_PLAYBACK));

	GenericScreen::show();
}

void IpStbConfigScreen::onClick(MButton *self) {
	if (self == _back)
	{
		control()->showScreen(Control::eAdmin);
	}
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
	else if (self == _manageNetworkBuffer) {
		control()->cfg()->set(Config::MANAGE_IP_NETWORK_BUFFER, _manageNetworkBuffer->checked()?1:0);
		MMessageBox mbox(fb(), 350, 200);
		mbox.set("This will not take effect \n until the next channel change", MMessageBox::bOK);
		mbox.setWrapMode(MLabel::NewLineWrap);
		mbox.process();
	}
#endif
	else if (self == _useLivePlayback) {
		control()->cfg()->set(Config::IP_USE_LIVE_PLAYBACK, _useLivePlayback->checked()?1:0);
		MMessageBox mbox(fb(), 350, 200);
		mbox.set("This will not take effect \n until the next channel change", MMessageBox::bOK);
		mbox.setWrapMode(MLabel::NewLineWrap);
		mbox.process();
	}
}
