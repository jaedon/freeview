/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_asf_packetizer.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 11/10/11 12:49p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/7425/file/asf/bmuxlib_file_asf_packetizer.c $
 * 
 * Hydra_Software_Devel/2   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1691/1   11/10/11 11:56a nilesh
 * SW7425-1691: Added support for stop-->start without requiring
 * destroy/create
 * 
 * Hydra_Software_Devel/1   9/1/11 12:04p nilesh
 * SW7425-720: ASF Packetizer
 * 
 ***************************************************************************/

#include "bstd.h" /* also includes berr, bdbg, etc */
#include "bkni.h"
#include "bdbg.h"           /* debug interface */

#include "bmuxlib_file_asf.h"
#include "bmuxlib_file_asf_priv.h"

#include "bmuxlib_input.h"
#include "bmuxlib_output.h"

#include "bmuxlib_file_asf_packetizer.h"

BDBG_MODULE(BMUXLIB_FILE_ASF_PACKETIZER);
BDBG_FILE_MODULE(BMUXLIB_FILE_ASF_PACKETIZER_INPUT);
BDBG_FILE_MODULE(BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE);
BDBG_FILE_MODULE(BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT);
BDBG_FILE_MODULE(BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE);


#define BMUXlib_File_ASF_P_Packetizer_MAX_PACKETS             64
#define BMUXlib_File_ASF_P_Packetizer_MAX_PAYLOADS_PER_PACKET 63
#define BMUXlib_File_ASF_P_Packetizer_MAX_BUFFERS_PER_PAYLOAD 2
#define BMUXlib_File_ASF_P_Packetizer_MAX_DESCRIPTORS         ( ( BMUXlib_File_ASF_P_Packetizer_MAX_PACKETS ) * ( BMUXlib_File_ASF_P_Packetizer_MAX_PAYLOADS_PER_PACKET ) * ( BMUXlib_File_ASF_P_Packetizer_MAX_BUFFERS_PER_PAYLOAD ) )
#define BMUXlib_File_ASF_P_Packetizer_MAX_STREAMS             127

/* Packetizer Input State Machine
 *
 *                         | _New State_                                                                                                        |
                           | AddPacket              | ProcessCurrentDescriptor | GetNextDescriptor          | AddPayload           | FinalizePacket |
| _Current State_          |
| AddPacket                | [No Packets Available] | Packet Available         | X                          | X                    | X              |
| ProcessCurrentDescriptor | X                      | X                        | Descriptor Done            | Descriptor Not Done  | X              |
| GetNextDescriptor        | X                      | Descriptor Empty         | [Descriptor Not Available] | Descriptor Not Empty | X              |
| AddPayload               | X                      | Packet Not Full          | X                          | X                    | Packet Full    |
| FinalizePacket           | Payloads               | [No Payloads]            | X                          | X                    | X              |

[xxx] indicates an blocked state
*/

typedef enum BMUXlib_File_ASF_P_Packetizer_Input_State
{
   BMUXlib_File_ASF_P_Packetizer_Input_State_eAddPacket,
   BMUXlib_File_ASF_P_Packetizer_Input_State_eProcessCurrentDescriptor,
   BMUXlib_File_ASF_P_Packetizer_Input_State_eGetNextDescriptor,
   BMUXlib_File_ASF_P_Packetizer_Input_State_eAddPayload,
   BMUXlib_File_ASF_P_Packetizer_Input_State_eFinalizePacket
} BMUXlib_File_ASF_P_Packetizer_Input_State;

typedef enum BMUXlib_File_ASF_P_Packetizer_Output_State
{
   BMUXlib_File_ASF_P_Packetizer_Output_State_eGetPacket,
   BMUXlib_File_ASF_P_Packetizer_Output_State_ePayloadParsingInformation,
   BMUXlib_File_ASF_P_Packetizer_Output_State_eMultiplePayloadHeader,
   BMUXlib_File_ASF_P_Packetizer_Output_State_ePayloadHeader,
   BMUXlib_File_ASF_P_Packetizer_Output_State_ePayload,
   BMUXlib_File_ASF_P_Packetizer_Output_State_ePadding,
   BMUXlib_File_ASF_P_Packetizer_Output_State_ePacketDone
} BMUXlib_File_ASF_P_Packetizer_Output_State;

/**********/
/* Packet */
/**********/
/* Payload Parsing Info */
#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_MAXSIZE 12
typedef union BMUXlib_File_ASF_Packet_PayloadParsingInfo
{
      uint8_t data[BMUXlib_File_ASF_Packet_PayloadParsingInfo_MAXSIZE];
} BMUXlib_File_ASF_Packet_PayloadParsingInfo;

static const uint8_t s_auiDefaultPayloadParsingInfo[BMUXlib_File_ASF_Packet_PayloadParsingInfo_MAXSIZE] =
{
 0x18, /* Length Type ( Multiple Payload = 0, Sequence Type = 0, Padding Length = 3, Packet Length = 0, Error Correction = 0) */
 0x5D, /* Property Flags ( Replicated Data Length = 1, Offset Into Media Length = 3, Media Object Number Length = 1, Stream Number Length = 1) */
 /* NO Packet Length */
 /* NO Sequence */
 0x00, 0x00, 0x00, 0x00, /* Padding Length */
 0x00, 0x00, 0x00, 0x00, /* Send Time */
 0x00, 0x00, /* Duration */
};

#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_MULTIPLEPAYLOAD_OFFSET 0
#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_MULTIPLEPAYLOAD_MASK 0x01
#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_MULTIPLEPAYLOAD_SHIFT 0

#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_PADDINGLENGTH 2
#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_SENDTIME 6
#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_DURATION 10

#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_SetMultiplePayloadsPresent(pHeader, multiple) \
   BMUXlib_File_ASF_P_Set8_Mask( (pHeader), BMUXlib_File_ASF_Packet_PayloadParsingInfo_MULTIPLEPAYLOAD_OFFSET, BMUXlib_File_ASF_Packet_PayloadParsingInfo_MULTIPLEPAYLOAD_MASK, BMUXlib_File_ASF_Packet_PayloadParsingInfo_MULTIPLEPAYLOAD_SHIFT, multiple )

