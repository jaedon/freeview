/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __XMR2XML_H__
#define __XMR2XML_H__

DRM_RESULT DRM_CALL _PrintNameableGuid(       DRM_BYTE          *f_pbXmlBuffer,
                                        const DRM_GUID          *pGuid,
                                        const DRM_CONST_STRING  *pdstrLabel );

DRM_RESULT DateTimeToString( DRM_UINT64 f_u64DateTime, DRM_STRING *f_pdstrDateTime );

DRM_RESULT AddGenericValueToXml( const DRM_BYTE             *f_pbValue,
                                       DRM_DWORD             f_cbValue,
                                 const DRM_CONST_STRING     *f_pdstrTagName,
                                       DRM_BYTE             *f_pbXml,
                                       DRM_DWORD             f_cbXml,
                                       DRM_BOOL              f_fAlsoDisplayAsDecimal,
                                       DRM_BOOL              f_fInNetworkByteOrder );

DRM_RESULT AddBLOBToXml(
    const DRM_CONST_STRING *f_pdstrTag,
    const DRM_BYTE         *f_pbTokenData,
          DRM_DWORD         f_cbTokenData,
          DRM_BYTE         *f_pbXmlBuffer,
          DRM_DWORD         f_cbXmlBuffer,
          DRM_BOOL          f_fB64Encode );

DRM_RESULT FormatXmrAsXml(
    __in_ecount( f_cbXMR ) const DRM_BYTE       *f_pbXMR,
    __in                         DRM_DWORD       f_cbXMR,
    __out_ecount(f_cbXmlBuffer)  DRM_BYTE       *f_pbXmlBuffer,
    __in                         DRM_DWORD       f_cbXmlBuffer,
    __out_ecount(1)              DRM_STRING     *f_pdstrXml );

#endif /* __XMR2XML_H__ */
