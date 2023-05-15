/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMM2TSHEADERPARSERS_C
#include <drmm2tsparsers.h>
#include <drmm2tsbitstream.h>
#include <drmm2tsbuffermgr.h>
#include <drmm2tsmacros.h>
#include <drmm2tslogs.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define PAT_TABLE_ID                            0x00
#define PMT_TABLE_ID                            0x02
#define NETWORK_PROGRAM_NUMBER                  0x0000
#define PTS_FLAG                                0x02            /* the 2nd lsb is set to indicate the PTS flag */
#define PROGRAM_PID_PAIR_SIZE                   4
#define PAT_OFFSET_AFTER_SECTION_LENGTH_FIELD   4               /* include pointer_field(8), table_id(8), section_syntax_indicator(1), 0(1), reserved(2), section_length(12) */
#define PMT_OFFSET_AFTER_SECTION_LENGTH_FIELD   4               /* include pointer_field(8), table_id(8), section_syntax_indicator(1), 0(1), reserved(2), section_length(12) */
#define PES_OFFSET_AFTER_HEADER_DATA_LENGTH     9               /* inculde fields from packet_start_code_prefix to PES_header_data_length(8) */
#define PAT_OR_PMT_CRC_FIELD_LENGTH             4


/**********************************************************************
**
** Function:    DRM_M2TS_ParseTSPacketHeader
**
** Synopsis:    Parse the TS pakcet header (4 bytes) and return  below fields:
**              1) PID
**              2) Unit start flag
**              3) Continuity counter
**
** Arguments:
**
** [f_pPacket]              -- Pointer to a packet to parse its packet data.
** [f_pwPID]                -- Out parameter to contain the parsed PID.
** [f_pfUnitStart]          -- Out parameter to contain a flag to indicate
**                             whether the packet is a unit start.
** [f_pbContinuityCounter]  -- Out parameter to contain the parsed continuity
**                             counter value.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_PACKET_SYNC_BYTE_INVALID if the packet
**                              doesn't start with the TS packet's sync byte.
**                          DRM_E_M2TS_ADAPTATION_LENGTH_INVALID if the packet
**                              contains an invalid adaptation field lenght.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_ParseTSPacketHeader( 
    __inout                 DRM_M2TS_PACKET     *f_pPacket,
    __out_ecount( 1 )       DRM_WORD            *f_pwPID,
    __out_ecount( 1 )       DRM_BOOL            *f_pfUnitStart,
    __out_ecount( 1 )       DRM_BYTE            *f_pbContinuityCounter )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkArg( f_pPacket != NULL );
    ChkArg( f_pwPID != NULL );
    ChkArg( f_pfUnitStart != NULL );
    ChkArg( f_pbContinuityCounter != NULL );

    ChkBOOL( f_pPacket->rgbData[0] == DRM_M2TS_START_CODE, DRM_E_M2TS_PACKET_SYNC_BYTE_INVALID );

    /* 
    ** The packet header has following fields:
    ** sync_byte(8)
    ** transport_error(1)
    ** unit_start(1)
    ** priority_flag(1)
    ** PID(13)
    ** scrabling_control(2)
    ** adaptation_flag(1)
    ** payload_flag(1)
    ** continuity_counter(4) 
    */

    *f_pfUnitStart          = DRM_M2TS_IS_UNIT_START_PACKET( f_pPacket->rgbData );
    *f_pwPID                = DRM_M2TS_GET_PID( f_pPacket->rgbData );
    *f_pbContinuityCounter  = DRM_M2TS_GET_CONTINUITY_COUNTER( f_pPacket->rgbData );

    if( DRM_M2TS_HAS_ADAPTATION_FIELD( f_pPacket->rgbData ) )
    {
        DRM_BYTE cbAdaptation = f_pPacket->rgbData[4];
        ChkBOOL( cbAdaptation < DRM_M2TS_MAX_TS_PAYLOAD_SIZE, DRM_E_M2TS_ADAPTATION_LENGTH_INVALID );
        f_pPacket->cbPayload = DRM_M2TS_HAS_PAYLOAD( f_pPacket->rgbData ) ? (DRM_BYTE)( DRM_M2TS_MAX_TS_PAYLOAD_SIZE - cbAdaptation - 1 ) : (DRM_BYTE)0;

        if( ( f_pPacket->rgbData[5] & DRM_M2TS_DISCONTINUITY_INDICATOR_MASK ) != 0 )
        {
            f_pPacket->wPacketFlags |= DRM_M2TS_PACKET_FLAG_DISCONTINUITY_PACKET;
        }
    }
    else
    {
        f_pPacket->cbPayload = DRM_M2TS_HAS_PAYLOAD( f_pPacket->rgbData ) ? (DRM_BYTE)DRM_M2TS_MAX_TS_PAYLOAD_SIZE : (DRM_BYTE)0;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_SetTSPacketHeader
**
** Synopsis:    Initialize the packet header with the values specified 
**              in the input arguments.  The transport_error_indicator,
**              transport_priority, transpoprt_scrambling_control's value
**              will be set to 0.
**
** Arguments:
**
** [f_fUnitStart]           -- Flag to indicate whether the packet is unit start.
** [f_wPID]                 -- PID of the packet.
** [f_fPayload]             -- Flag to inidcate whether the packet has payload.
** [f_fAdaptation]          -- Flag to indicate whether the packet had adaptation field.
** [f_bContinuityCounter]   -- The continuity counter of the packet.
** [f_cbPacketHeader]       -- The length of the packet header buffer in bytes.
** [f_pbPacketHeader]       -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_SetTSPacketHeader(
    __in                          const DRM_BOOL             f_fUnitStart,
    __in                          const DRM_WORD             f_wPID,
    __in                          const DRM_BOOL             f_fPayload,
    __in                          const DRM_BOOL             f_fAdaptation,
    __in                          const DRM_BYTE             f_bContinuityCounter,
    __in                          const DRM_DWORD            f_cbPacketHeader,
    __out_bcount( f_cbPacketHeader )    DRM_BYTE            *f_pbPacketHeader )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_BYTE    bPID        = 0;
            
    ChkArg( f_wPID <= DRM_M2TS_MAX_PID_VALUE );
    ChkArg( f_cbPacketHeader >= DRM_M2TS_PACKET_HEADER_SIZE );
    ChkArg( f_pbPacketHeader != NULL );
    DRMASSERT( f_bContinuityCounter >= 0 && f_bContinuityCounter <= 15 );  /* Continuity counter is a 4-bit field, value from 0 to 15 */

    MEMSET( f_pbPacketHeader, 0, DRM_M2TS_PACKET_HEADER_SIZE );

    /* Set the start code 0x47 */
    f_pbPacketHeader[0] = DRM_M2TS_START_CODE;

    /* Set the PID */
    bPID = (DRM_BYTE)( f_wPID >> BITS_PER_BYTE );
    f_pbPacketHeader[1] |= bPID;     /* the left 5 bits */
    bPID = ( DRM_BYTE )f_wPID;
    f_pbPacketHeader[2] = bPID;      /* the right 8 bits */

    /* Set the unit start flag */
    if( f_fUnitStart )
    {
        f_pbPacketHeader[1] |= 0x40;
    }

    /* Set the continuity counter */
    f_pbPacketHeader[3] |= f_bContinuityCounter;

    /* Set the payload flag */
    if( f_fPayload )
    {
        f_pbPacketHeader[3] |= 0x10;
    }

    /* Set the adaptation field flag */
    if( f_fAdaptation )
    {
        f_pbPacketHeader[3] |= 0x20;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _HandleStreamTypeChanged 
**
** Synopsis:    We are in the transition of changing the stream type.
**              If the original type is an audio or video stream then drop
**              the existing and incomplete PES packets.  If the original 
**              type is a PAT or PMT then mark them as ready to unblock 
**              encryption.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pStream]              -- Pointer to a stream that is about to change
**                             its stream type.
** [f_eNewPacketType]       -- The new packet type of the stream.
** [f_wNewPID]              -- The new PID of the stream.
** [f_bNewStreamType]       -- The new stream type of the stream.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL _HandleStreamTypeChanged(
    __in      const DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_STREAM                 *f_pStream,
    __in      const DRM_M2TS_PACKET_TYPE             f_eNewPacketType,
    __in      const DRM_WORD                         f_wNewPID,
    __in      const DRM_BYTE                         f_bNewStreamType ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL _HandleStreamTypeChanged(
    __in      const DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_STREAM                 *f_pStream,
    __in      const DRM_M2TS_PACKET_TYPE             f_eNewPacketType,
    __in      const DRM_WORD                         f_wNewPID,
    __in      const DRM_BYTE                         f_bNewStreamType )
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_M2TS_STREAM *pNextStreamSaved   = NULL;

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pStream != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pStream != NULL );

    if( DRM_M2TS_IS_PES( f_pStream->ePacketType ) )
    {
        /* Drop last PES and current PES */

        if( f_pStream->pLastPES != NULL )
        {
            f_pStream->pLastPES->ePESState     = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;
            f_pStream->pLastPES->drPESResult   = DRM_E_M2TS_STREAM_OR_PACKET_TYPE_CHANGED;
            f_pStream->pLastPES->fHasSample    = FALSE;
        }

        if( f_pStream->pCurrentPES != NULL )
        {
            f_pStream->pCurrentPES->oPacketList = f_pStream->oPacketList;
            f_pStream->pCurrentPES->ePESState   = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;
            f_pStream->pCurrentPES->drPESResult = DRM_E_M2TS_STREAM_OR_PACKET_TYPE_CHANGED;
            f_pStream->pCurrentPES->fHasSample  = FALSE;
        }
        else
        {
            DRM_M2TS_PES oPES = { 0 };

            /* 
            ** Current PES hasn't been created yet, use a temporary PES in order to log the event 
            ** and unblock the encryption if we do have packets on the packet list
            */
            oPES.oPacketList = f_pStream->oPacketList;
            oPES.wPID        = f_pStream->wPID;
            oPES.bStreamType = f_pStream->bStreamType;

            /* Drop the entire PES by marking each packet as bad on the packet list of the PES */
            DRM_M2TS_SET_PACKET_STATE_ON_PERUNIT_LIST( DRM_M2TS_PACKET, &oPES.oPacketList, eDRM_M2TS_PACKET_STATE_BAD );

            /* Log an event for the PES that is going to be dropped */
            (DRM_VOID)DRM_M2TS_LogEventWithPES( f_pEncContext, eDRM_M2TS_LOG_CATEGORY_WARNING, TRUE, DRM_E_M2TS_STREAM_OR_PACKET_TYPE_CHANGED, &oPES );
        }
    }
    else
    {
        /* PAT or PMT Packet, mark them as ready to unblock the encryption */
        DRM_M2TS_SET_PACKET_STATE_ON_PERUNIT_LIST( DRM_M2TS_PACKET, &f_pStream->oPacketList, eDRM_M2TS_PACKET_STATE_READY );
    }

    /* Reset stream's state */

    pNextStreamSaved                = f_pStream->pNext;

    MEMSET( f_pStream, 0, SIZEOF( DRM_M2TS_STREAM ) );

    f_pStream->pNext                = pNextStreamSaved;

    f_pStream->wPID                 = f_wNewPID;
    f_pStream->bStreamType          = f_bNewStreamType;
    f_pStream->fFoundFirstUnitStart = FALSE;
    f_pStream->ePacketType          = f_eNewPacketType;

    return dr;
}

