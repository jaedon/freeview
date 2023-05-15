/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


/*Most of function has additional argument using as flags to
  decide which input should be overwritten:
    1st bit overwrite pbXmlContext string 
    2nd bit overwrite cbXmlContext size
    3rd bit overwrite pwszNodeName string   
    4th bit overwrite pwszNodeName size
    5th bit Attribute name/CData has leading space
    6th bit Attribute name/CData has traling space
    7th bit attribute value/Data has leading space 
    8th bit attribute value/Data has trailing space
 */

#include <stdafx.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>
#include <XMLBuilderTestAPI.h>

ENTER_PKTEST_NAMESPACE_CODE

static DRM_DWORD    cbXmlContext=0;
static _XMBContext *pbXmlContext=NULL;


/*
** This macro is used to reallocate buffer for XML context if existing is too small
** If XML bulder function returns DRM_E_BUFFERTOOSMALL - macro reallocates buffer and 
** calls the function again
*/
#define CallXmlBuilder(context, fn)   {\
    while (TRUE)\
    {\
        dr = (fn);\
        if ( dr == DRM_E_BUFFERTOOSMALL )\
        {\
            ChkDR(TestXMLB_DRMTOOLS_ReallocXmlContext(&(context)));\
            continue;\
        }\
        ChkDR(dr);\
        break;\
    }\
}

/******************************************************************************
** 
** Function :   _LogARGs
**
** Synopsis :   Logs input arguments, ensures it's number does not exceed specified maximum
** 
** Arguments :  [f_argc]            - number of arguments
**              [f_argv]            - array of arguments
**              [f_argcMax]         - max of arguments
**              [f_pszFunctionName] - name of calling function to logging purpose
** 
** Returns :    DRM_SUCCESS if new buffer was allocated successfully
** 
******************************************************************************/
static DRM_RESULT DRM_CALL _LogARGs(
    __in long                  f_argc, 
    __in_ecount(f_argc) char **f_argv,    
    __in long                  f_argcMax,
    __in char                 *f_pszFunctionName )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_LONG   i = 0;

    ChkArg( f_argv != NULL );
    ChkArg( f_pszFunctionName != NULL );
    for( i=0; i<f_argc; i++ )
    {
        Log( "Trace", "\t\tFrom %s: argv[%d]=%s", f_pszFunctionName, i, f_argv[i] );
    }

    if( f_argc < f_argcMax )
    {
        Log( "Trace", "\t\tFrom %s: error in number of arguments.", f_pszFunctionName );
        ChkDR( DRM_E_TEST_INVALIDARG );
    }

ErrorExit:
    return dr;
    
}

/******************************************************************************
** 
** Function :   TestXMLB_DRMTOOLS_ReallocXmlContext
**
** Synopsis :   Allocate double the size of the current 
**              wide character based XML context
** 
** Arguments :  [f_ppbXmlContext] - pointer to wide character based Xml content
** 
** Returns :    DRM_SUCCESS if new buffer was allocated successfully
** 
******************************************************************************/
DRM_RESULT DRM_CALL TestXMLB_DRMTOOLS_ReallocXmlContext(
    IN OUT _XMBContext    **f_ppbXmlContext)
{
    DRM_RESULT   dr              = DRM_SUCCESS;
    DRM_DWORD    cbContextXML    = 0;
    _XMBContext *pbContextXMLNew = NULL;

    /* get current size */
    ChkDR( DRM_XMB_GetContextSize( *f_ppbXmlContext, &cbContextXML ) ); 
    /*  This should never be 0 */
    ChkBOOL ( cbContextXML != 0, DRM_E_LOGICERR );
    
    /* alloc double of the current size */
    cbContextXML *= 2;
    ChkMem( pbContextXMLNew = (_XMBContext *) Oem_MemAlloc( cbContextXML ) );  

    ChkDR( DRM_XMB_ReallocDocument( *f_ppbXmlContext,    
                                    cbContextXML, 
                                    pbContextXMLNew ) );
    
    SAFE_OEM_FREE( *f_ppbXmlContext );

    *f_ppbXmlContext = pbContextXMLNew;

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        SAFE_OEM_FREE( pbContextXMLNew );
    }

    return dr;
}
 
/*  Reinitialize all if iIndex=0*/
DRM_RESULT DRM_CALL TestXMLBUtilReinitialize(long iIndex, __in_opt char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    if (argv!=NULL){
        iIndex=(int)atoi(argv[0]);
    }

    if(iIndex==0 ||iIndex==1){ 
        SAFE_OEM_FREE(pbXmlContext);
        pbXmlContext=NULL;
    }
    if(iIndex==0 ||iIndex==2){ 
        cbXmlContext=0; 
    }
   
    return dr;
}

