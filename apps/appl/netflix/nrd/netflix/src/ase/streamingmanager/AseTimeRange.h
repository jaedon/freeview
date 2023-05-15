/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
/*
 * This software module was originally developed by Qualcomm, Inc. in
 * the course of development of the ISO/IEC MPEG-B DASH standard for
 * reference purposes and its performance may not have been optimized.
 * This software module is an implementation of one or more tools as
 * specified by the ISO/IEC MPEG-B DASH standard.
 *
 * ISO/IEC gives users free license to this software module or
 * modifications thereof for use in products claiming conformance to
 * audiovisual and image-coding related ITU Recommendations and/or ISO/IEC
 * International Standards.
 *
 * ISO/IEC gives users the same free license to this software module or
 * modifications thereof for research purposes and further ISO/IEC
 * standardisation.
 *
 * Those intending to use this software module in products are advised that
 * its use may infringe existing patents. ISO/IEC have no liability for use
 * of this software module or modifications thereof.  Copyright is not
 * released for products that do not conform to audiovisual and
 * image-coding related ITU Recommendations and/or ISO/IEC International
 * Standards.
 *
 * Qualcomm, Inc. retains full right to modify and use the code for its own
 * purpose, assign or donate the code to a third party and to inhibit third
 * parties from using the code for products that do not conform to
 * audiovisual and image-coding related ITU Recommendations and/or ISO/IEC
 * International Standards.
 *
 * This copyright notice must be included in all copies or derivative
 * works.  Copyright (c) ISO/IEC 2010.
 */

/*
 * This module has been modified from its original version by Netflix
 */

#ifndef ASE_TIME_RANGE_H
#define ASE_TIME_RANGE_H

#include <nrdase/AseTimeStamp.h>

namespace netflix {
namespace ase
{

/** An interval of time.
 *
 *  The interval is closed at the start and open at the end:
 *  [ start, start + duration )
 */
class AseTimeRange
{
/* Member functions */
public:
    AseTimeRange( );

    AseTimeRange( AseTimeStamp const& start, AseTimeStamp const& duration );

    AseTimeRange( AseTimeRange const& other );

    AseTimeRange& operator=( AseTimeRange const& other );

    AseTimeStamp getStart() const;

    AseTimeStamp getDuration() const;

    AseTimeStamp getEnd() const;

    bool operator<( AseTimeStamp const& other ) const;

    bool operator>( AseTimeStamp const& other ) const;


    /** Shift the interval forwards or backwards in time */
    AseTimeRange& shiftForward( AseTimeStamp const& shift );
    AseTimeRange& shiftBackward( AseTimeStamp const& shift );

    /** Extend or reduce the duration */
    AseTimeRange& extend( AseTimeStamp const& extend );
    AseTimeRange& reduce( AseTimeStamp const& reduce );

/* Member data */
protected:
    AseTimeStamp   mStart;
    AseTimeStamp   mDuration;
};

inline AseTimeRange::AseTimeRange( )
{
}

inline AseTimeRange::AseTimeRange( AseTimeStamp const& start, AseTimeStamp const& duration )
    : mStart( start )
    , mDuration( duration )
{
}

inline AseTimeRange::AseTimeRange( AseTimeRange const& other )
    : mStart( other.mStart )
    , mDuration( other.mDuration )
{
}

inline AseTimeRange& AseTimeRange::operator=( AseTimeRange const& other )
{
    mStart = other.mStart;
    mDuration = other.mDuration;
    return *this;
}

inline AseTimeStamp AseTimeRange::getStart() const
{
    return mStart;
}

inline AseTimeStamp AseTimeRange::getDuration() const
{
    return mDuration;
}

inline AseTimeStamp AseTimeRange::getEnd() const
{
    return mStart + mDuration;
}

inline bool AseTimeRange::operator<( AseTimeStamp const& other ) const
{
    return other < mStart;
}

inline bool AseTimeRange::operator>( AseTimeStamp const& other ) const
{
    return other >= getEnd();
}

inline AseTimeRange& AseTimeRange::shiftForward( AseTimeStamp const& shift )
{
    mStart += shift;
    return *this;
}

inline AseTimeRange& AseTimeRange::shiftBackward( AseTimeStamp const& shift )
{
    if (mStart >= shift)
    {
        mStart -= shift;
    }
    else
    {
        mStart = AseTimeStamp::ZERO;
    }

    return *this;
}

inline AseTimeRange& AseTimeRange::extend( AseTimeStamp const& extend )
{
    mDuration += extend;
    return *this;
}

inline AseTimeRange& AseTimeRange::reduce( AseTimeStamp const& reduce )
{
    if (mDuration >= reduce)
    {
        mDuration -= reduce;

    }
    else
    {
        mDuration = AseTimeStamp::ZERO;
    }

    return *this;
}

}} /* end namespace ase */

#endif /* ASE_TimeRange_H */
