/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer_ts.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 8/13/12 12:28p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer_ts.c $
 * 
 * Hydra_Software_Devel/11   8/13/12 12:28p nilesh
 * SW7425-3362: Merged FNRT support
 * 
 * Hydra_Software_Devel/SW7425-3362/1   7/5/12 1:54p nilesh
 * SW7425-3362: Added FNRTlib support
 * 
 * Hydra_Software_Devel/10   4/12/12 12:49p nilesh
 * SW7425-2568: Added mux config parser support
 * 
 * Hydra_Software_Devel/9   2/15/12 10:00a nilesh
 * SW7425-2217: Merged Pid2Buffer Support
 * 
 * Hydra_Software_Devel/SW7425-2217/1   1/26/12 5:27p nilesh
 * SW7425-2217: Added PID2Buffer support
 * 
 * Hydra_Software_Devel/8   2/13/12 5:59p nilesh
 * SW7425-2221: PID channels are now disabled/removed when mux is stopped
 * 
 * Hydra_Software_Devel/7   10/3/11 5:07p nilesh
 * SW7425-891: Added dual simul encoder support
 * 
 * Hydra_Software_Devel/6   9/12/11 1:46p nilesh
 * SW7425-891: Added dual encode support
 * 
 * Hydra_Software_Devel/5   6/8/11 12:01a nilesh
 * SW7425-484: Rename uiPacingCounterValue to uiPacingCounter
 * 
 * Hydra_Software_Devel/4   6/7/11 11:32p nilesh
 * SW7425-484: Updated to match XPT TsMux API.  Set bNonRealTimeMode in TS
 * muxlib start settings.
 * 
 * Hydra_Software_Devel/3   5/26/11 5:19p nilesh
 * SW7425-484: Added non-realtime support
 * 
 * Hydra_Software_Devel/2   5/26/11 3:37p nilesh
 * SW7425-117: Added support for different protocols. Added on-the-fly
 * PAT/PMT generation.
 * 
 * Hydra_Software_Devel/1   4/4/11 12:12p nilesh
 * SW7425-73: Merged MP4 Mux support
 * 
 * Hydra_Software_Devel/mp4_mux_devel/2   3/30/11 2:26a nilesh
 * SW7425-73: Moved include
 * 
 * Hydra_Software_Devel/mp4_mux_devel/1   3/30/11 2:00a nilesh
 * SW7425-73: Re-factored muxer to work with different containers
 *
 ***************************************************************************/

#include "bstd.h"
#include "muxer_ts.h"
#include "muxer_priv.h"

#include "bmuxlib_ts.h"

#include "bxpt.h"
#include "bxpt_playback.h"
#include "bxpt_rave.h"
#include "bxpt_tsmux.h"

#include "bvce.h"
#include "vce_encoder_priv.h"
#include "commands.h"
#include "param.h"

#include "string.h"

BDBG_MODULE(MUXER_TS);
BDBG_FILE_MODULE(MUXER_TS_STATUS);

extern BTST_P_Context *g_pContext;

#define BTST_DEFAULT_VIDEO_PID 0x0021
#define BTST_VIDEO_TRANSPORT_IDX 0

#define BTST_DEFAULT_AUDIO_PID 0x0022
#define BTST_AUDIO_TRANSPORT_IDX 1

#define BTST_SYSTEM_TRANSPORT_IDX 2

#define BTST_DEFAULT_PCR_PID 0x0012
#define BTST_DEFAULT_PCR_INTERVAL 50
#define BTST_PCR_TRANSPORT_IDX BTST_SYSTEM_TRANSPORT_IDX

#define BTST_DEFAULT_DPCR_INDEX 1

#define BTST_PAT_PID 0x0000
#define BTST_PAT_TRANSPORT_IDX BTST_SYSTEM_TRANSPORT_IDX
static const uint8_t s_auiTSPacket_PAT[188] =
{
   0x47,0x40,0x00,0x30,0xa6,0x40,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0x00,0x00,0xb0,0x0d,0x00,
   0x00,0xc1,0x00,0x00,
   0x00,0x01, /* Program Number */
   0xe0,0x0A, /* Program PID */
   0x65,0x80,0x5e,0xdc, /* CRC32 */
};

#define PAT_START_OFFSET 172
#define PAT_PMT_PID_OFFSET 183
#define PAT_CRC_OFFSET 184

#define BTST_DEFAULT_PMT_PID 0x0011
#define BTST_PMT_TRANSPORT_IDX BTST_SYSTEM_TRANSPORT_IDX
static const uint8_t s_auiTSPacket_PMT[188] =
{
   0x47,0x40,0x01,0x30,0xa1,0x40,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,
   0x00, /* Pointer Field */
   0x02, /* Table ID (PMT) */
   0xb0,0x12, /* Section Length */
   0x00,0x01, /* Program Number */
   0xc1, /* Version, Current/Next Indicator */
   0x00, /* Section Number */
   0x00, /* Last Section Number */
   0xe0, 0x12, /* PCR PID */
   0xf0,0x00, /* Program Info Length */
   0x10, /* Stream Type */
   0xe0,0x11, /* Elementary PID */
   0xf0,0x00, /* ES Info Length */
   0x3d,0x19,0x07,0x2f, /* CRC32 */
};

#define PMT_START_OFFSET 167
#define PMT_PID_OFFSET 2
#define PMT_PCR_PID_OFFSET 176
#define PMT_STREAM_TYPE_OFFSET 179
#define PMT_ES_PID_OFFSET 181
#define PMT_CRC_OFFSET 184

uint8_t auiStreamTypeLUT[BAVC_VideoCompressionStd_eMax] =
{
 0x1b, /* H.264 */
 0x02, /* MPEG2 */
 0xFF, /* H.261 */
 0xFF, /* H.263 */
 0xEA, /* VC1 */
 0x01, /* MPEG1 */
 0x02, /* MPEG2DTV */
 0xEA, /* VC1SimpleMain */
 0x10, /* MPEG4Part2 */
 0xFF, /* AVS */
 0x02, /* MPEG2_DSS_PES */
 0xFF, /* SVC */
 0xFF, /* SVC_BL */
 0xFF, /* MVC */
 0xFF/* VP6 */
};

/* Generate a CRC for the specified data/length */
/* Initialize crc to 0 for new calculation.  Use an old result for subsequent calls. */
static uint32_t CRC32_mpeg(
    uint32_t crc,
    uint8_t *data,
    int length
) {
        int j;
        crc = ~crc;
        while (length--)
        {
                for (j=0; j<8; j++)
                        crc = (crc<<1) ^ ((((*data >> (7-j)) ^ (crc >> 31)) & 1) ? 0x04c11db7 : 0);
                data++;
        }
        return crc;
}

/* Transport Stub */
#define MAX_TRANSPORT_CHANNELS 3

/* Transport Stub */
typedef struct TransportStubDeviceContext* TransportDeviceHandle;

typedef struct TransportDeviceSettings
{
      BCHP_Handle hChp;
      BREG_Handle hReg;
      BINT_Handle hInt;
      BMEM_Handle hMem;
      char *szFriendlyName;
      bool bNonRealTimeEncodeMode;
      unsigned uiDPCRIndex;
} TransportDeviceSettings;

BERR_Code
BTST_OpenTransportDevice(
         TransportDeviceHandle *phDeviceTransport,
         const TransportDeviceSettings* pstSettings
         );

BERR_Code
BTST_CloseTransportDevice(
         TransportDeviceHandle hDeviceTransport
         );

#if BTST_ENABLE_SELF_PACED_MUX
BERR_Code
BTST_TransportIncrementTime(
         TransportDeviceHandle hDeviceTransport,
         unsigned uiMilliseconds
         );
#endif

BERR_Code
BTST_GetTransportSettings(
         TransportDeviceHandle hDeviceTransport,
         BMUXlib_TS_TransportSettings *pstTransportSettings
         );

BERR_Code
BTST_SetTransportSettings(
         TransportDeviceHandle hDeviceTransport,
         const BMUXlib_TS_TransportSettings *pstTransportSettings
         );


BERR_Code
BTST_GetTransportStatus(
         TransportDeviceHandle hDeviceTransport,
         BMUXlib_TS_TransportStatus *pstTransportStatus
         );

/*********************/
/* Transport Channel */
/*********************/
typedef struct TransportStubContext* TransportChannelHandle;

typedef struct TransportChannelSettings
{
      uint32_t uiTransportIndex;
      uint32_t uiTransportDepth;
      char *szFriendlyName;
      unsigned uiDPCRIndex;
} TransportChannelSettings;

BERR_Code
BTST_OpenTransportChannel(
         TransportDeviceHandle hDeviceTransport,
         TransportChannelHandle *phChannelTransport,
         const TransportChannelSettings* pstSettings
         );

BERR_Code
BTST_CloseTransportChannel(
         TransportChannelHandle hChannelTransport
         );

BERR_Code
BTST_AddTransportDescriptors(
         void* pContext,
         const BMUXlib_TS_TransportDescriptor *astTransportDescriptors, /* Array of transport descriptors */
         size_t uiCount, /* Count of descriptors in array */
         size_t *puiQueuedCount /* Count of descriptors queued (*puiQueuedCount <= uiCount) */
         );

BERR_Code
BTST_GetCompletedTransportDescriptors(
         void* pContext,
         size_t *puiCompletedCount /* Count of descriptors completed */
         );

