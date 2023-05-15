/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmdla.h>
#include <drmutilitieslite.h>
#include <drmdlaconstants.h>

ENTER_PK_NAMESPACE_CODE;

/*************************************************************************
** static functions
**************************************************************************/

static DRM_DWORD _NumberWidth(DRM_DWORD dwNumber)
{
    DRM_DWORD cchNumber = 1;

    while (dwNumber > 10)
    {
        dwNumber /= 10;
        cchNumber++;
    }

    return cchNumber;
}

static DRM_DWORD _RequiredCharsForCLIENTINFOBlock (DRM_DWORD  levelAppSecurity,
                                                   DRM_DWORD  idSubject,
                                                   DRM_DWORD  cchSecurityVersion,
                                                   DRM_DWORD  cchClientVersion,
                                                   DRM_DWORD  cchDevcert,
                                                   DRM_DWORD  idSubject2,
                                                   DRM_DWORD  cchDrmkversion,
                                                   DRM_DWORD  cchRevInfo)
{
    DRM_DWORD cchRequired = 0;

    /* <CLIENTINFO> */

    cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagClientInfo.cchString, 0, 0, 0);

    /* <CLIENTID> */

    cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagClientID.cchString,
                                                 CCH_BASE64_EQUIV (SIZEOF (CLIENTID)), 0, 0);

    /* <CLIENTVERSION> */

    cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagClientVersion    .cchString,
                                                 cchClientVersion, 0, 0);

    /* <SECURITYVERSION> */

    cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrTagSecurityVersion.cchString,
                                                 cchSecurityVersion, 0, 0);

    /* <APPSECURITY> */

    cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagAppSecurity.cchString,
                                   _NumberWidth (levelAppSecurity), 0, 0);

    /* <SUBJECTID1> */

    cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagSubjectID1.cchString,
                                   _NumberWidth (idSubject), 0, 0);

    /* <SUBJECTID2> */

    if ( idSubject2 > 0 )
    {
        cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagSubjectID2.cchString,
                                       _NumberWidth (idSubject2), 0, 0);
    }
    else
    {
        cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagSubjectID2.cchString,
                                                     0, 0, 0);
    }


    /* <DRMKVERSION> */

    if ( cchDrmkversion > 0 )
    {
        cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagDrmkVersion.cchString,
                                                     cchDrmkversion, 0, 0);
    }
    else
    {
        cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagDrmkVersion.cchString,
                                                     0, 0, 0);
    }


    /* <DEVCERT> if it is there */


    if (cchDevcert != 0)
    {
        cchRequired += DRM_XMB_RequiredCharsForTag (g_dstrTagDevCert.cchString,
                                                    CCH_BASE64_ENC_LARGE_WCH(cchDevcert), 0, 0);
    }


    /* <REVINFO>, always enforce even if there is no blob in HDS */
    cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrReqTagRevInfo.cchString, cchRevInfo, 0, 0);

    return cchRequired;
}

/******************************************************************************
 *                PRIVATE FUNCTION _RequiredCharsForRequest
 *
 * calculate buffer requirement to build the license request
 *****************************************************************************/

