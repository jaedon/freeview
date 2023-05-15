/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <XmrTestAPI.h>
#include <drmhdrbuilder.h>
#include <drmxmr.h>
#include <drmxmrbuilder.h>
#include <drmxmrparser.h>
#include <drmkeygenerator.h>
#include <DrmManagerTestAPI.h>

ENTER_PKTEST_NAMESPACE_CODE

#define DRM_MAX_ENABLERS    2
#define DRM_STACK_SIZE      DRM_MAX_LICENSESIZE

typedef struct _tagTESTXMRTypes
{
    DRM_WORD        wObjectType;
    char*           szFriendlyName;
    DRM_DWORD       cchFriendlyName;
} _TEST_XMR_TYPES;

const _TEST_XMR_TYPES g_rgXMRObjectTypes[] =
{
    { XMR_OBJECT_TYPE_INVALID,                   "XMR_OBJECT_TYPE_INVALID",                   SIZEOF( "XMR_OBJECT_TYPE_INVALID"                  )  },
    { XMR_OBJECT_TYPE_OUTER_CONTAINER,           "XMR_OBJECT_TYPE_OUTER_CONTAINER",           SIZEOF( "XMR_OBJECT_TYPE_OUTER_CONTAINER"  )  },
    { XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER,  "XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER",  SIZEOF( "XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER" )  },
    { XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER,    "XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER",    SIZEOF( "XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER"   )  },
    { XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER,    "XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER",    SIZEOF( "XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER"   )  },
    { XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT,       "XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT",       SIZEOF( "XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT"      )  },
    { XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT,       "XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT",       SIZEOF( "XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT"      )  }
};