/**********/
/* Device */
/**********/
#ifndef BTST_VDC_ONLY /* otherwise hXpt is init in xport.c */
extern BXPT_Handle g_hXpt;
extern BXPT_Rave_Handle g_hRave;
#endif

typedef struct TransportStubContext
{
      bool bInUse;
      TransportDeviceHandle hDeviceTransport;

      TransportChannelSettings stSettings;

      uint32_t uiTransportDescriptorsReceived;

#ifdef BTST_P_USE_XPT_MUX_STUB
      const BMUXlib_TS_TransportDescriptor **astTransportDescriptors;
#else
      BXPT_Playback_Handle hXptPb;
      BXPT_PvrDescriptor8 *astPvrDescriptors;
#endif
#if BTST_P_DUMP_XPT_INPUT
      FILE *hPESOutput;
#endif

      uint32_t uiQueueSize;
      uint32_t uiTransportDescriptorsRead;
      uint32_t uiTransportDescriptorsWrite;
} TransportStubContext;

typedef struct TransportStubDeviceContext
{
      TransportDeviceSettings stSettings;

#ifndef BTST_P_USE_XPT_MUX_STUB
      BXPT_Handle hXpt;
      BXPT_PcrOffset_Handle hXptPcrOffset;
      BXPT_TsMux_Handle hXptTsMux;
#endif

      FILE *hFile;
      uint32_t uiCurrentESCR;
      uint32_t uiCurrentPacket2PacketDelta;

      BMUXlib_TS_TransportStatus stStatus;

      TransportStubContext ahChannel[MAX_TRANSPORT_CHANNELS];
} TransportStubDeviceContext;

