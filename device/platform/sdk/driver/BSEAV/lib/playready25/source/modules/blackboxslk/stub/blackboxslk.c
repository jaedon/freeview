/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXSLK_C
#include <blackboxlegacy.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*
** Functions defining whether Symmetric License Keys (SLKs) are supported
*/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsSlkCryptoSupported(DRM_VOID)
{
    return FALSE;
}
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsSlkCryptoUnsupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_RESULT DRM_CALL _SLK_RebindSymmetricKeyXMR(
    __inout         DRM_BB_CONTEXT       *f_pcontextBBX,
    __in      const DRM_SLK              *f_pslk,
    __inout         DRM_XMR_LICENSE      *f_pXmrLicense )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL _SLK_RebindSLK(
    __inout_ecount( 1 )             DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbEncSLK ) const DRM_BYTE       *f_pbEncSLK,
    __in                            DRM_DWORD       f_cbEncSLK,
    __out_ecount( 1 )               DRM_SLK        *f_pslk )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL _SLK_RebindSymmetricKeyXML(
    __inout         DRM_BB_CONTEXT       *f_pcontextBBX,
    __in      const DRM_SLK              *f_pslk,
    __inout         DRM_XML_BINDING_INFO *f_pXmlBindInfo,
    __in            DRM_DWORD             f_cbSymmKey )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

