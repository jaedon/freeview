/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: vce_encoder.c $
 * $brcm_Revision: Hydra_Software_Devel/85 $
 * $brcm_Date: 7/18/12 4:53p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vce/vce_encoder.c $
 * 
 * Hydra_Software_Devel/85   7/18/12 4:53p nilesh
 * SW7425-3156: Updated BVCE_Channel_ClearStatus behavior to be more
 * intuitive
 * 
 * Hydra_Software_Devel/84   7/3/12 1:17p nilesh
 * SW7435-200: Increase delay before destroying consumer thread to account
 * for slow NFS writes
 * 
 * Hydra_Software_Devel/83   6/5/12 12:02p nilesh
 * SW7425-3167: Removed bTrackInput and
 * bAllowGOPRestartOnSceneChangeDetect from BVCE_GOPStructure
 * 
 * Hydra_Software_Devel/82   5/29/12 12:41p nilesh
 * SW7425-3103: Added 15 and 20 fps support
 * 
 * Hydra_Software_Devel/81   5/1/12 1:03p nilesh
 * SW7425-1221: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1221/1   4/23/12 4:51p nilesh
 * SW7425-2244: Added 42-bit STC support
 * 
 * Hydra_Software_Devel/80   4/10/12 1:36p nilesh
 * SW7425-2568: Added key/value config support
 * 
 * Hydra_Software_Devel/79   3/12/12 11:26a nilesh
 * SW7425-2568: Added option to not dump ES file with "fe" command
 * 
 * Hydra_Software_Devel/78   3/8/12 11:17a nilesh
 * SW7425-2078: Enable dynamic GOP
 * 
 * Hydra_Software_Devel/77   3/7/12 4:09p nilesh
 * SW7435-1: VCE Refactoring
 * 
 * Hydra_Software_Devel/SW7435-1/2   3/7/12 3:55p nilesh
 * SW7435-1: Fixed open channel with non-zero instance
 * 
 * Hydra_Software_Devel/SW7435-1/1   3/7/12 2:43p nilesh
 * SW7435-1: Refactored to support quad transcode
 * 
 * Hydra_Software_Devel/76   3/6/12 1:42p nilesh
 * SW7425-1: Fixed memc0 heap usage
 * 
 * Hydra_Software_Devel/75   3/2/12 9:13a nilesh
 * SW7435-1: Fixed 7425 compilation
 * 
 * Hydra_Software_Devel/74   3/1/12 4:34p nilesh
 * SW7435-1: Added VICE_1 support
 * 
 * Hydra_Software_Devel/73   2/9/12 2:50p nilesh
 * SW7425-2220: Added fast channel change support
 * 
 * Hydra_Software_Devel/72   1/31/12 4:31p nilesh
 * SW7435-2: Added 7435 support
 * 
 * Hydra_Software_Devel/71   1/31/12 10:30a nilesh
 * SW7435-2: Fixed 7435 compilation
 * 
 * Hydra_Software_Devel/70   1/25/12 4:50p nilesh
 * SW7425-2220,SW7425-997: Change BVCE_Channel_StopEncode() prototype to
 * support different stop modes
 * 
 * Hydra_Software_Devel/69   12/19/11 2:51p nilesh
 * SW7425-1995: Updated to match new VCE PI
 * 
 * Hydra_Software_Devel/68   12/15/11 6:40a ajagmag
 * SW7425-1877: Updated XUDlib api as per review comments.
 * 
 * Hydra_Software_Devel/67   12/13/11 4:03p ajagmag
 * SW7425-1877: Removed stgRate from XUDlib settings.
 * 
 * Hydra_Software_Devel/66   12/12/11 4:23p nilesh
 * SW7425-1882: Fixed arc log thread shutdown race condition
 * 
 * Hydra_Software_Devel/65   12/9/11 10:40a nilesh
 * SW7425-1566: Moved MultiChannelMode to
 * BVCE_Channel_OpenSettings.eMultiChannelMode
 * 
 * Hydra_Software_Devel/64   12/7/11 2:21p nilesh
 * SW7425-1877: Added XUDlib support
 * 
 * Hydra_Software_Devel/63   12/6/11 4:27p nilesh
 * SW7425-1746: Added "ie" command to force encoder to insert an I frame
 * 
 * Hydra_Software_Devel/62   12/5/11 5:04p nilesh
 * SW7425-960: Removed debug printf
 * 
 * Hydra_Software_Devel/61   12/5/11 4:47p nilesh
 * SW7425-960: Added support for user data insertion via STG Display
 * Callback
 * 
 * Hydra_Software_Devel/60   12/5/11 2:15p nilesh
 * SW7425-960: Added VCE user data support
 * 
 * Hydra_Software_Devel/59   11/16/11 4:25p nilesh
 * SW7425-1566: Added ability to set multi channel mode in "oe" command
 * 
 * Hydra_Software_Devel/58   10/25/11 4:12p nilesh
 * SW7425-1580: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1580/1   10/24/11 4:48p nilesh
 * SW7425-1580: GetStatus - removed I picture count, added dropped counts
 * 
 * Hydra_Software_Devel/57   10/14/11 12:16p nilesh
 * SW7425-1213: Added ability to specify ARC debug log polling frequency
 * 
 * Hydra_Software_Devel/56   9/28/11 4:25p nilesh
 * SW7425-890: Added correct extension to ES files indicating protocol
 * 
 * Hydra_Software_Devel/55   9/26/11 3:21p nilesh
 * SW7425-1213: Added BVCE_Debug_SendCommand support
 * 
 * Hydra_Software_Devel/54   9/14/11 2:49p nilesh
 * SW7425-890: Fix dual encode logic
 * 
 * Hydra_Software_Devel/53   9/10/11 12:26a nilesh
 * SW7425-891: Added FORCE_MEMC0 Compile Time Hack
 * 
 * Hydra_Software_Devel/52   9/8/11 1:58p nilesh
 * SW7425-891: Add dual encode support
 * 
 * Hydra_Software_Devel/SW7425-891/2   9/8/11 1:14p nilesh
 * SW7425-891: Set instance when opening channel/output
 * 
 * Hydra_Software_Devel/SW7425-891/1   9/8/11 12:43p nilesh
 * SW7425-891: Added dual channel support
 * 
 * Hydra_Software_Devel/51   9/6/11 9:57a hongtaoz
 * SW7425-1246: fixed emulation compile error due to bavc API changes;
 * 
 * Hydra_Software_Devel/50   8/16/11 1:58p nilesh
 * SW7425-448: Added VCE Standby/Resume Commands
 * 
 * Hydra_Software_Devel/49   8/15/11 12:04p nilesh
 * SW7425-1006: Fixed low delay and nrt from defaulting to realtime and
 * normal delay when command set >= 104
 * 
 * Hydra_Software_Devel/48   8/15/11 11:38a nilesh
 * SW7425-1006: Moved NRT and low delay mode to BEFORE A2PDelay calc
 * (Command Set 104)
 * 
 * Hydra_Software_Devel/47   8/11/11 3:52p nilesh
 * SW7425-1006: Fixed incorrect ITB hMem used in open channel
 * 
 * Hydra_Software_Devel/46   8/9/11 5:01p nilesh
 * SW7425-1048: Moved Low Delay setting to command set "103"
 * 
 * Hydra_Software_Devel/45   8/3/11 1:34p vanessah
 * SW7425-1048: add low delay mode support for vdc_test
 * 
 * Hydra_Software_Devel/44   6/13/11 10:34a vanessah
 * SW7425-76: add NRT support in VDC_test
 * 
 * Hydra_Software_Devel/43   6/1/11 11:16a nilesh
 * SW7425-629: Updated supported VC1 profiles against VCE PI
 * 
 * Hydra_Software_Devel/42   5/26/11 5:19p nilesh
 * SW7425-484: Added non-realtime support
 * 
 * Hydra_Software_Devel/41   5/23/11 5:15p nilesh
 * SW7425-629: Update supported levels against VCE PI
 * 
 * Hydra_Software_Devel/40   5/11/11 11:58a nilesh
 * SW7425-447: Use correct VC-1 Protocol
 * 
 * Hydra_Software_Devel/39   4/28/11 2:04p vanessah
 * SW7425-32: remove vdc_test compile warning
 * 
 * Hydra_Software_Devel/38   4/28/11 4:32p nilesh
 * SW7425-429: Updated Documentation
 * 
 * Hydra_Software_Devel/37   4/28/11 4:28p nilesh
 * SW7425-429: Added additional encode parameters
 * 
 * Hydra_Software_Devel/36   4/15/11 11:06p nilesh
 * SW7425-173,SW7425-172: Added callback support
 * 
 * Hydra_Software_Devel/35   4/13/11 4:30p nilesh
 * SW7425-303: Added metadata support
 * 
 * Hydra_Software_Devel/34   4/11/11 4:39p nilesh
 * SW7425-171: Added support for events in BVCE_Channel_GetStatus
 * 
 * Hydra_Software_Devel/33   4/4/11 5:17p nilesh
 * SW7425-134: Changed dynamic encode setting parameters to match initial
 * encode settings
 * 
 * Hydra_Software_Devel/32   4/4/11 4:55p nilesh
 * SW7425-134: Added support for changing bitrate and framerate on-the-fly
 * during encode
 * 
 * Hydra_Software_Devel/31   3/2/11 6:32p nilesh
 * SW7425-141: Changed "EndToEndDelay" to "A2PDelay"
 * 
 * Hydra_Software_Devel/30   3/2/11 4:54p nilesh
 * SW7425-137: Removed uiMin and uiTarget bitrate settings
 * 
 * Hydra_Software_Devel/29   2/22/11 5:44p nilesh
 * SW7425-102: Cleaned up end-to-end delay API
 * 
 * Hydra_Software_Devel/28   2/22/11 3:28p nilesh
 * SW7425-1: Add end-to-end delay setting
 * 
 * Hydra_Software_Devel/27   2/10/11 4:08p hongtaoz
 * SW7425-1: added VCE channel open/close test;
 * 
 * Hydra_Software_Devel/26   2/8/11 12:36p nilesh
 * SW7425-1: VCE PI Changes
 * 
 * Hydra_Software_Devel/25   2/1/11 4:19p nilesh
 * SW7425-1: Combined common portions of audio/video compressed buffer
 * descriptors
 * 
 * Hydra_Software_Devel/24   1/26/11 5:39p nilesh
 * SW7425-1: Updated to use new VCE PI
 * 
 * Hydra_Software_Devel/22   1/24/11 1:31p hongtaoz
 * SW7425-1: added file logging of video descriptors;
 * 
 * Hydra_Software_Devel/21   1/18/11 3:21p nilesh
 * SW7425-1: Added EOS support
 * 
 * Hydra_Software_Devel/20   1/14/11 11:05a nilesh
 * SW7425-1: Added support for writing ES > 2GB. Added MB Arc debig log
 * support.  Increased max bitrate to 25Mbps.
 * 
 * Hydra_Software_Devel/19   1/2/11 7:11p hongtaoz
 * SW7425-1: add level setting for h.264 coding;
 * 
 * Hydra_Software_Devel/18   12/23/10 4:15p nilesh
 * SW7425-38: Fixed ITB/CDB wrap
 * 
 * Hydra_Software_Devel/17   12/20/10 3:51p nilesh
 * SW7425-1: Added VCE ITB/CDB buffer dump command "ice"
 * 
 * Hydra_Software_Devel/16   12/13/10 10:08p vanessah
 * SW7425-32:  Add VCE encoding parameter choices
 * 
 * Hydra_Software_Devel/15   12/13/10 2:03p hongtaoz
 * SW7425-32: add emu fwrite call for emulation device memory file
 * capture; extended VCE output capture period for emulation;
 * 
 * Hydra_Software_Devel/14   12/10/10 2:03p hongtaoz
 * SW7425-32: use util to access between file and device memory for emu;
 * tried to separate muxer's xpt channels vs playback decoder's xpt
 * channels;
 * 
 * Hydra_Software_Devel/13   12/1/10 1:59p hongtaoz
 * SW7425-1: don't use non-realtime mode if the system is not set up as
 * non-realtime mode;
 * 
 * Hydra_Software_Devel/12   11/8/10 1:29p nilesh
 * SW7425-1: Use VCE PI to set cabac endianess.
 * 
 * Hydra_Software_Devel/11   11/4/10 2:49p nilesh
 * SW7425-1: Disable debug logs by default.  (Enable via vdc_test "dbg"
 * command).  Increased time between dump of decoder log from 100ms to
 * 5000 ms.
 * 
 * Hydra_Software_Devel/10   11/3/10 2:39p nilesh
 * SW7425-38: Moved encoder context struct to separate private header file
 * 
 * Hydra_Software_Devel/9   11/2/10 12:24p nilesh
 * SW7425-1: Added encoder debug log command "le"
 * 
 * Hydra_Software_Devel/8   10/30/10 1:34a nilesh
 * SW7425-1: Flip CABAC endian-ness to generate PC compatible ES file
 * 
 * Hydra_Software_Devel/7   10/30/10 12:43a nilesh
 * SW7425-1: Switched to AFAP mode. Fixed writing of ES data.
 * 
 * Hydra_Software_Devel/6   10/29/10 11:15p nilesh
 * SW7425-1: Removed extra debug messages
 * 
 * Hydra_Software_Devel/5   10/26/10 6:26p hongtaoz
 * SW7425-1: used BDBG instead of BKNI_Printf in task loop;
 * 
 * Hydra_Software_Devel/4   10/26/10 1:47p nilesh
 * SW7425-1: Added CDB/ITB debug prints
 * 
 * Hydra_Software_Devel/3   10/25/10 11:40p hongtaoz
 * SW7425-1: forked a separate task to capture the ViCE2 encoder buffer;
 * 
 * Hydra_Software_Devel/2   10/21/10 4:59p nilesh
 * SW7425-1: Implemented GetStatus. Added GetEncodedFrames.
 * 
 * Hydra_Software_Devel/1   10/21/10 11:19a nilesh
 * SW7425-1: Added VCE Encoder Support (enable by compiling with
 * SUPPORT_VCE=y)
 *
 ***************************************************************************/

