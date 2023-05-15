/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_telem.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/27/12 6:43p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/7425/bvce_telem.c $
 * 
 * Hydra_Software_Devel/3   4/27/12 6:43p nilesh
 * SW7435-153: Fixed compilation warning when DEBUG=n
 * 
 * Hydra_Software_Devel/2   12/20/11 3:55p nilesh
 * SW7425-1882: Fixed BVCE_Telem parser hang when no payload is present
 * 
 * Hydra_Software_Devel/1   10/14/11 11:55a nilesh
 * SW7425-1213: Telemetry Parser
 *
 ***************************************************************************/

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bdbg.h"           /* debug interface */
#include "bkni.h"           /* kernel interface */

#include "bvce_telem.h"

BDBG_MODULE(BVCE_TELEM);
BDBG_FILE_MODULE(BVCE_TELEM_STATE);

void
BVCE_Telem_GetDefaultCreateSettings(
         BVCE_Telem_Settings *pstTelemSettings
         )
{
   BDBG_ENTER( BVCE_Telem_GetDefaultCreateSettings );

   BDBG_ASSERT( pstTelemSettings );

   if ( NULL != pstTelemSettings )
   {
      BKNI_Memset( pstTelemSettings, 0, sizeof( BVCE_Telem_Settings ) );
   }

   BDBG_LEAVE( BVCE_Telem_GetDefaultCreateSettings );
}

typedef enum BVCE_Telem_P_ParserState
{
   BVCE_Telem_P_ParserState_eFileHeader,
   BVCE_Telem_P_ParserState_eLostSync,
   BVCE_Telem_P_ParserState_eFindSync,
   BVCE_Telem_P_ParserState_ePacketHeader,
   BVCE_Telem_P_ParserState_eProcessTimestamp,
   BVCE_Telem_P_ParserState_eComputePayloadSize,
   BVCE_Telem_P_ParserState_eProcessPayload,
   BVCE_Telem_P_ParserState_eDone
} BVCE_Telem_P_ParserState;

#if BDBG_DEBUG_BUILD
static const char* BVCE_Telem_P_ParserStateLUT[BVCE_Telem_P_ParserState_eDone + 1] =
{
   "eFileHeader",
   "eLostSync",
   "eFindSync",
   "ePacketHeader",
   "eProcessTimestamp",
   "eComputePayloadSize",
   "eProcessPayload",
   "eDone"
};
#endif

static const char BVCE_TELEM_P_LostSyncMessage[] =
{
   '\n','\n','<','<','L','O','S','T',' ','S','Y','N','C','>','>','\n','\n'
};

typedef enum BVCE_Telem_P_FileHeaderState
{
   BVCE_Telem_P_FileHeaderState_eHeaderID,
   BVCE_Telem_P_FileHeaderState_eHeaderLength,
   BVCE_Telem_P_FileHeaderState_eTelemVersion,
   BVCE_Telem_P_FileHeaderState_eProductVersion,
   BVCE_Telem_P_FileHeaderState_eCPUSpeed,
   BVCE_Telem_P_FileHeaderState_eDone
} BVCE_Telem_P_FileHeaderState;

typedef enum BVCE_Telem_P_PacketHeaderState
{
   BVCE_Telem_P_PacketHeaderState_eStream,
   BVCE_Telem_P_PacketHeaderState_eGroup,
   BVCE_Telem_P_PacketHeaderState_eFlags,
   BVCE_Telem_P_PacketHeaderState_eSyncByte,
   BVCE_Telem_P_PacketHeaderState_eDone
} BVCE_Telem_P_PacketHeaderState;

