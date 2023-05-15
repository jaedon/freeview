/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMM2TSECMBUILDER_C
#include <drmmanager.h>
#include <drmconstants.h>
#include <drmutilities.h>
#include <drmm2tsbuilders.h>
#include <drmm2tsbuffermgr.h>
#include <drmm2tsparsers.h>
#include <drmm2tsmacros.h>
#include <drmm2tslogs.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define DRM_M2TS_ENCRYPTED_SEGMENTS_INCREMENT               200     /* Number of elements to increase each time the encrypted segements buffer is full */
#define DRM_M2TS_LENGTH_OF_PER_STREAM_FIELDS                23      /* include scrambling_control(2), streamParamsIncluded(1), PID(13), AlgorithmID(24), IV_size(8), KID(128), PES_count(8) */
#define DRM_M2TS_LENGTH_OF_FIELDS_BEFORE_ECM_SECLEN_FIELD   6       /* include 6 bytes for packet_start_code_prefix(24), stream_id(8) and pes_packet_length(16) fields */

/* ECM Format
**   packet_start_code_prefix (24) 0x000001
**   stream_id (8) 0xf0
**   pes_packet_length (16)
**   ecm_version (4) 0x1
**   pssh_count (4)
**   for(i=0; i < pssh_count; i++) {
**     (PSSH format)
**     SystemID (128) PlayReady protection system: 9A04F079-9840-4286-AB92-E65BE0885F95.
**     DataSize (32) Size of the Protection System Specific data in bytes.
**     Data (8*DataSize) Protection System Specific
**   }
**   scrambled_stream_count (8)
**   for(i=0; i < scrambled_stream_count; i++) {
**     (SENC data)
**     scrambling_control (2)
**     streamParamsIncluded (1)
**     wPID (13) The wPID of the scrambled ES
**     if(streamParamsIncluded) {
**       AlgorithmID (24)
**       IV_size (8)
**       KID (128)
**     }
**     PES_count (8)
**     for(int p = 0; p < PES_count; p++) {
**       PTS_timestamp (33)
**       sample_count (7)
**       for(i=0; i < sample_count; i++) {
**         InitializationVector (8*IV_size)
**         subsamples_count (16)
**         for(x=0; x < subsample_count; x++) {
**           BytesOfClearData (16)
**           BytesOfEncryptedData (32)
**         }
**       }
**     }
**   }
*/

#define ECM_HEADER                      { 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x10 }        /* prefix=0x000001, streamID=0xF0, packetLen=0x0000, version=0x1, psshCount=0x0 */
#define IV_SIZE                         8

/**********************************************************************
**
** Function:   DRM_M2TS_InsertECM
**
** Synopsis:   Creates a new ECM as the current ECM and adds a place
**             holder packet on the incoming packet list.  The place
**             holder packet will be replaced with the ECM packets
**             later.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_eECMType]             -- The type of ECM to be inserted in the incoming 
**                             packet list as a place holder.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_InsertECM(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __in      const DRM_M2TS_ECM_TYPE                f_eECMType )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_M2TS_PACKET    *pPlaceHolder    = NULL;
    DRM_M2TS_ECM       *pECM            = NULL;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_eECMType == eDRM_M2TS_ECM_TYPE_PRIMARY
         || f_eECMType == eDRM_M2TS_ECM_TYPE_SECONDARY
         || f_eECMType == eDRM_M2TS_ECM_TYPE_KR );

    ChkDR( DRM_M2TS_GetFreeECM( f_pEncContext, f_eECMType, &pECM ) );
    ChkDR( DRM_M2TS_GetFreePacket(  f_pEncContext,
                                    0,
                                    eDRM_M2TS_PACKET_STATE_PENDING,
                                    FALSE,  /* assign packet no. */
                                    NULL,
                                    &pPlaceHolder ) );

    f_pEncContext->pCurrentECM = pECM;
    pPlaceHolder->pECMData = pECM; /* to link to the ECM object, so we know which ECM to be placed here */
    pECM = NULL;

    DRM_M2TS_APPEND_TO_END_OF_LIST( &f_pEncContext->oIncomingPacketList, pPlaceHolder );

ErrorExit:
    if( pECM != NULL )
    {
        /* Put the ECM back to the free ECM list */
        DRM_M2TS_CleanupECM( f_pEncContext, pECM );
    }
    return dr;
}

/**********************************************************************
**
** Function:   DRM_M2TS_CleanupECM
**
** Synopsis:   Closes the license handle in the ECM and puts the ECM back to
**             the free ECM list.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pECM]                 -- An DRM_M2TS_ECM object to be cleaned up.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_CleanupECM(
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_ECM                    *f_pECM )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pECM != NULL );

    if( f_pECM->hLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        ChkDR( Drm_LocalLicense_Release( &f_pECM->hLicense ) );
        f_pECM->hLicense = DRM_LICENSE_HANDLE_INVALID;
    }

    Drm_Prnd_MemFree( f_pECM->pbPROBlob );
    f_pECM->pbPROBlob = NULL;
    f_pECM->cbPROBlob = 0;

    DRM_M2TS_PUSH_TO_TOP_LIST( &f_pEncContext->oFreeECMList, f_pECM );

ErrorExit:
   return dr;
}


/**********************************************************************
**
** Function:    DRM_M2TS_RegisterAndTransferPES
**
** Synopsis:    Transfers data from stream to current PES and registers
**              a new PES to the current ECM.  If current ECM already has
**              max number of PES allowed, or the new PES is video and
**              current ECM already has one video PES then create a new ECM
**              as the current ECM.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pStream]              -- Pointer to the stream that a new PES just arrived
** [f_fStreamStoppped]      -- Flag to indicate whether the stream has stopped
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_INTERNAL_ERROR when something unexpected
**                              happened
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_RegisterAndTransferPES(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_STREAM                 *f_pStream,
    __in      const DRM_BOOL                         f_fStreamStoppped )
{
    DRM_RESULT       dr                     = DRM_SUCCESS;
    DRM_M2TS_ECM    *pCurrentECM            = NULL;
    DRM_M2TS_PES    *pPES                   = NULL;
    DRM_M2TS_PES    *pLastPESNeedAlignment  = NULL;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pStream != NULL );

    if( f_pStream->pLastPES != NULL
     && f_pStream->pLastPES->ePESState != eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD )
    {
        DRM_M2TS_PACKET *pPacket = NULL;

        /*
        ** The last PES hasn't reached eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD yet.
        ** This is abnormal that means the current PES doesn't have its own payload
        ** because current PES doesn't contain the start code/sync word and the next
        ** unit start packet has arrived.  If there is start code/sync word then
        ** last PES would be in eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD state already.
        ** So, lets make the entire payload of current PES as the overflow of the last PES.
        */

        /* First make sure last PES is in eDRM_M2TS_PES_STATE_HAS_SECTION_HEADER */
        if( f_pStream->pLastPES->ePESState == eDRM_M2TS_PES_STATE_HAS_SECTION_HEADER )
        {
            /* Make the payload of current PES as overflow of last PES */
            ChkBOOL( f_pStream->oPacketList.pHead != NULL && f_pStream->oPacketList.pTail != NULL, DRM_E_M2TS_INTERNAL_ERROR );
            pPacket = f_pStream->oPacketList.pHead;
            while( pPacket != NULL )
            {
                ChkDR( DRM_ByteSub( pPacket->cbPayload, pPacket->cbHeader, &pPacket->cbPayloadLastPES ) );
                pPacket = pPacket->pNextPerUnit;
            }

            /*
            ** Assume last PES' overflow ends here, DRM_M2TS_CheckPESCompleteness will check
            ** the next PES to ensure that there is no additional overflow in the next PES.
            ** DRM_M2TS_CheckPESCompleteness will return error if it does.
            */
            f_pStream->oPacketList.pTail->wPacketFlags     |= DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET;
            f_pStream->pLastPES->pHeadOfOverflowedPackets   = f_pStream->oPacketList.pHead;
            f_pStream->pLastPES->fNeedRealignment           = TRUE;
            pLastPESNeedAlignment                           = f_pStream->pLastPES;
        }
        else
        {
            ChkBOOL( f_pStream->pLastPES->ePESState == eDRM_M2TS_PES_STATE_BEGIN, DRM_E_M2TS_INTERNAL_ERROR );

            /*
            ** Last PES hasn't have the complete PES header yet that means it doesn't have payload for the PES prior
            ** to it, nor it has payload for itself. Set an error against the last PES in order to drop it later.
            */
            f_pStream->pLastPES->drPESResult = DRM_E_M2TS_INCOMPLETE_SECTION_HEADER;
            f_pStream->pLastPES->pHeadOfOverflowedPackets = NULL;
        }

        f_pStream->pLastPES->ePESState = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;
    }

    if( f_pStream->pCurrentPES != NULL
     && f_pStream->oPacketList.cItems == 0 )
    {
        /* Current PES is empty.  Most likely that the previous PES was dropped.  We can just reuse it. */
        ChkBOOL( f_pStream->ePESState == eDRM_M2TS_PES_STATE_BEGIN, DRM_E_M2TS_INTERNAL_ERROR );
        ChkBOOL( f_pStream->pLastPES == NULL, DRM_E_M2TS_INTERNAL_ERROR );
    }
    else
    {
        /* Transfer data from stream to current PES */
        if( f_pStream->pCurrentPES != NULL )
        {
            pPES = f_pStream->pCurrentPES;
            pPES->qwPTS                 = f_pStream->qwPTS;
            pPES->cbPESPacketLength     = f_pStream->cbPESPacketLength;
            pPES->cbHeader              = f_pStream->cbHeader;
            pPES->bDataAlignmentFlag    = f_pStream->bDataAlignmentFlag;
            pPES->ePESState             = f_pStream->ePESState;
            pPES->oPacketList           = f_pStream->oPacketList;
            pPES->pLastPESNeedAlignment = pLastPESNeedAlignment;
        }
        f_pStream->pLastPES = f_pStream->pCurrentPES;
        f_pStream->pCurrentPES = NULL;

        if( !f_fStreamStoppped )
        {
            /* See if current ECM can take this new PES, if not create a new ECM as the current ECM */
            pCurrentECM = f_pEncContext->pCurrentECM;
            AssertChkArg( pCurrentECM != NULL );

            if(   pCurrentECM->cPESPackets >= DRM_M2TS_MAX_PES_PACKETS_PER_ECM
             || ( pCurrentECM->fHasVideoPES && DRM_M2TS_IS_VIDEO_STREAM( f_pStream->bStreamType ) ) )
            {
                ChkDR( DRM_M2TS_InsertECM( f_pEncContext, eDRM_M2TS_ECM_TYPE_SECONDARY ) );
            }

            pCurrentECM = f_pEncContext->pCurrentECM;
            if( DRM_M2TS_IS_VIDEO_STREAM( f_pStream->bStreamType ) )
            {
                pCurrentECM->fHasVideoPES = TRUE;
            }

            /* Initialize the new PES */

            pPES = &pCurrentECM->rgPESPackets[ pCurrentECM->cPESPackets++ ];
            f_pStream->pCurrentPES = pPES;

            MEMSET( pPES, 0 , SIZEOF( DRM_M2TS_PES ) );

            pPES->ePESState                 = eDRM_M2TS_PES_STATE_BEGIN;
            pPES->wPID                      = f_pStream->wPID;
            pPES->bStreamType               = f_pStream->bStreamType;
            pPES->fHasSample                = FALSE;
            pPES->drPESResult               = DRM_SUCCESS;
        }
        else
        {
            /*
            ** Since the stream has stopped and we are not sure whether the PES packet is completed
            ** let's drop the PES packet to be safe.
            */
            if( f_pStream->pLastPES != NULL )
            {
                if( f_pStream->pLastPES->ePESState != eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD )
                {
                    f_pStream->pLastPES->drPESResult = DRM_E_M2TS_WAITED_TOO_LONG;
                }
                f_pStream->pLastPES->ePESState = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;
                f_pStream->pLastPES = NULL;
            }
        }
    }

    /* Initialize stream for the new PES */
    f_pStream->qwPTS                = DRM_UI64HL( 0, 0 );
    f_pStream->cbPESPacketLength    = 0;
    f_pStream->cbHeader             = 0;
    f_pStream->bDataAlignmentFlag   = DRM_M2TS_PES_DATA_ALIGNED;
    f_pStream->ePESState            = eDRM_M2TS_PES_STATE_BEGIN;
    DRM_M2TS_INIT_LIST( f_pStream->oPacketList );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   _FindLastOverflowPacket
