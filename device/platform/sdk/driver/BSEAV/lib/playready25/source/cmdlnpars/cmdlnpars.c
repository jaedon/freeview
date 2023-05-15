/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_CMDLNPARS_C
#include <drmbytemanip.h>
#include <drmcrt.h>
#include <drmutilities.h>
#include <drmcmdlnpars.h>
#include <drmutf.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/* "drmpath" */
static const DRM_WCHAR        g_rgwchDRMPATH_OPTION_STRING[] = { WCHAR_CAST('d'),  WCHAR_CAST('r'),  WCHAR_CAST('m'),  WCHAR_CAST('p'),  WCHAR_CAST('a'),  WCHAR_CAST('t'),  WCHAR_CAST('h'),  WCHAR_CAST('\0') };
static const DRM_CONST_STRING g_dstrDRMPATH_OPTION_STRING    = CREATE_DRM_STRING( g_rgwchDRMPATH_OPTION_STRING );


/*********************************************************************
**
**  Function:  DRM_CMD_ConvertToArgvArgc
**
**  Synopsis:  Converts a raw command line string to individual argv parameters
**
**  Arguments:
**     [lpCmdLine]  -- Command line
**     [argv]       -- Arguments
**
**  Returns:
**
**
*********************************************************************/
DRM_LONG DRM_CALL DRM_CMD_ConvertToArgvArgc(
    __inout __nullterminated                  const DRM_WCHAR *lpCmdLine,
    __out_ecount_opt( DRM_CMD_MAX_ARGUMENTS )       DRM_WCHAR *argv[DRM_CMD_MAX_ARGUMENTS] )
{
    DRM_LONG            nIndex     = 0;
    DRM_WCHAR          *pwszCmd    = (DRM_WCHAR*) lpCmdLine;
    DRM_WCHAR           rgwcSeps[] = { ONE_WCHAR(' ', '\0'), ONE_WCHAR('\t', '\0'), ONE_WCHAR('\n', '\0'), ONE_WCHAR('\0', '\0') };

    const DRM_WCHAR    *pwszSeps   = NULL;

    if(     argv        == NULL
      ||    lpCmdLine   == NULL )
    {
        goto ErrorExit;
    }

    do
    {
        DRM_BOOL fQuoted = FALSE;
        DRM_BOOL fEnded  = FALSE;

        /* Skip leading seperator characters */
        while( *pwszCmd )
        {
            for( pwszSeps = rgwcSeps; *pwszSeps && *pwszSeps != *pwszCmd; pwszSeps++ )
            {
                ;
            }

            if (!*pwszSeps)
            {
                break;
            }
            pwszCmd++;
        }

        if( !*pwszCmd )
        {
            argv[nIndex] = NULL;
        }
        else
        {
            if( *pwszCmd == WCHAR_CAST( '\"' ) )
            {
                pwszCmd++;
                fQuoted = TRUE;
            }
            argv[nIndex] = pwszCmd;
        }

        /* Skip over the argument and NULL terminate it */
        for ( ; *pwszCmd && !fEnded ; pwszCmd++ )
        {
            /*
            ** Skip quoted strings first
            */
            if( fQuoted && *pwszCmd == WCHAR_CAST( '\"' ) )
            {
                fEnded = TRUE;
                fQuoted = FALSE;
            }
            else
            {

                for( pwszSeps = rgwcSeps; *pwszSeps && *pwszSeps != *pwszCmd; pwszSeps++ )
                {
                    ;
                }
                if( *pwszSeps )
                {
                    fEnded = TRUE;
                }
            }
            if( fEnded )
            {
                *pwszCmd = '\0';

            }
        }
    } while( argv[nIndex++] != NULL && nIndex < DRM_CMD_MAX_ARGUMENTS );

    if( nIndex < DRM_CMD_MAX_ARGUMENTS )
    {
        nIndex--;
    }

ErrorExit:

    return nIndex;
}