/******************************************************************************
**
** Function :   _SignXmrLicense
**
** Synopsis :   This is implementation of DRM_XMR_CB_SIGNATURE callback function
**
** Arguments :  f_pvSignatureContext  - Context needed by caller to retrieve
**                                      appropriate integrity key
**              f_pbLicense           - Pointer to license buffer
**              f_cbLicense           - Size of license buffer
**              f_pbSignatureBuffer   - Pointer to output signature buffer
**              f_cbSignatureBuffer   - Size of signature buffer
**              f_ibSignatureBuffer   - Offset (index) in signature buffer
**                                      where to start signature writing
**
** Returns :    DRM_SUCCESS if signing was successful
**
******************************************************************************/
static DRM_RESULT DRM_CALL _SignXmrLicense(
    __in                                const DRM_VOID  *f_pvSignatureContext,
    __in_bcount( f_cbLicense )          const DRM_BYTE  *f_pbLicense,
    __in                                const DRM_DWORD  f_cbLicense,
    __out_bcount( f_cbSignatureBuffer )       DRM_BYTE  *f_pbSignatureBuffer,
    __in                                const DRM_DWORD  f_cbSignatureBuffer,
    __in                                const DRM_DWORD  f_ibSignatureBuffer )
{
    DRM_RESULT dr                       = DRM_SUCCESS;
    DRM_AES_KEY oIntegrityKey;

    if( f_pvSignatureContext != NULL )
    {
        /*
        ** Sanity check the arguments
        */
        ChkArg( f_pbSignatureBuffer != NULL );
        ChkArg( f_cbSignatureBuffer - f_ibSignatureBuffer >= DRM_AES_BLOCKLEN );
        ChkArg( f_pbLicense         != NULL );
        ChkArg( f_cbLicense         >  0 );
        ChkArg( f_pbLicense         == f_pbSignatureBuffer );
        ChkArg( f_ibSignatureBuffer >  f_cbLicense );

        /*
        ** Sign the license with integrity key
        */
        ChkDR( Oem_Aes_SetKey(
               (DRM_BYTE*)f_pvSignatureContext,
               &oIntegrityKey ) );

        ChkDR( Oem_Omac1_Sign( &oIntegrityKey,
                               f_pbLicense,
                               0,
                               f_cbLicense,
                               f_pbSignatureBuffer + f_ibSignatureBuffer ) );
    }

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _GetXMRType(const char *f_szXMRType, __out_ecount(1) DRM_WORD* f_wObjectType)
{
    DRM_RESULT      dr          = DRM_E_TEST_UNEXPECTED_OUTPUT;
    DRM_DWORD       dwIndex     = 0;
    ChkArg( f_szXMRType != NULL);
    ChkArg( f_wObjectType != NULL );

    *f_wObjectType = XMR_OBJECT_TYPE_INVALID;
    for ( dwIndex = 0; dwIndex < NO_OF(g_rgXMRObjectTypes); dwIndex++)
    {
        if ( DRMCRT_strncmp( f_szXMRType, g_rgXMRObjectTypes[dwIndex].szFriendlyName, g_rgXMRObjectTypes[dwIndex].cchFriendlyName ) == 0 )
        {
            *f_wObjectType = g_rgXMRObjectTypes[dwIndex].wObjectType;
            ChkDR( DRM_SUCCESS );
        }
    }

ErrorExit:
    return dr;
}
/*****************************************************************************
** Function:     _TestVerifyEnablerGuid
**
** Synopsis:     Verify the guids in the enabler objects after Unpack license
**
** Arguments:    argv[0]: Object/container type
                 argv[1]: Value to compare
**
** Returns:      DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
static DRM_RESULT DRM_CALL  _TestVerifyEnablerGuid(
    long argc,
    __in_ecount( argc ) char **argv,
    DRM_XMR_UNKNOWN_CONTAINER       *pUnkContainer,
    DRM_DWORD iArgContainer )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_CONST_STRING dstrEnabler      = EMPTY_DRM_STRING;
    DRM_GUID         drmidEnabler     = {0};
    DRM_GUID         drmidLicEnabler  = {0};
    DRM_DWORD        iCurr            = 0;
    DRM_WORD         wObjectType      =  0;

    ChkArg( (DRM_DWORD)argc >= iArgContainer + 3 );

    ChkDR( _GetXMRType(argv[iArgContainer + 1], &wObjectType) );

    Log( "", "ObjectType From Argument: %X", wObjectType );


    ChkArg(
        pUnkContainer->pObject != NULL
     && pUnkContainer->pObject->wType == wObjectType );

    Log( "", "ObjectType From License : %X", pUnkContainer->pObject->wType );

    ChkDR( MakeDRMString( &dstrEnabler, argv[iArgContainer + 2] ) );
    ChkDR( DRM_UTL_StringToGuid(&dstrEnabler, &drmidEnabler) );


    Log( "", "Guid From Argument: {%8X-%4X-%4X-%2X%2X-%2X%2X%2X%2X%2X%2X}",
         drmidEnabler.Data1,
         drmidEnabler.Data2,
         drmidEnabler.Data3,
         GET_BYTE( drmidEnabler.Data4, 0 ),
         GET_BYTE( drmidEnabler.Data4, 1 ),
         GET_BYTE( drmidEnabler.Data4, 2 ),
         GET_BYTE( drmidEnabler.Data4, 3 ),
         GET_BYTE( drmidEnabler.Data4, 4 ),
         GET_BYTE( drmidEnabler.Data4, 5 ),
         GET_BYTE( drmidEnabler.Data4, 6 ),
         GET_BYTE( drmidEnabler.Data4, 7 ) );

    MEMCPY( &drmidLicEnabler.Data1, pUnkContainer->pObject->pbBuffer + pUnkContainer->pObject->ibData + iCurr, SIZEOF(drmidLicEnabler.Data1 ) );
    iCurr+=SIZEOF(drmidLicEnabler.Data1);
    MEMCPY( &drmidLicEnabler.Data2, pUnkContainer->pObject->pbBuffer + pUnkContainer->pObject->ibData + iCurr, SIZEOF(drmidLicEnabler.Data2 ) );
    iCurr+=SIZEOF(drmidLicEnabler.Data2);
    MEMCPY( &drmidLicEnabler.Data3, pUnkContainer->pObject->pbBuffer + pUnkContainer->pObject->ibData + iCurr, SIZEOF(drmidLicEnabler.Data3 ) );
    iCurr+=SIZEOF(drmidLicEnabler.Data3);
    MEMCPY( &drmidLicEnabler.Data4, pUnkContainer->pObject->pbBuffer + pUnkContainer->pObject->ibData + iCurr, SIZEOF(drmidLicEnabler.Data4 ) );
    iCurr+=SIZEOF(drmidLicEnabler.Data4);

    Log( "", "Guid From License : {%8X-%4X-%4X-%2X%2X-%2X%2X%2X%2X%2X%2X}",
         drmidLicEnabler.Data1,
         drmidLicEnabler.Data2,
         drmidLicEnabler.Data3,
         GET_BYTE( drmidLicEnabler.Data4, 0 ),
         GET_BYTE( drmidLicEnabler.Data4, 1 ),
         GET_BYTE( drmidLicEnabler.Data4, 2 ),
         GET_BYTE( drmidLicEnabler.Data4, 3 ),
         GET_BYTE( drmidLicEnabler.Data4, 4 ),
         GET_BYTE( drmidLicEnabler.Data4, 5 ),
         GET_BYTE( drmidLicEnabler.Data4, 6 ),
         GET_BYTE( drmidLicEnabler.Data4, 7 ) );

    if ( MEMCMP( ( DRM_BYTE *)&drmidEnabler, pUnkContainer->pObject->pbBuffer + pUnkContainer->pObject->ibData, SIZEOF(DRM_GUID) ) != 0 )
    {
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

ErrorExit:
    FREEDRMSTRING( dstrEnabler );
    return dr;
}


/*****************************************************************************
** Function:    TestStartLicense
**
** Synopsis:    Tests the DRM_Start_License methods
**
** Arguments:
**      argv[0]: LID( Random or test specific ones )
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL  TestStartLicense(long argc, __in_ecount( argc )char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD                                   dwVersion             = XMR_VERSION_3;
    DRM_ID                                      drmidLID;
    DRM_CONST_STRING                            dstrLID               = EMPTY_DRM_STRING;
    DRM_XMR_BUILDER_CONTEXT                     *pXmrBuilder          = NULL;
    DRM_BYTE                                    *pXmrBuilderStack     = NULL;

    ChkArg(argc==1);
    ChkArg(argv[0]!=NULL);
    MAP_TO_GLOBAL_TEST_VAR( pXmrBuilder, XMR_BUILDERCONTEXT );
    MAP_TO_GLOBAL_TEST_VAR( pXmrBuilderStack, XMRLICENSE_BUILDER_STACK);

    if( DRMCRT_strncmp( argv[0], "RANDOM", 6 ) == 0 )
    {
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&drmidLID, SIZEOF( DRM_GUID ) ) );
    }
    else
    {
        ChkDR( MakeDRMString( &dstrLID, argv[0] ) );
        ChkDR( DRM_UTL_StringToGuid( &dstrLID, (DRM_GUID*)&drmidLID ) );
    }

    ChkDR( DRM_XMR_StartLicense( pXmrBuilderStack,
        DRM_STACK_SIZE,
        &drmidLID,
        dwVersion,
        pXmrBuilder ) );

ErrorExit:
    FREEDRMSTRING( dstrLID );
    return dr;
}

static DRM_RESULT DRM_CALL  _FinishLicense( DRM_BYTE* pbKey )
{
    DRM_RESULT                   dr                                 = DRM_SUCCESS;
    DRM_XMR_BUILDER_CONTEXT      *pXmrBuilder                        = NULL;
    DRM_BYTE                     *pbLicense            = NULL;
    DRM_DWORD                     cbLicense            = 0;
    DRM_DWORD                    *pcbLicense           = &cbLicense;
    DRM_BOOL                     *pfUseBackup          = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pXmrBuilder, XMR_BUILDERCONTEXT );
    MAP_TO_GLOBAL_TEST_VAR( pfUseBackup, XMRLICENSE_BUFFER_USEBACKUP );
    MAP_TO_GLOBAL_TEST_VAR_EX( pbLicense, pcbLicense, *pfUseBackup ? XMRLICENSE_BUFFER_BACKUP : XMRLICENSE_BUFFER );

    if( *pcbLicense != 0 )
    {
        *pfUseBackup = TRUE;
    }

    dr = DRM_XMR_FinishLicense( pXmrBuilder,
                                pbKey,
                                _SignXmrLicense,
                                NULL,
                                pcbLicense );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
        ChkDR( DRM_E_FAIL );
    }
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbLicense, *pfUseBackup ? XMRLICENSE_BUFFER_BACKUP : XMRLICENSE_BUFFER, ( CCH_BASE64_EQUIV(*pcbLicense) + 1 ));
    MAP_TO_GLOBAL_TEST_VAR_EX( pbLicense, pcbLicense, *pfUseBackup ? XMRLICENSE_BUFFER_BACKUP : XMRLICENSE_BUFFER );
    ChkDR( DRM_XMR_FinishLicense( pXmrBuilder,
                                  pbKey,
                                  _SignXmrLicense,
                                  pbLicense,
                                  pcbLicense ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:     TestParseOuterContainersAndFinishLicense
**
** Synopsis:     Test the Parsing structures of Xmr builder
**
** Arguments:

**
** Returns:      DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL  TestParseOuterContainersAndFinishLicense(long argc,__in_ecount( argc )char **argv)
{
    DRM_RESULT                   dr= DRM_SUCCESS;
    DRM_XMR_BUILDER_CONTEXT      *pXmrBuilder      = NULL;
    DRM_XMR_LICENSE              *pXmrLicense      = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pXmrLicense, XMRLICENSE );
    MAP_TO_GLOBAL_TEST_VAR( pXmrBuilder, XMR_BUILDERCONTEXT );

    ChkDR( DRM_XMR_AddObject( pXmrBuilder, XMR_OBJECT_TYPE_OUTER_CONTAINER, &pXmrLicense->containerOuter) );

    ChkDR( _FinishLicense( NULL ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:     TestUnpackLicense
**
** Synopsis:     Test the DRM_XMR_Unpack_License
**
** Arguments:

**
** Returns:      DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/

DRM_RESULT DRM_CALL  TestUnpackLicense(long argc,__in_ecount( argc )char **argv)
{
    DRM_RESULT                       dr                  = DRM_SUCCESS;
    DRM_XMR_LICENSE                  *pXmrLicense        = NULL;
    DRM_BYTE                         *pbLicense          = NULL;
    DRM_DWORD                         cbLicense          = 0;
    DRM_DWORD                        *pcbLicense         = &cbLicense;
    DRM_BYTE                         *pXmrParserStack    = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT       stack              = {0};
    DRM_BOOL                         *pfUseBackup        = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pfUseBackup, XMRLICENSE_BUFFER_USEBACKUP );
    MAP_TO_GLOBAL_TEST_VAR_EX( pbLicense, pcbLicense, *pfUseBackup ? XMRLICENSE_BUFFER_BACKUP : XMRLICENSE_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pXmrLicense, XMRLICENSE );
    MAP_TO_GLOBAL_TEST_VAR( pXmrParserStack, XMRLICENSE_PARSER_STACK);

    ZEROMEM( pXmrParserStack, DRM_MAX_LICENSESIZE );
    ZEROMEM( pXmrLicense, SIZEOF( DRM_XMR_LICENSE ) );

    ChkDR( DRM_STK_Init( &stack, pXmrParserStack, DRM_MAX_LICENSESIZE ) );
    ChkDR( DRM_XMR_UnpackLicense(pbLicense, *pcbLicense, &stack, pXmrLicense) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:     TestVerifyUnpackedLicense
**
** Synopsis:     Verify the xmr objects after Unpack license
**               Searches the Unknown Containers or objects
**               in argv[0] for argv[1]
**
** Arguments:    argv[0]: Parent Type
**               argv[1]: Child Type
**               argv[2]: Value to compare (optional)
**
**               argv[0] and argv[1] repeat. argv[2] is present when needed.
**
** Returns:      DRM_SUCCESS                  - on success
**
** Note:
**               Supports Play and Copy Enablers and Unknown Containers
**               in the Outer Container only.
**
******************************************************************************/
DRM_RESULT DRM_CALL  TestVerifyUnpackedLicense(long argc, __in_ecount( argc ) char **argv)
{
    DRM_RESULT       dr                      = DRM_SUCCESS;
    DRM_WORD         wObjectType             = 0;
    DRM_WORD         wObjectType2            = 0;
    DRM_XMR_LICENSE *pXmrLicense             = NULL;
    long             iArg                    = 0;
    DRM_DWORD        cPlayEnablerExpected    = 0;
    DRM_DWORD        cCopyEnablerExpected    = 0;
    DRM_BOOL         fFound                  = FALSE;

    DRM_XMR_UNKNOWN_CONTAINER *pUnkContainer = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pXmrLicense, XMRLICENSE );

    if(argc<1)
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    for( iArg = 0; iArg < argc; iArg++ )
    {
        Log( "Verify", "\targv[%u]: %s", iArg, argv[iArg] );
    }

    for( iArg = 0; iArg < argc; iArg++ )
    {
        ChkDR( _GetXMRType(argv[iArg], &wObjectType) );

        switch(wObjectType)
        {
            case XMR_OBJECT_TYPE_OUTER_CONTAINER:
                ChkArg( iArg + 1 < argc );

                pUnkContainer = &pXmrLicense->containerOuter.containerUnknown;
                fFound = FALSE;

                ChkDR( _GetXMRType( argv[ iArg+1 ], &wObjectType2 ) );

                /*
                ** Look for the unknown object.
                */
                while( pUnkContainer != NULL && pUnkContainer->fValid && !fFound )
                {
                    fFound = ( pUnkContainer->wType == wObjectType2 );
                    pUnkContainer = pUnkContainer->pNext;
                }
                ChkBOOL( fFound, DRM_E_TEST_UNEXPECTED_OUTPUT );
                iArg+=1;
                break;

            case XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER: /* fall through */
            case XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER:
                fFound = FALSE;
                if( wObjectType == XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER )
                {
                    pUnkContainer = &pXmrLicense->containerOuter.containerPlaybackPolicies.UnknownContainer;
                }
                else
                {
                    pUnkContainer = &pXmrLicense->containerOuter.containerCopyPolicies.UnknownContainer;
                }

                while( pUnkContainer != NULL && pUnkContainer->fValid && !fFound )
                {
                    if(pUnkContainer->wType != wObjectType)
                    {
                        ChkDR(DRM_E_TEST_UNEXPECTED_OUTPUT);
                    }
                    dr = _TestVerifyEnablerGuid( argc, argv, pUnkContainer, iArg );
                    if( dr == DRM_SUCCESS )
                    {
                        fFound = TRUE;
                    }
                    else if( dr == DRM_S_FALSE )
                    {
                        dr = DRM_SUCCESS;
                        pUnkContainer = pUnkContainer->pNext;
                    }
                    else
                    {
                        ChkDR( dr );
                    }
                }
                if( !fFound )
                {
                    ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                }

                if( wObjectType == XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER )
                {
                    cPlayEnablerExpected++;
                }
                else
                {
                    cCopyEnablerExpected++;
                }
                iArg += 2; /* For the enabler type / GUID */
                break;

            default:
                ChkDR( DRM_E_NOTIMPL );
                break;
        }
    }

    pUnkContainer = &pXmrLicense->containerOuter.containerPlaybackPolicies.UnknownContainer;
    while( cPlayEnablerExpected > 0 )
    {
        if( pUnkContainer == NULL || !pUnkContainer->fValid )
        {
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
        pUnkContainer = pUnkContainer->pNext;
        cPlayEnablerExpected--;
    }
    if( pUnkContainer != NULL && pUnkContainer->fValid )
    {
        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    pUnkContainer = &pXmrLicense->containerOuter.containerCopyPolicies.UnknownContainer;
    while( cCopyEnablerExpected > 0 )
    {
        if( pUnkContainer == NULL || !pUnkContainer->fValid )
        {
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
        pUnkContainer = pUnkContainer->pNext;
        cCopyEnablerExpected--;
    }
    if( pUnkContainer != NULL && pUnkContainer->fValid )
    {
        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:     TestAddUnknownXMRObject
**
** Synopsis:     Adds an unknown object to the license
**
** Arguments:   argv[0] -- Object Type (word)
**              argv[1] -- Parent Object Type (word)
**              argv[2] -- Is a duplicate object allowed? (BOOL value)
**              argv[3] -- Flags
**              argv[4] -- Size of the object to add. Adds random data of this size.
**
** Returns:      DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestAddUnknownXMRObject( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr          = DRM_SUCCESS;
    DRM_XMR_BUILDER_CONTEXT *pXmrBuilder = NULL;
    DRM_WORD                 wType       = XMR_OBJECT_TYPE_INVALID;
    DRM_WORD                 wParentType = XMR_OBJECT_TYPE_INVALID;
    DRM_WORD                 wFlags      = XMR_FLAGS_NONE;
    DRM_DWORD                cbObject    = 0;
    DRM_BYTE                *pbObject    = NULL;
    DRM_BOOL                 fDuplicate  = FALSE;

    ChkArg( argc >  4 );
    ChkArg( NULL != argv[0] );
    ChkArg( NULL != argv[1] );
    ChkArg( NULL != argv[2] );
    ChkArg( NULL != argv[3] );
    ChkArg( NULL != argv[4] );

    MAP_TO_GLOBAL_TEST_VAR( pXmrBuilder, XMR_BUILDERCONTEXT );

    wType       = ( DRM_WORD )  strtoul( argv[0], NULL, 0 );
    wParentType = ( DRM_WORD )  strtoul( argv[1], NULL, 0 );
    wFlags      = ( DRM_WORD )  strtoul( argv[3], NULL, 0 );
    cbObject    = ( DRM_DWORD ) strtoul( argv[4], NULL, 0 );

    ChkDR( StringToBool( argv[2], &fDuplicate ) );

    if( 0 != cbObject )
    {
        ChkMem( pbObject = ( DRM_BYTE * ) Oem_MemAlloc( cbObject ) );
    }

    ChkDR( DRM_XMR_AddUnknownObject( pXmrBuilder,
                                     wType,
                                     fDuplicate,
                                     wParentType,
                                     wFlags,
                                     pbObject,
                                     cbObject ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:     TestAddXMRObjectsAndFinishLicense
**
** Synopsis:     Add the xmr objects
**
*** Arguments:  argv[0] : priority object
**              argv[1] : security level
**              argv[2] : Playback container
**              argv[3] : Playcount
**              argv[4] : Copy container
**              argv[5] : copy count
**              argv[6] : metering ID
**              argv[7] : KID
**              argv[8] : play enablers (count)
**              argv[9] : play enablers
**              argv[10]: playenablers
**              argv[11]: copy enablers (count)
**              argv[12]: copy enablers
**              argv[13]: copy enablers
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestAddXMRObjectsAndFinishLicense( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                                  dr = DRM_SUCCESS;
    PUBKEY_P256                                 pubKey;
    PRIVKEY_P256                                privKey;
    DRM_ID                                      drmidKID;
    DRM_CONST_STRING                            dstrKID = EMPTY_DRM_STRING;
    DRM_CONST_STRING                            dstrSeed = EMPTY_DRM_STRING;
    DRM_XMR_METERING                            xmrMID;
    DRM_XMR_PRIORITY                            xmrPriority;
    DRM_XMR_PLAYBACK_RIGHTS                     xmrPlay = {0};
    DRM_XMR_COPY_RIGHTS                         xmrCopy = {0};
    DRM_XMR_MINIMUM_ENVIRONMENT                 xmrMinEnv;
    DRM_ID                                      drmidMID;
    DRM_XMR_CONTENT_KEY                         contentKey;
    DRM_XMR_ECC_DEVICE_KEY                      deviceKey;
    DRM_XMR_EMBEDDING_BEHAVIOR                  xmrEmbeddingBehavior;
    CIPHERTEXT_P256                             encryptedContentKey;
    DRM_CRYPTO_CONTEXT                          *pcryptoContext = NULL;
    DRM_BYTE                                    rgbKey[ DRM_AES_KEYSIZE_128 * 2 ] = { 0 };
    DRM_CHAR                                    *pstrRandomKeyID = NULL;
    DRM_CONST_STRING                            rgdstrDTCPGuid[DRM_MAX_ENABLERS]    = {0};
    DRM_CONST_STRING                            rgdstrCSSGuid[DRM_MAX_ENABLERS]     = {0};
    DRM_XMR_BUILDER_CONTEXT                     *pXmrBuilder    = NULL;
    DRM_CONST_STRING                            dstrContentKey  = EMPTY_DRM_STRING;
    DRM_DWORD                                   cbBuffer        = 0;
    DRM_DWORD                                   iEnabler        = 0;
    DRM_XMR_REVOCATION_INFORMATION_VERSION      xmrRIV = {0};
    DRM_GUID                                    drmidCSS[DRM_MAX_ENABLERS]      = {0};
    DRM_GUID                                    drmidDTCP[DRM_MAX_ENABLERS]     = {0};
    DRM_DWORD                                   cbsize                          = SIZEOF(DRM_GUID);
    DRM_BYTE                                    *pbScratchKeySeed = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pXmrBuilder, XMR_BUILDERCONTEXT );

    ChkMem(  pcryptoContext = (DRM_CRYPTO_CONTEXT*) Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pcryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    if ( argc >= 1
      && argv[0] != NULL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_PRIORITY_OBJECT, &xmrPriority ) );
        xmrPriority.dwValue = strtoul( argv[0], NULL, 0 );
        ChkDR( DRM_XMR_AddObject( pXmrBuilder, XMR_OBJECT_TYPE_PRIORITY_OBJECT, &xmrPriority ) );
    }
    /*
    ** security level
    */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SECURITY_LEVEL, &xmrMinEnv ) );
    xmrMinEnv.fValid                = TRUE;
    if ( argc >= 2
      && argv[1] != NULL )
    {
        xmrMinEnv.wMinimumSecurityLevel = (DRM_WORD)strtoul( argv[1], NULL, 0 );
    }
    else
    {
        xmrMinEnv.wMinimumSecurityLevel = (DRM_WORD)150;
    }
    ChkDR( DRM_XMR_AddObject( pXmrBuilder, XMR_OBJECT_TYPE_SECURITY_LEVEL, &xmrMinEnv ) );

    /*
    ** Playback policy
    */
    if ( argc >= 3
      && argv[2] != NULL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, &xmrPlay) );
         xmrPlay.fValid            = TRUE;
        if ( argc >= 4
          && argv[3] != NULL )
        {
            xmrPlay.PlayCount.fValid  = TRUE;
            xmrPlay.PlayCount.dwValue = strtoul( argv[3], NULL, 0 );
        }

        ChkDR( DRM_XMR_AddObject( pXmrBuilder,
            XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,
            &xmrPlay) );
    }

    /*
    ** Copy policy
    */
    if ( argc >= 5
      && argv[4] != NULL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER, &xmrCopy ) );
        xmrCopy.fValid            = TRUE;
        if ( argc >= 6
            && argv[5 ]!=NULL )
        {
            xmrCopy.CopyCount.fValid  = TRUE;
            xmrCopy.CopyCount.dwValue = strtoul( argv[5], NULL, 0 );
        }
        ChkDR( DRM_XMR_AddObject( pXmrBuilder,
            XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER,
            &xmrCopy ) );
    }

    /*
    ** metering ID
    */
    if ( argc >= 7
       && argv[6] != NULL )
    {
        DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;

        ChkDR( MakeDRMString( &dstrMID, argv[6] ) );
        ChkDR( DRM_UTL_DecodeKID( &dstrMID, &drmidMID ) );
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_METERING_OBJECT, &xmrMID ) );
        xmrMID.pguidBuffer = drmidMID.rgb;
        xmrMID.iGuid = 0;
        ChkDR( DRM_XMR_AddObject( pXmrBuilder, XMR_OBJECT_TYPE_METERING_OBJECT, &xmrMID ) );
    }

    /*
    ** Get shared random KID if it exists
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( RANDOM_KID_VAR, ( DRM_VOID ** ) &pstrRandomKeyID, NULL ) ) )
    {
        pstrRandomKeyID = NULL;
    }

    ChkDR( MakeDRMString( &dstrSeed, "XVBovsmzhP9gRIZxWfFta3VVRPzVEWmJsazEJ46I") );

    /*
    ** Allocate memory for scratch buffer used in content key generation functions.
    */
