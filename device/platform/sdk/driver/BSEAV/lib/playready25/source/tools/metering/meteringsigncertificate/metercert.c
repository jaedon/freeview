/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h>

#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcrt.h>
#include <drmcmdlnpars.h>
#include <drmxmlparser.h>
#include <drmxmlbuilder.h>

#include <drmtoolsconstants.h>
#include <drmtoolsmacros.h>
#include <drmtoolsutils.h>
#include <drmmeterconstants.h>
#include <drmconstants.h>
#include <drmmathsafe.h>

/*
const BYTE Cert [SIZEOF (CERT)] =
{
    'certVersion
    0x00, 0x01, 0x00, 0x00,

    'dataLen
    0x38, 0x00, 0x00, 0x00,

    'signature
    0x75, 0xED, 0xB7, 0xE0, 0xF9, 0xCA, 0x83, 0x2C,
    0x1D, 0x9C, 0x94, 0x71, 0x94, 0xEC, 0xFE, 0xB3,
    0x92, 0xB3, 0x82, 0x55, 0x47, 0x5D, 0x5F, 0xDE,
    0x54, 0x62, 0x4A, 0xA9, 0x8E, 0xAB, 0xE8, 0x22,
    0xBE, 0x1E, 0xCE, 0xAC, 0x36, 0xCE, 0x0C, 0x1F,

    'public key
    0xAE, 0xF2, 0x91, 0xD5, 0xDA, 0xBE, 0x13, 0x37,
    0x46, 0x0F, 0xC3, 0x43, 0xD8, 0x88, 0x64, 0x9F,
    0x43, 0x8F, 0x12, 0x85, 0x99, 0x64, 0xA0, 0xB0,
    0x82, 0x27, 0x69, 0xED, 0x8E, 0x52, 0x1D, 0x1F,
    0x8D, 0x14, 0x92, 0x5A, 0xCD, 0xD3, 0xD6, 0x7C,

    0x14, 0x04, 0x04, 0x1E, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01
};

*/

/* *************************  METER CERT FORMAT ******************************
 <METERCERT version="1.0">
    <DATA>
        <MID>UiW5yBMep2CuevGg5+FgA3==</MID>
        <PUBLICKEY>meter cert public key</PUBLICKEY>
        <URL>http://www.bad.contoso.com</URL>
    </DATA>
    <CERTIFICATECHAIN type="MSDRM">
       <CERTIFICATE>AAEAADgAAADx8JZr89PDErAy8nc4Dk*ndLPDbqG0oXwUd3YrbpUiZx5!xDYvTuoGkLwhX2yhPd*XemSyR4vt980PUUPh0mOVjfcPK0zVi9lcr5zN8Jy7LhQDCA0AAAAIAAAAAQAAAAE=</CERTIFICATE>
       <CERTIFICATE>AAEAADgAAADu8MlLHuLr6TlzZkXkL3*jW7kJKYNtuoDnkMItge!KwrcRjRhd0NAV7BZ*gTLq*LKyT3GcIWH8d5JkWiklMf7Aod!lhBgJpcTRu!chtojzMhQDBxMAABINAAAAAgAATGk=</CERTIFICATE>
    </CERTIFICATECHAIN>
    <SIGNATURE>
        <HASHALGORITHM type="SHA"></HASHALGORITHM>
        <SIGNALGORITHM type="MSDRM"></SIGNALGORITHM>
        <VALUE>4TFUkNdkfX!zaTAug0*drckKrS98ysCeeycyL6lnEKArQiNbRjKIgQ==</VALUE>
    </SIGNATURE>
</METERCERT>

 ****************************************************************************/

/*******************************************************************
 *                    PRIVATE FUNCTION ParseMeterCert
 *
 * purpose: verify that all required entries are present in the
 *          ANSI XML and that they are properly formed
 *          write the file to the passed-in buffer as UNICODE
 *******************************************************************/

