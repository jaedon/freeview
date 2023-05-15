/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmtypes.h>
#include <drmcertparserconstants.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_CHAR  g_rgchFeatureSampleProtection   [] = { INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('\0')};
#define g_rgchFeatureSampleProtection_LEN 23
DRM_STR_CONST                DRM_CHAR  g_rgchFeatureExport             [] = { INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('\0')};
#define g_rgchFeatureExport_LEN 13
DRM_STR_CONST                DRM_CHAR  g_rgchFeatureExportIncl         [] = { INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('W'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('x'), INIT_CHAR_OBFUS('p'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('t'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('\0')};
#define g_rgchFeatureExportIncl_LEN 17
DRM_STR_CONST                DRM_CHAR  g_rgchKeyUsageSignCRL           [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('i'), INIT_CHAR_OBFUS('g'), INIT_CHAR_OBFUS('n'), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('R'), INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('\0')};
#define g_rgchKeyUsageSignCRL_LEN 9
DRM_STR_CONST                DRM_CHAR  g_rgchTagModelNumber            [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('o'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('l'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('u'), INIT_CHAR_OBFUS('m'), INIT_CHAR_OBFUS('b'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagModelNumber_LEN 13
DRM_STR_CONST                DRM_CHAR  g_rgchTagHardwareID             [] = { INIT_CHAR_OBFUS('c'), INIT_CHAR_OBFUS(':'), INIT_CHAR_OBFUS('H'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('d'), INIT_CHAR_OBFUS('w'), INIT_CHAR_OBFUS('a'), INIT_CHAR_OBFUS('r'), INIT_CHAR_OBFUS('e'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagHardwareID_LEN 12

const DRM_ANSI_CONST_STRING  g_dastrFeatureSampleProtection             = CREATE_DRM_ANSI_STRING_EX( g_rgchFeatureSampleProtection, g_rgchFeatureSampleProtection_LEN );
const DRM_ANSI_CONST_STRING  g_dastrFeatureExport                       = CREATE_DRM_ANSI_STRING_EX( g_rgchFeatureExport, g_rgchFeatureExport_LEN );
const DRM_ANSI_CONST_STRING  g_dastrFeatureExportIncl                   = CREATE_DRM_ANSI_STRING_EX( g_rgchFeatureExportIncl, g_rgchFeatureExportIncl_LEN);
const DRM_ANSI_CONST_STRING  g_dastrKeyUsageSignCRL                     = CREATE_DRM_ANSI_STRING_EX( g_rgchKeyUsageSignCRL, g_rgchKeyUsageSignCRL_LEN );
const DRM_ANSI_CONST_STRING  g_dastrTagModelNumber                      = CREATE_DRM_ANSI_STRING_EX( g_rgchTagModelNumber, g_rgchTagModelNumber_LEN );
const DRM_ANSI_CONST_STRING  g_dastrTagHardwareID                       = CREATE_DRM_ANSI_STRING_EX( g_rgchTagHardwareID, g_rgchTagHardwareID_LEN );

EXIT_PK_NAMESPACE_CODE;