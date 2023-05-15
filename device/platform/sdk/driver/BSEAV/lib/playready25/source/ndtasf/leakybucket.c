/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_LEAKYBUCKET_C
#include <ndtcontextsizes.h>
#include <leakybucket.h>
#include <ndtasf_internal.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#if DRM_SUPPORT_AUDIO_ONLY == 0
/*****************************************************************************
 **
 ** Function:    ASFLeakyBucket_Initialize
 **
 ** Synopsis:    Initializes the leaky bucket.  The context initialized
 **              with this function is then used in subsequent
 **              ASFLeakyBucket calls.
 **
 ** Arguments:   [pAsfLeakyBucketContext] -- context
 **              [dwBitrate]              -- bit rate in bits per second
 **              {hnsBucketSize]          -- bucket size in hundreds of
 **                                          nanoseconds.
 ** Returns:     DRM_SUCCESS
 **
 *****************************************************************************/
DRM_RESULT ASFLeakyBucket_Initialize(
    __out      ASF_LEAKY_BUCKET_CONTEXT * const pAsfLeakyBucketContext,
    __in const DRM_DWORD                        dwBitrate,
    __in const DRM_UINT64                       hnsBucketSize
)
{
    pAsfLeakyBucketContext->m_hnsBucketSize = hnsBucketSize;
    pAsfLeakyBucketContext->m_dwBitrate = dwBitrate;
    pAsfLeakyBucketContext->m_dwFlags = 0;
    pAsfLeakyBucketContext->m_hnsNextLeakTime = DRM_UI64( 0 );
    pAsfLeakyBucketContext->m_hnsLastLeakTime = DRM_UI64( 0 );
    pAsfLeakyBucketContext->m_cbLastLeakSize = 0;
    pAsfLeakyBucketContext->m_dwTotalBitrateAdjust = 0;
    pAsfLeakyBucketContext->m_hnsLastAutoAdjustInterval = DRM_UI64( 0 );
    pAsfLeakyBucketContext->m_cbLastAutoAdjustSize = 0;

    MUX_TRACE(("Leaky bucket created for bitrate %d, bucket size %lu hns",
               pAsfLeakyBucketContext->m_dwBitrate,
               DRM_UI64Low32( pAsfLeakyBucketContext->m_hnsBucketSize ) ));

    return DRM_SUCCESS;
}



/*****************************************************************************
 **
 ** Function:    ASFLeakyBucket_AddToBucket
 **
 ** Synopsis:    Generates a leak time for a sample.  Normally the leak time
 **              generated is the current next leak time of the bucket plus
 **              the size of the sample divided by the bitrate.
 **
 ** Arguments:   [pAsfLeakyBucketContext] -- context
 **              [cbSize]                 -- size of sample
 **              [hnsTimestamp]           -- timestamp of sample
 **              [hnsLeakTimeFloor]       -- leak floor of sample
 **              [phnsLeakTime]           -- leak time
 **
 ** Returns:     DRM_SUCCESS
 **              MF_E_UNEXPECTED if input parameters are unexpected
 **              MF_E_BANDWITH_OVERRUN if bucket is overflowing
 **
 *****************************************************************************/
