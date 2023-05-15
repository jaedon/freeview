/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMIMPL_COMMON_C
#include <drmcrt.h>
#include <drmcmdlnpars.h>
#include <drmhds_impl.h>
#include <drmerr.h>
#include <drmbytemanip.h>

#if DRM_SUPPORT_SST_REDUNANCY
#include <drmtoken.h>
#endif /* DRM_SUPPORT_SST_REDUNANCY */
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/* Number of entries in the block header cache. Each entry will occupy
** SIZEOF( BLOCK_HEADER_CACHE_ENTRY ) bytes of memory. A larger cache
** will mean a lower chance of needing to load a block header from the disk
*/
#define MAX_CACHED_BLOCK_HEADERS 500

typedef struct
{
    DRM_DWORD dwBlockNumber;
    DRM_DWORD dwCacheHits;
    DRM_DWORD dwBlockHeaderMetadata;
    DRM_BYTE  rgbBlockHeader[CALC_MAXBLOCKHEADERLEN];
    DRM_DWORD cbBytesAvailable;
    OEM_FILEHDL hHDSFileHandle;

} BLOCK_HEADER_CACHE_ENTRY;

#if DRM_SUPPORT_DATASTORE_PREALLOC
#define DEF_DATASTORE_INITSIZE      1024     /* By default prealloc 1024 K for initial datastore size */
#define DEF_DATASTORE_GROWSIZE      1024     /* By default grow 1024 K for each datastore expansion */
#else
#define DEF_DATASTORE_INITSIZE      0
#define DEF_DATASTORE_GROWSIZE      0
#endif /* DRM_SUPPORT_DATASTORE_PREALLOC */

/* Currently 32K is the only supported block size that has been tested by Microsoft. */
#define DEF_DATASTORE_BLOCKSIZE     32 * 1024;  /* By default use 32K block */

#if DRM_SUPPORT_SST_REDUNANCY
DRM_API DRM_RESULT DRM_CALL VariableSSTRedundancyGetorSet(
    __in const DRM_CONST_STRING *pdstrToken,
   __in TOKEN *pNewValue,
   __out TOKEN *pResult,
   __inout DRM_VOID *pvOpaqueData,
   __in DRM_BOOL fSet )
{
    return DRM_E_NOTIMPL;
}

#endif /* DRM_SUPPORT_SST_REDUNANCY */

/*
**      NOTE TO IMPLEMENTOR:
**
**      This cache MUST be global to all instances of the porting kit sharing
**      the same HDS file. Also, there should be appropriate resource locking
**      added around access to the cache to protect its integrity. Also, if
**      multiple HDS files are used, the caches for each file must be separate.
**
*/
static BLOCK_HEADER_CACHE_ENTRY g_rgBlockHeaderCache[MAX_CACHED_BLOCK_HEADERS] = {0};

/*****************************************************************************
** Function: Oem_Hds_ClearBlockHeaderCache
**
** Synopsis: This function should clear any cached data. This function will
**           be triggered if Drm_ClearAllCaches is called, and is used
**           when the HDS, devcert, or private key may have changed
**           (ie if the device is formatted)
**
*****************************************************************************/
DRM_API DRM_VOID DRM_CALL Oem_Hds_ClearBlockHeaderCache( DRM_VOID )
{
    DRM_DWORD i;

    for( i = 0; i < MAX_CACHED_BLOCK_HEADERS; i++ )
    {
        g_rgBlockHeaderCache[i].dwBlockNumber = 0;
    }
}