/*********************************************************************
**
**  Function:  DRM_CMD_ParseCmdLine
**
**  Synopsis:  Parse a string for command line arguments.
**
**  Arguments:
**     [pwszArgument]   -- Source string
**     [pwchOptionChar] -- A pointer to a wide character to report the flag given.
**                         This character will be the NULL character if only the param
**                         is given.
**     [f_pdstrParam]      -- The parameter value associated with the option character, may be NULL
**     [f_pdstrExtra]      -- Extra data, may be NULL
**
**  Returns:
**
**  Notes:  Argument syntax is -c[:param[,extra]] or /c[:param[,extra]] or param .
**          If more than one char follows "-" then function returns an error.
**
*********************************************************************/
DRM_BOOL DRM_CALL DRM_CMD_ParseCmdLine(
    __in_opt            const DRM_WCHAR        *pwszArgument,
    __out_ecount_opt(1)       DRM_WCHAR        *pwchOptionChar,
    __out_opt                 DRM_CONST_STRING *f_pdstrParam,
    __out_opt                 DRM_CONST_STRING *f_pdstrExtra )
{
    DRM_BOOL  fRet  = FALSE;
    DRM_DWORD i     = 0;
    DRM_DWORD dwLen = 0;

    if ( pwszArgument == NULL || pwchOptionChar == NULL )
    {
        goto ErrorExit;
    }
    if ( f_pdstrParam != NULL )
    {
        f_pdstrParam->pwszString = NULL;
        f_pdstrParam->cchString = 0;
    }
    if ( f_pdstrExtra != NULL )
    {
        f_pdstrExtra->pwszString = NULL;
        f_pdstrExtra->cchString = 0;
    }

    dwLen = (DRM_DWORD)DRMCRT_wcslen( pwszArgument ); /* excludes terminal null */
    if ( pwszArgument[0] == g_wchMinus
      || pwszArgument[0] == g_wchForwardSlash )
    {
        /*
        ** pwszArgument must be either "-c", or "-c:something", or error i.e. "-something"
        */
        if ( dwLen < 2 ) /* too short */
        {
            goto ErrorExit;
        }

        *pwchOptionChar = pwszArgument[1];

        if ( dwLen == 2 ) /* -c */
        {
            fRet = TRUE;
            goto ErrorExit;
        }

        if ( pwszArgument[2] != g_wchColon ) /* error */
        {
            goto ErrorExit;
        }

        /* parse parameters */
        if ( f_pdstrParam != NULL )
        {
            /* parse first parameter */
            for ( i = 3; i < dwLen; i++ )
            {
                if ( pwszArgument[i] == g_wchNull
                    || pwszArgument[i] == g_wchComma )
                    break;
                ++f_pdstrParam->cchString;
            }
            if ( f_pdstrParam->cchString > 0 )
            {
                f_pdstrParam->pwszString = &pwszArgument[3];
            }

            fRet = TRUE;

            if ( f_pdstrExtra == NULL ) /* done parsing parameters */
            {
                goto ErrorExit;
            }
            else if ( i < dwLen - 1 )
            {
                ++i; /* skip comma */

                /* parse second parameter */
                f_pdstrExtra->pwszString = &pwszArgument[i];
                f_pdstrExtra->cchString = dwLen - i;
            }
                    }
    }
    else
    {
        *pwchOptionChar = WCHAR_CAST('\0');
        if ( f_pdstrParam == NULL )
        {
            goto ErrorExit; /* will return FALSE if parameter is expected but a caller did not pass f_pdstrParam */
        }
        /* parse parameter */
        f_pdstrParam->pwszString = &pwszArgument[0];
        f_pdstrParam->cchString = dwLen;

        fRet = TRUE;
    }

ErrorExit:
    return fRet;
}

