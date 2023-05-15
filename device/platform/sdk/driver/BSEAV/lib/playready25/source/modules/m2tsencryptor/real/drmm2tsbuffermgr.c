/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMM2TSBUFFERMGR_C
#include <oemcommon.h>
#include <drmm2tsbuffermgr.h>
#include <drmm2tslogs.h>
#include <drmm2tsmacros.h>
#include <drmmathsafe.h>
#include <drmmanager.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    DRM_M2TS_MemAllocEncryptorContext
**
** Synopsis:    Allocates heap using Oem_MemAlloc for DRM_M2TS_ENCRYPTOR_CONTEXT
**              and initializes the context.  The max heap allowed must
**              be greater than the size of the context.  If f_cbMaxHeapAllowed
**              is zero then there is no limitation.
**
** Arguments:
**
** [f_cbMaxHeapAllowed]     -- Specify the max number of bytes that the encryptor
**                             can allocate.
** [f_ppEncContext]         -- Out parameter to contain the enryptor context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_MemAllocEncryptorContext(
    __in          const DRM_DWORD                        f_cbMaxHeapAllowed,
    __deref_out         DRM_M2TS_ENCRYPTOR_CONTEXT     **f_ppEncContext )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkArg( f_cbMaxHeapAllowed == 0 || f_cbMaxHeapAllowed > SIZEOF( DRM_M2TS_ENCRYPTOR_CONTEXT ) );
    ChkArg( f_ppEncContext != NULL );

    ChkMem( *f_ppEncContext = ( DRM_M2TS_ENCRYPTOR_CONTEXT * )Oem_MemAlloc( SIZEOF( DRM_M2TS_ENCRYPTOR_CONTEXT ) ) );

    MEMSET( *f_ppEncContext, 0, SIZEOF( DRM_M2TS_ENCRYPTOR_CONTEXT ) );

    (*f_ppEncContext)->cbHeapAllocated = SIZEOF( DRM_M2TS_ENCRYPTOR_CONTEXT );
    (*f_ppEncContext)->cbHeapAllowed   = f_cbMaxHeapAllowed == 0 ? MAX_UNSIGNED_TYPE( DRM_DWORD ) : f_cbMaxHeapAllowed;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   DRM_M2TS_MemAlloc
**
** Synopsis:   Allocates heap using Oem_MemAlloc and records the total heap
**             allocated.  If it exceeds the max heap allowed return NULL.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_cbSize]               -- Number of bytes to allocate.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_VOID * DRM_CALL DRM_M2TS_MemAlloc(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT              *f_pEncContext,
    __in      const DRM_DWORD                                f_cbSize )
{
    DRM_RESULT   dr                 = DRM_SUCCESS;
    DRM_VOID    *pv                 = NULL;
    DRM_DWORD    cbHeapAllocated    = 0;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_cbSize > 0 );

    ChkDR( DRM_DWordAdd( f_pEncContext->cbHeapAllocated, f_cbSize, &cbHeapAllocated ) );
    if( cbHeapAllocated <= f_pEncContext->cbHeapAllowed )
    {
        pv = Oem_MemAlloc( f_cbSize );
        if( pv != NULL )
        {
#if DRM_DEBUG_M2TS
            TRACE(("DRM_M2TS_MemAlloc size=: %d", f_cbSize));
#endif /* DRM_DEBUG_M2TS */
#if DBG
            f_pEncContext->cbActualMaxHeapAllocated = max( cbHeapAllocated, f_pEncContext->cbActualMaxHeapAllocated );
#endif /* DBG */
            f_pEncContext->cbHeapAllocated = cbHeapAllocated;
        }
    }

ErrorExit:
    return pv;
}

/**********************************************************************
**
** Function:   DRM_M2TS_MemFree
**
** Synopsis:   Frees heap using Oem_MemFree and reduces the total heap
**             allocated accordingly.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pv]                   -- Pointer to a buffer to free.
** [f_cbSize]               -- Number of bytes pointed by f_pv to be freed.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_MemFree(
    __inout             DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout             DRM_VOID                            *f_pv,
    __in          const DRM_DWORD                            f_cbSize )
{
    DRM_RESULT   dr     = DRM_SUCCESS;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_cbSize <= f_pEncContext->cbHeapAllocated );
    ChkArg( ( f_pv != NULL ) == ( f_cbSize > 0 ) );

    if( f_pv != NULL )
    {
#if DRM_DEBUG_M2TS
        TRACE(("DRM_M2TS_MemFree size=: %d", f_cbSize));
#endif /* DRM_DEBUG_M2TS */
        f_pEncContext->cbHeapAllocated -= f_cbSize;
        Oem_MemFree( f_pv );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   DRM_M2TS_MemAllocAndCopy
