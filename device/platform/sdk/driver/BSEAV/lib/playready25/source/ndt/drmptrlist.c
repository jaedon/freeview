/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPTRLIST_C
#include <drmcrt.h>
#include <drmptrlist.h>
#include <drmerr.h>
#include <drmbytemanip.h>
#include <oemcommon.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;


DRM_API DRM_RESULT DRM_CALL DRM_PtrList_Initialize(
    __out       PTR_LIST  * const f_pPtrList,
    __in const  DRM_DWORD         f_dwSize
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pPtrList );

    f_pPtrList->pHead         = NULL;
    f_pPtrList->dwMaxSize     = f_dwSize;
    f_pPtrList->dwCurrentSize = 0;

 ErrorExit:
    return dr;
}


DRM_API DRM_VOID DRM_CALL DRM_PtrList_Uninitialize(
    __out PTR_LIST * const f_pPtrList
)
{
    if ( NULL != f_pPtrList )
    {
        DRM_VOID  *pbData;

        while ( f_pPtrList->dwCurrentSize > 0 )
        {
            DRM_PtrList_RemoveHead(f_pPtrList, &pbData);
        }

        ZEROMEM(f_pPtrList, SIZEOF(PTR_LIST));
    }

    return;
}


DRM_API DRM_RESULT DRM_CALL DRM_PtrList_GetFront(
    __in const PTR_LIST  * const f_pPtrList,
    __out      DRM_VOID        **f_ppbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PTR_LIST_ENTRY *pEntry;

    ChkArg( NULL != f_pPtrList );
    ChkBOOL(f_pPtrList->dwCurrentSize > 0, DRM_E_FAIL);

    pEntry     = f_pPtrList->pHead;
    *f_ppbData = pEntry->pbData;

 ErrorExit:
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_PtrList_RemoveHead(
    __in    PTR_LIST  * const f_pPtrList,
    __out   DRM_VOID        **f_ppbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PTR_LIST_ENTRY *pEntry;

    ChkArg( NULL != f_pPtrList );
    ChkBOOL(f_pPtrList->dwCurrentSize > 0, DRM_E_FAIL);

    pEntry = f_pPtrList->pHead;
    f_pPtrList->pHead = f_pPtrList->pHead->pNext;
    *f_ppbData = pEntry->pbData;
    SAFE_OEM_FREE( pEntry );
    f_pPtrList->dwCurrentSize--;

 ErrorExit:

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_PtrList_AddTail(
    __in  PTR_LIST * const f_pPtrList,
    __in  DRM_VOID * const f_pbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PTR_LIST_ENTRY *pEntry;
    PTR_LIST_ENTRY *pNew = NULL;

    ChkArg( NULL != f_pPtrList );
    ChkBOOL(f_pPtrList->dwCurrentSize < f_pPtrList->dwMaxSize, DRM_E_FAIL);

    pNew = (PTR_LIST_ENTRY *) Oem_MemAlloc(SIZEOF(PTR_LIST_ENTRY));
    ChkMem(pNew);

    pNew->pbData = f_pbData;
    pNew->pNext = NULL;
    f_pPtrList->dwCurrentSize++;

    if ( NULL == f_pPtrList->pHead )
    {
        f_pPtrList->pHead = pNew;
    }
    else
    {
        pEntry = f_pPtrList->pHead;
        while( NULL != pEntry->pNext )
        {
            pEntry = pEntry->pNext;
        }
        pEntry->pNext = pNew;
    }

 ErrorExit:
    if ( DRM_FAILED(dr) )
    {
        SAFE_OEM_FREE( pNew );
    }

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_PtrList_GetTail(
    __in const   PTR_LIST * const f_pPtrList,
    __out        DRM_VOID       **f_ppbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pPtrList );

    if ( NULL == f_pPtrList->pHead )
    {
        ChkDR( DRM_E_FAIL );
    }
    else
    {
        PTR_LIST_ENTRY *pEntry;
        pEntry = f_pPtrList->pHead;
        while( NULL != pEntry->pNext )
        {
            pEntry = pEntry->pNext;
        }
        *f_ppbData = pEntry->pbData;
    }

 ErrorExit:
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_PtrList_GetByIndex(
    __in const PTR_LIST * const   f_pPtrList,
    __in const DRM_DWORD          f_dwIndex,
    __out      DRM_VOID         **f_ppbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pPtrList );

    if ( NULL == f_pPtrList->pHead || f_dwIndex >= f_pPtrList->dwCurrentSize )
    {
        ChkDR( DRM_E_FAIL );
    }
    else
    {
        PTR_LIST_ENTRY *pEntry;
        DRM_DWORD i = 0;

        pEntry = f_pPtrList->pHead;
        while( NULL != pEntry->pNext && i < f_dwIndex )
        {
            i++;
            pEntry = pEntry->pNext;
        }
        DRMASSERT( NULL != pEntry );
        *f_ppbData = pEntry->pbData;
    }

 ErrorExit:
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_PtrList_RemoveByIndex(
    __in       PTR_LIST * const   f_pPtrList,
    __in const DRM_DWORD          f_dwIndex,
    __out      DRM_VOID         **f_ppbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pPtrList );

    if ( NULL == f_pPtrList->pHead || f_dwIndex >= f_pPtrList->dwCurrentSize )
    {
        ChkDR( DRM_E_FAIL );
    }
    else
    {
        PTR_LIST_ENTRY *pEntry;
        PTR_LIST_ENTRY *pPrevious = NULL;
        DRM_DWORD i = 0;

        pEntry = f_pPtrList->pHead;
        while( NULL != pEntry->pNext && i < f_dwIndex )
        {
            i++;
            pPrevious = pEntry;
            pEntry = pEntry->pNext;
        }
        DRMASSERT( NULL != pEntry );
        *f_ppbData = pEntry->pbData;

        if ( NULL == pPrevious )
        {
            f_pPtrList->pHead = f_pPtrList->pHead->pNext;
        }
        else
        {
            pPrevious->pNext = pEntry->pNext;
        }

        SAFE_OEM_FREE( pEntry );
        f_pPtrList->dwCurrentSize--;
    }

 ErrorExit:
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
