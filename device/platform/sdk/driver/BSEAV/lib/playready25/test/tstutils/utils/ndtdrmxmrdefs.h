/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
 * drmxmrdefs.h
 *
 * This file describes the XMR License format
 */

#ifndef _PFD_DRM_XMR_DEFS_H_
#define _PFD_DRM_XMR_DEFS_H_

#include "drmcommon.h"
#include <oemaes.h>

typedef struct __tagXMR_INCLUSION_LIST
{
    DRM_DWORD  cGUIDS;
    DRM_GUID   *pGUIDS;
} XMR_INCLUSION_LIST;
typedef struct __tagXMR_GLOBAL_REQUIREMENTS
{
    DRM_WORD  wMinAppSecurityLevel;
    DRM_WORD  wReserved;
    DRM_ID    SerialNumber;
    XMR_INCLUSION_LIST il;
    DRM_DWORD dwReserved1;
    DRM_DWORD dwReserved2;
    DRM_DWORD dwSourceId;
    DRM_DWORD   dwRevInfoSeq;
} XMR_GLOBAL_REQUIREMENTS;

typedef struct __tagXMR_MINIMUM_OUTPUT_PROTECTION_LEVELS
{
    DRM_WORD wReserved1;
    DRM_WORD wUncompressedDigitalVideo;
    DRM_WORD wAnalogVideo;
    DRM_WORD wReserved2;
    DRM_WORD wUncompressedDigitalAudio;

} XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS;

typedef struct __tagXMR_VIDEO_OUTPUT_PROTECTION
{
    DRM_GUID   guidId;
    DRM_DWORD  cbConfigData;
    DRM_BYTE  *pbConfigData;

} XMR_VIDEO_OUTPUT_PROTECTION;

typedef struct __tagXMR_VIDEO_OUTPUT_PROTECTION_IDS
{
    DRM_DWORD cEntries;
    XMR_VIDEO_OUTPUT_PROTECTION *rgVop;

} XMR_VIDEO_OUTPUT_PROTECTION_IDS;

typedef struct __tagXMR_CONTENT_KEY
{
    DRM_GUID guidKeyId;
    DRM_WORD wSymmetricCipherType;
    DRM_WORD wAsymmetricCipherType;
    DRM_WORD cbEncryptedKey;
    DRM_BYTE *pbEncryptedKey;

} XMR_CONTENT_KEY;

typedef struct __tagXMR_UPLINK_KID
{
    DRM_ID idKID;
    DRM_ID idChainedChecksum;
} XMR_UPLINK_KID;
typedef struct __tagXMR_DEVICE_KEY
{
    DRM_DWORD  dwExponent;
    DRM_WORD   wModulusLength;
    DRM_BYTE  *pbModulus;
} XMR_DEVICE_KEY;


typedef struct __tagXMR_KEY_MATERIAL
{
    XMR_DEVICE_KEY dk;
    XMR_CONTENT_KEY ck;
    XMR_UPLINK_KID uk;
} XMR_KEY_MATERIAL;

typedef struct __tagXMR_LICENSE
{
    DRM_ID RightsId;
    XMR_GLOBAL_REQUIREMENTS gr;
    XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS mopl;
    XMR_VIDEO_OUTPUT_PROTECTION_IDS vopi;
    XMR_KEY_MATERIAL km;
} XMR_LICENSE;

typedef struct __tagXMR_ROOT_LICENSE_INFO
{
    XMR_LICENSE *pXmrLicense;
    DRM_ID *pIDChainedChecksum;
    DRM_AES_KEY *pAesContentKey;

} XMR_ROOT_LICENSE_INFO;

#endif /* _DRM_XMR_DEFS_H_ */
