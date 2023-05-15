/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ipstbconfigscreen.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/1/06 3:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/ipstbconfigscreen.h $
 * 
 * 2   3/1/06 3:25p jjordan
 * PR18981: fix "live playback" and add config option IP_USE_LIVE_PLAYBACK
 * 
 * 1   8/12/05 10:37a jjordan
 * PR16668: add IP STB config screen to IP STB GUI
 * 
 ***************************************************************************/

#include "genericscreen.h"
#include "mbutton.h"

class IpStbConfigScreen : public GenericScreen {
public:
	IpStbConfigScreen(Control *control);

	// events
	void onClick(MButton *self);

	// overrides
	void show();

protected:
    Control *_control;
	MPushButton *_back;
	MCheckBox *_manageNetworkBuffer;
	MCheckBox *_useLivePlayback;
};
