/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: native.h $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 11/15/07 8:25p $
 *
 * Module Description: Native header file.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/native.h $
 * 
 * Hydra_Software_Devel/8   11/15/07 8:25p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/7   9/22/07 1:57p nissen
 * PR 30062: Fixed build problem.
 * 
 * Hydra_Software_Devel/6   11/10/06 12:42p nissen
 * PR 20908: Added support for offscreen back buffer (quadruple
 * buffering).
 * 
 * Hydra_Software_Devel/5   6/5/06 12:57p nissen
 * PR 20908: Removed unnessary includes.
 * 
 * Hydra_Software_Devel/4   11/11/05 4:19p nissen
 * PR 17423: Moved VDC code from native to app.
 * 
 * Hydra_Software_Devel/3   9/22/05 6:47p nissen
 * PR 17194: Updated FrameWork support.
 * 
 * Hydra_Software_Devel/2   7/28/04 2:37p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#ifndef _NATIVE_H__
#define _NATIVE_H__

/***************************************************************************/
#define NativeDisplay   unsigned long
#define NativeWindow    unsigned long
#define NativePixmap    unsigned long

#endif /* _NATIVE_H__ */
