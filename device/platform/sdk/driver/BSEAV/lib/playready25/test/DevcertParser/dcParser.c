/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmlicense.h>
#include <drmdevcert.h>
#include <drmxmlparser.h>
#include <DCParserTestAPI.h>

ENTER_PKTEST_NAMESPACE_CODE

DRM_CONST_STRING g_dstrDevCert;

/*
** Test API: DRM_DCP_GetAttribute
**  argv[0]: status of devcert: NORMAL, NULL
**  argv[1]: eDRM_DEVCERT_ATTRIBUTES value
**  argv[2]: expected return node value, IGNORE or NULL
**  argv[3]: expected return value, IGNORE or NULL
*/
DRM_RESULT DRM_CALL TEST_API_DevCertGetAttribute(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_CONST_STRING dstrRetNode, dstrRetVal;

    ChkArg( argc == 4 && argv[1] );

    ChkDR( DRM_DCP_GetAttribute(
        NULL,
        argv[0] ? &g_dstrDevCert: NULL,
        (eDRM_DEVCERT_ATTRIBUTES)atol( argv[1] ),
        argv[2] ? &dstrRetNode: NULL,
        argv[3] ? &dstrRetVal: NULL ) );

    /* Now compare the return node to the expected node */
    if( argv[2] && strcmp( argv[2], "IGNORE" ) && CompDRMString( &dstrRetNode, argv[2] ) )
    {
        ((DRM_WCHAR*)dstrRetNode.pwszString)[dstrRetNode.cchString] = 0;
        Log( "UNEXPECTEDNODE", "Retrieved=%S,Expecting=%s", dstrRetNode.pwszString, argv[2] );
        dr = DRM_S_FALSE; /* Not matched! */
        goto ErrorExit;
    }

    /* Now compare the return value to the expected value */
    if( argv[3] && strcmp( argv[3], "IGNORE" ) && CompDRMString( &dstrRetVal, argv[3] ) )
    {
        ((DRM_WCHAR*)dstrRetVal.pwszString)[dstrRetVal.cchString] = 0;
        Log( "UNEXPECTEDVALUE", "Retrieved=%S,Expecting=%s", dstrRetVal.pwszString, argv[3] );
        dr = DRM_S_FALSE; /* Not matched! */
        goto ErrorExit;
    }

ErrorExit:
    return dr;
}

/*
** Test API DRM_DCP_VerifyDeviceCert
**  argv[0]: devcert status: NORMAL or NULL, EMPTY
**  argv[1]: verify flag
**  argv[2]: crypto context status: NORMAL or NULL
*/
DRM_RESULT DRM_CALL TEST_API_DevCertVerifyDeviceCert(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_CRYPTO_CONTEXT *pCrypto = NULL;
    DRM_CONST_STRING dstrEmpty = {0};
    const DRM_CONST_STRING *pdstrDevcert = NULL;

    ChkArg( argc == 3 && argv[1] );
    if( argv[0] )
    {
        pdstrDevcert = *(argv[0]) ? &g_dstrDevCert : &dstrEmpty;
    }

    if( argv[2] )
    {
        ChkMem( pCrypto = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF(DRM_CRYPTO_CONTEXT ) ) );
        ZEROMEM( pCrypto, SIZEOF(DRM_CRYPTO_CONTEXT) );
    }

    ChkDR( DRM_DCP_VerifyDeviceCert( NULL, pdstrDevcert, (DRM_DWORD)atol(argv[1]), NULL, pCrypto ) );

ErrorExit:
    SAFE_OEM_FREE(pCrypto);
    return dr;
}

/*
** Modify the global g_dcStruct for other tests.
**  argv[0]: operation: Remove, Modify
**  argv[1]: node name
**  argv[2]: attribute name
**  argv[3]: path
*/
DRM_RESULT DRM_CALL TEST_ChangeDevcertNode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_CONST_STRING dstrNode = {0}, dstrAttr = {0}, dstrPath = {0};
    DRM_CONST_STRING dstrSubNode      = {0};
    DRM_CONST_STRING dstrNodeToModify = {0};
    DRM_CONST_STRING dstrDataToModify = {0};

    ChkArg(argc == 4 && argv[0]);

    ChkDR( MakeDRMString( &dstrNode, argv[1] ) );
    ChkDR( MakeDRMString( &dstrAttr, argv[2] ) );
    ChkDR( MakeDRMString( &dstrPath, argv[3] ) );

    /* Search the part to remove */
    ChkDR( DRM_XML_GetSubNode( &g_dstrDevCert, &dstrNode, argv[2] ? &g_dstrAttributeType : NULL, &dstrAttr, 0, &dstrSubNode, NULL, 1 ) );

    if( dstrPath.pwszString )
    {
        ChkDR( DRM_XML_GetSubNodeByPath( &dstrSubNode, &dstrPath, NULL, NULL, &dstrNodeToModify, &dstrDataToModify, g_wchForwardSlash ) );
    }
    else
    {
        memcpy( &dstrNodeToModify, &dstrSubNode, SIZEOF(dstrDataToModify) );
    }

    if( !strcmp( argv[0], "Remove" ) )
    {
        DRM_STRING dstrDevcert;
        memcpy( &dstrDevcert, &g_dstrDevCert, SIZEOF(dstrDevcert) );
        ChkDR( DRM_UTL_StringRemoveSubString( &dstrDevcert, &dstrNodeToModify ) );
        memcpy( &g_dstrDevCert, &dstrDevcert, SIZEOF(g_dstrDevCert) );

    }
    else if( !strcmp( argv[0], "Modify" ) )
    {
        DRM_WCHAR_NATIVE ch;

        ChkArg( dstrDataToModify.pwszString );

        /* Convert to native before incrementing to ensure that incrementing doesn't generate invalid WCHAR sequences (on big-endian). */
        ch = NATIVE_WCHAR( dstrDataToModify.pwszString[0] );
        ch++;
        ((DRM_WCHAR*)dstrDataToModify.pwszString)[0] = WCHAR_CAST( ch );
    }

ErrorExit:
    FREEDRMSTRING( dstrNode );
    FREEDRMSTRING( dstrAttr );
    FREEDRMSTRING( dstrPath );
    return dr;
}

/*
** roll forward the machine clock in years to expire everything
**  argv[0]: years to roll forward
*/
DRM_RESULT DRM_CALL TEST_RollForwardClockInYears(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    long lSeconds;

    ChkArg( argc == 1 && argv[0] );
    lSeconds = atol(argv[0]) * 365 * 24 * 60 * 60;
    ChkDR( tChangeSystemTime(lSeconds) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(DCParser)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    tResetSystemTimeOffset();
    g_dstrDevCert.pwszString = NULL;

    ChkArg( LoadTestFile( NULL, "devcert.dat", (DRM_BYTE **)&g_dstrDevCert.pwszString, &g_dstrDevCert.cchString ) );
    g_dstrDevCert.cchString /= SIZEOF(DRM_WCHAR);

ErrorExit:
    return dr;
}


DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(DCParser)(long lTCID, const char *strTCName)
{
    /* reset system clock to current year */
    tResetSystemTimeOffset();

    FREEDRMSTRING( g_dstrDevCert );
    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST( DCParser )
    API_ENTRY(TEST_API_DevCertGetAttribute)
    API_ENTRY(TEST_API_DevCertVerifyDeviceCert)
    API_ENTRY(TEST_ChangeDevcertNode)
    API_ENTRY(TEST_RollForwardClockInYears)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
