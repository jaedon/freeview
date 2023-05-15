/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmdomainapi.h>
#include <drmsoapxmlutility.h>
#include <drmxmlparser.h>
#include <DomainTestAPI.h>
#include <drmsoapxmlconstants.h>
#include <drmmathsafe.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

/*
** ---------------------------------------
** Domain Test Global Variables
** ---------------------------------------
*/
DRM_APP_CONTEXT *m_pContextManagerDomain = {0};


/*********************************************************************
**           Name: _TestValidateChallenge
**
**    Description: Internal test function that validates Join/Leave challenge
**
**          Input: pbChallenge - Pointer to challenge
**            cbChallenge    - Count of bytes in challenge
**            bJoin        - Specifies Join/Leave
**            bIndirectQueryData - Specifies whether INDIRECT_QUERY_DATA
**                                is used
**                 ppchArgs        - Array of arguments
**                            ppchArgs[0] - Version
**                            ppchArgs[1] - Service ID
**                            ppchArgs[2] - Account ID
**                            ppchArgs[3] - Friendly Name
**                            ppchArgs[4] - Custom Data
**
**         Output: None
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_FAIL == Failure
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/



static DRM_RESULT _TestValidateChallenge(
    const DRM_BYTE  *pbChallenge,
          DRM_DWORD  cbChallenge,
          DRM_BOOL   bJoin,
          DRM_BOOL   bIndirectQueryData,
    const DRM_CHAR **ppchArgs )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR *pchChallenge = NULL;
    DRM_DWORD index = 0;
    DRM_LONG ret = 0;

    DRM_ANSI_CONST_STRING dastrTag;
    const DRM_CHAR* xmlTags[] = {"soap:Body", "JoinDomain", "challenge", "Challenge", "Domain"};

    const DRM_CHAR* xmlVerify[] = {"Version", "ServiceID", "AccountID", "FriendlyName", "CustomData"};

    DRM_SUBSTRING dsstrXmlNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubData = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlTemp    = EMPTY_DRM_SUBSTRING;

    DRM_CONST_STRING sChallenge = EMPTY_DRM_STRING;

    if ( !bJoin )
    {
        xmlTags[1] = "LeaveDomain";
    }

    /* Create a string with Challenge bytes */
    ChkMem(pchChallenge = (DRM_CHAR *)Oem_MemAlloc(cbChallenge + 1));
    for(index = 0; index < cbChallenge; index++)
    {
        pchChallenge[index] = (DRM_CHAR)pbChallenge[index];
    }
    pchChallenge[cbChallenge] = '\0';

    ChkDR( MakeDRMString(&sChallenge, pchChallenge) );

    /* Make sure that the challenge is valid Xml */
    ChkBOOL(DRM_XML_Validate(&sChallenge), DRM_E_FAIL);

    dsstrXmlNode.m_cch = cbChallenge;
    dsstrXmlNode.m_ich = 0;

    /* ClientInfo and ClientVersion should NOT be present here
    ** since there is no SDK runtime.
    **/
    if( DRM_SUCCEEDED( DRM_XML_GetSubNodeA(
                                    pchChallenge,
                                   &dsstrXmlNode,
                                   &g_dastrReqTagClientInfo,
                                    NULL,
                                    NULL,
                                    0,
                                    NULL,
                                   &dsstrXmlTemp,
                                    1 ) ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    if( DRM_SUCCEEDED( DRM_XML_GetSubNodeA(
                                    pchChallenge,
                                   &dsstrXmlNode,
                                   &g_dastrReqTagClientVersion,
                                    NULL,
                                    NULL,
                                    0,
                                    NULL,
                                   &dsstrXmlTemp,
                                    1 ) ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    /* Traverse through the Xml to get to the challenge data */
    for(index = 0; index < 5; index++)
    {
        dastrTag.pszString = xmlTags[index];
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(xmlTags[index]), &dastrTag.cchString ) );
        ChkDR(DRM_XML_GetSubNodeA(
            pchChallenge,
            &dsstrXmlNode,
            &dastrTag,
            NULL,
            NULL,
            0,
            &dsstrXmlNode,
            &dsstrXmlData,
            1));
    }

    /* Verify challenge data */
    for(index = 0; index < 5; index++)
    {
        /* Ignore Indirect Query Data */
        if(bIndirectQueryData) break;

        /* Ignore Parameters Not Specified */
        if(NULL == ppchArgs[index]) continue;

        dastrTag.pszString = xmlVerify[index];
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(xmlVerify[index]), &dastrTag.cchString ) );
        ChkDR(DRM_XML_GetSubNodeA(
            pchChallenge,
            &dsstrXmlNode,
            &dastrTag,
            NULL,
            NULL,
            0,
            &dsstrXmlSubNode,
            &dsstrXmlSubData,
            1));

        /* Ignore comparison of custom data due to Xml Encoding */
        if(DRMCRT_strncmp("CustomData", dastrTag.pszString, dastrTag.cchString) == 0) continue;

        ret = DRMCRT_strncmp(
            ppchArgs[index],
            &pchChallenge[dsstrXmlSubData.m_ich],
            dsstrXmlSubData.m_cch);

        if(ret != 0) dr = DRM_E_FAIL;

    }

