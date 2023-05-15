/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmhds.h>
#include <drmbcertparser.h>
#include <drmxmlparser.h>
#include <drmxmlbuildera.h>
#include <oemaescommon.h>
#include <oemaes.h>
#include <drmxmlsig.h>
#include <drmxmlsigconstants.h>
#include <drmsoapxmlconstants.h>
#include <drmsoapxmlutility.h>
#include <drmdomainconstants.h>
#include <drmmoveconstants.h>
#include <drmlicacqv3constants.h>
#include <drmmeterconstants.h>
#include <drmcontract.h>
#include <oem.h>
#include <drmkeyfile.h>
#include <drmmove.h>
#include <drmmetering.h>
#include <drmdomainapi.h>

ENTER_PK_NAMESPACE_CODE;

typedef struct tagDRM_SOAPXML_PROTOCOL_INTERNAL_DEFINE
{
          DRM_BOOL (DRM_CALL *pfnIsSupported)(DRM_VOID);
    const DRM_ANSI_CONST_STRING *pdastrCustomDataPath;
    const DRM_ANSI_CONST_STRING *pdastrDataPath;
    const DRM_ANSI_CONST_STRING *pdastrSignaturePath;
          DRM_BOOL               fSignatureRequired;
    const DRM_ANSI_CONST_STRING *pdastrCertTag;
    const PUBKEY_P256           *ppubkey;
} DRM_SOAPXML_PROTOCOL_INTERNAL_DEFINE;

#define EMPTY_DRM_SOAPXML_PROTOCOL_INTERNAL_DEFINE { NULL, NULL, NULL, NULL, FALSE, NULL, NULL }

static DRM_NO_INLINE DRM_BOOL DRM_CALL _ProtocolSupported(DRM_VOID) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_BOOL DRM_CALL _ProtocolSupported(DRM_VOID)
{
    CLAW_AUTO_RANDOM_CIPHER
    return TRUE;
}

static const DRM_SOAPXML_PROTOCOL_INTERNAL_DEFINE g_rgSoapProtoDefines[ DRM_SOAPXML_PROTOCOL_MAXIMUM_DEFINED + 1 ] = {
      EMPTY_DRM_SOAPXML_PROTOCOL_INTERNAL_DEFINE,                                                                                                                                                                                   /* DRM_SOAPXML_PROTOCOL_UNKNOWN       */
      { _ProtocolSupported,                 &g_dastrSOAPExceptionCustomDataPath, &g_dastrSOAPExceptionRootPath,                    &g_dastrSOAPExceptionSignaturePath, FALSE, NULL,                             NULL                    },  /* DRM_SOAPXML_PROTOCOL_EXCEPTION     */
      { DRM_DOMAIN_IsDomainSupported,       &g_dastrDomJoinCustomDataPath,       &g_dastrDomJoinDomainPath,                        &g_dastrDomJoinSignaturePath      , TRUE,  &g_dastrDomJoinSigningCertTag,    &g_oWMRMECC256PubKey    },  /* DRM_SOAPXML_PROTOCOL_DOMAIN_JOIN   */
      { DRM_DOMAIN_IsDomainSupported,       &g_dastrDomLeaveCustomDataPath,      &g_dastrDomLeaveDomainPath,                       &g_dastrDomLeaveSignaturePath     , TRUE,  NULL,                             &g_oWMRMECC256PubKey    },  /* DRM_SOAPXML_PROTOCOL_DOMAIN_LEAVE  */
      { DRM_METERING_IsMeteringSupported,   &g_dastrMeterCertCustomDataPath,     &g_dastrMeterCertMeteringCertificateResponsePath, &g_dastrMeterCertSignaturePath    , TRUE, NULL,                              NULL                    },  /* DRM_SOAPXML_PROTOCOL_METERING_CERT */
      { DRM_METERING_IsMeteringSupported,   &g_dastrMeterDataCustomDataPath,     &g_dastrMeterDataMeteringResponsePath,            &g_dastrMeterDataSignaturePath    , TRUE, &g_dastrMeteringMeterCertTag,      NULL                    },  /* DRM_SOAPXML_PROTOCOL_METERING_DATA */
      { _ProtocolSupported,                 &g_dastrLicAcqCustomDataPath,        &g_dastrLicLicenseResponsePath,                   &g_dastrLicSignaturePath          , TRUE, &g_dastrLicAcqSigningCertTag,      &g_oWMRMECC256PubKey    },  /* DRM_SOAPXML_PROTOCOL_LIC_ACQ       */
      { _ProtocolSupported,                 &g_dastrLicAckCustomDataPath,        &g_dastrLicAcqLAckPath,                           &g_dastrLicAckSignaturePath       , TRUE, NULL,                              &g_oWMRMECC256PubKey    },  /* DRM_SOAPXML_PROTOCOL_LIC_ACK       */
      { DRM_MOVE_IsMoveSupported,           NULL,                                &g_dastrMovMoveResponsePath,                      &g_dastrMovMoveSignaturePath      , TRUE, NULL,                              &g_pubkeyMoveService    },  /* DRM_SOAPXML_PROTOCOL_MOVE_REQ_RESPONSE     */
      { DRM_MOVE_IsMoveSupported,           NULL,                                &g_dastrMovRefreshResponsePath,                   &g_dastrMovRefreshSignaturePath   , TRUE, NULL,                              &g_pubkeyMoveService    },  /* DRM_SOAPXML_PROTOCOL_MOVE_REFRESH_RESPONSE */
      { DRM_MOVE_IsMoveSupported,           NULL,                                &g_dastrMovCopyResponsePath,                      &g_dastrMovCopySignaturePath      , TRUE, NULL,                              &g_pubkeyMoveService    },  /* DRM_SOAPXML_PROTOCOL_COPY_REQ_RESPONSE     */
};

#define IS_PROTOCOL_SUPPORTED(iProtocol)                            \
    ( ( g_rgSoapProtoDefines[(iProtocol)].pfnIsSupported != NULL )  \
   && ( g_rgSoapProtoDefines[(iProtocol)].pfnIsSupported() ) )


