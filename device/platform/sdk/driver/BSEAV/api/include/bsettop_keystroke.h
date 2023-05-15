/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_keystroke.h
 * $brcm_Revision: 
 * $brcm_Date: 
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_keystroke.h $
 * 
 * 2   2/17/05 1:10p ywu
 * PR 13748: Create an IR keystrokes automation utility
 * 
 * Irvine_BSEAVSW_Devel/2   1/12/05 2:47p erickson
 * PR13748: fixed minor compile warnings
 * 
 * Irvine_BSEAVSW_Devel/1   1/5/05 12:37p ywu
 * PR 13748: Create an IR keystrokes automation utility
 * bsettop_keystroke.c/h
 * 
 * Irvine_BSEAVSW_Devel/PR13748   1/3/05 12:00pm Ella
 * Initial version for keystrokes automation. Has been verified in Kylin
 *
 ***************************************************************************/

#ifndef BSETTOP_KEYSTROKE_H__
#define BSETTOP_KEYSTROKE_H__


#ifdef __cplusplus
extern "C"
{
#endif


bresult buser_input_keystrokes_open(
	buser_input_t ui, 
	bool playback,
	const char *file,
	const char *stressfile);

void buser_input_keystrokes_close(
	buser_input_t ui);

void buser_input_keystrokes_set_settings(
	buser_input_t ui,
	const buser_input_settings *settings);

bresult buser_input_keystrokes_get_event(
	buser_input_t ui, /* user input object */
	buser_input_event *event, /* [out,size_is(nevents)] event array from the user */
	unsigned nevents, /* number of entries in the event array */
	unsigned *result_nevents /* [out] number of entries read */
	);


#ifdef __cplusplus
}
#endif


#endif
