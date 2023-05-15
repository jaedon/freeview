/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcertparsercache.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
** Function: DRM_CERTPARSERCACHE_PackCertParserEntry
**
** Synopsis: Function that packs a cert parser entry into a packed
**           word list buffer.
**
** Arguments:
**
** [f_iAttrib]              -- Index of cert parser entry to be packed.
** [f_pbPacked]             -- Pointer to a buffer to store the packed
**                             cert parser entries.
** [f_cbPacked]             -- Size of the buffer to store the packed
**                             cert parser entries.
** [f_poEntry]              -- Pointer to a cert parser entry.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: The words stored in the packed word list buffer is already in the
**       format of machine byte order.
**       Since it is an internal function, parameter check is relaxed for
**       performance reasons.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_CERTPARSERCACHE_PackCertParserEntry(
    __in  DRM_WORD                            f_iAttrib,
    __in_bcount( f_cbPacked )
          DRM_BYTE                           *f_pbPacked,
    __in  DRM_DWORD                           f_cbPacked,
    __out DRM_DEVICE_CERT_PARSER_CACHE_ENTRY *f_poEntry )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbEntry = f_pbPacked + DRM_CB_CERT_PARSER_ENTRY * f_iAttrib;

    ChkArg( f_cbPacked == DRM_CB_CERT_PARSER_ENTRIES &&
            f_iAttrib < DRM_DEVCERT_MAXATTRIBUTES );

    MEMCPY( pbEntry, &f_poEntry->m_fValid, SIZEOF( DRM_WORD ) );
    pbEntry += SIZEOF( DRM_WORD );

    MEMCPY( pbEntry, &f_poEntry->m_iNodeStart, SIZEOF( DRM_WORD ) );
    pbEntry += SIZEOF( DRM_WORD );

    MEMCPY( pbEntry, &f_poEntry->m_iNodeLength, SIZEOF( DRM_WORD ) );
    pbEntry += SIZEOF( DRM_WORD );

    MEMCPY( pbEntry, &f_poEntry->m_iValueStart, SIZEOF( DRM_WORD ) );
    pbEntry += SIZEOF( DRM_WORD );

    MEMCPY( pbEntry, &f_poEntry->m_iValueLength, SIZEOF( DRM_WORD ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_CERTPARSERCACHE_UnackCertParserEntry
**
** Synopsis: Function that unpacks a cert parser entry from a packed
**           word list buffer.
**
** Arguments:
**
** [f_iAttrib]              -- Index of cert parser entry to be packed.
** [f_pbPacked]             -- Pointer to a buffer to store the packed
**                             cert parser entries.
** [f_cbPacked]             -- Size of the buffer to store the packed
**                             cert parser entries.
** [f_poEntry]              -- Pointer to a cert parser entry.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: The words stored in the packed word list buffer is already in the
**       format of machine byte order.
**       Since it is an internal function, parameter check is relaxed for
**       performance reasons.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_CERTPARSERCACHE_UnpackCertParserEntry(
    __in  DRM_WORD                            f_iAttrib,
    __in_bcount( f_cbPacked )
          const DRM_BYTE                     *f_pbPacked,
    __in  DRM_DWORD                           f_cbPacked,
    __out DRM_DEVICE_CERT_PARSER_CACHE_ENTRY *f_poEntry )
{
    DRM_RESULT      dr      = DRM_SUCCESS;
    const DRM_BYTE *pbEntry = f_pbPacked + DRM_CB_CERT_PARSER_ENTRY * f_iAttrib;

    ChkArg( f_cbPacked == DRM_CB_CERT_PARSER_ENTRIES &&
            f_iAttrib < DRM_DEVCERT_MAXATTRIBUTES );

    MEMCPY( &f_poEntry->m_fValid, pbEntry, SIZEOF( DRM_WORD ) );
    pbEntry += SIZEOF( DRM_WORD );

    MEMCPY( &f_poEntry->m_iNodeStart, pbEntry, SIZEOF( DRM_WORD ) );
    pbEntry += SIZEOF( DRM_WORD );

    MEMCPY( &f_poEntry->m_iNodeLength, pbEntry, SIZEOF( DRM_WORD ) );
    pbEntry += SIZEOF( DRM_WORD );

    MEMCPY( &f_poEntry->m_iValueStart, pbEntry, SIZEOF( DRM_WORD ) );
    pbEntry += SIZEOF( DRM_WORD );

    MEMCPY( &f_poEntry->m_iValueLength, pbEntry, SIZEOF( DRM_WORD ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _LoadCertParserCache
**
** Synopsis: Function that loads the XML device parser cache from the key file.
**           It compares the SHA-1 hash of the passed in
**           DRM_DEVICE_CERT_PARSER_CACHE against the one in the key file.
**           If they match, load the cert parser cache from the key file.
**
** Arguments:
**
** [f_poKeyFileContext]     -- Pointer to a key file context.
** [f_poParserCache]        -- Pointer to a device cert parser cache.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: The caller makes sure that the certp parser cache field of the passed
**       in key file context contains a valid cert parser cache. This function
**       just uses the cert parser of the key file context without further
**       verification.
**
**********************************************************************/
static DRM_RESULT _LoadCertParserCache(
    __in_opt DRM_KEYFILE_CONTEXT         *f_poKeyFileContext,
    __inout DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_KEYFILE *poKeyFile = NULL;
    KF_CERT_PARSER_CACHE_OBJECT *poCertParserCache = NULL;

    ChkArg( f_poKeyFileContext != NULL );
    ChkArg( f_poParserCache != NULL );

    poKeyFile = &f_poKeyFileContext->keyfile;

    poCertParserCache = &poKeyFile->CertParserCache;

    if ( poCertParserCache->xbbaHash.cbData == DRM_SHA1_DIGEST_LEN &&
         poCertParserCache->wlData.cWORDs == ( DRM_CB_CERT_PARSER_ENTRIES / SIZEOF( DRM_WORD ) ) &&
         MEMCMP( poCertParserCache->xbbaHash.pbDataBuffer + poCertParserCache->xbbaHash.iData,
                 f_poParserCache->m_rgbSHAHash,
                 DRM_SHA1_DIGEST_LEN ) == 0 )
    {
        /* Load the cert parser cache. */
        MEMCPY( f_poParserCache->m_rgbPacked,
                poCertParserCache->wlData.pwordBuffer + poCertParserCache->wlData.iWords,
                poCertParserCache->wlData.cWORDs * SIZEOF( DRM_WORD ) );

        f_poParserCache->m_fUpdated = FALSE;
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _SaveCertParserCache
**
** Synopsis: Function that saves the XML device parser cache to the key file.
**
** Arguments:
**
** [f_poKeyFileContext]     -- Pointer to a key file context.
** [f_poParserCache]        -- Pointer to a device cert parser cache.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _SaveCertParserCache(
    __in_opt DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __in     DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poKeyFileContext != NULL );
    ChkArg( f_poParserCache != NULL );

    ChkDR( DRM_KF_SetCertParserCache( f_poKeyFileContext,
                                      f_poParserCache ) );

    f_poParserCache->m_fUpdated = FALSE;

ErrorExit:

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_CERTPARSERCACHE_DCP_StartCertParserCache(
    __in     const DRM_CONST_STRING       *f_pdstrDevCert,
    __in_opt DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __out    DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_KEYFILE *poKeyFile = NULL;
    DRM_SHA_CONTEXT oSHAContext = { 0 };

    ChkDRMString( f_pdstrDevCert );
    ChkArg( f_poParserCache != NULL );

    if ( f_poKeyFileContext != NULL )
    {
        poKeyFile = &f_poKeyFileContext->keyfile;
    }

    ZEROMEM( f_poParserCache, SIZEOF( DRM_DEVICE_CERT_PARSER_CACHE ) );

    f_poParserCache->m_fUsed = ( poKeyFile != NULL &&
                                 poKeyFile->fValid &&
                                 f_poKeyFileContext->hKeyfile != OEM_INVALID_HANDLE_VALUE &&
                                 f_pdstrDevCert->cchString <= DRM_MAX_CCH_DEVICE_CERT_FOR_CACHE );

    if ( f_poParserCache->m_fUsed )
    {
        ChkDR( DRM_SHA_Init( &oSHAContext, eDRM_SHA_1 ) );
        ChkDR( DRM_SHA_Update( PB_DSTR( f_pdstrDevCert ), CB_DSTR( f_pdstrDevCert ), &oSHAContext ) );
        ChkDR( DRM_SHA_Finalize( &oSHAContext, DRM_SHA1_DIGEST_LEN, f_poParserCache->m_rgbSHAHash ) );

        if ( poKeyFile->CertParserCache.fValid &&
             poKeyFile->CertParserCache.wVersion == DRM_CERT_PARSER_CACHE_VERSION_1 )
        {
            ChkDR( _LoadCertParserCache( f_poKeyFileContext,
                                         f_poParserCache ) );
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_CERTPARSERCACHE_DCP_StopCertParserCache(
    __in_opt DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __in     DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poParserCache != NULL );

    if ( f_poParserCache->m_fUsed && f_poParserCache->m_fUpdated )
    {
        ChkDR( _SaveCertParserCache( f_poKeyFileContext,
                                     f_poParserCache ) );

        /* Need to flush the result to the key file. */
        ChkDR( DRM_KF_StoreToFile( f_poKeyFileContext ) );
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

