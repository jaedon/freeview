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
#if !defined ITK_H__
#define ITK_H__

/**
 * @file 
 *
 * iTK initialisation and host registration
 */
#include "itk_types.h"

/**
 * @defgroup host Interactive TV Kit API
 */

/**
 * @ingroup host
 * @defgroup conf Kit configuration
 * @{
 */

/**
 * Managers provided by the platform
 */
typedef enum itk_manager_t
{
    ITK_DEMUX = 1,   /**< Demux and SI Tables */
    ITK_DISPLAY,     /**< Display stack */
    ITK_MISC,        /**< Miscellaneous host functions */
    ITK_HTTP,        /**< HTTP client */
    ITK_NVS,         /**< Non-volatile storage */
    ITK_OS,          /**< Operation System resources */
    ITK_DVR,         /**< DVR control */
    ITK_SERVICE,     /**< Service management */
    ITK_ZLIB,        /**< Zlib decompression */
    ITK_FONT,        /**< External font driver */
    ITK_CRYPTO,      /**< Cryptographic interface */
    ITK_APPMMI,      /**< Application MMI interface */
    ITK_REMINDER,    /**< Reminder interface */
    ITK_MANAGER_LAST /**< This is not a manager. Do not register. */
} itk_manager_t;

/**
 * Initialises the Interactive TV Kit
 *
 * This must be called by the host <em>after</em> registering its managers.
 *
 * @return ITKE_OK if the initialisation is successful
 * @return ITKE_FAIL if the initialisation fails, at which point the iTK can not
 *         be used further
 */
ITK_EXPORT itk_errcode_t ITK_init(void);

/**
 * Terminates the Interactive TV Kit
 *
 * This must be the last call made by the host
 */
ITK_EXPORT void ITK_term(void);

/**
 * Registers a resource manager
 *
 * @param[in] type The type of manager the host wants to register
 * @param[in] manager A pointer to the manager interface
 */
ITK_EXPORT void ITK_registerManager(itk_manager_t type,  void *manager);

/**
 * Gets a resource manager
 *
 * @param[in] type The type of manager the host wants to get
 *
 * @return A pointer to the manager, NULL if it is not registered
 */
ITK_EXPORT void *ITK_getManager(itk_manager_t type);


/**
 * Requests the iTK version number
 *
 * The string format is "X.Y.Z" followed by '\0' character, where X, Y and Z are 
 * alphanumeric characters representing the iTK Major, Minor and Update versions, 
 * respectively. These digits contains '0'-'9' and 'A'-'Z'. The host 
 * supplies the buffer along with an indicator of the size of the buffer, 
 * and the client writes the version string into the buffer.
 * The buffer shall be big enough to store the string.
 *
 * @param[in] buffer Pointer to a buffer to hold the version string
 * @param[in] bufferLength Length of the buffer. Minimum length of the buffer is 6 bytes
 */
ITK_EXPORT void ITK_getVersionInfo(char *buffer, size_t bufferLength);

/**
 * @}
 */
#endif
