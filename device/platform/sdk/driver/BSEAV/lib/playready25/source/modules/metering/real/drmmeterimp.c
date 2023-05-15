/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMMETERIMP_C
#include <drmmeterimp.h>
#include <drmxmlparser.h>
#include <drmxmlbuildera.h>
#include <drmxmlsig.h>
#include <drmsoapxmlconstants.h>
#include <drmsoapxmlutility.h>
#include <drmbcertparser.h>
#include <drmbcrl.h>
#include <drmmetercertrevocation.h>
#include <drmmeterconstants.h>
#include <drmxmlsigconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

/*
** FFFFFFFF (8 characters) = 4294967295 (10 characters).
*/
#define MAX_CCH_BASE10_DWORD_STRING     10

/*
** Total number of KIDs included in a single batch to
** be removed from the meter store using a HDS batch
** delete API (DRM_HDS_BlockScanDelete()). Bigger value
** uses more stack space but reduces the total number
** of calls of the batch delete API.
*/
#define MAX_KIDS_PER_BATCH   10

/*
** Structure that encapsulates information needed to build the
** metering challenge.
*/
typedef struct
{
    /* Meter ID (MID). */
    DRM_MID m_oMID;

    /* The current transaction ID (TID). */
    DRM_TID m_oTID;

    /* Black box context. */
    DRM_BB_CONTEXT *m_poBBXContext;

    /* Data store context. */
    DRM_DST *m_poDSTContext;

    /* Meter store context. */
    DRM_METERING_CONTEXT m_oMeterStoreContext;

    /* Stack context used to allocate of small chunk of memory. */
    DRM_STACK_ALLOCATOR_CONTEXT m_oStackContext;

    /*
    ** Max character count of the meter data challenge that
    ** may or may not includes meter data of all KIDs in
    ** the metering store.
    */
    DRM_DWORD m_cchMax;

    /*
    ** The character count of fixed portion of the meter
    ** data challenge that is not encrypted. For example,
    ** XML signature.
    */
    DRM_DWORD m_cchFixedUnencrypted;

    /*
    ** The character count of fixed portion of the meter
    ** data challenge that is created by XML encryption
    ** (Data that is under the <EncryptedData> (inclusive)).
    */
    DRM_DWORD m_cchFixedEncrypted;

    /*
    ** The variable character count of the meter data challenge
    ** based on the character count of the buffer provided by
    ** the user.
    ** It's the character count of the buffer provided by the
    ** user minus the character count of the fixed portions
    ** (both encrypted and unencrypted) of the meter data
    ** challenge.
    */
    DRM_DWORD m_cchVaried;

    /*
    ** Password derived from the meter ID (MID) and is used
    ** to retrieve data in the meter store.
    */
    DRM_BYTE m_rgbSSTPassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ];

    /*
    ** Key information used to encrypt portion of the meter
    ** data challenge.
    */
    DRM_XML_KEY m_oXMLKey;

    /* Pointer to a buffer used to stored the device cert. */
    DRM_BYTE *m_pbDeviceCert;

    /* Size (number of bytes) of the device cert. */
    DRM_DWORD m_cbDeviceCert;

    /* Pointer to a buffer used to stored the custom data string. */
    DRM_CHAR *m_pchCustomData;

    /* Size (number of characters) of the custom data string. */
    DRM_DWORD m_cchCustomData;

    /*
    ** Flag indicating whether metering data of all KIDs
    ** has been included in the generated meter data challenge.
    */
    DRM_BOOL m_fPartialData;
} _METER_CHALLENGE_CONTEXT;


/* pseudo-hashes to read/write TIDs and TIDKeys in the metering store */
/* External references. */
extern DRM_EXPORT_VAR const DRM_KID g_kidTID;
extern                const DRM_KID g_kidTIDPrev;

const DRM_EXPORT_VAR DRM_KID g_kidTIDKey =
{
    'K', 0,
    'E', 0,
    'Y', 0,
    '4', 0,
    'T', 0,
    'I', 0,
    'D', 0,
    '\0', 0
};


/* Forward declaration of local function prototypes. */
static DRM_RESULT _ProcessAllKIDs(
    __in        _METER_CHALLENGE_CONTEXT *f_poChallengeContext,
    __inout_opt _XMBContextA             *f_poXMLContext,
    __out       DRM_DWORD                *f_pcchData );



/******************************************************************************
**
** Function: _WriteTIDKeyToMeteringStore
**
** Synopsis: Stores the clear XML key, a.k.a. TIDKey, (used to encrypt/decrypt
**           the KID list in the challenge/response to/from the server) in
**           the secure store's MeteringStore.
**
** Arguments:
**
** [f_pmeterstore]     -- Pointer to the MeterStore context.
** [f_rgbPasswordSST]  -- The secure store password.
** [f_pmid]            -- Pointer to the metering ID.
** [f_prgbClearAESKey] -- Pointer to the TIDKey.
** [f_pDatastore]      -- Pointer to the data store.
**
** Returns: DRM_SUCCESS on success.
**          DRM_E_INVALIDARG if the arguments are invalid.
**
** Notes:   Must write at least 24 bytes or SST slot code will fail.
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _WriteTIDKeyToMeteringStore (
    __in                                   DRM_METERING_CONTEXT *f_pmeterstore,
    __in_bcount(DRM_SHA1_DIGEST_LEN) const DRM_BYTE              f_rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                             const DRM_MID              *f_pmid,
    __in_bcount(SIZEOF(_AES_KEY))    const DRM_BYTE             *f_prgbClearAESKey,
    __in                                   DRM_DST              *f_pDatastore )
{
    DRM_RESULT dr = DRM_SUCCESS;


    ChkDR( DRM_SST_SetData( f_pmeterstore,
                            f_pmid,
                           &g_kidTIDKey,
                            f_rgbPasswordSST,
                            SECURE_STORE_METERING_DATA,
                            f_pDatastore,
                            f_prgbClearAESKey,
                            SIZEOF( _AES_KEY ) ) );
ErrorExit:
    return dr;
}



/******************************************************************************
**
** Function: _ReadTIDKeyFromMeteringStore
**
** Synopsis: Retrieves the clear XML key, a.k.a. TIDKey, (used to encrypt/decrypt
**           the KID list in the challenge/response to/from the server) from
**           the secure store's MeteringStore.
**
** Arguments:
**
** [f_pmeterstore]     -- Pointer to the MeterStore context.
** [f_rgbPasswordSST]  -- The secure store password.
** [f_pmid]            -- Pointer to the metering ID.
** [f_prgbClearAESKey] -- Pointer to a buffer to store the TIDKey.
** [f_pDatastore]      -- Pointer to the data store.
**
** Returns: DRM_SUCCESS on success.
**          DRM_E_INVALIDARG if the arguments are invalid.
**
** Notes:   none.
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _ReadTIDKeyFromMeteringStore (
    __in                                   DRM_METERING_CONTEXT *f_pmeterstore,
    __in_bcount(DRM_SHA1_DIGEST_LEN) const DRM_BYTE              f_rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                             const DRM_MID              *f_pmid,
    __out_bcount(sizeof(_AES_KEY))         DRM_BYTE             *f_prgbClearAESKey,
    __in                                   DRM_DST              *f_pDatastore )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  cbSlot = SIZEOF( _AES_KEY );


    ChkDRMap( DRM_SST_GetData( f_pmeterstore,
                               f_pmid,
                              &g_kidTIDKey,
                               f_rgbPasswordSST,
                               SECURE_STORE_METERING_DATA,
                               f_pDatastore,
                               f_prgbClearAESKey,
                              &cbSlot ),
              DRM_E_DST_SLOT_NOT_FOUND,
              DRM_E_METERSTORE_DATA_NOT_FOUND );

ErrorExit:
    return dr;
}



/*********************************************************************
**
** Function: DRM_MTR_ParseBinaryMeterCert
**
** Synopsis: Function that parses a binary meter certificate to retrieve
**           the embedded meter ID (MID).
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDSTContext]         -- Pointer to a data store context.
** [f_poSSTContext]         -- Pointer to a secure store context.
** [f_poRevStore]           -- Pointer to a revocation store context.
** [f_pbRevocationBuffer]   -- Pointer to a revocation buffer.
** [f_cbRevocationBuffer]   -- Size of the revocation buffer.
** [f_pbMeterCert]          -- Pointer to a buffer that contains the
**                             binary meter certificate.
** [f_cbMeterCert]          -- Size (number of bytes) of a buffer that
**                             contains the binary meter certificate.
** [f_poMID]                -- Pointer to a variable to received the
**                             parsed out meter ID (MID).
** [f_pchURL]               -- Pointer to a buffer to receive a parsed out
**                             URL. If it is NULL, the function returns an
**                             error and the size of the required buffer
**                             is stored in *f_pcchURL (if f_pcchURL is
**                             not NULL).
** [f_pcchURL]              -- Pointer to a variable that contains the size
**                             (in characters) of the URL buffer during input
**                             and receives the size (in characters) of the
**                             URL during output. if it is NULL, URL will
**                             not be parsed out from the meter certificate.
** [f_poServerPubKey]       -- Pointer to a variable to receive the server
**                             public key from the meter certificate.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the URL buffer is not
**                          big enough.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_ParseBinaryMeterCert(
    __inout                            DRM_BB_CONTEXT               *f_poBBXContext,
    __inout                            DRM_DST                      *f_poDSTContext,
    __inout                            DRM_SECSTORE_CONTEXT         *f_poSSTContext,
    __inout_opt                        DRM_REVOCATIONSTORE_CONTEXT  *f_poRevStore,
    __in_bcount_opt(f_cbRevocationBuffer)
                                       DRM_BYTE                     *f_pbRevocationBuffer,
    __in                               DRM_DWORD                     f_cbRevocationBuffer,
    __in_bcount(f_cbMeterCert)   const DRM_BYTE                     *f_pbMeterCert,
    __in                         const DRM_DWORD                     f_cbMeterCert,
    __out                              DRM_MID                      *f_poMID,
    __out_ecount_opt(*f_pcchURL)       DRM_CHAR                     *f_pchURL,
    __inout_opt                        DRM_DWORD                    *f_pcchURL,
    __out                              PUBKEY_P256                  *f_poServerPubKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwOffset = 0;
    DRM_BCERT_CHAIN_HEADER oChainHeader = { 0 };
    DRM_BCERT_CERTIFICATE oCert = { 0 };
    DRM_DWORD cbCRL = 0;
    DRMFILETIME ftCurrentTime = {0};
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC_DRM_MTR_ParseBinaryMeterCert );

    ChkArg( f_pbMeterCert != NULL && f_cbMeterCert > 0 );
    ChkArg( f_poMID != NULL );
    ChkArg( ( f_pchURL == NULL && f_pcchURL== NULL ) ||
            ( f_pchURL == NULL && f_pcchURL!= NULL && *f_pcchURL == 0 ) ||
            ( f_pchURL != NULL && f_pcchURL!= NULL && *f_pcchURL > 0 ) );
    ChkArg( f_poServerPubKey != NULL );

    OEM_ECC_ZeroPublicKey_P256( f_poServerPubKey );

    /*
    ** Verifying certificate chain if it's trusted.
    */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );
    ChkDR( DRM_BCert_InitVerificationContext( &ftCurrentTime,
                                              (const PUBKEY_P256*) g_rgbMSPlayReadyRootIssuerPubKey,
                                              DRM_BCERT_CERTTYPE_METERING,
                                              &f_poBBXContext->CryptoContext,
                                              TRUE, /* Verify signatures */
                                              FALSE,
                                              NULL,
                                              0,
                                              FALSE,    /* do not collect errors, just stop parsing */
                                              f_poBBXContext,
                                              f_poSSTContext,
                                              f_poDSTContext,
                                              &oVerificationContext ) );

    ChkDR( DRM_BCert_ParseCertificateChain( f_pbMeterCert, f_cbMeterCert, &oVerificationContext ) );

    /*
    ** Retrieve leaf certificate and read metering URL from it.
    */
    ChkDR( DRM_BCert_GetChainHeader( f_pbMeterCert,
                                     f_cbMeterCert,
                                     &dwOffset,
                                     &oChainHeader ) );

    ChkDR( DRM_BCert_GetCertificate( f_pbMeterCert,
                                     f_cbMeterCert,
                                     &dwOffset,
                                     &oCert,
                                     DRM_BCERT_CERTTYPE_METERING ) );

    MEMCPY( f_poMID,
            &oCert.Type.MeteringInfo.MeteringID,
            SIZEOF( DRM_MID ) );

    if ( f_pcchURL != NULL)
    {
        if ( f_pchURL == NULL ||
             *f_pcchURL < oCert.Type.MeteringInfo.MeteringURL.cb )
        {
            *f_pcchURL = oCert.Type.MeteringInfo.MeteringURL.cb;

            return DRM_E_BUFFERTOOSMALL;
        }

        MEMCPY( f_pchURL,
                oCert.Type.MeteringInfo.MeteringURL.rgb,
                oCert.Type.MeteringInfo.MeteringURL.cb );

        *f_pcchURL = oCert.Type.MeteringInfo.MeteringURL.cb;
    }

    if( DRM_METERCERTREVOCATION_IsMeterCertRevocationSupported() )
    {
        /*
        ** Retrieve public key from the leaf certificate
        */
        ChkDR( DRM_BCert_GetPublicKey( f_pbMeterCert,
                                       f_cbMeterCert,
                                       0,
                                       f_poServerPubKey ) );

        /*
        ** Check that the metering certificate wasn't revoked
        */
        ChkArg( f_poRevStore != NULL );
        dr = DRM_RVS_GetRevocationData( f_poRevStore, &g_guidRevocationTypePlayReadySilverLightRuntime, NULL, &cbCRL );

        if ( dr == DRM_E_BUFFERTOOSMALL )
        {
            /* Retrieves CRL, then check if revoked */
            cbCRL = f_cbRevocationBuffer;
            ChkDR( DRM_RVS_GetRevocationData( f_poRevStore, &g_guidRevocationTypePlayReadySilverLightRuntime, f_pbRevocationBuffer, &cbCRL ) );

            /*
            ** Check if metercert certificate is revoked.
            */
            ChkDR( DRM_BCrl_CheckIfPlayReadyCertRevoked( f_pbMeterCert, f_cbMeterCert, f_pbRevocationBuffer, cbCRL, NULL, NULL, &f_poBBXContext->CryptoContext, f_poBBXContext, f_poSSTContext, f_poDSTContext ) );
        }
    }

    {
        if( dr == DRM_E_DST_SLOT_NOT_FOUND ) /* Slot not found means there's just no CRL available which is fine */
        {
            dr = DRM_SUCCESS;
        }
    }
    ChkDR( dr );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************
