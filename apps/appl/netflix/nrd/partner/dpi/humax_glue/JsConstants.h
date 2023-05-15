/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef JsConstants_h
#define JsConstants_h

// Defines to parse or populate data used to communicate with UI
#define PARTNERNAME             "HUMAX"

// Result types
#define CREDENTIALS             "pin"
#define SUCCESSEVENT            "onSuccess"
#define FAILUREEVENT            "onFailure"
#define COMPLETEEVENT           "onComplete"
#define ERRORSTR                "error"

// Payment data
#define DATASTR                 "data"

// Payment parnter data
#define NAMESTR                 "name"
#define PAYMENTDESCRIPTIONSHORT "paymentDescriptionShort"
#define PAYMENTDESCRIPTION      ""

// User data
#define PAYMENTS                "payments"

// PIN prompt type
#define NUMERIC4DIGITPIN        "numeric4DigitPin"
#define NOPIN                   "none"

// Errors
#define ERROR_REQUEST_AUTH      "requestAuth"
#define ERROR_NETWORK           "networkError"
#define ERROR_UNKNOWN_METHOD    "unknownMethod"
#define ERROR_UNKNOWN_SERVICE   "unknownService"
#define ERROR_INVALID_PIN       "invalidPIN"

#endif // JsConstants.h
