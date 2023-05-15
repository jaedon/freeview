/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_image_debug.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 9:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_image_debug.h $
 * 
 * 1   2/7/05 9:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   9/9/04 1:52p erickson
 * PR12608: convert dbg from private impl to magnum impl
 * 
 * Irvine_BSEAVSW_Devel/3   2/3/04 5:54p bandrews
 * PR9107: Fixed bug in DBG_PRINT_RECT.
 *
 * Irvine_BSEAVSW_Devel/2   2/3/04 5:32p bandrews
 * PR9107: Fixed a problem with the debug levels.
 *
 * Irvine_BSEAVSW_Devel/1   1/30/04 4:42p bandrews
 * PR9107: BWIN Image API
 *
 ****************************************************************/

#ifndef BWIN_IMAGE_DEBUG_H__
#define BWIN_IMAGE_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DBG_ENTER(x) BDBG_MSG(("enter " #x))
#define DBG_LEAVE(x) BDBG_MSG(("leave " #x))
#define DBG_CALL(x) BDBG_MSG(("call " #x))
#define DBG_PRINT_RECT(name, rect) BDBG_MSG((#name "%d,%d,%d,%d", \
	(rect).x,(rect).y,(rect).width,(rect).height))
#define DBG_TRACE(x) BDBG_MSG(x)

#ifdef __cplusplus
}
#endif

#endif /* BWIN_IMAGE_DEBUG_H__ */