**
** Synopsis:   Allocate a larger heap and copy the data over.  It doesn't
**             free the original allocation in f_pvIn.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_cbUnitSize]           -- Number of bytes per unit.
** [f_cCurrentUnits]        -- Number of units in the input buffer.
** [f_cIncUnits]            -- Number of units to increase.
** [f_pvIn]                 -- Pointer to the input buffer.
** [f_ppvOut]               -- Out parameter to the output buffer.  The buffer size
**                             is (f_cbUnitSizem + f_cIncUnits) * f_cbUnitSize bytes.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_ARITHMETIC_OVERFLOW
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_MemAllocAndCopy(
    __inout             DRM_M2TS_ENCRYPTOR_CONTEXT              *f_pEncContext,
    __in          const DRM_DWORD                                f_cbUnitSize,
    __in          const DRM_DWORD                                f_cCurrentUnits,
    __in          const DRM_DWORD                                f_cIncUnits,
    __in_opt      const DRM_VOID                                *f_pvIn,
    __deref_out         DRM_VOID                               **f_ppvOut )
{
    DRM_RESULT   dr             = DRM_SUCCESS;
    DRM_VOID    *pvOut          = NULL;
    DRM_DWORD    cbCurrent      = 0;
    DRM_DWORD    cbNew          = 0;
    DRM_DWORD    cTotalUnits    = 0;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_ppvOut != NULL );
    ChkArg( f_cbUnitSize > 0 );
    ChkArg( f_cIncUnits > 0 );

    *f_ppvOut = NULL;

    ChkDR( DRM_DWordAdd( f_cCurrentUnits, f_cIncUnits, &cTotalUnits ) );
    ChkDR( DRM_DWordMult( cTotalUnits, f_cbUnitSize, &cbNew ) );
    ChkDR( DRM_DWordMult( f_cCurrentUnits, f_cbUnitSize, &cbCurrent ) );

    ChkMem( pvOut = DRM_M2TS_MemAlloc( f_pEncContext, cbNew ) );

    MEMCPY( pvOut, f_pvIn, cbCurrent );

    *f_ppvOut = pvOut;
    pvOut = NULL;

