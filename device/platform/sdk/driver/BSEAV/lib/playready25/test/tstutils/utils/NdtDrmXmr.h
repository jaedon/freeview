/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
 * DrmXmr.h
 *
 * Header file for routines in DrmXmr.c
 */


#ifndef _DRMXMR_H_
#define _DRMXMR_H_

#include "ndtdrmxmrdefs.h"
#include "oemrsaoaeppss.h"
#include "oemaes.h"
#include "drmxmr.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
 * Procedure forwards
 */

DRM_RESULT
DrmXmrUnpackLicense(
    __in const DRM_RSA_PRIVATE_KEY_2048BIT *pPrivateKey,
    __in DRM_BYTE *Message,
    __in DRM_DWORD MessageSize,
    __out XMR_LICENSE **ppXmrLicense
    );

DRM_RESULT
DrmXmrGetKeyFromLicense(
    __in const DRM_RSA_PRIVATE_KEY_2048BIT *pPrivateKey,
    __in const XMR_LICENSE *pXmrLicense ,
    __in DRM_BOOL bGetContentEncryptionKey,
    __out DRM_AES_KEY **ppAesKey
    );

DRM_VOID
DrmXmrFreeLicense(
    __in XMR_LICENSE *pXmrLicense
    );

DRM_RESULT
DrmXmrGetKeyFromLicenseEx(
    __in DRM_BOOL bRSAPrivateKey,
    __in const DRM_BYTE *pbDecryptionKey,
    __in const XMR_LICENSE *pXmrLicense ,
    __in DRM_BOOL bGetContentEncryptionKey,
    __out DRM_AES_KEY **ppAesKey
    );

DRM_RESULT
DrmXmrUnpackRootLicense(
    __in const DRM_RSA_PRIVATE_KEY_2048BIT *pPrivateKey,
    __in DRM_BYTE *Message,
    __in DRM_DWORD MessageSize,
    __out XMR_ROOT_LICENSE_INFO *pXmrRootLicenseInfo
    );

DRM_RESULT
DrmXmrUnpackLeafLicense(
    __in XMR_ROOT_LICENSE_INFO XmrRootLicenseInfo,
    __in DRM_BYTE *Message,
    __in DRM_DWORD MessageSize,
    __out XMR_LICENSE **ppXmrLicense
    );


EXIT_PKTEST_NAMESPACE

#endif /* _DRMXMR_H_ */