/* test include */
#include "config.h"
#include "test_util.h"
#include "interactive.h"
#include "commands.h"
#include "string.h"
#include "param.h"

#include "bstd.h"
#include "bvce.h"
#include "bavc.h"

#ifndef BTST_VDC_ONLY
#include "bxvd_userdata.h"
#endif

#include "vce_encoder_priv.h"
#if defined(EMULATION) /* prepare emulation client memory paging to avoid fwrite failure */
#include "pagelib.h"
#endif
BDBG_MODULE(VCE_ENCODER);

void BTST_Encoder_WatchdogCallbackHandler_isr(
          void *pPrivateContext,
          int32_t iPrivateParam,
          const BVCE_WatchdogCallbackInfo *pInfo )
{
   BSTD_UNUSED(pPrivateContext);
   BSTD_UNUSED(iPrivateParam);
   BSTD_UNUSED(pInfo);

   BDBG_ERR(("Encoder Watchdog Occurred"));
}

void BTST_Encoder_Channel_EventCallbackHandler_isr(
          void *pPrivateContext,
          int32_t iPrivateParam,
          const BVCE_Channel_EventCallbackInfo *pInfo )
{
   BSTD_UNUSED(pPrivateContext);
   BSTD_UNUSED(iPrivateParam);
   BSTD_UNUSED(pInfo);

   BDBG_MSG(("Encoder Channel Event Occurred\n"));
}

#if (BCHP_CHIP == 7435)
#define NUM_INSTANCES 2
#define GEN_HEAP_MEM 0
#define FW_HEAP_MEM 0
#define PIC_HEAP_MEM 0
#define SECURE_HEAP_MEM 0

#define GEN_HEAP_MEM_1 1
#define FW_HEAP_MEM_1 1
#define PIC_HEAP_MEM_1 1
#define SECURE_HEAP_MEM_1 0
#else
#define NUM_INSTANCES 1
#define GEN_HEAP_MEM 1
#define FW_HEAP_MEM 1
#define PIC_HEAP_MEM 1
#define SECURE_HEAP_MEM 0
#endif

void
BTST_OpenEncoder(BTST_P_Context *pContext)
{
   BERR_Code rc;
   /* Allocate Context */
   BTST_VCEEncoder_Info *pstInfo = pContext->pEncoderInfo;

#if 0
   BDBG_SetModuleLevel("BVCE", BDBG_eMsg);
   BDBG_SetModuleLevel("BVCE_IMAGE", BDBG_eTrace);
   BDBG_SetModuleLevel("BAFL", BDBG_eTrace);
   BDBG_SetModuleLevel("VCE_ENCODER", BDBG_eMsg);
#endif

   if ( NULL == pstInfo )
   {
      BTST_InitializeEncoder( pContext );
      pstInfo = pContext->pEncoderInfo;
      pstInfo->state.bCompatibilityMode = true;
   }

   /* Open VCE Handle */
   if ( true == pstInfo->state.bCompatibilityMode )
   {
      printf("Device number (0-%d): ", NUM_INSTANCES-1);
      BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiInstance);
      if ( pstInfo->state.uiInstance >= NUM_INSTANCES )
      {
         printf("Invalid Instance (%d)...Defaulting to 0\n", pstInfo->state.uiInstance);
         pstInfo->state.uiInstance = 0;
      }
   }

   if ( NULL != pstInfo->instance[pstInfo->state.uiInstance].hVce )
   {
      BDBG_ERR(("Error VCE instance %d already open", pstInfo->state.uiInstance));
      BERR_TRACE( BERR_INVALID_PARAMETER);
      return;
   }

   {
      BVCE_OpenSettings *pstOpenSettings = &pstInfo->instance[pstInfo->state.uiInstance].stOpenSettings;

      BVCE_GetDefaultOpenSettings(pstOpenSettings);
      pstOpenSettings->uiInstance = pstInfo->state.uiInstance;

      switch ( pstOpenSettings->uiInstance )
      {
         case 0:
            pstInfo->instance[pstInfo->state.uiInstance].hMem = pContext->astMViCEInfo[GEN_HEAP_MEM].hMem;

            /* 7425 RTS requirs VCE ARC firmware to be loaded on memc1 */
            pstOpenSettings->hFirmwareMem[0] = pContext->astMViCEInfo[FW_HEAP_MEM].hMem;
            pstOpenSettings->hFirmwareMem[1] = pContext->astMViCEInfo[FW_HEAP_MEM].hMem;

            /* 7425 RTS requires cabac/secure memory to be loaded on memc0 */
            pstOpenSettings->hSecureMem = pContext->astMViCEInfo[SECURE_HEAP_MEM].hMem;

            /* 7425 RTS requires picture memory to be loaded on memc1 */
            pstOpenSettings->hPictureMem = pContext->astMViCEInfo[PIC_HEAP_MEM].hMem;
            break;
#if NUM_INSTANCES > 1
         case 1:
            pstInfo->instance[pstInfo->state.uiInstance].hMem = pContext->astMViCEInfo[GEN_HEAP_MEM_1].hMem;

            /* 7425 RTS requirs VCE ARC firmware to be loaded on memc1 */
            pstOpenSettings->hFirmwareMem[0] = pContext->astMViCEInfo[FW_HEAP_MEM_1].hMem;
            pstOpenSettings->hFirmwareMem[1] = pContext->astMViCEInfo[FW_HEAP_MEM_1].hMem;

            /* 7425 RTS requires cabac/secure memory to be loaded on memc0 */
            pstOpenSettings->hSecureMem = pContext->astMViCEInfo[SECURE_HEAP_MEM_1].hMem;

            /* 7425 RTS requires picture memory to be loaded on memc1 */
            pstOpenSettings->hPictureMem = pContext->astMViCEInfo[PIC_HEAP_MEM_1].hMem;
            break;
#endif

         default:
            break;
      }

      rc = BVCE_Open(
               &pstInfo->instance[pstInfo->state.uiInstance].hVce,
               pContext->hChp,
               pContext->hReg,
               pstInfo->instance[pstInfo->state.uiInstance].hMem,
               pContext->hInt,
               pstOpenSettings);

      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error opening VCE Device"));
         BERR_TRACE( rc );
         return;
      }

      pstInfo->state.uiNumInstancesOpen++;
   }

   /* Setup Device Callback(s) */
   {
      BVCE_CallbackSettings stCallbackSettings;

      BVCE_GetDefaultCallbackSettings(&stCallbackSettings);

      stCallbackSettings.stWatchdog.fCallback = BTST_Encoder_WatchdogCallbackHandler_isr;
      stCallbackSettings.stWatchdog.bEnable = true;

      BVCE_SetCallbackSettings(pstInfo->instance[pstInfo->state.uiInstance].hVce, &stCallbackSettings);
   }

   if ( true == pstInfo->state.bCompatibilityMode )
   {
      pstInfo->state.bAutoOpenFirstChannel = true;
      BTST_OpenEncChan(pContext);
      pstInfo->state.bAutoOpenFirstChannel = false;
   }
}

void
BTST_CloseEncoder(BTST_P_Context *pContext)
{
   unsigned uiIndex = 0;
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   if ( NULL != pstInfo )
   {
      for ( uiIndex = 0; uiIndex < VCE_ENCODER_NUM_ENCODE_CHANNELS; uiIndex++ )
      {
         if ( NULL != pstInfo->instance[pstInfo->state.uiInstance].channel[uiIndex].hVceCh )
         {
            BVCE_Channel_Close(pstInfo->instance[pstInfo->state.uiInstance].channel[uiIndex].hVceCh);
            pstInfo->instance[pstInfo->state.uiInstance].channel[uiIndex].hVceCh = NULL;
         }
      }

      for ( uiIndex = 0; uiIndex < VCE_ENCODER_NUM_OUTPUT_CHANNELS; uiIndex++ )
      {
         if ( NULL != pstInfo->instance[pstInfo->state.uiInstance].output[uiIndex].hVceOutput )
         {
            BVCE_Output_Close(pstInfo->instance[pstInfo->state.uiInstance].output[uiIndex].hVceOutput);
            pstInfo->instance[pstInfo->state.uiInstance].output[uiIndex].hVceOutput = NULL;
         }
      }

      for ( uiIndex = 0; uiIndex < VCE_ENCODER_NUM_OUTPUT_CHANNELS; uiIndex++ )
      {
         if ( NULL != pstInfo->instance[pstInfo->state.uiInstance].buffer[uiIndex].hVceOutputBuffers )
         {
            BVCE_Output_FreeBuffers(pstInfo->instance[pstInfo->state.uiInstance].buffer[uiIndex].hVceOutputBuffers);

            pstInfo->instance[pstInfo->state.uiInstance].buffer[uiIndex].hVceOutputBuffers = NULL;
         }
      }

      if ( NULL != pstInfo->instance[pstInfo->state.uiInstance].hVce )
      {
         BVCE_Close(pstInfo->instance[pstInfo->state.uiInstance].hVce);
         pstInfo->instance[pstInfo->state.uiInstance].hVce = NULL;

         pstInfo->state.uiNumInstancesOpen--;
      }

      if ( true == pstInfo->state.bCompatibilityMode )
      {
         if ( 0 == pstInfo->state.uiNumInstancesOpen )
         {
            BKNI_Free(pstInfo);
            pContext->pEncoderInfo = NULL;
         }
      }
   }
}

