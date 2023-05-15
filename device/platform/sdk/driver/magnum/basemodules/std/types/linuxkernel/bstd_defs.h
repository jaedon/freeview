/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bstd_defs.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 4/8/09 4:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/std/types/linuxkernel/bstd_defs.h $
 * 
 * Hydra_Software_Devel/6   4/8/09 4:23p vsilyaev
 * PR 54015: Don't include bkni_multi.h into the bdbg.h. All thread-aware
 * modules should include explicitly bkni_multi.h
 * 
 * Hydra_Software_Devel/5   7/30/08 9:01a erickson
 * PR45177: uintptr_t is now defined in linux 2.6.18-5.1 header files
 * 
 * Hydra_Software_Devel/3   4/8/04 9:53p vsilyaev
 * PR 10508: Added missing types
 * 
 * Hydra_Software_Devel/2   9/9/03 12:48p erickson
 * added BSTD_UNUSED macro
 * 
 * Hydra_Software_Devel/1   3/5/03 5:17p erickson
 * Initial linux bstd_defs.h
 *
 ***************************************************************************/
#ifndef BSTD_DEFS_H__
#define BSTD_DEFS_H__

#include <linux/types.h>

#define UINT8_C(c)	c ## U
#define UINT16_C(c)	c ## U
#define UINT32_C(c)	c ## U
#define UINT64_C(c)	c ## ULL

#define INT8_C(c)	c
#define INT16_C(c)	c
#define INT32_C(c)	c
#define INT64_C(c)	c ## LL


#define  INT8_MIN (-128)
#define  INT8_MAX ( 127)
#define UINT8_MAX ( 255)

#define  INT16_MIN (-32768)
#define  INT16_MAX ( 32767)
#define UINT16_MAX ( 65535)

#define  INT32_MIN (-2147483648L)
#define  INT32_MAX ( 2147483647L)
#define UINT32_MAX ( 4294967295UL)


#include <stdbool.h>
#include <stdarg.h>

#define BSTD_UNUSED(x) ((void)x)

/* in linux with non preemptive kernel we shall be safe without locking debug interface */
#define BDBG_LOCK()   
#define BDBG_UNLOCK() 

#endif /* BSTD_DEFS_H__ */

