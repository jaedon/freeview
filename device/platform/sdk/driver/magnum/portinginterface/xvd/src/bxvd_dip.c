/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_dip.c $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 10/8/12 5:41p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_dip.c $
 * 
 * Hydra_Software_Devel/15   10/8/12 5:41p davidp
 * SW7445-16: Add support for 7445 platform.
 * 
 * Hydra_Software_Devel/14   8/7/12 4:47p davidp
 * SW7425-3668: Allow null mask in DIP_P_SetInterruptConfiguration.
 * 
 * Hydra_Software_Devel/13   1/11/12 4:59p davidp
 * SW7425-2132: Restore VDC RUL done interrupt mask register when
 * processing watchdog.
 * 
 * Hydra_Software_Devel/12   9/8/11 1:30p btosi
 * SW7425-1064: added debug messages to help debug dual decode 3D
 * 
 * Hydra_Software_Devel/11   7/20/11 3:07p davidp
 * SW7420-2001: Reorder header-file includes.
 * 
 * Hydra_Software_Devel/10   6/30/11 4:25p davidp
 * SW7425-826: Clear vsync interrupt but when registering vdc mask with
 * decoder.
 * 
 * Hydra_Software_Devel/9   6/27/11 4:18p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/8/11 7:07p davidp
 * SW7425-542: Add Display Info macros.
 * 
 * Hydra_Software_Devel/SW7425-542/1   6/7/11 5:51p davidp
 * SW7425-542: Add support for AVD FW Host Interface Memory API.
 * 
 * Hydra_Software_Devel/8   6/18/10 5:05p davidp
 * SW7405-2993: Fix compiler warning on non DEBUG build.
 * 
 * Hydra_Software_Devel/7   4/27/10 11:49a pblanco
 * SW7405-2993: Conditionalize inclusion of stdio.h
 * 
 * Hydra_Software_Devel/6   4/20/10 10:36a pblanco
 * SW7405-2993: Fix build error for 7403.
 * 
 * Hydra_Software_Devel/5   4/19/10 12:08p pblanco
 * SW7405-2993: FIx build error for 7401.
 * 
 * Hydra_Software_Devel/4   4/16/10 10:43a pblanco
 * SW7405-2993: Add latency measuring code. Conditionalized compile for
 * now, run time control later.
 * 
 * Hydra_Software_Devel/3   3/16/10 3:35p davidp
 * SW7405-4010: Save local copy of vsync_parity, stc_snapshot and
 * stc1_snapshot to prevent inconsistent data from being used.
 * 
 * Hydra_Software_Devel/2   3/8/10 4:40p davidp
 * SW7405-2993: Fix non-debug build issue.
 * 
 * Hydra_Software_Devel/1   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"          /* For malloc */
#include "bmem.h"
#include "bxvd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_dip.h"

#if (BCHP_CHIP == 7401 || BCHP_CHIP == 7403)
#include "bchp_decode_ip_shim.h"
#elif (BCHP_CHIP != 7445)
#include "bchp_decode_ip_shim_0.h"
#endif

BDBG_MODULE(BXVD_DIP);
BDBG_FILE_MODULE(BXVD_DIPCTL);
BDBG_FILE_MODULE(BXVD_DIPDBG);

#define BXVD_DIP_P_MEASURE_LATENCY 0
#define LATENCY_FILE_OUTPUT 1

#if BXVD_DIP_P_MEASURE_LATENCY
#include <stdio.h>
#endif

typedef struct BXVD_DisplayInterruptProvider_P_DisplayInterruptHandler
{
      BXDM_DisplayInterruptHandler_isr fCallback;
      void *pPrivateContext;
} BXVD_DisplayInterruptProvider_P_DisplayInterruptHandler;

