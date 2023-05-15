/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#undef __STRICT_ANSI__
#include <stdio.h>
#include <stdarg.h>
#include <drmcommon.h>
#include <errno.h>

#ifndef DRM_MSC_VER

/*
** Locking support in this file is implemented in terms of Microsoft compiler
** specific APIs
*/
#undef DRM_SUPPORT_LOCKING
#endif

#if DRM_SUPPORT_LOCKING
#include <io.h>
#include <share.h>
#include <sys/locking.h>
#endif

#include <drmutilities.h>
#include <drmcrt.h>
#include <oem.h>

ENTER_PK_NAMESPACE_CODE;

#include "bstd.h"
#include "bdbg.h"
BDBG_MODULE(oem_fileio);

#if CB_NATIVE_BYTE > 1
/* The size of the buffer when reading files on 16-bit */
/* Must be an even number */
#define OEM_READFILE_CHUNK_SIZE 128
#endif

/**********************************************************************
** Function:    NativeBytesToPacked
** Synopsis:    Copy native byte string to packed string 
** Arguments:   [f_pNativeString] -- input native string
**              [f_pPackedString] -- output packed string 
**                                   packed string is different from native string if 16-bit addressing is used
**              [f_cch]           -- size of input string in elements
**                                   The NULL terminator should be included in the count if it is to be copied
***********************************************************************/
#if CB_NATIVE_BYTE > 1
static void NativeBytesToPacked(
    __in_bcount(f_cch)  DRM_BYTE *f_pNativeString,
    __out_ecount(f_cch) DRM_BYTE *f_pPackedString,
                        DRM_DWORD f_cch )
{
    DRM_DWORD ich = 0;

    if( f_pNativeString == NULL
     || f_pPackedString == NULL
     || f_cch == 0 )
    {
        return;
    }

    for(ich = 1; ich <= f_cch; ich++ )
    {
        PUT_CHAR(f_pPackedString, f_cch - ich, f_pNativeString[f_cch - ich]);
    }
}
#endif /* CB_NATIVE_BYTE > 1 */

/**********************************************************************
** Function:    PackedCharsToNative
** Synopsis:    Convert packed string to native byte string
** Arguments:   [f_pPackedString] -- packed string, 
**                                   size of the string should be sufficient for conversion to native byte string
**              [f_cch]           -- size of input string in elements. 
**                                   The NULL terminator should be included in the count if it is to be copied
***********************************************************************/
static void PackedCharsToNative(
    __inout_ecount(f_cch) DRM_CHAR *f_pPackedString,
                          DRM_DWORD f_cch )
{
    DRM_DWORD ich = 0;

    if( f_pPackedString == NULL
     || f_cch == 0 )
    {
        return;
    }
    for( ich = 1; ich <= f_cch; ich++ )
    {
        f_pPackedString[f_cch - ich] = GET_CHAR( f_pPackedString, f_cch - ich );
    }
}

/**********************************************************************
** Function:    FileExists
** Synopsis:    Verifies if specified file exists
** Arguments:   [f_pszFileName] -- file name 
** Returns:     TRUE if specified file exists, 
**              FALSE otherwise
**
***********************************************************************/
#ifdef PRDY_ROBUSTNESS
DRM_API DRM_BOOL DRM_CALL FileExists(
    __in const DRM_CHAR *f_pszFileName )
#else
static DRM_BOOL FileExists(
    __in const DRM_CHAR *f_pszFileName )
#endif
{
    FILE     *pfile   = NULL;
    DRM_BOOL  bExists = FALSE;

    if ((pfile = (FILE*)fopen (f_pszFileName, "r")) != NULL)
    {
        bExists = TRUE;
        
        fclose (pfile);
    }
    
    return bExists;
}

