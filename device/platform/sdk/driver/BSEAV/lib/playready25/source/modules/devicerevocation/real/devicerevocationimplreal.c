/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DEVICEREVOCATIONIMPLREAL_C
#include <drmdevicerevocation.h>
#include <drmrevocationtypes.h>
#include <drmrevocation.h>
#include <drmpubkeydeviceexclusion.h>
#include <drmxmlparser.h>
#include <drmutilitieslite.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

DRM_API DRM_NO_INLINE DRM_RESULT DRM_RVK_UpdateRevocationListDecoded(
    __in                                   DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                   DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_ecount(f_cbRevocationList)        DRM_BYTE                    *f_pbRevocationList,
    __in                                   DRM_DWORD                    f_cbRevocationList,
    __in                             const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                   DRM_DWORD                    f_cbRevocationBuffer,
    __in                             const DRM_GUID                    *f_pGUID,
    __in                             const PUBKEY                      *f_ppubkey,
    __out_opt                              DRM_BOOL                    *f_pfUpdated )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  idSerialNew = 0,
               idSerialOld = 0,
               cbReturned  = 0;
    DRM_STRING dstrRevocationList = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC__UpdateRevocationList );

    ChkArg(   f_pcontextCrypto     != NULL
           && f_pContextRev        != NULL
           && f_pGUID              != NULL
           && f_pbRevocationList   != NULL
           && f_pbRevocationBuffer != NULL
           && f_cbRevocationBuffer  > 0
           && f_cbRevocationList    > 0 );

    if( f_pfUpdated != NULL )
    {
        *f_pfUpdated = FALSE;
    }

    DSTR_FROM_PB( &dstrRevocationList, f_pbRevocationList, f_cbRevocationList );

    idSerialNew = DRM_NO_PREVIOUS_CRL;
    idSerialOld = DRM_NO_PREVIOUS_CRL;


    ChkDR( DRM_RVK_VerifyRevocationList( f_pContextRev,
                                         f_pcontextCrypto,
                                         (DRM_CONST_STRING*)&dstrRevocationList,
                                         f_ppubkey,
                                         &idSerialNew ) );

    cbReturned = f_cbRevocationBuffer;
    /* get and verify the current SST image
     */
    dr = DRM_RVK_GetSSTRevocationList(
        f_pGUID,
        f_pcontextCrypto,
        f_pContextRev,
        f_ppubkey,
        f_pbRevocationBuffer,
        &cbReturned,
        &idSerialOld );

    /* This can be the first Revocation list */

    if ( dr == DRM_E_DST_SLOT_NOT_FOUND
        || ( ( idSerialOld == DRM_NO_PREVIOUS_CRL ) && DRM_SUCCEEDED( dr ) )
        || ( ( idSerialOld < idSerialNew ) && DRM_SUCCEEDED( dr ) ) )
    {
        /* Store the list as a wrapped CRL */
        ChkDR( DRM_RVS_StoreRevocationData( f_pContextRev,
                                            f_pGUID,
                                            f_pbRevocationList,
                                            f_cbRevocationList ) );
        if( f_pfUpdated != NULL )
        {
            *f_pfUpdated = TRUE;
        }
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function :   DRM_RVK_UpdateRevocationList
**
** Synopsis :   updates the revocation list in the secure store
**
** Arguments :
** [f_pcontextCrypto]      --
** [f_pcontextRev]         -- Revocation store context
** [f_pbRevocationList]    -- input buffer for exclusion list
** [f_cbRevocationList]    -- buffer size
** [f_pdstrRevocationList] -- the new DCRL
** [f_pGUID]               -- DRM_GUID of the CRL type

** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_RESULT DRM_RVK_UpdateRevocationList(
    __in                                                                                    DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                                                                    DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "f_rgchBase length defined by f_pdasstrRevocationList")
    __inout_ecount( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch )       DRM_CHAR                    *f_rgchBase,
PREFAST_POP
    __in                                                                              const DRM_SUBSTRING               *f_pdasstrRevocationList,
    __in_bcount( f_cbRevocationBuffer )                                               const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                                                                    DRM_DWORD                    f_cbRevocationBuffer,
    __in                                                                              const DRM_GUID                    *f_pGUID,
    __in                                                                              const PUBKEY                      *f_ppubkey )
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_DWORD        cbDecoded      = 0;
    const DRM_BYTE  *pbDecoded      = NULL;
    DRM_DWORD        cbAligned      = 0;
    DRM_BYTE        *pbAligned      = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateRevocationList );

    ChkArg(   f_pcontextCrypto  != NULL
           && f_pContextRev      != NULL
           && f_pGUID               != NULL
           && f_rgchBase            != NULL
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

    /* decode the XML for the new exclusion list */
    ChkDR( DRM_B64_DecodeA( f_rgchBase,
                            f_pdasstrRevocationList,
                            &cbAligned,
                            pbAligned,
                            0 ) );

    ChkDR( DRM_RVK_UpdateRevocationListDecoded(
        f_pcontextCrypto,
        f_pContextRev,
        pbAligned,
        cbAligned,
        f_pbRevocationBuffer,
        f_cbRevocationBuffer,
        f_pGUID,
        f_ppubkey,
        NULL ) );


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
** Function: DRM_RVK_GetDeviceRevocationList
**
** Synopsis: gets the current device exclusion list from the secure store
**
** Arguments:
** [f_pcontextCRYP]      -- just a buffer
** [f_pContextRev]       -- revocation store context
** [f_pbBuffer]          -- output buffer for exclusion list; NULL to request
**                          required size
** [f_pcbBuffer]         -- DRM_DWORD to hold max buffer size on in, bytes actually
**                          used on out
** [f_pidSerial]         -- receives numerical representation of <INDEX> tag
*****************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_GetDeviceRevocationList(
    __in                                           DRM_CRYPTO_CONTEXT          *f_pcontextCRYP,
    __in                                           DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __out_bcount_opt( *f_pcbRevocationData ) const DRM_BYTE                    *f_pbRevocationData,
    __inout                                        DRM_DWORD                   *f_pcbRevocationData,
    __out                                          DRM_DWORD                   *f_pidSerial )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetDeviceRevocationList );

    ChkArg(   f_pcontextCRYP        != NULL
           && f_pContextRev         != NULL
           && f_pcbRevocationData   != NULL
           && f_pidSerial           != NULL );

    dr = DRM_RVK_GetSSTRevocationList(
        &g_guidRevocationTypeDevice,
        f_pcontextCRYP,
        f_pContextRev,
        &g_pubkeyDeviceRevocation,
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

/*****************************************************************************
** Function: DRM_RVK_CheckDeviceRevocation
**
** Synopsis: compare a devcert to an exclusion entry and seek a match result
**
** Arguments:
** [f_pdstrDevCert]      -- a DEVICE_CERT struct pointing to the devcert XML
** [f_pdstrXMLExclusion] -- a DSTR pointing to an exclusion list entry with the
** [f_pfExcluded]        -- pointer to a boolean variable to hold the match
**                          result
** Returns: - DRM_SUCCESS regardless of match
**          - error code on XML or numerical translation failure
**
** Notes:
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RVK_CheckDeviceRevocation(
    __in  const DRM_CONST_STRING *f_pdstrDevCert,
    __in  const DRM_CONST_STRING *f_pdstrXMLExclusion,
    __out       DRM_BOOL          *f_pfExcluded )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(f_pfExcluded != NULL);
    ChkDRMString(f_pdstrXMLExclusion);
    ChkDRMString(f_pdstrDevCert);

    *f_pfExcluded = FALSE;

    /* get the devcert XML with the <DATA> tag at the root */
    ChkDR(DRM_XMU_MatchNodeFromTemplate(f_pdstrDevCert,
                                        f_pdstrXMLExclusion,
                                        TRUE,
                                        f_pfExcluded));

ErrorExit:
    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;