DRM_RESULT DRM_CALL TestXMLBCreateDocument(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_CONST_STRING pwszRootNodeName = {0,0};
    int iRootNodeNameNullFlag=0;
    DRM_WCHAR pwszLocalString[512]={0};
 
    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBCreateDocument: argv[%d]=%s", i, argv[i]);
    }
    /*at least 5 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<5){
        Log("Trace", "\t\tFrom TestXMLBCreateDocument: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[0])||!IsDigitalOnlyString(argv[3])||!IsDigitalOnlyString(argv[4])){
        Log("Trace", "\t\tFrom TestXMLBCreateDocument: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
  
    iOption=(long)atol(argv[4]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        TestXMLBUtilReinitialize(1,NULL);
        if(argv[1]!=NULL){
            /*the 2nd bit is to overwrite cbXmlContext size*/ 
            if(iOption&2){
                cbXmlContext=(DRM_DWORD)atol(argv[0]);
                pbXmlContext=(_XMBContext*)Oem_MemAlloc(cbXmlContext*sizeof(DRM_BYTE)+1);
                if(!pbXmlContext)
                    ChkDR(DRM_E_FAIL);
            }
            else{
                cbXmlContext=(DRM_DWORD)strlen(argv[1]);
                pbXmlContext=(_XMBContext*)Oem_MemAlloc(cbXmlContext*sizeof(DRM_BYTE)+1);
                if(!pbXmlContext)
                    ChkDR(DRM_E_FAIL);
                memcpy(pbXmlContext, argv[1], cbXmlContext+1);
            }
        }
    }
    if(iOption&4){
        if(argv[2]!=NULL){
            if(strcmp(argv[2],"nullstring")){
                TST_UTL_mbstowcs(pwszLocalString, argv[2], strlen(argv[2])+1);
                pwszRootNodeName.pwszString=pwszLocalString;
                pwszRootNodeName.cchString=(DRM_DWORD)DRMCRT_wcslen(pwszLocalString);
            }
            else{
                pwszRootNodeName.pwszString=NULL;
                pwszRootNodeName.cchString=0;
            }   
        }
        else{
            iRootNodeNameNullFlag=1;
        }
    }
    if(iOption&8)
        pwszRootNodeName.cchString=(DRM_DWORD)atol(argv[3]);

    ChkDR(DRM_XMB_CreateDocument(cbXmlContext, pbXmlContext, iRootNodeNameNullFlag>0?NULL:&pwszRootNodeName));
   
ErrorExit:
    /*pbXmlContext is freed somewhere else*/
    return dr;
}

DRM_RESULT DRM_CALL TestXMLBReallocDocument(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    _XMBContext *pbLocalXmlContext=NULL;
    _XMBContext *pbNewXmlContext=NULL;
    DRM_DWORD cbNewXmlContext=0;
    int j=0;

    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBReallocDocument: argv[%d]=%s", i, argv[i]);
    }
    /*at least 4 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestXMLBReallocDocument: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[1])||!IsDigitalOnlyString(argv[3])){ 
        Log("Trace", "\t\tFrom TestXMLBReallocDocument: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
  
    iOption=(long)atol(argv[3]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        if(argv[0]!=NULL){
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            if(!pbLocalXmlContext)
                ChkDR(DRM_E_FAIL);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }
    cbNewXmlContext=(DRM_DWORD)atol(argv[1]);
    if(argv[2]!=NULL){
        pbNewXmlContext=(_XMBContext*)Oem_MemAlloc(cbNewXmlContext+1);
        if(!pbNewXmlContext)
            ChkDR(DRM_E_FAIL);
        memset(pbNewXmlContext, 0, cbNewXmlContext+1);
    }
    if(j==1){
        ChkDR(DRM_XMB_ReallocDocument(pbLocalXmlContext,cbNewXmlContext,pbNewXmlContext));
    }
    else{
        ChkDR(DRM_XMB_ReallocDocument(pbXmlContext,cbNewXmlContext,pbNewXmlContext));
    }
    /*save the new pbXmlContext*/
    TestXMLBUtilReinitialize(1,NULL);
    pbXmlContext=pbNewXmlContext;
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    pbNewXmlContext=NULL;
    return dr;
}