static DRM_DWORD _RequiredCharsForRequest (IN DRM_LICENSE_CHALLENGE *f_plicensechallenge)
{
    DRM_DWORD cchRequired = 0;

    cchRequired  = DRM_XMB_RequiredCharsForTag (g_dstrTagLicenseRequest    .cchString,
                                                0,
                                                g_dstrAttributeVersion      .cchString,
                                                g_dstrAttributeVersion2Value.cchString);

    cchRequired += DRM_XMB_RequiredCharsForTag (g_dstrTagV1Challenge.cchString,
                                                CCH_CDATA_REQUIRED (&f_plicensechallenge->dstrV1Challenge),
                                                0,
                                                0);

    if (f_plicensechallenge->cUplinks > 0)
    {
        DRM_DWORD cchUplink  = 0;

        cchRequired  += DRM_XMB_RequiredCharsForTag (g_dstrTagUplinks.cchString, 0, 0, 0);

        cchUplink    = DRM_XMB_RequiredCharsForTag (g_dstrTagUplink.cchString,
                                                    CCH_BASE64_EQUIV (SIZEOF (DRM_LICENSE_STATE_DATA)),
                                                    g_dstrTagKID.cchString,
                                                    CCH_BASE64_EQUIV (SIZEOF (DRM_KID)));

        cchRequired += CCH_BASE64_ENC_LARGE_WCH(f_plicensechallenge->cUplinks * cchUplink);
    } /* end if cUplinks > 0 */

    if (f_plicensechallenge->cRights > 0)
    {
        DRM_DWORD iRight = 0;

        cchRequired += DRM_XMB_RequiredCharsForTag (g_dstrTagActionList.cchString, 0, 0, 0);

        for (iRight = 0;
             iRight < f_plicensechallenge->cRights;
             iRight++)
        {
            cchRequired += DRM_XMB_RequiredCharsForTag (g_dstrTagAction.cchString,
                                                        f_plicensechallenge->ppdstrRights [iRight]->cchString,
                                                        0,
                                                        0);
        }
    } /* end if cRights > 0 */

    if (f_plicensechallenge->dstrClientIdentity.cchString != 0)
    {
        cchRequired += f_plicensechallenge->dstrClientIdentity.cchString;
    }
    else
    {
        /*
        ** In any failure case I assume max length for the version.
        */

        DRM_DWORD dwVerLen = 0;
        if ( !DRM_SUCCEEDED( DRM_STR_StringCchLengthW( f_plicensechallenge->pwszBBVer, VER_STRING_MAX_LEN, &dwVerLen ) ) )
        {
            dwVerLen = VER_STRING_MAX_LEN + 1;
        }

        cchRequired += _RequiredCharsForCLIENTINFOBlock (f_plicensechallenge->levelAppSecurity,
                                                         f_plicensechallenge->idSubject,
                                                         dwVerLen,
                                                         g_dstrReqTagWMDRMPDClientVersionData.cchString,
                                                         f_plicensechallenge->dstrDeviceCert.cchString,
                                                         0,
                                                         0,
                                                         f_plicensechallenge->dstrRevInfo.cchString);
    }

    /* the header is added as a formed XML node */

    cchRequired += f_plicensechallenge->dstrHeader.cchString;

    return cchRequired;
}

/******************************************************************************
 *                PRIVATE FUNCTION _WriteUplinkNode
 *
 * write the <UPLINKS> section and its subnodes
 *****************************************************************************/

static DRM_RESULT _WriteUplinkNode (IN DRM_LICENSE_CHALLENGE *f_plicensechallenge)
{
    DRM_DWORD iUplink = 0;
    DRM_RESULT dr     = DRM_SUCCESS;

    ChkDR(DRM_XMB_WriteTag (f_plicensechallenge->pbXML,     /* <UPLINKS> */
                            &g_dstrTagUplinks,
                             NULL,
                             NULL,
                             NULL,
                             wttOpen));

    for (iUplink = 0;
         iUplink < f_plicensechallenge->cUplinks;
         iUplink++)
    {
        DRM_DWORD        cchEncoded    = CCH_BASE64_EQUIV(SIZEOF (DRM_LICENSE_STATE_DATA));
        DRM_CONST_STRING dstrStateData = EMPTY_DRM_STRING;

        dstrStateData.pwszString = (DRM_WCHAR *) f_plicensechallenge->rgbStack;
        dstrStateData.cchString  =  cchEncoded;

        ChkDR(DRM_B64_EncodeW((DRM_BYTE *) (f_plicensechallenge->rglicensestatedata + iUplink),
                               SIZEOF (DRM_LICENSE_STATE_DATA),
                              (DRM_WCHAR *) f_plicensechallenge->rgbStack,
                              &cchEncoded,
                               DRM_BASE64_ENCODE_WMDRM));

        ChkDR(DRM_XMB_WriteTag(f_plicensechallenge->pbXML,             /* <UPLINK KID = "B64">B64B64B64</UPLINK> */
                              &g_dstrTagUplink,
                              &dstrStateData,
                              &g_dstrTagKID,
                               f_plicensechallenge->rgdstrUplinkKID + iUplink,
                               wttClosed));
    }

    ChkDR(DRM_XMB_EncryptAndCloseCurrNode(f_plicensechallenge->pbXML,
                                         &(f_plicensechallenge->pcontextBBX->CryptoContext),
                                          f_plicensechallenge->pcontextBBX->pOEMContext,
                                         &g_pubkeyLicenseServer,
                                          NULL)); /* </UPLINKS> */

ErrorExit:

    return dr;
}

/******************************************************************************
 *                PRIVATE FUNCTION _WriteActionsNode
 *
 * write the <ACTIONS> section and its subnodes
 *****************************************************************************/

