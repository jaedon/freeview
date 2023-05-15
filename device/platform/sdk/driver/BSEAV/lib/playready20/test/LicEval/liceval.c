/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tclib.h"  /* this needs to be here because it #includes <windows.h> */

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmliceval.h>
#include <drmxmr.h>
#include <drmheaderparser.h>

#include <drmmanager.h>
#include <oempkcrypto.h>
#include <drmlicacq.h>
#include <oemcipher.h>
#include <drmblackbox.h>
#include <drmrevocation.h>
#include <drmviewrights.h>
#include <drmcrt.h>

#include "tOEMImp.h"
#include "tOEMImp_PD.h"
#include "licenses.h"
#include "tstutils.h"
#include "tstHDSutils.h"
#include "tstXMLutils.h"
#include "teststubcerts.h"

#include <stdafx.h>
#include <LicEvalTestAPI.h>
#include <DrmManagerTestAPI.h>
#include <drmapprevocation.h>
#include <drmwmdrm.h>

ENTER_PKTEST_NAMESPACE_CODE

/*
** Global context data
*/
DRM_DST             *g_pDatastore       = NULL;
#pragma prefast(suppress:22112)     /* ignore the warning about predictable address for function pointer */
DRM_LICEVAL_CONTEXT  g_LicEvalContext   = { 0 };
DRM_BB_CONTEXT       g_BBContextLicEval = { 0 };
DRM_LICSTORE_CONTEXT g_LicStoreContext  = { 0 };