/**********************************************************************
**
** Function:    _ParsePATHeader
**
** Synopsis:    Parse the PAT header's program numbers and the associated
**              PIDs if we have the entire section header.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pStream]              -- Pointer to a PAT stream.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_TABLE_ID_INVALID if the PAT table
**                              ID is invalid.
**                          DRM_E_M2TS_SECTION_LENGTH_INVALID if the section
**                              length is invalid.
**                          DRM_E_M2TS_PAT_HEADER_INVALID if the section 
**                              header is invalid.
**                          DRM_E_M2TS_CRC_FIELD_INVALID if the CRC field
**                              is invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ParsePATHeader( 
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __inout     DRM_M2TS_STREAM             *f_pStream ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ParsePATHeader( 
    __inout     DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __inout     DRM_M2TS_STREAM             *f_pStream ) 
{
    DRM_RESULT                  dr                  = DRM_SUCCESS;
    DRM_WORD                    cbSection           = 0;
    DRM_DWORD                   cbCRCCalculation    = 0;
    DRM_BYTE                    bTableId            = DRM_M2TS_UNDEFINED_TABLE_ID;
    DRM_BYTE                    bCurrentNextFlag    = DRM_M2TS_CURRENT_PAT_OR_PMT;
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContext         = { 0 };

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pStream != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pStream != NULL );

    AssertChkArg( f_pStream->oPacketList.cItems >= 1 ); 

    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, f_pStream->oPacketList.pHead ) );

    /* Do nothing if we don't have the section length field */
    if( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) >= PAT_OFFSET_AFTER_SECTION_LENGTH_FIELD )
    {
        /* Skip pointer_field(8) and bytes specified by the pointer_field */
        ChkDR( DRM_M2TS_BSR_SkipPointerField( &oBSRContext ) );

        ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &bTableId ) );
        ChkBOOL( bTableId == PAT_TABLE_ID, DRM_E_M2TS_TABLE_ID_INVALID );

        /* Skip section_syntax_indicator(1), 0(1), reserved(2) */
        ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

        /* Get section_length */
        ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &cbSection ) );
        ChkBOOL( cbSection >= DRM_M2TS_MINIMUN_PAT_SECTION_LENGTH, DRM_E_M2TS_SECTION_LENGTH_INVALID );
        ChkDR( DRM_WordAdd( cbSection, PAT_OFFSET_AFTER_SECTION_LENGTH_FIELD, &f_pStream->cbHeader ) );

        /* Do nothing if we don't have the entire header */
        if( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) >= cbSection )
        {
            DRM_M2TS_STREAM     *pStream        = NULL;
            DRM_WORD             programNumber  = 0;
            DRM_WORD             wPID           = DRM_M2TS_UNDEFINED_PID; 

            /* 
            ** Good, we have all the bytes for the section header.
            ** Now, parse the program number and PMT's PID.
            ** Set a limit of bytes to read to cbSection.
            */
            ChkDR( DRM_M2TS_BSR_SetCounter( &oBSRContext, cbSection ) );

            /* Skip transport_stream_id(16), reserved(2), version_no(5) */
            ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 23 ) );

            /* Read current_next_indicator(1) */
            ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 1, &bCurrentNextFlag ) );

            /* Skip section_number(8), last_section_number(8) */
            ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 16 ) );

            while( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) >= PROGRAM_PID_PAIR_SIZE + DRM_M2TS_CRC_SIZE )
            {
                ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 16, &programNumber ) );

                /* Skip reserved(3) */
                ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 3 ) );

                ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 13, &wPID ) );
                ChkDR( DRM_M2TS_RegisterPID( f_pEncContext, wPID ) );

                if( programNumber != NETWORK_PROGRAM_NUMBER
                 && bCurrentNextFlag == DRM_M2TS_CURRENT_PAT_OR_PMT )
                {
                    ChkDR( DRM_M2TS_CreateStream( f_pEncContext, wPID, eDRM_M2TS_PACKET_TYPE_PMT, &pStream ) );
                    if( pStream->ePacketType != eDRM_M2TS_PACKET_TYPE_PMT )
                    {
                        /* Packet type changed */
                        ChkDR( _HandleStreamTypeChanged( f_pEncContext, pStream, eDRM_M2TS_PACKET_TYPE_PMT, wPID, DRM_M2TS_STREAM_TYPE_RESERVED ) );
                    }
                }
            }

            /* Make sure there are 4 bytes left for the CRC */
            ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == DRM_M2TS_CRC_SIZE, DRM_E_M2TS_PAT_HEADER_INVALID );

            /* Validate the CRC */
            ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, f_pStream->oPacketList.pHead ) );
            ChkDR( DRM_M2TS_BSR_SkipPointerField( &oBSRContext ) );
            ChkDR( DRM_DWordSub( f_pStream->cbHeader, DRM_M2TS_CRC_SIZE, &cbCRCCalculation ) );
            ChkDR( DRM_DWordDecOne( cbCRCCalculation ) ); /* -1 for the poiner field */
            ChkDR( DRM_M2TS_BSR_ValidateCRC( &oBSRContext, cbCRCCalculation ) ); 

            /* Set it directly to "has full payload" because PAT only has the section data */
            f_pStream->ePESState = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;
        }
    }

    if( f_pStream->ePESState == eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD )
    {
        /* 
        ** The PAT is completed now. Since there are no addition work needs 
        ** to be done, mark each PAT packet as ready 
        */
        DRM_M2TS_SET_PACKET_STATE_ON_PERUNIT_LIST( DRM_M2TS_PACKET, &f_pStream->oPacketList, eDRM_M2TS_PACKET_STATE_READY );
    }

