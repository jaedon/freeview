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
#include <time.h>

#if !defined( ARM )
#include <errno.h>
#else
#define  ENOENT 2
#endif

#include <drmwindowsenv.h>
#include <tOEMIMP.h>
#include <tclib.h>
#include <drmcontextsizes.h>
#include <tstutils.h>
#include <drmcrt.h>
#include <oemkeyfile.h>
#include <drmkeyfile.h>
#include <drmblackbox.h>
#include <drmerror.h>
#include <drmwmdrm.h>
#include <drmmathsafe.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_FUNCTION_NEEDS_REVIEW, "Ignore prefast warnings about reviewing CreateProcessA parameters since this is test code.");

ENTER_PKTEST_NAMESPACE_CODE;

#define MAX_COPY_BUFFER 12000

/* devcert.dat */
const DRM_WCHAR DEVCERT_FILE_NAME [] = {
    ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* bdevcert.dat */
const DRM_WCHAR BDEVCERT_FILE_NAME [] = {
    ONE_WCHAR('b', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'),
    ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0') };


/* devcerttemplate.dat */
const DRM_WCHAR DEVCERT_TEMPLATE_FILE_NAME [] = {
    ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'),
    ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* bgroupcert.dat */
const DRM_WCHAR GROUP_CERT_FILE_NAME [] = {
    ONE_WCHAR('b', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('o', '\0'),
    ONE_WCHAR('u', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'),
    ONE_WCHAR('r', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'),
    ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };


/* priv.dat */
const DRM_WCHAR PRIVATE_KEY_FILE_NAME [] = {
    ONE_WCHAR('p', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'),
    ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0') };


/* wmdrmpd.dat */
const DRM_WCHAR WMDRMPD_KEY_FILE_NAME [] = {
    ONE_WCHAR('w', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('r', '\0'),
    ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };


/* zgpriv.dat */
const DRM_WCHAR GC_PRIVATE_KEY_FILE_NAME [] = {
    ONE_WCHAR('z', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('r', '\0'),
    ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'),
    ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };

/* zprivsig.dat */
const DRM_WCHAR Z_PRIVATE_KEY_FILE_NAME_SIGN [] = {
    ONE_WCHAR('z', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),
    ONE_WCHAR('v', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('g', '\0'),
    ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0') };

/* zprivencr.dat */
const DRM_WCHAR Z_PRIVATE_KEY_FILE_NAME_ENCRYPT [] = {
    ONE_WCHAR('z', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),
    ONE_WCHAR('v', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('c', '\0'),
    ONE_WCHAR('r', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),
    ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0') };

/* DRMDeviceStore.hds */
const DRM_WCHAR DEVICE_STORE_FILE_NAME[] = {
    ONE_WCHAR('D', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('D', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('o', '\0'),
    ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('h', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0') };

/* ndrcerttemplate.dat */
const DRM_WCHAR NDR_MODEL_CERT [] = {
    ONE_WCHAR('n', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('t', '\0'),
    ONE_WCHAR('e', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('l', '\0'),
    ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

/* ndrgpriv.dat */
const DRM_WCHAR NDR_MODEL_PRIVATE_KEY [] = {
    ONE_WCHAR('n', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('g', '\0'),
    ONE_WCHAR('p', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('v', '\0'),
    ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),
    ONE_WCHAR('\0', '\0')
};

/* ndrcert.dat */
const DRM_WCHAR NDR_CERT [] =
{
    ONE_WCHAR('n', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('c', '\0'),
    ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

/* ndrpriv.dat */
const DRM_WCHAR NDR_PRIVATE_KEY [] = {
    ONE_WCHAR('n', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('p', '\0'),
    ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

/* keyfile.dat */
const DRM_WCHAR KEYFILE_FILE_NAME[] =
{
    ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('f', '\0'),
    ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')
};


/*********************************************************
**
** Function: TST_OEM_IsConditionTrue
**
** Synopsis: Platform specific condition checking. Called by
**           TST_UTL_IsConditionTrue.
**
** Arguments:
**            f_pfCondition  -- TRUE if the codition matches
**            f_pszCondition -- String repesenting the condition
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_IsConditionTrue( __out DRM_BOOL       *f_pfCondition,
                                             __in  const DRM_CHAR *f_pszCondition )
{
    DRM_RESULT      dr                = DRM_SUCCESS;
    const DRM_CHAR *pszKeyFileUpgrade = "KeyFileUpgradeSupported";
    const DRM_CHAR *pszDomainSupport  = "DomainSupported";

    ChkArg( NULL != f_pfCondition );
    ChkArg( NULL != f_pszCondition );

    if( 0 == strcmp( f_pszCondition, pszKeyFileUpgrade ) )
    {
        *f_pfCondition = TRUE;
    }
    else if  ( 0 == strcmp( f_pszCondition, pszDomainSupport ) )
    {
        *f_pfCondition = TRUE;
    }

ErrorExit:
    return dr;
}

void DRM_CALL tDRMSleep(DRM_DWORD dwMillSecond)
{
    Sleep(dwMillSecond);
}

DRM_BOOL DRM_CALL tMoveFile(const WCHAR *wszSrc, const WCHAR *wszTarget)
{
    SetFileAttributesW(wszTarget, FILE_ATTRIBUTE_NORMAL);
    return MoveFileW(wszSrc, wszTarget);
}

static DRM_RESULT _GetDRMFileName(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *pdstrPathname,
    __in                                  const DRM_WCHAR           *wszFileName )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_DWORD        cchCopy          = 0;

    ZEROMEM(wszPathName, GET_DRM_FILE_NAME_PATH_LENGTH * sizeof(DRM_WCHAR));

    if( pdstrPathname != NULL )
    {
        ChkDR( DRM_STR_StringCchCopyNW( wszPathName,
                        GET_DRM_FILE_NAME_PATH_LENGTH,
                        g_dstrDrmPath.pwszString,
                        g_dstrDrmPath.cchString ) );

        pdstrPathname->cchString = g_dstrDrmPath.cchString;

        /* The path must have a trailing "\" */
        if( wszPathName[ g_dstrDrmPath.cchString - 1 ] != g_wchPathSeparator )
        {
            wszPathName[ g_dstrDrmPath.cchString ] = g_wchPathSeparator;
            wszPathName[ g_dstrDrmPath.cchString + 1 ] = g_wchNull;
            pdstrPathname->cchString = g_dstrDrmPath.cchString + 1;
        }

        /* Add file name if specified */
        if( wszFileName != NULL )
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszFileName ), &cch ) );
            cchCopy = ( DRM_DWORD ) min( cch, GET_DRM_FILE_NAME_PATH_LENGTH - pdstrPathname->cchString - 1 );
            DRM_BYT_CopyBytes( ( DRM_BYTE * ) ( &wszPathName[0] ), SIZEOF( DRM_WCHAR ) * pdstrPathname->cchString, ( DRM_BYTE * ) (&wszFileName[0]), 0, SIZEOF( DRM_WCHAR ) * cchCopy );
            wszPathName[cchCopy + pdstrPathname->cchString] = g_wchNull;
        }

        pdstrPathname->pwszString = &wszPathName[0];
        pdstrPathname->cchString  = ( DRM_DWORD ) DRMCRT_wcslen(wszPathName);
    }

ErrorExit:
    return dr;
}

/* DANGER!!! THESE FUNCTIONS RETURN A POINTER TO A STATIC LOCAL FUNCTION!!
** The output from them won't last across function calls, you must make your
** own local copy IMMEDIATELY after calling them.
*/

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

DRM_RESULT DRM_CALL tGetBinCertPathname(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *f_pdstrPathname )
{
    return _GetDRMFileName( f_wszPathName, f_pdstrPathname, BDEVCERT_FILE_NAME );
}

DRM_RESULT DRM_CALL tGetDRMPath(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            f_wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *f_pdstrPathname )
{
    return _GetDRMFileName( f_wszPathName, f_pdstrPathname, NULL );
}


DRM_RESULT DRM_CALL tRemoveFile(const DRM_CHAR* filename)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( filename != NULL );

    if( 0 != remove( filename ) )
    {
        /*
        ** It is not an error if the file does not exist.
        */
        if( GetLastError() != ENOENT )
        {
            /* Wait two seconds (just once) for any possible Win32 host OS file I/O race condition to clear */
            tDRMSleep( 2000 );

            if( 0 != remove( filename ) )
            {
                DRM_DWORD dwError = GetLastError();
            
                /*
                ** It is not an error if the file does not exist.
                */
                if( dwError != ENOENT )
                {
                    dr = DRM_RESULT_FROM_WIN32( dwError );
                    Log("Trace"," \t\tFrom tRemoveFile - Failed 0x%x to remove file %s after two attempts\n", dr, filename );
                    ChkDR( dr );
                }
            }
        }
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL tRemoveFileW(const DRM_WCHAR *wcszPathname)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR *szPathName = NULL;
    DRM_DWORD cchPathName = 0;

    cchPathName = ( DRM_DWORD ) DRMCRT_wcslen(wcszPathname);
    ChkMem(szPathName = (char*)Oem_MemAlloc( cchPathName + 1 ) );
    ZEROMEM( szPathName, cchPathName + 1 );
    DRM_UTL_DemoteUNICODEtoASCII( wcszPathname, szPathName, cchPathName + 1 );

    ChkDR( tRemoveFile( szPathName ) );

ErrorExit:
    SAFE_OEM_FREE(szPathName);
    return dr;
}

DRM_RESULT DRM_CALL RemoveDRMFile(DRM_DWORD dwRemove)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrpath;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    if( dwRemove & RMFILE_STORE )
    {
        ChkDR( tGetDeviceStorePathname(wszPathName, &dstrpath) );
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if( dwRemove & RMFILE_PD_DEVCERT )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, DEVCERT_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if( dwRemove & RMFILE_PR_DEVCERT )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, BDEVCERT_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if( dwRemove & RMFILE_ND_DEVCERT )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, NDR_CERT));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if( dwRemove & RMFILE_PD_DEVCERTTEMPLATE )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, DEVCERT_TEMPLATE_FILE_NAME));
#if defined (WINCE)
        dr = tRemoveFileW(dstrpath.pwszString);
        if( DRM_FAILED( dr ) )
        {
            char *szError         = NULL;
            szError = DRM_ERR_GetErrorNameFromCode( dr, NULL );

            Log( "Trace", "WINCE test: removal of %S failed with %s(%#X).", dstrpath.pwszString, szError, dr );
            dr = DRM_SUCCESS;
        }
#else
        ChkDR(tRemoveFileW(dstrpath.pwszString));
#endif /* WINCE */
    }

    if( dwRemove & RMFILE_PR_DEVCERTTEMPLATE )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, GROUP_CERT_FILE_NAME));
#if defined (WINCE)
        dr = tRemoveFileW(dstrpath.pwszString);
        if( DRM_FAILED( dr ) )
        {
            char *szError         = NULL;
            szError = DRM_ERR_GetErrorNameFromCode( dr, NULL );

            Log( "Trace", "WINCE test: removal of %S failed with %s(%#X).", dstrpath.pwszString, szError, dr );
            dr = DRM_SUCCESS;
        }
#else
        ChkDR(tRemoveFileW(dstrpath.pwszString));
#endif /* WINCE */
    }

    if( dwRemove & RMFILE_ND_DEVCERTTEMPLATE )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, NDR_MODEL_CERT));
#if defined (WINCE)
        dr = tRemoveFileW(dstrpath.pwszString);
        if( DRM_FAILED( dr ) )
        {
            char *szError         = NULL;
            szError = DRM_ERR_GetErrorNameFromCode( dr, NULL );

            Log( "Trace", "WINCE test: removal of %S failed with %s(%#X).", dstrpath.pwszString, szError, dr );
            dr = DRM_SUCCESS;
        }
#else
        ChkDR(tRemoveFileW(dstrpath.pwszString));
#endif /* WINCE */
    }

    if( dwRemove & RMFILE_PD_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, PRIVATE_KEY_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if( dwRemove & RMFILE_PR_GROUP_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, GC_PRIVATE_KEY_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if( dwRemove & RMFILE_PR_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, Z_PRIVATE_KEY_FILE_NAME_SIGN));
        ChkDR(tRemoveFileW(dstrpath.pwszString));

        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if( dwRemove & RMFILE_ND_GROUP_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, NDR_MODEL_PRIVATE_KEY));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if( dwRemove & RMFILE_ND_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, NDR_PRIVATE_KEY));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if( dwRemove & RMFILE_KEYFILE )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, KEYFILE_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

ErrorExit:
    return dr;
}

DRM_BOOL DRM_CALL _IsDebuggerAttached()
{
#ifdef IsDebuggerPresent
    /* If we managed to pick up a Windows header, use it */
    return IsDebuggerPresent();
#else
#ifdef _M_IX86
    /* On x86 we can detect the debugger with this */
    DRM_DWORD dw;

    __asm
    {
        push eax                            /* Preserve the eax and ecx registers */
        push ecx
        mov eax, fs:[0x18]                  /* Get the TIB (thread information block) address */
        mov eax, dword ptr [eax + 0x30]     /* Get the pointer at offset 0x30 */
        mov ecx, dword ptr [eax]            /* Dereference the whole DRM_DWORD */
        mov dw, ecx                         /* Save it in our local variable */
        pop ecx                             /* Restore the registers */
        pop eax
    }

    /* The 3rd byte indicates whether a debugger is attached */
    return (DRM_BOOL)(dw & 0x00010000 ? TRUE : FALSE);
#else
    return FALSE;
#endif
#endif
}

static DRM_BB_CONTEXT s_tempBlackboxContext = {0};
static DRM_KEYFILE_CONTEXT s_tempKeyfileContext = {0};
DRM_RESULT DRM_CALL _StoreCertInKeyfile( DRM_KF_CERT_TYPE f_eCertType, const DRM_BYTE *pbCert, DRM_DWORD cbCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fKeyfileOpen = FALSE;
    DRM_BOOL fKeyfileInitialized = FALSE;
    PRIVKEY    GCPrivkey   = {0};
    DRM_DWORD  cbGCPrivKey = SIZEOF( PRIVKEY );

    ChkArg( f_eCertType != eKF_CERT_TYPE_INVALID );
    ChkArg( pbCert != NULL );
    ChkArg( cbCert != 0 );

    ChkDR( DRM_BBX_Initialize( NULL, &s_tempBlackboxContext ) );

    s_tempBlackboxContext.pKeyFileContext = &s_tempKeyfileContext;

    ChkDR( DRM_KF_Initialize( NULL, &s_tempKeyfileContext ) );
    fKeyfileInitialized = TRUE;
    ChkDR( DRM_KF_Open( &s_tempKeyfileContext,
                         TRUE,
                        OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
    fKeyfileOpen = TRUE;
    ChkDR( DRM_KF_LoadFromFile( &s_tempKeyfileContext, TRUE ) );

    ChkDR( DRM_KF_SetCertificate( &s_tempKeyfileContext, f_eCertType, pbCert, cbCert ) );
    ChkDR( DRM_BBX_SetupSecureStoreKey( &s_tempBlackboxContext, NULL ) );

    if( f_eCertType == eKF_CERT_TYPE_WMDRM )
    {
        DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrDevCert = EMPTY_DRM_STRING;
        PUBKEY pubkeyCurrent = {0};
        PRIVKEY privkeyCurrent = {0};
        DRM_DWORD cb = 0;
        DRM_ENCRYPTED_KEY privkey = {0};
        DRM_BYTE rgbKeyBuffer[128] = {0};

        dstrDevCert.pwszString = (const DRM_WCHAR*)pbCert;
        dstrDevCert.cchString = cbCert / SIZEOF( DRM_WCHAR );

        ChkDR( DRM_DCP_GetAttribute( NULL, &dstrDevCert, DRM_DEVCERT_DEVICEPUBKEY, NULL, &dstrValue) );
        cb = SIZEOF( PUBKEY );
        if(DRM_FAILED(DRM_B64_DecodeW(&dstrValue,
                                       &cb,
                            (DRM_BYTE*)&pubkeyCurrent,
                                        0)))
        {
            ChkDR( DRM_E_INVALID_DEVICE_CERTIFICATE );
        }

        ChkDR( DRM_DCP_GetAttribute( NULL, &dstrDevCert, DRM_DEVCERT_DEVICEPRIVKEY, NULL, &dstrValue) );

        if( dstrValue.cchString != CCH_BASE64_EQUIV( SIZEOF(PRIVKEY) ) )
        {
            ChkDR( DRM_E_INVALID_DEVICE_CERTIFICATE );
            goto ErrorExit;
        }

        cb = SIZEOF(PRIVKEY);
        if(DRM_FAILED(DRM_B64_DecodeW(&dstrValue,
                                       &cb,
                                       (DRM_BYTE*)&privkeyCurrent,
                                        0)))
        {
            ChkDR( DRM_E_INVALID_DEVICE_CERTIFICATE );
        }

        ChkDR( Oem_Device_GetKey( NULL,
                          DRM_DKT_WMDRMPD_GROUP,
                          (DRM_BYTE *)&GCPrivkey,
                          &cbGCPrivKey ) );

        ChkDR( DRM_BBX_Legacy_SymmetricCipher( FALSE,
                                               ( DRM_BYTE * )&GCPrivkey,
                                               SIZEOF( GCPrivkey ),
                                               ( DRM_BYTE * )&privkeyCurrent,
                                               ( DRM_BYTE * )&privkeyCurrent,
                                               SIZEOF( PRIVKEY ) ) );

        privkey.cbEncryptedKey = SIZEOF(rgbKeyBuffer);
        privkey.pbEncryptedKey = rgbKeyBuffer;

        ChkDR( DRM_BBX_KF_Legacy_EncryptKey ( NULL,
                             (DRM_BYTE*)&privkeyCurrent,
                                        SIZEOF(PRIVKEY),
                                        (DRM_DEVICE_KEY_TYPE)0,
                                        &privkey ) );

        ChkDR( DRM_KF_SetPrivateKey( &s_tempKeyfileContext,
                                      eKF_KEY_TYPE_ECC_160,
                          (DRM_BYTE*)&pubkeyCurrent,
                                      SIZEOF(PUBKEY),
                                     &privkey ) );
    }

    ChkDR( DRM_KF_StoreToFile( &s_tempKeyfileContext ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( (DRM_BYTE*)&GCPrivkey, SIZEOF( PRIVKEY ) );  /* trash the key in memory */

    if( s_tempBlackboxContext.fInited )
    {
        DRM_BBX_Shutdown( &s_tempBlackboxContext );
    }

    if( fKeyfileOpen )
    {
        DRM_KF_Close( &s_tempKeyfileContext );
    }
    if( fKeyfileInitialized )
    {
        DRM_KF_Uninitialize( &s_tempKeyfileContext );
    }
    return dr;
}


static DRM_RESULT _SymmetricCipher(
    __in                                DRM_BOOL  f_fEncrypt,
    __in_bcount_opt( f_cbKey )   const  DRM_BYTE *f_pbKey,
    __in                                DRM_DWORD f_cbKey,
    __in_bcount( f_cbData )             DRM_BYTE *f_pbDataIn,
    __out_bcount_opt( f_cbData )        DRM_BYTE *f_pbDataOut,
    __in                                DRM_DWORD f_cbData )
{
    DRM_RESULT                  dr                                                            = DRM_SUCCESS;
    DRM_BYTE                   *rgbOutputKeyAligned                                           = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT stkContext                                                    = {0};
    DRM_AES_KEY                 aesKey                                                        = {0};
    DRM_BYTE                    rgbOutputKeyUnaligned[ DRM_AES_BLOCKLEN + SIZEOF(DRM_DWORD) ] = {0};
    DRM_BYTE                    rgbConstantBlock[DRM_AES_BLOCKLEN]                            = {0};

    ChkArg( f_pbDataIn  != NULL );
    ChkArg( f_pbKey     != NULL );
    ChkArg( f_cbData > 0 );
    ChkArg( f_cbKey > 0 );

    if ( f_pbDataOut != NULL )
    {
        MEMCPY( f_pbDataOut, f_pbDataIn, f_cbData );
    }
    else
    {
        f_pbDataOut = f_pbDataIn;
    }

    /*
    **  1. Get an OMAC1 of the privkey
    **  2. Generate an AES key using this hash
    **  3. Decrypt or encrypt ciphertext using this AES key
    */

    /*
    ** rgbOutputKeyAligned will be cast to a DWORD aligned
    ** structure in Oem_Aes_SetKey so make sure rgbOutputKeyAligned
    ** is DWORD aligned.
    */
    ChkDR( DRM_STK_Init( &stkContext,
                            rgbOutputKeyUnaligned,
                            SIZEOF( rgbOutputKeyUnaligned ) ) );

    ChkDR( DRM_STK_Alloc_Aligned( &stkContext,
                                    DRM_AES_BLOCKLEN,
                                    SIZEOF( DRM_DWORD ),
                                    NULL,
                                    ( DRM_VOID **) &rgbOutputKeyAligned ) );

    ChkBOOL( f_cbKey >= DRM_AES_KEYSIZE_128, DRM_E_CH_BAD_KEY );
    ChkDR( Oem_Aes_SetKey( f_pbKey, &aesKey ) );
    ChkDR( Oem_Omac1_Sign(&aesKey, rgbConstantBlock, 0, SIZEOF(rgbConstantBlock), rgbOutputKeyAligned ) );
    ChkDR( Oem_Aes_ZeroKey(&aesKey) );

    ChkDR( Oem_Aes_SetKey( rgbOutputKeyAligned, &aesKey ) );

    if ( f_fEncrypt )
    {
        ChkDR( Oem_Aes_EcbEncryptData( &aesKey, f_pbDataOut, f_cbData ) );
    }
    else
    {
        ChkDR( Oem_Aes_EcbDecryptData( &aesKey, f_pbDataOut, f_cbData ) );
    }

ErrorExit:
    (DRM_VOID)Oem_Aes_ZeroKey( &aesKey );
    OEM_SECURE_ZERO_MEMORY( rgbOutputKeyUnaligned, SIZEOF( rgbOutputKeyUnaligned ) );
    return dr;
}


static DRM_RESULT DRM_CALL _StoreKeyInKeyfile( DRM_KF_KEY_TYPE f_eKeyType, DRM_DEVICE_KEY_TYPE f_eDeviceKeyType, const DRM_BYTE *pbPubKey, DRM_DWORD cbPubKey, DRM_BYTE *pbKey, DRM_DWORD cbKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fKeyfileOpen = FALSE;
    DRM_BOOL fKeyfileInitialized = FALSE;
    DRM_ENCRYPTED_KEY privkey = {0};
    PRIVKEY_P256    GCPrivKey   = {0};
    DRM_DWORD       cbGCPrivKey = SIZEOF( PRIVKEY_P256 );
    DRM_BYTE rgbKeyBuffer[128] = {0};

    ChkArg( pbPubKey != NULL );
    ChkArg( pbKey != NULL );

    ChkDR( DRM_KF_Initialize( NULL, &s_tempKeyfileContext ) );
    fKeyfileInitialized = TRUE;
    ChkDR( DRM_KF_Open( &s_tempKeyfileContext,
                         TRUE,
                        OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
    fKeyfileOpen = TRUE;
    ChkDR( DRM_KF_LoadFromFile( &s_tempKeyfileContext, TRUE ) );

    if( f_eKeyType == eKF_KEY_TYPE_ECC_160 )
    {
        ChkArg( cbPubKey == SIZEOF( PUBKEY ) );
        ChkArg( cbKey == SIZEOF( PRIVKEY ) );
    }
    else if( f_eKeyType == eKF_KEY_TYPE_ECC_256 )
    {
        ChkArg( cbPubKey == SIZEOF( PUBKEY_P256) );
        ChkArg( cbKey == SIZEOF( PRIVKEY_P256 ) );

        ChkDR( Oem_Device_GetKey( NULL,
                          DRM_DKT_PLAYREADY_MODEL,
                          (DRM_BYTE *)&GCPrivKey,
                          &cbGCPrivKey ) );

        ChkDR( _SymmetricCipher( FALSE,
                                 (DRM_BYTE *)&GCPrivKey,
                                 SIZEOF( PRIVKEY_P256 ),
                                 pbKey,
                                 ( DRM_BYTE * )pbKey,
                                 SIZEOF( PRIVKEY_P256 ) ) );
    }
    else
    {
        ChkArg( FALSE );
    }
    privkey.cbEncryptedKey = SIZEOF(rgbKeyBuffer);
    privkey.pbEncryptedKey = rgbKeyBuffer;

/*
    ** Should this be modified to use key handles, or left as it is, given this is WMDRM?
    */
    ChkDR( DRM_BBX_KF_Legacy_EncryptKey ( NULL,
                                          pbKey,
                                          cbKey,
                                          (DRM_DEVICE_KEY_TYPE)f_eDeviceKeyType,
                                          &privkey ) );

    ChkDR( DRM_KF_SetPrivateKey( &s_tempKeyfileContext, f_eKeyType, pbPubKey, cbPubKey, &privkey ) );
    ChkDR( DRM_KF_StoreToFile( &s_tempKeyfileContext ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( (DRM_BYTE*)&GCPrivKey, SIZEOF( PRIVKEY_P256 ) );  /* trash the key in memory */

    if( fKeyfileOpen )
    {
        DRM_KF_Close( &s_tempKeyfileContext );
    }
    if( fKeyfileInitialized )
    {
        DRM_KF_Uninitialize( &s_tempKeyfileContext );
    }
    return dr;
}

DRM_RESULT DRM_CALL _CopyFileToDRMFolder(const DRM_WCHAR *wszSrc, const DRM_WCHAR *wszDest)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrDest = {0};
    DRM_DWORD cbData;
    DRM_BYTE *pbData = NULL;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    if(!wszSrc)
    {
        goto ErrorExit;
    }

    if( !DRM_WMDRM_IsWMDRMSupported() )
    {
        /*
        ** Skip WMDRM-specific files
        */
        if( ( DRMCRT_wcsicmp( wszDest, DEVCERT_FILE_NAME )          == 0 )
         || ( DRMCRT_wcsicmp( wszDest, DEVCERT_TEMPLATE_FILE_NAME ) == 0 )
         || ( DRMCRT_wcsicmp( wszDest, PRIVATE_KEY_FILE_NAME )      == 0 )
         || ( DRMCRT_wcsicmp( wszDest, WMDRMPD_KEY_FILE_NAME )      == 0 ) )
        {
            goto ErrorExit;
        }
    }

    ChkBOOL(LoadTestFileW(NULL, wszSrc, &pbData, &cbData), DRM_E_FILE_READ_ERROR);

    if( DRMCRT_wcsicmp( wszDest, DEVCERT_FILE_NAME ) == 0 )
    {
        ChkDR( _StoreCertInKeyfile( eKF_CERT_TYPE_WMDRM, pbData, cbData ) );
    }
    else if( DRMCRT_wcsicmp( wszDest, BDEVCERT_FILE_NAME ) == 0 )
    {
        ChkDR( _StoreCertInKeyfile( eKF_CERT_TYPE_PLAYREADY, pbData, cbData ) );
    }
    else if( DRMCRT_wcsicmp( wszDest, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT ) == 0
          || DRMCRT_wcsicmp( wszDest, Z_PRIVATE_KEY_FILE_NAME_SIGN ) == 0)
    {
        if( cbData == SIZEOF( PRIVKEY_P256 ) + SIZEOF( PUBKEY_P256 ) )
        {
            DRM_DEVICE_KEY_TYPE eDeviceKeyType = DRM_DKT_INVALID;

            if( DRMCRT_wcsicmp( wszDest, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT ) == 0 )
            {
                eDeviceKeyType = DRM_DKT_PLAYREADY_DEVICE_ENCRYPT;
            }
            else
            {
                eDeviceKeyType = DRM_DKT_PLAYREADY_DEVICE_SIGN;
            }

            ChkDR( _StoreKeyInKeyfile( eKF_KEY_TYPE_ECC_256,
                                       eDeviceKeyType,
                                       pbData + SIZEOF( PRIVKEY_P256 ),
                                       SIZEOF( PUBKEY_P256 ),
                                       pbData,
                                       SIZEOF(PRIVKEY_P256 ) ) );
        }
    }

    ChkDR(_GetDRMFileName(wszPathName, &dstrDest, wszDest));
    ChkBOOL(SaveToFileW(dstrDest.pwszString, pbData, cbData), DRM_E_FILE_WRITE_ERROR);

ErrorExit:
    SAFE_OEM_FREE(pbData);
    return dr;
}


static DRM_RESULT _tGetTCFileListImp( __in_z const char *szFileNameTemplate, __in_z char **szPlatforms, __inout_ecount_z_opt( *pdwCount ) char **szFileNames, __inout DRM_DWORD *pdwCount)
{

    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fMoreFiles = TRUE;
    DRM_DWORD dwFound = 0;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fd;
    DRM_CONST_STRING dstrFileNameTemplate = {0};
    char **szPlatform, szFileName[256] = {0};
    DRM_DWORD cchTemp = 0;
#if WINCE_TEST
    DRM_DWORD i = 0;
    DRM_BOOL fFound = FALSE;
#endif /* WINCE_TEST */

    /* ChkArg(szFileNameTemplate && szPlatforms && pdwCount); */

    ChkDR( TST_UTL_MakePathString( NULL, szFileNameTemplate, &dstrFileNameTemplate ) );

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_SUPERFLUOUS_CAST_25059, "Cast required on alternative platforms")
    hFindFile = FindFirstFileW((DRM_WCHAR_NATIVE *)dstrFileNameTemplate.pwszString, &fd);
PREFAST_POP /* __WARNING_SUPERFLUOUS_CAST_25059 */
    while (INVALID_HANDLE_VALUE != hFindFile && fMoreFiles) {
        for (szPlatform = szPlatforms; *szPlatform; szPlatform++) {
            DRM_UTL_DemoteUNICODEtoASCII( fd.cFileName, szFileName, sizeof(szFileName) );
            if(strstr(szFileName, *szPlatform)) {

#if WINCE_TEST
                /*
                ** To accommodate WM7 (which may enumerate the same file multiple times for files
                ** on a storage card folder), each newly enumerated file is checked against the existing
                ** files and if it already exists, simply ignore it.
                */
                if( szFileNames != NULL ) {
                    fFound = FALSE;
                    for ( i = 0; i < dwFound; i++ ) {
                        if( strstr( szFileNames[ i ], szFileName ) != NULL ) {
                            fFound = TRUE;
                            break;
                        }
                    }

                    if( fFound )
                    {
                        break;
                    }
                }
#endif /* WINCE_TEST */

                if(szFileNames && dwFound < *pdwCount) {
                    cchTemp = 0;
                    ChkDR( TST_UTL_MakePathA( NULL, szFileName, &(szFileNames[dwFound]), &cchTemp, FALSE ) );
                }
                dwFound++;
                break;
            }
        }
        fMoreFiles = FindNextFileW(hFindFile, &fd);
    }

    if(fMoreFiles) {
        DWORD dw = GetLastError();
        ChkDR(DRM_RESULT_FROM_WIN32(dw));
    }

    if(!szFileNames || dwFound > *pdwCount)
        dr = DRM_E_BUFFERTOOSMALL;

    *pdwCount = dwFound;
    
ErrorExit:
    if(hFindFile != INVALID_HANDLE_VALUE)
        FindClose(hFindFile);
    FREEDRMSTRING( dstrFileNameTemplate );
    return dr;

}

DRM_RESULT DRM_CALL tGetTCFileList(const char *szTestFilePrefix, const char **szPlatforms, const char ***pszTestFileNames)
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    char        szFN[260]    = {0};
    const char *pszWildCards = ".*.txt";
    DRM_DWORD   cchWildcards = ( DRM_DWORD ) strlen( pszWildCards );
    DRM_DWORD   dwCount      = 0;
    DRM_DWORD   cchPrefix    = 0;

    ChkArg( szTestFilePrefix );
    ChkArg( szPlatforms );
    ChkArg( pszTestFileNames );

    cchPrefix = ( DRM_DWORD ) strlen( szTestFilePrefix );

    ChkOverflow( cchPrefix + cchWildcards, cchPrefix );

    ChkArg( SIZEOF( szFN ) > cchPrefix + cchWildcards );

    MEMCPY(szFN, szTestFilePrefix, cchPrefix );
    DRM_BYT_CopyBytes( szFN, cchPrefix, pszWildCards, 0, cchWildcards );
    szFN[ cchPrefix + cchWildcards ] = '\0';

    if(DRM_E_BUFFERTOOSMALL != (dr = _tGetTCFileListImp(szFN, (char **)szPlatforms, NULL, &dwCount)))
    {
        ChkDR(dr);
    }

    ChkMem(*pszTestFileNames = (const char**)Oem_MemAlloc((dwCount + 1) * SIZEOF(char*)));
    ZEROMEM((char **)*pszTestFileNames, (dwCount + 1) * SIZEOF(char*));
    ChkDR(_tGetTCFileListImp(szFN, (char **)szPlatforms, (char **)*pszTestFileNames, &dwCount));
ErrorExit:
    return dr;
}

void DRM_CALL tFreeFileNameList(__in_opt const char **szTestFileNames)
{
    if(szTestFileNames)
    {
        const char **p = NULL;
        for ( p = szTestFileNames; *p; p++ )
        {
            SAFE_OEM_FREE(*p);
        }
        SAFE_OEM_FREE(szTestFileNames);
    }
}


/*
** Value is normalized to Milliseconds
*/
DRM_UINT64 DRM_CALL GetTimer (void)
{
    return DRM_UI64( Oem_Clock_GetTickCount( NULL ) );
}


DRM_VOID DRM_CALL tOEMSignalTestDone()
{
#if WINCE_TEST
    DRM_CONST_STRING dstrDoneFileName = EMPTY_DRM_STRING;
    DRM_BYTE szTemp[] = { 'I', '\'', 'm',' ','D', 'o', 'n', 'e'};

    ( DRM_VOID ) TST_UTL_MakePathString( NULL, "testing_done.txt", &dstrDoneFileName );
    ( DRM_VOID ) SaveToFileW( dstrDoneFileName.pwszString, szTemp, SIZEOF( szTemp ) );
    SAFE_OEM_FREE(dstrDoneFileName.pwszString );
#endif
}



/*********************************************************
**
** Function: WriteToolOutputToLog
**
** Synopsis: Reads data from supplied pipe and writes it to file
**
** Arguments:
**  f_hToolStdOut -- Handle of the pipe to read data from
**
** Returns     DRM_SUCCESS if data is read and written correctly
**             Other error code mapped from Win32 error code.
**
*********************************************************/
static DRM_RESULT WriteToolOutputToLog( __in HANDLE f_hToolStdOut )
{
#ifdef WINCE_TEST
    return DRM_E_NOTIMPL;
#else

    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_BYTE  *pbData = NULL;
    DRM_DWORD  cbData = 0;
    DRM_DWORD  cbRead = 0;

    /*
    ** Check amount of data present in the pipe. It is written to cbData variable
    */

    if( !PeekNamedPipe( f_hToolStdOut, NULL, 0, NULL, (LPDWORD)&cbData, NULL ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "PeekNamedPipe failed 0x%x.", dr );
        goto ErrorExit;
    }

    /*
    ** If pipe is not empty, read from it and write to log file.
    */
    if( cbData > 0 )
    {
        ChkMem( pbData = ( DRM_BYTE * ) Oem_MemAlloc( cbData ) );

        if( !ReadFile( f_hToolStdOut, pbData, cbData, (LPDWORD)&cbRead, NULL ) )
        {
            dr = DRM_RESULT_FROM_WIN32( GetLastError() );
            Log( "Trace", "ReadFile failed 0x%x.", dr );
            goto ErrorExit;
        }

        LogBlob( "Trace", "Stdout from execution:", pbData, cbRead );
    }

ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
#endif
}

/*********************************************************
**
** Function: TST_OEM_ExecuteShellCommand
**
** Synopsis: Executes a commandline and writes the output to the log
**
** Arguments:
**            f_szCommandLine -- Commond Line to execute
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_ExecuteShellCommand( __in_z DRM_CHAR *f_szCommandLine )
{
#ifdef WINCE_TEST
    return DRM_E_NOTIMPL;
#else
    DRM_RESULT          dr                       = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD           dwExitCode               = 0;
    PROCESS_INFORMATION processInfo              = {0};
    STARTUPINFOA        startupInfo              = {0};
    HANDLE              hStdOutRd                = NULL;
    HANDLE              hStdOutWr                = NULL;
    HANDLE              hStdInRd                 = NULL;
    HANDLE              hStdInWr                 = NULL;
    SECURITY_ATTRIBUTES saAttr                   = {0};
    DRM_DWORD           dwWaitResult             = WAIT_TIMEOUT;

    ChkArg( NULL != f_szCommandLine );

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    if( !CreatePipe( &hStdOutRd, &hStdOutWr, &saAttr, PIPE_SIZE ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "Failed to create out pipe 0x%x.", dr );
        goto ErrorExit;
    }
    SetHandleInformation( hStdOutRd, HANDLE_FLAG_INHERIT, 0);

    if( !CreatePipe( &hStdInRd, &hStdInWr, &saAttr, PIPE_SIZE ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "Failed to create in pipe 0x%x", dr );
        goto ErrorExit;
    }
    SetHandleInformation( hStdInWr, HANDLE_FLAG_INHERIT, 0);

    startupInfo.cb = SIZEOF(startupInfo);
    startupInfo.dwFlags    |= STARTF_USESTDHANDLES;
    startupInfo.hStdError  =  hStdOutWr;
    startupInfo.hStdOutput =  hStdOutWr;
    startupInfo.hStdInput  =  hStdInRd;

    Log( "Trace", "Launching: %s", f_szCommandLine );
    if( !CreateProcessA( NULL,
                         f_szCommandLine,
                         NULL,
                         NULL,
                         TRUE,
                         NORMAL_PRIORITY_CLASS,
                         NULL,
                         NULL,
                        &startupInfo,
                        &processInfo ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "Failed to create process 0x%x.", dr );
        goto ErrorExit;
    }

    Log( "Trace", "Waiting for tool to exit ..." );

    /*
    ** Stay in loop until tool runs
    */

    while ( dwWaitResult == WAIT_TIMEOUT )
    {
        /*
        ** Let tool run for one second. Then write output from pipe to log file
        */
        dwWaitResult = WaitForSingleObject( processInfo.hProcess, 1000 );

        /*
        ** Checks if pipe was created and writes to output
        */

        if( hStdOutRd != NULL )
        {
            ChkDR( WriteToolOutputToLog( hStdOutRd ) );
        }

        /*
        ** Log in case of wait failure. It may happen if handle of process invalid.
        */

        if( dwWaitResult == WAIT_FAILED || dwWaitResult == WAIT_ABANDONED )
        {
            dr = DRM_RESULT_FROM_WIN32( GetLastError() );
            Log( "Trace", "WaitForSingleObject failed 0x%x.", dr );
            goto ErrorExit;
        }
    }

    if( !GetExitCodeProcess( processInfo.hProcess, (LPDWORD)&dwExitCode ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "GetExitCodeProcess failed 0x%x.", dr );
        goto ErrorExit;
    }

    if( dwExitCode != 0 )
    {
        dr = (DRM_RESULT)dwExitCode;
        Log( "Trace", "Tool exited with non-zero error code 0x%x.", dr );
        goto ErrorExit;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    if( hStdOutRd != NULL )
    {
        CloseHandle( hStdOutRd );
    }
    if( hStdOutWr != NULL )
    {
        CloseHandle( hStdOutWr );
    }
    if( hStdInRd != NULL )
    {
        CloseHandle( hStdInRd );
    }
    if( hStdInWr != NULL )
    {
        CloseHandle( hStdInWr );
    }

    if( processInfo.hProcess != NULL )
    {
        CloseHandle( processInfo.hProcess );
    }
    if( processInfo.hThread != NULL )
    {
        CloseHandle( processInfo.hThread );
    }

    return dr;
#endif
}

typedef struct __tag_AsyncShellCommandData
{
    HANDLE  hStdOutRd;
    HANDLE  hStdOutWr;
    HANDLE  hStdInRd;
    HANDLE  hStdInWr;
    HANDLE  hProcess;
    HANDLE  hThread;

    HANDLE  hMap;
    BYTE   *pbMap;
    HANDLE  hEvent1;
    HANDLE  hEvent2;

} AsyncShellCommandData;

static DRM_RESULT _WaitForProcess(
    __in        HANDLE      f_hProcess,
    __in        HANDLE      f_hStdOutRd,
    __in        DRM_DWORD   f_dwWaitTime,
    __out_opt   DRM_BOOL   *f_pfEnded,
    __out_opt   DRM_BOOL   *f_pfStdOutRead )
{
#ifdef WINCE_TEST
    return DRM_E_NOTIMPL;
#else
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_DWORD   dwWaitResult    = 0;
    DRM_DWORD   cbData          = 0;
    DRM_BYTE   *pbData          = NULL;
    DRM_DWORD   cbRead          = 0;

    if( f_pfStdOutRead )
    {
        *f_pfStdOutRead = FALSE;
    }

    dwWaitResult = WaitForSingleObject( f_hProcess, f_dwWaitTime );
    if( dwWaitResult == WAIT_FAILED || dwWaitResult == WAIT_ABANDONED )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "WaitForSingleObject failed 0x%x.", dr );
        goto ErrorExit;
    }

    if( f_pfEnded != NULL )
    {
        *f_pfEnded = ( dwWaitResult == WAIT_OBJECT_0 );
    }

    if( !PeekNamedPipe( f_hStdOutRd, NULL, 0, NULL, (LPDWORD)&cbData, NULL ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "PeekNamedPipe failed 0x%x.", dr );
        goto ErrorExit;
    }
    if( cbData > 0 )
    {
        ChkMem( pbData = (DRM_BYTE*)Oem_MemAlloc( cbData + 1 ) );
        pbData[cbData] = 0;

        if( !ReadFile( f_hStdOutRd, pbData, cbData, (LPDWORD)&cbRead, NULL ) || cbRead != cbData )
        {
            dr = DRM_RESULT_FROM_WIN32( GetLastError() );
            if( DRM_SUCCEEDED( dr ) )
            {
                dr = DRM_E_FILE_READ_ERROR;
            }
            Log( "Trace", "ReadFile failed 0x%x.", dr );
            goto ErrorExit;
        }

        Log( "Trace", "\n\nBEGIN AsyncShellCommand output\n\n%s\nEND AsyncShellCommand output\n\n", pbData );
        if( f_pfStdOutRead )
        {
            *f_pfStdOutRead = TRUE;
        }
    }

ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
#endif
}

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandStart(
    __in_z                    const DRM_CHAR     *f_szCommandLine, 
    __out                           DRM_VOID    **f_ppvAsyncShellCommandData )
{
#ifdef WINCE_TEST
    return DRM_E_NOTIMPL;
#else
    DRM_RESULT              dr                      = DRM_E_TEST_INCOMPLETE;
    PROCESS_INFORMATION     processInfo             = {0};
    STARTUPINFOA            startupInfo             = {0};
    HANDLE                  hStdOutRd               = NULL;
    HANDLE                  hStdOutWr               = NULL;
    HANDLE                  hStdInRd                = NULL;
    HANDLE                  hStdInWr                = NULL;
    SECURITY_ATTRIBUTES     saAttr                  = {0};
    AsyncShellCommandData  *pAsyncShellCommandData  = NULL;
    DRM_DWORD               dwWaitResult            = 0;
    HANDLE                  hMap                    = NULL;
    BYTE                   *pbMap                   = NULL;
    HANDLE                  hEvent1                 = NULL;
    HANDLE                  hEvent2                 = NULL;
    const DRM_WCHAR         rgwchMapName[]          = DRMTEST_AsyncShellCommand_Map_Name;
    const DRM_WCHAR         rgwchEvent1Name[]       = DRMTEST_AsyncShellCommand_Event1_Name;
    const DRM_WCHAR         rgwchEvent2Name[]       = DRMTEST_AsyncShellCommand_Event2_Name;

    ChkArg( NULL != f_szCommandLine );

    ChkMem( pAsyncShellCommandData = (AsyncShellCommandData*)Oem_MemAlloc( SIZEOF( AsyncShellCommandData ) ) );
    ZEROMEM( pAsyncShellCommandData, SIZEOF( AsyncShellCommandData ) );

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    if( !CreatePipe( &hStdOutRd, &hStdOutWr, &saAttr, PIPE_SIZE ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "Failed to create out pipe 0x%x.", dr );
        goto ErrorExit;
    }
    SetHandleInformation( hStdOutRd, HANDLE_FLAG_INHERIT, 0);

    if( !CreatePipe( &hStdInRd, &hStdInWr, &saAttr, PIPE_SIZE ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "Failed to create in pipe 0x%x.", dr );
        goto ErrorExit;
    }
    SetHandleInformation( hStdInWr, HANDLE_FLAG_INHERIT, 0);

    hMap = CreateFileMappingW(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        DRMTEST_AsyncShellCommand_Map_Size,
        rgwchMapName );
    if( hMap == NULL )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "CreateFileMappingW failed 0x%x.", dr );
        goto ErrorExit;
    }

    pbMap = (BYTE*) MapViewOfFile(
        hMap,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        DRMTEST_AsyncShellCommand_Map_Size );
    if( pbMap == NULL )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "MapViewOfFile failed 0x%x", dr );
        goto ErrorExit;
    }

    hEvent1 = CreateEventW( &saAttr, FALSE, FALSE, rgwchEvent1Name );
    if( hEvent1 == INVALID_HANDLE_VALUE )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "Failed to create event 0x%x.", dr );
        goto ErrorExit;
    }

    hEvent2 = CreateEventW( &saAttr, FALSE, FALSE, rgwchEvent2Name );
    if( pAsyncShellCommandData->hEvent2 == INVALID_HANDLE_VALUE )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "Failed to create event 0x%x.", dr );
        goto ErrorExit;
    }

    startupInfo.cb = SIZEOF(startupInfo);
    startupInfo.dwFlags    |= STARTF_USESTDHANDLES;
    startupInfo.hStdError  =  hStdOutWr;
    startupInfo.hStdOutput =  hStdOutWr;
    startupInfo.hStdInput  =  hStdInRd;

    Log( "Trace", "Start AsyncShellCommand: %s", f_szCommandLine );
    if( !CreateProcessA(
        NULL,
        (LPSTR)f_szCommandLine,
        NULL,
        NULL,
        TRUE,
        NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &startupInfo,
        &processInfo ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "Failed to create process 0x%x.", dr );
        goto ErrorExit;
    }

    dwWaitResult = WaitForSingleObject( hEvent1, ASYNC_SHELL_COMMAND_DEFAULT_PING_TIME * ASYNC_SHELL_COMMAND_WAIT_PINGS );
    ChkBOOL( dwWaitResult == WAIT_OBJECT_0, HRESULT_FROM_WIN32( ERROR_TIMEOUT ) );

    /* Transfer ownership */
    pAsyncShellCommandData->hStdOutRd = hStdOutRd;
    pAsyncShellCommandData->hStdOutWr = hStdOutWr;
    pAsyncShellCommandData->hStdInRd  = hStdInRd;
    pAsyncShellCommandData->hStdInWr  = hStdInWr;
    pAsyncShellCommandData->hProcess  = processInfo.hProcess;
    pAsyncShellCommandData->hThread   = processInfo.hThread;
    pAsyncShellCommandData->hMap      = hMap;
    pAsyncShellCommandData->pbMap     = pbMap;
    pAsyncShellCommandData->hEvent1   = hEvent1;
    pAsyncShellCommandData->hEvent2   = hEvent2;
    *f_ppvAsyncShellCommandData       = pAsyncShellCommandData;
    hStdOutRd               = NULL;
    hStdOutWr               = NULL;
    hStdInRd                = NULL;
    hStdInWr                = NULL;
    processInfo.hProcess    = NULL;
    processInfo.hThread     = NULL;
    hMap                    = NULL;
    pbMap                   = NULL;
    hEvent1                 = NULL;
    hEvent2                 = NULL;
    pAsyncShellCommandData  = NULL;

    dr = DRM_SUCCESS;

ErrorExit:
    if( hStdOutRd != NULL )
    {
        CloseHandle( hStdOutRd );
    }
    if( hStdOutWr != NULL )
    {
        CloseHandle( hStdOutWr );
    }
    if( hStdInRd != NULL )
    {
        CloseHandle( hStdInRd );
    }
    if( hStdInWr != NULL )
    {
        CloseHandle( hStdInWr );
    }
    if( processInfo.hProcess != NULL )
    {
        CloseHandle( processInfo.hProcess );
    }
    if( processInfo.hThread != NULL )
    {
        CloseHandle( processInfo.hThread );
    }
    if( pbMap != NULL )
    {
        UnmapViewOfFile( pbMap );
    }
    if( hMap != NULL )
    {
        CloseHandle( hMap );
    }
    if( hEvent1 != NULL )
    {
        CloseHandle( hEvent1 );
    }
    if( hEvent2 != NULL )
    {
        CloseHandle( hEvent2 );
    }
    SAFE_OEM_FREE( pAsyncShellCommandData );

    return dr;
#endif
}

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandEnd(
    __inout                         DRM_VOID    **f_ppvAsyncShellCommandData )
{
#ifdef WINCE_TEST
    return DRM_E_NOTIMPL;
#else
    DRM_RESULT              dr                      = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD               dwExitCode              = 0;
    DRM_BOOL                fEnded                  = FALSE;
    AsyncShellCommandData  *pAsyncShellCommandData  = NULL;

    ChkArg(  f_ppvAsyncShellCommandData != NULL );
    ChkArg( *f_ppvAsyncShellCommandData != NULL );

    /* Take ownership - even if this function fails, it frees everything to avoid a leak */
    pAsyncShellCommandData = (AsyncShellCommandData*)*f_ppvAsyncShellCommandData;
    *f_ppvAsyncShellCommandData = NULL;

    Log( "Trace", "Waiting for AsyncShellCommand to exit ..." );

    while( !fEnded )
    {
        ChkDR( _WaitForProcess( pAsyncShellCommandData->hProcess, pAsyncShellCommandData->hStdOutRd, ASYNC_SHELL_COMMAND_DEFAULT_PING_TIME, &fEnded, NULL ) );
    }

    if( !GetExitCodeProcess( pAsyncShellCommandData->hProcess, (LPDWORD)&dwExitCode ) )
    {
        dr = DRM_RESULT_FROM_WIN32( GetLastError() );
        Log( "Trace", "GetExitCodeProcess failed 0x%x.", dr );
        goto ErrorExit;
    }

    if( dwExitCode != 0 )
    {
        Log( "Trace", "... AsyncShellCommand exited with non-zero error code 0x%x.", dwExitCode );
        dr = (DRM_RESULT)dwExitCode;
        DRMASSERT( DRM_FAILED( dr ) );
        goto ErrorExit;
    }

    Log( "Trace", "... AsyncShellCommand exited successfully." );

    dr = DRM_SUCCESS;
ErrorExit:
    if( pAsyncShellCommandData != NULL )
    {
        if( pAsyncShellCommandData->hStdOutRd != NULL )
        {
            CloseHandle( pAsyncShellCommandData->hStdOutRd );
        }
        if( pAsyncShellCommandData->hStdOutWr != NULL )
        {
            CloseHandle( pAsyncShellCommandData->hStdOutWr );
        }
        if( pAsyncShellCommandData->hStdInRd != NULL )
        {
            CloseHandle( pAsyncShellCommandData->hStdInRd );
        }
        if( pAsyncShellCommandData->hStdInWr != NULL )
        {
            CloseHandle( pAsyncShellCommandData->hStdInWr );
        }
        if( pAsyncShellCommandData->hProcess != NULL )
        {
            CloseHandle( pAsyncShellCommandData->hProcess );
        }
        if( pAsyncShellCommandData->hThread != NULL )
        {
            CloseHandle( pAsyncShellCommandData->hThread );
        }
        if( pAsyncShellCommandData->pbMap != NULL )
        {
            UnmapViewOfFile( pAsyncShellCommandData->pbMap );
        }
        if( pAsyncShellCommandData->hMap != NULL )
        {
            CloseHandle( pAsyncShellCommandData->hMap );
        }
        if( pAsyncShellCommandData->hEvent1 != NULL )
        {
            CloseHandle( pAsyncShellCommandData->hEvent1 );
        }
        if( pAsyncShellCommandData->hEvent2 != NULL )
        {
            CloseHandle( pAsyncShellCommandData->hEvent2 );
        }
        SAFE_OEM_FREE( pAsyncShellCommandData );
    }

    return dr;
#endif
}

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandPing(
    __in                                DRM_VOID                         *f_pvAsyncShellCommandData,
    __in                                DRM_DWORD                         f_dwMs )
{
#ifdef WINCE_TEST
    return DRM_E_NOTIMPL;
#else
    DRM_RESULT              dr                      = DRM_E_TEST_INCOMPLETE;
    AsyncShellCommandData  *pAsyncShellCommandData  = NULL;
    DRM_BOOL                fEnded                  = FALSE;

    ChkArg( f_pvAsyncShellCommandData != NULL );
    pAsyncShellCommandData = (AsyncShellCommandData*)f_pvAsyncShellCommandData;

    ChkDR( _WaitForProcess( pAsyncShellCommandData->hProcess, pAsyncShellCommandData->hStdOutRd, f_dwMs, &fEnded, NULL ) );
    ChkBOOL( !fEnded, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
#endif
}

DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandReadMessage(            
    __in                                DRM_VOID                         *f_pvAsyncShellCommandData,
    __out                               DRM_DWORD                        *f_pdwMessageType,
    __deref_out_ecount(*f_pcbMessage)   DRM_BYTE                        **f_ppbMessage,
    __out                               DRM_DWORD                        *f_pcbMessage )
{
#ifdef WINCE_TEST
    return DRM_E_NOTIMPL;
#else
    DRM_RESULT              dr                      = DRM_E_TEST_INCOMPLETE;
    AsyncShellCommandData  *pAsyncShellCommandData  = NULL;
    DRM_BYTE               *pbMessage               = NULL;
    DRM_DWORD               cbMessage               = 0;
    DRM_DWORD               dwWaitResult            = 0;

    ChkArg( f_pvAsyncShellCommandData != NULL );
    pAsyncShellCommandData = (AsyncShellCommandData*)f_pvAsyncShellCommandData;
    ChkArg( f_pdwMessageType          != NULL );
    ChkArg( f_ppbMessage              != NULL );
    ChkArg( f_pcbMessage              != NULL );
    *f_ppbMessage = NULL;
    *f_pcbMessage = 0;

    /* Wait for process to signal us that it has written data to shared memory */
    dwWaitResult = WaitForSingleObject( pAsyncShellCommandData->hEvent1, ASYNC_SHELL_COMMAND_DEFAULT_PING_TIME * ASYNC_SHELL_COMMAND_WAIT_PINGS );
    ChkBOOL( dwWaitResult == WAIT_OBJECT_0, HRESULT_FROM_WIN32( ERROR_TIMEOUT ) );

    /* Read message size */
    MEMCPY( &cbMessage, pAsyncShellCommandData->pbMap, SIZEOF(DRM_DWORD) );
    ChkBOOL( cbMessage >= SIZEOF(DRM_DWORD), DRM_E_FILE_READ_ERROR );

    /* Read message type */
    MEMCPY( f_pdwMessageType, pAsyncShellCommandData->pbMap + SIZEOF(DRM_DWORD), SIZEOF(DRM_DWORD) );

    cbMessage -= SIZEOF(DRM_DWORD);
    if( cbMessage > 0 )
    {
        /* Read message data */
        ChkMem( pbMessage = (DRM_BYTE*)Oem_MemAlloc( cbMessage ) );
        MEMCPY( pbMessage, pAsyncShellCommandData->pbMap + 2 * SIZEOF(DRM_DWORD), cbMessage );
        *f_pcbMessage = cbMessage;
        *f_ppbMessage = pbMessage;
        pbMessage     = NULL;
    }

    dr = DRM_SUCCESS;
ErrorExit:
    SAFE_OEM_FREE( pbMessage );
    return dr;
#endif
}

/* Note: 0 for f_dwMessageType always means "end process". */
DRM_RESULT DRM_CALL TST_OEM_AsyncShellCommandWriteMessage(
    __in                                DRM_VOID                         *f_pvAsyncShellCommandData,
    __in                                DRM_DWORD                         f_dwMessageType,
    __in_ecount(f_cbMessage)      const DRM_BYTE                         *f_pbMessage,
    __in                                DRM_DWORD                         f_cbMessage )
{
#ifdef WINCE_TEST
    return DRM_E_NOTIMPL;
#else
    DRM_RESULT              dr                      = DRM_E_TEST_INCOMPLETE;
    AsyncShellCommandData  *pAsyncShellCommandData  = NULL;
    DRM_DWORD               cbDataWrite             = 0;

    ChkArg( f_pvAsyncShellCommandData != NULL );
    pAsyncShellCommandData = (AsyncShellCommandData*)f_pvAsyncShellCommandData;

    /* Write message size */
    cbDataWrite = f_cbMessage + SIZEOF(DRM_DWORD);
    MEMCPY( pAsyncShellCommandData->pbMap,                     &cbDataWrite,     SIZEOF(DRM_DWORD) );

    /* Write message type */
    MEMCPY( pAsyncShellCommandData->pbMap + SIZEOF(DRM_DWORD), &f_dwMessageType, SIZEOF(DRM_DWORD) );

    if( f_cbMessage > 0 )
    {
        /* Write message data */
        MEMCPY( pAsyncShellCommandData->pbMap + 2*SIZEOF(DRM_DWORD), f_pbMessage, f_cbMessage );
    }

    /* Signal process that we have written data to shared memory */
    if( !SetEvent( pAsyncShellCommandData->hEvent2 ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        ChkDR( dr );
        ChkDR( E_UNEXPECTED );
    }

    dr = DRM_SUCCESS;
ErrorExit:
    return dr;
#endif
}

/*********************************************************
**
** Function: TST_OEM_CopyFile
**
** Synopsis: creates a directory at the specified path
**
** Arguments:
**  f_dstrSource -- File to copy
**  f_dstrDest   -- Destination for the copy
**
** Returns     DRM_SUCCESS if the directory is created
**             DRM_S_FALSE if the directory already exists
**             Other error code mapped from Win32 error code
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_CopyFile( __in DRM_CONST_STRING f_dstrSource,
                                      __in DRM_CONST_STRING f_dstrDest )
{
#ifndef WINCE_TEST
    DRM_RESULT  dr           = DRM_SUCCESS;
    OEM_FILEHDL fp_src       = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL fp_dst       = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD   dwFileSize   = 0;
    DRM_DWORD   dwData       = 0;
    DRM_DWORD   dwWritten    = 0;
    DRM_DWORD   dwCopyBuffer = 0;
    DRM_DWORD   dwCopied     = 0;
    DRM_BYTE   *pbData       = NULL;

    ChkArg( f_dstrSource.pwszString != NULL );
    ChkArg( f_dstrDest.pwszString   != NULL );

    fp_src = Oem_File_Open( NULL, f_dstrSource.pwszString, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL );

    if( fp_src == OEM_INVALID_HANDLE_VALUE )
    {
        TRACE( ( "TST_OEM_CopyFile failed to open source file %S.", f_dstrSource.pwszString ) );
        ChkDR( DRM_E_FILEOPEN );
    }

    if( !Oem_File_GetSize( fp_src, &dwFileSize ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    fp_dst = Oem_File_Open( NULL, f_dstrDest.pwszString, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );

    if( fp_dst == OEM_INVALID_HANDLE_VALUE )
    {
        /* Try deleting the file, it may be hidden or read-only */
        ChkDR( tRemoveFileW( f_dstrDest.pwszString ) );

        /* Try to open it again */
        fp_dst = Oem_File_Open( NULL, f_dstrDest.pwszString, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );

        if( fp_dst == OEM_INVALID_HANDLE_VALUE )
        {
            TRACE( ( "TST_OEM_CopyFile failed to open destination file %S.", f_dstrDest.pwszString ) );
            ChkDR( DRM_E_FILEOPEN );
        }
    }

    if( dwFileSize > MAX_COPY_BUFFER )
    {
        dwCopyBuffer = MAX_COPY_BUFFER;
    }
    else
    {
        dwCopyBuffer = dwFileSize;
    }

    ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( dwCopyBuffer + SIZEOF( DRM_WCHAR ) ) );

    while( dwCopied < dwFileSize )
    {
        ChkBOOL( Oem_File_Read( fp_src, pbData, dwCopyBuffer, &dwData ), DRM_E_FILE_READ_ERROR );
        ChkBOOL( Oem_File_Write( fp_dst, pbData, dwData, &dwWritten ), DRM_E_FILE_WRITE_ERROR );
        ChkBOOL( dwData == dwWritten, DRM_E_FILE_WRITE_ERROR );

        dwCopied += dwData;
        ZEROMEM( pbData, SIZEOF( pbData ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pbData );

    if( fp_src != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp_src );
    }
    if( fp_dst != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp_dst );
    }

    return dr;

#else
    DRM_RESULT dr           = DRM_SUCCESS;

    ChkDR( tRemoveFileW( f_dstrDest.pwszString ) );
    ChkWR( CopyFileW( f_dstrSource.pwszString, f_dstrDest.pwszString, FALSE ) );

ErrorExit:
    return dr;
#endif
}

/*********************************************************
**
** Function: TST_OEM_CreateDirectory
**
** Synopsis: creates a directory at the specified path
**
** Arguments:
**  f_szPath -- Directory to create.
**
** Returns     DRM_SUCCESS if the directory is created
**             DRM_S_FALSE if the directory already exists
**             Other error code mapped from Win32 error code
**
*********************************************************/
DRM_RESULT DRM_CALL TST_OEM_CreateDirectory( __in_z const DRM_CHAR *f_szPath )
{
#ifdef WINCE_TEST
    return DRM_E_NOTIMPL;
#else
    DRM_RESULT dr  = DRM_SUCCESS;

    ChkArg( NULL != f_szPath );

    if( !CreateDirectoryA( f_szPath, NULL ) )
    {
        DRM_DWORD dwError = GetLastError();

        if( ERROR_ALREADY_EXISTS == dwError )
        {
            dr = DRM_S_FALSE;
        }
        else
        {
            ChkDR( DRM_RESULT_FROM_WIN32( dwError ) );
        }
    }

ErrorExit:
    return dr;
#endif
}

DRM_VOID DRM_CALL TST_OEM_LeakScan()
{
    Oem_Test_Mem_Alloc_Check_Leakscan();
}

DRM_VOID DRM_CALL TST_OEM_LeakClear()
{
    Oem_Test_Mem_Alloc_Clear_Leakscan();
}

EXIT_PKTEST_NAMESPACE_CODE;

PREFAST_POP /* Ignore prefast warnings about reviewing CreateProcessA parameters since this is test code. */
PREFAST_POP /* Ignore prefast warning about ANSI functions for this entire file. */

