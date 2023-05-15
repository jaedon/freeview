/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TransportReporter.h"

#include "Ticks.h"
#include <nrdbase/Log.h>

#include <set>

using namespace std;
using namespace netflix;
using namespace netflix::ase;

#if !defined(BUILD_DEBUG)
    #define CHECK_EVENT_NAME(eventName) eventName
#else
    static inline const char * checkEventName(const char * eventName, const char * functionName)
    {

#if defined(NRDP_PLATFORM_X360) || defined(NRDP_PLATFORM_XB1)
            const char * baseFunctionName = strrchr(functionName, ':');
            assert(baseFunctionName && !strcmp(eventName, baseFunctionName + 1));
#else
            assert(!strcmp(eventName, functionName));
#endif

        return eventName;
    }

#define CHECK_EVENT_NAME(eventName) checkEventName(eventName, __FUNCTION__)
#endif

static std::string mediaTypeToString( MediaType mediaType )
{
    switch(mediaType)
    {
        case MEDIA_AUDIO:
            return "audio";
        case MEDIA_VIDEO:
            return "video";
        case MEDIA_HEADERS:
            return "headers";
        case MEDIA_TEXT:
            return "timedtext";
        default:
            break;
    }

    return "unknown";
}

static std::string networkErrorString(AseErrorCode err)
{
    switch (err)
    {
        case AS_DNS_ERROR:
        case AS_DNS_NO_DATA:
        case AS_DNS_QUERY_MALFORMED:
        case AS_DNS_SERVER_FAILURE:
        case AS_DNS_NOT_IMPLEMENTED:
        case AS_DNS_QUERY_REFUSED:
        case AS_DNS_BAD_NAME:
        case AS_DNS_NO_MEMORY:
        case AS_DNS_CHANNEL_DESTROYED:      return "dnsfailure";
        case AS_DNS_TIMEOUT:                return "dnstimeout";
        case AS_DNS_NOT_FOUND:              return "dnsnotfound";
        case AS_DNS_CONNECTION_REFUSED:     return "dnsconnectionrefused";

        case AS_CONNECTION_ERROR:           return "connectionerror";
        case AS_CONNECTION_REFUSED:         return "connectionrefused";
        case AS_CONNECTION_TIMEOUT:
        case AS_HTTP_CONNECTION_TIMEOUT:    return "connectiontimeout";
        case AS_CONNECTION_CLOSED:          return "connectionclosed";
        case AS_CONNECTION_RESET:           return "connectionreset";
        case AS_CONNECTION_RESET_WHILE_RECEIVING: return "connectionresetrecv";
        case AS_CONNECTION_RESET_ON_CONNECT: return "connectionresetconn";
        case AS_CONNECTION_NET_UNREACHABLE: return "noroutetohost";
        case AS_CONNECTION_SLOW:            return "connectionslow";

        case AS_SOCKET_ERROR:               return "socketerror";

        case AS_TIMEOUT:
        case AS_HTTP_CONNECTION_STALL:
        case AS_HTTP_TRANSACTION_TIMEOUT:   return "datatimeout";
        case AS_ABANDONED:                  return "abandoned";

        case AS_HTTP_PROTOCOL_ERROR:        return "httperror";

        case AS_INVALID_VALUE:              return "badcontent";


        // these don't map to a network error string
        case AS_NO_ERROR:
        case AS_UNKNOWN_ERROR:
        case AS_STALE_CREDENTIALS:
        case AS_DRM_FAILURE:
        case AS_MEDIA_HEADER_ERROR:
        case AS_MEDIA_INDEX_ERROR:
        case AS_DOMAIN_SELECTION_FAILURE:
            return std::string();

        // these will have http error codes instead of network strings
        case AS_HTTP_RESPONSE_3XX:
        case AS_HTTP_RESPONSE_4XX:
        case AS_HTTP_RESPONSE_5XX:
            return std::string();

        default:
            Log::warn(TRACE_PLAYBACK_REPORTER, "Unconverted AseErrorCode: %d", err);
            return std::string();
    }
}

