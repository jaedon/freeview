/***************************************************************************
 *     Copyright (c) 2002-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_decodepage.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 8/25/05 4:18p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_decodepage.h $
 * 
 * 2   8/25/05 4:18p erickson
 * PR16819: make DecodePage responsive to Control Start/StopDecode events,
 * and handle analog and no decode explicitly
 * 
 * 1   2/7/05 8:08p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/2   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 * 
 * Irvine_BSEAVSW_Devel/1   4/10/03 4:22p erickson
 * info panel rework
 * added edit channels and outputs save-state feature
 *
 ****************************************************************************/
#ifndef INFO_DECODEPAGE_H__
#define INFO_DECODEPAGE_H__

#include "infopanel.h"
#include "control.h"

/* DecodePage requires immediate updates when decode starts/stops or context is switched.
But it also needs timed updates for srcsize changes.
TODO: bdecode_attach/detach_audio doesn't result in a Control event */
class DecodePage : public InfoPanelStatusPage, public ControlEventHandler {
public:
	DecodePage(InfoPanel *infoPanel, const MRect &rect);
	void update() {read();}
	
	void read();
	void onControlEvent(Control *control, Control::Event event);
};

#endif //INFO_DECODEPAGE_H__
