/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <arpa/inet.h>

#include "NrdIpAddr.h"

namespace netflix {

/**
 * A generic IP address class.
 */

/** Constructor. */
NrdIpAddr::NrdIpAddr() : mValidFlag( false )
{
}


NrdIpAddr::NrdIpAddr( const NrdIpAddr& nrdIpAddr ) : mValidFlag( nrdIpAddr.mValidFlag ),
                                                     mVersion( nrdIpAddr.mVersion )
{
    mInAddr.s_addr = nrdIpAddr.mInAddr.s_addr;

#ifdef NRDP_HAS_IPV6
    memcpy( mIn6Addr.s6_addr, nrdIpAddr.mIn6Addr.s6_addr, 16 );
    mIn6ScopeId = nrdIpAddr.mIn6ScopeId;
#endif
}

/** Constructor */
NrdIpAddr::NrdIpAddr( const std::string& strIpAddr )
{
    setStrAddr( strIpAddr );
}

NrdIpAddr::NrdIpAddr( struct in_addr inAddr ) : mValidFlag( true ),
                                                mVersion( IP_V4 )
{
    mInAddr.s_addr = inAddr.s_addr;
}

#ifdef NRDP_HAS_IPV6
NrdIpAddr::NrdIpAddr(
                struct in6_addr in6Addr,
                const std::string& in6ScopeId) :  mValidFlag( true ),
                                                  mVersion( IP_V6 ),
                                                  mIn6ScopeId( in6ScopeId )
{
    memcpy( mIn6Addr.s6_addr, in6Addr.s6_addr, 16 );
}
#endif

/** Destructor. */
NrdIpAddr::~NrdIpAddr()
{
}

/** Assign operators. */
const NrdIpAddr& NrdIpAddr::operator=( const NrdIpAddr& nrdIpAddr )
{
    if (this != &nrdIpAddr)
    {
        mValidFlag = nrdIpAddr.mValidFlag;
        mVersion = nrdIpAddr.mVersion;

        mInAddr.s_addr = nrdIpAddr.mInAddr.s_addr;

#ifdef NRDP_HAS_IPV6
        memcpy( mIn6Addr.s6_addr, nrdIpAddr.mIn6Addr.s6_addr, 16 );
        mIn6ScopeId = nrdIpAddr.mIn6ScopeId;
#endif
    }

    return *this;
}

/** Check if the IP address is valid. */
bool NrdIpAddr::valid() const
{
    return mValidFlag;
}

/** Get the IP version. */
NrdIpAddr::Version NrdIpAddr::getVersion() const
{
    return mVersion;
}

/** Setter/Getter functions. */
void NrdIpAddr::setStrAddr( const std::string& strIpAddr )
{
    int retVal;

    mValidFlag = false;

    //Try to convert to IPv4 address.
    retVal = inet_pton(
                AF_INET,
                strIpAddr.c_str(),
                &mInAddr );

    if (retVal == 1)
    {
        mValidFlag = true;
        mVersion = IP_V4;
    }
#ifdef NRDP_HAS_IPV6
    else
    {
        //Try to convert to IPv4 address.

        //Check if there is a "%" for scope id.
        size_t percentPos = strIpAddr.find( '%' );

        std::string addrPart;

        mIn6ScopeId = "";

        if (percentPos != std::string::npos)
        {
            addrPart = strIpAddr.substr( 0, percentPos );

            if (percentPos < (strIpAddr.length() - 1))
            {
                mIn6ScopeId = strIpAddr.substr(
                                            percentPos + 1,
                                            std::string::npos );
            }
        }
        else
        {
            addrPart = strIpAddr;
        }

        retVal = inet_pton(
                    AF_INET6,
                    addrPart.c_str(),
                    &mIn6Addr );

        if (retVal == 1)
        {
            mValidFlag = true;
            mVersion = IP_V6;
        }
    }
#endif
}

std::string NrdIpAddr::getStrAddr() const
{
    std::string retVal = "";

    if (mValidFlag)
    {
        if (mVersion == IP_V4)
        {
            char tempStr[128];

            const char* pTemp;

            pTemp = inet_ntop(
                        AF_INET,
                        &mInAddr,
                        tempStr,
                        128 );

            if (pTemp)
            {
                retVal = pTemp;
            }
        }
#ifdef NRDP_HAS_IPV6
        else
        {
            char tempStr[128];

            const char* pTemp;

            pTemp = inet_ntop(
                        AF_INET6,
                        &mIn6Addr,
                        tempStr,
                        128 );

            if (pTemp)
            {
                retVal = pTemp;

                if (!mIn6ScopeId.empty())
                {
                    retVal += "%";
                    retVal += mIn6ScopeId;
                }
            }
        }
#endif
    }

    return retVal;
}

void NrdIpAddr::setInAddr( struct in_addr inAddr )
{
    mValidFlag = true;
    mVersion = IP_V4;
    mInAddr = inAddr;
}

struct in_addr NrdIpAddr::getInAddr() const
{
    return mInAddr;
}

#ifdef NRDP_HAS_IPV6

void NrdIpAddr::setIn6Addr( struct in6_addr in6Addr )
{
    mValidFlag = true;
    mVersion = IP_V6;
    memcpy( mIn6Addr.s6_addr, in6Addr.s6_addr, 16 );
}

struct in6_addr NrdIpAddr::getIn6Addr() const
{
    return mIn6Addr;
}

void NrdIpAddr::setIn6ScopeId( const std::string& in6ScopeId )
{
    mIn6ScopeId = in6ScopeId;
}

const std::string& NrdIpAddr::getIn6ScopeId() const
{
    return mIn6ScopeId;
}

#endif

/** Comparison operators. */
bool NrdIpAddr::operator==( const NrdIpAddr& rhs ) const
{
    bool retVal = false;

    if (mValidFlag && rhs.mValidFlag)
    {
        if ((mVersion == IP_V4) && (rhs.mVersion == IP_V4))
        {
            retVal = (mInAddr.s_addr == rhs.mInAddr.s_addr);
        }
#ifdef NRDP_HAS_IPV6
        else if ((mVersion == IP_V6) && (rhs.mVersion == IP_V6))
        {
            retVal = (memcmp(
                        mIn6Addr.s6_addr,
                        rhs.mIn6Addr.s6_addr,
                        16 ) == 0) && (mIn6ScopeId == rhs.mIn6ScopeId);
        }
#endif
    }

    return retVal;
}

bool NrdIpAddr::operator!=( const NrdIpAddr& rhs ) const
{
    return !(*this == rhs);
}

bool NrdIpAddr::operator<( const NrdIpAddr& rhs ) const
{
    bool retVal = false;

    if (mValidFlag && rhs.mValidFlag)
    {
        if ((mVersion == IP_V4) && (rhs.mVersion == IP_V6))
        {
            retVal = true;
        }
        else if ((mVersion == IP_V4) && (rhs.mVersion == IP_V4))
        {
            retVal = (mInAddr.s_addr < rhs.mInAddr.s_addr);
        }
#ifdef NRDP_HAS_IPV6
        else if ((mVersion == IP_V6) && (rhs.mVersion == IP_V6))
        {
            retVal = (memcmp(
                        mIn6Addr.s6_addr,
                        rhs.mIn6Addr.s6_addr,
                        16 ) < 0)
                     || ((memcmp(
                            mIn6Addr.s6_addr,
                            rhs.mIn6Addr.s6_addr,
                            16 ) == 0) && (mIn6ScopeId < rhs.mIn6ScopeId));

        }
#endif
    }

    return retVal;
}

}