#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_SetPaddingLength(pHeader, size) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_Packet_PayloadParsingInfo_PADDINGLENGTH, (size) )

#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_SetSendTime(pHeader, time) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_Packet_PayloadParsingInfo_SENDTIME, (time) )

#define BMUXlib_File_ASF_Packet_PayloadParsingInfo_SetDuration(pHeader, duration) \
   BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_Packet_PayloadParsingInfo_DURATION, (duration) )

/* Multiple Payload Header */
#define BMUXlib_File_ASF_Packet_MultiplePayloadHeader_MAXSIZE 1
typedef union BMUXlib_File_ASF_Packet_MultiplePayloadHeader
{
      uint8_t data[BMUXlib_File_ASF_Packet_MultiplePayloadHeader_MAXSIZE];
} BMUXlib_File_ASF_Packet_MultiplePayloadHeader;

#define BMUXlib_File_ASF_Packet_MultiplePayloadHeader_NUMBEROFPAYLOADS 0
#define BMUXlib_File_ASF_Packet_MultiplePayloadHeader_NUMBEROFPAYLOADS_MASK 0x3F
#define BMUXlib_File_ASF_Packet_MultiplePayloadHeader_NUMBEROFPAYLOADS_SHIFT 0
#define BMUXlib_File_ASF_Packet_MultiplePayloadHeader_LENGTHTYPE 0
#define BMUXlib_File_ASF_Packet_MultiplePayloadHeader_LENGTHTYPE_MASK 0xC0
#define BMUXlib_File_ASF_Packet_MultiplePayloadHeader_LENGTHTYPE_SHIFT 6

#define BMUXlib_File_ASF_Packet_MultiplePayloadHeader_SetNumberOfPayloads(pHeader, number) \
         BMUXlib_File_ASF_P_Set8_Mask( (pHeader), BMUXlib_File_ASF_Packet_MultiplePayloadHeader_NUMBEROFPAYLOADS, BMUXlib_File_ASF_Packet_MultiplePayloadHeader_NUMBEROFPAYLOADS_MASK, BMUXlib_File_ASF_Packet_MultiplePayloadHeader_NUMBEROFPAYLOADS_SHIFT, (number) )

#define BMUXlib_File_ASF_Packet_MultiplePayloadHeader_SetLengthType(pHeader, number) \
         BMUXlib_File_ASF_P_Set8_Mask( (pHeader), BMUXlib_File_ASF_Packet_MultiplePayloadHeader_LENGTHTYPE, BMUXlib_File_ASF_Packet_MultiplePayloadHeader_LENGTHTYPE_MASK, BMUXlib_File_ASF_Packet_MultiplePayloadHeader_LENGTHTYPE_SHIFT, (number) )

/* Payload Header */
#define BMUXlib_File_ASF_Packet_PayloadHeader_MAXSIZE 17
typedef union BMUXlib_File_ASF_Packet_PayloadHeader
{
      uint8_t data[BMUXlib_File_ASF_Packet_PayloadHeader_MAXSIZE];
} BMUXlib_File_ASF_Packet_PayloadHeader;

static const uint8_t s_auiDefaultPayloadHeader[BMUXlib_File_ASF_Packet_PayloadHeader_MAXSIZE] =
{
 0x00, /* Stream Number + Key Frame */
 0x00, /* Media Object Number */
 0x00, 0x00, 0x00, 0x00, /* Offset Into Media Object */
 0x08, /* Replicated Data Length */
 /* Replicated Data */
 0x00, 0x00, 0x00, 0x00, /* Media Object Size */
 0x00, 0x00, 0x00, 0x00, /* Presentation Time */
 /* Payload Length */
 0x00, 0x00
 /* Payload */

};

#define BMUXlib_File_ASF_Packet_PayloadHeader_STREAMNUMBER 0
#define BMUXlib_File_ASF_Packet_PayloadHeader_STREAMNUMBER_MASK 0x7F
#define BMUXlib_File_ASF_Packet_PayloadHeader_STREAMNUMBER_SHIFT 0
#define BMUXlib_File_ASF_Packet_PayloadHeader_KEYFRAME 0
#define BMUXlib_File_ASF_Packet_PayloadHeader_KEYFRAME_MASK 0x80
#define BMUXlib_File_ASF_Packet_PayloadHeader_KEYFRAME_SHIFT 7

#define BMUXlib_File_ASF_Packet_PayloadHeader_MEDIAOBJECTNUMBER 1
#define BMUXlib_File_ASF_Packet_PayloadHeader_OFFSETINTOMEDIA 2
#define BMUXlib_File_ASF_Packet_PayloadHeader_MEDIAOBJECTSIZE 7
#define BMUXlib_File_ASF_Packet_PayloadHeader_PRESENTATIONTIME 11
#define BMUXlib_File_ASF_Packet_PayloadHeader_PAYLOADLENGTH 15

#define BMUXlib_File_ASF_Packet_PayloadHeader_SetStreamNumber(pHeader, number) \
         BMUXlib_File_ASF_P_Set8_Mask( (pHeader), BMUXlib_File_ASF_Packet_PayloadHeader_STREAMNUMBER, BMUXlib_File_ASF_Packet_PayloadHeader_STREAMNUMBER_MASK, BMUXlib_File_ASF_Packet_PayloadHeader_STREAMNUMBER_SHIFT, (number) )

#define BMUXlib_File_ASF_Packet_PayloadHeader_SetKeyFrame(pHeader, keyframe) \
         BMUXlib_File_ASF_P_Set8_Mask( (pHeader), BMUXlib_File_ASF_Packet_PayloadHeader_STREAMNUMBER, BMUXlib_File_ASF_Packet_PayloadHeader_KEYFRAME_MASK, BMUXlib_File_ASF_Packet_PayloadHeader_KEYFRAME_SHIFT, (keyframe) )

