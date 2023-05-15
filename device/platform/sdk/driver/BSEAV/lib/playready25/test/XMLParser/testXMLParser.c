/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*Most of function has additional argument using as flags to
  decide which input should be overwritten:
    1st bit overwrite g_dstrXML string
    2nd bit overwrite g_dstrXML size
    3rd bit overwrite g_dstrXMLNode string
    4th bit overwrite g_dstrXMLNode size
    5th bit overwrite g_dstrXMLNodeData string
    6th bit overwrite g_dstrXMLNodeData size
    7th bit overwrite g_dstrXML with the resulted g_dstrXMLNode
    8th bit overwrite g_dstrXML with the resulted g_dstrXMLNodeData
*/

#include <stdafx.h>
#include <ctype.h>
#include <drmxmlparser.h>
#include <XMLParserTestAPI.h>
#include <drmmathsafe.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

static DRM_DWORD g_cbLicense=0;
static DRM_BYTE *g_pbLicense=NULL;
static DRM_CONST_STRING g_dstrXML = {0,0};
static DRM_CONST_STRING g_dstrXMLNodeData = {0,0};
static DRM_CONST_STRING g_dstrXMLNode = {0,0};
static DRM_ANSI_CONST_STRING g_dastrXML = {0,0};

static DRM_SUBSTRING g_dasstrNodeData  = {0,0};
static DRM_SUBSTRING g_dasstrNode      = {0,0};

 /*  Reinitialize all if iIndex=0*/
DRM_RESULT DRM_CALL TestXMLPUtilReinitialize(long iIndex, __in_opt char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    if (argv!=NULL){
        iIndex=(int)atoi(argv[0]);
    }

     if(iIndex==0 ||iIndex==1){
        SAFE_OEM_FREE(g_pbLicense);
        g_pbLicense=NULL;
        g_dstrXML.pwszString=0;
        g_dastrXML.pszString=0;
      }
     if(iIndex==0 ||iIndex==2){
        g_cbLicense=0;
        g_dstrXML.cchString=0;
        g_dastrXML.cchString=0;
    }
    if(iIndex==0 ||iIndex==3){
         g_dstrXMLNode.pwszString=0;
    }
     if(iIndex==0 ||iIndex==4){
         g_dstrXMLNode.cchString=0;
        g_dasstrNode.m_cch=0;
    }
    if(iIndex==0 ||iIndex==5){
         g_dstrXMLNodeData.pwszString=0;
    }
     if(iIndex==0 ||iIndex==6){
         g_dstrXMLNodeData.pwszString=0;
    }

    return dr;
}

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
     long iOption=0;
    DRM_CONST_STRING szTag = {0,0};
    DRM_CONST_STRING szAttrName = {0,0};
    DRM_CONST_STRING szAttrValue = {0,0};
    DRM_WCHAR szArrayAttrName[256]={0};
    DRM_WCHAR szArrayAttrValue[256]={0};
    DRM_WCHAR szArrayXMLTag[256]={0};
    DRM_DWORD nthNode=0;
    int iXMLNullFlag=0, iNodeNullFlag=0, iNodeDataNullFlag=0;

     /*at least 14 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<14){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNode: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
     }
    if(!IsDigitalOnlyString(argv[1])||!IsDigitalOnlyString(argv[3])||!IsDigitalOnlyString(argv[5])  ||
       !IsDigitalOnlyString(argv[7])||!IsDigitalOnlyString(argv[8])||!IsDigitalOnlyString(argv[10]) ||
       !IsDigitalOnlyString(argv[12])||!IsDigitalOnlyString(argv[13])){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNode: error in the argument.");
        ChkDR(DRM_E_FAIL);
     }

     iOption=(long)atol(argv[13]);
    /*the 1st bit is to overwrite szxml*/
    if(iOption&1){
        TestXMLPUtilReinitialize(1,NULL);
        if(argv[0]){
             if(!(*argv[0])){
                g_cbLicense=1;
                ChkMem(g_pbLicense=(DRM_BYTE*)Oem_MemAlloc(g_cbLicense*sizeof(DRM_WCHAR)));
                g_pbLicense[0] = 0;
            } else {
                 /*load license from a file. The file name is given by argv[0]*/
                ChkArg(LoadTestFile("xml", argv[0], &g_pbLicense, &g_cbLicense));
            }
            g_dstrXML.pwszString=(const DRM_WCHAR*)g_pbLicense;
            g_dstrXML.cchString=g_cbLicense>>1;
        }
        else
            iXMLNullFlag=1;
       }
     /*the 2nd bit is to overwrite szxml size*/
    if(iOption&2){
        g_dstrXML.cchString=(DRM_DWORD)atol(argv[1]);
       }
    if(argv[2]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &cch ) );
        TST_UTL_mbstowcs(szArrayXMLTag, argv[2], cch+1);
        szTag.pwszString=szArrayXMLTag;
    }
    else{
        szTag.pwszString=NULL;
    }
    szTag.cchString=(DRM_DWORD)atol(argv[3]);
    if(argv[4]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[4] ), &cch ) );
        TST_UTL_mbstowcs(szArrayAttrName, argv[4], cch+1);
        szAttrName.pwszString=szArrayAttrName;
    }
    else{
        szAttrName.pwszString=NULL;
    }
    szAttrName.cchString=(DRM_DWORD)atol(argv[5]);
    if(argv[6]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[6] ), &cch ) );
        TST_UTL_mbstowcs(szArrayAttrValue, argv[6], cch+1);
        szAttrValue.pwszString=szArrayAttrValue;
    }
    else{
        szAttrValue.pwszString=NULL;
    }
    szAttrValue.cchString=(DRM_DWORD)atol(argv[7]);
    nthNode=(DRM_DWORD)atol(argv[8]);
    if(iOption&8)
        g_dstrXMLNode.cchString=(DRM_DWORD)atol(argv[10]);
    if(iOption&32)
        g_dstrXMLNodeData.cchString=(DRM_DWORD)atol(argv[12]);
    if(argv[9]==NULL)
        iNodeNullFlag=1;
    if(argv[11]==NULL)
        iNodeDataNullFlag=1;
    /*overwrite g_dstrXML with the resulted g_dstrXMLNode or g_dstrXMLNodeData*/
    if(iOption&64){
        g_dstrXML.pwszString=g_dstrXMLNode.pwszString;
        g_dstrXML.cchString=g_dstrXMLNode.cchString;
    }
    if(iOption&128){
        g_dstrXML.pwszString=g_dstrXMLNodeData.pwszString;
        g_dstrXML.cchString=g_dstrXMLNodeData.cchString;
    }
    ChkDR(DRM_XML_GetNode(iXMLNullFlag>0?NULL:&g_dstrXML,&szTag,&szAttrName,&szAttrValue,nthNode,iNodeNullFlag>0?NULL:&g_dstrXMLNode,iNodeDataNullFlag>0?NULL:&g_dstrXMLNodeData));

