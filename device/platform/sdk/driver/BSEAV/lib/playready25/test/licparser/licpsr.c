/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <LicParserTestAPI.h>

#include <stdlib.h>
#include <string.h>

#include "tclib.h"  /* this needs to be here because it #includes <windows.h> */

#include "drmcommon.h"
#include "oempkcrypto.h"
#include "drmlicense.h"
#include "oemcipher.h"
#include "drmcontextsizes.h"
#include "drmutilities.h"
#include <drmcontentrevocation.h>
#include <drmwmdrm.h>

#include "tstutils.h"
#include "tLicGen.h"
#include "tGetClientID.h"

#include <tOEMIMP.h>
#include <tOEMIMP_PD.h>

USING_PK_NAMESPACE
ENTER_PKTEST_NAMESPACE_CODE

/* globals */
DRM_CONST_STRING g_szLic, *g_pszLic;
DRM_CONST_STRING g_wszContentRevocation;
const DRM_WCHAR *g_pszUserLic = NULL;

/*
    Test API LicenseGetAttribute
    argv[0]: attribute name, can be NULL
    argv[1]: the eDRM_LICENSE_ATTRIBUTES value
    argv[2]: the status for the returned attribute value pointer: NORMAL, NULL
    argv[3]: the expected attribute value returned from LicenseGetAttribute.
*/
DRM_RESULT DRM_CALL Test_API_LicenseGetAttribute(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_CONST_STRING szAttrName, *pszAttrName, szValue;
    eDRM_LICENSE_ATTRIBUTES eAttribute;

    szAttrName.pwszString = NULL;

    ChkArg(argc == 4);

    if (argv[0]) {
        pszAttrName = &szAttrName;
        ChkDR( MakeDRMString(pszAttrName, argv[0]) );
    } else
        pszAttrName = NULL;

    ChkArg(argv[1]);
    eAttribute = (eDRM_LICENSE_ATTRIBUTES)atoi(argv[1]);

    ChkDR(DRM_WMDRM_LIC_GetAttribute(&g_szLic, pszAttrName, eAttribute, NULL, (argv[2])? &szValue: NULL, 0));

    if (argv[3] && CompDRMString(&szValue, argv[3]))
        dr = DRM_S_FALSE;
ErrorExit:
    FREEDRMSTRING( szAttrName );
    return dr;
}

/*
    Literally set the string as the license for later tests
    argv[0]: the license string
*/
DRM_RESULT DRM_CALL Test_SetLicenseString(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg(argc == 1);

    if (argv[0]) {
        ChkDR( MakeDRMString(&g_szLic, argv[0]) );
        SAFE_OEM_FREE(g_pszUserLic);
        g_pszUserLic = g_szLic.pwszString;
    } else
        g_pszLic = NULL;
ErrorExit:
    return dr;
}

/* Test API LicenseGetEvent
    argv[0]: Event name to retrieve
    argv[1]: Event type name, can be NULL
    argv[2]: status of the returned condition string: NORMAL or NULL
    argv[3]: status of the returned action string: NORMAL or NULL
    argv[4]: expected condition string
    argv[5]: expected action string
*/
DRM_RESULT DRM_CALL Test_API_LicenseGetEvent(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_CONST_STRING *pszEvent, szEvent, *pszType, szType, szCondition, szAction;

    szEvent.pwszString = szType.pwszString = NULL;

    ChkArg(argc == 6);

    if (argv[0]) { /* Set the event parameter */
        pszEvent = &szEvent;
        ChkDR( MakeDRMString(pszEvent, argv[0]) );
    } else
        pszEvent = NULL;

    if (argv[1]) { /* Set the type parameter */
        pszType = &szType;
        ChkDR( MakeDRMString(pszType, argv[1]) );
    } else
        pszType = NULL;

    ChkDR(DRM_WMDRM_LIC_GetEvent(&g_szLic, pszEvent, pszType,
        NULL,
        argv[2]? &szCondition: NULL,
        argv[3]? &szAction: NULL,
        NULL));

    /* Check if the returned condition and action match the expected values */
    if ((argv[4] && CompDRMString(&szCondition, argv[4])) ||
        (argv[5] && CompDRMString(&szAction, argv[5])))
        dr = DRM_S_FALSE;

ErrorExit:
    FREEDRMSTRING( szEvent );
    FREEDRMSTRING( szType );
    return dr;
}

