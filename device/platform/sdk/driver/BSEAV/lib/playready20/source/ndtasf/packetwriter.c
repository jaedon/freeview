/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmptrlist.h>
#include <ndtcontextsizes.h>
#include <ndtasf_internal.h>
#include <mux.h>
#include <asfsample.h>
#include <asfmediabuffer.h>
#include <packetwriter.h>

ENTER_PK_NAMESPACE_CODE;

#if DRM_SUPPORT_AUDIO_ONLY == 0

static DRM_DWORD GetASFVarFieldSize( 
    __in const DRM_BYTE bLenType 
)
{
    DRM_DWORD ret = 0;


    switch ( bLenType )
    {
    case 0:
        ret = 0;
        break;
    case 1:
        ret = SIZEOF_U8;
        break;
    case 2:
        ret = SIZEOF(DRM_WORD);
        break;
    case 3:
        ret = SIZEOF(DRM_DWORD);
        break;
    default:
        DRMASSERT( FALSE );
    }

    return ret;
}



static DRM_RESULT SaveVarField( 
    __in const                   DRM_BYTE          f_bLenType,
    __in const                   DRM_DWORD         f_dwValue,
    __out_bcount(f_cbDataBuffer) DRM_BYTE  * const f_pDataBuffer,
    __in const                   DRM_DWORD         f_cbDataBuffer,
    __inout                      DRM_DWORD * const f_pdwOffset 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE bTmp;
    DRM_WORD wTmp;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_SaveVarField );

    switch ( f_bLenType )
    {
    case 0:
        /* nothing to save */
        break;
    case 1:
        ChkBOOL( *f_pdwOffset + SIZEOF(DRM_BYTE) <= f_cbDataBuffer, DRM_E_BUFFERTOOSMALL );
        bTmp = (DRM_BYTE) f_dwValue;
        SaveBYTEOffset( f_pDataBuffer, bTmp, *f_pdwOffset );
        break;
    case 2:
        ChkBOOL( *f_pdwOffset + SIZEOF(DRM_WORD) <= f_cbDataBuffer, DRM_E_BUFFERTOOSMALL );
        wTmp = (DRM_WORD) f_dwValue;
        SaveWORDOffset( f_pDataBuffer, wTmp, *f_pdwOffset );
        break;
    case 3:
        ChkBOOL( *f_pdwOffset + SIZEOF(DRM_DWORD) <= f_cbDataBuffer, DRM_E_BUFFERTOOSMALL );
        SaveDWORDOffset( f_pDataBuffer, f_dwValue, *f_pdwOffset );
        break;
    default:
        DRMASSERT( FALSE );
        ChkDR( DRM_E_FAIL );
    }

 ErrorExit:  
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



static DRM_BOOL IsPacketInfoSet(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
)
{
    /*
     * cbPacketLength gets set to a nonzero value at SetPacketInfo()
     */
    return( pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength > 0 );
}



static DRM_DWORD GetPacketHeaderSize(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
)
{
    DRM_DWORD dwSize = 0;


    /*
     * Adds up the packet header size (doesn't include EC data!)
     * according to the various length-type values
     */
    
    dwSize += 2;        /* First two bytes */

    dwSize += GetASFVarFieldSize( pAsfPacketWriterContext->m_ASFPacketInfo.bPacketLenType );
    dwSize += GetASFVarFieldSize( pAsfPacketWriterContext->m_ASFPacketInfo.bSequenceLenType );
    dwSize += GetASFVarFieldSize( pAsfPacketWriterContext->m_ASFPacketInfo.bPadLenType );
    
    dwSize += 4;        /* send time */
    dwSize += 2;        /* duration */

    if ( pAsfPacketWriterContext->m_ASFPacketInfo.fMultiPayloads )
    {
        dwSize += 1;
    }

    return( dwSize );
}



/****************************************************************************/
static DRM_DWORD GetBasicPayloadHeaderSize(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
)
{
    DRM_DWORD dwSize = 0;

    DRMASSERT( 1 == pAsfPacketWriterContext->m_ASFPacketInfo.bStreamIDLenType );
    dwSize += 1;  /* Stream ID (should be a DRM_BYTE) */

    dwSize += GetASFVarFieldSize( pAsfPacketWriterContext->m_ASFPacketInfo.bObjectIDLenType );
    dwSize += GetASFVarFieldSize( pAsfPacketWriterContext->m_ASFPacketInfo.bOffsetLenType );
    dwSize += GetASFVarFieldSize( pAsfPacketWriterContext->m_ASFPacketInfo.bReplicatedDataLenType );

    return( dwSize );
}



static DRM_RESULT InitializePacketInfoForWrite(
    __out ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
);

static DRM_RESULT GetCurrentPacketSnapshot(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __out      PACKET_SNAPSHOT           * const pSnapshot
);

static DRM_RESULT RestorePacket(
    __inout ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const PACKET_SNAPSHOT        * const pSnapshot
);

static DRM_RESULT InternalAddPayloadInfo(
    __inout    ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const ASF_PAYLOAD_PARSER_INFO   * const pPayloadInfo,
    __in const ASF_SAMPLE                * const pMediaObjectWithAttributes,
    __in const DRM_BOOL                          fOkayToCompress
);

static DRM_RESULT CalcPayloadSize(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const DRM_DWORD                         dwPayloadNumber
);



/****************************************************************************/
static DRM_RESULT GetReplicatedData(
    __in const ASF_SAMPLE * const pMediaObjectWithAttributes,
    __out      DRM_BYTE   * const pcbReplicatedDataSpace,
    __out_bcount(pMediaObjectWithAttributes->bRepData) DRM_BYTE * const pbReplicatedData
)
{
    /* The replicated data now comes from the sample that was input into the mux. */
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_GetReplicatedData );

    ChkArg( pbReplicatedData       != NULL );
    ChkArg( pcbReplicatedDataSpace != NULL );

    *pcbReplicatedDataSpace = pMediaObjectWithAttributes->bRepData;
    MEMCPY( pbReplicatedData, pMediaObjectWithAttributes->rgbRepData, *pcbReplicatedDataSpace );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_DWORD GetNonPaddingSpace(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
)
{
    DRM_DWORD dwResult;


    dwResult = pAsfPacketWriterContext->m_ASFPacketInfo.bECLen + GetPacketHeaderSize( pAsfPacketWriterContext );

    /*
     * This function assumes that all payload infos have correct sizes
     * but does not assume that m_ASFPacketInfo has an accurate packet size
     */

    DRMASSERT( pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize == pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads );

    if ( 0 < pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize )
    {
        /*
         * We can use the final payload's cbPacketOffset and cbTotalSize to "cheat"
         */
        ASF_PAYLOAD_PARSER_INFO *pFinalPayload = NULL;
        if ( DRM_SUCCEEDED( DRM_PtrList_GetTail( &pAsfPacketWriterContext->m_ASFPayloadInfos, (DRM_VOID **) &pFinalPayload ) ) )
        {
            dwResult = pFinalPayload->wPacketOffset + pFinalPayload->wTotalSize;
        }
    }

    return dwResult;
}



static DRM_RESULT OnHeaderSizeChange(
    __inout ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
);

static DRM_RESULT WritePacketHeader(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __out      ASF_SAMPLE                * const pPacket
    );

