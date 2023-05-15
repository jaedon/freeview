/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_WMDRMNETIMPLREAL_C
#include <drmwmdrmnet.h>
#include <oemrsaoaeppss.h>
#include <drmpubkeydeviceexclusion.h>
#include <drmrevocation.h>
#include <drmxmlparser.h>
#include <drmblobcache.h>
#include <drmcertparser.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_INVALID_PARAM_VALUE_1, "*f_ppEntries should not yield this warning given __checkReturn and declaration of DRM_RESULT, but it still does." )
__checkReturn DRM_RESULT DRM_NO_INLINE DRM_CALL DRM_RVK_GetWMDRMNETRevocationEntries(
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Out params can't be const")
    __inout_bcount( f_cbRevocationData )  DRM_BYTE            *f_pbRevocationData,
PREFAST_POP
    __in                                  DRM_DWORD            f_cbRevocationData,
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Out params can't be const")
    __out_ecount(1)                       DRM_DWORD           *f_pcEntries,
PREFAST_POP
    __deref_out_ecount( *f_pcEntries )    WMDRMNET_CRL_ENTRY **f_ppEntries )
PREFAST_POP /* __WARNING_INVALID_PARAM_VALUE_1 */
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD cEntries  = 0;
    DRM_DWORD cbEntries = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetWMDRMNETRevocationEntries );

    ChkArg(f_pbRevocationData != NULL
        && f_pcEntries        != NULL
        && f_ppEntries        != NULL );

    *f_pcEntries = 0;
    *f_ppEntries = NULL;

    /*
    ** (check that there is enough room for both the CRL ver and the entry count)
    */

    ChkArg( f_cbRevocationData >= (SIZEOF(DRM_DWORD) * 2) );

    /*
    ** skip over the version DRM_DWORD
    */

    f_pbRevocationData += SIZEOF(DRM_DWORD);
    f_cbRevocationData -= SIZEOF(DRM_DWORD);

    /*
    ** get the entry count and skip over it
    */

    NETWORKBYTES_TO_DWORD( cEntries, f_pbRevocationData, 0 );
    f_pbRevocationData += SIZEOF(DRM_DWORD);
    f_cbRevocationData -= SIZEOF(DRM_DWORD);

    if (cEntries > 0)
    {
        *f_pcEntries = cEntries;

        /*
        ** Get the entries.  we don't need to copy them, just update
        ** our array pointer to point to the right location.
        */
        cbEntries = cEntries * SIZEOF( WMDRMNET_CRL_ENTRY );

        ChkArg( f_cbRevocationData >= cbEntries );

        *f_ppEntries = (WMDRMNET_CRL_ENTRY*) f_pbRevocationData;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function :   DRM_RVK_UpdateWMDRMNETRevocationList
**
** Synopsis :   updates the WMDRMNET revocation list in the secure store
**
** Arguments :
** [f_pcontextCrypto]      -- Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code
** [f_pContextRev]         -- revocation store context
** [f_pbRevocationList]    -- input buffer for exclusion list
** [f_cbRevocationList]    -- buffer size
** [f_pdstrRevocationList] -- the new DCRL

** Returns :
**
** Notes :
**
******************************************************************************/
DRM_RESULT DRM_RVK_UpdateWMDRMNETRevocationList(
    __in                                                                                 DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                                                                 DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "f_rgchBase length defined by f_pdasstrRevocationList")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Out params can't be const")
    __inout_ecount( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch )    DRM_CHAR                    *f_rgchBase,
PREFAST_POP
PREFAST_POP
    __in                                                                           const DRM_SUBSTRING               *f_pdasstrRevocationList,
    __in_bcount( f_cbRevocationBuffer )                                            const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                                                                 DRM_DWORD                    f_cbRevocationBuffer,
    __in                                                                           const PUBKEY                      *f_ppubkey )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_DWORD        cbDecoded    = 0;
    const DRM_BYTE  *pbDecoded   = NULL;
    DRM_DWORD        cbAligned      = 0;
    DRM_BYTE        *pbAligned      = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateWMDRMNETRevocationList );

    ChkArg(   f_pcontextCrypto   != NULL
           && f_pContextRev      != NULL
           && f_rgchBase           != NULL
           && f_pbRevocationBuffer != NULL
           && f_cbRevocationBuffer > 0
           && f_pdasstrRevocationList != NULL
           && f_pdasstrRevocationList->m_cch > 0 );


     /* pbOffset will point within the base64 revocation list we were passed,
       such that after base64 decoding the end of the decoded data will co-incide with
       the end of the buffer we were given, minus one byte.

       This is because we are giving the B64 decoder overlapping memory for source and desitination,
       and we can't use the DECODE_IN_PLACE flag because this is ANSI where the offset may not coincide
       with a word boundary as required on 16-bit platforms.
     */

     pbDecoded = (DRM_BYTE*)f_rgchBase
                        + __CB_DECL( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch - CB_BASE64_DECODE( f_pdasstrRevocationList->m_cch ) - 1 )
                        + ( ( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch - CB_BASE64_DECODE( f_pdasstrRevocationList->m_cch ) ) % CB_NATIVE_BYTE );

    cbDecoded = CB_BASE64_DECODE( f_pdasstrRevocationList->m_cch ) + 1 - ( ( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch - CB_BASE64_DECODE( f_pdasstrRevocationList->m_cch ) ) % CB_NATIVE_BYTE );

    /* Ensure the pointer is DRM_WCHAR aligned */
    ChkDR( DRM_UTL_EnsureDataAlignment( pbDecoded, cbDecoded, &pbAligned, &cbAligned, SIZEOF( DRM_WCHAR ), NULL ) );
    ChkBOOL( cbAligned >= CB_BASE64_DECODE( f_pdasstrRevocationList->m_cch ), DRM_E_BUFFERTOOSMALL );

    /* decode the XML for the new exclusion list in place */
    ChkDR( DRM_B64_DecodeA( f_rgchBase,
                            f_pdasstrRevocationList,
                           &cbAligned,
                            pbAligned,
                            0 ) );

    ChkDR( DRM_RVK_UpdateWMDRMNETRevocationListDecoded(
        f_pcontextCrypto,
        f_pContextRev,
        pbAligned,
        cbAligned,
        f_pbRevocationBuffer,
        f_cbRevocationBuffer,
        NULL,
        NULL) );