typedef struct BXVD_DisplayInterruptProvider_P_ChannelContext
{
      BXVD_DisplayInterruptProvider_P_ChannelSettings stChannelSettings;

      BXVD_DisplayInterruptProvider_P_InterruptSettings stInterruptSettings;
      uint32_t uiRegMaskCurrent;
      bool bInterruptSettingsValid;

      /* BINT Callback Handle */
      BINT_CallbackHandle hPictureDataReadyInterruptCallback;
      BXDM_DisplayInterruptInfo stDisplayInterruptInfo;

      /* Application Callback */
      BXVD_DisplayInterruptProvider_P_DisplayInterruptHandler stDisplayInterruptHandler;
} BXVD_DisplayInterruptProvider_P_ChannelContext;

#if BXVD_DIP_P_MEASURE_LATENCY

#define BXVD_DIP_MAX_TIME_SAMPLES 2048

/* DM's persistent state between BXVD_StartDecode and
 * BXVD_StopDecode */

static uint32_t guiSampleIndex;
static uint32_t gauiTimeSample[BXVD_DIP_MAX_TIME_SAMPLES];


void BXVD_DIP_P_SampleTime(BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh)
{
   uint32_t uiTemp;
   BXVD_Handle hXvd = hXvdDipCh->stChannelSettings.hXvd;
   BXVD_ChannelHandle hCh = hXvd->ahChannel[0];
   BXVD_STC eStc;

   if (guiSampleIndex >= BXVD_DIP_MAX_TIME_SAMPLES)
   {
      BKNI_Printf("\nBXVD_DIP_P_SampleTime too many samples\n");
   }
   else
   {
#if (BCHP_CHIP == 7445)
      BDBG_ERR(("NEED TO ADD STC Read Macro in BXVD_DIP_P_SampleTime"));
#else
      if (hXvd->uDecoderInstance == 0)
      {
	 BXVD_GetSTCSource(hCh, &eStc);
         if (BXVD_STC_eZero == eStc  )
         {
            uiTemp = BREG_Read32(hXvd->hReg, BCHP_DECODE_IP_SHIM_0_STC0_REG);
         }
         else
         {
            uiTemp = BREG_Read32(hXvd->hReg, BCHP_DECODE_IP_SHIM_0_STC1_REG);
         }
      }
      else
#endif
      {
         uiTemp = 0xFFFFFFFF;
      }
      
      gauiTimeSample[guiSampleIndex] = uiTemp;
      guiSampleIndex++;
   }
}

static void BXVD_DIP_S_PrintSampleTimes(void *pXvdDipCh, uint32_t uiStcFromDecoder)
{
#if LATENCY_FILE_OUTPUT
   FILE *outfd;
#endif
   int32_t iNum45kTicks;
   int32_t iUsecs;

   BSTD_UNUSED(pXvdDipCh);

#if LATENCY_FILE_OUTPUT
   outfd = fopen("timeresults.txt", "a+");
   if (outfd == NULL)
   {
      BKNI_Printf("BXVD_DIP_S_PrintSampleTimes is unable to open output file\n");
      return;
   }
#endif
   /* Sampled STC, AVD STC snapshot and latency between the two. */
   iNum45kTicks = (int)gauiTimeSample[0] - (int)uiStcFromDecoder;
   iUsecs = ( iNum45kTicks * 1000 * 1000 ) / 45000;

#if !LATENCY_FILE_OUTPUT
   BKNI_Printf("%u,%u,%d,",
         uiStcFromDecoder,
         gauiTimeSample[0],
         iUsecs 
         );
#else
   fprintf(outfd, "%u,%u,%d,", uiStcFromDecoder, gauiTimeSample[0], iUsecs);
#endif

   /* The elapse time for the DM ISR routine. */
   iNum45kTicks = gauiTimeSample[2] - gauiTimeSample[0];
   iUsecs = ( iNum45kTicks * 1000 * 1000 ) / 45000;
#if !LATENCY_FILE_OUTPUT
   BKNI_Printf("%u,", iUsecs );
#else
   fprintf(outfd, "%u,", iUsecs );
#endif
   /* Execution time for callbacks. */
   iNum45kTicks = gauiTimeSample[2] - gauiTimeSample[1];
   iUsecs = ( iNum45kTicks * 1000 * 1000 ) / 45000;
#if !LATENCY_FILE_OUTPUT
   BKNI_Printf("%u ", iUsecs );
   BKNI_Printf("\n");
#else
   fprintf(outfd, "%u\n", iUsecs );
   fflush(outfd);
   fclose(outfd);
#endif
}
#endif

