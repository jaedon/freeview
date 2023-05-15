/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>

#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmxmlparser.h>
#include <drmhdrbuilder.h>
#include <drmkeygenerator.h>
#include <drmconstants.h>

#include <drmtoolsconstants.h>
#include <drmtoolsmacros.h>
#include <drmtoolsutils.h>
#include <drmtoolsmediafile.h>
#include <drmtoolspkg.h>

ENTER_PK_NAMESPACE_CODE;

#define CallHeaderBuilder(context, fn)   {\
    while (TRUE)\
    {\
        dr = (fn);\
        if ( dr == DRM_E_BUFFERTOOSMALL )\
        {\
            ChkDR(_ReallocHBContext(&(context)));\
            continue;\
        }\
        ChkDR(dr);\
        break;\
    }\
}

/******************************************************************************
**
** Function:   _ReallocHBContext
**
** Synopsis:   Reallocated header context
**
** Arguments:
** [ppbHBContext]  --  reference to header context
**
******************************************************************************/
static DRM_RESULT _ReallocHBContext( DRM_BYTE **ppbHBContext )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  cbContext    = 0;
    DRM_BYTE  *pbNewContext = NULL;

    ChkDR(DRM_HB_GetContextSize(*ppbHBContext, &cbContext));  /* get current size */
    ChkMem(pbNewContext = (DRM_BYTE*)Oem_MemAlloc(2 * cbContext));  /* alloc double of the current size */
    ChkDR(DRM_HB_ReallocHeader(*ppbHBContext,    2 * cbContext, pbNewContext));
    Oem_MemFree(*ppbHBContext);
    *ppbHBContext = pbNewContext;

ErrorExit:
    return dr;
}

static DRM_RESULT _GetNode(
    __in  const DRM_CONST_STRING *f_pdstrXML,
    __in  const DRM_CONST_STRING *f_pdstrTag,
    __in  const DRM_BOOL          f_fOptional,
    __out DRM_CONST_STRING       *f_pdstrNodeData )
{
   DRM_RESULT dr = DRM_SUCCESS;

   ChkArg( f_pdstrXML != NULL );
   ChkArg( f_pdstrTag != NULL );
   ChkArg( f_pdstrNodeData != NULL );

   dr = DRM_XML_GetSubNode( f_pdstrXML, f_pdstrTag,   NULL, NULL, 0, NULL, f_pdstrNodeData, 0 );
   if( f_fOptional && dr == DRM_E_XMLNOTFOUND )
   {
       dr = DRM_S_FALSE;
   }

ErrorExit:
   return dr;
}

/******************************************************************************
**
** Function:   _GenerateKeyID
**
** Synopsis:   Creates key id
**
** Arguments:
**
** [f_pdstrKeyID] -- Pointer to buffer for key id. Cannot  be NULL
**                   In case of error the value is undefined
**
** Returns:       DRM_SUCCESS if successful
**                DRM_E_FAIL - if key generation failed
**
 * Notes:         Caller must free f_pdstrKeyID->pwszString using Oem_MemFree
**                after use
**
******************************************************************************/
static DRM_RESULT _GenerateKeyID ( __out DRM_STRING *f_pdstrKeyID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pdstrKeyID != NULL );

    /* Get Key ID buffer size */
    dr = DRM_KG_GenerateKeyID( NULL, NULL, &(f_pdstrKeyID->cchString) );
    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }
    ChkBOOL( f_pdstrKeyID->cchString!=0, DRM_E_FAIL );

    /* Allocate buffer for key ID */
    ChkOverflow( f_pdstrKeyID->cchString * SIZEOF(DRM_WCHAR), f_pdstrKeyID->cchString );
    f_pdstrKeyID->pwszString = ( DRM_WCHAR * ) Oem_MemAlloc( f_pdstrKeyID->cchString * SIZEOF(DRM_WCHAR) );
    ChkMem( f_pdstrKeyID->pwszString );

    /* Generate key ID */
    ChkDR( DRM_KG_GenerateKeyID( NULL, f_pdstrKeyID->pwszString, &(f_pdstrKeyID->cchString) ) );

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        /* if key generation is failed - release allocated memory */
        if ( f_pdstrKeyID != NULL && f_pdstrKeyID->pwszString != NULL )
        {
            Oem_MemFree( f_pdstrKeyID->pwszString );
            f_pdstrKeyID->pwszString = NULL;
            f_pdstrKeyID->cchString = 0;
        }
    }
    return dr;
}

