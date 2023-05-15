/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMMETERIMPV1_C
#include <drmmeterimp.h>
#include <drmxmlparser.h>
#include <drmmeterconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/* minimum characters required for a KID expressed as XML*/
/* <KID value="16byteasBase64*filler=="><ACTION type="play">0</ACTION></KID> */

#define CCH_KID_REQUIRED_MINIMUM         75

/* minimum size of an operation entry e.g. "Play" in the metering store
   |2|"p"|2|4| */

#define CB_OPERATION_MINIMUM  (SIZEOF (DRM_WORD) * 2  \
                             + SIZEOF (DRM_WCHAR)     \
                             + SIZEOF (DRM_DWORD))

/* pseudo-hash to read/write TIDs in the metering store */

const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_KID g_kidTID =
{
    'K', 0,
    'I', 0,
    'D', 0,
    '4', 0,
    'T', 0,
    'I', 0,
    'D', 0,
    '\0', 0
};
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_KID g_kidTIDPrev =
{
    'K', 0,
    'I', 0,
    'D', 0,
    'P', 0,
    'R', 0,
    'E', 0,
    'V', 0,
    '\0', 0
};

static DRM_DWORD _RequiredCharsForBinary (const DRM_CONST_STRING *pdstrTag,
                                          DRM_WORD                cbBinary)
{
    return DRM_XMB_RequiredCharsForTag( pdstrTag->cchString,
                                        0,
                                        g_dstrAttributeType.cchString,
                                        (DRM_DWORD)CCH_BASE64_EQUIV(cbBinary) );
}

/*******************************************************************
 *              PRIVATE FUNCTION _RequiredCharsForKID
 *
 * purpose: determine the number of XML characters required to
 *          express this KID
 *  params: f_pbBuffer is the DST storage image of the KID, a
 *          heterogeneous array of actions and counts
 ******************************************************************/

static DRM_RESULT _RequiredCharsForKID(
    __in_ecount(f_cbBuffer) DRM_BYTE  *f_pbBuffer,
    __in                    DRM_DWORD  f_cbBuffer,
    __out_ecount(1)         DRM_DWORD *f_pcchRequired )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_DWORD        cchKID     = 0;
    DRM_DWORD        ibCurr     = 0;
    DRM_CONST_STRING dstrOper   = EMPTY_DRM_STRING,
                     dstrCount  = EMPTY_DRM_STRING;

    ChkArg(f_pbBuffer     != NULL);
    ChkArg(f_pcchRequired != NULL);

    /*          <KID> */

    cchKID += _RequiredCharsForBinary (&g_dstrTagKID, SIZEOF (DRM_KID));

    /* read action/count entries */

    while ((ibCurr + CB_OPERATION_MINIMUM - 1) < f_cbBuffer)
    {
        DRM_WORD  cbCurr = 0;
        DRM_WCHAR awchNum [16];
        TOKEN     token  = { 0 };
        DRM_BYTE *pbAction = NULL;

        /* string size */

        DRM_BYT_CopyBytes(&cbCurr, 0, f_pbBuffer, ibCurr, SIZEOF (DRM_WORD));

        FIX_ENDIAN_WORD( cbCurr );
        ibCurr += SIZEOF (DRM_WORD);

        /* string "Play" or whatever */

        if ((ibCurr % CB_NATIVE_BYTE) != 0)
        {
            /* if the UNICODE string isn't aligned in the packed buffer then move the string one
               char to the left (this is safe, the data won't be reparsed) */

            pbAction = (f_pbBuffer + __CB_DECL((ibCurr & ~CB_NATIVE_BYTE)));

            DRM_BYT_CopyBytes(pbAction, 0, f_pbBuffer, ibCurr, cbCurr);
        }
        else
        {
            pbAction = f_pbBuffer + __CB_DECL((ibCurr));
        }

        DSTR_FROM_PB(&dstrOper, pbAction, cbCurr);

        ibCurr += cbCurr;

        /* Token */

        DRM_BYT_CopyBytes( &token.TokenType, 0, f_pbBuffer, ibCurr, SIZEOF( DRM_DWORD ) );

        FIX_ENDIAN_DWORD( token.TokenType );

        ChkBOOL( token.TokenType == TOKEN_LONG, DRM_E_WRONG_TOKEN_TYPE );

        DRM_BYT_CopyBytes( &token.val.lValue, 0, f_pbBuffer, ibCurr + SIZEOF( DRM_DWORD ), SIZEOF( DRM_LONG ) );

        FIX_ENDIAN_LONG( token.val.lValue );

        ibCurr += SIZEOF( PERSISTEDTOKEN );

        /*              <ACTION type="Play">1</ACTION> */

        ChkDR( DRM_UTL_NumberToString( (DRM_DWORD)token.val.lValue, awchNum, NO_OF(awchNum), 0, 10, &dstrCount.cchString ) );
        dstrCount.pwszString = awchNum;

        cchKID += DRM_XMB_RequiredCharsForTag (g_dstrTagAction .cchString,
                                               dstrCount       .cchString,
                                               g_dstrLabelValue.cchString,
                                               dstrOper        .cchString);
    }

    *f_pcchRequired = cchKID + 1;

ErrorExit:
    return dr;
}

/*********************************************************************
 *
 * initial state: context is in a chosen KID
 *      function: determines required buffer size for KID data
 *                allocates buffer
 *                copies data into buffer
 *                caller must release buffer
 ********************************************************************/

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_INVALID_PARAM_VALUE_1, "*ppbBuffer should not yield this warning given __checkReturn and declaration of DRM_RESULT, but it still does." )
__checkReturn DRM_RESULT DRM_NO_INLINE DRM_MTR_GetKIDData (
    __inout                           DRM_METERING_CONTEXT         *f_pmeterstore,
    __inout                           DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack,
    __deref_out_ecount(*f_pcbDataOut) DRM_BYTE                    **f_ppbDataOut,
    __out                             DRM_DWORD                    *f_pcbDataOut )
PREFAST_POP /* __WARNING_INVALID_PARAM_VALUE_1 */
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_BYTE   *pbData = NULL;
    DRM_DWORD   cbData = 0;

    DRMASSERT (f_pmeterstore != NULL);
    DRMASSERT (f_pstack      != NULL);
    DRMASSERT (f_ppbDataOut  != NULL);
    DRMASSERT (f_pcbDataOut  != NULL);

    ChkArg(f_pmeterstore != NULL
        && f_pstack      != NULL
        && f_ppbDataOut  != NULL
        && f_pcbDataOut  != NULL);

    *f_ppbDataOut = NULL;
    *f_pcbDataOut = 0;

    dr = DRM_MTR_GetAllData (f_pmeterstore, NULL, &cbData);

    if (DRM_SUCCEEDED (dr)
    ||  dr == DRM_E_BUFFERTOOSMALL)
    {
        if (cbData > 0)
        {
            if (DRM_SUCCEEDED (dr = DRM_STK_Alloc (f_pstack, cbData, (DRM_VOID**) &pbData)))
            {
                if (DRM_SUCCEEDED (dr = DRM_MTR_GetAllData (f_pmeterstore, pbData, &cbData)))
                {
                    *f_ppbDataOut = pbData;
                    *f_pcbDataOut = cbData;
                }
                else
                {
                    DRM_STK_Free (f_pstack, pbData);
                }
            }
        }
        else
        {
            dr = DRM_SUCCESS;
            *f_ppbDataOut = NULL;
            *f_pcbDataOut = 0;
        }
    }

ErrorExit:
    return dr;
} /* DRM_MTR_GetKIDData */


/*******************************************************************
 *
 * purpose: create a new TID-sized blob of random bytes
 *******************************************************************/

DRM_API DRM_RESULT DRM_NO_INLINE DRM_MTR_CreateNewTID(
    __in_opt DRM_VOID *f_pOEMContext,
    __out    DRM_TID  *f_ptid )
{
    return Oem_Random_GetBytes (f_pOEMContext, (DRM_BYTE *)f_ptid, SIZEOF(DRM_TID));
}

/*******************************************************************
 *
 * purpose: manage the reading and writing of TIDs
 *******************************************************************/

DRM_API DRM_RESULT DRM_NO_INLINE DRM_MTR_ReadTIDFromMeteringStore(
    __in            DRM_METERING_CONTEXT   *f_pmeterstore,
    __in      const DRM_BYTE                f_rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in      const DRM_MID                *f_pmid,
    __out           DRM_TID                *f_ptid,
    __out_opt       DRM_CONST_STRING       *f_pdstrTidBase64,
    __in            DRM_DST                *f_pDatastore )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_BYTE   rgbSlotMinimum [__CB_DECL(DRM_SST_SLOT_V0_HEADER_SIZE)];
    DRM_DWORD  cbSlot = SIZEOF (rgbSlotMinimum);

    ChkArg (f_pmeterstore    != NULL
        &&  f_rgbPasswordSST != NULL
        &&  f_pmid           != NULL
        &&  f_ptid           != NULL
        &&  f_pDatastore    != NULL);

    ChkDRMap( DRM_SST_GetData (f_pmeterstore,
                           f_pmid,
                          &g_kidTID,
                           f_rgbPasswordSST,
                           SECURE_STORE_METERING_DATA,
                           f_pDatastore,
                           rgbSlotMinimum,
                          &cbSlot), DRM_E_DST_SLOT_NOT_FOUND, DRM_E_METERSTORE_DATA_NOT_FOUND );

    MEMCPY((DRM_BYTE *) f_ptid, rgbSlotMinimum, SIZEOF(DRM_TID));

    if (f_pdstrTidBase64 != NULL)
    {
        ChkArg(f_pdstrTidBase64->cchString >= CCH_BASE64_EQUIV (SIZEOF (DRM_TID)));

        ChkDR  (DRM_B64_EncodeW((DRM_BYTE *) f_ptid,
                          SIZEOF(DRM_TID),
                    (DRM_WCHAR *) f_pdstrTidBase64->pwszString,
                                 &f_pdstrTidBase64->cchString,
                                 0));
    }