void TransportReporter::tcpConnect(
                              uint32_t dltrackid,
                              int id,
                              int httpId,
                              AseUrl::HostPort const& hostport,
                              std::string const& src,
                              std::string const& dest,
                              MediaType mediaType,
                              int connectTimeMs,
                              int dnsLookupTimeMs )
{
    Variant args;
    Variant hp;
    args["dltrackid"] = dltrackid;
    args["id"] = id;
    args["httpId"] = httpId;
    hp["serverHostName"] = hostport.first;
    hp["serverPort"] = hostport.second;
    args["hostport"] = hp;
    args["src"] = src;
    args["dest"] = dest;
    args["mediaType"] = mediaTypeToString( mediaType );
    args["connectTimeMs"] = connectTimeMs;
    args["dnsLookupTimeMs"] = dnsLookupTimeMs;
    args["nativetime"] = Time::mono().ms();

    sendEvent( CHECK_EVENT_NAME("tcpConnect"), args );
}

void TransportReporter::tcpFailure(
                          uint32_t dltrackid,
                          int id,
                          int httpId,
                          AseUrl::HostPort const& hostport,
                          std::string const& src,
                          std::string const& dest,
                          MediaType mediaType,
                          int connectTimeMs,
                          int dnsLookupTimeMs,
                          AseErrorCode reason,
                          int lowLevelStatus )
{
    Variant args;
    Variant hp;
    args["dltrackid"] = dltrackid;
    args["id"] = id;
    args["httpId"] = httpId;
    hp["serverHostName"] = hostport.first;
    hp["serverPort"] = hostport.second;
    args["hostport"] = hp;
    args["src"] = src;
    args["dest"] = dest;
    args["mediaType"] = mediaTypeToString( mediaType );
    args["connectTimeMs"] = connectTimeMs;
    args["dnsLookupTimeMs"] = dnsLookupTimeMs;
    args["reason"] = reason;
    args["lowLevelStatus"] = lowLevelStatus;
    args["nativetime"] = Time::mono().ms();

    sendEvent( CHECK_EVENT_NAME("tcpFailure"), args );
}

void TransportReporter::tcpDisconnect(
                           uint32_t dltrackid,
                           int id,
                           int httpId,
                           int reason,
                           TcpDisconnectState tcpDisconnectState,
                           bool midResponse )
{
    Variant args;
    args["dltrackid"] = dltrackid;
    args["id"] = id;
    args["httpId"] = httpId;
    args["tcpDisconnectState"] = tcpDisconnectState;
    args["midResponse"] = midResponse;
    args["reason"] = reason;
    args["nativetime"] = Time::mono().ms();

    sendEvent(CHECK_EVENT_NAME("tcpDisconnect"), args);
}