**
** Function: _CalcKIDDataCharCount
**
** Synopsis: Function that calculates the character count of portion
**           of the meter data challenge that contains meter data of
**           a single KID.
**
** Arguments:
**
** [f_pbData]               -- Pointer to a buffer that contains the meter
**                             data of a KID.
** [f_cbData]               -- Size (number of bytes) of the buffer that
**                             contains the meter data of a KID.
** [f_pchCount]             -- Pointer to a variable that receives the
**                             calculated character count.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: Meter data of each KID may have data elements of many actions.
** For each action data element, the first portion is the string (prefixed
** by the length of the string), and the second portion is a secure store
** token that contains the meter count.
**
** The size of each action data element must be an even number of bytes.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CalcKIDDataCharCount(
    __in_bcount(f_cbData) const DRM_BYTE  *f_pbData,
    __in                  const DRM_DWORD  f_cbData,
    __out                       DRM_DWORD *f_pcchCount )
{
    DRM_RESULT     dr       = DRM_SUCCESS;
    DRM_WCHAR      rgwchScratch[ MAX_CCH_BASE10_DWORD_STRING ] = { 0 };
    DRM_DWORD      cchCount = 0;
    DRM_DWORD      ibCurr   = 0;
    DRM_WORD       cbAction = 0;
    PERSISTEDTOKEN oToken   = { 0 };
    DRM_DWORD      cchValue = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__CalcKIDDataCharCount );

    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );
    ChkArg( f_pcchCount != NULL );

    /* <Record KID="xxxxxxxxxxxxxxxxxxxxxxx"> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataRecordTag.cchString,
                                             0,
                                             g_dastrMeterDataRecordAttribName.cchString,
                                             CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) );

    for ( ; ; )
    {
        if ( ibCurr + SIZEOF( DRM_WORD) > f_cbData )
        {
            break;
        }

        /* Extract the size of the action string. */
        DRM_BYT_CopyBytes( &cbAction, 0, f_pbData, ibCurr, SIZEOF( DRM_WORD ) );

        ibCurr += SIZEOF( DRM_WORD );

        FIX_ENDIAN_WORD( cbAction );

        if ( ibCurr + cbAction > f_cbData )
        {
            break;
        }

        /* Skip the action string. */
        ibCurr += cbAction;

        if ( ibCurr + SIZEOF( PERSISTEDTOKEN ) > f_cbData )
        {
            break;
        }

        /* Extract the token that contains the meter data. */
        DRM_BYT_CopyBytes( &oToken.TokenType, 0, f_pbData, ibCurr, SIZEOF( DRM_DWORD ) );

        FIX_ENDIAN_DWORD( oToken.TokenType );

        ChkBOOL( oToken.TokenType == TOKEN_LONG, DRM_E_WRONG_TOKEN_TYPE );

        DRM_BYT_CopyBytes( &oToken.val.lValue, 0, f_pbData, ibCurr + SIZEOF( DRM_DWORD ), SIZEOF( DRM_LONG ) );

        FIX_ENDIAN_LONG( oToken.val.lValue );

        ibCurr += SIZEOF( PERSISTEDTOKEN );

        ChkDR( DRM_UTL_NumberToString( (DRM_DWORD)oToken.val.lValue,
                                       rgwchScratch,
                                       MAX_CCH_BASE10_DWORD_STRING,
                                       0,
                                       10,
                                       &cchValue ) );

        /* <ACTION Name="xxxx" Value="yyyy" /> */
        cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataActionTag.cchString,
                                                 0,
                                                 g_dastrMeterDataActionAttrib1Name.cchString,
                                                 cbAction );

        cchCount += DRM_XMB_RequiredCharsForTag( 0,
                                                 0,
                                                 g_dastrMeterDataActionAttrib2Name.cchString,
                                                 cchValue );
    }

ErrorExit:

    if ( f_pcchCount != NULL )
    {
        *f_pcchCount = cchCount;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _CalcFixedUnencryptedChallengeCharCount
**
** Synopsis: Function that calculates the character count of portion
**           of the meter data challenge that contains the fixed portion
**           of the unencrypted XML segment. "Fixed" means the size does
**           not change no matter how many slots are in the meter store.
**
** Arguments:
** [f_poChallengeContext]   -- Pointer to a meter data challenge data
**                             structure that contains all information
**                             needed to constructs a meter data challenge.
** [f_pchCount]             -- Pointer to a variable that receives the
**                             calculated character count.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CalcFixedUnencryptedChallengeCharCount(
    __in  const _METER_CHALLENGE_CONTEXT *f_poChallengeContext,
    __out       DRM_DWORD                *f_pcchCount )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchCount = 0;
    DRM_DWORD  cchCount1 = 0;
    DRM_DWORD  cchEncodedCustomData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__CalcFixedUnencryptedChallengeCharCount );

    ChkArg( f_poChallengeContext != NULL );
    ChkArg( f_poChallengeContext->m_poBBXContext != NULL );
    ChkArg( f_pcchCount != NULL );

#if DRM_USE_SOAP

    /* Add the character count of the XML root header (<?xml...>). */
    cchCount += g_dastrXMLRootTag.cchString;

    /* Add the character count of the SOAP header. */
    cchCount += DRM_SOAPXML_CalcSOAPHeaderCharCount();

#endif

    /* <ProcessMeteringData> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataRootTag.cchString,
                                             0,
                                             g_dastrMeterDataRootAttribName.cchString,
                                             g_dastrMeterDataRootAttribValue.cchString );

    /* <challenge> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataChallenge1Tag.cchString,
                                             0,
                                             0,
                                             0 );

    /* <Challenge> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataChallenge2Tag.cchString,
                                             0,
                                             g_dastrMeterDataChallenge2AttribName.cchString,
                                             g_dastrMeterDataChallenge2AttribValue.cchString );

    /* <MeteringData> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataMeteringDataTag.cchString,
                                             0,
                                             g_dastrMeterDataMeteringDataAttrib1Name.cchString,
                                             g_dastrMeterDataMeteringDataAttrib1Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForTag( 0,
                                             0,
                                             g_dastrMeterDataMeteringDataAttrib2Name.cchString,
                                             g_dastrMeterDataMeteringDataAttrib2Value.cchString );

    /* <Version> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataVersionTag.cchString,
                                             g_dastrMeterDataVersionValue.cchString,
                                             0,
                                             0 );

    /* <MID> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataMIDTag.cchString,
                                             CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ),
                                             0,
                                             0 );

    /* <TID> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataTIDTag.cchString,
                                             CCH_BASE64_EQUIV( SIZEOF( DRM_TID ) ),
                                             0,
                                             0 );

    /* <PARTIALDATA> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataPartialDataTag.cchString,
                                             g_dastrMeterDataPartialDataValue.cchString,
                                             0,
                                             0 );

    /* <SupportsMeterCertSignature> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrSupportsMeterCertSignatureTag.cchString,
                                             g_dastrTrueValue.cchString,
                                             0,
                                             0 );

    /* Add the character count of client information XML segment. */
    cchCount += DRM_SOAPXML_CalcClientInfo(f_poChallengeContext->m_poBBXContext->pClientInfo);


    /* Get the size (number of characters) of the XML encoded custom data. */
    ChkDR( DRM_SOAPXML_EncodeData( f_poChallengeContext->m_pchCustomData,
                                   f_poChallengeContext->m_cchCustomData,
                                   NULL,
                                   &cchEncodedCustomData ) );

    if ( cchEncodedCustomData > 0 )
    {
        /* <CustomData> */
        cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataCustomDataTag.cchString,
                                                 cchEncodedCustomData,
                                                 0,
                                                 0 );

        /* Possible alignment cost. */
        cchCount += SIZEOF( DRM_WORD );
    }


    /* Add the character count of the signature. */
    ChkDR( DRM_XMLSIG_GetSignatureNodeCharCount( &cchCount1 ));

    cchCount += cchCount1;

    /* Add space used by XML builder context. */
    cchCount += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realignment.
    */
    cchCount += SIZEOF( DRM_DWORD );

ErrorExit:

    if ( f_pcchCount != NULL )
    {
        *f_pcchCount = cchCount;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _CalcFixedEncryptedChallengeCharCount
**
** Synopsis: Function that calculates the character count of portion
**           of the meter data challenge that contains the fixed portion
**           of the encrypted XML segment. "Fixed" means the size does
**           not change no matter how many slots are in the meter store.
**
** Arguments:
**
** [f_poChallengeContext]   -- Pointer to a meter data challenge data
**                             structure that contains all information
**                             needed to constructs a meter data challenge.
** [f_pchCount]             -- Pointer to a variable that receives the
**                             calculated character count.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CalcFixedEncryptedChallengeCharCount(
    __in  const _METER_CHALLENGE_CONTEXT *f_poChallengeContext,
    __out       DRM_DWORD                *f_pcchCount )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__CalcFixedEncryptedChallengeCharCount );

    ChkArg( f_poChallengeContext != NULL );
    ChkArg( f_pcchCount != NULL );

    /* <Data> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataDataTag.cchString,
                                             0,
                                             0,
                                             0 );

    /* <Records> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrMeterDataRecordsTag.cchString,
                                             0,
                                             0,
                                             0 );

    /* Add the B64 encoded character count of the device certificate. */
    cchCount += CCH_BASE64_EQUIV( f_poChallengeContext->m_cbDeviceCert );

    ChkDR( DRM_XMLSIG_GetEncryptedDataNodeCharCount( &f_poChallengeContext->m_oXMLKey,
                                                     cchCount,
                                                     &cchCount ) );

ErrorExit:

    if ( f_pcchCount != NULL )
    {
        *f_pcchCount = cchCount;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _CalcVariableChallengeCharCount
**
** Synopsis: Function that calculates the character count of portion
**           of the meter data challenge that contains the actual meter
**           data of all KIDs.
**
** Arguments:
**
** [f_poChallengeContext]   -- Pointer to a meter data challenge data
**                             structure that contains all information
**                             needed to constructs a meter data challenge.
** [f_pchCount]             -- Pointer to a variable that receives the
**                             calculated character count.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CalcVariableChallengeCharCount(
    __in  _METER_CHALLENGE_CONTEXT *f_poChallengeContext,
    __out DRM_DWORD                *f_pcchCount )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__CalcVariableChallengeCharCount );

    ChkArg( f_poChallengeContext != NULL );
    ChkArg( f_pcchCount != NULL );

    /* Calculate the character count of meter data of all KIDs. */
    ChkDR( _ProcessAllKIDs( f_poChallengeContext, NULL, &cchCount ) );

