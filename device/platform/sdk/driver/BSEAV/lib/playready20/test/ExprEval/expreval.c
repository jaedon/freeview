/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <ExprEvalTestAPI.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tclib.h"  /* this needs to be here because it #includes <windows.h> */

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcontextsizes.h>
#include "tOEMImp.h" 
#include "tOEMImp_PD.h"

#include <drmcrt.h>
#include <drmresults.h>
#include <drmtypes.h>
#include <drmliceval.h>
#include <drmheaderparser.h>
#include <drmmanager.h>
#include <oempkcrypto.h>
#include <drmlicacq.h>
#include <oemcipher.h>
#include <drmblackbox.h>
#include <drmviewrights.h>

#include <drmrevocation.h>

#include "tstutils.h"
#include "tstHDSutils.h"
#include "tstXMLutils.h"
#include "expreval.h"
#include "licenses.h"
#include "teststubcerts.h"
#include <drmapprevocation.h>
#include <drmwmdrm.h>

ENTER_PKTEST_NAMESPACE_CODE

static DRM_DST *g_pDatastore = NULL;

/*
argv[0]: iOptions: the field of the LicEvalContext to be overwritten
argv[1]: instruction for this field: valid, wrong, invalid, NULL

iOptions:
    0.  all valid fields
    1.  pContextBBX;
    2.  ExprEvalContext;
    3.  dstrContentHeader;
    4.  dstrContentLicense;
    5.  dstrBBVer;
    6.  dstrDRMVer;
    7.  pcontextSSTLicense;
    8.  pcontextSSTGlobal;
    9.  pcontextSSTRevocation;
    12. rgbLID;
    13. rgbKID;
    14. fGlobalSecStoreWritable;
    15. dwFlags;        //Default as this LICEVAL_OPEN_CHECK_SETTINGS 
    16. pbBuffer;       //User gives a point to work in.  This is actually going to be used for revocation from the secure store 
    17. cbBuffer;       //Size of the user supplied pointer     
    18. fReserved;      // Must be initialized to 0 
    19. SecureStoreGlobalPassword // Talk to blackbox to get the global secure store password
*/

