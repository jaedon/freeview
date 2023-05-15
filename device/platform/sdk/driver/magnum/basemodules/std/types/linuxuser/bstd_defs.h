/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bstd_defs.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 4/27/12 11:36a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/std/types/linuxuser/bstd_defs.h $
 * 
 * Hydra_Software_Devel/6   4/27/12 11:36a erickson
 * SW7425-2954: add #define __STDC_CONSTANT_MACROS for c++
 * 
 * Hydra_Software_Devel/5   7/19/10 8:52a erickson
 * SW7405-4526: add #include unistd.h
 * 
 * Hydra_Software_Devel/4   6/21/10 1:23p erickson
 * SW7405-4526: remove ssize_t from linuxuser/bstd_types.h. it is already
 * defined in unistd.h.
 *
 * Hydra_Software_Devel/3   6/10/10 1:37p vsilyaev
 * SW7468-69: Fixed Win32/MINGW build
 *
 * Hydra_Software_Devel/2   9/9/03 12:48p erickson
 * added BSTD_UNUSED macro
 *
 * Hydra_Software_Devel/1   3/20/03 3:25p erickson
 * added linuxuser
 *
 * Hydra_Software_Devel/1   3/5/03 5:17p erickson
 * Initial linux bstd_defs.h
 *
 ***************************************************************************/
#ifndef BSTD_DEFS_H__
#define BSTD_DEFS_H__

#ifdef __cplusplus
/* required for UINT32_C used in berr.h */
#define __STDC_CONSTANT_MACROS
#endif

#include <iso646.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

#define BSTD_UNUSED(x) ((void)x)

#endif /* BSTD_DEFS_H__ */

