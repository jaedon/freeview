/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsymopt.h>
#include <drmxmlparser.h>
#include <oemeccp160.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
**  Function:  DRM_SYMOPT_DDC_VerifySymmetricSignature
**
**  Synopsis:  Verifies the symmetric signature of the device certificate.
**
**  Arguments:
**     [f_pdstrCert] -- Certificate to verify
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_DDC_VerifySymmetricSignature(
    IN DRM_VOID         *f_pOEMContext,
    IN DRM_CONST_STRING *f_pdstrCert )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    PRIVKEY          GCPrivKey        = { 0 };
    DRM_DWORD        cbGCPrivKey      = SIZEOF( PRIVKEY );
    DRM_CONST_STRING dstrCertNodeData = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataNodeData = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrInsert       = EMPTY_DRM_STRING;
    DRM_BYTE         rgbSymmSig[__CB_DECL(DRM_SHA1_DIGEST_LEN)] = {0};
    DRM_DWORD        cbSymmSig = SIZEOF( rgbSymmSig );


    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DEVICEDEVCERT, PERF_FUNC_DRM_SYMOPT_DDC_VerifySymmetricSignature);

    /* Extract the symmertic signature and verify it */
    ChkDR( DRM_XML_GetSubNode( f_pdstrCert,
                              &g_dstrTagCertificate,
                              &g_dstrAttributeType,
                              &g_dstrTagDevice,
                               0,
                               NULL,
                              &dstrCertNodeData,
                               1) );

    ChkDR( DRM_XML_GetNode( &dstrCertNodeData,
                            &g_dstrTagData,
                             NULL,
                             NULL,
                             0,
                            &dstrDataNodeData,
                             NULL) );

    ChkDR( DRM_XML_GetNode( &dstrCertNodeData,
                            &g_dstrTagSymSig,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dstrInsert));


    ChkDR( DRM_B64_DecodeW( &dstrInsert, &cbSymmSig, rgbSymmSig, 0 ) );

    ChkDR( Oem_Device_GetKey( f_pOEMContext, DRM_DKT_WMDRMPD_GROUP, (DRM_BYTE *)&GCPrivKey, &cbGCPrivKey ) );

    ChkDR( DRM_HMAC_VerifyMAC( eDRM_SHA_1,
                               (DRM_BYTE*)&GCPrivKey,
                                SIZEOF( GCPrivKey ),
                                PB_DSTR(&dstrDataNodeData),
                                0,
                                CB_DSTR(&dstrDataNodeData),
                                rgbSymmSig,
                                0,
                                DRM_SHA1_DIGEST_LEN ) );

    dr = DRM_SUCCESS;

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &GCPrivKey );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