ErrorExit:
      return dr;
}

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLSubNode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
     long iOption=0;
    DRM_CONST_STRING szTag = {0,0};
    DRM_CONST_STRING szAttrName = {0,0};
    DRM_CONST_STRING szAttrValue = {0,0};
    DRM_WCHAR szArrayAttrName[256]={0};
    DRM_WCHAR szArrayAttrValue[256]={0};
    DRM_WCHAR szArrayXMLTag[256]={0};
    DRM_DWORD nthNode=0, nAtNthLayer=0;

    /*at least 15 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<15){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLSubNode: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
     }
    if(!IsDigitalOnlyString(argv[1])||!IsDigitalOnlyString(argv[3])||!IsDigitalOnlyString(argv[5])  ||
       !IsDigitalOnlyString(argv[7])||!IsDigitalOnlyString(argv[8])||!IsDigitalOnlyString(argv[10]) ||
       !IsDigitalOnlyString(argv[12])||!IsDigitalOnlyString(argv[13])||!IsDigitalOnlyString(argv[14])){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLSubNode: error in the argument.");
        ChkDR(DRM_E_FAIL);
     }

     iOption=(long)atol(argv[14]);
    /*the 1st bit is to overwrite szxml*/
    if(iOption&1){
        TestXMLPUtilReinitialize(1,NULL);
        if(argv[0]!=NULL){
             if(!(*argv[0])){
                g_cbLicense=1;
                ChkMem(g_pbLicense=(DRM_BYTE*)Oem_MemAlloc(g_cbLicense*sizeof(DRM_WCHAR)));
                g_pbLicense[0] = 0;
            } else {
                 /*load license from a file. The file name is given by argv[0]*/
                ChkArg(LoadTestFile("xml", argv[0], &g_pbLicense, &g_cbLicense));
            }
            g_dstrXML.pwszString=(const DRM_WCHAR*)g_pbLicense;
            g_dstrXML.cchString=g_cbLicense>>1;
        }
       }
     /*the 2nd bit is to overwrite szxml size*/
    if(iOption&2){
        g_dstrXML.cchString=(DRM_DWORD)atol(argv[1]);
       }
    if(argv[2]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &cch ) );
        TST_UTL_mbstowcs(szArrayXMLTag, argv[2], cch+1);
        szTag.pwszString=szArrayXMLTag;
    }
    else{
        szTag.pwszString=NULL;
    }
    szTag.cchString=(DRM_DWORD)atol(argv[3]);
    if(argv[4]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[4] ), &cch ) );
        TST_UTL_mbstowcs(szArrayAttrName, argv[4], cch+1);
        szAttrName.pwszString=szArrayAttrName;
    }
    else{
        szAttrName.pwszString=NULL;
    }
    szAttrName.cchString=(DRM_DWORD)atol(argv[5]);
    if(argv[6]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[6] ), &cch ) );
        TST_UTL_mbstowcs(szArrayAttrValue, argv[6], cch+1);
        szAttrValue.pwszString=szArrayAttrValue;
    }
    else{
        szAttrValue.pwszString=NULL;
    }
    szAttrValue.cchString=(DRM_DWORD)atol(argv[7]);
    nthNode=(DRM_DWORD)atol(argv[8]);
    if(iOption&4)
        g_dstrXMLNode.cchString=(DRM_DWORD)atol(argv[10]);
    if(iOption&8)
        g_dstrXMLNodeData.cchString=(DRM_DWORD)atol(argv[12]);
    nAtNthLayer=(DRM_DWORD)atol(argv[13]);

    /*overwrite g_dstrXML with the resulted g_dstrXMLNode or g_dstrXMLNodeData*/
    if(iOption&64){
        g_dstrXML.pwszString=g_dstrXMLNode.pwszString;
        g_dstrXML.cchString=g_dstrXMLNode.cchString;
    }
    if(iOption&128){
        g_dstrXML.pwszString=g_dstrXMLNodeData.pwszString;
        g_dstrXML.cchString=g_dstrXMLNodeData.cchString;
    }
    ChkDR(DRM_XML_GetSubNode(&g_dstrXML,&szTag,&szAttrName,&szAttrValue,nthNode,&g_dstrXMLNode,&g_dstrXMLNodeData, nAtNthLayer));