#ifdef PRDY_ROBUSTNESS
DRM_API DRM_RESULT DRM_CALL Oem_File_Unlink(
    __in_z const DRM_WCHAR *f_pwszFileName
)
{
    DRM_CHAR  rgchFileName[DRM_MAX_PATH];
    DRM_DWORD cch      = 0;
    DRM_RESULT dr      = DRM_SUCCESS;

    ChkArg( f_pwszFileName != NULL );

     /* Safe to use, assume that input parameter is ASCII */
    DRM_UTL_DemoteUNICODEtoASCII( f_pwszFileName, rgchFileName, DRM_MAX_PATH );

    /* Number of characters not including NULL-terminator */
    ChkDR( DRM_STR_StringCchLengthW( f_pwszFileName, DRM_MAX_PATH, &cch));
    
    /* Check if there is space for NULL terminator */
    ChkBOOL( cch < DRM_MAX_PATH, DRM_E_BUFFERTOOSMALL );
    if (unlink(rgchFileName) < 0)
    {
        /* It is OK for unlink to fail when the file does not exist */
        if (errno != ENOENT){
            dr = DRM_E_FAIL;
        }
    }

ErrorExit:
    return dr;
}
#endif

DRM_API OEM_FILEHDL DRM_CALL Oem_File_Open(
    __in_opt   DRM_VOID  *f_pOEMContext,
    __in_z const DRM_WCHAR *f_pwszFileName,
    __in       DRM_DWORD  f_dwAccessMode,
    __in       DRM_DWORD  f_dwShareMode,
    __in       DRM_DWORD  f_dwCreationDisposition,
    __in       DRM_DWORD  f_dwAttributes )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    FILE            *pfile      = (FILE *)OEM_INVALID_HANDLE_VALUE;
    const DRM_CHAR  *pszMode    = NULL;
#if DRM_SUPPORT_LOCKING
    DRM_DWORD        fShare     = 0;
#endif
    DRM_BOOL         bOK        = FALSE;

    DRM_DWORD        cch        = 0;
    DRM_CHAR         rgchFileName[DRM_MAX_PATH];    
    const DRM_CHAR  *pszFName   = rgchFileName;
    
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Open);

    ChkArg( f_pwszFileName != NULL );
    
    /* Number of characters not including NULL-terminator */
    ChkDR( DRM_STR_StringCchLengthW( f_pwszFileName, DRM_MAX_PATH, &cch ) );
    
    /* Check if there is space for NULL terminator */
    ChkBOOL( cch < DRM_MAX_PATH, DRM_E_BUFFERTOOSMALL );
    
    /* Safe to use, assume that input parameter is ASCII */
    DRM_UTL_DemoteUNICODEtoASCII( f_pwszFileName, rgchFileName, DRM_MAX_PATH );

    /* Enforce NULL termination */
    PUT_CHAR(rgchFileName, cch, 0);

    /* Pack characters to native bytes, including NULL-terminator */
    PackedCharsToNative( rgchFileName, cch + 1 );

    if  (f_dwCreationDisposition == OEM_CREATE_NEW) 
    {
        DRMASSERT ((f_dwAccessMode & OEM_GENERIC_WRITE) != 0);

        bOK = (FileExists (pszFName) == FALSE);

        pszMode = "w+b";
    }
    else if (f_dwCreationDisposition == OEM_TRUNCATE_EXISTING)
    {
        DRMASSERT ((f_dwAccessMode & OEM_GENERIC_WRITE) != 0);

        bOK = FileExists (pszFName);

        pszMode = "w+b";
    }
    else if (f_dwCreationDisposition == OEM_CREATE_ALWAYS)
    {
        DRMASSERT ((f_dwAccessMode & OEM_GENERIC_WRITE) != 0);

        bOK      = TRUE;
        pszMode = "w+b";
    }
    else if (f_dwCreationDisposition == OEM_OPEN_EXISTING)
    {
        if (FileExists (pszFName))
        {
            if (f_dwAccessMode & OEM_GENERIC_WRITE)
            {
                pszMode = "r+b";
                bOK     = TRUE;
            }
            else if (f_dwAccessMode & OEM_GENERIC_READ)
            {
                pszMode = "rb";
                bOK     = TRUE;
            }
            else
            {
                DRMASSERT(FALSE);
            }
        }
    }
    else if (f_dwCreationDisposition == OEM_OPEN_ALWAYS)
    {
        if (FileExists (pszFName))
        {
            if ( ( f_dwAccessMode & OEM_GENERIC_WRITE )
              && ( f_dwAccessMode & OEM_GENERIC_READ ) )
            {
                pszMode = "r+b";
                bOK     = TRUE;
            }
            else if (f_dwAccessMode & OEM_GENERIC_READ)
            {
                pszMode = "rb";
                bOK     = TRUE;
            }
            else
            {
                DRMASSERT(FALSE);
            }
        }
        else
        {
            if ( ( f_dwAccessMode & OEM_GENERIC_WRITE )
              && ( f_dwAccessMode & OEM_GENERIC_READ ) )
            {
                pszMode = "w+b";
                bOK     = TRUE;
            }
            else if ( f_dwAccessMode & OEM_GENERIC_WRITE )
            {
                pszMode = "wb";
                bOK     = TRUE;
            }
            else
            {
                DRMASSERT(FALSE);
            }
        }
    }

    /* translate our I/O share flags to standard C's */
