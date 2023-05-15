/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>

#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmcmdlnpars.h>
#include <oempkcrypto.h>
#include <oemeccp256.h>
#include <oemrsaoaeppss.h>
#include <drmlicense.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>

#include <drmtoolsconstants.h>
#include <drmtoolsmacros.h>
#include <drmtoolsutils.h>

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************
** Function: PrintCopyrightInfo
**
** Synopsis: Prints copyright information
**
** Arguments:
**            [f_pszAppDesc]     -- pointer to copyright information
**
*****************************************************************************/
DRM_API DRM_VOID DRM_CALL PrintCopyrightInfo( __in_z const DRM_CHAR *f_pszAppDesc )
{
    DRMASSERT( f_pszAppDesc != NULL );

    printf( "Microsoft (R) %s\nCopyright (c) Microsoft Corporation. All rights reserved.\n\n", f_pszAppDesc );
}

/*****************************************************************************
** Function: PrintCopyrightAndVersionInfo
**
** Synopsis: Prints copyright and version information
**
** Arguments:
**            [f_pszAppDesc]     -- pointer to copyright information
**            [f_dwMajorVersion] -- major version number
**            [f_dwMinorVersion] -- minor version number
**
*****************************************************************************/
DRM_API DRM_VOID DRM_CALL PrintCopyrightAndVersionInfo(
    __in_z const DRM_CHAR  *f_pszAppDesc,
    __in         DRM_DWORD  f_dwMajorVersion,
    __in         DRM_DWORD  f_dwMinorVersion )
{
    DRMASSERT( f_pszAppDesc != NULL );

    printf( "Microsoft (R) %s version %d.%d\nCopyright (c) Microsoft Corporation. All rights reserved.\n\n", f_pszAppDesc, f_dwMajorVersion, f_dwMinorVersion);
}

/*****************************************************************************
** Function: DRMTOOLS_PrintLicenseStateData
**
** Synopsis: Prints a license state data struct
**
**
** Arguments:
**           [pszOutputPrefix]      -- prefix for each line printed
**           [pDRMLicenseStateData] -- License state data to be printed
** Returns:
**           DRM_SUCCESS           -- On success
**           DRM_E_INVALIDARG      -- if any argument is invalid
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_PrintLicenseStateData(
    __in_z const DRM_CHAR               *pszOutputPrefix,
    __in   const DRM_LICENSE_STATE_DATA *pDRMLicenseStateData )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD dwCountIndex  = 0;
    DRM_DWORD dwDateIndex   = 0;

    ChkArg( pszOutputPrefix     != NULL
        && pDRMLicenseStateData != NULL );

    /*
    ** Print the type of right given by aggregating all the licenses for the content
    */
    printf( "%sDRM_LICENSE_DATA.dwStreamId: %d\n", pszOutputPrefix, pDRMLicenseStateData->dwStreamId );
    switch( pDRMLicenseStateData->dwCategory )
    {
    case DRM_LICENSE_STATE_NORIGHT: /* No rights to perform this action */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_NORIGHT\n", pszOutputPrefix );
        break;

    case DRM_LICENSE_STATE_UNLIM: /* Unlimited rights to perform this action */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_UNLIM\n", pszOutputPrefix );
        break;

    case DRM_LICENSE_STATE_COUNT: /* Action may only be performed a certain number of times */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_COUNT\n", pszOutputPrefix );
        break;

    case DRM_LICENSE_STATE_FROM: /* Action cannot be performed until a specific date */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_FROM\n", pszOutputPrefix );
        break;

    case DRM_LICENSE_STATE_UNTIL: /* Action cannot be performed after a certain date */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_UNTIL\n", pszOutputPrefix );
        break;

    case DRM_LICENSE_STATE_FROM_UNTIL: /* Action can only be performed within a specific range of dates */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_FROM_UNTIL\n", pszOutputPrefix );
        break;

    case DRM_LICENSE_STATE_COUNT_FROM: /* Action can only be performed a certain number of times, starting from a specific date */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_COUNT_FROM\n", pszOutputPrefix );
        break;

    case DRM_LICENSE_STATE_COUNT_UNTIL: /* Action can be performed a certain number of times until a specific date */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_COUNT_UNTIL\n", pszOutputPrefix );
        break;

    case DRM_LICENSE_STATE_COUNT_FROM_UNTIL: /* Action can only be performed a certain number of times, and only is a specific range of dates */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_COUNT_FROM_UNTIL\n", pszOutputPrefix );
        break;

    case DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE: /* License restrictions don't occur until after the first use */
        printf( "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE\n", pszOutputPrefix );
        break;

    default:
        printf( "%sDRM_LICENSE_DATA.dwCategory: Unknown! - %d\n", pszOutputPrefix, pDRMLicenseStateData->dwCategory );
    }

    /*
    ** If count limited, print the number of times the action can be performed
    */
    if ( pDRMLicenseStateData->dwNumCounts != 0 )
    {
        printf( "%sDRM_LICENSE_DATA.dwCount:", pszOutputPrefix );
        for( dwCountIndex = 0; dwCountIndex < pDRMLicenseStateData->dwNumCounts; dwCountIndex++ )
        {
            printf( "  %d", pDRMLicenseStateData->dwCount[ dwCountIndex ] );
        }
        printf( "\n" );
    }

    /*
    ** If the action is date limited, print the date restriction(s)
    */
    if ( pDRMLicenseStateData->dwNumDates != 0 )
    {
        printf( "%sDRM_LICENSE_DATA.datetime:", pszOutputPrefix );
        for( dwDateIndex = 0; dwDateIndex < pDRMLicenseStateData->dwNumDates; dwDateIndex++ )
        {
            DRMSYSTEMTIME  st;
            Oem_Clock_FileTimeToSystemTime( &(pDRMLicenseStateData->datetime[ dwDateIndex ]), &st );
            printf( "  On %04d/%02d/%02d at %02d:%02d:%02d.%03d",
                st.wYear,
                st.wMonth,
                st.wDay,
                st.wHour,
                st.wMinute,
                st.wSecond,
                st.wMilliseconds);
        }
        printf( "\n" );
    }

    /*
    * If the aggregate license data cannot easily be represented, the "vague" value will be set.
    * This will happen when two or more licenses with different right types (like COUNT and
    * and FROM_UNTIL) make it impossible to represent the information simply.  For instance,
    * if license 1 allows 5 plays and license 2 allows unlimited playback during the current
    * month, then the user is guaranteed at least 5 plays, but possibly an unlimited number,
    * if done within the current month.
    */
    printf( "%sDRM_LICENSE_DATA.dwVague: %d \n", pszOutputPrefix, pDRMLicenseStateData->dwVague );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRMTOOLS_TestKeyPair
