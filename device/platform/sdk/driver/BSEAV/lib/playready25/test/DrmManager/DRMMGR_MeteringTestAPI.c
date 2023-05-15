/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <drmxmlparser.h>
#include <drmmetercertstore.h>
#include <drmutf.h>
#include <drmmeterapi.h>
#include <drmmeterimp.h>
#include <drmmeterconstants.h>
#include <drmmathsafe.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

#define MINIMUM_METER_RESP_SIZE 25

static DRM_RESULT _GetMeteringDataAsXML(
          _XMBContext      *pbData,
          DRM_DWORD         cbData,
    const PRIVKEY          *pPrivKey,
          DRM_CONST_STRING *pretXML )
{
    DRM_RESULT          dr               = DRM_SUCCESS;
    DRM_CRYPTO_CONTEXT *pcontextCrypto   = NULL;
    DRM_CONST_STRING    dstrXML          = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrListTop      = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrListData     = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrRecords      = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrMID          = EMPTY_DRM_STRING;

    ChkMem (pcontextCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc (sizeof (DRM_CRYPTO_CONTEXT)));
    ZEROMEM( pcontextCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    DSTR_FROM_PB(&dstrXML, pbData, cbData);

    ChkDR(DRM_XML_GetNode(&dstrXML, &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop));
    ChkDR(DRM_XML_GetNode(&dstrListTop, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrListData));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagMID, NULL, NULL, 0, NULL, &dstrMID));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagRecords, NULL, NULL, 0, NULL, &dstrRecords));

    ChkDR (DRM_XML_GetAndDecryptNode (&dstrListData,
                                     &g_dstrTagRecords,
                                      NULL,
                                      NULL,
                                      0,
                                      pcontextCrypto,
                                      pPrivKey,
                                      NULL,
                                      &dstrRecords));

    pretXML->pwszString = dstrXML.pwszString;
    pretXML->cchString = dstrXML.cchString;

ErrorExit:

    SAFE_OEM_FREE (pcontextCrypto);

    return dr;
}

static DRM_DWORD _RequiredCharsForResponse(
    const DRM_CONST_STRING *pdstrRecords,
          DRM_BOOL          fPartialData,
          DRM_DWORD        *pcchKIDs)
{
    DRM_DWORD  cchRequired = MINIMUM_METER_RESP_SIZE;
    DRM_DWORD  iKid        = 0;
    DRM_RESULT dr          = DRM_SUCCESS;

    ChkArg (pdstrRecords != NULL);
    ChkArg (pcchKIDs     != NULL);

    *pcchKIDs = 0;

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagMetering.cchString,
                                               0,
                                               g_dstrAttributeType.cchString,
                                               g_dstrResponse.cchString);

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagData.cchString,
                                               0,
                                               0,
                                               0);

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagMID.cchString,
                                               CCH_BASE64_EQUIV (SIZEOF (DRM_MID)),
                                               0,
                                               0);

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagTID.cchString,
                                               CCH_BASE64_EQUIV (SIZEOF (DRM_TID)),
                                               0,
                                               0);

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagRecords.cchString,
                                               0,
                                               0,
                                               0);

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagCmd.cchString,
                                               g_dstrReset.cchString,
                                               0,
                                               0);

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagHash.cchString,
                                               0,
                                               0,
                                               0);

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagHashAlg.cchString,
                                               0,
                                               g_dstrAttributeType.cchString,
                                               g_dstrHMAC.cchString);

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagValue.cchString,
                                               CCH_BASE64_EQUIV (DRM_SHA1_DIGEST_LEN),
                                               0,
                                               0);

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagKID.cchString,
                                               0,
                                               0,
                                               0);

    if (fPartialData)
    {
        cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagPartialData.cchString,
                                                   g_dstr1.cchString,
                                                   0,
                                                   0);
    }

    while (TRUE)
    {
        DRM_CONST_STRING dstrKIDCurr = EMPTY_DRM_STRING;

        if (DRM_SUCCEEDED (DRM_XML_GetNode (pdstrRecords, &g_dstrTagKID, NULL, NULL, iKid++, NULL, &dstrKIDCurr)))
        {
            *pcchKIDs += DRM_XMB_RequiredCharsForTag(g_dstrTagKID.cchString,
                                                     0,
                                                     g_dstrLabelValue.cchString,
                                                     CCH_BASE64_EQUIV (SIZEOF (DRM_KID)));
        }
        else
        {
            break;
        }
    }

    cchRequired += CCH_BASE64_EQUIV ((*pcchKIDs + DRM_ECC160_CIPHERTEXT_LEN) * SIZEOF (DRM_WCHAR));
    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagMSDRMSignature.cchString,
                                               CCH_BASE64_EQUIV (DRM_ECC160_SIGNATURE_LEN),
                                               0,
                                               0);


ErrorExit:
    return cchRequired;
}

/*******************************************************************
*                  PRIVATE FUNCTION WriteMeterResponse
*
* purpose: Write the XML with signature to the output file
*******************************************************************/

