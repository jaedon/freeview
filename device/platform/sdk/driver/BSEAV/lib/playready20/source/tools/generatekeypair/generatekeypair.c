/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmcmdlnpars.h>
#include <oempkcrypto.h>
#include <oemeccp256.h>
#include <oemrsa.h>
#include <drmlicense.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>
#include <drmdevcert.h>

#include <drmtoolsconstants.h>
#include <drmtoolsmacros.h>
#include <drmtoolsutils.h>
#include <drmxmlrsakeys.h>


#define XML_KEY_GEN_INIT_SIZE   1024*5
static DRM_CRYPTO_CONTEXT g_CryptoCtx = {0};

typedef struct __tagDRMSTATE
{
    FILE                *Outfp;
    FILE                *OutfpXml;
    DRM_CRYPTO_CONTEXT  *pCrypto;
} DRMSTATE;

/****************************************************************************/
static void _DRMClose(
    __inout DRMSTATE **f_ppState )
{
    if ( NULL != *f_ppState )
    {
        if ( NULL != (*f_ppState)->Outfp )
        {
            fclose( (*f_ppState)->Outfp );
        } /* end if */
        if ( NULL != (*f_ppState)->OutfpXml )
        {
            fclose( (*f_ppState)->OutfpXml );
        } /* end if */
        SAFE_OEM_FREE( (*f_ppState)->pCrypto );
        SAFE_OEM_FREE( *f_ppState );
    } /* end if */
}