**
** Synopsis :   Tests a public-key crypto key pair against itself
**
** Arguments :  [f_pContextCrypto] - Specifies the current crypto context
**              [f_eAlgorithm]     - Specifies the public key algorithm
**              [f_pbPubKey]       - Byte array containing the public key
**              [f_cbPubKey]       - Size of f_pbPubKey in bytes
**              [f_pbPrivKey]      - Byte array containing the private key
**              [f_cbPrivKey]      - Size of f_pbPrivKey in bytes
**
** Returns :
**              DRM_SUCCESS - Successfully tested the key pair
**              DRM_E_INVALIDARG - An argument was invalid or NULL
**              DRM_E_KEY_MISMATCH - The public and private keys specified do not match
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_TestKeyPair(
    __in                             DRM_CRYPTO_CONTEXT                *f_pContextCrypto,
    __in                             DRM_PKCRYPTO_SUPPORTED_ALGORITHMS  f_eAlgorithm,
    __in_bcount( f_cbPubKey )  const DRM_BYTE                          *f_pbPubKey,
    __in                             DRM_DWORD                          f_cbPubKey,
    __in_bcount( f_cbPrivKey ) const DRM_BYTE                          *f_pbPrivKey,
    __in                             DRM_DWORD                          f_cbPrivKey )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_BYTE         rgbBuffer1[__CB_DECL( ECC_P256_PLAINTEXT_SIZE_IN_BYTES )]  = { 0 };
    DRM_BYTE         rgbBuffer2[__CB_DECL( ECC_P256_CIPHERTEXT_SIZE_IN_BYTES )] = { 0 };
    DRM_BYTE         rgbBuffer3[__CB_DECL( OAEP_CB_ENCRYPTED_MSG_MAX_2048BIT )] = { 0 };
    const DRM_BYTE  *pbClearBuffer     = NULL;
    DRM_BYTE        *pbEncryptedBuffer = NULL;
    DRM_DWORD        cbEncrypted       = 0;
    DRM_DWORD        cbClearInput      = 0;
    DRM_DWORD        cbClear           = 0;
    DRM_DWORD        cbSignature       = 0;

    /*
    ** Key pointers and size arguments, and crypto context arg are checked by DRM_PK functions
    ** and do not require additional ChkArgs here
    */

    MEMSET( rgbBuffer1, 'b', SIZEOF( rgbBuffer1 ) );

    switch( f_eAlgorithm )
    {
    case eDRM_ECC_P160:
        cbClearInput = DRM_ECC160_PLAINTEXT_LEN;
        pbClearBuffer = rgbBuffer1;
        pbEncryptedBuffer = rgbBuffer2;
        cbEncrypted = DRM_ECC160_CIPHERTEXT_LEN;
        cbClear     = DRM_ECC160_PLAINTEXT_LEN;
        cbSignature = DRM_ECC160_SIGNATURE_LEN;
        break;

    case eDRM_ECC_P256:
        cbClearInput = SIZEOF( rgbBuffer1 );
        pbClearBuffer = rgbBuffer1;
        pbEncryptedBuffer = rgbBuffer2;
        cbEncrypted = ECC_P256_CIPHERTEXT_SIZE_IN_BYTES;
        cbClear = ECC_P256_PLAINTEXT_SIZE_IN_BYTES;
        cbSignature = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;
        /*
        ** Generate some plaintext
        */
        ChkDR( OEM_ECC_GenerateHMACKey_P256( ( PLAINTEXT_P256 * )rgbBuffer1,
                                             ( struct bigctx_t * )&f_pContextCrypto->rgbCryptoContext ) );
        break;

    case eDRM_RSA:
        cbClearInput = SIZEOF( rgbBuffer1 );;
        pbClearBuffer = rgbBuffer1;
        pbEncryptedBuffer = rgbBuffer3;
        cbEncrypted = SIZEOF( rgbBuffer3 );
        cbClear     = DRM_RSA_CB_MAX_PLAINTEXT_MAX_2048BIT;
        cbSignature = DRM_RSA_CB_MODULUS_MAX_2048BIT;
        break;
    default:
        ChkArg( FALSE );
    }

    ChkDR( DRM_PK_Encrypt( NULL,
                           f_pContextCrypto,
                           f_eAlgorithm,
                           f_cbPubKey,
                           f_pbPubKey,
                           cbClearInput,
                           pbClearBuffer,
                           &cbEncrypted,
                           pbEncryptedBuffer ) );

    ChkDR( DRM_PK_Decrypt( f_pContextCrypto,
                           f_eAlgorithm,
                           f_cbPrivKey,
                           f_pbPrivKey,
                           cbEncrypted,
                           pbEncryptedBuffer,
                           &cbClear,
                           pbEncryptedBuffer ) );

    ChkBOOL( MEMCMP( pbClearBuffer, pbEncryptedBuffer, cbClearInput ) == 0, DRM_E_KEY_MISMATCH );

    ChkDR( DRM_PK_Sign( f_pContextCrypto,
                        f_eAlgorithm,
                        f_cbPrivKey,
                        f_pbPrivKey,
                        cbClearInput,
                        pbClearBuffer,
                        &cbSignature,
                        pbEncryptedBuffer ) );

    ChkDR( DRM_PK_Verify( f_pContextCrypto,
                          f_eAlgorithm,
                          f_cbPubKey,
                          f_pbPubKey,
                          cbClearInput,
                          pbClearBuffer,
                          cbSignature,
                          pbEncryptedBuffer ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRMTOOLS_CreateKeyPair
**
** Synopsis :   Creates a public-private key pair
**
** Arguments :  [f_pContextCrypto] - Specifies the current crypto context (optional)
**              [f_eAlgorithm]     - Specifies the public key algorithm
**              [f_eRSAKeyLength]  - Specifies the key length for RSA algorithm,
**                                   will be ignored by other algorithms
**              [f_pbPubKey]       - Byte array containing the public key
**              [f_cbPubKey]       - Size of f_pbPubKey in bytes
**              [f_pbPrivKey]      - Byte array containing the private key
**              [f_cbPrivKey]      - Size of f_pbPrivKey in bytes
**              [f_pdstrPubKey]    - B64 encoded version of the pub key (optional)
**              [f_pdstrPrivKey]   - B64 encoded version of the priv key (optional)
**
** Returns :
**              DRM_SUCCESS - Successfully generated the key pair
**              DRM_E_INVALIDARG - An argument was invalid or NULL
**              DRM_E_KEY_MISMATCH - The public and private keys generated did not match
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_CreateKeyPair(
    __in                        DRM_CRYPTO_CONTEXT                        *f_pContextCrypto,
    __in                        DRM_PKCRYPTO_SUPPORTED_ALGORITHMS          f_eAlgorithm,
    __in                        DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT      f_eRSAKeyLength,
    __out_bcount( f_cbPubKey )  DRM_BYTE                                  *f_pbPubKey,
    __in                        DRM_DWORD                                  f_cbPubKey,
    __out_bcount( f_cbPrivKey ) DRM_BYTE                                  *f_pbPrivKey,
    __in                        DRM_DWORD                                  f_cbPrivKey,
    __out_opt                   DRM_STRING                                *f_pdstrPubKey,
    __out_opt                   DRM_STRING                                *f_pdstrPrivKey )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cbPubKey  = f_cbPubKey;
    DRM_DWORD  cbPrivKey = f_cbPrivKey;

    /*
    ** All required arguments are checked by the following two function calls
    */
    ChkDR( DRM_PK_GenKeyPair( f_pContextCrypto,
                              f_eAlgorithm,
                              f_eRSAKeyLength,
                              &cbPubKey,
                              f_pbPubKey,
                              &cbPrivKey,
                              f_pbPrivKey ) );

    ChkDR( DRMTOOLS_TestKeyPair( f_pContextCrypto,
                                 f_eAlgorithm,
                                 f_pbPubKey,
                                 cbPubKey,
                                 f_pbPrivKey,
                                 cbPrivKey ) );

    if ( f_pdstrPubKey != NULL )
    {
        f_pdstrPubKey->cchString  = CCH_BASE64_EQUIV( cbPubKey );
        f_pdstrPubKey->pwszString = ( DRM_WCHAR * )Oem_MemAlloc( f_pdstrPubKey->cchString * SIZEOF( DRM_WCHAR ) );
        ChkMem( f_pdstrPubKey->pwszString );

        ChkDR( DRM_B64_EncodeW( f_pbPubKey,
                                cbPubKey,
                                f_pdstrPubKey->pwszString,
                                &f_pdstrPubKey->cchString,
                                0 ) );
    }

    if ( f_pdstrPrivKey != NULL )
    {
        f_pdstrPrivKey->cchString  = CCH_BASE64_EQUIV( cbPrivKey );
        f_pdstrPrivKey->pwszString = ( DRM_WCHAR * )Oem_MemAlloc( f_pdstrPrivKey->cchString * SIZEOF( DRM_WCHAR ) );

        ChkMem( f_pdstrPrivKey->pwszString );

        ChkDR( DRM_B64_EncodeW( f_pbPrivKey,
                                cbPrivKey,
                                f_pdstrPrivKey->pwszString,
                                &f_pdstrPrivKey->cchString,
                                0 ) );
    }
ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRMTOOLS_ReallocXmlContext
**
** Synopsis :   Allocate double the size of the current
**              wide character based XML context
**
** Arguments :  [f_ppbXmlContext] - pointer to wide character based Xml content
**
** Returns :    DRM_SUCCESS if new buffer was allocated successfully
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_ReallocXmlContext(
    __inout _XMBContext    **f_ppbXmlContext)
{
    DRM_RESULT   dr              = DRM_SUCCESS;
    DRM_DWORD    cbContextXML    = 0;
    _XMBContext *pbContextXMLNew = NULL;

    /* get current size */
    ChkDR( DRM_XMB_GetContextSize( *f_ppbXmlContext, &cbContextXML ) );
    /*  This should never be 0 */
    ChkBOOL ( cbContextXML != 0, DRM_E_LOGICERR );

    /* alloc double of the current size */
    cbContextXML *= 2;
    ChkMem( pbContextXMLNew = (_XMBContext *) Oem_MemAlloc( cbContextXML ) );

    ChkDR( DRM_XMB_ReallocDocument( *f_ppbXmlContext,
                                    cbContextXML,
                                    pbContextXMLNew ) );

    SAFE_OEM_FREE( *f_ppbXmlContext );

    *f_ppbXmlContext = pbContextXMLNew;

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        SAFE_OEM_FREE( pbContextXMLNew );
    }

    return dr;
}

/******************************************************************************
**
** Function :   DRMTOOLS_Base64EncodeOfByteArray
**
** Synopsis :   Wrapper method for DRM_B64_EncodeW method:
**              Calculates memory required for base64 conversion
**              allocates it and performs the conversion.
**              It's responsibility of the caller to release this memory
**
** Arguments :  [f_pbArray]  - input byte array
**              [f_cbArray]  - input byte array size
**              [f_pdstrB64] - pointer to DRM_STRING that will keep result of conversion
**
** Returns :    DRM_SUCCESS - Successfully base64 encode byte array
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_Base64EncodeOfByteArray(
    __in_bcount( f_cbArray ) const DRM_BYTE   *f_pbArray,
    __in                     const DRM_DWORD   f_cbArray,
    __inout                        DRM_STRING *f_pdstrB64 )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbArray != NULL );
    ChkArg( f_pdstrB64 != NULL );
    /* incoming drm string should be empty */
    ChkArg( f_pdstrB64->pwszString == NULL && f_pdstrB64->cchString == 0 );

    f_pdstrB64->cchString = CCH_BASE64_EQUIV( f_cbArray );
    ChkMem( f_pdstrB64->pwszString
            = ( DRM_WCHAR* )Oem_MemAlloc( f_pdstrB64->cchString * SIZEOF ( DRM_WCHAR ) ) );
    ChkDR( DRM_B64_EncodeW( f_pbArray,
                            f_cbArray,
                            f_pdstrB64->pwszString,
                            &f_pdstrB64->cchString,
                            0 ) );
ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRMTOOLS_OpenOutputFile
**
** Synopsis :   Open an output file, prints out error
**
** Arguments :
**              [f_pdstrFileName]   -- pointer to DRM string containing file name
**              [f_pFileHandle]     -- pointer to file handler of output file
**
** Returns :    DRM_SUCCESS  -- if file was sucessfully opened
**
** Note:        It is responsibulity of caller to properly close opened output
**              file (file handler).
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_OpenOutputFile(
    __in  const   DRM_CONST_STRING    *f_pdstrFileName,
    __out         OEM_FILEHDL         *f_pFileHandle )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR   rgchFile[ MAX_FILENAME + 1 ] = { 0 };

    *f_pFileHandle = Oem_File_Open( NULL,
                                    f_pdstrFileName->pwszString,
                                    OEM_GENERIC_READ|OEM_GENERIC_WRITE,
                                    OEM_FILE_SHARE_READ|OEM_FILE_SHARE_WRITE,
                                    OEM_CREATE_ALWAYS,
                                    OEM_ATTRIBUTE_NORMAL );
    if ( *f_pFileHandle == OEM_INVALID_HANDLE_VALUE )
    {
        ChkArg( f_pdstrFileName->cchString <= MAX_FILENAME );

        /* Safe to use, assume that input parameter is in ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( f_pdstrFileName->pwszString,
                                      rgchFile,
                                      f_pdstrFileName->cchString );

        rgchFile[ f_pdstrFileName->cchString ] = '\0';

        printf( "%s could not be opened\n",  rgchFile );
        ChkDR( DRM_E_FILEOPEN );
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRMTOOLS_ReadBinaryFileData
**
** Synopsis :   Reads the content of a file into a DRM_BYTE buffer
**
** Arguments :  f_pdstrFileName
**              f_pbData
**              f_cbData
**
** Returns :
**
** Notes : function allocates memory for f_pbData, freeing it is a caller's
**         responsibility.
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_ReadBinaryFileData(
    __in  const DRM_CONST_STRING    *f_pdstrFileName,
    __out       DRM_BYTE           **f_ppbData,
    __out       DRM_DWORD           *f_cbData )
{
    DRM_RESULT  dr                           = DRM_SUCCESS;
    OEM_FILEHDL hfile                        = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD   cbFile                       = 0;
    DRM_DWORD   cbRead                       = 0;
    DRM_WCHAR   rgwchTemp [MAX_FILENAME + 1] = { 0 };
    DRM_CHAR    rgchTemp [MAX_FILENAME + 1]  = { 0 };

    ChkDR( DRM_STR_StringCchCopyNW( rgwchTemp, NO_OF(rgwchTemp), f_pdstrFileName->pwszString, f_pdstrFileName->cchString ) );
    DRM_UTL_DemoteUNICODEtoASCII( rgwchTemp, rgchTemp, NO_OF(rgchTemp) );

    hfile = Oem_File_Open( NULL,
                           rgwchTemp,
                           OEM_GENERIC_READ,
                           OEM_FILE_SHARE_READ,
                           OEM_OPEN_EXISTING,
                           OEM_ATTRIBUTE_NORMAL );
    if( hfile == OEM_INVALID_HANDLE_VALUE )
    {
        printf( "%s could not be opened\n",  rgchTemp );
        ChkDR( DRM_E_FILEOPEN );
    }

    if( !Oem_File_GetSize( hfile, &cbFile ) )
    {
        printf( "Error getting filesize of %s.\n", rgchTemp );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    ChkBOOL( cbFile != 0, DRM_E_FILE_READ_ERROR );

    if( !Oem_File_SetFilePointer( hfile, 0, OEM_FILE_BEGIN, NULL ) )
    {
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /*
    **  Read data from hfile
    */
    ChkMem( *f_ppbData = (DRM_BYTE *) Oem_MemAlloc( cbFile ) );

    if( !Oem_File_Read( hfile, *f_ppbData, cbFile, &cbRead )
     ||  cbRead != cbFile )
    {
        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    *f_cbData = cbFile;

ErrorExit:
    /*
    **  Close file
    */
    if( hfile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfile );
    }
    return dr;
}



/******************************************************************************
**
** Function :   DRMTOOLS_ReadFileData
**
** Synopsis :   Reads the content of a file into a DRM_STRING
**
** Arguments :  f_pdstrFileName
**              f_pdstrFileData
**
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_ReadFileData(
    __in  const DRM_CONST_STRING    *f_pdstrFileName,
    __out       DRM_STRING          *f_pdstrFileData)
{
    DRM_RESULT  dr                           = DRM_SUCCESS;
    OEM_FILEHDL hfile                        = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD   cbFile                       = 0;
    DRM_DWORD   cbRead                       = 0;
    DRM_WCHAR   rgwchTemp [MAX_FILENAME + 1] = { 0 };
    DRM_CHAR    rgchTemp [MAX_FILENAME + 1]  = { 0 };

    ChkDR( DRM_STR_StringCchCopyNW(rgwchTemp, NO_OF(rgwchTemp), f_pdstrFileName->pwszString, f_pdstrFileName->cchString) );
    DRM_UTL_DemoteUNICODEtoASCII( rgwchTemp, rgchTemp, NO_OF(rgchTemp) );

    hfile = Oem_File_Open(NULL,
                         rgwchTemp,
                         OEM_GENERIC_READ,
                         OEM_FILE_SHARE_READ,
                         OEM_OPEN_EXISTING,
                         OEM_ATTRIBUTE_NORMAL);
    if( hfile == OEM_INVALID_HANDLE_VALUE )
    {
        printf( "%s could not be opened\n",  rgchTemp );
        ChkDR( DRM_E_FILEOPEN );
    }

    if( !Oem_File_GetSize( hfile, &cbFile ) )
    {
        printf( "Error getting filesize of %s.\n", rgchTemp );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    ChkBOOL( cbFile != 0, DRM_E_FILE_READ_ERROR );

    if( ( cbFile % SIZEOF(DRM_WCHAR) ) != 0 )
    {
        printf( "%s is not Wide Char format.\n", rgchTemp );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    if( !Oem_File_SetFilePointer( hfile, 0, OEM_FILE_BEGIN, NULL ) )
    {
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /*
    **  Read data from hfile
    */
    ChkMem( f_pdstrFileData->pwszString = (DRM_WCHAR *) Oem_MemAlloc(cbFile) );

    if( !Oem_File_Read(hfile, f_pdstrFileData->pwszString, cbFile, &cbRead)
     ||  cbRead != cbFile )
    {
        ChkDR( DRM_E_FILE_READ_ERROR );
    }
    f_pdstrFileData->cchString = cbFile / SIZEOF(DRM_WCHAR);

ErrorExit:
    /*
    **  Close file
    */
    if( hfile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close(hfile);
    }
    return dr;
}

