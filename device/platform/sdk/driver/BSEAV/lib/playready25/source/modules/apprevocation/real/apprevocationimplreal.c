/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_APPREVOCATIONIMPLREAL_C
#include <drmapprevocation.h>
#include <drmrevocation.h>
#include <drmpubkeyrevocation.h>
#include <drmwmdrm.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

typedef DRM_DWORD DRM_CERT_TYPE;
#define CERTTYPE_V1APP 0

typedef struct __tagDRM_REVOCATION_LIST_ELEMENT
{
    DRM_CERT_TYPE certificatetype;
    DRM_DWORD     cbCert;
    DRM_BYTE      rgbCert [1];
}DRM_REVOCATION_LIST_ELEMENT;

typedef struct __tagDRM_REVOCATION_LIST
{
    DRM_BYTE  rgbSignature [__CB_DECL( DRM_ECC160_SIGNATURE_LEN) ];
    DRM_DWORD dwVersion;
    DRM_DWORD cb;
    DRM_DWORD idSerial;
    DRM_BYTE  rgbData [1]; /* An array of DRM_REVOCATION_LIST_ELEMENT structs which are variable length */
} DRM_REVOCATION_LIST;



/************************ APP REVOCATION ************************/
const DRM_DWORD CURRENT_REVOCATION_VERSION = 2;

