/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef _TGENERATORUTILS_H_
#define _TGENERATORUTILS_H_

#include "drmcommon.h"
#include "drmutilities.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE
DRM_API DRM_RESULT DRM_CALL TST_UTL_CopyDRMString( __out DRM_CONST_STRING      *f_pdstrTarget,
                                                   __in const DRM_CONST_STRING *f_pdstrSource );

DRM_API DRM_RESULT DRM_CALL TST_UTL_MakeRandomKID( __inout DRM_CONST_STRING *f_pdstrKID );
EXIT_PKTEST_NAMESPACE

#endif /* _TGENERATORUTILS_H_ */