ErrorExit:
      return dr;
}

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeAttribute(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
     long iOption=0;
    DRM_CONST_STRING szAttrName = {0,0};
    DRM_CONST_STRING szAttrValue = {0,0};
    DRM_WCHAR szArrayAttrName[256]={0};
    DRM_WCHAR szArrayAttrValue[256]={0};
    DRM_WCHAR szVerifyArrayAttrValue[256]={0};
    DRM_WCHAR szArrayXMLNode[256]={0};
    int iNodeNullFlag=0,iAttrNameNullFlag=0;

      /*at least 8 arguments. The last one is used to decide whether we need to overwrite the context*/
    /*7th argument to verify the value content*/
    if(argc<8){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNodeAttribute: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
     }
    if(!IsDigitalOnlyString(argv[1])||!IsDigitalOnlyString(argv[3])||!IsDigitalOnlyString(argv[5])||
        !IsDigitalOnlyString(argv[7])){
          Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNodeAttribute: error in the argument.");
        ChkDR(DRM_E_FAIL);
     }

     iOption=(long)atol(argv[7]);
    /*the 3rd bit is to overwrite g_dstrXMLNode*/
    if(iOption&4){
        TestXMLPUtilReinitialize(3,NULL);
        if(argv[0]!=NULL){
            if(strcmp(argv[0], "nullstring")){
                DRM_DWORD cch = 0;
                ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &cch ) );
                TST_UTL_mbstowcs(szArrayXMLNode, argv[0], cch+1);
                g_dstrXMLNode.pwszString=szArrayXMLNode;
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen(szArrayXMLNode), &g_dstrXMLNode.cchString ) );
            }
         }
        else
            iNodeNullFlag=1;
       }
     /*the 4th bit is to overwrite g_dstrXMLNode size*/
    if(iOption&8){
        g_dstrXMLNode.cchString=(DRM_DWORD)atol(argv[1]);
       }
    if(argv[2]!=NULL){
        if(strcmp(argv[2], "nullstring")){
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &cch ) );
            TST_UTL_mbstowcs(szArrayAttrName, argv[2], cch+1);
            szAttrName.pwszString=szArrayAttrName;
        }
    }
    else{
        iAttrNameNullFlag=1;
    }
    szAttrName.cchString=(DRM_DWORD)atol(argv[3]);
    if(argv[4]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[4] ), &cch ) );
        TST_UTL_mbstowcs(szArrayAttrValue, argv[4], cch+1);
        szAttrValue.pwszString=szArrayAttrValue;
    }
    else{
        szAttrValue.pwszString=0;
    }
    szAttrValue.cchString=(DRM_DWORD)atol(argv[5]);

    ChkDR(DRM_XML_GetNodeAttribute(iNodeNullFlag>0?NULL:&g_dstrXMLNode,iAttrNameNullFlag>0?NULL:&szAttrName,&szAttrValue));

    /*verify the attribute value*/
    if(argv[6]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[6] ), &cch ) );
        TST_UTL_mbstowcs(szVerifyArrayAttrValue, argv[6], cch+1);
        if(DRMCRT_wcsncmp(szVerifyArrayAttrValue,szAttrValue.pwszString,DRMCRT_wcslen(szVerifyArrayAttrValue))){
            ChkDR(DRM_E_FAIL);
        }
    }