static DRM_RESULT WritePayloadHeader(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const DRM_DWORD                         dwPayload,
    __out      ASF_SAMPLE                * const pPacket
);

static DRM_RESULT WritePayloadData(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const DRM_DWORD                         dwPayload,
    __out      ASF_SAMPLE                * const pPacket
);

static DRM_RESULT WritePadding(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __out      ASF_SAMPLE                * const pPacket
);

static DRM_RESULT WritePayloadDataWithoutEncrypt(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const ASF_PAYLOAD_PARSER_INFO   * const pPayload,
    __out      ASF_SAMPLE                * const pPacket
);

static DRM_DWORD GetPacketSizeLimit( 
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
)
{
    /*
     * cbPacketLength will always give us the fixed size
     */
    return( pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength );
}



/****************************************************************************
 **
 ** Function:    ASFPacketWriter_Initialize
 **
 ** Synopsis:    Initializes the packet writer.  The context initialized with
 **              this function is then used in subsequent ASFPacketWriter 
 **              calls.
 **
 ** Arguments:   [pAsfPacketWriterContext] -- packet writer context
 **              [dwFixedPacketSize]       -- packet size the writer will generate
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 ****************************************************************************/
DRM_RESULT ASFPacketWriter_Initialize(
    __out      ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const DRM_DWORD                         dwFixedPacketSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_Initialize );

    ZEROMEM( &pAsfPacketWriterContext->m_ASFPayloadInfos, SIZEOF( PTR_LIST ) );
    ZEROMEM( &pAsfPacketWriterContext->m_PayloadDataBuffers, SIZEOF( PTR_LIST ) );

    ChkDR( DRM_PtrList_Initialize( &pAsfPacketWriterContext->m_ASFPayloadInfos, 128 ) );
    ChkDR( DRM_PtrList_Initialize( &pAsfPacketWriterContext->m_PayloadDataBuffers, 128 ) );

    ChkDR( ASFPacketWriter_Clear( pAsfPacketWriterContext )  );

    pAsfPacketWriterContext->m_cbFixedPacketSize = dwFixedPacketSize;

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************
 **
 ** Function:    ASFPacketWriter_Uninitialize
 **
 ** Synopsis:    Uninitializes the packet writer.
 **
 ** Arguments:   [pAsfPacketWriterContext] -- packet writer context
 **
 ** Returns:     N/A
 **
 ****************************************************************************/
DRM_VOID ASFPacketWriter_Uninitialize(
    __out ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
)
{
    if ( NULL != pAsfPacketWriterContext )
    {
        (DRM_VOID) ASFPacketWriter_Clear( pAsfPacketWriterContext );
    }

    DRM_PtrList_Uninitialize( &pAsfPacketWriterContext->m_ASFPayloadInfos );
    DRM_PtrList_Uninitialize( &pAsfPacketWriterContext->m_PayloadDataBuffers );

    return;
}



