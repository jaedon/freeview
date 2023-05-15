/*
 * Copyright (c) 2008 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */
#if !defined ITK_ZLIB_H__
#define ITK_ZLIB_H__

#include "itk_types.h"
#include <stdlib.h>

/**
 * iTK interface to zlib compression.
 *
 * The iTK uses zlib compression as defined by RFC 1950
 * (http://www.ietf.org/rfc/rfc1950.txt). Currently the iTK only requires
 * an implementation of the inflate function.
 *
 * The standard zlib implementation is available as a C library, from
 * http://zlib.net. iTK has been developed and tested using version 1.2.3 of
 * zlib.
 */

/** Inflate return struct */
typedef struct {

    /** ITKE_OK on success, ITKE_NO_RESOURCE for a memory allocation
        problem, ITKE_FAIL for any other failure. */
    itk_errcode_t error;
    
    itk_uint32_t src_read;    /**< bytes read from the source buffer */
    itk_uint32_t dst_written; /**< bytes written to the destination buffer */
} itk_zlib_inflate_ret_t;

/**
 * iTK zlib memory allocation function.
 * @return Pointer to size bytes of memory on success, NULL otherwise.
 */
typedef void * (* itk_zlib_alloc_fn_t)(void * context, size_t size);

/**
 * iTK zlib memory release function. Frees memory successfully returned by the
 * allocate function.
 */
typedef void (* itk_zlib_free_fn_t)(void * context, void * ptr);

/**
 * Zlib inflator interface.
 */
typedef struct itk_zlib_inflator_t {
    /**
     * Release a zlib interface object and all associated resources.
     * @param thiz The zlib inflator.
     */
    void (* release)(struct itk_zlib_inflator_t * thiz);

    /**
     * Inflate compressed source data.
     * @param thiz The zlib inflator.
     * @param src The source data, to be inflated.
     * @param src_size The size in bytes of this data.
     * @param dst The resulting inflated data.
     * @param dst_size The size in bytes of the dst buffer.
     * @return The inflate return information.
     */
    itk_zlib_inflate_ret_t (* zinflate)(struct itk_zlib_inflator_t * thiz,
                                       itk_uint8_t const * src, size_t src_size,
                                       itk_uint8_t * dst, size_t dst_size);
} itk_zlib_inflator_t;

/**
 * Zlib interface.
 */
typedef struct itk_zlib_t {
    /**
     * Create a new Zlib inflator.
     * @param thiz The zlib interface object.
     * @param alloc Allocator for zlib to use.
     * @param free Memory free function for zlib to use.
     * @param context Context passed to alloc/free functions.
     * @return The new inflator, on success, otherwise NULL.
     */
    itk_zlib_inflator_t * (* newInflator)(struct itk_zlib_t * thiz,
                                          itk_zlib_alloc_fn_t alloc,
                                          itk_zlib_free_fn_t free,
                                          void * context);
} itk_zlib_t;

#endif // ITK_ZLIB_H__