#if DRM_SUPPORT_LOCKING    
    if ((f_dwShareMode & OEM_FILE_SHARE_WRITE)
    &&  (f_dwShareMode & OEM_FILE_SHARE_READ))
    {
        fShare = _SH_DENYNO;
    }
    else if (f_dwShareMode & OEM_FILE_SHARE_WRITE)
    {
        fShare = _SH_DENYRD;
    }
    else if (f_dwShareMode & OEM_FILE_SHARE_READ)
    {
        fShare = _SH_DENYWR;
    }
    else
    {
        fShare = _SH_DENYRW;
    }

#endif
    if (bOK)
    {
#if DRM_SUPPORT_LOCKING
        pfile = (FILE *) _fsopen (pszFName, pszMode, fShare);
#else
        pfile = (FILE *) fopen (pszFName, pszMode);
#endif
        if (pfile == NULL)
        {
            pfile = (FILE *) OEM_INVALID_HANDLE_VALUE;
        }
        else
        {
            if( f_dwAttributes & OEM_FILE_FLAG_WRITE_THROUGH )
            {
                /* If the caller is asking us to write through, disable
                ** the file I/O buffering. This doesn't neccessarily 
                ** write through directly to physical media, but
                ** will mean that all file handles to the same file
                ** should at least see the same data. Writes must
                ** make it to physical media expidiently if this
                ** option is used.
                */
                setvbuf( pfile, NULL, _IONBF, 0 );
            }
        }
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? OEM_INVALID_HANDLE_VALUE : (OEM_FILEHDL)pfile;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_Delete(
    __in_z const DRM_WCHAR *f_pwszFileName )
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    DRM_DWORD        cch      = 0;
    DRM_CHAR         rgchFileName[DRM_MAX_PATH];    
    const DRM_CHAR  *pszFName = rgchFileName;
    
    ChkArg( f_pwszFileName != NULL );
    
    /* Number of characters not including NULL-terminator */
    ChkDR( DRM_STR_StringCchLengthW( f_pwszFileName, DRM_MAX_PATH, &cch ) );
    
    /* Check if there is space for NULL terminator */
    ChkBOOL( cch < DRM_MAX_PATH, DRM_E_BUFFERTOOSMALL );
    
    /* Safe to use, assume that input parameter is ASCII */
    DRM_UTL_DemoteUNICODEtoASCII( f_pwszFileName, rgchFileName, DRM_MAX_PATH );

    /* Enforce NULL termination */
    PUT_CHAR(rgchFileName, cch, 0);

    /* Pack characters to native bytes, including NULL-terminator */
    PackedCharsToNative( rgchFileName, cch + 1 );

    if( 0 != remove(pszFName) )
    {
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
    return DRM_SUCCEEDED(dr) ? TRUE : FALSE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_Close(
    __in OEM_FILEHDL f_hFile)
{
    if( f_hFile == OEM_INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }
    else
    {
        return fclose((FILE *)f_hFile)==0;
    }
}

DRM_API DRM_BOOL DRM_CALL Oem_File_Read(
    __in_opt OEM_FILEHDL f_hFile,
    __out_ecount_part( f_nNumberOfBytesToRead, *f_pNumberOfBytesRead ) DRM_VOID *f_pBuffer,
    __in DRM_DWORD f_nNumberOfBytesToRead,
    __out DRM_DWORD *f_pNumberOfBytesRead)
{
    DRM_RESULT dr = DRM_SUCCESS;
#if CB_NATIVE_BYTE > 1
    DRM_BYTE rgbNative[OEM_READFILE_CHUNK_SIZE];
    DRM_DWORD actual = 0;
    DRM_DWORD ib = 0;
#endif

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Read);
    DRM_PROFILING_DETAIL_LEVEL(3);

    ChkArg( f_hFile   != OEM_INVALID_HANDLE_VALUE
     &&     f_pBuffer != NULL 
     &&     f_pNumberOfBytesRead != NULL );
    
#if CB_NATIVE_BYTE > 1
    *f_pNumberOfBytesRead = 0;
    while ( f_nNumberOfBytesToRead )
    {
        actual = fread(rgbNative, 1, min(f_nNumberOfBytesToRead, OEM_READFILE_CHUNK_SIZE), (FILE *)f_hFile);
        if ( 0 == actual )
        {
            break;
        }
        NativeBytesToPacked(rgbNative, (DRM_BYTE*)f_pBuffer + __CB_DECL(ib * OEM_READFILE_CHUNK_SIZE), actual);
        f_nNumberOfBytesToRead -= actual;
        *f_pNumberOfBytesRead += actual;
        ib++;
    }
#else  
    *f_pNumberOfBytesRead = (DRM_DWORD)fread(f_pBuffer, 1, (size_t)f_nNumberOfBytesToRead, (FILE *)f_hFile);
#endif

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? FALSE : TRUE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_Write(
    __in OEM_FILEHDL f_hFile,
    __in_ecount( f_nNumberOfBytesToWrite ) DRM_VOID *f_pBuffer,
    __in DRM_DWORD f_nNumberOfBytesToWrite,
    __out DRM_DWORD *f_pNumberOfBytesWritten )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if CB_NATIVE_BYTE > 1
    DRM_DWORD ib = 0;
    DRM_BYTE rgbNative[2];
#endif
        
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Write);
    DRM_PROFILING_DETAIL_LEVEL(3);

    ChkArg( f_hFile       != OEM_INVALID_HANDLE_VALUE
     &&     f_pBuffer     != NULL
     &&     f_pNumberOfBytesWritten  != NULL );

#if CB_NATIVE_BYTE > 1
    for( ib = 0; f_nNumberOfBytesToWrite > 1; ib+=2, f_nNumberOfBytesToWrite-=2 )
    {
        rgbNative[0] = GET_BYTE( f_pBuffer, ib );
        rgbNative[1] = GET_BYTE( f_pBuffer, ib+1 );        
        if( 1 != fwrite(rgbNative, sizeof(rgbNative), 1, (FILE *)f_hFile) )
        {
            break;
        }
    }
    if( f_nNumberOfBytesToWrite == 1 )
    {    
        rgbNative[0] = GET_BYTE( f_pBuffer, ib );
        if( fwrite(rgbNative, 1, 1, (FILE *)f_hFile) == 1 )
        {            
            ib++;
        }
    }    
    *f_pNumberOfBytesWritten = ib;
#else
    *f_pNumberOfBytesWritten = (DRM_DWORD)fwrite(f_pBuffer, 1, (size_t)f_nNumberOfBytesToWrite, (FILE *)f_hFile);
#endif

    ChkBOOL( *f_pNumberOfBytesWritten == f_nNumberOfBytesToWrite, DRM_E_FILEWRITEERROR );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? FALSE : TRUE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_SetFilePointer(
    __in OEM_FILEHDL f_hFile,
    __in DRM_LONG f_lDistanceToMove,
    __in DRM_DWORD f_dwMoveMethod,
    __out_opt DRM_DWORD *f_pdwNewFilePointer)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_SetFilePointer);
    DRM_PROFILING_DETAIL_LEVEL(3);

    ChkArg( f_hFile != OEM_INVALID_HANDLE_VALUE );

    if (fseek((FILE *)f_hFile,f_lDistanceToMove,(DRM_LONG)f_dwMoveMethod)==0)
    {
        if (f_pdwNewFilePointer!=NULL)
        {
            *f_pdwNewFilePointer = ftell((FILE *)f_hFile);
        }
    }
    else
    {
        ChkDR( DRM_E_FILESEEKERROR );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? FALSE : TRUE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_Lock(
    __in OEM_FILEHDL f_hFile,
    __in DRM_BOOL f_fExclusive,
    __in DRM_DWORD f_dwFileOffset,
    __in DRM_DWORD f_nNumberOfBytesToLock,
    __in DRM_BOOL f_fWait)
{
#if DRM_SUPPORT_LOCKING
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_LONG flag = 0;
    
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Lock);
    DRM_PROFILING_DETAIL_LEVEL(3);

    ChkArg( f_hFile != OEM_INVALID_HANDLE_VALUE );

    flag = f_fWait? _LK_LOCK : _LK_NBLCK;

    if (fseek((FILE *)f_hFile, f_dwFileOffset, SEEK_SET) == 0 )
    {
        /* _locking does not support non-exclusive mode, we ignore fExclusive */
        if (_locking(_fileno((FILE *)f_hFile), flag, (DRM_LONG)f_nNumberOfBytesToLock)!=0)
        {
            ChkDR( DRM_E_FAIL );
        }
    }
    else
    {
        ChkDR( DRM_E_FILESEEKERROR );
    }
    
ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? FALSE : TRUE;
#else
    return TRUE;
#endif
}

