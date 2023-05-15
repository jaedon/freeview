/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_outputs.h $
 * $brcm_Revision: 10 $
 * $brcm_Date: 4/19/10 3:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_outputs.h $
 * 
 * 10   4/19/10 3:34p calvinho
 * SW7420-466: Added function prototype setAspectRaio
 * 
 * 9   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 * 
 * 8   10/23/06 6:26p mward
 * PR24520: Support 480p output on 7118.  Disable features not available.
 * 
 * 7   9/6/06 2:43p erickson
 * PR21941: added output_format(--) and made -- and ++ relative to current
 * format
 * 
 * 6   9/6/06 1:22p erickson
 * PR21941: added output_format(++)
 * 
 * 5   10/10/05 3:27p erickson
 * PR17108: enable HD output selections for 7401
 * 
 * 4   8/4/05 5:24p erickson
 * PR15137: added Control event when output format changes
 * 
 * 3   3/24/05 1:21p erickson
 * PR14593: removed old platform
 * 
 * 2   3/24/05 10:44a erickson
 * PR14593: 93560 merge
 * 
 * Refsw_93560_Bringup/1   3/23/05 10:32p vsilyaev
 * 3560 fixes.
 * 
 * 1   2/7/05 8:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/7   12/14/04 7:18p bandrews
 * PR13158: 7318 support
 * 
 * Irvine_BSEAVSW_Devel/6   12/3/04 3:21p erickson
 * PR12388: vesa can have various format_settings and should be handled
 * differently
 * 
 * Irvine_BSEAVSW_Devel/5   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/4   10/8/04 10:12a erickson
 * PR12829: added static function for looking up output format, and made
 * PAL and NTSC more likely to succeed
 * 
 * Irvine_BSEAVSW_Devel/3   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 * 
 * Irvine_BSEAVSW_Devel/2   9/15/04 1:53p erickson
 * PR9683: convert from B_CHIP to BCHP_CHIP in brutus
 * 
 * Irvine_BSEAVSW_Devel/1   9/13/04 1:53p erickson
 * PR11081: added output control to infopanel
 *
 ****************************************************************************/
#ifndef INFO_OUTPUTS_H__
#define INFO_OUTPUTS_H__

#include "infobar.h"
#include "bsettop.h"
#include "control.h"

#if !((BCM_BOARD == 97118) && RNG_BOARD)
#define HD_SUPPORT
#endif

class OutputsInfoBarPlugin : public InfoBarPlugin, public ControlEventHandler {
public:
	OutputsInfoBarPlugin(InfoBar *parent);
	~OutputsInfoBarPlugin();

	static int setFormat(const char *format, bdisplay_settings *settings);
	
	/* set format by index. this makes for easy format change scripting. */
	static int setFormat(int i, bdisplay_settings *settings);
  static int setAspectRatio(const char *format, bool HD, bdisplay_settings *settings);
 	static int getFormatIndex(const bdisplay_settings *settings);
	static int totalFormats();

	void onControlEvent(Control *control, Control::Event event);
protected:
	MLabel *_panel;
};

#endif // INFO_OUTPUTS_H__
