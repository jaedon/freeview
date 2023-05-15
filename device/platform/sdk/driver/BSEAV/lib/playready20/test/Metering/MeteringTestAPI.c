/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmmanager.h>
#include <MeteringTestAPI.h>
#include <drmcmdlnpars.h>
#include <drmxmlparser.h>
#include <drmxmlbuilder.h>
#include <drmmetercertprivkey.h>
#include <V3MeteringTestHelpers.h>
#include <drmsoapxmlutility.h>
#include <drmmeterapi.h>
#include <drmmeterimp.h>
#include <drmdlaconstants.h>
#include <drmmeterconstants.h>

ENTER_PKTEST_NAMESPACE_CODE

/*
** ---------------------------------------
** Metering Test Global Variables
** ---------------------------------------
*/
DRM_APP_CONTEXT *m_pContextManager = {0};
const DRM_CHAR *m_pLogFile = "RefMeteringTestLogFile.xml";

/* "ProcessMeteringDataResult/Response/MeteringResponse" */
const DRM_CHAR g_rgchMeterDataMeteringResponsePath_Local [] = "ProcessMeteringDataResult/Response/MeteringResponse";
const DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringResponsePath_Local = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringResponsePath_Local, 51 );

/* "MID" */
const DRM_CHAR g_rgchMeterDataMIDPath_Local [] = "MID";
const DRM_ANSI_CONST_STRING g_dastrMeterDataMIDPath_Local = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMIDPath_Local, 3 );

/*****************************************************************************
** Function:    _VerifyMeterChallenge
**
** Synopsis:    Given a clear XML metering records and expected record,
**              verify that metering challenge is what we expect.
**
** Arguments:   f_pdstrExpectedData - Expected Metering data
**              f_cExpectedData - metering data length
**              f_dstrRecords - Metering data from meter challenge
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT _VerifyMeterChallenge(
    __in_ecount(f_cExpectedData) DRM_CONST_STRING *f_pdstrExpectedData,
    __in DRM_DWORD                           f_cExpectedData,
    __in DRM_CONST_STRING                    f_dstrRecords)
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_DWORD        dwSubindex     = 0;
    DRM_DWORD        index          = 0;
    DRM_BOOL         fFound         = TRUE;
    DRM_CONST_STRING dstrKidData    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKID        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrValue      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrAction     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrActionData = EMPTY_DRM_STRING;

    ChkArg( f_cExpectedData % 3 == 0 );    /* verification data come in triads */

    ChkDR(DRM_XML_GetNode(&f_dstrRecords, &g_dstrTagKID, NULL, NULL, dwSubindex, &dstrKidData, &dstrKID));

    for (index = 0; index < f_cExpectedData; index += 3)
    {
        #pragma prefast(suppress:28931)     /* this assignment is needed, prefast disagrees */
        fFound = FALSE;
        for(dwSubindex = 0;;dwSubindex++)
        {
            ChkDR(DRM_XML_GetNode(&f_dstrRecords, &g_dstrTagKID, NULL, NULL, dwSubindex, &dstrKidData, &dstrKID));
            ChkDR(DRM_XML_GetNodeAttribute(&dstrKidData, &g_dstrLabelValue, &dstrValue));
            if (! DRM_UTL_DSTRStringsEqual(&f_pdstrExpectedData[index],&dstrValue))
            {
                continue;
            }
            else
            {
                fFound = TRUE;
                break;
            }
        }

        ChkBOOL(fFound, DRM_E_FAIL);    // KID not found

        #pragma prefast(suppress:28931)     /* this assignment is needed, prefast disagrees */
        fFound = FALSE;

        for (dwSubindex = 0;;dwSubindex++)
        {
            ChkDR(DRM_XML_GetNode(&dstrKID, &g_dstrTagAction, NULL, NULL, dwSubindex, &dstrActionData, &dstrAction));
            ChkDR(DRM_XML_GetNodeAttribute(&dstrActionData, &g_dstrLabelValue, &dstrValue));
            if (! DRM_UTL_DSTRStringsEqual(&f_pdstrExpectedData[index + 1],&dstrValue))
            {
                continue;
            }
            else
            {
                #pragma prefast(suppress:28931)     /* this assignment is needed, prefast disagrees */
                fFound = TRUE;
                break;
            }
        }

        ChkBOOL(fFound, DRM_E_FAIL);    // Action not found

        ChkBOOL(DRM_UTL_DSTRStringsEqual(&f_pdstrExpectedData[index + 2],&dstrAction), DRM_E_FAIL);
    }

ErrorExit:

    return dr;
}

/*****************************************************************************
** Function:    _GetMIDFromMeterResponse
**
** Synopsis:    Extracted the MID from a metering response.
**
** Arguments:   pbResponse - Metering response
**              cbResponse - Metering response length
**              poMID      - Pointer to a variable to receive the MID.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**              Skip parameter check for internal function.
**
******************************************************************************/
DRM_RESULT _GetMIDFromMeterResponse(
    __in_ecount( cbResponse ) DRM_BYTE *pbResponse,
    __in DRM_DWORD cbResponse,
    __out DRM_MID *poMID)
{
    DRM_RESULT               dr                = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING    dastrResponse     = EMPTY_DRM_STRING;
    DRM_SUBSTRING            dasstrData        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING            dasstrResponse    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING            dasstrMID         = EMPTY_DRM_SUBSTRING;
    DRM_DWORD                cbData            = 0;

    DASTR_FROM_PB( &dastrResponse, pbResponse, cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                         &dasstrResponse ) );

#endif

    /*
    ** Extract <MeteringResponse> node from
    ** <ProcessMeteringDataResponse>/<ProcessMeteringDataResult>/<Response> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( ( DRM_CHAR *)dastrResponse.pszString,
                                      &dasstrResponse,
                                      &g_dastrMeterDataMeteringResponsePath_Local,
                                      NULL,
                                      NULL,
                                      &dasstrData,
                                      NULL,
                                      g_chForwardSlash ) );

    /*
    ** Extract <MID> node from the extracted <MeteringResponse> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( ( DRM_CHAR *)dastrResponse.pszString,
                                      &dasstrData,
                                      &g_dastrMeterDataMIDPath_Local,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrMID,
                                      g_chForwardSlash ) );

    cbData = SIZEOF( DRM_MID );

    ChkDR( DRM_B64_DecodeA( dastrResponse.pszString,
                            &dasstrMID,
                            &cbData,
                            ( DRM_BYTE *)poMID,
                            0 ) );

    ChkBOOL( cbData == SIZEOF( DRM_MID ), DRM_E_FAIL );

ErrorExit:

    return dr;
}

/*****************************************************************************
** Function:    _VerifyMeterState
**
** Synopsis:    Given a MID, and expected number of meter entries
**              left in the HDS, verify that HDS has the correct number of
**              metering data left.
**
** Arguments:   poMID                - Pointer to a MID.
**              dwExpectedNumEntries - Expected number of meter entries in HDS
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**              Skip parameter check for internal function.
**
******************************************************************************/
DRM_RESULT _VerifyMeterState(
    __in DRM_MID *poMID,
    __in DRM_DWORD dwExpectedNumEntries)
{
    DRM_RESULT               dr                = DRM_SUCCESS;
    DRM_METERINGENUM_CONTEXT oMeterEnumContext = { 0 };
    DRM_KID                  oKID              = { 0 };
    DRM_DWORD                cbData            = 0;
    DRM_DWORD                dwEntryCount      = 0;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = (DRM_APP_CONTEXT_INTERNAL *)m_pContextManager;

    ChkDR( DRM_MTR_InitEnum( poMID,
                             &oMeterEnumContext,
                             &(poAppContextInternal->oDatastoreHDS) ) );

    while(TRUE)
    {
        dr = DRM_MTR_EnumNext( &oMeterEnumContext, &oKID, &cbData );

        if( dr == DRM_E_NOMORE )
        {
            break;
        }

        dwEntryCount++;
    }

    if ( dwEntryCount == dwExpectedNumEntries )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        dr = DRM_E_METERING_STORE_CORRUPT;
    }

ErrorExit:

    return dr;
}

DRM_RESULT _GetMeterCertFromFile(const DRM_WCHAR *pwchInFile, DRM_WCHAR  *pwchMeterCert)
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    OEM_FILEHDL hfile       = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR   *pwchFile   = NULL;
    DRM_DWORD   cbFile      = 0;
    DRM_DWORD   cbRead      = 0;


    ChkArg( pwchInFile != NULL );
    ChkArg( pwchMeterCert != NULL );

    hfile = Oem_File_Open( NULL,
                           pwchInFile,
                           OEM_GENERIC_READ,
                           OEM_FILE_SHARE_NONE,
                           OEM_OPEN_EXISTING,
                           OEM_ATTRIBUTE_NORMAL );
    ChkBOOL( hfile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILENOTFOUND );

    ChkBOOL( Oem_File_GetSize( hfile, &cbFile ),
             DRM_E_FILESEEKERROR );

    ChkMem( pwchFile = (DRM_WCHAR*)Oem_MemAlloc ( cbFile + 2 ) );
    ZEROMEM( pwchFile, cbFile+2 );

    ChkBOOL( Oem_File_Read( hfile, pwchFile, cbFile, &cbRead ), DRM_E_FILEREADERROR );

    MEMCPY( pwchMeterCert, pwchFile, cbRead );
    LogBlob( "", "Reading Meter Cert: ", (DRM_BYTE *) pwchMeterCert, cbFile );

ErrorExit:
    SAFE_OEM_FREE( pwchFile );
    if ( hfile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close ( hfile );
    }

    return dr;
}

DRM_RESULT _GetMeterResponseFromFile(const DRM_WCHAR *pwchInResponseFile, DRM_WCHAR  **pwchMeterResponse)
{
    DRM_RESULT dr = DRM_E_FILENOTFOUND;
    OEM_FILEHDL hfile         = OEM_INVALID_HANDLE_VALUE;

    *pwchMeterResponse = NULL;

    if (pwchInResponseFile)
    {
        DRM_WCHAR  *pwchFile      = NULL;
        DRM_DWORD   cbFile        = 0,
                    cbRead        = 0;

        if ((hfile = Oem_File_Open (NULL,
                                    pwchInResponseFile,
                                    OEM_GENERIC_READ,
                                    OEM_FILE_SHARE_NONE,
                                    OEM_OPEN_EXISTING,
                                    OEM_ATTRIBUTE_NORMAL)) != OEM_INVALID_HANDLE_VALUE)
        {
            dr = DRM_E_FILEREADERROR;
            if (Oem_File_GetSize (hfile, &cbFile))
            {
                if ((pwchFile = (DRM_WCHAR*)Oem_MemAlloc (cbFile + 2)) != NULL)
                {
                    ZEROMEM( pwchFile, cbFile+2 );
                    if (Oem_File_Read (hfile, pwchFile, cbFile, &cbRead))
                    {
                        *pwchMeterResponse = pwchFile;
                        pwchFile = NULL;
                        LogBlob( "", "Reading Meter Response: ", ( DRM_BYTE* ) *pwchMeterResponse, cbFile );
                        dr = DRM_SUCCESS;
                    }
                    else
                    {
                        Log (m_pLogFile, "Unable to input file read file %S.\n", pwchInResponseFile);
                        Oem_MemFree (pwchFile);
                    }

                }
                else
                {
                    Log (m_pLogFile, "Unable to allocate memory to read file.\n");
                }
            }
            else
            {
                Log (m_pLogFile, "Unable to get size for file file.\n");
            }

        }
    }
    else
    {
        /* argument not supplied */

        Log (m_pLogFile, "Input file %S could not open.\n", pwchInResponseFile);
    }

    if (hfile != OEM_INVALID_HANDLE_VALUE)
        Oem_File_Close (hfile);
    return dr;
}