/******************************************************************************
**
** Function:   _GenerateContentKey
**
** Synopsis:   Creates content key
**
** Arguments:
**
** [f_pdstrContentKey] -- pointer to buffer for content key
**
** Returns:         -- DRM_SUCCESS if successful
**
******************************************************************************/
static DRM_RESULT _GenerateContentKey(
    __in  const DRM_CONST_STRING *f_pdstrKeySeed,
    __in  const DRM_CONST_STRING *f_pdstrKID,
    __in  DRM_SUPPORTED_CIPHERS   eCipherType,
    __out DRM_STRING             *f_pdstrContentKey )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  cbContentKey = 0;
    DRM_BYTE  *pbScratchKeySeed = NULL;

    ChkArg( f_pdstrKeySeed != NULL );
    ChkArg( f_pdstrKID != NULL );
    ChkArg( f_pdstrContentKey != NULL );

    if( eCipherType == eDRM_AES_COUNTER_CIPHER )
    {
        cbContentKey = DRM_AES_KEYSIZE_128;
    }
    else if( eCipherType == eDRM_RC4_CIPHER )
    {
        cbContentKey = DRMCIPHERKEYLEN_RC4;
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

    /* generate the content key */
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_ALLOC_SIZE_OVERFLOW, "ChkOverflow checks for arithmetic but Prefast still warns.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_ALLOC_SIZE_OVERFLOW_WITH_ACCESS, "ChkOverflow checks for arithmetic but Prefast still warns.")
    ChkOverflow( f_pdstrKeySeed->cchString * (SIZEOF(DRM_WCHAR)*1), f_pdstrKeySeed->cchString );
    ChkMem( pbScratchKeySeed = (DRM_BYTE *)Oem_MemAlloc( f_pdstrKeySeed->cchString * SIZEOF(DRM_WCHAR) ) );
    ZEROMEM( pbScratchKeySeed, f_pdstrKeySeed->cchString * SIZEOF(DRM_WCHAR) );
PREFAST_POP
PREFAST_POP
    dr = DRM_KG_GenerateContentKeyB64( f_pdstrKeySeed->pwszString, f_pdstrKeySeed->cchString,
                                       f_pdstrKID->pwszString, f_pdstrKID->cchString,
                                       eCipherType, cbContentKey,
                                       NULL, &f_pdstrContentKey->cchString,
                                       pbScratchKeySeed, f_pdstrKeySeed->cchString * SIZEOF(DRM_WCHAR));

    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );
    ChkBOOL( f_pdstrContentKey->cchString!=0, DRM_E_FAIL );

    ChkOverflow( f_pdstrContentKey->cchString * SIZEOF(DRM_WCHAR), f_pdstrContentKey->cchString );
    ChkMem( f_pdstrContentKey->pwszString = (DRM_WCHAR*)Oem_MemAlloc( f_pdstrContentKey->cchString * SIZEOF(DRM_WCHAR) ) );

    ChkDR( DRM_KG_GenerateContentKeyB64( f_pdstrKeySeed->pwszString, f_pdstrKeySeed->cchString,
                                         f_pdstrKID->pwszString, f_pdstrKID->cchString,
                                         eCipherType, cbContentKey,
                                         f_pdstrContentKey->pwszString, &f_pdstrContentKey->cchString,
                                         pbScratchKeySeed, f_pdstrKeySeed->cchString * SIZEOF(DRM_WCHAR)));