static DRM_RESULT _WriteActionsNode (IN DRM_LICENSE_CHALLENGE *f_plicensechallenge)
{
    DRM_DWORD  iRight = 0;
    DRM_RESULT dr     = DRM_SUCCESS;

    ChkDR(DRM_XMB_WriteTag (f_plicensechallenge->pbXML,      /* <ACTIONLIST> */
                            &g_dstrTagActionList,
                             NULL,
                             NULL,
                             NULL,
                             wttOpen));

    for (iRight = 0;
         iRight < f_plicensechallenge->cRights;
         iRight++)
    {
        ChkDR(DRM_XMB_WriteTag (f_plicensechallenge->pbXML, /* <ACTION> */
                                &g_dstrTagAction,
                                 f_plicensechallenge->ppdstrRights [iRight],
                                 NULL,
                                 NULL,
                                 wttClosed));
    }

    ChkDR(DRM_XMB_CloseCurrNode (f_plicensechallenge->pbXML, NULL)); /* </ACTIONLIST> */

ErrorExit:

    return dr;
}

/*
** Add the following node/subnode to the request:
**
** <CLIENTINFO>
**    <CLIENTID>...</CLIENTID>
**    <CLIENTVERSION>...</CLIENTVERSION>
**    <SECURITYVERSION>...</SECURITYVERSION>
**    <APPSECURITY>...</APPSECURITY>
**    <SUBJECTID1>...</SUBJECTID1>
**    <SUBJECTID2>...</SUBJECTID2>
**    <DRMKVERSION>...</DRMKVERSION>
**    <DEVCERT>...</DEVCERT> optional
** </CLIENTINFO>
*/

static DRM_RESULT _AddClientInfo (IN DRM_LICENSE_CHALLENGE *f_plicensechallenge)
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_DWORD        cchEncoded   = CCH_BASE64_EQUIV (SIZEOF (CLIENTID));
    DRM_DWORD        cbXml        = 0;
    DRM_CONST_STRING dstrScratch  = EMPTY_DRM_STRING;


    ChkArg (f_plicensechallenge != NULL);

    DRMCASSERT (SIZEOF (f_plicensechallenge->rgbStack) >= CCH_BASE64_EQUIV (SIZEOF (CLIENTID)));
    cbXml = f_plicensechallenge->cbXML;

    dstrScratch.pwszString = f_plicensechallenge->pwszBBVer;
    ChkDR( DRM_STR_StringCchLengthW( f_plicensechallenge->pwszBBVer, VER_STRING_MAX_LEN, &dstrScratch.cchString ) );

    cchEncoded = SIZEOF( f_plicensechallenge->rgbStack  );

    ChkDR( DRM_LA_GetClientInfo(&f_plicensechallenge->clientid,
                                 f_plicensechallenge->rgbStack,
                                &cchEncoded,
          (DRM_CONST_STRING *)  &g_dstrReqTagWMDRMPDClientVersionData,
                                &dstrScratch,
                                 f_plicensechallenge->levelAppSecurity,
                                 f_plicensechallenge->idSubject,
                                 0,
                                 NULL,
                                (f_plicensechallenge->dstrDeviceCert.cchString != 0)? &f_plicensechallenge->dstrDeviceCert : NULL,
                                 f_plicensechallenge->pbXML,
                                &cbXml,
                                &(f_plicensechallenge->pcontextBBX->CryptoContext),
                                 f_plicensechallenge->pcontextBBX->pOEMContext,
                                 FALSE,
                                 (f_plicensechallenge->dstrRevInfo.cchString != 0)? &f_plicensechallenge->dstrRevInfo : NULL));

ErrorExit:
    return dr;
}