BERR_Code
BXVD_DisplayInterruptProvider_P_GetDefaultChannelSettings(
         BXVD_DisplayInterruptProvider_P_ChannelSettings *pstXvdDipChSettings
         )
{
   BDBG_ENTER( BXVD_DisplayInterruptProvider_P_GetDefaultChannelSettings );

   BDBG_ASSERT( pstXvdDipChSettings );

   BKNI_Memset( pstXvdDipChSettings, 0, sizeof( BXVD_DisplayInterruptProvider_P_ChannelSettings ) );

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_P_GetDefaultChannelSettings );
   return BERR_TRACE(BERR_SUCCESS);
}

void
BXVD_DisplayInterruptProvider_S_PictureDataReady_isr(
         void* pXvdDipCh,
         int iParam2
         )
{
   BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh = (BXVD_DisplayInterruptProvider_P_ChannelHandle) pXvdDipCh;

   BXVD_P_DisplayInfo stDisplayInfo;
   BXVD_P_DisplayInfo stUpdateDisplayInfo;

   uint32_t i;

#if BDBG_DEBUG_BUILD
   BXVD_DisplayInterrupt eDisplayInterrupt = ( BXVD_DisplayInterrupt ) iParam2;

   BDBG_ASSERT( eDisplayInterrupt == hXvdDipCh->stChannelSettings.eDisplayInterrupt );
#else
   BSTD_UNUSED(iParam2);
#endif

   BDBG_ASSERT( hXvdDipCh );

#if BXVD_DIP_P_MEASURE_LATENCY
   guiSampleIndex = 0;
   BKNI_Memset( gauiTimeSample, 0, (sizeof( uint32_t) * BXVD_DIP_MAX_TIME_SAMPLES) );

   BXVD_DIP_P_SampleTime(hXvdDipCh);
#endif

   /*************************************/
   /* Update the Display Interrupt Info */
   /*************************************/

   for ( i = 0; i < 2; i++ )
   {
      BXVD_P_READ_DISPLAY_INFO(hXvdDipCh, stDisplayInfo);

      /* Extract Interrupt Polarity */
      {
         uint32_t uiTempBits = stDisplayInfo.vsync_parity &
            ( hXvdDipCh->stInterruptSettings.stRULIDMasks.ui32BottomFieldRULIDMask |
              hXvdDipCh->stInterruptSettings.stRULIDMasks.ui32TopFieldRULIDMask |
              hXvdDipCh->stInterruptSettings.stRULIDMasks.ui32ProgressiveFieldRULIDMask );

         /* Check if more than one vsync polarity bit is set */
         if (uiTempBits & (uiTempBits - 1))
         {
            BDBG_WRN(("Multiple bits set in the vsync polarity bit field (0x%08x)", stDisplayInfo.vsync_parity));
         }
      }

      if ( stDisplayInfo.vsync_parity & hXvdDipCh->stInterruptSettings.stRULIDMasks.ui32BottomFieldRULIDMask )
      {
         hXvdDipCh->stDisplayInterruptInfo.eInterruptPolarity = BAVC_Polarity_eBotField ;
      }
      else if ( stDisplayInfo.vsync_parity & hXvdDipCh->stInterruptSettings.stRULIDMasks.ui32TopFieldRULIDMask )
      {
         hXvdDipCh->stDisplayInterruptInfo.eInterruptPolarity = BAVC_Polarity_eTopField ;
      }
      else if ( stDisplayInfo.vsync_parity & hXvdDipCh->stInterruptSettings.stRULIDMasks.ui32ProgressiveFieldRULIDMask )
      {
         hXvdDipCh->stDisplayInterruptInfo.eInterruptPolarity = BAVC_Polarity_eFrame ;
      }
      else
      {
         /* We default to TopField for the vsync polarity if we don't
          * recognize the polarity type */
         BDBG_WRN(("Unrecognized VSYNC polarity type (0x%08x)", stDisplayInfo.vsync_parity));

         hXvdDipCh->stDisplayInterruptInfo.eInterruptPolarity = BAVC_Polarity_eTopField ;
      }

      /* Verify that the AVD FW hasn't updated the STC and Vsync Parity, meaning something is keeping the 
         PDR ISR from being serviced in a timely manner */

      BXVD_P_READ_DISPLAY_INFO(hXvdDipCh, stUpdateDisplayInfo);

      if (!BXVD_P_IS_DISPLAY_INFO_EQUAL(stUpdateDisplayInfo, stDisplayInfo))
      {
         BDBG_ERR(("AVD Picture Data Ready interrupt not processed in time!")); 
      }
      else
      {
         break;
      }
   }

   /* Extract STC snapshot(s) */
   BDBG_ASSERT( BXVD_P_STC_MAX == hXvdDipCh->stDisplayInterruptInfo.uiSTCCount );

   BXVD_P_SAVE_DIP_INFO_STC(hXvdDipCh, stDisplayInfo);

   /* Increment Interrupt Count */
   hXvdDipCh->stDisplayInterruptInfo.stInterruptCount.uiValue++;
   hXvdDipCh->stDisplayInterruptInfo.stInterruptCount.bValid = true;

   /* Execute the application's Picture Data Ready callback */
   if( NULL != hXvdDipCh->stDisplayInterruptHandler.fCallback )
   {
#if BXVD_DIP_P_MEASURE_LATENCY
      BXVD_DIP_P_SampleTime(hXvdDipCh);
#endif
      
      {
         /*
          * If enabled, print out a debug message every vsync.
          */
         char cPolarity;
         switch( hXvdDipCh->stDisplayInterruptInfo.eInterruptPolarity )
         {
            case BAVC_Polarity_eTopField:    cPolarity='T';    break;
            case BAVC_Polarity_eBotField:    cPolarity='B';    break;
            case BAVC_Polarity_eFrame:       cPolarity='F';    break;
            default:                         cPolarity='u';    break;
         }

         BDBG_MODULE_MSG( BXVD_DIPDBG, ("%08x %d.%d %c stc:%08x(%d) %d",
                  hXvdDipCh,
                  hXvdDipCh->stChannelSettings.hXvd->uDecoderInstance,
                  hXvdDipCh->stChannelSettings.eDisplayInterrupt,
                  cPolarity,
                  hXvdDipCh->stDisplayInterruptInfo.astSTC[0].uiValue,
                  hXvdDipCh->stDisplayInterruptInfo.astSTC[0].bValid,
                  hXvdDipCh->stDisplayInterruptInfo.stInterruptCount.uiValue
                  ));
      }

      hXvdDipCh->stDisplayInterruptHandler.fCallback(
               hXvdDipCh->stDisplayInterruptHandler.pPrivateContext,
               &hXvdDipCh->stDisplayInterruptInfo
               );
#if BXVD_DIP_P_MEASURE_LATENCY
      BXVD_DIP_P_SampleTime(hXvdDipCh);
#endif
   }

#if BXVD_DIP_P_MEASURE_LATENCY
#if !BXVD_P_FW_HIM_API

   BXVD_DIP_S_PrintSampleTimes(hXvdDipCh, stDisplayInfo.stc_snapshot);
#else
   BXVD_DIP_S_PrintSampleTimes(hXvdDipCh, stDisplayInfo.stc_snapshot[0]);
#endif

#endif

   return;
}

