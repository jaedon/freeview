/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SSOBridge.h"
#include <nrdbase/Log.h>
#include "DeviceLibNative.h"
#include "CurlUtils.h"
#include "JsConstants.h"

using namespace netflix::device;
using namespace netflix;

#define SsoPropertyList(Property)                   \
    Property(externalSsoServices, NfObject::ReadOnly)

PropertyTable(SSOBridge, SsoPropertyList, properties);

#define MethodList(Method)                      \
    Method(getExternalUserToken)

MethodTable(SSOBridge, MethodList);

SSOBridge::SSOBridge()
    : NfObject("sso", Properties::SSOBridge::properties, Methods::SSOBridge::methods)
{
}

SSOBridge::~SSOBridge()
{
}

Variant SSOBridge::invoke(int method, const Variant &data)
{
    NTRACE(TRACE_LOG, "SSOBridge::invoke - method id %d \n", method);
    const std::string index = data.mapValue<std::string>("idx");
    Variant result;
    result["idx"] = index;

    switch (method)
    {
        case Methods::SSOBridge::getExternalUserToken:
        {
            CurlUtils c;
            bool success = c.init();
            if(success)
            {
                success = c.makePostRequest( sConfiguration->tokenUrl,
                        c.constructHeaderList(sConfiguration->headerList), "" );
                if(success)
                {
                    result[DATASTR] = c.getBody().c_str();
                    sendEvent(SUCCESSEVENT, result);
                }
            }
            if(!success)
            {
                result[ERRORSTR] = ERROR_NETWORK;
                sendEvent(FAILUREEVENT, result);
            }
            break;
        }
        default:
        {
            result[ERRORSTR] = ERROR_UNKNOWN_METHOD;
            sendEvent(FAILUREEVENT, result);
            return false;
        }
    }
    return true;
}

bool SSOBridge::getProperty(int index, Variant *result) const
{
    NTRACE(TRACE_LOG, "SSOBridge::getProperty - property id %d \n", index);
    switch (index)
    {
        case Properties::SSOBridge::externalSsoServices:
        {
            Variant service;
            service.push_back(PARTNERNAME);
            *result = service;
            break;
        }
        default:
        {
            return false;
        }
    }
    return true;
}