/*********************************************************************
**
** Function: DRM_SOAPXML_EncodeData
**
** Synopsis: Function that does XML encoding of the passed in UTF8/ANSI
**           string that will be used in the text element.
**
** Arguments:
**
** [f_pchData]              -- Pointer to a buffer that contains the
**                             string to be XML encoded.
** [f_cchData]              -- Number of characters of the string to be XML
**                             encoded.
** [f_pchEncodedData]       -- Pointer to buffer to receive the XML encoded
**                             string. If f_pchEncodedData is NULL. The size
**                             of required buffer is stored in
**                             in *f_pcchEncodedData.
** [f_pcchEncodedData]      -- Pointer to a variable to receive the size of
**                             buffer that contains the XML encoded string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if f_pchEncodedData
**                          is not NULL and the size of the the buffer
**                          pointed by f_pchEncodedData is not big
**                          enough to receive the encoded result.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_EncodeData(
    __in_ecount( f_cchData )         const DRM_CHAR   *f_pchData,
    __in                                   DRM_DWORD   f_cchData,
    __out_ecount_opt( *f_pcchEncodedData ) DRM_CHAR   *f_pchEncodedData,
    __out                                  DRM_DWORD  *f_pcchEncodedData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_EncodeData );

    ChkArg( f_pchData == NULL || f_cchData > 0 );
    ChkArg( f_pcchEncodedData != NULL );

    if ( f_pchData == NULL )
    {
        *f_pcchEncodedData = 0;

        goto ErrorExit;
    }

    dr = DRM_UTL_XMLEncodeA( f_pchData,
                             f_cchData,
                             FALSE,
                             f_pchEncodedData,
                             f_pcchEncodedData );

    if ( DRM_FAILED( dr ) )
    {
        if ( f_pchEncodedData == NULL && dr == DRM_E_BUFFERTOOSMALL )
        {
            ChkDR( DRM_SUCCESS );
        }
        else
        {
            ChkDR( dr );
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_DecodeData
**
** Synopsis: Function that does XML decoding of the passed in
**           UTF8/ANSI string.
**
** Arguments:
**
** [f_pchData]              -- Pointer to a buffer that contains the
**                             UTF8/ANSI string to be XML decoded.
** [f_cchData]              -- Number of characters of the string to be XML
**                             decoded.
** [f_ppchDecodedData]      -- Pointer to buffer to receive the XML decoded
**                             string. If f_pchDecodedData is NULL, the size
**                             of required buffer is stored in
**                             *f_pcchDecodedData.
** [f_pcchDecodedData]      -- Pointer to a variable to receive the size of
**                             buffer that contains the XML decoded string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if f_pchDecodedData
**                          is not NULL and the size of the the buffer
**                          pointed by f_pchDecodedData is not big
**                          enough to receive the decoded result.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_DecodeData(
    __in_ecount( f_cchData )               const DRM_CHAR   *f_pchData,
    __in                                         DRM_DWORD   f_cchData,
    __out_ecount_opt( *f_pcchDecodedData )       DRM_CHAR   *f_pchDecodedData,
    __out                                        DRM_DWORD  *f_pcchDecodedData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_DecodeData );

    ChkArg( f_pchData == NULL || f_cchData > 0 );
    ChkArg( f_pcchDecodedData != NULL );

    if ( f_pchData == NULL )
    {
        *f_pcchDecodedData = 0;

        goto ErrorExit;
    }

    ChkDR( DRM_UTL_XMLDecodeA( f_pchData,
                             f_cchData,
                             f_pchDecodedData,
                             f_pcchDecodedData ) );
ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_PadANSIString
**
** Synopsis: Function that pad an ANSI string to make its length
**           multiples of a specific block size. The algorithm is
**           PKCS#7 which is defined in RFC 2898, section 6.11.
**
** Arguments:
**
** [f_pdastrIn]             -- Pointer to a DRM ANSI string to be padded.
** [f_cchPad]               -- Number of characters of each data block. The
**                             total length of the padded string will be
**                             the multiples of the block size. It should
**                             be in the range of [1, 255].
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:                    Caller must make sure the buffer underneath
**                          should be big enough to store the newly added
**                          padding characters. The extra bytes needed at
**                          the end of the buffer specified by f_pdastrIn
**                          should be in the range of [1, f_cchPad].
**
** Brief description of PKCS#7 padding algorithm (use block size 8 as example):
** Notation:
** ||  ||  : octet length operator
** ||      : concatenation operator
** Concatenate M and a padding string PS to form an encoded message EM:
** EM = M || PS ,
** where the padding string PS consists of 8 -( ||M|| mod 8 ) octets
** each with value 8 - ( ||M|| mod 8 ). The padding string PS will
** satisfy one of the following statements:
** PS = 01, if ||M|| mod 8 = 7 ;
** PS = 02 02, if ||M|| mod 8 = 6 ;
** ...
** PS = 08 08 08 08 08 08 08 08, if ||M|| mod 8 = 0.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_PadANSIString(
    __inout DRM_ANSI_STRING  *f_pdastrIn,
    __in    DRM_DWORD         f_cchPad )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchNew = 0;
    DRM_BYTE chPadding = 0;

    ChkDRMANSIString( f_pdastrIn );
    ChkArg( f_cchPad > 0 && f_cchPad <= 255 );

    cchNew = f_cchPad - ( f_pdastrIn->cchString % f_cchPad );

    ChkOverflow( f_cchPad, cchNew );

    chPadding = ( DRM_BYTE )cchNew;

    DRM_BYT_SetBytes( f_pdastrIn->pszString,
                      f_pdastrIn->cchString,
                      cchNew,
                      chPadding );

    f_pdastrIn->cchString += cchNew;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_IsBlankGUID
**
** Synopsis: Function that checks whether a passed in DRM_GUID contains
**           all 0.
**
** Arguments:
**
** [f_poGUID]               -- Pointer to a variable of type DRM_GUID.
**
** Returns:                 TRUE: The GUID contains all zeros or the input
**                          pointer is NULL.
**                          FALSE: The GUID does not contain all zeros.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_SOAPXML_IsBlankGUID(
    __in DRM_GUID *f_poGUID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fResult = TRUE;
    DRM_GUID oEmptyGUID = EMPTY_DRM_GUID;

    /* Return TRUE if the passed in GUID pointer is NULL. */
    ChkDR( f_poGUID != NULL );

    fResult = ( MEMCMP( &oEmptyGUID, f_poGUID, SIZEOF( DRM_GUID ) ) == 0 );

ErrorExit:

    return fResult;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_CalcDWORDCharCount
