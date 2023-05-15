/***************************************************************************
 *     Copyright (c) 1997-2001, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bargs_set.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/g1/bargs_set.h $
 * 
 * 1   2/7/05 7:37p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   9/11/03 10:29a erickson
 * added some output, format and pos options
 *
 ***************************************************************************/
#ifndef BARGS_SET_H__
#define BARGS_SET_H__

#include "bsettop.h"

#ifdef __cplusplus
extern "C" {
#endif

void bargs_print_options();
void bargs_set_outputs(const char *output, bdisplay_settings *settings);
void bargs_set_format(const char *format, bdisplay_settings *settings);

#ifdef __cplusplus
}
#endif

#endif /* BARGS_SET_H__ */