ErrorExit:

    if ( f_pcchCount != NULL )
    {
        *f_pcchCount = cchCount;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _CalcMaximumChallengeCharCount
**
** Synopsis: Function that calculates the character count of the meter
**           data challenge, depending on whether the actual meter data
**           of all KIDs is included.
**
** Arguments:
**
** [f_poChallengeContext]   -- Pointer to a meter data challenge data
**                             structure that contains all information
**                             needed to constructs a meter data challenge.
** [f_fKIDData]             -- Flag indicating whether to count the meter
**                             data of all KIDs.
** [f_pcchChallenge]        -- Pointer to a variable that receives the
**                             calculated character count.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CalcMaximumChallengeCharCount(
    __in        _METER_CHALLENGE_CONTEXT *f_poChallengeContext,
    __in  const DRM_BOOL                  f_fKIDData,
    __out       DRM_DWORD                *f_pcchChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;
    DRM_DWORD cchCount1 = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__CalcMaximumChallengeCharCount );

    ChkArg( f_poChallengeContext != NULL );
    ChkArg( f_pcchChallenge != NULL );

    /* Calculate the character count of the fixed unencrypted portion. */
    ChkDR( _CalcFixedUnencryptedChallengeCharCount( f_poChallengeContext,
                                                    &f_poChallengeContext->m_cchFixedUnencrypted ) );

    cchCount += f_poChallengeContext->m_cchFixedUnencrypted;

    /* Calculate the character count of the fixed encrypted portion. */
    ChkDR( _CalcFixedEncryptedChallengeCharCount( f_poChallengeContext,
                                                  &f_poChallengeContext->m_cchFixedEncrypted ) );

    cchCount += f_poChallengeContext->m_cchFixedEncrypted;

    if ( f_fKIDData )
    {
        /*
        ** Calculate the character count of the variable portion that
        ** contains meter data of all KIDs.
        */
        ChkDR( _CalcVariableChallengeCharCount( f_poChallengeContext,
                                                &cchCount1 ) );

        cchCount += cchCount1;
    }

ErrorExit:

    if( f_pcchChallenge != NULL )
    {
        *f_pcchChallenge = cchCount;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _CalcMeterCertChallengeCharCount
**
** Synopsis: Function that calculates the character count of the meter
**           certificate challenge, based on the count of the custom
**           data.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to the BBX Context
** [f_cchCustomData]        -- Size (number of characters) of a custom
**                             data buffer.
** [f_pcchChallenge]        -- Pointer to a variable that receives the
**                             calculated character count.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CalcMeterCertChallengeCharCount(
    __in const DRM_BB_CONTEXT   *f_poBBXContext,
    __in const DRM_DWORD         f_cchCustomData,
    __out      DRM_DWORD        *f_pcchChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pcchChallenge != NULL );

#if DRM_USE_SOAP

    /* Add the character count of the XML root header (<?xml...>). */
    cchCount += g_dastrXMLRootTag.cchString;

    /* Add the character count of the SOAP header. */
    cchCount += DRM_SOAPXML_CalcSOAPHeaderCharCount();

#endif

    /* <GetMeteringCertificate> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMeterCertRootTag.cchString,
                                              0,
                                              g_dastrMeterCertRootAttribName.cchString,
                                              g_dastrMeterCertRootAttribValue.cchString );

    /* <challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMeterCertChallenge1Tag.cchString,
                                              0,
                                              0,
                                              0 );
    /* <Challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMeterCertChallenge2Tag.cchString,
                                              0,
                                              0,
                                              0 );
    /* <MeterCert> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMeteringMeterCertTag.cchString,
                                              0,
                                              0,
                                              0 );
    /* <Version> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMeterCertVersionTag.cchString,
                                              g_dastrMeterCertVersion.cchString,
                                              0,
                                              0 );

    /* <MID> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMeterCertMIDTag.cchString,
                                              CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ),
                                              0,
                                              0 );

    /* Add the character count of client information XML segment. */
    cchCount += DRM_SOAPXML_CalcClientInfo(f_poBBXContext->pClientInfo);


    if ( f_cchCustomData > 0 )
    {
        /* <CustomData> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMeterCertCustomDataTag.cchString,
                                                  f_cchCustomData,
                                                  0,
                                                  0 );

        /* Possible alignment cost. */
        cchCount += SIZEOF( DRM_WORD );
    }

    /* Add space used by XML builder context. */
    cchCount += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realignment.
    */
    cchCount += SIZEOF( DRM_DWORD );

    *f_pcchChallenge = cchCount;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _BuildKIDDataXML
**
** Synopsis: Function that constructs a sub XML segment that contains
**           meter data of a single KID using the XML builder.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_poKID]                -- Pointer to a KID whose meter data is to be
**                             stored.
** [f_pbData]               -- Pointer to a buffer that contains the meter
**                             data of a KID.
** [f_cbData]               -- Size (number of bytes) of the buffer that
**                             contains the meter data of a KID.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: Meter data of each KID may have data elements of many actions.
** For each action data element, the first portion is the string (prefixed
** by the length of the string), and the second portion is a secure store
** token that contains the meter count.
**
** The size of each action data element must be an even number of bytes.
** See comments below for details.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildKIDDataXML(
    __inout                     _XMBContextA *f_poXMLContext,
    __in                  const DRM_KID      *f_poKID,
    __in_bcount(f_cbData)       DRM_BYTE     *f_pbData,
    __in                  const DRM_DWORD     f_cbData )
{
    DRM_RESULT            dr          = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING dastrKID    = EMPTY_DRM_STRING;
    DRM_BYTE              rgbKID[ CCH_BASE64_EQUIV( ( SIZEOF( DRM_KID ) ) ) ] = { 0 };
    DRM_DWORD             cbKID       = SIZEOF( rgbKID );
    DRM_WCHAR             rgwchScratch[ MAX_CCH_BASE10_DWORD_STRING ] = { 0 };
    DRM_DWORD             ibCurr      = 0;
    DRM_BYTE             *pbAction    = NULL;
    DRM_WORD              cbAction    = 0;
    PERSISTEDTOKEN        oToken      = { 0 };
    DRM_DWORD             cchValue    = 0;
    DRM_ANSI_CONST_STRING dastrAction = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING dastrValue  = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__BuildKIDDataXML );

    ChkArg( f_poXMLContext != NULL );
    ChkArg( f_poKID != NULL );
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );

    ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )f_poKID,
                            SIZEOF( DRM_GUID ),
                            ( DRM_CHAR * )rgbKID,
                            &cbKID,
                            0 ) );

    DRMASSERT( cbKID == SIZEOF( rgbKID ) );

    DASTR_FROM_PB( &dastrKID, rgbKID, cbKID );

    /* <Record> */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMeterDataRecordTag,
                              NULL,
                              &g_dastrMeterDataRecordAttribName,
                              &dastrKID,
                              wttOpen ) );

    for ( ; ; )
    {
        if ( ibCurr + SIZEOF( DRM_WORD) > f_cbData )
        {
            break;
        }

        /* Extract the size of the action string. */
        DRM_BYT_CopyBytes( &cbAction, 0, f_pbData, ibCurr, SIZEOF( DRM_WORD ) );

        ibCurr += SIZEOF( DRM_WORD );

        /*
        ** After the endian fix, cbAction must be an even number since the
        ** action string is stored as Unicode.
        */
        FIX_ENDIAN_WORD( cbAction );

        if ( ibCurr + cbAction > f_cbData )
        {
            break;
        }

        /* pbAction points to the start of the action string. */
        pbAction = f_pbData + __CB_DECL( ibCurr );

        /* Convert action string from Unicode to UTF8/ANSI in place. */
        DRM_UTL_DemoteUNICODEtoASCII( ( const DRM_WCHAR * )pbAction,
                                      ( DRM_CHAR * )pbAction,
                                      cbAction / SIZEOF( DRM_WCHAR ) );

        DASTR_FROM_PB( &dastrAction, pbAction, cbAction / SIZEOF( DRM_WCHAR ) );

        ibCurr += cbAction;

        if ( ibCurr + SIZEOF( PERSISTEDTOKEN ) > f_cbData )
        {
            break;
        }

        /*
        ** The size of a token should be an even number of bytes.
        ** See comment in drmtoken.h for details.
        */
        DRM_BYT_CopyBytes( &oToken.TokenType, 0, f_pbData, ibCurr, SIZEOF( DRM_DWORD ) );

        FIX_ENDIAN_DWORD( oToken.TokenType );

        ChkBOOL( oToken.TokenType == TOKEN_LONG, DRM_E_WRONG_TOKEN_TYPE );

        DRM_BYT_CopyBytes( &oToken.val.lValue, 0, f_pbData, ibCurr + SIZEOF(DRM_DWORD), SIZEOF( DRM_LONG ) );

        FIX_ENDIAN_LONG( oToken.val.lValue );

        ibCurr += SIZEOF( PERSISTEDTOKEN );

        ChkDR( DRM_UTL_NumberToString( (DRM_DWORD)oToken.val.lValue,
                                       rgwchScratch,
                                       MAX_CCH_BASE10_DWORD_STRING,
                                       0,
                                       10,
                                       &cchValue ) );

        DRMASSERT( cchValue <= MAX_CCH_BASE10_DWORD_STRING );

        /* Convert value string from Unicode to UTF8/ANSI. */
        DRM_UTL_DemoteUNICODEtoASCII( rgwchScratch,
                                      ( DRM_CHAR * )rgwchScratch,
                                      cchValue );

        DASTR_FROM_PB( &dastrValue, (DRM_BYTE * )rgwchScratch, cchValue );

        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrMeterDataActionTag,
                                  NULL,
                                  &g_dastrMeterDataActionAttrib1Name,
                                  &dastrAction,
                                  wttOpen ) );

        ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                      &g_dastrMeterDataActionAttrib2Name,
                                      &dastrValue ) );

        /* </Action> */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }

    /* </Record> */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _ProcessAllKIDs
**
** Synopsis: Function that enumerates slots of a specific meter ID (MID)
**           in the meter store and does one of the following tasks:
**           1. Accumulates the character count required to store the
**              meter data of all KIDs.
**           2. Stores as much meter data as possible using the XML
**              builder. The size limit is provided by a field of the
**              passed in data structure (f_poChallengeContext->m_cchVaried).
**
** Arguments:
**
** [f_poChallengeContext]   -- Pointer to a meter data challenge data
**                             structure that contains all information
**                             needed to constructs a meter data challenge.
** [f_poXMLContext]         -- Pointer to a XML builder context if the caller
**                             wants the function to actually stores meter
**                             data. If it is NULL, the caller just wants
**                             to know the size needed to stores meter data
**                             of all KIDs.
** [f_pcchData]             -- Pointer to a variable that is used to store
**                             the character count needed to store meter
**                             data of all KIDs being processed.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer in the XML
**                          builder is not big enough to store meter data
**                          of even a single KID.
**                          DRM_E_NOMORE if no KID is ready to be processed.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ProcessAllKIDs(
    __in        _METER_CHALLENGE_CONTEXT *f_poChallengeContext,
    __inout_opt _XMBContextA             *f_poXMLContext,
    __out       DRM_DWORD                *f_pcchData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_METERINGENUM_CONTEXT oMeterEnumContext = { 0 };
    DRM_KID oKID = { 0 };
    DRM_BYTE *pbData = NULL;
    DRM_DWORD cbData = 0;
    DRM_DWORD cchData = 0;
    DRM_DWORD cchRemaining = 0;
    DRM_BOOL fBuildXML = FALSE;
    DRM_BOOL fHasData = FALSE;
    DRM_BOOL fUseData = FALSE;
    DRM_BOOL fPartialData = TRUE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__ProcessAllKIDs );

    ChkArg( f_poChallengeContext != NULL );
    ChkArg( f_pcchData != NULL );

    /*
    ** Rereieves the size that can be used to store the encrypted
    ** meter data from a field of the passed in data structure.
    */
    cchRemaining = f_poChallengeContext->m_cchVaried;

    /*
    ** Set the flag indicating whether to actually store the meter
    ** data.
    */
    fBuildXML = ( f_poXMLContext != NULL );

    ChkDR( DRM_MTR_InitEnum( &f_poChallengeContext->m_oMID,
                             &oMeterEnumContext,
                             f_poChallengeContext->m_poDSTContext ) );

    *f_pcchData = 0;

    while ( DRM_SUCCEEDED( dr) )
    {
        dr = DRM_MTR_EnumNext( &oMeterEnumContext, &oKID, &cbData );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;

            /*
            ** Set the flag to indicate that all meter data has been
            ** processed since the end of enumeration is reached.
            */
            fPartialData = FALSE;

            break;
        }
        else
        {
            ChkDR( dr );
        }

        if ( MEMCMP( &oKID, &g_kidTID, SIZEOF( DRM_KID ) ) == 0 ||
             MEMCMP( &oKID, &g_kidTIDPrev, SIZEOF( DRM_KID ) ) == 0 )
        {
            /* Skip entries that are used to store the transaction IDs (TIDs). */
            continue;
        }

        /*
        ** Enumerate the next KID and load the meter data into the buffer
        ** that is part of the meter enumeration context.
        */
        dr = DRM_MTR_EnumLoadCurrent( &oMeterEnumContext,
                                      &f_poChallengeContext->m_oMeterStoreContext,
                                      f_poChallengeContext->m_rgbSSTPassword,
                                      &oKID,
                                      &cbData );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;

            /*
            ** Set the flag to indicate that all meter data has been
            ** processed since the end of enumeration is reached.
            */
            fPartialData = FALSE;

            break;
        }
        else
        {
            ChkDR( dr );
        }

        /*
        ** Load the meter data of the currently enumerated KID from
        ** the meter store. A stack allocated buffer is pointed by
        ** pbData and its size is stored in cbData after the call of
        ** DRM_MTR_GetKIDData().
        */
        if( pbData != NULL )
        {
            /*
            ** Caller, that is us, needs to free result of DRM_MTR_GetKIDData.
            ** So, free memory from the previous loop iteration.
            */
            ChkDR( DRM_STK_Free( &f_poChallengeContext->m_oStackContext, pbData ) );
            pbData = NULL;
        }
        ChkDR( DRM_MTR_GetKIDData( &f_poChallengeContext->m_oMeterStoreContext,
                                   &f_poChallengeContext->m_oStackContext,
                                   &pbData,
                                   &cbData ) );

        if ( cbData == 0 || pbData == NULL )
        {
            /*
            ** If there is no meter data for the currently enumerated
            ** KID in the meter store, just continue the enumeration.
            */
            continue;
        }

        fHasData = TRUE;

        /*
        ** Calculate the character count of the encrypted meter data
        ** of the currently enumerated KID.
        */
        ChkDR( _CalcKIDDataCharCount( pbData, cbData, &cchData ) );

        if ( fBuildXML )
        {
            if ( cchRemaining < CCH_BASE64_EQUIV( cchData ) )
            {
                /*
                ** The remaining size is not big enough to fit the encrypted
                ** meter data of the currently enumerated KID, need to
                ** break away from the loop.
                */
                break;
            }

            /*
            ** Constructs the meter data of the currently enumerated KID
            ** using the passed in XML builder.
            */
            ChkDR( _BuildKIDDataXML( f_poXMLContext,
                                     &oKID,
                                     pbData,
                                     cbData ) );

            /* Deduct the size available to store the encrypted meter data. */
            cchRemaining -= CCH_BASE64_EQUIV( cchData );
        }

        /*
        ** Accumulate the character count of the encrypted meter data.
        ** If the size of data before XML encryption is cchData, it will be
        ** changed to CCH_BASE64_EQUIV( cchData ) after XML encryption.
        */
        *f_pcchData += CCH_BASE64_EQUIV( cchData );

        /*
        ** Set a flag indicating that at least one KID has been
        ** successfully processed.
        */
        fUseData = TRUE;
    }

    /*
    ** Set a flag in the passed in data struture indicating whether meter
    ** data of all KIDs has been processed.
    */
    f_poChallengeContext->m_fPartialData = fPartialData;

