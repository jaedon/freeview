/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h>

#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcmdlnpars.h>
#include <drmcallbacks.h>
#include <drmheaderparser.h>
#include <drmxmlparser.h>
#include <drmconstants.h>

#include <drmtoolsconstants.h>
#include <drmtoolsmacros.h>
#include <drmtoolsutils.h>
#include <drmtoolsmediafile.h>
#include <drmtoolsinit.h>

#if DRM_SUPPORT_PROFILING
#include <drmprofile.h>
#endif


/****************************************
** Holds all the info used by the tool **
****************************************/
typedef struct _tagDRM_ASFVERIFYCONTEXT
{
    DRM_BOOL                fVerbose;
    DRM_BOOL                fCheckHDS;
    DRM_CONST_STRING       *pdstrOutputFile;
    DRM_CONST_STRING       *pdstrDataStoreFile;
    DRM_APP_CONTEXT        *poManagerContext;
    DRM_BYTE               *pbOpaqueBuffer;
    DRM_MEDIA_FILE_CONTEXT  oMediaFile;
} DRM_ASFVERIFYCONTEXT;

#define EMPTY_DRM_ASFVERIFYCONTEXT  {                              \
                                    FALSE,                         \
                                    FALSE,                         \
                                    NULL,                          \
                                    NULL,                          \
                                    NULL,                          \
                                    NULL,                          \
                                    EMPTY_DRM_MEDIA_FILE_CONTEXT }

/*********************************************************************
**
** Function: _PrintUsage
**
** Synopsis: Function that shows how the command line syntax of using
**           the application.
**
** Arguments:
**
** [f_pwszAppName]          -- Pointer to a wide character string of the
**                             application executable.
**
** Returns:                 N/A
**
**********************************************************************/
static DRM_VOID _PrintUsage(
    __in const DRM_WCHAR * f_pwszAppName )
{
    printf( "Syntax: %S [-?] -c:ContentFile [-s:StoreName] [-o:XMLV4HeaderOutputFile] [-v (turns on verbose mode)] \n", f_pwszAppName );

}


/******************************************************************************
** Handle callbacks from Drm_Reader_Bind
*******************************************************************************
*/
DRM_RESULT DRM_CALL BindCallback(
    __in const DRM_VOID                 *f_pvPolicyCallbackData,
    __in       DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
    __in const DRM_VOID                 *f_pv )
{
    return DRMTOOLS_PrintPolicyCallback( f_pvPolicyCallbackData, f_dwCallbackType );
}