#define BMUXlib_File_ASF_Packet_PayloadHeader_SetMediaObjectNumber(pHeader, number) \
         BMUXlib_File_ASF_P_Set8( (pHeader), BMUXlib_File_ASF_Packet_PayloadHeader_MEDIAOBJECTNUMBER, (number) )

#define BMUXlib_File_ASF_Packet_PayloadHeader_SetOffsetIntoMedia(pHeader, offset) \
         BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_Packet_PayloadHeader_OFFSETINTOMEDIA, (offset) )

#define BMUXlib_File_ASF_Packet_PayloadHeader_SetMediaObjectSize(pHeader, offset) \
         BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_Packet_PayloadHeader_MEDIAOBJECTSIZE, (offset) )

#define BMUXlib_File_ASF_Packet_PayloadHeader_SetPresentationTime(pHeader, offset) \
         BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_Packet_PayloadHeader_PRESENTATIONTIME, (offset) )

#define BMUXlib_File_ASF_Packet_PayloadHeader_SetPayloadLength(pHeader, length) \
         BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_Packet_PayloadHeader_PAYLOADLENGTH, (length) )

typedef struct BMUXlib_File_ASF_P_Packetizer_Packet
{
      size_t uiBytesAvailable; /* For each packet always reserve at least:
                                *  - 12 bytes for Payload Parsing Info
                                *  - 1 byte for Payload Count (for multiple payloads)
                                *  - 15 bytes for Payload header
                                */

      size_t uiBytesUsed;

      BMUXlib_File_ASF_Packet_PayloadParsingInfo stPayloadParsingInfo;

      BMUXlib_File_ASF_Packet_MultiplePayloadHeader stMultiplePayloadHeader;
      struct
      {
            BMUXlib_File_ASF_Packet_PayloadHeader stPayloadHeader;
            struct
            {
                  void *pBuffer;
                  size_t uiLength;
                  BMUXlib_Output_CompletedCallbackInfo stCompletedCallbackInfo;
            } astBuffer[BMUXlib_File_ASF_P_Packetizer_MAX_BUFFERS_PER_PAYLOAD];
            size_t uiBufferCount;
            size_t uiBufferQueuedCount;
      } astPayload[BMUXlib_File_ASF_P_Packetizer_MAX_PAYLOADS_PER_PACKET], *pCurrentPayload;
      size_t uiPayloadCount;
      size_t uiPayloadQueuedCount;

      unsigned uiSendTime;
} BMUXlib_File_ASF_P_Packetizer_Packet;

typedef struct BMUXlib_File_ASF_P_Packetizer_DescriptorMetadata
{
      size_t uiBytesConsumed;
} BMUXlib_File_ASF_P_Packetizer_DescriptorMetadata;

typedef struct BMUXlib_File_ASF_P_Packetizer_DescriptorInfo
{
      BMUXlib_File_ASF_P_Packetizer_Descriptor stDescriptor;
      BMUXlib_Output_CompletedCallbackInfo stCompletedCallbackInfo;
      BMUXlib_File_ASF_P_Packetizer_DescriptorMetadata stMetadata;
} BMUXlib_File_ASF_P_Packetizer_DescriptorInfo;

typedef struct BMUXlib_File_ASF_P_Packetizer_Context
{
      BMUXlib_File_ASF_P_Packetizer_CreateSettings stCreateSettings;

      void *pPaddingBuffer;

      /* Status */
      struct
      {
         BMUXlib_File_ASF_P_Packetizer_Input_State eInputState;
         BMUXlib_File_ASF_P_Packetizer_Output_State eOutputState;

         /* Descriptor Info */
         struct
         {
               BMUXlib_File_ASF_P_Packetizer_DescriptorInfo astDescriptorInfo[BMUXlib_File_ASF_P_Packetizer_MAX_DESCRIPTORS];
               size_t uiCount;
               unsigned uiWriteOffset;
               unsigned uiReadOffset;

               BMUXlib_File_ASF_P_Packetizer_DescriptorInfo *pCurrentDescriptorInfo;
         } stDescriptorInfo;

         /* Packet Info */
         struct
         {
               BMUXlib_File_ASF_P_Packetizer_Packet astPacket[BMUXlib_File_ASF_P_Packetizer_MAX_PACKETS];
               size_t uiCount;
               unsigned uiQueueOffset;
               unsigned uiWriteOffset;
               unsigned uiReadOffset;

               BMUXlib_File_ASF_P_Packetizer_Packet *pCurrentInputPacket;
               BMUXlib_File_ASF_P_Packetizer_Packet *pCurrentOutputPacket;
         } stPacketInfo;

         struct
         {
               uint32_t auiBytesInFrame[BMUXlib_File_ASF_P_Packetizer_MAX_STREAMS];
               uint64_t uiDataPacketsCount;
         } stStats;
      } status;

} BMUXlib_File_ASF_P_Packetizer_Context;

static const BMUXlib_File_ASF_P_Packetizer_CreateSettings s_stDefaultPacketizerCreateSettings =
{
 BMUXLIB_FILE_ASF_P_PACKETIZER_SIGNATURE_CREATESETTINGS, /* Signature */

 BMUXLIB_FILE_ASF_P_DEFAULT_PACKETLENGTH,      /* Packet Length */
};

void
BMUXlib_File_ASF_P_Packetizer_GetDefaultCreateSettings(
         BMUXlib_File_ASF_P_Packetizer_CreateSettings *pstSettings
         )
{
   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_GetDefaultCreateSettings );

   BDBG_ASSERT( pstSettings );

   *pstSettings = s_stDefaultPacketizerCreateSettings;

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_GetDefaultCreateSettings );
}

void
BMUXlib_File_ASF_P_Packetizer_Reset(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         )
{
   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_Reset );

   BKNI_Memset( &hPacketizer->status, 0, sizeof( hPacketizer->status ) );

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_Reset );
}

