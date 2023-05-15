/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "Mdx.h"
#include <upnp.h>
#include "MdxController.h"
#include "MdxTracing.h"
#include "MdxUtil.h"
#include "MdxInternal.h"
#include <nrdbase/Base64.h>
#include <nrdbase/UrlEncoder.h>

using namespace std;
using namespace netflix;
using namespace netflix::mdx;

MdxController* MdxController::pInstance = 0;

MdxController::MdxController()
    : ctrlpt_handle(-1)
{
}

MdxController * MdxController::getInstance()
{
    if(pInstance == 0){
        pInstance = new MdxController();
    }
    return pInstance;
}

void MdxController::destroyInstance()
{
    delete pInstance;
    pInstance = 0;
}

MdxController::~MdxController()
{

}

/********************************************************************************
 * MdxCtrlPointCallbackEventHandler
 *
 * Description:
 *       The callback handler registered with the SDK while registering
 *       the control point.  Detects the type of callback, and passes the
 *       request on to the appropriate function.
 *
 * Parameters:
 *   EventType -- The type of callback event
 *   Event -- Data structure containing event data
 *   Cookie -- Optional data specified during callback registration
 *
 ********************************************************************************/
int MdxController::MdxCtrlPointCallbackEventHandler(Upnp_EventType EventType, const void *Event, void */*Cookie*/)
{
    if(UPNP_DISCOVERY_SEARCH_TIMEOUT == EventType || !Event)
    {
        return 0;
    }

    struct Upnp_Discovery *d_event = (struct Upnp_Discovery *)Event;

    if (d_event->ErrCode != UPNP_E_SUCCESS) {
        MDXERROR("Error in Discovery Callback -- %d",    d_event->ErrCode);
        return 0;
    }

    string deviceId = d_event->DeviceId;
    string serviceType = d_event->ServiceType;

    const MdxController::ServiceTypes &serviceTypes = MdxController::getInstance()->serviceTypes_;
    size_t found = string::npos;

    for(ServiceTypes::const_iterator i = serviceTypes.begin(); i != serviceTypes.end(); ++i)
    {
        found = serviceType.find(*i);
        if(found != string::npos) {
            break;
        }
    }

    if (found == string::npos) {
        // not a service type we are interested in
        return 0;
    }


    switch ( EventType ) {
    case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
    case UPNP_DISCOVERY_SEARCH_RESULT: {
        string location = d_event->Location;
        string friendlyName = d_event->FriendlyName;
        IMdx::Listener::ResponseHeaders headers;

#ifdef MDXLIB_HAVE_TRACING
        Variant args;
        args["location"]     = location;
        args["serviceType"]  = serviceType;
        args["USN"]          = deviceId;
        args["friendlyName"] = Base64::decode(friendlyName);
        EVENT("UPNP_DISCOVERY", "UPNP", &args);
#endif

        MDXTRACE("Received %s\n"
                 "Loc: %s\n"
                 "ServiceType: %s\n"
                 "USN: %s\n"
                 "FriendlyName: %s",
                 EventType == UPNP_DISCOVERY_SEARCH_RESULT ? "Reply to Search" : "Advertisement",
                 location.c_str(), serviceType.c_str(), deviceId.c_str(), friendlyName.c_str());

        for(int j = 0; j < MATCHED_HEADER_MAX; j++)
        {
            if(strlen(d_event->MatchedHeaders[j][0]))
            {
                headers.push_back(std::pair<std::string, std::string>(d_event->MatchedHeaders[j][0], d_event->MatchedHeaders[j][1]));
            }
        }
        MdxInternal::getInstance()->callListener(DeviceFoundEvent(location,
                                                                  serviceType,
                                                                  deviceId,
                                                                  friendlyName,
                                                                  headers,
                                                                  false,
                                                                  false));
        break;
    }
    case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE: {
#ifdef MDXLIB_HAVE_TRACING
        Variant args;
        args["serviceType"]  = serviceType;
        args["USN"]          = deviceId;
        EVENT("UPNP_BYEBYE", "UPNP", &args);
#endif

        MDXTRACE("Received ByeBye\n"
                 "ServiceType: %s\n"
                 "USN: %s\n",
                 serviceType.c_str(),
                 deviceId.c_str());
        MdxInternal::getInstance()->callListener(DeviceByeByeEvent(serviceType, deviceId));
        break;
    }
    default:
        MDXWARN("MdxCtrlPointCallbackEventHandler: unknown event type %d",  EventType);
        break;
    }

    return 0;
}