ErrorExit:

    if( ( f_poChallengeContext != NULL )
     && ( pbData               != NULL ) )
    {
        /*
        ** Caller, that is us, needs to free result of DRM_MTR_GetKIDData.
        ** So, free memory from the last loop iteration.
        */
        DRM_RESULT dr1 = DRM_SUCCESS;
        dr1 = DRM_STK_Free( &f_poChallengeContext->m_oStackContext, pbData );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr1;
        }
    }

    if ( DRM_SUCCEEDED( dr ) && fBuildXML )
    {
        if ( fHasData )
        {
            if ( !fUseData )
            {
                /*
                ** If not even a single KID has been processed, it means that the
                ** buffer in the passed in XML builder (excluding the fixed overhead)
                ** is not big enough to store meter data of a single KID. Should
                ** return an error to the caller.
                */
                dr = DRM_E_BUFFERTOOSMALL;
            }
        }
        else
        {
            /*
            ** If no KID is to ready be processed, notify the caller about this
            ** situation.
            */
            dr = DRM_E_NOMORE;
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildMeterDataChallengeDataXML
**
** Synopsis: Function that constructs a sub XML segment to be encrypted
**           in the meter data challenge XML string using the XML builder.
**
** Arguments:
**
** [f_poChallengeContext]   -- Pointer to a meter data challenge data
**                             structure that contains all information
**                             needed to constructs a meter data challenge.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is used
**                             to receive a constructed XML segment string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:                    It is the caller's responsibility to release
**                          the memory allocated in this function. Since
**                          data contained in f_pdstrXML is stored in the
**                          memory allocated, when the memory is released,
**                          f_pdstrXML should not be used any more.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildMeterDataChallengeDataXML(
    __in  _METER_CHALLENGE_CONTEXT *f_poChallengeContext,
    __out DRM_ANSI_STRING          *f_pdastrXML )
{
    DRM_RESULT dr =DRM_SUCCESS;
    _XMBContextA *poXMLContext = NULL;
    DRM_BYTE *pbXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_DWORD cchData = 0;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrReservedBuffer = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__BuildMeterDataChallengeDataXML );

    ChkArg( f_poChallengeContext != NULL );
    ChkArg( f_pdastrXML != NULL );

    pbXMLContext = PB_DASTR( f_pdastrXML );
    cbXMLContext = CB_DASTR( f_pdastrXML );

    ChkDR( DRM_UTL_EnsureDataAlignment( pbXMLContext,
                                        cbXMLContext,
                                        ( DRM_BYTE ** )&poXMLContext,
                                        &cbXMLContext,
                                        SIZEOF( DRM_DWORD ),
                                        NULL ) );

    /*
    ** Add and open <Data> node.
    **
    ** Output:
    ** <Data>
    */
    ChkDR( DRM_XMB_CreateDocumentA( cbXMLContext,
                                    ( DRM_BYTE * )poXMLContext,
                                    &g_dastrMeterDataDataTag ) );

    ChkDR( DRM_XMB_GetXMLBaseA( poXMLContext, &pchXML ) );

    /*
    ** Add and open <Records> node.
    **
    ** Output:
    ** <Data>
    **      <DeviceCert>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterDataDeviceCertTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add the device certificate.
    **
    ** Output:
    ** <Data>
    **      <DeviceCert>
    **          device certificate
    */
    ChkDR( DRM_XMB_ReserveSpaceA( poXMLContext,
                                  SIZEOF( DRM_WORD ),
                                  CCH_BASE64_EQUIV( f_poChallengeContext->m_cbDeviceCert ),
                                  &dasstrReservedBuffer ) );

    DRM_BYT_MoveBytes( pchXML,
                       dasstrReservedBuffer.m_ich,
                       f_poChallengeContext->m_pbDeviceCert,
                       0,
                       f_poChallengeContext->m_cbDeviceCert );

    /*
    ** Since the space reserved is guaranteed to start at a DRM_WORD aligned address,
    ** using pointer arithmatic is OK.
    */
    ChkDR( DRM_B64_EncodeA( (DRM_BYTE *)pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
                            f_poChallengeContext->m_cbDeviceCert,
                            pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
                            &dasstrReservedBuffer.m_cch,
                            0 ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, NULL ) );

    /*
    ** Add and open <Records> node.
    **
    ** Output:
    ** <Data>
    **      ...
    **      <Records>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterDataRecordsTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /* Add meter data records. */
    ChkDR( _ProcessAllKIDs( f_poChallengeContext,
                            poXMLContext,
                            &cchData ) );

    /*
    ** Close all nodes that are still open.
    **
    ** Output:
    ** <Data>
    **  ...
    ** </Data>
    */
    ChkDR( DRM_XMB_CloseDocumentA( poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

    ChkDR( DRM_SOAPXML_PadANSIString( f_pdastrXML, DRM_AES_BLOCKLEN ) );

    DRM_BYT_MoveBytes( pbXMLContext, 0, pchXML, 0, f_pdastrXML->cchString );

    f_pdastrXML->pszString = ( DRM_CHAR * )pbXMLContext;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildMeterDataChallengeXML
**
** Synopsis: Function that constructs a meter data challenge XML string
**           using the XML builder.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_cbXMLContext]         -- Size (number of bytes) of internal buffer
**                             of a XML builder context.
** [f_poChallengeContext]   -- Pointer to a meter data challenge data
**                             structure that contains all information
**                             needed to constructs a meter data challenge.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is
**                             used to receive a constructed meter
**                             data challenge XML string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildMeterDataChallengeXML(
    __inout_bcount(f_cbXMLContext)       DRM_BYTE                 *f_pbXMLContext,
    __in                           const DRM_DWORD                 f_cbXMLContext,
    __in                                 _METER_CHALLENGE_CONTEXT *f_poChallengeContext,
    __out                                DRM_ANSI_STRING          *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContextA *poXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_SUBSTRING dasstrSignedNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrEncryptedDataNode = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING dastrEncryptedXML = EMPTY_DRM_STRING;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;
    DRM_BYTE rgbMID[ __CB_DECL( CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ) ) ] = { 0 };
    DRM_ANSI_CONST_STRING dastrMID = EMPTY_DRM_STRING;
    DRM_BYTE rgbTID[ __CB_DECL( CCH_BASE64_EQUIV( SIZEOF( DRM_TID ) ) ) ] = { 0 };
    DRM_ANSI_CONST_STRING dastrTID = EMPTY_DRM_STRING;
    DRM_DWORD cchData = 0;
    DRM_SUBSTRING dasstrCustomData = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchCustomData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__BuildMeterDataChallengeXML );

    ChkArg( f_pbXMLContext != NULL );
    ChkArg( f_cbXMLContext > 0 );
    ChkArg( f_poChallengeContext != NULL );
    ChkArg( f_poChallengeContext->m_poBBXContext != NULL );
    ChkArg( f_pdastrXML != NULL );

    ZEROMEM( f_pbXMLContext, f_cbXMLContext );

    ChkDR( DRM_UTL_EnsureDataAlignment( f_pbXMLContext,
                                        f_cbXMLContext,
                                        ( DRM_BYTE ** )&poXMLContext,
                                        &cbXMLContext,
                                        SIZEOF( DRM_DWORD ),
                                        NULL ) );

#if DRM_USE_SOAP

    /* Add SOAP headers. */
    ChkDR( DRM_SOAPXML_BuildSOAPHeaderXML( poXMLContext, cbXMLContext ) );

    /*
    ** Add and open <ProcessMeteringData> node.
    **
    ** Output:
    ** [<SOAP headers>]
    ** <ProcessMeteringData ...>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterDataRootTag,
                              NULL,
                              &g_dastrMeterDataRootAttribName,
                              &g_dastrMeterDataRootAttribValue,
                              wttOpen ) );

#else

    /*
    ** Add and open <ProcessMeteringData> node.
    **
    ** Output:
    ** <ProcessMeteringData ...>
    */
    ChkDR( DRM_XMB_CreateDocumentA( cbXMLContext,
                                    ( DRM_BYTE * )poXMLContext,
                                    &g_dastrMeterDataRootTag ) );

    ChkDR( DRM_XMB_AddAttributeA( poXMLContext,
                                  &g_dastrMeterDataRootAttribName,
                                  &g_dastrMeterDataRootAttribValue ) );

#endif

    ChkDR( DRM_XMB_GetXMLBaseA( poXMLContext, &pchXML ) );

    /*
    ** Add and open <challenge> node.
    **
    ** Output:
    ** <ProcessMeteringData>
    **      <challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterDataChallenge1Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <Challenge> node.
    **
    ** Output:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterDataChallenge2Tag,
                              NULL,
                              &g_dastrMeterDataChallenge2AttribName,
                              &g_dastrMeterDataChallenge2AttribValue,
                              wttOpen ) );

    /*
    ** Add and open <MeteringData> node.
    **
    ** Output:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterDataMeteringDataTag,
                              NULL,
                              &g_dastrMeterDataMeteringDataAttrib1Name,
                              &g_dastrMeterDataMeteringDataAttrib1Value,
                              wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( poXMLContext,
                                  &g_dastrMeterDataMeteringDataAttrib2Name,
                                  &g_dastrMeterDataMeteringDataAttrib2Value ) );

    /*
    ** Add and close <Version> node.
    **
    ** Output:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  <Version>...</Version>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterDataVersionTag,
                              &g_dastrMeterDataVersionValue,
                              NULL,
                              NULL,
                              wttClosed ) );

    cchData = SIZEOF( rgbMID );

    ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&f_poChallengeContext->m_oMID,
                            SIZEOF( DRM_MID ),
                            ( DRM_CHAR * )rgbMID,
                            &cchData,
                            0 ) );

    DRMASSERT( cchData == CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ) );

    dastrMID.pszString = ( DRM_CHAR * )rgbMID;
    dastrMID.cchString = cchData;

    /*
    ** Add and close <MID> node.
    **
    ** Output:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  ...
    **                  <MID>...</MID>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterDataMIDTag,
                              &dastrMID,
                              NULL,
                              NULL,
                              wttClosed ) );

    cchData = SIZEOF( rgbTID );

    ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&f_poChallengeContext->m_oTID,
                            SIZEOF( DRM_TID ),
                            ( DRM_CHAR * )rgbTID,
                            &cchData,
                            0 ) );

    DRMASSERT( cchData == CCH_BASE64_EQUIV( SIZEOF( DRM_TID ) ) );

    dastrTID.pszString = ( DRM_CHAR * )rgbTID;
    dastrTID.cchString = cchData;

    /*
    ** Add and close <TID> node.
    **
    ** Output:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  ...
    **                  <TID>...</TID>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterDataTIDTag,
                              &dastrTID,
                              NULL,
                              NULL,
                              wttClosed ) );


    /*
    ** Construct the client information XML segment.
    **
    ** Output:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  <ClientInfo>
    **                      ...
    **                  </ClientInfo>
    */
    ChkDR( DRM_SOAPXML_BuildClientInfo( poXMLContext,
                                        f_poChallengeContext->m_poBBXContext->pClientInfo ) );

    if ( f_poChallengeContext->m_fPartialData )
    {
        /*
        ** Add and close <PartialData> node.
        **
        ** Output:
        ** <ProcessMeteringData>
        **      <challenge>
        **          <Challenge>
        **              <MeteringData>
        **                  ...
        **                  <PartialData>...</PartialData>
        */
        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                  &g_dastrMeterDataPartialDataTag,
                                  &g_dastrMeterDataPartialDataValue,
                                  NULL,
                                  NULL,
                                  wttClosed ) );
    }

    /*
    ** Add and close <SupportsMeterCertSignature> node.
    **
    ** Output:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  ...
    **                  <SupportsMeterCertSignature>...</SupportsMeterCertSignature>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrSupportsMeterCertSignatureTag,
                              &g_dastrTrueValue,
                              NULL,
                              NULL,
                              wttClosed ) );

    /* Estimate the size of the XML encoded custom data. */
    ChkDR( DRM_SOAPXML_EncodeData( f_poChallengeContext->m_pchCustomData,
                                   f_poChallengeContext->m_cchCustomData,
                                   NULL,
                                   &cchCustomData ) );

    if ( cchCustomData > 0  )
    {
        ChkDR( DRM_SOAPXML_EnsureTagValueNativeByteAligned( poXMLContext,
                                                            &g_dastrMeterDataCustomDataTag ) );

        /*
        ** Add and open <CustomData> node.
        **
        ** Output:
        ** <ProcessMeteringData>
        **      <challenge>
        **          <Challenge>
        **              <MeteringData>
        **                  ...
        **                  <CustomData>
        */
        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                  &g_dastrMeterDataCustomDataTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        ChkDR( DRM_XMB_ReserveSpaceA( poXMLContext,
                                      SIZEOF( DRM_BYTE ),
                                      cchCustomData,
                                      &dasstrCustomData ) );

        ChkDR( DRM_SOAPXML_EncodeData( f_poChallengeContext->m_pchCustomData,
                                       f_poChallengeContext->m_cchCustomData,
                                       pchXML + __CB_DECL( dasstrCustomData.m_ich ),
                                       &cchCustomData ) );

        /*
        ** Close <CustomData> node.
        **
        ** Output:
        ** <ProcessMeteringData>
        **      <challenge>
        **          <Challenge>
        **              <MeteringData>
        **                  ...
        **                  <CustomData>...</CustomData>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, NULL ) );
    }

    ChkDR( DRM_XMB_GetCurrentBufferPointerA( poXMLContext,
                                             TRUE,
                                             ( DRM_CHAR ** )&dastrEncryptedXML.pszString ) );

    dastrEncryptedXML.cchString = f_poChallengeContext->m_cchFixedEncrypted +
                                  f_poChallengeContext->m_cchVaried;

    ChkDR( _BuildMeterDataChallengeDataXML( f_poChallengeContext,
                                            ( DRM_ANSI_STRING * )&dastrEncryptedXML ) );

    /* AES encrypt the data portion of the meter data challenge XML. */
    ChkDR( DRM_SOAPXML_EncryptDataWithXMLKey( f_poChallengeContext->m_poBBXContext->pOEMContext,
                                              &f_poChallengeContext->m_oXMLKey,
                                              &dastrEncryptedXML,
                                              &f_poChallengeContext->m_poBBXContext->CryptoContext ) );

    /*
    ** Build <EncryptedData> node.
    **
    ** Output:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  ...
    **                  <EncryptedData>...</<EncryptedData>
    */
    ChkDR( DRM_XMLSIG_BuildEncryptedDataNode( poXMLContext,
                                              &f_poChallengeContext->m_oXMLKey,
                                              dastrEncryptedXML.cchString,
                                              &dasstrEncryptedDataNode ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, &dasstrSignedNode ) );

    /* The whole <MeteringData> node (inclusive) is signed. */
    ChkDR( DRM_XMLSIG_BuildSignatureNode( poXMLContext,
                                          f_poChallengeContext->m_poBBXContext,
                                          pchXML,
                                          &dasstrSignedNode ) );

    /*
    ** Close all nodes that are still open.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <ProcessMeteringData>
    ** ...
    ** </ProcessMeteringData>
    ** [</SOAP headers>] (optional)
    */
    ChkDR( DRM_XMB_CloseDocumentA( poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_PrependXMLRoot( f_pdastrXML ) );

#endif

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildMeterCertChallengeXML
**
** Synopsis: Function that constructs a meter certificate challenge
**           XML string using the XML builder.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to the BBX Context
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_cbXMLContext]         -- Size (number of bytes) of internal buffer
**                             of a XML builder context.
** [f_poMID]                -- Pointer to a meter ID (MID).
** [f_pchCustomData]        -- Pointer to a character buffer that contains
**                             the custom data. It can be NULL.
** [f_cchCustomData]        -- Size (number of characters) of the custom
**                             data string.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is
**                             used to receive a constructed meter
**                             certificate challenge XML string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildMeterCertChallengeXML(
    __in                                   DRM_BB_CONTEXT  *f_poBBXContext,
    __inout_bcount(f_cbXMLContext)         DRM_BYTE        *f_pbXMLContext,
    __in                             const DRM_DWORD        f_cbXMLContext,
    __in                             const DRM_MID         *f_poMID,
    __in_ecount_opt(f_cchCustomData) const DRM_CHAR        *f_pchCustomData,
    __in                             const DRM_DWORD        f_cchCustomData,
    __out                                  DRM_ANSI_STRING *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContextA *poXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;
    DRM_BYTE rgbMID[ __CB_DECL( CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ) ) ] = { 0 };
    DRM_ANSI_CONST_STRING dastrMID = EMPTY_DRM_STRING;
    DRM_DWORD cchData = 0;
    DRM_SUBSTRING dasstrCustomData = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchCustomData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__BuildMeterCertChallengeXML );

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pbXMLContext != NULL );
    ChkArg( f_cbXMLContext > 0 );
    ChkArg( f_poMID != NULL );
    ChkArg( ( f_pchCustomData == NULL && f_cchCustomData == 0 ) ||
            ( f_pchCustomData != NULL && f_cchCustomData > 0 ) );
    ChkArg( f_pdastrXML != NULL );

    ZEROMEM( f_pbXMLContext, f_cbXMLContext );

    ChkDR( DRM_UTL_EnsureDataAlignment( f_pbXMLContext,
                                        f_cbXMLContext,
                                        ( DRM_BYTE ** )&poXMLContext,
                                        &cbXMLContext,
                                        SIZEOF( DRM_DWORD ),
                                        NULL ) );