BERR_Code
BMUXlib_File_ASF_P_Packetizer_Create(
         BMUXlib_File_ASF_P_Packetizer_Handle *phPacketizer,
         const BMUXlib_File_ASF_P_Packetizer_CreateSettings *pstSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BMUXlib_File_ASF_P_Packetizer_Handle hASFPacketizer;

   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_Create );

   BDBG_ASSERT( phPacketizer );
   BDBG_ASSERT( pstSettings );
   BDBG_ASSERT( BMUXLIB_FILE_ASF_P_PACKETIZER_SIGNATURE_CREATESETTINGS == pstSettings->uiSignature );

   *phPacketizer = NULL;

   hASFPacketizer = ( BMUXlib_File_ASF_P_Packetizer_Handle ) BKNI_Malloc( sizeof( BMUXlib_File_ASF_P_Packetizer_Context ) );

   if ( NULL != hASFPacketizer )
   {
      BKNI_Memset( hASFPacketizer, 0, sizeof( BMUXlib_File_ASF_P_Packetizer_Context ) );

      hASFPacketizer->stCreateSettings = *pstSettings;

      hASFPacketizer->pPaddingBuffer = BKNI_Malloc( hASFPacketizer->stCreateSettings.uiPacketSize );
      if ( NULL == hASFPacketizer->pPaddingBuffer )
      {
         BMUXlib_File_ASF_P_Packetizer_Destroy ( hASFPacketizer );
         rc = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
      }
      else
      {
         BKNI_Memset( hASFPacketizer->pPaddingBuffer, 0, hASFPacketizer->stCreateSettings.uiPacketSize );

         /* Reset State */
         BMUXlib_File_ASF_P_Packetizer_Reset( hASFPacketizer );

         *phPacketizer = hASFPacketizer;
      }
   }
   else
   {
      rc = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_Create );

   return BERR_TRACE( rc );
}

void
BMUXlib_File_ASF_P_Packetizer_Destroy(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         )
{
   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_Destroy );

   BDBG_ASSERT( hPacketizer );

   if ( NULL != hPacketizer->pPaddingBuffer )
   {
      BKNI_Free( hPacketizer->pPaddingBuffer );
      hPacketizer->pPaddingBuffer = NULL;
   }

   BKNI_Free( hPacketizer );

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_Destroy );
}

bool
BMUXlib_File_ASF_P_Packetizer_IsSpaceAvailable(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         )
{
   bool bResult;

   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_IsSpaceAvailable );

   bResult = hPacketizer->status.stDescriptorInfo.uiCount < BMUXlib_File_ASF_P_Packetizer_MAX_DESCRIPTORS;

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_IsSpaceAvailable );

   return bResult;
}

bool
BMUXlib_File_ASF_P_Packetizer_IsDescriptorPendingQueue(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         )
{
   bool bResult;

   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_IsDescriptorPendingQueue );

   bResult = (0 != hPacketizer->status.stDescriptorInfo.uiCount);

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_IsDescriptorPendingQueue );

   return bResult;
}

BERR_Code
BMUXlib_File_ASF_P_Packetizer_AddDescriptor(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer,
         BMUXlib_File_ASF_P_Packetizer_Descriptor *pstDescriptor,
         BMUXlib_Output_CompletedCallbackInfo *pstCompletedCallbackInfo
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_AddDescriptor );

   if ( true == BMUXlib_File_ASF_P_Packetizer_IsSpaceAvailable( hPacketizer ) )
   {
      hPacketizer->status.stDescriptorInfo.astDescriptorInfo[hPacketizer->status.stDescriptorInfo.uiWriteOffset].stDescriptor = *pstDescriptor;
      hPacketizer->status.stDescriptorInfo.astDescriptorInfo[hPacketizer->status.stDescriptorInfo.uiWriteOffset].stCompletedCallbackInfo = *pstCompletedCallbackInfo;
      BKNI_Memset( &hPacketizer->status.stDescriptorInfo.astDescriptorInfo[hPacketizer->status.stDescriptorInfo.uiWriteOffset].stMetadata, 0, sizeof( BMUXlib_File_ASF_P_Packetizer_DescriptorMetadata ) );

      hPacketizer->status.stDescriptorInfo.uiWriteOffset++;
      hPacketizer->status.stDescriptorInfo.uiWriteOffset %= BMUXlib_File_ASF_P_Packetizer_MAX_DESCRIPTORS;

      hPacketizer->status.stDescriptorInfo.uiCount++;
   }
   else
   {
      rc = BERR_TRACE( BERR_UNKNOWN );
   }

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_AddDescriptor );

   return BERR_TRACE( rc );
}

/* ASF Packet
 *    - Payload Parsing Info [X bytes]
 *    - Multiple Payload Header (n > 1) [2 bytes]
 *    - Payloads x n
 *       - Payload Header []
 *       - Payload
 *    - Padding
 */

/* Packetize State Machine
 *
 *
 */