DRM_RESULT DRM_CALL TestLicEvalContextSetting2(long argc, __in_ecount(argc) char **argv)
{
    int iOptions = 0;
    int idx;
    DRM_BYTE rgbBuffer[MAX_DEVICE_CERT_SIZE];    
    DRM_CONST_STRING dstrDevCert;
    DRM_BYTE szSecStorePassword[DRM_SHA1_DIGEST_LEN];
    DRM_LID lid;

    Log("Trace", "\t\tFrom TestLicEvalContextSetting2, argc = %d", argc);

    DSTR_FROM_PB( &dstrDevCert, rgbBuffer, SIZEOF(rgbBuffer) );

    for(idx=0; idx<argc; idx++){
        Log("Trace", "\t\tFrom TestLicEvalContextSetting2: argv[%d]=%s", idx, argv[idx]);
    }
    
    /* at least 2 arguments, 1st arg for options, 2nd arg for the instruction pass in. */
    ChkArg(argc > 1);

    iOptions = atoi(argv[0]);
    
    memset( &(Context), 0, sizeof( DRM_LICEVAL_CONTEXT ) );

    if (iOptions != 15) /* dwFlags: Default as this LICEVAL_OPEN_CHECK_SETTINGS */
        Context.dwFlags = LICEVAL_OPEN_CHECK_SETTINGS;    
    else if (!argv[1])
        Context.dwFlags =  0;
    else if(!strcmp(argv[1],"valid"))
        Context.dwFlags = LICEVAL_STORE_CHECK_SETTINGS;
    else if (!strcmp(argv[1],"invalid"))               
        Context.dwFlags = LICEVAL_OPEN_CHECK_SETTINGS | 8;

    if(iOptions!=1)
        Context.pcontextBBX = &oBBContext; 
    else            /* NULL CryptoContext */
        Context.pcontextBBX = NULL;

    if( oBBContext.pKeyFileContext == NULL )
    {
        ChkMem( oBBContext.pKeyFileContext = ( DRM_KEYFILE_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_KEYFILE_CONTEXT ) ) );
        ChkDR( DRM_KF_Initialize( NULL, oBBContext.pKeyFileContext ) );
        ChkDR( DRM_KF_Open( oBBContext.pKeyFileContext, TRUE, OEM_GENERIC_READ ) );
        ChkDR( DRM_KF_LoadFromFile( oBBContext.pKeyFileContext, TRUE ) );
        ChkDR( DRM_BBX_SetupSecureStoreKey( oBBContext.pKeyFileContext, NULL ) );
        ChkDR( DRM_KF_Close( oBBContext.pKeyFileContext ) );
    }

    /*blackbox initialized */
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_SetWMDRMCertCachedValues( &oBBContext ) );
    }

    ChkDR( DRM_BBX_Initialize( NULL, &oBBContext ) );

    if(iOptions!=3) {      /* 3.  dstrContentHeader; */
        ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentHeader, wszHeader );
    } else if (argv[1]) {
        if(!strcmp(argv[1],"DefaultContentHeader")) {
            ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentHeader, wszDefaultHeader );
        } else if(!strcmp(argv[1],"OldContentHeader")) {
            ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentHeader, wszOldHeader );
        } else if(!strcmp(argv[1],"BogusContentHeader")) {
            ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentHeader, wszBogusHeader );
        }
    }
    
    if(iOptions!=4) {     /* 4.  dstrContentLicense; */
        ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentLicense, wszLicense );
    } else if (!argv[1])
        Context.dstrContentLicense.pwszString = NULL;
    else if(!strcmp(argv[1],"wszOldLicense")) {
        ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentLicense, wszOldLicense);
    } else if(!strcmp(argv[1],"wszInvalidLicense")) {
        ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentLicense, wszInvalidLicense );
    } else if(!strcmp(argv[1],"NOStoreRight")) {
        ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentLicense,wszNOStoreRightLicense );
    } else if(!strcmp(argv[1],"NOSelectRight"))    {
        ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentLicense,wszNOSelectRightLicense );
    } else if(!strcmp(argv[1],"NODeleteRight"))    {
        ASSIGN_WSTR_TO_DRMSTRING( Context.dstrContentLicense,wszNODeleteRightLicense );
    }
    
    if(iOptions!=13) { /* 13. rgbKID; */
        ChkDR( DRM_HDR_GetAttribute(&(Context.dstrContentHeader), NULL, DRM_HEADER_ATTRIB_KID, &wszKID, 0) );
    } else if (!argv[1])
        wszKID.pwszString = NULL;
    else if(!strcmp(argv[1],"InvalidKID")) {
        ASSIGN_WSTR_TO_DRMSTRING(wszKID, InvalidrgbKID);
    }
    
    if (iOptions != 14) /* fGlobalSecStoreWritable*/
        Context.fGlobalSecStoreWritable = TRUE;

    Context.pDatastore = g_pDatastore;

    ChkDR(DRM_LST_Open(&oLicStoreContext, g_pDatastore, eDRM_LICENSE_STORE_XML));
    /* Add the license to the store */

    ChkDR(DRM_LST_AddLicense(&oLicStoreContext, Context.dstrContentLicense.cchString * sizeof(DRM_WCHAR), (DRM_BYTE*)Context.dstrContentLicense.pwszString, NULL, NULL, 0));

    ChkDR(DRM_LST_InitEnum(&oLicStoreContext, NULL, FALSE, &oLicStoreEnumContext));
    ChkDR(DRM_LST_EnumNext(&oLicStoreEnumContext, (DRM_KID*)rgbKID, &lid, NULL, &cbKID)); /* start the enumerator */

    /* If the following base64 decode fails because the buffer isn't big enough then it isn't a valid KID anyway. */
    cbKID = sizeof( rgbKID );
    memset( rgbKID, 0, sizeof( rgbKID ) );
    ChkDR( DRM_B64_DecodeW(&wszKID, &cbKID, rgbKID, 0) );

    /*Talk to blackbox to get the global secure store password */
    if(iOptions!=19) {
        ChkDR(DRM_SST_CreateGlobalStorePassword(szSecStorePassword, (DRM_BYTE*)&oBBContext));
    } else if (!argv[1])
        MEMSET(szSecStorePassword, 0, sizeof( szSecStorePassword ));
    else if( 0 == strcmp(argv[1],"InvalidGlobalSecStorePassword"))
        MEMSET(szSecStorePassword, 1, sizeof( szSecStorePassword ));
    
    ChkMem( Context.pcontextSSTGlobal = (DRM_SECSTORE_CONTEXT*) Oem_MemAlloc( sizeof(DRM_SECSTORE_CONTEXT) ) );

    ChkDR( DRM_SST_OpenKeyTokens (
        Context.pcontextSSTGlobal, 
        &g_rgbSecStoreGlobalName, 
        NULL,
        szSecStorePassword, 
        DRM_SECURE_STORE_OPEN_ALWAYS,
        SECURE_STORE_GLOBAL_DATA, 
        g_pDatastore));
 
    /*add revocation:  revocation store not opened here */
    /* 9. pcontextSSTRevocation */
    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        if(iOptions!=9) {
            Context.cbRevocationBuffer = REVOCATION_BUFFER_SIZE;
            SAFE_OEM_FREE(Context.pbRevocationBuffer);
            ChkMem(Context.pbRevocationBuffer = (DRM_BYTE*)Oem_MemAlloc(Context.cbRevocationBuffer));
            memcpy(&Context.appcert, g_teststubcert, sizeof(APPCERT));

            ChkMem( Context.pcontextSSTRevocation = (DRM_SECSTORE_CONTEXT*) Oem_MemAlloc( sizeof(DRM_SECSTORE_CONTEXT)  ) );

            ChkMem( Context.pContextRev = (DRM_REVOCATIONSTORE_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_REVOCATIONSTORE_CONTEXT ) ) );
            ChkDR( DRM_RVS_InitRevocationStore( Context.pDatastore,
                                                     Context.pcontextSSTRevocation,
                                                     Context.pcontextBBX,
                                                     Context.pContextRev ) );
        }
        else if (!(argv[1]))  /* NULL */
        {
            Context.pcontextSSTRevocation = NULL;
        }
    }
    else
    {
        Context.pcontextSSTRevocation = NULL;
    }

    if(iOptions!=5) {      /* 5. dstrBBVer; */ 
        /* The override the real version number from the cert with 2.3.0.2 */
        ASSIGN_WSTR_TO_DRMSTRING(Context.dstrBBVer, g_wcszLegacyBB_VERSION_STRING);
    } else if (!argv[1]) {
        Context.dstrBBVer.pwszString = NULL;
        Context.dstrBBVer.cchString =  0;
    } else if(!strcmp(argv[1],"invalid")) {
        ASSIGN_WSTR_TO_DRMSTRING(Context.dstrBBVer, g_wcszInvalidBB_VERSION_STRING);
    }

    if(iOptions!=6) {         /* 6.  dstrDRMVer; */
        Context.dstrDRMVer = g_dstrReqTagPlayReadyClientVersionData;
    } else if (!argv[1]) {
        Context.dstrDRMVer.pwszString = NULL;
        Context.dstrDRMVer.cchString = 0;
    } else if (!strcmp(argv[1],"invalid")) {
        ASSIGN_WSTR_TO_DRMSTRING(Context.dstrDRMVer, g_wcszInvalidDRM_VERSION_STRING);
    }

    /* We have a license.  Check the revocation list and app cert */
    if(DRM_APPREVOCATION_IsAppRevocationSupported() && iOptions!=9)     /* 9. pcontextSSTRevocation=NULL would crash */
    {
        ChkDR(DRM_RVK_PerformAppRevocationCheck(&Context.pcontextBBX->CryptoContext, 
                                        Context.pContextRev,
                                       &Context.dstrContentLicense,
                                       &Context.appcert,
                                        Context.pbRevocationBuffer,
                                        Context.cbRevocationBuffer,
                                       &Context.idCRLsCurrent ));
    }

    /* Get the LID for the license */
    ChkDR( DRM_WMDRM_LIC_GetAttribute( &(Context.dstrContentLicense), NULL, DRM_LICENSE_ATTRIB_LID, NULL, &wszKID, 0 ) );

    if(iOptions==12)    /* 12. rgbLID; */
    {
        if (!argv[1])
        {
            wszKID.pwszString = NULL;
            wszKID.cchString = 0;
        }
        else if(!strcmp(argv[1],"invalid"))
        {
            wszKID.pwszString = (const DRM_WCHAR *)InvalidrgbLID;
            wszKID.cchString = (DRM_DWORD)DRMCRT_wcslen((const DRM_WCHAR *)InvalidrgbLID );
        }
    }

    ChkDR( DRM_UTL_StringToGuid( &wszKID, (DRM_GUID*) Context.LID.rgb ) );

    if(iOptions!=7) {      /* 7. pcontextSSTLicense */
        ChkMem( Context.pcontextSSTLicense = (DRM_SECSTORE_CONTEXT*) Oem_MemAlloc( sizeof(DRM_SECSTORE_CONTEXT)  ) );
        ChkDR( DRM_SST_OpenKeyTokens (
            Context.pcontextSSTLicense, 
            &(Context.LID),
            NULL,
            szSecStorePassword, 
            DRM_SECURE_STORE_OPEN_ALWAYS,
            SECURE_STORE_LICENSE_DATA, 
            g_pDatastore));
    }

    /*
    ** Force expected values for the expression parser
    */
    Context.certinfoSDK.appSec = DRM_TST_EXPR_EVAL_VALID_SECLEVEL;
    Context.certinfoSDK.appcd_subject = DRM_TST_EXPR_EVAL_VALID_SUBJID;