ErrorExit:
    return dr;
} /* DRM_MTR_ReadTIDFromMeteringStore */

DRM_RESULT DRM_NO_INLINE DRM_MTR_WriteTIDToMeteringStore (
    __inout     DRM_METERING_CONTEXT *f_pmeterstore,
    __in  const DRM_BYTE              f_rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in  const DRM_MID              *f_pmid,
    __in  const DRM_TID              *f_ptid,
    __in        DRM_DST              *f_pDatastore )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbSlotMinimum [__CB_DECL(DRM_SST_SLOT_V0_HEADER_SIZE)];

    /* must write at least 24 bytes or SST slot code will fail */

    ZEROMEM(rgbSlotMinimum, SIZEOF(rgbSlotMinimum));
    MEMCPY (rgbSlotMinimum, f_ptid, SIZEOF(DRM_TID));

    ChkDR(DRM_SST_SetData(f_pmeterstore,
                          f_pmid,
                         &g_kidTID,
                          f_rgbPasswordSST,
                          SECURE_STORE_METERING_DATA,
                          f_pDatastore,
                          rgbSlotMinimum,
                   SIZEOF(rgbSlotMinimum)));
ErrorExit:
    return dr;
} /* DRM_MTR_WriteTIDToMeteringStore */

/*******************************************************************
 *              PRIVATE FUNCTION _VerifySignature
 *
 * purpose: given the <DATA> block, the base64-encoded signature and
 *          certificate (singular at the time of this writing),
 *          verify that the certificate and signed block match the
 *          public key
 *******************************************************************/

static DRM_RESULT _VerifySignature (const DRM_CONST_STRING    *f_pdstrListDataIn,
                                    const DRM_CONST_STRING    *f_pdstrDataSignIn,
                                    const DRM_CONST_STRING    *f_pdstrDataCertificateIn,
                                          DRM_LICEVAL_CONTEXT *f_pcontextLEVL)
{
    CERT            cert;
    DRM_BYTE        rgbSignature [__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] = { 0 };
    const DRM_BYTE *pbData      = PB_DSTR(f_pdstrListDataIn);
    DRM_DWORD       cbData      = CB_DSTR(f_pdstrListDataIn);
    DRM_DWORD       cbSignature = SIZEOF (rgbSignature);
    DRM_DWORD       cbCert      = SIZEOF (cert);
    DRM_RESULT      dr          = DRM_SUCCESS;

    /* decode the base64-encoded signature and certificate */

    ChkDR(DRM_B64_DecodeW(f_pdstrDataSignIn,        &cbSignature, rgbSignature,       0));
    ChkDR(DRM_B64_DecodeW(f_pdstrDataCertificateIn, &cbCert,      (DRM_BYTE *) &cert, 0));

    /* verify the certificate chain */
    ChkDR(DRM_UTL_CheckCert(&cert,
                            &g_pubkeyRootMeteringCert,
                             FALSE,
                             f_pcontextLEVL ));

    /* verify the signature */

    ChkBOOL( DRM_SUCCEEDED( DRM_PK_Verify( &f_pcontextLEVL->pcontextBBX->CryptoContext,
                                           eDRM_ECC_P160,
                                           DRM_ECC160_PUBLIC_KEY_LEN,
                                           ( const DRM_BYTE * )&cert.cd.pk,
                                           cbData,
                                           pbData,
                                           DRM_ECC160_SIGNATURE_LEN,
                                           rgbSignature ) ),
             DRM_E_INVALID_METERCERT_SIGNATURE );

ErrorExit:
    return dr;
} /* _VerifySignature */