#if DRM_USE_SOAP

    /* Add SOAP headers. */
    ChkDR( DRM_SOAPXML_BuildSOAPHeaderXML( poXMLContext, cbXMLContext ) );

    /*
    ** Add and open <GetMeteringCertificate> node.
    **
    ** Output:
    ** [<SOAP headers>]
    ** <GetMeteringCertificate>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterCertRootTag,
                              NULL,
                              &g_dastrMeterCertRootAttribName,
                              &g_dastrMeterCertRootAttribValue,
                              wttOpen ) );

#else

    /*
    ** Add and open <GetMeteringCertificate> node.
    **
    ** Output:
    ** <GetMeteringCertificate>
    */
    ChkDR( DRM_XMB_CreateDocumentA( cbXMLContext,
                                    ( DRM_BYTE * )poXMLContext,
                                    &g_dastrMeterCertRootTag ) );

    ChkDR( DRM_XMB_AddAttributeA( poXMLContext,
                                  &g_dastrMeterCertRootAttribName,
                                  &g_dastrMeterCertRootAttribValue ) );

#endif

    ChkDR( DRM_XMB_GetXMLBaseA( poXMLContext, &pchXML ) );

    /*
    ** Add and open <challenge> node.
    **
    ** Output:
    **  <GetMeteringCertificate>
    **      <challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterCertChallenge1Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <Challenge> node.
    **
    ** Output:
    **  <GetMeteringCertificate>
    **      <challenge>
    **          <Challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterCertChallenge2Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <MeterCert> node.
    **
    ** Output:
    **  <GetMeteringCertificate>
    **      <challenge>
    **          <Challenge>
    **              <MeterCert>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeteringMeterCertTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and close <version> node.
    **
    **  <GetMeteringCertificate>
    **      <challenge>
    **          <Challenge>
    **              <MeterCert>
    **                  <Version>...</Version>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterCertVersionTag,
                              &g_dastrMeterCertVersion,
                              NULL,
                              NULL,
                              wttClosed ) );

    cchData = SIZEOF( rgbMID );

    ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )f_poMID,
                            SIZEOF( DRM_MID ),
                            ( DRM_CHAR * )rgbMID,
                            &cchData,
                            0 ) );

    DRMASSERT( cchData == CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ) );

    dastrMID.pszString = ( DRM_CHAR * )rgbMID;
    dastrMID.cchString = cchData;

    /*
    ** Add and close <MID> node.
    **
    **  <GetMeteringCertificate>
    **      <challenge>
    **          <Challenge>
    **              <MeterCert>
    **                  <Version>...</Version>
    **                  <MID>...</MID>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrMeterCertMIDTag,
                              &dastrMID,
                              NULL,
                              NULL,
                              wttClosed ) );

    /*
    ** Construct the client information XML segment.
    **
    ** Output:
    **  <GetMeteringCertificate>
    **      <challenge>
    **          <Challenge>
    **              <MeterCert>
    **                  <ClientInfo>
    **                      ...
    **                  </ClientInfo>
    */
    ChkDR( DRM_SOAPXML_BuildClientInfo( poXMLContext,
                                        f_poBBXContext->pClientInfo  ) );

    /* Estimate the size of the XML encoded custom data. */
    ChkDR( DRM_SOAPXML_EncodeData( f_pchCustomData,
                                   f_cchCustomData,
                                   NULL,
                                   &cchCustomData ) );

    if ( cchCustomData > 0  )
    {
        ChkDR( DRM_SOAPXML_EnsureTagValueNativeByteAligned( poXMLContext,
                                                            &g_dastrMeterCertCustomDataTag ) );

        /*
        ** Add and open <CustomData> node.
        **
        **  <GetMeteringCertificate>
        **      <challenge>
        **          <Challenge>
        **              <MeterCert>
        **                  ...
        **                  <CustomData>
        */
        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                  &g_dastrMeterCertCustomDataTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        ChkDR( DRM_XMB_ReserveSpaceA( poXMLContext,
                                      SIZEOF( DRM_BYTE ),
                                      cchCustomData,
                                      &dasstrCustomData ) );

        ChkDR( DRM_SOAPXML_EncodeData( f_pchCustomData,
                                       f_cchCustomData,
                                       pchXML + __CB_DECL( dasstrCustomData.m_ich ),
                                       &cchCustomData ) );

        /*
        ** Close <CustomData> node.
        **
        **  <GetMeteringCertificate>
        **      <challenge>
        **          <Challenge>
        **              <MeterCert>
        **                  ...
        **                  <CustomData>...</CustomData>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, NULL ) );
    }

    /*
    ** Close all nodes that are still open.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <GetMeteringCertificate>
    ** ...
    ** </GetMeteringCertificate>
    ** [</SOAP headers>] (optional)
    */
    ChkDR( DRM_XMB_CloseDocumentA( poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_PrependXMLRoot( f_pdastrXML ) );

#endif

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _PrepareMeterChallengeContext
**
** Synopsis: Function that prepares a meter data challenge data structure
**           that encapsulates all the information needed to build the
**           meter data challenge. Using a data structure reduces the
**           number of parameters needed to pass between nested function
**           calls.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDSTContext]         -- Pointer to a data store context.
** [f_poRevStore]           -- Pointer to a revocation store context.
** [f_pbRevocationBuffer]   -- Pointer to a revocation buffer.
** [f_cbRevocationBuffer]   -- Size of the revocation buffer.
** [f_pbStack]              -- Pointer to a buffer that is used by a stack
**                             context (part of the meter data challenge
**                             context data structure).
** [f_cbStack]              -- Size (number of bytes) of the buffer used
**                             by a stack context.
** [f_pbMeterCert]          -- Pointer to a buffer that contains a meter
**                             certificate.
** [f_cbMeterCert]          -- Size (number of bytes) of a meter certificate.
** [f_pbDeviceCert]         -- Pointer to a buffer that contains the passed in device
**                             certificate.
** [f_cbDeviceCert]         -- Size (number of bytes) of the passed in device certificate.
** [f_pchCustomData]        -- Pointer to a character buffer that contains
**                             the custom data. It can be NULL.
** [f_cchCustomData]        -- Size (number of characters) of the custom
**                             data string.
** [f_pchURL]               -- Pointer to a buffer to receive a parsed out
**                             URL. If it is NULL, the function returns an
**                             error and the size of the required buffer
**                             is stored in *f_pcchURL.
** [f_pcchURL]              -- Pointer to a variable that contains the size
**                             (in characters) of the URL buffer during input
**                             and receives the size (in characters) of the
**                             URL during output.
** [f_poChallengeContext]   -- Pointer to a meter challenge context data
**                             structure to be initialized in this function.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the passed in URL buffer
**                          is not big enough.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _PrepareMeterChallengeContext(
    __inout                                DRM_BB_CONTEXT           *f_poBBXContext,
    __inout                                DRM_DST                  *f_poDSTContext,
    __inout                                DRM_SECSTORE_CONTEXT *f_poSSTContext,
    __inout_opt                            DRM_REVOCATIONSTORE_CONTEXT *f_poRevStore,
    __in_bcount_opt(f_cbRevocationBuffer)  DRM_BYTE                 *f_pbRevocationBuffer,
    __in                                   DRM_DWORD                 f_cbRevocationBuffer,
    __in_bcount(f_cbStack)                 DRM_BYTE                 *f_pbStack,
    __in                             const DRM_DWORD                 f_cbStack,
    __in_bcount(f_cbMeterCert )      const DRM_BYTE                 *f_pbMeterCert,
    __in                             const DRM_DWORD                 f_cbMeterCert,
    __in_bcount(f_cbDeviceCert)      const DRM_BYTE                 *f_pbDeviceCert,
    __in                             const DRM_DWORD                 f_cbDeviceCert,
    __in_ecount_opt(f_cchCustomData)       DRM_CHAR                 *f_pchCustomData,
    __in                             const DRM_DWORD                 f_cchCustomData,
    __out_ecount_opt(*f_pcchURL)           DRM_CHAR                 *f_pchURL,
    __inout                                DRM_DWORD                *f_pcchURL,
    __out                                  _METER_CHALLENGE_CONTEXT *f_poChallengeContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT dr1 = DRM_SUCCESS;
    DRM_WCHAR rgwchMID[ CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ) ] = { 0 };
    DRM_DWORD cchMID = NO_OF( rgwchMID );
    DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__PrepareMeterChallengeContext );

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDSTContext != NULL );
    ChkArg( f_pbStack != NULL  && f_cbStack > 0 );
    ChkArg( f_pbMeterCert != NULL && f_cbMeterCert > 0 );
    ChkArg( f_pbDeviceCert != NULL && f_cbDeviceCert > 0 );
    ChkArg( ( f_pchCustomData == NULL && f_cchCustomData == 0 ) ||
            ( f_pchCustomData != NULL && f_cchCustomData > 0 ) );
    ChkArg( f_pcchURL == NULL || (f_pchURL == NULL || *f_pcchURL > 0) );
    ChkArg( f_poChallengeContext != NULL );

    f_poChallengeContext->m_poBBXContext = f_poBBXContext;

    f_poChallengeContext->m_poDSTContext = f_poDSTContext;

    f_poChallengeContext->m_oStackContext.pbStack = f_pbStack;
    f_poChallengeContext->m_oStackContext.cbStack = f_cbStack;

    f_poChallengeContext->m_pchCustomData = f_pchCustomData;
    f_poChallengeContext->m_cchCustomData = f_cchCustomData;

    f_poChallengeContext->m_pbDeviceCert = ( DRM_BYTE * )f_pbDeviceCert;
    f_poChallengeContext->m_cbDeviceCert = f_cbDeviceCert;

    ChkDR( DRM_SOAPXML_InitXMLKey( f_poBBXContext->pOEMContext,
                                   &f_poChallengeContext->m_oXMLKey,
                                   &f_poBBXContext->CryptoContext ) );

    /* Use a full public key rather than a preshared key (identified by a key ID). */
    f_poChallengeContext->m_oXMLKey.m_eKeyFormat = DRM_XML_ECC256_METERING_PUBLIC_KEY_ID;

    /*
    ** Retrieve the meter ID (MID) and the meter server URL
    ** from the meter certificate.
    */
    dr1 = DRM_MTR_ParseBinaryMeterCert(  f_poBBXContext,
                                         f_poDSTContext,
                                         f_poSSTContext,
                                         f_poRevStore,
                                         f_pbRevocationBuffer,
                                         f_cbRevocationBuffer,
                                         f_pbMeterCert,
                                         f_cbMeterCert,
                                         &f_poChallengeContext->m_oMID,
                                         f_pchURL,
                                         f_pcchURL,
                                         &f_poChallengeContext->m_oXMLKey.m_oECC256PubKey );

    if ( DRM_FAILED( dr1 ) && dr1 != DRM_E_BUFFERTOOSMALL )
    {
        /*
        ** If error happens and the error code is not DRM_E_BUFFERTOOSMALL,
        ** exit immediately. Otherwise exit after the rest of the context
        ** preparation work is done.
        */
        dr = dr1;

        ChkDR( dr );
    }

    ChkDR( DRM_B64_EncodeW( ( DRM_BYTE * )&f_poChallengeContext->m_oMID,
                            SIZEOF( DRM_MID ),
                            rgwchMID,
                            &cchMID,
                            0 ) );

    DRMASSERT( cchMID == NO_OF( rgwchMID ) );

    DSTR_FROM_PB( &dstrMID, ( DRM_BYTE * )rgwchMID, SIZEOF( rgwchMID ) );

    /*
    ** Create the password that is used to access the meter store slots for
    ** a specific meter ID (MID).
    */
    ChkDR( DRM_MTR_CreateMeterStorePassword( &dstrMID,
                                             f_poChallengeContext->m_rgbSSTPassword,
                                             f_poChallengeContext->m_poBBXContext ) );

    /*
    ** Calculate the character count of the challenge XML that does not
    ** contains data of all KIDs.
    */
    ChkDR( _CalcMaximumChallengeCharCount( f_poChallengeContext,
                                           FALSE,
                                           &f_poChallengeContext->m_cchMax ) );

    /*
    ** Read the current transaction ID (TID) from the meter store.
    ** If it does not exist, create a new one.
    */
    dr = DRM_MTR_ReadTIDFromMeteringStore( &f_poChallengeContext->m_oMeterStoreContext,
                                           f_poChallengeContext->m_rgbSSTPassword,
                                           &f_poChallengeContext->m_oMID,
                                           &f_poChallengeContext->m_oTID,
                                           NULL,
                                           f_poChallengeContext->m_poDSTContext );

    if ( DRM_FAILED( dr ) )
    {
        ChkDR( DRM_MTR_CreateNewTID( f_poChallengeContext->m_poBBXContext->pOEMContext,
                                     &f_poChallengeContext->m_oTID ) );

        ChkDR( DRM_MTR_WriteTIDToMeteringStore( &f_poChallengeContext->m_oMeterStoreContext,
                                                f_poChallengeContext->m_rgbSSTPassword,
                                                &f_poChallengeContext->m_oMID,
                                                &f_poChallengeContext->m_oTID,
                                                f_poChallengeContext->m_poDSTContext ) );
    }

    /*
    ** Save the AES key obtained above, we will need it later to decrypt the KIDS in the response
    */
    ChkDR( _WriteTIDKeyToMeteringStore( &f_poChallengeContext->m_oMeterStoreContext,
                                         f_poChallengeContext->m_rgbSSTPassword,
                                        &f_poChallengeContext->m_oMID,
                            (DRM_BYTE *)&f_poChallengeContext->m_oXMLKey.m_oAESKey,
                                         f_poChallengeContext->m_poDSTContext ) );

    if ( dr1 == DRM_E_BUFFERTOOSMALL )
    {
        /*
        ** If the buffer for the URL is not big enough, exit with error.
        */
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function: _DecryptCipherData
**
** Synopsis: Function that extracts and decrypts a data buffer contained in an
**           XML segment, which should have a root node named <EncryptedData>
**           with only a <CipherData> sub-node containing the encrypted data.
**
** Arguments:
**
** [f_pszBase]    -- Base XML data string, all substrings are operating on it.
** [f_pdasstrXML] -- Pointer to a substring that contains a
**                   XML segment of the whole <EncryptedData>.
** [f_pbAESKey]   -- Pointer to the key to use to decrypt the data.
** [f_ppbData]    -- Pointer to a buffer pointer to receive the address of the
**                   decrypted data contained in the XML segment.
** [f_pcbData]    -- Pointer to a variable to receive the size (number of bytes)
**                   of the decrypted data.
**
** Returns: DRM_SUCCESS on success.
**          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: When the function completes, the original input XML string will
**       contain decrypted data that may cause the original XML string to be a
**       malformed XML string. It is the caller's responsibility to repair the
**       original XML string if it is going to be used for other XML parsings.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _DecryptCipherData(
    __inout_ecount(f_pdasstrXML->m_ich+f_pdasstrXML->m_cch)       DRM_CHAR      *f_pszBase,
    __in                                                    const DRM_SUBSTRING *f_pdasstrXML,
    __in_bcount( SIZEOF( _AES_KEY ) )                       const DRM_BYTE      *f_pbAESKey,
    __deref_out_bcount( *f_pcbData )                              DRM_BYTE     **f_ppbData,
    __out                                                         DRM_DWORD     *f_pcbData )
{
    DRM_RESULT      dr               = DRM_SUCCESS;
    DRM_BYTE       *pbCipherData     = NULL;
    DRM_DWORD       cbCipherData     = 0;
    DRM_SUBSTRING   dasstrCipherData = EMPTY_DRM_SUBSTRING;
    DRM_AES_KEY     oAESKey;
    const _AES_KEY *poXMLAESKey      = (const _AES_KEY *)f_pbAESKey;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__DecryptCipherData );

    ChkArg( f_pszBase    != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_pbAESKey   != NULL );
    ChkArg( f_ppbData    != NULL );
    ChkArg( f_pcbData    != NULL );


    /* Extract the data to be AES decrypted. */
    ChkDR( DRM_XMLSIG_ExtractCipherData( f_pszBase,
                                         f_pdasstrXML,
                                        &pbCipherData,
                                        &cbCipherData,
                                        &dasstrCipherData ) );

    ChkBOOL( cbCipherData >= DRM_AES_KEYSIZE_128, DRM_E_XMLSIG_FORMAT );

    /* Discount the size of the first DRM_AES_KEYSIZE_128 bytes of IV. */
    cbCipherData -= DRM_AES_KEYSIZE_128;

    ChkDR( Oem_Aes_SetKey( &poXMLAESKey->m_oClearAESKey.m_rgbPaddedAESKey[ DRM_AES_KEYSIZE_128 ],
                           &oAESKey ) );

    /*
    ** AES decrypt the cipher data.
    */
    ChkDR( Oem_Aes_CbcDecryptData( &oAESKey,
                                   pbCipherData + __CB_DECL(DRM_AES_KEYSIZE_128),
                                   cbCipherData,
                                   pbCipherData ) );


    /* Set the input buffer pointer to the beginning address of the decrypted data. */
    *f_ppbData = pbCipherData + __CB_DECL(DRM_AES_KEYSIZE_128);

    /* Store the size of the decrypted data. */
    *f_pcbData = cbCipherData;

ErrorExit:
    (DRM_VOID)Oem_Aes_ZeroKey( &oAESKey );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}