BERR_Code
BMUXlib_File_ASF_P_Packetizer_ProcessNewDescriptors(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         )
{
   bool bBlocked = false;
   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_ProcessNewDescriptors );

   while ( false == bBlocked )
   {
      switch ( hPacketizer->status.eInputState )
      {
         case BMUXlib_File_ASF_P_Packetizer_Input_State_eAddPacket:
            if ( hPacketizer->status.stPacketInfo.uiCount < ( BMUXlib_File_ASF_P_Packetizer_MAX_PACKETS - 1 ) )
            {
               /* Allocate a new packet */
               hPacketizer->status.stPacketInfo.pCurrentInputPacket = &hPacketizer->status.stPacketInfo.astPacket[hPacketizer->status.stPacketInfo.uiWriteOffset];

               BKNI_Memset( hPacketizer->status.stPacketInfo.pCurrentInputPacket,
                            0,
                            sizeof ( BMUXlib_File_ASF_P_Packetizer_Packet )
                           );

               hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable = hPacketizer->stCreateSettings.uiPacketSize;
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT, ("%5d (packet size)", hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable));

               /* Reserve Space for Payload Parsing Information */
               hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable -= BMUXlib_File_ASF_Packet_PayloadParsingInfo_MAXSIZE;
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT, ("%5d (%5d: payload parsing info)", hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable, BMUXlib_File_ASF_Packet_PayloadParsingInfo_MAXSIZE));

               /* Reserve space for Multiple Payload Header if we have multiple payloads */
               hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable -= BMUXlib_File_ASF_Packet_MultiplePayloadHeader_MAXSIZE;
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT, ("%5d (%5d: multiple payload header)", hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable, BMUXlib_File_ASF_Packet_MultiplePayloadHeader_MAXSIZE));

               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE, ("eAddPacket --> eProcessCurrentDescriptor") );
               hPacketizer->status.eInputState = BMUXlib_File_ASF_P_Packetizer_Input_State_eProcessCurrentDescriptor;
            }
            else
            {
               bBlocked = true;
            }
            break;

         case BMUXlib_File_ASF_P_Packetizer_Input_State_eProcessCurrentDescriptor:
            if ( ( NULL != hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo )
                 && ( hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stMetadata.uiBytesConsumed
                      < hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.uiLength ) )
            {
               /* We still have data left in the current descriptor, so continue with the current descriptor */
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE, ("eProcessCurrentDescriptor --> eAddPayload (Current Desc)") );
               hPacketizer->status.eInputState = BMUXlib_File_ASF_P_Packetizer_Input_State_eAddPayload;
            }
            else
            {
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE, ("eProcessCurrentDescriptor --> eGetNextDescriptor (New Desc)") );

               if ( NULL != hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo )
               {
                  BDBG_ASSERT( 0 != hPacketizer->status.stDescriptorInfo.uiCount );
                  hPacketizer->status.stDescriptorInfo.uiReadOffset++;
                  hPacketizer->status.stDescriptorInfo.uiReadOffset %= BMUXlib_File_ASF_P_Packetizer_MAX_DESCRIPTORS;
                  hPacketizer->status.stDescriptorInfo.uiCount--;
                  hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo = NULL;
               }

               hPacketizer->status.eInputState = BMUXlib_File_ASF_P_Packetizer_Input_State_eGetNextDescriptor;
            }

            break;

         case BMUXlib_File_ASF_P_Packetizer_Input_State_eGetNextDescriptor:
            {
               /* Select the next descriptor */
               if ( 0 == hPacketizer->status.stDescriptorInfo.uiCount )
               {
                  /* No more descriptors */
                  bBlocked = true;
               }
               else
               {
                  /* Descriptor Available */
                  hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo = &hPacketizer->status.stDescriptorInfo.astDescriptorInfo[hPacketizer->status.stDescriptorInfo.uiReadOffset];

                  if ( ( true == hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.stFrameInfo.bNewFrame )
                       && ( 0 == hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stMetadata.uiBytesConsumed ) )
                  {
                     hPacketizer->status.stStats.auiBytesInFrame[hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.uiStreamNumber] = 0;
                  }

                  if ( 0 != hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.uiLength )
                  {
                     BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE, ("eGetNextDescriptor --> eAddPayload (More Desc)") );
                     hPacketizer->status.eInputState = BMUXlib_File_ASF_P_Packetizer_Input_State_eAddPayload;
                  }
                  else
                  {
                     BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE, ("eGetNextDescriptor --> eProcessCurrentDescriptor (Empty Desc)") );
                     hPacketizer->status.eInputState = BMUXlib_File_ASF_P_Packetizer_Input_State_eProcessCurrentDescriptor;
                  }
               }
            }
            break;

         case BMUXlib_File_ASF_P_Packetizer_Input_State_eAddPayload:
            /* Process current descriptor */
            if ( ( hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable < BMUXlib_File_ASF_Packet_PayloadHeader_MAXSIZE * 2)
                 || ( BMUXlib_File_ASF_P_Packetizer_MAX_PAYLOADS_PER_PACKET == hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiPayloadCount ) )
            {
               /* Packet is full, so let's finalize this packet */
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE, ("eAddPayload --> eFinalizePacket (Packet Full (%d))", hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable) );
               hPacketizer->status.eInputState = BMUXlib_File_ASF_P_Packetizer_Input_State_eFinalizePacket;
            }
            else
            {
               /* Packet is not full, so let's add more payload to this packet */

               /* TODO: Only create another payload header if the descriptor contains a different frame than the current payload,
                * otherwise add it as a separate buffer */

               /* Allocate space for the payload header */
               hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable -= BMUXlib_File_ASF_Packet_PayloadHeader_MAXSIZE;
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT, ("%5d (%5d: payload header)", hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable, BMUXlib_File_ASF_Packet_PayloadHeader_MAXSIZE));

               hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload = &hPacketizer->status.stPacketInfo.pCurrentInputPacket->astPayload[hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiPayloadCount];
               hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiPayloadCount++;

               /* TODO: Check for max buffer count */
               hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->astBuffer[hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->uiBufferCount].pBuffer = (void*)
                        ((unsigned) hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.pBuffer + hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stMetadata.uiBytesConsumed);

               /**********************/
               /* Set payload header */
               /**********************/
               BKNI_Memcpy(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->stPayloadHeader.data,
                        s_auiDefaultPayloadHeader,
                        BMUXlib_File_ASF_Packet_PayloadHeader_MAXSIZE
                        );

               /* Set Stream Number */
               BMUXlib_File_ASF_Packet_PayloadHeader_SetStreamNumber(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->stPayloadHeader.data,
                        hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.uiStreamNumber
                        );

               /* Set Key Frame */
               BMUXlib_File_ASF_Packet_PayloadHeader_SetKeyFrame(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->stPayloadHeader.data,
                        (hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.stFrameInfo.bKeyFrame ? 1 : 0)
                        );

               /* Set Media Object Number */
               BMUXlib_File_ASF_Packet_PayloadHeader_SetMediaObjectNumber(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->stPayloadHeader.data,
                        hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.stFrameInfo.uiFrameNumber
                        );

               /* Set Offset Into Media */
               BMUXlib_File_ASF_Packet_PayloadHeader_SetOffsetIntoMedia(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->stPayloadHeader.data,
                        hPacketizer->status.stStats.auiBytesInFrame[hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.uiStreamNumber]
                        );

               /* Set Replicated Data Length (use default) */

               /* Set Media Object Size */
               BMUXlib_File_ASF_Packet_PayloadHeader_SetMediaObjectSize(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->stPayloadHeader.data,
                        hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.stFrameInfo.uiFrameSize
                        );

               /* Set Presentation Time */
               BMUXlib_File_ASF_Packet_PayloadHeader_SetPresentationTime(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->stPayloadHeader.data,
                        hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.stFrameInfo.uiPresentationTime
                        );


               /* Set Packet's Send Time based on the 1st payload's sent time */
               if ( 1 == hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiPayloadCount )
               {
                  hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiSendTime = hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.stFrameInfo.uiSendTime;
               }

               {
                  /* Determine the payload length, which is the smaller of the descriptor bytes available and the packet bytes available */
                  size_t uiDescriptorBytesLeft = hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.uiLength - hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stMetadata.uiBytesConsumed;
                  size_t uiPayloadLength;
                  if ( hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable < uiDescriptorBytesLeft )
                  {
                     uiPayloadLength = hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable;
                  }
                  else
                  {
                     uiPayloadLength = uiDescriptorBytesLeft;
                  }
                  hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable -= uiPayloadLength;
                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT, ("%5d (%5d: payload)", hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable, uiPayloadLength));

                  hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->astBuffer[hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->uiBufferCount].uiLength = uiPayloadLength;
                  hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stMetadata.uiBytesConsumed += uiPayloadLength;

                  hPacketizer->status.stStats.auiBytesInFrame[hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.uiStreamNumber] += uiPayloadLength;

                  BMUXlib_File_ASF_Packet_PayloadHeader_SetPayloadLength(
                           hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->stPayloadHeader.data,
                           uiPayloadLength
                           );

                  /* We're done with the descriptor, so set the associate the descriptor's completed callback info with this payload buffer */
                  if ( hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stMetadata.uiBytesConsumed == hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stDescriptor.uiLength )
                  {
                     hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->astBuffer[hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->uiBufferCount].stCompletedCallbackInfo =
                        hPacketizer->status.stDescriptorInfo.pCurrentDescriptorInfo->stCompletedCallbackInfo;
                  }
               }
               hPacketizer->status.stPacketInfo.pCurrentInputPacket->pCurrentPayload->uiBufferCount++;

               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE, ("eAddPayload --> eProcessCurrentDescriptor",hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable) );
               hPacketizer->status.eInputState = BMUXlib_File_ASF_P_Packetizer_Input_State_eProcessCurrentDescriptor;
            }

            break;

         case BMUXlib_File_ASF_P_Packetizer_Input_State_eFinalizePacket:

            if ( 0 == hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiPayloadCount )
            {
               /* No payloads are in this packet, so just exit.  We'll see if there are descriptors next time. */
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE, ("eFinalizePacket --> eProcessCurrentDescriptor (Empty Packet)") );
               hPacketizer->status.eInputState = BMUXlib_File_ASF_P_Packetizer_Input_State_eProcessCurrentDescriptor;
               bBlocked = true;
            }
            else
            {
               /* Payload Parsing Information */
               BKNI_Memcpy(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->stPayloadParsingInfo.data,
                        s_auiDefaultPayloadParsingInfo,
                        BMUXlib_File_ASF_Packet_PayloadParsingInfo_MAXSIZE
                        );

               /* Set Multiple Payloads Present Flag */
               BMUXlib_File_ASF_Packet_PayloadParsingInfo_SetMultiplePayloadsPresent(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->stPayloadParsingInfo.data,
                        (( hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiPayloadCount > 1 ) ? 1 : 0 )
                        );

               if ( hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiPayloadCount <= 1 )
               {
                  /* UN-Reserve space for Multiple Payload Header if we have a single payload */
                  hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable += BMUXlib_File_ASF_Packet_MultiplePayloadHeader_MAXSIZE;
                  hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable += 2; /* For unneeded payload length in single payload header */

                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT, ("%5d (%5d: single payload)", hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable, -(BMUXlib_File_ASF_Packet_MultiplePayloadHeader_MAXSIZE + 2) ));
               }
               else
               {
                  BMUXlib_File_ASF_Packet_MultiplePayloadHeader_SetNumberOfPayloads(
                           hPacketizer->status.stPacketInfo.pCurrentInputPacket->stMultiplePayloadHeader.data,
                           hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiPayloadCount
                           );

                  BMUXlib_File_ASF_Packet_MultiplePayloadHeader_SetLengthType(
                           hPacketizer->status.stPacketInfo.pCurrentInputPacket->stMultiplePayloadHeader.data,
                           0x02
                           );
               }

               /* Set Padding Length */
               BMUXlib_File_ASF_Packet_PayloadParsingInfo_SetPaddingLength(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->stPayloadParsingInfo.data,
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable
                        );

               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT, ("%5d (padding)", hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiBytesAvailable));

               /* Set Send Time - Use the send time of the first packet */
               BMUXlib_File_ASF_Packet_PayloadParsingInfo_SetSendTime(
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->stPayloadParsingInfo.data,
                        hPacketizer->status.stPacketInfo.pCurrentInputPacket->uiSendTime
                        );

               /* TODO: Set Duration (use default) */

               /* Increment write offset so that output queue logic will see this new packet */
               hPacketizer->status.stPacketInfo.uiWriteOffset++;
               hPacketizer->status.stPacketInfo.uiWriteOffset %= BMUXlib_File_ASF_P_Packetizer_MAX_PACKETS;
               hPacketizer->status.stPacketInfo.uiCount++;

               /* Start creating the next packet */
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_INPUT_STATE, ("eFinalizePacket --> eAddPacket") );
               hPacketizer->status.eInputState = BMUXlib_File_ASF_P_Packetizer_Input_State_eAddPacket;
            }
            break;
      }
   }

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_ProcessNewDescriptors );

   return BERR_TRACE( BERR_SUCCESS );
}