/*****************************************************************************
** Function: Oem_Hds_CheckBlockHeaderCache
**
** Synopsis: This function should attempt to retrieve the block header for the
**           specified block number from the cache. If there is no matching entry
**           in the cache, the function should return DRM_E_HDS_BLOCK_CACHE_MISS
**
** Arguments:
**           IN   f_dwBlockNumber          -- Block number of the block header to be retrieved,
**                                            note that it will always be greater than 0
**           IN   f_pHDSFileHandle         -- The OEM filehandle to the HDS file. This may be useful
**                                            if multiple HDS files are used on the same system, each
**                                            HDS file must have its own unique cache.
**           OUT  f_pdwBlockHeaderMetadata -- Returns metadata associated with the block header
**           OUT *f_pbBlock                -- Pointer to a buffer to receive the block header
**           IN   f_cbBytesToRead          -- Number of bytes to read from the cache
**
** Returns:
**           DRM_SUCCESS                 -- On success
**           DRM_E_INVALIDARG            -- if any argument is invalid
**           DRM_E_HDS_BLOCK_CACHE_MISS  -- A matching cache entry was not found
*****************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Definition must match common declaration" );
DRM_API DRM_RESULT DRM_CALL Oem_Hds_CheckBlockHeaderCache(
    __in      DRM_DWORD   f_dwBlockNumber,
    __in_opt  OEM_FILEHDL f_hHDSFileHandle,
    __out_opt DRM_DWORD  *f_pdwBlockHeaderMetadata,
    __out_bcount_opt( f_cbBytesToRead ) DRM_BYTE   *f_pbBlock,
    __in      DRM_DWORD   f_cbBytesToRead )
PREFAST_POP
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i = 0;

    ChkArg( f_dwBlockNumber > 0 );

    if( f_cbBytesToRead > 0 )
    {
        ChkArg( f_pbBlock != NULL &&
                f_cbBytesToRead <= CALC_MAXBLOCKHEADERLEN );
    }

    for( i = 0; i < MAX_CACHED_BLOCK_HEADERS; i++ )
    {
        if( g_rgBlockHeaderCache[i].dwBlockNumber == f_dwBlockNumber && g_rgBlockHeaderCache[i].hHDSFileHandle == f_hHDSFileHandle )
        {
            if( g_rgBlockHeaderCache[i].cbBytesAvailable < f_cbBytesToRead )
            {
                break;
            }

            /* We've got a cache hit */
            if( f_pdwBlockHeaderMetadata != NULL )
            {
                *f_pdwBlockHeaderMetadata = g_rgBlockHeaderCache[i].dwBlockHeaderMetadata;
            }

            if( f_cbBytesToRead > 0 )
            {
                MEMCPY( f_pbBlock, g_rgBlockHeaderCache[i].rgbBlockHeader, f_cbBytesToRead );
            }

            /* Track how many hits we get for this block header, the data can be used
            ** to throw out infrequently used entries */
            g_rgBlockHeaderCache[i].dwCacheHits++;

            goto ErrorExit;
        }
    }

    ChkDR( DRM_E_DST_BLOCK_CACHE_MISS );

ErrorExit:

    return dr;
}

/*****************************************************************************
** Function: Oem_Hds_UpdateBlockHeaderCache
**
** Synopsis: This function should attempt to retrieve the block header for the
**           specified block number from the cache. If there is no matching entry
**           in the cache, the function should return DRM_E_HDS_BLOCK_CACHE_MISS
**
** Arguments:
**           IN   f_dwBlockNumber          -- Block number of the block header to be updated,
**                                            note that it will always be greater than 0
**           IN   f_pHDSFileHandle         -- The OEM filehandle to the HDS file. This may be useful
**                                            if multiple HDS files are used on the same system, each
**                                            HDS file must have its own unique cache.
**           IN   f_dwBlockHeaderMetadata  -- Specifies metadata to be associated with the block header
**           IN  *f_pbBlock                -- Pointer to a buffer containg the block header data
**           IN   f_cbBytesToRead          -- Number of bytes to write to the cache
**
** Returns:
**           DRM_SUCCESS                 -- On success
**           DRM_E_INVALIDARG            -- if any argument is invalid
**           DRM_S_FALSE                 -- No space to create a new cache entry
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hds_UpdateBlockHeaderCache(
    __in                                      DRM_DWORD   f_dwBlockNumber,
    __in_opt                                  OEM_FILEHDL f_hHDSFileHandle,
    __in                                      DRM_DWORD   f_dwBlockHeaderMetadata,
    __in_bcount_opt( f_cbBytesToWrite ) const DRM_BYTE   *f_pbBlock,
    __in                                      DRM_DWORD   f_cbBytesToWrite )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_LONG i = 0;
    DRM_LONG iLastEmpty = -1;

    ChkArg( f_dwBlockNumber > 0 );

    if ( f_cbBytesToWrite > 0 )
    {
        ChkArg( f_pbBlock != NULL &&
                f_cbBytesToWrite <= CALC_MAXBLOCKHEADERLEN );
    }

    for ( i = 0; i < MAX_CACHED_BLOCK_HEADERS; i++ )
    {
        if ( g_rgBlockHeaderCache[i].dwBlockNumber == f_dwBlockNumber && g_rgBlockHeaderCache[i].hHDSFileHandle == f_hHDSFileHandle )
        {
            /* We've already got a cache entry, update it */

            g_rgBlockHeaderCache[i].dwBlockHeaderMetadata = f_dwBlockHeaderMetadata;

            if ( f_cbBytesToWrite > 0 )
            {
                MEMCPY ( g_rgBlockHeaderCache[i].rgbBlockHeader, f_pbBlock, f_cbBytesToWrite );

                if ( g_rgBlockHeaderCache[i].cbBytesAvailable < f_cbBytesToWrite )
                {
                    g_rgBlockHeaderCache[i].cbBytesAvailable = f_cbBytesToWrite;
                }
            }

            /* We're done */
            goto ErrorExit;
        }
        else if ( g_rgBlockHeaderCache[i].dwBlockNumber == 0 && iLastEmpty == -1 )
        {
            /* Take note of the empty block cache entry, we may need it */
            iLastEmpty = i;
        }
    }

    /* No existing entry, if there's empty space then create a new entry */
    if ( iLastEmpty > -1 )
    {
        g_rgBlockHeaderCache[iLastEmpty].dwBlockHeaderMetadata = f_dwBlockHeaderMetadata;

        if ( f_cbBytesToWrite > 0 )
        {
            MEMCPY ( g_rgBlockHeaderCache[iLastEmpty].rgbBlockHeader, f_pbBlock, f_cbBytesToWrite );
            g_rgBlockHeaderCache[iLastEmpty].cbBytesAvailable = f_cbBytesToWrite;
        }

        g_rgBlockHeaderCache[iLastEmpty].dwBlockNumber  = f_dwBlockNumber;
        g_rgBlockHeaderCache[iLastEmpty].hHDSFileHandle = f_hHDSFileHandle;

        goto ErrorExit;
    }

    /* No room to cache this one */
    dr = DRM_S_FALSE;