/*********************************************************************
**
** Function: _ProcessMeterDataResponse
**
** Synopsis: Function that processes the meter data response from the
**           server.
**
** Arguments:
**
** [f_pdastrResponse]       -- Pointer to a DRM UTF8/ANSI string that
**                             contains a meter data response XML
**                             received from the server.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDSTContext]         -- Pointer to a data store context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _ProcessMeterDataResponse(
    __inout       DRM_ANSI_STRING *f_pdastrResponse,
    __in          DRM_BB_CONTEXT  *f_poBBXContext,
    __in          DRM_DST         *f_poDSTContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrResponse = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrEncryptedData = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDecryptedData = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrData = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrVersion = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrMID = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrTID = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrRecords = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrKIDNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrKIDData = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrKeyName = EMPTY_DRM_SUBSTRING;
    DRM_KID rgKIDs[ MAX_KIDS_PER_BATCH ] = { 0 };
    DRM_DWORD iKIDs = 0;
    DRM_DWORD cbData = 0;
    DRM_MID oMID = { 0 };
    DRM_BYTE rgbMID[ CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ) * SIZEOF( DRM_WCHAR ) ] = { 0 };
    DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;
    DRM_TID oTIDInStore = { 0 };
    DRM_TID oTID = { 0 };
    DRM_METERING_CONTEXT oMeterStoreContext = { 0 };
    DRM_BYTE rgbSSTPassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_METERINGENUM_CONTEXT oMeterEnumContext = { 0 };
    DRM_CHAR *pchDecryptedData = NULL;
    DRM_DWORD cchDecryptedData = 0;
    DRM_BYTE rgbTIDKey[ __CB_DECL( SIZEOF( _AES_KEY ) ) ] = { 0 };


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__ProcessMeterDataResponse );

    ChkDRMANSIString( f_pdastrResponse );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDSTContext != NULL );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = f_pdastrResponse->cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )f_pdastrResponse->pszString,
                                         &dasstrResponse ) );

#endif

    /*
    ** Extract <MeteringResponse> node from
    ** <ProcessMeteringDataResponse>/<ProcessMeteringDataResult>/<Response> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrResponse,
                                      &g_dastrMeterDataMeteringResponsePath,
                                      NULL,
                                      NULL,
                                      &dasstrData,
                                      NULL,
                                      g_chForwardSlash ) );

    /*
    ** Extract <Version> node from the extracted <MeteringResponse> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrData,
                                      &g_dastrMeterDataVersionTag,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrVersion,
                                      g_chForwardSlash ) );
    /*
    ** Ensure that this reply is the correct metering protocol version; it must match
    ** the metering protocol version that we sent in the challenge.
    ** This version # is not to be confused with the fact that the metering protocol is
    ** contained within a V3 SOAP protocol.
    */
    ChkBOOL ( DRM_UTL_DASSTRStringsEqual( f_pdastrResponse->pszString,
                                         &dasstrVersion,
                                         &g_dastrMeterDataVersionValue ),
              DRM_E_PROTOCOL_VERSION_NOT_SUPPORTED );


    /*
    ** Extract <MID> node from the extracted <MeteringResponse> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrData,
                                      &g_dastrMeterDataMIDPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrMID,
                                      g_chForwardSlash ) );

    cbData = SIZEOF( DRM_MID );

    ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                            &dasstrMID,
                            &cbData,
                            ( DRM_BYTE *)&oMID,
                            0 ) );

    DRMASSERT( cbData == SIZEOF( DRM_MID ) );

    DSTR_FROM_PB( &dstrMID, rgbMID, SIZEOF( rgbMID ) );

    DRM_UTL_PromoteASCIItoUNICODE( f_pdastrResponse->pszString, &dasstrMID, ( DRM_STRING * )&dstrMID );

    /*
    ** Create the password that is used to access the meter store slots for
    ** a specific meter ID (MID).
    */
    ChkDR( DRM_MTR_CreateMeterStorePassword( &dstrMID, rgbSSTPassword, f_poBBXContext ) );

    /* Read the current transaction ID (TID) from the meter store. */
    ChkDR( DRM_MTR_ReadTIDFromMeteringStore( &oMeterStoreContext,
                                             rgbSSTPassword,
                                             &oMID,
                                             &oTIDInStore,
                                             NULL,
                                             f_poDSTContext ) );

    /*
    ** Extract <TID> node from the extracted <MeteringResponse> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrData,
                                      &g_dastrMeterDataTIDPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrTID,
                                      g_chForwardSlash ) );

    cbData = SIZEOF( DRM_TID );

    ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                            &dasstrTID,
                            &cbData,
                            ( DRM_BYTE *)&oTID,
                            0 ) );

    /*
    ** The transaction ID (TID) in the meter data response should match
    ** the one in the meter store.
    */
    if ( MEMCMP( &oTIDInStore, &oTID, SIZEOF( DRM_TID ) ) != 0 )
    {
        ChkDR( DRM_E_METERING_WRONG_TID );
    }

    /*
    ** Extract <EncryptedData> node from the extracted <MeteringResponse> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrData,
                                      &g_dastrMeterDataEncryptedDataPath,
                                      NULL,
                                      NULL,
                                      &dasstrEncryptedData,
                                      NULL,
                                      g_chForwardSlash ) );

    /*
    ** Extract <KeyName> node from the extracted <EncryptedData> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrEncryptedData,
                                      &g_dastrMeterDataKeyNamePath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrKeyName,
                                      g_chForwardSlash ) );

    /*
    ** Ensure that the Key Name is "TIDKey"
    */
    ChkBOOL ( DRM_UTL_DASSTRStringsEqual( f_pdastrResponse->pszString,
                                         &dasstrKeyName,
                                         &g_dastrMeterDataKeyNameValue ),
              DRM_E_PROTOCOL_VERSION_NOT_SUPPORTED );

    /*
    ** Decrypt data contained in the <EncryptedData> XML segment.
    ** The data is decrypted in place so the original response
    ** XML string may not be valid after the decryption is done.
    ** Before exiting this function, the encrypted data portion
    ** will be reset to be all blanks, therefore allowing additional
    ** XML parsing (e.g. parsing of custom data) of the original
    ** response XML string to be possible by other functions.
    */

    /*
    ** Get the clear AES key needed to decrypt the KID list in this repsonse
    */
    ChkDR( _ReadTIDKeyFromMeteringStore( &oMeterStoreContext,
                                          rgbSSTPassword,
                                         &oMID,
                                          rgbTIDKey,
                                          f_poDSTContext ) );

    ChkDR( _DecryptCipherData( f_pdastrResponse->pszString,
                              &dasstrEncryptedData,
                               rgbTIDKey,
               ( DRM_BYTE ** )&pchDecryptedData,
                              &cchDecryptedData ) );

    dasstrDecryptedData.m_ich = 0;
    dasstrDecryptedData.m_cch = cchDecryptedData;

    /*
    ** Erase the AES key from the stack and the Metering Store
    */
    OEM_SECURE_ZERO_MEMORY( rgbTIDKey, SIZEOF( _AES_KEY ) );
    ChkDR( _WriteTIDKeyToMeteringStore( &oMeterStoreContext,
                                         rgbSSTPassword,
                                        &oMID,
                                         rgbTIDKey,
                                         f_poDSTContext ) );


    /*
    ** Extract <Records> node from the decrypted data.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( pchDecryptedData,
                                      &dasstrDecryptedData,
                                      &g_dastrMeterDataRecordsPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrRecords,
                                      g_chForwardSlash ) );

    ChkDR( DRM_MTR_InitEnum( &oMID, &oMeterEnumContext, f_poDSTContext ) );

    for ( ; ; )
    {
        dr = DRM_XML_GetNodeA( pchDecryptedData,
                               &dasstrRecords,
                               &g_dastrMeterDataKIDPath,
                               NULL,
                               NULL,
                               0,
                               &dasstrKIDNode,
                               &dasstrKIDData );

        if ( dr == DRM_E_XMLNOTFOUND )
        {
            /* No more records to process. Exit the loop. */
            ChkDR( DRM_SUCCESS );

            break;
        }
        else
        {
            ChkDR( dr );
        }

        cbData = SIZEOF( DRM_KID );

        /* Store the extracted KID into an array slot. */
        ChkDR( DRM_B64_DecodeA( pchDecryptedData,
                                &dasstrKIDData,
                                &cbData,
                                ( DRM_BYTE *)( &rgKIDs[ iKIDs++ ] ),
                                0 ) );

        DRMASSERT( cbData == SIZEOF( DRM_KID ) );

        if ( iKIDs == MAX_KIDS_PER_BATCH )
        {
            /*
            ** When enough KIDs have been accumulated, delete the meter
            ** store slots of those KIDs in a batch for performance reason.
            */
            ChkDR( DRM_DST_BlockScanDelete( f_poDSTContext,
                                            &oMeterEnumContext.oSecStoreEnum.oNsContext,
                                            rgKIDs,
                                            iKIDs,
                                            &oMID,
                                            TRUE ) );

            iKIDs = 0;
        }

        dasstrRecords.m_ich = dasstrKIDNode.m_ich + dasstrKIDNode.m_cch;

        dasstrRecords.m_cch -= dasstrKIDNode.m_cch;
    }

    if ( iKIDs > 0 )
    {
        /* Delete the meter store slots of the remaining KIDs in the array. */
        ChkDR( DRM_DST_BlockScanDelete( f_poDSTContext,
                                        &oMeterEnumContext.oSecStoreEnum.oNsContext,
                                        rgKIDs,
                                        iKIDs,
                                        &oMID,
                                        TRUE ) );
    }

    /* Create a new transaction ID (TID) and store it to the meter store. */
    ChkDR( DRM_MTR_CreateNewTID( f_poBBXContext->pOEMContext, &oTID ) );

    ChkDR( DRM_MTR_WriteTIDToMeteringStore( &oMeterStoreContext,
                                            rgbSSTPassword,
                                            &oMID,
                                            &oTID,
                                            f_poDSTContext ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _ProcessMeterCertInResponse
**
** Synopsis: Function that processes the meter certificate response
**           from the server.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDSTContext]         -- Pointer to a data store context.
** [f_poSSTContext]         -- Pointer to a secure store context.
** [f_poRevStore]           -- Pointer to a revocation store context.
** [f_pbRevocationBuffer]   -- Pointer to a revocation buffer.
** [f_cbRevocationBuffer]   -- Size of the revocation buffer.
** [f_pdastrResponse]       -- Pointer to a DRM UTF8/ANSI string that
**                             contains a meter certificate response
**                             XML received from the server.
** [f_poLicEvalContext]     -- Optional pointer to a license evaluation context.
**                             If not specified, the certificate MUST be binary formatted.
** [f_poMeterCertStoreContext]
**                          -- Optional pointer to a meter certificate store
**                             context.  Required iff f_fUpdateMeterCert is TRUE.
** [f_pdastrMeterCertPath]  -- Pointer to DRM string containing the
**                             XML path to meter certificate
** [f_fUpdateMeterCert]     -- Indicates whether the meter cert storage
**                             must be updated with new meter cert
** [f_pbScratch]            -- Pointer to a buffer used to temporarily
**                             store the extracted meter certificate
**                             from the server response.
** [f_pcbScratch]           -- Pointer to a variable that contains the
**                             size (number of bytes) of the scratch
**                             buffer (mentioned above) during input
**                             and stores the size of the required
**                             scratch buffer size during output if the
**                             provided size is not big enough.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the size of the scratch
**                          buffer is not big enough to store the extracted
**                          meter certificate.
**                          DRM_E_METERCERT_NOT_FOUND - if meter certificate
**                          was not found in response
**
**********************************************************************/
static DRM_RESULT _ProcessMeterCertInResponse(
    __inout                                 DRM_BB_CONTEXT              *f_poBBXContext,
    __inout                                 DRM_DST                     *f_poDSTContext,
    __inout                                 DRM_SECSTORE_CONTEXT        *f_poSSTContext,
    __inout_opt                             DRM_REVOCATIONSTORE_CONTEXT *f_poRevStore,
    __in_bcount_opt(f_cbRevocationBuffer)   DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                    DRM_DWORD                    f_cbRevocationBuffer,
    __in                              const DRM_ANSI_CONST_STRING       *f_pdastrResponse,
    __in_opt                                DRM_LICEVAL_CONTEXT         *f_poLicEvalContext,
    __inout_opt                             DRM_METERCERT_CONTEXT       *f_poMeterCertStoreContext,
    __in                              const DRM_ANSI_CONST_STRING       *f_pdastrMeterCertPath,
    __in                              const DRM_BOOL                     f_fUpdateMeterCert,
    __inout_bcount_opt(*f_pcbScratch)       DRM_BYTE                    *f_pbScratch,
    __inout                                 DRM_DWORD                   *f_pcbScratch )
{
    DRM_RESULT          dr                  = DRM_SUCCESS;
    DRM_SUBSTRING       dasstrResponse      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrMeterCert     = EMPTY_DRM_SUBSTRING;
    DRM_CONST_STRING    dstrMeterCert       = EMPTY_DRM_STRING;
    DRM_MID             oMID                = { 0 };
    DRM_DWORD           cbRequiredSize      = 0;
    DRM_SUBSTRING       dasstrRequiredSize  = EMPTY_DRM_SUBSTRING;
    DRM_BOOL            fIsV2               = TRUE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC__ProcessMeterCertInResponse );

    ChkDRMANSIString( f_pdastrResponse );
    ChkArg( !f_fUpdateMeterCert || ( f_poMeterCertStoreContext != NULL ) );
    ChkArg( f_pdastrMeterCertPath != NULL );
    ChkArg( f_pcbScratch != NULL );
    ChkArg( f_pbScratch == NULL || *f_pcbScratch > 0 );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = f_pdastrResponse->cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )f_pdastrResponse->pszString,
                                         &dasstrResponse ) );

