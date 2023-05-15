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

#if !SIXTEEN_BIT_ADDRESSING
/* 
** windows based include files are not available on sixteen bit platform
*/
#include <drmwindowsenv.h>
#endif


#include <tOEMImp.h>
#include <tclib.h>
#include "drmcontextsizes.h"

#include "tstutils.h"
#include "drmcrt.h"
#include <oemkeyfile.h>
#include <drmkeyfile.h>
#include <drmblackbox.h>
#include <drmerror.h>
#include <drmwmdrm.h>

#undef HANDLE
#define HANDLE	void*

ENTER_PKTEST_NAMESPACE_CODE

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

    if ( 0 == strcmp( f_pszCondition, pszKeyFileUpgrade ) )
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
#if SIXTEEN_BIT_ADDRESSING
    /*Do not have Sleep() available on 16-bit platform*/
    unsigned i;
    for(i=0;i<(dwMillSecond *1000);i++);
#else
    usleep(dwMillSecond * 1000);
#endif
}

#if !SIXTEEN_BIT_ADDRESSING
/*Functions called by tMoveFile are not available on 16bit TI platform*/
DRM_BOOL DRM_CALL tMoveFile(const DRM_WCHAR *wszSrc, const DRM_WCHAR *wszTarget)
{
//    SetFileAttributesW(wszTarget, FILE_ATTRIBUTE_NORMAL);
    return rename(wszSrc, wszTarget);
}
#endif /*sixteenbitaddressing*/

static DRM_RESULT _GetDRMFileName(
    __in_ecount(GET_DRM_FILE_NAME_PATH_LENGTH)  DRM_WCHAR            wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH],
    __out                                       DRM_CONST_STRING    *pdstrPathname,
    __in                                  const DRM_WCHAR           *wszFileName )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_DWORD        cchCopy          = 0;
    
    ZEROMEM(wszPathName, GET_DRM_FILE_NAME_PATH_LENGTH * sizeof(DRM_WCHAR));
    
    if ( pdstrPathname != NULL )
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

    if ( 0 != remove( filename ) ) 
    {
        DRM_DWORD dwError = 0/*GESTATE*/;

        /* 
        ** It is not an error if the file does not exist. 
        */ 
        if ( dwError != 2 ) /* ENOENT */
        {
            Log("Trace"," \t\tFrom tRemoveFile - Failed to Remove file %s\n", filename );
            ChkDR( DRM_RESULT_FROM_WIN32( dwError ) );
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

#if SIXTEEN_BIT_ADDRESSING
    if ( -1 == remove( szPathName ) )
    {
        /*Cannot use the errorconversion based on Windows on 16-bit*/
        Log("Trace"," \t\tFrom RemoveFileW - Failed to Remove file\n");
    }
#else
    ChkDR( tRemoveFile( szPathName ) );
#endif

ErrorExit:
    SAFE_OEM_FREE(szPathName);
    return dr;
}

DRM_RESULT DRM_CALL RemoveDRMFile(DRM_DWORD dwRemove)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrpath;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    if ( dwRemove & RMFILE_STORE )
    {
        ChkDR( tGetDeviceStorePathname(wszPathName, &dstrpath) );
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PD_DEVCERT )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, DEVCERT_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PR_DEVCERT )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, BDEVCERT_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }
                
    if ( dwRemove & RMFILE_ND_DEVCERT )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, NDR_CERT));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PD_DEVCERTTEMPLATE )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, DEVCERT_TEMPLATE_FILE_NAME));
#if defined (WINCE)
        dr = tRemoveFileW(dstrpath.pwszString);
        if ( DRM_FAILED( dr ) )
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
   
    if ( dwRemove & RMFILE_PR_DEVCERTTEMPLATE )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, GROUP_CERT_FILE_NAME));
#if defined (WINCE)
        dr = tRemoveFileW(dstrpath.pwszString);
        if ( DRM_FAILED( dr ) )
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
       
    if ( dwRemove & RMFILE_ND_DEVCERTTEMPLATE )
    {     
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, NDR_MODEL_CERT));
#if defined (WINCE)
        dr = tRemoveFileW(dstrpath.pwszString);
        if ( DRM_FAILED( dr ) )
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

    if ( dwRemove & RMFILE_PD_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, PRIVATE_KEY_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PR_GROUP_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, GC_PRIVATE_KEY_FILE_NAME));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_PR_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, Z_PRIVATE_KEY_FILE_NAME_SIGN));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_ND_GROUP_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, NDR_MODEL_PRIVATE_KEY));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_ND_PRVKEY )
    {
        ChkDR(_GetDRMFileName(wszPathName, &dstrpath, NDR_PRIVATE_KEY));
        ChkDR(tRemoveFileW(dstrpath.pwszString));
    }

    if ( dwRemove & RMFILE_KEYFILE )
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

