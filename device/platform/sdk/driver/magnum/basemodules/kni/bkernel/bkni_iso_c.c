/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_iso_c.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/20/09 5:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/bkernel/bkni_iso_c.c $
 * 
 * Hydra_Software_Devel/1   11/20/09 5:24p jgarrett
 * SW7408-17: Adding bkernel
 * 
 * Hydra_Software_Devel/4   3/11/03 9:54p vsilyaev
 * support for  new bkni.h .
 * 
 * Hydra_Software_Devel/3   3/10/03 8:28p vsilyaev
 * Added support for the tagged kernet interface API.
 * 
 * Hydra_Software_Devel/2   3/10/03 6:47p vsilyaev
 * Fix compilation problem for release build.
 * 
 * Hydra_Software_Devel/1   3/10/03 1:17p vsilyaev
 * Implementation of the kernel interface functions for ISO C 89
 * complatible environment.
 *
 ***************************************************************************/


#include "bstd.h"
#include "bkni.h"

/* needed to support tagged interface */
#undef BKNI_Malloc
#undef BKNI_Free

#include <string.h>
#include <stdlib.h>

void *
BKNI_Memset(void *b, int c, size_t len)
{
    return memset(b, c, len);
}

void *
BKNI_Memcpy(void *dst, const void *src, size_t len)
{
    return memcpy(dst, src, len);
}

int 
BKNI_Memcmp(const void *b1, const void *b2, size_t len)
{
    return memcmp(b1, b2, len);
}

void *
BKNI_Memchr(const void *b, int c, size_t len)
{
    return memchr(b, c, len);

}

void *
BKNI_Memmove(void *dst, const void *src, size_t len)
{
    return memmove(dst, src, len);
}

void *
BKNI_Malloc(size_t size)
{
    return malloc(size);
}

void 
BKNI_Free(void *ptr)
{
    free(ptr);
    return;
}