BERR_Code
BTST_OpenTransportDevice(
         TransportDeviceHandle *phDeviceTransport,
         const TransportDeviceSettings* pstSettings
         )
{
   TransportStubDeviceContext *pTransportDeviceContext = NULL;

   BDBG_ASSERT( pstSettings );
   BDBG_ASSERT( pstSettings->hChp );
   BDBG_ASSERT( pstSettings->hInt );
   BDBG_ASSERT( pstSettings->hReg );
   BDBG_ASSERT( pstSettings->hMem );

   pTransportDeviceContext = ( TransportStubDeviceContext* ) BKNI_Malloc( sizeof( TransportStubDeviceContext ) );

   BKNI_Memset(
            pTransportDeviceContext,
            0,
            sizeof( TransportStubDeviceContext )
            );

   pTransportDeviceContext->stSettings = *pstSettings;

#ifndef BTST_P_USE_XPT_MUX_STUB
   {
#ifdef BTST_VDC_ONLY /* otherwise hXpt is init in xport.c */
      BXPT_DefaultSettings stXptSettings;

      BXPT_GetDefaultSettings(
               &stXptSettings,
               pstSettings->hChp
               );

      BXPT_Open(
               &pTransportDeviceContext->hXpt,
               pstSettings->hChp,
               pstSettings->hReg,
               pstSettings->hMem,
               pstSettings->hInt,
               &stXptSettings
               );

      {
         uint32_t uiInputBand;
         BXPT_InputBandConfig stInputBandConfig;

         for ( uiInputBand = 0; uiInputBand < BXPT_NUM_INPUT_BANDS; uiInputBand++ )
         {
            BXPT_GetInputBandConfig(
                     pTransportDeviceContext->hXpt,
                     uiInputBand,
                     &stInputBandConfig );

            stInputBandConfig.ForceValid = true;

            BXPT_SetInputBandConfig(
                     pTransportDeviceContext->hXpt,
                     uiInputBand,
                     &stInputBandConfig );
         }
      }
#else
      pTransportDeviceContext->hXpt = g_hXpt;
#endif

      /* Create PCROffset Handle */
      {
         BXPT_PcrOffset_Defaults Defaults;
         BXPT_PcrOffset_Settings Settings;

         BXPT_PcrOffset_GetChannelDefSettings( pTransportDeviceContext->hXpt, 1, &Defaults );  /* Pick a ChannelNo that is not used. 1 is just an example */
         BXPT_PcrOffset_Open( pTransportDeviceContext->hXpt, 1, &Defaults, &pTransportDeviceContext->hXptPcrOffset );
         BXPT_PcrOffset_GetSettings( pTransportDeviceContext->hXptPcrOffset, &Settings );

         Settings.UsePcrTimeBase = true;
         Settings.WhichPcr = pstSettings->uiDPCRIndex;  /* Where X is the same DPCR that the decoders are using */ /* TODO: Check this!!! */
         Settings.CountMode = BXPT_PcrOffset_StcCountMode_eBinary;
         Settings.BroadcastMode = BXPT_PcrOffset_BroadcastMode_eFull42;
         Settings.PidChannelNum = 0;

         BXPT_PcrOffset_SetSettings( pTransportDeviceContext->hXptPcrOffset, &Settings );
      }

      /* Create TSMux Handle */
      {
         BXPT_TsMux_Settings stTsMuxSettings;
         BXPT_TsMux_GetDefaultSettings( &stTsMuxSettings );

         stTsMuxSettings.uiMuxDelay = 100; /* 100 ms for now */
         stTsMuxSettings.hPcrOffset = pTransportDeviceContext->hXptPcrOffset;

         BXPT_TsMux_Create(
                  pTransportDeviceContext->hXpt,
                  &pTransportDeviceContext->hXptTsMux
             );

         BXPT_TsMux_SetSettings(
                         pTransportDeviceContext->hXptTsMux,
                         &stTsMuxSettings
                         );
      }

      BDBG_ASSERT( pTransportDeviceContext->hXpt );
   }
#endif

   *phDeviceTransport = pTransportDeviceContext;

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BTST_CloseTransportDevice(
         TransportDeviceHandle hDeviceTransport
         )
{
   BXPT_TsMux_Destroy(
            hDeviceTransport->hXptTsMux
       );

#ifndef BTST_P_USE_XPT_MUX_STUB
#ifdef BTST_VDC_ONLY /* otherwise hXpt is init in xport.c */
   BXPT_Close(
            hDeviceTransport->hXpt
            );
#endif
#endif

   BKNI_Free( hDeviceTransport );

   return BERR_TRACE( BERR_SUCCESS );
}

#ifdef BTST_P_USE_XPT_MUX_STUB
#if BTST_ENABLE_SELF_PACED_MUX
BERR_Code
BTST_TransportIncrementTime(
         TransportDeviceHandle hDeviceTransport,
         unsigned uiMilliseconds
         )
{
   hDeviceTransport->stStatus.uiESCR += 27000000 / 1000 * uiMilliseconds;
   hDeviceTransport->stStatus.uiSTC += 27000000 / 1000 * uiMilliseconds;
   hDeviceTransport->stStatus.uiSTC &= 0x3FFFFFFFFFFLLU; /* 42-bit wrap */

   return BERR_TRACE( BERR_SUCCESS );
}
#endif
#endif

BERR_Code
BTST_GetTransportSettings(
         TransportDeviceHandle hDeviceTransport,
         BMUXlib_TS_TransportSettings *pstTransportSettings
         )
{
   BXPT_TsMux_Settings stTsMuxSettings;

   BDBG_ENTER( BTST_GetTransportSettings );

   BXPT_TsMux_GetSettings(
            hDeviceTransport->hXptTsMux,
            &stTsMuxSettings);

   BKNI_Memset(
            pstTransportSettings,
            0,
            sizeof( BMUXlib_TS_TransportSettings )
            );

   pstTransportSettings->uiMuxDelay = stTsMuxSettings.uiMuxDelay;
   /* TODO: Add AFAP parameters */

   BDBG_LEAVE( BTST_GetTransportSettings );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BTST_SetTransportSettings(
         TransportDeviceHandle hDeviceTransport,
         const BMUXlib_TS_TransportSettings *pstTransportSettings
         )
{
   BXPT_TsMux_Settings stTsMuxSettings;

   BDBG_ENTER( BTST_SetTransportSettings );

   BXPT_TsMux_GetSettings(
            hDeviceTransport->hXptTsMux,
            &stTsMuxSettings);

   stTsMuxSettings.uiMuxDelay = pstTransportSettings->uiMuxDelay;

   if ( true == hDeviceTransport->stSettings.bNonRealTimeEncodeMode )
   {
      stTsMuxSettings.bAFAPMode = true;
      BDBG_ASSERT( true == pstTransportSettings->bNonRealTimeMode );
      stTsMuxSettings.AFAPSettings.uiPacingCounter = pstTransportSettings->stNonRealTimeSettings.uiPacingCounter;
      stTsMuxSettings.AFAPSettings.bEnablePacingPause = true;
      stTsMuxSettings.AFAPSettings.uiPacingSpeed = 32;
   }

   BXPT_TsMux_SetSettings(
            hDeviceTransport->hXptTsMux,
            &stTsMuxSettings);

   BDBG_LEAVE( BTST_SetTransportSettings );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BTST_GetTransportStatus(
         TransportDeviceHandle hDeviceTransport,
         BMUXlib_TS_TransportStatus *pstTransportStatus
         )
{
   BXPT_TsMux_Status stTsMuxStatus;

   BDBG_ENTER( BTST_GetTransportStatus );

   BDBG_ASSERT( hDeviceTransport );
   BDBG_ASSERT( pstTransportStatus );

   BXPT_TsMux_GetStatus(
            hDeviceTransport->hXptTsMux,
            &stTsMuxStatus
            );

   hDeviceTransport->stStatus.uiSTC = stTsMuxStatus.uiSTC;
   hDeviceTransport->stStatus.uiESCR = stTsMuxStatus.uiESCR;

   *pstTransportStatus = hDeviceTransport->stStatus;

   BDBG_LEAVE( BTST_GetTransportStatus );

   return BERR_TRACE( BERR_SUCCESS );
}

/***********/
/* Channel */
/***********/
BERR_Code
BTST_OpenTransportChannel(
         TransportDeviceHandle hDeviceTransport,
         TransportChannelHandle *phChannelTransport,
         const TransportChannelSettings* pstSettings
         )
{
   TransportStubContext *pTransportContext = &hDeviceTransport->ahChannel[pstSettings->uiTransportIndex];

   pTransportContext->stSettings = *pstSettings;

   pTransportContext->uiQueueSize = pTransportContext->stSettings.uiTransportDepth + 1;
#ifdef BTST_P_USE_XPT_MUX_STUB
   pTransportContext->astTransportDescriptors = ( const BMUXlib_TS_TransportDescriptor **) BKNI_Malloc( pTransportContext->uiQueueSize * sizeof( BMUXlib_TS_TransportDescriptor* ) );
#else
   pTransportContext->astPvrDescriptors = (BXPT_PvrDescriptor8 *) BMEM_AllocAligned(
            hDeviceTransport->stSettings.hMem,
            pTransportContext->uiQueueSize * sizeof( BXPT_PvrDescriptor8 ),
            4,
            0 );
   {
      BXPT_Playback_ChannelSettings stPlaybackSettings;

      BXPT_Playback_GetChannelDefaultSettings(
               hDeviceTransport->hXpt,
               pstSettings->uiTransportIndex,
               &stPlaybackSettings
               );

      if ( BTST_SYSTEM_TRANSPORT_IDX == pstSettings->uiTransportIndex )
      {
         stPlaybackSettings.SyncMode = BXPT_PB_SYNC_MPEG_BLIND;
         stPlaybackSettings.PacketLength = 0xBC;
      }
      else
      {
         stPlaybackSettings.SyncMode = BXPT_PB_SYNC_PES;
         stPlaybackSettings.PacketLength = 0xB8;
      }

      stPlaybackSettings.Use8WordDesc = true;
      stPlaybackSettings.PesBasedPacing = true;
      stPlaybackSettings.UsePcrTimeBase = true;
      stPlaybackSettings.WhichPcrToUse = pstSettings->uiDPCRIndex; /* Where X is the same DPCR that was set in BXPT_PcrOffset_Settings above */
      stPlaybackSettings.TimestampMode = BXPT_TimestampMode_e30_2U_Binary;
      stPlaybackSettings.Use32BitTimestamps = true;

      BDBG_MSG(("open playback channel %d", pstSettings->uiTransportIndex + BTST_MUXER_PB_CHANNEL_ID_BASE));
      BXPT_Playback_OpenChannel(
               hDeviceTransport->hXpt,
               &pTransportContext->hXptPb,
               pstSettings->uiTransportIndex + BTST_MUXER_PB_CHANNEL_ID_BASE, /* because playback channel lower is used by xvd decode */
               &stPlaybackSettings
               );

      BXPT_Playback_ConfigPacing(
               pTransportContext->hXptPb,
               BXPT_PacingControl_eStart
               );

      BXPT_TsMux_AddPlayback(
               hDeviceTransport->hXptTsMux,
               pTransportContext->hXptPb
               );

      BXPT_Playback_StartChannel(
               pTransportContext->hXptPb
               );

#if BTST_P_DUMP_XPT_INPUT
      {
         char filename[256];
         sprintf(filename, "encoder_output.%02d.%s",
                 pstSettings->uiTransportIndex + BTST_MUXER_PB_CHANNEL_ID_BASE,
                 (stPlaybackSettings.PacketLength == 0xB8) ? "pes" : "ts"
                );
         pTransportContext->hPESOutput = fopen(filename, "wb");
      }
#endif
   }
#endif

   pTransportContext->hDeviceTransport = hDeviceTransport;
   pTransportContext->bInUse = true;

   *phChannelTransport = pTransportContext;

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BTST_CloseTransportChannel(
         TransportChannelHandle hChannelTransport
         )
{
#if BTST_P_DUMP_XPT_INPUT
   fclose( hChannelTransport->hPESOutput );
#endif

#ifdef BTST_P_USE_XPT_MUX_STUB
   if ( NULL != hChannelTransport->astTransportDescriptors )
   {
      BKNI_Free(
               hChannelTransport->astTransportDescriptors
               );
      hChannelTransport->astTransportDescriptors = NULL;
   }
#else
   BXPT_Playback_StopChannel(
            hChannelTransport->hXptPb
            );

   BXPT_Playback_CloseChannel(
            hChannelTransport->hXptPb
            );
   hChannelTransport->hXptPb = NULL;

   if ( NULL != hChannelTransport->astPvrDescriptors )
   {
      BMEM_Free(
               hChannelTransport->hDeviceTransport->stSettings.hMem,
               hChannelTransport->astPvrDescriptors
               );
      hChannelTransport->astPvrDescriptors = NULL;
   }
#endif

   hChannelTransport->hDeviceTransport = NULL;
   hChannelTransport->bInUse = false;

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BTST_AddTransportDescriptors(
          void* pContext,
          const BMUXlib_TS_TransportDescriptor *astTransportDescriptors, /* Array of transport descriptors */
          size_t uiCount, /* Count of descriptors in array */
          size_t *puiQueuedCount /* Count of descriptors queued (*puiQueuedCount <= uiCount) */
          )
{
   TransportChannelHandle hChannelTransport = (TransportChannelHandle) pContext;
   uint32_t i;
   *puiQueuedCount = 0;

   for ( i = 0; (i < uiCount); i++ )
   {
      uint32_t uiTempWrite;
      uiTempWrite = hChannelTransport->uiTransportDescriptorsWrite + 1;
      uiTempWrite %= hChannelTransport->uiQueueSize;

      if ( uiTempWrite == hChannelTransport->uiTransportDescriptorsRead )
      {
         /* Queue is Full */
         break;
      }

#if BTST_P_DUMP_XPT_INPUT
      {
         BDBG_MSG(("To write %d bytes of PES", astTransportDescriptors[i].uiBufferLength));
         #if defined(EMULATION) /* prepare emulation client memory paging to avoid fwrite failure */
         {
            uint32_t uiBytesWritten = 0;

            while ( uiBytesWritten != astTransportDescriptors[i].uiBufferLength )
            {
               uiBytesWritten += BEMU_Client_Fwrite(
                        (void*) ((uint32_t) astTransportDescriptors[i].pBufferAddress + uiBytesWritten),
                        astTransportDescriptors[i].uiBufferLength - uiBytesWritten,
                        hChannelTransport->hPESOutput);
            }
            BDBG_MSG(("Wrote %d bytes of PES", uiBytesWritten));
         }

         #else
         fwrite( astTransportDescriptors[i].pBufferAddress,
                 1,
                 astTransportDescriptors[i].uiBufferLength,
                 hChannelTransport->hPESOutput);
         #endif

         fflush(hChannelTransport->hPESOutput);
      }
#endif

#ifdef BTST_P_USE_XPT_MUX_STUB
      hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsWrite] = astTransportDescriptors[i];
#else
      BKNI_Memset( &hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsWrite].Desc, 0, sizeof( BXPT_PvrDescriptor8 ) );
      BXPT_Playback_CreateDesc(
               hChannelTransport->hDeviceTransport->hXpt,
               &hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsWrite].Desc,
               (uint8_t *) astTransportDescriptors[i].pBufferAddress,
               astTransportDescriptors[i].uiBufferLength,
               false,
               false,
               NULL
               );

      BXPT_Tsmux_ConfigDesc(
               &hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsWrite],
               &astTransportDescriptors[i].stTsMuxDescriptorConfig
               );

      BDBG_MSG(("TRANSPORT[%d] (@%08x, %8u bytes, escr=%08x) - QUEUED",
               hChannelTransport->stSettings.uiTransportIndex,
               hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsWrite].Desc.BufferStartAddr,
               hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsWrite].Desc.BufferLength,
               hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsWrite].NextPacketPacingTimestamp
               ));

      BXPT_Playback_AddDescriptors(
               hChannelTransport->hXptPb,
               &hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsWrite].Desc,
               &hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsWrite].Desc
               );
