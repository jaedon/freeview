/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NetError.h"

#include <curl/curl.h>


namespace netflix {
namespace NetError {

//////////
//
// mapErrorGroup
//
//////////
NetErrorGroup::Group mapErrorGroup(NetErrorCode::Code ec)
{

    if (ec == NetErrorCode::SUCCESS) {
        return NetErrorGroup::SUCCESS;
    } else if ((ec >= NetErrorCode::NOTSECUREERROR) && (ec <= NetErrorCode::TIMEOUTERROR)) {
        return NetErrorGroup::CONNECTIVITY_ERROR;
    } else if (ec == NetErrorCode::DNS_ERROR) {
        return NetErrorGroup::NAMERESOLVEERROR;
    } else if ((ec >= NetErrorCode::SSLHANDSHAKEERROR) && (ec <= NetErrorCode::SSLCACERTFILEERROR)) {
        return NetErrorGroup::SSLERROR;
    } else if (static_cast<int>(ec) >= 200) {
        return NetErrorGroup::HTTP_ERROR;
    }
    return NetErrorGroup::UNKNOWN_ERROR;
}

//////////
//
// mapRecommendedAction
//
//////////
RecommendedAction mapRecommendedAction(NetErrorCode::Code /*ec*/, NetErrorGroup::Group /*eg*/)
{
    RecommendedAction ra = RunDiagTool;
    return ra;
}
/*
 * * NOTE: Server teams run reports based on the error codes and error groups.
 * * Please do not change the meaning of the values. If new error codes, groups
 * * needs to be added/integrated from a different stack, make sure the values
 * * do not overlap with any of the current ones.
 */
NetErrorCode::Code nativeErrorCodeToNetError(int nativeErroCode)
{
    switch (nativeErroCode) {
    case AS_NO_ERROR:
        return NetErrorCode::SUCCESS;

    case AS_DNS_ERROR:
    case AS_DNS_NOT_FOUND:
    case AS_DNS_NO_DATA:
    case AS_DNS_QUERY_MALFORMED:
    case AS_DNS_SERVER_FAILURE:
    case AS_DNS_NOT_IMPLEMENTED:
    case AS_DNS_QUERY_REFUSED:
    case AS_DNS_BAD_NAME:
    case AS_DNS_CONNECTION_REFUSED:
    case AS_DNS_NO_MEMORY:
    case AS_DNS_CHANNEL_DESTROYED:
    case AS_DNS_RESPONSE_MALFORMED:
    case AS_DNS_FORMAT_ERROR:
    case AS_DNS_BAD_FAMILY:
    case AS_DNS_EOF:
    case AS_DNS_FILE:
    case AS_DNS_BAD_STRING:
    case AS_DNS_BAD_FLAGS:
    case AS_DNS_NO_NAME:
    case AS_DNS_BAD_HINTS:
    case AS_DNS_NOT_INITIALIZED:
    case AS_DNS_CANCELLED:
        return NetErrorCode::DNS_ERROR;

    case AS_CONNECTION_NOT_OPEN:
    case AS_CONNECTION_ERROR:
    case AS_CONNECTION_REFUSED:
    case AS_CONNECTION_NET_UNREACHABLE:
    case AS_CONNECTION_RESET:
    case AS_CONNECTION_CLOSED:
    case AS_CONNECTION_INVALID_DESTINATION:
        return NetErrorCode::CONNECT_ERROR;

    case AS_HTTP_PROTOCOL_ERROR:
    case AS_HTTP_PROTOCOL_1_0:
    case AS_HTTP_RESPONSE_3XX:
    case AS_HTTP_RESPONSE_4XX:
    case AS_HTTP_RESPONSE_5XX:
    case AS_HTTP_PIPELINING_NOT_SUPPORTED:
    case AS_HTTP_PIPELINING_NOT_STABLE:
    case AS_HTTP_CONNECTION_ERROR:
    case AS_HTTP_TOO_MANY_REDIRECTS:
        return NetErrorCode::CONNECT_ERROR;

    case AS_SSL_ERROR:
        return NetErrorCode::SSLHANDSHAKEERROR;

    case AS_ILLEGAL_REQUEST:
        return NetErrorCode::SECURITYERROR;

    case AS_DNS_TIMEOUT:
    case AS_CONNECTION_TIMEOUT:
    case AS_HTTP_CONNECTION_STALL:
    case AS_HTTP_CONNECTION_TIMEOUT:
    case AS_HTTP_TRANSACTION_TIMEOUT:
        return NetErrorCode::TIMEOUTERROR;

    case AS_HTTP_INVALID_URL:
        return NetErrorCode::DATAURIERROR;

    case AS_COMPRESSION_ERROR:
        return NetErrorCode::COMPRESSIONERROR;

    case CURLE_COULDNT_RESOLVE_PROXY:
    case CURLE_COULDNT_RESOLVE_HOST:
        return NetErrorCode::DNS_ERROR;

    case CURLE_COULDNT_CONNECT:
        return NetErrorCode::CONNECT_ERROR;

    case CURLE_OPERATION_TIMEDOUT:
        return NetErrorCode::TIMEOUTERROR;

    case CURLE_SSL_CONNECT_ERROR:
        return NetErrorCode::SSLHANDSHAKEERROR;

    case CURLE_SSL_CACERT:
        return NetErrorCode::SSLCACERTERROR;

    case CURLE_SSL_CACERT_BADFILE:
        return NetErrorCode::SSLCACERTFILEERROR;

    case CURLE_SEND_ERROR:
        return NetErrorCode::SENDERROR;

    case CURLE_RECV_ERROR:
        return NetErrorCode::RECVERROR;
    default:
        break;
    }

    return NetErrorCode::NETWORKERROR;
}
}
}