/****************************************************************************/
static DRM_RESULT _DRMInit(
    __inout DRMSTATE **f_ppState )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkMem(  *f_ppState = ( DRMSTATE* ) Oem_MemAlloc( SIZEOF( DRMSTATE ) ) );
    ZEROMEM( *f_ppState, SIZEOF( DRMSTATE ) );

    ( *f_ppState )->Outfp = NULL;

    ChkMem(   ( *f_ppState )->pCrypto
            = ( DRM_CRYPTO_CONTEXT* )Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( ( *f_ppState )->pCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   _OutputArrayToText
**
** Synopsis :   Outputs array of numbers to a text file as array in C form
**              Example "{ 0x23, 0x24, 0x25, 0x26 }"
**
** Arguments :  [f_fp]              - file pointer
**              [f_pArray]          - array of numbers to output
**              [f_dwElemCount]     - number of elements in array
**              [f_dwElemSize]      - size of element (number) in array in bytes
**              [f_dwElemPerLine]   - Number of elements (numbers) per line in output file
**              [f_pchNumberFormat] - fprint formatting for element
**
** Returns :    DRM_SUCCESS - Successfully outputs data into the file
******************************************************************************/
static DRM_RESULT _OutputArrayToText(
    IN       FILE      *f_fp,
    __in_bcount( f_dwElemCount*f_dwElemSize ) const DRM_VOID  *f_pArray,
    IN const DRM_DWORD  f_dwElemCount,
    IN const DRM_DWORD  f_dwElemSize,
    IN const DRM_DWORD  f_dwElemPerLine,
    __in_z __format_string const DRM_CHAR  *f_pchNumberFormat )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i  = 0;
    DRM_DWORD  dwValue = 0;

    DRMASSERT( f_fp != NULL );
    DRMASSERT( f_pArray != NULL );
    DRMASSERT( f_dwElemPerLine > 0 );
    DRMASSERT( f_pchNumberFormat != NULL );

    fprintf( f_fp, "{" );
    for ( i = 0; i < f_dwElemCount; i++ )
    {
        if( 0 == i % f_dwElemPerLine )
        {
            fprintf( f_fp, "\n\t" );
        } /* end if */
        if ( SIZEOF( DRM_BYTE ) == f_dwElemSize )
        {
            dwValue = *(DRM_BYTE*)((DRM_BYTE*)f_pArray + i*f_dwElemSize);
        }
        else if ( SIZEOF( DRM_WORD ) == f_dwElemSize )
        {
            dwValue = *(DRM_WORD*)((DRM_BYTE*)f_pArray + i*f_dwElemSize);
        }
        else if ( SIZEOF( DRM_DWORD ) == f_dwElemSize )
        {
            dwValue = *(DRM_DWORD*)((DRM_BYTE*)f_pArray + i*f_dwElemSize);
        }
        fprintf( f_fp, f_pchNumberFormat, dwValue );
        if ( i == f_dwElemCount - 1 )
        {
            fprintf( f_fp, "\n" );
            break;
        }
        else
        {
            fprintf( f_fp, ", " );
        }
    } /* end for */
    fprintf( f_fp, "}\n" );

    return dr;
}

/******************************************************************************
**
** Function :   _OutputECCKeysToText
**
** Synopsis :   Outputs ECC keys to a text file
**
** Arguments :  [f_fp]        - file pointer to output file
**              [f_pPubKey]   - pointer to ECC public key
**              [f_cbPubKey]  - size of public key
**              [f_pPrivKey]  - pointer to ECC private key
**              [f_cbPrivKey] - size of private key
**
** Returns :    DRM_SUCCESS - Successfully outputs data into the file
******************************************************************************/
static DRM_RESULT _OutputECCKeysToText(
    IN                               FILE       *f_fp,
    __in_bcount( f_cbPubKey )  const DRM_BYTE   *f_pPubKey,
    IN                         const DRM_DWORD   f_cbPubKey,
    __in_bcount( f_cbPrivKey ) const DRM_BYTE   *f_pPrivKey,
    IN                         const DRM_DWORD   f_cbPrivKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_fp != NULL );
    DRMASSERT( f_pPubKey != NULL );
    DRMASSERT( f_pPrivKey != NULL );

    /* Output the generated private key to the text file */
    fprintf( f_fp, "\nPRIVKEY:\n" );
    ChkDR( _OutputArrayToText( f_fp, f_pPrivKey, f_cbPrivKey, SIZEOF(DRM_BYTE), 10, " 0x%2.2x" ) );

    /* Output the generated public key to the text file */
    fprintf( f_fp, "\nPUBKEY:\n" );
    ChkDR( _OutputArrayToText( f_fp, f_pPubKey, f_cbPubKey, SIZEOF(DRM_BYTE), 10, " 0x%2.2x" ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   _OutputECCKeysToXML
**
** Synopsis :   Output the generated ECC public and private keys to the XML file
**
** Arguments :  [f_fp]        - file pointer to output file
**              [f_pPubKey]   - pointer to ECC public key
**              [f_cbPubKey]  - size of public key
**              [f_pPrivKey]  - pointer to ECC private key
**              [f_cbPrivKey] - size of private key
**
** Returns :    DRM_SUCCESS - Successfully outputs data into the file
******************************************************************************/
static DRM_RESULT _OutputECCKeysToXML(
    IN                               FILE       *f_fp,
    __in_bcount( f_cbPubKey )  const DRM_BYTE   *f_pPubKey,
    IN                         const DRM_DWORD   f_cbPubKey,
    __in_bcount( f_cbPrivKey ) const DRM_BYTE   *f_pPrivKey,
    IN                         const DRM_DWORD   f_cbPrivKey )
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_DWORD    i  = 0;
    DRM_STRING   dstrPubkey  = EMPTY_DRM_STRING;
    DRM_STRING   dstrPrivkey = EMPTY_DRM_STRING;
    DRM_STRING   dstrClose   = EMPTY_DRM_STRING;
    _XMBContext *pbOut       = NULL;

    DRMASSERT( f_fp != NULL );
    DRMASSERT( f_pPubKey != NULL );
    DRMASSERT( f_pPrivKey != NULL );

    /* Convert Public key to base64 encoding */
    ChkDR( DRMTOOLS_Base64EncodeOfByteArray( f_pPubKey, f_cbPubKey, &dstrPubkey ) );

    /* Convert private key to base64 encoding */
    ChkDR( DRMTOOLS_Base64EncodeOfByteArray( f_pPrivKey, f_cbPrivKey, &dstrPrivkey ) );

    ChkMem( pbOut = ( _XMBContext* ) Oem_MemAlloc( XML_KEY_GEN_INIT_SIZE ) );
    ZEROMEM( pbOut, XML_KEY_GEN_INIT_SIZE );

    ChkDR( DRM_XMB_CreateDocument( XML_KEY_GEN_INIT_SIZE, pbOut, &g_dstrTagKeypair ) );

    /* Add PUBKEY node */
    CallXmlBuilder( pbOut, DRM_XMB_AddXMLNode( pbOut, &g_dstrTagPubkey, ( DRM_CONST_STRING* )&dstrPubkey ) );

    /* Add PRIVKEY node */
    CallXmlBuilder( pbOut, DRM_XMB_AddXMLNode( pbOut, &g_dstrTagPrivkey, ( DRM_CONST_STRING* )&dstrPrivkey ) );

    /* close the root node */
    CallXmlBuilder( pbOut, DRM_XMB_CloseDocument( pbOut, &dstrClose ) );

    i = dstrClose.cchString * SIZEOF( DRM_WCHAR );
    if ( fwrite( dstrClose.pwszString, 1, i, f_fp ) != i )
    {
        ChkDR( DRM_E_FILEWRITEERROR );
    } /* end if */

ErrorExit:
    SAFE_OEM_FREE( pbOut) ;
    SAFE_OEM_FREE( dstrPubkey.pwszString );
    SAFE_OEM_FREE( dstrPrivkey.pwszString );

    return dr;
}

/******************************************************************************
**
** Function :   _OutputRSAKeysToText
**
** Synopsis :   Output the RSA public and private keys to the text file
**
** Arguments :  [f_fp]        - file pointer to output file
**              [f_pPubKey]   - pointer to RSA public key structure
**              [f_pPrivKey]  - pointer to RSA private key structure
**
** Returns :    DRM_SUCCESS - Successfully outputs data into the file
******************************************************************************/
static DRM_RESULT _OutputRSAKeysToText(
    IN       FILE                *f_fp,
    IN const DRM_RSA_PUBLIC_KEY  *f_pPubKey,
    IN const DRM_RSA_PRIVATE_KEY *f_pPrivKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    INTERNAL_DRM_RSA_PRIVATE_KEY *pPvtKeyInternal = ( INTERNAL_DRM_RSA_PRIVATE_KEY * )f_pPrivKey;
    INTERNAL_DRM_RSA_PUBLIC_KEY  *pPubKeyInternal = ( INTERNAL_DRM_RSA_PUBLIC_KEY  * )f_pPubKey;

    DRMASSERT( f_fp != NULL );
    DRMASSERT( f_pPubKey != NULL );
    DRMASSERT( f_pPrivKey != NULL );

    fprintf( f_fp, "\nPUBKEY: " );
    fprintf( f_fp, "Key length: %u\n", pPubKeyInternal->eKeyLength );
    fprintf( f_fp, "Modulus:\n" );
    ChkDR( _OutputArrayToText( f_fp, pPubKeyInternal->rgdModulus, pPubKeyInternal->cdModulus, SIZEOF(digit_t), 8, " 0x%8.8x" ) );
    fprintf( f_fp, "Exponent:\n" );
    ChkDR( _OutputArrayToText( f_fp, pPubKeyInternal->rgdPubExp, CDIGITS_PUBEXP, SIZEOF(digit_t), 8, " 0x%8.8x" ) );

    fprintf( f_fp, "\nPRIVKEY: " );
    fprintf( f_fp, "Key length: %u\n", pPvtKeyInternal->eKeyLength );
    fprintf( f_fp, "Length of primes: %u, %u\n", pPvtKeyInternal->cdPrimes[0], pPvtKeyInternal->cdPrimes[1] );

    ChkDR( _OutputArrayToText( f_fp, pPvtKeyInternal->rgdPrimes[0], pPvtKeyInternal->cdPrimes[0], SIZEOF(digit_t), 8, " 0x%8.8x" ) );
    ChkDR( _OutputArrayToText( f_fp, pPvtKeyInternal->rgdPrimes[1], pPvtKeyInternal->cdPrimes[1], SIZEOF(digit_t), 8, " 0x%8.8x" ) );

    fprintf( f_fp, "Exponents:\n" );
    ChkDR( _OutputArrayToText( f_fp, pPvtKeyInternal->rgdCRTExponents[0], pPvtKeyInternal->cdPrimes[0], SIZEOF(digit_t), 8, " 0x%8.8x" ) );
    ChkDR( _OutputArrayToText( f_fp, pPvtKeyInternal->rgdCRTExponents[1], pPvtKeyInternal->cdPrimes[1], SIZEOF(digit_t), 8, " 0x%8.8x" ) );

    fprintf( f_fp, "IQMP:\n" );
    ChkDR( _OutputArrayToText( f_fp, pPvtKeyInternal->rgdIQMP, pPvtKeyInternal->cdPrimes[0]+pPvtKeyInternal->cdPrimes[1], SIZEOF(digit_t), 8, " 0x%8.8x" ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   _OutputRSAKeysToXML
**
** Synopsis :   Output the RSA public and private keys to the XML file
**
** Arguments :  [f_fp]        - file pointer to output file
**              [f_pPubKey]   - pointer to RSA public key structure
**              [f_pPrivKey]  - pointer to RSA private key structure
**
** Returns :    DRM_SUCCESS - Successfully outputs data into the file
******************************************************************************/
static DRM_RESULT _OutputRSAKeysToXML(
    IN       FILE                *f_fp,
    IN const DRM_RSA_PUBLIC_KEY  *f_pPubKey,
    IN const DRM_RSA_PRIVATE_KEY *f_pPrivKey )
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_DWORD    i  = 0;
    DRM_STRING   dstrClose               = EMPTY_DRM_STRING;
    _XMBContext *pbOut                   = NULL;

    DRMASSERT( f_fp != NULL );
    DRMASSERT( f_pPubKey != NULL );
    DRMASSERT( f_pPrivKey != NULL );

    /* Initialize XML builder context */
    ChkMem( pbOut = ( _XMBContext* ) Oem_MemAlloc( XML_KEY_GEN_INIT_SIZE ) );
    ZEROMEM( pbOut, XML_KEY_GEN_INIT_SIZE );

    ChkDR( DRM_XMB_CreateDocument( XML_KEY_GEN_INIT_SIZE, pbOut, &g_dstrTagKeypair ) );

    /* Add PUBKEY node */

    CallXmlBuilder( pbOut, DRM_XMB_OpenNode( pbOut, &g_dstrTagPubkey ) );
    CallXmlBuilder( pbOut, DRM_XMB_OpenNode( pbOut, &g_dstrTagKeyValue ) );

    ChkDR( DRM_XML_RSA_WritePublicKeyNode( pbOut, &g_dstrTagRSAKeyValue, f_pPubKey ) );

    CallXmlBuilder( pbOut, DRM_XMB_CloseCurrNode( pbOut, NULL ) );
    CallXmlBuilder( pbOut, DRM_XMB_CloseCurrNode( pbOut, NULL ) );

    /* Add PRIVKEY node  */
    CallXmlBuilder( pbOut, DRM_XMB_OpenNode( pbOut, &g_dstrTagPrivkey ) );
    CallXmlBuilder( pbOut, DRM_XMB_OpenNode( pbOut, &g_dstrTagKeyValue ) );

    ChkDR( DRM_XML_RSA_WritePrivateKeyNode( pbOut, &g_dstrTagRSAKeyValue, f_pPrivKey ) );

    CallXmlBuilder( pbOut, DRM_XMB_CloseCurrNode( pbOut, NULL ) );
    CallXmlBuilder( pbOut, DRM_XMB_CloseCurrNode( pbOut, NULL ) );

    /* close the root node */
    CallXmlBuilder( pbOut, DRM_XMB_CloseDocument( pbOut, &dstrClose ) );

    i = dstrClose.cchString * SIZEOF( DRM_WCHAR );
    if ( fwrite( dstrClose.pwszString, 1, i, f_fp ) != i )
    {
        ChkDR( DRM_E_FILEWRITEERROR );
    } /* end if */

ErrorExit:
    SAFE_OEM_FREE( pbOut ) ;

    return DRM_SUCCESS;
}


/******************************************************************************
**
** Function :   _CreateKeyPair
**
** Synopsis :   Creates public/private key pair according to specified algorithm,
**              key length and other clarifying data
**
** Arguments :  [f_eAlgorithm]    - Encryption algorithm
**              [f_eRSAKeyLength] - Length for RSA key, used by RSA algorithm only
**              [f_fRestrictPriv] - Used by ECC256 algorithm
**              [f_pState]        -
**              [f_pCryptoCtx]    - Crypto context
**
** Returns :    DRM_SUCCESS - Successfully outputs data into the file
******************************************************************************/
static DRM_RESULT _CreateKeyPair(
    __in    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS  f_eAlgorithm,
    __in    DRM_RSA_SUPPORTED_KEY_LENGTHS      f_eRSAKeyLength,
    __in    DRM_BOOL                           f_fRestrictPriv,
    __inout DRMSTATE                          *f_pState,
    __inout DRM_CRYPTO_CONTEXT                *f_pCryptoCtx )
{

    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_BYTE   *pPubKey     = NULL;
    DRM_BYTE   *pPrivKey    = NULL;
    DRM_DWORD   cbPrivKey   = 0;
    DRM_DWORD   cbPubKey    = 0;

    ChkArg( NULL != f_pState );
    ChkArg( f_pCryptoCtx != NULL );
    /*
    ** Allocate correct amount of memory and then gen ECC key pair,
    ** this is controlled by the type of ECC key that was specified by f_eKeySize
    */
    switch ( f_eAlgorithm )
    {
    case eDRM_ECC_P160:
        cbPrivKey = SIZEOF( PRIVKEY );
        cbPubKey  = SIZEOF( PUBKEY );
        ChkArg( !f_fRestrictPriv ); /* restricted ECC keys are only an option for P256 */
        break;
    case eDRM_ECC_P256:
        cbPrivKey = SIZEOF( PRIVKEY_P256 );
        cbPubKey  = SIZEOF( PUBKEY_P256 );
        break;
    case eDRM_RSA:
        cbPrivKey = SIZEOF( DRM_RSA_PRIVATE_KEY );
        cbPubKey  = SIZEOF( DRM_RSA_PUBLIC_KEY );
        break;
    default: /* should never get to this case */
        ChkDR( DRM_E_INVALIDARG );
    } /* end switch */

    /* Allocate memory for keys */
    ChkMem( pPubKey  = ( DRM_BYTE* ) Oem_MemAlloc( cbPubKey ) );
    ChkMem( pPrivKey = ( DRM_BYTE* ) Oem_MemAlloc( cbPrivKey ) );

    /*
    ** Generate keys
    */

    /*
    ** Generate the key pair such that the private key can be:
    **   -Any 256bit integer less then the curve order if we are not using restricted keys
    **   -Any 256bit x-coord on the curve (and thus can be encrypted with P256 in a single
    **    block) if we are use restricted keys.
    */
    if ( f_eAlgorithm == eDRM_ECC_P256 && !f_fRestrictPriv )
    {
        ChkDR( OEM_ECC_GenKeyPair_P256( ( PUBKEY_P256* )pPubKey,
                                        ( PRIVKEY_P256* )pPrivKey ,
                                        (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext ) );
    } /* end if */
    else
    {
        ChkDR( DRM_PK_GenKeyPair( f_pState->pCrypto->rgbCryptoContext,
                                  f_eAlgorithm,
                                  f_eRSAKeyLength,
                                  &cbPubKey,
                                  pPubKey,
                                  &cbPrivKey,
                                  pPrivKey ) );
    } /* end else */

    /*
    ** Test key pair
    */
    ChkDR( DRMTOOLS_TestKeyPair( f_pState->pCrypto,
                                 f_eAlgorithm,
                                 pPubKey,
                                 cbPubKey,
                                 pPrivKey,
                                 cbPrivKey ) );

    /*
    ** If we are to output to the text file then do it now for both the priv and pub keys
    ** (This is toggled by setting the file pointer to the text file to and from NULL)
    */
    if ( NULL != f_pState->Outfp )
    {
        if ( f_eAlgorithm == eDRM_ECC_P160 || f_eAlgorithm == eDRM_ECC_P256 )
        {
            ChkDR( _OutputECCKeysToText( f_pState->Outfp, pPubKey, cbPubKey, pPrivKey, cbPrivKey ) ) ;
        }
        else if ( f_eAlgorithm == eDRM_RSA )
        {
            ChkDR( _OutputRSAKeysToText( f_pState->Outfp, (DRM_RSA_PUBLIC_KEY*) pPubKey, (DRM_RSA_PRIVATE_KEY*) pPrivKey ) ) ;
        }

    }

    /*
    ** If we are to output to the XML file then do it now for both the priv and pub keys
    ** (This is toggled by setting the file pointer to the XML file to and from NULL)
    */
    if ( NULL != f_pState->OutfpXml )
    {
        if ( f_eAlgorithm == eDRM_ECC_P160 || f_eAlgorithm == eDRM_ECC_P256 )
        {
            ChkDR( _OutputECCKeysToXML( f_pState->OutfpXml, pPubKey, cbPubKey, pPrivKey, cbPrivKey ) );
        }
        else if ( f_eAlgorithm == eDRM_RSA )
        {
            ChkDR( _OutputRSAKeysToXML( f_pState->OutfpXml, (DRM_RSA_PUBLIC_KEY*) pPubKey, (DRM_RSA_PRIVATE_KEY*) pPrivKey ) ) ;
        }
    } /* end if */

ErrorExit:

    SAFE_OEM_FREE( pPrivKey );
    SAFE_OEM_FREE( pPubKey );

   return dr;
}

/****************************************************************************/


/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void _PrintUsage(
    __in const DRM_WCHAR *f_pwszAppName)
{
    printf( "Syntax:  %S [-?] -o:OutputFile -x:OutputXmlFile -p:{ECC160, ECC256, RSA1024, RSA2048} [-r]\n"
            "Example: %S -o:out160.txt  -x:out160.xml  -p:ECC160\n"
            "Example: %S -o:out256.txt, -x:out256.xml  -p:ECC256\n"
            "Example: %S -o:out256.txt  -x:out256.xml  -p:ECC256 -r\n"
            "Example: %S -o:out1024.txt -x:out1024.xml -p:RSA1024\n"
            "Example: %S -o:out2048.txt -x:out2048.xml -p:RSA2048\n"
            "\n"
            "Options:\n"
            "\t[-?]: Display usage information\n"
            "\t[-o]: The output file name for a text format (displaying the\n"
            "\t      keys in byte format). If this option is not specified then\n"
            "\t      the -x option must be used, use of both is permitted.\n"
            "\t[-x]: The output file name for an XML format (displaying the\n"
            "\t      keys in base 64 encoded format). If this option is not specified \n"
            "\t      then the -o option must be used, use of both is permitted.\n"
            "\t[-p]: The type of key pair to generate. There are currently only 4 valid\n"
            "\t      options for this arg: ECC160 which will generate a WMDRM ECC P160 legacy\n"
            "\t      key pair, ECC256 which will generate a FIPS ECC P256 key pair,\n"
            "\t      RSA1024 which will generate a RSA 1024 bit length key pair or\n"
            "\t      RSA2048 which will generate a RSA 2048 bit length key pair\n"
            "\t[-r]: Use a restricted private key such that it can always be encrypted\n"
            "\t      in a single P256 block. This is an optional arg that is only valid\n"
            "\t      when -P:256 is specified. NOTE: This shrinks the total key space by\n"
            "\t      roughly 1/4th, allowing for around 2^254 total private keys.\n\n",
            f_pwszAppName,
            f_pwszAppName,
            f_pwszAppName,
            f_pwszAppName,
            f_pwszAppName,
            f_pwszAppName );
} /* end _PrintUsage */

/******************************************************************************
**
*******************************************************************************
*/

DRM_LONG DRM_CALL DRM_Main(
    __in              DRM_LONG    argc,
    __in_ecount(argc) DRM_WCHAR **argv )
{
    DRM_PKCRYPTO_SUPPORTED_ALGORITHMS KeyType = eDRM_ECC_P160;
    DRM_RSA_SUPPORTED_KEY_LENGTHS     eRSAKeyLength = eDRM_RSA_UNKNOWN;
    DRM_CONST_STRING dstrOutputFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrXMLFile    = EMPTY_DRM_STRING;
    DRM_BOOL         fAlgSpecified  = FALSE;
    DRM_BOOL         fRestrictPriv  = FALSE;
    DRM_LONG     i      = 0;
    DRM_RESULT   dr     = DRM_SUCCESS;
    DRMSTATE    *pState = NULL;

    PrintCopyrightInfo( "Key Pair Generation Tool" );

    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    for( i = 1; i<argc; i++ )
    {
        DRM_WCHAR        wchOption = 0;
        DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;
        if ( !DRM_CMD_ParseCmdLine(argv[i], &wchOption, &dstrParam, NULL) )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
        wchOption = DRMCRT_towlower( wchOption );
        switch( wchOption )
        {
        case WCHAR_CAST( 'o' ): /* Get the output text filename, this is an optional field */
            dstrOutputFile = dstrParam;
            break;
        case WCHAR_CAST( 'x' ): /* Get the output XML filename, this is an optional field */
            dstrXMLFile = dstrParam;
            break;
        case WCHAR_CAST( 'p' ): /* Get the key type, this is a required param */
            fAlgSpecified = TRUE;
            if ( 0 == DRMCRT_wcsncmp( dstrParam.pwszString, (const DRM_WCHAR *)L"ECC160", 6 ) )
            {
                /*
                ** We are going to generate ECC P160 keys
                */
                KeyType = eDRM_ECC_P160;
            } /* end if */
            else if ( 0 == DRMCRT_wcsncmp( dstrParam.pwszString, (const DRM_WCHAR *)L"ECC256", 6 ))
            {
                /*
                ** We are going to generate ECC P256 keys
                */
                KeyType = eDRM_ECC_P256;
            } /* end if */
            else if ( 0 == DRMCRT_wcsncmp( dstrParam.pwszString, (const DRM_WCHAR *)L"RSA1024", 7 ) )
            {
                /* We are going to generate RSA 1024 bit keys */
                KeyType = eDRM_RSA;
                eRSAKeyLength = eDRM_RSA_1024;
            } /* end if */
            else if ( 0 == DRMCRT_wcsncmp( dstrParam.pwszString, (const DRM_WCHAR *)L"RSA2048", 7 ))
            {
                /* We are going to generate RSA 2048 bit keys */
                KeyType = eDRM_RSA;
                eRSAKeyLength = eDRM_RSA_2048;
            } /* end if */
            else
            {
                /*
                ** The key type that the user input is unsupported!
                */
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            } /* end if */
            break;
        case WCHAR_CAST( 'r' ): /* Restrict the private key space such that it may be encrypted in one ECCP256 block */
            fRestrictPriv = TRUE;
            break;
        case WCHAR_CAST( '?' ):
            goto _PrintUsage;
        default:
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }

    /*
    ** Algorithm type must be specified
    */
    if ( !fAlgSpecified )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    /*
    ** the -r option may only be used with P256 keys
    */
    if (     TRUE == fRestrictPriv
          && eDRM_ECC_P256 != KeyType )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    } /* end if */

    /*
    ** One or both of the output files must be specified, check to make sure this is true
    */
    if (   (   dstrOutputFile.cchString == 0
            && dstrXMLFile.cchString    == 0 ) )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    } /* end if */

    if ( dstrOutputFile.cchString > ( DRM_MAX_PATH - 1 )
      || dstrXMLFile.cchString    > ( DRM_MAX_PATH - 1 ) )
    {
        printf( "File name is too big. Max Filename is %d \n", DRM_MAX_PATH );
        ChkDR( DRM_E_FAIL );
    } /* end if */

    ChkDR( _DRMInit( &pState ) );

    /*
    ** If specified then open the text file
    */
    if ( 0 < dstrOutputFile.cchString )
    {
        DRM_CHAR rgchFileName[ DRM_MAX_PATH ];

        /*  Zero memory so buffer after call to DRM_UTL_DemoteUNICODEtoASCII is NULL terminated */
        ZEROMEM( rgchFileName, SIZEOF( rgchFileName ) );

        /* Safe to use, assume that input parameter is in ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( dstrOutputFile.pwszString,
                                      rgchFileName,
                                      dstrOutputFile.cchString );

        pState->Outfp = ( FILE* )fopen( rgchFileName, "w+b" );
        if ( NULL == pState->Outfp )
        {
            printf( "Output text file not opened\n" );
            ChkDR( DRM_E_FAIL );
        } /* end if */
    } /* end if */

    /*
    ** If specified then open the XML file
    */
    if ( 0 < dstrXMLFile.cchString )
    {
        DRM_CHAR rgchFileName[ DRM_MAX_PATH ];

        /*  Zero memory so buffer after call to DRM_UTL_DemoteUNICODEtoASCII is NULL terminated */
        ZEROMEM( rgchFileName, SIZEOF( rgchFileName ) );

        /* Safe to use, assume that input parameter is in ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( dstrXMLFile.pwszString,
                                      rgchFileName,
                                      dstrXMLFile.cchString );

        pState->OutfpXml = (FILE*)fopen( rgchFileName, "w+b" );
        if ( NULL == pState->OutfpXml )
        {
            printf( "Output XML file not opened\n" );
            ChkDR( DRM_E_FAIL );
        } /* end if */
    } /* end if */

    /*
    ** Create the key pair and output them the the opened files
    */
    if ( pState->Outfp || pState->OutfpXml )
    {
        ChkDR( _CreateKeyPair( KeyType, eRSAKeyLength, fRestrictPriv, pState, &g_CryptoCtx ) );
    } /* end if */

ErrorExit:

    if ( pState )
    {
        _DRMClose( &pState );
    } /* end if */
    if ( DRM_FAILED(dr) )
    {
        printf( "Error: 0x%08X\n", dr );
    }
    return dr;

_PrintUsage:
    _PrintUsage( argv[0] );
    return dr;
}