#endif

      (*puiQueuedCount)++;
      hChannelTransport->uiTransportDescriptorsReceived++;
      hChannelTransport->uiTransportDescriptorsWrite = uiTempWrite;
   }

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BTST_GetCompletedTransportDescriptors(
         void* pContext,
         size_t *puiCompletedCount /* Count of descriptors completed */
         )
{
   TransportChannelHandle hChannelTransport = (TransportChannelHandle) pContext;

#ifndef BTST_P_USE_XPT_MUX_STUB
   BXPT_PvrDescriptor8 *pCurrentPvrDescriptor = NULL;
   BXPT_Playback_ChannelStatus stPlaybackStatus;

   BXPT_Playback_GetCurrentDescriptorAddress(
            hChannelTransport->hXptPb,
            (BXPT_PvrDescriptor **) &pCurrentPvrDescriptor
            );

   BXPT_Playback_GetChannelStatus(
            hChannelTransport->hXptPb,
            &stPlaybackStatus
            );
#endif

   *puiCompletedCount = 0;

   while ( hChannelTransport->uiTransportDescriptorsRead != hChannelTransport->uiTransportDescriptorsWrite )
   {
#ifdef BTST_P_USE_XPT_MUX_STUB
      uint32_t uiESCR;
      uint32_t uiPacket2PacketDelta;

      if ( hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->bNextPacketPacingTimestampValid )
      {
         uiESCR = hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiNextPacketPacingTimestamp;
      }
      else
      {
         uiESCR = hChannelTransport->hDeviceTransport->uiCurrentESCR;
      }

      if ( hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->bPacket2PacketTimestampDeltaValid)
      {
         uiPacket2PacketDelta = hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiPacket2PacketTimestampDelta;
      }
      else
      {
         uiPacket2PacketDelta = hChannelTransport->hDeviceTransport->uiCurrentPacket2PacketDelta;
      }

      hChannelTransport->hDeviceTransport->uiCurrentESCR = uiESCR;
      hChannelTransport->hDeviceTransport->uiCurrentPacket2PacketDelta = uiPacket2PacketDelta;

      if ( 0 != hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiBufferLength )
      {
         hChannelTransport->hDeviceTransport->uiCurrentESCR += (hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiBufferLength / 184) * hChannelTransport->hDeviceTransport->uiCurrentPacket2PacketDelta;
      }

#if BTST_ENABLE_SELF_PACED_MUX
      /* TODO: Handle wrap */
      if ( hChannelTransport->hDeviceTransport->uiCurrentESCR > (hChannelTransport->hDeviceTransport->stStatus.uiESCR & 0xFFFFFFFF) )
      {
         break;
      }
#endif

      BDBG_MSG(("TRANSPORT[%d] (@%08x, %8u bytes, rap=%d, escr=%08x (%d), pkt2pkt=%8x (%d))",
               hChannelTransport->stSettings.uiTransportIndex,
               hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->pBufferAddress,
               hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiBufferLength,
               hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->bRandomAccessIndication,
               hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiNextPacketPacingTimestamp,
               hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->bNextPacketPacingTimestampValid,
               hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiPacket2PacketTimestampDelta,
               hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->bPacket2PacketTimestampDeltaValid
               ));

#if 0
      if ( 0 != hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiBufferLength )
      {
         uint32_t j;
         BKNI_Printf("\t\t{\n\t\t");

         for (j = 0; j < hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiBufferLength && j <= 188; j++)
         {
            if ( j && (0 == (j % 8))) BKNI_Printf("\n\t\t");
            BKNI_Printf("%02x ", ((uint8_t*) hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->pBufferAddress)[j]);
         }
         if ( j != hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiBufferLength )
         {
            BKNI_Printf("...");
         }
         BKNI_Printf("\n\t\t}\n");

         if ( ( NULL != hChannelTransport->hFile )
              && ( 188 == hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiBufferLength ) )
         {
            fwrite(hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->pBufferAddress,
                   hChannelTransport->astTransportDescriptors[hChannelTransport->uiTransportDescriptorsRead]->uiBufferLength,
                   1,
                   hChannelTransport->hFile
                   );
         }
      }
#endif
#else
      if ( ( NULL != pCurrentPvrDescriptor )
           && ( ( pCurrentPvrDescriptor == &hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsRead] )
                && ( false == stPlaybackStatus.Finished ) )
         )
      {
         break;
      }

      BDBG_MSG(("TRANSPORT[%d] (@%08x, %8u bytes, escr=%08x) - DONE",
               hChannelTransport->stSettings.uiTransportIndex,
               hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsRead].Desc.BufferStartAddr,
               hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsRead].Desc.BufferLength,
               hChannelTransport->astPvrDescriptors[hChannelTransport->uiTransportDescriptorsRead].NextPacketPacingTimestamp
               ));

#endif
      (*puiCompletedCount)++;
      hChannelTransport->uiTransportDescriptorsRead++;
      hChannelTransport->uiTransportDescriptorsRead %= hChannelTransport->uiQueueSize;
   }


   return BERR_TRACE( BERR_SUCCESS );
}

/* Muxer */

static const uint8_t g_ProgramAssociationTable[] =
{
 0x00, 0x00, 0x00 /* TODO: Fill in PAT */
};

static const uint8_t g_ProgramMapTable[] =
{
 0x00, 0x00, 0x00 /* TODO: Fill in PMT */
};

/* PID channel number may be shared with decoder side playback */
extern int g_iXptChNum;

#define BTST_SYSTEMDATA_COUNT 2

#define BTST_TS_MAX_FILENAME 256

typedef struct BTST_TS_Muxer_Info
{
      BTST_P_Context *pTestContext;

      BMUXlib_TS_Handle hMuxTS;

      BREG_Handle               hReg;
      BMEM_Handle hMem;
      TransportDeviceHandle hDeviceTransport;
      TransportChannelHandle ahChannelTransport[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES];

      char szFilename[BTST_TS_MAX_FILENAME];
      bool bStartCapture;
      BTST_TaskHandle           hCaptureMuxOutputTask;

      FILE *hTSOutput;

      BMUXlib_TS_SystemData astSystemDataBuffer[BTST_SYSTEMDATA_COUNT];

      bool bAdvancedConfigMode;
      BAVC_VideoCompressionStd eProtocol;
      bool bNonRealTimeMode;
      bool bInsertPatPmt;

      BMUXlib_TS_StartSettings stStartSettings;
      BMUXlib_TS_MuxSettings stSettings;
} BTST_TS_Muxer_Info;

void* BTST_TS_OpenMux
   (BTST_P_Context *pContext)
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) BKNI_Malloc( sizeof( BTST_TS_Muxer_Info ) );

   BKNI_Memset(
            pstTsMuxerInfo,
            0,
            sizeof( BTST_TS_Muxer_Info )
            );

   pstTsMuxerInfo->pTestContext = pContext;
   pstTsMuxerInfo->hReg = pContext->hReg;
   pstTsMuxerInfo->hMem = pContext->astMemInfo[0].hMem;
   pstTsMuxerInfo->bInsertPatPmt = true;

   printf("Select Protocol (0=H.264, 1=MPEG2, 8=MPEG4Part2, 99=Advanced Config): ");
   BTST_P_Scanf(pContext, "%u", &pstTsMuxerInfo->eProtocol);
   if ( 99 == pstTsMuxerInfo->eProtocol )
   {
      pstTsMuxerInfo->bAdvancedConfigMode = true;
      pstTsMuxerInfo->eProtocol = BAVC_VideoCompressionStd_eH264;
      pstTsMuxerInfo->bNonRealTimeMode = false;
   }
   else
   {
      switch ( pstTsMuxerInfo->eProtocol )
      {
         case BAVC_VideoCompressionStd_eH264:
         case BAVC_VideoCompressionStd_eMPEG2:
         case BAVC_VideoCompressionStd_eMPEG4Part2:
            break;
         default:
            BKNI_Printf("Unsupported protocol. Defaulting to %d\n", BAVC_VideoCompressionStd_eH264);
            pstTsMuxerInfo->eProtocol = BAVC_VideoCompressionStd_eH264;
            break;
      }

      {
         unsigned iChoice;

         printf("Mux Mode (0=realtime, 1=non-realtime): ");
         BTST_P_Scanf(pContext, "%d", &iChoice);

         pstTsMuxerInfo->bNonRealTimeMode = ((0 == iChoice) ? false : true);
      }
   }

   /* Create TS Muxer */
   {
      BMUXlib_TS_CreateSettings stCreateSettings;
      BDBG_MSG(("Creating Muxer"));

      BMUXlib_TS_GetDefaultCreateSettings(
               &stCreateSettings
               );

      /* 7425 RTS requires cabac/secure memory to be loaded on memc0, so we also allocate the headers on memc0 */
      stCreateSettings.hMem = pContext->astMemInfo[0].hMem;

      BMUXlib_TS_Create(
               &pstTsMuxerInfo->hMuxTS,
               &stCreateSettings
               );

      BMUXlib_TS_GetDefaultMuxSettings(
               &pstTsMuxerInfo->stSettings
               );
   }

   return pstTsMuxerInfo;
}

void BTST_TS_ConfigMux
   (void *pMuxContext)
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) pMuxContext;
   BMUXlib_TS_MuxSettings *pstSettings = &pstTsMuxerInfo->stSettings;

   /* Configure Muxer */
   {
      BDBG_MSG(("Configuring Muxer"));

      PARAM_PREFIX(64)

      #define PARAM(_key,_field,_type,_help) \
         else if ( 0 == strcmp(PARAM_KEY,_key) )\
         {\
            pstSettings->_field = (_type) PARAM_VALUE;\
         }
      #include "mux_ts_params.txt"

      PARAM_HELP_PREFIX()

      #define PARAM(_key,_field,_type,_help) \
         BKNI_Printf("\""_key"\"" _help "\n");
      #include "mux_ts_params.txt"

      PARAM_HELP_POSTFIX()
      PARAM_POSTFIX()

      BMUXlib_TS_SetMuxSettings(
         pstTsMuxerInfo->hMuxTS,
         pstSettings
         );
   }
}

void BTST_TS_CloseMux
        (void *pMuxContext)
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) pMuxContext;

   /* Destroy TS Muxer */
   {
      BDBG_MSG(("Destroy Muxer"));
      BMUXlib_TS_Destroy(
               pstTsMuxerInfo->hMuxTS
               );
   }

   BKNI_Free(pstTsMuxerInfo);
}

