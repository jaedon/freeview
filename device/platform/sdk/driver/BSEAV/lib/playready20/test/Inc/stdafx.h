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
#include "tclib.h"
#include "tstutils.h"
#include "tstHDSutils.h"
#include "tstXMLutils.h"
#include "tlicgen.h"
#include "tResponsegen.h"
#include "tgetclientID.h"
#include "tOEMImp.h"
#include "tOEMImp_PD.h"
#include "drmtestresults.h"
#include "drmchkerr.h"
#include "teststubcerts.h"

/*
** Required DRM headers
*/
#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmcmdlnpars.h>
#include <drmmanager.h>
#include "PKCommonTestAPI.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE


#define APP_CONTEXT_VAR "SharedAppContext"
#define DRMBUFFER_VAR "SharedOpaqueBuffer"
#define DEV_STORE_VAR "SharedDeviceStore"

#define TEST_DRM_BUFFER_SIZE MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE

#define GET_SHARED_APP_CONTEXT( pApp ) do {                                                                                             \
        ChkDR( DRM_TST_GetTestVar( APP_CONTEXT_VAR, ( DRM_VOID ** ) &pApp, NULL ) );                                                    \
    }                                                                                                                                   \
    while ( FALSE )

#define GET_SHARED_DRMBUFFER( pbOpaqueBuffer ) do {                                                                                     \
        ChkDR( DRM_TST_GetTestVar( DRMBUFFER_VAR, ( DRM_VOID ** ) &pbOpaqueBuffer, NULL ) );                                            \
    }                                                                                                                                   \
    while ( FALSE )

#define GET_SHARED_DEVICE_STORE( pDeviceStore ) do {                                                                                    \
        ChkDR( DRM_TST_GetTestVar( DEV_STORE_VAR, ( DRM_VOID ** ) &pDeviceStore, NULL ) );                                              \
    }                                                                                                                                   \
    while ( FALSE )

#define INIT_VARIABLE_BY_NAME( pVar, name, cb ) do {                                                                                    \
        dr = DRM_TST_GetTestVar( name, ( DRM_VOID ** ) &pVar, NULL );                                                                   \
        if ( dr == DRM_E_TEST_VARIABLE_NOTFOUND )                                                                                       \
        {                                                                                                                               \
            ChkDR( DRM_TST_AllocTestVar( name,                                                                                          \
                                         cb,                                                                                            \
                                         ( DRM_VOID ** ) &pVar,                                                                         \
                                         NULL ) );                                                                                      \
        }                                                                                                                               \
        ChkDR( dr );                                                                                                                    \
    }                                                                                                                                   \
    while ( FALSE )

#define INIT_SHARED_STORE( pDeviceStore ) do {                                                                                          \
        INIT_VARIABLE_BY_NAME( pDeviceStore, DEV_STORE_VAR, SIZEOF( DRM_CONST_STRING ) );                                               \
        {                                                                                                                               \
            DRM_WCHAR __macro_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};                                                      \
            DRM_WCHAR *__macro_pwszDeviceStore = NULL;                                                                                  \
                                                                                                                                        \
            dr = DRM_SUCCESS;                                                                                                           \
                                                                                                                                        \
            FREEDRMSTRINGPOINTER( pDeviceStore );                                                                                       \
            ChkDR( tGetDeviceStorePathname( __macro_wszPathName, pDeviceStore ) );                                                      \
            ChkOverflow( pDeviceStore->cchString + 1, pDeviceStore->cchString );                                                        \
            ChkOverflow( SIZEOF( DRM_WCHAR ) * ( pDeviceStore->cchString + 1 ), pDeviceStore->cchString + 1 );                          \
            ChkMem( __macro_pwszDeviceStore = ( DRM_WCHAR * ) Oem_MemAlloc( SIZEOF( DRM_WCHAR ) * ( pDeviceStore->cchString + 1 ) ) );  \
            MEMCPY( __macro_pwszDeviceStore, pDeviceStore->pwszString, SIZEOF( DRM_WCHAR ) * pDeviceStore->cchString );                 \
            __macro_pwszDeviceStore[ pDeviceStore->cchString ] = g_wchNull;                                                             \
            pDeviceStore->pwszString = __macro_pwszDeviceStore;                                                                         \
                                                                                                                                        \
        }                                                                                                                               \
    }                                                                                                                                   \
    while ( FALSE )

#define INIT_SHARED_APP_CONTEXT_NO_DRMINIT( pApp, pbOpaqueBuffer, pDeviceStore ) do {                                                   \
        INIT_VARIABLE_BY_NAME( pApp, APP_CONTEXT_VAR, SIZEOF( DRM_APP_CONTEXT ) );                                                      \
        INIT_VARIABLE_BY_NAME( pbOpaqueBuffer, DRMBUFFER_VAR, TEST_DRM_BUFFER_SIZE );                                                   \
        INIT_SHARED_STORE( pDeviceStore );                                                                                              \
    }                                                                                                                                   \
    while ( FALSE )

#define INIT_SHARED_APP_CONTEXT( pApp, pDeviceStore ) do {                                                                              \
        DRM_BYTE *__macro_pbOpaqueBuffer = NULL;                                                                                        \
        INIT_SHARED_APP_CONTEXT_NO_DRMINIT( pApp, __macro_pbOpaqueBuffer, pDeviceStore );                                               \
        ChkDR( TestDrmInitializeWithActivation( pApp, NULL, __macro_pbOpaqueBuffer, TEST_DRM_BUFFER_SIZE, pDeviceStore ) );             \
    }                                                                                                                                   \
    while ( FALSE )

#define FREE_SHARED_APP_CONTEXT( pApp, pDeviceStore  ) do {                                                                             \
        DRM_BYTE *__macro_pbOpaqueBuffer = NULL;                                                                                        \
        if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( DRMBUFFER_VAR, ( DRM_VOID ** ) &__macro_pbOpaqueBuffer, NULL ) ) )                      \
        {                                                                                                                               \
            DRM_TST_FreeTestVar( DRMBUFFER_VAR );                                                                                       \
        }                                                                                                                               \
        if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( APP_CONTEXT_VAR, ( DRM_VOID ** ) &pApp, NULL ) ) )                                      \
        {                                                                                                                               \
            ChkDR( TestScanDevicePrivateKey( pApp ) );                                                                                  \
            Drm_Uninitialize( pApp );                                                                                                   \
            DRM_TST_FreeTestVar( APP_CONTEXT_VAR );                                                                                     \
        }                                                                                                                               \
        if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( DEV_STORE_VAR, ( DRM_VOID ** ) &pDeviceStore, NULL ) ) )                                \
        {                                                                                                                               \
            FREEDRMSTRINGPOINTER( pDeviceStore );                                                                                       \
            DRM_TST_FreeTestVar( DEV_STORE_VAR );                                                                                       \
        }                                                                                                                               \
    }                                                                                                                                   \
    while ( FALSE )


EXIT_PKTEST_NAMESPACE

#endif /* __DRM_TEST_STDAFX_H */