ErrorExit:
      return dr;
}

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeA(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
     long iOption=0;
    DRM_ANSI_CONST_STRING szTag = {0,0};
    DRM_ANSI_CONST_STRING szAttrName = {0,0};
    DRM_ANSI_CONST_STRING szAttrValue = {0,0};
    DRM_CHAR szArrayAttrName[256]={0};
    DRM_CHAR szArrayAttrValue[256]={0};
    DRM_CHAR szArrayXMLTag[256]={0};
    DRM_SUBSTRING dasstr;
    DRM_DWORD nthNode=0;

     /*at least 14 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<14){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNode: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
     }
    if(!IsDigitalOnlyString(argv[1])||!IsDigitalOnlyString(argv[3])||!IsDigitalOnlyString(argv[5])  ||
       !IsDigitalOnlyString(argv[7])||!IsDigitalOnlyString(argv[8])||!IsDigitalOnlyString(argv[10]) ||
       !IsDigitalOnlyString(argv[12])||!IsDigitalOnlyString(argv[13])){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNode: error in the argument.");
        ChkDR(DRM_E_FAIL);
     }

     iOption=(long)atol(argv[13]);

    /*the 1st bit is to overwrite szxml*/
    if( iOption & 1 )
    {
        TestXMLPUtilReinitialize( 1, NULL );
        ChkArg( argv[0] );
        if( !( *argv[0] ) )
        {
            g_cbLicense=1;
            ChkMem( g_pbLicense=(DRM_BYTE*)Oem_MemAlloc( g_cbLicense * sizeof(DRM_CHAR) ) );
            g_pbLicense[0] = 0;
        } else {
            /*
            ** LoadTestFile() does not check file format.
            ** Make sure that file loaded contains ANSI data.
            */
            ChkArg( LoadTestFile( "xml", argv[0], &g_pbLicense, &g_cbLicense ) );
        }
        g_dastrXML.pszString=(const DRM_CHAR*)g_pbLicense;
        g_dastrXML.cchString=g_cbLicense;
       }

     /*the 2nd bit is to overwrite szxml size*/
    if(iOption&2){
        g_dastrXML.cchString=(DRM_DWORD)atol(argv[1]);
       }

    if(argv[2]!=NULL){
        DRM_SIZE_T cb = DRMCRT_strlen( argv[2] );
        memcpy(szArrayXMLTag, argv[2], min( cb+1, SIZEOF( szArrayXMLTag ) ) );
        szTag.pszString=szArrayXMLTag;
    }
    else{
        szTag.pszString=NULL;
    }
    szTag.cchString=(DRM_DWORD)atol(argv[3]);

    if(argv[4]!=NULL){
        DRM_SIZE_T cb = DRMCRT_strlen( argv[4] );
        memcpy(szArrayAttrName, argv[4], min( cb+1, SIZEOF( szArrayAttrName ) ) );
        szAttrName.pszString=szArrayAttrName;
    }
    else{
        szAttrName.pszString=NULL;
    }
    szAttrName.cchString=(DRM_DWORD)atol(argv[5]);

    if(argv[6]!=NULL){
        DRM_SIZE_T cb = DRMCRT_strlen( argv[6] );
        memcpy(szArrayAttrValue, argv[6], min( cb+1, SIZEOF( szArrayAttrValue ) ) );
        szAttrValue.pszString=szArrayAttrValue;
    }
    else{
        szAttrValue.pszString=NULL;
    }
    szAttrValue.cchString=(DRM_DWORD)atol(argv[7]);

    nthNode=(DRM_DWORD)atol(argv[8]);

    dasstr.m_ich = 0;
    dasstr.m_cch = g_dastrXML.cchString;

    if(iOption&8)
        g_dasstrNode.m_cch=(DRM_DWORD)atol(argv[10]);
    if(iOption&32)
        g_dasstrNodeData.m_cch=(DRM_DWORD)atol(argv[12]);

    /*overwrite g_dstrXML with the resulted g_dstrXMLNode or g_dstrXMLNodeData*/
    if(iOption&64){
        dasstr.m_ich = g_dasstrNode.m_ich;
        dasstr.m_cch = g_dasstrNode.m_cch;
    } else if(iOption&128){
        dasstr.m_ich = g_dasstrNodeData.m_ich;
        dasstr.m_cch = g_dasstrNodeData.m_cch;
    }

    ChkDR(DRM_XML_GetNodeA(g_dastrXML.pszString,
                          &dasstr,
                          &szTag,
                          &szAttrName,
                          &szAttrValue,
                           nthNode,
                           argv[9]? &g_dasstrNode: NULL,
                           argv[11]? &g_dasstrNodeData: NULL));

ErrorExit:
      return dr;
}

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLSubNodeA(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
     long iOption=0;
    DRM_ANSI_CONST_STRING szTag = {0,0};
    DRM_ANSI_CONST_STRING szAttrName = {0,0};
    DRM_ANSI_CONST_STRING szAttrValue = {0,0};
    DRM_CHAR szArrayAttrName[256]={0};
    DRM_CHAR szArrayAttrValue[256]={0};
    DRM_CHAR szArrayXMLTag[256]={0};
    DRM_DWORD nthNode=0, nAtNthLayer=0;
    DRM_SUBSTRING sstrXML;

     /*at least 15 arguments. The last one is used to decide whether we need to overwrite the context*/
    if(argc<15){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLSubNode: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
     }
    if(!IsDigitalOnlyString(argv[1])||!IsDigitalOnlyString(argv[3])||!IsDigitalOnlyString(argv[5])  ||
       !IsDigitalOnlyString(argv[7])||!IsDigitalOnlyString(argv[8])||!IsDigitalOnlyString(argv[10]) ||
       !IsDigitalOnlyString(argv[12])||!IsDigitalOnlyString(argv[13])||!IsDigitalOnlyString(argv[14])){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLSubNode: error in the argument.");
        ChkDR(DRM_E_FAIL);
     }

     iOption=(long)atol(argv[14]);
    /*the 1st bit is to overwrite szxml*/
    if(iOption&1){
        TestXMLPUtilReinitialize(1,NULL);
        if(argv[0]){
            if (!(*argv[0])) {
                g_cbLicense = 1;
                ChkMem(g_pbLicense=(DRM_BYTE*)Oem_MemAlloc(g_cbLicense*sizeof(DRM_WCHAR)));
                g_pbLicense[0] = 0;
            } else {
                /*load license from a file. The file name is given by argv[0]*/
                ChkArg(LoadTestFile("xml", argv[0], &g_pbLicense, &g_cbLicense));
            }
            g_dastrXML.pszString=(const DRM_CHAR*)g_pbLicense;
            g_dastrXML.cchString=g_cbLicense;
        }
       }

     /*the 2nd bit is to overwrite szxml size*/
    if(iOption&2)
        g_dastrXML.cchString=(DRM_DWORD)atol(argv[1]);

    if(argv[2]!=NULL){
        DRM_SIZE_T cb = DRMCRT_strlen( argv[2] );
        memcpy(szArrayXMLTag, argv[2], min( cb+1, SIZEOF( szArrayXMLTag ) ) );
        szTag.pszString=szArrayXMLTag;
    } else
        szTag.pszString=NULL;
    szTag.cchString=(DRM_DWORD)atol(argv[3]);

    if(argv[4]!=NULL){
        DRM_SIZE_T cb = DRMCRT_strlen( argv[4] );
        memcpy(szArrayAttrName, argv[4], min( cb+1, SIZEOF( szArrayAttrName ) ) );
        szAttrName.pszString=szArrayAttrName;
    } else
        szAttrName.pszString=NULL;
    szAttrName.cchString=(DRM_DWORD)atol(argv[5]);

    if(argv[6]!=NULL){
        DRM_SIZE_T cb = DRMCRT_strlen( argv[6] );
        memcpy(szArrayAttrValue, argv[6], min( cb+1, SIZEOF( szArrayAttrValue ) ) );
        szAttrValue.pszString=szArrayAttrValue;
    } else
        szAttrValue.pszString=NULL;
    szAttrValue.cchString=(DRM_DWORD)atol(argv[7]);

    nthNode=(DRM_DWORD)atol(argv[8]);
    if(iOption&4)
        g_dasstrNode.m_cch=(DRM_DWORD)atol(argv[10]);
    if(iOption&8)
        g_dasstrNodeData.m_cch=(DRM_DWORD)atol(argv[12]);
    nAtNthLayer=(DRM_DWORD)atol(argv[13]);

    /*overwrite g_dstrXML with the resulted g_dstrXMLNode or g_dstrXMLNodeData*/
    if(iOption&64){
        g_dastrXML.pszString= (const DRM_CHAR *)(g_pbLicense + g_dasstrNode.m_ich);
        g_dastrXML.cchString=g_dasstrNode.m_cch;
    }
    if(iOption&128){
        g_dastrXML.pszString= (const DRM_CHAR *)(g_pbLicense + g_dasstrNodeData.m_ich);
        g_dastrXML.cchString=g_dasstrNodeData.m_cch;
    }

    sstrXML.m_ich = 0;
    sstrXML.m_cch = g_dastrXML.cchString;
    ChkDR(DRM_XML_GetSubNodeA(g_dastrXML.pszString, &sstrXML, &szTag, &szAttrName,&szAttrValue,nthNode,&g_dasstrNode,&g_dasstrNodeData, nAtNthLayer));

ErrorExit:
      return dr;
}

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeAttributeA(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
     long iOption=0;
    DRM_ANSI_CONST_STRING szAttrName      = { 0, 0 };
    DRM_SUBSTRING szAttrValue     = { 0, 0 };
    DRM_CHAR szArrayAttrName        [256] = { 0 };
    DRM_CHAR szArrayXMLNode         [256] = { 0 };

      /*at least 8 arguments. The last one is used to decide whether we need to overwrite the context*/
    /*7th argument to verify the value content*/
    if(argc<8){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNodeAttribute: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
     }
    if(!IsDigitalOnlyString(argv[1])||!IsDigitalOnlyString(argv[3])||!IsDigitalOnlyString(argv[5])||
        !IsDigitalOnlyString(argv[7])){
          Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNodeAttribute: error in the argument.");
        ChkDR(DRM_E_FAIL);
     }

     iOption=(long)atol(argv[7]);
    /*the 3rd bit is to overwrite g_dstrXMLNode*/
    if(iOption&4){
        TestXMLPUtilReinitialize(3,NULL);
        if(argv[0])
        {
            DRM_SIZE_T cb = DRMCRT_strlen( argv[0] );
            g_dastrXML.cchString = ( DRM_DWORD ) min( SIZEOF( szArrayXMLNode ) - 1, cb );
            MEMCPY(szArrayXMLNode, argv[0], g_dastrXML.cchString );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
            szArrayXMLNode[g_dastrXML.cchString] = '\0';
#pragma prefast(pop)
            g_dastrXML.pszString = szArrayXMLNode;
            g_dasstrNode.m_ich = 0;
            g_dasstrNode.m_cch = g_dastrXML.cchString;
         }
       }
     /*the 4th bit is to overwrite g_dstrXMLNode size*/
    if(iOption&8){
        g_dasstrNode.m_cch=(DRM_DWORD)atol(argv[1]);
       }
    if(argv[2])
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[2] );
        DRM_DWORD cAttr = ( DRM_DWORD ) min( SIZEOF( szArrayAttrName ) - 1, cb );
        MEMCPY( szArrayAttrName, argv[2], cAttr );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
        szArrayAttrName[cAttr] = '\0';
