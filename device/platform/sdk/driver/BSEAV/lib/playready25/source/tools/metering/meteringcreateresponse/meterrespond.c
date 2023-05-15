/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h> /* for missing CRT functions like printf */

#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcrt.h>
#include <drmcmdlnpars.h>
#include <drmxmlparser.h>
#include <drmxmlbuilder.h>
#include <drmtoolsutils.h>
#include <drmmeterapi.h>
#include <drmmeterconstants.h>
#include <oemeccp160.h>
#include <drmconstants.h>

/*************** CHALLENGE *************
   <METERDATA type="challenge">
      <DATA>
         <MID>UiW5yBMep2CuevGg5+FgA3==</MID>
         <TID>Mep2CuevGgUiW5yB5+FgA3==</TID>
         <RECORDS>
            <KID value="KID1">
               <ACTION value="Play">5</ACTION>
            </KID>
            <KID value="KID2">
               <ACTION value="Play">71</ACTION>
               <ACTION value="Burn">2</ACTION>
            </KID>
            ...
         </RECORDS>
         <PARTIALDATA>1</PARTIALDATA>
      </DATA>
      <MSDRM_SIGNATURE_VALUE>
         dW1*kFht1AFwIH!VA4PjjHoV4jN2K6Jksga1FXMzaePVi6ISfOEIEA==
      </MSDRM_SIGNATURE_VALUE>
      <CERTIFICATE private="1">
         TEALKodF8A70IWtir0fcZLgNuITbAj!Y4Zmja9Xc4U8DuFCCKrwdewIDAAKnU3O11yi
         dK4YXBaHPLZifoE4wf9KbKccf50fUzaIcjFArH2KH*1BO
      </CERTIFICATE>
   </METERDATA>


************** RESPONSE *************

   <METERDATA type="response">
      <DATA>
         <MID>UiW5yBMep2CuevGg5+FgA3==</MID>
         <TID>Mep2CuevGgUiW5yB5+FgA3==</TID>
         <COMMAND>RESET</COMMAND>
         <RECORDS>
           <DATA>
             <KID value="KID1" />
             <KID value="KID2" />
              ...
           </DATA>
         </RECORDS>
         <PARTIALDATA>1</PARTIALDATA>
      </DATA>
      <MSDRM_SIGNATURE_VALUE>
         4PjjHoV4jN2K6Jksga1FXMzaePVi6ISfOEIEA dW1*kFht1AFwIH!VA==
      </MSDRM_SIGNATURE_VALUE>
      <METERCERT version="1.0">
         <DATA>
            <MID>UiW5yBMep2CuevGg5+FgA3==</MID>
            <PUBLICKEY>meter cert public key</PUBLICKEY>
            <URL>http://www.bad.contoso.com</URL>
         </DATA>
         <SIGNATURE>
            ...
         </SIGNATURE>
         <CERTIFICATECHAIN>
            <CERTIFICATE>
               ...
            </CERTIFICATE>
            <CERTIFICATE>
               ...
            </CERTIFICATE>
         </CERTIFICATECHAIN>
      </METERCERT>
   </METERDATA>
*/


const PUBKEY pubkeyRoot =
{
    0x05, 0x9B, 0xB0, 0x4F, 0xE1, 0x58, 0xBC, 0x27,
    0x47, 0x63, 0xD4, 0x52, 0x73, 0xCB, 0xFA, 0x36,
    0xF8, 0xA2, 0x6B, 0x4A, 0xA6, 0xCA, 0xE0, 0xB1,
    0xC0, 0x84, 0xD6, 0x62, 0xC9, 0xBF, 0xB8, 0x2D,
    0xC8, 0x67, 0x8A, 0x35, 0xCA, 0xB5, 0x02, 0x13
};