static DRM_RESULT WriteMeterResponse(
    const DRM_CONST_STRING *pdstrFileResponse,
    const DRM_CONST_STRING *pdstrMID,
    const DRM_CONST_STRING *pdstrTID,
    const DRM_CONST_STRING *pdstrRecords,
    const PUBKEY           *ppubkeEncrypt,
          PRIVKEY          *pSignKey)
{
    DRM_BOOL          fPartialData = 0;
    OEM_FILEHDL       hFileOut = OEM_INVALID_HANDLE_VALUE;
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_DWORD        cbOut      = 0;
    DRM_DWORD        cbWritten  = 0;
    DRM_DWORD        cchKIDs    = 0;
    DRM_DWORD        iKID       = 0;
    DRM_STRING       strXML     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKID    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKIDB64 = EMPTY_DRM_STRING;
    DRM_STRING       dstrClose  = EMPTY_DRM_STRING;
    _XMBContext     *pbOut      = NULL;
    DRM_WCHAR       *pwszOut    = NULL;
    DRM_CRYPTO_CONTEXT *pcontextCrypto   = NULL;


    hFileOut = Oem_File_Open(NULL,
                                pdstrFileResponse->pwszString,
                                OEM_GENERIC_WRITE,
                                OEM_FILE_SHARE_NONE,
                                OEM_CREATE_ALWAYS,
                                OEM_ATTRIBUTE_NORMAL);
    if (hFileOut == OEM_INVALID_HANDLE_VALUE)
        ChkDR(DRM_E_FILE_WRITE_ERROR);

    ChkMem(pcontextCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc(SIZEOF (DRM_CRYPTO_CONTEXT)));
    ZEROMEM( pcontextCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    cbOut     =  SIZEOF (DRM_WCHAR) * _RequiredCharsForResponse(pdstrRecords, fPartialData, &cchKIDs) ;

    if (cchKIDs ==0)
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    ChkMem(pbOut = (_XMBContext *)Oem_MemAlloc(cbOut));
    ZEROMEM( pbOut, cbOut );

    ChkDR(DRM_XMB_CreateDocument(cbOut,  pbOut,               &g_dstrTagMetering));              /* <METERDATA type="response"> */
    ChkDR(DRM_XMB_AddAttribute  (pbOut, &g_dstrAttributeType, &g_dstrResponse));

    ChkDR(DRM_XMB_WriteTag(pbOut, &g_dstrTagData, NULL,        NULL, NULL, wttOpen));   /*   <DATA> */
    ChkDR(DRM_XMB_WriteTag(pbOut, &g_dstrTagMID,  pdstrMID,    NULL, NULL, wttClosed)); /*     <MID>base64</MID> */
    ChkDR(DRM_XMB_WriteTag(pbOut, &g_dstrTagTID,  pdstrTID,    NULL, NULL, wttClosed)); /*     <TID>base64</TID> */
    ChkDR(DRM_XMB_WriteTag(pbOut, &g_dstrTagCmd, &g_dstrReset, NULL, NULL, wttClosed)); /*     <COMMAND>RESET</COMMAND> */
    ChkDR(DRM_XMB_WriteTag(pbOut, &g_dstrTagRecords, NULL,     NULL, NULL, wttOpen)); /*     <RECORDS> */

    while (TRUE)
    {
        dr = DRM_XML_GetNode(pdstrRecords, &g_dstrTagKID, NULL, NULL, iKID++, &dstrKID, NULL);

        if (DRM_SUCCEEDED (dr))
        {
            /* extract the value="KIDasbase64" from the challenge */

            ChkDR(DRM_XML_GetNodeAttribute(&dstrKID, &g_dstrLabelValue, &dstrKIDB64));

            /* write the KID entries in the response */

            ChkDR(DRM_XMB_WriteTag(pbOut, &g_dstrTagKID, NULL, &g_dstrLabelValue, &dstrKIDB64, wttClosed));   /*   <KID value="base64"/> */
        }
        else if (dr == DRM_E_NOMORE
              || dr == DRM_E_XMLNOTFOUND)
        {
            dr = DRM_SUCCESS;
            break;
        }
        else
        {
            ChkDR(dr);
        }
    } /* end while more KIDs */

    ChkDR(DRM_XMB_EncryptAndCloseCurrNode(pbOut,
                                           pcontextCrypto,
                                           NULL,
                                           ppubkeEncrypt,
                                           NULL)); /* </RECORDS> */

    if (fPartialData)
    {                                                                 /*     <PARTIALDATA>1</PARTIALDATA> */
        ChkDR(DRM_XMB_WriteTag(pbOut, &g_dstrTagPartialData, &g_dstr1, NULL, NULL, wttClosed));
    }

    /*Close the data node */
    ChkDR(DRM_XMB_CloseCurrNode(pbOut, &dstrClose));
    ChkDR(DRM_XMB_CloseDocument(pbOut, &strXML));  /* </METERDATA> */

    MEMMOVE (pbOut,
             PB_DSTR(&strXML),
             CB_DSTR(&strXML));

    pwszOut = (DRM_WCHAR *) pbOut;
    pwszOut [strXML.cchString] = g_wchNull;

    dr = Oem_File_Write(hFileOut, (DRM_VOID *) pwszOut, CB_DSTR(&strXML), &cbWritten)
                ? DRM_SUCCESS
                : DRM_E_FILE_WRITE_ERROR;

ErrorExit:

    SAFE_OEM_FREE(pbOut);

    SAFE_OEM_FREE(pcontextCrypto);

    if (hFileOut != OEM_INVALID_HANDLE_VALUE)
        Oem_File_Close(hFileOut);
    return dr;
}

static DRM_RESULT _GetMeterDataFromFile( const DRM_WCHAR *pwchInResponseFile, DRM_WCHAR **ppwchMeterData )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    OEM_FILEHDL hfile       = OEM_INVALID_HANDLE_VALUE;
    DRM_CHAR   *pch         = NULL;
    DRM_DWORD   cch         = 0;
    DRM_WCHAR  *pwchFile    = NULL;
    DRM_DWORD   cbFile      = 0;
    DRM_DWORD   cbRead      = 0;

    *ppwchMeterData = NULL;

    if( pwchInResponseFile == NULL )
    {
        Log( "Trace", "Input file NULL.\n" );
        ChkArg( FALSE );
    }

    cch = (DRM_DWORD)( DRMCRT_wcslen( pwchInResponseFile ) + 1 );
    ChkMem( pch = (DRM_CHAR*)Oem_MemAlloc( SIZEOF(DRM_CHAR) * cch ) );
    ZEROMEM( pch, SIZEOF(DRM_CHAR) * cch );
    DRM_UTL_DemoteUNICODEtoASCII( pwchInResponseFile, pch, cch );

    hfile = Oem_File_Open(
        NULL,
        pwchInResponseFile,
        OEM_GENERIC_READ,
        OEM_FILE_SHARE_NONE,
        OEM_OPEN_EXISTING,
        OEM_ATTRIBUTE_NORMAL );
    if( hfile == OEM_INVALID_HANDLE_VALUE )
    {
        /* file could not be opened */
        Log( "Trace", "Input file %s could not open.\n", pch );
        ChkDR( DRM_E_FILENOTFOUND );
    }

    if( !Oem_File_GetSize( hfile, &cbFile ) )
    {
        Log( "Trace", "Unable to get size for file.\n" );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    pwchFile = (DRM_WCHAR*)Oem_MemAlloc( cbFile + 2 );
    if( pwchFile == NULL )
    {
        Log( "Trace", "Unable to allocate memory to read file.\n" );
        ChkMem( NULL );
    }

    MEMSET( pwchFile, 0, cbFile + 2 );

    if( !Oem_File_Read( hfile, pwchFile, cbFile, &cbRead ) )
    {
        Log( "Trace", "Unable to input file read file %s.\n", pch );
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    *ppwchMeterData = pwchFile;
    pwchFile        = NULL;

ErrorExit:
    if( hfile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfile );
    }
    SAFE_OEM_FREE( pwchFile );
    SAFE_OEM_FREE( pch );
    return dr;
}

static DRM_RESULT _SignResponse(
    const DRM_CONST_STRING *pdstrResponse,
    const DRM_CONST_STRING *pdstrCert,
          DRM_CONST_STRING *pdstrOut,
    const PRIVKEY          *pSignKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContext     *pbOut             = NULL;
    DRM_WCHAR       *pwszOut           = NULL;
    DRM_CRYPTO_CONTEXT *pcontextCrypto = NULL;
    DRM_DWORD        cbOut             = 0;
    DRM_CONST_STRING dstrSign          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrXML           = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListTop       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataNode      = EMPTY_DRM_STRING;
    DRM_STRING       strXML            = EMPTY_DRM_STRING;
    DRM_DWORD        cbBuffer          = 0;
    DRM_DWORD        cbSignature       = DRM_ECC160_SIGNATURE_LEN;

    ChkMem(pcontextCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc(SIZEOF (DRM_CRYPTO_CONTEXT)));
    ZEROMEM( pcontextCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    cbOut = (SIZEOF (DRM_WCHAR) * pdstrResponse->cchString);
    ChkOverflow( cbOut, pdstrResponse->cchString );
    ChkOverflow( cbOut * 3, cbOut );
    cbOut *= 3;
    cbOut += SIZEOF (DRM_WCHAR) * pdstrCert->cchString;
    ChkOverflow( cbOut, pdstrCert->cchString );
    cbOut += 1000;
    ChkOverflow( cbOut, 1000 );

    ChkMem(pbOut = (_XMBContext *)Oem_MemAlloc(cbOut));
    ZEROMEM( pbOut, cbOut );

    dstrXML.pwszString = (const DRM_WCHAR*)pbOut;
    dstrXML.cchString = cbOut;

    ChkDR(DRM_XML_GetNode(pdstrResponse, &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop));
    ChkDR(DRM_XML_GetNode(&dstrListTop, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrDataNode))

    ChkDR(DRM_XMB_CreateDocument(cbOut,  pbOut,               &g_dstrTagMetering));              /* <METERDATA type="response"> */
    ChkDR(DRM_XMB_AddAttribute  (pbOut, &g_dstrAttributeType, &g_dstrResponse));
    /*Sign it with Signing key*/

    ChkDR(DRM_XMB_OpenNode(pbOut,&g_dstrTagData));
    ChkDR(DRM_XMB_AddData(pbOut, &dstrDataNode));
    ChkDR(DRM_XMB_CloseCurrNode(pbOut, &strXML));
    ChkDR(DRM_PK_Sign(pcontextCrypto->rgbCryptoContext,
                      eDRM_ECC_P160,
                      DRM_ECC160_PRIVATE_KEY_LEN,
                      ( const DRM_BYTE * )pSignKey,
                      CB_DSTR(&strXML),
                      (DRM_BYTE *)strXML.pwszString,
                      &cbSignature,
                      pcontextCrypto->union_cert.cert.sign));

    /*Base64 encode signature*/
    cbBuffer=DRM_ECC160_SIGNATURE_B64LEN;
    ChkDR(DRM_B64_EncodeW((DRM_BYTE *)pcontextCrypto->union_cert.cert.sign,
                           cbSignature,
              (DRM_WCHAR *)pcontextCrypto->rgbCryptoContext,
                          &cbBuffer,
                          0));
    /*Write to tag*/

    DSTR_FROM_PB(&dstrSign, pcontextCrypto->rgbCryptoContext, cbBuffer*SIZEOF(DRM_WCHAR));

    ChkDR(DRM_XMB_WriteTag(pbOut, &g_dstrTagMSDRMSignature, &dstrSign, NULL, NULL, wttClosed));

    ChkDR(DRM_XMB_AppendNode(pbOut, pdstrCert));
    ChkDR(DRM_XMB_CloseDocument(pbOut, &strXML));  /* </METERDATA> */

    MEMMOVE (pbOut,
             PB_DSTR(&strXML),
             CB_DSTR(&strXML));

    pwszOut = (DRM_WCHAR *) pbOut;
    pwszOut [strXML.cchString] = g_wchNull;

    pdstrOut->pwszString = pwszOut;
    pdstrOut->cchString = strXML.cchString;

ErrorExit:

    SAFE_OEM_FREE( pcontextCrypto );

    return dr;
}

/*****************************************************************************
** Function:    TestManagerGenerateMeterChallenge
**
** Synopsis:
**
** Arguments:   argv[0] - Metering Certificate file
**              argv[1] - file private key
**              argv[2] - response file [optional]
**              argv[3] - expected Mid [optional]
**              argv[4] - expected KID [optional]
**              argv[5] - expected Action [optional]
**              argv[6] - expected value [optional]
**              argv[7] - expected KID [optional]
**              argv[8] - expected Action [optional]
**              argv[9] - expected value [optional]
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGenerateMeterChallenge( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                  = DRM_SUCCESS;

    DRM_APP_CONTEXT  *poManagerContext   = NULL;

    DRM_CONST_STRING dstrMeterCertInFile = EMPTY_DRM_STRING;

    _XMBContext      *pbMeteringData     = NULL;
    DRM_DWORD        cbMeteringData      = 0;
    DRM_CONST_STRING dstrMeterCert       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMeteringXML     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKey         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrExpMID          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrExpKID          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrExpAction       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrExpValue        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFileOut         = EMPTY_DRM_STRING;

    DRM_DWORD        cbReqd              = 0;
    DRM_DWORD        index               = 0;
    DRM_DWORD        subindex            = 0;
    DRM_BOOL         bFound              = FALSE;
    DRM_CONST_STRING dstrListTop         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListData        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrRecords         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMID             = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKID             = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTID             = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKidData         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrAction          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrActionData      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrValue           = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFallbackCert    = EMPTY_DRM_STRING;
    DRM_WCHAR       *pwchMeterCert       = NULL;
    DRM_KID          expectedValue       = {0};
    DRM_KID          actualValue         = {0};
    DRM_DWORD        dwszIDSize          = DRM_ID_SIZE;

    ChkArg( argc > 1 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkDR( TST_UTL_MakePathString( NULL,
                                      argv[0],
                                     &dstrMeterCertInFile ) );

    ChkDR( MakeDRMString( &dstrPrivKey, argv[1] ) );

    ChkDR( DRM_B64_DecodeW( &dstrPrivKey,
                            &cbReqd,
                            NULL,
                            DRM_BASE64_DECODE_IN_PLACE ) );

    ChkDR( _GetMeterDataFromFile( dstrMeterCertInFile.pwszString, &pwchMeterCert ) );

    ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( pwchMeterCert ), &dstrMeterCert.cchString ) );
    dstrMeterCert.pwszString = pwchMeterCert;


    dr = Drm_Metering_GenerateChallenge( poManagerContext,
                                         PB_DSTR(&dstrMeterCert),
                                         CB_DSTR(&dstrMeterCert),
                                         NULL,
                                         0,
                                         NULL,
                                         NULL,
                                         NULL,
                                         &cbMeteringData );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        if( dr == DRM_SUCCESS )
        {
            ChkDR( DRM_E_LOGICERR );
        }
        ChkDR( dr );
    }

    ChkMem ( pbMeteringData = (_XMBContext*)Oem_MemAlloc( cbMeteringData ) );
    ZEROMEM( pbMeteringData, cbMeteringData );

    ChkDR( Drm_Metering_GenerateChallenge( poManagerContext,
                                           PB_DSTR(&dstrMeterCert),
                                           CB_DSTR(&dstrMeterCert),
                                           NULL,
                                           0,
                                           NULL,
                                           NULL,
                                           (DRM_BYTE*)pbMeteringData,
                                           &cbMeteringData ) );

    ChkDR( _GetMeteringDataAsXML( pbMeteringData, cbMeteringData, (PRIVKEY *)dstrPrivKey.pwszString, &dstrMeteringXML ) );

    ChkDR( DRM_XML_GetNode( &dstrMeteringXML, &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop ) );
    ChkDR( DRM_XML_GetNode( &dstrListTop, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrListData ) );
    ChkDR( DRM_XML_GetNode( &dstrListData, &g_dstrTagMID, NULL, NULL, 0, NULL, &dstrMID ) );
    ChkDR( DRM_XML_GetNode( &dstrListData, &g_dstrTagTID, NULL, NULL, 0, NULL, &dstrTID ) );
    ChkDR( DRM_XML_GetNode( &dstrListData, &g_dstrTagRecords, NULL, NULL, 0, NULL, &dstrRecords ) );

    if( argc < 3 )
    {
        /*
        ** expect no data
        */
        if( dstrRecords.cchString == 0 )
        {
            goto ErrorExit; /* success */
        }
        else
        {
            ChkDR( DRM_E_FAIL );
        }
    }
    else if( dstrRecords.cchString == 0 )
    {
        /*
        ** expected data
        */
        ChkDR( DRM_E_FAIL );
    }


    ChkDR( DRM_XML_GetNode( &dstrRecords, &g_dstrTagKID, NULL, NULL, subindex, &dstrKidData, &dstrKID ) );
    ChkDR( DRM_XML_GetNode( &dstrListTop,  &g_dstrTagCertificate, NULL, NULL, 0, NULL, &dstrFallbackCert ) );

    if( argc > 3 )
    {
        ChkArg( ( argc - 4 ) % 3 == 0 );    /* args after argv[3] come in triads */
        ChkDR( MakeDRMString( &dstrExpMID, argv[3] ) );

        ChkDR ( DRM_B64_DecodeW ( &dstrExpMID ,  &dwszIDSize , expectedValue.rgb , 0 ) );
        dwszIDSize = DRM_ID_SIZE;
        ChkDR ( DRM_B64_DecodeW ( &dstrMID ,  &dwszIDSize , actualValue.rgb , 0 ) );

        /*
        ** Verify MIDs
        */
        if( MEMCMP( expectedValue.rgb, actualValue.rgb, DRM_ID_SIZE ) !=0 )
        {
            ChkDR( DRM_E_METERING_MID_MISMATCH );
        }

        for( index = 4; index + 2 < (DRM_DWORD)argc; index += 3 )
        {
            ChkDR( MakeDRMString(&dstrExpKID, argv[index]) );

            bFound = FALSE;
            for( subindex = 0; !bFound; subindex++ )
            {
                ChkDR( DRM_XML_GetNode( &dstrRecords, &g_dstrTagKID, NULL, NULL, subindex, &dstrKidData, &dstrKID ) );
                ChkDR( DRM_XML_GetNodeAttribute( &dstrKidData, &g_dstrLabelValue, &dstrValue ) );

                ZEROMEM( expectedValue.rgb, DRM_ID_SIZE );
                ZEROMEM( actualValue.rgb, DRM_ID_SIZE );

                dwszIDSize = DRM_ID_SIZE;
                ChkDR( DRM_B64_DecodeW ( &dstrExpKID,  &dwszIDSize, expectedValue.rgb, 0 ) );
                dwszIDSize = DRM_ID_SIZE;
                ChkDR( DRM_B64_DecodeW ( &dstrValue,  &dwszIDSize, actualValue.rgb, 0 ) );

                if( MEMCMP( expectedValue.rgb, actualValue.rgb, DRM_ID_SIZE ) == 0 )
                {
                    bFound = TRUE;
                }
            }
            if( !bFound )    /* KID not found */
            {
                ChkDR( DRM_E_FAIL );
            }

            FREEDRMSTRING( dstrExpKID );

            bFound = FALSE;
            ChkDR( MakeDRMString( &dstrExpAction, argv[index+1] ) );
            for( subindex = 0; !bFound; subindex++ )
            {
                ChkDR( DRM_XML_GetNode( &dstrKID, &g_dstrTagAction, NULL, NULL, subindex, &dstrActionData, &dstrAction ) );
                ChkDR( DRM_XML_GetNodeAttribute( &dstrActionData, &g_dstrLabelValue, &dstrValue ) );
                if( DRM_UTL_DSTRStringsEqual( &dstrExpAction, &dstrValue ) )
                {
                    bFound = TRUE;
                }
            }
            if( !bFound )    /* Action not found */
            {
                ChkDR( DRM_E_FAIL );
            }

            FREEDRMSTRING( dstrExpAction );

            ChkDR( MakeDRMString( &dstrExpValue, argv[index+2] ) );
            if ( !DRM_UTL_DSTRStringsEqual( &dstrExpValue,&dstrAction ) )
            {
                ChkDR(DRM_E_FAIL);
            }

            FREEDRMSTRING( dstrExpValue );
        }
    }
    if ( argc > 2 && argv[2] != NULL )
    {
        PKCERT fallbackCert;
        DRM_DWORD cbB64Decode = 0;
        cbB64Decode = SIZEOF( PKCERT );
        ChkDR( DRM_B64_DecodeW( &dstrFallbackCert, &cbB64Decode, (DRM_BYTE *)&fallbackCert, 0 ) );

        ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrFileOut ) );

        WriteMeterResponse( &dstrFileOut, &dstrMID, &dstrTID, &dstrRecords, (PUBKEY *)&fallbackCert.pk.pk, (PRIVKEY*)dstrPrivKey.pwszString );
    }

