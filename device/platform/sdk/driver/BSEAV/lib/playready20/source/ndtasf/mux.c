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
#include <mux.h>
#include <asfsample.h>

ENTER_PK_NAMESPACE_CODE;

#if DRM_SUPPORT_AUDIO_ONLY == 0

static DRM_RESULT _Clear(
    __inout ASF_MULTIPLEXER_CONTEXT * const pAsfMuxContext
)
{
    DRM_WORD i;
    ASF_SAMPLE *pPacket = NULL;

    for (i = 0; i < MAXSTREAMS; i++)
    {
        ASFMuxStream_Uninitialize( pAsfMuxContext->m_rgpMuxStreams[i] );
        SAFE_OEM_FREE( pAsfMuxContext->m_rgpMuxStreams[i]);
        pAsfMuxContext->m_rgpMuxStreams[i] = NULL;
    }
    ZEROMEM( pAsfMuxContext->m_rgMuxStats, SIZEOF(pAsfMuxContext->m_rgMuxStats) );


    SAFE_OEM_FREE( pAsfMuxContext->m_StreamNumbers );
    pAsfMuxContext->m_cStreamNumbers = 0;

    ASFMuxSendWindow_Uninitialize( pAsfMuxContext->m_pMuxSendWindow );
    SAFE_OEM_FREE( pAsfMuxContext->m_pMuxSendWindow );

    while ( DRM_SUCCEEDED( DRM_PtrList_RemoveHead( &pAsfMuxContext->m_WaitingPackets, (DRM_VOID **)&pPacket ) ) )
    {
        if ( NULL != pPacket )
        {
            ASFSample_Release( pPacket );
            pPacket = NULL;
        }
    }

    SAFE_OEM_FREE( pAsfMuxContext->m_ppMuxQueueObjects );

    return( DRM_SUCCESS );
}



static DRM_RESULT _CreateMuxStream(
    __inout    ASF_MULTIPLEXER_CONTEXT * const pAsfMuxContext,
    __in const DRM_UINT64                      hnsBaseSendTime, 
    __in       ASF_STREAM_INFO                *pASFStreamConfig 
)
{
    DRM_RESULT      dr = DRM_SUCCESS;
    ASF_MUX_STREAM_CONTEXT *pNewMuxStream = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_CreateMuxStream );

    if (( 0 == pASFStreamConfig->wStreamNumber ) || ( pASFStreamConfig->wStreamNumber > MFASF_MAX_STREAM_NUMBER ))
    {
        ChkDR( DRM_E_ASF_INVALID_STREAM_NUMBER );
    }

    ChkMem( pNewMuxStream = (ASF_MUX_STREAM_CONTEXT *) Oem_MemAlloc(SIZEOF(ASF_MUX_STREAM_CONTEXT)) );

    ChkDR( ASFMuxStream_Initialize( pNewMuxStream,
                                    pASFStreamConfig, 
                                    pAsfMuxContext->m_msPreroll,
                                    hnsBaseSendTime,
                                    pAsfMuxContext ) );

    pAsfMuxContext->m_rgpMuxStreams[ pASFStreamConfig->wStreamNumber ] = pNewMuxStream;
    

 ErrorExit:

    if ( DRM_FAILED( dr ) )
    {
        ASFMuxStream_Uninitialize( pNewMuxStream );
        SAFE_OEM_FREE( pNewMuxStream );
    }

    pAsfMuxContext->m_cMuxQueueObjects = 0;

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