ErrorExit:
    if( dr == DRM_E_BUFFER_BOUNDS_EXCEEDED )
    {
        dr = DRM_E_M2TS_PAT_HEADER_INVALID;
    }
    return dr;
}

/**********************************************************************
**
** Function:    _AddCAPIDToRemovableList
**
** Synopsis:    Add other CA system's PID to removable list if it is not
**              on the list already.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_wPID_CA]              -- CA_PID to be added to the removable list.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_OUTOFMEMORY when out of memory.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _AddCAPIDToRemovableList( 
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __in      const DRM_WORD                         f_wPID_CA ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _AddCAPIDToRemovableList( 
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __in      const DRM_WORD                         f_wPID_CA ) 
{
    DRM_RESULT                       dr             = DRM_SUCCESS;
    DRM_BOOL                         fFound         = FALSE;
    const DRM_M2TS_REMOVABLE_PID    *pRemovablePID  = NULL;

    DRMASSERT( f_pEncContext != NULL );
    __analysis_assume( f_pEncContext != NULL );

    pRemovablePID = f_pEncContext->oRemovablePIDList.pHead;
    while( pRemovablePID != NULL )
    {
        if( pRemovablePID->wPID == f_wPID_CA )
        {
            fFound = TRUE;
            break;
        }
        pRemovablePID = pRemovablePID->pNext;
    }

    if( !fFound )
    {
        ChkDR( DRM_M2TS_CreateRemovablePID( f_pEncContext, f_wPID_CA ) );
    }

ErrorExit:
    return dr;                   
}

/**********************************************************************
**
** Function:    _ParsePMTHeader
**
** Synopsis:    Parse the PMT header's stream type, PID of the stream  
**              and PID of the existing CA_descriptor that will be removed 
**              later if we have the entire section header.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pStream]              -- Pointer to a PMT stream.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_TABLE_ID_INVALID if the PAT table
**                              ID is invalid.
**                          DRM_E_M2TS_SECTION_LENGTH_INVALID if the section
**                              length is invalid.
**                          DRM_E_M2TS_PROGRAM_INFO_LENGTH_INVALID if the  
**                              program info length is invalid.
**                          DRM_E_M2TS_CA_DESCRIPTOR_LENGTH_INVALID if the
**                              CA_descriptor length is invalid.
**                          DRM_E_M2TS_CRC_FIELD_INVALID if the CRC field
**                              is invalid.
**                          DRM_E_M2TS_PMT_HEADER_INVALID if the PMT header
**                              is invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ParsePMTHeader( 
    __in        DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __inout     DRM_M2TS_STREAM             *f_pStream ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ParsePMTHeader( 
    __in        DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __inout     DRM_M2TS_STREAM             *f_pStream ) 
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_WORD                    cbSection       = 0;
    DRM_BYTE                    bTableId        = DRM_M2TS_UNDEFINED_TABLE_ID;
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContext     = { 0 };

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pStream != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pStream != NULL );

    AssertChkArg( f_pStream->oPacketList.cItems >= 1 ); 

    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, f_pStream->oPacketList.pHead ) );

    /* Do nothing if we don't have the section length field */
    if( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) >= PMT_OFFSET_AFTER_SECTION_LENGTH_FIELD )
    {
        /* Skip pointer_field(8) and bytes specified by the pointer_field */
        ChkDR( DRM_M2TS_BSR_SkipPointerField( &oBSRContext ) );

        ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &bTableId ) );
        ChkBOOL( bTableId == PMT_TABLE_ID, DRM_E_M2TS_TABLE_ID_INVALID );

        /* Skip section_syntax_indicator(1), 0(1), reserved(2) */
        ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

        /* Get section_length */
        ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &cbSection ) );
        ChkBOOL( cbSection >= DRM_M2TS_MINIMUN_PMT_SECTION_LENGTH, DRM_E_M2TS_SECTION_LENGTH_INVALID );
        ChkDR( DRM_WordAdd( cbSection, PMT_OFFSET_AFTER_SECTION_LENGTH_FIELD, &f_pStream->cbHeader ) );

        /* Do nothing if we don't have the entire header */
        if( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) >= cbSection )
        {
            DRM_M2TS_STREAM         *pAudioVideoStream      = NULL;
            DRM_WORD                 cbProgramInfo          = 0;
            DRM_WORD                 cbESInfo               = 0;
            DRM_WORD                 cbDescriptor           = 0;
            DRM_DWORD                cbCRCCalculation       = 0;
            DRM_BYTE                 bStreamType            = DRM_M2TS_STREAM_TYPE_RESERVED;
            DRM_BYTE                 bCurrentNextFlag       = DRM_M2TS_CURRENT_PAT_OR_PMT;
            DRM_BYTE                 bDescriptorTag         = 0;
            DRM_WORD                 wPID                   = DRM_M2TS_UNDEFINED_PID; 
            DRM_WORD                 wPID_CA                = DRM_M2TS_UNDEFINED_PID;
            DRM_WORD                 wPCR_PID               = DRM_M2TS_UNDEFINED_PID;
            DRM_BOOL                 fSupportedStreamType   = FALSE;

            /*
            ** Good, we have all the bytes for the section header.
            ** Now, parse the audio and video stream data 
            */

            /* Set a limit of bytes to read to cbSection */
            ChkDR( DRM_M2TS_BSR_SetCounter( &oBSRContext, cbSection ) );

            /* Skip program_number(16), reserved(2), version_no(5) */
            ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 23 ) );

            /* current_next_indicator(1) */
            ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 1, &bCurrentNextFlag ) );

            /* Skip section_number(8), last_section_number(8), reserved(3) */
            ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 19 ) );

            /* PCR_PID(13) */
            ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 13, &wPCR_PID ) );
            ChkDR( DRM_M2TS_RegisterPID( f_pEncContext, wPCR_PID ) );

            /* Skip reserved(4) */
            ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

            /* Skip the program info section */
            ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &cbProgramInfo ) );
            ChkBOOL( cbSection - DRM_M2TS_MINIMUN_PMT_SECTION_LENGTH >= cbProgramInfo, DRM_E_M2TS_PROGRAM_INFO_LENGTH_INVALID );
            ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, cbProgramInfo ) );

            /* Loop for each stream type and ES_Info */
            while( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) >= 9 ) /* stream_type(8) + reserved(3) + elementary_PID(13) + reserved(4) + ES_info_length(12) + CRC(32) */
            {
                ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &bStreamType ) );

                fSupportedStreamType = DRM_M2TS_IS_SUPPORTED_STEAM_TYPE( bStreamType );

                /* Skip reserved(3) */
                ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 3 ) );

                ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 13, &wPID ) );
                ChkDR( DRM_M2TS_RegisterPID( f_pEncContext, wPID ) );

                /* Skip reserved(4) */
                ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

                ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &cbESInfo ) );

                /* Loop for each descriptor in the ES_info field */
                while( cbESInfo > 0 )
                {
                    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &bDescriptorTag ) );
                    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 8, &cbDescriptor ) );  /* descriptor_length */
                    ChkBOOL( cbESInfo >= 2, DRM_E_M2TS_CA_DESCRIPTOR_LENGTH_INVALID ); /* 2 for the above two fields read, bDescriptorTag and cbDescriptor */
                    ChkBOOL( cbESInfo - 2 >= cbDescriptor, DRM_E_M2TS_CA_DESCRIPTOR_LENGTH_INVALID );
                    if( bDescriptorTag == DRM_M2TS_CA_DESCRIPTOT_TAG )
                    {
                        ChkBOOL( cbDescriptor >= DRM_CA_DESCRIPTOR_MIN_SIZE, DRM_E_M2TS_CA_DESCRIPTOR_LENGTH_INVALID );
                        ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 19 ) );  /* CA_system_ID(16) and reserved(3) */
                        ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 13, &wPID_CA ) );
                        ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, (DRM_DWORD)( cbDescriptor - DRM_CA_DESCRIPTOR_MIN_SIZE ) ) );  /* Skip the rest of the descriptor */

                        ChkDR( DRM_M2TS_RegisterPID( f_pEncContext, wPID_CA ) );

                        if( fSupportedStreamType
                         && bCurrentNextFlag == DRM_M2TS_CURRENT_PAT_OR_PMT )
                        {
                            ChkDR( _AddCAPIDToRemovableList( f_pEncContext, wPID_CA ) );
                        }
                    }
                    else
                    {
                        /* Not a CA_Descriptor or supported stream type, skip the rest of the CA_descriptor */
                        ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, cbDescriptor ) );
                    }

                    ChkDR( DRM_WordSub( cbESInfo, cbDescriptor, &cbESInfo ) );
                    ChkDR( DRM_WordSub( cbESInfo, 2, &cbESInfo ) ); /* 2 for the descriptor_tag(8) and descriptor_length(8) */
                }

                /* 
                ** Encrypt the stream if it is a supported video stream or a supported audio stream and the 
                ** caller wants the audio stream be encrypted too.
                */
                if( fSupportedStreamType 
                 && bCurrentNextFlag == DRM_M2TS_CURRENT_PAT_OR_PMT
                 && ( DRM_M2TS_IS_VIDEO_STREAM( bStreamType ) || f_pEncContext->fEncryptAudio ) )
                {
                    DRM_M2TS_PACKET_TYPE ePacketType = DRM_M2TS_IS_VIDEO_STREAM( bStreamType ) ? eDRM_M2TS_PACKET_TYPE_PES_VIDEO : eDRM_M2TS_PACKET_TYPE_PES_AUDIO;
                    /* Add the audio or video stream if it doesn't exist */
                    ChkDR( DRM_M2TS_CreateStream( f_pEncContext, 
                                                  wPID,
                                                  ePacketType,
                                                  &pAudioVideoStream ) ); 

                    /* if the stream type is reserved which means this is a new stream */
                    if( DRM_M2TS_STREAM_TYPE_RESERVED == pAudioVideoStream->bStreamType )
                    {
                        pAudioVideoStream->bStreamType = bStreamType;
                    }
                    else if( pAudioVideoStream->bStreamType != bStreamType )
                    {
                        /* Stream type changed */
                        ChkDR( _HandleStreamTypeChanged( f_pEncContext, pAudioVideoStream, ePacketType, wPID, bStreamType ) );
                    }
                }
            }

            /* Make sure there are 4 bytes left for the CRC */
            ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == DRM_M2TS_CRC_SIZE, DRM_E_M2TS_PMT_HEADER_INVALID );

            /* Validate the CRC */
            ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, f_pStream->oPacketList.pHead ) );
            ChkDR( DRM_M2TS_BSR_SkipPointerField( &oBSRContext ) );
            ChkDR( DRM_DWordSub( f_pStream->cbHeader, DRM_M2TS_CRC_SIZE, &cbCRCCalculation ) );
            ChkDR( DRM_DWordDecOne( cbCRCCalculation ) ); /* -1 for the poiner field */
            ChkDR( DRM_M2TS_BSR_ValidateCRC( &oBSRContext, cbCRCCalculation ) );

            ChkDR( DRM_M2TS_SetECMPID( f_pEncContext ) );

            /* Set it directly to "has full payload" because PAT has only the section data */
            f_pStream->ePESState = eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD;
        }
    }