DRM_BOOL GetFileData(
    const DRM_WCHAR *pszFileData,
    DRM_DWORD       *pcbFile,
    DRM_WCHAR      **ppwszFile)
{
    DRM_BOOL    bOK     = FALSE;
    DRM_DWORD   cbRead  = 0;
    OEM_FILEHDL hfileIn = OEM_INVALID_HANDLE_VALUE;

    if ( ( hfileIn = Oem_File_Open ( NULL,
                                 pszFileData,
                                 OEM_GENERIC_READ,
                                 OEM_FILE_SHARE_NONE,
                                 OEM_OPEN_EXISTING,
                                 OEM_ATTRIBUTE_NORMAL ) ) != OEM_INVALID_HANDLE_VALUE)
    {
        /* read the input file and cache it */

        Oem_File_GetSize (hfileIn, pcbFile);

        if ( ( *ppwszFile = (DRM_WCHAR *)Oem_MemAlloc (*pcbFile) ) != NULL )
        {
            if (Oem_File_Read (hfileIn, *ppwszFile, *pcbFile, &cbRead))
            {
                bOK = TRUE;
            }
            else
            {
                Oem_MemFree (*ppwszFile);
                *ppwszFile = NULL;
                printf ("error reading file\n");
            }
        }
        else
        {
            printf ("error allocating memory\n");
        }

        Oem_File_Close (hfileIn);
    }
    else
    {
        printf ("error opening input file %S\n", pszFileData);
    }

    return bOK;
}



DRM_BOOL GetMASPrivateKey(
    const DRM_WCHAR *pszFilePrivKey,
    PRIVKEY         *pprivkey)
{
    DRM_DWORD        cbFile       = 0,
                     cbRead       = 0,
                     cbPrivKey    = SIZEOF (PRIVKEY);
    DRM_BOOL         bOK          = FALSE;
    DRM_BYTE        *pbFile       = NULL;
    DRM_CONST_STRING dstrXML      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKey  = EMPTY_DRM_STRING;
    OEM_FILEHDL      hfilePrivKey = OEM_INVALID_HANDLE_VALUE;

    if ((hfilePrivKey = Oem_File_Open (NULL,
                                      pszFilePrivKey,
                                      OEM_GENERIC_READ,
                                      OEM_FILE_SHARE_NONE,
                                      OEM_OPEN_EXISTING,
                                      OEM_ATTRIBUTE_NORMAL)) != OEM_INVALID_HANDLE_VALUE)
    {
        if (Oem_File_GetSize (hfilePrivKey, &cbFile))
        {
            if ((pbFile = (DRM_BYTE *)Oem_MemAlloc (cbFile)) != NULL)
            {
                if (Oem_File_Read (hfilePrivKey, pbFile, cbFile, &cbRead))
                {
                    DRM_RESULT dr = DRM_SUCCESS;

                    DSTR_FROM_PB(&dstrXML, pbFile, cbFile);

                    ChkDR(DRM_XML_GetNode(&dstrXML,
                                          &g_dstrTagPrivateKey,
                                           NULL,
                                           NULL,
                                           0,
                                           NULL,
                                          &dstrPrivKey));

                    ChkDR(DRM_B64_DecodeW(&dstrPrivKey,
                                          &cbPrivKey,
                              (DRM_BYTE *) pprivkey,
                                           0));

                    bOK = TRUE;
                }
ErrorExit:
                Oem_MemFree (pbFile);
            }
            else
            {
                printf ("error allocating memory\n");
            }
        }

        Oem_File_Close (hfilePrivKey);
    }
    else
    {
        printf ("error opening private key file %S\n", pszFilePrivKey);
    }

    return bOK;
}

/*******************************************************************
*                    PRIVATE FUNCTION DoSetup
*
* purpose: set up everything needed; open both files and allocate
*          a buffer to image the input file.  If anything fails,
*          release all resources
*******************************************************************/

static DRM_BOOL DoSetup(
    const DRM_WCHAR  *pszFilePrivKey,
    const DRM_WCHAR  *pszFileChallenge,
    const DRM_WCHAR  *pszFileResponse,
    const DRM_WCHAR  *pszFileMeterCert,
    PRIVKEY          *pprivkey,
    OEM_FILEHDL      *phfileOut,
    DRM_WCHAR       **ppwszFile,
    DRM_DWORD        *pcbFile,
    DRM_WCHAR       **ppszCertFile,
    DRM_DWORD        *pcbCertFile)
{
    DRM_BOOL    fOK          = FALSE;

    *phfileOut = OEM_INVALID_HANDLE_VALUE;

    if (GetMASPrivateKey     (pszFilePrivKey,   pprivkey)
    &&  GetFileData (pszFileChallenge, pcbFile, ppwszFile)
    &&  GetFileData (pszFileMeterCert, pcbCertFile, ppszCertFile))
    {
        if ((*phfileOut = Oem_File_Open(NULL,
                                       pszFileResponse,
                                       OEM_GENERIC_WRITE,
                                       OEM_FILE_SHARE_NONE,
                                       OEM_CREATE_ALWAYS,
                                       OEM_ATTRIBUTE_NORMAL)) != OEM_INVALID_HANDLE_VALUE)
        {
            fOK = TRUE;
        }
        else
        {
            Oem_MemFree(*ppwszFile);
            printf ("error opening output file %S\n", pszFileResponse);
        }
    }

    return fOK;
} /* DoSetup */