BERR_Code
BTST_GetUserDataBuffer(
   void *pvContext,
   const void **pBuffer0, /* Pointer to a buffer of TS Packets. E.g. *pBuffer0 points to the beginning if the buffer.  Can be NULL if uiLength0=0. */
   size_t *puiLength0,
   const void **pBuffer1, /* Needed to handle FIFO wrap. Can be NULL if uiLength1=0. */
   size_t *puiLength1
   )
{
   BERR_Code rc;
   BXPT_RaveCx_Handle hRaveCtx = (BXPT_RaveCx_Handle) pvContext;
   BXPT_Rave_ContextPtrs CtxPtrs;

   *pBuffer0 = NULL;
   *puiLength0 = 0;
   *pBuffer1 = NULL;
   *puiLength1 = 0;

   rc = BXPT_Rave_CheckBuffer( hRaveCtx, &CtxPtrs );

   if ( BERR_SUCCESS == rc )
   {
      *pBuffer0 = CtxPtrs.Cdb.DataPtr;
      *puiLength0 = CtxPtrs.Cdb.ByteCount;

      *pBuffer1 = CtxPtrs.Cdb.WrapDataPtr;
      *puiLength1 = CtxPtrs.Cdb.WrapByteCount;
   }


   return BERR_TRACE( rc );
}

BERR_Code
BTST_ConsumeUserDataBuffer(
   void *pvContext,
   size_t uiNumBytesConsumed
   )
{
   BERR_Code rc;
   BXPT_RaveCx_Handle hRaveCtx = (BXPT_RaveCx_Handle) pvContext;
   BXPT_Rave_ContextPtrs CtxPtrs;

   BXPT_Rave_CheckBuffer( hRaveCtx, &CtxPtrs );

   rc = BXPT_Rave_UpdateReadOffset(
      hRaveCtx,
      uiNumBytesConsumed,
      CtxPtrs.Itb.ByteCount + CtxPtrs.Itb.WrapByteCount );

   return BERR_TRACE( rc );
}

