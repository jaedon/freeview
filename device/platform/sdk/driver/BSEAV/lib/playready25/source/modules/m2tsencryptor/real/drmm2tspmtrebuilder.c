/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMM2TSPMTREBUILDER_C
#include <drmm2tsbuilders.h>
#include <drmm2tsbuffermgr.h>
#include <drmm2tsparsers.h>
#include <drmm2tsbitstream.h>
#include <drmm2tsmacros.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    DRM_M2TS_RebuildPMTPacket 
**
** Synopsis:    Inserts our CA_descriptor with CA_system_ID = 0x4b13 and
**              CA_PID = 0x1FFE and removes existing CA_descriptors if there
**              is one.  It alos updates the section_length field accordingly.
**              Additional TS packet will be appended if the PMT stream 
**              doesn't have room to accommodate the CA_descriptor.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pStream]              -- PMT stream to be updated with CA_descriptor
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL  DRM_M2TS_RebuildPMTPacket(
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout     DRM_M2TS_STREAM                 *f_pStream )
{
    DRM_RESULT                   dr                                     = DRM_SUCCESS;
    DRM_M2TS_PACKET             *pAppendedPacket                        = NULL;
    DRM_M2TS_PACKET             *pOriginalTail                          = NULL;
    DRM_M2TS_PACKET             *pHead                                  = NULL;
    DRM_WORD                     cbSection                              = 0;
    DRM_WORD                     cbESInfo                               = 0;
    DRM_WORD                     cbProgramInfo                          = 0;
    DRM_BYTE                     rgPacketData[ DRM_M2TS_PACKET_SIZE ]   = { 0 };
    DRM_BYTE                     bStreamType                            = DRM_M2TS_STREAM_TYPE_RESERVED;
    DRM_BOOL                     fFoundCADescriptor                     = FALSE;
    DRM_WORD                     cbESInfoRemaining                      = 0;
    DRM_BOOL                     fExtraPacketAppended                   = FALSE;
    DRM_DWORD                    cbCRCCalculation                       = 0;
    DRM_M2TS_BIT_STREAM_CONTEXT  oBSRContext                            = { 0 };
    DRM_M2TS_BIT_STREAM_CONTEXT  oBSRSectionLengthMarker                = { 0 };
    DRM_M2TS_BIT_STREAM_CONTEXT  oBSRESInfoLengthMarker                 = { 0 };

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pStream != NULL );
    ChkArg( f_pStream->oPacketList.pHead != NULL );

    pOriginalTail = f_pStream->oPacketList.pTail;
    pHead         = f_pStream->oPacketList.pHead;

    /* Remove adaptation field when there is one and the stream has only one packet */
    if( f_pStream->oPacketList.cItems == 1
     && DRM_M2TS_HAS_ADAPTATION_FIELD( pHead->rgbData )
     && pHead->rgbData[ DRM_M2TS_ADAPTATION_LENGTH_FIELD_OFFSET + 1 ] == 0 ) /* Ensure no adaptation flags */
    {
        DRM_BYTE cbAdaptation = pHead->rgbData[ DRM_M2TS_ADAPTATION_LENGTH_FIELD_OFFSET ];
        DRM_BYTE bOffset      = 0;

        DRM_M2TS_REMOVE_ADAPTATION_FIELD( pHead->rgbData );

        ChkDR( DRM_ByteIncOne( cbAdaptation ) );
        ChkDR( DRM_ByteSub( DRM_M2TS_PACKET_SIZE, pHead->cbPayload, &bOffset ) );
        MEMMOVE( &pHead->rgbData[ DRM_M2TS_PACKET_HEADER_SIZE ],
                 &pHead->rgbData[ bOffset ],
                 pHead->cbPayload );

        ChkDR( DRM_ByteSub( DRM_M2TS_PACKET_SIZE, cbAdaptation, &bOffset ) );
        MEMSET( &pHead->rgbData[ bOffset ], DRM_M2TS_STUFFING_BYTE, cbAdaptation );

        pHead->cbPayload = DRM_M2TS_MAX_TS_PAYLOAD_SIZE;
    }

    /* Intialize a PMT packet data to be used in the extra TS packet that will be acquired in the next step. */
    MEMSET( rgPacketData, DRM_M2TS_STUFFING_BYTE, SIZEOF( rgPacketData ) );
    ChkDR( DRM_M2TS_SetTSPacketHeader( FALSE,         /* Unit start indicator */
                                       f_pStream->wPID,
                                       TRUE,          /* Payload flag */
                                       FALSE,         /* Adapation field flag */
                                       0,             /* Continuity counter which will be adjusted before returning to caller */
                                       SIZEOF( rgPacketData ),
                                       rgPacketData ) );                                     

    /* Get a packet from the free list and append it to the PMT stream just in case adding the CA_descriptor may need extra packet */
    ChkDR( DRM_M2TS_GetFreePacket( f_pEncContext,
                                   DRM_M2TS_MAX_TS_PAYLOAD_SIZE,
                                   eDRM_M2TS_PACKET_STATE_READY,
                                   FALSE,           /* assign packet no. */
                                   rgPacketData,
                                   &pAppendedPacket ) );
    DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &f_pStream->oPacketList, pAppendedPacket );
    fExtraPacketAppended = TRUE;

    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, f_pStream->oPacketList.pHead ) );

    ChkDR( DRM_M2TS_BSR_SkipPointerField( &oBSRContext ) );

    /* Skip all the fields prior to the section_length (12 bits) */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 12 ) );

    /* 
    ** Before reading the section_length save BSR context in order to  
    ** update the section_length later
    */
    oBSRSectionLengthMarker = oBSRContext;
    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &cbSection ) );

    /* Set the limit to the number of bytes to read */
    ChkDR( DRM_M2TS_BSR_SetCounter( &oBSRContext, cbSection ) );

    /* Skip all the fields prior to the program_info_length */
    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 7 ) );
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

    /* Read program_info_length(12) */
    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &cbProgramInfo ) );

    /* Skip entire program_info section */
    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, cbProgramInfo ) );

    /* 
    ** Loop for each stream type, if the stream type is supported
    ** replace existing CA_descriptor with ours if it exists
    ** otherwise add our CA_descriptor to ES_info section.
    */
    while( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) >= 9 ) /* stream_type(8) + reserved(3) + elementary_PID(13) + reserved(4) + ES_info_length(12) + CRC(32) */
    {
        ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &bStreamType ) );

        /* Skip reserved(3), elementary_PID(13), reserved(4) */
        ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 2 ) );
        ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

        /* 
        ** Before reading the ES_info length save it in order to update 
        ** ES_info length later
        */
        oBSRESInfoLengthMarker = oBSRContext;
        ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &cbESInfo ) );

        fFoundCADescriptor = FALSE;
        cbESInfoRemaining = cbESInfo;

        /* 
        ** Scan each descriptor if there is CA_descriptor replace it with ours. 
        ** If there are more than one we will replace all with ours.  This however
        ** shouldn't happen in a normal case.
        */
        while( cbESInfoRemaining > 0 )
        {
            DRM_BYTE    bDescriptorTag;
            DRM_BYTE    cbDescriptor;

            ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &bDescriptorTag ) );
            ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &cbDescriptor ) );

            ChkBOOL( cbESInfoRemaining >= 2, DRM_E_M2TS_CA_DESCRIPTOR_LENGTH_INVALID ); /* 2 for the descriptor(8) and descriptor_length(8) */
            ChkBOOL( cbESInfoRemaining - 2 >= cbDescriptor, DRM_E_M2TS_CA_DESCRIPTOR_LENGTH_INVALID );
            ChkDR( DRM_WordSub( cbESInfoRemaining, 2, &cbESInfoRemaining ) );
                    
            if( bDescriptorTag == DRM_M2TS_CA_DESCRIPTOT_TAG
             && DRM_M2TS_IS_SUPPORTED_STEAM_TYPE( bStreamType ) )
            {
                ChkBOOL( cbDescriptor >= DRM_CA_DESCRIPTOR_MIN_SIZE, DRM_E_M2TS_CA_DESCRIPTOR_LENGTH_INVALID );

                /* 
                ** Good, found an existing CA_descriptor 
                ** We can reuse it, just replace the CA_SystemID and CA_PID with ours
                */
                fFoundCADescriptor = TRUE;

                /* Replace the CA_systemID with our system ID (0x4B13) */
                ChkDR( DRM_M2TS_BSR_WriteBitsFromWORD( &oBSRContext, 16, (DRM_WORD)DRM_M2TS_PLAYREADY_CA_SYSTEM_ID ) );

                /* Skip reserved(3) */
                ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 3 ) );

                /* Replace the CA_PID(13) with our CA PID */
                ChkDR( DRM_M2TS_BSR_WriteBitsFromWORD( &oBSRContext, 13, f_pEncContext->wPID_CA ) );

                /* Skip the rest of the ES_info */
                ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, (DRM_DWORD)( cbDescriptor - DRM_CA_DESCRIPTOR_MIN_SIZE ) ) ); /* For the CA_System_ID(16), reserved(3), CA_PID(13) */
            }
            else
            {
                /* Skip the rest of the descriptor */
                ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, cbDescriptor ) );
            }

            ChkDR( DRM_WordSub( cbESInfoRemaining, cbDescriptor, &cbESInfoRemaining ) );
        }

        /* CA_descriptor doesn't exist, add our CA_descriptor */
        if( !fFoundCADescriptor 
         && DRM_M2TS_IS_SUPPORTED_STEAM_TYPE( bStreamType ) )
        {
            /* 
            ** rgCADescruptor contains following fields and values,
            **   descriptor_tag(8) - 0x09
            **   descriptor_length(8) - 0x4
            **   CA_system_ID(16) - 0x4b13
            **   reserved(3) - 0
            **   CA_PID(13) - 0 (place holder for wPID_CA)
            */
            DRM_BYTE rgbCADescriptor[6] = { 0x09, 0x04, DRM_CA_SYSTEM_ID_HIGHBYTE, DRM_CA_SYSTEM_ID_LOWBYTE, 0x00, 0x00 };

            /* Put the wPID_CA to the last two bytes of rgbCADescriptor */
            WORD_TO_NETWORKBYTES( rgbCADescriptor, 4, f_pEncContext->wPID_CA );

            ChkDR( DRM_M2TS_BSR_InsertBytes( &oBSRContext, SIZEOF( rgbCADescriptor ), rgbCADescriptor ) );

            /* Update ES_info length */
            ChkOverflow( (DRM_DWORD) 0x0FFF, (DRM_DWORD)cbESInfo + SIZEOF( rgbCADescriptor ) );
            ChkDR( DRM_M2TS_BSR_WriteBitsFromWORD( &oBSRESInfoLengthMarker, 12, ( DRM_WORD )( cbESInfo + SIZEOF( rgbCADescriptor ) ) ) );

            /* Adjust section_length */
            ChkDR( DRM_WordAdd( cbSection, SIZEOF( rgbCADescriptor ), &cbSection ) );
            ChkOverflow( (DRM_WORD) 0x0FFF, cbSection );

            ChkDR( DRM_WordAdd( f_pStream->cbHeader, SIZEOF( rgbCADescriptor ), &f_pStream->cbHeader ) );
        }
    }

    /* Update section_length */
    ChkDR( DRM_M2TS_BSR_WriteBitsFromWORD( &oBSRSectionLengthMarker, 12, cbSection ) );

    /* 
    ** Recalculate CRC 
    ** CRC calculation starts from the first byte of the table_ID field and ends at the byte before the CRC_32 field.
    ** Reinit the BSR context excluding the adaptation field, so it should point to the table ID field now 
    */
    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, f_pStream->oPacketList.pHead ) );
    ChkDR( DRM_M2TS_BSR_SkipPointerField( &oBSRContext ) );
    ChkDR( DRM_DWordSub( f_pStream->cbHeader, DRM_M2TS_CRC_SIZE, &cbCRCCalculation ) );
    ChkDR( DRM_DWordDecOne( cbCRCCalculation ) );
    ChkDR( DRM_M2TS_BSR_UpdateCRC( &oBSRContext, cbCRCCalculation ) );

    /* Mark each PMT packet as ready and remove the appended packet if it is not needed */
    {
        DRM_M2TS_PACKET* pCurrentPacket = f_pStream->oPacketList.pHead;
        DRM_DWORD cbHeader = f_pStream->cbHeader;

        while( pCurrentPacket != NULL )
        {
            pCurrentPacket->ePacketState = eDRM_M2TS_PACKET_STATE_READY;

            if( cbHeader <= pCurrentPacket->cbPayload )
            {
                if( pCurrentPacket->pNextPerUnit != NULL )
                {
                    /* The appended packet is not used */
                    /* Since we only appended one packet so there should be another packet after it */
                    DRMASSERT( pCurrentPacket->pNextPerUnit->pNextPerUnit == NULL );
                    DRMASSERT( pAppendedPacket == pCurrentPacket->pNextPerUnit );
                }
                else
                {
                    /* Append the packet to the incoming packet list */
                    DRM_M2TS_APPEND_TO_END_OF_LIST( &f_pEncContext->oIncomingPacketList, pAppendedPacket );

                    /* pAppendedPacket to NULL in order to keep the appended packet */
                    pAppendedPacket = NULL;
                }

                break;
            }

            ChkDR( DRM_DWordSub( cbHeader, pCurrentPacket->cbPayload, &cbHeader ) );
            pCurrentPacket = pCurrentPacket->pNextPerUnit;
        }
    }

ErrorExit:

    if( pAppendedPacket != NULL )
    {
        if( fExtraPacketAppended )
        {
            DRM_M2TS_ENSURE_ITEM_ON_PER_UNIT_LIST( &f_pStream->oPacketList, DRM_M2TS_PACKET, pOriginalTail );
            DRM_M2TS_RESET_PER_UNIT_LIST_TO_ORIGINAL_TAIL_NO_FAIL( &f_pStream->oPacketList, DRM_M2TS_PACKET, pOriginalTail );
        }

        /* Put the appended packet back to the free packet list */
        DRM_M2TS_PUSH_TO_TOP_LIST_NO_FAIL( &f_pEncContext->oFreePacketList, pAppendedPacket );
    }

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
