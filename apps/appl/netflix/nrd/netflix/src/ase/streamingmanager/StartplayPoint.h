/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STARTPLAY_POINT_H
#define STARTPLAY_POINT_H
namespace netflix {
namespace ase {
struct StartplayPoint
{
    /** If the player downloads data faster than mBitrateInBps for mTimeDuration,
     * it is ok to startplay without rebuffering.
     */

    StartplayPoint( int bps, AseTimeInterval const& time );

    int mBitrateInBps;
    AseTimeInterval mTimeDuration;
};

inline StartplayPoint::StartplayPoint( int bps, AseTimeInterval const& time )
    : mBitrateInBps( bps )
    , mTimeDuration( time )
{
}

}}

#endif