/* Test API LicenseVerifySignature
    argv[0]: status of the crypto context: NORMAL or NULL
    argv[1]: status of the return result: NORMAL or NULL
    argv[2]: expected return value
*/
DRM_RESULT DRM_CALL Test_API_LicenseVerifySignature(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BB_CONTEXT *pcontextBBX = NULL;
    DRM_LONG lResult = 0;

    ChkArg(argc == 3);

    if (argv[0]) {
        ChkMem(pcontextBBX = (DRM_BB_CONTEXT*)Oem_MemAlloc(sizeof(DRM_BB_CONTEXT)));
        memset(pcontextBBX, 0, sizeof(DRM_BB_CONTEXT));
    }

    ChkDR(DRM_WMDRM_LIC_VerifySignature(g_pszLic, pcontextBBX, argv[1]? &lResult: NULL));

    if (argv[2] && lResult != atol(argv[2]))
        dr = DRM_S_FALSE;
ErrorExit:
    SAFE_OEM_FREE(pcontextBBX);
    return dr;
}

/* Test API LicenseVerifyCertChain
    argv[0]: value for fCheckExpiry: 0 or 1
    argv[1]: status of the crypto context: NORMAL or NULL
    argv[2]: status of the return result: NORMAL or NULL
    argv[3]: expected return value
*/
DRM_RESULT DRM_CALL Test_API_LicenseVerifyCertChain(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fCheckExpiry;
    DRM_LICEVAL_CONTEXT contextLEVL = {0};
    DRM_BB_CONTEXT *pcontextBBX = NULL;
    DRM_LONG lResult = 0;

    ChkArg(argc == 4 && argv[0]);

    fCheckExpiry = (DRM_BOOL)atoi(argv[0]);

    if (argv[1]) {
        ChkMem(pcontextBBX = (DRM_BB_CONTEXT*)Oem_MemAlloc(sizeof(DRM_BB_CONTEXT)));
        contextLEVL.pcontextBBX = pcontextBBX;
        memset(pcontextBBX, 0, sizeof(DRM_BB_CONTEXT));
    }

    ChkDR(DRM_WMDRM_LIC_VerifyCertChain(g_pszLic, fCheckExpiry, &contextLEVL, argv[2]? &lResult:NULL));
    if (argv[3] && lResult != atol(argv[3]))
        dr = DRM_S_FALSE;
ErrorExit:
    SAFE_OEM_FREE(pcontextBBX);
    return dr;
}