**
** Synopsis: Function that calculates the character count of a DWORD
**           decimal value when it is converted into a string.
**
** Arguments:
**
** [f_dwNumber]             -- The DWORD value to be calculated.
**
** Returns:                 A long integer value that is the character
**                          count of the DWORD value.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_DWORD DRM_CALL DRM_SOAPXML_CalcDWORDCharCount(
    __in DRM_DWORD f_dwNumber )
{
    DRM_DWORD cchCount = 1;

    while ( f_dwNumber >= 10 )
    {
        f_dwNumber /= 10;

        cchCount++;
    }

    return cchCount;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_GetDeviceCert
**
** Synopsis: Function that retrieves a device certificate and the
**           size (number of characters) of a device certificate.
**           The returned format can be B64 encoded or not.
**
** Arguments:
**
** [f_poKeyFileContext]     -- Pointer to a key file context.
** [f_fB64Encoded]          -- Flag indicating whether the device certificate
**                             should be returned as B64 encoded.
** [f_pchDeviceCert]        -- Pointer to a character buffer that is used
**                             to receive the device certificate.
**                             It can be NULL, in which case only the size
**                             (number of characters) of the device
**                             certificate is returned.
** [f_pcchDeviceCert]       -- Pointer to a variable that contains the
**                             size (number of characters) of the buffer
**                             mentioned above during input and receives
**                             the size (number of characters) of the contained
**                             device certificate during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if f_pchDeviceCert is not
**                          NULL and *f_pcchDeviceCert is smaller than the
**                          size (number of characters) to store the device
**                          certificate.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_GetDeviceCert(
    __in                                  DRM_KEYFILE_CONTEXT   *f_poKeyFileContext,
    __in                                  DRM_BOOL               f_fB64Encoded,
    __out_ecount_opt( *f_pcchDeviceCert ) DRM_CHAR              *f_pchDeviceCert,
    __inout                               DRM_DWORD             *f_pcchDeviceCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbBuffer = NULL;
    DRM_DWORD cbBuffer = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_GetDeviceCert );

    ChkArg( f_pcchDeviceCert != NULL );

    /*
    ** Retrieve the binary device certificate
    */
    ChkDR( DRM_KF_GetCertificate( f_poKeyFileContext, eKF_CERT_TYPE_PLAYREADY, &pbBuffer, &cbBuffer ) );

    if ( f_fB64Encoded )
    {
         /*
         ** If the supplied buffer is big enough, B64 encode the binary data to ASCII chars in place.
         */
         if ( *f_pcchDeviceCert < CCH_BASE64_EQUIV( cbBuffer ) )
         {
             dr = DRM_E_BUFFERTOOSMALL;
         }
         else
         {
             ChkArg( f_pchDeviceCert != NULL );
             MEMCPY( f_pchDeviceCert, pbBuffer, cbBuffer );

             dr = DRM_B64_EncodeA( (DRM_BYTE *)f_pchDeviceCert,
                                   cbBuffer,
                                   (DRM_CHAR *)f_pchDeviceCert,
                                   f_pcchDeviceCert,
                                   0 );
        }
    }
    else
    {
        if ( *f_pcchDeviceCert < cbBuffer )
        {
             dr = DRM_E_BUFFERTOOSMALL;
        }
        else
        {
            MEMCPY( f_pchDeviceCert, pbBuffer, cbBuffer );
        }
    }

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR ( dr );
    }
    else
    {
        *f_pcchDeviceCert = f_fB64Encoded ? CCH_BASE64_EQUIV( cbBuffer ) : cbBuffer;

        if ( f_pchDeviceCert != NULL )
        {
            ChkDR( dr );
        }

        dr = DRM_SUCCESS;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_InitXMLKey
**
** Synopsis: Function that creates a DRM_XML_KEY object, which is used
**           to improve the public key crypto performance. This object
**           contains a randomly generated AES key that is protected by
**           a pre-shared WMRM ECC-256 key pair.
**
** Arguments:
** [f_poOEMContext]         -- Pointer to an OEM implemented context.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY structure to store
**                             the generated encrypted AES key.
** [f_pCryptoCtx]           -- Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_InitXMLKey(
    __in_opt DRM_VOID             *f_poOEMContext,
    __out    DRM_XML_KEY          *f_poXMLKey,
    __inout  DRM_CRYPTO_CONTEXT   *f_pCryptoCtx )
{
    DRM_RESULT dr = DRM_SUCCESS;
    PLAINTEXT_P256 oPlainText = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_InitXMLKey );

    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_pCryptoCtx != NULL );

    ZEROMEM( f_poXMLKey, SIZEOF( DRM_XML_KEY ) );

    f_poXMLKey->m_eKeyInfoType = DRM_XML_KEYINFO_ENCRYPTED;

    f_poXMLKey->m_eKeyFormat = DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID;

    /*
    ** Generate a random AES key (second 16 bytes).
    ** The second 16 bytes MUST be filled ealier than
    ** the first 16 bytes.
    */
    ChkDR( Oem_Random_GetBytes( f_poOEMContext,
                               &oPlainText.m_rgbPlaintext[ DRM_AES_KEYSIZE_128 ],
                               DRM_AES_KEYSIZE_128 ) );

    /* Generate random 16 bytes of padding such that the entire buffer can be encrypted with ECC. */
    ChkDR( OEM_ECC_GenerateHMACKey_P256( &oPlainText, (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext ) );

    DRMCASSERT( DRM_XML_AES_KEY_SIZE == SIZEOF( PLAINTEXT_P256 ) );

    /* Store the 32 bytes generated as the padded AES key. */
    MEMCPY( f_poXMLKey->m_oAESKey.m_oClearAESKey.m_rgbPaddedAESKey,
            oPlainText.m_rgbPlaintext,
            DRM_XML_AES_KEY_SIZE );

    /* Generate a random, 16-byte initialization vector */
    ChkDR( Oem_Random_GetBytes( f_poOEMContext,
                                f_poXMLKey->m_oAESKey.m_oClearAESKey.m_rgbIV,
                                DRM_AES_KEYSIZE_128 ) );



ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_EncryptDataWithXMLKey
**
** Synopsis: Function that uses AES CBC mode to encrypt the passed in
**           data. Then it uses a pre-shared WMRM/Metering public ECC-256
**           key to encrypt the AES key in place.
**
** Arguments:
**
** [f_poOEMContext]         -- Pointer to an OEM implemented context.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY structure to store
**                             the generated encrypted AES key.
** [f_pdastrData]           -- Pointer to a DRM string that is to be
**                             encrypted in place.
** [f_pCryptoCtx]           -- Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_EncryptDataWithXMLKey(
    __in_opt    DRM_VOID                *f_poOEMContext,
    __inout     DRM_XML_KEY             *f_poXMLKey,
    __inout     DRM_ANSI_CONST_STRING   *f_pdastrData,
    __inout     DRM_CRYPTO_CONTEXT      *f_pCryptoCtx )
{
    DRM_RESULT dr = DRM_SUCCESS;
    PLAINTEXT_P256 oPlainText = { 0 };
    CIPHERTEXT_P256 oCipherText = { 0 };
    DRM_AES_KEY oAESKey = { 0 };
    DRM_BYTE *pbData = NULL;
    DRM_DWORD cbData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_EncryptDataWithXMLKey );

    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID ||
            f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_METERING_PUBLIC_KEY_ID ||
            f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_PUBLIC_KEY_DATA );
    ChkArg( f_pCryptoCtx != NULL );
    ChkDRMANSIString( f_pdastrData );

    pbData = PB_DASTR( f_pdastrData );
    cbData = CB_DASTR( f_pdastrData );

    ChkDR( Oem_Aes_SetKey( &f_poXMLKey->m_oAESKey.m_oClearAESKey.m_rgbPaddedAESKey[ DRM_AES_KEYSIZE_128 ],
                           &oAESKey ) );

    /* Leave space for DRM_AES_KEYSIZE_128 bytes of IV at the beginning of the buffer. */
    DRM_BYT_MoveBytes( pbData + __CB_DECL( DRM_AES_KEYSIZE_128 ),
                       0,
                       pbData,
                       0,
                       cbData );

    /* Store Initialization Vector. */
    DRM_BYT_CopyBytes( pbData,
                       0,
                       f_poXMLKey->m_oAESKey.m_oClearAESKey.m_rgbIV,
                       0,
                       DRM_AES_KEYSIZE_128 );

    f_pdastrData->cchString += DRM_AES_KEYSIZE_128;

    /*
    ** Encrypt the passed in data in place with the random AES key just
    ** generated. It assumes AES CBC ciphering does not change the size
    ** of a packed data buffer.
    */
    ChkDR( Oem_Aes_CbcEncryptData( &oAESKey,
                                   pbData + __CB_DECL( DRM_AES_KEYSIZE_128 ),
                                   cbData,
                                   f_poXMLKey->m_oAESKey.m_oClearAESKey.m_rgbIV ) );

    ZEROMEM( &oPlainText, SIZEOF( PLAINTEXT_P256 ) );

    DRMCASSERT( DRM_XML_AES_KEY_SIZE == SIZEOF( PLAINTEXT_P256 ) );

    /*
    ** Copy padded key value
    */
    MEMCPY( oPlainText.m_rgbPlaintext,
            f_poXMLKey->m_oAESKey.m_oClearAESKey.m_rgbPaddedAESKey,
            DRM_XML_AES_KEY_SIZE );

    if ( f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID )
    {
        /*
        ** Encrypt the random AES key in place using a pre-shared WMRM public key.
        */
        ChkDR( OEM_ECC_Encrypt_P256( &g_oWMRMECC256PubKey,
                                     &oPlainText,
                                     &oCipherText,
                                     ( struct bigctx_t * )f_pCryptoCtx->rgbCryptoContext ) );
    }
    else if ( f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_PUBLIC_KEY_DATA ||
              f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_METERING_PUBLIC_KEY_ID )
    {
        ChkDR( OEM_ECC_Encrypt_P256( &f_poXMLKey->m_oECC256PubKey,
                                     &oPlainText,
                                     &oCipherText,
                                     ( struct bigctx_t * )f_pCryptoCtx->rgbCryptoContext ) );
    }

    /* Store the ECC-256 encrypted AES key in the passed in XML key structure. */
    MEMCPY( f_poXMLKey->m_oAESKey.m_rgbCipherAESKey,
            oCipherText.m_rgbCiphertext,
            ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );

ErrorExit:
    (DRM_VOID)Oem_Aes_ZeroKey( &oAESKey );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

#if DRM_USE_SOAP

/*********************************************************************
**
** Function: DRM_SOAPXML_SkipSOAPHeaders
**
** Synopsis: Function that skips the SOAP headers of a passed in server
**           response string.
**
** Arguments:
**
** [f_pszResponse]          -- Pointer to a character buffer that contains
**                             the server response.
** [f_pdasstrResponse]      -- Pointer to a sub string that defines the range
**                             of the original server reponse during input
**                             and contains portion of the original server
**                             response that has the SOAP header skipped.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SOAPXML_XML_FORMAT if expected XML node
**                          cannot be parsed out from the server response.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_SkipSOAPHeaders(
    __in_ecount( f_pdasstrResponse->m_ich + f_pdasstrResponse->m_cch ) const DRM_CHAR         *f_pszResponse,
    __inout                                                                  DRM_SUBSTRING    *f_pdasstrResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrTemp = EMPTY_DRM_SUBSTRING;

    ChkArg( f_pszResponse != NULL );
    ChkArg( f_pdasstrResponse != NULL );

    /* Strip out the standard SOAP headers. */
    dr = DRM_XML_GetNodeA( f_pszResponse,
                           f_pdasstrResponse,
                          &g_dastrSOAPEnvelopeTag,
                           NULL,
                           NULL,
                           0,
                           NULL,
                          &dasstrTemp );
    if( DRM_SUCCEEDED( dr ) )
    {

        *f_pdasstrResponse = dasstrTemp;

        ChkDR( DRM_XML_GetNodeA( f_pszResponse,
                                 f_pdasstrResponse,
                                 &g_dastrSOAPBodyTag,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                &dasstrTemp ) );

        *f_pdasstrResponse = dasstrTemp;
    }
    else
    {
        /* Handle the case where a server may have serialized the soap headers using the abbreviated syntax <s:Body>... */
        ChkDR( DRM_XML_GetNodeA( f_pszResponse,
                                 f_pdasstrResponse,
                                &g_dastrSOAPEnvelopeAbbrevTag,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                &dasstrTemp ) );

        *f_pdasstrResponse = dasstrTemp;

        ChkDR( DRM_XML_GetNodeA( f_pszResponse,
                                 f_pdasstrResponse,
                                 &g_dastrSOAPBodyAbbrevTag,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                &dasstrTemp ) );

        *f_pdasstrResponse = dasstrTemp;

    }