ErrorExit:
    SAFE_OEM_FREE( pbScratchKeySeed );
    return dr;
}

/*********************************************************************
**
** Function: DRMTOOLS_BuildContentHeader_Ex
**
** Synopsis: API that builds a a RM header using the passed in parameters.
**           Can't be used for v.4.1 because DRM_HB_...() APIs
**           do not have support for it!
**
** Arguments:
**
** [f_poCrypto]             -- Pointer to a crypto context (optional).
** [f_pdstrSigningKey]      -- Pointer to a B64 encoded string of signing key (optional).
** [f_pdstrContentID]       -- Pointer to a B64 encoded string of content ID (optional).
** [f_pdstrKeySeed]         -- Pointer to a B64 encoded string of key seed.
** [f_pdstrKeyID]           -- Pointer to a B64 encoded string of key ID.
** [f_pdstrLAURL]           -- Pointer to a B64 encoded string of silent license acquisition URL (optional).
** [f_pdstrLUIURL]          -- Pointer to a B64 encoded string of non-silent license acquisition URL (optional).
** [f_pdstrDSID]            -- Pointer to a B64 encoded string of service ID (optional).
** [f_pdstrCustomData]      -- Pointer to a B64 encoded string of custom data (optional).
** [f_pdstrDecryptorSetup]  -- Pointer to a B64 encoded string of DECRYPTORSETUP (optional).
**                             May only be used with v4.1 headers.
** [f_eHeaderType]          -- Type of RM header to be generated.
** [f_eCipherType]          -- Type of cipher (Cocktail-RC4 or AES CTR).
** [f_ppbHeader]            -- Pointer to a pointer of buffer that contains the returned header.
**                             It is caller's responsibility to free the buffer.
** [f_pcbHeader]            -- Pointer to a variable to receive the size of the header returned.
** [f_pdstrContentKey]      -- Pointer to a string to receive the generated content key.
**                             It is caller's responsibility to free the buffer of the content key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_BuildContentHeader_Ex(
    __in_opt       DRM_CRYPTO_CONTEXT      *f_poCrypto,
    __in_opt const DRM_CONST_STRING        *f_pdstrSigningKey,
    __in_opt const DRM_CONST_STRING        *f_pdstrContentID,
    __in     const DRM_CONST_STRING        *f_pdstrKeySeed,
    __in     const DRM_CONST_STRING        *f_pdstrKeyID,
    __in_opt const DRM_CONST_STRING        *f_pdstrLAURL,
    __in_opt const DRM_CONST_STRING        *f_pdstrLUIURL,
    __in_opt const DRM_CONST_STRING        *f_pdstrDSID,
    __in_opt const DRM_CONST_STRING        *f_pdstrCustomData,
    __in_opt const DRM_CONST_STRING        *f_pdstrDecryptorSetup,
    __in           DRM_CONTENT_SET_PROPERTY f_eHeaderType,
    __in           DRM_SUPPORTED_CIPHERS    f_eCipherType,
    __deref_inout_bcount_opt( *f_pcbHeader )
                   DRM_BYTE               **f_ppbHeader,
    __inout        DRM_DWORD               *f_pcbHeader,
    __inout        DRM_STRING              *f_pdstrContentKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbContext = NULL;
    DRM_BYTE *pbHeaderXML = NULL;
    DRM_DWORD cbHeaderXML = 0;
    DRM_DWORD cbContext = 0;

    ChkDRMString( f_pdstrKeySeed );
    ChkDRMString( f_pdstrKeyID );
    ChkArg( f_ppbHeader != NULL );
    ChkArg( f_pcbHeader != NULL );
    ChkArg( f_pdstrContentKey != NULL );

    /*
    ** Verify allowed header formats.
    */
    ChkArg( f_eHeaderType == DRM_CSP_V2_HEADER
         || f_eHeaderType == DRM_CSP_V2_4_HEADER
         || f_eHeaderType == DRM_CSP_V4_HEADER );

    /* Verify allowed ciphers. */
    ChkArg( f_eCipherType == eDRM_RC4_CIPHER
         || f_eCipherType == eDRM_AES_COUNTER_CIPHER );

    /* Use DrmHdrBuilder to build the header. */
    dr = DRM_HB_CreateHeader( NULL, f_eHeaderType, &cbContext);
    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }

    ChkMem( pbContext = ( DRM_BYTE * )Oem_MemAlloc( cbContext ) );
    ChkDR( DRM_HB_CreateHeader( pbContext, f_eHeaderType, &cbContext ) );

    ChkDR( DRM_HB_OpenNode( pbContext, &g_dstrTagProtectInfo ) );

    if ( f_eCipherType == eDRM_RC4_CIPHER )
    {
        CallHeaderBuilder( pbContext, DRM_HB_SetAttribute( pbContext, &g_dstrTagKeyLen, &g_dstrKeyLenNodeDataCocktail ) );
        CallHeaderBuilder( pbContext, DRM_HB_SetAttribute( pbContext, &g_dstrTagAlgID,  &g_dstrTagCOCKTAIL ) );
    }
    else if ( f_eCipherType == eDRM_AES_COUNTER_CIPHER )
    {
        CallHeaderBuilder( pbContext, DRM_HB_SetAttribute( pbContext, &g_dstrTagKeyLen, &g_dstrKeyLenNodeDataAESCTR ) );
        CallHeaderBuilder( pbContext, DRM_HB_SetAttribute( pbContext, &g_dstrTagAlgID,  &g_dstrTagAESCTR ) );
    }

    ChkDR( DRM_HB_CloseCurrNode( pbContext ) );

    if ( f_eHeaderType == DRM_CSP_V2_HEADER || f_eHeaderType == DRM_CSP_V2_4_HEADER )
    {
        CallHeaderBuilder( pbContext, DRM_HB_SetLicAcqUrl( pbContext, f_pdstrLAURL ) );
    }

    /* V4, v4.1 or V2.4 attributes. */
    if (    f_eHeaderType == DRM_CSP_V4_HEADER
         || f_eHeaderType == DRM_CSP_V4_1_HEADER
         || f_eHeaderType == DRM_CSP_V2_4_HEADER )
    {
        if ( f_eHeaderType == DRM_CSP_V2_4_HEADER )
        {
            ChkDR( DRM_HB_OpenNode( pbContext, &g_dstrTagV4DATA ) );
        }

        if ( f_pdstrLAURL != NULL && f_pdstrLAURL->cchString != 0 && f_pdstrLAURL->pwszString != NULL )
        {
            CallHeaderBuilder( pbContext, DRM_HB_SetAttribute( pbContext, &g_dstrTagLAURL, f_pdstrLAURL ) );
        }

        if ( f_pdstrLUIURL != NULL && f_pdstrLUIURL->cchString != 0 && f_pdstrLUIURL->pwszString != NULL )
        {
            CallHeaderBuilder( pbContext, DRM_HB_SetAttribute( pbContext, &g_dstrTagLUIURL, f_pdstrLUIURL ) );
        }

        if ( f_pdstrDSID != NULL && f_pdstrDSID->cchString != 0 && f_pdstrDSID->pwszString != NULL )
        {
            CallHeaderBuilder( pbContext, DRM_HB_SetAttribute(pbContext, &g_dstrTagDSID, f_pdstrDSID ) );
        }

        if ( f_eHeaderType == DRM_CSP_V2_4_HEADER )
        {
            ChkDR( DRM_HB_CloseCurrNode( pbContext ) );
        }
    }

    /* DECRYPTORSETUP only exists in v4.1 headers */
    if ( f_eHeaderType == DRM_CSP_V4_1_HEADER )
    {
        if ( f_pdstrDecryptorSetup != NULL && f_pdstrDecryptorSetup->cchString != 0 && f_pdstrDecryptorSetup->pwszString != NULL )
        {
            CallHeaderBuilder( pbContext, DRM_HB_SetAttribute(pbContext, &g_dstrTagDecryptorSetup, f_pdstrDecryptorSetup ) );
        }
    }

    if ( f_pdstrCustomData != NULL && f_pdstrCustomData->cchString != 0 && f_pdstrCustomData->pwszString != NULL )
    {
        CallHeaderBuilder( pbContext, DRM_HB_AddData( pbContext, f_pdstrCustomData ) );
    }

    CallHeaderBuilder( pbContext, DRM_HB_SetKeyID( pbContext, f_pdstrKeyID ) );

    /* Optional attributes */
    if ( f_pdstrContentID != NULL && f_pdstrContentID->cchString != 0 && f_pdstrContentID->pwszString != NULL )
    {
        CallHeaderBuilder( pbContext, DRM_HB_SetAttribute( pbContext, &g_dstrCID, f_pdstrContentID ) );
    }

    /* Generate the content key. */
    ChkDR( _GenerateContentKey( f_pdstrKeySeed,
                                f_pdstrKeyID,
                                f_eCipherType,
                                f_pdstrContentKey ) );

    CallHeaderBuilder( pbContext, DRM_HB_SetChecksum( pbContext, ( DRM_CONST_STRING * )f_pdstrContentKey ) );

    CallHeaderBuilder( pbContext, DRM_HB_CloseHeader( pbContext,
                                                      f_poCrypto,
                                                      ( DRM_CONST_STRING * )f_pdstrSigningKey,
                                                      &pbHeaderXML,
                                                      &cbHeaderXML ) );

    /* Allocate buffer for return. */
    ChkMem( *f_ppbHeader = ( DRM_BYTE * )Oem_MemAlloc( cbHeaderXML ) );
    MEMCPY( *f_ppbHeader, pbHeaderXML, cbHeaderXML );
    *f_pcbHeader = cbHeaderXML;