void
BTST_OpenEncChan(BTST_P_Context *pContext)
{
   BERR_Code rc;
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   if ( true == pstInfo->state.bCompatibilityMode )
   {
      if ( false == pstInfo->state.bAutoOpenFirstChannel )
      {
         printf("Channel number: (0-%d): ", VCE_ENCODER_NUM_ENCODE_CHANNELS-1);
         BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiChannel);
      }
      else
      {
         pstInfo->state.uiChannel = 0;
      }
   }

   if ( NULL != pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh)
   {
      printf("ERROR: VCE Channel [%d][%d] is already open!\n", pstInfo->state.uiInstance, pstInfo->state.uiChannel );
      return;
   }

   /* Open VCE Channel */
   {
      BVCE_Channel_OpenSettings *pstChannelOpenSettings = &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].stOpenSettings;

      BVCE_Channel_GetDefaultOpenSettings(pstChannelOpenSettings);

      pstChannelOpenSettings->uiInstance = pstInfo->state.uiChannel;

      {
         unsigned uiMultiChannelMode = 0;

         printf("Multi Channel Mode (0=Multi,1=Single,2=MultiNRT,99=Extended Config Mode): ");
         BTST_P_Scanf(pContext, "%u", &uiMultiChannelMode);
         if ( 99 == uiMultiChannelMode )
         {
            PARAM_PREFIX(64)

            #define PARAM(_key,_field,_type,_help) \
               else if ( 0 == strcmp(PARAM_KEY,_key) )\
               {\
                  pstChannelOpenSettings->_field = (_type) PARAM_VALUE;\
               }
            #include "vce_channel_open_params.txt"

            PARAM_HELP_PREFIX()

            #define PARAM(_key,_field,_type,_help) \
                  BKNI_Printf("\""_key"\"" _help "\n");
            #include "vce_channel_open_params.txt"

            PARAM_HELP_POSTFIX()
            PARAM_POSTFIX()
         }
         else
         {
            pstChannelOpenSettings->eMultiChannelMode = uiMultiChannelMode;
         }

      }

      rc = BVCE_Channel_Open(
         pstInfo->instance[pstInfo->state.uiInstance].hVce,
         &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
         pstChannelOpenSettings);

      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error opening VCE Channel"));
         BERR_TRACE( rc );
         return;
      }
   }

   /* Setup Channel Callback(s) */
   {
      BVCE_Channel_CallbackSettings stChCallbackSettings;

      BVCE_Channel_GetDefaultCallbackSettings(&stChCallbackSettings);

      stChCallbackSettings.stEvent.fCallback = BTST_Encoder_Channel_EventCallbackHandler_isr;
      stChCallbackSettings.stEvent.bEnable = true;

      BVCE_Channel_SetCallbackSettings(
         pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
         &stChCallbackSettings);
   }

   /* Open Output Channel */
   {
      BVCE_Output_OpenSettings *pstOutputOpenSettings = &pstInfo->instance[pstInfo->state.uiInstance].output[pstInfo->state.uiChannel].stOutputOpenSettings;

      BVCE_Output_GetDefaultOpenSettings(pstOutputOpenSettings);

      pstOutputOpenSettings->uiInstance = pstInfo->state.uiChannel;

      rc = BVCE_Output_Open(
         pstInfo->instance[pstInfo->state.uiInstance].hVce,
         &pstInfo->instance[pstInfo->state.uiInstance].output[pstInfo->state.uiChannel].hVceOutput,
         pstOutputOpenSettings);

      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error opening output context"));
         BERR_TRACE( rc );
         return;
      }
   }

   /* Allocate Output (CDB/ITB) Buffers */
   {
      BVCE_Output_AllocBuffersSettings stOutputAllocBuffersSettings;

      BVCE_Output_GetDefaultAllocBuffersSettings(&stOutputAllocBuffersSettings);

      /* Set the CABAC CDB to little endian mode so we can dump the ES using the MIPs */
      stOutputAllocBuffersSettings.stConfig.Cdb.LittleEndian = true;
      stOutputAllocBuffersSettings.hITBMem = pContext->astMViCEInfo[0].hMem;

      rc = BVCE_Output_AllocBuffers(
         pstInfo->instance[pstInfo->state.uiInstance].hVce,
         &pstInfo->instance[pstInfo->state.uiInstance].buffer[pstInfo->state.uiChannel].hVceOutputBuffers,
         &stOutputAllocBuffersSettings);

      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error allocating output buffers"));
         BERR_TRACE( rc );
         return;
      }
   }

   /* Assign Buffer to Output Context */
   BVCE_Output_SetBuffers(
      pstInfo->instance[pstInfo->state.uiInstance].output[pstInfo->state.uiChannel].hVceOutput,
      pstInfo->instance[pstInfo->state.uiInstance].buffer[pstInfo->state.uiChannel].hVceOutputBuffers
   );
}

void
BTST_CloseEncChan(BTST_P_Context *pContext)
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   if ( true == pstInfo->state.bCompatibilityMode )
   {
      printf("Channel number: (0-%d): ", VCE_ENCODER_NUM_ENCODE_CHANNELS-1);
      BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiChannel);
   }

   if ( NULL != pstInfo )
   {
      BVCE_Output_Handle hVceOutput = pstInfo->instance[pstInfo->state.uiInstance].output[pstInfo->state.uiChannel].hVceOutput;
      pstInfo->instance[pstInfo->state.uiInstance].output[pstInfo->state.uiChannel].hVceOutput = NULL;

      if ( NULL != pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh )
      {
         BVCE_Channel_Close(pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh);
         pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh = NULL;
      }

      if ( NULL != hVceOutput )
      {
         BVCE_Output_Close(hVceOutput);
         hVceOutput = NULL;
      }

      if ( NULL != pstInfo->instance[pstInfo->state.uiInstance].buffer[pstInfo->state.uiChannel].hVceOutputBuffers )
      {
         BVCE_Output_FreeBuffers(pstInfo->instance[pstInfo->state.uiInstance].buffer[pstInfo->state.uiChannel].hVceOutputBuffers);

         pstInfo->instance[pstInfo->state.uiInstance].buffer[pstInfo->state.uiChannel].hVceOutputBuffers = NULL;
      }
   }
}

/* Encode Command Guide:
 *
 * The "enc" is always followed by the <command version>.  The valid parameters are defined by the <command version> specified.
 *
 * Common:
 *  <command version> = see versions below
 *  <frame rate> = (0) current/default (1)23.97 (2) 24 (3) 25 (4) 29.97 (5)30 (6) 50 (7) 59.94 (8) 60 (9) 14.985 (11) 15 (12) 20
 *  <bit rate> = (0=current/default, 10,000 ~ 10,000,000) bps
 *  <protocol> = (0) H264 (1) MPEG2 (2) MPEG4 (3) VC-1
 *   <profile> = H264: (0) High (1) Main (2) Constrained Baseline
 *   <profile> = MPEG2: N/A
 *   <profile> = MPEG4: (0) Simple (1) Advanced Simple
 *   <profile> = VC-1: (0) Main (1) Simple
 *  <level> = integer specific for protocol
 *  <GOP Structure> = (0) I only (1) IPP (2) IBP (3) IBBP (6) tracking input (7) IP infinite (8) IP infinite w scene change detection
 *  <GOP Length> = number of pictures per GOP
 *  <End-to-End Delay> = (in 27 Mhz clock ticks, 0=default=XXX, max=YYY)
 *  <When To Apply> = (0)=Now, (1)=OnBVNChange
 *  <Input Type> = (0)=Progressive, (1)=Interlaced
 *  <RT/NRT Mode> = (0=realtime, 1=non-realtime)
 *  <low delay> = (0=n,1=y)
 *  <channel> = [0,1]
 *
 * Command Versions:
 *  100:
 *    - Base
 *    Usage: enc 100 <frame rate> <bit rate> <protocol> <profile> <level> <GOP Structure> <GOP Length> <End-To-End Delay>
 *
 *  101:
 *    - Added now vs on bvn input change
 *    - Added progressive vs interlaced input type
 *    Usage: enc 101 <frame rate> <bit rate> <protocol> <profile> <level> <GOP Structure> <GOP Length> <When To Apply> <Input Type> <End-To-End Delay>
 *
 *  102:
 *    - Added realtime vs non-realtime encode mode
 *    Usage: enc 102 <frame rate> <bit rate> <protocol> <profile> <level> <GOP Structure> <GOP Length> <When To Apply> <Input Type> <End-To-End Delay> <RT/NRT Mode>
 *
 *  103:
 *    - Added low delay mode
 *    Usage: enc 103 <frame rate> <bit rate> <protocol> <profile> <level> <GOP Structure> <GOP Length> <When To Apply> <Input Type> <End-To-End Delay> <RT/NRT Mode> <Low Delay Mode>
 *
 *  104:
 *    - Moved NRT and Low Delay Setting BEFORE A2PDelay because A2PDelat is dependent on those 2 params\
 *     Usage: enc 104 <frame rate> <bit rate> <protocol> <profile> <level> <GOP Structure> <GOP Length> <When To Apply> <Input Type> <RT/NRT Mode> <Low Delay Mode> <End-To-End Delay>
 *
 *  105:
 *    - Added channel number
 *     Usage: enc 105 <channel> <frame rate> <bit rate> <protocol> <profile> <level> <GOP Structure> <GOP Length> <When To Apply> <Input Type> <RT/NRT Mode> <Low Delay Mode> <End-To-End Delay>
 *
 *  106
 *    - Removed channel number
 *     Usage: enc 106 <frame rate> <bit rate> <protocol> <profile> <level> <GOP Structure> <GOP Length> <When To Apply> <Input Type> <RT/NRT Mode> <Low Delay Mode> <End-To-End Delay>
 *
 *  107
 *    - key/value pair config
 */