#pragma prefast(pop)
        szAttrName.pszString=szArrayAttrName;
        szAttrName.cchString=(DRM_DWORD)atol(argv[3]);
    }

    ChkDR(DRM_XML_GetNodeAttributeA(argv[0]? g_dastrXML.pszString: NULL,
                                    &g_dasstrNode,
                                    argv[2]? &szAttrName: NULL,
                                   &szAttrValue));
    /*verify the value content*/
    if(argv[6]){
        ChkArg(szAttrValue.m_cch == DRMCRT_strlen(argv[6]));
        ChkArg(!strncmp(argv[6], g_dastrXML.pszString + szAttrValue.m_ich, szAttrValue.m_cch));
    }
ErrorExit:
      return dr;
}

DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeCData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
     long iOption=0;
    DRM_CONST_STRING szCData = {0,0};
    DRM_WCHAR szArrayCData[256]={0};
    DRM_WCHAR szVerifyArrayCData[256]={0};
    DRM_WCHAR szArrayXMLNode[256]={0};
    int iNodeNullFlag=0;

     /*at least 6 arguments. The last one is used to decide whether we need to overwrite the context*/
    /*the 5th is to verify the CData content*/
    if(argc<6){
        Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNodeCData: error in number of arguments.");
        ChkDR(DRM_E_FAIL);
     }
    if(!IsDigitalOnlyString(argv[1])||!IsDigitalOnlyString(argv[3])||!IsDigitalOnlyString(argv[5])){
          Log("Trace", "\t\tFrom TestXMLPUtilGetXMLNodeCData: error in the argument.");
         ChkDR(DRM_E_FAIL);
    }

     iOption=(long)atol(argv[5]);
    /*the 3rd bit is to overwrite g_dstrXMLNode*/
    if(iOption&4){
        TestXMLPUtilReinitialize(3,NULL);
        if(argv[0]!=NULL){
            if(strcmp(argv[0], "nullstring")){
                DRM_DWORD cch = 0;
                ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &cch ) );
                TST_UTL_mbstowcs(szArrayXMLNode, argv[0], cch+1);
                g_dstrXMLNode.pwszString=szArrayXMLNode;
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen(szArrayXMLNode), &g_dstrXMLNode.cchString ) );
            }
            else{
                g_dstrXMLNode.pwszString=NULL;
                g_dstrXMLNode.cchString=0;
            }
        }
        else
            iNodeNullFlag=1;
       }
     /*the 4th bit is to overwrite g_dstrXMLNode size*/
    if(iOption&8){
        g_dstrXMLNode.cchString=(DRM_DWORD)atol(argv[1]);
       }
    if(argv[2]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &cch ) );
        TST_UTL_mbstowcs(szArrayCData, argv[2], cch+1);
        szCData.pwszString=szArrayCData;
    }
    else{
        szCData.pwszString=NULL;
    }
    szCData.cchString=(DRM_DWORD)atol(argv[3]);

    ChkDR(DRM_XML_GetNodeCData(iNodeNullFlag>0?NULL:&g_dstrXMLNode,&szCData));
    /*verify the CData*/
    if(argv[4]!=NULL){
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[4] ), &cch ) );
        TST_UTL_mbstowcs(szVerifyArrayCData, argv[4], cch+1);
        if(DRMCRT_wcsncmp(szVerifyArrayCData,szCData.pwszString,DRMCRT_wcslen(szVerifyArrayCData))){
            ChkDR(DRM_E_FAIL);
        }
    }
ErrorExit:
      return dr;
}

