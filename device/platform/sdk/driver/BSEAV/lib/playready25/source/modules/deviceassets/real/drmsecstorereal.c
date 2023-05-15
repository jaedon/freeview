/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMSECSTORE_C
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmsecurestore.h>
#include <oemshahmac.h>
#include <oem.h>
#include <drmdatastore.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*
    A secure store entry in the DST will look like this:
    4 bytes -- type of entry ( see type enum )
    20 bytes -- hmac of data with password
    n bytes of data
    The decrypted data will contain attributes name/value pairs like so:
    DRM_WORD ( length in bytes of the attribute name )
    n bytes of name
    TOKEN - 12 bytes
    --- Next attribute
*/

static const DRM_DST_NAMESPACE SEC_STORE_NAMESPACE                 = { 's', 'e', 'c', 'u', 'r', 'e', 's', 't', 'o', 'r', 'e', '\0' };
static const DRM_DST_NAMESPACE METER_STORE_NAMESPACE               = { 'm', 'e', 't', 'e', 'r', 'i', 'n', 'g', 's', 't', 'o', 'r', 'e', '\0' };
static const DRM_DST_NAMESPACE DEVICE_REGISTRATION_STORE_NAMESPACE = { 'd', 'e', 'v', 'i', 'c', 'e', 'r', 'e', 'g', 's', 't', 'o', 'r', 'e'  };
static const DRM_DST_NAMESPACE TRANSIENT_STORE_NAMESPACE           = { 't', 'r', 'a', 'n', 's', 'i', 'e', 'n', 't', 's', 't', 'o', 'r', 'e'  };

typedef enum
{
    ALLOW_ALL           = 0x0,
    DISALLOW_DEVICE_REG = 0x1,
    DISALLOW_RESERVED_2 = 0x2,  /* Reserved - Not Supported */
    DISALLOW_CACHEDCERT = 0x4,
    DISALLOW_REVOCATION = 0x8,
} eDRM_IS_VALID_SECURE_STORE_TYPE_OPTIONS;

static DRM_NO_INLINE DRM_BOOL _IsValidSecureStoreType(
    __in       eDRM_SECURE_STORE_TYPE f_eType,
    __in       DRM_DWORD              f_dwOptions )
{
    DRM_BOOL fAllowRevocation   = ((f_dwOptions & DISALLOW_REVOCATION ) == 0);
    DRM_BOOL fAllowDeviceReg    = ((f_dwOptions & DISALLOW_DEVICE_REG ) == 0);
    DRM_BOOL fAllowCachedCert   = ((f_dwOptions & DISALLOW_CACHEDCERT ) == 0);
    return ( ( f_eType == SECURE_STORE_LICENSE_DATA    )
          || ( f_eType == SECURE_STORE_GLOBAL_DATA     )
          || ( fAllowRevocation   && ( f_eType == SECURE_STORE_REVOCATION_DATA          ) )
          || ( f_eType == SECURE_STORE_METERING_DATA   )
          || ( f_eType == SECURE_STORE_TRANSIENT_DATA  )
          || ( fAllowDeviceReg    && ( f_eType == SECURE_STORE_DEVICE_REGISTRATION_DATA ) )
          || ( fAllowCachedCert   && ( f_eType == SECURE_STORE_CACHED_BLOB_DATA  ) ) );
}

static DRM_NO_INLINE const DRM_DST_NAMESPACE* _MapSecureStoreTypeToNamespace(
    __in       eDRM_SECURE_STORE_TYPE f_eType )
{
    switch( f_eType )
    {
    case SECURE_STORE_METERING_DATA:
        return &METER_STORE_NAMESPACE;
    case SECURE_STORE_DEVICE_REGISTRATION_DATA:
        return &DEVICE_REGISTRATION_STORE_NAMESPACE;
    case SECURE_STORE_TRANSIENT_DATA:
        return &TRANSIENT_STORE_NAMESPACE;
    case SECURE_STORE_GLOBAL_DATA: /* Fall through */
    case SECURE_STORE_REVOCATION_DATA:
    case SECURE_STORE_LICENSE_DATA:
    case SECURE_STORE_CACHED_BLOB_DATA:
    default:
        return &SEC_STORE_NAMESPACE;
    }
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "Buffer sizes are fixed and the function is static.")
static DRM_NO_INLINE DRM_VOID _PrepareTokenForWrite(
    __in_ecount(1)                           const TOKEN    *pToken,
    __out_ecount( SIZEOF( PERSISTEDTOKEN ) )       DRM_BYTE  pbPersistedToken[ SIZEOF( PERSISTEDTOKEN ) ] )
PREFAST_POP
{
    DRM_RESULT dr = DRM_SUCCESS;
    PERSISTEDTOKEN persistedToken = { 0 };

    ChkArg( pToken != NULL && pbPersistedToken != NULL );

    persistedToken.TokenType = pToken->TokenType;

    switch( persistedToken.TokenType )
    {
    case TOKEN_LONG:
        persistedToken.val.lValue = pToken->val.lValue;
        FIX_ENDIAN_LONG( persistedToken.val.lValue );
        break;
    case TOKEN_DATETIME:
        persistedToken.val.u64DateTime = pToken->val.u64DateTime;
        FIX_ENDIAN_QWORD( persistedToken.val.u64DateTime );
        break;
    case TOKEN_BYTEBLOB:
        persistedToken.val.lValue = (DRM_LONG)pToken->val.byteValue.cbBlob;
        FIX_ENDIAN_LONG( persistedToken.val.lValue );
        DRM_BYT_CopyBytes( pbPersistedToken,
                           SIZEOF( PERSISTEDTOKEN ),
                           pToken->val.byteValue.pbBlob,
                           0,
                           pToken->val.byteValue.cbBlob );
        break;
    default:
        DRMASSERT( FALSE );
        break;
    }
    FIX_ENDIAN_DWORD( persistedToken.TokenType );

    MEMCPY( pbPersistedToken, (DRM_BYTE*)&persistedToken, SIZEOF( PERSISTEDTOKEN ) );
ErrorExit:
    return;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "Buffer sizes are fixed and the function is static.")
static DRM_NO_INLINE DRM_VOID DRM_CALL _PrepareTokenForRead(
    __in_ecount( SIZEOF( PERSISTEDTOKEN ) ) DRM_BYTE  pbPersistedToken[ SIZEOF( PERSISTEDTOKEN ) ],
    __out_ecount(1)                         TOKEN    *pToken )
PREFAST_POP
{
    DRM_RESULT dr = DRM_SUCCESS;
    PERSISTEDTOKEN persistedToken;

    ChkArg( pToken != NULL && pbPersistedToken != NULL );

    /* Input may not be aligned */
    MEMCPY( (DRM_BYTE*)&persistedToken, pbPersistedToken, SIZEOF( PERSISTEDTOKEN ) );

    pToken->TokenType = persistedToken.TokenType;

    FIX_ENDIAN_DWORD( pToken->TokenType );
    switch( pToken->TokenType )
    {
    case TOKEN_LONG:
        pToken->val.lValue =  persistedToken.val.lValue;
        FIX_ENDIAN_LONG( pToken->val.lValue );
        break;
    case TOKEN_DATETIME:
        pToken->val.u64DateTime = persistedToken.val.u64DateTime;
        FIX_ENDIAN_QWORD( pToken->val.u64DateTime );
        break;
    case TOKEN_BYTEBLOB:
        pToken->val.byteValue.cbBlob = (DRM_DWORD)persistedToken.val.lValue;
        FIX_ENDIAN_DWORD( pToken->val.byteValue.cbBlob );
        pToken->val.byteValue.pbBlob = pbPersistedToken + __CB_DECL( SIZEOF( PERSISTEDTOKEN ) );
        break;
    default:
        DRMASSERT( FALSE );
        break;
    }

ErrorExit:
    return;
}

DRM_VOID _PersistedToken_To_Token(
    const PERSISTEDTOKEN *pPersistedToken,
    TOKEN                *pToken)
{
    DRMASSERT( pToken != NULL && pPersistedToken != NULL );

    if ( pToken == NULL || pPersistedToken == NULL )
    {
        return;
    }

    pToken->TokenType = pPersistedToken->TokenType;

    switch( pToken->TokenType )
    {
    case TOKEN_LONG:
        pToken->val.lValue =  pPersistedToken->val.lValue;
        break;
    case TOKEN_DATETIME:
        pToken->val.u64DateTime = pPersistedToken->val.u64DateTime;
        break;
    default:
        DRMASSERT( FALSE );
        break;
    }
}

DRM_NO_INLINE DRM_VOID _Token_To_PersistedToken(
    const TOKEN    *pToken,
    PERSISTEDTOKEN *pPersistedToken)
{
    DRMASSERT( pToken != NULL && pPersistedToken != NULL );

    if ( pToken == NULL || pPersistedToken == NULL )
    {
        return;
    }

    pPersistedToken->TokenType = pToken->TokenType;

    switch( pPersistedToken->TokenType )
    {
    case TOKEN_LONG:
        pPersistedToken->val.lValue =  pToken->val.lValue;
        break;
    case TOKEN_DATETIME:
        pPersistedToken->val.u64DateTime = pToken->val.u64DateTime;
        break;
     default:
        DRMASSERT( FALSE );
        break;
    }
}