ErrorExit:

    if ( DRM_FAILED( dr ) && dr != DRM_E_INVALIDARG )
    {
        dr = DRM_E_SOAPXML_XML_FORMAT;
    }

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_CalcSOAPHeaderCharCount
**
** Synopsis: Function that calculates the character count of a SOAP
**           header that has an envelope and a body tag.
**
** Arguments:               N/A
**
** Returns:                 A long integer value that is the character
**                          count of the calculated SOAP header.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_DWORD DRM_CALL DRM_SOAPXML_CalcSOAPHeaderCharCount( DRM_VOID )
{
    DRM_DWORD cchCount = 0;

    /* <soap12:Envelope ... > */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrSOAPEnvelopeTag.cchString,
                                             0,
                                             g_dastrSOAPEnvelopeAttrib1Name.cchString,
                                             g_dastrSOAPEnvelopeAttrib1Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForTag( 0,
                                             0,
                                             g_dastrSOAPEnvelopeAttrib2Name.cchString,
                                             g_dastrSOAPEnvelopeAttrib2Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForTag( 0,
                                             0,
                                             g_dastrSOAPEnvelopeAttrib3Name.cchString,
                                             g_dastrSOAPEnvelopeAttrib3Value.cchString );

    /* <soap12:Body> */
    cchCount += DRM_XMB_RequiredCharsForTag( g_dastrSOAPBodyTag.cchString,
                                             0,
                                             0,
                                             0 );

    return cchCount;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_PrependXMLRoot
**
** Synopsis: Function that prepend a XML root tag at the beginning of
**           a XML string.
**
** Arguments:
**
** [f_pdastrXML]            -- Pointer to a DRM ANSI string that is the XML
**                             string where the XML root tag is going to
**                             be prepended.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:                    Caller needs to make sure the passed in string
**                          has a buffer that is big enough to accomodate
**                          the inserted XML root tag.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_PrependXMLRoot(
    __inout DRM_ANSI_STRING *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDRMANSIString( f_pdastrXML );

    /* Reserve space at the beginning of the passed in XMLs string. */
    ChkDR( DRM_UTL_StringInsertBlankSubStringA( f_pdastrXML,
                                                0,
                                                g_dastrXMLRootTag.cchString ) );

    MEMCPY( f_pdastrXML->pszString,
            PB_DASTR( &g_dastrXMLRootTag ),
            CB_DASTR( &g_dastrXMLRootTag ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_BuildSOAPHeaderXML
**
** Synopsis: Function that constructs a SOAP header using the XML builder.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_cbXMLContext]         -- Size (number of bytes) of internal buffer
**                             of a XML builder context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_BuildSOAPHeaderXML(
    __inout  _XMBContextA *f_poXMLContext,
    __in     DRM_DWORD     f_cbXMLContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_BuildSOAPHeaderXML );

    ChkArg( f_poXMLContext != NULL );

    /*
    ** Add and open <soap12:Envelope> node.
    **
    ** Output:
    ** <soap12:Envelope ... >
    */
    ChkDR( DRM_XMB_CreateDocumentA( f_cbXMLContext,
                                    ( DRM_BYTE * )f_poXMLContext,
                                    &g_dastrSOAPEnvelopeTag ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrSOAPEnvelopeAttrib1Name,
                                  &g_dastrSOAPEnvelopeAttrib1Value ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrSOAPEnvelopeAttrib2Name,
                                  &g_dastrSOAPEnvelopeAttrib2Value ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrSOAPEnvelopeAttrib3Name,
                                  &g_dastrSOAPEnvelopeAttrib3Value ) );

    /*
    ** Add and open <soap12:Body> node.
    **
    ** Output:
    ** <soap12:Body ... >
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrSOAPBodyTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

#endif

/*********************************************************************
**
** Function: DRM_SOAPXML_PrepareXMLContext
**
** Synopsis: Function that initializes a XML builder context over the
**           passed in buffer.
**
** Arguments:
**
** [f_pbXMLContext]         -- Pointer to a buffer on which the XML builder
**                             context is to be created.
** [f_cbXMLContext]         -- Size (number of bytes) of the XML builder
**                             context to be created.
** [f_ppoXMLContext]        -- Pointer to a pointer to the created XML builder
**                             context.
** [f_pcbXMLContext]        -- Pointer to a variable that receives the total size
**                             (number of bytes) of the created XML builder context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_PrepareXMLContext(
    __in_bcount( f_cbXMLContext )          DRM_BYTE        *f_pbXMLContext,
    __in                                   DRM_DWORD        f_cbXMLContext,
    __deref_out_bcount( *f_pcbXMLContext ) _XMBContextA   **f_ppoXMLContext,
    __out                                  DRM_DWORD       *f_pcbXMLContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbXMLContext != NULL );
    ChkArg( f_cbXMLContext > 0 );
    ChkArg( f_ppoXMLContext != NULL );
    ChkArg( f_pcbXMLContext != NULL );

    ZEROMEM( f_pbXMLContext, f_cbXMLContext );

    ChkDR( DRM_UTL_EnsureDataAlignment( f_pbXMLContext,
                                        f_cbXMLContext,
                                        ( DRM_BYTE ** )f_ppoXMLContext,
                                        f_pcbXMLContext,
                                        SIZEOF( DRM_DWORD ),
                                        NULL ) );

ErrorExit:

    return dr;
}


static DRM_NO_INLINE DRM_RESULT DRM_CALL _GetAdditionalResponseExceptionData(
    __in_bcount( f_cbResponse )      const DRM_BYTE   *f_pbResponse,
    __in                                   DRM_DWORD   f_cbResponse,
    __in                                   DRM_DWORD   f_dwDataType,
    __out_ecount_opt( *f_pcchDataString )  DRM_CHAR   *f_pchDataString,
    __inout                                DRM_DWORD  *f_pcchDataString )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  cchInput = 0;
    DRM_ANSI_CONST_STRING *pdastrDataStringPath = NULL;
    DRM_ANSI_CONST_STRING  dastrResponse        = EMPTY_DRM_STRING;
    DRM_SUBSTRING          dasstrResponse       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING          dasstrException      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING          dasstrData           = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC__GetAdditionalResponseExceptionData );

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pcchDataString != NULL );
    ChkArg( f_pchDataString == NULL || *f_pcchDataString > 0 );

    cchInput = *f_pcchDataString;

    /*
    ** At this point, assume the server response is an error response.
    */
    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                        &dasstrResponse ) );

#endif

    /*
    ** Extract <Exception> node from the server response.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( dastrResponse.pszString,
                                      &dasstrResponse,
                                      &g_dastrSOAPExceptionRootPath,
                                      NULL,
                                      NULL,
                                      &dasstrException,
                                      NULL,
                                      g_chForwardSlash ) );

    switch ( f_dwDataType )
    {
        case DRM_GARD_REDIRECT_URL:
            pdastrDataStringPath = ( DRM_ANSI_CONST_STRING * )
                                   &g_dastrSOAPGARDRedirectUrlPath;
            break;

        case DRM_GARD_SERVICE_ID:
            pdastrDataStringPath = ( DRM_ANSI_CONST_STRING * )
                                   &g_dastrSOAPGARDServiceIdPath;
            break;

        case DRM_GARD_ACCOUNT_ID:
            pdastrDataStringPath = ( DRM_ANSI_CONST_STRING * )
                                   &g_dastrSOAPGARDAccountIdPath;
            break;

        default:
            ChkDR( DRM_E_INVALIDARG );
            break;
    }

    /*
    ** Extract the data string using the selected path.
    */
    dr = DRM_XML_GetSubNodeByPathA( dastrResponse.pszString,
                                    &dasstrException,
                                    pdastrDataStringPath,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &dasstrData,
                                    g_chForwardSlash );

    if ( f_pchDataString == NULL ||
         *f_pcchDataString < dasstrData.m_cch )
    {
        *f_pcchDataString = dasstrData.m_cch;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    if ( dasstrData.m_cch <= cchInput )
    {
        DRM_BYT_CopyBytes( f_pchDataString,
                           0,
                           dastrResponse.pszString,
                           dasstrData.m_ich,
                           dasstrData.m_cch );
    }

    *f_pcchDataString = dasstrData.m_cch;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


static DRM_NO_INLINE DRM_RESULT DRM_CALL _GetAdditionalResponseDomainIDData(
    __in_bcount( f_cbResponse )      const DRM_BYTE   *f_pbResponse,
    __in                                   DRM_DWORD   f_cbResponse,
    __in                                   DRM_DWORD   f_dwDataType,
    __out_ecount_opt( *f_pcchDataString )  DRM_CHAR   *f_pchDataString,
    __inout                                DRM_DWORD  *f_pcchDataString )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING dastrResponse  = EMPTY_DRM_STRING;
    DRM_SUBSTRING         dasstrResponse = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrDomain   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrID       = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC__GetAdditionalResponseDomainIDData );

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pcchDataString != NULL );
    ChkArg( f_pchDataString == NULL || *f_pcchDataString > 0 );

    /*
    ** At this point, assume the server response is an error response.
    */
    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;


#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                         &dasstrResponse ) );

#endif

    /*
    ** Extract <Domain> node from the response, it is in different places for join and leave
    */
    dr = DRM_XML_GetSubNodeByPathA(  dastrResponse.pszString,
                                    &dasstrResponse,
                                    &g_dastrDomJoinDomainPath,
                                     NULL,
                                     NULL,
                                    &dasstrDomain,
                                     NULL,
                                     g_chForwardSlash );

    if ( DRM_E_XMLNOTFOUND == dr )
    {
        ChkDR( DRM_XML_GetSubNodeByPathA(  dastrResponse.pszString,
                                          &dasstrResponse,
                                          &g_dastrDomLeaveDomainPath,
                                           NULL,
                                           NULL,
                                          &dasstrDomain,
                                           NULL,
                                           g_chForwardSlash ) );
    }
    else
    {
        ChkDR( dr );
    }

    if ( DRM_GARD_SERVICE_ID == f_dwDataType )
    {
        /*
        ** Extract <ServiceID> node from the <Domain> node
        */
        ChkDR( DRM_XML_GetSubNodeByPathA(  dastrResponse.pszString,
                                          &dasstrDomain,
                                          &g_dastrDomJoinServiceIDPath,
                                           NULL,
                                           NULL,
                                           NULL,
                                          &dasstrID,
                                           g_chForwardSlash ) );

    }
    else if ( DRM_GARD_ACCOUNT_ID == f_dwDataType )
    {
        /*
        ** Extract <AccountID> node from the <Domain> node
        */
        ChkDR( DRM_XML_GetSubNodeByPathA(  dastrResponse.pszString,
                                          &dasstrDomain,
                                          &g_dastrDomJoinAccountIDPath,
                                           NULL,
                                           NULL,
                                           NULL,
                                          &dasstrID,
                                           g_chForwardSlash ) );
    }
    else
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    if ( f_pchDataString == NULL || *f_pcchDataString < dasstrID.m_cch )
    {
        *f_pcchDataString = dasstrID.m_cch;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    DRM_BYT_CopyBytes( f_pchDataString,
                       0,
                       dastrResponse.pszString,
                       dasstrID.m_ich,
                       dasstrID.m_cch );
    *f_pcchDataString = dasstrID.m_cch;


ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


static DRM_NO_INLINE DRM_RESULT _GetProtocol(
    __in_bcount( f_cbResponse ) const DRM_BYTE       *f_pbResponse,
    __in                              DRM_DWORD       f_cbResponse,
    __out                             DRM_DWORD      *f_pdwProtocolType )
{
    DRM_RESULT            dr               = DRM_SUCCESS;
    DRM_DWORD             iProtocol        = DRM_SOAPXML_PROTOCOL_UNKNOWN+1;  /* Start at the first real protocol */
    DRM_BOOL              fDataFound       = FALSE;
    DRM_ANSI_CONST_STRING dastrResponse    = EMPTY_DRM_STRING;
    DRM_SUBSTRING         dasstrResponse   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrData       = EMPTY_DRM_SUBSTRING;

    ChkArg( f_pdwProtocolType != NULL );
    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                        &dasstrResponse ) );
#endif

    /*
    ** Scan through the known protocols looking for data sections
    */
    while( !fDataFound && ( iProtocol <= DRM_SOAPXML_PROTOCOL_MAXIMUM_DEFINED ) )
    {
        if( IS_PROTOCOL_SUPPORTED(iProtocol)
         && g_rgSoapProtoDefines[iProtocol].pdastrDataPath != NULL )
        {
            dr = DRM_XML_GetSubNodeByPathA( ( DRM_CHAR * )dastrResponse.pszString,
                                             &dasstrResponse,
                                              g_rgSoapProtoDefines[iProtocol].pdastrDataPath,
                                              NULL,
                                              NULL,
                                             &dasstrData,
                                              NULL,
                                              g_chForwardSlash );
            if( DRM_SUCCEEDED( dr ) )
            {
                fDataFound = TRUE;
            }
        }
        if( !fDataFound )
        {
            iProtocol += 1;
        }
    }

    ChkBOOL( fDataFound, DRM_E_SOAPXML_PROTOCOL_NOT_SUPPORTED );

    *f_pdwProtocolType = iProtocol;


ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_ValidateProtocolSignature
**
** Synopsis: Loops through the known DRM Protocol Response types.
**           If the data section for the type is found then it looks for
**           a signature node. If the signature node is found, the signature
**           is validated against the data section. If there is no signature,
**           then we look for Custom Data. A signature must be included
**           when there is a custom data node.
**
**           If a protocol type is given, but doesn't match we will try to
**           validate for an Exception message.
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size (number of bytes) of the server response.
** [f_pbScratchBCertBuffer] -- Pointer to a scratch buffer for cert processing
** [f_cbScratchBCertBuffer] -- Size (number of bytes) of the scratch buffer.
**                             Return walue is size of meter cert.
** [f_pCryptoCtx]           -- Pointer to a Crypto Context for Signature validation.
** [f_pdwProtocolType]      -- Dword Protocol Type. If this is DRM_SOAPXML_PROTOCOL_UNKNOWN
**                             then we loop through all protocols. We store the protocol here
**                             exit.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SOAPXML_SIGNATURE_MISSING if custom data is found
**                              without a signature node.
**                          DRM_E_XMLSIG_SHA_VERIFY_FAILURE - if signature validation is failing
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_ValidateProtocolSignature(
    __in_bcount( f_cbResponse )   const DRM_BYTE            *f_pbResponse,
    __in                                DRM_DWORD            f_cbResponse,
    __inout_bcount_opt( f_cbScratchBCertBuffer )
                                        DRM_BYTE            *f_pbScratchBCertBuffer,
    __in                          const DRM_DWORD            f_cbScratchBCertBuffer,
    __in                                DRM_CRYPTO_CONTEXT  *f_pCryptoCtx,
    __inout                             DRM_DWORD           *f_pdwProtocolType )
{
    DRM_RESULT            dr                         = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING dastrResponse              = EMPTY_DRM_STRING;
    DRM_SUBSTRING         dasstrResponse             = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrData                 = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrCustomData           = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrSignature            = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrCert                 = EMPTY_DRM_SUBSTRING;
    DRM_DWORD             cbCert                     = 0;
    PUBKEY_P256           oParsedPubKey              = {0};
    const PUBKEY_P256    *ppubkey                    = &g_oWMRMECC256PubKey;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_ValidateProtocolSignature );

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pdwProtocolType != NULL );
    ChkArg( *f_pdwProtocolType <= DRM_SOAPXML_PROTOCOL_MAXIMUM_DEFINED );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                        &dasstrResponse ) );
