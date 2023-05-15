/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _TSTHDSUTILS_H_
#define _TSTHDSUTILS_H_

#include "drmcommon.h"
#include "drmutilities.h"
#include "drmcontextsizes.h"
#include "drmdatastore.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/* Allocates the hds context and opens the specific store. If fails, then *ppHds==NULL */
DRM_RESULT DRM_CALL OpenHDS(DRM_DST **ppHds, const DRM_WCHAR *pwszStorePath, DRM_BOOL fCreateNew);

/* Close the hds store and release the context. *pHds is invalid after the call succeeds. */
DRM_RESULT DRM_CALL CloseHDS(DRM_DST *pHds);


EXIT_PKTEST_NAMESPACE

DRM_RESULT DRM_CALL TST_CheckForLicenseSecureStoreEntry( __in DRM_WCHAR *f_pwszFullPath, 
                                                         __in DRM_ID    *f_pLID );

DRM_RESULT DRM_CALL TST_GetNameSpaceSlotCount( __in const DRM_WCHAR *f_pwszFullPath,
                                               __in const DRM_CHAR  *f_pszNameSpaceName,
                                               __out DRM_DWORD      *f_pdwInUse );

#endif /* _TSTHDSUTILS_H_ */

