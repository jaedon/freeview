/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmcrt.h>
#include <drmorderedlist.h>

ENTER_PK_NAMESPACE_CODE;

DRM_RESULT DRM_CALL DRM_OrderedList_Initialize(
    __out      ORDERED_LIST            *pOrderedList,
    __in const DRM_DWORD                dwSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( pOrderedList );

    pOrderedList->dwCurrentSize = 0;
    pOrderedList->dwMaxSize     = dwSize;
    pOrderedList->pHead         = NULL;

 ErrorExit:
    return dr;
}



DRM_VOID DRM_CALL DRM_OrderedList_Uninitialize(
    __out ORDERED_LIST * const pOrderedList
)
{
    if ( NULL != pOrderedList )
    {
        ORDERED_LIST_ENTRY *pEntry;
        for( pEntry = pOrderedList->pHead; NULL != pEntry; )
        {
            ORDERED_LIST_ENTRY *pFreeMe = pEntry;
            pEntry = pEntry->pNext;
            SAFE_OEM_FREE(pFreeMe);
        }

        ZEROMEM(pOrderedList, SIZEOF(ORDERED_LIST));
    }

    return;
}



DRM_RESULT DRM_CALL DRM_OrderedList_GetCount(
    __in const ORDERED_LIST * const pOrderedList,
    __out      DRM_DWORD    * const pdwCount
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    ChkArg( NULL != pOrderedList );
    ChkArg( NULL != pdwCount );

    *pdwCount = pOrderedList->dwCurrentSize;

 ErrorExit:
    return dr;
}



DRM_RESULT DRM_CALL DRM_OrderedList_AddEntry(
    __inout    ORDERED_LIST * const pOrderedList,
    __in const DRM_UINT64           qwValue,
    __in       DRM_VOID     * const pbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ORDERED_LIST_ENTRY *pNew = NULL;


    ChkArg( NULL != pOrderedList );

    ChkBOOL( pOrderedList->dwCurrentSize < pOrderedList->dwMaxSize, DRM_E_FAIL);

    ChkMem( pNew = (ORDERED_LIST_ENTRY *) Oem_MemAlloc(SIZEOF(ORDERED_LIST_ENTRY)) );

    pNew->qwValue = qwValue;
    pNew->pbData  = pbData;
    pNew->pNext   = NULL;

    if ( NULL == pOrderedList->pHead )
    {
        pOrderedList->pHead = pNew;
    }
    else
    {
        ORDERED_LIST_ENTRY *pEntry;
        ORDERED_LIST_ENTRY *pPrevious = NULL;

        for( pEntry = pOrderedList->pHead; NULL != pEntry; pEntry = pEntry->pNext )
        {
            if ( DRM_UI64Les(qwValue, pEntry->qwValue) )
            {
                break;
            }
            pPrevious = pEntry;
        }

        if ( NULL == pPrevious )
        {
            pNew->pNext = pEntry;
            pOrderedList->pHead = pNew;
        }
        else
        {
            pNew->pNext = pEntry;
            pPrevious->pNext = pNew;
        }
    }

    pOrderedList->dwCurrentSize++;

 ErrorExit:
    if ( DRM_FAILED(dr) )
    {
        SAFE_OEM_FREE(pNew);
    }

    return dr;
}