/*********************************************************************
**
** Function: _VerifyHeader
**
** Synopsis: Makes sure an XML header is readable and contains all required tags.
**
** Arguments:
**
** [pbXMLHeader]                       -- Buffer containing the data to write.
** [cbXMLHeader]                       -- Size of the buffer.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_FILE_WRITE_ERROR       -- If there is a problem writing to the file
**
** Note:    This currently uses PlayReady XML Parsing to verify header.
**          However, once the DRM_HDR_* APIs are able to parse all the
**          V4 and "V2.4" header attributes, this function should be
**          updated to call into DRM_HDR_* APIs for verification.
**
**********************************************************************/
static DRM_RESULT _VerifyHeader( __in const DRM_BYTE         *pbXMLHeader,
                                 __in       DRM_DWORD         cbXMLHeader,
                                 __in       DRM_BOOL          fIsV4,
                                 __in       DRM_BOOL          fVerbose )
{
    DRM_RESULT       dr              = DRM_E_LOGICERR;
    DRM_CONST_STRING dstrValue       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrData        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrV4Data      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrProtectInfo = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrHeader      = EMPTY_DRM_STRING;

    ChkArg( pbXMLHeader != NULL );
    ChkArg( cbXMLHeader != 0 );

    dstrHeader.pwszString = (const DRM_WCHAR *)pbXMLHeader;
    dstrHeader.cchString  = cbXMLHeader / SIZEOF( DRM_WCHAR );

    /* Ensure the outter tag is WRMHEADER */
    if( DRM_FAILED( DRM_XML_GetNode( &dstrHeader, &g_dstrTagWrmHeader, NULL, NULL, 0, NULL, &dstrData ) ) )
    {
        printf( "\nHeader is invalid: Cannot locate WRMHEADER tag.\n" );
        ChkDR( DRM_E_CH_INVALID_HEADER );
    }

    /* Make sure there is a version number */
    if( DRM_FAILED( DRM_XML_GetNodeAttribute( &dstrHeader, &g_dstrAttributeVersion, &dstrData ) ) ||
        dstrData.cchString == 0 )
    {
        printf( "\nHeader is invalid: Cannot read WRMHEADER version number.\n" );
        ChkDR( DRM_E_CH_VERSION_MISSING );
    }

    /* Make sure we understand the version of the header */
    if( DRM_UTL_DSTRStringsEqual( &g_dstrAttributeVersion2Value, &dstrData ) && !fIsV4 )
    {
        if ( fVerbose )
        {
            printf( "\tV2 Header contains proper version number.\n" );
        }
    }
    else if ( DRM_UTL_DSTRStringsEqual( &g_dstrAttributeVersion4Value, &dstrData ) && fIsV4 )
    {
        if ( fVerbose )
        {
            printf( "\tV4 Header contains proper version number.\n" );
        }
    }
    else
    {
        printf( "\nHeader is invalid: Version number does not match expected value.\n" );
        /* PlayReady only supports V2 and V4 XML Headers */
        ChkDR( DRM_E_CH_UNSUPPORTED_VERSION );
    }

    /* Get the data section. */
    if( DRM_FAILED( DRM_XML_GetSubNode( &dstrHeader, &g_dstrTagData, NULL, NULL, 0, &dstrData, NULL, 1 ) ) )
    {
        printf( "\nHeader is invalid: Cannot find Data tag.\n" );
        ChkDR( DRM_E_CH_INVALID_HEADER );
    }
    else if ( fVerbose )
    {
        printf( "\tData Tag Found.\n" );
    }

    /* Get the KID */
    if( DRM_FAILED( DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrTagKID, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) )
    {
        printf( "\nHeader is invalid: Cannot read KID.\n" );
        ChkDR( DRM_E_CH_KID_MISSING );
    }
    else if ( fVerbose )
    {
        printf( "\t\tKID Found.\n" );
    }

    /* Get the Checksum */
    if( DRM_FAILED( DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrTagChecksum, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) )
    {
        if( !fIsV4 )
        {
            printf( "\nHeader is invalid: Cannot read Checksum.\n" );
            ChkDR( DRM_E_CH_CHECKSUM_MISSING );
        }
    }
    else if ( fVerbose )
    {
        printf( "\t\tChecksum Found.\n" );
    }

    if ( fIsV4 )
    {
        /* Check for V4-specific tags */

        /* Look for the Service ID tag (optional) */
        if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrTagDSID, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) && fVerbose )
        {
            printf( "\t\tService ID Found.\n" );
        }

        /* Look for the Protection Info tag (optional) */
        if( DRM_FAILED( DRM_XML_GetSubNode( &dstrData, &g_dstrTagProtectInfo, NULL, NULL, 0, &dstrProtectInfo, NULL, 1 ) ) )
        {
            if ( fVerbose )
            {
                printf( "\tV4 Header warning: Header contains no Protect Info.\n" );
            }
        }
        else
        {
            if ( fVerbose )
            {
                printf( "\t\tProtect Info Found.\n" );
            }

            /* Look for the Key Length (optional) */
            if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrProtectInfo, &g_dstrTagKeyLen, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) && fVerbose )
            {
                printf( "\t\t\tKey Length Found.\n" );
            }

            /* Look for the Alg ID (optional) */
            if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrProtectInfo, &g_dstrTagAlgID, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) && fVerbose )
            {
                printf( "\t\t\tAlg ID Found.\n" );
            }
        }
        /* Look for the V4 LAURL (optional) */
        if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrTagV24LAURL, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) && fVerbose )
        {
            printf( "\t\tLA_URL Found.\n" );
        }
        /* Look for the V4 LUIURL (optional) */
        if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrTagV24LUIURL, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) && fVerbose )
        {
            printf( "\t\tLUI_URL Found.\n" );
        }
        printf( "\nThis content contains a valid V4 Header.\n" );
    }

    else
    {
        /* Look for the  (V2) LAINFO */
        if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrTagLAINFO, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) && fVerbose )
        {
            printf( "\t\t(V2) LAURL Found.\n" );
        }

        /* Check for V2.4 specific tags */

        /* Look for the V4Data tag found in "V2.4" Headers */
        if( DRM_FAILED( DRM_XML_GetSubNode( &dstrData, &g_dstrTagV4DATA, NULL, NULL, 0, &dstrV4Data, NULL, 1 ) ) )
        {
            if ( fVerbose )
            {
                printf( "\nV2 Header Warning: Cannot find V4Data tag.  This content is not PlayReady.\n" );
            }
            printf( "\nThis content contains a valid V2 (not V2.4) Header.\n" );
        }
        else
        {
            if ( fVerbose )
            {
                printf( "\t\tV4Data Found.\n" );
            }
            /* Get the V4-style LA_URL (optional) */
            if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrV4Data, &g_dstrTagV24LAURL, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) && fVerbose )
            {
                printf( "\t\t\tLA_URL Found.\n" );
            }
            /* Get the V4-style LUI_URL (optional) */
            if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrV4Data, &g_dstrTagV24LUIURL, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) && fVerbose )
            {
                printf( "\t\t\tLUI_URL Found.\n" );
            }

            /* Get the V4-style DS_ID (optional) */
            if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( &dstrV4Data, &g_dstrTagDSID, NULL, NULL, NULL, &dstrValue, g_wchForwardSlash ) ) && fVerbose )
            {
                printf( "\t\t\tDomain Service ID Found.\n" );
            }
            printf( "\nThis content contains a valid V2.4 Header.\n" );
        }
    }

    /* Enough fields were present.  Basic checks pass */
    dr = DRM_SUCCESS;
ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _CheckForLicenses
**
** Synopsis: Checks for licenses corresponding to the header.
**
** Arguments:
**
** [poToolContext]                     -- The tool context.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**          DRM_E_FILEOPEN             -- If there is a problem opening the file
**
**********************************************************************/
static DRM_RESULT _CheckForLicenses( __inout DRM_ASFVERIFYCONTEXT *poToolContext )
{
    DRM_RESULT             dr               = DRM_SUCCESS;
    const DRM_CONST_STRING *apdcsRights [1] = { NULL };

    ChkArg( poToolContext != NULL );

    apdcsRights[0] = &g_dstrWMDRM_RIGHT_PLAYBACK;

    if (    poToolContext->oMediaFile.eFileType == eDRM_MEDIA_FILE_TYPE_ASF
        &&  poToolContext->oMediaFile.FormatContext.oAsfFile.oFoundObjects.fFoundV1
        && !poToolContext->oMediaFile.FormatContext.oAsfFile.oFoundObjects.fFoundV2
        && !poToolContext->oMediaFile.FormatContext.oAsfFile.oFoundObjects.fFoundV4 )
    {
        ChkDR( Drm_Content_SetProperty( poToolContext->poManagerContext,
                                        DRM_CSP_AUTODETECT_HEADER,
                                        ( DRM_BYTE * ) &poToolContext->oMediaFile.FormatContext.oAsfFile.oV1Data,
                                        SIZEOF( DRM_CSP_V1_HEADER_DATA ) ) );
    }
    else
    {
        ChkDR( Drm_Content_SetProperty( poToolContext->poManagerContext,
                                        DRM_CSP_AUTODETECT_HEADER,
                                        poToolContext->oMediaFile.rgbDRMHeaderData,
                                        poToolContext->oMediaFile.cbDRMHeaderData ) );
    }
    dr = Drm_Reader_Bind ( poToolContext->poManagerContext,
                           apdcsRights,
                           NO_OF (apdcsRights),
                           (DRMPFNPOLICYCALLBACK)BindCallback,
                           NULL,
                           &(poToolContext->oMediaFile.oDecryptContext) );

    if ( dr == DRM_E_LICENSE_NOT_FOUND )
    {
        printf( "No license is found which matches this header.\n" );
    }
    else if ( dr == DRM_E_LICENSE_EXPIRED )
    {
        printf( "There is an expired license which matches this header.\n" );
    }
    else if ( dr == DRM_SUCCESS )
    {
        printf( "There is a license which matches this header.\n" );
    }
    else
    {
        printf( "Error looking for matching license.\n" );
        ChkDR(dr);
    }

    /* Do not return any expected error codes returned by Drm_Reader_Bind */
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _ReadProtectedContent
**
** Synopsis: Opens a protected ASF file, parses it, and verifies the headers.
**
** Arguments:
**
** [f_pdstrContentFile]                -- The name of the file to read.
** [poToolContext]                     -- The tool context.
**
** Returns:
**          DRM_SUCCESS                -- On success
**          DRM_INVALIDARG             -- If any arguments are invalid (NULL pointers)
**          DRM_E_ARITHMETIC_OVERFLOW  -- If the total bytes read becomes too long
**          DRM_E_FILE_READ_ERROR        -- If there is a problem reading the file
**          DRM_E_FILEOPEN             -- If there is a problem opening the file
**
**********************************************************************/
static DRM_RESULT _ReadProtectedContent( __in    const DRM_CONST_STRING     *f_pdstrContentFile,
                                         __inout       DRM_ASFVERIFYCONTEXT *poToolContext )
{
    DRM_RESULT          dr                 = DRM_SUCCESS;

    ChkDRMString( f_pdstrContentFile );
    ChkArg( poToolContext != NULL );

    poToolContext->oMediaFile.FormatContext.oAsfFile.fVerbose = poToolContext->fVerbose;
    /* Open protected content */
    ChkDR( DRMTOOLS_OpenMediaFile( f_pdstrContentFile, &poToolContext->oMediaFile ) );


    if( poToolContext->fCheckHDS )
    {
        ChkDRMString( poToolContext->pdstrDataStoreFile );

        ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, poToolContext->pdstrDataStoreFile, TOOLS_DRM_BUFFER_SIZE, &poToolContext->pbOpaqueBuffer, &poToolContext->poManagerContext ) );
    }
    if (   poToolContext->oMediaFile.eFileType == eDRM_MEDIA_FILE_TYPE_ENVELOPE
        || poToolContext->oMediaFile.FormatContext.oAsfFile.oFoundObjects.fFoundV4 )
    {
        ChkDR( _VerifyHeader( poToolContext->oMediaFile.rgbDRMHeaderData, poToolContext->oMediaFile.cbDRMHeaderData, TRUE, poToolContext->fVerbose ) );
    }
    else
    {
        ChkDR( _VerifyHeader( poToolContext->oMediaFile.rgbDRMHeaderData, poToolContext->oMediaFile.cbDRMHeaderData, FALSE, poToolContext->fVerbose ) );
    }
    if( poToolContext->fCheckHDS )
    {
        /* Check for a matching license */
        ChkDR( _CheckForLicenses( poToolContext ) );
    }

    if( poToolContext->pdstrOutputFile->cchString != 0 )
    {
        ChkDR( DRMTOOLS_SaveContentHeaderToFile( &poToolContext->oMediaFile, poToolContext->pdstrOutputFile ) );
    }

