/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <toem_paths.h>
#include <tOEMIMP.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

/*
** The default file and fuzz paths may be overridden by the test code
*/
static const DRM_WCHAR g_rgwchLocalPath [] =
{
    ONE_WCHAR('.', '\0'),  ONE_WCHAR('\\', '\0'), ONE_WCHAR('\0', '\0')
};

DRM_CONST_STRING g_dstrFilePath = CREATE_DRM_STRING( g_rgwchLocalPath );

static DRM_CHAR  g_rgchDataPath[DRM_MAX_PATH] = { '\0' };
static DRM_CHAR  g_rgchRunPath[DRM_MAX_PATH]  = { '\0' };


/*********************************************************
**
** Function: TST_OEM_InitPaths
**
** Synopsis: sets up the path variables for the platform.
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


/* zgpriv.dat */
const DRM_WCHAR GC_PRIVATE_KEY_FILE_NAME [] = {
    ONE_WCHAR('z', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('r', '\0'),
    ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'),
    ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* bgroupcert.dat */
const DRM_WCHAR GROUP_CERT_FILE_NAME [] = {
    ONE_WCHAR('b', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('o', '\0'),
    ONE_WCHAR('u', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'),
    ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'),
    ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* priv.dat */
const DRM_WCHAR PRIVATE_KEY_FILE_NAME[] = {
    ONE_WCHAR('p', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'),
    ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0') };

/* devcerttemplate.dat */
const DRM_WCHAR DEVCERT_TEMPLATE_FILE_NAME [] = {
    ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'),
    ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')};

/* bdevcert.dat */
const DRM_WCHAR BDEVCERT_FILE_NAME [] = {
    ONE_WCHAR('b', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'),
    ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0') };

/* zprivsig.dat Needed by PKCommonTestAPI.c */
const DRM_WCHAR Z_PRIVATE_KEY_FILE_NAME_SIGN[] = {
    ONE_WCHAR('z', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'),
    ONE_WCHAR('v', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('g', '\0'),
    ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0') };

/* zprivencr.dat Needed by PKCommonTestAPI.c */
const DRM_WCHAR Z_PRIVATE_KEY_FILE_NAME_ENCRYPT[] = {
    ONE_WCHAR('z', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'),
    ONE_WCHAR('v', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('c', '\0'),
    ONE_WCHAR('r', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'),
    ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* devcert.dat */
const DRM_WCHAR DEVCERT_FILE_NAME [] = {
    ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* ndrcert.dat Needed by tOEMImp.c */
const DRM_WCHAR NDR_CERT[] = {
    ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* ndrcerttemplate.dat Needed by tOEMImp_PD.c */
const DRM_WCHAR NDR_MODEL_CERT[] = {
    ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'),
    ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')};

/* ndrgpriv.dat Needed by tOEMImp_PD.c */
const DRM_WCHAR NDR_MODEL_PRIVATE_KEY[] = {
    ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('g', '\0'),
    ONE_WCHAR('p', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'),
    ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0') };

/* ndrpriv.dat Needed by tOEMImp.c */
const DRM_WCHAR NDR_PRIVATE_KEY[] ={
    ONE_WCHAR('n', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('p', '\0'),
    ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* keyfile.dat */
const DRM_WCHAR KEYFILE_FILE_NAME[] =
{
    ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('f', '\0'),
    ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')
};

/* wmdrmpd.dat */
const DRM_WCHAR WMDRMPD_KEY_FILE_NAME [] = {
    ONE_WCHAR('w', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('r', '\0'),
    ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* store.dat */
const DRM_WCHAR DEVICE_STORE_FILE_NAME[] = {
    ONE_WCHAR('s', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('r', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'),
    ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };


    
static DRM_RESULT _GetDRMFileName(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *pdstrPathname,
    __in                                  const DRM_WCHAR           *wszFileName )
{
    DRM_RESULT        dr        = DRM_SUCCESS;
    DRM_DWORD         cchCopy   = 0;
    DRM_CONST_STRING *pdstrPath = &g_dstrDrmPath;


    ZEROMEM(wszPathName, GET_DRM_FILE_NAME_PATH_LENGTH * sizeof(DRM_WCHAR));

    if ( pdstrPathname != NULL )
    {
        ChkDR( DRM_STR_StringCchCopyNW( wszPathName,
                                       GET_DRM_FILE_NAME_PATH_LENGTH,
                                       pdstrPath->pwszString,
                                       pdstrPath->cchString ) );

        pdstrPathname->cchString = pdstrPath->cchString;

        /* The path must have a trailing "\" */
        if( wszPathName[ pdstrPath->cchString - 1 ] != g_wchPathSeparator )
        {
            wszPathName[ pdstrPath->cchString ] = g_wchPathSeparator;
            wszPathName[ pdstrPath->cchString + 1 ] = g_wchNull;
            pdstrPathname->cchString = pdstrPath->cchString + 1;
        }

        /* Add file name if specified */
        if ( wszFileName != NULL )
        {
            cchCopy = ( DRM_DWORD ) min( DRMCRT_wcslen( wszFileName ), GET_DRM_FILE_NAME_PATH_LENGTH - pdstrPathname->cchString - 1 );
            DRM_BYT_CopyBytes( ( DRM_BYTE * ) ( &wszPathName[0] ), SIZEOF( DRM_WCHAR ) * pdstrPathname->cchString, ( DRM_BYTE * ) (&wszFileName[0]), 0, SIZEOF( DRM_WCHAR ) * cchCopy );
            wszPathName[cchCopy + pdstrPathname->cchString] = g_wchNull;
        }

        pdstrPathname->pwszString = &wszPathName[0];
        pdstrPathname->cchString  = ( DRM_DWORD ) DRMCRT_wcslen(wszPathName);
    }

ErrorExit:
    return dr;
}


    
DRM_RESULT tGetDRMFileName(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *f_pdstrPathname,
    __in                                  const DRM_WCHAR           *f_wszFileName )
{
    return _GetDRMFileName( f_wszPathName, f_pdstrPathname, f_wszFileName );
}

DRM_RESULT DRM_CALL tGetDeviceStorePathname(
                                            __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
                                            __out                                       DRM_CONST_STRING    *f_pdstrPathname )
{
    return _GetDRMFileName( f_wszPathName, f_pdstrPathname, DEVICE_STORE_FILE_NAME );
}

DRM_RESULT DRM_CALL tGetDeviceCertPathname(
                                           __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
                                           __out                                       DRM_CONST_STRING    *f_pdstrPathname )
{
    return _GetDRMFileName( f_wszPathName, f_pdstrPathname, DEVCERT_FILE_NAME );
}



EXIT_PKTEST_NAMESPACE_CODE;

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