static DRM_DWORD _RequiredCharsForResponse(
    DRM_CONST_STRING *pdstrRecords,
    DRM_BOOL          fPartialData,
    DRM_DWORD        *pcchKIDs)
{
    DRM_DWORD  cchRequired = 25;
    DRM_LONG   iKid        = 0;
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
    DRM_CONST_STRING *pdstrFileResponse,
    DRM_CONST_STRING *pdstrMID,
    DRM_CONST_STRING *pdstrTID,
    DRM_CONST_STRING *pdstrRecords,
    PUBKEY           *ppubkeEncrypt,
    PRIVKEY          *pSignKey)
{
    DRM_BOOL            fPartialData    = 0;
    OEM_FILEHDL         hFileOut        = OEM_INVALID_HANDLE_VALUE;
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_DWORD           cbOut           = 0;
    DRM_DWORD           cbWritten       = 0;
    DRM_DWORD           cchKIDs         = 0;
    DRM_LONG            iKID            = 0;
    DRM_STRING          strXML          = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrKID         = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrKIDB64      = EMPTY_DRM_STRING;
    DRM_STRING          dstrClose       = EMPTY_DRM_STRING;
    _XMBContext        *pbOut           = NULL;
    DRM_WCHAR          *pwszOut         = NULL;
    DRM_CRYPTO_CONTEXT *pcontextCrypto  = NULL;

    hFileOut = Oem_File_Open( NULL,
                              pdstrFileResponse->pwszString,
                              OEM_GENERIC_WRITE,
                              OEM_FILE_SHARE_NONE,
                              OEM_CREATE_ALWAYS,
                              OEM_ATTRIBUTE_NORMAL );

    ChkBOOL( hFileOut != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEWRITEERROR );

    ChkMem( pcontextCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc( SIZEOF ( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pcontextCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    cbOut =  SIZEOF (DRM_WCHAR) * _RequiredCharsForResponse( pdstrRecords, fPartialData, &cchKIDs ) ;
    ChkArg( cchKIDs != 0 );

    ChkMem( pbOut = (_XMBContext *)Oem_MemAlloc( cbOut ) );
    ZEROMEM( pbOut, cbOut );

    ChkDR( DRM_XMB_CreateDocument( cbOut,  pbOut,               &g_dstrTagMetering ) );     /* <METERDATA type="response"> */
    ChkDR( DRM_XMB_AddAttribute(   pbOut, &g_dstrAttributeType, &g_dstrResponse ) );

    ChkDR( DRM_XMB_WriteTag( pbOut, &g_dstrTagData, NULL,        NULL, NULL, wttOpen ) );   /* <DATA> */
    ChkDR( DRM_XMB_WriteTag( pbOut, &g_dstrTagMID,  pdstrMID,    NULL, NULL, wttClosed ) ); /* <MID>base64</MID> */
    ChkDR( DRM_XMB_WriteTag( pbOut, &g_dstrTagTID,  pdstrTID,    NULL, NULL, wttClosed ) ); /* <TID>base64</TID> */
    ChkDR( DRM_XMB_WriteTag( pbOut, &g_dstrTagCmd, &g_dstrReset, NULL, NULL, wttClosed ) ); /* <COMMAND>RESET</COMMAND> */
    ChkDR( DRM_XMB_WriteTag( pbOut, &g_dstrTagRecords, NULL,     NULL, NULL, wttOpen ) );   /* <RECORDS> */

    while ( dr != DRM_E_NOMORE && dr != DRM_E_XMLNOTFOUND )
    {
        dr = DRM_XML_GetNode( pdstrRecords, &g_dstrTagKID, NULL, NULL, iKID++, &dstrKID, NULL );

        if ( DRM_SUCCEEDED ( dr ) )
        {
            /*
            ** extract the value="KIDasbase64" from the challenge
            */
            ChkDR( DRM_XML_GetNodeAttribute( &dstrKID, &g_dstrLabelValue, &dstrKIDB64 ) );

            /*
            ** write the KID entries in the response
            */
            ChkDR( DRM_XMB_WriteTag( pbOut, &g_dstrTagKID, NULL, &g_dstrLabelValue, &dstrKIDB64, wttClosed ) ); /* <KID value="base64"/> */
        }
        else if ( dr != DRM_E_NOMORE && dr != DRM_E_XMLNOTFOUND )
        {
            ChkDR(dr);
        }
    } /* end while more KIDs */
//printf("%S\n", (DRM_WCHAR*)(pbOut + 32));

    ChkDR( DRM_XMB_EncryptAndCloseCurrNode( pbOut,
                                            pcontextCrypto,
                                            NULL,
                                            ppubkeEncrypt,
                                            NULL ) ); /* </RECORDS> */

    if ( fPartialData )
    {                                                                 /*     <PARTIALDATA>1</PARTIALDATA> */
        ChkDR( DRM_XMB_WriteTag( pbOut, &g_dstrTagPartialData, &g_dstr1, NULL, NULL, wttClosed ) );
    }

    /*
    ** Close the data node
    */
    ChkDR( DRM_XMB_CloseCurrNode( pbOut, &dstrClose ) );
    ChkDR( DRM_XMB_CloseDocument( pbOut, &strXML ) ); /* </METERDATA> */

    pwszOut = strXML.pwszString;
    pwszOut [strXML.cchString] = g_wchNull;

    LogBlob( "", "Writing Meter Response: ", ( DRM_BYTE* ) pwszOut, CB_DSTR( &strXML ) );

    ChkBOOL( Oem_File_Write(hFileOut, (DRM_VOID *) pwszOut, CB_DSTR( &strXML ), &cbWritten ), DRM_E_FILEWRITEERROR );

ErrorExit:

    SAFE_OEM_FREE( pbOut );
    SAFE_OEM_FREE( pcontextCrypto );

    if ( hFileOut != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hFileOut );
    }
    return dr;
}

DRM_RESULT _SignResponse(DRM_CONST_STRING *pdstrResponse,
                         DRM_CONST_STRING *pdstrCert,
                         DRM_CONST_STRING *pdstrOut,
                         PRIVKEY *pSignKey)
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

    dstrXML.pwszString = (DRM_WCHAR*)pbOut;
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

    pwszOut = (DRM_WCHAR *) strXML.pwszString;
    pwszOut [strXML.cchString] = g_wchNull;

    pdstrOut->pwszString = pwszOut;
    pdstrOut->cchString = strXML.cchString;

    LogBlob( "", "Signed Meter Response: ", (DRM_BYTE *) PB_DSTR (pdstrOut), CB_DSTR (pdstrOut) );

ErrorExit:

    SAFE_OEM_FREE( pcontextCrypto );

    return dr;
}

DRM_RESULT _GetMeteringDataAsXML(_XMBContext *pbData, DRM_DWORD cbData, PRIVKEY *pPrivKey, DRM_CONST_STRING *pretXML)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CRYPTO_CONTEXT *pcontextCrypto   = NULL;
    DRM_CONST_STRING dstrXML = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTemp = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListTop = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListData = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrRecords = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;

    ChkMem (pcontextCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc (sizeof (DRM_CRYPTO_CONTEXT)));
    ZEROMEM( pcontextCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    DSTR_FROM_PB(&dstrXML, pbData, cbData);

    ChkDR(DRM_XML_GetNode(&dstrXML, &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop));
    ChkDR(DRM_XML_GetNode(&dstrListTop, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrListData));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagMID, NULL, NULL, 0, NULL, &dstrMID));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagRecords, NULL, NULL, 0, NULL, &dstrRecords));

    /* ClientInfo and ClientVersion should NOT be present here
    ** since there is no SDK runtime.
    **/
    if( DRM_SUCCEEDED( DRM_XML_GetNode(&dstrListData, &g_dstrReqTagClientInfo, NULL, NULL, 0, NULL, &dstrTemp) ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    if( DRM_SUCCEEDED( DRM_XML_GetNode(&dstrListData, &g_dstrReqTagClientVersion, NULL, NULL, 0, NULL, &dstrTemp) ) )
    {
        ChkDR( DRM_E_FAIL );
    }

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

    LogBlob( "", "Getting Metering Data: ", (DRM_BYTE *) PB_DSTR (pretXML), CB_DSTR (pretXML) );

ErrorExit:
    if (pcontextCrypto != NULL)
    {
        Oem_MemFree (pcontextCrypto);
    }
    return dr;
}

/*

    argv[0] - filename of first metering response
    argv[1] - filename of second metering response
    argv[2] - whether the TIDs in argv[0] and argv[1] should match.
*/

DRM_RESULT DRM_CALL TestDRM_VerifyTID( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_CONST_STRING    dstrMeterResp1 = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrMeterResp2 = EMPTY_DRM_STRING;
    DRM_WCHAR           *pwchMeterResponse = 0;
    DRM_CONST_STRING    dstrListTop = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrListData = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrMID = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrTID1 = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrTID2 = EMPTY_DRM_STRING;

    ChkArg(argc>2);

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkDR( TST_UTL_MakePathString( NULL,
                                       argv[0],
                                      &dstrMeterResp1 ) );

    ChkDR(_GetMeterResponseFromFile(dstrMeterResp1.pwszString, &pwchMeterResponse));
    ChkArg(pwchMeterResponse);

    FREEDRMSTRING( dstrMeterResp1 );

    dstrMeterResp1.cchString = DRMCRT_wcslen(pwchMeterResponse);
    dstrMeterResp1.pwszString = pwchMeterResponse;

    //parse TID out from first response
    ChkDR(DRM_XML_GetNode(&dstrMeterResp1, &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop));
    ChkDR(DRM_XML_GetNode(&dstrListTop, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrListData));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagMID, NULL, NULL, 0, NULL, &dstrMID));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagTID, NULL, NULL, 0, NULL, &dstrTID1));

    ChkDR( TST_UTL_MakePathString( NULL,
                                       argv[1],
                                      &dstrMeterResp2 ) );

    ChkDR(_GetMeterResponseFromFile(dstrMeterResp2.pwszString, &pwchMeterResponse));
    ChkArg(pwchMeterResponse);

    FREEDRMSTRING( dstrMeterResp2 );

    dstrMeterResp2.cchString = DRMCRT_wcslen(pwchMeterResponse);
    dstrMeterResp2.pwszString = pwchMeterResponse;

    //parse TID out from second response
    ChkDR(DRM_XML_GetNode(&dstrMeterResp2, &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop));
    ChkDR(DRM_XML_GetNode(&dstrListTop, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrListData));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagMID, NULL, NULL, 0, NULL, &dstrMID));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagTID, NULL, NULL, 0, NULL, &dstrTID2));

    if(!strcmp("TRUE",argv[2])) //TIDs should match
    {
        if(!DRM_UTL_DSTRStringsEqual(&dstrTID1,&dstrTID2))
            ChkDR(DRM_E_FAIL);
    }
    else //TIDs should not match
    {
        if(DRM_UTL_DSTRStringsEqual(&dstrTID1,&dstrTID2))
            ChkDR(DRM_E_FAIL);
    }

ErrorExit:
    FREEDRMSTRING( dstrMeterResp1 );
    FREEDRMSTRING( dstrMeterResp2 );
    return dr;
}