ErrorExit:
    if ( poToolContext != NULL )
    {
        DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &poToolContext->pbOpaqueBuffer, &poToolContext->poManagerContext );
        DRMTOOLS_CloseMediaFile( &poToolContext->oMediaFile );
        SAFE_OEM_FREE(poToolContext->poManagerContext);
    }

    return dr;
}


/*********************************************************************
**
** Function: DRM_Main
**
** Synopsis: The DRM_Main function.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- An array of arguments.
**
** Returns: DRM_SUCCESS     -- on success
**
**********************************************************************/
#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:6262) /* Temporarily disable stack limit check. */
#endif
DRM_LONG DRM_CALL DRM_Main(
    __in                    DRM_LONG argc,
    __in_ecount_opt( argc ) DRM_WCHAR** argv )
{
    DRM_RESULT           dr              = DRM_SUCCESS;
    DRM_ASFVERIFYCONTEXT oToolContext    = EMPTY_DRM_ASFVERIFYCONTEXT;
    DRM_CONST_STRING     dstrContentFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrStoreName   = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrOutputFile  = EMPTY_DRM_STRING;

    DRM_LONG i;

    PrintCopyrightInfo( "ASF DRM Verify" );

    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }
    if ( argc > 5 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    ChkArg( argv != NULL );
    for ( i = 1; i < argc; i++ )
    {
        DRM_WCHAR wchOption;
        DRM_CONST_STRING dstrParam;

        if ( !DRM_CMD_ParseCmdLine( argv[ i ], &wchOption, &dstrParam, NULL ) )
        {
            /* See if it's a default parameter that PK knows how to handle */
            if ( DRM_FAILED( DRM_CMD_TryProcessDefaultOption( argv[i] ) ) )
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        }
        else
        {
            wchOption = DRMCRT_towlower( wchOption );

            switch ( wchOption )
            {
                case WCHAR_CAST( 'c' ):
                    dstrContentFile.pwszString = dstrParam.pwszString;
                    dstrContentFile.cchString  = dstrParam.cchString;
                    break;

                case WCHAR_CAST( 's' ):
                    dstrStoreName.pwszString = dstrParam.pwszString;
                    dstrStoreName.cchString  = dstrParam.cchString;
                    oToolContext.fCheckHDS   = TRUE;
                    break;

                case WCHAR_CAST( 'o' ):
                    dstrOutputFile.pwszString = dstrParam.pwszString;
                    dstrOutputFile.cchString  = dstrParam.cchString;
                    break;

                case WCHAR_CAST( 'v' ):
                    oToolContext.fVerbose = TRUE;
                    break;

                case WCHAR_CAST( '?' ):
                    goto _PrintUsage;

                default:
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
            }
        }
    }

    if( dstrContentFile.cchString == 0 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    oToolContext.pdstrOutputFile = &dstrOutputFile;
    oToolContext.pdstrDataStoreFile = &dstrStoreName;

#if DRM_SUPPORT_PROFILING
    ChkDR( DRM_Start_Profiling( NULL, NULL ) );
#endif

    ChkDR( _ReadProtectedContent( &dstrContentFile, &oToolContext ) );

ErrorExit:
    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success!\r\n" );
    }
    else
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }
    return dr;

_PrintUsage:

    _PrintUsage( argv == NULL ? &g_wchNull : argv[ 0 ] );
    return dr;
}
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif
