/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXKEYFILEREAD_C

#include <oemkeyfile.h>
#include <oemhal.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************************************************
** Function:    DRM_BBX_KF_CalculateOmac
**
** Synopsis:    Creates an OMAC signature of the specified keyfile data
**              using the AES keyfile decryption key specified or the current AES keyfile decryption key.
**
** Arguments:   [f_pOEMContext]             : A pointer to the OEM context; may be NULL
**              [f_pguidAESDecryptionKeyID] : Specifies on input the AES decryption key ID to use
**                                            for calculating the OMAC if f_fUseCurrentKey is FALSE,
**                                            otherwise is undefined on input and contains
**                                            the current AES decryption key ID used in the OMAC on return.
**              [f_fUseCurrentKey]          : If FALSE, the AES key to use for the OMAC calculation is specified
**                                            by the f_pguidAESDecryptionKeyID parameter. If TRUE then
**                                            the current AES keyfile key is used instead
**                                            and returned in f_ pguidAESDecryptionKeyID.
**              [f_pbData]                  : Pointer to the keyfile data to be signed
**              [f_cbData]                  : Size in bytes of the keyfile data to be signed
**              [f_rgbSignatureBuffer]      : Buffer to receive the OMAC signature
**
** Notes:       This function is called extensively from areas where there is no Blackbox object available.
**              For this reason the function uses the OEM HAL layer directly, rather than using blackbox
**              key handles
**
*****************************************************************************************************************/
DRM_RESULT DRM_API DRM_CALL DRM_BBX_KF_CalculateOmac (
    __in_opt    DRM_VOID    *f_pOEMContext,
    __inout     DRM_GUID    *f_pguidAESDecryptionKeyID,
    __in        DRM_BOOL     f_fUseCurrentKey,
    __in_bcount(f_cbData) const DRM_BYTE *f_pbData,
    __in        DRM_DWORD    f_cbData,
    __inout_bcount( DRM_AES_BLOCKLEN ) DRM_BYTE f_rgbSignatureBuffer[DRM_AES_BLOCKLEN]
)
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_BOOL                    fHalInitialized = FALSE;
    DRM_GUID                    emptyGuid       = EMPTY_DRM_GUID;
    OEM_HAL_KEY_REGISTER_INDEX  ikeyFileKey     = -1;
    DRM_DWORD                   cbSignature     = DRM_AES_BLOCKLEN;

    ChkArg( f_pguidAESDecryptionKeyID != NULL ); /* even if undefined it should not be NULL */
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );

    /*
    ** If using the default key, initialize the key buffer to zero
    */
    if ( f_fUseCurrentKey )
    {
        *f_pguidAESDecryptionKeyID = emptyGuid;
    }

    /*
    ** As we don't know whether the HAL is initialized, make sure. The HAL reference counts
    ** initializations, so doing this won't have any effect if it is already initialized
    */
    ChkDR( Oem_Hal_Initialize() );

    fHalInitialized = TRUE;

    ChkDR( Oem_Hal_GetPreloadedIndex( f_pOEMContext,
                                      OEM_HAL_KEY_TYPE_AES_KEYFILE,
                                      ( DRM_BYTE* )f_pguidAESDecryptionKeyID,
                                      SIZEOF( DRM_GUID ),
                                      &ikeyFileKey ) );

    ChkDR( Oem_Hal_CreateOMAC1Signature( f_pbData,
                                         f_cbData,
                                         f_rgbSignatureBuffer,
                                         &cbSignature,
                                         OEM_HAL_KEY_TYPE_AES_KEYFILE,
                                         ikeyFileKey ) );

    DRMASSERT( cbSignature == DRM_AES_BLOCKLEN );

ErrorExit:
    /*
    ** No need to free the key register as it is a preallocated key register
    */

    /*
    ** Deinitialize the HAL if it was successfully initialized
    */
    if( fHalInitialized )
    {
        ( DRM_VOID)Oem_Hal_Deinitialize();
    }

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

