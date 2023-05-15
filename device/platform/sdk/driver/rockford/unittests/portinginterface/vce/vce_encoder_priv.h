/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: vce_encoder_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 3/12/12 11:26a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vce/vce_encoder_priv.h $
 * 
 * Hydra_Software_Devel/14   3/12/12 11:26a nilesh
 * SW7425-2568: Added option to not dump ES file with "fe" command
 * 
 * Hydra_Software_Devel/13   3/7/12 4:10p nilesh
 * SW7435-1: VCE Refactoring
 * 
 * Hydra_Software_Devel/SW7435-1/1   3/7/12 2:43p nilesh
 * SW7435-1: Refactored to support quad transcode
 * 
 * Hydra_Software_Devel/12   1/31/12 4:31p nilesh
 * SW7435-2: Added 7435 support
 * 
 * Hydra_Software_Devel/11   12/7/11 2:21p nilesh
 * SW7425-1877: Added XUDlib support
 * 
 * Hydra_Software_Devel/10   12/5/11 4:47p nilesh
 * SW7425-960: Added support for user data insertion via STG Display
 * Callback
 * 
 * Hydra_Software_Devel/9   10/14/11 12:16p nilesh
 * SW7425-1213: Added ability to specify ARC debug log polling frequency
 * 
 * Hydra_Software_Devel/8   9/28/11 4:25p nilesh
 * SW7425-890: Added correct extension to ES files indicating protocol
 * 
 * Hydra_Software_Devel/7   9/14/11 2:49p nilesh
 * SW7425-890: Fix dual encode logic
 * 
 * Hydra_Software_Devel/6   9/8/11 1:58p nilesh
 * SW7425-891: Add dual encode support
 * 
 * Hydra_Software_Devel/SW7425-891/1   9/8/11 12:43p nilesh
 * SW7425-891: Added dual channel support
 * 
 * Hydra_Software_Devel/5   4/4/11 4:55p nilesh
 * SW7425-134: Added support for changing bitrate and framerate on-the-fly
 * during encode
 * 
 * Hydra_Software_Devel/4   1/24/11 1:32p hongtaoz
 * SW7425-1: added file logging of video descriptors;
 * 
 * Hydra_Software_Devel/3   1/14/11 11:05a nilesh
 * SW7425-1: Added support for writing ES > 2GB. Added MB Arc debig log
 * support.  Increased max bitrate to 25Mbps.
 * 
 * Hydra_Software_Devel/2   12/20/10 3:51p nilesh
 * SW7425-1: Added VCE ITB/CDB buffer dump command "ice"
 * 
 * Hydra_Software_Devel/1   11/3/10 2:39p nilesh
 * SW7425-38: Moved encoder context struct to separate private header file
 *
 ***************************************************************************/

#ifndef VCE_ENCODER_PRIV_H_
#define VCE_ENCODER_PRIV_H_

#include "bxudlib.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#define VCE_ENCODER_NUM_INSTANCES 2
#define VCE_ENCODER_NUM_ENCODE_CHANNELS 2
#define VCE_ENCODER_NUM_OUTPUT_CHANNELS 2
#define VCE_ENCODER_NUM_OUTPUT_BUFFERS 2

typedef struct BTST_VCEDebugLog_Info
{
      void *pBuffer;
      size_t uiSize;
      FILE *hFileOutput;
} BTST_VCEDebugLog_Info;

typedef struct BTST_VCEEncoder_Channel_Info
{
   BVCE_Channel_OpenSettings stOpenSettings;

   BVCE_Channel_Handle       hVceCh;

   BVCE_Channel_StartEncodeSettings stStartEncodeSettings;
   BVCE_Channel_EncodeSettings stEncodeSettings;
   bool                      bEncodeStarted;
   /* BAVC_VideoCompressionStd  eProtocol; */

#ifndef BTST_VDC_ONLY
   struct
   {
      /* CC User Data */
      uint8_t uiCCByte;
      BAVC_Polarity eCCPolarity;
      BXVD_Userdata_Handle hXvdUserData;
      BXUDlib_Handle hXud;
   } cc;
#endif
} BTST_VCEEncoder_Channel_Info;

typedef struct BTST_VCEEncoder_Info
{
      BREG_Handle               hReg;

      struct
      {
         BVCE_OpenSettings stOpenSettings;

         BMEM_Handle               hMem;
         BVCE_Handle               hVce;
         BTST_VCEDebugLog_Info     stVCEDebugLogInfo[BVCE_ArcInstance_eMax];

         BTST_VCEEncoder_Channel_Info channel[VCE_ENCODER_NUM_ENCODE_CHANNELS];

         struct
         {
            BVCE_Output_OpenSettings stOutputOpenSettings;

            BVCE_Output_Handle        hVceOutput;
            unsigned                  uiFileIndex;
            size_t                    uiESBytesWrittenThisFile;
            FILE                     *hESOutput;
            FILE                     *hITBOutput;
            FILE                     *hCDBOutput;
         } output[VCE_ENCODER_NUM_OUTPUT_CHANNELS];

         struct
         {
            BVCE_OutputBuffers_Handle hVceOutputBuffers;
         } buffer[VCE_ENCODER_NUM_OUTPUT_BUFFERS];

      } instance[VCE_ENCODER_NUM_INSTANCES];

      BTST_TaskHandle           hTaskCaptureViceBuffer;
      bool                      bEnableViceBufferCaptureHandler;
      bool bSkipESDump;

      BTST_TaskHandle           hTaskCaptureViceItbCdbBuffer;
      bool                      bEnableViceItbCdbBufferCaptureHandler;

      BTST_TaskHandle           hTaskCaptureViceDebugLog;
      bool                      bEnableViceDebugLogCaptureHandler;
      unsigned                  uiPollFrequency;

      struct
      {
         unsigned uiNumInstancesOpen;
         unsigned uiInstance;
         unsigned uiChannel;
         bool bAutoOpenFirstChannel;
         bool bCompatibilityMode;
      } state;
} BTST_VCEEncoder_Info;

#ifdef __cplusplus
}
#endif

#endif /* VCE_ENCODER_PRIV_H_ */
