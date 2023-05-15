/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMM2TSENCRYPTOR_C    
#include <oemcommon.h>
#include <drmmanager.h>
#include <drmm2tsencryptor.h>
#include <drmm2tsbitstream.h>
#include <drmm2tsbuffermgr.h>
#include <drmm2tsparsers.h>
#include <drmm2tsbuilders.h>
#include <drmm2tslogs.h>
#include <drmm2tsmacros.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_M2TS_IsM2TSEncryptorSupported(DRM_VOID) { return TRUE; }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_M2TS_IsM2TSEncryptorUnsupported(DRM_VOID) { return FALSE; }

#define CHECK_ENCRYPTION_CONTEXT_INITIALIZED( pEncContext )                     \
    do {                                                                        \
        ChkArg( (pEncContext) != NULL );                                        \
        ChkBOOL( (pEncContext)->fInit, DRM_E_M2TS_CONTEXT_NOT_INITIALIZED );    \
    } while( FALSE )

#if DBG
static DRM_NO_INLINE DRM_VOID _PrintDebugTrace(
    __in  const DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in  const DRM_RESULT                   f_dr,
    __in  const DRM_M2TS_PACKET             *f_pPacket,
    __in  const DRM_M2TS_STREAM             *f_pStream ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_VOID _PrintDebugTrace(
    __in  const DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in  const DRM_RESULT                   f_dr,
    __in  const DRM_M2TS_PACKET             *f_pPacket,
    __in  const DRM_M2TS_STREAM             *f_pStream )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( DRM_SUCCEEDED( f_dr ) )
    {
        /* Walk the linked lists and check the lengths of the linked lists */
        DRMASSERT( f_pEncContext->cActualPacketsAllocated == f_pEncContext->oFreePacketList.cItems + f_pEncContext->oIncomingPacketList.cItems + f_pEncContext->oCachedPacketList.cItems );
        DRM_M2TS_VERIFY_LINKED_LIST_LENGTH( &f_pEncContext->oFreePacketList,        DRM_M2TS_PACKET );
        DRM_M2TS_VERIFY_LINKED_LIST_LENGTH( &f_pEncContext->oIncomingPacketList,    DRM_M2TS_PACKET );
        DRM_M2TS_VERIFY_LINKED_LIST_LENGTH( &f_pEncContext->oCachedPacketList,      DRM_M2TS_PACKET );
        DRM_M2TS_VERIFY_LINKED_LIST_LENGTH( &f_pEncContext->oFreeECMList,           DRM_M2TS_ECM );
        DRM_M2TS_VERIFY_LINKED_LIST_LENGTH( &f_pEncContext->oStreamList,            DRM_M2TS_STREAM );
        DRM_M2TS_VERIFY_LINKED_LIST_LENGTH( &f_pEncContext->oRemovablePIDList,      DRM_M2TS_REMOVABLE_PID );
    }
    else
    {
        if( f_dr != DRM_E_M2TS_CONTEXT_NOT_INITIALIZED
          && f_pEncContext != NULL )
        {
            TRACE(("  Encryption Context: Next Packet#:%d", f_pEncContext->dwNextPacketNo));

            if( f_dr != DRM_E_M2TS_NEED_KEY_DATA )
            {
                TRACE(("Encryption error: 0x%08X", f_dr));
                if( f_pStream != NULL )
                {
                    DRM_DWORD nSeqNo = 0;

                    if( f_pPacket != NULL )
                    {
                        TRACE(("  Current Packet: Packet#:%d State:%d cbPayload:%d, cbHeader:%d, cbPayloadLastPES=%d, fLastOfLastPESPacket=%d",
                            f_pPacket->dwPacketNumber,
                            f_pPacket->ePacketState,
                            f_pPacket->cbPayload,
                            f_pPacket->cbHeader,
                            f_pPacket->cbPayloadLastPES,
                            ( DRM_M2TS_IS_LAST_OF_LAST_PES_PACKET( f_pPacket ) ? 1 : 0 ) ) );
                    }

                    TRACE(("  Current Stream: PID:%d, StreamType:%d, State:%d Packets:%d",
                        f_pStream->wPID,
                        f_pStream->bStreamType,
                        f_pStream->ePESState,
                        f_pStream->oPacketList.cItems));
                    f_pPacket = f_pStream->oPacketList.pHead;
                    while( f_pPacket != NULL )
                    {
                        TRACE(("    Packet %d: Packet#:%d State:%d cbPayload:%d, cbHeader:%d, cbPayloadLastPES=%d, fLastOfLastPESPacket=%d",
                            nSeqNo++,
                            f_pPacket->dwPacketNumber,
                            f_pPacket->ePacketState,
                            f_pPacket->cbPayload,
                            f_pPacket->cbHeader,
                            f_pPacket->cbPayloadLastPES,
                            ( DRM_M2TS_IS_LAST_OF_LAST_PES_PACKET( f_pPacket ) ? 1 : 0 ) ) );
                        f_pPacket = f_pPacket->pNextPerUnit;
                    }
                }
            }
        }
    }

ErrorExit:
    DRMASSERT( DRM_SUCCEEDED( dr ) );
}
#endif /* DBG */

/**********************************************************************
**
** Function:    Drm_M2ts_Initialize
**
** Synopsis:    Create an encryptor handle and set an limit of the max heap
**              that the encryptor can use.  There is not limit when it is
**              zero.
**              Initialize the encryptor context to be ready for recryption.
**
** Arguments:   
**
**  [f_cbMaxHeapAllowed]   -- The max amount of heap that the 
**                            encryptor can use, 0 means no limit
**  [f_phEncryptor]        -- The handle of the encryptor.  It
**                            is actually a pointer to the 
**                            encryptor context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_Initialize(
    __in  const DRM_DWORD                    f_cbMaxHeapAllowed,
    __out       DRM_M2TS_ENCRYPTOR_HANDLE   *f_phEncryptor )
{
    DRM_RESULT                               dr             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_CONTEXT              *pEncContext    = NULL;
    DRM_M2TS_STREAM                         *pPATStream     = NULL;

    ChkArg( f_phEncryptor != NULL );

    *f_phEncryptor = DRM_M2TS_ENCRYPTOR_HANDLE_INVALID;

    /* Allocate and initialize memory for the encryptor context */
    ChkDR( DRM_M2TS_MemAllocEncryptorContext( f_cbMaxHeapAllowed, &pEncContext ) );

    /* Initialize the content */
    pEncContext->fEncryptAudio      = TRUE;
    pEncContext->bScramblingControl = DRM_M2TS_SCRAMBLING_CONTROL_01;
    pEncContext->fWaitingForNewKey  = TRUE;
    pEncContext->wPID_CA             = DRM_M2TS_PLAYREADY_DEFAULT_CA_PID;

    /* Allocate a block of free packets */
    ChkDR( DRM_M2TS_InitPackets( pEncContext, DRM_M2TS_INITIAL_PACKET_LIST_LENGTH ) );

    /* Allocate a block of free ECMs */
    ChkDR( DRM_M2TS_InitECMs( pEncContext, DRM_M2TS_INITIAL_ECM_LIST_LENGTH ) );

    /* Create the first stream type, PAT, on the stream list. */
    ChkDR( DRM_M2TS_CreateStream( pEncContext,
                                  DRM_M2TS_PID_OF_PAT,
                                  eDRM_M2TS_PACKET_TYPE_PAT,
                                  &pPATStream ) );

    /* Add the PID of the PAT to the array of PIDs */
    ChkDR( DRM_M2TS_RegisterPID( pEncContext, DRM_M2TS_PID_OF_PAT ) );

    /* Allocate two blocks of memory for the sample and subsample data */
    ChkDR( DRM_M2TS_AllocateSamples( pEncContext ) );
    ChkDR( DRM_M2TS_AllocateSubsamples( pEncContext ) );

    pEncContext->fInit = TRUE;

    *f_phEncryptor = ( DRM_M2TS_ENCRYPTOR_HANDLE )pEncContext;
    pEncContext = NULL;

ErrorExit:
    if( pEncContext != NULL )
    {
        DRM_RESULT dr2 = DRM_SUCCESS;

        /*
        ** Pretend that the context was initialized successfully so we can use Drm_M2ts_Uninitialize to free memory.
        ** Otherwise, we'll end up leaking memory because the caller will get DRM_E_INVALIDARG if they call
        ** Drm_M2ts_Uninitialize on a handle that was not initialized successfully.
        */
        pEncContext->fInit = TRUE;

        dr2 = Drm_M2ts_Uninitialize( ( DRM_M2TS_ENCRYPTOR_HANDLE )pEncContext );

        /* pEncContext now points to invalid memory */

        pEncContext = NULL;

        DRMASSERT( DRM_SUCCEEDED( dr2 ) );
    }
    return dr;
}

