/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmxmlparser.h>
#include <drmxmlbuilder.h>
#include <drmxmlsigconstants.h>
#include <drmxmlsig.h>
#include <drmmeterconstants.h>
#include <drmheaderparser.h>
#include <drmenvelope.h>
#include <V3MeteringTestHelpers.h>
#include <drmsoapxmlutility.h>
#include <drmsoapxmlconstants.h>

ENTER_PKTEST_NAMESPACE_CODE
#define MAX_PACKET_BUFFER 12000

static const DRM_GUID g_guidASFHeaderObject         = { 0x75b22630, 0x668e, 0x11cf, { 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c } };
static const DRM_GUID g_guidPlayreadyHeaderObject   = { 0x9a04f079, 0x9840, 0x4286, { 0xab, 0x92, 0xe6, 0x5b, 0xe0, 0x88, 0x5f, 0x95 } };

static const DRM_GUID g_guidASFExtendedHeaderObject = { 0x5FBF03B5, 0xA92E, 0x11CF, { 0x8E, 0xE3, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 } };

/*********************************************************************
**
** Function: _CalcSHA256Hash
**
** Synopsis: Function that generates a SHA-256 hash of the passed in data.
**
** Arguments:
**
** [f_pdastrData]           -- Pointer to a DRM ANSI string for which a
**                             SHA-256 hash is generated.
** [f_rgbHash]              -- Pointer to a buffer that receives a generated
**                             SHA-256 hash.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CalcSHA256Hash(
    __in                                const DRM_ANSI_CONST_STRING *f_pdastrData,
    __out_bcount(SHA256_DIGEST_SIZE_IN_BYTES) DRM_BYTE               f_rgbHash[ __CB_DECL( SHA256_DIGEST_SIZE_IN_BYTES ) ] )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_SHA_CONTEXT oSHAContext = { 0 };

    ChkDRMANSIString( f_pdastrData );
    ChkArg( f_rgbHash != NULL );

    /* Generate a SHA-1 hash of the passed in data. */
    ChkDR( DRM_SHA_Init( &oSHAContext, eDRM_SHA_256 ) );

    ChkDR( DRM_SHA_Update( PB_DASTR( f_pdastrData ),
                           CB_DASTR( f_pdastrData ),
                           &oSHAContext ) );

    ChkDR( DRM_SHA_Finalize( &oSHAContext,
                             SHA256_DIGEST_SIZE_IN_BYTES,
                             f_rgbHash ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _DO_NOT_USE_ExtractCipherData
**
** Synopsis: Function that extracts a cipher data buffer contained in
**           a XML segment and applies B64 decoder.
**           This function does not decrypt cipher data.
**           The XML segment should be in the form of
**           <...>/<CipherData>/<CipherValue>, and the name of the root
**           node can be any string.
**
**      IMPORTANT NOTE: THIS FUNCTION IS A COPY OF
**      _ExtractCipherData to allow the decryption of the
**      MeteringData Encrypted data nodes with the server private keys.
**      It is NOT intended for use outside of this test file.
**
** Arguments:
**
** [f_pszBase]              -- base XML data string for f_pdasstrXML
**                             and other substrings.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment whose root node contains a
**                             <CipherData> subnode.
** [f_ppbCipherData]        -- Pointer to a pointer to a buffer that
**                             receives the B64 decoded cipher data
**                             contained in the XML segment.
** [f_pcbCipherData]        -- Pointer to a variable to receive the size
**                             (number of bytes) of the B64 decoded cipher
**                             data contained in the XML segment.
** [f_pdasstrCipherData]    -- Pointer to a variable to receive the a
**                             substring that cover the encoded cipher data.
**                             It is the caller's responsiblity to cover
**                             the the substring with space (' ') to
**                             ensure the whole XML string is still valid
**                             for future parsing.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT DRM_CALL _DO_NOT_USE_ExtractCipherData(
    IN const DRM_CHAR *f_pszBase,
    IN const DRM_SUBSTRING *f_pdasstrXML,
    OUT DRM_BYTE **f_ppbCipherData,
    OUT DRM_DWORD *f_pcbCipherData,
    OUT DRM_SUBSTRING *f_pdasstrCipherData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrCipherData = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchExtra = 0;

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_ppbCipherData != NULL );
    ChkArg( f_pcbCipherData != NULL );

    *f_ppbCipherData = NULL;
    *f_pcbCipherData = 0;

    /*
    ** Extract <CipherValue> node from the passed in root node.
    ** It contains the cipher data string.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszBase,
                                      f_pdasstrXML,
                                      &g_dastrSigCipherValuePath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrCipherData,
                                      g_chForwardSlash ) );

    if ( f_pdasstrCipherData != NULL )
    {
        *f_pdasstrCipherData = dasstrCipherData;
    }

    *f_pcbCipherData = dasstrCipherData.m_cch;

    if ( *f_pcbCipherData > 0 )
    {
        ChkDR( DRM_B64_DecodeA( f_pszBase,
                                &dasstrCipherData,
                                f_pcbCipherData,
                                NULL,
                                DRM_BASE64_DECODE_IN_PLACE ) );

        if ( dasstrCipherData.m_ich % SIZEOF( DRM_DWORD ) != 0 )
        {
            /* If the encoded string is not beginning at an aligned address,
            ** shift it to make aligned.
            */
            cchExtra = SIZEOF( DRM_DWORD ) - ( dasstrCipherData.m_ich % SIZEOF( DRM_DWORD ) );

            /* *f_ppbCipherData will point to an aligned address. */
            *f_ppbCipherData = ( DRM_BYTE * )f_pszBase + __CB_DECL( dasstrCipherData.m_ich + cchExtra );

            DRM_BYT_MoveBytes( *f_ppbCipherData,
                               0,
                               ( DRM_BYTE * )f_pszBase,
                               dasstrCipherData.m_ich,
                               *f_pcbCipherData );
        }
        else
        {
            /* The encoded string is beginning at an aligned address. */
            *f_ppbCipherData = ( DRM_BYTE * )f_pszBase + __CB_DECL( dasstrCipherData.m_ich );
        }
    }
    else
    {
        /* If <CipherValue> does not have any content, something is wrong. */
        ChkDR( DRM_E_XMLSIG_FORMAT );
    }

ErrorExit:

    return dr;
}