#endif

    /*
    ** Extract <MeterCert> node from f_pdastrMeterCertPath node
    */
    dr = DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                    &dasstrResponse,
                                    f_pdastrMeterCertPath,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &dasstrMeterCert,
                                    g_chForwardSlash );
    ChkDRMap( dr, DRM_E_XMLNOTFOUND, DRM_E_METERCERT_NOT_FOUND );

    cbRequiredSize = dasstrMeterCert.m_cch;

    if ( f_pbScratch == NULL || *f_pcbScratch < cbRequiredSize )
    {
        *f_pcbScratch = cbRequiredSize;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ZEROMEM( f_pbScratch, cbRequiredSize );

    DRM_BYT_CopyBytes( f_pbScratch,
                       0,
                       f_pdastrResponse->pszString,
                       dasstrMeterCert.m_ich,
                       dasstrMeterCert.m_cch );

    dasstrRequiredSize.m_ich = 0;
    dasstrRequiredSize.m_cch = cbRequiredSize;

    ChkDR( DRM_B64_DecodeA( ( DRM_CHAR * )f_pbScratch,
                            &dasstrRequiredSize,
                            &cbRequiredSize,
                            f_pbScratch,
                            DRM_BASE64_DECODE_IN_PLACE ) );

    DSTR_FROM_PB( &dstrMeterCert, f_pbScratch, cbRequiredSize );

    if ( ( f_poLicEvalContext == NULL )
      || DRM_FAILED( DRM_MTR_ParseMeterCert( &dstrMeterCert,
                                             f_poLicEvalContext,
                                             NULL,
                                             NULL,
                                             &oMID,
                                             NULL ) ) )
    {
        /* Try to parse the meter cert as a binary meter cert. */
        PUBKEY_P256 oServerPubKey;

        ChkDR( DRM_MTR_ParseBinaryMeterCert( f_poBBXContext,
                                             f_poDSTContext,
                                             f_poSSTContext,
                                             f_poRevStore,
                                             f_pbRevocationBuffer,
                                             f_cbRevocationBuffer,
                                             f_pbScratch,
                                             cbRequiredSize,
                                             &oMID,
                                             NULL,
                                             NULL,
                                             &oServerPubKey ) );
        fIsV2 = FALSE;
    }

    if ( f_fUpdateMeterCert )
    {
        /* Store the meter certificate to the meter certificate store. */
        ChkDR( DRM_MCS_UpdateMeterCert( f_poMeterCertStoreContext,
                                        NULL,
                                        fIsV2,
                                        f_pbScratch,
                                        cbRequiredSize,
                                        &oMID ) );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_MTR_GenerateMeterDataChallenge
**
** Synopsis: API that generates a meter data challenge XML to be sent
**           to the server.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDSTContext]         -- Pointer to a data store context.
** [f_poSSTContext]         -- Pointer to a secure store context.
** [f_poRevStore]           -- Pointer to a revocation store context.
** [f_pbRevocationBuffer]   -- Pointer to a revocation buffer.
** [f_cbRevocationBuffer]   -- Size of the revocation buffer.
** [f_pbStack]              -- Pointer to a buffer that is used by a stack
**                             context (part of the meter data challenge
**                             context data structure).
** [f_cbStack]              -- Size (number of bytes) of the buffer used
**                             by a stack context.
** [f_pbMeterCert]          -- Pointer to a buffer that contains a meter
**                             certificate.
** [f_cbMeterCert]          -- Size (number of bytes) of a meter certificate.
** [f_pbDeviceCert]         -- Pointer to a buffer that contains the passed in device
**                             certificate.
** [f_cbDeviceCert]         -- Size (number of bytes) of the passed in device certificate.
** [f_pchURL]               -- Pointer to a buffer that receives the parsed out
**                             license acquisition URL.
** [f_pcchURL]              -- Pointer to a variable that contains the size
**                             (number of characters) of the buffer that is used
**                             to store the license acquisition URL, and stores
**                             the actual size (number of characters) of the
**                             parsed out license acquisition URL.
** [f_pchCustomData]        -- Pointer to a character buffer that contains
**                             the custom data. It can be NULL.
** [f_cchCustomData]        -- Size (number of characters) of the custom
**                             data string.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated
**                             challenge byte stream. If it is NULL, the
**                             function returns an error and the size of
**                             the required buffer is stored in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the
**                             size (in bytes) of the challenge buffer
**                             during input and receives the size (in
**                             bytes) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_FALSE if the meter data challenge does not
**                          contains the meter data of all KIDs.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffers to retrieve
**                          the URL or challenge data are not big enough.
**
** Note: Caller may provide a buffer that is not big enough to store meter
** data of all KIDs but big enough to store meter data of at least one KID.
** If so the API will try to store as much meter data as possible into the
** provided buffer. If the buffer provided by the caller is either NULL or
** not enough to store meter data of even a single KID, the API returns a
** recommended size that is big enough to store meter data of all KIDs in
** the meter store.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_GenerateMeterDataChallenge(
    __inout                                DRM_BB_CONTEXT *f_poBBXContext,
    __inout                                DRM_DST        *f_poDSTContext,
    __inout                                DRM_SECSTORE_CONTEXT *f_poSSTContext,
    __inout_opt                            DRM_REVOCATIONSTORE_CONTEXT *f_poRevStore,
    __in_bcount_opt(f_cbRevocationBuffer)  DRM_BYTE       *f_pbRevocationBuffer,
    __in                                   DRM_DWORD       f_cbRevocationBuffer,
    __in_bcount(f_cbStack)                 DRM_BYTE       *f_pbStack,
    __in                                   DRM_DWORD       f_cbStack,
    __in_bcount(f_cbMeterCert)       const DRM_BYTE       *f_pbMeterCert,
    __in                                   DRM_DWORD       f_cbMeterCert,
    __in_bcount(f_cbDeviceCert)      const DRM_BYTE       *f_pbDeviceCert,
    __in                                   DRM_DWORD       f_cbDeviceCert,
    __in_ecount_opt(f_cchCustomData) const DRM_CHAR       *f_pchCustomData,
    __in                                   DRM_DWORD       f_cchCustomData,
    __out_ecount_opt(*f_pcchURL)           DRM_CHAR       *f_pchURL,
    __inout                                DRM_DWORD      *f_pcchURL,
    __out_bcount_opt( *f_pcbChallenge )    DRM_BYTE       *f_pbChallenge,
    __inout                                DRM_DWORD      *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT dr1 = DRM_SUCCESS;
    _METER_CHALLENGE_CONTEXT oChallengeContext = { 0 };
    DRM_ANSI_STRING dastrXML = EMPTY_DRM_STRING;
    DRM_DWORD cchXMLContext = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC_DRM_MTR_GenerateMeterDataChallenge );

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDSTContext != NULL );
    ChkArg( f_pbStack != NULL  && f_cbStack > 0 );
    ChkArg( f_pbMeterCert != NULL && f_cbMeterCert > 0 );
    ChkArg( f_pbDeviceCert != NULL && f_cbDeviceCert > 0 );
    ChkArg( ( f_pchCustomData == NULL && f_cchCustomData == 0 ) ||
            ( f_pchCustomData != NULL && f_cchCustomData > 0 ) );
    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL ||
            *f_pcbChallenge > 0 );

    ZEROMEM( &oChallengeContext, SIZEOF( _METER_CHALLENGE_CONTEXT ) );

    /*
    ** Encapsulates all the necessary information to build a meter
    ** data challenge into a data structure.
    */
    dr1 = _PrepareMeterChallengeContext( f_poBBXContext,
                                         f_poDSTContext,
                                         f_poSSTContext,
                                         f_poRevStore,
                                         f_pbRevocationBuffer,
                                         f_cbRevocationBuffer,
                                         f_pbStack,
                                         f_cbStack,
                                         f_pbMeterCert,
                                         f_cbMeterCert,
                                         f_pbDeviceCert,
                                         f_cbDeviceCert,
                                         ( DRM_CHAR * )f_pchCustomData,
                                         f_cchCustomData,
                                         f_pchURL,
                                         f_pcchURL,
                                         &oChallengeContext );

    if ( DRM_FAILED( dr1 ) && dr1 != DRM_E_BUFFERTOOSMALL )
    {
        /*
        ** If error happens and the error code is not DRM_E_BUFFERTOOSMALL,
        ** exit immediately. Otherwise exit after the required size of the
        ** challenge buffer gets calculated.
        */
        dr = dr1;

        ChkDR( dr );
    }

    /* Calculate the minimum size of the required buffer. */
    cchXMLContext = oChallengeContext.m_cchFixedEncrypted +
                    oChallengeContext.m_cchFixedUnencrypted;

    ChkOverflow( cchXMLContext, oChallengeContext.m_cchFixedEncrypted );

    if ( f_pbChallenge == NULL || *f_pcbChallenge < cchXMLContext )
    {
        /*
        ** If the buffer provided by the caller is not even big enough to store
        ** the fixed portions (encrypted and unencrypted) of the meter data
        ** challenge XML, return with an error.
        */
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    if ( dr1 == DRM_E_BUFFERTOOSMALL )
    {
        /*
        ** If the buffer for challenge is big enough but the buffer for URL
        ** is not, exit with error.
        */
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Calculate the size of buffer that can be used to store the actual
    ** meter data.
    */
    oChallengeContext.m_cchVaried = *f_pcbChallenge -
                                    oChallengeContext.m_cchFixedUnencrypted -
                                    oChallengeContext.m_cchFixedEncrypted;

    ChkDR( _BuildMeterDataChallengeXML( f_pbChallenge,
                                        *f_pcbChallenge,
                                        &oChallengeContext,
                                        &dastrXML ) );

    ChkBOOL( dastrXML.cchString <= *f_pcbChallenge,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbChallenge, dastrXML.pszString, dastrXML.cchString );

    /* Return the actual size (number of bytes) of the generated challenge. */
    *f_pcbChallenge = dastrXML.cchString;

    if ( oChallengeContext.m_fPartialData )
    {
        dr = DRM_S_FALSE;
    }

ErrorExit:

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        /*
        ** Calculate the character count of the challenge XML that includes
        ** meter data of all KIDs.
        */
        dr1 = _CalcMaximumChallengeCharCount( &oChallengeContext,
                                              TRUE,
                                              &oChallengeContext.m_cchMax );

        if ( DRM_SUCCEEDED( dr1 ) )
        {
            *f_pcbChallenge = oChallengeContext.m_cchMax;
        }
        else
        {
            /*
            ** If we couldn't calculate the size, we shouldn't give the caller DRM_E_BUFFERTOOSMALL.
            ** If we do, they'll think we gave them a valid size and then call back into us after
            ** allocating a zero-byte buffer (or one-byte, if they add space for a null-terminator)
            ** and with a size of 0, which will then cause this function to fail on a ChkArg.
            */
            dr = dr1;
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_MTR_ProcessMeterDataResponse
**
** Synopsis: API that processes a meter data response from the server.
**           It deletes all meter data records of KIDs that are in the
**           response for a specific meter ID (MID) that is also in the
**           response.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDSTContext]         -- Pointer to a data store context.
** [f_poSSTContext]         -- Pointer to a secure store context.
** [f_poRevStore]           -- Pointer to a revocation store context.
** [f_pbRevocationBuffer]   -- Pointer to a revocation buffer.
** [f_cbRevocationBuffer]   -- Size of the revocation buffer.
** [f_pbScratch]            -- Pointer to a buffer used to temporarily
**                             store the extracted meter certificate
**                             from the server response.
** [f_pcbScratch]           -- Pointer to a variable that contains the
**                             size (number of bytes) of the scratch
**                             buffer (mentioned above) during input
**                             and stores the size of the required
**                             scratch buffer size during output if the
**                             provided size is not big enough.
** [f_pbResponse]           -- Pointer to a buffer that contains the meter
**                             data response from the server.
** [f_cbResponse]           -- Size of the meter data response from the server.
**                             It must be bigger than 0.
** [f_pfResponseSignatureChecked]
**                          -- Pointer to a flag indicating whether the signature
**                             of the response has already been checked.
** [f_pdwProtocol]          -- Pointer to a variable to store the type of the
**                             protocol used by the response.
** [f_pResult]              -- Optional pointer to a variable that receives the
**                             status code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_ProcessMeterDataResponse(
    __in                                    DRM_BB_CONTEXT              *f_poBBXContext,
    __in                                    DRM_DST                     *f_poDSTContext,
    __inout                                 DRM_SECSTORE_CONTEXT        *f_poSSTContext,
    __inout_opt                             DRM_REVOCATIONSTORE_CONTEXT *f_poRevStore,
    __in_bcount_opt(f_cbRevocationBuffer)   DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                    DRM_DWORD                    f_cbRevocationBuffer,
    __inout_bcount(*f_pcbScratch)           DRM_BYTE                    *f_pbScratch,
    __inout                                 DRM_DWORD                   *f_pcbScratch,
    __inout_bcount(f_cbResponse)            DRM_BYTE                    *f_pbResponse,
    __in                                    DRM_DWORD                    f_cbResponse,
    __inout                                 DRM_BOOL                    *f_pfResponseSignatureChecked,
    __inout                                 DRM_DWORD                   *f_pdwProtocol,
    __out_opt                               DRM_RESULT                  *f_pResult )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING dastrcResponse = EMPTY_DRM_STRING;
    DRM_BOOL fOverwriteResult = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC_DRM_MTR_ProcessMeterDataResponse );

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDSTContext != NULL );
    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pfResponseSignatureChecked != NULL );
    ChkArg( f_pdwProtocol != NULL );

    fOverwriteResult = TRUE;

    if( !(*f_pfResponseSignatureChecked) || *f_pdwProtocol != DRM_SOAPXML_PROTOCOL_METERING_DATA_RESPONSE )
    {
        DRM_DWORD   dwProtocol  = DRM_SOAPXML_PROTOCOL_METERING_DATA_RESPONSE;

        /*
        ** This validates an exception or the protocol we request.
        */
        dr = DRM_SOAPXML_ValidateProtocolSignature ( f_pbResponse,
                                                     f_cbResponse,
                                                     f_pbScratch,
                                                     *f_pcbScratch,
                                                     &f_poBBXContext->CryptoContext,
                                                     &dwProtocol );
        ChkDRMap( dr, DRM_E_XMLSIG_SHA_VERIFY_FAILURE, DRM_E_INVALID_METERRESPONSE_SIGNATURE );

        *f_pdwProtocol                  = dwProtocol;
        *f_pfResponseSignatureChecked   = TRUE;
    }

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );
    DASTR_FROM_PB( &dastrcResponse, f_pbResponse, f_cbResponse );

    dr = ( _ProcessMeterCertInResponse( f_poBBXContext,
                                        f_poDSTContext,
                                        f_poSSTContext,
                                        f_poRevStore,
                                        f_pbRevocationBuffer,
                                        f_cbRevocationBuffer,
                                        &dastrcResponse,
                                        NULL,
                                        NULL,
                                        &g_dastrMeterDataMeterCertPath,
                                        FALSE,
                                        f_pbScratch,
                                        f_pcbScratch ) );

    /* if meter cert is not found then there is old meter response format */
    ChkDRMap( dr, DRM_E_METERCERT_NOT_FOUND, DRM_SUCCESS );

    ChkDR( _ProcessMeterDataResponse( &dastrResponse,
                                      f_poBBXContext,
                                      f_poDSTContext ) );

     /* Try to detect whether there is custom data from the server. */
    dr = DRM_SOAPXML_ParseCustomDataForProtocol( f_pbResponse,
                                                 f_cbResponse,
                                                 DRM_SOAPXML_PROTOCOL_METERING_DATA_RESPONSE,
                                                 NULL,
                                                 NULL );

    if ( DRM_SUCCEEDED( dr ) )
    {
        /* Yes there is a custom data string from the server. */
        dr = DRM_S_MORE_DATA;
    }
    else
    {
        dr = DRM_SUCCESS;
    }

