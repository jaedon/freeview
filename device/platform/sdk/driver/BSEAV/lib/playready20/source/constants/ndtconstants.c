/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <ndtconstants.h>

ENTER_PK_NAMESPACE_CODE;

/* Note: when updating a string, don't forget to update the comment and length
 * of the string also.
 */

/* "MTP" */
const DRM_CHAR  g_rgchRegistrationAddress       [] = "MTP";
#define g_rgchRegistrationAddress_LEN 3

/* "c:WMDRMReceiver" */
const DRM_CHAR  g_rgchTagWMDRMReceiver          [] = "c:WMDRMReceiver";
#define g_rgchTagWMDRMReceiver_LEN 15

/* "c:WMDRMTransmitter" */
const DRM_CHAR  g_rgchTagWMDRMTransmitter       [] = "c:WMDRMTransmitter";
#define g_rgchTagWMDRMTransmitter_LEN 18

/* "c:WMDRMSharedCertificate" */
const DRM_CHAR  g_rgchTagWMDRMSharedCertificate [] = "c:WMDRMSharedCertificate";
#define g_rgchTagWMDRMSharedCertificate_LEN 24

/* "Play" */
const DRM_CHAR  g_rgchDRM_RIGHT_PLAYBACK        [] = "Play";
#define g_rgchDRM_RIGHT_PLAYBACK_LEN 4

const DRM_ANSI_CONST_STRING  g_dastrTagWMDRMSharedCertificate           = CREATE_DRM_ANSI_STRING_EX( g_rgchTagWMDRMSharedCertificate, g_rgchTagWMDRMSharedCertificate_LEN );
const DRM_ANSI_CONST_STRING  g_dastrTagWMDRMTransmitter                 = CREATE_DRM_ANSI_STRING_EX( g_rgchTagWMDRMTransmitter, g_rgchTagWMDRMTransmitter_LEN );
const DRM_ANSI_CONST_STRING  g_dastrTagWMDRMReceiver                    = CREATE_DRM_ANSI_STRING_EX( g_rgchTagWMDRMReceiver, g_rgchTagWMDRMReceiver_LEN );

const DRM_ID g_idNULLSessionID = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const DRM_ID g_idReservedSession = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
const DRM_ANSI_CONST_STRING  g_dastrDRM_RIGHT_PLAYBACK                  = CREATE_DRM_ANSI_STRING_EX( g_rgchDRM_RIGHT_PLAYBACK, g_rgchDRM_RIGHT_PLAYBACK_LEN );

const DRM_WCHAR        g_rgwchDRM_RIGHT_PLAYBACK[] = {
    ONE_WCHAR('P', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('y', '\0'),
    ONE_WCHAR('\0', '\0')
};
const DRM_CONST_STRING g_dstrDRM_RIGHT_PLAYBACK    = CREATE_DRM_STRING( g_rgwchDRM_RIGHT_PLAYBACK );

/* WMDRM-NDT_Device_Attr_Last_Validation_Time */
const DRM_WCHAR g_rgwchDeviceAttrLastValidationTime[] = { ONE_WCHAR('W', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('R', '\0'),
                                                          ONE_WCHAR('M', '\0'), ONE_WCHAR('-', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('D', '\0'),
                                                          ONE_WCHAR('T', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('e', '\0'),
                                                          ONE_WCHAR('v', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'),
                                                          ONE_WCHAR('_', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('t', '\0'),
                                                          ONE_WCHAR('r', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('a', '\0'),
                                                          ONE_WCHAR('s', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('V', '\0'),
                                                          ONE_WCHAR('a', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('d', '\0'),
                                                          ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'),
                                                          ONE_WCHAR('n', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('i', '\0'),
                                                          ONE_WCHAR('m', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0','\0') };
const DRM_CONST_STRING g_dstrDeviceAttrLastValidationTime = CREATE_DRM_STRING( g_rgwchDeviceAttrLastValidationTime );


EXIT_PK_NAMESPACE_CODE;