void
BTST_StartEncoder(BTST_P_Context *pContext)
{
   BERR_Code rc;
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;
   int iChoice;
   int iNumber, iLevel;
   int iCommandVersion = 0;
   bool bOnBVNInputChange = false;
   BVDC_Display_Handle             hDisplay;
   bool                            bEnable;
   BVDC_Display_StgSettings        stStgSettings;

#define MAXBITRATE            25000000
#define MINBITRATE            10000
#define MAXFRAMERATECODE      10
#define MINFRAMERATECODE      1
#define MAX(a, b)             (((a)>=(b))?(a):(b))
#define MIN(a, b)             (((a)<=(b))?(a):(b))
   /* Configure Parameters */
   {
      BVCE_Channel_StartEncodeSettings *pstChStartEncodeSettings = NULL;
      BVCE_Channel_EncodeSettings *pstChEncodeSettings = NULL;

	   hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	   printf("Command set version: ");
      BTST_P_Scanf(pContext, "%d", &iChoice);

      if ( iChoice < 100 )
      {
         iCommandVersion = 100;
      }
      else
      {
         iCommandVersion = iChoice;
      }

      if ( iChoice == 107 )
      {
         pstChEncodeSettings = &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].stEncodeSettings;
         pstChStartEncodeSettings = &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].stStartEncodeSettings;

         if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
         {
            BVCE_Channel_GetDefaultEncodeSettings(pstChEncodeSettings);
            BVCE_Channel_GetDefaultStartEncodeSettings(pstChStartEncodeSettings);
         }
         else
         {
            BVCE_Channel_GetEncodeSettings(
               pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
               pstChEncodeSettings
            );
         }

         BKNI_Printf("Enter configuration key/value pairs (\"help:0\" for key list, \"end:0\" to end config):");
         PARAM_PREFIX(64)

         #define PARAM(_key,_field,_type,_help) \
            else if ( ( 0 == strcmp(PARAM_KEY,_key) ) && \
                      ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted ) )\
            {\
               pstChStartEncodeSettings->_field = (_type) PARAM_VALUE;\
            }
         #include "vce_channel_start_encode_params.txt"

         #define PARAM(_key,_field,_type,_help) \
            else if ( 0 == strcmp(PARAM_KEY,_key) )\
            {\
               pstChEncodeSettings->_field = (_type) PARAM_VALUE;\
            }
         #include "vce_channel_encode_params.txt"

         PARAM_HELP_PREFIX()

         #define PARAM(_key,_field,_type,_help) \
            if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )\
               BKNI_Printf("\""_key"\"" _help "\n");
         #include "vce_channel_start_encode_params.txt"

         #define PARAM(_key,_field,_type,_help) \
            BKNI_Printf("\""_key"\"" _help "\n");
         #include "vce_channel_encode_params.txt"

         PARAM_HELP_POSTFIX()
         PARAM_POSTFIX()
      }
      else
      {
         if ( iChoice == 105 )
         {
            printf("Channel number: ");
            BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiChannel);
         }

         pstChEncodeSettings = &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].stEncodeSettings;
         pstChStartEncodeSettings = &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].stStartEncodeSettings;

         if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
         {
            BVCE_Channel_GetDefaultEncodeSettings(pstChEncodeSettings);
            BVCE_Channel_GetDefaultStartEncodeSettings(pstChStartEncodeSettings);
         }
         else
         {
            BVCE_Channel_GetEncodeSettings(
               pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
               pstChEncodeSettings
            );
         }

        if (BVDC_DisplayTg_eStg == pContext->eMasterTg[pContext->iSelectedComp])
        {
          BVDC_Display_GetStgConfiguration(hDisplay, &bEnable, &stStgSettings);
          pstChStartEncodeSettings->bNonRealTimeEncodeMode = stStgSettings.bNonRealTime;
        }

         printf("(0) current/default (1)23.97 (2) 24 (3) 25 (4) 29.97 (5)30 (6) 50 (7) 59.94 (8) 60 (9) 14.985 (12) 15 fps (13) 20fps\n");
         printf("Encoding frame rate: ");

         if ( iChoice >= 100 ) /* Backwards compatibility for old encode command */
         {
            BTST_P_Scanf(pContext, "%d", &iChoice);
         }
         iChoice = (iChoice > MAXFRAMERATECODE) ? MAXFRAMERATECODE : iChoice;
         if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
         {
            iChoice = (iChoice < MINFRAMERATECODE) ? MINFRAMERATECODE : iChoice;
         }

         switch ( iChoice )
         {
            case 0:
               printf("Using current frame rate (%d)\n", pstChEncodeSettings->stFrameRate.eFrameRate);
               break;

            case 1:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e23_976;
               break;

            case 2:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e24;
               break;

            case 3:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e25;
               break;

            case 4:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e29_97;
               break;

            case 5:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e30;
               break;

            case 6:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e50;
               break;

            case 7:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e59_94;
               break;

            case 8:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e60;
               break;

            case 9:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e14_985;
               break;

            case 12:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e15;
               break;

            case 13:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e20;
               break;

            default:
               pstChEncodeSettings->stFrameRate.eFrameRate = BAVC_FrameRateCode_e60;
               break;
         }

         pstChEncodeSettings->stFrameRate.bVariableFrameRateMode = false;

         printf("Target bit rate (0=current/default, 10,000 ~ 10,000,000) bps: ");
         BTST_P_Scanf(pContext, "%d", &iNumber);

         iNumber = (iNumber > MAXBITRATE) ? MAXBITRATE : iNumber;
         if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
         {
            iNumber = (iNumber < MINBITRATE) ? MINBITRATE : iNumber;
         }
         if ( 0 != iNumber )
         {
            pstChEncodeSettings->stBitRate.uiMax = iNumber;
         }
         else
         {
            printf("Using current target bit rate (%d)\n", pstChEncodeSettings->stBitRate.uiMax);
         }

         printf("Protocol: (0) H264 (1) MPEG2 (2) MPEG4 (3) VC-1: ");
         BTST_P_Scanf(pContext, "%d", &iChoice);
         switch ( iChoice )
         {
            case (0):
            default:
               pstChStartEncodeSettings->stProtocolInfo.eProtocol = BAVC_VideoCompressionStd_eH264;
               printf("Select H264 profile (0) High (1) Main (2) Constrained Baseline: ");
               BTST_P_Scanf(pContext, "%d", &pstChStartEncodeSettings->stProtocolInfo.eProfile);
               pstChStartEncodeSettings->stProtocolInfo.eProfile
                        = MIN(pstChStartEncodeSettings->stProtocolInfo.eProfile, 2);
               switch ( pstChStartEncodeSettings->stProtocolInfo.eProfile )
               {
                  case 1:
                     pstChStartEncodeSettings->stProtocolInfo.eProfile = BAVC_VideoCompressionProfile_eMain;
                     break;
                  case 2:
                     pstChStartEncodeSettings->stProtocolInfo.eProfile = BAVC_VideoCompressionProfile_eBaseline;
                     break;

                  default:
                     printf("Unrecognized profile. Using default (High)\n");
                  case 0:
                     pstChStartEncodeSettings->stProtocolInfo.eProfile = BAVC_VideoCompressionProfile_eHigh;
                     break;
               }
               break;
            case (1):
               pstChStartEncodeSettings->stProtocolInfo.eProtocol = BAVC_VideoCompressionStd_eMPEG2;
               pstChStartEncodeSettings->stProtocolInfo.eProfile = BAVC_VideoCompressionProfile_eMain;
               break;
            case (2):
               pstChStartEncodeSettings->stProtocolInfo.eProtocol = BAVC_VideoCompressionStd_eMPEG4Part2;
               printf("MPEG4Part2 profile (0) Simple (1) Advanced Simple: ");
               BTST_P_Scanf(pContext, "%d", &iChoice);
               switch ( iChoice )
               {
                  case 1:
                     pstChStartEncodeSettings->stProtocolInfo.eProfile = BAVC_VideoCompressionProfile_eAdvancedSimple;
                     break;

                  default:
                     printf("Unrecognized profile. Using default (Simple)\n");
                  case 0:
                     pstChStartEncodeSettings->stProtocolInfo.eProfile = BAVC_VideoCompressionProfile_eSimple;
               }
               break;
            case (3):
               pstChStartEncodeSettings->stProtocolInfo.eProtocol = BAVC_VideoCompressionStd_eVC1SimpleMain;
               printf("VC1 profile (0) Main: ");
               BTST_P_Scanf(pContext, "%d", &iChoice);
               switch ( iChoice )
               {
                  default:
                     printf("Unrecognized profile. Using default (Main)\n");
                  case 0:
                     pstChStartEncodeSettings->stProtocolInfo.eProfile = BAVC_VideoCompressionProfile_eMain;
                     break;
               }
               break;
         }

         printf("Level (in tenth unit): ");
         BTST_P_Scanf(pContext, "%d", &iLevel);
         switch ( pstChStartEncodeSettings->stProtocolInfo.eProtocol )
         {
            case BAVC_VideoCompressionStd_eH264:
            {
               switch ( iLevel )
               {
                  case 10:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e10;
                     break;
                  case 11:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e11;
                     break;
                  case 12:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e12;
                     break;
                  case 13:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e13;
                     break;
                  case 20:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e20;
                     break;
                  case 21:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e21;
                     break;
                  case 22:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e22;
                     break;
                  case 30:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e30;
                     break;
                  case 31:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e31;
                     break;
                  case 32:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e32;
                     break;
                  case 40:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e40;
                     break;
                  default:
                     printf("Unrecognized level. Using default (3.1)\n");
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e31;
                     break;
               }
            }
               break;
            case BAVC_VideoCompressionStd_eMPEG2:
            {
               switch ( iLevel )
               {
                  case 4:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_eHigh;
                     break;

                  case 8:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_eMain;
                     break;

                  case 10:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_eLow;
                     break;

                  default:
                     printf("Unrecognized level. Using default (Main)\n");
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_eMain;
                     break;
               }
            }
               break;
            case BAVC_VideoCompressionStd_eMPEG4Part2:
            {
               switch ( iLevel )
               {
                  case 1:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e10;
                     break;
                  case 2:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e20;
                     break;
                  case 3:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e30;
                     break;
                  case 4:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e40;
                     break;
                  case 5:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e50;
                     break;
                  default:
                     printf("Unrecognized level. Using default (L1)\n");
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_e10;
                     break;
               }
            }
               break;
            case BAVC_VideoCompressionStd_eVC1SimpleMain:
            {
               switch ( iLevel )
               {
                  case 0:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_eLow;
                     break;
                  case 2:
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_eMain;
                     break;
                  default:
                     printf("Unrecognized level. Using default (Main)\n");
                     pstChStartEncodeSettings->stProtocolInfo.eLevel = BAVC_VideoCompressionLevel_eMain;
                     break;
               }
            }
               break;

            default:
               printf("Unrecognized protocol!\n");
         }
         if ( true == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
         {
            printf("Changing protocol/profile/level dynamically not supported!\n");
         }

         printf("(0) I only (1) IPP (2) IBP (3) IBBP (6) tracking input (7) IP infinite (8) IP infinite w scene change detection\n");
         printf("GOP structure: ");

         BTST_P_Scanf(pContext, "%d", &iChoice);

         if ( iChoice < 6 )
         {
            printf("GoP length: ");
            BTST_P_Scanf(pContext, "%d", &iNumber);
            if ( iNumber < 1 )
               iNumber = 1;
         }

         switch ( iChoice )
         {
            case 0:
               pstChEncodeSettings->stGOPStructure.bAllowOpenGOP = false;
               pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames = 0;
               pstChEncodeSettings->stGOPStructure.uiNumberOfPFrames = 0;
               break;

            case 1:

               pstChEncodeSettings->stGOPStructure.bAllowOpenGOP = false;
               pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames = 0;
               pstChEncodeSettings->stGOPStructure.uiNumberOfPFrames = (iNumber - 1) / (1
                        + pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames);
               break;

            case 2:
               pstChEncodeSettings->stGOPStructure.bAllowOpenGOP = false;
               pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames = 1;
               pstChEncodeSettings->stGOPStructure.uiNumberOfPFrames = (iNumber - 1) / (1
                        + pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames);
               break;

            case 3:
               pstChEncodeSettings->stGOPStructure.bAllowOpenGOP = false;
               pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames = 2;
               pstChEncodeSettings->stGOPStructure.uiNumberOfPFrames = (iNumber - 1) / (1
                        + pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames);
               break;

            case 6:
               pstChEncodeSettings->stGOPStructure.bAllowOpenGOP = false;
               pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames = 0;
               pstChEncodeSettings->stGOPStructure.uiNumberOfPFrames = 0;
               break;

            case 7:
               pstChEncodeSettings->stGOPStructure.bAllowOpenGOP = false;
               pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames = 0;
               pstChEncodeSettings->stGOPStructure.uiNumberOfPFrames = 0xFFFFFFFF;
               break;

            case 8:
               pstChEncodeSettings->stGOPStructure.bAllowOpenGOP = false;
               pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames = 0;
               pstChEncodeSettings->stGOPStructure.uiNumberOfPFrames = 0xFFFFFFFF;
               break;

            default:
               break;
         }

         if ( iCommandVersion >= 101 )
         {
            /* Set BVN Input Change */
            printf("When should settings be applied? (0)=Now, (1)=OnBVNChange: ");
            BTST_P_Scanf(pContext, "%d", &iNumber);

            if ( 1 == iNumber  )
            {
               bOnBVNInputChange = true;
            }

            /* Set Input Type */
            printf("Input Type? (0)=Progressive, (1)=Interlaced: ");
            BTST_P_Scanf(pContext, "%d", &iNumber);

            if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
            {
               if ( iNumber == 1 )
               {
                  pstChStartEncodeSettings->eInputType = BAVC_ScanType_eInterlaced;
               }
               else
               {
                  pstChStartEncodeSettings->eInputType = BAVC_ScanType_eProgressive;
               }
            }
            else
            {
               printf("Changing Input Type dynamically not supported\n");
            }
         }

         if ( iCommandVersion >= 104 )
         {
            printf("Encode Mode (0=realtime, 1=non-realtime): ");
            BTST_P_Scanf(pContext, "%d", &iNumber);

            if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
            {
               pstChStartEncodeSettings->bNonRealTimeEncodeMode = ((0 == iNumber) ? false : true);
            }
            else
            {
               printf("Changing NRT Encode Mode dynamically not supported!\n");
            }
         }
         else
         {
            pstChStartEncodeSettings->bNonRealTimeEncodeMode = false;
         }

         if ( iCommandVersion >= 104 )
         {
            printf("Low Delay Mode (0=n, 1=y): ");
            BTST_P_Scanf(pContext, "%d", &iNumber);

            if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
            {
               if( false == pstChStartEncodeSettings->bNonRealTimeEncodeMode )
               {
                  pstChStartEncodeSettings->bPipelineLowDelayMode = ((0 == iNumber) ? false : true);
               }
               else if ( 1 == iNumber )
               {
                  printf("Low delay mode not allowed when NRT Encode is enabled!\n");
               }
            }
            else
            {
               printf("Changing Low Delay Mode dynamically not supported!\n");
            }
         }
         else
         {
            pstChStartEncodeSettings->bPipelineLowDelayMode = false;
         }

         /* End-To-End Delay */
         {
            BVCE_A2PDelay stA2PDelay;
            BVCE_GetA2PDelayInfo(pstChEncodeSettings, pstChStartEncodeSettings, &stA2PDelay);

            printf(
                     "End-to-end delay (in 27 Mhz clock ticks, 0=default=%d, max=%d): ",
                     stA2PDelay.uiMin,
                     stA2PDelay.uiMax);
            BTST_P_Scanf(pContext, "%d", &iNumber);

            if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
            {
               if ( 0 == iNumber )
               {
                  pstChEncodeSettings->uiA2PDelay = stA2PDelay.uiMin;
               }
               else
               {
                  if ( (uint32_t) iNumber > stA2PDelay.uiMax )
                  {
                     printf("WARNING: Specified end-to-end delay is larger than max allowed!\n");
                  }
                  pstChEncodeSettings->uiA2PDelay = iNumber;
               }
            }
            else
            {
               printf("Changing A2P Delay dynamically not supported, yet!\n");
            }
         }

         /* Adaptive Low Delay Mode */
         {
            unsigned uiAdaptiveLowDelayMode;

            printf("Adaptive Low Delay Mode? (0=off,1=on): ");
            BTST_P_Scanf(pContext, "%u", &uiAdaptiveLowDelayMode);

            if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
            {
               if ( 0 != uiAdaptiveLowDelayMode )
               {
                  pstChStartEncodeSettings->bAdaptiveLowDelayMode = true;
               }
            }
            else
            {
               printf("Changing Adaptive Low Delay Mode dynamically not supported, yet!\n");
            }
         }
      }

      if ( false == bOnBVNInputChange )
      {
         BVCE_Channel_SetEncodeSettings(
            pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
            pstChEncodeSettings
            );

         printf("\nEncodeSetting:\n");
         printf(
                  " Protocol: %d\n",
                  pstChStartEncodeSettings->stProtocolInfo.eProtocol);
         printf(" Profile %d\n", pstChStartEncodeSettings->stProtocolInfo.eProfile);
         printf(" Level %d\n", pstChStartEncodeSettings->stProtocolInfo.eLevel);
         printf(
                  " p_frames=%d, b_frames=%d, open_gop=%d\n",
                  pstChEncodeSettings->stGOPStructure.uiNumberOfPFrames,
                  pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames,
                  pstChEncodeSettings->stGOPStructure.bAllowOpenGOP);
         printf(" Frame Rate %d\n", pstChEncodeSettings->stFrameRate.eFrameRate);
         printf(" Max Bitrate = %d\n", pstChEncodeSettings->stBitRate.uiMax);
         printf(" A2P Delay = %d\n", pstChEncodeSettings->uiA2PDelay);
         printf(" Input Type = %d\n", pstChStartEncodeSettings->eInputType);
         printf(" Adaptive Low Delay = %d\n", pstChStartEncodeSettings->bAdaptiveLowDelayMode );
      }
      else
      {
         BVCE_Channel_EncodeSettings_OnInputChange stChEncodeSettingsOnInputChange;

         BVCE_Channel_GetEncodeSettings_OnInputChange(
            pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
            &stChEncodeSettingsOnInputChange
         );

         stChEncodeSettingsOnInputChange.stBitRate = pstChEncodeSettings->stBitRate;

         BVCE_Channel_SetEncodeSettings_OnInputChange(
            pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
            &stChEncodeSettingsOnInputChange
         );

         printf("\nEncodeSetting_OnInputChange:\n");
         printf(" Max Bitrate = %d\n", stChEncodeSettingsOnInputChange.stBitRate.uiMax);
      }

      if ( ( iCommandVersion >= 102 )
           && ( iCommandVersion <= 103 ) )
      {
         printf("Encode Mode (0=realtime, 1=non-realtime): ");
         BTST_P_Scanf(pContext, "%d", &iNumber);

         if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
         {
            pstChStartEncodeSettings->bNonRealTimeEncodeMode = ((0 == iNumber) ? false : true);
         }
         else
         {
            printf("Changing NRT Encode Mode dynamically not supported!\n");
         }
      }

      if ( iCommandVersion == 103 )
      {
         printf("Low Delay Mode (0=n, 1=y): ");
         BTST_P_Scanf(pContext, "%d", &iNumber);

         if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
         {
            if( false == pstChStartEncodeSettings->bNonRealTimeEncodeMode )
            {
               pstChStartEncodeSettings->bPipelineLowDelayMode = ((0 == iNumber) ? false : true);
            }
            else if ( 1 == iNumber )
            {
               printf("Low delay mode not allowed when NRT Encode is enabled!\n");
            }
         }
         else
         {
            printf("Changing Low Delay Mode dynamically not supported!\n");
         }
      }

      if ( false == pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted )
      {
         /* TODO: Configure Static Decode Settings*/

         pstChStartEncodeSettings->hOutputHandle = pstInfo->instance[pstInfo->state.uiInstance].output[pstInfo->state.uiChannel].hVceOutput;

         rc = BVCE_Channel_StartEncode(
            pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
            pstChStartEncodeSettings);
      }
   }

   pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted = true;
}