/*********************************************************************
**
** Function: _DO_NOT_USE_ExtractEncryptedKey
**
** Synopsis: Function that extracts and decrypts an AES key contained
**           in a XML segment.
**           The XML segment should have a root node named <EncryptedKey>.
**           Under it there are three subnodes:
**           1. <EncryptionMethod> that contains the method used to do
**              the encryption.
**           2. <KeyInfo> that contains an ECC-256 public key used to
**              encrypt the AES key. Its matching ECC-256 private key
**              should be used to decrypt the AES key.
**           3. <CipherData> that contains the encrypted AES key.
**
**      IMPORTANT NOTE: THIS FUNCTION IS ***MOSTLY**** A COPY OF
**      _ExtractEncryptedKey to allow the decryption of the
**      MeteringData Encrypted data nodes with the server private keys.
**      It is NOT intended for use outside of this test file.
**
** Arguments:
**
** [f_pPrivKey]             -- PRIVKEY_P256.
** [f_pszBase]              -- base XML data string, all substrings
**                             are operating on it.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment whose root node contains a
**                             <CipherData> subnode.
** [f_ppbKey]               -- Pointer to a pointer to a buffer that
**                             receives the decrypted AES key contained
**                             in the XML segment.
** [f_pcbKey]               -- Pointer to a variable to receive the size
**                             (number of bytes) of the decrypted AES key.
** [f_pdasstrKey]           -- Pointer to a variable to receive the a
**                             substring that cover the encoded key.
**                             It is the caller's responsiblity to cover
**                             the the substring with space (' ') to
**                             ensure the whole XML string is still valid
**                             for future parsing.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT DRM_CALL _DO_NOT_USE_ExtractEncryptedKey(
    IN PRIVKEY_P256   *f_pPrivKey,
    IN const DRM_CHAR *f_pszBase,
    IN const DRM_SUBSTRING *f_pdasstrXML,
    OUT DRM_BYTE **f_ppbKey,
    OUT DRM_DWORD *f_pcbKey,
    OUT DRM_SUBSTRING *f_pdasstrKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrKeyInfo  = EMPTY_DRM_SUBSTRING;
    PLAINTEXT_P256  oPlainText      = { 0 };
    CIPHERTEXT_P256 oCipherText     = { 0 };
    DRM_CRYPTO_CONTEXT *pCryptoContext = NULL;

    ChkArg( f_pPrivKey != NULL );
    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_ppbKey != NULL );
    ChkArg( f_pcbKey != NULL );
    ChkArg( f_pdasstrKey != NULL );

    *f_ppbKey = NULL;
    *f_pcbKey = 0;

    ChkMem( pCryptoContext = ( DRM_CRYPTO_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );

    /*
    ** Extract <KeyInfo> node from <EncryptedKey> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszBase,
                                      f_pdasstrXML,
                                      &g_dastrSigKeyInfoPath,
                                      NULL,
                                      NULL,
                                      &dasstrKeyInfo,
                                      NULL,
                                      g_chForwardSlash ) );


    /*
    ** Extract the the encrypted AES key from the <CipherValue> node.
    */
    ChkDR( _DO_NOT_USE_ExtractCipherData( f_pszBase,
                               f_pdasstrXML,
                               f_ppbKey,
                               f_pcbKey,
                               f_pdasstrKey ) );

    /*
    ** ECC-256 decrypt the AES key in place using the matching private key of
    ** the public key.
    ** It assumes the size of the ECC-256 decrypted AES key can be contained in
    ** the original buffer, and the size of the ECC-256 decrypted AES key is
    ** equal or smaller than the size of the ECC-256 encrypted AES key.
    */

    ZEROMEM( &oPlainText, SIZEOF( PLAINTEXT_P256 ) );
    ZEROMEM( &oCipherText, SIZEOF( CIPHERTEXT_P256 ) );

    ChkBOOL( *f_pcbKey == SIZEOF( CIPHERTEXT_P256 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    MEMCPY( oCipherText.m_rgbCiphertext, *f_ppbKey, *f_pcbKey );

    ChkDR( OEM_ECC_Decrypt_P256( f_pPrivKey,
                                 &oCipherText,
                                 &oPlainText,
                                 (struct bigctx_t *)pCryptoContext ) );

    MEMCPY( *f_ppbKey, oPlainText.m_rgbPlaintext, ECC_P256_PLAINTEXT_SIZE_IN_BYTES );

    *f_pcbKey = ECC_P256_PLAINTEXT_SIZE_IN_BYTES;

    /* Zero the temoprary decoded buffer to avoid leaking secrets in the heap. */
    OEM_SECURE_ZERO_MEMORY( &oPlainText, SIZEOF( PLAINTEXT_P256 ) );

ErrorExit:
    SAFE_OEM_FREE( pCryptoContext );

    return dr;
}


/*********************************************************************
**
** Function: _DO_NOT_USE_ExtractEncryptedData
**
** Synopsis: Function that extracts and decrypts a data buffer contained
**           in a XML segment.
**           The XML segment should have a root node named <EncryptedData>
**           and should have three subnodes under it. They are:
**           1. <EncryptionMethod> that contains the method used to do
**              the encryption.
**           2. <KeyInfo> that contains an AES key used to encrypt the
**              data.
**           3. <CipherData> that contains the encrypted data.
**
**      IMPORTANT NOTE: THIS FUNCTION IS ***MOSTLY**** A COPY OF
**      DRM_XMLSIG_ExtractEncryptedData to allow the decryption of the
**      MeteringData Encrypted data nodes with the server private keys.
**      It is NOT intended for use outside of this test file.
**
** Arguments:
**
** [f_pPrivKey]             -- Pointer to PRIVKEY_P256
** [f_pszBase]              -- base XML data string, all substrings
**                             are operating on it.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment of the whole <EncryptedData>.
** [f_pbData]               -- Pointer to to a buffer that receives the
**                             decrypted data contained in the XML segment.
** [f_pcbData]              -- Pointer to a variable to specify the size
**                             (number of bytes) of the buffer to store
**                             the decrypted data, It receives the actual
**                             size (number of bytes) of the decrypted
**                             data during output.
** [f_pbKey]                -- Pointer to receive AES Key if not NULL
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL _DO_NOT_USE_ExtractEncryptedData(
    IN PRIVKEY_P256   *f_pPrivKey,
    IN const DRM_CHAR *f_pszBase,
    IN const DRM_SUBSTRING *f_pdasstrXML,
    OUT DRM_BYTE *f_pbData,
    IN OUT DRM_DWORD *f_pcbData,
    OUT DRM_BYTE     *f_pbKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrEncryptedKey = EMPTY_DRM_SUBSTRING;
    DRM_BYTE *pbKey = NULL;
    DRM_DWORD cbKey = 0;
    DRM_BYTE *pbCipherData = NULL;
    DRM_DWORD cbCipherData = 0;
    DRM_AES_KEY oAESKey = { 0 };
    DRM_SUBSTRING dasstrKey = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrCipherData = EMPTY_DRM_SUBSTRING;

    ChkArg( f_pPrivKey != NULL );
    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_pbData != NULL );
    ChkArg( f_pcbData != NULL );

    /*
    ** Extract <EncryptedKey> node from <EncryptedData> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszBase,
                                      f_pdasstrXML,
                                      &g_dastrSigEncryptedKeyPath,
                                      NULL,
                                      NULL,
                                      &dasstrEncryptedKey,
                                      NULL,
                                      g_chForwardSlash ) );

    /* Extract AES key from <EncryptedKey> node. */
    ChkDR( _DO_NOT_USE_ExtractEncryptedKey( f_pPrivKey,
                                           f_pszBase,
                                           &dasstrEncryptedKey,
                                           &pbKey,
                                           &cbKey,
                                           &dasstrKey ) );

    ChkBOOL( cbKey == DRM_XML_AES_KEY_SIZE, DRM_E_XMLSIG_FORMAT );

    if( f_pbKey != NULL )
	{
        MEMCPY( f_pbKey, &pbKey[ DRM_AES_KEYSIZE_128 ], DRM_AES_KEYSIZE_128 );
	}

    ChkDR( Oem_Aes_SetKey( &pbKey[ DRM_AES_KEYSIZE_128 ],
                           &oAESKey ) );

    /*
    ** Set all bytes of the decoded string to ' ' to prevent future parsing
    ** error of the current XML string.
    */
    DRM_BYT_SetBytes( ( DRM_BYTE * )f_pszBase,
                      dasstrKey.m_ich,
                      dasstrKey.m_cch,
                      ' ' );

    /* Extract the data to be AES decrypted. */
    ChkDR( _DO_NOT_USE_ExtractCipherData( f_pszBase,
                               f_pdasstrXML,
                               &pbCipherData,
                               &cbCipherData,
                               &dasstrCipherData ) );

    if ( cbCipherData < DRM_AES_KEYSIZE_128 )
    {
        ChkDR( DRM_E_XMLSIG_FORMAT );
    }

    /* Discount the size of the first DRM_AES_KEYSIZE_128 bytes of IV. */
    cbCipherData -= DRM_AES_KEYSIZE_128;

    if ( *f_pcbData >= cbCipherData )
    {
        *f_pcbData = cbCipherData;
    }
    else
    {
        /*
        ** If the size of the passed in buffer is smaller than the size of
        ** the decrypted data, exit with an error code.
        */
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** AES decrypt the cipher data.
    ** The first DRM_AES_KEYSIZE_128 bytes of pbCipherData is the IV.
    */
    ChkDR( Oem_Aes_CbcDecryptData( &oAESKey,
                                   pbCipherData + __CB_DECL( DRM_AES_KEYSIZE_128 ),
                                   cbCipherData,
                                   pbCipherData ) );

    MEMCPY( f_pbData, pbCipherData + __CB_DECL( DRM_AES_KEYSIZE_128 ), *f_pcbData );

    /*
    ** Set all bytes of the decoded string to ' ' to prevent future parsing
    ** error of the current XML string.
    */
    DRM_BYT_SetBytes( ( DRM_BYTE * )f_pszBase,
                      dasstrCipherData.m_ich,
                      dasstrCipherData.m_cch,
                      ' ' );

ErrorExit:

    return dr;
}

/* "challenge/Challenge/MeteringData/EncryptedData" */
const DRM_ANSI_CONST_STRING g_dastrMeterDataEncryptedDataPath_Local = CREATE_DRM_ANSI_STRING( "challenge/Challenge/MeteringData/EncryptedData" );

/*****************************************************************************
**
** Function:    _IsEnvelope
**
** Synopsis:    Tests the first DWORD of the file pointer given to see if it matches
**              the signature of a PlayReady Envelope file.
**
** Arguments:
**
** [f_hFile]         -- an opened file handle
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
static DRM_BOOL DRM_CALL _IsEnvelope( __in OEM_FILEHDL f_hFile )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwSignature = 0;
    DRM_DWORD  cbRead      = 0;

    ChkArg( f_hFile != OEM_INVALID_HANDLE_VALUE );

    ChkBOOL( Oem_File_Read( f_hFile, ( DRM_BYTE * )&dwSignature, SIZEOF( dwSignature ), &cbRead ), DRM_E_FAIL );

    ChkBOOL( dwSignature == DRM_ENVELOPE_FILE_SIGNATURE, DRM_E_FAIL );

ErrorExit:
    /*
    ** No matter what the result is, reset the file pointer to the beginning of the file
    */
    if( f_hFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_SetFilePointer( f_hFile, 0, OEM_FILE_BEGIN, NULL );
    }
    return DRM_SUCCEEDED( dr );
}