/*******************************************************************
 *                  FUNCTION DRM_SST_LoadAttributesIntoCache
 *
 * purpose: scan through an entire slot that has been cached and populate an
 *          array of the data for easy manipulation later
 *
 * assumptions:  the data will be in this format
 * |2 |  cb  | Token |
 * |cb| Name | Value |
 ******************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_SST_LoadAttributesIntoCache(
    __in const DRM_BYTE        *pbSlotData,
    __in       DRM_DWORD        cbSlotData,
    __out      CachedAttribute  rgAttributes[DRM_MAX_ATTRIBUTES_PER_SST_KEY],
    __out      DRM_WORD        *pwNumAttributes )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    TOKEN      token = {0};

    ChkArg( pwNumAttributes != NULL );

    /* The first assert ensures that when we write tokens to disk they are align on boundaries that
       are compatible with the DRM_WCHAR alignment.  If they aren't most processors could have a
       alignment fault later when we access the strings.
       The second assert ensures that the size of token struct remains consistent (12 bytes) under
       all conditions (target processors, compiler switches).  This is eminent for maintaining
       backward compatability */
    DRMCASSERT( ( SIZEOF( PERSISTEDTOKEN ) % SIZEOF( DRM_WCHAR ) ) == 0 );
    DRMSIZEASSERT( SIZEOF( PERSISTEDTOKEN ), 12 );

    for( *pwNumAttributes = 0;
         ( *pwNumAttributes < DRM_MAX_ATTRIBUTES_PER_SST_KEY ) && ( cbSlotData > 0 );
         (*pwNumAttributes)++ )
    {
        DRM_WORD cbString = 0;

        if( cbSlotData < SIZEOF( DRM_WORD ) )
        {
            /* Absolutely too small */
            dr = DRM_E_SECURESTORE_CORRUPT;
            goto ErrorExit;
        }
        MEMCPY( &cbString, pbSlotData, SIZEOF( cbString ) );
        FIX_ENDIAN_WORD( cbString );
        pbSlotData += __CB_DECL(SIZEOF( DRM_WORD ));
        cbSlotData -= SIZEOF( DRM_WORD );

        if( cbSlotData < cbString + SIZEOF( PERSISTEDTOKEN ) )
        {
            /* There are not enough bytes left in the slot to finish this token out */
            dr = DRM_E_SECURESTORE_CORRUPT;
            goto ErrorExit;
        }

        DSTR_FROM_PB( &rgAttributes[*pwNumAttributes].dstrAttribute, pbSlotData, cbString );

        pbSlotData += __CB_DECL(cbString);
        cbSlotData -= cbString;

        rgAttributes[*pwNumAttributes].pTokenValue = (DRM_BYTE*)pbSlotData;
        pbSlotData += __CB_DECL(SIZEOF( PERSISTEDTOKEN ));
        cbSlotData -= SIZEOF( PERSISTEDTOKEN );

        _PrepareTokenForRead( rgAttributes[*pwNumAttributes].pTokenValue, &token );
        if( token.TokenType == TOKEN_BYTEBLOB )
        {
            /* For binary tokens the binary data follows the token struct */
            pbSlotData += __CB_DECL( token.val.byteValue.cbBlob + (token.val.byteValue.cbBlob % 2));
            cbSlotData -= __CB_DECL( token.val.byteValue.cbBlob + (token.val.byteValue.cbBlob % 2));
        }

        ZEROMEM( &rgAttributes[*pwNumAttributes].TokenDelta, SIZEOF( rgAttributes[*pwNumAttributes].TokenDelta ) );
        rgAttributes[*pwNumAttributes].dwFlags = 0;
    }

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _VerifySlotHash(
    __inout     DRM_SECSTORE_CONTEXT *pcontextSST,
    __in        DRM_BOOL              fRaw,
    __in  const DRM_BYTE              rgbPassword [__CB_DECL(DRM_SHA1_DIGEST_LEN)] )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    const DRM_BYTE  *pbSlotHash     = NULL;
    DRM_BYTE        *pbBuffer       = NULL;
    DRM_BYTE         rgbVerifiedHash[__CB_DECL(DRM_SHA1_DIGEST_LEN)] = { 0 };
    DRM_DWORD        cbBuffer       = 0;
    DRM_DWORD        cbHeader       = 0;
    DRM_HMAC_CONTEXT oHmacContext   = DRM_HMAC_CONTEXT_DEFAULT;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMSECSTORE, PERF_FUNC__VerifySlotHash);

    cbHeader = ( pcontextSST->dwSlotVersion == 0 )
             ? DRM_SST_SLOT_V0_HEADER_SIZE
             : DRM_SST_SLOT_HEADER_SIZE;
    
    /* Ensure that the slot data size is at least big enough for the header data */
    ChkBOOL(pcontextSST->cbSlotData >= cbHeader, DRM_E_SECURESTORE_CORRUPT);
    
    pbBuffer = pcontextSST->rgbSlotData + __CB_DECL( cbHeader );


    /* If it is a raw slot the min-header has been loaded.
       Other wise the whole slot has been cached already */
    ChkDR( DRM_HMAC_Init( &oHmacContext, eDRM_SHA_1, rgbPassword, DRM_SHA1_DIGEST_LEN ) );

    if( fRaw )
    {
        DRM_DWORD cbRead = 0;
        DRM_DWORD cbSlot = pcontextSST->cbSlotData - cbHeader;

        cbBuffer = SIZEOF( pcontextSST->rgbSlotData ) - cbHeader;
        while( cbSlot > 0 )
        {
            DRM_DWORD cbToRead = min( cbBuffer, cbSlot );

            ChkDR( DRM_DST_SlotRead( pcontextSST->pDatastore,
                                     &pcontextSST->oSlotContext,
                                     cbToRead,
                                     pbBuffer,
                                     &cbRead ) );
            if( cbToRead != cbRead )
            {
                dr = DRM_E_FILE_READ_ERROR;
                goto ErrorExit;
            }
            ChkDR( DRM_HMAC_Update( &oHmacContext, pbBuffer, cbRead ) );
            cbSlot -= cbRead;
        }
    }
    else
    {
        cbBuffer = pcontextSST->cbSlotData  - cbHeader;
        ChkDR( DRM_HMAC_Update( &oHmacContext, pbBuffer, cbBuffer ) );
    }

    ChkDR( DRM_HMAC_Finalize( &oHmacContext, rgbVerifiedHash, DRM_SHA1_DIGEST_LEN ) );

    pbSlotHash = ( pcontextSST->dwSlotVersion == 0 )
               ? ( pcontextSST->rgbSlotData + __CB_DECL(DRM_SST_SLOT_V0_HASH_OFFSET) )
               : ( pcontextSST->rgbSlotData + __CB_DECL(DRM_SST_SLOT_HASH_OFFSET) );
    if( 0 != MEMCMP( pbSlotHash, rgbVerifiedHash, DRM_SHA1_DIGEST_LEN ) )
    {
        dr = DRM_E_INVALID_SECURESTORE_PASSWORD;
        goto ErrorExit;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_NO_INLINE DRM_RESULT _InitSlot(
    __inout       DRM_SECSTORE_CONTEXT  *pcontextSST,
    __in          DRM_BOOL               fRaw,
    __in          eDRM_SECURE_STORE_TYPE eType,
    __in    const DRM_BYTE               rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)])
{
    DRM_DWORD        dwSlotMetaData = eType;
    DRM_RESULT       dr        = DRM_SUCCESS;
    DRM_DWORD        cbWritten = 0;
    DRM_DWORD        cbToWrite = 0;
    DRM_HMAC_CONTEXT hmac      = DRM_HMAC_CONTEXT_DEFAULT;

    if ( fRaw )
    {
        dwSlotMetaData |= DRM_SST_RAW_DATA;
    }

    /*  Set the slot version    */
    pcontextSST->dwSlotVersion = DRM_SST_SLOT_VERSION;

    /* Secure store section was just created.  Add the Initial hash */
    ChkDR(DRM_HMAC_Init(&hmac, eDRM_SHA_1, rgbPassword, DRM_SHA1_DIGEST_LEN));
    ChkDR(DRM_HMAC_FinalizeOffset(&hmac, pcontextSST->rgbSlotData, DRM_SST_SLOT_HASH_OFFSET, DRM_SHA1_DIGEST_LEN ) );

    DRM_SST_SET_SLOT_METADATA(dwSlotMetaData);
    DWORD_TO_BYTES(pcontextSST->rgbSlotData, dwSlotMetaData);
    DWORD_TO_BYTES(pcontextSST->rgbSlotData + __CB_DECL( SIZEOF( dwSlotMetaData)), pcontextSST->cbSlotData);

    cbToWrite = SIZEOF(dwSlotMetaData)
              + SIZEOF(pcontextSST->cbSlotData)
              + DRM_SHA1_DIGEST_LEN;

    ChkDR( DRM_DST_SlotSeek( pcontextSST->pDatastore,
                             &pcontextSST->oSlotContext,
                             0,
                             eDRM_DST_SEEKSET,
                             NULL ) );
    ChkDR( DRM_DST_SlotWrite( pcontextSST->pDatastore,
                              &pcontextSST->oSlotContext,
                              cbToWrite,
                              pcontextSST->rgbSlotData,
                              &cbWritten ) );
    if ( cbWritten != cbToWrite )
    {
        dr = DRM_E_FILE_WRITE_ERROR;
        goto ErrorExit;
    }
#if !_DATASTORE_WRITE_THRU
    ChkDR( pcontextSST->pDatastore->oDataStoreInterface.pfnCommitNamespace(&pcontextSST->oNsContext) );
#endif

ErrorExit:
    return dr;
}


static DRM_NO_INLINE DRM_RESULT _LoadSlot(
    __inout       DRM_SECSTORE_CONTEXT  *pcontextSST,
    __in          DRM_BOOL               fRaw,
    __in          eDRM_SECURE_STORE_TYPE eType,
    __in    const DRM_BYTE               rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)])
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cbRead    = 0;
    DRM_DWORD  cbToRead  = 0;
    DRM_DWORD  dwSlotMetaData = 0;

    /* A secure store slot is never this small.  Somethings wrong */
    ChkBOOL(pcontextSST->cbSlot >= DRM_SST_SLOT_V0_HEADER_SIZE, DRM_E_SECURESTORE_CORRUPT);

    if ( fRaw )
    {
        cbToRead = DRM_SST_SLOT_V0_HEADER_SIZE;
    }
    else
    {
        ChkBOOL((pcontextSST->cbSlot <= SIZEOF(pcontextSST->rgbSlotData)), DRM_E_SECURESTORE_FULL);
        cbToRead = pcontextSST->cbSlot;
    }

    ChkDR( DRM_DST_SlotRead( pcontextSST->pDatastore,
                             &pcontextSST->oSlotContext,
                             cbToRead,
                             pcontextSST->rgbSlotData,
                             &cbRead ) );
    ChkBOOL(cbToRead == cbRead, DRM_E_FILE_READ_ERROR);

    /*
    **  First check the version number
    */
    BYTES_TO_DWORD( dwSlotMetaData, pcontextSST->rgbSlotData );
    pcontextSST->dwSlotVersion = dwSlotMetaData & DRM_SST_SLOT_VERSION_MASK;
    if ( pcontextSST->dwSlotVersion == 0 )
    {
        if ( (DRM_SST_SLOT_SIZE_VALID_MASK & dwSlotMetaData) != 0 )
        {
            DRM_SST_GET_SLOT_SIZE_FROM_METADATA(pcontextSST->cbSlotData, dwSlotMetaData);
        }
        dwSlotMetaData &= ~DRM_SST_SLOT_SIZE_MASK;
    }
    else
    {
        if ( fRaw )
        {
            /*
            **  We need to read one more DWORD. This DWORD is just the last
            **  DWORD of the hash. We haven't read it yet because we first
            **  read just DRM_SST_SLOT_V0_HEADER_SIZE amount of data from
            **  the beginning of the slot.
            */
            ChkDR( DRM_DST_SlotRead( pcontextSST->pDatastore,
                                     &pcontextSST->oSlotContext,
                                     DRM_SST_SLOT_SIZEDATA_SIZE,
                                     pcontextSST->rgbSlotData + __CB_DECL( DRM_SST_SLOT_V0_HEADER_SIZE ),
                                     &cbRead ) );
            ChkBOOL(cbRead == DRM_SST_SLOT_SIZEDATA_SIZE, DRM_E_FILE_READ_ERROR);
        }
        BYTES_TO_DWORD( pcontextSST->cbSlotData,
                        pcontextSST->rgbSlotData + __CB_DECL( DRM_SST_SLOT_METADATA_SIZE ) );
    }

    if ( (DRM_SST_SLOT_SIZE_VALID_MASK & dwSlotMetaData) == 0 )
    {
        pcontextSST->cbSlotData = pcontextSST->cbSlot;
    }
    else
    {
        dwSlotMetaData &= ~((DRM_DWORD)DRM_SST_SLOT_SIZE_VALID_MASK);
    }

    dwSlotMetaData &= ~((DRM_DWORD)DRM_SST_SLOT_VERSION_MASK);

    ChkBOOL(pcontextSST->cbSlotData <= pcontextSST->cbSlot, DRM_E_SECURESTORE_CORRUPT);
    if ( fRaw )
    {
        ChkBOOL((dwSlotMetaData == (((DRM_DWORD)eType)|((DRM_DWORD)DRM_SST_RAW_DATA))), DRM_E_SECURESTORE_CORRUPT);
    }
    else
    {
        ChkBOOL((dwSlotMetaData == (DRM_DWORD)eType), DRM_E_SECURESTORE_CORRUPT);
    }

    /* verify the slot if password is given */
    if ( !pcontextSST->fNoPassword )
    {
        dr = _VerifySlotHash(pcontextSST, fRaw, rgbPassword);
    }

