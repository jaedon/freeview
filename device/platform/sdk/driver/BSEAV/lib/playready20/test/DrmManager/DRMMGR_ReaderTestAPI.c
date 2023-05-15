/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DrmManagerTestAPI.h>
#include <drmkeygenerator.h>

ENTER_PKTEST_NAMESPACE_CODE

#define PLAYRIGHT 4
#define RIGHT1234 1234
#define EXECUTERIGHT 63
#define RIGHT100  100
#define RIGHT1000 1000

#define RESTRICTION1000 1000
#define RESTRICTION1001 1001
#define RESTRICTION200  200
#define RESTRICTION201  201
#define RESTRICTION2001 2001
#define RESTRICTION2002 2002

/*
** output protection level
*/
const char *g_guidWMDRMDeviceCopy = "{00000507-0001-0010-8000-00AA006D2EA4}";
const char *g_guidPDDRMDeviceCopy = "{00000507-0002-0010-8000-00AA006D2EA4}";
const char *g_guidExtension1 = "{00000507-0005-0010-8000-00AA006D2EA4}";
const char *g_guidExtension2 = "{00000507-0006-0010-8000-00AA006D2EA4}";
const char *g_guidExtension3 = "{00000507-0007-0010-8000-00AA006D2EA4}";
const DRM_DWORD g_dwExtension1 = 0x00000001;
const DRM_DWORD g_dwExtension2 = 0x0001A633;