static DRM_RESULT ParseMeterCert (const DRM_CONST_STRING *f_pdstrXML,
                                        DRM_CONST_STRING *f_pdstrData)
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_CONST_STRING dstrDataMID       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataURL       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataPublicKey = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListData      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListTop       = EMPTY_DRM_STRING;
    DRM_DWORD        cbMID              = 0;
    DRM_MID          oMID;

    ChkDR (DRM_XML_GetNode(f_pdstrXML,
                           &g_dstrTagMeterCert,
                            NULL,
                            NULL,
                            0,
                            NULL,
                           &dstrListTop));

    /* verify that the tags are present and properly formed */

    ChkDR (DRM_XML_GetNode(&dstrListTop,  &g_dstrTagData,       NULL, NULL, 0, NULL, &dstrListData));
    ChkDR (DRM_XML_GetNode(&dstrListData, &g_dstrTagMID,        NULL, NULL, 0, NULL, &dstrDataMID));
    ChkDR (DRM_XML_GetNode(&dstrListData, &g_dstrTagURL,        NULL, NULL, 0, NULL, &dstrDataURL));
    ChkDR (DRM_XML_GetNode(&dstrListData, &g_dstrTagDataPubKey, NULL, NULL, 0, NULL, &dstrDataPublicKey));

    if (dstrDataMID.cchString != CCH_BASE64_EQUIV (SIZEOF (DRM_MID)))
    {
        printf ("MID string malformed\n");
        ChkDR  (DRM_E_CH_BAD_KEY);
    }

    /* Check that we can decode the MID and that it's the right size */
    cbMID = SIZEOF( DRM_MID );
    dr = DRM_B64_DecodeW( &dstrDataMID, &cbMID, (DRM_BYTE*)&oMID, 0 );
    if( DRM_FAILED( dr )
        || cbMID != SIZEOF( DRM_MID ) )
    {
        ChkDR (DRM_E_CH_BAD_KEY);
    }

    /* Our B64 decode will accept ! and *, but this is not standard B64 so we need to reject them */
    for( cbMID = 0; cbMID < dstrDataMID.cchString; cbMID++ )
    {
        DRM_DWORD wc = NATIVE_WCHAR( dstrDataMID.pwszString[ cbMID ] );

        if(     wc == 0x21  /* '!' */
            ||  wc == 0x2A  /* '*' */ )
        {
            ChkDR (DRM_E_CH_BAD_KEY);
        }
    }

    if (dstrDataURL.cchString < 10)
    {
        printf ("URL string malformed\n");
        ChkDR  (DRM_E_INVALIDARG);
    }

    if (dstrDataPublicKey.cchString == 0)
    {
        printf ("Public Key string malformed\n");
        ChkDR  (DRM_E_INVALIDARG);
    }

    f_pdstrData->pwszString = dstrListTop.pwszString;
    f_pdstrData->cchString  = dstrListTop.cchString;

ErrorExit:

    return dr;
}

/*******************************************************************
 *                    PRIVATE FUNCTION SignMeterCert
 *
 * purpose: accumulate the signature for the XML's <DATA> section
 *          encode this signature as base64
 *          insert the <SIGNATURE> section
 *******************************************************************/