#pragma prefast(push)
#pragma prefast(disable:22022) /* ignore arithmetic overflow warning, already handled */
    ChkOverflow( dstrSeed.cchString * SIZEOF(DRM_WCHAR), dstrSeed.cchString );
    ChkMem( pbScratchKeySeed = (DRM_BYTE *)Oem_MemAlloc( dstrSeed.cchString * SIZEOF(DRM_WCHAR) ) );
    ZEROMEM( pbScratchKeySeed, dstrSeed.cchString * SIZEOF(DRM_WCHAR) );
#pragma prefast(pop)

    /*
    ** Pick up KID
    */
    if( argc >= 8 )
    {
        if( DRMCRT_strncmp( argv[7], "PREGENERATEDRANDOM", 18 ) == 0 && pstrRandomKeyID != NULL)
        {
            ChkDR( MakeDRMString( &dstrKID, pstrRandomKeyID ) );
        }
        else if( DRMCRT_strncmp( argv[7], "RANDOM", 6 ) == 0 )
        {
            ChkMem( dstrKID.pwszString = (DRM_WCHAR*)Oem_MemAlloc( ( DRM_GUID_STRING_LEN + 1 ) * SIZEOF( DRM_WCHAR ) ) );
            dstrKID.cchString = DRM_GUID_STRING_LEN + 1;
            ChkDR( DRM_KG_GenerateKeyID( NULL, (DRM_WCHAR*)dstrKID.pwszString, &dstrKID.cchString ) );
        }
    }

    ChkDR( DRM_UTL_DecodeKID( &dstrKID, &drmidKID ) );
    /*
    ** Generate content key
    */
    dstrContentKey.cchString = CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 );
    ChkMem( dstrContentKey.pwszString = (DRM_WCHAR*)Oem_MemAlloc( ( dstrContentKey.cchString + 1 ) * SIZEOF( DRM_WCHAR ) ) );
    ChkDR( DRM_KG_GenerateContentKeyB64( dstrSeed.pwszString,
                                         dstrSeed.cchString,
                                         dstrKID.pwszString,
                                         dstrKID.cchString,
                                         eDRM_AES_COUNTER_CIPHER,
                                         DRM_AES_KEYSIZE_128,
                             (DRM_WCHAR*)dstrContentKey.pwszString,
                                        &dstrContentKey.cchString,
                                        pbScratchKeySeed,
                                        dstrSeed.cchString * SIZEOF(DRM_WCHAR)) );
    cbBuffer = SIZEOF( rgbKey ) - DRM_AES_KEYSIZE_128;
    ChkDR( DRM_B64_DecodeW( &dstrContentKey, &cbBuffer, &rgbKey[DRM_AES_KEYSIZE_128], 0 ) );
    ChkBOOL( (cbBuffer == DRM_AES_KEYSIZE_128 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Generate rand integrity key
    */
    ChkDR( OEM_ECC_GenerateHMACKey_P256( (PLAINTEXT_P256 *)rgbKey, (struct bigctx_t *)&pcryptoContext->rgbCryptoContext ) );

    /*
    ** Generate random key pair
    */
    ChkDR( OEM_ECC_GenKeyPair_P256( &pubKey, &privKey, (struct bigctx_t *)&pcryptoContext->rgbCryptoContext ) );

    /*
    ** Encrypt content with asym public key
    */
    ChkDR( OEM_ECC_Encrypt_P256( &pubKey, (PLAINTEXT_P256 *)rgbKey, &encryptedContentKey, (struct bigctx_t *)&pcryptoContext->rgbCryptoContext ) );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT, &deviceKey ) );
    deviceKey.fValid          = TRUE;
    deviceKey.wEccCurveType   = XMR_ECC_CURVE_TYPE_P256;
    deviceKey.pbKeyData       = pubKey.m_rgbPubkey;
    deviceKey.cbKeyData       = sizeof( PUBKEY_P256 );
    deviceKey.iKeyData        = 0;
    ChkDR( DRM_XMR_AddObject( pXmrBuilder, XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT,      &deviceKey ) );

    /*
    ** Set up and add the encrypted content key to the license
    */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT,    &contentKey ) );

    contentKey.pbguidKeyID               = drmidKID.rgb;
    contentKey.iguidKeyID                = 0;
    contentKey.wSymmetricCipherType      = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR;
    contentKey.wKeyEncryptionCipherType  = XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256;
    contentKey.pbEncryptedKeyBuffer      = encryptedContentKey.m_rgbCiphertext;
    contentKey.cbEncryptedKey            = ECC_P256_CIPHERTEXT_SIZE_IN_BYTES;//static_cast<DRM_WORD>(ECC_P256_CIPHERTEXT_SIZE_IN_BYTES);
    contentKey.iEncryptedKey             = 0;

    ChkDR( DRM_XMR_AddObject( pXmrBuilder, XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT,         &contentKey ) );

    if( (argc >= 9)
        && argv[8] != NULL )
    {
        DRM_DWORD   dwArgOffset                     = 8;
        DRM_DWORD   cEnablers                       = (DRM_DWORD)(atol(argv[8]));
        DRM_DWORD   dwIndex                         = 0;

        ChkArg(
            ( cEnablers > 0 )
         && ( cEnablers <= DRM_MAX_ENABLERS )
         && ( (DRM_DWORD)argc >= ( cEnablers + dwArgOffset ) ) );

        for( iEnabler = 0; iEnabler < cEnablers; iEnabler++ )
        {
            dwIndex = iEnabler + dwArgOffset + 1;
            ChkArg( dwIndex < (DRM_DWORD)argc );
            ChkArg( argv[dwIndex] != NULL );

            ChkDR( MakeDRMString( &rgdstrDTCPGuid[iEnabler], argv[dwIndex] ) );
            ChkDR( DRM_UTL_StringToGuid(&rgdstrDTCPGuid[iEnabler], &drmidDTCP[iEnabler]) );
            ChkDR( DRM_XMR_AddUnknownObject(
                pXmrBuilder, XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER, TRUE, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, XMR_FLAGS_CONTAINER, NULL, 0));
            ChkDR( DRM_XMR_AddUnknownObject(
                pXmrBuilder, XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT, TRUE, XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER, 0,  (DRM_BYTE*)&drmidDTCP[iEnabler], cbsize));
        }
    }

    if( (argc >= 12)
      && argv[11] != NULL )
    {
        DRM_DWORD   dwArgOffset                     = 11;
        DRM_DWORD   cEnablers                       = (DRM_DWORD)(atol(argv[11]));
        DRM_DWORD   dwIndex                         = 0;

        ChkArg(
            ( cEnablers > 0 )
         && ( cEnablers <= DRM_MAX_ENABLERS )
         && ( (DRM_DWORD)argc >= ( cEnablers + dwArgOffset ) ) );

        for( iEnabler = 0; iEnabler < cEnablers; iEnabler++ )
        {
            dwIndex = iEnabler + dwArgOffset + 1;
            ChkArg( dwIndex < (DRM_DWORD)argc );
            ChkArg( argv[dwIndex] != NULL );

            ChkDR( MakeDRMString( &rgdstrCSSGuid[iEnabler], argv[dwIndex] ) );
            ChkDR( DRM_UTL_StringToGuid(&rgdstrCSSGuid[iEnabler], &drmidCSS[iEnabler]) );
            ChkDR( DRM_XMR_AddUnknownObject(
                pXmrBuilder, XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER, TRUE, XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER, XMR_FLAGS_CONTAINER, NULL, 0));
            ChkDR( DRM_XMR_AddUnknownObject(
                pXmrBuilder, XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT, TRUE, XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER, 0,  (DRM_BYTE*)&drmidCSS[iEnabler], cbsize));
        }
    }
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT, &xmrEmbeddingBehavior ) );
    xmrEmbeddingBehavior.wValue = XMR_EMBEDDING_BEHAVIOR_IGNORE;
    ChkDR( DRM_XMR_AddObject( pXmrBuilder, XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT, &xmrEmbeddingBehavior ) );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, &xmrRIV ) );
    xmrRIV.fValid  = TRUE;
    xmrRIV.dwValue = 0;
    ChkDR( DRM_XMR_AddObject( pXmrBuilder, XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, &xmrRIV ) );

    ChkDR( _FinishLicense( rgbKey ) );