/***************************************************************************
**
** Function: TestXMLPUtilGetXMLNodeCDataA
**
** Synopsis: Function tests XML CDATA tag parsing on "real license" examples,
**           test strings and faulty input data.
**
** Arguments:
**
** [argc]     -- Total number of arguments.
** [argv]     -- Pointer to a array of arguments.
**
** argv[0]    -- XML data string. May contain real XML string, may be set into
**               a special value "nullstring" to imitate zero-length XML. Note that
**               this value may be ignored in favor of global variables with test data.
** argv[1]    -- size of XML data string, may be ignored.
** argv[2]    -- not used in this testcase
** argv[3]    -- not used in this testcase
** argv[4]    -- data string used for verification, compare it with data that XML API returns.
** argv[5]    -- combination of bits with test options:
**               0x0100 - overwrite value of global XML data string with argv[0];
**               0x1000 - overwrite size of XML data string with argv[1];
**
** Returns:      DRM_RESULT
**
** Note:     Some tests require that the global string variables
**           are initialized with XML data. The test engine will call functions
**           like TestXMLPUtilGetXMLNode for that purpose.
***************************************************************************/
DRM_RESULT DRM_CALL TestXMLPUtilGetXMLNodeCDataA(
    long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT            dr                      = DRM_SUCCESS;
    long                  iOption                 = 0;
    DRM_SUBSTRING         dasstrCData             = EMPTY_DRM_SUBSTRING;
    DRM_CHAR              szXMLData[256]          = { 0 };
    DRM_CHAR              szVerifyArrayCData[256] = { 0 };
    int                   iNodeNullFlag           = 0;

    if ( argc < 6 )
    {
        Log( "Trace", "\t\tFrom TestXMLPUtilGetXMLNodeCData: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }
    if ( !IsDigitalOnlyString( argv[1] )
        || !IsDigitalOnlyString( argv[3] )
        || !IsDigitalOnlyString( argv[5] ) )
    {
        Log( "Trace", "\t\tFrom TestXMLPUtilGetXMLNodeCData: error in the argument." );
        ChkDR( DRM_E_FAIL );
    }

    iOption = (long) atol( argv[5] );

    if ( iOption & 4 )
    {
        TestXMLPUtilReinitialize( 3, NULL );
        if( argv[0] != NULL )
        {
            if( strcmp( argv[0], "nullstring" ) )
            {
                DRM_SIZE_T cb = DRMCRT_strlen( argv[0] );
                g_dastrXML.cchString = ( DRM_DWORD ) min( SIZEOF( szXMLData ) - 1, cb );

                MEMCPY( szXMLData, argv[0], g_dastrXML.cchString );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                szXMLData[ g_dastrXML.cchString ] = '\0';
#pragma prefast(pop)
                g_dastrXML.pszString = szXMLData;

                g_dasstrNode.m_ich = 0;
                g_dasstrNode.m_cch = ( DRM_DWORD ) DRMCRT_strlen( argv[0] );

            }
            else
            {
                g_dasstrNode.m_ich = 0;
                g_dasstrNode.m_cch = 0;
            }
        }
        else
        {
            iNodeNullFlag = 1;
        }
    }

    if ( iOption & 8 )
    {
        g_dasstrNode.m_cch = (DRM_DWORD)atol( argv[1] );
    }

    ChkDR( DRM_XML_GetNodeCDataA(
            iNodeNullFlag > 0 ? NULL : g_dastrXML.pszString,
            iNodeNullFlag > 0 ? NULL : &g_dasstrNode,
            &dasstrCData ) );
    /*
    ** verify the CData
    */
    if ( argv[4] != NULL )
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[4] );
        DRM_DWORD cVerifyString = ( DRM_DWORD ) min( cb, SIZEOF( szVerifyArrayCData ) - 1 );
        MEMCPY( szVerifyArrayCData, argv[4], cVerifyString );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
        szVerifyArrayCData[cVerifyString] = '\0';
#pragma prefast(pop)

        if( DRMCRT_strncmp( szVerifyArrayCData,
                            g_dastrXML.pszString + dasstrCData.m_ich,
                            dasstrCData.m_cch ) )
        {
            ChkDR( DRM_E_FAIL );
        }
    }

ErrorExit:
      return dr;
}


/***************************************************************************
**
** Function: TestXMLPUtilEnumNextSubNode
**
** Synopsis: Function tests DRM_XML_EnumNextSubNode() API.
**
** Arguments:
**
** [argc]     -- Total number of arguments.
** [argv]     -- Pointer to a array of arguments.
**
** argv[0]    -- XML data string. Contains real XML string, may be set into
**               a special value "nullstring" to imitate zero-length XML.
** argv[1]    -- a number, level of hierarhy
** argv[2]    -- name of XML tag expected
** argv[3]    -- expected node data
** argv[4]    -- the first attribute name in a node, may be set into "nullstring"
** argv[5]    -- the first attribute value in a node, may be set into "nullstring"
** argv[6]    -- 0 or 1, whether expected node is a leaf
**
** Returns:      DRM_RESULT
***************************************************************************/
DRM_RESULT DRM_CALL TestXMLPUtilEnumNextSubNode(
                      long argc,
    __in_ecount(argc) char **argv )

{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD iLevel = 0;
    DRM_BOOL fLeaf = FALSE;
    DRM_BOOL fExpectedLeaf = FALSE;

    DRM_WCHAR   wszXMLData[256]   = { 0 };
    DRM_WCHAR   wszTagName[256]   = { 0 };
    DRM_WCHAR   wszNodeData[256]  = { 0 };
    DRM_WCHAR   wszAttrName[256]  = { 0 };
    DRM_WCHAR   wszAttrValue[256] = { 0 };

    DRM_CONST_STRING dstrTagName   = { 0, 0 };
    DRM_CONST_STRING dstrExpectedTagName = { 0, 0 };
    DRM_CONST_STRING dstrNode      = { 0, 0 };
    DRM_CONST_STRING dstrNodeData  = { 0, 0 };
    DRM_CONST_STRING dstrAttrName  = { 0, 0 };
    DRM_CONST_STRING dstrExpectedAttrName  = { 0, 0 };
    DRM_CONST_STRING dstrAttrValue = { 0, 0 };
    DRM_CONST_STRING dstrExpectedAttrValue = { 0, 0 };

    if ( argc < 7  )
    {
        Log( "Trace", "\t\tFrom TestXMLPUtilEnumNextSubNode: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    TestXMLPUtilReinitialize( 0, NULL );

    if ( strcmp( argv[0], "nullstring" ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszXMLData ) );
        TST_UTL_mbstowcs( wszXMLData, argv[0], cch + 1 );
        g_dstrXML.pwszString = wszXMLData;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszXMLData ), &g_dstrXML.cchString ) );
    }
    else
    {
        g_dstrXML.pwszString = NULL;
        g_dstrXML.cchString = 0;
    }

    iLevel =(DRM_DWORD)atol(argv[1]);

    if ( strcmp( argv[2], "nullstring" ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszTagName ) );
        TST_UTL_mbstowcs( wszTagName, argv[2], cch + 1 );
        dstrExpectedTagName.pwszString = wszTagName;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszTagName ), &dstrExpectedTagName.cchString ) );
    }
    else
    {
        dstrExpectedTagName.pwszString = NULL;
        dstrExpectedTagName.cchString = 0;
    }
    if ( strcmp( argv[3], "nullstring" ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[3] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszNodeData ) );
        TST_UTL_mbstowcs( wszNodeData, argv[3], cch + 1 );
        g_dstrXMLNodeData.pwszString = wszNodeData;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszNodeData ), &g_dstrXMLNodeData.cchString ) );
    }
    else
    {
        g_dstrXMLNodeData.pwszString = NULL;
        g_dstrXMLNodeData.cchString = 0;
    }
    if ( strcmp( argv[4], "nullstring" ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[4] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszAttrName ) );
        TST_UTL_mbstowcs( wszAttrName, argv[4], cch + 1 );
        dstrExpectedAttrName.pwszString = wszAttrName;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszAttrName ), &dstrExpectedAttrName.cchString ) );
    }
    else
    {
        dstrExpectedAttrName.pwszString = NULL;
        dstrExpectedAttrName.cchString = 0;
    }
    if ( strcmp( argv[5], "nullstring" ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[5] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszAttrValue ) );
        TST_UTL_mbstowcs( wszAttrValue, argv[5], cch + 1 );
        dstrExpectedAttrValue.pwszString = wszAttrValue;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszAttrValue ), &dstrExpectedAttrValue.cchString ) );
    }
    else
    {
        dstrExpectedAttrValue.pwszString = NULL;
        dstrExpectedAttrValue.cchString = 0;
    }

    fExpectedLeaf = (atoi(argv[6]) == 0) ? FALSE : TRUE;

    ChkDR( DRM_XML_EnumNextSubNode( &g_dstrXML,
                                    &dstrTagName,
                                    &dstrNode,
                                    &dstrNodeData,
                                    &dstrAttrName,
                                    &dstrAttrValue,
                                    &fLeaf,
                                    iLevel ) );

    if ( dstrExpectedTagName.cchString > 0
        && !DRM_UTL_DSTRStringsEqual( &dstrTagName, &dstrExpectedTagName ) )
    {
        Log( "Trace", "\t\tFrom TestXMLPUtilEnumNextSubNode: tag name is %S", dstrTagName.pwszString );
        ChkDR( DRM_E_FAIL );
    }
    if ( g_dstrXMLNodeData.cchString > 0
        && !DRM_UTL_DSTRStringsEqual( &dstrNodeData, &g_dstrXMLNodeData ) )
    {
        Log( "Trace", "\t\tFrom TestXMLPUtilEnumNextSubNode: node data is %S", dstrNodeData.pwszString );
        ChkDR( DRM_E_FAIL );
    }
    if ( dstrExpectedAttrName.cchString > 0
        && !DRM_UTL_DSTRStringsEqual( &dstrAttrName, &dstrExpectedAttrName ) )
    {
        Log( "Trace", "\t\tFrom TestXMLPUtilEnumNextSubNode: attribute name is %S", dstrAttrName.pwszString );
        ChkDR( DRM_E_FAIL );
    }
    if ( dstrExpectedAttrValue.cchString > 0
        && !DRM_UTL_DSTRStringsEqual( &dstrAttrValue, &dstrExpectedAttrValue ) )
    {
        Log( "Trace", "\t\tFrom TestXMLPUtilEnumNextSubNode: attribute value is %S", dstrAttrValue.pwszString );
        ChkDR( DRM_E_FAIL );
    }
    if ( fExpectedLeaf != fLeaf )
    {
        Log(  "Trace", "\t\tFrom TestXMLPUtilEnumNextSubNode: fLeaf does not match expected value" );
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
      return dr;
}

