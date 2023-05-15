/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: balloc.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/5/07 5:28p $
 *
 * Module Description:
 *
 * Interface for alloc libraries
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/utils/balloc.c $
 * 
 * 1   2/5/07 5:28p vsilyaev
 * PR 25701:Portions of stackable parsing framework
 * 
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "balloc.h"

static void *
b_kni_alloc(balloc_iface_t alloc, size_t size)
{
	BSTD_UNUSED(alloc);
	return BKNI_Malloc(size);
}

static void
b_kni_free(balloc_iface_t alloc, void *ptr)
{
	BSTD_UNUSED(alloc);
	BKNI_Free(ptr);
}

const struct balloc_iface bkni_alloc[] = {
	{b_kni_alloc, b_kni_free}
};