ErrorExit:
    FREEDRMSTRING( dstrKID );
    FREEDRMSTRING( dstrSeed );
    for( iEnabler = 0; iEnabler < DRM_MAX_ENABLERS; iEnabler++ )
    {
        FREEDRMSTRING( rgdstrDTCPGuid[iEnabler] );
        FREEDRMSTRING( rgdstrCSSGuid[iEnabler] );
    }
    SAFE_OEM_FREE( pcryptoContext );
    SAFE_OEM_FREE( pbScratchKeySeed );
    return dr;
}


DRM_RESULT DRM_CALL MODULE_PRETESTCASE(xmr)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr= DRM_SUCCESS;
    DRM_XMR_BUILDER_CONTEXT *pXmrBuilder            = NULL;
    DRM_XMR_LICENSE           *pXmrLicense          = NULL;
    DRM_BYTE                  *pXmrBuilderStack     = NULL;
    DRM_BYTE                  *pXmrParserStack      = NULL;
    DRM_BYTE                  *pbLicense            = NULL;
    DRM_BYTE                  *pbLicenseBackup      = NULL;
    DRM_BOOL                  *pfUseBackup          = NULL;
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pXmrBuilder, XMR_BUILDERCONTEXT, SIZEOF(DRM_XMR_BUILDER_CONTEXT) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pXmrLicense, XMRLICENSE, SIZEOF(DRM_XMR_LICENSE) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pXmrBuilderStack, XMRLICENSE_BUILDER_STACK, DRM_STACK_SIZE);
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pXmrParserStack, XMRLICENSE_PARSER_STACK, DRM_STACK_SIZE);
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbLicense, XMRLICENSE_BUFFER, 0);
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbLicenseBackup, XMRLICENSE_BUFFER_BACKUP, 0);
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pfUseBackup, XMRLICENSE_BUFFER_USEBACKUP, SIZEOF(DRM_BOOL));
    *pfUseBackup = FALSE;

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(xmr)(long lTCID, const char *strTCName)
{

    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST(xmr)
    API_ENTRY(TestAddUnknownXMRObject)
    API_ENTRY(TestStartLicense)
    API_ENTRY(TestAddXMRObjectsAndFinishLicense)
    API_ENTRY(TestUnpackLicense)
    API_ENTRY(TestVerifyUnpackedLicense)
    API_ENTRY(TestParseOuterContainersAndFinishLicense)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