**
** Synopsis:   Find the last packet that contains the overflow payload
**             of last PES
**
** Arguments:
**
** [f_pPacketList]          -- Pointer to a packet list of a DRM_M2TS_PES object 
**                             to search for the last packet within the PES that  
**                             contains the previous PES' payload.
** [f_ppLastOverflowPacket] -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _FindLastOverflowPacket(
    __inout     DRM_M2TS_PACKET_LIST    *f_pPacketList,
    __out       DRM_M2TS_PACKET        **f_ppLastOverflowPacket ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _FindLastOverflowPacket(
    __inout     DRM_M2TS_PACKET_LIST    *f_pPacketList,
    __out       DRM_M2TS_PACKET        **f_ppLastOverflowPacket )
{
    DRM_RESULT           dr                     = DRM_SUCCESS;
    DRM_M2TS_PACKET     *pPacket                = NULL;
    DRM_M2TS_PACKET     *pLastOverflowPacket    = NULL;
    DRM_BOOL             fHasOverflowPayload    = FALSE;

    DRMASSERT( f_pPacketList != NULL );
    DRMASSERT( f_ppLastOverflowPacket != NULL );
    __analysis_assume( f_pPacketList != NULL );
    __analysis_assume( f_ppLastOverflowPacket != NULL );

    *f_ppLastOverflowPacket = NULL;

    pPacket = f_pPacketList->pHead;
    while( pPacket != NULL )
    {
        if( pPacket->cbPayloadLastPES > 0 )
        {
            fHasOverflowPayload = TRUE;
            pLastOverflowPacket = pPacket;
        }

        if( DRM_M2TS_IS_LAST_OF_LAST_PES_PACKET( pPacket ) )
        {
            *f_ppLastOverflowPacket = pPacket;
            break;
        }

        pPacket = pPacket->pNextPerUnit;
    }

    if( fHasOverflowPayload
     && *f_ppLastOverflowPacket == NULL )
    {
        /* Not suppose to happen */
        DRMASSERT( FALSE );

        /* Fix the states just in case this does happen */
        *f_ppLastOverflowPacket = pLastOverflowPacket;
        pLastOverflowPacket->wPacketFlags |= DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET;
    }

    return dr;
}

/**********************************************************************
**
** Function:   _FillDroppedSpacesWithAdaptationField
**
** Synopsis:   The caller has either dropped overflow payload, PES header
**             or the regular payload.  Base on the updated cbPayload,
**             cbHeader and cbPayloadLastPES, this function will use
**             the adaptation field to fill the spaces created by the caller.
**             It will add a new packet when it is necessary.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pPES]                 -- Pointer to the PES that owns the packet pointed by f_pPacket.
** [f_pPacket]              -- The packet to be filled with adapation stuffing bytes.
** [f_cbToFill]             -- Number of stuffing bytes to fill.
** [f_ppPacketNew]          -- In a case where there isn't enough space for the adaptation
**                             field this function will create a new packet and return it 
**                             to the caller.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY if out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _FillDroppedSpacesWithAdaptationField(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_PACKET                 *f_pPacket,
    __in      const DRM_BYTE                         f_cbToFill) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _FillDroppedSpacesWithAdaptationField(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_PACKET                 *f_pPacket,
    __in      const DRM_BYTE                         f_cbToFill)
{
    DRM_RESULT           dr                     = DRM_SUCCESS;
    DRM_BYTE             cbPacketData           = 0;

    ChkArg( f_cbToFill <= DRM_M2TS_MAX_TS_PAYLOAD_SIZE );

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pPacket != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pPacket != NULL );

    if( f_cbToFill > 0 )
    {
        if( DRM_M2TS_HAS_ADAPTATION_FIELD( f_pPacket->rgbData ) )
        {
            /*
            ** The length field of the adaptation field is on the fifth byte (rgbData[4]) of the packet.
            ** Becasue the length field doesn't count itself, so we need to +1 in order
            ** to include all the parts on the packet to ensure that the sum of the parts
            ** equal to DRM_M2TS_MAX_TS_PAYLOAD_SIZE.
            */
            ChkDR( DRM_ByteAdd( f_pPacket->cbPayload, f_cbToFill, &cbPacketData ) );
            ChkDR( DRM_ByteAdd( cbPacketData, f_pPacket->rgbData[4], &cbPacketData ) );
            ChkDR( DRM_ByteIncOne( cbPacketData ) );
            ChkBOOL( cbPacketData == DRM_M2TS_MAX_TS_PAYLOAD_SIZE, DRM_E_M2TS_INTERNAL_ERROR );

            /* Add the byte count of the overflow payload to the adaptation field and extend the stuffing bytes */
            f_pPacket->rgbData[4] += f_cbToFill;
            MEMSET( f_pPacket->rgbData + DRM_M2TS_PACKET_SIZE - f_pPacket->cbPayload - f_cbToFill, DRM_M2TS_STUFFING_BYTE, f_cbToFill );

            /* Remove the payload flag if there is no payload */
            if( f_pPacket->cbPayload == 0 )
            {
                DRM_M2TS_REMOVE_PAYLOAD_FLAG( f_pPacket->rgbData );
            }
        }
        else
        {
            AssertChkArg( DRM_M2TS_HAS_PAYLOAD( f_pPacket->rgbData ) );

            ChkDR( DRM_ByteAdd( f_pPacket->cbPayload, f_cbToFill, &cbPacketData ) );
            ChkBOOL( cbPacketData == DRM_M2TS_MAX_TS_PAYLOAD_SIZE, DRM_E_M2TS_INTERNAL_ERROR );

            /* This packet doesn't have adaptation field, we need to create one */

            /* Set adaptation flag for the current packet */
            DRM_M2TS_SET_ADAPTATION_FIELD( f_pPacket->rgbData );

            f_pPacket->rgbData[4] = (DRM_BYTE)( f_cbToFill - 1 ); /* Adaptation field length */
            if( f_cbToFill > 1 )
            {
                f_pPacket->rgbData[5] = 0; /* Adaptation flags */
                if( f_cbToFill > 2 )
                {
                    MEMSET( f_pPacket->rgbData + 6, DRM_M2TS_STUFFING_BYTE, (DRM_SIZE_T)( f_cbToFill - 2 ) );
                }
            }
        }

        DRMASSERT( f_pPacket->cbPayload + f_pPacket->rgbData[4] + 1 == DRM_M2TS_MAX_TS_PAYLOAD_SIZE );
        DRMASSERT( f_pPacket->cbPayload >= f_pPacket->cbHeader );
        DRMASSERT( f_pPacket->cbPayloadLastPES == 0 );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   DRM_M2TS_KeepOverflowPayload
**
** Synopsis:   Keep the packets that have the overflow payload and drop
**             the rest of the packets on the PES.
**             Since the PES is going to be dropped, we need to make the
**             first packet of the PES as non-unit-start so it becomes
**             the continuation packet of last PES.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pPES]                 -- Pointer to the PES that contains the overflow payload
**                             to keep.
** [f_pPacketList]          -- Contain the list of overflow packets to be adjusted
**                             and added to the end of the packet list of f_pPES.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_M2TS_KeepOverflowPayload(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_PES                    *f_pPES,
    __inout         DRM_M2TS_PACKET_LIST            *f_pPacketList )
{
    DRM_RESULT           dr                     = DRM_SUCCESS;
    DRM_M2TS_PACKET     *pPacket                = NULL;
    DRM_BYTE             bCoutinuityCounter     = 0;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pPES != NULL );
    ChkArg( f_pPES->oPacketList.pTail != NULL );
    ChkArg( f_pPacketList != NULL );
    ChkArg( f_pPacketList->pHead != NULL );

    bCoutinuityCounter = (DRM_BYTE)( ( DRM_M2TS_GET_CONTINUITY_COUNTER( f_pPES->oPacketList.pTail->rgbData ) + 1 ) & DRM_M2TS_CONTINUITY_COUNTER_MASK );

    while( f_pPacketList->cItems > 0 )
    {
        DRM_M2TS_POP_FROM_TOP_PER_UNIT_LIST( f_pPacketList, DRM_M2TS_PACKET, pPacket );

        if( pPacket->cbPayloadLastPES > 0 )
        {
            DRM_BYTE    cbToDrop               = 0;
            DRM_BYTE    bOffsetHeader          = 0;
            DRM_BYTE    bOffsetOverflowPayload = 0;

            ChkDR( DRM_ByteSub( pPacket->cbPayload, pPacket->cbPayloadLastPES, &cbToDrop ) ); /* to drop header and the payload of current PES */
            ChkDR( DRM_ByteSub( DRM_M2TS_PACKET_SIZE, pPacket->cbPayload, &bOffsetHeader ) );
            ChkDR( DRM_ByteAdd( bOffsetHeader, pPacket->cbHeader, &bOffsetOverflowPayload ) );

            ChkBOOL( pPacket->cbPayload >= pPacket->cbPayloadLastPES
                    && DRM_M2TS_PACKET_SIZE > pPacket->cbPayload, DRM_E_M2TS_INTERNAL_ERROR );

            /* Remove DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET flag */
            pPacket->wPacketFlags &= ~DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET;

            /* Remove the unit start flag to make it a member packet of f_pPES */
            DRM_M2TS_REMOVE_UNIT_START_FLAG( pPacket->rgbData );

            /* Move the overflow payload to the end of the packet */
            ChkOverflow( DRM_M2TS_PACKET_SIZE, pPacket->cbPayloadLastPES );
            MEMMOVE( pPacket->rgbData + DRM_M2TS_PACKET_SIZE - pPacket->cbPayloadLastPES,
                     pPacket->rgbData + bOffsetOverflowPayload,
                     pPacket->cbPayloadLastPES );

            /* Make the overflow payload as the regular payload */
            pPacket->cbHeader           = 0;
            pPacket->cbPayload          = pPacket->cbPayloadLastPES;
            pPacket->cbPayloadLastPES   = 0;

            /* Replace existing adaptation field with stuffing bytes */
            if( DRM_M2TS_HAS_ADAPTATION_FIELD( pPacket->rgbData ) )
            {
                ChkBOOL( pPacket->rgbData[ DRM_M2TS_ADAPTATION_LENGTH_FIELD_OFFSET ] <= DRM_M2TS_MAX_ADAPTATION_FIELD_LENGTH, DRM_E_M2TS_ADAPTATION_LENGTH_INVALID );
                if( pPacket->rgbData[ DRM_M2TS_ADAPTATION_LENGTH_FIELD_OFFSET ] >= 1 )
                {
                    pPacket->rgbData[ DRM_M2TS_ADAPTATION_LENGTH_FIELD_OFFSET + 1 ] = 0; /* set all of the adaptation flags/indicators to 0 */
                    MEMSET( pPacket->rgbData + DRM_M2TS_ADAPTATION_LENGTH_FIELD_OFFSET + 2, 
                            DRM_M2TS_STUFFING_BYTE, 
                            (DRM_SIZE_T)( pPacket->rgbData[ DRM_M2TS_ADAPTATION_LENGTH_FIELD_OFFSET ] - 1 ) );
                }
            }

            DRM_M2TS_SET_PAYLOAD_FLAG( pPacket->rgbData );

            ChkDR( _FillDroppedSpacesWithAdaptationField( f_pEncContext, pPacket, cbToDrop ) );

            /* Update the continuity counter in case there is a packet that doesn't have the overflow payload */
            DRM_M2TS_SET_AND_INC_CONTINUITY_COUNTER( pPacket->rgbData, bCoutinuityCounter );

            /* Insert the updated packet to incoming packet list and the per unit list to be next to the last packet on the per unit list */
            DRM_M2TS_INSERT_ITEM( &f_pEncContext->oIncomingPacketList, f_pPES->oPacketList.pTail, pPacket );
            DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &f_pPES->oPacketList, pPacket );
            pPacket = NULL;
        }
    }