/**********************************************************************
**
** Function:    Drm_M2ts_Uninitialize
**
** Synopsis:    Delete all previous allocated buffers, close all license
**              handles and delete encryptor context
**
** Arguments:   
**
** [f_hEncryptor]           -- The handle of the encryptor.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_CONTEXT_NOT_INITIALIZED if the 
**                              encryptor handle hasn't been initialized
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_Uninitialize(
    __in  DRM_M2TS_ENCRYPTOR_HANDLE  f_hEncryptor )
{
    DRM_RESULT                       dr             = DRM_SUCCESS;
    DRM_M2TS_PACKET                 *pPacket        = NULL;
    DRM_M2TS_PACKET                 *pPacketNext    = NULL;
    DRM_M2TS_ENCRYPTOR_CONTEXT      *pEncContext    = ( DRM_M2TS_ENCRYPTOR_CONTEXT * )f_hEncryptor;
    DRM_DWORD                        dwSize         = 0;

    CHECK_ENCRYPTION_CONTEXT_INITIALIZED( pEncContext );

    if( pEncContext->hLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        ChkDR( Drm_LocalLicense_Release( &pEncContext->hLicense ) );
        pEncContext->hLicense = DRM_LICENSE_HANDLE_INVALID;
    }

    /* Free memory */
    ChkDR( DRM_DWordMult( pEncContext->cSamplesAllocated, SIZEOF( DRM_M2TS_SAMPLE ), &dwSize ) );
    SAFE_M2TS_OEM_FREE( pEncContext, pEncContext->pSamples, dwSize );
    ChkDR( DRM_DWordMult( pEncContext->cSubsamplesAllocated, SIZEOF( DRM_M2TS_SUBSAMPLE ), &dwSize ) );
    SAFE_M2TS_OEM_FREE( pEncContext, pEncContext->pSubsamples, dwSize );
    ChkDR( DRM_DWordMult( pEncContext->cEncryptedSegments, SIZEOF( DRM_VOID * ), &dwSize ) );
    SAFE_M2TS_OEM_FREE( pEncContext, pEncContext->ppbEncryptedSegments, dwSize );
    ChkDR( DRM_DWordMult( pEncContext->cEncryptedSegments, SIZEOF( DRM_DWORD ), &dwSize ) );
    SAFE_M2TS_OEM_FREE( pEncContext, pEncContext->prgcbEncryptedSegments, dwSize );
    ChkDR( DRM_DWordMult( pEncContext->cRegisteredPIDsAllocated, SIZEOF( DRM_WORD ), &dwSize ) );
    SAFE_M2TS_OEM_FREE( pEncContext, pEncContext->pwRegisteredPIDs, dwSize );

    DRM_M2TS_FREE_LIST( pEncContext, &pEncContext->oFreePacketList,         DRM_M2TS_PACKET );
    DRM_M2TS_FREE_LIST( pEncContext, &pEncContext->oCachedPacketList,       DRM_M2TS_PACKET );
    DRM_M2TS_FREE_LIST( pEncContext, &pEncContext->oFreeECMList,            DRM_M2TS_ECM );
    DRM_M2TS_FREE_LIST( pEncContext, &pEncContext->oStreamList,             DRM_M2TS_STREAM );
    DRM_M2TS_FREE_LIST( pEncContext, &pEncContext->oRemovablePIDList,       DRM_M2TS_REMOVABLE_PID );

    /* Free incoming packet list */
    pPacket = pEncContext->oIncomingPacketList.pHead;
    while( pPacket != NULL )
    {
        if( pPacket->pECMData != NULL )
        {
            DRM_M2TS_ECM *pECM = ( DRM_M2TS_ECM * )pPacket->pECMData;

            DRMASSERT( pECM->hLicense != DRM_LICENSE_HANDLE_INVALID );
            if( pECM->hLicense != DRM_LICENSE_HANDLE_INVALID )
            {
                ChkDR( Drm_LocalLicense_Release( &pECM->hLicense ) );
                pECM->hLicense = DRM_LICENSE_HANDLE_INVALID;
            }

            (DRM_VOID)Drm_Prnd_MemFree( pECM->pbPROBlob );

            SAFE_M2TS_OEM_FREE( pEncContext, pPacket->pECMData, SIZEOF( DRM_M2TS_ECM ) );
        }

        pPacketNext = pPacket->pNext;

        ChkDR( DRM_M2TS_MemFree( pEncContext, pPacket, SIZEOF( DRM_M2TS_PACKET ) ) );

        pPacket = pPacketNext;
        ChkDR( DRM_DWordDecOne( pEncContext->oIncomingPacketList.cItems ) );
    }
    DRMASSERT( pEncContext->oIncomingPacketList.cItems == 0 );

    /* Check if there is a memory leak.  The only thing left is the encryptor context. */
    DRMASSERT( pEncContext->cbHeapAllocated == SIZEOF( DRM_M2TS_ENCRYPTOR_CONTEXT ) );
#if DBG
    {
        TRACE(("Memory Consumption:\n"));
        TRACE(("  Total Packets Allocated:        %d\n", pEncContext->cActualPacketsAllocated));
        TRACE(("  Total ECMs Allocated:           %d\n", pEncContext->cActualECMsAllocated));
        TRACE(("  Max. Number of Packets Used:    %d\n", pEncContext->cActualPacketsUsed));
        TRACE(("  Max. Number of ECM Used:        %d\n", pEncContext->cActualECMsUsed));
        TRACE(("  Max. Number of Samples Used:    %d\n", pEncContext->cActualSamplesUsed));
        TRACE(("  Max. Number of Subsample used:  %d\n", pEncContext->cActualSubsamplesUsed));
        TRACE(("  Max Heap Allocated:             %d\n", pEncContext->cbActualMaxHeapAllocated));
    }
