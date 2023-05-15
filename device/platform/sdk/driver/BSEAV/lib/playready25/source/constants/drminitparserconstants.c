/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <drmtypes.h>
#include <drminitparserconstants.h>

ENTER_PK_NAMESPACE_CODE;


/* "Metering" */
DRM_STR_CONST DRM_WCHAR g_rgwchMeteringInitiator [] =
{
    INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'e'),
    INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 'g'),
    INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrMeteringInitiator = CREATE_DRM_STRING( g_rgwchMeteringInitiator );

/* "JoinDomain" */
DRM_STR_CONST DRM_WCHAR g_rgwchJoinDomainInitiator [] =
{
    INIT_WCHAR_OBFUS( 'J'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'n'),
    INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'a'),
    INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrJoinDomainInitiator = CREATE_DRM_STRING( g_rgwchJoinDomainInitiator );

/* "LeaveDomain" */
DRM_STR_CONST DRM_WCHAR g_rgwchLeaveDomainInitiator [] =
{
    INIT_WCHAR_OBFUS( 'L'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'v'),
    INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'm'),
    INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrLeaveDomainInitiator = CREATE_DRM_STRING( g_rgwchLeaveDomainInitiator );

/* "LicenseAcquisition" */
DRM_STR_CONST DRM_WCHAR g_rgwchLicenseAcquisitionInitiator [] =
{
    INIT_WCHAR_OBFUS( 'L'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'c'), INIT_WCHAR_OBFUS( 'e'),
    INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'A'),
    INIT_WCHAR_OBFUS( 'c'), INIT_WCHAR_OBFUS( 'q'), INIT_WCHAR_OBFUS( 'u'), INIT_WCHAR_OBFUS( 'i'),
    INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'i'),
    INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrLicenseAcquisitionInitiator = CREATE_DRM_STRING( g_rgwchLicenseAcquisitionInitiator );

/* "CustomData" */
DRM_STR_CONST DRM_WCHAR g_rgwchCustomDataInitiatorTag [] =
{
    INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'u'), INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( 't'),
    INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( 'a'),
    INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrCustomDataInitiatorTag = CREATE_DRM_STRING( g_rgwchCustomDataInitiatorTag );

/* "PlayReadyInitiator" */
DRM_STR_CONST DRM_WCHAR g_rgwchPRInitiatorTag[] =
{
    INIT_WCHAR_OBFUS( 'P'), INIT_WCHAR_OBFUS( 'l'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'y'),
    INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'd'),
    INIT_WCHAR_OBFUS( 'y'), INIT_WCHAR_OBFUS( 'I'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 'i'),
    INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 't'),
    INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrPRInitiatorTag = CREATE_DRM_STRING( g_rgwchPRInitiatorTag );

/* "CertificateServer" */
DRM_STR_CONST DRM_WCHAR g_rgwchCertificateServerTag [] =
{
    INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 't'),
    INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'f'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'c'),
    INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'S'),
    INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 'v'), INIT_WCHAR_OBFUS( 'e'),
    INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrCertificateServerTag = CREATE_DRM_STRING( g_rgwchCertificateServerTag );

/* "MeteringID" */
DRM_STR_CONST DRM_WCHAR g_rgwchMeteringIDTag [] =
{
    INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'e'),
    INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 'g'),
    INIT_WCHAR_OBFUS( 'I'), INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrMeteringIDTag = CREATE_DRM_STRING( g_rgwchMeteringIDTag );

/* "MaxPackets" */
DRM_STR_CONST DRM_WCHAR g_rgwchMaxPacketsTag [] =
{
    INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'x'), INIT_WCHAR_OBFUS( 'P'),
    INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'c'), INIT_WCHAR_OBFUS( 'k'), INIT_WCHAR_OBFUS( 'e'),
    INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrMaxPacketsTag = CREATE_DRM_STRING( g_rgwchMaxPacketsTag );

/* "DomainController" */
DRM_STR_CONST DRM_WCHAR g_rgwchDomainControllerTag [] =
{
    INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'a'),
    INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'o'),
    INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 'o'),
    INIT_WCHAR_OBFUS( 'l'), INIT_WCHAR_OBFUS( 'l'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'),
    INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrDomainControllerTag = CREATE_DRM_STRING( g_rgwchDomainControllerTag );

/* "DS_ID" */
DRM_STR_CONST DRM_WCHAR g_rgwchServiceIDTag [] =
{
    INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( 'S'), INIT_WCHAR_OBFUS( '_'), INIT_WCHAR_OBFUS( 'I'),
    INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrServiceIDTag = CREATE_DRM_STRING( g_rgwchServiceIDTag );

/* "AccountID" */
DRM_STR_CONST DRM_WCHAR g_rgwchAccountIDTag [] =
{
    INIT_WCHAR_OBFUS( 'A'), INIT_WCHAR_OBFUS( 'c'), INIT_WCHAR_OBFUS( 'c'), INIT_WCHAR_OBFUS( 'o'),
    INIT_WCHAR_OBFUS( 'u'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'I'),
    INIT_WCHAR_OBFUS( 'D'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrAccountIDTag = CREATE_DRM_STRING( g_rgwchAccountIDTag );

/* "Content" */
DRM_STR_CONST DRM_WCHAR g_rgwchContentURLTag [] =
{
    INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 't'),
    INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrContentURLTag = CREATE_DRM_STRING( g_rgwchContentURLTag );

/* "Header" */
DRM_STR_CONST DRM_WCHAR g_rgwchHeaderTag [] =
{
    INIT_WCHAR_OBFUS( 'H'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'd'),
    INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrHeaderTag = CREATE_DRM_STRING( g_rgwchHeaderTag );

/* "WMConvert" */
DRM_STR_CONST DRM_WCHAR g_rgwchWMConvertTag [] =
{
    INIT_WCHAR_OBFUS( 'W'), INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'o'),
    INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 'v'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'),
    INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( '\0')
};
const DRM_CONST_STRING g_dstrWMConvertTag = CREATE_DRM_STRING( g_rgwchWMConvertTag );


EXIT_PK_NAMESPACE_CODE;