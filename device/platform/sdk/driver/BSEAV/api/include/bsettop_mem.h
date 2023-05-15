/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_mem.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 1/8/08 10:18a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_mem.h $
 * 
 * 2   1/8/08 10:18a vsilyaev
 * PR 35854: Use special type to hide 'void *' from the proxy layer
 * 
 * 1   11/26/07 5:23p vsilyaev
 * PR 35854: Memory management interface
 * 
 *
 *
 ***************************************************************************/

#ifndef BSETTOP_MEM_H__
#define BSETTOP_MEM_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* This typedef works around an issue with the auto-generated proxy layer. */
typedef void * bmem_ptr;

/*
Summary:
Cached memory allocation function
Description:
This allocates a buffer on the current heap and in cached memory for use elsewhere.
*/
bmem_ptr bmem_alloc(
	size_t buffer_length
);

/*
Summary:
Memory de-allocation for external use
Description:
This frees a buffer allocated with bsettop_mem_alloc()
*/
void bmem_free(
	void *buffer
);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_MEM_H__ */