ErrorExit:
    if( pPacket != NULL )
    {
        /* Something went wrong, recycle the packet */
        DRM_M2TS_PUSH_TO_TOP_LIST_NO_FAIL(  &f_pEncContext->oFreePacketList, pPacket );
    }
    return dr;
}

/**********************************************************************
**
** Function:   DRM_M2TS_DropOverflowPayload
**
** Synopsis:   Drop the overflow payload for the last PES from current
**             PES by replacing the overflow payload with adaptation field,
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pPES]                 -- Pointer to a packet list of a PES that contains 
**                             the overflow payload to drop.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
 DRM_API DRM_NO_INLINE DRM_RESULT DRM_M2TS_DropOverflowPayload(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_PACKET_LIST            *f_pPacketList )
{
    DRM_RESULT           dr                     = DRM_SUCCESS;
    DRM_M2TS_PACKET     *pPacket                = NULL;
    DRM_M2TS_PACKET     *pLastOverflowPacket    = NULL;
    DRM_BYTE             bContinuityCounter     = 0;
    DRM_BYTE             bContinuityCounterLast = 0;

    ChkArg( f_pEncContext );
    ChkArg( f_pPacketList != NULL );
    ChkArg( f_pPacketList->pHead != NULL );
    ChkArg( f_pPacketList->pTail != NULL );

    bContinuityCounter      = DRM_M2TS_GET_CONTINUITY_COUNTER( f_pPacketList->pHead->rgbData );
    bContinuityCounterLast  = DRM_M2TS_GET_CONTINUITY_COUNTER( f_pPacketList->pTail->rgbData );

    /* Find the last packet that contains the overflow payload of last PES */
    ChkDR( _FindLastOverflowPacket( f_pPacketList, &pLastOverflowPacket ) );

    if( pLastOverflowPacket != NULL )
    {
        pPacket = f_pPacketList->pHead;
        while( pPacket != NULL )
        {
            if( pPacket->cbPayloadLastPES > 0 )
            {
                DRM_BYTE cbPayloadLastPES       = pPacket->cbPayloadLastPES;
                DRM_BYTE bOffsetHeader          = 0;
                DRM_BYTE bNewOffsetHeader       = 0;
                DRM_BYTE bOffsetOverflowPayload = 0;

                ChkDR( DRM_ByteSub( DRM_M2TS_PACKET_SIZE, pPacket->cbPayload, &bOffsetHeader ) );
                ChkDR( DRM_ByteAdd( bOffsetHeader, pPacket->cbHeader, &bOffsetOverflowPayload ) );
                ChkDR( DRM_ByteAdd( bOffsetOverflowPayload, cbPayloadLastPES, &bNewOffsetHeader ) );
                ChkDR( DRM_ByteSub( bNewOffsetHeader, pPacket->cbHeader, &bNewOffsetHeader ) );
                ChkBOOL( DRM_M2TS_PACKET_SIZE >= bNewOffsetHeader, DRM_E_M2TS_INTERNAL_ERROR );

                DRMASSERT( pPacket->cbHeader + cbPayloadLastPES <= pPacket->cbPayload );
                DRMASSERT( pPacket->cbPayload <= DRM_M2TS_MAX_TS_PAYLOAD_SIZE );

                /* Remove overflow payload by moving the PES header right before the current PES' payload */
                MEMMOVE( &pPacket->rgbData[ bNewOffsetHeader ],
                         &pPacket->rgbData[ bOffsetHeader ],
                         pPacket->cbHeader );

                pPacket->cbPayloadLastPES = 0;
                ChkDR( DRM_ByteSub( pPacket->cbPayload, cbPayloadLastPES, &pPacket->cbPayload ) );

                ChkDR( _FillDroppedSpacesWithAdaptationField( f_pEncContext, pPacket, cbPayloadLastPES ) );
            }

            if( pPacket == pLastOverflowPacket )
            {
                DRMASSERT( DRM_M2TS_IS_LAST_OF_LAST_PES_PACKET( pPacket ) );

                /* Remove DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET flag */
                pPacket->wPacketFlags &= ~DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET;

                break;
            }

            pPacket = pPacket->pNextPerUnit;
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   DRM_M2TS_BuildSubsamplesMappings
**
** Synopsis:   Build subsample mappings for each PES packets in the ECM.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pECM]                 -- Pointer to the ECM that we want to create the
**                             the subsample mappings for all the PES packets
**                             it owns.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BuildSubsamplesMappings(
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_ECM                    *f_pECM  )
{
    DRM_RESULT           dr                 = DRM_SUCCESS;
    DRM_M2TS_STREAM     *pStream            = NULL;
    DRM_M2TS_PES        *pPES               = NULL;
    DRM_M2TS_PACKET     *pPacket            = NULL;
    DRM_WORD             iPES               = 0;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pECM != NULL );

    /*
    ** The sample and subsample table used for last ECM should have been encrypted
    ** therefore there are free now.  It is safe to reset them now.
    */
    f_pEncContext->dwIndexNextFreeSample = 0;
    f_pEncContext->dwIndexNextFreeSubsample = 0;

    DRMASSERT( f_pECM->cPESPackets > 0 );

    /* Create subsample mappings for each PES in the ECM */
    for( iPES = 0; iPES < f_pECM->cPESPackets; iPES++ )
    {
        DRM_M2TS_SUBSAMPLE_CONTEXT oSubsampleContext = { 0 };

        pPES = &f_pECM->rgPESPackets[ iPES ];
        pPES->dwIndexFirstSample = 0;
        pPES->dwIndexLastSample = 0;
        pPES->fHasSample = FALSE;

        ChkDR( DRM_M2TS_GetStream( f_pEncContext,
                                   pPES->wPID,
                                   &pStream ) );

        pPacket = pPES->oPacketList.pHead;

        /* Init the subsample context to be ready for creating the subsample mappings for the PES */
        oSubsampleContext.bLast = DRM_M2TS_STUFFING_BYTE;     /* Any value except 0x00 */

        DRM_M2TS_VERIFY_PER_UNIT_LINKED_LIST_LENGTH( &pPES->oPacketList, DRM_M2TS_PACKET );

        /* Only build the subsamples for the PES that is good */
        while( DRM_SUCCEEDED( pPES->drPESResult )
            && pPacket != NULL )
        {
            ChkDR( DRM_M2TS_FillSubsampleContextBuffer( FALSE, &oSubsampleContext, &pPacket, NULL ) );

            oSubsampleContext.fLastCall = pPacket == NULL;

            /* Create sample and subsample only when the PES has payload */
            if( oSubsampleContext.cbBuffer > 0 )
            {
                switch( pPES->bStreamType )
                {
                case DRM_M2TS_STREAM_TYPE_MPEG2_VIDEO:
                case DRM_M2TS_STREAM_TYPE_DIGICIPHER_II_VIDEO:
                    ChkDR( DRM_M2TS_MPEG2_VIDEO_CreateSubsampleMappings( f_pEncContext,
                                                                         &oSubsampleContext,
                                                                         pPES ) );
                    break;

                case DRM_M2TS_STREAM_TYPE_H264:
                    ChkDR( DRM_M2TS_H264_CreateSubsampleMappings( f_pEncContext,
                                                                  &oSubsampleContext,
                                                                  pPES ) );
                    break;

                case DRM_M2TS_STREAM_TYPE_MPEGA_AUDIO:
                    ChkDR( DRM_M2TS_CreateAudioSubsampleMappings( f_pEncContext,
                                                                  DRM_M2TS_SyncAndParseMPEGAAudioFrameHeaderCallback,
                                                                  &oSubsampleContext,
                                                                  pPES,
                                                                  pStream ) );
                    break;

                case DRM_M2TS_STREAM_TYPE_ADTS:
                    ChkDR( DRM_M2TS_CreateAudioSubsampleMappings( f_pEncContext,
                                                                  DRM_M2TS_SyncAndParseADTSFrameHeaderCallback,
                                                                  &oSubsampleContext,
                                                                  pPES,
                                                                  pStream ) );
                    break;

                case DRM_M2TS_STREAM_TYPE_DDPLUS:
                    ChkDR( DRM_M2TS_CreateAudioSubsampleMappings( f_pEncContext,
                                                                  DRM_M2TS_SyncAndParseDDPlusFrameHeaderCallback,
                                                                  &oSubsampleContext,
                                                                  pPES,
                                                                  pStream ) );
                    break;

                default:
                    ChkDR( DRM_E_M2TS_INTERNAL_ERROR );
                    break;
                }
            }
        }
    }

    f_pECM->fSubsampleMappingsCompleted = TRUE;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   _AddEncryptedSegment
**
** Synopsis:   Adds an entry to the encryptor context's encrypted segements
**             buffer.  If the buffer is full increase it by
**             DRM_M2TS_ENCRYPTED_SEGMENTS_INCREMENT.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pPESEncContext]       -- Pointer to the PES encryption context.
** [f_pPacket]              -- Pointer to the packet that its protected payload will
**                             be added to the encrypted segments array.
** [f_dwOffsetEncryptedSegment] -- Offset of the packet's payload for the encrypted 
**                             segment.
** [f_cbEncryptedSegment]   -- Length fo the encrypted segment in bytes.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _AddEncryptedSegment(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT              *f_pEncContext,
    __inout         DRM_M2TS_PES_ENCRYPTION_CONTEXT         *f_pPESEncContext,
    __in            DRM_M2TS_PACKET                         *f_pPacket,
    __in      const DRM_DWORD                                f_dwOffsetEncryptedSegment,
    __in      const DRM_DWORD                                f_cbEncryptedSegment ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _AddEncryptedSegment(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT              *f_pEncContext,
    __inout         DRM_M2TS_PES_ENCRYPTION_CONTEXT         *f_pPESEncContext,
    __in            DRM_M2TS_PACKET                         *f_pPacket,
    __in      const DRM_DWORD                                f_dwOffsetEncryptedSegment,
    __in      const DRM_DWORD                                f_cbEncryptedSegment )
{
    DRM_RESULT      dr                      = DRM_SUCCESS;
    DRM_BYTE      **ppbEncryptedSegments    = NULL;
    DRM_DWORD      *prgcbEncryptedSegments  = NULL;

    DRMASSERT( f_pEncContext != NULL );
    __analysis_assume( f_pEncContext != NULL );
    if( f_cbEncryptedSegment > 0 )
    {
        /* Ensure the offset is inside the payload and the length of the encrypted segment is within the boundary of the packet's data */
        ChkArg( f_pPacket != NULL );
        ChkArg( ( f_dwOffsetEncryptedSegment < DRM_M2TS_PACKET_SIZE )
             && ( f_cbEncryptedSegment < DRM_M2TS_PACKET_SIZE )
             && ( f_pPacket->cbPayload < DRM_M2TS_PACKET_SIZE )
             && ( f_dwOffsetEncryptedSegment >= (DRM_DWORD)( DRM_M2TS_PACKET_SIZE - f_pPacket->cbPayload ) )
             && ( f_dwOffsetEncryptedSegment + f_cbEncryptedSegment <= DRM_M2TS_PACKET_SIZE ) );

        /* Increase the encrypted segments buffer if it is full */
        if( f_pPESEncContext->dwIndexNextEncryptedSegment >= f_pEncContext->cEncryptedSegments )
        {
            if( f_pEncContext->cEncryptedSegments > 0 )
            {
                DRMASSERT( f_pEncContext->ppbEncryptedSegments != NULL );
                DRMASSERT( f_pEncContext->prgcbEncryptedSegments != NULL );
            }
            else
            {
                DRMASSERT( f_pEncContext->ppbEncryptedSegments == NULL );
                DRMASSERT( f_pEncContext->prgcbEncryptedSegments == NULL );
            }

            /* Copy to a larger buffer and copy data over */
            ChkDR( DRM_M2TS_MemAllocAndCopy( f_pEncContext,
                                             SIZEOF( DRM_DWORD ),
                                             f_pEncContext->cEncryptedSegments,
                                             DRM_M2TS_ENCRYPTED_SEGMENTS_INCREMENT,
                                             f_pEncContext->prgcbEncryptedSegments,
                                             (DRM_VOID **)&prgcbEncryptedSegments ) );

            ChkDR( DRM_M2TS_MemAllocAndCopy( f_pEncContext,
                                             SIZEOF( DRM_BYTE * ),
                                             f_pEncContext->cEncryptedSegments,
                                             DRM_M2TS_ENCRYPTED_SEGMENTS_INCREMENT,
                                             f_pEncContext->ppbEncryptedSegments,
                                             (DRM_VOID **)&ppbEncryptedSegments ) );

            /* Free the original buffers */
            ChkDR( DRM_M2TS_MemFree( f_pEncContext, f_pEncContext->prgcbEncryptedSegments, f_pEncContext->cEncryptedSegments * SIZEOF( DRM_DWORD ) ) );
            ChkDR( DRM_M2TS_MemFree( f_pEncContext, f_pEncContext->ppbEncryptedSegments,   f_pEncContext->cEncryptedSegments * SIZEOF( DRM_VOID * ) ) );

            /* Good, both buffers are done, we can transfer now */
            ChkDR( DRM_DWordAdd( f_pEncContext->cEncryptedSegments, DRM_M2TS_ENCRYPTED_SEGMENTS_INCREMENT, &f_pEncContext->cEncryptedSegments ) );
            f_pEncContext->ppbEncryptedSegments   = ppbEncryptedSegments;
            f_pEncContext->prgcbEncryptedSegments = prgcbEncryptedSegments;

            ppbEncryptedSegments = NULL;
            prgcbEncryptedSegments = NULL;
        }

        f_pEncContext->prgcbEncryptedSegments[ f_pPESEncContext->dwIndexNextEncryptedSegment ] = f_cbEncryptedSegment;
        f_pEncContext->ppbEncryptedSegments[ f_pPESEncContext->dwIndexNextEncryptedSegment ] = f_pPacket->rgbData + f_dwOffsetEncryptedSegment;

        ChkDR( DRM_DWordIncOne( f_pPESEncContext->dwIndexNextEncryptedSegment ) );
    }

ErrorExit:
    /* If ppbEncryptedSegments or prgcbEncryptedSegments is not NULL then the calculation for the size shouldn't cause interger overflow */
    SAFE_M2TS_OEM_FREE( f_pEncContext, ppbEncryptedSegments,   ( f_pEncContext->cEncryptedSegments + DRM_M2TS_ENCRYPTED_SEGMENTS_INCREMENT ) * SIZEOF( DRM_VOID * ) );
    SAFE_M2TS_OEM_FREE( f_pEncContext, prgcbEncryptedSegments, ( f_pEncContext->cEncryptedSegments + DRM_M2TS_ENCRYPTED_SEGMENTS_INCREMENT ) * SIZEOF( DRM_DWORD ) );
    return dr;
}

/**********************************************************************
**
** Function:   _SetSubsampleSegments
**
** Synopsis:   Locates the clear or encrypted subsample's location and
**             length.  And creates an entry in ppbEncryptedSegments and
**             prgcbEncryptedSegments for each encrypted subsample.
**             An encrypted subsample may have one or more encrypted
**             segements because it may spread over packet boundary.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pPESEncContext]       -- Pointer to the PES encryption context.
** [f_IsClear]              -- Flag to indicate whether the subsample is clear.
** [f_cbSubsample]          -- Length of the subsample in bytes.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _SetSubsampleSegments(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT              *f_pEncContext,
    __inout         DRM_M2TS_PES_ENCRYPTION_CONTEXT         *f_pPESEncContext,
    __in      const DRM_BOOL                                 f_IsClear,
    __in      const DRM_DWORD                                f_cbSubsample ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _SetSubsampleSegments(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT              *f_pEncContext,
    __inout         DRM_M2TS_PES_ENCRYPTION_CONTEXT         *f_pPESEncContext,
    __in      const DRM_BOOL                                 f_IsClear,
    __in      const DRM_DWORD                                f_cbSubsample )
{
    DRM_RESULT       dr                     = DRM_SUCCESS;
    DRM_M2TS_PACKET *pPacket                = NULL;
    DRM_DWORD        cbSubsampleRemaining   = f_cbSubsample;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pPESEncContext != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pPESEncContext != NULL );

    /* First look for the subsample from the PES packet list */
    while( cbSubsampleRemaining > 0
        && f_pPESEncContext->pPacket != NULL )
    {
        pPacket = f_pPESEncContext->pPacket;

        if( cbSubsampleRemaining >= f_pPESEncContext->cbPayloadRemaining )
        {
            /* The subsample needs more bytes than the remaining bytes in the current packet */

            if( !f_IsClear )
            {
                ChkDR( _AddEncryptedSegment( f_pEncContext,
                                             f_pPESEncContext,
                                             pPacket,
                                             (DRM_DWORD)( DRM_M2TS_PACKET_SIZE - f_pPESEncContext->cbPayloadRemaining ),
                                             f_pPESEncContext->cbPayloadRemaining ) );
            }

            ChkDR( DRM_DWordSub( cbSubsampleRemaining, f_pPESEncContext->cbPayloadRemaining, &cbSubsampleRemaining ) );
            f_pPESEncContext->cbPayloadRemaining = 0;

            /* Rotate to next packet on the list */
            f_pPESEncContext->pPacket = f_pPESEncContext->pPacket->pNextPerUnit;
            if( f_pPESEncContext->pPacket != NULL )
            {
                ChkDR( DRM_ByteSub( f_pPESEncContext->pPacket->cbPayload, f_pPESEncContext->pPacket->cbHeader, &f_pPESEncContext->cbPayloadRemaining ) );
                ChkDR( DRM_ByteSub( f_pPESEncContext->cbPayloadRemaining, f_pPESEncContext->pPacket->cbPayloadLastPES, &f_pPESEncContext->cbPayloadRemaining ) );
                DRMASSERT( f_pPESEncContext->cbPayloadRemaining < DRM_M2TS_PACKET_SIZE );
            }
        }
        else
        {
            /* We have more (remaining) bytes in the current packet than the subsample */
            if( !f_IsClear )
            {
                ChkDR( _AddEncryptedSegment( f_pEncContext,
                                             f_pPESEncContext,
                                             pPacket,
                                             (DRM_DWORD)( DRM_M2TS_PACKET_SIZE - f_pPESEncContext->cbPayloadRemaining ),
                                             cbSubsampleRemaining ) );
            }

            ChkBOOL( cbSubsampleRemaining < DRM_M2TS_PACKET_SIZE, DRM_E_M2TS_INTERNAL_ERROR );
            ChkDR( DRM_ByteSub( f_pPESEncContext->cbPayloadRemaining, (DRM_BYTE)cbSubsampleRemaining, &f_pPESEncContext->cbPayloadRemaining ) );
            cbSubsampleRemaining = 0;
        }
    }

    ChkBOOL( cbSubsampleRemaining == 0, DRM_E_M2TS_INTERNAL_ERROR );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   _EncryptPES
**
** Synopsis:   Encrypt a single PES.
**             Before this function calls Drm_LocalLicense_EncryptSample
**             to encrypt the PES, it creates the segements of the
**             encrypted subsamples.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pECM]                 -- Pointer to the ECM that owns the PES to be encrypted.
** [f_pPES]                 -- Pointer to the PES to be encrypted.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _EncryptPES(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __in      const DRM_M2TS_ECM                    *f_pECM,
    __inout         DRM_M2TS_PES                    *f_pPES ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _EncryptPES(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __in      const DRM_M2TS_ECM                    *f_pECM,
    __inout         DRM_M2TS_PES                    *f_pPES )
{
    DRM_RESULT                       dr                     = DRM_SUCCESS;
    DRM_M2TS_PES_ENCRYPTION_CONTEXT  oPESEncContext         = { 0 };
    DRM_DWORD                        dwIndexSample          = 0;
    DRM_DWORD                        dwIndexSubsample       = 0;
    const DRM_M2TS_PACKET           *pPacket                = NULL;
    DRM_M2TS_SAMPLE                 *pSample                = NULL;
    const DRM_M2TS_SUBSAMPLE        *pSubsample             = NULL;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pECM != NULL );
    DRMASSERT( f_pPES != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pECM != NULL );
    __analysis_assume( f_pPES != NULL );

    if( f_pPES->fHasSample )
    {
        dwIndexSample = f_pPES->dwIndexFirstSample;

        ChkBOOL( f_pPES->oPacketList.pHead != NULL, DRM_E_M2TS_INTERNAL_ERROR );
        oPESEncContext.pPacket = f_pPES->oPacketList.pHead;

        pPacket = oPESEncContext.pPacket;
        ChkDR( DRM_ByteSub( pPacket->cbPayload, pPacket->cbHeader, &oPESEncContext.cbPayloadRemaining ) );
        ChkDR( DRM_ByteSub( oPESEncContext.cbPayloadRemaining, pPacket->cbPayloadLastPES, &oPESEncContext.cbPayloadRemaining ) );
        DRMASSERT( oPESEncContext.cbPayloadRemaining < DRM_M2TS_PACKET_SIZE );

        while ( dwIndexSample <= f_pPES->dwIndexLastSample )
        {
            oPESEncContext.dwIndexNextEncryptedSegment = 0;

            pSample = &f_pEncContext->pSamples[ dwIndexSample ];

            DRMASSERT( pSample->fHasSubsample );
            dwIndexSubsample = pSample->dwIndexFirstSubsample;
            while ( dwIndexSubsample <= pSample->dwIndexLastSubsample )
            {
                pSubsample = &f_pEncContext->pSubsamples[ dwIndexSubsample ];

                ChkDR( _SetSubsampleSegments( f_pEncContext, &oPESEncContext, TRUE, pSubsample->cbClearBytes ) );
                ChkDR( _SetSubsampleSegments( f_pEncContext, &oPESEncContext, FALSE, pSubsample->cbEncryptedBytes ) );

                ChkDR( DRM_DWordIncOne( dwIndexSubsample ) );
            }

#if DBG
            {
                DRM_DWORD cbEncryptedSubsamples = 0;
                DRM_DWORD cbEncryptedSegments   = 0;
                DRM_DWORD iSegment              = 0;

                /* Make sure the total length of the encrypted subsamples matches with encrypted segments */

                /* Aggregate the total length of the encrypted subsamples */
                dwIndexSubsample = pSample->dwIndexFirstSubsample;
                while ( dwIndexSubsample <= pSample->dwIndexLastSubsample )
                {
                    pSubsample = &f_pEncContext->pSubsamples[ dwIndexSubsample ];
                    ChkDR( DRM_DWordAdd( cbEncryptedSubsamples, pSubsample->cbEncryptedBytes, &cbEncryptedSubsamples ) );
                    ChkDR( DRM_DWordIncOne( dwIndexSubsample ) );
                }

                /* Aggregate the total length of the encrypted segments */
                for( iSegment = 0; iSegment < oPESEncContext.dwIndexNextEncryptedSegment; iSegment++ )
                {
                    ChkDR( DRM_DWordAdd( cbEncryptedSegments, f_pEncContext->prgcbEncryptedSegments[ iSegment ], &cbEncryptedSegments ) );
                }

                DRMASSERT( cbEncryptedSegments == cbEncryptedSubsamples );
            }
#endif /* DBG */

            /* Encrypt the sample */
            if( oPESEncContext.dwIndexNextEncryptedSegment > 0 )
            {
                ChkDR( Drm_LocalLicense_EncryptSample( f_pECM->hLicense,
                                                       oPESEncContext.dwIndexNextEncryptedSegment,
                                                       f_pEncContext->prgcbEncryptedSegments,
                                                       f_pEncContext->ppbEncryptedSegments,
                                                       &pSample->qwIV ) );
            }
            ChkDR( DRM_DWordIncOne( dwIndexSample ) );
        }
    }
    else
    {
        ChkBOOL( !DRM_M2TS_HasPayload( f_pPES ), DRM_E_M2TS_INTERNAL_ERROR );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   DRM_M2TS_EncryptPESPackets
**
** Synopsis:   Encrypt each PES in the ECM.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pECM]                 -- Pointer to the ECM that all its PES packets 
**                             will be encrypted.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_INTERNAL_ERROR when something 
**                              unexpected happened.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_EncryptPESPackets(
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_ECM                    *f_pECM  )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_M2TS_STREAM     *pStream    = NULL;
    DRM_M2TS_PES        *pPES       = NULL;
    DRM_WORD             wPID       = DRM_M2TS_UNDEFINED_PID;
    DRM_WORD             iPES       = 0;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pECM != NULL );

    for( iPES = 0; iPES < f_pECM->cPESPackets; iPES++ )
    {
        pPES = &f_pECM->rgPESPackets[ iPES ];
        if( pPES->wPID != wPID )
        {
            wPID = pPES->wPID;
            ChkDR( DRM_M2TS_GetStream( f_pEncContext, wPID, &pStream ) );
        }

        DRMASSERT( pStream != NULL );

        if( DRM_SUCCEEDED( pPES->drPESResult ) )
        {
            DRMASSERT( pPES->ePESState == eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD );
            ChkBOOL( pPES->ePESState == eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD, DRM_E_M2TS_INTERNAL_ERROR );

            ChkDR( _EncryptPES( f_pEncContext, f_pECM, pPES ) );

            DRM_M2TS_SET_PACKET_STATE_ON_PERUNIT_LIST( DRM_M2TS_PACKET, &pPES->oPacketList, eDRM_M2TS_PACKET_STATE_READY );
        }
        else
        {
            /* Drop the entire PES by marking each packet as bad on the packet list of the PES */
            DRM_M2TS_SET_PACKET_STATE_ON_PERUNIT_LIST( DRM_M2TS_PACKET, &pPES->oPacketList, eDRM_M2TS_PACKET_STATE_BAD );

            /* Log an event for the PES that is going to be dropped */
            (DRM_VOID)DRM_M2TS_LogEventWithPES( f_pEncContext, eDRM_M2TS_LOG_CATEGORY_WARNING, TRUE, pPES->drPESResult, pPES );
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _SetECMTPacket
**
** Synopsis:    Create a single TS packet and append it to the end of
**              the ECM builder's packet list.  Base on the parameters
**              this function will set the following fields and the payload
**              1) payload_unit_start_indicator
**              2) adaptation_filed_control
**              3) continuity_counter
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poECMBuilderContext]  -- Pointer to the ECM builder context.
** [f_cbAdaptation]         -- Length of the adaptation field for the ECM packet
** [f_pwOffset]             -- Out parameter to contain the offset of the buffer
**                             in the ECM builder context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _SetECMTPacket(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT    *f_poECMBuilderContext,
    __in      const DRM_BYTE                         f_cbAdaptation,
    __inout         DRM_WORD                        *f_pwOffset ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _SetECMTPacket(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT    *f_poECMBuilderContext,
    __in      const DRM_BYTE                         f_cbAdaptation,
    __inout         DRM_WORD                        *f_pwOffset )
{
    DRM_RESULT           dr             = DRM_SUCCESS;
    DRM_M2TS_PACKET     *pPacket        = NULL;
    DRM_BYTE             cbPayload      = 0;
    DRM_BYTE             bOffsetPayload = 0;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_poECMBuilderContext != NULL );
    DRMASSERT( f_pwOffset != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_poECMBuilderContext != NULL );
    __analysis_assume( f_pwOffset != NULL );

    ChkDR( DRM_M2TS_GetFreePacket(  f_pEncContext,
                                    0,                  /* cbPayload */
                                    eDRM_M2TS_PACKET_STATE_READY,
                                    FALSE,              /* assign PacketNo */
                                    NULL,               /* pbPacket */
                                    &pPacket ) );
    /* Write TS Packet header */
    pPacket->rgbData[0] = DRM_M2TS_START_CODE;
    pPacket->rgbData[3] |= 0x10;         /* Set payload only */
    WORD_TO_NETWORKBYTES( pPacket->rgbData, 1, f_poECMBuilderContext->pECM->wPID ); /* set ECM PID */
    if( f_poECMBuilderContext->cECMPacketCreated == 0 )
    {
        pPacket->rgbData[1] |= 0x40; /* Set unit start flag */
    }
    pPacket->rgbData[3] |= f_pEncContext->bNextECMCC++;   /* Set ECM packet's continuity counter */
    f_pEncContext->bNextECMCC &= 0x0F;

    if( f_cbAdaptation > 0 )
    {
        AssertChkArg( f_cbAdaptation <= DRM_M2TS_MAX_TS_PAYLOAD_SIZE - 1 );
        pPacket->rgbData[3] |= 0x20;  /* Set adaptation field flag */
        pPacket->rgbData[4] = (DRM_BYTE)( f_cbAdaptation - 1 );
        if( f_cbAdaptation > 1 )
        {
            pPacket->rgbData[5] = 0; /* Set adaptation field's flags to all 0 */
            if( f_cbAdaptation > 2 )
            {
                MEMSET( pPacket->rgbData + 6, DRM_M2TS_STUFFING_BYTE, (DRM_SIZE_T)( f_cbAdaptation - 2 ) );
            }
        }
        cbPayload = (DRM_DWORD)( DRM_M2TS_MAX_TS_PAYLOAD_SIZE - f_cbAdaptation );
    }
    else
    {
        cbPayload = DRM_M2TS_MAX_TS_PAYLOAD_SIZE;
    }

    /* Copy the ECM payload */
    DRMASSERT( DRM_M2TS_PACKET_HEADER_SIZE + f_cbAdaptation + cbPayload == DRM_M2TS_PACKET_SIZE );
    DRMASSERT( *f_pwOffset + cbPayload <= f_poECMBuilderContext->cbBuffer );
    ChkDR( DRM_ByteAdd( DRM_M2TS_PACKET_HEADER_SIZE, f_cbAdaptation, &bOffsetPayload ) );
    MEMCPY( &pPacket->rgbData[ bOffsetPayload ],
            &f_poECMBuilderContext->rgbBuffer[ *f_pwOffset ],
            cbPayload );

    DRM_M2TS_APPEND_TO_END_OF_LIST( f_poECMBuilderContext->pPacketList, pPacket );

    ChkDR( DRM_WordIncOne( f_poECMBuilderContext->cECMPacketCreated ) );
    ChkDR( DRM_WordAdd( *f_pwOffset, cbPayload, f_pwOffset ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _CreateECMPackets
**
** Synopsis:    Create as many TS packets for the ECM as possible for the
**              data in the buffer.  When it is the last call it will write
**              the remaining data to create the last TS packet for the
**              ECM.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poECMBuilderContext]  -- Pointer to the ECM builder context.
** [f_fLastCall]            -- Flag to indicate whether this is the last call
**                             for the creating the ECM packets.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CreateECMPackets(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT    *f_poECMBuilderContext,
    __in      const DRM_BOOL                         f_fLastCall ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CreateECMPackets(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT    *f_poECMBuilderContext,
    __in      const DRM_BOOL                         f_fLastCall )
{
    DRM_RESULT      dr             = DRM_SUCCESS;
    DRM_WORD        wOffset        = 0;
    DRM_WORD        cbRemaining    = 0;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_poECMBuilderContext != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_poECMBuilderContext != NULL );

    cbRemaining = f_poECMBuilderContext->cbBuffer;

    while( cbRemaining >= DRM_M2TS_MAX_TS_PAYLOAD_SIZE )
    {
        ChkDR( _SetECMTPacket( f_pEncContext,
                               f_poECMBuilderContext,
                               0,                   /* Adaptation field length */
                               &wOffset ) );

        ChkDR( DRM_WordSub( cbRemaining, DRM_M2TS_MAX_TS_PAYLOAD_SIZE, &cbRemaining ) );
    }

    if( f_fLastCall && cbRemaining > 0 )
    {
        DRM_BYTE cbAdaptationField = 0;
        /* Flush the remaining data to the final TS packet */
        /* Now the last TS packet for the ECM */
        ChkDR( DRM_WordToByte( (DRM_WORD)( DRM_M2TS_MAX_TS_PAYLOAD_SIZE - cbRemaining ), &cbAdaptationField ) );
        ChkDR( _SetECMTPacket(  f_pEncContext,
                                f_poECMBuilderContext,
                                cbAdaptationField,
                                &wOffset ) );

        ChkDR( DRM_WordSub( f_poECMBuilderContext->cbBuffer, wOffset, &cbRemaining ) );
    }

    /* shift the remaining data in the buffer to the left */
    if( cbRemaining > 0 )
    {
        MEMMOVE( f_poECMBuilderContext->rgbBuffer, f_poECMBuilderContext->rgbBuffer + wOffset, cbRemaining );
    }

    ChkDR( DRM_DWordAdd( f_poECMBuilderContext->cbWritten, wOffset, &f_poECMBuilderContext->cbWritten ) );
    f_poECMBuilderContext->cbBuffer = cbRemaining;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _WriteECMData_BYTES
**
** Synopsis:    Write a byte array to ECM builder context's buffer.
**              When the buffer is full it will create TS packets for the
**              ECM to clear more room in the buffer.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poECMBuilderContext]  -- Pointer to the ECM builder context.
** [f_dwOffset]             -- Offset of f_pbData.
** [f_cbData]               -- Number of bytes to write to the ECM packet.
** [f_pbData]               -- Pointer to the byte array to add to the ECM packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _WriteECMData_BYTES(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in            DRM_DWORD                            f_dwOffset,
    __in            DRM_DWORD                            f_cbData,
    __in      const DRM_BYTE                            *f_pbData ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _WriteECMData_BYTES(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in            DRM_DWORD                            f_dwOffset,
    __in            DRM_DWORD                            f_cbData,
    __in      const DRM_BYTE                            *f_pbData )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_DWORD   cbAvailable = 0;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_poECMBuilderContext != NULL );
    DRMASSERT( f_pbData != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_poECMBuilderContext != NULL );
    __analysis_assume( f_pbData != NULL );

    while( f_cbData > 0 )
    {
        DRM_DWORD cBytes  = 0;
        DRM_WORD  wcBytes = 0;
        ChkDR( DRM_DWordSub( SIZEOF( f_poECMBuilderContext->rgbBuffer ), f_poECMBuilderContext->cbBuffer, &cbAvailable ) );
        cBytes = min( f_cbData, cbAvailable );
        MEMCPY( f_poECMBuilderContext->rgbBuffer + f_poECMBuilderContext->cbBuffer,
                f_pbData + f_dwOffset,
                cBytes );

        ChkDR( DRM_DWordAdd( f_dwOffset, cBytes, &f_dwOffset ) );
        ChkDR( DRM_DWordSub( f_cbData, cBytes, &f_cbData ) );
        ChkDR( DRM_DWordToWord( cBytes, &wcBytes ) );
        ChkDR( DRM_WordAdd( f_poECMBuilderContext->cbBuffer, wcBytes, &f_poECMBuilderContext->cbBuffer ) );

        if( f_poECMBuilderContext->cbBuffer == SIZEOF( f_poECMBuilderContext->rgbBuffer ) )
        {
            ChkDR( _CreateECMPackets( f_pEncContext,
                                      f_poECMBuilderContext,
                                      FALSE ) );             /* fLastCall */
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _WriteECMData_QWORD
**
** Synopsis:    Write a quard word to ECM builder context's buffer.
**              When the buffer is full it will create TS packets for the
**              ECM to clear more room in the buffer.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poECMBuilderContext]  -- Pointer to the ECM builder context.
** [f_qwData]               -- Data to add to the ECM packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _WriteECMData_QWORD(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in      const DRM_UINT64                           f_qwData ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _WriteECMData_QWORD(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in      const DRM_UINT64                           f_qwData )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_poECMBuilderContext != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_poECMBuilderContext != NULL );

    if( f_poECMBuilderContext->cbBuffer > SIZEOF( f_poECMBuilderContext->rgbBuffer ) - SIZEOF( f_qwData ) )
    {
        ChkDR( _CreateECMPackets( f_pEncContext,
                                  f_poECMBuilderContext,
                                  FALSE ) );              /* fLastCall */
    }

    QWORD_TO_NETWORKBYTES( f_poECMBuilderContext->rgbBuffer, f_poECMBuilderContext->cbBuffer, f_qwData );
    ChkDR( DRM_WordAdd( f_poECMBuilderContext->cbBuffer, SIZEOF( f_qwData ), &f_poECMBuilderContext->cbBuffer ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _WriteECMData_DWORD
**
** Synopsis:    Write a DWORD to ECM builder context's buffer.
**              When the buffer is full it will create TS packets for the
**              ECM to clear more room in the buffer.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poECMBuilderContext]  -- Pointer to the ECM builder context.
** [f_dwData]               -- Data to add to the ECM packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _WriteECMData_DWORD(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in      const DRM_DWORD                            f_dwData ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _WriteECMData_DWORD(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in      const DRM_DWORD                            f_dwData )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_poECMBuilderContext != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_poECMBuilderContext != NULL );

    if( f_poECMBuilderContext->cbBuffer > SIZEOF( f_poECMBuilderContext->rgbBuffer ) - SIZEOF( f_dwData ) )
    {
        ChkDR( _CreateECMPackets( f_pEncContext,
                                  f_poECMBuilderContext,
                                  FALSE ) );              /* fLastCall */
    }

    DWORD_TO_NETWORKBYTES( f_poECMBuilderContext->rgbBuffer, f_poECMBuilderContext->cbBuffer, f_dwData );
    ChkDR( DRM_WordAdd( f_poECMBuilderContext->cbBuffer, SIZEOF( f_dwData ), &f_poECMBuilderContext->cbBuffer ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _WriteECMData_WORD
**
** Synopsis:    Write a word to ECM builder context's buffer.
**              When the buffer is full it will create TS packets for the
**              ECM to clear more room in the buffer.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poECMBuilderContext]  -- Pointer to the ECM builder context.
** [f_wData]                -- Data to add to the ECM packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _WriteECMData_WORD(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in      const DRM_WORD                             f_wData ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _WriteECMData_WORD(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in      const DRM_WORD                             f_wData )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_poECMBuilderContext != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_poECMBuilderContext != NULL );

    if( f_poECMBuilderContext->cbBuffer > SIZEOF( f_poECMBuilderContext->rgbBuffer ) - SIZEOF( f_wData ) )
    {
        ChkDR( _CreateECMPackets( f_pEncContext,
                                  f_poECMBuilderContext,
                                  FALSE ) );              /* fLastCall */
    }

    WORD_TO_NETWORKBYTES( f_poECMBuilderContext->rgbBuffer, f_poECMBuilderContext->cbBuffer, f_wData );
    ChkDR( DRM_WordAdd( f_poECMBuilderContext->cbBuffer, SIZEOF( f_wData ), &f_poECMBuilderContext->cbBuffer ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _WriteECMData_SUBSAMPLE
**
** Synopsis:    Write a subsample mapping to ECM builder context's buffer.
**              When the buffer is full it will create TS packets for the
**              ECM to clear more room in the buffer.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_poECMBuilderContext]  -- Pointer to the ECM builder context.
** [f_wData1]               -- Clear byte count of a subsample to add to the ECM packet.
** [f_dwData2]              -- Protected byte count of a subsample to add to the ECM packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _WriteECMData_SUBSAMPLE(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in      const DRM_WORD                             f_wData1,
    __in      const DRM_DWORD                            f_dwData2 ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _WriteECMData_SUBSAMPLE(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM_BUILDER_CONTEXT        *f_poECMBuilderContext,
    __in      const DRM_WORD                             f_wData1,
    __in      const DRM_DWORD                            f_dwData2 )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_poECMBuilderContext != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_poECMBuilderContext != NULL );

    if( f_poECMBuilderContext->cbBuffer > SIZEOF( f_poECMBuilderContext->rgbBuffer ) - SIZEOF( f_wData1 ) - SIZEOF( f_dwData2 ) )
    {
        ChkDR( _CreateECMPackets( f_pEncContext,
                                  f_poECMBuilderContext,
                                  FALSE ) );              /* fLastCall */
    }

    WORD_TO_NETWORKBYTES( f_poECMBuilderContext->rgbBuffer, f_poECMBuilderContext->cbBuffer, f_wData1 );
    ChkDR( DRM_WordAdd( f_poECMBuilderContext->cbBuffer, SIZEOF( f_wData1 ), &f_poECMBuilderContext->cbBuffer ) );

    DWORD_TO_NETWORKBYTES( f_poECMBuilderContext->rgbBuffer, f_poECMBuilderContext->cbBuffer, f_dwData2 );
    ChkDR( DRM_WordAdd( f_poECMBuilderContext->cbBuffer, SIZEOF( f_dwData2 ), &f_poECMBuilderContext->cbBuffer ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   _CalculateECMPakcetsNeeded
**
** Synopsis:   Base on the number of PES packets, samples and subsamples
**             in the ECM to calculate the size of the ECM.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pECM]                 -- Pointer to the ECM to calculate its ECM packets size.
** [f_pwSortedIndexes]      -- Array of sorted indexes according to the PID of the 
**                             PES packets contained in the ECM.
** [f_pcStreams]            -- Out parameter to contain the number of streams 
**                             contained in the ECM.
** [f_pcbECM]               -- Out parameter to contain the calculated ECM size 
**                             in bytes.
** [f_pcPackets]            -- Out parameter to contain the number of packets
**                             needed for the ECM.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_ECM_PAYLOAD_OVER_LIMIT when the calcuated
**                              ECM size is over 64k bytes.
**                          DRM_E_M2TS_INTERNAL_ERROR when something unexpected
**                              happened.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CalculateECMPakcetsNeeded(
    __in                                              const DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout                                                 DRM_M2TS_ECM                    *f_pECM,
    __in_ecount( DRM_M2TS_MAX_PES_PACKETS_PER_ECM )   const DRM_WORD                        *f_pwSortedIndexes,
    __inout                                                 DRM_BYTE                        *f_pcStreams,
    __out                                                   DRM_DWORD                       *f_pcbECM,
    __out                                                   DRM_WORD                        *f_pcPackets ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CalculateECMPakcetsNeeded(
    __in                                              const DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout                                                 DRM_M2TS_ECM                    *f_pECM,
    __in_ecount( DRM_M2TS_MAX_PES_PACKETS_PER_ECM )   const DRM_WORD                        *f_pwSortedIndexes,
    __inout                                                 DRM_BYTE                        *f_pcStreams,
    __out                                                   DRM_DWORD                       *f_pcbECM,
    __out                                                   DRM_WORD                        *f_pcPackets )
{
    DRM_RESULT               dr                     = DRM_SUCCESS;
    DRM_DWORD                cbECM                  = 0;
    DRM_BYTE                 cStreams               = 0;
    DRM_DWORD                cPES                   = 0;
    DRM_DWORD                cPESLast               = 0;
    DRM_DWORD                cSamples               = 0;
    DRM_DWORD                cSamplesPerPES         = 0;
    DRM_DWORD                cSubsamples            = 0;
    DRM_DWORD                cSubsamplesPerSample   = 0;
    DRM_DWORD                cbSize                 = 0;
    DRM_WORD                 wCurrentPID            = DRM_M2TS_UNDEFINED_PID;
    DRM_DWORD                iSample                = 0;
    DRM_WORD                 iPES                   = 0;
    DRM_M2TS_PES            *pPES                   = NULL;
    const DRM_M2TS_SAMPLE   *pSample                = NULL;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pECM != NULL );
    DRMASSERT( f_pwSortedIndexes != NULL );
    DRMASSERT( f_pcStreams != NULL );
    DRMASSERT( f_pcbECM != NULL );
    DRMASSERT( f_pcPackets != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pECM != NULL );
    __analysis_assume( f_pwSortedIndexes != NULL );
    __analysis_assume( f_pcStreams != NULL );
    __analysis_assume( f_pcbECM != NULL );
    __analysis_assume( f_pcPackets != NULL );

    /* ECM fixed fields, include packet_start_code_prefix(24), stream_id(8), pes_packet_length(16), ecm_version(4), pssh_count(4) */
    cbECM = 7;

    if( f_pECM->cbPROBlob > 0 )
    {
        /* The PSSH_ECM_DATA includes SystemID(128), DataSize(32) and Data(8*DataSize) */
        ChkDR( DRM_DWordAdd( cbECM, SIZEOF( DRM_GUID ) + SIZEOF( DRM_DWORD ), &cbECM ) );
        ChkDR( DRM_DWordAdd( cbECM, f_pECM->cbPROBlob, &cbECM ) );
    }

    /* Plus 1 for the scrambled_stream_count(8) */
    ChkDR( DRM_DWordIncOne( cbECM ) );

    /* Calculate SENC_ECM_DATA */

    DRMASSERT( f_pECM->cPESPackets <= DRM_M2TS_MAX_PES_PACKETS_PER_ECM );
    ChkBOOL( f_pECM->cPESPackets <= DRM_M2TS_MAX_PES_PACKETS_PER_ECM, DRM_E_M2TS_INTERNAL_ERROR );

    /* Count the PES, total number of samples and subsamples in the ECM */
    for( iPES = 0; iPES < f_pECM->cPESPackets; iPES++ )
    {
        pPES = &f_pECM->rgPESPackets[ f_pwSortedIndexes[ iPES ] ];
        if( pPES->fHasSample && DRM_SUCCEEDED( pPES->drPESResult ) )
        {
            if( wCurrentPID != pPES->wPID )
            {
                if( cPESLast != cPES )
                {
                    ChkDR( DRM_ByteIncOne( cStreams ) );
                    cPESLast = cPES;
                }
                wCurrentPID = pPES->wPID;
            }

            DRMASSERT( pPES->dwIndexLastSample >= pPES->dwIndexFirstSample );

            cSubsamplesPerSample = 0;

            for( iSample = pPES->dwIndexFirstSample; iSample <= pPES->dwIndexLastSample; iSample++ )
            {
                DRM_DWORD cSubsamples2 = 0;

                pSample = &f_pEncContext->pSamples[ iSample ];

                /* There should have least one subsample in the sample */
                DRMASSERT( pSample->fHasSubsample );
                DRMASSERT( pSample->dwIndexLastSubsample >= pSample->dwIndexFirstSubsample );

                DRM_M2TS_SUBSAMPLE_COUNT( pSample, cSubsamples2 );
                ChkDR( DRM_DWordAdd( cSubsamplesPerSample, cSubsamples2, &cSubsamplesPerSample ) );
            }

            DRM_M2TS_SAMPLE_COUNT( pPES, cSamplesPerPES );
            if( cSamplesPerPES > SCHAR_MAX
             || cSubsamplesPerSample > MAX_UNSIGNED_TYPE( DRM_WORD ) )
            {
                /*
                ** The max allowed samples per PES is 127 (sample_count field in ECM is a 7-bit field)
                ** The max allowed subsample per sample is 65,535 (subsample_count in ECM is a 16-bit field)
                */
                pPES->drPESResult = DRM_E_M2TS_TOO_MANY_SUBSAMPLES;
            }
            else
            {
                ChkDR( DRM_DWordAdd( cSamples, cSamplesPerPES, &cSamples ) );
                ChkDR( DRM_DWordAdd( cSubsamples, cSubsamplesPerSample, &cSubsamples ) );

                ChkDR( DRM_DWordIncOne( cPES ) );
            }
        }
    }

    /* include the last stream if it is not empty */
    if( cPESLast != cPES )
    {
        ChkDR( DRM_ByteIncOne( cStreams ) );
    }
    DRMASSERT( cStreams <= *f_pcStreams );

    /*
    ** First the per-stream fields, include scrambling_control(2), streamParamsIncluded(1), PID(13), AlgorithmID(24), IV_size(8), KID(128), PES_count(8).
    ** Always include the stream parameters: AlgorithmID(24), IV_size(8), KID(128)
    */
    ChkDR( DRM_DWordMult( DRM_M2TS_LENGTH_OF_PER_STREAM_FIELDS, cStreams, &cbSize ) );
    ChkDR( DRM_DWordAdd( cbECM, cbSize, &cbECM ) );
    *f_pcStreams = cStreams;

    /* Next the per-PES fields, include PTS_timestamp(33), sample_count(7) */
    ChkDR( DRM_DWordMult( 5, cPES, &cbSize ) );
    ChkDR( DRM_DWordAdd( cbECM, cbSize, &cbECM ) );

    /* Add IV(8*IV_size, here the IV_size is 8 bytes) and subsample_count(16) for each sample */
    ChkDR( DRM_DWordMult( 10, cSamples, &cbSize ) );
    ChkDR( DRM_DWordAdd( cbECM, cbSize, &cbECM ) );

    /* Add BytesOfClearData(16) and BytesOfEncryptedData(32) for each subsample */
    ChkDR( DRM_DWordMult( 6, cSubsamples, &cbSize ) );
    ChkDR( DRM_DWordAdd( cbECM, cbSize, &cbECM ) );

    *f_pcbECM = cbECM;

    /* The size of the ECM payload cannot exceed 64k */
    ChkBOOL( ( cbECM - DRM_M2TS_LENGTH_OF_FIELDS_BEFORE_ECM_SECLEN_FIELD ) <= MAX_UNSIGNED_TYPE( DRM_WORD ), DRM_E_M2TS_ECM_PAYLOAD_OVER_LIMIT );

    /*
    ** Now, cbECM has the max size of the ECM, divid it by DRM_M2TS_MAX_TS_PAYLOAD_SIZE to get the packet count.
    ** Since, we don't need to return the exact packet count, but a packet count that will always be sufficient
    ** so add additional two packets to be sure
    */
    ChkDR( DRM_DWordToWord( cbECM / DRM_M2TS_MAX_TS_PAYLOAD_SIZE + 2, f_pcPackets ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:   _SortPESPackets
**
** Synopsis:   Sort the PES packets inside the ECM according to PID.
**             The sorded results (indexes) are in f_prgNewIndexes.
**             The order of the PES packets of same PID must not be changed.
**             The function also returns the number of stream types in the
**             ECM to the caller.
**
** Arguments:
**
** [f_pECM]                 -- Pointer to the ECM that its PES packets will be sorted
**                             acording to the PID of the PES packets.
** [f_prgNewIndexes]        -- Out parameter to contain the sorted indexes.
** [f_pcStreams]            -- Out parameter to contain the number of streams 
**                             contained in the ECM.
**
** Returns:                 None
**
**********************************************************************/
static DRM_NO_INLINE DRM_VOID _SortPESPackets(
    __in                                          const DRM_M2TS_ECM    *f_pECM,
    __out_ecount( DRM_M2TS_MAX_PES_PACKETS_PER_ECM )    DRM_WORD        *f_prgNewIndexes,
    __out                                               DRM_BYTE        *f_pcStreams ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_VOID _SortPESPackets(
    __in                                          const DRM_M2TS_ECM    *f_pECM,
    __out_ecount( DRM_M2TS_MAX_PES_PACKETS_PER_ECM )    DRM_WORD        *f_prgNewIndexes,
    __out                                               DRM_BYTE        *f_pcStreams )
{
    DRM_BOOL        rgfSorted[ DRM_M2TS_MAX_PES_PACKETS_PER_ECM ]  = { 0 };
    DRM_BYTE        cSorted                                        = 0;
    DRM_WORD        wPID                                           = 0;
    DRM_WORD        iPES                                           = 0;

    DRMASSERT( f_pECM != NULL );
    DRMASSERT( f_prgNewIndexes != NULL );
    DRMASSERT( f_pcStreams != NULL );
    __analysis_assume( f_pECM != NULL );
    __analysis_assume( f_prgNewIndexes != NULL );
    __analysis_assume( f_pcStreams != NULL );

    *f_pcStreams = 0;

    /* For transport stream with one audio and one video, the 'while-loop' iteration should be 2 .  */
    while( cSorted < f_pECM->cPESPackets )
    {
        wPID = DRM_M2TS_UNDEFINED_PID;

        for( iPES = 0; iPES < f_pECM->cPESPackets; iPES++ )
        {
            if( !rgfSorted[ iPES ] )
            {
                if( wPID == DRM_M2TS_UNDEFINED_PID )
                {
                    (*f_pcStreams)++;
                    wPID = f_pECM->rgPESPackets[ iPES ].wPID;
                }

                if( f_pECM->rgPESPackets[ iPES ].wPID == wPID )
                {
                    f_prgNewIndexes[ cSorted++ ] = iPES;
                    rgfSorted[ iPES ] = TRUE;
                }
            }
        }
    }
}

/**********************************************************************
**
** Function:   DRM_M2TS_BuildECMPackets
**
** Synopsis:   Builds ECM packets for the PES packets in the ECM.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pECM]                 -- Pointer to the ECM to build the ECM packets.
** [f_pECMPacketList]       -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_INTERNAL_ERROR when something 
**                              unexpected happened.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_BuildECMPackets(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT          *f_pEncContext,
    __inout         DRM_M2TS_ECM                        *f_pECM,
    __inout         DRM_M2TS_PACKET_LIST                *f_pECMPacketList )
{
    DRM_RESULT                      dr                  = DRM_SUCCESS;
    const DRM_M2TS_PES             *pPES                = NULL;
    const DRM_M2TS_SAMPLE          *pSample             = NULL;
    const DRM_M2TS_SUBSAMPLE       *pSubsample          = NULL;
    DRM_M2TS_ECM_BUILDER_CONTEXT    oECMBuilderContext  = { 0 };
    DRM_WORD                        cEstPackets         = 0;
    DRM_BOOL                        fIncludeStreamParas = FALSE;
    DRM_DWORD                       cbECM               = 0;
    DRM_DWORD                       cbPacketLength      = 0;
    DRM_BYTE                        cStreams            = 0;
    DRM_BYTE                        rgbECMHeader[7]     = ECM_HEADER;
    DRM_BYTE                        rgbSENCHeader[23]   = { 0 };    /*
                                                                    ** For the following fields
                                                                    ** Scrambled_control(2)
                                                                    ** streamParamsIncluded(1)
                                                                    ** wPID(13)
                                                                    ** AlgorithmID(24) if streamParamsIncluded
                                                                    ** IV_size(8) if streamParamsIncluded
                                                                    ** KID(128) if streamParamsIncluded
                                                                    ** PES_count(8)
                                                                    */
    DRM_BYTE                        bSENCHeaderOffset   = 0;
    DRM_UINT64                      qwPTSAndCountCombo  = DRM_UI64HL( 0, 0 );
    DRM_BYTE                        rgSampleHeader[8]   = { 0 };    /* Include PTS_timestamp(33) and sample_count(7) */
    DRM_WORD                        rgwSortedIndexes[ DRM_M2TS_MAX_PES_PACKETS_PER_ECM ] = { 0 };   /* Sorted by the PES's wPID */
    DRM_WORD                        wCurrentPID         = DRM_M2TS_UNDEFINED_PID;
    DRM_BYTE                        cPES                = 0;
    DRM_BYTE                        cPESWithoutError    = 0;
    DRM_WORD                        iPES                = 0;
    DRM_WORD                        iPES2               = 0;
    DRM_DWORD                       iSample             = 0;
    DRM_DWORD                       iSubsample          = 0;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pECM != NULL );
    ChkArg( f_pECMPacketList != NULL );

    fIncludeStreamParas = ( f_pECM->eECMType == eDRM_M2TS_ECM_TYPE_PRIMARY );

    /*
    ** The ECM doesn't have PES packets.  This happens when, for example,
    ** a primary ECM is added and key rotation ECM added right after it
    */
    ChkBOOL( f_pECM->cPESPackets != 0, DRM_S_FALSE );

    AssertChkArg( f_pECM->cPESPackets <= DRM_M2TS_MAX_PES_PACKETS_PER_ECM );

    /* Count the number of PES without error */
    for( iPES = 0; iPES < f_pECM->cPESPackets; iPES++ )
    {
        cPESWithoutError += DRM_SUCCEEDED( f_pECM->rgPESPackets[ iPES ].drPESResult ) ? 1 : 0;
    }

    /* No need to create the ECM if all of the PES packets in the ECM are bad and the content doesn't rotate keys */
    ChkBOOL( cPESWithoutError > 0 || f_pECM->ePROType != eDRM_M2TS_PRO_TYPE_NO_PRO, DRM_S_FALSE );

    if( f_pECM->eECMType != eDRM_M2TS_ECM_TYPE_SECONDARY )
    {
        /* Create the PRO blob if PRO is requested */
        if( f_pECM->ePROType == eDRM_M2TS_PRO_TYPE_KEY_ROTATION_WITH_LICENSE_IN_PRO )
        {
            ChkDR( Drm_LocalLicense_CreatePlayReadyObject( f_pECM->hLicense, NULL, NULL, &f_pECM->cbPROBlob, &f_pECM->pbPROBlob ) );
        }
        else if( f_pECM->ePROType == eDRM_M2TS_PRO_TYPE_KEY_ROTATION_WITH_WRMHEADER_IN_PRO )
        {
            ChkDR( Drm_LocalLicense_CreateRMHeader( (DRM_KID *)&f_pECM->oKeyId, &f_pECM->cbPROBlob, &f_pECM->pbPROBlob ) );
        }
    }

    ChkBOOL( ( f_pECM->pbPROBlob != NULL ) == ( f_pECM->cbPROBlob > 0 ), DRM_E_M2TS_INTERNAL_ERROR );

    /*
    ** Sort the PES packets in the ECM by PID. Can't use the stream type because
    ** we may have more than one audio stream in, for example, different languages
    ** in same stream type
    */
    _SortPESPackets( f_pECM, rgwSortedIndexes, &cStreams );

    /* Make sure we have enough free packets to accommodate the ECM pakcets */
    ChkDR( _CalculateECMPakcetsNeeded( f_pEncContext,
                                       f_pECM,
                                       rgwSortedIndexes,
                                       &cStreams,
                                       &cbECM,
                                       &cEstPackets ) );

    if( cStreams == 0 && f_pECM->cbPROBlob == 0 )
    {
        /* 
        ** All the PES packets in the ECM either don't have samples or are invalid.  This ECM 
        ** doesn't have PSSH data either.  It is okay to remove the entire ECM by not building the ECM.
        ** Note: ECM spec allows an ECM without scrambled stream data, i.e. cStreams equals to 0.
        */
        goto ErrorExit;
    }

    if( f_pEncContext->oFreePacketList.cItems < cEstPackets )
    {
        ChkDR( DRM_M2TS_InitPackets( f_pEncContext, (DRM_DWORD)MAX( DRM_M2TS_PACKETS_TO_ALLOCATE, cEstPackets ) ) );
    }

    /* Let's build the ECM packets now */

    oECMBuilderContext.pEncContext = f_pEncContext;
    oECMBuilderContext.pECM = f_pECM;
    oECMBuilderContext.pPacketList = f_pECMPacketList;

    /* Update pes_packet_lenght(16) */
    cbPacketLength  = cbECM - 6; /* exclude packet_start_code_prefix(24), stream_id(8) and pes_packet_length(16) */
    ChkDR( DRM_DWordToByte( cbPacketLength >> 8, &rgbECMHeader[4] ) );
    ChkDR( DRM_DWordToByte( cbPacketLength & 0xff, &rgbECMHeader[5] ) );

    if( f_pECM->cbPROBlob > 0 )
    {
        DRM_GUID oPRProtectionSystemId = DRM_PR_PROTECTION_SYSTEM_ID;

        /* Set pssh_count(4) to 1 */
        rgbECMHeader[6] |= 0x01;

        /* Write ECM header */
        ChkDR( _WriteECMData_BYTES( f_pEncContext,
                                    &oECMBuilderContext,
                                    0,
                                    SIZEOF( rgbECMHeader ),
                                    rgbECMHeader ) );
        /*
        ** Write protection system specific header for PlayReady
        ** The PlayReady Protection System ID, 9A04F079-9840-4286-AB92-E65BE0885F95,
        ** is always in big endian form.
        */
        MAKE_GUID_BIG_ENDIAN( oPRProtectionSystemId );
        ChkDR( _WriteECMData_BYTES( f_pEncContext,
                                    &oECMBuilderContext,
                                    0,
                                    SIZEOF( DRM_PR_PROTECTION_SYSTEM_ID ),
                                    (const DRM_BYTE *)&oPRProtectionSystemId ) );

        /* Write PRO size(32) */
        ChkDR( _WriteECMData_DWORD( f_pEncContext,
                                    &oECMBuilderContext,
                                    f_pECM->cbPROBlob ) );

        /* Write PRO */
        ChkDR( _WriteECMData_BYTES( f_pEncContext,
                                    &oECMBuilderContext,
                                    0,
                                    f_pECM->cbPROBlob,
                                    f_pECM->pbPROBlob ) );
    }
    else
    {
        /* Write ECM header */
        ChkDR( _WriteECMData_BYTES( f_pEncContext,
                                    &oECMBuilderContext,
                                    0,
                                    SIZEOF( rgbECMHeader ),
                                    rgbECMHeader ) );
    }

    /* Write scrambled scream count */
    ChkDR( _WriteECMData_BYTES( f_pEncContext, &oECMBuilderContext, 0, 1, &cStreams ) );

    /* Write SENC_ECM_DATA */

    for( iPES = 0; iPES < f_pECM->cPESPackets; iPES++ )
    {
        pPES = &f_pECM->rgPESPackets[ rgwSortedIndexes[ iPES ] ];

        if( pPES->fHasSample
         && DRM_SUCCEEDED( pPES->drPESResult ) )
        {
            DRM_DWORD cSampleCount = 0;

            if( pPES->wPID != wCurrentPID )
            {
                MEMSET( rgbSENCHeader, 0, SIZEOF( rgbSENCHeader ) );

                /* Switch to another stream type */
                wCurrentPID = pPES->wPID;

                /* Handle Scrambled_control(2), StreamParasIncluded(1), PID(13) */
                WORD_TO_NETWORKBYTES( rgbSENCHeader, 0, pPES->wPID );
                bSENCHeaderOffset = 2;
                rgbSENCHeader[0] |= f_pEncContext->bScramblingControl << 6;          /* Scrambled_control is the first two msb */

                /* Always include stream parameters */
                rgbSENCHeader[0] |= 0x20;   /* StreamParasIncluded is the third msb */

                /* Set AlgorithID(24) to 0x1 for AES 128-bit in CTR mode */
                MEMCPY( rgbSENCHeader + bSENCHeaderOffset, "\x00\x00\x01", 3 );
                bSENCHeaderOffset += 3;

                /* Set IV_size(8) to 8 bytes */
                rgbSENCHeader[ bSENCHeaderOffset++ ] = IV_SIZE;

                /* Set KID(128) */
                MEMCPY( rgbSENCHeader + bSENCHeaderOffset, &f_pECM->oKeyId, SIZEOF( f_pECM->oKeyId ) );
                bSENCHeaderOffset += SIZEOF( f_pECM->oKeyId );

                /* Count the PES packets first then set it */
                cPES = pPES->fHasSample ? (DRM_BYTE)1 : (DRM_BYTE)0;
                for( iPES2 = (DRM_WORD)( iPES + 1 );
                     iPES2 < f_pECM->cPESPackets && f_pECM->rgPESPackets[ rgwSortedIndexes[ iPES2 ] ].wPID == wCurrentPID;
                     iPES2++ )
                {
                    if( f_pECM->rgPESPackets[ rgwSortedIndexes[ iPES2 ] ].fHasSample
                     && DRM_SUCCEEDED( f_pECM->rgPESPackets[ rgwSortedIndexes[ iPES2 ] ].drPESResult ) )
                    {
                        cPES++;
                    }
                }
                rgbSENCHeader[ bSENCHeaderOffset++ ] = cPES;

                /* Write SENC header */
                DRMASSERT( bSENCHeaderOffset == 3 || bSENCHeaderOffset == SIZEOF( rgbSENCHeader ) );
                ChkDR( _WriteECMData_BYTES( f_pEncContext, &oECMBuilderContext, 0, bSENCHeaderOffset, rgbSENCHeader ) );
            }

            /* Write Sample Header, include PTS_timestamp(33), sample_count 7 */
            DRM_M2TS_SAMPLE_COUNT( pPES, cSampleCount );
            ChkDR( DRM_UInt64Add( DRM_UI64ShL( pPES->qwPTS, 7 ), DRM_UI64HL( 0, cSampleCount ), &qwPTSAndCountCombo ) );
            QWORD_TO_NETWORKBYTES( rgSampleHeader, 0, qwPTSAndCountCombo );
            ChkDR( _WriteECMData_BYTES( f_pEncContext, &oECMBuilderContext, 3, 5, rgSampleHeader ) ); /* write the last 5 bytes */

            /* Write each sample */
            for( iSample = pPES->dwIndexFirstSample; iSample <= pPES->dwIndexLastSample; iSample++ )
            {
                DRM_DWORD cSubsamples = 0;

                pSample = &f_pEncContext->pSamples[ iSample ];

                /* Write IV */
                ChkDR( _WriteECMData_QWORD( f_pEncContext, &oECMBuilderContext, pSample->qwIV ) );

                /* Write subsample_count */
                DRM_M2TS_SUBSAMPLE_COUNT( pSample, cSubsamples );
                ChkOverflow( MAX_UNSIGNED_TYPE( DRM_WORD ), cSubsamples ); /* Check again, even though _CalculateECMPakcetsNeeded has checked */
                ChkDR( _WriteECMData_WORD( f_pEncContext, &oECMBuilderContext, ( DRM_WORD )cSubsamples ) );

                for( iSubsample = pSample->dwIndexFirstSubsample; iSubsample <= pSample->dwIndexLastSubsample; iSubsample++ )
                {
                    pSubsample = &f_pEncContext->pSubsamples[ iSubsample ];

                    /* Write subsample entry */
                    ChkDR( _WriteECMData_SUBSAMPLE( f_pEncContext, &oECMBuilderContext, pSubsample->cbClearBytes, pSubsample->cbEncryptedBytes ) );
                }
            }
        }
    }

    if( oECMBuilderContext.cbBuffer > 0 )
    {
        ChkDR( _CreateECMPackets( f_pEncContext, &oECMBuilderContext, TRUE ) );
    }

    DRMASSERT( 0 == oECMBuilderContext.cbBuffer );
    ChkBOOL( cbECM == oECMBuilderContext.cbWritten, DRM_E_M2TS_INTERNAL_ERROR );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