void
BMUXlib_File_ASF_P_Packetizer_ProcessCompletedPacket(
         void* pPrivateData,
         const BMUXlib_Output_Descriptor *pOutputDescriptor
         )
{
   BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer = (BMUXlib_File_ASF_P_Packetizer_Handle) pPrivateData;

   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_ProcessCompletedPacket );

   BSTD_UNUSED( pOutputDescriptor );

   BDBG_ASSERT( 0 != hPacketizer->status.stPacketInfo.uiCount );

   hPacketizer->status.stPacketInfo.uiCount--;

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_ProcessCompletedPacket );
}

BERR_Code
BMUXlib_File_ASF_P_Packetizer_ScheduleProcessedPackets(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer,
         BMUXlib_Output_Handle hOutput
         )
{
   bool bBlocked = false;

   BDBG_ENTER( BMUXlib_File_ASF_P_Packetizer_ScheduleProcessedPackets );

   while ( false == bBlocked )
   {
      switch ( hPacketizer->status.eOutputState )
      {
         case BMUXlib_File_ASF_P_Packetizer_Output_State_eGetPacket:
            if ( hPacketizer->status.stPacketInfo.uiQueueOffset != hPacketizer->status.stPacketInfo.uiWriteOffset )
            {
               hPacketizer->status.stPacketInfo.pCurrentOutputPacket = &hPacketizer->status.stPacketInfo.astPacket[hPacketizer->status.stPacketInfo.uiQueueOffset];

               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE, ("eGetPacket --> ePayloadParsingInformation") );
               hPacketizer->status.eOutputState = BMUXlib_File_ASF_P_Packetizer_Output_State_ePayloadParsingInformation;
            }
            else
            {
               bBlocked = true;
            }
            break;

         case BMUXlib_File_ASF_P_Packetizer_Output_State_ePayloadParsingInformation:
            if ( true == BMUXlib_Output_IsSpaceAvailable( hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;

               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hPacketizer->status.stPacketInfo.pCurrentOutputPacket->stPayloadParsingInfo.data;
               stOutputDescriptor.stStorage.uiLength = BMUXlib_File_ASF_Packet_PayloadParsingInfo_MAXSIZE;
               stOutputDescriptor.stStorage.uiOffset = BMUXlib_Output_GetCurrentOffset( hOutput );

               hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed += stOutputDescriptor.stStorage.uiLength;
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT, ("[%08x] %5d (%5d@%08x : payload parsing info)",
                  (uint32_t) hPacketizer->status.stStats.uiDataPacketsCount,
                  hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed,
                  stOutputDescriptor.stStorage.uiLength,
                  stOutputDescriptor.stStorage.uiOffset
                  ));

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hOutput,
                        &stOutputDescriptor,
                        NULL
                        ));

               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE, ("ePayloadParsingInformation --> eMultiplePayloadHeader") );
               hPacketizer->status.eOutputState = BMUXlib_File_ASF_P_Packetizer_Output_State_eMultiplePayloadHeader;
            }
            else
            {
               bBlocked = true;
            }
            break;

         case BMUXlib_File_ASF_P_Packetizer_Output_State_eMultiplePayloadHeader: /* Optional */
            if ( hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiPayloadCount > 1 )
            {
               if ( true == BMUXlib_Output_IsSpaceAvailable( hOutput ) )
               {
                  BMUXlib_Output_Descriptor stOutputDescriptor;

                  BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

                  stOutputDescriptor.stStorage.bWriteOperation = true;
                  stOutputDescriptor.stStorage.pBufferAddress = hPacketizer->status.stPacketInfo.pCurrentOutputPacket->stMultiplePayloadHeader.data;
                  stOutputDescriptor.stStorage.uiLength = BMUXlib_File_ASF_Packet_MultiplePayloadHeader_MAXSIZE;
                  stOutputDescriptor.stStorage.uiOffset = BMUXlib_Output_GetCurrentOffset( hOutput );

                  hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed += stOutputDescriptor.stStorage.uiLength;
                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT, ("[%08x] %5d (%5d@%08x : multiple payload header)",
                     (uint32_t) hPacketizer->status.stStats.uiDataPacketsCount,
                     hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed,
                     stOutputDescriptor.stStorage.uiLength,
                     stOutputDescriptor.stStorage.uiOffset
                     ));

                  BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                           hOutput,
                           &stOutputDescriptor,
                           NULL
                           ));

                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE, ("eMultiplePayloadHeader --> ePayloadHeader (%d Payloads)", hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiPayloadCount) );
                  hPacketizer->status.eOutputState = BMUXlib_File_ASF_P_Packetizer_Output_State_ePayloadHeader;
               }
               else
               {
                  bBlocked = true;
               }
            }
            else
            {
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE, ("eMultiplePayloadHeader --> ePayloadHeader (Single Payload)") );
               hPacketizer->status.eOutputState = BMUXlib_File_ASF_P_Packetizer_Output_State_ePayloadHeader;
            }
            break;

         case BMUXlib_File_ASF_P_Packetizer_Output_State_ePayloadHeader:
            if ( hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiPayloadQueuedCount != hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiPayloadCount )
            {
               if ( true == BMUXlib_Output_IsSpaceAvailable( hOutput ) )
               {
                  BMUXlib_Output_Descriptor stOutputDescriptor;

                  BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

                  stOutputDescriptor.stStorage.bWriteOperation = true;
                  stOutputDescriptor.stStorage.pBufferAddress = hPacketizer->status.stPacketInfo.pCurrentOutputPacket->astPayload[hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiPayloadQueuedCount].stPayloadHeader.data;
                  stOutputDescriptor.stStorage.uiLength = BMUXlib_File_ASF_Packet_PayloadHeader_MAXSIZE;
                  if ( hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiPayloadCount <= 1 )
                  {
                     stOutputDescriptor.stStorage.uiLength -= 2; /* We don't need the last 2 bytes of the payload header for a single payload header */
                  }
                  stOutputDescriptor.stStorage.uiOffset = BMUXlib_Output_GetCurrentOffset( hOutput );

                  hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed += stOutputDescriptor.stStorage.uiLength;
                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT, ("[%08x] %5d (%5d@%08x : payload header)",
                     (uint32_t) hPacketizer->status.stStats.uiDataPacketsCount,
                     hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed,
                     stOutputDescriptor.stStorage.uiLength,
                     stOutputDescriptor.stStorage.uiOffset
                     ));

                  BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                           hOutput,
                           &stOutputDescriptor,
                           NULL
                           ));

                  BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE, ("ePayloadHeader --> ePayload[%d]", hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiPayloadQueuedCount) );
                  hPacketizer->status.eOutputState = BMUXlib_File_ASF_P_Packetizer_Output_State_ePayload;
               }
               else
               {
                  bBlocked = true;
               }
            }
            else
            {
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE, ("ePayloadHeader --> ePadding (Payloads Done)") );
               hPacketizer->status.eOutputState = BMUXlib_File_ASF_P_Packetizer_Output_State_ePadding;
            }

            break;

         case BMUXlib_File_ASF_P_Packetizer_Output_State_ePayload:
         {
            hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload = &hPacketizer->status.stPacketInfo.pCurrentOutputPacket->astPayload[hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiPayloadQueuedCount];

            if ( true == BMUXlib_Output_IsSpaceAvailable( hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;

               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload->astBuffer[hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload->uiBufferQueuedCount].pBuffer;
               stOutputDescriptor.stStorage.uiLength = hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload->astBuffer[hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload->uiBufferQueuedCount].uiLength;
               stOutputDescriptor.stStorage.uiOffset = BMUXlib_Output_GetCurrentOffset( hOutput );

               hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed += stOutputDescriptor.stStorage.uiLength;
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT, ("[%08x] %5d (%5d@%08x : payload)",
                  (uint32_t) hPacketizer->status.stStats.uiDataPacketsCount,
                  hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed,
                  stOutputDescriptor.stStorage.uiLength,
                  stOutputDescriptor.stStorage.uiOffset
                  ));

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hOutput,
                        &stOutputDescriptor,
                        &hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload->astBuffer[hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload->uiBufferQueuedCount].stCompletedCallbackInfo
                        ));

               hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload->uiBufferQueuedCount++;

               if ( hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload->uiBufferQueuedCount
                    == hPacketizer->status.stPacketInfo.pCurrentOutputPacket->pCurrentPayload->uiBufferCount )
               {
                  /* Go back and process the next payload */
                  hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiPayloadQueuedCount++;
               }

               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE, ("ePayload (%d) --> ePayloadHeader", stOutputDescriptor.stStorage.uiLength) );
               hPacketizer->status.eOutputState = BMUXlib_File_ASF_P_Packetizer_Output_State_ePayloadHeader;
            }
            else
            {
               bBlocked = true;
            }
         }
            break;
         case BMUXlib_File_ASF_P_Packetizer_Output_State_ePadding: /* Optional */
         {
            unsigned uiPaddingSize = hPacketizer->stCreateSettings.uiPacketSize - hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed;

            BDBG_ASSERT( hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed <= hPacketizer->stCreateSettings.uiPacketSize );
            BDBG_ASSERT( hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesAvailable == uiPaddingSize );

            if ( true == BMUXlib_Output_IsSpaceAvailable( hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;
               BMUXlib_Output_CompletedCallbackInfo stCallbackInfo;

               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );
               BKNI_Memset( &stCallbackInfo, 0, sizeof( BMUXlib_Output_CompletedCallbackInfo ) );

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hPacketizer->pPaddingBuffer;
               stOutputDescriptor.stStorage.uiLength = uiPaddingSize;
               stOutputDescriptor.stStorage.uiOffset = BMUXlib_Output_GetCurrentOffset( hOutput );

               hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed += stOutputDescriptor.stStorage.uiLength;
               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT, ("[%08x] %5d (%5d@%08x : padding)",
                  (uint32_t) hPacketizer->status.stStats.uiDataPacketsCount,
                  hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed,
                  stOutputDescriptor.stStorage.uiLength,
                  stOutputDescriptor.stStorage.uiOffset
                  ));

               stCallbackInfo.pCallback = BMUXlib_File_ASF_P_Packetizer_ProcessCompletedPacket;
               stCallbackInfo.pCallbackData = hPacketizer;

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hOutput,
                        &stOutputDescriptor,
                        &stCallbackInfo
                        ));

               BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE, ("ePadding --> ePacketDone (Padding [%d])", uiPaddingSize) );
               hPacketizer->status.eOutputState = BMUXlib_File_ASF_P_Packetizer_Output_State_ePacketDone;

               hPacketizer->status.stStats.uiDataPacketsCount++;
            }
            else
            {
               bBlocked = true;
            }
         }
            break;

         case BMUXlib_File_ASF_P_Packetizer_Output_State_ePacketDone:

            BDBG_ASSERT( hPacketizer->status.stPacketInfo.pCurrentOutputPacket->uiBytesUsed == hPacketizer->stCreateSettings.uiPacketSize );

            hPacketizer->status.stPacketInfo.uiQueueOffset++;
            hPacketizer->status.stPacketInfo.uiQueueOffset %= BMUXlib_File_ASF_P_Packetizer_MAX_PACKETS;

            BDBG_MODULE_MSG( BMUXLIB_FILE_ASF_PACKETIZER_OUTPUT_STATE, ("ePacketDone --> eGetPacket") );
            hPacketizer->status.eOutputState = BMUXlib_File_ASF_P_Packetizer_Output_State_eGetPacket;
            break;
      }
   }

   BDBG_LEAVE( BMUXlib_File_ASF_P_Packetizer_ScheduleProcessedPackets );

   return BERR_TRACE( BERR_SUCCESS );
}

uint64_t
BMUXlib_File_ASF_P_Packetizer_GetDataPacketCount(
         BMUXlib_File_ASF_P_Packetizer_Handle hPacketizer
         )
{
   return hPacketizer->status.stStats.uiDataPacketsCount;
}
