/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_DEBUG
#define __NATIVE_DEBUG

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * See DAE 7.15.5
 * \defgroup Debug Debug print API
 * @{
 */

/**
   This function corresponds to the OIPF function debug() as defined in DAE 7.15.5
   \param arg As defined in DAE 7.15.5
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_debug(const char* arg);

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_DEBUG */
