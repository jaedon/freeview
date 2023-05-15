/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __CONFIGURATION_KEYS_H__
#define __CONFIGURATION_KEYS_H__

#include <string>

// UserKeys should be specific to a user account
// SystemKeys should be system global

namespace netflix {

class SystemKeys
{
public:
    enum Key
    {

        // MediaControl keys
        streamDownloadSpeed,
        pipelineDetection,
        playbackFailureRecord,
        lkgNumConnections,


        // StorageBridge
        storageBridge,

        // AppTrustStore
        appTrustStore,

        // UI set values
        UIVersion,
        UICookies,
        UILanguages,
        registered,

        criticalErrors,
        nrdStarted,

        //Network configurations
        IpConnectivityParas,

        DiskStoreKey,

        DiskCacheKey,
        DiskCache,
        SurfaceCache,
        ScriptEngine,
        PrefetchUrls,
        InjectJS,

        crashCount,
        upTime
    };

    static const std::string prefix;
    static const char *toString(Key key);
};

}

#endif // __CONFIGURATION_KEYS_H__
