/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MDX_CONTROLLER_H
#define MDX_CONTROLLER_H

#include "MdxUtil.h"

#include <upnp.h>
#include <UpnpString.h>
#include <upnptools.h>

namespace netflix {
namespace mdx {

/**
 * @class MdxController MdxController.h
 * @brief MDX Controller Internal Class.
 */
class MdxController
{
public:
    static MdxController* getInstance();
    static void destroyInstance();

    /** Destructor. */
    virtual ~MdxController();

    //================================================================================
    // MdxController implementation.
    //================================================================================
    static int MdxCtrlPointCallbackEventHandler(Upnp_EventType, const void *, void *);

    void AddService(const std::string serviceType);
    int  Start(const std::vector<std::string> &responseHeaders);
    int  Search(const std::string &serviceType, int mx, int numSsdpSearches = -1);
    int  Stop(void);

private:
    typedef std::vector<std::string> ServiceTypes;

    ServiceTypes serviceTypes_;
    static MdxController* pInstance;
    UpnpClient_Handle ctrlpt_handle;

private:
    /** Constructor. */
    MdxController();
};

}} // namespace netflix::mdx

#endif // MDX_CONTROLLER_H
