/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmhdsblockheadercache.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_HBHC_IsHdsBlockHeaderCacheSupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL DRM_HBHC_IsHdsBlockHeaderCacheUnsupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HBHC_GetBlockHeaderFromCache(
    __in     _NsContext     *f_pNS,
    __in     DRM_DWORD       f_nBlkNum,
    __inout  _CommBlockHDR **f_ppBlock,
    __in     DRM_BOOL        f_fGenericHeaderOnly,
    __out    DRM_BOOL       *f_pfFound ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HBHC_GetBlockHeaderFromCache(
    __in     _NsContext     *f_pNS,
    __in     DRM_DWORD       f_nBlkNum,
    __inout  _CommBlockHDR **f_ppBlock,
    __in     DRM_BOOL        f_fGenericHeaderOnly,
    __out    DRM_BOOL       *f_pfFound )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pfFound != NULL );

    if( f_fGenericHeaderOnly )
    {
        /* They only want the generic header, so we don't need to lookup the block type */
        dr = Oem_Hds_CheckBlockHeaderCache( f_nBlkNum,
                        f_pNS->pHDS->fp,
                        NULL,
                        (*f_ppBlock)->File.bFileImage,
                        SIZEOF( _CommBlockFileHDR ) );
        if( DRM_SUCCEEDED( dr ) )
        {
            /* Found it, was in the cache. We're done. */
            *f_pfFound = TRUE;
            goto ErrorExit;
        }
        else if( dr == DRM_E_DST_BLOCK_CACHE_MISS )
        {
            /* Bad luck, it's not in the cache. We'll just have to load it from disk */
            dr = DRM_SUCCESS;
        }
        ChkDR( dr );
    }
    else
    {
        DRM_DWORD dwBlockHeaderMetadata = 0;

        /* Need the block type before we can try loading the block itself */
        dr = Oem_Hds_CheckBlockHeaderCache( f_nBlkNum,
                        f_pNS->pHDS->fp,
                        &dwBlockHeaderMetadata,
                        NULL,
                        0 );

        if( dr == DRM_E_DST_BLOCK_CACHE_MISS )
        {
            /* Bad luck, it's not in the cache. We'll just have to load it from disk */
            ChkDR( DRM_SUCCESS );
        }
        else if( DRM_SUCCEEDED(dr) )
        {
            /* Sweet, we got a hit in the cache. */
            _EBlockType eBlockType;
            DRM_DWORD cbCachedBlockHeaderSize = 0;

            /* The least significant byte holds the block type */
            eBlockType = (_EBlockType)(dwBlockHeaderMetadata & 0xFF);

            if( eBlockType == eCHILDBLOCK )
            {
                cbCachedBlockHeaderSize = GetMemsize_ChildBlock(f_pNS);
            }
            else if( eBlockType == eDATABLOCK || eBlockType == eFREEBLOCK )
            {
                cbCachedBlockHeaderSize = GetMemsize_DataBlock();
            }
            else
            {
                ChkDR( DRM_E_DST_BLOCK_CACHE_CORRUPT );
            }
            cbCachedBlockHeaderSize -= ( SIZEOF( _CommBlockHDR ) - SIZEOF( _CommBlockFileHDR ) );

            /* Allocate a new block buffer if not given */
            if ( *f_ppBlock == NULL )
            {
                ChkDR(DRM_HDSIMPL_AllocBlockBuffer(f_pNS, eBlockType, f_ppBlock));
                ChkMem( *f_ppBlock );
            }

/*
** Suppress Microsoft's PREfast static analysis tool's erroneous buffer overrun warning
** for the enclosed statement.  There is no buffer overflow.
*/
#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:26000)
#endif
            dr = Oem_Hds_CheckBlockHeaderCache( f_nBlkNum,
                            f_pNS->pHDS->fp,
                        &dwBlockHeaderMetadata,
                            (*f_ppBlock)->File.bFileImage,
                            cbCachedBlockHeaderSize );
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif

            if( eBlockType == eCHILDBLOCK )
            {
                ((_ChildBlockHDR*)(*f_ppBlock))->nCurrSlotPos = MAXSLOTPOS;
                ((_ChildBlockHDR*)(*f_ppBlock))->nNextSlotPos = MAXSLOTPOS;
                ((_ChildBlockHDR*)(*f_ppBlock))->nPayloadSize = f_pNS->nImagesize_ChildBlockPayload;
            }
            else if( eBlockType == eDATABLOCK || eBlockType == eFREEBLOCK )
            {
                ((_DataBlockHDR*)(*f_ppBlock))->nPayloadSize = f_pNS->pHDS->nImagesize_DataBlockPayload;
            }

            /* So although it was in the cache, it may have been removed since we checked.
            ** need to make sure we handle that eventuality properly */
            if( dr == DRM_E_DST_BLOCK_CACHE_MISS )
            {
                /* Bad luck, it's been removed from the cache. We'll just have to load it from disk */
                dr = DRM_SUCCESS;
            }
            else if( DRM_SUCCEEDED(dr) )
            {
                *f_pfFound = TRUE;
                goto ErrorExit;
            }

            ChkDR( dr );
        }
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HBHC_UpdateBlockHeaderCache(
    __in                                      DRM_DWORD   f_dwBlockNumber,
    __in_opt                                  OEM_FILEHDL f_hHDSFileHandle,
    __in                                      DRM_DWORD   f_dwBlockHeaderMetadata,
    __in_bcount_opt( f_cbBytesToWrite ) const DRM_BYTE   *f_pbBlock,
    __in                                      DRM_DWORD   f_cbBytesToWrite )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_Hds_UpdateBlockHeaderCache(
        f_dwBlockNumber,
        f_hHDSFileHandle,
        f_dwBlockHeaderMetadata,
        f_pbBlock,
        f_cbBytesToWrite ) );
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