#endif

    /*
    ** If they didn't tell use the protocol try to find out.
    */
    if ( *f_pdwProtocolType == DRM_SOAPXML_PROTOCOL_UNKNOWN )
    {
        ChkDR( _GetProtocol( f_pbResponse, f_cbResponse, f_pdwProtocolType ) );
    }

    if( IS_PROTOCOL_SUPPORTED(*f_pdwProtocolType)
     && g_rgSoapProtoDefines[*f_pdwProtocolType].ppubkey != NULL )
    {
        ppubkey = g_rgSoapProtoDefines[*f_pdwProtocolType].ppubkey;
    }

    if( IS_PROTOCOL_SUPPORTED(*f_pdwProtocolType)
     && g_rgSoapProtoDefines[*f_pdwProtocolType].pdastrDataPath != NULL )
    {
        dr = DRM_XML_GetSubNodeByPathA( ( DRM_CHAR * )dastrResponse.pszString,
                                       &dasstrResponse,
                                        g_rgSoapProtoDefines[*f_pdwProtocolType].pdastrDataPath,
                                        NULL,
                                        NULL,
                                       &dasstrData,
                                        NULL,
                                        g_chForwardSlash );

        /*
        ** If we didn't match the protocol above, see if it's an exception
        */
        if ( DRM_FAILED(dr) )
        {
            ChkDR( DRM_XML_GetSubNodeByPathA( ( DRM_CHAR * )dastrResponse.pszString,
                                             &dasstrResponse,
                                              g_rgSoapProtoDefines[DRM_SOAPXML_PROTOCOL_EXCEPTION].pdastrDataPath,
                                              NULL,
                                              NULL,
                                             &dasstrData,
                                              NULL,
                                              g_chForwardSlash ) );
            /*
            ** Don't make the assignment to the variable until after checking.
            */
            *f_pdwProtocolType = DRM_SOAPXML_PROTOCOL_EXCEPTION;
        }
        else
        {
            if ( g_rgSoapProtoDefines[*f_pdwProtocolType].pdastrCertTag != NULL )
            {
                dr =  DRM_XML_GetSubNodeByPathA( ( DRM_CHAR * )dastrResponse.pszString,
                                                 &dasstrData,
                                                 g_rgSoapProtoDefines[*f_pdwProtocolType].pdastrCertTag,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 &dasstrCert,
                                                 g_chForwardSlash );

                if ( *f_pdwProtocolType == DRM_SOAPXML_PROTOCOL_METERING_DATA_RESPONSE )
                {
                    ChkBOOL( DRM_SUCCEEDED( dr ) || dr == DRM_E_XMLNOTFOUND, dr );
                }
                else
                {
                    ChkDR( dr );
                }

                /* if cert is found then obtain public key */
                if ( DRM_SUCCEEDED( dr ) )
                {
                    ChkArg( f_pbScratchBCertBuffer != NULL );

                    /* Clean out scratch buffer before use */
                    ZEROMEM( f_pbScratchBCertBuffer, f_cbScratchBCertBuffer );

                    cbCert = f_cbScratchBCertBuffer;
                    ChkDR( DRM_B64_DecodeA( ( DRM_CHAR * )dastrResponse.pszString,
                                             &dasstrCert,
                                             &cbCert,
                                             f_pbScratchBCertBuffer,
                                             0 ) );

                    ChkDR( DRM_BCert_GetPublicKeyByUsage(
                        f_pbScratchBCertBuffer,
                        f_cbScratchBCertBuffer,
                        0,
                        *f_pdwProtocolType == DRM_SOAPXML_PROTOCOL_LIC_ACQ_RESPONSE || *f_pdwProtocolType == DRM_SOAPXML_PROTOCOL_DOMAIN_JOIN_RESPONSE ?
                            DRM_BCERT_KEYUSAGE_SIGN_RESPONSE :
                            DRM_BCERT_KEYUSAGE_UNKNOWN,
                        &oParsedPubKey,
                        NULL ) );
                    ppubkey = &oParsedPubKey;

                    /* Clean out scratch buffer after use */
                    ZEROMEM( f_pbScratchBCertBuffer, f_cbScratchBCertBuffer );
                }
            }
        }
    }

    /*
    ** Extract <Signature> node from the server response if exists.
    */
    dr = DRM_XML_GetSubNodeByPathA( dastrResponse.pszString,
                                   &dasstrResponse,
                                    IS_PROTOCOL_SUPPORTED(*f_pdwProtocolType) ? g_rgSoapProtoDefines[*f_pdwProtocolType].pdastrSignaturePath : NULL,
                                    NULL,
                                    NULL,
                                   &dasstrSignature,
                                    NULL,
                                    g_chForwardSlash );

    if ( DRM_SUCCEEDED( dr ) )
    {
        if( ppubkey == NULL )
        {
            /*
            **  We found a signature but never found a public key to verify against.
            **
            */
            DRMASSERT( !"We should always have a public key to compare against by now." );
            ChkDR( DRM_E_XMLNOTFOUND );
        }

        ChkDR( DRM_XMLSIG_VerifySignature( dastrResponse.pszString,
                                           &dasstrData,
                                           dastrResponse.pszString,
                                           &dasstrSignature,
                                           ppubkey,
                                           f_pCryptoCtx ) );
    }
    else
    {
        /*
        ** Signature node is required in license acquisition response and
        ** domain join response.
        */
        if( *f_pdwProtocolType == DRM_SOAPXML_PROTOCOL_LIC_ACQ_RESPONSE
         || *f_pdwProtocolType == DRM_SOAPXML_PROTOCOL_DOMAIN_JOIN_RESPONSE )
        {
            ChkDR( dr );
        }

        /*
        ** If we don't have a Signature node, let's make sure we don't have custom data.
        */
        dr = DRM_SUCCESS; /* Make sure this is success here, before checking for custom data. */

        /*
        ** Check that either the signature is not require or we don't have custom data.
        */
        ChkBOOL( !IS_PROTOCOL_SUPPORTED(*f_pdwProtocolType)
              || !g_rgSoapProtoDefines[*f_pdwProtocolType].fSignatureRequired
              || DRM_FAILED(
                    DRM_XML_GetSubNodeByPathA( ( DRM_CHAR * )dastrResponse.pszString,
                                              &dasstrResponse,
                                               g_rgSoapProtoDefines[*f_pdwProtocolType].pdastrCustomDataPath,
                                               NULL,
                                               NULL,
                                              &dasstrCustomData,
                                               NULL,
                                               g_chForwardSlash ) ),
                 DRM_E_SOAPXML_SIGNATURE_MISSING );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*********************************************************************
**
** Function: _ParseCustomData
**
** Synopsis: Function that tries to parse out the custom data string from
**           the server response.
**           The function has no prior knowledge of the type of server
**           response it receives, so it tries all the possibilities by
**           using different XML paths until succeeded, or return failure
**           if none of the known XML path can be used to retrieve the
**           custom data string.
**
**
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size (number of bytes) of the server response.
** [f_pchCustomData]        -- Pointer to a character buffer to receive the parsed
**                             out custom string. If it is NULL, the function
**                             returns an error and the size of the required buffer
**                             is stored in *f_cchCustomData.
** [f_cchCustomData]        -- Pointer to a variable that contains the size (in
**                             characters) of the buffer mentioned above during
**                             input and receives the size (in characters) of the
**                             custom data during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the custom data is not big enough.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ParseCustomData(
    __in_bcount(f_cbResponse)     const DRM_BYTE   *f_pbResponse,
    __in                                DRM_DWORD   f_cbResponse,
    __out_ecount_opt(*f_pcchCustomData) DRM_CHAR   *f_pchCustomData,
    __inout                             DRM_DWORD  *f_pcchCustomData )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  dwProtocol = DRM_SOAPXML_PROTOCOL_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC__ParseCustomData );

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pcchCustomData != NULL );

    ChkDR( _GetProtocol( f_pbResponse, f_cbResponse, &dwProtocol ) );

    ChkDR( DRM_SOAPXML_ParseCustomDataForProtocol( f_pbResponse,
                                                   f_cbResponse,
                                                   dwProtocol,
                                                   f_pchCustomData,
                                                   f_pcchCustomData ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _ParseCustomDataByPath
**
** Synopsis: Function that tries to parse out the custom data string from
**           the server response using a particular XML path.
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size (number of bytes) of the server response.
** [f_pdastrPath]           -- Pointer to a DRM string that contains the XML
**                             path used to parse out the custom data.
** [f_pchCustomData]        -- Pointer to a character buffer to receive the parsed
**                             out custom string. If it is NULL, the function
**                             returns an error and the size of the required buffer
**                             is stored in *f_cchCustomData.
** [f_pcchCustomData]       -- Pointer to a variable that contains the size (in
**                             characters) of the buffer mentioned above during
**                             input and receives the size (in characters) of the
**                             custom data during output. If it is NULL, it means
**                             that the caller only wants to know whether there is
**                             custom data but is not interest in the size of the
**                             custom data. If f_pcchCustomData is NULL,
**                             f_pchCustomData must also be NULL.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the custom data is not big enough.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ParseCustomDataByPath(
    __in_bcount( f_cbResponse )       const DRM_BYTE               *f_pbResponse,
    __in                                    DRM_DWORD               f_cbResponse,
    __in                              const DRM_ANSI_CONST_STRING  *f_pdastrPath,
    __out_ecount_opt( *f_pcchCustomData )   DRM_CHAR               *f_pchCustomData,
    __inout                                 DRM_DWORD              *f_pcchCustomData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_SUBSTRING dasstrResponse = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrNode = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchOffset = 0;
    DRM_BYTE bSave = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC__ParseCustomDataByPath );

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkDRMANSIString( f_pdastrPath );
    ChkArg( f_pcchCustomData != NULL || f_pchCustomData == NULL );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                        &dasstrResponse ) );