/*******************************************************************
*               PRIVATE FUNCTION ParseMeterChallenge
*
* purpose: verify that all required entries are present in the
*          UNICODE XML and that they are properly formed
*          write the file to the passed-in buffer as UNICODE
*******************************************************************/

static DRM_RESULT ParseMeterChallenge(
    const DRM_WCHAR        *pwszFile,
          DRM_DWORD         cchFile,
          DRM_CONST_STRING *pdstrMID,
          DRM_CONST_STRING *pdstrTID,
          DRM_CONST_STRING *pdstrRecords,
          PUBKEY           *ppubkeyEnceypt,
    const PRIVKEY          *pprivkeyMAS,
          DRM_BOOL         *pfPartialData)
{
    DRM_CRYPTO_CONTEXT *pcontextCrypto   = NULL;
    DRM_WCHAR          *pszDataCopy      = NULL;
    DRM_RESULT          dr               = DRM_SUCCESS;
    DRM_CONST_STRING    dstrXML          = EMPTY_DRM_STRING,
                        dstrNodeData     = EMPTY_DRM_STRING,
                        dstrListData     = EMPTY_DRM_STRING,
                        dstrSignature    = EMPTY_DRM_STRING,
                        dstrPartialData  = EMPTY_DRM_STRING,
                        dstrListTop      = EMPTY_DRM_STRING,
                        dstrTemp         = EMPTY_DRM_STRING,
                        dstrFallbackCert = EMPTY_DRM_STRING;
    PKCERT              fallbackCert;
    DRM_DWORD           cbB64Decode=0;
    DRM_BYTE            rgbSignature[__CB_DECL(DRM_ECC160_SIGNATURE_LEN)];

    ChkMem(pcontextCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc(SIZEOF (DRM_CRYPTO_CONTEXT)));
    ZEROMEM( pcontextCrypto, SIZEOF(DRM_CRYPTO_CONTEXT) );

    dstrXML.pwszString   = pwszFile;
    dstrXML.cchString    = cchFile;
    *pfPartialData       = FALSE;

    ChkDR(DRM_XML_GetNode(&dstrXML, &g_dstrTagMetering, NULL, NULL, 0, NULL, &dstrListTop));

    /* get <DATA> */
    ChkDR(DRM_XML_GetNode(&dstrListTop,  &g_dstrTagData,        NULL, NULL, 0, &dstrNodeData, &dstrListData));
    /* verify the signature over the <DATA> tags against the secure
    ** store key sent to the MAS in the challenge
    */

    /* get <MSDRM_SIGNATURE_VALUE> */
    ChkDR(DRM_XML_GetNode(&dstrListTop,  &g_dstrTagMSDRMSignature, NULL, NULL, 0, NULL, &dstrSignature));
    if (dstrSignature.cchString != DRM_ECC160_SIGNATURE_B64LEN)
    {
        printf("SIGNATURE string malformed\n");
        ChkDR(DRM_E_INVALIDARG);
    }
    cbB64Decode = __CB_DECL(DRM_ECC160_SIGNATURE_LEN);
    ChkDR(DRM_B64_DecodeW(&dstrSignature, &cbB64Decode, rgbSignature, 0));

    /* get <CERTIFICATE private="1"> */
    ChkDR(DRM_XML_GetNode(&dstrListTop,  &g_dstrTagCertificate, NULL, NULL, 0, NULL, &dstrFallbackCert));

    if (dstrFallbackCert.cchString != CCH_BASE64_EQUIV(SIZEOF(PKCERT)))
    {
        printf("CERTIFICATE string malformed\n");

        ChkDR(DRM_E_INVALIDARG);
    }

    cbB64Decode = SIZEOF(PKCERT);

    ChkDR(DRM_B64_DecodeW(&dstrFallbackCert, &cbB64Decode, (DRM_BYTE *)&fallbackCert, 0));

    /* verify fallbackCert with pubkeyRoot */

    if ( DRM_FAILED( DRM_PK_Verify( pcontextCrypto->rgbCryptoContext,
                                    eDRM_ECC_P160,
                                    DRM_ECC160_PUBLIC_KEY_LEN,
                                    ( const DRM_BYTE * )&pubkeyRoot,
                                    SIZEOF( PKV ),
                                    ( const DRM_BYTE * )&fallbackCert.pk,
                                    DRM_ECC160_SIGNATURE_LEN,
                                    fallbackCert.sign ) ) )
    {
        ChkDR( DRM_E_CH_BAD_KEY );
    }

    /* verify signature of <DATA> node with pubkey in fallback cert */
    if ( DRM_FAILED( DRM_PK_Verify(pcontextCrypto->rgbCryptoContext,
                     eDRM_ECC_P160,
                     DRM_ECC160_PUBLIC_KEY_LEN,
                     ( const DRM_BYTE * )&fallbackCert.pk.pk,
                      CB_DSTR(&dstrNodeData),
                      PB_DSTR(&dstrNodeData),
                      DRM_ECC160_SIGNATURE_LEN,
                      rgbSignature)) )
    {
        ChkDR(DRM_E_CH_BAD_KEY);
    }

    /*Copy the pubkey*/

    MEMCPY((DRM_BYTE *) ppubkeyEnceypt, (DRM_BYTE *) &fallbackCert.pk.pk, SIZEOF(PUBKEY));

    /* get <MID> */

    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagMID,  NULL, NULL, 0, NULL, pdstrMID));

    if (pdstrMID->cchString != CCH_BASE64_EQUIV (SIZEOF (DRM_MID)))
    {
        printf("MID string malformed\n");
        ChkDR(DRM_E_CH_BAD_KEY);
    }

    /* get <TID> */

    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagTID, NULL, NULL, 0, NULL, pdstrTID));

    if (pdstrTID->cchString != CCH_BASE64_EQUIV (SIZEOF (DRM_TID)))
    {
        printf("TID string malformed\n");
        ChkDR(DRM_E_CH_BAD_KEY);
    }

    /* get <PARTIALDATA> */

    dr  = DRM_XML_GetNode(&dstrListData, &g_dstrTagPartialData, NULL, NULL, 0, NULL, &dstrPartialData);

    if (DRM_SUCCEEDED (dr)
     && dstrPartialData.cchString      == 1
     && dstrPartialData.pwszString [0] == g_wch1)
    {
        *pfPartialData = TRUE;
    }

    ChkBOOL(dstrNodeData.cchString!=0, DRM_E_FAIL);
    ChkOverflow(dstrNodeData.cchString * SIZEOF(DRM_WCHAR), dstrNodeData.cchString);
    ChkMem(pszDataCopy = (DRM_WCHAR *)Oem_MemAlloc(dstrNodeData.cchString * SIZEOF(DRM_WCHAR)));
    MEMCPY(pszDataCopy, PB_DSTR(&dstrNodeData), dstrNodeData.cchString * SIZEOF(DRM_WCHAR));

    dstrNodeData.pwszString = pszDataCopy;

    ChkDR(DRM_XML_GetNode(&dstrListData, &g_dstrTagRecords, NULL, NULL, 0, NULL, &dstrTemp));
    if (dstrTemp.cchString > 0)
    {
        /* decrypt the <RECORDS> section with the MAS' private key */

        ChkDR(DRM_XML_GetAndDecryptNode(&dstrListData,
                                        &g_dstrTagRecords,
                                         NULL,
                                         NULL,
                                         0,
                                         pcontextCrypto,
                                         pprivkeyMAS,
                                         NULL,
                                         pdstrRecords));

    }
    else
    {
        printf("This metering challenge does not have any records\n");
        pdstrRecords->cchString  = 0;
        pdstrRecords->pwszString = NULL;
        ChkDR(DRM_E_INVALIDARG);
   }

