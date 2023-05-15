/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SignupBridge.h"

#include <nrd/ISystem.h>

using namespace std;
using namespace netflix;

#define SsoPropertyList(Property)                   \
    Property(externalSignUpServices, NfObject::ReadOnly)

PropertyTable(SignupBridge, SsoPropertyList, properties);

#define MethodList(Method)                      \
    Method(getExternalUserData)                 \
    Method(requestExternalUserConfirmation)

MethodTable(SignupBridge, MethodList);

SignupBridge::SignupBridge()
    : NfObject("signup", Properties::SignupBridge::properties, Properties::SignupBridge::PropertyCount,
               Methods::SignupBridge::methods, Methods::SignupBridge::MethodCount)
{
}

SignupBridge::~SignupBridge()
{
}

bool SignupBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::SignupBridge::externalSignUpServices:
    {
        Variant services;
        services.push_back("Signup-Service-ALPHA123");
        services.push_back("Signup-Service-BETA+ _9");
        *result = services;
    }
    return true;
    default:
        return false;
    }
}

bool SignupBridge::invoke(int method, const Variant &var)
{
    bool ok;

    const std::string index = var.mapValue<std::string>("idx");

    switch (method) {
    case Methods::SignupBridge::getExternalUserData:
        NTRACE(TRACE_NBP,"Methods::SignupBridge::getExternalUserData '%s' '%s'\n",
               var.mapValue<string>("service", &ok).c_str(),
               var.mapValue<string>("uid", &ok).c_str());
        {
            Variant map;
            map["firstname"] = "Joe";
            map["lastname"] = "Smith";
            map["email"] = "joesmith@email.com";
            map["billingzip"] = "95032";
            map["payments"] = "A Partner Wallet, A third-party wallet, Another wallet";
            map["uid"] = var.mapValue<string>("uid", &ok);
            map["idx"] = index;
            sendEvent("onSuccess", map);
        }
        {
            Variant map;
            map["errorcode"] = "87654321";
            map["message"] = "Something bad happened";
            map["uid"] = var.mapValue<string>("uid", &ok);
            map["idx"] = index;
            sendEvent("onFailure", map);
        }
        {
            Variant map;
            map["idx"] = index;
            sendEvent("onComplete", map);
        }
        break;
    case Methods::SignupBridge::requestExternalUserConfirmation:
        NTRACE(TRACE_NBP,"Methods::SignupBridge::requestExternalUserConfirmation '%s' '%s'\n",
               var.mapValue<string>("service", &ok).c_str(),
               var.mapValue<string>("uid", &ok).c_str());
        {
            Variant map;
            map["confirmed"] = true;
            map["issuetime"] = 11112222;
            map["expiretime"] = 22223333;
            map["userid"] = "a permanent unique external user ID";
            map["signerid"] = "the unique ID of the signing public/private key pair";
            map["signature"] = "Base64-encoded SHA256-RSA signature computed over the token data";
            map["uid"] = var.mapValue<string>("uid", &ok);
            map["idx"] = index;
            sendEvent("onSuccess", map);
        }
        {
            Variant map;
            map["errorcode"] = "345773457";
            map["message"] = "More bad stuff";
            map["uid"] = var.mapValue<string>("uid", &ok);
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

