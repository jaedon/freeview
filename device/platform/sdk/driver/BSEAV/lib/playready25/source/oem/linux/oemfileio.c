/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#undef __STRICT_ANSI__
#define DRM_BUILDING_OEMFILEIO_C
#include <stdio.h>
#include <stdarg.h>
#include <drmcommon.h>
#include <errno.h>

#include <drmcompiler.h>

#include <stdbool.h> // vspector
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include <drmutilities.h>
#include <drmcrt.h>
#include <oem.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#ifndef HUMAX_PLATFORM_BASE
#include "bstd.h"
#include "bdbg.h"
#endif

BDBG_MODULE(oem_fileio);

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Changing parameters to const to satisfy warning would make them not match the standard OEM interface.");

#define DRM_LOCK_WAIT_DURATION  10000   /* ~10 seconds   */

static pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER; // vspector - this macro should only be used for static mutexes, and only once
#if 0
#ifndef __cplusplus
typedef enum {false, true} bool;
#endif
#endif

typedef struct __tagOEM_LINUX_FILE_LOCK_ENTRY {
    bool                                   fExclusive;
    unsigned int                           dwFileOffset;
    unsigned int                           dwBytesLocked;
    struct __tagOEM_LINUX_FILE_LOCK_ENTRY *pNext;
} OEM_LINUX_FILE_LOCK_ENTRY;

typedef struct {
    FILE                        *pfile;
    pthread_mutex_t              oCS;
    OEM_LINUX_FILE_LOCK_ENTRY   *pFileLocks;
} OEM_LINUX_FILE_CONTEXT;

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
    __in_opt   DRM_VOID    *f_pOEMContext,
    __in_z const DRM_WCHAR *f_pwszFileName,
    __in       DRM_DWORD    f_dwAccessMode,
    __in       DRM_DWORD    f_dwShareMode,
    __in       DRM_DWORD    f_dwCreationDisposition,
    __in       DRM_DWORD    f_dwAttributes )
{
    DRM_RESULT              dr       = DRM_SUCCESS;

    OEM_LINUX_FILE_CONTEXT *pFileCtx = (OEM_LINUX_FILE_CONTEXT *)OEM_INVALID_HANDLE_VALUE;
    FILE                   *pfile    = NULL;

    const DRM_CHAR         *pszMode  = NULL;
    DRM_BOOL                bOK      = FALSE;

    DRM_DWORD               cch      = 0;
    DRM_CHAR                rgchFileName[DRM_MAX_PATH];
    const DRM_CHAR         *pszFName = rgchFileName;

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

    if (bOK)
    {

        pfile = (FILE *) fopen (pszFName, pszMode);

        if (pfile != NULL)
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

            pFileCtx = (OEM_LINUX_FILE_CONTEXT*)Oem_MemAlloc( SIZEOF( OEM_LINUX_FILE_CONTEXT ) );
            if( pFileCtx != NULL )
            {
                ZEROMEM( pFileCtx, SIZEOF( OEM_LINUX_FILE_CONTEXT ) );
                pFileCtx->pfile = pfile;
                //pFileCtx->oCS = PTHREAD_MUTEX_INITIALIZER;
#ifndef __cplusplus
				if(pthread_mutex_init(&pFileCtx->oCS,NULL) != 0){
                	DRMASSERT(FALSE);
                }
#else
                pFileCtx->oCS = global_mutex; // vspector;
#endif

            }
            else
            {
                fclose( pfile );
                ChkDR( DRM_E_FAIL );
            }
        }
        else
        {
            ChkDR( DRM_E_FAIL );
        }
    }


ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? OEM_INVALID_HANDLE_VALUE : (OEM_FILEHDL)pFileCtx;
}

#ifdef TEE
DRM_API DRM_BOOL DRM_CALL Oem_File_Rename(

    __in_z const DRM_WCHAR *f_pwszFileNameFrom,
    __in_z const DRM_WCHAR *f_pwszFileNameTo )

