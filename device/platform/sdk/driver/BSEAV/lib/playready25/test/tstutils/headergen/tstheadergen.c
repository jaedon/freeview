/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmcommon.h>
#include <tgeneratorutils.h>
#include <HeaderGen.h>
#include <tRightsParser.h>
#include <drmxmlparser.h>
#include <drmplayreadyobj.h>
#include <drmembeddedstore_impl.h>

/*******************************************************************************
** Example of header pool data file:
**
** <HeadersPool>
**   <WRMHeader
**     TestCaseID="Name of Test Sample"
**     Version="4.1"
**     LAURL=""
**     LUIURL=""
**     DSID=""
**     DecryptorSetup="ONDEMAND" >
**     <PROTECTINFO number_of_entries="1">
**       <PROTECTINFO_entry KID="" ALGID="AESCTR" CHECKSUM="" />
**     </PROTECTINFO>
**     <CustomAttributes xlmns="">
**     </CustomAttributes>
**   </WRMHeader>
**   <WRMHeader
**     ...>
**   ...
**   </WRMHeader>
**   ...
** </HeadersPool>
**
** Format description:
** Version - can be "4.1", "4.0" and "*" for DRM_HEADER_VERSION_UNKNOWN. Required.
** LAURL   - some string, optional parameter
** LUIURL  - some string, optional parameter
** DSID    - some string, optional parameter
** DecryptorSetup - "ONDEMAND" optional, default is to not set DecryptorSetup
** ContentEntries - lists content entries. There must be one and only one entry
**                  if version is less than 4.1 and none, one or more for 4.1.
** KID            - either a value or "*" for random generated KID
** ALGID          - AESCTR or COCKTAIL
** CHECKSUM       - value or omitted
** CustomAttributes - optional
**
**
** All parameters can be overriden via TestWRMHeaderOverrides struct
*******************************************************************************/
ENTER_PKTEST_NAMESPACE_CODE