static DRM_RESULT _SizeQueueObjectsArray(    
    __inout    ASF_MULTIPLEXER_CONTEXT * const pAsfMuxContext,
    __in const DRM_DWORD                       dwArraySize 
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_SizeQueueObjectsArray );

    /*
     * If the array is large enough, proceed.
     */
    if ( pAsfMuxContext->m_cMuxQueueObjects >= dwArraySize )
    {
        goto ErrorExit;
    }

    /*
     * We need more space!
     */
    SAFE_OEM_FREE( pAsfMuxContext->m_ppMuxQueueObjects );

    pAsfMuxContext->m_ppMuxQueueObjects = (ASF_MUX_QUEUE_OBJECT_CONTEXT **) Oem_MemAlloc(SIZEOF(ASF_MUX_QUEUE_OBJECT_CONTEXT *) * dwArraySize);
    ChkMem(pAsfMuxContext->m_ppMuxQueueObjects);
    ZEROMEM(pAsfMuxContext->m_ppMuxQueueObjects, SIZEOF(ASF_MUX_QUEUE_OBJECT_CONTEXT *) * dwArraySize);

    pAsfMuxContext->m_cMuxQueueObjects = dwArraySize;

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMux_Initialize
 **
 ** Synopsis:    Initializes the Mux.  The context initialized with this 
 **              function is then used in subsequent ASFMux calls.
 **
 ** Arguments:   [pAsfMuxContext]    -- Mux context
 **              [pStreamInfoList]   -- list of streams for the mux
 **              [dwFixedPacketSize] -- size of packets the mux will generate
 **              [qwPreroll]         -- preroll
 **              [dwMaxBitrate]      -- maximum bitrate of the streams
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 *****************************************************************************/
DRM_RESULT ASFMux_Initialize(
    __inout    ASF_MULTIPLEXER_CONTEXT * const pAsfMuxContext,
    __in const PTR_LIST                * const pStreamInfoList,
    __in const DRM_DWORD                       dwFixedPacketSize,
    __in const DRM_UINT64                      qwPreroll,
    __in const DRM_DWORD                       dwMaxBitrate
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 hnsBaseSendTime = DRM_UI64(0);
    DRM_WORD cStreams = 0;
    DRM_WORD i = 0;
    PTR_LIST_ENTRY  *pEntry;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_Initialize );

    ChkArg( NULL != pAsfMuxContext );
    ChkArg( NULL != pStreamInfoList );

    ZEROMEM( pAsfMuxContext, SIZEOF( ASF_MULTIPLEXER_CONTEXT ) );
    
    pAsfMuxContext->m_dwFlags = 0;
    pAsfMuxContext->m_msSyncTolerance = DRM_UI64(3000);
    
    pAsfMuxContext->m_cbFixedPacketSize = dwFixedPacketSize;
    pAsfMuxContext->m_dwMaxBitratePlusOverhead = dwMaxBitrate;
    pAsfMuxContext->m_pMuxSendWindow = NULL;
    pAsfMuxContext->m_ppMuxQueueObjects = NULL;
    pAsfMuxContext->m_cMuxQueueObjects = 0;
    pAsfMuxContext->m_cPackets = DRM_UI64(0);

    pAsfMuxContext->m_hnsLastPacketSendTime = DRM_UI64(0);
    pAsfMuxContext->m_hnsLastPacketDuration = DRM_UI64(0);
    pAsfMuxContext->m_msPreroll = qwPreroll;

    DRM_PtrList_Initialize( &pAsfMuxContext->m_WaitingPackets, 128 );

    ChkDR( _Clear(pAsfMuxContext) );
        
    /* Stream count now comes from the stream info list
     *    ChkDR( pIProfile->GetStreamCount ( pAsfMuxContext, &cStreams ) );
     */
    DRMASSERT( pStreamInfoList->dwCurrentSize <= MFASF_MAX_STREAM_NUMBER );
    DRMASSERT( pStreamInfoList->dwCurrentSize != 0 );

    for ( pEntry = pStreamInfoList->pHead; NULL != pEntry; pEntry = pEntry->pNext )
    {
        cStreams++;
    }

    pAsfMuxContext->m_StreamNumbers = (DRM_WORD *)Oem_MemAlloc( SIZEOF( DRM_WORD ) * cStreams);
    i = 0;
    for ( pEntry = pStreamInfoList->pHead; NULL != pEntry; pEntry = pEntry->pNext )
    {
        ASF_STREAM_INFO *pInfo;
        pInfo = (ASF_STREAM_INFO *) pEntry->pbData;
        pAsfMuxContext->m_StreamNumbers[i] = pInfo->wStreamNumber;

        ChkDR( _CreateMuxStream( pAsfMuxContext, hnsBaseSendTime, pInfo ) );

        ZEROMEM( &pAsfMuxContext->m_rgMuxStats[ pInfo->wStreamNumber ], SIZEOF( ASF_MUX_STATISTICS ) );
        i++;
    }
    pAsfMuxContext->m_cStreamNumbers = cStreams;
    
    /*
     * Create the send window
     * send window size should be AV sync size + buffer window size
     *
     *
     *
     * The "send window" for all streams is the max over all streams of that stream's
     * buffer window plus that stream's max latency through the codec.
     * Here is a quick-and-dirty explanation for why the send window is
     * what it is.
     *
     * The writer sends samples to the codecs in strict order of presentation time.
     * Let the presentation time of a sample that gets sent at a particular point
     * in time be P.  All streams' codecs will be getting samples with prestime
     * approximately P right now.
     *
     * Since the codec has a certain max latency of L_s, it could
     * output a compressed sample that needs to be PRESENTED as early as P-L_s
     * and as late as P.
     * (If L_s is unavailable, we assume that it is equal to the stream's buffer window.)
     *
     * When it gets to us, we assign it a sendtime (see CWMMuxStream::OnSample())
     * which could be up to b_s earlier than the prestime that comes into the mux,
     * where b_s is the stream's buffer window.  That means we can end up with a send
     * window as early as (P-L_s) - b_s.
     *
     * So to accommodate all streams, we need the send window to hold samples with
     * sendtimes ranging from P - (L_S + b_S) to P, where S is the stream that maximizes
     * L_s + b_s.
     *
     *
     * AV sync size should be pipeline latency,
     * For now, it is always 3s in asf sink. It should be changed according to real codec latency.
     * This is useful for low latency codec.
     * 
     * buffer window size is used to make sure the send time difference is covered.
     */ 
    pAsfMuxContext->m_pMuxSendWindow = (ASF_MUX_SEND_WINDOW_CONTEXT *) Oem_MemAlloc( SIZEOF( *pAsfMuxContext->m_pMuxSendWindow ) );
    ChkMem(pAsfMuxContext->m_pMuxSendWindow);
    ChkDR( ASFMuxSendWindow_Initialize( pAsfMuxContext->m_pMuxSendWindow,
                                        DRM_UI64Low32(pAsfMuxContext->m_msPreroll) + DRM_UI64Low32(pAsfMuxContext->m_msSyncTolerance),
                                        pAsfMuxContext->m_cbFixedPacketSize,
                                        DRM_UI64Low32(pAsfMuxContext->m_msPreroll),
                                        pAsfMuxContext ) );

    ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        _Clear(pAsfMuxContext);
        SAFE_OEM_FREE(pAsfMuxContext->m_pMuxSendWindow);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMux_Uninitialize
 **
 ** Synopsis:    Uninitializes the Mux.
 **
 ** Arguments:   [pAsfMuxContext]    -- Mux context
 **
 ** Returns:     N/A
 **
 *****************************************************************************/