static DRM_RESULT SignMeterCert  (DRM_CONST_STRING *f_pdstrData,
                                  DRM_WCHAR        *f_pwszSignature,
                                  DRM_DWORD         f_cchSignature  )
{
    DRM_BYTE             rgbSignature[__CB_DECL( DRM_ECC160_SIGNATURE_LEN )]   = { 0x00 };
    DRM_WCHAR            rgchSignature[CCH_BASE64_EQUIV(SIZEOF(rgbSignature))] = { 0 };
    const DRM_BYTE      *pbData         = PB_DSTR(f_pdstrData);
    DRM_DWORD            cbData         = CB_DSTR(f_pdstrData);
    DRM_DWORD            cbCert         = SIZEOF(CERT);
    DRM_DWORD            cbSignature    = DRM_ECC160_SIGNATURE_LEN;
    DRM_RESULT           dr             = DRM_SUCCESS;
    DRM_DWORD            cchSignature   = NO_OF(rgchSignature);
    CERT                 cert           = { 0 };
    DRM_CRYPTO_CONTEXT  *pContextCRYP   = NULL;


    /* allocate a crypto context */
    ChkMem( pContextCRYP = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc( SIZEOF(DRM_CRYPTO_CONTEXT) ) );
    ZEROMEM( pContextCRYP, SIZEOF(DRM_CRYPTO_CONTEXT) );

    /* sign the <DATA> section */
    ChkDR (DRM_PK_Sign (pContextCRYP,
                        eDRM_ECC_P160,
                        DRM_ECC160_PRIVATE_KEY_LEN,
                        ( const DRM_BYTE * )&g_privkeyMeterCert,
                        cbData,
                        pbData,
                        &cbSignature,
                        rgbSignature));

    /* verify the signature */
    ChkDR( DRM_PK_Verify( pContextCRYP,
                          eDRM_ECC_P160,
                          DRM_ECC160_PUBLIC_KEY_LEN,
                          ( const DRM_BYTE * )&g_pubkeyMeteringCertVerify,
                          cbData,
                          pbData,
                          DRM_ECC160_SIGNATURE_LEN,
                          rgbSignature) );

    /* verify the certificate chain */
    ChkDR (DRM_B64_DecodeW   (&g_dstrCertMetercert, &cbCert, (DRM_BYTE*) &cert, 0));
    ChkDR (DRM_UTL_CheckCertNoCache (&cert,
                                     &g_pubkeyRootMeteringCert,
                                      FALSE,
                                      pContextCRYP,
                                      NULL ));

    /* encode and cache the signature */
    ChkDR (DRM_B64_EncodeW(rgbSignature,
                           SIZEOF(rgbSignature),
                           rgchSignature,
                          &cchSignature,
                           0));

    ChkDR( DRM_STR_StringCchCopyNW(f_pwszSignature, f_cchSignature, rgchSignature, cchSignature) );

ErrorExit:
    SAFE_OEM_FREE(pContextCRYP);
    return dr;
}

/*******************************************************************
 *                  PRIVATE FUNCTION WriteMeterCert
 *
 * purpose: Write the XML with signature to the output file
 *******************************************************************/

static DRM_RESULT WriteMeterCert(       OEM_FILEHDL         f_hfile,
                                  const DRM_CONST_STRING   *f_pdstrData,
                                  const DRM_WCHAR   *const  f_pwszSignature,
                                        _XMBContext *const  pXmlContext,
                                        DRM_DWORD           cbOut)
{
    DRM_DWORD        cbWrite       = 0,
                     cbWritten     = 0;
    DRM_WCHAR       *pwszOut       = NULL;
    DRM_STRING       strXML        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSignature = EMPTY_DRM_STRING;
    DRM_RESULT       dr            = DRM_SUCCESS;

    ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen (f_pwszSignature), &dstrSignature.cchString ) );
    dstrSignature.pwszString =                f_pwszSignature;

    ChkDR (DRM_XMB_CreateDocument (cbOut,  pXmlContext,                    &g_dstrTagMeterCert));                                          /* <METERCERT version="1.0"> */
    ChkDR (DRM_XMB_AddAttribute   (pXmlContext, &g_dstrAttributeVersion,   &g_dstrVersion_1_0));

    /*  <TAG label="text">data</TAG>     <TAG>                      data            attr label            attr text                      XML */

    ChkDR (DRM_XMB_AppendNode   (pXmlContext,                             f_pdstrData));                                                                               /*   <DATA>base64</DATA>*/

    ChkDR (DRM_XMB_WriteTag     (pXmlContext, &g_dstrTagCertificateChain, NULL,                NULL, NULL, wttOpen));   /*   <CERTIFICATECHAIN> */
    ChkDR (DRM_XMB_WriteTag     (pXmlContext, &g_dstrTagCertificate,     &g_dstrCertMetercert, NULL, NULL, wttClosed)); /*      <CERTIFICATE>base64</CERTIFICATE> */
    ChkDR (DRM_XMB_CloseCurrNode(pXmlContext,  NULL));                                                                                   /*   </CERTIFICATECHAIN> */

    ChkDR (DRM_XMB_WriteTag     (pXmlContext, &g_dstrTagSignature, NULL,          NULL,                 NULL,        wttOpen));   /*   <SIGNATURE> */
    ChkDR (DRM_XMB_WriteTag     (pXmlContext, &g_dstrTagHashAlg,   NULL,         &g_dstrAttributeType, &g_dstrSHA,   wttClosed)); /*      <HASHALGORITHM type="SHA"></HASHALGORITHM> */
    ChkDR (DRM_XMB_WriteTag     (pXmlContext, &g_dstrTagSignAlg,   NULL,         &g_dstrAttributeType, &g_dstrMSDRM, wttClosed)); /*      <SIGNALGORITHM type="MSDRM"></SIGNALGORITHM> */
    ChkDR (DRM_XMB_WriteTag     (pXmlContext, &g_dstrTagValue,    &dstrSignature, NULL,                 NULL,        wttClosed)); /*      <VALUE>base64</VALUE> */
    ChkDR (DRM_XMB_CloseCurrNode(pXmlContext,  NULL));                                                                                   /*   </SIGNATURE> */
    ChkDR (DRM_XMB_CloseDocument(pXmlContext, &strXML));                                                                                 /* </METERCERT> */

    pwszOut = (DRM_WCHAR *) pXmlContext;
    cbWrite = strXML.cchString * SIZEOF (DRM_WCHAR);

    DRM_STR_StringCchCopyNW (pwszOut,
                        cbOut / SIZEOF(DRM_WCHAR),    /* cannot use the NO_OF(pwszOut) macro here */
                        strXML.pwszString,
                        strXML.cchString);

    ChkDR (Oem_File_Write (f_hfile, (DRM_BYTE*)g_rgbFFFE, SIZEOF(g_rgbFFFE), &cbWritten));

    ChkDR (Oem_File_Write (f_hfile, pwszOut, cbWrite, &cbWritten));

