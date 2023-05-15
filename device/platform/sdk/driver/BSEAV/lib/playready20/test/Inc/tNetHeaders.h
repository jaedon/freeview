/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __tNETHEADERS_H
#define __tNETHEADERS_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#define HTTP_HEADER_JOIN            "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/JoinDomain\"\r\n"
#define HTTP_HEADER_LEAVE           "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/LeaveDomain\"\r\n"
#define HTTP_HEADER_LICGET          "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/AcquireLicense\"\r\n"
#define HTTP_HEADER_LICACK          "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/AcknowledgeLicense\"\r\n"
#define HTTP_HEADER_ACTIVATIONPOST  "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/PlayReady/ActivationService/v1/Activate\"\r\n"
#define HTTP_HEADER_LEGACY          "Content-Type: application/x-www-form-urlencoded\r\n\r\n"
#define HTTP_HEADER_METERCERT       "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/GetMeteringCertificate\"\r\n"
#define HTTP_HEADER_METERDATA       "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/ProcessMeteringData\"\r\n"

#define LEGACY_CHALLENGE_FORMAT "challenge="
#define LEGACY_METERCHALLENGE_FORMAT "meterchallenge="
#define LEGACY_SERVERMETERCHALLENGE_FORMAT "meterverification="
#define LEGACY_METERCERTCHALLENGE_FORMAT "getmetercert=1&mid="

#define MAX_HTTP_HEADER_SIZE 4096

#define AGENT_NAME "MSPR_PK_Test"
#define MAX_URL_SIZE         1024

typedef enum __tageDRM_TEST_NET_ACTION {
    eDRM_TEST_NET_UNKNOWN               = 0,
    eDRM_TEST_NET_LEGACY                = 1,
    eDRM_TEST_NET_LICGET                = 2,
    eDRM_TEST_NET_LICACK                = 4,
    eDRM_TEST_NET_DOMLEAVE              = 8,
    eDRM_TEST_NET_DOMJOIN               = 16,
    eDRM_TEST_NET_DOWNLOAD              = 32,
    eDRM_TEST_NET_METERCERT             = 64,
    eDRM_TEST_NET_METERDATA             = 128,
    eDRM_TEST_NET_SERVERMETERING        = 256,
    eDRM_TEST_NET_ACTIVATIONGET         = 512,
    eDRM_TEST_NET_ACTIVATIONPOST        = 1024,
    eDRM_TEST_NET_LICGET_LEGACY         = eDRM_TEST_NET_LICGET         | eDRM_TEST_NET_LEGACY,
    eDRM_TEST_NET_METERCERT_LEGACY      = eDRM_TEST_NET_METERCERT      | eDRM_TEST_NET_LEGACY,
    eDRM_TEST_NET_METERDATA_LEGACY      = eDRM_TEST_NET_METERDATA      | eDRM_TEST_NET_LEGACY,
    eDRM_TEST_NET_SERVERMETERING_LEGACY = eDRM_TEST_NET_SERVERMETERING | eDRM_TEST_NET_LEGACY
} eDRM_TEST_NET_ACTION;


EXIT_PKTEST_NAMESPACE

#endif /* __tNETHEADERS_H */