ErrorExit:
    if( pvOut != NULL )
    {
        DRM_M2TS_MemFree( f_pEncContext, pvOut, cbNew );
    }
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_InitPackets
**
** Synopsis:    Allocates number of TS packets specified to the free
**              packet list
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_cRequested]           -- Number of DRM_M2TS_PACKET objects to create
**                             and place in the free packet list.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_OUTOFMEMORY if out of memory
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_InitPackets(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_DWORD                    f_cRequested )
{
    DRM_RESULT           dr          = DRM_SUCCESS;
    DRM_DWORD            iPacket     = 0;
    DRM_M2TS_PACKET     *pPacket     = NULL;

    ChkArg( f_pEncContext != NULL );

    for( iPacket = 0; iPacket < f_cRequested; iPacket++ )
    {
        ChkMem( pPacket = (DRM_M2TS_PACKET *)DRM_M2TS_MemAlloc( f_pEncContext, SIZEOF( DRM_M2TS_PACKET ) ) );

#if DBG
        f_pEncContext->cActualPacketsAllocated++;
#endif /* DBG */

        /*
        ** No need to initialize the allocated buffer because each packet will be initialized later
        ** in DRM_M2TS_GetFreePacket()
        */
        DRM_M2TS_PUSH_TO_TOP_LIST( &f_pEncContext->oFreePacketList, pPacket );

        pPacket = NULL;
    }

ErrorExit:
    SAFE_M2TS_OEM_FREE( f_pEncContext, pPacket, SIZEOF( DRM_M2TS_PACKET ) );
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_InitECMs
**
** Synopsis:    Allocates number of EMCs specified to the free ECM list
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_cRequested]           -- Number of DRM_M2TS_ECMT objects to create
**                             and place in the free ECM list.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_OUTOFMEMORY if out of memory
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_InitECMs(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_DWORD                    f_cRequested )
{
    DRM_RESULT       dr      = DRM_SUCCESS;
    DRM_DWORD        iECM    = 0;
    DRM_M2TS_ECM    *pECM    = NULL;

    ChkArg( f_pEncContext != NULL );

    for( iECM = 0; iECM < f_cRequested; iECM++ )
    {
        ChkMem( pECM = (DRM_M2TS_ECM *)DRM_M2TS_MemAlloc( f_pEncContext, SIZEOF( DRM_M2TS_ECM ) ) );

#if DBG
        f_pEncContext->cActualECMsAllocated++;
#endif /* DBG */

        /*
        ** No need to initialize the allocated buffer because each ECM will be initialized in
        ** DRM_M2TS_GetFreeECM().
        */
        DRM_M2TS_PUSH_TO_TOP_LIST( &f_pEncContext->oFreeECMList, pECM );

        pECM = NULL;
    }


ErrorExit:
    SAFE_M2TS_OEM_FREE( f_pEncContext, pECM, SIZEOF( DRM_M2TS_ECM ) );
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_CreateRemovablePID
**
** Synopsis:    Creates a new removable PID
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_wPIDRemovable]        -- A removable PID which is a PID of an ECM packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_OUTOFMEMORY if out of memory
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_CreateRemovablePID(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_WORD                     f_wPIDRemovable )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_M2TS_REMOVABLE_PID  *pRemovablePID      = NULL;

    ChkArg( f_pEncContext != NULL );

    /* Create a new removable PID */

    ChkMem( pRemovablePID = (DRM_M2TS_REMOVABLE_PID *)DRM_M2TS_MemAlloc( f_pEncContext, SIZEOF( DRM_M2TS_REMOVABLE_PID ) ) );

    MEMSET( pRemovablePID, 0, SIZEOF( DRM_M2TS_REMOVABLE_PID ) );
    pRemovablePID->wPID = f_wPIDRemovable;

    DRM_M2TS_APPEND_TO_END_OF_LIST( &f_pEncContext->oRemovablePIDList, pRemovablePID );

    pRemovablePID = NULL;