ErrorExit:

    return dr;
}

/*****************************************************************************
** Function: Oem_Hds_GetParams
**
** Synopsis: This function retrieves parameters of HDS file. OEM may override
**           the default implementation to suit its platform.
**
** Arguments:
**           IN   f_pOEMContext            -- Pointer to an OEM context. It may be NULL.
**           IN   f_pwszDeviceStoreName    -- Pointer to a location of HDS file.
**           OUT  f_pdwDataStoreFileSizeInitKB
**                                         -- Pointer to a variable to receive the
**                                            initial size (in KB) of the HDS file.
**                                            It may be NULL.
**           OUT  f_pdwDataStoreFileSizeGrowKB
**                                         -- Pointer to a variable to receive the
**                                            growth size (in KB) of the HDS file.
**                                            It may be NULL.
**           OUT  f_pdwDataStoreBlockSize
**                                         -- Pointer to a variable to receive the
**                                            block size of the HDS file.
**                                            It may be NULL.
**
** Returns:
**           DRM_SUCCESS                 -- On success
**           DRM_E_INVALIDARG            -- if any argument is invalid
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hds_GetParams(
    __in_opt        DRM_VOID  *f_pOEMContext,
    __in      const DRM_WCHAR *f_pwszDeviceStoreName,
    __out_opt       DRM_DWORD *f_pdwDataStoreFileSizeInitKB,
    __out_opt       DRM_DWORD *f_pdwDataStoreFileSizeGrowKB,
    __out_opt       DRM_DWORD *f_pdwDataStoreBlockSize )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pwszDeviceStoreName != NULL );

    if ( f_pdwDataStoreFileSizeInitKB != NULL )
    {
        *f_pdwDataStoreFileSizeInitKB = DEF_DATASTORE_INITSIZE;
    }

    if ( f_pdwDataStoreFileSizeGrowKB != NULL )
    {
        *f_pdwDataStoreFileSizeGrowKB = DEF_DATASTORE_GROWSIZE;
    }

    if ( f_pdwDataStoreBlockSize != NULL )
    {
        /* Currently 32K is the only supported block size that has been tested by Microsoft. */
        *f_pdwDataStoreBlockSize = DEF_DATASTORE_BLOCKSIZE;
    }

ErrorExit:

    return dr;
}

#if defined(DRM_MSC_VER) || defined(__GNUC__)
/*
DRMFORCEINLINE DRM_VOID * DRM_CALL DRMCRT_LocalMemcpy(
    __out_bcount_full( f_cbCount )  DRM_VOID  *f_pOut,
    __in_bcount( f_cbCount ) const  DRM_VOID  *f_pIn,
    __in                            DRM_DWORD  f_cbCount ) DRM_ALWAYS_INLINE_ATTRIBUTE;
*/
DRM_VOID * DRM_CALL DRMCRT_LocalMemcpy(
    __out_bcount_full( f_cbCount )  DRM_VOID  *f_pOut,
    __in_bcount( f_cbCount ) const  DRM_VOID  *f_pIn,
    __in                            DRM_DWORD  f_cbCount )
{
    const char * pIn = (const char *) f_pIn;
    char * pOut = (char *) f_pOut;
    while (f_cbCount--)
    {
        *pOut++ = *pIn++;
    }
    return pOut;
}