DRM_RESULT DRM_CALL TestXMLBCloseDocument(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_STRING pszXMLString = {0,0};
    DRM_WCHAR szArrayXMLString[2000]={0};
    DRM_WCHAR szVerifyArrayXMLString[2000]={0};
    _XMBContext *pbLocalXmlContext=NULL;

    int j=0,iStringNullFlag=0;
    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBCloseDocument: argv[%d]=%s", i, argv[i]);
    }
    /*at least 5 arguments. The last one is used to decide whether we need to overwrite the context*/
    /*4th is used to verify the xml content*/
    if(argc<5){
        Log("Trace", "\t\tFrom TestXMLBCloseDocument: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[2])||!IsDigitalOnlyString(argv[4])){ 
        Log("Trace", "\t\tFrom TestXMLBCloseDocument: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
    iOption=(long)atol(argv[4]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        if(argv[0]!=NULL){
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }

    if(argv[1]!=NULL){
        if(strcmp(argv[1], "nullstring")){
            TST_UTL_mbstowcs(szArrayXMLString, argv[1], strlen(argv[1])+1);
            pszXMLString.pwszString=szArrayXMLString;
            pszXMLString.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayXMLString);
        }
    }
    else{
        iStringNullFlag=1;
    }
    
    if(j==1){
        ChkDR(DRM_XMB_CloseDocument(pbLocalXmlContext,iStringNullFlag>0?NULL:&pszXMLString));
    }   
    else{
        ChkDR(DRM_XMB_CloseDocument(pbXmlContext,iStringNullFlag>0?NULL:&pszXMLString));
    }
    /*verify the result*/
    if(argv[3]!=NULL){
        TST_UTL_mbstowcs(szVerifyArrayXMLString, argv[3], strlen(argv[3])+1);
        if(DRMCRT_wcsncmp(szVerifyArrayXMLString,pszXMLString.pwszString,DRMCRT_wcslen(szVerifyArrayXMLString))){
            ChkDR(DRM_E_FAIL);
        }
    }    
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    return dr;
}
  
DRM_RESULT DRM_CALL TestXMLBOpenNode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_CONST_STRING pszNodeName = {0,0};
    DRM_WCHAR szArrayNodeName[512]={0};
    _XMBContext *pbLocalXmlContext=NULL;
    int j=0, iNodeNameNullFlag=0;
    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBOpenNode: argv[%d]=%s", i, argv[i]);
    }
    /*at least 4 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestXMLBOpenNode: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[2])||!IsDigitalOnlyString(argv[3])){ 
        Log("Trace", "\t\tFrom TestXMLBOpenNode: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
    iOption=(long)atol(argv[3]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        if(argv[0]!=NULL){
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            if(!pbLocalXmlContext)
                ChkDR(DRM_E_FAIL);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }

    if(iOption&4){
        if(argv[1]!=NULL){
            if(strcmp(argv[1],"nullstring")){
                TST_UTL_mbstowcs(szArrayNodeName, argv[1], strlen(argv[1])+1);
                pszNodeName.pwszString=szArrayNodeName;
                pszNodeName.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayNodeName);
            }
            else{
                pszNodeName.pwszString=NULL;
                pszNodeName.cchString=0;
            }   
        }
        else{
            iNodeNameNullFlag=1;
        }
    }
    if(iOption&8)
        pszNodeName.cchString=(DRM_DWORD)atol(argv[2]);
    
    if(j==1){
        ChkDR(DRM_XMB_OpenNode(pbLocalXmlContext,iNodeNameNullFlag>0?NULL:&pszNodeName));
    }   
    else{
        ChkDR(DRM_XMB_OpenNode(pbXmlContext,iNodeNameNullFlag>0?NULL:&pszNodeName));
    }
     
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    return dr;
}

DRM_RESULT DRM_CALL TestXMLBCloseCurrNode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_STRING pszXMLFragment = {0,0};
    DRM_WCHAR szArrayXMLFragment[2048]={0};
    _XMBContext *pbLocalXmlContext=NULL;
    int j=0,iFragmentNullFlag=0;

    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBCloseCurrNode: argv[%d]=%s", i, argv[i]);
    }
    /*at least 4 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestXMLBCloseCurrNode: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[2])||!IsDigitalOnlyString(argv[3])){ 
        Log("Trace", "\t\tFrom TestXMLBCloseCurrNode: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
    iOption=(long)atol(argv[3]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        if(argv[0]!=NULL){
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            if(!pbLocalXmlContext)
                ChkDR(DRM_E_FAIL);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }

    if(argv[1]!=NULL){
        if(strcmp(argv[1], "nullstring")){
            TST_UTL_mbstowcs(szArrayXMLFragment, argv[1], strlen(argv[1])+1);
            pszXMLFragment.pwszString=szArrayXMLFragment;
            pszXMLFragment.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayXMLFragment);
        }
    }
    else{
        iFragmentNullFlag=1;
    }
    
    if(j==1){
        ChkDR(DRM_XMB_CloseCurrNode(pbLocalXmlContext,iFragmentNullFlag>0?NULL:&pszXMLFragment));
    }   
    else{
        ChkDR(DRM_XMB_CloseCurrNode(pbXmlContext,iFragmentNullFlag>0?NULL:&pszXMLFragment));
    }
     
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    return dr;
}

DRM_RESULT DRM_CALL TestXMLBGetCurrNodeName(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_STRING pszNodeName = {0,0};
    DRM_WCHAR szArrayNodeName[256]={0}; 
    _XMBContext *pbLocalXmlContext=NULL;
    int iNodeNameNullFlag=0, j=0; 

    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBGetCurrNodeName: argv[%d]=%s", i, argv[i]);
    }
    /*at least 4 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestXMLBGetCurrNodeName: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[2])||!IsDigitalOnlyString(argv[3])){ 
        Log("Trace", "\t\tFrom TestXMLBGetCurrNodeName: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
    iOption=(long)atol(argv[3]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        if(argv[0]!=NULL){
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            if(!pbLocalXmlContext)
                ChkDR(DRM_E_FAIL);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }

    if(argv[1]!=NULL){
        if(strcmp(argv[1], "nullstring")){
            TST_UTL_mbstowcs(szArrayNodeName, argv[1], strlen(argv[1])+1);
            pszNodeName.pwszString=szArrayNodeName;
            pszNodeName.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayNodeName);
        }
    }
    else{
        iNodeNameNullFlag=1;
    }
    
    if(j==1){
        ChkDR(DRM_XMB_GetCurrNodeName(pbLocalXmlContext,iNodeNameNullFlag>0?NULL:&pszNodeName));
    }   
    else{
        ChkDR(DRM_XMB_GetCurrNodeName(pbXmlContext,iNodeNameNullFlag>0?NULL:&pszNodeName));
    }
    Log("Trace", "\t\tThe obtained node name is:%S", pszNodeName.pwszString);
     
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    return dr;
}

DRM_RESULT DRM_CALL TestXMLBGetContextSize(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_DWORD cbLocalXmlContext=0;
    _XMBContext *pbLocalXmlContext=NULL;
    int j=0;
    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBGetContextSize: argv[%d]=%s", i, argv[i]);
    }
    /*at least 3 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<3){
        Log("Trace", "\t\tFrom TestXMLBGetContextSize: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[1])||!IsDigitalOnlyString(argv[2])){ 
        Log("Trace", "\t\tFrom TestXMLBGetContextSize: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
    iOption=(long)atol(argv[2]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        if(argv[0]!=NULL){
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            if(!pbLocalXmlContext)
                ChkDR(DRM_E_FAIL);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }
    cbLocalXmlContext=(long)atol(argv[1]);;

    if(j==1){
        ChkDR(DRM_XMB_GetContextSize(pbLocalXmlContext,&cbLocalXmlContext));
    }   
    else{
        ChkDR(DRM_XMB_GetContextSize(pbXmlContext,&cbLocalXmlContext));
    }
    /*overwrite the context size*/
    if(iOption&2){
        cbXmlContext=cbLocalXmlContext;  
    }
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    return dr;
}