ErrorExit:
    return dr;    /* DRM_SUCCESS; */
}

DRM_RESULT DRM_CALL TestDRMEvaluateExpression(long argc, __in_ecount(argc) char **argv)
{
    TOKEN tResult;
    DRM_CONST_STRING MyExpression;

    MyExpression.pwszString = NULL;
    
    ChkArg(argc > 0);

    Context.contextEXPR.GetVariable = DRM_WMDRM_EXPR_GlobalGetVariable;
    Context.contextEXPR.SetVariable = DRM_WMDRM_EXPR_GlobalSetVariable;
    Context.contextEXPR.pvOpaqueData = &Context;

    ChkDR( MakeDRMString(&MyExpression, argv[0]) );
    ChkDR(DRM_WMDRM_EXPR_EvaluateExpression( &MyExpression, 
                                             &Context.contextEXPR, 
                                             &tResult ) );

    if (argc > 1 && argv[1]) {
        switch(tResult.TokenType)
        {
        case TOKEN_LONG:
            if (tResult.val.lValue != atol(argv[1])) {
                Log("Trace", "Returns long value %d, Expecting %d", tResult.val.lValue, atol(argv[1]));
                dr = DRM_S_FALSE;
            }
        
            break;
        case TOKEN_STRING:
            if (CompDRMString(&(tResult.val.stringValue), argv[1])) {
                Log("Trace", "Returns string value: %S, Expecting: %s", tResult.val.stringValue, argv[1]);
                dr = DRM_S_FALSE;
            }
            
            break;
        case TOKEN_DATETIME:
            break;
        default:
            Log("Trace",  "Unexpected variant returned from evaluation!" );
            dr = DRM_S_FALSE;
            break;
        }
    }
ErrorExit:
    FREEDRMSTRING( MyExpression );
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(ExprEval)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr;
    DRM_CONST_STRING wszHDSPath;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};
    
    RemoveDRMFile(RMFILE_STORE);
    ChkDR(SetDeviceEnv(DEVCERT_TEMPLATE_FILE_NAME, 
                       PRIVATE_KEY_FILE_NAME, 
                       GROUP_CERT_FILE_NAME,
                       GC_PRIVATE_KEY_FILE_NAME,
                       TRUE));
    ChkDR( tGetDeviceStorePathname(wszPathName, &wszHDSPath) );
    ChkDR(OpenHDS(&g_pDatastore, wszHDSPath.pwszString, FALSE));
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(ExprEval)(long lTCID, const char *strTCName)
{
    DRM_LST_Close(&oLicStoreContext);
    DRM_SST_CloseKey(Context.pcontextSSTGlobal, g_pDatastore, NULL, NULL );
    DRM_SST_CloseKey(Context.pcontextSSTLicense, g_pDatastore, NULL, NULL );

    SAFE_OEM_FREE(Context.pcontextSSTGlobal);
    SAFE_OEM_FREE(Context.pcontextSSTLicense);
    SAFE_OEM_FREE(Context.pcontextSSTRevocation );
    SAFE_OEM_FREE(Context.pContextRev);
    memset( &Context, 0, sizeof( DRM_LICEVAL_CONTEXT ) );

    if( oBBContext.pKeyFileContext != NULL )
    {
        DRM_KF_Uninitialize(oBBContext.pKeyFileContext);
        SAFE_OEM_FREE(oBBContext.pKeyFileContext);
    }


    memset( &oBBContext, 0, sizeof( DRM_BB_CONTEXT ) );

    CloseHDS(g_pDatastore);
    g_pDatastore = NULL;
    RemoveDRMFile(RMFILE_STORE);

    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST(ExprEval)
    API_ENTRY(TestLicEvalContextSetting2)
    API_ENTRY(TestDRMEvaluateExpression)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