/*****************************************************************************
** Function: DRM_MTR_CreateMeterStorePassword
**
** Synopsis: use the MID to create the hash used as the secure store password
**           for this MID
**
** Arguments:
**           [f_pdstrMID]       -- the MID as base64
**           [f_rgbPasswordSST] -- buffer to receive the password
**           [f_pcontextBBX]    -- initialized BBX context
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_CreateMeterStorePassword(
    __in                                             const DRM_CONST_STRING *f_pdstrMID,
    __out_bcount( DRM_SHA1_DIGEST_LEN )                    DRM_BYTE          f_rgbPasswordSST[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    __inout_ecount( 1 )                                    DRM_BB_CONTEXT   *f_pcontextBBX )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_MID    mid = { 0 };
    DRM_DWORD  cbMid = SIZEOF( mid );

    ChkArg( NULL != f_pdstrMID );
    ChkArg( NULL != f_pcontextBBX );

    /* convert possible non-standard base 64 encoding of MID to binary MID */

    ChkDR( DRM_B64_DecodeW( f_pdstrMID,
                            &cbMid,
                            ( DRM_BYTE* ) &mid,
                            0 ) );

    ChkDR( DRM_MTR_CreateMeterStorePasswordFromBinaryMID( &mid,
                                                          f_rgbPasswordSST,
                                                          f_pcontextBBX ) );

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function: DRM_MTR_CreateMeterStorePasswordFromBinaryMID
**
** Synopsis: use the MID to create the hash used as the secure store password
**           for this MID
**
** Arguments:
**           [f_pmid]           -- the MID in binary form
**           [f_rgbPasswordSST] -- buffer to receive the password
**           [f_pcontextBBX]    -- initialized BBX context
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_CreateMeterStorePasswordFromBinaryMID(
    __in                                const DRM_MID        *f_pmid,
    __out_bcount( DRM_SHA1_DIGEST_LEN )       DRM_BYTE        f_rgbPasswordSST[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    __inout_ecount( 1 )                       DRM_BB_CONTEXT *f_pcontextBBX )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR  rgwchMeterID [CCH_BASE64_EQUIV(SIZEOF(DRM_MID))] = { 0 };
    DRM_DWORD  cchMeterID = NO_OF( rgwchMeterID );

    ChkArg( NULL != f_pmid );
    ChkArg( NULL != f_pcontextBBX );

    /* convert binary MID to standard base 64 encoding */

    ChkDR( DRM_B64_EncodeW( ( DRM_BYTE * ) f_pmid,
                            SIZEOF( *f_pmid ),
                            rgwchMeterID,
                            &cchMeterID,
                            0) );

    /* hash the standard base 64 encoding to produce the password */

    ChkDR( DRM_BBX_HashValue( ( DRM_BYTE * ) rgwchMeterID,
                              cchMeterID * SIZEOF( DRM_WCHAR ),
                              f_rgbPasswordSST,
                              f_pcontextBBX ) );

ErrorExit:

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_MTR_OpenId(
    __inout                                  DRM_METERING_CONTEXT *f_pcontextMTR,
    __in                               const DRM_MID              *f_pmid,
    __in                               const DRM_KID              *f_pkid,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE              f_rgbPasswordMTR[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    __in                                     DRM_DST              *f_pDatastore )
{
    DRM_RESULT       dr    = DRM_E_LOGICERR;

    ChkArg (f_pcontextMTR    != NULL
         && f_pmid           != NULL
         && f_pkid           != NULL
         && f_rgbPasswordMTR != NULL
         && f_pDatastore    != NULL);

    DRMCASSERT(SIZEOF(DRM_METERING_CONTEXT) == SIZEOF(DRM_SECSTORE_CONTEXT));

    ChkDR (DRM_SST_OpenKeyTokens(f_pcontextMTR,
                      (DRM_ID *) f_pkid,
                      (DRM_ID *) f_pmid,
                                 f_rgbPasswordMTR,
                                 DRM_SECURE_STORE_OPEN_ALWAYS,
                                 SECURE_STORE_METERING_DATA,
                                 f_pDatastore));


    /* Metering context is setup successfully */
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_IncrementCount(
    __inout       DRM_METERING_CONTEXT *f_pcontextMTR,
    __in    const DRM_CONST_STRING     *f_pdstrAction )
{
    DRM_RESULT dr = DRM_E_LOGICERR;
    TOKEN      token;

    ChkArg(f_pcontextMTR);
    ChkDRMString(f_pdstrAction);

    /* Store entry has been opened/created.  Try to read the KID.  If it doesn't exist add it. */
    dr = DRM_SST_GetTokenValue(f_pcontextMTR, f_pdstrAction , &token);
    if (DRM_FAILED(dr))
    {
        dr = DRM_E_METERING_STORE_CORRUPT;
        goto ErrorExit;
    }

    if (dr == DRM_S_FALSE)
    {
        /* Action doesn't exist.  Initialize the count to 0*/
        token.val.lValue = 0;
        token.TokenType  = TOKEN_LONG;
    }

    /* Increment it and write it back */
    token.val.lValue++;
    ChkDR(DRM_SST_SetTokenValue(f_pcontextMTR, f_pdstrAction , &token));
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_GetAllData(
    __inout                        DRM_METERING_CONTEXT *f_pcontextMTR,
    __out_bcount_opt( *f_pcbData ) DRM_BYTE             *f_pbData,
    __out                          DRM_DWORD            *f_pcbData )
{
    return DRM_SST_GetAllData( f_pcontextMTR, f_pbData, f_pcbData );
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_CloseContext(
    __inout DRM_METERING_CONTEXT *f_pcontextMTR,
    __in    DRM_DST              *f_pDatastore)
{
    return DRM_SST_CloseKey( f_pcontextMTR, f_pDatastore, NULL, NULL );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_InitEnum(
    __in  const DRM_MID                   *f_pmid,
    __out       DRM_METERINGENUM_CONTEXT  *f_pcontextMTREnum,
    __in        DRM_DST                   *f_pDatastore )
{
    MEMCPY( &f_pcontextMTREnum->oMID, f_pmid, SIZEOF( DRM_ID ) );
    return DRM_SST_OpenEnumerator ( SECURE_STORE_METERING_DATA, NULL, &f_pcontextMTREnum->oSecStoreEnum, f_pDatastore, TRUE);
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_EnumNext(
    __inout  DRM_METERINGENUM_CONTEXT *f_pcontextMTREnum,
    __out    DRM_KID                  *f_pkid,
    __out    DRM_DWORD                *f_pcbData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_MID oMID = {0};

    do
    {
        dr = DRM_SST_EnumNext( &f_pcontextMTREnum->oSecStoreEnum, f_pkid, &oMID, f_pcbData );
    } while( DRM_SUCCEEDED( dr ) && MEMCMP( &oMID, &f_pcontextMTREnum->oMID, SIZEOF( DRM_MID ) ) != 0 );

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_EnumLoadCurrent(
    __in                                     DRM_METERINGENUM_CONTEXT *f_pcontextMTREnum,
    __in                                     DRM_METERING_CONTEXT     *f_pcontextMTR,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                  f_rgbPasswordMTR[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    __out                                    DRM_KID                  *f_pkid,
    __out                                    DRM_DWORD                *f_pcbData )
{
    return DRM_SST_EnumLoadCurrent(&f_pcontextMTREnum->oSecStoreEnum,
                                   f_pcontextMTR,
                                   f_rgbPasswordMTR,
                                   f_pkid,
                                   NULL,
                                   f_pcbData);
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_EnumDeleteCurrent(
    __in DRM_METERINGENUM_CONTEXT *f_pcontextMTREnum,
    __in DRM_METERING_CONTEXT     *f_pcontextMTR )
{
    return DRM_SST_EnumDeleteCurrent(&f_pcontextMTREnum->oSecStoreEnum,
                                     f_pcontextMTR);
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_UpdateData(
    __in                    const DRM_MID                 *f_pmid,
    __in                    const DRM_LID                 *f_plid,
    __in                    const DRM_KID                 *f_pkid,
    __in_ecount(f_cActions) const DRM_CONST_STRING       **f_ppdstrActions,
    __in                          DRM_DWORD                f_cActions,
    __in                          DRM_METERING_CONTEXT    *f_pcontextMTR,
    __in                          DRM_DST                 *f_pDatastore,
    __in                          DRM_BB_CONTEXT          *f_pbcontextBBX )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  iAction    = 0;
    DRM_BYTE   rgbPassword  [__CB_DECL(DRM_SHA1_DIGEST_LEN)];

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_METERING, PERF_FUNC_DRM_MTR_UpdateData);

    ChkDR( DRM_MTR_CreateMeterStorePasswordFromBinaryMID( f_pmid,
                                                          rgbPassword,
                                                          f_pbcontextBBX ) );

    ChkDR( DRM_MTR_OpenId(f_pcontextMTR,
                          f_pmid,
                          f_pkid,
                          rgbPassword,
                          f_pDatastore));

    for (iAction = 0; iAction < f_cActions; iAction++)
    {
        if(DRM_UTL_DSTRStringsEqual( f_ppdstrActions [iAction], &g_dstrWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE ) == FALSE )
        {
            ChkDR(DRM_MTR_IncrementCount(f_pcontextMTR,
                                         f_ppdstrActions [iAction]));
        }
    }

    ChkDR(DRM_MTR_CloseContext(f_pcontextMTR,
                               f_pDatastore));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_ParseMeterCert(
    __in      const DRM_CONST_STRING    *f_pdstrMeterCert,
    __in            DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __out_opt       PUBKEY              *f_ppubkeyMAS,
    __out_opt       DRM_CONST_STRING    *f_pdstrDataMID,
    __out_opt       DRM_MID             *f_pmid,
    __out_opt       DRM_CONST_STRING    *f_pdstrDataURL )
{
    DRM_RESULT       dr                  = DRM_SUCCESS;
    DRM_CONST_STRING dstrListTop         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListData        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListSignature   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListCertChain   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNodeData        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataPublicKey   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataSignature   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataCertificate = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMID              = EMPTY_DRM_STRING;
    DRM_DWORD        cbMID               = SIZEOF(DRM_MID);
    DRM_DWORD        cbPublicKey         = SIZEOF(PUBKEY);

    /* some processors require WORD-alignment; FAIL if this string is unaligned */
    ChkArg(((DRM_DWORD_PTR) f_pdstrMeterCert->pwszString) % SIZEOF (DRM_WCHAR) == 0);

    ChkDR(DRM_XML_GetNode(f_pdstrMeterCert,   &g_dstrTagMeterCert,        NULL, NULL, 0, NULL,          &dstrListTop));
    ChkDR(DRM_XML_GetNode(&dstrListTop,       &g_dstrTagData,             NULL, NULL, 0, &dstrNodeData, &dstrListData));
    ChkDR(DRM_XML_GetNode(&dstrListTop,       &g_dstrTagSignature,        NULL, NULL, 0, NULL,          &dstrListSignature));
    ChkDR(DRM_XML_GetNode(&dstrListTop,       &g_dstrTagCertificateChain, NULL, NULL, 0, NULL,          &dstrListCertChain));
    ChkDR(DRM_XML_GetNode(&dstrListData,      &g_dstrTagMID,              NULL, NULL, 0, NULL,          &dstrMID));
    if( f_pdstrDataURL != NULL )
    {
        ChkDR(DRM_XML_GetNode(&dstrListData,      &g_dstrTagURL,              NULL, NULL, 0, NULL,          f_pdstrDataURL));
    }
    ChkDR(DRM_XML_GetNode(&dstrListData,      &g_dstrTagDataPubKey,       NULL, NULL, 0, NULL,          &dstrDataPublicKey));
    ChkDR(DRM_XML_GetNode(&dstrListSignature, &g_dstrTagValue,            NULL, NULL, 0, NULL,          &dstrDataSignature));
    ChkDR(DRM_XML_GetNode(&dstrListCertChain, &g_dstrTagCertificate,      NULL, NULL, 0, NULL,          &dstrDataCertificate));

    if( f_pdstrDataMID != NULL )
    {
        *f_pdstrDataMID = dstrMID;
    }

    /* verify the certificate and signature against the metering public key */
    ChkDR(_VerifySignature(&dstrNodeData, &dstrDataSignature, &dstrDataCertificate, f_pcontextLEVL));

    /* base 64 decode the necessary fields */
    if( f_ppubkeyMAS != NULL )
    {
        ChkDR(DRM_B64_DecodeW(&dstrDataPublicKey, &cbPublicKey, (DRM_BYTE *)f_ppubkeyMAS, 0));
    }

    if( f_pmid != NULL )
    {
        ChkDR(DRM_B64_DecodeW(&dstrMID,     &cbMID,       (DRM_BYTE *)f_pmid,       0));
    }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function: _ProcessOneKid
**
** Synopsis: read action/count entries - this test is to avoid accidentally
**           parsing any "scrap" at the end of the buffer
**
** Arguments:
*****************************************************************************/
static DRM_RESULT _ProcessOneKid(
    __in                     DRM_DWORD         f_cbDataKID,
    __in_ecount(f_cbDataKID) DRM_BYTE         *f_pbDataKID,
    __out                    DRM_CONST_STRING *f_pdstrData,
    __inout                  _XMBContext      *f_pbAlignedBuffer )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  ibCurr = 0;

    while ( ((ibCurr + CB_OPERATION_MINIMUM - 1) < f_cbDataKID)  &&  DRM_SUCCEEDED(dr) )
    {
        DRM_CONST_STRING  dstrOper = EMPTY_DRM_STRING;
        DRM_BYTE         *pbAction = NULL;
        DRM_WORD          cbCurr   = 0;
        DRM_WCHAR         awchNum[16];
        TOKEN             token    = { 0 };

        /* string size */
        DRM_BYT_CopyBytes(&cbCurr, 0, f_pbDataKID, ibCurr, SIZEOF (DRM_WORD));
        FIX_ENDIAN_WORD( cbCurr );
        ibCurr += SIZEOF (DRM_WORD);


        /* string "Play" or whatever */
        if ((ibCurr % CB_NATIVE_BYTE) != 0)
        {
            /* if the UNICODE string isn't aligned in the packed buffer then move the string one
               char to the left (this is safe, the data won't be reparsed) */
            pbAction = (f_pbDataKID + __CB_DECL((ibCurr & ~CB_NATIVE_BYTE)));
            DRM_BYT_CopyBytes(pbAction, 0, f_pbDataKID, ibCurr, cbCurr);
        }
        else
        {
            pbAction = f_pbDataKID + __CB_DECL(ibCurr);
        }

        DSTR_FROM_PB(&dstrOper, pbAction, cbCurr);
        ibCurr += cbCurr;


        /* count size */
        DRM_BYT_CopyBytes( &token.TokenType, 0, f_pbDataKID, ibCurr, SIZEOF( DRM_DWORD ) );

        FIX_ENDIAN_DWORD( token.TokenType );

        ChkBOOL( token.TokenType == TOKEN_LONG, DRM_E_WRONG_TOKEN_TYPE );

        DRM_BYT_CopyBytes( &token.val.lValue, 0, f_pbDataKID, ibCurr + SIZEOF( DRM_DWORD ), SIZEOF( DRM_LONG ) );

        FIX_ENDIAN_LONG( token.val.lValue );

        ibCurr += SIZEOF( PERSISTEDTOKEN );

        ChkDR( DRM_UTL_NumberToString( (DRM_DWORD)token.val.lValue, awchNum, NO_OF(awchNum), 0, 10, &f_pdstrData->cchString ) );
        f_pdstrData->pwszString = awchNum;

        /* got all the data for this KID; write its XML node: <ACTION value="play">1</ACTION> */
        dr = DRM_XMB_WriteTag(
                 f_pbAlignedBuffer,
                 &g_dstrTagAction,
                 f_pdstrData,
                 &g_dstrLabelValue,
                 &dstrOper,
                 wttClosed);
    } /* end while operations and counts */

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function: _ProcessAllKIDs
**
** Synopsis: Process all KIDs for the challenge that can fit
**
**
** Arguments:
*****************************************************************************/
static DRM_RESULT _ProcessAllKIDs(
    __in        DRM_METERINGENUM_CONTEXT    *f_pmeterenum,
    __in        DRM_METERING_CONTEXT        *f_pmeterstore,
    __in  const DRM_MID                     *f_pmid,
    __in  const DRM_BYTE                     f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in  const DRM_DST                     *f_pDatastore,
    __in        DRM_STACK_ALLOCATOR_CONTEXT *f_pcontextStack,
    __out       _XMBContext                 *f_pbAlignedBuffer,
    __out       DRM_DWORD                   *f_pcKIDs,
    __out       DRM_BOOL                    *f_pfPartial )
{
    DRM_RESULT        dr        = DRM_SUCCESS;
    DRM_RESULT        drXMB     = DRM_SUCCESS;
    DRM_KID           kid       = { 0 };
    DRM_BYTE         *pbDataKID = NULL;
    DRM_WCHAR        *pwchKID   = NULL;
    DRM_CONST_STRING  dstrData  = EMPTY_DRM_STRING;

    ChkArg(f_pmeterenum     != NULL
        && f_pmeterstore    != NULL
        && f_pmid           != NULL
        && f_rgbPasswordSST != NULL
        && f_pDatastore    != NULL
        && f_pcontextStack  != NULL
        && f_pbAlignedBuffer!= NULL
        && f_pcKIDs         != NULL
        && f_pfPartial      != NULL);

    *f_pfPartial = FALSE;

    ChkDR(DRM_STK_Alloc(f_pcontextStack, (CCH_BASE64_EQUIV (SIZEOF (kid)) * SIZEOF (DRM_WCHAR)), (DRM_VOID **) &pwchKID));

    while( DRM_SUCCEEDED(drXMB) )
    {
        DRM_DWORD cchKID    = 0;
        DRM_DWORD cbDataKID = 0;
        XMBState  xmbState  = {0};

        dr = DRM_MTR_EnumNext (f_pmeterenum, &kid, &cbDataKID);

        if (dr == DRM_E_NOMORE)
        {
            dr = DRM_SUCCESS;
            break;
        }
        ChkDR(dr);

        /* don't try to read the pseudo-KID representing the current transaction */

        if (MEMCMP(&kid, &g_kidTID,     SIZEOF (DRM_KID)) == 0
        ||  MEMCMP(&kid, &g_kidTIDPrev, SIZEOF (DRM_TID)) == 0)
        {
            continue;
        }

        dr = DRM_MTR_EnumLoadCurrent(f_pmeterenum,
                                     f_pmeterstore,
                                     f_rgbPasswordSST,
                                    &kid,
                                    &cbDataKID);

        if (dr == DRM_E_NOMORE)
        {
            dr = DRM_SUCCESS;
            break;
        }

        ChkDR(dr);

        dr = DRM_MTR_GetKIDData(f_pmeterstore,
                                f_pcontextStack,
                                &pbDataKID,
                                &cbDataKID);

        if (DRM_FAILED(dr))
        {
            break;
        }

        if (cbDataKID == 0
        ||  pbDataKID == NULL)
        {
            continue;    /* skip this empty slot */
        }

        /* set up for parsing KID data, _RequiredCharsForKID validates the stream integrity */

        dr = _RequiredCharsForKID (pbDataKID, cbDataKID, &cchKID);

        if (DRM_FAILED(dr))
        {
            break;
        }

        /* open <KID value="base64encoded kid"> node */

        cchKID = CCH_BASE64_EQUIV (SIZEOF(DRM_KID));

        ChkDR(DRM_B64_EncodeW((DRM_BYTE *) &kid, SIZEOF(kid), pwchKID, &cchKID, 0));

        dstrData.pwszString = pwchKID;
        dstrData.cchString  = cchKID;

        /* save the xml builder state and build the KID node */

        ChkDR(DRM_XMB_SaveState(f_pbAlignedBuffer, &xmbState));

        drXMB = DRM_XMB_WriteTag (f_pbAlignedBuffer, &g_dstrTagKID, NULL, &g_dstrLabelValue, &dstrData, wttOpen);

        /* read action/count entries
        ** this test is to avoid accidentally parsing any "scrap" at the end of the buffer */

        if (DRM_SUCCEEDED(drXMB))
        {
            drXMB = _ProcessOneKid(cbDataKID, pbDataKID, &dstrData, f_pbAlignedBuffer);

            SAFE_STK_FREE(f_pcontextStack, pbDataKID);

            if (DRM_SUCCEEDED(drXMB))
            {
                drXMB = DRM_XMB_CloseCurrNode(f_pbAlignedBuffer,  NULL);          /* </KID> */

                if (DRM_SUCCEEDED(drXMB))
                {
                    (*f_pcKIDs)++;   /* count # of KIDs processed */
                }
            }
        }

        dr = drXMB;  /* remember the XMB return code, success or failure, so it is passed back */

        /*
        ** This while loop continues to fill up the  buffer with KIDs until there is no more room left,
        ** at which time drXMB will be DRM_E_BUFFERTOOSMALL.  This is the expected exit scenario and
        ** reason for the f_pfPartial flag set below, so reset the overall return code (dr) to success.
        */
        if (drXMB == DRM_E_BUFFERTOOSMALL)
        {
            dr = DRM_SUCCESS;
        }

        if (DRM_FAILED(drXMB))
        {
            *f_pfPartial = TRUE;
            /* restore XMB state to where it was before adding the current <KID> node */
            ChkDR(DRM_XMB_RestoreState(f_pbAlignedBuffer, &xmbState));
            break;
        }
    } /* end while more KIDs */

ErrorExit:

    SAFE_STK_FREE(f_pcontextStack, pbDataKID);
    SAFE_STK_FREE(f_pcontextStack, pwchKID);
    return dr;
}


/*****************************************************************************
** Function: _BuildChallenge
**
** Synopsis: Build a challenge for sizeonly, overhead only or real
**           - if *f_pcbChallenge, a sizeonly will be processed
**           - if f_pcwchTrailsize != NULL, no kid will be included
**
**
** Arguments:
*****************************************************************************/
static DRM_RESULT _BuildChallenge(
    __inout          _XMBContext                 *f_pbAlignedBuffer,
    __inout   const DRM_DWORD                   *f_pcbChallenge,
    __out           DRM_STRING                  *f_pdstrXML,
    __in      const DRM_CONST_STRING            *f_pdstrDataMID,
    __in      const DRM_CONST_STRING            *f_pdstrDataURL,
    __in      const DRM_CONST_STRING            *f_pdstrDataTID,
    __in            DRM_METERINGENUM_CONTEXT    *f_pmeterenum,
    __in            DRM_STACK_ALLOCATOR_CONTEXT *f_pcontextStack,
    __in            DRM_METERING_CONTEXT        *f_pmeterstore,
    __in      const DRM_MID                     *f_pmid,
    __in      const DRM_BYTE                     f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in      const DRM_DST                     *f_pDatastore,
    __in            DRM_BB_CONTEXT              *f_pcontextBlackBox,
    __in      const PUBKEY                      *f_ppubkeyMAS,
    __in            PKCERT                      *f_pBBCert,
    __out_opt       DRM_DWORD                   *f_pcwchTrailsize )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_DWORD   cKIDs       = 0;
    XMBState    stateXMB    = { 0 };
    DRM_BOOL    fPartial    = FALSE;
    DRM_STRING  dstrRecordsNode  = EMPTY_DRM_STRING;
    DRM_WCHAR  *pwszB64Signature = NULL;
    DRM_WCHAR  *pwszB64FallbackCert = NULL;

    /* <METERDATA type="challenge"> */
    ChkDR(DRM_XMB_AddAttribute(f_pbAlignedBuffer, &g_dstrAttributeType, &g_dstrChallenge));

    /* add other nodes
    */
    ChkDR(DRM_XMB_WriteTag(f_pbAlignedBuffer, &g_dstrTagData, NULL,           NULL, NULL, wttOpen));   /*       <DATA> */
    ChkDR(DRM_XMB_WriteTag(f_pbAlignedBuffer, &g_dstrTagURL,  f_pdstrDataURL, NULL, NULL, wttClosed)); /*        <URL>...</URL> */
    ChkDR(DRM_XMB_WriteTag(f_pbAlignedBuffer, &g_dstrTagMID,  f_pdstrDataMID, NULL, NULL, wttClosed)); /*        <MID>...</MID> */
    ChkDR(DRM_XMB_WriteTag(f_pbAlignedBuffer, &g_dstrTagTID,  f_pdstrDataTID, NULL, NULL, wttClosed)); /*        <TID>base64-encode</TID> */

    /* Open the <RECORDS> node and enumerate the Key IDs for this Metering ID */
    ChkDR(DRM_XMB_WriteTag(f_pbAlignedBuffer, &g_dstrTagRecords, NULL, NULL, NULL, wttOpen));   /*        <RECORDS> */

    if ( f_pcwchTrailsize == NULL )
    {
        /* from this point on, if buffer too small, do a partial one */

        ChkDR(_ProcessAllKIDs(f_pmeterenum,
                              f_pmeterstore,
                              f_pmid,
                              f_rgbPasswordSST,
                              f_pDatastore,
                              f_pcontextStack,
                              f_pbAlignedBuffer,
                             &cKIDs,
                             &fPartial));

        /* conclude the challenge */

        if (cKIDs == 0
        &&  fPartial)
        {
            /* we do not have enough buffer to do one KID. */
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }
    }

    if (*f_pcbChallenge > 0)
    {
        /* use actual buffer size if this is a real run */
        ChkDR( DRM_XMB_ReallocDocument(f_pbAlignedBuffer, *f_pcbChallenge, f_pbAlignedBuffer) );
    }

    /* save current state of XMB for the purpose of marking the begining of XML trailer */
    if ( f_pcwchTrailsize != NULL )
    {
        ChkDR(DRM_XMB_SaveState(f_pbAlignedBuffer, &stateXMB));
    }

    /* encrypt and base64-encode the entire RECORDS section in place: </RECORDS> */
    ChkDR(DRM_XMB_EncryptAndCloseCurrNode(
                f_pbAlignedBuffer,
                &f_pcontextBlackBox->CryptoContext,
                f_pcontextBlackBox->pOEMContext,
                f_ppubkeyMAS,
                &dstrRecordsNode));

    /* if not all KIDs could fit in the output buffer, add a tag indicating that there is more to do */

    if (fPartial)
    {
        /* <PARTIALDATA>1</PARTIALDATA> */
        ChkDR(DRM_XMB_WriteTag(f_pbAlignedBuffer, &g_dstrTagPartialData, &g_dstr1, NULL, NULL, wttClosed));
    }

    /* Close and Sign the <DATA> node with device private key */
    {
        DRM_CONST_STRING dstrData   = EMPTY_DRM_STRING;
        DRM_DWORD        cbRequired = DRM_ECC160_SIGNATURE_B64LEN;


        /* alloc memory from local stack */
        ChkDR(DRM_STK_Alloc(f_pcontextStack,
                            DRM_ECC160_SIGNATURE_B64LEN * SIZEOF (DRM_WCHAR),
             (DRM_VOID **) &pwszB64Signature));

        ChkDR(DRM_XMB_CloseCurrNode(f_pbAlignedBuffer, (DRM_STRING *)&dstrData));

        if (*f_pcbChallenge > 0)
        {
            ChkDR(DRM_BBX_SignData(f_pcontextBlackBox,
                                   PB_DSTR(&dstrData),
                                   CB_DSTR(&dstrData),
                                   eBBX_PRIVKEY_MACHINE,
                                   (DRM_BYTE *) pwszB64Signature));
        }

        /* encode in-place */

        ChkDR(DRM_B64_EncodeW((DRM_BYTE *)pwszB64Signature,  /* encode in-place */
                               DRM_ECC160_SIGNATURE_LEN,
                               pwszB64Signature,
                               &cbRequired,
                               0));

        /* <MSDRM_SIGNATURE_VALUE>base64</MSDRM_SIGNATURE_VALUE> */
        dstrData.cchString  = DRM_ECC160_SIGNATURE_B64LEN;
        dstrData.pwszString = pwszB64Signature;

        ChkDR(DRM_XMB_WriteTag(f_pbAlignedBuffer, &g_dstrTagMSDRMSignature,  &dstrData, NULL, NULL, wttClosed));
    }

    /* <CERTIFICATE private="1">B64-fallback cert</CERTIFICATE> */
    {
        DRM_CONST_STRING dstrFallbackCert = EMPTY_DRM_STRING;

        /* alloc memory from local stack */
        ChkDR(DRM_STK_Alloc(f_pcontextStack,
                            (CCH_BASE64_EQUIV(SIZEOF(PKCERT)) * SIZEOF(DRM_WCHAR)),
                            (DRM_VOID **) &pwszB64FallbackCert));

        dstrFallbackCert.cchString  = CCH_BASE64_EQUIV(SIZEOF(PKCERT));
        dstrFallbackCert.pwszString = pwszB64FallbackCert;

        ChkDR(DRM_B64_EncodeW((DRM_BYTE *)f_pBBCert, SIZEOF(PKCERT), pwszB64FallbackCert, &dstrFallbackCert.cchString, 0));

        ChkDR(DRM_XMB_WriteTag(f_pbAlignedBuffer, &g_dstrTagCertificate, &dstrFallbackCert, &g_dstrRootSigValueVersionTag, &g_dstrRootSigValueVersionVal, wttClosed));

        /* </METERDATA> */
        ChkDR(DRM_XMB_CloseDocument(f_pbAlignedBuffer, f_pdstrXML));

        if ( f_pcwchTrailsize != NULL )
        {
            *f_pcwchTrailsize = f_pdstrXML->cchString - stateXMB.iwchNextStringPos;
        }
    }

ErrorExit:

    SAFE_STK_FREE(f_pcontextStack, pwszB64FallbackCert);
    SAFE_STK_FREE(f_pcontextStack, pwszB64Signature);

    return dr;
}



/*****************************************************************************
** Function: _AttemptGenerateMeterChallenge
**
** Synopsis: open the secure store for a given MID and create a metering
**           challenge XML blob.  A two-stage call, one to get required
**           sizes and another to fill in allocated buffers
**
** Arguments:
**           [f_pcontextBB]     -- initialized blackbox context
**           [f_pDatastore]     -- initialized DST context
**           [f_pBBCert]        -- pointer to fallback cert
**           [f_pbStack]
**           [f_cbStack]        -- pointer and size to a buffer to use for allocations
**           [f_eMode]          -- enumerative type indicating the "state" of this call
**           [f_pbChallenge]    -- buffer that will receive XML; NULL to
**                                 request buffer size
**           [f_pcbChallenge]   -- points to DWORD filled in with buffer
**                                 size, or to receive requested size
**           [f_pcKIDs]         -- (optional) to specify # of KIDs to be in the challenge.
**                                 use NULL or value 0 to get a full challenge.
**                                 Returns count of KIDs available in query mode
**           [f_pcbRecords]     -- (optional) to return the count of byte in the <RECORDS> node
**           [f_ppubkeyMAS]     -- metering authority public key extracted from the meter cert
**           [f_pdstrDataMID]   -- metering ID extracted from the meter cert
**           [f_pmid]           -- metering ID extracted from the meter cert as binary
**           [f_pdstrDataURL]   -- metering URL to write to challenge
*****************************************************************************/

static DRM_RESULT _AttemptGenerateMeterChallenge(
    __in                         DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in_ecount(f_cbStack)       DRM_BYTE            *f_pbStack,
    __in                         DRM_DWORD            f_cbStack,
    __out_opt                    _XMBContext         *f_pbChallenge,
    __inout                      DRM_DWORD           *f_pcbChallenge,
    __in                   const PUBKEY              *f_ppubkeyMAS,
    __in                   const DRM_CONST_STRING    *f_pdstrDataMID,
    __in                   const DRM_MID             *f_pmid,
    __in                   const DRM_CONST_STRING    *f_pdstrDataURL)
{
    DRM_RESULT                   dr = DRM_SUCCESS;
    DRM_METERING_CONTEXT        *pmeterstore      = NULL;
    DRM_METERINGENUM_CONTEXT    *pmeterenum       = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT  contextStack     = { 0, 0, NULL };
    DRM_TID                      tid              = { 0 };
    DRM_BYTE                     rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)] = { 0 };

    DRM_WCHAR        *pwchTID          = NULL;
    DRM_STRING        strXML           = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrData         = EMPTY_DRM_STRING;
    _XMBContext      *pbAlignedBuffer  = f_pbChallenge;
    _XMBContext      *pbTempBuff       = NULL;
    DRM_DWORD         cbAdjustment     = 0;
    _XMBContext      *pbBuffer         = NULL;
    DRM_DWORD         cwchTrailsize    = 0;
    PKCERT           *pCert            = NULL;

    ChkArg(f_pcontextLEVL != NULL
        && f_pcontextLEVL->pcontextBBX != NULL
        && f_pcontextLEVL->pDatastore != NULL
        && f_ppubkeyMAS   != NULL
        && f_pdstrDataMID != NULL
        && f_pmid         != NULL
        && f_pdstrDataURL != NULL
        && f_pcbChallenge != NULL
        && f_pbStack      != NULL
        && f_cbStack       > 0);

#if SIXTEEN_BIT_ADDRESSING
    pbAlignedBuffer = f_pbChallenge;
#else

    /* since this buffer is passed in as bytes, misalignment should not be regarded as a FAIL;
    ** adjust the buffer pointer for the duration
    */
    if (f_pbChallenge != NULL)
    {
        ChkDR(DRM_UTL_EnsureDataAlignment((DRM_BYTE *)f_pbChallenge, *f_pcbChallenge, (DRM_BYTE **)&pbAlignedBuffer, f_pcbChallenge, SIZEOF (DRM_DWORD), &cbAdjustment));
    }
#endif

    contextStack.cbStack = f_cbStack;
    contextStack.pbStack = f_pbStack;

    /* allocate large variables off the local stack
    */
    if ( f_pbChallenge == NULL  ||  *f_pcbChallenge == 0)
    {
        ChkDR(DRM_STK_Alloc(&contextStack, DRM_SIMXMLDOC_BUFFSIZE, (DRM_VOID **)&pbTempBuff));
        pbAlignedBuffer = pbTempBuff;
    }

    ChkDR(DRM_STK_Alloc(&contextStack, (CCH_BASE64_EQUIV (SIZEOF (tid)) * SIZEOF (DRM_WCHAR)), (DRM_VOID **) &pwchTID));

    ChkDR(DRM_STK_Alloc(&contextStack, SIZEOF(DRM_METERING_CONTEXT),     (DRM_VOID **) &pmeterstore));
    ChkDR(DRM_STK_Alloc(&contextStack, SIZEOF(DRM_METERINGENUM_CONTEXT), (DRM_VOID **) &pmeterenum));

    /* create the password that will be used to access the secure store
    ** note: the use of the B64 as the key rather than the decoded MID is by design
    */
    ChkDR( DRM_MTR_CreateMeterStorePasswordFromBinaryMID( f_pmid,
                                                          rgbPasswordSST,
                                                          f_pcontextLEVL->pcontextBBX ) );

    /*
    ** get the TID from the metering store
    ** if there is no TID in the meter store for this MID, create it now
    */

    dstrData.cchString  = CCH_BASE64_EQUIV (SIZEOF (DRM_TID));
    dstrData.pwszString = pwchTID;

    dr = DRM_MTR_ReadTIDFromMeteringStore( pmeterstore,
                                           rgbPasswordSST,
                                           f_pmid,
                                           &tid,
                                           &dstrData,
                                           f_pcontextLEVL->pDatastore );

    if (DRM_FAILED(dr))
    {
        ChkDR(DRM_MTR_CreateNewTID (f_pcontextLEVL->pcontextBBX->pOEMContext, &tid));
        ChkDR(DRM_MTR_WriteTIDToMeteringStore(pmeterstore, rgbPasswordSST, f_pmid, &tid, f_pcontextLEVL->pDatastore));
        ChkDR(DRM_B64_EncodeW((DRM_BYTE *)&tid, SIZEOF(tid), (DRM_WCHAR *)dstrData.pwszString, &dstrData.cchString, 0));
    }

    /* start creating the meter challenge
    ** enumerate the storages
    */

#if DRM_SUPPORT_DEVICE_SIGNING_KEY
    pCert = &f_pcontextLEVL->pcontextBBX->cachedCertValues.m_pubKeySigningCert;
#else
    pCert = &f_pcontextLEVL->pcontextBBX->cachedCertValues.m_BBCompatibilityCert;
#endif

    ChkDR( DRM_MTR_InitEnum(f_pmid, pmeterenum, f_pcontextLEVL->pDatastore) );

    /* this is a query for the required buffer size; fake it */
    if ( *f_pcbChallenge == 0  ||  f_pbChallenge == NULL )
    {
        /* <METERDATA type="challenge"> */
        ChkDR(DRM_XMB_SimulateCreateDocument(DRM_SIMXMLDOC_BUFFSIZE, pbAlignedBuffer, &g_dstrTagMetering));
        *f_pcbChallenge = 0;
    }
    else
    {
        /* calculate overhead size */
        DRM_DWORD cbEffBuffer = 0;
        DRM_DWORD cbRequired  = 0;

        ChkDR(DRM_STK_Alloc(&contextStack, DRM_SIMXMLDOC_BUFFSIZE, (DRM_VOID **)&pbBuffer));
        ChkDR(DRM_XMB_SimulateCreateDocument(DRM_SIMXMLDOC_BUFFSIZE, pbBuffer, &g_dstrTagMetering));

        /* calculate the buffer requirements without KIDs */

        ChkDR(_BuildChallenge(pbBuffer,
                             &cbEffBuffer,
                             &strXML,
                              f_pdstrDataMID,
                              f_pdstrDataURL,
                             &dstrData,
                              pmeterenum,
                             &contextStack,
                              pmeterstore,
                              f_pmid,
                              rgbPasswordSST,
                              f_pcontextLEVL->pDatastore,
                              f_pcontextLEVL->pcontextBBX,
                              f_ppubkeyMAS,
                              pCert,
                             &cwchTrailsize));
        SAFE_STK_FREE(&contextStack, pbBuffer);

        /* calculate effective buffer size before encryption and b64 encode:
        ** encryption will increase the XML blob by DRM_ECC160_CIPHERTEXT_LEN bytes
        ** B64 encode increase the XML blob by 8/3 of the encrypted content: 4/3 for B64 encode and 2 (sizeof(WCHAR)) bytes for unicode */

        cbRequired = (CB_DSTR(&strXML)
                   + cbAdjustment
                   + DRM_XMB_OVERHEAD
                   + DRM_SIMXMLDOC_BUFFSIZE)
                   + CCH_BASE64_ENC_LARGE_WCH(DRM_SIMXMLDOC_BUFFSIZE);

        if (*f_pcbChallenge < cbRequired)
        {
            *f_pcbChallenge = cbRequired;
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        /* max allowable buffer size for KIDS
        ** available KIDs data = buffersize - XML builder overhead - challenge string overhead
        */
        cbEffBuffer = (((*f_pcbChallenge - DRM_XMB_OVERHEAD - CB_DSTR(&strXML)) / (4 * SIZEOF(DRM_WCHAR))) * 3)
                    - DRM_ECC160_CIPHERTEXT_LEN;

        /* adjusted size for XMB context */
        cbEffBuffer += SIZEOF(_XMBContext) + CB_DSTR(&strXML) - (cwchTrailsize * SIZEOF(DRM_WCHAR));

        /* <METERDATA type="challenge"> */
        ChkDR(DRM_XMB_CreateDocument(cbEffBuffer, pbAlignedBuffer, &g_dstrTagMetering));
    }

    ChkDR(_BuildChallenge(pbAlignedBuffer,
                          f_pcbChallenge,
                         &strXML,
                          f_pdstrDataMID,
                          f_pdstrDataURL,
                         &dstrData,
                          pmeterenum,
                         &contextStack,
                          pmeterstore,
                          f_pmid,
                          rgbPasswordSST,
                          f_pcontextLEVL->pDatastore,
                          f_pcontextLEVL->pcontextBBX,
                          f_ppubkeyMAS,
                          pCert,
                          NULL));

    /*
    ** if we are building a challenge then report the actual usage and copy into the head of the buffer
    */

    if (f_pbChallenge != NULL)
    {
        *f_pcbChallenge = CB_DSTR(&strXML);

        strXML.pwszString [strXML.cchString] = g_wchNull;

        /* save the results in output buffers */

        MEMMOVE(f_pbChallenge, strXML.pwszString, *f_pcbChallenge + SIZEOF(DRM_WCHAR));
    }
    else
    {
        /*
        ** if we're querying for required size include the overhead requirements
        */

        *f_pcbChallenge = CB_DSTR(&strXML)
                        + cbAdjustment
                        + DRM_XMB_OVERHEAD
                        + DRM_SIMXMLDOC_BUFFSIZE
                        + CCH_BASE64_ENC_LARGE_WCH(DRM_SIMXMLDOC_BUFFSIZE);
        dr = DRM_E_BUFFERTOOSMALL;
    }

ErrorExit:

    SAFE_STK_FREE(&contextStack, pbBuffer);
    SAFE_STK_FREE(&contextStack, pmeterenum);
    SAFE_STK_FREE(&contextStack, pmeterstore);
    SAFE_STK_FREE(&contextStack, pwchTID);
    SAFE_STK_FREE(&contextStack, pbTempBuff);

    return dr;
} /* _AttemptGenerateMeterChallenge */




/*****************************************************************************
** Function: DRM_MTR_GenerateMeterChallenge
**
** Synopsis: open the secure store for a given MID and create a metering
**           challenge XML blob.  A two-stage call, one to get required
**           sizes and another to fill in allocated buffers
**
** Arguments:
**           [f_pcontextLEVL]   -- initialized license evaluator context
**           [f_pbStack]
**           [f_cbStack]        -- pointer and size to a buffer to use for allocations
**           [f_pdstrMeterCert] -- XML blob representing the metering
**                                 certificate for the MAS that will
**                                 receive the metering challenge
**           [f_pChallenge]     -- buffer that will receive XML; NULL to
**                                 request buffer size
**           [f_pcbChallenge]   -- points to DWORD filled in with buffer
**                                 size, or to receive requested size
**           [f_pszURL]         -- optional buffer to receive metering URL;
**                                 NULL to request buffer size
**           [f_pcchURL]        -- points to DWORD filled in with URL length
**                                 (in characters, not bytes) or to receive requested size
*****************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_MTR_GenerateMeterChallenge(
    __in                                 DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __inout_bcount( f_cbStack )          DRM_BYTE            *f_pbStack,
    __in                                 DRM_DWORD            f_cbStack,
    __in                           const DRM_CONST_STRING    *f_pdstrMeterCert,
    __out                                _XMBContext         *f_pChallenge,
    __inout                              DRM_DWORD           *f_pcbChallenge,
    __out_ecount_opt( *f_pcchURL )       DRM_CHAR            *f_pchURL,
    __inout                              DRM_DWORD           *f_pcchURL )
{
    DRM_RESULT        dr = DRM_SUCCESS;
    PUBKEY            pubkeyMAS;
    DRM_MID           mid;
    DRM_CONST_STRING  dstrDataMID = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrDataURL = EMPTY_DRM_STRING;

    ChkArg(f_pcontextLEVL   != NULL
        && f_pbStack        != NULL
        && f_cbStack         > 0
        && f_pdstrMeterCert != NULL
        && f_pcbChallenge   != NULL);

    ChkDR(DRM_MTR_ParseMeterCert(f_pdstrMeterCert, f_pcontextLEVL, &pubkeyMAS, &dstrDataMID, &mid, &dstrDataURL));

    ChkArg( dstrDataURL.cchString < DRM_SAFE_UINT );

    /*
    ** get sizes when:
    ** challenge buffer is NULL or
    ** challenge size is NULL or
    ** URL size is not NULL and (URL size < actual size + 1  or  URL buffer is NULL)
    */
    if (f_pChallenge  == NULL
    || *f_pcbChallenge == 0
    || (f_pcchURL      != NULL
     && (*f_pcchURL < dstrDataURL.cchString + 1
       || f_pchURL == NULL)))
    {
        ChkDR(_AttemptGenerateMeterChallenge( f_pcontextLEVL,
                                              f_pbStack,
                                              f_cbStack,
                                              NULL,
                                              f_pcbChallenge,
                                             &pubkeyMAS,
                                             &dstrDataMID,
                                             &mid,
                                             &dstrDataURL));

        goto ErrorExit;  /* if the above function is successful, we are also done */
    }

    /* copy URL to buffer */

    if (f_pchURL != NULL
    &&  f_pcchURL != NULL)
    {
        ZEROMEM( f_pchURL, *f_pcchURL );

        ChkDR( DRM_UTL_XMLDecodeUA( dstrDataURL.pwszString,
                                    dstrDataURL.cchString,
                                    f_pchURL,
                                    f_pcchURL ) );
    }

    /* generate the challenge */
    ChkDR(_AttemptGenerateMeterChallenge( f_pcontextLEVL,
                                          f_pbStack,
                                          f_cbStack,
                                          f_pChallenge,
                                          f_pcbChallenge,
                                         &pubkeyMAS,
                                         &dstrDataMID,
                                         &mid,
                                         &dstrDataURL));

ErrorExit:

    if ( f_pcchURL != NULL )
    {
        *f_pcchURL = dstrDataURL.cchString + 1;
    }

    return dr;
} /* DRM_MTR_GenerateMeterChallenge */

/*****************************************************************************
** Function: DRM_MTR_ProcessMeterResponse
**
** Synopsis: open the secure store for a given MID and delete the KIDs named
**           in the response XML
**
** Arguments:
**           [pvcontextBlackBoxIn] -- initialized blackbox context
**           [f_pbStack]
**           [f_cbStack]  -- pointer and size to a buffer to use
**                           for allocations
**           [f_pbData]   -- metering response XML
**           [f_cbData]   -- size of metering response XML in bytes
**           [f_pfFlags]-- points to DWORD filled in with any flags passed
**                           back to the caller.
**           [f_pDatastore]-- initialized DST context
**
** Notes:
**         Currently defined flags:
**
**            DRM_METER_RESPONSE_PARTIAL - not all KIDs for this MID were sent
**                                         and another challenge should be
**                                         generated
** Algorithm:
** After the <KID> areas are Base64 decoded and decrypted, it leaves 5/8 of the
** original size of the buffer at end. We can use that area to store all the
** KIDs there for better retrieval performance. The KIDs found in the response
** XML string are first Base64 decoded and the sorted and inserted in a list in
** ascending order. Then the list is reconciled with the Metering Store: the
** store is enumerated for all KIDs associated with the MID, for each KIDs visited,
** it is matched with the list of sorted KIDs. When all the KIDs in the list is
** all matched or the metering stored is exhausted, we are done.
*****************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_MTR_ProcessMeterResponse(
    __inout                        DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __inout_bcount( f_cbStack )    DRM_BYTE            *f_pbStack,
    __in                           DRM_DWORD            f_cbStack,
    __in_bcount( f_cbData )  const DRM_BYTE            *f_pbData,
    __in                           DRM_DWORD            f_cbData,
    __out                          DRM_DWORD           *f_pfFlags )
{
    DRM_RESULT        dr             = DRM_SUCCESS;
    DRM_METERING_CONTEXT         *pmeterstore       = NULL;
    DRM_METERINGENUM_CONTEXT     *pmeterenum        = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT   contextStack      = { 0, 0, NULL };
    DRM_MID           mid;
    DRM_TID           tid;
    DRM_KID          *pkids          = NULL;
    DRM_BYTE          rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)] = { 0 };
    DRM_WCHAR         awchTID [CCH_BASE64_EQUIV (SIZEOF (DRM_TID))]   = { 0 };
    DRM_CONST_STRING  dstrXML        = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrListTop    = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrListData   = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrDataMID    = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrCertMID    = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrDataCmd    = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrTIDRequest = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrRecords    = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrTIDStore   = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrDataMore   = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrCertRoot   = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrCertSign   = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrCertChain  = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrCertData   = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrCertPubkey = EMPTY_DRM_STRING;
    DRM_DWORD         cbMID          = SIZEOF (mid);
    DRM_DWORD         cbBuffer       = 0;
    DRM_CONST_STRING dstrDataKID   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrAttrValue = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTag       = EMPTY_DRM_STRING;
    DRM_BOOL         fIsLeaf       = FALSE;
    DRM_DWORD        cb            = 0;
    DRM_BYTE        *pbKIDs        = NULL;
    const PUBKEY    *pPubKey       = NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_METERING, PERF_FUNC_DRM_MTR_ProcessMeterResponse);

    /* must be WORD-aligned */

    ChkArg (((DRM_DWORD_PTR) f_pbData) % SIZEOF (DRM_WCHAR) == 0);

    ChkArg (f_pcontextLEVL != NULL
        &&  f_pbData       != NULL
        &&  f_pbStack      != NULL
        &&  f_pfFlags      != NULL
        &&  f_cbData        > 0
        &&  f_cbStack       > 0);

    *f_pfFlags = 0;

    contextStack.cbStack = f_cbStack;
    contextStack.pbStack = f_pbStack;

    DSTR_FROM_PB(&dstrXML, f_pbData, f_cbData);

    ChkDR(DRM_XML_GetNode (&dstrXML,      &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop));

    /*
    ** First check the Meter cert signature
    ** Get the <DATA> with the tag
    ** Get the signature value
    */

    ChkDR( DRM_XML_GetNode( &dstrListTop,  &g_dstrTagMeterCert,         NULL, NULL, 0,  NULL,          &dstrCertRoot  ) );
    ChkDR( DRM_XML_GetNode( &dstrCertRoot, &g_dstrTagData,              NULL, NULL, 0, &dstrCertData,   NULL  ) );
    ChkDR( DRM_XML_GetNode( &dstrCertRoot, &g_dstrTagSignature,         NULL, NULL, 0, &dstrCertSign,   NULL  ) );
    ChkDR( DRM_XML_GetNode( &dstrCertRoot, &g_dstrTagCertificateChain,  NULL, NULL, 0, &dstrCertChain,  NULL  ) );
    ChkDR( DRM_UTL_VerifyXMLSignature( &dstrCertData,
                                       &dstrCertSign,
                                       &dstrCertChain,
                                        FALSE,
                                        TRUE,
                                        WM_DRM_ROOTPUBKEY_MTR,
                                        f_pcontextLEVL) );

    /* Now check the signature on Data portion of METERDATA */

    ChkDR( DRM_XML_GetNode( &dstrCertRoot, &g_dstrTagData,           NULL, NULL, 0, NULL,         &dstrCertData ) );   /* Get cert public key  */
    ChkDR( DRM_XML_GetNode( &dstrCertData, &g_dstrTagDataPubKey,     NULL, NULL, 0, NULL,         &dstrCertPubkey ) ); /* Get MID also to compare afterwards */
    ChkDR( DRM_XML_GetNode( &dstrCertData, &g_dstrTagMID,            NULL, NULL, 0, NULL,         &dstrCertMID ) );
    ChkDR( DRM_XML_GetNode( &dstrListTop,  &g_dstrTagData,           NULL, NULL, 0, &dstrListData, NULL ) );           /* Get the Meter <DATA> with the tag */
    ChkDR( DRM_XML_GetNode( &dstrListTop,  &g_dstrTagMSDRMSignature, NULL, NULL, 0, NULL,          &dstrCertSign ) );  /* Get Signature value */


    cbBuffer = SIZEOF(PUBKEY);
    ChkDR(DRM_B64_DecodeW(&dstrCertPubkey, &cbBuffer,(DRM_BYTE*) &(f_pcontextLEVL->pcontextBBX->CryptoContext.pubKey), 0) );

    ChkDRMap( DRM_UTL_VerifyXMLSignatureEx(&dstrListData, &dstrCertSign, NULL, FALSE, FALSE, WM_DRM_ROOTPUBKEY_MTR, f_pcontextLEVL), DRM_E_INVALID_SIGNATURE, DRM_E_INVALID_METERRESPONSE_SIGNATURE ) ;

    /* get the MID, TID, and COMMAND data out of this XML blob */
    /* all are required */

    ChkDR(DRM_XML_GetNode (&dstrListTop,  &g_dstrTagData,     NULL, NULL, 0, NULL, &dstrListData));
    ChkDR(DRM_XML_GetNode (&dstrListData, &g_dstrTagMID,      NULL, NULL, 0, NULL, &dstrDataMID));
    ChkDR(DRM_XML_GetNode (&dstrListData, &g_dstrTagCmd,      NULL, NULL, 0, NULL, &dstrDataCmd));
    ChkDR(DRM_XML_GetNode (&dstrListData, &g_dstrTagTID,      NULL, NULL, 0, NULL, &dstrTIDRequest));
    ChkDR(DRM_XML_GetNode (&dstrListData, &g_dstrTagRecords,  NULL, NULL, 0, NULL, &dstrRecords));


    /*Verify MIDs*/
    if (! DRM_UTL_DSTRStringsEqual(&dstrCertMID,&dstrDataMID))
    {
        ChkDR(DRM_E_METERING_MID_MISMATCH);
    }


    /* get the optional PARTIALDATA tag */
    dr = DRM_XML_GetNode(&dstrListData,  &g_dstrTagPartialData, NULL, NULL, 0, NULL, &dstrDataMore);

    if (DRM_SUCCEEDED (dr)
    &&  dstrDataMore.cchString > 0)
    {
        *f_pfFlags = DRM_METER_RESPONSE_PARTIAL;
    }

    /* only the command "RESET" is supported */

    ChkBOOL(DRM_UTL_DSTRStringsEqual(&dstrDataCmd, &g_dstrReset), DRM_E_METERING_INVALID_COMMAND);

    ChkDR(DRM_B64_DecodeW(&dstrDataMID, &cbMID, (DRM_BYTE *) &mid, 0));

    ChkDR( DRM_MTR_CreateMeterStorePasswordFromBinaryMID( &mid,
                                                      rgbPasswordSST,
                                                      f_pcontextLEVL->pcontextBBX ) );

    ChkDR(DRM_STK_Alloc(&contextStack, SIZEOF (DRM_METERING_CONTEXT),      (DRM_VOID **) &pmeterstore));
    ChkDR(DRM_STK_Alloc(&contextStack, SIZEOF (DRM_METERING_CONTEXT),      (DRM_VOID **) &pmeterenum));

    /* get the TID from the metering store */

    dstrTIDStore.cchString  = NO_OF (awchTID);
    dstrTIDStore.pwszString =        awchTID;

    ChkDR(DRM_MTR_ReadTIDFromMeteringStore(pmeterstore,
                                           rgbPasswordSST,
                                           &mid,
                                           &tid,
                                           &dstrTIDStore,
                                           f_pcontextLEVL->pDatastore));

    /* verify that we have been passed the right TID */

    ChkBOOL(DRM_UTL_DSTRStringsEqual(&dstrTIDRequest, &dstrTIDStore), DRM_E_METERING_WRONG_TID);

    if (dstrRecords.cchString   > 0
    &&  dstrRecords.pwszString != NULL)
    {
        DRM_CONST_STRING dstrDataInner = EMPTY_DRM_STRING;
        DRM_DWORD iKID          = 0;

        /* get each <KID> value from the request and try to open it in the metering store */
        if ( dstrRecords.cchString == 0 )
        {
            /* no records, this is not an error */
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }

        cbBuffer = CB_DSTR(&dstrRecords);
        ChkDR(DRM_B64_DecodeW(&dstrRecords, &cbBuffer, NULL, DRM_BASE64_DECODE_IN_PLACE));

        /* The KID data inside <RECORDS> tag is smaller now since it was B64 decoded */
        dstrRecords.cchString = cbBuffer / SIZEOF(DRM_WCHAR);

#if DRM_SUPPORT_DEVICE_SIGNING_KEY
    pPubKey = &f_pcontextLEVL->pcontextBBX->cachedCertValues.pubkeySigning;
#endif

        if (! DRM_BBX_DecryptLicense(PB_DSTR(&dstrRecords),
                                     cbBuffer,
                                     NULL,  /* in-place */
                                     f_pcontextLEVL->pcontextBBX,
                                     pPubKey ) )
        {
            ChkDR(DRM_E_METERING_RESPONSE_DECRYPT_FAILED);
        }

        /* re-use the metering response buffer;
        ** after b64decoding and decryption we have 5/8 of the area.to work with and it
        ** is guaranteed to hold the decoded KIDs, so we decode directly into it
        */

        ChkFAIL(! ISODD(cbBuffer));

#if SIXTEEN_BIT_ADDRESSING
        if ((cbBuffer % SIZEOF(DRM_BYTE)) != 0)
        {
            cbBuffer++;
        }

        ChkFAIL(! ISODD(cbBuffer));
#endif
        /* pbKIDs is guaranteed to be BYTE16 aligned because it's UNICODE */

        pbKIDs    = PB_DSTR(&dstrRecords);
        pbKIDs   += __CB_DECL(cbBuffer);
        pkids = (DRM_KID *) pbKIDs;

        /* Decrypted data is at dstrRecords.pwszString + DRM_ECC160_CIPHERTEXT_LEN (BYTE offset) per DRM_BBX_DecryptLicense */
        AssertChkArg( dstrRecords.cchString >= DRM_ECC160_CIPHERTEXT_LEN / SIZEOF(DRM_WCHAR) );  /* DRM_BBX_DecryptLicense would have failed with DRM_E_INVALIDARG if this condition wasn't met */
        dstrRecords.pwszString += DRM_ECC160_CIPHERTEXT_LEN / SIZEOF(DRM_WCHAR);
        dstrRecords.cchString -= DRM_ECC160_CIPHERTEXT_LEN / SIZEOF(DRM_WCHAR);

        dr = DRM_XML_GetNode(&dstrRecords, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrDataInner);

        if (dr == DRM_E_XMLNOTFOUND)
        {
            dr = DRM_SUCCESS;
            dstrDataInner = dstrRecords;
        }
        ChkDR(dr);

        if (dstrDataInner.cchString == 0)
        {
            dr = DRM_SUCCESS;    /* no KID found */
            goto ErrorExit;
        }

        ChkDR(DRM_MTR_InitEnum(&mid, pmeterenum, f_pcontextLEVL->pDatastore));

        while (TRUE)
        {
            INIT_DRM_STRING(dstrTag);
            INIT_DRM_STRING(dstrDataKID);
            INIT_DRM_STRING(dstrAttrValue);

            if (dstrDataInner.pwszString == NULL
            ||  dstrDataInner.cchString  == 0)
            {
                break;
            }

            dr = DRM_XML_EnumNextNode(&dstrDataInner, &dstrTag, &dstrDataKID, NULL, NULL, NULL, &fIsLeaf);

            if (dr == DRM_E_NOMORE
            ||  dr == DRM_E_XMLNOTFOUND)
            {
                ChkDR(DRM_SUCCESS);  /* This sets 'dr' and keeps the PREfast tool happy. */
                break;
            }

            ChkDR(dr);

            /* if this is a KID tag */

            if (DRMCRT_wcsncmp(g_dstrTagKID.pwszString, dstrTag.pwszString, g_dstrTagKID.cchString) == 0)
            {
                /* get the value attribute */

                dr = DRM_XML_GetNodeAttribute(&dstrDataKID, &g_dstrLabelValue, &dstrAttrValue);

                if (DRM_SUCCEEDED(dr))
                {
                    cb     =  CB_DSTR(&dstrAttrValue);

                    /* base64 decode the kid value */

                    ChkDR(DRM_B64_DecodeW(&dstrAttrValue,
                                          &cb,
                             (DRM_BYTE *) (pkids + iKID++),
                                           0));
                }
            } /* endif is <KID> */

            /* advance dstrDataInner past the current <KID> node */

            dstrDataInner.cchString -= (DRM_DWORD)(dstrDataKID.pwszString - dstrDataInner.pwszString) + dstrDataKID.cchString;
            dstrDataInner.pwszString = dstrDataKID.pwszString                                         + dstrDataKID.cchString;
        } /* endwhile more KIDs in the response */

        if (iKID > 0)
        {
            ChkDR( DRM_DST_BlockScanDelete( pmeterstore->pDatastore,
                                            &pmeterenum->oSecStoreEnum.oNsContext,
                                            pkids,
                                            iKID,
                                            &mid,
                                            TRUE ) ); /* Wait for locks */
        }
    } /* end if <RECORDS> isn't empty */

    /* Now create a new TID and write it to the metering store */

    ChkDR(DRM_MTR_CreateNewTID (f_pcontextLEVL->pcontextBBX->pOEMContext, &tid));

    ChkDR(DRM_MTR_WriteTIDToMeteringStore(pmeterstore,
                                          rgbPasswordSST,
                                          &mid,
                                          &tid,
                                          f_pcontextLEVL->pDatastore));

ErrorExit:

    SAFE_STK_FREE(&contextStack, pmeterenum);
    SAFE_STK_FREE(&contextStack, pmeterstore);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
} /* DRM_MTR_ProcessMeterResponse */

EXIT_PK_NAMESPACE_CODE;