ErrorExit:
    FREEDRMSTRING( dstrPrivKey );
    FREEDRMSTRING( dstrFileOut );
    FREEDRMSTRING( dstrMeterCertInFile );
    SAFE_OEM_FREE( pbMeteringData );
    FREEDRMSTRING( dstrExpMID );
    FREEDRMSTRING( dstrExpKID );
    FREEDRMSTRING( dstrExpAction );
    FREEDRMSTRING( dstrExpValue );
    SAFE_OEM_FREE( pwchMeterCert );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerProcessMeterResponse
**
** Synopsis:
**
** Arguments:   argv[0] - Metering Certificate file
**              argv[1] - file private key
**              argv[2] - response file
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerProcessMeterResponse( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                     = DRM_SUCCESS;
    DRM_DWORD        flagsResponse          = 0;
    DRM_CONST_STRING dstrMeterReponseInFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMeterCertInFile    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSignedReponse      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMeterCert          = EMPTY_DRM_STRING;
    DRM_WCHAR       *pwchMeterCert          = NULL;
    DRM_CONST_STRING dstrPrivKey            = EMPTY_DRM_STRING;
    DRM_DWORD        cbReqd                 = 0;
    DRM_CONST_STRING dstrMeterResp          = EMPTY_DRM_STRING;
    DRM_WCHAR       *pwchMeterResponse      = 0;
    DRM_APP_CONTEXT *poManagerContext       = NULL;

    ChkArg( argc > 2 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkDR( TST_UTL_MakePathString( NULL, argv[0], &dstrMeterCertInFile ) );

    ChkDR( _GetMeterDataFromFile( dstrMeterCertInFile.pwszString, &pwchMeterCert ) );

    ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( pwchMeterCert ), &dstrMeterCert.cchString ) );
    dstrMeterCert.pwszString = pwchMeterCert;

    ChkDR( MakeDRMString( &dstrPrivKey, argv[1] ) );

    ChkDR( DRM_B64_DecodeW( &dstrPrivKey,
                         &cbReqd,
                          NULL,
                          DRM_BASE64_DECODE_IN_PLACE ) );


    ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrMeterReponseInFile ) );

    ChkDR( _GetMeterDataFromFile( dstrMeterReponseInFile.pwszString, &pwchMeterResponse ) );
    ChkArg( pwchMeterResponse );

    ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( pwchMeterResponse ), &dstrMeterResp.cchString ) );
    dstrMeterResp.pwszString = pwchMeterResponse;

    ChkDR( _SignResponse( &dstrMeterResp, &dstrMeterCert, &dstrSignedReponse, (PRIVKEY*)dstrPrivKey.pwszString ) );

    ChkDR  ( Drm_Metering_ProcessResponse ( poManagerContext,
                             (DRM_BYTE *) PB_DSTR( &dstrSignedReponse ),
                                          CB_DSTR( &dstrSignedReponse ),
                                          &flagsResponse ) );

