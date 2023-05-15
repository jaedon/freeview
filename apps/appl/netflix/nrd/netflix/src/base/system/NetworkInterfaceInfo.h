/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef NETWORK_INTERFACE_INFO_H
#define NETWORK_INTERFACE_INFO_H

#include <string>

namespace netflix {

/**
 * @class NetworkInterfaceInfo NetworkInterfaceInfo
 * @brief A data structure containing information about a network interface.
 */
struct NetworkInterfaceInfo
{
    void reset()
    {
        defaultInterfaceName_.clear();
        carrier_.clear();
        countryCode_.clear();
        networkCode_.clear();
        networkType_.clear();
        networkSpec_.clear();
        ssid_.clear();
        wirelessChannel_.clear();
        ipVersionsAvailable_.clear();
        ipVersionPreferred_.clear();
        ipVersionPolicy_.clear();
        additionalInfo_.clear();
    }

    std::string defaultInterfaceName_;
    std::string carrier_;
    std::string countryCode_;
    std::string networkCode_;
    std::string networkType_;
    std::string networkSpec_;
    std::string ssid_;
    std::string wirelessChannel_;
    std::string ipVersionsAvailable_;
    std::string ipVersionPreferred_;
    std::string ipVersionPolicy_;
    std::string additionalInfo_;
};

} // namespace netflix

#endif // NETWORK_INTERFACE_INFO_H