ErrorExit:
    if( dr == DRM_E_BUFFER_BOUNDS_EXCEEDED )
    {
        dr = DRM_E_M2TS_PMT_HEADER_INVALID;
    }
    return dr;
}

/**********************************************************************
**
** Function:    _ParsePESHeader
**
** Synopsis:    Parse the PES header to get the data_alignment_flag,
**              PTS and update PES_scrambling_control if we have the
**              entire PES header.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pStream]              -- Pointer to a audio or video PES stream.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_M2TS_PES_START_CODE_NOT_FOUND if the PES doesn't
**                              start with the PES start code.
**                          DRM_E_M2TS_PES_HEADER_INVALID if the PES header
**                              is invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ParsePESHeader( 
    __in      const DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __inout         DRM_M2TS_STREAM             *f_pStream ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _ParsePESHeader( 
    __in      const DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __inout         DRM_M2TS_STREAM             *f_pStream ) 
{
    DRM_RESULT                  dr                  = DRM_SUCCESS;
    DRM_WORD                    cbPESHeaderData     = 0;
    DRM_BYTE                    bPTS_DTS_flags      = 0;
    DRM_DWORD                   dwPESSartCode       = 0;
    DRM_WORD                    wPTS                = 0;
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContext         = { 0 };

    DRMASSERT( f_pEncContext != NULL );
    DRMASSERT( f_pStream != NULL );
    __analysis_assume( f_pEncContext != NULL );
    __analysis_assume( f_pStream != NULL );

    AssertChkArg( f_pStream->oPacketList.cItems >= 1 ); 

    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, f_pStream->oPacketList.pHead ) );

    /* Do nothing if we don't have the PES_header_data_length field */
    if( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) >= PES_OFFSET_AFTER_HEADER_DATA_LENGTH )
    {
        /* verify the PES start code 0x000001 */
        ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 24, &dwPESSartCode ) );
        ChkBOOL( dwPESSartCode == 1, DRM_E_M2TS_PES_START_CODE_NOT_FOUND );

        /* skip stream_id(8) */
        ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 1 ) );

        ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 16, &f_pStream->cbPESPacketLength ) );

        /* Audio stream should always have the PES packet length */
        if( !DRM_M2TS_IS_VIDEO_STREAM( f_pStream->bStreamType ) 
         && f_pStream->cbPESPacketLength == 0 )
        {
            /* Just log the event, the encryptor can continue without the packet length,  */
            (DRM_VOID)DRM_M2TS_LogEventWithPacket( f_pEncContext, eDRM_M2TS_LOG_CATEGORY_WARNING, FALSE, DRM_E_M2TS_PES_PACKET_LENGTH_NOT_SPECIFIED, f_pStream->oPacketList.pHead );
        }

        /* Skip '10'(2) */
        ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 2 ) );

        /* Update scrambling_control */
        ChkDR( DRM_M2TS_BSR_WriteBitsFromBYTE( &oBSRContext, 2, f_pEncContext->bScramblingControl ) );

        /* Skip PES_priority(1) */
        ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 1 ) );

        ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 1, &f_pStream->bDataAlignmentFlag ) );

        /* Skip copyright(1), original_or_copy(1) */
        ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 2 ) );

        ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 2, &bPTS_DTS_flags ) );
        
        /* Skip ESCR_flag(1), ES_rate_flag(1), DSM_trick_mode_flag(1), additional_copy_info_flag(1), PES_CRC_flag(1), PES_extension_flag(1) */
        ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 6 ) );

        /* Get PES_header_data_length */
        ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 8, &cbPESHeaderData ) );
        ChkDR( DRM_WordAdd( cbPESHeaderData, PES_OFFSET_AFTER_HEADER_DATA_LENGTH, &f_pStream->cbHeader ) );

        /* Do nothing if we don't have the entire header */
        if( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) >= cbPESHeaderData )
        {
            /* If the PES header data lenght is zero there is not PTS field in the PES header so the PTS will be zero */
            if( cbPESHeaderData > 0 )
            {
                DRM_BYTE iByte;

                /* Good, we have all the bytes for the section header. */

                /* Set a limit of bytes to read to cbPESHeaderData */
                ChkDR( DRM_M2TS_BSR_SetCounter( &oBSRContext, cbPESHeaderData ) );

                /* Skip 0010(4) or 0011(4) */
                ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 4 ) );

                /* Now, parse the PTS */
                if( ( bPTS_DTS_flags & PTS_FLAG ) == 0 )
                {
                    ChkDR( DRM_E_M2TS_PTS_NOT_EXIST );
                }

                /* Read PTS bit 32-30 */
                ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 3, &wPTS ) );
                f_pStream->qwPTS = DRM_UI64HL( 0, wPTS );
                ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 1 ) );  /* skip marker_bit(1) */

                /* Read PTS bit 29-15 first then bit 14-0 */
                for( iByte = 0; iByte < 2; iByte++ )
                {
                    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 15, &wPTS ) );
                    f_pStream->qwPTS = DRM_UI64Add( DRM_UI64ShL( f_pStream->qwPTS, 15 ),  DRM_UI64HL( 0, wPTS ) );

                    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 1 ) ); /* skip marker_bit(1) */
                }
            }

            f_pStream->ePESState = eDRM_M2TS_PES_STATE_HAS_SECTION_HEADER;
        }
    }