/*********************************************************************
**
** Function: DRMTOOLS_WriteBufferToFile
**
** Synopsis: Writes a buffer to a file.
**
** Arguments:
**
** [f_pdstrOutputFile]                 -- The name of the file in which to write.
** [pbBuffer]                          -- Buffer containing the data to write.
** [cbBuffer]                          -- Size of the buffer.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_FILE_WRITE_ERROR       -- If there is a problem writing to the file
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_WriteBufferToFile(
    __in const DRM_CONST_STRING    *f_pdstrOutputFile,
    __in const DRM_BYTE            *pbBuffer,
    __in const DRM_DWORD            cbBuffer )
{
    DRM_RESULT  dr       = DRM_SUCCESS;
    OEM_FILEHDL hfileOut = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD   cbRead   = 0;
    DRM_CHAR    rgchFile[ MAX_FILENAME + 1 ] = { 0 };

    ChkArg( pbBuffer != NULL );
    ChkArg( f_pdstrOutputFile->pwszString != NULL );
    ChkArg( f_pdstrOutputFile->cchString <= MAX_FILENAME );

    ChkDR( DRMTOOLS_OpenOutputFile( f_pdstrOutputFile, &hfileOut ) );

    if ( !Oem_File_Write( hfileOut, (DRM_VOID *)pbBuffer, cbBuffer, &cbRead ) ||
         cbRead != cbBuffer )
    {
        /* Safe to use, assume that input parameter is in ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( f_pdstrOutputFile->pwszString,
                                      rgchFile,
                                      f_pdstrOutputFile->cchString );

        rgchFile[ f_pdstrOutputFile->cchString ] = '\0';

        printf( "Failed to write buffer to %s.\n", rgchFile );

        ChkDR( DRM_E_FILE_WRITE_ERROR );
    }

ErrorExit:
    if ( hfileOut != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfileOut );
    }
    return dr;
}

DRM_API DRM_VOID DRM_CALL DRMTOOLS_PrintErrorCode(
    __in DRM_RESULT dr )
{
    if( DRM_SUCCEEDED( dr ) )
    {
        printf("Succeeded, dr is ");
    }
    else
    {
        printf("FAILED, dr is ");
    }

    switch( dr )
    {
    case DRM_E_LICENSE_EXPIRED:
        printf( "DRM_E_LICENSE_EXPIRED : License expired 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_E_CLK_NOT_SET:
        printf( "DRM_E_CLK_NOT_SET : Secure clock is not set 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_E_NO_CLK_SUPPORTED:
        printf( "DRM_E_NO_CLK_SUPPORTED : This device does not support any Clock. So time bound licenses cannot be played 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_E_RIGHTS_NOT_AVAILABLE:
        printf( "DRM_E_RIGHTS_NOT_AVAILABLE : The rights the app has requested are not available in the license  0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_E_HEADER_NOT_SET:
        printf( "DRM_E_HEADER_NOT_SET : A v2 content header is not set 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_E_CONDITION_NOT_SUPPORTED:
        printf( "DRM_E_CONDITION_NOT_SUPPORTED : A condition in the license is not supported by this verison of DRM 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_E_INVALID_LICENSE:
        printf( "DRM_E_INVALID_LICENSE : The license is invalid 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_E_LICENSE_NOT_FOUND:
        printf( "DRM_E_LICENSE_NOT_FOUND : No license found 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_E_DOMAIN_BIND_LICENSE:
        printf( "DRM_E_DOMAIN_BIND_LICENSE : Cannot bind to a domain private key 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_E_NO_OPL_CALLBACK:
        printf( "DRM_E_NO_OPL_CALLBACK : There is no callback function to process the output restrictions in the license 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_SUCCESS:
        printf( "DRM_SUCCESS : The operation succeeded 0x%X\n", (DRM_DWORD)dr );
        break;
    case DRM_S_FALSE:
        printf( "DRM_S_FALSE : 0x%X\n", (DRM_DWORD)dr );
        break;
    default:
        printf( "0x%X\n", (DRM_DWORD)dr );
        break;
    }
}

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_PrintOPLOutputIDs( __in const DRM_OPL_OUTPUT_IDS *f_pOPLs )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i;
    DRM_WCHAR rgwszGUID[DRM_GUID_STRING_LEN+1] = {0};
    DRM_CHAR  rgszGUID[NO_OF(rgwszGUID)] = {0};

    printf("    (%d GUIDs)\r\n", f_pOPLs->cIds );
    for( i = 0; i < f_pOPLs->cIds; i++ )
    {
        ChkDR( DRM_UTL_GuidToString( &f_pOPLs->rgIds[i],
                            rgwszGUID ) );
        /* Safe to use, input parameter is in ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( rgwszGUID,
                                      rgszGUID,
                                      NO_OF(rgwszGUID) );

        printf("    GUID = %s\r\n", rgszGUID);
    }
    printf("\r\n");
ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_PrintVideoOutputProtectionIDs( __in const DRM_VIDEO_OUTPUT_PROTECTION_IDS_EX *f_pOPLs )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i;
    DRM_WCHAR rgwszGUID[DRM_GUID_STRING_LEN+1] = {0};
    DRM_CHAR  rgszGUID[NO_OF(rgwszGUID)] = {0};

    printf("    (%d entries)\r\n", f_pOPLs->cEntries );
    for( i = 0; i < f_pOPLs->cEntries; i++ )
    {
        ChkDR( DRM_UTL_GuidToString( &f_pOPLs->rgVop[i].guidId,
                            rgwszGUID ) );
        /* Safe to use, input parameter is in ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( rgwszGUID,
                                      rgszGUID,
                                      NO_OF(rgwszGUID) );

        printf("    GUID = %s, dwConfigData = %d\r\n", rgszGUID, f_pOPLs->rgVop[i].dwConfigData);
    }
    printf("\r\n");
ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_PrintInclusionListGUIDs( __in const DRM_INCLUSION_LIST_CALLBACK_STRUCT *f_pInclusionList )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i;
    DRM_WCHAR rgwszGUID[DRM_GUID_STRING_LEN+1] = {0};
    DRM_CHAR  rgszGUID[NO_OF(rgwszGUID)] = {0};

    for( i = 0; i < DRM_MAX_INCLUSION_GUIDS; i++ )
    {
        if( f_pInclusionList->rgfInclusionListValid[i][f_pInclusionList->dwChainDepth] )
        {
            ChkDR( DRM_UTL_GuidToString( &f_pInclusionList->rgInclusionList[i],
                                rgwszGUID ) );
            /* Safe to use, input parameter is in ASCII */
            DRM_UTL_DemoteUNICODEtoASCII( rgwszGUID,
                                          rgszGUID,
                                          NO_OF(rgwszGUID) );

            printf("    GUID = %s\r\n", rgszGUID);
        }
    }
    printf("\r\n");
ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_PrintPolicyCallback(
    __in const DRM_VOID                *f_pvPolicyCallbackData,
    __in       DRM_POLICY_CALLBACK_TYPE f_dwCallbackType )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const DRM_PLAY_OPL_EX *oplPlay = NULL;
    const DRM_COPY_OPL *oplCopy = NULL;

    switch( f_dwCallbackType )
    {
        case DRM_PLAY_OPL_CALLBACK:
            printf("  Got DRM_PLAY_OPL_CALLBACK from Bind:\r\n");
            ChkArg( f_pvPolicyCallbackData != NULL );
            oplPlay = (const DRM_PLAY_OPL_EX*)f_pvPolicyCallbackData;

            printf("    minOPL:\r\n");
            printf("    wCompressedDigitalVideo   = %d\r\n", oplPlay->minOPL.wCompressedDigitalVideo);
            printf("    wUncompressedDigitalVideo = %d\r\n", oplPlay->minOPL.wUncompressedDigitalVideo);
            printf("    wAnalogVideo              = %d\r\n", oplPlay->minOPL.wAnalogVideo);
            printf("    wCompressedDigitalAudio   = %d\r\n", oplPlay->minOPL.wCompressedDigitalAudio);
            printf("    wUncompressedDigitalAudio = %d\r\n", oplPlay->minOPL.wUncompressedDigitalAudio);
            printf("\r\n");

            printf("    oplIdReserved:\r\n");
            ChkDR( DRMTOOLS_PrintOPLOutputIDs( &oplPlay->oplIdReserved ) );

            printf("    vopi:\r\n");
            ChkDR( DRMTOOLS_PrintVideoOutputProtectionIDs( &oplPlay->vopi ) );

            break;

        case DRM_COPY_OPL_CALLBACK:
            printf("  Got DRM_COPY_OPL_CALLBACK from Bind:\r\n");

            ChkArg( f_pvPolicyCallbackData != NULL );
            oplCopy = (const DRM_COPY_OPL*)f_pvPolicyCallbackData;

            printf("    wMinimumCopyLevel   = %d\r\n", oplCopy->wMinimumCopyLevel);
            printf("\r\n");

            printf("    oplIdIncludes:\r\n");
            ChkDR( DRMTOOLS_PrintOPLOutputIDs( &oplCopy->oplIdIncludes ) );

            printf("    oplIdIncludes:\r\n");
            ChkDR( DRMTOOLS_PrintOPLOutputIDs( &oplCopy->oplIdExcludes ) );

            break;

        case DRM_INCLUSION_LIST_CALLBACK:
            printf("  Got DRM_INCLUSION_LIST_CALLBACK from Bind:\r\n");
            ChkArg( f_pvPolicyCallbackData != NULL );

            printf("    Inclusion GUIDs:\r\n");
            ChkDR( DRMTOOLS_PrintInclusionListGUIDs( (DRM_INCLUSION_LIST_CALLBACK_STRUCT*)f_pvPolicyCallbackData ) );

            break;
        case DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK:
            {
                const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT *pExtCallback = (const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT*)f_pvPolicyCallbackData;
                DRM_DWORD i = 0;

                printf("  Got DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK from Bind:\r\n");

                printf("    wRightID = %d\r\n", pExtCallback->wRightID);
                printf("    wType    = %d\r\n", pExtCallback->pRestriction->wType);
                printf("    wFlags   = %x\r\n", pExtCallback->pRestriction->wFlags);

                printf("    Data     = ");

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_LOOP_INDEX_UNDERFLOW, "ignore loop index underflow warning, already handled" )
                for( i = pExtCallback->pRestriction->ibData; (i - pExtCallback->pRestriction->ibData) < pExtCallback->pRestriction->cbData; i++ )
                {
                    printf("0x%.2X ", GET_BYTE( pExtCallback->pRestriction->pbBuffer, i ) );
                }
PREFAST_POP
                printf("\r\n\r\n");

                /* Report that restriction was not understood */
                dr = DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD;
            }
            break;
        case DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK:
            {
                const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT *pExtCallback = (const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT*)f_pvPolicyCallbackData;
                DRM_DWORD i = 0;

                printf("  Got DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK from Bind:\r\n");

                printf("    wRightID = %d\r\n", pExtCallback->wRightID);
                printf("    wType    = %d\r\n", pExtCallback->pRestriction->wType);
                printf("    wFlags   = %x\r\n", pExtCallback->pRestriction->wFlags);

                printf("    Data     = ");
                for( i = pExtCallback->pRestriction->ibData; (i - pExtCallback->pRestriction->ibData) < pExtCallback->pRestriction->cbData; i++ )
                {
                    printf("0x%.2X ", GET_BYTE( pExtCallback->pRestriction->pbBuffer, i ) );
                }
                printf("\r\n\r\n");

                /* Report that restriction was not understood */
                dr = DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD;
            }
            break;
        case DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK:
            {
                const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT *pExtCallback = (const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT*)f_pvPolicyCallbackData;
                DRM_DWORD i = 0;

                printf("  Got DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK from Bind:\r\n");

                printf("    wRightID = %d\r\n", pExtCallback->wRightID);
                printf("    wType    = %d\r\n", pExtCallback->pRestriction->wType);
                printf("    wFlags   = %x\r\n", pExtCallback->pRestriction->wFlags);

                printf("    Data     = ");
                for( i = pExtCallback->pRestriction->ibData; (i - pExtCallback->pRestriction->ibData) < pExtCallback->pRestriction->cbData; i++ )
                {
                    printf("0x%.2X ", GET_BYTE( pExtCallback->pRestriction->pbBuffer, i ) );
                }
                printf("\r\n\r\n");

                /* Report that restriction was not understood */
                dr = DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD;
            }
            break;
    default:
        printf("  Callback from Bind with unknown callback type of %d.\r\n", f_dwCallbackType);

        /* Report that this callback type is not implemented */
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    return dr;

}

EXIT_PK_NAMESPACE_CODE;