ErrorExit:

    if ( pbContext != NULL )
    {
        Oem_MemFree( pbContext );
    }

    return dr;
}

/*********************************************************************
**
** Function: DRMTOOLS_BuildContentHeader
**
** Synopsis: API that builds a a RM header using the passed in parameters.
**
** Arguments:
**
** [f_poCrypto]             -- Pointer to a crypto context.
** [f_pdstrFileInit]        -- Pointer to a initialization XML file.
** [f_pdstrContentID]       -- Pointer to a B64 encoded string of content ID (optional).
** [f_pdstrKeyID]           -- Pointer to a B64 encoded string of key ID.
** [f_eHeaderType]          -- Type of RM header to be generated.
** [f_eCipherType]          -- Type of cipher (Cocktail-RC4 or AES CTR).
** [f_ppbHeader]            -- Pointer to a pointer of buffer that contains the returned header.
**                             It is caller's responsibility to free the buffer.
** [f_pcbHeader]            -- Pointer to a variable to receive the size of the header returned.
** [f_pdstrContentKey]      -- Pointer to a string to receive the generated content key.
**                             It is caller's responsibility to free the buffer of the content key.
** [f_pcbChunk]             -- Pointer to a variable to receive the block size.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_BuildContentHeader(
    __in       DRM_CRYPTO_CONTEXT      *f_poCrypto,
    __in const DRM_CONST_STRING        *f_pdstrFileInit,
    __in const DRM_CONST_STRING        *f_pdstrContentID,
    __inout    DRM_CONST_STRING        *f_pdstrKeyID,
    __in       DRM_CONTENT_SET_PROPERTY f_eHeaderType,
    __in       DRM_SUPPORTED_CIPHERS    f_eCipherType,
    __deref_inout_bcount_opt( *f_pcbHeader )
               DRM_BYTE               **f_ppbHeader,
    __inout    DRM_DWORD               *f_pcbHeader,
    __inout    DRM_STRING              *f_pdstrContentKey,
    __out      DRM_DWORD               *f_pcbChunk )
{
    DRM_RESULT  dr                      = DRM_SUCCESS;
    DRM_BOOL    bParseError             = FALSE;
    DRM_STRING  strPkgInit              = EMPTY_DRM_STRING;
    DRM_STRING  strKeySeed              = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPkgInitContent = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDecryptorSetup = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSigningKey     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrLAURL          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrLUIURL         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDSID           = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrCustomData     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrEncBlkSize     = EMPTY_DRM_STRING;

    /* Verify input parameters. */
    ChkArg( f_poCrypto != NULL );
    ChkDRMString( f_pdstrFileInit);
    ChkArg( f_pdstrContentID != NULL );
    ChkArg( f_pdstrKeyID != NULL );
    ChkArg( f_ppbHeader != NULL );
    ChkArg( f_pcbHeader != NULL );
    ChkArg( f_pdstrContentKey != NULL );
    ChkArg( f_pcbChunk != NULL );

    /* Verify allowed header formats. */
    ChkArg( f_eHeaderType == DRM_CSP_V2_HEADER
         || f_eHeaderType == DRM_CSP_V2_4_HEADER
         || f_eHeaderType == DRM_CSP_V4_HEADER
         || f_eHeaderType == DRM_CSP_V4_1_HEADER );

    /* Verify allowed ciphers. */
    ChkArg( f_eCipherType == eDRM_RC4_CIPHER
         || f_eCipherType == eDRM_AES_COUNTER_CIPHER );

    /* Read init file. */
    ChkDR( DRMTOOLS_ReadFileData( f_pdstrFileInit, &strPkgInit ) );

    bParseError = TRUE;

    ChkDR( _GetNode( ( DRM_CONST_STRING * )&strPkgInit,
                     &g_dstrTagRoot,
                     FALSE,
                     &dstrPkgInitContent ) );

    ChkDR( _GetNode( &dstrPkgInitContent,
                     &g_dstrTagKeySeed,
                     FALSE,
                     ( DRM_CONST_STRING * )&strKeySeed ) );

    ChkDR( _GetNode( &dstrPkgInitContent,
                     &g_dstrSigningKey,
                     TRUE,
                     &dstrSigningKey ) );

    ChkDR( _GetNode( &dstrPkgInitContent,
                     &g_dstrTagCustomData,
                     TRUE,
                     &dstrCustomData ) );

    ChkDR( _GetNode( &dstrPkgInitContent,
                     &g_dstrEncBlkSize,
                     TRUE,
                     &dstrEncBlkSize ) );

    /* License acquisition url PD style is not used in V4 headers. */
    if ( f_eHeaderType != DRM_CSP_V4_HEADER && f_eHeaderType != DRM_CSP_V4_1_HEADER )
    {
        ChkDR( _GetNode( &dstrPkgInitContent,
                         &g_dstrTagLAINFO,
                         FALSE,
                         &dstrLAURL ) );
    }

    if (    f_eHeaderType == DRM_CSP_V4_HEADER
         || f_eHeaderType == DRM_CSP_V4_1_HEADER
         || f_eHeaderType == DRM_CSP_V2_4_HEADER )
    {
       ChkDR( _GetNode( &dstrPkgInitContent,
                        &g_dstrTagLAURL,
                        TRUE,
                        &dstrLAURL ) );

       ChkDR( _GetNode( &dstrPkgInitContent,
                        &g_dstrTagLUIURL,
                        TRUE,
                        &dstrLUIURL ) );

       ChkDR( _GetNode( &dstrPkgInitContent,
                        &g_dstrTagDSID,
                        TRUE,
                        &dstrDSID ) );
    }

    /* DECRYPTORSETUP only exists in v4.1 headers, it is optional */
    if ( f_eHeaderType == DRM_CSP_V4_1_HEADER )
    {
        ChkDR( _GetNode( &dstrPkgInitContent,
                         &g_dstrTagDecryptorSetup,
                          TRUE, /* optional */
                         &dstrDecryptorSetup ) );
    }

    if ( dstrEncBlkSize.cchString != 0 && dstrEncBlkSize.pwszString != NULL )
    {
        ChkDR( DRMCRT_wcsntol( dstrEncBlkSize.pwszString, dstrEncBlkSize.cchString, ( DRM_LONG * )f_pcbChunk ) );
    }

    /* Generate KeyID if not supplied. */
    if ( f_pdstrKeyID->cchString == 0 || f_pdstrKeyID->pwszString == NULL )
    {
        ChkDR( _GenerateKeyID( ( DRM_STRING * )f_pdstrKeyID ) );
    }

    bParseError = FALSE;

    ChkDR( DRMTOOLS_BuildContentHeader_Ex(   f_poCrypto,
                                            &dstrSigningKey,
                                             f_pdstrContentID,
                                           ( const DRM_CONST_STRING* )&strKeySeed,
                                             f_pdstrKeyID,
                                            &dstrLAURL,
                                            &dstrLUIURL,
                                            &dstrDSID,
                                            &dstrCustomData,
                                            &dstrDecryptorSetup,
                                             f_eHeaderType,
                                             f_eCipherType,
                                             f_ppbHeader,
                                             f_pcbHeader,
                                             f_pdstrContentKey ) );

