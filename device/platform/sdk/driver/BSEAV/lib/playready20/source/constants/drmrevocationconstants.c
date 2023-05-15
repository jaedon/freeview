/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmrevocationconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

const                DRM_CHAR       g_rgchTagRevocation                          [] = "REVOCATION";
const                DRM_CHAR       g_rgchAppRevocation                          [] = "AppRevocationList"; /* ODD count */
const                DRM_CHAR       g_rgchTagRevocationInfo                      [] = "REVOCATIONINFO";
const                DRM_CHAR       g_rgchDeviceRevocation                       [] = "DeviceRevocationList";
const                DRM_CHAR       g_rgchWMDRMNET_Revocation                    [] = "WMDRMNET";

/* {CD75E604-543D-4A9C-9F09-FE6D24E8BF90} */
const                DRM_CHAR       g_rgchRevocationGuidWMDRMNET                 [] = "{CD75E604-543D-4A9C-9F09-FE6D24E8BF90}, ";
/* {3129E375-CEB0-47D5-9CCA-9DB74CFD4332} */
const                DRM_CHAR       g_rgchRevocationGuidDevice                   [] = "{3129E375-CEB0-47D5-9CCA-9DB74CFD4332}, ";
/* {90A37313-0ECF-4CAA-A906-B188F6129300} */
const                DRM_CHAR       g_rgchRevocationGuidApp                      [] = "{90A37313-0ECF-4CAA-A906-B188F6129300}, ";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagRevocation                        = CREATE_DRM_ANSI_STRING_EX( g_rgchTagRevocation, 10 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrAppRevocation                        = CREATE_DRM_ANSI_STRING_EX( g_rgchAppRevocation, 17 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagRevocationInfo                    = CREATE_DRM_ANSI_STRING_EX( g_rgchTagRevocationInfo, 14 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDeviceRevocation                     = CREATE_DRM_ANSI_STRING_EX( g_rgchDeviceRevocation, 20 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrWMDRMNET_Revocation                  = CREATE_DRM_ANSI_STRING_EX( g_rgchWMDRMNET_Revocation, 8);

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrRevocationGuidWMDRMNET               = CREATE_DRM_ANSI_STRING_EX( g_rgchRevocationGuidWMDRMNET, 38 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrRevocationGuidDevice                 = CREATE_DRM_ANSI_STRING_EX( g_rgchRevocationGuidDevice  , 38 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrRevocationGuidApp                    = CREATE_DRM_ANSI_STRING_EX( g_rgchRevocationGuidApp     , 38 );

const DRM_DISCARDABLE DRM_LID g_lidRevocationInfo =
{
    'R', 'E', 'V', 'O', 'C', 'A',
    'T', 'I', 'O', 'N', 'I', 'N',
    'F', 'O', '\0', '\0'
};

const DRM_DISCARDABLE DRM_LID g_lidRevocationInfo2 =
{
    'R', 'E', 'V', 'O', 'C', 'A',
    'T', 'I', 'O', 'N', 'I', 'N',
    'F', 'O', '2', '\0'
};

/* DeviceRevocation */
const DRM_DISCARDABLE DRM_LID g_lidDeviceRevocation =

{
    'D', 'e', 'v', 'i', 'c', 'e',
    'R', 'e', 'v', 'o', 'c', 'a',
    't', 'i', 'o', 'n'
};

/* WMDRMNET_RVK */
const DRM_DISCARDABLE DRM_LID g_lidWMDRMNET_Revocation =

{
    'W', 'M', 'D', 'R', 'M', 'N',
    'E', 'T', '_', 'R', 'V', 'K',
    '\0', '\0', '\0', '\0'
};

/* RevocationList */
const DRM_DISCARDABLE DRM_LID g_lidAppRevocation =

{
    'R', 'e', 'v', 'o', 'c', 'a',
    't', 'i', 'o', 'n', 'L', 'i',
    's', 't', '\0', '\0'
};

/* PRRTRevocList */
const DRM_DISCARDABLE DRM_LID g_lidPRRTRevocation =

{
    'P', 'R', 'R', 'T', 'R', 'e',
    'v', 'o', 'c', 'L', 'i', 's',
    't', '\0', '\0', '\0'
};

/* PRAppRevocList */
const DRM_DISCARDABLE DRM_LID g_lidPRAppRevocation =

{
    'P', 'R', 'A', 'p', 'p', 'R',
    'e', 'v', 'o', 'c', 'L', 'i',
    's', 't', '\0', '\0'
};

EXIT_PK_NAMESPACE_CODE;