#endif /* DBG */

    /* Finally, delete the encryptor context */
    pEncContext->fInit = FALSE;
    SAFE_M2TS_OEM_FREE( pEncContext, pEncContext, SIZEOF( DRM_M2TS_ENCRYPTOR_CONTEXT ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    Drm_M2ts_SetEncryptionProperty
**
** Synopsis:    Set the encryptor properties
**
** Arguments:   
**
** [f_hEncryptor]           -- The encryptor handle
** [f_poProperty]           -- The property to set
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_CONTEXT_NOT_INITIALIZED if the 
**                              encryptor handle hasn't been initialized
**                          DRM_E_M2TS_CANNOT_CHANGE_PARAMETER if the encryption
**                              has started, i.e. Drm_M2ts_Encrypt has been called.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_SetEncryptionProperty(
        __in                DRM_M2TS_ENCRYPTOR_HANDLE        f_hEncryptor,
        __in          const DRM_M2TS_ENCRYPTION_PROPERTY    *f_poProperty )
{
    DRM_RESULT                    dr          = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_CONTEXT   *pEncContext = ( DRM_M2TS_ENCRYPTOR_CONTEXT * )f_hEncryptor;

    CHECK_ENCRYPTION_CONTEXT_INITIALIZED( pEncContext );

    ChkArg( f_poProperty != NULL );
    ChkArg( f_poProperty->ePropertyType == eDRM_M2TS_PROPERTY_TYPE_ENCYRPT_AUDIO
         || f_poProperty->ePropertyType == eDRM_M2TS_PROPERTY_TYPE_ENABLE_LOGGING );

    ChkBOOL( !pEncContext->fEncryptionStarted, DRM_E_M2TS_CANNOT_CHANGE_PARAMETER );

    if( f_poProperty->ePropertyType == eDRM_M2TS_PROPERTY_TYPE_ENCYRPT_AUDIO )
    {
        pEncContext->fEncryptAudio = f_poProperty->Value.fValue;
    }
    else if( f_poProperty->ePropertyType == eDRM_M2TS_PROPERTY_TYPE_ENABLE_LOGGING )
    {
        pEncContext->pLoggingCallbackContext = f_poProperty->Value.LoggingCallback.pCallbackContext;
        pEncContext->pfnLoggingCallback = f_poProperty->Value.LoggingCallback.pfnLoggingCallback;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    Drm_M2ts_SetKeyData
**
** Synopsis:    Set the key data for encryption before calling
**              calling Drm_M2ts_Encrypt to set the initial key data for
**              encryption.
**              For key rotation content, the caller can use this
**              function to set a new key whenever Drm_M2ts_Encrypt returns
**              DRM_E_M2TS_NEED_KEY_DATA or anytime the caller wants to
**              change the key.
**
** Arguments:   
**
** [f_hEncryptor]           -- The encryptor handle
** [f_hLicense]             -- The handle of the license for encryption
** [f_poKeyId]              -- The key ID of the license
** [f_ePROType]             -- Type of PlayReady object to embed to 
**                             PSSH of the ECM
** [f_dwDuration]           -- The duration of the key, 0 - no key rotation
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_CONTEXT_NOT_INITIALIZED if the 
**                              encryptor handle hasn't been initialized
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_SetKeyData(
    __in                DRM_M2TS_ENCRYPTOR_HANDLE        f_hEncryptor,
    __in                DRM_LICENSE_HANDLE               f_hLicense,
    __in          const DRM_GUID                        *f_poKeyId,
    __in          const DRM_M2TS_PRO_TYPE                f_ePROType,
    __in          const DRM_DWORD                        f_dwDuration )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_CONTEXT  *pEncContext    = ( DRM_M2TS_ENCRYPTOR_CONTEXT * )f_hEncryptor;
    DRM_M2TS_STREAM             *pStream        = NULL;
    DRM_GUID                     oEmptyGUID     = EMPTY_DRM_GUID;

    CHECK_ENCRYPTION_CONTEXT_INITIALIZED( pEncContext );

    ChkArg( f_hLicense != DRM_LICENSE_HANDLE_INVALID );
    ChkArg( f_poKeyId  != NULL && !IDENTICAL_GUIDS( f_poKeyId, &oEmptyGUID ) );
    ChkArg( f_ePROType == eDRM_M2TS_PRO_TYPE_NO_PRO
         || f_ePROType == eDRM_M2TS_PRO_TYPE_KEY_ROTATION_WITH_LICENSE_IN_PRO
         || f_ePROType == eDRM_M2TS_PRO_TYPE_KEY_ROTATION_WITH_WRMHEADER_IN_PRO );

    pEncContext->fWaitingForNewKey      = FALSE;
    pEncContext->oKeyId                 = *f_poKeyId;
    pEncContext->ePROType               = f_ePROType;
    pEncContext->qwCurrentKeyDuration   = f_dwDuration == 0
                                          ? DRM_UI64HL( 0, 0 )
                                          : DRM_UI64Mul( DRM_UI64HL( 0, f_dwDuration ), DRM_UI64HL( 0, 10000000 ) );  /* convert from seconds to Hns (100 nanosecond) */

    if( pEncContext->hLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        ChkDR( Drm_LocalLicense_Release( &pEncContext->hLicense ) );
        pEncContext->hLicense = DRM_LICENSE_HANDLE_INVALID;
    }
    ChkDR( Drm_LocalLicense_AddRef( f_hLicense ) );
    pEncContext->hLicense = f_hLicense;

    /* reset the aggregated duration in each stream */
    pStream = pEncContext->oStreamList.pHead;
    while( pStream != NULL )
    {
        pStream->qwAggregatedDuration = DRM_UI64HL( 0, 0 );
        pStream = ( DRM_M2TS_STREAM * )pStream->pNext;
    }

    /* Insert a ECM for the new key data if encryption has started */
    if( pEncContext->pCurrentECM != NULL )
    {
        ChkDR( DRM_M2TS_InsertECM( pEncContext, eDRM_M2TS_ECM_TYPE_KR ) );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _CanPIDBeRemoved
**
** Synopsis:    Return TRUE if the PID is on the removable wPID list,
**              otherwise return FALSE.
**
** Arguments:   
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_wPID]                 -- PID to check whether it can be removed
**
** Returns:                 TRUE - yes, can be removed
**                          FALSE - no, cannot be removed
**
**********************************************************************/
static DRM_NO_INLINE DRM_BOOL _CanPIDBeRemoved(
    __in  const DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __in  const DRM_WORD                         f_wPID ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL _CanPIDBeRemoved(
    __in  const DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __in  const DRM_WORD                         f_wPID )
{
    const DRM_M2TS_REMOVABLE_PID *pRemovablePID  = NULL;

    DRMASSERT( f_pEncContext != NULL );
    __analysis_assume( f_pEncContext != NULL );

    pRemovablePID  = f_pEncContext->oRemovablePIDList.pHead;
    while( pRemovablePID != NULL )
    {
        if( pRemovablePID->wPID == f_wPID )
        {
            return TRUE;
        }
        pRemovablePID = pRemovablePID->pNext;
    }

    return FALSE;
}

/**********************************************************************
**
** Function:    _ResetStream
**
** Synopsis:    Something wrong with current PES in the stream. It becomes
**              useless.  We need to clear all the packets on the packet list
**              and mark last PES as eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD.
** Arguments:   
**
** [f_pStream]              -- Pointer to a stream object to be reset.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ResetStream(
    __inout     DRM_M2TS_STREAM     *f_pStream ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ResetStream(
    __inout     DRM_M2TS_STREAM     *f_pStream )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pStream != NULL );
    __analysis_assume( f_pStream != NULL );

    /* Update last PES */

    if( f_pStream->pLastPES != NULL )
    {
        /*
        ** Set last PES' state to eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD.
        ** If the data alignment flag of current PES is 0 it is likely that
        ** the last sample in the last PES will be incomplete due to reset.
        */
        f_pStream->pLastPES->ePESState = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;
        f_pStream->pLastPES->pHeadOfOverflowedPackets = NULL;
    }

    /* Reset stream's states */

    f_pStream->qwPTS              = DRM_UI64HL( 0, 0 );
    f_pStream->cbHeader           = 0;
    f_pStream->cbPESPacketLength  = 0;
    f_pStream->bDataAlignmentFlag = DRM_M2TS_PES_DATA_NOT_ALIGNED;
    f_pStream->ePESState          = eDRM_M2TS_PES_STATE_BEGIN;
    f_pStream->pLastPES           = NULL;

    /* Mark all packets on the PES' packet list as bad and clear the per unit list */
    DRM_M2TS_SET_PACKET_STATE_ON_PERUNIT_LIST( DRM_M2TS_PACKET, &f_pStream->oPacketList, eDRM_M2TS_PACKET_STATE_BAD );
    DRM_M2TS_INIT_LIST( f_pStream->oPacketList );

    return dr;
}

/**********************************************************************
**
** Function:    _UnblockPESPackets
**
** Synopsis:    Check each audio and video streams if there is any stream
**              that we haven't received the next TS packet for a predefined
**              time (in packets) we need to drop it in order to unblock the
**              encryption.
**
** Arguments:   
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _UnblockPESPackets(
    __inout             DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _UnblockPESPackets(
    __inout             DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext )
{
    DRM_RESULT           dr             = DRM_SUCCESS;
    DRM_M2TS_STREAM     *pStream        = NULL;

    DRMASSERT( f_pEncContext != NULL );
    __analysis_assume( f_pEncContext != NULL );

    pStream = f_pEncContext->oStreamList.pHead;
    while( pStream != NULL )
    {
        if( DRM_M2TS_IS_PES( pStream->ePacketType )
         && pStream->oPacketList.cItems > 0 )
        {
            DRM_DWORD cPackets =   f_pEncContext->dwNextPacketNo >= pStream->oPacketList.pTail->dwPacketNumber
                                 ? f_pEncContext->dwNextPacketNo - pStream->oPacketList.pTail->dwPacketNumber
                                 : f_pEncContext->dwNextPacketNo + ( MAX_UNSIGNED_TYPE( DRM_DWORD ) - pStream->oPacketList.pTail->dwPacketNumber );
            if( cPackets > DRM_M2TS_MAX_PES_PACKET_SPAN )
            {
                ChkDR( DRM_M2TS_RegisterAndTransferPES( f_pEncContext, pStream, TRUE /* stream stopped */ ) );
            }
        }

        pStream = pStream->pNext;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _UnblockIncomingList