ErrorExit:
    if( ( f_rgchBase != NULL )
        && ( f_pdasstrRevocationList != NULL )
        && ( f_pdasstrRevocationList->m_cch != 0 ) )
    {
        /* Since we decoded in-place, we need to clean up otherwise the XML parsing may
         * not work properly later on. So we have to overwrite our binary with spaces.
         *
         * NOTE: This means that we CANNOT process the same revocation list in the same
         * license response buffer twice since we're overwriting the buffer we were given
         */
        DRM_BYT_SetBytes( f_rgchBase, f_pdasstrRevocationList->m_ich, f_pdasstrRevocationList->m_cch, ' ');
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*****************************************************************************
** Function: DRM_RVK_GetWMDRMNETList
**
** Synopsis: gets the current link protection list from the secure store
**
** Arguments:
** [f_pcontextCRYP]   -- a buffer
** [f_pContextRev]    -- revocation store context
** [f_pbRevocationData]  -- output buffer for exclusion list; NULL to request
**                       required size
** [f_pcbRevocationData] -- DRM_DWORD to hold max buffer size on in, bytes
**                       actually used on out
** [f_pidSerial]      -- receives numerical representation of <INDEX> tag
*****************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_GetWMDRMNETList(
    __in                                           DRM_CRYPTO_CONTEXT           *f_pcontextCRYP,
    __in                                           DRM_REVOCATIONSTORE_CONTEXT  *f_pContextRev,
    __out_bcount_opt( *f_pcbRevocationData ) const DRM_BYTE                     *f_pbRevocationData,
    __inout                                        DRM_DWORD                    *f_pcbRevocationData,
    __out                                          DRM_DWORD                    *f_pidSerial )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT          dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetWMDRMNETList );

    ChkArg(   f_pcontextCRYP        != NULL
           && f_pContextRev         != NULL
           && f_pcbRevocationData   != NULL
           && f_pidSerial           != NULL );

    dr = DRM_RVK_GetSSTRevocationList(
        &g_guidRevocationTypeWMDRMNET,
        f_pcontextCRYP,
        f_pContextRev,
        &g_pubkeyWMDRMNDRevocation,
        f_pbRevocationData,
        f_pcbRevocationData,
        f_pidSerial );
    if ( dr == DRM_E_FILENOTFOUND
      || dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        dr                   = DRM_SUCCESS;
        *f_pidSerial         = 0;
        *f_pcbRevocationData = 0;
        goto ErrorExit;
    }
    ChkDR(dr);

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function :   DRM_RVK_VerifyWMDRMNETRevocationList
**
** Synopsis :   Verify signature on the WMDRMNET CRL and return the version
**              of the CRL.
**
** Arguments :  f_pContextRev   - Revocation store context (optional)
**              f_pdstrList     - WMDRMNET Revocation list
**              f_pdwVersion    - Version or Serial number on revocation list
**              f_pCryptoCtx    - Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns :
**
** Notes :      If the verification fails, the buffer will be filled with
**              spaces to prevent the XML parsing from being confused later
**
******************************************************************************/
DRM_NO_INLINE DRM_RESULT DRM_RVK_VerifyWMDRMNETRevocationList(
    __in_opt DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in     DRM_CONST_STRING            *f_pdstrList,
    __out    DRM_DWORD                   *f_pdwVersion,
    __in     DRM_CRYPTO_CONTEXT          *f_pCryptoCtx )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr                 = DRM_SUCCESS;
    DRM_DWORD  cbData             = 0;
    DRM_DWORD  cbEncoded         = 0;
    DRM_STRING dstrRevocationData = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyWMDRMNETRevocationList );

    ChkArg( f_pdstrList  != NULL
         && f_pdwVersion != NULL );
    ChkArg( f_pCryptoCtx != NULL );


    DSTR_FROM_PB( &dstrRevocationData,
                  PB_DSTR( f_pdstrList ),
                  CB_DSTR( f_pdstrList ) );

    cbData = CB_BASE64_DECODE(dstrRevocationData.cchString);


    ChkDR( DRM_B64_DecodeW( (DRM_CONST_STRING *) &dstrRevocationData,
                         &cbData,
                          NULL,
                          DRM_BASE64_DECODE_IN_PLACE ) );

    ChkDR( DRM_RVK_VerifyBinaryWMDRMNETSignature(
        f_pContextRev,
        (DRM_BYTE*)dstrRevocationData.pwszString,
        cbData,
        f_pdwVersion,
        f_pCryptoCtx ) );

    cbEncoded = dstrRevocationData.cchString;
    ChkDR( DRM_B64_EncodeW( (DRM_BYTE*)dstrRevocationData.pwszString,
                         cbData,
                         dstrRevocationData.pwszString,
                        &cbEncoded,
                         0 ) );

 ErrorExit:

    /*
     * If verification failed, the CRL may be in binary form and could
     * could mess up XML parsing later on, so we overwrite it with spaces.
     */
    if( DRM_FAILED( dr ) )
    {
        DRM_BYT_SetBytes( dstrRevocationData.pwszString,
                    0,
                    dstrRevocationData.cchString,
                    ' ' );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function :   DRM_RVK_UpdateWMDRMNETRevocationListDecoded
**
** Synopsis :   updates the WMDRMNET revocation list in the secure store
**
** Arguments :
** [f_pcontextCrypto]      -- Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code
** [f_pContextRev]         -- revocation store context
** [f_pbRevocationList]    -- input buffer for exclusion list
** [f_cbRevocationList]    -- buffer size
** [f_pdstrRevocationList] -- the new DCRL
** [f_pfUpdated]           -- Optional flag indicates whether the CRL was
**                            updated in the HDS or not.
** [f_pdwVersion]          -- optional parameter which receives the version of the
**                            revocation list that is saved on the system when this
**                            function exits. This is not necessarily the version of the
**                            CRL passed into this function, since we only save
**                            crls if they're a higher version than what is already
**                            there.
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_NO_INLINE DRM_RESULT DRM_RVK_UpdateWMDRMNETRevocationListDecoded(
    __in                                     DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                     DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_ecount(f_cbRevocationList)          DRM_BYTE                    *f_pbRevocationList,
    __in                                     DRM_DWORD                    f_cbRevocationList,
    __in_ecount(f_cbRevocationBuffer)  const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                     DRM_DWORD                    f_cbRevocationBuffer,
    __out_opt                                DRM_BOOL                    *f_pfUpdated,
    __out_opt                                DRM_DWORD                   *f_pdwVersion )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_DWORD        idSerialNew  = 0,
                     idSerialOld  = 0,
                     cbReturned   = 0;
    DRM_CONST_STRING dstrTemplate  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrList       = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateWMDRMNETRevocationListDecoded );

    ChkArg(   f_pcontextCrypto   != NULL
           && f_pContextRev      != NULL
           && f_pbRevocationList           != NULL
           && f_pbRevocationBuffer != NULL
           && f_cbRevocationBuffer > 0
           && f_cbRevocationList > 0 );

    if( f_pfUpdated != NULL )
    {
        *f_pfUpdated = FALSE;
    }
    if ( f_pdwVersion != NULL )
    {
        *f_pdwVersion = 0;
    }

    DSTR_FROM_PB( &dstrList, f_pbRevocationList, f_cbRevocationList );

    idSerialNew = DRM_NO_PREVIOUS_CRL;
    idSerialOld = DRM_NO_PREVIOUS_CRL;
    if ( DRM_RVK_IsWrappedCRL( &dstrList ) )
    {
        ChkDR( DRM_RVK_VerifyRevocationList( f_pContextRev,
                                             f_pcontextCrypto,
                                             &dstrList,
                                             &g_pubkeyWMDRMNDRevocation,
                                             &idSerialNew ) );

        ChkDR( DRM_XML_GetSubNode( &dstrList,
                                  &g_dstrTemplate,
                                   NULL,
                                   NULL,
                                   0,
                                   NULL,
                                  &dstrTemplate,
                                   1 ) );

        /* We will store the unwrapped CRL in decoded form */
        ChkDR( DRM_B64_DecodeW( (const DRM_CONST_STRING*)&dstrTemplate,
                               &f_cbRevocationList,
                               NULL,
                               DRM_BASE64_DECODE_IN_PLACE ) );

        f_pbRevocationList = (DRM_BYTE*)dstrTemplate.pwszString;

    }
    else
    {
        ChkDR( DRM_RVK_VerifyBinaryWMDRMNETSignature(
            f_pContextRev,
            f_pbRevocationList,
            f_cbRevocationList,
            &idSerialNew,
            f_pcontextCrypto ) );
    }


    cbReturned = f_cbRevocationBuffer;
    /* get and verify the current SST image
     */
    dr = DRM_RVK_GetSSTRevocationList(
        &g_guidRevocationTypeWMDRMNET,
        f_pcontextCrypto,
        f_pContextRev,
        &g_pubkeyWMDRMNDRevocation,
        f_pbRevocationBuffer,
        &cbReturned,
        &idSerialOld );

    /* This can be the first Revocation list */

    if ( dr == DRM_E_DST_SLOT_NOT_FOUND
        || ( ( idSerialOld == DRM_NO_PREVIOUS_CRL ) && DRM_SUCCEEDED( dr ) )
        || ( ( idSerialOld < idSerialNew ) && DRM_SUCCEEDED( dr ) ) )
    {

        /* store the new list. */
        ChkDR( DRM_RVS_StoreRevocationData( f_pContextRev,
                                           &g_guidRevocationTypeWMDRMNET,
                                            f_pbRevocationList,
                                            f_cbRevocationList ) );
        if( f_pfUpdated != NULL )
        {
            *f_pfUpdated = TRUE;
        }
        if (f_pdwVersion)
        {
            *f_pdwVersion = idSerialNew;
        }
    }
    else
    {
        ChkDR( dr );

        /* CRL did not get updated, so set it to the old version. */
        if (f_pdwVersion)
        {
            *f_pdwVersion = idSerialOld;
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_VerifyBinaryWMDRMNETSignature(
    __in_opt                      DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_bcount( f_cbData ) const DRM_BYTE                    *f_pbData,
    __in                          DRM_DWORD                    f_cbData,
    __inout                       DRM_DWORD                   *f_pidSerial,
    __in                          DRM_CRYPTO_CONTEXT          *f_pCryptoCtx )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_BYTE            bSignatureType  = 0;
    DRM_DWORD           cEntries        = 0;
    DRM_UINT64          cbEntries       = DRM_UI64LITERAL(0, 0);
    DRM_DWORD           cbSigned        = 0;
    DRM_DWORD           dwVersion       = 0;
    DRM_SUBSTRING       dasstrCertificate = EMPTY_DRM_SUBSTRING;
    DRM_DWORD           cbData          = f_cbData;
    DRM_DWORD           ibData          = 0;
    DRM_WORD           cbSignature      = 0;
#if SIXTEEN_BIT_ADDRESSING
    DRM_BYTE            rgbSignature[ __CB_DECL( WMDRMNET_CRL_SIGNATURE_LEN_RSA_SHA1 ) ];
#endif
    _RevocationStoreContext *pRevStoreContextInternal = ( _RevocationStoreContext * )f_pContextRev;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyBinaryWMDRMNETSignature );

    /*
    ** Get the CRL Version
    */

    ChkArg(f_cbData     >= SIZEOF(DRM_DWORD)
        && f_pbData     != NULL
        && f_pidSerial  != NULL
        && f_pCryptoCtx != NULL );

    NETWORKBYTES_TO_DWORD( dwVersion, f_pbData, 0 );
    cbSigned += SIZEOF(DRM_DWORD);
    ibData   += SIZEOF(DRM_DWORD);
    cbData   -= SIZEOF(DRM_DWORD);

    /* if the expected version number is given it must match, otherwise its' a query */

    if (*f_pidSerial != DRM_NO_PREVIOUS_CRL)
    {
        ChkArg(dwVersion == *f_pidSerial);
    }
    else
    {
        *f_pidSerial = dwVersion;
    }

    /*
    ** Get the number of entries
    */
    ChkArg(cbData >= SIZEOF(DRM_DWORD));

    NETWORKBYTES_TO_DWORD( cEntries, f_pbData, ibData );
    cbSigned += SIZEOF(DRM_DWORD);
    ibData   += SIZEOF(DRM_DWORD);
    cbData   -= SIZEOF(DRM_DWORD);

    /*
    ** Get the size of the entries
    **
    */
    cbEntries = DRM_UI64Mul(DRM_UI64HL( 0, cEntries ), DRM_UI64HL( 0, SIZEOF(WMDRMNET_CRL_ENTRY) ));

    ChkArg(cbData >= DRM_UI64Low32(cbEntries)); /* use qword to prevent arithmetic overflows */


    cbSigned += SIZEOF(WMDRMNET_CRL_ENTRY) * cEntries;
    ibData   += SIZEOF(WMDRMNET_CRL_ENTRY) * cEntries;
    cbData   -= SIZEOF(WMDRMNET_CRL_ENTRY) * cEntries;

    /*
    ** Get the certificate length
    */

    ChkArg(cbData >= SIZEOF(DRM_DWORD));

    NETWORKBYTES_TO_DWORD( dasstrCertificate.m_cch, f_pbData, ibData );

    cbSigned += SIZEOF(DRM_DWORD);
    ibData   += SIZEOF(DRM_DWORD);
    cbData   -= SIZEOF(DRM_DWORD);

    ChkArg(dasstrCertificate.m_cch > 0);

    /*
    ** Get the certificate.  we don't need to copy it, just update
    ** our byte offset to point to the right location.
    */

    ChkArg(cbData >= dasstrCertificate.m_cch);
    dasstrCertificate.m_ich = ibData;


    cbSigned += dasstrCertificate.m_cch;
    ibData   += dasstrCertificate.m_cch;
    cbData   -= dasstrCertificate.m_cch;

    /*
    ** Get the signature type
    */

    ChkArg(cbData >= SIZEOF(DRM_BYTE));

    bSignatureType = GET_BYTE( f_pbData, ibData );

    ibData++;
    cbData--;

    ChkArg( bSignatureType == WMDRMNET_CRL_SIGNATURE_TYPE_RSA_SHA1 );

    /*
    ** get the signature length
    */

    ChkArg(cbData >= SIZEOF(DRM_WORD));

    NETWORKBYTES_TO_WORD( cbSignature, f_pbData, ibData );
    ibData += SIZEOF(DRM_WORD);
    cbData -= SIZEOF(DRM_WORD);

    ChkArg( cbSignature == WMDRMNET_CRL_SIGNATURE_LEN_RSA_SHA1 );

    /*
    ** get the signature and validate it
    */
    ChkArg(cbData >= WMDRMNET_CRL_SIGNATURE_LEN_RSA_SHA1);

#if SIXTEEN_BIT_ADDRESSING
    DRM_BYT_CopyBytes( rgbSignature,
                      0,
                      f_pbData,
                      ibData,
                      WMDRMNET_CRL_SIGNATURE_LEN_RSA_SHA1 );

    dr = DRM_RVK_VerifyCRLSignature(
        f_pContextRev == NULL ? NULL : pRevStoreContextInternal->pContextBB,
        f_pContextRev == NULL ? NULL : pRevStoreContextInternal->pContextSST,
        f_pContextRev == NULL ? NULL : pRevStoreContextInternal->pDatastore,
        f_pbData,
        cbSigned,
        rgbSignature,
        WMDRMNET_CRL_SIGNATURE_LEN_RSA_SHA1,
        dasstrCertificate,
        f_pCryptoCtx );
#else
    dr = DRM_RVK_VerifyCRLSignature(
        f_pContextRev == NULL ? NULL : pRevStoreContextInternal->pContextBB,
        f_pContextRev == NULL ? NULL : pRevStoreContextInternal->pContextSST,
        f_pContextRev == NULL ? NULL : pRevStoreContextInternal->pDatastore,
        f_pbData,
        cbSigned,
        f_pbData + ibData,
        WMDRMNET_CRL_SIGNATURE_LEN_RSA_SHA1,
        dasstrCertificate,
        f_pCryptoCtx );
#endif

    if (DRM_FAILED(dr))
    {
        ChkDR(DRM_E_INVALID_SIGNATURE);
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    _ValidateCertificate
**
** Synopsis:    Validates the device certificate. Optionally returns WMDRM-ND certificate
**
** Arguments:
**              [f_rgchDeviceCertificate]     - Specifies the device certificate
**              [f_cbDeviceCertificate]       - Specifies the size (in bytes) of the Device Certificate
**              [f_pdasstrWMDRMNDCertificate] - Optional, can be NULL.  If not NULL, returns the
**                                              DRM_SUBSTRING pointing to the WMDRM-ND certificate
**              [f_pCryptoCtx]          -- Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ValidateCertificate(
    __in       const DRM_CHAR             *f_rgchBase,
    __in             DRM_SUBSTRING         f_dasstrCertificate,
    __inout_opt      DRM_SUBSTRING        *f_pdasstrWMDRMNDCertificate,
    __in             DRM_CRYPTO_CONTEXT   *f_pCryptoCtx )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_rgchBase );
    ChkArg( f_pCryptoCtx != NULL );

    if (DRM_FAILED( DRM_WCP_VerifyCertificateCollection(
                             f_rgchBase,
                            &f_dasstrCertificate,
                            &g_dastrMSNDRootPubKeyB64,
                             NULL,
                             0,
                             f_pdasstrWMDRMNDCertificate,
                             NULL,
                             f_pCryptoCtx ) ) )
    {
        ChkDR(DRM_E_INVALID_DEVICE_CERTIFICATE);
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_RVK_VerifyCRLSignature
**
** Synopsis:    Verifies the CRL signature.
**
** Arguments:
**              [f_pcontextBBX]          - Optional pointer to a black box context
**              [f_pcontextSST]          - Optional pointer to a secure store context
**              [f_pDatastore]           - Optional pointer to a data store context
**              [f_pbSignedBytes]        - Specifies the bytes that are signed
**              [f_cbSignedBytes]        - Specifies the size of the signed bytes
**              [f_pbSignature]          - Specifies the signature
**              [f_cbSignature]          - Specifies the size signature
**              [f_pbCertificate]        - Specifies the certificate chain used to find signing key
**              [f_cbCertificate]        - Specifies the size certificate chain used to find signing key
**              [f_pCryptoCtx]          -- Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     DRM_SUCCESS if the signature is valid
**              DRM_XMLNOTFOUND if unable to locate signing key
**              passes return codes from other failures up to caller
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_VerifyCRLSignature(
    __in_opt                             DRM_BB_CONTEXT       *f_pcontextBBX,
    __in_opt                             DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in_opt                             DRM_DST              *f_pDatastore,
    __in_bcount( f_cbSignedBytes ) const DRM_BYTE             *f_pbSignedBytes,
    __in                                 DRM_DWORD             f_cbSignedBytes,
    __in_bcount( f_cbSignature )   const DRM_BYTE             *f_pbSignature,
    __in                                 DRM_DWORD             f_cbSignature,
    __in                                 DRM_SUBSTRING         f_dasstrCertificate,
    __in                                 DRM_CRYPTO_CONTEXT   *f_pCryptoCtx )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT          dr     = DRM_SUCCESS;

    DRM_RSA_PUBLIC_KEY_2048BIT  pubKey = { 0 };
    DRM_BOOL fVerified = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyCRLSignature );

    ChkDR( DRM_BLOBCACHE_Verify( f_pcontextBBX,
                                 f_pcontextSST,
                                 f_pDatastore,
                                 f_pbSignedBytes,
                                 f_cbSignedBytes,
                                 f_pbSignature,
                                 f_cbSignature,
                                 &fVerified ) );

    if ( fVerified )
    {
        goto ErrorExit;
    }

    ChkArg( f_pCryptoCtx    != NULL );
    /* validate certificate chain */
    dr = _ValidateCertificate( (const DRM_CHAR *) f_pbSignedBytes, f_dasstrCertificate, NULL, f_pCryptoCtx );
    if( DRM_FAILED(dr) )
    {
        TRACE(( "DRM_RVK_VerifyCRLSignature: Invalid Certificate\n" ));
        ChkDR( dr );
    }

    /* find certificate that signed CRL */
    {
        DRM_SUBSTRING       dasstrCollection = f_dasstrCertificate;
        DRM_SUBSTRING       dasstrData = EMPTY_DRM_SUBSTRING;
        DRM_DWORD           cNodes;
        DRM_DWORD           iNode;


        ChkDR( DRM_XML_GetNodeA( (const DRM_CHAR *) f_pbSignedBytes, &dasstrCollection, &g_dastrTagCertificateCollection, NULL, NULL, 0, NULL, &dasstrData ));

        /* enumerate c:Certificate nodes */

        ChkDR(DRM_XML_CountMatchingNodesA( (const DRM_CHAR *) f_pbSignedBytes,
                                        &dasstrData,
                                        &g_dastrTagWMDRMCertificate,
                                        NULL,
                                        NULL,
                                        &cNodes));

        ChkArg(cNodes >= 1
            && cNodes <= DRM_CERTIFICATE_COLLECTION_MAX);

        for (iNode = 0; iNode < cNodes; iNode++)
        {
            DRM_SUBSTRING dasstrCertificate = EMPTY_DRM_SUBSTRING;
            DRM_SUBSTRING dasstrTag         = EMPTY_DRM_SUBSTRING;
            DRM_SUBSTRING rgdasstrAttrs [3]   = { 0 };
            DRM_DWORD     cAttrEntries        = NO_OF(rgdasstrAttrs);
            DRM_DWORD     dwUsage    = 0;

            ChkDR(DRM_XML_EnumNextNodeA( (const DRM_CHAR *) f_pbSignedBytes,
                                        &dasstrData,
                                         iNode,
                                        &dasstrTag,
                                        &dasstrCertificate,
                                         NULL,
                                         NULL,
                                         NULL));

            /* there should be nothing but <c:Certificate>s in <CertificateCollection> */

            ChkArg(DRM_UTL_DASSTRStringsEqual( (const DRM_CHAR *) f_pbSignedBytes,
                                              &dasstrTag,
                                              &g_dastrTagWMDRMCertificate));

            /* Check if cert signed CRL, and if so, get public key */
            ChkDR( DRM_WCP_GetKeyUsage( (const DRM_CHAR *) f_pbSignedBytes,
                                        &dasstrCertificate,
                                        &dwUsage ) );

            if( dwUsage & DRM_WMDRM_CERT_KEYUSAGE_SIGNCRL )
            {
                DRM_DWORD cbDestination;
                DRM_BYTE  rgbExponent[__CB_DECL( DRM_RSA_CB_PUBEXP )];
                DRM_BYTE  rgbModulus [__CB_DECL(DRM_RSA_CB_MODULUS_MAX_2048BIT * 2 + 8)] = { 0 };
                DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT eKeyLength = eDRM_RSA_KEY_LENGTH_UNKNOWN_2048BIT;

                cAttrEntries = 3;
                ChkDR( DRM_WCP_GetAttribute( (const DRM_CHAR *) f_pbSignedBytes,
                                            &dasstrCertificate,
                                             DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SELF_RSA,
                                             rgdasstrAttrs,
                                            &cAttrEntries ) );

                /*
                *  Decode Exponent
                */
                cbDestination = 4;
                ChkDR( DRM_B64_DecodeA( (const DRM_CHAR *) f_pbSignedBytes,
                                        rgdasstrAttrs,
                                       &cbDestination,
                                        rgbExponent,
                                        0 ) );
                PUT_BYTE( rgbExponent, 3, GET_BYTE( rgbExponent, 2 ) );
                PUT_BYTE( rgbExponent, 2, GET_BYTE( rgbExponent, 1 ) );
                PUT_BYTE( rgbExponent, 1, GET_BYTE( rgbExponent, 0 ) );
                PUT_BYTE( rgbExponent, 0, 0 );

                /*
                *  Decode Modulus
                */
                cbDestination = SIZEOF(rgbModulus);
                ChkDR( DRM_B64_DecodeA( (const DRM_CHAR *) f_pbSignedBytes,
                                        rgdasstrAttrs + 1,
                                       &cbDestination,
                                        rgbModulus,
                                        0 ) );

                ChkDR( OEM_RSA_GetKeyLengthFromModulusLength_2048BIT( cbDestination, &eKeyLength ) );

                ChkDR( OEM_RSA_SetPublicKey_2048BIT( eKeyLength,
                                                     SIZEOF( rgbExponent ),
                                                     rgbExponent,
                                                     cbDestination,
                                                     rgbModulus,
                                                     &pubKey ) );
            }
        }
    }

    dr = OEM_RSA_PssVerify_2048BIT( &pubKey,
                                     f_cbSignedBytes,
                                     f_pbSignedBytes,
                                     0,
                                     f_cbSignature,
                                     f_pbSignature,
                                     f_pCryptoCtx );

    if( DRM_FAILED(dr) )
    {
        TRACE(( "DRM_RVK_VerifyCRLSignature: Signature Invalid\n" ));
        ChkDR( dr );
    }

    if( f_pcontextBBX != NULL
     && f_pcontextSST != NULL
     && f_pDatastore  != NULL )
    {
        ChkDR( DRM_BLOBCACHE_Update( f_pcontextBBX,
                                     f_pcontextSST,
                                     f_pDatastore,
                                     f_pbSignedBytes,
                                     f_cbSignedBytes,
                                     f_pbSignature,
                                     f_cbSignature ) );
    }

ErrorExit:
    OEM_RSA_ZeroPublicKey_2048BIT( &pubKey );
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_RESULT DRM_CALL DRM_XMB_RSASignAndCloseCurrNodeA(
    __inout         _XMBContextA                *f_pbContextXMLA,
    __in      const DRM_RSA_PRIVATE_KEY_2048BIT *f_pprivkeyRSA,
    __in            DRM_BOOL                     f_fIncludeTag,
    __out           DRM_DWORD                   *f_pcbSignature,
    __out_bcount    (*f_pcbSignature)  DRM_BYTE *f_pbSignature,
    __out_ecount_opt(*f_pcchSignature) DRM_CHAR *f_pszSignature,
    __inout         DRM_DWORD                   *f_pcchSignature,
    __out_opt       DRM_SUBSTRING               *f_pdasstrXMLFragment,
    __in            DRM_CRYPTO_CONTEXT          *f_pCryptCtx)
{
    DRM_RESULT    dr = DRM_SUCCESS;
    _XMBContextA *pcontextXMLA   = (_XMBContextA *) f_pbContextXMLA;
    DRM_DWORD     cchContent     = 0;
    DRM_DWORD     cchTagName     = 0;
    DRM_SUBSTRING dasstrFragment = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_RSASignAndCloseCurrNodeA );

    ChkArg(f_pbContextXMLA    != NULL
       &&  f_pprivkeyRSA      != NULL
       &&  f_pcbSignature     != NULL
       &&  f_pbSignature      != NULL
       &&  pcontextXMLA->fInited);

    ChkDR(DRM_XMB_CalcNodeSizeA(f_pbContextXMLA, &cchContent, &cchTagName));

    if (! f_fIncludeTag)
    {
        DRM_SUBSTRING dasstr = EMPTY_DRM_SUBSTRING;

        dasstr.m_ich = pcontextXMLA->ichNextString - cchContent;
        dasstr.m_cch = cchContent;


        /* sign the content */
        ChkDR(OEM_RSA_PssSign_2048BIT(f_pprivkeyRSA,
                                      cchContent,
                         (DRM_BYTE *) pcontextXMLA->rgchXML,
                                      pcontextXMLA->ichNextString - cchContent,
                                      f_pcbSignature,
                                      f_pbSignature,
                                      f_pCryptCtx));
    }

    /* close the node */
    ChkDR(DRM_XMB_CloseCurrNodeA(f_pbContextXMLA, &dasstrFragment));

    if (f_fIncludeTag)
    {
        /* sign the content */
        ChkDR(OEM_RSA_PssSign_2048BIT(f_pprivkeyRSA,
                                      dasstrFragment.m_cch,
                         (DRM_BYTE *) pcontextXMLA->rgchXML,
                                      dasstrFragment.m_ich,
                                      f_pcbSignature,
                                      f_pbSignature,
                                      f_pCryptCtx));

    }

    /* Base64 encode the signature */
    if (f_pszSignature != NULL)
    {
        ChkDR(DRM_B64_EncodeA( f_pbSignature,
                              *f_pcbSignature,
                               f_pszSignature,
                               f_pcchSignature,
                               0));
    }

    if (f_pdasstrXMLFragment != NULL)
    {
        *f_pdasstrXMLFragment = dasstrFragment;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_RESULT DRM_CALL DRM_XMB_HashAndRSASignAndCloseCurrNodeA(
    __inout                                                         _XMBContextA                 *f_pbContextXMLA,
    __in                                                            DRM_SHA1_CONTEXT             *f_pcontextSHA,
    __in                                                      const DRM_RSA_PRIVATE_KEY_2048BIT  *f_pprivkeyRSA,
    __in                                                            DRM_BOOL                      f_fIncludeTag,
    __out_bcount(DRM_SHA1_DIGEST_LEN)                               DRM_BYTE                      f_rgbSHA  [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __out_ecount(DRM_SHA1_B64ENC_DIGEST_LEN)                        DRM_CHAR                      f_rgchSHA [__CB_DECL(DRM_SHA1_B64ENC_DIGEST_LEN)],
    __out_ecount(1)                                                 DRM_DWORD                    *f_pcbSignature,
    __out_bcount(*f_pcbSignature)                                   DRM_BYTE                     *f_pbSignature,
    __out                                                           DRM_DWORD                    *f_pcchSignature,
    __out_ecount(DRM_RSA_B64ENC_PRIME_MAX)                          DRM_CHAR                      f_rgchSignature [__CB_DECL(DRM_RSA_B64ENC_PRIME_MAX)],
    __out_opt                                                       DRM_SUBSTRING                *f_pdasstrXMLFragment,
    __in                                                            DRM_CRYPTO_CONTEXT           *f_pCryptCtx )
{
    DRM_RESULT      dr = DRM_SUCCESS;
    _XMBContextA   *pcontextXMLA   = (_XMBContextA *) f_pbContextXMLA;
    DRM_SUBSTRING   dasstrFragment = { 0 };
    DRM_DWORD       cchContent     = 0;
    DRM_DWORD       cchTagName     = 0;
    const DRM_BYTE *pbContent      = 0;
    DRM_DWORD       ichStart       = 0;
    DRM_DWORD       cchSignature   = 0;
    DRM_DWORD       cchSHA         = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_HashAndRSASignAndCloseCurrNodeA );

    DRMCASSERT( DRM_SHA1_B64ENC_DIGEST_LEN == CCH_BASE64_EQUIV( DRM_SHA1_DIGEST_LEN ) );
    DRMCASSERT( DRM_RSA_B64ENC_PRIME_MAX   == CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX_2048BIT ) );

    ChkArg( f_pbContextXMLA  != NULL );
    ChkArg( f_pcontextSHA  != NULL );
    ChkArg( f_pprivkeyRSA  != NULL );
    ChkArg( f_rgbSHA  != NULL );
    ChkArg( f_pcbSignature  != NULL );
    ChkArg( f_pbSignature  != NULL );
    ChkArg( f_pcchSignature  != NULL );
    ChkArg( pcontextXMLA->fInited );

    ChkDR(DRM_XMB_CalcNodeSizeA(f_pbContextXMLA, &cchContent, &cchTagName));

    ichStart  = pcontextXMLA->ichNextString - cchContent;

    pbContent = (DRM_BYTE*)pcontextXMLA->rgchXML + ichStart;

    DRM_SHA1_Init(f_pcontextSHA);

    if (! f_fIncludeTag)
    {
        DRM_SUBSTRING dasstr = EMPTY_DRM_SUBSTRING;

        /* create the SHA digest over the content */

        DRM_SHA1_Update(pbContent, cchContent, f_pcontextSHA);

        /* RSA Sign the content */

        dasstr.m_ich = 0;
        dasstr.m_cch = cchContent;
        cchSignature = CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX_2048BIT );

        /* sign the content */
        ChkDR( OEM_RSA_PssSign_2048BIT( f_pprivkeyRSA,
                                        cchContent,
                                        pbContent,
                                        0,
                                        f_pcbSignature,
                                        f_pbSignature,
                                        f_pCryptCtx ) );

        ChkDR( DRM_B64_EncodeA(  f_pbSignature,
                                *f_pcbSignature,
                                 f_rgchSignature,
                                &cchSignature,
                                 0 ) );
        *f_pcchSignature = cchSignature;

    }

    /* close the node */
    ChkDR(DRM_XMB_CloseCurrNodeA(f_pbContextXMLA, &dasstrFragment));

    if (f_fIncludeTag)
    {
        DRM_SUBSTRING dasstr = EMPTY_DRM_SUBSTRING;

        pbContent = (DRM_BYTE*)pcontextXMLA->rgchXML + dasstrFragment.m_ich;

        /* create the SHA hash */

        DRM_SHA1_Update  (pbContent,
                          dasstrFragment.m_cch,
                          f_pcontextSHA);

        DRM_SHA1_Finalize(f_pcontextSHA,
                          f_rgbSHA);

        /* Base64 encode the digest */

        if (f_rgchSHA != NULL)
        {
            cchSHA = DRM_SHA1_B64ENC_DIGEST_LEN;

            ChkDR(DRM_B64_EncodeA(f_rgbSHA,
                                  DRM_SHA1_DIGEST_LEN,
                                  f_rgchSHA,
                                 &cchSHA,
                                  0));
        }

        /* RSA Sign the content */

        dasstr.m_ich = 0;
        dasstr.m_cch = dasstrFragment.m_cch;
        cchSignature = CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX_2048BIT );

        /* sign the content */
        ChkDR( OEM_RSA_PssSign_2048BIT(  f_pprivkeyRSA,
                                         dasstrFragment.m_cch,
                                         pbContent,
                                         0,
                                         f_pcbSignature,
                                         f_pbSignature,
                                         f_pCryptCtx ) );

        ChkDR( DRM_B64_EncodeA(  f_pbSignature,
                                *f_pcbSignature,
                                 f_rgchSignature,
                                &cchSignature,
                                 0 ) );
        *f_pcchSignature = cchSignature;

    }

    if (f_pdasstrXMLFragment != NULL)
    {
        *f_pdasstrXMLFragment = dasstrFragment;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;