static DRM_NO_INLINE DRM_RESULT _CheckCertInRevocationList(
          DRM_CERT_TYPE        f_certificatetype,
    const DRM_BYTE            *f_pbCertData,
          DRM_DWORD            f_cbCertData,
    const DRM_REVOCATION_LIST *f_prevocationlist ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CheckCertInRevocationList(
          DRM_CERT_TYPE        f_certificatetype,
    const DRM_BYTE            *f_pbCertData,
          DRM_DWORD            f_cbCertData,
    const DRM_REVOCATION_LIST *f_prevocationlist )
{
    const DRM_BYTE  *pbElt    = f_prevocationlist->rgbData;
    DRM_DWORD        cbElt    = 0;
    DRM_DWORD        ibElt    = 0;
    DRM_RESULT       dr       = DRM_SUCCESS;
    CLAW_AUTO_RANDOM_CIPHER

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC__CheckCertInRevocationList );

    cbElt = f_prevocationlist->cb;
    FIX_ENDIAN_DWORD( cbElt );

    /* The revocation list is an array of DRM_REVOCATION_LIST_ELEMENT structs, so lets check them */
    while ( ibElt < cbElt )
    {
        DRM_CERT_TYPE  ecertType = 0;
        DRM_DWORD      cbCert    = 0;

        DRM_BYT_CopyBytes( &ecertType, 0, pbElt, ibElt, SIZEOF( DRM_DWORD ) );
        FIX_ENDIAN_DWORD( ecertType );
        ibElt += SIZEOF( DRM_CERT_TYPE );

        DRM_BYT_CopyBytes( &cbCert, 0, pbElt, ibElt, SIZEOF( DRM_DWORD ) );
        FIX_ENDIAN_DWORD( cbCert );
        ibElt += SIZEOF( DRM_DWORD );

        if ((ecertType == f_certificatetype)
         && (cbCert    == f_cbCertData)
         && (DRM_BYT_CompareBytes(f_pbCertData, 0, pbElt, ibElt, f_cbCertData) == 0))
        {
            ChkDR(DRM_E_CERTIFICATE_REVOKED);
        }

        ibElt += cbCert;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*****************************************************************************
** Function - DRM_RVK_PerformAppRevocationCheck
**
** Synopsis - calls app revocation operation
**
** Arguments
** [f_pcontextLEVL] -- initialized liceval context
** [f_pcontextBBX]  -- initialized blackbox context
** [f_pcontextHDS]  -- initialized HDS context
*****************************************************************************/
/*
NOTE: this function will only work for legacy WMDRM app certs. we need to
implement similar functionality for PlayReady app certs.
*/
DRM_API DRM_RESULT DRM_CALL DRM_RVK_PerformAppRevocationCheck(
    __in                                      DRM_CRYPTO_CONTEXT          *f_pContextCrypto,
    __in                                      DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                                const DRM_CONST_STRING            *f_pdstrContentLicense,
    __in                                      APPCERT                     *f_pAppCert,
    __in_bcount( f_cbRevocationBuffer )       DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                      DRM_DWORD                    f_cbRevocationBuffer,
    __inout                                   DRM_REVLIST_ID_VALUES       *f_pRevListIDs )
{
    return DRM_WMDRM_PerformAppRevocationCheck( f_pContextCrypto, f_pContextRev, f_pdstrContentLicense, f_pAppCert, f_pbRevocationBuffer, f_cbRevocationBuffer, f_pRevListIDs );
}

/*****************************************************************************
** Function: DRM_RVK_GetCurrentAppRevocationList
**
** Synopsis: open the secure store and get the version number of the
**           app revocation list currently stored
**
** Parameters:
**
** [f_pcontextSST]    -- secure store context
** [f_rgbPasswordSST] -- password used to retrieve secure store data
** [f_pbBuffer]       -- working buffer
** [f_pcbBuffer]      -- pointer to variable to receive app revocation list size
** [f_pcontextHDS]    -- initialized HDS context
** [f_pidCRLApp]      -- pointer to variable to receive app revocation list index
*****************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_GetCurrentAppRevocationList(
    __in                            DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_bcount_opt( *f_pcbBuffer ) DRM_BYTE                    *f_pbBuffer,
    __inout                         DRM_DWORD                   *f_pcbBuffer,
    __out                           DRM_DWORD                   *f_pidCRLApp )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr    = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetCurrentAppRevocationList );

    ChkArg (f_pContextRev     != NULL
        &&  f_pidCRLApp      != NULL
        &&  f_pcbBuffer      != NULL);

    dr = DRM_RVS_GetRevocationData( f_pContextRev,
                       &g_guidRevocationTypeApp,
                        f_pbBuffer,
                        f_pcbBuffer );

    if (DRM_SUCCEEDED(dr))
    {
        ChkBOOL( f_pbBuffer != NULL,                        DRM_E_LOGICERR);
        ChkBOOL(*f_pcbBuffer > SIZEOF(DRM_REVOCATION_LIST), DRM_E_BUFFERTOOSMALL);

        *f_pidCRLApp = ((DRM_REVOCATION_LIST *) f_pbBuffer)->idSerial;

        FIX_ENDIAN_DWORD(*f_pidCRLApp);
    }
    else if (dr == DRM_E_BUFFERTOOSMALL)
    {
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }
    else if (dr == DRM_E_FILENOTFOUND
          || dr == DRM_E_DST_SLOT_NOT_FOUND)
    {
        dr          = DRM_SUCCESS;
       *f_pidCRLApp = DRM_APP_REVOCATION_VERSION_NONE;
       *f_pcbBuffer = 0;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*****************************************************************************
** Function DRM_RVK_CheckAppCertForRevocation
**
** f_pbAppCRL -- DRM_REVOCATION_LIST passed in as byte buffer
** f_cbAppCRL -- size of the App CRL in bytes
** f_pappcert -- app certificate to check against what is in the store.
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RVK_CheckAppCertForRevocation(
    __in_bcount( f_cbAppCRL ) DRM_BYTE  *f_pbAppCRL,
    __in                      DRM_DWORD  f_cbAppCRL,
    __in                      APPCERT   *f_pappcert )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(f_pbAppCRL    != NULL
        && f_pappcert    != NULL
        && f_cbAppCRL     > 0);

    ChkDR(_CheckCertInRevocationList(CERTTYPE_V1APP,
                       (DRM_BYTE *) &f_pappcert->appcd.pk.pk,
                             SIZEOF (f_pappcert->appcd.pk.pk),
            (DRM_REVOCATION_LIST *)  f_pbAppCRL));

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: DRM_RVK_UpdateAppRevocationListA
**
** Synopsis: Expanded version of DRM_RVK_PerformAppRevocationCheck.  Allows an
**           app to query for app revocation without the lic eval context.
**
** Parameters:
**
** [pcontextCRYP]         -- Crypto buffer
** [f_pContextRev]        -- revocation store context
** [f_pbBuffer]           -- working buffer
** [f_cbBuffer]           -- size of f_pbBuffer in bytes
** [f_pdstrRevocationList]-- Revocation list string that will be updated in the store
** [f_pappcert]           -- app certificate to check against what is in the store.
** [f_idCRLAppCurrent]    -- version number/index of the currently stored App CRL
** [f_pfUpdatedAppRevocationList] -- optional pointer to a bool variable to be set TRUE
**                           if the stored App CRL was updated.  PC-side callers
**                           should update cached global App CRL
 ****************************************************************************/
/*
Note: this function only works for lgegacy wmdrm app certs and will only support
pre-playready types of crls and rev info.
*/

DRM_API DRM_RESULT DRM_CALL DRM_RVK_UpdateAppRevocationListA(
    __in                                                                                 DRM_CRYPTO_CONTEXT          *f_pcontextCRY,
    __in                                                                                 DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_bcount( f_cbBuffer )                                                            DRM_BYTE                    *f_pbBuffer,
    __in                                                                                 DRM_DWORD                    f_cbBuffer,
    __in_ecount( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch ) const DRM_CHAR                    *f_rgchBase,
    __in                                                                           const DRM_SUBSTRING               *f_pdasstrRevocationList,
    __in                                                                                 APPCERT                     *f_pappcert,
    __in                                                                                 DRM_DWORD                    f_idCRLAppCurrent,
    __out_opt                                                                            DRM_BOOL                    *f_pfUpdatedAppRevocationList )
{
    DRM_RESULT            dr              = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateAppRevocationListA );

    /* Note: f_pappcert is optional */
    ChkArg(f_pcontextCRY != NULL
        && f_pContextRev  != NULL
        && f_cbBuffer     > 0
        && f_rgchBase     != NULL
        && f_pdasstrRevocationList != NULL );


    if (f_pfUpdatedAppRevocationList != NULL)
    {
       *f_pfUpdatedAppRevocationList = FALSE;
    }


    /* If they don't pass us a buffer, it's a signal to just decode in place */
    if( f_pbBuffer == NULL )
    {

        /* pbOffset will point within the base64 revocation list we were passed,
           such that after base64 decoding the end of the decoded data will co-incide with
           the end of the buffer we were given, minus one byte.

           This is because we are giving the B64 decoder overlapping memory for source and desitination,
           and we can't use the DECODE_IN_PLACE flag because this is ANSI where the offset may not coincide
           with a word boundary as required on 16-bit platforms.
         */

        f_pbBuffer = (DRM_BYTE*)f_rgchBase
                            + __CB_DECL( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch - CB_BASE64_DECODE( f_pdasstrRevocationList->m_cch ) - 1 )
                            + ( ( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch - CB_BASE64_DECODE( f_pdasstrRevocationList->m_cch ) ) % CB_NATIVE_BYTE );

        f_cbBuffer = CB_BASE64_DECODE( f_pdasstrRevocationList->m_cch ) + 1 - ( ( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch - CB_BASE64_DECODE( f_pdasstrRevocationList->m_cch ) ) % CB_NATIVE_BYTE );

        ChkDR( DRM_B64_DecodeA( f_rgchBase,
                    f_pdasstrRevocationList,
                   &f_cbBuffer,
                    f_pbBuffer,
                    0 ) );

        /*
        ** If we're not aligned, we have slack space behind the pointer due to the base 64 decode
        */
        if( (DRM_DWORD_PTR)f_pbBuffer % SIZEOF( DRM_DWORD ) != 0 )
        {
            DRM_BYTE *pbAlign = NULL;

            pbAlign = f_pbBuffer - ((DRM_DWORD_PTR)f_pbBuffer % SIZEOF( DRM_DWORD ));

            /* Ensure that the slack space does exist */
            ChkBOOL( f_cbBuffer - CB_BASE64_DECODE( f_cbBuffer ) >= ((DRM_DWORD_PTR)f_pbBuffer % SIZEOF( DRM_DWORD )),
                    DRM_E_BUFFERTOOSMALL );

            MEMMOVE( pbAlign, f_pbBuffer, f_cbBuffer );
            f_pbBuffer = pbAlign;
        }
    }
    else
    {
        ChkDR( DRM_B64_DecodeA( f_rgchBase,
                    f_pdasstrRevocationList,
                   &f_cbBuffer,
                    f_pbBuffer,
                    0 ) );
    }

    ChkDR( DRM_RVK_UpdateAppRevocationListDecoded( f_pcontextCRY,
                f_pContextRev,
                f_pbBuffer,
                f_cbBuffer,
                f_pappcert,
                f_idCRLAppCurrent,
                f_pfUpdatedAppRevocationList,
                NULL ) );


ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_UpdateAppRevocationListW(
    __in                      DRM_CRYPTO_CONTEXT          *f_pcontextCRY,
    __in                      DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_bcount( f_cbBuffer ) DRM_BYTE                    *f_pbBuffer,
    __in                      DRM_DWORD                    f_cbBuffer,
    __in                      DRM_CONST_STRING            *f_pdstrRevocationList,
    __in_opt                  APPCERT                     *f_pappcert,
    __in                      DRM_DWORD                    f_idCRLAppCurrent,
    __out                     DRM_BOOL                    *f_pfUpdatedAppRevocationList,
    __out_opt                 DRM_DWORD                   *f_pdwVersion )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT            dr              = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateAppRevocationListW );

    /* Note: f_pappcert is optional */
    ChkArg(f_pcontextCRY             != NULL
        && f_pContextRev             != NULL
        && f_cbBuffer                > 0
        && f_pdstrRevocationList     != NULL );


    if (f_pfUpdatedAppRevocationList != NULL)
    {
       *f_pfUpdatedAppRevocationList = FALSE;
    }
    if (f_pdwVersion != NULL)
    {
        *f_pdwVersion = 0;
    }

    if( f_pbBuffer == NULL )
    {
        /* If they don't pass us a buffer, it's a signal to just decode in place */

        ChkDR( DRM_B64_DecodeW(f_pdstrRevocationList,
                               &f_cbBuffer,
                               NULL,
                               DRM_BASE64_DECODE_IN_PLACE ) );

        f_pbBuffer = (DRM_BYTE*)f_pdstrRevocationList->pwszString;
    }
    else
    {
        ChkDR( DRM_B64_DecodeW(f_pdstrRevocationList,
                               &f_cbBuffer,
                               f_pbBuffer,
                               0 ) );
    }

    ChkDR( DRM_RVK_UpdateAppRevocationListDecoded( f_pcontextCRY,
                                                   f_pContextRev,
                                                   f_pbBuffer,
                                                   f_cbBuffer,
                                                   f_pappcert,
                                                   f_idCRLAppCurrent,
                                                   f_pfUpdatedAppRevocationList,
                                                   f_pdwVersion) );


ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_RESULT DRM_NO_INLINE DRM_CALL DRM_RVK_UpdateAppRevocationListDecoded(
    __in                              DRM_CRYPTO_CONTEXT            *f_pcontextCRY,
    __in                              DRM_REVOCATIONSTORE_CONTEXT   *f_pContextRev,
    __in_bcount( f_cbRevocationList ) DRM_BYTE                      *f_pbRevocationList,
    __in                              DRM_DWORD                      f_cbRevocationList,
    __in_opt                          APPCERT                       *f_pappcert,
    __in                              DRM_DWORD                      f_idCRLAppCurrent,
    __out_opt                         DRM_BOOL                      *f_pfUpdatedAppRevocationList,
    __out_opt                         DRM_DWORD                     *f_pdwVersion )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT            dr              = DRM_SUCCESS;
    DRM_DWORD             idCRLAppNew     = 0;
    DRM_REVOCATION_LIST  *prevocationlist = NULL;
    DRM_DWORD             cbSigned         = 0;
    DRM_DWORD             dwVersion        = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateAppRevocationListDecoded );

    /* Note: f_pappcert is optional */
    ChkArg(f_pcontextCRY != NULL
        && f_pContextRev != NULL
        && f_pbRevocationList     != NULL
        && f_cbRevocationList > 0 );

    DRMASSERT( (DRM_DWORD_PTR)f_pbRevocationList % SIZEOF( DRM_DWORD ) == 0 );

    if (f_pfUpdatedAppRevocationList != NULL)
    {
       *f_pfUpdatedAppRevocationList = FALSE;
    }
    if (f_pdwVersion != NULL)
    {
        *f_pdwVersion = 0;
    }

    prevocationlist = (DRM_REVOCATION_LIST *)  f_pbRevocationList;
    dwVersion       = prevocationlist->dwVersion;

    FIX_ENDIAN_DWORD( dwVersion );

    if (dwVersion != CURRENT_REVOCATION_VERSION)
    {
        ChkDR( DRM_E_INVALID_REVOCATION_LIST );
    }

    idCRLAppNew = prevocationlist->idSerial;
    FIX_ENDIAN_DWORD( idCRLAppNew );

    if (f_idCRLAppCurrent == DRM_APP_REVOCATION_VERSION_NONE
    ||  f_idCRLAppCurrent  < idCRLAppNew)
    {
        /* Verify & store the new list. */

        cbSigned = prevocationlist->cb;
        FIX_ENDIAN_DWORD(cbSigned);

        cbSigned += ( 3 * SIZEOF( DRM_DWORD ) );

        if ( DRM_SUCCEEDED( DRM_PK_Verify( f_pcontextCRY,
                                           eDRM_ECC_P160,
                                           DRM_ECC160_PUBLIC_KEY_LEN,
                                           ( const DRM_BYTE * )&g_pubkeyRevocation,
                                           cbSigned,
                                           ( const DRM_BYTE * )&prevocationlist->dwVersion,
                                           DRM_ECC160_SIGNATURE_LEN,
                                           prevocationlist->rgbSignature ) ) )
        {
            /* The new list is verified.  Store it. */

            ChkDR( DRM_RVS_StoreRevocationData( f_pContextRev,
                                                &g_guidRevocationTypeApp,
                                                f_pbRevocationList,
                                                f_cbRevocationList ) );

            if (f_pfUpdatedAppRevocationList != NULL)
            {
                *f_pfUpdatedAppRevocationList = TRUE;
            }
            if (f_pdwVersion)
            {
                *f_pdwVersion = idCRLAppNew;
            }
        }

        if( f_pappcert != NULL )
        {
            ChkDR(_CheckCertInRevocationList(CERTTYPE_V1APP,
                                             (DRM_BYTE *) &f_pappcert->appcd.pk.pk,
                                             SIZEOF (f_pappcert->appcd.pk.pk),
                                             prevocationlist));
        }
    }
    else
    {
        /* CRL did not get updated, so set it to the old version. */
        if (f_pdwVersion)
        {
            *f_pdwVersion = f_idCRLAppCurrent;
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;

}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;

