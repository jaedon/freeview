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
/**
 *
 * A class for handling arbitrary AseTimeStamps
 *
 * XXX: Overflow checking
 */

#include <nrdase/AseTimeStamp.h>

namespace netflix
{
namespace ase
{

// XXX Those are pretty generic math functions.  Put elsewhere?

static ullong gcd( const ullong& a, const ullong& b )
{
    return ( b == 0 ) ? a : gcd( b, a % b );
}

static ullong lcm( const ullong& a, const ullong& b )
{
    if ( a % b == 0 )
    {
        return a;
    }
    else if ( b % a == 0 )
    {
        return b;
    }
    else
    {
        return ( a * b ) / gcd( a, b );
    }
}

AseTimeStamp const AseTimeStamp::ZERO( 0ULL, 1ULL );
AseTimeStamp const AseTimeStamp::INFINITE( AseTimeStamp::infiniteNumber, 1ULL ) ;

AseTimeStamp::AseTimeStamp( const AseTimeStamp& other, ullong nTimescale )
{
    if (other != INFINITE)
    {
        const ullong l = lcm( nTimescale, other.mnTimescale );
        mnTimescale = nTimescale;
        mnAseTimeStamp = other.mnAseTimeStamp * ( l / other.mnTimescale ) / ( l / nTimescale );
    }
    else
    {
        mnTimescale = 1ULL;
        mnAseTimeStamp = infiniteNumber;
    }
}

bool AseTimeStamp::operator==( const AseTimeStamp& other ) const
{
    if ((mnAseTimeStamp != infiniteNumber) && (other.mnAseTimeStamp != infiniteNumber))
    {
        if ( mnTimescale == other.mnTimescale )
            return ( mnAseTimeStamp == other.mnAseTimeStamp );

        const ullong l = lcm( mnTimescale, other.mnTimescale );
        return AseTimeStamp( *this, l ) == AseTimeStamp( other, l );
    }
    else if ((mnAseTimeStamp == infiniteNumber) && (other.mnAseTimeStamp == infiniteNumber))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool AseTimeStamp::operator<( const AseTimeStamp& other ) const
{
    if ((mnAseTimeStamp != infiniteNumber) && (other.mnAseTimeStamp != infiniteNumber))
    {
        if ( mnTimescale == other.mnTimescale )
            return ( mnAseTimeStamp < other.mnAseTimeStamp );

        ullong nTimescale = lcm( mnTimescale, other.mnTimescale );
        return AseTimeStamp( *this, nTimescale ) < AseTimeStamp( other, nTimescale );
    }
    else if ((mnAseTimeStamp != infiniteNumber) && (other.mnAseTimeStamp == infiniteNumber))
    {
        return true;
    }
    else
    {
        return false;
    }
}

AseTimeStamp& AseTimeStamp::operator+=( const AseTimeStamp& other )
{
    if ((mnAseTimeStamp != infiniteNumber) && (other.mnAseTimeStamp != infiniteNumber))
    {
        if ( mnTimescale == other.mnTimescale )
            mnAseTimeStamp += other.mnAseTimeStamp;
        else
        {
            const ullong nTimescale = lcm( mnTimescale, other.mnTimescale );
            changeTimescale( nTimescale );
            *this += AseTimeStamp( other, nTimescale );
        }
    }
    else
    {
        *this = INFINITE;
    }

    return *this;
}

AseTimeStamp& AseTimeStamp::operator-=( const AseTimeStamp& other )
{
    if ((mnAseTimeStamp != infiniteNumber) && (other.mnAseTimeStamp != infiniteNumber))
    {
        if ( mnTimescale == other.mnTimescale )
            mnAseTimeStamp -= other.mnAseTimeStamp;
        else
        {
            ullong nTimescale = lcm( mnTimescale, other.mnTimescale );
            changeTimescale( nTimescale );
            *this -= AseTimeStamp( other, nTimescale );
        }
    }
    else if (other.mnAseTimeStamp == infiniteNumber)
    {
        *this = ZERO;
    }

    return *this;
}

ullong AseTimeStamp::operator/( const AseTimeStamp& other ) const
{
    if ((mnAseTimeStamp != infiniteNumber) && (other.mnAseTimeStamp != infiniteNumber))
    {
        ullong fac = gcd( mnTimescale, other.mnTimescale );
        return ( mnAseTimeStamp * ( other.mnTimescale / fac ) ) / ( ( mnTimescale / fac ) * other.mnAseTimeStamp );

    }
    else if ((mnAseTimeStamp != infiniteNumber) && (other.mnAseTimeStamp == infiniteNumber))
    {
        return 0ULL;
    }
    else if ((mnAseTimeStamp == infiniteNumber) && (other.mnAseTimeStamp != infiniteNumber))
    {
        return infiniteNumber;
    }
    else
    {
        return 1ULL;
    }
}

ullong operator*( ullong const& lhs, AseTimeStamp const& rhs )
{
    if (rhs != AseTimeStamp::INFINITE)
    {
        return lhs * rhs.getNumberOfTicks() / rhs.getTicksPerSecond();
    }
    else
    {
        return AseTimeStamp::infiniteNumber;
    }
}

ullong operator/( const ullong& lhs, const AseTimeStamp& rhs )
{
    if (rhs != AseTimeStamp::INFINITE)
    {
        return ( lhs * rhs.getTicksPerSecond() / rhs.getNumberOfTicks() );
    }
    else
    {
        return 0ULL;
    }
}

void AseTimeStamp::set( ullong nAseTimeStamp, ullong nTimescale )
{
    mnAseTimeStamp = nAseTimeStamp;
    mnTimescale = nTimescale;
}

void AseTimeStamp::changeTimescale( ullong nTimescale )
{
    if ( mnTimescale != nTimescale )
    {
        mnAseTimeStamp = ((ullong)( mnAseTimeStamp * nTimescale ))
                                    / (ullong)mnTimescale;
        mnTimescale = nTimescale;
    }
}

ullong AseTimeStamp::getNumberOfTicks() const
{
    return mnAseTimeStamp;
}

ullong AseTimeStamp::getTicksPerSecond() const
{
    return mnTimescale;
}

float AseTimeStamp::getValueInSeconds() const
{
    return (float)mnAseTimeStamp/(float)mnTimescale;
}

ullong AseTimeStamp::getValueInMs() const
{
    return mnAseTimeStamp * msecPerSecond / mnTimescale;
}

}} // end namespace ase