static DRM_KEYFILE_CONTEXT s_tempKeyfileContext = {0};
DRM_RESULT DRM_CALL _StoreCertInKeyfile( DRM_KF_CERT_TYPE f_eCertType, DRM_BYTE *pbCert, DRM_DWORD cbCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fKeyfileOpen = FALSE;
    DRM_BOOL fKeyfileInitialized = FALSE;
    PRIVKEY    GCPrivkey   = {0};
    DRM_DWORD  cbGCPrivKey = SIZEOF( PRIVKEY );

    ChkArg( f_eCertType != eKF_CERT_TYPE_INVALID );
    ChkArg( pbCert != NULL );
    ChkArg( cbCert != 0 );

    ChkDR( DRM_KF_Initialize( NULL, &s_tempKeyfileContext ) );
    fKeyfileInitialized = TRUE;
    ChkDR( DRM_KF_Open( &s_tempKeyfileContext, 
                         TRUE,
                        OEM_GENERIC_READ | OEM_GENERIC_WRITE ) );
    fKeyfileOpen = TRUE;
    ChkDR( DRM_KF_LoadFromFile( &s_tempKeyfileContext, TRUE ) );

    ChkDR( DRM_KF_SetCertificate( &s_tempKeyfileContext, f_eCertType, pbCert, cbCert ) );
    ChkDR( DRM_BBX_SetupSecureStoreKey( &s_tempKeyfileContext, NULL ) );

    if( f_eCertType == eKF_CERT_TYPE_WMDRM )
    {
        DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrDevCert = EMPTY_DRM_STRING;
        PUBKEY pubkeyCurrent = {0};
        PRIVKEY privkeyCurrent = {0};
        DRM_DWORD cb = 0;
        DRM_ENCRYPTED_KEY privkey = {0};
        DRM_BYTE rgbKeyBuffer[128] = {0};

        dstrDevCert.pwszString = (DRM_WCHAR*)pbCert;
        dstrDevCert.cchString = cbCert / SIZEOF( DRM_WCHAR );
        
        ChkDR( DRM_DCP_GetAttribute( NULL, &dstrDevCert, DRM_DEVCERT_DEVICEPUBKEY, NULL, &dstrValue) );
        cb = SIZEOF( PUBKEY );
        if (DRM_FAILED(DRM_B64_DecodeW(&dstrValue, 
                                       &cb, 
                            (DRM_BYTE*)&pubkeyCurrent,
                                        0)))
        {
            ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
        }
        
        ChkDR( DRM_DCP_GetAttribute( NULL, &dstrDevCert, DRM_DEVCERT_DEVICEPRIVKEY, NULL, &dstrValue) );
        
        if ( dstrValue.cchString != CCH_BASE64_EQUIV( SIZEOF(PRIVKEY) ) )
        {
            ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
            goto ErrorExit;
        }

        cb = SIZEOF(PRIVKEY);
        if (DRM_FAILED(DRM_B64_DecodeW(&dstrValue, 
                                       &cb, 
                                       (DRM_BYTE*)&privkeyCurrent,
                                        0)))
        {
            ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
        }

        ChkDR( Oem_Device_GetKey( NULL, 
                          DRM_DKT_WMDRMPD_GROUP,
                          (DRM_BYTE *)&GCPrivkey, 
                          &cbGCPrivKey ) );

        ChkDR( DRM_BBX_SymmetricCipher( FALSE,
                                        TRUE,
                                        ( DRM_BYTE * )&GCPrivkey,
                                        SIZEOF( GCPrivkey ),                                    
                                        ( DRM_BYTE * )&privkeyCurrent,
                                        ( DRM_BYTE * )&privkeyCurrent,
                                        SIZEOF( PRIVKEY ) ) );

        privkey.cbEncryptedKey = SIZEOF(rgbKeyBuffer);
        privkey.pbEncryptedKey = rgbKeyBuffer;

        ChkDR( DRM_BBX_KF_EncryptKey ( NULL,
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

DRM_RESULT DRM_CALL _StoreKeyInKeyfile( DRM_KF_KEY_TYPE f_eKeyType, DRM_DEVICE_KEY_TYPE f_eDeviceKeyType, DRM_BYTE *pbPubKey, DRM_DWORD cbPubKey, DRM_BYTE *pbKey, DRM_DWORD cbKey )
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

        ChkDR( DRM_BBX_SymmetricCipher( FALSE,
                                        FALSE, 
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

    ChkDR( DRM_BBX_KF_EncryptKey ( NULL,
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

    if (!wszSrc)
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

    ChkBOOL(LoadTestFileW(NULL, wszSrc, &pbData, &cbData), DRM_E_FILEREADERROR);

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
    ChkBOOL(SaveToFileW(dstrDest.pwszString, pbData, cbData), DRM_E_FILEWRITEERROR);
    
ErrorExit:
    SAFE_OEM_FREE(pbData);
    return dr;
}


static DRM_RESULT _tGetTCFileListImp( __in_z char *szFileNameTemplate, __in_z char **szPlatforms, __inout_ecount_z_opt( *pdwCount ) char **szFileNames, __inout DRM_DWORD *pdwCount)
{

    DRM_RESULT dr = DRM_SUCCESS;
/*WIN32 based Function calls and data types used here are not available on TI 16-bit platform */
#if 0/*GESTATE*///!SIXTEEN_BIT_ADDRESSING
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

    hFindFile = FindFirstFileW((DRM_WCHAR_NATIVE *)dstrFileNameTemplate.pwszString, &fd);
    while (INVALID_HANDLE_VALUE != hFindFile && fMoreFiles) {
        for (szPlatform = szPlatforms; *szPlatform; szPlatform++) {
            DRM_UTL_DemoteUNICODEtoASCII( fd.cFileName, szFileName, sizeof(szFileName) );
            if (strstr(szFileName, *szPlatform)) {

#if WINCE_TEST
                /* 
                ** To accommodate WM7 (which may enumerate the same file multiple times for files 
                ** on a storage card folder), each newly enumerated file is checked against the existing
                ** files and if it already exists, simply ignore it.
                */
                if ( szFileNames != NULL ) {
                    fFound = FALSE;
                    for ( i = 0; i < dwFound; i++ ) {
                        if ( strstr( szFileNames[ i ], szFileName ) != NULL ) {
                            fFound = TRUE;
                            break;
                        }
                    }
                
                    if ( fFound )
                    {
                        break;
                    } 
                }
#endif /* WINCE_TEST */
                
                if (szFileNames && dwFound < *pdwCount) {
                    cchTemp = 0;
                    ChkDR( TST_UTL_MakePathA( NULL, szFileName, &(szFileNames[dwFound]), &cchTemp, FALSE ) );
                }
                dwFound++;
                break;
            }
        }
        fMoreFiles = FindNextFileW(hFindFile, &fd);
    }

    if (fMoreFiles) {
        dr = 0/*GESTATE*/;
        ChkDR(DRM_RESULT_FROM_WIN32(dr));
    }

    if (!szFileNames || dwFound > *pdwCount)
        dr = DRM_E_BUFFERTOOSMALL;

    *pdwCount = dwFound;
ErrorExit:
    if (hFindFile != INVALID_HANDLE_VALUE)
        FindClose(hFindFile);
    FREEDRMSTRING( dstrFileNameTemplate );
#endif /* !SIXTEEN_BIT_ADDRESSING */
    return dr;

}

DRM_RESULT DRM_CALL tGetTCFileList(const char *szTestFilePrefix, const char **szPlatforms, const char ***pszTestFileNames)
{
    DRM_RESULT dr           = DRM_SUCCESS;
    char       szFN[260]    = {0};
    char      *pszWildCards = ".*.txt";
    DRM_DWORD  cchWildcards = ( DRM_DWORD ) strlen( pszWildCards );
    DRM_DWORD  dwCount      = 0;
    DRM_DWORD  cchPrefix    = 0;
    
    ChkArg( szTestFilePrefix );
    ChkArg( szPlatforms );
    ChkArg( pszTestFileNames );

    cchPrefix = ( DRM_DWORD ) strlen( szTestFilePrefix );

    ChkOverflow( cchPrefix + cchWildcards, cchPrefix );

    ChkArg( SIZEOF( szFN ) > cchPrefix + cchWildcards );

    MEMCPY(szFN, szTestFilePrefix, cchPrefix );
    DRM_BYT_CopyBytes( szFN, cchPrefix, pszWildCards, 0, cchWildcards );
    szFN[ cchPrefix + cchWildcards ] = '\0';
    
    if (DRM_E_BUFFERTOOSMALL != (dr = _tGetTCFileListImp(szFN, (char **)szPlatforms, NULL, &dwCount))) 
    {
        ChkDR(dr);
    }

    ChkMem(*pszTestFileNames = (const char**)Oem_MemAlloc((dwCount + 1) * SIZEOF(char*)));
    ZEROMEM((char **)*pszTestFileNames, (dwCount + 1) * SIZEOF(char*));
    ChkDR(_tGetTCFileListImp(szFN, (char **)szPlatforms, (char **)*pszTestFileNames, &dwCount));
ErrorExit:
    return dr;
}

void DRM_CALL tFreeFileNameList(const char **szTestFileNames)
{
    if (szTestFileNames)
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
    

#if SIXTEEN_BIT_ADDRESSING

/*
//------------------------------------------------------------------------------
// Name: tFreeFileNameList_pfd
// Desc:  This function is used to free the memory used for storing the filenames on sixteen-bit platform. 
//
//Parameters:
//    szTestFileNames       input pointing to the location containing the list of flienames. 
//
//------------------------------------------------------------------------------*/

void DRM_CALL tFreeFileNameList_pfd(char **szTestFileNames)
{
#if defined(DRM_MSC_VER)
    if (szTestFileNames) {
        char **p = NULL;
        for (p = szTestFileNames; *p; p++)
                {
                    SAFE_OEM_FREE(*p);
                }
        SAFE_OEM_FREE(szTestFileNames);
    }

#endif /* DRM_MSC_VER */
}

/*
//------------------------------------------------------------------------------
// Name: tGetTCFileList_pfd
// Desc:  This function is used to get a list of filenames on 16-bit platform. 
//
//------------------------------------------------------------------------------*/

DRM_RESULT DRM_CALL tGetTCFileList_pfd(char *szTestFilePrefix, char **szPlatforms, char ***pszTestFileNames)
{
#ifdef DRM_MSC_VER

    DRM_RESULT dr = DRM_SUCCESS;
    char szFN[MAX_PATH] = {0};
    char      *pszWildCards = ".*.txt";
    DRM_DWORD  cchWildcards = strlen( pszWildCards );
    DRM_DWORD  dwCount      = 0;
    DRM_DWORD  cchPrefix    = 0;
    
    ChkArg( szTestFilePrefix );
    ChkArg( szPlatforms );
    ChkArg( pszTestFileNames );

    cchPrefix = strlen( szTestFilePrefix );

    ChkArg( SIZEOF( szFN ) > cchPrefix + cchWildcards );

    MEMCPY(szFN, szTestFilePrefix, cchPrefix );
    DRM_BYT_CopyBytes( szFN, cchPrefix, pszWildCards, 0, cchWildcards );
    szFN[ cchPrefix + cchWildcards ] = '\0';


    if (DRM_E_BUFFERTOOSMALL != (dr = _tGetTCFileListImp(szFN, szPlatforms, NULL, &dwCount))) {
        ChkDR(dr);
    }

    ChkMem(*pszTestFileNames = (char**)Oem_MemAlloc((dwCount + 1) * SIZEOF(char*)));
    ZeroMemory(*pszTestFileNames, (dwCount + 1) * sizeof(char*));
    ChkDR(_tGetTCFileListImp(szFN, szPlatforms, *pszTestFileNames, &dwCount));
ErrorExit:
    return dr;
#else /* DRM_MSC_VER */
#ifdef __TMS320C55X__
    static char *testfile = "files\\DailyTestDrmManager.pc.pk.ce.txt";
    *pszTestFileNames = &testfile;
    return DRM_SUCCESS;
#else
    return DRM_E_NOTIMPL;
#endif

#endif /* DRM_MSC_VER */
}            
#endif /* SIXTEEN_BIT_ADDRESSING */


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
#ifdef __GNUC__/* GESTATE */ //WINCE_TEST
    return DRM_E_NOTIMPL;
#else

    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_BYTE  *pbData = NULL;
    DRM_DWORD  cbData = 0;
    DRM_DWORD  cbRead = 0;
    
    /*
    ** Check amount of data present in the pipe. It is written to cbData variable
    */
    
    if ( !PeekNamedPipe( f_hToolStdOut, NULL, 0, NULL, (LPDWORD)&cbData, NULL ) )
    {
        dr = DRM_RESULT_FROM_WIN32( 0/*GESTATE*/ );
        Log( "Trace", "PeekNamedPipe failed.");
        goto ErrorExit;
    }

    /*
    ** If pipe is not empty, read from it and write to log file.
    */
    if ( cbData > 0 )
    {
        ChkMem( pbData = ( DRM_BYTE * ) Oem_MemAlloc( cbData ) );
    
        if ( !ReadFile( f_hToolStdOut, pbData, cbData, (LPDWORD)&cbRead, NULL ) )
        {
            dr = DRM_RESULT_FROM_WIN32( 0/*GESTATE*/ );
            Log( "Trace", "ReadFile failed.");
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
#ifdef __GNUC__ /* GESTATE *///WINCE_TEST
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
        dr = DRM_RESULT_FROM_WIN32( 0/*GESTATE*/ );
        Log( "Trace", "Failed to create out pipe!");
        goto ErrorExit;
    }
    SetHandleInformation( hStdOutRd, HANDLE_FLAG_INHERIT, 0);
    
    if( !CreatePipe( &hStdInRd, &hStdInWr, &saAttr, PIPE_SIZE ) )
    {
        dr = DRM_RESULT_FROM_WIN32( 0/*GESTATE*/ );
        Log( "Trace", "Failed to create in pipe!");
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
        dr = DRM_RESULT_FROM_WIN32( 0/*GESTATE*/ );
        Log( "Trace", "Failed to create process!");
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

        if ( hStdOutRd != NULL )
        {
            ChkDR( WriteToolOutputToLog( hStdOutRd ) );
        }

        /*
        ** Log in case of wait failure. It may happen if handle of process invalid.
        */ 

        if ( dwWaitResult == WAIT_FAILED || dwWaitResult == WAIT_ABANDONED )
        {
            dr = DRM_RESULT_FROM_WIN32( 0/*GESTATE*/ );
            Log( "Trace", "WaitForSingleObject failed.");
            goto ErrorExit;
        }
    }
    
    if( !GetExitCodeProcess( processInfo.hProcess, (LPDWORD)&dwExitCode ) )
    {
        dr = DRM_RESULT_FROM_WIN32( 0/*GESTATE*/ );
        Log( "Trace", "GetExitCodeProcess failed.");
        goto ErrorExit;
    }
    
    if( dwExitCode != 0 )
    {
        Log( "Trace", "Tool exited with non-zero error code!");
        dr = dwExitCode;
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
        ChkBOOL( Oem_File_Read( fp_src, pbData, dwCopyBuffer, &dwData ), DRM_E_FILEREADERROR );
        ChkBOOL( Oem_File_Write( fp_dst, pbData, dwData, &dwWritten ), DRM_E_FILEWRITEERROR );
        ChkBOOL( dwData == dwWritten, DRM_E_FILEWRITEERROR );

        dwCopied += dwData;
        ZEROMEM( pbData, SIZEOF( pbData ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pbData );

    if ( fp_src != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp_src );
    }
    if ( fp_dst != OEM_INVALID_HANDLE_VALUE )
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
DRM_RESULT DRM_CALL TST_OEM_CreateDirectory( __in_z DRM_CHAR *f_szPath )
{
#ifdef __GNUC__ /* GESTATE */ //WINCE_TEST
    return DRM_E_NOTIMPL;
#else
    DRM_RESULT dr  = DRM_SUCCESS;

    ChkArg( NULL != f_szPath );

    if( !CreateDirectoryA( f_szPath, NULL ) )
    {
        DRM_DWORD dwError = 0/*GESTATE*/;
        
        if ( ERROR_ALREADY_EXISTS == dwError )
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

EXIT_PKTEST_NAMESPACE_CODE