#endif

    ChkDR( DRM_XML_GetSubNodeByPathA( ( DRM_CHAR * )dastrResponse.pszString,
                                      &dasstrResponse,
                                      f_pdastrPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrNode,
                                      g_chForwardSlash ) );

    ChkBOOL( f_pcchCustomData != NULL, DRM_E_BUFFERTOOSMALL );

    /*
    ** DRM_SOAPXML_DecodeData() only takes an DRM_WORD aligned buffer so some
    ** manipulation is needed when the code is running on a 16-bits addressing
    ** system. The manipulation is to shift the data to be XML decoded one byte
    ** to have it start at a DRM_WORD aligned address. After the XML decoding,
    ** the data is shifted back to its original address. A byte at the end of
    ** the original data will be saved before the shift, and will be restored
    ** after the shift.
    ** Please note that the code assumes there is at least one byte that follows
    ** the data to be XML decoded. Since the data to be XML decoded is always
    ** wrapped in a XML node, the assumption is valid.
    */
    if ( dasstrNode.m_ich % CB_NATIVE_BYTE != 0 )
    {
        /* The buffer needd to be shifted for at most one byte. */
        cchOffset = 1;

        /* Save the one byte that will be overwritten. */
        bSave = GET_BYTE( ( DRM_BYTE * )dastrResponse.pszString, dasstrNode.m_ich + dasstrNode.m_cch );

        /* Shift the custom data string one byte ahead. */
        DRM_BYT_MoveBytes( ( DRM_BYTE * )dastrResponse.pszString,
                           dasstrNode.m_ich + 1,
                           ( DRM_BYTE * )dastrResponse.pszString,
                           dasstrNode.m_ich,
                           dasstrNode.m_cch );
    }

    dr = DRM_SOAPXML_DecodeData( ( DRM_CHAR * )dastrResponse.pszString + __CB_DECL( dasstrNode.m_ich + cchOffset ),
                                   dasstrNode.m_cch,
                                   f_pchCustomData,
                                   f_pcchCustomData );

    if ( dasstrNode.m_ich % CB_NATIVE_BYTE != 0 )
    {
        /* Restore the overwritten byte. */
        DRM_BYT_MoveBytes( ( DRM_BYTE * )dastrResponse.pszString,
                           dasstrNode.m_ich,
                           ( DRM_BYTE * )dastrResponse.pszString,
                           dasstrNode.m_ich + 1,
                           dasstrNode.m_cch );

        PUT_BYTE( ( DRM_BYTE * )dastrResponse.pszString, dasstrNode.m_ich + dasstrNode.m_cch, bSave );
    }

    ChkDR( dr );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_ParseCustomDataForProtocol
