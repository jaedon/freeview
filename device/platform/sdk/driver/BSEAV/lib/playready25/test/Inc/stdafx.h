/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_TEST_STDAFX_H
#define __DRM_TEST_STDAFX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/*
** test headers
*/
#include <tclib.h>
#include <tstutils.h>
#include <tstHDSutils.h>
#include <tstXMLutils.h>
#include <tLicGen.h>
#include <tResponsegen.h>
#include <tGetClientID.h>
#include <tOEMIMP.h>
#include <tOEMIMP_PD.h>
#include <drmtestresults.h>
#include <drmchkerr.h>
#include <teststubcerts.h>

/*
** Required DRM headers
*/
#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmcmdlnpars.h>
#include <drmmanager.h>
#include <drmallstrings.h>
#include <PKCommonTestAPI.h>
#include <drmtestrsautils.h>

#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE;

#define APP_CONTEXT_VAR "SharedAppContext"
#define DRMBUFFER_VAR "SharedOpaqueBuffer"
#define DEV_STORE_VAR "SharedDeviceStore"

#define TEST_DRM_BUFFER_SIZE MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE


/*
** --------------------------------------
** GET_SHARED_* macros
** --------------------------------------
*/

#define GET_SHARED_APP_CONTEXT_NOFAIL( pApp ) do {                                                                                          \
        dr = DRM_TST_GetTestVar( APP_CONTEXT_VAR, ( DRM_VOID ** ) &pApp, NULL );                                                            \
        if( dr == DRM_E_TEST_VARIABLE_NOTFOUND )                                                                                            \
        {                                                                                                                                   \
            pApp = NULL;                                                                                                                    \
            dr = DRM_SUCCESS;                                                                                                               \
        }                                                                                                                                   \
        ChkDR( dr );                                                                                                                        \
    }                                                                                                                                       \
    while ( FALSE )

#define GET_SHARED_APP_CONTEXT( pApp ) do {                                                                                                 \
        ChkDR( DRM_TST_GetTestVar( APP_CONTEXT_VAR, ( DRM_VOID ** ) &pApp, NULL ) );                                                        \
    }                                                                                                                                       \
    while ( FALSE )

#define GET_SHARED_DRMBUFFER( pbOpaqueBuffer ) do {                                                                                         \
        ChkDR( DRM_TST_GetTestVar( DRMBUFFER_VAR, ( DRM_VOID ** ) &pbOpaqueBuffer, NULL ) );                                                \
    }                                                                                                                                       \
    while ( FALSE )

#define GET_SHARED_DEVICE_STORE( pDeviceStore ) do {                                                                                        \
        ChkDR( DRM_TST_GetTestVar( DEV_STORE_VAR, ( DRM_VOID ** ) &pDeviceStore, NULL ) );                                                  \
    }                                                                                                                                       \
    while ( FALSE )


/*
** --------------------------------------
** INIT_* macros
** --------------------------------------
*/
    
#define INIT_VARIABLE_BY_NAME( pVar, name, cb ) do {                                                                                        \
        dr = DRM_TST_GetTestVar( name, ( DRM_VOID ** ) &pVar, NULL );                                                                       \
        if ( dr == DRM_E_TEST_VARIABLE_NOTFOUND )                                                                                           \
        {                                                                                                                                   \
            ChkDR( DRM_TST_AllocTestVar( name,                                                                                              \
                                         cb,                                                                                                \
                                         ( DRM_VOID ** ) &pVar,                                                                             \
                                         NULL ) );                                                                                          \
            ZEROMEM( pVar, cb );                                                                                                            \
        }                                                                                                                                   \
        ChkDR( dr );                                                                                                                        \
    }                                                                                                                                       \
    while ( FALSE )

#define INIT_STORE_BY_NAME( pDeviceStore, szDevStoreNameVar ) do {                                                                          \
        INIT_VARIABLE_BY_NAME( pDeviceStore, szDevStoreNameVar, SIZEOF( DRM_CONST_STRING ) );                                               \
        {                                                                                                                                   \
            DRM_WCHAR __macro_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};                                                          \
            DRM_WCHAR *__macro_pwszDeviceStore = NULL;                                                                                      \
                                                                                                                                            \
            dr = DRM_SUCCESS;                                                                                                               \
                                                                                                                                            \
            FREEDRMSTRINGPOINTER( pDeviceStore );                                                                                           \
            ChkDR( tGetDeviceStorePathname( __macro_wszPathName, pDeviceStore ) );                                                          \
            ChkOverflow( pDeviceStore->cchString + 1, pDeviceStore->cchString );                                                            \
            ChkOverflow( SIZEOF( DRM_WCHAR ) * ( pDeviceStore->cchString + 1 ), pDeviceStore->cchString + 1 );                              \
            ChkMem( __macro_pwszDeviceStore = ( DRM_WCHAR * ) Oem_MemAlloc( SIZEOF( DRM_WCHAR ) * ( pDeviceStore->cchString + 1 ) ) );      \
            MEMCPY( __macro_pwszDeviceStore, pDeviceStore->pwszString, SIZEOF( DRM_WCHAR ) * pDeviceStore->cchString );                     \
            __macro_pwszDeviceStore[ pDeviceStore->cchString ] = g_wchNull;                                                                 \
            pDeviceStore->pwszString = __macro_pwszDeviceStore;                                                                             \
                                                                                                                                            \
        }                                                                                                                                   \
    }                                                                                                                                       \
    while ( FALSE )

