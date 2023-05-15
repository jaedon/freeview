/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*  File:       sendwindow.c
 *
 *  Contents:   Where the compressed media objects stay until they
 *              are ready to be sent
 *
 *  The following is a high-level picture of how our mux forms packets
 *  to send:
 *
 *  Compressed media samples come to the mux and are given send times
 *  based on the relevant stream's current bandwidth usage (see
 *  ASF_MUX_STREAM for more details on this).
 *
 *  The ASF_MUX_SEND_WINDOW maintains a "send window" which is a list of
 *  compressed samples, ordered by send time, that are ready to be
 *  written to the sink.  The window size (in milliseconds) is
 *  determined at construction time.
 *
 *  The ASF_MUX_SEND_WINDOW also maintains a reference to an ASF_PACKET_WRITER_CONTEXT
 *  that in general represents a partially-full packet.  
 *  As it comes time to write a sample, we add it (or a fragment of it) as a 
 *  payload.  When the write packet is full, we "write" it out to an IMFSample, 
 *  clear out the IMFASFPacketParser, and start filling it with new data.
 *
 *  When a sample arrives (ASFMuxSendWindow_AddSampleAndSendPackets()),
 *  we look at its sendtime, and for all samples in our window whose
 *  sendtimes are more than msWindowSize behind this packet, we write
 *  as much as we can of it into the packet (sending the packet if it's
 *  full, as described above).
 *
 *  Thus, the layout of data in the packets will roughly look as though
 *  you had laid them out end to end and chopped them into chunks of
 *  the appropriate fixed packet size.  There will be exceptions, e.g. for
 *  streams that require fixed-size, unfragmentable payloads.  But that's
 *  the general idea.
 *
 *--------------------------------------------------------------------------
 */

#include <drmcommon.h>
#include <drmptrlist.h>
#include <ndtcontextsizes.h>
#include <ndtasf_internal.h>
#include <sendwindow.h>
#include <asfsample.h>
#include <asfmediabuffer.h>
#include <mux.h>

ENTER_PK_NAMESPACE_CODE;

#if DRM_SUPPORT_AUDIO_ONLY == 0

static DRM_RESULT InitializeWritePacket(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext
);

static DRM_UINT64 GetCutoffSendTime(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext
);

static DRM_RESULT PacketizeAndSend(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext,
    __in const DRM_UINT64                          hnsCutoffSendTime,
    __in const DRM_BOOL                            fFlush 
);

static DRM_RESULT PacketizeSample(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT  * const pAsfMuxSendWindowContext,
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject,
    __out   DRM_BOOL                     * const pfReject,
    __out   DRM_BOOL                     * const pfFull 
);

static DRM_RESULT AddPayload(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT  * const pAsfMuxSendWindowContext,
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject
);

static DRM_RESULT FindFirstSampleThatFits(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext,
    __in const DRM_DWORD                           dwStartIndex,
    __out      DRM_DWORD                          *pdwFoundIndex 
);

static DRM_BOOL ReadyToSend(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT  * const pAsfMuxSendWindowContext,
    __in const ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject 
);

static DRM_RESULT SendOffWritePacket(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext,
    __in const DRM_UINT64                          hnsDuration 
);

static DRM_RESULT PackageAndSend(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext,
    __in const DRM_UINT64                          hnsDuration,
    __in const DRM_UINT64                          hnsSendTime 
);

static DRM_RESULT HandleFinishedQueueObject(
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject 
);



/*****************************************************************************
 **
 ** Function:    ASFMuxSendWindow_Initialize
 **
 ** Synopsis:    Initializes the send window.  The context initialized with 
 **              this function is then used in subsequent ASFMuxSendWindow
 **              calls.
 **
 ** Arguments:   [pAsfMuxSendWindowContext] -- send window context
 **              [msWindowSize]             -- window size
 **              [cbFixedPacketSize]        -- output packet size
 **              [msFirstPresTime]          -- first presentation time
 **              [pMux]                     -- backpointer to mux
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxSendWindow_Initialize(
    __out      ASF_MUX_SEND_WINDOW_CONTEXT         * const pAsfMuxSendWindowContext,
    __in const DRM_DWORD                                   msWindowSize,
    __in const DRM_DWORD                                   cbFixedPacketSize,
    __in const DRM_DWORD                                   msFirstPresTime,
    __in       struct __tagASF_MULTIPLEXER_CONTEXT * const pMux 
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxSendWindow_Initialize );

    ChkArg( NULL != pAsfMuxSendWindowContext );
    DRMASSERT( NULL != pMux );

    pAsfMuxSendWindowContext->m_pMux = pMux;
    pAsfMuxSendWindowContext->m_msWindowSize = msWindowSize;
    pAsfMuxSendWindowContext->m_cbFixedPacketSize = cbFixedPacketSize;
    pAsfMuxSendWindowContext->m_msFirstPresTime = msFirstPresTime;
    pAsfMuxSendWindowContext->m_hnsLatestSendTimeQueued = DRM_UI64( 0 );
    pAsfMuxSendWindowContext->m_hnsWritePacketSendTime = DRM_UI64( 0 );
    pAsfMuxSendWindowContext->m_cbSent = DRM_UI64( 0 );
    pAsfMuxSendWindowContext->m_cPacketsSent = 0;
    pAsfMuxSendWindowContext->m_msLastSendTimeSent = DRM_UI64( 0 );
    pAsfMuxSendWindowContext->m_msLastPresTimeSent = DRM_UI64( 0 );
    pAsfMuxSendWindowContext->m_msLastSendDurationSent = DRM_UI64( 0 );

    ChkDR( DRM_OrderedList_Initialize( &pAsfMuxSendWindowContext->m_Samples, 196 ) );

    ChkDR( ASFPacketWriter_Initialize( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext, 
                                       pAsfMuxSendWindowContext->m_cbFixedPacketSize ) );
    ChkDR( InitializeWritePacket( pAsfMuxSendWindowContext ) );

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMuxSendWindow_Uninitialize
 **
 ** Synopsis:    Uninitializes the send window.
 **
 ** Arguments:   [pAsfMuxSendWindowContext] -- send window context
 **
 ** Returns:     void
 **
 *****************************************************************************/
DRM_VOID ASFMuxSendWindow_Uninitialize(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext
)
{
    if ( NULL != pAsfMuxSendWindowContext )
    {
        DRM_DWORD cSamples = 0;

        (DRM_VOID) DRM_OrderedList_GetCount( &pAsfMuxSendWindowContext->m_Samples, &cSamples );
        if ( cSamples > 0 )
        {
            MUX_TRACE(("We're shutting down with %d samples still in the queue, should have flushed\n", 
                       cSamples));
        }

        ASFPacketWriter_Uninitialize( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext );
    }

    return;
}