ErrorExit:
    FREEDRMSTRING( sChallenge );
    SAFE_OEM_FREE(pchChallenge);
    return dr;
}



/*********************************************************************
**           Name: _TestDrm_GenerateChallenge
**
**    Description: Internal test function that executes various test scenarios
**                 for generating challenge
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - Test Category. 0: Join, 1: Leave
**                            argv[1] - DRM App Context
**                            argv[2] - Flags
**                            argv[3] - Account ID
**                            argv[4] - Service ID
**                            argv[4] - Revision
**                            argv[6] - Friendly Name
**                            argv[7] - Friendly Name Count
**                            argv[8] - Custom Data
**                            argv[9] - Custom Data Count
**                            argv[10] - Challenge
**                            argv[11] - Challenge Count
**                pDrmContext - Pointer to DRM context
**
**         Output: ppbChallenge - Double pointer to challenge
**                 pcbChallenge - Pointer to count of challenge
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/


static DRM_RESULT _TestDrm_GenerateChallenge(
    __in long argc,
    __in_ecount(argc) char **argv,
    __in    DRM_APP_CONTEXT *pDrmContext,
    __deref_out_ecount_opt(*pcbChallenge)    DRM_BYTE **ppbChallenge,
    __out DRM_DWORD *pcbChallenge)
{
    /* Parameters */
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD dwFlags = 0;
    DRM_DOMAIN_ID DomainID = {0};
    DRM_DOMAIN_ID *pDomainID = &DomainID;
    const DRM_CHAR *pszFriendlyName = "PK Domains Test";
    DRM_DWORD cchFriendlyName = 0;
    const DRM_CHAR *pszCustomData = "Some Custom Data";
    DRM_DWORD cchCustomData = 0;
    DRM_DWORD dwRevision = 0;
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrServiceID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = SIZEOF(DRM_GUID);
    DRM_DWORD testCategory = 0;

    /* Result */
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(pszFriendlyName), &cchFriendlyName ) );
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(pszCustomData), &cchCustomData ) );

    /* Prepare account ID */
    ChkDR( MakeDRMString(&dstrAccountID, "NjU0MzIxMDk4NzY1NDMyMQ==") );
    ChkDR(DRM_B64_DecodeW(&dstrAccountID,
        &cbSize,
        (DRM_BYTE*)&DomainID.m_oAccountID,
        0));

    cbSize = SIZEOF(DRM_GUID);

    /* Prepare Service ID */
    ChkDR( MakeDRMString(&dstrServiceID, "AH+03juKbUGbHl1V/QIwRA==") );
    ChkDR(DRM_B64_DecodeW(&dstrServiceID,
        &cbSize,
        (DRM_BYTE*)&DomainID.m_oServiceID,
        0));

    DomainID.m_dwRevision = dwRevision;

    /* Process arguments */
    switch(argc)
    {
    case 12: /* Will be processed later */
        __fallthrough;
    case 11: /* Will be processed later */
        __fallthrough;
    case 10: /* Get Custom Data count */
        cchCustomData = (NULL == argv[9]? cchCustomData: atol(argv[9]));
        __fallthrough;
    case 9: /* Get Custom Data */
        pszCustomData = argv[8];
        if ( pszCustomData != NULL && cchCustomData > DRMCRT_strlen(pszCustomData) )
        {
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(pszCustomData), &cchCustomData ) );
        }
        __fallthrough;
    case 8: /* Get Friendly Name count (ignore in case of Leave Challenge) */
        cchFriendlyName = (NULL == argv[7]? cchFriendlyName: atol(argv[7]));
        __fallthrough;
    case 7: /* Get Friendly Name (ignore in case of Leave Challenge) */
        pszFriendlyName = argv[6];
        __fallthrough;
    case 6: /* Get Revision ID */
        if(NULL == argv[5])
        {
            DomainID.m_dwRevision = 0;
        }
        else
        {
            DomainID.m_dwRevision = (DRM_DWORD)atol(argv[5]);
        }
        __fallthrough;
    case 5: /* Get Service ID */
        if(NULL == argv[4])
        {
            ZEROMEM(&DomainID.m_oServiceID, SIZEOF(DRM_GUID));
        }
        else
        {
            cbSize = SIZEOF(DRM_GUID);

            FREEDRMSTRING( dstrServiceID );
            ChkDR( MakeDRMString(&dstrServiceID, argv[4]) );
            ChkDR(DRM_B64_DecodeW(&dstrServiceID,
                &cbSize,
                (DRM_BYTE*)&DomainID.m_oServiceID,
                0));
        }
        __fallthrough;
    case 4: /* Get Account ID */
        if(NULL == argv[3])
        {
            ZEROMEM(&DomainID.m_oAccountID, SIZEOF(DRM_GUID));
        }
        else
        {
            cbSize = SIZEOF(DRM_GUID);

            FREEDRMSTRING( dstrAccountID );
            ChkDR( MakeDRMString(&dstrAccountID, argv[3]) );
            ChkDR(DRM_B64_DecodeW(&dstrAccountID,
                &cbSize,
                (DRM_BYTE*)&DomainID.m_oAccountID,
                0));
        }
        __fallthrough;
    case 3: /* Get Flags */
        dwFlags = (NULL == argv[2]? dwFlags: atol(argv[2]));
        __fallthrough;
    case 2: /* See if context is NULL */
        if(NULL == argv[1])
            pDrmContext = NULL;
        else /* For negative test cases, corrupt DrmContext */
        {
            DRM_BYTE by = 0;
            ChkDR( DRM_LongToByte( atoi(argv[1]), &by ) );
            ChkDR( DRM_ByteAdd( *pDrmContext->rgbOpaqueBuffer, by, pDrmContext->rgbOpaqueBuffer ) );
        }
        __fallthrough;
    case 1: /* Get test type */
        testCategory = (NULL == argv[0]? testCategory: atoi(argv[0]));
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    if(0 == testCategory)
    {
        /* Call with NULL Challenge to Get Size of Challenge */
        dr = Drm_JoinDomain_GenerateChallenge(pDrmContext,
            dwFlags,
            &DomainID,
            pszFriendlyName,
            cchFriendlyName,
            pszCustomData,
            cchCustomData,
            NULL,
            pcbChallenge);
    }
    else
    {
        /* Call with NULL Challenge to Get Size of Challenge */
        dr = Drm_LeaveDomain_GenerateChallenge(pDrmContext,
            dwFlags,
            &DomainID,
            pszCustomData,
            cchCustomData,
            NULL,
            pcbChallenge);
    }
