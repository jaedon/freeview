/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_misc.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_misc.h $
 * 
 * 1   2/7/05 8:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/6   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/5   9/13/04 1:54p erickson
 * PR11081: refactored info panel controls to be more minimal, support
 * more options
 *
 * Irvine_BSEAVSW_Devel/4   5/18/04 2:59p bandrews
 * PR10148: Removed global image and replaced with global string
 *
 * Irvine_BSEAVSW_Devel/3   2/13/04 1:54p erickson
 * PR8987: infobar now reflects current display format
 *
 * Irvine_BSEAVSW_Devel/2   12/16/03 12:25a erickson
 * PR8731: POD ui work
 *
 * Irvine_BSEAVSW_Devel/1   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 *
 ****************************************************************************/
#ifndef INFO_MISC_H__
#define INFO_MISC_H__

#include "infopanel.h"

/*=******************************
This module contains simple infobar plugins.
Currently, it only has the "exit" plugin which hides the infobar.
*********************************/

/* Must have an Exit button in order to get off of the info panel
using frontpanel only when there's no info button (e.g. 7320) */
class ExitInfoBarPlugin : public InfoBarPlugin {
public:
	ExitInfoBarPlugin(InfoBar *parent);
	~ExitInfoBarPlugin();
	bool keyDown(int ascii, int modifiers, int scancode);
};

#endif //INFO_MISC_H__

