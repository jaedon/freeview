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

const                DRM_WCHAR      g_rgwchTagError                              [] =  { ONE_WCHAR('E', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('\0', '\0') }; /* "ERROR" */
const                DRM_WCHAR      g_rgwchExprVarInGracePeriod                  [] =  { ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('\0', '\0') }; /* "ingraceperiod" */
const                DRM_WCHAR      g_rgwchExprVarGpStartTime                    [] =  { ONE_WCHAR('g', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('\0', '\0') }; /* "gpstarttime" */
const                DRM_WCHAR      g_rgwchStatus                                [] =  { ONE_WCHAR('s', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0') }; /* "status" */
const                DRM_WCHAR      g_rgwchTagGmtTime                            [] =  { ONE_WCHAR('G', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0') }; /* "GMTTIME" */
const                DRM_WCHAR      g_rgwchTagRefreshDate                        [] =  { ONE_WCHAR('R', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('H', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0') }; /* "REFRESHDATE" */
const                DRM_WCHAR      g_rgwchTagDRMReqRoot                         [] =  { ONE_WCHAR('D', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('\0', '\0') }; /* "DRMCLOCK" */

const                DRM_WCHAR      g_rgwchDefaultPlayReadySecureClockServerUrl  [] = { ONE_WCHAR('h', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR(':', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('f', '\0'), ONE_WCHAR('w', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('/', '\0'), ONE_WCHAR('?', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('=', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('4', '\0'), ONE_WCHAR('9', '\0'), ONE_WCHAR('4', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('\0', '\0') }; /* "http://go.microsoft.com/fwlink/?LinkId=149408" */

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

