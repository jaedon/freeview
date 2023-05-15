/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <string.h>
#include "toem_paths.h"
#include "tOEMImp.h"

ENTER_PKTEST_NAMESPACE_CODE

/*
** The default file and fuzz paths may be overridden by the test code
*/
#if WINCE_TEST
/*
** \windows\drmtest\
*/
static const DRM_WCHAR g_rgwchLocalPath [] =
{
    ONE_WCHAR( '\\', '\0' ), ONE_WCHAR( 'w',  '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'n', '\0' ), 
    ONE_WCHAR( 'd',  '\0' ), ONE_WCHAR( 'o',  '\0' ), ONE_WCHAR( 'w', '\0' ), ONE_WCHAR( 's', '\0' ), 
    ONE_WCHAR( '\\', '\0' ), ONE_WCHAR( 'd',  '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'm', '\0' ),
    ONE_WCHAR( 't',  '\0' ), ONE_WCHAR( 'e',  '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( 't', '\0' ),
    ONE_WCHAR( '\\', '\0' ), ONE_WCHAR( '\0', '\0' )
};
#else
/*
** .\
*/
static const DRM_WCHAR g_rgwchLocalPath [] =
{
    ONE_WCHAR('.', '\0'),  ONE_WCHAR('\\', '\0'), ONE_WCHAR('\0', '\0')
};
#endif

DRM_CONST_STRING g_dstrFilePath = CREATE_DRM_STRING( g_rgwchLocalPath );

static DRM_CHAR  g_rgchDataPath[DRM_MAX_PATH] = { '\0' };
static DRM_CHAR  g_rgchRunPath[DRM_MAX_PATH]  = { '\0' };


/*********************************************************
** 
** Function: TST_OEM_InitPaths
** 
** Synopsis: sets up the path variables for the platform.
** 
**
** Returns     DRM_SUCCESS
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_InitPaths()
{
    DRM_RESULT dr               = DRM_SUCCESS;

    DRM_UTL_DemoteUNICODEtoASCII( g_dstrFilePath.pwszString, g_rgchDataPath, SIZEOF( g_rgchDataPath ) - 2 );
    ChkDR( DRM_STR_StringCchCopyA( g_rgchRunPath, SIZEOF( g_rgchRunPath ) - 2, ".\\" ) );
    
ErrorExit:
    return dr;
}


/*********************************************************
** 
** Function: TST_OEM_GetTestDataDirectory
** 
** Synopsis: Gets the path holding the test data. 
**           On iOS, this is the App Bundle directory.
** 
** Arguments:
**  f_pszDataDirectory -- Char array to receive the directory.
**
** Returns     DRM_SUCCESS
**             DRM_E_TEST_INVALIDARG 
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_GetTestDataDirectory( __inout_z DRM_CHAR f_pszDataDirectory[DRM_MAX_PATH]  )
{
    DRM_RESULT dr = DRM_SUCCESS;
    
    ChkArg( NULL != f_pszDataDirectory );
    
    MEMCPY( f_pszDataDirectory, g_rgchDataPath, DRM_MAX_PATH );
    
ErrorExit:
    return dr;
}

/*********************************************************
** 
** Function: TST_OEM_GetTestRunDirectory
** 
** Synopsis: Gets the directory for use during test execution.
**           On iOS, this is the App Documents directory.
** 
** Arguments:
**  f_pszRunDirectory -- Char array to receive the directory.
**
** Returns     DRM_SUCCESS
**             DRM_E_TEST_INVALIDARG 
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_GetTestRunDirectory( __inout_z DRM_CHAR f_pszRunDirectory[DRM_MAX_PATH]  )
{
    DRM_RESULT dr = DRM_SUCCESS;
        
    ChkArg( NULL != f_pszRunDirectory );
    
    MEMCPY( f_pszRunDirectory, g_rgchRunPath, DRM_MAX_PATH );
    
    
ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE