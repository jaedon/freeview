/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MDX_TARGET_H
#define MDX_TARGET_H

#include <upnp.h>

namespace netflix {
namespace mdx {

/**
 * @class MdxTarget MdxTarget.h
 */
class MdxTarget
{
public:

    static MdxTarget* getInstance(int advtPeriod,
                                  int mdxPort,
                                  int numSsdpReplies,
                                  std::string mdxUSN,
                                  std::string mdxFriendlyName,
                                  std::string mdxManufacturer,
                                  std::string mdxModelName );
    static void destroyInstance();

    /** Destructor. */
    virtual ~MdxTarget();

    //================================================================================
    // MdxTarget implementation.
    //================================================================================

    virtual int RevealPresence(const std::string &serviceType);
    virtual int SetDeviceReplyHeaders(const std::vector<std::string> &deviceReplyHeaders);
    virtual int HidePresence();
    virtual int MdxTargetSendAdvt();
    virtual int MdxTargetStopAdvt();

private:
    int advtPeriod_;
    int mdxPort_;
    int numSsdpReplies_;
    std::string mdxUSN_;
    std::string mdxFriendlyName_;
    std::string mdxManufacturer_;
    std::string mdxModelName_;

    /*! Device handle supplied by UPnP SDK. */
    UpnpDevice_Handle upnpDevicehandle_;
    Upnp_FunPtr upnpFuncPtr_;
    static MdxTarget* pInstance;

    /** Constructor. */
    MdxTarget(int advtPeriod,
              int mdxPort,
              int numSsdpReplies,
              const std::string &mdxUSN,
              const std::string &mdxFriendlyName,
              const std::string &mdxManufacturer,
              const std::string &mdxModelName );

    static int TargetCallbackEventHandler(Upnp_EventType EventType, const void *Event, void *Cookie);
};

}} // namespace netflix::mdx

#endif // MDX_TARGET_H