DRM_VOID ASFMux_Uninitialize(
    __in ASF_MULTIPLEXER_CONTEXT * const pAsfMuxContext
)
{
    if ( NULL != pAsfMuxContext )
    {
        _Clear( pAsfMuxContext );
    }

    return;
}



/*****************************************************************************
 **
 ** Function:    ASFMux_ProcessSample
 **
 ** Synopsis:    Processes a sample.  Determines a send time for the sample
 **              based on the incoming presentation time, and then packetizes
 **              any samples that are ready to be sent.
 **
 ** Arguments:   [pAsfMuxContext]     -- Mux context
 **              [wStreamNumber]      -- stream number of incoming sample
 **              [pISample]           -- incoming sample
 **              [hnsTimestampAdjust] -- sample time of incoming sample
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMux_ProcessSample(
    __inout    ASF_MULTIPLEXER_CONTEXT * const pAsfMuxContext,
    __in const DRM_WORD                        wStreamNumber,
    __in       ASF_SAMPLE              * const pISample,
    __in const DRM_UINT64                      hnsTimestampAdjust
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbSample = 0; 
    ASF_MUX_STREAM_CONTEXT *pMuxStream = NULL;
    DRM_DWORD cMaxObjects = 0;
    DRM_DWORD dwFlags = 0;
    DRM_DWORD cObjects = 0;
    ASF_SAMPLE *pPacket = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_ProcessSample );

    MUX_TRACE(( "ASFMux_ProcessSample" ));

    ChkArg( pAsfMuxContext );
    ChkArg( pISample );

    if( ( wStreamNumber > MFASF_MAX_STREAM_NUMBER ) || 
          !pAsfMuxContext->m_rgpMuxStreams[ wStreamNumber ] )
    {
        return( DRM_E_ASF_INVALID_STREAM_NUMBER );
    }

    if ( DRM_SUCCEEDED( DRM_PtrList_GetFront( &pAsfMuxContext->m_WaitingPackets,(DRM_VOID **)&pPacket ) ) )
    {
        ChkDR( DRM_E_ASF_NOT_ACCEPTING );
    }

    ASFSample_GetTotalLength( pISample, &cbSample );
    
    MUX_TRACE(( "%s : Sample to mux: stream %d, size %lu, prestime %lu, with adjustment %lu",
                __FUNCTION__,
                wStreamNumber,
                cbSample,
                DRM_UI64Low32(pISample->hnsSampleTime),
                DRM_UI64Low32(pISample->hnsSampleTime) + DRM_UI64Low32(hnsTimestampAdjust) ));
    
    pMuxStream = pAsfMuxContext->m_rgpMuxStreams[ wStreamNumber ];
    /*
     * What's the maximum number of objects that could be produced here?
     */
    ChkDR( ASFMuxStream_HowManyObjectsWillBeReturned( pMuxStream, pISample, &cMaxObjects ) );
    
    ChkDR( _SizeQueueObjectsArray( pAsfMuxContext, cMaxObjects ) );

    cObjects = cMaxObjects;
    dwFlags = pISample->dwFlags;
    
    /*
     * Send them to the MuxStream
     */
    dr = ASFMuxStream_OnSample( pMuxStream,
                                pISample, 
                                hnsTimestampAdjust,
                                dwFlags, 
                                pAsfMuxContext->m_ppMuxQueueObjects, 
                                &cObjects );
    if ( DRM_FAILED( dr ) )
    {
        if( ( DRM_E_ASF_BANDWIDTH_OVERRUN != dr ) &&
            ( DRM_E_ASF_LATE_SAMPLE != dr ) )
        {
            MUX_TRACE(( "%s: MUX FAILED FOR UNEXPECTED/UNKNOWN REASON\n", __FUNCTION__ ));
        }
        else if( DRM_E_ASF_LATE_SAMPLE == dr )
        {
            MUX_TRACE(( "%s: MUX FAILED FOR LATE SAMPLE ARRIVAL \n", __FUNCTION__ ));
        }
        else
        {
            MUX_TRACE(( "%s: MUX FAILED FOR BANDWIDTH OVERRUN\n", __FUNCTION__ ));
        }
    }
    ChkDR(dr);
    
    /*
     * Now that we have a send time, we can put it in line to be packetized
     */
    if ( cObjects > 0 )
    {
        ChkDR( ASFMuxSendWindow_AddObjectsAndSendPackets( pAsfMuxContext->m_pMuxSendWindow, 
                                                          pAsfMuxContext->m_ppMuxQueueObjects, 
                                                          cObjects ) );
    }

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMux_GetNextPacket
 **
 ** Synopsis:    Gets a completed packet out of the mux.  If there are are no
 **              packets completed, ppIPacket is set to NULL and success is
 **              returned.
 **
 ** Arguments:   [pAsfMuxContext]     -- Mux context
 **              [ppIPacket]          -- completed packet
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMux_GetNextPacket(
    __in  ASF_MULTIPLEXER_CONTEXT  * const pAsfMuxContext,
    __out ASF_SAMPLE                     **ppIPacket
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_GetNextPacket );

    ChkArg( pAsfMuxContext );
    ChkArg( ppIPacket );

    *ppIPacket = NULL;

    if ( DRM_FAILED( DRM_PtrList_GetFront( &pAsfMuxContext->m_WaitingPackets, (DRM_VOID **)ppIPacket ) ) )
    {
        /*
         * Nothing is waiting.  This is a success case
         */
        goto ErrorExit;
    }

    ChkDR( DRM_PtrList_RemoveHead( &pAsfMuxContext->m_WaitingPackets, (DRM_VOID **)ppIPacket ) );
    /*
     * Leave the ref on *ppIPacket for the caller
     */

 ErrorExit:
    if ( DRM_FAILED( dr ) && NULL != ppIPacket )
    {
        if ( NULL != *ppIPacket )
        {
            ASFSample_Release( *ppIPacket );
            SAFE_OEM_FREE( *ppIPacket );
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMux_Flush
 **
 ** Synopsis:    Flushes all samples out of the mux, packetizing them.
 **
 ** Arguments:   [pAsfMuxContext]     -- Mux context
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMux_Flush(
    __in ASF_MULTIPLEXER_CONTEXT * const pAsfMuxContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_Flush );

    ChkArg( NULL != pAsfMuxContext );

    /*
     * Clean up each MuxStream, which will release any buffers
     * they have refed
     */
    for ( i = 0; i < pAsfMuxContext->m_cStreamNumbers; i++ )
    {
        DRMASSERT( NULL != pAsfMuxContext->m_rgpMuxStreams[ pAsfMuxContext->m_StreamNumbers[i] ] );

        ASFMuxStream_Cleanup(pAsfMuxContext->m_rgpMuxStreams[ pAsfMuxContext->m_StreamNumbers[i] ]);
    }

    /*
     * Flush the sendwindow
     */
    ChkDR( ASFMuxSendWindow_Flush(pAsfMuxContext->m_pMuxSendWindow) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMux_OnCompletedPacket
 **
 ** Synopsis:    Called internally by the mux when enough samples have been
 **              added to a packet to fill it up.
 **
 ** Arguments:   [pAsfMuxContext]     -- Mux context
 **              [pIPacket]           -- completed packet
 **              [hnsSampleTime]      -- sample time of completed packet
 **              [hnsSampleDuration]  -- duration of packet
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMux_OnCompletedPacket(
    __inout    ASF_MULTIPLEXER_CONTEXT * const pAsfMuxContext,
    __inout    ASF_SAMPLE              * const pIPacket,
    __in const DRM_UINT64                      hnsSampleTime,
    __in const DRM_UINT64                      hnsSampleDuration
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 hnsSendTime = DRM_UI64LITERAL( 0, 0 );


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_OnCompletedPacket );

    ChkArg( NULL != pAsfMuxContext );

    MUX_TRACE(( "ASFMux_OnCompletedPacket" ));

    /*
     * Mux should not worry about the quota. As long as there are samples coming in,
     * mux just simply adds them into packets. Sink should do the limitation work based on quota number. 
     */
        
    /*
     * Collect some stats
     */
    hnsSendTime = hnsSampleTime;

    /* DRMASSERT( hnsSendTime >= pAsfMuxContext->m_hnsLastPacketSendTime ); */
    DRMASSERT( ( DRM_UI64Les( pAsfMuxContext->m_hnsLastPacketSendTime, hnsSendTime ) 
                 || DRM_UI64Eql( hnsSendTime, pAsfMuxContext->m_hnsLastPacketSendTime ) )
               && "Packet send times are going backwards?!" );
    pAsfMuxContext->m_hnsLastPacketSendTime = hnsSendTime;

    pAsfMuxContext->m_hnsLastPacketDuration = hnsSampleDuration;


    /*
     * Add it to the tail of the queue
     */
    ChkDR( DRM_PtrList_AddTail( &pAsfMuxContext->m_WaitingPackets, pIPacket ) );
    ASFSample_AddRef( pIPacket );

    pAsfMuxContext->m_cPackets = DRM_UI64Add( pAsfMuxContext->m_cPackets, DRM_UI64( 1 ) );

    /*
     * The packet is ready in mux.
     */
    MUX_TRACE(( "packet# %lu: at sendtime = %lu", 
                DRM_UI64Low32(pAsfMuxContext->m_cPackets),
                DRM_UI64Low32(pAsfMuxContext->m_hnsLastPacketSendTime) ));

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMux_GetMinStreamSendtimeCutoff
 **
 ** Synopsis:    Called internally by the mux to update the send time cutoff.
 **
 ** Arguments:   [pAsfMuxContext]     -- Mux context
 **              [phnsSendTimeCutoff] -- cutoff send time for the mux
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMux_GetMinStreamSendtimeCutoff(
    __in const ASF_MULTIPLEXER_CONTEXT * const pAsfMuxContext,
    __out      DRM_UINT64              * const phnsSendTimeCutoff 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 hnsMinSendTimeCutoff = INVALID_SAMPLE_TIME;
    DRM_DWORD i = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_GetMinStreamSendtimeCutoff );

    MUX_TRACE(( "ASFMux_GetMinStreamSendtimeCutoff" ));

    ChkArg( NULL != pAsfMuxContext );
    ChkArg( phnsSendTimeCutoff );

    for ( i = 0; i < pAsfMuxContext->m_cStreamNumbers; i++ )
    {
        DRM_WORD wStreamNumber = pAsfMuxContext->m_StreamNumbers[i];
        DRM_UINT64 hnsStreamSendTimeCutoff;

        DRMASSERT( NULL != pAsfMuxContext->m_rgpMuxStreams[ wStreamNumber ] );

        ChkDR( ASFMuxStream_GetHappyCutoffSendtime(pAsfMuxContext->m_rgpMuxStreams[ wStreamNumber ],
                                                   &hnsStreamSendTimeCutoff) );

        MUX_TRACE(( "%s: Stream %d willing to packtize sendtimes earlier than %ld",
                    __FUNCTION__,
                    wStreamNumber,
                    DRM_UI64Low32(hnsStreamSendTimeCutoff) ));
        /* if ( hnsStreamSendTimeCutoff < hnsMinSendTimeCutoff ) */
        if ( DRM_UI64Les( hnsStreamSendTimeCutoff, hnsMinSendTimeCutoff ) )
        {
            hnsMinSendTimeCutoff = hnsStreamSendTimeCutoff;
        }
    }

    *phnsSendTimeCutoff = hnsMinSendTimeCutoff;

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMux_AdjustSendTimeForOffset
 **
 ** Synopsis:    Called internally by the mux to adjust the send time of a
 **              sample
 **
 ** Arguments:   [pAsfMuxContext]       -- Mux context
 **              [cbOriginalObjectSize] -- original object size
 **              [pQueueObject]         -- Queue object to update
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMux_AdjustSendTimeForOffset( 
    __in const ASF_MULTIPLEXER_CONTEXT      * const pAsfMuxContext,
    __in const DRM_DWORD                            cbOriginalObjectSize,
    __inout    ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WORD wStreamNumber;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMux_AdjustSendTimeForOffset );

    ChkArg( pQueueObject );
    ChkArg( NULL != pAsfMuxContext );

    wStreamNumber = pQueueObject->m_wStreamNumber;

    if ( ( wStreamNumber > MFASF_MAX_STREAM_NUMBER ) || 
         !pAsfMuxContext->m_rgpMuxStreams[ wStreamNumber ] )
    {
        ChkDR( DRM_E_ASF_INVALID_STREAM_NUMBER );
    }

    ChkDR( ASFMuxStream_AdjustSendTimeForOffset( pAsfMuxContext->m_rgpMuxStreams[ wStreamNumber ],
                                                 cbOriginalObjectSize,
                                                 pQueueObject ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

#endif /* DRM_SUPPORT_AUDIO_ONLY == 0 */

EXIT_PK_NAMESPACE_CODE;
