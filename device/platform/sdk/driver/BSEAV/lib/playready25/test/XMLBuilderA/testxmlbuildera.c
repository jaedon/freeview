/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
    Most of function has additional argument using as flags to
    decide which input should be overwritten:
    1st bit overwrite g_poXMLContextA string
    2nd bit overwrite g_cbXmlContextA size
    3rd bit overwrite pwszNodeName string
    4th bit overwrite pwszNodeName size
    5th bit Attribute name/CData has leading space
    6th bit Attribute name/CData has traling space
    7th bit attribute value/Data has leading space
    8th bit attribute value/Data has trailing space
*/

#include <stdafx.h>
#include <ctype.h>
#include <drmxmlbuildera.h>
#include <XMLBuilderATestAPI.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

ENTER_PKTEST_NAMESPACE_CODE

/* Global XML builder context. */
static DRM_DWORD g_cbXmlContextA = 0;
static _XMBContextA *g_poXMLContextA = NULL;

/*********************************************************************
**
** Function: TestXMLBAUtilReinitialize
**
** Synopsis: Function that initializes the test environment.
**
** Arguments:
**
** [iIndex]                 -- Type of action to take.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other types of error.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBAUtilReinitialize(
    __in long iIndex,
    __in_opt char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if ( argv != NULL )
    {
        iIndex = ( int )atoi( argv[ 0 ] );
    }

    if ( iIndex ==0 || iIndex == 1 )
    {
        SAFE_OEM_FREE( g_poXMLContextA );
        g_poXMLContextA = NULL;
    }

     if ( iIndex==0 || iIndex == 2 )
     {
        g_cbXmlContextA = 0;
    }

    return dr;
}

/*********************************************************************
**
** Function: TestXMLBACreateDocument
**
** Synopsis: Function that creates and initializes a XML builder context.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other types of error.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBACreateDocument(
                                         __in                 long argc,
                                         __in_ecount(argc)    char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    long i, iOption = 0;
    DRM_ANSI_CONST_STRING dastrRootNodeName = EMPTY_DRM_STRING;
    int iRootNodeNameNullFlag = 0;
    DRM_CHAR szLocalString[ 512 ] = { 0 };

    for( i = 0; i < argc; i++ )
    {
        Log( "Trace", "\t\tFrom TestXMLBACreateDocument: argv[ %d ]=%s", i, argv[ i ] );
    }

    /* At least 5 arguments. The last one is used to decide whether we need to overwrite the context. */
    if ( argc < 5 )
    {
        Log( "Trace", "\t\tFrom TestXMLBACreateDocument: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    if ( !IsDigitalOnlyString( argv[ 0 ] ) ||
         !IsDigitalOnlyString( argv[ 3 ] ) ||
         !IsDigitalOnlyString( argv[ 4 ] ) )
    {
        Log( "Trace", "\t\tFrom TestXMLBACreateDocument: error in the argument." );
        ChkDR( DRM_E_FAIL );
    }

    iOption = ( long )atol( argv[ 4 ] );

    /* The 1st bit is to overwrite g_poXMLContextA. */
    if ( iOption & 1 )
    {
        TestXMLBAUtilReinitialize( 1, NULL );

        if ( argv[ 1 ] != NULL )
        {
             /* The 2nd bit is to overwrite g_cbXmlContext size. */
            if ( iOption & 2 )
            {
                g_cbXmlContextA = ( DRM_DWORD )atol( argv[ 0 ] );
                g_poXMLContextA = ( _XMBContextA * ) Oem_MemAlloc( g_cbXmlContextA + 1 );

                if ( g_poXMLContextA == NULL )
                {
                    ChkDR( DRM_E_FAIL );
                }
            }
            else
            {
                g_cbXmlContextA = ( DRM_DWORD )strlen( argv[ 1 ] );
                g_poXMLContextA = ( _XMBContextA * )Oem_MemAlloc( g_cbXmlContextA + 1 );

                if ( g_poXMLContextA == NULL )
                {
                    ChkDR( DRM_E_FAIL );
                }
                MEMCPY( g_poXMLContextA, argv[ 1 ], g_cbXmlContextA + 1 );
            }
        }
    }

    if ( iOption & 4 )
    {
        if( argv[ 2 ] != NULL )
        {
            if( strcmp( argv[ 2 ], "nullstring" ) )
            {
                DRM_DWORD cchLocalString = ( DRM_DWORD ) min( strlen( argv[ 2 ] ), SIZEOF( szLocalString ) - 1 );
                MEMCPY( szLocalString, argv[ 2 ], cchLocalString );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                szLocalString[ cchLocalString ] = '\0';
#pragma prefast(pop)
                dastrRootNodeName.pszString = szLocalString;
                dastrRootNodeName.cchString = cchLocalString;
            }
            else
            {
                dastrRootNodeName.pszString = NULL;
                dastrRootNodeName.cchString = 0;
            }
        }
        else
        {
            iRootNodeNameNullFlag = 1;
        }
    }

    if ( iOption & 8 )
    {
        dastrRootNodeName.cchString = ( DRM_DWORD )atol( argv[ 3 ] );
    }

    ChkDR( DRM_XMB_CreateDocumentA( g_cbXmlContextA, (DRM_BYTE *)g_poXMLContextA, iRootNodeNameNullFlag > 0 ? NULL : &dastrRootNodeName ) );

ErrorExit:

    /* g_poXMLContextA is freed somewhere else. */
    return dr;
}