DRM_RESULT DRM_CALL TestXMLBAddAttribute(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_CONST_STRING pszAttrName = {0,0};
    DRM_WCHAR szArrayAttrName[512]={0}; 
    DRM_WCHAR szArrayAttr[512]={0}; 
    DRM_CONST_STRING pszAttrValue = {0,0};
    DRM_WCHAR szArrayAttrValue[512]={0}; 
    _XMBContext *pbLocalXmlContext=NULL;
    int iNodeNameNullFlag=0,iNodeValueNullFlag=0, j=0; 
    
    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBAddAttribute: argv[%d]=%s", i, argv[i]);
    }
    /*at least 6 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<6){
        Log("Trace", "\t\tFrom TestXMLBAddAttribute: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[2])||!IsDigitalOnlyString(argv[4])||!IsDigitalOnlyString(argv[5])){ 
        Log("Trace", "\t\tFrom TestXMLBAddAttribute: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
    iOption=(long)atol(argv[5]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        if(argv[0]!=NULL){
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            if(!pbLocalXmlContext)
                ChkDR(DRM_E_FAIL);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }

    if(argv[1]!=NULL){
        if(strcmp(argv[1], "nullstring"))
        {
            DRM_DWORD cchAttr = ( DRM_DWORD ) min( strlen( argv[ 1 ] ), NO_OF( szArrayAttrName ) - 2 );
            TST_UTL_mbstowcs(szArrayAttrName, argv[1], cchAttr + 1);
            if((iOption&16)||(iOption&32))
            { 
                if(iOption&16)
                {
                    szArrayAttr[0] = ONE_WCHAR( ' ', '\0' );
                    DRM_BYT_CopyBytes(  ( DRM_BYTE * )szArrayAttr, SIZEOF( DRM_WCHAR ), ( DRM_BYTE * ) szArrayAttrName, 0, cchAttr * SIZEOF( DRM_WCHAR ) );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                    szArrayAttr[cchAttr + 1] = ONE_WCHAR( '\0', '\0' );
#pragma prefast(pop)
                }
                if(iOption&32)
                {
                    DRM_BYT_CopyBytes( ( DRM_BYTE * ) szArrayAttr, 0, ( DRM_BYTE * ) szArrayAttrName, 0, cchAttr * SIZEOF( DRM_WCHAR ) );
                    szArrayAttr[cchAttr] = ONE_WCHAR( ' ', '\0' );
                    szArrayAttr[cchAttr + 1] = ONE_WCHAR( '\0', '\0' );
                }
                pszAttrName.pwszString=szArrayAttr;
                pszAttrName.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayAttr);
            }
            else{
                pszAttrName.pwszString=szArrayAttrName;
                pszAttrName.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayAttrName);
            }
        }
    }
    else{
        iNodeNameNullFlag=1;
    }
    if(argv[3]!=NULL){
        if(strcmp(argv[3], "nullstring"))
        {
            DRM_DWORD cchAttr = ( DRM_DWORD ) min( strlen( argv[ 3 ] ), NO_OF( szArrayAttrName ) - 2 );
            TST_UTL_mbstowcs(szArrayAttrValue, argv[3], strlen(argv[3])+1);
            if((iOption&64)||(iOption&128)){ 
                if(iOption&64)
                {
                    szArrayAttr[0] = ONE_WCHAR( ' ', '\0' );
                    DRM_BYT_CopyBytes( ( DRM_BYTE * ) szArrayAttr, SIZEOF( DRM_WCHAR ), ( DRM_BYTE * ) szArrayAttrValue, 0, cchAttr * SIZEOF( DRM_WCHAR ) );
                    szArrayAttr[cchAttr + 1] = ONE_WCHAR( '\0', '\0' );
                }
                if(iOption&128)
                {
                    DRM_BYT_CopyBytes( ( DRM_BYTE * ) szArrayAttr, 0, ( DRM_BYTE * ) szArrayAttrValue, 0, cchAttr * SIZEOF( DRM_WCHAR ) );
                    szArrayAttr[cchAttr] = ONE_WCHAR( ' ', '\0' );
                    szArrayAttr[cchAttr + 1] = ONE_WCHAR( '\0', '\0' );
                }
                pszAttrValue.pwszString=szArrayAttr;
                pszAttrValue.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayAttr);
            }
            else{
                pszAttrValue.pwszString=szArrayAttrValue;
                pszAttrValue.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayAttrValue);
            }
        }
    }
    else{
        iNodeValueNullFlag=1;
    }
    
    if(j==1){
        ChkDR(DRM_XMB_AddAttribute(pbLocalXmlContext,iNodeNameNullFlag>0?NULL:&pszAttrName,iNodeValueNullFlag>0?NULL:&pszAttrValue));
    }   
    else{
        ChkDR(DRM_XMB_AddAttribute(pbXmlContext,iNodeNameNullFlag>0?NULL:&pszAttrName,iNodeValueNullFlag>0?NULL:&pszAttrValue));
    }
     
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    return dr;
}

DRM_RESULT DRM_CALL TestXMLBAddData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_CONST_STRING pszXMLData = {0,0};
    DRM_WCHAR szArrayXMLData[512]={0}; 
    DRM_WCHAR szArrayXMLDataSpace[512]={0}; 
    _XMBContext *pbLocalXmlContext=NULL;
    int iXmlDataNullFlag=0, j=0; 

    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBAddData: argv[%d]=%s", i, argv[i]);
    }
    /*at least 4 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestXMLBAddData: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[2])||!IsDigitalOnlyString(argv[3])){ 
        Log("Trace", "\t\tFrom TestXMLBAddData: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
    iOption=(long)atol(argv[3]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        if(argv[0]!=NULL){
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            if(!pbLocalXmlContext)
                ChkDR(DRM_E_FAIL);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }

    if(argv[1]!=NULL){
        if(strcmp(argv[1], "nullstring"))
        {
            DRM_DWORD cchXMLData = ( DRM_DWORD ) min( strlen( argv[ 1 ] ), NO_OF( szArrayXMLData ) - 2 );

            TST_UTL_mbstowcs(szArrayXMLData, argv[1], cchXMLData+1);
            if((iOption&64)||(iOption&128)){ 
                if(iOption&64)
                {
                    szArrayXMLDataSpace[0] = ONE_WCHAR( ' ', '\0' );
                    DRM_BYT_CopyBytes( ( DRM_BYTE * ) szArrayXMLDataSpace, SIZEOF( DRM_WCHAR ), ( DRM_BYTE * ) szArrayXMLData, 0, cchXMLData * SIZEOF( DRM_WCHAR ) );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                    szArrayXMLDataSpace[cchXMLData + 1] = ONE_WCHAR( '\0', '\0' );
#pragma prefast(pop)
                }
                if(iOption&128)
                {
                    DRM_BYT_CopyBytes( ( DRM_BYTE * ) szArrayXMLDataSpace, 0, ( DRM_BYTE * ) szArrayXMLData, 0, cchXMLData * SIZEOF( DRM_WCHAR ) );
                    szArrayXMLDataSpace[cchXMLData] = ONE_WCHAR( ' ', '\0' );
                    szArrayXMLDataSpace[cchXMLData + 1] = ONE_WCHAR( '\0', '\0' );
                }
                pszXMLData.pwszString=szArrayXMLDataSpace;
                pszXMLData.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayXMLDataSpace);
            }
            else{
                pszXMLData.pwszString=szArrayXMLData;
                pszXMLData.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayXMLData);
            }
        }
    }
    else{
        iXmlDataNullFlag=1;
    }
    
    if(j==1){
        ChkDR(DRM_XMB_AddData(pbLocalXmlContext,iXmlDataNullFlag>0?NULL:&pszXMLData));
    }   
    else{
        ChkDR(DRM_XMB_AddData(pbXmlContext,iXmlDataNullFlag>0?NULL:&pszXMLData));
    }
     
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    return dr;
}

DRM_RESULT DRM_CALL TestXMLBAddCData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_CONST_STRING pszXMLCData = {0,0};
    DRM_WCHAR szArrayXMLCData[512]={0}; 
    DRM_WCHAR szArrayXMLCDataSpace[512]={0}; 
    _XMBContext *pbLocalXmlContext=NULL;
    int iXmlCDataNullFlag=0, j=0; 

    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBAddCData: argv[%d]=%s", i, argv[i]);
    }
    /*at least 4 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestXMLBAddCData: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[2])||!IsDigitalOnlyString(argv[3])){ 
        Log("Trace", "\t\tFrom TestXMLBAddCData: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
    iOption=(long)atol(argv[3]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1){
        if(argv[0]!=NULL){
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            if(!pbLocalXmlContext)
                ChkDR(DRM_E_FAIL);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }

    if(argv[1]!=NULL){
        if(strcmp(argv[1], "nullstring"))
        {
            DRM_DWORD cchXMLCData = ( DRM_DWORD ) min( strlen( argv[ 1 ] ), NO_OF( szArrayXMLCData ) - 2 );

            TST_UTL_mbstowcs(szArrayXMLCData, argv[1], strlen(argv[1])+1);
            if((iOption&16)||(iOption&32)){ 
                if(iOption&16){
                    szArrayXMLCDataSpace[0] = ONE_WCHAR( ' ', '\0' );
                    DRM_BYT_CopyBytes( ( DRM_BYTE * ) szArrayXMLCDataSpace, SIZEOF( DRM_WCHAR ), ( DRM_BYTE * ) szArrayXMLCData, 0, cchXMLCData * SIZEOF( DRM_WCHAR ) );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                    szArrayXMLCDataSpace[cchXMLCData + 1] = ONE_WCHAR( '\0', '\0' );
#pragma prefast(pop)
                }
                if(iOption&32){
                    DRM_BYT_CopyBytes( ( DRM_BYTE * ) szArrayXMLCDataSpace, 0, ( DRM_BYTE * ) szArrayXMLCData, 0, cchXMLCData * SIZEOF( DRM_WCHAR ) );
                    szArrayXMLCDataSpace[cchXMLCData] = ONE_WCHAR( ' ', '\0' );
                    szArrayXMLCDataSpace[cchXMLCData + 1] = ONE_WCHAR( '\0', '\0' );
                }
                pszXMLCData.pwszString=szArrayXMLCDataSpace;
                pszXMLCData.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayXMLCDataSpace);
            }
            else{
                pszXMLCData.pwszString=szArrayXMLCData;
                pszXMLCData.cchString=(DRM_DWORD)DRMCRT_wcslen(szArrayXMLCData);
            }
        }
    }
    else{
        iXmlCDataNullFlag=1;
    }
    
    if(j==1){
        ChkDR(DRM_XMB_AddCData(pbLocalXmlContext,iXmlCDataNullFlag>0?NULL:&pszXMLCData));
    }   
    else{
        ChkDR(DRM_XMB_AddCData(pbXmlContext,iXmlCDataNullFlag>0?NULL:&pszXMLCData));
    }
     
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    return dr;
}

/**********************************************************************
**
** Function: TestXMLBAppendNode   
**
** Synopsis: Tests function DRM_XMB_AppendNode ( adding string to XML context )
**              
**
** Arguments:   IN argv[0] - specifies if local context should be used
**              IN argv[1] - XML string (node) to append to context     
**              IN argv[2] - length of XML string
**              IN argv[3] - flags (see description at the beginning of the file)
**
** Returns:     DRM_SUCCESS on success. 
**              DRM I/O errors
**      
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBAppendNode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long i,iOption=0;
    DRM_CONST_STRING pwszXMLString = {0};
    _XMBContext *pbLocalXmlContext=NULL;
    int iXmlStringNullFlag=0, j=0; 

    for(i=0; i<argc; i++){
        Log("Trace", "\t\tFrom TestXMLBAppendNode: argv[%d]=%s", i, argv[i]);
    }
    /*at least 4 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<4){
        Log("Trace", "\t\tFrom TestXMLBAppendNode: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
    }
    if(!IsDigitalOnlyString(argv[2])||!IsDigitalOnlyString(argv[3])){ 
        Log("Trace", "\t\tFrom TestXMLBAppendNode: error in the argument.");
        ChkDR(DRM_E_FAIL);
    }
    iOption=(long)atol(argv[3]);
    /*the 1st bit is to overwrite pbXmlContext*/ 
    if(iOption&1)
    {
        if(argv[0]!=NULL)
        {
            pbLocalXmlContext=(_XMBContext*)Oem_MemAlloc((DRM_DWORD)strlen(argv[0])*sizeof(DRM_BYTE)+1);
            if(!pbLocalXmlContext)
                ChkDR(DRM_E_FAIL);
            memcpy(pbLocalXmlContext, argv[0], strlen(argv[0])+1);
        }
        j=1;
    }

    if(argv[1]!=NULL){
        if(strcmp(argv[1], "nullstring"))
            ChkDR( MakeDRMString(&pwszXMLString, argv[1]) );
    }
    else{
        iXmlStringNullFlag=1;
    }
    
    if(j==1){
        ChkDR(DRM_XMB_AppendNode(pbLocalXmlContext,iXmlStringNullFlag>0?NULL:&pwszXMLString));
    }   
    else{
        ChkDR(DRM_XMB_AppendNode(pbXmlContext,iXmlStringNullFlag>0?NULL:&pwszXMLString));
    }
     
ErrorExit:
    SAFE_OEM_FREE(pbLocalXmlContext);
    FREEDRMSTRING( pwszXMLString );
    return dr;
}

/**********************************************************************
**
** Function: TestXMLBWriteTag
**
** Synopsis: Test function DRM_XMB_WriteTag
**              
** Arguments:   IN argv[0] - tag
**              IN argv[1] - data
**              IN argv[2] - attribute name
**              IN argv[3] - attribute value 
**              IN argv[4] - TRUE if tag should be closed
**              IN argv[5] - TRUE if CallXmlBuilder macro should be used, FALSE otherwise
**
** Returns:     DRM_SUCCESS on success. 
**              DRM I/O errors
**      
** Note:        Global context is used only 
**            
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBWriteTag(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT        dr             = DRM_SUCCESS;
    DRM_CONST_STRING  dstrTag        = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrData       = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrAttrName   = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrAttrValue  = EMPTY_DRM_STRING;
    DRM_BOOL          fCloseTag      = TRUE;
    DRM_BOOL          fReallocateBuf = FALSE;

    ChkDR( _LogARGs( argc, argv, 6, "TestXMLBWriteTag" ) );
    ChkArg( argc == 6 );

    if( argv[0]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrTag, argv[0] ) );
    }

    if( argv[1]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrData, argv[1] ) );
    }

    if( argv[2]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrAttrName, argv[2] ) );
    }

    if( argv[3]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrAttrValue, argv[3] ) );
    }

    ChkArg( argv[4] ); 
    ChkDR( StringToBool( argv[4], &fCloseTag ) );

    ChkArg( argv[5] ); 
    ChkDR( StringToBool( argv[5], &fReallocateBuf ) );

    if( fReallocateBuf )
    {
        CallXmlBuilder( pbXmlContext, 
                        DRM_XMB_WriteTag( pbXmlContext, 
                                          (dstrTag.pwszString==NULL)?NULL:&dstrTag,
                                          (dstrData.pwszString==NULL)?NULL:&dstrData,
                                          (dstrAttrName.pwszString==NULL)?NULL:&dstrAttrName,
                                          (dstrAttrValue.pwszString==NULL)?NULL:&dstrAttrValue,
                                          fCloseTag?wttClosed:wttOpen ) );

    }
    else
    {
        ChkDR( DRM_XMB_WriteTag( pbXmlContext, 
                                 (dstrTag.pwszString==NULL)?NULL:&dstrTag,
                                 (dstrData.pwszString==NULL)?NULL:&dstrData,
                                 (dstrAttrName.pwszString==NULL)?NULL:&dstrAttrName,
                                 (dstrAttrValue.pwszString==NULL)?NULL:&dstrAttrValue,
                                 fCloseTag?wttClosed:wttOpen ) );
    }

ErrorExit:
    FREEDRMSTRING( dstrTag );
    FREEDRMSTRING( dstrData );
    FREEDRMSTRING( dstrAttrName );
    FREEDRMSTRING( dstrAttrValue );
    
    return dr;
}


/**********************************************************************
**
** Function: TestWriteCDATATag
**
** Synopsis: Test function DRM_XMB_WriteCDATATag
**              
** Arguments:   IN argv[0] - tag
**              IN argv[1] - CDATA
**              IN argv[2] - attribute label
**              IN argv[3] - attribute text
**              IN argv[4] - TRUE if tag should be closed
**              IN argv[5] - TRUE if CallXmlBuilder macro should be used, FALSE otherwise
**
** Returns:     DRM_SUCCESS on success. 
**              DRM I/O errors
**      
** Note:        Global context is used only  
**            
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBWriteCDATATag(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT        dr             = DRM_SUCCESS;
    DRM_CONST_STRING  dstrTag        = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrCDATA      = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrAttrLabel  = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrAttrText   = EMPTY_DRM_STRING;
    DRM_BOOL          fCloseTag      = FALSE;
    DRM_BOOL          fReallocateBuf = FALSE;

    ChkDR( _LogARGs( argc, argv, 6, "TestWriteCDATATag" ) );
    ChkArg( argc == 6 );
    
    if( argv[0]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrTag, argv[0] ) );
    }

    if( argv[1]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrCDATA, argv[1] ) );
    }

    if( argv[2]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrAttrLabel, argv[2] ) );
    }

    if( argv[3]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrAttrText, argv[3] ) );
    }

    ChkArg( argv[4] ); 
    ChkDR( StringToBool( argv[4], &fCloseTag ) );

    ChkArg( argv[5] ); 
    ChkDR( StringToBool( argv[5], &fReallocateBuf ) );

    if( fReallocateBuf )
    {
        CallXmlBuilder( pbXmlContext, 
                        DRM_XMB_WriteCDATATag( pbXmlContext, 
                                               (dstrTag.pwszString==NULL)?NULL:&dstrTag,
                                               (dstrCDATA.pwszString==NULL)?NULL:&dstrCDATA,
                                               (dstrAttrLabel.pwszString==NULL)?NULL:&dstrAttrLabel,
                                               (dstrAttrText.pwszString==NULL)?NULL:&dstrAttrText,
                                               fCloseTag?wttClosed:wttOpen ) );

    }
    else
    {
        ChkDR( DRM_XMB_WriteCDATATag( pbXmlContext, 
                                      (dstrTag.pwszString==NULL)?NULL:&dstrTag,
                                      (dstrCDATA.pwszString==NULL)?NULL:&dstrCDATA,
                                      (dstrAttrLabel.pwszString==NULL)?NULL:&dstrAttrLabel,
                                      (dstrAttrText.pwszString==NULL)?NULL:&dstrAttrText,
                                      fCloseTag?wttClosed:wttOpen ) );
    }

ErrorExit:
    FREEDRMSTRING( dstrTag );
    FREEDRMSTRING( dstrCDATA );
    FREEDRMSTRING( dstrAttrLabel );
    FREEDRMSTRING( dstrAttrText );
    
    return dr;
}


/**********************************************************************
**
** Function: TestXMLBAddNode   
**
** Synopsis: Test function TestXMLBAddNode
**              
** Arguments:   IN argv[0] - node name
**              IN argv[1] - node value     
**              IN argv[2] - TRUE if CallXmlBuilder macro should be used, FALSE otherwise
**
** Returns:     DRM_SUCCESS on success. 
**              DRM I/O errors
**      
** Note:        Global context is used only 
**            
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBAddNode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT        dr                = DRM_SUCCESS;
    DRM_CONST_STRING  dstrNodeName      = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrNodeValue     = EMPTY_DRM_STRING;
    DRM_BOOL          fReallocateBuf    = FALSE;

    ChkDR( _LogARGs( argc, argv, 3, "TestXMLBAddNode" ) );
    ChkArg( argc == 3 );
    
    if( argv[0]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrNodeName, argv[0] ) );
    }

    if( argv[1]!=NULL )
    {
        ChkDR( MakeDRMString( &dstrNodeValue, argv[1] ) );
    }

    ChkArg( argv[2] ); 
    ChkDR( StringToBool( argv[2], &fReallocateBuf ) );

    if( fReallocateBuf )
    {
        CallXmlBuilder( pbXmlContext, 
                        DRM_XMB_AddXMLNode( pbXmlContext, 
                                            (dstrNodeName.pwszString==NULL)?NULL:&dstrNodeName,
                                            (dstrNodeValue.pwszString==NULL)?NULL:&dstrNodeValue ) );

    }
    else
    {
        ChkDR( DRM_XMB_AddXMLNode( pbXmlContext, 
                                   (dstrNodeName.pwszString==NULL)?NULL:&dstrNodeName,
                                   (dstrNodeValue.pwszString==NULL)?NULL:&dstrNodeValue ) );
    }

ErrorExit:
    FREEDRMSTRING( dstrNodeName );
    FREEDRMSTRING( dstrNodeValue );
    
    return dr;
}

/**********************************************************************
**
** Function: TestXMLBValidateDocument   
**
** Synopsis: Validates file or global XML context
**              
**
** Arguments:   IN argv[0] - path to XML file or NULL for pbXmlContext
**              IN argv[1] - XML file name or NULL for pbXmlContext   
**
** Returns:     DRM_SUCCESS on success. 
**              DRM I/O errors
**              DRM_E_FAIL if XML is not valid
**      
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBValidateDocument(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrXML = EMPTY_DRM_STRING;
    DRM_BOOL         fReleaseMem = FALSE;

    ChkArg( argc == 2 );

    if ( argv[0] == NULL || argv[1] == NULL )
    {
        dstrXML.pwszString = pbXmlContext->XmlString;
        dstrXML.cchString = pbXmlContext->wNextStringPos;
    }
    else 
    {
        ChkBOOL( LoadTestFile( argv[0], argv[1], (DRM_BYTE**)&dstrXML.pwszString, &dstrXML.cchString ), DRM_E_FILEOPEN);
        dstrXML.cchString /= SIZEOF( DRM_WCHAR );
        fReleaseMem = TRUE;
    }
    ChkBOOL( DRM_XML_Validate( &dstrXML ), DRM_E_FAIL );
    
ErrorExit:  
    if ( fReleaseMem )
    {
        FREEDRMSTRING( dstrXML );
    }
    return dr;
} 

/* Get the default implementations for WrapTestRun, PreTestCase and PostTestCase */
IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE( XMLBuilder )

BEGIN_TEST_API_LIST( XMLBuilder )
    API_ENTRY(TestXMLBCreateDocument)
    API_ENTRY(TestXMLBReallocDocument)
    API_ENTRY(TestXMLBCloseDocument)
    API_ENTRY(TestXMLBOpenNode)
    API_ENTRY(TestXMLBCloseCurrNode)
    API_ENTRY(TestXMLBGetCurrNodeName)
    API_ENTRY(TestXMLBGetContextSize)
    API_ENTRY(TestXMLBAddAttribute)
    API_ENTRY(TestXMLBAddData)
    API_ENTRY(TestXMLBAddCData)
    API_ENTRY(TestXMLBAppendNode)
    API_ENTRY(TestXMLBUtilReinitialize)
    API_ENTRY(TestXMLBWriteTag)    
    API_ENTRY(TestXMLBWriteCDATATag)    
    API_ENTRY(TestXMLBAddNode)    
    API_ENTRY(TestXMLBValidateDocument)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