ErrorExit:
    SAFE_M2TS_OEM_FREE( f_pEncContext, pRemovablePID, SIZEOF( DRM_M2TS_REMOVABLE_PID ) );
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_CreateStream
**
** Synopsis:    Creates a new stream
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_wPID]                 -- PID of the stream to be created.
** [f_ePacketType]          -- Packet type of the stream to be created.
** [f_ppStream]             -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_OUTOFMEMORY if out of memory
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_CreateStream(
    __inout                 DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in              const DRM_WORD                     f_wPID,
    __in              const DRM_M2TS_PACKET_TYPE         f_ePacketType,
    __out_ecount_opt( 1 )   DRM_M2TS_STREAM            **f_ppStream )
{
    DRM_RESULT              dr          = DRM_SUCCESS;
    DRM_M2TS_STREAM_LIST   *pStreamList = NULL;
    DRM_M2TS_STREAM        *pStream     = NULL;
    DRM_BOOL                fFree       = FALSE;

    ChkArg( f_pEncContext != NULL );

    pStreamList = &f_pEncContext->oStreamList;

    dr = DRM_M2TS_GetStream( f_pEncContext, f_wPID, &pStream );
    if( DRM_SUCCEEDED( dr ) )
    {
        /* The stream already exist, it is fine just make sure the stream type is identical. */
        if( pStream->ePacketType != f_ePacketType )
        {
            /* Ensure that PID 0 is still the PAT packet */
            ChkBOOL( f_wPID != DRM_M2TS_PID_OF_PAT || f_ePacketType == eDRM_M2TS_PACKET_TYPE_PAT, DRM_E_M2TS_PAT_PID_IS_NOT_ZERO );
        }
    }
    else if( dr == DRM_E_NOT_FOUND )
    {
        dr = DRM_SUCCESS;

        /* Create a new stream */
        fFree = TRUE;

        ChkMem( pStream = (DRM_M2TS_STREAM *)DRM_M2TS_MemAlloc( f_pEncContext, SIZEOF( DRM_M2TS_STREAM ) ) );

        MEMSET( pStream, 0, SIZEOF( DRM_M2TS_STREAM ) );

        pStream->wPID = f_wPID;
        pStream->ePacketType = f_ePacketType;

        DRM_M2TS_APPEND_TO_END_OF_LIST( pStreamList, pStream );
    }

    if( f_ppStream != NULL )
    {
        *f_ppStream = pStream;
    }

    pStream = NULL;

ErrorExit:
    if( fFree )
    {
        SAFE_M2TS_OEM_FREE( f_pEncContext, pStream, SIZEOF( DRM_M2TS_STREAM ) );
    }
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_GetStream
**
** Synopsis:    Gets a stream from stream list that has the specified PID
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_wPID]                 -- PID of the stream to get.
** [f_ppStream]             -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOT_FOUND if the stream doesn't exist
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_GetStream(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT    *f_pEncContext,
    __in      const DRM_WORD                       f_wPID,
    __out           DRM_M2TS_STREAM              **f_ppStream )
{
    DRM_RESULT              dr          = DRM_E_NOT_FOUND;
    DRM_M2TS_STREAM        *pStream     = NULL;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_ppStream != NULL );
    *f_ppStream = NULL;

    pStream = f_pEncContext->oStreamList.pHead;

    while( pStream != NULL )
    {
        if( pStream->wPID == f_wPID )
        {
            dr = DRM_SUCCESS;
            *f_ppStream = pStream;
            break;
        }
        pStream = pStream->pNext;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_GetFreePacket
**
** Synopsis:    Get a free packet from the free pakcet list and initialize
**              it.  If the free list is empty try to allocate more pakcets
**              first.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_cbPayload]            -- The initialize size of the packet's payload in bytes.
** [f_ePacketState]         -- The initialize state of the packet.
** [f_fIncludePacketNumber] -- Indicator whether to set the packet number.
** [f_pbPacketData]         -- Packet's data and has to be 188 bytes.
** [f_ppPacket]             -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_GetFreePacket(
    __inout                                         DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in                                      const DRM_BYTE                     f_cbPayload,
    __in                                      const DRM_M2TS_PACKET_STATE        f_ePacketState,
    __in                                      const DRM_BOOL                     f_fIncludePacketNumber,
    __in_bcount_opt( DRM_M2TS_PACKET_SIZE )   const DRM_BYTE                    *f_pbPacketData,
    __out_ecount( 1 )                               DRM_M2TS_PACKET            **f_ppPacket )
{
    DRM_RESULT              dr              = DRM_SUCCESS;
    DRM_M2TS_PACKET_LIST   *pFreePacketList = NULL;
    DRM_M2TS_PACKET        *pPacket         = NULL;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_cbPayload <= DRM_M2TS_MAX_TS_PAYLOAD_SIZE );
    ChkArg( f_ppPacket != NULL );

    pFreePacketList = &f_pEncContext->oFreePacketList;

    *f_ppPacket = NULL;

    /* Allocate more packets to the free packet list if the list is empty */
    if( pFreePacketList->cItems == 0 )
    {
        DRMASSERT( pFreePacketList->pHead == NULL );
        DRMASSERT( pFreePacketList->pTail == NULL );
        ChkDR( DRM_M2TS_InitPackets( f_pEncContext, DRM_M2TS_PACKETS_TO_ALLOCATE ) );
    }

    AssertChkArg( pFreePacketList->cItems > 0 );
    AssertChkArg( pFreePacketList->pHead != NULL );
    AssertChkArg( pFreePacketList->pTail != NULL );

    DRM_M2TS_POP_FROM_TOP_LIST( pFreePacketList, DRM_M2TS_PACKET, pPacket );

    /* Initiatize the packet */
    MEMSET( pPacket, 0, SIZEOF( DRM_M2TS_PACKET ) );

    pPacket->cbPayload = f_cbPayload;
    pPacket->ePacketState = f_ePacketState;

    if( f_fIncludePacketNumber )
    {
        pPacket->dwPacketNumber = f_pEncContext->dwNextPacketNo;
    }

#if DBG
    f_pEncContext->cActualPacketsUsed = max( f_pEncContext->cActualPacketsUsed, f_pEncContext->cActualPacketsAllocated - pFreePacketList->cItems );
#endif /* DBG */

    if( f_pbPacketData != NULL )
    {
        MEMCPY( pPacket->rgbData, f_pbPacketData, DRM_M2TS_PACKET_SIZE );
    }

    *f_ppPacket = pPacket;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_GetFreeECM