/*****************************************************************************
** Function:    TestManagerPrepareOPLCallback
**
** Synopsis:    
**
** Arguments:   argv[0]: OPL - wCompressedDigitalVideo
**              argv[1]: OPL - wUncompressedDigitalVideo
**              argv[2]: OPL - wAnalogVideo
**              argv[3]: OPL - wCompressedDigitalAudio
**              argv[4]: OPL - wUncompressedDigitalAudio
**              argv[5]: OPL - <currently unused>
**              argv[6]: OPL - extension count
**              argv[7]: (optional) Extended Restriction Module (default 0)
**              argv[8]: (optional) Restricted Source ID to Allow (default 0)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerPrepareOPLCallback( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr                 = DRM_SUCCESS;
    DRM_WORD  *pwOPL              = NULL;
    DRM_DWORD *pdwExtRestrictMod  = NULL;
    DRM_DWORD *pdwSourceID        = NULL;
    DRM_BOOL  *pfNeedsCallBack    = NULL;


    ChkArg( argc > 6 );

    MAP_TO_GLOBAL_TEST_VAR( pfNeedsCallBack, NEEDS_CALLBACK_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pwOPL, OPL_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pdwExtRestrictMod, EXT_RESTRICTION_MODULE_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pdwSourceID, RESTRICTED_SOURCE_ID_VAR );
    
    
#if SIXTEEN_BIT_ADDRESSING
    pwOPL[0]=(DRM_WORD)atoi(argv[0]);
    pwOPL[1]=(DRM_WORD)atoi(argv[1]);
    pwOPL[2]=(DRM_WORD)atoi(argv[2]);
    pwOPL[3]=(DRM_WORD)atoi(argv[3]);
    pwOPL[4]=(DRM_WORD)atoi(argv[4]);
    pwOPL[5]=(DRM_WORD)atoi(argv[5]);
    pwOPL[6]=(DRM_WORD)atoi(argv[6]);
#else
    pwOPL[0]=(DRM_WORD)atol(argv[0]);
    pwOPL[1]=(DRM_WORD)atol(argv[1]);
    pwOPL[2]=(DRM_WORD)atol(argv[2]);
    pwOPL[3]=(DRM_WORD)atol(argv[3]);
    pwOPL[4]=(DRM_WORD)atol(argv[4]);
    pwOPL[5]=(DRM_WORD)atol(argv[5]);
    pwOPL[6]=(DRM_WORD)atol(argv[6]);
#endif  


 
    if( argc > 7 && argv[7] != NULL)
    {
        ChkDR( DRMCRT_AtoDWORD( argv[7], DRMCRT_strlen( argv[7] ), 10, pdwExtRestrictMod ) );
        *pfNeedsCallBack = TRUE;
    }
    else
    {
        *pdwExtRestrictMod = 0;
        *pfNeedsCallBack = FALSE;
    }

    if( argc > 8 && argv[8] != NULL)
    {
        ChkDR( DRMCRT_AtoDWORD( argv[8], DRMCRT_strlen( argv[8] ), 10, pdwSourceID ) );
    }
    else
    {
        *pdwSourceID = 0;
    }

    TRACE( ( "OPL Callback: \n"
             "\tCompressedDigitalVideo   %d \n"
             "\tUncompressedDigitalVideo %d \n"
             "\twAnalogVideo             %d \n"
             "\tCompressedDigitalAudio   %d \n"
             "\tUncompressedDigitalAudio %d \n"
             "\tCurrently Unused         %d \n"
             "\tExtension count          %d \n"
             "\tExtensible Policy Module %d \n"
             "\tSource ID                %d \n",
             pwOPL[0], 
             pwOPL[1],
             pwOPL[2],
             pwOPL[3],
             pwOPL[4],
             pwOPL[5],
             pwOPL[6],
            *pdwExtRestrictMod,
            *pdwSourceID ) );

ErrorExit:
    return dr;  
}

/*****************************************************************************
** Function:    OPLCallback
**
** Synopsis:    
**
** Arguments:   f_pvOutputLevelsData : 
**              f_dwCallbackType : 
**              f_pv : 
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL OPLCallback(
    IN const DRM_VOID                 *f_pvOutputLevelsData,
    IN       DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
    IN const DRM_VOID                 *f_pv )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_PLAY_OPL_EX *f_pOutputLevels   = NULL; 
    DRM_GUID         localGuid1        = EMPTY_DRM_GUID;
    DRM_GUID         localGuid3        = EMPTY_DRM_GUID;
    DRM_CONST_STRING wszGUID           = {0};
    DRM_CONST_STRING dstrAttribute     = EMPTY_DRM_STRING;
    DRM_WORD        *pwOPL             = NULL;
    DRM_DWORD       *pdwExtRestrictMod = NULL;
    DRM_DWORD       *pdwSourceID       = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pwOPL, OPL_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pdwExtRestrictMod, EXT_RESTRICTION_MODULE_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pdwSourceID, RESTRICTED_SOURCE_ID_VAR );
 
    if( f_dwCallbackType == DRM_PLAY_OPL_CALLBACK )
    {
        f_pOutputLevels = (DRM_PLAY_OPL_EX*)f_pvOutputLevelsData;
        /* 
        ** 1. check wCompressedDigitalVideo;
        ** wUncompressedDigitalVideo;
        ** wAnalogVideo;
        ** wCompressedDigitalAudio;
        ** wUncompressedDigitalAudio;
        */

        /*
        ** check the min level of wCompressedDigitalVideo
        */
        if( f_pOutputLevels->minOPL.wCompressedDigitalVideo != pwOPL[0] )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }       
        /*
        ** check the min level of wUncompressedDigitalVideo
        */
        if( f_pOutputLevels->minOPL.wUncompressedDigitalVideo != pwOPL[1] )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }       
        /*
        ** check the min level of wAnalogVideo
        */
        if( f_pOutputLevels->minOPL.wAnalogVideo != pwOPL[2] )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }       
        /*
        ** check the min level of wCompressedDigitalAudio
        */
        if( f_pOutputLevels->minOPL.wCompressedDigitalAudio != pwOPL[3] )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }       
        /*
        ** check the min level of wUncompressedDigitalAudio
        */
        if( f_pOutputLevels->minOPL.wUncompressedDigitalAudio != pwOPL[4] )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }       

        /*
        ** 2. the number of extension entries should be matched
        */
        if( f_pOutputLevels->vopi.cEntries != pwOPL[6] ) 
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH ); 
        }
    
        /*
        ** extension GUID and configuration data, up to two
        */
        if( f_pOutputLevels->vopi.cEntries >=1 )
        {
            ChkDR( MakeDRMString( &wszGUID, g_guidExtension1 ) );
            DRM_UTL_StringToGuid( &wszGUID, (DRM_GUID*)&localGuid1 );
            FREEDRMSTRING( wszGUID );
            
            ChkDR( MakeDRMString( &wszGUID, g_guidExtension3 ) );
            DRM_UTL_StringToGuid( &wszGUID, (DRM_GUID*)&localGuid3 );
            FREEDRMSTRING( wszGUID );
            
            if( MEMCMP( &(f_pOutputLevels->vopi.rgVop[0].guidId), &localGuid1, SIZEOF( DRM_GUID ) ) == 0 )
            {
                if( MEMCMP( &(f_pOutputLevels->vopi.rgVop[0].dwConfigData), &g_dwExtension1, SIZEOF( DRM_DWORD ) ) != 0 )
                {
                    ChkDR( DRM_E_TEST_OPL_MISMATCH );
                }
            }
            else if( MEMCMP( &(f_pOutputLevels->vopi.rgVop[0].guidId), &localGuid3, SIZEOF( DRM_GUID ) ) == 0 )
            {
                /*
                ** if we find Guid 3 then we expect no config data
                */
                if( f_pOutputLevels->vopi.rgVop[0].dwConfigData != 0 )
                {
                    ChkDR( DRM_E_TEST_OPL_MISMATCH );
                }
            }
            else
            {
                ChkDR( DRM_E_TEST_OPL_MISMATCH );
            }
            
        }

        if( f_pOutputLevels->vopi.cEntries >= 2 )
        {
            ChkDR( MakeDRMString( &wszGUID, g_guidExtension2 ) );
            DRM_UTL_StringToGuid( &wszGUID, (DRM_GUID*)&localGuid1 );
            FREEDRMSTRING( wszGUID );
            if( MEMCMP( &(f_pOutputLevels->vopi.rgVop[1].guidId), &localGuid1, SIZEOF( DRM_GUID ) ) != 0 )
            {
                 ChkDR( DRM_E_TEST_OPL_MISMATCH );
            }
            if( MEMCMP( &(f_pOutputLevels->vopi.rgVop[1].dwConfigData), &g_dwExtension2, SIZEOF( DRM_DWORD ) ) != 0 )
            {
                 ChkDR( DRM_E_TEST_OPL_MISMATCH );
            }
        }
    }
    else if( f_dwCallbackType == DRM_COPY_OPL_CALLBACK )
    {
        ChkDR( DRM_E_TEST_INVALID_OPL_CALLBACK );
    }
    else if( f_dwCallbackType == DRM_INCLUSION_LIST_CALLBACK )
    {
        dr = DRM_SUCCESS;
    }
    else if( f_dwCallbackType == DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK )
    {
        /* 
        ** This callback gives the application a chance to see all the extensible restrictions possible
        ** during a license query 
        */
    }
    else if( f_dwCallbackType == DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK )
    {
        /* does nothing */
    }
    else if( f_dwCallbackType == DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK )
    {
        /* does nothing */
    }
    else if ( f_dwCallbackType == DRM_RESTRICTED_SOURCEID_CALLBACK )
    {
        DRM_RESTRICTED_SOURCEID_CALLBACK_STRUCT *pCallback = (DRM_RESTRICTED_SOURCEID_CALLBACK_STRUCT*)f_pvOutputLevelsData;
        
        if( *pdwSourceID != pCallback->dwSourceID )
        {
            TRACE(("SourceID reported: %u", pCallback->dwSourceID));
            TRACE(("SourceID Expected: %u", *pdwSourceID));
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }
    else 
    {
        TRACE( ( "Unknown Callback type: %u", f_dwCallbackType ) );
        ChkDR( DRM_E_TEST_INVALID_OPL_CALLBACK );
    }

ErrorExit:
    FREEDRMSTRING( dstrAttribute );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerBind
**
** Synopsis:    
**
** Arguments:   argv[0] : indicates if drmmanager or decryptor context should be overwritten, or saved:
**                  0x1 - use argv[1] to overwrite drmmanager context
**                  0x2 - use argv[2] to overwrite decrypt context
**                  0x4 - load decrypt context from a test variable named argv[2]
**                  0x8 - argv[2] is a name of a test variable to save decrypt context in
**              argv[1] : the context to overwrite to the local drmmanager context
**              argv[2] : the context to overwrite to the local decrypt context
**              argv[3] : indicates use of callback function for output protection level
**              argv[4] : optional, maximum number of milliseconds Bind should take
**              argv[5] : optional, specifies the expected LID (leaf or simple)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:        Option 0x2 obviously is not compatible with options 0x4 and 0x8
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerBind( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT     *pLocalManagerContext   = NULL;
    DRM_DECRYPT_CONTEXT *pLocalDecryptContext   = NULL;
    DRM_DECRYPT_CONTEXT *pSharedDecryptContext  = NULL;
    DRM_LONG             iOption                = 0;
    DRM_BOOL            *pfNeedsCallBack        = NULL;
    DRM_DWORD           *pdwNumRights           = NULL;
    DRM_CONST_STRING   **ppRightsArray          = NULL;
    DRM_LID              oVerifyLID             = { 0 };
    DRM_CONST_STRING     wszVerifyLID           = { 0 };
    DRM_WCHAR            pwszVerifyLID[256]     = { 0 };

#if DRM_SUPPORT_PROFILING
    DRM_UINT64  ui64BindStartTime = DRM_UI64LITERAL(0,0);
    DRM_UINT64  ui64MaxBindTime   = DRM_UI64LITERAL(0,0);
#endif

    ChkArg( argc > 0 );  

    MAP_TO_GLOBAL_TEST_VAR( pfNeedsCallBack, NEEDS_CALLBACK_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pdwNumRights, NUMBER_OF_RIGHTS_VAR );
    MAP_TO_GLOBAL_TEST_VAR( ppRightsArray, RIGHTS_ARRAY_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pSharedDecryptContext, DECRYPT_CONTEXT_VAR );

#if SIXTEEN_BIT_ADDRESSING
    iOption = (DRM_LONG)atoi( argv[0] );
#else
    iOption = (DRM_LONG)atol( argv[0] );
#endif

    /* 
    ** make sure the default output buffer for Drm_Reader_Bind is reset 
    */
    MEMSET( pSharedDecryptContext, 0, SIZEOF( DRM_DECRYPT_CONTEXT ) ); 
    
    /*
    ** manager context
    */
    if ( iOption&1 ) 
    { 
        /*
        ** Context is from the input
        */
        if ( argc > 1 && argv[1] != NULL )
        {
            ChkMem( pLocalManagerContext = (DRM_APP_CONTEXT *)Oem_MemAlloc( SIZEOF( DRM_APP_CONTEXT ) ) );
            MEMSET( pLocalManagerContext, 0, SIZEOF( DRM_APP_CONTEXT ) ); 
            MEMCPY( pLocalManagerContext->rgbOpaqueBuffer, argv[1], min( DRMCRT_strlen( argv[1] ) + 1, DRM_APP_CONTEXT_BUFFER_SIZE ) );
        }
    }
    else
    {
        /*
        ** Context is from the shared variable
        */
        GET_SHARED_APP_CONTEXT( pLocalManagerContext );
    }

    /*
    ** decrypt context
    */
    if ( iOption&2 ) 
    { 
        /*
        ** Context is from the input
        */
        if ( argc > 2 && argv[2] != NULL )
        {
            ChkMem( pLocalDecryptContext = (DRM_DECRYPT_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_DECRYPT_CONTEXT ) ) );
            MEMSET( pLocalDecryptContext, 0, SIZEOF( DRM_DECRYPT_CONTEXT ) ); 
            MEMCPY( pLocalDecryptContext->rgbBuffer, argv[2], min( DRMCRT_strlen( argv[2] ) + 1, SIZEOF( DRM_CIPHER_CONTEXT ) ) );
        }
    }
    else
    {
        /*
        ** Context is from the shared variable
        */
        pLocalDecryptContext = pSharedDecryptContext;
    }


    if (  iOption & 4 ) 
    {
        /*
        ** load decryptor context from this test variable
        */

        ChkArg( ( iOption & 2 ) == 0 );
        ChkArg( argc > 2 && argv[2] != NULL );

        MAP_TO_GLOBAL_TEST_VAR( pLocalDecryptContext, argv[2] );        
    }
    

#if DRM_SUPPORT_PROFILING
    if( argc > 4 && argv[4] != NULL )
    {
        ui64MaxBindTime = DRM_UI64(atol( argv[4] ));
    }
    START_TIMER( ui64BindStartTime );
#endif
    
    ChkDR( DRM_Init_ProfileStack() );
    if( argc > 3 && argv[3] != NULL )
    {
        /*
        ** need to call TestManagerPrepareOPLCallback first
        */
        ChkDR( Drm_Reader_Bind( pLocalManagerContext,
                            (const DRM_CONST_STRING **)ppRightsArray,
                            *pdwNumRights,
                            OPLCallback, 
                            NULL,
                            pLocalDecryptContext ) );
    }
    else
    {
        ChkDR( Drm_Reader_Bind( pLocalManagerContext,
                            (const DRM_CONST_STRING **)ppRightsArray,
                            *pdwNumRights,
                            NULL, 
                            NULL,
                            pLocalDecryptContext ) );

    }

    ChkBOOL( *pfNeedsCallBack == FALSE, DRM_E_FAIL );

#if DRM_SUPPORT_PROFILING
        STOP_TIMER( ui64BindStartTime, ui64BindStartTime);
        Log( "Perf", "Drm_Reader_Bind took %d ms to execute.", DRM_UI64Low32(ui64BindStartTime) );
    
        if( !(DRM_UI64Eql(ui64MaxBindTime, DRM_UI64(0)))
         && DRM_UI64Les( ui64MaxBindTime, ui64BindStartTime ) )
        {
            ChkDR( DRM_E_TEST_TOO_SLOW );
        }
#endif

    /*
    ** Verify the LID bound to is the expected one
    */
    if ( argc > 5 && argv[5] != NULL )
    {        
        TST_UTL_mbstowcs( pwszVerifyLID, argv[5],  strlen( argv[5] ) + 1 );
        wszVerifyLID.pwszString = pwszVerifyLID;
        wszVerifyLID.cchString = ( DRM_DWORD )strlen( argv[5] );
        ChkDR( DRM_UTL_StringToGuid( &wszVerifyLID, ( DRM_GUID * )oVerifyLID.rgb ) );

        ChkBOOL( strncmp( ( const DRM_CHAR * )oVerifyLID.rgb, 
                          ( const DRM_CHAR * )&( ( ( DRM_APP_CONTEXT_INTERNAL * )pLocalManagerContext )->oFFLicense.rglid[0] ), 
                          DRM_ID_SIZE ) == 0, DRM_E_FAIL );
    }
    
    if (  iOption & 8 ) 
    {
        /*
        ** save decryptor context in this test variable
        */
        DRM_BYTE  *pbDecryptorVar  = NULL;        
        ChkArg( ( iOption & 2 ) == 0 );
        ChkArg( argc > 2 && argv[2] != NULL );

        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbDecryptorVar, argv[2], SIZEOF( DRM_DECRYPT_CONTEXT ) );
        MEMCPY(pbDecryptorVar, pLocalDecryptContext, SIZEOF( DRM_DECRYPT_CONTEXT ) );
    }

