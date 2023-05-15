/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/StringUtils.h>
#include "AseCommonUtil.h"
#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


namespace netflix {
namespace ase {

/** Check if a URL has the specific destination host name and port number. */
bool checkUrlDestination(
                const AseUrl& url,
                const std::string& hostName,
                uint16_t portNumber )
{
    bool retVal = false;

    uint16_t urlPortNumber = 80;

    if (url.HasPort())
    {
        urlPortNumber = url.GetPortNumber();
    }

    if ((strCaseCmp( url.GetHost().c_str(), hostName.c_str() ) == 0)
        && (urlPortNumber == portNumber))
    {
        retVal = true;
    }

    return retVal;
}

/** Get TCP disconnect state name. */
const char* getTcpDisconnectStateName( TcpDisconnectState tcpDisconnectState )
{
    const char* pRetVal = "Unknown";

    switch (tcpDisconnectState)
    {
        case TCP_DISCONNECT_UNKNOWN_STATE:
        {
            pRetVal = "Unknown";
            break;
        }

        case TCP_DISCONNECT_BY_LOCAL:
        {
            pRetVal = "ByLocal";
            break;
        }

        case TCP_DISCONNECT_BY_REMOTE_WHILE_IDLE:
        {
            pRetVal = "ByRemoteWhileIdle";
            break;
        }

        case TCP_DISCONNECT_BY_REMOTE_WHILE_SENDING:
        {
            pRetVal = "ByRemoteWhileSending";
            break;
        }

        case TCP_DISCONNECT_BY_REMOTE_WHILE_RECEIVING:
        {
            pRetVal = "ByRemoteWhileReceiving";
            break;
        }
    }

    return pRetVal;
}

/** Convert the byte range of a range request to a range request header. */
std::pair<std::string, std::string> convertByteRangeToHeader( ByteRange const& byteRange )
{
    char byteRangeValue[64];

    if (byteRange.getEnd() != -1)
    {
        snprintf(
            byteRangeValue,
            64,
            "bytes=%lld-%lld",
            byteRange.getStart(),
            byteRange.getEnd() );
    }
    else
    {
        snprintf(
            byteRangeValue,
            64,
            "bytes=%lld-",
            byteRange.getStart() );
    }

    return std::make_pair( std::string( "Range" ), std::string( byteRangeValue) );
}

}}

