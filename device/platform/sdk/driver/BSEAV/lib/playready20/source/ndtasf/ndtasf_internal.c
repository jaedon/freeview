/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <ndtcontextsizes.h>
#include <ndtasf_internal.h>
#include <ndtasfguids.h>
#include <drmndtmanager.h>
#include <ndtencrypt.h>
#include <ndtasftypes.h>
#include <drmptrlist.h>
#include <asfsample.h>
#include <mux.h>
#include <asfmediabuffer.h>

ENTER_PK_NAMESPACE_CODE;


/*
 * The redirection key corresponds to the base64 encoding of the
 * GUID C3BEBD76-F892-4f26-8A30-558E001678C4.
 *
 * Notice that the DRM base64 encoding replaces the '+' with '!'
 * and the '/' with '*' (see the file drmbase64.c)
 */
#define WMDRM_ND_REDIRECTION_KEYID      { 'd', 'r', \
                                          '2', '!', \
                                          'w', '5', \
                                          'L', '4', \
                                          'J', 'k', \
                                          '!', 'K', \
                                          'M', 'F', \
                                          'W', 'O', \
                                          'A', 'B', \
                                          'Z', '4', \
                                          'x', 'A', \
                                          '=', '=', \
                                          '\0', '\0'}
#define WMDRM_ND_REDIRECTION_KEYID_SIZE 24
#define WMDRM_ND_REDIRECTION_URL        { 'h', 't', \
                                          't', 'p', \
                                          ':', '/', \
                                          '/', 'g', \
                                          'o', '.', \
                                          'm', 'i', \
                                          'c', 'r', \
                                          'o', 's', \
                                          'o', 'f', \
                                          't', '.', \
                                          'c', 'o', \
                                          'm', '/', \
                                          'f', 'w', \
                                          'l', 'i', \
                                          'n', 'k', \
                                          '/', '?', \
                                          'L', 'i', \
                                          'n', 'k', \
                                          'I', 'd', \
                                          '=', '2', \
                                          '8', '7', \
                                          '4', '5', \
                                          '\0', '\0'}
#define WMDRM_ND_REDIRECTION_URL_SIZE   44

#define WMDRM_ND_CONTENT_ENCRYPTION_OBJECT_SIZE  \
                ( MIN_OBJ_SIZE + 4 * SIZEOF( DRM_DWORD ) +    \
                  WMDRM_ND_REDIRECTION_KEYID_SIZE +     \
                  4 +                                   \
                  WMDRM_ND_REDIRECTION_KEYID_SIZE + 1 + \
                  WMDRM_ND_REDIRECTION_URL_SIZE + 1 )

#define WMDRM_ND_PAYLOAD_EXTENSION_SIZE     ( SIZEOF( DRM_GUID ) + \
                                              SIZEOF( DRM_WORD ) + \
                                              SIZEOF( DRM_DWORD ) )

#define ASF_STREAM_PROPERTIES_OBJECT_EX_SIZE     MIN_OBJ_SIZE + 64

#define WMDRM_ND_LICENSE_KEY_ID_SIZE        16

#define ASF_HEADER_EXTENSION_SIZE           46


/*
** Offset of object size from the beginning of an object
*/
#define ASF_OBJECT_SIZE_OFFSET 16

/*
** Offset of type specific data size from the beginning of the stream properties object
*/
#define STREAM_PROPERTIES_TYPE_SPECIFIC_SIZE_OFFSET 64

/*
** Size of the ASF WAVEFORMATEX struct in the audio stream properties object
** note that SIZEOF( ASF_WAVEFORMATEX ) != SIZEOF_ASF_WAVEFORMATEX due to
** alignment padding
*/
#define SIZEOF_ASF_WAVEFORMATEX 18

/*
** Offset of encryption bit from the beginning of a Stream properties object
*/
#define ASF_ENCRYPTION_BIT_OFFSET 48

/*
** Offset of format tag field from the beginning of a Stream properties object
*/
#define AUDIO_STREAM_WFX_FORMAT_TAG_OFFSET 78

/*
** Offset of WAVEFORMATEX 'cbSize' field (Audio stream) from the beginning of a Stream properties object
*/
#define WFX_CBSIZE_OFFSET  94

/*
** Offset of bitmap info header from the beginning of a Stream properties object
*/
#define VIDEO_STREAM_BMIH_OFFSET 89

/*
** Offset of compression format tag from the beginning of a bitmap info header
*/
#define BMIH_COMPRESSION_TAG_OFFSET 16

/*
** Offset of a video info header from the beginning of a stream properties object
*/
#define VIDEO_STREAM_VIH_SIZE_OFFSET 87

/*
** Value of the placeholder WAVEFORMATEX FormatTag for an Audio Stream in PYA files
*/
#define PMF_AUDIO_FORMATTAG   0x5052        /* "PR" */

/*
** Value of the placeholder Format Tag for a Video Stream in PYV files
*/
#define PMF_VIDEO_FORMATTAG   0x59445250    /* "PRDY" */


/****************************************************************************/
static DRM_RESULT _AddToStreamInfoList(
    __in       NDT_ASF_CONTEXT * const pAsfContext,
    __in const DRM_DWORD               dwStreamNumber,
    __in const DRM_DWORD               dwBitRate,
    __in const DRM_GUID                guidStreamType,
    __out_opt  ASF_STREAM_INFO       **ppStreamInfo)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_STREAM_INFO *pNew = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__AddToStreamInfoList );

    DRMASSERT( NULL != pAsfContext );

    ChkMem( pNew = (ASF_STREAM_INFO*)Oem_MemAlloc( SIZEOF(ASF_STREAM_INFO) ) );
    ZEROMEM( pNew, SIZEOF(ASF_STREAM_INFO) );

    pNew->wStreamNumber = (DRM_WORD) dwStreamNumber;
    pNew->dwAverageBitrate = dwBitRate;
    pNew->guidStreamType = guidStreamType;
    pNew->dwMaxBitrate = 0;
    pNew->msMaxBufferWindow = 0;

    ChkDR( DRM_PtrList_AddTail(&pAsfContext->StreamInfoList, pNew) );

    if ( ppStreamInfo )
    {
        *ppStreamInfo = pNew;
    }

    /*
    ** Set the local ASF_STREAM_INFO pointer to NULL, so that the SAFE_OEM_FREE
    ** macro below does not deallocate the memory in this success path.
    */
    pNew = NULL;

 ErrorExit:
    SAFE_OEM_FREE(pNew);
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _FindStreamInStreamInfoList(
    __in const NDT_ASF_CONTEXT * const pAsfContext,
    __in const DRM_DWORD               dwStreamNumber,
    __out      ASF_STREAM_INFO       **ppInfo
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PTR_LIST_ENTRY  *pList;
    ASF_STREAM_INFO *pInfo;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__FindStreamInStreamInfoList );

    ChkArg( NULL != pAsfContext );
    ChkArg( NULL != ppInfo );

    for (pList = pAsfContext->StreamInfoList.pHead; NULL != pList; pList = pList->pNext)
    {
        pInfo = (ASF_STREAM_INFO *) pList->pbData;
        if ( (pInfo != NULL) && (dwStreamNumber == (DRM_DWORD) pInfo->wStreamNumber) )
        {
            *ppInfo = pInfo;
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }
    }

    *ppInfo = NULL;
    ChkDR( DRM_E_FAIL );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_VOID _FreeSamples(
    __inout NDT_ASF_CONTEXT * const pAsfContext
)
{
    PTR_LIST_ENTRY     *pEntry;
    ASF_SAMPLE *pSample = NULL;

    if ( NULL != pAsfContext && NULL != pAsfContext->Samples.pHead )
    {
        for (pEntry = pAsfContext->Samples.pHead; NULL != pEntry; pEntry = pEntry->pNext)
        {
            pSample = (ASF_SAMPLE *) pEntry->pbData;
            if ( NULL != pSample )
            {
                ASFSample_Release( pSample );
                pSample = NULL;
            }
            pEntry->pbData = NULL;
        }

        DRM_PtrList_Uninitialize( &pAsfContext->Samples );
    }

    return;
}



static DRM_RESULT _GetNextReadySample(
    __inout NDT_ASF_CONTEXT * const pAsfContext,
    __out   ASF_SAMPLE            **ppSample,
    __out   DRM_BOOL        * const pfReady
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD   i;
    ASF_SAMPLE *pSample = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__GetNextReadySample );

    ChkArg( NULL != pAsfContext );
    ChkArg( NULL != ppSample );
    ChkArg( NULL != pfReady );

    *pfReady = FALSE;

    for ( i = 0; i < pAsfContext->Samples.dwCurrentSize; i++ )
    {
        ChkDR( DRM_PtrList_GetByIndex( &pAsfContext->Samples, i, (DRM_VOID **) &pSample ) );

        if ( ASF_SAMPLE_READY & pSample->dwFlags )
        {
            *ppSample = pSample;
            ChkDR( ASFSample_AddRef( *ppSample ) );
            *pfReady = TRUE;
            break;
        }
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



static DRM_RESULT _GetSampleInProgress(
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __in const DRM_DWORD               dwStreamNumber,
    __in const DRM_DWORD               dwObjectPres,
    __out      ASF_SAMPLE            **ppSample
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i;
    ASF_SAMPLE *pSample = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__GetSampleInProgress );

    ChkArg( NULL != pAsfContext );

    for ( i = 0; i < pAsfContext->Samples.dwCurrentSize; i++ )
    {
        ChkDR( DRM_PtrList_GetByIndex( &pAsfContext->Samples, i, (DRM_VOID **) &pSample ) );

        if ( dwStreamNumber == pSample->dwStreamNumber &&
             DRM_UI64Eql( DRM_UI64Mul(DRM_UI64(dwObjectPres),
                                      DRM_UI64(10000)),
                          pSample->hnsSampleTime) )
        {
            DRMASSERT( 0 == (pSample->dwFlags & ASF_SAMPLE_READY) );
            *ppSample = pSample;
            ChkDR( ASFSample_AddRef( *ppSample ) );
            goto ErrorExit;
        }
    }

    ChkDR( DRM_E_FAIL );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


static DRM_BOOL _RemoveSample(
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __in const ASF_SAMPLE      * const pSampleToRemove
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i;
    DRM_BOOL fFound   = FALSE;
    ASF_SAMPLE *pSample = NULL;


    DRMASSERT(NULL != pAsfContext);

    ChkBOOL( NULL != pAsfContext->Samples.pHead, DRM_E_FAIL );

    for (i = 0; i < pAsfContext->Samples.dwCurrentSize; i++ )
    {
        ChkDR( DRM_PtrList_GetByIndex( &pAsfContext->Samples, i, (DRM_VOID **) &pSample ) );

        if ( pSample == pSampleToRemove )
        {
            ChkDR( DRM_PtrList_RemoveByIndex( &pAsfContext->Samples, i, (DRM_VOID **) &pSample ) );
            if ( NULL != pSample )
            {
                ASFSample_Release( pSample );
                pSample = NULL;
            }
            fFound = TRUE;
            break;
        }

    }
    DRMASSERT( fFound );

 ErrorExit:
    return fFound;
}



/****************************************************************************/
static DRM_DWORD WMCBGetData(
    __inout    NDT_ASF_CONTEXT  * const pAsfContext,
    __in const DRM_UINT64               nOffset,
    __in const DRM_DWORD                nNumBytesRequest,
    __out      DRM_BYTE               **ppData )
{
    DRM_DWORD cbRead = 0;

    if ( nNumBytesRequest > FILE_BUFFER_SIZE )
    {
        DRMASSERT( nNumBytesRequest <= FILE_BUFFER_SIZE );
        *ppData = NULL;
        return (DRM_DWORD)0;
    }

    if( pAsfContext->hInFile != OEM_INVALID_HANDLE_VALUE )
    {
        if ( ! Oem_File_SetFilePointer( pAsfContext->hInFile, DRM_UI64Low32( nOffset) , OEM_FILE_BEGIN, NULL ) )
        {
            *ppData = NULL;
            return (DRM_DWORD)0;
        }

        if ( ! Oem_File_Read( pAsfContext->hInFile, pAsfContext->pbFileBuffer,  nNumBytesRequest, &cbRead ) )
        {
            *ppData = NULL;
            return (DRM_DWORD)0;
        }

        *ppData = pAsfContext->pbFileBuffer;
    }

    return cbRead;
}



/****************************************************************************/
static DRM_RESULT _ASFReadData(
    __in                           OEM_FILEHDL  hInFile,
    __out_bcount(FILE_BUFFER_SIZE) DRM_BYTE    *pbFileBuffer,
    __in const                     DRM_DWORD    nNumBytesRequest
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbRead = 0;
    DRM_BOOL fRead;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFReadData );

    ChkArg ( nNumBytesRequest <= FILE_BUFFER_SIZE &&
             NULL != pbFileBuffer &&
             OEM_INVALID_HANDLE_VALUE != hInFile );

    fRead = Oem_File_Read(hInFile, pbFileBuffer, nNumBytesRequest, &cbRead);
    ChkBOOL( fRead && (cbRead == nNumBytesRequest), DRM_E_FAIL );

 ErrorExit:
    TRACE_IF_FAILED(("ASFReadData failed 0x%lx\n", dr));
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _AppendToHeaderBuffer(
    __inout                       ASF_TRANSCRYPT_CONTEXT * const pAsfTranscrypt,
    __in_bcount(dwDataSize) const DRM_BYTE               * const pData,
    __in const                    DRM_DWORD                      dwDataSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_TRANSCRYPT_HEADER_CONTEXT *pHeader = &(pAsfTranscrypt->Header);

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__AppendToHeaderBuffer );

    ChkOverflow( pHeader->dwHeaderOffset + dwDataSize, dwDataSize );
    if ( pHeader->dwHeaderOffset + dwDataSize > pHeader->cbHeaderSize )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    DRM_BYT_CopyBytes(pHeader->pbHeader, pHeader->dwHeaderOffset, pData, 0, dwDataSize);

    pHeader->dwHeaderOffset += dwDataSize;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _CopyObjectToHeaderBuffer(
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __in const DRM_UINT64              nOffset,
    __in const DRM_DWORD               nNumBytesRequest )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_DWORD   dwNowWanted;
    DRM_DWORD   dwAskedFor;
    DRM_DWORD   dwThisBuffer;
    DRM_DWORD   dwTotalUsed = 0;
    DRM_BYTE   *pData;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__CopyObjectToHeaderBuffer );

    dwNowWanted = nNumBytesRequest;
    while ( dwNowWanted )
    {
        dwAskedFor = dwNowWanted > FILE_BUFFER_SIZE ? FILE_BUFFER_SIZE : dwNowWanted;
        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(nOffset, DRM_UI64(dwTotalUsed)), dwAskedFor, &pData );
        if ( dwThisBuffer != dwAskedFor )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }
        dwTotalUsed += dwThisBuffer;
        ChkDR( _AppendToHeaderBuffer( &pAsfContext->ASFTranscryptContext, pData, dwThisBuffer ) );
        dwNowWanted -= dwThisBuffer;
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _AppendToDataBuffer(
    __inout                       ASF_TRANSCRYPT_CONTEXT * const pAsfTranscrypt,
    __in_bcount(dwDataSize) const DRM_BYTE               * const pData,
    __in const                    DRM_DWORD                      dwDataSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_DATA_CONTEXT *pDataContext = &(pAsfTranscrypt->Data.DataContext);


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__AppendToDataBuffer );

    ChkOverflow( pDataContext->dwDataBufferOffset + dwDataSize, dwDataSize );
    if ( pDataContext->dwDataBufferOffset + dwDataSize > pDataContext->cbDataBufferSize )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    DRM_BYT_CopyBytes(pDataContext->pbDataBuffer,
                      pDataContext->dwDataBufferOffset,
                      pData,
                      0,
                      dwDataSize);

    pDataContext->dwDataBufferOffset += dwDataSize;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _SaveHeaderObjectToNewHeader(
    __inout ASF_TRANSCRYPT_CONTEXT * const pAsfTranscrypt,
    __out   DRM_GUID               * const pObjId,
    __out   DRM_UINT64             * const pqwObjSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pHeaderData;
    ASF_TRANSCRYPT_HEADER_CONTEXT *pHeader = &(pAsfTranscrypt->Header);
    DRM_DWORD dwOffset;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__SaveHeaderObjectToNewHeader );

    ChkOverflow( pHeader->dwHeaderOffset + SIZEOF( DRM_GUID ) + SIZEOF( DRM_UINT64 ), pHeader->dwHeaderOffset );
    if ( pHeader->dwHeaderOffset + SIZEOF( DRM_GUID ) + SIZEOF( DRM_UINT64 ) > pHeader->cbHeaderSize )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    pHeaderData = pHeader->pbHeader;
    dwOffset = pHeader->dwHeaderOffset;

    SaveGUIDOffset( pHeaderData, *pObjId, dwOffset );
    SaveQWORDOffset( pHeaderData, *pqwObjSize, dwOffset );

    pHeader->dwHeaderOffset += SIZEOF( DRM_GUID ) + /* Header object GUID */
                               SIZEOF( DRM_UINT64 );       /* Object size */

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFLoadHeaderObject(
    __inout NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_GUID objectId;
    DRM_UINT64 qwSize;
    DRM_DWORD dwHeaders = 0;
    DRM_BYTE  bAlign = 0;
    DRM_BYTE  bArch = 0;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwWanted =0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwOffset;
    ASF_PARSER_STATE *pState = NULL;
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFLoadHeaderObject );

    ChkArg( pAsfContext );

    pState = &(pAsfContext->ParserState);

    dwWanted = MIN_OBJ_SIZE +       /* GUID + object size */
               SIZEOF( DRM_DWORD ) +      /* */
               2;    /* */

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_I2UI64( pState->iCurrentFileOffset ), dwWanted, &pData );
    if ( dwThisBuffer != dwWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwOffset = 0;
    LoadGUIDOffset( objectId, pData, dwOffset );
    LoadQWORDOffset( qwSize, pData, dwOffset );
    LoadDWORDOffset( dwHeaders, pData, dwOffset );

    bAlign = GET_BYTE(pData, dwOffset);
    dwOffset++;
    bArch = GET_BYTE(pData, dwOffset);
    dwOffset++;

    if( ! DRM_IsEqualGUID( &CLSID_ASFHeaderObjectV0, &objectId ) ||
        ( 1 != bAlign ) ||
        ( 2 != bArch ) )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    pState->qwHeaderSize = qwSize;
    pState->cbOriginalHeaderSize = DRM_UI64Low32(qwSize);
    pState->dwNoOfHeaders = dwHeaders;
    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwThisBuffer ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFLoadObjectHeader(
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __out      DRM_GUID        * const pObjectId,
    __out      DRM_UINT64      * const pqwSize,
    __in const DRM_UINT64              PrevOffset
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwWanted = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwOffset = 0;
    ASF_PARSER_STATE *pState = &(pAsfContext->ParserState);


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFLoadObjectHeader );

    dwWanted = MIN_OBJ_SIZE;

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add( DRM_I2UI64( pState->iCurrentFileOffset ), PrevOffset ), dwWanted, &pData );
    if ( dwThisBuffer != dwWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwOffset = 0;
    LoadGUIDOffset( *pObjectId, pData, dwOffset );
    LoadQWORDOffset( *pqwSize, pData, dwOffset );

    /* if (*pqwSize) < MIN_OBJ_SIZE || ( (*pqwSize) - MIN_OBJ_SIZE + pState->iCurrentFileOffset > pState->qwHeaderSize ) */
    if ( DRM_UI64Les( *pqwSize, DRM_UI64( MIN_OBJ_SIZE ) ) ||
         DRM_UI64Les( pState->qwHeaderSize,
                      DRM_UI64Add( DRM_UI64Sub( *pqwSize, DRM_UI64( MIN_OBJ_SIZE ) ),
                                   DRM_I2UI64(pState->iCurrentFileOffset) ) ) )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    if ( DRM_UI64Eql(PrevOffset, DRM_UI64(0)) )
    {
        pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwWanted ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFProcessContentEncryptionObject(
    __inout NDT_ASF_CONTEXT * const pAsfContext,
    __in    DRM_DWORD               dwObjSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE bKeyId[] = WMDRM_ND_REDIRECTION_KEYID;
    DRM_BYTE bRedirUrl[] = WMDRM_ND_REDIRECTION_URL;
    DRM_BYTE bDrm[] = { 'D', 'R', 'M', '\0' };
    DRM_DWORD dwOffset = 0;
    DRM_BOOL  fTranscrypting; /* FIXME REMOVE */
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;
    DRM_BYTE    *pHeaderData;
    DRM_UINT64 qwSaveSize;
    DRM_DWORD dwSaveSize;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessContentEncryptionObject );

    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    fTranscrypting = TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode;

    dwObjSize -= MIN_OBJ_SIZE;

    if ( TRUE == fTranscrypting )
    {
        /*
         * We don't necessarily process the Content Encryption Object
         * We will just skip it and generate a new one in the new header
         * buffer
         */

        dwOffset = pAsfTranscryptContext->Header.dwHeaderOffset;
        pHeaderData = pAsfTranscryptContext->Header.pbHeader;

        /* Object header */
        SaveGUIDOffset( pHeaderData, CLSID_ASFContentEncryptionObject, dwOffset );
        qwSaveSize = DRM_UI64( WMDRM_ND_CONTENT_ENCRYPTION_OBJECT_SIZE );
        SaveQWORDOffset( pHeaderData, qwSaveSize, dwOffset );

        /* Secret Data */
        dwSaveSize = WMDRM_ND_REDIRECTION_KEYID_SIZE;
        SaveDWORDOffset( pHeaderData, dwSaveSize, dwOffset );
        DRM_BYT_CopyBytes( pHeaderData, dwOffset, bKeyId, 0, WMDRM_ND_REDIRECTION_KEYID_SIZE );

        dwOffset += dwSaveSize;

        /* Protection Type */
        dwSaveSize = 4;
        SaveDWORDOffset( pHeaderData, dwSaveSize, dwOffset );
        DRM_BYT_CopyBytes(pHeaderData, dwOffset, bDrm, 0, 4);

        dwOffset += dwSaveSize;

        /* Key ID */
        /* Include '\0' this time */
        dwSaveSize = WMDRM_ND_REDIRECTION_KEYID_SIZE + 1;
        SaveDWORDOffset( pHeaderData, dwSaveSize, dwOffset );
        DRM_BYT_CopyBytes( pHeaderData, dwOffset, bKeyId, 0, WMDRM_ND_REDIRECTION_KEYID_SIZE + 1 );

        dwOffset += dwSaveSize;

        /* License URL */
        dwSaveSize = WMDRM_ND_REDIRECTION_URL_SIZE + 1;
        SaveDWORDOffset( pHeaderData, dwSaveSize, dwOffset );
        DRM_BYT_CopyBytes( pHeaderData, dwOffset, bRedirUrl, 0, WMDRM_ND_REDIRECTION_URL_SIZE + 1 );

        dwOffset += dwSaveSize;

        pAsfTranscryptContext->Header.dwHeaderOffset += WMDRM_ND_CONTENT_ENCRYPTION_OBJECT_SIZE;

    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFProcessFilePropertiesObject(
    __in_opt   DRM_VOID        * const f_pOEMContext,
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __in       DRM_DWORD               dwObjSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *pHeaderData = NULL;
    DRM_DWORD dwWanted = 0;
    DRM_DWORD dwFlags = 0;
    ASF_PARSER_STATE *pState;
    DRM_DWORD dwSaveSize = 0;
    DRM_UINT64 qwSaveSize;
    DRMFILETIME ftCreationDate;
    DRM_UINT64         qwCreationDate;
    DRM_DWORD dwOffset = 0;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessFilePropertiesObject );

    pState = &(pAsfContext->ParserState);

    dwObjSize -= MIN_OBJ_SIZE;
    dwWanted = dwObjSize;

    dwOffset = pAsfTranscryptContext->Header.dwHeaderOffset;
    pHeaderData = pAsfTranscryptContext->Header.pbHeader;

    Oem_Random_GetBytes( f_pOEMContext,
                         (DRM_BYTE *)&(pAsfTranscryptContext->guidFileID),
                         SIZEOF( DRM_GUID ) );
    SaveGUIDOffset( pHeaderData, pAsfTranscryptContext->guidFileID, dwOffset);

    /* Regenerate File size */
    if ( 1 == pAsfContext->dwNoOfStreams && TRUE == pAsfContext->fAudioOnly )
    {
        qwSaveSize = DRM_UI64Add(
                         DRM_UI64( pAsfTranscryptContext->Header.cbHeaderSize ),
                         DRM_UI64Mul( DRM_UI64(pState->dwPacketSize +
                                           pAsfContext->dwAesPayloadExtensionAdditionalBytes ),
                                      DRM_UI64(pState->dwTotalPackets) ) );
    }
    else
    {
        qwSaveSize = DRM_UI64Add( DRM_UI64( pAsfTranscryptContext->Header.cbHeaderSize ),
                                  DRM_UI64( 50 ) );
    }

    SaveQWORDOffset( pHeaderData, qwSaveSize, dwOffset );

    /* Regenerate Creation Date */
    Oem_Clock_GetSystemTimeAsFileTime( f_pOEMContext, &ftCreationDate );
    FILETIME_TO_UI64( ftCreationDate, qwCreationDate );
    SaveQWORDOffset( pHeaderData, qwCreationDate, dwOffset );

#if DRM_SUPPORT_AUDIO_ONLY == 0
    if ( !( TRUE == pAsfContext->fAudioOnly && 1 == pAsfContext->dwNoOfStreams ) )
    {
        /* Set number of packets to -1 (unknown) */
        DRM_UINT64 qwTmp = DRM_UI64HL( 0, (DRM_DWORD) -1 );

        SaveQWORDOffset( pHeaderData, qwTmp, dwOffset );
    }
    else
#endif
    {
        SaveQWORDOffset(pHeaderData,pAsfContext->qwDataPacketsCount, dwOffset);
    }

    /* Save play Duration, Send Duration and Preroll */
    SaveQWORDOffset(pHeaderData, pAsfContext->qwDuration, dwOffset );
    SaveQWORDOffset(pHeaderData, pAsfContext->qwSendDuration, dwOffset );
    SaveQWORDOffset(pHeaderData, pAsfContext->qwPreroll, dwOffset );

    dwFlags = pAsfContext->dwFlags;

#if DRM_SUPPORT_AUDIO_ONLY == 0
    if ( !( TRUE == pAsfContext->fAudioOnly && 1 == pAsfContext->dwNoOfStreams ) )
    {
        /* Set broadcast flag */
        dwFlags |= 1;

        /* Seeking is not currently supported for video files.
         * Unset the seekable flag.
         */
        dwFlags &= 0xfffffffd;

    }
#endif
    SaveDWORDOffset( pHeaderData, dwFlags, dwOffset );

   /* Adjust Min and Max packet size and bitrate
    * (extra bytes for sample ID size (WORD) + sample ID itself(QWORD))
    */
    if ( 1 == pAsfContext->dwNoOfStreams
         && TRUE == pAsfContext->fAudioOnly )
    {
        pAsfContext->dwOutputPacketSize = pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes;
    }
    else
    {
        pAsfContext->dwOutputPacketSize = pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes * pAsfContext->dwNoOfStreams;
    }

    /* Update packet sizes and max bit rate in the header.
     * NewMaxBitRate = OldMaxBitRate * NewPacketSize / OldPacketSize
     */
    SaveDWORDOffset( pHeaderData, pAsfContext->dwOutputPacketSize, dwOffset );
    SaveDWORDOffset( pHeaderData, pAsfContext->dwOutputPacketSize, dwOffset );

    dwSaveSize = pState->dwMaxBitrate * pAsfContext->dwOutputPacketSize / pState->dwPacketSize;
    SaveDWORDOffset( pHeaderData, dwSaveSize, dwOffset );

    pAsfTranscryptContext->Header.dwHeaderOffset = dwOffset;

    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwWanted ) );

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



static DRM_RESULT _ASFParseFilePropertiesObject(
    __inout NDT_ASF_CONTEXT * const pAsfContext,
    __in    DRM_DWORD               dwObjSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwWanted = 0;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwNowWanted = 0;
    DRM_DWORD dwPacketSize = 0;
    DRM_DWORD dwMaxBitRate = 0;
    ASF_PARSER_STATE *pState = NULL;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;
    DRM_DWORD dwOffset = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseFilePropertiesObject );

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);

    dwObjSize -= MIN_OBJ_SIZE;
    dwWanted = dwObjSize;

    dwNowWanted = SIZEOF( DRM_GUID );
    dwTotalUsed += dwNowWanted;

    dwOffset += SIZEOF( DRM_GUID );

    /* Read File Size (DRM_UINT64), Creation Date (DRM_UINT64), Data Packets Count (DRM_UINT64),
     * Play Duration (DRM_UINT64), Send Duration (DRM_UINT64), Preroll (DRM_UINT64) and Flags (U32)
     */
    dwNowWanted = 6 * SIZEOF( DRM_UINT64 ) + SIZEOF( DRM_DWORD );
    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
        TRACE(("Stated size (%d + %d > %d) not large enough\n", dwTotalUsed, dwNowWanted, dwObjSize));
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwTotalUsed += dwNowWanted;

    dwOffset = 0;
    LoadQWORDOffset( pAsfTranscryptContext->Header.qwFileSize, pData, dwOffset );

    /* Skip creation date */
    dwOffset += SIZEOF(DRM_UINT64);

    /* Read number of packets */
    LoadQWORDOffset( pAsfContext->qwDataPacketsCount, pData, dwOffset );

    /* Read Play Duration, Send Duration and Preroll */
    LoadQWORDOffset( pAsfContext->qwDuration, pData, dwOffset );
    LoadQWORDOffset( pAsfContext->qwSendDuration, pData, dwOffset );
    LoadQWORDOffset( pAsfContext->qwPreroll, pData, dwOffset );

    LoadDWORDOffset( pAsfContext->dwFlags, pData, dwOffset );
    /* Read Min and Max packet size properties */
    dwNowWanted = 3 * SIZEOF( DRM_DWORD );
    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwTotalUsed += dwNowWanted;

    /* Read max/min packet size and max bit rate */
    dwOffset = 0;
    LoadDWORDOffset( dwPacketSize, pData, dwOffset );
    LoadDWORDOffset( dwPacketSize, pData, dwOffset );
    LoadDWORDOffset( dwMaxBitRate, pData, dwOffset );

    /* Save original packet size and number of packets and max bitrate */
    pState->dwPacketSize = dwPacketSize;
    pState->dwTotalPackets = DRM_UI64Low32( pAsfContext->qwDataPacketsCount );
    pState->dwMaxBitrate = dwMaxBitRate;

    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwWanted ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