/*
** Version string data
*/
#define VER_STRING_MAX_LEN 20
const DRM_WCHAR g_wcszINVALID_BB_VERSION_STRING[]  =  { ONE_WCHAR('1', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('\0', '\0') }; /* "1.0.0.0" */
const DRM_WCHAR g_wcszINVALID_DRM_VERSION_STRING[] =  { ONE_WCHAR('1', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('\0', '\0') }; /* "1.0.0.1" */
DRM_WCHAR_NATIVE g_wszBBVerString[VER_STRING_MAX_LEN]     = { 0 };

/*
** LID and KID data
*/
#define KID_MAX_LEN 20
const DRM_WCHAR g_wcszINVALID_LID[] = { ONE_WCHAR('{', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('-', '\0'), ONE_WCHAR('3', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('-', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('-', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('3', '\0'), ONE_WCHAR('-', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('5', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('5', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('9', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('}', '\0'), ONE_WCHAR('\0', '\0') }; /* "{11117A28-3AAA-7777-AF13-F77A5258A9F1}" */
const DRM_WCHAR g_wcszINVALID_KID[] = { ONE_WCHAR('1', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('z', '\0'), ONE_WCHAR('U', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('Z', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('4', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('Z', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('=', '\0'), ONE_WCHAR('=', '\0'), ONE_WCHAR('\0', '\0') }; /* "1111Sn1czUaZpE4OPSbZbg==" */

/*
** XMR License Data
*/
#define g_cbMaxStack 4096
DRM_STACK_ALLOCATOR_CONTEXT g_Stack            = { 0 };
DRM_BYTE g_rgbStack[__CB_DECL( g_cbMaxStack )] = { 0 };


/*
** Test action qualifiers GUIDs
*/

DRM_DEFINE_LOCAL_GUID( TEST_ACTION_QUALIFIER_0,
                 0x43b4f9b6, 0xb8d8, 0x45b7, 0xa5, 0xa, 0xe1, 0xed, 0xa7, 0xff, 0x57, 0x70 );

DRM_DEFINE_LOCAL_GUID( TEST_ACTION_QUALIFIER_1,
                 0xbe971824, 0x44ae, 0x4c93, 0xb0, 0xac, 0x5, 0xf4, 0x1f, 0x21, 0xdf, 0x97 );

DRM_DEFINE_LOCAL_GUID( TEST_ACTION_QUALIFIER_2,
                 0xacfadc7a, 0x9d4, 0x4069, 0xbb, 0xb0, 0xc3, 0xc, 0x9c, 0x86, 0xb2, 0x63 );

DRM_DEFINE_LOCAL_GUID( TEST_ACTION_QUALIFIER_3,
                 0x22053805, 0xba2a, 0x46fa, 0xac, 0x41, 0xcf, 0x20, 0x97, 0x5, 0x40, 0xac );

DRM_DEFINE_LOCAL_GUID( TEST_ACTION_QUALIFIER_4,
                 0xc45e1ae0, 0x4b71, 0x4412, 0xa9, 0x33, 0x97, 0x18, 0x7, 0x27, 0x4b, 0x19 );

DRM_DEFINE_LOCAL_GUID( TEST_ACTION_QUALIFIER_5,
                 0xf00c930b, 0x1783, 0x4348, 0xad, 0x5a, 0xed, 0x5d, 0xc5, 0xe6, 0xeb, 0xa6 );

DRM_DEFINE_LOCAL_GUID( TEST_ACTION_QUALIFIER_6,
                 0x27b15d2e, 0x909d, 0x4bba, 0xa0, 0x80, 0xf1, 0xad, 0x64, 0xde, 0x79, 0x61 );

DRM_DEFINE_LOCAL_GUID( TEST_ACTION_QUALIFIER_7,
                 0xe8458ee1, 0x7048, 0x498d, 0x89, 0xb3, 0x9b, 0xe, 0x26, 0xf3, 0xfc, 0xe6 );

DRM_DEFINE_LOCAL_GUID( TEST_ACTION_QUALIFIER_NONEXIST,
                 0x97de2a42, 0x3c34, 0x4281, 0x8f, 0xaa, 0xfd, 0x88, 0xfb, 0x8e, 0x76, 0x2b );


/*
** _TestAddActionQualifiersToTheLicenseXMR
**
** Synopsis: Adds Action Qualifiers objects to the existing license XMR
**
** f_pStack     : Specifies existing allocator context
** f_pXmrLicense: Specifies existing license XMR to add Qualifiers objects to
*/
static DRM_RESULT DRM_CALL _TestAddActionQualifiersToTheLicenseXMR(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN          DRM_XMR_LICENSE             *f_pXmrLicense )
{
    DRM_RESULT                  dr                          = DRM_SUCCESS;    
    DRM_DWORD                   dwAdditionalStackNeeded     = 0;
    DRM_XMR_UNKNOWN_CONTAINER  *pEnablerContainers[ 6 ];
    DRM_XMR_UNKNOWN_OBJECT     *pEnablerObjects[ 8 ];
    DRM_DWORD                   dwObjectCount = 0;

    /*
    ** Here is what we add to the license XMR:
    **
    ** <PlayContainer>
    **      <EnablerContainer_0>
    **          <TEST_ACTION_QUALIFIER_0>
    **          <TEST_ACTION_QUALIFIER_1>
    **      <EnablerContainer_2>
    **          <TEST_ACTION_QUALIFIER_2>
    **          <EnablerContainer_3>
    **              <TEST_ACTION_QUALIFIER_3>
    ** <CopyContainer>
    **      <EnablerContainer_1>
    **          <TEST_ACTION_QUALIFIER_4>
    **          <TEST_ACTION_QUALIFIER_5>
    **          <EnablerContainer_4>
    **              <TEST_ACTION_QUALIFIER_6>
    **      <EnablerContainer_5>
    **          <TEST_ACTION_QUALIFIER_7>
    **
    ** All EnablerContainer are DRM_XMR_UNKNOWN_CONTAINER objects;
    ** All TEST_ACTION_QUALIFIER_# are in DRM_XMR_UNKNOWN_OBJECT objects
    */

    /* 
    ** Check if we have enough space left on the stack
    ** We are adding: 8 DRM_XMR_UNKNOWN_OBJECT objects and 4 DRM_XMR_UNKNOWN_CONTAINER objects 
    ** (CopyContainer and PlayContainer each have 1 DRM_XMR_UNKNOWN_CONTAINER already)
    */
    dwAdditionalStackNeeded = SIZEOF(DRM_XMR_UNKNOWN_CONTAINER) * (NO_OF(pEnablerContainers) - 2) +
                              SIZEOF(DRM_XMR_UNKNOWN_OBJECT) * NO_OF(pEnablerObjects);
    ChkBOOL( dwAdditionalStackNeeded <= f_pStack->cbStack - f_pStack->nStackTop, DRM_E_OUTOFMEMORY );

    /* 
    ** Add Copy Policies Container if it's not there
    */
    if ( !XMR_IS_COPY_VALID( f_pXmrLicense ) )
    {
        ZEROMEM( &f_pXmrLicense->containerOuter.containerCopyPolicies, SIZEOF(DRM_XMR_COPY_RIGHTS) );
        f_pXmrLicense->containerOuter.containerCopyPolicies.fValid = TRUE;
    }

    /* 
    ** First 2 EnablerContainers already exist in PlayContainer and CopyContainer
    */
    pEnablerContainers[ 0 ] = &f_pXmrLicense->containerOuter.containerPlaybackPolicies.UnknownContainer;
    ZEROMEM( pEnablerContainers[ 0 ], SIZEOF(DRM_XMR_UNKNOWN_CONTAINER) );
    pEnablerContainers[ 1 ] = &f_pXmrLicense->containerOuter.containerCopyPolicies.UnknownContainer;
    ZEROMEM( pEnablerContainers[ 1 ], SIZEOF(DRM_XMR_UNKNOWN_CONTAINER) );

    /* 
    ** Allocate the space for the rest of the objets from the stack
    */
    for ( dwObjectCount = 2 ; dwObjectCount < NO_OF(pEnablerContainers) ; ++dwObjectCount )
    {
        pEnablerContainers[ dwObjectCount ] = (DRM_XMR_UNKNOWN_CONTAINER*)(f_pStack->pbStack + f_pStack->nStackTop);
        f_pStack->nStackTop = f_pStack->nStackTop + SIZEOF(DRM_XMR_UNKNOWN_CONTAINER);

        ZEROMEM( pEnablerContainers[ dwObjectCount ], SIZEOF(DRM_XMR_UNKNOWN_CONTAINER) );
    }
    for ( dwObjectCount = 0 ; dwObjectCount < NO_OF(pEnablerObjects) ; ++dwObjectCount )
    {
        pEnablerObjects[ dwObjectCount ] = (DRM_XMR_UNKNOWN_OBJECT*)(f_pStack->pbStack + f_pStack->nStackTop);
        f_pStack->nStackTop = f_pStack->nStackTop + SIZEOF(DRM_XMR_UNKNOWN_OBJECT);

        ZEROMEM( pEnablerObjects[ dwObjectCount ], SIZEOF(DRM_XMR_UNKNOWN_OBJECT) );
    }

    /* 
    ** Initialize objects
    */
    for ( dwObjectCount = 0 ; dwObjectCount < NO_OF(pEnablerContainers) ; ++dwObjectCount )
    {
        pEnablerContainers[ dwObjectCount ]->fValid = TRUE;
    }
    pEnablerContainers[ 0 ]->wType = XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER;
    pEnablerContainers[ 2 ]->wType = XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER;
    pEnablerContainers[ 3 ]->wType = XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER;
    pEnablerContainers[ 1 ]->wType = XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER;
    pEnablerContainers[ 4 ]->wType = XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER;
    pEnablerContainers[ 5 ]->wType = XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER;
    for ( dwObjectCount = 0 ; dwObjectCount < 4 ; ++dwObjectCount )
    {
        pEnablerObjects[ dwObjectCount ]->fValid = TRUE;
        pEnablerObjects[ dwObjectCount ]->wType = XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT;
        pEnablerObjects[ dwObjectCount ]->cbData = SIZEOF(DRM_GUID);
    }
    for ( dwObjectCount = 4 ; dwObjectCount < NO_OF(pEnablerObjects) ; ++dwObjectCount )
    {
        pEnablerObjects[ dwObjectCount ]->fValid = TRUE;
        pEnablerObjects[ dwObjectCount ]->wType = XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT;
        pEnablerObjects[ dwObjectCount ]->cbData = SIZEOF(DRM_GUID);
    }

    /* 
    ** Put the Enabler Containers into the proper places
    */
    pEnablerContainers[ 0 ]->pNext = pEnablerContainers[ 2 ];
    pEnablerContainers[ 1 ]->pNext = pEnablerContainers[ 5 ];
    pEnablerContainers[ 1 ]->pUnkChildcontainer = pEnablerContainers[ 4 ];
    pEnablerContainers[ 2 ]->pUnkChildcontainer = pEnablerContainers[ 3 ];

    /* 
    ** Put Enabler Objects and GUIDs in place
    */
    pEnablerContainers[ 0 ]->pObject = pEnablerObjects[ 0 ];
    pEnablerObjects[ 0 ]->pbBuffer = (DRM_BYTE*) &TEST_ACTION_QUALIFIER_0;
    pEnablerObjects[ 0 ]->pNext = pEnablerObjects[ 1 ];
    pEnablerObjects[ 1 ]->pbBuffer = (DRM_BYTE*) &TEST_ACTION_QUALIFIER_1;
    pEnablerContainers[ 2 ]->pObject = pEnablerObjects[ 2 ];
    pEnablerObjects[ 2 ]->pbBuffer = (DRM_BYTE*) &TEST_ACTION_QUALIFIER_2;
    pEnablerContainers[ 3 ]->pObject = pEnablerObjects[ 3 ];
    pEnablerObjects[ 3 ]->pbBuffer = (DRM_BYTE*) &TEST_ACTION_QUALIFIER_3;
    pEnablerContainers[ 1 ]->pObject = pEnablerObjects[ 4 ];
    pEnablerObjects[ 4 ]->pbBuffer = (DRM_BYTE*) &TEST_ACTION_QUALIFIER_4;
    pEnablerObjects[ 4 ]->pNext = pEnablerObjects[ 5 ];
    pEnablerObjects[ 5 ]->pbBuffer = (DRM_BYTE*) &TEST_ACTION_QUALIFIER_5;
    pEnablerContainers[ 4 ]->pObject = pEnablerObjects[ 6 ];
    pEnablerObjects[ 6 ]->pbBuffer = (DRM_BYTE*) &TEST_ACTION_QUALIFIER_6;
    pEnablerContainers[ 5 ]->pObject = pEnablerObjects[ 7 ];
    pEnablerObjects[ 7 ]->pbBuffer = (DRM_BYTE*) &TEST_ACTION_QUALIFIER_7;

ErrorExit:
    return dr;    
}

/*
** TestLicEvalContextSetting
** argv[0]: License Type: XML or XMR
** argv[1]: iOptions: the field of the LicEvalContext to be overwritten
** argv[2]: instruction for this field: valid, wrong, invalid, NULL, 
**          NOStoreRight, NOSelectRight, NODeleteRight, ActionQualifiers.
** argv[3]: (optional) Shared stored license variable
** 
** iOptions:
**     0.  all valid fields
**     1.  pcontextBBX              - Default to g_BBContextLicEval
**     2.  dstrContentHeader        - Default to g_wszHEADER1
**     3.  dstrContentLicense       - Default to g_wszDEFAULT_LICENSE ( or XMR )
**     4.  dstrBBVer;
**     5.  dstrDRMVer               - Default to g_dstrReqTagPlayReadyClientVersionData
**     6.  pcontextSSTLicense;
**     7.  pcontextSSTRevocation 
**     8.  rgbLID                   - Default to zeroes
**     9.  wszKID
**     10. dwFlags                   - Default to LICEVAL_OPEN_CHECK_SETTINGS
**     11. SecureStoreGlobalPassword - Default gets the global secure store password from blackbox
*/
DRM_RESULT DRM_CALL TestLicEvalContextSetting( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr              = DRM_SUCCESS;    
    DRM_DWORD        dwOptions       = 0;
    DRM_DWORD        cbKID           = KID_MAX_LEN;
    DRM_LONG         idx             = 0;   
    DRM_STRING       dstrDevCert     = EMPTY_DRM_STRING;    
    DRM_CONST_STRING dstrTemp        = EMPTY_DRM_STRING;    
    DRM_BYTE         szOEMGlobalPassword[__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };
    DRM_BYTE         szSecStorePassword[__CB_DECL( DRM_SHA1_DIGEST_LEN )]  = { 0 };
    DRM_BYTE         rgbBBVer[__CB_DECL( VERSION_LEN )]               = { 0 };  
    DRM_BYTE         rgbKID[__CB_DECL( KID_MAX_LEN )]                 = { 0 };
    DRM_BYTE        *pbLicense  = NULL;
    DRM_DWORD       *pcbLicense = NULL;

    Log( "Trace", "From TestLicEvalContextSetting, argc = %d", argc );

    for( idx = 0; idx < argc; idx++ )
    {
        Log( "Trace", "From TestLicEvalContextSetting: argv[%d] = %s", idx, argv[idx] );
    }
    ChkArg( argc    >  2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    dwOptions = atoi( argv[1] );
    
    ChkArg( dwOptions <= 11 );

    if ( strncmp( argv[0], "XML", 3 ) != 0 )
    {
        ChkArg( strncmp( argv[0], "XMR", 3 ) == 0 );
        g_LicEvalContext.fLicenseIsXMR = TRUE;
    }
    
    /*
    ** 10. dwFlags
    */
    if ( dwOptions != 10 )
    {
        g_LicEvalContext.dwFlags = LICEVAL_OPEN_CHECK_SETTINGS;
    }
    else if ( argv[2] != NULL )
    {    
        if ( strncmp( argv[2], "valid", 5 ) == 0 )
        {
            g_LicEvalContext.dwFlags = LICEVAL_STORE_CHECK_SETTINGS;
        }
        else             
        {
            ChkArg( strncmp( argv[2], "invalid", 7 ) == 0 );
            g_LicEvalContext.dwFlags = LICEVAL_OPEN_CHECK_SETTINGS | 8;
        }
    }

    /*
    ** 1. pcontextBBX 
    */
    if ( dwOptions != 1 )
    {
        g_LicEvalContext.pcontextBBX = &g_BBContextLicEval; 
    }
    else
    {
        g_LicEvalContext.pcontextBBX = NULL;
    }

    /*
    ** Initialize Blackbox
    */

    if( g_BBContextLicEval.pKeyFileContext == NULL )
    {
        ChkMem( g_BBContextLicEval.pKeyFileContext = ( DRM_KEYFILE_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_KEYFILE_CONTEXT ) ) );
        ChkDR( DRM_KF_Initialize( NULL, g_BBContextLicEval.pKeyFileContext ) );
        ChkDR( DRM_KF_Open( g_BBContextLicEval.pKeyFileContext, TRUE, OEM_GENERIC_READ ) );
        ChkDR( DRM_KF_LoadFromFile( g_BBContextLicEval.pKeyFileContext, TRUE ) );
        ChkDR( DRM_BBX_SetupSecureStoreKey( g_BBContextLicEval.pKeyFileContext, NULL ) );
        ChkDR( DRM_KF_Close( g_BBContextLicEval.pKeyFileContext ) );
    }
    
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_SetWMDRMCertCachedValues( &g_BBContextLicEval ) );
    }

    ChkDR( DRM_BBX_Initialize( NULL, &g_BBContextLicEval ) );

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_GetWMDRMCert( &dstrDevCert, &g_BBContextLicEval.CryptoContext ) );

        ChkDR( DRM_DCP_GetAttribute(  NULL,
                                      (DRM_CONST_STRING*)&dstrDevCert, 
                                      DRM_DEVCERT_GROUPSECURITYLEVEL, 
                                      NULL, 
                                     &dstrTemp ) );
        DRMCRT_wcsntol( dstrTemp.pwszString, 
                        dstrTemp.cchString, 
                       (DRM_LONG *)&( g_LicEvalContext.certinfoSDK.appSec ) );
    
        ChkDR( DRM_DCP_GetAttribute( NULL, (DRM_CONST_STRING*)&dstrDevCert, DRM_DEVCERT_SUBJECTID, NULL, &dstrTemp ) );
        DRMCRT_wcsntol( dstrTemp.pwszString, 
                        dstrTemp.cchString, 
                       (DRM_LONG *)&( g_LicEvalContext.certinfoSDK.appcd_subject ) );
    }

    ChkDR( DRM_BBX_Initialize( NULL, &g_BBContextLicEval ) );

    /*
    ** 2. dstrContentHeader; 
    */
    if ( dwOptions != 2 ) 
    {
        if ( !g_LicEvalContext.fLicenseIsXMR )        
        { 
            ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrContentHeader, g_wszHEADER1 );
        }            
    }     
    else if ( argv[2] != NULL && strncmp( argv[2], "default", 7 ) == 0 )    
    { 
        ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrContentHeader, g_wszHEADER2 );
    }    
       
    /*
    ** 3.  dstrContentLicense; 
    */
    if ( dwOptions == 3 && argv[2] == NULL ) /* NULL dstrContentLicense */ 
    {
        g_LicEvalContext.dstrContentLicense.pwszString = NULL; 
        g_LicEvalContext.plicenseXMR = NULL;
    }
    else if ( g_LicEvalContext.fLicenseIsXMR )
    {
        ChkArg( argc    >  3 );
        ChkArg( argv[3] != NULL);

        MAP_TO_GLOBAL_TEST_VAR_EX( pbLicense, pcbLicense, argv[3] );

        ChkMem( g_LicEvalContext.plicenseXMR = (DRM_XMR_LICENSE *)Oem_MemAlloc( SIZEOF( DRM_XMR_LICENSE ) ) );                 

        ChkDR( DRM_XMR_UnpackLicense( pbLicense, 
                                     *pcbLicense,
                                     &g_Stack, 
                                      g_LicEvalContext.plicenseXMR ) );

        if ( argv[2] != NULL && (strncmp( argv[2], "ActionQualifiers", strlen("ActionQualifiers") ) == 0) )
        {
            /* Add Action Qualifiers to the license XMR
            */
            ChkDR( _TestAddActionQualifiersToTheLicenseXMR( &g_Stack, g_LicEvalContext.plicenseXMR ) );
        }

        DSTR_FROM_PB( &g_LicEvalContext.dstrContentLicense,
                       pbLicense,
                      *pcbLicense );    

    }
    else if ( dwOptions != 3 ) 
    {
        ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrContentLicense, g_wszDEFAULT_LICENSE );
    }                             
    else if ( strncmp( argv[2], "old", 3 ) == 0 ) 
    {
        ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrContentLicense, g_wszOLD_LICENSE );
    } 
    else if ( strncmp( argv[2], "invalid", 7 ) == 0 ) 
    {
        ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrContentLicense, g_wszINVALID_LICENSE );
    } 
    else if ( strncmp( argv[2], "NOStoreRight", 12 ) == 0 ) 
    {
        ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrContentLicense, g_wszNO_STORE_RIGHT_LICENSE );
    } 
    else if ( strncmp( argv[2], "NOSelectRight", 13 ) == 0 )    
    {
        ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrContentLicense, g_wszNO_SELECT_RIGHT_LICENSE );
    } 
    else if ( strncmp( argv[2], "NODeleteRight", 13 ) == 0 ) 
    {
        ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrContentLicense, g_wszNO_DELETE_RIGHT_LICENSE );
    }
    else
    {
        ChkArg( FALSE );
    }     
            
    ChkDR( DRM_LST_Open( &g_LicStoreContext, 
                          g_pDatastore, 
                          g_LicEvalContext.fLicenseIsXMR ? 
                          eDRM_LICENSE_STORE_XMR : eDRM_LICENSE_STORE_XML ) );
   
    /*
    ** 11. SecureStoreGlobalPassword 
    */
    if ( dwOptions != 11 ) 
    {
        /*
        ** Talk to blackbox to get the global secure store password 
        */
        ChkDR( Oem_Device_GetSecureStoreGlobalPasswordSeed( szOEMGlobalPassword ) );
        ChkDR( DRM_BBX_HashValue( szOEMGlobalPassword, 
                                  DRM_SHA1_DIGEST_LEN, 
                                  szSecStorePassword, 
                                 &g_BBContextLicEval ) );
    } 
    else if ( argv[2] == NULL ) /* NULL SecureStoreGlobalPassword*/
    {
        MEMSET( szSecStorePassword, 0, DRM_SHA1_DIGEST_LEN );
    }
    else if ( strncmp( argv[2], "invalid", 7 ) == 0 )
    {
        MEMSET( szSecStorePassword, 1, DRM_SHA1_DIGEST_LEN );
    }
    
    ChkMem( g_LicEvalContext.pcontextSSTGlobal = 
            ( DRM_SECSTORE_CONTEXT * )Oem_MemAlloc( SIZEOF( DRM_SECSTORE_CONTEXT ) ) );

    ChkDR( DRM_SST_OpenKeyTokens ( g_LicEvalContext.pcontextSSTGlobal, 
                                   &g_rgbSecStoreGlobalName, 
                                    NULL,
                                    szSecStorePassword, 
                                    DRM_SECURE_STORE_OPEN_ALWAYS,
                                    SECURE_STORE_GLOBAL_DATA, 
                                    g_pDatastore ) );
    
    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        /*
        ** add revocation:  revocation store not opened here 
        */
    
        /*
        ** 7. pcontextSSTRevocation 
        */
        if ( dwOptions != 7 ) 
        {
            SAFE_OEM_FREE( g_LicEvalContext.pbRevocationBuffer );
            g_LicEvalContext.cbRevocationBuffer = REVOCATION_BUFFER_SIZE;
        
            ChkMem( g_LicEvalContext.pbRevocationBuffer = 
                    ( DRM_BYTE * )Oem_MemAlloc( g_LicEvalContext.cbRevocationBuffer ) );
            MEMCPY( &g_LicEvalContext.appcert, g_teststubcert, SIZEOF( APPCERT ) );

            ChkMem( g_LicEvalContext.pcontextSSTRevocation = 
                ( DRM_SECSTORE_CONTEXT * )Oem_MemAlloc( SIZEOF( DRM_SECSTORE_CONTEXT ) ) );

            ChkMem( g_LicEvalContext.pContextRev = 
                ( DRM_REVOCATIONSTORE_CONTEXT * )Oem_MemAlloc( SIZEOF( DRM_REVOCATIONSTORE_CONTEXT ) ) );
            ChkDR( DRM_RVS_InitRevocationStore( g_pDatastore,
                                                g_LicEvalContext.pcontextSSTRevocation,
                                                g_LicEvalContext.pcontextBBX,
                                                g_LicEvalContext.pContextRev ) );
        } 
        else if ( argv[2] == NULL ) /* NULL pcontextSSTRevocation */
        {
            g_LicEvalContext.pcontextSSTRevocation = NULL;
        }
    }
    else
    {
        g_LicEvalContext.pcontextSSTRevocation = NULL;
    }
        
    /*
    ** 4. dstrBBVer
    */
    if ( dwOptions != 4 )
    {        
        /*
        ** The real current version of drm is 2.3.101.0 
        */
        MEMCPY( rgbBBVer, 
                g_BBContextLicEval.cachedCertValues.m_BBCompatibilityCert.pk.version, 
                VERSION_LEN );
        ChkDR( DRM_UTL_GetVersionAsString( rgbBBVer, (DRM_WCHAR *)g_wszBBVerString ) );

        g_LicEvalContext.dstrBBVer.pwszString = (const DRM_WCHAR *)g_wszBBVerString;
        g_LicEvalContext.dstrBBVer.cchString = 
            ( DRM_DWORD )DRMCRT_wcslen( g_LicEvalContext.dstrBBVer.pwszString );
    } 
    else if ( argv[2] == NULL )  
    {
        g_LicEvalContext.dstrBBVer.pwszString = NULL;
        g_LicEvalContext.dstrBBVer.cchString =  0;
    } 
    else if ( strncmp( argv[2], "invalid", 7 ) == 0 )
    {
        ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrBBVer, g_wcszINVALID_BB_VERSION_STRING );
    }

    /*
    ** 5.  dstrDRMVer; 
    */
    if ( dwOptions != 5 ) 
    {
        g_LicEvalContext.dstrDRMVer = g_dstrReqTagPlayReadyClientVersionData;
    } 
    else if ( argv[2] == NULL ) 
    {
        g_LicEvalContext.dstrDRMVer.pwszString = NULL;
        g_LicEvalContext.dstrDRMVer.cchString = 0;
    } 
    else if ( strncmp( argv[2], "invalid", 7 ) == 0 )
    {
        ASSIGN_WSTR_TO_DRMSTRING( g_LicEvalContext.dstrDRMVer, g_wcszINVALID_DRM_VERSION_STRING );
    }

    g_LicEvalContext.fGlobalSecStoreWritable = FALSE;

    /*
    ** We have a license.  Check the revocation list and app cert 
    */
       
    /*
    ** If XMR, get the LID and KID from the license
    ** Otherwise get the LID and KID as determined by dwOptions
    */     
    if ( !g_LicEvalContext.fLicenseIsXMR )
    {   
        /*
        ** 7. pcontextSSTRevocation = NULL would crash 
        */
        if ( DRM_APPREVOCATION_IsAppRevocationSupported() && dwOptions != 7 ) 
        {
            ChkDR( DRM_RVK_PerformAppRevocationCheck( &g_LicEvalContext.pcontextBBX->CryptoContext, 
                                                       g_LicEvalContext.pContextRev,
                                                      &g_LicEvalContext.dstrContentLicense,
                                                      &g_LicEvalContext.appcert,
                                                       g_LicEvalContext.pbRevocationBuffer,
                                                       g_LicEvalContext.cbRevocationBuffer,
                                                      &g_LicEvalContext.idCRLsCurrent ) );
        }
         
        /*
        ** Get the LID for the XML license 
        */
        ChkDR( DRM_WMDRM_LIC_GetAttribute( &( g_LicEvalContext.dstrContentLicense ), 
                                            NULL, 
                                            DRM_LICENSE_ATTRIB_LID, 
                                            NULL, 
                                           &dstrTemp, 
                                            0 ) );
    
        /*
        ** 8. rgbLID
        */
        if ( dwOptions == 8 ) 
        {
            if ( argv[2] == NULL ) 
            {
                dstrTemp.pwszString = NULL;
                dstrTemp.cchString = 0;
            } 
            else if ( strncmp( argv[2], "invalid", 7 ) == 0 ) 
            {
                ASSIGN_WSTR_TO_DRMSTRING( dstrTemp, g_wcszINVALID_LID );
            }
        }    
    
        ChkDR( DRM_UTL_StringToGuid( &dstrTemp, ( DRM_GUID * )g_LicEvalContext.LID.rgb ) );  
    
        /*
        ** 9. wszKID ( for XML license )
        */
        if ( dwOptions != 9 ) 
        {
            ChkDR( DRM_HDR_GetAttribute( &( g_LicEvalContext.dstrContentHeader ), 
                                          NULL, 
                                          DRM_HEADER_ATTRIB_KID, 
                                         &dstrTemp, 
                                          0 ) );
        } 
        else if ( argv[2] == NULL )  /* rgbKID = NULL */
        {
            dstrTemp.pwszString = NULL;
        }
        else if ( strncmp( argv[2], "invalid", 7 ) == 0 ) 
        {
            ASSIGN_WSTR_TO_DRMSTRING( dstrTemp, g_wcszINVALID_KID );
        }

        /*
        ** If the following base64 decode fails because the buffer isn't big enough,
        ** then it isn't a valid KID anyway.
        */
        MEMSET( rgbKID, 0, KID_MAX_LEN );
        ChkDR( DRM_B64_DecodeW( &dstrTemp, &cbKID, rgbKID, 0 ) );
                                      
        g_LicEvalContext.fLicenseIsXMR = FALSE;
        g_LicEvalContext.plicenseXMR = NULL;               
    }
    else if ( g_LicEvalContext.plicenseXMR != NULL )
    {   
        /*
        ** 8. rgbLID ( for XMR license )
        */
        if ( dwOptions == 8 ) 
        {
            if ( argv[2] == NULL ) 
            {
                dstrTemp.pwszString = NULL;
                dstrTemp.cchString = 0;
            } 
            else if ( strncmp( argv[2], "invalid", 7 ) == 0 ) 
            {
                ASSIGN_WSTR_TO_DRMSTRING( dstrTemp, g_wcszINVALID_LID );
            }
            
            ChkDR( DRM_UTL_StringToGuid( &dstrTemp, ( DRM_GUID * )g_LicEvalContext.LID.rgb ) );  
        }    
        else
        {
            DRM_BYT_CopyBytes( g_LicEvalContext.LID.rgb,
                               0,
                               g_LicEvalContext.plicenseXMR->pbRightsIdBuffer,
                               g_LicEvalContext.plicenseXMR->iRightsId,
                               SIZEOF( g_LicEvalContext.LID.rgb ) );
        }        
    
        /*
        ** 9. wszKID ( for XMR license )
        */
        if ( dwOptions != 9 ) 
        {
            DRM_BYT_CopyBytes( g_LicEvalContext.KID.rgb,
                               0,
                               g_LicEvalContext.plicenseXMR->containerOuter.containerKeys.ContentKey.pbguidKeyID,
                               g_LicEvalContext.plicenseXMR->containerOuter.containerKeys.ContentKey.iguidKeyID,
                               SIZEOF( g_LicEvalContext.KID.rgb ) );
        } 
        else if ( argv[2] == NULL )  /* rgbKID = NULL */
        {
            dstrTemp.pwszString = NULL;
        }
        else if ( strncmp( argv[2], "invalid", 7 ) == 0 ) 
        {
            ASSIGN_WSTR_TO_DRMSTRING( dstrTemp, g_wcszINVALID_KID );
        }                                                                        
    }                                         

    /*
    ** 6. pcontextSSTLicense 
    */
    if ( dwOptions != 6 )
    {                                    
        ChkMem( g_LicEvalContext.pcontextSSTLicense = ( DRM_SECSTORE_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_SECSTORE_CONTEXT ) ) );
        ChkDR( DRM_SST_OpenKeyTokens ( g_LicEvalContext.pcontextSSTLicense, 
                                      &( g_LicEvalContext.LID ),
                                       NULL,
                                       szSecStorePassword, 
                                       DRM_SECURE_STORE_OPEN_ALWAYS,
                                       SECURE_STORE_LICENSE_DATA, 
                                       g_pDatastore ) );                                      
    }
    g_LicEvalContext.fLegacyAppInfoValid = FALSE; 
    g_LicEvalContext.fAppInfoValid = FALSE;        

ErrorExit:
    FREEDRMSTRING( dstrDevCert );
    
    return dr;    
}

