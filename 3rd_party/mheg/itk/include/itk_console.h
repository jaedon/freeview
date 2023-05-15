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
#if !defined ITK_CONSOLE_H__
#define ITK_CONSOLE_H__

#include "itk_types.h"
/**
 * @file 
 * Debug and monitoring console
 */

/**
 * @ingroup host
 * @defgroup console Console I/O
 * @{
 */

/**
 * Sends a message to the console output
 *
 * @param [in] fmt The message to be sent, in printf() style.
 */
ITK_EXPORT void ITK_consoleMessage(const char *fmt, ...);

/**
 * Indicates to the host that a fatal error has occurred in the iTK.
 * It is not expected that this function returns, instead that a
 * reboot or other recovery device is required.
 *
 * @param [in] fmt A message describing the error, in printf() style.
 */
ITK_EXPORT void ITK_fatalError(const char *fmt, ...);

/**
 * The prototype for a SIMON function implementation
 *
 * The argv strings are each terminated with a NIL ('\\0') character.
 *
 * @param[in] context The context provided when the callback was registered
 * @param[in] argc The number of argument strings provided
 * @param[in] argv A pointer to an array[argc] of parameter strings. argv[0]
 *                 is the command name 
 */
typedef void (*itk_simon_callback_t)(void *context, int argc, char **argv);

/**
 * Adds a SIMON function to the host's monitor
 *
 * @param[in] cmd A command name
 * @param[in] callback A callback function to invoke when this command is required
 * @param[in] callbackContext A context to return when the callback is invoked
 * @param[in] helpStr A string (without '\\n') that describes the operation of the
 *                    command
 *
 * @return ITKE_OK if the command was added to the list
 * @return ITKE_FAIL if the commands could not be registered
 */
ITK_EXPORT itk_errcode_t ITK_addSimonCmd(const char *cmd,
                                         itk_simon_callback_t callback, 
                                         void *callbackContext, 
                                         const char *helpStr);

/**
 * Removes a SIMON function from the host's monitor
 *
 * @param[in] cmd The command name
 */
ITK_EXPORT void ITK_removeSimonCmd(const char *cmd);

/**
 * Outputs a message from a SIMON function
 *
 * @note SIMON command implementations must use ITK_sendSimon rather
 *       than ITK_consoleMessage to allow the host to route the
 *       output differently if desired.
 *
 * @param [in] fmt The message to be sent, in printf() style.
 */
ITK_EXPORT void ITK_sendSimon(const char *fmt, ...);

/**
 * @}
 */
#endif