void TransportReporter::downloadRecord(uint32_t dltrackid, const HttpAttempt &newHttpAttempt)
{
    // make sure the ManifestUrl is good
#if 0
    shared_ptr<const ManifestUrl> manifestUrl = newHttpAttempt.mOriginalUrl;
    if (!manifestUrl.get())
    {
        Log::error(TRACE_PLAYBACK_REPORTER, "Invalid ManifestUrl in HttpAttempt");
        return;
    }

    // get the cdnId
    uint32_t cdnId = manifestUrl->getCdnId();


    // get the ManifestStream
    shared_ptr<ManifestStream> stream = manifestUrl->getParent();
    if (!stream.get())
    {
        Log::error(TRACE_PLAYBACK_REPORTER,
                   "Unable to get ManifestStream from ManifestUrl");
        return;
    }

    // get downloadableId
    std::string dlid;
    ContentProfile profile;
    if (!ManifestUtilities::parseStreamKey(/*out*/dlid, /*out*/profile,
                                           stream->getStreamKey()))
    {
        Log::error(TRACE_PLAYBACK_REPORTER, "Unable to parse StreamKey %s",
                   stream->getStreamKey().c_str());
        return;
    }

    // determine type of download
    std::string dlidTag;
    switch (stream->getMimeType().getType())
    {
        case MimeType::MIME_TYPE_VIDEO:
            dlidTag = "video";
            break;

        case MimeType::MIME_TYPE_AUDIO:
            dlidTag = "audio";
            break;

        case MimeType::MIME_TYPE_TEXT:
            dlidTag = "timedtext";
            break;

        default:
            Log::error(TRACE_PLAYBACK_REPORTER, "Invalid MimeType: %d",
                       stream->getMimeType().getType());
            return;
    }
#endif

    AseErrorCode lastError = newHttpAttempt.mErrorCode;
    if ((lastError == AS_CONNECTION_TIMEOUT) && (newHttpAttempt.mBrecv != 0))
    {
        Log::error(TRACE_PLAYBACK_REPORTER,
                   "downloadRecord reported connection timeout but recevied %d bytes",
                   newHttpAttempt.mBrecv);

        // change the error to a datatimeout
        lastError = AS_HTTP_CONNECTION_STALL;
    }

    std::string networkError;
    if ( (lastError != AS_NO_ERROR) && (lastError != AS_TIMEOUT) )
    {
        // according to current logging spec. if the http request manager does not
        // download the amount of data for bandwidth measurement on time, it is not a
        // network error.
        networkError = networkErrorString(lastError);
        if (networkError.empty())
            networkError = StringFormatter::sformat("%d", lastError);

        Log::info(TRACE_PLAYBACK_REPORTER, "Error in downloadRecord: %d", lastError );
    }

    Variant args;
    Variant httpAttempt;
    int const interval = newHttpAttempt.mTinterval;
    httpAttempt["mRange"] = std::make_pair(newHttpAttempt.mRange.start(), newHttpAttempt.mRange.end());
    httpAttempt["mBrecv"] = newHttpAttempt.mBrecv;
    httpAttempt["mErrorCode"] = newHttpAttempt.mErrorCode;
    httpAttempt["mHrecv"] = newHttpAttempt.mHrecv;
    httpAttempt["mLocation"] = newHttpAttempt.mLocation.str();
    httpAttempt["mOpenRange"] = newHttpAttempt.mOpenRange;
 //   args["mOpenRange"] = newHttpAttempt.mOriginalUrl; not used
    httpAttempt["mPipelined"] = newHttpAttempt.mPipelined;
    httpAttempt["mServerTcpInfo"] = newHttpAttempt.mServerTcpInfo;
    if ( newHttpAttempt.mTcomp != AseTimeVal::ZERO )
        httpAttempt["mTcomp"] = newHttpAttempt.mTcomp.convertTo<TimeMono>().ms();
    httpAttempt["mTcpId"] = newHttpAttempt.mTcpId;
    httpAttempt["mTinterval"] = interval;
    if ( newHttpAttempt.mTresp != AseTimeVal::ZERO )
        httpAttempt["mTresp"] = newHttpAttempt.mTresp.convertTo<TimeMono>().ms();
    httpAttempt["mTreq"] = newHttpAttempt.mTreq.convertTo<TimeMono>().ms();
    httpAttempt["mUrl"] = newHttpAttempt.mUrl.str();
    httpAttempt["mHttpCode"] = newHttpAttempt.mHttpCode;
    if ( newHttpAttempt.mTtrace != newHttpAttempt.mTresp )
        httpAttempt["mTtrace"] = newHttpAttempt.mTtrace.convertTo<TimeMono>().ms();

    Variant mTrace;
    if ( !newHttpAttempt.mTrace.empty() )
    {
        // Here we need to convert from the (start,end,*bytes) entries in the httpattempt to
        // the ( duration, *bytes ) format of the download record
        AseTimeVal current = newHttpAttempt.mTtrace;
        BwTrace::const_iterator it = newHttpAttempt.mTrace.begin();

        it->checkInterval( interval );

        int ms = 0;
        for( ; it != newHttpAttempt.mTrace.end(); ++it )
        {
            AseTimeVal start = it->mStart;
            const AseTimeVal end = it->mEnd;
            if ( start > current )
            {
                int const gap = ( start - current ).ms();
                if ( gap > 100 || mTrace.empty() )
                {
                    mTrace.push_back( std::make_pair( gap, -1 ) );
                }
                else
                {
                    ms += gap;
                    mTrace.back()[ 0 ] = ms;
                    if ( ms > gap && ms % interval <= gap )
                    {
                        mTrace.back().push_back( 0 );
                    }
                }

                current += AseTimeVal::fromMS( (ullong) gap );
            }

            ms = ( end - current ).ms();

            Variant entry;
            entry.push_back( ms );
            if ( !it->mTrace.empty() )
            {
                entry.insert( entry.size(), it->mTrace.begin(), it->mTrace.end() );
            }
            else
            {
                entry.push_back( 0 );
            }

            mTrace.push_back( entry );

            current += AseTimeVal::fromMS( (ullong) ms );
        }
    }

    httpAttempt["mTrace"] = mTrace;
    args["dltrackid"] = dltrackid;
    args["newHttpAttempt"] = httpAttempt;
    args["nativetime"] = Time::mono().ms();
    //args["originalUrl"] = originalUrl.str();
    //args["cdnId"] = cdnId;
    //args["dlid"] = dlid;
    //args["dlidTag"] = dlidTag;
    if ( !networkError.empty() ) args["networkError"] = networkError;
    //if ( !contentError.empty() ) args["contentError"] = contentError;

    sendEvent(CHECK_EVENT_NAME("downloadRecord"), args);
}