ErrorExit:
    return dr;
}

/*******************************************************************
 *              PRIVATE FUNCTION ParseAndSignMeterCert
 *
 * purpose: high-level driver of this tool.
 *          opens the files and calls next-lower-level functions
 *          to verify, sign, and write the metering certificate
 *******************************************************************/
#define DATA_LENGTH    4096
static DRM_RESULT ParseAndSignMeterCert (const DRM_WCHAR *f_pszFileInput,
                                         const DRM_WCHAR *f_pszFileOutput)
{
    DRM_RESULT        dr       = DRM_SUCCESS;
    OEM_FILEHDL       hfileIn  = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL       hfileOut = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR         rgchSignature [CCH_BASE64_EQUIV (DRM_ECC160_SIGNATURE_LEN) + 1];
    DRM_WCHAR        *pwszData = NULL;
    DRM_WCHAR        *pwszOut  = NULL;
    DRM_DWORD         cbFile   = 0;
    DRM_DWORD         cbRead   = 0;
    DRM_BYTE         *pbFile   = NULL;
    DRM_CONST_STRING  dstrXML  = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrData = EMPTY_DRM_STRING;

    /* allocate the "Data" and "Out" arrays */
    ChkMem( pwszData = (DRM_WCHAR *)Oem_MemAlloc(DATA_LENGTH * SIZEOF(DRM_WCHAR)) );
    ChkMem( pwszOut  = (DRM_WCHAR *)Oem_MemAlloc(DATA_LENGTH * SIZEOF(DRM_WCHAR) * 2) );

    hfileIn = Oem_File_Open(NULL,
                           f_pszFileInput,
                           OEM_GENERIC_READ,
                           OEM_FILE_SHARE_NONE,
                           OEM_OPEN_EXISTING,
                           OEM_ATTRIBUTE_NORMAL);

    if (hfileIn == OEM_INVALID_HANDLE_VALUE)
    {
        printf("error opening input file %S", f_pszFileInput);
        ChkDR(DRM_E_FILENOTFOUND);
    }

    hfileOut = Oem_File_Open(NULL,
                            f_pszFileOutput,
                            OEM_GENERIC_WRITE,
                            OEM_FILE_SHARE_NONE,
                            OEM_CREATE_ALWAYS,
                            OEM_ATTRIBUTE_NORMAL);


    if (hfileIn == OEM_INVALID_HANDLE_VALUE)
    {
        printf("error opening output file %S", f_pszFileOutput);
        ChkDR(DRM_E_FILENOTFOUND);
    }

    dstrData.pwszString = pwszData;
    dstrData.cchString  = DATA_LENGTH;   /* cannot use the NO_OF(pwszData) macro here */

    if (Oem_File_GetSize (hfileIn, &cbFile) == FALSE)
    {
        ChkDR (DRM_E_FILE_READ_ERROR);
    }

    cbFile += SIZEOF(DRM_WCHAR);

    ChkMem (pbFile = (DRM_BYTE *)Oem_MemAlloc (cbFile));
    ChkDR  (Oem_File_Read (hfileIn, pbFile, cbFile, &cbRead));

    DSTR_FROM_PB(&dstrXML, pbFile, cbFile);

    ChkDR( ParseMeterCert( &dstrXML, &dstrData ) );
    ChkDR( SignMeterCert ( &dstrData, rgchSignature, NO_OF( rgchSignature ) ) );
    ChkDR( WriteMeterCert( hfileOut,
                          &dstrData,
                           rgchSignature,
            (_XMBContext *)pwszOut,
                           (DATA_LENGTH * SIZEOF(DRM_WCHAR) * 2)) );  /* SIZEOF(pwszOut) */

    printf ("%S", pwszOut);

ErrorExit:

    SAFE_OEM_FREE(pbFile);
    SAFE_OEM_FREE(pwszData);
    SAFE_OEM_FREE(pwszOut);

    if (hfileIn != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close (hfileIn);
    }

    if (hfileOut != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close (hfileOut);
    }

    return dr;
}