/*HeadersPool*/
static const DRM_WCHAR dwchHeadersPool[]                    = { WCHAR_CAST('H'), WCHAR_CAST('e'), WCHAR_CAST('a'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('P'), WCHAR_CAST('o'), WCHAR_CAST('o'), WCHAR_CAST('l'), WCHAR_CAST('\0') };
/*WRMHeader*/
static const DRM_WCHAR dwchWRMHeader[]                      = { WCHAR_CAST('W'), WCHAR_CAST('R'), WCHAR_CAST('M'), WCHAR_CAST('H'), WCHAR_CAST('e'), WCHAR_CAST('a'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('\0') };
/*TestCaseID*/
static const DRM_WCHAR dwchTestCastID[]                     = { WCHAR_CAST('T'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('t'), WCHAR_CAST('C'), WCHAR_CAST('a'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*Version*/
static const DRM_WCHAR dwchVersion[]                        = { WCHAR_CAST('V'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('s'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*4.0*/
static const DRM_WCHAR dwchVersion4[]                       = { WCHAR_CAST('4'), WCHAR_CAST('.'), WCHAR_CAST('0'), WCHAR_CAST('\0') };
/*4.1*/
static const DRM_WCHAR dwchVersion4_1[]                     = { WCHAR_CAST('4'), WCHAR_CAST('.'), WCHAR_CAST('1'), WCHAR_CAST('\0') };
/*Unknown*/
static const DRM_WCHAR dwchVersionUnknown[]                 = { WCHAR_CAST('U'), WCHAR_CAST('n'), WCHAR_CAST('k'), WCHAR_CAST('n'), WCHAR_CAST('o'), WCHAR_CAST('w'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*LA_URL*/
static const DRM_WCHAR dwchLA_URL[]                         = { WCHAR_CAST('L'), WCHAR_CAST('A'), WCHAR_CAST('_'), WCHAR_CAST('U'), WCHAR_CAST('R'), WCHAR_CAST('L'), WCHAR_CAST('\0') };
/*LUI_URL*/
static const DRM_WCHAR dwchLUI_URL[]                        = { WCHAR_CAST('L'), WCHAR_CAST('U'), WCHAR_CAST('I'), WCHAR_CAST('_'), WCHAR_CAST('U'), WCHAR_CAST('R'), WCHAR_CAST('L'), WCHAR_CAST('\0') };
/*DS_ID*/
static const DRM_WCHAR dwchDS_ID[]                          = { WCHAR_CAST('D'), WCHAR_CAST('S'), WCHAR_CAST('_'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*DECRYPTORSETUP*/
static const DRM_WCHAR dwchDectyptorSetup[]                 = { WCHAR_CAST('D'), WCHAR_CAST('E'), WCHAR_CAST('C'), WCHAR_CAST('R'), WCHAR_CAST('Y'), WCHAR_CAST('P'), WCHAR_CAST('T'), WCHAR_CAST('O'), WCHAR_CAST('R'), WCHAR_CAST('S'), WCHAR_CAST('E'), WCHAR_CAST('T'), WCHAR_CAST('U'), WCHAR_CAST('P'), WCHAR_CAST('\0') };
/*CUSTOMATTRIBUTES*/
static const DRM_WCHAR dwchCUSTOMATTRIBUTES[]               = { WCHAR_CAST('C'), WCHAR_CAST('U'), WCHAR_CAST('S'), WCHAR_CAST('T'), WCHAR_CAST('O'), WCHAR_CAST('M'), WCHAR_CAST('A'), WCHAR_CAST('T'), WCHAR_CAST('T'), WCHAR_CAST('R'), WCHAR_CAST('I'), WCHAR_CAST('B'), WCHAR_CAST('U'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('S'), WCHAR_CAST('\0') };
/*PROTECTINFO*/
static const DRM_WCHAR dwchPROTECTINFO[]                    = { WCHAR_CAST('P'), WCHAR_CAST('R'), WCHAR_CAST('O'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('C'), WCHAR_CAST('T'), WCHAR_CAST('I'), WCHAR_CAST('N'), WCHAR_CAST('F'), WCHAR_CAST('O'), WCHAR_CAST('\0') };
/*PROTECTINFO_entry*/
static const DRM_WCHAR dwchPROTECTINFO_entry[]              = { WCHAR_CAST('P'), WCHAR_CAST('R'), WCHAR_CAST('O'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('C'), WCHAR_CAST('T'), WCHAR_CAST('I'), WCHAR_CAST('N'), WCHAR_CAST('F'), WCHAR_CAST('O'), WCHAR_CAST('_'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*number_of_entries*/
static const DRM_WCHAR dwchAttribNumberOfEntries[]          = { WCHAR_CAST('n'), WCHAR_CAST('u'), WCHAR_CAST('m'), WCHAR_CAST('b'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('_'), WCHAR_CAST('o'), WCHAR_CAST('f'), WCHAR_CAST('_'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*KID*/
static const DRM_WCHAR dwchKID[]                            = { WCHAR_CAST('K'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*ALGID*/
static const DRM_WCHAR dwchALGID[]                          = { WCHAR_CAST('A'), WCHAR_CAST('L'), WCHAR_CAST('G'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*AESCTR*/
static const DRM_WCHAR dwchALGIDAESCTR[]                    = { WCHAR_CAST('A'), WCHAR_CAST('E'), WCHAR_CAST('S'), WCHAR_CAST('C'), WCHAR_CAST('T'), WCHAR_CAST('R'), WCHAR_CAST('\0') };
/*RC4*/
static const DRM_WCHAR dwchALGIDRC4[]                       = { WCHAR_CAST('R'), WCHAR_CAST('C'), WCHAR_CAST('4'), WCHAR_CAST('\0') };
/*CONTENTKEY*/
static const DRM_WCHAR dwchCONTENTKEY[]                     = { WCHAR_CAST('C'), WCHAR_CAST('O'), WCHAR_CAST('N'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('N'), WCHAR_CAST('T'), WCHAR_CAST('K'), WCHAR_CAST('E'), WCHAR_CAST('Y'), WCHAR_CAST('\0') };

static const DRM_CONST_STRING g_dstrHeadersPool            = CREATE_DRM_STRING(dwchHeadersPool);
static const DRM_CONST_STRING g_dstrWRMHeader              = CREATE_DRM_STRING(dwchWRMHeader);
static const DRM_CONST_STRING g_dstrTestCaseID             = CREATE_DRM_STRING(dwchTestCastID);

//const DRM_CONST_STRING g_dstrVersion                       = CREATE_DRM_STRING(dwchVersion);
#ifndef __cplusplus
extern DRM_CONST_STRING g_dstrVersion ;  
#else
const DRM_CONST_STRING g_dstrVersion                       = CREATE_DRM_STRING(dwchVersion);
#endif

//extern DRM_CONST_STRING g_dstrVersion ;  
const DRM_CONST_STRING g_dstrVersion4                      = CREATE_DRM_STRING(dwchVersion4);
const DRM_CONST_STRING g_dstrVersion4_1                    = CREATE_DRM_STRING(dwchVersion4_1);
const DRM_CONST_STRING g_dstrVersionUnknown                = CREATE_DRM_STRING(dwchVersionUnknown);

static const DRM_CONST_STRING g_dstrLA_URL                 = CREATE_DRM_STRING(dwchLA_URL);
static const DRM_CONST_STRING g_dstrLUI_URL                = CREATE_DRM_STRING(dwchLUI_URL);
static const DRM_CONST_STRING g_dstrDS_ID                  = CREATE_DRM_STRING(dwchDS_ID);
static const DRM_CONST_STRING g_dstrDecryptorSetup         = CREATE_DRM_STRING(dwchDectyptorSetup);
static const DRM_CONST_STRING g_dstrCUSTOMATTRIBUTES       = CREATE_DRM_STRING(dwchCUSTOMATTRIBUTES);

static const DRM_CONST_STRING g_dstrPROTECTINFO            = CREATE_DRM_STRING(dwchPROTECTINFO);
static const DRM_CONST_STRING g_dstrPROTECTINFO_entry      = CREATE_DRM_STRING(dwchPROTECTINFO_entry);
static const DRM_CONST_STRING g_dstrAttribNumberOfEntries  = CREATE_DRM_STRING(dwchAttribNumberOfEntries);
static const DRM_CONST_STRING g_dstrKID                    = CREATE_DRM_STRING(dwchKID);
static const DRM_CONST_STRING g_dstrALGID                  = CREATE_DRM_STRING(dwchALGID);
const DRM_CONST_STRING g_dstrALGIDAESCTR                   = CREATE_DRM_STRING(dwchALGIDAESCTR);
const DRM_CONST_STRING g_dstrALGIDRC4                      = CREATE_DRM_STRING(dwchALGIDRC4);
static const DRM_CONST_STRING g_dstrCONTENTKEY             = CREATE_DRM_STRING(dwchCONTENTKEY);


static const DRM_BYTE s_PRONoWrmHeader[] = {0x0A, 0x28, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x00, 0x28 };


static DRM_VOID _FreeWRMHeaderParams( __in TestWRMHeaderParams *f_pParams );


static DRM_RESULT _TestModifyHeaderParams( __in           TestWRMHeaderParams    *f_pParams,
                                           __in_opt const TestWRMHeaderOverrides *f_pOverrideParams )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pParams != NULL );

    if ( f_pOverrideParams == NULL )
    {
        goto ErrorExit;
    }

    if ( f_pOverrideParams->bOverrideSet )
    {
        if ( DRM_UTL_DSTRStringsEqual( &f_pOverrideParams->dstrWRMHeaderVersion, &g_dstrVersion4 ) )
        {
            f_pParams->eWRMHeaderVersion = DRM_HEADER_VERSION_4;
        }
        else if ( DRM_UTL_DSTRStringsEqual( &f_pOverrideParams->dstrWRMHeaderVersion, &g_dstrVersion4_1 ) )
        {
            f_pParams->eWRMHeaderVersion = DRM_HEADER_VERSION_4_1;
        }
        else if ( DRM_UTL_DSTRStringsEqual( &f_pOverrideParams->dstrWRMHeaderVersion, &g_dstrVersionUnknown ) )
        {
            f_pParams->eWRMHeaderVersion = DRM_HEADER_VERSION_UNKNOWN;
        }

        if ( f_pOverrideParams->dstrPlayReadySilentURL.cchString > 0  )
        {
            FREEDRMSTRING( f_pParams->dstrPlayReadySilentURL );
            ChkDR( TST_UTL_CopyDRMString( &f_pParams->dstrPlayReadySilentURL, &f_pOverrideParams->dstrPlayReadySilentURL ) );
        }

        if ( f_pOverrideParams->dstrKID.cchString > 0
          || f_pOverrideParams->dstrContentKey.cchString > 0
          || f_pOverrideParams->dstrCipherType.cchString > 0 ) /* need to create/update PROTECTINFO entry */
        {
            TestWRMHeaderProtectInfoParams *pModifiedEntry = NULL;

            /*
            ** Can either add a new entry to the empty list
            ** or override data in the first entry of the non-empty list
            */
            if ( f_pParams->pProtectInfoList == NULL )
            {
                ChkMem( pModifiedEntry = (TestWRMHeaderProtectInfoParams*)Oem_MemAlloc( SIZEOF( TestWRMHeaderProtectInfoParams ) ) );
                ZEROMEM(pModifiedEntry,SIZEOF( TestWRMHeaderProtectInfoParams ));

                f_pParams->pProtectInfoList = pModifiedEntry;

                pModifiedEntry->pNext = NULL;
            }
            else
            {
                pModifiedEntry = f_pParams->pProtectInfoList;
            }

            if ( f_pOverrideParams->dstrKID.pwszString[0] == L'*' ) /* override KID with random value */
            {
                DRM_CONST_STRING dstrRandomKID = EMPTY_DRM_STRING;

                ChkDR( TST_UTL_MakeRandomKID( &dstrRandomKID ) );
                FREEDRMSTRING( pModifiedEntry->dstrKID );
                pModifiedEntry->dstrKID.pwszString = dstrRandomKID.pwszString;
                pModifiedEntry->dstrKID.cchString = dstrRandomKID.cchString;
            }
            else if ( f_pOverrideParams->dstrKID.cchString > 0 )
            {
                FREEDRMSTRING( pModifiedEntry->dstrKID );
                ChkDR( TST_UTL_CopyDRMString( &pModifiedEntry->dstrKID, &f_pOverrideParams->dstrKID ) );
            }
            if ( f_pOverrideParams->dstrContentKey.cchString > 0 )
            {
                FREEDRMSTRING( pModifiedEntry->dstrContentKey );
                ChkDR( TST_UTL_CopyDRMString( &pModifiedEntry->dstrContentKey, &f_pOverrideParams->dstrContentKey ) );
            }
            if ( f_pOverrideParams->dstrCipherType.cchString > 0 )
            {
                if ( DRM_UTL_DSTRStringsEqual( &f_pOverrideParams->dstrCipherType, &g_dstrALGIDAESCTR ) )
                {
                     pModifiedEntry->eCipherType = eDRM_AES_COUNTER_CIPHER;
                }
                else if( DRM_UTL_DSTRStringsEqual( &f_pOverrideParams->dstrCipherType, &g_dstrALGIDRC4 ) )
                {
                     pModifiedEntry->eCipherType = eDRM_RC4_CIPHER;
                }
            }
        }
    }

ErrorExit:
    return dr;
}


static DRM_RESULT _TestGetHeaderParamsFromPool( __in  const DRM_CONST_STRING *f_pdstrPool,
                                                __in  const DRM_CONST_STRING *f_pdstrId,
                                                __out TestWRMHeaderParams   **f_ppParams )
{
    DRM_RESULT                       dr                     = DRM_SUCCESS;
    TestWRMHeaderParams             *pParams                = NULL;
    DRM_CONST_STRING                 dstrPoolXML            = EMPTY_DRM_STRING;
    DRM_CONST_STRING                 dstrHeaderNode         = EMPTY_DRM_STRING;
    DRM_CONST_STRING                 dstrHeaderNodeData     = EMPTY_DRM_STRING;
    DRM_CONST_STRING                 dstrProtectInfoNode    = EMPTY_DRM_STRING;
    DRM_CONST_STRING                 dstrValue              = EMPTY_DRM_STRING;
    DRM_CONST_STRING                 dstrProtectInfoEntry   = EMPTY_DRM_STRING;
    DRM_DWORD                        dwEntriesCount         = 0;
    DRM_DWORD                        dwCount                = 0;
    TestWRMHeaderProtectInfoParams  *pCurrentEntry          = NULL;
    DRM_CONST_STRING                 dstrKID                = EMPTY_DRM_STRING;
    DRM_CONST_STRING                 dstrContentKey         = EMPTY_DRM_STRING;

    ChkArg( f_pdstrPool != NULL );
    ChkArg( f_pdstrId   != NULL );
    ChkArg( f_ppParams  != NULL );

    *f_ppParams = NULL;

    ChkMem( pParams = (TestWRMHeaderParams*)Oem_MemAlloc( SIZEOF( TestWRMHeaderParams ) ) );
    ZEROMEM( pParams,SIZEOF( TestWRMHeaderParams ) );

    ChkDR( DRM_XML_GetNode( f_pdstrPool, &g_dstrHeadersPool, NULL, NULL, 0, NULL, &dstrPoolXML ) );

    /*
    ** Retrieve specific <WRMHeader> subnode from <HeadersPool>
    */
    ChkDR( DRM_XML_GetNode( &dstrPoolXML,
                            &g_dstrWRMHeader,
                            &g_dstrTestCaseID,
                             f_pdstrId,
                             0,
                            &dstrHeaderNode,
                            &dstrHeaderNodeData ) );

    if ( DRM_SUCCEEDED ( DRM_XML_GetNodeAttribute( &dstrHeaderNode, &g_dstrVersion, &dstrValue ) ) )
    {
        if ( DRM_UTL_DSTRStringsEqual( &dstrValue, &g_dstrVersion4 ) )
        {
            pParams->eWRMHeaderVersion = DRM_HEADER_VERSION_4;
        }
        else if ( DRM_UTL_DSTRStringsEqual( &dstrValue, &g_dstrVersion4_1 ) )
        {
            pParams->eWRMHeaderVersion = DRM_HEADER_VERSION_4_1;
        }
        else if ( DRM_UTL_DSTRStringsEqual( &dstrValue, &g_dstrVersionUnknown ) )
        {
            pParams->eWRMHeaderVersion = DRM_HEADER_VERSION_UNKNOWN;
        }
    }
    if ( DRM_SUCCEEDED( DRM_XML_GetNodeAttribute( &dstrHeaderNode, &g_dstrLA_URL, &dstrValue ) ) )
    {
        ChkDR( TST_UTL_CopyDRMString( &pParams->dstrPlayReadySilentURL, &dstrValue ) );
    }
    if ( DRM_SUCCEEDED( DRM_XML_GetNodeAttribute( &dstrHeaderNode, &g_dstrLUI_URL, &dstrValue ) ) )
    {
        ChkDR( TST_UTL_CopyDRMString( &pParams->dstrPlayReadyNonSilentURL, &dstrValue ) );
    }
    if ( DRM_SUCCEEDED( DRM_XML_GetNodeAttribute( &dstrHeaderNode, &g_dstrDS_ID, &dstrValue ) ) )
    {
        ChkDR( TST_UTL_CopyDRMString( &pParams->dstrServiceID, &dstrValue ) );
    }
    if ( DRM_SUCCEEDED( DRM_XML_GetNodeAttribute( &dstrHeaderNode, &g_dstrDecryptorSetup, &dstrValue ) ) )
    {
        ChkDR( TST_UTL_CopyDRMString( &pParams->dstrDecryptorSetup, &dstrValue ) );
    }
    if ( DRM_SUCCEEDED( DRM_XML_GetNode( &dstrHeaderNode, &g_dstrCUSTOMATTRIBUTES, NULL, NULL, 0, NULL, &dstrValue ) ) )
    {
        ChkDR( TST_UTL_CopyDRMString( &pParams->dstrCustomAttriburesXML, &dstrValue ) );
    }

    /*
    ** Read inside PROTECTINFO if it is present
    */
    if ( DRM_SUCCEEDED( DRM_XML_GetNode( &dstrHeaderNodeData,
                                         &g_dstrPROTECTINFO,
                                          NULL,
                                          NULL,
                                          0,
                                         &dstrProtectInfoNode,
                                          NULL ) ) )
    {
        ChkDR( DRM_XML_GetNodeAttribute( &dstrProtectInfoNode, &g_dstrAttribNumberOfEntries, &dstrValue ) );

        ChkDR( DRMCRT_WtoDWORD(  dstrValue.pwszString, dstrValue.cchString, 10, &dwEntriesCount, NULL ) );

        /*
        ** Note: current implementation of WRMHeader allows only one PROTECTINFO entry is supported.
        */
        for( dwCount = 0, pParams->pProtectInfoList = NULL; dwCount < dwEntriesCount; dwCount++ )
        {
            DRM_BOOL                        fReadEntry     = FALSE;
            DRM_CONST_STRING                dstrAlgID      = EMPTY_DRM_STRING;
            TestWRMHeaderProtectInfoParams *pEnd           = NULL;

            ChkDR( DRM_XML_GetSubNode( &dstrProtectInfoNode,
                                       &g_dstrPROTECTINFO_entry,
                                        NULL,
                                        NULL,
                                        dwCount,
                                       &dstrProtectInfoEntry,
                                        NULL,
                                        1 ) );

            if ( DRM_SUCCEEDED( DRM_XML_GetNodeAttribute( &dstrProtectInfoEntry, &g_dstrKID, &dstrKID ) ) )
            {
                fReadEntry = TRUE;
            }
            if ( DRM_SUCCEEDED( DRM_XML_GetNodeAttribute( &dstrProtectInfoEntry, &g_dstrCONTENTKEY, &dstrContentKey ) ) )
            {
                fReadEntry = TRUE;
            }
            if ( DRM_SUCCEEDED( DRM_XML_GetNodeAttribute( &dstrProtectInfoEntry, &g_dstrALGID, &dstrAlgID ) ) )
            {
                if ( DRM_UTL_DSTRStringsEqual( &dstrAlgID, &g_dstrALGIDAESCTR )
                  || DRM_UTL_DSTRStringsEqual( &dstrAlgID, &g_dstrALGIDRC4 ) )
                {
                    fReadEntry = TRUE;
                }
                else
                {
                    ChkDR( DRM_E_TEST_INVALIDARG );
                }
            }
            if ( fReadEntry )
            {
                /*
                ** Allocate a new entry
                */
                ChkMem( pCurrentEntry = (TestWRMHeaderProtectInfoParams*)Oem_MemAlloc( SIZEOF( TestWRMHeaderProtectInfoParams ) ) );
                ZEROMEM( pCurrentEntry, SIZEOF( TestWRMHeaderProtectInfoParams ) );

                pCurrentEntry->pNext = NULL;

                if ( pEnd )
                {
                    pEnd->pNext = pCurrentEntry; /* connect new element with the previous one */
                }
                else
                {
                    pParams->pProtectInfoList = pCurrentEntry; /* the first list element is a head pointer */
                }
                pEnd = pCurrentEntry; /* remember the end of the list */

                /*
                ** Fill with data
                */
                ChkDR( TST_UTL_CopyDRMString( &pCurrentEntry->dstrKID, &dstrKID ) );
                ChkDR( TST_UTL_CopyDRMString( &pCurrentEntry->dstrContentKey, &dstrContentKey ) );
                if ( DRM_UTL_DSTRStringsEqual( &dstrAlgID, &g_dstrALGIDAESCTR ) )
                {
                    pCurrentEntry->eCipherType = eDRM_AES_COUNTER_CIPHER;
                }
                else if( DRM_UTL_DSTRStringsEqual( &dstrAlgID, &g_dstrALGIDRC4 ) )
                {
                    pCurrentEntry->eCipherType = eDRM_RC4_CIPHER;
                }
            }
            else
            {
                ChkDR( DRM_E_TEST_INVALIDARG ); /* something wrong with pool data */
            }

        }
    }

    *f_ppParams = pParams;

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        _FreeWRMHeaderParams( pParams );
    }
    return dr;
}


static DRM_VOID _FreeWRMHeaderParams( __in TestWRMHeaderParams *f_pParams )
{
    TestWRMHeaderProtectInfoParams *pCurrNode = NULL;

    if ( !f_pParams )
    {
        return;
    }

    FREEDRMSTRING( f_pParams->dstrPlayReadySilentURL );
    FREEDRMSTRING( f_pParams->dstrPlayReadyNonSilentURL );
    FREEDRMSTRING( f_pParams->dstrServiceID );
    FREEDRMSTRING( f_pParams->dstrCustomAttriburesXML );
    FREEDRMSTRING( f_pParams->dstrDecryptorSetup );

    if ( f_pParams->pProtectInfoList )
    {
        pCurrNode = f_pParams->pProtectInfoList;

        while ( pCurrNode != NULL )
        {
            TestWRMHeaderProtectInfoParams *pNextNode = pCurrNode->pNext;

            FREEDRMSTRING( pCurrNode->dstrKID );
            FREEDRMSTRING( pCurrNode->dstrContentKey );
            SAFE_OEM_FREE( pCurrNode );

            pCurrNode = pNextNode;
        }
    }
    Oem_MemFree( f_pParams );
}


/*********************************************************************************************
** Function:  TST_UTL_TestGeneratePRO
**
** Synopsis:  Function creates test PRO sample using WRMHeader if it is provided
**            and adding PRO data records for various testcases.
**
** Arguments: f_pbWRMHeader   - buffer containing WRMHeader data, optional
**            f_cbWRMHeader   - size of f_pbWRMHeader
**            f_dwTestParams  - instructs to create PRO with no WRMHeader (there maybe more params in future).
**            f_ppbPRO        - points to output buffer containing PRO. Caller must free this memory.
**            f_pcbPRO        - size of *f_ppbPRO. It is either NULL or specified by the caller in which case
**                              we try to use it. Note: this does not work for special test cases.
**
** Returns:     DRM_SUCCESS     - Successfully embedded a license.
**
*********************************************************************************************/
DRM_API DRM_RESULT DRM_CALL TST_UTL_TestGeneratePRO( __in_bcount_opt(f_cbWRMHeader) const DRM_BYTE  *f_pbWRMHeader,
                                                    __in                            const DRM_DWORD  f_cbWRMHeader,
                                                    __in                            DRM_DWORD        f_dwTestParams,
                                                    __deref_out_bcount(*f_pcbPRO)   DRM_BYTE       **f_ppbPRO,
                                                    __inout                         DRM_DWORD       *f_pcbPRO )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CONST_STRING dstrHeader = EMPTY_DRM_STRING;
    DRM_DWORD        dwPROSize  = 0;

    ChkArg( f_ppbPRO != NULL );
    ChkArg( f_pcbPRO != NULL );

    if( f_dwTestParams == TEST_PRO_NoSpecialTestcases ) /* no special cases, create PRO with WRMHeader in it */
    {
        ChkArg( f_pbWRMHeader != NULL && f_cbWRMHeader > 0 );

        dstrHeader.pwszString = (const DRM_WCHAR*)f_pbWRMHeader;
        dstrHeader.cchString = f_cbWRMHeader / SIZEOF(DRM_WCHAR);


        if ( *f_pcbPRO > 0 ) /* size of PRO is specified by the caller of this API */
        {
            dwPROSize = *f_pcbPRO;
        }
        else
        {
            dr = DRM_PRO_Create( &dstrHeader, NULL, &dwPROSize, MAX_EMBEDDED_STORE_LEN );
        }

        if ( dr != DRM_E_BUFFERTOOSMALL )
        {
            ChkDR( dr );
        }

        ChkMem( *f_ppbPRO = (DRM_BYTE*) Oem_MemAlloc( dwPROSize ) );

        ChkDR( DRM_PRO_Create( &dstrHeader, *f_ppbPRO, &dwPROSize, MAX_EMBEDDED_STORE_LEN ) );

        *f_pcbPRO = dwPROSize;

    }
    else
    {
        /*
        ** no WRMHeader record at all MAX_EMBEDDED_STORE_LEN
        */
        *f_pcbPRO = SIZEOF(s_PRONoWrmHeader) + MAX_EMBEDDED_STORE_LEN;
        ChkMem( *f_ppbPRO = (DRM_BYTE*) Oem_MemAlloc( *f_pcbPRO ) );
        ZEROMEM( *f_ppbPRO, *f_pcbPRO );
        DRM_BYT_CopyBytes( *f_ppbPRO, 0, s_PRONoWrmHeader, 0, SIZEOF(s_PRONoWrmHeader) );
    }

ErrorExit:
    return dr;
}


/*********************************************************************************************
** Function:  TST_UTL_TestGenerateWRMHeader
**
** Synopsis:  Function creates WRMHeader out of XML configuration read from the pool of test data.
**
** Arguments: f_pdstrPool       - content of header test data pool file, should be valid XML.
**                                Not to be confused with test data file name!
**            f_pdstrId         - header test data ID, value of TestCaseID attribute.
**            f_pbHeader        - output byte buffer containing WRMHeader, optional.
**                                If NULL then function returns DRM_E_BUFFERTOOSMALL and
**                                f_pcbHeader is a number of bytes needed to hold the header.
**            f_pcbHeader       - size of f_pbHeader
**            f_pOverrideParams - Structure containing optional overrides of test data. Optional.
**
** Returns:     DRM_SUCCESS     - Successfully embedded a license.
**
*********************************************************************************************/
DRM_API DRM_RESULT DRM_CALL TST_UTL_TestGenerateWRMHeader( __in                             const DRM_CONST_STRING       *f_pdstrPool,
                                                           __in                             const DRM_CONST_STRING       *f_pdstrId,
                                                           __inout_bcount_opt(*f_pcbHeader)       DRM_BYTE               *f_pbHeader,
                                                           __out                                  DRM_DWORD              *f_pcbHeader,
                                                           __in_opt                         const TestWRMHeaderOverrides *f_pOverrideParams )
{
    DRM_RESULT            dr                                                          = DRM_SUCCESS;
    TestWRMHeaderParams  *pParams                                                     = NULL;
    DRM_CONST_STRING      dstrKID                                                     = EMPTY_DRM_STRING;
    DRM_SUPPORTED_CIPHERS eCipherType                                                 = eDRM_CIPHER_INVALID;
    DRM_CONST_STRING      dstrChecksum                                                = EMPTY_DRM_STRING;
    DRM_BYTE              rgbContentKey[ DRMCIPHERKEYLEN_MAX ]                        = {0};
    DRM_WCHAR             rgwchChecksum[CCH_BASE64_EQUIV(DRM_AES_CK_CHECKSUM_LENGTH)] = {0};
    DRM_DWORD             cchChecksumStr                                              = 0;
    DRM_KID               oKID                                                        = {0};
    DRM_DWORD             cbKID                                                       = SIZEOF( oKID );
    DRM_DWORD             cbContentKey                                                = SIZEOF( rgbContentKey );
    DRM_BYTE             *pbChecksum                                                  = NULL;
    DRM_DWORD             cbChecksum                                                  = 0;

    ChkArg( f_pcbHeader != NULL );

    /*
    ** Read params from data pool
    */
    ChkDR( _TestGetHeaderParamsFromPool( f_pdstrPool, f_pdstrId, &pParams ) );
    ChkArg( pParams != NULL );

    /*
    ** Override params if needed
    */
    ChkDR( _TestModifyHeaderParams( pParams, f_pOverrideParams ) );

    if ( pParams->pProtectInfoList != NULL )
    {
        dstrKID.pwszString = pParams->pProtectInfoList->dstrKID.pwszString;
        dstrKID.cchString = pParams->pProtectInfoList->dstrKID.cchString;

        eCipherType = pParams->pProtectInfoList->eCipherType;

        /*
        ** checksum - if content key, cipher type and KID are provided
        */
        if ( pParams->pProtectInfoList->dstrKID.cchString > 0
          && pParams->pProtectInfoList->dstrContentKey.cchString > 0
          && eCipherType != eDRM_CIPHER_INVALID )
        {
            ChkDR( DRM_B64_DecodeW( &pParams->pProtectInfoList->dstrContentKey, &cbContentKey, rgbContentKey, 0 ) );

            ChkDR( DRM_B64_DecodeW( &pParams->pProtectInfoList->dstrKID, &cbKID, oKID.rgb, 0 ) );

            dr = DRM_PRO_CalculateRMHeaderChecksum( eCipherType,
                                                    rgbContentKey,
                                                    cbContentKey,
                                                   &oKID,
                                                    NULL,
                                                   &cbChecksum );

            ChkBOOL( DRM_E_BUFFERTOOSMALL == dr, dr );

            ChkMem( pbChecksum = (DRM_BYTE *) Oem_MemAlloc( cbChecksum ) );
            ZEROMEM( pbChecksum, cbChecksum );

            ChkDR( DRM_PRO_CalculateRMHeaderChecksum( eCipherType,
                                                      rgbContentKey,
                                                      cbContentKey,
                                                     &oKID,
                                                      pbChecksum,
                                                     &cbChecksum ) );

            cchChecksumStr = NO_OF( rgwchChecksum );
            ChkDR( DRM_B64_EncodeW( pbChecksum,
                                    cbChecksum,
                                    rgwchChecksum,
                                   &cchChecksumStr,
                                    0 ) );
            dstrChecksum.pwszString = rgwchChecksum;
            dstrChecksum.cchString = cchChecksumStr;
        }
    }


    /*
    ** If f_pbHeader is NULL then DRM_PRO_CreateRMHeader should return DRM_E_BUFFERTOOSMALL and size in bytes.
    ** otherwise a header will be created.
    */
    dr = DRM_PRO_CreateRMHeader( pParams->eWRMHeaderVersion,
                                 eCipherType,
                                &dstrKID,
                                &dstrChecksum,
                                &pParams->dstrPlayReadySilentURL,
                                &pParams->dstrPlayReadyNonSilentURL,
                                &pParams->dstrServiceID,
                                &pParams->dstrCustomAttriburesXML,
                                &pParams->dstrDecryptorSetup,
                                 f_pbHeader,
                                 f_pcbHeader );

ErrorExit:
    _FreeWRMHeaderParams( pParams );
    SAFE_OEM_FREE( pbChecksum );
    return dr;
}


/*********************************************************************************************
** Function:  TST_UTL_TestAddLicenseBufferToPRO
**
** Synopsis:  Adds one license to embedded license store in the PRO.
**
** Arguments: f_pdstrKID    - string containing base64-encoded GUID for KID
**            f_pdstrLID    - string containing GUID for LID, see DRM_UTL_StringToLittleEndianGuid() for string format
**            f_pbLicense   - license binary blob, allow NULLs as this is a test API
**            f_cbLicense   - size of f_pbLicense, can be 0
**            f_pbPROBuffer - PRO buffer which data will be modified, can't be NULL
**            f_cbPROBuffer - size of PRO buffer, should not be zero
**
** Returns:   DRM_SUCCESS       - Successfully embedded a license.
**
** Note:      There is no dependency on current state of DRM app context.
*********************************************************************************************/
DRM_API DRM_RESULT DRM_CALL TST_UTL_TestAddLicenseBufferToPRO( __in                          const DRM_CONST_STRING *f_pdstrKID,
                                                               __in                          const DRM_CONST_STRING *f_pdstrLID,
                                                               __in_bcount_opt(f_cbLicense)  const DRM_BYTE         *f_pbLicense,
                                                               __in                          const DRM_DWORD         f_cbLicense,
                                                               __inout_bcount(f_cbPROBuffer) const DRM_BYTE         *f_pbPROBuffer,
                                                               __in                          const DRM_DWORD         f_cbPROBuffer )
{
    DRM_RESULT           dr               = DRM_SUCCESS;
    DRM_BYTE            *pbEmbeddedRecord = NULL;
    DRM_DWORD            cbEmbeddedRecord = 0;
    DRM_BYTE            *pbLicenseBuffer  = NULL;
    DRM_DST              oDatastoreEST    = { eDRM_DST_NONE, { 0 }, NULL };
    DRM_DWORD            cbLicense        = 0;
    DRM_LICSTORE_CONTEXT oELSContext      = {0};
    DRM_BOOL             fESTOpened       = FALSE;
    DRM_KID              oKID             = {0};
    DRM_LID              oLID             = {0};
    DRM_DST_CONTEXT     *pDSTContext      = NULL;

    ChkArg( f_pbPROBuffer != NULL );
    ChkArg( f_cbPROBuffer > 0 );

    ChkArg( f_pdstrKID != NULL );
    ChkArg( f_pdstrLID != NULL );

    ChkMem( pDSTContext = (DRM_DST_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_DST_CONTEXT) ) );
    ZEROMEM( pDSTContext, SIZEOF(DRM_DST_CONTEXT) );

    ChkDR( DRM_UTL_DecodeKID( f_pdstrKID, &oKID ) );
    ChkDR( DRM_UTL_StringToLittleEndianGuid( f_pdstrLID, ( DRM_GUID * )oLID.rgb ) );

    ChkDR( DRM_PRO_GetRecord( f_pbPROBuffer,
                              f_cbPROBuffer,
                              PLAYREADY_EMBEDDED_LICENSE_STORE,
                             &pbEmbeddedRecord,
                             &cbEmbeddedRecord ) );

    /*
    ** Set up enumeration over embedded store and HDS (XMR) store
    ** Get the embedded license store from the PlayReady Object
    */
    ChkDR( DRM_DST_Init ( eDRM_DST_EMBEDDED, pDSTContext, DRM_EST_CONTEXT_LEN, &oDatastoreEST ) );

    if( DRM_FAILED( DRM_DST_OpenStore( NULL,
                                       pbEmbeddedRecord,
                                       cbEmbeddedRecord,
                                       0,
                                      &oDatastoreEST ) ) )
    {
        /*
        ** Create the store if it doesn't already exist
        */
        ChkDR( DRM_DST_CreateStore( NULL,
                                    pbEmbeddedRecord,
                                    cbEmbeddedRecord,
                                    0,
                                    eDRM_DSTBLKNUM_DWORD,
                                    FALSE,
                                    0,
                                   &oDatastoreEST ) );
        ChkDR( DRM_DST_OpenStore( NULL,
                                  pbEmbeddedRecord,
                                  cbEmbeddedRecord,
                                  0,
                                 &oDatastoreEST ) );
    }
    fESTOpened = TRUE;

    /*
    ** copy license to another buffer
    ** and make a room for license priority.
    */
    cbLicense = f_cbLicense + SIZEOF( DRM_DWORD );
    ChkMem( pbLicenseBuffer = (DRM_BYTE*)Oem_MemAlloc( cbLicense ) );
    ZEROMEM( pbLicenseBuffer,cbLicense );

    DRM_BYT_CopyBytes( pbLicenseBuffer,
                       SIZEOF( DRM_DWORD ),
                       f_pbLicense,
                       0,
                       f_cbLicense );

    /*
    ** Add license directly to embedded store
    */
    ChkDR( DRM_LST_Open( &oELSContext,
                         &oDatastoreEST,
                          eDRM_LICENSE_STORE_XMR ) );

    ChkDR( DRM_LST_AddLicense( &oELSContext,
                                f_cbLicense,
                                pbLicenseBuffer,
                                &oKID,
                                &oLID,
                                0 ) );

ErrorExit:
    SAFE_OEM_FREE( pbLicenseBuffer );

    if ( oELSContext.fInited )
    {
        DRM_LST_Close( &oELSContext );
    }
    if ( fESTOpened )
    {
        DRM_DST_CloseStore( &oDatastoreEST );
    }
    SAFE_OEM_FREE(pDSTContext);
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