/*****************************************************************************
** Function:    TestDRM_Manager_AddManyMeteringData
**
** Synopsis:
**
** Arguments:   argv[0] - MID
**              argv[1] - Number of KIDs to add.
**
** Returns:     DRM_SUCCESS             - on success
**              DRM_E_INVALIDARG        - if any parameter is invalid
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestDRM_Manager_AddManyMeteringData( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT              dr                  = DRM_SUCCESS;
    DRM_METERING_CONTEXT    meterStore          = {0};
    DRM_BYTE                rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)] = { 0 };
    DRM_CONST_STRING        dstrMID             = EMPTY_DRM_STRING;
    DRM_CONST_STRING        dstrKID             = EMPTY_DRM_STRING;
    DRM_CONST_STRING        dstrAction          = EMPTY_DRM_STRING;
    DRM_APP_CONTEXT_INTERNAL *pcontext          = (DRM_APP_CONTEXT_INTERNAL *) m_pContextManager;
    DRM_BB_CONTEXT          *pcontextBlackBox   = &pcontext->oBlackBoxContext;
    DRM_DST                 *pDatastore         = &pcontext->oDatastoreHDS;
    DRM_DWORD               index               = 0;
    DRM_DWORD               kidCount            = 0;
    DRM_MID                 mid                 = {0};
    DRM_KID                 kid                 = {0};
    DRM_DWORD               cbMID               = SIZEOF (mid);
    TOKEN                   token               = {0};
    DRM_BOOL                bStoreOpen          = FALSE;
    DRM_WCHAR               kidbuf[CCH_BASE64_EQUIV(SIZEOF(DRM_KID))+1] = {0};
    DRM_KID                 kidTempKID          = {0};
    DRM_DWORD               cch                 = 0;

    ChkArg( argc > 1 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkDR( MakeDRMString( &dstrMID, argv[0] ) );
    ChkDR( DRM_BBX_HashValue( (DRM_BYTE*)dstrMID.pwszString, dstrMID.cchString*2, rgbPasswordSST, pcontextBlackBox ) );

    ChkDR( DRM_B64_DecodeW( &dstrMID, &cbMID, (DRM_BYTE  *)&mid, 0 ) );

    ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 10, &kidCount ) );
    for ( index = 0; index < kidCount; index++ )
    {
        MEMSET( kidTempKID.rgb, 0, SIZEOF(DRM_KID) );
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)kidTempKID.rgb, SIZEOF( DRM_KID ) ) );

        cch = CCH_BASE64_EQUIV( SIZEOF (DRM_KID) );
        DRM_B64_EncodeW( (DRM_BYTE*)kidTempKID.rgb, SIZEOF (DRM_KID), kidbuf, &cch, 0 );
        dstrKID.pwszString = kidbuf;
        dstrKID.cchString = ( DRM_DWORD ) DRMCRT_wcslen(kidbuf);

        cch = SIZEOF( DRM_KID );

        ChkDR( DRM_B64_DecodeW( &dstrKID, &cch, (DRM_BYTE*)&kid, 0 ) );
        ChkDR( MakeDRMString( &dstrAction, "Play" ) );

        ChkDR( DRM_MTR_OpenId( &meterStore, &mid, &kid, rgbPasswordSST, pDatastore ) );
        bStoreOpen = TRUE;

        /*
        ** zero out any data
        */
        ChkDR( DRM_SST_GetTokenValue( &meterStore, &dstrAction, &token ) );
        token.val.lValue = 1;
        token.TokenType  = TOKEN_LONG;
        ChkDR( DRM_SST_SetTokenValue(&meterStore, &dstrAction , &token ) );

        DRM_MTR_CloseContext( &meterStore, pDatastore );
        bStoreOpen = FALSE;
    }
ErrorExit:

    FREEDRMSTRING( dstrMID );
    FREEDRMSTRING( dstrAction );

    if ( bStoreOpen )
    {
        DRM_MTR_CloseContext( &meterStore, pDatastore );
    }

    return dr;
}

/*
    argv[0] - MID
    argv[1] - KID
    argv[2] - Action
    argv[3] - Value
*/
DRM_RESULT DRM_CALL TestDRM_Manager_AddMeteringData( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_METERING_CONTEXT meterStore = {0};
    DRM_BYTE rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)] = { 0 };
    DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrAction = EMPTY_DRM_STRING;
    DRM_APP_CONTEXT_INTERNAL *pcontext = (DRM_APP_CONTEXT_INTERNAL *) m_pContextManager;
    DRM_BB_CONTEXT *pcontextBlackBox = &pcontext->oBlackBoxContext;
    DRM_DST *pDatastore = &pcontext->oDatastoreHDS;
    long index = 0;
    long count = 0;
    DRM_MID mid = {0};
    DRM_KID kid = {0};
    DRM_DWORD cbMID = SIZEOF (mid);
    DRM_DWORD cbKID = SIZEOF (kid);
    TOKEN token = {0};
    DRM_BOOL bStoreOpen = FALSE;

    ChkArg( argc > 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkDR( MakeDRMString(&dstrMID, argv[0]) );

    ChkDR(DRM_BBX_HashValue((DRM_BYTE*)dstrMID.pwszString,dstrMID.cchString*2,rgbPasswordSST,pcontextBlackBox));

    ChkDR(DRM_B64_DecodeW(&dstrMID, &cbMID, (DRM_BYTE  *)&mid, 0));

    ChkDR( MakeDRMString(&dstrKID, argv[1]) );
    ChkDR(DRM_B64_DecodeW(&dstrKID, &cbKID, (DRM_BYTE  *)&kid, 0));

    ChkDR( MakeDRMString(&dstrAction, argv[2]) );

    ChkDR(DRM_MTR_OpenId(&meterStore, &mid, &kid, rgbPasswordSST, pDatastore));
    bStoreOpen = TRUE;
    count = atoi(argv[3]);

    // zero out any data
    ChkDR(DRM_SST_GetTokenValue(&meterStore, &dstrAction, &token));
    token.val.lValue = 0;
    token.TokenType  = TOKEN_LONG;
    ChkDR(DRM_SST_SetTokenValue(&meterStore, &dstrAction , &token));

    // call DRM_MTR_IncrementCount instead of directly setting value above to test DRM_MTR_IncrementCount.
    for (index = 0; index < count; index++)
    {
        ChkDR(DRM_MTR_OpenId(&meterStore, &mid, &kid, rgbPasswordSST, pDatastore));
        bStoreOpen = TRUE;
        ChkDR(DRM_MTR_IncrementCount(&meterStore, &dstrAction));
        DRM_MTR_CloseContext(&meterStore, pDatastore);
        bStoreOpen = FALSE;
    }
ErrorExit:

    FREEDRMSTRING( dstrMID );
    FREEDRMSTRING( dstrKID );
    FREEDRMSTRING( dstrAction );

    if (bStoreOpen)
        DRM_MTR_CloseContext(&meterStore, pDatastore);

    return dr;
}