ErrorExit:
    FREEDRMSTRING( dstrPrivKey );
    FREEDRMSTRING( dstrMeterCertInFile );
    FREEDRMSTRING( dstrMeterReponseInFile );
    SAFE_OEM_FREE( pwchMeterResponse );
    SAFE_OEM_FREE( pwchMeterCert );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerAddManyMeteringData
**
** Synopsis:
**
** Arguments:   argv[0] - MID
**              argv[1] - Number of KIDs to add.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerAddManyMeteringData( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                dr                = DRM_SUCCESS;
    DRM_METERING_CONTEXT      meterStore        = {0};
    DRM_BYTE                  rgbPasswordSST [__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };
    DRM_CONST_STRING          dstrMID           = EMPTY_DRM_STRING;
    DRM_CONST_STRING          dstrKID           = EMPTY_DRM_STRING;
    DRM_CONST_STRING          dstrAction        = EMPTY_DRM_STRING;
    DRM_APP_CONTEXT_INTERNAL *pcontext          = NULL;
    DRM_BB_CONTEXT           *pcontextBlackBox  = NULL;
    DRM_DST                  *pDatastore        = NULL;
    DRM_LONG                  index             = 0;
    DRM_LONG                  kidCount          = 0;
    DRM_MID                   mid               = {0};
    DRM_KID                   kid               = {0};
    DRM_DWORD                 cbMID             = SIZEOF (mid);
    TOKEN                     token             = {0};
    DRM_BOOL                  bStoreOpen        = FALSE;
    DRM_WCHAR                 kidbuf[CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ) + 1] = {0};
    DRM_WCHAR                 buff[SIZEOF( DRM_KID )] = {0};
    DRM_DWORD                 cch               = 0;

    ChkArg( argc > 1 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    GET_SHARED_APP_CONTEXT( pcontext );

    pcontextBlackBox  = &pcontext->oBlackBoxContext;
    pDatastore        = &pcontext->oDatastoreHDS;

    ChkDR( MakeDRMString( &dstrMID, argv[0] ) );
    ChkDR( DRM_BBX_HashValue( (DRM_BYTE*)dstrMID.pwszString, dstrMID.cchString * 2, rgbPasswordSST, pcontextBlackBox ) );

    ChkDR( DRM_B64_DecodeW( &dstrMID, &cbMID, (DRM_BYTE  *)&mid, 0 ) );

    kidCount = atoi( argv[1] );
    for ( index = 0; index < kidCount; index++ )
    {
        DRM_LONG subIndex = 0;
        MEMSET( buff, 0, SIZEOF( buff ) );
        for ( subIndex = 0; subIndex < 12; subIndex++ )
        {
           buff[subIndex] = (DRM_WCHAR)rand();
        }

        cch = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
        DRM_B64_EncodeW( (DRM_BYTE*)buff, SIZEOF( DRM_KID ), kidbuf, &cch, 0 );
        dstrKID.pwszString = kidbuf;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( kidbuf ), &dstrKID.cchString ) );

        cch = SIZEOF( DRM_KID );

        ChkDR( DRM_B64_DecodeW( &dstrKID, &cch, (DRM_BYTE  *)&kid, 0 ) );
        ChkDR( MakeDRMString( &dstrAction, "Play" ) );

        ChkDR( DRM_MTR_OpenId( &meterStore, &mid, &kid, rgbPasswordSST, pDatastore ) );
        bStoreOpen = TRUE;

        /*
        ** zero out any data
        */
        ChkDR( DRM_SST_GetTokenValue( &meterStore, &dstrAction, &token ) );
        token.val.lValue = 1;
        token.TokenType  = TOKEN_LONG;
        ChkDR( DRM_SST_SetTokenValue( &meterStore, &dstrAction , &token ) );

        DRM_MTR_CloseContext( &meterStore, pDatastore );
        bStoreOpen = FALSE;
    }

