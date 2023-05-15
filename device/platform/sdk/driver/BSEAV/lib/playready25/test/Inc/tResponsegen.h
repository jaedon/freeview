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

DRM_RESULT TestLicResponseSetClientID(const CLIENTID *pClientID);

DRM_RESULT TestResponseInit(__in _XMBContext *pbContext, __in DRM_DWORD cbContext);
DRM_RESULT TestResponseAddLicense(__in _XMBContext *pbContext, __in DRM_CONST_STRING *pwszLicense);
DRM_RESULT TestResponseAddSLK(
    __in       _XMBContext      *pbContext,
    __in const DRM_CONST_STRING *pdstrSlkId,
    __in const DRM_CONST_STRING *pdstrSlkData );
DRM_RESULT TestResponseAddNode(__in _XMBContext *pbContext,
                            __in const DRM_CONST_STRING *pwszNodeName,
                            __in const DRM_CONST_STRING *pwszAttrName,
                            __in const DRM_CONST_STRING *pwszAttrValue,
                            __in const DRM_BYTE *pbData,
                            __in const DRM_DWORD cbData);

DRM_RESULT TestResponseAddRevocationInfo(
    __in        _XMBContext      *pbContext,
    __in  const DRM_CONST_STRING *pwszRevocationInfo );
DRM_RESULT TestResponseAddRevocation(
    __in        _XMBContext      *pbContext,
    __in  const DRM_CONST_STRING *pwszGUID,
    __in  const DRM_CONST_STRING *pwszRevocation );


/*    Caller should free **ppbResponse. */
DRM_RESULT TestResponseGetResponse(__in _XMBContext *pbContext, __deref_out_ecount_opt(*pcbResponse) DRM_BYTE **ppbResponse, __out DRM_DWORD *pcbResponse);

EXIT_PKTEST_NAMESPACE

#endif /*_tResponseGen_h*/