void
BTST_StopEncoder(BTST_P_Context *pContext)
{
   unsigned uiStopImmediate = 0;
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   if ( true == pstInfo->state.bCompatibilityMode )
   {
      printf("Channel number: ");
      BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiChannel);
   }

   printf("Stop Immediately?: (0=no,1=yes): ");
   BTST_P_Scanf(pContext, "%u", &uiStopImmediate);

   {
      BVCE_Channel_StopEncodeSettings stStopSettings;
      BVCE_Channel_GetDefaultStopEncodeSettings( &stStopSettings );

      if ( 0 != uiStopImmediate )
      {
         stStopSettings.eStopMode = BVCE_Channel_StopMode_eImmediate;
      }

      BVCE_Channel_StopEncode(
         pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
         &stStopSettings
      );
   }

   pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].bEncodeStarted = false;
}

void
BTST_GetEncoderStatus(BTST_P_Context *pContext)
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;
   BVCE_Channel_Status stChannelStatus;

   if ( true == pstInfo->state.bCompatibilityMode )
   {
      printf("Channel number: ");
      BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiChannel);
   }

   BVCE_Channel_GetStatus(
      pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
      &stChannelStatus);

   BKNI_Printf("error flags = %08x\n", stChannelStatus.uiErrorFlags);
   BKNI_Printf("event flags = %08x\n", stChannelStatus.uiEventFlags);
   BKNI_Printf("total received = %d\n", stChannelStatus.uiTotalPicturesReceived);
   BKNI_Printf("total dropped (FRC) = %d\n", stChannelStatus.uiTotalPicturesDroppedDueToFrameRateConversion);
   BKNI_Printf("total dropped (Errors) = %d\n", stChannelStatus.uiTotalPicturesDroppedDueToErrors);
   BKNI_Printf("total encoded = %d\n", stChannelStatus.uiTotalPicturesEncoded);
   BKNI_Printf("last picture ID = %08x\n", stChannelStatus.uiLastPictureIdEncoded);
   BKNI_Printf("STC Snapshot = %011llx\n", stChannelStatus.uiSTCSnapshot);

   {
      BVCE_Channel_Status stChannelClearStatus;
      BKNI_Memset( &stChannelClearStatus, 0, sizeof( BVCE_Channel_Status ) );
      stChannelClearStatus.uiErrorFlags = stChannelStatus.uiErrorFlags;
      stChannelClearStatus.uiEventFlags = stChannelStatus.uiEventFlags;

      BVCE_Channel_ClearStatus(
         pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh,
         &stChannelStatus);
   }
}