**
** Synopsis:    Get a free ECM from the free ECM list and initialize
**              it.  If the free list is empty try to allocate more ECMs
**              first.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_eECMType]             -- Type of ECM to return.
** [f_ppECM]                -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_GetFreeECM(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_M2TS_ECM_TYPE            f_eECMType,
    __out           DRM_M2TS_ECM               **f_ppECM )
{
    DRM_RESULT               dr              = DRM_SUCCESS;
    DRM_M2TS_ECM_LIST       *pFreeECMList    = NULL;
    DRM_M2TS_ECM            *pECM            = NULL;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_ppECM != NULL );
    *f_ppECM = NULL;

    pFreeECMList    = &f_pEncContext->oFreeECMList;

    /* Allocate more ECMs to the free ECM list if the list is empty */
    if( pFreeECMList->cItems == 0 )
    {
        DRMASSERT( pFreeECMList->pHead == NULL );
        DRMASSERT( pFreeECMList->pTail == NULL );
        ChkDR( DRM_M2TS_InitECMs( f_pEncContext, DRM_M2TS_ECMS_TO_ALLOCATE ) );
    }

    AssertChkArg( pFreeECMList->cItems > 0 );
    AssertChkArg( pFreeECMList->pHead != NULL );
    AssertChkArg( pFreeECMList->pTail != NULL );

    DRM_M2TS_POP_FROM_TOP_LIST( pFreeECMList, DRM_M2TS_ECM, pECM );

    /* Initiatize the ECM */
    MEMSET( pECM, 0, SIZEOF( DRM_M2TS_ECM ) );

    pECM->eECMType  = f_eECMType;
    pECM->oKeyId    = f_pEncContext->oKeyId;
    pECM->ePROType  = f_pEncContext->ePROType;
    pECM->wPID      = f_pEncContext->wPID_CA;

    ChkDR( Drm_LocalLicense_AddRef( f_pEncContext->hLicense ) );
    pECM->hLicense = f_pEncContext->hLicense;

    *f_ppECM = pECM;

#if DBG
    f_pEncContext->cActualECMsUsed = max( f_pEncContext->cActualECMsUsed, f_pEncContext->cActualECMsAllocated - pFreeECMList->cItems );
#endif /* DBG */

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_AllocateSamples
**
** Synopsis:    Allocate memory for the sample array directly from heap.
**              If this is 're-alloc" for larger space, this function will
**              copy the data from old buffer to the new one.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_AllocateSamples(
    __inout    DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext )
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_M2TS_SAMPLE *pSamples       = NULL;
    DRM_DWORD        cIncSamples    = 0;

    ChkArg( f_pEncContext != NULL );

    /*
    ** DRM_M2TS_SAMPLES_BUFFER_INCREASE_PERCENTAGE < 1 and cSamplesAllocated is a DRM_DWORD.
    ** Since the double multiplication makes the value smaller but does not change its sign,
    ** no overflow can occur in the multiplication below and the cast to DRM_DWORD is safe.
    */
    DRMCASSERT( DRM_M2TS_SAMPLES_BUFFER_INCREASE_PERCENTAGE > 0 );
    DRMCASSERT( DRM_M2TS_SAMPLES_BUFFER_INCREASE_PERCENTAGE < 1 );
    DRMCASSERT( SIZEOF(f_pEncContext->cSamplesAllocated) == SIZEOF(DRM_DWORD) );
    cIncSamples = f_pEncContext->cSamplesAllocated == 0 ? DRM_M2TS_SAMPLES_PER_ECM :
                                                          (DRM_DWORD)( ((double)f_pEncContext->cSamplesAllocated) * ((double)DRM_M2TS_SAMPLES_BUFFER_INCREASE_PERCENTAGE) );

    ChkDR( DRM_M2TS_MemAllocAndCopy( f_pEncContext,
                                     SIZEOF( DRM_M2TS_SAMPLE ),
                                     f_pEncContext->cSamplesAllocated,
                                     cIncSamples,
                                     f_pEncContext->pSamples,
                                     ( DRM_VOID ** )&pSamples ) );

    /* Free the original buffer */
    ChkDR( DRM_M2TS_MemFree( f_pEncContext, f_pEncContext->pSamples, f_pEncContext->cSamplesAllocated * SIZEOF( DRM_M2TS_SAMPLE ) ) );

    /* Take ownership of the new buffer */
    ChkDR( DRM_DWordAdd( f_pEncContext->cSamplesAllocated, cIncSamples, &f_pEncContext->cSamplesAllocated ) );
    f_pEncContext->pSamples = pSamples;

    pSamples = NULL;