ErrorExit:

    if ( bParseError )
    {
        TRACE (("Error: Failed in parsing package init XML\n"));
    }

    SAFE_OEM_FREE( strPkgInit.pwszString );

    return dr;
}

/******************************************************************************
**
** Function:   DRMTOOLS_WrapV4XMLIntoPlayReadyObject
**
** Synopsis:   Wraps V4 XML header into PlayReady object
**             Allocated new buffer for PlayReady object
**             and releases memory used by V4 XML header
**
** Arguments:
**
** [f_pbHeader]       -- Input: pointer to V4 XML header (UTF-16),
**                       Output: pointer to PlayReady object header
** [f_cbHeader]       -- Input: size of V4 XML header,
**                       Output: size of PlayReady object header (including V4 XML header)
**
** Returns:       DRM_SUCCESS       if successful
**                DRM_E_INVALIDARG  if input parameters is invalid
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_WrapV4XMLIntoPlayReadyObject(
    __inout_ecount(*pcbHeader) DRM_BYTE  **ppbHeader,
    __inout                    DRM_DWORD  *pcbHeader )
{
    DRM_RESULT           dr                = DRM_SUCCESS;
    DRM_BYTE            *pbPlayReadyHeader = NULL;
    DRM_DWORD            cbPlayReadyHeader = 0;
    DRM_CONST_STRING     dstrV4XML         = EMPTY_DRM_STRING;

    ChkArg( ppbHeader != NULL );
    ChkArg( pcbHeader != NULL );

    cbPlayReadyHeader = *pcbHeader +
                        SIZE_OF_DRM_PLAYREADY_DATA_HEADER +
                        SIZE_OF_DRM_PLAYREADY_RECORD_HEADER;

    cbPlayReadyHeader += SIZE_OF_DRM_PLAYREADY_RECORD_HEADER +
                             MAX_EMBEDDED_STORE_LEN;

    dstrV4XML.pwszString = (const DRM_WCHAR *)*ppbHeader;
    dstrV4XML.cchString  = *pcbHeader / SIZEOF( DRM_WCHAR );

    ChkOverflow( cbPlayReadyHeader, *pcbHeader );

    ChkMem( pbPlayReadyHeader = (DRM_BYTE*) Oem_MemAlloc( cbPlayReadyHeader ) );
    ZEROMEM( pbPlayReadyHeader, cbPlayReadyHeader );

    ChkDR( DRM_PRO_Create( &dstrV4XML, pbPlayReadyHeader, &cbPlayReadyHeader, MAX_EMBEDDED_STORE_LEN ) );

    /* Release Copy V4 XML header */
    SAFE_OEM_FREE( *ppbHeader );

    /* Assign new pointers */
    *ppbHeader = pbPlayReadyHeader;
    *pcbHeader = cbPlayReadyHeader;

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
