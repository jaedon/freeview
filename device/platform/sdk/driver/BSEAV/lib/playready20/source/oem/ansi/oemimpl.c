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
#include <drmcommon.h>
#include <drmcrt.h>
#include <drmcmdlnpars.h>
#ifdef PRDY_ROBUSTNESS
#include "nexus_random_number.h"
#endif
ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
** Platform Initialization functions 
***********************************************************************
*/

DRM_API DRM_RESULT DRM_CALL Oem_Platform_Init()
{
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_Platform_UnInit()
{
    return DRM_SUCCESS;
}


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
DRM_API DRM_VOID * DRM_CALL Oem_MemAlloc(
    __in DRM_DWORD f_cbSize )
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
DRM_API DRM_VOID DRM_CALL Oem_MemFree(
    __in DRM_VOID *f_pv )
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
DRM_API DRM_VOID* DRM_CALL Oem_MemRealloc(
    __inout DRM_VOID *f_pv, 
    __in DRM_DWORD f_cbSize )
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

/**********************************************************************
** Random number generation functions 
***********************************************************************
*/
#ifndef PRDY_ROBUSTNESS
static       DRM_UINT64 g_qwRandomSeed     =  DRM_UI64LITERAL( 0x25B946EB, 0xC0B36173); /* default value */   
static       DRM_BOOL   g_fSeedInitialized =  FALSE;
static const DRM_UINT64 RANDOM_MULTIPLIER  =  DRM_UI64LITERAL( 0x2B992DDF, 0xA23249D5 ); /* odd */ 
static const DRM_UINT64 INIT_MULTIPLIER    =  DRM_UI64LITERAL( 0,          14142135 );
#endif

/**********************************************************************
** Function:    Oem_Random_SetSeed
**
** Synopsis:    Manually set the seed for the random number generator. 
**              The seed is multiplied by INIT_MULTIPLIER before use
**
** Arguments:   [f_pOEMContext] -- OEM specified context
**              [f_qwNewSeed]   -- The new seed value
**
** Returns:     None
**
***********************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_Random_SetSeed (
    __in_opt DRM_VOID *f_pOEMContext,
    __in const DRM_UINT64  f_qwNewSeed )
{
	#ifndef PRDY_ROBUSTNESS
    g_qwRandomSeed     = DRM_UI64Mul( INIT_MULTIPLIER, f_qwNewSeed );
    g_fSeedInitialized = TRUE;
	#else
    return;
	#endif
}

#define PACKED_CHAR_BIT ( CHAR_BIT / CB_NATIVE_BYTE )

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
DRM_API DRM_RESULT DRM_CALL Oem_Random_GetBytes( 
    __in_opt DRM_VOID *f_pOEMContext,
    __out_bcount( f_cbData ) DRM_BYTE  *f_pbData, 
    __in DRM_DWORD  f_cbData )
{
    const DRM_DWORD SHIFT_COUNT = PACKED_CHAR_BIT * ( SIZEOF( DRM_UINT64 ) - SIZEOF( DRM_DWORD ) );
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_DWORD   cbBytesLeft  = f_cbData;
    DRM_DWORD   cbBytesToCopy = f_cbData;
    DRM_DWORD   cbCurrent    = 0;
    uint8_t *pOffset = f_pbData;
    NEXUS_RandomNumberOutput random_nb;
    NEXUS_RandomNumberGenerateSettings settings;

    ChkArg( f_pbData != NULL );

    if(f_cbData % 16 != 0){
        cbBytesLeft += 16 - (f_cbData % 16);
    }

    while ( cbBytesLeft != 0 )
    {
        // Generate random number
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

        // Copy the password in the return buffer
        if(cbBytesToCopy < random_nb.size){
            MEMCPY(pOffset, random_nb.buffer, cbBytesToCopy);
        }
        else{
            MEMCPY(pOffset, random_nb.buffer, random_nb.size);
        }
        cbBytesToCopy -= random_nb.size;

        pOffset += random_nb.size;

    }

ErrorExit:
    return dr;
}
#else
DRM_API DRM_RESULT DRM_CALL Oem_Random_GetBytes( 
    __in_opt DRM_VOID *f_pOEMContext,
    __out_bcount( f_cbData ) DRM_BYTE  *f_pbData, 
    __in DRM_DWORD  f_cbData )
{
    const DRM_DWORD SHIFT_COUNT = PACKED_CHAR_BIT * ( SIZEOF( DRM_UINT64 ) - SIZEOF( DRM_DWORD ) );
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_DWORD   cbBytesLeft  = f_cbData;
    DRM_DWORD   ib           = 0;
    DRM_DWORD   cbCurrent    = 0;
    DRM_DWORD   dwValue      = 0;
    DRMFILETIME ft           = { 0 };
    DRM_UINT64  u64          = DRM_UI64LITERAL( 0, 0 );

    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );

    /*
    ** Set random seed the first time, unless it has been manually set using
    ** Oem_Random_SetSeed
    */
    if ( !g_fSeedInitialized )
    {
        Oem_Clock_GetSystemTimeAsFileTime( f_pOEMContext, &ft );
        FILETIME_TO_UI64( ft, u64 );
        Oem_Random_SetSeed( f_pOEMContext, u64 );
    }
    while ( cbBytesLeft != 0 )
    {
        cbCurrent = min( cbBytesLeft, SIZEOF( DRM_DWORD ) );
        cbBytesLeft -= cbCurrent;
        g_qwRandomSeed = DRM_UI64Add( DRM_UI64Mul( g_qwRandomSeed, RANDOM_MULTIPLIER ), DRM_UI64( 1 ) );
        dwValue = DRM_I64ToUI32( DRM_UI2I64( DRM_UI64ShR( g_qwRandomSeed, SHIFT_COUNT ) ) );
        do 
        {
            ChkBOOL( ib < f_cbData, DRM_E_ARITHMETIC_OVERFLOW ); 

            PUT_BYTE( f_pbData, ib, ( DRM_BYTE )( dwValue & UCHAR_MAX ) );
            ib++;
            dwValue >>= PACKED_CHAR_BIT;
            cbCurrent--;
        } while ( cbCurrent != 0 );
    }
    
ErrorExit:
    return dr;
} 
#endif

/**********************************************************************
** Function:    Oem_GetCurrentThreadId
**
** Synopsis:    Function that returns the current thread ID.
**              
**
** Arguments:   N/A
**
** Returns:     The ID of the current thread.
**
***********************************************************************/
DRM_API DRM_DWORD DRM_CALL Oem_GetCurrentThreadId( DRM_VOID )
{
    /* Return a fixed number as thread ID. */
    return 1;
}

EXIT_PK_NAMESPACE_CODE;