typedef struct BVCE_Telem_P_Context
{
   BVCE_Telem_P_ParserState eParserState;
   unsigned uiBytesRead;
   unsigned uiBytesWritten;
   unsigned uiBytesInPayload;
   unsigned uiPaddingInPayload;

   struct
   {
      BVCE_Telem_P_FileHeaderState eState;
      unsigned uiBytesRead;
      uint32_t uiHeaderID;
      uint32_t uiHeaderLength;
      uint32_t uiTelemetryVersion;
      uint32_t uiProductVersion;
      uint32_t uiCPUSpeed;
   } stFileHeader;

   struct
   {
      BVCE_Telem_P_PacketHeaderState eState;

      uint8_t uiSyncByte;
      uint8_t uiFlags;
      uint8_t uiGroup;
      uint8_t uiStreamID;
   } stPacketHeader;
} BVCE_Telem_P_Context;

BERR_Code
BVCE_Telem_Create(
         BVCE_Telem_Handle *phVCETelem,
         const BVCE_Telem_Settings *pstTelemSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BVCE_Telem_Handle hVCETelem;

   BDBG_ENTER( BVCE_Telem_Create );

   BDBG_ASSERT( phVCETelem );
   BSTD_UNUSED( pstTelemSettings );

   if ( NULL == phVCETelem )
   {
      rc = BERR_TRACE( BERR_INVALID_PARAMETER );
   }
   else
   {
      *phVCETelem = NULL;

      hVCETelem = ( BVCE_Telem_Handle ) BKNI_Malloc( sizeof( BVCE_Telem_P_Context ) );

      if ( NULL == hVCETelem )
      {
         rc = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
      }
      else
      {
         BKNI_Memset( hVCETelem , 0, sizeof( BVCE_Telem_P_Context ) );

         *phVCETelem = hVCETelem;
      }
   }

   BDBG_LEAVE( BVCE_Telem_Create );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_Telem_Destroy(
         BVCE_Telem_Handle hVCETelem
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BVCE_Telem_Destroy );

   BDBG_ASSERT( hVCETelem );

   if ( NULL == hVCETelem )
   {
      rc = BERR_TRACE( BERR_INVALID_PARAMETER );
   }
   else
   {
      BKNI_Free( hVCETelem );
   }

   BDBG_LEAVE( BVCE_Telem_Destroy );

   return BERR_TRACE( rc );
}

#define SWAP32(x) ((x) = (((x) & 0xFF000000) >> 24 ) | (((x) & 0x00FF0000) >> 8 ) | (((x) & 0x0000FF00) << 8 ) | (((x) & 0x000000FF) << 24 ))
#define BVCE_TELEM_PACKET_HEADER_SYNC 0xBC
#define BVCE_TELEM_PACKET_HEADER_FLAGS_PAYLOAD_PRESENT 0x08
#define BVCE_TELEM_PACKET_HEADER_FLAGS_PAYLOAD_VARIABLE_LENGTH 0x40
#define BVCE_TELEM_PACKET_HEADER_STREAM_CONSOLE 0x02

