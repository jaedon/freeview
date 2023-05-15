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
#include <muxstream.h>
#include <ndtasfguids.h>
#include <leakybucket.h>

ENTER_PK_NAMESPACE_CODE;

#if DRM_SUPPORT_AUDIO_ONLY == 0

#define MUX_STREAM_BASE    0
#define MUX_STREAM_AUDIO   1
#define MUX_STREAM_VIDEO   2

static DRM_RESULT InternalInit(
    __out      ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_STREAM_INFO        * const pIStreamConfig
);

static DRM_RESULT SetupLeakyBucket(
    __inout ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext
);

static DRM_RESULT ProcessNewWriterSample(
    __inout    ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __in const DRM_UINT64                     hnsTimestampAdjust,
    __out      DRM_UINT64             * const phnsSendTime,
    __out      DRM_DWORD              * const pdwObjectID,
    __inout    DRM_DWORD                     *pdwFlags
);

static DRM_RESULT SendTimeFromPresTime(
    __inout    ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const DRM_UINT64                     hnsPresTime,
    __in const DRM_DWORD                      cbObjectSize,
    __out      DRM_UINT64             * const phnsSendTime
);

static DRM_DWORD AssignObjectID(
    __out      ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const DRM_DWORD                      dwSampleFlags
);

static DRM_RESULT UpdateStats(
    __out      ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const DRM_UINT64                     hnsPresTime,
    __in const DRM_DWORD                      cbObjectSize
);

static DRM_RESULT GetAdjustedSampleTime(
    __in const ASF_SAMPLE * const pISample,
    __in const DRM_UINT64         hnsTimestampAdjust,
    __out      DRM_UINT64 * const phnsAdjustedTime
);


/******************************************************************************/
/* AUDIO function prototypes */
static DRM_RESULT ASFAudioMuxStream_Initialize(
    __out ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in  ASF_STREAM_INFO        * const pIStreamConfig
);

static DRM_RESULT ASFAudioMuxStream_HowManyObjectsWillBeReturned(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __out      DRM_DWORD              * const pcObjects
);

static DRM_RESULT ASFAudioMuxStream_OnSample(
    __inout    ASF_MUX_STREAM_CONTEXT * const f_pAsfMuxStreamContext,
    __in       ASF_SAMPLE             * const f_pISample,
    __in const DRM_UINT64                     f_hnsTimestampAdjust,
    __in       DRM_DWORD                      f_dwFlags,
    __inout_ecount( *f_pcQueueObjects )
               ASF_MUX_QUEUE_OBJECT_CONTEXT **f_ppMuxQueueObjects,
    __inout    DRM_DWORD              * const f_pcQueueObjects
);

static DRM_RESULT ASFAudioMuxStream_ApproxSampleDuration(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __out      DRM_UINT64             * const pmsDuration
);

static DRM_RESULT ASFAudioMuxStream_GetHappyCutoffSendtime(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __out      DRM_UINT64             * const phnsNextSendtime
);

static DRM_VOID ASFAudioMuxStream_Cleanup(
    __inout ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext
);

static DRM_RESULT ASFAudioMuxStream_ReadMediaType(
    __out ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in  ASF_STREAM_INFO        * const pIStreamConfig
);


/******************************************************************************/
/* VIDEO function prototypes */
static DRM_RESULT ASFVideoMuxStream_Initialize(
    __out  ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in   ASF_STREAM_INFO        * const pIStreamConfig
);

static DRM_RESULT ASFVideoMuxStream_OnSample(
    __inout       ASF_MUX_STREAM_CONTEXT * const f_pAsfMuxStreamContext,
    __in          ASF_SAMPLE             * const f_pISample,
    __in    const DRM_UINT64                     f_hnsTimestampAdjust,
    __in    const DRM_DWORD                      f_dwFlags,
    __out_ecount( *f_pcQueueObjects )
                  ASF_MUX_QUEUE_OBJECT_CONTEXT **f_ppMuxQueueObjects,
    __out         DRM_DWORD              * const f_pcQueueObjects
);

static DRM_RESULT ASFVideoMuxStream_ApproxSampleDuration(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __out      DRM_UINT64                    *pmsDuration
);

static DRM_RESULT ASFVideoMuxStream_GetHappyCutoffSendtime(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __out      DRM_UINT64             * const phnsNextSendtime
);

static DRM_VOID ASFVideoMuxStream_Cleanup(
    __inout_opt ASF_MUX_STREAM_CONTEXT * const f_pAsfMuxStreamContext
);

static DRM_RESULT ASFVideoMuxStream_ReadMediaType(
    __out      ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_STREAM_INFO        * const pIStreamConfig
 );


/*****************************************************************************
 * This does some things
 *      1. Assigns the sample a send time (this does a bandwidth check
 *         and determines the next time it can send )
 *      2. Sets the object ID.
 *      3. Creates a ASF_MUX_QUEUE_OBJECT_CONTEXT so that it can wait to be sent.
 *****************************************************************************/
