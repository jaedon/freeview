/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMSTKALLOCIMPLREAL_C
#include <drmstkalloc.h>
#include <drmbytemanip.h>
#include <drmerr.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;


/*
** Returns a pointer to where the next call to Alloc will allocate memory from, but doesn't
** actually allocate yet. Useful if the size of the buffer desired is not known in advance.
*/
DRM_API DRM_RESULT DRM_CALL DRM_STK_PreAlloc(
    __in                           DRM_STACK_ALLOCATOR_CONTEXT   *pContext,
    __out                          DRM_DWORD                     *pcbSize,
    __deref_out_bcount( *pcbSize ) DRM_VOID                     **ppbBuffer )
{
    DRM_RESULT dr     = DRM_SUCCESS;

    ChkArg( pContext  != NULL
         && ppbBuffer != NULL
         && pcbSize   != NULL );

    ChkOverflow( pContext->nStackTop + SIZEOF(DRM_DWORD), pContext->nStackTop );
    ChkOverflow( pContext->pbStack + ((pContext->nStackTop + SIZEOF(DRM_DWORD))/CB_NATIVE_BYTE), pContext->pbStack );

    *ppbBuffer = pContext->pbStack + ((pContext->nStackTop + SIZEOF(DRM_DWORD))/CB_NATIVE_BYTE);

    ChkOverflow( pContext->cbStack, pContext->nStackTop + SIZEOF( DRM_DWORD ) );

    *pcbSize = pContext->cbStack - (pContext->nStackTop + SIZEOF(DRM_DWORD));

    pContext->fWasPreAlloc = TRUE;

#if DBG
    /* clear the buffer */
    MEMSET((DRM_BYTE*)*ppbBuffer, 0xa, *pcbSize);
#endif

ErrorExit:
    return dr;
}

/*
** Push the stack to allocate the requested size of buffer
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_INVALID_PARAM_VALUE_1, "*ppbBuffer should not yield this warning given __checkReturn and declaration of DRM_RESULT, but it still does." )
__checkReturn DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_STK_Alloc(
    __in                         DRM_STACK_ALLOCATOR_CONTEXT *pContext,
    __in                         DRM_DWORD                    cbSize,
    __deref_out_bcount( cbSize ) DRM_VOID                   **ppbBuffer )
PREFAST_POP /* __WARNING_INVALID_PARAM_VALUE_1 */
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  dwSize = cbSize;

    ChkArg( pContext  != NULL
         && ppbBuffer != NULL
         && cbSize     > 0 );

    *ppbBuffer = NULL;
    /* allocate buffer from top of stack */
    if ( ( dwSize % SIZEOF(DRM_DWORD_PTR) )!=0 )
    {
        /* adjust cbSize for alignment of DRM_DWORD_PTR */
        dwSize += SIZEOF(DRM_DWORD_PTR) - ( dwSize % SIZEOF(DRM_DWORD_PTR) );
        ChkOverflow( dwSize, cbSize );
    }

    if ( ( pContext->nStackTop                              )  >= pContext->cbStack
      || ( pContext->nStackTop + dwSize                     )  >= pContext->cbStack
      || ( pContext->nStackTop + dwSize                     )   < pContext->nStackTop
      || ( pContext->nStackTop + dwSize + SIZEOF(DRM_DWORD) )   > pContext->cbStack
      || ( pContext->nStackTop + dwSize + SIZEOF(DRM_DWORD) )   < pContext->nStackTop )
    {
        ChkDR( DRM_E_OUTOFMEMORY );
    }

    DRM_BYT_CopyBytes((DRM_BYTE*)pContext->pbStack, pContext->nStackTop, (DRM_BYTE*)&dwSize, 0, SIZEOF(DRM_DWORD));
    *ppbBuffer = pContext->pbStack + ((pContext->nStackTop + SIZEOF(DRM_DWORD))/CB_NATIVE_BYTE);

    pContext->nStackTop += (DRM_DWORD)(dwSize+SIZEOF(DRM_DWORD));
    /*
    TRACE(("Alloc: top=0x%08X  pb=0x%08X  cb=%d\n", pContext->nStackTop, *ppbBuffer, dwSize));
    */
    if( pContext->fWasPreAlloc )
    {
        pContext->fWasPreAlloc = FALSE;
    }
    else
    {
#if DBG
        /* clear the buffer */
        MEMSET((DRM_BYTE*)*ppbBuffer, 0xa, dwSize);
#endif
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_STK_Alloc_Aligned
**
**  Synopsis:  Allocates a buffer from a give stack context and align
**             the returned buffer pointer using the provided alignment
**             value.
**
**  Arguments:
**  [pContext]      -- Stack allocator context to allocate from.
**  [cbSize]        -- Size of the buffer needed.
**  [cbAlign]       -- Value of the alignment.
**  [ppbBufferRaw]  -- Pointer to pointer to hold the new memory offset.
**                     This is the buffer to be freed using DRM_STK_Free().
**                     This parameter is optional if caller does not call
**                     DRM_STK_FREE() with the allocated buffer.
**  [ppbBufferAligned] -- Pointer to pointer to hold the aligned buffer.
**                      It may be the same as the buffer pointed by ppbBufferRaw
**                      if the buffer pointed by ppbBufferRaw is already
**                      aligned.
**
**  Notes:  Pointers must be freed in LIFO order ( just like a stack ).
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STK_Alloc_Aligned(
    __in                         DRM_STACK_ALLOCATOR_CONTEXT *pContext,
    __in                         DRM_DWORD                    cbSize,
    __in                         DRM_DWORD                    cbAlign,
    __out_opt                    DRM_VOID                   **ppbBufferRaw,
    __deref_out_bcount( cbSize ) DRM_VOID                   **ppbBufferAligned )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_DWORD  dwSize  = cbSize;
    DRM_BYTE  *pbAlloc = NULL;
    DRM_DWORD  cbDiff  = 0;

    ChkArg( pContext  != NULL
         && ppbBufferAligned != NULL
         && cbSize     > 0
         && cbAlign > 0 );

    *ppbBufferAligned = NULL;

    if ( ppbBufferRaw != NULL )
    {
        *ppbBufferRaw = NULL;
    }

    /* allocate buffer from top of stack */
    if ( ( dwSize % SIZEOF(DRM_DWORD_PTR) )!=0 )
    {
        /* adjust cbSize for alignment of DRM_DWORD_PTR */
        dwSize += SIZEOF(DRM_DWORD_PTR) - ( dwSize % SIZEOF(DRM_DWORD_PTR) );
        ChkOverflow( dwSize, cbSize );
    }

    pbAlloc = pContext->pbStack + ((pContext->nStackTop + SIZEOF(DRM_DWORD))/CB_NATIVE_BYTE);

    if ( ppbBufferRaw != NULL )
    {
        *ppbBufferRaw = pbAlloc;
    }

    if ( ( ( DRM_DWORD_PTR )pbAlloc ) % cbAlign != 0 )
    {
        cbDiff = cbAlign - ( ( DRM_DWORD_PTR )pbAlloc ) % cbAlign;

        ChkOverflow( dwSize + cbAlign, dwSize );
        dwSize += cbAlign;

        *ppbBufferAligned = pbAlloc + cbDiff;
    }
    else
    {
        *ppbBufferAligned = pbAlloc;
    }

    if ( ( pContext->nStackTop                              )  >= pContext->cbStack
      || ( pContext->nStackTop + dwSize                     )  >= pContext->cbStack
      || ( pContext->nStackTop + dwSize                     )   < pContext->nStackTop
      || ( pContext->nStackTop + dwSize + SIZEOF(DRM_DWORD) )   > pContext->cbStack
      || ( pContext->nStackTop + dwSize + SIZEOF(DRM_DWORD) )   < pContext->nStackTop )
    {
        *ppbBufferAligned = NULL;

        ChkDR( DRM_E_OUTOFMEMORY );
    }

    DRM_BYT_CopyBytes((DRM_BYTE*)pContext->pbStack, pContext->nStackTop, (DRM_BYTE*)&dwSize, 0, SIZEOF(DRM_DWORD));

    pContext->nStackTop += (DRM_DWORD)(dwSize+SIZEOF(DRM_DWORD));
    /*
    TRACE(("Alloc: top=0x%08X  pb=0x%08X  cb=%d\n", pContext->nStackTop, pbAlloc, dwSize));
    */
    if( pContext->fWasPreAlloc )
    {
        pContext->fWasPreAlloc = FALSE;
    }
    else
    {
#if DBG
        /* clear the buffer */
        MEMSET(pbAlloc, 0xa, dwSize);
#endif
    }

ErrorExit:
    return dr;
}