/*****************************************************************************
** Function:    TestDRM_Manager_GeneratePartialMeterChallenge
**
** Synopsis:
**
** Arguments:   argv[0] - Metering Certificate file
**              argv[1] - file private key
**              argv[2] - response file
**              argv[3] - Max Number of bytes in challenge
**
** Returns:     DRM_SUCCESS             - on success
**              DRM_E_INVALIDARG        - if any parameter is invalid
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestDRM_Manager_GeneratePartialMeterChallenge( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT          dr                  = DRM_SUCCESS;

    DRM_CONST_STRING    dstrMeterCertInFile = EMPTY_DRM_STRING;

    _XMBContext         *pbMeteringData     = NULL;
    DRM_DWORD           cbMeteringData      = 0;
    DRM_CONST_STRING    dstrMeterCert       = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrMeteringXML     = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrPrivKey         = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrFileOut         = EMPTY_DRM_STRING;

    DRM_DWORD           cbReqd              = 0;
    DRM_DWORD           maxLen              = 0;
    DRM_CONST_STRING    dstrListTop         = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrListData        = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrRecords         = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrMID             = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrTID             = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrFallbackCert    = EMPTY_DRM_STRING;
    DRM_WCHAR           pwchMeterCert[2000] = {0};
    PKCERT              fallbackCert        = {0};
    DRM_DWORD           cbB64Decode         = 0;

    ChkArg( argc > 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[3] != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkDR( TST_UTL_MakePathString( NULL,
                                   argv[0],
                                   &dstrMeterCertInFile ) );

    ChkDR( MakeDRMString( &dstrPrivKey, argv[1] ) );

    ChkDR( DRMCRT_AtoDWORD( argv[3], (DRM_DWORD)strlen( argv[3] ), 10, &maxLen ) );

    ChkDR( DRM_B64_DecodeW( &dstrPrivKey,
                            &cbReqd,
                            NULL,
                            DRM_BASE64_DECODE_IN_PLACE ) );

    ChkDR( _GetMeterCertFromFile( dstrMeterCertInFile.pwszString, pwchMeterCert ) );

    dstrMeterCert.cchString  = DRMCRT_wcslen( pwchMeterCert );
    dstrMeterCert.pwszString = pwchMeterCert;
    dr = Drm_Metering_GenerateChallenge (m_pContextManager,
                                         PB_DSTR(&dstrMeterCert),
                                         CB_DSTR(&dstrMeterCert),
                                         NULL,
                                         0,
                                         NULL,
                                         NULL,
                                         NULL,
                                         &cbMeteringData);

    if (dr != DRM_E_BUFFERTOOSMALL)
    {
        if( dr == DRM_SUCCESS)
        {
            ChkDR( DRM_E_LOGICERR );
        }
        ChkDR( dr );
    }

    cbMeteringData = min( cbMeteringData, maxLen );

    ChkMem ( pbMeteringData = (_XMBContext*)Oem_MemAlloc ( cbMeteringData ) );
    ZEROMEM( pbMeteringData, cbMeteringData );
    printf("%s %s\n", argv[0], argv[2] ? argv[2] : "No response");
    ChkDR(Drm_Metering_GenerateChallenge (m_pContextManager,
                                          PB_DSTR(&dstrMeterCert),
                                          CB_DSTR(&dstrMeterCert),
                                          NULL,
                                          0,
                                          NULL,
                                          NULL,
                                          (DRM_BYTE*)pbMeteringData,
                                          &cbMeteringData));

    ChkDR( _GetMeteringDataAsXML( pbMeteringData, cbMeteringData, (PRIVKEY *)dstrPrivKey.pwszString, &dstrMeteringXML ) );

    ChkDR( DRM_XML_GetNode( &dstrMeteringXML, &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop ) );
    ChkDR( DRM_XML_GetNode( &dstrListTop, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrListData ) );
    ChkDR( DRM_XML_GetNode( &dstrListData, &g_dstrTagMID, NULL, NULL, 0, NULL, &dstrMID ) );
    ChkDR( DRM_XML_GetNode( &dstrListData, &g_dstrTagTID, NULL, NULL, 0, NULL, &dstrTID ) );
    ChkDR( DRM_XML_GetNode( &dstrListData, &g_dstrTagRecords, NULL, NULL, 0, NULL, &dstrRecords ) );
    Log( m_pLogFile, "Partial Meter Challenge Records Length:\t%d\n", dstrRecords.cchString );
    ChkDR( DRM_XML_GetNode( &dstrListTop,  &g_dstrTagCertificate, NULL, NULL, 0, NULL, &dstrFallbackCert ) );

    cbB64Decode = SIZEOF( PKCERT );
    ChkDR( DRM_B64_DecodeW( &dstrFallbackCert, &cbB64Decode, (DRM_BYTE *)&fallbackCert, 0 ) );

    if ( argv[2] != NULL )
    {
        ChkDR( TST_UTL_MakePathString( NULL,
                                       argv[2],
                                      &dstrFileOut ) );
        ChkDR( WriteMeterResponse( &dstrFileOut,
                                   &dstrMID,
                                   &dstrTID,
                                   &dstrRecords,
                                   (PUBKEY *)&fallbackCert.pk.pk,
                                   (PRIVKEY*)dstrPrivKey.pwszString ) );
    }
    else if ( dstrRecords.cchString != 0 )
    {
        Log( m_pLogFile, "Error: Partial Meter Challenge Expected to have 0 records\n " );
        LogBlob( "", "Current Records: ", (DRM_BYTE *)dstrRecords.pwszString, dstrRecords.cchString );
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
    FREEDRMSTRING( dstrFileOut );
    FREEDRMSTRING( dstrMeterCertInFile );
    SAFE_OEM_FREE( pbMeteringData );
    FREEDRMSTRING( dstrPrivKey );

    return dr;
}

/*
    argv[0] - Metering Certificate file
    argv[1] - file private key
    argv[2] - response file [optional]
    argv[3] - expected Mid [optional]
    argv[4] - expected KID [optional]
    argv[5] - expected Action [optional]
    argv[6] - expected value [optional]
    argv[7] - expected KID [optional]
    argv[8] - expected Action [optional]
    argv[9] - expected value [optional]
    ...
*/
DRM_RESULT DRM_CALL TestDRM_Manager_GenerateMeterChallenge( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr                        = DRM_SUCCESS;

    DRM_CONST_STRING dstrMeterCertInFile = EMPTY_DRM_STRING;

    _XMBContext         *pbMeteringData = NULL;
    DRM_DWORD            cbMeteringData = 0;
    DRM_CONST_STRING     dstrMeterCert  = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrMeteringXML = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrPrivKey = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrExpMID = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrExpKID = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrExpAction = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrExpValue = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrFileOut = EMPTY_DRM_STRING;

    DRM_DWORD            cbReqd = 0;
    long                 index = 0;
    long subindex = 0;
    DRM_CONST_STRING dstrListTop = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListData = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrRecords = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKidData = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFallbackCert = EMPTY_DRM_STRING;
    DRM_WCHAR        pwchMeterCert[2000] = {0};

    DRM_CONST_STRING*    pbExpectedData     = NULL;
    DRM_DWORD            dwExpectedDataSize = 0;

    ChkArg( argc > 1 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkDR( TST_UTL_MakePathString( NULL,
                                      argv[0],
                                     &dstrMeterCertInFile ) );

    ChkDR( MakeDRMString(&dstrPrivKey, argv[1]) );

    ChkDR(DRM_B64_DecodeW(&dstrPrivKey,
                         &cbReqd,
                          NULL,
                          DRM_BASE64_DECODE_IN_PLACE));

    ChkDR  (_GetMeterCertFromFile(dstrMeterCertInFile.pwszString, pwchMeterCert));

    dstrMeterCert.cchString  = DRMCRT_wcslen (pwchMeterCert);
    dstrMeterCert.pwszString = pwchMeterCert;


    dr = Drm_Metering_GenerateChallenge (m_pContextManager,
                                         PB_DSTR(&dstrMeterCert),
                                         CB_DSTR(&dstrMeterCert),
                                         NULL,
                                         0,
                                         NULL,
                                         NULL,
                                         NULL,
                                         &cbMeteringData);

    if (dr != DRM_E_BUFFERTOOSMALL)
    {
        if( dr == DRM_SUCCESS)
        {
            ChkDR(DRM_E_LOGICERR);
        }
        ChkDR(dr);
    }

    ChkMem (pbMeteringData = (_XMBContext*)Oem_MemAlloc (cbMeteringData));
    ZEROMEM( pbMeteringData, cbMeteringData );

    ChkDR(Drm_Metering_GenerateChallenge (m_pContextManager,
                                          PB_DSTR(&dstrMeterCert),
                                          CB_DSTR(&dstrMeterCert),
                                          NULL,
                                          0,
                                          NULL,
                                          NULL,
                                          (DRM_BYTE*)pbMeteringData,
                                          &cbMeteringData));

    ChkDR(_GetMeteringDataAsXML(pbMeteringData, cbMeteringData, (PRIVKEY *)dstrPrivKey.pwszString, &dstrMeteringXML));

    ChkDR(DRM_XML_GetNode(&dstrMeteringXML, &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop));
    ChkDR(DRM_XML_GetNode(&dstrListTop, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrListData));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagMID, NULL, NULL, 0, NULL, &dstrMID));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagTID, NULL, NULL, 0, NULL, &dstrTID));
    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagRecords, NULL, NULL, 0, NULL, &dstrRecords));

    if (argc < 3)
    {   // expect no data
        if (dstrRecords.cchString == 0)
            goto ErrorExit; // success
        else
            ChkDR(DRM_E_FAIL);
    }
    else if (dstrRecords.cchString == 0)
    {   // expected data
        ChkDR(DRM_E_FAIL);
    }


    ChkDR(DRM_XML_GetNode(&dstrRecords, &g_dstrTagKID, NULL, NULL, subindex, &dstrKidData, &dstrKID));
    ChkDR(DRM_XML_GetNode(&dstrListTop,  &g_dstrTagCertificate, NULL, NULL, 0, NULL, &dstrFallbackCert));

    if ( argc > 4 )
    {
        ChkMem (pbExpectedData = (DRM_CONST_STRING*)Oem_MemAlloc ( SIZEOF(DRM_CONST_STRING) * (argc - 4) ));
        ZEROMEM( pbExpectedData, SIZEOF(DRM_CONST_STRING) * (argc - 4) );

        for( index = 0; index < argc - 4; index++ )
        {
            ChkDR( MakeDRMString(&pbExpectedData[index], argv[index + 4]) );
        }

        dwExpectedDataSize = argc - 4;
        ChkDR(_VerifyMeterChallenge(pbExpectedData, dwExpectedDataSize, dstrRecords));
    }

    if (argc > 2 && *argv[2])
    {
        PKCERT fallbackCert;
        DRM_DWORD cbB64Decode = 0;
        cbB64Decode = SIZEOF(PKCERT);
        ChkDR(DRM_B64_DecodeW(&dstrFallbackCert, &cbB64Decode, (DRM_BYTE *)&fallbackCert, 0));

        ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrFileOut ) );

        WriteMeterResponse(&dstrFileOut, &dstrMID, &dstrTID, &dstrRecords, (PUBKEY *)&fallbackCert.pk.pk, (PRIVKEY*)dstrPrivKey.pwszString);
    }

