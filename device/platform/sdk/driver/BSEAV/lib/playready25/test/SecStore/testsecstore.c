/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <SecStoreTestAPI.h>

#include <tclib.h>
#include <tstutils.h>
#include <tstHDSutils.h>
#include <tstXMLutils.h>
#include <drmutilities.h>
#include <oempkcrypto.h>
#include <drmcontextsizes.h>
#include <drmsecurestore.h>
#include <tOEMIMP.h>
#include <tOEMIMP_PD.h>
#include <drmwmdrm.h>
#include <drmmathsafe.h>

ENTER_PKTEST_NAMESPACE_CODE;

#define MAX_CONTEXT_NUM    5
/*global variables*/
static DRM_SECSTORE_CONTEXT g_oContext[MAX_CONTEXT_NUM];
static DRM_DST *g_pDst = NULL;
static DRM_SECSTOREENUM_CONTEXT g_oSecEnumContext;
static DRM_BYTE g_szSecStorePassword[DRM_SHA1_DIGEST_LEN]={0};

static DRM_WORD g_wRightID = 0;
static DRM_WORD g_wRestrictionID = 0;

DRMINLINE DRM_SIZE_T localmin( DRM_SIZE_T a, DRM_SIZE_T b ) { return min(a,b); }

DRM_RESULT DRM_CALL TestDRM_SST_Initialize(long iIndex, __in_opt char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    int i=0;

    if (argv!=NULL){
        iIndex=(int)atoi(argv[0]);
    }
    if(iIndex==0 ||iIndex==1){
        for(i=0; i<MAX_CONTEXT_NUM; i++){
            memset(&g_oContext[i], 0, sizeof(g_oContext[i]));
        }
    }
    if(iIndex==0 ||iIndex==2){
        memset(&g_oSecEnumContext, 0, sizeof(g_oSecEnumContext));
    }
    if(iIndex==20)
        RemoveDRMFile(RMFILE_STORE); /*delete store*/
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_GetData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr         = DRM_SUCCESS;
    DRM_ID                  Key1;      
    const DRM_ID           *pKey1      = NULL;
    DRM_ID                  Key2;      
    const DRM_ID           *pKey2      = NULL;
    DRM_BYTE               *pbPassword = NULL;
    eDRM_SECURE_STORE_TYPE  iSecType   = SECURE_STORE_LICENSE_DATA;
    int iContextNull=0, iDSTContextNull=0, iSizeNull=0;
    DRM_DWORD cbData=0;
    DRM_BYTE *pbData=NULL;

    /*at least 9 arguments.*/
    if(argc<9){
        Log("Trace", "\t\tFrom TestDRM_SST_GetData: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iContextNull=1;

    if(argv[1]!=NULL){
        memset(Key1.rgb, 0, DRM_ID_SIZE);
        memcpy(Key1.rgb, argv[1], localmin(DRMCRT_strlen(argv[1]) + 1, DRM_ID_SIZE));
        pKey1 = &Key1;
    }

    if(argv[2]!=NULL){
        memset(Key2.rgb, 0, DRM_ID_SIZE);
        memcpy(Key2.rgb, argv[2], localmin(DRMCRT_strlen(argv[2]) + 1, DRM_ID_SIZE));
        pKey2 = &Key2;
    }

    if(argv[3]!=NULL){
        pbPassword=(DRM_BYTE*)Oem_MemAlloc( ( DRM_DWORD ) DRMCRT_strlen(argv[3])+1);
        if(!pbPassword)
            ChkDR(DRM_E_FAIL);
        memcpy(pbPassword, argv[3],DRMCRT_strlen(argv[3])+1);
    }

    /*secure type*/
    iSecType=(eDRM_SECURE_STORE_TYPE)atol(argv[4]);

    if(argv[5]==NULL)
        iDSTContextNull=1;

    if(argv[6]!=NULL){
        cbData=( DRM_DWORD ) DRMCRT_strlen(argv[6])+1;
        ChkMem(pbData=(DRM_BYTE*)Oem_MemAlloc(cbData));
        memcpy(pbData, argv[6], cbData);
    }
    if(argv[7]==NULL)
        iSizeNull=1;

    dr=DRM_SST_GetData(iContextNull>0?NULL:&g_oContext[0], pKey1,pKey2, pbPassword, iSecType, iDSTContextNull>0?NULL:g_pDst,pbData, iSizeNull>0?NULL:&cbData);
    if (dr==DRM_E_BUFFERTOOSMALL){
        SAFE_OEM_FREE(pbData);
        ChkMem(pbData = (DRM_BYTE *)Oem_MemAlloc(cbData + 1));
    }
    else{
        ChkDR(dr);
    }

    ChkDR(DRM_SST_GetData(iContextNull>0?NULL:&g_oContext[0], pKey1,pKey2, pbPassword, iSecType, iDSTContextNull>0?NULL:g_pDst,pbData, &cbData));

    /*verify the data*/
    if(argv[8]!=NULL){
        if(DRMCRT_strncmp((const DRM_CHAR*)pbData,argv[8], DRMCRT_strlen(argv[8]))){
            ChkDR(DRM_E_FAIL);
        }
    }
ErrorExit:
    SAFE_OEM_FREE(pbPassword);
    SAFE_OEM_FREE(pbData);
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_SetData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr         = DRM_SUCCESS;
    DRM_ID                  Key1;      
    const DRM_ID           *pKey1      = NULL;
    DRM_ID                  Key2;      
    const DRM_ID           *pKey2      = NULL;
    DRM_BYTE               *pbPassword = NULL;
    eDRM_SECURE_STORE_TYPE  iSecType   = SECURE_STORE_LICENSE_DATA;
    int iContextNull=0, iDSTContextNull=0;
    DRM_DWORD cbData=0;
    DRM_BYTE *pbData=NULL;

    /*at least 7 arguments.*/
    if(argc<7){
        Log("Trace", "\t\tFrom TestDRM_SST_SetData: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iContextNull=1;

    if(argv[1]!=NULL){
        memset(Key1.rgb, 0, DRM_ID_SIZE);
        memcpy(Key1.rgb, argv[1], localmin(DRMCRT_strlen(argv[1]) + 1, DRM_ID_SIZE));
        pKey1 = &Key1;
    }

    if(argv[2]!=NULL){
        memset(Key2.rgb, 0, DRM_ID_SIZE);
        memcpy(Key2.rgb, argv[2], localmin(DRMCRT_strlen(argv[2]) + 1, DRM_ID_SIZE));
        pKey2 = &Key2;
    }

    if(argv[3]!=NULL){
        ChkArg( DRMCRT_strlen(argv[3]) >= DRM_SHA1_DIGEST_LEN );

        pbPassword=(DRM_BYTE*)Oem_MemAlloc((DRM_DWORD)DRMCRT_strlen(argv[3])+1);
        if(!pbPassword)
            ChkDR(DRM_E_FAIL);
        memcpy(pbPassword, argv[3],DRMCRT_strlen(argv[3])+1);
    }

    /*secure type*/
    iSecType=(eDRM_SECURE_STORE_TYPE)atoi(argv[4]);

    if(argv[5]==NULL)
        iDSTContextNull=1;

    if(argv[6]!=NULL){
        cbData= ( DRM_DWORD ) DRMCRT_strlen(argv[6])+1;
        ChkMem(pbData=(DRM_BYTE*)Oem_MemAlloc(cbData));
        memcpy(pbData, argv[6], cbData);
    }

    ChkDR(DRM_SST_SetData(iContextNull>0?NULL:&g_oContext[0], pKey1,pKey2, pbPassword, iSecType, iDSTContextNull>0?NULL:g_pDst,pbData, cbData));

ErrorExit:
    SAFE_OEM_FREE(pbPassword);
    SAFE_OEM_FREE(pbData);
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_OpenKeyTokens(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr         = DRM_SUCCESS;
    DRM_ID                  Key1;      
    const DRM_ID           *pKey1      = NULL;
    DRM_ID                  Key2;      
    const DRM_ID           *pKey2      = NULL;
    DRM_BYTE               *pbPassword = NULL;
    eDRM_SECURE_STORE_TYPE  iSecType   = SECURE_STORE_LICENSE_DATA;
    DRM_DWORD dwFlags=0;
    int iContextNull=0, iDSTContextNull=0, iContextNum=0;

    /*at least 7 arguments.*/
    if(argc<7){
        Log("Trace", "\t\tFrom TestDRM_SST_OpenKeyTokens: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iContextNull=1;
    else
        iContextNum=atoi(argv[0]);

    if(argv[1]!=NULL){
        /*16 bytes*/
        memset(Key1.rgb, 0, DRM_ID_SIZE);
        memcpy(Key1.rgb, argv[1], localmin(DRMCRT_strlen(argv[1]) + 1, DRM_ID_SIZE));
        pKey1 = &Key1;
    }

    if(argv[2]!=NULL){
        /*16 bytes*/
        memset(Key2.rgb, 0, DRM_ID_SIZE);
        memcpy(Key2.rgb, argv[2], localmin(DRMCRT_strlen(argv[2]) + 1, DRM_ID_SIZE));
        pKey2 = &Key2;
    }

    if(argv[3]!=NULL){
        /*20 bites*/
        pbPassword=(DRM_BYTE*)Oem_MemAlloc(DRM_SHA1_DIGEST_LEN);
        if(!pbPassword)
            ChkDR(DRM_E_FAIL);
        if(DRMCRT_strncmp(argv[3],"Password", DRMCRT_strlen(argv[3]))){
            memcpy(pbPassword, argv[3],localmin(DRMCRT_strlen(argv[3]) + 1, DRM_SHA1_DIGEST_LEN));
        }
        else{
            memcpy(pbPassword, g_szSecStorePassword,DRM_SHA1_DIGEST_LEN);
        }
    }

    /*secure flag*/
    dwFlags=(DRM_DWORD)atol(argv[4]);

    /*secure type*/
    iSecType=(eDRM_SECURE_STORE_TYPE)atoi(argv[5]);

    if(argv[6]==NULL)
        iDSTContextNull=1;

    ChkDR(DRM_SST_OpenKeyTokens(iContextNull>0?NULL:&g_oContext[iContextNum], pKey1,pKey2, pbPassword, dwFlags, iSecType, iDSTContextNull>0?NULL:g_pDst));

ErrorExit:
    if(pbPassword)
        SAFE_OEM_FREE(pbPassword);
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_GetTokenValue(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    TOKEN myToken;
    DRM_WCHAR pwszAttr[256]={0};
    DRM_CONST_STRING pwszAttribute;
    int iContextNull=0, iContextNum=0;

    /*At least 4 arguments
        argv[0]: g_oContext;
        argv[1]: attribute name;
        argv[2]: token type;
        argv[3]: token value to be verified
    */
    if(argc<4){
        Log("Trace", "\t\tFrom TestDRM_SST_GetTokenValue: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iContextNull=1;
    else
        iContextNum=atoi(argv[0]);

    /*attribute name*/
    if(argv[1]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( pwszAttr ) );
        TST_UTL_mbstowcs(pwszAttr, argv[1], cch+1);
        pwszAttribute.cchString= ( DRM_DWORD ) DRMCRT_wcslen(pwszAttr)+1;
        pwszAttribute.pwszString=pwszAttr;
    }
    else{
        pwszAttribute.pwszString=NULL;
        pwszAttribute.cchString=0;
    }

    ChkDR(DRM_SST_GetTokenValue(iContextNull>0?NULL:&g_oContext[iContextNum], &pwszAttribute, &myToken));

     /*verify the results*/
    if(argv[2]!=NULL && argv[3]!=NULL){
        if(TOKEN_LONG==(DRM_EXPR_TOKEN_TYPE)atoi(argv[2])){
            if(myToken.TokenType!=TOKEN_LONG || myToken.val.lValue!=(DRM_LONG)atol(argv[3])){
                TRACE(("GetToken returned %d, but was expecting %d.", myToken.val.lValue, atol(argv[3]) ));
                ChkDR(DRM_E_FAIL);
            }
        }
        else if(TOKEN_FUNCTION==(DRM_EXPR_TOKEN_TYPE)atoi(argv[2])){
            if(myToken.TokenType!=TOKEN_FUNCTION || myToken.val.fnValue!=(DRM_DWORD)atoi(argv[3])){
                ChkDR(DRM_E_FAIL);
            }
        }
        else if(TOKEN_DATETIME==(DRM_EXPR_TOKEN_TYPE)atoi(argv[2])){
            if(myToken.TokenType!=TOKEN_DATETIME || !DRM_UI64Eql(myToken.val.u64DateTime, DRM_UI64(atol(argv[3])))){
                TRACE(("GetToken returned %d, but was expecting %d.", DRM_UI64Low32(myToken.val.u64DateTime), atol(argv[3]) ));
                ChkDR(DRM_E_FAIL);
            }
        }
        else if(TOKEN_STRING==(DRM_EXPR_TOKEN_TYPE)atoi(argv[2])){
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[3] ), &cch ) );
            memset(pwszAttr,0,sizeof(pwszAttr));
            ChkArg( cch + 1 <= NO_OF( pwszAttr ) );
            TST_UTL_mbstowcs(pwszAttr, argv[3],cch+1 );
            if(myToken.TokenType!=TOKEN_STRING || DRMCRT_wcsncmp(myToken.val.stringValue.pwszString,pwszAttr,myToken.val.stringValue.cchString)){
                ChkDR(DRM_E_FAIL);
            }
        }
        else if(TOKEN_BYTEBLOB==(DRM_EXPR_TOKEN_TYPE)atoi(argv[2])){
            if(myToken.TokenType!=TOKEN_BYTEBLOB || DRMCRT_strncmp((const DRM_CHAR*)myToken.val.byteValue.pbBlob,argv[3],DRMCRT_strlen(argv[3]))){
                ChkDR(DRM_E_FAIL);
            }
        }
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_SetTokenValue(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    TOKEN myToken;
    DRM_WCHAR pwszAttr[256]={0}, pwszString[256]={0};
    DRM_CONST_STRING pwszAttribute;
    int iContextNull=0, iContextNum=0;
    DRM_DWORD dwFlags = 0;

    /*At least 4 arguments
        argv[0]: g_oContext;
        argv[1]: attribute name;
        argv[2]: token type;
        argv[3]: taken value to be set
        argv[4]: secure store conflict resolution flags
        argv[5]: right ID
        argv[6]: restriction ID
    */
    if(argc<4){
        Log("Trace", "\t\tFrom TestDRM_SST_GetTokenValue: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iContextNull=1;
    else
        iContextNum=atoi(argv[0]);

    /*attribute name*/
    if(argv[1]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( pwszAttr ) );
        TST_UTL_mbstowcs(pwszAttr, argv[1], cch+1);
        pwszAttribute.cchString= ( DRM_DWORD ) DRMCRT_wcslen(pwszAttr)+1;
        pwszAttribute.pwszString=pwszAttr;
    }
    else{
        pwszAttribute.pwszString=NULL;
        pwszAttribute.cchString=0;
    }

    /*set token type and values*/
    myToken.TokenType=(DRM_EXPR_TOKEN_TYPE)atoi(argv[2]);
    if(myToken.TokenType==TOKEN_LONG){
        myToken.val.lValue=(DRM_LONG)atol(argv[3]);
    }
    else if(myToken.TokenType==TOKEN_FUNCTION){
        myToken.val.fnValue=(DRM_EXPR_FUNCTION_TYPE)atoi(argv[3]);
    }
    else if(myToken.TokenType==TOKEN_DATETIME){
        myToken.val.u64DateTime=DRM_UI64(atol(argv[3]));
    }
    else if(myToken.TokenType==TOKEN_STRING){
        if(argv[3]!=NULL){
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &cch ) );
            ChkArg( cch + 1 <= NO_OF( pwszString ) );
            TST_UTL_mbstowcs(pwszString, argv[3], cch+1);
            myToken.val.stringValue.cchString= ( DRM_DWORD )  DRMCRT_wcslen(pwszString)+1;
            myToken.val.stringValue.pwszString=pwszString;
        }
        else{
            myToken.val.stringValue.pwszString=NULL;
            myToken.val.stringValue.cchString=0;
        }
    }
    else if(myToken.TokenType==TOKEN_BYTEBLOB){
        if(argv[3]!=NULL)
        {
            myToken.val.byteValue.cbBlob = ( DRM_DWORD )DRMCRT_strlen(argv[3])+1;
            myToken.val.byteValue.pbBlob = (DRM_BYTE*)argv[3];
        }
        else
        {
            myToken.val.byteValue.pbBlob = NULL;
            myToken.val.byteValue.cbBlob = 0;
        }
    }

    if(argc > 4)
    {
        if( argv[4] != NULL )
        {
            dwFlags = (DRM_DWORD)atoi( argv[4] );
        }
        ChkDR(DRM_SST_SetExplicitResolutionTokenValue(iContextNull>0?NULL:&g_oContext[iContextNum], &pwszAttribute, &myToken, dwFlags));
    }
    else
    {
        ChkDR(DRM_SST_SetTokenValue(iContextNull>0?NULL:&g_oContext[iContextNum], &pwszAttribute, &myToken));
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_GetAllData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_BYTE *pbData=NULL;
    DRM_DWORD cbData=0;
    int iContextNull=0, iSizeNull=0, iContextNum=0;


    /*at least 4 arguments.*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestDRM_SST_GetAllData: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iContextNull=1;
    else
        iContextNum=atoi(argv[0]);

    if(argv[1]!=NULL){
        cbData= ( DRM_DWORD ) DRMCRT_strlen(argv[2])+1;
        ChkMem(pbData=(DRM_BYTE*)Oem_MemAlloc(cbData));
        memcpy(pbData, argv[2], cbData);
    }
    if(argv[3]==NULL)
        iSizeNull=1;

    dr=DRM_SST_GetAllData(iContextNull>0?NULL:&g_oContext[iContextNum], pbData, iSizeNull>0?NULL:&cbData);
    if (dr==DRM_E_BUFFERTOOSMALL){
        SAFE_OEM_FREE(pbData);
        ChkMem(pbData = (DRM_BYTE *)Oem_MemAlloc(cbData + 1));
    }
    else{
        ChkDR(dr);
    }
    ChkDR(DRM_SST_GetAllData(iContextNull>0?NULL:&g_oContext[iContextNum], pbData, &cbData));

ErrorExit:
    SAFE_OEM_FREE(pbData);
    return dr;
}

DRM_RESULT DRM_CALL PolicyCallback(
    __in const DRM_VOID                 *f_pvPolicyData,
    __in       DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
    __in const DRM_VOID                 *f_pv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SECURE_STATE_TOKEN_RESOLVE_DATA *pData = (DRM_SECURE_STATE_TOKEN_RESOLVE_DATA *)f_pvPolicyData;
    DRM_DWORD i = 0;


    ChkBOOL( f_dwCallbackType == DRM_SECURE_STATE_TOKEN_RESOLVE_CALLBACK, DRM_E_NOTIMPL );

    ChkArg( pData->pOriginallyIntendedValue->TokenType == pData->pOnDisk->TokenType );

    ChkBOOL( pData->wRightID == g_wRightID, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pData->wRestrictionID == g_wRestrictionID, DRM_E_TEST_UNEXPECTED_OUTPUT );

    switch( pData->pOriginallyIntendedValue->TokenType )
    {
        case TOKEN_LONG:
            /* Merge by multiplying the original by 1000 and adding the new value */
            pData->pOnDisk->val.lValue = (pData->pOnDisk->val.lValue * 1000) + pData->pOriginallyIntendedValue->val.lValue;
            break;

        case TOKEN_DATETIME:
            /* Merge by multiplying the original by 1000 and adding the new value */
            pData->pOnDisk->val.u64DateTime =  DRM_UI64Add( DRM_UI64Mul(pData->pOnDisk->val.u64DateTime, DRM_UI64( 1000 ) ), pData->pOriginallyIntendedValue->val.u64DateTime );
            break;

        case TOKEN_BYTEBLOB:
            ChkArg( pData->pOriginallyIntendedValue->val.byteValue.cbBlob == pData->pOnDisk->val.byteValue.cbBlob );

            /* Merge them by interleaving (ie abc + def = aec) */
            for( i = 0; i < pData->pOriginallyIntendedValue->val.byteValue.cbBlob; i++ )
            {
                PUT_BYTE( pData->pOnDisk->val.byteValue.pbBlob, i, (i % 2 == 0) ? GET_BYTE( pData->pOnDisk->val.byteValue.pbBlob, i ) : GET_BYTE( pData->pOriginallyIntendedValue->val.byteValue.pbBlob, i ) );
            }
            break;

        default:
            ChkArg( FALSE );
    }


ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_CloseKey(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    int iContextNull=0, iDSTContextNull=0, iContextNum=0;
    DRM_BOOL fUseCallback = FALSE;

    /*at least 2 arguments.*/
    if(argc<2){
        Log("Trace", "\t\tFrom TestDRM_SST_CloseKey: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iContextNull=1;
    else
        iContextNum=atoi(argv[0]);

    if(argv[1]==NULL)
        iDSTContextNull=1;

    if( argc > 2
     && argv[2] != NULL )
    {
        fUseCallback = TRUE;
    }

    ChkDR(DRM_SST_CloseKey(iContextNull>0?NULL:&g_oContext[iContextNum], iDSTContextNull>0?NULL:g_pDst, fUseCallback ? PolicyCallback : NULL, NULL));

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_DeleteKey(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr         = DRM_SUCCESS;
    DRM_ID                  Key1;      
    const DRM_ID           *pKey1      = NULL;
    DRM_ID                  Key2;      
    const DRM_ID           *pKey2      = NULL;
    eDRM_SECURE_STORE_TYPE  iSecType   = SECURE_STORE_LICENSE_DATA;
    int iContextNull=0, iDSTContextNull=0, iContextNum=0;

    /*at least 5 arguments.*/
    if(argc<5){
        Log("Trace", "\t\tFrom TestDRM_SST_DeleteKey: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iContextNull=1;
    else
        iContextNum=atoi(argv[0]);

    /*secure type*/
    iSecType=(eDRM_SECURE_STORE_TYPE)atoi(argv[1]);

    if(argv[2]!=NULL){
        memset(Key1.rgb, 0, DRM_ID_SIZE);
        memcpy(Key1.rgb, argv[2], localmin(DRMCRT_strlen(argv[2]) + 1, DRM_ID_SIZE));
        pKey1 = &Key1;
    }

    if(argv[3]!=NULL){
        memset(Key2.rgb, 0, DRM_ID_SIZE);
        memcpy(Key2.rgb, argv[3], localmin(DRMCRT_strlen(argv[3]) + 1, DRM_ID_SIZE));
        pKey2 = &Key2;
    }

    if(argv[4]==NULL)
        iDSTContextNull=1;

    ChkDR(DRM_SST_DeleteKey(iContextNull>0?NULL:&g_oContext[iContextNum],iSecType,pKey1,pKey2, iDSTContextNull>0?NULL:g_pDst));

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_OpenEnumerator(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr         = DRM_SUCCESS;
    DRM_ID                  Key1;      
    const DRM_ID           *pKey1      = NULL;
    eDRM_SECURE_STORE_TYPE  iSecType   = SECURE_STORE_LICENSE_DATA;
    int iEnumContextNull=0, iDSTContextNull=0;

    /*at least 4 arguments.*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestDRM_SST_OpenEnumerator: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    /*secure type*/
    iSecType=(eDRM_SECURE_STORE_TYPE)atoi(argv[0]);

    if(argv[1]!=NULL){
        memset(Key1.rgb, 0, DRM_ID_SIZE);
        memcpy(Key1.rgb, argv[1], localmin(DRMCRT_strlen(argv[1]) + 1, DRM_ID_SIZE));
        pKey1 = &Key1;
    }

    if(argv[2]==NULL)
        iEnumContextNull=1;

    if(argv[3]==NULL)
        iDSTContextNull=1;

    ChkDR(DRM_SST_OpenEnumerator(iSecType,pKey1,iEnumContextNull>0?NULL:&g_oSecEnumContext,iDSTContextNull>0?NULL:g_pDst, FALSE));

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_EnumNext(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_ID Key1, *pKey1=NULL;
    DRM_DWORD cbNum=0;
    int iEnumContextNull=0,iSizeNull=0;

    /*at least 4 arguments.*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestDRM_SST_EnumNext: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iEnumContextNull=1;

    if(argv[1]!=NULL){
        memset(Key1.rgb, 0, DRM_ID_SIZE);
        memcpy(Key1.rgb, argv[1], localmin(DRMCRT_strlen(argv[1]) + 1, DRM_ID_SIZE));
        pKey1 = &Key1;
    }

    if(argv[2]==NULL){
        iSizeNull=1;
    }
    else
        cbNum = (DRM_DWORD)atol(argv[2]);

    ChkDR(DRM_SST_EnumNext(iEnumContextNull>0?NULL:&g_oSecEnumContext,NULL,pKey1, iSizeNull>0?NULL:&cbNum));

    /*verify key value*/
    if(argv[3]!=NULL){
        AssertChkArg( pKey1 != NULL );
        if(DRMCRT_strncmp((const DRM_CHAR *)pKey1->rgb,argv[3], localmin(DRMCRT_strlen(argv[3]) + 1, DRM_ID_SIZE))){
            ChkDR(DRM_E_FAIL);
        }
    }

ErrorExit:
    return dr;

}
DRM_RESULT _iTestGetDevCertValues( DRM_BB_CONTEXT *pcontextBBX )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_SetWMDRMCertCachedValues( pcontextBBX ) );
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_CreateGlobalStorePassword(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_BB_CONTEXT *poBBContext = NULL;

    ChkMem( poBBContext = ( DRM_BB_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_BB_CONTEXT ) ) );
    ZEROMEM( poBBContext, SIZEOF( DRM_BB_CONTEXT ) );

    ChkMem( poBBContext->pKeyFileContext = ( DRM_KEYFILE_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_KEYFILE_CONTEXT ) ) );
    ChkDR( DRM_KF_Initialize( NULL, poBBContext->pKeyFileContext ) );
    ChkDR( DRM_KF_Open( poBBContext->pKeyFileContext, TRUE, OEM_GENERIC_READ ) );
    ChkDR( DRM_KF_LoadFromFile( poBBContext->pKeyFileContext, TRUE ) );
    ChkDR( DRM_KF_Close( poBBContext->pKeyFileContext ) );

    ChkDR( DRM_BBX_Initialize( NULL, poBBContext ) );
    ChkDR(_iTestGetDevCertValues(poBBContext));
    ChkDR( DRM_BBX_TestDeviceKeyPair( poBBContext ) );

    ChkDR(DRM_SST_CreateGlobalStorePassword(NULL, g_szSecStorePassword, (DRM_BYTE*)poBBContext));

ErrorExit:
    if( poBBContext != NULL )
    {
        if( poBBContext->pKeyFileContext != NULL )
        {
            DRM_KF_Uninitialize(poBBContext->pKeyFileContext);
            SAFE_OEM_FREE( poBBContext->pKeyFileContext );
        }

        DRM_BBX_Shutdown(poBBContext);
    }
    SAFE_OEM_FREE( poBBContext );
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_CreateLicenseStatePassword(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_BB_CONTEXT *poBBContext = NULL;
    DRM_LID localLID;

    ChkMem( poBBContext = ( DRM_BB_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_BB_CONTEXT ) ) );
    ZEROMEM( poBBContext, SIZEOF( DRM_BB_CONTEXT ) );

    ChkMem( poBBContext->pKeyFileContext = ( DRM_KEYFILE_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_KEYFILE_CONTEXT ) ) );
    ChkDR( DRM_KF_Initialize( NULL, poBBContext->pKeyFileContext ) );
    ChkDR( DRM_KF_Open( poBBContext->pKeyFileContext, TRUE, OEM_GENERIC_READ ) );
    ChkDR( DRM_KF_LoadFromFile( poBBContext->pKeyFileContext, TRUE ) );
    ChkDR( DRM_BBX_SetupSecureStoreKey( poBBContext, NULL ) );
    ChkDR( DRM_KF_Close( poBBContext->pKeyFileContext ) );


    ChkDR( DRM_BBX_Initialize( NULL, poBBContext ) );
    ChkDR(_iTestGetDevCertValues(poBBContext));
    ChkDR( DRM_BBX_TestDeviceKeyPair( poBBContext ) );

    if(argv[0]!=NULL){
        memcpy(localLID.rgb, argv[0], localmin(DRMCRT_strlen(argv[0])+1, DRM_ID_SIZE));
    }

    ChkDR(DRM_SST_CreateLicenseStatePassword(&localLID, g_szSecStorePassword, poBBContext));

ErrorExit:
    if( poBBContext != NULL )
    {
        if( poBBContext->pKeyFileContext != NULL )
        {
            DRM_KF_Uninitialize(poBBContext->pKeyFileContext);
            SAFE_OEM_FREE( poBBContext->pKeyFileContext );
        }

        DRM_BBX_Shutdown(poBBContext);
    }
    SAFE_OEM_FREE( poBBContext );
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_OpenAndLockSlot(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr         = DRM_SUCCESS;
    DRM_ID                  Key1;      
    const DRM_ID           *pKey1      = NULL;
    DRM_ID                  Key2;      
    const DRM_ID           *pKey2      = NULL;
    DRM_BYTE               *pbPassword = NULL;
    eDRM_SECURE_STORE_TYPE  iSecType   = SECURE_STORE_LICENSE_DATA;
    DRM_DWORD dwFlags=0;
    int iContextNull=0, iDSTContextNull=0, iContextNum=0;

    /*at least 7 arguments.*/
    if(argc<7){
        Log("Trace", "\t\tFrom TestDRM_SST_OpenAndLockSlot: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }

    if(argv[0]==NULL)
        iDSTContextNull=1;

    /*secure type*/
    iSecType=(eDRM_SECURE_STORE_TYPE)atoi(argv[1]);

    if(argv[2]!=NULL){
        /*16 bytes*/
        memset(Key1.rgb, 0, DRM_ID_SIZE);
        memcpy(Key1.rgb, argv[2], localmin(DRMCRT_strlen(argv[2]) + 1, DRM_ID_SIZE));
        pKey1 = &Key1;
    }

    if(argv[3]!=NULL){
        /*16 bytes*/
        memset(Key2.rgb, 0, DRM_ID_SIZE);
        memcpy(Key2.rgb, argv[3], localmin(DRMCRT_strlen(argv[3]) + 1, DRM_ID_SIZE));
        pKey2 = &Key2;
    }

    if(argv[4]!=NULL){
        /*20 bites*/
        pbPassword=(DRM_BYTE*)Oem_MemAlloc(DRM_SHA1_DIGEST_LEN);
        if(!pbPassword)
            ChkDR(DRM_E_FAIL);
        if(DRMCRT_strncmp(argv[4],"Password", DRMCRT_strlen(argv[4]))){
            memcpy(pbPassword, argv[4],localmin(DRMCRT_strlen(argv[4]) + 1, DRM_SHA1_DIGEST_LEN));
        }
        else{
            memcpy(pbPassword, g_szSecStorePassword,DRM_SHA1_DIGEST_LEN);
        }
    }

    /*secure flag*/
    dwFlags=(DRM_DWORD)atol(argv[5]);

    if(argv[6]==NULL)
        iContextNull=1;
    else
        iContextNum=atoi(argv[6]);


    ChkDR(DRM_SST_OpenAndLockSlot(iDSTContextNull>0?NULL:g_pDst, iSecType, pKey1, pKey2, pbPassword, dwFlags, iContextNull>0?NULL:&g_oContext[iContextNum], NULL));

ErrorExit:
    if(pbPassword)
        SAFE_OEM_FREE(pbPassword);

    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_SetLockedData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    int iContextNum=0;
    DRM_DWORD cbData=0;
    DRM_BYTE *pbData=NULL;

    ChkArg( argc    >  0 );
    ChkArg( argv[0] != NULL );
    iContextNum=atoi(argv[0]);
    if(argc > 1 && argv[1]!=NULL){
        cbData= ( DRM_DWORD ) DRMCRT_strlen(argv[1])+1;
        ChkMem(pbData=(DRM_BYTE*)Oem_MemAlloc(cbData));
        memcpy(pbData, argv[1], cbData);
    }

    ChkDR(DRM_SST_SetLockedData(&g_oContext[iContextNum],cbData, pbData));

ErrorExit:
    SAFE_OEM_FREE(pbData);
    return dr;
}

DRM_RESULT DRM_CALL TestDRM_SST_GetLockedData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    int iContextNum=0;
    DRM_DWORD cbData=0;
    DRM_BYTE *pbData=NULL;

    ChkArg( argc > 0 );
    iContextNum=atoi(argv[0]);
    dr=DRM_SST_GetLockedData(&g_oContext[iContextNum],pbData, &cbData);
    if (dr==DRM_E_BUFFERTOOSMALL){
        ChkMem(pbData = (DRM_BYTE *)Oem_MemAlloc(cbData + 1));
    }
    else{
        ChkDR(dr);
    }
    ChkDR(DRM_SST_GetLockedData(&g_oContext[iContextNum], pbData, &cbData));

    /*verify the data*/
    if ( argc > 1 && argv[1] != NULL )
    {
        ChkBOOL( 0 == memcmp( (char*)pbData,argv[1], localmin( DRMCRT_strlen(argv[1]), cbData ) ), DRM_E_FAIL );
    }
ErrorExit:
    SAFE_OEM_FREE(pbData);
    return dr;
}


DRM_RESULT DRM_CALL TestDRM_SST_CloseLockedSlot(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    int iContextNum=0;

    ChkArg( argc    >  0 );
    ChkArg( argv[0] != NULL );
    iContextNum=atoi(argv[0]);
    ChkDR(DRM_SST_CloseLockedSlot(&g_oContext[iContextNum]));

ErrorExit:
    return dr;
}


DRM_RESULT DRM_CALL MODULE_PRETESTCASE(SecStore)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr;

    DRM_CONST_STRING dstrStoreName;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};
    RemoveDRMFile(RMFILE_STORE);
    ChkDR(SetDeviceEnv(DEVCERT_TEMPLATE_FILE_NAME,
                       PRIVATE_KEY_FILE_NAME,
                       GROUP_CERT_FILE_NAME,
                       GC_PRIVATE_KEY_FILE_NAME,
                       TRUE));
    ChkDR( tGetDeviceStorePathname(wszPathName, &dstrStoreName) );
    ChkDR(OpenHDS(&g_pDst, dstrStoreName.pwszString, FALSE));

    g_wRightID = 0;
    g_wRestrictionID = 0;

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(SecStore)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr;
    ChkDR(CloseHDS(g_pDst));
    g_pDst = NULL;
    RemoveDRMFile(RMFILE_STORE);
ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST(SecStore)
    API_ENTRY(TestDRM_SST_GetData)
    API_ENTRY(TestDRM_SST_SetData)
    API_ENTRY(TestDRM_SST_OpenEnumerator)
    API_ENTRY(TestDRM_SST_EnumNext)
    API_ENTRY(TestDRM_SST_OpenKeyTokens)
    API_ENTRY(TestDRM_SST_GetTokenValue)
    API_ENTRY(TestDRM_SST_SetTokenValue)
    API_ENTRY(TestDRM_SST_GetAllData)
    API_ENTRY(TestDRM_SST_CloseKey)
    API_ENTRY(TestDRM_SST_DeleteKey)
    API_ENTRY(TestDRM_SST_Initialize)
    API_ENTRY(TestDRM_SST_CreateGlobalStorePassword)
    API_ENTRY(TestDRM_SST_CreateLicenseStatePassword)
    API_ENTRY(TestDRM_SST_OpenAndLockSlot)
    API_ENTRY(TestDRM_SST_SetLockedData)
    API_ENTRY(TestDRM_SST_GetLockedData)
    API_ENTRY(TestDRM_SST_CloseLockedSlot)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