ErrorExit:
    if ( iOption&1 ) 
    { 
        SAFE_OEM_FREE( pLocalManagerContext );
    }

    if ( iOption&2 ) 
    { 
        SAFE_OEM_FREE( pLocalDecryptContext );
    }
    return dr;
}


/*****************************************************************************
** Function:    TestManagerInitDecrypt
**
** Synopsis:    
**
** Arguments:   argv[0] : DecryptContext : NULL or NORMAL
**              argv[1] : pbLast15 (offset from actual), or NULL
**              argv[2] : cbPayload (optional). If NULL or not specified, use g_cbData
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerInitDecrypt( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr                  = DRM_SUCCESS;
    DRM_DECRYPT_CONTEXT *pDecryptContext     = NULL;
    DRM_BYTE            *pbEncryptedData     = NULL;
    DRM_DWORD           *pcbData             = NULL;
    DRM_BYTE            *pbLast15            = NULL;
    DRM_CHAR            *pszTemp             = NULL;
    DRM_DWORD            cbPayload           = 0;
        
    /*
    ** 2 args are required
    */
    ChkArg( argc >= 2 );

    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedData, ENCRYPTED_DATA_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pcbData, DRMMGR_READER_DATA_SIZE_VAR );

    cbPayload = *pcbData;
       
    /*
    ** DecryptContext context: NULL or NORMAL
    */
    if ( argv[0] != NULL ) 
    { 
        MAP_TO_GLOBAL_TEST_VAR( pDecryptContext, DECRYPT_CONTEXT_VAR );
    }   

    if ( argv[1] != NULL )
    {
        if ( *pcbData > 15 )
        {                                   
            pbLast15 = pbEncryptedData + *pcbData - 15; 
        }           

        pbLast15 += atoi( argv[1] );
    }

    if ( argc == 3 && argv[2] != NULL )
    {
        cbPayload =  strtoul( argv[2], &pszTemp, 0 );            
        ChkArg( pszTemp != argv[2] );   
    }        

    ChkDR( Drm_Reader_InitDecrypt( pDecryptContext, pbLast15, cbPayload ) );       
    
 ErrorExit:        
    return dr;
}