static BERR_Code
BXVD_DisplayInterruptProvider_S_SetupInterrupts(
         BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh
         )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_DisplayInterruptProvider_S_SetupInterrupts );

   BDBG_ASSERT( hXvdDipCh );

   if ( hXvdDipCh->hPictureDataReadyInterruptCallback ) {
      rc = BINT_DisableCallback( hXvdDipCh->hPictureDataReadyInterruptCallback );
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   } else {
      rc = BINT_CreateCallback(
               &hXvdDipCh->hPictureDataReadyInterruptCallback,
               hXvdDipCh->stChannelSettings.hInterrupt,
               hXvdDipCh->stChannelSettings.interruptId,
               BXVD_DisplayInterruptProvider_S_PictureDataReady_isr,
               ( void* ) hXvdDipCh,
               hXvdDipCh->stChannelSettings.eDisplayInterrupt
               );

      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }

   rc = BINT_EnableCallback( hXvdDipCh->hPictureDataReadyInterruptCallback );
   if (rc != BERR_SUCCESS )
   {
      return BERR_TRACE(rc);
   }

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_S_SetupInterrupts );

   return BERR_TRACE( BERR_SUCCESS );
}

static BERR_Code
BXVD_DisplayInterruptProvider_S_TeardownInterrupts(
         BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh
         )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_DisplayInterruptProvider_S_TeardownInterrupts );

   BDBG_ASSERT( hXvdDipCh );

   if ( hXvdDipCh->hPictureDataReadyInterruptCallback ) {
      rc = BINT_DisableCallback( hXvdDipCh->hPictureDataReadyInterruptCallback );
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }

      rc = BINT_DestroyCallback( hXvdDipCh->hPictureDataReadyInterruptCallback );
      if (rc != BERR_SUCCESS )
      {
         return BERR_TRACE(rc);
      }

      hXvdDipCh->hPictureDataReadyInterruptCallback = NULL;
   }

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_S_TeardownInterrupts );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXVD_DisplayInterruptProvider_P_OpenChannel(
         BXVD_DisplayInterruptProvider_P_ChannelHandle *phXvdDipCh,
         const BXVD_DisplayInterruptProvider_P_ChannelSettings *pstXvdDipChSettings
         )
{
   BERR_Code rc;
   BXVD_DisplayInterruptProvider_P_ChannelContext *pXvdDipCh = NULL;

   BDBG_ENTER( BXVD_DisplayInterruptProvider_P_OpenChannel );

   BDBG_ASSERT( phXvdDipCh );
   BDBG_ASSERT( pstXvdDipChSettings );

   /* Set the handle to NULL in case the allocation fails */
   *phXvdDipCh = NULL;

   pXvdDipCh = ( BXVD_DisplayInterruptProvider_P_ChannelContext* ) BKNI_Malloc( sizeof( BXVD_DisplayInterruptProvider_P_ChannelContext ) );
   if ( NULL == pXvdDipCh )
   {
      return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }

   /* Zero out the newly allocated context */
   BKNI_Memset( ( void * ) pXvdDipCh, 0, sizeof( BXVD_DisplayInterruptProvider_P_ChannelContext ) );

   /* Allocate STC snapshot array */
   pXvdDipCh->stDisplayInterruptInfo.uiSTCCount = BXVD_P_STC_MAX;
   pXvdDipCh->stDisplayInterruptInfo.astSTC = ( BXDM_QualifiedValue * ) BKNI_Malloc( sizeof ( BXDM_QualifiedValue ) * pXvdDipCh->stDisplayInterruptInfo.uiSTCCount );
   if ( NULL == pXvdDipCh->stDisplayInterruptInfo.astSTC )
   {
      BXVD_DisplayInterruptProvider_P_CloseChannel( pXvdDipCh );
      return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }

   /* TODO: Validate the channel settings */
   pXvdDipCh->stChannelSettings = *pstXvdDipChSettings;

   /* Install Picture Data Ready Callback Handler */
   rc = BXVD_DisplayInterruptProvider_S_SetupInterrupts( pXvdDipCh );
   if ( BERR_SUCCESS != rc )
   {
      BXVD_DisplayInterruptProvider_P_CloseChannel( pXvdDipCh );
      return BERR_TRACE(rc);
   }

   *phXvdDipCh = pXvdDipCh;

   BDBG_MODULE_MSG( BXVD_DIPCTL, ("Create: hDip:%08x Decoder:%d hXvd:%08x eDisplayInterrupt:%d interruptId:%08x", 
                  pXvdDipCh,
                  pXvdDipCh->stChannelSettings.hXvd->uDecoderInstance,
                  pXvdDipCh->stChannelSettings.hXvd,
                  pXvdDipCh->stChannelSettings.eDisplayInterrupt,
                  pXvdDipCh->stChannelSettings.interruptId
                  ));

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_P_OpenChannel );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXVD_DisplayInterruptProvider_P_CloseChannel(
         BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh
         )
{
   BDBG_ENTER( BXVD_DisplayInterruptProvider_P_CloseChannel );

   if ( NULL != hXvdDipCh )
   {
      BDBG_MODULE_MSG( BXVD_DIPCTL, ("Destroy: hDip:%08x Decoder:%d", 
                           hXvdDipCh,
                           hXvdDipCh->stChannelSettings.hXvd->uDecoderInstance ));

      BXVD_DisplayInterruptProvider_S_TeardownInterrupts( hXvdDipCh );

      BKNI_Free( hXvdDipCh->stDisplayInterruptInfo.astSTC );
      BKNI_Free( hXvdDipCh );
   }

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_P_CloseChannel );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXVD_DisplayInterruptProvider_P_ProcessWatchdog(
         BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh
         )
{
   BERR_Code rc;
   BDBG_ENTER( BXVD_DisplayInterruptProvider_P_ProcessWatchdog );

   BDBG_ASSERT( hXvdDipCh );

   rc = BXVD_DisplayInterruptProvider_S_SetupInterrupts( hXvdDipCh );
   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   if ( true == hXvdDipCh->bInterruptSettingsValid )
   {
      /* clear pending BVNF interrupts since they are edge-triggered */
      BREG_Write32(hXvdDipCh->stChannelSettings.hRegister,
                   hXvdDipCh->stChannelSettings.uiInterruptClearRegister,
                   hXvdDipCh->uiRegMaskCurrent);

      /* Restore mask register */
      BREG_Write32(hXvdDipCh->stChannelSettings.hRegister,
                   hXvdDipCh->stChannelSettings.uiInterruptMaskRegister,
                   hXvdDipCh->uiRegMaskCurrent);

      rc = BXVD_P_HostCmdSendConfig(hXvdDipCh->stChannelSettings.hXvd,
                                    hXvdDipCh->stChannelSettings.eDisplayInterrupt,
                                    hXvdDipCh->uiRegMaskCurrent);
      if ( BERR_SUCCESS != rc )
      {
         return BERR_TRACE( rc );
      }
   }

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_P_ProcessWatchdog );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXVD_DisplayInterruptProvider_P_GetDefaultInterruptSettings(
         BXVD_DisplayInterruptProvider_P_InterruptSettings *pstXvdDipIntSettings
         )
{
   BDBG_ENTER( BXVD_DisplayInterruptProvider_P_GetDefaultInterruptSettings );

   BDBG_ASSERT( pstXvdDipIntSettings );

   BKNI_Memset( pstXvdDipIntSettings, 0, sizeof( BXVD_DisplayInterruptProvider_P_InterruptSettings ) );

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_P_GetDefaultInterruptSettings );

   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code
BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration(
         BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh,
         const BXVD_DisplayInterruptProvider_P_InterruptSettings *pstXvdDipIntSettings
         )
{
   BERR_Code rc;

   uint32_t uiRegMaskNew;

   BDBG_ENTER( BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration );

   BDBG_ASSERT( hXvdDipCh );
   BDBG_ASSERT( pstXvdDipIntSettings );

   uiRegMaskNew = ( pstXvdDipIntSettings->stRULIDMasks.ui32ProgressiveFieldRULIDMask |
                    pstXvdDipIntSettings->stRULIDMasks.ui32TopFieldRULIDMask |
                    pstXvdDipIntSettings->stRULIDMasks.ui32BottomFieldRULIDMask );

   if ( ( false == hXvdDipCh->bInterruptSettingsValid )
        || ( hXvdDipCh->uiRegMaskCurrent != uiRegMaskNew ))
   {
      hXvdDipCh->bInterruptSettingsValid = true;
      hXvdDipCh->stInterruptSettings = *pstXvdDipIntSettings;
      hXvdDipCh->uiRegMaskCurrent = uiRegMaskNew;

      /* clear pending BVNF interrupts since they are edge-triggered */
      BREG_Write32(hXvdDipCh->stChannelSettings.hRegister,
                   hXvdDipCh->stChannelSettings.uiInterruptClearRegister,
                   hXvdDipCh->uiRegMaskCurrent);

      BDBG_MODULE_MSG( BXVD_DIPCTL, ("hDip:%08x SetInterrupt Decoder:%d hXvd:%08x eDisplayInterrupt:%d mask:%08x", 
                  hXvdDipCh,
                  hXvdDipCh->stChannelSettings.hXvd->uDecoderInstance,
                  hXvdDipCh->stChannelSettings.hXvd,
                  hXvdDipCh->stChannelSettings.eDisplayInterrupt,
                  hXvdDipCh->uiRegMaskCurrent
                  ));

      BREG_Write32(hXvdDipCh->stChannelSettings.hRegister,
                   hXvdDipCh->stChannelSettings.uiInterruptMaskRegister,
                   hXvdDipCh->uiRegMaskCurrent);

      rc = BXVD_P_HostCmdSendConfig(hXvdDipCh->stChannelSettings.hXvd,
                                    hXvdDipCh->stChannelSettings.eDisplayInterrupt,
                                    hXvdDipCh->uiRegMaskCurrent);
      if ( BERR_SUCCESS != rc )
      {
         return BERR_TRACE( rc );
      }
   }

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXVD_DisplayInterruptProvider_P_GetInterruptConfiguration(
         BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh,
         BXVD_DisplayInterruptProvider_P_InterruptSettings *pstXvdDipIntSettings
         )
{
   BDBG_ENTER( BXVD_DisplayInterruptProvider_P_GetInterruptConfiguration );

   BDBG_ASSERT( hXvdDipCh );
   BDBG_ASSERT( pstXvdDipIntSettings );

   *pstXvdDipIntSettings = hXvdDipCh->stInterruptSettings;

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_P_GetInterruptConfiguration );
   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code
BXVD_DisplayInterruptProvider_InstallCallback_DisplayInterrupt(
         BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh,
         BXDM_DisplayInterruptHandler_isr fCallback,
         void *pPrivateContext
         )
{
   BDBG_ENTER( BXVD_DisplayInterruptProvider_InstallCallback_DisplayInterrupt );

   BDBG_ASSERT( hXvdDipCh );

   hXvdDipCh->stDisplayInterruptHandler.fCallback = fCallback;
   hXvdDipCh->stDisplayInterruptHandler.pPrivateContext = pPrivateContext;

   BDBG_MODULE_MSG( BXVD_DIPCTL, ("hDip:%08x Install DIH Decoder:%d hXvd:%08x eDisplayInterrupt:%d hDih:%08x cb:%08x", 
                  hXvdDipCh,
                  hXvdDipCh->stChannelSettings.hXvd->uDecoderInstance,
                  hXvdDipCh->stChannelSettings.hXvd,
                  hXvdDipCh->stChannelSettings.eDisplayInterrupt,
                  hXvdDipCh->stDisplayInterruptHandler.pPrivateContext,
                  hXvdDipCh->stDisplayInterruptHandler.fCallback
                  ));

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_InstallCallback_DisplayInterrupt );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXVD_DisplayInterruptProvider_GetDisplayInterruptInfo_isr(
         BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh,
         BXDM_DisplayInterruptInfo *pstXvdDisplayInterruptInfo
         )
{
   BDBG_ENTER( BXVD_DisplayInterruptProvider_GetDisplayInterruptInfo_isr );

   BDBG_ASSERT( hXvdDipCh );
   BDBG_ASSERT( pstXvdDisplayInterruptInfo );

   *pstXvdDisplayInterruptInfo = hXvdDipCh->stDisplayInterruptInfo;

   BDBG_LEAVE( BXVD_DisplayInterruptProvider_GetDisplayInterruptInfo_isr );

   return BERR_TRACE( BERR_SUCCESS );
}