DRM_RESULT DRM_CALL DRM_OrderedList_RemoveEntry(
    __inout    ORDERED_LIST  * const pOrderedList,
    __in const DRM_DWORD             dwFirstEntryToConsider,
    __out      DRM_UINT64    * const pqwValue,
    __out      DRM_VOID            **ppbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i = 0;
    ORDERED_LIST_ENTRY *pEntry;
    ORDERED_LIST_ENTRY *pPrevious = NULL;


    ChkArg( NULL != pOrderedList );
    ChkArg( NULL != ppbData );
    ChkArg( NULL != pqwValue );
    ChkBOOL(dwFirstEntryToConsider < pOrderedList->dwCurrentSize, DRM_E_FAIL);

    pEntry = pOrderedList->pHead;
    for ( i = 0; i < dwFirstEntryToConsider && NULL != pEntry; i++ )
    {
        pPrevious = pEntry;
        pEntry = pEntry->pNext;
    }

    if ( NULL == pEntry )
    {
        ChkDR( DRM_E_FAIL );
    }
    else
    {
        *ppbData = pEntry->pbData;
        *pqwValue = pEntry->qwValue;
        pOrderedList->dwCurrentSize--;

        if ( NULL == pPrevious )
        {
            ORDERED_LIST_ENTRY *pFreeMe = pOrderedList->pHead;
            pOrderedList->pHead = pOrderedList->pHead->pNext;
            SAFE_OEM_FREE( pFreeMe );
        }
        else
        {
            ORDERED_LIST_ENTRY *pFreeMe = pEntry;
            pPrevious->pNext = pEntry->pNext;
            SAFE_OEM_FREE( pFreeMe );
        }
    }

 ErrorExit:
    return dr;
}



DRM_RESULT DRM_CALL DRM_OrderedList_AddLeadingEntry(
    __inout    ORDERED_LIST  * const pOrderedList,
    __in const DRM_UINT64            qwValue,
    __in       DRM_VOID      * const pbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ORDERED_LIST_ENTRY *pNew = NULL;


    ChkArg( NULL != pOrderedList);
    ChkBOOL( pOrderedList->dwCurrentSize < pOrderedList->dwMaxSize, DRM_E_FAIL);

    pNew = (ORDERED_LIST_ENTRY *) Oem_MemAlloc(SIZEOF(ORDERED_LIST_ENTRY));
    ChkMem(pNew);
    pNew->qwValue = qwValue;
    pNew->pbData  = pbData;
    pNew->pNext   = NULL;

    if ( NULL == pOrderedList->pHead )
    {
        pOrderedList->pHead = pNew;
    }
    else
    {
        ORDERED_LIST_ENTRY *pEntry;
        ORDERED_LIST_ENTRY *pPrevious = NULL;

        for( pEntry = pOrderedList->pHead; NULL != pEntry; pEntry = pEntry->pNext )
        {
            if ( DRM_UI64Les(qwValue, pEntry->qwValue)
                 || DRM_UI64Eql(qwValue, pEntry->qwValue) )
            {
                break;
            }
            pPrevious = pEntry;
        }

        if ( NULL == pPrevious )
        {
            pNew->pNext = pEntry;
            pOrderedList->pHead = pNew;
        }
        else
        {
            pNew->pNext = pEntry;
            pPrevious->pNext = pNew;
        }
    }

    pOrderedList->dwCurrentSize++;

 ErrorExit:
    if ( DRM_FAILED(dr) )
    {
        SAFE_OEM_FREE(pNew);
    }

    return dr;
}



DRM_RESULT DRM_CALL DRM_OrderedList_GetEntry(
    __in const ORDERED_LIST  * const pOrderedList,
    __in const DRM_DWORD             dwFirstEntryToConsider,
    __out      DRM_UINT64    * const pqwValue,
    __out      DRM_VOID            **ppbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i;
    ORDERED_LIST_ENTRY *pEntry;


    ChkArg( NULL != pOrderedList );
    ChkArg( NULL != ppbData );
    ChkArg( NULL != pqwValue );
    ChkBOOL(dwFirstEntryToConsider < pOrderedList->dwCurrentSize, DRM_E_FAIL);

    pEntry = pOrderedList->pHead;
    for ( i = 0; i < dwFirstEntryToConsider && NULL != pEntry; i++ )
    {
        pEntry = pEntry->pNext;
    }

    if ( NULL != pEntry )
    {
        *pqwValue = pEntry->qwValue;
        *ppbData = pEntry->pbData;
    }
    else
    {
        ChkDR( DRM_E_FAIL );
    }

 ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
