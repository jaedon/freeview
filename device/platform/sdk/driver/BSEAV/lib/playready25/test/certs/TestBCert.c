/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmbcert.h>
#include <drmbcertbuilder.h>
#include <drmbcertparser.h>
#include <drmblackbox.h>
#include <TestBCert.h>

ENTER_PKTEST_NAMESPACE_CODE

/*****************************************************************************
** Function:    _tGetSetBCChainBuffer
**
** Synopsis:    Either writes over or outputs the static cert chain buffer
**              used in the cert building test cases.
**
** Arguments:   [pbBinCertChain] : Either the cert chain output buffer (fGet ==TRUE)
**                                 or the input cert chain which should replace the
**                                 current chain (fGet == FALSE)
**              [cbBinCertChain] : The size of the cert chain on input and output.
**              [fGet]           : Flag indicating if current cert chain should be
**                                 output or overwritten
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the output buffer is too small.
**                                     cbBinCertChain will hold the required buffer
**                                     size if this is returned
**
** Notes:       NOT THREAD SAFE!
**              Input chains of too long a size currently get truncated without
**              any error messages
**
******************************************************************************/
DRM_RESULT DRM_CALL _tGetSetBCChainBuffer( DRM_BYTE *pbBinCertChain, DRM_DWORD *pcbBinCertChain, DRM_BOOL fGet)
{
    DRM_RESULT       dr                          = DRM_E_TEST_INCOMPLETE;
    static DRM_BYTE  *pbStaticBinCertChain       = NULL;
    static DRM_DWORD cbStaticBinCertChain        = 0;


    if ( fGet )
    {
        Log("Trace", "Getting current bin cert chain");
        /* Make sure that output buffers can hold current chain */
        if ( *pcbBinCertChain < cbStaticBinCertChain )
        {
            /* if output buffer is too small, output the required size and return the proper error code */
            *pcbBinCertChain = cbStaticBinCertChain;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        /* Copy the current chain to the output buffer */
        MEMCPY( pbBinCertChain, pbStaticBinCertChain, cbStaticBinCertChain );
        *pcbBinCertChain = cbStaticBinCertChain;
    }
    else
    {
        Log("Trace", "Resetting current bin cert chain");

        SAFE_OEM_FREE( pbStaticBinCertChain );
        cbStaticBinCertChain = 0;

        if ( pbBinCertChain != NULL )
        {
            ChkMem( pbStaticBinCertChain = (DRM_BYTE *)Oem_MemAlloc( *pcbBinCertChain * SIZEOF(DRM_BYTE) ) );
            ZEROMEM( pbStaticBinCertChain, *pcbBinCertChain * SIZEOF(DRM_BYTE) );
            cbStaticBinCertChain = *pcbBinCertChain;
            MEMCPY( pbStaticBinCertChain, pbBinCertChain, cbStaticBinCertChain );
        }
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_LoadBinCertChain
**
** Synopsis:    Loads a binary certificate chain from a specified file.
**
** Arguments:   argv[0]   : The name of the test file to load
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Test files are required to be in a directory called "files" in
**              the same folder in which the executable is being run.
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_LoadBinCertChain(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr                  = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE   *pbTestBinCertChain = NULL;
    DRM_DWORD  cbTestBinCertChain  = 0;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkBOOL( LoadTestFile( "files", argv[0], &pbTestBinCertChain, &cbTestBinCertChain ), DRM_E_TEST_INVALIDARG );

    ChkDR( _tGetSetBCChainBuffer( pbTestBinCertChain, &cbTestBinCertChain, FALSE ) );

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbTestBinCertChain );
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(Certs)(long lTCID, const char *strTCName)
{
    DRM_RESULT              dr = DRM_E_TEST_INCOMPLETE;
    DRM_BCERT_BUILDER_DATA  Data = {0};

    ChkDR( _tGetSetCurCertBuildData( &Data, FALSE ) );
    ChkDR( DRM_BBX_Initialize( NULL, &g_BbxCtx ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(Certs)(long lTCID, const char *strTCName)
{
    DRM_RESULT              dr   = DRM_E_TEST_INCOMPLETE;

    /* Free any memory still allocated to the cert builder data and current cert chain buffer */
    ChkDR( _tGetSetBCChainBuffer( NULL, NULL, FALSE ) );

    ChkDR( Test_ClearBuilderData( 0, NULL ) );

    ChkDR( DRM_BBX_Shutdown( &g_BbxCtx ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST( Certs )
    API_ENTRY(Test_LoadBinCertChain)
    API_ENTRY(Test_SetDefaultBuilderData)
    API_ENTRY(Test_ClearBuilderData)
    API_ENTRY(Test_AddKeyToBuilderData)
    API_ENTRY(Test_SetSecurityLevel)
    API_ENTRY(Test_SetExpiration)
    API_ENTRY(Test_SetPCBuilderData)
    API_ENTRY(Test_SetDomainBuilderData)
    API_ENTRY(Test_SetDeviceBuilderData)
    API_ENTRY(Test_SetSilverLightBuilderData)
    API_ENTRY(Test_SetMeteringBuilderData)
    API_ENTRY(Test_SetServerBuilderData)
    API_ENTRY(Test_SetBuilderFlagData)
    API_ENTRY(Test_SetManufacturerInfo)
    API_ENTRY(Test_SetFeatureInfo)
    API_ENTRY(Test_AddCert)
    API_ENTRY(Test_AddExtendedDataToCert)
    API_ENTRY(Test_CompareCertChains)
    API_ENTRY(Test_SignAndAddCert)
    API_ENTRY(Test_ParseChainHeader)
    API_ENTRY(Test_ParseCertificate)
    API_ENTRY(Test_ParseCertificateChain)
    API_ENTRY(Test_VerifyCertFromChain)
    API_ENTRY(Test_VerifySecurityVersionForCert)
    API_ENTRY(Test_FindObjectInCertByType)
    API_ENTRY(Test_RSAToXML)
    API_ENTRY(Test_CreateDeviceCertificate)
    API_ENTRY(Test_CreateModelCertificate)
    API_ENTRY(Test_CreateCompanyCertificate)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