/*****************************************************************************
 **
 ** Function:    ASFMuxSendWindow_AddObjectsAndSendPackets
 **
 ** Synopsis:    Adds objects the the send window and generates packets.
 **              When a new sample is added to the send queue, if some 
 **              samples fall out of the buffer window and there is enough
 **              data to form at least one entire packet, we form the 
 **              packet(s) and send it (them) away.
 **
 ** Arguments:   [pAsfMuxSendWindowContext] -- send window context
 **              [ppMuxQueueObjects]        -- mux queue objects generated
 **              [cObjects]                 -- number of objects
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxSendWindow_AddObjectsAndSendPackets(  
    __inout               ASF_MUX_SEND_WINDOW_CONTEXT  * const pAsfMuxSendWindowContext,
    __in_ecount(cObjects) ASF_MUX_QUEUE_OBJECT_CONTEXT       **ppMuxQueueObjects,
    __in const            DRM_DWORD                            cObjects 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_MUX_QUEUE_OBJECT_CONTEXT *pObject = NULL;
    DRM_UINT64 hnsCutoffSendTime;
    DRM_DWORD i;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxSendWindow_AddObjectsAndSendPackets );

    ChkBOOL( cObjects > 0, DRM_E_FAIL );
    ChkArg( ppMuxQueueObjects );

    for ( i = 0; i < cObjects; i++ )
    {
        DRM_UINT64 hnsSendTime;

        pObject = ppMuxQueueObjects[i];

        /*
         * Is this is greatest send time we've seen so far?
         */
        hnsSendTime = pObject->m_hnsSendTime;
        /* if ( hnsSendTime > pAsfMuxSendWindowContext->m_hnsLatestSendTimeQueued ) */
        if ( DRM_UI64Les( pAsfMuxSendWindowContext->m_hnsLatestSendTimeQueued, hnsSendTime ) )
        {
            pAsfMuxSendWindowContext->m_hnsLatestSendTimeQueued = hnsSendTime;
        }

        /*
         * Add the sample into the appropriate position in the send window
         */
        ChkDR( DRM_OrderedList_AddEntry( &pAsfMuxSendWindowContext->m_Samples, hnsSendTime, pObject ) );

        MUX_TRACE(("Object %d for stream %d added to send window", 
                   pObject->m_bObjectID, 
                   pObject->m_wStreamNumber));
    }
    /*
     * End of dealing with new objects
     */

    /*
     * Any samples that have fallen out of the send window:
     * Packetize them up
     */

    /*
     * The cutoff will m_msWindowSize msec back from the new end
     * of the send window
     */
    hnsCutoffSendTime = GetCutoffSendTime(pAsfMuxSendWindowContext);
    MUX_TRACE(("Cutoff send time = %d", 
               hnsCutoffSendTime));

    ChkDR( PacketizeAndSend( pAsfMuxSendWindowContext, hnsCutoffSendTime, FALSE ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMuxSendWindow_Flush
 **
 ** Synopsis:    Flushes objects through the send window.
 **
 ** Arguments:   [pAsfMuxSendWindowContext] -- send window context
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxSendWindow_Flush(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PACKET_PARSER_INFO ASFPacketInfo;
    DRM_DWORD cSamples = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxSendWindow_Flush );

    /*
     * Packetize and send all samples in the send window queue
     * until there aren't any more in the queue
     */

    while ( DRM_SUCCEEDED( DRM_OrderedList_GetCount(&pAsfMuxSendWindowContext->m_Samples, &cSamples) ) )
    {
        if ( cSamples == 0 )
        {
            break;
        }
        ChkDR( PacketizeAndSend( pAsfMuxSendWindowContext, DRM_UI64( 0 ), TRUE ) );
    }

    /*
     * Force the standing write packet to be sent.
     * For the send duration, we make a guess based on the bitrate
     * Seconds = bits / (bits / second)
     */
    ZEROMEM( &ASFPacketInfo, SIZEOF( ASFPacketInfo ) );
    ChkDR( ASFPacketWriter_GetPacketInfo( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext, &ASFPacketInfo ) );

    if ( ASFPacketInfo.dwPayloads > 0 )
    {
        DRM_UINT64 hnsDuration;

        /* hnsDuration =
         *   (8 * pAsfMuxSendWindowContext->m_cbFixedPacketSize * 1000 * 10000) / pAsfMuxSendWindowContext->m_pMux->m_dwMaxBitratePlusOverhead;
         */
        hnsDuration = DRM_UI64Div( DRM_UI64Mul( DRM_UI64( 8L * 1000L * 10000L ),
                                                DRM_UI64( pAsfMuxSendWindowContext->m_cbFixedPacketSize ) ),
                                   DRM_UI64( pAsfMuxSendWindowContext->m_pMux->m_dwMaxBitratePlusOverhead ) );
        (DRM_VOID) SendOffWritePacket( pAsfMuxSendWindowContext, hnsDuration );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



static DRM_RESULT InitializeWritePacket(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_PACKET_PARSER_INFO ASFPacketInfo;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_InitializeWritePacket );

    /*
     * Clear out the ASFPacketWriter
     */
    ChkDR( ASFPacketWriter_Clear(&pAsfMuxSendWindowContext->m_ASFPacketWriterContext) );

    /*
     * We need to make sure that the ASFPacketWriter at least has
     * the fixed packet size so that it can know how much space
     * remains
     */
    ZEROMEM( &ASFPacketInfo, SIZEOF(ASFPacketInfo) );

    ASFPacketInfo.cbPacketLength = pAsfMuxSendWindowContext->m_cbFixedPacketSize;

    ChkDR( ASFPacketWriter_SetPacketInfo( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext, &ASFPacketInfo ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************
 * The cutoff sendtime is determined to be the LATER of the following
 * two values
 *      * The latest sendtime seen overall minus the size of the send window
 *      * The min over all streams of the latest sendtime seen for that stream
 ***************************************************************************/
static DRM_UINT64 GetCutoffSendTime(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext
)
{
    DRM_UINT64 hnsMinPerStreamCutoff = DRM_UI64( 0 );
    DRM_UINT64 hnsWindowCutoff = DRM_UI64( 0 );


    MUX_TRACE(("ASFMuxSendWindow_GetCutoffSendTime"));

    /*
     * The latest sendtime seen overall minus the size of the send window
     */
    /* if ( pAsfMuxSendWindowContext->m_hnsLatestSendTimeQueued > (10000 * pAsfMuxSendWindowContext->m_msWindowSize) ) */
    if ( DRM_UI64Les( DRM_UI64Mul( DRM_UI64( 10000 ),
                                   DRM_UI64( pAsfMuxSendWindowContext->m_msWindowSize ) ),
                      pAsfMuxSendWindowContext->m_hnsLatestSendTimeQueued ) )
    {
        /* hnsWindowCutoff = pAsfMuxSendWindowContext->m_hnsLatestSendTimeQueued - (10000 * pAsfMuxSendWindowContext->m_msWindowSize); */
        hnsWindowCutoff = DRM_UI64Sub( pAsfMuxSendWindowContext->m_hnsLatestSendTimeQueued,
                                       DRM_UI64Mul( DRM_UI64( 10000 ),
                                                    DRM_UI64( pAsfMuxSendWindowContext->m_msWindowSize ) ) );

        MUX_TRACE(("Last sendtime = %lu ms, Window size = %d ms, cutoff according to window is %lu ms",
                   DRM_UI64Low32( pAsfMuxSendWindowContext->m_hnsLatestSendTimeQueued ) / 10000,
                   pAsfMuxSendWindowContext->m_msWindowSize,
                   DRM_UI64Low32( hnsWindowCutoff ) / 10000 ));
    }
    /*else...
     *      We haven't even seen a send window worth of samples,
     *      so don't do anything
     *
     * What is the minimum "next send time"?
     */
    if ( DRM_FAILED( ASFMux_GetMinStreamSendtimeCutoff( pAsfMuxSendWindowContext->m_pMux, &hnsMinPerStreamCutoff ) ) )
    {
        hnsMinPerStreamCutoff = DRM_UI64( 0 );
    }

    MUX_TRACE(("All streams have gotten up to sendtime %lu ms, will use this for the cutoff if it is later",
               DRM_UI64Low32( hnsMinPerStreamCutoff ) / 10000 ));

    /*
     * Return whichever is later
     */
    /* if ( hnsMinPerStreamCutoff > hnsWindowCutoff ) */
    if ( DRM_UI64Les( hnsWindowCutoff, hnsMinPerStreamCutoff ) )
    {
        return hnsMinPerStreamCutoff;
    }
    else
    {
        return hnsWindowCutoff;
    }
    
}



/**************************************************************************
 * Packetizes and sends media objects sitting in the send window.
 * If fFlush is TRUE, then packetizes and sends everything currently
 * in the send window.
 * Otherwise packetizes and sends only those objects with send times
 * before hnsCutoffSendTime
 ***************************************************************************/
static DRM_RESULT PacketizeAndSend(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext,
    __in const DRM_UINT64                          hnsCutoffSendTime,
    __in const DRM_BOOL                            fFlush 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_MUX_QUEUE_OBJECT_CONTEXT *pQueueObject = NULL;
    DRM_UINT64 hnsQueueObjectSendTime = DRM_UI64( 0 );
    DRM_BOOL fDonePacketizing = FALSE;
    DRM_DWORD dwFirstEntryToConsider = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_PacketizeAndSend );

    MUX_TRACE(("ASFMuxSendWindow_PacketizeAndSend"));

    while ( pAsfMuxSendWindowContext->m_Samples.dwCurrentSize > dwFirstEntryToConsider && !fDonePacketizing )
    {
        /*
         * Get the first sample and temporarily hold it off the queue.
         * We'll put it back only if we don't use the whole thing.
         */
        ChkDR( DRM_OrderedList_RemoveEntry( &pAsfMuxSendWindowContext->m_Samples, 
                                            dwFirstEntryToConsider, 
                                            &hnsQueueObjectSendTime, 
                                            (DRM_VOID **) &pQueueObject ) );
        ChkFAIL( pQueueObject != NULL );
        if ( pQueueObject->m_wStreamNumber > MFASF_MAX_STREAM_NUMBER )
        {
            DRMASSERT( FALSE );
            ChkDR( DRM_E_FAIL );
        }

        /*
         * Is its send time early enough to send?
         */
        /* if ( fFlush || hnsQueueObjectSendTime < hnsCutoffSendTime ) */
        if ( fFlush || DRM_UI64Les( hnsQueueObjectSendTime, hnsCutoffSendTime ) )
        {
            /*
             * Before packetizing, keep track of how many bytes remained
             * to be packetized.
             * We'll use this to determine how many bytes were packetized
             */
            DRM_DWORD cbOriginalObjectSize = pQueueObject->m_cbSize;
            DRM_BOOL fRejected = FALSE;
            DRM_BOOL fFull = FALSE;

            MUX_TRACE(("Will packetize up to %d bytes from object at sendtime t=%lu",
                       cbOriginalObjectSize,
                       DRM_UI64Low32( hnsQueueObjectSendTime) ));

            /*
             * Try to write this sample into the write packet
             */
            ChkDR( PacketizeSample( pAsfMuxSendWindowContext, pQueueObject, &fRejected, &fFull ) );

            /*
             * Either clean up the list item or put it back on the queue,
             * depending on whether the whole thing got packetized.
             */

            if ( pQueueObject->m_cbSize > 0 )
            {
                /*
                 * There is still some stuff left to be packetized in
                 * the future: Put it back on the list with an
                 * adjusted sendtime
                 */
                if ( pQueueObject->m_cbSize < cbOriginalObjectSize )
                {
                    /*
                     * Have the appropriate ASF_MUX_STREAM adjust the send time
                     * for us
                     *
                     * If it fails, then no big deal...
                     */
                    ASFMux_AdjustSendTimeForOffset( pAsfMuxSendWindowContext->m_pMux, cbOriginalObjectSize, pQueueObject );
                }

                /*
                 * The remaining fragment goes back in the queue.  We call AddLeadingEntry
                 * here - in the case where there are multiple objects with the same send time
                 * we want to make sure this one is placed ahead of others of the same time.
                 */
                ChkDR( DRM_OrderedList_AddLeadingEntry( &pAsfMuxSendWindowContext->m_Samples, pQueueObject->m_hnsSendTime, pQueueObject ) );
            }
            else
            {
                /*
                 * This sample has been completely packetized, so we're done with it
                 */
                (DRM_VOID) HandleFinishedQueueObject( pQueueObject );

                ASFMuxQueueObject_Release( pQueueObject );
                pQueueObject = NULL;
            }

            /*
             * If this sample was rejected, then we put it back on the queue
             * exactly as it was before, which means it will go back into
             * exactly the same position in the ordered list.
             * But since this sample got rejected, next time we're looking
             * for something to packetize, let's start with something we
             * think will be able to fit.
             *
             * If, on the other hand, the sample wasn't rejected, we should
             * start looking at the rejected samples again, since there might
             * be room for them now.
             */
            if ( fRejected )
            {
                /*
                 * Look for something that could possibly fit in the remaining space
                 */
                    
                DRM_DWORD dwStartAt = dwFirstEntryToConsider + 1;
                DRM_DWORD dwFoundAt = 0;

                dr = FindFirstSampleThatFits( pAsfMuxSendWindowContext, dwStartAt, &dwFoundAt );
                if ( DRM_SUCCEEDED( dr ) && !fFull && dwFoundAt <= REASONABLE_LOOKAHEAD_FOR_PAYLOAD )
                {
                    /*
                     * We found something that will fit, start looking here
                     */
                    dwFirstEntryToConsider = dwFoundAt;
                }
                else
                {
                    /*
                     * Nothing else will fit in this packet, so just go ahead
                     * and send it so we can start a new one
                     */
                    ChkDR( SendOffWritePacket( pAsfMuxSendWindowContext, 
                                               DRM_UI64Sub( hnsQueueObjectSendTime, 
                                                            pAsfMuxSendWindowContext->m_hnsWritePacketSendTime ) ) );
                }
            }
            else
            {
                dwFirstEntryToConsider = 0;
            }
        }
        else
        {
            /*
             * This sample's send time is after the cutoff for sending,
             * and we're not flushing:
             *
             * This is an ordered list, so this and everything after it
             * are not ready to be sent yet.  We call AddLeadingEntry
             * here - in the case where there are multiple objects with
             * the same send time we want to make sure this one is placed
             * ahead of others of the same time.
             */
            fDonePacketizing = TRUE;
            ChkDR( DRM_OrderedList_AddLeadingEntry( &pAsfMuxSendWindowContext->m_Samples, hnsQueueObjectSendTime, pQueueObject ) );
        }
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************
 * Whatever remains of pSample's data gets packetized into a payload
 * here.
 * Objects that can't be fragmented will be rejected unless there
 * is space for the whole thing.
 * If it happens to make a full write packet, that write packet
 * gets sent to the sink.
 ***************************************************************************/
static DRM_RESULT PacketizeSample(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT  * const pAsfMuxSendWindowContext,
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject,
    __out   DRM_BOOL                     * const pfReject,
    __out   DRM_BOOL                     * const pfFull 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fReadyToSend;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_PacketizeSample );

    MUX_TRACE(("ASFMuxSendWindow_PacketizeSample"));

    ChkArg( pQueueObject );
    ChkArg( pfReject );
    ChkArg( pfFull );

    *pfReject = FALSE;
    *pfFull = FALSE;

    /*
     * We may have to send off the current packet
     */
    fReadyToSend = ReadyToSend( pAsfMuxSendWindowContext, pQueueObject );

    if ( !fReadyToSend )
    {
        /*
         * More can probably be done with this packet:
         * Let's stick another payload in there
         */
        dr = AddPayload( pAsfMuxSendWindowContext, pQueueObject );
        if ( DRM_FAILED( dr ) )
        {
            if ( DRM_E_ASF_TOO_MANY_PAYLOADS == dr )
            {
                MUX_TRACE(("Packet did not accept payload because it is full with prestime %lu",
                           DRM_UI64Low32( pQueueObject->m_msPresTime ) ));

                /*
                 * Everything's really OK, we're just rejecting a payload because it is full
                 */
                *pfFull = TRUE;
                *pfReject = TRUE;
                dr = DRM_SUCCESS;
            }

            if ( DRM_E_ASF_NOT_ACCEPTING == dr )
            {
                MUX_TRACE(("Packet did not accept payload from object with prestime %lu",
                           DRM_UI64Low32( pQueueObject->m_msPresTime ) ));

                /* Everything's really OK, we're just rejecting a payload */
                *pfReject = TRUE;
                dr = DRM_SUCCESS;
            }

            goto ErrorExit;
        }

        /*
         * If this pushes ahead the lastest pres time we've seen, then
         * update that stat
         */
        /* if ( pQueueObject->m_msPresTime > pAsfMuxSendWindowContext->m_msLastPresTimeSent ) */
        if ( DRM_UI64Les( pAsfMuxSendWindowContext->m_msLastPresTimeSent, pQueueObject->m_msPresTime ) )
        {
            pAsfMuxSendWindowContext->m_msLastPresTimeSent = pQueueObject->m_msPresTime;
        }
    }
    else
    {
        DRM_UINT64 qwDuration;

        /*
         * We're out of space: Let's send the write packet we've been working on
         */
        /* if ( pQueueObject->m_hnsSendTime < pAsfMuxSendWindowContext->m_hnsWritePacketSendTime ) */
        if ( DRM_UI64Les( pQueueObject->m_hnsSendTime, pAsfMuxSendWindowContext->m_hnsWritePacketSendTime ) )
        {
            /*
             * This is an edge-case.  It means that right before we're ready to send off
             * this existing write packet, someone enqueued an object whose send
             * time is a little earlier.  In this case we squash back the send time
             */
            /* DRMASSERT( (pQueueObject->m_hnsSendTime / 10000) >= pAsfMuxSendWindowContext->m_msLastSendTimeSent ); */
            DRMASSERT( DRM_UI64Les( pAsfMuxSendWindowContext->m_msLastSendTimeSent,
                                    DRM_UI64Div( pQueueObject->m_hnsSendTime, DRM_UI64( 10000 ) ) ) ||
                       DRM_UI64Eql( pAsfMuxSendWindowContext->m_msLastSendTimeSent,
                                    DRM_UI64Div( pQueueObject->m_hnsSendTime, DRM_UI64( 10000 ) ) ) );
            pAsfMuxSendWindowContext->m_hnsWritePacketSendTime = pQueueObject->m_hnsSendTime;
        }

        qwDuration = DRM_UI64Sub( pQueueObject->m_hnsSendTime, pAsfMuxSendWindowContext->m_hnsWritePacketSendTime );

        ChkDR( SendOffWritePacket( pAsfMuxSendWindowContext, qwDuration ) );
        ChkDR( PacketizeSample( pAsfMuxSendWindowContext, pQueueObject, pfReject, pfFull ) );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************
 * This adds as much as it can from the ASF_MUX_QUEUE_OBJECT, given
 * the amount of space available
 ***************************************************************************/
static DRM_RESULT AddPayload(
    __inout ASF_MUX_SEND_WINDOW_CONTEXT  * const pAsfMuxSendWindowContext,
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 hnsSendTime;
    ASF_PACKET_PARSER_INFO ASFPacketInfo;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_AddPayload );

    MUX_TRACE(("ASFMuxSendWindow_AddPayload"));

    hnsSendTime = pQueueObject->m_hnsSendTime;

    ZEROMEM( &ASFPacketInfo, SIZEOF( ASFPacketInfo ) );
    ChkDR( ASFPacketWriter_GetPacketInfo( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext, &ASFPacketInfo ) );

    /*
     * Figure out the send time, or make sure that the one already in the write
     * packet is okay
     */
    if ( 0 == ASFPacketInfo.dwPayloads )
    {
        /*
         * This is the first (and earliest) payload in the packet,
         * so we will use it to set the send time
         */
        /* DRMASSERT( (pAsfMuxSendWindowContext->m_hnsWritePacketSendTime / 10000) == pAsfMuxSendWindowContext->m_msLastSendTimeSent ); */
        DRMASSERT( DRM_UI64Eql( DRM_UI64Div( pAsfMuxSendWindowContext->m_hnsWritePacketSendTime, DRM_UI64( 10000 ) ),
                                pAsfMuxSendWindowContext->m_msLastSendTimeSent ) );
        /* DRMASSERT( hnsSendTime >= pAsfMuxSendWindowContext->m_hnsWritePacketSendTime ); */
        DRMASSERT( DRM_UI64Les( pAsfMuxSendWindowContext->m_hnsWritePacketSendTime, hnsSendTime ) ||
                   DRM_UI64Eql( pAsfMuxSendWindowContext->m_hnsWritePacketSendTime, hnsSendTime ) );
        pAsfMuxSendWindowContext->m_hnsWritePacketSendTime = hnsSendTime;

        MUX_TRACE(("This is the first payload: Packet gets sendtime %lu",
                   DRM_UI64Low32( pAsfMuxSendWindowContext->m_hnsWritePacketSendTime ) ));
    }
    else
    {
        /*
         * The write packet already contains a payload.
         */
        /* DRMASSERT( hnsSendTime >= (10000 * pAsfMuxSendWindowContext->m_msLastSendTimeSent) ); */
        DRMASSERT( DRM_UI64Les( DRM_UI64Mul( DRM_UI64( 10000 ), pAsfMuxSendWindowContext->m_msLastSendTimeSent),
                                hnsSendTime ) ||
                   DRM_UI64Eql( DRM_UI64Mul( DRM_UI64( 10000 ), pAsfMuxSendWindowContext->m_msLastSendTimeSent),
                                hnsSendTime ) );
        /* if ( hnsSendTime < pAsfMuxSendWindowContext->m_hnsWritePacketSendTime ) */
        if ( DRM_UI64Les( hnsSendTime, pAsfMuxSendWindowContext->m_hnsWritePacketSendTime ) )
        {
            /* This shouldn't happen */
            pAsfMuxSendWindowContext->m_hnsWritePacketSendTime = hnsSendTime;
        }

        MUX_TRACE(("Placing payload, send time %lu, in packet with sendtime %lu",
                   DRM_UI64Low32( hnsSendTime ),
                   DRM_UI64Low32( pAsfMuxSendWindowContext->m_hnsWritePacketSendTime ) ));
    }

    /*
     * Let the QueueObject add itself as a payload
     * Also, keep track of how much got packetized
     */
    ChkDR( ASFMuxQueueObject_GeneratePayload( pQueueObject, &pAsfMuxSendWindowContext->m_ASFPacketWriterContext ) );

 ErrorExit:

    if ( DRM_SUCCEEDED( dr ) )
    {
        MUX_TRACE(("%d bytes from object %d for stream %d at prestime %lu added to packet", 
                   pQueueObject->m_cbSize - pQueueObject->m_cbSize, 
                   pQueueObject->m_bObjectID, 
                   pQueueObject->m_wStreamNumber,
                   DRM_UI64Low32( pQueueObject->m_msPresTime ) ));

        MUX_TRACE(("%d bytes from stream %d at prestime(ms) %lu added to packet#%d at sendtime %lu", 
                   pQueueObject->m_cbSize - pQueueObject->m_cbSize, 
                   pQueueObject->m_wStreamNumber,
                   DRM_UI64Low32( pQueueObject->m_msPresTime ),
                   pAsfMuxSendWindowContext->m_cPacketsSent + 1,
                   DRM_UI64Low32( pAsfMuxSendWindowContext->m_hnsWritePacketSendTime ) ));
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************/
static DRM_RESULT FindFirstSampleThatFits(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext,
    __in const DRM_DWORD                           dwStartIndex,
    __out      DRM_DWORD                          *pdwFoundIndex 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fSomethingCanFit = FALSE;
    DRM_DWORD dwIndex = dwStartIndex;
    ASF_MUX_QUEUE_OBJECT_CONTEXT *pQueueObject = NULL;
    DRM_UINT64 hnsQueueSampleSendTime = DRM_UI64( 0 );


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_FindFirstSampleThatFits );

    ChkArg( pdwFoundIndex );

    *pdwFoundIndex = pAsfMuxSendWindowContext->m_Samples.dwCurrentSize;

    while ( !fSomethingCanFit && dwIndex < pAsfMuxSendWindowContext->m_Samples.dwCurrentSize )
    {
        DRM_DWORD cbSpaceAvailable = 0;
        ChkDR( DRM_OrderedList_GetEntry( &pAsfMuxSendWindowContext->m_Samples, 
                                         dwIndex, 
                                         &hnsQueueSampleSendTime, 
                                         (DRM_VOID **) &pQueueObject ) );

        ChkDR( ASFPacketWriter_GetDataSpaceAvailable( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext,
                                                      pQueueObject->m_wStreamNumber,
                                                      pQueueObject->m_bObjectID,
                                                      pQueueObject->m_pMFSample,
                                                      &cbSpaceAvailable ) );

        if ( pQueueObject->m_fFragmentable )
        {

            if ( cbSpaceAvailable > 0 )
            {
                /*
                 * Found something to fit!
                 */
                fSomethingCanFit = TRUE;
                *pdwFoundIndex = dwIndex;
                break;
            }
        }
        else
        {
            /*
             * This can't be fragmented, so only if we can take the whole payload
             */
            if ( cbSpaceAvailable >= pQueueObject->m_cbTargetSize )
            {
                fSomethingCanFit = TRUE;
                *pdwFoundIndex = dwIndex;
                break;
            }
        }
        
        dwIndex++;
    }

    if ( !fSomethingCanFit )
    {
        ChkDR( DRM_E_ASF_NOT_ACCEPTING );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
 }



/**************************************************************************
 * Returns TRUE if we should be sending the outgoing packet now
 ***************************************************************************/
static DRM_BOOL ReadyToSend(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT  * const pAsfMuxSendWindowContext,
    __in const ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbSpaceAvailable = 0;
    DRM_DWORD cbMinimumReasonablePayloadSize;
    DRM_BOOL fReadyToSend = FALSE;


    MUX_TRACE(("ASFMuxSendWindow_ReadyToSend"));

    /*
     * In general, we're ready to send if there's not enough space for another payload
     * header and a handful of payload bytes.
     * (Let's say that it's only worth it to add another payload if the amount of
     * payload data that we add is at least the size of the payload overhead, otherwise
     * there was no point in adding it!)
     */
    ChkDR( ASFPacketWriter_GetDataSpaceAvailable( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext,
                                                  pQueueObject->m_wStreamNumber,
                                                  pQueueObject->m_bObjectID,
                                                  pQueueObject->m_pMFSample,
                                                  &cbSpaceAvailable ) );

    cbMinimumReasonablePayloadSize = PAYLOAD_OVERHEAD;
    fReadyToSend = (cbSpaceAvailable < cbMinimumReasonablePayloadSize);

    MUX_TRACE(("Packet has %d bytes of free space left: %s ready to send",
               cbSpaceAvailable,
               fReadyToSend ? "" : "not"));

 ErrorExit:
    return fReadyToSend;
}



/**************************************************************************
 * Sends off the current write packet and starts up a new one
 ***************************************************************************/
static DRM_RESULT SendOffWritePacket(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext,
    __in const DRM_UINT64                          hnsDuration 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 qwSCR = pAsfMuxSendWindowContext->m_hnsWritePacketSendTime;
    ASF_PACKET_PARSER_INFO ASFPacketInfo;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_SendOffWritePacket );

    /*
     * Set send time and duration.
     * Since the packet that was passed into this function will determine
     * the send time of the next packet, we use that to calculate this
     * packet's duration
     */
    ZEROMEM( &ASFPacketInfo, SIZEOF( ASFPacketInfo ) );
    ChkDR( ASFPacketWriter_GetPacketInfo( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext, &ASFPacketInfo ) );
    DRMASSERT( ASFPacketInfo.dwPayloads > 0 );

    ASFPacketInfo.dwSCR = DRM_UI64Low32( DRM_UI64Div( qwSCR, DRM_UI64( 10000 ) ) );
    ASFPacketInfo.wDuration = (DRM_WORD) DRM_UI64Low32( DRM_UI64Div( hnsDuration, DRM_UI64( 10000 ) ) );

    ChkDR( ASFPacketWriter_SetPacketInfo( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext, &ASFPacketInfo ) );

    /*
     * Send it off
     */
    ChkDR( PackageAndSend( pAsfMuxSendWindowContext, hnsDuration, qwSCR ) );

    MUX_TRACE(("ASFMuxSendWindow_SendOffWritePacket(): Sent packet with sendtime = %lu", 
               DRM_UI64Low32( qwSCR ) / 10000 ));

    /*
     * Update packet send stats
     */
    pAsfMuxSendWindowContext->m_cbSent = DRM_UI64Add( pAsfMuxSendWindowContext->m_cbSent, DRM_UI64( pAsfMuxSendWindowContext->m_cbFixedPacketSize ) );
    pAsfMuxSendWindowContext->m_cPacketsSent++;
    pAsfMuxSendWindowContext->m_msLastSendTimeSent = DRM_UI64Div( qwSCR, DRM_UI64( 10000 ) );
    pAsfMuxSendWindowContext->m_msLastSendDurationSent = DRM_UI64Div( hnsDuration, DRM_UI64( 10000 ) );

    /*
     * OK, now let's get down to business, reinitialize the outgoing write packet,
     * and packetize this sample by calling this function again.  The write packet
     * should be big enough to fit some data in this time around
     */
    ChkDR( InitializeWritePacket(pAsfMuxSendWindowContext) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************
 * This is the standard method for packaging up data and sending it
 * back to the sink:
 * The data gets serialized in "packet form" in a single buffer,
 * and that buffer gets sent to the sink.
 * Perf costs: Everything -- including all of the payload data -- gets
 * memcopied here
 ***************************************************************************/
static DRM_RESULT PackageAndSend(
    __inout    ASF_MUX_SEND_WINDOW_CONTEXT * const pAsfMuxSendWindowContext,
    __in const DRM_UINT64                          hnsDuration,
    __in const DRM_UINT64                          hnsSendTime 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_SAMPLE *pPacketSample = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_PackageAndSend );

    /*
     * Get a packet buffer from the pool and store to it
     */
    ChkDR( ASFPacketWriter_WritePacket( &pAsfMuxSendWindowContext->m_ASFPacketWriterContext, &pPacketSample ) );

    /*
     * Pass it to the mux, who will then own getting it out to the user
     */
    ChkDR( ASFMux_OnCompletedPacket( pAsfMuxSendWindowContext->m_pMux, pPacketSample, hnsSendTime, hnsDuration ) );

 ErrorExit:
    if ( NULL != pPacketSample )
    {
        ASFSample_Release( pPacketSample );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************/
static DRM_RESULT HandleFinishedQueueObject(
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_SAMPLE *pUsedSample = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_HandleFinishedQueueObject );

    DRMASSERT( NULL != pQueueObject );
    if ( !pQueueObject )
    {
        ChkDR( DRM_E_FAIL );
    }

    MUX_TRACE(("Done with sample(s) for stream %d, presentation time %I64u",
               pQueueObject->m_wStreamNumber,
               pQueueObject->m_msPresTime));

    /*
     * For each IMFSample that this queue object used the tail of,
     * logging the buffer WMI trace that we're done with this sample
     */
    while( DRM_SUCCEEDED( ASFMuxQueueObject_PopUsedSample( pQueueObject, &pUsedSample ) ) )
    {
#if LOG_BUFFER
        if ( bWMILOG_BUFFER )
        {
            /*
             * we are logging
             */
            DRM_UINT64 hnsSampleTime = INVALID_SAMPLE_TIME;
            DRM_DWORD cbSample = 0;
            DRM_DWORD cbBuffer;
            DRM_UINT64 hnsSampleDuration = DRM_UI64( 0 );

            hnsSampleTime = pUsedSample->hnsSampleTime;

            cbBuffer = MFWMI_GetMFSampleTotalSpace( pUsedSample );

            cbSample = pUsedSample->cbTotalLength;
                    
            hnsSampleDuration = pUsedSample->hnsSampleDuration;

            MUX_TRACE(( "HandleFinishedQueueObject: stream %d, size %d, time %lu duration %lu",
                        pQueueObject->m_wStreamNumber,
                        cbSample,
                        DRM_UI64Low32( hnsSampleTime ),
                        DRM_UI64Low32( hnsSampleDuration ) ));
        }
#endif /* LOG_BUFFER */
        if ( NULL != pUsedSample )
        {
            ASFSample_Release( pUsedSample );
            pUsedSample = NULL;
        }
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/**************************************************************************
 * The following few methods are for classes that represent compressed
 * media objects waiting to be packetized and sent
 ***************************************************************************/

/**************************************************************************/
static DRM_RESULT FragmentableObject_Initialize(
    __out ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __in  ASF_SAMPLE                   * const pMFSample
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    
    pAsfMuxQueueObjectContext->m_cbAlreadyPacketized = 0;

    pAsfMuxQueueObjectContext->m_pMFSample = pMFSample;
    ChkDR( ASFSample_AddRef( pAsfMuxQueueObjectContext->m_pMFSample ) );

    pAsfMuxQueueObjectContext->m_cbSize = pAsfMuxQueueObjectContext->m_pMFSample->cbTotalLength;
    pAsfMuxQueueObjectContext->m_cbTotalObjectSize = pAsfMuxQueueObjectContext->m_cbSize;
    
 ErrorExit:
    return dr;
}

static DRM_RESULT UnfragmentableObject_Initialize(
    __out      ASF_MUX_QUEUE_OBJECT_CONTEXT *pAsfMuxQueueObjectContext,
    __in       ASF_SAMPLE                   *pMFFirstSample,
    __in const DRM_DWORD                     cbOffsetIntoFirstSample,
    __in const DRM_DWORD                     cbTargetSize
);

static DRM_VOID UnfragmentableObject_Uninitialize(
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext
);

static DRM_RESULT FragmentableObject_GeneratePayload( 
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __inout ASF_PACKET_WRITER_CONTEXT    * const pASFPacketWriter
);

static DRM_RESULT UnfragmentableObject_GeneratePayload( 
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __inout ASF_PACKET_WRITER_CONTEXT    * const pASFPacketWriter
);

static DRM_RESULT FragmentableObject_PopUsedSample( 
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __out   ASF_SAMPLE                         **ppUsedSample 
);

static DRM_RESULT UnfragmentableObject_PopUsedSample( 
    __in  ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __out ASF_SAMPLE                         **ppUsedSample 
);

static DRM_RESULT UnfragmentableObject_AddSegment(
    __inout    ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __in       ASF_SAMPLE                   * const pMFSample,
    __in const DRM_DWORD                            cbOffsetIntoSample,
    __out      DRM_DWORD                    * const pcbBytesUsed 
);

/*****************************************************************************
 **
 ** Function:    ASFMuxQueueObject_Initialize
 **
 ** Synopsis:    Initializes the mux queue.  The context initialized with this 
 **              function is then used in subsequent ASFMuxQueueObject calls.
 **
 ** Arguments:    
 **              [pAsfMuxQueueObjectContext] -- mux queue object context
 **              [dwType]                    -- type of object to create
 **              [pMFSample]                 -- sample for this object
 **              [cbOffsetIntoFirstSample]   -- offset into the sample
 **              [cbTargetSize]              -- total size of the sample
 **              [msPresTime]                -- presentation time of the sample
 **              [hnsSendTime]               -- send time of the sample
 **              [wStreamNumber]             -- stream number of the sample
 **              [bObjectID]                 -- object id of the sample
 **              [dwFlags]                   -- flags of the sample
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxQueueObject_Initialize(
    __out      ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __in const DRM_DWORD                            dwType,
    __in       ASF_SAMPLE                   * const pMFSample,
    __in const DRM_DWORD                            cbOffsetIntoFirstSample,
    __in const DRM_DWORD                            cbTargetSize,
    __in const DRM_UINT64                           msPresTime,
    __in const DRM_UINT64                           hnsSendTime,
    __in const DRM_WORD                             wStreamNumber,
    __in const DRM_BYTE                             bObjectID,
    __in const DRM_DWORD                            dwFlags
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_Initialize );

    ChkArg( NULL != pAsfMuxQueueObjectContext );

    pAsfMuxQueueObjectContext->m_msPresTime = msPresTime;
    pAsfMuxQueueObjectContext->m_hnsSendTime = hnsSendTime;
    pAsfMuxQueueObjectContext->m_wStreamNumber = wStreamNumber;
    pAsfMuxQueueObjectContext->m_bObjectID = bObjectID;
    pAsfMuxQueueObjectContext->m_dwFlags = dwFlags;
    pAsfMuxQueueObjectContext->m_fFragmentable = TRUE;
    pAsfMuxQueueObjectContext->m_cbSize = 0;
    pAsfMuxQueueObjectContext->m_cbTotalObjectSize = 0;
    pAsfMuxQueueObjectContext->m_pMFSample = NULL;
    pAsfMuxQueueObjectContext->m_dwType = dwType;
    pAsfMuxQueueObjectContext->m_cRef = 1;

    switch (dwType)
    {
    case ASF_MUX_QUEUE_TYPE_FRAGMENTABLE:
        ChkDR( FragmentableObject_Initialize( pAsfMuxQueueObjectContext,
                                              pMFSample ) );
        break;
    case ASF_MUX_QUEUE_TYPE_UNFRAGMENTABLE:
        ChkDR( UnfragmentableObject_Initialize( pAsfMuxQueueObjectContext,
                                                pMFSample,
                                                cbOffsetIntoFirstSample,
                                                cbTargetSize ) );
        break;
    default:
        ChkDR(DRM_E_NOTIMPL);
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMuxQueueObject_Uninitialize
 **
 ** Synopsis:    Uninitializes the mux queue object.
 **
 ** Arguments:   [pAsfMuxQueueObjectContext] -- mux queue object context
 **
 ** Returns:     void
 **
 *****************************************************************************/
DRM_VOID ASFMuxQueueObject_Uninitialize(
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext
)
{
    if ( NULL != pAsfMuxQueueObjectContext )
    {
        if ( ASF_MUX_QUEUE_TYPE_FRAGMENTABLE == pAsfMuxQueueObjectContext->m_dwType )
        {
            ;
        }
        else if ( ASF_MUX_QUEUE_TYPE_UNFRAGMENTABLE == pAsfMuxQueueObjectContext->m_dwType )
        {
            UnfragmentableObject_Uninitialize(pAsfMuxQueueObjectContext);
        }
    }

    return;
}



/*****************************************************************************
 **
 ** Function:    ASFMuxQueueObject_GeneratePayload
 **
 ** Synopsis:    Based on the type of object this is, redirects the call to the
 **              proper "subclass" function.
 **
 ** Arguments:   [pAsfMuxQueueObjectContext] -- mux queue object context
 **              [pASFPacketWriter]          -- packet writer context
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxQueueObject_GeneratePayload( 
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __inout ASF_PACKET_WRITER_CONTEXT    * const pASFPacketWriter
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_GeneratePayload );

    switch ( pAsfMuxQueueObjectContext->m_dwType )
    {
    case ASF_MUX_QUEUE_TYPE_FRAGMENTABLE:
        ChkDR( FragmentableObject_GeneratePayload( pAsfMuxQueueObjectContext,
                                                   pASFPacketWriter ) );
        break;
    case ASF_MUX_QUEUE_TYPE_UNFRAGMENTABLE:
        ChkDR( UnfragmentableObject_GeneratePayload( pAsfMuxQueueObjectContext,
                                                     pASFPacketWriter ) );
        break;
    default:
        ChkDR(DRM_E_NOTIMPL);
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMuxQueueObject_FillInASFPayloadInfo
 **
 ** Synopsis:    Populates an ASF_PAYLOAD_PARSER_INFO struct for the mux
 **              queue object being processed.
 **
 ** Arguments:   [pAsfMuxQueueObjectContext] -- mux queue object context
 **              [pASFPayloadInfo]           -- payload information
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxQueueObject_FillInASFPayloadInfo(
    __in const ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __out      ASF_PAYLOAD_PARSER_INFO      * const pASFPayloadInfo 
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_FillInASFPayloadInfo );

    ChkArg( pASFPayloadInfo );
    
    ZEROMEM( pASFPayloadInfo, SIZEOF( *pASFPayloadInfo ) );

    /*
     * Set the stuff that's easy to set
     */
    pASFPayloadInfo->bStreamId = (DRM_BYTE) pAsfMuxQueueObjectContext->m_wStreamNumber;
    pASFPayloadInfo->fIsKeyFrame = (pAsfMuxQueueObjectContext->m_dwFlags & MF_SAMPLE_FLAG_CLEANPOINT);
    pASFPayloadInfo->bObjectId = pAsfMuxQueueObjectContext->m_bObjectID;
    pASFPayloadInfo->dwObjectPres = DRM_UI64Low32( pAsfMuxQueueObjectContext->m_msPresTime );
        
 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMuxQueueObject_PopUsedSample
 **
 ** Synopsis:    Based on the type of object this is, redirects the call to the
 **              proper "subclass" function.
 **
 ** Arguments:   [pAsfMuxQueueObjectContext] -- mux queue object context
 **              [ppUsedSample]              -- used sample
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxQueueObject_PopUsedSample( 
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __out   ASF_SAMPLE                         **ppUsedSample
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_PopUsedSample );

    switch ( pAsfMuxQueueObjectContext->m_dwType )
    {
    case ASF_MUX_QUEUE_TYPE_FRAGMENTABLE:
        ChkDR( FragmentableObject_PopUsedSample( pAsfMuxQueueObjectContext,
                                                 ppUsedSample ) );
        break;
    case ASF_MUX_QUEUE_TYPE_UNFRAGMENTABLE:
        ChkDR( UnfragmentableObject_PopUsedSample( pAsfMuxQueueObjectContext,
                                                   ppUsedSample ) );
        break;
    default:
        ChkDR(DRM_E_NOTIMPL);
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMuxQueueObject_AddSegment
 **
 ** Synopsis:    Based on the type of object this is, redirects the call to the
 **              proper "subclass" function.
 **
 ** Arguments:   [pAsfMuxQueueObjectContext] -- mux queue object context
 **              [pMFSample]                 -- sample to add segment from
 **              [cbOffsetIntoSample]        -- offset into the sample
 **              [pcbBytesUsed]              -- number of bytes used by this segment
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxQueueObject_AddSegment( 
    __inout    ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __in       ASF_SAMPLE                   * const pMFSample,
    __in const DRM_DWORD                            cbOffsetIntoSample,
    __out      DRM_DWORD                    * const pcbBytesUsed 
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFMuxQueueObject_AddSegment );

    switch ( pAsfMuxQueueObjectContext->m_dwType )
    {
    case ASF_MUX_QUEUE_TYPE_FRAGMENTABLE:
        /* Fragmentable Objects don't need this function */
        ChkDR(DRM_E_NOTIMPL);
        break;
    case ASF_MUX_QUEUE_TYPE_UNFRAGMENTABLE:
        ChkDR( UnfragmentableObject_AddSegment( pAsfMuxQueueObjectContext,
                                                pMFSample,
                                                cbOffsetIntoSample,
                                                pcbBytesUsed ) );
        break;
    default:
        ChkDR(DRM_E_NOTIMPL);
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMuxQueueObject_AddRef
 **
 ** Synopsis:    Function to track number of references to this
 **              object.  This function must only be used by mux queue objects
 **              created from dynamically allocated memory.
 **
 ** Arguments:   [pAsfMuxQueueObjectContext] -- mux queue object context
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxQueueObject_AddRef(
    __out ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != pAsfMuxQueueObjectContext );

    pAsfMuxQueueObjectContext->m_cRef++;

 ErrorExit:  
    return dr;
}



/*****************************************************************************
 **
 ** Function:    ASFMuxQueueObject_Release
 **
 ** Synopsis:    Smart pointer function to track number of references to this
 **              object.  This function must only be used by mux queue objects
 **              created from dynamically allocated memory.  If the count on
 **              the smart pointer reaches zero, the memory for this mux queue
 **              object is freed.
 **
 ** Arguments:   [pAsfMuxQueueObjectContext] -- mux queue object context
 **
 ** Returns:     void
 **
 *****************************************************************************/
DRM_VOID ASFMuxQueueObject_Release(
    __out ASF_MUX_QUEUE_OBJECT_CONTEXT *pAsfMuxQueueObjectContext
)
{
    if ( NULL != pAsfMuxQueueObjectContext )
    {
        pAsfMuxQueueObjectContext->m_cRef--;

        if ( 0 == pAsfMuxQueueObjectContext->m_cRef )
        {
            ASFMuxQueueObject_Uninitialize( pAsfMuxQueueObjectContext );
            SAFE_OEM_FREE( pAsfMuxQueueObjectContext );
        }
    }
}



/**************************************************************************/
static DRM_RESULT FragmentableObject_GeneratePayload(
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __inout ASF_PACKET_WRITER_CONTEXT    * const pASFPacketWriter
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbMaxPayloadData = 0;
    ASF_PAYLOAD_PARSER_INFO ASFPayloadInfo;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_FragmentableObject_GeneratePayload );

    ChkArg( pASFPacketWriter );

    /*
     * Determine how much space there is for this particular payload
     */
    ChkDR( ASFPacketWriter_GetDataSpaceAvailable( pASFPacketWriter,
                                                  pAsfMuxQueueObjectContext->m_wStreamNumber,
                                                  pAsfMuxQueueObjectContext->m_bObjectID,
                                                  pAsfMuxQueueObjectContext->m_pMFSample,
                                                  &cbMaxPayloadData ) );

    /*
     * Assemble an ASF_PAYLOAD_PARSER_INFO for this payload
     */
    ZEROMEM( &ASFPayloadInfo, SIZEOF( ASFPayloadInfo ) );

    ChkDR( ASFMuxQueueObject_FillInASFPayloadInfo( pAsfMuxQueueObjectContext, &ASFPayloadInfo ) );

    /*
     * Set the ASF_PAYLOAD_PARSER_INFO fields that have to do with the object's
     * representation
     */
    ASFPayloadInfo.dwObjectOffset = pAsfMuxQueueObjectContext->m_cbAlreadyPacketized;
    ASFPayloadInfo.dwObjectSize = pAsfMuxQueueObjectContext->m_pMFSample->cbTotalLength;

    if ( pAsfMuxQueueObjectContext->m_cbSize >= cbMaxPayloadData )
    {
        /*
         * This object is bigger than what we're told there's room for;
         * packetize as much as possible
         */
        ASFPayloadInfo.wTotalDataBytes = (DRM_WORD) cbMaxPayloadData;
    }
    else
    {
        /*
         * There's room for the whole thing
         */
        ASFPayloadInfo.wTotalDataBytes = (DRM_WORD) pAsfMuxQueueObjectContext->m_cbSize;
    }

    /*
     * Add the payload
     */
    ChkDR( ASFPacketWriter_AddPayload( pASFPacketWriter,
                                       &ASFPayloadInfo,
                                       pAsfMuxQueueObjectContext->m_pMFSample,
                                       TRUE ) );

    /*
     * Adjust the size
     */
    pAsfMuxQueueObjectContext->m_cbSize -= ASFPayloadInfo.wTotalDataBytes;
    pAsfMuxQueueObjectContext->m_cbAlreadyPacketized += ASFPayloadInfo.wTotalDataBytes;

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************/
static DRM_RESULT FragmentableObject_PopUsedSample(
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __out   ASF_SAMPLE                         **ppUsedSample 
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_FragmentableObject_PopUsedSample );

    if ( ppUsedSample )
    {
        *ppUsedSample = NULL;
    }

    /*
     * Since we're about to release the sample, we had better
     * be done generating payloads for it
     */
    DRMASSERT( 0 == pAsfMuxQueueObjectContext->m_cbSize );
    ChkBOOL( pAsfMuxQueueObjectContext->m_cbSize == 0, DRM_E_FAIL );
    ChkBOOL( NULL != pAsfMuxQueueObjectContext->m_pMFSample, DRM_E_NOMORE );

    if ( ppUsedSample )
    {
        *ppUsedSample = pAsfMuxQueueObjectContext->m_pMFSample;
        ChkDR( ASFSample_AddRef( pAsfMuxQueueObjectContext->m_pMFSample ) );
    }

    if ( NULL != pAsfMuxQueueObjectContext->m_pMFSample )
    {
        ASFSample_Release( pAsfMuxQueueObjectContext->m_pMFSample );
        pAsfMuxQueueObjectContext->m_pMFSample = NULL;
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************/
static DRM_RESULT UnfragmentableObject_StartMFSample(
    __out      ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __in       ASF_SAMPLE                   * const pMFFirstSample,
    __in const DRM_DWORD                            cbOffsetIntoFirstSample 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbUsed = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_StartMFSample );

    ChkArg( pMFFirstSample );

    /*
     * Create the "internal" MF sample that we will build up ourselves
     */
    pAsfMuxQueueObjectContext->m_pInternalSample = (ASF_SAMPLE *) Oem_MemAlloc(SIZEOF(ASF_SAMPLE));
    ChkMem(pAsfMuxQueueObjectContext->m_pInternalSample);
    ChkDR( ASFSample_Initialize(pAsfMuxQueueObjectContext->m_pInternalSample) );

    pAsfMuxQueueObjectContext->m_pMFSample = pAsfMuxQueueObjectContext->m_pInternalSample;
    ChkDR( ASFSample_AddRef( pAsfMuxQueueObjectContext->m_pMFSample ) );

    pAsfMuxQueueObjectContext->m_pMFSample->bRepData = pMFFirstSample->bRepData;
    MEMCPY( pAsfMuxQueueObjectContext->m_pMFSample->rgbRepData, pMFFirstSample->rgbRepData, pAsfMuxQueueObjectContext->m_pMFSample->bRepData );

    /*
     * Add this sample
     */
    ChkDR( ASFMuxQueueObject_AddSegment( pAsfMuxQueueObjectContext, pMFFirstSample, cbOffsetIntoFirstSample, &cbUsed ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



static DRM_RESULT UnfragmentableObject_Initialize(
    __out      ASF_MUX_QUEUE_OBJECT_CONTEXT *pAsfMuxQueueObjectContext,
    __in       ASF_SAMPLE                   *pMFFirstSample,
    __in const DRM_DWORD                     cbOffsetIntoFirstSample,
    __in const DRM_DWORD                     cbTargetSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_Initialize );

    pAsfMuxQueueObjectContext->m_cbTargetSize = cbTargetSize;
    pAsfMuxQueueObjectContext->m_cbOffsetIntoFirstSample = cbOffsetIntoFirstSample;
    pAsfMuxQueueObjectContext->m_pInternalSample = NULL;

    ChkDR( DRM_PtrList_Initialize( &pAsfMuxQueueObjectContext->m_SamplesToBeFinished, 1 ) );
    ChkDR( UnfragmentableObject_StartMFSample( pAsfMuxQueueObjectContext, pMFFirstSample, cbOffsetIntoFirstSample ) );

    pAsfMuxQueueObjectContext->m_cbTotalObjectSize = pAsfMuxQueueObjectContext->m_cbTargetSize;
    pAsfMuxQueueObjectContext->m_fFragmentable = FALSE;

 ErrorExit:   
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************/
static DRM_VOID UnfragmentableObject_Uninitialize(
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext
)
{
    if ( NULL != pAsfMuxQueueObjectContext )
    {
        if ( NULL != pAsfMuxQueueObjectContext->m_pInternalSample )
        {
            ASFSample_Release( pAsfMuxQueueObjectContext->m_pInternalSample );
            pAsfMuxQueueObjectContext->m_pInternalSample = NULL;
        }

        while( DRM_SUCCEEDED( ASFMuxQueueObject_PopUsedSample( pAsfMuxQueueObjectContext, NULL ) ) )
        {
        }

        DRM_PtrList_Uninitialize( &pAsfMuxQueueObjectContext->m_SamplesToBeFinished );
    }

    return;
}



/**************************************************************************/
static DRM_RESULT UnfragmentableObject_GeneratePayload(
    __inout ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __inout ASF_PACKET_WRITER_CONTEXT    * const pASFPacketWriter
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbMaxPayloadData = 0;
    ASF_PAYLOAD_PARSER_INFO ASFPayloadInfo;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_GeneratePayload );

    /*
     * Determine how much space there is for this particular payload
     */
    ChkDR( ASFPacketWriter_GetDataSpaceAvailable( pASFPacketWriter,
                                                  pAsfMuxQueueObjectContext->m_wStreamNumber,
                                                  pAsfMuxQueueObjectContext->m_bObjectID,
                                                  pAsfMuxQueueObjectContext->m_pMFSample,
                                                  &cbMaxPayloadData ) );

    /*
     * This object forms a payload as an all-or-nothing deal, since
     * it can't be fragmented
     */
    if ( cbMaxPayloadData < pAsfMuxQueueObjectContext->m_cbTargetSize )
    {
        ChkDR( DRM_E_ASF_NOT_ACCEPTING );
    }

    /*
     * Create an ASF_PAYLOAD_PARSER_INFO.  
     */
    ZEROMEM( &ASFPayloadInfo, SIZEOF( ASFPayloadInfo ) );

    ChkDR( ASFMuxQueueObject_FillInASFPayloadInfo( pAsfMuxQueueObjectContext, &ASFPayloadInfo ) );

    /*
     * Fill in the fields that are specific to us
     */
    ASFPayloadInfo.dwObjectOffset = 0;
    ASFPayloadInfo.dwObjectSize = pAsfMuxQueueObjectContext->m_cbTargetSize;
    ASFPayloadInfo.wTotalDataBytes = (DRM_WORD) pAsfMuxQueueObjectContext->m_cbTargetSize;

    ChkDR( ASFPacketWriter_AddPayload( pASFPacketWriter,
                                       &ASFPayloadInfo,
                                       pAsfMuxQueueObjectContext->m_pMFSample,
                                       TRUE ) );

    /*
     * All done with this object
     */
    pAsfMuxQueueObjectContext->m_cbSize = 0;
    if ( NULL != pAsfMuxQueueObjectContext->m_pMFSample )
    {
        ASFSample_Release( pAsfMuxQueueObjectContext->m_pMFSample );
        pAsfMuxQueueObjectContext->m_pMFSample = NULL;
    }

 ErrorExit:

    /*
     * Usually, if we fail this function, it's because there wasn't
     * space for m_pMFSample in the current packet.  In this case,
     * we'll be trying to stick m_pMFSample in a later packet, so we'll 
     * still want it... don't release it!
     */

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************/
static DRM_RESULT UnfragmentableObject_PopUsedSample(
    __in  ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __out ASF_SAMPLE                         **ppUsedSample 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_SAMPLE *pFirstUsedSample = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_PopUsedSample );

    if ( ppUsedSample )
    {
        *ppUsedSample = NULL;
    }

    /*
     * Since we're about to release the sample, we had better
     * be done generating payloads for it
     */
     
    /* The below assert SHOULD be there!
     * We need some code to be able to packetize whatever bytes
     * remain in this sample at the end of encoding.
     */
#if 0
    DRMSSERT( 0 == pAsfMuxQueueObjectContext->m_cbSize );
    ChkBOOL( pAsfMuxQueueObjectContext->m_cbSize == 0, DRM_E_FAIL );
#endif

    if ( DRM_SUCCEEDED( DRM_PtrList_RemoveHead( &pAsfMuxQueueObjectContext->m_SamplesToBeFinished, (DRM_VOID **) &pFirstUsedSample ) ) )
    {
        if ( ppUsedSample )
        {
            *ppUsedSample = pFirstUsedSample;
        }
        else
        {
            if ( NULL != pFirstUsedSample )
            {
                ASFSample_Release( pFirstUsedSample );
                pFirstUsedSample = NULL;
            }
        }
    }
    else
    {
        ChkDR( DRM_E_FAIL );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**************************************************************************/
static DRM_RESULT UnfragmentableObject_AddSegment(
    __inout    ASF_MUX_QUEUE_OBJECT_CONTEXT * const pAsfMuxQueueObjectContext,
    __in       ASF_SAMPLE                   * const pMFSample,
    __in const DRM_DWORD                            cbOffsetIntoSample,
    __out      DRM_DWORD                    * const pcbBytesUsed 
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_MEDIA_BUFFER *pMFBuffer = NULL;
    DRM_DWORD cbSampleSize = 0;
    DRM_DWORD cBuffers = 0;
    DRM_DWORD cbSampleBytesSeen = 0;
    DRM_DWORD i = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_UnfragmentableObject_AddSegment );

    ChkArg( pMFSample );
    ChkArg( pcbBytesUsed );

    *pcbBytesUsed = 0;

    if ( pAsfMuxQueueObjectContext->m_cbSize >= pAsfMuxQueueObjectContext->m_cbTargetSize )
    {
        ChkDR( DRM_E_ASF_NOT_ACCEPTING );
    }

    cbSampleSize = pMFSample->cbTotalLength;

    ChkArg( cbOffsetIntoSample <= cbSampleSize );

    /*
     * Go tdrough the buffers of the input sample and 
     * add the appropriate ones to our sample
     */
    ChkDR( ASFSample_GetBufferCount( pMFSample, &cBuffers ) );

    for ( i = 0; (i < cBuffers) && (pAsfMuxQueueObjectContext->m_cbSize < pAsfMuxQueueObjectContext->m_cbTargetSize); i++ )
    {
        DRM_DWORD cbBuffer = 0;

        ChkDR( ASFSample_GetBufferByIndex( pMFSample, i, &pMFBuffer ) );

        ChkFAIL( pMFBuffer != NULL );

        cbBuffer = pMFBuffer->blobBuffer.cbBlob;

        if ( cbOffsetIntoSample < (cbSampleBytesSeen + cbBuffer) )
        {
            /*
             * Part or all of this buffer needs to be added
             */
            DRM_DWORD cbOffsetIntoBuffer = 0;
            DRM_BOOL fWillUseUpToEndOfSample;
            DRM_DWORD cbBufferBytesToAdd;

            if ( cbSampleBytesSeen < cbOffsetIntoSample )
            {
                cbOffsetIntoBuffer = cbOffsetIntoSample - cbSampleBytesSeen;
            }

            fWillUseUpToEndOfSample = FALSE;
            cbBufferBytesToAdd = cbBuffer - cbOffsetIntoBuffer;
            if ( (pAsfMuxQueueObjectContext->m_cbSize + cbBufferBytesToAdd) >= pAsfMuxQueueObjectContext->m_cbTargetSize )
            {
                fWillUseUpToEndOfSample = TRUE;
            }

            if ( (pAsfMuxQueueObjectContext->m_cbSize + cbBufferBytesToAdd) > pAsfMuxQueueObjectContext->m_cbTargetSize )
            {
                cbBufferBytesToAdd = pAsfMuxQueueObjectContext->m_cbTargetSize - pAsfMuxQueueObjectContext->m_cbSize;
            }

            /*
             * If the whole thing won't fit, save the offset into the buffer
             */
            if (( cbOffsetIntoBuffer > 0 ) || ( cbBufferBytesToAdd < cbBuffer ))
            {
                pMFBuffer->cbCurrentOffset = cbOffsetIntoBuffer;
            }

            ChkDR( ASFSample_AddBuffer( pAsfMuxQueueObjectContext->m_pInternalSample, pMFBuffer ) );

            pAsfMuxQueueObjectContext->m_cbSize += cbBufferBytesToAdd;
            DRMASSERT( pAsfMuxQueueObjectContext->m_cbSize <= pAsfMuxQueueObjectContext->m_cbTargetSize );

            *pcbBytesUsed += cbBufferBytesToAdd;

            ASFMediaBuffer_Release( pMFBuffer );
            pMFBuffer = NULL;

            if ( fWillUseUpToEndOfSample )
            {
                /*
                 * Put this on the list of samples that will
                 * be finished at the end
                 */
                if ( DRM_SUCCEEDED( DRM_PtrList_AddTail( &pAsfMuxQueueObjectContext->m_SamplesToBeFinished, pMFSample ) ) )
                {
                    ChkDR( ASFSample_AddRef( pMFSample ) );
                }
            }
        }

    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}




#endif /* DRM_SUPPORT_AUDIO_ONLY == 0 */

EXIT_PK_NAMESPACE_CODE;
