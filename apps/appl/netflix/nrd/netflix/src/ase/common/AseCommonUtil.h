/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ASE_COMMON_UTIL_H
#define ASE_COMMON_UTIL_H

#include <nrdbase/StdInt.h>
#include <string>
#include <sstream>
#include <iostream>
#include <string>

#include <nrdnet/AseUrl.h>
#include <nrdnet/NetCommonDataTypes.h>

#include <nrdase/ByteRange.h>
#include <nrdase/AseCommonDataTypes.h>

namespace netflix {
namespace ase {

template <class T>
bool validateValue( T& value, T min, T max, T defaultValue )
{
    bool valid = true;
    if ( (value < min) || (value > max) )
    {
        value = defaultValue;
        valid = false;
    }
    return valid;
}

template <class T>
bool fromString(T& t,
        const std::string& s,
        std::ios_base& (*f)(std::ios_base&) = std::dec )
{
    if ( s == "" )
        return false;

    std::istringstream iss(s);
    return !(iss >> f >> t).fail();
}

template <class T>
T fromString(
        const std::string& s,
        T defaultValue,
        std::ios_base& (*f)(std::ios_base&) = std::dec )
{
    if (s == "")
    {
        return defaultValue;
    }

    T retVal = defaultValue;

    std::istringstream iss(s);

    if (!(iss >> f >> retVal).fail())
    {
        return retVal;
    }
    else
    {
        return defaultValue;
    }
}

template <class T>
std::string toString( T t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

/** Check if a URL has the specific destination host name and port number. */
bool checkUrlDestination(
                const AseUrl& url,
                const std::string& hostName,
                uint16_t portNumber );

/** Get TCP disconnect state name. */
const char* getTcpDisconnectStateName( TcpDisconnectState tcpDisconnectState );

/** Convert the byte range of a range request to a range request header. */
std::pair<std::string, std::string> convertByteRangeToHeader( ByteRange const& byteRange );

}}

#endif
