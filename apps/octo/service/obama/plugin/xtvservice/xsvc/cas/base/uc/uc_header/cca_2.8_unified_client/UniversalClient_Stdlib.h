/*
 * Copyright 2009-2012 Irdeto B.V.
 *
 * This file and the information contained herein are the subject of copyright
 * and intellectual property rights under international convention. All rights
 * reserved. No part of this file may be reproduced, stored in a retrieval
 * system or transmitted in any form by any means, electronic, mechanical or
 * optical, in whole or in part, without the prior written permission of Irdeto
 * B.V.
 */

/**
 * @file UniversalClient_Stdlib.h
 * Header for C runtime standard library wrappers. This file contains the prototypes for all standard CRT functions 
 * that are used by the Cloaked CA Agent.
 */
#ifndef UNIVERSALCLIENT_STDLIB_H__INCLUDED__
#define UNIVERSALCLIENT_STDLIB_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "UniversalClient_API.h"

/** @defgroup stdlib Cloaked CA Agent Standard Library
 *  Cloaked CA Agent Standard Library Functions
 *
 * These functions are a direct mapping of the corresponding ANSI C runtime libraries. 
 * In cases where these functions are already available on the platform, they can be used directly.
 * On platforms where full ANSI C runtime support is not provided, they need to be implemented manually.
 *  @{
 */

/**
 * CRT strlen function
 */
uc_uint32
UniversalClient_Stdlib_strlen(
    const uc_char *pString);

/**
 * CRT strcpy function
 */
uc_char *
UniversalClient_Stdlib_strcpy(
    uc_char *pDest,
    const uc_char *pSource);

/**
 * CRT memcpy function
 */
void *
UniversalClient_Stdlib_memcpy(
    void *pDest,
    const void *pSource,
    uc_uint32 count);

/**
 * CRT memset function
 */
void *
UniversalClient_Stdlib_memset(
    void *dest,
    uc_uint8 c,
    uc_uint32 count);

/**
 * CRT memcmp function
 */
uc_sint32
UniversalClient_Stdlib_memcmp(
    const uc_uint8 *pBuf1,
    const uc_uint8 *pBuf2,
    uc_uint32 len);

/**
 * CRT srand function
 */
void 
UniversalClient_Stdlib_srand(uc_uint32 seed);

/**
 * CRT rand function
 */
uc_uint32 
UniversalClient_Stdlib_rand(void);


/**
 * CRT printf function
 *
 * @param[in] pFormat  
 *
 * \note Implementation of this requires access to varargs implementation of the core CRT method.
 */
uc_sint32
UniversalClient_Stdlib_printf(
    const uc_char *pFormat,
    ...);

/**
 * CRT sprintf function
 *
 * @param[in] buffer  
 *
 * @param[in] format  
 *
 * \note Implementation of this requires access to varargs implementation of the core CRT method.
 */
uc_uint32
UniversalClient_Stdlib_sprintf(
    uc_char *buffer,
    const uc_char *format,
    ...);

/** @} */ /* End of stdlib */
#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_STDLIB_H__INCLUDED__ */

