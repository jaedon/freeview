/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMTESTRSAUTILS_H__
#define __DRMTESTRSAUTILS_H__ 1

#include <drmtypes.h>
#include <drmrevocationenum.h>
#include <oemrsa.h>

#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE

DRM_CHAR DRM_API TST_UTL_tolower( DRM_CHAR szChar );
DRM_RESULT ParseGUID(DRM_GUID *pGuid, __in_ecount(DRM_GUID_STRING_LEN) DRM_BYTE *pString);
DRM_RESULT ParseDRMID(DRM_ID *pTo, const char *pFrom);
DRM_RESULT Test_DrmRevocationTypeEnumGUID( DRM_REVOCATION_TYPE_ENUM eRevType, DRM_GUID **ppGuid );
DRM_RESULT DRM_CALL Test_ReadRSAPrivateKeyFromFile2048( const char *pszFile, DRM_RSA_PRIVATE_KEY_2048BIT *pPrivateKey );

EXIT_PKTEST_NAMESPACE

#endif /* __DRMTESTRSAUTILS_H__ */