/*********************************************************************
**
** Function: TestXMLBACloseDocument
**
** Synopsis: Function that closes a XML builder context.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other types of error.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBACloseDocument(
                                        __in              long argc,
                                        __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    long i,iOption = 0;
    DRM_SUBSTRING dasstrXMLString = EMPTY_DRM_SUBSTRING;
    DRM_CHAR szVerifyArrayXMLString[ 2000 ] = { 0 };
    DRM_CHAR *szBase = NULL;
    _XMBContextA *pbLocalXmlContextA = NULL;
    int j = 0, iStringNullFlag = 0;

    for( i = 0; i < argc; i++ )
    {
        Log( "Trace", "\t\tFrom TestXMLBACloseDocument: argv[ %d ]=%s", i, argv[ i ] );
    }

    /* At least 5 arguments. The last one is used to decide whether we need to overwrite the context. */
    /* 4th is used to verify the xml content. */
    if ( argc < 5 )
    {
        Log( "Trace", "\t\tFrom TestXMLBACloseDocument: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    if ( !IsDigitalOnlyString( argv[ 2 ] ) || !IsDigitalOnlyString( argv[ 4 ] ) )
    {
        Log( "Trace", "\t\tFrom TestXMLBACloseDocument: error in the argument." );
        ChkDR( DRM_E_FAIL );
    }

    iOption = ( long )atol( argv[ 4 ] );

    if ( iOption & 1 )
    {
        if ( argv[ 0 ] != NULL )
        {
            ChkMem( pbLocalXmlContextA = ( _XMBContextA* )Oem_MemAlloc( ( DRM_DWORD ) strlen( argv[ 0 ] ) + 1 ) );
            MEMCPY( pbLocalXmlContextA, argv[ 0 ], strlen( argv[ 0 ] ) + 1 );
        }
        j = 1;
    }

    if ( argv[ 1 ] == NULL )
    {
        iStringNullFlag = 1;
    }

    if ( j == 1 )
    {
        ChkDR( DRM_XMB_CloseDocumentA( pbLocalXmlContextA, iStringNullFlag > 0 ? NULL : &dasstrXMLString ) );

        ChkDR( DRM_XMB_GetXMLBaseA( pbLocalXmlContextA, &szBase) );
    }
    else
    {
        ChkDR( DRM_XMB_CloseDocumentA( g_poXMLContextA, iStringNullFlag > 0 ? NULL : &dasstrXMLString ) );

        ChkDR( DRM_XMB_GetXMLBaseA( g_poXMLContextA, &szBase) );
    }

    /* Verify the result. */
    if ( argv[ 3 ] != NULL )
    {
        MEMCPY( szVerifyArrayXMLString, argv[ 3 ], min( SIZEOF( szVerifyArrayXMLString ), strlen( argv[ 3 ] ) + 1 ) );
        if ( strncmp( szVerifyArrayXMLString, szBase + dasstrXMLString.m_ich, strlen( szVerifyArrayXMLString ) ) )
        {
            ChkDR( DRM_E_FAIL );
        }
    }

ErrorExit:

    SAFE_OEM_FREE( pbLocalXmlContextA );

    return dr;
}

/*********************************************************************
**
** Function: TestXMLBAOpenNode
**
** Synopsis: Function that creates a new XML node.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other types of error.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBAOpenNode(
                                   __in              long argc,
                                   __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    long i,iOption = 0;
    DRM_ANSI_CONST_STRING dastrNodeName = EMPTY_DRM_STRING;
    DRM_CHAR szArrayNodeName[ 512 ] = { 0 };
    _XMBContextA *pbLocalXmlContextA = NULL;
    int j = 0, iNodeNameNullFlag = 0;

    for( i = 0; i < argc; i++ )
    {
        Log( "Trace", "\t\tFrom TestXMLBAOpenNode: argv[ %d ]=%s", i, argv[ i ] );
    }

    /* At least 4 arguments. The last one is used to decide whether we need to overwrite the context. */
    if ( argc < 4 )
    {
        Log( "Trace", "\t\tFrom TestXMLBAOpenNode: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    if ( !IsDigitalOnlyString( argv[ 2 ] ) || !IsDigitalOnlyString( argv[ 3 ] ) )
    {
        Log( "Trace", "\t\tFrom TestXMLBAOpenNode: error in the argument." );
        ChkDR( DRM_E_FAIL );
    }

    iOption = ( long )atol( argv[ 3 ] );

    if ( iOption & 1 )
    {
        if ( argv[ 0 ] != NULL )
        {
            ChkMem( pbLocalXmlContextA = ( _XMBContextA* )Oem_MemAlloc( ( DRM_DWORD ) strlen( argv[ 0 ] ) + 1 ) );
            MEMCPY( pbLocalXmlContextA, argv[ 0 ], strlen( argv[ 0 ] ) + 1 );
        }
        j = 1;
    }

    if ( iOption & 4 )
    {
        if ( argv[ 1 ] != NULL )
        {
            if ( strcmp( argv[ 1 ], "nullstring" ) )
            {
                DRM_DWORD cchArrayNodeName = min( ( DRM_DWORD ) strlen( argv[ 1 ] ), SIZEOF( szArrayNodeName ) - 1 );
                MEMCPY( szArrayNodeName, argv[ 1 ], cchArrayNodeName );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                szArrayNodeName[ cchArrayNodeName ] = '\0';
#pragma prefast(pop)
                dastrNodeName.pszString = szArrayNodeName;
                dastrNodeName.cchString = cchArrayNodeName;
            }
            else
            {
                dastrNodeName.pszString = NULL;
                dastrNodeName.cchString = 0;
            }
        }
        else
        {
            iNodeNameNullFlag = 1;
        }
    }

    if ( iOption & 8 )
    {
        dastrNodeName.cchString = ( DRM_DWORD )atol( argv[ 2 ] );
    }

    if ( j==1 )
    {
        ChkDR( DRM_XMB_OpenNodeA( pbLocalXmlContextA, iNodeNameNullFlag > 0 ? NULL : &dastrNodeName ) );
    }
    else
    {
        ChkDR( DRM_XMB_OpenNodeA( g_poXMLContextA, iNodeNameNullFlag > 0 ? NULL : &dastrNodeName ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pbLocalXmlContextA );

    return dr;
}

/*********************************************************************
**
** Function: TestXMLBACloseCurrNode
**
** Synopsis: Function that closes the current XML node.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other types of error.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBACloseCurrNode(
                                        __in              long argc,
                                        __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    long i,iOption = 0;
    DRM_SUBSTRING dasstrXMLFragment = EMPTY_DRM_SUBSTRING;
    _XMBContextA *pbLocalXmlContextA = NULL;
    int j = 0, iFragmentNullFlag = 0;

    for( i = 0; i < argc; i++ )
    {
        Log( "Trace", "\t\tFrom TestXMLBACloseCurrNode: argv[ %d ]=%s", i, argv[ i ] );
    }

    /* At least 4 arguments. The last one is used to decide whether we need to overwrite the context. */
    if ( argc < 4 )
    {
        Log( "Trace", "\t\tFrom TestXMLBACloseCurrNode: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    if ( !IsDigitalOnlyString( argv[ 2 ] ) || !IsDigitalOnlyString( argv[ 3 ] ) )
    {
        Log( "Trace", "\t\tFrom TestXMLBACloseCurrNode: error in the argument." );
        ChkDR( DRM_E_FAIL );
    }

    iOption = ( long )atol( argv[ 3 ] );

    if ( iOption & 1 )
    {
        if ( argv[ 0 ] != NULL )
        {
            pbLocalXmlContextA = ( _XMBContextA* )Oem_MemAlloc( ( DRM_DWORD )strlen( argv[ 0 ] ) + 1 );

            if ( pbLocalXmlContextA == NULL )
            {
                ChkDR( DRM_E_FAIL );
            }
            MEMCPY( pbLocalXmlContextA, argv[ 0 ], strlen( argv[ 0 ] ) + 1 );
        }
        j=1;
    }

    if ( argv[ 1 ] == NULL )
    {
        iFragmentNullFlag = 1;
    }

    if ( j==1 )
    {
        ChkDR( DRM_XMB_CloseCurrNodeA( pbLocalXmlContextA, iFragmentNullFlag > 0 ? NULL : &dasstrXMLFragment ) );
    }
    else
    {
        ChkDR( DRM_XMB_CloseCurrNodeA( g_poXMLContextA, iFragmentNullFlag > 0? NULL : &dasstrXMLFragment ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pbLocalXmlContextA );

    return dr;
}

/*********************************************************************
**
** Function: TestXMLBAAddAttribute
**
** Synopsis: Function that add an attribute pair to the current XML node.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other types of error.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBAAddAttribute(
                                        __in              long argc,
                                        __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    long i,iOption = 0;
    DRM_ANSI_CONST_STRING dastrAttrName = EMPTY_DRM_STRING;
    DRM_CHAR szArrayAttrName[ 512 ] = { 0 };
    DRM_ANSI_CONST_STRING dastrAttrValue = EMPTY_DRM_STRING;
    DRM_CHAR szArrayAttrValue[ 512 ] = { 0 };
    _XMBContextA *pbLocalXmlContextA = NULL;
    int iNodeNameNullFlag = 0, iNodeValueNullFlag = 0, j = 0;

    for ( i = 0; i < argc; i++ )
    {
        Log( "Trace", "\t\tFrom TestXMLBAAddAttribute: argv[ %d ]=%s", i, argv[ i ] );
    }

    /* At least 6 arguments. The last one is used to decide whether we need to overwrite the context. */
    if ( argc < 6 )
    {
        Log( "Trace", "\t\tFrom TestXMLBAAddAttribute: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    if ( !IsDigitalOnlyString( argv[ 2 ] ) || !IsDigitalOnlyString( argv[ 4 ] ) || !IsDigitalOnlyString( argv[ 5 ] ) )
    {
        Log( "Trace", "\t\tFrom TestXMLBAAddAttribute: error in the argument." );
        ChkDR( DRM_E_FAIL );
    }

    iOption = ( long )atol( argv[ 5 ] );

    if ( iOption & 1 )
    {
        if( argv[ 0 ] != NULL )
        {
            pbLocalXmlContextA = ( _XMBContextA * )Oem_MemAlloc( ( DRM_DWORD ) strlen( argv[ 0 ] ) + 1 );
            if ( pbLocalXmlContextA == NULL )
            {
                ChkDR( DRM_E_FAIL );
            }
            MEMCPY( pbLocalXmlContextA, argv[ 0 ], strlen( argv[ 0 ] ) + 1 );
        }
        j = 1;
    }

    if ( argv[ 1 ] != NULL )
    {
        if( strcmp( argv[ 1 ], "nullstring" ) )
        {
            /*
            ** size - 2 to have room for an extra space and for a null char
            */
            DRM_DWORD cchAttr = min( ( DRM_DWORD ) strlen( argv[ 1 ] ), SIZEOF( szArrayAttrName ) - 2 );
            if ( iOption & 16 )
            {
                szArrayAttrName[0] = ' ';
                DRM_BYT_CopyBytes( szArrayAttrName, 1, argv[ 1 ], 0, cchAttr );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                szArrayAttrName[cchAttr + 1 ] = '\0';
#pragma prefast(pop)
            }
            else if ( iOption & 32 )
            {
                DRM_BYT_CopyBytes( szArrayAttrName, 0, argv[ 1 ], 0, cchAttr );
                szArrayAttrName[cchAttr] = ' ';
                szArrayAttrName[cchAttr + 1 ] = '\0';
            }
            else
            {
                DRM_BYT_CopyBytes( szArrayAttrName, 0, argv[ 1 ], 0, cchAttr );
                szArrayAttrName[cchAttr]  = '\0';
            }
            dastrAttrName.pszString = szArrayAttrName;
            dastrAttrName.cchString = ( DRM_DWORD )strlen( szArrayAttrName );
        }
    }
    else
    {
        iNodeNameNullFlag = 1;
    }

    if ( argv[ 3 ] != NULL )
    {
        if ( strcmp( argv[ 3 ], "nullstring" ) )
        {
            /*
            ** size - 2 to have room for an extra space and for a null char
            */
            DRM_DWORD cchAttr = min( ( DRM_DWORD ) strlen( argv[ 3 ] ), SIZEOF( szArrayAttrValue ) - 2 );
            if ( iOption & 64 )
            {
                szArrayAttrValue[0] = ' ';
                DRM_BYT_CopyBytes( szArrayAttrValue, 1, argv[ 3 ], 0, cchAttr );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                szArrayAttrValue[cchAttr + 1 ] = '\0';
#pragma prefast(pop)
            }
            else if ( iOption & 128 )
            {
                DRM_BYT_CopyBytes( szArrayAttrValue, 0, argv[ 3 ], 0, cchAttr );
                szArrayAttrValue[cchAttr] = ' ';
                szArrayAttrValue[cchAttr + 1 ] = '\0';
            }
            else
            {
                DRM_BYT_CopyBytes( szArrayAttrValue, 0, argv[ 3 ], 0, cchAttr );
                szArrayAttrValue[cchAttr]  = '\0';
            }
            dastrAttrValue.pszString = szArrayAttrValue;
            dastrAttrValue.cchString = ( DRM_DWORD )strlen( szArrayAttrValue );

        }
    }
    else
    {
        iNodeValueNullFlag = 1;
    }

    if ( j == 1 )
    {
        ChkDR( DRM_XMB_AddAttributeA( pbLocalXmlContextA, iNodeNameNullFlag > 0 ? NULL : &dastrAttrName, iNodeValueNullFlag > 0 ? NULL : &dastrAttrValue ) );
    }
    else
    {
        ChkDR( DRM_XMB_AddAttributeA( g_poXMLContextA, iNodeNameNullFlag > 0 ? NULL : &dastrAttrName, iNodeValueNullFlag > 0 ? NULL : &dastrAttrValue ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pbLocalXmlContextA );

    return dr;
}

/*********************************************************************
**
** Function: TestXMLBAAddData
**
** Synopsis: Function that adds data to the current XML node.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other types of error.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBAAddData(
                                  __in              long argc,
                                  __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    long i,iOption = 0;
    DRM_ANSI_CONST_STRING dastrXMLData = EMPTY_DRM_STRING;
    DRM_CHAR szArrayXMLData[ 512 ] = { 0 };
    _XMBContextA *pbLocalXmlContextA = NULL;
    int iXmlDataNullFlag = 0, j = 0;

    for ( i=0; i < argc; i++ )
    {
        Log( "Trace", "\t\tFrom TestXMLBAAddData: argv[ %d ]=%s", i, argv[ i ] );
    }

    /* At least 4 arguments. The last one is used to decide whether we need to overwrite the context. */
    if ( argc < 4 )
    {
        Log( "Trace", "\t\tFrom TestXMLBAAddData: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    if ( !IsDigitalOnlyString( argv[ 2 ] ) || !IsDigitalOnlyString( argv[ 3 ] ) )
    {
        Log( "Trace", "\t\tFrom TestXMLBAAddData: error in the argument." );
        ChkDR( DRM_E_FAIL );
    }

    iOption = ( long )atol( argv[ 3 ] );

    if ( iOption & 1 )
    {
        if ( argv[ 0 ] != NULL )
        {
            pbLocalXmlContextA = ( _XMBContextA* )Oem_MemAlloc( ( DRM_DWORD )strlen( argv[ 0 ] ) + 1 );
            if ( pbLocalXmlContextA == NULL )
            {
                ChkDR( DRM_E_FAIL );
            }
            MEMCPY( pbLocalXmlContextA, argv[ 0 ], strlen( argv[ 0 ] ) + 1 );
        }
        j = 1;
    }

    if ( argv[ 1 ] != NULL )
    {
        if ( strcmp( argv[ 1 ], "nullstring" ) )
        {
            DRM_DWORD cchXMLData = min( ( DRM_DWORD ) strlen( argv[ 1 ] ), SIZEOF( szArrayXMLData ) - 2 );
            if ( iOption & 64 )
            {
                szArrayXMLData[0] = ' ';
                DRM_BYT_CopyBytes( szArrayXMLData, 1, argv[ 1 ], 0, cchXMLData );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                szArrayXMLData[ cchXMLData + 1] = '\0';
#pragma prefast(pop)
            }
            else if ( iOption & 128 )
            {
                DRM_BYT_CopyBytes( szArrayXMLData, 0, argv[ 1 ], 0, cchXMLData );
                szArrayXMLData[cchXMLData] = ' ';
                szArrayXMLData[cchXMLData + 1] = '\0';
            }
            else
            {
                DRM_BYT_CopyBytes( szArrayXMLData, 0, argv[ 1 ], 0, cchXMLData );
                szArrayXMLData[cchXMLData]  = '\0';
            }
            dastrXMLData.pszString = szArrayXMLData;
            dastrXMLData.cchString = ( DRM_DWORD )strlen( szArrayXMLData );

        }
    }
    else
    {
        iXmlDataNullFlag = 1;
    }

    if ( j == 1 )
    {
        ChkDR( DRM_XMB_AddDataA( pbLocalXmlContextA, iXmlDataNullFlag > 0 ? NULL : &dastrXMLData ) );
    }
    else
    {
        ChkDR( DRM_XMB_AddDataA( g_poXMLContextA, iXmlDataNullFlag > 0 ? NULL : &dastrXMLData ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pbLocalXmlContextA );

    return dr;
}

/*********************************************************************
**
** Function: TestXMLBAAddCData
**
** Synopsis: Function that adds CDATA to the current XML node.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other types of error.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBAAddCData(
                                    __in              long argc,
                                    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    long i,iOption = 0;
    DRM_ANSI_CONST_STRING dastrXMLCData = EMPTY_DRM_STRING;
    DRM_CHAR szArrayXMLCData[ 512 ] = { 0 };
    _XMBContextA *pbLocalXmlContextA = NULL;
    int iXmlCDataNullFlag = 0, j = 0;

    for ( i = 0; i < argc; i++ )
    {
        Log( "Trace", "\t\tFrom TestXMLBAAddCData: argv[ %d ]=%s", i, argv[ i ] );
    }

    /* At least 4 arguments. The last one is used to decide whether we need to overwrite the context. */
    if ( argc < 4 )
    {
        Log( "Trace", "\t\tFrom TestXMLBAAddCData: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    if ( !IsDigitalOnlyString( argv[ 2 ] ) || !IsDigitalOnlyString( argv[ 3 ] ) )
    {
        Log( "Trace", "\t\tFrom TestXMLBAAddCData: error in the argument." );
        ChkDR( DRM_E_FAIL );
    }

    iOption = ( long )atol( argv[ 3 ] );

    if ( iOption & 1 )
    {
        if ( argv[ 0 ] != NULL )
        {
            pbLocalXmlContextA = ( _XMBContextA* )Oem_MemAlloc( ( DRM_DWORD ) strlen( argv[ 0 ] ) + 1 );
            if ( pbLocalXmlContextA == NULL )
            {
                ChkDR( DRM_E_FAIL );
            }
            MEMCPY( pbLocalXmlContextA, argv[ 0 ], strlen( argv[ 0 ] ) + 1 );
        }
        j = 1;
    }

    if ( argv[ 1 ] != NULL )
    {
        if ( strcmp( argv[ 1 ], "nullstring" ) )
        {
            DRM_DWORD cchXMLCData = ( DRM_DWORD ) min( strlen( argv[ 1 ] ), SIZEOF( szArrayXMLCData ) - 2 );
            if ( iOption & 16 )
            {
                szArrayXMLCData[0] = ' ';
                DRM_BYT_CopyBytes( szArrayXMLCData, 1, argv[ 1 ], 0, cchXMLCData );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
                szArrayXMLCData[ cchXMLCData + 1] = '\0';
#pragma prefast(pop)
            }
            else if ( iOption & 32 )
            {
                DRM_BYT_CopyBytes( szArrayXMLCData, 0, argv[ 1 ], 0, cchXMLCData );
                szArrayXMLCData[cchXMLCData] = ' ';
                szArrayXMLCData[cchXMLCData + 1] = '\0';
            }
            else
            {
                DRM_BYT_CopyBytes( szArrayXMLCData, 0, argv[ 1 ], 0, cchXMLCData );
                szArrayXMLCData[cchXMLCData]  = '\0';
            }
            dastrXMLCData.pszString = szArrayXMLCData;
            dastrXMLCData.cchString = ( DRM_DWORD )strlen( szArrayXMLCData );

        }
    }
    else
    {
        iXmlCDataNullFlag = 1;
    }

    if ( j == 1 )
    {
        ChkDR( DRM_XMB_AddCDataA( pbLocalXmlContextA, iXmlCDataNullFlag > 0 ? NULL : &dastrXMLCData ) );
    }
    else
    {
        ChkDR( DRM_XMB_AddCDataA( g_poXMLContextA, iXmlCDataNullFlag > 0 ? NULL : &dastrXMLCData ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pbLocalXmlContextA );

    return dr;
}

/*********************************************************************
**
** Function: TestXMLBAReserveSpace
**
** Synopsis: Function that reserves some space in the current XML node.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL for other types of error.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestXMLBAReserveSpace(
                                        __in              long argc,
                                        __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    long i, j = 0, iOption = 0;
    DRM_ANSI_CONST_STRING dastrXMLData = EMPTY_DRM_STRING;
    DRM_CHAR szArrayXMLData[ 512 ] = { 0 };
    DRM_CHAR *pszBase = NULL;
    DRM_SUBSTRING dasstrData = EMPTY_DRM_SUBSTRING;
    _XMBContextA *pbLocalXmlContextA = NULL;

    for ( i = 0; i < argc; i++ )
    {
        Log( "Trace", "\t\tFrom TestXMLBAReserveSpace: argv[ %d ]=%s", i, argv[ i ] );
    }

    /* At least 4 arguments. The last one is used to decide whether we need to overwrite the context. */
    if ( argc < 4 )
    {
        Log( "Trace", "\t\tFrom TestXMLBAReserveSpace: error in number of arguments." );
        ChkDR( DRM_E_FAIL );
    }

    if ( !IsDigitalOnlyString( argv[ 2 ] ) || !IsDigitalOnlyString( argv[ 3 ] ) )
    {
        Log( "Trace", "\t\tFrom TestXMLBAReserveSpace: error in the argument." );
        ChkDR( DRM_E_FAIL );
    }

    iOption = ( long )atol( argv[ 3 ] );

    if ( iOption & 1 )
    {
        if ( argv[ 0 ] != NULL )
        {
            pbLocalXmlContextA = ( _XMBContextA* )Oem_MemAlloc( ( DRM_DWORD ) strlen( argv[ 0 ] ) + 1 );
            if ( pbLocalXmlContextA == NULL )
            {
                ChkDR( DRM_E_FAIL );
            }
            MEMCPY( pbLocalXmlContextA, argv[ 0 ], strlen( argv[ 0 ] ) + 1 );
        }
        j = 1;
    }

    if ( argv[ 1 ] != NULL )
    {
        DRM_DWORD cchXMLData = ( DRM_DWORD ) min( strlen( argv[ 1 ] ), SIZEOF( szArrayXMLData ) - 2 );
        if ( iOption & 64 )
        {
            szArrayXMLData[0] = ' ';
            DRM_BYT_CopyBytes( szArrayXMLData, 1, argv[ 1 ], 0, cchXMLData );
#pragma prefast(push)
#pragma prefast(disable:26015) /* PREfast complains that we might overflow, but it's simply not possible */
            szArrayXMLData[ cchXMLData + 1] = '\0';
#pragma prefast(pop)
        }
        else if ( iOption & 128 )
        {
            DRM_BYT_CopyBytes( szArrayXMLData, 0, argv[ 1 ], 0, cchXMLData );
            szArrayXMLData[cchXMLData] = ' ';
            szArrayXMLData[cchXMLData + 1] = '\0';
        }
        else
        {
            DRM_BYT_CopyBytes( szArrayXMLData, 0, argv[ 1 ], 0, cchXMLData );
            szArrayXMLData[cchXMLData]  = '\0';
        }
        dastrXMLData.pszString = szArrayXMLData;
        dastrXMLData.cchString = ( DRM_DWORD )strlen( szArrayXMLData );
    }

    if ( j == 1 )
    {
        ChkDR( DRM_XMB_ReserveSpaceA( pbLocalXmlContextA, SIZEOF(DRM_CHAR), dastrXMLData.cchString, &dasstrData ) );

        ChkDR( DRM_XMB_GetXMLBaseA( pbLocalXmlContextA, &pszBase ) );
    }
    else
    {
        ChkDR( DRM_XMB_ReserveSpaceA( g_poXMLContextA, SIZEOF(DRM_CHAR), dastrXMLData.cchString, &dasstrData ) );

        ChkDR( DRM_XMB_GetXMLBaseA( g_poXMLContextA, &pszBase ) );
    }

    DRM_BYT_CopyBytes( pszBase,
                       dasstrData.m_ich,
                       dastrXMLData.pszString,
                       0,
                       dastrXMLData.cchString );

ErrorExit:

    SAFE_OEM_FREE( pbLocalXmlContextA );

    return dr;
}

/* Get the default implementations for WrapTestRun, PreTestCase and PostTestCase. */
IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE( XMLBuilderA )

BEGIN_TEST_API_LIST( XMLBuilderA )
    API_ENTRY( TestXMLBACreateDocument )
    API_ENTRY( TestXMLBACloseDocument )
    API_ENTRY( TestXMLBAOpenNode )
    API_ENTRY( TestXMLBACloseCurrNode )
    API_ENTRY( TestXMLBAAddAttribute )
    API_ENTRY( TestXMLBAAddData )
    API_ENTRY( TestXMLBAAddCData )
    API_ENTRY( TestXMLBAReserveSpace )
    API_ENTRY( TestXMLBAUtilReinitialize )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