ErrorExit:

    if (pszDataCopy != NULL)
    {
        Oem_MemFree (pszDataCopy);
    }

    if (pcontextCrypto != NULL)
    {
        Oem_MemFree (pcontextCrypto);
    }

    return dr;
}

/*******************************************************************
*               PRIVATE FUNCTION ParseMeterCert
*
* purpose: verify that all required entries are present in the
*          UNICODE XML and that they are properly formed
*          write the file to the passed-in buffer as UNICODE
*******************************************************************/

static DRM_RESULT ParseMeterCert(
    const DRM_WCHAR        *pwszFile,
          DRM_DWORD         cchFile,
          DRM_CONST_STRING *pdstrMID,
    const PRIVKEY          *pprivkey)
{
    DRM_LICEVAL_CONTEXT contextLEVL      = { 0 };
    DRM_BB_CONTEXT     *pcontextBBX      = NULL;
    DRM_RESULT          dr               = DRM_SUCCESS;
    DRM_CONST_STRING    dstrXML          = EMPTY_DRM_STRING,
                        dstrPubkey       = EMPTY_DRM_STRING,
                        dstrNodeData     = EMPTY_DRM_STRING,
                        dstrListData     = EMPTY_DRM_STRING,
                        dstrNodeSign     = EMPTY_DRM_STRING,
                        dstrNodeCert     = EMPTY_DRM_STRING,
                        dstrNodeRoot     = EMPTY_DRM_STRING;


    ChkMem(pcontextBBX = (DRM_BB_CONTEXT *)Oem_MemAlloc(SIZEOF(DRM_BB_CONTEXT)));
    ZEROMEM( pcontextBBX, SIZEOF(DRM_BB_CONTEXT) );

    contextLEVL.pcontextBBX = pcontextBBX;

    dstrXML.pwszString   = pwszFile;
    dstrXML.cchString    = cchFile;

    ChkDR(DRM_XML_GetNode(&dstrXML,         &g_dstrTagMeterCert,        NULL, NULL, 0, NULL,          &dstrNodeRoot));
    ChkDR(DRM_XML_GetNode(&dstrNodeRoot,    &g_dstrTagData,             NULL, NULL, 0, &dstrNodeData, &dstrListData));
    ChkDR(DRM_XML_GetNode(&dstrNodeRoot,    &g_dstrTagSignature,        NULL, NULL, 0, &dstrNodeSign, NULL));
    ChkDR(DRM_XML_GetNode(&dstrNodeRoot,    &g_dstrTagCertificateChain, NULL, NULL, 0, &dstrNodeCert, NULL));

    ChkDR( DRM_UTL_VerifyXMLSignature( &dstrNodeData, &dstrNodeSign, &dstrNodeCert, FALSE, TRUE, WM_DRM_ROOTPUBKEY_MTR, &contextLEVL ) );

    ChkDR(DRM_XML_GetNode(&dstrListData,    &g_dstrTagMID,              NULL, NULL, 0, NULL, pdstrMID));

    ChkDR(DRM_XML_GetNode(&dstrListData,    &g_dstrTagDataPubKey,       NULL, NULL, 0, NULL, &dstrPubkey));

    /*Test the pubkey in Meter cert against the given private key to sign*/

    if (pprivkey)
    {
        DRM_BYTE    *pInbuf         = NULL;
        DRM_BYTE    *pOutbuf        = NULL;
        DRM_BYTE    *pEncryptedbuf  = NULL;
        DRM_DWORD    cbBuffer       = SIZEOF(PUBKEY);
        DRM_DWORD    cbTemp         = DRM_ECC160_CIPHERTEXT_LEN;
        ChkDR(DRM_B64_DecodeW(&dstrPubkey, &cbBuffer,(DRM_BYTE*) &(pcontextBBX->CryptoContext.pubKey), 0) );

        DRMCASSERT(DRM_ECC160_PLAINTEXT_LEN <= SIZEOF(pcontextBBX->CryptoContext.pubKey2));

        pInbuf = (DRM_BYTE *)&(pcontextBBX->CryptoContext.pubKey2);
        MEMSET(pInbuf, 'a', SIZEOF(PUBKEY));
        DRMCASSERT(DRM_ECC160_CIPHERTEXT_LEN <= SIZEOF(pcontextBBX->CryptoContext.union_cert.cert ) );

        pEncryptedbuf =  (DRM_BYTE *)&(pcontextBBX->CryptoContext.union_cert.cert);
        pOutbuf = (DRM_BYTE *)&(pcontextBBX->CryptoContext.signature);

        ChkDR( DRM_PK_Encrypt( NULL,
                               &(pcontextBBX->CryptoContext.rgbCryptoContext ),
                               eDRM_ECC_P160,
                               DRM_ECC160_PUBLIC_KEY_LEN,
                               ( const DRM_BYTE * )&(pcontextBBX->CryptoContext.pubKey),
                               DRM_ECC160_PLAINTEXT_LEN,
                               pInbuf,
                               &cbTemp,
                               pEncryptedbuf));

        cbTemp = SIZEOF(pcontextBBX->CryptoContext.pubKey2);
        ChkDR( DRM_PK_Decrypt( &pcontextBBX->CryptoContext.rgbCryptoContext,
                               eDRM_ECC_P160,
                               DRM_ECC160_PRIVATE_KEY_LEN,
                               ( const DRM_BYTE * )pprivkey,
                               DRM_ECC160_CIPHERTEXT_LEN,
                               pEncryptedbuf,
                               &cbTemp,
                               pOutbuf ) );

        if (MEMCMP(pInbuf, pOutbuf, cbTemp) !=0)
        {
            ChkDR (DRM_E_KEY_MISMATCH);
        }
    }


ErrorExit:
    if (pcontextBBX != NULL)
    {
        Oem_MemFree (pcontextBBX);
    }

    return dr;
}


