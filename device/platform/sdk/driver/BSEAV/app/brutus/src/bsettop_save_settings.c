/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_save_settings.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:57p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/bsettop_save_settings.c $
 * 
 * 1   2/7/05 7:57p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   6/2/04 2:44p erickson
 * PR11204: changed/removed deprecated settop api functions
 *
 * Irvine_BSEAVSW_Devel/1   8/19/03 4:38p erickson
 * fixed outputs screen (rf video select not working)
 *
 ***************************************************************************/
#include "bsettop.h"
#include "bsettop_save_settings.h"

void bdisplay_save_settings(bdisplay_extended_settings *save, const bdisplay_settings *settings)
{
	save->settings = *settings;
	if (settings->component)
		boutput_component_get(settings->component, &save->component_settings);
	if (settings->rf)
		boutput_rf_get(settings->rf, &save->rf_settings);
	if (settings->dvi)
		boutput_dvi_get(settings->dvi, &save->dvi_settings);
}

void bdisplay_restore_settings(bdisplay_settings *settings, const bdisplay_extended_settings *save)
{
	*settings = save->settings;
	if (settings->component)
		boutput_component_set(settings->component, &save->component_settings);
	if (settings->rf)
		boutput_rf_set(settings->rf, &save->rf_settings);
	if (settings->dvi)
		boutput_dvi_set(settings->dvi, &save->dvi_settings);
}
