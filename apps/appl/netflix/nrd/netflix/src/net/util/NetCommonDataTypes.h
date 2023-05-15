/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NET_COMMON_DATA_TYPES_H
#define NET_COMMON_DATA_TYPES_H

namespace netflix {

/** The state when a TCP connection is disconnected. */
typedef enum
{
    TCP_DISCONNECT_UNKNOWN_STATE = 0,
    TCP_DISCONNECT_BY_LOCAL,
    TCP_DISCONNECT_BY_REMOTE_WHILE_IDLE,
    TCP_DISCONNECT_BY_REMOTE_WHILE_SENDING,
    TCP_DISCONNECT_BY_REMOTE_WHILE_RECEIVING
} TcpDisconnectState;

/**The IP version. */
enum IpVersion
{
    IP_VERSION_4 = 0,
    IP_VERSION_6
};

/** The platform IP connectivity mode. */
enum IpConnectivityMode
{
    IP_V4_ONLY_MODE = 0,
    IP_V6_ONLY_MODE,
    IP_DUAL_STACK_MODE
};

inline std::string ipConnectivityModeToString(IpConnectivityMode mode)
{
    switch(mode)
    {
    case IP_DUAL_STACK_MODE: return "dual";
    case IP_V6_ONLY_MODE: return "6";
    case IP_V4_ONLY_MODE: return "4";
    default: return "";
    }
}

enum DnsQueryType
{
    DNS_QUERY_SINGLE_A_TYPE = 0,
    DNS_QUERY_QUAD_A_TYPE
};

inline std::string dnsQueryTypeToString(DnsQueryType type)
{
    if(type == DNS_QUERY_SINGLE_A_TYPE)
    {
        return "A";
    } else {
        return "AAAA";
    }
}

/*
 * Network Connectivity Types
 */
enum ConnectionState
{
    CONN_STATE_UNKNOWN,
    CONN_STATE_CONNECTED,
    CONN_STATE_DISCONNECTED
};


enum PhysicalLayerType
{
    PHYSLAYER_TYPE_MOBILE,
    PHYSLAYER_TYPE_GSM,
    PHYSLAYER_TYPE_CDMA,
    PHYSLAYER_TYPE_WIMAX,
    PHYSLAYER_TYPE_WIFI,
    PHYSLAYER_TYPE_WIRED
};

enum PhysicalLayerSubtype
{
    PHYSLAYER_SUBTYPE_UNKNOWN,
    PHYSLAYER_SUBTYPE_2G,
    PHYSLAYER_SUBTYPE_3G,
    PHYSLAYER_SUBTYPE_4G,
    PHYSLAYER_SUBTYPE_802_11A,
    PHYSLAYER_SUBTYPE_802_11B,
    PHYSLAYER_SUBTYPE_802_11G,
    PHYSLAYER_SUBTYPE_802_11N,
    PHYSLAYER_SUBTYPE_10MBPS_ETH,
    PHYSLAYER_SUBTYPE_100MBPS_ETH,
    PHYSLAYER_SUBTYPE_1000MBPS_ETH
};

inline std::string physicalLayerTypeToString(PhysicalLayerType t)
{
    switch(t)
    {
    case PHYSLAYER_TYPE_MOBILE:    return "mobile";
    case PHYSLAYER_TYPE_GSM:       return "gsm";
    case PHYSLAYER_TYPE_CDMA:      return "cdma";
    case PHYSLAYER_TYPE_WIMAX:     return "wimax";
    case PHYSLAYER_TYPE_WIFI:      return "wifi";
    case PHYSLAYER_TYPE_WIRED:     return "wired";
    default: return "";
    }
}

inline std::string physicalLayerSubtypeToString(PhysicalLayerSubtype t)
{
    switch(t)
    {
    case PHYSLAYER_SUBTYPE_2G:    return "2g";
    case PHYSLAYER_SUBTYPE_3G:       return "3g";
    case PHYSLAYER_SUBTYPE_4G:      return "4g";
    case PHYSLAYER_SUBTYPE_802_11A:     return "a";
    case PHYSLAYER_SUBTYPE_802_11B:      return "b";
    case PHYSLAYER_SUBTYPE_802_11G:     return "g";
    case PHYSLAYER_SUBTYPE_802_11N:     return "n";
    case PHYSLAYER_SUBTYPE_10MBPS_ETH:     return "10Mbps";
    case PHYSLAYER_SUBTYPE_100MBPS_ETH:     return "100Mbps";
    case PHYSLAYER_SUBTYPE_1000MBPS_ETH:     return "1000Mbps";
    default: return "";
    }
}

}

#endif