#define INIT_SHARED_STORE( pDeviceStore ) INIT_STORE_BY_NAME( pDeviceStore, DEV_STORE_VAR )

#define INIT_APP_CONTEXT_BY_NAME_NO_DRMINIT( pApp, szAppVar, pbOpaqueBuffer, szOpaqueBufferVar, pDeviceStore, szDevStoreNameVar ) do {      \
        INIT_VARIABLE_BY_NAME( pApp, szAppVar, SIZEOF( DRM_APP_CONTEXT ) );                                                                 \
        INIT_VARIABLE_BY_NAME( pbOpaqueBuffer, szOpaqueBufferVar, TEST_DRM_BUFFER_SIZE );                                                   \
        INIT_STORE_BY_NAME( pDeviceStore, szDevStoreNameVar );                                                                              \
    }                                                                                                                                       \
    while ( FALSE )

#define INIT_SHARED_APP_CONTEXT_NO_DRMINIT( pApp, pbOpaqueBuffer, pDeviceStore ) do {                                                       \
        INIT_VARIABLE_BY_NAME( pApp, APP_CONTEXT_VAR, SIZEOF( DRM_APP_CONTEXT ) );                                                          \
        INIT_VARIABLE_BY_NAME( pbOpaqueBuffer, DRMBUFFER_VAR, TEST_DRM_BUFFER_SIZE );                                                       \
        INIT_SHARED_STORE( pDeviceStore );                                                                                                  \
    }                                                                                                                                       \
    while ( FALSE )

#define INIT_APP_CONTEXT_BY_NAME( pApp, szAppVar, szOpaqueBufferVar, pDeviceStore, szDevStoreNameVar ) do {                                 \
        DRM_BYTE *__macro_pbOpaqueBuffer = NULL;                                                                                            \
        INIT_APP_CONTEXT_BY_NAME_NO_DRMINIT( pApp, szAppVar, __macro_pbOpaqueBuffer, szOpaqueBufferVar, pDeviceStore, szDevStoreNameVar );  \
        ChkDR( TestDrmInitializeWithActivation( pApp, NULL, __macro_pbOpaqueBuffer, TEST_DRM_BUFFER_SIZE, pDeviceStore ) );                 \
    }                                                                                                                                       \
    while ( FALSE )

#define INIT_APP_CONTEXT_BY_NAME_NO_DEV_STORE( pApp, szAppVar, szOpaqueBufferVar, szDevStoreNameVar ) do {                                  \
        DRM_CONST_STRING *__macro_pwszDeviceStoreName = NULL;                                                                               \
        INIT_APP_CONTEXT_BY_NAME( pApp, szAppVar, szOpaqueBufferVar, __macro_pwszDeviceStoreName, szDevStoreNameVar );                      \
    }                                                                                                                                       \
    while ( FALSE )

#define INIT_APP_CONTEXT_BY_NAME_NO_DEV_STORE_WITH_REV( pApp, szAppVar, szOpaqueBufferVar, szDevStoreNameVar, szRevBufferVar ) do {         \
        DRM_BYTE *__macro_pbRevocationBuffer = NULL;                                                                                        \
        INIT_APP_CONTEXT_BY_NAME_NO_DEV_STORE( pApp, szAppVar, szOpaqueBufferVar, szDevStoreNameVar );                                      \
        MAP_TO_GLOBAL_TEST_VAR_Alloc( __macro_pbRevocationBuffer, szRevBufferVar, REVOCATION_BUFFER_SIZE );                                 \
        ZEROMEM( __macro_pbRevocationBuffer, REVOCATION_BUFFER_SIZE );                                                                      \
        ChkDR( Drm_Revocation_SetBuffer( pApp, __macro_pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );                                      \
    }                                                                                                                                       \
    while ( FALSE )

#define INIT_SHARED_APP_CONTEXT( pApp, pDeviceStore ) do {                                                                                  \
        DRM_BYTE *__macro_pbOpaqueBuffer = NULL;                                                                                            \
        INIT_SHARED_APP_CONTEXT_NO_DRMINIT( pApp, __macro_pbOpaqueBuffer, pDeviceStore );                                                   \
        ChkDR( TestDrmInitializeWithActivation( pApp, NULL, __macro_pbOpaqueBuffer, TEST_DRM_BUFFER_SIZE, pDeviceStore ) );                 \
    }                                                                                                                                       \
    while ( FALSE )