ErrorExit:

    FREEDRMSTRING( dstrPrivKey );
    FREEDRMSTRING( dstrFileOut );
    FREEDRMSTRING( dstrMeterCertInFile );
    SAFE_OEM_FREE(pbMeteringData);
    FREEDRMSTRING( dstrExpMID );
    FREEDRMSTRING( dstrExpKID );
    FREEDRMSTRING( dstrExpAction );
    FREEDRMSTRING( dstrExpValue );

    if(( argc > 4) && ( pbExpectedData != NULL ))
    {
        for( index = 0; index < argc - 4; index++ )
        {
            FREEDRMSTRING( pbExpectedData[index] );
        }
    }

    SAFE_OEM_FREE(pbExpectedData);

    return dr;
}


/*
    argv[0] - Metering Certificate file
    argv[1] - file private key
    argv[2] - response file
*/
DRM_RESULT DRM_CALL TestDRM_Manager_ProcessMeterResponse( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr                     = DRM_SUCCESS;
    DRM_DWORD        flagsResponse          = 0;
    DRM_CONST_STRING dstrMeterReponseInFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMeterCertInFile    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSignedReponse      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrMeterCert          = EMPTY_DRM_STRING;
    DRM_WCHAR        pwchMeterCert[2000]    = {0};
    DRM_CONST_STRING dstrPrivKey            = EMPTY_DRM_STRING;
    DRM_DWORD        cbReqd                 = 0;
    DRM_CONST_STRING dstrMeterResp          = EMPTY_DRM_STRING;
    DRM_WCHAR       *pwchMeterResponse      = 0;

    ChkArg( argc > 2 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkDR( TST_UTL_MakePathString( NULL, argv[0], &dstrMeterCertInFile ) );

    ChkDR  (_GetMeterCertFromFile(dstrMeterCertInFile.pwszString, pwchMeterCert));

    dstrMeterCert.cchString  = DRMCRT_wcslen (pwchMeterCert);
    dstrMeterCert.pwszString = pwchMeterCert;

    ChkDR( MakeDRMString(&dstrPrivKey, argv[1]) );

    ChkDR(DRM_B64_DecodeW(&dstrPrivKey,
                         &cbReqd,
                          NULL,
                          DRM_BASE64_DECODE_IN_PLACE));


    ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrMeterReponseInFile ) );

    ChkDR  (_GetMeterResponseFromFile(dstrMeterReponseInFile.pwszString, &pwchMeterResponse));
    ChkArg(pwchMeterResponse);

    dstrMeterResp.cchString  = DRMCRT_wcslen (pwchMeterResponse);
    dstrMeterResp.pwszString = pwchMeterResponse;

    ChkDR(_SignResponse(&dstrMeterResp, &dstrMeterCert, &dstrSignedReponse, (PRIVKEY*)dstrPrivKey.pwszString));

    LogBlob( "", "Processing Meter Response: ", (DRM_BYTE *) PB_DSTR (&dstrSignedReponse), CB_DSTR (&dstrSignedReponse) );
    ChkDR  (Drm_Metering_ProcessResponse (m_pContextManager,
                             (DRM_BYTE *) PB_DSTR (&dstrSignedReponse),
                                          CB_DSTR (&dstrSignedReponse),
                                          &flagsResponse));

ErrorExit:
    FREEDRMSTRING( dstrPrivKey );
    FREEDRMSTRING( dstrMeterCertInFile );
    FREEDRMSTRING( dstrMeterReponseInFile );
    SAFE_OEM_FREE( pwchMeterResponse);

    return dr;
}

/*********************************************************************
**
** Function: TestDRM_CompareMeterCert
**
** Synopsis: Function that compares a meter certificate stored in the
**           meter store against the data in a certificate file,
**           whose file name is passed in as argv[1].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other errors.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_CompareMeterCert( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )m_pContextManager;
    DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;
    DRM_MID oMID = { 0 };
    DRM_DWORD cbData = 0;
    DRM_BYTE *pbMeterCertInFile = NULL;
    DRM_DWORD cbMeterCertInFile = 0;
    DRM_BYTE *pbMeterCertInStore = NULL;
    DRM_DWORD cbMeterCertInStore = 0;

    ChkArg( argc == 2 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    /* argv[0] is the B64 encoded meter ID(MID). */
    ChkDR( MakeDRMString( &dstrMID, argv[ 0 ] ) );

    cbData = SIZEOF( DRM_MID );

    ChkDR( DRM_B64_DecodeW( &dstrMID,
                            &cbData,
                            ( DRM_BYTE * )&oMID,
                            0 ) );

    dr = DRM_MCS_GetMeterCertByMID( &poAppContextInternal->oMeterCertContext,
                                    &oMID,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &cbMeterCertInStore );

    /* Cannot be successful. */
    if ( dr == DRM_SUCCESS )
    {
        dr = DRM_E_FAIL;
    }

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        /* If the return code is other than "buffer too small", exit. */
        ChkDR( dr );
    }

    ChkMem( pbMeterCertInStore =  ( DRM_BYTE * )Oem_MemAlloc( cbMeterCertInStore ) );
    ZEROMEM( pbMeterCertInStore, cbMeterCertInStore );

    ChkDR( DRM_MCS_GetMeterCertByMID( &poAppContextInternal->oMeterCertContext,
                                      &oMID,
                                      NULL,
                                      NULL,
                                      NULL,
                                      pbMeterCertInStore,
                                      &cbMeterCertInStore ) );

    ChkBOOL( LoadTestFile( "files", argv[ 1 ], &pbMeterCertInFile, &cbMeterCertInFile ), DRM_E_FILEREADERROR );

    if ( cbMeterCertInStore != cbMeterCertInFile )
    {
        ChkDR( DRM_E_FAIL );
    }
    else
    {
        ChkDR(DRM_SUCCESS );
    }

    ChkBOOL( MEMCMP( pbMeterCertInStore, pbMeterCertInFile, cbMeterCertInStore ) == 0, DRM_E_FAIL );

ErrorExit:
    SAFE_OEM_FREE( pbMeterCertInFile );
    SAFE_OEM_FREE( pbMeterCertInStore );
    FREEDRMSTRING( dstrMID );
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_UpdateTID
**
** Synopsis: Function that set the current transaction ID (TID) in the
**           meter store to a specific value.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_UpdateTID( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )m_pContextManager;
    DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTID = EMPTY_DRM_STRING;
    DRM_METERING_CONTEXT oMeterStoreContext = { 0 };
    DRM_BYTE rgbSSTPassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_MID oMID = { 0 };
    DRM_MID oTID = { 0 };
    DRM_DWORD cbData = 0;
    DRM_BYTE rgbSlotMinimum[ __CB_DECL( DRM_SST_SLOT_V0_HEADER_SIZE ) ];

    ChkArg( argc == 2 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    /* argv[0] is the B64 encoded meter ID MID). */
    ChkDR( MakeDRMString( &dstrMID, argv[ 0 ] ) );

    /* argv[1] is the B64 encoded transaction ID(TID). */
    ChkDR( MakeDRMString( &dstrTID, argv[ 1 ] ) );

    cbData = SIZEOF( DRM_MID );

    ChkDR( DRM_B64_DecodeW( &dstrMID,
                            &cbData,
                            ( DRM_BYTE * )&oMID,
                            0 ) );

    cbData = SIZEOF( DRM_TID );

    ChkDR( DRM_B64_DecodeW( &dstrTID,
                            &cbData,
                            ( DRM_BYTE * )&oTID,
                            0 ) );

    ChkDR( DRM_BBX_HashValue( PB_DSTR( &dstrMID ),
                              CB_DSTR( &dstrMID ),
                              rgbSSTPassword,
                              &poAppContextInternal->oBlackBoxContext ) );

    ZEROMEM( rgbSlotMinimum, SIZEOF( rgbSlotMinimum ) );
    MEMCPY( rgbSlotMinimum, &oTID, SIZEOF( DRM_TID ) );

    ChkDR( DRM_SST_SetData( &oMeterStoreContext,
                            &oMID,
                            &g_kidTID,
                            rgbSSTPassword,
                            SECURE_STORE_METERING_DATA,
                            &poAppContextInternal->oDatastoreHDS,
                            rgbSlotMinimum,
                            SIZEOF( rgbSlotMinimum ) ) );

ErrorExit:
    return dr;
}