void
BTST_P_CaptureViceBufferHandler(BTST_TaskHandle hTask, void *pContext)
{
   uint32_t uiFramesWritten[VCE_ENCODER_NUM_INSTANCES][VCE_ENCODER_NUM_OUTPUT_CHANNELS] = { { 0, 0 }, { 0, 0 } };
   bool bStarted[VCE_ENCODER_NUM_INSTANCES][VCE_ENCODER_NUM_OUTPUT_CHANNELS] = { { false, false }, { false, false } };
   const BAVC_VideoBufferDescriptor *astVideoDescriptors[2];
   size_t uiNumVideoDescriptors[2];
   char filename[1024];
   size_t uiDescs;
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext;
   unsigned uiInstance;
   unsigned uiOutputNum;

   for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
   {
      for ( uiOutputNum = 0; uiOutputNum < VCE_ENCODER_NUM_OUTPUT_CHANNELS; uiOutputNum++ )
      {
         pstInfo->instance[uiInstance].output[uiOutputNum].uiFileIndex = 0;
         pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput = NULL;
      }
   }

   BSTD_UNUSED(hTask);

   while ( pstInfo->bEnableViceBufferCaptureHandler )
   {
      for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
      {
         for ( uiOutputNum = 0; uiOutputNum < VCE_ENCODER_NUM_OUTPUT_CHANNELS; uiOutputNum++ )
         {
            BVCE_Output_Handle hVceOutput = pstInfo->instance[uiInstance].output[uiOutputNum].hVceOutput;
            if ( NULL != hVceOutput )
            {
               BAVC_VideoBufferStatus stBufferStatus;
               uint32_t i, j;

               BVCE_Output_GetBufferStatus(hVceOutput, &stBufferStatus);

               BVCE_Output_GetBufferDescriptors(
                        hVceOutput,
                        &astVideoDescriptors[0],
                        &uiNumVideoDescriptors[0],
                        &astVideoDescriptors[1],
                        &uiNumVideoDescriptors[1]);

               if ( uiNumVideoDescriptors[0] + uiNumVideoDescriptors[1] )
                  BDBG_MSG(("Got %d VCE Buffers from output[%d][%d]",
                                    uiNumVideoDescriptors[0] + uiNumVideoDescriptors[1],
                                    uiInstance,
                                    uiOutputNum
                           ));

               for ( uiDescs = 0, i = 0; i < 2; i++ )
               {
                  uiDescs += uiNumVideoDescriptors[i];
                  if ( (NULL != astVideoDescriptors[i]) && (0 != uiNumVideoDescriptors[i]) )
                  {
                     for ( j = 0; j < uiNumVideoDescriptors[i]; j++ )
                     {
                        BDBG_MSG(("%02d: desc[%d][%d] flag=0x%x, vflag=0x%x offset=%08x, size=0x%x",
                                          uiOutputNum,
                                          i,
                                          j,
                                          astVideoDescriptors[i][j].stCommon.uiFlags,
                                          astVideoDescriptors[i][j].uiVideoFlags,
                                          astVideoDescriptors[i][j].stCommon.uiOffset,
                                          astVideoDescriptors[i][j].stCommon.uiLength));

                        if ( 0 != (astVideoDescriptors[i][j].stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS) )
                        {
                           BKNI_Printf("####EOS Seen on output[%d][%d]!####\n", uiInstance, uiOutputNum);
                        }
                        else if ( astVideoDescriptors[i][j].stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START )
                        {
                           if ( true == bStarted[uiInstance][uiOutputNum] )
                           {
                              uiFramesWritten[uiInstance][uiOutputNum]++;
                           }
                           else
                           {
                              bStarted[uiInstance][uiOutputNum] = true;
                           }
                        }

                        if ( astVideoDescriptors[i][j].stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA )
                        {
                           switch ( astVideoDescriptors[i][j].uiDataUnitType )
                           {
                              case BAVC_VideoMetadataType_eCommon:
                              {
                                 BAVC_VideoMetadataDescriptor *pMetadataDescriptor = (BAVC_VideoMetadataDescriptor *) (((uint32_t)stBufferStatus.stCommon.pMetadataBufferBaseAddress ) + astVideoDescriptors[i][j].stCommon.uiOffset);

                                 BDBG_MSG(("%02d: Metadata[%d]: mflags=%08x br=(%08x) fr=%u dim=(%4ux%4u)",
                                             uiOutputNum,
                                             astVideoDescriptors[i][j].uiDataUnitType,
                                             pMetadataDescriptor->uiMetadataFlags,
                                             pMetadataDescriptor->stBitrate.uiMax,
                                             pMetadataDescriptor->stFrameRate.eFrameRateCode,
                                             pMetadataDescriptor->stDimension.coded.uiWidth,
                                             pMetadataDescriptor->stDimension.coded.uiHeight
                                             ));
                              }
                                 break;

                              default:
                                 BDBG_MSG(("%02d: Unrecognized metadata descriptor type (%d)", uiOutputNum, astVideoDescriptors[i][j].uiDataUnitType));

                           }
                        }
                        else
                        {
                           /* Linux has 2GB file limit, so make sure we don't go beyond that */
                           if ( NULL != pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput )
                           {
                              if ( pstInfo->instance[uiInstance].output[uiOutputNum].uiESBytesWrittenThisFile + astVideoDescriptors[i][j].stCommon.uiLength >= 0x80000000 )
                              {
                                 fflush(pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput);
                                 fclose(pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput);
                                 pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput = NULL;
                              }
                           }

                           if ( false == pstInfo->bSkipESDump )
                           {
                              if ( NULL == pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput )
                              {
                                 pstInfo->instance[uiInstance].output[uiOutputNum].uiESBytesWrittenThisFile = 0;
                                 sprintf(filename, "encode_output_[%02d][%02d]_%03d.es", uiInstance, uiOutputNum, pstInfo->instance[uiInstance].output[uiOutputNum].uiFileIndex);
                                 switch ( pstInfo->instance[uiInstance].channel[uiOutputNum].stStartEncodeSettings.stProtocolInfo.eProtocol )
                                 {
                                    case BAVC_VideoCompressionStd_eH264:
                                       strcat(filename, ".h264");
                                       break;
                                    case BAVC_VideoCompressionStd_eMPEG2:
                                       strcat(filename, ".mpeg2");
                                       break;
                                    case BAVC_VideoCompressionStd_eVC1SimpleMain:
                                       strcat(filename, ".vc1sm");
                                       break;
                                    case BAVC_VideoCompressionStd_eMPEG4Part2:
                                       strcat(filename, ".mpeg4");
                                       break;

                                    default:
                                       break;
                                 }

                                 pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput = fopen(filename, "wb");
                                 pstInfo->instance[uiInstance].output[uiOutputNum].uiFileIndex++;

                                 if ( NULL == pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput )
                                 {
                                    BDBG_ERR(("Error Opening encoder output file %s", filename));
                                    return;
                                 }
                              }

                              if ( 0 != astVideoDescriptors[i][j].stCommon.uiLength )
                              {
                                 BDBG_MSG((
                                          "Frame %08d (%07d bytes written) PTS = %09llx\n",
                                          uiFramesWritten[uiInstance][uiOutputNum],
                                          astVideoDescriptors[i][j].stCommon.uiLength,
                                          astVideoDescriptors[i][j].stCommon.uiPTS));
               #if defined(EMULATION)/* prepare emulation client memory paging to avoid fwrite failure */
                                 BEMU_Client_Fwrite(
                                          ((uint8_t*) stBufferStatus.stCommon.pFrameBufferBaseAddress) + astVideoDescriptors[i][j].stCommon.uiOffset,
                                          astVideoDescriptors[i][j].stCommon.uiLength,
                                          pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput
                                 );
               #else
                                 fwrite(
                                          ((uint8_t*) stBufferStatus.stCommon.pFrameBufferBaseAddress)
                                                   + astVideoDescriptors[i][j].stCommon.uiOffset,
                                          1,
                                          astVideoDescriptors[i][j].stCommon.uiLength,
                                          pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput);
               #endif
                                 pstInfo->instance[uiInstance].output[uiOutputNum].uiESBytesWrittenThisFile += astVideoDescriptors[i][j].stCommon.uiLength;
                              }
                           }
                        }
                     }
                  }
               }
               BVCE_Output_ConsumeBufferDescriptors(hVceOutput, uiDescs);
               if ( NULL != pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput )
               {
                  fflush(pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput);
               }
            }
         }
      }
   #if defined(EMULATION)
         BKNI_Sleep(10000); /* Wait 100 s before checking for more frames */
   #else
         BKNI_Sleep(100); /* Wait 100 ms before checking for more frames */
   #endif
   }

   for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
   {
      for ( uiOutputNum = 0; uiOutputNum < VCE_ENCODER_NUM_OUTPUT_CHANNELS; uiOutputNum++ )
      {
         if ( NULL != pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput )
         {
            fclose(pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput);
            pstInfo->instance[uiInstance].output[uiOutputNum].hESOutput = NULL;
         }
      }
   }
}

void
BTST_GetEncodedFrames(BTST_P_Context *pContext)
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;
   unsigned uiSkipESDump = 0;

   /* toggle it */
   pstInfo->bEnableViceBufferCaptureHandler ^= true;

   if ( pstInfo->bEnableViceBufferCaptureHandler )
   {
      BKNI_Printf("Skip ES Dump? (0=no,1=yes): ");
      BTST_P_Scanf(pContext, "%u", &uiSkipESDump);

      if ( 0 != uiSkipESDump )
      {
         pstInfo->bSkipESDump = true;
      }
      else
      {
         pstInfo->bSkipESDump = false;
      }

      printf("start to capture video encoder buffers\n");
      BTST_CreateTask(&pstInfo->hTaskCaptureViceBuffer, BTST_P_CaptureViceBufferHandler, (void*) pstInfo);
   }
   else
   {
      printf("stop to capture video encoder buffers\n");
      BKNI_Sleep(5000);
      TEST_CLOSE_HANDLE(BTST_DestroyTask, pstInfo->hTaskCaptureViceBuffer);
   }
   return;
}

void
BTST_P_CaptureViceItbCdbBufferHandler(BTST_TaskHandle hTask, void *pContext)
{
   char filename[1024];
   bool bCabacReady[VCE_ENCODER_NUM_INSTANCES][VCE_ENCODER_NUM_OUTPUT_CHANNELS] = { { false, false }, { false, false } };
   unsigned uiInstance = 0;
   unsigned uiOutputNum = 0;

   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext;

   BSTD_UNUSED(hTask);

   while ( pstInfo->bEnableViceItbCdbBufferCaptureHandler )
   {
      for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
      {
         for ( uiOutputNum = 0; uiOutputNum < VCE_ENCODER_NUM_OUTPUT_CHANNELS; uiOutputNum++ )
         {
            BVCE_Output_Handle hVceOutput = pstInfo->instance[uiInstance].output[uiOutputNum].hVceOutput;
            FILE *hITBOutput = NULL;
            FILE *hCDBOutput = NULL;

            if ( NULL != hVceOutput )
            {
               const BAVC_VideoBufferDescriptor *astVideoDescriptors[2];
               size_t uiNumVideoDescriptors[2];
               BAVC_VideoContextMap stVceOutputRegisters;

               if ( NULL == pstInfo->instance[uiInstance].output[uiOutputNum].hITBOutput )
               {
                  sprintf(filename, "encode_output_[%02d][%02d].itb", uiInstance, uiOutputNum);
                  pstInfo->instance[uiInstance].output[uiOutputNum].hITBOutput = fopen(filename, "wb");
                  if ( NULL == pstInfo->instance[uiInstance].output[uiOutputNum].hITBOutput )
                  {
                     BDBG_ERR(("Error Opening encoder output file %s", filename));
                     return;
                  }
               }

               hITBOutput = pstInfo->instance[uiInstance].output[uiOutputNum].hITBOutput;

               if ( NULL == pstInfo->instance[uiInstance].output[uiOutputNum].hCDBOutput )
               {
                  sprintf(filename, "encode_output_[%02d][%02d].cdb", uiInstance, uiOutputNum);
                  pstInfo->instance[uiInstance].output[uiOutputNum].hCDBOutput = fopen(filename, "wb");

                  if ( NULL == pstInfo->instance[uiInstance].output[uiOutputNum].hCDBOutput )
                  {
                     fclose(pstInfo->instance[uiInstance].output[uiOutputNum].hITBOutput);
                     pstInfo->instance[uiInstance].output[uiOutputNum].hITBOutput = NULL;
                     BDBG_ERR(("Error Opening encoder output file %s", filename));
                     return;
                  }
               }

               hCDBOutput = pstInfo->instance[uiInstance].output[uiOutputNum].hCDBOutput;

               BVCE_Output_GetRegisters(pstInfo->instance[uiInstance].output[uiOutputNum].hVceOutput, &stVceOutputRegisters);

               if ( false == bCabacReady[uiInstance][uiOutputNum] )
               {
                  BVCE_Output_GetBufferDescriptors(
                     pstInfo->instance[uiInstance].output[uiOutputNum].hVceOutput,
                     &astVideoDescriptors[0],
                     &uiNumVideoDescriptors[0],
                     &astVideoDescriptors[1],
                     &uiNumVideoDescriptors[1]
                  );

                  if ( (uiNumVideoDescriptors[0] + uiNumVideoDescriptors[1]) != 0 )
                  {
                     bCabacReady[uiInstance][uiOutputNum] = true;
                     BDBG_MSG(("Cabac[%d][%d] is READY!", uiInstance, uiOutputNum));
                  }
                  else
                  {
                     BDBG_MSG(("Cabac[%d][%d] is NOT Ready...", uiInstance, uiOutputNum));
                  }
               }

               if ( true == bCabacReady[uiInstance][uiOutputNum] )
               {
                  /* Dump ITB Data */
                  uint32_t uiBaseOffset;
                  uint32_t uiReadOffset;
                  uint32_t uiValidOffset;
                  uint32_t uiEndOffset;

                  uiBaseOffset = BREG_Read32(
                           pstInfo->hReg,
                           stVceOutputRegisters.ITB_Base
                  );

                  uiReadOffset = BREG_Read32(
                           pstInfo->hReg,
                           stVceOutputRegisters.ITB_Read
                  );

                  uiValidOffset = BREG_Read32(
                           pstInfo->hReg,
                           stVceOutputRegisters.ITB_Valid
                  );

                  uiEndOffset = BREG_Read32(
                           pstInfo->hReg,
                           stVceOutputRegisters.ITB_End
                  );

                  if ( uiEndOffset != 0 )
                  {
                     uiEndOffset += 1;
                  }

                  if ( uiValidOffset != 0 )
                  {
                     uiValidOffset += 1;
                     uiValidOffset += 64;

                     if ( uiValidOffset >= uiEndOffset )
                     {
                        uiValidOffset = uiBaseOffset + (uiValidOffset - uiEndOffset);
                     }
                  }

                  while ( uiReadOffset != uiValidOffset )
                  {
                     void *pBuffer = NULL;
                     size_t uiLength;

                     BMEM_ConvertOffsetToAddress(pstInfo->instance[uiInstance].hMem, uiReadOffset, &pBuffer);

                     if ( uiReadOffset < uiValidOffset )
                     {
                        uiLength = uiValidOffset - uiReadOffset;
                     }
                     else
                     {
                        uiLength = uiEndOffset - uiReadOffset;
                     }

         #if defined(EMULATION)/* prepare emulation client memory paging to avoid fwrite failure */
                     {
                        uint32_t uiBytesWritten = 0;

                        while ( uiBytesWritten != uiLength )
                        {
                           uiBytesWritten += BEMU_Client_Fwrite(
                                    (void *) ((uint32_t) pBuffer + uiBytesWritten),
                                    uiLength - uiBytesWritten,
                                    pstInfo->hITBOutput
                           );
                        }
                     }
         #else
                     fwrite(pBuffer, 1, uiLength, hITBOutput);
         #endif
                     BDBG_MSG(("Wrote %d bytes of ITB", uiLength));

                     uiReadOffset += uiLength;
                     if ( uiReadOffset == uiEndOffset )
                     {
                        uiReadOffset = uiBaseOffset;
                     }
                  }
                  BREG_Write32(
                           pstInfo->hReg,
                           stVceOutputRegisters.ITB_Read,
                           uiReadOffset
                  );
                  fflush(hITBOutput);

                  /* Dump CDB Data */
                  uiBaseOffset = BREG_Read32(
                           pstInfo->hReg,
                           stVceOutputRegisters.CDB_Base
                  );

                  uiReadOffset = BREG_Read32(
                           pstInfo->hReg,
                           stVceOutputRegisters.CDB_Read
                  );

                  uiValidOffset = BREG_Read32(
                           pstInfo->hReg,
                           stVceOutputRegisters.CDB_Valid
                  );

                  uiEndOffset = BREG_Read32(
                           pstInfo->hReg,
                           stVceOutputRegisters.CDB_End
                  );

                  if ( uiEndOffset != 0 )
                  {
                     uiEndOffset += 1;
                  }

                  if ( uiValidOffset != 0 )
                  {
                     uiValidOffset += 1;

                     if ( uiValidOffset >= uiEndOffset )
                     {
                        uiValidOffset = uiBaseOffset + (uiValidOffset - uiEndOffset);
                     }
                  }

                  while ( uiReadOffset != uiValidOffset )
                  {
                     void *pBuffer = NULL;
                     size_t uiLength;

                     BMEM_ConvertOffsetToAddress(pstInfo->instance[uiInstance].hMem, uiReadOffset, &pBuffer);

                     if ( uiReadOffset < uiValidOffset )
                     {
                        uiLength = uiValidOffset - uiReadOffset;
                     }
                     else
                     {
                        uiLength = uiEndOffset - uiReadOffset;
                     }

         #if defined(EMULATION)/* prepare emulation client memory paging to avoid fwrite failure */
                     {
                        uint32_t uiBytesWritten = 0;

                        while ( uiBytesWritten != uiLength )
                        {
                           uiBytesWritten += BEMU_Client_Fwrite(
                                    (void *) ((uint32_t) pBuffer + uiBytesWritten),
                                    uiLength - uiBytesWritten,
                                    hCDBOutput
                           );
                        }
                     }
         #else
                     fwrite(pBuffer, 1, uiLength, hCDBOutput);
         #endif
                     BDBG_MSG(("Wrote %d bytes of CDB", uiLength));

                     uiReadOffset += uiLength;
                     if ( uiReadOffset == uiEndOffset )
                     {
                        uiReadOffset = uiBaseOffset;
                     }
                  }
                  BREG_Write32(
                           pstInfo->hReg,
                           stVceOutputRegisters.CDB_Read,
                           uiReadOffset
                  );
                  fflush(hCDBOutput);

         #if defined(EMULATION)
                  BKNI_Sleep(10000); /* Wait 100 s before checking for more frames */
         #else
                  BKNI_Sleep(100); /* Wait 100 ms before checking for more frames */
         #endif
               }
            }
         }
      }
   }

   for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
   {
      for ( uiOutputNum = 0; uiOutputNum < VCE_ENCODER_NUM_OUTPUT_CHANNELS; uiOutputNum++ )
      {
         if ( NULL != pstInfo->instance[uiInstance].output[uiOutputNum].hITBOutput )
         {
            fclose(pstInfo->instance[uiInstance].output[uiOutputNum].hITBOutput);
            pstInfo->instance[uiInstance].output[uiOutputNum].hITBOutput = NULL;
         }

         if ( NULL != pstInfo->instance[uiInstance].output[uiOutputNum].hCDBOutput )
         {
            fclose(pstInfo->instance[uiInstance].output[uiOutputNum].hCDBOutput);
            pstInfo->instance[uiInstance].output[uiOutputNum].hCDBOutput = NULL;
         }
      }
   }
}

