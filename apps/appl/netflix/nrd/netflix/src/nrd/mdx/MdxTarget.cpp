/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxLog.h"
#include "MdxTarget.h"
#include "MdxTracing.h"
#include "MdxUtil.h"
#include <nrdbase/Base64.h>
#include <nrdbase/Version.h>

using namespace std;
using namespace netflix;
using namespace netflix::mdx;

MdxTarget* MdxTarget::pInstance = 0;

MdxTarget::MdxTarget(int advtPeriod,
                     int mdxPort,
                     int numSsdpReplies,
                     const string &mdxUSN,
                     const string &mdxFriendlyName,
                     const string &mdxManufacturer,
                     const string &mdxModelName )
    : advtPeriod_(advtPeriod)
    , mdxPort_(mdxPort)
    , numSsdpReplies_(numSsdpReplies)
    , mdxUSN_(mdxUSN)
    , mdxFriendlyName_(mdxFriendlyName)
    , mdxManufacturer_(mdxManufacturer)
    , mdxModelName_(mdxModelName)
    , upnpDevicehandle_(-1)
{

}

MdxTarget * MdxTarget::getInstance(int advtPeriod,
                                   int mdxPort,
                                   int numSsdpReplies,
                                   string mdxUSN,
                                   string mdxFriendlyName,
                                   string mdxManufacturer,
                                   string mdxModelName)
{
    if(pInstance == 0){
        pInstance = new MdxTarget(advtPeriod,
                                  mdxPort,
                                  numSsdpReplies,
                                  mdxUSN,
                                  mdxFriendlyName,
                                  mdxManufacturer,
                                  mdxModelName);
    }
    return pInstance;
}

void MdxTarget::destroyInstance()
{
    delete pInstance;
    pInstance = 0;
}

static std::string createDeviceDescXml(
        const std::string &USN,
        const std::string &friendlyName,
        const std::string &manufacturer,
        const std::string &modelName,
        const std::string &serviceType)
{
    std::string rawXml =
        "<?xml version=\"1.0\"?>"
        "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
        "<specVersion>"
        "<major>1</major>"
        "<minor>0</minor>"
        "</specVersion>"
        "<device>"
        "<deviceType>urn:schemas-upnp-org:device:mdxdevice:1</deviceType>"
        "<friendlyName>" + friendlyName + "</friendlyName>"
        "<manufacturer>"+manufacturer+"</manufacturer>"
        "<modelName>"+modelName+"</modelName>"
        "<UDN>" + USN + "</UDN>"
        "<UPC>123456789</UPC>"
        "<serviceList>"
        "<service>"
        "<serviceType>" + serviceType + "</serviceType>"
        "<serviceId>urn:upnp-org:serviceId:mdxcontrol1</serviceId>"
        "</service>"
        "</serviceList>"
        "<presentationURL>/tvdevicepres.html</presentationURL>"
        "</device>"
        "</root>";

    return rawXml;
}

int MdxTarget::RevealPresence(const std::string &serviceType)
{
    int ret = UPNP_E_SUCCESS;

    upnpFuncPtr_ =  &TargetCallbackEventHandler;

    const char * mdxIpAddress = UpnpGetServerIpAddress();
    unsigned short advtPort = UpnpGetSSDPPort();

    MDXTRACE("Target: UPnP was Initialized\n"
             "\tipaddress = %s ssdp port = %u mdx port= %u\n",
             mdxIpAddress ? mdxIpAddress : "{NULL}", advtPort, mdxPort_);
    NRDP_UNUSED(advtPort);

    std::string mdxFriendlyNameSafe = Base64::encode(mdxFriendlyName_);

    std::string deviceDesc = createDeviceDescXml(mdxUSN_, mdxFriendlyNameSafe,
            mdxManufacturer_, mdxModelName_, serviceType);

    MDXTRACE( "Registering the RootDevice "
              "from buffer\n%s\n", deviceDesc.c_str());

    char location[128];
    snprintf(location, sizeof(location), "http://%s:%d", mdxIpAddress, mdxPort_);

    ret = UpnpRegisterRootDevice(location,
                                 deviceDesc.c_str(),
                                 deviceDesc.length(),
                                 1,
                                 numSsdpReplies_,
                                 upnpFuncPtr_,
                                 &upnpDevicehandle_,
                                 &upnpDevicehandle_);

    if (ret != UPNP_E_SUCCESS)
    {
        MDXERROR("Error registering the rootdevice : %d %s", ret, UpnpGetErrorMessage(ret));

        return MDX_ERROR;
    }

    return MDX_SUCCESS;
}

int MdxTarget::SetDeviceReplyHeaders(const std::vector<std::string> &deviceReplyHeaders)
{
    int ret = UPNP_E_SUCCESS;

    vector<const char *> deviceReplyHeadersp;
    for(vector<string>::const_iterator i = deviceReplyHeaders.begin(); i != deviceReplyHeaders.end(); ++i) {
        deviceReplyHeadersp.push_back(i->c_str());
    }

    ret = UpnpSetDeviceReplyHeaders(upnpDevicehandle_, &deviceReplyHeadersp[0], deviceReplyHeadersp.size());

    if (ret != UPNP_E_SUCCESS)
    {
        MDXERROR("Could not set device reply headers : %d %s", ret, UpnpGetErrorMessage(ret));

        return MDX_ERROR;
    }

    return MDX_SUCCESS;
}

int MdxTarget::HidePresence()
{
    UpnpUnRegisterRootDevice(upnpDevicehandle_);
    return MDX_SUCCESS;
}

int MdxTarget::MdxTargetSendAdvt()
{
    MDXTRACE("RootDevice Registered");

    int ret = UpnpSendAdvertisement(upnpDevicehandle_, advtPeriod_);

    if (ret != UPNP_E_SUCCESS)
    {
        MDXERROR("Error sending advertisements : %d %s", ret, UpnpGetErrorMessage(ret));

        return MDX_ERROR;
    }
    MDXTRACE("Advertisements Sent");
    return MDX_SUCCESS;
}

int MdxTarget::MdxTargetStopAdvt()
{
    return MDX_SUCCESS;
}


MdxTarget::~MdxTarget()
{
    MdxTargetStopAdvt();
}


int MdxTarget::TargetCallbackEventHandler(Upnp_EventType EventType, const void *Event, void *Cookie)
{
    (void)Event;
    (void)Cookie;
    switch (EventType) {
    case UPNP_TRACE_EVENT:
        EVENT((const char *)Event, "UPNP", NULL);
        break;

        /* ignore these cases, since this is not a control point */
    case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
    case UPNP_DISCOVERY_SEARCH_RESULT:
    case UPNP_DISCOVERY_SEARCH_TIMEOUT:
    case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
        break;
    default:
        MDXWARN("TargetCallbackEventHandler: unknown event type %d",  EventType);
        break;
    }
    /* Print a summary of the event received */
    //    MDXTRACE(EventType, Event);

    return 0;
}
