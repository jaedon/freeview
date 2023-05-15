/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsecureclockconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_WCHAR      g_rgwchTagError                              [] =  { INIT_WCHAR_OBFUS('E'), INIT_WCHAR_OBFUS('R'), INIT_WCHAR_OBFUS('R'), INIT_WCHAR_OBFUS('O'), INIT_WCHAR_OBFUS('R'), INIT_WCHAR_OBFUS('\0') }; /* "ERROR" */
DRM_STR_CONST                DRM_WCHAR      g_rgwchExprVarInGracePeriod                  [] =  { INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('n'), INIT_WCHAR_OBFUS('g'), INIT_WCHAR_OBFUS('r'), INIT_WCHAR_OBFUS('a'), INIT_WCHAR_OBFUS('c'), INIT_WCHAR_OBFUS('e'), INIT_WCHAR_OBFUS('p'), INIT_WCHAR_OBFUS('e'), INIT_WCHAR_OBFUS('r'), INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('o'), INIT_WCHAR_OBFUS('d'), INIT_WCHAR_OBFUS('\0') }; /* "ingraceperiod" */
DRM_STR_CONST                DRM_WCHAR      g_rgwchExprVarGpStartTime                    [] =  { INIT_WCHAR_OBFUS('g'), INIT_WCHAR_OBFUS('p'), INIT_WCHAR_OBFUS('s'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('a'), INIT_WCHAR_OBFUS('r'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('m'), INIT_WCHAR_OBFUS('e'), INIT_WCHAR_OBFUS('\0') }; /* "gpstarttime" */
DRM_STR_CONST                DRM_WCHAR      g_rgwchStatus                                [] =  { INIT_WCHAR_OBFUS('s'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('a'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('u'), INIT_WCHAR_OBFUS('s'), INIT_WCHAR_OBFUS('\0') }; /* "status" */
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagGmtTime                            [] =  { INIT_WCHAR_OBFUS('G'), INIT_WCHAR_OBFUS('M'), INIT_WCHAR_OBFUS('T'), INIT_WCHAR_OBFUS('T'), INIT_WCHAR_OBFUS('I'), INIT_WCHAR_OBFUS('M'), INIT_WCHAR_OBFUS('E'), INIT_WCHAR_OBFUS('\0') }; /* "GMTTIME" */
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagRefreshDate                        [] =  { INIT_WCHAR_OBFUS('R'), INIT_WCHAR_OBFUS('E'), INIT_WCHAR_OBFUS('F'), INIT_WCHAR_OBFUS('R'), INIT_WCHAR_OBFUS('E'), INIT_WCHAR_OBFUS('S'), INIT_WCHAR_OBFUS('H'), INIT_WCHAR_OBFUS('D'), INIT_WCHAR_OBFUS('A'), INIT_WCHAR_OBFUS('T'), INIT_WCHAR_OBFUS('E'), INIT_WCHAR_OBFUS('\0') }; /* "REFRESHDATE" */
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagDRMReqRoot                         [] =  { INIT_WCHAR_OBFUS('D'), INIT_WCHAR_OBFUS('R'), INIT_WCHAR_OBFUS('M'), INIT_WCHAR_OBFUS('C'), INIT_WCHAR_OBFUS('L'), INIT_WCHAR_OBFUS('O'), INIT_WCHAR_OBFUS('C'), INIT_WCHAR_OBFUS('K'), INIT_WCHAR_OBFUS('\0') }; /* "DRMCLOCK" */

DRM_STR_CONST                DRM_WCHAR      g_rgwchDefaultPlayReadySecureClockServerUrl  [] = { INIT_WCHAR_OBFUS('h'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('p'), INIT_WCHAR_OBFUS(':'), INIT_WCHAR_OBFUS('/'), INIT_WCHAR_OBFUS('/'), INIT_WCHAR_OBFUS('g'), INIT_WCHAR_OBFUS('o'), INIT_WCHAR_OBFUS('.'), INIT_WCHAR_OBFUS('m'), INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('c'), INIT_WCHAR_OBFUS('r'), INIT_WCHAR_OBFUS('o'), INIT_WCHAR_OBFUS('s'), INIT_WCHAR_OBFUS('o'), INIT_WCHAR_OBFUS('f'), INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('.'), INIT_WCHAR_OBFUS('c'), INIT_WCHAR_OBFUS('o'), INIT_WCHAR_OBFUS('m'), INIT_WCHAR_OBFUS('/'), INIT_WCHAR_OBFUS('f'), INIT_WCHAR_OBFUS('w'), INIT_WCHAR_OBFUS('l'), INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('n'), INIT_WCHAR_OBFUS('k'), INIT_WCHAR_OBFUS('/'), INIT_WCHAR_OBFUS('?'), INIT_WCHAR_OBFUS('L'), INIT_WCHAR_OBFUS('i'), INIT_WCHAR_OBFUS('n'), INIT_WCHAR_OBFUS('k'), INIT_WCHAR_OBFUS('I'), INIT_WCHAR_OBFUS('d'), INIT_WCHAR_OBFUS('='), INIT_WCHAR_OBFUS('1'), INIT_WCHAR_OBFUS('4'), INIT_WCHAR_OBFUS('9'), INIT_WCHAR_OBFUS('4'), INIT_WCHAR_OBFUS('0'), INIT_WCHAR_OBFUS('8'), INIT_WCHAR_OBFUS('\0') }; /* "http://go.microsoft.com/fwlink/?LinkId=149408" */

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagError                              = CREATE_DRM_STRING( g_rgwchTagError );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrExprVarInGracePeriod                  = CREATE_DRM_STRING( g_rgwchExprVarInGracePeriod );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrExprVarGpStartTime                    = CREATE_DRM_STRING( g_rgwchExprVarGpStartTime );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrStatus                                = CREATE_DRM_STRING( g_rgwchStatus );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagGmtTime                            = CREATE_DRM_STRING( g_rgwchTagGmtTime );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagRefreshDate                        = CREATE_DRM_STRING( g_rgwchTagRefreshDate );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDRMReqRoot                         = CREATE_DRM_STRING( g_rgwchTagDRMReqRoot );

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDefaultPlayReadySecureClockServerUrl  = CREATE_DRM_STRING( g_rgwchDefaultPlayReadySecureClockServerUrl );

const DRM_DISCARDABLE PUBKEY g_pubkeyDefaultPlayReadySecureClockServer =
{
    {
        0xF8, 0x23, 0x61, 0xBE, 0xFC, 0xF5, 0x59, 0xA3,
        0x55, 0xD4, 0x01, 0x54, 0x99, 0xEB, 0x71, 0x92,
        0xF9, 0xBD, 0x88, 0x3E, 0x14, 0xAC, 0x4F, 0x5C,
        0x9C, 0x65, 0x22, 0xFA, 0xA7, 0x2A, 0x77, 0x13,
        0x22, 0x5E, 0x60, 0xF5, 0xFE, 0x29, 0x18, 0x18
    }
};

EXIT_PK_NAMESPACE_CODE;