DRM_RESULT ASFLeakyBucket_AddToBucket(
    __inout    ASF_LEAKY_BUCKET_CONTEXT * const pAsfLeakyBucketContext,
    __in const DRM_DWORD                        cbSize,
    __in const DRM_UINT64                       hnsTimestamp,
    __in const DRM_UINT64                       hnsLeakTimeFloor,
    __out      DRM_UINT64               * const phnsLeakTime
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 hnsLeakTime;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF_STREAM, PERF_FUNC_ASFLeakyBucket_AddToBucket );

    DRMASSERT( cbSize > 0 );

    if ( 0 == cbSize )
    {
        return( DRM_E_ASF_UNEXPECTED );
    }

    /* DRMASSERT( hnsTimestamp >= hnsLeakTimeFloor ) */
    DRMASSERT( ( DRM_UI64Les( hnsLeakTimeFloor, hnsTimestamp ) || DRM_UI64Eql( hnsTimestamp, hnsLeakTimeFloor ) )
               && "There is no way we can leak this object before the floor given" );
    /* if ( hnsTimestamp < hnsLeakTimeFloor ) */
    if ( DRM_UI64Les( hnsTimestamp, hnsLeakTimeFloor ) )
    {
        return( DRM_E_ASF_UNEXPECTED );
    }

    /* This won't actually catch all cases of timestamps going
     * backwards.  We can add that validation if it's deemed
     * necessary.
     */
    /* DRMASSERT( hnsTimestamp >= pAsfLeakyBucketContext->m_hnsLastLeakTime ) */
    DRMASSERT( ( DRM_UI64Les( pAsfLeakyBucketContext->m_hnsLastLeakTime, hnsTimestamp ) || DRM_UI64Eql( hnsTimestamp, pAsfLeakyBucketContext->m_hnsLastLeakTime ) )
               && "Timestamps shouldn't go backwards" );
    /* if ( hnsTimestamp < pAsfLeakyBucketContext->m_hnsLastLeakTime ) */
    if ( DRM_UI64Les( hnsTimestamp, pAsfLeakyBucketContext->m_hnsLastLeakTime ) )
    {
        return( DRM_E_ASF_UNEXPECTED );
    }

    /* DRMASSERT( ( hnsTimestamp >= pAsfLeakyBucketContext->m_hnsBucketSize ) ); */
    DRMASSERT( ( DRM_UI64Les( pAsfLeakyBucketContext->m_hnsBucketSize, hnsTimestamp ) || DRM_UI64Eql( hnsTimestamp, pAsfLeakyBucketContext->m_hnsBucketSize ) )
               && "Timestamps need to be greater than the bucket size" );

    MUX_TRACE(("Object received at leaky bucket: Size = %d, Timestamp = %lu.  Next leak time = %lu, requested leak time floor is %lu",
               cbSize,
               DRM_UI64Low32( hnsTimestamp ),
               DRM_UI64Low32( pAsfLeakyBucketContext->m_hnsNextLeakTime ),
               DRM_UI64Low32( hnsLeakTimeFloor) ));

    /* if ( hnsTimestamp < pAsfLeakyBucketContext->m_hnsNextLeakTime ) */
    if ( DRM_UI64Les( hnsTimestamp, pAsfLeakyBucketContext->m_hnsNextLeakTime ) )
    {
        /*
         * This is an overflow.
         */
        TRACE(("Size %d timestamp %lu is overflowing the leaky bucket (next leak time is %lu)",
               cbSize,
               DRM_UI64Low32( hnsTimestamp ),
               DRM_UI64Low32( pAsfLeakyBucketContext->m_hnsNextLeakTime ) ));

        if ( LEAKYBUCKETFLAGS_AUTOADJUST_BITRATE & pAsfLeakyBucketContext->m_dwFlags )
        {
            DRM_UINT64 hnsLastLeakDurationLimit;
            DRM_DWORD dwBitrateNeeded;

            /* hnsLastLeakDurationLimit = hnsTimestamp - pAsfLeakyBucketContext->m_hnsLastLeakTime; */
            hnsLastLeakDurationLimit = DRM_UI64Sub( hnsTimestamp, pAsfLeakyBucketContext->m_hnsLastLeakTime );

            /*
             * We'll increase the bitrate enough so that the last
             * chunk that we leaked would have just finished
             * leaking in time for this object.
             */

            /* if ( 0 == hnsLastLeakDurationLimit ) */
            if ( DRM_UI64Eql( DRM_UI64( 0 ), hnsLastLeakDurationLimit ) )
            {
                /*
                 * We'd get here if we had a bunch of objects, all with the same
                 * timestamp, that all overflowed the bucket and necessitated bitrate
                 * adjustments.  In this case, we want to treat all frames with
                 * this timestamp as one big object over the specified time interval
                 */
                hnsLastLeakDurationLimit = pAsfLeakyBucketContext->m_hnsLastAutoAdjustInterval;

                /* DRMASSERT( hnsLastLeakDurationLimit > 0 ); */
                DRMASSERT( DRM_UI64Les( DRM_UI64( 0 ), hnsLastLeakDurationLimit ) );
                /* if ( hnsLastLeakDurationLimit <= 0 ) note: this is unsigned, so just have to check == 0 */
                if ( DRM_UI64Eql( hnsLastLeakDurationLimit, DRM_UI64( 0 ) ) )
                {
                    ChkDR( DRM_E_ASF_UNEXPECTED );
                }

                pAsfLeakyBucketContext->m_cbLastLeakSize += pAsfLeakyBucketContext->m_cbLastAutoAdjustSize;

                MUX_TRACE(("This timestamp and last leak time are equal (%lu): Using %lu as the adjustment interval, size is %d",
                           DRM_UI64Low32( hnsTimestamp ),
                           DRM_UI64Low32( hnsLastLeakDurationLimit ),
                           pAsfLeakyBucketContext->m_cbLastLeakSize));
            }

            /* dwBitrateNeeded = (DRM_DWORD)( ( 8 * 1000 * 10000 * (DRM_UINT64) pAsfLeakyBucketContext->m_cbLastLeakSize ) / hnsLastLeakDurationLimit ); */
            dwBitrateNeeded = DRM_UI64Low32( DRM_UI64Div( DRM_UI64Mul( DRM_UI64( 8L * 1000L * 10000L ),
                                                                       DRM_UI64( pAsfLeakyBucketContext->m_cbLastLeakSize ) ),
                                                          hnsLastLeakDurationLimit ) );

            /*
             * We can change the parameters like this and keep going
             */
            DRMASSERT( dwBitrateNeeded >= pAsfLeakyBucketContext->m_dwBitrate );
            pAsfLeakyBucketContext->m_dwTotalBitrateAdjust += (dwBitrateNeeded - pAsfLeakyBucketContext->m_dwBitrate);
            pAsfLeakyBucketContext->m_dwBitrate = dwBitrateNeeded;
            pAsfLeakyBucketContext->m_hnsNextLeakTime = hnsTimestamp;
            pAsfLeakyBucketContext->m_hnsLastAutoAdjustInterval = hnsLastLeakDurationLimit;
            pAsfLeakyBucketContext->m_cbLastAutoAdjustSize = pAsfLeakyBucketContext->m_cbLastLeakSize;

            MUX_TRACE(("Had to adjust the bitrate to %d",
                       pAsfLeakyBucketContext->m_dwBitrate));
        }
        else
        {
            /*
             * Since we can't internally adjust anything, we need
             * to report this as an overflow
             */
            DRMASSERT( !"An object is overflowing the leaky bucket" );
            TRACE(("Bucket is overflowing: rejecting object at time %lu",
                   DRM_UI64Low32( hnsTimestamp ) ));

            ChkDR( DRM_E_ASF_BANDWIDTH_OVERRUN );
        }
    }
    /* else if( hnsTimestamp > pAsfLeakyBucketContext->m_hnsNextLeakTime + pAsfLeakyBucketContext->m_hnsBucketSize ) */
    else if( DRM_UI64Les( DRM_UI64Add( pAsfLeakyBucketContext->m_hnsNextLeakTime, pAsfLeakyBucketContext->m_hnsBucketSize),
                          hnsTimestamp ) )
    {
        /*
         * This is underflow. Adjust m_hnsNextLeakTime(send time) to make sure that "(P - B) <= S <= P".
         */
        MUX_TRACE(("Size %d timestamp %lu is underflowing the leaky bucket. Current next leak time = %lu.",
                   cbSize,
                   DRM_UI64Low32( hnsTimestamp ),
                   DRM_UI64Low32( pAsfLeakyBucketContext->m_hnsNextLeakTime ) ));

        /* pAsfLeakyBucketContext->m_hnsNextLeakTime = hnsTimestamp - pAsfLeakyBucketContext->m_hnsBucketSize; */
        pAsfLeakyBucketContext->m_hnsNextLeakTime = DRM_UI64Sub( hnsTimestamp, pAsfLeakyBucketContext->m_hnsBucketSize );

        MUX_TRACE(("Size %d timestamp %lu is underflowing the leaky bucket. New next leak time = %lu.",
                   cbSize,
                   DRM_UI64Low32( hnsTimestamp ),
                   DRM_UI64Low32( pAsfLeakyBucketContext->m_hnsNextLeakTime ) ));
    }

    /*
     * If we're here, then the object can be leaked.
     */
    hnsLeakTime = pAsfLeakyBucketContext->m_hnsNextLeakTime;
    /* if ( hnsLeakTime < hnsLeakTimeFloor ) */
    if ( DRM_UI64Les( hnsLeakTime, hnsLeakTimeFloor ) )
    {
        hnsLeakTime = hnsLeakTimeFloor;

        TRACE(("Warning: Leak times are being artifically pushed ahead.  This may cause overflows later."));
    }

    /*
     * Now figure out when we're going to be able to leak the next object
     */
    /* pAsfLeakyBucketContext->m_hnsNextLeakTime = hnsLeakTime +
     *   ( (DRM_UINT64) cbSize * 8 * 1000 * 10000 ) / pAsfLeakyBucketContext->m_dwBitrate;
     */
    pAsfLeakyBucketContext->m_hnsNextLeakTime =
        DRM_UI64Add( hnsLeakTime,
                     DRM_UI64Div( DRM_UI64Mul( DRM_UI64( cbSize ),
                                               DRM_UI64( 8L * 1000L * 10000L ) ),
                                  DRM_UI64( pAsfLeakyBucketContext->m_dwBitrate ) ) );


    MUX_TRACE(("Assign object leak time %lu.  Next leak time is %lu",
               DRM_UI64Low32( hnsLeakTime ),
               DRM_UI64Low32( pAsfLeakyBucketContext->m_hnsNextLeakTime ) ));

    /*
     * Update the state and return the leak time
     */
    pAsfLeakyBucketContext->m_hnsLastLeakTime = hnsLeakTime;
    pAsfLeakyBucketContext->m_cbLastLeakSize = cbSize;

    if ( phnsLeakTime )
    {
        *phnsLeakTime = hnsLeakTime;
    }

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

#endif /* DRM_SUPPORT_AUDIO_ONLY == 0 */

EXIT_PK_NAMESPACE_CODE;