PREFAST_POP

    if(argc > 5 && NULL == argv[3] && NULL == argv[4] && NULL == argv[5]) pDomainID = NULL;

    /* Allocate memory for the challenge */
    if( DRM_E_BUFFERTOOSMALL != dr )
    {
        ChkDR( dr );
    }
    ChkMem(pbChallenge = (DRM_BYTE *)Oem_MemAlloc(*pcbChallenge));

    switch(argc)
    {
    case 12: /* Check if Challenge count should be NULL */
        if(NULL == argv[11]) /* Check if Challenge Count should be NULL */
        {
            pcbChallenge = NULL;
        }
        else /* Otherwise use the value as an offset for negative test cases */
        {
            *pcbChallenge = *pcbChallenge + atol(argv[11]);
        }
        __fallthrough;
    case 11:
        if(NULL == argv[10]) /* Check if Challenge should be NULL */
        {
            SAFE_OEM_FREE( pbChallenge );
        }
        else /* Otherwise use the value as an offset to corrupt challenge for negative test cases */
        {
            DRM_BYTE by = 0;
            ChkDR( DRM_LongToByte( atoi(argv[10]), &by ) );
            ChkDR( DRM_ByteAdd( *pbChallenge, by, pbChallenge ) );
        }
    }

    if(0 == testCategory)
    {
        /* Final call */
        dr = Drm_JoinDomain_GenerateChallenge(pDrmContext,
            dwFlags,
            pDomainID,
            pszFriendlyName,
            cchFriendlyName,
            pszCustomData,
            cchCustomData,
            pbChallenge,
            pcbChallenge);
    }
    else
    {
        /* Final call */
        dr = Drm_LeaveDomain_GenerateChallenge(pDrmContext,
            dwFlags,
            pDomainID,
            pszCustomData,
            cchCustomData,
            pbChallenge,
            pcbChallenge);
    }

ErrorExit:
    FREEDRMSTRING( dstrAccountID );
    FREEDRMSTRING( dstrServiceID );
    *ppbChallenge = pbChallenge;
    return dr;
}