static DRM_RESULT OnSample(
    __inout       ASF_MUX_STREAM_CONTEXT * const f_pAsfMuxStreamContext,
    __in          ASF_SAMPLE             * const f_pISample,
    __in    const DRM_UINT64                     f_hnsTimestampAdjust,
    __in          DRM_DWORD                      f_dwFlags,
    __out_ecount( *f_pcQueueObjects )
                  ASF_MUX_QUEUE_OBJECT_CONTEXT **f_ppMuxQueueObject,
    __out         DRM_DWORD              * const f_pcQueueObjects
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fFreeable = FALSE;
    DRM_UINT64 hnsPresTime = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 msPresTime;
    DRM_UINT64 hnsSendTime = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD dwObjectID = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_OnSample );

    DRMASSERT( NULL != f_pAsfMuxStreamContext );
    DRMASSERT( NULL != f_pISample );
    DRMASSERT( NULL != f_ppMuxQueueObject );
    DRMASSERT( NULL != f_pcQueueObjects );

    *f_ppMuxQueueObject = NULL;
    fFreeable = TRUE;
    *f_pcQueueObjects = 0;

    /*
     * Process this new sample and get its vital stats
     */
    ChkDR( GetAdjustedSampleTime( f_pISample, f_hnsTimestampAdjust, &hnsPresTime ) );

    msPresTime = DRM_UI64Add( DRM_UI64Div( hnsPresTime,
                                           DRM_UI64( 10000 ) ),
                              f_pAsfMuxStreamContext->m_msPreroll );

    hnsSendTime = DRM_UI64( 0 );
    dwObjectID = 0;
    ChkDR( ProcessNewWriterSample( f_pAsfMuxStreamContext,
                                   f_pISample,
                                   f_hnsTimestampAdjust,
                                   &hnsSendTime,
                                   &dwObjectID,
                                   &f_dwFlags ) );

    DRMASSERT( dwObjectID <= 255 );

    /*
     * Step 3: Wrap it in an object.
     * For fragmentable objects like this one, we'll always place the media object
     * in exactly one queue object.
     */
    *f_ppMuxQueueObject = (ASF_MUX_QUEUE_OBJECT_CONTEXT *) Oem_MemAlloc( SIZEOF( ASF_MUX_QUEUE_OBJECT_CONTEXT ) );
    ChkMem( *f_ppMuxQueueObject );
    ZEROMEM( *f_ppMuxQueueObject, SIZEOF( ASF_MUX_QUEUE_OBJECT_CONTEXT ) );
    ChkDR( ASFMuxQueueObject_Initialize( *f_ppMuxQueueObject,
                                         ASF_MUX_QUEUE_TYPE_FRAGMENTABLE,
                                         f_pISample,
                                         0, /* this isn't used for fragmentable object initialization */
                                         0, /* this isn't used for fragmentable object initialization */
                                         msPresTime,
                                         hnsSendTime,
                                         f_pAsfMuxStreamContext->m_wStreamNumber,
                                         (DRM_BYTE) dwObjectID,
                                         f_dwFlags ) );

    *f_pcQueueObjects = 1;

 ErrorExit:
    if ( DRM_FAILED( dr ) && fFreeable )
    {
        ASFMuxQueueObject_Uninitialize( *f_ppMuxQueueObject );
        SAFE_OEM_FREE( *f_ppMuxQueueObject );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT ApproxSampleDuration(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __out DRM_UINT64                  * const pmsDuration
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbSample = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ApproxSampleDuration );

    ChkArg( NULL != pAsfMuxStreamContext );
    ChkArg( NULL != pmsDuration );

    /*
     * Absent other information, we'll just make a guess based on bitrate and
     * how long it would take to stuff this object down the pipe
     */
    if ( !pISample )
    {
        DRMASSERT( FALSE );
        *pmsDuration = DRM_UI64( 0 );
        goto ErrorExit;
    }

    cbSample = pISample->cbTotalLength;

    DRMASSERT( 0 != pAsfMuxStreamContext->m_dwMaxBitrate );
    *pmsDuration = DRM_UI64Div( DRM_UI64Mul( DRM_UI64( cbSample ),
                                           DRM_UI64( 8 * 1000 ) ),
                              DRM_UI64( pAsfMuxStreamContext->m_dwMaxBitrate ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT GetHappyCutoffSendtime(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __out      DRM_UINT64             * const phnsNextSendtime
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_GetHappyCutoffSendtime );

    MUX_TRACE(("ASFMuxStream_GetHappyCutoffSendtime"));

    ChkArg( NULL != pAsfMuxStreamContext );
    ChkArg( NULL != phnsNextSendtime );

    *phnsNextSendtime = DRM_UI64( 0 );

    /*
     * For fragmentable objects, since we're 1-in-1-out in the OnSample()
     * call, whatever sendtime we're going to assign next is a good cutoff,
     * since we'll never assign something an earlier sendtime
     */
    if( pAsfMuxStreamContext->m_pLeakyBucket )
    {
        *phnsNextSendtime = pAsfMuxStreamContext->m_pLeakyBucket->m_hnsNextLeakTime;

        MUX_TRACE(("Leaky bucket will assign %lu as its next send time",
                   DRM_UI64Low32( *phnsNextSendtime ) ));
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}   /* ASFMuxStream_GetHappyCutoffSendtime */



/******************************************************************************/
static DRM_RESULT AdjustSendTimeForOffset(
    __in const ASF_MUX_STREAM_CONTEXT       * const pAsfMuxStreamContext,
    __in const DRM_DWORD                            cbOriginalObjectSize,
    __inout    ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbBytesSent;
    DRM_UINT64 hnsSamplePresTime;
    DRM_UINT64 hnsNewSendTime;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_AdjustSendTimeForOffset );

    /*
     * This base class implementation is for the benefit of "generic" streams
     * that may have enormous objects (very bursty) that get fragmented many
     * times.  For these streams, we don't want loads of packets with the same
     * sendtime; rather, we want to push the send time ahead for each packet.
     *
     * Video streams wouldn't want to do something like this.
     */
    ChkArg( pAsfMuxStreamContext );
    ChkArg( pQueueObject );
    ChkArg( pQueueObject->m_wStreamNumber == pAsfMuxStreamContext->m_wStreamNumber );

    /*
     * Validate the size arg
     */
    ChkArg( cbOriginalObjectSize >= pQueueObject->m_cbSize );

    cbBytesSent = cbOriginalObjectSize - pQueueObject->m_cbSize;

    /*
     * Use the bitrate to push the sample's sendtime up, not to exceed its
     * presentation time, of course.
     * This only makes sense for non-VBR streams, since for VBR we can't
     * really rely on the bitrate we have.
     */
    hnsSamplePresTime = DRM_UI64Mul( DRM_UI64( 10000 ), pQueueObject->m_msPresTime );
    hnsNewSendTime = pQueueObject->m_hnsSendTime;

    if ( pAsfMuxStreamContext->m_pLeakyBucket )
    {
        DRM_DWORD dwLeakyBucketBitrate = 0;
        dwLeakyBucketBitrate = pAsfMuxStreamContext->m_pLeakyBucket->m_dwBitrate;
        DRMASSERT( 0 != dwLeakyBucketBitrate );
        /* hnsNewSendTime += (1000 * 10000 * (DRM_UINT64) cbBytesSent * 8 / (DRM_UINT64) dwLeakyBucketBitrate); */
        hnsNewSendTime = DRM_UI64Add( hnsNewSendTime,
                                      DRM_UI64Div( DRM_UI64Mul( DRM_UI64( 1000L * 10000L * 8L ),
                                                                DRM_UI64( cbBytesSent ) ),
                                                   DRM_UI64( dwLeakyBucketBitrate ) ) );
    }

    /* if ( hnsNewSendTime <= hnsSamplePresTime ) */
    if ( DRM_UI64Les( hnsNewSendTime, hnsSamplePresTime ) ||
         DRM_UI64Eql( hnsNewSendTime, hnsSamplePresTime ) )
    {
        pQueueObject->m_hnsSendTime = hnsNewSendTime;
    }
    else
    {
        /* Aw, phooey */
        pQueueObject->m_hnsSendTime = hnsSamplePresTime;
    }

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}   /* ASFMuxStream_AdjustSendTimeForOffset */



/******************************************************************************
 **
 ** Function:    ASFMuxStream_Initialize
 **
 ** Synopsis:    Initializes the mux stream.  The context initialized with
 **              this functionis then used in subsequent ASFMuxStream calls.
 **
 ** Arguments:   [pAsfMuxStreamContext] -- context
 **              [pIStreamConfig]       -- configuration data of this stream
 **              [msPreroll]            -- preroll
 **              [hnsFirstSendTime]     -- iniitial send time of this stream
 **              [pMuxContext]          -- backpointer to the mux
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during processing
 **
 *****************************************************************************/
DRM_RESULT ASFMuxStream_Initialize(
    __out      ASF_MUX_STREAM_CONTEXT              * const pAsfMuxStreamContext,
    __in       ASF_STREAM_INFO                     * const pIStreamConfig,
    __in const DRM_UINT64                                  msPreroll,
    __in const DRM_UINT64                                  hnsFirstSendTime,
    __in       struct __tagASF_MULTIPLEXER_CONTEXT * const pMuxContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_Initialize );

    pAsfMuxStreamContext->m_msPreroll = msPreroll;
    pAsfMuxStreamContext->m_pMux = pMuxContext;
    pAsfMuxStreamContext->m_wStreamNumber = 0;
    pAsfMuxStreamContext->m_dwMaxBitrate = 0;
    pAsfMuxStreamContext->m_msMaxBufferWindow = 0;
    pAsfMuxStreamContext->m_fAllObjectsAreCleanPoints = FALSE;
    pAsfMuxStreamContext->m_dwNextObjectID = 0;
    pAsfMuxStreamContext->m_hnsFirstSendTime = hnsFirstSendTime;
    pAsfMuxStreamContext->m_msFirstPresTimeSeen = DRM_UI64( 0 );
    pAsfMuxStreamContext->m_msLatestPresTimeSeen = DRM_UI64( 0 );
    pAsfMuxStreamContext->m_msLatestDurationSeen = DRM_UI64( 0 );
    pAsfMuxStreamContext->m_cFrames = DRM_UI64( 0 );
    pAsfMuxStreamContext->m_cbFrameData = DRM_UI64( 0 );
    pAsfMuxStreamContext->m_cbLargestObjectSeen = 0;
    pAsfMuxStreamContext->m_pLeakyBucket = NULL;

    ChkDR( InternalInit( pAsfMuxStreamContext, pIStreamConfig ) );

    if ( 0 == MEMCMP(&pIStreamConfig->guidStreamType, &CLSID_ASFStreamTypeAudioMedia, SIZEOF(DRM_GUID)) )
    {
        MUX_TRACE(("Creating AUDIO Mux Stream \n"));
        pAsfMuxStreamContext->m_Type = MUX_STREAM_AUDIO;
        ChkDR( ASFAudioMuxStream_Initialize( pAsfMuxStreamContext, pIStreamConfig ) );
    }
    else if ( 0 == MEMCMP(&pIStreamConfig->guidStreamType, &CLSID_ASFStreamTypeVideoMedia, SIZEOF(DRM_GUID)) )
    {
        MUX_TRACE(("Creating VIDEO Mux Stream \n"));
        pAsfMuxStreamContext->m_Type = MUX_STREAM_VIDEO;
        ChkDR( ASFVideoMuxStream_Initialize( pAsfMuxStreamContext, pIStreamConfig ) );
    }
    else
    {
        MUX_TRACE(("Creating GENERIC Mux Stream \n"));
        pAsfMuxStreamContext->m_Type = MUX_STREAM_BASE;
        ChkDR( ASFMuxStream_ReadMediaType( pAsfMuxStreamContext, pIStreamConfig ) );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
 **
 ** Function:    ASFMuxStream_Uninitialize
 **
 ** Synopsis:    Uninitializes the mux stream.
 **
 ** Arguments:   [pAsfMuxStreamContext] -- context
 **
 ** Returns:     N/A
 **
 *****************************************************************************/
DRM_VOID ASFMuxStream_Uninitialize(
    __inout ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext
)
{
    if ( NULL != pAsfMuxStreamContext )
    {
        ASFMuxStream_Cleanup(pAsfMuxStreamContext);
        SAFE_OEM_FREE( pAsfMuxStreamContext->m_pLeakyBucket );
    }

    return;
}



/******************************************************************************
 **
 ** Function:    ASFMuxStream_HowManyObjectsWillBeReturned
 **
 ** Synopsis:    Based on the type of stream this is, redirects the call to the
 **              proper "subclass".
 **
 ** Arguments:   [pAsfMuxStreamContext] -- context
 **              [pSample]              -- sample
 **              [pcObjects]            -- number of objects
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 *****************************************************************************/
DRM_RESULT ASFMuxStream_HowManyObjectsWillBeReturned(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pSample,
    __out      DRM_DWORD              * const pcObjects
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_HowManyObjectsWillBeReturned );

    ChkArg( NULL != pAsfMuxStreamContext );

    switch( pAsfMuxStreamContext->m_Type )
    {
    case MUX_STREAM_AUDIO:
        ChkDR( ASFAudioMuxStream_HowManyObjectsWillBeReturned( pAsfMuxStreamContext, pSample, pcObjects ) );
        break;
    case MUX_STREAM_VIDEO:
        /* Fragmentable objects: Always exactly 1 */
        *pcObjects = 1;
        break;
    case MUX_STREAM_BASE:
        /* Fragmentable objects: Always exactly 1 */
        *pcObjects = 1;
        break;
    default:
        ChkDR( DRM_E_NOTIMPL );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
 **
 ** Function:    ASFMuxStream_OnSample
 **
 ** Synopsis:    Based on the type of stream this is, redirects the call to the
 **              proper "subclass".
 **
 ** Arguments:   [f_pAsfMuxStreamContext] -- context
 **              [f_pISample]             -- sample
 **              [f_hnsTimestampAdjust]   -- adjusted sample time
 **              [f_dwFlags]              -- sample flags
 **              [f_ppMuxQueueObject]     -- mux queue object created for this sample
 **              [f_pcQueueObjects]       -- number of queue objects
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 *****************************************************************************/
DRM_RESULT ASFMuxStream_OnSample(
    __inout       ASF_MUX_STREAM_CONTEXT       * const f_pAsfMuxStreamContext,
    __in          ASF_SAMPLE                   * const f_pISample,
    __in    const DRM_UINT64                           f_hnsTimestampAdjust,
    __in    const DRM_DWORD                            f_dwFlags,
    __inout_ecount( *f_pcQueueObjects )
                  ASF_MUX_QUEUE_OBJECT_CONTEXT       **f_ppMuxQueueObject,
    __inout       DRM_DWORD                    * const f_pcQueueObjects
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_OnSample );

    ChkArg( NULL != f_pAsfMuxStreamContext );
    ChkArg( NULL != f_pISample );
    ChkArg( NULL != f_ppMuxQueueObject );
    ChkArg( NULL != f_pcQueueObjects );

    switch( f_pAsfMuxStreamContext->m_Type )
    {
    case MUX_STREAM_AUDIO:
        ChkDR( ASFAudioMuxStream_OnSample( f_pAsfMuxStreamContext,
                                           f_pISample,
                                           f_hnsTimestampAdjust,
                                           f_dwFlags,
                                           f_ppMuxQueueObject,
                                           f_pcQueueObjects ) );
        break;
    case MUX_STREAM_VIDEO:
        ChkDR( ASFVideoMuxStream_OnSample( f_pAsfMuxStreamContext,
                                           f_pISample,
                                           f_hnsTimestampAdjust,
                                           f_dwFlags,
                                           f_ppMuxQueueObject,
                                           f_pcQueueObjects ) );
        break;
    case MUX_STREAM_BASE:
        ChkDR( OnSample( f_pAsfMuxStreamContext,
                         f_pISample,
                         f_hnsTimestampAdjust,
                         f_dwFlags,
                         f_ppMuxQueueObject,
                         f_pcQueueObjects ) );
        break;
    default:
        /* This should never happen */
        DRMASSERT( FALSE );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
 **
 ** Function:    ASFMuxStream_GetObjectSizeForBandwidthPolicing
 **
 ** Synopsis:
 **
 ** Arguments:   [pAsfMuxStreamContext] -- context
 **              [cbObjectSize]         -- original object size
 **              [pcbNewObjectSize]     -- updated object size
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 *****************************************************************************/
DRM_RESULT ASFMuxStream_GetObjectSizeForBandwidthPolicing(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const DRM_DWORD                      cbObjectSize,
    __out      DRM_DWORD              * const pcbNewObjectSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_GetObjectSizeForBandwidthPolicing );

    ChkArg( NULL != pAsfMuxStreamContext );

    switch( pAsfMuxStreamContext->m_Type )
    {
    case MUX_STREAM_AUDIO:
        *pcbNewObjectSize = cbObjectSize;
        break;
    case MUX_STREAM_VIDEO:
        /*
         * For video, we include the 17-byte payload overhead for each object
         * when counting up bandwidth usage
         */
        *pcbNewObjectSize = cbObjectSize + PAYLOAD_OVERHEAD;
        break;
    case MUX_STREAM_BASE:
        *pcbNewObjectSize = cbObjectSize;
        break;
    default:
        ChkDR( DRM_E_NOTIMPL );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
 **
 ** Function:    ASFMuxStream_ApproxSampleDuration
 **
 ** Synopsis:    Based on the type of stream this is, redirects the call to the
 **              proper "subclass".
 **
 ** Arguments:   [pAsfMuxStreamContext] -- context
 **              [pISample]             -- sample
 **              [pmsDuration]          -- approximate duration of sample
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 *****************************************************************************/
DRM_RESULT ASFMuxStream_ApproxSampleDuration(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __out      DRM_UINT64             * const pmsDuration
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_ApproxSampleDuration );

    ChkArg( NULL != pAsfMuxStreamContext );

    switch( pAsfMuxStreamContext->m_Type )
    {
    case MUX_STREAM_AUDIO:
        ChkDR( ASFAudioMuxStream_ApproxSampleDuration( pAsfMuxStreamContext,
                                                       pISample,
                                                       pmsDuration ) );
        break;
    case MUX_STREAM_VIDEO:
        ChkDR( ASFVideoMuxStream_ApproxSampleDuration( pAsfMuxStreamContext,
                                                       pISample,
                                                       pmsDuration ) );
        break;
    case MUX_STREAM_BASE:
        ChkDR( ApproxSampleDuration( pAsfMuxStreamContext,
                                     pISample,
                                     pmsDuration ) );
        break;
    default:
        ChkDR( DRM_E_NOTIMPL );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
 **
 ** Function:    ASFMuxStream_GetHappyCutoffSendtime
 **
 ** Synopsis:    Based on the type of stream this is, redirects the call to the
 **              proper "subclass".
 **
 ** Arguments:   [pAsfMuxStreamContext] -- context
 **              [phnsNextSendtime]     -- next send time of this stream
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 *****************************************************************************/
DRM_RESULT ASFMuxStream_GetHappyCutoffSendtime(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __out      DRM_UINT64             * const phnsNextSendtime
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_GetHappyCutoffSendtime );

    ChkArg( NULL != pAsfMuxStreamContext );

    switch( pAsfMuxStreamContext->m_Type )
    {
    case MUX_STREAM_AUDIO:
        ChkDR( ASFAudioMuxStream_GetHappyCutoffSendtime( pAsfMuxStreamContext, phnsNextSendtime ) );
        break;
    case MUX_STREAM_VIDEO:
        ChkDR( ASFVideoMuxStream_GetHappyCutoffSendtime( pAsfMuxStreamContext, phnsNextSendtime ) );
        break;
    case MUX_STREAM_BASE:
        ChkDR( GetHappyCutoffSendtime( pAsfMuxStreamContext, phnsNextSendtime ) );
        break;
    default:
        ChkDR( DRM_E_NOTIMPL );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
 **
 ** Function:    ASFMuxStream_AdjustSendTimeForOffset
 **
 ** Synopsis:    Based on the type of stream this is, redirects the call to the
 **              proper "subclass".
 **
 ** Arguments:   [pAsfMuxStreamContext] -- context
 **              [cbOriginalObjectSize] -- original object size
 **              [pQueueObject]         -- queue object of this stream
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 *****************************************************************************/
DRM_RESULT ASFMuxStream_AdjustSendTimeForOffset(
    __in const ASF_MUX_STREAM_CONTEXT       * const pAsfMuxStreamContext,
    __in const DRM_DWORD                            cbOriginalObjectSize,
    __out      ASF_MUX_QUEUE_OBJECT_CONTEXT * const pQueueObject
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_AdjustSendTimeForOffset );

    ChkArg( NULL != pAsfMuxStreamContext );

    switch( pAsfMuxStreamContext->m_Type )
    {
    case MUX_STREAM_AUDIO:
        /* Not overridden by subclass */
        ChkDR( AdjustSendTimeForOffset( pAsfMuxStreamContext,
                                        cbOriginalObjectSize,
                                        pQueueObject ) );
        break;
    case MUX_STREAM_VIDEO:
        /*
         * No need to do anything: Video frames tend to be smallish in general
         * and it's okay not to bump the sendtime up.  In fact, since video
         * tends to operate close to its buffer window allowance, it's a good
         * idea not to do this, since you could end up having the sendtime for a
         * fragment of object 59 go past the sendtime for the first fragment of
         * object 60.  Not a big deal, but hey.
         */
        break;
    case MUX_STREAM_BASE:
        ChkDR( AdjustSendTimeForOffset( pAsfMuxStreamContext,
                                        cbOriginalObjectSize,
                                        pQueueObject ) );
        break;
    default:
        ChkDR( DRM_E_NOTIMPL );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
 **
 ** Function:    ASFMuxStream_Cleanup
 **
 ** Synopsis:    Based on the type of stream this is, redirects the call to the
 **              proper "subclass".
 **
 ** Arguments:
 **              [pAsfMuxStreamContext] -- context
 **
 ** Returns:     N/A
 **
 *****************************************************************************/
DRM_VOID ASFMuxStream_Cleanup(
    __inout ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext
)
{
    if ( NULL != pAsfMuxStreamContext )
    {
        switch( pAsfMuxStreamContext->m_Type )
        {
        case MUX_STREAM_AUDIO:
            ASFAudioMuxStream_Cleanup( pAsfMuxStreamContext );
            break;
        case MUX_STREAM_VIDEO:
            ASFVideoMuxStream_Cleanup( pAsfMuxStreamContext );
            break;
        case MUX_STREAM_BASE:
            break;
        default:
            /* This should never happen */
            DRMASSERT( FALSE );
        }
    }
}



/******************************************************************************
 **
 ** Function:    ASFMuxStream_ReadMediaType
 **
 ** Synopsis:    Based on the type of stream this is, redirects the call to the
 **              proper "subclass".
 **
 ** Arguments:   [pAsfMuxStreamContext] -- context
 **              [pIStreamConfig]       -- stream configuration of this stream
 **
 ** Returns:     DRM_SUCCESS on success or
 **              DRM_E_INVALIDARG if any of the arguments are invalid or
 **              passes up an error code from a function called during initialization
 **
 *****************************************************************************/
DRM_RESULT ASFMuxStream_ReadMediaType(
    __inout ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in    ASF_STREAM_INFO        * const pIStreamConfig
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFMuxStream_ReadMediaType );

    if ( NULL != pAsfMuxStreamContext )
    {
        switch( pAsfMuxStreamContext->m_Type )
        {
        case MUX_STREAM_AUDIO:
            ChkDR( ASFAudioMuxStream_ReadMediaType( pAsfMuxStreamContext, pIStreamConfig ) );
            break;
        case MUX_STREAM_VIDEO:
            ChkDR( ASFVideoMuxStream_ReadMediaType( pAsfMuxStreamContext, pIStreamConfig ) );
            break;
        case MUX_STREAM_BASE:
            break;
        default:
            /* This should never happen */
            DRMASSERT( FALSE );
        }
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT InternalInit(
    __out      ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_STREAM_INFO        * const pIStreamConfig
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fBandwidthKnown = FALSE;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_InternalInit );

    ChkArg( NULL != pIStreamConfig );

    pAsfMuxStreamContext->m_wStreamNumber = pIStreamConfig->wStreamNumber;

    if ( (0 != pIStreamConfig->dwMaxBitrate)
         && (0 != pIStreamConfig->msMaxBufferWindow ) )
    {
        pAsfMuxStreamContext->m_dwMaxBitrate      = pIStreamConfig->dwMaxBitrate;
        pAsfMuxStreamContext->m_msMaxBufferWindow = pIStreamConfig->msMaxBufferWindow;
        fBandwidthKnown = TRUE;
    }

    if ( fBandwidthKnown &&
         ( pAsfMuxStreamContext->m_msMaxBufferWindow > 0 ) &&
         ( pAsfMuxStreamContext->m_dwMaxBitrate > 0 ) )
    {
        ChkDR( SetupLeakyBucket( pAsfMuxStreamContext ) );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr ;
}



/******************************************************************************/
static DRM_RESULT SetupLeakyBucket(
    __inout ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwMuxFlags = 0;
    DRM_DWORD dwFlags = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_SetupLeakyBucket );

    DRMASSERT( NULL == pAsfMuxStreamContext->m_pLeakyBucket );
    SAFE_OEM_FREE( pAsfMuxStreamContext->m_pLeakyBucket );

    pAsfMuxStreamContext->m_pLeakyBucket = (ASF_LEAKY_BUCKET_CONTEXT *) Oem_MemAlloc(SIZEOF(ASF_LEAKY_BUCKET_CONTEXT));
    ChkMem(pAsfMuxStreamContext->m_pLeakyBucket);
    ChkDR( ASFLeakyBucket_Initialize( pAsfMuxStreamContext->m_pLeakyBucket,
                                      pAsfMuxStreamContext->m_dwMaxBitrate,
                                      DRM_UI64Mul( DRM_UI64( pAsfMuxStreamContext->m_msMaxBufferWindow ),
                                                   DRM_UI64( 10000 ) ) ) );

    pAsfMuxStreamContext->m_pLeakyBucket->m_hnsNextLeakTime = pAsfMuxStreamContext->m_hnsFirstSendTime;

    dwFlags = pAsfMuxStreamContext->m_pLeakyBucket->m_dwFlags;
    dwMuxFlags = pAsfMuxStreamContext->m_pMux->m_dwFlags;

    if( MFASF_MULTIPLEXER_AUTOADJUST_BITRATE & dwMuxFlags )
    {
        dwFlags |= LEAKYBUCKETFLAGS_AUTOADJUST_BITRATE;
    }

    pAsfMuxStreamContext->m_pLeakyBucket->m_dwFlags |= dwFlags;

 ErrorExit:

    if ( DRM_FAILED( dr ) )
    {
        SAFE_OEM_FREE( pAsfMuxStreamContext->m_pLeakyBucket );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
 * This does the common processing of a new object: assigns a sendtime,
 * objectID, and updates stats
 ******************************************************************************/
static DRM_RESULT ProcessNewWriterSample(
    __inout    ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __in const DRM_UINT64                     hnsTimestampAdjust,
    __out      DRM_UINT64             * const phnsSendTime,
    __out      DRM_DWORD              * const pdwObjectID,
    __inout    DRM_DWORD                     *pdwFlags
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbSample = 0;
    DRM_UINT64 hnsPresTime = DRM_UI64( 0 );


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ProcessNewWriterSample );

    ChkArg(pISample);
    ChkArg(phnsSendTime);
    ChkArg(pdwObjectID);
    ChkArg(pdwFlags);

    ChkDR( GetAdjustedSampleTime( pISample, hnsTimestampAdjust, &hnsPresTime ) );

    hnsPresTime = DRM_UI64Add( hnsPresTime,
                               DRM_UI64Mul( pAsfMuxStreamContext->m_msPreroll,
                                            DRM_UI64( 10000 ) ) );

    cbSample = pISample->cbTotalLength;

    /*
     * Step 1: Get a sendtime
     */
    ChkDR( SendTimeFromPresTime( pAsfMuxStreamContext, hnsPresTime, cbSample, phnsSendTime ) );

    /*
     * m_msLatestPresTimeSeen was updated in SendTimeFromPresTime
     * We can't rely on pSample->m_msDuration, it may be invalid
     */

    ChkDR( ASFMuxStream_ApproxSampleDuration( pAsfMuxStreamContext,
                                              pISample,
                                              &pAsfMuxStreamContext->m_msLatestDurationSeen ) );

    /*
     * Step 2:  Assign an object ID.
     *          This should be done EVEN IF the above failed,
     *          because in that case we should be skipping an
     *          object number.  This here consumes an object number
     */
    *pdwObjectID = AssignObjectID( pAsfMuxStreamContext, *pdwFlags );

    /* We could fail here if we are overrunning the bandwidth */
    ChkDR( dr );

    /*
     * Step 2.5: If all samples are clean points, make sure this one is too
     */
    if ( pAsfMuxStreamContext->m_fAllObjectsAreCleanPoints )
    {
        *pdwFlags |= MF_SAMPLE_FLAG_CLEANPOINT;
    }

#ifdef SIMULATE_MUX_SAMPLE_DROPPING
    {
        double randScale;
        DRM_DWORD  dwMagic;

        randScale = ((double)rand()/(double)RAND_MAX);
        randScale *= 100;
        dwMagic = (DRM_DWORD) randScale;

        /*
         * Simulate a three percent drop.
         */

        if ( ( dwMagic == 01 ) ||
             ( dwMagic == 19 ) ||
             ( dwMagic == 72 ) )
        {
            dr = MF_E_BANDWIDTH_OVERRUN;
        }
     }
#endif

 ErrorExit:

    if ( DRM_SUCCEEDED( dr ) )
    {
        /*
         * Update the stats
         */
        UpdateStats( pAsfMuxStreamContext,
                     DRM_UI64Sub( hnsPresTime,
                                  DRM_UI64Mul( pAsfMuxStreamContext->m_msPreroll,
                                               DRM_UI64( 10000 ) ) ),
                     cbSample );
    }
    else
    {
        /*
         * Something went wrong, so we'll be dropping it.
         */
        TRACE(("Dropping sample for stream %d, prestime %lu due to error in mux",
               pAsfMuxStreamContext->m_wStreamNumber,
               DRM_UI64Low32( hnsPresTime ) / 10000 ));
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
 * This assigns a sendtime, performs a bandwidth check, and determines
 * when is the next sendtime this stream can assign.
 * The value hnsPresTime is already shifted by the first presentation time
 * i.e. buffer window.
 ******************************************************************************/
static DRM_RESULT SendTimeFromPresTime(
    __inout    ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const DRM_UINT64                     hnsPresTime,
    __in const DRM_DWORD                      cbObjectSize,
    __out      DRM_UINT64             * const phnsSendTime
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 hnsMuxLastSendTime;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_SendTimeFromPresTime );

    MUX_TRACE(("ASFMuxStream_SendTimeFromPresTime"));

    ChkArg( phnsSendTime );

    /*
     * Keep track of the latest presentation time seen
     */
    /* if ( (hnsPresTime - 10000 * pAsfMuxStreamContext->m_msPreroll) < (pAsfMuxStreamContext->m_msLatestPresTimeSeen * 10000) ) */
    if ( DRM_UI64Les( DRM_UI64Sub( hnsPresTime,
                                   DRM_UI64Mul( DRM_UI64( 10000 ),
                                                pAsfMuxStreamContext->m_msPreroll ) ),
                      DRM_UI64Mul( pAsfMuxStreamContext->m_msLatestPresTimeSeen,
                                   DRM_UI64( 10000 ) ) ) )
    {
        TRACE(("Out-of-order sample for stream %d (prestime = %lu, last prestime = %d)",
               pAsfMuxStreamContext->m_wStreamNumber,
               DRM_UI64Low32( hnsPresTime ) / 10000,
               pAsfMuxStreamContext->m_msLatestPresTimeSeen));
    }
    else
    {
        pAsfMuxStreamContext->m_msLatestPresTimeSeen = DRM_UI64Sub( DRM_UI64Div( hnsPresTime,
                                                                  DRM_UI64( 10000 ) ),
                                                     pAsfMuxStreamContext->m_msPreroll );
    }

    /*
     * Make sure that the send window can still accommodate this object.
     * If it can't, then squash up the send time accordingly.
     * The send time can't be squashed past the pres time
     */
    hnsMuxLastSendTime = pAsfMuxStreamContext->m_pMux->m_hnsLastPacketSendTime;

    /*
     * We're lagging behind if this presentation time has been shut out of
     * the mux's send window.  We can't packetize this sample.
     */
    /* if ( ( hnsPresTime < hnsMuxLastSendTime) && ( pAsfMuxStreamContext->m_msPreroll > 0 ) ) */
    if ( DRM_UI64Les( hnsPresTime, hnsMuxLastSendTime ) &&
         DRM_UI64Les( DRM_UI64( 0 ), pAsfMuxStreamContext->m_msPreroll ) )
    {
        TRACE(("Late sample for stream %d (prestime = %lu, expecting at least %lu)",
               pAsfMuxStreamContext->m_wStreamNumber,
               DRM_UI64Low32( hnsPresTime ) / 10000,
               DRM_UI64Low32( hnsMuxLastSendTime ) / 10000));

        ChkDR( DRM_E_ASF_LATE_SAMPLE );
    }

    if ( pAsfMuxStreamContext->m_pLeakyBucket )
    {
        DRM_UINT64 hnsBucketLeakTime = DRM_UI64( 0 );
        DRM_DWORD cbObjectSizeForBandwidthPolicing;

        ChkDR( ASFMuxStream_GetObjectSizeForBandwidthPolicing( pAsfMuxStreamContext,
                                                               cbObjectSize,
                                                               &cbObjectSizeForBandwidthPolicing ) );
        ChkDR( ASFLeakyBucket_AddToBucket( pAsfMuxStreamContext->m_pLeakyBucket,
                                           cbObjectSizeForBandwidthPolicing,
                                           hnsPresTime,
                                           hnsMuxLastSendTime,
                                           &hnsBucketLeakTime ) );

        *phnsSendTime = hnsBucketLeakTime;
    }
    else
    {
        DRM_UINT64 hnsLameSendTime;

        /*
         * Don't know what to check against, so don't bother checking
         */
        hnsLameSendTime = DRM_UI64Sub( hnsPresTime,
                                       DRM_UI64Mul( DRM_UI64( pAsfMuxStreamContext->m_msMaxBufferWindow ),
                                                    DRM_UI64( 10000 ) ) );

        /*
         * The floor for the send time is the presentation time minus the buffer window,
         * or the mux's last send time, whichever is later.
         */
        /* if ( hnsLameSendTime >= hnsMuxLastSendTime ) */
        if ( DRM_UI64Les( hnsMuxLastSendTime, hnsLameSendTime ) ||
             DRM_UI64Eql( hnsMuxLastSendTime, hnsLameSendTime ) )
        {
            *phnsSendTime = hnsLameSendTime;
        }
        else
        {
            *phnsSendTime = hnsMuxLastSendTime;
        }
    }

    MUX_TRACE(("Sample for stream %d, prestime(ms) = %lu, sendtime(ms) = %lu, cbObjectSize = %d",
               pAsfMuxStreamContext->m_wStreamNumber,
               DRM_UI64Low32( hnsPresTime ) / 10000,
               DRM_UI64Low32( *phnsSendTime ) / 10000,
               cbObjectSize));

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/******************************************************************************/
static DRM_DWORD AssignObjectID(
    __out      ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const DRM_DWORD                      dwSampleFlags
)
{
    DRM_DWORD dwObjectID;

    /*
     * The general behavior here is to use the m_dwNextObjectID
     * for this assignment and then push m_dwNextObjectID mod 256.
     * Discontinuities will cause something to get skipped, though.
     */
    if ( dwSampleFlags & MF_SAMPLE_FLAG_DISCONTINUITY )
    {
        pAsfMuxStreamContext->m_dwNextObjectID = (pAsfMuxStreamContext->m_dwNextObjectID + 1) % 256;
    }

    dwObjectID = pAsfMuxStreamContext->m_dwNextObjectID;

    pAsfMuxStreamContext->m_dwNextObjectID = (pAsfMuxStreamContext->m_dwNextObjectID + 1) % 256;

    return dwObjectID;
}



/******************************************************************************/
static DRM_RESULT UpdateStats(
    __out      ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const DRM_UINT64                     hnsPresTime,
    __in const DRM_DWORD                      cbObjectSize
)
{
    /* if ( 0 == pAsfMuxStreamContext->m_cFrames ) */
    if ( DRM_UI64Eql( DRM_UI64( 0 ), pAsfMuxStreamContext->m_cFrames ) )
    {
        pAsfMuxStreamContext->m_msFirstPresTimeSeen = DRM_UI64Div( hnsPresTime, DRM_UI64( 10000 ) );
    }

    pAsfMuxStreamContext->m_cFrames = DRM_UI64Add( pAsfMuxStreamContext->m_cFrames, DRM_UI64( 1 ) );
    pAsfMuxStreamContext->m_cbFrameData = DRM_UI64Add( pAsfMuxStreamContext->m_cbFrameData, DRM_UI64( cbObjectSize ) );

    if ( cbObjectSize > pAsfMuxStreamContext->m_cbLargestObjectSeen )
    {
        pAsfMuxStreamContext->m_cbLargestObjectSeen = cbObjectSize;
    }

    return DRM_SUCCESS;
}



/******************************************************************************/
static DRM_RESULT GetAdjustedSampleTime(
    __in const ASF_SAMPLE * const pISample,
    __in const DRM_UINT64         hnsTimestampAdjust,
    __out      DRM_UINT64 * const phnsAdjustedTime
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 hnsOriginalTime = DRM_UI64LITERAL( 0, 0 );


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_GetAdjustedSampleTime );

    DRMASSERT( NULL != pISample );
    DRMASSERT( NULL != phnsAdjustedTime );

    ChkArg( pISample );
    ChkArg( phnsAdjustedTime );

    *phnsAdjustedTime = DRM_UI64( 0 );

    hnsOriginalTime = pISample->hnsSampleTime;

    /* ASSERT (hnsOriginalTime + hnsTimestampAdjust) >= 0 */
    DRMASSERT( DRM_I64Les(DRM_I64(0),
                          DRM_I64Add(DRM_UI2I64(hnsOriginalTime),
                                     DRM_UI2I64(hnsTimestampAdjust)))
            || DRM_UI64Eql(DRM_UI64(0),
                          DRM_UI64Add(hnsOriginalTime,
                                      hnsTimestampAdjust)) );

    if ( !( DRM_I64Les(DRM_I64(0),
                    DRM_I64Add(DRM_UI2I64(hnsOriginalTime),
                               DRM_UI2I64(hnsTimestampAdjust))) ||
            DRM_UI64Eql(DRM_UI64(0),
                    DRM_UI64Add(hnsOriginalTime,
                                hnsTimestampAdjust)) ) )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    *phnsAdjustedTime = DRM_UI64Add( hnsOriginalTime, hnsTimestampAdjust );

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static DRM_RESULT ASFAudioMuxStream_Initialize(
    __out ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in  ASF_STREAM_INFO        * const pIStreamConfig
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    pAsfMuxStreamContext->m_fIsPayloadSizeFixed = FALSE;
    pAsfMuxStreamContext->m_cbFixedPayloadSize =  0;
    pAsfMuxStreamContext->m_pWFX = NULL;
    pAsfMuxStreamContext->m_cbWFX = 0;
    pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress = NULL;

    ChkDR( ASFMuxStream_ReadMediaType( pAsfMuxStreamContext, pIStreamConfig ) );

 ErrorExit:
    return dr;
}



/**********************************************************************************
 * Returns how many objects will be output from an OnSample() call if pSample
 * is passed in
 **********************************************************************************/
static DRM_RESULT ASFAudioMuxStream_HowManyObjectsWillBeReturned(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __out      DRM_DWORD              * const pcObjects
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFAudioMuxStream_HowManyObjectsWillBeReturned );

    ChkArg( NULL != pAsfMuxStreamContext );
    ChkArg( NULL != pISample );
    ChkArg( NULL != pcObjects );

    if ( !pAsfMuxStreamContext->m_fIsPayloadSizeFixed )
    {
        /*
         * For fragmentable streams, sample processing is 1-in-1-out
         */
        *pcObjects = 1;
    }
    else
    {
        DRM_DWORD cbSampleLength = 0;
        DRM_DWORD cbMaxOutputBytes;

        cbSampleLength = pISample->cbTotalLength;
        cbMaxOutputBytes = cbSampleLength;

        /*
         * At most, we'll output all the bytes we have, which is this sample
         * plus the bytes we're currently hanging onto
         */
        if ( pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress )
        {
            cbMaxOutputBytes += pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress->m_cbSize;
        }

        /*
         * Chopped into objects of size m_cbFixedPayloadSize
         */
        DRMASSERT( 0 != pAsfMuxStreamContext->m_cbFixedPayloadSize );
        *pcObjects = (cbMaxOutputBytes / pAsfMuxStreamContext->m_cbFixedPayloadSize) + 1;
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*********************************************************************************
 * For audio streams that can't be fragmented, we add buffers to the
 * CWMUnfragmentableObject that is in progress.  Only if it becomes
 * a full payload do we packetize and send it
 **********************************************************************************/
static DRM_RESULT ASFAudioMuxStream_OnSample(
    __inout    ASF_MUX_STREAM_CONTEXT * const f_pAsfMuxStreamContext,
    __in       ASF_SAMPLE             * const f_pISample,
    __in const DRM_UINT64                     f_hnsTimestampAdjust,
    __in       DRM_DWORD                      f_dwFlags,
    __inout_ecount( *f_pcQueueObjects )
               ASF_MUX_QUEUE_OBJECT_CONTEXT **f_ppMuxQueueObjects,
    __inout    DRM_DWORD              * const f_pcQueueObjects
)
{
    DRM_RESULT dr                 = DRM_SUCCESS;
    DRM_DWORD  cbSample           = 0;
    DRM_UINT64 hnsPresTime        = DRM_UI64( 0 );
    DRM_DWORD  cbBytesPayloadized = 0;
    DRM_DWORD  cQueueIndex        = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFAudioMuxStream_OnSample );

    DRMASSERT( NULL != f_pAsfMuxStreamContext );
    DRMASSERT( NULL != f_pISample );
    DRMASSERT( NULL != f_ppMuxQueueObjects );
    DRMASSERT( NULL != f_pcQueueObjects );

    /*
     * If the audio can be fragmented, then just use the base class's implementation
     */
    if ( !f_pAsfMuxStreamContext->m_fIsPayloadSizeFixed )
    {
        dr = OnSample( f_pAsfMuxStreamContext,
                       f_pISample,
                       f_hnsTimestampAdjust,
                       f_dwFlags,
                       f_ppMuxQueueObjects,
                       &cQueueIndex );

        goto ErrorExit;
    }

    ChkBOOL( *f_pcQueueObjects != 0, DRM_E_BUFFERTOOSMALL );

#ifdef SIMULATE_MUX_SAMPLE_DROPPING
    {
        double randScale;
        DRM_DWORD  dwMagic;

        randScale = ((double)rand()/(double)RAND_MAX);
        randScale *= 100;
        dwMagic = (DRM_DWORD) randScale;

        /*
         * Simulate a three percent drop.
         */

        if ( ( dwMagic == 01 ) ||
             ( dwMagic == 19 ) ||
             ( dwMagic == 72 ) )
        {
            dr = DRM_E_ASF_BANDWIDTH_OVERRUN;
            break;
        }
    }
#endif

    cbSample = f_pISample->cbTotalLength;

    ChkDR( GetAdjustedSampleTime( f_pISample, f_hnsTimestampAdjust, &hnsPresTime ) );

    hnsPresTime = DRM_UI64Add( hnsPresTime, DRM_UI64Mul( f_pAsfMuxStreamContext->m_msPreroll, DRM_UI64( 10000 ) ) );

    /* MUX_TRACE(("ASFAudioMuxStream_OnSample(): %d bytes for stream %d, prestime = %d",
     *       cbSample,
     *       pAsfMuxStreamContext->m_wStreamNumber,
     *       pSample->m_msPresTime );
     */

    /*
     * Unless this is PCM data, we better have this object be a multiple of
     * the fixed payload size
     */
    if ( WAVE_FORMAT_PCM != f_pAsfMuxStreamContext->m_pWFX->wFormatTag && f_pAsfMuxStreamContext->m_pWFX->nBlockAlign )
    {
        DRMASSERT( (cbSample % f_pAsfMuxStreamContext->m_pWFX->nBlockAlign) == 0 );
        if ( 0 != (cbSample % f_pAsfMuxStreamContext->m_pWFX->nBlockAlign) )
        {
            ChkDR( DRM_E_INVALIDARG );
        }
    }

    /*
     * In the following code we're going to "payloadize" these bytes into
     * objects of the appropriate fixed payload size.
     *
     * If this data is compressed, then presumably the size of pSample
     * is an integral multiple of the fixed payload size, so this
     * one object will divide cleanly into N objects.
     *
     * If this data is PCM, then it is likely it won't divide evenly
     * by m_cbFixedPayloadSize.  The remainder is held in
     * m_pUnfragmentableObjectInProgress and will be returned once
     * we have enough additional data to do so.
     */

    while( cbBytesPayloadized < cbSample )
    {
        /*
         * If the thing was full, we should have sent it!
         */
        ChkBOOL( !f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress
                 || f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress->m_cbSize < f_pAsfMuxStreamContext->m_cbFixedPayloadSize, DRM_E_ASF_UNEXPECTED );

        if ( !f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress )
        {
            DRM_DWORD dwObjectID;
            DRM_UINT64 msAdjustPresTime = DRM_UI64LITERAL( 0, 0 );
            DRM_UINT64 hnsSendTime = DRM_UI64LITERAL( 0, 0 );

            /*
             * Time to start a new one:
             * Let's get a prestime, sendtime, and object ID
             */

            /*
             * Get an object ID.
             * If there is a bandwidth overrun below (from the call to
             * SendTimeFromPresTime), we still do the right thing by
             * consuming an objectID
             */
            dwObjectID = AssignObjectID( f_pAsfMuxStreamContext, f_dwFlags );
            DRMASSERT( dwObjectID <= 255 );

            /*
             * If we're hacking up this sample, then we might need to push the
             * sendtime forward
             */

            /*
             * NOTE: VBR
             *
             *
             *                if ( m_fVBREnabled )
             *                {
             *                     *
             *                     * This works properly only if all payloads have the same bitrate,
             *                     * otherwise this estimation is not accurate
             *                     *
             *                    msAdjustPresTime = ( msPresTime + ( pSample->m_msDuration * cbBytesPayloadized / cbSample ) );
             *                }
             *                else
             */
            {
                /*
                 * In non-VBR case we can rely on pWFX->nAvgBytesPerSec
                 */
                DRM_UINT64 msPresTime = DRM_UI64Div( hnsPresTime, DRM_UI64( 10000 ) );
                /* msAdjustPresTime = msPresTime + ((1000 * cbBytesPayloadized) / pAsfMuxStreamContext->m_pWFX->nAvgBytesPerSec); */
                msAdjustPresTime = DRM_UI64Add( msPresTime,
                                                DRM_UI64Div( DRM_UI64Mul( DRM_UI64( 1000 ),
                                                                          DRM_UI64( cbBytesPayloadized ) ),
                                                             DRM_UI64( f_pAsfMuxStreamContext->m_pWFX->nAvgBytesPerSec ) ) );
            }

            /*
             * Get a sendtime, keeping in mind that we send m_cbFixedPayloadSize
             * at a time
             * We could fail here if we are overrunning the bandwidth
             */
            ChkDR( SendTimeFromPresTime( f_pAsfMuxStreamContext,
                                         DRM_UI64Mul( DRM_UI64( 10000 ), msAdjustPresTime ),
                                         f_pAsfMuxStreamContext->m_cbFixedPayloadSize,
                                         &hnsSendTime ) );

            /*
             * m_msLatestPresTimeSeen was updated in SendTimeFromPresTime,
             * let's update m_msLatestDurationSeen
             */
            ChkDR( ASFMuxStream_ApproxSampleDuration( f_pAsfMuxStreamContext,
                                                      f_pISample,
                                                      &f_pAsfMuxStreamContext->m_msLatestDurationSeen ) );

            /*
             * If all samples are clean points, make sure this one is too
             */
            if ( f_pAsfMuxStreamContext->m_fAllObjectsAreCleanPoints )
            {
                f_dwFlags |= MF_SAMPLE_FLAG_CLEANPOINT;
            }

            /*
             * Create the queue object
             */
            if ( DRM_SUCCEEDED( dr ) )
            {
                DRMASSERT( NULL == f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress );
                f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress =
                    (ASF_MUX_QUEUE_OBJECT_CONTEXT *) Oem_MemAlloc( SIZEOF(ASF_MUX_QUEUE_OBJECT_CONTEXT) );
                ChkMem( f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress );
                ChkDR( ASFMuxQueueObject_Initialize( f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress,
                                                     ASF_MUX_QUEUE_TYPE_UNFRAGMENTABLE,
                                                     f_pISample,
                                                     cbBytesPayloadized,
                                                     f_pAsfMuxStreamContext->m_cbFixedPayloadSize,
                                                     msAdjustPresTime,
                                                     hnsSendTime,
                                                     f_pAsfMuxStreamContext->m_wStreamNumber,
                                                     (DRM_BYTE) dwObjectID,
                                                     f_dwFlags ) );

                /*
                 * This new object will use as many bytes as it can,
                 * but no more than a fixed-size payload worth
                 */
                if ( (cbSample - cbBytesPayloadized) > f_pAsfMuxStreamContext->m_cbFixedPayloadSize )
                {
                    cbBytesPayloadized += f_pAsfMuxStreamContext->m_cbFixedPayloadSize;
                }
                else
                {
                    cbBytesPayloadized += cbSample - cbBytesPayloadized;
                }
            }
            ChkDR( dr );
        }
        else
        {
            DRM_DWORD cbBytesUsed = 0;

            /*
             * There is already an unfragmentable object in progress.
             * Just add this buffer at the appropriate offset
             */
            ChkDR( ASFMuxQueueObject_AddSegment( f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress,
                                                 f_pISample,
                                                 0,
                                                 &cbBytesUsed ) );
            cbBytesPayloadized += cbBytesUsed;
        }

        /*
         * Next step: Put the object on our list of objects to return,
         * if it is full
         */
        if ( f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress->m_cbSize >= f_pAsfMuxStreamContext->m_cbFixedPayloadSize )
        {
            /*
             * Put it on the list if there is still room
             */
            ChkBOOL ( cQueueIndex < (*f_pcQueueObjects), DRM_E_BUFFERTOOSMALL );

            f_ppMuxQueueObjects[cQueueIndex] = f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress;
            cQueueIndex++;

            /*
             * We created an object, so update the stats
             */
            UpdateStats( f_pAsfMuxStreamContext,
                         DRM_UI64Div( DRM_UI64Sub( f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress->m_msPresTime,
                                                   f_pAsfMuxStreamContext->m_msPreroll ),
                                      DRM_UI64( 10000 ) ),
                         cbSample );

            /*
             * Setting this to NULL indicates that next time we need to
             * create a new one.
             * They will be released in CWMMuxSendWindow
             */
            f_pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress = NULL;
        }

    }
    ChkDR(dr);

 ErrorExit:

    if ( DRM_FAILED( dr ) )
    {
        DRM_DWORD i;

        /*
         * Something went wrong, so we'll be dropping it
         */
        TRACE( ("Dropping sample for stream %d, prestime %lu due to error in mux",
               f_pAsfMuxStreamContext->m_wStreamNumber,
               DRM_UI64Low32( DRM_UI64Div( hnsPresTime, DRM_UI64( 10000 ) ) ) ));

        if ( NULL != f_pcQueueObjects )
        {
            for ( i = 0; (i < *f_pcQueueObjects) && (i < cQueueIndex); i++ )
            {
                ASFMuxQueueObject_Uninitialize(f_ppMuxQueueObjects[i]);
                SAFE_OEM_FREE(f_ppMuxQueueObjects[i]);
            }
            *f_pcQueueObjects = 0;
        }
    }
    else {
        *f_pcQueueObjects = cQueueIndex;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT ASFAudioMuxStream_ApproxSampleDuration(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __out      DRM_UINT64             * const pmsDuration
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFAudioMuxStream_ApproxSampleDuration );

    ChkArg( NULL != pAsfMuxStreamContext );
    ChkArg( NULL != pmsDuration );

    *pmsDuration = DRM_UI64( 0 );
    if ( !pISample )
    {
        goto ErrorExit;
    }

    /*
     * For compressed audio we use pSample->m_msDuration,
     * since that is set correctly by the codec;
     * for uncompressed, use the default duration calculation
     */
    if ( WAVE_FORMAT_PCM != pAsfMuxStreamContext->m_pWFX->wFormatTag )
    {
        DRM_UINT64 hnsDuration = DRM_UI64LITERAL( 0, 0 );
        hnsDuration = pISample->hnsSampleTime;

        *pmsDuration = DRM_UI64Div( hnsDuration, DRM_UI64( 10000 ) );
    }
    else
    {
        ChkDR( ApproxSampleDuration( pAsfMuxStreamContext,
                                     pISample,
                                     pmsDuration ) );
    }

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************/
static DRM_RESULT ASFAudioMuxStream_GetHappyCutoffSendtime(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __out      DRM_UINT64             * const phnsNextSendtime
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFAudioMuxStream_GetHappyCutoffSendtime );

    MUX_TRACE(("ASFAudioMuxStream_GetHappyCutoffSendtime"));

    ChkArg( NULL != pAsfMuxStreamContext );
    ChkArg( NULL != phnsNextSendtime );

    if ( NULL != pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress )
    {
        /*
         * We have an object we're working on that we haven't yet put
         * in the SendWindow's queue.  Make sure no one packetizes
         * anything later
         */
        MUX_TRACE(("Currently working on unfragmentable object 0x%p with sendtime %lu",
                   pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress,
                   DRM_UI64Low32( pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress->m_hnsSendTime ) ));

        *phnsNextSendtime = pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress->m_hnsSendTime;
    }
    else
    {
        ChkDR( GetHappyCutoffSendtime( pAsfMuxStreamContext, phnsNextSendtime ) );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_VOID ASFAudioMuxStream_Cleanup(
    __inout ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext
)
{
    if ( NULL != pAsfMuxStreamContext )
    {
        /*
         * This function should return m_pUnfragmentableObjectInProgress
         * if it still contains any bytes of data in the case of uncompressed PCM data.
         * That is because there was no guarantee that the number of PCM audio bytes we
         * receive divides evenly into the fixed payload size.  Right now we're just
         * dropping these bytes instead of packetizing them
         */

        if ( NULL != pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress )
        {
            ASFMuxQueueObject_Release( pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress );
            pAsfMuxStreamContext->m_pUnfragmentableObjectInProgress = NULL;
        }
    }

    return;
}



/******************************************************************************/
static DRM_RESULT ASFAudioMuxStream_ReadMediaType(
    __out ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in  ASF_STREAM_INFO        * const pIStreamConfig
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg (pIStreamConfig );

    /*
     * Get the WAVEFORMATEX
     */
    pAsfMuxStreamContext->m_pWFX = &pIStreamConfig->WFX;
    pAsfMuxStreamContext->m_cbWFX = SIZEOF( ASF_WAVEFORMATEX ) + pAsfMuxStreamContext->m_pWFX->cbSize;

    pAsfMuxStreamContext->m_fIsPayloadSizeFixed = TRUE;
    pAsfMuxStreamContext->m_cbFixedPayloadSize = pAsfMuxStreamContext->m_pWFX->nBlockAlign;

 ErrorExit:

    return dr;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static DRM_RESULT ASFVideoMuxStream_Initialize(
    __out  ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in   ASF_STREAM_INFO        * const pIStreamConfig
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    pAsfMuxStreamContext->m_fSetSampleDurations = FALSE;
    pAsfMuxStreamContext->m_pLastQueueObjectSeen = NULL;

    pAsfMuxStreamContext->m_fAllObjectsAreCleanPoints = FALSE;

    ChkDR( ASFMuxStream_ReadMediaType( pAsfMuxStreamContext, pIStreamConfig ) );

 ErrorExit:
    return dr;
}



/******************************************************************************/
static DRM_RESULT ASFVideoMuxStream_OnSample(
    __inout       ASF_MUX_STREAM_CONTEXT * const f_pAsfMuxStreamContext,
    __in          ASF_SAMPLE             * const f_pISample,
    __in    const DRM_UINT64                     f_hnsTimestampAdjust,
    __in    const DRM_DWORD                      f_dwFlags,
    __out_ecount( *f_pcQueueObjects )
                  ASF_MUX_QUEUE_OBJECT_CONTEXT **f_ppMuxQueueObjects,
    __out         DRM_DWORD              * const f_pcQueueObjects
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ASF_MUX_QUEUE_OBJECT_CONTEXT *pAsfMuxStreamContextFragmentableObject;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFVideoMuxStream_OnSample );

    /*
     * This uses the base class implementation and then sets durations
     * if it can
     */
    DRMASSERT( NULL != f_pAsfMuxStreamContext );
    DRMASSERT( NULL != f_pISample );
    DRMASSERT( NULL != f_ppMuxQueueObjects );
    DRMASSERT( NULL != f_pcQueueObjects );

    /*
     * First hand it off to the base class
     */
    ChkDR( OnSample( f_pAsfMuxStreamContext,
                     f_pISample,
                     f_hnsTimestampAdjust,
                     f_dwFlags,
                     f_ppMuxQueueObjects,
                     f_pcQueueObjects ) );

    if ( 0 == *f_pcQueueObjects )
    {
        /*
         * Nothing to deal with here
         */
        goto ErrorExit;
    }

    /*
     * Validate the output
     */
    DRMASSERT( 1 == *f_pcQueueObjects );

    DRMASSERT( NULL != f_ppMuxQueueObjects[ 0 ] && TRUE == f_ppMuxQueueObjects[ 0 ]->m_fFragmentable );
    if ( !f_ppMuxQueueObjects[ 0 ] || !f_ppMuxQueueObjects[ 0 ]->m_fFragmentable )
    {
        ChkDR( DRM_E_FAIL );
    }

    pAsfMuxStreamContextFragmentableObject = (ASF_MUX_QUEUE_OBJECT_CONTEXT *) f_ppMuxQueueObjects[ 0 ];

    if ( f_pAsfMuxStreamContext->m_pLastQueueObjectSeen )
    {
        /*
         * OK, we're done with this guy
         */
        if ( NULL != f_pAsfMuxStreamContext->m_pLastQueueObjectSeen )
        {
            ASFMuxQueueObject_Release( f_pAsfMuxStreamContext->m_pLastQueueObjectSeen );
            f_pAsfMuxStreamContext->m_pLastQueueObjectSeen = NULL;
        }
    }


    /*
     * Now the this new object -- whose duration we don't yet know enough
     * to say for sure, should get the "guess" duration of 1msec,
     * and we'll hang onto a ref in m_pLastQueueObjectSeen, so that
     * next time we see something, we'll be able to assign it a duration
     */
    f_pAsfMuxStreamContext->m_pLastQueueObjectSeen = pAsfMuxStreamContextFragmentableObject;
    ChkDR( ASFMuxQueueObject_AddRef( f_pAsfMuxStreamContext->m_pLastQueueObjectSeen ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT ASFVideoMuxStream_ApproxSampleDuration(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_SAMPLE             * const pISample,
    __out      DRM_UINT64                    *pmsDuration
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFVideoMuxStream_ApproxSampleDuration );

    UNREFERENCED_PARAMETER( pISample );

    ChkArg( NULL != pAsfMuxStreamContext );
    ChkArg( NULL != pmsDuration );

    *pmsDuration = DRM_UI64Div( pAsfMuxStreamContext->m_hnsAvgTimePerFrame, DRM_UI64( 10000 ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_RESULT ASFVideoMuxStream_GetHappyCutoffSendtime(
    __in const ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __out      DRM_UINT64             * const phnsNextSendtime
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFVideoMuxStream_GetHappyCutoffSendtime );

    MUX_TRACE(("ASFVideoMuxStream_GetHappyCutoffSendtime"));

    ChkArg( NULL != pAsfMuxStreamContext );
    ChkArg( NULL != phnsNextSendtime );

    if ( pAsfMuxStreamContext->m_pLastQueueObjectSeen )
    {
        /*
         * We'd like to avoid sending off this sample that
         * we're pointing to if possible.
         * This is because we'll be able to assign it
         * a duration once we see another sample come in,
         * but until then we don't know the duration
         */
        MUX_TRACE(("Last queue object 0x%p has send time %lu, keeping it around for duration-setting purposes",
                   pAsfMuxStreamContext->m_pLastQueueObjectSeen,
                   DRM_UI64Low32( pAsfMuxStreamContext->m_pLastQueueObjectSeen->m_hnsSendTime ) ));

        *phnsNextSendtime = pAsfMuxStreamContext->m_pLastQueueObjectSeen->m_hnsSendTime;
    }
    else
    {
        /*
         * We always end up here if we're not doing durations
         */
        ChkDR( GetHappyCutoffSendtime( pAsfMuxStreamContext, phnsNextSendtime ) );
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************/
static DRM_VOID ASFVideoMuxStream_Cleanup(
    __inout_opt ASF_MUX_STREAM_CONTEXT * const f_pAsfMuxStreamContext
)
{
    if ( NULL != f_pAsfMuxStreamContext )
    {
        if ( NULL != f_pAsfMuxStreamContext->m_pLastQueueObjectSeen )
        {
            ASFMuxQueueObject_Release( f_pAsfMuxStreamContext->m_pLastQueueObjectSeen );
            f_pAsfMuxStreamContext->m_pLastQueueObjectSeen = NULL;
        }
    }
}



/******************************************************************************/
static DRM_RESULT ASFVideoMuxStream_ReadMediaType(
    __out      ASF_MUX_STREAM_CONTEXT * const pAsfMuxStreamContext,
    __in const ASF_STREAM_INFO        * const pIStreamConfig
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_MUX, PERF_FUNC_ASFVideoMuxStream_ReadMediaType );

    ChkArg( pIStreamConfig );

    pAsfMuxStreamContext->m_hnsAvgTimePerFrame = pIStreamConfig->hnsAvgTimePerFrame;

    if ( DRM_UI64Eql( DRM_UI64( 0 ), pAsfMuxStreamContext->m_hnsAvgTimePerFrame ) )
    {
        /*
         * Guess either 15 or 30 fps
         */
        if ( pAsfMuxStreamContext->m_dwMaxBitrate > 150000 )
        {
            pAsfMuxStreamContext->m_hnsAvgTimePerFrame = DRM_UI64( 666666 );
        }
        else
        {
            pAsfMuxStreamContext->m_hnsAvgTimePerFrame = DRM_UI64( 333333 );
        }
    }

    /*
     * Now that we know more about the media type, we can come up
     * with a better bitrate for bandwidth policing.
     */

    if ( pAsfMuxStreamContext->m_pLeakyBucket )
    {
        DRM_DWORD dwBitratePlusOverhead;

        dwBitratePlusOverhead = pAsfMuxStreamContext->m_dwMaxBitrate +
            8L * 17L * DRM_UI64Low32( DRM_UI64Div( DRM_UI64( 10000000L ), pAsfMuxStreamContext->m_hnsAvgTimePerFrame ) );

        pAsfMuxStreamContext->m_pLeakyBucket->m_dwBitrate = dwBitratePlusOverhead;
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

#endif /* DRM_SUPPORT_AUDIO_ONLY == 0 */

EXIT_PK_NAMESPACE_CODE;
