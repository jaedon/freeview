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
    IN DRM_RSA_PRIVATE_KEY *pPrivateKey,
    IN DRM_BYTE *Message,
    IN DRM_DWORD MessageSize,
    OUT XMR_LICENSE **ppXmrLicense
    );

DRM_RESULT
DrmXmrGetKeyFromLicense(
    IN DRM_RSA_PRIVATE_KEY *pPrivateKey,
    IN XMR_LICENSE *pXmrLicense ,
    IN DRM_BOOL bGetContentEncryptionKey,
    OUT DRM_AES_KEY **ppAesKey
    );

DRM_VOID
DrmXmrFreeLicense(
    IN XMR_LICENSE *pXmrLicense
    );

DRM_RESULT
DrmXmrGetKeyFromLicenseEx(
    IN DRM_BOOL bRSAPrivateKey,
    IN DRM_BYTE *pbDecryptionKey,
    IN XMR_LICENSE *pXmrLicense ,
    IN DRM_BOOL bGetContentEncryptionKey,
    OUT DRM_AES_KEY **ppAesKey
    );

DRM_RESULT
DrmXmrUnpackRootLicense(
    IN DRM_RSA_PRIVATE_KEY *pPrivateKey,
    IN DRM_BYTE *Message,
    IN DRM_DWORD MessageSize,
    OUT XMR_ROOT_LICENSE_INFO *pXmrRootLicenseInfo
    );

DRM_RESULT
DrmXmrUnpackLeafLicense(
    IN XMR_ROOT_LICENSE_INFO XmrRootLicenseInfo,
    IN DRM_BYTE *Message,
    IN DRM_DWORD MessageSize,
    OUT XMR_LICENSE **ppXmrLicense
    );


EXIT_PKTEST_NAMESPACE

#endif /* _DRMXMR_H_ */