/*
** Arguments:
**              argv[0] -- LICEVAL Operation
**              argv[1] -- Operation State
**              argv[2] -- Action String
**              argv[3] -- If NULL pass NULL LicEval Context
**              argv[4] -- If NULL pass NULL to f_pfPerform
**              
*/
DRM_RESULT DRM_CALL TestLicenseEvalPerformOperations( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_LONG         idx          = 0;
    DRM_CONST_STRING dstrAction   = EMPTY_DRM_STRING;
    DRM_BOOL         fPerform     = FALSE;
    DRM_BOOL         fActionExist = FALSE;
    eDRM_LICEVAL_OPERATION_STATE eOperationState = (eDRM_LICEVAL_OPERATION_STATE)0;
    eDRM_LICEVAL_OPERATIONS      eOperation      = (eDRM_LICEVAL_OPERATIONS)0;

    Log( "Trace", "From TestLicenseEvalPerformOperations, argc = %d", argc );

    for( idx = 0; idx < argc; idx++ )
    {
        Log( "Trace", "From TestLicenseEvalPerformOperations: argv[%d] = %s", idx, argv[idx] );
    }

    ChkArg( argc == 5 );

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "DRM_LICENSE_EVAL_SELECT", 23 ) == 0 )
        {
            eOperation = DRM_LICENSE_EVAL_SELECT;
        }
        else if ( strncmp( argv[0], "DRM_LICENSE_EVAL_STORE", 22 ) == 0 )
        {
            eOperation = DRM_LICENSE_EVAL_STORE;
        }
        else if ( strncmp( argv[0], "DRM_LICENSE_EVAL_DELETE", 23 ) == 0 )
        {
            eOperation = DRM_LICENSE_EVAL_DELETE;
        }
        else if ( strncmp( argv[0], "DRM_LICENSE_EVAL_REPORTCLOCK_ROLLBACK", 37 ) == 0 )
        {
            eOperation = DRM_LICENSE_EVAL_REPORTCLOCK_ROLLBACK;
        }
        else if ( strncmp( argv[0], "DRM_LICENSE_EVAL_ACTION", 23 ) == 0 )
        {
            eOperation=DRM_LICENSE_EVAL_ACTION;
        }
        else
        {
            eOperation = (eDRM_LICEVAL_OPERATIONS)atoi( argv[0] );
        }
    }

    if ( argv[1] != NULL )
    {
        if ( strncmp( argv[1], "DRM_LICENSE_EVAL_CAN_DO_OPERATION", 33 ) == 0 )
        {
            eOperationState = DRM_LICENSE_EVAL_CAN_DO_OPERATION;
        }
        else if ( strncmp( argv[1], "DRM_LICENSE_EVAL_DONE_WITH_OPERATION", 36 ) == 0 )
        {
            eOperationState=DRM_LICENSE_EVAL_DONE_WITH_OPERATION;
        }
        else
        {
            eOperationState = (eDRM_LICEVAL_OPERATION_STATE)atoi( argv[1] );
        }
    }

    ChkDR( MakeDRMString( &dstrAction, argv[2] ) );

    ChkDR( DRM_LEVL_PerformOperations( argv[3] != NULL? &g_LicEvalContext : NULL, 
                                       eOperation, 
                                       eOperationState, 
                                       argv[2] != NULL? &dstrAction : NULL, 
                                       argv[4] != NULL? &fPerform : NULL,
                                      &fActionExist,
                                       g_pDatastore,
                                       NULL,
                                       NULL ) );