/**************************************************************************
**                  PUBLIC FUNCTION DRM_LA_CreateChallenge
***************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LA_CreateChallenge (
    IN DRM_LICENSE_CHALLENGE *f_plicensechallenge,
    OUT      DRM_CHAR        *f_pszChallenge,
    IN OUT   DRM_DWORD       *f_pcchChallenge)
{
    DRM_RESULT   dr    = DRM_SUCCESS;
    DRM_DWORD    cbXML = 0;
    _XMBContext *pbXML = NULL;
    DRM_STRING   dstr  = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMLICACQREQ, PERF_FUNC_DRM_LA_CreateChallenge);

    ChkArg (f_plicensechallenge != NULL
        &&  f_pcchChallenge     != NULL);

    if (f_plicensechallenge->dstrHeader.cchString != 0)
    {
        ChkDRMString (&f_plicensechallenge->dstrHeader);
    }

    if (f_plicensechallenge->dstrV1Challenge.cchString != 0)
    {
        ChkDRMString (&f_plicensechallenge->dstrV1Challenge);
    }
    else
    {
        ASSIGN_DRM_STRING (f_plicensechallenge->dstrV1Challenge, g_dstrDefaultV1Data);
    }

    if (f_plicensechallenge->dstrDeviceCert.cchString != 0)
    {
        ChkDRMString (&f_plicensechallenge->dstrDeviceCert);
    }

    /* calculate the required size for the challenge XML and express as bytes */

    cbXML = _RequiredCharsForRequest (f_plicensechallenge) * SIZEOF (DRM_WCHAR)
          + g_cbXMLBuilderMinimum;

    /* verify that the XML context is WCHAR-aligned and adjust the requirement if it isn't */

    /* check buffer sizes; adjust for possible buffer realignment */

    if   (f_pszChallenge ==  NULL
      || *f_pcchChallenge <  CCH_BASE64_EQUIV (cbXML))
    {
        *f_pcchChallenge = CCH_BASE64_EQUIV (cbXML);
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    ChkDR(DRM_UTL_EnsureDataAlignment ((DRM_BYTE*)f_pszChallenge,
                                       cbXML,
                                       (DRM_BYTE**)&pbXML,
                                      &cbXML,
                                       SIZEOF (DRM_DWORD),
                                       NULL));

    /* Initialize Context */

    ZEROMEM(pbXML, cbXML);

    f_plicensechallenge->pbXML = pbXML;
    f_plicensechallenge->cbXML = cbXML;

    /* Init XML Builder
    ** Add document root tag: <LicenseRequest version=2.0.0.0"> */

    ChkDR(DRM_XMB_CreateDocument(f_plicensechallenge->cbXML,  f_plicensechallenge->pbXML, &g_dstrTagLicenseRequest));
    ChkDR(DRM_XMB_AddAttribute  (f_plicensechallenge->pbXML, &g_dstrAttributeVersion,   &g_dstrAttributeVersion2Value));
    ChkDR(DRM_XMB_WriteCDATATag (f_plicensechallenge->pbXML, &g_dstrTagV1Challenge,     &f_plicensechallenge->dstrV1Challenge, NULL, NULL, wttClosed));

    if (f_plicensechallenge->cUplinks > 0)
    {
        ChkDR(_WriteUplinkNode(f_plicensechallenge));
    }

    if (f_plicensechallenge->cRights > 0)
    {
        ChkDR(_WriteActionsNode(f_plicensechallenge));
    }

    if (f_plicensechallenge->dstrClientIdentity.cchString != 0)
    {
        ChkDR(DRM_XMB_AppendNode(f_plicensechallenge->pbXML, &f_plicensechallenge->dstrClientIdentity));
    }
    else
    {
        ChkDR(_AddClientInfo(f_plicensechallenge));
    }

    if (f_plicensechallenge->dstrHeader.cchString != 0)
    {
        ChkDR(DRM_XMB_AppendNode(f_plicensechallenge->pbXML, &f_plicensechallenge->dstrHeader));
    }

    ChkDR(DRM_XMB_CloseDocument (f_plicensechallenge->pbXML, &dstr));

    /* construct challenge buffer */

    if( CB_DSTR(&dstr) > *f_pcchChallenge )
    {
        /* This should never happen, but PREfast complains, plus it's good to be paranoid */
        *f_pcchChallenge = CCH_BASE64_EQUIV( CB_DSTR(&dstr) );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    MEMMOVE( f_pszChallenge, PB_DSTR(&dstr), CB_DSTR(&dstr) );
    ChkDR(DRM_B64_EncodeA((DRM_BYTE*)f_pszChallenge,
                          CB_DSTR(&dstr),
                          f_pszChallenge,
                          f_pcchChallenge,
                          DRM_BASE64_ENCODE_WMDRM));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**************************************************************************
**                  PUBLIC FUNCTION DRM_LA_GetClientInfo
***************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_LA_GetClientInfo (
    IN       CLIENTID               *f_pclientid,
    __in_bcount( *f_pcbStack ) DRM_BYTE *f_pbStack,
    IN OUT   DRM_DWORD              *f_pcbStack,
    IN       DRM_CONST_STRING       *f_pdstrClientVersionData,
    IN       DRM_CONST_STRING       *f_pdstrSecurityVersionData,
    IN       DRM_DWORD               f_AppSecurityData,
    IN       DRM_DWORD               f_SubjectId1,
    IN       DRM_DWORD               f_SubjectId2,
    IN       DRM_CONST_STRING       *f_pdstrDRMKVersion,
    IN       DRM_CONST_STRING       *f_pdstrDevcert,
    IN OUT   _XMBContext            *f_pbXML,
    IN OUT   DRM_DWORD              *f_pcbXML,
    IN       DRM_CRYPTO_CONTEXT     *f_contextCRYP,
    IN       DRM_VOID               *f_pOEMContext,
    IN       DRM_BOOL                f_fCreateNewDoc,
    IN       DRM_CONST_STRING       *f_pdstrRevInfo)
{
    DRM_RESULT  dr                   =  DRM_SUCCESS;
    DRM_DWORD   cbXML                =  0;
    DRM_DWORD   cchDevcert           =  0;
    DRM_DWORD   cbDRMKVersion        =  0;
    DRM_DWORD   cchRevInfo           =  0;
    DRM_WCHAR  *pszScratch           = (DRM_WCHAR *) f_pbStack;
    DRM_DWORD   cchEncoded           = CCH_BASE64_EQUIV (SIZEOF(CLIENTID));
    DRM_CONST_STRING dstrScratch     = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMLICACQREQ, PERF_FUNC_DRM_LA_GetClientInfo);

    ChkArg (f_pclientid         != NULL
        &&  f_pcbStack          != NULL
        &&  f_pcbXML            != NULL
        &&  f_AppSecurityData   > 0
        &&  f_SubjectId1        > 0);

    ChkArg( *f_pcbStack >= cchEncoded * SIZEOF( DRM_WCHAR ) );

    ChkDRMString(f_pdstrClientVersionData);
    ChkDRMString(f_pdstrSecurityVersionData);

    if ( f_pdstrRevInfo != NULL )
    {
        ChkDRMString(f_pdstrRevInfo);
        cchRevInfo = f_pdstrRevInfo->cchString;
    }

    if ( f_pdstrDRMKVersion != NULL )
    {
        ChkDRMString(f_pdstrDRMKVersion);
        cbDRMKVersion = f_pdstrDRMKVersion->cchString;
    }

    if ( f_pdstrDevcert != NULL )
    {
        ChkDRMString(f_pdstrDevcert);
        cchDevcert = f_pdstrDevcert->cchString;
    }

    /* calculate the required size for the challenge XML and express as bytes */

    cbXML += _RequiredCharsForCLIENTINFOBlock (f_AppSecurityData,
                                               f_SubjectId1,
                                               f_pdstrSecurityVersionData->cchString,
                                               f_pdstrClientVersionData->cchString,
                                               cchDevcert,
                                               f_SubjectId2,
                                               cbDRMKVersion,
                                               cchRevInfo) * SIZEOF(DRM_WCHAR);

    if ( f_fCreateNewDoc )
    {
        cbXML += g_cbXMLBuilderMinimum;
    }


    if ( f_pbStack == NULL
    ||   *f_pcbStack <   CCH_BASE64_EQUIV (SIZEOF (CLIENTID))
    ||   f_pbXML == NULL
    ||   *f_pcbXML < cbXML)
    {

        *f_pcbStack = CCH_BASE64_EQUIV (SIZEOF (CLIENTID));
        *f_pcbXML = cbXML;
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    cbXML = *f_pcbXML;

    if ( f_fCreateNewDoc )
    {
        ChkDR(DRM_UTL_EnsureDataAlignment ((DRM_BYTE*)f_pbXML,
                                            cbXML,
                                           (DRM_BYTE**)&f_pbXML,
                                           &cbXML,
                                            SIZEOF (DRM_DWORD),
                                            NULL));

        ZEROMEM(f_pbXML, cbXML);
        /* Init XML Builder
        ** Add document root tag: <CLIENTINFO> */

        ChkDR(DRM_XMB_CreateDocument(cbXML,  f_pbXML, &g_dstrReqTagClientInfo));
    }
    else
    {
        ChkDR(DRM_XMB_WriteTag (f_pbXML,
                                &g_dstrReqTagClientInfo,
                                 NULL,
                                 NULL,
                                 NULL,
                                 wttOpen));
    }

    /*      <CLIENTID> */

    ChkDR(DRM_B64_EncodeW((DRM_BYTE*) f_pclientid,
                           SIZEOF(CLIENTID),
                           pszScratch,
                          &cchEncoded,
                           DRM_BASE64_ENCODE_WMDRM));

    dstrScratch.pwszString = pszScratch;
    dstrScratch.cchString  = cchEncoded;

    ChkDR(DRM_XMB_WriteTag (f_pbXML,
                            &g_dstrReqTagClientID,
                            &dstrScratch,
                             NULL,
                             NULL,
                             wttClosed));

     /*      <REVINFO> */
    ChkDR(DRM_XMB_WriteTag (f_pbXML,
                            &g_dstrReqTagRevInfo,
                            f_pdstrRevInfo,
                             NULL,
                             NULL,
                             wttClosed));

    /*      <CLIENTVERSION> */

    ChkDR(DRM_XMB_WriteTag (f_pbXML,
                            &g_dstrReqTagClientVersion,
                             f_pdstrClientVersionData,
                             NULL,
                             NULL,
                             wttClosed));

    /*      <SECURITYVERSION> */


    ChkDR(DRM_XMB_WriteTag (f_pbXML,
                            &g_dstrTagSecurityVersion,
                             f_pdstrSecurityVersionData,
                             NULL,
                             NULL,
                             wttClosed));

    /*      <APPSECURITY> */

    ChkDR( DRM_UTL_NumberToString (f_AppSecurityData, (DRM_WCHAR*)dstrScratch.pwszString, 20, 0, 10, &dstrScratch.cchString ) );

    ChkDR(DRM_XMB_WriteTag (f_pbXML,
                            &g_dstrReqTagAppSecurity,
                            &dstrScratch,
                             NULL,
                             NULL,
                             wttClosed));

    /*      <SUBJECTID1> */

    ChkDR( DRM_UTL_NumberToString (f_SubjectId1, (DRM_WCHAR*)dstrScratch.pwszString, 20, 0, 10, &dstrScratch.cchString) );

    ChkDR(DRM_XMB_WriteTag (f_pbXML,
                            &g_dstrReqTagSubjectID1,
                            &dstrScratch,
                             NULL,
                             NULL,
                             wttClosed));

    /*      <SUBJECTID2> */

    if (f_SubjectId2 != 0)
    {
        ChkDR( DRM_UTL_NumberToString (f_SubjectId2, (DRM_WCHAR*)dstrScratch.pwszString, 20, 0, 10, &dstrScratch.cchString) );

        ChkDR(DRM_XMB_WriteTag (f_pbXML,
                                &g_dstrReqTagSubjectID2,
                                &dstrScratch,
                                 NULL,
                                 NULL,
                                 wttClosed));


    }
    else
    {
        ChkDR(DRM_XMB_WriteTag (f_pbXML,
                                &g_dstrReqTagSubjectID2,
                                 NULL,
                                 NULL,
                                 NULL,
                                 wttClosed));
    }

    /*      <DRMKVERSION> */

    ChkDR(DRM_XMB_WriteTag (f_pbXML,
                            &g_dstrReqTagDrmkVersion,
                             f_pdstrDRMKVersion,
                             NULL,
                             NULL,
                             wttClosed));

    /*      <DEVCERT> */

    if ( f_pdstrDevcert != NULL )
    {
        ChkDR(DRM_XMB_OpenNode               (f_pbXML,&g_dstrTagDevCert));
        ChkDR(DRM_XMB_AddData                (f_pbXML, f_pdstrDevcert));
        ChkDR(DRM_XMB_EncryptAndCloseCurrNode(f_pbXML, f_contextCRYP, f_pOEMContext, &g_pubkeyLicenseServer, NULL));
    }

    /* </CLIENTINFO> */

    if ( f_fCreateNewDoc )
    {
        ChkDR(DRM_XMB_CloseDocument (f_pbXML, (DRM_STRING *)&dstrScratch));
        *f_pcbXML = CB_DSTR( &dstrScratch );

        /*arrange the bytes to original position*/
        if ((DRM_BYTE*)f_pbXML != PB_DSTR( &dstrScratch ) )
        {
            MEMCPY((DRM_BYTE*)f_pbXML, dstrScratch.pwszString, CB_DSTR(&dstrScratch));
        }
    }
    else
    {
        ChkDR(DRM_XMB_CloseCurrNode (f_pbXML, NULL));
    }


ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;


