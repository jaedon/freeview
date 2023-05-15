/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _TSTXMLUTILS_H_
#define _TSTXMLUTILS_H_

#include "drmcommon.h"
#include "drmutilities.h"
#include "drmcontextsizes.h"
#include "tstutils.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/* wszXML is <XX><DATA>...</DATA><SIGNATURE>...</SIGNATURE></XX> 
*  This function verify the <DATA> section using the public key.
*/
DRM_RESULT DRM_CALL VerifyXMLDataSigature(DRM_CONST_STRING *pwszXML, DRM_CONST_STRING *pwszPubKey);

EXIT_PKTEST_NAMESPACE

#endif /* _TSTXMLUTILS_H_ */