ErrorExit:
    if ( bStoreOpen )
        DRM_MTR_CloseContext( &meterStore, pDatastore );

    return dr;
}


/*********************************************************
**
** Function: TestManagerGetLicenseMID
**
** Synopsis: This tests the function Drm_License_GetProperty.  It
** takes as arguments all MIDs expected to be in the license
** chain refered to in the DRM_APP_CONTEXT.  This should
** be called after successfully calling TestManagerBind.
**
** Arguments:
**   argc                    -- must be at least equal to the
**                           number of MIDS, no greater than
**                           the number of MIDs + 3
**   argv[0]                 -- The number of MIDs to expect
**   argv[0< i <= NO_OF(rgMIDs)]   -- The expected MIDs, in the
                                   expected output order
**   argv[NO_OF(rgMIDs) + 1] -- optional flag indicating that
**                           a NULL context should be used
**                           (default is FALSE)
**   argv[NO_OF(rgMIDs) + 2] -- optional flag indicating that
**                           a NULL output buffer should be used
**                           (default is FALSE)
**   argv[NO_OF(rgMIDs) + 3] -- optional flag indicating that a
**                           NULL output buffer size should be used
**                           (default is FALSE)
**   argv[NO_OF(rgMIDs) + 4] -- optional size of output buffer to
**                           pass into the function
**                           (default can hold the max number of MIDS)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGetLicenseMID( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT             dr               = DRM_E_TEST_INCOMPLETE;
    DRM_LGP_MID_EXTRA_DATA rgMIDs           = {0};             /* The MID output buffer */
    DRM_DWORD              dwExpectedMIDs   = 0;               /* The expected number of MIDs */
    DRM_DWORD              cbMIDs           = SIZEOF(rgMIDs);  /* size of rgMIDs buffer */
    DRM_DWORD              i                = 0;               /* Counter used to iterate through MIDs */
    DRM_BOOL               fUseNullContext  = FALSE;           /* Flag indicating use of NULL context */
    DRM_BOOL               fUseNullBuffer   = FALSE;           /* Flag indicating use of NULL output buffer */
    DRM_BOOL               fUseNullSize     = FALSE;           /* Flag indicating use of NULL buffer size */
    DRM_DWORD              dwOutputMIDS     = 0;               /* Holds the number of MIDs found */
    DRM_APP_CONTEXT       *poManagerContext = NULL;

    ChkArg( argc >= 1 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    /*
    ** Parse the number of MIDs from input
    */
    ChkArg( argv[0] && IsDigitalOnlyString(argv[0]) );
    dwExpectedMIDs = (DRM_DWORD)atoi(argv[0]);
    ChkBOOL( (DRM_LONG)DRM_MAX_LICENSE_CHAIN_DEPTH >= dwExpectedMIDs, DRM_E_FAIL );

    /*
    ** Check for optional arguments
    */
    if ( argc > 1 + (DRM_LONG)dwExpectedMIDs )
    {
        /*
        ** Set NULL context flag
        */
        ChkDR( StringToBool( argv[1 + (DRM_LONG)dwExpectedMIDs], &fUseNullContext ) );
    }
    if ( argc > 2 + (DRM_LONG)dwExpectedMIDs )
    {
        /*
        ** Set NULL output buffer flag
        */
        ChkDR( StringToBool( argv[2 + (DRM_LONG)dwExpectedMIDs], &fUseNullBuffer ) );
    }
    if ( argc > 3 + (DRM_LONG)dwExpectedMIDs )
    {
        /*
        ** Set NULL buffer size flag
        */
        ChkDR( StringToBool(argv[3 + (DRM_LONG)dwExpectedMIDs], &fUseNullSize) );
    }
    if ( ( argc > 4 + (DRM_LONG)dwExpectedMIDs) && IsDigitalOnlyString( argv[4 + (DRM_LONG)dwExpectedMIDs] ) )
    {
        /*
        ** Set specified buffer size
        */
        cbMIDs = (DRM_DWORD)atoi( argv[3 + (DRM_LONG)dwExpectedMIDs] );
    }

PREFAST_PUSH_DISABLE_EXPLAINED( 26015, "Test conditions that may contradict SAL annotated API." );

    /*
    ** Call Drm_License_GetProperty with the specified arguments
    */
    ChkDR( Drm_License_GetProperty( fUseNullContext ? NULL : poManagerContext,
                                    DRM_LGP_MID,
                                    fUseNullBuffer ? NULL : (DRM_BYTE *)&rgMIDs,
                                    fUseNullSize ? NULL : &cbMIDs,
                                    &dwOutputMIDS ) );

PREFAST_POP;

    /*
    ** Check that the number of MIDs output matches expectation
    */
    ChkArg( dwExpectedMIDs == dwOutputMIDS );

    /*
    ** Check that each output MID matches expected output MID
    */
    for( i = 1; i <= dwExpectedMIDs; i++ )
    {
        ChkArg( argc > (long) i );
        ChkArg( argv[i] != NULL );

        ChkBOOL( strncmp( rgMIDs.szMID[i-1], argv[i], 24 ) == 0, DRM_E_TEST_MID_MISMATCH);
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerMeterCertInitEnum
**
** Synopsis:
**
** Arguments
**              argv[0] -- opt, if Null use NULL manager Context
**              argv[1] -- opt, if NULL use a NULL enum pointer
**
**              both use valid pointers if they are missing.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerMeterCertInitEnum( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr               = DRM_SUCCESS;
    DRM_APP_CONTEXT    *poManagerContext = NULL;
    DRM_METERCERT_ENUM *poMeterCertEnum  = NULL;

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    GET_SHARED_APP_CONTEXT( poManagerContext );
    MAP_TO_GLOBAL_TEST_VAR( poMeterCertEnum, METER_CERT_ENUM_CONTEXT_VAR );

    ChkDR( Drm_MeterCert_InitEnum( ( argc >= 1 && argv[0] == NULL ) ? NULL : poManagerContext,
                                   ( argc >= 2 && argv[1] == NULL ) ? NULL : poMeterCertEnum) );

ErrorExit:
    return dr;
}

#define DRM_MAX_TEST_METERCERT_SIZE 10240

/*****************************************************************************
** Function:    TestManagerMeterCertInitEnum
**
** Synopsis:
**
** Arguments:
**              argv[0] -- MID, can be NULL
**              argv[1] -- LAINFO
**              argv[2] -- opt, Cert
**              argv[3] -- opt, switch for LAINFO, see below
**              argv[4] -- opt, switch for Cert, see below
**              argv[5] -- opt, if NULL, null metercertenum
**              argv[6] -- opt, if TRUE then URL is V2 server
**
**              Keywords
**              NULL -- use a null pointer
**              BADSIZE -- use a buffer that is too small, only for argv[3] and argv[4]
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerMeterCertEnumNext( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT    dr              = DRM_SUCCESS;
    DRM_SUBSTRING dsstrMID        = EMPTY_DRM_SUBSTRING;
    DRM_MID       mid             = {0};
    DRM_DWORD     cbMID           = SIZEOF( mid.rgb );
    DRM_MID       midReturned     = {0};
    DRM_WCHAR     rgwcLAINFO[100] = {0};
    DRM_DWORD     cchLAINFO       = NO_OF( rgwcLAINFO );
    DRM_WCHAR    *pwszMeterCert   = NULL;
    DRM_DWORD     cchMeterCert    = DRM_MAX_TEST_METERCERT_SIZE;
    DRM_DWORD     cbMeterCert     = 0;
    DRM_BOOL      fIsV2           = FALSE;

    DRM_BOOL      fBadSizeLAINFO  = FALSE;
    DRM_BOOL      fBadSizeCert    = FALSE;
    DRM_BOOL      fNULLLAINFO     = FALSE;
    DRM_BOOL      fNULLCert       = FALSE;
    DRM_BOOL      fNULLSizeLAINFO = FALSE;
    DRM_BOOL      fNULLSizeCert   = FALSE;
    DRM_BOOL      fNULLmid        = FALSE;
    DRM_BOOL      fNULLEnum       = FALSE;
    DRM_METERCERT_ENUM *poMeterCertEnum  = NULL;

    ChkArg( argc    >= 2 );
    ChkArg( argc    <= 7 );
    ChkArg( argv[1] != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    MAP_TO_GLOBAL_TEST_VAR( poMeterCertEnum, METER_CERT_ENUM_CONTEXT_VAR );

PREFAST_PUSH_DISABLE_EXPLAINED( 22011, "Already checking integer overflow." );
    ChkOverflow( cchMeterCert * SIZEOF( DRM_WCHAR ), cchMeterCert );
    ChkMem( pwszMeterCert = ( DRM_WCHAR * )Oem_MemAlloc( cchMeterCert * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( ( DRM_BYTE * )pwszMeterCert, cchMeterCert * SIZEOF( DRM_WCHAR ) );
PREFAST_POP;

    if ( NULL == argv[0] )
    {
        fNULLmid = TRUE;
    }
    else
    {
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &dsstrMID.m_cch ) );
        dsstrMID.m_ich = 0;
        ChkDR( DRM_B64_DecodeA( argv[0], &dsstrMID, &cbMID, mid.rgb, 0 ) );

        MEMCPY( &midReturned, &mid, SIZEOF( DRM_MID ) );
    }

    if ( 4 <= argc )
    {
        if ( NULL == argv[3] )
        {
            fNULLLAINFO = TRUE;
        }
        else if ( 0 == strncmp( argv[3], "BADSIZE", 7 ) )
        {
            fBadSizeLAINFO = TRUE;
            cchLAINFO = 0;
        }
        else if ( 0 == strncmp( argv[3], "NULLSIZE", 8 ) )
        {
            fNULLLAINFO = TRUE;
            fNULLSizeLAINFO = TRUE;
        }
    }
    if ( 5 <= argc )
    {
        if ( NULL == argv[4] )
        {
            fNULLCert = TRUE;
        }
        else if ( 0 == strncmp( argv[4], "BADSIZE", 7 ) )
        {
            fBadSizeCert = TRUE;
            cchMeterCert = 0;
        }
        else if ( 0 == strncmp( argv[4], "NULLSIZE", 8 ) )
        {
            fNULLCert = TRUE;
            fNULLSizeCert = TRUE;
        }
    }

    if ( 6 <= argc && argv[5] == NULL )
    {
            fNULLEnum = TRUE;
    }

    cbMeterCert = cchMeterCert * SIZEOF( DRM_WCHAR );

PREFAST_PUSH_DISABLE_EXPLAINED( 26015, "Parameter combination may violate SAL." );

    dr = Drm_MeterCert_EnumNext( fNULLEnum   ? NULL : poMeterCertEnum,
                                 fNULLmid    ? NULL : &midReturned,
                                 fNULLLAINFO ? NULL :  rgwcLAINFO,
                                 fNULLSizeLAINFO ? NULL : &cchLAINFO,
                                 &fIsV2,
                                 fNULLCert   ? NULL :  ( DRM_BYTE * )pwszMeterCert,
                                 fNULLSizeCert ? NULL : &cbMeterCert );

PREFAST_POP;

    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }
    dr = DRM_SUCCESS;

    cchMeterCert = cbMeterCert / SIZEOF( DRM_WCHAR );

    if( argc > 6
     && argv[6] != NULL )
    {
        DRM_BOOL fValue = FALSE;

        StringToBool( argv[6], &fValue );

        ChkBOOL( fValue == fIsV2, DRM_E_TEST_UNEXPECTED_OUTPUT )
    }

    if ( !fNULLmid )
    {
        ChkBOOL( MEMCMP( &mid, &midReturned, SIZEOF( DRM_MID ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    if ( !fNULLLAINFO && !fBadSizeLAINFO )
    {
        DRM_UTL_DemoteUNICODEtoASCII( rgwcLAINFO, ( DRM_CHAR * ) rgwcLAINFO, min( cchLAINFO, SIZEOF( rgwcLAINFO ) / SIZEOF( DRM_WCHAR ) ) );
        ChkBOOL( strncmp( argv[1], ( DRM_CHAR * ) rgwcLAINFO, cchLAINFO ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else if ( !fNULLSizeLAINFO )
    {
        ChkBOOL( DRMCRT_strlen( argv[1] ) == cchLAINFO , DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    if ( argc >= 3 )
    {
        ChkArg( argv[2] != NULL );

        if ( *( argv[2] ) != '\0' )
        {
            if( !fNULLCert && !fBadSizeCert )
            {
                DRM_UTL_DemoteUNICODEtoASCII( pwszMeterCert, ( DRM_CHAR * ) pwszMeterCert, min( cchMeterCert, DRM_MAX_TEST_METERCERT_SIZE ) );
                ChkBOOL( strncmp( argv[2], ( DRM_CHAR * ) pwszMeterCert, cchMeterCert ) == 0, DRM_E_TEST_METER_CERTIFICATE_MISMATCH );
            }
            else if ( !fNULLSizeCert )
            {
                ChkBOOL( DRMCRT_strlen( argv[2] ) == cchMeterCert, DRM_E_TEST_UNEXPECTED_OUTPUT );
            }
        }
    }

ErrorExit:
    SAFE_OEM_FREE( pwszMeterCert );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerUpdateMeterCert
**
** Synopsis:
**
** Arguments:
**              argv[0] -- MeterCert, if NULL pass a NULL Pointer
**              argv[1] -- opt, mid expected, if NULL use a NULL pointer for the MID
**              argv[2] -- opt, if NULL pass a NULL DRM_APP_CONTEXT
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerUpdateMeterCert( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT        dr             = DRM_SUCCESS;
    DRM_SUBSTRING     dsstrTemp      = EMPTY_DRM_SUBSTRING;
    DRM_MID           mid            = {0};
    DRM_MID           midRet         = {0};
    DRM_DWORD         cbMID          = SIZEOF( mid.rgb );
    DRM_STRING        dstrMeterCert  = EMPTY_DRM_STRING;
    DRM_CONST_STRING *pdstrMeterCert = NULL;
    DRM_APP_CONTEXT  *poManagerContext = NULL;

    ChkArg( argc > 0 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    GET_SHARED_APP_CONTEXT( poManagerContext );

PREFAST_PUSH_DISABLE_EXPLAINED( 22011, "Already checking integer overflow." );
    dstrMeterCert.cchString = DRM_MAX_TEST_METERCERT_SIZE;
    ChkOverflow( dstrMeterCert.cchString * SIZEOF( DRM_WCHAR ), dstrMeterCert.cchString );
    ChkMem( dstrMeterCert.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrMeterCert.cchString * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( ( DRM_BYTE * )dstrMeterCert.pwszString, dstrMeterCert.cchString * SIZEOF( DRM_WCHAR ) );
PREFAST_POP;

    if ( argv[0] != NULL )
    {
        /*
        ** Convert to UTF-16 (DRM string)
        */
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRM_STR_UTF8toDSTR( ( DRM_CHAR* )( argv[0] ), cch, &dstrMeterCert ) );

        pdstrMeterCert = ( DRM_CONST_STRING * ) &dstrMeterCert;
    }
    ChkDR( Drm_MeterCert_Update( ( argc > 2 && argv[2] == NULL ) ? NULL : poManagerContext,
                                  pdstrMeterCert != NULL ? PB_DSTR( pdstrMeterCert ) : NULL,
                                  pdstrMeterCert != NULL ? CB_DSTR( pdstrMeterCert ) : 0,
                                 ( argc > 1 && argv[1] == NULL ) ? NULL : &midRet ) );

    if ( argc > 1 && argv[1] != NULL )
    {
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &dsstrTemp.m_cch ) );
        dsstrTemp.m_ich = 0;
        ChkDR( DRM_B64_DecodeA( argv[1], &dsstrTemp, &cbMID, mid.rgb, 0 ) );

        ChkBOOL( MEMCMP( mid.rgb, midRet.rgb, SIZEOF( DRM_MID ) ) == 0, DRM_E_TEST_METER_CERTIFICATE_MISMATCH );
    }