static DRM_RESULT _ASFParsePlayReadyObject(
    __inout NDT_ASF_CONTEXT * const pAsfContext,
    __inout ASF_PARSER_STATE       *pState,
    __in    DRM_DWORD               dwSize,
    __in const DRM_UINT64           ui64PrevOffset
)
{
    DRM_RESULT     dr           = DRM_SUCCESS;
    DRM_BYTE      *pData        = NULL;
    DRM_DWORD      dwWanted     = 0;
    DRM_DWORD      dwThisBuffer = 0;
    ASF_FILE_INFO *pFileInfo    = NULL;
    DRM_DWORD      dwObjSize    = dwSize;

    ChkArg( pAsfContext != NULL );
    ChkArg( pState != NULL );

    pFileInfo = &pAsfContext->ASFFileInfo;
    pState->cbPlayReadyObject = dwSize;
    pState->fEncrypted = TRUE;

    ChkBOOL( dwObjSize >= MIN_OBJ_SIZE, DRM_E_ASF_BAD_ASF_HEADER );

    ChkMem( pFileInfo->pPlayReadyObj = (ASF_PLAYREADY_OBJECT *)Oem_MemAlloc( SIZEOF( ASF_PLAYREADY_OBJECT ) ) );
    ZEROMEM( pFileInfo->pPlayReadyObj, SIZEOF( ASF_PLAYREADY_OBJECT ) );

    /* Read the whole object into memory */
    dwWanted = dwObjSize - MIN_OBJ_SIZE;
    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add( DRM_I2UI64(pState->iCurrentFileOffset), ui64PrevOffset ), dwWanted, &pData );
    if ( dwThisBuffer != dwWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    ChkMem( pFileInfo->pPlayReadyObj->pbData = (DRM_BYTE*)Oem_MemAlloc( dwWanted ) );
    pFileInfo->pPlayReadyObj->cbData = dwWanted;

    MEMCPY( pFileInfo->pPlayReadyObj->pbData, pData, dwWanted );

ErrorExit:
    return dr;
}


/****************************************************************************/