void BTST_TS_StartMux
        (
           void *pMuxContext,
           void *pOutputContext
           )
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) pMuxContext;
   BMUXlib_VideoEncoderInterface *pstInputInterface = (BMUXlib_VideoEncoderInterface *) pOutputContext;

   /* Start Muxer */
   {
      BDBG_MSG(("Starting Muxer"));

      BMUXlib_TS_GetDefaultStartSettings(
               &pstTsMuxerInfo->stStartSettings
               );

      if ( false == pstTsMuxerInfo->bAdvancedConfigMode )
      {
         pstTsMuxerInfo->stStartSettings.bNonRealTimeMode = pstTsMuxerInfo->bNonRealTimeMode;
      }

      /* Setup Video */
      pstTsMuxerInfo->stStartSettings.video[0].uiPID = BTST_DEFAULT_VIDEO_PID;
      pstTsMuxerInfo->stStartSettings.video[0].stInputInterface = *pstInputInterface;
      pstTsMuxerInfo->stStartSettings.video[0].stInputInterface.stBufferInfo.eProtocol = pstTsMuxerInfo->eProtocol;
      pstTsMuxerInfo->stStartSettings.video[0].uiTransportChannelIndex = BTST_VIDEO_TRANSPORT_IDX;
      pstTsMuxerInfo->stStartSettings.video[0].uiPESStreamID = 0xE0;
      pstTsMuxerInfo->stStartSettings.uiNumValidVideoPIDs = 1;

#if 0
      /* Setup Audio */
      pstTsMuxerInfo->stStartSettings.audio[0].uiPID = BTST_DEFAULT_AUDIO_PID;
      pstTsMuxerInfo->stStartSettings.audio[0].stInputInterface.pContext = hAudioEncoder;
      pstTsMuxerInfo->stStartSettings.audio[0].stInputInterface.fGetBufferDescriptors = BTST_GetAudioBufferDescriptors;
      pstTsMuxerInfo->stStartSettings.audio[0].stInputInterface.fConsumeBufferDescriptors = BTST_ConsumeAudioBufferDescriptors;
      pstTsMuxerInfo->stStartSettings.audio[0].stInputInterface.fGetBufferStatus = BTST_GetAudioBufferStatus;
      pstTsMuxerInfo->stStartSettings.audio[0].uiTransportChannelIndex = BTST_AUDIO_TRANSPORT_IDX;
      pstTsMuxerInfo->stStartSettings.uiNumValidAudioPIDs = 1;
#endif

      /* Setup TS User Data */
      if ( 0 != g_pContext->uiNumValidPid2Buf )
      {
         unsigned i;

         for ( i = 0; (i < g_pContext->uiNumValidPid2Buf) && (i < BMUXLIB_TS_MAX_USERDATA_PIDS); i++ )
         {
            pstTsMuxerInfo->stStartSettings.userdata[i].stUserDataInterface.fGetUserDataBuffer = BTST_GetUserDataBuffer;
            pstTsMuxerInfo->stStartSettings.userdata[i].stUserDataInterface.fConsumeUserDataBuffer = BTST_ConsumeUserDataBuffer;
            pstTsMuxerInfo->stStartSettings.userdata[i].stUserDataInterface.pContext = g_pContext->pid2buf[i].hRaveCtx;
         }
         pstTsMuxerInfo->stStartSettings.uiNumValidUserdataPIDs = g_pContext->uiNumValidPid2Buf;
      }

      /* Setup PCR */
      pstTsMuxerInfo->stStartSettings.stPCRData.uiPID = BTST_DEFAULT_PCR_PID;
      pstTsMuxerInfo->stStartSettings.stPCRData.uiInterval = BTST_DEFAULT_PCR_INTERVAL;
      pstTsMuxerInfo->stStartSettings.stPCRData.uiTransportChannelIndex = BTST_PCR_TRANSPORT_IDX;

      /* Setup Mux Service Period */
      pstTsMuxerInfo->stStartSettings.uiServiceLatencyTolerance = 20;

      if ( true == pstTsMuxerInfo->bAdvancedConfigMode )
      {
         unsigned uiExtendedConfig;

         BKNI_Printf("Extended Config? (0=no,1=yes): ");
         BTST_P_Scanf(pstTsMuxerInfo->pTestContext, "%u", &uiExtendedConfig);

         if ( 1 == uiExtendedConfig )
         {
            BMUXlib_TS_StartSettings *pstStartSettings = &pstTsMuxerInfo->stStartSettings;

            PARAM_PREFIX(64)

            #define PARAM(_key,_field,_type,_help) \
               else if ( 0 == strcmp(PARAM_KEY,_key) )\
               {\
                  pstStartSettings->_field = (_type) PARAM_VALUE;\
               }
            #include "mux_ts_start_params.txt"

            #define PARAM(_key,_field,_type,_help) \
               else if ( 0 == strcmp(PARAM_KEY,_key) )\
               {\
                  pstTsMuxerInfo->_field = (_type) PARAM_VALUE;\
               }
            #include "mux_ts_systemdata_params.txt"

            PARAM_HELP_PREFIX()

            #define PARAM(_key,_field,_type,_help) \
               BKNI_Printf("\""_key"\"" _help "\n");
            #include "mux_ts_start_params.txt"
            #define PARAM(_key,_field,_type,_help) \
               BKNI_Printf("\""_key"\"" _help "\n");
            #include "mux_ts_systemdata_params.txt"

            PARAM_HELP_POSTFIX()
            PARAM_POSTFIX()
         }
      }

      /* Open Video Transport Stub */
      {
         TransportDeviceSettings stSettings;

         BDBG_MSG(("Open Transport Device"));

         BKNI_Memset(
                  &stSettings,
                  0,
                  sizeof( TransportDeviceSettings )
                  );

         stSettings.szFriendlyName = "XPT";
         stSettings.hChp = pstTsMuxerInfo->pTestContext->hChp;
         stSettings.hReg = pstTsMuxerInfo->pTestContext->hReg;
         stSettings.hInt = pstTsMuxerInfo->pTestContext->hInt;
         stSettings.hMem = pstTsMuxerInfo->pTestContext->astMemInfo[0].hMem;
         stSettings.bNonRealTimeEncodeMode = pstTsMuxerInfo->stStartSettings.bNonRealTimeMode;
         stSettings.uiDPCRIndex = BTST_DEFAULT_DPCR_INDEX;

         BTST_OpenTransportDevice(
                  &pstTsMuxerInfo->hDeviceTransport,
                  &stSettings
                  );
      }
      BKNI_Memset(pstTsMuxerInfo->ahChannelTransport, 0, sizeof(TransportChannelHandle) *  BMUXLIB_TS_MAX_TRANSPORT_INSTANCES );
      {
         TransportChannelSettings stSettings;

         BDBG_MSG(("Open Video Transport"));

         BKNI_Memset(
                  &stSettings,
                  0,
                  sizeof( TransportChannelSettings )
                  );

         stSettings.szFriendlyName = "Video XPT";
         stSettings.uiTransportDepth = 16;
         stSettings.uiTransportIndex = BTST_VIDEO_TRANSPORT_IDX;
         stSettings.uiDPCRIndex = BTST_DEFAULT_DPCR_INDEX;

         BTST_OpenTransportChannel(
                  pstTsMuxerInfo->hDeviceTransport,
                  &pstTsMuxerInfo->ahChannelTransport[BTST_VIDEO_TRANSPORT_IDX],
                  &stSettings
                  );
      }

      /* Open System Transport Stub */
      {
         TransportChannelSettings stSettings;

         BDBG_MSG(("Open System Transport"));

         BKNI_Memset(
                  &stSettings,
                  0,
                  sizeof( TransportChannelSettings )
                  );

         stSettings.szFriendlyName = "System XPT";
         stSettings.uiTransportDepth = 256;
         stSettings.uiTransportIndex = BTST_SYSTEM_TRANSPORT_IDX;
         stSettings.uiDPCRIndex = BTST_DEFAULT_DPCR_INDEX;

         BTST_OpenTransportChannel(
                  pstTsMuxerInfo->hDeviceTransport,
                  &pstTsMuxerInfo->ahChannelTransport[BTST_SYSTEM_TRANSPORT_IDX],
                  &stSettings
                  );
      }

      /* Setup Transport */
      pstTsMuxerInfo->stStartSettings.transport.stDeviceInterface.pContext = pstTsMuxerInfo->hDeviceTransport;
      pstTsMuxerInfo->stStartSettings.transport.stDeviceInterface.fGetTransportSettings = (BMUXlib_TS_GetTransportSettings) BTST_GetTransportSettings;
      pstTsMuxerInfo->stStartSettings.transport.stDeviceInterface.fSetTransportSettings = (BMUXlib_TS_SetTransportSettings) BTST_SetTransportSettings;
      pstTsMuxerInfo->stStartSettings.transport.stDeviceInterface.fGetTransportStatus = (BMUXlib_TS_GetTransportStatus) BTST_GetTransportStatus;

      pstTsMuxerInfo->stStartSettings.transport.stChannelInterface[BTST_VIDEO_TRANSPORT_IDX].pContext = pstTsMuxerInfo->ahChannelTransport[BTST_VIDEO_TRANSPORT_IDX];
      pstTsMuxerInfo->stStartSettings.transport.stChannelInterface[BTST_VIDEO_TRANSPORT_IDX].fAddTransportDescriptors = BTST_AddTransportDescriptors;
      pstTsMuxerInfo->stStartSettings.transport.stChannelInterface[BTST_VIDEO_TRANSPORT_IDX].fGetCompletedTransportDescriptors = BTST_GetCompletedTransportDescriptors;

#if 0
      pstTsMuxerInfo->stStartSettings.transport.stChannelInterface[BTST_AUDIO_TRANSPORT_IDX].pContext = hAudioTransport;
      pstTsMuxerInfo->stStartSettings.transport.stChannelInterface[BTST_AUDIO_TRANSPORT_IDX].fAddTransportDescriptors = BTST_AddTransportDescriptors;
      pstTsMuxerInfo->stStartSettings.transport.stChannelInterface[BTST_AUDIO_TRANSPORT_IDX].fGetCompletedTransportDescriptors = BTST_GetCompletedTransportDescriptors;
#endif

      pstTsMuxerInfo->stStartSettings.transport.stChannelInterface[BTST_SYSTEM_TRANSPORT_IDX].pContext = pstTsMuxerInfo->ahChannelTransport[BTST_SYSTEM_TRANSPORT_IDX];
      pstTsMuxerInfo->stStartSettings.transport.stChannelInterface[BTST_SYSTEM_TRANSPORT_IDX].fAddTransportDescriptors = BTST_AddTransportDescriptors;
      pstTsMuxerInfo->stStartSettings.transport.stChannelInterface[BTST_SYSTEM_TRANSPORT_IDX].fGetCompletedTransportDescriptors = BTST_GetCompletedTransportDescriptors;


      /* TODO: Move system data logic to a separate command? */
      if ( true == pstTsMuxerInfo->bInsertPatPmt )
      {
         BERR_Code rc = BERR_SUCCESS;
         unsigned i;

         for ( i = 0; i < BTST_SYSTEMDATA_COUNT; i++ )
         {
            BKNI_Memset( &pstTsMuxerInfo->astSystemDataBuffer[i], 0, sizeof( BMUXlib_TS_SystemData) );
            pstTsMuxerInfo->astSystemDataBuffer[i].pData = BMEM_Alloc( pstTsMuxerInfo->hMem, 188 );
            if ( NULL == pstTsMuxerInfo->astSystemDataBuffer[i].pData )
            {
                BDBG_ERR(("Cannot allocate memory"));
                rc = BERR_UNKNOWN;
                break;
            }
            pstTsMuxerInfo->astSystemDataBuffer[i].uiSize = 188;
         }

         if ( rc == BERR_SUCCESS )
         {
            pstTsMuxerInfo->astSystemDataBuffer[0].uiTimestampDelta = 0;
            BKNI_Memcpy(
                     (void *) pstTsMuxerInfo->astSystemDataBuffer[0].pData,
                     s_auiTSPacket_PAT,
                     sizeof( s_auiTSPacket_PAT )
                     );

            ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[0].pData)[PAT_PMT_PID_OFFSET] = BTST_DEFAULT_PMT_PID & 0xFF;
            {
               uint32_t uiCRC = CRC32_mpeg(0, (uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[0].pData + PAT_START_OFFSET, sizeof( s_auiTSPacket_PAT ) - ( PAT_START_OFFSET + 4 ));

               ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[0].pData)[PAT_CRC_OFFSET + 0] = (uiCRC >> 24) & 0xFF;
               ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[0].pData)[PAT_CRC_OFFSET + 1] = (uiCRC >> 16) & 0xFF;
               ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[0].pData)[PAT_CRC_OFFSET + 2] = (uiCRC >>  8) & 0xFF;
               ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[0].pData)[PAT_CRC_OFFSET + 3] = (uiCRC >>  0) & 0xFF;
            }

            pstTsMuxerInfo->astSystemDataBuffer[1].uiTimestampDelta = 0;
            BKNI_Memcpy(
                     (void *) pstTsMuxerInfo->astSystemDataBuffer[1].pData,
                     s_auiTSPacket_PMT,
                     sizeof( s_auiTSPacket_PMT )
                     );

            ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[1].pData)[PMT_PID_OFFSET] = BTST_DEFAULT_PMT_PID & 0xFF;
            ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[1].pData)[PMT_STREAM_TYPE_OFFSET] = auiStreamTypeLUT[pstTsMuxerInfo->eProtocol];
            ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[1].pData)[PMT_PCR_PID_OFFSET] = pstTsMuxerInfo->stStartSettings.stPCRData.uiPID & 0xFF;
            ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[1].pData)[PMT_ES_PID_OFFSET] = pstTsMuxerInfo->stStartSettings.video[0].uiPID & 0xFF;

            {
               uint32_t uiCRC = CRC32_mpeg(0, (uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[1].pData + PMT_START_OFFSET, sizeof( s_auiTSPacket_PMT ) - ( PMT_START_OFFSET + 4 ));

               ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[1].pData)[PMT_CRC_OFFSET + 0] = (uiCRC >> 24) & 0xFF;
               ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[1].pData)[PMT_CRC_OFFSET + 1] = (uiCRC >> 16) & 0xFF;
               ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[1].pData)[PMT_CRC_OFFSET + 2] = (uiCRC >>  8) & 0xFF;
               ((uint8_t *) pstTsMuxerInfo->astSystemDataBuffer[1].pData)[PMT_CRC_OFFSET + 3] = (uiCRC >>  0) & 0xFF;
            }

            {
               size_t uiSystemDataBufferQueued = 0;
               BMUXlib_TS_AddSystemDataBuffers(
                        pstTsMuxerInfo->hMuxTS,
                        (const BMUXlib_TS_SystemData *) pstTsMuxerInfo->astSystemDataBuffer,
                        BTST_SYSTEMDATA_COUNT,
                        &uiSystemDataBufferQueued
                        );

               BDBG_ASSERT( uiSystemDataBufferQueued == BTST_SYSTEMDATA_COUNT );
             }
         }
      }

      BMUXlib_TS_SetMuxSettings(
         pstTsMuxerInfo->hMuxTS,
         &pstTsMuxerInfo->stSettings
         );

      BMUXlib_TS_Start(
               pstTsMuxerInfo->hMuxTS,
               &pstTsMuxerInfo->stStartSettings
               );
   }
}

BERR_Code
BTST_TS_DoMux(
         void *pMuxContext,
         BMUXlib_DoMux_Status *pstStatus
   )
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) pMuxContext;

   return BMUXlib_TS_DoMux(pstTsMuxerInfo->hMuxTS, pstStatus);
}

void BTST_TS_FinishMux
        (void *pMuxContext)
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) pMuxContext;

   /* Finish Muxer */
   {
      BMUXlib_TS_FinishSettings stFinishSettings;
      BDBG_MSG(("Prepare for Stop"));

      BMUXlib_TS_GetDefaultFinishSettings(
               &stFinishSettings
               );

      BKNI_EnterCriticalSection();

      BMUXlib_TS_Finish(
               pstTsMuxerInfo->hMuxTS,
               &stFinishSettings
               );

      BKNI_LeaveCriticalSection();
   }
}

void BTST_TS_StopMux
        (void *pMuxContext)
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) pMuxContext;

   /* Stop Muxer */
   {
      BDBG_MSG(("Stopping"));
      BMUXlib_TS_Stop(
               pstTsMuxerInfo->hMuxTS
               );
   }

   BDBG_MSG(("Sleeping...press enter continue"));
