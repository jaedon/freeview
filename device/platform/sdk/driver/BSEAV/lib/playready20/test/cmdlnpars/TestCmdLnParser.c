/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tclib.h"
#include "tstutils.h"
#include "drmcrt.h"
#include "drmcmdlnpars.h"

ENTER_PKTEST_NAMESPACE_CODE

/***************************************************************************
**
** Function: TestCommandLineParser
**
** Synopsis: Tests method DRM_CMD_ParseCmdLine
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- Input commmand line
** argv[1]      -- command option (char)
** argv[2]      -- Expected command value (can be omitted or NULL)
** argv[3]      -- Expected extra data (can be omitted or NULL)
**
** Returns:     DRM_SUCCESS on               -- on success.
**              DRM_E_TEST_PARSING_ERROR     -- if parsing of command line failed
**              DRM_E_TEST_UNEXPECTED_OUTPUT -- if parsing returned unexpected value
**                                              or no value
***************************************************************************/
DRM_RESULT DRM_CALL TestCommandLineParser(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT        dr                 = DRM_SUCCESS;
    DRM_CONST_STRING  dstrCmdLine        = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrParam          = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrExtra          = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrExpectedOption = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrExpectedParam  = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrExpectedExtra  = EMPTY_DRM_STRING;
    DRM_WCHAR         wchOptionChar      = 0;
    DRM_DWORD         dwArgC             = 0;
    DRM_DWORD         i                  = 0;
    DRM_WCHAR        *prgwszArgV[DRM_CMD_MAX_ARGUMENTS] = { 0 };
    DRM_BOOL          fFound             = FALSE;

    ChkArg( argc >= 2 && argc <= 4 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( MakeDRMString( &dstrCmdLine, argv[0] ) );
    ChkDR( MakeDRMString( &dstrExpectedOption, argv[1] ) );

    if ( argc > 2 && argv[2] != NULL )
    {
        ChkDR( MakeDRMString( &dstrExpectedParam, argv[2] ) );
    }

    if ( argc > 3 && argv[3] != NULL )
    {
        ChkDR( MakeDRMString( &dstrExpectedExtra, argv[3] ) );
    }

    dwArgC = DRM_CMD_ConvertToArgvArgc( dstrCmdLine.pwszString, prgwszArgV );
    ChkBOOL( dwArgC > 0, DRM_E_TEST_INVALIDARG ); /* Expecting at least 1 parameter */

    for( i = 0; i < dwArgC; i++ )
    {
        /* If parameter is expected then get it, otherwise - do not ask for it */
        if( !DRM_CMD_ParseCmdLine( prgwszArgV[i],
                                   &wchOptionChar,
                                   dstrExpectedParam.pwszString!=NULL?&dstrParam:NULL,
                                   dstrExpectedExtra.pwszString!=NULL?&dstrExtra:NULL ) )
        {
            ChkDR( DRM_E_TEST_PARSING_ERROR );
        }
        if( wchOptionChar == dstrExpectedOption.pwszString[0] )
        {
            if( dstrParam.cchString != 0 || dstrExpectedParam.cchString != 0 )
            {
                if( !DRM_UTL_DSTRStringsEqual( &dstrParam, &dstrExpectedParam ) )
                {
                    ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                }
            }
            if( dstrExtra.cchString != 0 || dstrExpectedExtra.cchString != 0 )
            {
                if( !DRM_UTL_DSTRStringsEqual( &dstrExtra, &dstrExpectedExtra ) )
                {
                    ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                }
            }
            fFound = TRUE; /* command option was found */
        }
    }

    if( !fFound )
    {
        dr = DRM_E_TEST_UNEXPECTED_OUTPUT;
    }

ErrorExit:
    FREEDRMSTRING( dstrCmdLine );
    FREEDRMSTRING( dstrExpectedOption );
    FREEDRMSTRING( dstrExpectedParam );
    FREEDRMSTRING( dstrExpectedExtra );
    return dr;
}

/***************************************************************************
**
** Function: TestCommandLineParserMultiChar
**
** Synopsis: Tests method DRM_CMD_ParseCmdLine2
**           This method is almost the same as DRM_CMD_ParseCmdLine
**           but option command is string instead of single character
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- Input commmand line
** argv[1]      -- command option (char)
** argv[2]      -- Expected command value (can be omitted or NULL)
** argv[3]      -- Expected extra data (can be omitted or NULL)
**
** Returns:     DRM_SUCCESS on               -- success.
**              DRM_E_TEST_PARSING_ERROR     -- if parsing of command line failed
**              DRM_E_TEST_UNEXPECTED_OUTPUT -- if parsing returned unexpected value
**                                              or no value
***************************************************************************/
DRM_RESULT DRM_CALL TestCommandLineParserMultiChar(
    IN long argc,
    IN __in_ecount(argc) char **argv )
{
    DRM_RESULT        dr                 = DRM_SUCCESS;
    DRM_CONST_STRING  dstrCmdLine        = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrParam          = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrExtra          = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrExpectedOption = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrExpectedParam  = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrExpectedExtra  = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrOption         = EMPTY_DRM_STRING;
    DRM_DWORD         dwArgC             = 0;
    DRM_DWORD         i                  = 0;
    DRM_BOOL          fFound             = FALSE;
    DRM_WCHAR        *prgwszArgV[DRM_CMD_MAX_ARGUMENTS] = { 0 };

    ChkArg( argc >= 2 && argc <= 4 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( MakeDRMString( &dstrCmdLine, argv[0] ) );
    ChkDR( MakeDRMString( &dstrExpectedOption, argv[1] ) );

    if ( argc > 2 && argv[2] != NULL )
    {
        ChkDR( MakeDRMString( &dstrExpectedParam, argv[2] ) );
    }

    if ( argc > 3 && argv[3] != NULL )
    {
        ChkDR( MakeDRMString( &dstrExpectedExtra, argv[3] ) );
    }

    dwArgC = DRM_CMD_ConvertToArgvArgc( dstrCmdLine.pwszString, prgwszArgV );
    ChkBOOL( dwArgC > 0, DRM_E_TEST_INVALIDARG ); /* Expecting at least 1 parameter */

    for( i = 0; i < dwArgC; i++ )
    {
        /* If parameter is expected then get it, otherwise - do not ask for it */
        if( !DRM_CMD_ParseCmdLineMultiChar( prgwszArgV[i],
                                           &dstrOption,
                                            dstrExpectedParam.pwszString!=NULL?&dstrParam:NULL,
                                            dstrExpectedExtra.pwszString!=NULL?&dstrExtra:NULL ) )
        {
            ChkDR( DRM_E_TEST_PARSING_ERROR );
        }
        if( DRM_UTL_DSTRStringsEqual( &dstrOption, &dstrExpectedOption ) )
        {
            if( dstrParam.cchString != 0 || dstrExpectedParam.cchString != 0 )
            {
                if( !DRM_UTL_DSTRStringsEqual( &dstrParam, &dstrExpectedParam ) )
                {
                    ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                }
            }
            if( dstrExtra.cchString != 0 || dstrExpectedExtra.cchString != 0 )
            {
                if( !DRM_UTL_DSTRStringsEqual( &dstrExtra, &dstrExpectedExtra ) )
                {
                    ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
                }
            }
            fFound = TRUE; /* command option was found */
        }
    }

    if( !fFound )
    {
        dr = DRM_E_TEST_UNEXPECTED_OUTPUT;
    }

ErrorExit:
    FREEDRMSTRING( dstrCmdLine );
    FREEDRMSTRING( dstrExpectedOption );
    FREEDRMSTRING( dstrExpectedParam );
    FREEDRMSTRING( dstrExpectedExtra );
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE
IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_APIMAP(RefTestCmdLnParser_ansi, "CmdLnParser")
    PKTEST_API_ENTRY( TestCommandLineParser )
    PKTEST_API_ENTRY( TestCommandLineParserMultiChar )
END_APIMAP