**
** Synopsis:    To unblock the incoming packet list is to set the packet
**              at front as a bad packet.  We can do that if we are in
**              the closing session mode or the blocking PAT or PMT packet
**              has been in the incomplete state for long periof of time.
**
** Arguments:   
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pIncomingPcketList]   -- Pointer to the incoming packet list
** [f_fClosingSession]      -- Flag to indicate whether it is the end of encryption
** [f_pfUnblocked]          -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _UnblockIncomingList(
    __inout             DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in          const DRM_M2TS_PACKET_LIST        *f_pIncomingPcketList,
    __in          const DRM_BOOL                     f_fClosingSession,
    __out               DRM_BOOL                    *f_pfUnblocked ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _UnblockIncomingList(
    __inout             DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in          const DRM_M2TS_PACKET_LIST        *f_pIncomingPcketList,
    __in          const DRM_BOOL                     f_fClosingSession,
    __out               DRM_BOOL                    *f_pfUnblocked )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_BOOL             fCanDrop   = FALSE;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pIncomingPcketList != NULL );
    __analysis_assume( f_pIncomingPcketList != NULL );
    DRMASSERT( f_pIncomingPcketList->pHead != NULL );
    DRMASSERT( f_pfUnblocked != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pIncomingPcketList->pHead != NULL );
    __analysis_assume( f_pfUnblocked != NULL );

    *f_pfUnblocked = FALSE;

    if( f_fClosingSession )
    {
        fCanDrop = TRUE;
    }
    else
    {
        DRM_WORD               wPID       = DRM_M2TS_GET_PID( f_pIncomingPcketList->pHead->rgbData );
        DRM_M2TS_STREAM       *pStream    = NULL;
        const DRM_M2TS_PACKET *pPacket    = NULL;

        ChkDR( DRM_M2TS_GetStream( f_pEncContext, wPID, &pStream ) );
        if( pStream->ePacketType == eDRM_M2TS_PACKET_TYPE_PAT || pStream->ePacketType == eDRM_M2TS_PACKET_TYPE_PMT )
        {
            if( f_pIncomingPcketList->cItems > DRM_M2TS_MAX_PAT_PMT_SPAN )
            {
                /*
                ** See if the head packet on the incoming packet list is on the stream packet list
                ** if it is we have to reset the stream's packet list because the head packet will be
                ** dropped.
                */
                pPacket = pStream->oPacketList.pHead;
                while( pPacket != NULL )
                {
                    if( pPacket == f_pIncomingPcketList->pHead )
                    {
                        /* reset the PAT/PMT stream */
                        pStream->cbHeader = 0;
                        pStream->ePESState = eDRM_M2TS_PES_STATE_BEGIN;
                        DRM_M2TS_INIT_LIST( pStream->oPacketList );
                        break;
                    }
                    pPacket = pPacket->pNextPerUnit;
                }

                fCanDrop = TRUE;
            }
        }
        else
        {
            /* 
            ** We don't expect other types of packets will be at front of the incoming packet list.
            ** But it can happen if a stream contains garbages and the first byte of a packet 
            ** just happens to be 0x47 (the TS packet's sync byte).
            */
            fCanDrop = TRUE;
        }
    }

    if( fCanDrop )
    {
        /* set the head packet as bad and log an event */
        f_pIncomingPcketList->pHead->ePacketState = eDRM_M2TS_PACKET_STATE_BAD;
        (DRM_VOID)DRM_M2TS_LogEventWithPacket( f_pEncContext, eDRM_M2TS_LOG_CATEGORY_WARNING, TRUE, DRM_E_M2TS_WAITED_TOO_LONG, f_pIncomingPcketList->pHead );

        *f_pfUnblocked = TRUE;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _RecheckLastPESPayload
**
** Synopsis:    See if the start code is split into two packets,
**              one in the last packet of the last PES and the other
**              in the current PES' first packet.  If that's the case
**              adjust the last PES' state.
**
** Arguments:   
**
** [f_pStream]              -- Pointer to the stream object to be 
**                             rechecked.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_INTERNAL_ERROR when something
**                              unexpected happened.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _RecheckLastPESPayload(
    __inout     DRM_M2TS_STREAM     *f_pStream ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _RecheckLastPESPayload(
    __inout     DRM_M2TS_STREAM     *f_pStream )
{
    DRM_RESULT           dr                     = DRM_SUCCESS;
    DRM_M2TS_PACKET     *pPacketLastPES         = NULL;
    DRM_M2TS_PACKET     *pPacketCurrentPES      = NULL;

    ChkArg( f_pStream != NULL );
    ChkArg( f_pStream->pLastPES != NULL );

    pPacketLastPES      = f_pStream->pLastPES->oPacketList.pTail;
    pPacketCurrentPES   = f_pStream->oPacketList.pHead;

    if( pPacketLastPES != NULL
     && pPacketCurrentPES != NULL )
    {
        DRM_BYTE    rgbBuffer[ DRM_M2TS_PACKET_SIZE * 2 ]   = { 0 };
        DRM_DWORD   dwStartCodeOffset                       = 0;
        DRM_BYTE    cbPayloadLastPES                        = 0;
        DRM_BYTE    cbPayloadCurrentPES                     = 0;
        DRM_BYTE    bOffsetLastPES                          = 0;
        DRM_BYTE    bOffsetCurrentPES                       = 0;
        DRM_DWORD   cbBuffer                                = 0;
        DRM_BYTE    cbDelta                                 = (DRM_BYTE)( DRM_M2TS_IS_START_WITH_PARTIAL_START_CODE( pPacketLastPES ) ? 1 : 0 );

        ChkDR( DRM_ByteSub( pPacketLastPES->cbPayload, pPacketLastPES->cbHeader, &cbPayloadLastPES ) );
        ChkDR( DRM_ByteSub( pPacketCurrentPES->cbPayload, pPacketCurrentPES->cbHeader, &cbPayloadCurrentPES ) );
        ChkDR( DRM_DWordAdd( cbPayloadLastPES, cbPayloadCurrentPES, &cbBuffer ) );

        /*
        ** Copy the payload of last PES' last packet and the first packet of current PES
        ** to the buffer in order to search the start code.
        */
        ChkBOOL( cbBuffer > 0 && cbBuffer < SIZEOF( rgbBuffer ), DRM_E_M2TS_INTERNAL_ERROR );
        ChkDR( DRM_ByteSub( DRM_M2TS_PACKET_SIZE, cbPayloadLastPES, &bOffsetLastPES ) );
        ChkDR( DRM_ByteSub( DRM_M2TS_PACKET_SIZE, cbPayloadCurrentPES, &bOffsetCurrentPES ) );
        MEMCPY( rgbBuffer,
                &pPacketLastPES->rgbData[ bOffsetLastPES ], 
                cbPayloadLastPES );
        MEMCPY( rgbBuffer + cbPayloadLastPES,
                &pPacketCurrentPES->rgbData[ bOffsetCurrentPES ], 
                cbPayloadCurrentPES );

        /* Try to find the sync code */
        /* 
        ** cbDelta is 1 when the tail packet of the last PES contains a partial start code of previous PES
        ** in that case we intentionally skip the first byte to avoid a partial start code of 0x000001 
        ** being mistaken as a real start code.
        */
        if( DRM_M2TS_FindVideoStartCode( f_pStream, rgbBuffer + cbDelta, cbBuffer - cbDelta, &dwStartCodeOffset )
         && dwStartCodeOffset < cbPayloadLastPES )
        {
            /* We do have a partial start code in the last few bytes of the last packet of the last PES */

            /* Adjust the overflow payload of last PES */
            ChkDR( DRM_DWordToByte( dwStartCodeOffset + cbDelta, &pPacketLastPES->cbPayloadLastPES ) );

            /*
            ** Set the state back to eDRM_M2TS_PES_STATE_HAS_SECTION_HEADER, so DRM_M2TS_CheckPESCompleteness
            ** will check the completeness of the last PES.
            */
            f_pStream->pLastPES->ePESState = eDRM_M2TS_PES_STATE_HAS_SECTION_HEADER;

            pPacketCurrentPES->wPacketFlags |= DRM_M2TS_PACKET_FLAG_START_WITH_PARTIAL_START_CODE;
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _ReturnEncryptedPackets
**
** Synopsis:    Seach the TS packets in the order of the incoming packet chain
**              and return all the packets that have 'ready' state and stop
**              searching once it finds a packet that is not in that state.
**
**              Throw away TS packets that are in 'bad' state.
**
**              Put the packets back to the free packet chain once the TS
**              packets are returned or thrown away.
**
** Arguments:   
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_cbDataOut]            -- Length of the output buffer in bytes
** [f_pbDataOut]            -- Pointer to the output buffer to store the encypted/
**                             processed packets
** [f_pcbReturned]          -- Out parameter to contain the byte count of the 
**                             returned packets.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_OUTPUT_BUFFER_FULL when the output
**                              buffer is full.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ReturnEncryptedPackets(
    __inout                         DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in                      const DRM_DWORD                    f_cbDataOut,
    __out_bcount(f_cbDataOut)       DRM_BYTE                    *f_pbDataOut,
    __out                           DRM_DWORD                   *f_pcbReturned ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ReturnEncryptedPackets(
    __inout                         DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in                      const DRM_DWORD                    f_cbDataOut,
    __out_bcount(f_cbDataOut)       DRM_BYTE                    *f_pbDataOut,
    __out                           DRM_DWORD                   *f_pcbReturned )
{
    DRM_RESULT               dr                     = DRM_SUCCESS;
    const DRM_BYTE          *pbDataEnd              = NULL;
    DRM_BYTE                *pbDataCurrent          = NULL;
    DRM_M2TS_PACKET_LIST    *pIncomingPacketList    = NULL;
    DRM_M2TS_PACKET         *pPacket                = NULL;
    DRM_M2TS_STREAM         *pStream                = NULL;
    DRM_WORD                 wPID                   = 0;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pbDataOut != NULL );
    DRMASSERT( f_pcbReturned != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pbDataOut != NULL );
    __analysis_assume( f_pcbReturned != NULL );

    pbDataEnd              = f_pbDataOut + f_cbDataOut;
    pIncomingPacketList    = &f_pEncContext->oIncomingPacketList;

    while( pIncomingPacketList->pHead != NULL )
    {
        pPacket = pIncomingPacketList->pHead;

        if( pPacket->ePacketState == eDRM_M2TS_PACKET_STATE_READY )
        {
            /* The TS packet is ready to return to caller */

            pbDataCurrent = f_pbDataOut + *f_pcbReturned;
            if( ( pbDataCurrent + DRM_M2TS_PACKET_SIZE ) > pbDataEnd )
            {
                /* There are more data to return but the output buffer is full already */
                ChkDR( DRM_E_M2TS_OUTPUT_BUFFER_FULL );
            }

            /* Update the continuity counter if it is a known stream */
            wPID = DRM_M2TS_GET_PID( pPacket->rgbData );
            if( DRM_SUCCEEDED( DRM_M2TS_GetStream( f_pEncContext, wPID, &pStream ) ) )
            {
                DRM_M2TS_UPDATE_CONTINUITY_COUNTER( pStream, pPacket, pPacket->wPacketFlags );
            }

            /* copy processed/encrypted TS packet data to the output data buffer */
            MEMCPY( pbDataCurrent, pIncomingPacketList->pHead->rgbData, DRM_M2TS_PACKET_SIZE );

            /* Recycle the packet to the free list */
            DRM_M2TS_POP_FROM_TOP_LIST( pIncomingPacketList, DRM_M2TS_PACKET, pPacket );
            DRM_M2TS_PUSH_TO_TOP_LIST( &f_pEncContext->oFreePacketList, pPacket );

            *f_pcbReturned += DRM_M2TS_PACKET_SIZE;
        }
        else if( pPacket->ePacketState == eDRM_M2TS_PACKET_STATE_BAD )
        {
            /*
            ** The TS packet is bad, throw it away
            ** Recycle the packet to the free list
            */
            DRM_M2TS_POP_FROM_TOP_LIST( pIncomingPacketList, DRM_M2TS_PACKET, pPacket );
            DRM_M2TS_PUSH_TO_TOP_LIST( &f_pEncContext->oFreePacketList, pPacket );
        }
        else
        {
            /* The TS packet isn't ready yet, no need to continue */
            break;
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _EncryptPESPackets
**
** Synopsis:    Start the encryption if the PES packets in the ECM are
**              all completed.  Druing closing session it will
**              mark all pending PES as completed so it can flush and
**              encrypt all remaining PES packets.
**
** Arguments:   
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_fClosingSession]      -- Flag to indicate whether it is end of encryption.
** [f_cbDataOut]            -- Length of the output buffer in bytes
** [f_pbDataOut]            -- Pointer to the output buffer to store the encypted/
**                             processed packets
** [f_pcbReturned]          -- Out parameter to contain the byte count of the 
**                             returned packets.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_OUTPUT_BUFFER_FULL when the output
**                              buffer is full.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _EncryptPESPackets(
    __inout                             DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __in                          const DRM_BOOL                         f_fClosingSession,
    __in                          const DRM_DWORD                        f_cbDataOut,
    __out_bcount(f_cbDataOut)           DRM_BYTE                        *f_pbDataOut,
    __out                               DRM_DWORD                       *f_pcbReturned ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _EncryptPESPackets(
    __inout                             DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __in                          const DRM_BOOL                         f_fClosingSession,
    __in                          const DRM_DWORD                        f_cbDataOut,
    __out_bcount(f_cbDataOut)           DRM_BYTE                        *f_pbDataOut,
    __out                               DRM_DWORD                       *f_pcbReturned )
{
    DRM_RESULT               dr                     = DRM_SUCCESS;
    DRM_M2TS_PACKET_LIST    *pIncomingPacketList    = NULL;
    DRM_M2TS_ECM            *pECM                   = NULL;
    DRM_M2TS_PACKET_LIST     ecmPacketList          = { 0 };
    DRM_M2TS_PACKET         *pECMPlaceHolderPacket  = NULL;
    DRM_BOOL                 fReadyToEncrypt        = TRUE;
    DRM_BOOL                 fEncrypted             = FALSE;
    DRM_BOOL                 fUnblocked             = FALSE;
    DRM_DWORD                iPES;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pbDataOut != NULL );
    DRMASSERT( f_pcbReturned != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pbDataOut != NULL );
    __analysis_assume( f_pcbReturned != NULL );

    pIncomingPacketList = &f_pEncContext->oIncomingPacketList;

    do
    {
        /* First, try to return all the processed/encrypted packets back */

        ChkDR( _ReturnEncryptedPackets( f_pEncContext,
                                        f_cbDataOut,
                                        f_pbDataOut,
                                        f_pcbReturned ) );

        /*
        ** Normally, the packet that will cause _ReturnEncryptedPackets() to stop is the packet that
        ** represents a ECM which is just a place holder for ECM and will be replaced with the
        ** real TS packets for the ECM after the packets belong to the ECM are encrypted.
        */
        if( pIncomingPacketList->pHead == NULL )
        {
            break;
        }

        pECM = ( DRM_M2TS_ECM * )pIncomingPacketList->pHead->pECMData;
        if( pECM == NULL )
        {
            /* The ECM packet is not at front, try to unblock it if it is possible */
            ChkDR( _UnblockIncomingList( f_pEncContext, pIncomingPacketList, f_fClosingSession, &fUnblocked ) );
            if( fUnblocked )
            {
                if (f_fClosingSession)
                {
                    /* Set fEncrypted to true so it will continue to encrypt the rest of the packets */
                    fEncrypted = TRUE;
                }
                continue;
            }
            else
            {
                /* We need to wait.  If it is still not completed after a predefined time we will unblock it. */
                break;
            }
        }
        else
        {
            ChkDR( _UnblockPESPackets( f_pEncContext ) );
        }

        if( pECM->cPESPackets == 0 )
        {
            if( pECM != f_pEncContext->pCurrentECM || f_fClosingSession )
            {
                /*
                ** The ECM is empty, mark the packet as a bad packet in order to throw it away.
                ** This can happen. For example, there are two consecutive PMT packets, the ECM
                ** after the first PMT will not contain any PES packets.
                */
                pIncomingPacketList->pHead->ePacketState = eDRM_M2TS_PACKET_STATE_BAD;
                pIncomingPacketList->pHead->pECMData = NULL;
                ChkDR( DRM_M2TS_CleanupECM( f_pEncContext, pECM ) );

                if (f_fClosingSession)
                {
                    /* Set fEncrypted to true so it will continue to encrypt the rest of the packets */
                    fEncrypted = TRUE;
                }

                continue;
            }
            else
            {
                /* The ECM is the current ECM and there may be more PES packets to come. */
                break;
            }
        }

        if( f_fClosingSession )
        {
            /* Transfer the data from stream to the PES in the ECM that is still in the "begin" state */
            for( iPES = 0; iPES < pECM->cPESPackets; iPES++ )
            {
                DRM_M2TS_STREAM *pStream;
                DRM_M2TS_PES    *pPES           = &pECM->rgPESPackets[ iPES ];

                if( pPES->ePESState == eDRM_M2TS_PES_STATE_BEGIN )
                {
                    ChkDR( DRM_M2TS_GetStream(  f_pEncContext, pPES->wPID, &pStream ) );

                    DRMASSERT( pStream->pCurrentPES == pPES );

                    pPES->qwPTS                 = pStream->qwPTS;
                    pPES->cbPESPacketLength     = pStream->cbPESPacketLength;
                    pPES->cbHeader              = pStream->cbHeader;
                    pPES->bDataAlignmentFlag    = pStream->bDataAlignmentFlag;
                    pPES->ePESState             = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;
                    pPES->oPacketList           = pStream->oPacketList;

                    DRM_M2TS_INIT_LIST( pStream->oPacketList );
                    pStream->pLastPES           = pPES;
                    pStream->pCurrentPES        = NULL;
                    pStream->bDataAlignmentFlag = DRM_M2TS_PES_DATA_ALIGNED;

                    ChkDR( DRM_M2TS_CheckLastPESContinuityCounter( pStream ) );
                }
            }
        }
        else
        {
            /* Check the PES packets for the ECM see if all of them have complete payload */
            for( iPES = 0; iPES < pECM->cPESPackets; iPES++ )
            {
                if( pECM->rgPESPackets[ iPES ].ePESState != eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD
                 || pECM->rgPESPackets[ iPES ].fNeedRealignment )
                {
                    fReadyToEncrypt = FALSE;
                    break;
                }
            }
        }

        if( fReadyToEncrypt )
        {
            if( !pECM->fSubsampleMappingsCompleted )
            {
                ChkDR( DRM_M2TS_BuildSubsamplesMappings( f_pEncContext, pECM ) );
                pECM->fSubsampleMappingsCompleted = TRUE;
            }

            if( !pECM->fEncrypted )
            {
                ChkDR( DRM_M2TS_EncryptPESPackets( f_pEncContext, pECM ) );
                pECM->fEncrypted = TRUE;
            }

            DRM_M2TS_INIT_LIST( ecmPacketList );

            DRMASSERT( pIncomingPacketList->pHead->pECMData != NULL );
            ChkDR( DRM_M2TS_BuildECMPackets( f_pEncContext, pECM, &ecmPacketList ) );

            /* Remove the place holder packet and put it back to free packet list */
            DRM_M2TS_POP_FROM_TOP_LIST( pIncomingPacketList,
                                        DRM_M2TS_PACKET,
                                        pECMPlaceHolderPacket );
            DRM_M2TS_PUSH_TO_TOP_LIST(  &f_pEncContext->oFreePacketList,
                                        pECMPlaceHolderPacket );

            if( ecmPacketList.cItems > 0 )
            {
                /* Insert the packets on the ECM packet list to the top of the incoming packet list */
                DRM_M2TS_ADD_LIST_TO_TOP_LIST( pIncomingPacketList,
                                               DRM_M2TS_PACKET,
                                               &ecmPacketList );
                DRM_M2TS_INIT_LIST( ecmPacketList );
            }

            /*
            ** Since we have encrypted the samples and built the ECM packets,
            ** there is no need to keep the ECM, so put the ECM back to free ECM list
            */
            ChkDR( DRM_M2TS_CleanupECM( f_pEncContext, pECM ) );

            fEncrypted = TRUE;
        }
        else
        {
            fEncrypted = FALSE;
        }

    } while( fEncrypted );

ErrorExit:
    if( DRM_FAILED( dr )
     && ecmPacketList.cItems > 0 )
    {
        /* Failed to create the ECM packets, recycle them back to the free packet list */
        DRM_M2TS_ADD_LIST_TO_TOP_LIST_NO_FAIL( &f_pEncContext->oFreePacketList, DRM_M2TS_PACKET, &ecmPacketList );
    }

    return dr;
}

/**********************************************************************
**
** Function:    _SyncM2TSSyncByte
**
** Synopsis:    Sync to the next MPEG2 TS sync byte (0x47).
**
** Arguments:   
**
** [f_cbDataIn]             -- Length of the input buffer in bytes.
** [f_pbDataIn]             -- Pointer of the input buffer to search.
** [f_pdwOffset]            -- Offset to the input buffer to start the seach
**                             Upon return it contains the offset of the sync 
**                             byte's position if succeeded.
**
** Returns:                 TRUE if found the MPEG2 TS sync byte.
**                          FALSE if failed to find the MPEG2 TS sync byte.
**
**********************************************************************/
static DRM_BOOL _SyncM2TSSyncByte(
    __in                          const DRM_DWORD        f_cbDataIn,
    __in_bcount_opt(f_cbDataIn)   const DRM_BYTE        *f_pbDataIn,
    __inout                             DRM_DWORD       *f_pdwOffset )
{
    DRM_BOOL fSynced = FALSE;

    DRMASSERT( f_cbDataIn > 0 );
    DRMASSERT( f_pbDataIn != NULL );
    DRMASSERT( f_pdwOffset != NULL );
    __analysis_assume( f_cbDataIn > 0 );
    __analysis_assume( f_pbDataIn != NULL );
    __analysis_assume( f_pdwOffset != NULL );
    
    for( ; *f_pdwOffset < f_cbDataIn; (*f_pdwOffset)++ )
    {
        if( f_pbDataIn[ *f_pdwOffset ] == DRM_M2TS_START_CODE )
        {
            /* 
            ** Found a possible MPEG2 TS sync byte.  This could be just a random byte
            ** that happens to be 0x47.  If that's the case we will try again next time
            ** which may be another random 0x47 but eventually we will be able to sync
            ** after a few packets.
            */
            fSynced = TRUE;
            break;
        }
    }

    return fSynced;
}

/**********************************************************************
**
** Function:    _CacheIncomingPacketData
**
** Synopsis:    Copy the data to the cached packet list
**
** Arguments:   
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_cbDataIn]             -- Length of the input buffer in bytes
** [f_pbDataIn]             -- Pointer to the input buffer to cache
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CacheIncomingPacketData(
    __inout                             DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in                          const DRM_DWORD                    f_cbDataIn,
    __in_bcount_opt(f_cbDataIn)   const DRM_BYTE                    *f_pbDataIn ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CacheIncomingPacketData(
    __inout                             DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in                          const DRM_DWORD                    f_cbDataIn,
    __in_bcount_opt(f_cbDataIn)   const DRM_BYTE                    *f_pbDataIn )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_M2TS_PACKET *pPacket    = NULL;

    DRMASSERT( f_pEncContext != NULL );
    __analysis_assume( f_pEncContext != NULL );
    ChkArg( ( f_cbDataIn > 0 ) == ( f_pbDataIn != NULL ) );
    ChkArg( f_pEncContext->bOffsetCachedLastIncompletedPacket < DRM_M2TS_PACKET_SIZE );

    ChkBOOL( f_pEncContext->drLastEncryption != DRM_S_MORE_DATA || f_cbDataIn == 0, DRM_E_M2TS_GET_ENCRYPTED_DATA_FIRST );

    if( f_cbDataIn > 0 )
    {
        DRM_DWORD   dwOffset    = 0;
        DRM_DWORD   cbCopy      = 0;

        /* Fill the last incompleted packet first and create a new pakcet object when the packet is filled */
        if( f_pEncContext->bOffsetCachedLastIncompletedPacket > 0 )
        {
            DRM_BYTE bcbCopy = 0;
            cbCopy = min( f_cbDataIn, (DRM_DWORD)(DRM_M2TS_PACKET_SIZE - f_pEncContext->bOffsetCachedLastIncompletedPacket) );

            MEMCPY( f_pEncContext->rgbCachedLastIncompletedPacket + f_pEncContext->bOffsetCachedLastIncompletedPacket, f_pbDataIn, cbCopy );
            dwOffset += cbCopy;
            ChkDR( DRM_DWordToByte( cbCopy, &bcbCopy ) );
            ChkDR( DRM_ByteAdd( f_pEncContext->bOffsetCachedLastIncompletedPacket, bcbCopy, &f_pEncContext->bOffsetCachedLastIncompletedPacket ) );

            DRMASSERT( f_pEncContext->bOffsetCachedLastIncompletedPacket <= DRM_M2TS_PACKET_SIZE );

            if( f_pEncContext->bOffsetCachedLastIncompletedPacket == DRM_M2TS_PACKET_SIZE )
            {
                f_pEncContext->dwNextPacketNo++;

                ChkDR( DRM_M2TS_GetFreePacket( f_pEncContext,
                                               0,       /* f_cbPayload */
                                               eDRM_M2TS_PACKET_STATE_PENDING,
                                               TRUE,    /* f_fIncludePacketNumber */
                                               f_pEncContext->rgbCachedLastIncompletedPacket,
                                               &pPacket ) );
                DRM_M2TS_APPEND_TO_END_OF_LIST( &f_pEncContext->oCachedPacketList, pPacket );
                pPacket = NULL;
                f_pEncContext->bOffsetCachedLastIncompletedPacket = 0;
            }
        }

        /* Create a new pakcet object for each 188-byte unit in input data */
        while( f_cbDataIn - dwOffset >= DRM_M2TS_PACKET_SIZE )
        {
            if( f_pbDataIn[dwOffset] != DRM_M2TS_START_CODE
             && !_SyncM2TSSyncByte( f_cbDataIn, f_pbDataIn, &dwOffset ) )
            {
                break;
            }

            f_pEncContext->dwNextPacketNo++;

            ChkDR( DRM_M2TS_GetFreePacket( f_pEncContext,
                                           0,       /* f_cbPayload */
                                           eDRM_M2TS_PACKET_STATE_PENDING,
                                           TRUE,    /* f_fIncludePacketNumber */
                                           f_pbDataIn + dwOffset,
                                           &pPacket ) );
            DRM_M2TS_APPEND_TO_END_OF_LIST( &f_pEncContext->oCachedPacketList, pPacket );
            pPacket = NULL;
            ChkDR( DRM_DWordAdd( dwOffset, DRM_M2TS_PACKET_SIZE, &dwOffset ) );
        }

        /* Save the remaining in the input data */
        if( f_cbDataIn > dwOffset )
        {
            DRM_BOOL fSynced = TRUE;

            if( f_pbDataIn[dwOffset] != DRM_M2TS_START_CODE )
            {
                fSynced = _SyncM2TSSyncByte( f_cbDataIn, f_pbDataIn, &dwOffset );
            }

            if( fSynced )
            {
                cbCopy = f_cbDataIn - dwOffset;
                DRMASSERT( cbCopy < DRM_M2TS_PACKET_SIZE );
                MEMCPY( f_pEncContext->rgbCachedLastIncompletedPacket, f_pbDataIn + dwOffset, cbCopy );
                ChkDR( DRM_DWordToByte( cbCopy, &f_pEncContext->bOffsetCachedLastIncompletedPacket ) );
            }
        }
    }

ErrorExit:
    if( pPacket != NULL )
    {
        /* Something went wrong, recycle the packet */
        DRMASSERT( DRM_FAILED( dr ) );
        DRM_M2TS_PUSH_TO_TOP_LIST_NO_FAIL( &f_pEncContext->oFreePacketList, pPacket );
    }
    return dr;
}

/**********************************************************************
**
** Function:    _ProcessPacket
**
** Synopsis:    Process the packet
**
** Arguments:   
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pStream]              -- Pointer to a stream of the packet to be processed.
** [f_pPacket]              -- Pointer to a pakcet to process.
** [f_fUnitStart]           -- Flag indicate whether the packet is a unit start.
** [f_pfPESCompleted]       -- Out parameter to contain a flag to indicate
**                             a PES is completed.
** [f_pfPacketCommitted]    -- Out parameter to contain a flag to indicate
**                             whether the packet has been committed (added to
**                             incoming packet list.
** [f_cbDataOut]            -- Length of the output buffer in bytes
** [f_pbDataOut]            -- Pointer to the output buffer to store the encypted/
**                             processed packets
** [f_pcbReturned]          -- Out parameter to contain the byte count of the 
**                             returned packets.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ProcessPacket(
    __inout                             DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __inout                             DRM_M2TS_STREAM             *f_pStream,
    __inout                             DRM_M2TS_PACKET             *f_pPacket,
    __in                          const DRM_BOOL                     f_fUnitStart,
    __out                               DRM_BOOL                    *f_pfPESCompleted,
    __out                               DRM_BOOL                    *f_pfPacketCommitted,
    __out_bcount(*f_pcbDataOut)         DRM_BYTE                    *f_pbDataOut,
    __in                          const DRM_DWORD                   *f_pcbDataOut,
    __inout                             DRM_DWORD                   *f_pcbReturned ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ProcessPacket(
    __inout                             DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __inout                             DRM_M2TS_STREAM             *f_pStream,
    __inout                             DRM_M2TS_PACKET             *f_pPacket,
    __in                          const DRM_BOOL                     f_fUnitStart,
    __out                               DRM_BOOL                    *f_pfPESCompleted,
    __out                               DRM_BOOL                    *f_pfPacketCommitted,
    __out_bcount(*f_pcbDataOut)         DRM_BYTE                    *f_pbDataOut,
    __in                          const DRM_DWORD                   *f_pcbDataOut,
    __inout                             DRM_DWORD                   *f_pcbReturned )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pStream != NULL );
    DRMASSERT( f_pPacket != NULL );
    DRMASSERT( f_pfPESCompleted != NULL );
    DRMASSERT( f_pfPacketCommitted != NULL );
    DRMASSERT( f_pbDataOut != NULL );
    DRMASSERT( f_pcbDataOut != NULL && *f_pcbDataOut > 0 );
    DRMASSERT( f_pcbReturned != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pStream != NULL );
    __analysis_assume( f_pPacket != NULL );
    __analysis_assume( f_pfPESCompleted != NULL );
    __analysis_assume( f_pfPacketCommitted != NULL );
    __analysis_assume( f_pbDataOut != NULL );
    __analysis_assume( f_pcbDataOut != NULL && *f_pcbDataOut > 0 );
    __analysis_assume( f_pcbReturned != NULL );

    *f_pfPESCompleted    = FALSE;
    *f_pfPacketCommitted = FALSE;

    if( f_fUnitStart )
    {
        if( DRM_M2TS_IS_PES( f_pStream->ePacketType ) )
        {
            /*
            ** A new PES just arrived, transfer data from stream to the current PES and register
            ** the new PES to current ECM or a new ECM if the current ECM can't take it
            */
            ChkDR( DRM_M2TS_RegisterAndTransferPES( f_pEncContext, f_pStream, FALSE /* stream stopped */ ) );
        }
        else
        {
            /*
            ** A new PAT or PMT unit just arrived, make sure that the last PAT or PMT is in right state.
            ** Discard the PAT or PMT packets of the last PAT or PMT if they are not in
            ** eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD state.
            */
            if( f_pStream->ePESState != eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD )
            {
                DRM_M2TS_PACKET *pPATOrPMTPacket = f_pStream->oPacketList.pHead;

                /* Mark every packet on the list as bad and log an event */
                while( pPATOrPMTPacket != NULL )
                {
                    (DRM_VOID)DRM_M2TS_LogEventWithPacket( f_pEncContext, eDRM_M2TS_LOG_CATEGORY_WARNING, TRUE, DRM_E_M2TS_INCOMPLETE_SECTION_HEADER, pPATOrPMTPacket );
                    pPATOrPMTPacket->ePacketState = eDRM_M2TS_PACKET_STATE_BAD;
                    pPATOrPMTPacket = pPATOrPMTPacket->pNextPerUnit;
                }
            }

            /* Reset the PAT/PMT stream to process the new one */
            f_pStream->cbHeader = 0;
            f_pStream->ePESState = eDRM_M2TS_PES_STATE_BEGIN;
            DRM_M2TS_INIT_LIST( f_pStream->oPacketList );
        }
    }

    /* Parse the (section) header of PAT, PMT or PES */
    if( f_pStream->ePESState == eDRM_M2TS_PES_STATE_BEGIN )
    {
        ChkDR( DRM_M2TS_ParsePacketHeader( f_pEncContext, f_pStream, f_pPacket ) );
    }

    /* Append current packet to the stream's per unit packet list */
    DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &f_pStream->oPacketList, f_pPacket );

    /* Commit the packet to the incoming packet list */
    DRM_M2TS_APPEND_TO_END_OF_LIST( &f_pEncContext->oIncomingPacketList, f_pPacket );
    *f_pfPacketCommitted = TRUE;

    if( f_fUnitStart )
    {
        if( !f_pStream->fFoundFirstUnitStart )
        {
            /* Found the first packet in this stream that is unit start */
            f_pStream->fFoundFirstUnitStart = TRUE;
        }

        if( DRM_M2TS_IS_VIDEO_STREAM( f_pStream->bStreamType )
         && f_pStream->pLastPES != NULL
         && f_pStream->pLastPES->ePESState == eDRM_M2TS_PES_STATE_HAS_SECTION_HEADER
         && !DRM_M2TS_HasPayload( f_pStream->pLastPES ) )
        {
            /*
            ** The last PES  doesn't have its own payload.  See if the start code is split into
            **  two  parts, one in the last packet of the last PES and the other in the first packet
            ** of the current PES, that we weren't able to know.  Now, we have the next packet
            ** we should try it now.
            **
            ** Because of the way the audio and video are parsed, this case can only happen to
            ** video PES. Audio sample is parsed using the frame size contained inside the
            ** frame header, but the video sample is parsed by searching the start code.
            */
            ChkDR( _RecheckLastPESPayload( f_pStream ) );
        }
    }

    /* Check if the previous PES packet is completed */
    if( f_pStream->pLastPES != NULL
     && f_pStream->ePESState == eDRM_M2TS_PES_STATE_HAS_SECTION_HEADER )
    {
        /* PAT and PMT packets always skip this state, so this packet must belong to a PES */
        DRMASSERT( DRM_M2TS_IS_PES( f_pStream->ePacketType ) );

        ChkDR( DRM_M2TS_CheckPESCompleteness( f_pEncContext, f_pStream, f_pfPESCompleted ) );
    }

    /* Add CA_descryptor for our ECM and remove other type of CA_descriptor */
    if( f_pStream->ePESState == eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD
     && f_pStream->ePacketType == eDRM_M2TS_PACKET_TYPE_PMT )
    {
        ChkDR( DRM_M2TS_RebuildPMTPacket( f_pEncContext, f_pStream ) );

        /* A primary ECM packet is needed right after the PMT packet */
        ChkDR( DRM_M2TS_InsertECM( f_pEncContext, eDRM_M2TS_ECM_TYPE_PRIMARY ) );
    }

    /* A PES just completed, see if we can encrypt now and return the encrypted packets back */
    if( f_pStream != NULL
     && DRM_M2TS_IS_PES( f_pStream->ePacketType )
     && *f_pfPESCompleted )
    {
        ChkDR( _EncryptPESPackets( f_pEncContext,
                                   FALSE,           /* closing session */
                                   *f_pcbDataOut,
                                   f_pbDataOut,
                                   f_pcbReturned ) );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    Drm_M2ts_Encrypt
**
** Synopsis:    Encrypt the TS packets contained in f_pbDataIn and
**              store the encrypted TS packets in f_pbDataOut.
**              The caller should call this function again when the
**              result is DRM_S_MORE_DATA to get all the encrypted TS
**              packets without passing additional TS packets for encryption.
**              The caller should notify this function when there are
**              no additional TS packets to encrypt by setting
**              f_fLastTSPackets to TRUE.  The encryptor will encrypt
**              and flush out all existing PES packets to the caller.
**              The call should call Drm_m2ts_SetKeyData to set a new key
**              when the result is DRM_E_M2TS_NEED_KEY_DATA.  This is
**              to notify the caller that it is time to rotate the key.
**
** Arguments:   
**
** [f_hEncryptor]           -- The encryptor handle
** [f_cbDataIn]             -- The number of bytes in f_pbDataIn to
**                             encrypt.  The caller should pass 0
**                             if the result of last call is 
**                             DRM_S_MORE_DATA.
** [f_pbDataIn]             -- Contain the TS packets to be encrypted.
**                             The first f_pbDataIn passed must start on 
**                             the TS packet boundary.  The subsequent 
**                             f_pbDataIn is expected to be a continuation
**                             of previous one.  The caller should pass 
**                             NULL if the result of last call is 
**                             DRM_S_MORE_DATA.
** [f_pcbDataOut]           -- The number of bytes encrypted and 
**                             returned in f_pbDataOut
** [f_pbDataOut]            -- Contains the encrypted TS packets
** [f_fLastTSPackets]       -- TRUE/FALSE flag to indicate whether
**                             this is the last bacth of TS packets.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_CONTEXT_NOT_INITIALIZED if the 
**                              encryptor handle hasn't been initialized
**                          DRM_S_MORE_DATA when there are more data to
**                              return.
**                          DRM_E_M2TS_NEED_KEY_DATA when it is time to
**                              rotate to next key or the caller has set
**                              the initial key data.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_M2ts_Encrypt(
    __in                                DRM_M2TS_ENCRYPTOR_HANDLE    f_hEncryptor,
    __in_bcount_opt(f_cbDataIn)   const DRM_BYTE                    *f_pbDataIn,
    __in                          const DRM_DWORD                    f_cbDataIn,
    __out_bcount(*f_pcbDataOut)         DRM_BYTE                    *f_pbDataOut,
    __inout                             DRM_DWORD                   *f_pcbDataOut,
    __in                          const DRM_BOOL                     f_fLastTSPackets )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_CONTEXT  *pEncContext            = ( DRM_M2TS_ENCRYPTOR_CONTEXT * )f_hEncryptor;
    DRM_M2TS_STREAM             *pStream                = NULL;
    DRM_M2TS_PACKET             *pPacket                = NULL;
    DRM_DWORD                    cbReturned             = 0;
    DRM_WORD                     wPID                   = DRM_M2TS_UNDEFINED_PID;
    DRM_BYTE                     bContinuityCounter     = 0;
    DRM_BOOL                     fUnitStart             = FALSE;
    DRM_BOOL                     fPESCompleted          = FALSE;
    DRM_BOOL                     fPacketCommitted       = FALSE;

    CHECK_ENCRYPTION_CONTEXT_INITIALIZED( pEncContext );

    ChkArg( f_pcbDataOut != NULL );
    ChkArg( *f_pcbDataOut > 0 );
    ChkArg( f_pbDataOut != NULL );

    /* Since we don't return partial packet back, trim *f_pcbDataOut if it is not a multiplication of 188 */
    *f_pcbDataOut = ( *f_pcbDataOut / DRM_M2TS_PACKET_SIZE ) * DRM_M2TS_PACKET_SIZE;
    ChkArg( *f_pcbDataOut >= DRM_M2TS_PACKET_SIZE );

    pEncContext->fEncryptionStarted = TRUE;

    if( f_fLastTSPackets )
    {
        pEncContext->fClosingSession = TRUE;
    }

    ChkDR( _CacheIncomingPacketData( pEncContext, f_cbDataIn, f_pbDataIn ) );
#if DBG
    DRM_M2TS_VERIFY_LINKED_LIST_LENGTH( &pEncContext->oCachedPacketList, DRM_M2TS_PACKET );
#endif /* DBG */

    /* Need (next) encryption key before we can continue */
    ChkBOOL( !pEncContext->fWaitingForNewKey, DRM_E_M2TS_NEED_KEY_DATA );

    /* Encrypt and copy all the processed/encrypted packet to the output buffer */
    ChkDR( _EncryptPESPackets( pEncContext,
                               FALSE,           /* closing session */
                               *f_pcbDataOut,
                               f_pbDataOut,
                               &cbReturned ) );

    while( pEncContext->oCachedPacketList.cItems > 0  )
    {
        DRM_M2TS_POP_FROM_TOP_LIST( &pEncContext->oCachedPacketList, DRM_M2TS_PACKET, pPacket );

        /* Get the PID and figure out the payload size of the TS packet */
        dr = DRM_M2TS_ParseTSPacketHeader( pPacket,
                                           &wPID,
                                           &fUnitStart,
                                           &bContinuityCounter );
        if( DRM_FAILED( dr )
         && DRM_E_M2TS_DROP_PACKET == DRM_M2TS_TranslateError( dr ) )
        {
            (DRM_VOID)DRM_M2TS_LogEventWithPacket( pEncContext, eDRM_M2TS_LOG_CATEGORY_WARNING, TRUE, dr, pPacket );
            dr = DRM_SUCCESS;

            /* Recycle the packet to free packet list */
            DRM_M2TS_PUSH_TO_TOP_LIST( &pEncContext->oFreePacketList, pPacket );
            pPacket = NULL;
        }
        else
        {
            ChkDR( dr );

            if( pEncContext->oRemovablePIDList.cItems > 0
             && _CanPIDBeRemoved( pEncContext, wPID ) )
            {
                /* This is an ECM packet of the original CA_descriptor that we can throw away */
                DRM_M2TS_PUSH_TO_TOP_LIST( &pEncContext->oFreePacketList, pPacket );
                pPacket = NULL;
            }
            else
            {
                dr = DRM_M2TS_GetStream( pEncContext, wPID, &pStream );
                if( dr == DRM_E_NOT_FOUND )
                {
                    if( pEncContext->oStreamList.cItems <= 2 )
                    {
                        dr = DRM_SUCCESS;

                        /*
                        ** This packet must be before the first PAT and PMT because we have less than or equal to two stream data.
                        ** PAT is added at beginning and PMT is added after the PAT is parsed which means the PMT hasn't arrived
                        ** yet.  This packet is not PMT and should be dropped by not placing it on the incoming packet list.
                        */

                        (DRM_VOID)DRM_M2TS_LogEventWithPacket( pEncContext, eDRM_M2TS_LOG_CATEGORY_WARNING, TRUE, DRM_E_M2TS_UNKNOWN_PACKET, pPacket );

                        /* Put the packet back to the free packet list */
                        DRM_M2TS_PUSH_TO_TOP_LIST( &pEncContext->oFreePacketList, pPacket );
                    }
                    else
                    {
                        dr = DRM_SUCCESS;

                        /*
                        ** Not the type of packet that we care about
                        ** Just put it to the incoming packet list and mark it as ready to return it to caller later.
                        */
                        pPacket->ePacketState = eDRM_M2TS_PACKET_STATE_READY;
                        DRM_M2TS_APPEND_TO_END_OF_LIST( &pEncContext->oIncomingPacketList, pPacket );
                    }

                    pPacket = NULL;
                }
                else if ( dr == DRM_SUCCESS
                       && !fUnitStart
                       && ( !pStream->fFoundFirstUnitStart
                         || pStream->ePESState == eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD ) )
                {
                    /*
                    ** This packet appears after the PMT but we haven't found the unit start packet for that PID yet which means
                    ** this packet is just a continuation packet of an unit start packet that appears before the PMT.
                    ** Or, a packet that is not an unit start but the encryptor is expecting an unit start packet.
                    */

                    (DRM_VOID)DRM_M2TS_LogEventWithPacket( pEncContext, eDRM_M2TS_LOG_CATEGORY_WARNING, TRUE, DRM_E_M2TS_NOT_UNIT_START_PACKET, pPacket );

                    /* Put the packet back to the free packet list */
                    DRM_M2TS_PUSH_TO_TOP_LIST( &pEncContext->oFreePacketList, pPacket );

                    pPacket = NULL;
                }
                else
                {
                    /* This packet is either a PAT, PMT or PES that we need to encrypt. */
                    ChkDR( dr );
                    DRMASSERT( pStream != NULL );

                    dr = _ProcessPacket( pEncContext,
                                         pStream,
                                         pPacket,
                                         fUnitStart,
                                         &fPESCompleted,
                                         &fPacketCommitted,
                                         f_pbDataOut,
                                         f_pcbDataOut,
                                         &cbReturned );
                    if( DRM_FAILED( dr ) )
                    {
                        DRM_RESULT          drSaved      = dr;
                        DRM_RESULT          drTranslated = DRM_M2TS_TranslateError( drSaved );
                        DRM_M2TS_PACKET    *pPacketSaved = pPacket;

                        if( !fPacketCommitted )
                        {
                            /* Put the packet back to the free packet list */
                            DRM_M2TS_PUSH_TO_TOP_LIST( &pEncContext->oFreePacketList, pPacket );
                        }
                        pPacket = NULL;

                        if( drTranslated == DRM_E_M2TS_DROP_PACKET )
                        {
                            DRMASSERT( !fPacketCommitted );

                            (DRM_VOID)DRM_M2TS_LogEventWithPacket( pEncContext, eDRM_M2TS_LOG_CATEGORY_WARNING, TRUE, drSaved, pPacketSaved );

                            /* Just in case the packet was committed */
                            pPacketSaved->ePacketState = eDRM_M2TS_PACKET_STATE_BAD;

                            if( pStream != NULL )
                            {
                                /* Reset the stream's state because this PES becomes useless */
                                ChkDR( _ResetStream( pStream ) );
                            }
                        }
                        else if( drTranslated == DRM_E_M2TS_DROP_PES )
                        {
                            /* ECM Build should have handled all the errors mapped to DRM_E_M2TS_DROP_PES */
                            DRMASSERT( FALSE );
                        }
                        else
                        {
                            ChkDR( dr );
                        }

                        dr = DRM_SUCCESS;
                    }
                    else
                    {
                        DRMASSERT( fPacketCommitted );
                        pPacket = NULL;
                    }

                    /* Is time to rotate the key? */
                    if( fPESCompleted
                     && DRM_UI64Les( DRM_UI64HL( 0, 0 ), pEncContext->qwCurrentKeyDuration ) )
                    {
                        /* Search each stream see if the aggregated duration is greater than current key's duration */
                        pStream = pEncContext->oStreamList.pHead;
                        while( pStream != NULL )
                        {
                            if( !DRM_UI64Les( pStream->qwAggregatedDuration, pEncContext->qwCurrentKeyDuration ) )
                            {
                                (DRM_VOID)DRM_M2TS_LogEventWithKeyRotation( pEncContext, pStream->qwPTS, pStream->qwAggregatedDuration, pEncContext->qwCurrentKeyDuration );

                                pEncContext->fWaitingForNewKey = TRUE;
                                ChkDR( DRM_E_M2TS_NEED_KEY_DATA );
                            }

                            pStream = pStream->pNext;
                        }
                    }
                }

                DRMASSERT( pEncContext->cActualPacketsAllocated == pEncContext->oFreePacketList.cItems + pEncContext->oIncomingPacketList.cItems + pEncContext->oCachedPacketList.cItems );
            }
        }
    }

    /*
    ** The caller has signaled that this is the last batch of TS packets
    ** and all of them have been processed, so mark every PES packets as completed
    ** in order to encrypt and return the remaining PES packets back to caller.
    */
    if( pEncContext->fClosingSession )
    {
        ChkDR( _EncryptPESPackets( pEncContext,
                                   TRUE,            /* closing session */
                                   *f_pcbDataOut,
                                   f_pbDataOut,
                                   &cbReturned ) );
    }

ErrorExit:

    if( f_pcbDataOut != NULL )
    {
        *f_pcbDataOut = cbReturned;
    }

    if( dr == DRM_E_M2TS_OUTPUT_BUFFER_FULL )
    {
        dr = DRM_S_MORE_DATA;
    }

    if( pPacket != NULL )
    {
        DRMASSERT( DRM_FAILED( dr ) );

        /* Put the packet back to the free packet list */
        DRM_M2TS_PUSH_TO_TOP_LIST_NO_FAIL( &pEncContext->oFreePacketList, pPacket );
    }

#if DBG
    _PrintDebugTrace( pEncContext, dr, pPacket, pStream );
#endif /* DBG */

    if( pEncContext != NULL )
    {
        pEncContext->drLastEncryption = dr;
    }

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