ErrorExit:
    FREEDRMSTRING( dstrAction );
    return dr;
}

/*
** TestLicenseEvalPerformOperationsXMR
**
** Synopsis: Tests DRM_LEVL_PerformOperationsXMR() API
**
** argv[0]: Specifies the operation to be perfomed 
**          (DRM_LICENSE_EVAL_SELECT, DRM_LICENSE_EVAL_STORE, DRM_LICENSE_EVAL_DELETE, DRM_LICENSE_EVAL_REPORTCLOCK_ROLLBACK, DRM_LICENSE_EVAL_ACTION)
** argv[1]: Specifies the operation state
**          (DRM_LICENSE_EVAL_CAN_DO_OPERATION, DRM_LICENSE_EVAL_DONE_WITH_OPERATION)
** argv[2]: Specifies Action for DRM_LICENSE_EVAL_ACTION operation
**          (Play, Copy)
** argv[3]: Specifies Action Qualifier
**          (0, 1, 2, 3, 4, 5, 6, 7, NONEXIST)
** argv[4]: Specifies if LicEvalContext to be used
**          (NULL, Valid)
** argv[5]: Specifies f_pfPerform parameter of the DRM_LEVL_PerformOperationsXMR
**          (NULL, Valid)
** argv[6]: Specifies if operation is expected to PERFORM or NOPERFORM
**          (PERFORM, NOPERFORM)
*/
DRM_RESULT DRM_CALL TestLicenseEvalPerformOperationsXMR( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_LONG         idx          = 0;
    DRM_BOOL         fPerform     = FALSE;
    DRM_BOOL         fActionExist = FALSE;
    eDRM_LICEVAL_OPERATION_STATE eOperationState = (eDRM_LICEVAL_OPERATION_STATE)0;
    eDRM_LICEVAL_OPERATIONS      eOperation      = (eDRM_LICEVAL_OPERATIONS)0;
    DRM_GUID const  *pguidAction = NULL;
    DRM_GUID const  *pguidActionQualifier = NULL;
    DRM_BOOL         fExpectedPerform = TRUE;

    Log( "Trace", "From TestLicenseEvalPerformOperationsXMR, argc = %d", argc );

    for( idx = 0; idx < argc; idx++ )
    {
        Log( "Trace", "From TestLicenseEvalPerformOperationsXMR: argv[%d] = %s", idx, argv[idx] );
    }

    ChkArg( argc == 7 );

    if ( argv[0] != NULL )
    {
        if ( strncmp( argv[0], "DRM_LICENSE_EVAL_SELECT", 23 ) == 0 )
        {
            eOperation = DRM_LICENSE_EVAL_SELECT;
        }
        else if ( strncmp( argv[0], "DRM_LICENSE_EVAL_STORE", 22 ) == 0 )
        {
            eOperation = DRM_LICENSE_EVAL_STORE;
        }
        else if ( strncmp( argv[0], "DRM_LICENSE_EVAL_DELETE", 23 ) == 0 )
        {
            eOperation = DRM_LICENSE_EVAL_DELETE;
        }
        else if ( strncmp( argv[0], "DRM_LICENSE_EVAL_REPORTCLOCK_ROLLBACK", 37 ) == 0 )
        {
            eOperation = DRM_LICENSE_EVAL_REPORTCLOCK_ROLLBACK;
        }
        else if ( strncmp( argv[0], "DRM_LICENSE_EVAL_ACTION", 23 ) == 0 )
        {
            eOperation=DRM_LICENSE_EVAL_ACTION;
        }
        else
        {
            eOperation = (eDRM_LICEVAL_OPERATIONS)atoi( argv[0] );
        }
    }

    if ( argv[1] != NULL )
    {
        if ( strncmp( argv[1], "DRM_LICENSE_EVAL_CAN_DO_OPERATION", 33 ) == 0 )
        {
            eOperationState = DRM_LICENSE_EVAL_CAN_DO_OPERATION;
        }
        else if ( strncmp( argv[1], "DRM_LICENSE_EVAL_DONE_WITH_OPERATION", 36 ) == 0 )
        {
            eOperationState=DRM_LICENSE_EVAL_DONE_WITH_OPERATION;
        }
        else
        {
            eOperationState = (eDRM_LICEVAL_OPERATION_STATE)atoi( argv[1] );
        }
    }

    if ( argv[2] != NULL )
    {
        if ( strncmp( argv[2], "Play", strlen("Play") ) == 0 )
        {
            pguidAction = &DRM_ACTION_PLAY;
        }
        else if ( strncmp( argv[2], "Copy", strlen("Copy") ) == 0 )
        {
            pguidAction = &DRM_ACTION_COPY;
        }
    }

    if ( argv[3] != NULL )
    {
        if ( strncmp( argv[3], "NONEXIST", strlen("NONEXIST") ) == 0 )
        {
            pguidActionQualifier = &TEST_ACTION_QUALIFIER_NONEXIST;
        }
        else
        {
            DRM_DWORD       dwTestGUID_ID   = 0;

            dwTestGUID_ID = strtoul( argv[3], NULL, 0 );

            switch( dwTestGUID_ID )
            {
                case 0:
                    pguidActionQualifier = &TEST_ACTION_QUALIFIER_0;
                    break;
                    
                case 1:
                    pguidActionQualifier = &TEST_ACTION_QUALIFIER_1;
                    break;
                    
                case 2:
                    pguidActionQualifier = &TEST_ACTION_QUALIFIER_2;
                    break;
                    
                case 3:
                    pguidActionQualifier = &TEST_ACTION_QUALIFIER_3;
                    break;
                    
                case 4:
                    pguidActionQualifier = &TEST_ACTION_QUALIFIER_4;
                    break;
                    
                case 5:
                    pguidActionQualifier = &TEST_ACTION_QUALIFIER_5;
                    break;
                    
                case 6:
                    pguidActionQualifier = &TEST_ACTION_QUALIFIER_6;
                    break;
                    
                case 7:
                    pguidActionQualifier = &TEST_ACTION_QUALIFIER_7;
                    break;
                    
            }
        }
    }

    if ( argv[6] != NULL )
    {
        if ( strncmp( argv[6], "PERFORM", strlen("PERFORM") ) == 0 )
        {
            fExpectedPerform = TRUE;
        }
        if ( strncmp( argv[6], "NOPERFORM", strlen("NOPERFORM") ) == 0 )
        {
            fExpectedPerform = FALSE;
        }
    }
    
    ChkDR( DRM_LEVL_PerformOperationsXMR( argv[4] != NULL? &g_LicEvalContext : NULL, 
                                       eOperation, 
                                       eOperationState, 
                                       argv[2] != NULL? pguidAction : NULL, 
                                       argv[3] != NULL? pguidActionQualifier : NULL,
                                       0,
                                       argv[5] != NULL? &fPerform : NULL,
                                      &fActionExist,
                                       g_pDatastore,
                                       NULL,
                                       NULL ) );

    if ( argv[6] != NULL )
    {
        ChkBOOL( fPerform == fExpectedPerform, DRM_E_FAIL );
    }
    
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE( LicEval )( long lTCID, const char *strTCName )
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_CONST_STRING *pdstrHDSPath = NULL;
    DRM_APP_CONTEXT  *pAppContext  = NULL;
    
    g_Stack.nStackTop = 0;
    g_Stack.cbStack   = g_cbMaxStack;
    g_Stack.pbStack   = g_rgbStack;
    ZEROMEM( g_rgbStack, g_cbMaxStack );   
    ZEROMEM( &g_LicEvalContext, SIZEOF( DRM_LICEVAL_CONTEXT ) );
    ZEROMEM( &g_LicStoreContext, SIZEOF( DRM_LICSTORE_CONTEXT ) );

    FREE_SHARED_APP_CONTEXT( pAppContext, pdstrHDSPath  );

    RemoveDRMFile( RMFILE_ALL );

    ChkDR( SetDeviceEnv( DEVCERT_TEMPLATE_FILE_NAME, 
                         PRIVATE_KEY_FILE_NAME, 
                         GROUP_CERT_FILE_NAME,
                         GC_PRIVATE_KEY_FILE_NAME,
                         TRUE ) );

    RemoveDRMFile( RMFILE_STORE );
    INIT_SHARED_APP_CONTEXT( pAppContext, pdstrHDSPath );

    ChkDR( OpenHDS( &g_pDatastore, pdstrHDSPath->pwszString, FALSE ) );  
   
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE( LicEval )( long lTCID, const char *strTCName )
{                         
    DRM_LST_Close( &g_LicStoreContext );
    DRM_SST_CloseKey( g_LicEvalContext.pcontextSSTGlobal, g_pDatastore, NULL, NULL );
    DRM_SST_CloseKey( g_LicEvalContext.pcontextSSTLicense, g_pDatastore, NULL, NULL );       
        
    SAFE_OEM_FREE( g_LicEvalContext.pcontextSSTGlobal );
    SAFE_OEM_FREE( g_LicEvalContext.pcontextSSTLicense );
    SAFE_OEM_FREE( g_LicEvalContext.plicenseXMR );
    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        SAFE_OEM_FREE( g_LicEvalContext.pcontextSSTRevocation );
        SAFE_OEM_FREE( g_LicEvalContext.pbRevocationBuffer );
        SAFE_OEM_FREE( g_LicEvalContext.pContextRev );
    }

    ZEROMEM( &g_LicEvalContext, SIZEOF( DRM_LICEVAL_CONTEXT ) );

    if( g_BBContextLicEval.pKeyFileContext != NULL )
    {
        DRM_KF_Uninitialize(g_BBContextLicEval.pKeyFileContext);
        SAFE_OEM_FREE(g_BBContextLicEval.pKeyFileContext);
    }
    ZEROMEM( &g_BBContextLicEval, SIZEOF( DRM_BB_CONTEXT ) );

    if ( g_pDatastore != NULL )
    {
        CloseHDS( g_pDatastore );
        g_pDatastore = NULL;
    }      

    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST( LicEval ) 
    API_ENTRY( TestLicEvalContextSetting )
    API_ENTRY( TestLicenseEvalPerformOperations )
    API_ENTRY( TestLicenseEvalPerformOperationsXMR )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