**
** Synopsis: Function that tries to parse out the custom data string from
**           the server response using a particular XML path.
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size (number of bytes) of the server response.
** [f_dwProtocolType]       -- Protocol Type ID, member of DRM_SOAPXML_PROTOCOL_TYPE.
** [f_pchCustomData]        -- Pointer to a character buffer to receive the parsed
**                             out custom string. If it is NULL, the function
**                             returns an error and the size of the required buffer
**                             is stored in *f_cchCustomData.
** [f_pcchCustomData]       -- Pointer to a variable that contains the size (in
**                             characters) of the buffer mentioned above during
**                             input and receives the size (in characters) of the
**                             custom data during output. If it is NULL, it means
**                             that the caller only wants to know whether there is
**                             custom data but is not interest in the size of the
**                             custom data. If f_pcchCustomData is NULL,
**                             f_pchCustomData must also be NULL.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SOAPXML_PROTOCOL_NOT_SUPPORTED if the prtocol type given doesn't have an entry in the array.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the custom data is not big enough.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_ParseCustomDataForProtocol(
    __in_bcount( f_cbResponse )     const DRM_BYTE  *f_pbResponse,
    __in                                  DRM_DWORD  f_cbResponse,
    __in                                  DRM_DWORD  f_dwProtocolType,
    __out_ecount_opt( *f_pcchCustomData ) DRM_CHAR  *f_pchCustomData,
    __inout_opt                           DRM_DWORD *f_pcchCustomData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_ParseCustomDataForProtocol );

    ChkArg( f_dwProtocolType <= DRM_SOAPXML_PROTOCOL_MAXIMUM_DEFINED );

    ChkBOOL( IS_PROTOCOL_SUPPORTED(f_dwProtocolType) && g_rgSoapProtoDefines[ f_dwProtocolType ].pdastrCustomDataPath != NULL, DRM_E_SOAPXML_PROTOCOL_NOT_SUPPORTED );

    ChkDR( _ParseCustomDataByPath( f_pbResponse,
                                    f_cbResponse,
                                    g_rgSoapProtoDefines[ f_dwProtocolType ].pdastrCustomDataPath,
                                    f_pchCustomData,
                                    f_pcchCustomData ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_ParseStatusCodeAndChooseBest
**
** Synopsis: Function that parses a server response to look for the
**           status code and whether custom data exists. Also, the
**           caller can pass their current error code and this function
**           will decide whether to overwrite the caller's original code
**           with this new one or pass it as an optional f_pResult output
**           or ignore it.
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size (number of bytes) of the server response.
** [f_drOriginal]           -- Original error code from the calling function
** [f_pResult]              -- Pointer to a variable that receives the status code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SOAPXML_INVALID_STATUS_CODE if the status code
**                          cannot be parsed out from the server response.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_ParseStatusCodeAndChooseBest(
    __in_bcount( f_cbResponse ) const DRM_BYTE    *f_pbResponse,
    __in                              DRM_DWORD    f_cbResponse,
    __in                              DRM_RESULT   f_drOriginal,
    __out_opt                         DRM_RESULT  *f_pResult )
{
    DRM_RESULT dr = f_drOriginal;
    DRM_RESULT drSoapParsing = DRM_SUCCESS;

    if ( f_pResult != NULL )
    {
        *f_pResult = f_drOriginal;

        /* Try to parse out status code from the server response. */
        drSoapParsing = DRM_SOAPXML_ParseStatusCode( f_pbResponse,
                                           f_cbResponse,
                                           f_pResult );

        /* If the SOAP parsing failed and we didn't otherwise fail, we should use that SOAP parse error
        ** code unless it's the error code that happens when there just isn't a code present in the response
        */
        if( !DRM_FAILED(f_drOriginal)
          && drSoapParsing != DRM_E_SOAPXML_XML_FORMAT
          && DRM_FAILED(drSoapParsing) )
        {
            dr = drSoapParsing;
            *f_pResult = drSoapParsing;
        }
    }
    else if( f_pResult == NULL )
    {
        DRM_RESULT drSoap = DRM_SUCCESS;

        /* The caller didn't give us somewhere to output the SOAP error. If there was an error, we
        ** should return the code as part of our normal dr return
        */
        drSoapParsing = DRM_SOAPXML_ParseStatusCode( f_pbResponse,
                                           f_cbResponse,
                                          &drSoap );

        /* If the SOAP parsing failed and we didn't otherwise fail, we should use that SOAP parse error
        ** code unless it's the error code that happens when there just isn't a code present in the response.
        ** Also, if there was a failure code in the SOAP response but no other errors elsewhere, we should
        ** return that.
        */
        if( !DRM_FAILED(f_drOriginal) && drSoapParsing != DRM_E_SOAPXML_XML_FORMAT )
        {
            if( DRM_FAILED(drSoapParsing) )
            {
                /* SOAP parsing failed but the original function didn't, we should return that parsing
                ** error
                */
                dr = drSoapParsing;
            }
            else if( DRM_FAILED( drSoap ) )
            {
                /* There was a failure code in the SOAP message from the server, since nothing else
                ** failed we should return this
                */
                dr = drSoap;
            }
        }
    }

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_ParseStatusCode
**
** Synopsis: Function that parses a server response to look for the
**           status code and whether custom data exists.
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size (number of bytes) of the server response.
** [f_pResult]              -- Pointer to a variable that receives the status code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SOAPXML_INVALID_STATUS_CODE if the status code
**                          cannot be parsed out from the server response.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_ParseStatusCode(
    __in_bcount( f_cbResponse ) const DRM_BYTE   *f_pbResponse,
    __in                              DRM_DWORD   f_cbResponse,
    __out                             DRM_RESULT *f_pResult )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_SUBSTRING dasstrResponse = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrData = EMPTY_DRM_SUBSTRING;
    DRM_BYTE rgbStatusCode[ 2 * DRM_MAX_CCH_BASE16_DWORD_STRING ] = { 0 };
    DRM_DWORD dwStatusCode = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_ParseStatusCode );

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pResult != NULL );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                        &dasstrResponse ) );

#endif

    /* Extract <StatusCode> node from <soap:Fault>/<detail> node. */
    dr = DRM_XML_GetSubNodeByPathA( dastrResponse.pszString,
                                      &dasstrResponse,
                                      &g_dastrSOAPStatusCodePath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrData,
                                      g_chForwardSlash );
    if( DRM_E_XMLNOTFOUND == dr )
    {
        ChkDR( DRM_E_SOAPXML_XML_FORMAT );
    }
    ChkDR( dr );

    ChkOverflow( dasstrData.m_cch + 1, dasstrData.m_cch );

    ChkBOOL( dasstrData.m_cch < SIZEOF( rgbStatusCode ), DRM_E_SOAPXML_INVALID_STATUS_CODE );

    ZEROMEM( rgbStatusCode, SIZEOF( rgbStatusCode ) );

    DRM_BYT_CopyBytes( rgbStatusCode,
                       0,
                       dastrResponse.pszString,
                       dasstrData.m_ich,
                       dasstrData.m_cch );

    /*
    ** Converts string with value to DWORD, count of valid characters is dasstrData.m_cch.
    */
    dr = DRMCRT_AtoDWORD( ( DRM_CHAR * )rgbStatusCode, dasstrData.m_cch, 16, &dwStatusCode );

    if ( DRM_FAILED( dr ) )
    {
        ChkDR( DRM_E_SOAPXML_INVALID_STATUS_CODE );
    }

    *f_pResult = ( DRM_RESULT )dwStatusCode;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*********************************************************************