void
BTST_GetItbBCdbBuffers(BTST_P_Context *pContext)
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   /* toggle it */
   pstInfo->bEnableViceItbCdbBufferCaptureHandler ^= true;

   if ( pstInfo->bEnableViceItbCdbBufferCaptureHandler )
   {
      printf("start to capture video encoder itb/cdb buffers\n");
      BTST_CreateTask(&pstInfo->hTaskCaptureViceItbCdbBuffer, BTST_P_CaptureViceItbCdbBufferHandler, (void*) pstInfo);
   }
   else
   {
      printf("stop to capture video encoder itb/cdb buffers\n");
      BKNI_Sleep(5000);
      TEST_CLOSE_HANDLE(BTST_DestroyTask, pstInfo->hTaskCaptureViceItbCdbBuffer);
   }
   return;
}

void
BTST_P_CaptureViceDebugLogHandler(BTST_TaskHandle hTask, void *pContext)
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext;
   unsigned uiInstance;
   BVCE_ArcInstance eArcInstance;
   char filename[1024];

   for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
   {
      for ( eArcInstance = 0; eArcInstance < BVCE_ArcInstance_eMax; eArcInstance++ )
      {
         sprintf(filename, "arc_[%d][%d]_debug.log", uiInstance, eArcInstance);
         pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].hFileOutput = fopen(filename, "wb");

         if ( NULL == pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].hFileOutput )
         {
            BDBG_ERR(("Error Opening arc[%d][%d] debug log file %s", uiInstance, eArcInstance, filename));
            return;
         }
      }
   }

   BSTD_UNUSED(hTask);
   while ( pstInfo->bEnableViceDebugLogCaptureHandler )
   {
      for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
      {
         for ( eArcInstance = 0; eArcInstance < BVCE_ArcInstance_eMax; eArcInstance++ )
         {
            uint32_t uiBytesRead = 0;

            BVCE_Debug_ReadBuffer(
               pstInfo->instance[uiInstance].hVce,
               eArcInstance,
               pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].pBuffer,
               pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].uiSize,
               &uiBytesRead);

            if ( 0 != uiBytesRead )
            {
               BKNI_Printf("Arc [%d][%d] Debug Log (%09d bytes written)\n", uiInstance, eArcInstance, uiBytesRead);

               fwrite(
                  pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].pBuffer,
                  1,
                  uiBytesRead,
                  pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].hFileOutput);

               fflush(pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].hFileOutput);
            }
      }
      }
      BKNI_Sleep(pstInfo->uiPollFrequency); /* Wait specified time before checking for more debug log data */
   }

   for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
   {
      for ( eArcInstance = 0; eArcInstance < BVCE_ArcInstance_eMax; eArcInstance++ )
      {
         fclose(pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].hFileOutput);
         pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].hFileOutput = NULL;
      }
   }
}

void
BTST_GetEncoderDebugLog(BTST_P_Context *pContext)
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;
   unsigned uiInstance;
   BVCE_ArcInstance eArcInstance;

   /* toggle it */
   if ( true == pstInfo->bEnableViceDebugLogCaptureHandler )
   {
      pstInfo->bEnableViceDebugLogCaptureHandler = false;
   }
   else
   {
      pstInfo->bEnableViceDebugLogCaptureHandler = true;
   }

   if ( pstInfo->bEnableViceDebugLogCaptureHandler )
   {
      BKNI_Printf("Poll Frequency (in ms) (0 = default(%d ms)): ", 500);
      BTST_P_Scanf(pContext, "%u", &pstInfo->uiPollFrequency);
      if ( 0 == pstInfo->uiPollFrequency ) pstInfo->uiPollFrequency = 500;

      printf("start to capture video encoder debug log\n");
      for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
      {
         for ( eArcInstance = 0; eArcInstance < BVCE_ArcInstance_eMax; eArcInstance++ )
         {
            pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].uiSize = 512 * 1024;
            /* system memory instead of device memory */
            pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].pBuffer
                     = BKNI_Malloc(pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].uiSize);

            if ( NULL == pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].pBuffer )
            {
               BDBG_ERR(("Error allocating Arc [%d][%d] debug log", uiInstance, eArcInstance));
               pstInfo->bEnableViceDebugLogCaptureHandler = false;
               return;
            }

            BKNI_Memset(
               pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].pBuffer,
               0,
               pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].uiSize
            );
         }
      }

      BTST_CreateTask(&pstInfo->hTaskCaptureViceDebugLog, BTST_P_CaptureViceDebugLogHandler, (void*) pstInfo);
   }
   else
   {
      printf("stop to capture video encoder debug log\n");
      BKNI_Sleep(pstInfo->uiPollFrequency*2);
      TEST_CLOSE_HANDLE(BTST_DestroyTask, pstInfo->hTaskCaptureViceDebugLog);
      BKNI_Sleep(100);

      for ( uiInstance = 0; uiInstance < VCE_ENCODER_NUM_INSTANCES; uiInstance++ )
      {
         for ( eArcInstance = 0; eArcInstance < BVCE_ArcInstance_eMax; eArcInstance++ )
         {
            if ( NULL != pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].pBuffer )
            {
               BKNI_Free(pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].pBuffer);
               pstInfo->instance[uiInstance].stVCEDebugLogInfo[eArcInstance].pBuffer = NULL;
            }
         }
      }
   }
   return;
}

void BTST_StandbyEncoder
        (BTST_P_Context *pContext)
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   BVCE_Power_Standby( pstInfo->instance[pstInfo->state.uiInstance].hVce );
}

void BTST_ResumeEncoder
        ( BTST_P_Context *pContext )
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   BVCE_Power_Resume( pstInfo->instance[pstInfo->state.uiInstance].hVce );
}

#define VCE_DEBUG_MAX_COMMAND_LENGTH 2048
void BTST_DebugEncoder
         ( BTST_P_Context *pContext )
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;
   unsigned uiArcInstance = 0;
   unsigned uiNumCommands = 0;
   char szCommand[VCE_DEBUG_MAX_COMMAND_LENGTH];
   size_t uiCommandLength = 0;

   BKNI_Memset( szCommand, 0, VCE_DEBUG_MAX_COMMAND_LENGTH );

   BKNI_Printf("Arc Instance (0 = PicArc, 1 = MBArc): ");
   BTST_P_Scanf(pContext, "%u", &uiArcInstance);

   BKNI_Printf("Num Commands: ");
   BTST_P_Scanf(pContext, "%u", &uiNumCommands);

   {
      unsigned i;
      for ( i = 0; i < uiNumCommands; i++ )
      {
         BKNI_Printf("   Command %2d: ", i+1);
         BTST_P_Scanf(pContext, "%s", &szCommand[uiCommandLength]);
         uiCommandLength += strlen( &szCommand[uiCommandLength] );

         /* Add Null Terminator */
         uiCommandLength++;
      }
      /* Add Double Null Terminator */
      uiCommandLength++;

      BDBG_ASSERT( uiCommandLength < VCE_DEBUG_MAX_COMMAND_LENGTH );
   }


   BVCE_Debug_SendCommand(
      pstInfo->instance[pstInfo->state.uiInstance].hVce,
      uiArcInstance,
      szCommand
      );
}

#ifndef BTST_VDC_ONLY
#define SUPPORT_XUD 1

#define BTST_USERDATA_MAX_FIELDS 1
#define BTST_USERDATA_MAX_DESC_PER_FIELD 3

static void BTST_P_UserDataEncodeCb_isr
   ( void                   *pvhContext,
    int                      iParam2,
    void                    *pvVdcData )