#if 0
   BKNI_Sleep(1000);
#elif 1
   sleep(1);
#else
   {
      char str[255];
      gets(str);
   }
#endif

   {
      unsigned i;

      for ( i = 0; i < BTST_SYSTEMDATA_COUNT; i++ )
      {
         if ( NULL != pstTsMuxerInfo->astSystemDataBuffer[i].pData )
         {
            BMEM_Free( pstTsMuxerInfo->hMem, (void *) pstTsMuxerInfo->astSystemDataBuffer[i].pData );
            pstTsMuxerInfo->astSystemDataBuffer[i].pData = NULL;
         }
      }
   }

   /* Close System Transport Stub */
   {
      BDBG_MSG(("Close System Transport"));
      BTST_CloseTransportChannel(
               pstTsMuxerInfo->ahChannelTransport[BTST_SYSTEM_TRANSPORT_IDX]
               );

      pstTsMuxerInfo->ahChannelTransport[BTST_SYSTEM_TRANSPORT_IDX] = NULL;
   }

   /* Close Video Transport Stub */
   {
      BDBG_MSG(("Close Video Transport"));
      BTST_CloseTransportChannel(
               pstTsMuxerInfo->ahChannelTransport[BTST_VIDEO_TRANSPORT_IDX]
               );
      pstTsMuxerInfo->ahChannelTransport[BTST_VIDEO_TRANSPORT_IDX] = NULL;
   }

   /* Close Transport Device */
   {
      BDBG_MSG(("Close Transport Device"));

      BTST_CloseTransportDevice(
               pstTsMuxerInfo->hDeviceTransport
               );
   }
}