void TransportReporter::aggregateTrace(const BwEntry &bwEntry){

    Variant args;
    Variant bwentry;

    bwentry["start"] = bwEntry.mStartMono.ms();
    bwentry["end"] = bwEntry.mStartMono.ms() + ( bwEntry.mEnd.ms() - bwEntry.mStart.ms() );
    bwentry["interval"] = bwEntry.getInterval();
    bwentry["trace"] = bwEntry.mTrace;

    args["bwentry"] = bwentry;

    sendEvent(CHECK_EVENT_NAME("aggregateTrace"), args);
}

void TransportReporter::reportRoute(
        const std::string &dst, const std::vector<PacketTrace>& routePacketTraces)
{
    Variant args;
    args["dst"] = dst;
    Variant route;
    std::set<std::string> sources;
    std::vector<PacketTrace>::const_iterator it = routePacketTraces.begin();
    for( ; it != routePacketTraces.end(); ++it)
    {
         sources.insert( it->mLocalIp );
    }
    for( it = routePacketTraces.begin(); it != routePacketTraces.end(); ++it)
    {
       Variant entry;
       entry["num"] = it->mHopNumber;
       entry["ip"] = it->mNodeIp;
       entry["rtt"] = it->mRtt;

       if ( sources.size() > 1 )
       {
           entry["src"] = it->mLocalIp;
       }

       route.push_back( entry );
    }
    args["routePacketTraces"] = route;
    sendEvent(CHECK_EVENT_NAME("reportRoute"), args);
}

void TransportReporter::reportHostNameResolved(
        DnsQueryType dnsQueryType,
        const std::vector<std::string>& nameServers,
        int lookupTimeMs,
        const std::vector<std::string>& cnames,
        const std::vector<IpAddressTtlRecord>& ipAddressTtlRecords )
{
    Variant args;
    args["dnsQueryType"] = dnsQueryType;
    args["nameServers"] = nameServers;
    args["lookupTimeMs"] = lookupTimeMs;
    args["cnames"] = cnames;
    Variant ips;
    std::vector<IpAddressTtlRecord>::const_iterator iit = ipAddressTtlRecords.begin();
    for( ; iit != ipAddressTtlRecords.end(); ++iit )
    {
        Variant ip;
        ip["ip"] = iit->mIpAddress;
        ip["ttl"] = iit->mTtl;

        ips.push_back( ip );
    }
    args["ipAddressTtlRecords"] = ips;
    sendEvent(CHECK_EVENT_NAME("reportHostNameResolved"), args);
}

void TransportReporter::reportDnsLookupFailure(
    DnsQueryType dnsQueryType,
    const std::vector<std::string>& nameServers,
    int lookupTimeMs,
    const std::string& hostName,
    int32_t failureCode)
{
    Variant args;
    args["dnsQueryType"] = (int)dnsQueryType;
    args["nameServers"] = nameServers;
    args["lookupTimeMs"] = lookupTimeMs;
    args["hostName"] = hostName;
    args["failureCode"] = failureCode;
    args["nativetime"] = Time::mono().ms();
    sendEvent(CHECK_EVENT_NAME("reportDnsLookupFailure"), args);
}

void TransportReporter::reportNetworkMonitorState( Variant const& netmon )
{
    Variant args;
    args["netmon"] = netmon;
    sendEvent(CHECK_EVENT_NAME("reportNetworkMonitorState"), args);
}

void TransportReporter::setNetworkBytes(uint32_t bytes)
{
    Variant args;
    args["bytes"] = bytes;
    sendEvent(CHECK_EVENT_NAME("setNetworkBytes"), args);
}

void TransportReporter::addNetworkBytes(uint32_t bytes)
{
    NRDP_UNUSED(bytes);
}