{
    DRM_RESULT       dr       = DRM_SUCCESS;
    DRM_DWORD        cch      = 0;
    DRM_CHAR         rgchFileNameFrom[DRM_MAX_PATH];
    DRM_CHAR         rgchFileNameTo[DRM_MAX_PATH];

    ChkArg( f_pwszFileNameFrom != NULL );
    ChkArg( f_pwszFileNameTo != NULL );

    {
    /* Number of characters not including NULL-terminator */
    ChkDR( DRM_STR_StringCchLengthW( f_pwszFileNameFrom, DRM_MAX_PATH, &cch ) );

    /* Check if there is space for NULL terminator */
    ChkBOOL( cch < DRM_MAX_PATH, DRM_E_BUFFERTOOSMALL );

    /* Safe to use, assume that input parameter is ASCII */
    DRM_UTL_DemoteUNICODEtoASCII( f_pwszFileNameFrom, rgchFileNameFrom, DRM_MAX_PATH );

    /* Enforce NULL termination */
    PUT_CHAR(rgchFileNameFrom, cch, 0);

    /* Pack characters to native bytes, including NULL-terminator */
    PackedCharsToNative( rgchFileNameFrom, cch + 1 );
    }

    {
    /* Number of characters not including NULL-terminator */
    ChkDR( DRM_STR_StringCchLengthW( f_pwszFileNameTo, DRM_MAX_PATH, &cch ) );

    /* Check if there is space for NULL terminator */
    ChkBOOL( cch < DRM_MAX_PATH, DRM_E_BUFFERTOOSMALL );

    /* Safe to use, assume that input parameter is ASCII */
    DRM_UTL_DemoteUNICODEtoASCII( f_pwszFileNameTo, rgchFileNameTo, DRM_MAX_PATH );

    /* Enforce NULL termination */
    PUT_CHAR(rgchFileNameTo, cch, 0);

    /* Pack characters to native bytes, including NULL-terminator */
    PackedCharsToNative( rgchFileNameTo, cch + 1 );
    }

#if defined(__ORBIS__) || defined(__CELLOS_LV2__)
        remove(rgchFileNameTo);
#endif

    if( 0 != rename(rgchFileNameFrom, rgchFileNameTo) )
    {
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:

    return DRM_SUCCEEDED(dr) ? TRUE : FALSE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_GetHash(
    __in_z const DRM_WCHAR *f_pwszFileName,
    __out_bcount( SHA256_DIGEST_SIZE_IN_BYTES ) DRM_BYTE *f_pbFileHash)
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    ChkArg( f_pwszFileName != NULL );
    ChkArg( f_pbFileHash != NULL );

    OEM_FILEHDL hFile = OEM_INVALID_HANDLE_VALUE;

    hFile = Oem_File_Open(
        NULL,
        f_pwszFileName,
        OEM_GENERIC_READ,
        OEM_FILE_SHARE_READ,
        OEM_OPEN_EXISTING,
        0 );

    if( OEM_INVALID_HANDLE_VALUE != hFile)
    {
        DRM_DWORD nRead = 0;
#define TMP_READ_BUF_SIZE 256
        DRM_BYTE buf[TMP_READ_BUF_SIZE];
        DRM_SHA256_Context sha256Context = { 0 };
        ChkDR(DRM_SHA256_Init( &sha256Context ) );
        while ( 1 ) {
            ChkBOOL( Oem_File_Read( hFile, buf, TMP_READ_BUF_SIZE, &nRead), DRM_E_FILE_READ_ERROR );
            if ( nRead == 0 ) break;
            ChkDR( DRM_SHA256_Update( &sha256Context, buf, nRead ) );
            if ( nRead != TMP_READ_BUF_SIZE ) break;
        }
#undef TMP_READ_BUF_SIZE

        DRM_SHA256_Digest digestValue = { 0 };
        ChkDR( DRM_SHA256_Finalize( &sha256Context, &digestValue ) );
        DRM_BYT_CopyBytes( f_pbFileHash, 0, digestValue.m_rgbDigest, 0, SHA256_DIGEST_SIZE_IN_BYTES );
    }
    else
    {
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

ErrorExit:
    if( OEM_INVALID_HANDLE_VALUE != hFile)
    {
        (void)Oem_File_Close( hFile );
    }

    return DRM_SUCCEEDED(dr) ? TRUE : FALSE;
}
#endif

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
    OEM_LINUX_FILE_CONTEXT *pFileCtx = (OEM_LINUX_FILE_CONTEXT*)f_hFile;
    DRM_BOOL                fResult  = FALSE;

    if( pFileCtx == NULL || pFileCtx == OEM_INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }


    /* Nobody should hold any outstanding file locks */
    DRMASSERT( pFileCtx->pFileLocks == NULL );

    fResult = (fclose(pFileCtx->pfile)==0);

    /* Free any remaning allocated locks. There really shouldn't have been
    ** any but we have to free them regardless.
    */
    while( pFileCtx->pFileLocks != NULL )
    {
        OEM_LINUX_FILE_LOCK_ENTRY *pTemp = pFileCtx->pFileLocks;
        pFileCtx->pFileLocks = pTemp->pNext;
        SAFE_OEM_FREE( pTemp );
    }

    SAFE_OEM_FREE( pFileCtx );

    return fResult;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_Read(
    __in_opt OEM_FILEHDL                                                         f_hFile,
    __out_ecount_part( f_nNumberOfBytesToRead, *f_pNumberOfBytesRead ) DRM_VOID *f_pBuffer,
    __in DRM_DWORD                                                               f_nNumberOfBytesToRead,
    __out DRM_DWORD                                                             *f_pNumberOfBytesRead)
{
    OEM_LINUX_FILE_CONTEXT *pFileCtx = (OEM_LINUX_FILE_CONTEXT*)f_hFile;


    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Read);
    DRM_PROFILING_DETAIL_LEVEL(3);

    if( pFileCtx == NULL || pFileCtx == OEM_INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }

    if( f_pBuffer == NULL || f_pNumberOfBytesRead == NULL)
    {
        return FALSE;
    }

    *f_pNumberOfBytesRead = (DRM_DWORD)fread(f_pBuffer, 1, (size_t)f_nNumberOfBytesToRead, pFileCtx->pfile);

    DRM_PROFILING_LEAVE_SCOPE;
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_Write(
    __in OEM_FILEHDL                                 f_hFile,
    __in_ecount( f_nNumberOfBytesToWrite ) DRM_VOID *f_pBuffer,
    __in DRM_DWORD                                   f_nNumberOfBytesToWrite,
    __out DRM_DWORD                                 *f_pNumberOfBytesWritten )
{
    DRM_RESULT              dr       = DRM_SUCCESS;
    OEM_LINUX_FILE_CONTEXT *pFileCtx = (OEM_LINUX_FILE_CONTEXT*)f_hFile;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Write);
    DRM_PROFILING_DETAIL_LEVEL(3);

    if( pFileCtx == NULL || pFileCtx == OEM_INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }

    if( f_pBuffer == NULL || f_pNumberOfBytesWritten == NULL)
    {
        return FALSE;
    }

    *f_pNumberOfBytesWritten = (DRM_DWORD)fwrite(f_pBuffer, 1, (size_t)f_nNumberOfBytesToWrite, pFileCtx->pfile);

    ChkBOOL( *f_pNumberOfBytesWritten == f_nNumberOfBytesToWrite, DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? FALSE : TRUE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_SetFilePointer(
    __in OEM_FILEHDL     f_hFile,
    __in DRM_LONG        f_lDistanceToMove,
    __in DRM_DWORD       f_dwMoveMethod,
    __out_opt DRM_DWORD *f_pdwNewFilePointer)
{
    DRM_RESULT              dr       = DRM_SUCCESS;
    OEM_LINUX_FILE_CONTEXT *pFileCtx = (OEM_LINUX_FILE_CONTEXT*)f_hFile;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_SetFilePointer);
    DRM_PROFILING_DETAIL_LEVEL(3);
    if (pFileCtx == NULL || pFileCtx == OEM_INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }


    if (fseek(pFileCtx->pfile,f_lDistanceToMove,(DRM_LONG)f_dwMoveMethod)==0)
    {
        if (f_pdwNewFilePointer!=NULL)
        {
            *f_pdwNewFilePointer = ftell(pFileCtx->pfile);
        }
    }
    else
    {
        ChkDR( DRM_E_FILE_SEEK_ERROR );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? FALSE : TRUE;
}


/* Searches for any existing locks that overlap the supplied region in the lock list
** REQUIRES that the caller already hold the file context critical section
*/
static DRM_BOOL DRM_CALL _FindLockInList(
    __in OEM_LINUX_FILE_CONTEXT *f_pFileCtx,
    __in DRM_DWORD               f_dwFileOffset,
    __in DRM_DWORD               f_dwBytes,
    __inout DRM_BOOL            *f_pfExclusive )
{
    const OEM_LINUX_FILE_LOCK_ENTRY *pLock           = NULL;
    DRM_BOOL                         fResult         = FALSE;
    DRM_BOOL                         fExclusiveFound = FALSE;

    if( f_dwBytes == 0 )
    {
        return FALSE;
    }

    pLock = f_pFileCtx->pFileLocks;
    while( pLock != NULL )
    {
        /* Check if the locked region overlaps the requested region */

        DRM_DWORD dwStartA = pLock->dwFileOffset;
        DRM_DWORD dwEndA = pLock->dwFileOffset + pLock->dwBytesLocked - 1;
        DRM_DWORD dwStartB = f_dwFileOffset;
        DRM_DWORD dwEndB = f_dwFileOffset + f_dwBytes - 1;

        /* If the end of region A is less than the start of region B,
        ** or the end of region B is less than the start of region A,
        ** then there is no overlap. Otherwise, there must be an overlap.
        */
        if(   !(dwEndA < dwStartB
             || dwEndB < dwStartA) )
        {
            /* We found an existing lock that overlaps the requested region */

            /* If we found at least one exclusive lock, then we return exclusive */
            fExclusiveFound |= pLock->fExclusive;
            fResult = TRUE;

            if( fExclusiveFound )
            {
                break;
            }
        }

        pLock = pLock->pNext;
    }

    if( f_pfExclusive != NULL )
    {
        *f_pfExclusive = fExclusiveFound;
    }

    return fResult;
}

/* Deletes the specified lock from the lock list
** REQUIRES that the caller already hold the file context critical section
*/
static DRM_NO_INLINE DRM_BOOL DRM_CALL _DeleteLockInList(
    __in OEM_LINUX_FILE_CONTEXT  *f_pFileCtx,
    __in DRM_DWORD                f_dwFileOffset,
    __in DRM_DWORD                f_dwBytesLocked )
{
    OEM_LINUX_FILE_LOCK_ENTRY  *pLock   = NULL;
    OEM_LINUX_FILE_LOCK_ENTRY **pLast   = NULL;
    DRM_BOOL                    fResult = FALSE;

    if( f_dwBytesLocked == 0 )
    {
        return TRUE;
    }

    pLock = f_pFileCtx->pFileLocks;
    pLast = &f_pFileCtx->pFileLocks;
    while( pLock != NULL )
    {
        if( pLock->dwFileOffset == f_dwFileOffset
         && pLock->dwBytesLocked == f_dwBytesLocked )
        {
            *pLast = pLock->pNext;
            SAFE_OEM_FREE( pLock );
            fResult = TRUE;
            break;
        }

        pLast = &pLock->pNext;
        pLock = pLock->pNext;
    }

    return fResult;
}

/* Adds the specified lock to the lock list, note that this function does not check if there is already
** an incompatible lock in the list, need to call _FindLockInList beforehand.
** REQUIRES that the caller already hold the file context critical section
*/
static DRM_NO_INLINE DRM_BOOL DRM_CALL _AddLockToList(
    __in OEM_LINUX_FILE_CONTEXT *f_pFileCtx,
    __in DRM_DWORD               f_dwFileOffset,
    __in DRM_DWORD               f_dwBytesToLock,
    __in DRM_BOOL                f_fExclusive )
{
    OEM_LINUX_FILE_LOCK_ENTRY *pLock   = NULL;
    DRM_BOOL                   fResult = TRUE;
    DRM_RESULT                 dr      = DRM_SUCCESS;

    if( f_dwBytesToLock == 0 )
    {
        return TRUE;
    }

    ChkMem( pLock = (OEM_LINUX_FILE_LOCK_ENTRY*)Oem_MemAlloc( SIZEOF( OEM_LINUX_FILE_LOCK_ENTRY ) ) );

    ZEROMEM( pLock, SIZEOF( OEM_LINUX_FILE_LOCK_ENTRY ) );

    pLock->dwBytesLocked = f_dwBytesToLock;
    pLock->dwFileOffset = f_dwFileOffset;
    pLock->fExclusive = f_fExclusive;
    pLock->pNext = f_pFileCtx->pFileLocks;

    f_pFileCtx->pFileLocks = pLock;

ErrorExit:
    return dr == DRM_SUCCESS && fResult;
}


static int _SetLock(
    __in int   f_fd,
    __in int   f_cmd,
    __in short f_lockType,
    __in short f_whence,
    __in off_t f_offset,
    __in off_t f_length)
{
    struct flock lock_info;
    lock_info.l_type = f_lockType;
    lock_info.l_whence = f_whence;
    lock_info.l_start = f_offset;
    lock_info.l_len = f_length;

    return fcntl(f_fd, f_cmd, &lock_info);
}
/*
** Note, file locks on linux are by default advisory locks. Which means locks will
** only block file lockers and won't block I/Os. So if the developer want to synchronize
** read/write on a file, make sure to always lock the file first and unlock it after use.
** Since an advisory lock does not block I/Os on a file, one can always write to a locked
** file if he/she forgot to acquire a file lock first.
*/

/*
** To lock the entire file, set f_nNumberOfBytesToLock to MAX_SIGNED_TYPE(DRM_LONG)
*/
DRM_API DRM_BOOL DRM_CALL Oem_File_Lock(
    __in OEM_FILEHDL f_hFile,
    __in DRM_BOOL    f_fExclusive,
    __in DRM_DWORD   f_dwFileOffset,
    __in DRM_DWORD   f_nNumberOfBytesToLock,
    __in DRM_BOOL    f_fWait)
{
    OEM_LINUX_FILE_CONTEXT *pFileCtx = (OEM_LINUX_FILE_CONTEXT*)f_hFile;

    int       cmd            = -1;
    short     l_type         = -1;
    DRM_BOOL  fExclusive     = FALSE;
    DRM_BOOL  fResult        = FALSE;
    int       lockResult     = -1;
    DRM_DWORD tickCountBegin = Oem_Clock_GetTickCount(NULL);

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Lock);
    DRM_PROFILING_DETAIL_LEVEL(3);

    /*
    ** The passed in offset and length are unsigned, but Linux takes signed int for both
    ** parameters, since the maxminum file size we support is 2GB, we need to make sure
    ** there's no integer overflow issue happens here.
    */
    if( ( f_dwFileOffset > MAX_SIGNED_TYPE( DRM_LONG ) )
     || ( f_nNumberOfBytesToLock > MAX_SIGNED_TYPE( DRM_LONG ) ) )
    {
        return FALSE;
    }

    if( pFileCtx == NULL || pFileCtx == OEM_INVALID_HANDLE_VALUE || f_nNumberOfBytesToLock == 0 )
    {
        return FALSE;
    }

    /* we are using F_SETLK only instead of supporting the wait option of the fcntl()
    ** system call. We wait in a loop for the lock. This is to solve the problem of
    ** sharing a single file handle acrosss multiple threads that causes deadlock.
    ** we wait max of 10 seconds and return error if not able to lock when fWait
    ** is TRUE.
    */
    cmd = F_SETLK;

    if( f_fExclusive )
    {
        l_type = F_WRLCK;
    }
    else
    {
        l_type = F_RDLCK;
    }

    while( TRUE )
    {
        /* Enter the file context critical section to allow us to access the file region lock list */
        pthread_mutex_lock( &(pFileCtx->oCS) );

        /* Check for any existing lock that overlaps the requested file region. We must
        ** fail if a caller tries to acquire a shared lock after having already owning an
        ** exclusive lock.
        */
        if( _FindLockInList( pFileCtx, f_dwFileOffset, f_nNumberOfBytesToLock, &fExclusive )
         && ( f_fExclusive || fExclusive ) )
        {
            /* We already have an exlusive lock, or we're looking to get an exlusive lock but there's already
            ** an existing lock of any type, so we fail (or keep trying if fWait).
            */
            fResult = FALSE;
        }
        else
        {
            /* Lock the region of the file to protect against cross-process users of the file */
            lockResult = _SetLock(fileno(pFileCtx->pfile),
                                  cmd,
                                  l_type,
                                  SEEK_SET,
                                  f_dwFileOffset,
                                  f_nNumberOfBytesToLock);

            if( lockResult == 0 )
            {
                /* We got a file lock from Linux, need to keep track of it */
                fResult = _AddLockToList( pFileCtx, f_dwFileOffset, f_nNumberOfBytesToLock, f_fExclusive );
                if( !fResult )
                {
                    /*
                    ** We failed to add the lock to the list - so we will not be able to keep track of it.
                    ** Unlock the file and if there is time remaining, we can try again.
                    */
                    _SetLock(fileno(pFileCtx->pfile),
                                    cmd,
                                    F_UNLCK,
                                    SEEK_SET,
                                    f_dwFileOffset,
                                    f_nNumberOfBytesToLock);
                }
            }
        }

        /* We're done with the lock list, so release the CS */
        pthread_mutex_unlock( &(pFileCtx->oCS) );

        if( fResult
         || !f_fWait
         || ( Oem_Clock_GetTickCount(NULL) - tickCountBegin > DRM_LOCK_WAIT_DURATION ) )
        {
            break;
        }

        sleep( 1 );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return fResult;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_Unlock(
    __in OEM_FILEHDL f_hFile,
    __in DRM_DWORD   f_dwFileOffset,
    __in DRM_DWORD   f_nNumberOfBytesToUnlock)
{

    DRM_BOOL                fResult  = FALSE;
    OEM_LINUX_FILE_CONTEXT *pFileCtx = (OEM_LINUX_FILE_CONTEXT*)f_hFile;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_Unlock);
    DRM_PROFILING_DETAIL_LEVEL(4);

    if( pFileCtx == NULL || pFileCtx == OEM_INVALID_HANDLE_VALUE || f_nNumberOfBytesToUnlock == 0 )
    {
        return FALSE;
    }

    if( ( f_dwFileOffset > MAX_SIGNED_TYPE( DRM_LONG ) )
     || ( f_nNumberOfBytesToUnlock > MAX_SIGNED_TYPE( DRM_LONG ) ) )
    {
        return FALSE;
    }

    pthread_mutex_lock( &pFileCtx->oCS );


    if(_SetLock(fileno(pFileCtx->pfile),
                F_SETLK,
                F_UNLCK,
                SEEK_SET,
                f_dwFileOffset,
                f_nNumberOfBytesToUnlock) == 0)
    {
        /* Delete the lock from our lock list */
        fResult = _DeleteLockInList( pFileCtx, f_dwFileOffset, f_nNumberOfBytesToUnlock );

        /* The lock really should have been in that list */
        DRMASSERT( fResult );
    }

    /* We're done, release the CS */
    pthread_mutex_unlock( &pFileCtx->oCS );


    DRM_PROFILING_LEAVE_SCOPE;
    return fResult;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_SetEndOfFile(
    __in OEM_FILEHDL f_hFile)
{
    /* no CRT support */
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_GetSize(
    __in OEM_FILEHDL  f_hFile,
    __out DRM_DWORD  *f_pcbFile)
{
    DRM_RESULT              dr        = DRM_SUCCESS;
    DRM_DWORD               oFileCurr = 0;
    OEM_LINUX_FILE_CONTEXT *pFileCtx  = (OEM_LINUX_FILE_CONTEXT*)f_hFile;


    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_GetSize);
    DRM_PROFILING_DETAIL_LEVEL(4);

    if( pFileCtx == NULL || pFileCtx == OEM_INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }

    oFileCurr = ftell (pFileCtx->pfile);  /* remember the current location within the file */


    if (fseek (pFileCtx->pfile, 0, SEEK_END) == 0)  /* go to the end of the file */
    {
        *f_pcbFile = ftell (pFileCtx->pfile);
        if (fseek (pFileCtx->pfile, oFileCurr, SEEK_SET) != 0)  /* reset back to the original location */
        {
            ChkDR( DRM_E_FILE_SEEK_ERROR );
        }
    }
    else
    {
        ChkDR( DRM_E_FILE_SEEK_ERROR );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_FAILED( dr ) ? FALSE : TRUE;
}

DRM_API DRM_BOOL DRM_CALL Oem_File_FlushBuffers(
    __in OEM_FILEHDL f_hFile)
{
#if DRM_SUPPORT_TRACING
    DRM_RESULT              dr       = DRM_SUCCESS;
#endif
    DRM_BOOL                fResult  = FALSE;
    OEM_LINUX_FILE_CONTEXT *pFileCtx = (OEM_LINUX_FILE_CONTEXT*)f_hFile;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_OEMFILEIO, PERF_FUNC_Oem_File_FlushBuffers);
    DRM_PROFILING_DETAIL_LEVEL(2);

    if (pFileCtx == NULL)
    {
        return FALSE;
    }
    if( pFileCtx->pfile != OEM_INVALID_HANDLE_VALUE )
    {
        fResult = fflush(pFileCtx->pfile)==0;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return fResult;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the standard OEM interface. */

EXIT_PK_NAMESPACE_CODE;
