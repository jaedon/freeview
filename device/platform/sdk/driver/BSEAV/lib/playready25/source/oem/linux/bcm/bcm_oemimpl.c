/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#ifdef PRDY_ROBUSTNESS
#include "nexus_random_number.h"
#include "bcm_oem.h"
#include "bkni.h"
#endif
#if 0
#include "common_crypto.h"
#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_security.h"
#endif
/**********************************************************************
** Memory allocation functions 
***********************************************************************
*/

/**********************************************************************
** Function:    Oem_MemAlloc
**
** Synopsis:    Allocates memory from heap. 
**
** Arguments:   [f_cbSize] Number of bytes to allocate.
**
** Returns:     Valid pointer if succeeds, NULL if failed. 
**
***********************************************************************/
DRM_VOID * BCM_Oem_MemAlloc(
    DRM_DWORD f_cbSize )
{
    if ( f_cbSize == 0 )
    {
        return NULL;
    }
#if SIXTEEN_BIT_ADDRESSING
    DRM_DWORD cb = f_cbSize / CB_NATIVE_BYTE;
    
    if ( ( f_cbSize % CB_NATIVE_BYTE ) != 0 )
    {
        cb++;
    }
    
	#ifdef PRDY_ROBUSTNESS
    return BKNI_Malloc( cb );
	#else
    return malloc( cb );
	#endif
#else
#ifdef PRDY_ROBUSTNESS
    return (DRM_VOID *) BKNI_Malloc( f_cbSize );
#else
    return malloc( f_cbSize );
#endif    
#endif    
}

/**********************************************************************
** Function:    Oem_MemFree
**
** Synopsis:    Frees allocated memory. 
**
** Arguments:   [f_pv] Pointer to memory buffer.
**
** Returns:     None
**
***********************************************************************/
DRM_VOID BCM_Oem_MemFree(
    DRM_VOID *f_pv )
{
#ifdef PRDY_ROBUSTNESS
    BKNI_Free( f_pv );
#else
    free( f_pv );
#endif
}

/**********************************************************************
** Function:    Oem_MemRealloc
**
** Synopsis:    Re-allocates memory from heap. 
**
** Arguments:   [f_pv]     Pointer to previous allocated memory buffer.
**              [f_cbSize] Number of bytes to allocate.
**
** Returns:     Valid pointer if succeeds, NULL if failed. 
**
***********************************************************************/
DRM_VOID* BCM_Oem_MemRealloc(
    DRM_VOID *f_pv, 
    DRM_DWORD f_cbSize )
{
#if SIXTEEN_BIT_ADDRESSING
    DRM_DWORD cb = f_cbSize / CB_NATIVE_BYTE;
    
    if ( ( f_cbSize % CB_NATIVE_BYTE ) != 0 )
    {
        cb++;
    }

    if( f_pv != NULL )
    {
#ifdef PRDY_ROBUSTNESS
        DRM_VOID *pBuf;
        pBuf = (DRM_VOID *)BKNI_Malloc(cb);
        BKNI_Memcpy(pBuf, f_pv, cb);
        BKNI_Free(f_pv);
        return pBuf;
#else
        return realloc( f_pv, cb );
#endif
    }
    
    return malloc( cb );
#else
    if( f_pv != NULL )
    {
#ifdef PRDY_ROBUSTNESS
        DRM_VOID *pBuf;
        pBuf = (DRM_VOID *)BKNI_Malloc(f_cbSize);
        BKNI_Memcpy(pBuf, f_pv, f_cbSize);
        BKNI_Free(f_pv);
        return pBuf;
#else
        return realloc( f_pv, f_cbSize );
#endif
    }
#ifdef PRDY_ROBUSTNESS
    return (DRM_VOID *)BKNI_Malloc( f_cbSize );
#else
    return malloc( f_cbSize );
#endif
#endif
}

#define PACKED_CHAR_BIT ( 8 / 1 )

/**********************************************************************
** Function:    Oem_Random_GetBytes
**
** Synopsis:    Generate pseudo-random array of bytes using an OEM provided 
**              random number generator. Initializes the random seed if it has 
**              not already been set using Oem_Random_SetSeed or a previous call
**              to Oem_Random_GetBytes.
**
** Arguments:   [f_pOEMContext] -- OEM specified context
**              [f_pbData]      -- Buffer to hold the random bytes
**              [f_cbData]      -- Count of bytes to generate and fill in pbData
**
** Returns:     DRM_SUCCESS
**                  Success
**              DRM_E_ARITHMETIC_OVERFLOW
**                  An arithmetic overflow occurred during processing.
**
** Notes:       This generator has not been designed to be fast, nor to have
**              good cryptographic or statistical properties. It is not designed
**              for multitasking.
***********************************************************************/
#ifdef PRDY_ROBUSTNESS
DRM_RESULT BCM_Oem_Random_GetBytes( 
    DRM_VOID *f_pOEMContext,
    DRM_BYTE  *f_pbData, 
    DRM_DWORD  f_cbData )
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_DWORD   cbBytesLeft  = f_cbData;
    DRM_DWORD   cbBytesToCopy = f_cbData;
    DRM_DWORD   cbCurrent    = 0;
    uint8_t *pOffset = f_pbData;
    NEXUS_RandomNumberOutput random_nb;
    NEXUS_RandomNumberGenerateSettings settings;

	(void)(f_pOEMContext);
    ChkArg( f_pbData != NULL );

    if(f_cbData % 16 != 0){
        cbBytesLeft += 16 - (f_cbData % 16);
    }

    while ( cbBytesLeft != 0 )
    {
        /*/ Generate random number */
        NEXUS_RandomNumber_GetDefaultGenerateSettings(&settings);

        cbCurrent = min( cbBytesLeft, NEXUS_MAX_RANDOM_NUMBER_LENGTH);
        cbBytesLeft -= cbCurrent;

        settings.type = NEXUS_RandomNumberType_eRngRaw;
        settings.destination = NEXUS_RandomNumberDestination_eHost;
        settings.randomNumberSize = cbCurrent;

        if(NEXUS_RandomNumber_Generate( &settings, &random_nb) != NEXUS_SUCCESS)
        {
            ChkDR(DRM_E_CRYPTO_FAILED);
        }

        if(random_nb.size != settings.randomNumberSize) ChkDR(DRM_E_CRYPTO_FAILED);

        /* Copy the password in the return buffer */
        if(cbBytesToCopy < random_nb.size){
            memcpy(pOffset, random_nb.buffer, cbBytesToCopy);
        }
        else{
            memcpy(pOffset, random_nb.buffer, random_nb.size);
        }
        cbBytesToCopy -= random_nb.size;

        pOffset += random_nb.size;

    }

ErrorExit:
    return dr;
}
#endif