/*****************************************************************************
**
** Function:    _FindPROInEnvelope
**
** Synopsis:    moves the file pointer to the beginning of the DRM header in
**              an envelope file.
**
** Arguments:
**
** [f_hFile]                -- Open File handle to search for the DRM Header
** [f_pcbDRMHeaderData]     -- output pointer to receive the size of the header
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
static DRM_RESULT DRM_CALL _FindPROInEnvelope(
    __in          OEM_FILEHDL                 f_hFile,
    __out         DRM_DWORD                  *f_pcbDRMHeaderData)
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_DWORD                 cbRead               = 0;
    DRM_DWORD                 cbOriginalFilename   = 0;
    DRM_DWORD                 cbDrmHeaderLen       = 0;

    /*
    ** Temporary buffer to read the envelope header into. Due to slack in the struct, the
    ** actual envelope header could potentially be smaller than SIZEOF( DRM_ENVELOPE_HEADER )
    ** but should never be larger
    */
    DRM_BYTE rgbHeaderBuffer[SIZEOF( DRM_ENVELOPE_HEADER )] = {0};

    ChkArg( f_hFile    != OEM_INVALID_HANDLE_VALUE );

    /*
    ** Read the file header.
    */
    ChkBOOL( Oem_File_Read(  f_hFile,
                             rgbHeaderBuffer,
                             DRM_ENVELOPE_MINIMUM_HEADER_SIZE,
                            &cbRead ), DRM_E_FILEREADERROR );

    ChkBOOL( cbRead == DRM_ENVELOPE_MINIMUM_HEADER_SIZE, DRM_E_FILEREADERROR );

    /*
    ** Skip most of the envelope header data. we only want the DRM header
    */
    cbRead = 0;
    cbRead += SIZEOF( DRM_DWORD ); /* Signature */
    cbRead += SIZEOF( DRM_DWORD ); /* Envelope Header Size */

    cbRead += SIZEOF( DRM_DWORD ); /* Data Offset */
    cbRead += SIZEOF( DRM_WORD );  /* Version */
    cbRead += SIZEOF( DRM_WORD );  /* Compatible Version */
    cbRead += SIZEOF( DRM_DWORD ); /* Cipher Type */
    cbRead += DRM_ENVELOPE_CIPHER_DATA_SIZE;

    LITTLEENDIAN_BYTES_TO_WORD( cbOriginalFilename, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_WORD );

    LITTLEENDIAN_BYTES_TO_DWORD( cbDrmHeaderLen, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    /*
    ** Check that the header length is valid.
    */
    ChkBOOL( cbDrmHeaderLen % SIZEOF( DRM_WCHAR ) == 0, DRM_E_ENVELOPE_CORRUPT );

    /*
    ** Skip over the original filename.
    */
    ChkBOOL( Oem_File_SetFilePointer( f_hFile,
                                      cbOriginalFilename,
                                      OEM_FILE_CURRENT,
                                      NULL ), DRM_E_FILESEEKERROR );

    *f_pcbDRMHeaderData = cbDrmHeaderLen;

ErrorExit:

    return dr;
}

