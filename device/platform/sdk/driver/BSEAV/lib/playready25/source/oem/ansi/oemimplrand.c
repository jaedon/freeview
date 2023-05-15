/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMIMPLRAND_C
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <drmcrt.h>
#include <drmcmdlnpars.h>
#include <drmerr.h>
#include <drmbytemanip.h>
#include <oemcommon.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;


/**********************************************************************
** Random number generation functions
***********************************************************************
*/
static       DRM_UINT64 g_qwRandomSeed     =  DRM_UI64LITERAL( 0x25B946EB, 0xC0B36173); /* default value */
static       DRM_BOOL   g_fSeedInitialized =  FALSE;
static const DRM_UINT64 RANDOM_MULTIPLIER  =  DRM_UI64LITERAL( 0x2B992DDF, 0xA23249D5 ); /* odd */
static const DRM_UINT64 INIT_MULTIPLIER    =  DRM_UI64LITERAL( 0,          14142135 );

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
		g_qwRandomSeed	   = DRM_UI64Mul( INIT_MULTIPLIER, f_qwNewSeed );
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
	return BCM_Oem_Random_GetBytes(f_pOEMContext, f_pbData, f_cbData);
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

EXIT_PK_NAMESPACE_CODE;