/*
DRMFORCEINLINE DRM_VOID * DRM_CALL DRMCRT_ScrubMemory(
    __out_bcount( f_cbCount ) DRM_VOID  *f_ptr,
    IN DRM_DWORD  f_cbCount  ) DRM_ALWAYS_INLINE_ATTRIBUTE;
*/
DRM_VOID * DRM_CALL DRMCRT_ScrubMemory(
    __out_bcount( f_cbCount ) DRM_VOID  *f_ptr,
    __in DRM_DWORD  f_cbCount  )
{
    /*
    ** Casting the pointer to volatile makes MS and GNU compilers act
    ** as if another thread can see and access the buffer. This
    ** prevents the compiler from reordering or optimizing away
    ** the writes.
    */

    volatile char *vptr = (volatile char *)f_ptr;

    while (f_cbCount)
    {
        *vptr = 0;
        vptr++;
        f_cbCount--;
    }

    return f_ptr;
}

 
DRM_VOID * DRM_CALL DRMCRT_LocalMemset(
    __out_bcount_full( f_cbCount )  DRM_VOID  *f_pOut,
    __in                            DRM_BYTE   f_cbValue,
    __in                            DRM_DWORD  f_cbCount )
{
    /* w/o volatile some compilers can figure out this is an
       implementation of a memset */
    volatile unsigned char * pOut = (unsigned char *) f_pOut;
    while (f_cbCount--)
    {
        *pOut++ = f_cbValue;
    }
    return f_pOut;
}

 
DRM_VOID * DRM_CALL DRMCRT_LocalDWORDSetZero(
    __out_bcount_full( f_cdwCount ) DRM_DWORD  *f_pdwOut,
    __in                            DRM_DWORD   f_cdwCount )
{
    /* w/o volatile some compilers can figure out this is an
       implementation of a memset */
    volatile DRM_DWORD * pdwOut = f_pdwOut;
    while (f_cdwCount--)
    {
        *pdwOut++ = 0;
    }
    return f_pdwOut;
}

 
DRM_BOOL DRM_CALL DRMCRT_LocalAreEqual(
    __in_bcount( f_cbCount ) const  DRM_VOID  *f_pLHS,
    __in_bcount( f_cbCount ) const  DRM_VOID  *f_pRHS,
    __in                            DRM_DWORD  f_cbCount )
{
    /*
    ** Without volatile, some compilers can figure out this is an
    ** implementation of a memcmp-similar function or can
    ** optimize out checks which we don't want.
    */
    volatile DRM_DWORD dwResult = 0;

#if TARGET_SUPPORTS_UNALIGNED_DWORD_POINTERS

    /* Optimize: When unaligned pointers are allowed, compare in DRM_DWORD-sized groups. */
    volatile const DRM_DWORD *pLHS = (const DRM_DWORD*) f_pLHS;
    volatile const DRM_DWORD *pRHS = (const DRM_DWORD*) f_pRHS;

    while( f_cbCount >= SIZEOF(DRM_DWORD) )
    {
        dwResult |= (*pLHS ^ *pRHS);
        pLHS++;
        pRHS++;
        f_cbCount -= SIZEOF(DRM_DWORD);
    }

#else   /* TARGET_SUPPORTS_UNALIGNED_DWORD_POINTERS */

    volatile const DRM_BYTE *pLHS = (const DRM_BYTE*) f_pLHS;
    volatile const DRM_BYTE *pRHS = (const DRM_BYTE*) f_pRHS;

#endif  /* TARGET_SUPPORTS_UNALIGNED_DWORD_POINTERS */

    while( f_cbCount-- )
    {
        volatile const DRM_BYTE bLHS = ((const DRM_BYTE*)pLHS)[f_cbCount];
        volatile const DRM_BYTE bRHS = ((const DRM_BYTE*)pRHS)[f_cbCount];
        dwResult |= (DRM_DWORD)( bLHS ^ bRHS );
    }

    return ( dwResult == 0 );
}


 
DRM_VOID * DRM_CALL DRMCRT_LocalDWORDcpy(
    __out_ecount_full( f_cdwCount )  DRM_DWORD  *f_pdwOut,
    __in_ecount( f_cdwCount ) const  DRM_DWORD  *f_pdwIn,
    __in                             DRM_DWORD  f_cdwCount )
{
    /* w/o volatile some compilers can figure out this is an
       implementation of a memcpy */
    volatile const DRM_DWORD * pdwIn = (const DRM_DWORD *) f_pdwIn;
    DRM_DWORD * pdwOut = (DRM_DWORD *) f_pdwOut;
    while (f_cdwCount--)
    {
        *pdwOut++ = *pdwIn++;
    }
    return pdwOut;
}

#endif


EXIT_PK_NAMESPACE_CODE;