static DRM_RESULT _ASFProcessStreamPropertiesObject(
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __in       DRM_DWORD               dwObjSize,
    __in const DRM_UINT64              qwOffset
)
{
    DRM_RESULT         dr             = DRM_SUCCESS;
    ASF_PARSER_STATE  *pState         = NULL;
    DRM_BYTE          *pData          = NULL;
    ASF_STREAM_INFO   *pStreamInfo    = NULL;
    DRM_DWORD          dwThisBuffer   = 0;
    DRM_WORD           wStreamNum     = 0;
    DRM_DWORD          dwStreamOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessStreamPropertiesObject );

    ChkArg( pAsfContext != NULL);
    ChkBOOL( dwObjSize >= ASF_ENCRYPTION_BIT_OFFSET + SIZEOF( DRM_WORD ), DRM_E_ASF_BAD_ASF_HEADER );

    pState = &(pAsfContext->ParserState);

    dwThisBuffer = WMCBGetData( pAsfContext,
                                DRM_UI64Sub(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), qwOffset ), DRM_UI64(MIN_OBJ_SIZE)),
                                dwObjSize + MIN_OBJ_SIZE,
                                &pData );
    if ( dwThisBuffer != dwObjSize + MIN_OBJ_SIZE )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    /*
    ** If we are encrypting clear content, or if we are processing a PMF file, then
    ** set the encrypted bit in the stream number WORD to TRUE and save the WORD back.
    ** (Although already encrypted with AES, a PMF file has this bit set to FALSE.)
    */
    if ( ( pAsfContext->fEncryptClear )
      || ( pAsfContext->fIsPlayReady ) )  /* a PMF file */
    {
        dwStreamOffset = ASF_ENCRYPTION_BIT_OFFSET + MIN_OBJ_SIZE;
        LoadWORDOffset( wStreamNum, pData, dwStreamOffset );
        wStreamNum |= 0x8000;

        dwStreamOffset = ASF_ENCRYPTION_BIT_OFFSET + MIN_OBJ_SIZE;
        SaveWORDOffset( pData, wStreamNum, dwStreamOffset );
    }

    /*
    ** If we are processing a PMF file, then we must shrink the size of this object
    ** by either two (if audio) or four (if video) bytes to account for the "extra" format tag
    ** skipped-over below.
    */
    if ( pAsfContext->fIsPlayReady ) /* a PMF file */
    {
        DRM_DWORD          cbFirstHalf        = 0;
        DRM_DWORD          cbSecondHalf       = 0;
        DRM_UINT64         cbStreamObject     = DRM_UI64LITERAL(0,0);
        DRM_UINT64         cbHeaderObject     = DRM_UI64LITERAL(0,0);
        DRM_DWORD          cbTypeSpecificData = 0;

        /*
        ** Get the stream info using the stream number obtained above.
        */
        ChkDR( _FindStreamInStreamInfoList(pAsfContext, (DRM_DWORD)(wStreamNum & 0x7f), &pStreamInfo) );

        dwStreamOffset = ASF_OBJECT_SIZE_OFFSET;
        LoadQWORDOffset( cbStreamObject, pData, dwStreamOffset );

        dwStreamOffset = STREAM_PROPERTIES_TYPE_SPECIFIC_SIZE_OFFSET;
        LoadDWORDOffset( cbTypeSpecificData, pData, dwStreamOffset );

        if ( DRM_IsEqualGUID( &CLSID_ASFStreamTypeAudioMedia, &(pStreamInfo->guidStreamType) ) )
        {
            DRM_WORD           wWFXcbSize     = 0;
            DRM_DWORD          dwOffsetToOriginalWFXFormat = 0;
            DRM_WORD           wWFXwFormatTag = 0;

            /*
            ** If we are processing a PMF file, then the FormatTag has been replaced with "PR" (0x5052).
            ** The real format tag has been added as the last two bytes of the Codec Specific Data,
            ** which comes immediately after the 'cbSize' field (the size of the Codec Specific Data).
            ** The following steps undo this.
            */

            /* Update the ASF header object size -
            ** Because the ASF header object header has already been written
            ** to the header buffer by ProcessHeaders, we have to reach backward in the
            ** header buffer and update the header size.
            */
            pAsfContext->ASFTranscryptContext.Header.cbHeaderSize -= SIZEOF( DRM_WORD );
            cbHeaderObject = DRM_UI64( pAsfContext->ASFTranscryptContext.Header.cbHeaderSize );
            dwStreamOffset = ASF_OBJECT_SIZE_OFFSET;
            SaveQWORDOffset( pAsfContext->ASFTranscryptContext.Header.pbHeader,
                             cbHeaderObject,
                             dwStreamOffset );

            /* Update the stream properties object size -
            ** Because the stream properties object header (length MIN_OBJ_SIZE) has already been written
            ** to the header buffer by _SaveHeaderObjectToNewHeader, we have to reach backward in the
            ** header buffer and update the object size.
            */
            cbStreamObject = DRM_UI64Sub( cbStreamObject, DRM_UI64( SIZEOF( DRM_WORD ) ) );  /* subtract 2 bytes since we're removing the "PR" tag */
            dwStreamOffset = pAsfContext->ASFTranscryptContext.Header.dwHeaderOffset
                             - MIN_OBJ_SIZE
                             + ASF_OBJECT_SIZE_OFFSET;
            SaveQWORDOffset( pAsfContext->ASFTranscryptContext.Header.pbHeader, cbStreamObject, dwStreamOffset );

            /* Update the format specific data size in the stream properties object */
            cbTypeSpecificData -= SIZEOF( DRM_WORD );
            dwStreamOffset = STREAM_PROPERTIES_TYPE_SPECIFIC_SIZE_OFFSET;
            SaveDWORDOffset( pData, cbTypeSpecificData, dwStreamOffset );

            /*
            ** Load the format tag, validate that it's PlayReady,
            ** then replace it with the original real format tag
            */
            ChkBOOL( pStreamInfo->WFX.wFormatTag == PMF_AUDIO_FORMATTAG, DRM_E_ASF_BAD_ASF_HEADER );
            dwOffsetToOriginalWFXFormat = AUDIO_STREAM_WFX_FORMAT_TAG_OFFSET
                                          + SIZEOF_ASF_WAVEFORMATEX
                                          + pStreamInfo->WFX.cbSize
                                          - SIZEOF( DRM_WORD );
            dwStreamOffset = dwOffsetToOriginalWFXFormat;
            LoadWORDOffset( wWFXwFormatTag, pData, dwStreamOffset );
            dwStreamOffset = AUDIO_STREAM_WFX_FORMAT_TAG_OFFSET;
            SaveWORDOffset( pData, wWFXwFormatTag, dwStreamOffset );

            /*
            ** Reduce the value in the 'cbSize' field in the WAVEFORMATEX structure.
            */
            dwStreamOffset = WFX_CBSIZE_OFFSET;
            LoadWORDOffset( wWFXcbSize, pData, dwStreamOffset );
            wWFXcbSize -= SIZEOF( DRM_WORD );
            dwStreamOffset = WFX_CBSIZE_OFFSET;
            SaveWORDOffset( pData, wWFXcbSize, dwStreamOffset );
            pStreamInfo->WFX.cbSize -= SIZEOF( DRM_WORD );

            /*
            ** Skip over the last two bytes of the Codec Specific Data (where
            ** the real format tag was stored); this data section is in the middle of the object.
            ** First, calculate the number of the bytes prior, and append them.
            */
            cbFirstHalf = dwOffsetToOriginalWFXFormat - MIN_OBJ_SIZE;
            ChkDR( _AppendToHeaderBuffer( &pAsfContext->ASFTranscryptContext,
                                           pData + MIN_OBJ_SIZE,
                                           cbFirstHalf ) );

            /*
            ** Then, calculate the number of bytes afterward, and append them.
            */
            cbSecondHalf = dwThisBuffer - cbFirstHalf - SIZEOF( DRM_WORD ) - MIN_OBJ_SIZE;

            ChkDR( _AppendToHeaderBuffer( &pAsfContext->ASFTranscryptContext,
                                           pData + cbFirstHalf + SIZEOF( DRM_WORD ) + MIN_OBJ_SIZE,
                                           cbSecondHalf ) );
        }
        else if ( DRM_IsEqualGUID( &CLSID_ASFStreamTypeVideoMedia, &(pStreamInfo->guidStreamType) ) )
        {
            DRM_DWORD           dwBMIHSize     = 0;
            DRM_WORD            wVIHSize       = 0;
            DRM_DWORD           biCompressionOriginal = 0;

            /* Update the ASF header object size -
            ** Because the ASF header object header has already been written
            ** to the header buffer by ProcessHeaders, we have to reach backward in the
            ** header buffer and update the header size.
            */
            pAsfContext->ASFTranscryptContext.Header.cbHeaderSize -= SIZEOF( DRM_DWORD );
            cbHeaderObject = DRM_UI64( pAsfContext->ASFTranscryptContext.Header.cbHeaderSize );
            dwStreamOffset = ASF_OBJECT_SIZE_OFFSET;
            SaveQWORDOffset( pAsfContext->ASFTranscryptContext.Header.pbHeader,
                             cbHeaderObject,
                             dwStreamOffset );


            /* Update the stream properties object size
            ** Because the stream properties object header (length MIN_OBJ_SIZE) has already been written
            ** to the header buffer by _SaveHeaderObjectToNewHeader, we have to reach backward in the
            ** header buffer and update the object size.
            */
            cbStreamObject = DRM_UI64Sub( cbStreamObject, DRM_UI64( SIZEOF( DRM_DWORD ) ) );  /* subtract 4 bytes since we're removing the PRDY tag */
            dwStreamOffset = pAsfContext->ASFTranscryptContext.Header.dwHeaderOffset
                             - MIN_OBJ_SIZE
                             + ASF_OBJECT_SIZE_OFFSET;
            SaveQWORDOffset( pAsfContext->ASFTranscryptContext.Header.pbHeader, cbStreamObject, dwStreamOffset );

            /* Update the format specific data size in the stream properties object */
            cbTypeSpecificData -= SIZEOF( DRM_DWORD );
            dwStreamOffset = STREAM_PROPERTIES_TYPE_SPECIFIC_SIZE_OFFSET;
            SaveDWORDOffset( pData, cbTypeSpecificData, dwStreamOffset );

            /*
            ** Load the compression tag, validate that it's PlayReady,
            ** then replace it with the original real format tag
            */
            ChkBOOL( pStreamInfo->VIH.BMIH.biCompression == PMF_VIDEO_FORMATTAG, DRM_E_ASF_BAD_ASF_HEADER );
            dwStreamOffset = VIDEO_STREAM_BMIH_OFFSET
                             + pStreamInfo->VIH.BMIH.biSize
                             - SIZEOF( DRM_DWORD );
            LoadDWORDOffset( biCompressionOriginal, pData, dwStreamOffset );
            dwStreamOffset = VIDEO_STREAM_BMIH_OFFSET + BMIH_COMPRESSION_TAG_OFFSET;
            SaveDWORDOffset( pData, biCompressionOriginal, dwStreamOffset );

            /*
            ** Reduce the value in the 'cbFormatDataSize' field in the video info header structure.
            */
            dwStreamOffset = VIDEO_STREAM_VIH_SIZE_OFFSET;
            LoadWORDOffset( wVIHSize, pData, dwStreamOffset );
            wVIHSize -= SIZEOF( DRM_DWORD );
            dwStreamOffset = VIDEO_STREAM_VIH_SIZE_OFFSET;
            SaveWORDOffset( pData, wVIHSize, dwStreamOffset );
            pStreamInfo->VIH.cbFormatDataSize -= SIZEOF( DRM_DWORD );


            /*
            ** Reduce the value in the 'biSize' field in the BITMAPINFOHEADER structure.
            */
            dwStreamOffset = VIDEO_STREAM_BMIH_OFFSET;
            LoadDWORDOffset( dwBMIHSize, pData, dwStreamOffset );
            dwBMIHSize -= SIZEOF( DRM_DWORD );
            dwStreamOffset = VIDEO_STREAM_BMIH_OFFSET;
            SaveDWORDOffset( pData, dwBMIHSize, dwStreamOffset );
            pStreamInfo->VIH.BMIH.biSize -= SIZEOF( DRM_DWORD );

            /*
            ** Skip over the last four bytes of the Codec Specific Data (where
            ** the real compression tag was stored); this data section is in the middle of the object.
            ** First, calculate the number of the bytes prior, and append them.
            */
            cbFirstHalf = VIDEO_STREAM_BMIH_OFFSET
                        + pStreamInfo->VIH.BMIH.biSize
                        - MIN_OBJ_SIZE;
            ChkDR( _AppendToHeaderBuffer( &pAsfContext->ASFTranscryptContext,
                                           pData + MIN_OBJ_SIZE,
                                           cbFirstHalf ) );

            /*
            ** Then, calculate the number of bytes afterward, and append them.
            */
            cbSecondHalf = dwThisBuffer - cbFirstHalf - SIZEOF( DRM_DWORD ) - MIN_OBJ_SIZE;
            ChkDR( _AppendToHeaderBuffer( &pAsfContext->ASFTranscryptContext,
                                           pData + cbFirstHalf + SIZEOF( DRM_DWORD ) + MIN_OBJ_SIZE,
                                           cbSecondHalf ) );
        }
        else
        {
            ChkDR( _AppendToHeaderBuffer( &pAsfContext->ASFTranscryptContext, pData + MIN_OBJ_SIZE, dwThisBuffer - MIN_OBJ_SIZE ) );
        }
    }
    else
    {
        ChkDR( _AppendToHeaderBuffer( &pAsfContext->ASFTranscryptContext, pData + MIN_OBJ_SIZE, dwThisBuffer - MIN_OBJ_SIZE ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}




/****************************************************************************/
static DRM_RESULT _ASFParseStreamPropertiesObject(
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __in       DRM_DWORD               dwObjSize,
    __in const DRM_UINT64              PrevOffset,
    __in const DRM_DWORD               dwBitRate
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_GUID streamType = {0};
    DRM_GUID ecStrategy;
    DRM_UINT64 qwOffset;
    DRM_DWORD dwTypeSpecific = 0;
    DRM_DWORD dwErrConcealment = 0;
    DRM_WORD wStreamNum = 0;
    DRM_DWORD dwJunk = 0;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwNowWanted = 0;
    DRM_DWORD dwOffset = 0;
    ASF_PARSER_STATE *pState = &(pAsfContext->ParserState);
    ASF_STREAM_INFO *pStreamInfo = NULL;
    DRM_DWORD dwObjOffset = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseStreamPropertiesObject );

    dwObjSize -= MIN_OBJ_SIZE;

    dwNowWanted = 2 * SIZEOF( DRM_GUID ) +  /* Stream Type and Error Correction Type */
                  SIZEOF( DRM_UINT64 ) +           /* Time Offset */
                  3 * SIZEOF( DRM_DWORD ) +       /* Type-Specific Data Length, Error Correction Data Length and Reserved */
                  SIZEOF( DRM_WORD );            /* Flags */

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwOffset = 0;
    LoadGUIDOffset( streamType, pData, dwOffset );
    LoadGUIDOffset( ecStrategy, pData, dwOffset );
    LoadQWORDOffset( qwOffset, pData, dwOffset );
    LoadDWORDOffset( dwTypeSpecific, pData, dwOffset );
    LoadDWORDOffset( dwErrConcealment, pData, dwOffset );
    LoadWORDOffset( wStreamNum, pData, dwOffset );
    wStreamNum &= 0x7f;
    LoadDWORDOffset( dwJunk, pData, dwOffset );

    /* Flag that we found the stream */
    ChkBOOL( wStreamNum < MAXSTREAMS, DRM_E_ASF_BAD_ASF_HEADER );
    pState->fFoundStream[wStreamNum] = TRUE;

    dwTotalUsed += dwNowWanted;

    if ( DRM_SUCCEEDED( _FindStreamInStreamInfoList(pAsfContext, (DRM_DWORD) (wStreamNum), &pStreamInfo) ) )
    {
        /* stream has been added, just need to update the media type */
        MEMCPY( &pStreamInfo->guidStreamType, &streamType, SIZEOF( DRM_GUID ));
    }
    else
    {
        _AddToStreamInfoList(pAsfContext, (DRM_DWORD) wStreamNum, dwBitRate, streamType, &pStreamInfo);
    }

    ChkMem( pStreamInfo->pbStreamPropertiesObj = (DRM_BYTE*)Oem_MemAlloc( dwObjSize + MIN_OBJ_SIZE ) );
    MEMCPY( pStreamInfo->pbStreamPropertiesObj, pData, dwNowWanted );
    dwObjOffset = dwNowWanted;

    /* Copy type-specific data and error correction data as it is */
    dwNowWanted = dwTypeSpecific + dwErrConcealment;
    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    DRM_BYT_CopyBytes( pStreamInfo->pbStreamPropertiesObj, dwObjOffset, pData, 0, dwNowWanted );
    dwObjOffset += dwNowWanted;

    if ( DRM_IsEqualGUID( &CLSID_ASFStreamTypeAudioMedia, &streamType ) )
    {
        ChkDR( _FindStreamInStreamInfoList(pAsfContext, (DRM_DWORD) (wStreamNum & 0x7f), &pStreamInfo) );
        dwOffset = 0;
        LoadWORDOffset ( pStreamInfo->WFX.wFormatTag     , pData, dwOffset );
        LoadWORDOffset ( pStreamInfo->WFX.nChannels      , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->WFX.nSamplesPerSec , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->WFX.nAvgBytesPerSec, pData, dwOffset );
        LoadWORDOffset ( pStreamInfo->WFX.nBlockAlign    , pData, dwOffset );
        LoadWORDOffset ( pStreamInfo->WFX.wBitsPerSample , pData, dwOffset );
        LoadWORDOffset ( pStreamInfo->WFX.cbSize         , pData, dwOffset );
    }
    else if ( DRM_IsEqualGUID( &CLSID_ASFStreamTypeVideoMedia, &streamType ) )
    {
        ChkDR( _FindStreamInStreamInfoList(pAsfContext, (DRM_DWORD) (wStreamNum & 0x7f), &pStreamInfo) );
        dwOffset = 0;

        /* Load the Video Info Header part of the video stream properties */
        LoadDWORDOffset( pStreamInfo->VIH.dwEncodedImageWidth   , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->VIH.dwEncodedImageHeight  , pData, dwOffset );
        LoadBYTEOffset ( pStreamInfo->VIH.bReserved             , pData, dwOffset );
        LoadWORDOffset ( pStreamInfo->VIH.cbFormatDataSize      , pData, dwOffset );

        /* Load the BITMAPINFOHEADER part */
        LoadDWORDOffset( pStreamInfo->VIH.BMIH.biSize           , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->VIH.BMIH.biWidth          , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->VIH.BMIH.biHeight         , pData, dwOffset );
        LoadWORDOffset ( pStreamInfo->VIH.BMIH.biPlanes         , pData, dwOffset );
        LoadWORDOffset ( pStreamInfo->VIH.BMIH.biBitCount       , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->VIH.BMIH.biCompression    , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->VIH.BMIH.biSizeImage      , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->VIH.BMIH.biXPelsPerMeter  , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->VIH.BMIH.biYPelsPerMeter  , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->VIH.BMIH.biClrUsed        , pData, dwOffset );
        LoadDWORDOffset( pStreamInfo->VIH.BMIH.biClrImportant   , pData, dwOffset );
    }


    if (DRM_UI64Eql(PrevOffset, DRM_UI64(0)) )
    {
        pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwObjSize ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFProcessStreamPropertiesObjectEx(
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __in       DRM_DWORD               dwObjSize,
    __in const DRM_UINT64              PrevOffset
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WORD i = 0;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwNowWanted = 0;
    DRM_WORD wStreamNameCount = 0;
    DRM_WORD wPayloadExtnSysCount = 0;
    DRM_WORD wLangIdIndex = 0;
    DRM_WORD wStreamNameLen = 0;
    DRM_GUID ExtnSystemId;
    DRM_WORD wExtnDataSize = 0;
    DRM_DWORD dwExtnSysInfoLen = 0;
    ASF_PARSER_STATE *pState;
    DRM_BYTE *pPayloadExtnSysCount = NULL;
    DRM_BYTE *pNewHeader = NULL;
    DRM_WORD wSaveSize = 0;
    DRM_DWORD dwSaveSize = 0;
    DRM_DWORD dwDataBitrate = 0;
    DRM_DWORD dwBufferSize = 0;
    DRM_DWORD dwInitialBufferFullness = 0;
    DRM_DWORD dwAlternateDataBitrate = 0;
    DRM_DWORD dwAlternateBufferSize = 0;
    DRM_DWORD dwAlternateInitialBufferFullness = 0;
    DRM_DWORD dwMaximumObjectSize = 0;
    DRM_DWORD dwPayloadExtnSysCountOffset = 0;
    DRM_DWORD dwOffset = 0;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;
    DRM_WORD wStreamNum;
    DRM_UINT64 hnsAvgTimePerFrame;
    DRM_BOOL   fFoundAESPayload = FALSE;
    DRM_DWORD dwPayloadExtDataOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessStreamPropertiesObjectEx );

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);

    dwObjSize -= MIN_OBJ_SIZE;

    /* Read in most of the Extended Stream Property object's header */
    dwNowWanted = 3 * SIZEOF( DRM_UINT64 ) +   /* Start Time, End Time and Average Time per Frame */
                  8 * SIZEOF( DRM_DWORD ) +   /* Data Bitrate, Buffer Size, Initial Buffer Fullness,
                                         * Alternate Data BitRate, Alternate Buffer Size, Alternate
                                         * Initial Buffer Fullness, Maximum Object Size and Flags */
                  2 * SIZEOF( DRM_WORD );    /* Stream Number and Stream Language ID Index */

    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwTotalUsed += dwNowWanted;

    ChkDR( _AppendToHeaderBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );
    /* Skip unwanted data: start time and end time  */
    dwOffset = 2 * SIZEOF(DRM_UINT64);

    /* Read data to allow us to identify VBR vs CBR content */
    LoadDWORDOffset( dwDataBitrate, pData, dwOffset );
    LoadDWORDOffset( dwBufferSize, pData, dwOffset );
    LoadDWORDOffset( dwInitialBufferFullness, pData, dwOffset );
    LoadDWORDOffset( dwAlternateDataBitrate, pData, dwOffset );
    LoadDWORDOffset( dwAlternateBufferSize, pData, dwOffset );
    LoadDWORDOffset( dwAlternateInitialBufferFullness, pData, dwOffset );

    LoadDWORDOffset( dwMaximumObjectSize, pData, dwOffset );
    if ( dwMaximumObjectSize > pAsfContext->dwMaximumObjectSize )
    {
        pAsfContext->dwMaximumObjectSize = dwMaximumObjectSize;
    }

    /* Skip unwanted data: flags */
    dwOffset += SIZEOF(DRM_DWORD);

    /* Read stream number */
    LoadWORDOffset( wStreamNum, pData, dwOffset );

    /* Skip unwanted data: Stream Language ID Index */
    dwOffset += SIZEOF(DRM_WORD);

    /* Read average time per frame */
    LoadQWORDOffset( hnsAvgTimePerFrame, pData, dwOffset );

    if ( dwObjSize > dwTotalUsed )
    {
        /* Read Stream Name count */
        dwNowWanted = SIZEOF( DRM_WORD );
        if ( dwTotalUsed + dwNowWanted > dwObjSize )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwTotalUsed += dwNowWanted;

        ChkDR( _AppendToHeaderBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );

        dwOffset = 0;
        LoadWORDOffset( wStreamNameCount, pData, dwOffset );
    }

    if ( dwObjSize > dwTotalUsed )
    {
        /* Read Payload Extensions System count */
        dwNowWanted = SIZEOF( DRM_WORD );
        if ( dwTotalUsed + dwNowWanted > dwObjSize )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwTotalUsed += dwNowWanted;
        dwOffset = 0;

        pPayloadExtnSysCount = pAsfTranscryptContext->Header.pbHeader;
        dwPayloadExtnSysCountOffset = pAsfTranscryptContext->Header.dwHeaderOffset;
        ChkDR( _AppendToHeaderBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );
        LoadWORDOffset( wPayloadExtnSysCount, pData, dwOffset );
    }

    /* Read stream names */
    if ( dwObjSize > dwTotalUsed )
    {
        if ( wStreamNameCount > 0 )
        {
            for( i = 0; i < wStreamNameCount; i++ )
            {
                /* Read Language ID Index and Stream Name length */
                dwNowWanted = 2 * SIZEOF( DRM_WORD );
                if ( dwTotalUsed + dwNowWanted > dwObjSize )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
                if ( dwThisBuffer != dwNowWanted )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                dwTotalUsed += dwNowWanted;

                ChkDR( _AppendToHeaderBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );

                dwOffset = 0;
                LoadWORDOffset( wLangIdIndex , pData, dwOffset );
                LoadWORDOffset( wStreamNameLen , pData, dwOffset );

                if ( wStreamNameLen > 0 )
                {
                    dwNowWanted = wStreamNameLen;
                    if ( dwTotalUsed + dwNowWanted > dwObjSize )
                    {
                        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                    }

                    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
                    if ( dwThisBuffer != dwNowWanted )
                    {
                        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                    }

                    dwTotalUsed += dwNowWanted;

                    ChkDR( _AppendToHeaderBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );
                }
            }
        }
    }

    /* Read Playload extension systems */
    if ( dwObjSize > dwTotalUsed )
    {
        if ( wPayloadExtnSysCount > 0 )
        {
            for( i = 0; i < wPayloadExtnSysCount; i++ )
            {
                dwNowWanted = SIZEOF( DRM_GUID ) +  /* Extension System ID */
                              SIZEOF( DRM_WORD ) +       /* Extension Data Size */
                              SIZEOF( DRM_DWORD );        /* Extension System Info Length */
                if ( dwTotalUsed + dwNowWanted > dwObjSize )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
                if ( dwThisBuffer != dwNowWanted )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                dwTotalUsed += dwNowWanted;

                /*
                ** Append the ASF Payload Extension to the output buffer
                */
                ChkDR( _AppendToHeaderBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );

                /*
                ** From the ASF Payload Extension, read-in the Extension System ID (a GUID), Data Size, and Info Length
                */
                dwOffset = 0;
                LoadGUIDOffset( ExtnSystemId, pData, dwOffset );
                LoadWORDOffset( wExtnDataSize, pData, dwOffset );
                LoadDWORDOffset( dwExtnSysInfoLen, pData, dwOffset );

                if ( DRM_IsEqualGUID( &ExtnSystemId, &CLSID_ASF_Payload_Extension_Encryption_SampleID ) )
                {
                    ChkBOOL( !fFoundAESPayload, DRM_E_ASF_BAD_ASF_HEADER );
                    fFoundAESPayload = TRUE;

                    /* There's already a sample ID, we don't need to add one */
                    pAsfContext->dwAesPayloadExtensionAdditionalBytes = 0;
                    pAsfContext->rgdwSampleIDRepDataOffset[wStreamNum] = dwPayloadExtDataOffset;
                    if( wExtnDataSize == 0xFFFF && dwPayloadExtDataOffset != 0xFFFF )
                    {
                        /* If it's a variable length field then data starts after a WORD */
                        pAsfContext->rgdwSampleIDRepDataOffset[wStreamNum] += SIZEOF( DRM_WORD );
                    }
                }

                ChkOverflow( dwPayloadExtDataOffset + wExtnDataSize, dwPayloadExtDataOffset );
                if( wExtnDataSize == 0xFFFF )
                {
                    /* Variable length field encountered */
                    dwPayloadExtDataOffset = 0xFFFF;
                }
                else if( dwPayloadExtDataOffset != 0xFFFF )
                {
                    dwPayloadExtDataOffset += wExtnDataSize;
                }

                /*
                ** If the Extension System Info Length is non-zero,
                ** read it in as well, and then append it to the output buffer.
                */
                if ( dwExtnSysInfoLen > 0 )
                {
                    dwNowWanted = dwExtnSysInfoLen;
                    if ( dwTotalUsed + dwNowWanted > dwObjSize )
                    {
                        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                    }

                    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
                    if ( dwThisBuffer != dwNowWanted )
                    {
                        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                    }

                    dwTotalUsed += dwNowWanted;

                    ChkDR( _AppendToHeaderBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );
                }
            }
        }
    }

    if ( !fFoundAESPayload )
    {
        /* Add payload extension system for WMDRMND in the new header buffer */
        pNewHeader = pAsfTranscryptContext->Header.pbHeader;
        dwOffset = pAsfTranscryptContext->Header.dwHeaderOffset;
        SaveGUIDOffset( pNewHeader, CLSID_ASF_Payload_Extension_Encryption_SampleID, dwOffset );
        wSaveSize = 0xffff;
        SaveWORDOffset( pNewHeader, wSaveSize, dwOffset );
        dwSaveSize = 0;
        SaveDWORDOffset( pNewHeader, dwSaveSize, dwOffset );

        pAsfTranscryptContext->Header.dwHeaderOffset += WMDRM_ND_PAYLOAD_EXTENSION_SIZE;

        /* Update payload extension system count */
        wSaveSize = wPayloadExtnSysCount + 1;
        dwOffset = dwPayloadExtnSysCountOffset;
        SaveWORDOffset( pPayloadExtnSysCount, wSaveSize, dwOffset );

        /* We'll be adding extra data to the payload extension for the sample ID */
        pAsfContext->dwAesPayloadExtensionAdditionalBytes = SIZEOF( DRM_WORD ) + SIZEOF( DRM_UINT64 );
    }

    /*
    ** Sometimes the Extended Stream Properties object contains an embedded
    ** Stream Properties object.
    */
    if( dwObjSize > dwTotalUsed )
    {
        DRM_GUID sobjId;
        DRM_UINT64 sqwSize;

        ChkDR( _ASFLoadObjectHeader( pAsfContext, &sobjId, &sqwSize, DRM_UI64Add(PrevOffset, DRM_UI64(dwTotalUsed)) ) );

        if ( DRM_IsEqualGUID( &CLSID_ASFStreamPropertiesObjectV1, &sobjId ) )
        {
            dwNowWanted = DRM_UI64Low32( sqwSize );
            if ( dwTotalUsed + dwNowWanted > dwObjSize )
            {
                ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
            }

            ChkDR( _SaveHeaderObjectToNewHeader( pAsfTranscryptContext, &sobjId, &sqwSize ) );

            ChkDR( _ASFProcessStreamPropertiesObject( pAsfContext,
                                                       dwNowWanted - MIN_OBJ_SIZE,
                                                       DRM_UI64Add( PrevOffset, DRM_UI64( dwTotalUsed + MIN_OBJ_SIZE ) ) ) );
        }
    }


ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFParseStreamPropertiesObjectEx(
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __in       DRM_DWORD               dwObjSize,
    __in const DRM_UINT64              PrevOffset
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WORD i = 0;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwNowWanted = 0;
    DRM_WORD wStreamNameCount = 0;
    DRM_WORD wPayloadExtnSysCount = 0;
    DRM_WORD wLangIdIndex = 0;
    DRM_WORD wStreamNameLen = 0;
    DRM_GUID ExtnSystemId;
    DRM_WORD wExtnDataSize = 0;
    DRM_DWORD dwExtnSysInfoLen = 0;
    ASF_PARSER_STATE *pState;
    DRM_DWORD dwDataBitrate = 0;
    DRM_DWORD dwBufferSize = 0;
    DRM_DWORD dwInitialBufferFullness = 0;
    DRM_DWORD dwAlternateDataBitrate = 0;
    DRM_DWORD dwAlternateBufferSize = 0;
    DRM_DWORD dwAlternateInitialBufferFullness = 0;
    DRM_DWORD dwOffset = 0;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;
    ASF_STREAM_INFO *pStreamInfo = NULL;
    DRM_WORD wStreamNum;
    DRM_UINT64 hnsAvgTimePerFrame;
    DRM_BOOL   fFoundAESPayload = FALSE;
    DRM_DWORD  dwPayloadExtDataOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseStreamPropertiesObjectEx );

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);

    dwObjSize -= MIN_OBJ_SIZE;

    /* Read in most of the Extended Stream Property object's header */
    dwNowWanted = 3 * SIZEOF( DRM_UINT64 ) +   /* Start Time, End Time and Average Time per Frame */
                  8 * SIZEOF( DRM_DWORD ) +   /* Data Bitrate, Buffer Size, Initial Buffer Fullness,
                                         * Alternate Data BitRate, Alternate Buffer Size, Alternate
                                         * Initial Buffer Fullness, Maximum Object Size and Flags */
                  2 * SIZEOF( DRM_WORD );    /* Stream Number and Stream Language ID Index */

    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwTotalUsed += dwNowWanted;

    /* Skip unwanted data: start time and end time  */
    dwOffset = 2 * SIZEOF(DRM_UINT64);

    /* Read data to allow us to identify VBR vs CBR content */
    LoadDWORDOffset( dwDataBitrate, pData, dwOffset );
    LoadDWORDOffset( dwBufferSize, pData, dwOffset );
    LoadDWORDOffset( dwInitialBufferFullness, pData, dwOffset );
    LoadDWORDOffset( dwAlternateDataBitrate, pData, dwOffset );
    LoadDWORDOffset( dwAlternateBufferSize, pData, dwOffset );
    LoadDWORDOffset( dwAlternateInitialBufferFullness, pData, dwOffset );

    if ( dwDataBitrate != dwAlternateDataBitrate &&
         dwBufferSize != dwAlternateBufferSize )
    {
        pAsfTranscryptContext->fIsVBRContent = TRUE;
    }
    else
    {
        pAsfTranscryptContext->fIsVBRContent = FALSE;
    }

    /* Skip unwanted data: maximum object size and flags */
    dwOffset += 2 * SIZEOF(DRM_DWORD);

    /* Read stream number */
    LoadWORDOffset( wStreamNum, pData, dwOffset );
    ChkBOOL( wStreamNum < MAXSTREAMS, DRM_E_ASF_BAD_ASF_HEADER );
    pState->fFoundStreamPropertiesObjectEx[wStreamNum] = TRUE;

    if ( DRM_SUCCEEDED(  _FindStreamInStreamInfoList(pAsfContext, wStreamNum, &pStreamInfo) ) )
    {
        /* Already been added, just need to set the bitrate */
        pStreamInfo->dwAverageBitrate = dwDataBitrate;
    }
    else
    {
        /* Not added yet */
        ChkDR( _AddToStreamInfoList(pAsfContext, wStreamNum, dwDataBitrate, CLSID_ASFStreamTypeUnknown, NULL) );
    }

    /* Skip unwanted data: Stream Language ID Index */
    dwOffset += SIZEOF(DRM_WORD);

    /* Read average time per frame */
    LoadQWORDOffset( hnsAvgTimePerFrame, pData, dwOffset );

    /* set values in the stream info list */
    ChkDR( _FindStreamInStreamInfoList(pAsfContext, wStreamNum, &pStreamInfo) );
    pStreamInfo->hnsAvgTimePerFrame = hnsAvgTimePerFrame;
    pStreamInfo->dwMaxBitrate = dwAlternateDataBitrate;
    pStreamInfo->msMaxBufferWindow = dwAlternateBufferSize;

    if ( dwObjSize > dwTotalUsed )
    {
        /* Read Stream Name count */
        dwNowWanted = SIZEOF( DRM_WORD );
        if ( dwTotalUsed + dwNowWanted > dwObjSize )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwTotalUsed += dwNowWanted;

        dwOffset = 0;
        LoadWORDOffset( wStreamNameCount, pData, dwOffset );
    }

    if ( dwObjSize > dwTotalUsed )
    {
        /* Read Payload Extensions System count */
        dwNowWanted = SIZEOF( DRM_WORD );
        if ( dwTotalUsed + dwNowWanted > dwObjSize )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwTotalUsed += dwNowWanted;
        dwOffset = 0;

        LoadWORDOffset( wPayloadExtnSysCount, pData, dwOffset );
    }

    /* Read stream names */
    if ( dwObjSize > dwTotalUsed )
    {
        if ( wStreamNameCount > 0 )
        {
            for( i = 0; i < wStreamNameCount; i++ )
            {
                /* Read Language ID Index and Stream Name length */
                dwNowWanted = 2 * SIZEOF( DRM_WORD );
                if ( dwTotalUsed + dwNowWanted > dwObjSize )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
                if ( dwThisBuffer != dwNowWanted )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                dwTotalUsed += dwNowWanted;

                dwOffset = 0;
                LoadWORDOffset( wLangIdIndex , pData, dwOffset );
                LoadWORDOffset( wStreamNameLen , pData, dwOffset );

                if ( wStreamNameLen > 0 )
                {
                    dwNowWanted = wStreamNameLen;
                    if ( dwTotalUsed + dwNowWanted > dwObjSize )
                    {
                        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                    }

                    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
                    if ( dwThisBuffer != dwNowWanted )
                    {
                        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                    }

                    dwTotalUsed += dwNowWanted;

                }
            }
        }
    }

    /* Read Playload extension systems */
    if ( dwObjSize > dwTotalUsed )
    {
        if ( wPayloadExtnSysCount > 0 )
        {
            for( i = 0; i < wPayloadExtnSysCount; i++ )
            {
                dwNowWanted = SIZEOF( DRM_GUID ) +  /* Extension System ID */
                              SIZEOF( DRM_WORD ) +       /* Extension Data Size */
                              SIZEOF( DRM_DWORD );        /* Extension System Info Length */
                if ( dwTotalUsed + dwNowWanted > dwObjSize )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
                if ( dwThisBuffer != dwNowWanted )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                dwTotalUsed += dwNowWanted;

                dwOffset = 0;
                LoadGUIDOffset( ExtnSystemId, pData, dwOffset );
                LoadWORDOffset( wExtnDataSize, pData, dwOffset );
                LoadDWORDOffset( dwExtnSysInfoLen, pData, dwOffset );

                if ( DRM_IsEqualGUID( &ExtnSystemId, &CLSID_ASF_Payload_Extension_Encryption_SampleID ) )
                {
                    ChkBOOL( !fFoundAESPayload, DRM_E_ASF_BAD_ASF_HEADER );
                    fFoundAESPayload = TRUE;

                    /* There's already a sample ID, we don't need to add one */
                    pAsfContext->dwAesPayloadExtensionAdditionalBytes = 0;
                    pAsfContext->rgdwSampleIDRepDataOffset[wStreamNum] = dwPayloadExtDataOffset;
                    if( wExtnDataSize == 0xFFFF && dwPayloadExtDataOffset != 0xFFFF )
                    {
                        /* If it's a variable length field then data starts after a WORD */
                        pAsfContext->rgdwSampleIDRepDataOffset[wStreamNum] += SIZEOF( DRM_WORD );
                    }
                }

                ChkOverflow( dwPayloadExtDataOffset + wExtnDataSize, dwPayloadExtDataOffset );
                if( wExtnDataSize == 0xFFFF )
                {
                    /* Variable length field encountered */
                    dwPayloadExtDataOffset = 0xFFFF;
                }
                else if( dwPayloadExtDataOffset != 0xFFFF )
                {
                    dwPayloadExtDataOffset += wExtnDataSize;
                }


                if ( dwExtnSysInfoLen > 0 )
                {
                    dwNowWanted = dwExtnSysInfoLen;
                    if ( dwTotalUsed + dwNowWanted > dwObjSize )
                    {
                        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                    }

                    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), PrevOffset), DRM_UI64(dwTotalUsed)) , dwNowWanted, &pData );
                    if ( dwThisBuffer != dwNowWanted )
                    {
                        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                    }

                    dwTotalUsed += dwNowWanted;
                }
            }
        }
    }

    /*
    ** Sometimes the Extended Stream Properties object contains an embedded
    ** Stream Properties object.
    */
    if ( dwObjSize > dwTotalUsed )
    {
        DRM_GUID sobjId;
        DRM_UINT64 sqwSize;

        ChkDR( _ASFLoadObjectHeader( pAsfContext, &sobjId, &sqwSize, DRM_UI64Add(PrevOffset, DRM_UI64(dwTotalUsed)) ) );

        if ( DRM_IsEqualGUID( &CLSID_ASFStreamPropertiesObjectV1, &sobjId ) )
        {
            dwNowWanted = DRM_UI64Low32( sqwSize );
            if ( dwTotalUsed + dwNowWanted > dwObjSize )
            {
                ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
            }

            ChkDR( _ASFParseStreamPropertiesObject( pAsfContext, dwNowWanted, DRM_UI64Add(DRM_UI64Add(PrevOffset, DRM_UI64(dwTotalUsed)), DRM_UI64(MIN_OBJ_SIZE)), dwDataBitrate ) );
        }
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT DRM_CALL _ASFParseHeaderExtensionObject(
    __inout NDT_ASF_CONTEXT * const pAsfContext,
    __in    DRM_DWORD               dwObjSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_GUID guidReservedField1;
    DRM_WORD wReservedField2;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwNowWanted = 0;
    ASF_PARSER_STATE *pState = NULL;
    DRM_DWORD dwOffset = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseHeaderExtensionObject );

    dwObjSize -= MIN_OBJ_SIZE;

    pState = &(pAsfContext->ParserState);

    /* Copy Reserved fields 1 and 2 */
    dwNowWanted = SIZEOF( DRM_GUID ) + SIZEOF( DRM_WORD );
    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
         ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwTotalUsed += dwNowWanted;

    dwOffset = 0;
    LoadGUIDOffset( guidReservedField1, pData, dwOffset );
    LoadWORDOffset( wReservedField2, pData, dwOffset );

    if ( ( ! DRM_IsEqualGUID( &CLSID_ASFPacketClock1, &guidReservedField1 ) ) ||
         ( wReservedField2 != 6 ) )
    {
         ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    /* Read header extension data size */
    dwNowWanted = SIZEOF( DRM_DWORD );
    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
         ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
         ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwTotalUsed += dwNowWanted;

    dwOffset = 0;
    LoadDWORDOffset( pAsfContext->ASFTranscryptContext.Header.dwHeaderExtensionDataSize,
                     pData,
                     dwOffset );

    if ( pAsfContext->ASFTranscryptContext.Header.dwHeaderExtensionDataSize >= MIN_OBJ_SIZE )
    {
        /* There are more objects. Parse them. */
        DRM_GUID sobjId;
        DRM_UINT64 sqwSize;

        while ( dwTotalUsed < dwObjSize )
        {
            ChkDR( _ASFLoadObjectHeader( pAsfContext, &sobjId, &sqwSize, DRM_UI64(dwTotalUsed) ) );

            if ( DRM_IsEqualGUID( &CLSID_ASFStreamPropertiesObjectEx, &sobjId ) )
            {
                dwNowWanted = DRM_UI64Low32( sqwSize );
                if ( dwTotalUsed + dwNowWanted > dwObjSize )
                {
                     ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                ChkDR( _ASFParseStreamPropertiesObjectEx( pAsfContext,
                                                          dwNowWanted,
                                                          DRM_UI64( dwTotalUsed + MIN_OBJ_SIZE ) ) );
            }
            /*
            ** -----------------------------------
            ** PlayReady Object
            ** -----------------------------------
            */
            else if ( DRM_IsEqualGUID( &CLSID_ASFPlayReadyObject, &sobjId ) )
            {
                dwNowWanted = DRM_UI64Low32( sqwSize );
                if ( dwTotalUsed + dwNowWanted > dwObjSize )
                {
                     ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                pState->fPlayReadyObjectInsideEHO = TRUE;
                ChkDR( _ASFParsePlayReadyObject( pAsfContext, pState, dwNowWanted, DRM_UI64( dwTotalUsed + MIN_OBJ_SIZE ) ) );
            }

            dwTotalUsed += DRM_UI64Low32( sqwSize );
        }
    }

    /* Skip data if it is left */
    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwObjSize ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFParseStreamBitratePropertiesObject(
    __inout NDT_ASF_CONTEXT * const pAsfContext,
    __in    DRM_DWORD               dwObjSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwNowWanted = 0;
    DRM_DWORD dwAverageBitrate = 0;
    DRM_WORD  wFlags = 0;
    DRM_WORD  cRecords;
    DRM_WORD  i;
    DRM_DWORD dwOffset = 0;
    ASF_PARSER_STATE *pState = NULL;
    ASF_STREAM_INFO *pStreamInfo = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseStreamBitratePropertiesObject );

    pState = &pAsfContext->ParserState;

    dwObjSize -= MIN_OBJ_SIZE;

    /* Read Bitrate Records Count (DRM_WORD)
     */
    dwNowWanted = SIZEOF( DRM_WORD );
    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }
    dwTotalUsed += dwNowWanted;

    LoadWORDOffset( cRecords, pData, dwOffset );

    /* Get Each Bitrate Record */
    for ( i = 0; i < cRecords; i++ )
    {
        /* Read Bitrate Record: Flags (DRM_WORD) + Average Bitrate (DRM_DWORD)
         */
        dwNowWanted = SIZEOF( DRM_WORD ) + SIZEOF( DRM_DWORD );
        if ( dwTotalUsed + dwNowWanted > dwObjSize )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
        }

        dwTotalUsed += dwNowWanted;

        dwOffset = 0;
        LoadWORDOffset( wFlags, pData, dwOffset );
        LoadDWORDOffset( dwAverageBitrate, pData, dwOffset );

        /* Update the bitrate if necessary */
        if ( DRM_SUCCEEDED( _FindStreamInStreamInfoList(pAsfContext, wFlags & 0x7f, &pStreamInfo) ) )
        {
            if ( 0 == pStreamInfo->dwAverageBitrate )
            {
                /* Already been added, just need to set the bitrate */
                pStreamInfo->dwAverageBitrate = dwAverageBitrate;
            }
            else
            {
                /* The stream properties object seems to be a better value
                 * to use, so if the bitrate is already set then leave well
                 * enough alone.
                 */
            }
        }
        else
        {
            /* Not added yet */
            _AddToStreamInfoList(pAsfContext, wFlags & 0x7f, dwAverageBitrate, CLSID_ASFStreamTypeUnknown, NULL);
        }
    }

    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwTotalUsed ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFAppendAdvContentEncryptionObject(
    __inout ASF_TRANSCRYPT_CONTEXT * const pAsfTranscryptContext
)
{
    DRM_RESULT                      dr          = DRM_SUCCESS;
    DRM_BYTE                       *pHeaderData = NULL;
    DRM_UINT64                      qwSaveSize  = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD                       dwSaveSize  = 0;
    DRM_WORD                        wSaveSize   = 0;
    ASF_TRANSCRYPT_HEADER_CONTEXT  *pHeader     = &(pAsfTranscryptContext->Header);
    DRM_DWORD                       dwOffset    = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFAppendAdvContentEncryptionObject );

    ChkOverflow( pHeader->dwHeaderOffset + ASF_TRANSCRYPT_WMDRMND_OBJECT_SIZE, pHeader->dwHeaderOffset );
    if ( pHeader->dwHeaderOffset + ASF_TRANSCRYPT_WMDRMND_OBJECT_SIZE > pHeader->cbHeaderSize )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    pHeaderData = pHeader->pbHeader;
    dwOffset = pHeader->dwHeaderOffset;

    SaveGUIDOffset( pHeaderData, CLSID_ASFAdvancedContentEncryptionObject, dwOffset );
    qwSaveSize = DRM_UI64( ASF_TRANSCRYPT_WMDRMND_OBJECT_SIZE );
    SaveQWORDOffset( pHeaderData, qwSaveSize, dwOffset );
    wSaveSize = 1;
    SaveWORDOffset( pHeaderData, wSaveSize, dwOffset );

    /* Content Encryption record */
    SaveGUIDOffset( pHeaderData, CLSID_ASF_ContentEncryptionSystem_WMDRM_ND_AES, dwOffset );
    dwSaveSize = 1;
    wSaveSize = 1;
    SaveDWORDOffset( pHeaderData, dwSaveSize, dwOffset );
    SaveWORDOffset( pHeaderData, wSaveSize, dwOffset );

    /* Object Encryption record */
    wSaveSize = 1;
    SaveWORDOffset( pHeaderData, wSaveSize, dwOffset );     /* Object ID Type */
    wSaveSize = 2;
    SaveWORDOffset( pHeaderData, wSaveSize, dwOffset );     /* Object ID Length */
    wSaveSize = 0;
    SaveWORDOffset( pHeaderData, wSaveSize, dwOffset );     /* Object ID */

    /* Add WMDRMND Key ID next */
    dwSaveSize = WMDRM_ND_LICENSE_KEY_ID_SIZE;
    SaveDWORDOffset( pHeaderData, dwSaveSize, dwOffset );
    DRM_BYT_CopyBytes( pHeaderData, dwOffset,
            pAsfTranscryptContext->pbLicenseKeyID, 0,
            pAsfTranscryptContext->cbLicenseKeyID );

    pHeader->dwHeaderOffset += ASF_TRANSCRYPT_WMDRMND_OBJECT_SIZE;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFProcessHeaderExtensionObject(
    __inout NDT_ASF_CONTEXT     * const pAsfContext,
    __in    DRM_DWORD                   dwObjSize )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_GUID guidReservedField1;
    DRM_WORD wReservedField2;
    DRM_DWORD dwHeaderExtensionDataSize = 0;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwNowWanted = 0;
    ASF_PARSER_STATE *pState;
    DRM_WORD wStreamNum;
    DRM_DWORD dwOffset = 0;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessHeaderExtensionObject );

    dwObjSize -= MIN_OBJ_SIZE;

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);

    /* Copy Reserved fields 1 and 2 */
    dwNowWanted = SIZEOF( DRM_GUID ) + SIZEOF( DRM_WORD );
    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
         ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwTotalUsed += dwNowWanted;

    ChkDR( _AppendToHeaderBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );
    dwOffset = 0;
    LoadGUIDOffset( guidReservedField1, pData, dwOffset );
    LoadWORDOffset( wReservedField2, pData, dwOffset );

    if ( ( ! DRM_IsEqualGUID( &CLSID_ASFPacketClock1, &guidReservedField1 ) ) ||
         ( wReservedField2 != 6 ) )
    {
         ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    /* Read header extension data size */
    dwNowWanted = SIZEOF( DRM_DWORD );
    if ( dwTotalUsed + dwNowWanted > dwObjSize )
    {
         ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
         ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwTotalUsed += dwNowWanted;

    /* Save position of the extension size so we can tweak it later */
    dwOffset = 0;
    LoadDWORDOffset( dwHeaderExtensionDataSize, pData, dwOffset );

    dwHeaderExtensionDataSize +=
        pAsfTranscryptContext->Header.cbHeaderExtensionObjectExtra;
    SaveDWORDOffset( pAsfTranscryptContext->Header.pbHeader,
                     dwHeaderExtensionDataSize,
                     pAsfTranscryptContext->Header.dwHeaderOffset );

    if ( dwTotalUsed < dwObjSize )
    {
        /* There are more objects. Parse them. */
        DRM_GUID sobjId;
        DRM_UINT64 sqwSize;

        while ( dwTotalUsed < dwObjSize )
        {
            ChkDR( _ASFLoadObjectHeader( pAsfContext, &sobjId, &sqwSize, DRM_UI64(dwTotalUsed) ) );

            if ( DRM_IsEqualGUID( &CLSID_ASFStreamPropertiesObjectEx, &sobjId ) )
            {
                dwNowWanted = DRM_UI64Low32( sqwSize );
                if ( dwTotalUsed + dwNowWanted > dwObjSize )
                {
                     ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                /*
                ** If the file is clear or RC4/COCKTAIL encrypted, then we will be
                ** adding the ND Payload Extension to the Extended Stream Properties object.
                ** If the file is already AES encrypted, then this data is already present.
                */
                if ( pAsfContext->eCipherType != eDRM_AES_COUNTER_CIPHER )
                {
                    sqwSize = DRM_UI64Add( sqwSize, DRM_UI64( WMDRM_ND_PAYLOAD_EXTENSION_SIZE ) );
                }

                ChkDR( _SaveHeaderObjectToNewHeader( &pAsfContext->ASFTranscryptContext,
                                                     &sobjId,
                                                     &sqwSize ) );
                ChkDR( _ASFProcessStreamPropertiesObjectEx( pAsfContext,
                                                            dwNowWanted,
                                                            DRM_UI64( dwTotalUsed + MIN_OBJ_SIZE ) ) );
                dwTotalUsed += dwNowWanted;
            }
            /* PlayReady Object */
            else if ( DRM_IsEqualGUID( &CLSID_ASFPlayReadyObject, &sobjId ) )
            {
                dwNowWanted = DRM_UI64Low32( sqwSize );
                dwTotalUsed += dwNowWanted;
            }
            else
            {

                dwNowWanted = DRM_UI64Low32( sqwSize );
                ChkDR( _CopyObjectToHeaderBuffer( pAsfContext,
                                                  DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset),
                                                              DRM_UI64(dwTotalUsed)),
                                                  dwNowWanted ) );
                dwTotalUsed += dwNowWanted;
            }
        }
    }

    /* Add Advanced Content Encryption object to the new header buffer */
    ChkDR( _ASFAppendAdvContentEncryptionObject( pAsfTranscryptContext ) );

    for(wStreamNum = 0; wStreamNum < MAXSTREAMS; wStreamNum++)
    {
        if( pState->fFoundStream[wStreamNum] )
        {
          ChkBOOL( pState->fFoundStreamPropertiesObjectEx[wStreamNum], DRM_E_ASF_EXTENDED_STREAM_PROPERTIES_OBJ_NOT_FOUND );
        }
    }

    /* Skip data if it is left */
    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwObjSize ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



static DRM_RESULT _ASFParseHeaders(
    __inout NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_RESULT        dr = DRM_SUCCESS;
    DRM_GUID          objId;
    DRM_UINT64        qwSize;
    DRM_DWORD         dwSize = 0;
    ASF_PARSER_STATE *pState = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseHeaders );

    ChkArg( pAsfContext );

    pState = &(pAsfContext->ParserState);

    if(pState->fHeaderParsed)
    {
        goto ErrorExit;
    }

    pState->iCurrentFileOffset = DRM_I64( 0 );

    /* Parse ASF Header Object */
    ChkDR( _ASFLoadHeaderObject( pAsfContext ) );

    pState->iFirstPacketOffset = DRM_I64Add( DRM_UI2I64(pState->qwHeaderSize), DRM_I64( DATA_OBJECT_SIZE ) );
    ChkOverflowI64( pState->iFirstPacketOffset, DRM_I64( DATA_OBJECT_SIZE ) );

    /* Scan Header Objects */
    while( DRM_I64Les( pState->iCurrentFileOffset,
                       DRM_I64Sub( pState->iFirstPacketOffset, DRM_I64( DATA_OBJECT_SIZE ) ) ) )
    {
        ChkDR( _ASFLoadObjectHeader( pAsfContext, &objId, &qwSize, DRM_UI64(0) ) );
        dwSize = DRM_UI64Low32( qwSize );

        /*
        ** -----------------------------------
        ** Stream Properties Object
        ** -----------------------------------
        */
        if ( DRM_IsEqualGUID( &CLSID_ASFStreamPropertiesObjectV1, &objId ) )
        {
            ChkDR( _ASFParseStreamPropertiesObject( pAsfContext, dwSize, DRM_UI64(0), 0 ) );
            continue;
        }

        /*
        ** -----------------------------------
        ** Content Encryption Object
        ** -----------------------------------
        */
        else if ( DRM_IsEqualGUID( &CLSID_ASFContentEncryptionObject, &objId ) )
        {
            DRM_BYTE        *pData = NULL;
            DRM_DWORD dwWanted = 0;
            DRM_DWORD dwNowWanted = 0;
            DRM_DWORD dwThisBuffer = 0;
            DRM_DWORD dwTotalUsed = 0;
            DRM_DWORD dwDataLeft = 0;
            DRM_DWORD dwObjSize = dwSize;
            DRM_DWORD dwOffset = 0;

            ASF_CONTENT_ENCRYPTION_OBJECT *pContentEncObj;

            pState->cbContentEncryptionObject = dwSize;
            pState->fEncrypted = TRUE;

            ChkMem( pContentEncObj = (ASF_CONTENT_ENCRYPTION_OBJECT *)Oem_MemAlloc( SIZEOF( ASF_CONTENT_ENCRYPTION_OBJECT ) ) );
            ZEROMEM( pContentEncObj, SIZEOF( ASF_CONTENT_ENCRYPTION_OBJECT ) );
            pAsfContext->ASFFileInfo.pContentEncObj = pContentEncObj;

            /* Read the whole object into memory */
            /* FIXME does this load more than necessary */
            dwWanted = dwObjSize;
            dwThisBuffer = WMCBGetData( pAsfContext, DRM_I2UI64(pState->iCurrentFileOffset), dwWanted, &pData );
            if ( dwThisBuffer != dwWanted )
            {
                ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
            }

            /* Read Secret Data */
            dwNowWanted = SIZEOF( DRM_DWORD );
            if ( dwTotalUsed + dwNowWanted > dwObjSize )
            {
                ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
            }

            dwOffset = 0;
            LoadDWORDOffset( pContentEncObj->cbSecretData, pData, dwOffset );

            dwTotalUsed += SIZEOF( DRM_DWORD );

            if( pContentEncObj->cbSecretData > 0 )
            {
                ChkMem( pContentEncObj->pbSecretData = (DRM_BYTE*)Oem_MemAlloc( pContentEncObj->cbSecretData ) );
                ZEROMEM( pContentEncObj->pbSecretData, pContentEncObj->cbSecretData );

                dwDataLeft = pContentEncObj->cbSecretData;
                if ( dwTotalUsed + dwDataLeft > dwObjSize )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                DRM_BYT_CopyBytes( pContentEncObj->pbSecretData, 0, pData, dwOffset, pContentEncObj->cbSecretData );
                dwTotalUsed += dwDataLeft;
                dwOffset += dwDataLeft;
            }

            /* Read Protection Type */
            dwNowWanted = SIZEOF( DRM_DWORD );
            if ( dwTotalUsed + dwNowWanted > dwObjSize )
            {
                ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
            }

            LoadDWORDOffset( pContentEncObj->cbProtectionType, pData, dwOffset );

            dwTotalUsed += SIZEOF( DRM_DWORD );

            if ( pContentEncObj->cbProtectionType > 0 )
            {
                ChkMem( pContentEncObj->pbProtectionType = (DRM_BYTE*)Oem_MemAlloc( pContentEncObj->cbProtectionType ) );
                ZEROMEM( pContentEncObj->pbProtectionType, pContentEncObj->cbProtectionType );

                dwDataLeft = pContentEncObj->cbProtectionType;
                if ( dwTotalUsed + dwDataLeft > dwObjSize )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                DRM_BYT_CopyBytes( pContentEncObj->pbProtectionType, 0, pData, dwOffset, pContentEncObj->cbProtectionType );
                dwTotalUsed += dwDataLeft;
                dwOffset += dwDataLeft;
            }

            /* Read Key ID */
            dwNowWanted = SIZEOF( DRM_DWORD );
            if ( dwTotalUsed + dwNowWanted > dwObjSize )
            {
                ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
            }

            LoadDWORDOffset( pContentEncObj->cbKeyID, pData, dwOffset );

            dwTotalUsed += SIZEOF( DRM_DWORD );

            dwDataLeft = pContentEncObj->cbKeyID;
            if ( pContentEncObj->cbKeyID > 0 )
            {
                ChkMem( pContentEncObj->pbKeyID = (DRM_BYTE*)Oem_MemAlloc( pContentEncObj->cbKeyID ) );
                ZEROMEM( pContentEncObj->pbKeyID, pContentEncObj->cbKeyID );

                if ( dwTotalUsed + dwDataLeft > dwObjSize )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                DRM_BYT_CopyBytes( pContentEncObj->pbKeyID, 0, pData, dwOffset, pContentEncObj->cbKeyID );
                dwOffset += dwDataLeft;
                dwTotalUsed += dwDataLeft;
            }

            /* Read License URL */
            dwNowWanted = SIZEOF( DRM_DWORD );
            if ( dwTotalUsed + dwNowWanted > dwObjSize )
            {
                ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
            }

            LoadDWORDOffset( pContentEncObj->cbLicenseURL, pData, dwOffset);

            dwTotalUsed += SIZEOF( DRM_DWORD );

            if( pContentEncObj->cbLicenseURL > 0 )
            {
                ChkMem( pContentEncObj->pbLicenseURL= (DRM_BYTE*)Oem_MemAlloc( pContentEncObj->cbLicenseURL ) );
                ZEROMEM( pContentEncObj->pbLicenseURL, pContentEncObj->cbLicenseURL );

                dwDataLeft = pContentEncObj->cbLicenseURL;
                if ( dwTotalUsed + dwDataLeft > dwObjSize )
                {
                    ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
                }

                DRM_BYT_CopyBytes( pContentEncObj->pbLicenseURL, 0, pData, dwOffset, pContentEncObj->cbLicenseURL );
                dwTotalUsed += dwDataLeft;
                dwOffset += dwDataLeft;
            }

        }

        /*
        ** -----------------------------------
        ** Extended Content Encryption Object
        ** -----------------------------------
        */
        else if ( DRM_IsEqualGUID( &CLSID_ASFContentEncryptionObjectEx, &objId ) )
        {
            DRM_BYTE  *pData = NULL;
            DRM_DWORD dwWanted;
            DRM_DWORD dwThisBuffer;
            ASF_FILE_INFO *pFileInfo = &pAsfContext->ASFFileInfo;
            DRM_DWORD dwObjSize = dwSize;

            pState->cbContentEncryptionObjectEx = dwSize;
            pState->fEncrypted = TRUE;

            ChkMem( pFileInfo->pContentEncObjExt = (ASF_CONTENT_ENCRYPTION_OBJECT_EXT *)Oem_MemAlloc( SIZEOF( ASF_CONTENT_ENCRYPTION_OBJECT_EXT ) ) );
            ZEROMEM( pFileInfo->pContentEncObjExt, SIZEOF( ASF_CONTENT_ENCRYPTION_OBJECT_EXT ) );

            /* Read the whole object into memory */
            dwWanted = dwObjSize;
            dwThisBuffer = WMCBGetData( pAsfContext, DRM_I2UI64(pState->iCurrentFileOffset), dwWanted, &pData );
            if ( dwThisBuffer != dwWanted )
            {
                ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
            }

            ChkMem( pFileInfo->pContentEncObjExt->pbData = (DRM_BYTE*)Oem_MemAlloc( dwWanted ) );
            pFileInfo->pContentEncObjExt->cbData = dwWanted;

            MEMCPY( pFileInfo->pContentEncObjExt->pbData, pData, dwWanted );
        }


        /*
        ** -----------------------------------
        ** PlayReady Object
        ** -----------------------------------
        */
        else if ( DRM_IsEqualGUID( &CLSID_ASFPlayReadyObject, &objId ) )
        {
            DRM_UINT64 ui64PrevOffset = DRM_UI64LITERAL( 0, 0 );

            pState->fPlayReadyObjectInsideEHO = FALSE;
            ChkDR( _ASFParsePlayReadyObject( pAsfContext, pState, dwSize, ui64PrevOffset ) );
        }

        /*
        ** -----------------------------------
        ** File Properties Object
        ** -----------------------------------
        */
        else if ( DRM_IsEqualGUID( &CLSID_ASFFilePropertiesObjectV2, &objId ) )
        {
            /*  The code right now takes care of v2 of the object
             *  need to add code to handle v0 and v1 (?)
             */
            ChkDR( _ASFParseFilePropertiesObject( pAsfContext, dwSize ) );
            continue;
        }

        /*
        ** -----------------------------------
        ** Stream bitrate properties object
        ** -----------------------------------
        */
        else if ( DRM_IsEqualGUID( &CLSID_ASFStreamBitratePropertiesObject, &objId ) )
        {
            ChkDR( _ASFParseStreamBitratePropertiesObject( pAsfContext, dwSize ) );
            continue;
        }

        /*
        ** -----------------------------------
        ** Header Extension Object, a.k.a Clock Object
        ** -----------------------------------
        */
        else if ( DRM_IsEqualGUID( &CLSID_ASFHeaderExtensionObject, &objId ) )
        {
            ChkDR( _ASFParseHeaderExtensionObject( pAsfContext, dwSize ) );
            continue;
        }

        ChkBOOL( dwSize >= MIN_OBJ_SIZE, DRM_E_ASF_BAD_ASF_HEADER );
        dwSize -= MIN_OBJ_SIZE;
        pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwSize ) );
    }

    pState->iLastPacketOffset = pState->iFirstPacketOffset;

    if ( pState->dwTotalPackets > 0 )
    {
        pState->iLastPacketOffset = DRM_I64Add(pState->iLastPacketOffset, DRM_I64((pState->dwTotalPackets - 1 ) * pState->dwPacketSize));
    }

    /* if pState->iCurrentFileOffset != pState->iFirstPacketOffset - DATA_OBJECT_SIZE */
    if ( ! DRM_I64Eql( pState->iCurrentFileOffset, DRM_I64Sub( pState->iFirstPacketOffset, DRM_I64( DATA_OBJECT_SIZE ) ) ) )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    pState->fHeaderParsed = TRUE;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