/********************************************************************
**           Name: _TestDrm_ProcessResponse
**
**    Description: Internal test function that executes various test scenarios
**                 for processing response
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - Test Category. 0: Join, 1: Leave
**                            argv[1] - DRM App Ccntext
**                            argv[2] - Response filename
**                            argv[3] - Response count
**                            argv[4] - Result
**                            argv[5] - Domain ID
**                 pDrmContext - Pointer to DRM context
**
**         Output: pResult - Pointer to result
**                     pDomainID - Pointer to DomainID
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

static DRM_RESULT _TestDrm_ProcessResponse(
    __in long argc,
    __in_ecount(argc) char **argv,
    __in DRM_APP_CONTEXT *pDrmContext,
    __out DRM_RESULT *pResult,
    __out DRM_DOMAIN_ID *pDomainID,
    DRM_PROCESS_DJ_RESPONSE_FLAG dwFlags)
{
    DRM_RESULT Result = DRM_SUCCESS;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;
    DRM_DWORD cbResponseOffset = 0;
    const DRM_CHAR *pchResponseFile = NULL;

    /* Result */
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_DWORD testCategory = 0;
    *pResult = Result;

    /* Process arguments */
    switch(argc)
    {
    case 6: /* See if Domain ID should be NULL */
        if(NULL == argv[5]) pDomainID = NULL;
        __fallthrough;
    case 5: /* See if result should be NULL */
        if(NULL == argv[4]) pResult = NULL;
        __fallthrough;
    case 4: /* Get Response Count Offset (to change response count for negative tests) */
        cbResponseOffset = (NULL == argv[3]? cbResponseOffset: atoi(argv[3]));
        __fallthrough;
    case 3: /* Get Response from file */
        if(NULL != argv[2])
        {
            pchResponseFile = argv[2];
            ChkBOOL(LoadTestFile(NULL,
                pchResponseFile,
                &pbResponse,
                &cbResponse),
                DRM_E_FILE_READ_ERROR);
        }
        else
        {
            pbResponse = NULL;
            cbResponse = 0;
        }
        __fallthrough;
    case 2: /* Check if DRM Context should be NULL */
        if(NULL == argv[1])
            pDrmContext = NULL;
        else
        {
            DRM_BYTE by = 0;
            ChkDR( DRM_LongToByte( atoi(argv[1]), &by ) );
            ChkDR( DRM_ByteAdd( *pDrmContext->rgbOpaqueBuffer, by, pDrmContext->rgbOpaqueBuffer ) );
        }
        __fallthrough;
    case 1: /* Get Test Category */
        testCategory = (NULL == argv[0]? testCategory: atoi(argv[0]));
    }


    cbResponse = cbResponse + cbResponseOffset;

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    if(0 == testCategory) /* Process Join Response */
    {
        dr = Drm_JoinDomain_ProcessResponse(pDrmContext,
            dwFlags,
            pbResponse,
            cbResponse,
            pResult,
            pDomainID);
    }
    else /* Process Leave Response */
    {
        dr = Drm_LeaveDomain_ProcessResponse(pDrmContext,
            pbResponse,
            cbResponse,
            pResult);
    }
PREFAST_POP

ErrorExit:
    SAFE_OEM_FREE(pbResponse);
    return dr;
}



/*********************************************************************
**           Name: _TestDrm_GetAdditionalResponseData
**
**    Description: Internal test function that executes various test scenarios
**                 for getting additional response data
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - Response filename
**                            argv[1] - Response count
**                            argv[2] - Data Type
**                            argv[3] - Data
**                            argv[4] - Data Count
**                 pDrmContext - Pointer to DRM context
**
**         Output: ppbDataString - Double Pointer to Data String
**                 pcbDataString - Pointer to Data String count
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

static DRM_RESULT _TestDrm_GetAdditionalResponseData(
    __in long argc,
    __in_ecount(argc) char **argv,
    __in DRM_APP_CONTEXT *pDrmContext,
    __deref_out_ecount_opt(*pcbDataString) DRM_CHAR **ppbDataString,
    __out DRM_DWORD *pcbDataString)
{
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;
    DRM_DWORD cbResponseOffset = 0;
    const DRM_CHAR *pchResponseFile = NULL;
    DRM_DWORD dwDataType = 0;
    DRM_CHAR *pbDataString = NULL;

    /* Result */
    DRM_RESULT dr = DRM_SUCCESS;

    /* Process arguments */
    switch(argc)
    {
    case 5: /* Will be processed later */
        __fallthrough;
    case 4: /* Will be processed later */
        __fallthrough;
    case 3: /* Get Data Type */
        dwDataType = (NULL == argv[2]? dwDataType: atoi(argv[2]));
        __fallthrough;
    case 2:
        cbResponseOffset = (NULL == argv[1]? cbResponseOffset: atoi(argv[1]));
        __fallthrough;
    case 1: /* Get Response from file */
        if(NULL != argv[0])
        {
            pchResponseFile = argv[0];
            ChkBOOL(LoadTestFile(NULL,
                pchResponseFile,
                &pbResponse,
                &cbResponse),
                DRM_E_FILE_READ_ERROR);
        }
        else
        {
            pbResponse = NULL;
            cbResponse = 0;
        }
    }

    /* Get count */
    dr = Drm_GetAdditionalResponseData (
        m_pContextManagerDomain,
        pbResponse,
        cbResponse,
        dwDataType,
        NULL,
        pcbDataString );

        cbResponse = cbResponse + cbResponseOffset ;

    /* Allocate */
    ChkArg( DRM_E_BUFFERTOOSMALL == dr || DRM_E_INVALIDARG == dr );
    ChkMem( pbDataString = (DRM_CHAR *)Oem_MemAlloc( max( 1, *pcbDataString ) ) );
    ZEROMEM( pbDataString, max( 1, *pcbDataString ) );

    /*
    ** The above allocation procedure ensures that we don't use a NULL pointer with 0 size and
    ** invalidate the test (Oem_MemAlloc may return a NULL pointer if asked for a size == 0).
    */

    switch(argc)
    {
    case 5: /* Check if Data count should be NULL */
        if(NULL == argv[4]) /* Check if Data Count should be NULL */
        {
            pcbDataString = NULL;
        }
        else /* Otherwise use the value as an offset for negative test cases */
        {
            *pcbDataString = *pcbDataString + atol(argv[4]);
        }
        __fallthrough;
    case 4:
        if(NULL == argv[3]) /* Check if Data should be NULL */
        {
            SAFE_OEM_FREE(pbDataString);
        }
    }

    /* Final Call */
    dr = Drm_GetAdditionalResponseData (
        m_pContextManagerDomain,
        pbResponse,
        cbResponse,
        dwDataType,
        pbDataString,
        pcbDataString );

