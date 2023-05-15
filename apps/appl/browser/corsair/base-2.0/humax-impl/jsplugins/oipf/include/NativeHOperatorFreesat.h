/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef	CONFIG_OP_FREESAT

#ifndef __NATIVE_HUMAX_OPERATORFREESAT
#define __NATIVE_HUMAX_OPERATORFREESAT

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   \defgroup Configuration Configuration and Settings
   @{

   This is the native interface for the Configuration and setting APIs
   as defined in DAE 7.3.
 */

#ifdef OIPF

#endif /* OIPF */

/*------------------------------------------
  Configuration
  ------------------------------------------*/
/** @name Configuration
	The Configuration class as defined in DAE 7.3.2
	@{*/



OOIFReturnCode native_HGetPostCode(const char **retval);
OOIFReturnCode native_HSetPostCode(const char *retval);
OOIFReturnCode native_HGetBouquetName(const char **retval);
OOIFReturnCode native_HSetBouquetName(const char *retval);
OOIFReturnCode native_HGetSiMode(OOIFNumber *retval);
OOIFReturnCode native_HSetSiMode(OOIFNumber retval);
OOIFReturnCode native_HGetBouquetId(OOIFNumber *retval);
OOIFReturnCode native_HGetRegionCode(OOIFNumber *retval);
OOIFReturnCode native_HGetAdultChannels(OOIFBoolean *retval);
OOIFReturnCode native_HSetAdultChannels(OOIFBoolean retval);
OOIFReturnCode native_HSetMHEGOperationControl(OOIFBoolean retval);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HUMAX_CONFIGURATION */
#endif

