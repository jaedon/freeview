/*
 * Copyright (c) 2007 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */
#if !defined ITK_TYPES_H__
#define ITK_TYPES_H__
/**
 * @file
 *
 * Global types
 */

/**
 * @ingroup host
 * @defgroup types Global types
 * @{
 *
 * The Interactive TV Kit general types
 */



/**
 * All module exports are strict C
 */
#if defined __cplusplus
#define ITK_EXPORT extern "C"
#else
#define ITK_EXPORT extern
#endif

/**
 * From stdlib we allow size_t
 */
#include <stdlib.h>

/**
 * Standard sized integer types
 */

/**
 * Unsigned byte
 */
typedef unsigned char itk_uint8_t;

/**
 * Signed byte
 */
typedef signed char itk_int8_t;

/**
 * Unsigned short
 */
typedef unsigned short itk_uint16_t;

/**
 * Signed short
 */
typedef signed short itk_int16_t;

/**
 * Define sizes of 32-bit integer types depending on platform
 */
#if defined ITK_DEFINE_INT32_AS_INT
/**
 * Unsigned int
 */
typedef unsigned int itk_uint32_t;

/**
 * Signed int
 */
typedef signed int itk_int32_t;
#else
/**
 * Unsigned long
 */
typedef unsigned long itk_uint32_t;

/**
 * Signed long
 */
typedef signed long itk_int32_t;
#endif

/**
 * iTK-specific types
 */

/**
 * Points are used for locations on a graphic device
 */
typedef struct itk_point_t
{
    itk_int32_t x;
    itk_int32_t y;
} itk_point_t;

/**
 * Rectangles are defined by their top-left point, width and height.
 * For simplicity all fields are signed, but negative widths and
 * heights are unlikely to be correct.
 */
typedef struct itk_rect_t
{
    itk_int32_t x;
    itk_int32_t y;
    itk_int32_t w;
    itk_int32_t h;
} itk_rect_t;

/**
 * Colours are defined by r,g,b,t components.
 *
 * @note The alpha is defined as a transparency. This is the opposite
 * model to the standard definition of alpha. Hence a value trans = 0
 * means opaque, trans = 255 is fully transparent
 */
typedef struct itk_colour_t
{
    itk_uint8_t red;   /**< Red component */
    itk_uint8_t green; /**< Green component */
    itk_uint8_t blue;  /**< Blue component */
    itk_uint8_t trans; /**< Transparency component */
} itk_colour_t;

/**
 * NSAP is the unique service identifier, defined by the Original Network Id,
 * Network Id, Transport Stream Id and Service Id.
 */
typedef struct itk_nsap_t
{
    itk_uint16_t onid; /**< Original Network Id */
    itk_uint16_t nid;  /**< Network Id */
    itk_uint16_t tsid; /**< Transport Stream Id */
    itk_uint16_t sid;  /**< Service Id */
} itk_nsap_t;

/**
 * The maximum number of entries in a palette.
 * @note This means 16bpp palette is not supported.
 */
#define ITK_PALETTE_SIZE 256

/**
 * A palette table
 */
typedef struct itk_palette_t
{
    size_t entry_size;
    itk_colour_t entry[ITK_PALETTE_SIZE]; /**< An array of colours */
} itk_palette_t;

/**
 * Error codes. The following error codes should be used for all
 * APIs. Their semantics may change depending on the context.
 */
typedef enum itk_errcode_t
{
    ITKE_OK = 0,        /**< The call was successful */
    ITKE_FAIL,          /**< Generic failure */
    ITKE_NOT_SUPPORTED, /**< The requested call is not supported by this implementation */
    ITKE_NOT_READY,     /**< The action can not be completed due to a resource being tied up */
    ITKE_NOT_FOUND,     /**< The resource could not be located */
    ITKE_NO_RESOURCE,   /**< The target does not have sufficient resources to complete the action */
    ITKE_TIMEOUT        /**< The operation has not completed within a defined timeout */
} itk_errcode_t;

/**
 * Generic definitions used for parameters in API calls
 */

/**
 * Enable and disable items
 */
typedef enum itk_control_t
{
    ITK_DISABLE = 0,
    ITK_ENABLE
} itk_control_t;

/**
 * Wait or don't wait for items
 */
typedef enum itk_wait_t
{
    ITK_NO_WAIT = 0,
    ITK_WAIT
} itk_wait_t;

/**
 * Indicates if an item is present or absent
 */
typedef enum itk_present_t
{
    ITK_NOT_PRESENT = 0,
    ITK_PRESENT
} itk_present_t;

/**
 * Indicates if an item is valid or invalid
 */
typedef enum itk_validation_t
{
    ITK_NOT_VALID = 0,
    ITK_VALID
} itk_validation_t;

/**
 * @}
 */
#endif