**
** Function: DRM_SOAPXML_GetAdditionalResponseData
**
** Synopsis: Function that retrieves a piece of data element string from the
**           server response depending on the passed in data element type.
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size (number of bytes) of the server response.
** [f_dwDataType]           -- The data element type of the data string to be
**                             retrieved. Should be one of the DRM_GARD values.
** [f_pchDataString]        -- Pointer to a buffer to receive the data string.
**                             If it is NULL, the function returns an error and
**                             the size of the required buffer is stored in
**                             *f_pcchDataString.
** [f_pcchDataString]       -- Pointer to a variable that contains the size
**                             (in characters) of the buffer to receive the data
**                             string during input and receives the size (in
**                             characters) of the data string during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**                          DRM_E_SOAPXML_DATA_NOT_FOUND if the requested
**                          data is not found.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_GetAdditionalResponseData(
    __in_bcount( f_cbResponse )      const DRM_BYTE   *f_pbResponse,
    __in                                   DRM_DWORD   f_cbResponse,
    __in                                   DRM_DWORD   f_dwDataType,
    __out_ecount_opt( *f_pcchDataString )  DRM_CHAR   *f_pchDataString,
    __inout                                DRM_DWORD  *f_pcchDataString )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_GetAdditionalResponseData );

    switch( f_dwDataType )
    {
    case DRM_GARD_SERVICE_ID:
    case DRM_GARD_ACCOUNT_ID:
        /*
        ** Account ID and Service ID can be in different places in the case of a soap exception
        ** and a successful response
        */
        dr =  _GetAdditionalResponseExceptionData( f_pbResponse,
                                                   f_cbResponse,
                                                   f_dwDataType,
                                                   f_pchDataString,
                                                   f_pcchDataString );
        if ( DRM_E_XMLNOTFOUND == dr )
        {
            ChkDR( _GetAdditionalResponseDomainIDData( f_pbResponse,
                                                       f_cbResponse,
                                                       f_dwDataType,
                                                       f_pchDataString,
                                                       f_pcchDataString ) );
        }
        else
        {
            ChkDR( dr );
        }
        break;
    case DRM_GARD_REDIRECT_URL:
        ChkDR( _GetAdditionalResponseExceptionData( f_pbResponse,
                                                    f_cbResponse,
                                                    f_dwDataType,
                                                    f_pchDataString,
                                                    f_pcchDataString ) );
        break;
    case DRM_GARD_CUSTOM_DATA:
        ChkDR( _ParseCustomData( f_pbResponse,
                                 f_cbResponse,
                                 f_pchDataString,
                                 f_pcchDataString ) );
        break;
    default:
        ChkDR( DRM_E_INVALIDARG );
        break;
    };

ErrorExit:
    if ( dr == DRM_E_XMLNOTFOUND )
    {
        dr = DRM_E_SOAPXML_DATA_NOT_FOUND;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*********************************************************************
**
** Function: Drm_GetAdditionalResponseData
**
** Synopsis: API that retrieves a piece of data element string from the
**           server response depending on the passed in data element type.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size (number of bytes) of the server response.
** [f_dwDataType]           -- The data element type of the data string to be
**                             retrieved. Should be one of the DRM_GARD values.
** [f_pchDataString]        -- Pointer to a buffer to receive the data string.
**                             If it is NULL, the function returns an error and
**                             the size of the required buffer is stored in
**                             *f_pcchDataString.
** [f_pcchDataString]       -- Pointer to a variable that contains the size
**                             (in characters) of the buffer to receive the data
**                             string during input and receives the size (in
**                             characters) of the data string during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**                          DRM_E_SOAPXML_DATA_NOT_FOUND if the requested
**                          data is not found.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_GetAdditionalResponseData(
    __in                                  DRM_APP_CONTEXT  *f_poAppContext,
    __in_bcount( f_cbResponse ) const     DRM_BYTE         *f_pbResponse,
    __in                                  DRM_DWORD         f_cbResponse,
    __in                                  DRM_DWORD         f_dwDataType,
    __out_ecount_opt( *f_pcchDataString ) DRM_CHAR         *f_pchDataString,
    __inout                               DRM_DWORD        *f_pcchDataString )
{
    DRM_RESULT                dr               = DRM_SUCCESS;
    DRM_DWORD                 dwProtocol       = DRM_SOAPXML_PROTOCOL_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_Drm_GetAdditionalResponseData );

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pcchDataString != NULL );
    ChkArg( f_pchDataString == NULL || *f_pcchDataString > 0 );

    /*
    ** Try to validate the signature before accessing the data.
    */
    ChkDR( _GetProtocol( f_pbResponse, f_cbResponse, &dwProtocol ) );

    ChkDR( DRM_SOAPXML_GetAdditionalResponseData( f_pbResponse,
                                                  f_cbResponse,
                                                  f_dwDataType,
                                                  f_pchDataString,
                                                  f_pcchDataString ) );
ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_GetAdditionalResponseData, dr );

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_EnsureTagValueNativeByteAligned
**
** Synopsis: Function that ensures the value of the XML tag begin at
**           a native byte aligned address in the ANSI/UTF8 XML builder
**           context.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_pdstrTagName]         -- Pointer to DRM ANSI string of the XML tag name.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_EnsureTagValueNativeByteAligned(
    __inout    _XMBContextA           *f_poXMLContext,
    __in const DRM_ANSI_CONST_STRING  *f_pdastrTagName )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR *pchCurr = NULL;
    DRM_BOOL fAdjust = FALSE;

    ChkArg( f_poXMLContext != NULL );
    ChkDRMANSIString( f_pdastrTagName );

    /*
    ** Get the current location of the XML builder context.
    ** This is the location to store the next data to be added
    ** to the XML builder context.
    */
    ChkDR( DRM_XMB_GetCurrentBufferPointerA( f_poXMLContext,
                                             FALSE,
                                             &pchCurr ) );

    DRMASSERT( pchCurr != NULL );

    /* Check whether the XML tag value can be aligned at the native byte boundary. */
    fAdjust = ( ( ( DRM_DWORD_PTR )( pchCurr + f_pdastrTagName->cchString ) % CB_NATIVE_BYTE ) != 0 );

    if ( fAdjust )
    {
        *pchCurr = ' ';

         /*
         ** If the XML tag value is not going to be native byte aligned,
         ** shift the current XML builder context pointer forward by one character.
         */
         ChkDR( DRM_XMB_ShiftCurrentPointerA( f_poXMLContext, 1 ) );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_BuildClientInfo
**
** Synopsis: Function that constructs a client information XML segment
**           that is part of a challenge XML
**
** Arguments:
**
** [f_poXMLContext]          -- Pointer to a XML builder context.
** [f_pszString]             -- Pointer to pre-allocated buffer
** [f_cchBuffer]             -- Maxmimum size ( number of character count )
**                              of the temporary buffer uses in this
**                              function.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SOAPXML_BuildClientInfo(
    __inout                   _XMBContextA     *f_poXMLContext,
    __in_opt                  DRM_CLIENT_INFO  *f_poClientInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_SOAPXML, PERF_FUNC_DRM_SOAPXML_BuildClientInfo );

    ChkArg( f_poXMLContext != NULL );

    /* Client info is optional, only add it if the client version is present. In the future,
       if/when more fields are added to client info, the "if" condition should be updated
       accordingly. */
    if ( (NULL != f_poClientInfo) &&
         (f_poClientInfo->m_dastrClientVersion.cchString > 0) )
    {
        /*
        ** Add and open <ClientInfo> node.
        **
        ** Output:
        ** <ClientInfo>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                 &g_dastrReqTagClientInfo,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        /*
        ** Add and close <ClientVersion> node.
        **
        ** Output:
        ** <ClientInfo>
        **      <ClientVersion>...</ClientVersion>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrReqTagClientVersion,
                                  (DRM_ANSI_CONST_STRING *) &f_poClientInfo->m_dastrClientVersion,
                                  NULL,
                                  NULL,
                                  wttClosed ) );

        /*
        ** Close <ClientInfo> node.
        **
        ** Output:
        ** <ClientInfo>
        **      ...
        ** </ClientInfo>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }



ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_SOAPXML_CalcClientInfo
**
** Synopsis: Function that calculates the character count of the client
**           infomation XML segment
**
** Arguments:
**                          N/A
**
** Returns:                 A long integer value that is the character
**                          count of the client information XML segment.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_DWORD DRM_CALL DRM_SOAPXML_CalcClientInfo( __in_opt DRM_CLIENT_INFO *f_poClientInfo )
{
    DRM_DWORD  cchCount = 0;

    /* Client info is optional, only add the calculation if client version is present. In the future,
       if/when more fields are added to client info, the "if" condition should be updated
       accordingly. */
    if ( (NULL != f_poClientInfo) &&
         (f_poClientInfo->m_dastrClientVersion.cchString > 0) )
    {
        /* <ClientInfo> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrReqTagClientInfo.cchString,
                                                  0,
                                                  0,
                                                  0 );

        /* <ClientVersion> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrReqTagClientVersion.cchString,
                                                  0,
                                                  0,
                                                  0 );

        cchCount += f_poClientInfo->m_dastrClientVersion.cchString;
    }

    return cchCount;
}

EXIT_PK_NAMESPACE_CODE;