/***************************************************************************
**
** Function: TestXMLPUtilGetSubNodeByPath
**
** Synopsis: Function tests
** test strings and faulty input data.
**
** Arguments:
**
** [argc]     -- Total number of arguments.
** [argv]     -- Pointer to a array of arguments.
**
** argv[0]    -- XML data string. Contains real XML string, may be set into
**               a special value "nullstring" to imitate zero-length XML.
** argv[1]    -- Node path string, may be set into "nullstring"
** argv[2]    -- Attribute name string, may be set into "nullstring"
** argv[3]    -- Attribute value string, may be set into "nullstring"
**
** Returns:      DRM_RESULT
**
** Note: this DRT is testing a scenario when f_pdstrXMLNodeOut == NULL
** which is used during DRM initialization but not was not covered with DRTs.
***************************************************************************/

DRM_RESULT DRM_CALL TestXMLPUtilGetSubNodeByPath(
                                                long argc,
                              __in_ecount(argc) char **argv )
{
    DRM_RESULT dr                    = DRM_SUCCESS;
    DRM_WCHAR   wszXMLData[256]      = { 0 };
    DRM_WCHAR   wszNodePath[256]     = { 0 };
    DRM_WCHAR   wszAttrName[256]     = { 0 };
    DRM_WCHAR   wszAttrValue[256]    = { 0 };
    DRM_CONST_STRING dstrNodePath    = { 0, 0 };
    DRM_CONST_STRING dstrAttrName    = { 0, 0 };
    DRM_CONST_STRING dstrAttrValue   = { 0, 0 };

    /*
    ** at least 4 arguments.
    */
    if ( argc < 4 )
    {
        Log( "Trace", "\t\tFrom TestXMLPUtilGetXMLNode: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    TestXMLPUtilReinitialize( 1, NULL );

    if ( strcmp( argv[0], "nullstring" ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszXMLData ) );
        TST_UTL_mbstowcs( wszXMLData, argv[0], cch + 1 );
        g_dstrXML.pwszString = wszXMLData;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszXMLData ), &g_dstrXML.cchString ) );
    }
    else
    {
        g_dstrXML.pwszString = NULL;
        g_dstrXML.cchString = 0;
    }

    if ( strcmp( argv[1], "nullstring" ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[1] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszNodePath ) );
        TST_UTL_mbstowcs( wszNodePath, argv[1], cch + 1 );
        dstrNodePath.pwszString = wszNodePath;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszNodePath ), &dstrNodePath.cchString ) );
    }
    else
    {
        dstrNodePath.pwszString = NULL;
        dstrNodePath.cchString = 0;
    }

    if ( strcmp( argv[2], "nullstring" ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszAttrName ) );
        TST_UTL_mbstowcs( wszAttrName, argv[2], cch + 1 );
        dstrAttrName.pwszString = wszAttrName;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszAttrName ), &dstrAttrName.cchString ) );
    }
    else
    {
        dstrAttrName.pwszString = NULL;
        dstrAttrName.cchString = 0;
    }

    if ( strcmp( argv[3], "nullstring" ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[3] ), &cch ) );
        ChkArg( cch + 1 <= NO_OF( wszAttrValue ) );
        TST_UTL_mbstowcs( wszAttrValue, argv[3], cch + 1 );
        dstrAttrValue.pwszString = wszAttrValue;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszAttrValue ), &dstrAttrValue.cchString ) );
    }
    else
    {
        dstrAttrValue.pwszString = NULL;
        dstrAttrValue.cchString = 0;
    }

    ChkDR( DRM_XML_GetSubNodeByPath(
                &g_dstrXML,
                &dstrNodePath,
                &dstrAttrName,
                &dstrAttrValue,
                NULL,
                NULL,
                g_wchForwardSlash ) );

ErrorExit:
      return dr;
}