static DRM_DWORD _RequiredCharsForResponse(
    const DRM_CONST_STRING *pdstrRecords,
          DRM_BOOL          fPartialData,
          DRM_DWORD        *pcchKIDs)
{
    DRM_DWORD  cchRequired = 25;
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
          OEM_FILEHDL       hfileIn,
    const DRM_CONST_STRING *pdstrMID,
    const DRM_CONST_STRING *pdstrTID,
    const DRM_CONST_STRING *pdstrRecords,
    const PUBKEY           *ppubkeEncrypt,
          DRM_BOOL          fPartialData,
    const DRM_WCHAR        *pwszCertFile,
          DRM_DWORD         cchCertFile,
    const PRIVKEY          *pSignKey)
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_DWORD        cbOut      = 0;
    DRM_DWORD        cbWritten  = 0;
    DRM_DWORD        cchKIDs    = 0;
    DRM_DWORD        cbBuffer   = 0;
    DRM_DWORD        cbSignature= DRM_ECC160_SIGNATURE_LEN;
    DRM_DWORD        iKID       = 0;
    DRM_STRING       strXML     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKID    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKIDB64 = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSign   = EMPTY_DRM_STRING;
    DRM_STRING       dstrClose  = EMPTY_DRM_STRING;
    _XMBContext     *pbOut      = NULL;
    DRM_WCHAR       *pwszOut    = NULL;
    DRM_CRYPTO_CONTEXT *pcontextCrypto   = NULL;

    ChkMem(pcontextCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc(SIZEOF (DRM_CRYPTO_CONTEXT)));
    ZEROMEM( pcontextCrypto, SIZEOF(DRM_CRYPTO_CONTEXT) );

    ChkOverflow( _RequiredCharsForResponse(pdstrRecords, fPartialData, &cchKIDs) + cchCertFile,
                 _RequiredCharsForResponse(pdstrRecords, fPartialData, &cchKIDs) );
    ChkOverflow( SIZEOF(DRM_WCHAR) * (_RequiredCharsForResponse(pdstrRecords, fPartialData, &cchKIDs) + cchCertFile),
                 _RequiredCharsForResponse(pdstrRecords, fPartialData, &cchKIDs) + cchCertFile );
    cbOut = SIZEOF(DRM_WCHAR) * ( _RequiredCharsForResponse(pdstrRecords, fPartialData, &cchKIDs ) + cchCertFile );

    if (cchKIDs ==0)
    {
        printf("This challenge does not have any KIDS\n");
        ChkDR(DRM_E_INVALIDARG);
    }

    ChkMem(pbOut = (_XMBContext *)Oem_MemAlloc(cbOut));

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
             ||  dr == DRM_E_XMLNOTFOUND)
        {
            ChkDR(DRM_SUCCESS);  /* This sets 'dr' and keeps the PREfast tool happy. */
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
    ChkDR(DRM_XMB_CloseCurrNode(pbOut,  &dstrClose));

    /*Sign it with Signing key*/

    ChkDR(DRM_PK_Sign(pcontextCrypto->rgbCryptoContext,
                      eDRM_ECC_P160,
                      DRM_ECC160_PRIVATE_KEY_LEN,
                      ( const DRM_BYTE * )pSignKey,
                      CB_DSTR(&dstrClose),
                      PB_DSTR(&dstrClose),
                      &cbSignature,
                      pcontextCrypto->union_cert.cert.sign));

    /*Base64 encode signature*/
    cbBuffer = DRM_ECC160_SIGNATURE_B64LEN;

    ChkDR(DRM_B64_EncodeW((DRM_BYTE *)pcontextCrypto->union_cert.cert.sign,
                           cbSignature,
                           (DRM_WCHAR *) pcontextCrypto->rgbCryptoContext,
                           &cbBuffer,
                           0));
    /*Write to tag*/

    DSTR_FROM_PB(&dstrSign, pcontextCrypto->rgbCryptoContext, cbBuffer * SIZEOF(DRM_WCHAR));

    ChkDR(DRM_XMB_WriteTag(pbOut, &g_dstrTagMSDRMSignature, &dstrSign, NULL, NULL, wttClosed));

    DSTR_FROM_PB(&dstrSign, pwszCertFile, cchCertFile*SIZEOF(DRM_WCHAR));

    ChkDR(DRM_XMB_AppendNode   (pbOut, &dstrSign));
    ChkDR(DRM_XMB_CloseDocument(pbOut, &strXML));  /* </METERDATA> */

    DRMCRT_memmove (pbOut,
             PB_DSTR(&strXML),
             CB_DSTR(&strXML));

    pwszOut = (DRM_WCHAR *) pbOut;
    pwszOut [strXML.cchString] = g_wchNull;

    dr = Oem_File_Write(hfileIn, (DRM_VOID *) pwszOut, CB_DSTR(&strXML), &cbWritten)
                ? DRM_SUCCESS
                : DRM_E_FILE_WRITE_ERROR;

ErrorExit:

    if (pbOut != NULL)
    {
        Oem_MemFree(pbOut);
    }
    if (pcontextCrypto != NULL)
    {
        Oem_MemFree(pcontextCrypto);
    }


    return dr;
}

