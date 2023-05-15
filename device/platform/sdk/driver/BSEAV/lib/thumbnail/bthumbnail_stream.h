/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthumbnail_stream.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 8/14/09 3:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/thumbnail/bthumbnail_stream.h $
 * 
 * 6   8/14/09 3:13p erickson
 * PR55994: refactor so that bthumbnail_extractor can be used standlone
 * with nexus
 *
 * 5   6/20/08 12:33p erickson
 * PR43222: impl asf/avi thumbnails
 *
 * 4   3/20/08 3:17a erickson
 * PR40307: clean up surfaces if we exceed the maximum_memory
 *
 * 3   3/20/08 1:47a erickson
 * PR40307: update
 *
 * 2   3/18/08 3:55p erickson
 * PR40307: update
 *
 * 1   3/17/08 9:22a erickson
 * PR40307: add thumbnail library
 *
 ******************************************************************/
#ifndef BTHUMBNAIL_STREAM_H__
#define BTHUMBNAIL_STREAM_H__

#include "bstd.h"
#include "bthumbnail.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bthumbnail_stream *bthumbnail_stream_t;

typedef struct bthumbnail_stream_create_settings {
    void *user_context;
    const char *name; /* used for debug. memory is not copied. */
} bthumbnail_stream_create_settings;

void bthumbnail_stream_get_default_create_settings(
    bthumbnail_stream_create_settings *create_settings /* [out] */
    );

bthumbnail_stream_t bthumbnail_stream_create(
    bthumbnail_manager_t manager,
    const bthumbnail_stream_create_settings *create_settings
    );

void bthumbnail_stream_get_create_settings(
    bthumbnail_stream_t stream,
    bthumbnail_stream_create_settings *create_settings /* [out] */
    );

void bthumbnail_stream_destroy(
    bthumbnail_stream_t stream
    );

/**
A bthumbnail_stream supports two views:
1) view_window - a subset of the stream. may have narrower spacing.
2) full_view - spans the entire stream. should have very wide spacing.
**/
typedef struct bthumbnail_stream_settings {
    unsigned first_time; /* bounds for the entire stream, in msec */
    unsigned last_time; /* bounds for the entire stream, in msec */

    struct {
        bool visible; /* is the view window being viewed on the UI right now? */
        unsigned spacing; /* in seconds */
        unsigned first_time, last_time; /* current window in stream which is being viewed */
    } view_window;

    struct {
        bool visible; /* is the full view being viewed on the UI right now? */
        unsigned number; /* number of stills to span the stream */
        unsigned min_spacing; /* in seconds */
    } full_view;
} bthumbnail_stream_settings;

void bthumbnail_stream_get_settings(
    bthumbnail_stream_t stream,
    bthumbnail_stream_settings *settings /* [out] */
    );

int bthumbnail_stream_set_settings(
    bthumbnail_stream_t stream,
    const bthumbnail_stream_settings *settings
    );

typedef struct bthumbnail_stream_data {
    unsigned total_thumbnails; /* number of thumbnails created */
} bthumbnail_stream_data;

void bthumbnail_stream_get_data(
    bthumbnail_stream_t stream,
    bthumbnail_stream_data *data /* [out] */
    );

/* get all thumbnails already available within a window */
int bthumbnail_stream_get_thumbnails(
    bthumbnail_stream_t stream,
    unsigned start_time,           /* in seconds */
    unsigned end_time,           /* in seconds */
    bthumbnail_t *thumbnail_array, /* size_is(amount) array of thumbnails */
    unsigned total,                /* maximum of thumbnails that can be retrieved */
    unsigned *actual_read          /* [out] actual number of bthumbnail_t's populated in thumbnail_array */
    );

/* get all thumbnails already available based on the "full_view" settings */
int bthumbnail_stream_get_full_view_thumbnails(
    bthumbnail_stream_t stream,
    bthumbnail_t *thumbnail_array, /* size_is(amount) array of thumbnails */
    unsigned total,                /* maximum of thumbnails that can be retrieved */
    unsigned *actual_read          /* [out] actual number of bthumbnail_t's populated in thumbnail_array */
    );

#ifdef __cplusplus
}
#endif

#endif /* BTHUMBNAIL_STREAM_H__ */