/*****************************************************************************
**
** Function:    _FindPROInASFFile
**
** Synopsis:    moves the file pointer to the beginning of the DRM header in
**              an ASF file.
**
** Arguments:
**
** [f_pFileHandle]          -- Open File handle to search for the DRM Header
** [f_pcbPlayreadySize]     -- output pointer to receive the size of the header
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
static DRM_RESULT _FindPROInASFFile(
                                __in  OEM_FILEHDL  f_pFileHandle ,
                                __out DRM_DWORD   *f_pcbPlayreadySize )
{
    DRM_RESULT dr              = DRM_SUCCESS;
    DRM_DWORD  cbFile          = 0;
    DRM_DWORD  cbRead          = 0;
    DRM_UINT64 cbHeaderObj     = DRM_UI64( 0 );
    DRM_BYTE  *pbHeaderData    = NULL;
    DRM_UINT64 cbHeaderData    = DRM_UI64( 0 );
    DRM_GUID   guidHeader      = {0};
    DRM_UINT64 qwHeaderObjSize = DRM_UI64( 0 );
    DRM_UINT64 cbExamined      = DRM_UI64( 0 );
    DRM_UINT64 cbOffset        = DRM_UI64( 0 );
    DRM_BOOL   fPROFound       = FALSE;

    DRM_BYTE   *pbExtendedHeaderData       = NULL;
    DRM_DWORD   cbReadExtend               = 0;
    DRM_UINT64  qwOffsetInExtendedHeader   = DRM_UI64( 0 );
    DRM_UINT64  qwExtendedHeaderObjSize    = DRM_UI64( 0 );
    DRM_UINT64  qwExtendedHeaderDataSize   = DRM_UI64( 0 );
    DRM_UINT64  cbExtendedHeaderObj        = DRM_UI64( 0 );
    DRM_UINT64  cbExaminedInExtendedHeader = DRM_UI64( 0 );

    ChkArg( f_pFileHandle  != NULL );
    ChkArg( f_pFileHandle  != OEM_INVALID_HANDLE_VALUE );
    ChkFAIL( Oem_File_GetSize( f_pFileHandle , &cbFile ) );

    /*
    ** Header object structure is represented as the following:
    **
    ** Field Name         | Field Type
    ** ----------------------------
    ** Object ID            DRM_GUID
    ** Object Size          DRM_UINT64
    ** # of H. Objects      DRM_DWORD
    ** Reserved Flag1       DRM_BYTE
    ** Reserved Flag2       DRM_BYTE
    */

    /*
    ** cbHeaderObj is the sum of all Field Sizes in an ASF Header
    */
    cbHeaderObj = DRM_UI64Add( DRM_UI64Add( DRM_UI64Add( DRM_UI64( SIZEOF( DRM_GUID ) ),
                                                         DRM_UI64( SIZEOF( DRM_UINT64 ) ) ),
                                            DRM_UI64Add( DRM_UI64( SIZEOF( DRM_DWORD ) ),
                                                         DRM_UI64( SIZEOF( DRM_BYTE ) ) ) ),
                               DRM_UI64( SIZEOF( DRM_BYTE ) ) );

    /*
    ** If the file isn't as big as the expected header size, we can't parse it.
    */
    ChkBOOL( DRM_UI64Les( cbHeaderObj, DRM_UI64( cbFile ) ) ||
             DRM_UI64Eql( cbHeaderObj, DRM_UI64( cbFile ) ),
             DRM_E_TEST_INVALID_FILE );

    /*
    ** Read the ASF Header Object
    */
    ChkBOOL( Oem_File_Read( f_pFileHandle , ( DRM_BYTE* ) &guidHeader, SIZEOF( DRM_GUID ), &cbRead ) && cbRead == SIZEOF( DRM_GUID ),
             DRM_E_FILEREADERROR );

    /*
    ** Check the first few bytes against the expected ASF Header DRM_GUID - fail if it doesn't match
    */
    if( MEMCMP( ( DRM_BYTE * )&guidHeader, ( DRM_BYTE * )&g_guidASFHeaderObject, SIZEOF( DRM_GUID ) ) != 0 )
    {
        ChkDR( DRM_E_TEST_INVALID_FILE );
    }

    /*
    ** If the first bits match the ASF Header Obj DRM_GUID, read the Object Size from the header.
    */
    cbRead = 0;
    ChkBOOL( Oem_File_Read( f_pFileHandle , ( DRM_BYTE * )&qwHeaderObjSize, SIZEOF( DRM_UINT64 ), &cbRead ) && cbRead == SIZEOF( DRM_UINT64 ),
             DRM_E_FILEREADERROR );


    /*
    ** if the header size is bigger than the size of the file, fail.
    */
    if( DRM_UI64Les( DRM_UI64( cbFile ), qwHeaderObjSize ) )
    {
        ChkDR( DRM_E_TEST_INVALID_FILE );
    }

    /*
    ** Data section is the objet size minus the first 30 bytes.
    */
    cbHeaderData = DRM_UI64Sub( qwHeaderObjSize, cbHeaderObj );

    ChkBOOL( DRM_UI64High32( cbHeaderData ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
    ChkMem( pbHeaderData = ( DRM_BYTE * ) Oem_MemAlloc( DRM_UI64Low32( cbHeaderData ) ) );

    cbRead = 0;
    ChkBOOL( Oem_File_Read( f_pFileHandle , pbHeaderData, DRM_UI64Low32( cbHeaderData ), &cbRead ) && DRM_UI64Eql( DRM_UI64( cbRead ), cbHeaderData ),
             DRM_E_FILEREADERROR );


    /*
    ** Skip past the extra fields ( Num Header Obj, Reserved 1 & 2 ) in the ASF Header
    */
    cbExamined = DRM_UI64( SIZEOF( DRM_DWORD ) + ( 2 * SIZEOF( DRM_BYTE ) ) );

    /*
    ** Cycle tdrough the objects in the ASF Header and look for the Playready Object
    */
    while( DRM_UI64Les( cbExamined, cbHeaderData ) && !fPROFound )
    {

        /*
        ** If the DRM_GUID of the object matches the Playready DRM_GUID
        ** return the filehandle pointing to the PRO, set the offset, and set fPROFound to true
        */
        ChkBOOL( DRM_UI64High32( cbExamined ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
        if( MEMCMP( ( DRM_BYTE * ) ( pbHeaderData + DRM_UI64Low32( cbExamined ) ), ( DRM_BYTE * ) &g_guidPlayreadyHeaderObject, SIZEOF( DRM_GUID ) ) == 0 )
        {
            /*  If the PlayReady Object exists, set the fHasPRO to true,
            **  and reset the file pointer within the handler to point to the beginning of the PRO data.
            **
            **  PRO Object Construction
            **
            **  Field Name         | Field Type
            **  ----------------------------
            **  Object ID            DRM_GUID
            **  Object Size          DRM_UINT64
            **  System ID            DRM_GUID
            **  System Version       DRM_DWORD
            **  Data Size            DRM_DWORD
            **  Data                 DRM_BYTE
            **
            */

            fPROFound = TRUE;

            /*
            ** The offset should be set to the PRO data offset, located in the Data field
            */
            cbOffset = DRM_UI64Add( cbExamined,
                     DRM_UI64( ( ( 2 * SIZEOF( DRM_GUID ) ) + SIZEOF( DRM_UINT64 ) + ( 2 * SIZEOF( DRM_DWORD ) ) ) /* Skip the extra fields in the ASF Object Header */
                     + ( SIZEOF( DRM_GUID ) + SIZEOF( DRM_UINT64 ) ) ) );                                /* Skip the ASF Header DRM_GUID & Obj Size */

            /*
            ** Find the PRObject size, located in the Data Size field
            */
            *f_pcbPlayreadySize = *( DRM_DWORD * )( pbHeaderData
                                            + DRM_UI64Low32( cbExamined )
                                            + ( ( 2 * SIZEOF( DRM_GUID ) ) + SIZEOF( DRM_UINT64 ) + SIZEOF( DRM_DWORD ) ) );

            /*
            ** Reset the File pointer to point to the beginning of the data field
            */
            ChkBOOL( DRM_UI64High32( cbOffset ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
            ChkBOOL( Oem_File_SetFilePointer( f_pFileHandle, DRM_UI64Low32( cbOffset ), OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );

        } /* If the DRM_GUID doesn't match, skip ahead to the next object. */
        else if( MEMCMP( ( DRM_BYTE * ) ( pbHeaderData + DRM_UI64Low32( cbExamined ) ), ( DRM_BYTE * ) &g_guidASFExtendedHeaderObject, SIZEOF( DRM_GUID ) ) == 0 )
        {
            /*
            ** Extended Header object structure is represented as the following:
            **
            ** Field Name                         | Field Type              | size  (bits)
            ** ----------------------------
            ** Object ID                            DRM_GUID                  128
            ** Object Size                          DRM_UINT64                64
            ** Reserved Field1                      DRM_GUID                  128
            ** Reserved Field2                      DRM_WORD                  16
            ** Header Extention Data size           DRM_DWORD                 32
            ** Header Extention Data                DRM_BYTE                  varies
            */

            /*
            ** cbExtendedHeaderObj is the sum of all Field Sizes in an ASF extended Header
            */
            cbExtendedHeaderObj = DRM_UI64( SIZEOF( DRM_GUID ) + SIZEOF( DRM_UINT64 ) + SIZEOF( DRM_GUID ) + SIZEOF( DRM_WORD ) + SIZEOF( DRM_DWORD ) );
            qwOffsetInExtendedHeader =  DRM_UI64Add( DRM_UI64( SIZEOF( DRM_GUID ) + SIZEOF( DRM_UINT64 ) + SIZEOF( DRM_GUID ) ), cbExamined );


            /*
            ** Reset the File pointer to point to the beginning of the data field
            */
            ChkBOOL( DRM_UI64High32( qwOffsetInExtendedHeader ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
            ChkBOOL( Oem_File_SetFilePointer( f_pFileHandle, DRM_UI64Low32( qwOffsetInExtendedHeader ), OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );

            /*
            ** If the first bits match the ASF extended Header Obj DRM_GUID, read the Object Size from the header.
            */
            cbReadExtend = 0;
            ChkBOOL( Oem_File_Read( f_pFileHandle , ( DRM_BYTE * )&qwExtendedHeaderObjSize, SIZEOF( DRM_UINT64 ), &cbReadExtend ) && cbReadExtend == SIZEOF( DRM_UINT64 ),
                 DRM_E_FILEREADERROR );

            /*
            ** cbOffsetInExtendedHeader = cbHeaderObj + cbExamined + cbExtendedHeaderObj;
            */
            qwOffsetInExtendedHeader = DRM_UI64Sub( DRM_UI64Add( DRM_UI64Add( DRM_UI64( SIZEOF( DRM_GUID ) + SIZEOF( DRM_UINT64 ) ), cbExamined ),
                                                                 cbExtendedHeaderObj ),
                                                    DRM_UI64( SIZEOF( DRM_DWORD ) ) );


            /*
            ** Reset the File pointer to point to the beginning of the data field
            */
            ChkBOOL( DRM_UI64High32( qwOffsetInExtendedHeader ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
            ChkBOOL( Oem_File_SetFilePointer( f_pFileHandle, DRM_UI64Low32( qwOffsetInExtendedHeader ), OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );

            /*
            ** read the Object data Size from the header. it should be equal to the object size field minus 46 bytes.
            */
            cbReadExtend = 0;
            ChkBOOL( Oem_File_Read( f_pFileHandle , ( DRM_BYTE * )&qwExtendedHeaderDataSize, SIZEOF( DRM_DWORD ), &cbReadExtend ) && cbReadExtend == SIZEOF( DRM_DWORD ),
                    DRM_E_FILEREADERROR );

            ChkBOOL( DRM_UI64Eql( DRM_UI64Sub( qwExtendedHeaderObjSize, qwExtendedHeaderDataSize ), cbExtendedHeaderObj ), DRM_E_TEST_INVALID_FILE );

            ChkBOOL( DRM_UI64High32( qwExtendedHeaderDataSize ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
            ChkMem( pbExtendedHeaderData = ( DRM_BYTE * ) Oem_MemAlloc( DRM_UI64Low32( qwExtendedHeaderDataSize ) ) );

            cbReadExtend = 0;
            ChkBOOL( Oem_File_Read( f_pFileHandle , pbExtendedHeaderData, DRM_UI64Low32( qwExtendedHeaderDataSize ), &cbReadExtend ) && cbReadExtend == DRM_UI64Low32( qwExtendedHeaderDataSize ),
                     DRM_E_FILEREADERROR );

            cbExaminedInExtendedHeader = DRM_UI64( 0 );

            /*
            ** Cycle through the objects in the ASF extended Header and look for the Playready Object
            */
            while( DRM_UI64Les( cbExaminedInExtendedHeader, qwExtendedHeaderDataSize ) && !fPROFound )
            {

                /*
                ** If the DRM_GUID of the object matches the Playready DRM_GUID
                ** return the filehandle pointing to the PRO, set the offset, and set fPROFound to true
                */
                ChkBOOL( DRM_UI64High32( cbExaminedInExtendedHeader ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
                if( MEMCMP( ( DRM_BYTE * ) ( pbExtendedHeaderData + DRM_UI64Low32( cbExaminedInExtendedHeader ) ), ( DRM_BYTE * ) &g_guidPlayreadyHeaderObject, SIZEOF( DRM_GUID ) ) == 0 )
                {
                    /*  If the PlayReady Object exists, set the fHasPRO to true,
                    **  and reset the file pointer within the handler to point to the beginning of the PRO data.
                    **
                    **  PRO Object Construction
                    **
                    **  Field Name         | Field Type
                    **  ----------------------------
                    **  Object ID            DRM_GUID
                    **  Object Size          DRM_UINT64
                    **  System ID            DRM_GUID
                    **  System Version       DRM_DWORD
                    **  Data Size            DRM_DWORD
                    **  Data                 DRM_BYTE
                    **
                    */

                    fPROFound = TRUE;

                    /*
                    ** The offset should be set to the PRO data offset, located in the Data field
                    ** header Object ID  + Object Size
                    ** cbExamined (data part)
                    ** extended header
                    ** cbExaminedInExtendedHeader (data part)
                    ** Playready (Object ID + Object Size + System ID + System Version + Data Size)
                    */
                    qwOffsetInExtendedHeader = DRM_UI64Add( DRM_UI64Add( cbExamined,
                                                                         DRM_UI64( SIZEOF( DRM_GUID ) +
                                                                                   SIZEOF( DRM_UINT64 ) +
                                                                                   ( ( 2 * SIZEOF( DRM_GUID ) ) +
                                                                                   SIZEOF( DRM_UINT64 ) +
                                                                                   ( 2 * SIZEOF( DRM_DWORD ) ) ) ) ),
                                                            DRM_UI64Add( cbExtendedHeaderObj, cbExaminedInExtendedHeader ) );



                    /*
                    ** Find the PRObject size, located in the Data Size field
                    */
                    ChkBOOL( DRM_UI64High32( cbExaminedInExtendedHeader ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
                    *f_pcbPlayreadySize = *( DRM_DWORD * )( pbExtendedHeaderData
                                                          + DRM_UI64Low32( cbExaminedInExtendedHeader )
                                                          + ( ( 2 * SIZEOF( DRM_GUID ) ) + SIZEOF( DRM_UINT64 ) + SIZEOF( DRM_DWORD ) ) );

                    /*
                    ** Reset the File pointer to point to the beginning of the data field
                    */
                    ChkBOOL( DRM_UI64High32( qwOffsetInExtendedHeader ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
                    ChkBOOL( Oem_File_SetFilePointer( f_pFileHandle, DRM_UI64Low32( qwOffsetInExtendedHeader ), OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );


                } /* If the DRM_GUID doesn't match, skip ahead to the next object. */
                else
                {
                    /*
                    ** This gets the ObjectSize Field for the current object and increments cbExaminedInExtendedHeader
                    */
                    cbExaminedInExtendedHeader = DRM_UI64Add( cbExaminedInExtendedHeader,
                                                              *( DRM_UINT64 * )( pbExtendedHeaderData
                                                                               + DRM_UI64Low32( cbExaminedInExtendedHeader )
                                                                               + SIZEOF( DRM_GUID ) ) );
                }
            }
            /*
            ** Update cbExamined in the outer loop
            */
            cbExamined = DRM_UI64Add( cbExamined, DRM_UI64Add( cbExtendedHeaderObj, cbExaminedInExtendedHeader ) );

            ChkBOOL( DRM_UI64High32( cbExamined ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
        }
        else
        {
            /*
            ** This gets the ObjectSize Field for the current object and increments cbExamined
            */
            cbExamined = DRM_UI64Add( cbExamined,
                                      *( DRM_UINT64 * )( pbHeaderData
                                                       + DRM_UI64Low32( cbExamined )
                                                       + SIZEOF( DRM_GUID ) ) );

            ChkBOOL( DRM_UI64High32( cbExamined ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
        }

    }

ErrorExit:

    SAFE_OEM_FREE( pbHeaderData );
    SAFE_OEM_FREE( pbExtendedHeaderData );

    if( DRM_SUCCEEDED( dr ) && !fPROFound )
    {
        dr = DRM_S_FALSE;
    }
    return dr;

}

/*********************************************************************
**
** Function: Test_MTR_VerifyChallengeData
**
** Synopsis: Verifies the location & data of the metering challenge
**
**
** Arguments:
** [f_pbChallenge]           -- Pointer to UTF-8 metering challenge data
** [f_cbChallenge]           -- Size of UTF-8 metering challenge data in bytes
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TST_MTR_VerifyChallengeData(
    __in_bcount(f_cbChallenge) const DRM_BYTE  *f_pbChallenge,
    __in                       const DRM_DWORD  f_cbChallenge )
{
    DRM_RESULT    dr             = DRM_SUCCESS;
    DRM_SUBSTRING dsstrChallenge = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrResult    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrResult2   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrResult3   = EMPTY_DRM_SUBSTRING;

    ChkArg( f_pbChallenge != NULL );

    dsstrChallenge.m_cch = f_cbChallenge;

    #if DRM_USE_SOAP
        ChkDR( DRM_SOAPXML_SkipSOAPHeaders( (DRM_CHAR*)f_pbChallenge, &dsstrChallenge ) );
    #endif

    /*
    ** Get:
    ** <ProcessMeteringData ...>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                             &dsstrChallenge,
                             &g_dastrMeterDataRootTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrResult ) );

    /*
    ** Get:
    ** <ProcessMeteringData>
    **      <challenge>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                             &dsstrResult,
                             &g_dastrMeterDataChallenge1Tag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrResult ) );

    /*
    ** Get:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                             &dsstrResult,
                             &g_dastrMeterDataChallenge2Tag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrResult ) );

    /*
    ** Get:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                             &dsstrResult,
                             &g_dastrMeterDataMeteringDataTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrResult ) );

    /*
    ** Get:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  <Version>...</Version>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                             &dsstrResult,
                             &g_dastrMeterDataVersionTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrResult2 ) );

    /*
    ** Get:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  ...
    **                  <MID>...</MID>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                             &dsstrResult,
                             &g_dastrMeterDataMIDTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrResult2 ) );

    /*
    ** Get:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  ...
    **                  <TID>...</TID>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                             &dsstrResult,
                             &g_dastrMeterDataTIDTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrResult2 ) );

    /*
    ** Get:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  ...
    **                  <ClientInfo>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                              &dsstrResult,
                              &g_dastrReqTagClientInfo,
                               NULL,
                               NULL,
                               0,
                               NULL,
                              &dsstrResult2 ) );

    /*
    ** Get:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  ...
    **                  <ClientInfo>
    **                      <ClientVersion>...</ClientVersion>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                              &dsstrResult2,
                              &g_dastrReqTagClientVersion,
                               NULL,
                               NULL,
                               0,
                               NULL,
                              &dsstrResult3 ) );

    /*
    ** Get:
    ** <ProcessMeteringData>
    **      <challenge>
    **          <Challenge>
    **              <MeteringData>
    **                  ...
    **                  <SupportsMeterCertSignature>...</SupportsMeterCertSignature>
    */
    ChkDR( DRM_XML_GetNodeA(  (const DRM_CHAR*) f_pbChallenge,
                             &dsstrResult,
                             &g_dastrSupportsMeterCertSignatureTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrResult2 ) );

ErrorExit:

    return dr;
}


/*****************************************************************************
**
** Function:    TST_HLP_GetKidFromFile
**
** Synopsis:    Gets the KID out of the PlayReadyHeader in a Content File
**
** Arguments:
**
** [f_pszFileName]         -- file from which to get the KID
** [f_pdastrKid]           -- DRM_ANSI_STRING to receive the KID. cchString must be CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_RESULT DRM_CALL TST_HLP_GetKidFromFile(
                                    __in_z  DRM_CHAR        *f_pszFileName,
                                    __inout DRM_ANSI_STRING *f_pdastrKid )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    OEM_FILEHDL      fp            = OEM_INVALID_HANDLE_VALUE;
    DRM_BYTE        *pbPRO         = NULL;
    DRM_DWORD        cbPRO         = 0;
    DRM_DWORD        cbRead        = 0;
    DRM_BYTE        *pbXML         = NULL;
    DRM_DWORD        cbXML         = 0;

    DRM_WCHAR        rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ] = { 0 };

    DRM_CONST_STRING dstrKid       = EMPTY_DRM_STRING;
    DRM_STRING       dstrFilename  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrHeaderXML = EMPTY_DRM_STRING;

    DRM_WCHAR        rgwchFilename[ DRM_MAX_PATH + 1 ] = {0};
    DRM_SUBSTRING    dsstrFilename                     = EMPTY_DRM_SUBSTRING;

    ChkArg( f_pszFileName          != NULL );
    ChkArg( f_pdastrKid            != NULL );
    ChkArg( f_pdastrKid->pszString != NULL );
    ChkArg( f_pdastrKid->cchString == CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 );

    dsstrFilename.m_ich = 0;
    dsstrFilename.m_cch = DRMCRT_strlen(f_pszFileName);
    dstrFilename.pwszString = rgwchFilename;
    dstrFilename.cchString = DRM_MAX_PATH + 1;
    DRM_UTL_PromoteASCIItoUNICODE( f_pszFileName, &dsstrFilename, &dstrFilename );

    dstrKid.pwszString = rgwchKid;
    dstrKid.cchString  = NO_OF( rgwchKid );

    fp = Oem_File_Open( NULL,
                        dstrFilename.pwszString,
                        OEM_GENERIC_READ,
                        OEM_FILE_SHARE_READ,
                        OEM_OPEN_EXISTING,
                        OEM_ATTRIBUTE_NORMAL );

    ChkBOOL( OEM_INVALID_HANDLE_VALUE != fp, DRM_E_FILEOPEN );

    //Find the PlayReady Object if it exists in the file.
    if( _IsEnvelope( fp ) )
    {
        ChkDR( _FindPROInEnvelope( fp, &cbPRO ) );
    }
    else
    {
        ChkDR( _FindPROInASFFile( fp , &cbPRO ) );
    }

    // If there was no PlayReady Object to be found, fail.
    ChkBOOL( DRM_S_FALSE != dr, DRM_E_TEST_INVALID_FILE );

    ChkMem( pbPRO = ( DRM_BYTE * ) Oem_MemAlloc( cbPRO ) );

    // Read the Playready Object from the file.
    ChkBOOL( Oem_File_Read( fp , pbPRO, cbPRO, &cbRead ) && cbRead == cbPRO,
             DRM_E_FILEREADERROR );

    ChkDR( DRM_PRO_GetRecord( pbPRO, cbPRO, PLAYREADY_WRMHEADER, &pbXML, &cbXML ) );

    dstrHeaderXML.pwszString = ( DRM_WCHAR * ) pbXML;
    dstrHeaderXML.cchString = cbXML / SIZEOF( DRM_WCHAR );

    ChkDR( DRM_HDR_GetAttribute( &dstrHeaderXML, NULL, DRM_HEADER_ATTRIB_KID, &dstrKid, 0 ) );

    DRM_UTL_DemoteUNICODEtoASCII( dstrKid.pwszString, f_pdastrKid->pszString, f_pdastrKid->cchString );

    f_pdastrKid->pszString[CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) )] = '\0';

    f_pdastrKid->cchString = DRMCRT_strlen( f_pdastrKid->pszString );

ErrorExit:
    if( fp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp );
    }
    SAFE_OEM_FREE( pbPRO );
    return dr;
}


/***********************************************************
**
**  Function: TST_MTR_DecryptV3ChallengeData
**
**  Synopsis: Decrypts the Metering Challenge using the supplied Private Key
**
**  Arguments:
**
**      [f_pbChallenge]      -- Byte Pointer holding the challenge
**      [f_cbChallenge]      -- Size of the Challenge.
**      [f_pPrivKey]         -- Private key with which to decrypt the Challenge
**      [f_ppbDecryptedData] -- Pointer to a Pointer to receive the data.
**                              The caller should free this using Oem_MemFree.
**                              This **MUST** point to NULL on input.
**      [f_pcbDecryptedData] -- Size of the Challenge on output.
**      [f_pbKey]            -- Pointer to receive AES Key if not NULL
**
***********************************************************/
DRM_RESULT DRM_CALL TST_MTR_DecryptV3ChallengeData(
    __in_bcount(f_cbChallenge)                  const DRM_BYTE     *f_pbChallenge,
    __in                                        const DRM_DWORD     f_cbChallenge,
    __in                                              PRIVKEY_P256 *f_pPrivKey,
    __deref_out_bcount_opt(*f_pcbDecryptedData)       DRM_BYTE    **f_ppbDecryptedData,
    __inout                                           DRM_DWORD    *f_pcbDecryptedData,
    __out_opt                                         DRM_BYTE     *f_pbKey )
{
    DRM_RESULT    dr             = DRM_SUCCESS;
    DRM_CHAR     *pchTempString  = NULL;
    DRM_DWORD     cchTempString  = 0;
    DRM_SUBSTRING dsstrXML       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrMeterData = EMPTY_DRM_SUBSTRING;

    ChkArg( f_ppbDecryptedData != NULL );
    ChkArg( f_pcbDecryptedData != NULL );
    ChkArg( f_pbChallenge      != NULL );
    ChkArg( f_pPrivKey         != NULL );

    ChkArg( *f_ppbDecryptedData == NULL );
    ChkArg( *f_pcbDecryptedData == 0 );


    /*
    ** Copy the Challenge String locally to preserve the original after decryption.
    */
    cchTempString = f_cbChallenge;
    ChkOverflow( cchTempString + 1, cchTempString );
    ChkMem( pchTempString = ( DRM_CHAR * ) Oem_MemAlloc( cchTempString + 1 ) );
    MEMCPY( pchTempString, f_pbChallenge, cchTempString );
    PUT_CHAR( pchTempString, cchTempString, '\0' );

    dsstrXML.m_ich = 0;
    dsstrXML.m_cch = cchTempString;

    Log( "", "Decoding Challenge: %s", pchTempString );
#if DRM_USE_SOAP
    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( pchTempString, &dsstrXML ) );
#endif

    ChkDR( DRM_XML_GetSubNodeByPathA(  pchTempString,
                                      &dsstrXML,
                                      &g_dastrMeterDataEncryptedDataPath_Local,
                                       NULL,
                                       NULL,
                                      &dsstrMeterData,
                                       NULL,
                                       g_chForwardSlash ) );

    *f_pcbDecryptedData = dsstrMeterData.m_cch;

    ChkOverflow( *f_pcbDecryptedData + 1, *f_pcbDecryptedData );
    ChkMem( *f_ppbDecryptedData = ( DRM_BYTE * ) Oem_MemAlloc( *f_pcbDecryptedData + 1 ) );


    ChkDR( _DO_NOT_USE_ExtractEncryptedData( f_pPrivKey,
                                             pchTempString,
                                            &dsstrMeterData,
                                            *f_ppbDecryptedData,
                                             f_pcbDecryptedData,
                                             f_pbKey ) );

    PUT_CHAR( *f_ppbDecryptedData, *f_pcbDecryptedData, '\0' );


ErrorExit:

    SAFE_OEM_FREE( pchTempString );

    return dr;
}

/***********************************************************
**
**  Function: TST_MTR_ValidateV3Metering
**
**  Synopsis: Decrypts the Metering Challenge using the supplied Private Key and
**            validates the expected data.
**
**  Arguments:
**
**      [f_pPrivKey]    -- Private key with which to decrypt the Challenge
**      [f_pbChallenge] -- Byte Pointer holding the challenge
**      [f_cbChallenge] -- Size of the Challenge.
**      [argc]          -- number of items stored in argv
**      [argv]          -- an array of character strings. There must be at least three entries.
**                         The Entries are in triplets:
**                          argv[0] -- KID or filename from which to parse the KID.
**                          argv[1] -- Action
**                          argv[2] -- number of counts
**                          ...
**
**  Notes:
**          The Filename CANNOT be the length of a KID in Base64 Form.
**
***********************************************************/
DRM_RESULT DRM_CALL TST_MTR_ValidateV3Metering(
    __in                       const DRM_WCHAR *f_pwszPrivKey,
    __in                       const DRM_DWORD  f_cchPrivKey,
    __in_bcount(f_cbChallenge)       DRM_BYTE  *f_pbChallenge,
    __in                       const DRM_DWORD  f_cbChallenge,
    __in                             long       argc,
    __in_ecount(argc)                char     **argv )
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    long             iArg           = 0;
    DRM_SUBSTRING    dsstrData      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dsstrRecords   = EMPTY_DRM_SUBSTRING;
    DRM_CHAR        *pchDataSection = NULL;
    DRM_DWORD        cbDataSection  = 0;
    DRM_CONST_STRING dstrPrivKey    = EMPTY_DRM_STRING;
    DRM_DWORD        cbPrivKey = SIZEOF( PRIVKEY_P256 );
    PRIVKEY_P256     oPrivKey = { 0 };

    ChkArg( f_pbChallenge != NULL );
    ChkArg( f_pwszPrivKey != NULL );
    ChkArg( argc     >  2 );
    ChkArg( argc % 3 == 0 );
    ChkArg( argv     != NULL );
    ChkArg( argv[0]  != NULL );
    ChkArg( argv[1]  != NULL );
    ChkArg( argv[2]  != NULL );

    dstrPrivKey.pwszString = f_pwszPrivKey;
    dstrPrivKey.cchString  = f_cchPrivKey;

    ChkDR(DRM_B64_DecodeW( &dstrPrivKey,
                           &cbPrivKey,
                            oPrivKey.m_rgbPrivkey,
                            0));

    ChkDR( TST_MTR_DecryptV3ChallengeData( f_pbChallenge,
                                           f_cbChallenge,
                                          &oPrivKey,
                                           (DRM_BYTE**) &pchDataSection,
                                          &cbDataSection,
                                           NULL ) );
    dsstrData.m_ich = 0;
    dsstrData.m_cch = cbDataSection;

    Log("", "MeteringData Section: %s", pchDataSection );

    ChkDR( DRM_XML_GetSubNodeA(  pchDataSection,
                                &dsstrData,
                                &g_dastrMeterDataRecordsTag,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                &dsstrRecords,
                                 1 ) );

    for ( iArg = 0; iArg + 3 <= argc; iArg += 3)
    {
        DRM_ANSI_STRING dastrKIDToFind             = EMPTY_DRM_STRING;
        DRM_ANSI_STRING dastrActionToFind          = EMPTY_DRM_STRING;
        DRM_SUBSTRING   dasstrExpectedActionValue  = EMPTY_DRM_SUBSTRING;
        DRM_SUBSTRING   dsstrKIDNode               = EMPTY_DRM_SUBSTRING;
        DRM_SUBSTRING   dsstrActionNode            = EMPTY_DRM_SUBSTRING;
        DRM_SUBSTRING   dsstrActionValue           = EMPTY_DRM_SUBSTRING;
        DRM_CHAR        rgchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ] = { 0 };

        ChkArg( argv[iArg]      != NULL );
        ChkArg( argv[iArg + 1 ] != NULL );
        ChkArg( argv[iArg + 2 ] != NULL );

        dastrKIDToFind.pszString = argv[iArg];
        dastrKIDToFind.cchString = DRMCRT_strlen( argv[iArg] );

        /*
        ** If the string isn't the size of a B64 encoded KID assume it's a file.
        */
        if( dastrKIDToFind.cchString != CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) )
        {
            dastrKIDToFind.pszString = rgchKid;
            dastrKIDToFind.cchString = NO_OF( rgchKid );
            ChkDR( TST_HLP_GetKidFromFile( argv[iArg], &dastrKIDToFind ) );
        }

        Log( "", "Looking For KID: %s Action: %s Count: %s ", dastrKIDToFind.pszString, argv[iArg+1], argv[iArg+2] );

        dastrActionToFind.pszString = argv[iArg + 1];
        dastrActionToFind.cchString = DRMCRT_strlen( argv[iArg + 1] );

        dasstrExpectedActionValue.m_ich = 0;
        dasstrExpectedActionValue.m_cch = DRMCRT_strlen( argv[iArg + 2] );

        /*
        ** Find the KID in the Records.
        */

        ChkDR( DRM_XML_GetSubNodeA( pchDataSection,
                                       &dsstrRecords,
                                       &g_dastrMeterDataRecordTag,
                                       &g_dastrMeterDataRecordAttribName,
                                       (DRM_ANSI_CONST_STRING *)&dastrKIDToFind,
                                        0,
                                       &dsstrKIDNode,
                                        NULL,
                                        0 ) );

         ChkDR( DRM_XML_GetSubNodeA( pchDataSection,
                                       &dsstrKIDNode,
                                       &g_dastrMeterDataActionTag,
                                       &g_dastrMeterDataActionAttrib1Name,
                                       (DRM_ANSI_CONST_STRING *)&dastrActionToFind,
                                        0,
                                       &dsstrActionNode,
                                        NULL,
                                        1 ) );


         ChkDR( DRM_XML_GetNodeAttributeA( pchDataSection,
                                          &dsstrActionNode,
                                          &g_dastrMeterDataActionAttrib2Name,
                                          &dsstrActionValue ) );

         ChkBOOL( DRM_UTL_DASSSTRStringsEqual( pchDataSection,
                                             &dsstrActionValue,
                                             argv[iArg+2],
                                             &dasstrExpectedActionValue ), DRM_E_TEST_METERING_DATA_INCORRECT );

         Log( "","\tFound!!" );
    }

ErrorExit:
    SAFE_OEM_FREE( pchDataSection );

    return dr;
}

const DRM_ANSI_CONST_STRING g_dastrMeterResponsePath_Local = CREATE_DRM_ANSI_STRING( "ProcessMeteringDataResult/Response/MeteringResponse" );
const DRM_ANSI_CONST_STRING g_dastrMeterResponseCipherValuePath_Local = CREATE_DRM_ANSI_STRING( "ProcessMeteringDataResult/Response/MeteringResponse/EncryptedData/CipherData/CipherValue" );
const DRM_ANSI_CONST_STRING g_dastrMeterResponseSignedInfoPath_Local = CREATE_DRM_ANSI_STRING( "ProcessMeteringDataResult/Response/Signature/SignedInfo" );
const DRM_ANSI_CONST_STRING g_dastrMeterResponseDigestValuePath_Local = CREATE_DRM_ANSI_STRING( "ProcessMeteringDataResult/Response/Signature/SignedInfo/Reference/DigestValue" );
const DRM_ANSI_CONST_STRING g_dastrMeterResponseSignatureValuePath_Local = CREATE_DRM_ANSI_STRING( "ProcessMeteringDataResult/Response/Signature/SignatureValue" );
const DRM_ANSI_CONST_STRING g_dastrMeterChallengeTidPath_Local = CREATE_DRM_ANSI_STRING( "challenge/Challenge/MeteringData/TID" );
const DRM_ANSI_CONST_STRING g_dastrTagTid_Local = CREATE_DRM_ANSI_STRING( "TID" );
const DRM_ANSI_CONST_STRING g_dastrTagMeterCert_Local = CREATE_DRM_ANSI_STRING( "MeterCert" );

/*
 *  BUGBUG 14646: Do not use a template file to create the response.  
 *         It contains the metering cert which may be different from the one that the metering challenge was created with.  
 *         If the certs do not match then the generated response would be rejected.
 *         Generate everything from scratch include certs
 */

/***********************************************************
**
**  Function:    TST_MTR_GenerateV3Response
**
**  Synopsis:    Decrypts the Metering Challenge using the supplied Private Key and
**               patch an response template for testing.
**  Limitations: The template need to match the metering cert used in the challenge
**
**  Arguments:
**
**      [f_pwszPrivKey]    -- Private key with which to decrypt the Challenge
**      [f_cchPrivKey]     -- Size of the Private key
**      [f_pbChallenge]    -- The Metering Challenge.
**      [f_cbChallenge]    -- Size of the Challenge
**      [f_ppbResponse]    -- Pointer to the pointer of the Response.
**      [f_pcbResponse]    -- Pointer to Size of the Response.
**
************************************************************/
DRM_RESULT DRM_CALL TST_MTR_GenerateV3Response(
    __in                       const DRM_WCHAR *f_pwszPrivKey,
    __in                       const DRM_DWORD  f_cchPrivKey,
    __in_bcount(f_cbChallenge)       DRM_BYTE  *f_pbChallenge,
    __in                       const DRM_DWORD  f_cbChallenge,
    __inout_bcount(f_pcbResponse)    DRM_BYTE **f_ppbResponse,
    __inout                          DRM_DWORD *f_pcbResponse )
{

    DRM_RESULT            dr                          = DRM_SUCCESS;
    DRM_BYTE             *pbResponse                  = NULL;
    DRM_CHAR             *pbBuffer                    = ( DRM_CHAR * ) *f_ppbResponse;
    DRM_DWORD             cbBuffer                    = *f_pcbResponse;
    _XMBContextA         *pbRecords                   = NULL;
    DRM_DWORD             cbRecords                   = 0;
    DRM_BYTE             *pbRecordsCoded              = NULL;
    SIGNATURE_P256        oECC256Signature            = { 0 };
    DRM_DWORD             cchScratch                  = 0;
    DRM_DWORD             cchData                     = 0;
    DRM_DWORD             cbDataLen                   = 0;
    DRM_AES_KEY           g_aesKey                    = { 0 };
    DRM_DWORD             iNode                       = 0;
    DRM_CHAR             *pchXML = NULL;
    DRM_SUBSTRING         dsstrChanlenge              = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrXML                    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrResponse               = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrResponseTID            = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrResponseMeterCert      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrResponseCipherValue    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrResponseDigestValue    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrResponseSignatureValue = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrSignedInfo             = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrKIDNode                = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrKID                    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrData                   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrRecords                = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING dastrData                   = EMPTY_DRM_STRING;
    DRM_CHAR             *pchDataSection              = NULL;
    DRM_DWORD             cbDataSection               = 0;
    DRM_CONST_STRING      dstrPrivKey                 = EMPTY_DRM_STRING;
    DRM_DWORD             cbPrivKey                   = SIZEOF( PRIVKEY_P256 );
    PRIVKEY_P256          oPrivKey                    = { 0 };

    DRM_BYTE              rgbKey[__CB_DECL( DRM_AES_KEYSIZE_128 )]                              = { 0 };
    DRM_BYTE              rgbIV[__CB_DECL( DRM_AES_BLOCKLEN )]                                  = { 0 };
    DRM_BYTE              rgbHash[ __CB_DECL( SHA256_DIGEST_SIZE_IN_BYTES ) ]                   = { 0 };
    DRM_CHAR              rgchScratch[ CCH_BASE64_EQUIV( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ) ] = { 0 };
    DRM_CRYPTO_CONTEXT   *pCryptoContext                                                        = NULL;
	
    ChkArg( f_pbChallenge != NULL );
    ChkArg( f_pwszPrivKey != NULL );

    dsstrXML.m_ich = 0;
    dsstrXML.m_cch = cbBuffer;

    Log( "", "Generating Response." );
#if DRM_USE_SOAP
    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( pbBuffer, &dsstrXML ) );
#endif

    ChkDR( DRM_XML_GetSubNodeByPathA(  pbBuffer,
                                      &dsstrXML,
                                      &g_dastrMeterResponsePath_Local,
                                       NULL,
                                       NULL,
                                      &dsstrResponse,
                                       NULL,
                                       g_chForwardSlash ) );

    ChkDR( DRM_XML_GetSubNodeA(  pbBuffer,
                                &dsstrResponse,
                                &g_dastrTagTid_Local,
                                 NULL,
                                 NULL,
                                 0,
                                &dsstrResponseTID,
                                 NULL,
                                 1 ) );

    ChkMem( pbResponse = ( DRM_BYTE * ) Oem_MemAlloc( MAX_PACKET_BUFFER ) );
    MEMCPY( pbResponse, pbBuffer, dsstrResponseTID.m_ich );
    *f_pcbResponse = dsstrResponseTID.m_ich;
    dastrData.pszString = ( DRM_CHAR * ) pbResponse + dsstrResponse.m_ich;

    dsstrChanlenge.m_ich = 0;
    dsstrChanlenge.m_cch = f_cbChallenge;

#if DRM_USE_SOAP
    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( (DRM_CHAR *)f_pbChallenge, &dsstrChanlenge ) );
#endif

    ChkDR( DRM_XML_GetSubNodeByPathA( (DRM_CHAR *) f_pbChallenge,
                                      &dsstrChanlenge,
                                      &g_dastrMeterChallengeTidPath_Local,
                                       NULL,
                                       NULL,
                                      &dsstrData,
                                       NULL,
                                       g_chForwardSlash ) );

    MEMCPY( ( pbResponse + *f_pcbResponse ), ( f_pbChallenge + dsstrData.m_ich ), dsstrData.m_cch );
    *f_pcbResponse += dsstrData.m_cch;

    ChkDR( DRM_XML_GetSubNodeA(  pbBuffer,
                                &dsstrResponse,
                                &g_dastrTagMeterCert_Local,
                                 NULL,
                                 NULL,
                                 0,
                                &dsstrResponseMeterCert,
                                 NULL,
                                 1 ) );

    ChkDR( DRM_XML_GetSubNodeByPathA(  pbBuffer,
                                      &dsstrXML,
                                      &g_dastrMeterResponseCipherValuePath_Local,
                                       NULL,
                                       NULL,
                                       NULL,
                                      &dsstrResponseCipherValue,
                                       g_chForwardSlash ) );
    
    cbDataLen = dsstrResponseCipherValue.m_ich - dsstrResponseMeterCert.m_ich;
    MEMCPY( ( pbResponse + *f_pcbResponse ), ( pbBuffer + dsstrResponseMeterCert.m_ich ), cbDataLen );
    *f_pcbResponse += cbDataLen;
    
    ChkDR( DRM_XML_GetSubNodeByPathA(  pbBuffer,
                                      &dsstrXML,
                                      &g_dastrMeterResponseSignedInfoPath_Local,
                                       NULL,
                                       NULL,
                                      &dsstrSignedInfo,
                                       NULL,
                                       g_chForwardSlash ) );

    ChkDR( DRM_XML_GetSubNodeByPathA(  pbBuffer,
                                      &dsstrXML,
                                      &g_dastrMeterResponseDigestValuePath_Local,
                                       NULL,
                                       NULL,
                                       NULL,
                                      &dsstrResponseDigestValue,
                                       g_chForwardSlash ) );

    ChkDR( DRM_XML_GetSubNodeByPathA(  pbBuffer,
                                      &dsstrXML,
                                      &g_dastrMeterResponseSignatureValuePath_Local,
                                       NULL,
                                       NULL,
                                       NULL,
                                      &dsstrResponseSignatureValue,
                                       g_chForwardSlash ) );

    /*
    ** Decrypt challenge data, create KID records, and encrypt data
    */
                                       
    dstrPrivKey.pwszString = f_pwszPrivKey;
    dstrPrivKey.cchString  = f_cchPrivKey;

    ChkDR(DRM_B64_DecodeW( &dstrPrivKey,
                           &cbPrivKey,
                            oPrivKey.m_rgbPrivkey,
                            0));

    ChkDR( TST_MTR_DecryptV3ChallengeData( f_pbChallenge,
                                           f_cbChallenge,
                                          &oPrivKey,
                                         ( DRM_BYTE** ) &pchDataSection,
                                          &cbDataSection,
                                           rgbKey ) );
    dsstrData.m_ich = 0;
    dsstrData.m_cch = cbDataSection;

    Log("", "MeteringData Section: %s", pchDataSection );

    ChkDR( DRM_XML_GetSubNodeA(  pchDataSection,
                                &dsstrData,
                                &g_dastrMeterDataRecordsTag,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                &dsstrRecords,
                                 1 ) );

    ChkMem( pbRecords = ( _XMBContextA * )Oem_MemAlloc( MAX_PACKET_BUFFER ) );
    cbRecords = MAX_PACKET_BUFFER;

    ChkDR( DRM_XMB_CreateDocumentA( cbRecords,
                                  ( DRM_BYTE * )pbRecords,
                                   &g_dastrMeterDataDataTag ) );

    ChkDR( DRM_XMB_AddAttributeA( pbRecords,
                                 &g_dastrMeterDataRootAttribName,
                                 &g_dastrMeterDataRootAttribValue ) );

    ChkDR( DRM_XMB_GetXMLBaseA( pbRecords, &pchXML ) );

    ChkDR( DRM_XMB_WriteTagA( pbRecords,
                             &g_dastrMeterDataRecordsTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    while( TRUE )
    {
        DRM_SUBSTRING dasstrTag          = EMPTY_DRM_SUBSTRING;
        DRM_SUBSTRING dasstr1stAttrLabel = EMPTY_DRM_SUBSTRING;
        DRM_ANSI_CONST_STRING dastrKID   = EMPTY_DRM_STRING;
    
        dr = DRM_XML_EnumNextNodeA(pchDataSection,
                                  &dsstrRecords,
                                   iNode++,
                                  &dasstrTag,
                                  &dsstrKIDNode,
                                   NULL,
                                  &dasstr1stAttrLabel,
                                  &dsstrKID);

        if (dr == DRM_E_XMLNOTFOUND)                                                   
        {
            dr = DRM_SUCCESS;
            break;
        }
        else
        {
            ChkDR(dr);
            *(pchDataSection + dsstrKID.m_ich + dsstrKID.m_cch) = '\0';
            dastrKID.pszString = ( DRM_CHAR * )(pchDataSection + dsstrKID.m_ich);
            dastrKID.cchString = dsstrKID.m_cch;
            ChkDR( DRM_XMB_WriteTagA( pbRecords,
                                     &g_dastrMeterDataRecordAttribName,
                                     &dastrKID,
                                      NULL,
                                      NULL,
                                      wttClosed ) );
            *(pchDataSection + dsstrKID.m_ich + dsstrKID.m_cch) = '\"';
        }
    }
    ChkDR( DRM_XMB_CloseCurrNodeA( pbRecords, NULL ) );
    ChkDR( DRM_XMB_CloseDocumentA( pbRecords, &dsstrData ) );
    
    cbDataLen = dsstrData.m_cch;
    ChkDR( Oem_Random_GetBytes( NULL, rgbIV, DRM_AES_BLOCKLEN ) );
    if ( cbDataLen % DRM_AES_BLOCKLEN != 0 )
    {
        cbDataLen += DRM_AES_BLOCKLEN - ( cbDataLen % DRM_AES_BLOCKLEN );
    }

    ChkDR( Oem_Aes_SetKey( rgbKey, &g_aesKey ) );
    ChkDR( Oem_Aes_CbcEncryptData( &g_aesKey, ( DRM_BYTE * ) &pbRecords->rgchXML[0], cbDataLen, rgbIV ) );
    
    cchData = CCH_BASE64_EQUIV( cbDataLen + DRM_AES_BLOCKLEN );
    ChkMem( pbRecordsCoded = ( DRM_BYTE * )Oem_MemAlloc( cchData ) );
    MEMCPY( pbRecordsCoded, rgbIV, DRM_AES_BLOCKLEN );
    MEMCPY( pbRecordsCoded + DRM_AES_BLOCKLEN, &pbRecords->rgchXML[0], cbDataLen );

    ChkDR( DRM_B64_EncodeA( pbRecordsCoded,
                            cbDataLen + DRM_AES_BLOCKLEN,
                          ( DRM_CHAR * )pbRecordsCoded,
                           &cchData,
                            0 ) );

    MEMCPY( ( pbResponse + *f_pcbResponse ), pbRecordsCoded, cchData );
    *f_pcbResponse += cchData;
    
    dsstrSignedInfo.m_ich = *f_pcbResponse + dsstrSignedInfo.m_ich - dsstrResponseCipherValue.m_ich - dsstrResponseCipherValue.m_cch;

    dastrData.cchString = *f_pcbResponse + dsstrResponse.m_cch - dsstrResponseCipherValue.m_ich - dsstrResponseCipherValue.m_cch;

    cchData = dsstrResponseDigestValue.m_ich - dsstrResponseCipherValue.m_ich - dsstrResponseCipherValue.m_cch;
    MEMCPY( ( pbResponse + *f_pcbResponse ), pbBuffer + ( dsstrResponseCipherValue.m_ich + dsstrResponseCipherValue.m_cch ), cchData );
    *f_pcbResponse += cchData;

    ChkDR( _CalcSHA256Hash( &dastrData, rgbHash ) );
    cchScratch = CCH_BASE64_EQUIV( SHA256_DIGEST_SIZE_IN_BYTES );
    ChkDR( DRM_B64_EncodeA( rgbHash,
                            SHA256_DIGEST_SIZE_IN_BYTES,
                            rgchScratch,
                           &cchScratch,
                            0 ) );

    MEMCPY( ( pbResponse + *f_pcbResponse ), rgchScratch, cchScratch );
    *f_pcbResponse += cchScratch;
    
    cchData = dsstrResponseSignatureValue.m_ich - dsstrResponseDigestValue.m_ich - dsstrResponseDigestValue.m_cch;
    MEMCPY( ( pbResponse + *f_pcbResponse ), pbBuffer + ( dsstrResponseDigestValue.m_ich + dsstrResponseDigestValue.m_cch ), cchData );
    *f_pcbResponse += cchData;

    /*
    ** OEM_ECDSA_Sign_P256 Sign the response
    */
    
    ChkDR(DRM_B64_DecodeW( &dstrPrivKey,
                           &cbPrivKey,
                            oPrivKey.m_rgbPrivkey,
                            0));
    
    ChkMem( pCryptoContext = ( DRM_CRYPTO_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    ChkDR( OEM_ECDSA_Sign_P256( ( DRM_BYTE * ) pbResponse + dsstrSignedInfo.m_ich,
                                 dsstrSignedInfo.m_cch,
                                &oPrivKey,
                                &oECC256Signature,
                               ( struct bigctx_t * ) pCryptoContext ) );

    cchScratch = CCH_BASE64_EQUIV( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES );
    ChkDR( DRM_B64_EncodeA( oECC256Signature.m_rgbSignature,
                            ECDSA_P256_SIGNATURE_SIZE_IN_BYTES,
                            rgchScratch,
                            &cchScratch,
                            0 ) );

    MEMCPY( ( pbResponse + *f_pcbResponse ), rgchScratch, cchScratch );
    *f_pcbResponse += cchScratch;

    cchData = cbBuffer - dsstrResponseSignatureValue.m_ich - dsstrResponseSignatureValue.m_cch;
    MEMCPY( ( pbResponse + *f_pcbResponse ), pbBuffer + ( dsstrResponseSignatureValue.m_ich + dsstrResponseSignatureValue.m_cch ), cchData );
    *f_pcbResponse += cchData;
    PUT_CHAR( pbResponse, *f_pcbResponse, '\0' );
    *f_ppbResponse = pbResponse;

ErrorExit:
    SAFE_OEM_FREE( pchDataSection );
    SAFE_OEM_FREE( pbRecords );
    SAFE_OEM_FREE( pbRecordsCoded );
    SAFE_OEM_FREE( pCryptoContext );
    SAFE_OEM_FREE( pbBuffer );

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