ErrorExit:
    *ppbDataString = pbDataString;
    SAFE_OEM_FREE(pbResponse);
    return dr;
}



/*********************************************************************
**           Name: _TestDrm_DomainCert_Find
**
**    Description: Internal test function that executes various test scenarios
**                 for getting domain certificate
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - DRM App Context
**                            argv[1] - Account ID
**                            argv[2] - Service ID
**                            argv[3] - Revision ID
**                            argv[4] - Cert
**                            argv[5] - Cert Count
**                 pDrmContext - Pointer to DRM context
**
**         Output: ppbCert - Double Pointer to Certificate
**                 pcbCert - Pointer to Certificate
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

static DRM_RESULT _TestDrm_DomainCert_Find(
    __in long argc,
    __in_ecount(argc) char **argv,
    __in DRM_APP_CONTEXT *pDrmContext,
    __deref_out_ecount_opt(*pcbCert) DRM_BYTE **ppbCert,
    __out DRM_DWORD *pcbCert)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAIN_ID DomainID = {0};
    const DRM_DOMAIN_ID *pDomainID = &DomainID;
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrServiceID = EMPTY_DRM_STRING;

    DRM_DWORD cbSize = SIZEOF(DRM_GUID);
    DRM_BYTE *pbCert = NULL;
    DRM_DWORD cbCert = 0;
    pcbCert = &cbCert;

    /* Prepare Account ID */
    ChkDR( MakeDRMString(&dstrAccountID, "NjU0MzIxMDk4NzY1NDMyMQ==") );
    ChkDR(DRM_B64_DecodeW(&dstrAccountID,
        &cbSize,
        (DRM_BYTE*)&DomainID.m_oAccountID,
        0));

    cbSize = SIZEOF(DRM_GUID);

    /* Prepare Service ID */
    ChkDR( MakeDRMString(&dstrServiceID, "AH+03juKbUGbHl1V/QIwRA==") );
    ChkDR(DRM_B64_DecodeW(&dstrServiceID,
        &cbSize,
        (DRM_BYTE*)&DomainID.m_oServiceID,
        0));

    /* Process arguments */
    switch(argc)
    {
    case 6: /* Will be processed later */
        __fallthrough;
    case 5: /* Will be processed later */
        __fallthrough;
    case 4: /* Get Revision ID */
        if(NULL == argv[3])
        {
            DomainID.m_dwRevision = 0;
        }
        else
        {
            DomainID.m_dwRevision = (DRM_DWORD)atol(argv[3]);
        }
        __fallthrough;
    case 3: /* Get Service ID */
        if(NULL == argv[2])
        {
            ZEROMEM(&DomainID.m_oServiceID, SIZEOF(DRM_GUID));
        }
        else
        {
            cbSize = SIZEOF(DRM_GUID);

            FREEDRMSTRING( dstrServiceID );
            ChkDR( MakeDRMString(&dstrServiceID, argv[1]) );
            ChkDR(DRM_B64_DecodeW(&dstrServiceID,
                &cbSize,
                (DRM_BYTE*)&DomainID.m_oServiceID,
                0));
        }
        __fallthrough;
    case 2: /* Get Account ID */
        if(NULL == argv[1])
        {
            ZEROMEM(&DomainID.m_oAccountID, SIZEOF(DRM_GUID));
        }
        else
        {
            cbSize = SIZEOF(DRM_GUID);

            FREEDRMSTRING( dstrAccountID );
            ChkDR( MakeDRMString(&dstrAccountID, argv[1]) );
            ChkDR(DRM_B64_DecodeW(&dstrAccountID,
                &cbSize,
                (DRM_BYTE*)&DomainID.m_oAccountID,
                0));
        }
        __fallthrough;
    case 1: /* See if context is NULL */
        if(NULL == argv[0])
            pDrmContext = NULL;
        else /* For negative test cases */
        {
            DRM_BYTE by = 0;
            ChkDR( DRM_LongToByte( atoi(argv[0]), &by ) );
            ChkDR( DRM_ByteAdd( *pDrmContext->rgbOpaqueBuffer, by, pDrmContext->rgbOpaqueBuffer ) );
        }
    }

    if(argc > 3 && NULL == argv[1] && NULL == argv[2] && NULL == argv[3]) pDomainID = NULL;

    /* Get size of pbCert */
PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    dr = Drm_DomainCert_Find(pDrmContext,
        pDomainID,
        NULL,
        pcbCert);