ErrorExit:

    /* If pSamples is not NULL then the calculation for the size shouldn't cause interger overflow */
    SAFE_M2TS_OEM_FREE( f_pEncContext, pSamples, ( f_pEncContext->cSamplesAllocated + cIncSamples ) * SIZEOF( DRM_M2TS_SAMPLE ) );

    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_AllocateSubsamples
**
** Synopsis:    Allocate memory for the sample array directly from heap.
**              If this is 're-alloc" for larger space, this function will
**              copy the data from old buffer to the new one.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_AllocateSubsamples(
    __inout    DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext )
{
    DRM_RESULT           dr             = DRM_SUCCESS;
    DRM_M2TS_SUBSAMPLE  *pSubsamples    = NULL;
    DRM_DWORD            cIncSubsamples = 0;

    ChkArg( f_pEncContext != NULL );

    /*
    ** DRM_M2TS_SAMPLES_BUFFER_INCREASE_PERCENTAGE < 1 and cSamplesAllocated is a DRM_DWORD.
    ** Since the double multiplication makes the value smaller but does not change its sign,
    ** no overflow can occur in the multiplication below and the cast to DRM_DWORD is safe.
    */
    DRMCASSERT( DRM_M2TS_SAMPLES_BUFFER_INCREASE_PERCENTAGE > 0 );
    DRMCASSERT( DRM_M2TS_SAMPLES_BUFFER_INCREASE_PERCENTAGE < 1 );
    DRMCASSERT( SIZEOF(f_pEncContext->cSubsamplesAllocated) == SIZEOF(DRM_DWORD) );
    cIncSubsamples = f_pEncContext->cSubsamplesAllocated == 0 ? DRM_M2TS_SUBSAMPLES_PER_ECM :
                                                                (DRM_DWORD)( ((double)f_pEncContext->cSubsamplesAllocated) * ((double)DRM_M2TS_SUBSAMPLES_BUFFER_INCREASE_PERCENTAGE) );

    ChkDR( DRM_M2TS_MemAllocAndCopy( f_pEncContext,
                                     SIZEOF( DRM_M2TS_SUBSAMPLE ),
                                     f_pEncContext->cSubsamplesAllocated,
                                     cIncSubsamples,
                                     f_pEncContext->pSubsamples,
                                     ( DRM_VOID ** )&pSubsamples ) );

    /* Free the original buffer */
    ChkDR( DRM_M2TS_MemFree( f_pEncContext, f_pEncContext->pSubsamples, f_pEncContext->cSubsamplesAllocated * SIZEOF( DRM_M2TS_SUBSAMPLE ) ) );

    /* Take ownership of the new buffer */
    ChkDR( DRM_DWordAdd( f_pEncContext->cSubsamplesAllocated, cIncSubsamples, &f_pEncContext->cSubsamplesAllocated ) );
    f_pEncContext->pSubsamples = pSubsamples;

    pSubsamples = NULL;

ErrorExit:

    /* If pSubsamples is not NULL then the calculation for the size shouldn't cause interger overflow */
    SAFE_M2TS_OEM_FREE( f_pEncContext, pSubsamples, ( f_pEncContext->cSubsamplesAllocated + cIncSubsamples ) * SIZEOF( DRM_M2TS_SUBSAMPLE ) );

    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_SetECMPID
**
** Synopsis:    Select a PID for the ECM packets.  Do nothing if current
**              PID is not used by the content.  This is done by checking
**              the registered PIDs.  If current PID for ECM is used
**              then select another PID in the range according MPEG2 TS
**              spec.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_SetECMPID(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pEncContext != NULL );

    if( f_pEncContext->cRegisteredPIDs > 0 )
    {
        DRM_DWORD iPID = 0;
        /*
        ** pwRegisteredPIDs is a sorted array in ascending order and
        ** the default of CA PID is 0x1FFE which is the last selectable
        ** PID according to MPEG2 TS spec.  When 0x1FFE or current CA PID
        ** is used by the content, we want to use the current CA PID minus
        ** one as the CA PID if it is not used already.  Otherwise try to
        ** minus one again.
        */
        for( iPID = f_pEncContext->cRegisteredPIDs; iPID > 0; iPID-- )
        {
            if( f_pEncContext->wPID_CA > f_pEncContext->pwRegisteredPIDs[ iPID - 1 ] )
            {
                /* The CA PID is unique, do nothing */
                break;
            }
            else if( f_pEncContext->wPID_CA == f_pEncContext->pwRegisteredPIDs[ iPID - 1 ] )
            {
                /*
                ** The CA PID is used, select another one.  The range of PID that we can select
                ** from are 0x0010 to 0x1FFE.
                */
                f_pEncContext->wPID_CA--;
                ChkBOOL( f_pEncContext->wPID_CA >= DRM_M2TS_FIRST_SELECTABLE_PID, DRM_E_M2TS_SET_CA_PID_FAILED );
            }
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_RegisterPID
**
** Synopsis:    Insert PID to pwRegisteredPIDs array if it is not already on the
**              array.  pwRegisteredPIDs array is sorted by PID in ascending order.
**              This function will increase the array when it doesn't have
**              room for a new PID.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_wPID]                 -- PID to register.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_RegisterPID(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_WORD                     f_wPID )
{
    DRM_RESULT   dr                 = DRM_SUCCESS;
    DRM_DWORD    iPID               = 0;
    DRM_BOOL     fFound             = FALSE;
    DRM_WORD    *pwRegisteredPIDs   = NULL;
    DRM_DWORD    cRegisteredPIDs    = 0;

    ChkArg( f_pEncContext != NULL );

    for( iPID = f_pEncContext->cRegisteredPIDs; iPID > 0 && !fFound; iPID-- )
    {
        if( f_pEncContext->pwRegisteredPIDs[ iPID - 1 ] == f_wPID )
        {
            fFound = TRUE;
        }
        else if( f_pEncContext->pwRegisteredPIDs[ iPID - 1 ] < f_wPID )
        {
            /* Not found, the PID should be inserted at index iPID */
            break;
        }
    }

    if( !fFound )
    {
        if( f_pEncContext->cRegisteredPIDs == f_pEncContext->cRegisteredPIDsAllocated )
        {
            /* The array is full, need to allocate more */
            ChkDR( DRM_DWordAdd( f_pEncContext->cRegisteredPIDsAllocated, DRM_M2TS_PIDS_TO_ALLOCATE, &cRegisteredPIDs ) );
            ChkDR( DRM_M2TS_MemAllocAndCopy( f_pEncContext,
                                             SIZEOF( DRM_WORD ),
                                             f_pEncContext->cRegisteredPIDsAllocated,
                                             DRM_M2TS_PIDS_TO_ALLOCATE,
                                             f_pEncContext->pwRegisteredPIDs,
                                             (DRM_VOID **)&pwRegisteredPIDs ) );

            /* Take the owenership of the new buffer */
            ChkDR( DRM_M2TS_MemFree( f_pEncContext, f_pEncContext->pwRegisteredPIDs, f_pEncContext->cRegisteredPIDsAllocated * SIZEOF( DRM_WORD ) ) );
            f_pEncContext->pwRegisteredPIDs = pwRegisteredPIDs;
            f_pEncContext->cRegisteredPIDsAllocated += DRM_M2TS_PIDS_TO_ALLOCATE;
            pwRegisteredPIDs = NULL;
        }

        /* Insert the PID at iPID position */
        if( f_pEncContext->cRegisteredPIDs > iPID )
        {
            MEMMOVE( f_pEncContext->pwRegisteredPIDs + iPID + 1,
                     f_pEncContext->pwRegisteredPIDs + iPID,
                     ( f_pEncContext->cRegisteredPIDs - iPID ) * SIZEOF( DRM_WORD ) );
        }
        f_pEncContext->pwRegisteredPIDs[ iPID ] = f_wPID;
        f_pEncContext->cRegisteredPIDs++;
    }

ErrorExit:
    if( f_pEncContext != NULL )
    {
        SAFE_M2TS_OEM_FREE( f_pEncContext, pwRegisteredPIDs, cRegisteredPIDs * SIZEOF( DRM_WORD ) );
    }
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
