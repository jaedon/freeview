/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: balloc.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/12/08 3:07p $
 *
 * Module Description:
 *
 * Interface for alloc libraries
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/utils/balloc.h $
 * 
 * 3   6/12/08 3:07p vsilyaev
 * PR 43607: Renamed alloc and free to bmem_alloc and bmem_free
 * 
 * 2   3/15/07 10:50a vsilyaev
 * PR 25701: Added missing 'extern'
 * 
 * 1   2/5/07 5:28p vsilyaev
 * PR 25701:Portions of stackable parsing framework
 * 
 * 
 *******************************************************************************/
#ifndef __BALLOC_H__
#define __BALLOC_H__ 

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct balloc_iface const *balloc_iface_t;

struct balloc_iface {
	void *(*bmem_alloc)(balloc_iface_t alloc, size_t size);
	void (*bmem_free)(balloc_iface_t alloc, void *ptr);
};

/* BKNI based allocator */
extern const struct balloc_iface bkni_alloc[1];
#ifdef __cplusplus
}
#endif

#endif /* __BALLOC_H__ */