static DRM_RESULT _ASFProcessHeaders(
    __in_opt   DRM_VOID        * const f_pOEMContext,
    __inout    NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_RESULT        dr = DRM_SUCCESS;
    DRM_GUID          objId;
    DRM_UINT64        qwSize;
    DRM_DWORD         dwSize = 0;
    ASF_PARSER_STATE *pState = NULL;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;
    DRM_WORD          wStreamNum;
    DRM_DWORD         cbExtraStreamPropertiesObjectEx = 0;
    DRM_UINT64        qwSaveSize;
    DRM_DWORD         dwNoOfHeaders = 0;
    DRM_DWORD         cbCurrHeaderSize = 0;
    DRM_DWORD         cbNextHeaderSize = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessHeaders );

    ChkArg( pAsfContext );

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);

    /* Parse the header if we haven't already. */
    if(! pState->fHeaderParsed )
    {
        ChkDR( _ASFParseHeaders( pAsfContext ) );
    }

    /* Determine which streams need extended stream properties objects */
    for(wStreamNum = 0; wStreamNum < MAXSTREAMS; wStreamNum++)
    {
        if( pState->fFoundStream[wStreamNum])
        {
            /*
            ** If the file is clear or RC4/COCKTAIL encrypted, then we will be
            ** adding the ND Payload Extension to the Extended Stream Properties object.
            ** If the file is already AES encrypted, then this data is already present.
            */
            if ( pAsfContext->eCipherType != eDRM_AES_COUNTER_CIPHER )
            {
                cbExtraStreamPropertiesObjectEx += WMDRM_ND_PAYLOAD_EXTENSION_SIZE;
            }

            /*
            ** If the file is missing the entire Extended Stream Properties object,
            ** then we will be adding that as well.
            */
            if( ! pState->fFoundStreamPropertiesObjectEx[wStreamNum] )
            {
                cbExtraStreamPropertiesObjectEx += ASF_STREAM_PROPERTIES_OBJECT_EX_SIZE;
            }
        }
    }

    SAFE_OEM_FREE( pAsfTranscryptContext->Header.pbHeader );

    /* Calculate transcrypted header size */
    ChkOverflow( cbExtraStreamPropertiesObjectEx + ASF_TRANSCRYPT_WMDRMND_OBJECT_SIZE,
                 cbExtraStreamPropertiesObjectEx ); /* Verify increase */

    pAsfTranscryptContext->Header.cbHeaderExtensionObjectExtra =
        cbExtraStreamPropertiesObjectEx +
        ASF_TRANSCRYPT_WMDRMND_OBJECT_SIZE;

    /*
    ** Calculate adjusted header size for ASF header block:
    ** 1. Remove V2 ("Ex" below) and PlayReady objects.
    ** 2. Update V1 object (remove old, add new).
    ** 3. Include room for extra header extension metadata as well.
    */
    cbCurrHeaderSize = pState->cbOriginalHeaderSize;
    cbNextHeaderSize = cbCurrHeaderSize - pState->cbContentEncryptionObject;
    ChkOverflow( cbCurrHeaderSize, cbNextHeaderSize ); /* Verify decrease */

    cbCurrHeaderSize = cbNextHeaderSize;
    cbNextHeaderSize = cbCurrHeaderSize - pState->cbContentEncryptionObjectEx;
    ChkOverflow( cbCurrHeaderSize, cbNextHeaderSize ); /* Verify decrease */

    cbCurrHeaderSize = cbNextHeaderSize;
    cbNextHeaderSize = cbCurrHeaderSize - pState->cbPlayReadyObject;
    ChkOverflow( cbCurrHeaderSize, cbNextHeaderSize ); /* Verify decrease */

    cbCurrHeaderSize = cbNextHeaderSize;
    cbNextHeaderSize = cbCurrHeaderSize + WMDRM_ND_CONTENT_ENCRYPTION_OBJECT_SIZE;
    ChkOverflow( cbNextHeaderSize, cbCurrHeaderSize ); /* Verify increase */

    cbCurrHeaderSize = cbNextHeaderSize;
    cbNextHeaderSize = cbCurrHeaderSize + pAsfTranscryptContext->Header.cbHeaderExtensionObjectExtra;
    ChkOverflow( cbNextHeaderSize, cbCurrHeaderSize ); /* Verify increase */
    pAsfTranscryptContext->Header.cbHeaderSize = cbNextHeaderSize;

    ChkMem( pAsfTranscryptContext->Header.pbHeader = (DRM_BYTE*)
            Oem_MemAlloc( pAsfTranscryptContext->Header.cbHeaderSize ) );

    pAsfTranscryptContext->Header.dwHeaderOffset = 0;
    SaveGUIDOffset( pAsfTranscryptContext->Header.pbHeader,
                    CLSID_ASFHeaderObjectV0,
                    pAsfTranscryptContext->Header.dwHeaderOffset );

    qwSaveSize = DRM_UI64( pAsfTranscryptContext->Header.cbHeaderSize );
    SaveQWORDOffset( pAsfTranscryptContext->Header.pbHeader,
                     qwSaveSize,
                     pAsfTranscryptContext->Header.dwHeaderOffset );

    dwNoOfHeaders = pState->dwNoOfHeaders;

    /*
    ** Decrement header count for the headers we are removing entirely.
    */
    if( pState->cbContentEncryptionObjectEx )
    {
        dwNoOfHeaders--;
    }

    if( pState->cbPlayReadyObject && !pState->fPlayReadyObjectInsideEHO )
    {
        dwNoOfHeaders--;
    }

    /*
    ** If we are transcrypting clear content or PlayReady content, we will be
    ** adding a Content Encryption Object to the ASF header.
    */
    if ( pAsfContext->fEncryptClear ||
         pAsfContext->fIsPlayReady )
    {
        dwNoOfHeaders++;
    }

    /* Set the number-of-header objects DWORD */
    SaveDWORDOffset( pAsfTranscryptContext->Header.pbHeader,
                     dwNoOfHeaders,
                     pAsfTranscryptContext->Header.dwHeaderOffset );

    /* Set the alignment byte */
    PUT_BYTE( pAsfTranscryptContext->Header.pbHeader,
              pAsfTranscryptContext->Header.dwHeaderOffset,
              1 );
    pAsfTranscryptContext->Header.dwHeaderOffset++;

    /* Set the architecture byte */
    PUT_BYTE( pAsfTranscryptContext->Header.pbHeader,
              pAsfTranscryptContext->Header.dwHeaderOffset,
              2 );
    pAsfTranscryptContext->Header.dwHeaderOffset++;

    /* Seek to first header object */
    pState->iCurrentFileOffset = DRM_I64( SIZEOF(DRM_GUID) + SIZEOF(DRM_UINT64) + SIZEOF(DRM_DWORD) + SIZEOF_U8 + SIZEOF_U8 );

    pState->iFirstPacketOffset = DRM_I64Add( DRM_UI2I64(pState->qwHeaderSize), DRM_I64( DATA_OBJECT_SIZE ) );
    ChkOverflowI64( pState->iFirstPacketOffset, DRM_I64( DATA_OBJECT_SIZE ) );

    /* Scan Header Objects */
    while( DRM_I64Les( pState->iCurrentFileOffset,
                       DRM_I64Sub( pState->iFirstPacketOffset, DRM_I64( DATA_OBJECT_SIZE ) ) ) )
    {
        ChkDR( _ASFLoadObjectHeader( pAsfContext, &objId, &qwSize, DRM_UI64(0) ) );
        dwSize = DRM_UI64Low32( qwSize );

        /* Content Encryption Object */
        if ( DRM_IsEqualGUID( &CLSID_ASFContentEncryptionObject, &objId ) )
        {
            /* We explicitly don't save the header object here.
             * We will calculate the new header inside _ASFProcessContentEncryptionObject
            */
            ChkDR( _ASFProcessContentEncryptionObject( pAsfContext, dwSize ) );
            pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwSize - MIN_OBJ_SIZE ) );
        }
        /* Extended Content Encryption Object */
        else if ( DRM_IsEqualGUID( &CLSID_ASFContentEncryptionObjectEx, &objId ) )
        {
            pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwSize - MIN_OBJ_SIZE ) );
        }
        /* PlayReady Object */
        else if ( DRM_IsEqualGUID( &CLSID_ASFPlayReadyObject, &objId ) )
        {
            pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwSize - MIN_OBJ_SIZE ) );
        }
        /* File Properties Object */
        else if ( DRM_IsEqualGUID( &CLSID_ASFFilePropertiesObjectV2, &objId ) )
        {
            /*  The code right now takes care of v2 of the object
             *  need to add code to handle v0 and v1 (?)
             */
            ChkDR( _SaveHeaderObjectToNewHeader( pAsfTranscryptContext, &objId, &qwSize ) );
            ChkDR( _ASFProcessFilePropertiesObject( f_pOEMContext, pAsfContext, dwSize ) );
        }
        /* Header Extension Object, a.k.a Clock Object */
        else if ( DRM_IsEqualGUID( &CLSID_ASFHeaderExtensionObject, &objId ) )
        {
            qwSize = DRM_UI64Add( qwSize, DRM_UI64( pAsfTranscryptContext->Header.cbHeaderExtensionObjectExtra ) );
            if( pState->fPlayReadyObjectInsideEHO )
            {
                ChkBOOL( DRM_UI64Les( DRM_UI64( pState->cbPlayReadyObject ), qwSize ) || DRM_UI64Eql( DRM_UI64( pState->cbPlayReadyObject ), qwSize ), DRM_E_ARITHMETIC_OVERFLOW );
                qwSize = DRM_UI64Sub( qwSize, DRM_UI64( pState->cbPlayReadyObject ) );
            }

            ChkDR( _SaveHeaderObjectToNewHeader( pAsfTranscryptContext, &objId, &qwSize ) );
            ChkDR( _ASFProcessHeaderExtensionObject( pAsfContext, dwSize ) );
        }
        /* Stream Properties Object */
        else if ( DRM_IsEqualGUID( &CLSID_ASFStreamPropertiesObjectV1, &objId )  )
        {
            ChkDR( _SaveHeaderObjectToNewHeader( pAsfTranscryptContext, &objId, &qwSize ) );
            dwSize -= MIN_OBJ_SIZE;
            ChkDR( _ASFProcessStreamPropertiesObject( pAsfContext, dwSize, DRM_UI64( 0 ) ) );
            pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwSize ) );
        }
        else
        {
            /* Copy this object verbatim to new header buffer */

            /* Save object header first */
            ChkDR( _SaveHeaderObjectToNewHeader( pAsfTranscryptContext, &objId, &qwSize ) );

            dwSize -= MIN_OBJ_SIZE;
            ChkDR( _CopyObjectToHeaderBuffer( pAsfContext,
                                              DRM_I2UI64(pState->iCurrentFileOffset),
                                              dwSize ) );

            pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwSize ) );
        }
    }


    /*
    ** If we are transcrypting clear content, or if we are processing a PMF file, then
    ** we must add a Content Encryption Object to the ASF header.
    */
    if ( ( pAsfContext->fEncryptClear ) || ( pAsfContext->fIsPlayReady ) )  /* a PMF file */
    {
        ChkDR( _ASFProcessContentEncryptionObject( pAsfContext, WMDRM_ND_CONTENT_ENCRYPTION_OBJECT_SIZE ) );
    }


    pState->iLastPacketOffset = pState->iFirstPacketOffset;

    if ( pState->dwTotalPackets > 0 )
    {
        pState->iLastPacketOffset = DRM_I64Add(pState->iLastPacketOffset, DRM_I64((pState->dwTotalPackets - 1 ) * pState->dwPacketSize));
    }

    /* if pState->iCurrentFileOffset != pState->iFirstPacketOffset - DATA_OBJECT_SIZE */
    if ( ! DRM_I64Eql( pState->iCurrentFileOffset, DRM_I64Sub( pState->iFirstPacketOffset, DRM_I64( DATA_OBJECT_SIZE ) ) ) )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT _ParseLengthType(
    __inout     NDT_ASF_CONTEXT * const pAsfContext,
    __in const  DRM_BYTE                bLenType,
    __out       DRM_DWORD       * const pdwTotalUsed,
    __out       DRM_DWORD       * const pdwSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwNowWanted = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwOffset = 0;
    ASF_PARSER_STATE *pState = &(pAsfContext->ParserState);
    ASF_PACKET_PARSER_INFO *pPacketInfo = pState->pPacketInfo;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ParseLengthType );

    switch( bLenType )
    {
    case 0x01:
        {
            DRM_BYTE b = 0;

            /* Can't be */
            dwNowWanted = SIZEOF_U8;
            dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(*pdwTotalUsed)), dwNowWanted, &pData );
            if ( dwThisBuffer != dwNowWanted )
            {
                ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
            }

            *pdwTotalUsed += dwNowWanted;
            dwOffset = 0;
            LoadBYTEOffset( b, pData, dwOffset );

            *pdwSize = (DRM_DWORD)(b);
            pPacketInfo->dwParseOffset++;

            break;
        }

    case 0x02:
        {
            /* Can be */
            DRM_WORD w = 0;

            dwNowWanted = SIZEOF( DRM_WORD );
            dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(*pdwTotalUsed)), dwNowWanted, &pData );
            if ( dwThisBuffer != dwNowWanted )
            {
                ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
            }

            *pdwTotalUsed += dwNowWanted;
            dwOffset = 0;
            LoadWORDOffset( w, pData, dwOffset );

            *pdwSize = (DRM_DWORD)(w);
            pPacketInfo->dwParseOffset += SIZEOF( DRM_WORD );

            break;
        }

    case 0x03:
        {
            /* Can be */
            DRM_DWORD dw = 0;

            dwNowWanted = SIZEOF( DRM_DWORD );
            dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(*pdwTotalUsed)), dwNowWanted, &pData );
            if ( dwThisBuffer != dwNowWanted )
            {
                ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
            }

            *pdwTotalUsed += dwNowWanted;
            dwOffset = 0;
            LoadDWORDOffset( dw, pData, dwOffset );

            *pdwSize = (DRM_DWORD)(dw);
            pPacketInfo->dwParseOffset += SIZEOF( DRM_DWORD );

            break;
        }

    default:
        break;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT _ASFParseLengthType(
    __inout                        ASF_PARSER_STATE * const pState,
    __in                           OEM_FILEHDL              hInFile,
    __out_bcount(FILE_BUFFER_SIZE) DRM_BYTE                *pbFileBuffer,
    __in const                     DRM_BYTE                 bLenType,
    __out                          DRM_DWORD        * const pdwTotalUsed,
    __out                          DRM_DWORD        * const pdwSize
)
{
    DRM_RESULT              dr = DRM_SUCCESS;
    DRM_DWORD               dwNowWanted = 0;
    DRM_DWORD               dwOffset = 0;
    ASF_PACKET_PARSER_INFO *pPacketInfo = pState->pPacketInfo;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParseLengthType );

    DRMASSERT( NULL != pState );
    DRMASSERT( OEM_INVALID_HANDLE_VALUE != hInFile );
    DRMASSERT( NULL != pbFileBuffer );
    DRMASSERT( NULL != pdwTotalUsed );
    DRMASSERT( NULL != pdwSize );

    switch( bLenType )
    {
    case 0x01:
        {
            DRM_BYTE b = 0;

            /* Can't be */
            dwNowWanted = SIZEOF_U8;
            ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );

            *pdwTotalUsed += dwNowWanted;
            dwOffset = 0;
            LoadBYTEOffset( b, pbFileBuffer, dwOffset );

            *pdwSize = (DRM_DWORD)(b);
            pPacketInfo->dwParseOffset++;

            break;
        }

    case 0x02:
        {
            /* Can be */
            DRM_WORD w = 0;

            dwNowWanted = SIZEOF( DRM_WORD );
            ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );

            *pdwTotalUsed += dwNowWanted;
            dwOffset = 0;
            LoadWORDOffset( w, pbFileBuffer, dwOffset );

            *pdwSize = (DRM_DWORD)(w);
            pPacketInfo->dwParseOffset += SIZEOF( DRM_WORD );

            break;
        }

    case 0x03:
        {
            /* Can be */
            DRM_DWORD dw = 0;

            dwNowWanted = SIZEOF( DRM_DWORD );
            ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );

            *pdwTotalUsed += dwNowWanted;
            dwOffset = 0;
            LoadDWORDOffset( dw, pbFileBuffer, dwOffset );

            *pdwSize = (DRM_DWORD)(dw);
            pPacketInfo->dwParseOffset += SIZEOF( DRM_DWORD );

            break;
        }

    default:
        break;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT _ASFProcessPacketHeader(
    __inout NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  b = 0;
    DRM_DWORD dwNowWanted = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwOffset = 0;
    DRM_DWORD dwDataOffset = 0;
    DRM_BYTE  *pData = NULL;
    DRM_BOOL  fTranscrypting;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;
    ASF_PARSER_STATE *pState = NULL;
    ASF_PACKET_PARSER_INFO *pPacketInfo = NULL;
    DRM_BYTE *pDataBuffer = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessPacketHeader );

    pState = &(pAsfContext->ParserState);
    pPacketInfo = pState->pPacketInfo;
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);

    fTranscrypting = TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode;
    if ( TRUE == fTranscrypting )
    {
        pDataBuffer = pAsfTranscryptContext->Data.DataContext.pbDataBuffer;
        dwOffset = pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset;
    }

    /* Copy Error Correction data straight to the data buffer */
    dwNowWanted = SIZEOF_U8;
    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
         ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
    }

    dwDataOffset = 0;
    LoadBYTEOffset( b, pData, dwDataOffset );
    dwTotalUsed += dwNowWanted;

    pPacketInfo->fParityPacket = FALSE;
    pPacketInfo->dwParseOffset = 0;

    pPacketInfo->fEccPresent = (DRM_BOOL)( ( b & 0x80 ) == 0x80 );
    pPacketInfo->bECLen = 0;

    if ( pPacketInfo->fEccPresent )
    {
        /* Error correction Data is there */
        /*
        if ( b & 0x10 )
        {
            /* Means Opaque Data, Not Payloads */
            /*
            pPacketInfo->fParityPacket = TRUE;
            return DRM_SUCCESS;
        }
        */

        if ( b & 0x60 )
        {
            /* If error correction data is NOT coded with 4 bits */
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        pPacketInfo->bECLen = ( b & 0x0f );
        if ( pPacketInfo->bECLen != 2 )
        {
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        dwNowWanted = (DRM_DWORD)pPacketInfo->bECLen;
        pPacketInfo->dwParseOffset = (DRM_DWORD)( 1 + pPacketInfo->bECLen );

        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        dwTotalUsed += dwNowWanted;

        if ( TRUE == fTranscrypting )
        {
            /* Save Error correction data into data buffer */
            SaveBYTEOffset( pDataBuffer, b, dwOffset );

            DRM_BYT_CopyBytes( pDataBuffer, dwOffset, pData, 0, dwNowWanted);
            dwOffset += dwNowWanted;
        }

        /* Read first byte of the payload parsing information */
        dwNowWanted = SIZEOF_U8;
        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        dwTotalUsed += dwNowWanted;
        dwDataOffset = 0;
        LoadBYTEOffset( b, pData, dwDataOffset );
    }

    pPacketInfo->dwPacketLenTypeOffset = pPacketInfo->dwParseOffset;

    /* Padding length type */
    pPacketInfo->bPadLenType = ( b & 0x18 ) >> 3;
    if ( pPacketInfo->bPadLenType == 3 )
    {
       ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
    }

    /* Packet len type */
    pPacketInfo->bPacketLenType = ( b & 0x60 ) >> 5;

    if ( pPacketInfo->bPacketLenType != 0 &&
         pPacketInfo->bPacketLenType != 2 )
    {
        ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
    }

    /* Must be 0 */
    pPacketInfo->bSequenceLenType = ( b & 0x06 ) >> 1;

    pPacketInfo->fMultiPayloads = (DRM_BOOL)( b & 0x01 );

    /* We only support 1 payload per packet */
    DRMASSERT( pPacketInfo->fMultiPayloads == FALSE );

    if ( TRUE == fTranscrypting )
    {
        SaveBYTEOffset( pDataBuffer, b, dwOffset );
    }
    pPacketInfo->dwParseOffset++;

    /*
     * Read property flags
     */
    dwNowWanted = SIZEOF_U8;
    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
    }

    dwTotalUsed += dwNowWanted;
    dwDataOffset = 0;
    LoadBYTEOffset( b, pData, dwDataOffset );

    /* Offset into media object length type should always be DWORD */
    pPacketInfo->bOffsetLenType = 3;

    /* It must be 5d according to specs */
    if ( b != 0x5d )
    {
        /* Stream Number len type is not 01 (bits 6,7) */
        if ( ( b & 0xc0 ) != 0x40 )
        {
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        if ( ( b & 0x30 ) != 0x10 )
        {
            /* Obj number type len is not 01 */
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        /* Bits 2,3 */
        pPacketInfo->bOffsetLenType = ( b & 0x0c ) >> 2;

        /* Should be 11 */
        if ( pPacketInfo->bOffsetLenType == 0 )
        {
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }
        else if ( pPacketInfo->bOffsetLenType < 3 )
        {
            /* not in spec */
            DRMASSERT( !"Offset into Media Object length type not in spec" );
        }

        if ( ( b & 0x03 ) != 0x01 )
        {
            /* Rep data len type is not 01 (bit 0,1) */
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }
    }

    if ( TRUE == fTranscrypting )
    {
        SaveBYTEOffset( pDataBuffer, b, dwOffset );
    }
    pPacketInfo->dwParseOffset++;

    /* Packet length type */
    pPacketInfo->dwPacketLenOffset = pPacketInfo->dwParseOffset;

    ChkDR( _ParseLengthType( pAsfContext,
                             pPacketInfo->bPacketLenType,
                             &dwTotalUsed,
                             &(pPacketInfo->dwExplicitPacketLength) )  );

    if ( TRUE == fTranscrypting )
    {
        if ( pPacketInfo->bPacketLenType == 0x01 )
        {
            SaveBYTEOffset( pDataBuffer, (DRM_BYTE)pPacketInfo->dwExplicitPacketLength, dwOffset );
        }
        else if ( pPacketInfo->bPacketLenType == 0x02 )
        {
            DRM_WORD temp = (DRM_WORD)pPacketInfo->dwExplicitPacketLength;
            SaveWORDOffset( pDataBuffer, temp, dwOffset );
        }
        else if ( pPacketInfo->bPacketLenType == 0x03 )
        {
            SaveDWORDOffset( pDataBuffer, pPacketInfo->dwExplicitPacketLength, dwOffset );
        }
    }
    /* Sequence length type */
    pPacketInfo->dwSequenceOffset = pPacketInfo->dwParseOffset;

    ChkDR( _ParseLengthType( pAsfContext,
                             pPacketInfo->bSequenceLenType,
                             &dwTotalUsed,
                             &(pPacketInfo->dwSequenceNum) )  );

    if ( TRUE == fTranscrypting )
    {
        if ( pPacketInfo->bSequenceLenType == 0x01 )
        {
            SaveBYTEOffset( pDataBuffer, (DRM_BYTE)pPacketInfo->dwSequenceNum, dwOffset );
        }
        else if ( pPacketInfo->bSequenceLenType == 0x02 )
        {
            DRM_WORD temp = (DRM_WORD)pPacketInfo->dwSequenceNum;
            SaveWORDOffset( pDataBuffer, temp, dwOffset );
        }
        else if ( pPacketInfo->bSequenceLenType == 0x03 )
        {
            SaveDWORDOffset( pDataBuffer, pPacketInfo->dwSequenceNum, dwOffset );
        }
    }

    /* Padding length type */
    pPacketInfo->dwPadLenOffset = pPacketInfo->dwParseOffset;

    ChkDR( _ParseLengthType( pAsfContext,
                             pPacketInfo->bPadLenType,
                             &dwTotalUsed,
                             &(pPacketInfo->dwPadding) )  );

    if ( TRUE == fTranscrypting )
    {
        if ( pPacketInfo->bPadLenType == 0x01 )
        {
            SaveBYTEOffset( pDataBuffer, (DRM_BYTE)pPacketInfo->dwPadding, dwOffset );
        }
        else if ( pPacketInfo->bPadLenType == 0x02 )
        {
            DRM_WORD temp = (DRM_WORD)pPacketInfo->dwPadding;
            SaveWORDOffset( pDataBuffer, temp, dwOffset );
        }
        else if ( pPacketInfo->bPadLenType == 0x03 )
        {
            SaveDWORDOffset( pDataBuffer, pPacketInfo->dwPadding, dwOffset );
        }
    }

    /* Now read Send Time and Duration (6 bytes) */
    dwNowWanted = SIZEOF( DRM_DWORD ) + SIZEOF( DRM_WORD );
    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
    }

    dwTotalUsed += dwNowWanted;

    if ( TRUE == fTranscrypting )
    {
        DRM_BYT_CopyBytes( pDataBuffer, dwOffset, pData, 0, dwNowWanted );
    }
    dwOffset += dwNowWanted;

    LoadDWORDOffset( pPacketInfo->dwSCR, pData, dwDataOffset );
    LoadWORDOffset( pPacketInfo->wDuration, pData, dwDataOffset );

    pPacketInfo->dwParseOffset += dwNowWanted ;

    if ( pPacketInfo->fEccPresent && pPacketInfo->fParityPacket )
    {
        ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
    }

    pPacketInfo->dwPayLenTypeOffset = 0;
    pPacketInfo->bPayLenType = 0;
    pPacketInfo->bPayBytes = 0;
    pPacketInfo->dwPayloads = 1;

    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( pPacketInfo->dwParseOffset ) );

    if ( TRUE == fTranscrypting )
    {
        pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset += pPacketInfo->dwParseOffset;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/******************************************************************************/
static DRM_RESULT _ASFProcessPayloadHeader(
    __inout NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwPayloadSize = 0;
    DRM_BYTE b = 0;
    DRM_BYTE  *pData = NULL;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwNowWanted = 0;
    ASF_PACKET_PARSER_INFO  *pPacketInfo = NULL;
    ASF_PAYLOAD_PARSER_INFO *pPayloadInfo = NULL;
    DRM_DWORD dwRepDataOffset = 0;
    DRM_WORD wTotalDataBytes = 0;
    ASF_PARSER_STATE *pState;
    DRM_BYTE *pDataBuffer = NULL;
    DRM_BOOL fTranscrypting;
    DRM_WORD wSaveSize = 0;
    DRM_DWORD dwOffsetInfoSize = 0;
    DRM_DWORD dwOffset = 0;
    DRM_DWORD dwDataOffset = 0;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFProcessPayloadHeader );

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    fTranscrypting = TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode;

    /* pState->iCurrentPacketOffset +
               pState->pPacketInfo->dwParseOffset >=
               pState->iCurrentFileOffset */
    /* It's possible this ASSERT isn't valid if seeking by time is supported */
    DRMASSERT(!DRM_I64Les(pState->iCurrentFileOffset, DRM_I64Add(pState->iCurrentPacketOffset, DRM_I64(pState->pPacketInfo->dwParseOffset) ) ) );

    /*
    if pState->iCurrentPacketOffset + pState->pPacketInfo->dwParseOffset > pState->iCurrentFileOffset */
    if ( DRM_I64Les(
            pState->iCurrentFileOffset,
            DRM_I64Add( pState->iCurrentPacketOffset, DRM_I64( pState->pPacketInfo->dwParseOffset ) )
         )
    )
    {
        /* Skip data. */
        /* dwNowWanted = pState->iCurrentPacketOffset +
         *      pState->pPacketInfo->dwParseOffset - pState->iCurrentFileOffset
         */
        dwNowWanted = DRM_I64ToUI32(
            DRM_I64Sub(
                DRM_I64Add( pState->iCurrentPacketOffset, DRM_I64(pState->pPacketInfo->dwParseOffset) ),
                pState->iCurrentFileOffset
            )
        );

        pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwNowWanted ) );
    }

    pPacketInfo = pState->pPacketInfo;
    pPayloadInfo = pState->pPayloadInfo;

    if ( TRUE == fTranscrypting )
    {
        pDataBuffer = pAsfTranscryptContext->Data.DataContext.pbDataBuffer;
        dwOffset = pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset;
    }

    /* Read Stream ID and Media Object Number */
    dwNowWanted = 2 * SIZEOF_U8 ;
    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
    }

    dwTotalUsed += dwNowWanted;
    if ( TRUE == fTranscrypting )
    {
        dwDataOffset = 0;
        /* Save them to the data buffer */
        DRM_BYT_CopyBytes( pDataBuffer, dwOffset, pData, dwDataOffset, dwNowWanted );
    }
    dwOffset += dwNowWanted;

    LoadBYTEOffset( b, pData, dwDataOffset );

    pPayloadInfo->wPacketOffset = (DRM_WORD)pPacketInfo->dwParseOffset;

    pPayloadInfo->bStreamId = b & 0x7f;
    pPayloadInfo->fIsKeyFrame = ( ( b & 0x80 ) >> 7 );

    LoadBYTEOffset( b, pData, dwDataOffset );

    /* Only byte is accepted */
    pPayloadInfo->bObjectId = b;

    /*  Find out the Extn Data Size for this stream id */

    ChkDR( _ParseLengthType( pAsfContext,
                             pPacketInfo->bOffsetLenType,
                             &dwTotalUsed,
                             &(pPayloadInfo->dwObjectOffset) )  );

    if ( TRUE == fTranscrypting )
    {
        if ( pPacketInfo->bOffsetLenType == 0x01 )
        {
            SaveBYTEOffset( pDataBuffer, (DRM_BYTE)pPayloadInfo->dwObjectOffset, dwOffset );
            dwOffsetInfoSize = SIZEOF_U8;
        }
        else if ( pPacketInfo->bOffsetLenType == 0x02 )
        {
            DRM_WORD temp = (DRM_WORD)pPayloadInfo->dwObjectOffset;
            SaveWORDOffset( pDataBuffer, temp, dwOffset );
            dwOffsetInfoSize = SIZEOF( DRM_WORD );
        }
        else if ( pPacketInfo->bOffsetLenType == 0x03 )
        {
            SaveDWORDOffset( pDataBuffer, pPayloadInfo->dwObjectOffset, dwOffset );
            dwOffsetInfoSize = SIZEOF( DRM_DWORD );
        }
    }
    else
    {
        dwOffsetInfoSize = SIZEOF( DRM_DWORD );
    }

    /* The size of the offset into media object field must be subtracted from
       the parse offset because it is accounted for in the total size (from
       the replicated data offset.  This is a fix for padding lengths other
       than 4, which happened to coincide with the size of this field.
       We can't subtract the field size from wTotalSize as it's used
       to calculate the payload length. */
    pPacketInfo->dwParseOffset -= dwOffsetInfoSize;

    /* Replicated data offset (from payload's perspective)
     * Replicated data offset = stream number length + media object number length
     * + offset into media object length
    */
    dwRepDataOffset = SIZEOF_U8 + SIZEOF_U8 + dwOffsetInfoSize;

    /* Read Replicated Data length */
    dwNowWanted = SIZEOF_U8;
    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
    }

    dwTotalUsed += dwNowWanted;
    dwDataOffset = 0;
    LoadBYTEOffset( b, pData, dwDataOffset );

    /* RepData Length. Only byte is accepted. */
    pPayloadInfo->bRepData = b;
    pPayloadInfo->dwObjectPres = 0xffffffff;

    /*
    ** The Replicated Data will be grown by some number of bytes
    ** to store the a sample ID size (WORD) and, in some cases, the sample ID itself (QWORD).
    ** Here, we update this Replicated Data size value.
    */
    if ( TRUE == fTranscrypting )
    {
        SaveBYTEOffset( pDataBuffer,
                        b + pAsfContext->dwAesPayloadExtensionAdditionalBytes,
                        dwOffset );
    }

    /* If replicated data length is set to 1, this is compressed payload data */
    if ( pPayloadInfo->bRepData == 1 )
    {
        pPayloadInfo->fIsCompressedPayload = TRUE;
        pPayloadInfo->dwObjectPres = pPayloadInfo->dwObjectOffset;
        pPayloadInfo->dwObjectOffset = 0;
        pPayloadInfo->dwObjectSize = 0;

        /* Read Presentation Time Delta */
        dwNowWanted = SIZEOF_U8;
        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
        }

        dwTotalUsed += dwNowWanted;
        dwDataOffset = 0;
        LoadBYTEOffset( b, pData, dwDataOffset );

        pPayloadInfo->dwDeltaPresTime = b;
        wTotalDataBytes = 0;
    }
    else if( pPayloadInfo->bRepData >= 8 )
    {
        pPayloadInfo->fIsCompressedPayload = FALSE;

        /* Read Media Object Size and Media Object Presentation Time */
        dwNowWanted = 2 * SIZEOF( DRM_DWORD );
        dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
        }

        dwTotalUsed += dwNowWanted;
        dwDataOffset = 0;
        LoadDWORDOffset( pPayloadInfo->dwObjectSize, pData, dwDataOffset );
        LoadDWORDOffset( pPayloadInfo->dwObjectPres, pData, dwDataOffset );

        if ( TRUE == fTranscrypting )
        {
            /* Save current replicated data */
            SaveDWORDOffset( pDataBuffer, pPayloadInfo->dwObjectSize, dwOffset );
            SaveDWORDOffset( pDataBuffer, pPayloadInfo->dwObjectPres, dwOffset );
        }
    }

    if ( TRUE == fTranscrypting )
    {
        if( pAsfContext->dwAesPayloadExtensionAdditionalBytes > 0 )
        {
            /*
            ** Append the sample ID size to the payload extension data.
            */
            wSaveSize = SIZEOF( DRM_UINT64 );
            SaveWORDOffset( pDataBuffer, wSaveSize, dwOffset );

            /*
            ** Append the sample ID to the payload extension data.
            ** The sample ID is stored in network byte order
            */
            QWORD_TO_NETWORKBYTES( pDataBuffer, dwOffset, pAsfContext->qwNextSampleID );
            dwOffset += SIZEOF( DRM_UINT64 );

            /*
            ** Update the output data buffer offset
            */
            pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset += pAsfContext->dwAesPayloadExtensionAdditionalBytes;
        }
        /*                                 media object size/time      existing sample ID */
        else if( pPayloadInfo->bRepData >= (2 * SIZEOF( DRM_DWORD )) + SIZEOF( DRM_UINT64 ) )
        {
            /* There's already a sample ID in the replicated data, let's read all of the remaining
            ** replicated data and just copy it
            **
            ** We want the rest of the rep data minus the two DWORDs of media object size and presentation time
            ** which were already read and copied
            */
            dwNowWanted = pPayloadInfo->bRepData - (2 * SIZEOF( DRM_DWORD ));
            dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
            if ( dwThisBuffer != dwNowWanted )
            {
                ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
            }
            dwTotalUsed += dwNowWanted;
            dwDataOffset = 0;

            MEMCPY( pDataBuffer + dwOffset, pData + dwDataOffset, dwNowWanted );
            dwOffset += dwNowWanted;
            dwDataOffset += dwNowWanted;
        }
        else
        {
            ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
        }
    }

    pPayloadInfo->wTotalSize = (DRM_WORD)( dwRepDataOffset +
                                      SIZEOF_U8 +
                                      pPayloadInfo->bRepData );

    DRMASSERT( pPayloadInfo->wTotalSize >= dwTotalUsed );

    if ( pPayloadInfo->wTotalSize > dwTotalUsed )
    {
        /* Skip data. */
        dwNowWanted = pPayloadInfo->wTotalSize - dwTotalUsed;
        pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwNowWanted ) );
        dwTotalUsed += dwNowWanted;
    }

    /* We only support one payload per packet */
    DRMASSERT( pPacketInfo->fMultiPayloads == FALSE );

    if( pPacketInfo->dwExplicitPacketLength > 0 )
    {
        dwPayloadSize = pPacketInfo->dwExplicitPacketLength -
                        pPacketInfo->dwParseOffset -
                        pPayloadInfo->wTotalSize -
                        pPacketInfo->dwPadding;
    }
    else
    {
        /*
        dwPayloadSize = pState->dwPacketSize -
                        pPacketInfo->dwParseOffset -
                        pPayloadInfo->wTotalSize -
                        pPacketInfo->dwPadding;
        */

        /* payload size = total packet size - packet header - padding */
        dwPayloadSize = pState->dwPacketSize -
                        pPayloadInfo->wPacketOffset -
                        pPayloadInfo->wTotalSize -
                        pPacketInfo->dwPadding;
    }

    if ( 0 == wTotalDataBytes )
    {
        wTotalDataBytes = (DRM_WORD)dwPayloadSize;
    }

    pPayloadInfo->wPayloadSize = (DRM_WORD)dwPayloadSize;

    pPayloadInfo->wTotalSize += pPacketInfo->bPayBytes +
                                (DRM_WORD)pPayloadInfo->wPayloadSize;

    pPayloadInfo->wTotalDataBytes = wTotalDataBytes;

    pPacketInfo->dwParseOffset += pPayloadInfo->wTotalSize;

    if ( pPacketInfo->dwParseOffset > pState->dwPacketSize ||
         ( pPacketInfo->dwParseOffset == pState->dwPacketSize &&
           pState->dwPayload < pPacketInfo->dwPayloads - 1 ) )
    {
        ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
    }

    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwTotalUsed ) );
    if ( TRUE == fTranscrypting )
    {
        pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset += dwTotalUsed;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
DRM_API DRM_RESULT DRM_CALL _ASFParsePacketHeader(
    __in                           OEM_FILEHDL              hInFile,
    __out_bcount(FILE_BUFFER_SIZE) DRM_BYTE                *pbFileBuffer,
    __inout                        ASF_PARSER_STATE * const pState
)
{
    DRM_RESULT              dr = DRM_SUCCESS;
    DRM_BYTE                b = 0;
    DRM_DWORD               dwNowWanted = 0;
    DRM_DWORD               dwTotalUsed = 0;
    DRM_DWORD               dwOffset = 0;
    ASF_PACKET_PARSER_INFO *pPacketInfo;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParsePacketHeader );

    ChkArg( OEM_INVALID_HANDLE_VALUE != hInFile );
    ChkArg( NULL != pbFileBuffer );
    ChkArg( NULL != pState );

    pPacketInfo = pState->pPacketInfo;

    /* Copy Error Correction data straight to the data buffer */
    dwNowWanted = SIZEOF_U8;
    ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );

    dwOffset = 0;
    LoadBYTEOffset( b, pbFileBuffer, dwOffset );
    dwTotalUsed += dwNowWanted;

    pPacketInfo->fParityPacket = FALSE;
    pPacketInfo->dwParseOffset = 0;

    pPacketInfo->fEccPresent = (DRM_BOOL)( ( b & 0x80 ) == 0x80 );
    pPacketInfo->bECLen = 0;

    if ( pPacketInfo->fEccPresent )
    {
        /* Error correction Data is there */
        /*
        if ( b & 0x10 )
        {
            /* Means Opaque Data, Not Payloads */
            /*
            pPacketInfo->fParityPacket = TRUE;
            return DRM_SUCCESS;
        }
        */

        if ( b & 0x60 )
        {
            /* If error correction data is NOT coded with 4 bits */
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        pPacketInfo->bECLen = ( b & 0x0f );
        if ( pPacketInfo->bECLen != 2 )
        {
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        dwNowWanted = (DRM_DWORD) pPacketInfo->bECLen;
        pPacketInfo->dwParseOffset = (DRM_DWORD)( 1 + pPacketInfo->bECLen );

        ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
        dwTotalUsed += dwNowWanted;

        /* Read first byte of the payload parsing information */
        dwNowWanted = SIZEOF_U8;
        ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
        dwTotalUsed += dwNowWanted;
        dwOffset = 0;
        LoadBYTEOffset( b, pbFileBuffer, dwOffset );
    }

    pPacketInfo->dwPacketLenTypeOffset = pPacketInfo->dwParseOffset;

    /* Padding length type */
    pPacketInfo->bPadLenType = ( b & 0x18 ) >> 3;
    if ( pPacketInfo->bPadLenType == 3 )
    {
       ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
    }

    /* Packet len type */
    pPacketInfo->bPacketLenType = ( b & 0x60 ) >> 5;

    if ( pPacketInfo->bPacketLenType != 0 &&
         pPacketInfo->bPacketLenType != 2 )
    {
        ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
    }

    /* Must be 0 */
    pPacketInfo->bSequenceLenType = ( b & 0x06 ) >> 1;

    pPacketInfo->fMultiPayloads = (DRM_BOOL)( b & 0x01 );

    pPacketInfo->dwParseOffset++;

    /*
     * Read property flags
     */
    dwNowWanted = SIZEOF_U8;
    ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
    dwTotalUsed += dwNowWanted;
    dwOffset = 0;
    LoadBYTEOffset( b, pbFileBuffer, dwOffset );

    /* Offset into media object length type should always be DWORD */
    pPacketInfo->bOffsetLenType = 3;

    /* It must be 5d according to specs */
    if ( b != 0x5d )
    {
        /* Stream Number len type is not 01 (bits 6,7) */
        if ( ( b & 0xc0 ) != 0x40 )
        {
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        if ( ( b & 0x30 ) != 0x10 )
        {
            /* Obj number type len is not 01 */
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }

        /* Bits 2,3 */
        pPacketInfo->bOffsetLenType = ( b & 0x0c ) >> 2;

        /* Should be 11 */
        if ( pPacketInfo->bOffsetLenType == 0 )
        {
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }
        else if ( pPacketInfo->bOffsetLenType < 3 )
        {
            /* not in spec */
            DRMASSERT( !"Offset into Media Object length type not in spec" );
        }

        if ( ( b & 0x03 ) != 0x01 )
        {
            /* Rep data len type is not 01 (bit 0,1) */
            ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
        }
    }

    pPacketInfo->dwParseOffset++;

    /* Packet length type */
    pPacketInfo->dwPacketLenOffset = pPacketInfo->dwParseOffset;

    ChkDR( _ASFParseLengthType(pState,
                               hInFile,
                               pbFileBuffer,
                               pPacketInfo->bPacketLenType,
                              &dwTotalUsed,
                              &pPacketInfo->dwExplicitPacketLength) );

    /* Sequence length type */
    pPacketInfo->dwSequenceOffset = pPacketInfo->dwParseOffset;

    ChkDR( _ASFParseLengthType(pState,
                               hInFile,
                               pbFileBuffer,
                               pPacketInfo->bSequenceLenType,
                              &dwTotalUsed,
                              &pPacketInfo->dwSequenceNum) );

    /* Padding length type */
    pPacketInfo->dwPadLenOffset = pPacketInfo->dwParseOffset;

    ChkDR( _ASFParseLengthType(pState,
                               hInFile,
                               pbFileBuffer,
                               pPacketInfo->bPadLenType,
                              &dwTotalUsed,
                              &pPacketInfo->dwPadding) );

    /* Now read Send Time and Duration (6 bytes) */
    dwNowWanted = SIZEOF( DRM_DWORD ) + SIZEOF( DRM_WORD );
    ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
    dwTotalUsed += dwNowWanted;

    dwOffset = 0;
    LoadDWORDOffset(pPacketInfo->dwSCR, pbFileBuffer, dwOffset );
    LoadWORDOffset(pPacketInfo->wDuration, pbFileBuffer, dwOffset );

    pPacketInfo->dwParseOffset += dwNowWanted ;

    if ( pPacketInfo->fEccPresent && pPacketInfo->fParityPacket )
    {
        ChkDR( DRM_E_ASF_BAD_PACKET_HEADER );
    }

    /* Multiple payloads */
    if ( TRUE == pPacketInfo->fMultiPayloads )
    {
        dwNowWanted = SIZEOF_U8;
        ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
        dwTotalUsed += dwNowWanted;

        dwOffset = 0;
        LoadBYTEOffset( b, pbFileBuffer, dwOffset );

        pPacketInfo->dwParseOffset += dwNowWanted ;

        pPacketInfo->dwPayLenTypeOffset = 0;
        pPacketInfo->bPayLenType = b >> 6;
        pPacketInfo->bPayBytes = 0;
        pPacketInfo->dwPayloads = b & 0x3f;

    }
    else
    {
        pPacketInfo->dwPayLenTypeOffset = 0;
        pPacketInfo->bPayLenType = 0;
        pPacketInfo->bPayBytes = 0;
        pPacketInfo->dwPayloads = 1;
    }

    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( pPacketInfo->dwParseOffset ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
DRM_API DRM_RESULT DRM_CALL _ASFParsePayloadHeader(
    __in                           OEM_FILEHDL              hInFile,
    __out_bcount(FILE_BUFFER_SIZE) DRM_BYTE                *pbFileBuffer,
    __inout                        ASF_PARSER_STATE * const pState
)
{
    DRM_RESULT               dr              = DRM_SUCCESS;
    DRM_DWORD                dwPayloadSize   = 0;
    DRM_BYTE                 b               = 0;
    DRM_DWORD                dwTotalUsed     = 0;
    DRM_DWORD                dwNowWanted     = 0;
    ASF_PACKET_PARSER_INFO  *pPacketInfo     = NULL;
    ASF_PAYLOAD_PARSER_INFO *pPayloadInfo    = NULL;
    DRM_DWORD                dwRepDataOffset = 0;
    DRM_WORD                 wTotalDataBytes = 0;
    DRM_DWORD                dwOffset        = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFParsePayloadHeader );

    ChkArg( OEM_INVALID_HANDLE_VALUE != hInFile );
    ChkArg( NULL != pbFileBuffer );
    ChkArg( NULL != pState );

    pPacketInfo = pState->pPacketInfo;
    pPayloadInfo = pState->pPayloadInfo;

    /* Read Stream ID and Media Object Number */
    dwNowWanted = 2 * SIZEOF_U8 ;
    ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
    dwTotalUsed += dwNowWanted;

    dwOffset = 0;
    LoadBYTEOffset( b, pbFileBuffer, dwOffset );

    pPayloadInfo->wPacketOffset = (DRM_WORD) pPacketInfo->dwParseOffset;
    pPacketInfo->dwParseOffset += dwNowWanted;

    pPayloadInfo->bStreamId = b & 0x7f;
    pPayloadInfo->fIsKeyFrame = ( ( b & 0x80 ) >> 7 );

    LoadBYTEOffset( b, pbFileBuffer, dwOffset );

    /* Only byte is accepted */
    pPayloadInfo->bObjectId = b;

    /*  Find out the Extn Data Size for this stream id */
    ChkDR( _ASFParseLengthType(pState,
                               hInFile,
                               pbFileBuffer,
                               pPacketInfo->bOffsetLenType,
                              &dwTotalUsed,
                              &pPayloadInfo->dwObjectOffset) );

    /* Replicated data offset (from payload's perspective)
     * Replicated data offset = stream number length + media object number length
     * + offset into media object length
    */
    dwRepDataOffset = SIZEOF_U8 + SIZEOF_U8 + SIZEOF( DRM_DWORD );

    /* Read Replicated Data length */
    dwNowWanted = SIZEOF_U8;
    ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
    dwTotalUsed += dwNowWanted;
    pPacketInfo->dwParseOffset += dwNowWanted;
    dwOffset = 0;
    LoadBYTEOffset( b, pbFileBuffer, dwOffset );

    /* RepData Length. Only byte is accepted. */
    pPayloadInfo->bRepData = b;
    pPayloadInfo->dwObjectPres = 0xffffffff;

    /* If replicated data length is set to 1, this is compressed payload data */
    if ( pPayloadInfo->bRepData == 1 )
    {
        pPayloadInfo->fIsCompressedPayload = TRUE;
        pPayloadInfo->dwObjectPres = pPayloadInfo->dwObjectOffset;
        pPayloadInfo->dwObjectOffset = 0;
        pPayloadInfo->dwObjectSize = 0;

        /* Read Presentation Time Delta */
        dwNowWanted = SIZEOF_U8;
        ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
        dwTotalUsed += dwNowWanted;
        pPacketInfo->dwParseOffset += dwNowWanted;
        dwOffset = 0;
        LoadBYTEOffset( b, pbFileBuffer, dwOffset );

        pPayloadInfo->dwDeltaPresTime = b;
        wTotalDataBytes = 0;
    }
    else if( pPayloadInfo->bRepData >= 8 )
    {
        pPayloadInfo->fIsCompressedPayload = FALSE;

        /* Read Media Object Size and Media Object Presentation Time */
        dwNowWanted = 2 * SIZEOF( DRM_DWORD );
        ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
        dwTotalUsed += dwNowWanted;
        pPacketInfo->dwParseOffset += dwNowWanted;
        dwOffset = 0;
        LoadDWORDOffset( pPayloadInfo->dwObjectSize, pbFileBuffer, dwOffset );
        LoadDWORDOffset( pPayloadInfo->dwObjectPres, pbFileBuffer, dwOffset );
        DRM_BYT_CopyBytes( pPayloadInfo->rgbRepData, 0, pbFileBuffer, 0, dwNowWanted );
    }

    pPayloadInfo->wTotalSize = (DRM_WORD)( dwRepDataOffset +
                                      SIZEOF_U8 +
                                      pPayloadInfo->bRepData );

    ChkBOOL( pPayloadInfo->wTotalSize >= dwTotalUsed, DRM_E_ASF_UNEXPECTED );

    /* Copy rest of replicated data. */
    if ( pPayloadInfo->wTotalSize > dwTotalUsed )
    {
        dwNowWanted = pPayloadInfo->wTotalSize - dwTotalUsed;

        ChkDR( _ASFReadData(hInFile, pbFileBuffer, dwNowWanted) );
        dwTotalUsed += dwNowWanted;
        pPacketInfo->dwParseOffset += dwNowWanted;

        ChkOverflow( 2 * SIZEOF( DRM_DWORD ) + dwNowWanted, dwNowWanted );
        ChkBOOL( SIZEOF( pPayloadInfo->rgbRepData ) > 2 * SIZEOF( DRM_DWORD ) + dwNowWanted,
                 DRM_E_BUFFER_BOUNDS_EXCEEDED );
        DRM_BYT_CopyBytes( pPayloadInfo->rgbRepData, 2 * SIZEOF( DRM_DWORD ), pbFileBuffer, 0, dwNowWanted );
    }

    if ( TRUE == pPacketInfo->fMultiPayloads )
    {
        /* Payload Length */
        ChkDR( _ASFParseLengthType(pState,
                                   hInFile,
                                   pbFileBuffer,
                                   pPacketInfo->bPayLenType,
                                  &dwTotalUsed,
                                  &dwPayloadSize) );
    }
    else
    {
        if( pPacketInfo->dwExplicitPacketLength > 0 )
        {
            dwPayloadSize = pPacketInfo->dwExplicitPacketLength -
                pPacketInfo->dwParseOffset -
                pPayloadInfo->wTotalSize -
                pPacketInfo->dwPadding;
        }
        else
        {
            /*
              dwPayloadSize = pState->dwPacketSize -
              pPacketInfo->dwParseOffset -
              pPayloadInfo->wTotalSize -
              pPacketInfo->dwPadding;
            */

            /* payload size = total packet size - packet header - padding */
            dwPayloadSize = pState->dwPacketSize -
                pPayloadInfo->wPacketOffset -
                pPayloadInfo->wTotalSize -
                pPacketInfo->dwPadding;
        }
    }

    if ( 0 == wTotalDataBytes )
    {
        wTotalDataBytes = (DRM_WORD) dwPayloadSize;
    }

    pPayloadInfo->wPayloadSize = (DRM_WORD) dwPayloadSize;

    pPayloadInfo->wTotalSize += pPacketInfo->bPayBytes +
                                (DRM_WORD) pPayloadInfo->wPayloadSize;

    pPayloadInfo->wTotalDataBytes = wTotalDataBytes;

    pPacketInfo->dwParseOffset += dwPayloadSize;

    if ( pPacketInfo->dwParseOffset > pState->dwPacketSize ||
         ( pPacketInfo->dwParseOffset == pState->dwPacketSize &&
           pState->dwPayload < pPacketInfo->dwPayloads - 1 ) )
    {
        ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
    }

    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwTotalUsed ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT _DRMDecrypt(
    __in                             DRM_DECRYPT_CONTEXT * const f_pDecryptContext,
    __inout_bcount_opt(f_cbDataSize) DRM_BYTE            * const f_pbData,
    __in                       const DRM_DWORD                   f_cbDataSize )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__DRMDecrypt );

    DRMASSERT( f_pDecryptContext != NULL );
    ChkArg( f_pbData != NULL || f_cbDataSize < DRM_CPHR_CB_FOR_INITDECRYPT );
    ChkArg( f_cbDataSize > 0 );

    if ( f_cbDataSize < DRM_CPHR_CB_FOR_INITDECRYPT )
    {
        ChkDR( Drm_Reader_InitDecrypt( f_pDecryptContext, f_pbData, f_cbDataSize ) );
    }
    else
    {
        DRM_BYTE rgbLast15[__CB_DECL(DRM_CPHR_CB_FOR_INITDECRYPT)];

        DRM_BYT_CopyBytes( rgbLast15,
                           0,
                           f_pbData,
                           f_cbDataSize - DRM_CPHR_CB_FOR_INITDECRYPT,
                           DRM_CPHR_CB_FOR_INITDECRYPT );

        ChkDR( Drm_Reader_InitDecrypt( f_pDecryptContext, rgbLast15, f_cbDataSize ) );
    }

    ChkDR( Drm_Reader_Decrypt( f_pDecryptContext, NULL, f_pbData, f_cbDataSize ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT _ASFTranscryptPacket(
    __inout NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwNowWanted = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwTotalUsed = 0;
    DRM_BYTE  *pData = NULL;
    DRM_WORD wNextSubPayloadSize = 0;
    DRM_DWORD dwOffset = 0;
    NDT_CRYPTO_DATA_ASF cryptoData;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    ASF_PARSER_STATE *pState = &(pAsfContext->ParserState);


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptPacket );

    DRMASSERT( TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode );
    if ( DRM_I64Les(pState->iLastPacketOffset, pState->iCurrentPacketOffset) )
    {
        pState->tParseState = P_END;
        goto ErrorExit;
    }

    /* Adjust packet offset information */
    pState->iPreviousPacketOffset = pState->iCurrentPacketOffset;
    pState->iCurrentPacketOffset = pState->iNextPacketOffset;
    pState->iNextPacketOffset = DRM_I64Add( pState->iNextPacketOffset, DRM_I64( pState->dwPacketSize ) );

    DRMASSERT( ! DRM_I64Les( pState->iCurrentPacketOffset, pState->iCurrentFileOffset ) );

    /* Transcrypt PACKET header */
    ChkDR( _ASFProcessPacketHeader( pAsfContext ) );

    if ( pState->pPacketInfo->fEccPresent &&
         pState->pPacketInfo->fParityPacket )
    {
        /* Go to next packet */
        pState->tParseState = P_NEW_PACKET;
        goto ErrorExit;
    }

    pState->tParseState = P_PAYLOAD_HEADER;
    pState->dwPayload = 0;

    /* Transcrypt PAYLOAD header */
    ChkDR( _ASFProcessPayloadHeader( pAsfContext ) );

    pState->wPayStart = pState->pPayloadInfo->wPacketOffset +
                        pState->pPayloadInfo->wTotalSize -
                        pState->pPayloadInfo->wPayloadSize;

    /* pState->iCurrentPacketOffset + pState->wPayStart >= pState->iCurrentFileOffset */
    DRMASSERT(
        ! DRM_I64Les(
            DRM_I64Add( pState->iCurrentPacketOffset, DRM_I64( pState->wPayStart ) ),
            pState->iCurrentFileOffset
        )
    );

    /* if pState->iCurrentPacketOffset + pState->wPayStart > pState->iCurrentFileOffset */
    if ( DRM_I64Les( pState->iCurrentFileOffset, DRM_I64Add( pState->iCurrentPacketOffset, DRM_I64( pState->wPayStart ) ) ) )
    {
        /* dwSkipBytes = pState->iCurrentPacketOffset + pState->wPayStart - pState->iCurrentFileOffset */
        DRM_DWORD dwSkipBytes = DRM_I64ToUI32(
            DRM_I64Sub(
                DRM_I64Add( pState->iCurrentPacketOffset, DRM_I64(pState->wPayStart ) ),
                pState->iCurrentFileOffset
            )
        );

        pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwSkipBytes ) );
    }

    pState->tParseState = P_PAYLOAD;

    if ( pState->fEncrypted != TRUE )
    {
        /* This stream is not encrypted */
        pState->tParseState = P_PAYLOAD_END;
        goto ErrorExit;
    }

    if ( pState->pPayloadInfo->wPayloadSize <= 0 )
    {
        pState->tParseState = P_PAYLOAD_END;
        goto ErrorExit;
    }

    pState->dwPayloadOffset = DRM_UI64Low32( DRM_UI64Add( DRM_I2UI64( pState->iCurrentPacketOffset ), DRM_UI64( pState->wPayStart ) ) );

    if( pState->pPayloadInfo->bRepData != 1 )
    {
        pState->tParseState = P_UNCOMPRESSEDPAYLOAD;
        pState->pPayloadInfo->fIsCompressedPayload = FALSE;
    }
    else
    {
        pState->tParseState = P_COMPRESSEDPAYLOAD;
        pState->pPayloadInfo->fIsCompressedPayload = TRUE;
    }

    /* For audio only files, we only support one sample per payload.
     * That means the size of the payload has to be the same as the
     * size of the object inside the payload
     */
    DRMASSERT( pState->pPayloadInfo->wPayloadSize == pState->pPayloadInfo->dwObjectSize );

    if ( pState->tParseState == P_UNCOMPRESSEDPAYLOAD )
    {
        dwNowWanted = pState->pPayloadInfo->wPayloadSize;
        dwThisBuffer = WMCBGetData( pAsfContext, DRM_I2UI64(pState->iCurrentFileOffset), dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
        }

        /* Give this buffer to DRM and decrypt it there */
        if ( ( !pAsfContext->fEncryptClear ) &&
             ( pAsfContext->eCipherType == eDRM_RC4_CIPHER ) )
        {
            ChkDR( _DRMDecrypt( &pAsfContext->decryptContext, pData, dwNowWanted ) );
        }

        pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwNowWanted ) );
    }
    else
    {
        DRM_WORD wBytesRead = 0;
        DRM_BYTE  b = 0;

        DRMASSERT( !"Compressed payload not supported for now" );

        dwTotalUsed = 0;

        dwNowWanted = SIZEOF_U8 ;
        dwThisBuffer = WMCBGetData( pAsfContext, DRM_I2UI64(pState->iCurrentFileOffset), dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
        }

        dwTotalUsed += dwNowWanted;
        dwOffset = 0;

        LoadBYTEOffset( b, pData, dwOffset );
        wNextSubPayloadSize = (DRM_WORD)b;

        if ( ( pState->pPayloadInfo->wTotalDataBytes < wNextSubPayloadSize ) ||
             ( wNextSubPayloadSize == 0 ) )
        {
            pState->tParseState = P_PAYLOAD_END;
            /*break;*/
            goto ErrorExit;
        }

        if ( pState->pPayloadInfo->wTotalDataBytes > wNextSubPayloadSize )
        {
            wBytesRead = wNextSubPayloadSize + 1;
        }
        else if ( pState->pPayloadInfo->wTotalDataBytes == wNextSubPayloadSize )
        {
            wBytesRead = wNextSubPayloadSize;
        }

        while( pState->pPayloadInfo->wTotalDataBytes >= wBytesRead )
        {
            /* pState->iCurrentPacketOffset + pState->wPayStart + dwTotalUsed >= pState->iCurrentFileOffset */
            DRMASSERT(
                ! DRM_I64Les(
                    DRM_I64Add(
                        pState->iCurrentPacketOffset,
                        DRM_I64( pState->wPayStart + dwTotalUsed )
                    ),
                    pState->iCurrentFileOffset
                )
            );

            /* if pState->iCurrentPacketOffset + pState->wPayStart + dwTotalUsed > pState->iCurrentFileOffset */
            if ( DRM_I64Les( pState->iCurrentFileOffset, DRM_I64Add( pState->iCurrentPacketOffset, DRM_I64( pState->wPayStart + dwTotalUsed ) ) ) )
            {
                /* dwSkipBytes = pState->iCurrentPacketOffset + pState->wPayStart + dwTotalUsed - pState->iCurrentFileOffset */
                DRM_DWORD dwSkipBytes = DRM_I64ToUI32(
                    DRM_I64Sub(
                        DRM_I64Add(
                            pState->iCurrentPacketOffset,
                            DRM_I64( pState->wPayStart + dwTotalUsed )
                        ),
                        pState->iCurrentFileOffset
                    )
                );

                pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwSkipBytes ) );
            }

            if ( wNextSubPayloadSize > 0 )
            {
                /*DRMASSERT(wNextSubPayloadSize >=15);*/
                dwNowWanted = wNextSubPayloadSize;
                dwThisBuffer = WMCBGetData( pAsfContext, DRM_I2UI64(pState->iCurrentFileOffset), dwNowWanted, &pData );
                if ( dwThisBuffer != dwNowWanted )
                {
                    ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
                }

                /* Give this buffer to DRM and decrypt it there */
                if ( ( !pAsfContext->fEncryptClear ) &&
                     ( pAsfContext->eCipherType == eDRM_RC4_CIPHER ) )
                {
                    ChkDR( _DRMDecrypt( &pAsfContext->decryptContext, pData, dwNowWanted ) );
                }

                pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwNowWanted ) );

                pState->tParseState = P_PAYLOAD_END;
                dwTotalUsed += (DRM_DWORD)wNextSubPayloadSize;
            }

            if ( pState->pPayloadInfo->wTotalDataBytes > wBytesRead )
            {
                dwNowWanted = SIZEOF_U8 ;
                dwThisBuffer = WMCBGetData( pAsfContext, DRM_I2UI64(pState->iCurrentFileOffset), dwNowWanted, &pData );
                if ( dwThisBuffer != dwNowWanted )
                {
                    ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
                }

                dwTotalUsed += dwNowWanted;
                dwOffset = 0;
                LoadBYTEOffset( b, pData, dwOffset );

                wNextSubPayloadSize = (DRM_WORD)b;
                if ( ( pState->pPayloadInfo->wTotalDataBytes < wNextSubPayloadSize ) ||
                        ( wNextSubPayloadSize == 0 ) )
                {
                    pState->tParseState = P_PAYLOAD_END;
                    /*break;*/
                    goto ErrorExit;
                }

                wBytesRead += wNextSubPayloadSize+1;
            }
            else
            {
                /*break;*/
                goto ErrorExit;
            }
        } /* end of while */

        pState->tParseState = P_PAYLOAD_END;
    }

    /* Re-encrypt using AES */
    cryptoData.SampleData.pbBlob = pData;
    cryptoData.SampleData.cbBlob = dwNowWanted;
    cryptoData.qwSampleID = pAsfContext->qwNextSampleID;
    cryptoData.qwOffset = DRM_UI64(0);

    pAsfContext->qwNextSampleID = DRM_UI64Add( pAsfContext->qwNextSampleID, DRM_UI64( 1 ) );

    if ( pAsfContext->eCipherType != eDRM_AES_COUNTER_CIPHER )
    {
        ChkDR( NDTMGR_Encrypt( (NDTMGR_CONTEXT*)pAsfTranscryptContext->pNDTManagerContext,
                               pAsfTranscryptContext->idSession,
                               NDT_ENCRYPTION_MODE_ASF,
                               &cryptoData ) );
    }

    ChkDR( _AppendToDataBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );

    /* Copy original padding to data buffer */
    dwNowWanted = pState->pPacketInfo->dwPadding;
    if ( dwNowWanted > 0 )
    {
        dwThisBuffer = WMCBGetData( pAsfContext, DRM_I2UI64(pState->iCurrentFileOffset), dwNowWanted, &pData );
        if ( dwThisBuffer != dwNowWanted )
        {
            ChkDR( DRM_E_ASF_BAD_PAYLOAD_HEADER );
        }

        ChkDR( _AppendToDataBuffer( pAsfTranscryptContext, pData, dwNowWanted ) );

        pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwNowWanted ) );
    }

    pState->dwPayload++;
    if ( pState->dwPayload >= pState->pPacketInfo->dwPayloads )
    {
        /* We're done with the payload(s) in this packet */
        pState->tParseState = P_NEW_PACKET;
    }
    else
    {
        /* There are still payloads to be processed in this packet */
        DRMASSERT( !"We don't support multiple payloads per packet in this implementation." );
        pState->tParseState = P_PAYLOAD_HEADER;
    }


ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        /* Go to next payload */
        pState->tParseState = P_PAYLOAD_END;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT _ASFTranscryptData(
    __inout NDT_ASF_CONTEXT * const pAsfContext,
    __out   DRM_BOOL        * const pfEndOfPackets
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwBufferOffset = 0;
    ASF_PARSER_STATE *pState = &(pAsfContext->ParserState);
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    ASF_DATA_CONTEXT *pData = &(pAsfTranscryptContext->Data.DataContext);


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptData );

    DRMASSERT( TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode );

    *pfEndOfPackets = FALSE;

    /* Continue transcrypting packets until we run out of buffer space or
     * we consume all packets
    */
    dwBufferOffset = pData->dwDataBufferOffset +
                     pState->dwPacketSize +
                     pAsfContext->dwAesPayloadExtensionAdditionalBytes;

    while( dwBufferOffset < pData->cbDataBufferSize &&
           DRM_UI64Les( DRM_UI64(pState->dwPacketsProcessed ), DRM_UI64(pState->dwTotalPackets ) ) )
    {
        dr = _ASFTranscryptPacket( pAsfContext );
        if ( dr ==  DRM_E_ASF_BAD_PACKET_HEADER || dr == DRM_E_ASF_BAD_PAYLOAD_HEADER )
        {
            /* salvage what can be salvaged */
            pState->dwTotalPackets    = pState->dwPacketsProcessed;
            pState->iLastPacketOffset = pState->iPreviousPacketOffset;

            TRACE(("An error occurred while parsing the ASF header.  Attempting to salvage what can be salvaged\n"));

            dr = DRM_SUCCESS;
            break;
        }
        else
        {
            ChkDR(dr);
        }

        pState->dwPacketsProcessed++;
        dwBufferOffset = pData->dwDataBufferOffset +
                         pState->dwPacketSize +
                         pAsfContext->dwAesPayloadExtensionAdditionalBytes;
    }

    if ( DRM_UI64Eql(DRM_UI64(pState->dwPacketsProcessed ), DRM_UI64(pState->dwTotalPackets ) ) )
    {
        *pfEndOfPackets = TRUE;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT _ASFTranscryptDataObjectHeader(
    __inout NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PARSER_STATE *pState;
    DRM_DWORD dwNowWanted = 0;
    DRM_DWORD dwThisBuffer = 0;
    DRM_DWORD dwTotalUsed = 0;
    DRM_BYTE  *pData = NULL;
    DRM_BYTE  *pDataObj = NULL;
    DRM_UINT64 qwDataObjSize = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD dwDataObjOffset = 0;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptDataObjectHeader );

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    DRMASSERT( TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode );

    /* Allocate space for a few packets + some space for the Data Object header */
    ChkOverflow( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes, pAsfContext->dwAesPayloadExtensionAdditionalBytes );
    ChkOverflow( ( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) * ASF_PACKET_TRANSCRYPTS_PER_CALL, ( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) );
    ChkOverflow( (( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) * ASF_PACKET_TRANSCRYPTS_PER_CALL ) + DATA_OBJECT_SIZE, ( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) * ASF_PACKET_TRANSCRYPTS_PER_CALL );
    pAsfTranscryptContext->Data.DataContext.cbDataBufferSize =
        ( ( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) * ASF_PACKET_TRANSCRYPTS_PER_CALL ) +
        DATA_OBJECT_SIZE;

    /* Read Data Object header */
    dwNowWanted = DATA_OBJECT_SIZE;
    dwThisBuffer = WMCBGetData( pAsfContext, DRM_UI64Add(DRM_I2UI64(pState->iCurrentFileOffset), DRM_UI64(dwTotalUsed)), dwNowWanted, &pData );
    if ( dwThisBuffer != dwNowWanted )
    {
        ChkDR( DRM_E_ASF_BAD_DATA_HEADER );
    }

    SAFE_OEM_FREE( pAsfTranscryptContext->Data.DataContext.pbDataBuffer );
    ChkMem( pAsfTranscryptContext->Data.DataContext.pbDataBuffer = (DRM_BYTE*)Oem_MemAlloc( pAsfTranscryptContext->Data.DataContext.cbDataBufferSize ) );
    ZEROMEM( pAsfTranscryptContext->Data.DataContext.pbDataBuffer, pAsfTranscryptContext->Data.DataContext.cbDataBufferSize );
    pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset = 0;

    dwTotalUsed += dwNowWanted;

    MEMCPY( pAsfTranscryptContext->Data.DataContext.pbDataBuffer, pData, dwNowWanted );

    pDataObj = pAsfTranscryptContext->Data.DataContext.pbDataBuffer;
    dwDataObjOffset = SIZEOF( DRM_GUID );

    /* Adjust data object size */
    DRMASSERT( 1 <= pAsfContext->dwNoOfStreams );
    if ( 1 == pAsfContext->dwNoOfStreams && TRUE == pAsfContext->fAudioOnly )
    {
        /* The data object size is easily predictable if there is a single audio stream */
        qwDataObjSize = DRM_UI64Add(
            DRM_UI64( DATA_OBJECT_SIZE ),
            DRM_UI64Mul(
                DRM_UI64( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ),
                DRM_UI64(pState->dwTotalPackets)
                )
            );
    }
    else
    {
        /* If there's multiple streams or a video stream, the packet size cannot be easily determined */
        qwDataObjSize = DRM_UI64( DATA_OBJECT_SIZE );
    }
    SaveQWORDOffset( pDataObj, qwDataObjSize, dwDataObjOffset );

    /* Insert regenerated File ID */
    SaveGUIDOffset( pDataObj, pAsfTranscryptContext->guidFileID, dwDataObjOffset );

    /* Insert data packet count if necessary */
    if ( !(1 == pAsfContext->dwNoOfStreams && TRUE == pAsfContext->fAudioOnly) )
    {
        /* If there's multiple streams or a video stream, the number of packets is unpredictable */
        DRM_UINT64 qwTmp = DRM_UI64( 0 );
        SaveQWORDOffset( pDataObj, qwTmp, dwDataObjOffset );
    }

    pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset += dwNowWanted;
    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwNowWanted ) );

    /* We should be done with the Header object and with the Data Object header */
    DRMASSERT( DRM_I64Eql( pState->iCurrentFileOffset, pState->iFirstPacketOffset ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_VOID _ClearContentEncObj(
    __out ASF_CONTENT_ENCRYPTION_OBJECT *pContentEncObj
)
{
    if ( pContentEncObj )
    {
        SAFE_OEM_FREE( pContentEncObj->pbSecretData );
        SAFE_OEM_FREE( pContentEncObj->pbProtectionType );
        SAFE_OEM_FREE( pContentEncObj->pbKeyID );
        SAFE_OEM_FREE( pContentEncObj->pbLicenseURL );
        SAFE_OEM_FREE( pContentEncObj );
    }
}



/****************************************************************************/
static DRM_VOID _ClearContentEncObjExt(
    __out ASF_CONTENT_ENCRYPTION_OBJECT_EXT *pContentEncObjExt
)
{
    if ( pContentEncObjExt )
    {
        SAFE_OEM_FREE( pContentEncObjExt->pbData );
        SAFE_OEM_FREE( pContentEncObjExt );
    }
}



/****************************************************************************/
static DRM_VOID _ParseClose(
    __out ASF_PARSER_STATE * const f_pState
)
{
    if ( f_pState )
    {
        SAFE_OEM_FREE( f_pState->pPacketInfo );
        SAFE_OEM_FREE( f_pState->pPayloadInfo );
        ZEROMEM( f_pState, SIZEOF( ASF_PARSER_STATE ) );
    }
}



/****************************************************************************/
static DRM_RESULT _FillOutputBuffer(
    __inout    ASF_TRANSCRYPT_CONTEXT * const f_pAsfTranscryptContext,
    __out_bcount_part(f_cbBuffer,(*f_pdwBytesWritten) + f_dwOffset)
               DRM_BYTE                      *f_pbBuffer,
    __in const DRM_DWORD                      f_dwOffset,
    __in const DRM_DWORD                      f_cbBuffer,
    __out      DRM_DWORD              * const f_pdwBytesWritten
)
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_DWORD         dwDataToSend = 0;
    ASF_DATA_CONTEXT *pData        = NULL;

    DRMASSERT( f_pAsfTranscryptContext != NULL );
    DRMASSERT( f_pbBuffer              != NULL );
    DRMASSERT( f_pdwBytesWritten       != NULL );
    if (f_dwOffset >= f_cbBuffer)
    {
        /*
         * Function is being incorrectly called.
         */
        *f_pdwBytesWritten = 0;
        ChkDR( DRM_E_INVALIDARG );
    }

    pData = &(f_pAsfTranscryptContext->Data.DataContext);

    dwDataToSend = pData->dwDataBufferOffset - pData->dwSentDataOffset;
    if ( dwDataToSend > f_cbBuffer - f_dwOffset )
    {
        /* The remaining data is still too big. Send whatever we can. */
        *f_pdwBytesWritten = f_cbBuffer - f_dwOffset;
        DRM_BYT_CopyBytes(f_pbBuffer, f_dwOffset, pData->pbDataBuffer, pData->dwSentDataOffset, *f_pdwBytesWritten);
        pData->dwSentDataOffset += *f_pdwBytesWritten;
    }
    else
    {
        /* The remaining data fits in the output buffer. */
        *f_pdwBytesWritten = dwDataToSend;
        DRM_BYT_CopyBytes( f_pbBuffer, f_dwOffset, pData->pbDataBuffer, pData->dwSentDataOffset, *f_pdwBytesWritten );
        pData->dwSentDataOffset = 0;

        /* Signal that we can now re-use our internal buffer as we have
         * sent everything in it already.
         */
        pData->dwDataBufferOffset = 0;
    }

ErrorExit:
    return dr;
}



#if DRM_SUPPORT_AUDIO_ONLY == 0

/****************************************************************************/
static DRM_RESULT _FillOutputBufferWithMuxPackets(
    __inout       ASF_TRANSCRYPT_CONTEXT * const f_pAsfTranscryptContext,
    __out_bcount_part(f_cbBuffer,(*f_pdwBytesWritten) + f_dwOffset)
                  DRM_BYTE                      *f_pbBuffer,
    __in          DRM_DWORD                      f_dwOffset,
    __in    const DRM_DWORD                      f_cbBuffer,
    __out         DRM_DWORD              * const f_pdwBytesWritten
)
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_DWORD         cbRemaining  = 0;
    ASF_MEDIA_BUFFER *pMediaBuffer = NULL;
    ASF_SAMPLE       *pTmpSample   = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__FillOutputBufferWithMuxPackets );

    DRMASSERT( f_pAsfTranscryptContext != NULL );
    DRMASSERT( f_pbBuffer              != NULL );
    DRMASSERT( f_pdwBytesWritten       != NULL );

    if (f_dwOffset >= f_cbBuffer)
    {
        /*
         * Function is being incorrectly called.
         */
        *f_pdwBytesWritten = 0;
        ChkDR( DRM_E_INVALIDARG );
    }

    cbRemaining = f_cbBuffer - f_dwOffset;
    *f_pdwBytesWritten = 0;
    while ( cbRemaining > 0 )
    {
        DRM_DWORD cMediaBuffers = 0;

        if ( NULL == f_pAsfTranscryptContext->pOutputPacket )
        {
            ChkDR( ASFMux_GetNextPacket( (ASF_MULTIPLEXER_CONTEXT*)f_pAsfTranscryptContext->pMux, &pTmpSample ) );

            if ( NULL == pTmpSample )
            {
                /* There are no remaining packets in the mux */
                goto ErrorExit;
            }
            else
            {
                f_pAsfTranscryptContext->pOutputPacket = pTmpSample;
            }
        }

        ChkDR( ASFSample_GetBufferCount( f_pAsfTranscryptContext->pOutputPacket, &cMediaBuffers ) );
        DRMASSERT( cMediaBuffers > 0 );
        do
        {
            /*
             *  pMediaBuffer will points to f_pAsfTranscryptContext->pOutputPacket->MediaBuffers[ 0 ].data
             */
            ChkDR( ASFSample_GetBufferByIndex( f_pAsfTranscryptContext->pOutputPacket, 0, &pMediaBuffer ) );
            ChkMem( pMediaBuffer );
            /*
             *  Make sure that ASF_MEDIA_BUFFER has correct reference counter and release of memoty will not happen twice
             */
            ChkBOOL( pMediaBuffer->cRef > 1, DRM_E_ASF_UNEXPECTED );

            if ( pMediaBuffer->cbCurrentLength > cbRemaining )
            {
                ChkBOOL( f_dwOffset         <= f_cbBuffer - cbRemaining, DRM_E_BUFFERTOOSMALL );
                ChkBOOL( *f_pdwBytesWritten <= f_cbBuffer - cbRemaining, DRM_E_BUFFERTOOSMALL );

                /*
                 *  This media buffer has enough remaining data to fill the output buffer
                 */
                DRM_BYT_CopyBytes( f_pbBuffer,
                                   f_dwOffset,
                                   pMediaBuffer->blobBuffer.pbBlob,
                                   pMediaBuffer->cbCurrentOffset,
                                   cbRemaining );

                f_dwOffset += cbRemaining;
                *f_pdwBytesWritten += cbRemaining;

                /*
                 *  There is still data in this buffer that needs to be processed.
                 *  Don't remove the buffer from the sample just yet.
                 */

                pMediaBuffer->cbCurrentOffset += cbRemaining;
                pMediaBuffer->cbCurrentLength -= cbRemaining;

                cbRemaining = 0;

                /*
                 *  Done with data processing, lets release the pointer
                 */
                ASFMediaBuffer_Release( pMediaBuffer );
                pMediaBuffer = NULL;
            }
            else
            {
                ChkBOOL( f_dwOffset         <= f_cbBuffer - pMediaBuffer->cbCurrentLength, DRM_E_BUFFERTOOSMALL );
                ChkBOOL( *f_pdwBytesWritten <= f_cbBuffer - pMediaBuffer->cbCurrentLength, DRM_E_BUFFERTOOSMALL );

                /*
                 *  The media buffer fits in the output buffer
                 */
                DRM_BYT_CopyBytes( f_pbBuffer,
                                   f_dwOffset,
                                   pMediaBuffer->blobBuffer.pbBlob,
                                   pMediaBuffer->cbCurrentOffset,
                                   pMediaBuffer->cbCurrentLength );

                f_dwOffset += pMediaBuffer->cbCurrentLength;
                *f_pdwBytesWritten += pMediaBuffer->cbCurrentLength;
                cbRemaining -= pMediaBuffer->cbCurrentLength;

                /*
                 *  Done with data processing, lets release the temp pointer
                 */
                ASFMediaBuffer_Release( pMediaBuffer );
                pMediaBuffer = NULL;

                /*
                 *  Temp pointer is released, now remove the node.
                 */
                ASFSample_RemoveBufferByIndex( f_pAsfTranscryptContext->pOutputPacket, 0 );
            }

            ChkDR( ASFSample_GetBufferCount( f_pAsfTranscryptContext->pOutputPacket, &cMediaBuffers ) );
        } while ( cMediaBuffers > 0 && cbRemaining != 0 && *f_pdwBytesWritten < f_cbBuffer );

        if ( 0 == cMediaBuffers )
        {
            /* Processed all the media buffers in this packet.
             * Time to release it.
             */
            ASFSample_Release( f_pAsfTranscryptContext->pOutputPacket );
            f_pAsfTranscryptContext->pOutputPacket = NULL;
        }

    }

 ErrorExit:
    if ( NULL != pMediaBuffer )
    {
        ASFMediaBuffer_Release( pMediaBuffer );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

#endif /* DRM_SUPPORT_AUDIO_ONLY == 0 */



/****************************************************************************/
static DRM_RESULT _FillOutputBufferFromHeaderBuffer(
    __inout    ASF_TRANSCRYPT_CONTEXT * const f_pAsfTranscryptContext,
    __out_bcount_part( f_cbBuffer,(*f_pdwBytesWritten) + f_dwOffset)
               DRM_BYTE                      *f_pbBuffer,
    __in const DRM_DWORD                      f_dwOffset,
    __in const DRM_DWORD                      f_cbBuffer,
    __out      DRM_DWORD              * const f_pdwBytesWritten
)
{
    DRM_RESULT dr                = DRM_SUCCESS;
    DRM_DWORD  cbDataToSend      = 0;
    DRM_DWORD  cbBufferRemaining = 0;
    ASF_TRANSCRYPT_HEADER_CONTEXT *pHeader = NULL;

    DRMASSERT( f_pAsfTranscryptContext != NULL );
    DRMASSERT( f_pbBuffer              != NULL );
    DRMASSERT( f_pdwBytesWritten       != NULL );

    pHeader = &(f_pAsfTranscryptContext->Header);

    if (f_dwOffset >= f_cbBuffer)
    {
        /*
         * Function is being incorrectly called. Throw the current packet on
         * the floor and continue.
         */
        *f_pdwBytesWritten = 0;
        pHeader->dwSentDataOffset = 0;
        pHeader->dwHeaderOffset = 0;
        ChkDR( DRM_E_INVALIDARG );
    }

    cbBufferRemaining = f_cbBuffer - f_dwOffset;
    cbDataToSend = pHeader->dwHeaderOffset - pHeader->dwSentDataOffset;

    if ( cbDataToSend > cbBufferRemaining )
    {
        /* The remaining data is still too big. Send whatever we can. */
        *f_pdwBytesWritten = cbBufferRemaining;
        DRM_BYT_CopyBytes( f_pbBuffer, f_dwOffset, pHeader->pbHeader, pHeader->dwSentDataOffset, cbBufferRemaining );
        pHeader->dwSentDataOffset += cbBufferRemaining;
    }
    else
    {
        /* The remaining data fits in the output buffer. */
        *f_pdwBytesWritten = cbDataToSend;
        DRM_BYT_CopyBytes( f_pbBuffer, f_dwOffset, pHeader->pbHeader, pHeader->dwSentDataOffset, cbDataToSend );
        pHeader->dwSentDataOffset = 0;

        /* Signal that we can now re-use our internal buffer as we have
         * sent everything in it already.
         */
        pHeader->dwHeaderOffset = 0;
    }

ErrorExit:
    return dr;
}



static DRM_RESULT _ASFSetHeaderDone(
    __inout NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    ASF_PARSER_STATE *pState = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFSetHeaderDone );

    pState = &(pAsfContext->ParserState);
    pState->fHeaderDone = TRUE;
    if ( !pState->fSeekPositionSet )
    {
        DRMASSERT( DRM_I64Eql(pState->iCurrentFileOffset, DRM_I64Sub(pState->iFirstPacketOffset, DRM_I64(DATA_OBJECT_SIZE) ) ) );
        pState->iCurrentPacketOffset = pState->iFirstPacketOffset;
        pState->iNextPacketOffset = pState->iFirstPacketOffset;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFTranscryptHeader(
    __in_opt                 DRM_VOID        *f_pOEMContext,
    __inout                  NDT_ASF_CONTEXT *pAsfContext,
    __out_bcount(*pcbBuffer) DRM_BYTE        *pbBuffer,
    __inout                  DRM_DWORD       *pcbBuffer
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    ASF_PARSER_STATE *pState = NULL;
    ASF_TRANSCRYPT_HEADER_CONTEXT *pHeader = NULL;
    DRM_DWORD   cbBufferSize = 0;
    DRM_DWORD   dwBytesWritten = 0;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptHeader );

    /* Save buffer size and initialize out parameters */
    cbBufferSize = *pcbBuffer;
    *pcbBuffer = 0;

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    DRMASSERT( TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode );
    pHeader = &(pAsfTranscryptContext->Header);

    if ( pState->fHeaderDone )
    {
        goto ErrorExit;
    }

    /* If there is data yet to be delivered in the data buffer, copy it to
     * the output buffer first.
     */
    if ( pHeader->dwSentDataOffset != 0 )
    {
        DRMASSERT( pHeader->dwHeaderOffset >= pHeader->dwSentDataOffset );

        ChkDR( _FillOutputBufferFromHeaderBuffer( pAsfTranscryptContext, pbBuffer, 0, cbBufferSize, &dwBytesWritten ) );

        *pcbBuffer = dwBytesWritten;

        if ( 0 == pHeader->dwHeaderOffset )
        {
            /* If the output buffer is not full, we just moved the last bytes
             * of the transcrypted header to the output buffer and this
             * function should not be called anymore for this file.
             */
            _ASFSetHeaderDone(pAsfContext);
        }
    }
    else
    {
        ChkDR( _ASFProcessHeaders( f_pOEMContext, pAsfContext ) );

        ChkDR( _FillOutputBufferFromHeaderBuffer( pAsfTranscryptContext, pbBuffer, 0, cbBufferSize, &dwBytesWritten ) );

        *pcbBuffer = dwBytesWritten;

        /* If the transcrypted header is enough to fill in the
        * output buffer, we're done for this call.
        */
        if ( pHeader->dwSentDataOffset == 0 )
        {
            ChkDR( _ASFSetHeaderDone( pAsfContext ) );
        }
    }

    /* Prepare for packet transcryption if we're done with the header. */
    if ( pState->fHeaderDone )
    {
        pState->tParseState = P_NEW_PACKET;

#if DRM_SUPPORT_AUDIO_ONLY == 0

        pAsfTranscryptContext->pMux = Oem_MemAlloc( SIZEOF( ASF_MULTIPLEXER_CONTEXT ) );
        ChkMem( pAsfTranscryptContext->pMux );
        ChkDR( ASFMux_Initialize( (ASF_MULTIPLEXER_CONTEXT *) pAsfTranscryptContext->pMux,
                                  &pAsfContext->StreamInfoList,
                                  pAsfContext->dwOutputPacketSize,
                                  pAsfContext->qwPreroll,
                                  pAsfContext->ParserState.dwMaxBitrate ) );

#endif /* DRM_SUPPORT_AUDIO_ONLY == 0 */

    }

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
#if DRM_SUPPORT_AUDIO_ONLY == 0
        ASFMux_Uninitialize( (ASF_MULTIPLEXER_CONTEXT *)pAsfTranscryptContext->pMux );
#endif /* DRM_SUPPORT_AUDIO_ONLY == 0 */

    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFSeekToTimePosition(
    __inout NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    ASF_PARSER_STATE *pState = NULL;
    DRM_BOOL    fFound = FALSE;
    DRM_DWORD   dwCurrentPos;
    DRM_UINT64  qwSeekPacket = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64  qwSeekPos = DRM_UI64LITERAL( 0, 0 );
    DRM_BOOL    fTranscrypting;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFSeekToTimePosition );

    ChkArg( pAsfContext );
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    fTranscrypting = TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode;

    if ( TRUE == fTranscrypting )
    {
        ChkArg( pAsfTranscryptContext );

        /* Check transcryption engine state */
        if ( pAsfTranscryptContext->State != ASF_TRANSCRYPTION_STATE_INITIALIZED )
        {
            ChkDR( DRM_E_ND_FAILED_SEEK );
        }
    }

    pState = &(pAsfContext->ParserState);

    /* Check that the time position has actually been set */
    if ( !pState->fSeekPositionSet )
    {
        ChkDR( DRM_E_ND_FAILED_SEEK );
    }

    /* Walk backwards through the packets until we find one with
     * a presentation time before our seek time.
     */
    qwSeekPos = pState->qwSeekPosition;
    qwSeekPacket = pState->qwSeekPacket;
    while( ! DRM_UI64Les( qwSeekPos, DRM_I2UI64( pState->iFirstPacketOffset ) ) )
    {
        /* Set position of the next packet to be transcrypted */
        pState->iCurrentFileOffset = DRM_UI2I64(qwSeekPos);
        pState->iNextPacketOffset = DRM_UI2I64(qwSeekPos);
        pState->iCurrentPacketOffset = DRM_UI2I64(qwSeekPos);
        if ( TRUE == fTranscrypting )
        {
            pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset = 0;
        }
        ChkDR( _ASFProcessPacketHeader( pAsfContext ) );

        if ( ! pState->pPacketInfo->fParityPacket )
        {
            pState->tParseState = P_PAYLOAD_HEADER;
            pState->dwPayload = 0;
            ChkDR( _ASFProcessPayloadHeader( pAsfContext ) );

            if ( pState->pPayloadInfo->dwObjectPres <= pState->dwTimePosition )
            {
                fFound = TRUE;
                break;
            }
        }
        if( DRM_UI64Les( qwSeekPos, DRM_UI64( pState->dwPacketSize ) ) )
        {
            fFound = TRUE;
            qwSeekPos = DRM_I2UI64( pState->iFirstPacketOffset );
            qwSeekPacket = DRM_UI64( 0 );
            break;
        }
        else
        {
            qwSeekPos = DRM_UI64Sub( qwSeekPos, DRM_UI64( pState->dwPacketSize ) );
        }
        qwSeekPacket = DRM_UI64Sub( qwSeekPacket, DRM_UI64( 1 ) );
    }

    if ( ! fFound )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    if ( TRUE == fTranscrypting )
    {
        /* Flush all buffers and reset state */
        pAsfTranscryptContext->Header.dwHeaderOffset   = 0;
        pAsfTranscryptContext->Header.dwSentDataOffset = 0;
        pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset = 0;
        pAsfTranscryptContext->Data.DataContext.dwSentDataOffset   = 0;
        pAsfTranscryptContext->fAllPacketsTranscrypted = FALSE;

        pState->tParseState = P_NEW_PACKET;
    }

    /* Set position of the next packet to be transcrypted */
    pState->iCurrentFileOffset = DRM_UI2I64(qwSeekPos);
    pState->iNextPacketOffset = DRM_UI2I64(qwSeekPos);
    pState->iCurrentPacketOffset = DRM_UI2I64(qwSeekPos);

    pState->dwPacketsProcessed = DRM_UI64Low32( qwSeekPacket );

    if ( ! Oem_File_SetFilePointer( pAsfContext->hInFile, DRM_UI64Low32( qwSeekPos ), OEM_FILE_BEGIN, &dwCurrentPos ) )
    {
        ChkDR( DRM_E_ND_FAILED_SEEK );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFTranscryptNextPackets(
    __inout                  NDT_ASF_CONTEXT        * const pAsfContext,
    __out_bcount(*pcbBuffer) DRM_BYTE               * const pbBuffer,
    __in                     DRM_DWORD                      dwOffset,
    __inout                  DRM_DWORD              * const pcbBuffer,
    __out                    DRM_BOOL               * const pfEndOfPackets )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_RESULT  drReadSample = DRM_SUCCESS;
    ASF_PARSER_STATE *pState = NULL;
    ASF_DATA_CONTEXT *pData = NULL;
    DRM_DWORD cbBufferSize = 0;
    DRM_DWORD dwBytesWritten = 0;
    DRM_DWORD dwTotalBytesWritten = 0;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;
    ASF_MULTIPLEXER_CONTEXT *pMux;
    NDT_SAMPLEINFO SampleInfo;
    ASF_SAMPLE *pSample = NULL;
    DRM_DWORD cbSampleData;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptNextPackets );

    /* Save buffer size and initialize out parameters */
    cbBufferSize = *pcbBuffer;
    *pcbBuffer = 0;
    *pfEndOfPackets = FALSE;

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    DRMASSERT( TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode );
    pData = &(pAsfTranscryptContext->Data.DataContext);

    /*
    ** pAsfContext->dwMaximumObjectSize is only set in function _ASFProcessStreamPropertiesObjectEx,
    ** and an Extended Stream Properties Object is not present in some files.
    */
    if ( pAsfContext->fAudioOnly == FALSE || pAsfContext->dwNoOfStreams != 1 )
    {
        cbSampleData = pAsfContext->dwMaximumObjectSize;
        DRMASSERT( cbSampleData > 0 );
    }

    /* If there is data yet to be delivered in the data buffer and a seek is not
     * pending or if the data object header has not been delivered, copy it to
     * the output buffer first.
     */
    if ( ( DRM_I64Les( pState->iCurrentFileOffset, pState->iFirstPacketOffset )
          ||  !pState->fSeekPositionSet ) )
    {
        DRMASSERT( pData->dwDataBufferOffset >= pData->dwSentDataOffset );

#if DRM_SUPPORT_AUDIO_ONLY == 1
        if ( pData->dwSentDataOffset != 0 )
        {
             ChkDR( _FillOutputBuffer( pAsfTranscryptContext, pbBuffer, dwOffset, cbBufferSize, &dwBytesWritten ) );
        }
#else
        if ( TRUE == pAsfContext->fAudioOnly && 1 == pAsfContext->dwNoOfStreams )
        {
            ChkDR( _FillOutputBuffer( pAsfTranscryptContext, pbBuffer, dwOffset, cbBufferSize, &dwBytesWritten ) );
        }
        else
        {
            ChkDR( _FillOutputBufferWithMuxPackets( pAsfTranscryptContext, pbBuffer, dwOffset, cbBufferSize, &dwBytesWritten ) );
        }
#endif
        /* If the output buffer is full, return immediatelly. */
        if ( dwBytesWritten == cbBufferSize )
        {
            *pcbBuffer = cbBufferSize;
            goto ErrorExit;
        }

        dwOffset += dwBytesWritten;
        dwTotalBytesWritten += dwBytesWritten;
    }
    else if ( pState->fSeekPositionSet )
    {
        /* If a seek is pending after the data header object has
         * been delivered, just flush the buffer.
         */
        pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset = 0;
        pAsfTranscryptContext->Data.DataContext.dwSentDataOffset   = 0;
        pAsfTranscryptContext->fAllPacketsTranscrypted = FALSE;
    }

    if ( pAsfTranscryptContext->fAllPacketsTranscrypted )
    {
        *pcbBuffer = dwOffset;
        *pfEndOfPackets = TRUE;
        goto ErrorExit;
    }

    /* If iCurrentFileOffset < iFirstPacketOffset, that means the Data Object
     * header wasn't parsed yet (i.e. this is the first call to this function
     * for the given ASF_TRANSCRYPT_CONTEXT).
     * In that case, we need to transcrypt the data object header before anything
     * else.
     */
    if ( DRM_I64Les( pState->iCurrentFileOffset, pState->iFirstPacketOffset ) )
    {
        ChkDR( _ASFTranscryptDataObjectHeader( pAsfContext ) );

        ChkDR( _FillOutputBuffer( pAsfTranscryptContext, pbBuffer, dwOffset, cbBufferSize, &dwBytesWritten ) );

        /* If the transcrypted data object header is enough to fill in the
         * out buffer, we're done for this call.
         */
        if ( dwBytesWritten + dwOffset == cbBufferSize )
        {
            *pcbBuffer = cbBufferSize;
            goto ErrorExit;
        }

        dwOffset += dwBytesWritten;
        dwTotalBytesWritten += dwBytesWritten;
    }

    /* Seek to the proper packet if a time position was set */
    if ( pState->fSeekPositionSet )
    {
        _ASFSeekToTimePosition(pAsfContext);
        pState->fSeekPositionSet = FALSE;
    }

    /* Keep transcrypting while we have space in the output buffer */
    do
    {

#if DRM_SUPPORT_AUDIO_ONLY == 1

        UNREFERENCED_PARAMETER( drReadSample );
        UNREFERENCED_PARAMETER( SampleInfo );
        UNREFERENCED_PARAMETER( pMux );

        ChkDR( _ASFTranscryptData( pAsfContext, &(pAsfTranscryptContext->fAllPacketsTranscrypted) ) );

        ChkDR( _FillOutputBuffer( pAsfTranscryptContext, pbBuffer, dwOffset, cbBufferSize, &dwBytesWritten ) );

        dwOffset += dwBytesWritten;
        dwTotalBytesWritten += dwBytesWritten;

#else
        pMux = (ASF_MULTIPLEXER_CONTEXT *) pAsfContext->ASFTranscryptContext.pMux;

        if ( TRUE == pAsfContext->fAudioOnly && 1 == pAsfContext->dwNoOfStreams )
        {
            ChkDR( _ASFTranscryptData( pAsfContext, &(pAsfTranscryptContext->fAllPacketsTranscrypted) ) );
            ChkDR( _FillOutputBuffer( pAsfTranscryptContext, pbBuffer, dwOffset, cbBufferSize, &dwBytesWritten ) );
        }
        else
        {

            drReadSample = NDTMGR_ASF_Internal_ReadSample( pAsfContext,
                                                           &SampleInfo,
                                                           &pSample,
                                                           &cbSampleData);

            if ( DRM_SUCCEEDED( drReadSample ) )
            {
                ChkFAIL( pSample != NULL );

                /* The mux wants the sample time to be the presentation time minus the preroll */
                pSample->hnsSampleTime = DRM_UI64Sub( pSample->hnsSampleTime,
                                                      DRM_UI64Mul( DRM_UI64( 10000 ),
                                                                   pAsfContext->qwPreroll ) );

                ChkDR( ASFMux_ProcessSample( pMux,
                                             (DRM_WORD) SampleInfo.dwStreamID,
                                             pSample,
                                             DRM_UI64(0) ) );

                ASFSample_Release( pSample );
                pSample = NULL;
            }
            else if ( DRM_E_NOMORE == drReadSample )
            {
                ChkDR( ASFMux_Flush( pMux ) );
            }
            else
            {
                ChkDR( drReadSample );
            }

            ChkDR( _FillOutputBufferWithMuxPackets( pAsfTranscryptContext, pbBuffer, dwOffset, cbBufferSize, &dwBytesWritten ) );
        }

        dwOffset += dwBytesWritten;
        dwTotalBytesWritten += dwBytesWritten;

        if ( DRM_E_NOMORE == drReadSample
             && pMux->m_WaitingPackets.dwCurrentSize == 0 )
        {
             pAsfTranscryptContext->fAllPacketsTranscrypted = TRUE;
        }

#endif /* DRM_SUPPORT_AUDIO_ONLY */

    } while( dwOffset < cbBufferSize && ! pAsfTranscryptContext->fAllPacketsTranscrypted );

    /* If we transcrypted all packets to the internal buffer AND
     * we copied everything from the internal buffer to the output buffer,
     * then it's safe to indicate the end of transcryption to the caller
     */
    if ( pAsfTranscryptContext->fAllPacketsTranscrypted &&
         pData->dwSentDataOffset == 0 && pData->dwDataBufferOffset == 0 )
    {
        *pfEndOfPackets = TRUE;
    }

    *pcbBuffer = dwOffset;

ErrorExit:
    if ( NULL != pSample )
    {
        ASFSample_Release( pSample );
        pSample = NULL;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT _ASFSetupTranscryption(
    __in_opt DRM_VOID        *f_pOEMContext,
    __inout  NDT_ASF_CONTEXT *pAsfContext
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    ASF_PARSER_STATE *pState = NULL;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFSetupTranscryption );

    ChkArg( pAsfContext );

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    DRMASSERT( NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode );

    ChkDR( _ASFProcessHeaders( f_pOEMContext, pAsfContext ) );

    /*
     *  NOTE: The call above will fill the internal header buffer but
     *  will not set the fHeaderDone flag. The first call to TranscryptNextBlock
     *  will then call _ASFProcessHeaders again and then copy the internal
     *  buffer to the output buffer. This can be optimized.
     */

    /* Assert pAsfTranscryptContext->qwDuration > 0 */
    DRMASSERT( DRM_UI64Les( DRM_UI64( 0 ), pAsfContext->qwDuration ) );
    if ( DRM_UI64Eql( pAsfContext->qwDuration, DRM_UI64( 0 ) ) )
    {
        /* Header parsing didn't get a valid Duration from the File Properties object */
        ChkDR( DRM_E_FAIL );
    }

    /* Allocate space for a few packets + some space for the Data Object header */
    ChkOverflow( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes, pAsfContext->dwAesPayloadExtensionAdditionalBytes );
    ChkOverflow( ( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) * ASF_PACKET_TRANSCRYPTS_PER_CALL, ( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) );
    ChkOverflow( (( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) * ASF_PACKET_TRANSCRYPTS_PER_CALL ) + DATA_OBJECT_SIZE, ( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) * ASF_PACKET_TRANSCRYPTS_PER_CALL );
    pAsfTranscryptContext->Data.DataContext.cbDataBufferSize =
        ( ( pState->dwPacketSize + pAsfContext->dwAesPayloadExtensionAdditionalBytes ) * ASF_PACKET_TRANSCRYPTS_PER_CALL )
        + DATA_OBJECT_SIZE;

#if SIXTEEN_BIT_ADDRESSING
    if( pAsfTranscryptContext->Data.DataContext.cbDataBufferSize > 0x1fffe )
    {
        TRACE(("Requested data buffer size %lu greater than 0xffff words, resetting\n", pAsfTranscryptContext->Data.DataContext.cbDataBufferSize));
        pAsfTranscryptContext->Data.DataContext.cbDataBufferSize = 0x1fffe;
    }
#endif

    SAFE_OEM_FREE(pAsfTranscryptContext->Data.DataContext.pbDataBuffer);
    ChkMem( pAsfTranscryptContext->Data.DataContext.pbDataBuffer = (DRM_BYTE*)Oem_MemAlloc( pAsfTranscryptContext->Data.DataContext.cbDataBufferSize ) );
    ZEROMEM( pAsfTranscryptContext->Data.DataContext.pbDataBuffer, pAsfTranscryptContext->Data.DataContext.cbDataBufferSize );
    pAsfTranscryptContext->Data.DataContext.dwDataBufferOffset = 0;

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    NDTMGR_ASF_Internal_InitializeTranscryption
**
** Synopsis:    Initializes the ASF context for transcryption.  A previous
**              call to NDTMGR_ASF_Internal_Initialize must be made before
**              calling this function.  The context initialized with this function
**              is then used in subsequent ASF trancryption calls.
**              Once done, the caller should use NDTMGR_ASF_Internal_Uninitialize
**              to free up the resources associated with the transcryption
**              context.
**
** Arguments:   [pAsfTranscryptContext] -- The ASF transcryption context
**              [pNDTManagerContext]    -- NDT Manager context
**              [idSession]             -- Session ID
**              [pbLicenseKeyID]        -- KID (generated by ProcessLicenseChallenge)
**              [cbLicenseKeyID]        -- KID size
**
** Returns:     DRM_SUCCESS on success
**              DRM_E_INVALIDARG if any of the arguments are invalid
**              DRM_E_OUTOFMEMORY if it runs out of memory
**********************************************************************/
DRM_RESULT DRM_CALL NDTMGR_ASF_Internal_InitializeTranscryption(
    __out                       NDT_ASF_CONTEXT * const pAsfContext,
    __in                        DRM_VOID        * const pNDTManagerContext,
    __in const                  DRM_ID                  idSession,
    __in_bcount(cbLicenseKeyID) DRM_BYTE        * const pbLicenseKeyID,
    __in const                  DRM_DWORD               cbLicenseKeyID
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_TRANSCRYPT_CONTEXT      *pAsfTranscryptContext = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_InitializeTranscryption );

    ChkArg( pAsfContext );
    ChkArg( pNDTManagerContext );
    ChkArg( pbLicenseKeyID );
    ChkArg( cbLicenseKeyID > 0 );

    pAsfTranscryptContext = &pAsfContext->ASFTranscryptContext;
    SAFE_OEM_FREE( pAsfTranscryptContext->Header.pbHeader );
    ZEROMEM( pAsfTranscryptContext, SIZEOF( ASF_TRANSCRYPT_CONTEXT ) );

    pAsfTranscryptContext->pNDTManagerContext = pNDTManagerContext;
    pAsfTranscryptContext->idSession = idSession;
    pAsfTranscryptContext->fAllPacketsTranscrypted = FALSE;
    pAsfTranscryptContext->pbLicenseKeyID = pbLicenseKeyID;
    pAsfTranscryptContext->cbLicenseKeyID = cbLicenseKeyID;

    pAsfTranscryptContext->State = ASF_TRANSCRYPTION_STATE_INITIALIZED;

ErrorExit:
    if ( DRM_FAILED( dr ) && pAsfTranscryptContext )
    {
        pAsfTranscryptContext->pNDTManagerContext = NULL;
        pAsfTranscryptContext->pbLicenseKeyID = NULL;
        pAsfTranscryptContext->cbLicenseKeyID = 0;

        pAsfTranscryptContext->State = ASF_TRANSCRYPTION_STATE_UNINITIALIZED;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    NDTMGR_ASF_Internal_Initialize
**
** Synopsis:    Initializes the ASF context for parsing ASF files.
**              The context initialized with this function
**              is then used in subsequent ASF calls.
**              Once done, the caller should use NDTMGR_ASF_Internal_Uninitialize
**              to free up the resources associated with the context.
**
** Arguments:   [pAsfContext]       -- The ASF context
**              [hFile]             -- Handle to an opened, readable ASF file
**
** Returns:     DRM_SUCCESS on success
**              DRM_E_INVALIDARG if any of the arguments are invalid
**              DRM_E_OUTOFMEMORY if it runs out of memory
**********************************************************************/
DRM_RESULT DRM_CALL NDTMGR_ASF_Internal_Initialize(
    __inout    NDT_ASF_CONTEXT * const pAsfContext,
    __in       OEM_FILEHDL             hFile
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PARSER_STATE *pState = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_Initialize );

    ChkArg( pAsfContext );
    ZEROMEM( pAsfContext, SIZEOF( NDT_ASF_CONTEXT ) );

    if ( hFile == OEM_INVALID_HANDLE_VALUE )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    pState = &(pAsfContext->ParserState);

    /* File buffer allocation */
    ChkMem( pAsfContext->pbFileBuffer = (DRM_BYTE *)Oem_MemAlloc( FILE_BUFFER_SIZE ) );

    pAsfContext->hInFile = hFile;

    if ( pState->pPacketInfo == NULL )
    {
        ChkMem( pState->pPacketInfo = (ASF_PACKET_PARSER_INFO*)Oem_MemAlloc( SIZEOF( ASF_PACKET_PARSER_INFO ) ) );
        ZEROMEM( pState->pPacketInfo, SIZEOF( ASF_PACKET_PARSER_INFO ) );
    }

    if ( pState->pPayloadInfo == NULL )
    {
        ChkMem( pState->pPayloadInfo = (ASF_PAYLOAD_PARSER_INFO*)Oem_MemAlloc( SIZEOF( ASF_PAYLOAD_PARSER_INFO ) ) );
        ZEROMEM( pState->pPayloadInfo, SIZEOF( ASF_PAYLOAD_PARSER_INFO ) );
    }

    ChkDR( DRM_PtrList_Initialize(&pAsfContext->StreamInfoList, 127) );
    ChkDR( DRM_PtrList_Initialize(&pAsfContext->Samples, 127) );

ErrorExit:
    if ( DRM_FAILED( dr ) && pAsfContext )
    {
        pAsfContext->hInFile = OEM_INVALID_HANDLE_VALUE;
        SAFE_OEM_FREE( pAsfContext->pbFileBuffer );
        SAFE_OEM_FREE( pAsfContext->ParserState.pPacketInfo );
        SAFE_OEM_FREE( pAsfContext->ParserState.pPayloadInfo );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    NDTMGR_ASF_Internal_GetFileInfo
**
** Synopsis:    Retrieves content encryption information about the
**              file associated with the ASF context.
**              Check the ASF_FILE_INFO structure to learn what
**              information is returned.
**
** Arguments:   [pAsfContext] -- The ASF context
**              [pFileInfo]   -- Pointer to caller allocated
**                               ASF_FILE_INFO structure. This
**                               structure will be filled on return.
**
** Returns:     DRM_SUCCESS on success
**              DRM_E_INVALIDARG if any of the arguments are invalid
**              DRM_E_OUTOFMEMORY if it runs out of memory
**              WM_E_BADASFHEADER if it finds problems parsing ASF headers
**********************************************************************/
DRM_RESULT DRM_CALL NDTMGR_ASF_Internal_GetFileInfo(
    __inout NDT_ASF_CONTEXT * const pAsfContext,
    __inout ASF_FILE_INFO   * const pFileInfo
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_DWORD         dwStreamCount = 0;
    DRM_BOOL    fFoundAudioStream = FALSE;
    PTR_LIST_ENTRY  *pList;
    ASF_STREAM_INFO *pInfo;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_GetFileInfo );

    ChkArg( pAsfContext );
    ChkArg( pFileInfo );

    /* Initialize file info structure */
    ZEROMEM( pFileInfo, SIZEOF( ASF_FILE_INFO ) );

    ChkDR( _ASFParseHeaders( pAsfContext ) );

    for (pList = pAsfContext->StreamInfoList.pHead; NULL != pList; pList = pList->pNext)
    {
        pInfo = (ASF_STREAM_INFO *) pList->pbData;
        dwStreamCount++;
        if( DRM_IsEqualGUID( &pInfo->guidStreamType, &CLSID_ASFStreamTypeAudioMedia ) )
        {
            fFoundAudioStream = TRUE;
        }
    }


    MEMCPY( pFileInfo, &pAsfContext->ASFFileInfo, SIZEOF( ASF_FILE_INFO ));
    ZEROMEM( &pAsfContext->ASFFileInfo, SIZEOF( ASF_FILE_INFO ) );

    /* AudioOnly only true if there is only one stream of type audio */
    pFileInfo->fAudioOnly = ( ( fFoundAudioStream && dwStreamCount == 1 ) ? TRUE : FALSE );
    pFileInfo->dwStreamCount = dwStreamCount;

    pAsfContext->fAudioOnly = pFileInfo->fAudioOnly;
    pAsfContext->dwNoOfStreams = dwStreamCount;

ErrorExit:
    if ( DRM_FAILED( dr ) && pFileInfo )
    {
        if ( pFileInfo->pContentEncObjExt )
        {
            SAFE_OEM_FREE( pFileInfo->pContentEncObjExt->pbData );
            SAFE_OEM_FREE( pFileInfo->pContentEncObjExt );
        }

        if ( pFileInfo->pContentEncObj )
        {
            SAFE_OEM_FREE( pFileInfo->pContentEncObj->pbKeyID );
            SAFE_OEM_FREE( pFileInfo->pContentEncObj->pbLicenseURL );
            SAFE_OEM_FREE( pFileInfo->pContentEncObj->pbProtectionType );
            SAFE_OEM_FREE( pFileInfo->pContentEncObj->pbSecretData );
            SAFE_OEM_FREE( pFileInfo->pContentEncObj );
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    NDTMGR_ASF_Internal_ClearFileInfo
**
** Synopsis:    Clears up the ASF_FILE_INFO structure pointer passed in
**              as parameter. It should be used after a call to
**              NDTMGR_ASF_Internal_GetFileInfo to release all buffers allocated
**              in that function.
**
** Arguments:   [pFileInfo] -- Pointer to the structured to be cleared out.
**
** Returns:     DRM_SUCCESS on success
**
**********************************************************************/
DRM_RESULT DRM_CALL NDTMGR_ASF_Internal_ClearFileInfo(
    __out ASF_FILE_INFO * const pFileInfo
)
{
    if ( pFileInfo )
    {
        _ClearContentEncObj( pFileInfo->pContentEncObj );

        _ClearContentEncObjExt( pFileInfo->pContentEncObjExt );

        ZEROMEM( pFileInfo, SIZEOF( ASF_FILE_INFO ) );
    }

    return DRM_SUCCESS;
}



/**********************************************************************
**
** Function:    _ASF_TranscryptNextBlock
**
** Synopsis:    Transcrypt ASF files from Cocktail encrypted to AES
**              encrypted. This function will transcrypt the ASF header
**              first and then the packets. It will to fill the output
**              buffer everytime it is called. The EndOfPackets flags
**              indicates whether the transcryption reached the end of
**              the file. NDTMGR_ASF_Internal_Uninitialize should be called to free
**              up the buffers used during transcryption.
**
** Arguments:   [f_pOEMContext]     -- Pointer to the OEM context; may be NULL
**              [pAsfContext]       -- The ASF context
**              [ppBuffer]          -- Address of the pointer to the buffer
**                                     containing the transcrypted ASF header.
**              [pcbBufferSize]     -- Address of a DRM_DWORD that, on input,
**                                     will contain the size of the buffer where
**                                     the transcrypted content should be placed
**                                     in. On output, it specifies how many bytes
**                                     were written to the buffer.
**              [pfEndOfPackets]    -- Address of a flag that indicates whether
**                                     the transcryption function reached the
**                                     end of the content.
**
** Returns:     DRM_SUCCESS on success
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_OUTOFMEMORY if it runs out of memory
**              WM_E_BADASFHEADER if it finds problems parsing ASF headers
**              WM_E_BADPACKETHEADER if it finds problems parsing an ASF packet header
**              WM_E_BADPAYLOADHEADER if it finds problems parsing an ASF payload header
**              WM_E_BADDATAHEADER if it finds problems parsing an ASF data object header
**
**********************************************************************/
static DRM_RESULT DRM_CALL _ASFTranscryptNextBlock(
    __in_opt                  DRM_VOID        *f_pOEMContext,
    __inout                   NDT_ASF_CONTEXT *pAsfContext,
    __out_bcount(*pcbBuffer)  DRM_BYTE        *pbBuffer,
    __inout                   DRM_DWORD       *pcbBuffer,
    __out                     DRM_BOOL        *pfEndOfPackets
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    ASF_PARSER_STATE *pState = NULL;
    DRM_DWORD cbBufferSize = 0;
    DRM_DWORD dwBytesWritten = 0;
    DRM_DWORD dwOffset = 0;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC__ASFTranscryptNextBlock );

    ChkArg( pAsfContext );
    ChkArg( pbBuffer );
    ChkArg( pcbBuffer );
    ChkArg( pfEndOfPackets );

    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    DRMASSERT( TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode );

    /* Save buffer size and initialize out parameters */
    cbBufferSize = *pcbBuffer;
    *pcbBuffer = 0;
    *pfEndOfPackets = FALSE;

    pState = &(pAsfContext->ParserState);

    /* Check transcryption engine state */
    if ( pAsfTranscryptContext->State != ASF_TRANSCRYPTION_STATE_INITIALIZED )
    {
        ChkDR( DRM_E_ASF_INVALID_OPERATION );
    }

    /* First, process the ASF header */
    if ( ! pState->fHeaderDone )
    {
        dwBytesWritten = cbBufferSize;
        ChkDR( _ASFTranscryptHeader( f_pOEMContext, pAsfContext, pbBuffer, &dwBytesWritten ) );
        if ( dwBytesWritten == cbBufferSize )
        {
            *pcbBuffer = cbBufferSize;
            goto ErrorExit;
        }

        dwOffset = dwBytesWritten;
        ChkDR( _ASFSetHeaderDone(pAsfContext) );
        if ( !pState->fSeekPositionSet )
        {
            DRMASSERT( DRM_I64Eql(pState->iCurrentFileOffset, DRM_I64Sub(pState->iFirstPacketOffset, DRM_I64(DATA_OBJECT_SIZE) ) ) );
            pState->iCurrentPacketOffset = pState->iFirstPacketOffset;
            pState->iNextPacketOffset = pState->iFirstPacketOffset;
        }
    }

    /* If we get here, we're done with the header. Let's go ahead and
     * transcrypt the ASF packets.
     */
    dwBytesWritten = cbBufferSize;
    ChkDR( _ASFTranscryptNextPackets( pAsfContext,
                                      pbBuffer,
                                      dwOffset,
                                      &dwBytesWritten,
                                      pfEndOfPackets ) );

    *pcbBuffer = dwBytesWritten;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    NDTMGR_ASF_Internal_SetTimePosition
**
** Synopsis:    Used for seeking, this function will set the ASF
**              context so that the next time GetNextDataBlock is called
**              it will start from the position in time specified
**              in the call to SetTimePosition.
**
** Arguments:   [f_pOEMContext]  -- Pointer to the OEM context; may be NULL
**              [pAsfContext]    -- The ASF context
**              [dwTimePosition  -- Time position in milliseconds where
**                                  the transcryption engine should start
**                                  from the next time TranscryptNextBlock
**                                  is called.
**
** Returns:     DRM_SUCCESS on success
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_OUTOFMEMORY if it runs out of memory
**              WM_E_BADASFHEADER if it finds problems parsing ASF headers
**              WM_E_BADPACKETHEADER if it finds problems parsing an ASF packet header
**              WM_E_BADPAYLOADHEADER if it finds problems parsing an ASF payload header
**              WM_E_BADDATAHEADER if it finds problems parsing an ASF data object header
**
**********************************************************************/
DRM_RESULT DRM_CALL NDTMGR_ASF_Internal_SetTimePosition(
    __in_opt   DRM_VOID        *f_pOEMContext,
    __inout    NDT_ASF_CONTEXT *pAsfContext,
    __in       DRM_DWORD        dwTimePosition
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    ASF_PARSER_STATE *pState = NULL;
    DRM_UINT64  qwSeekPacket = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64  qwSeekTime = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64  qwSeekPos = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64  hnsTimePosition;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_SetTimePosition );

    ChkArg( pAsfContext );

    pState = &(pAsfContext->ParserState);

    if ( DRM_UI64Eql( pAsfContext->qwDuration, DRM_UI64( 0 ) ) )
    {
        /* Header Object has not been parsed yet, do that now */
        ChkDR( _ASFProcessHeaders( f_pOEMContext, pAsfContext ) );
    }
    /* Assert pAsfContext->qwDuration > 0 */
    DRMASSERT( DRM_UI64Les( DRM_UI64( 0 ), pAsfContext->qwDuration ) );
    if ( DRM_UI64Eql( pAsfContext->qwDuration, DRM_UI64( 0 ) ) )
    {
        /* Header parsing didn't get a valid Duration from the File Properties object */
        ChkDR( DRM_E_FAIL );
    }

    /* Detect overflow */
    if ( ! DRM_UI64Eql(DRM_UI64(dwTimePosition + DRM_UI64Low32(pAsfContext->qwPreroll)),
                       DRM_UI64Add(DRM_UI64(dwTimePosition),
                                   pAsfContext->qwPreroll)) )
    {
            ChkDR( DRM_E_ND_FAILED_SEEK );
    }

    dwTimePosition += DRM_UI64Low32(pAsfContext->qwPreroll);
    hnsTimePosition = DRM_UI64Mul(DRM_UI64(dwTimePosition), DRM_UI64(10000));

    /* Don't let the specified seek time go beyond end of file */
    if ( DRM_UI64Les( hnsTimePosition, pAsfContext->qwDuration ) )
    {
        qwSeekTime = hnsTimePosition;
    }
    else
    {
        qwSeekTime = pAsfContext->qwDuration;
    }

    /* Calculate packet we should land on */
    qwSeekPacket = DRM_UI64Div(
        DRM_UI64Mul(
            DRM_UI64( pState->dwTotalPackets ),
            qwSeekTime
            ),
        pAsfContext->qwDuration
    );

    /* Convert packet into a file offset */
    qwSeekPos = DRM_I2UI64(pState->iFirstPacketOffset);
    if ( DRM_UI64Les( DRM_UI64( 0 ), qwSeekPacket ) )
    {
        qwSeekPos = DRM_UI64Add(
            qwSeekPos,
            DRM_UI64Mul( qwSeekPacket, DRM_UI64( pState->dwPacketSize ) )
        );
    }

    /* Seek past end of file not allowed */
    if ( DRM_UI64Les( DRM_I2UI64( pState->iLastPacketOffset ), qwSeekPos ) )
    {
        ChkDR( DRM_E_ND_FAILED_SEEK );
    }

    pState->dwTimePosition = dwTimePosition;
    pState->qwSeekPosition = qwSeekPos;
    pState->qwSeekPacket   = qwSeekPacket;
    pState->fSeekPositionSet = TRUE;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    NDTMGR_ASF_Internal_Uninitialize
**
** Synopsis:    Unitializes the ASF context and frees up all
**              memory associated with it.
**
** Arguments:   [pAsfContext] -- The ASF context
**
** Returns:     DRM_SUCCESS on success
**
**********************************************************************/
DRM_RESULT DRM_CALL NDTMGR_ASF_Internal_Uninitialize(
    __inout  NDT_ASF_CONTEXT * const pAsfContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_TRANSCRYPT_CONTEXT          *pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    ASF_TRANSCRYPT_HEADER_CONTEXT   *pHeader = &(pAsfTranscryptContext->Header);
    ASF_DATA_CONTEXT                *pData = &(pAsfTranscryptContext->Data.DataContext);
    ASF_TRANSCRYPT_DATA_CONTEXT     *pTranscryptData = &(pAsfTranscryptContext->Data);


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_Uninitialize );

    ChkArg( pAsfContext );

#if DRM_SUPPORT_AUDIO_ONLY == 0
    if ( (ASF_MULTIPLEXER_CONTEXT *)pAsfTranscryptContext->pMux != NULL )
    {
        ASFMux_Flush( (ASF_MULTIPLEXER_CONTEXT *)pAsfTranscryptContext->pMux );
    }
    ASFMux_Uninitialize((ASF_MULTIPLEXER_CONTEXT *) pAsfTranscryptContext->pMux);
    SAFE_OEM_FREE(pAsfTranscryptContext->pMux);
#endif /* DRM_SUPPORT_AUDIO_ONLY == 0 */

    if ( NULL != pAsfTranscryptContext->pOutputPacket )
    {
        ASFSample_Release( pAsfTranscryptContext->pOutputPacket );
        pAsfTranscryptContext->pOutputPacket = NULL;
    }

    if ( pHeader->pbHeader )
    {
        SAFE_OEM_FREE( pHeader->pbHeader );
        ZEROMEM( pHeader, SIZEOF( ASF_TRANSCRYPT_HEADER_CONTEXT ) );
    }

    if ( pData->pbDataBuffer )
    {
        SAFE_OEM_FREE( pData->pbDataBuffer );
        ZEROMEM( pTranscryptData, SIZEOF( ASF_TRANSCRYPT_DATA_CONTEXT ) );
    }

    SAFE_OEM_FREE( pAsfContext->pbFileBuffer );

    while( 0 < pAsfContext->StreamInfoList.dwCurrentSize )
    {
        ASF_STREAM_INFO *pFreeMe = NULL;
        ChkDR( DRM_PtrList_RemoveHead( &pAsfContext->StreamInfoList, (DRM_VOID **) &pFreeMe ) );
        if(pFreeMe)
        {
            SAFE_OEM_FREE( pFreeMe->pbStreamPropertiesObj );
        }
        SAFE_OEM_FREE( pFreeMe );
    }
    DRM_PtrList_Uninitialize( &pAsfContext->StreamInfoList );
    _FreeSamples( pAsfContext );

    pAsfTranscryptContext->pNDTManagerContext = NULL;

    _ParseClose( &(pAsfContext->ParserState) );

    NDTMGR_ASF_Internal_ClearFileInfo( &pAsfContext->ASFFileInfo );

    pAsfTranscryptContext->fAllPacketsTranscrypted = FALSE;
    ZEROMEM( &(pAsfTranscryptContext->guidFileID), SIZEOF( DRM_GUID ) );
    pAsfTranscryptContext->pbLicenseKeyID = NULL;
    pAsfTranscryptContext->cbLicenseKeyID = 0;
    SAFE_OEM_FREE( pAsfContext->InclusionList.pguidBuffer );

    pAsfTranscryptContext->State = ASF_TRANSCRYPTION_STATE_UNINITIALIZED;

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************
** The remainder of this file consists of calls that implement the
** NDTMGR ASF interface to the transcription engine.  These calls
** underly the primary NDTMGR_ASF APIs for delivering ASF media.
******************************************************************/
DRM_RESULT DRM_CALL NDTMGR_ASF_Internal_GetHeader(
    __in                     NDT_ASF_CONTEXT  * const pAsfContext,
    __out_bcount(*pcbHeader) DRM_BYTE         * const pbHeader,
    __inout                  DRM_DWORD        * const pcbHeader
)
{
    DRM_RESULT        dr = DRM_SUCCESS;
    ASF_PARSER_STATE *pState = NULL;
    DRM_DWORD         cbHeaderBytesRead = 0;
    DRM_DWORD         cbHeaderBytesRemaining;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_GetHeader );

    pState = &(pAsfContext->ParserState);

    ChkArg( pAsfContext != NULL &&
            pcbHeader != NULL &&
            (*pcbHeader == 0 || pbHeader != NULL ) );

/*
 * NOTE: transcrypting the header for sample mode doesn't need to
 * do everything that transcrypting for muxed mode does.  Right now
 * sample mode does no header transcryption, but it needs to at least
 * add the advanced content encryption object.
 * Sample mode does not have to transcrypt the packet and payload
 * headers.
 *
 */

    ChkDR( _ASFParseHeaders(pAsfContext) );

    pState->iCurrentFileOffset = DRM_I64(0);
    if ( ! Oem_File_SetFilePointer( pAsfContext->hInFile, 0, OEM_FILE_BEGIN, NULL ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    cbHeaderBytesRemaining = DRM_UI64Low32(pState->qwHeaderSize);
    if(*pcbHeader < cbHeaderBytesRemaining)
    {
        *pcbHeader = cbHeaderBytesRemaining;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    cbHeaderBytesRead = cbHeaderBytesRemaining;
    if ( !Oem_File_Read( pAsfContext->hInFile, (DRM_VOID*)pbHeader, cbHeaderBytesRead, &cbHeaderBytesRead ) )
    {
        ChkDR( DRM_E_FILEREADERROR );
    }

    if ( cbHeaderBytesRead != cbHeaderBytesRemaining )
    {
        ChkDR( DRM_E_FILEREADERROR );
    }

    pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( cbHeaderBytesRead ) );
    ChkDR( _ASFSetHeaderDone( pAsfContext ) );
    pState->tParseState = P_NEW_PACKET;

    if ( !pState->fSeekPositionSet )
    {
        pState->iCurrentPacketOffset = pState->iFirstPacketOffset;
        pState->iNextPacketOffset = pState->iFirstPacketOffset;
        Oem_File_SetFilePointer(pAsfContext->hInFile, DRM_UI64Low32(DRM_I2UI64(pState->iFirstPacketOffset)), OEM_FILE_BEGIN, NULL);
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_RESULT NDTMGR_ASF_Internal_ReadSample(
    __inout    NDT_ASF_CONTEXT  * const pAsfContext,
    __out      NDT_SAMPLEINFO   * const pSampleInfo,
    __out      ASF_SAMPLE             **ppSample,
    __in const DRM_DWORD        * const pcbSampleData
)
{
    DRM_RESULT                dr = DRM_SUCCESS;
    DRM_DWORD                 dwNowWanted = 0;
    ASF_MEDIA_BUFFER         *pNewMediaBuffer = NULL;
    DRM_BYTEBLOB              NewBlob = { NULL , 0};
    DRM_BOOL                  bFragmented = FALSE;
    NDT_CRYPTO_DATA_ASF       cryptoData;
    ASF_TRANSCRYPT_CONTEXT   *pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    ASF_PARSER_STATE         *pState = &(pAsfContext->ParserState);
    ASF_STREAM_INFO          *pStreamInfo = NULL;
    ASF_SAMPLE               *pCurrentSample = NULL;
    ASF_SAMPLE               *pNewSample = NULL;
    DRM_BOOL                  fReady;
    DRM_BOOL                  fResult;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_ReadSample );

    if ( pState->fSeekPositionSet )
    {
        _ASFSeekToTimePosition(pAsfContext);
        pState->fSeekPositionSet = FALSE;
        pState->tParseState = P_NEW_PACKET;
    }

    while ( DRM_SUCCEEDED( _GetNextReadySample( pAsfContext, &pCurrentSample, &fReady ) ) )
    {
        if ( TRUE == fReady )
        {
            break;
        }

        if ( DRM_I64Les(pState->iLastPacketOffset, pState->iCurrentPacketOffset) )
        {
            pState->tParseState = P_END;
            ChkDR(DRM_E_NOMORE);
        }

        if ( P_NEW_PACKET == pState->tParseState )
        {
            /* Parse PACKET header */
            pState->iCurrentFileOffset = pState->iCurrentPacketOffset;
            if ( ! Oem_File_SetFilePointer( pAsfContext->hInFile, DRM_UI64Low32(DRM_I2UI64(pState->iCurrentFileOffset)), OEM_FILE_BEGIN, NULL ) )
            {
                ChkDR(DRM_E_FAIL);
            }
            ChkDR( _ASFParsePacketHeader( pAsfContext->hInFile, pAsfContext->pbFileBuffer, &pAsfContext->ParserState ) );
            pState->dwPayload = 0;
        }

        if ( pState->pPacketInfo->fEccPresent &&
             pState->pPacketInfo->fParityPacket )
        {
            /* Go to next packet */
            pState->tParseState = P_NEW_PACKET;
            pState->iPreviousPacketOffset = pState->iCurrentPacketOffset;
            if ( DRM_I64Eql( pState->iCurrentPacketOffset, pState->iNextPacketOffset ) )
            {
                pState->iNextPacketOffset = DRM_I64Add( pState->iNextPacketOffset, DRM_I64( pState->dwPacketSize ) );
            }
            pState->iCurrentPacketOffset = pState->iNextPacketOffset;
            pState->iNextPacketOffset = DRM_I64Add( pState->iNextPacketOffset, DRM_I64( pState->dwPacketSize ) );

            continue;
        }

        pState->tParseState = P_PAYLOAD_HEADER;

        /* Parse PAYLOAD header */
        ChkDR( _ASFParsePayloadHeader( pAsfContext->hInFile, pAsfContext->pbFileBuffer, &pAsfContext->ParserState ) );

        pState->wPayStart = pState->pPayloadInfo->wPacketOffset +
            pState->pPayloadInfo->wTotalSize -
            pState->pPayloadInfo->wPayloadSize;

        /* if pState->iCurrentPacketOffset + pState->wPayStart > pState->iCurrentFileOffset */
        if ( DRM_I64Les( pState->iCurrentFileOffset, DRM_I64Add( pState->iCurrentPacketOffset, DRM_I64( pState->wPayStart ) ) ) )
        {
            /* dwSkipBytes = pState->iCurrentPacketOffset + pState->wPayStart - pState->iCurrentFileOffset */
            DRM_DWORD dwSkipBytes = DRM_I64ToUI32(
                DRM_I64Sub(
                    DRM_I64Add( pState->iCurrentPacketOffset, DRM_I64(pState->wPayStart ) ),
                    pState->iCurrentFileOffset
                    )
                );

            pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwSkipBytes ) );
        }

        pState->tParseState = P_PAYLOAD;

        if ( pState->pPayloadInfo->wPayloadSize <= 0 )
        {
            pState->tParseState = P_PAYLOAD_END;
            goto ErrorExit;
        }

        pState->dwPayloadOffset = DRM_UI64Low32( DRM_UI64Add( DRM_I2UI64( pState->iCurrentPacketOffset ), DRM_UI64( pState->wPayStart ) ) );

        if( pState->pPayloadInfo->bRepData != 1 )
        {
            pState->tParseState = P_UNCOMPRESSEDPAYLOAD;
            pState->pPayloadInfo->fIsCompressedPayload = FALSE;
        }
        else
        {
            pState->tParseState = P_COMPRESSEDPAYLOAD;
            pState->pPayloadInfo->fIsCompressedPayload = TRUE;
        }

        if ( pState->tParseState == P_UNCOMPRESSEDPAYLOAD )
        {
            dwNowWanted = pState->pPayloadInfo->wPayloadSize;

            NewBlob.pbBlob = (DRM_BYTE *) Oem_MemAlloc( dwNowWanted );
            NewBlob.cbBlob = dwNowWanted;
            ChkMem( NewBlob.pbBlob );

            pNewMediaBuffer = (ASF_MEDIA_BUFFER *) Oem_MemAlloc( SIZEOF( ASF_MEDIA_BUFFER ) );
            ChkMem( pNewMediaBuffer );
            ChkDR( ASFMediaBuffer_Initialize( pNewMediaBuffer, NewBlob, 0 ) );
            NewBlob.pbBlob = NULL;
            ChkFAIL( pNewMediaBuffer != NULL );

            _ASFReadData( pAsfContext->hInFile, pNewMediaBuffer->blobBuffer.pbBlob, pNewMediaBuffer->blobBuffer.cbBlob );

            pState->iCurrentFileOffset = DRM_I64Add( pState->iCurrentFileOffset, DRM_I64( dwNowWanted ) );

            /* Find out if this is a fragmented packet */
            bFragmented = (pState->pPayloadInfo->dwObjectSize > pState->pPayloadInfo->wPayloadSize) ? TRUE : FALSE;

            if ( FALSE == bFragmented || 0 == pState->pPayloadInfo->dwObjectOffset )
            {
                /* Need to add a new sample to the queue */
                pNewSample = (ASF_SAMPLE *) Oem_MemAlloc( SIZEOF( *pNewSample ) );
                ChkMem( pNewSample );

                ChkDR( ASFSample_Initialize( pNewSample) );
                pNewSample->dwStreamNumber = (DRM_DWORD) pState->pPayloadInfo->bStreamId;
                pNewSample->hnsSampleTime = DRM_UI64Mul( DRM_UI64( pState->pPayloadInfo->dwObjectPres ),
                                                      DRM_UI64(10000) );
                if ( pState->pPayloadInfo->fIsKeyFrame )
                {
                    pNewSample->dwFlags |= MF_SAMPLE_FLAG_CLEANPOINT;
                }
                ChkDR( _FindStreamInStreamInfoList(pAsfContext,
                                                   (DRM_DWORD) pState->pPayloadInfo->bStreamId,
                                                   &pStreamInfo) );
                /* duration = sample size * 8 * 10000 / bit rate */
                ChkFAIL( pStreamInfo->dwAverageBitrate != 0 );
                pNewSample->hnsSampleDuration = DRM_UI64Div(
                    DRM_UI64Mul( DRM_UI64(pState->pPayloadInfo->dwObjectSize),
                                 DRM_UI64((DRM_DWORD) 80000)),
                    DRM_UI64(pStreamInfo->dwAverageBitrate) );

                ChkDR( DRM_PtrList_AddTail( &pAsfContext->Samples, pNewSample ) );
                ChkDR( ASFSample_AddRef( pNewSample ) );
                pCurrentSample = pNewSample;
                pNewSample = NULL;
            }
            else
            {
                /* Sample already exists.  Find it */
                ChkDR( _GetSampleInProgress( pAsfContext,
                                             pState->pPayloadInfo->bStreamId,
                                             pState->pPayloadInfo->dwObjectPres,
                                             &pCurrentSample ) );
            }

            ChkDR( ASFSample_AddBuffer( pCurrentSample, pNewMediaBuffer ) );

            /* This function is done with the new media buffer and AddBuffer() adds a
             * ref for the media buffer in the sample, so we must release here
             */
            ASFMediaBuffer_Release( pNewMediaBuffer );
            pNewMediaBuffer = NULL;

            if ( pCurrentSample->cbTotalLength == pState->pPayloadInfo->dwObjectSize )
            {
                pCurrentSample->dwFlags |= ASF_SAMPLE_READY;
            }
        }
        else
        {
            DRMASSERT( !"Compressed payload not supported for now" );
            ChkDR( DRM_E_NOTIMPL );
        }
        if ( NULL != pCurrentSample )
        {
            ASFSample_Release( pCurrentSample );
            pCurrentSample = NULL;
        }

        /* successfully got a payload, move on to next */
        pState->dwPayload++;

        if ( pState->dwPayload >= pState->pPacketInfo->dwPayloads )
        {
            /* We're done with the payload(s) in this packet */
            pState->tParseState = P_NEW_PACKET;

            pState->iPreviousPacketOffset = pState->iCurrentPacketOffset;
            if ( DRM_I64Eql(pState->iCurrentPacketOffset, pState->iNextPacketOffset ) )
            {
                pState->iNextPacketOffset = DRM_I64Add( pState->iNextPacketOffset, DRM_I64( pState->dwPacketSize ) );
            }
            pState->iCurrentPacketOffset = pState->iNextPacketOffset;
            pState->iNextPacketOffset = DRM_I64Add( pState->iNextPacketOffset, DRM_I64( pState->dwPacketSize ) );
        }

    }

    if ( *pcbSampleData < pCurrentSample->cbTotalLength )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }
    ChkDR(dr);

    ChkDR( _FindStreamInStreamInfoList(pAsfContext, pCurrentSample->dwStreamNumber, &pStreamInfo) );
    pCurrentSample->bRepData = pState->pPayloadInfo->bRepData;
    MEMCPY( pCurrentSample->rgbRepData, pState->pPayloadInfo->rgbRepData, pState->pPayloadInfo->bRepData );

    /* Do Transcryption if necessary */
    if ( TRUE == pAsfContext->fProtectedContentPlayback )
    {
        DRM_DWORD  i;
        DRM_DWORD  cBuffers;
        ASF_MEDIA_BUFFER *pMediaBuffer = NULL;
        DRM_DWORD  cbTotal = 0;
        DRM_DWORD  dwRepDataOffset = 0;
        DRM_WORD   wTmpSizeOfSampleID = SIZEOF( DRM_UINT64 );

        ChkDR( ASFSample_GetBufferCount( pCurrentSample, &cBuffers ) );
        for (i = 0; i < cBuffers; i++)
        {
            ChkDR( ASFSample_GetBufferByIndex( pCurrentSample, i, &pMediaBuffer ) );
            ChkFAIL( pMediaBuffer != NULL );

            if ( ( !pAsfContext->fEncryptClear ) &&
                 ( pAsfContext->eCipherType == eDRM_RC4_CIPHER  ) )
            {
                ChkDR( _DRMDecrypt( &pAsfContext->decryptContext, pMediaBuffer->blobBuffer.pbBlob, pMediaBuffer->blobBuffer.cbBlob ) );
            }

            /* Re-encrypt using AES */
            cryptoData.SampleData.pbBlob = pMediaBuffer->blobBuffer.pbBlob;
            cryptoData.SampleData.cbBlob = pMediaBuffer->blobBuffer.cbBlob;
            cryptoData.qwSampleID = pStreamInfo->qwCurrentSampleID;
            cryptoData.qwOffset = DRM_UI64( cbTotal );

            if ( pAsfContext->eCipherType != eDRM_AES_COUNTER_CIPHER )
            {
                ChkDR( NDTMGR_Encrypt( (NDTMGR_CONTEXT *) pAsfTranscryptContext->pNDTManagerContext,
                                       pAsfTranscryptContext->idSession,
                                       NDT_ENCRYPTION_MODE_ASF,
                                       (DRM_BYTE*) &cryptoData ) );
            }

            cbTotal += pMediaBuffer->blobBuffer.cbBlob;
            ASFMediaBuffer_Release( pMediaBuffer );
            pMediaBuffer = NULL;
        }

        if( pAsfContext->dwAesPayloadExtensionAdditionalBytes > 0 )
        {
            /* Add Sample ID to replicated data */
            dwRepDataOffset = pCurrentSample->bRepData;
            SaveWORDOffset( pCurrentSample->rgbRepData, wTmpSizeOfSampleID, dwRepDataOffset );
            QWORD_TO_NETWORKBYTES( pCurrentSample->rgbRepData, dwRepDataOffset, pStreamInfo->qwCurrentSampleID);
            pCurrentSample->bRepData += ( DRM_BYTE )( pAsfContext->dwAesPayloadExtensionAdditionalBytes );
        }
        else if( pAsfContext->rgdwSampleIDRepDataOffset[pCurrentSample->dwStreamNumber] == 0xFFFF )
        {
            /* At least one variable length field is in the payload extension before the sample ID.
            ** Currently we don't support extracting the sample ID in this case. For mux mode this
            ** is fine, but for sample mode the app may not be able to rely on the returned sample ID
            */
            ZEROMEM( &pAsfContext->qwNextSampleID, SIZEOF( DRM_UINT64 ) );
        }
        else
        {
            /* Sample ID already present in file, just read it out */

            ChkOverflow( pAsfContext->rgdwSampleIDRepDataOffset[pCurrentSample->dwStreamNumber] + (2 * SIZEOF( DRM_DWORD ) ) + SIZEOF( DRM_UINT64 ), pAsfContext->rgdwSampleIDRepDataOffset[pCurrentSample->dwStreamNumber] );
            ChkBOOL( pAsfContext->rgdwSampleIDRepDataOffset[pCurrentSample->dwStreamNumber] + (2 * SIZEOF( DRM_DWORD ) ) + SIZEOF( DRM_UINT64 ) <= pCurrentSample->bRepData, DRM_E_ASF_BAD_PAYLOAD_HEADER );
            NETWORKBYTES_TO_QWORD( pStreamInfo->qwCurrentSampleID, pCurrentSample->rgbRepData, pAsfContext->rgdwSampleIDRepDataOffset[pCurrentSample->dwStreamNumber] + (2 * SIZEOF( DRM_DWORD ) ) );
        }
    }

    pSampleInfo->dwStreamID         = pCurrentSample->dwStreamNumber;
    pSampleInfo->qwSampleTimeNS     = pCurrentSample->hnsSampleTime;
    pSampleInfo->qwSampleDurationNS = pCurrentSample->hnsSampleDuration;
    pSampleInfo->qwSampleID        = pStreamInfo->qwCurrentSampleID;
    pStreamInfo->qwCurrentSampleID = pAsfContext->qwNextSampleID;
    pAsfContext->qwNextSampleID    = DRM_UI64Add(pAsfContext->qwNextSampleID, DRM_UI64(1));

    *ppSample = pCurrentSample;
    ChkDR( ASFSample_AddRef( *ppSample ) ); /* add ref for caller */
    fResult = _RemoveSample( pAsfContext, pCurrentSample );
    DRMASSERT( fResult );
    ChkBOOL( fResult, DRM_E_ASF_UNEXPECTED );

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        /* Go to next packet */
        pState->tParseState = P_PAYLOAD_END;
    }

    SAFE_OEM_FREE( NewBlob.pbBlob );

    if ( NULL != pNewMediaBuffer )
    {
        ASFMediaBuffer_Release( pNewMediaBuffer );
        pNewMediaBuffer = NULL;
    }

    if ( NULL != pNewSample )
    {
        ASFSample_Release( pNewSample );
        pNewSample = NULL;
    }

    if ( NULL != pCurrentSample )
    {
        ASFSample_Release( pCurrentSample );
        pCurrentSample = NULL;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_RESULT NDTMGR_ASF_Internal_ReadData(
    __in_opt                 DRM_VOID        *f_pOEMContext,
    __inout                  NDT_ASF_CONTEXT *pAsfContext,
    __out_bcount(*f_pcbData) DRM_BYTE        *f_pbData,
    __inout                  DRM_DWORD       *f_pcbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwWanted = *f_pcbData;
    DRM_DWORD cbHeaderBytesRead = 0;
    ASF_PARSER_STATE *pState;
    ASF_TRANSCRYPT_HEADER_CONTEXT *pHeader;
    DRM_BOOL fEndOfPackets = FALSE;
    DRM_BOOL fTranscrypting;
    ASF_TRANSCRYPT_CONTEXT *pAsfTranscryptContext;
    DRM_DWORD cbWanted = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_ReadData );

    pState = &(pAsfContext->ParserState);
    pAsfTranscryptContext = &(pAsfContext->ASFTranscryptContext);
    pHeader = &(pAsfTranscryptContext->Header);

    DRMASSERT( NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode );
    fTranscrypting = TRUE == pAsfContext->fProtectedContentPlayback && NDT_ASFDELIVERY_MODE_MUXED == pAsfContext->ASFDeliveryMode;
    if( TRUE == fTranscrypting )
    {
        ChkDR( _ASFTranscryptNextBlock( f_pOEMContext,
                                        pAsfContext,
                                        f_pbData,
                                        f_pcbData,
                                        &fEndOfPackets ) );
        ChkBOOL( fEndOfPackets == FALSE, DRM_E_NOMORE );
    }
    else
    {
        /* Must output header object and data object header before any packets
         */
        if ( FALSE == pState->fHeaderDone )
        {
            /* First: read until the header is complete */
            DRM_DWORD cbHeaderBytesRemaining;

            if ( 0 == pHeader->dwSentDataOffset )
            {
                pState->iCurrentFileOffset = DRM_I64(0);
                ChkDR( _ASFSetupTranscryption( f_pOEMContext, pAsfContext ) );
                if ( ! Oem_File_SetFilePointer( pAsfContext->hInFile, 0, OEM_FILE_BEGIN, NULL ) )
                {
                    ChkDR( DRM_E_FAIL );
                }
            }

            cbHeaderBytesRemaining = DRM_UI64Low32(pState->qwHeaderSize) + DATA_OBJECT_SIZE - pHeader->dwSentDataOffset;
            if ( *f_pcbData >= cbHeaderBytesRemaining )
            {
                cbHeaderBytesRead = cbHeaderBytesRemaining;
            }
            else
            {
                cbHeaderBytesRead = *f_pcbData;
            }

            cbWanted = cbHeaderBytesRead;
            if ( !Oem_File_Read( pAsfContext->hInFile, (DRM_VOID*) f_pbData, cbHeaderBytesRead, &cbHeaderBytesRead ) )
            {
                ChkDR( DRM_E_FILEREADERROR );
            }
            fEndOfPackets |= ( cbHeaderBytesRead < cbWanted ) ? TRUE : FALSE;

            pHeader->dwSentDataOffset += cbHeaderBytesRead;
            if ( pHeader->dwSentDataOffset >= cbHeaderBytesRemaining )
            {
                ChkDR( _ASFSetHeaderDone( pAsfContext ) );
            }

            dwWanted -= cbHeaderBytesRead;
        }

        if ( TRUE == pState->fHeaderDone )
        {
            DRM_DWORD cbDataBytesRead = dwWanted;

            if ( pState->fSeekPositionSet )
            {
                _ASFSeekToTimePosition(pAsfContext);

                if ( ! Oem_File_SetFilePointer( pAsfContext->hInFile,
                                           DRM_UI64Low32( DRM_I2UI64(pState->iCurrentPacketOffset) ),
                                           OEM_FILE_BEGIN,
                                           NULL ) )
                {
                    ChkDR( DRM_E_FAIL );
                }

                pState->fSeekPositionSet = FALSE;
            }

#ifdef __TMS320C55X__
            if ( (1 & cbHeaderBytesRead) && (0 < cbDataBytesRead) )
            {
                /* If the header length was odd, then the first 8 bits of the next
                 * read must be spliced with the last 16-bit byte of the header
                 */
                DRM_BYTE  bTmp;
                DRM_DWORD cbTmp = 1;
                if ( !Oem_File_Read( pAsfContext->hInFile,
                                      (DRM_VOID*) &bTmp,
                                      cbTmp,
                                      &cbTmp ) )
                {
                    ChkDR( DRM_E_FILEREADERROR );
                }
                PUT_BYTE(f_pbData, cbHeaderBytesRead, bTmp >> 8);
                dwWanted -= cbTmp;
                cbDataBytesRead -= cbTmp;
                pState->fSeekPositionSet = FALSE;
            }

#endif
            cbWanted = cbDataBytesRead;
            if (!Oem_File_Read( pAsfContext->hInFile,
                                 (DRM_VOID*) ( f_pbData + __CB_DECL( cbHeaderBytesRead ) ),
                                 cbDataBytesRead,
                                 &cbDataBytesRead ) )
            {
                ChkDR( DRM_E_FILEREADERROR );
            }
            fEndOfPackets |= ( cbDataBytesRead < cbWanted ) ? TRUE : FALSE;

            dwWanted -= cbDataBytesRead;
        }

        *f_pcbData = *f_pcbData - dwWanted;

        /*
        ** Check EOF, which is affirmed if either of the above READs resulted in
        ** the number of bytes read being less than what we wanted
        */
        if ( TRUE == fEndOfPackets )
        {
            ChkDR( DRM_E_NOMORE );
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
