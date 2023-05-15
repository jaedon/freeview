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
#if !defined ITK_REDKEY_H__
#define ITK_REDKEY_H__

#include "itk_types.h"

/**
 * @file 
 * RedKey Interface
 */

/**
 * @ingroup host
 * @defgroup redKey RedKey Interface
 * @{
 */

/**
 * Status of the RedKey engine 
 */
typedef enum redkey_state_t
{
    REDKEY_UNINITIALISED = 0,   /**< RedKey Module Uninitialised */
	REDKEY_STOPPED,             /**< RedKey Waiting for a command; No MHEG Application running */
	REDKEY_RUNNING,				/**< RedKey module Active; MHEG Application may/may not be running */
    REDKEY_LOST                 /**< RedKey Module Terminated */
} redkey_state_t;

/**
 * MHEG profiles available
 */
typedef enum redkey_profile_t
{
    REDKEY_PROFILE_NULL = -1,   /**< Null profile, unconfigured */
    REDKEY_PROFILE_UK_SD = 0,   /**< United Kingdom SD profile 1.06 */
    REDKEY_PROFILE_UK_HD,       /**< United Kingdom HD profile 1.06 */
	REDKEY_PROFILE_NZ_SD,       /**< New Zealand SD profile */
    REDKEY_PROFILE_NZ_HD,       /**< New Zealand HD profile */
    REDKEY_PROFILE_HK,          /**< Hong Kong profile */
    REDKEY_PROFILE_UK_FSAT,     /**< United Kingdom Freesat profile */
    REDKEY_PROFILE_CIPLUS,      /**< CI Plus Only profile */
    REDKEY_PROFILE_INTERNAL1,   /**< Profile for RedKey internal use only */
    REDKEY_PROFILE_INTERNAL2,   /**< Profile for RedKey internal use only */
    REDKEY_PROFILE_AUSTRALIA,   /**< Australia Profile */
    REDKEY_PROFILE_SOUTH_AFRICA,/**< South Africa Profile */
    REDKEY_PROFILE_INTERNAL3,   /**< Profile for RedKey internal use only */
    REDKEY_PROFILE_THAI,        /**< Thailand Profile */
    REDKEY_PROFILE_VAST,        /**< Australia VAST Profile */
    REDKEY_PROFILE_INTERNAL4,   /**< Profile for RedKey internal use only */
    REDKEY_PROFILE_INTERNAL5,   /**< Profile for RedKey internal use only */
    REDKEY_PROFILE_FREESATG2,   /**< Freesat G2 Profile */
    REDKEY_PROFILE_COUNT        /**< Enumeration to count total number of supported Profiles */
} redkey_profile_t;

/** MHEG Application state */
typedef enum redkey_mheg_app_state_t
{
    REDKEY_MHEG_APP_NOT_RUNNING = 0,    /**< An MHEG application is not running */
	REDKEY_MHEG_APP_STARTING,           /**< An MHEG application is starting up */
	REDKEY_MHEG_APP_RUNNING,	        /**< An MHEG application is running */
    REDKEY_MHEG_APP_STOPPING            /**< An running MHEG application is stopping */
} redkey_mheg_app_state_t;

/**
 * Prototype for the RedKey state callback
 *
 * The callback is invoked to deliver the state of the RedKey engine 
 * each time a change occurs.
 *
 * @see RedKey_init()
 *
 * @param[in] context The context given when the callback was defined
 * @param[in] state The new state of RedKey
 */
typedef void (*redkey_state_callback_t) (void *context, redkey_state_t state);

/**
 * Prototype for the RedKey state callback
 *
 * The callback is invoked to indicate a change in the state of any 
 * MHEG application.
 * @param[in] context The context given when the callback was defined
 * @param[in] state The new state of any MHEG application.
 */
typedef void (*redkey_mheg_app_state_callback_t) (void *context, redkey_mheg_app_state_t state);

/**
 * Initialises RedKey
 *
 * @see @ref redkey_memsize for details of the amount of memory to allocate
 *
 * @param[in] profile MHEG profile to use
 * @param[in] memSize Amount of memory available for RedKey
 * @param[in] redKeyStateCallback Callback to be invoked on changes of RedKey state
 *            This should not be NULL
 * @param[in] callbackContext Context to use for both callbacks
 *
 * @return ITKE_OK if the initialisation is successful
 * @return ITKE_FAIL if the initialisation fails, at which point RedKey can not be used
 */
ITK_EXPORT itk_errcode_t RedKey_init(redkey_profile_t profile,
                                     size_t memSize,
                                     redkey_state_callback_t redKeyStateCallback,
                                     void *redkeyStateCallbackContext, 
                                     redkey_mheg_app_state_callback_t mhegAppStateCallback,
                                     void *mhegAppStateCallbackContext);

/**
 * Terminates RedKey releasing back the resources allocated from the host.
 */
ITK_EXPORT itk_errcode_t RedKey_term(void);

/**
 * Executes an MHEG application
 *
 * Requesting an application to execute will over-ride the normal searching
 * process and will terminate any currently active application in favour of
 * the requested application.
 *
 * @param[in] object File reference of the application to execute
 * @param[in] params Optional parameter string encoded as mime type
 *                   application/x-www-form-urlencoded or NULL.
 *                   See User Guide for details.
 *
 * @return ITKE_OK   If the application launched, or an error code.
 */
ITK_EXPORT itk_errcode_t RedKey_executeApp(const itk_uint8_t *object, const itk_uint8_t *params );

/**
 * Sets args that can be accessed with GLa resident program by an MHEG application.
 *
 * Arguments take effect at next application launch follwing a ITK_SERVICE_TUNE_START event and 
 * persist until the application quits, either under application control or following a
 * ITK_SERVICE_TUNE_STOP event.
 *
 * To reset the launch args call with NULL pointer and zero length.
 *
 * @param[in] args          NULL terminated argument string
 * @param[in] argsLength    length of argument string
 */
ITK_EXPORT void RedKey_setLaunchArgs(const char *args, itk_uint16_t argsLength);

/**
 * Stops the MHEG application that was started with RedKey_executeApp
 *
 */
ITK_EXPORT void RedKey_stopApp(void);

/**
 * @}
 */
#endif