PREFAST_POP

    /* Allocate memory for the cert */
    ChkArg( DRM_E_BUFFERTOOSMALL == dr || DRM_E_INVALIDARG == dr );
    ChkMem( pbCert = (DRM_BYTE *)Oem_MemAlloc( max( 1, *pcbCert ) ) );

    switch(argc)
    {
    case 6: /* Check if Challenge count should be NULL */
        if(NULL == argv[5]) /* Check if Cert Count should be NULL */
            pcbCert = NULL;
        else /* Otherwise use the value as an offset for negative test cases */
        {
            *pcbCert = *pcbCert + atol(argv[5]);
        }
        __fallthrough;
    case 5:
        if(NULL == argv[4]) /* Check if Cert should be NULL */
        {
            SAFE_OEM_FREE(pbCert);
        }
        else
        {
            /*
            ** Make sure that it doesn't use a NULL pointer with 0 size and invalidate the test
            ** Oem_MemAlloc may return a NULL pointer if asked for a size == 0
            */
            #pragma prefast(push)
            #pragma prefast(disable:28924) /* PreFast Complains about a redundant check of pbCert, the check is Needed, */
            if ( pcbCert != NULL && *pcbCert == 0 && pbCert == NULL )
            {
                ChkMem( pbCert = ( DRM_BYTE * ) Oem_MemAlloc(1) );
            }
            #pragma prefast(pop)
        }
    }

    /* Final call */
    dr = Drm_DomainCert_Find(pDrmContext,
        pDomainID,
        pbCert,
        pcbCert);

ErrorExit:
    FREEDRMSTRING( dstrAccountID );
    FREEDRMSTRING( dstrServiceID );
    *ppbCert = pbCert;
    return dr;
}



/*********************************************************************
**           Name: _TestDrm_DomainCert_InitEnum
**
**    Description: Internal test function that executes various test scenarios
**                 for initializing DRM_DOMAIN_CERT_ENUM_CONTEXT
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - DRM App Context
**                            argv[1] - Cert Context
**                 pDrmContext - Pointer to DRM context
**
**         Output: pCertContext - Pointer to Cert Context
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

static DRM_RESULT _TestDrm_DomainCert_InitEnum(
    __in long argc,
    __in_ecount(argc) char **argv,
    __in DRM_APP_CONTEXT *pDrmContext,
    __out DRM_DOMAIN_CERT_ENUM_CONTEXT *pCertContext)
{
    DRM_RESULT dr = DRM_SUCCESS;
    switch(argc)
    {
    case 2:
        if(NULL == argv[1]) /* Check if Cert Context should be NULL */
            pCertContext = NULL;
        __fallthrough;
    case 1:
        if(NULL == argv[0])
            pDrmContext = NULL;
        else /* For negative test cases */
        {
            DRM_BYTE by = 0;
            ChkDR( DRM_LongToByte( atoi(argv[0]), &by ) );
            ChkDR( DRM_ByteAdd( *pDrmContext->rgbOpaqueBuffer, by, pDrmContext->rgbOpaqueBuffer ) );
        }
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( Drm_DomainCert_InitEnum(pDrmContext, pCertContext) );
PREFAST_POP
ErrorExit:
    return dr;
}



/*********************************************************************
**           Name: _TestDrm_DomainCert_EnumNext
**
**    Description: Internal test function that executes various test scenarios
**                 for enumerating certificates
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                          argv[0] - Cert context
**                          argv[1] - Domain cert or domain key flag
**                          argv[2] - Cert context init flag
**                          argv[3] - Cert count
**                          argv[4] - Cert info
**                pCertContext - Pointer to Cert Context
**
**         Output: pcCert - Pointer to count of certificate
**                 pCertInfo - Pointer to certificate Info
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                            - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

static DRM_RESULT _TestDrm_DomainCert_EnumNext(
    __in long argc,
    __in_ecount(argc) char **argv,
    __in DRM_DOMAIN_CERT_ENUM_CONTEXT *pCertContext,
    __out DRM_DWORD *pcCert,
    __out DRM_DOMAINCERT_INFO *pCertInfo)
{
    DRM_DWORD cCert = 0;
    DRM_DOMAINCERT_INFO CertInfo = {0};
    DRM_RESULT dr = DRM_SUCCESS;

    pcCert = &cCert;
    pCertInfo = &CertInfo;

    switch(argc)
    {
    case 5:
        if(NULL == argv[4]) pCertInfo = NULL;
        __fallthrough;
    case 4:
        if(NULL == argv[3]) pcCert = NULL;
        __fallthrough;
    case 3:
        if(NULL != pCertContext)
        {
            if(0 == atoi(argv[2]))
                pCertContext->m_fInited = FALSE;
            else
                pCertContext->m_fInited = TRUE;
        }
        __fallthrough;
    case 2:
        if(NULL != pCertContext)
        {
            if(0 == atoi(argv[1]))
                pCertContext->m_fCert = FALSE;
            else
                pCertContext->m_fCert = TRUE;
        }
        __fallthrough;
    case 1:
        if(NULL == argv[0]) /* Check if Cert Context should be NULL */
            pCertContext = NULL;
    }

    /* Call to get count and info */
PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    dr = Drm_DomainCert_EnumNext(
        pCertContext,
        pcCert,
        pCertInfo );
PREFAST_POP

    return dr;
}