BERR_Code
BVCE_Telem_Parse(
   BVCE_Telem_Handle hVCETelem,
   const uint8_t *pInputBuffer0, /* [in] */
   size_t uiInputLength0, /* [in] */
   const uint8_t *pInputBuffer1, /* [in] */
   size_t uiInputLength1, /* [in] */
   size_t *puiInputLengthRead, /* out */
   char *szOutputBuffer,   /* [out] pointer to buffer where log is copied to */
   size_t uiOutputLength,  /* [in] maximum number of bytes to copy to buffer */
   size_t *puiOutputLengthWritten  /* [out] number of bytes copied from debug log */
   )
{
   BERR_Code rc = BERR_SUCCESS;
   size_t uiTotalBytesToRead = uiInputLength0 + uiInputLength1;
   BDBG_ENTER( BVCE_Telem_Parse );

   BDBG_ASSERT( hVCETelem );
   BDBG_ASSERT( pInputBuffer0 );
   BDBG_ASSERT( puiInputLengthRead );
   BDBG_ASSERT( szOutputBuffer );
   BDBG_ASSERT( puiOutputLengthWritten );

   *puiInputLengthRead = 0;
   *puiOutputLengthWritten = 0;

   while ( ( *puiOutputLengthWritten < uiOutputLength )
           && ( *puiInputLengthRead < uiTotalBytesToRead )
           && ( BERR_SUCCESS == rc ) )
   {
      const uint8_t *puiByte;
      BVCE_Telem_P_ParserState eParserState = hVCETelem->eParserState;

      if ( *puiInputLengthRead < uiInputLength0 )
      {
         puiByte = pInputBuffer0 + *puiInputLengthRead;
      }
      else
      {
         puiByte = pInputBuffer1 + (*puiInputLengthRead - uiInputLength0);
      }

      switch ( hVCETelem->eParserState )
      {
         case BVCE_Telem_P_ParserState_eFileHeader:
            switch ( hVCETelem->stFileHeader.eState )
            {
               case BVCE_Telem_P_FileHeaderState_eHeaderID:
                  hVCETelem->stFileHeader.uiHeaderID <<= 8;
                  hVCETelem->stFileHeader.uiHeaderID |= *puiByte;
                  hVCETelem->uiBytesRead++;
                  (*puiInputLengthRead)++;

                  if ( 4 == hVCETelem->uiBytesRead )
                  {
                     SWAP32(hVCETelem->stFileHeader.uiHeaderID);
                     hVCETelem->uiBytesRead = 0;
                     hVCETelem->stFileHeader.eState++;
                  }
                  break;

               case BVCE_Telem_P_FileHeaderState_eHeaderLength:
                  hVCETelem->stFileHeader.uiHeaderLength <<= 8;
                  hVCETelem->stFileHeader.uiHeaderLength |= *puiByte;
                  hVCETelem->uiBytesRead++;
                  (*puiInputLengthRead)++;

                  if ( 4 == hVCETelem->uiBytesRead )
                  {
                     SWAP32(hVCETelem->stFileHeader.uiHeaderLength);
                     hVCETelem->uiBytesRead = 0;
                     hVCETelem->stFileHeader.eState++;
                  }
                  break;

               case BVCE_Telem_P_FileHeaderState_eTelemVersion:
                  hVCETelem->stFileHeader.uiTelemetryVersion <<= 8;
                  hVCETelem->stFileHeader.uiTelemetryVersion |= *puiByte;
                  hVCETelem->stFileHeader.uiBytesRead++;
                  hVCETelem->uiBytesRead++;
                  (*puiInputLengthRead)++;

                  if ( 4 == hVCETelem->uiBytesRead )
                  {
                     SWAP32(hVCETelem->stFileHeader.uiTelemetryVersion);
                     hVCETelem->uiBytesRead = 0;
                     hVCETelem->stFileHeader.eState++;
                  }
                  break;

               case BVCE_Telem_P_FileHeaderState_eProductVersion:
                  hVCETelem->stFileHeader.uiProductVersion <<= 8;
                  hVCETelem->stFileHeader.uiProductVersion |= *puiByte;
                  hVCETelem->stFileHeader.uiBytesRead++;
                  hVCETelem->uiBytesRead++;
                  (*puiInputLengthRead)++;

                  if ( 4 == hVCETelem->uiBytesRead )
                  {
                     SWAP32(hVCETelem->stFileHeader.uiProductVersion);
                     hVCETelem->uiBytesRead = 0;
                     hVCETelem->stFileHeader.eState++;
                  }
                  break;

               case BVCE_Telem_P_FileHeaderState_eCPUSpeed:
                  hVCETelem->stFileHeader.uiCPUSpeed <<= 8;
                  hVCETelem->stFileHeader.uiCPUSpeed |= *puiByte;
                  hVCETelem->stFileHeader.uiBytesRead++;
                  hVCETelem->uiBytesRead++;
                  (*puiInputLengthRead)++;

                  if ( 4 == hVCETelem->uiBytesRead )
                  {
                     SWAP32(hVCETelem->stFileHeader.uiCPUSpeed);
                     hVCETelem->uiBytesRead = 0;
                     hVCETelem->stFileHeader.eState++;
                  }
                  break;

               case BVCE_Telem_P_FileHeaderState_eDone:
                  if ( hVCETelem->stFileHeader.uiBytesRead < hVCETelem->stFileHeader.uiHeaderLength )
                  {
                     /* Skip Unknown Header Info */
                     hVCETelem->stFileHeader.uiBytesRead++;
                     (*puiInputLengthRead)++;
                  }
                  else
                  {
                     BDBG_MSG(("Header ID:%08x, Telemetry Version:%08x, Product Version:%08x, CPU Speed:%08x\n",
                        hVCETelem->stFileHeader.uiHeaderID,
                        hVCETelem->stFileHeader.uiTelemetryVersion,
                        hVCETelem->stFileHeader.uiProductVersion,
                        hVCETelem->stFileHeader.uiCPUSpeed
                        ));
                     hVCETelem->stFileHeader.uiHeaderLength = 0;
                     hVCETelem->uiBytesRead = 0;
                     hVCETelem->stFileHeader.eState = 0;
                     hVCETelem->eParserState = BVCE_Telem_P_ParserState_eFindSync;
                  }
                  break;
            }
            break;

         case BVCE_Telem_P_ParserState_eLostSync:
            if ( hVCETelem->uiBytesWritten < sizeof(BVCE_TELEM_P_LostSyncMessage)/sizeof(char) )
            {
               szOutputBuffer[*puiOutputLengthWritten] = BVCE_TELEM_P_LostSyncMessage[hVCETelem->uiBytesWritten];
               (*puiOutputLengthWritten)++;
               hVCETelem->uiBytesWritten++;
            }
            else
            {
               hVCETelem->uiBytesWritten = 0;
               hVCETelem->eParserState = BVCE_Telem_P_ParserState_eFindSync;
            }
            break;

         case BVCE_Telem_P_ParserState_eFindSync:
            if ( ( uiTotalBytesToRead - *puiInputLengthRead ) >= 4 )
            {
               if ( BVCE_TELEM_PACKET_HEADER_SYNC != puiByte[3] )
               {
                  (*puiInputLengthRead)++;
               }
               else
               {
                  hVCETelem->eParserState = BVCE_Telem_P_ParserState_ePacketHeader;
               }
            }
            else
            {
               BDBG_WRN(("Finding Sync..."));
               rc = BERR_UNKNOWN;
            }
            break;

         case BVCE_Telem_P_ParserState_ePacketHeader:
            switch ( hVCETelem->stPacketHeader.eState )
            {
               case BVCE_Telem_P_PacketHeaderState_eSyncByte:
                  hVCETelem->stPacketHeader.uiSyncByte = *puiByte;
                  if ( BVCE_TELEM_PACKET_HEADER_SYNC != hVCETelem->stPacketHeader.uiSyncByte )
                  {
                     BDBG_ERR(("Lost Sync!"));
                     hVCETelem->stPacketHeader.eState = 0;
                     hVCETelem->eParserState = BVCE_Telem_P_ParserState_eLostSync;
                  }
                  else
                  {
                     (*puiInputLengthRead)++;
                     hVCETelem->stPacketHeader.eState++;
                  }
                  break;

               case BVCE_Telem_P_PacketHeaderState_eFlags:
                  hVCETelem->stPacketHeader.uiFlags = *puiByte;
                  (*puiInputLengthRead)++;
                  hVCETelem->stPacketHeader.eState++;
                  break;

               case BVCE_Telem_P_PacketHeaderState_eGroup:
                  hVCETelem->stPacketHeader.uiGroup = *puiByte;
                  (*puiInputLengthRead)++;
                  hVCETelem->stPacketHeader.eState++;
                  break;

               case BVCE_Telem_P_PacketHeaderState_eStream:
                  hVCETelem->stPacketHeader.uiStreamID = *puiByte;
                  (*puiInputLengthRead)++;
                  hVCETelem->stPacketHeader.eState++;
                  break;

               case BVCE_Telem_P_PacketHeaderState_eDone:
                  hVCETelem->stPacketHeader.eState = 0;
                  hVCETelem->eParserState = BVCE_Telem_P_ParserState_eProcessTimestamp;
                  break;
            }
            break;

         case BVCE_Telem_P_ParserState_eProcessTimestamp:
            /* Skip Time Stamp */
            hVCETelem->uiBytesRead++;
            (*puiInputLengthRead)++;

            if ( 8 == hVCETelem->uiBytesRead )
            {
               hVCETelem->uiBytesRead = 0;
               hVCETelem->eParserState = BVCE_Telem_P_ParserState_eComputePayloadSize;
            }
           break;

         case BVCE_Telem_P_ParserState_eComputePayloadSize:
            if ( 0 != ( hVCETelem->stPacketHeader.uiFlags & BVCE_TELEM_PACKET_HEADER_FLAGS_PAYLOAD_PRESENT ) )
            {
               if ( 0 != ( hVCETelem->stPacketHeader.uiFlags & BVCE_TELEM_PACKET_HEADER_FLAGS_PAYLOAD_VARIABLE_LENGTH ) )
               {
                  /* Variable Length Payload */
                  hVCETelem->uiBytesInPayload <<= 8;
                  hVCETelem->uiBytesInPayload |= *puiByte;
                  hVCETelem->uiBytesRead++;
                  (*puiInputLengthRead)++;

                  if ( 4 == hVCETelem->uiBytesRead )
                  {
                     SWAP32(hVCETelem->uiBytesInPayload);
                     hVCETelem->uiPaddingInPayload = 4 - (hVCETelem->uiBytesInPayload % 4);
                     hVCETelem->uiBytesRead = 0;
                     hVCETelem->eParserState = BVCE_Telem_P_ParserState_eProcessPayload;
                  }
               }
               else
               {
                  /* Standard Payload */
                  hVCETelem->uiBytesInPayload = 4;
                  hVCETelem->uiPaddingInPayload = 0;
                  hVCETelem->eParserState = BVCE_Telem_P_ParserState_eProcessPayload;
               }
            }
            else
            {
               /* No Payload */
               hVCETelem->uiBytesInPayload = 0;
               hVCETelem->uiPaddingInPayload = 0;
               hVCETelem->eParserState = BVCE_Telem_P_ParserState_eDone;
            }
            break;

         case BVCE_Telem_P_ParserState_eProcessPayload:
            if ( hVCETelem->uiBytesRead < ( hVCETelem->uiBytesInPayload + hVCETelem->uiPaddingInPayload ) )
            {
               if ( hVCETelem->uiBytesRead < hVCETelem->uiBytesInPayload )
               {
                  if ( BVCE_TELEM_PACKET_HEADER_STREAM_CONSOLE == hVCETelem->stPacketHeader.uiStreamID )
                  {
                     szOutputBuffer[*puiOutputLengthWritten] = *puiByte;
                     (*puiOutputLengthWritten)++;
                  }
               }

               hVCETelem->uiBytesRead++;
               (*puiInputLengthRead)++;
            }
            else
            {
               hVCETelem->uiBytesRead = 0;
               hVCETelem->eParserState = BVCE_Telem_P_ParserState_eDone;
            }
            break;

         case BVCE_Telem_P_ParserState_eDone:
            hVCETelem->uiBytesRead = 0;
            hVCETelem->eParserState = BVCE_Telem_P_ParserState_ePacketHeader;
            break;
      }

      if ( hVCETelem->eParserState != eParserState )
      {
         BDBG_MODULE_MSG( BVCE_TELEM_STATE, ("eParserState (%s(%d) --> %s(%d))",
            BVCE_Telem_P_ParserStateLUT[eParserState],
            eParserState,
            BVCE_Telem_P_ParserStateLUT[hVCETelem->eParserState],
            hVCETelem->eParserState ));
      }
   }

   BDBG_LEAVE( BVCE_Telem_Parse );

   return BERR_TRACE( rc );
}
