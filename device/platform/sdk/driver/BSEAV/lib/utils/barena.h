/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: barena.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/14/07 5:46p $
 *
 * Module Description:
 *
 * Arena alloc library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/utils/barena.h $
 * 
 * 2   2/14/07 5:46p vsilyaev
 * PR 25701: Added code to dump content/status of allocator
 * 
 * 1   2/5/07 5:29p vsilyaev
 * PR 25701:Portions of stackable parsing framework
 * 
 * 
 *******************************************************************************/
#ifndef _BARENA_H__
#define _BARENA_H__
#include "balloc.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct barena *barena_t;
barena_t barena_create(balloc_iface_t alloc, size_t arena_size);
void barena_destroy(barena_t arena);
balloc_iface_t barena_alloc_iface(barena_t arena);

void *barena_alloc(barena_t arena, size_t size);
void barena_free(barena_t arena, void *ptr);
bool barena_test_block(barena_t arena, void *ptr);

void barena_join(barena_t parent, barena_t child);
barena_t barena_last_child(barena_t arena);
void barena_detach(barena_t parent, barena_t child);
void barena_dump(barena_t arena);


#ifdef __cplusplus
}
#endif

#endif /* __BARENA_H__ */