/*********************************************************************
**           Name: TestDrm_GenerateChallenge
**
**    Description: Test function that executes various test scenarios for
**                   generating Join or Leave challenge
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - Test Category. 0: Join, 1: Leave
**                            argv[1] - DRM App Context
**                            argv[2] - Flags
**                            argv[3] - Account ID
**                            argv[4] - Service ID
**                            argv[5] - Revision
**                            argv[6] - Friendly Name
**                            argv[7] - Friendly Name Count
**                            argv[8] - Custom Data
**                            argv[9] - Custom Data Count
**                            argv[10] - Challenge
**                            argv[11] - Challenge Count
**                            argv[12] - Max Challenge Size
**         Output: None
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

DRM_RESULT DRM_CALL TestDrm_GenerateChallenge(
    __in long argc,
    __in_ecount(argc) char **argv)
{
    DRM_BYTE        *pbChallenge        = NULL;
    DRM_DWORD        cbChallenge        = 0;
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_RESULT       dr2                = DRM_SUCCESS;
    DRM_CONST_STRING filename           = EMPTY_DRM_STRING;
    long             args               = argc;
    const DRM_CHAR  *argverif[5]; /* For challenge validation */
    DRM_DWORD        dwMaxChallengeSize = 0;

    if(argc > 12) args = 12;

    dr2 = _TestDrm_GenerateChallenge(
        args,
        argv,
        m_pContextManagerDomain,
        &pbChallenge,
        &cbChallenge);

    /* Log Challenge in a file */
    if(argc > 12 && (NULL != argv[12]))
    {
        ChkDR( MakeDRMString(&filename, argv[12]) );
        SaveToFileW(filename.pwszString, pbChallenge, cbChallenge);
    }

    if(argc > 13 && NULL != argv[13])
    {
        dwMaxChallengeSize = (DRM_DWORD)atol(argv[13]);
        if(cbChallenge > dwMaxChallengeSize)
        {
            Log("Trace", "From TestDrm_GenerateChallenge: Actual challenge is larger than max. allowed challenge");
            ChkDR(DRM_E_FAIL);
        }
    }

    dr = dr2;
    /* Validate Challenge */
    if(dr == 0 && pbChallenge != NULL && cbChallenge > 0 && argc > 8)
    {
        argverif[0] = "2"; /* Version */
        argverif[1] = argv[4]; /* Service ID */
        argverif[2] = argv[3]; /* Account ID */
        argverif[3] = argv[6]; /* Friendly Name */
        argverif[4] = argv[8]; /* Custom Data */
        ChkDR(_TestValidateChallenge(pbChallenge,
            cbChallenge,
            (atoi(argv[0]) == 0? TRUE:FALSE),
            (atoi(argv[2]) == 2? TRUE:FALSE),
            argverif));
    }

ErrorExit:
    SAFE_OEM_FREE(pbChallenge);
    FREEDRMSTRING( filename );
    return dr;
}



/*********************************************************************
**           Name: TestDrm_ProcessResponse
**
**    Description: Test function that executes various test scenarios
**                 for processing response
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - Test Category. 0: Join, 1: Leave
**                            argv[1] - DRM App Ccntext
**                            argv[2] - Response filename
**                            argv[3] - Response count
**                            argv[4] - Result
**                            argv[5] - DomainID
**                            argv[6] - Expected Result
**
**         Output: None
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

DRM_RESULT DRM_CALL TestDrm_ProcessResponse(
    __in long argc,
    __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT Result = DRM_SUCCESS;
    DRM_DOMAIN_ID DomainID = {0};
    long args = argc;

    /* Any argument greater than 6 will be processed here */
    if(argc > 6) args = 6;

    dr = _TestDrm_ProcessResponse(args,
        argv,
        m_pContextManagerDomain,
        &Result,
        &DomainID,
        DRM_PROCESS_DJ_RESPONSE_SIGNATURE_NOT_REQUIRED );
    /* Additional Validation */

    if(argc > 6 && (NULL != argv[6]))
    {
        ChkBOOL(Result == (DRM_RESULT)strtoul( argv[6], (char **)NULL, 0 ), DRM_E_FAIL);
    }

ErrorExit:
    return dr;
}


DRM_RESULT DRM_CALL TestDrm_ProcessResponse_VerifySignature(
    __in long argc,
    __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT Result = DRM_SUCCESS;
    DRM_DOMAIN_ID DomainID = {0};
    long args = argc;

    /* Any argument greater than 6 will be processed here */
    if(argc > 6) args = 6;

    dr = _TestDrm_ProcessResponse(args,
        argv,
        m_pContextManagerDomain,
        &Result,
        &DomainID,
        DRM_PROCESS_DJ_RESPONSE_NO_FLAGS );
    /* Additional Validation */

    if(argc > 6 && (NULL != argv[6]))
    {
        ChkBOOL(Result == (DRM_RESULT)strtoul( argv[6], (char **)NULL, 0 ), DRM_E_FAIL);
    }

ErrorExit:
    return dr;
}



