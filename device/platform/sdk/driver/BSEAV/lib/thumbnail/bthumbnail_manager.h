/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthumbnail_manager.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 8/14/09 3:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/thumbnail/bthumbnail_manager.h $
 * 
 * 4   8/14/09 3:13p erickson
 * PR55994: refactor so that bthumbnail_extractor can be used standlone
 * with nexus
 *
 * 3   3/20/08 3:17a erickson
 * PR40307: clean up surfaces if we exceed the maximum_memory
 *
 * 2   3/18/08 3:55p erickson
 * PR40307: update
 *
 * 1   3/17/08 9:22a erickson
 * PR40307: add thumbnail library
 *
 ******************************************************************/
#ifndef BTHUMBNAIL_MANAGER_H__
#define BTHUMBNAIL_MANAGER_H__

#include "bstd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
The bthumbnail library has no internal synchronization. You must not call it re-entrantly.
You must not call bthumbnail from a bthumbnail callback.

The bthumbnail library depends on:
    bstd.h (from Magnum)
    bmedia_player
    stdio

All thumbnail decoding, destriping and scaling is handled by means of callbacks.
**/

typedef struct bthumbnail_manager *bthumbnail_manager_t;

typedef struct bthumbnail_manager_settings {
    unsigned maximum_memory;    /* bthumbnail_manager will free up the least-necessary thumbnails when this is exceeded */

    unsigned compressed_buffer_size; /* size of maximum uncompressed still picture */

    void (*request_ready)(void *context); /* notify the app when it should call bthumbnail_manager_get_next_request */
    void (*delete_surface)(void *context, void *surface); /* request application to delete a void *surface which was passed into bthumbnail_manager_set_surface */
    void *callback_context;
} bthumbnail_manager_settings;

void bthumbnail_manager_get_default_settings(
    bthumbnail_manager_settings *settings /* [out] */
    );

bthumbnail_manager_t bthumbnail_manager_create(
    const bthumbnail_manager_settings *settings
    );

void bthumbnail_manager_destroy(
    bthumbnail_manager_t manager
    );

/**
The application must ask the manager for the next bthumbnail_t that requires a still decode.
The application should use bthumbnail_extractor decode the still, then report the results by calling bthumbnail_manager_set_surface.

Call bthumbnail_get_data to learn stream and codec information.

If it returns non-zero, then there is no bthumbnail_t that needs to be decoded. App can wait for request_ready callback
before calling again.
**/
int bthumbnail_manager_get_next_request(
    bthumbnail_manager_t manager,
    bthumbnail_t *thumbnail /* [out] */
    );

/**
Give the bsurface_t which resulted from the last call to bthumbnail_manager_get_next_request.
bthumbnail_manager will now own the bsurface_t and will deallocate it when it wants to.
The application should not remember the bthumbnail_t as it could be deallocated.
**/
int bthumbnail_manager_set_surface(
    bthumbnail_manager_t manager,
    bthumbnail_t thumbnail,
    void *surface, /* application-defined surface handle. see bthumbnail_manager_settings.delete_surface. */
    unsigned size  /* size of the surface */
    );

/**
Summary:
Make all streams invisible.

Description:
A UI can use this to make all streams invisible, then make visible the specific streams it wants.
**/
void bthumbnail_manager_clear_visible(
    bthumbnail_manager_t manager
    );

#ifdef __cplusplus
}
#endif

#endif /* BTHUMBNAIL_MANAGER_H__ */