/*!
 * \brief Call this function to initialize the UPnP library and start the TV
 * Control Point.  This function provides a callback handler to process any
 * UPnP events that are received.
 *
 * \return MDX_SUCCESS if everything went well, else MDX_ERROR.
 */
int MdxController::Start(const vector<string> &responseHeaders)
{
    int rc;
#if defined(BUILD_DEBUG)
    int advtPort;
    const char *ip_address = 0;

    ip_address = UpnpGetServerIpAddress();
    advtPort = UpnpGetSSDPPort();

    MDXTRACE("Controller: UPnP was initialized\n"
             "\tipaddress = %s ssdp port = %d",
             ip_address ? ip_address : "{NULL}", advtPort);
#endif

    MDXTRACE("Registering Control Point");

    vector<const char *>responseHeadersp;
    for(vector<string>::const_iterator i = responseHeaders.begin(); i != responseHeaders.end(); ++i) {
        responseHeadersp.push_back(i->c_str());
    }

    rc = UpnpRegisterClient2(MdxCtrlPointCallbackEventHandler,
                             this,
                             &responseHeadersp[0],
                             responseHeadersp.size(),
                             &ctrlpt_handle);
    if (rc != UPNP_E_SUCCESS) {
        MDXERROR("Error registering Control Point: %d %s", rc, UpnpGetErrorMessage(rc));

        return MDX_ERROR;
    }

    MDXTRACE("Control Point Registered");

    return MDX_SUCCESS;
}

void MdxController::AddService(const std::string serviceType )
{
    if(!serviceType.length())
    {
        return;
    }

    string serviceTypeNoVersion;

    // do not include the version number at the end, because we want to match
    // advertisements and replies regardless of version number
    if(serviceType.length() > 2)
    {
        serviceTypeNoVersion = serviceType.substr(0, serviceType.length() - 2);
    }
    else
    {
        serviceTypeNoVersion = serviceType;
    }

    ServiceTypes::const_iterator i;
    for(i = serviceTypes_.begin(); i != serviceTypes_.end(); ++i) {
        if(*i == serviceTypeNoVersion) {
            // already added
            return;
        }
    }

    serviceTypes_.push_back(serviceTypeNoVersion);
}

/********************************************************************************
 * Search
 *
 * Description:
 *     Issue an SSDP search for the given service type
 *
 * Parameters:
 *   serviceType - the service type to search for
 *   mx - the mx value (seconds to delay the response)
 *
 ********************************************************************************/
int MdxController::Search(const std::string &serviceType, int mx, int numSsdpSearches)
{
    MDXTRACE("UpnpSearchAsync for: %s", serviceType.c_str());

    int rc;

    // Add this service type (if necessary) to the service types whose events we
    // report back to the event listener(s)
    AddService(serviceType);

    rc = UpnpSearchAsync(ctrlpt_handle, mx, numSsdpSearches, serviceType.c_str(), 0);
    if (UPNP_E_SUCCESS != rc) {
        MDXERROR("Error sending search request %d %s", rc, UpnpGetErrorMessage(rc));

        return MDX_ERROR;
    }

    return MDX_SUCCESS;
}

int MdxController::Stop(void)
{
    UpnpUnRegisterClient( ctrlpt_handle );

    return MDX_SUCCESS;
}



