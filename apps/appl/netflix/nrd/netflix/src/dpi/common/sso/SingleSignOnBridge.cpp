/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SingleSignOnBridge.h"

#include <nrd/ISystem.h>

using namespace std;
using namespace netflix;

#define SsoPropertyList(Property)                   \
    Property(externalSsoServices, NfObject::ReadOnly)


PropertyTable(SingleSignOnBridge, SsoPropertyList, properties);

#define MethodList(Method)                      \
    Method(getExternalUserToken)                \
    Method(requestExternalUserAuth)

MethodTable(SingleSignOnBridge, MethodList);

SingleSignOnBridge::SingleSignOnBridge()
    : NfObject("singlesignon",
               Properties::SingleSignOnBridge::properties,
               Properties::SingleSignOnBridge::PropertyCount,
               Methods::SingleSignOnBridge::methods,
               Methods::SingleSignOnBridge::MethodCount)
{
}

SingleSignOnBridge::~SingleSignOnBridge()
{
}

bool SingleSignOnBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::SingleSignOnBridge::externalSsoServices:
    {
        Variant services;
        services.push_back("SSO-Service-A");
        services.push_back("SSO-Service-B");
        *result = services;
    }
    return true;
    default:
        return false;
    }
}

bool SingleSignOnBridge::invoke(int method, const Variant &var)
{
    bool ok;

    const std::string index = var.mapValue<std::string>("idx");

    switch (method) {
    case Methods::SingleSignOnBridge::getExternalUserToken:
        NTRACE(TRACE_NBP,"Methods::SingleSignOnBridge::getExternalUserToken '%s'\n", var.mapValue<string>("service", &ok).c_str());
        {
            Variant map;
            map["issuetime"] = 87654321;
            map["expiretime"] = 12345678;
            map["userid"] = "a userid";
            map["signerid"] = "a signerid";
            map["signature"] = "a signature";
            map["idx"] = index;
            sendEvent("onSuccess", map);
        }
        {
            Variant map;
            map["errorcode"] = "87654321";
            map["message"] = "Something bad happened";
            map["idx"] = index;
            sendEvent("onFailure", map);
        }
        {
            Variant map;
            map["idx"] = index;
            sendEvent("noUser", map);
            sendEvent("onComplete", map);
        }
        break;
    case Methods::SingleSignOnBridge::requestExternalUserAuth:
        NTRACE(TRACE_NBP,"Methods::SingleSignOnBridge::requestExternalUserAuth '%s'\n", var.mapValue<string>("service", &ok).c_str());
        {
            Variant map;
            map["issuetime"] = 11112222;
            map["expiretime"] = 22223333;
            map["userid"] = "userid 2";
            map["signerid"] = "signerid 2";
            map["signature"] = "signature 2";
            map["idx"] = index;
            sendEvent("onSuccess", map);
        }
        {
            Variant map;
            map["errorcode"] = "345773457";
            map["message"] = "More bad stuff";
            map["idx"] = index;
            sendEvent("onFailure", map);
        }
        {
            Variant map;
            map["idx"] = index;
            sendEvent("onComplete", map);
        }
        break;
    default:
        return false;
    }
    return true;
}