/*****************************************************************************
** Function:    TestManagerDecrypt
**
** Synopsis:    Decrypts data stored in ENCRYPTED_DATA_VAR test variable 
**              and compares the result with value in DRMMGR_READER_DATA_VAR test variable
**              using decryptor in DECRYPT_CONTEXT_VAR
**
** Arguments:   argv[0] : DecryptContext : NULL or NORMAL
**              argv[1] : Data length to decrypt
**              argv[2] : pbData : NULL or NORMAL
**              argv[3] : Number of bytes for mis-alignment tests 
**              argv[4] : DRM_AES_COUNTER_MODE_CONTEXT: NULL or NORMAL
**
**              These have the form ###-### where the first set of digits is the high dword and the second set 
**              is the low dword. If the character after the first se is not '-' then the first set is treated
**              as the low DWORD and the high DWORD is 0.
**              argv[5] : Reset IV for counter-mode context (optional). If NULL, use default.
**              argv[6] : Reset Byte offset for counter-mode context (optional).
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerDecrypt( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                    dr              = DRM_SUCCESS;
    DRM_DECRYPT_CONTEXT          *pDecryptContext = NULL;
    DRM_DWORD                     cbDataToDecrypt = 0;
    DRM_BYTE                     *pbData          = NULL;
    DRM_DWORD                     iNumBytes       = 0;
    DRM_AES_COUNTER_MODE_CONTEXT *pCtrModeContext = NULL;
    DRM_AES_COUNTER_MODE_CONTEXT *pSharedAESCntxt = NULL;
    DRM_CHAR                     *pszTemp         = NULL;
    DRM_BYTE                     *pbEncryptedData = NULL;
    DRM_BYTE                     *pbSharedData    = NULL;
    
    /*
    ** 5 args are required
    */
    ChkArg( argc    >= 5 );
    ChkArg( argv[1] != NULL );    
    ChkArg( argv[3] != NULL ); 

    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedData, ENCRYPTED_DATA_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pSharedAESCntxt, AES_COUNTER_MODE_CONTEXT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pbSharedData, DRMMGR_READER_DATA_VAR );

    /*
    ** DecryptContext context: NULL or NORMAL
    */
    if ( argv[0] != NULL )
    { 
        MAP_TO_GLOBAL_TEST_VAR( pDecryptContext, DECRYPT_CONTEXT_VAR );
    }

    /*
    ** Data length to decrypt
    */
    cbDataToDecrypt = ( DRM_DWORD )strtoul( argv[1], &pszTemp, 0 );
    ChkArg( pszTemp != argv[1] );           

    /*
    ** Byte count for alignment test
    */    
    iNumBytes = ( DRM_DWORD )strtoul( argv[3], &pszTemp, 0 );
    ChkArg( pszTemp != argv[3] );      

    /*
    ** pbData: NULL or NORMAL
    */
    if ( argv[2] != NULL && iNumBytes == 0 )
    {
        pbData = pbEncryptedData;        
    }

    /*
    ** DRM_AES_COUNTER_MODE_CONTEXT: NULL or NORMAL
    */
    if ( argv[4] != NULL )
    {
        pCtrModeContext = pSharedAESCntxt;

        /*
        ** Counter-mode context specifics
        */
        if ( argc >= 6 && argv[5] != NULL )
        {
            ChkDR( TST_UTL_AtoUI64( argv[5], DRMCRT_strlen( argv[5] ), &pSharedAESCntxt->qwInitializationVector ) );
        }
        else
        {
            pSharedAESCntxt->qwInitializationVector = g_qwDefaultInitializationVector;
        }
        if ( argc >= 7 && argv[6] != NULL )
        {
            ChkDR( TST_UTL_AtoUI64( argv[6], DRMCRT_strlen( argv[6] ), &pSharedAESCntxt->qwBlockOffset ) );
        }        
        else
        {
            pSharedAESCntxt->qwBlockOffset = DRM_UI64( 0 );
        }   
        pSharedAESCntxt->bByteOffset = 0x00;
    }
    
    /*
    ** mis-alignment test
    */
    if( iNumBytes > 0 )
    {        
        DRM_BYTE *pbTempData = NULL;
        
        ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( MAXCIPHERDATA ) );
        ZEROMEM( pbData, MAXCIPHERDATA );
        DRM_BYT_CopyBytes( pbData, iNumBytes, pbEncryptedData, 0, cbDataToDecrypt );
        pbTempData = pbData + iNumBytes;        
       
        ChkDR( Drm_Reader_Decrypt( pDecryptContext,
                                   pCtrModeContext,
                                   pbTempData, 
                                   cbDataToDecrypt ) );
    
        ChkBOOL( MEMCMP( pbTempData, 
                         pbSharedData,                          
                         cbDataToDecrypt ) == 0,                 
                 DRM_E_TEST_DRMMANAGER_MISALIGNED_BYTES );        
    }
    else
    {        
        ChkDR( Drm_Reader_Decrypt( pDecryptContext,
                                   pCtrModeContext,
                                   pbData, 
                                   cbDataToDecrypt ) );    

        ChkBOOL( MEMCMP( pbData, pbSharedData, cbDataToDecrypt ) == 0,
                 DRM_E_TEST_DECRYPT_ERROR );
    }

 ErrorExit:
    if ( pbData != pbEncryptedData )
    {
        SAFE_OEM_FREE( pbData );    
    }
    return dr;
}

