/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMWMDRMUTILREAL_C
#include <drmwmdrm.h>
#include <drmconstants.h>
#include <drmxmlparser.h>
#include <drmwmdrmconstants.h>
#include <drmlastinclude.h>

#if DRM_SUPPORT_SECUREOEMHAL
#error DRM_SUPPORT_SECUREOEMHAL is set.  You can not implement a secure HAL that supports WMDRM.  You must link to the stub version of the WMDRM module.
#endif /* DRM_SUPPORT_SECUREOEMHAL */

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_IsWMDRMSupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_IsWMDRMUnsupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_HDR_VerifyLegacySignature(
    __in const DRM_CONST_STRING   *f_pdstrContentHeader,
    __in const DRM_CONST_STRING   *f_pdstrPubKey,
    __in       DRM_CONST_STRING   *f_pdstrData,
    __in       DRM_CRYPTO_CONTEXT *f_pContext )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CONST_STRING dstrValue  = EMPTY_DRM_STRING;
    DRM_DWORD        dwSize     = SIZEOF( PUBKEY );

    ChkDRMString( f_pdstrContentHeader );
    ChkDRMString( f_pdstrPubKey );
    ChkDRMString( f_pdstrData );
    ChkArg( f_pContext );

    /* verification of V2 header - signature section */
    if( DRM_FAILED( DRM_XML_GetSubNodeByPath(  f_pdstrContentHeader,
                                              &g_dstrXPathSigValue,
                                               NULL,
                                               NULL,
                                               NULL,
                                              &dstrValue,
                                               g_wchForwardSlash ) ) )
    {
        ChkDR( DRM_E_CH_INVALID_HEADER );
    }

    dwSize = DRM_ECC160_SIGNATURE_LEN;
    if( DRM_FAILED( DRM_B64_DecodeW( &dstrValue, &dwSize, f_pContext->signature, 0 ) ) )
    {
        ChkDR( DRM_E_CH_UNABLE_TO_VERIFY );
    }

    /* decode the pubkey and verify the signature with it */
    ChkDR( DRM_B64_DecodeW( f_pdstrPubKey, &dwSize, ( DRM_BYTE* )&f_pContext->pubKey, 0 ) );
    ChkDR( DRM_PK_Verify(   f_pContext->rgbCryptoContext,
                            eDRM_ECC_P160,
                            DRM_ECC160_PUBLIC_KEY_LEN,
                          ( const DRM_BYTE* )&f_pContext->pubKey,
                            CB_DSTR( f_pdstrData ),
                            PB_DSTR( f_pdstrData ),
                            DRM_ECC160_SIGNATURE_LEN,
                            f_pContext->signature ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