ErrorExit:
    if( dr == DRM_E_BUFFER_BOUNDS_EXCEEDED )
    {
        dr = DRM_E_M2TS_PES_HEADER_INVALID;
    }
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_ParsePacketHeader
**
** Synopsis:    Parses the PAT, PMT and PES' header and updates the cbHeader
**              field in each of the packet on the per unit chain.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_pStream]              -- Pointer to a stream.
** [f_pPacket]              -- Pointer to a packet tp be parsed.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_NOT_UNIT_START_PACKET if the PES
**                              doesn't start with a unit start packet.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_M2TS_ParsePacketHeader(  
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_STREAM                 *f_pStream,
    __inout         DRM_M2TS_PACKET                 *f_pPacket )
{
    DRM_RESULT           dr                  = DRM_SUCCESS;
    DRM_M2TS_PACKET     *pOriginalTailPacket = NULL;
    DRM_M2TS_PACKET     *pPacket             = NULL;
    DRM_BOOL             fAppended           = FALSE;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pStream != NULL );
    ChkArg( f_pPacket != NULL );

    DRMASSERT( f_pStream->ePESState == eDRM_M2TS_PES_STATE_BEGIN );

    /* Temporarily append the packet to the stream's packet list on the per unit chain */
    pOriginalTailPacket = f_pStream->oPacketList.pTail;
    DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &f_pStream->oPacketList, f_pPacket );
    fAppended = TRUE;

    ChkBOOL( f_pStream->oPacketList.pHead != NULL
          && DRM_M2TS_IS_UNIT_START_PACKET( f_pStream->oPacketList.pHead->rgbData ), DRM_E_M2TS_NOT_UNIT_START_PACKET );

    if( f_pStream->ePacketType == eDRM_M2TS_PACKET_TYPE_PAT )
    {
        ChkDR( _ParsePATHeader( f_pEncContext, f_pStream ) );
    }
    else if( f_pStream->ePacketType == eDRM_M2TS_PACKET_TYPE_PMT )
    {
        ChkDR( _ParsePMTHeader( f_pEncContext, f_pStream ) );
    }
    else
    {
        DRMASSERT( DRM_M2TS_IS_PES( f_pStream->ePacketType ) );
        ChkDR( _ParsePESHeader( f_pEncContext, f_pStream ) );
    }

    /* Upate the cbHeader field in each of the PES packet on the per unit chain */
    if( DRM_M2TS_IS_PES( f_pStream->ePacketType )
     && f_pStream->ePESState != eDRM_M2TS_PES_STATE_BEGIN )
    {
        DRM_DWORD cbHeaderRemaining = f_pStream->cbHeader;

        DRMASSERT( f_pStream->cbHeader > 0 );

        pPacket = f_pStream->oPacketList.pHead;
        while( pPacket != NULL && cbHeaderRemaining > 0 )
        {
            ChkDR( DRM_DWordToByte( pPacket->cbPayload > cbHeaderRemaining ? cbHeaderRemaining : pPacket->cbPayload, &pPacket->cbHeader ) );
            cbHeaderRemaining -= pPacket->cbHeader;

            pPacket = pPacket->pNextPerUnit;
        }

        DRMASSERT( cbHeaderRemaining == 0 );
    }

