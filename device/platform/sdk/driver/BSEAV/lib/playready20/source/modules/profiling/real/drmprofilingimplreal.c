/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmprofile.h>

ENTER_PK_NAMESPACE_CODE;


#if DRM_SUPPORT_PROFILING

/* L"perf.dat" */
static const DRM_WCHAR g_rgwchPerfLog[] = {ONE_WCHAR('p', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('f', '\0'),
                              ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
                              ONE_WCHAR('\0', '\0')};

/* L"perfagg.dat" */
static const DRM_WCHAR g_rgwchPerfAggLog[] = {ONE_WCHAR('p', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('f', '\0'),
                                 ONE_WCHAR('a', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('.', '\0'),
                                 ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')};

/* L"perfuserdata.dat" */
static const DRM_WCHAR g_rgwchPerfUserDataLog[] = {ONE_WCHAR('p', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('f', '\0'),
                                      ONE_WCHAR('u', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'),
                                      ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('a', '\0'),
                                      ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'),
                                      ONE_WCHAR('\0', '\0')};

/*L"perfstacksize.dat"*/
static const DRM_WCHAR g_rgwchPerfStackSizeLog[] = {ONE_WCHAR('p', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('f', '\0'),
                                      ONE_WCHAR('s', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'),
                                      ONE_WCHAR('k', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('z', '\0'),
                                      ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'),
                                      ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')};

/*L"StackSize*/
static const DRM_WCHAR g_rgwchStackSizePrefix[] = {ONE_WCHAR('s', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'),
                                      ONE_WCHAR('k', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('z', '\0'),
                                      ONE_WCHAR('e', '\0'),ONE_WCHAR('\0', '\0')};

/* Hash a string into a DRM_DWORD to facilitate user data logging since it only takes DRM_DWORD */
DRM_DWORD Perf_String_Data(DRM_CONST_STRING * pdstrIn)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwResult = 0;
    DRM_LONG nSize = 0, i = 0;
    const DRM_WCHAR * pStr = NULL;

    ChkArg(pdstrIn != NULL);

    nSize = pdstrIn->cchString;
    pStr = pdstrIn->pwszString;

    for (i = 0; i < nSize; i++)
        dwResult += *(pStr++);

ErrorExit:
    return dwResult;
}

/* Hash a binary buffer into a DRM_DWORD to facilitate user data logging since it only takes DRM_DWORD */
DRM_DWORD Perf_Binary_Data(DRM_BYTE * pbIn, DRM_DWORD dwSize)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwResult = 0;
    DRM_DWORD i = 0;

    ChkArg(pbIn != NULL);

    for (i = 0; i < dwSize; i++)
        dwResult += *(pbIn+i);

ErrorExit:
    return dwResult;
}

PERF_AGG_SCOPE_CONTEXT * Perf_Allocate_Agg_Scope()
{
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();

    if (pPerfContext->m_nCurrAggScope >= MAX_PROFILE_AGG_SCOPES)
        return NULL;

    MEMSET(&pPerfContext->m_AggScopes[pPerfContext->m_nCurrAggScope], 0, SIZEOF(PERF_AGG_SCOPE_CONTEXT));

    return &pPerfContext->m_AggScopes[pPerfContext->m_nCurrAggScope++];
}

DRM_API DRM_RESULT DRM_CALL _WriteProfileSample(DRM_BYTE * pbData, DRM_DWORD dwSize)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cch = 0;
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();

    /* Check whether the cached sample buffer is full */
    if (pPerfContext->m_dwBufferIndex + dwSize > MAX_PROFILE_BUFFER_SIZE)
    {
        /* Flush out all content in the cached sample buffer */
        ChkDR(Oem_File_Write(pPerfContext->m_fp, pPerfContext->m_bBuffer, pPerfContext->m_dwBufferIndex, &cch));

        pPerfContext->m_dwBufferIndex = 0;
    }

    /* Store the sample in the cached sample buffer */
    MEMCPY(&(pPerfContext->m_bBuffer[pPerfContext->m_dwBufferIndex]), pbData, dwSize);

    pPerfContext->m_dwBufferIndex += dwSize;

ErrorExit:
    return dr;
}

#if PROFILE_USER_DATA
DRM_API DRM_RESULT DRM_CALL _WriteProfileUserData(DRM_BYTE * pbData, DRM_DWORD dwSize)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cch = 0;
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();

    /* Check whether the cached user data buffer is full */
    if (pPerfContext->m_dwBufferUserDataIndex + dwSize > MAX_PROFILE_BUFFER_SIZE)
    {
        /* Flush out all content in the cached user data buffer */
        ChkDR(Oem_File_Write(pPerfContext->m_fpUserData, pPerfContext->m_bBufferUserData, pPerfContext->m_dwBufferUserDataIndex, &cch));

        pPerfContext->m_dwBufferUserDataIndex = 0;
    }

    /* Store the user data in the cached user data buffer */
    MEMCPY(&(pPerfContext->m_bBufferUserData[pPerfContext->m_dwBufferUserDataIndex]), pbData, dwSize);

    pPerfContext->m_dwBufferUserDataIndex += dwSize;

ErrorExit:
    return dr;
}
#endif

/**********************************************************************
Function: Perf_Log_Profile
Synopsis: This functions logs the profiling data to perf*.dat file when
          profiling is turned on
Arguments: pScopeContext : the scope context used to store profiling data
           nScopeLevel   : Unsigned Int used to track the scope level of
                           the current function being logged
***********************************************************************/
DRM_RESULT Perf_Log_Profile(PERF_SCOPE_CONTEXT * pScopeContext, DRM_DWORD nScopeLevel)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();
    DRM_DWORD dwTickCount = 0;

    ChkArg(pScopeContext != NULL);

    if (pPerfContext->m_nDetailLevel != 0 && pPerfContext->m_nDetailLevel < pScopeContext->m_nDetailLevel)
        goto ErrorExit;

    if (pPerfContext->m_nFeatureSet != 0 && ((pPerfContext->m_nFeatureSet & pScopeContext->m_nFeatureSet) == 0))
        goto ErrorExit;

    if( pPerfContext->m_fp == OEM_INVALID_HANDLE_VALUE )
    {
        /* Not using ChkArg here since it would flood the trace log.
        ** An invalid handle value is a sign that we've stopped
        ** profiling but all functions will still call into profiling.
        */
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    dwTickCount = PERF_CURRENT_MICROSECOND_COUNT;

    ChkDR(_WriteProfileSample((DRM_BYTE *) &pScopeContext->m_dwScopeID, SIZEOF(DRM_DWORD)));

    ChkDR(_WriteProfileSample((DRM_BYTE *) &pScopeContext->m_dwParentScopeID, SIZEOF(DRM_DWORD)));

    ChkDR(_WriteProfileSample((DRM_BYTE *) &pScopeContext->m_dwDuration, SIZEOF(DRM_DWORD)));

    ChkDR(_WriteProfileSample((DRM_BYTE *)&nScopeLevel,SIZEOF(DRM_DWORD)));

#if PROFILE_USER_DATA
    /* Only log if all user data in the scope context is not 0xffffffff */
    if (pScopeContext->m_dwUserData1 != 0xffffffff && pScopeContext->m_dwUserData2 != 0xffffffff)
    {
        ChkDR(_WriteProfileUserData((DRM_BYTE *) &pScopeContext->m_dwScopeID, SIZEOF(DRM_DWORD)));

        ChkDR(_WriteProfileUserData((DRM_BYTE *) &pScopeContext->m_dwUserData1, SIZEOF(DRM_DWORD)));

        ChkDR(_WriteProfileUserData((DRM_BYTE *) &pScopeContext->m_dwUserData2, SIZEOF(DRM_DWORD)));
    }
#endif

    pPerfContext->m_dwLogWriteDuration = pPerfContext->m_dwLogWriteDuration + PERF_CURRENT_MICROSECOND_COUNT - dwTickCount;

ErrorExit:
    return dr;
}

DRM_RESULT Perf_Enter_Scope(PERF_SCOPE_CONTEXT * pScopeContext)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();
    DRM_BOOL fResetInProfile = FALSE;

    ChkArg(pScopeContext != NULL);

    if( !pPerfContext->m_bRunning
     ||  pPerfContext->m_bInProfile)
    {
        /* Profiling is either turned off or we're already inside it */
        goto ErrorExit;
    }
    pPerfContext->m_bInProfile = TRUE;
    fResetInProfile = TRUE;

    if (pPerfContext->m_nCurrScope >= MAX_PROFILE_SCOPE_LEVEL)
    {
        /*    printf("Scope stack is full!\n"); */
        goto ErrorExit;
    }

#if PROFILE_USER_DATA

    /* Initialize user data in the current scope context */
    pScopeContext->m_dwUserData1 = 0xffffffff;

    pScopeContext->m_dwUserData2 = 0xffffffff;

#endif

    pScopeContext->m_dwDuration = PERF_CURRENT_MICROSECOND_COUNT - pPerfContext->m_dwLogWriteDuration;

    if (pPerfContext->m_nCurrScope == 0)
    {
        pScopeContext->m_dwParentScopeID = 0;
    }
    else
    {
        pScopeContext->m_dwParentScopeID = pPerfContext->m_dwScopeID[pPerfContext->m_nCurrScope - 1];
    }

    pPerfContext->m_dwScopeID[pPerfContext->m_nCurrScope++] = pScopeContext->m_dwScopeID;

#if PROFILE_STACK_SIZE

    /*initialize the StackBase if it has not already been initialized */
    if(pPerfContext->m_dwStackBase == 0)
    {
        pPerfContext->m_dwStackBase = (DRM_DWORD_PTR)pScopeContext;
        pPerfContext->m_dwMaxStackSize = 0;
    }

#endif

ErrorExit:
    if( fResetInProfile )
    {
        pPerfContext->m_bInProfile = FALSE;
    }

    return dr;
}

DRM_RESULT Perf_Leave_Scope(PERF_SCOPE_CONTEXT * pScopeContext)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwNow = 0;
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();
    DRM_BOOL fResetInProfile = FALSE;

    ChkArg(pScopeContext != NULL);

    if( !pPerfContext->m_bRunning
     ||  pPerfContext->m_bInProfile)
    {
        /* Profiling is either turned off or we're already inside it */
        goto ErrorExit;
    }
    pPerfContext->m_bInProfile = TRUE;
    fResetInProfile = TRUE;

    if (pPerfContext->m_nCurrScope == 0)
    {
        /*  printf("Scope stack is empty!\n"); */
        DRMASSERT(FALSE);
        dr = DRM_E_PERF_SCOPING_ERROR;
        goto ErrorExit;
    }

    if (pPerfContext->m_dwScopeID[pPerfContext->m_nCurrScope - 1] != pScopeContext->m_dwScopeID)
    {
        /*  printf("Scope ID mismatch!\n"); */
        DRMASSERT(FALSE);
        dr = DRM_E_PERF_SCOPING_ERROR;
        goto ErrorExit;
    }

    dwNow = PERF_CURRENT_MICROSECOND_COUNT;

    pScopeContext->m_dwDuration = dwNow - pScopeContext->m_dwDuration - pPerfContext->m_dwLogWriteDuration;

    ChkDR( Perf_Log_Profile(pScopeContext,pPerfContext->m_nCurrScope) );

#if PROFILE_STACK_SIZE
{
    /*Measure the stack usage till this point, and store it if it is max. so far */
    if( (pPerfContext->m_dwStackBase - (DRM_DWORD_PTR)&dr) > pPerfContext->m_dwMaxStackSize)
    {
        pPerfContext->m_dwMaxStackSize = (pPerfContext->m_dwStackBase - (DRM_DWORD_PTR)&dr);

    }

    if(pPerfContext->m_nCurrScope == 2)
    {
        if(pPerfContext->m_fpStackSize != OEM_INVALID_HANDLE_VALUE)
        {
            ChkBOOL(Oem_File_Write(pPerfContext->m_fpStackSize, &pScopeContext->m_dwScopeID, SIZEOF(DRM_DWORD), &dwNow), DRM_E_FILEWRITEERROR );
            ChkBOOL(Oem_File_Write(pPerfContext->m_fpStackSize, &pPerfContext->m_dwMaxStackSize, SIZEOF(DRM_DWORD), &dwNow), DRM_E_FILEWRITEERROR );
        }

        /*If the stack usage of this API is greater than that of the max usage recorded so far, save this value */
        if(pPerfContext->m_dwMaxStackSize > pPerfContext->m_dwMaxStackSizeinScenario)
        {
            pPerfContext->m_dwMaxStackSizeinScenario = pPerfContext->m_dwMaxStackSize;
            pPerfContext->m_dwMaxStackSizeScopeID = pScopeContext->m_dwScopeID;
        }

        /*We are at the top level API - so reset the stack base and max stack size*/
        pPerfContext->m_dwMaxStackSize = 0;
        pPerfContext->m_dwStackBase = 0;
    }
}

#endif

    pPerfContext->m_nCurrScope--;

ErrorExit:
    if( fResetInProfile )
    {
        pPerfContext->m_bInProfile = FALSE;
    }
    return dr;
}

DRM_RESULT Perf_Enter_Agg_Scope(PERF_AGG_SCOPE_CONTEXT ** ppAggScopeContext, DRM_DWORD dwScopeID)
{
    DRM_RESULT dr = DRM_SUCCESS;

    if (*ppAggScopeContext == NULL)
    {
        *ppAggScopeContext = Perf_Allocate_Agg_Scope();

        ChkArg(*ppAggScopeContext != NULL);

        (*ppAggScopeContext)->m_dwScopeID = dwScopeID;
    }

    /* Increment the call count of the scope */
    (*ppAggScopeContext)->m_dwCallCount++;

ErrorExit:
    return dr;
}

DRM_RESULT Perf_Leave_Agg_Scope(PERF_AGG_SCOPE_CONTEXT * pAggScopeContext, DRM_DWORD dwDuration)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();

    ChkArg(pAggScopeContext != NULL);

    ChkArg(pAggScopeContext->m_dwScopeID != 0);

    if (pPerfContext->m_nDetailLevel != 0 && pPerfContext->m_nDetailLevel < pAggScopeContext->m_nDetailLevel)
        goto ErrorExit;

    if (pPerfContext->m_nFeatureSet != 0 && ((pPerfContext->m_nFeatureSet & pAggScopeContext->m_nFeatureSet) == 0))
        goto ErrorExit;

    /* Accumulate the dutation of the scope */
    pAggScopeContext->m_dwDuration += dwDuration;

ErrorExit:
    return dr;
}

DRM_RESULT Perf_Start_Profiling( DRM_VOID *pOEMContext, DRM_CONST_STRING *dstrPerfFileName )
{
    DRM_RESULT dr = DRM_SUCCESS;
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;
    DRM_STRING dstrPerfStackSizeLog = EMPTY_DRM_STRING;
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();

    if (pPerfContext->m_bRunning)
        goto ErrorExit;

    pPerfContext->m_bInProfile = TRUE;

#if PROFILE_USE_SCOPE
    ChkArg(pPerfContext->m_fp == OEM_INVALID_HANDLE_VALUE);

    fp = Oem_File_Open(pOEMContext,
                      (NULL == dstrPerfFileName? g_rgwchPerfLog:dstrPerfFileName->pwszString),
                      OEM_GENERIC_WRITE,
                      OEM_FILE_SHARE_NONE,
                      (NULL == dstrPerfFileName? OEM_OPEN_ALWAYS:OEM_CREATE_ALWAYS),
                      OEM_ATTRIBUTE_NORMAL);

    if (fp == OEM_INVALID_HANDLE_VALUE)
    {
        if(dstrPerfFileName == NULL)
        {
            fp = Oem_File_Open(pOEMContext,
                          (NULL == dstrPerfFileName? g_rgwchPerfLog:dstrPerfFileName->pwszString),
                          OEM_GENERIC_WRITE,
                          OEM_FILE_SHARE_NONE,
                          OEM_CREATE_NEW,
                          OEM_ATTRIBUTE_NORMAL);
        }
        ChkBOOL(fp != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN);
    }
    else
    {
        if (!Oem_File_SetFilePointer(fp, 0, OEM_FILE_END, NULL))
        {
            ChkDR(DRM_E_FILESEEKERROR);
        }
    }

    pPerfContext->m_fp = fp;

#endif

#if PROFILE_USE_AGG_SCOPE
    fp = OEM_INVALID_HANDLE_VALUE;

    fp = Oem_File_Open(pOEMContext,
                      (NULL == dstrPerfFileName? g_rgwchPerfAggLog:dstrPerfFileName->pwszString),
                      OEM_GENERIC_WRITE,
                      OEM_FILE_SHARE_NONE,
                      OEM_OPEN_ALWAYS,
                      OEM_ATTRIBUTE_NORMAL);

    if (fp == OEM_INVALID_HANDLE_VALUE)
    {
        fp = Oem_File_Open(pOEMContext,
                          (NULL == dstrPerfFileName? g_rgwchPerfAggLog:dstrPerfFileName->pwszString),
                          OEM_GENERIC_WRITE,
                          OEM_FILE_SHARE_NONE,
                          OEM_CREATE_NEW,
                          OEM_ATTRIBUTE_NORMAL);

        ChkBOOL(fp != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN);
    }
    else
    {
        if (!Oem_File_SetFilePointer(fp, 0, OEM_FILE_END, NULL))
        {
            ChkDR(DRM_E_FILESEEKERROR);
        }
    }

    pPerfContext->m_fpAgg = fp;

#endif

#if PROFILE_USER_DATA
    fp = OEM_INVALID_HANDLE_VALUE;

    fp = Oem_File_Open(pOEMContext,
                      (NULL == dstrPerfFileName? g_rgwchPerfUserDataLog:dstrPerfFileName->pwszString),
                      OEM_GENERIC_WRITE,
                      OEM_FILE_SHARE_NONE,
                      OEM_OPEN_ALWAYS,
                      OEM_ATTRIBUTE_NORMAL);

    if (fp == OEM_INVALID_HANDLE_VALUE)
    {
        fp = Oem_File_Open(pOEMContext,
                          (NULL == dstrPerfFileName? g_rgwchPerfUserDataLog:dstrPerfFileName->pwszString),
                          OEM_GENERIC_WRITE,
                          OEM_FILE_SHARE_NONE,
                          OEM_CREATE_NEW,
                          OEM_ATTRIBUTE_NORMAL);

        ChkBOOL(fp != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN);
    }
    else
    {
        if (!Oem_File_SetFilePointer(fp, 0, OEM_FILE_END, NULL))
        {
            ChkDR(DRM_E_FILESEEKERROR);
        }
    }

    pPerfContext->m_fpUserData = fp;

#endif

#if PROFILE_STACK_SIZE
    fp = OEM_INVALID_HANDLE_VALUE;

    if(dstrPerfFileName != NULL)
    {
        DRM_DWORD dwStackPrefixLength = 0;

        ChkDR(DRM_STR_StringCchLengthW(g_rgwchStackSizePrefix, DRM_STRSAFE_MAX_CCH, &dwStackPrefixLength));
        dstrPerfStackSizeLog.cchString = dstrPerfFileName->cchString + dwStackPrefixLength + 1;

        ChkMem(dstrPerfStackSizeLog.pwszString = (DRM_WCHAR*)Oem_MemAlloc((dstrPerfStackSizeLog.cchString)* SIZEOF(DRM_WCHAR)));
        ChkDR(DRM_STR_StringCchCopyNW(dstrPerfStackSizeLog.pwszString, dstrPerfStackSizeLog.cchString , g_rgwchStackSizePrefix, dwStackPrefixLength));
        ChkDR(DRM_STR_StringCchCatNW(dstrPerfStackSizeLog.pwszString,dstrPerfStackSizeLog.cchString, dstrPerfFileName->pwszString, dstrPerfFileName->cchString));
    }


    fp = Oem_File_Open(pOEMContext,
                      (NULL == dstrPerfFileName? g_rgwchPerfStackSizeLog: dstrPerfStackSizeLog.pwszString),
                      OEM_GENERIC_WRITE,
                      OEM_FILE_SHARE_NONE,
                      OEM_CREATE_ALWAYS,
                      OEM_ATTRIBUTE_NORMAL);

    if (fp == OEM_INVALID_HANDLE_VALUE)
    {
        fp = Oem_File_Open(pOEMContext,
                      (NULL == dstrPerfFileName? g_rgwchPerfStackSizeLog: dstrPerfStackSizeLog.pwszString),
                      OEM_GENERIC_WRITE,
                      OEM_FILE_SHARE_NONE,
                      OEM_CREATE_NEW,
                      OEM_ATTRIBUTE_NORMAL);

        ChkBOOL(fp != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN);
    }
    else
    {
        if (!Oem_File_SetFilePointer(fp, 0, OEM_FILE_END, NULL))
        {
            ChkDR(DRM_E_FILESEEKERROR);
        }
    }

    pPerfContext->m_fpStackSize= fp;
#endif

    pPerfContext->m_bRunning = TRUE;
    pPerfContext->m_dwLogWriteDuration = 0;

ErrorExit:
    pPerfContext->m_bInProfile = FALSE;
    SAFE_OEM_FREE(dstrPerfStackSizeLog.pwszString);
    return dr;
}

DRM_RESULT Perf_Stop_Profiling()
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cch = 0;
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();

#if PROFILE_USE_AGG_SCOPE
    DRM_DWORD i = 0;
#endif

    if (!pPerfContext->m_bRunning)
        goto ErrorExit;

    pPerfContext->m_bInProfile = TRUE;
    pPerfContext->m_bRunning = FALSE;

#if PROFILE_USE_SCOPE
    ChkArg(pPerfContext->m_fp != OEM_INVALID_HANDLE_VALUE);

    /* Check whether there is any sample in the cached sample buffer */
    if (pPerfContext->m_dwBufferIndex > 0)
    {
        /* Flush out all the cached samples */
        ChkDR(Oem_File_Write(pPerfContext->m_fp, pPerfContext->m_bBuffer, pPerfContext->m_dwBufferIndex, &cch));
    }

    ChkDR(Oem_File_Close(pPerfContext->m_fp));

    pPerfContext->m_fp = OEM_INVALID_HANDLE_VALUE;
    ZEROMEM(pPerfContext->m_bBuffer,MAX_PROFILE_BUFFER_SIZE);
    pPerfContext->m_dwBufferIndex = 0;
#endif

#if PROFILE_USE_AGG_SCOPE
    /* Write data of all aggregated scopes*/
    for (i = 0; i < pPerfContext->m_nCurrAggScope; i++)
    {
        ChkDR(Oem_File_Write(pPerfContext->m_fpAgg, &pPerfContext->m_AggScopes[i].m_dwScopeID, SIZEOF(DRM_DWORD), &cch));
        ChkDR(Oem_File_Write(pPerfContext->m_fpAgg, &pPerfContext->m_AggScopes[i].m_dwDuration, SIZEOF(DRM_DWORD), &cch));
        ChkDR(Oem_File_Write(pPerfContext->m_fpAgg, &pPerfContext->m_AggScopes[i].m_dwCallCount, SIZEOF(DRM_DWORD), &cch));
    }

    ChkDR(Oem_File_Close(pPerfContext->m_fpAgg));

    pPerfContext->m_fpAgg = OEM_INVALID_HANDLE_VALUE;
#endif

#if PROFILE_USER_DATA
    ChkArg(pPerfContext->m_fpUserData != OEM_INVALID_HANDLE_VALUE);

    /* Check whether there is any user data in the cached user data buffer */
    if (pPerfContext->m_dwBufferUserDataIndex > 0)
    {
        /* Flush out all the cached samples */
        ChkDR(Oem_File_Write(pPerfContext->m_fpUserData, pPerfContext->m_bBufferUserData, pPerfContext->m_dwBufferUserDataIndex, &cch));
    }

    ChkDR(Oem_File_Close(pPerfContext->m_fpUserData));

    pPerfContext->m_fpUserData = OEM_INVALID_HANDLE_VALUE;
    ZEROMEM(pPerfContext->m_bBufferUserData,MAX_PROFILE_BUFFER_SIZE);
    pPerfContext->m_dwBufferUserDataIndex = 0;
#endif

#if PROFILE_STACK_SIZE
    ChkArg(pPerfContext->m_fpStackSize!= OEM_INVALID_HANDLE_VALUE);

     if((pPerfContext->m_dwMaxStackSizeinScenario != 0) && (pPerfContext->m_dwMaxStackSizeScopeID != 0 ))
    {
        ChkDR(Oem_File_Write(pPerfContext->m_fpStackSize,&(pPerfContext->m_dwMaxStackSizeScopeID),SIZEOF(DRM_DWORD),&cch));
        ChkDR(Oem_File_Write(pPerfContext->m_fpStackSize,&(pPerfContext->m_dwMaxStackSizeinScenario),SIZEOF(DRM_DWORD),&cch));
    }
    ChkDR(Oem_File_Close(pPerfContext->m_fpStackSize));
    pPerfContext->m_fpStackSize = OEM_INVALID_HANDLE_VALUE;
#endif

ErrorExit:
    pPerfContext->m_bInProfile = FALSE;
    return dr;
}

DRM_RESULT Perf_Init_ProfileStack()
{
    DRM_RESULT dr = DRM_SUCCESS;

#if PROFILE_STACK_SIZE
    PERF_GLOBAL_CONTEXT *pPerfContext = Oem_Profiling_GetProfileContext();

     pPerfContext->m_dwStackBase = (DRM_DWORD_PTR)&dr;
     pPerfContext->m_dwMaxStackSize = 0;
 #endif

     return dr;
}

#endif /* #if DRM_SUPPORT_PROFILING */

EXIT_PK_NAMESPACE_CODE;
