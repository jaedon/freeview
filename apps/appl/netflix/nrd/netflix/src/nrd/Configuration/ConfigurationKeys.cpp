/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ConfigurationKeys.h"

#include <assert.h>

using namespace netflix;

const std::string SystemKeys::prefix = "s";

//////////
//
// SystemKeys
//
//////////
const char *SystemKeys::toString(SystemKeys::Key key)
{
    switch (key) {
    case streamDownloadSpeed: return "MC/SDS";
    case pipelineDetection: return "MC/PDR";
    case playbackFailureRecord: return "MC/PFR";
    case lkgNumConnections: return "MC/LNC";

    case storageBridge: return "Storage";
    case appTrustStore: return "AppTS";

    case UIVersion: return "UI/ver";
    case UICookies: return "UI/cks";
    case UILanguages: return "UI/lang";
    case registered: return "reg";

    case criticalErrors: return "CriticalErrors";
    case nrdStarted: return "BOOT/nrdStarted";

    case IpConnectivityParas: return "NET/IpConnParas";

    case DiskStoreKey: return "DISKSTORE/key";

    case DiskCacheKey: return "Gibbon/DiskCacheKey";
    case DiskCache: return "Gibbon/DiskCache";
    case SurfaceCache: return "Gibbon/SurfaceCache";

    case ScriptEngine: return "Gibbon/ScriptEngine";
    case PrefetchUrls: return "Gibbon/PrefetchUrls";
    case InjectJS: return "Gibbon/InjectJS";

    case crashCount: return "BOOT/crashCount";
    case upTime: return "BOOT/upTime";
    }

    // should never get here
    assert(0);
    return 0;
}