/*********************************************************************
**           Name: TestDrm_GetAdditionalResponseData
**
**    Description: Test function that executes various test scenarios
**                 for getting additional response data
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - Response filename
**                            argv[1] - Response count
**                            argv[2] - Data Type
**                            argv[3] - Data
**                            argv[4] - Data Count
**                            argv[5] - Expected Result
**
**         Output: None
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

DRM_RESULT DRM_CALL TestDrm_GetAdditionalResponseData(
    __in long argc,
    __in_ecount(argc) char **argv)
{
    DRM_CHAR *pbDataString = NULL;
    DRM_DWORD cbDataString = 0;
    DRM_RESULT dr = DRM_SUCCESS;
    long args = argc;

    /* Any argument greater than 6 will be processed here */
    if(argc > 5) args = 5;

    dr = _TestDrm_GetAdditionalResponseData(args,
        argv,
        m_pContextManagerDomain,
        &pbDataString,
        &cbDataString);

    /* Additional Validation */
    if(argc > 5 && (NULL != argv[5]) && (NULL != pbDataString))
    {
        ChkBOOL(strncmp(pbDataString, argv[5], cbDataString) == 0, DRM_E_FAIL);
    }

ErrorExit:
    SAFE_OEM_FREE( pbDataString );
    return dr;
}



/*********************************************************************
**           Name: TestDrm_DomainCert_Find
**
**    Description: Test function that executes various test scenarios
**                 for getting a domain certificate
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - DRM App Context
**                            argv[1] - Account ID
**                            argv[2] - Service ID
**                            argv[3] - Revision ID
**                            argv[4] - Cert
**                            argv[5] - Cert Count
**
**         Output: None
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

DRM_RESULT DRM_CALL TestDrm_DomainCert_Find(
    __in long argc,
    __in_ecount(argc) char **argv)
{
    DRM_BYTE *pbCert = NULL;
    DRM_DWORD cbCert = 0;
    DRM_RESULT dr = DRM_SUCCESS;
    long args = argc;

    if(argc > 6) args = 6;

    dr = _TestDrm_DomainCert_Find(args,
        argv,
        m_pContextManagerDomain,
        &pbCert,
        &cbCert);

    SAFE_OEM_FREE(pbCert);
    return dr;
}



/*********************************************************************
**           Name: TestDrm_DomainCert_InitEnum
**
**    Description: Test function that executes various test scenarios
**                 for initializing DRM_DOMAIN_CERT_ENUM_CONTEXT
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                            argv[0] - DRM App Context
**                            argv[1] - Cert Context
**
**         Output: None
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                              - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

DRM_RESULT DRM_CALL TestDrm_DomainCert_InitEnum(
    __in long argc,
    __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAIN_CERT_ENUM_CONTEXT CertContext;
    long args = argc;

    if(args > 2) args = 2;

    dr = _TestDrm_DomainCert_InitEnum(args,
        argv,
        m_pContextManagerDomain,
        &CertContext
        );

    return dr;
}



/*********************************************************************
**           Name: TestDrm_DomainCert_EnumNext
**
**    Description: Test function that executes various test scenarios
**                 for enumerating certificates
**
**          Input: argc - Count of arguments
**                 argv - Array of arguments
**                          argv[0] - Cert context
**                          argv[1] - Domain cert or domain key flag
**                          argv[2] - Cert context init flag
**                          argv[3] - Cert count
**                          argv[4] - Cert info
**
**         Output: None
**
**        Returns: DRM_RESULT - DRM_SUCCESS == Success
**                            - DRM_E_INVALIDARG == Invalid arguments
**
** Warnings/Notes: Additional return codes to be documented
**
*********************************************************************/

DRM_RESULT DRM_CALL TestDrm_DomainCert_EnumNext(
    __in long argc,
    __in_ecount(argc) char **argv)
{
    DRM_DOMAIN_CERT_ENUM_CONTEXT CertContext = {0};
    DRM_DWORD cCert = 0;
    DRM_DOMAINCERT_INFO CertInfo = {0};
    DRM_RESULT dr = DRM_SUCCESS;
    long args = argc;

    if(args > 5) args = 5;

    ChkDR(Drm_DomainCert_InitEnum(m_pContextManagerDomain, &CertContext));

    dr = _TestDrm_DomainCert_EnumNext(
        args,
        argv,
        &CertContext,
        &cCert,
        &CertInfo);

ErrorExit:
    return dr;
}



DRM_RESULT DRM_CALL MODULE_PRETESTCASE(Domain)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    GET_SHARED_APP_CONTEXT( m_pContextManagerDomain );

ErrorExit:

    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(Domain)(long lTCID, const char *strTCName)
{
    RemoveDRMFile(RMFILE_STORE);

    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST(Domain)
    API_ENTRY( TestDrm_GenerateChallenge )
    API_ENTRY( TestDrm_ProcessResponse )
    API_ENTRY( TestDrm_ProcessResponse_VerifySignature )
    API_ENTRY( TestDrm_DomainCert_Find )
    API_ENTRY( TestDrm_DomainCert_InitEnum )
    API_ENTRY( TestDrm_DomainCert_EnumNext )
    API_ENTRY( TestDrm_GetAdditionalResponseData )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

