/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_save_settings.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:57p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/bsettop_save_settings.h $
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
#ifndef BSETTOP_SAVE_SETTINGS_H__
#define BSETTOP_SAVE_SETTINGS_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	bdisplay_settings settings;
	boutput_component_settings component_settings;
	boutput_rf_settings rf_settings;
	boutput_dvi_settings dvi_settings;
} bdisplay_extended_settings;

/**
Summary:
Save not only the bdisplay_settings but also the individual boutput_XXX settings
for each output which is connect.
**/
void bdisplay_save_settings(bdisplay_extended_settings *save, const bdisplay_settings *settings);

/**
Summary:
Restore settings captured with bdisplay_save_settings.

Description:
This is only guaranteed to work if you don't move resources to another display between
save and restore.
**/
void bdisplay_restore_settings(bdisplay_settings *settings, const bdisplay_extended_settings *save);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_SAVE_SETTINGS_H__ */