DRM_API DRM_BOOL DRM_CALL Oem_File_Unlock(
    __in OEM_FILEHDL f_hFile,
    __in DRM_DWORD f_dwFileOffset,
    __in DRM_DWORD f_nNumberOfBytesToUnlock)
{
#if DRM_SUPPORT_LOCKING
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Unlock);
    DRM_PROFILING_DETAIL_LEVEL(4);

    ChkArg( f_hFile != OEM_INVALID_HANDLE_VALUE );
        
    if (fseek((FILE *)f_hFile, f_dwFileOffset, SEEK_SET) == 0)
    {
        if (_locking(_fileno((FILE *)f_hFile), _LK_UNLCK, (DRM_LONG)f_nNumberOfBytesToUnlock)!=0)
        {
            ChkDR( DRM_E_FAIL );
        }
    }
    else
    {
        ChkDR( DRM_E_FILESEEKERROR );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? FALSE : TRUE;
#else
    return TRUE;
#endif
}

DRM_API DRM_BOOL DRM_CALL Oem_File_SetEndOfFile(
    __in OEM_FILEHDL f_hFile)
{
    /* no CRT support */
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_GetSize(
    __in OEM_FILEHDL f_hFile,
    __out DRM_DWORD *f_pcbFile)
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD oFileCurr = 0;
    FILE     *pfile     = (FILE *) f_hFile;
    
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_GetSize);
    DRM_PROFILING_DETAIL_LEVEL(4);

    ChkArg( f_hFile   != OEM_INVALID_HANDLE_VALUE
     ||     f_pcbFile != NULL );

    oFileCurr = ftell (pfile);  /* remember the current location within the file */

    if (fseek (pfile, 0, SEEK_END) == 0)  /* go to the end of the file */
    {
        *f_pcbFile = ftell (pfile);
        if (fseek (pfile, oFileCurr, SEEK_SET) != 0)  /* reset back to the original location */
        {
            ChkDR( DRM_E_FILESEEKERROR );
        }
    }
    else
    {
        ChkDR( DRM_E_FILESEEKERROR );
    }
    
ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? FALSE : TRUE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_FlushBuffers(
    __in OEM_FILEHDL f_hFile)
{
#if DRM_SUPPORT_TRACING
    DRM_RESULT dr = DRM_SUCCESS;
#endif
    DRM_BOOL fResult = FALSE;
    
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_FlushBuffers);
    DRM_PROFILING_DETAIL_LEVEL(2);
    
    if( f_hFile != OEM_INVALID_HANDLE_VALUE )
    {
        fResult = fflush((FILE *)f_hFile)==0;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return fResult;
}

EXIT_PK_NAMESPACE_CODE;