ErrorExit:

    return dr;
}
static DRM_NO_INLINE DRM_RESULT _OpenAndVerifySlot(
    __inout       DRM_SECSTORE_CONTEXT    *pcontextSST,
    __in          DRM_DST                 *pDatastore,
    __in    const DRM_BYTE                 rgbPassword [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in          eDRM_SECURE_STORE_TYPE   eType,
    __in          DRM_BOOL                 fRaw,
    __in          DRM_DWORD                dwFlags,
    __in          DRM_DWORD                cbMinSlotSizeForCreate)
{
    DRM_RESULT dr              = DRM_SUCCESS;
    DRM_BOOL   fCloseNamespace = FALSE,
               fCloseSlot      = FALSE,
               fCreated        = FALSE;
    DRM_DWORD  dwSlotMetaData  = eType;
    DRM_DWORD  cbPreallocatedSlot  = max( cbMinSlotSizeForCreate,
                                            ( eType == SECURE_STORE_DEVICE_REGISTRATION_DATA)
                                                ?  (DRM_DWORD)DRM_SST_DEVREG_PREALLOCATE_SIZE
                                                :  (DRM_DWORD)DRM_SST_PREALLOCATE_SIZE );
    DRM_DWORD  dwDstOpenMode = 0;

    pcontextSST->fNoPassword = FALSE;
    pcontextSST->fDirty      = FALSE;
    pcontextSST->pDatastore  = pDatastore;

    if ( rgbPassword == NULL
      || DRM_UTL_IsZeros(rgbPassword, DRM_SHA1_DIGEST_LEN) )
    {
        MEMSET( pcontextSST->rgbPassword, 0, DRM_SHA1_DIGEST_LEN );
        pcontextSST->fNoPassword = TRUE;
    }

    if ( ( dwFlags & DRM_SECURE_STORE_CREATE_NEW ) != 0 )
    {
        ChkDR( DRM_DST_OpenNamespace( pDatastore,
                                      pcontextSST->pNamespaceId,
                                      (DRM_DST_OPENMODE)(eDRM_DST_OPEN_ALWAYS | eDRM_DST_LOCKWAIT),
                                      DRM_SECURE_STORE_NUM_CHILD_NODES,
                                      DRM_SST_PREALLOCATE_SIZE,
                                      &pcontextSST->oNsContext ) );

        /* Clear the slot hint or it'll try to search there first */
        ZEROMEM( &pcontextSST->slotHint, SIZEOF( DRM_DST_SLOT_HINT ) );
        dwDstOpenMode = eDRM_DST_CREATE_NEW;
    }
    else if ( ( dwFlags & DRM_SECURE_STORE_OPEN_ALWAYS ) != 0 )
    {
        ChkDR( DRM_DST_OpenNamespace( pDatastore,
                                      pcontextSST->pNamespaceId,
                                      (DRM_DST_OPENMODE)(eDRM_DST_OPEN_ALWAYS | eDRM_DST_LOCKWAIT),
                                      DRM_SECURE_STORE_NUM_CHILD_NODES,
                                      DRM_SST_PREALLOCATE_SIZE,
                                      &pcontextSST->oNsContext ) );

        dwDstOpenMode = eDRM_DST_OPEN_ALWAYS;
    }
    else if( ( dwFlags & DRM_SECURE_STORE_OPEN_EXISTING ) != 0 )
    {
        dr = DRM_DST_OpenNamespace( pDatastore,
                                    pcontextSST->pNamespaceId,
                                    (DRM_DST_OPENMODE)(eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT),
                                    DRM_SECURE_STORE_NUM_CHILD_NODES,
                                    DRM_SST_PREALLOCATE_SIZE,
                                    &pcontextSST->oNsContext );
        if ( dr == DRM_E_DST_NAMESPACE_NOT_FOUND )
        {
            dr = DRM_E_DST_SLOT_NOT_FOUND;
        }
        ChkDR(dr);

        dwDstOpenMode = eDRM_DST_OPEN_EXISTING;
    }
    else
    {
        /* Invalid open mode */
        ChkArg( FALSE );
    }
    fCloseNamespace = TRUE;

    if( fRaw )
    {
        dwSlotMetaData |= DRM_SST_RAW_DATA;
    }

    if ( ( dwFlags & DRM_SECURE_STORE_OPEN_ALWAYS ) != 0
     ||  ( dwFlags & DRM_SECURE_STORE_CREATE_NEW ) != 0 )
    {

        pcontextSST->cbSlotData = DRM_SST_SLOT_HEADER_SIZE;
        pcontextSST->cbSlot     = DRM_SST_SLOT_HEADER_SIZE
                                + cbPreallocatedSlot;
    }

    dr = DRM_DST_OpenSlot( pDatastore,
                           &pcontextSST->oNsContext,
                           dwDstOpenMode | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                           &pcontextSST->rgbKey1,
                           &pcontextSST->rgbKey2,
                           &pcontextSST->slotHint,
                           &pcontextSST->cbSlot,
                           &pcontextSST->oSlotContext);
    if ( ( ( dwFlags & DRM_SECURE_STORE_OPEN_ALWAYS )          && dr == DRM_S_FALSE )
     ||  ( ( dwFlags & DRM_SECURE_STORE_CREATE_NEW ) && DRM_SUCCEEDED( dr ) ) )
    {
        fCreated = TRUE;
        dr = DRM_SUCCESS;
    }
    ChkDR( dr );
    fCloseSlot = TRUE;

    /* The slot is open.*/
    if( fCreated )
    {
        dr = _InitSlot(pcontextSST, fRaw, eType, rgbPassword);
    }
    else
    {
        dr = _LoadSlot(pcontextSST, fRaw, eType, rgbPassword);
    }
    pcontextSST->eType = eType;

ErrorExit:
    if( DRM_FAILED( dr ) )
    {
        if( fCloseSlot )
        {
            DRM_DST_CloseSlot( pDatastore, &pcontextSST->oSlotContext );
            ZEROMEM( &pcontextSST->slotHint, SIZEOF( DRM_DST_SLOT_HINT ) );
        }
        if( fCloseNamespace )
        {
            DRM_DST_CloseNamespace( pDatastore, &pcontextSST->oNsContext );
        }
    }
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_LoadKeyTokens(
    __inout    DRM_SECSTORE_CONTEXT   *pcontextSST,
    __in       DRM_DWORD               dwFlags,
    __in       DRM_DST                *pDatastore )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbHeader = 0;
    DRM_BOOL   fClose = FALSE;

    ChkArg( pcontextSST != NULL
         && pDatastore != NULL
         && pcontextSST->fInited );

    if( pcontextSST->fOpened )
    {
        /* Already open */
        goto ErrorExit;
    }

    ChkDR( _OpenAndVerifySlot( pcontextSST, pDatastore, pcontextSST->rgbPassword, pcontextSST->eType, FALSE, dwFlags, 0) );
    fClose = TRUE;

    /* The data has been read and verified.  Run through the data and create a map of the keys */
    cbHeader = ( pcontextSST->dwSlotVersion == 0)
             ? DRM_SST_SLOT_V0_HEADER_SIZE
             : DRM_SST_SLOT_HEADER_SIZE;
    ChkDR( DRM_SST_LoadAttributesIntoCache( pcontextSST->rgbSlotData + __CB_DECL( cbHeader ),
                                            pcontextSST->cbSlotData  - cbHeader,
                                            pcontextSST->rgAttributes,
                                           &pcontextSST->wNumAttributes ) );

    pcontextSST->wNumOriginalAttributes = pcontextSST->wNumAttributes;
    pcontextSST->fOpened = TRUE;

ErrorExit:
    if ( fClose
      && pcontextSST != NULL )
    {
        DRM_DST_CloseSlot( pDatastore, &pcontextSST->oSlotContext );
        DRM_DST_CloseNamespace( pDatastore, &pcontextSST->oNsContext );
    }
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL _LoadKeyTokens(
    __inout    DRM_SECSTORE_CONTEXT   *pcontextSST,
    __in       DRM_DST                *pDatastore )
{
    return DRM_SST_LoadKeyTokens( pcontextSST, DRM_SECURE_STORE_OPEN_ALWAYS, pDatastore );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_OpenKeyTokens(
    __inout                                  DRM_SECSTORE_CONTEXT   *pcontextSST,
    __in                               const DRM_ID                 *pKey1,
    __in_opt                           const DRM_ID                 *pKey2,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                rgbPassword [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                     DRM_DWORD               dwFlags,
    __in                                     eDRM_SECURE_STORE_TYPE  eType,
    __in                                     DRM_DST                *pDatastore )
{
    DRM_RESULT  dr        = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_OpenKeyTokens);

    ChkArg( pcontextSST != NULL
         && pKey1       != NULL
         && pDatastore != NULL );

    ZEROMEM( pcontextSST, SIZEOF( *pcontextSST ) );

    /* Check for supported flags */
    if ( ( dwFlags & ~(DRM_SECURE_STORE_CREATE_NEW | DRM_SECURE_STORE_OPEN_ALWAYS) ) ||
        !( _IsValidSecureStoreType(eType, DISALLOW_DEVICE_REG | DISALLOW_REVOCATION | DISALLOW_CACHEDCERT) ) )
    {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    pcontextSST->fNoPassword = FALSE;
    if ( rgbPassword == NULL
      || DRM_UTL_IsZeros(rgbPassword, DRM_SHA1_DIGEST_LEN) )
    {
        MEMSET( pcontextSST->rgbPassword, 0, DRM_SHA1_DIGEST_LEN );
        pcontextSST->fNoPassword = TRUE;
    }

    pcontextSST->pNamespaceId = _MapSecureStoreTypeToNamespace( eType );

    /* Copy the IDs so we can open the right slot later */
    MEMCPY( &pcontextSST->rgbKey1, pKey1, SIZEOF( pcontextSST->rgbKey1 ) );
    if( pKey2 )
    {
        MEMCPY( &pcontextSST->rgbKey2, pKey2, SIZEOF( pcontextSST->rgbKey2 ) );
    }
    else
    {
        ZEROMEM( &pcontextSST->rgbKey2, SIZEOF( pcontextSST->rgbKey2 ) );
    }

    if ( !pcontextSST->fNoPassword )
    {
        MEMCPY( pcontextSST->rgbPassword, rgbPassword, DRM_SHA1_DIGEST_LEN );
    }

    pcontextSST->eType       = eType;
    pcontextSST->pDatastore  = pDatastore;
    pcontextSST->fInited     = TRUE;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return ( dr );
}

/******************************************************************************
**
** Function :   DRM_SST_OpenAndLockSlot
**
** Synopsis :   Opens and locks the appropriate slot
**
** Arguments :  f_pDatastore    :
**              f_eType         :   Type of secure store
**              f_pKey1         :   Key1 buffer
**              f_pKey2         :   Key2 buffer
**              f_rgbPassword   :   Secure store password
**              f_dwFlags       :   Only allowed flags for now is
**                                  DRM_SECURE_STORE_CREATE_IF_NOT_EXIST | DRM_SECURE_STORE_OPEN_ALWAYS
**              f_pcontextSST   :   Secure store context for opened slot
**              f_pcbData       :   (Optional - can be NULL)If slot already
**                                  exists, on return it contains slot size
**
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_OpenAndLockSlot(
    __in                                     DRM_DST                *f_pDatastore,
    __in                                     eDRM_SECURE_STORE_TYPE  f_eType,
    __in                               const DRM_ID                 *f_pKey1,
    __in_opt                           const DRM_ID                 *f_pKey2,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                f_rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                     DRM_DWORD               f_dwFlags,
    __out                                    DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __out_opt                                DRM_DWORD              *f_pcbData )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    /*
    **  Check input
    */
    ChkArg( (f_dwFlags & ~DRM_SECURE_STORE_VALID_FLAGS) == 0 );
    ChkArg( f_pcontextSST != NULL
         && f_pDatastore != NULL
         && f_pKey1       != NULL );

    f_pcontextSST->fNoPassword = FALSE;
    if ( f_rgbPassword == NULL
      || DRM_UTL_IsZeros(f_rgbPassword, DRM_SHA1_DIGEST_LEN) )
    {
        MEMSET( f_pcontextSST->rgbPassword, 0, DRM_SHA1_DIGEST_LEN );
        f_pcontextSST->fNoPassword = TRUE;
    }

    /*
    **  Clear the output flag in secstore
    */
    f_pcontextSST->fLocked = FALSE;

    /*
    **  Check for supported flags
    */
    if ( !_IsValidSecureStoreType( f_eType, ALLOW_ALL ) )
    {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    /*
    **  Initialize the secstore context
    */
    ZEROMEM( f_pcontextSST, SIZEOF( *f_pcontextSST ) );
    f_pcontextSST->pNamespaceId = _MapSecureStoreTypeToNamespace( f_eType );

    /*
    **  Copy the IDs so we can open the right slot later
    */
    MEMCPY( &f_pcontextSST->rgbKey1, f_pKey1, SIZEOF( f_pcontextSST->rgbKey1 ) );
    if( f_pKey2 != NULL )
    {
        MEMCPY( &f_pcontextSST->rgbKey2, f_pKey2, SIZEOF( f_pcontextSST->rgbKey2 ) );
    }
    else
    {
        ZEROMEM(&f_pcontextSST->rgbKey2, SIZEOF( f_pcontextSST->rgbKey2 ) );
    }

    /*
    **  Open and lock slot
    */
    ChkDR( _OpenAndVerifySlot( f_pcontextSST,
                               f_pDatastore,
                               f_rgbPassword,
                               f_eType,
                               TRUE,
                               f_dwFlags,
                               f_pcbData == NULL ? 0 : *f_pcbData ) );

    /*
    **  Save the securestore password
    */
    if ( !f_pcontextSST->fNoPassword )
    {
        MEMCPY( f_pcontextSST->rgbPassword,
                f_rgbPassword,
                SIZEOF(f_pcontextSST->rgbPassword) );
    }

    /*
    **  Set the slot size for the caller
    */
    if (f_pcbData != NULL)
    {
        *f_pcbData = ( f_pcontextSST->dwSlotVersion == 0 )
                   ? f_pcontextSST->cbSlotData - DRM_SST_SLOT_V0_HEADER_SIZE
                   : f_pcontextSST->cbSlotData - DRM_SST_SLOT_HEADER_SIZE;
    }

    /*
    **  Secstore context now holds the lock. Set appropriate flag
    */
    f_pcontextSST->fLocked = TRUE;

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   DRM_SST_GetLockedData
**
** Synopsis :   Gets data from the slot if this thread holds the lock to the slot
**
** Arguments :  f_pcontextSST   : Secure store context
**              f_pbData        : Buffer for slot data
**              f_pcbData       : Buffer size; if slot is bigger, on return, it
**                                would contain the slot size
**
** Returns :    DRM_E_SECURESTORE_LOCK_NOT_OBTAINED if  DRM_SST_OpenAndLockSlot has
**              not been successfully called prior to this function
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_GetLockedData(
    __in                           DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __out_bcount_opt( *f_pcbData ) DRM_BYTE               *f_pbData,
    __out                          DRM_DWORD              *f_pcbData )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_DWORD   cbRemaining = 0;
    DRM_DWORD   cbHeader    = 0;

    ChkArg( f_pcontextSST != NULL
         && f_pcbData     != NULL );

    /*
    **  Check if the caller holds the lock for the secure store
    */
    if (!f_pcontextSST->fLocked)
    {
        ChkDR(DRM_E_SECURESTORE_LOCK_NOT_OBTAINED);
    }

    /*
    **  Check size of input buffer
    */
    cbHeader = ( f_pcontextSST->dwSlotVersion == 0 )
             ? DRM_SST_SLOT_V0_HEADER_SIZE
             : DRM_SST_SLOT_HEADER_SIZE;
    cbRemaining = f_pcontextSST->cbSlotData - cbHeader;

    if( f_pbData == NULL || *f_pcbData < cbRemaining )
    {
        *f_pcbData = cbRemaining;
        dr = DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }
    *f_pcbData = cbRemaining;

    /*
    **  Get the data
    */
    ChkDR( DRM_DST_SlotSeek( f_pcontextSST->pDatastore,
                             &f_pcontextSST->oSlotContext,
                             (DRM_LONG)cbHeader,
                             eDRM_DST_SEEKSET, NULL ) );
    ChkDR( DRM_DST_SlotRead( f_pcontextSST->pDatastore,
                             &f_pcontextSST->oSlotContext,
                             cbRemaining,
                             f_pbData,
                             f_pcbData ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_SST_SetLockedData
**
** Synopsis :   Sets data into the slot if this thread holds the lock to the slot
**
** Arguments :  f_pcontextSST   : Secure store context
**              f_cbData        : Buffer size;
**              f_pbData        : Data Buffer
**
** Returns :    DRM_E_SECURESTORE_LOCK_NOT_OBTAINED if  DRM_SST_OpenAndLockSlot has
**              not been successfully called prior to this function
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_SetLockedData(
    __in                          DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in                          DRM_DWORD               f_cbData,
    __in_bcount( f_cbData ) const DRM_BYTE               *f_pbData )
{
    DRM_RESULT        dr              = DRM_SUCCESS;
    DRM_DWORD         cbWritten       = 0;
    DRM_DWORD         dwSlotMetaData  = 0;

    ChkArg( f_pcontextSST != NULL
         && f_pbData      != NULL );

    /*
    **  Check if the caller holds the lock for the secure store
    */
    if (!f_pcontextSST->fLocked)
    {
        ChkDR(DRM_E_SECURESTORE_LOCK_NOT_OBTAINED);
    }


    f_pcontextSST->cbSlotData = DRM_SST_SLOT_HEADER_SIZE + f_cbData;
    if ( f_pcontextSST->cbSlotData > f_pcontextSST->cbSlot )
    {
        ChkDR( DRM_DST_SlotResize( f_pcontextSST->pDatastore,
                                   &f_pcontextSST->oSlotContext,
                                   f_pcontextSST->cbSlotData ) );
    }

    /*
    **  If this was a v0 slot, update it to current version
    */
    if ( f_pcontextSST->dwSlotVersion == 0 )
    {
        /* Clear the old format size fields in the slot meta data */
        BYTES_TO_DWORD( dwSlotMetaData, f_pcontextSST->rgbSlotData );
        DRM_SST_SET_SLOT_METADATA( dwSlotMetaData );
        DWORD_TO_BYTES( f_pcontextSST->rgbSlotData, dwSlotMetaData );
    }

    /*
    **  Obtain HMAC of data to be written to secstore
    */
    ChkDR( DRM_HMAC_CreateMAC( eDRM_SHA_1,
                               f_pcontextSST->rgbPassword,
                               DRM_SHA1_DIGEST_LEN,
                               f_pbData,
                               0,
                               f_cbData,
                               f_pcontextSST->rgbSlotData,
                               DRM_SST_SLOT_HASH_OFFSET,
                               DRM_SHA1_DIGEST_LEN ) );

    /*
    **  Find the right slot
    */
    ChkDR( DRM_DST_SlotSeek( f_pcontextSST->pDatastore,
                             &f_pcontextSST->oSlotContext,
                             0,
                             eDRM_DST_SEEKSET,
                             NULL ) );

    /* Update the size in the slot meta data */
    DWORD_TO_BYTES( f_pcontextSST->rgbSlotData + __CB_DECL( DRM_SST_SLOT_METADATA_SIZE ),
                    f_pcontextSST->cbSlotData );

    if( DRM_SST_SLOT_HEADER_SIZE + f_cbData <= SIZEOF( f_pcontextSST->rgbSlotData )
     && DRM_SST_SLOT_HEADER_SIZE + f_cbData  > f_cbData )
    {
        /* Copy to in memory buffer to minimize calls to SlotWrite */
        DRM_BYT_CopyBytes( f_pcontextSST->rgbSlotData, DRM_SST_SLOT_DATA_OFFSET, f_pbData, 0, f_cbData );
        ChkDR( DRM_DST_SlotWrite( f_pcontextSST->pDatastore,
                                  &f_pcontextSST->oSlotContext,
                                  f_pcontextSST->cbSlotData,
                                  f_pcontextSST->rgbSlotData,
                                  &cbWritten ) );
        if ( cbWritten != f_pcontextSST->cbSlotData )
        {
            ChkDR( DRM_E_FILE_WRITE_ERROR );
        }
    }
    else
    {

        /*
        **  Write HMAC to secstore
        */
        ChkDR( DRM_DST_SlotWrite( f_pcontextSST->pDatastore,
                                  &f_pcontextSST->oSlotContext,
                                  DRM_SST_SLOT_HEADER_SIZE,
                                  f_pcontextSST->rgbSlotData,
                                  &cbWritten ) );
        if ( cbWritten != DRM_SST_SLOT_HEADER_SIZE )
        {
            ChkDR( DRM_E_FILE_WRITE_ERROR );
        }

        /*
        **  Write data to slot
        */
        ChkDR( DRM_DST_SlotWrite( f_pcontextSST->pDatastore,
                                  &f_pcontextSST->oSlotContext,
                                  f_cbData,
                                  f_pbData,
                                  &cbWritten ) );
        if( cbWritten != f_cbData )
        {
            ChkDR( DRM_E_FILE_WRITE_ERROR );
        }
    }

#if !_DATASTORE_WRITE_THRU
    ChkDR( DRM_DST_CommitNamespace( f_pcontextSST->pDatastore,
                                    &f_pcontextSST->oNsContext ) );
#endif

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_SST_CloseLockedSlot
**
** Synopsis :   Closes the slot, releasing lock in the process
**
** Arguments :  f_pcontextSST - secure store context
**
** Returns :    DRM_SUCCESS
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_CloseLockedSlot(
    __in  DRM_SECSTORE_CONTEXT *f_pcontextSST )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkArg( f_pcontextSST != NULL );

    /*
    **  Close the slot - release lock if the caller holds the lock for the
    **  secure store
    */
    if (f_pcontextSST->fLocked)
    {
        DRM_DST_CloseSlot( f_pcontextSST->pDatastore, &f_pcontextSST->oSlotContext );
        DRM_DST_CloseNamespace( f_pcontextSST->pDatastore, &f_pcontextSST->oNsContext );
        f_pcontextSST->fLocked = FALSE;
    }

ErrorExit:
    if( f_pcontextSST != NULL )
    {
        ZEROMEM( &f_pcontextSST->slotHint, SIZEOF( DRM_DST_SLOT_HINT ) );
    }
    return dr;
}

/******************************************************************************
**
** Function :   DRM_SST_GetData
**
** Synopsis :
**
** Arguments :
**
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_GetData(
    __in                                     DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in                               const DRM_ID                 *f_pKey1,
    __in_opt                           const DRM_ID                 *f_pKey2,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                f_rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                     eDRM_SECURE_STORE_TYPE  f_eType,
    __in                                     DRM_DST                *f_pDatastore,
    __out_bcount_opt( *f_pcbData )           DRM_BYTE               *f_pbData,
    __out                                    DRM_DWORD              *f_pcbData )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   cbData  = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_GetData);

    /*
    **  All other parameters will be checked by internal functions
    **  This is just a wrapper over them
    */
    ChkArg( f_pcbData != NULL );

    ChkDR( DRM_SST_OpenAndLockSlot( f_pDatastore,
                                    f_eType,
                                    f_pKey1,
                                    f_pKey2,
                                    f_rgbPassword,
                                    DRM_SECURE_STORE_OPEN_EXISTING,
                                    f_pcontextSST,
                                    &cbData ) );
    if ( *f_pcbData < cbData)
    {
        *f_pcbData = cbData;
        dr = DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }

    /*
    **  Read data now
    */
    ChkDR( DRM_SST_GetLockedData( f_pcontextSST,
                                  f_pbData,
                                  f_pcbData ) );

ErrorExit:

    /*
    **  Release lock
    */
    DRM_SST_CloseLockedSlot( f_pcontextSST );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function :   DRM_SST_SetData
**
** Synopsis :
**
** Arguments :
**
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_SetData(
    __in                                     DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in                               const DRM_ID                 *f_pKey1,
    __in_opt                           const DRM_ID                 *f_pKey2,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                f_rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                     eDRM_SECURE_STORE_TYPE  f_eType,
    __in                                     DRM_DST                *f_pDatastore,
    __in_bcount( f_cbData )            const DRM_BYTE               *f_pbData,
    __in                                     DRM_DWORD               f_cbData )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   cbSlot  = f_cbData;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_SetData);

    /*
    **  Parameters will be checked by internal functions
    **  This is just a wrapper over them
    */
    ChkDR( DRM_SST_OpenAndLockSlot( f_pDatastore,
                                    f_eType,
                                    f_pKey1,
                                    f_pKey2,
                                    f_rgbPassword,
                                    DRM_SECURE_STORE_OPEN_ALWAYS,
                                    f_pcontextSST,
                                   &cbSlot ) );
    /*
    **  Set data now
    */
    ChkDR( DRM_SST_SetLockedData( f_pcontextSST,
                                  f_cbData,
                                  f_pbData ) );

ErrorExit:

    /*
    **  Release lock
    */
    DRM_SST_CloseLockedSlot( f_pcontextSST );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;

}

static DRM_NO_INLINE DRM_RESULT _UpdateAttributeTokenInSlot(
    DRM_SECSTORE_CONTEXT *pcontextSST,
    DRM_CONST_STRING     *pdstrAttribute,
    const PERSISTEDTOKEN *pTokenDelta,
    TOKEN                *pTokenAbsolute,
    DRM_DWORD             dwFlags,
    DRM_BOOL              fNewAttribute,
    DRM_BOOL              fDataChangedOnDisk,
    DRMPFNPOLICYCALLBACK  pfnMergeCallback,
    const DRM_VOID       *pvCallbackData )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  dwOffset  = 0;
    DRM_WORD   cbString  = 0;
    TOKEN      token;

    if ( pcontextSST->dwSlotVersion == 0)
    {
        dwOffset    = DRM_SST_SLOT_V0_DATA_OFFSET;
    }
    else
    {
        dwOffset    = DRM_SST_SLOT_DATA_OFFSET;
    }
    while( dwOffset < pcontextSST->cbSlotData )
    {
        DRM_BYT_CopyBytes( &cbString, 0, pcontextSST->rgbSlotData, dwOffset, SIZEOF( cbString ) );
        FIX_ENDIAN_WORD( cbString );
        dwOffset += SIZEOF( cbString );

        if( cbString == CB_DSTR( pdstrAttribute )
         && DRM_BYT_CompareBytes( pcontextSST->rgbSlotData, dwOffset, PB_DSTR( pdstrAttribute ), 0, CB_DSTR( pdstrAttribute ) ) == 0 )
        {
            dwOffset += cbString;

            _PrepareTokenForRead( pcontextSST->rgbSlotData + __CB_DECL( dwOffset ),
                                    &token );

                /* If fNewAttribute is TRUE, then this is a new attribute for our context but someother context has already
                    written it.  Tread here very carfully
                    The valid scenarios for this is relative expiration, or saveddatetime being updated,
                    or perhaps metering.  In the time cases we will do nothing.  In the count
                    cases we can safely add the diff */

            if( (dwFlags & DRM_TOKEN_RESOLVE_BITMASK) == DRM_TOKEN_RESOLVE_LAST_WINS
              || (( dwFlags & DRM_TOKEN_RESOLVE_BITMASK) == DRM_TOKEN_RESOLVE_CALLBACK &&  !fDataChangedOnDisk ) )
            {
                /* The resolve option DRM_TOKEN_RESOLVE_LAST_WINS means we should
                ** overwrite any existing data, even if this is a new attribute.
                ** Also, if the resolve option is for callback but the data on
                ** disk never changed, we overwrite too.
                */

                switch( pTokenDelta->TokenType )
                {
                case TOKEN_LONG:
                    token.val.lValue = pTokenAbsolute->val.lValue + pTokenDelta->val.lValue;
                    break;

                case TOKEN_DATETIME:
                    token.val.u64DateTime = DRM_UI64Add( pTokenAbsolute->val.u64DateTime, pTokenDelta->val.u64DateTime );
                    break;

                case TOKEN_BYTEBLOB:
                    /* Size shouldn't have changed */
                    ChkArg( token.val.byteValue.cbBlob == pTokenAbsolute->val.byteValue.cbBlob );

                    token.val.byteValue.pbBlob = pTokenAbsolute->val.byteValue.pbBlob;
                    break;

                default:
                    ChkDR( DRM_E_CPRMEXP_UPDATE_FAILURE );
                }
            }
            else if( (dwFlags & DRM_TOKEN_RESOLVE_BITMASK) == DRM_TOKEN_RESOLVE_FIRST_WINS )
            {
                /* The resolve option DRM_TOKEN_RESOLVE_FIRST_WINS means we shouldn't
                ** overwrite any existing data if it's changed since we had loaded it */

                switch( pTokenDelta->TokenType )
                {
                case TOKEN_LONG:

                    /* Make sure some other context hasn't changed the value on disk.
                    ** If it has, we shouldn't overwite, we just let it be.
                    */
                    if( token.val.lValue == pTokenAbsolute->val.lValue )
                    {
                        /* Add the delta to the value from the original value */
                        token.val.lValue += pTokenDelta->val.lValue;
                    }
                    break;

                case TOKEN_DATETIME:
                    /* Make sure some other context hasn't changed the value on disk.
                    ** If it has, we shouldn't overwite, we just let it be.
                    */
                    if( DRM_UI64Eql( token.val.u64DateTime, pTokenAbsolute->val.u64DateTime ) )
                    {
                        /* Add the delta to the value from the original value */
                        token.val.u64DateTime = DRM_UI64Add( token.val.u64DateTime, pTokenDelta->val.u64DateTime );
                    }
                    break;

                case TOKEN_BYTEBLOB:
                    /* This should be hit as SetToken ChkArgs this. We can't perform this
                    ** type of resolve on binary data as we don't cache the original value
                    ** to know whether the state on disk has been disturbed or not.
                    **
                    ** Fall through to error
                    */
                default:
                    ChkDR( DRM_E_CPRMEXP_UPDATE_FAILURE );
                }
            }
            else if( (dwFlags & DRM_TOKEN_RESOLVE_BITMASK) == DRM_TOKEN_RESOLVE_DELTA )
            {
                /* The resolve option DRM_TOKEN_RESOLVE_DELTA means we should
                ** add the delta regardless of whether the value on disk has changed,
                ** but we should fail if we're decrementing past zero. */

                switch( pTokenDelta->TokenType )
                {
                case TOKEN_LONG:

                    /* If we decremented past zero, then we need to fail. */
                    if( token.val.lValue >= 0
                     && token.val.lValue < -pTokenDelta->val.lValue )
                    {
                        ChkDR( DRM_E_CPRMEXP_UPDATE_FAILURE );
                    }

                    /* Add the delta to the value from disk */
                    token.val.lValue += pTokenDelta->val.lValue;
                    break;

                case TOKEN_DATETIME:
                    /* Add the delta to the value from disk */
                    token.val.u64DateTime = DRM_UI64Add( token.val.u64DateTime, pTokenDelta->val.u64DateTime );
                    break;

                case TOKEN_BYTEBLOB:
                    /* This should be hit as SetToken ChkArgs this. We can't perform this
                    ** type of resolve on binary data as we don't cache the original value
                    ** to know whether the state on disk has been disturbed or not.
                    **
                    ** Fall through to error
                    */
                default:
                    ChkDR( DRM_E_CPRMEXP_UPDATE_FAILURE );
                }
            }
            else if( (dwFlags & DRM_TOKEN_RESOLVE_BITMASK) == DRM_TOKEN_RESOLVE_CALLBACK )
            {
                DRM_SECURE_STATE_TOKEN_RESOLVE_DATA callbackData = {0};
                TOKEN tokenIntended;
                DRM_DWORD dwID = 0;

                ChkArg( pfnMergeCallback != NULL );

                MEMCPY( &tokenIntended, pTokenAbsolute, SIZEOF(tokenIntended) ); 
                /* Provide the application with all the data it needs to resolve this conflict */
                callbackData.pdstrAttributeName = pdstrAttribute;
                callbackData.fNewAttribute = fNewAttribute;
                callbackData.pOnDisk = &token;
                callbackData.pOriginallyIntendedValue = &tokenIntended;

                /* Need to add the delta for LONG/DATETIME */
                switch( pTokenDelta->TokenType )
                {
                case TOKEN_LONG:
                    tokenIntended.val.lValue = pTokenAbsolute->val.lValue + pTokenDelta->val.lValue;
                    break;

                case TOKEN_DATETIME:
                    tokenIntended.val.u64DateTime = DRM_UI64Add( pTokenAbsolute->val.u64DateTime, pTokenDelta->val.u64DateTime );
                    break;
                }

                /*
                ** Check if the attribute name is greater than 9 characters with a period at the end
                ** (4 characters for each of right ID and restriction ID in hex, and 1 character for a period)
                */
                if( pdstrAttribute->cchString >= 9
                 && *(pdstrAttribute->pwszString + 8) == WCHAR_CAST('.') )
                {
                    /*
                    ** Parse out the 4 hex character right ID and restriction ID
                    */
                    DRM_CHAR rgchID[5] = {0};

                    /* Demote 4 character hex right ID to ANSI and decode hex */
                    DRM_UTL_DemoteUNICODEtoASCII( pdstrAttribute->pwszString, rgchID, 4 );
                    ChkDR( DRMCRT_AtoDWORD( rgchID, NO_OF( rgchID ), 16, &dwID ) );
                    callbackData.wRightID = (DRM_WORD)dwID;

                    /* Demote 4 character hex restriction ID to ANSI and decode hex */
                    DRM_UTL_DemoteUNICODEtoASCII( pdstrAttribute->pwszString + 4, rgchID, 4 );
                    ChkDR( DRMCRT_AtoDWORD( rgchID, NO_OF( rgchID ), 16, &dwID ) );
                    callbackData.wRestrictionID = (DRM_WORD)dwID;
                }

                ChkDR( pfnMergeCallback( &callbackData, DRM_SECURE_STATE_TOKEN_RESOLVE_CALLBACK, pvCallbackData ) );
            }

            _PrepareTokenForWrite( &token, pcontextSST->rgbSlotData + __CB_DECL( dwOffset ) );

            dr = DRM_SUCCESS;
            goto ErrorExit;
        }
        else
        {
            dwOffset += cbString;

            _PrepareTokenForRead( pcontextSST->rgbSlotData + __CB_DECL( dwOffset ), &token );
            dwOffset += SIZEOF( PERSISTEDTOKEN );

            if( token.TokenType == TOKEN_BYTEBLOB )
            {
                dwOffset += token.val.byteValue.cbBlob + (token.val.byteValue.cbBlob % 2);
            }
        }
    }

    /* If we got here then the attribute was not found in the slot.  This is good for new attributes, bad for non-new attributes */
    if( !fNewAttribute )
    {
        ChkDR( DRM_E_CPRMEXP_UPDATE_FAILURE );
    }

    dwOffset                 = pcontextSST->cbSlotData;
    pcontextSST->cbSlotData += SIZEOF( DRM_WORD )
                             + CB_DSTR( pdstrAttribute )
                             + CALC_PERSISTEDTOKEN_FILE_LENGTH( pTokenAbsolute );

    if( pcontextSST->cbSlotData > DRM_SEC_STORE_MAX_SLOT_SIZE )
    {
        ChkDR( DRM_E_SECURESTORE_FULL );
    }

    if( CB_DSTR( pdstrAttribute ) > MAX_UNSIGNED_TYPE( DRM_WORD ) )
    {
        ChkDR( DRM_E_SECURESTORE_FULL );
    }

    cbString = (DRM_WORD) ( CB_DSTR( pdstrAttribute ) & MAX_UNSIGNED_TYPE( DRM_WORD ) );

    FIX_ENDIAN_WORD( cbString );

    DRM_BYT_CopyBytes( pcontextSST->rgbSlotData, dwOffset, &cbString,                 0, SIZEOF( cbString ) );
    dwOffset += SIZEOF( cbString );
    FIX_ENDIAN_WORD( cbString );
    DRM_BYT_CopyBytes( pcontextSST->rgbSlotData, dwOffset, PB_DSTR( pdstrAttribute ), 0, cbString );
    dwOffset += CB_DSTR( pdstrAttribute );

    DRMASSERT( pTokenAbsolute->TokenType == pTokenDelta->TokenType );
    MEMCPY( &token, pTokenAbsolute, SIZEOF(token) ); 

    switch( pTokenDelta->TokenType )
    {
    case TOKEN_LONG:
        token.val.lValue += pTokenDelta->val.lValue;
        break;
    case TOKEN_DATETIME:
        token.val.u64DateTime = DRM_UI64Add( pTokenDelta->val.u64DateTime, pTokenAbsolute->val.u64DateTime );
        break;
    case TOKEN_BYTEBLOB:
        token.val.byteValue.pbBlob = pTokenAbsolute->val.byteValue.pbBlob;
        break;
    default:
        ChkDR( DRM_E_CPRMEXP_UPDATE_FAILURE );
    }

    _PrepareTokenForWrite( &token, pcontextSST->rgbSlotData + __CB_DECL( dwOffset ) );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _ApplyDiffsToStore(
    __in           DRM_SECSTORE_CONTEXT *pcontextSST,
    __inout        DRM_SECSTORE_CONTEXT *pcontextSSTFromDisk,
    __in           DRMPFNPOLICYCALLBACK  pfnMergeCallback,
    __in     const DRM_VOID             *pvCallbackData )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_WORD   iCount = 0;
    TOKEN      token;
    DRM_BOOL   fDataChangedOnDisk = FALSE;

    if( MEMCMP( pcontextSST->rgbSlotData + __CB_DECL( DRM_SST_SLOT_HASH_OFFSET ),
                 pcontextSSTFromDisk->rgbSlotData + __CB_DECL( DRM_SST_SLOT_HASH_OFFSET ),
                 DRM_SHA1_DIGEST_LEN ) != 0 )
    {
        /*
        ** Another instance of DRM wrote updated data to disk between OpenKey and CloseKey
        */
        fDataChangedOnDisk = TRUE;
    }

    for( iCount = 0; iCount < pcontextSST->wNumOriginalAttributes; iCount++ )
    {
        if( (pcontextSST->rgAttributes[iCount].dwFlags & DRM_TOKEN_FLAG_DIRTY) != 0 )
        {
            _PrepareTokenForRead( pcontextSST->rgAttributes[iCount].pTokenValue, &token);

            /* Make sure that if this needs to be resolved with a callback that they did specify one */
            ChkArg( (pcontextSST->rgAttributes[iCount].dwFlags & DRM_TOKEN_RESOLVE_BITMASK) != DRM_TOKEN_RESOLVE_CALLBACK
                 || pfnMergeCallback != NULL );

            ChkDR( _UpdateAttributeTokenInSlot( pcontextSSTFromDisk,
                                                &(pcontextSST->rgAttributes[iCount].dstrAttribute),
                                                &(pcontextSST->rgAttributes[iCount].TokenDelta),
                                                &token,
                                                pcontextSST->rgAttributes[iCount].dwFlags,
                                                FALSE,
                                                fDataChangedOnDisk,
                                                pfnMergeCallback,
                                                pvCallbackData ) );
        }
    }

    for( iCount = pcontextSST->wNumOriginalAttributes; iCount < pcontextSST->wNumAttributes; iCount++ )
    {
        if( (pcontextSST->rgAttributes[iCount].dwFlags & DRM_TOKEN_FLAG_DIRTY) != 0 )
        {
            _PrepareTokenForRead( pcontextSST->rgAttributes[iCount].pTokenValue, &token);

            /* Make sure that if this needs to be resolved with a callback that they did specify one */
            ChkArg( (pcontextSST->rgAttributes[iCount].dwFlags & DRM_TOKEN_RESOLVE_BITMASK) != DRM_TOKEN_RESOLVE_CALLBACK
                 || pfnMergeCallback != NULL );

            ChkDR( _UpdateAttributeTokenInSlot( pcontextSSTFromDisk,
                                                &(pcontextSST->rgAttributes[iCount].dstrAttribute),
                                                &(pcontextSST->rgAttributes[iCount].TokenDelta),
                                                &token,
                                                pcontextSST->rgAttributes[iCount].dwFlags,
                                                TRUE,
                                                fDataChangedOnDisk,
                                                pfnMergeCallback,
                                                pvCallbackData ) );
        }
    }

ErrorExit:
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_CloseKey(
    __in           DRM_SECSTORE_CONTEXT *pcontextSST,
    __in           DRM_DST              *pDatastore,
    __in_opt       DRMPFNPOLICYCALLBACK  pfnMergeCallback,
    __in_opt const DRM_VOID             *pvCallbackData )
{
    DRM_RESULT           dr              = DRM_SUCCESS;
    DRM_DWORD            cbWritten       = 0;
    DRM_DWORD            dwSlotMetaData  = 0;
    DRM_BOOL             fCloseStore     = FALSE;
    DRM_SECSTORE_CONTEXT contextSST      = { 0 };

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_CloseKey);

    ChkArg (pcontextSST          != NULL
         && pDatastore          != NULL
         && pcontextSST->fInited != FALSE);

    if( !pcontextSST->fOpened
     || !pcontextSST->fDirty
     ||  pcontextSST->fNoPassword )
    {
        /* They didn't touch anything in the store, so we're done.
        ** Return DRM_S_FALSE to notify callers that nothing changed
        ** in the secure store.
        */
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    MEMCPY( &contextSST.rgbKey1, &pcontextSST->rgbKey1, SIZEOF( contextSST.rgbKey1 ) );
    MEMCPY( &contextSST.rgbKey2, &pcontextSST->rgbKey2, SIZEOF( contextSST.rgbKey2 ) );
    MEMCPY( &contextSST.slotHint, &pcontextSST->slotHint, SIZEOF( contextSST.slotHint ) );
    contextSST.pNamespaceId = pcontextSST->pNamespaceId;

    ChkDR( _OpenAndVerifySlot( &contextSST, pDatastore, pcontextSST->rgbPassword, pcontextSST->eType, FALSE, DRM_SECURE_STORE_OPEN_EXISTING, 0 ) );
    fCloseStore = TRUE;

    /* The slot hash of the now opened slot is valid.  Go about the process of applying diff's to the
       attributes in the slot */
    ChkDR( _ApplyDiffsToStore( pcontextSST, &contextSST, pfnMergeCallback, pvCallbackData ) );

    /* Update the size in the slot meta data */
    if ( contextSST.dwSlotVersion == 0 )
    {
        /*
        **  We must update the format of this slot before writing it to disc
        */

        /*  Set the metadata first  */
        BYTES_TO_DWORD( dwSlotMetaData, contextSST.rgbSlotData );
        DRM_SST_SET_SLOT_METADATA( dwSlotMetaData );
        DWORD_TO_BYTES( contextSST.rgbSlotData, dwSlotMetaData );

        /*  Move the rest of the data down by a DWORD       */
        /*  We have enough space in the bufer to do this    */
        DRM_BYT_MoveBytes( contextSST.rgbSlotData,
                           DRM_SST_SLOT_DATA_OFFSET,
                           contextSST.rgbSlotData,
                           DRM_SST_SLOT_V0_DATA_OFFSET,
                           contextSST.cbSlotData - DRM_SST_SLOT_V0_HEADER_SIZE );

        /*  Increment the slotsize by a DWORD   */
        contextSST.cbSlotData += DRM_SST_SLOT_SIZEDATA_SIZE;

        ChkOverflow( contextSST.cbSlotData, DRM_SST_SLOT_SIZEDATA_SIZE );
    }

    /*  Update the size data in the buffer  */
    DWORD_TO_BYTES( contextSST.rgbSlotData + __CB_DECL( DRM_SST_SLOT_METADATA_SIZE ),
                    contextSST.cbSlotData );

    ChkDR( DRM_HMAC_CreateMAC( eDRM_SHA_1,
                               pcontextSST->rgbPassword,
                               DRM_SHA1_DIGEST_LEN,
                               contextSST.rgbSlotData,
                               DRM_SST_SLOT_DATA_OFFSET,
                               contextSST.cbSlotData - DRM_SST_SLOT_HEADER_SIZE,
                               contextSST.rgbSlotData,
                               DRM_SST_SLOT_HASH_OFFSET,
                               DRM_SHA1_DIGEST_LEN ) );

    /* Write the new hash back to disk */
    if( contextSST.cbSlot < contextSST.cbSlotData )
    {
        ChkDR( DRM_DST_SlotResize( pDatastore, &contextSST.oSlotContext, contextSST.cbSlotData ) );
    }
    ChkDR( DRM_DST_SlotSeek( pDatastore, &contextSST.oSlotContext, 0, eDRM_DST_SEEKSET, NULL ) );
    ChkDR( DRM_DST_SlotWrite( pDatastore, &contextSST.oSlotContext, contextSST.cbSlotData, contextSST.rgbSlotData, &cbWritten ) );

#if !_DATASTORE_WRITE_THRU
    ChkDR( DRM_DST_CommitNamespace( pDatastore, &contextSST.oNsContext ) );
#endif

ErrorExit:
    if( pcontextSST != NULL )
    {
        ZEROMEM( &pcontextSST->slotHint, SIZEOF( DRM_DST_SLOT_HINT ) );
    }

    if( fCloseStore )
    {
        DRM_DST_CloseSlot( pDatastore, &contextSST.oSlotContext );
        DRM_DST_CloseNamespace( pDatastore, &contextSST.oNsContext   );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_GetTokenValue(
    __in       DRM_SECSTORE_CONTEXT *pcontextSST,
    __in const DRM_CONST_STRING     *pdstrAttribute,
    __out      TOKEN                *pToken )
{
    DRM_RESULT                  dr       = DRM_SUCCESS; /* Return DRM_S_FALSE to indicate that it couldn't be found */

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_GetTokenValue );

    ChkArg (pcontextSST          != NULL
         && pcontextSST->fInited != FALSE
         && pToken               != NULL);

    if( !pcontextSST->fOpened )
    {
        /* This is the first time they're touching data, so need to load it */
        ChkDR( _LoadKeyTokens( pcontextSST, pcontextSST->pDatastore ) );
    }

    ChkDR( DRM_SST_GetTokenValueFromCache( pdstrAttribute,
                                           pcontextSST->rgAttributes,
                                           pcontextSST->wNumAttributes,
                                           pToken) );
ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_GetTokenValueFromCache(
    __in const DRM_CONST_STRING     *pdstrAttribute,
    __in       CachedAttribute       rgAttributes[DRM_MAX_ATTRIBUTES_PER_SST_KEY],
    __in       DRM_WORD              wNumAttributes,
    __out      TOKEN                *pToken )
{
    DRM_RESULT                  dr       = DRM_SUCCESS; /* Return DRM_S_FALSE to indicate that it couldn't be found */
    DRM_DWORD                   dwCount  = 0;

    ChkArg ( pToken != NULL && wNumAttributes < DRM_MAX_ATTRIBUTES_PER_SST_KEY );
    ChkDRMString( pdstrAttribute );

    for( dwCount = 0; dwCount < wNumAttributes; dwCount++ )
    {
        if( pdstrAttribute->cchString == rgAttributes[dwCount].dstrAttribute.cchString
         && 0 == MEMCMP( PB_DSTR( pdstrAttribute ), PB_DSTR( &rgAttributes[dwCount].dstrAttribute ), CB_DSTR( pdstrAttribute ) ) )
        {

            /* We matched the string value. Return the token value.*/
            _PrepareTokenForRead( rgAttributes[dwCount].pTokenValue, pToken );

            /* Update the result token with the delta data */
            switch( pToken->TokenType )
            {
            case TOKEN_LONG:
                /* NewDiff = NewValue - CachedValue */
                pToken->val.lValue += rgAttributes[dwCount].TokenDelta.val.lValue;
                break;
            case TOKEN_DATETIME:
                pToken->val.u64DateTime = DRM_UI64Add( pToken->val.u64DateTime, rgAttributes[dwCount].TokenDelta.val.u64DateTime );
                break;
            }

            dr = DRM_SUCCESS; /* DRM_SUCCESS indicates that the value was found */
            goto ErrorExit;
        }
    }

    /* Value wasn't found */
    dr = DRM_S_FALSE;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_SetTokenValue(
    __in       DRM_SECSTORE_CONTEXT *pcontextSST,
    __in const DRM_CONST_STRING     *pdstrAttribute,
    __in const TOKEN                *pToken )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwFlags  = 0;

    switch( pToken->TokenType )
    {
    case TOKEN_LONG:
        dwFlags = DRM_TOKEN_RESOLVE_DELTA;
        break;

    case TOKEN_DATETIME:
        dwFlags = DRM_TOKEN_RESOLVE_FIRST_WINS;
        break;
    }

    dr = DRM_SST_SetExplicitResolutionTokenValue( pcontextSST, pdstrAttribute, pToken, dwFlags );

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_SetExplicitResolutionTokenValue(
    __in       DRM_SECSTORE_CONTEXT *pcontextSST,
    __in const DRM_CONST_STRING     *pdstrAttribute,
    __in const TOKEN                *pToken,
    __in       DRM_DWORD             dwFlags )
{
    DRM_RESULT dr       = DRM_S_FALSE; /* Return DRM_S_FALSE to indicate that it couldn't be found */
    DRM_DWORD  dwCount  = 0;
    DRM_WORD   cbString = 0;
    TOKEN      token;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_SetExplicitResolutionTokenValue );

    ChkArg (pcontextSST          != NULL
        &&  pToken               != NULL
        &&  pcontextSST->fInited != FALSE
        &&  (pToken->TokenType   == TOKEN_LONG
          || pToken->TokenType   == TOKEN_DATETIME
          || pToken->TokenType   == TOKEN_BYTEBLOB));

    ChkDRMString( pdstrAttribute );

    /* make sure the slot is not opened without password */
    ChkBOOL(!pcontextSST->fNoPassword, DRM_E_INVALID_SECURESTORE_PASSWORD);

    switch( dwFlags & DRM_TOKEN_RESOLVE_BITMASK )
    {
        case DRM_TOKEN_RESOLVE_DELTA:
        case DRM_TOKEN_RESOLVE_FIRST_WINS:
            ChkArg( pToken->TokenType == TOKEN_LONG
                 || pToken->TokenType == TOKEN_DATETIME ); /* Only long or datetime types are allowed for delta resolves */
            break;

        case DRM_TOKEN_RESOLVE_LAST_WINS:
        case DRM_TOKEN_RESOLVE_CALLBACK:
            break;
        default:
            ChkArg( FALSE ); /* Unknown token resolve type */
    }

    if( !pcontextSST->fOpened )
    {
        /* This is the first time they're touching data, so need to load it */
        ChkDR( _LoadKeyTokens( pcontextSST, pcontextSST->pDatastore ) );
    }

    /* Set the dirty flag to indicate it needs to be written to disk */
    pcontextSST->fDirty = TRUE;

    /* Search to see if the token already exists */
    for( dwCount = 0; dwCount < pcontextSST->wNumAttributes; dwCount++ )
    {
        /* Compare the attribute name to see if it matches */
        if( pdstrAttribute->cchString == pcontextSST->rgAttributes[dwCount].dstrAttribute.cchString
         && 0 == MEMCMP( PB_DSTR( pdstrAttribute ), PB_DSTR( &pcontextSST->rgAttributes[dwCount].dstrAttribute ), CB_DSTR( pdstrAttribute ) ) )
        {
            /* We matched the string value. Update the delta.*/
            _PrepareTokenForRead( pcontextSST->rgAttributes[dwCount].pTokenValue, &token );

            if( pToken->TokenType != token.TokenType )
            {
                /* You can't change the token type!! */
                ChkDR( DRM_E_WRONG_TOKEN_TYPE );
            }

            pcontextSST->rgAttributes[dwCount].dwFlags = dwFlags | DRM_TOKEN_FLAG_DIRTY;
            pcontextSST->rgAttributes[dwCount].TokenDelta.TokenType = token.TokenType;
            switch( token.TokenType )
            {
                case TOKEN_LONG:
                    /* We're storing only the delta */
                    /* NewDiff = NewValue - CachedValue */
                    pcontextSST->rgAttributes[dwCount].TokenDelta.val.lValue = pToken->val.lValue - token.val.lValue;
                    break;
                case TOKEN_DATETIME:
                    /* We're storing only the delta */
                    /* NewDiff = NewValue - CachedValue */
                    pcontextSST->rgAttributes[dwCount].TokenDelta.val.u64DateTime = DRM_UI64Sub(pToken->val.u64DateTime, token.val.u64DateTime);
                    break;
                case TOKEN_BYTEBLOB:
                    /* Can't change the size */
                    ChkArg( pToken->val.byteValue.cbBlob == token.val.byteValue.cbBlob );

                    MEMCPY( (DRM_BYTE*)pcontextSST->rgAttributes[dwCount].pTokenValue + __CB_DECL( SIZEOF( PERSISTEDTOKEN ) ),
                                    pToken->val.byteValue.pbBlob,
                                    pToken->val.byteValue.cbBlob );
                    break;
                default:
                    DRMASSERT( FALSE );
            }

            dr = DRM_SUCCESS;
            goto ErrorExit;
        }
    }

    /* Attribute doesn't exist.  Try to create a new one.  Be weary of running out of slot space, as well as
       cached attribute structure space */
    if( pcontextSST->wNumAttributes == DRM_MAX_ATTRIBUTES_PER_SST_KEY )
    {
        ChkDR( DRM_E_SECURESTORE_FULL );
    }
    if( pcontextSST->cbSlotData                                                                                                   > DRM_SEC_STORE_MAX_SLOT_SIZE
     || pcontextSST->cbSlotData + SIZEOF( DRM_WORD )                                                                              > DRM_SEC_STORE_MAX_SLOT_SIZE
     || pcontextSST->cbSlotData + SIZEOF( DRM_WORD ) + CB_DSTR( pdstrAttribute )                                                  > DRM_SEC_STORE_MAX_SLOT_SIZE
     || pcontextSST->cbSlotData + SIZEOF( DRM_WORD ) + CB_DSTR( pdstrAttribute ) + CALC_PERSISTEDTOKEN_FILE_LENGTH( pToken )      > DRM_SEC_STORE_MAX_SLOT_SIZE )
    {
        /* Not enough room in the slot */
        ChkDR( DRM_E_SECURESTORE_FULL );
    }

    /* Fill in the data in the slot buffer and the cache attribute array */
    ZEROMEM(       &pcontextSST->rgAttributes[pcontextSST->wNumAttributes],
            SIZEOF( pcontextSST->rgAttributes[pcontextSST->wNumAttributes] ) );

    if( CB_DSTR( pdstrAttribute ) > MAX_UNSIGNED_TYPE( DRM_WORD ) )
    {
        ChkDR( DRM_E_SECURESTORE_FULL );
    }

    cbString = (DRM_WORD) CB_DSTR( pdstrAttribute );
    FIX_ENDIAN_WORD( cbString );
    MEMCPY( &(pcontextSST->rgbSlotData[__CB_DECL(pcontextSST->cbSlotData)]), &cbString, SIZEOF( cbString ) );
    pcontextSST->cbSlotData += SIZEOF( cbString );
    FIX_ENDIAN_WORD( cbString );

    pcontextSST->rgAttributes[pcontextSST->wNumAttributes].dstrAttribute.cchString  = pdstrAttribute->cchString;
    pcontextSST->rgAttributes[pcontextSST->wNumAttributes].dstrAttribute.pwszString = (const DRM_WCHAR*) &(pcontextSST->rgbSlotData[__CB_DECL(pcontextSST->cbSlotData)]);
    MEMCPY( &(pcontextSST->rgbSlotData[__CB_DECL(pcontextSST->cbSlotData)]), pdstrAttribute->pwszString, CB_DSTR( pdstrAttribute ) );
    pcontextSST->cbSlotData += CB_DSTR( pdstrAttribute );

    if( pToken->TokenType == TOKEN_BYTEBLOB )
    {
        /* We're storing an absolute value, not a delta */
        MEMCPY( &token, pToken, SIZEOF(token) ); 
        ZEROMEM( &(pcontextSST->rgAttributes[dwCount].TokenDelta), SIZEOF( PERSISTEDTOKEN ) );
    }
    else
    {
        /* We're storing a delta from 0 since the token doesn't exist */
        _Token_To_PersistedToken( pToken, &(pcontextSST->rgAttributes[dwCount].TokenDelta));
        ZEROMEM(&token, SIZEOF( TOKEN ) );
        token.TokenType = pToken->TokenType;
    }
    pcontextSST->rgAttributes[dwCount].TokenDelta.TokenType = pToken->TokenType;
    pcontextSST->rgAttributes[dwCount].dwFlags = dwFlags | DRM_TOKEN_FLAG_DIRTY;

    pcontextSST->rgAttributes[pcontextSST->wNumAttributes].pTokenValue = &(pcontextSST->rgbSlotData[__CB_DECL(pcontextSST->cbSlotData)]);
    _PrepareTokenForWrite( &token, &(pcontextSST->rgbSlotData[__CB_DECL(pcontextSST->cbSlotData)]) );
    pcontextSST->cbSlotData += CALC_PERSISTEDTOKEN_FILE_LENGTH( pToken );

    pcontextSST->wNumAttributes++;

    dr = DRM_SUCCESS;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function: DRM_SST_DeleteKey
**
** Synopsis: Delete an entry from the secure store
**
** Arguments:
**           [f_pcontextSST] -- secure store context, need not be initialized
**           [f_typeSST]     -- one of the legal enums corresponding to the
**                              layer to open
**           [f_pid1]
**           [f_pid2]        -- major and minor "keys" of the data store to
**                              delete
**           [f_pDatastore] -- initialized DST context
**
** Notes:    Indiscriminate deletion of keys could open up the system for
**           replay attacks
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_DeleteKey(
    __in       DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in       eDRM_SECURE_STORE_TYPE  f_typeSST,
    __in const DRM_ID                 *f_pid1,
    __in const DRM_ID                 *f_pid2,
    __in       DRM_DST                *f_pDatastore)
{
    DRM_RESULT  dr  = DRM_SUCCESS;
    DRM_DST_KEY id2 = {0};

    ChkArg(f_pcontextSST != NULL
        && f_pDatastore != NULL
        && f_pid1        != NULL);

    /* Check for supported flags */

    ChkArg( _IsValidSecureStoreType( f_typeSST, DISALLOW_CACHEDCERT ) );

    f_pcontextSST->pNamespaceId = _MapSecureStoreTypeToNamespace( f_typeSST );

    if( f_pid2 != NULL )
    {
        MEMCPY( &id2, f_pid2, SIZEOF( id2 ) );
    }
    ChkDR( DRM_DST_OpenNamespace( f_pDatastore,
                                  f_pcontextSST->pNamespaceId,
                                  (DRM_DST_OPENMODE)(eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT),
                                  0,
                                  0,
                                  &f_pcontextSST->oNsContext ) );

    ChkDR( DRM_DST_DeleteSlot( f_pDatastore,
                               &f_pcontextSST->oNsContext,
                               f_pid1,
                               &id2,
                               NULL,
                               TRUE ) );

ErrorExit:
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_GetAllData(
    __in                         DRM_SECSTORE_CONTEXT *pcontextSST,
    __out_bcount_opt( *pcbData ) DRM_BYTE             *pbData,
    __inout                      DRM_DWORD            *pcbData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbRequired = 0;
    DRM_DWORD   cbHeader    = 0;

    ChkArg (pcontextSST          != NULL
        &&  pcbData              != NULL
        &&  pcontextSST->fInited != FALSE);

    if( !pcontextSST->fOpened )
    {
        /* This is the first time they're touching data, so need to load it */
        ChkDR( _LoadKeyTokens( pcontextSST, pcontextSST->pDatastore ) );
    }

    cbHeader = ( pcontextSST->dwSlotVersion == 0 )
             ? DRM_SST_SLOT_V0_HEADER_SIZE
             : DRM_SST_SLOT_HEADER_SIZE;

    ChkBOOL( pcontextSST->cbSlotData >= cbHeader, DRM_E_SECURESTORE_CORRUPT );
    cbRequired = pcontextSST->cbSlotData - cbHeader;

    if (  pbData == NULL
      || *pcbData < cbRequired)
    {
        *pcbData = cbRequired;
        dr = DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }

    MEMCPY (pbData, pcontextSST->rgbSlotData + __CB_DECL( cbHeader ), cbRequired);

    *pcbData = cbRequired;
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_OpenEnumerator(
    __in           eDRM_SECURE_STORE_TYPE    eType,
    __in_opt const DRM_ID                   *pKey1,
    __out          DRM_SECSTOREENUM_CONTEXT *pcontextSSTEnum,
    __in           DRM_DST                  *pDatastore,
    __in           DRM_BOOL                  fExclusiveLock )
{
    DRM_RESULT dr=DRM_SUCCESS;
    const DRM_DST_NAMESPACE *pNamespaceId = NULL;
    DRM_DWORD  dwLockMode = eDRM_DST_LOCKWAIT;

    ChkArg( pcontextSSTEnum && pDatastore );
    if ( fExclusiveLock )
    {
        dwLockMode |= eDRM_DST_LOCKEXCLUSIVE;
    }
    else
    {
        dwLockMode |= eDRM_DST_LOCKSHARED;
    }

    /* Check for supported flags */
    if (!_IsValidSecureStoreType( eType, DISALLOW_CACHEDCERT ) )
    {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    ZEROMEM(pcontextSSTEnum, SIZEOF(pcontextSSTEnum));

    pNamespaceId = _MapSecureStoreTypeToNamespace( eType );

    dr = DRM_DST_OpenNamespace( pDatastore,
                                pNamespaceId,
                                (DRM_DST_OPENMODE)(eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT),
                                0,
                                0,
                                &pcontextSSTEnum->oNsContext );
    if ( dr == DRM_E_DST_NAMESPACE_NOT_FOUND )
    {
        dr = DRM_SUCCESS;
        pcontextSSTEnum->fInited = TRUE;
        pcontextSSTEnum->eMode   = eSSTEnumNone;
        goto ErrorExit;
    }
    ChkDR( dr );

    dr =  DRM_DST_InitSlotEnum( pDatastore,
                                &pcontextSSTEnum->oNsContext,
                                pKey1,
                                ( DRM_DST_LOCKMODE )dwLockMode,
                                &pcontextSSTEnum->oDstEnumContext);
    if( dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        /* No Slots found. */
        pcontextSSTEnum->eMode = eSSTEnumNone;
        dr = DRM_SUCCESS;
    }
    else if( DRM_FAILED( dr ) )
    {
        goto ErrorExit;
    }
    else if ( pKey1 )
    {
        pcontextSSTEnum->eMode = eSSTEnumFiltered;
    }
    else
    {
        pcontextSSTEnum->eMode = eSSTEnumNatural;
    }
    pcontextSSTEnum->pDatastore   = pDatastore;
    pcontextSSTEnum->fCurrIsValid = FALSE;
    pcontextSSTEnum->fInited      = TRUE;
    pcontextSSTEnum->eType        = eType;

ErrorExit:
    return dr;
}


/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_EnumNext(
    __in                DRM_SECSTOREENUM_CONTEXT *pcontextSSTEnum,
    __out_ecount_opt(1) DRM_ID                   *pKey1,
    __out_ecount_opt(1) DRM_ID                   *pKey2,
    __out_ecount(1)     DRM_DWORD                *pcbData )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD  cbSlot = 0;
    DRM_DWORD  cbRead           = 0;
    DRM_DWORD  dwSlotMetaData   = 0;

    ChkArg (pcontextSSTEnum           != NULL
         && pcontextSSTEnum->fInited  != FALSE
         && pcbData         != NULL);

    if ( pcontextSSTEnum->eMode == eSSTEnumNone )
    {
        dr = DRM_E_NOMORE;
        goto ErrorExit;
    }

    *pcbData = 0;

    dr = DRM_DST_SlotEnumNext( pcontextSSTEnum->pDatastore,
                               &pcontextSSTEnum->oDstEnumContext,
                               &pcontextSSTEnum->oSlotContext,
                               &pcontextSSTEnum->oCurrKID,
                               &pcontextSSTEnum->oCurrLID,
                               &cbSlot );
    if ( dr == DRM_E_NOMORE )
    {
        goto ErrorExit;
    }
    else if ( dr == DRM_E_DST_BLOCK_MISMATCH
           || dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        ChkDR(DRM_E_SECURESTORE_CORRUPT);
    }
    ChkDR(dr);

    if( pKey1 != NULL )
    {
        MEMCPY(pKey1->rgb, pcontextSSTEnum->oCurrKID.rgb, DRM_ID_SIZE);
    }
    if( pKey2 != NULL )
    {
        MEMCPY(pKey2->rgb, pcontextSSTEnum->oCurrLID.rgb, DRM_ID_SIZE);
    }

    pcontextSSTEnum->fCurrIsValid = TRUE;

    /*
    **  Figure out the slot version
    */
    ChkDR( DRM_DST_SlotRead( pcontextSSTEnum->pDatastore,
                             &pcontextSSTEnum->oSlotContext,
                             DRM_SST_SLOT_METADATA_SIZE,
                             (DRM_BYTE *)&dwSlotMetaData,
                             &cbRead ) );
    if( cbRead != DRM_SST_SLOT_METADATA_SIZE )
    {
        dr = DRM_E_FILE_READ_ERROR;
        goto ErrorExit;
    }
    FIX_ENDIAN_DWORD( dwSlotMetaData );

    /*
    **  Read the slot size
    */
    ChkDR( DRM_DST_SlotRead( pcontextSSTEnum->pDatastore,
                             &pcontextSSTEnum->oSlotContext,
                             DRM_SST_SLOT_SIZEDATA_SIZE,
                             (DRM_BYTE *)pcbData,
                             &cbRead ) );
    if( cbRead != DRM_SST_SLOT_SIZEDATA_SIZE )
    {
        *pcbData = 0;
        dr = DRM_E_FILE_READ_ERROR;
        goto ErrorExit;
    }
    FIX_ENDIAN_DWORD( *pcbData );

    *pcbData = ( ( dwSlotMetaData & DRM_SST_SLOT_VERSION_MASK ) == 0 )
             ? *pcbData - DRM_SST_SLOT_V0_HEADER_SIZE
             : *pcbData - DRM_SST_SLOT_HEADER_SIZE;
    ChkDR( DRM_DST_CloseSlot( pcontextSSTEnum->pDatastore, &pcontextSSTEnum->oSlotContext ) );

ErrorExit:
    return dr;
}

/******************************************************************************
** Function :   DRM_SST_EnumDeleteCurrent
** Synopsis :   Delete the current enumerated slot
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_SST_EnumDeleteCurrent(
    __in       DRM_SECSTOREENUM_CONTEXT *pcontextSSTEnum,
    __inout    DRM_SECSTORE_CONTEXT     *pcontextSST )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_DWORD  cbSlot           = 0;

    ChkArg (pcontextSSTEnum          != NULL
         && pcontextSSTEnum->fInited != FALSE );

    ChkBOOL( pcontextSSTEnum->eMode != eSSTEnumNone, DRM_E_NOMORE);

    dr = DRM_DST_SlotEnumReloadCurrent( pcontextSSTEnum->pDatastore,
                                        &pcontextSSTEnum->oDstEnumContext,
                                        &pcontextSST->oSlotContext,
                                        &pcontextSSTEnum->oCurrKID,
                                        &pcontextSSTEnum->oCurrLID,
                                        &cbSlot );
    ChkBOOL(dr != DRM_E_NOMORE, DRM_E_NOMORE);
    ChkBOOL((dr != DRM_E_DST_BLOCK_MISMATCH && dr != DRM_E_DST_SLOT_NOT_FOUND), DRM_E_SECURESTORE_CORRUPT);
    ChkDR(dr);

    dr = DRM_DST_SlotEnumDeleteCurrent( pcontextSSTEnum->pDatastore,
                                        &pcontextSSTEnum->oDstEnumContext,
                                        &pcontextSST->oSlotContext );
    DRM_DST_CloseSlot( pcontextSSTEnum->pDatastore, &pcontextSST->oSlotContext );
    pcontextSSTEnum->fCurrIsValid = FALSE;
    pcontextSST->fInited = FALSE;
    pcontextSST->fOpened = FALSE;

ErrorExit:
    return dr;
}


/******************************************************************************
** Function :   DRM_SST_EnumLoadCurrent
** Synopsis :   Load the current slot in memory
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_EnumLoadCurrent(
    __in                                     DRM_SECSTOREENUM_CONTEXT *pcontextSSTEnum,
    __inout                                  DRM_SECSTORE_CONTEXT     *pcontextSST,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                  rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __out                                    DRM_ID                   *pKey1,
    __out_opt                                DRM_ID                   *pKey2,
    __inout                                  DRM_DWORD                *pcbData )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_DWORD  cbSlot           = 0;
    DRM_DWORD  cbRead           = 0;
    DRM_DWORD  dwSlotMetaData   = 0;
    DRM_DWORD  cbData           = 0;

    ChkArg (pcontextSSTEnum          != NULL
         && pcontextSSTEnum->fInited != FALSE
         && pcbData                  != NULL);


    ChkBOOL( pcontextSSTEnum->eMode != eSSTEnumNone, DRM_E_NOMORE);

    dr = DRM_DST_SlotEnumReloadCurrent( pcontextSSTEnum->pDatastore,
                                        &pcontextSSTEnum->oDstEnumContext,
                                        &pcontextSST->oSlotContext,
                                        &pcontextSSTEnum->oCurrKID,
                                        &pcontextSSTEnum->oCurrLID,
                                        &cbSlot );
    ChkBOOL(dr != DRM_E_NOMORE, DRM_E_NOMORE);
    ChkBOOL((dr != DRM_E_DST_BLOCK_MISMATCH && dr != DRM_E_DST_SLOT_NOT_FOUND), DRM_E_SECURESTORE_CORRUPT);
    ChkDR(dr);

    if( pKey1 != NULL )
    {
        MEMCPY(pKey1->rgb, pcontextSSTEnum->oCurrKID.rgb, DRM_ID_SIZE);
    }

    if( pKey2 != NULL )
    {
        MEMCPY(pKey2->rgb, pcontextSSTEnum->oCurrLID.rgb, DRM_ID_SIZE);
    }
    pcontextSSTEnum->fCurrIsValid = TRUE;

    /*
    **  Figure out the slot version
    */
    ChkDR( DRM_DST_SlotRead( pcontextSSTEnum->pDatastore,
                             &pcontextSST->oSlotContext,
                             DRM_SST_SLOT_METADATA_SIZE,
                             ( DRM_BYTE * )&dwSlotMetaData,
                             &cbRead ) );
    if ( cbRead != DRM_SST_SLOT_METADATA_SIZE )
    {
        dr = DRM_E_FILE_READ_ERROR;
        goto ErrorExit;
    }
    FIX_ENDIAN_DWORD( dwSlotMetaData );

    cbData = ( ( dwSlotMetaData & DRM_SST_SLOT_VERSION_MASK ) == 0 )
             ? cbSlot - DRM_SST_SLOT_V0_HEADER_SIZE
             : cbSlot - DRM_SST_SLOT_HEADER_SIZE;

    /* read the slot data */
    pcontextSST->cbSlot = cbData;

    DRM_DST_SlotSeek( pcontextSSTEnum->pDatastore, &pcontextSST->oSlotContext, 0, eDRM_DST_SEEKSET, NULL);
    dr = _LoadSlot(pcontextSST, FALSE, pcontextSSTEnum->eType, rgbPassword);
    if ( DRM_SUCCEEDED(dr) )
    {
        *pcbData = cbData;
    }
    DRM_DST_CloseSlot( pcontextSSTEnum->pDatastore, &pcontextSST->oSlotContext );
    pcontextSST->fInited = TRUE;
    pcontextSST->fOpened = TRUE;

ErrorExit:
    return dr;
}
/*****************************************************************************
** Function: DRM_SST_CreateGlobalStorePassword
**
** Synopsis: use the keyeed to create the hash used as the secure store password
**           for the global store
**
** Arguments:
**           [f_rgbSeed]     -- buffer to hold keyseed
**           [f_rgbPassword] -- buffer to hold Password
**           [f_pcontextBBX] -- initialized BBX context
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_CreateGlobalStorePassword(
    __in_opt DRM_VOID *f_pOEMContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE  f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                DRM_BYTE *f_pbContextBBX )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   rgbSeed [__CB_DECL(DRM_SHA1_DIGEST_LEN)];

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_CreateGlobalStorePassword);

    ChkDR(Oem_Device_GetSecureStoreGlobalPasswordSeed(f_pOEMContext, rgbSeed));

    ChkDR(DRM_BBX_HashValue(rgbSeed,
                            DRM_SHA1_DIGEST_LEN-1,
                            f_rgbPasswordSST,
                            (DRM_BB_CONTEXT *) f_pbContextBBX));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*****************************************************************************
** Function: DRM_SST_CreateLicenseStatePassword
**
** Synopsis: use the LID to create the hash used as the secure store password
**           for this license
**
** Arguments:
**           [f_rgbLID]         -- the LID
**           [f_rgbPasswordSST] -- buffer to receive the password
**           [f_pcontextBBX]    -- initialized BBX context
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SST_CreateLicenseStatePassword(
    __in                                const DRM_LID        *f_plid,
    __out_bcount( DRM_SHA1_DIGEST_LEN )       DRM_BYTE        f_rgbPasswordLST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                      DRM_BB_CONTEXT *f_pContextBBX )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMSECSTORE, PERF_FUNC_DRM_SST_CreateLicenseStatePassword);

    ChkDR(DRM_BBX_HashValue((DRM_BYTE*) f_plid,
                            SIZEOF (DRM_LID),
                            f_rgbPasswordLST,
                            f_pContextBBX));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
