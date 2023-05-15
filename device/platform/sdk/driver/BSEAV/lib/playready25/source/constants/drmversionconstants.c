/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmversionconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;


const DRM_WCHAR g_rgwchReqTagPlayReadyClientVersionData[] = { ONE_WCHAR('2', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('5', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('\0', '\0') };
const DRM_CONST_STRING g_dstrReqTagPlayReadyClientVersionData = CREATE_DRM_STRING( g_rgwchReqTagPlayReadyClientVersionData );


const DRM_WCHAR g_rgwchReqTagWMDRMPDClientVersionData[] = { ONE_WCHAR('1', '\0'), ONE_WCHAR('2', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('5', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('1', '\0'), ONE_WCHAR('7', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('8', '\0'), ONE_WCHAR('\0', '\0') };
const DRM_CONST_STRING g_dstrReqTagWMDRMPDClientVersionData = CREATE_DRM_STRING( g_rgwchReqTagWMDRMPDClientVersionData );


const DRM_DWORD g_dwWMDRMPDVersionMajor = 12;
const DRM_DWORD g_dwWMDRMPDVersionMinor = 5;
const DRM_DWORD g_dwWMDRMPDVersionRevision = 0;
const DRM_DWORD g_dwWMDRMPDVersionBuild = 1788;


EXIT_PK_NAMESPACE_CODE;