/*********************************************************************
**
**  Function:  DRM_CMD_ParseCmdLineMultiChar
**
**  Synopsis:  Parse a string for command line arguments.
**             Same as DRM_CMD_ParseCmdLine except command is can contain multiple characters,
**             not just 1 character
**
**  Arguments:
**     [pwszArgument]   -- Source string
**     [pwszOptionStr]  -- A substring in pwszArgument to report the flag given.
**                         This will be an empty substring if only the param is given.
**     [f_pdstrParam]      -- The parameter value associated with the option string
**     [f_pdstrExtra]      -- Extra data
**
**  Returns:
**
**  Notes:  Argument syntax is -cmd[:param[,extra]] or /cmd[:param[,extra]] or param
**
*********************************************************************/
DRM_BOOL DRM_CALL DRM_CMD_ParseCmdLineMultiChar(
    __in      const DRM_WCHAR        *pwszArgument,
    __out           DRM_CONST_STRING *pwszOptionStr,
    __out_opt       DRM_CONST_STRING *f_pdstrParam,
    __out_opt       DRM_CONST_STRING *f_pdstrExtra )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  i     = 0;
    DRM_DWORD  dwLen = 0;

    ChkArg( pwszArgument != NULL );
    ChkArg( pwszOptionStr != NULL );

    dwLen = (DRM_DWORD)DRMCRT_wcslen( pwszArgument );

    /* parse option command */
    if ( pwszArgument[i] == g_wchMinus || pwszArgument[i] == g_wchForwardSlash )
    {
        ChkBOOL( dwLen >= (i+1), DRM_E_INVALIDARG );
        i++; /* skip dash or forward slash */

        pwszOptionStr->pwszString = &pwszArgument[i];
        pwszOptionStr->cchString = 0;
        for( ; pwszArgument[i]!= g_wchNull && pwszArgument[i]!=g_wchColon; i++ )
        {
            pwszOptionStr->cchString++;
        }
        ChkDRMString( pwszOptionStr );

        ChkBOOL( f_pdstrParam != NULL, DRM_SUCCESS ); /* No parameter processing required - done */
        ChkBOOL( pwszArgument[i]!= g_wchNull, DRM_SUCCESS ); /* No data to process - done */

        /* parse parameter */
        ChkBOOL( dwLen >= (i+1) && pwszArgument[i] == g_wchColon, DRM_E_INVALIDARG );
        i++; /* skip colon */

        f_pdstrParam->pwszString = &pwszArgument[i];
        f_pdstrParam->cchString = 0;

        for( ; pwszArgument[i]!= g_wchNull && pwszArgument[i]!=g_wchComma; i++ )
        {
            f_pdstrParam->cchString++;
        }

        ChkBOOL( f_pdstrExtra != NULL, DRM_SUCCESS ); /* No extra data processing required - done */
        ChkBOOL( pwszArgument[i]!= g_wchNull, DRM_SUCCESS ); /* No data to process - done */

        /* parse the extra parameter */
        ChkBOOL( dwLen >= (i+1) && pwszArgument[i] == g_wchComma, DRM_E_INVALIDARG );
        i++; /* skip comma */

        f_pdstrExtra->pwszString = &pwszArgument[i];
        f_pdstrExtra->cchString = 0;
        for( ; pwszArgument[i]!=g_wchNull; i++ )
        {
            f_pdstrExtra->cchString++;
        }
    }
    else
    {
        ChkArg( f_pdstrParam != NULL );

        /*
        ** Handle just being provided a param
        */
        pwszOptionStr->pwszString = NULL;
        pwszOptionStr->cchString = 0;

        f_pdstrParam->pwszString = &pwszArgument[i];
        f_pdstrParam->cchString = 0;

        for( ; pwszArgument[i]!= g_wchNull && pwszArgument[i]!=g_wchComma; i++ )
        {
            f_pdstrParam->cchString++;
        }
    }

ErrorExit:
    return DRM_SUCCEEDED( dr );
}


/*********************************************************************
**
**  Function:  DRM_CMD_TryProcessDefaultOption
**
**  Synopsis:  Looks for parameters known to PK DRM and acts on them.
**             Known parameters are "-drmpath"
**
**  Arguments:
**     [pdstrOption]     -- A string to report the flag given.
**                         This will be an empty substring if only the param is given.
**     [f_pdstrParam]      -- The parameter value associated with the option string
**     [f_pdstrExtra]      -- Extra data
**
**  Returns:
**      DRM_SUCCESS     -- recognized a parameter as a known default
**                         and succeeded to apply settings
**      DRM_E_INVALID_COMMAND_LINE    -- option is unknown or required parameter is missing.
**      DRM_E_...       -- everything else
**  Notes:  Argument syntax is -cmd[:param[,extra]] or /cmd[:param[,extra]] or param
**
*********************************************************************/

DRM_RESULT DRM_CALL DRM_CMD_TryProcessDefaultOption(
    __in const DRM_WCHAR  *f_wszArgument
)
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_CONST_STRING dstrOption   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrParam    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrExtra    = EMPTY_DRM_STRING;
    DRM_WCHAR       *pwszNewPath  = NULL;

    ChkArg( f_wszArgument != NULL );

    if ( !DRM_CMD_ParseCmdLineMultiChar( f_wszArgument,
                                         &dstrOption,
                                         &dstrParam,
                                         &dstrExtra ) )
    {
        ChkArg( DRM_E_INVALID_COMMAND_LINE );
    }

    if ( DRM_UTL_DSTRStringsEqual( &dstrOption, &g_dstrDRMPATH_OPTION_STRING ) )
    {
        /*
        ** Expecting -drmpath:<path overriding C:\WMDRMPD>
        */
        ChkBOOL( dstrParam.pwszString != NULL && dstrParam.cchString > 0, DRM_E_INVALID_COMMAND_LINE );

        ChkMem( pwszNewPath = (DRM_WCHAR*)Oem_MemAlloc( (dstrParam.cchString + 1) * SIZEOF( DRM_WCHAR ) ) );
        ChkDR( DRM_STR_StringCchCopyNW( pwszNewPath,
                                        dstrParam.cchString + 1,
                                        dstrParam.pwszString,
                                        dstrParam.cchString ) );

        pwszNewPath[dstrParam.cchString] = 0;
        g_dstrDrmPath.pwszString = pwszNewPath;
        g_dstrDrmPath.cchString = dstrParam.cchString;
    }
    else
    {
        ChkArg(DRM_E_INVALID_COMMAND_LINE);
    }
ErrorExit:
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