/* Test API LicenseGetContentRevocation
    argv[0]: index to the revocation string to retrieve
    argv[1]: the expected returned revocation sequence number for the content. Can be NULL optionally
    argv[2]: status of the return revocation string:NORMAL or NULL
    argv[3]: the expected returned content owner pub key. Can be NULL
    argv[4]: the expected returned condition string. Can be NULL
    argv[5]: the status for the crypto context: NULL or NORMAL
*/
DRM_RESULT DRM_CALL Test_API_LicenseGetContentRevocation(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CRYPTO_CONTEXT *pCryptoContext = NULL;
    if( DRM_CONTENTREVOCATION_IsContentRevocationSupported() )
    {
        DRM_CONST_STRING wszOwnerPubKey, wszCondition;
        DRM_DWORD dwRevocationSequenceNumber = 0;

        ChkArg(argc == 6 && argv[0] && argv[2]);

        if (argv[5]) {
            ChkMem(pCryptoContext = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc(sizeof(DRM_CRYPTO_CONTEXT)));
            memset(pCryptoContext, 0, sizeof(DRM_CRYPTO_CONTEXT));
        }

        ChkDR(DRM_CONTENTREVOCATION_LIC_GetContentRevocation(g_pszLic, (DRM_DWORD)atol(argv[0]),
            argv[1]? &dwRevocationSequenceNumber: NULL,
            argv[2]? &g_wszContentRevocation: NULL,
            argv[3]? &wszOwnerPubKey: NULL,
            argv[4]? &wszCondition: NULL,
            pCryptoContext
            ));

        /* Verify expected results */
        if ((argv[1] && dwRevocationSequenceNumber !=(DRM_DWORD) atol(argv[1])) ||
            (argv[3] && CompDRMString(&wszOwnerPubKey, argv[3])) ||
            (argv[4] && CompDRMString(&wszCondition, argv[4])))
            dr = DRM_S_FALSE;
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    SAFE_OEM_FREE(pCryptoContext);
    return dr;
}

/* Test API LicenseVerifyContentRevocation
    argv[0]: status of the content revocation string: NORMAL or NULL
    argv[1]: the expected returned license server public key in the revocation string. Can be NULL
    argv[2]: the expected returned revocation sequence number for the content. Can be NULL optionally
    argv[3]: the expected returned content owner pub key. Can be NULL
    argv[4]: the expected returned condition string. Can be NULL
    argv[5]: the status for the crypto context: NULL or NORMAL
*/
DRM_RESULT DRM_CALL Test_API_LicenseVerifyContentRevocation(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CRYPTO_CONTEXT *pCryptoContext = NULL;
    if( DRM_CONTENTREVOCATION_IsContentRevocationSupported() )
    {
        DRM_CONST_STRING wszLSPubKey, wszOwnerPubKey, wszCondition;
        DRM_DWORD dwRevocationSequenceNumber = 0;

        ChkArg(argc == 6);

        if (argv[5]) {
            ChkMem(pCryptoContext = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc(sizeof(DRM_CRYPTO_CONTEXT)));
            memset(pCryptoContext, 0, sizeof(DRM_CRYPTO_CONTEXT));
        }

        ChkDR(DRM_CONTENTREVOCATION_LIC_VerifyContentRevocation(
            argv[0]? &g_wszContentRevocation: NULL,
            argv[1]? &wszLSPubKey: NULL,
            argv[2]? &dwRevocationSequenceNumber: NULL,
            argv[3]? &wszOwnerPubKey: NULL,
            argv[4]? &wszCondition: NULL,
            pCryptoContext
            ));

        /* Verify expected results */
        if ((argv[1] && CompDRMString(&wszLSPubKey, argv[1])) ||
            (argv[2] && dwRevocationSequenceNumber != (DRM_DWORD)atol(argv[2])) ||
            (argv[3] && CompDRMString(&wszOwnerPubKey, argv[3])) ||
            (argv[4] && CompDRMString(&wszCondition, argv[4])))
            dr = DRM_S_FALSE;
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    SAFE_OEM_FREE(pCryptoContext);
    return dr;
}

/* Test API LicenseGetEnablingBits
    argv[0]: status of the license string: NORMAL or NULL
    argv[1]: expected value of algorithm in the enablingbits
    argv[2]: status of the pubkey in the enablingbits: NORMAL or NULL
    argv[3]: status of pdValueLen: NORMAL or NULL
    argv[4]: status of pbSignature: NORMAL or NULL
====
    argv[0]: status of the license string: NORMAL or NULL
    argv[1]: dwIndex in the chain
    argv[2]: expected value of algorithm in the enablingbits
    argv[3]: status of the pubkey in the enablingbits: NORMAL or NULL
    argv[4]: status of pdValueLen: NORMAL or NULL
    argv[5]: status of pVerPubKey: NORMAL or NULL
    argv[6]: status of pbSignature: NORMAL or NULL
    argv[7]: status of pChainedChecksum: NORMAL or NULL
    argv[8]: status of pContext: NORMAL or NULL
*/
DRM_RESULT DRM_CALL Test_API_LicenseGetEnablingBits(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwAlgo = 0;
    DRM_DWORD dwIndex = 0;
    PUBKEY pubKey = { 0 };
    PUBKEY VerPubKey = { 0 };
    DRM_BYTE *pbValue = NULL;
    DRM_BYTE pbSignature[DRM_ECC160_SIGNATURE_LEN] = {0};
    DRM_BYTE pChainedChecksum[CHECKSUM_LENGTH] = {0};
    DRM_CRYPTO_CONTEXT *pCryptoContext = NULL;
    DRM_DWORD cbValue = 0;

    ChkArg(argc == 9 && argv[1]);
    dwIndex = (DRM_DWORD)atol(argv[1]);

    if (argv[8]) {
        ChkMem(  pCryptoContext = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF(DRM_CRYPTO_CONTEXT) )  );
    }

    /* First to get the length of pbValue */
    dr = DRM_WMDRM_LIC_GetEnablingBits(argv[0]? &g_szLic: NULL,
        dwIndex,
        argv[2]? &dwAlgo: NULL,
        argv[3]? &pubKey: NULL,
        NULL,
        argv[4]? &cbValue: NULL,
        argv[5]? &VerPubKey: NULL,
        argv[6]? pbSignature: NULL,
        argv[7]? pChainedChecksum: NULL,
        pCryptoContext);
    if (dr != DRM_E_BUFFERTOOSMALL)
        ChkDR(dr);

    ChkMem(pbValue = (DRM_BYTE *)Oem_MemAlloc(cbValue));

    ChkDR(DRM_WMDRM_LIC_GetEnablingBits(argv[0]? &g_szLic: NULL,
        dwIndex,
        argv[2]? &dwAlgo: NULL,
        argv[3]? &pubKey: NULL,
        pbValue,
        argv[4]? &cbValue: NULL,
        argv[5]? &VerPubKey: NULL,
        argv[6]? pbSignature: NULL,
        argv[7]? pChainedChecksum: NULL,
        pCryptoContext));

    /* Verify algorithm */
    if (argv[2] && (dwAlgo != (DRM_DWORD)atol(argv[2])))
        dr = DRM_S_FALSE;
ErrorExit:
    SAFE_OEM_FREE(pbValue);
    SAFE_OEM_FREE(pCryptoContext);
    return dr;
}

/*
    argv[0]: option in creating a test license
    argv[1]: extra xml string to add to the license
    argv[2]: optional flag to the generated license type: 0(simple), 1(leaf) and 2(root)
*/
DRM_RESULT DRM_CALL Test_MakeLicense(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    ELicenseType eLicType = MAKE_SIMPLE_LICENSE;
    ChkArg(argc >= 2 && argv[0]);

    if (argc > 2) {
        ChkArg(argv[2]);
        eLicType = (ELicenseType)atol(argv[2]);
    }

    ChkDR(TestLicGenMakeLicense((DRM_DWORD)atol(argv[0]), argv[1],NULL, eLicType, &g_szLic, NULL, FALSE));
ErrorExit:
    return dr;
}

/*
    calls Test_SetTestKey to set keys for licgen
    argv[0]: what key to set?
    argv[1]: key value
*/
DRM_RESULT DRM_CALL Test_SetTestKey(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    ChkArg(argc == 2 && argv[0]);
    ChkDR(TestLicGenSetKeys(argv[0], argv[1]));
ErrorExit:
    return dr;
}

/*
    calls Test_SetTestKeyByName to set keys for licgen
    argv[0]: what key to set?
    argv[1]: key name
*/
DRM_RESULT DRM_CALL Test_SetTestKeyByName(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    ChkArg(argc == 2 && argv[0]);
    ChkDR(TestLicGenSetKeysByName(argv[0], argv[1]));
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(LicParser)(long lTCID, const char *strTCName)
{
    CLIENTID clientID;
    DRM_RESULT dr;

    RemoveDRMFile(RMFILE_STORE);
    ChkDR(SetDeviceEnv(DEVCERT_TEMPLATE_FILE_NAME,
                       PRIVATE_KEY_FILE_NAME,
                       GROUP_CERT_FILE_NAME,
                       GC_PRIVATE_KEY_FILE_NAME,
                       TRUE));
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR(TestGetClientID(&clientID));
        ChkDR(TestLicResponseSetClientID(&clientID));
    }
    ChkDR(TestLicGenInit(10000));

    memset(&g_szLic, 0, sizeof(g_szLic));
    memset(&g_wszContentRevocation, 0, sizeof(g_wszContentRevocation));
    g_pszLic = &g_szLic;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(LicParser)(long lTCID, const char *strTCName)
{
    TestLicGenShutDown();
    SAFE_OEM_FREE(g_pszUserLic);
    g_pszUserLic = NULL;
    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST(LicParser)
    API_ENTRY(Test_MakeLicense)
    API_ENTRY(Test_SetTestKey)
    API_ENTRY(Test_SetTestKeyByName)
    API_ENTRY(Test_API_LicenseGetAttribute)
    API_ENTRY(Test_SetLicenseString)
    API_ENTRY(Test_API_LicenseGetEvent)
    API_ENTRY(Test_API_LicenseVerifySignature)
    API_ENTRY(Test_API_LicenseVerifyCertChain)
    API_ENTRY(Test_API_LicenseGetContentRevocation)
    API_ENTRY(Test_API_LicenseVerifyContentRevocation)
    API_ENTRY(Test_API_LicenseGetEnablingBits)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
