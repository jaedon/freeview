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
#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include <nrdase/AseCommonDataTypes.h>

namespace netflix {
namespace ase {

/** Stores an offset or a duration in an (almost) arbitrary time scale.
 *
 *  A AseTimeStamp stores a number of ticks in a selectable timescale.
 *  The timescale is measured in ticks per second, which has to be an
 *  integer.
 *
 *  AseTimeStamps can be added, subtracted and compared.  Doing so is
 *  guaranteed not to cause rounding errors.  (Thus the sum of two
 *  AseTimeStamps may need to be in a finer scale than either scale of the
 *  operands.)
 */
class AseTimeStamp
{
    /* Member functions */
    public:
        AseTimeStamp();

        AseTimeStamp( ullong nAseTimeStamp, ullong nTimescale );

        AseTimeStamp( AseTimeStamp const& other );

        AseTimeStamp( AseTimeStamp const& other, ullong nTimescale );

        AseTimeStamp& operator=( AseTimeStamp const& other );

        bool operator==( AseTimeStamp const& other ) const;

        bool operator!=( AseTimeStamp const& other ) const;

        bool operator<( AseTimeStamp const& other ) const;

        bool operator>( AseTimeStamp const& other ) const;

        bool operator<=( AseTimeStamp const& other ) const;

        bool operator>=( AseTimeStamp const& other ) const;

        AseTimeStamp& operator+=( AseTimeStamp const& other );

        AseTimeStamp& operator-=( AseTimeStamp const& other );

        AseTimeStamp operator+( AseTimeStamp const& other ) const;

        AseTimeStamp operator-( AseTimeStamp const& other ) const;

        ullong operator/( AseTimeStamp const& other ) const;

        AseTimeStamp operator*( ullong const& other ) const;

        AseTimeStamp& operator*=( ullong const& other );

        bool isInfinite() const;

        bool isZero() const;

        void set( ullong nAseTimeStamp, ullong nTimescale );

        void changeTimescale( ullong nTimescale );

        /* Querying */

        ullong getNumberOfTicks() const;

        ullong getTicksPerSecond() const;

        float getValueInSeconds() const;
        ullong getValueInMs() const;

        static const ullong msecPerSecond = 1000;

        static const ullong infiniteNumber = 0xFFFFFFFFFFFFFFFFULL;

        static AseTimeStamp const ZERO;
        static AseTimeStamp const INFINITE;

    /* Member data */
    protected:
        ullong        mnAseTimeStamp;    // ticks
        ullong        mnTimescale;    // ticks per second
};

typedef AseTimeStamp AseTimeInterval;

ullong operator/( ullong const& lhs, AseTimeStamp const& rhs );
ullong operator*( ullong const& lhs, AseTimeStamp const& rhs );

inline AseTimeStamp::AseTimeStamp()
    : mnAseTimeStamp( 0ULL )
    , mnTimescale( 1ULL )
{
}

inline AseTimeStamp::AseTimeStamp( ullong nAseTimeStamp, ullong nTimescale )
    : mnAseTimeStamp( nAseTimeStamp )
    , mnTimescale( nTimescale )
{
    assert( nTimescale > 0ULL );
}

inline AseTimeStamp::AseTimeStamp( AseTimeStamp const& other )
    : mnAseTimeStamp( other.mnAseTimeStamp )
    , mnTimescale( other.mnTimescale )
{
}

inline AseTimeStamp& AseTimeStamp::operator=( AseTimeStamp const& other )
{
    mnAseTimeStamp = other.mnAseTimeStamp;
    mnTimescale = other.mnTimescale;

    return *this;
}

inline bool AseTimeStamp::operator!=( AseTimeStamp const& other ) const
{
    return !( *this == other );
}

inline bool AseTimeStamp::operator>( const AseTimeStamp& other ) const
{
    return !((*this) == other) && !((*this) < other);
}

inline bool AseTimeStamp::operator<=( AseTimeStamp const& other ) const
{
    return !( *this > other );
}

inline bool AseTimeStamp::operator>=( AseTimeStamp const& other ) const
{
    return !( *this < other );
}

inline AseTimeStamp AseTimeStamp::operator+( AseTimeStamp const& other ) const
{
    return AseTimeStamp( *this ) += other;
}

inline AseTimeStamp AseTimeStamp::operator-( AseTimeStamp const& other ) const
{
    return AseTimeStamp( *this ) -= other;
}

inline AseTimeStamp AseTimeStamp::operator*( ullong const& other ) const
{
    return AseTimeStamp( *this ) *= other;
}

inline AseTimeStamp& AseTimeStamp::operator*=( ullong const& other )
{
    if (mnAseTimeStamp != infiniteNumber)
    {
        mnAseTimeStamp *= other;
    }

    return *this;
}

inline bool AseTimeStamp::isZero() const
{
    return mnAseTimeStamp == 0ULL;
}

inline bool AseTimeStamp::isInfinite() const
{
    return mnAseTimeStamp == infiniteNumber;
}

}}
#endif