/*****************************************************************************
** Function:    TestManagerCommit
**
** Synopsis:    
**
** Arguments:   argv[0] : Max time (used in profiling builds)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerCommit( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT     *poManagerContext   = NULL;
#if DRM_SUPPORT_PROFILING
    DRM_UINT64 ui64StartTime = DRM_UI64LITERAL(0,0);
    DRM_UINT64 ui64MaxTime   = DRM_UI64LITERAL(0,0);
#endif

#if DRM_SUPPORT_PROFILING
    if( argc > 0 && argv[0] != NULL )
    {
        ui64MaxTime = DRM_UI64(atol( argv[0] ));
    }
    START_TIMER( ui64StartTime );
#endif

    GET_SHARED_APP_CONTEXT( poManagerContext );
    ChkDR( DRM_Init_ProfileStack() );
    ChkDR( Drm_Reader_Commit( poManagerContext, NULL, NULL ) );

#if DRM_SUPPORT_PROFILING
    STOP_TIMER( ui64StartTime, ui64StartTime);
    Log( "Perf", "Drm_Reader_Commit took %d ms to execute.", DRM_UI64Low32(ui64StartTime) );

    if( !DRM_UI64Eql(ui64MaxTime, DRM_UI64(0))
     && DRM_UI64Les( ui64MaxTime, ui64StartTime ) )
    {
        ChkDR( DRM_E_TEST_TOO_SLOW );
    }
#endif

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerSetRights
**
** Synopsis:    Add a right string to the global g_wszRights array.
**
** Arguments:   argv[0]: the right string
**              argv[1]: If TRUE will clear all previous rights from the array
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetRights( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT           dr             = DRM_SUCCESS;
    DRM_DWORD           *pdwNumRights   = NULL;
    DRM_CONST_STRING   **ppRightsArray  = NULL;
    DRM_BOOL             fClearExistingRights = FALSE;

    MAP_TO_GLOBAL_TEST_VAR( pdwNumRights, NUMBER_OF_RIGHTS_VAR );
    MAP_TO_GLOBAL_TEST_VAR( ppRightsArray, RIGHTS_ARRAY_VAR );

    ChkArg( argc >= 1 );
    ChkArg( argv[0] );

    if( argc > 1 && argv[1] != NULL )
    {
        ChkDR( StringToBool( argv[1], &fClearExistingRights ) );
    }

    if( fClearExistingRights )
    {
        DRM_DWORD i = 0;

        for(i = 0; i < *pdwNumRights; i++)
        {
            FREEDRMSTRINGPOINTER( ppRightsArray[i] );
            SAFE_OEM_FREE( ppRightsArray[i] );
        }
        *pdwNumRights = 0;
    }
    else
    {
        ChkArg( *pdwNumRights < MAX_RIGHTS );
    }

    ChkMem( ppRightsArray[*pdwNumRights] = (DRM_CONST_STRING*)Oem_MemAlloc( SIZEOF( DRM_CONST_STRING ) ) );
    ZEROMEM( ( DRM_BYTE * )ppRightsArray[*pdwNumRights], SIZEOF( DRM_CONST_STRING ) );
    ChkDR( MakeDRMString( ppRightsArray[*pdwNumRights], argv[0] ) );
    *pdwNumRights = *pdwNumRights + 1;
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    tDRM_KG_GenerateContentKey
**
** Synopsis:    
**
** Arguments:   pszKeySeed : 
**              pszKid :
**              pszKey :
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
static DRM_RESULT tDRM_KG_GenerateContentKey(DRM_CONST_STRING *pszKeySeed, DRM_CONST_STRING *pszKid, DRM_CONST_STRING *pszKey)
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_BYTE  *pbScratchKeySeed = NULL;
    DRM_DWORD  cchKey           = 0;

#pragma prefast(push)
#pragma prefast(disable:22022) /* ignore arithmetic overflow warning, already handled */
    ChkOverflow( pszKeySeed->cchString * SIZEOF(DRM_WCHAR), pszKeySeed->cchString );
    ChkMem( pbScratchKeySeed = (DRM_BYTE *)Oem_MemAlloc( pszKeySeed->cchString * SIZEOF(DRM_WCHAR) ) );
    ZEROMEM( ( DRM_BYTE * )pbScratchKeySeed, pszKeySeed->cchString * SIZEOF( DRM_WCHAR ) );