/*
** --------------------------------------
** FREE_* macros
** --------------------------------------
** These macros must NOT contain any ChkDR() [or similar] function calls, because these macros are often used
** in both ErrorExit clauses and post-test-case cleanup functions.  Doing so could skip cleanup operations.
*/
        
#define FREE_APP_CONTEXT_BY_NAME( pApp, szAppVar, szOpaqueBufferVar, pDeviceStore, szDevStoreNameVar ) do {                                 \
        DRM_BYTE *__macro_pbOpaqueBuffer = NULL;                                                                                            \
        if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( szOpaqueBufferVar, ( DRM_VOID ** ) &__macro_pbOpaqueBuffer, NULL ) ) )                     \
        {                                                                                                                                   \
            (DRM_VOID)DRM_TST_FreeTestVar( szOpaqueBufferVar );                                                                             \
        }                                                                                                                                   \
        if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( szAppVar, ( DRM_VOID ** ) &pApp, NULL ) ) )                                                \
        {                                                                                                                                   \
            (DRM_VOID)TestScanDevicePrivateKey( pApp );                                                                                     \
            Drm_Uninitialize( pApp );                                                                                                       \
            (DRM_VOID)DRM_TST_FreeTestVar( szAppVar );                                                                                      \
        }                                                                                                                                   \
        if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( szDevStoreNameVar, ( DRM_VOID ** ) &pDeviceStore, NULL ) ) )                               \
        {                                                                                                                                   \
            FREEDRMSTRINGPOINTER( pDeviceStore );                                                                                           \
            (DRM_VOID)DRM_TST_FreeTestVar( szDevStoreNameVar );                                                                             \
        }                                                                                                                                   \
    }                                                                                                                                       \
    while ( FALSE )

#define FREE_APP_CONTEXT_BY_NAME_NO_DEV_STORE( pApp, szAppVar, szOpaqueBufferVar, szDevStoreNameVar ) do {                                  \
        DRM_CONST_STRING *__macro_pwszDeviceStoreName = NULL;                                                                               \
        FREE_APP_CONTEXT_BY_NAME( pApp, szAppVar, szOpaqueBufferVar, __macro_pwszDeviceStoreName, szDevStoreNameVar                         \
    } while ( FALSE )

#define FREE_APP_CONTEXT_BY_NAME_NO_DEV_STORE_OR_APP( szAppVar, szOpaqueBufferVar, szDevStoreNameVar ) do {                                 \
        DRM_APP_CONTEXT  *__macro_pApp                = NULL;                                                                               \
        DRM_CONST_STRING *__macro_pwszDeviceStoreName = NULL;                                                                               \
        FREE_APP_CONTEXT_BY_NAME( __macro_pApp, szAppVar, szOpaqueBufferVar, __macro_pwszDeviceStoreName, szDevStoreNameVar );              \
    } while ( FALSE )

#define FREE_SHARED_APP_CONTEXT( pApp, pDeviceStore )   FREE_APP_CONTEXT_BY_NAME( pApp, APP_CONTEXT_VAR, DRMBUFFER_VAR, pDeviceStore, DEV_STORE_VAR )


/*
** --------------------------------------
** Miscellaneous macros
** --------------------------------------
*/
        
#define HASARG(idx) ( ( argc > idx ) && ( DRMCRT_strlen( argv[idx] ) > 0 ) )

#define DRM_TEST_EXPECT_ERROR( hr, hrExpected )     \
    do {                                            \
        if( hr != hrExpected )                      \
        {                                           \
            ChkDR( hr );                            \
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );  \
        }                                           \
    } while( FALSE );

#define DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( hr, hrExpected )  \
    do {                                                            \
        if( hr != hrExpected )                                      \
        {                                                           \
            ChkDR( hr );                                            \
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );                  \
        }                                                           \
        hr = DRM_SUCCESS;                                           \
    } while( FALSE );


#define POSTTESTCASE_FUNCTION_CALL( Func )                          \
    do {                                                            \
        DRM_RESULT drLocal = DRM_SUCCESS;                           \
        drLocal = (Func);                                           \
        if ( DRM_FAILED( drLocal ) )                                \
        {                                                           \
            Log( "", "\t%s FAILED, dr = 0x%x", #Func, drLocal );    \
            dr = DRM_E_TEST_CLEANUP_FAIL;                           \
        }                                                           \
    } while ( FALSE );


EXIT_PKTEST_NAMESPACE;

#endif /* __DRM_TEST_STDAFX_H */