/*
** Pop the stack to free the allocated buffer
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_VOIDPTR_BUFFER, "pbBuffer points into internal the internal buffer and its size is defined by *(pbBuffer-sizeof(DWORD))" )
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_STK_Free(
    __in DRM_STACK_ALLOCATOR_CONTEXT *pContext,
    __in DRM_VOID                    *pbBuffer )
PREFAST_POP
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  dwSize = 0;

    if ( pbBuffer == NULL )
    {
        /* pbBuffer is NULL, simply return */
        ChkDR( DRM_E_LOGICERR );
    }

    ChkArg( pContext != NULL );

    /* verify the buffer with the stack */
    MEMCPY((DRM_BYTE*)&dwSize,(DRM_BYTE*)pbBuffer - sizeof(DRM_DWORD),SIZEOF(DRM_DWORD) );
    /*
    TRACE((" Free: top=0x%08X  pb=0x%08X  cb=%d\n", pContext->nStackTop, pbBuffer, dwSize));
    */
    if ( ((DRM_BYTE*)pbBuffer+(dwSize/CB_NATIVE_BYTE)) != &pContext->pbStack[(pContext->nStackTop/CB_NATIVE_BYTE)])
    {
        TRACE(("\n\n***  DRM_STK_Free(): heap corrupted ***\n\n"));
        DRMASSERT( FALSE );
        ChkDR( DRM_E_STACK_CORRUPT ); /* internal stack corrupted */
    }

#if DBG
    /* clear the buffer */
    MEMSET((DRM_BYTE*)pbBuffer-sizeof(DRM_DWORD), 0xb, dwSize+SIZEOF(DRM_DWORD));
#endif

    pContext->nStackTop -= (DRM_DWORD)(dwSize+SIZEOF(DRM_DWORD));

ErrorExit:
    return dr;
}

/* Init the stack. Return error if already initialized.
*/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_STK_Init(
    __in                 DRM_STACK_ALLOCATOR_CONTEXT *pContext,
    __in_bcount( cbSize) DRM_BYTE                    *pbBuffer,
    __in                 DRM_DWORD                    cbSize )

{   DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( pContext != NULL && pbBuffer != NULL  && cbSize != 0  );
 /*  Check that stack was not initilized. */
    if ( pContext->pbStack != NULL )
    {
        ChkDR( DRM_E_STACK_ALREADY_INITIALIZED );
    }
#if DBG
    /* Set all memory to 0xcd, so we can see un-allocated memory. */
    MEMSET( pbBuffer, 0xCD, cbSize );
#endif

    pContext->pbStack = pbBuffer;
    pContext->cbStack = cbSize;
    pContext->nStackTop = 0;

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
