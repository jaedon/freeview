/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SignupBridge.h"
#include <nrdbase/Log.h>
#include "DeviceLibNative.h"
#include "CurlUtils.h"
#include "JsConstants.h"

using namespace netflix::device;
using namespace netflix;

#define SignupPropertyList(Property)                      \
    Property(externalPaymentServices, NfObject::ReadOnly) \
    Property(externalSignUpServices, NfObject::ReadOnly)  \
    Property(userConfirmationType, NfObject::ReadOnly)

PropertyTable(SignupBridge, SignupPropertyList, properties);

#define MethodList(Method)                      \
    Method(authorizeCredentials)                \
    Method(getPaymentData)                      \
    Method(getPaymentPartnerData)               \
    Method(getUserData)                         \
    Method(resetCredentials)

MethodTable(SignupBridge, MethodList);

SignupBridge::SignupBridge()
    : NfObject("signup", Properties::SignupBridge::properties, Methods::SignupBridge::methods),
      isAuthorized(false)
{
}

void SignupBridge::init()
{
    startEventListener();
}

void SignupBridge::event(ApplicationEvent::Type type, const Variant &/*data*/)
{
    if (type == ApplicationEvent::Event_UrlAboutToChange) {
        isAuthorized = false;
    }
}

SignupBridge::~SignupBridge()
{
}

Variant SignupBridge::invoke(int method, const Variant &data)
{
    NTRACE(TRACE_LOG, "SignupBridge::invoke - method id %d \n", method);
    Variant result;
    const std::string index = data.mapValue<std::string>("idx");

    // We need to ALWAYS set 'idx' for callbacks (onSuccess, onFailure, etc)
    // to get to the right caller
    result["idx"] = index;

    switch (method)
    {
        // Authorize credentials is used to "check" if the entered credentials
        // are valid.
        case Methods::SignupBridge::authorizeCredentials:
        {
            const char* pin = data.value(CREDENTIALS).string().c_str();

            if(!sConfiguration->pinCode.empty())
            {
                if (strcmp(pin, sConfiguration->pinCode.c_str()) == 0)
                {
                    isAuthorized = true;
                    sendEvent(SUCCESSEVENT, result);
                }
                else
                {
                    isAuthorized = false;
                    result[ERRORSTR] = ERROR_INVALID_PIN;
                    sendEvent(FAILUREEVENT, result);
                }
            }
            else
            {
                isAuthorized = true;
                sendEvent(SUCCESSEVENT, result);
            }

            sendEvent(COMPLETEEVENT, result);
            break;
        }

        // Return the name and any other details about the partner here.
        case Methods::SignupBridge::getPaymentPartnerData:
        {
            result[NAMESTR] = PARTNERNAME;
            result[PAYMENTDESCRIPTIONSHORT] = PAYMENTDESCRIPTION;
            sendEvent(SUCCESSEVENT, result);
            sendEvent(COMPLETEEVENT, result);
            break;
        }

        // Return user related data like name, email, etc here.
        case Methods::SignupBridge::getUserData:
        {
            result["firstName"] = "Joe";
            result["lastName"] = "Smith";

            // Create a random email, makes it easier for testing
            time_t rawtime;
            struct tm * timeinfo;
            char buffer[80];
            time (&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(buffer,80,"%d%m%Y%I%M%S",timeinfo);
            std::string str(buffer);
            std::string email = "tester_mvpd_" + str + "@netflix.com";

            result["email"] = email;
            result["billingZip"] = "95032";

            // Add supported payment types for this user.
            Variant payments;
            payments.push_back(PARTNERNAME);
            result[PAYMENTS] = payments;

            sendEvent(SUCCESSEVENT, result);
            sendEvent(COMPLETEEVENT, result);
            break;
        }

        // Return payment realted information here.  This will bring up a PIN prompt if the user
        // is not authorized yet.
        case Methods::SignupBridge::getPaymentData:
        {
            bool success = false;
            NTRACE(TRACE_LOG, "getPaymentData - isAuthorized(%s)", isAuthorized?"TRUE":"FALSE");
            if (isAuthorized||sConfiguration->pinCode.empty())
            {
                CurlUtils c;
                success = c.init();
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
                    // send a network
                    result[ERRORSTR] = ERROR_NETWORK;
                    sendEvent(FAILUREEVENT, result);
                }
            }
            else
            {
                // send an unauthorized error
                result[ERRORSTR] = ERROR_REQUEST_AUTH;
                sendEvent(FAILUREEVENT, result);
            }

            sendEvent(COMPLETEEVENT, result);
            break;
        }

        // Called from PartnerBridge.js when it is reloaded.
        case Methods::SignupBridge::resetCredentials:
        {
            isAuthorized = false;
            break;
        }

        default:
        {
            result[ERRORSTR] = ERROR_UNKNOWN_METHOD;
            sendEvent(FAILUREEVENT, result);
            sendEvent(COMPLETEEVENT, result);
            return false;
        }
    }
    return true;
}

bool SignupBridge::getProperty(int index, Variant *result) const
{
    NTRACE(TRACE_LOG, "SignupBridge::getProperty - property id %d \n", index);
    switch (index)
    {
        // If the partner wants Netflix to prompt, specify the type of prompt required
        case Properties::SignupBridge::userConfirmationType:
        {
            (*result) = sConfiguration->pinCode.empty()?
                NOPIN:NUMERIC4DIGITPIN;  // This means UI is responsible to bring up PIN prompt
                                                    // and the type of the prompt is 4 digit numeric
            break;
        }

        // Provide a list of signup services available.
        case Properties::SignupBridge::externalSignUpServices:
        {
            (*result).push_back(PARTNERNAME);  // Add more if partner supports more payment methods
            break;
        }

        // Provide a list of supported payment methods.. This can be partner name,
        // paypal, or any other payment services supported
        case Properties::SignupBridge::externalPaymentServices:
        {
            (*result).push_back(PARTNERNAME);
            break;
        }
        default:
        {
            return false;
        }
    }
    return true;
}