ErrorExit:

    dr = DRM_SOAPXML_ParseStatusCodeAndChooseBest(f_pbResponse,
                                                  f_cbResponse,
                                                  dr,
                                                  fOverwriteResult ? f_pResult : NULL );
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_MTR_GenerateMeterCertChallenge
**
** Synopsis: API that generates a meter certificate challenge XML to
**           be sent to the server.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to the BBX Context
** [f_poMID]                -- Pointer to a meter ID (MID).
** [f_pchCustomData]        -- Pointer to a character buffer that contains
**                             the custom data. It can be NULL.
** [f_cchCustomData]        -- Size (number of characters) of the custom
**                             data string.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated
**                             challenge byte stream. If it is NULL, the
**                             function returns an error and the size of
**                             the required buffer is stored in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the
**                             size (in bytes) of the challenge buffer
**                             during input and receives the size (in
**                             bytes) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_MTR_GenerateMeterCertChallenge(
    __in                                    DRM_BB_CONTEXT  *f_poBBXContext,
    __in                              const DRM_MID         *f_poMID,
    __in_ecount_opt(f_cchCustomData)  const DRM_CHAR        *f_pchCustomData,
    __in                                    DRM_DWORD        f_cchCustomData,
    __out_bcount_opt(*f_pcbChallenge)       DRM_BYTE        *f_pbChallenge,
    __inout                                 DRM_DWORD       *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchChallenge = 0;
    DRM_ANSI_STRING dastrXML = EMPTY_DRM_STRING;
    DRM_DWORD cchEncodedCustomData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC_DRM_MTR_GenerateMeterCertChallenge );

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poMID != NULL );
    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL ||
            *f_pcbChallenge > 0 );
    ChkArg( ( f_pchCustomData == NULL && f_cchCustomData == 0 ) ||
            ( f_pchCustomData != NULL && f_cchCustomData > 0 ) );

    /* Get the size (number of characters) of the XML encoded custom data. */
    ChkDR( DRM_SOAPXML_EncodeData( f_pchCustomData,
                                   f_cchCustomData,
                                   NULL,
                                   &cchEncodedCustomData ) );

    ChkDR( _CalcMeterCertChallengeCharCount( f_poBBXContext, cchEncodedCustomData, &cchChallenge ) );

    if ( f_pbChallenge == NULL ||
         *f_pcbChallenge < cchChallenge )
    {
        *f_pcbChallenge = cchChallenge;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( _BuildMeterCertChallengeXML( f_poBBXContext,
                                        f_pbChallenge,
                                        *f_pcbChallenge,
                                        f_poMID,
                                        f_pchCustomData,
                                        f_cchCustomData,
                                        &dastrXML ) );

    ChkBOOL( dastrXML.cchString <= *f_pcbChallenge,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbChallenge, dastrXML.pszString, dastrXML.cchString );

    /* Return the actual size (number of bytes) of the generated challenge. */
    *f_pcbChallenge = dastrXML.cchString;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_MTR_ProcessMeterCertResponse
**
** Synopsis: API that processes a meter certificate response from the
**           server. It extracts the embedded meter certificate and
**           stores it into the meter certificate store.
**
** Arguments:
**
** [f_poLicEvalContext]     -- Pointer to a license evaluation context.
** [f_poMeterCertStoreContext]
**                          -- Pointer to a meter certificate store context.
** [f_pbScratch]            -- Pointer to a buffer used to temporarily
**                             store the extracted meter certificate
**                             from the server response.
** [f_pcbScratch]           -- Pointer to a variable that contains the
**                             size (number of bytes) of the scratch
**                             buffer (mentioned above) during input
**                             and stores the size of the required
**                             scratch buffer size during output if the
**                             provided size is not big enough.
** [f_pbResponse]           -- Pointer to a buffer that contains the meter
**                             certificate response from the server.
** [f_cbResponse]           -- Size of the meter certificate response from
**                             the server. It must be bigger than 0.
** [f_pResult]              -- Optional pointer to an variable that receives
**                             the status code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_METERCERT_NOT_FOUND - if meter certificate
**                          was not found in response
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_MTR_ProcessMeterCertResponse(
    __in                       DRM_LICEVAL_CONTEXT   *f_poLicEvalContext,
    __in                       DRM_METERCERT_CONTEXT *f_poMeterCertStoreContext,
    __in_bcount(*f_pcbScratch) DRM_BYTE              *f_pbScratch,
    __inout                    DRM_DWORD             *f_pcbScratch,
    __in_bcount(f_cbResponse)  DRM_BYTE              *f_pbResponse,
    __in                       DRM_DWORD              f_cbResponse,
    __out_opt                  DRM_RESULT            *f_pResult )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_BOOL fOverwriteResult = FALSE;
    DRM_REVOCATIONSTORE_CONTEXT *poRevStoreContext = NULL;
    DRM_BYTE *pbRevBuffer = NULL;
    DRM_DWORD cbRevBuffer = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC_DRM_MTR_ProcessMeterCertResponse );

    ChkArg( f_poLicEvalContext != NULL );
    ChkArg( f_poMeterCertStoreContext != NULL );
    ChkArg( f_pcbScratch != NULL );
    ChkArg( f_pbScratch == NULL || *f_pcbScratch > 0 );
    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );

    fOverwriteResult = TRUE;

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        poRevStoreContext = f_poLicEvalContext->pContextRev;
        pbRevBuffer = f_poLicEvalContext->pbRevocationBuffer;
        cbRevBuffer = f_poLicEvalContext->cbRevocationBuffer;
    }

    ChkDR( _ProcessMeterCertInResponse( f_poLicEvalContext->pcontextBBX,
                                        f_poLicEvalContext->pDatastore,
                                        f_poLicEvalContext->pcontextSSTRevocation,
                                        poRevStoreContext,
                                        pbRevBuffer,
                                        cbRevBuffer,
                                        &dastrResponse,
                                        f_poLicEvalContext,
                                        f_poMeterCertStoreContext,
                                        &g_dastrMeterCertMeterCertPath,
                                        TRUE,
                                        f_pbScratch,
                                        f_pcbScratch ) );

    /* Try to detect whether there is custom data from the server. */
    dr = DRM_SOAPXML_ParseCustomDataForProtocol( f_pbResponse,
                                                 f_cbResponse,
                                                 DRM_SOAPXML_PROTOCOL_METERING_CERT_RESPONSE,
                                                 NULL,
                                                 NULL );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        /* Yes there is a custom data string from the server. */
        dr = DRM_S_MORE_DATA;
    }
    else
    {
        dr = DRM_SUCCESS;
    }

ErrorExit:

    dr = DRM_SOAPXML_ParseStatusCodeAndChooseBest(f_pbResponse,
                                                  f_cbResponse,
                                                  dr,
                                                  fOverwriteResult ? f_pResult : NULL );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;
