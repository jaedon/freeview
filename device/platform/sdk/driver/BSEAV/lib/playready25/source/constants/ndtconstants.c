/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <ndtconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

/* Note: when updating a string, don't forget to update the comment and length
 * of the string also.
 */

/* "MTP" */
DRM_STR_CONST DRM_CHAR  g_rgchRegistrationAddress       [] = { INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('\0')};
#define g_rgchRegistrationAddress_LEN 3

/* "c:WMDRMReceiver" */
DRM_STR_CONST DRM_CHAR  g_rgchTagWMDRMReceiver          [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('v'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagWMDRMReceiver_LEN 15

/* "c:WMDRMTransmitter" */
DRM_STR_CONST DRM_CHAR  g_rgchTagWMDRMTransmitter       [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('s'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagWMDRMTransmitter_LEN 18

/* "c:WMDRMSharedCertificate" */
DRM_STR_CONST DRM_CHAR  g_rgchTagWMDRMSharedCertificate [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('h'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('f'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagWMDRMSharedCertificate_LEN 24

/* "Play" */
DRM_STR_CONST DRM_CHAR  g_rgchDRM_RIGHT_PLAYBACK        [] = { INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('y'), INIT_CHAR_OBFUS('\0')};
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

DRM_STR_CONST DRM_WCHAR        g_rgwchDRM_RIGHT_PLAYBACK[] = {
    INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('y'),
    INIT_WCHAR_OBFUS('\0')
};
const DRM_CONST_STRING g_dstrDRM_RIGHT_PLAYBACK    = CREATE_DRM_STRING( g_rgwchDRM_RIGHT_PLAYBACK );

/* Device_Attr_Last_Validation_Time */
DRM_STR_CONST DRM_WCHAR g_rgwchDeviceAttrLastValidationTime[] = {
    INIT_WCHAR_OBFUS('D'), INIT_WCHAR_OBFUS('e'), INIT_WCHAR_OBFUS('v'), INIT_WCHAR_OBFUS('i'),
    INIT_WCHAR_OBFUS('c'), INIT_WCHAR_OBFUS('e'), INIT_WCHAR_OBFUS('_'), INIT_WCHAR_OBFUS('A'),
    INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('r'), INIT_WCHAR_OBFUS('_'),
    INIT_WCHAR_OBFUS('L'), INIT_WCHAR_OBFUS('a'), INIT_WCHAR_OBFUS('s'), INIT_WCHAR_OBFUS('t'),
    INIT_WCHAR_OBFUS('_'), INIT_WCHAR_OBFUS('V'), INIT_WCHAR_OBFUS('a'), INIT_WCHAR_OBFUS('l'),
    INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('d'), INIT_WCHAR_OBFUS('a'), INIT_WCHAR_OBFUS('t'),
    INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('o'), INIT_WCHAR_OBFUS('n'), INIT_WCHAR_OBFUS('_'),
    INIT_WCHAR_OBFUS('T'), INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('m'), INIT_WCHAR_OBFUS('e'),
    INIT_WCHAR_OBFUS('\0') };
const DRM_CONST_STRING g_dstrDeviceAttrLastValidationTime = CREATE_DRM_STRING( g_rgwchDeviceAttrLastValidationTime );

EXIT_PK_NAMESPACE_CODE;