{
  BVDC_Display_CallbackData *pDisplayCbData = (BVDC_Display_CallbackData *)pvVdcData;
  BTST_VCEEncoder_Channel_Info *pstChannelInfo = (BTST_VCEEncoder_Channel_Info *) pvhContext;

#if !SUPPORT_XUD
  uint32_t ulStgPictureId    = pDisplayCbData->ulStgPictureId;
#if 0
  uint32_t ulDecodePictureId = pDisplayCbData->ulDecodePictureId;
#endif
  BUDP_DCCparse_ccdata stCCData[BTST_USERDATA_MAX_FIELDS];
  uint8_t auiFieldInfo[BTST_USERDATA_MAX_FIELDS*(sizeof(BUDP_Encoder_FieldInfo) + ( ( BTST_USERDATA_MAX_DESC_PER_FIELD * sizeof( BUDP_Encoder_PacketDescriptor ) ) - sizeof( BUDP_Encoder_PacketDescriptor) ) )];
  BUDP_Encoder_FieldInfo *pstFieldInfo = (BUDP_Encoder_FieldInfo *) auiFieldInfo;
  unsigned uiFieldInfoSize = BTST_USERDATA_MAX_FIELDS*(sizeof(BUDP_Encoder_FieldInfo) + ( ( BTST_USERDATA_MAX_DESC_PER_FIELD * sizeof( BUDP_Encoder_PacketDescriptor ) ) - sizeof( BUDP_Encoder_PacketDescriptor) ) );


  BKNI_Memset(pstFieldInfo, 0, uiFieldInfoSize);
  BKNI_Memset(stCCData, 0, sizeof( BUDP_DCCparse_ccdata ) * BTST_USERDATA_MAX_FIELDS);
#endif

  /* Call stg picture id update with the information
   * provided in pVdcData */
  if( (pDisplayCbData->stMask.bStgPictureId) && ( pstChannelInfo->bEncodeStarted ) )
  {
     BDBG_MSG(("STG ISR (stgPicId=%d, decodePicId=%d]", pDisplayCbData->ulStgPictureId, pDisplayCbData->ulDecodePictureId));
#if SUPPORT_XUD
     {
        BXUDlib_DisplayInterruptHandler_isr(pstChannelInfo->cc.hXud, pDisplayCbData);
     }
#else
     {
        unsigned i;

        for ( i = 0; i < BTST_USERDATA_MAX_FIELDS; i++ )
        {
           stCCData[i].bIsAnalog = true;
           stCCData[i].cc_valid = true;
           stCCData[i].cc_data_1 = pstInfo->uiCCByte++;
           stCCData[i].cc_data_2 = pstInfo->uiCCByte++;
        }

        {
           BUDP_Encoder_FieldInfo *pstCurrentFieldInfo = pstFieldInfo;

           for ( i = 0; i < BTST_USERDATA_MAX_FIELDS; i++ )
           {
              unsigned j;

              pstCurrentFieldInfo->uiStgPictureId = ulStgPictureId;
              pstCurrentFieldInfo->ePolarity = (pstInfo->eCCPolarity++)%BAVC_Polarity_eFrame;
              pstCurrentFieldInfo->uiNumDescriptors = BTST_USERDATA_MAX_DESC_PER_FIELD;

              for ( j = 0; j < BTST_USERDATA_MAX_DESC_PER_FIELD; j++ )
              {
                 switch (j)
                 {
                    case 0:
                       pstCurrentFieldInfo->stPacketDescriptor[j].ePacketFormat = BUDP_DCCparse_Format_DVS157;
                       pstCurrentFieldInfo->stPacketDescriptor[j].data.stDvs157.stCC.uiNumLines = 1;
                       pstCurrentFieldInfo->stPacketDescriptor[j].data.stDvs157.stCC.astLine = &stCCData[i];
                       break;
                    case 1:
                       pstCurrentFieldInfo->stPacketDescriptor[j].ePacketFormat = BUDP_DCCparse_Format_DVS053;
                       pstCurrentFieldInfo->stPacketDescriptor[j].data.stDvs053.stCC.uiNumLines = 1;
                       pstCurrentFieldInfo->stPacketDescriptor[j].data.stDvs053.stCC.astLine = &stCCData[i];
                       break;
                    case 2:
                       pstCurrentFieldInfo->stPacketDescriptor[j].ePacketFormat = BUDP_DCCparse_Format_ATSC53;
                       pstCurrentFieldInfo->stPacketDescriptor[j].data.stAtsc53.stCC.uiNumLines = 1;
                       pstCurrentFieldInfo->stPacketDescriptor[j].data.stAtsc53.stCC.astLine = &stCCData[i];
                       break;
                    default:
                       BDBG_ASSERT(0);
                 }
              }

              pstCurrentFieldInfo = BUDP_ENCODER_FIELDINFO_GET_NEXT( pstCurrentFieldInfo );
           }
        }

        {
           unsigned uiQueuedCount = 0;

           BVCE_Channel_UserData_AddBuffers_isr(
              pstInfo->hVceCh[pstInfo->uiCCChannelNum],
              pstFieldInfo,
              BTST_USERDATA_MAX_FIELDS,
              &uiQueuedCount
              );

           if ( uiQueuedCount != BTST_USERDATA_MAX_FIELDS )
           {
              BDBG_ERR(("uiCount (%d) != uiQueuedCount (%d)",
                 BTST_USERDATA_MAX_FIELDS,
                 uiQueuedCount));
           }
        }
     }
#endif
  }

  BSTD_UNUSED(iParam2);
  return;
}

void BTST_P_UserDataDecodeCb_isr( void * pParm1, int parm2 )
{
   BTST_VCEEncoder_Channel_Info *pstChannelInfo = (BTST_VCEEncoder_Channel_Info *) pParm1;
   BAVC_USERDATA_info     stUserDataInfo;
   unsigned uiUserDataCount = 0;
   BSTD_UNUSED( parm2 );

   while ( BERR_SUCCESS == BXVD_Userdata_Read_isr( pstChannelInfo->cc.hXvdUserData, &stUserDataInfo ) )
   {
#if SUPPORT_XUD
      BXUDlib_UserDataHandler_isr(pstChannelInfo->cc.hXud, &stUserDataInfo );
#endif
      uiUserDataCount++;
   }

   BDBG_MSG(("XVD User Data ISR [%d]", uiUserDataCount));
}
#endif

void BTST_UserDataEncode
    ( BTST_P_Context              *pContext )
 {
#ifdef BTST_VDC_ONLY
   BSTD_UNUSED( pContext );
#else
    BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
    bool *pbStgCb =
       &(pContext->abStgCb[pContext->iSelectedComp]);
    BVDC_Display_Settings *pDispSettings = &pContext->stDispSettings[pContext->iSelectedComp];
    BVDC_DisplayTg      eMasterTg = pDispSettings->eMasterTg;
    BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;
    BXVD_ChannelHandle hXvdCh = ( BXVD_ChannelHandle ) pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

    if((!hDisplay) || (!BTST_P_USED_STG(eMasterTg)))
    {
       return;
    }

    if ( true == pstInfo->state.bCompatibilityMode )
    {
       printf("Channel number: ");
       BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiChannel);
    }

    if(*pbStgCb)
    {
       /* Teardown XVD User Data Callback */
       {
          BXVD_Userdata_Enable(
             pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXvdUserData,
             false
             );

          BXVD_Userdata_UninstallInterruptCallback(
             pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXvdUserData,
             BTST_P_UserDataDecodeCb_isr
          );

          BXVD_Userdata_Close(
             pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXvdUserData
             );

          pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXvdUserData = NULL;
       }

       /* Teardown VDC STG Callback */
       {
          printf("Disabled STG User Data Encoder callback.\n");
          /*BVDC_Display_UnInstallRateChangeCallback(hDisplay);*/
          BVDC_Display_InstallCallback(
             hDisplay,
             NULL,
             (void*)pContext,
             BHDM_Callback_Type_eVideoChange);
       }

#if SUPPORT_XUD
       /* Destroy XUDlib */
       {
          BXUDlib_Destroy( pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXud );
          pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXud = NULL;
       }
#endif
    }
    else
    {
#if SUPPORT_XUD
       /* Create XUDlib */
       {
          BXUDlib_CreateSettings stXUDlibCreateSettings;

          BXUDlib_GetDefaultCreateSettings( &stXUDlibCreateSettings );

          BXUDlib_Create( &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXud, &stXUDlibCreateSettings );

          {
             BXUDlib_Settings stXudSettings;

             BXUDlib_GetSettings( pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXud, &stXudSettings );

             stXudSettings.sinkInterface.pPrivateSinkContext = pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh;
             stXudSettings.sinkInterface.userDataAdd_isr = (BXUDlib_UserDataSink_Add) BVCE_Channel_UserData_AddBuffers_isr;
             stXudSettings.sinkInterface.userDataStatus_isr = (BXUDlib_UserDataSink_Status) BVCE_Channel_UserData_GetStatus_isr;

             BXUDlib_SetSettings( pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXud, &stXudSettings );
          }
       }
#endif

       /* Setup VDC STG Callback */
       {
          BVDC_Display_CallbackSettings   stSettings;

          printf("Enabled STG User Data Encoder callback.\n");
          BVDC_Display_GetCallbackSettings(hDisplay, &stSettings);
          stSettings.stMask.bStgPictureId = true;
          BVDC_Display_SetCallbackSettings(hDisplay, &stSettings);

          BVDC_Display_InstallCallback(
             hDisplay,
             BTST_P_UserDataEncodeCb_isr,
             &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel],
             BHDM_Callback_Type_eVideoChange);
       }

       /* Setup XVD User Data Callback */
       {
          BXVD_Userdata_Settings stXvdUserDataSettings;

          BXVD_Userdata_GetDefaultSettings( &stXvdUserDataSettings );
          BXVD_Userdata_Open(
             hXvdCh,
             &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXvdUserData,
             &stXvdUserDataSettings
             );

          BXVD_Userdata_InstallInterruptCallback(
             pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXvdUserData,
             BTST_P_UserDataDecodeCb_isr,
             &pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel],
             0);

          BXVD_Userdata_Enable(
             pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].cc.hXvdUserData,
             true
             );
       }
    }

    /* Toggle */
    *pbStgCb = !(*pbStgCb);

    if(pContext->bApplyImmediately)
    {
       BTST_P_ApplyChanges(pContext);
    }
#endif
    return;
 }

void BTST_BeginNewRapEncode
         ( BTST_P_Context *pContext )
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   if ( true == pstInfo->state.bCompatibilityMode )
   {
      printf("Channel number: ");
      BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiChannel);
   }

   BVCE_Channel_BeginNewRAP(
      pstInfo->instance[pstInfo->state.uiInstance].channel[pstInfo->state.uiChannel].hVceCh
      );

   BKNI_Printf("Inserted New RAP\n");
}

void BTST_SetActiveEncoder
         ( BTST_P_Context *pContext )
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   printf("Instance number (0-%d): ", NUM_INSTANCES-1);
   BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiInstance);
   if ( pstInfo->state.uiInstance >= NUM_INSTANCES )
   {
      printf("Invalid Instance (%d)...Defaulting to 0\n", pstInfo->state.uiInstance);
      pstInfo->state.uiInstance = 0;
   }

   printf("Channel number (0-%d): ", VCE_ENCODER_NUM_ENCODE_CHANNELS-1);
   BTST_P_Scanf(pContext, "%u", &pstInfo->state.uiChannel);
   if ( pstInfo->state.uiChannel >= VCE_ENCODER_NUM_ENCODE_CHANNELS )
   {
      printf("Invalid Channel (%d)...Defaulting to 0\n", pstInfo->state.uiChannel);
      pstInfo->state.uiChannel = 0;
   }
}

void BTST_GetActiveEncoder
         ( BTST_P_Context *pContext )
{
   BTST_VCEEncoder_Info *pstInfo = (BTST_VCEEncoder_Info *) pContext->pEncoderInfo;

   printf("Active Encoder = [%d][%d]\n", pstInfo->state.uiInstance, pstInfo->state.uiChannel );
}

void BTST_InitializeEncoder
         ( BTST_P_Context *pContext )
{
   BTST_VCEEncoder_Info *pstInfo = pContext->pEncoderInfo;

   if ( NULL == pstInfo )
   {
      pstInfo = (BTST_VCEEncoder_Info *) BKNI_Malloc(sizeof(BTST_VCEEncoder_Info));

      if ( NULL == pstInfo )
      {
         BDBG_ERR(("Error allocating encoder context"));
         BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
         return;
      }

      BKNI_Memset(pstInfo, 0, sizeof(BTST_VCEEncoder_Info));
      pContext->pEncoderInfo = (void *) pstInfo;
      pstInfo->hReg = pContext->hReg;
   }
   else
   {
      BKNI_Free(pstInfo);
      pContext->pEncoderInfo = NULL;
   }
}