ErrorExit:
    FREEDRMSTRING( dstrMeterCert );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerUpdateMeterCertFromFile
**
** Synopsis:
**
** Arguments:
**              argv[0] -- Optional: Name of the file that contains the meter cert.
**              argv[1] -- Optional: MID expected, if NULL the returned MID is not checked.
**              argv[2] -- Optional: if NULL a NULL DRM_APP_CONTEXT will be used.
**
** Returns:     DRM_SUCCESS - on success
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerUpdateMeterCertFromFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dsstrTemp = EMPTY_DRM_SUBSTRING;
    DRM_MID oMID = { 0 };
    DRM_MID oMIDRet = { 0 };
    DRM_DWORD cbMID = SIZEOF( oMID.rgb );
    DRM_BYTE *pbMeterCert = NULL;
    DRM_DWORD cbMeterCert = 0;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    ChkArg( argc > 0 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    if ( argv[ 0 ] != NULL )
    {
        ChkArg( LoadTestFile( NULL,
                              argv[0],
                              &pbMeterCert,
                              &cbMeterCert ) );
    }

    ChkDR( Drm_MeterCert_Update( ( argc > 2 && argv[ 2 ] == NULL ) ? NULL : poManagerContext,
                                 pbMeterCert,
                                 cbMeterCert,
                                 ( argc > 1 && argv[ 1 ] == NULL ) ? NULL : &oMIDRet ) );

    if ( argc > 1 && argv[ 1 ] != NULL )
    {
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[ 1 ] ), &dsstrTemp.m_cch ) );
        dsstrTemp.m_ich = 0;
        ChkDR( DRM_B64_DecodeA( argv[ 1 ], &dsstrTemp, &cbMID, oMID.rgb, 0 ) );

        ChkBOOL( MEMCMP( oMID.rgb, oMIDRet.rgb, SIZEOF( DRM_MID ) ) == 0, DRM_E_TEST_METER_CERTIFICATE_MISMATCH );
    }