/***************************************************************************
**
** Function: TestXMLPUtilGetSubNodeByPathA
**
** Synopsis: Function tests
** test strings and faulty input data.
**
** Arguments:
**
** [argc]     -- Total number of arguments.
** [argv]     -- Pointer to a array of arguments.
**
** argv[0]    -- XML data string. Contains real XML string, may be set into
**               a special value "nullstring" to imitate zero-length XML.
** argv[1]    -- Node path string, may be set into "nullstring"
** argv[2]    -- Attribute name string, may be set into "nullstring"
** argv[3]    -- Attribute value string, may be set into "nullstring"
** argv[4]    -- String for verification purpose, optional. Use it to
**               compare with a node returned.
**
** Returns:      DRM_RESULT
**
***************************************************************************/

DRM_RESULT DRM_CALL TestXMLPUtilGetSubNodeByPathA(
                                                long argc,
                              __in_ecount(argc) char **argv )
{
    DRM_RESULT dr                  = DRM_SUCCESS;
    DRM_CHAR   szXMLData[256]      = { 0 };
    DRM_CHAR   szNodePath[256]     = { 0 };
    DRM_CHAR   szAttrName[256]     = { 0 };
    DRM_CHAR   szAttrValue[256]    = { 0 };
    DRM_CHAR   szVerification[256] = { 0 };
    DRM_ANSI_CONST_STRING dastrNodePath  = { 0, 0 };
    DRM_ANSI_CONST_STRING dastrAttrName  = { 0, 0 };
    DRM_ANSI_CONST_STRING dastrAttrValue = { 0, 0 };
    DRM_SUBSTRING         dasstr         = EMPTY_DRM_SUBSTRING;

    /*
    ** at least 4 arguments.
    */
    if( argc < 4 )
    {
        Log( "Trace", "\t\tFrom TestXMLPUtilGetXMLNode: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    TestXMLPUtilReinitialize( 1, NULL );

    if( strcmp( argv[0], "nullstring" ) )
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[0] );
        g_dastrXML.cchString = ( DRM_DWORD ) min( SIZEOF( szXMLData ) - 1, cb );

        MEMCPY( szXMLData, argv[0], g_dastrXML.cchString );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
        szXMLData[ g_dastrXML.cchString ] = '\0';
#pragma prefast(pop)
        g_dastrXML.pszString = szXMLData;

        dasstr.m_ich = 0;
        dasstr.m_cch = g_dastrXML.cchString;
    }
    else
    {
        g_dastrXML.pszString = NULL;
        g_dastrXML.cchString = 0;

        dasstr.m_ich = 0;
        dasstr.m_cch = 0;
    }

    if( strcmp( argv[1], "nullstring" ) )
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[1] );
        dastrNodePath.cchString = ( DRM_DWORD ) min( SIZEOF( szNodePath ) - 1, cb );
        MEMCPY( szNodePath, argv[1], dastrNodePath.cchString );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
        szNodePath[ dastrNodePath.cchString ] = '\0';
#pragma prefast(pop)
        dastrNodePath.pszString = szNodePath;
    }
    else
    {
        dastrNodePath.pszString = NULL;
        dastrNodePath.cchString = 0;
    }

    if( strcmp( argv[2], "nullstring" ) )
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[2] );
        dastrAttrName.cchString = ( DRM_DWORD ) min( SIZEOF( szAttrName ) - 1, cb );
        MEMCPY( szAttrName, argv[2], dastrAttrName.cchString );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
        szAttrName[ dastrAttrName.cchString ] = '\0';
#pragma prefast(pop)
        dastrAttrName.pszString = szAttrName;
    }
    else
    {
        dastrAttrName.pszString = NULL;
        dastrAttrName.cchString = 0;
    }

    if( strcmp( argv[3], "nullstring" ) )
    {
        DRM_SIZE_T cb = DRMCRT_strlen( argv[3] );
        dastrAttrValue.cchString = ( DRM_DWORD ) min( SIZEOF( szAttrValue ) - 1, cb );
        MEMCPY( szAttrValue, argv[3], dastrAttrValue.cchString );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
        szAttrValue[ dastrAttrValue.cchString ] = '\0';
#pragma prefast(pop)
        dastrAttrValue.pszString = szAttrValue;
    }
    else
    {
        dastrAttrValue.pszString = NULL;
        dastrAttrValue.cchString = 0;
    }

    ChkDR( DRM_XML_GetSubNodeByPathA(
                g_dastrXML.pszString,
                &dasstr,
                &dastrNodePath,
                &dastrAttrName,
                &dastrAttrValue,
                &g_dasstrNode,
                NULL,
                '/' ) );

    /*
    ** verify the result
    */
    if ( argc > 4 && argv[4] != NULL )
    {
        DRM_SIZE_T cb            = DRMCRT_strlen( argv[4] );
        DRM_DWORD  cVerification = ( DRM_DWORD ) min( SIZEOF( szVerification ) - 1, cb );

        MEMCPY( szVerification, argv[4], cVerification );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
        szVerification[ cVerification ] = '\0';
#pragma prefast(pop)
        if( DRMCRT_strncmp( szVerification,
                            g_dastrXML.pszString + g_dasstrNode.m_ich,
                            g_dasstrNode.m_cch ) )
        {
            ChkDR( DRM_E_FAIL );
        }
    }

ErrorExit:
      return dr;
}


/* Get the default implementations for WrapTestRun, PreTestCase and PostTestCase */
IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE( XMLParser )

BEGIN_TEST_API_LIST( XMLParser )
    API_ENTRY(TestXMLPUtilGetXMLNode)
    API_ENTRY(TestXMLPUtilGetXMLSubNode)
    API_ENTRY(TestXMLPUtilGetXMLNodeAttribute)
    API_ENTRY(TestXMLPUtilGetXMLNodeCData)
    API_ENTRY(TestXMLPUtilGetXMLNodeCDataA)
    API_ENTRY(TestXMLPUtilGetXMLNodeA)
    API_ENTRY(TestXMLPUtilGetXMLSubNodeA)
    API_ENTRY(TestXMLPUtilGetXMLNodeAttributeA)
    API_ENTRY(TestXMLPUtilGetSubNodeByPath)
    API_ENTRY(TestXMLPUtilGetSubNodeByPathA)
    API_ENTRY(TestXMLPUtilEnumNextSubNode)
    API_ENTRY(TestXMLPUtilReinitialize)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