ErrorExit:
    /* Remove the temporarily appended pakcet */
    if( fAppended )
    {
        DRM_M2TS_ENSURE_ITEM_ON_PER_UNIT_LIST( &f_pStream->oPacketList, DRM_M2TS_PACKET, pOriginalTailPacket );
        DRM_M2TS_RESET_PER_UNIT_LIST_TO_ORIGINAL_TAIL_NO_FAIL( &f_pStream->oPacketList, DRM_M2TS_PACKET, pOriginalTailPacket );
    }
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_UpdatePESPacketLength
**
** Synopsis:    Update the PES packet length field using the adjustment
**              amount of bytes specified in the argument.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_cbAdjustment]         -- The amount of bytes to increase if it is a positive
**                             value or decrease if it is a negative value.
** [f_pPacketList]          -- Pointer to a packet list of a PES packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_INTERNAL_ERROR 
**                          DRM_E_ARITHMETIC_OVERFLOW
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_M2TS_UpdatePESPacketLength( 
    __in      const DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_LONG                     f_cbAdjustment,
    __inout         DRM_M2TS_PACKET_LIST        *f_pPacketList ) 
{
    DRM_RESULT                  dr                  = DRM_SUCCESS;
    DRM_WORD                    cbPESPacketLength   = 0;
    DRM_DWORD                   dwPESSartCode       = 0;
    DRM_WORD                    cbAdjustment        = 0;
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContext         = { 0 };
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContextMarker   = { 0 };

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pPacketList != NULL );

    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, f_pPacketList->pHead ) );

    /* Verify the PES start code 0x000001 */
    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 24, &dwPESSartCode ) );
    ChkBOOL( dwPESSartCode == 1, DRM_E_M2TS_INTERNAL_ERROR );

    /* skip stream_id(8) */
    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 1 ) );

    /* Save the context in order to update the PES packet length after we read it */
    oBSRContextMarker = oBSRContext;

    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 16, &cbPESPacketLength ) );

    if( cbPESPacketLength != 0 )
    {
        if( f_cbAdjustment > 0 )
        {
            ChkDR( DRM_LongToWord( f_cbAdjustment, &cbAdjustment ) );
            ChkDR( DRM_WordAdd( cbPESPacketLength, cbAdjustment, &cbPESPacketLength ) );
        }
        else
        {
            ChkBOOL( f_cbAdjustment > DRM_LONG_MIN, DRM_E_ARITHMETIC_OVERFLOW );
            ChkDR( DRM_LongToWord( f_cbAdjustment * -1, &cbAdjustment ) ); /* convert to positive number */
            ChkDR( DRM_WordSub( cbPESPacketLength, cbAdjustment, &cbPESPacketLength ) );
        }
    
        ChkDR( DRM_M2TS_BSR_WriteBitsFromWORD( &oBSRContextMarker, 16, cbPESPacketLength ) );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_M2TS_UpdateAlignmentFlag
**
** Synopsis:    Update the PES packet data alignment indicator field.
**
** Arguments:
**
** [f_pEncContext]          -- Pointer to a encryptor context initialized via
**                             Drm_M2ts_Initialize.
** [f_bAlignmentFlag]       -- The value of the data alignment indicator to be set.
** [f_pPacketList]          -- Pointer to a packet list of a PES packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_M2TS_INTERNAL_ERROR 
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_M2TS_UpdateAlignmentFlag( 
    __in      const DRM_M2TS_ENCRYPTOR_CONTEXT  *f_pEncContext,
    __in      const DRM_BYTE                     f_bAlignmentFlag,
    __inout         DRM_M2TS_PACKET_LIST        *f_pPacketList ) 
{
    DRM_RESULT                  dr                  = DRM_SUCCESS;
    DRM_DWORD                   dwPESSartCode       = 0;
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContext         = { 0 };

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pPacketList != NULL );
    ChkArg( f_bAlignmentFlag == 0 || f_bAlignmentFlag == 1 );

    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, f_pPacketList->pHead ) );

    /* Verify the PES start code 0x000001 */
    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 24, &dwPESSartCode ) );
    ChkBOOL( dwPESSartCode == 1, DRM_E_M2TS_INTERNAL_ERROR );

    /* skip stream_id(8), PES_packet_length(16) */
    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 3 ) );

    /* skip '10', PES_scrambling_control(2), PES_priority(1) */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 5 ) );

    /* Update data_alignment_indicator */
    ChkDR( DRM_M2TS_BSR_WriteBitsFromBYTE( &oBSRContext, f_bAlignmentFlag, f_bAlignmentFlag ) );

ErrorExit:
    return dr;
}
                                           
EXIT_PK_NAMESPACE_CODE;