ErrorExit:
    SAFE_OEM_FREE( pbMeterCert );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerUpdateMeterCert
**
** Synopsis:
**
** Arguments:
**              argv[0] -- MID to invalidate
**              argv[1] -- opt, if NULL use a NULL DRM_APP_CONTEXT
**              argv[2] -- opt, if NULL pass a NULL pointer for the MID
**              argv[3] -- opt, if NULL then the function doesn't assume the MID is already
**                              in the store and doesn't try to get info about the cert before
**                              and after to verify invalid state.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerInvalidateMeterCert( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT    dr                = DRM_SUCCESS;
    DRM_SUBSTRING dsstrMID          = EMPTY_DRM_SUBSTRING;
    DRM_MID       mid               = {0};
    DRM_DWORD     cbmid             = SIZEOF( mid.rgb );
    DRM_DWORD     cchMeterCert      = 0;
    DRM_DWORD     cchMeterCertFirst = 0;
    DRM_DWORD     cchLAINFO         = 0;
    DRM_APP_CONTEXT  *poManagerContext = NULL;

    ChkArg( argc    >= 1  );
    ChkArg( argv[0] != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    dsstrMID.m_ich = 0;
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &dsstrMID.m_cch ) );
    ChkDR( DRM_B64_DecodeA( argv[0], &dsstrMID, &cbmid, mid.rgb, 0 ) );

    if ( argc < 4 || argv[3] != NULL )
    {
        dr = DRM_MCS_GetMeterCertByMID( &( ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext )->oMeterCertContext,
                                          &mid,
                                           NULL,
                                          &cchLAINFO,
                                           NULL,
                                           NULL,
                                          &cchMeterCertFirst );
        if( dr != DRM_E_BUFFERTOOSMALL )
        {
           ChkDR( dr );
        }
    }

    ChkDR( Drm_MeterCert_Invalidate( ( argc >= 2 && argv[1] == NULL ) ? NULL :  poManagerContext,
                                        ( argc >= 3 && argv[2] == NULL ) ? NULL : &mid ) );

    if ( argc < 4 || argv[3] != NULL )
    {
        dr = DRM_MCS_GetMeterCertByMID( &( ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext )->oMeterCertContext,
                                          &mid,
                                           NULL,
                                          &cchLAINFO,
                                           NULL,
                                           NULL,
                                          &cchMeterCert );

        if( dr != DRM_E_BUFFERTOOSMALL )
        {
           ChkDR( dr );
        }
        else
        {
            dr = DRM_SUCCESS;
        }

        ChkBOOL( cchMeterCert == 0 && ( cchMeterCertFirst != 0 || cchMeterCert != cchMeterCertFirst ), DRM_E_FAIL );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerUpdateMeterCert
**
** Synopsis:
**
** Arguments:
**              argv[0] -- MID to delete
**              argv[1] -- opt, if NULL use an NULL DRM_APP_CONTEXT
**              argv[2] -- opt, if NULL pass a NULL pointer for the MID
**              argv[3] -- opt, if NULL then the function doesn't assume the MID is already
**                              in the store and doesn't try to get info about the cert before
**                              and after to verify deletion.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerDeleteMeterCert( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT    dr                = DRM_SUCCESS;
    DRM_SUBSTRING dsstrMID          = EMPTY_DRM_SUBSTRING;
    DRM_MID       mid               = {0};
    DRM_DWORD     cbmid             = SIZEOF( mid.rgb );
    DRM_DWORD     cchMeterCert      = 0;
    DRM_DWORD     cchLAINFO         = 0;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    ChkArg( argc    >= 1  );
    ChkArg( argv[0] != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    GET_SHARED_APP_CONTEXT( poManagerContext );

    dsstrMID.m_ich = 0;
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &dsstrMID.m_cch ) );
    ChkDR( DRM_B64_DecodeA( argv[0], &dsstrMID, &cbmid, mid.rgb, 0 ) );

    if ( argc < 4 || argv[3] != NULL )
    {
        dr = DRM_MCS_GetMeterCertByMID( &( ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext )->oMeterCertContext,
                                          &mid,
                                           NULL,
                                          &cchLAINFO,
                                           NULL,
                                           NULL,
                                          &cchMeterCert );
        if( dr != DRM_E_BUFFERTOOSMALL )
        {
           ChkDR( dr );
        }
    }

    ChkDR( Drm_MeterCert_Delete( ( argc >= 2 && argv[1] == NULL ) ? NULL :  poManagerContext,
                                    ( argc >= 3 && argv[2] == NULL ) ? NULL : &mid ) );

    if ( argc < 4 || argv[3] != NULL )
    {
        dr = DRM_MCS_GetMeterCertByMID( &( ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext )->oMeterCertContext,
                                             &mid,
                                              NULL,
                                             &cchLAINFO,
                                              NULL,
                                              NULL,
                                             &cchMeterCert );

        if( dr != DRM_E_METERCERT_NOT_FOUND )
        {
            /*
            ** The metercert should have been deleted, need to fail here
            */
            if( DRM_SUCCEEDED( dr ) )
            {
                dr = DRM_E_FAIL;
            }

            ChkDR( dr );
        }
        else
        {
            dr = DRM_SUCCESS;
        }
    }

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