#define METERCHALLENGEV3       L"__MeterChallengeV3.txt"
#define METERRESPONSEV3        L"__MeterResponse.txt"
#define METERRESPONSEV3TPLT    "MeterResponseTemplate.dat"

/*********************************************************************
**
** Function: TestDRM_Manager_GenerateMeterChallengeV3
**
** Synopsis: Function that generates a meter data challenge.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- argv[0] - App Context - "NULL" or "NORMAL"
**                             argv[1] - Metering certificate file
**                             argv[2] - Custom Data
**                             argv[3] - Custom Data Size - Specified size or "NORMAL"
**                             argv[4] - URL size - Specified size or "NORMAL"
**                             argv[5] - Challenge Buffer size - Specified size or "NORMAL"
**                             argv[6] - Private Key [optional]
**                             argv[7] - expected KID [optional]
**                             argv[8] - expected Action [optional]
**                             argv[9] - expected value [optional]
**                             argv[A] - expected KID [optional]
**                             argv[B] - expected Action [optional]
**                             argv[C] - expected value [optional]
**                             ...
**
**
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_Manager_GenerateMeterChallengeV3( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT *pContextManager = NULL;
    DRM_BYTE *pbMeterCert = NULL;
    DRM_DWORD cbMeterCert = 0;
    DRM_CHAR *pchURL = NULL;
    DRM_DWORD cchURL = 0;
    DRM_BOOL fCustomDataIsNull = FALSE;
    DRM_DWORD cchCustomData = 0;
    DRM_CONST_STRING dstrPrivKey = EMPTY_DRM_STRING;

    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
    DRM_CHAR *pbBuffer = NULL;
    DRM_DWORD cbBuffer = 0;

    ChkArg( argc >= 6 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    if ( argv[0] != NULL && (0 == strcmp( argv[0], "NORMAL")) )
    {
        pContextManager = m_pContextManager;
    }

    /* argv[1] is the file that contains the meter certificate. */
    if( argv[ 1 ] != NULL )
    {
        ChkBOOL( LoadTestFile( NULL, argv[ 1 ], &pbMeterCert, &cbMeterCert ), DRM_E_FILEREADERROR );
    }

    /* argv[2] is the custom data, and it can be NULL. */
    if ( argv[ 2 ] == NULL )
    {
        fCustomDataIsNull = TRUE;
    }

    if( argv[3] != NULL && (0 == strcmp( argv[3], "NORMAL")) && !fCustomDataIsNull )
    {
        cchCustomData = ( DRM_DWORD )strlen( argv[ 2 ] );
    }
    else if( argv[3] != NULL )
    {
        cchCustomData = atoi( argv[3] );
    }

    dr = Drm_Metering_GenerateChallenge( pContextManager,
                                         pbMeterCert,
                                         cbMeterCert,
                                         fCustomDataIsNull ? NULL : argv[ 2 ],
                                         cchCustomData,
                                         pchURL,
                                         &cchURL,
                                         NULL,
                                         &cbChallenge );

    /* Cannot be successful. */
    if ( dr == DRM_SUCCESS )
    {
        dr = DRM_E_FAIL;
    }

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        /* If the return code is other than "buffer too small", exit. */
        ChkDR( dr );
    }

    /*
    ** argv[4] is the size of URL buffer specified by the caller.
    ** It may be smaller than the size needed to store the retrieved URL.
    */
    if ( argv[ 4 ] != NULL && (0 != strcmp( argv[4], "NORMAL")) )
    {
        cchURL = atol( argv[ 4 ] );
    }

    /*
    ** argv[5] is the size of challenge buffer specified by the caller.
    ** It may be smaller than the size needed to store the retrieved challenge.
    */
    if ( argv[ 5 ] != NULL && (0 != strcmp( argv[5], "NORMAL")) )
    {
        cbChallenge = atol( argv[ 5 ] );
    }

    if( cchURL > 0 )
    {
        ChkMem( pchURL = ( DRM_CHAR * )Oem_MemAlloc( cchURL ) );
        ZEROMEM( pchURL, cchURL );
    }

    if( cbChallenge > 0 )
    {
        ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );
        ZEROMEM( pbChallenge, cbChallenge );
    }

    ChkDR( Drm_Metering_GenerateChallenge( pContextManager,
                                           pbMeterCert,
                                           cbMeterCert,
                                           fCustomDataIsNull ? NULL : argv[ 1 ],
                                           cchCustomData,
                                           pchURL,
                                           &cchURL,
                                           pbChallenge,
                                           &cbChallenge ) );

    ChkDR( TST_MTR_VerifyChallengeData( pbChallenge, cbChallenge ) );

    if ( argc > 9 )
    {
        ChkDR( MakeDRMString(&dstrPrivKey, argv[6]) );

        ChkDR( TST_MTR_ValidateV3Metering( dstrPrivKey.pwszString,
                                           dstrPrivKey.cchString,
                                           pbChallenge,
                                           cbChallenge,
                                           argc-7,      /* Pass in the args starting with argv[7] */
                                          &(argv[7]) ) );
    }

    ChkDR( SaveToFileW( ( DRM_WCHAR * )METERCHALLENGEV3, pbChallenge, cbChallenge ) );

    /*
    ** BUGBUG 14646: Do not use a template file to create the response.
    **               This should be 'if ( argc >= 7 )' after BUG 14646 get fixed
    */
    if ( argc == 7 )
    {
        ChkArg( argv[6] != NULL );

        ChkDR( MakeDRMString(&dstrPrivKey, argv[6]) );

        ChkBOOL( LoadTestFile( "files", METERRESPONSEV3TPLT, (DRM_BYTE **)&pbBuffer, &cbBuffer ), DRM_E_FILEREADERROR );

        ChkDR( TST_MTR_GenerateV3Response( dstrPrivKey.pwszString,
                                           dstrPrivKey.cchString,
                                           pbChallenge,
                                           cbChallenge,
                             (DRM_BYTE **)&pbBuffer,
                                          &cbBuffer ) );

        ChkDR( SaveToFileW( ( DRM_WCHAR * )METERRESPONSEV3, ( DRM_BYTE * ) pbBuffer, cbBuffer ) );
    }

    ChkDR( DRM_SUCCESS );

ErrorExit:
    FREEDRMSTRING( dstrPrivKey );
    SAFE_OEM_FREE( pchURL );
    SAFE_OEM_FREE( pbChallenge );
    SAFE_OEM_FREE( pbMeterCert );
    SAFE_OEM_FREE( pbBuffer );

    return dr;
}

/*********************************************************************
**
** Function: TestDRM_Manager_GeneratePartialMeterChallengeV3
**
** Synopsis: Function that generates a partial meter data challenge
**           using a buffer whose size is specified by the caller.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL DRM_API TestDRM_Manager_GeneratePartialMeterChallengeV3( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr                     = DRM_SUCCESS;
    DRM_BYTE   *pbMeterCert            = NULL;
    DRM_DWORD   cbMeterCert            = 0;
    DRM_CHAR    rgchURL[ 1024 ]        = { 0 };
    DRM_DWORD   cchURL                 = 1024;
    DRM_BOOL    fCustomDataIsNull      = FALSE;
    DRM_DWORD   cchCustomData          = 0;
    DRM_DWORD   cbActualChallenge      = 0;
    DRM_BYTE   *pbChallenge            = NULL;
    DRM_DWORD   cbChallenge            = 0;

    ChkArg( argc == 3 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    /* argv[0] is the file that contains the meter certificate. */
    ChkBOOL( LoadTestFile( "files", argv[ 0 ], &pbMeterCert, &cbMeterCert ), DRM_E_FILEREADERROR );

    /* argv[1] is the custom data, and it can be NULL. */
    if ( argv[ 1 ] == NULL )
    {
        fCustomDataIsNull = TRUE;
    }
    else
    {
        cchCustomData = ( DRM_DWORD )strlen( argv [ 1 ] );
    }

    /*
    ** argv[2] is the size of buffer specified by the caller.
    ** It may be smaller than the size needed to store meter
    ** data of all KIDs.
    */
    cbActualChallenge = atol( argv[ 2 ] );

    dr = Drm_Metering_GenerateChallenge( m_pContextManager,
                                         pbMeterCert,
                                         cbMeterCert,
                                         fCustomDataIsNull ? NULL : argv[ 1 ],
                                         cchCustomData,
                                         rgchURL,
                                         &cchURL,
                                         NULL,
                                         &cbChallenge );

    /* Cannot be successful. */
    if ( dr == DRM_SUCCESS )
    {
        dr = DRM_E_FAIL;
    }

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        /* If the return code is other than "buffer too small", exit. */
        ChkDR( dr );
    }

    ChkBOOL( cchURL <= 1024,  DRM_E_TEST_UNEXPECTED_OUTPUT );

    cbChallenge = min( cbChallenge, cbActualChallenge );

    ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );
    ZEROMEM( pbChallenge, cbChallenge );

    ChkDR( Drm_Metering_GenerateChallenge( m_pContextManager,
                                           pbMeterCert,
                                           cbMeterCert,
                                           fCustomDataIsNull ? NULL : argv[ 1 ],
                                           cchCustomData,
                                           rgchURL,
                                           &cchURL,
                                           pbChallenge,
                                           &cbChallenge ) );