void BTST_TS_P_CaptureMuxBufferHandler
        ( BTST_TaskHandle          hTask,
          void                    *pContext )
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) pContext;
   unsigned vPidChannelId;
   unsigned pcrPidChannelId;
   unsigned patPidChannelId;
   unsigned pmtPidChannelId;

   BXPT_Rave_Handle hRave;
   BXPT_RaveCx_Handle hCtx;


   BSTD_UNUSED(hTask);

   /* hook up video playback channel with record (RAVE) channel for muxer;
    * TODO: hook up audio and system data playback channel with record channels as well for muxer!
    */
   {
#ifdef BTST_VDC_ONLY /* otherwise hRave is init in xport.c */
     BXPT_Rave_ChannelSettings stRaveSettings;
#endif
     BXPT_Rave_RecordSettings stRaveRecordSettings;

     BAVC_CdbItbConfig CdbItb =
     {
      { 512 * 1024, 4, true },
      { 512 * 1024, 4, true },
      false
     };

     BXPT_Playback_PacketizeConfig stPacketConfigV =
     {
      BTST_DEFAULT_VIDEO_PID,                                 /* PID. Will be placed into the TS header */
      BTST_VIDEO_TRANSPORT_IDX,                       /* Mapped to PID channel 0 */
      BXPT_Playback_PacketizerMode_Pes_MapAll,        /* Map everything, regardless of Stream ID */
      {
       0x00                                                                            /* Ignored, since we're mapping all PES streams */
      }
     };

     stPacketConfigV.Pid = pstTsMuxerInfo->stStartSettings.video[0].uiPID;

#ifdef BTST_VDC_ONLY /* otherwise hRave is init in xport.c */
     BXPT_Rave_GetChannelDefaultSettings(
              pstTsMuxerInfo->hDeviceTransport->hXpt,
              0, /* RAVE has only 1 channel for now, but with N contexts */
              &stRaveSettings
              );

     BDBG_MSG(("Opening RAVE channel %d", vPidChannelId));
     BXPT_Rave_OpenChannel(
              pstTsMuxerInfo->hDeviceTransport->hXpt,
              &hRave,
              0,
              &stRaveSettings
              );
     vPidChannelId = stPacketConfigV.ChannelNum;
#else
     /* PID channel is global resource shared between decoder and muxer */
     vPidChannelId = g_iXptChNum++;
     stPacketConfigV.ChannelNum = vPidChannelId;

     hRave = g_hRave;
#endif

     BDBG_MSG(("Allocating Record Context"));
     BXPT_Rave_AllocContext(
              hRave,
              BXPT_RaveCx_eRecord,
              &CdbItb,
              &hCtx
              );

     BXPT_Rave_GetRecordConfig(
              hCtx,
              &stRaveRecordSettings
              );

     stRaveRecordSettings.OutputFormat = BAVC_StreamType_eTsMpeg;
     stRaveRecordSettings.UseTimeStamps = false;
     stRaveRecordSettings.TsInitEn = false;
     stRaveRecordSettings.StreamIdHi = 0xEF;
     stRaveRecordSettings.StreamIdLo = 00;
     stRaveRecordSettings.BandHoldEn = true;

     {
        BXPT_Rave_ContextThresholds stRaveThresholds;

        BXPT_Rave_ComputeThresholds( hCtx, CdbItb.Cdb.Length, CdbItb.Itb.Length, &stRaveThresholds );
        stRaveRecordSettings.CdbUpperThreshold = stRaveThresholds.CdbUpper;
        stRaveRecordSettings.CdbLowerThreshold = 1;
        stRaveRecordSettings.ItbUpperThreshold = stRaveThresholds.ItbUpper;
        stRaveRecordSettings.ItbLowerThreshold = 1;
        BDBG_MSG(( "CdbUpperThreshold %u, CdbLowerThreshold %u, ItbUpperThreshold %u, ItbLowerThreshold %u",
            stRaveRecordSettings.CdbUpperThreshold, stRaveRecordSettings.CdbLowerThreshold, stRaveRecordSettings.ItbUpperThreshold, stRaveRecordSettings.ItbLowerThreshold ));
     }

     BDBG_MSG(("Setting Record Config"));
     BXPT_Rave_SetRecordConfig(
              hCtx,
              &stRaveRecordSettings
              );

     /* VIDEO Channel */
     BDBG_MSG(("Adding PID Channel %08x", vPidChannelId));
     BXPT_Rave_AddPidChannel(
              hCtx,
              vPidChannelId,
              false
              );

     BDBG_MSG(("Configuring PID Channel %d with PB band %d (PID:%04x)",
              vPidChannelId,
              BTST_MUXER_PB_CHANNEL_ID_BASE + BTST_VIDEO_TRANSPORT_IDX,
              pstTsMuxerInfo->stStartSettings.video[0].uiPID));
     BXPT_ConfigurePidChannel(/* connect PID channel with playback paser band */
              pstTsMuxerInfo->hDeviceTransport->hXpt,
              vPidChannelId,
              pstTsMuxerInfo->stStartSettings.video[0].uiPID,
              BXPT_PB_PARSER( BTST_MUXER_PB_CHANNEL_ID_BASE + BTST_VIDEO_TRANSPORT_IDX )
              );

     BDBG_MSG(("Enabling PID Channel"));
     BXPT_EnablePidChannel(
              pstTsMuxerInfo->hDeviceTransport->hXpt,
              vPidChannelId
              );

     BDBG_MSG(("Start Packetizing Stream"));
     BXPT_Playback_PacketizeStream(
              pstTsMuxerInfo->hDeviceTransport->ahChannel[BTST_VIDEO_TRANSPORT_IDX].hXptPb,
              0,
              &stPacketConfigV,
              true
              );
   }

   /* SYSTEM Channel */
   {

#ifdef BTST_VDC_ONLY
      pcrPidChannelId = BTST_PCR_TRANSPORT_IDX + 0;
      patPidChannelId = BTST_PAT_TRANSPORT_IDX + 1;
      pmtPidChannelId = BTST_PMT_TRANSPORT_IDX + 2;
#else
      pcrPidChannelId = g_iXptChNum++;
      patPidChannelId = g_iXptChNum++;
      pmtPidChannelId = g_iXptChNum++;
#endif
      /* PCR PID */
      {
         BDBG_MSG(("Adding PID Channel", pcrPidChannelId));
         BXPT_Rave_AddPidChannel(
                  hCtx,
                  pcrPidChannelId,
                  false
                  );

         BDBG_MSG(("Configuring PID Channel %d with PB band %d (PID:%04x)",
                  pcrPidChannelId,
                  BTST_MUXER_PB_CHANNEL_ID_BASE + BTST_PCR_TRANSPORT_IDX,
                  pstTsMuxerInfo->stStartSettings.stPCRData.uiPID));
         BXPT_ConfigurePidChannel(
                  pstTsMuxerInfo->hDeviceTransport->hXpt,
                  pcrPidChannelId,
                  pstTsMuxerInfo->stStartSettings.stPCRData.uiPID,
                  BXPT_PB_PARSER( BTST_MUXER_PB_CHANNEL_ID_BASE + BTST_PCR_TRANSPORT_IDX )
                  );

         BDBG_MSG(("Enabling PID Channel: %04d", pcrPidChannelId));
         BXPT_EnablePidChannel(
                  pstTsMuxerInfo->hDeviceTransport->hXpt,
                  pcrPidChannelId
                  );
      }

      /* PAT PID */
      {
         BDBG_MSG(("Adding PID Channel", patPidChannelId));
         BXPT_Rave_AddPidChannel(
                  hCtx,
                  patPidChannelId,
                  false
                  );

         BDBG_MSG(("Configuring PID Channel %d with PB band %d (PID:%04x)",
                  patPidChannelId,
                  BTST_MUXER_PB_CHANNEL_ID_BASE + BTST_PAT_TRANSPORT_IDX,
                  BTST_PAT_PID));
         BXPT_ConfigurePidChannel(
                  pstTsMuxerInfo->hDeviceTransport->hXpt,
                  patPidChannelId,
                  BTST_PAT_PID,
                  BXPT_PB_PARSER( BTST_MUXER_PB_CHANNEL_ID_BASE + BTST_PAT_TRANSPORT_IDX )
                  );

         BDBG_MSG(("Enabling PID Channel: %04d", patPidChannelId));
         BXPT_EnablePidChannel(
                  pstTsMuxerInfo->hDeviceTransport->hXpt,
                  patPidChannelId
                  );
      }

      /* PMT PID */
      {
         BDBG_MSG(("Adding PID Channel", pmtPidChannelId));
         BXPT_Rave_AddPidChannel(
                  hCtx,
                  pmtPidChannelId,
                  false
                  );

         BDBG_MSG(("Configuring PID Channel %d with PB band %d (PID:%04x)",
                  pmtPidChannelId,
                  BTST_MUXER_PB_CHANNEL_ID_BASE + BTST_PMT_TRANSPORT_IDX,
                  BTST_DEFAULT_PMT_PID));
         BXPT_ConfigurePidChannel(
                  pstTsMuxerInfo->hDeviceTransport->hXpt,
                  pmtPidChannelId,
                  BTST_DEFAULT_PMT_PID,
                  BXPT_PB_PARSER( BTST_MUXER_PB_CHANNEL_ID_BASE + BTST_PMT_TRANSPORT_IDX )
                  );

         BDBG_MSG(("Enabling PID Channel: %04d", pmtPidChannelId));
         BXPT_EnablePidChannel(
                  pstTsMuxerInfo->hDeviceTransport->hXpt,
                  pmtPidChannelId
                  );      }
   }

   BDBG_MSG(("Enabling Context"));
   BXPT_Rave_EnableContext(
            hCtx
            );

   while( pstTsMuxerInfo->bStartCapture )
   {
      BXPT_Rave_ContextPtrs stCtxPtrs;
      char filename[1024];
      static uint64_t uiTotalBytesWritten = 0;
      static uint32_t uiTotalBytesWrittenThisFile = 0;
      static uint32_t uiTotalInstancesWithZeroBytes = 0;
      static uint32_t uiTotalBytesWrittenSinceLastPrint = 0;
      static unsigned uiFileIndex = 0;

      BXPT_Rave_CheckBuffer(
               hCtx,
               &stCtxPtrs
               );

      /* Linux has 2GB file limit, so make sure we don't go beyond that */
      if ( NULL != pstTsMuxerInfo->hTSOutput )
      {
         if ( uiTotalBytesWrittenThisFile + stCtxPtrs.Cdb.ByteCount >= 0x80000000 )
         {
            fflush(pstTsMuxerInfo->hTSOutput);
            fclose(pstTsMuxerInfo->hTSOutput);
            pstTsMuxerInfo->hTSOutput = NULL;
         }
      }

      if ( NULL == pstTsMuxerInfo->hTSOutput )
      {
         uiTotalBytesWrittenThisFile = 0;
         sprintf(filename, "%s_%03d.ts", pstTsMuxerInfo->szFilename, uiFileIndex);
         pstTsMuxerInfo->hTSOutput = fopen(filename, "wb");
         uiFileIndex++;

         if ( NULL == pstTsMuxerInfo->hTSOutput )
         {
            BDBG_ERR(("Error Opening encoder output file %s", filename));
            return;
         }
      }

      if ( 0 != stCtxPtrs.Cdb.ByteCount )
      {
         BDBG_MSG(("To write %d bytes of TS", stCtxPtrs.Cdb.ByteCount));
         #if defined(EMULATION) /* prepare emulation client memory paging to avoid fwrite failure */
         {
            uint32_t uiBytesWritten = 0;

            while ( uiBytesWritten != stCtxPtrs.Cdb.ByteCount )
            {
               uiBytesWritten += BEMU_Client_Fwrite(
                                 (void *) ((uint32_t) stCtxPtrs.Cdb.DataPtr + uiBytesWritten),
                                 stCtxPtrs.Cdb.ByteCount - uiBytesWritten,
                                 pstTsMuxerInfo->hTSOutput
                                );
            }
            BDBG_MSG(("Wrote %d bytes of TS", uiBytesWritten));
         }
         #else

         fwrite(
                  (void *) stCtxPtrs.Cdb.DataPtr,
                  sizeof( char ),
                  stCtxPtrs.Cdb.ByteCount,
                  pstTsMuxerInfo->hTSOutput
                 );
         uiTotalBytesWritten += stCtxPtrs.Cdb.ByteCount;
         uiTotalBytesWrittenThisFile += stCtxPtrs.Cdb.ByteCount;
         uiTotalBytesWrittenSinceLastPrint += stCtxPtrs.Cdb.ByteCount;
         #endif

         BXPT_Rave_UpdateReadOffset(
                  hCtx,
                  stCtxPtrs.Cdb.ByteCount,
                  stCtxPtrs.Itb.ByteCount
                  );

         fflush(pstTsMuxerInfo->hTSOutput);
      }
      else
      {
         BDBG_MSG(("No TS data"));
         uiTotalInstancesWithZeroBytes++;
      }

      if ( uiTotalBytesWrittenSinceLastPrint > 1024*1024 )
      {
         BDBG_MODULE_MSG( MUXER_TS_STATUS, ("TS Bytes Written: %12llu (%d)", uiTotalBytesWritten, uiTotalInstancesWithZeroBytes));
         uiTotalBytesWrittenSinceLastPrint = 0;
      }

#if 0
      if ( uiTotalInstancesWithZeroBytes > 256 )
      {
         BKNI_Fail();
      }
#endif

      #if defined(EMULATION) /* emulator may be slow; sleep longer before next poll */
      BKNI_Sleep(10000); /* Wait 100 s before checking for more frames */
      #else
      BKNI_Sleep(100); /* Wait 100 ms before checking for more frames */
      #endif
   }

   /* disconnect video PID channel with muxer RAVE context;
    * TODO: same for audio and system data PID channels when they are added
    */

   BDBG_MSG(("Disabling vPID Channel %d", vPidChannelId));
   BXPT_DisablePidChannel(
            pstTsMuxerInfo->hDeviceTransport->hXpt,
            vPidChannelId
            );

   BDBG_MSG(("Removing vPID Channel %d", vPidChannelId));
   BXPT_Rave_RemovePidChannel(
            hCtx,
            vPidChannelId
            );

   BDBG_MSG(("Disabling PCR Channel %d", pcrPidChannelId));
   BXPT_DisablePidChannel(
            pstTsMuxerInfo->hDeviceTransport->hXpt,
            pcrPidChannelId
            );

   BDBG_MSG(("Removing PCR Channel %d", pcrPidChannelId));
   BXPT_Rave_RemovePidChannel(
            hCtx,
            pcrPidChannelId
            );

   BDBG_MSG(("Disabling PAT Channel %d", patPidChannelId));
   BXPT_DisablePidChannel(
            pstTsMuxerInfo->hDeviceTransport->hXpt,
            patPidChannelId
            );

   BDBG_MSG(("Removing PAT Channel %d", patPidChannelId));
   BXPT_Rave_RemovePidChannel(
            hCtx,
            patPidChannelId
            );

   BDBG_MSG(("Disabling PMT Channel %d", pmtPidChannelId));
   BXPT_DisablePidChannel(
            pstTsMuxerInfo->hDeviceTransport->hXpt,
            pmtPidChannelId
            );

   BDBG_MSG(("Removing PMT Channel %d", pmtPidChannelId));
   BXPT_Rave_RemovePidChannel(
            hCtx,
            pmtPidChannelId
            );

   BDBG_MSG(("Disabling Context"));
   BXPT_Rave_DisableContext(
            hCtx
            );

   BDBG_MSG(("Freeing Context"));
   BXPT_Rave_FreeContext(
            hCtx
            );

#if defined(BTST_VDC_ONLY) /* otherwise hXpt is init in xport.c */
   BDBG_MSG(("Closing RAVE"));
   BXPT_Rave_CloseChannel(
            hRave
            );
#endif
   if ( NULL != pstTsMuxerInfo->hTSOutput )
   {
      fclose( pstTsMuxerInfo->hTSOutput );
      pstTsMuxerInfo->hTSOutput = NULL;
   }
}

void
BTST_TS_StartCapture(
         void *pMuxContext,
         char* szFilename
         )
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) pMuxContext;
   pstTsMuxerInfo->bStartCapture = true;
   strncpy(pstTsMuxerInfo->szFilename, szFilename, BTST_TS_MAX_FILENAME);

   BTST_CreateTask(&pstTsMuxerInfo->hCaptureMuxOutputTask, BTST_TS_P_CaptureMuxBufferHandler, (void*)pstTsMuxerInfo);
}

void
BTST_TS_StopCapture(
         void *pMuxContext
         )
{
   BTST_TS_Muxer_Info *pstTsMuxerInfo = (BTST_TS_Muxer_Info *) pMuxContext;
   pstTsMuxerInfo->bStartCapture = false;
   BKNI_Sleep(100);
   BTST_DestroyTask(pstTsMuxerInfo->hCaptureMuxOutputTask);
   pstTsMuxerInfo->hCaptureMuxOutputTask = NULL;
}
