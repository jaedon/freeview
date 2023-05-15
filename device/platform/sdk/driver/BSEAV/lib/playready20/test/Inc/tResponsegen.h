/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _tResponseGen_h
#define _tResponseGen_h

#include "drmcommon.h"

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

DRM_RESULT TestLicResponseSetClientID(CLIENTID *pClientID);

DRM_RESULT TestResponseInit(IN _XMBContext *pbContext, IN DRM_DWORD cbContext);
DRM_RESULT TestResponseAddLicense(IN _XMBContext *pbContext, IN DRM_CONST_STRING *pwszLicense);
DRM_RESULT TestResponseAddSLK(IN _XMBContext *pbContext, IN DRM_CONST_STRING *pwszSLKID, IN DRM_CONST_STRING *pwszSLKData);
DRM_RESULT TestResponseAddNode(IN _XMBContext *pbContext, 
                            IN const DRM_CONST_STRING *pwszNodeName,
                            IN const DRM_CONST_STRING *pwszAttrName,
                            IN const DRM_CONST_STRING *pwszAttrValue,
                            IN const DRM_BYTE *pbData,
                            IN const DRM_DWORD cbData);

DRM_RESULT TestResponseAddRevocationInfo(IN _XMBContext *pbContext, IN DRM_CONST_STRING *pwszRevocationInfo);
DRM_RESULT TestResponseAddRevocation(IN _XMBContext *pbContext, IN DRM_CONST_STRING *pwszGUID, IN DRM_CONST_STRING *pwszRevocation);


/*    Caller should free **ppbResponse. */
DRM_RESULT TestResponseGetResponse(IN _XMBContext *pbContext, OUT DRM_BYTE **ppbResponse, OUT DRM_DWORD *pcbResponse);
 
EXIT_PKTEST_NAMESPACE

#endif /*_tResponseGen_h*/
