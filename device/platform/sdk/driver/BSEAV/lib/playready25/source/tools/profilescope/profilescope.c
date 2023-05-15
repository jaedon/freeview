/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

#include <drmerr.h>
#include <oemcommon.h>
#include <drmcmdlnpars.h>
#include <drmbytemanip.h>
#include <drmtoolsconstants.h>

#if DRM_SUPPORT_PROFILING
#include <drmprofile.h>
#endif


#define MAXBUFFER   2048

DRM_LONG DRM_CALL DRM_Main(
    __in                    DRM_LONG argc,
    __in_ecount_opt( argc ) DRM_WCHAR** argv )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;

    /* L"Scope.dat"; */
    DRM_WCHAR strHeaderFile[] = {ONE_WCHAR('s', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('e', '\0'),
                                 ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')};

    DRM_CHAR szLineBuffer[MAXBUFFER];
    DRM_LONG    i = 0;
    DRM_DWORD cch = 0;

#if DRM_SUPPORT_PROFILING
    PERF_GLOBAL_CONTEXT *pProfileContext = Oem_Profiling_GetProfileContext();
    pProfileContext->m_bInProfile = TRUE;
#endif /* #if DRM_SUPPORT_PROFILING */

    fp = Oem_File_Open (NULL,
                       strHeaderFile,
                       OEM_GENERIC_WRITE,
                       OEM_FILE_SHARE_NONE,
                       OEM_CREATE_ALWAYS,
                       OEM_ATTRIBUTE_NORMAL);

    if (fp == OEM_INVALID_HANDLE_VALUE)
    {
        ChkDR(DRM_E_FILE_WRITE_ERROR);
    }

    for( i = 0; g_Scopes[i].m_pszScopeName != NULL; i++ )
    {
        ChkDR(Oem_File_Write(fp, &g_Scopes[i].m_dwScopeID, SIZEOF(DRM_DWORD), &cch));

        /* Separator of scope records is '|' */
        PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI function.")
        StringCchPrintfA(szLineBuffer, MAXBUFFER, "%s|", g_Scopes[i].m_pszScopeName);
        PREFAST_POP

        ChkDR(Oem_File_Write(fp, szLineBuffer, (DRM_DWORD)DRMCRT_strlen(szLineBuffer), &cch));
    }

    printf("Total %d scopes processed.\n", i);
    dr = DRM_SUCCESS;
ErrorExit:
    if (fp != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close (fp);
    }

#if DRM_SUPPORT_PROFILING
    pProfileContext->m_bInProfile = TRUE;
#endif /* #if DRM_SUPPORT_PROFILING */

    if ( DRM_FAILED( dr ) )
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }
    return dr;
}