#pragma prefast(pop)

    dr = DRM_KG_GenerateContentKeyB64( (DRM_WCHAR*)pszKeySeed->pwszString, 
                                    pszKeySeed->cchString, 
                                    (DRM_WCHAR*)pszKid->pwszString, 
                                    pszKid->cchString, 
                                    eDRM_RC4_CIPHER,
                                    DRMCIPHERKEYLEN_RC4,
                                    NULL, 
                                    &pszKey->cchString,
                                    pbScratchKeySeed,
                                    pszKeySeed->cchString * SIZEOF(DRM_WCHAR) );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        cchKey = pszKey->cchString;
        FREEDRMSTRINGPOINTER( pszKey );
        pszKey->cchString = cchKey;
    }

#pragma prefast(push)
#pragma prefast(disable:22011) /* ignore arithmetic overflow warning, already handled */
    ChkOverflow( pszKey->cchString * SIZEOF(DRM_WCHAR), pszKey->cchString );
    ChkMem( pszKey->pwszString = (DRM_WCHAR*)Oem_MemAlloc( pszKey->cchString * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( ( DRM_BYTE * )pszKey->pwszString, pszKey->cchString * SIZEOF( DRM_WCHAR ) );
#pragma prefast(pop)
    ChkDR( DRM_KG_GenerateContentKeyB64( (DRM_WCHAR*)pszKeySeed->pwszString, 
                                      pszKeySeed->cchString, 
                                      (DRM_WCHAR*)pszKid->pwszString, 
                                      pszKid->cchString, 
                                      eDRM_RC4_CIPHER,
                                      DRMCIPHERKEYLEN_RC4,
                                      (DRM_WCHAR*)pszKey->pwszString, 
                                      &pszKey->cchString,
                                      pbScratchKeySeed,
                                      pszKeySeed->cchString * SIZEOF(DRM_WCHAR) ) );
ErrorExit:
    SAFE_OEM_FREE( pbScratchKeySeed );

    return dr;
}

/*****************************************************************************
** 
** Function: TestManagerCheckDecryptContext
** 
** Synopsis: This verifies that the appropriate values are
** included in the decrypt context returned by Drm_Reader_Bind.
** This must be called after calling TestManagerBind using
** the global decrypt context option.
** 
** The values are tested by encrypting data using the expected 
** content key and then decrypting the data using the decrypt
** context returned by Drm_Reader_Bind.  If the decrypted content
** matches the original plain text then the context is valid.
**
** Arguments:
**   argc    -- must be 2
**   argv[0] -- The KID of the license previously bound to
**   argv[1] -- The key seed of the license previously bound to
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerCheckDecryptContext( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT         dr                    = DRM_SUCCESS;       /* Return value of test function*/
    DRM_CIPHER_CONTEXT *pcontextDCRY         = NULL;             /* The decrypt context previously output by Drm_Reader_Bind */
    DRM_DWORD          cbExpectedContentKey  = 0;                /* Size, in bytes, of the expected content key */
    DRM_BYTE           *pbExpectedContentKey = NULL;             /* Holds the expected content key */
    DRM_CONST_STRING   dstrContentKey        = EMPTY_DRM_STRING; /* Holds the expected content key */
    DRM_CONST_STRING   dstrKID               = EMPTY_DRM_STRING; /* Holds the KID */
    DRM_CONST_STRING   dstrKeySeed           = EMPTY_DRM_STRING;                 /* Holds the keyseed */
    DRM_CIPHER_CONTEXT cipherContext         = { (DRM_SUPPORTED_CIPHERS) 0, 0 }; /* local cipher context for encrypting the plain text */
    DRM_BYTE           rgbPlainText[DRM_CPHR_CB_FOR_INITDECRYPT * 3]  = { 0 };   /* Holds the plain text that will be encrypted */
    DRM_BYTE           rgbCipherText[DRM_CPHR_CB_FOR_INITDECRYPT * 3] = { 0 };   /* Holds the result of decrypting the encrypted plain text */
    DRM_AES_COUNTER_MODE_CONTEXT *pSharedAESCntxt                     = NULL;    /* Holds the shared AES context */

    ChkArg(argc == 2);

    /* 
    ** Get the cipher context previously output by Drm_Reader_Bind 
    */
    MAP_TO_GLOBAL_TEST_VAR( pcontextDCRY, DECRYPT_CONTEXT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pSharedAESCntxt, AES_COUNTER_MODE_CONTEXT_VAR );

    /*
    ** If the cipher type is INVALID then we cannot encrypt/decrypt data so return
    ** a failure code.
    */
    ChkBOOL( pcontextDCRY->eCipherType != eDRM_CIPHER_INVALID, DRM_E_CIPHER_NOTINITIALIZED );

    /* 
    ** Parse the input KID 
    */
    ChkDR( MakeDRMString( &dstrKID, argv[0] ) );

    /* 
    ** Parse the input key seed 
    */
    ChkDR( MakeDRMString( &dstrKeySeed, argv[1] ) );

    /* 
    ** Get the expected content key and decode it 
    */
    ChkDR( tDRM_KG_GenerateContentKey( &dstrKeySeed, &dstrKID, &dstrContentKey ) );
    ChkDR( tDRM_B64_Decode( &dstrContentKey, &cbExpectedContentKey, &pbExpectedContentKey ) );

    /*
    ** Create some content to encrypt then decrypt
    */
    ChkDR( Oem_Random_GetBytes(NULL, rgbPlainText, SIZEOF(rgbPlainText)) );

    ChkBOOL ( SIZEOF(rgbCipherText) >= SIZEOF(rgbPlainText), DRM_E_BUFFERTOOSMALL );
    memcpy(rgbCipherText, rgbPlainText, SIZEOF(rgbPlainText));

    /*
    ** Initialize encryption context using the expected content key.
    */
    ChkDR( DRM_CPHR_Init( &cipherContext, 
                          pcontextDCRY->eCipherType,
                          cbExpectedContentKey,
                          pbExpectedContentKey) );

    /*
    ** Encrypt.
    */
    ChkDR( DRM_CPHR_Encrypt(&cipherContext, pSharedAESCntxt, sizeof(rgbCipherText), rgbCipherText) );

    /*
    ** Initialize decryption context using the content key output by Drm_Reader_Bind
    */
    ChkBOOL( SIZEOF( rgbCipherText ) >= DRM_CPHR_CB_FOR_INITDECRYPT, DRM_E_BUFFERTOOSMALL );
    ChkDR( DRM_BBX_CPHR_InitDecrypt( pcontextDCRY, 
                                     rgbCipherText + SIZEOF(rgbCipherText) - DRM_CPHR_CB_FOR_INITDECRYPT, 
                                     SIZEOF(rgbCipherText) ) );

    /*
    ** Decrypt.
    */
    ChkDR( DRM_BBX_CPHR_Decrypt( pcontextDCRY, pSharedAESCntxt, SIZEOF(rgbCipherText), rgbCipherText ) );

    /*
    ** Are the decrypted bytes the same as the plain text?
    */
    ChkBOOL (0 ==  memcmp(rgbCipherText, rgbPlainText, SIZEOF(rgbPlainText) ), DRM_E_CH_BAD_KEY )

ErrorExit:
    /* 
    ** free memory allocated for pbExpectedContentKey, dstrKID, dstrKeySeed, and dstrContentKey 
    */
    FREEDRMSTRING( dstrKID );
    FREEDRMSTRING( dstrKeySeed );
    FREEDRMSTRING( dstrContentKey );
    SAFE_SECURE_OEM_FREE( pbExpectedContentKey, cbExpectedContentKey );

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