ErrorExit:
    SAFE_OEM_FREE( pbChallenge );
    SAFE_OEM_FREE( pbMeterCert );
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_Manager_ProcessMeterResponseV3
**
** Synopsis: Function that processes a meter data response contained
**           in a file. The file name is stored in argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- argv[0] - App Context - NULL or NORMAL
**                             argv[1] - Meter Response - Specified file or NULL
**                             argv[2] - Meter Resposne size - Specified or NORMAL
**                             argv[3] - expected number of meter entries
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL DRM_API TestDRM_Manager_ProcessMeterResponseV3( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT *pContextManager = NULL;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;
    DRM_RESULT dr1 = DRM_SUCCESS;
    DRM_MID oMID = { 0 };

#if DRM_TEST_SUPPORT_NET_IO
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
#endif /* DRM_TEST_SUPPORT_NET_IO */

    ChkArg( argc >= 4);

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    if ( argv[0] != NULL && (0 == strcmp( argv[0], "NORMAL")) )
    {
        pContextManager = m_pContextManager;
    }

    if( argv[1] != NULL )
    {
        if ( DRMCRT_strncmp( argv[1], "http://", 7 ) == 0 )
        {
#if DRM_TEST_SUPPORT_NET_IO
            ChkBOOL( LoadTestFileW( ( DRM_WCHAR * )L"",
                                    ( DRM_WCHAR * )METERCHALLENGEV3,
                                    &pbChallenge,
                                    &cbChallenge ), DRM_E_FILEREADERROR );

            ChkDR( tOEM_SendNetData( argv[1],
                                     eDRM_TEST_NET_METERDATA,
                                     pbChallenge,
                                     cbChallenge,
                                     &pbResponse,
                                     &cbResponse ) );
#else
        ChkDR( DRM_E_NOTIMPL );
#endif /* DRM_TEST_SUPPORT_NET_IO */
        }
        else
        {
            ChkBOOL( LoadTestFile( "files", argv[ 1 ], &pbResponse, &cbResponse ), DRM_E_FILEREADERROR );
        }
    }

    if( argv[2] != NULL && (0 != strcmp( argv[2], "NORMAL")) )
    {
        cbResponse = atol( argv[2] );
    }

    ChkDR( _GetMIDFromMeterResponse( pbResponse,
                                     cbResponse,
                                     &oMID ) );

    ChkDR( Drm_Metering_ProcessResponse( pContextManager,
                                         pbResponse,
                                         cbResponse,
                                         ( DRM_DWORD * )&dr1 ) );

    if( argc > 4 && argv[4] != NULL )
    {
        ChkBOOL( strtol( argv[4], NULL, 0 ) == dr1, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    ChkDR( _VerifyMeterState( &oMID,
                              atoi(argv[3]) ) );

ErrorExit:

#if DRM_TEST_SUPPORT_NET_IO
    SAFE_OEM_FREE( pbChallenge );
#endif /* DRM_TEST_SUPPORT_NET_IO */

    SAFE_OEM_FREE( pbResponse );
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_MTR_GenerateMeterCertChallenge
**
** Synopsis: Function that generates a meter certificate challenge
**
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_MTR_GenerateMeterCertChallenge( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;
    DRM_MID oMID = { 0 };
    DRM_DWORD cbData = 0;
    DRM_BOOL fMIDIsNull = FALSE;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;

    ChkArg( argc == 2 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    if( argv[0] == NULL )
    {
        fMIDIsNull = TRUE;
    }
    else
    {

        /* argv[0] is the B64 encoded meter ID(MID). */
        ChkDR( MakeDRMString( &dstrMID, argv[ 0 ] ) );

        cbData = SIZEOF( DRM_TID );

        ChkDR( DRM_B64_DecodeW( &dstrMID,
                            &cbData,
                            ( DRM_BYTE * )&oMID,
                            0 ) );
    }

    dr = Drm_MeterCert_GenerateChallenge( m_pContextManager,
                                          fMIDIsNull ? NULL : &oMID,
                                          NULL,
                                          0,
                                          NULL,
                                          &cbChallenge );

    /* Cannot be successful. */
    if ( dr == DRM_SUCCESS )
    {
        dr = DRM_E_FAIL;
    }

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        /* If the return code is other than "buffer too small", exit. */
        ChkDR( dr );
    }

    ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );
    ZEROMEM( pbChallenge, cbChallenge );

    ChkDR( Drm_MeterCert_GenerateChallenge( m_pContextManager,
                                            &oMID,
                                            NULL,
                                            0,
                                            pbChallenge,
                                            &cbChallenge ) );

ErrorExit:
    SAFE_OEM_FREE( pbChallenge );
    FREEDRMSTRING( dstrMID );
    return dr;
}

/*********************************************************************
**
** Function: TestDRM_MTR_ProcessMeterCertResponse
**
** Synopsis: Function that processes a meter certificate response contained
**           in a file. The file name is stored in argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_MTR_ProcessMeterCertResponse( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;
    DRM_RESULT dr1 = DRM_SUCCESS;

    ChkArg( argc >= 1 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    /* argv[0] is the name of the file that contains the response. */
    ChkBOOL( LoadTestFile( "files", argv[ 0 ], &pbResponse, &cbResponse ), DRM_E_FILEREADERROR );

    ChkDR( Drm_MeterCert_ProcessResponse( m_pContextManager,
                                             pbResponse,
                                             cbResponse,
                                             &dr1 ) );

    if( argc > 1 && argv[1] != NULL )
    {
        ChkBOOL( strtol( argv[1], NULL, 0 ) == dr1, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    SAFE_OEM_FREE( pbResponse );
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_Manager_Initialize( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(Metering)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    GET_SHARED_APP_CONTEXT( m_pContextManager );

ErrorExit:

    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(Metering)(long lTCID, const char *strTCName)
{
    RemoveDRMFile(RMFILE_STORE);

    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST(Metering)
    API_ENTRY(TestDRM_Manager_GenerateMeterChallenge)
    API_ENTRY(TestDRM_Manager_ProcessMeterResponse)
    API_ENTRY(TestDRM_Manager_Initialize)
    API_ENTRY(TestDRM_Manager_AddMeteringData)
    API_ENTRY(TestDRM_Manager_AddManyMeteringData)
    API_ENTRY(TestDRM_Manager_GeneratePartialMeterChallenge)
    API_ENTRY(TestDRM_VerifyTID)
    API_ENTRY(TestDRM_CompareMeterCert)
    API_ENTRY(TestDRM_MTR_GenerateMeterCertChallenge)
    API_ENTRY(TestDRM_MTR_ProcessMeterCertResponse)
    API_ENTRY(TestDRM_UpdateTID)
    API_ENTRY(TestDRM_Manager_GenerateMeterChallengeV3)
    API_ENTRY(TestDRM_Manager_GeneratePartialMeterChallengeV3)
    API_ENTRY(TestDRM_Manager_ProcessMeterResponseV3)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