/****************************************************************************
 **
 ** Function:    ASFPacketWriter_SetPacketInfo
 **
 ** Synopsis:    Sets the packet info that the packet writer is currently
 **              working on.
 **
 ** Arguments:   [pAsfPacketWriterContext] -- packet writer context
 **              [pPacketInfo]             -- packet information
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 ****************************************************************************/
DRM_RESULT ASFPacketWriter_SetPacketInfo(
    __inout    ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const ASF_PACKET_PARSER_INFO * const pPacketInfo
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fIsPacketInfoSet;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_SetPacketInfo );

    ChkArg( pPacketInfo );

    /*
     * Validate the contents of pPacketInfo
     */

    fIsPacketInfoSet = IsPacketInfoSet( pAsfPacketWriterContext );

    if ( fIsPacketInfoSet )
    {
        /*
         * Changing the packet length once it's already been set is off limits.
         * To do this requires that you Clear() out the packet and start over.
         */
        if ( pPacketInfo->cbPacketLength != pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength )
        {
            return( DRM_E_ASF_NOT_ACCEPTING );
        }
    }

    pAsfPacketWriterContext->m_ASFPacketInfo.dwSCR = pPacketInfo->dwSCR;
    pAsfPacketWriterContext->m_ASFPacketInfo.wDuration = pPacketInfo->wDuration;
    /* Ignore cPayloads; we figure that out ourselves */
    pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength = pPacketInfo->cbPacketLength;

    if ( !fIsPacketInfoSet )
    {
        ChkDR( InitializePacketInfoForWrite( pAsfPacketWriterContext ) );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************
 **
 ** Function:    ASFPacketWriter_GetPacketInfo
 **
 ** Synopsis:    Gets the packet info that the packet writer is currently
 **              working on.
 **
 ** Arguments:   [pAsfPacketWriterContext] -- packet writer context
 **              [pPacketInfo]             -- packet information
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 ****************************************************************************/
DRM_RESULT ASFPacketWriter_GetPacketInfo(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __out      ASF_PACKET_PARSER_INFO    * const pPacketInfo 
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_GetPacketInfo );

    ChkArg( pPacketInfo );

    /*
     * m_ASFPacketInfo can be assumed to be valid for the last ASF_SAMPLE passed to us
     */
    MEMCPY( pPacketInfo, &pAsfPacketWriterContext->m_ASFPacketInfo, SIZEOF( ASF_PACKET_PARSER_INFO ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************
 **
 ** Function:    ASFPacketWriter_GetDataSpaceAvailable
 **
 ** Synopsis:    Gets the remaining space available in the packet given an
 **              input sample.
 **
 ** Arguments:   [pAsfPacketWriterContext]    -- packet writer context
 **              [wStreamNumber]              -- stream number of sample
 **              [bObjectID]                  -- object id of sample
 **              [pMediaObjectWithAttributes] -- sample
 **              [pcbSpaceAvailable]          -- remaining space
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 ****************************************************************************/
DRM_RESULT ASFPacketWriter_GetDataSpaceAvailable(
    __inout    ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const DRM_WORD                          wStreamNumber,
    __in const DRM_BYTE                          bObjectID,
    __in const ASF_SAMPLE                * const pMediaObjectWithAttributes,
    __out      DRM_DWORD                 * const pcbSpaceAvailable 
)
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_RESULT dr2 = DRM_SUCCESS;
    PACKET_SNAPSHOT PacketSnapshot;
    ASF_PAYLOAD_PARSER_INFO BogusPayloadInfo;
    DRM_DWORD cbNonPaddingSpace;
    DRM_DWORD cbPacketLimit;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_GetDataSpaceAvailable );

    ChkArg( pcbSpaceAvailable );
    *pcbSpaceAvailable = 0;

    /*
     * Store the current state so that it can be restored at the end
     */
    ChkDR( GetCurrentPacketSnapshot( pAsfPacketWriterContext, &PacketSnapshot ) );

    /*
     * Concoct a bogus payload info so that we can pretend to add it
     * and see what effect it has on our current size
     */
    ZEROMEM( &BogusPayloadInfo, SIZEOF( BogusPayloadInfo ) );

    BogusPayloadInfo.bStreamId = (DRM_BYTE) wStreamNumber;
    BogusPayloadInfo.bObjectId = (DRM_BYTE) bObjectID;

    /*
     * This will pretend to add this payload
     */
    dr = InternalAddPayloadInfo( pAsfPacketWriterContext, &BogusPayloadInfo, pMediaObjectWithAttributes, FALSE );
    if ( DRM_FAILED( dr ) )
    {
        *pcbSpaceAvailable = 0;
            
        ChkDR( DRM_SUCCESS );
        goto ErrorExit;
    }

    /*
     * The amount of space that is left is the limit on this packet's length
     * minus the amount of non-padding space already occupied
     */
    cbNonPaddingSpace = GetNonPaddingSpace( pAsfPacketWriterContext );
    cbPacketLimit = GetPacketSizeLimit( pAsfPacketWriterContext );

    DRMASSERT( cbNonPaddingSpace <= cbPacketLimit );

    if ( cbNonPaddingSpace <= cbPacketLimit )
    {
        *pcbSpaceAvailable = cbPacketLimit - cbNonPaddingSpace;
    }
    else
    {
        *pcbSpaceAvailable = 0;
    }

 ErrorExit:
    /*
     * Under all circumstances, restore to our original state.
     */
    dr2 = RestorePacket( pAsfPacketWriterContext, &PacketSnapshot );

    if ( DRM_SUCCEEDED( dr ) )
    {
        dr = dr2;
    }
    
    DRM_PROFILING_LEAVE_SCOPE;   
    return dr;
}



/****************************************************************************
 **
 ** Function:    ASFPacketWriter_AddPayload
 **
 ** Synopsis:    Adds a payload to a packet.
 **
 ** Arguments:   [pAsfPacketWriterContext]      -- packet writer context
 **              [pPayloadInfo]                 -- payload info to add
 **              [pMFSample]                    -- sample to add
 **              [fSampleIsCompleteMediaObject] -- partial or complete object
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 ****************************************************************************/
DRM_RESULT ASFPacketWriter_AddPayload(
    __inout    ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const ASF_PAYLOAD_PARSER_INFO   * const pPayloadInfo,
    __in const ASF_SAMPLE                * const pMFSample,
    __in const DRM_BOOL                          fSampleIsCompleteMediaObject 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PACKET_SNAPSHOT PacketSnapshot;
    ASF_MEDIA_BUFFER *pMediaBuffer = NULL;
    DRM_DWORD cbMFSampleRequired;
    ASF_PAYLOAD_PARSER_INFO *pPayloadParseInfo;
    DRM_DWORD cMediaBuffers;
    DRM_DWORD cbObjectProcessed = 0;
    DRM_DWORD cbPayloadDataProcessed = 0;
    DRM_DWORD i = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_AddPayload );

    ChkArg( pPayloadInfo );
    ChkArg( pMFSample );

    if ( !IsPacketInfoSet( pAsfPacketWriterContext ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    if ( pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads >= MAX_PACKET_PAYLOADS )
    {
        return( DRM_E_ASF_TOO_MANY_PAYLOADS );
    }

    ZEROMEM( &PacketSnapshot, SIZEOF( PacketSnapshot ) );

    /*
     * Validate pMFSample
     */

    if ( (pPayloadInfo->dwObjectOffset + pPayloadInfo->wTotalDataBytes) > pPayloadInfo->dwObjectSize )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    cbMFSampleRequired = fSampleIsCompleteMediaObject ? 
        pPayloadInfo->dwObjectSize : pPayloadInfo->wTotalDataBytes;
    if ( pPayloadInfo->wTotalDataBytes > cbMFSampleRequired )
    {
        ChkDR( DRM_E_INVALIDARG );
    }
       
    /*
     * Try to add the payload, but roll back if it doesn't fit
     */
    ChkDR( GetCurrentPacketSnapshot( pAsfPacketWriterContext, &PacketSnapshot ) );

    /*
     * Temporarily disable compressed payloads
     */
    dr = InternalAddPayloadInfo( pAsfPacketWriterContext, pPayloadInfo, pMFSample, FALSE );
    if ( DRM_E_ASF_INVALID_DATA == dr )
    {
        /*
         * Reject this payload because there's too much stuff to add.
         * We have to roll back to our original state
         */
        ChkDR( RestorePacket( pAsfPacketWriterContext, &PacketSnapshot ) );

        dr = DRM_E_ASF_INVALID_DATA;
    }
    ChkDR( dr );

    /*
     * The payload info has been successfully added.
     * Now hang onto the buffers
     */
    DRMASSERT( pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads > 0 );
    if ( 0 == pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads )
    {
        ChkDR( DRM_E_FAIL );
    }

    ChkDR( DRM_PtrList_GetByIndex( &pAsfPacketWriterContext->m_ASFPayloadInfos,
                                   pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads - 1,
                                   (DRM_VOID **) &pPayloadParseInfo ) );

    pPayloadParseInfo->dwPayloadDataStartBuffer = pPayloadParseInfo->cPayloadDataBuffers = 0;

    ChkDR( ASFSample_GetBufferCount( pMFSample, &cMediaBuffers ) );

    if ( !fSampleIsCompleteMediaObject )
    {
        /*
         * This IMFSample starts with our current fragment
         */
        cbObjectProcessed = pPayloadParseInfo->dwObjectOffset;
    }

    for ( i = 0; (i < cMediaBuffers) && (cbPayloadDataProcessed < pPayloadParseInfo->wTotalDataBytes); i++ )
    {
        DRM_DWORD cbBufferLength = 0;
        DRM_DWORD cbOffsetToPush = 0;
        DRM_DWORD cbLengthToPush = 0;

        ChkDR( ASFSample_GetBufferByIndex( pMFSample, i, &pMediaBuffer ) );
        ChkFAIL( pMediaBuffer != NULL );
        cbBufferLength = pMediaBuffer->blobBuffer.cbBlob;

        /*
         * Is there any payload data in this buffer?
         */
        if ( (cbObjectProcessed + cbBufferLength) > pPayloadParseInfo->dwObjectOffset )
        {
            if ( cbObjectProcessed < pPayloadParseInfo->dwObjectOffset )
            {
                /*
                 * The start is somewhere within this buffer
                 */
                cbOffsetToPush = pPayloadParseInfo->dwObjectOffset - cbObjectProcessed;

                DRMASSERT( cbOffsetToPush < cbBufferLength ); 

                cbBufferLength -= cbOffsetToPush;
            }
            else
            {
                cbOffsetToPush = 0;
            }

            if ( (pPayloadParseInfo->wTotalDataBytes - cbPayloadDataProcessed) < cbBufferLength )
            {
                /*
                 * The payload data ends before the end of this buffer
                 */
                cbLengthToPush = pPayloadParseInfo->wTotalDataBytes - cbPayloadDataProcessed;
            }
            else
            {
                cbLengthToPush = cbBufferLength;
            }

            ChkDR( ASFMediaBuffer_AddRef( pMediaBuffer ) );
            pMediaBuffer->cbCurrentOffset = cbOffsetToPush;
            pMediaBuffer->cbCurrentLength = cbLengthToPush;
            ChkDR( DRM_PtrList_AddTail( &pAsfPacketWriterContext->m_PayloadDataBuffers, pMediaBuffer ) );

            /*
             * Account for this buffer
             */
            cbPayloadDataProcessed += cbLengthToPush;

            if ( 0 == pPayloadParseInfo->cPayloadDataBuffers )
            {
                pPayloadParseInfo->dwPayloadDataStartBuffer = pAsfPacketWriterContext->m_PayloadDataBuffers.dwCurrentSize - 1;
            }

            pPayloadParseInfo->cPayloadDataBuffers++;
        }

        cbObjectProcessed += cbBufferLength + cbOffsetToPush;

        ASFMediaBuffer_Release( pMediaBuffer );
        pMediaBuffer = NULL;
    }

 ErrorExit:
    if ( NULL != pMediaBuffer )
    {
        ASFMediaBuffer_Release( pMediaBuffer );
        pMediaBuffer = NULL;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************
 **
 ** Function:    ASFPacketWriter_WritePacket
 **
 ** Synopsis:    Outputs the packet.
 **
 ** Arguments:   [pAsfPacketWriterContext] -- packet writer context
 **              [ppASFPacket]             -- packet
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 ****************************************************************************/
DRM_RESULT ASFPacketWriter_WritePacket(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __out      ASF_SAMPLE                      **ppASFPacket
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_SAMPLE *pNewSample = NULL;
    DRM_WORD i;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_WritePacket );

    if ( !IsPacketInfoSet( pAsfPacketWriterContext ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    pNewSample = (ASF_SAMPLE *) Oem_MemAlloc( SIZEOF( ASF_SAMPLE ) );
    ChkMem( pNewSample );
    ChkDR( ASFSample_Initialize( pNewSample ) );

    /*
     * Packet header
     */
    ChkDR( WritePacketHeader( pAsfPacketWriterContext, pNewSample ) );

    /*
     * Payloads
     */
    DRMASSERT( pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads == pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize );
    if ( pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads != pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize )
    {
        ChkDR( DRM_E_FAIL );
    }

    for ( i = 0; i < pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads; i++ )
    {
        ChkDR( WritePayloadHeader( pAsfPacketWriterContext, i, pNewSample ) );
        ChkDR( WritePayloadData( pAsfPacketWriterContext, i, pNewSample ) );
    }

    /*
     * Padding
     */
    ChkDR( WritePadding( pAsfPacketWriterContext, pNewSample ) );

    *ppASFPacket = pNewSample;

 ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        if ( NULL != pNewSample )
        {
            ASFSample_Release( pNewSample );
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************
 **
 ** Function:    ASFPacketWriter_Clear
 **
 ** Synopsis:    Clears out any packet information the context is holding on to.
 **
 ** Arguments:   [pAsfPacketWriterContext] -- packet writer context
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 ****************************************************************************/
DRM_RESULT ASFPacketWriter_Clear(
    __out ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFPacketWriter_Clear );

    ZEROMEM( &pAsfPacketWriterContext->m_ASFPacketInfo, SIZEOF( pAsfPacketWriterContext->m_ASFPacketInfo ) );

    while ( pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize > 0 )
    {
        DRM_VOID *pFreeMe = NULL;
        (DRM_VOID) DRM_PtrList_RemoveHead( &pAsfPacketWriterContext->m_ASFPayloadInfos, (DRM_VOID **) &pFreeMe );
        SAFE_OEM_FREE( pFreeMe );
    }

    while ( pAsfPacketWriterContext->m_PayloadDataBuffers.dwCurrentSize > 0 )
    {
        ASF_MEDIA_BUFFER *pReleaseMe = NULL;

        (DRM_VOID) DRM_PtrList_RemoveHead( &pAsfPacketWriterContext->m_PayloadDataBuffers, (DRM_VOID **) &pReleaseMe );
        ASFMediaBuffer_Release( pReleaseMe );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT InitializePacketInfoForWrite(
    __out ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbPacketHeader;
    DRM_BOOL fIsFixedSize;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_InitializePacketInfoForWrite );

    /* The rest of this is taken from InitializePacketInfoForWrite */

    pAsfPacketWriterContext->m_ASFPacketInfo.fEccPresent = TRUE;
    pAsfPacketWriterContext->m_ASFPacketInfo.bECLen = 3;

#ifdef SUPPORT_VARIABLE_PACKET_SIZE
    if ( ASF_VARIABLE_SIZE_PACKET != pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength )
    {
        pAsfPacketWriterContext->m_ASFPacketInfo.bPacketLenType = 0;
    }
    else
    {
        /*
         * Keep this capped at 64KB.  
         */
        pAsfPacketWriterContext->m_ASFPacketInfo.bPacketLenType = 0x02;
    }
#else
        pAsfPacketWriterContext->m_ASFPacketInfo.bPacketLenType = 0;
#endif

    /*
     * We'll adjust this down later if the padding length fits in a BYTE
     */
    pAsfPacketWriterContext->m_ASFPacketInfo.bPadLenType = 0x02;

    pAsfPacketWriterContext->m_ASFPacketInfo.bSequenceLenType = 0;

    pAsfPacketWriterContext->m_ASFPacketInfo.fMultiPayloads = FALSE; /* adjust later if we get >1 payload */

    pAsfPacketWriterContext->m_ASFPacketInfo.bOffsetLenType = 0x03;

    pAsfPacketWriterContext->m_ASFPacketInfo.bReplicatedDataLenType = 0x01;
    pAsfPacketWriterContext->m_ASFPacketInfo.bReplicatedDataLenBytes = 1;

    pAsfPacketWriterContext->m_ASFPacketInfo.bStreamIDLenType = 0x01;
    pAsfPacketWriterContext->m_ASFPacketInfo.bObjectIDLenType = 0x01;

    pAsfPacketWriterContext->m_ASFPacketInfo.dwSequenceNum = 0;


    cbPacketHeader = pAsfPacketWriterContext->m_ASFPacketInfo.bECLen + GetPacketHeaderSize( pAsfPacketWriterContext );
    if ( cbPacketHeader > pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength )
    {
        ChkDR( DRM_E_ASF_INVALID_DATA );
    }

    /*
     * This needs to be updated every time a payload is added for variable-length packets
     */
#ifdef SUPPORT_VARIABLE_PACKET_SIZE
    fIsFixedSize = (ASF_VARIABLE_SIZE_PACKET != pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength);
#else
    fIsFixedSize = TRUE;
#endif

    if ( fIsFixedSize )
    {
        pAsfPacketWriterContext->m_ASFPacketInfo.dwExplicitPacketLength = 0;
    }
    else
    {
        pAsfPacketWriterContext->m_ASFPacketInfo.dwExplicitPacketLength = GetNonPaddingSpace( pAsfPacketWriterContext );
        
        /*
         * cbPacketLength should be changed to a meaningful number that 
         * we can return to the user
         */
        pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength = pAsfPacketWriterContext->m_ASFPacketInfo.dwExplicitPacketLength;
    }

    /*
     * This needs to be updated every time a payload is added for fixed-length packets
     */
    if ( fIsFixedSize )
    {
        /*
         * Initially, everything after this packet header is padding
         */
        pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding = pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength - cbPacketHeader;
    }
    else
    {
        pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding = 0;
        pAsfPacketWriterContext->m_ASFPacketInfo.bPadLenType = 0;
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT GetCurrentPacketSnapshot(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __out      PACKET_SNAPSHOT           * const pSnapshot
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PAYLOAD_PARSER_INFO *pFinalPayload;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_GetCurrentPacketSnapshot );

    ChkArg( pSnapshot );

    /*
     * Pick up the packetinfo
     */
    pSnapshot->PacketParseInfo = pAsfPacketWriterContext->m_ASFPacketInfo;

    /*
     * If the final N payloads are compressed, remember some info about them
     */
    pSnapshot->cFinalCompressedSubPayloads = 0;
    pSnapshot->cbFinalCompressedPayloadTotal = 0;

    if ( 0 == pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads )
    {
        goto ErrorExit;
    }

    DRMASSERT( pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize == pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads );
    ChkDR( DRM_PtrList_GetTail( &pAsfPacketWriterContext->m_ASFPayloadInfos, (DRM_VOID **) &pFinalPayload ) );

    pSnapshot->cbFinalPayloadTotalSize = pFinalPayload->wTotalSize;

    if ( !pFinalPayload->fIsCompressedPayload )
    {
        /*
         * The final payload is not compressed
         */
        goto ErrorExit;
    }

 ErrorExit:

    if ( DRM_FAILED( dr ) )
    {
        ZEROMEM( pSnapshot, SIZEOF( PACKET_SNAPSHOT ) );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT RestorePacket(
    __inout ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const PACKET_SNAPSHOT        * const pSnapshot
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PAYLOAD_PARSER_INFO *pFinalPayloadInfo;
    DRM_WORD cRestoredPayloads;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_RestorePacket );

    ChkArg(pSnapshot );
    ChkArg( pSnapshot->PacketParseInfo.dwPayloads >= pSnapshot->cFinalCompressedSubPayloads );

    /*
     * If there are any extra payloads, cut them off
     */
    cRestoredPayloads = (DRM_WORD) pSnapshot->PacketParseInfo.dwPayloads;

    while ( pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize > cRestoredPayloads )
    {
        DRM_DWORD dwPayloadToRemove = pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize - 1;
        ASF_PAYLOAD_PARSER_INFO *pPayloadInfo;
        
        ChkDR( DRM_PtrList_GetByIndex( &pAsfPacketWriterContext->m_ASFPayloadInfos, dwPayloadToRemove, (DRM_VOID **) &pPayloadInfo ) );

        ChkDR( DRM_PtrList_RemoveByIndex( &pAsfPacketWriterContext->m_ASFPayloadInfos, dwPayloadToRemove, (DRM_VOID **) &pPayloadInfo ) );
        SAFE_OEM_FREE( pPayloadInfo );
    }

    /*
     * Restore the PACKET_PARSE_INFO
     */
    pAsfPacketWriterContext->m_ASFPacketInfo = pSnapshot->PacketParseInfo;

    /*
     * Restore the state of the final compressed payload, if it exists
     */
    if ( 0 == pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads )
    {
        goto ErrorExit;
    }

    ChkDR( DRM_PtrList_GetByIndex( &pAsfPacketWriterContext->m_ASFPayloadInfos, pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads - 1, (DRM_VOID **) &pFinalPayloadInfo ) );

    /*
     * Restore the old total size (might have increased if we went from single payload
     * to multi payload and now we're going back to single)
     */
    pFinalPayloadInfo->wTotalSize = (DRM_WORD) pSnapshot->cbFinalPayloadTotalSize;

    /*
     * Since the packet header might have undergone some change, let's 
     * now recalculate the cbPacketOffset of all payloads
     */
    ChkDR( OnHeaderSizeChange( pAsfPacketWriterContext ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT InternalAddPayloadInfo(
    __inout    ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const ASF_PAYLOAD_PARSER_INFO   * const pPayloadInfo,
    __in const ASF_SAMPLE                * const pMediaObjectWithAttributes,
    __in const DRM_BOOL                          fOkayToCompress
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PAYLOAD_PARSER_INFO *pPayloadParseInfo = NULL;
    ASF_PAYLOAD_PARSER_INFO *pPreviousPayloadInfo = NULL;
    DRM_DWORD dwPayloadNumber = 0;
    DRM_DWORD cbNonPaddingSpace;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_InternalAddPayloadInfo );

    ChkArg( pPayloadInfo );

    /*
     * We add this payload and the amount of data specified in pPayloadInfo
     * without regard for how big this makes the packet
     */
    pPayloadParseInfo = (ASF_PAYLOAD_PARSER_INFO *) Oem_MemAlloc( SIZEOF ( ASF_PAYLOAD_PARSER_INFO ) );
    ChkMem ( pPayloadParseInfo );

    ZEROMEM( pPayloadParseInfo, SIZEOF( ASF_PAYLOAD_PARSER_INFO ) );

    MEMCPY( pPayloadParseInfo, pPayloadInfo, SIZEOF( ASF_PAYLOAD_PARSER_INFO ) );

    /*
     * There are a few fields of the ASF_PAYLOAD_PARSER_INFO set of fields that we determine
     *
     * pPayloadParseInfo->cbTotalSize gets filled in by CalcPayloadSize
     */

    ChkDR( GetReplicatedData( pMediaObjectWithAttributes,
                              &(pPayloadParseInfo->bRepData),
                              pPayloadParseInfo->rgbRepData ) );

    if ( pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads > 0 )
    {
        ChkDR( DRM_PtrList_GetByIndex( &pAsfPacketWriterContext->m_ASFPayloadInfos, 
                                       pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads - 1,
                                       (DRM_VOID **) &pPreviousPayloadInfo ) );
    }

    if ( pPreviousPayloadInfo && !pAsfPacketWriterContext->m_ASFPacketInfo.fMultiPayloads )
    {
        /*
         * There are now more than one payload.  This means that
         * the previous payload's space will be more than if it
         * had been the only payload here
         */
        pAsfPacketWriterContext->m_ASFPacketInfo.fMultiPayloads = TRUE;
        pAsfPacketWriterContext->m_ASFPacketInfo.bPayLenType = 0x02;
        pAsfPacketWriterContext->m_ASFPacketInfo.bPayBytes = (DRM_BYTE) GetASFVarFieldSize( pAsfPacketWriterContext->m_ASFPacketInfo.bPayLenType );
        
        ChkDR( CalcPayloadSize( pAsfPacketWriterContext, 0 ) );
        
        ChkDR( OnHeaderSizeChange( pAsfPacketWriterContext ) );
    }

    /*
     * At this point, all of the existing payloads have the correct size numbers,
     * so we can correctly calculate the payload's actual occupied space
     */
    pPayloadParseInfo->wPacketOffset = (DRM_WORD) GetNonPaddingSpace( pAsfPacketWriterContext );

    /*
     * Add the payload
     */
    ChkDR( DRM_PtrList_AddTail( &pAsfPacketWriterContext->m_ASFPayloadInfos, pPayloadParseInfo ) );
    pPayloadParseInfo = NULL;
    dwPayloadNumber = pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize - 1;

    /*
     * We're adding a payload...
     */
    pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads++;

    /*
     * Get the payload's size numbers right
     */
    ChkDR( CalcPayloadSize( pAsfPacketWriterContext, dwPayloadNumber ) );

    /*
     * Get the packet's size numbers right
     */
    cbNonPaddingSpace = GetNonPaddingSpace( pAsfPacketWriterContext );

    if ( pAsfPacketWriterContext->m_ASFPacketInfo.dwExplicitPacketLength > 0 )
    {
        /*
         * Variable-length packet
         */
        pAsfPacketWriterContext->m_ASFPacketInfo.dwExplicitPacketLength = pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength = cbNonPaddingSpace;
    }
    else
    {
        /*
         * Fixed-length packet: The payload may not fit
         */
        DRM_DWORD cbCurrentNonPadding = cbNonPaddingSpace;

        if ( cbCurrentNonPadding > pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength )
        {
            pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding = 0;

            /*
             * We'll fail later on
             */
        }
        else
        {
            pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding = pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength - cbCurrentNonPadding;

            if ( fOkayToCompress && (pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding < 255) && (0x01 != pAsfPacketWriterContext->m_ASFPacketInfo.bPadLenType) ) 
            {
                pAsfPacketWriterContext->m_ASFPacketInfo.bPadLenType = 0x1;  /* padding size fits in a DRM_BYTE */

                /*
                 * This changes all the header size
                 */
                ChkDR( OnHeaderSizeChange( pAsfPacketWriterContext ) );
            }
        }
    }

    if ( cbNonPaddingSpace > GetPacketSizeLimit( pAsfPacketWriterContext ) )
    {
        ChkDR( DRM_E_ASF_INVALID_DATA );
    }

 ErrorExit:
    SAFE_OEM_FREE( pPayloadParseInfo );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT CalcPayloadSize(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const DRM_DWORD                         dwPayloadNumber
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PAYLOAD_PARSER_INFO *pPayloadInfo;
    DRM_DWORD cbSize = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_CalcPayloadSize );

    /*
     * Precondition: All payloads up to but not including dwPayloadNumber
     * have the right size numbers
     */
    if ( (dwPayloadNumber >= pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize) ||
         (dwPayloadNumber >= pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads) )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    ChkDR( DRM_PtrList_GetByIndex( &pAsfPacketWriterContext->m_ASFPayloadInfos, dwPayloadNumber, (DRM_VOID **) &pPayloadInfo) );

    /*
     * Payloads have the following structure:
     * 1. Basic payload data
     * 2. Replicated data
     * 3. If there are multiple payloads, payload length
     * 4. Payload data (in the compressed payload case, this is all the sub-payloads)
     */
    if ( !pPayloadInfo->fIsCompressedPayload )
    {
        cbSize += GetBasicPayloadHeaderSize( pAsfPacketWriterContext );

        cbSize += pPayloadInfo->bRepData;

        if ( pAsfPacketWriterContext->m_ASFPacketInfo.fMultiPayloads )
        {
            cbSize += pAsfPacketWriterContext->m_ASFPacketInfo.bPayBytes;
        }
    }

    cbSize += pPayloadInfo->wTotalDataBytes;

    /*
     * Now we can set cbTotalSize
     */
    pPayloadInfo->wTotalSize = (DRM_WORD) cbSize;

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT OnHeaderSizeChange(
    __inout ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbPacketOffset;
    DRM_DWORD i;
    DRM_DWORD cbCurrentNonPadding;


    /*
     * When the header size changes, all we need to do is update 
     * all payloads' cbPacketOffset fields
     */
    cbPacketOffset = pAsfPacketWriterContext->m_ASFPacketInfo.bECLen + GetPacketHeaderSize( pAsfPacketWriterContext );
    for ( i = 0; i < pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize; i++ )
    {
        ASF_PAYLOAD_PARSER_INFO *pPayloadInfo;

        ChkDR( DRM_PtrList_GetByIndex( &pAsfPacketWriterContext->m_ASFPayloadInfos, i, (DRM_VOID **) &pPayloadInfo ) );

        pPayloadInfo->wPacketOffset = (DRM_WORD) cbPacketOffset;

        cbPacketOffset += pPayloadInfo->wTotalSize;
    }

    /*
     * Update the padding if fixed-size and the explicit packet length if variable-size
     */
    cbCurrentNonPadding = GetNonPaddingSpace( pAsfPacketWriterContext );

    if ( pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength >= cbCurrentNonPadding )
    {
        pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding = pAsfPacketWriterContext->m_ASFPacketInfo.cbPacketLength - cbCurrentNonPadding;
    }
    else
    {
        /*
         * The packet is overstuffed and invalid
         */
        pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding = 0;
        ChkDR( DRM_E_ASF_INVALID_DATA );
    }

 ErrorExit:
    return( DRM_SUCCESS );
}



/****************************************************************************/
static DRM_RESULT WritePacketHeader(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __out      ASF_SAMPLE                * const pPacket
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbPacketHeader = 0;
    DRM_BYTE   bLengthFlags = 0;
    DRM_BYTE   bPropertyFlags = 0;
    DRM_DWORD  cMediaBuffers = 0;
    ASF_MEDIA_BUFFER *pPacketHeaderBuffer = NULL;
    DRM_BYTEBLOB blobBuffer = { NULL, 0 };
    DRM_DWORD dwOffset = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePacketHeader );

    ChkArg( pPacket );
    ChkDR( ASFSample_GetBufferCount( pPacket, &cMediaBuffers ) );
    if ( cMediaBuffers > 0 )
    {
        ChkDR( DRM_E_FAIL );
    }

    cbPacketHeader = GetPacketHeaderSize( pAsfPacketWriterContext ) + pAsfPacketWriterContext->m_ASFPacketInfo.bECLen;
    blobBuffer.cbBlob = cbPacketHeader;
    blobBuffer.pbBlob = (DRM_BYTE *) Oem_MemAlloc( blobBuffer.cbBlob );
    ChkMem( blobBuffer.pbBlob );

    pPacketHeaderBuffer = (ASF_MEDIA_BUFFER *) Oem_MemAlloc( SIZEOF( ASF_MEDIA_BUFFER ) );
    ChkMem( pPacketHeaderBuffer );
    ChkDR( ASFMediaBuffer_Initialize( pPacketHeaderBuffer, blobBuffer, 0 ) );
    blobBuffer.pbBlob = NULL;

    if ( pAsfPacketWriterContext->m_ASFPacketInfo.fEccPresent )
    {
        DRM_WORD wTmp = 0;
        /*
         * The first BYTE indicates whether this even is EC data (and if it
         * is, some more info about it).
         * From lowest to highest bits:
         *      4b: Error correction length (if Error correction length type is 0)
         *      1b: fOpaqueData
         *      2b: Error correction length type
         *      1b: fECPresent: Tells us if we are even looking at EC data
         */
        DRM_BYTE bECFlags = 0;

        bECFlags |= 0x02;   /* -Indicate that 2B of EC data follow */
        /* -Not opaque */
        /* -No EC length type */
        bECFlags |= 0x80;   /* -Indicates EC present */

        SaveBYTEOffset( pPacketHeaderBuffer->blobBuffer.pbBlob, bECFlags, dwOffset );

        /*
         * Write 2B of EC data (today we just write '00 00')
         */
        SaveWORDOffset( pPacketHeaderBuffer->blobBuffer.pbBlob, wTmp, dwOffset );
    }

    /*
     * The first two BYTEs are always present and look as follows
     * (bits specified in lowest-to-highest order):
     *      BYTE 0: 1b: fMultiplePayloadsPresent
     *              2b: Sequence length type
     *              2b: Padding length type
     *              2b: Packet length type
     *              1b: Error-correction data present (should be 0!)
     *      BYTE 1: 2b: Replicated data length type
     *              2b: Media object offset length type
     *              2b: Media object ID length type
     *              2b: Stream ID length type
     */
    if ( pAsfPacketWriterContext->m_ASFPacketInfo.fMultiPayloads )
    {
        bLengthFlags |= 0x01;                           /* -Multi payloads */
    }
    bLengthFlags |= ((pAsfPacketWriterContext->m_ASFPacketInfo.bPacketLenType << 5) & 0x60);     /* -Sequence length type */
    bLengthFlags |= ((pAsfPacketWriterContext->m_ASFPacketInfo.bPadLenType << 3) & 0x18);        /* -Padding length type */
    bLengthFlags |= ((pAsfPacketWriterContext->m_ASFPacketInfo.bSequenceLenType << 1) & 0x06);   /* -Packet length type */

    SaveBYTEOffset( pPacketHeaderBuffer->blobBuffer.pbBlob, bLengthFlags, dwOffset );

    bPropertyFlags |= (pAsfPacketWriterContext->m_ASFPacketInfo.bStreamIDLenType << 6) & 0xc0;   /* -Replicated data length type */
    bPropertyFlags |= (pAsfPacketWriterContext->m_ASFPacketInfo.bObjectIDLenType << 4) & 0x30;   /* -Media object offset length type */
    bPropertyFlags |= (pAsfPacketWriterContext->m_ASFPacketInfo.bOffsetLenType << 2) & 0x0c;     /* -Media object ID length type */
    bPropertyFlags |= (pAsfPacketWriterContext->m_ASFPacketInfo.bReplicatedDataLenType) & 0x03;  /* -Stream ID length type */

    SaveBYTEOffset( pPacketHeaderBuffer->blobBuffer.pbBlob, bPropertyFlags, dwOffset );

    /*
     * Packet length (for variable-size packets)
     */
    ChkDR( SaveVarField( pAsfPacketWriterContext->m_ASFPacketInfo.bPacketLenType, 
                         pAsfPacketWriterContext->m_ASFPacketInfo.dwExplicitPacketLength,
                         pPacketHeaderBuffer->blobBuffer.pbBlob,
                         pPacketHeaderBuffer->blobBuffer.cbBlob,
                         &dwOffset ) );

    /*
     * Sequence number
     */
    ChkDR( SaveVarField( pAsfPacketWriterContext->m_ASFPacketInfo.bSequenceLenType,
                         pAsfPacketWriterContext->m_ASFPacketInfo.dwSequenceNum,
                         pPacketHeaderBuffer->blobBuffer.pbBlob,
                         pPacketHeaderBuffer->blobBuffer.cbBlob,
                         &dwOffset ) );

    /*
     * Padding length
     */
    ChkDR( SaveVarField( pAsfPacketWriterContext->m_ASFPacketInfo.bPadLenType,
                         pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding,
                         pPacketHeaderBuffer->blobBuffer.pbBlob,
                         pPacketHeaderBuffer->blobBuffer.cbBlob,
                         &dwOffset ) );

    /*
     * Send time
     */
    SaveDWORDOffset( pPacketHeaderBuffer->blobBuffer.pbBlob, pAsfPacketWriterContext->m_ASFPacketInfo.dwSCR, dwOffset );


    /*
     * Duration
     */
    SaveWORDOffset( pPacketHeaderBuffer->blobBuffer.pbBlob, pAsfPacketWriterContext->m_ASFPacketInfo.wDuration, dwOffset );

    if ( pAsfPacketWriterContext->m_ASFPacketInfo.fMultiPayloads )
    {
        /*
         * Multiple payload info:
         *          low 6b: Number of payloads
         *          high 2b: Payload Length type
         */
        DRM_BYTE bPayloadFlags = 0;

        if ( 1 == pPacket->bRepData )
        {
            /* NOTE: This won't work for compressed payloads */
            ChkDR( DRM_E_NOTIMPL );
        }

        bPayloadFlags |= (pAsfPacketWriterContext->m_ASFPacketInfo.dwPayloads & 0x3f);
        bPayloadFlags |= (pAsfPacketWriterContext->m_ASFPacketInfo.bPayLenType << 6) & 0xc0;

        SaveBYTEOffset( pPacketHeaderBuffer->blobBuffer.pbBlob, bPayloadFlags, dwOffset );
    }

    /*
     * The packet header is done
     */
    ChkDR( ASFSample_AddBuffer( pPacket, pPacketHeaderBuffer ) );

 ErrorExit:
    if ( NULL != pPacketHeaderBuffer )
    {
        ASFMediaBuffer_Release( pPacketHeaderBuffer );
    }
    SAFE_OEM_FREE( blobBuffer.pbBlob );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT WritePayloadHeader(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const DRM_DWORD                         dwPayload,
    __out      ASF_SAMPLE                * const pPacket
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PAYLOAD_PARSER_INFO *pPayload;
    DRM_DWORD cbPayloadHeader;
    DRM_BYTE bStreamFlags = 0;
    DRM_DWORD cMediaBuffers = 0;
    ASF_MEDIA_BUFFER *pPayloadHeaderBuffer = NULL;
    DRM_BYTEBLOB blobBuffer = { NULL, 0 };
    DRM_DWORD dwOffset = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePayloadHeader );

    ChkArg( dwPayload < pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize );
    ChkArg( pPacket );

    ChkDR( DRM_PtrList_GetByIndex( &pAsfPacketWriterContext->m_ASFPayloadInfos, dwPayload, (DRM_VOID **) &pPayload ) );

    ChkDR( ASFSample_GetBufferCount( pPacket, &cMediaBuffers ) );
    if ( cMediaBuffers == 0 )
    {
        ChkDR( DRM_E_FAIL );
    }

    DRMASSERT( pPayload->wTotalSize > pPayload->wTotalDataBytes );
    cbPayloadHeader = pPayload->wTotalSize - pPayload->wTotalDataBytes;
    blobBuffer.cbBlob = cbPayloadHeader;
    blobBuffer.pbBlob = (DRM_BYTE *) Oem_MemAlloc( blobBuffer.cbBlob );
    ChkMem( blobBuffer.pbBlob );

    pPayloadHeaderBuffer = (ASF_MEDIA_BUFFER *) Oem_MemAlloc( SIZEOF( ASF_MEDIA_BUFFER ) );
    ChkMem( pPayloadHeaderBuffer );
    ChkDR( ASFMediaBuffer_Initialize( pPayloadHeaderBuffer, blobBuffer, 0 ) );
    blobBuffer.pbBlob = NULL;

    /*
     * Stream number: high bit is fKeyFrame
     */
    if ( pPayload->fIsKeyFrame )
    {
        bStreamFlags |= 0x80;
    }
    bStreamFlags |= (pPayload->bStreamId & 0x7f);

    SaveBYTEOffset( pPayloadHeaderBuffer->blobBuffer.pbBlob, bStreamFlags, dwOffset );

    /*
     * Media Object ID
     */
    SaveVarField( pAsfPacketWriterContext->m_ASFPacketInfo.bObjectIDLenType, 
                  pPayload->bObjectId, 
                  pPayloadHeaderBuffer->blobBuffer.pbBlob,
                  pPayloadHeaderBuffer->blobBuffer.cbBlob,
                  &dwOffset );

    DRMASSERT( pPayload->bRepData >= STANDARD_REPLICATED_DATA_SIZE );
    if ( pPayload->bRepData < STANDARD_REPLICATED_DATA_SIZE )
    {
        ChkDR( DRM_E_FAIL );
    }

    /*
     * The rest of the payload header is:
     *  -Media object offset
     *  -Replicated data length (is >= 8)
     *  -Replicated data (DRM_DWORD cbObjectSize, DRM_DWORD msObjectPresTime, and possibly more)
     *  -Payload length (if there are multiple payloads)
     */
    SaveVarField( pAsfPacketWriterContext->m_ASFPacketInfo.bOffsetLenType,
                  pPayload->dwObjectOffset,
                  pPayloadHeaderBuffer->blobBuffer.pbBlob,
                  pPayloadHeaderBuffer->blobBuffer.cbBlob,
                  &dwOffset );
    
    SaveVarField( pAsfPacketWriterContext->m_ASFPacketInfo.bReplicatedDataLenType,
                  pPayload->bRepData,
                  pPayloadHeaderBuffer->blobBuffer.pbBlob,
                  pPayloadHeaderBuffer->blobBuffer.cbBlob,
                  &dwOffset );

    SaveDWORDOffset( pPayloadHeaderBuffer->blobBuffer.pbBlob, pPayload->dwObjectSize, dwOffset );
    SaveDWORDOffset( pPayloadHeaderBuffer->blobBuffer.pbBlob, pPayload->dwObjectPres, dwOffset );

    /*
     * Write remaining replicated data!!!
     * Object Size and Object Presentation Time have already been written
     */
    DRM_BYT_CopyBytes( pPayloadHeaderBuffer->blobBuffer.pbBlob, 
                       dwOffset, 
                       pPayload->rgbRepData,
                       2 * SIZEOF( DRM_DWORD ), 
                       pPayload->bRepData - STANDARD_REPLICATED_DATA_SIZE );
    dwOffset += pPayload->bRepData - STANDARD_REPLICATED_DATA_SIZE;

    if ( pAsfPacketWriterContext->m_ASFPacketInfo.fMultiPayloads )
    {
        SaveVarField( pAsfPacketWriterContext->m_ASFPacketInfo.bPayLenType,
                      pPayload->wTotalDataBytes,
                      pPayloadHeaderBuffer->blobBuffer.pbBlob,
                      pPayloadHeaderBuffer->blobBuffer.cbBlob,
                      &dwOffset );
    }

    /*
     * The payload header is done
     */

    ChkDR( ASFSample_AddBuffer( pPacket, pPayloadHeaderBuffer ) );

 ErrorExit:
    if ( NULL != pPayloadHeaderBuffer )
    {
        ASFMediaBuffer_Release( pPayloadHeaderBuffer );
    }
    SAFE_OEM_FREE( blobBuffer.pbBlob );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT WritePayloadData(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const DRM_DWORD                         dwPayload,
    __out      ASF_SAMPLE                * const pPacket
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PAYLOAD_PARSER_INFO *pPayload;
    DRM_DWORD  cMediaBuffers = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePayloadData );

    ChkArg( dwPayload < pAsfPacketWriterContext->m_ASFPayloadInfos.dwCurrentSize );
    ChkArg( pPacket );

    ChkDR( DRM_PtrList_GetByIndex( &pAsfPacketWriterContext->m_ASFPayloadInfos, dwPayload, (DRM_VOID **) &pPayload ) );

    ChkDR( ASFSample_GetBufferCount( pPacket, &cMediaBuffers ) );
    if ( cMediaBuffers == 0 )
    {
        ChkDR( DRM_E_FAIL );
    }

    DRMASSERT( (pPayload->cPayloadDataBuffers > 0) || (0 == pPayload->wTotalDataBytes ) );
    ChkDR( WritePayloadDataWithoutEncrypt( pAsfPacketWriterContext, pPayload, pPacket ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT WritePadding(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __out      ASF_SAMPLE                * const pPacket
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_MEDIA_BUFFER *pPaddingBuffer = NULL;
    DRM_BYTEBLOB blobBuffer = { NULL, 0 };
    DRM_DWORD dwOffset = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePadding );

    ChkArg( pPacket );

    if ( 0 == pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding )
    {
        goto ErrorExit;
    }

    blobBuffer.cbBlob = pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding;
    blobBuffer.pbBlob = (DRM_BYTE *) Oem_MemAlloc( blobBuffer.cbBlob );
    ChkMem( blobBuffer.pbBlob );

    pPaddingBuffer = (ASF_MEDIA_BUFFER *) Oem_MemAlloc( SIZEOF( ASF_MEDIA_BUFFER ) );
    ChkMem( pPaddingBuffer );
    ChkDR( ASFMediaBuffer_Initialize( pPaddingBuffer, blobBuffer, 0 ) );
    blobBuffer.pbBlob = NULL;

    DRM_BYT_SetBytes( pPaddingBuffer->blobBuffer.pbBlob, dwOffset, pAsfPacketWriterContext->m_ASFPacketInfo.dwPadding, 0xcc );
    /*
     * Add it to the end of the MFSample
     */
    ChkDR( ASFSample_AddBuffer( pPacket, pPaddingBuffer ) );

 ErrorExit:
    if ( NULL != pPaddingBuffer )
    {
        ASFMediaBuffer_Release( pPaddingBuffer );
    }
    SAFE_OEM_FREE( blobBuffer.pbBlob );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/****************************************************************************/
static DRM_RESULT WritePayloadDataWithoutEncrypt(
    __in const ASF_PACKET_WRITER_CONTEXT * const pAsfPacketWriterContext,
    __in const ASF_PAYLOAD_PARSER_INFO   * const pPayload,
    __out      ASF_SAMPLE                * const pPacket
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_MEDIA_BUFFER *pBuffer = NULL;
    ASF_MEDIA_BUFFER *pBufferWrapper = NULL;
    DRM_DWORD dwBuffer;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_WritePayloadDataWithoutEncrypt );

    ChkArg( pPayload );
    ChkArg( pPacket );

    for ( dwBuffer = pPayload->dwPayloadDataStartBuffer; 
          dwBuffer < (pPayload->dwPayloadDataStartBuffer + pPayload->cPayloadDataBuffers); 
          dwBuffer++ )
    {
        ChkDR( DRM_PtrList_GetByIndex( &pAsfPacketWriterContext->m_PayloadDataBuffers, dwBuffer, (DRM_VOID **) &pBuffer ) );

        if ( pBuffer->cbCurrentOffset + pBuffer->cbCurrentLength == pBuffer->blobBuffer.cbBlob )
        {
            ChkDR( ASFSample_AddBuffer( pPacket, pBuffer ) );
        }
        else
        {
            pBufferWrapper = (ASF_MEDIA_BUFFER *) Oem_MemAlloc( SIZEOF( ASF_MEDIA_BUFFER ) );
            ChkMem( pBufferWrapper );
            ChkDR( ASFMediaBuffer_InitializeWrapper( pBufferWrapper, 
                                                     pBuffer,
                                                     pBuffer->cbCurrentOffset, 
                                                     pBuffer->cbCurrentLength ) );

            ChkDR( ASFSample_AddBuffer( pPacket, pBufferWrapper ) );
        }

        if ( NULL != pBufferWrapper )
        {
            ASFMediaBuffer_Release( pBufferWrapper );
            pBufferWrapper = NULL;
        }

    }
 
ErrorExit:
    if ( NULL != pBufferWrapper )
    {
        ASFMediaBuffer_Release( pBufferWrapper );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


#endif /* DRM_SUPPORT_AUDIO_ONLY == 0 */

EXIT_PK_NAMESPACE_CODE;