/******************************************************************************
** Print a command menu to console
*******************************************************************************/

static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf ("Syntax: %S [-?] -i:InputFile -o:OutputFile \n", pwszAppName);
}

/*******************************************************************
 *                      PUBLIC FUNCTION DRM_Main
 *
 * purpose: entrypoint; validates args and calls driver
 *******************************************************************/

DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR **argv )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CONST_STRING dstrOutput = EMPTY_DRM_STRING,
                     dstrInput  = EMPTY_DRM_STRING;
    DRM_LONG         iArg       =   0;

    PrintCopyrightInfo( "DRM Metering Certificate Signing Tool" );

    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    if (argc == 3)
    {
        ChkArg( argv != NULL );
        for (iArg = 1;
             iArg < argc;
             iArg++)
        {
            DRM_WCHAR        chOption;
            DRM_CONST_STRING dstrParam;

            if  (DRM_CMD_ParseCmdLine(argv [iArg], &chOption, &dstrParam, NULL))
            {
                chOption = DRMCRT_towlower (chOption);

                switch (chOption)
                {
                    case WCHAR_CAST('i'):
                        ASSIGN_DRM_STRING(dstrInput, dstrParam);
                        break;

                    case WCHAR_CAST('o'):
                        ASSIGN_DRM_STRING(dstrOutput, dstrParam);
                        break;

                    case WCHAR_CAST('?'):
                        goto _PrintUsage;

                    default:
                        dr = DRM_E_INVALID_COMMAND_LINE;
                        goto _PrintUsage;
                }
            }
            else
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        } /* end for args */

        if (dstrOutput.cchString == 0)
        {
            /* argument not supplied */

            printf ("Output filename not supplied.\n");
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        if (dstrInput.cchString > 0)
        {
            /* met all requirements */

            ChkDR( ParseAndSignMeterCert( dstrInput .pwszString,
                                          dstrOutput.pwszString ) );
        }
        else
        {
            /* argument not supplied */

            printf ("Input filename not supplied.\n");
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }
    else
    {
        /* argument not supplied */
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

ErrorExit:
    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success!\r\n" );
    }
    else
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }
    return dr;

_PrintUsage:
    PrintUsage (argv == NULL ? &g_wchNull : argv [0]);
    return dr;
}