/*******************************************************************
*              PRIVATE FUNCTION RespondToChallenge
*
* purpose: high-level driver of this tool.
*          opens the files and calls next-lower-level functions
*          to verify, sign, and write the metering response
*******************************************************************/

static DRM_RESULT RespondToChallenge (
    const DRM_WCHAR  *pszFilePrivKey,
    const DRM_WCHAR  *pszFileChallenge,
    const DRM_WCHAR  *pszFileResponse,
    const DRM_WCHAR  *pszFileMeterCert)
{
    PRIVKEY              privkeyMAS    = { 0 };
    PUBKEY               pubkeyEncrypt = { 0 };
    DRM_RESULT           dr            = DRM_SUCCESS;
    OEM_FILEHDL          hfileOut      = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD            cbFile        = 0;
    DRM_DWORD            cbCertFile    = 0;
    DRM_BOOL             fPartialData  = FALSE;
    DRM_WCHAR           *pwszFile      = NULL;
    DRM_WCHAR           *pwszCertFile  = NULL;
    DRM_CONST_STRING     dstrMID       = EMPTY_DRM_STRING,
                         dstrCertMID   = EMPTY_DRM_STRING,
                         dstrTID       = EMPTY_DRM_STRING,
                         dstrRecords   = EMPTY_DRM_STRING;

    OEM_ECC_ZeroPrivateKey_P160( &privkeyMAS );

    ChkFAIL (DoSetup (pszFilePrivKey,
                      pszFileChallenge,
                      pszFileResponse,
                      pszFileMeterCert,
                     &privkeyMAS,
                     &hfileOut,
                     &pwszFile,
                     &cbFile,
                     &pwszCertFile,
                     &cbCertFile));

    ChkDR(ParseMeterChallenge (pwszFile,
                               cbFile / SIZEOF (DRM_WCHAR),
                               &dstrMID,
                               &dstrTID,
                               &dstrRecords,
                               &pubkeyEncrypt,
                               &privkeyMAS,
                               &fPartialData));

    ChkDR(ParseMeterCert (pwszCertFile,
                          cbCertFile / SIZEOF (DRM_WCHAR),
                          &dstrCertMID,
                          &privkeyMAS ));

    /*Verify MIDs*/
    if (! DRM_UTL_DSTRStringsEqual(&dstrCertMID,&dstrMID))
    {
        ChkDR(DRM_E_METERING_MID_MISMATCH);
    }

    ChkDR(WriteMeterResponse(hfileOut,
                            &dstrMID,
                            &dstrTID,
                            &dstrRecords,
                            &pubkeyEncrypt,
                             fPartialData,
                             pwszCertFile,
                             cbCertFile / SIZEOF (DRM_WCHAR),
                            &privkeyMAS));

    printf ("metering reset is %s\n", fPartialData ? "partial" : "entire");

ErrorExit:

    if (pwszFile != NULL)
    {
        Oem_MemFree (pwszFile);
    }
    if (pwszCertFile != NULL)
    {
        Oem_MemFree (pwszCertFile);
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

static void PrintUsage(
    const DRM_WCHAR *pwszAppName )
{
    printf ("Syntax: %S [-?] -p:PrivKeyFile -i:InputFile -o:ResponseFile -m:metercert.xml\n", pwszAppName);
    printf ("PrivKeyFile   - UNICODE XML with MAS' private key\n"
            "InputFile     - UNICODE XML of metering challenge\n"
            "ResponseFile  - UNICODE XML of metering response\n"
            "metercert.xml - UNICODE XML of metering cert\n");
}

/*******************************************************************
*                      PUBLIC FUNCTION DRM_Main
*
* purpose: entrypoint; validates args and calls driver
*******************************************************************/

DRM_LONG DRM_CALL DRM_Main (
    DRM_LONG    argc,
    DRM_WCHAR **argv)
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_CONST_STRING dstrPrivKey        = EMPTY_DRM_STRING,
                     dstrResponse       = EMPTY_DRM_STRING,
                     dstrInput          = EMPTY_DRM_STRING,
                     dstrFileMeterCert  = EMPTY_DRM_STRING;
    DRM_LONG         iArg               = 0;

    PrintCopyrightInfo( "DRM create metering response tool" );

    if( DRM_METERING_IsMeteringSupported() )
    {
        if ( argc <= 1 )
        {
            dr = DRM_S_MORE_DATA;
            goto _PrintUsage;
        }

        ChkArg( argv != NULL );
        for (iArg = 1; iArg < argc; iArg++)
        {
            DRM_WCHAR        wchOption = 0;
            DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;

            if( !DRM_CMD_ParseCmdLine (argv [iArg], &wchOption, &dstrParam, NULL))
            {
                /* See if it's a default parameter that PK knows how to handle */
                if ( DRM_FAILED( DRM_CMD_TryProcessDefaultOption( argv[iArg] ) ) )
                {
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
            else
            {
                wchOption = DRMCRT_towlower (wchOption);

                switch (wchOption)
                {
                case WCHAR_CAST('i'):
                    ASSIGN_DRM_STRING(dstrInput, dstrParam);
                    break;

                case WCHAR_CAST('o'):
                    ASSIGN_DRM_STRING(dstrResponse, dstrParam);
                    break;

                case WCHAR_CAST('p'):
                    ASSIGN_DRM_STRING(dstrPrivKey, dstrParam);
                    break;

                case WCHAR_CAST('m'):
                    ASSIGN_DRM_STRING(dstrFileMeterCert, dstrParam);
                    break;

                case WCHAR_CAST('?'):
                    goto _PrintUsage;

                default:
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
        }

        if (dstrPrivKey .cchString      == 0
         || dstrResponse.cchString      == 0
         || dstrInput.cchString         == 0
         || dstrFileMeterCert.cchString == 0 )
        {
            printf ("Missing arguments.\n");
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        ChkDR( RespondToChallenge( dstrPrivKey .pwszString,
                                   dstrInput   .pwszString,
                                   dstrResponse.pwszString,
                                   dstrFileMeterCert.pwszString ) );
    }
    else
    {
        printf( "This tool requires that the real metering library be linked to it.\r\n" );
        ChkDR( DRM_E_METERING_NOT_SUPPORTED );
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

