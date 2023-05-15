/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmdevicestore.h>
#include <drmdatastore.h>
#include <byteorder.h>

ENTER_PK_NAMESPACE_CODE;

/* DEVICEID */
static const DRM_WCHAR g_wszDeviceID[]            = { ONE_WCHAR('D', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0') };

/* We store the size of the attribute name and attribute value as a DWORD */
#define DEVICESTORE_ENTRY_SIZE( namelen, contentlen ) ( SIZEOF( DRM_DWORD ) + namelen + SIZEOF( DRM_DWORD ) + contentlen )


static DRM_RESULT _CalculateStoreID( const DRM_BYTE *pbData, DRM_DWORD cbData, DRM_ID *pID )
{
    DRM_RESULT      dr  = DRM_SUCCESS;
    DRM_SHA_CONTEXT sha = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE        rgb[__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC__CalculateStoreID );

    ChkArg( pbData != NULL
        && cbData != 0
        && pID != NULL );

    ChkDR( DRM_SHA_Init( &sha, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Update( pbData, cbData, &sha ) );
    ChkDR( DRM_SHA_Finalize( &sha, DRM_SHA1_DIGEST_LEN, rgb ) );
    MEMCPY( pID, rgb, SIZEOF( DRM_ID ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


static DRM_RESULT _CalculateKeyPassword(
    DRM_BB_CONTEXT *f_pcontextBBX,
    const DRM_ID *f_pid,
    DRM_BYTE f_rgbPassword[DRM_SHA1_DIGEST_LEN] )
{
    /* Assuming DRM_BBX_HashValue checks arguments */
    return DRM_BBX_HashValue( (DRM_BYTE*)f_pid, SIZEOF ( DRM_ID ), f_rgbPassword, f_pcontextBBX);
}

static DRM_RESULT _WriteDeviceStoreEntryItem(
    IN      const DRM_BYTE *f_pbName,
    IN      const DRM_DWORD f_cbName,
    IN      const DRM_BYTE *f_pbContent,
    IN      const DRM_DWORD f_cbContent,
    IN OUT        DRM_BYTE *f_pbBase,
    IN            DRM_DWORD f_ibOffset )
{
    DRM_DWORD dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC__WriteDeviceStoreEntryItem );

    ChkArg( f_pbName != NULL
        &&   f_cbName != 0
        &&   f_pbContent != NULL
        &&   f_pbBase != NULL );

    /* The caller must use the DEVICESTORE_ENTRY_SIZE macro to compute the required buffer size */

    DWORD_TO_NETWORKBYTES( f_pbBase, f_ibOffset, f_cbName );
    f_ibOffset += SIZEOF( DRM_DWORD );

    DRM_BYT_CopyBytes( f_pbBase, f_ibOffset, f_pbName, 0, f_cbName );
    f_ibOffset += f_cbName;

    DWORD_TO_NETWORKBYTES( f_pbBase, f_ibOffset, f_cbContent );
    f_ibOffset += SIZEOF( DRM_DWORD );

    DRM_BYT_CopyBytes( f_pbBase, f_ibOffset, f_pbContent, 0, f_cbContent );
    f_ibOffset += f_cbContent;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_RESULT _ReadDeviceStoreEntryItem(
    OUT           DRM_DWORD *f_pibName,
    OUT           DRM_DWORD *f_pcbName,
    OUT           DRM_DWORD *f_pibContent,
    OUT           DRM_DWORD *f_pcbContent,
    IN      const DRM_BYTE  *f_pbBase,
    IN OUT        DRM_DWORD *f_pibOffset,
    IN            DRM_DWORD  f_cbSize )
{
    DRM_DWORD dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC__ReadDeviceStoreEntryItem );

    ChkArg( f_pibName != NULL
        &&   f_pcbName != NULL
        &&   f_pibContent != NULL
        &&   f_pcbContent != NULL
        &&   f_pbBase != NULL
        &&   f_pibOffset != NULL );

    /* The device data should terminate only at the exact end of the buffer */
    if( f_cbSize == *f_pibOffset )
    {
        ChkDR( DRM_E_NOMORE );
    }

    /* If the entry has extra bytes, but not enough for a DWORD, the entry is corrupt */
    ChkBOOL( f_cbSize >= ( *f_pibOffset + SIZEOF( DRM_DWORD ) ), DRM_E_INVALID_DEVSTORE_ENTRY );

    NETWORKBYTES_TO_DWORD( *f_pcbName, f_pbBase, *f_pibOffset );
    *f_pibOffset += SIZEOF( DRM_DWORD );

    ChkBOOL( f_cbSize >= ( *f_pibOffset + *f_pcbName + SIZEOF( DRM_DWORD ) ), DRM_E_INVALID_DEVSTORE_ENTRY );
    *f_pibName = *f_pibOffset;
    *f_pibOffset+= *f_pcbName;

    NETWORKBYTES_TO_DWORD( *f_pcbContent, f_pbBase, *f_pibOffset );
    *f_pibOffset += SIZEOF( DRM_DWORD );

    ChkBOOL( f_cbSize >= ( *f_pibOffset + *f_pcbContent ), DRM_E_INVALID_DEVSTORE_ENTRY );
    *f_pibContent = *f_pibOffset;
    *f_pibOffset += *f_pcbContent;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_RESULT _SetDirtyFlag( _DeviceHandle *pDeviceHandle, DRM_DWORD dwIndex )
{
    DRM_DWORD   i = 0;
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_DWORD  *pdwNewList = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC__SetDirtyFlag );

    ChkArg( pDeviceHandle != NULL );

    for( i = 0; i < pDeviceHandle->dwNumDirtyAttributes; i++ )
    {
        if( pDeviceHandle->pdwDirtyAttributes[i] == dwIndex )
        {
            /* The dirty flag has already been set, so we're done */
            goto ErrorExit;
        }
    }

    if( pDeviceHandle->dwNumDirtyAttributes == pDeviceHandle->dwDirtyAttributesSize )
    {
        /* Our dirty attribute list buffer is full, so let's double it's size */
        ChkOverflow((pDeviceHandle->dwDirtyAttributesSize+1), pDeviceHandle->dwDirtyAttributesSize);
        ChkOverflow((2*(pDeviceHandle->dwDirtyAttributesSize+1)), (pDeviceHandle->dwDirtyAttributesSize+1));
        ChkOverflow((2*(pDeviceHandle->dwDirtyAttributesSize+1)*SIZEOF(DRM_DWORD)), (2*(pDeviceHandle->dwDirtyAttributesSize+1)));
        ChkMem( pdwNewList = (DRM_DWORD *)Oem_MemAlloc( 2 * (pDeviceHandle->dwDirtyAttributesSize + 1) * SIZEOF( DRM_DWORD ) ) );

        if( pDeviceHandle->dwNumDirtyAttributes > 0 )
        {
            DRM_BYT_CopyBytes( pdwNewList, 0, pDeviceHandle->pdwDirtyAttributes, 0, pDeviceHandle->dwDirtyAttributesSize * SIZEOF( DRM_DWORD ) );
        }

        SAFE_OEM_FREE( pDeviceHandle->pdwDirtyAttributes );
        pDeviceHandle->pdwDirtyAttributes = pdwNewList;
        pDeviceHandle->dwDirtyAttributesSize = (pDeviceHandle->dwDirtyAttributesSize + 1) * 2;

        pdwNewList = NULL;
    }

    pDeviceHandle->pdwDirtyAttributes[pDeviceHandle->dwNumDirtyAttributes++] = dwIndex;

ErrorExit:
    SAFE_OEM_FREE( pdwNewList );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_RESULT _GetDirtyFlag( _DeviceHandle *pDeviceHandle, DRM_DWORD dwIndex, DRM_BOOL *pfDirty )
{
    DRM_DWORD   i = 0;
    DRM_RESULT  dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC__GetDirtyFlag );

    ChkArg( pDeviceHandle != NULL
            && pfDirty != NULL
            && pDeviceHandle->pdwDirtyAttributes != NULL );

    *pfDirty = FALSE;
    for( i = 0;i < pDeviceHandle->dwNumDirtyAttributes; i++ )
    {
        if( pDeviceHandle->pdwDirtyAttributes[i] == dwIndex )
        {
            /* The dirty flag had been set */
            *pfDirty = TRUE;
            break;
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_RESULT _InitDeviceHandle( _DeviceHandle **ppDeviceHandle, _DeviceStoreContext *pDeviceStore, DRM_BYTE *pbDeviceID, DRM_DWORD cbDeviceID, DRM_BYTE *pbEntry, DRM_DWORD cbEntry)
{
    DRM_DWORD dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC__InitDeviceHandle );

    ChkArg( ppDeviceHandle );

    ChkMem( *ppDeviceHandle = (_DeviceHandle *)Oem_MemAlloc( SIZEOF( _DeviceHandle ) ) );
    ZEROMEM( *ppDeviceHandle, SIZEOF( _DeviceHandle ) );

    (*ppDeviceHandle)->oDeviceID.pbBlob = pbDeviceID;
    (*ppDeviceHandle)->oDeviceID.cbBlob = cbDeviceID;

    (*ppDeviceHandle)->pbEntry = pbEntry;
    (*ppDeviceHandle)->cbEntry = cbEntry;

    (*ppDeviceHandle)->pDeviceStoreContext = pDeviceStore;

    (*ppDeviceHandle)->dwDirtyAttributesSize = DEVICESTORE_NUMDIRTYATTRIBUTES_INTIAL;
    ChkMem( (*ppDeviceHandle)->pdwDirtyAttributes = (DRM_DWORD *)Oem_MemAlloc( DEVICESTORE_NUMDIRTYATTRIBUTES_INTIAL * SIZEOF( DRM_DWORD ) ) );

    /* We don't know how many attributes there are yet */
    (*ppDeviceHandle)->dwNumAttributes = DEVICESTORE_NUMATTRIBUTES_UNKNOWN;

ErrorExit:

    if( DRM_FAILED( dr ) )
    {
        if( ppDeviceHandle != NULL )
        {
            if( *ppDeviceHandle != NULL )
            {
                if( (*ppDeviceHandle)->pdwDirtyAttributes != NULL )
                {
                    SAFE_OEM_FREE( (*ppDeviceHandle)->pdwDirtyAttributes );
                }

                SAFE_OEM_FREE( *ppDeviceHandle );
            }
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_Initialize(
    IN     DRM_DEVICESTORE_CONTEXT   *f_pDeviceStoreContext,
    IN     DRM_APP_CONTEXT       *f_pDeviceStoreInitData )
{
    DRM_RESULT                dr            = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *pContext     = (DRM_APP_CONTEXT_INTERNAL*)f_pDeviceStoreInitData;
    _DeviceStoreContext      *pDeviceStore = (_DeviceStoreContext*)f_pDeviceStoreContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_Initialize );

    ChkArg(f_pDeviceStoreContext != NULL
        && f_pDeviceStoreInitData != NULL );

    /* Make sure that the size of the context matches the size of our _DeviceStoreContext */
    DRMSIZEASSERT( SIZEOF( DRM_DEVICESTORE_CONTEXT ), SIZEOF( _DeviceStoreContext ) );


    ZEROMEM( f_pDeviceStoreContext, SIZEOF( _DeviceStoreContext ) );

    /* Copy out pointers to the necessary contexts from DRM_APP_CONTEXT */
    pDeviceStore->pDatastore  = &pContext->oDatastoreHDS;
    pDeviceStore->pBBXContext = &pContext->oBlackBoxContext;
    pDeviceStore->pSecureStoreContext = &pContext->oSecStoreGlobalContext;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



DRM_API DRM_VOID DRM_CALL DRM_DVS_Uninitialize(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext )
{
    return;
}




DRM_API DRM_RESULT DRM_CALL DRM_DVS_RegisterDevice(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    IN     DRM_BYTEBLOB                  f_DeviceID,
       OUT DRM_DEVICESTORE_HANDLE       *f_pDeviceHandle )
{
    DRM_RESULT                    dr            = DRM_SUCCESS;
    _DeviceStoreContext          *pDeviceStore = (_DeviceStoreContext*)f_pDeviceStoreContext;
    DRM_DWORD                     dwSize = 0,
                                  cbData = 0;
    DRM_BYTE                      rgbPassword[DRM_SHA1_DIGEST_LEN];
    DRM_ID                        key = {0};
    DRM_BYTE                     *pbData = NULL;
    _DeviceHandle                *pDeviceHandle = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_RegisterDevice );

    ChkArg( f_pDeviceHandle             != NULL
         && f_DeviceID.pbBlob           != NULL
         && f_DeviceID.cbBlob           > 0
         && f_pDeviceStoreContext       != NULL );

    *f_pDeviceHandle = NULL;


    ChkDR( _CalculateStoreID( f_DeviceID.pbBlob, f_DeviceID.cbBlob, &key ) );
    ChkDR( _CalculateKeyPassword( pDeviceStore->pBBXContext, &key, rgbPassword ) );


    /* Check for already registered...
    */
    dr = DRM_SST_GetData(   pDeviceStore->pSecureStoreContext,
                                 &key,
                                 &key,
                                 rgbPassword,
                                 SECURE_STORE_DEVICE_REGISTRATION_DATA,
                                 pDeviceStore->pDatastore,
                                 NULL,
                                 &dwSize );
    if( DRM_SUCCEEDED( dr )
        || dr == DRM_E_BUFFERTOOSMALL )
    {
       ChkDR( DRM_E_DEVICE_ALREADY_REGISTERED );
    }
    else if( dr != DRM_E_DSTSLOTNOTFOUND )
    {
        ChkDR( dr );
    }
    /* dr should be DRM_E_DSTSLOTNOTFOUND */


    /* Make a device handle
    */
    ChkDR( _InitDeviceHandle( &pDeviceHandle, pDeviceStore, NULL, f_DeviceID.cbBlob, NULL, 0 ) );

    ChkMem( pDeviceHandle->oDeviceID.pbBlob = (DRM_BYTE *)Oem_MemAlloc( f_DeviceID.cbBlob ) );
    MEMCPY( pDeviceHandle->oDeviceID.pbBlob, f_DeviceID.pbBlob, f_DeviceID.cbBlob );


    /* Write the device ID as the first attribute
    */
    cbData = DEVICESTORE_ENTRY_SIZE( SIZEOF( g_wszDeviceID ), f_DeviceID.cbBlob );
    ChkMem( pbData = (DRM_BYTE *)Oem_MemAlloc( cbData ) );

    pDeviceHandle->pbEntry = pbData;
    pDeviceHandle->cbEntry = cbData;

    ChkDR( _WriteDeviceStoreEntryItem( (DRM_BYTE *)g_wszDeviceID,
                                                        SIZEOF( g_wszDeviceID ),
                                                        f_DeviceID.pbBlob,
                                                        f_DeviceID.cbBlob,
                                                        pbData,
                                                        0 ) );

    /* There is now just 1 attribute, the DEVICEID attribute */
    pDeviceHandle->dwNumAttributes = 1;

    /* We've created the device, so now let's commit it to the store */
    dr = DRM_SST_SetData( pDeviceStore->pSecureStoreContext,
                            &key,
                            &key,
                            rgbPassword,
                            SECURE_STORE_DEVICE_REGISTRATION_DATA,
                            pDeviceStore->pDatastore,
                            pDeviceHandle->pbEntry,
                            pDeviceHandle->cbEntry );


    *f_pDeviceHandle = pDeviceHandle;

ErrorExit:
    if( DRM_FAILED( dr )
        && pDeviceHandle != NULL )
    {
        /* We only want to free the handle, not commit it */
        pDeviceHandle->fDontCommitOnFree = TRUE;
        DRM_DVS_FreeDevice( pDeviceHandle );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;

}



DRM_API DRM_RESULT DRM_CALL DRM_DVS_UnRegisterDevice(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    IN     DRM_BYTEBLOB                  f_DeviceID )
{
    DRM_RESULT                    dr            = DRM_SUCCESS;
    _DeviceStoreContext          *pDeviceStore = (_DeviceStoreContext*)f_pDeviceStoreContext;
    DRM_ID                        key = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_UnRegisterDevice );

    ChkArg(f_pDeviceStoreContext != NULL
         && f_DeviceID.pbBlob      != NULL );

    ChkDR( _CalculateStoreID( f_DeviceID.pbBlob, f_DeviceID.cbBlob, &key ) );

    ChkDR( DRM_SST_DeleteKey( pDeviceStore->pSecureStoreContext,
                SECURE_STORE_DEVICE_REGISTRATION_DATA,
                &key,
                &key,
                pDeviceStore->pDatastore ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_API DRM_VOID DRM_CALL DRM_DVS_FreeDevice(
    IN    DRM_DEVICESTORE_HANDLE            f_hDeviceHandle )
{
    _DeviceHandle                *pDeviceHandle = NULL;

    if( f_hDeviceHandle != NULL )
    {
        pDeviceHandle = (_DeviceHandle *)f_hDeviceHandle;

        if( !pDeviceHandle->fDontCommitOnFree )
        {
            DRM_DVS_CommitDevice( f_hDeviceHandle );
        }

        SAFE_OEM_FREE( pDeviceHandle->pbEntry );
        SAFE_OEM_FREE( pDeviceHandle->oDeviceID.pbBlob );
        SAFE_OEM_FREE( pDeviceHandle->pdwDirtyAttributes );
        SAFE_OEM_FREE( pDeviceHandle );
    }

    return;
}


DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetFirstDevice(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
       OUT DRM_DEVICESTORE_HANDLE       *f_pDeviceHandle )
{
    DRM_RESULT                    dr            = DRM_SUCCESS;
    _DeviceStoreContext          *pDeviceStore = (_DeviceStoreContext*)f_pDeviceStoreContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetFirstDevice );

    ChkArg( f_pDeviceStoreContext != NULL
        &&   f_pDeviceHandle        != NULL
        &&   !pDeviceStore->fEnumInited );

    ChkDR( DRM_SST_OpenEnumerator( SECURE_STORE_DEVICE_REGISTRATION_DATA,
                                    NULL,
                                    &pDeviceStore->oSecStoreEnum,
                                    pDeviceStore->pDatastore,
                                    FALSE) );

    pDeviceStore->fEnumInited = TRUE;
    ChkDR( DRM_DVS_GetNextDevice( f_pDeviceStoreContext, f_pDeviceHandle ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetNextDevice(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
       OUT DRM_DEVICESTORE_HANDLE       *f_pDeviceHandle )
{
    DRM_RESULT                    dr            = DRM_SUCCESS;
    _DeviceStoreContext          *pDeviceStore = (_DeviceStoreContext*)f_pDeviceStoreContext;
    DRM_DWORD                     dwSize = 0,
                                  cbData = 0;
    DRM_BYTE                      rgbPassword[DRM_SHA1_DIGEST_LEN];
    DRM_ID                        key = {0};
    DRM_BYTE                     *pbData = NULL;
    _DeviceHandle                *pDeviceHandle = NULL;
    DRM_BYTEBLOB                  DeviceID = {NULL, 0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetNextDevice );

    ChkArg( f_pDeviceStoreContext != NULL
        &&  f_pDeviceHandle        != NULL
        &&  pDeviceStore->fEnumInited );


    *f_pDeviceHandle = NULL;

    dr = DRM_SST_EnumNext(  &pDeviceStore->oSecStoreEnum,
                                  NULL,
                                 &key,
                                 &cbData );
    if ( DRM_E_NOMORE == dr )
    {
        pDeviceStore->fEnumInited = FALSE;
    }
    ChkDR(dr);

    /* Create the device handle */
    ChkDR( _InitDeviceHandle( &pDeviceHandle, pDeviceStore, NULL, 0, NULL, 0 ) );

    ChkMem( pbData = (DRM_BYTE *)Oem_MemAlloc( cbData ) );
    pDeviceHandle->pbEntry = pbData;
    pDeviceHandle->cbEntry = cbData;

    ChkDR( _CalculateKeyPassword( pDeviceStore->pBBXContext, &key, rgbPassword ) );

    dr = DRM_SST_GetData(   pDeviceStore->pSecureStoreContext,
                            &key,
                            &key,
                            rgbPassword,
                            SECURE_STORE_DEVICE_REGISTRATION_DATA,
                            pDeviceStore->pDatastore,
                            pbData,
                            &cbData );
    if( DRM_E_FILENOTFOUND == dr || DRM_E_DSTSLOTNOTFOUND == dr )
    {
        ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
    }
    ChkDR( dr );

    *f_pDeviceHandle = pDeviceHandle;


    /* Load the device ID attribute into the handle */
    DeviceID.pbBlob = (DRM_BYTE*)g_wszDeviceID;
    DeviceID.cbBlob = SIZEOF( g_wszDeviceID );
    dwSize = 0;
    dr = DRM_DVS_GetDeviceAttr( *f_pDeviceHandle, DeviceID, NULL, &dwSize );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }
    pDeviceHandle->oDeviceID.cbBlob = dwSize;

    if( dr == DRM_E_BUFFERTOOSMALL)
    {
        ChkMem( pDeviceHandle->oDeviceID.pbBlob = (DRM_BYTE *)Oem_MemAlloc( dwSize ) );
        ChkDR( DRM_DVS_GetDeviceAttr( *f_pDeviceHandle, DeviceID, pDeviceHandle->oDeviceID.pbBlob, &dwSize ) );
    }

ErrorExit:
    if( DRM_FAILED( dr )
        && pDeviceHandle != NULL )
    {
        /* We only want to free the handle, not commit it */
        pDeviceHandle->fDontCommitOnFree = TRUE;
        DRM_DVS_FreeDevice( pDeviceHandle );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceByID(
    IN OUT DRM_DEVICESTORE_CONTEXT      *f_pDeviceStoreContext,
    IN     DRM_BYTEBLOB                     f_DeviceID,
       OUT DRM_DEVICESTORE_HANDLE            *f_pDeviceHandle )
{
    DRM_RESULT                    dr            = DRM_SUCCESS;
    _DeviceStoreContext          *pDeviceStore = (_DeviceStoreContext*)f_pDeviceStoreContext;
    DRM_DWORD                     cbData = 0;
    DRM_BYTE                      rgbPassword[DRM_SHA1_DIGEST_LEN];
    DRM_ID                        key = {0};
    DRM_BYTE                     *pbData = NULL;
    _DeviceHandle                *pDeviceHandle = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceByID );

    ChkArg( f_pDeviceStoreContext != NULL
        &&   f_pDeviceHandle        != NULL );


    *f_pDeviceHandle = NULL;


    /* Create the device handle */
    ChkDR( _InitDeviceHandle( &pDeviceHandle, pDeviceStore, NULL, f_DeviceID.cbBlob, NULL, 0 ) );

    ChkMem( pDeviceHandle->oDeviceID.pbBlob = (DRM_BYTE *)Oem_MemAlloc( f_DeviceID.cbBlob ) );
    MEMCPY( pDeviceHandle->oDeviceID.pbBlob, f_DeviceID.pbBlob, f_DeviceID.cbBlob );

    ChkDR( _CalculateStoreID( f_DeviceID.pbBlob, f_DeviceID.cbBlob, &key ) );
    ChkDR( _CalculateKeyPassword( pDeviceStore->pBBXContext, &key, rgbPassword ) );

    dr = DRM_SST_GetData( pDeviceStore->pSecureStoreContext,
                                 &key,
                                 &key,
                                 rgbPassword,
                                 SECURE_STORE_DEVICE_REGISTRATION_DATA,
                                 pDeviceStore->pDatastore,
                                 NULL,
                                 &cbData );
    if( DRM_E_FILENOTFOUND == dr || DRM_E_DSTSLOTNOTFOUND == dr )
    {
        ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
    }
    else if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbData = (DRM_BYTE *)Oem_MemAlloc( cbData ) );

        dr = DRM_SST_GetData( pDeviceStore->pSecureStoreContext,
                                &key,
                                &key,
                                rgbPassword,
                                SECURE_STORE_DEVICE_REGISTRATION_DATA,
                                pDeviceStore->pDatastore,
                                pbData,
                                &cbData );

        pDeviceHandle->pbEntry = pbData;
        pDeviceHandle->cbEntry = cbData;

        pDeviceHandle->pDeviceStoreContext = pDeviceStore;
    }
    ChkDR( dr );

    *f_pDeviceHandle = pDeviceHandle;

ErrorExit:
    if( DRM_FAILED( dr )
        && pDeviceHandle != NULL )
    {
        /* We only want to free the handle, not commit it */
        pDeviceHandle->fDontCommitOnFree = TRUE;
        DRM_DVS_FreeDevice( pDeviceHandle );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceID(
    IN     DRM_DEVICESTORE_HANDLE             f_hDeviceHandle,
       OUT DRM_BYTE                        *f_pbDeviceID,
    IN OUT DRM_DWORD                       *f_pcbDeviceID )
{
    DRM_DWORD                     dr = DRM_SUCCESS;
    _DeviceHandle                *pDeviceHandle = (_DeviceHandle *)f_hDeviceHandle;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceID );

    ChkArg( f_pcbDeviceID != NULL
        && f_hDeviceHandle != NULL );


    if( *f_pcbDeviceID < pDeviceHandle->oDeviceID.cbBlob )
    {
        *f_pcbDeviceID = pDeviceHandle->oDeviceID.cbBlob;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkArg( f_pbDeviceID != NULL );

    MEMCPY( f_pbDeviceID, pDeviceHandle->oDeviceID.pbBlob, pDeviceHandle->oDeviceID.cbBlob );
    *f_pcbDeviceID = pDeviceHandle->oDeviceID.cbBlob;


ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceAttr(
    IN     DRM_DEVICESTORE_HANDLE             f_hDeviceHandle,
    IN     DRM_BYTEBLOB                     f_AttrName,
       OUT DRM_BYTE                        *f_pbAttrValue,
    IN OUT DRM_DWORD                       *f_pcbAttrValue )
{
    DRM_DWORD                     dr = DRM_SUCCESS;
    _DeviceHandle                *pDeviceHandle = (_DeviceHandle*)f_hDeviceHandle;
    DRM_DWORD                     ibOffset = 0;
    DRM_DWORD                     ibName = 0;
    DRM_DWORD                     cbName = 0;
    DRM_DWORD                     ibContent = 0;
    DRM_DWORD                     cbContent = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceAttr );

    ChkArg( f_hDeviceHandle != NULL
        && f_pcbAttrValue != NULL
        && f_hDeviceHandle != NULL
        && f_AttrName.pbBlob != NULL
        && f_AttrName.cbBlob != 0
        && pDeviceHandle->pbEntry != NULL );

    do {
        ChkDR( _ReadDeviceStoreEntryItem(&ibName,
                    &cbName,
                    &ibContent,
                    &cbContent,
                    pDeviceHandle->pbEntry,
                    &ibOffset,
                    pDeviceHandle->cbEntry) );

        if( ( cbName == f_AttrName.cbBlob )
            && ( DRM_BYT_CompareBytes( pDeviceHandle->pbEntry,
                    ibName,
                    f_AttrName.pbBlob,
                    0,
                    f_AttrName.cbBlob ) == 0 ) )
        {

            if( *f_pcbAttrValue < cbContent )
            {
                *f_pcbAttrValue = cbContent;
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            *f_pcbAttrValue = cbContent;

            ChkArg( f_pbAttrValue != NULL );

            DRM_BYT_CopyBytes( f_pbAttrValue, 0, pDeviceHandle->pbEntry, ibContent, cbContent );

            break;
        }

    } while( dr == DRM_SUCCESS );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceAttrCount(
    IN     DRM_DEVICESTORE_HANDLE               f_hDeviceHandle,
       OUT DRM_DWORD                           *f_pdwAttrCount )
{
    DRM_DWORD                     dr = DRM_SUCCESS;
    _DeviceHandle                *pDeviceHandle = (_DeviceHandle*)f_hDeviceHandle;
    DRM_DWORD                     ibOffset = 0;
    DRM_DWORD                     ibName = 0;
    DRM_DWORD                     cbName = 0;
    DRM_DWORD                     ibContent = 0;
    DRM_DWORD                     cbContent = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceAttrCount );

    ChkArg( f_hDeviceHandle != NULL
        && f_pdwAttrCount != NULL
        && pDeviceHandle->pbEntry != NULL );

    if( DEVICESTORE_NUMATTRIBUTES_UNKNOWN == pDeviceHandle->dwNumAttributes )
    {
        *f_pdwAttrCount = 0;
        do {
            ChkDR( _ReadDeviceStoreEntryItem(&ibName,
                        &cbName,
                        &ibContent,
                        &cbContent,
                        pDeviceHandle->pbEntry,
                        &ibOffset,
                        pDeviceHandle->cbEntry) );


            (*f_pdwAttrCount)++;
        } while( dr == DRM_SUCCESS );

        /* Update our cache */
        pDeviceHandle->dwNumAttributes = (*f_pdwAttrCount);
    }
    else
    {
        *f_pdwAttrCount = pDeviceHandle->dwNumAttributes;
    }

ErrorExit:

    if( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



DRM_API DRM_RESULT DRM_CALL DRM_DVS_GetDeviceAttrByIndex(
    IN     DRM_DEVICESTORE_HANDLE             f_hDeviceHandle,
    IN     DRM_DWORD                        f_dwAttrIndex,
       OUT DRM_BYTE                        *f_pbAttrName,
    IN OUT DRM_DWORD                       *f_pcbAttrName,
       OUT DRM_BYTE                        *f_pbAttrValue,
    IN OUT DRM_DWORD                       *f_pcbAttrValue )
{
    DRM_DWORD                     dr = DRM_SUCCESS;
    _DeviceHandle                *pDeviceHandle = (_DeviceHandle*)f_hDeviceHandle;
    DRM_DWORD                     ibOffset = 0;
    DRM_DWORD                     ibName = 0;
    DRM_DWORD                     cbName = 0;
    DRM_DWORD                     ibContent = 0;
    DRM_DWORD                     cbContent = 0;
    DRM_DWORD                     dwIndex = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_GetDeviceAttrByIndex );

    ChkArg( f_hDeviceHandle != NULL );
    ChkArg( f_pcbAttrValue != NULL );
    ChkArg( f_pcbAttrName != NULL );
    ChkArg( f_dwAttrIndex >= 1 );
    ChkArg( pDeviceHandle->pbEntry != NULL );

    do {
        ChkDR( _ReadDeviceStoreEntryItem(&ibName,
                    &cbName,
                    &ibContent,
                    &cbContent,
                    pDeviceHandle->pbEntry,
                    &ibOffset,
                    pDeviceHandle->cbEntry) );

        if( dwIndex++ == f_dwAttrIndex )
        {

            if( *f_pcbAttrValue < cbContent )
            {
                *f_pcbAttrValue = cbContent;
                dr = DRM_E_BUFFERTOOSMALL;
            }

            if( *f_pcbAttrName < cbName )
            {
                *f_pcbAttrName = cbName;
                dr = DRM_E_BUFFERTOOSMALL;
            }
            ChkDR( dr );

            *f_pcbAttrValue = cbContent;
            *f_pcbAttrName = cbName;

            ChkArg( f_pbAttrValue != NULL
                && f_pbAttrName != NULL );

            DRM_BYT_CopyBytes( f_pbAttrValue, 0, pDeviceHandle->pbEntry, ibContent, cbContent );
            DRM_BYT_CopyBytes( f_pbAttrName, 0, pDeviceHandle->pbEntry, ibName, cbName );

            break;
        }

    } while( dr == DRM_SUCCESS );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_DVS_SetDeviceAttr(
    IN     DRM_DEVICESTORE_HANDLE           f_hDeviceHandle,
    IN     DRM_BYTEBLOB                     f_AttrName,
    IN     DRM_BYTEBLOB                     f_AttrValue )
{
    DRM_DWORD                     dr = DRM_SUCCESS;
    _DeviceHandle                *pDeviceHandle = (_DeviceHandle*)f_hDeviceHandle;
    DRM_DWORD                     ibOffset = 0;
    DRM_DWORD                     ibName = 0;
    DRM_DWORD                     cbName = 0;
    DRM_DWORD                     ibContent = 0;
    DRM_DWORD                     cbContent = 0;
    DRM_DWORD                     cbNewEntry = 0;
    DRM_BYTE                     *pbNew = NULL;
    DRM_DWORD                     dwIndex = 0;
    DRM_BOOL                      fFoundExisting = FALSE;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_SetDeviceAttr );

    ChkArg( f_hDeviceHandle        != NULL );
    ChkArg( f_AttrValue.pbBlob     != NULL );
    ChkArg( f_hDeviceHandle        != NULL );
    ChkArg( f_AttrName.pbBlob      != NULL );
    ChkArg( f_AttrName.cbBlob      != 0    );
    ChkArg( pDeviceHandle->pbEntry != NULL );

    /* They can't change the device ID */
    ChkBOOL( f_AttrName.cbBlob != SIZEOF( g_wszDeviceID )
        || MEMCMP( f_AttrName.pbBlob, g_wszDeviceID, SIZEOF( g_wszDeviceID ) ) != 0, DRM_E_INVALID_DEVSTORE_ATTRIBUTE );


    /* Check to see if there's already an attribute with this name */
    do {
        dr = _ReadDeviceStoreEntryItem(&ibName,
                    &cbName,
                    &ibContent,
                    &cbContent,
                    pDeviceHandle->pbEntry,
                    &ibOffset,
                    pDeviceHandle->cbEntry);

        if(    DRM_SUCCEEDED( dr )
            && ( cbName == f_AttrName.cbBlob )
            && ( DRM_BYT_CompareBytes( pDeviceHandle->pbEntry,
                    ibName,
                    f_AttrName.pbBlob,
                    0,
                    f_AttrName.cbBlob ) == 0 ) )
        {
            /* We found an existing entry for this attribute name */
            fFoundExisting = TRUE;
            break;
        }

        dwIndex++;
    } while( dr == DRM_SUCCESS );

    if( dr != DRM_E_NOMORE )
    {
        ChkDR( dr );
    }


    if( fFoundExisting )
    {
        /* Found an existing attribute with this name, so need to overwrite it
        */

        /* If the size of the attribute value has changed, we need to deal with it */
        if( cbContent != f_AttrValue.cbBlob )
        {
            ChkOverflow( pDeviceHandle->cbEntry, cbContent );
            ChkOverflow( pDeviceHandle->cbEntry - cbContent + f_AttrValue.cbBlob, pDeviceHandle->cbEntry - cbContent );
            cbNewEntry = pDeviceHandle->cbEntry - cbContent + f_AttrValue.cbBlob;

            if( cbContent < f_AttrValue.cbBlob )
            {
                /* We need to allocate a new buffer to hold the larger sized value */
                ChkMem( pbNew = (DRM_BYTE *)Oem_MemAlloc( cbNewEntry ) );

                /* Copy the old data into the new buffer */
                DRM_BYT_CopyBytes( pbNew, 0, pDeviceHandle->pbEntry, 0, ibContent );
                DRM_BYT_CopyBytes( pbNew, ibContent + f_AttrValue.cbBlob, pDeviceHandle->pbEntry, ibContent + cbContent,  pDeviceHandle->cbEntry - ibContent - cbContent );

                /* Free the old buffer and replace it with the new one */
                SAFE_OEM_FREE( pDeviceHandle->pbEntry );
                pDeviceHandle->pbEntry = pbNew;
                pbNew = NULL;
            }
            else
            {
                /* The new attribute is shorter than the old one, so we can use the same buffer.
                   However, we have to shift the entry back by however much the difference in size is*/
                DRM_BYT_MoveBytes( pDeviceHandle->pbEntry, ibContent + f_AttrValue.cbBlob, pDeviceHandle->pbEntry, ibContent + cbContent,  pDeviceHandle->cbEntry - ibContent - cbContent );
            }
            pDeviceHandle->cbEntry = cbNewEntry;


            /* Store the new value size */
            DWORD_TO_NETWORKBYTES( pDeviceHandle->pbEntry, ibContent - SIZEOF( DRM_DWORD ), f_AttrValue.cbBlob );
        }


        /* Store the value */
        DRM_BYT_CopyBytes( pDeviceHandle->pbEntry, ibContent, f_AttrValue.pbBlob, 0, f_AttrValue.cbBlob );
    }
    else
    {
        /* The attribute name doesn't exist yet, so let's create it
        */

        dwIndex--;
        /* Allocate a new buffer to hold the additional entry */
        cbNewEntry = pDeviceHandle->cbEntry + DEVICESTORE_ENTRY_SIZE( f_AttrName.cbBlob, f_AttrValue.cbBlob);
        ChkOverflow(cbNewEntry, pDeviceHandle->cbEntry);

        ChkMem( pbNew = (DRM_BYTE *)Oem_MemAlloc( cbNewEntry ) );
        DRM_BYT_CopyBytes( pbNew, 0, pDeviceHandle->pbEntry, 0, pDeviceHandle->cbEntry );

        /* Write out the new entry */
        ChkDR( _WriteDeviceStoreEntryItem( f_AttrName.pbBlob, f_AttrName.cbBlob, f_AttrValue.pbBlob, f_AttrValue.cbBlob, pbNew, pDeviceHandle->cbEntry ) );

        /* Free the old buffer and replace it with the new one */
        SAFE_OEM_FREE( pDeviceHandle->pbEntry );
        pDeviceHandle->pbEntry = pbNew;
        pDeviceHandle->cbEntry = cbNewEntry;

        /* Keep our attribute count up to date */
        if( DEVICESTORE_NUMATTRIBUTES_UNKNOWN != pDeviceHandle->dwNumAttributes )
        {
            pDeviceHandle->dwNumAttributes++;
        }

        pbNew = NULL;
    }

    /* Set the dirty flag */
    ChkDR( _SetDirtyFlag( pDeviceHandle, dwIndex ) );

ErrorExit:

    SAFE_OEM_FREE( pbNew );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;

}


DRM_API DRM_RESULT DRM_CALL DRM_DVS_CommitDevice(
    IN     DRM_DEVICESTORE_HANDLE             f_hDeviceHandle )
{
    DRM_RESULT                    dr            = DRM_SUCCESS;
    _DeviceStoreContext          *pDeviceStore = NULL;
    DRM_BYTE                      rgbPassword[DRM_SHA1_DIGEST_LEN];
    DRM_ID                        key = {0};
    _DeviceHandle                *pDeviceHandle = (_DeviceHandle *)f_hDeviceHandle;
    DRM_DWORD                     cbDataInStore = 0;
    _DeviceHandle                *pDeviceInStore = NULL;
    DRM_BYTEBLOB                  attrName = {NULL, 0};
    DRM_BYTEBLOB                  attrValue = {NULL, 0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DEVICESTORE, PERF_FUNC_DRM_DVS_CommitDevice );

    ChkArg( f_hDeviceHandle != NULL );
    ChkArg( pDeviceHandle->pbEntry != NULL
            && pDeviceHandle->cbEntry != 0
            && pDeviceHandle->oDeviceID.pbBlob != NULL
            && pDeviceHandle->oDeviceID.cbBlob != 0
            && pDeviceHandle->pDeviceStoreContext != NULL );

    pDeviceStore = pDeviceHandle->pDeviceStoreContext;

    ChkArg( pDeviceStore );

    ChkDR( _CalculateStoreID( pDeviceHandle->oDeviceID.pbBlob, pDeviceHandle->oDeviceID.cbBlob, &key ) );
    ChkDR( _CalculateKeyPassword( pDeviceStore->pBBXContext, &key, rgbPassword ) );



    ChkDR( DRM_SST_OpenAndLockSlot( pDeviceStore->pDatastore,
                                        SECURE_STORE_DEVICE_REGISTRATION_DATA,
                                        &key,
                                        &key,
                                        rgbPassword,
                                        DRM_SECURE_STORE_OPEN_ALWAYS,
                                        pDeviceStore->pSecureStoreContext,
                                        &cbDataInStore ) );



    /* Check if there is already data in the store */
    if( cbDataInStore != 0 )
    {
        DRM_DWORD i = 0;
        DRM_DWORD dwCount = 0;
        DRM_BOOL fDirty = FALSE;

        ChkMem( pDeviceInStore = (_DeviceHandle *)Oem_MemAlloc( SIZEOF( _DeviceHandle ) ) );
        ZEROMEM( pDeviceInStore, SIZEOF( _DeviceHandle ) );

        pDeviceInStore->cbEntry = cbDataInStore;
        ChkMem( pDeviceInStore->pbEntry = (DRM_BYTE *)Oem_MemAlloc( cbDataInStore ) );
        ChkDR( DRM_SST_GetLockedData( pDeviceStore->pSecureStoreContext, pDeviceInStore->pbEntry, &(pDeviceInStore->cbEntry) ) );

        if( pDeviceHandle->dwNumDirtyAttributes > 0 )
        {
            /* Since there are some dirty attributes, */
            /* we need to update the store with only the attributes that were changed */
            DRM_DVS_GetDeviceAttrCount( f_hDeviceHandle, &dwCount );
            for( i = 0; i < dwCount; i++ )
            {
                ChkDR( _GetDirtyFlag( pDeviceHandle, i, &fDirty ) );

                if( fDirty )
                {
                    attrName.cbBlob = 0;
                    attrValue.cbBlob = 0;
                    dr = DRM_DVS_GetDeviceAttrByIndex( f_hDeviceHandle, i, NULL, &(attrName.cbBlob), NULL, &(attrValue.cbBlob) );

                    if( dr != DRM_E_BUFFERTOOSMALL )
                    {
                        ChkDR( dr );
                    }

                    ChkMem( attrName.pbBlob = (DRM_BYTE *)Oem_MemAlloc( attrName.cbBlob ) );
                    ChkMem( attrValue.pbBlob = (DRM_BYTE *)Oem_MemAlloc( attrValue.cbBlob ) );

                    ChkDR( DRM_DVS_GetDeviceAttrByIndex( f_hDeviceHandle, i, attrName.pbBlob, &(attrName.cbBlob), attrValue.pbBlob, &(attrValue.cbBlob) ) );

                    ChkDR( DRM_DVS_SetDeviceAttr( (DRM_DEVICESTORE_HANDLE)pDeviceInStore, attrName, attrValue ) );

                    SAFE_OEM_FREE( attrName.pbBlob );
                    SAFE_OEM_FREE( attrValue.pbBlob );
                }
            }
        }

        /* We now update our cache with the latest merged data */
        SAFE_OEM_FREE( pDeviceHandle->pbEntry );
        pDeviceHandle->pbEntry = pDeviceInStore->pbEntry;
        pDeviceHandle->cbEntry = pDeviceInStore->cbEntry;

        pDeviceInStore->pbEntry = NULL;
        pDeviceInStore->cbEntry = 0;

        /* We don't know how many attributes there are now since we merged */
        pDeviceHandle->dwNumAttributes = DEVICESTORE_NUMATTRIBUTES_UNKNOWN;
    }

    /* If we changed anything, we need to save it into the store */
    if( pDeviceHandle->dwNumDirtyAttributes > 0 )
    {
        ChkDR( DRM_SST_SetLockedData( pDeviceStore->pSecureStoreContext, pDeviceHandle->cbEntry, pDeviceHandle->pbEntry ) );
    }

    /* We just committed to the store, so nothing's dirty anymore. All clean. */
    pDeviceHandle->dwNumDirtyAttributes = 0;

ErrorExit:
    if( pDeviceStore != NULL )
    {
        DRM_SST_CloseLockedSlot( pDeviceStore->pSecureStoreContext );
    }

    if( pDeviceInStore != NULL)
    {
        /* We only want to free the handle, not commit it */
        pDeviceInStore->fDontCommitOnFree = TRUE;
        DRM_DVS_FreeDevice( (DRM_DEVICESTORE_HANDLE) pDeviceInStore );
    }

    SAFE_OEM_FREE( attrName.pbBlob );
    SAFE_OEM_FREE( attrValue.pbBlob );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

