/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NET_ERROR_H__
#define __NET_ERROR_H__

#include  <curl/curl.h>
#include <nrdbase/AseErrorCodes.h>

namespace netflix {

namespace NetErrorGroup
{
enum Group {
#define DEFINE_ERROR_GROUP(group) group,
#include <nrdnet/NetErrorInternal.h>
    Count
};
#undef DEFINE_ERROR_GROUP
}

namespace NetErrorCode
{
enum Code {
#define DEFINE_ERROR_CODE(code) code,
#include <nrdnet/NetErrorInternal.h>
    Count
};
#undef DEFINE_ERROR_CODE
}

namespace NetError
{
enum RecommendedAction {
    Retry,
    Restart,
    Reactivate,
    ModifyDns,
    RedirectSettings,
    Reinstall,
    RunDiagTool,
    LogError
};

typedef NetErrorCode::Code ErrorCode;
typedef NetErrorGroup::Group ErrorGroup;

NetErrorGroup::Group mapErrorGroup(NetErrorCode::Code ec);
RecommendedAction mapRecommendedAction(NetErrorCode::Code ec, NetErrorGroup::Group eg);
NetErrorCode::Code nativeErrorCodeToNetError(AseErrorCode reason);
inline const char *errorGroupToString(NetErrorGroup::Group group)
{
#define DEFINE_ERROR_GROUP(group) #group ,
    const char *names[] = {
#include <nrdnet/NetErrorInternal.h>
        0
    };
#undef DEFINE_ERROR_GROUP
    return names[group];
}

inline const char *errorCodeToString(NetErrorCode::Code code)
{
#define DEFINE_ERROR_CODE(code) #code ,
    const char *names[] = {
#include <nrdnet/NetErrorInternal.h>
        0
    };
#undef DEFINE_ERROR_CODE
    return names[code];
}
}

}

#endif // __NET_ERROR_H__
