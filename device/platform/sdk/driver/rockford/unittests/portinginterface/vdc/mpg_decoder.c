/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mpg_decoder.c $
 * $brcm_Revision: Hydra_Software_Devel/64 $
 * $brcm_Date:
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/mpg_decoder.c $
 * 
 * Hydra_Software_Devel/64   10/9/12 10:17a vanessah
 * SW7425-2501: removal of unnecessary header files
 * 
 * Hydra_Software_Devel/63   9/26/12 3:23p nilesh
 * SW7425-2568: Added TSM mode support for playback decode
 * 
 * Hydra_Software_Devel/62   9/12/12 6:07p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/61   8/16/12 5:47p vanessah
 * SW7425-2501: clean up unnecessary include file
 * 
 * Hydra_Software_Devel/60   6/29/12 4:38p vanessah
 * SW7425-3364: remove reference of obsolete ulVertFreqMask
 * 
 * Hydra_Software_Devel/59   5/23/12 11:32a vanessah
 * SW7360-23: add 7360 a0 support
 * 
 * Hydra_Software_Devel/58   5/18/12 3:47p vanessah
 * SW7425-3069: add resource leak change test
 * 
 * Hydra_Software_Devel/57   5/3/12 1:32p tdo
 * SW7584-21: Add PI/vdc support for 7584
 *
 * Hydra_Software_Devel/56   3/1/12 11:05a tdo
 * SW7425-2330: Put back the old SVD heap size
 *
 * Hydra_Software_Devel/55   2/29/12 7:00p tdo
 * SW7435-9: Bring up VDC for 7435A0
 *
 * Hydra_Software_Devel/54   2/29/12 12:44p tdo
 * SW7425-2330: Add svc/mvc support back
 *
 * Hydra_Software_Devel/53   2/10/12 4:13p tdo
 * SW7425-2330: Remove warning
 *
 * Hydra_Software_Devel/52   2/10/12 4:11p tdo
 * SW7425-2330: Enable power management 2.0 in appframework
 *
 * Hydra_Software_Devel/51   12/15/11 12:53p nilesh
 * SW7425-1110: Fixed CC dump when NULL packets are enabled
 *
 * Hydra_Software_Devel/50   12/14/11 11:04p nilesh
 * SW7425-1967: Added support for SEI userdata
 *
 * Hydra_Software_Devel/49   12/14/11 10:47a nilesh
 * SW7425-1110: Added CC Dump option toggle
 *
 * Hydra_Software_Devel/48   11/30/11 4:28p pntruong
 * SW7429-16: Initial bringup.
 *
 * Hydra_Software_Devel/47   10/12/11 11:02a pntruong
 * SW7429-16: Initial port.
 *
 * Hydra_Software_Devel/46   5/4/11 11:29a tdo
 * SW7422-417: Add cmd to toggle 3d cap for decode channel open
 *
 * Hydra_Software_Devel/45   4/27/11 4:16p tdo
 * SW7231-133, SW7422-417: Add support for multiple channel decode within
 * a decoder with streamer input and playback  Add option for users to
 * manually force SVD to only decode AVC, this is needed to do multi
 * channel decode with SVD due to memory allocation issue.  Cleanup old
 * code.
 *
 * Hydra_Software_Devel/44   4/15/11 1:54p tdo
 * SW7422-417: Add MVC/SVC support to vdc_test
 *
 * Hydra_Software_Devel/43   3/29/11 4:59p darnstein
 * SW7405-3191: Use BXVD enums when calling a BXVD function.
 *
 * Hydra_Software_Devel/42   1/18/11 2:01p yuxiaz
 * SW7552-17: Merge from Branch for 7552 support.
 *
 * Hydra_Software_Devel/SW7552-17/1   12/20/10 7:31p xhuang
 * SW7552-17: add 7552 support
 *
 * Hydra_Software_Devel/41   12/10/10 2:05p hongtaoz
 * SW7425-32: revert back previous checkin by accident;
 *
 * Hydra_Software_Devel/40   12/10/10 2:02p hongtaoz
 * SW7425-32: use EMU utility to access between file and device memory;
 * tried to separate xpt muxer PID/rave channels from playback decoder's
 * channels;
 *
 * Hydra_Software_Devel/39   12/3/10 12:25a hongtaoz
 * SW7425-32: added DM instance select for multi-decode test; add
 * 7425/22/20 to the multi-decode list; added 7425 gfx support;
 *
 * Hydra_Software_Devel/38   12/2/10 3:08p katrep
 * SW7231-16:add more support for 7231
 *
 * Hydra_Software_Devel/37   11/18/10 5:05p darnstein
 * SW7231-16: port to 7344 and 7346.
 *
 * Hydra_Software_Devel/36   11/11/10 6:45p pntruong
 * SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
 * stb chips.  Removed dead codes.
 *
 * Hydra_Software_Devel/35   11/1/10 2:03p yuxiaz
 * SW7358-17: Added 7358 support to vdc_test.
 *
 * Hydra_Software_Devel/34   10/29/10 3:09p tdo
 * SW7422-13: add 7422 support to vdc_test
 *
 * Hydra_Software_Devel/33   10/26/10 6:39p hongtaoz
 * SW7425-32: added XPT compile support for 7425;
 *
 * Hydra_Software_Devel/32   10/19/10 10:51a pntruong
 * SW7425-16, SW3548-1650: Adapted to the removal of
 * BXVD_ChannelSettings.bFGTEnable.  Also get rid of all reference to fgt
 * in vdctest app.
 *
 * Hydra_Software_Devel/31   12/15/09 5:49p yuxiaz
 * SW7408-13: Get vdc_test compile for 7408. Clean up compile flags.
 *
 * Hydra_Software_Devel/30   12/1/09 4:46p rpan
 * SW7468-22: Got vdc_test compiled for 7468.
 *
 * Hydra_Software_Devel/29   11/23/09 4:00p rpan
 * SW7468-22: Initial work for 7468.
 *
 * Hydra_Software_Devel/28   10/15/09 2:20p yuxiaz
 * SW7405-3184: Change decode resolution for mosaic mode.
 *
 * Hydra_Software_Devel/27   10/14/09 4:49p tdo
 * SW7420-376: Bringup appframework for 7410
 *
 * Hydra_Software_Devel/26   8/31/09 5:57p albertl
 * SW7125-13: Added initial 7125 support.
 *
 * Hydra_Software_Devel/25   8/4/09 1:01p darnstein
 * PR55225: mechanical updates for 7342 chipset.
 *
 * Hydra_Software_Devel/24   7/31/09 6:49p syang
 * PR 55812:  add 7550 support
 *
 * Hydra_Software_Devel/23   7/10/09 5:55p rpan
 * PR55188: Convert scanf() to BTST_P_Scanf().
 *
 * Hydra_Software_Devel/22   6/16/09 9:08p tdo
 * PR55956: Add VDC support for 7420B0.
 *
 * Hydra_Software_Devel/21   5/18/09 11:10a tdo
 * PR53239: Remove usage of private defintions in vdc unittest.
 *
 * Hydra_Software_Devel/20   3/17/09 1:59p darnstein
 * PR53265: Remove BVBI, BVBIlib software. It never worked.
 *
 * Hydra_Software_Devel/19   2/4/09 1:41p tdo
 * PR51627: add vdc_test support for 7336
 *
 * Hydra_Software_Devel/18   12/2/08 5:42p tdo
 * PR46484: Bringup vdc_test
 *
 * Hydra_Software_Devel/17   7/21/08 1:12p rpan
 * PR36501: Free up allocated RAVE memory before openning a new context.
 * More xpt call clean-up.
 *
 * Hydra_Software_Devel/16   6/11/08 1:25p tdo
 * PR41488: Fix compiling error for 3563 build
 *
 * Hydra_Software_Devel/15   6/11/08 12:57p tdo
 * PR41488: Cleaning warnings
 *
 * Hydra_Software_Devel/14   4/8/08 11:14a tdo
 * PR41488: Add 3556 build support
 *
 * Hydra_Software_Devel/13   3/20/08 1:40p yuxiaz
 * PR39807: Static buffer bring up on 3548.
 *
 * Hydra_Software_Devel/12   12/28/07 2:15p rpan
 * PR38468: Updated to match the new 7400 XVD interface.
 *
 * Hydra_Software_Devel/11   12/14/07 11:17a tdo
 * PR36898: Bringup 7335
 *
 * Hydra_Software_Devel/10   11/9/07 11:58a yuxiaz
 * PR34540: 7325 bringup.
 *
 * Hydra_Software_Devel/9   10/31/07 3:29p yuxiaz
 * PR29569: Added FGT memory in BXVD_Open.
 *
 * Hydra_Software_Devel/8   10/30/07 11:56a yuxiaz
 * PR29569, PR36290: Only enable bFGTEnable on 7405 for now.
 *
 * Hydra_Software_Devel/7   10/29/07 2:58p yuxiaz
 * PR34540: Add support for 7325.
 *
 * Hydra_Software_Devel/6   10/25/07 3:31p yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/5   10/17/07 1:41p rpan
 * PR34857: Fixed a build issue for 7405 and 7440.
 *
 * Hydra_Software_Devel/4   10/16/07 5:23p rpan
 * PR34857: Got it to build with PREREL XVD.
 *
 * Hydra_Software_Devel/3   10/16/07 2:45p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   10/9/07 5:16p rpan
 * PR34857: Initial revision.
 *
 *
 ***************************************************************************/

#include <stdio.h>          /* for printf */
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "platform.h"
#include "framework.h"
#include "framework_board.h"

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bkni.h"           /* kernel interface */
#include "bkni_multi.h"     /* kernel interface */
#include "btst_kni.h"       /* to handle task */

/* format information */
#include "bfmt.h"

/* audio video common */
#include "bavc.h"

/* top level interrupts */
#include "int1.h"
#include "bint_plat.h"

/* test include */
#include "config.h"
#include "test_util.h"
#include "interactive.h"
#include "mpg_decoder.h"
#include "commands.h"
#include "bmem_debug.h"

#ifndef IKOS_EMULATION
#include "bxpt.h"
#include "bxpt_pcr.h"
#include "bxpt_pcr_offset.h"

#include "bxpt_rave.h"
#include "bxpt_rsbuf.h"
#endif

#if (BFramework_NUM_XVD_CORES > 0)
#include "bxvd.h"
#elif (BFramework_NUM_MVD_CORES > 0)
#include "bmvd.h"
#endif
#include "btmr.h"

#if (FRAMEWORK_DO_I2C == 1)
#include "bi2c.h"
#endif

#if MACROVISION_SUPPORT
#include "bvdc_macrovision.h"
#endif

#if BTST_P_SUPPORT_HSM
#include "bsp_s_commands.h"
#include "bsp_s_misc.h"
#include "bhsm_misc.h"
#include "bsp_s_hw.h"
#include "bsp_s_keycommon.h"
#include "bhsm_keyladder.h"
#endif

#include "budp_dccparse.h"

BDBG_MODULE(vdc_test);

#ifdef VXWORKS
#define USE_INTMGR 1
#include "bcmmips.h"
#endif

/***************************************************************************
 * Global variables
 *
 */
int g_iXptChNum = 0;
extern BTST_P_Context *g_pContext;
extern BAVC_XptContextMap CtxMap[BTST_P_MAX_CHANNELS];
extern BAVC_XptContextMap EnhCtxMap[BTST_P_MAX_CHANNELS];
extern bool g_bCapture;

#if (BFramework_NUM_XVD_CORES >0)

typedef struct XVDMemoryConfig
{
      uint32_t uiContextSize;
      uint32_t uiCabacSize;
      uint32_t uiVideoAtomSize;
      uint32_t uiVideoAtomCount;
} XVDMemoryConfig;

#define SUPPORTED_VIDEOCOMPRESSION_STD_COUNT 4
#define SUPPORTED_RESOLUTION_COUNT_VIDEO 4
#define SUPPORTED_RESOLUTION_COUNT_STILL 2


/* XVD Memory requirements based on "XVD Core Mosaic Mode API", Rev
 * 2.6, June 11, 2007 */
const XVDMemoryConfig sXVDMemoryTable_Video[SUPPORTED_VIDEOCOMPRESSION_STD_COUNT][SUPPORTED_RESOLUTION_COUNT_VIDEO] =
{
   /* AVC Video */
   {  /* Context,  Cabac bin,   Vid Blk,  Blk Cnt */
      {3625984, 5210112, 3440640, 7}, /* AVC HD */
      {1683456, 2084864,  786432, 9}, /* AVC SD */
      {1179648,  417792,  294912, 9}, /* AVC CIF */
      { 706960,   73728,  131072, 7}  /* AVC QCIF */
   },
   /* MPEG2 Video */
   {  /* Context,  Cabac bin,   Vid Blk,  Blk Cnt */
      { 491480, 2097152, 3440640, 6}, /* MPEG2 HD w/ BTP */
      { 491480, 1048576,  786432, 6}, /* MPEG2 SD w/ BTP */
      { 337464,  312448,  294912, 5}, /* MPEG2 CIF */
      { 337464,   54656,  131072, 5}  /* MPEG2 QCIF */
   },
   /* VC1 Video */
   {  /* Context,  Cabac bin,   Vid Blk,  Blk Cnt */
      {2723936, 3906176, 3440640, 5}, /* VC1 HD */
      {1289216, 1562496,  786432, 5}, /* VC1 SD */
      { 896000,  312448,  294912, 5}, /* VC1 CIF */
      { 699393,   54656,  131072, 5}  /* VC1 QCIF */
   },
   {  /* Context,  Cabac bin,   Vid Blk,  Blk Cnt */
      {2723936, 3906176, 3440640, 5}, /* MPEG4/DivX HD */
      {1289216, 1562496,  786432, 5}, /* MPEG4/DivX SD */
      { 896000,  312448,  294912, 5}, /* MPEG4/DivX CIF */
      { 699393,   54656,  131072, 5}  /* MPEG4/DivX QCIF */
   }
};
#endif

static void BTST_P_UserDataDecodeCb_isr( void * pParm1, int parm2 )
{
   BTST_MDEC_Info       *pContext = (BTST_MDEC_Info *) pParm1;

   BAVC_USERDATA_info     stUserDataInfo;
   unsigned uiUserDataCount = 0;
   BSTD_UNUSED( parm2 );

   while ( BERR_SUCCESS == BXVD_Userdata_Read_isr( pContext->userdata[parm2].hXvdUserData, &stUserDataInfo ) )
   {
      uint32_t offset = 0;

      while (offset < stUserDataInfo.ui32UserDataBufSize)
      {
         unsigned i;
         size_t bytesParsed = 0;
         uint8_t ccCount = 0;
         BUDP_DCCparse_ccdata ccData[32];

         BUDP_DCCparse_isr(&stUserDataInfo, offset, &bytesParsed, &ccCount, ccData);
         if ( 0 == ccCount ) BUDP_DCCparse_SEI_isr(&stUserDataInfo, offset, &bytesParsed, &ccCount, ccData);

         for( i = 0; i < ccCount; i++ )
         {
            FILE *hFile = NULL;
            bool bHeader = false;

            if ( true == ccData[i].bIsAnalog )
            {
               if ( ( ( true == pContext->userdata[parm2].output608.bFilterNull ) && ( 0 != (ccData[i].cc_data_1 & 0x7F) ) )
                    || ( ( false == pContext->userdata[parm2].output608.bFilterNull ) ) )
               {
                  hFile = pContext->userdata[parm2].output608.hFile;
                  if ( false == pContext->userdata[parm2].output608.bHeader )
                  {
                     bHeader = true;
                     pContext->userdata[parm2].output608.bHeader = true;
                  }
               }
            }
            else
            {
               if ( ( ( true == pContext->userdata[parm2].output708.bFilterNull ) && ( 0 != (ccData[i].cc_data_1) ) )
                    || ( ( false == pContext->userdata[parm2].output708.bFilterNull ) ) )
               {
                  hFile = pContext->userdata[parm2].output708.hFile;
                  if ( false == pContext->userdata[parm2].output708.bHeader )
                  {
                     bHeader = true;
                     pContext->userdata[parm2].output708.bHeader = true;
                  }
               }
            }

            if ( NULL != hFile )
            {
               if ( true == bHeader )
               {
                  fprintf( hFile, "Polarity,Format,CC Valid,CC Priority,CC Line Offset,CC Type,CC Data[0],CC Data[1]\n");
               }

               fprintf( hFile, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                  ccData[i].polarity,
                  ccData[i].format,
                  ccData[i].cc_valid,
                  ccData[i].cc_priority,
                  ccData[i].line_offset,
                  ccData[i].seq.cc_type,
                  ccData[i].cc_data_1,
                  ccData[i].cc_data_2
                  );
            }
         }

         offset += bytesParsed;
      }
      uiUserDataCount++;
   }

   BDBG_MSG(("XVD User Data ISR [%d]", uiUserDataCount));
}

void BTST_SetSTCUsingPTSInfo_Isr(
   void *pParm1,
   int iParm2,
   void *pData
   )
{
   BXVD_PTSInfo *pPTSInfo =  ((BXVD_PTSInfo *)pData);
   unsigned uiSTC = pPTSInfo->ui32RunningPTS - 6006;
   BXVD_ChannelHandle hXvdCh = (BXVD_ChannelHandle) pParm1;

   /* Only seed STC with a valid PTS */
   switch (pPTSInfo->ePTSType)
   {
      case BXVD_PTSType_eCoded:
      case BXVD_PTSType_eInterpolatedFromValidPTS:
         {
            BERR_Code rc;

            rc = BXPT_PcrOffset_SetStc_isr( GetXPTPcrOffset(iParm2), uiSTC );
            if ( BERR_SUCCESS == rc )
            {
               BXVD_SetSTCInvalidFlag_isr( hXvdCh, false );
            }
            else
            {
               BDBG_ERR(("Unable to set STC"));
            }
         }
         break;
      default:
         break;
   }

   return;
}

void BTST_StartDecode
    (BTST_P_Context          *pContext,
     uint16_t                 ui16VidPid,
     uint16_t                 ui16PcrPid,
     uint16_t                 ui16EnhPid,
     BAVC_StreamType          eStreamType,
     BAVC_VideoCompressionStd eVideoCmprStd,
     int                      iXptChNum)
{
    BTST_MDEC_ChannelHandle hMpgChn = pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

#if (BFramework_NUM_MVD_CORES == 0)
    BSTD_UNUSED(ui16VidPid);
    BSTD_UNUSED(ui16PcrPid);
    BSTD_UNUSED(ui16EnhPid);
    BSTD_UNUSED(eStreamType);
#endif

    if (IS_XVD(pContext->eSelectedMDec))
    {
#if (BFramework_NUM_XVD_CORES > 0)
        BXVD_DecodeSettings  decodeSettings;
        BXVD_GetDecodeDefaultSettings((BXVD_ChannelHandle)hMpgChn, &decodeSettings);

#if BTST_P_SUPPORT_MULTI_DECODE
		if (pContext->iSelectedMDecDm > 0)
        {
			decodeSettings.eDisplayInterrupt = BXVD_DisplayInterrupt_eOne;
        }
        else
        {
			decodeSettings.eDisplayInterrupt = BXVD_DisplayInterrupt_eZero;
        }
#endif

        decodeSettings.eVideoCmprStd = eVideoCmprStd;  /* Mpeg2 or AVC */
        decodeSettings.ulMultiStreamId = 0;
        decodeSettings.ulVideoSubStreamId = 0;
        decodeSettings.bCrcMode = g_bCapture;
        decodeSettings.bPlayback = pContext->bPlayback;
        decodeSettings.stDataXprtOutput.eXptOutputId = BAVC_XptOutputId_eParserBand1;
        decodeSettings.stDataXprtOutput.eXptSourceId = BAVC_XptOutputId_eParserBand2;
        decodeSettings.eTimeBase = pContext->iSelectedVidSrc? BAVC_Timebase_e1 : BAVC_Timebase_e0;
        decodeSettings.pContextMap = &CtxMap[iXptChNum];
		if(ui16EnhPid != 0x1FFF)
		{
			decodeSettings.aContextMapExtended[0] = &EnhCtxMap[iXptChNum];
			decodeSettings.uiContextMapExtNum = 1;
		}

		/* Setup XVD User Data Callback */
		if ( true == pContext->bCCDump )
		{
         BXVD_Userdata_Settings stXvdUserDataSettings;

         BXVD_Userdata_GetDefaultSettings( &stXvdUserDataSettings );
         BXVD_Userdata_Open(
            hMpgChn,
            &pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].hXvdUserData,
            &stXvdUserDataSettings
         );

         BXVD_Userdata_InstallInterruptCallback(
            pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].hXvdUserData,
            BTST_P_UserDataDecodeCb_isr,
            &pContext->astMDecInfo[pContext->eSelectedMDec],
            pContext->iSelectedMDecCh);

         BXVD_Userdata_Enable(
            pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].hXvdUserData,
            true
         );

         {
            unsigned uiFilterNull = 0;
            char str[256];

            printf("Filter NULL 608 CC? (0=no,1=yes): ");
            BTST_P_Scanf(pContext, "%u", &uiFilterNull);

            pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output608.bFilterNull = (0 != uiFilterNull );
            sprintf(str, "userdata_608_[%02x][%02x].csv",pContext->eSelectedMDec, pContext->iSelectedMDecCh);
            pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output608.hFile = fopen(str,"wb");
            pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output608.bHeader = false;

            uiFilterNull = 0;
            printf("Filter NULL 708 CC? (0=no,1=yes): ");
            BTST_P_Scanf(pContext, "%u", &uiFilterNull);

            pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output708.bFilterNull = (0 != uiFilterNull );
            sprintf(str, "userdata_708_[%02x][%02x].csv",pContext->eSelectedMDec, pContext->iSelectedMDecCh);
            pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output708.hFile = fopen(str,"wb");
            pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output708.bHeader = false;
         }
		}

      /* If playback, plumb the FirstPTSReady and PTSError callbacks */
      if ( ( true == pContext->bPlayback )
           && ( true == pContext->bTSMMode ) )
      {
         BXVD_InstallInterruptCallback(
            (BXVD_ChannelHandle) hMpgChn,
            BXVD_Interrupt_eFirstPTSReady,
            BTST_SetSTCUsingPTSInfo_Isr,
            (BXVD_ChannelHandle) hMpgChn,
            iXptChNum);

         BXVD_InstallInterruptCallback(
            (BXVD_ChannelHandle) hMpgChn,
            BXVD_Interrupt_ePTSError,
            BTST_SetSTCUsingPTSInfo_Isr,
            (BXVD_ChannelHandle) hMpgChn,
            iXptChNum);
      }

        BXVD_StartDecode((BXVD_ChannelHandle) hMpgChn, &decodeSettings);
#endif
    }
    else
    {
#if (BFramework_NUM_MVD_CORES > 0)
        uint16_t ui16XptBand;
        uint32_t ulRefreshRateChoice;
        BAVC_XptOutput stXptOutput;
        BMVD_VideoParams decodeParams;

        BSTD_UNUSED(eVideoCmprStd);
        ulRefreshRateChoice = 0;
        ui16XptBand = 1;
        stXptOutput.eXptOutputId =
            (ui16XptBand == 0) ?  BAVC_XptOutputId_eParserBand0 :
            (ui16XptBand == 1) ?  BAVC_XptOutputId_eParserBand1 :
            (ui16XptBand == 2) ?  BAVC_XptOutputId_eParserBand2 :
            (BAVC_XptOutputId_eParserBand0);

        BKNI_Memset(&decodeParams, 0, sizeof(BMVD_VideoParams));
        decodeParams.eStreamType    = eStreamType;
        decodeParams.ui16VideoPid = ui16VidPid;
        decodeParams.ui16PcrPid = ui16PcrPid;
        decodeParams.eTimeBase  = pContext->iSelectedVidSrc ? BAVC_Timebase_e1 : BAVC_Timebase_e0;
        decodeParams.stDataXprtOutput.eXptOutputId = stXptOutput.eXptOutputId;
        decodeParams.stDataXprtOutput.eXptSourceId = stXptOutput.eXptSourceId;
        decodeParams.eIconScalingMode = BMVD_IconMode_eDisable;
        decodeParams.bEnableRMM  = false;
        decodeParams.bSynchOnRAI = false;
        decodeParams.bContinuityCtrIgnore  = false;
        decodeParams.bEnableDropBOnOpenGOP = true;
        decodeParams.ui32MonitorRefreshRate =
            (ulRefreshRateChoice == 0) ? BFMT_VERT_60Hz :
            (ulRefreshRateChoice == 1) ? BFMT_VERT_59_94Hz : BFMT_VERT_50Hz ;
        decodeParams.sXptContextMap = CtxMap[iXptChNum];;

        BMVD_StartDecode((BMVD_ChannelHandle) hMpgChn, &decodeParams);
        BMVD_SetVideoDisplayMode(hMpgChn, BMVD_DisplayMode_eTimeStampManagedMode);
#endif
    }
}



void BTST_StopDecode
    (BTST_P_Context          *pContext)
{
    BTST_MDEC_ChannelHandle hMpgChn = pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

    if (IS_XVD(pContext->eSelectedMDec))
    {
#if (BFramework_NUM_XVD_CORES > 0)
        BXVD_StopDecode((BXVD_ChannelHandle) hMpgChn);

        /* Teardown XVD User Data Callback */
        if ( NULL != pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].hXvdUserData )
        {
           if ( NULL != pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output608.hFile )
           {
              fclose( pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output608.hFile );
              pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output608.hFile = NULL;

           }

           if ( NULL != pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output708.hFile )
           {
              fclose( pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output708.hFile );
              pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].output708.hFile = NULL;
           }

           BXVD_Userdata_Enable(
              pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].hXvdUserData,
              false
              );

           BXVD_Userdata_UninstallInterruptCallback(
                        pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].hXvdUserData,
                        BTST_P_UserDataDecodeCb_isr
                        );

           BXVD_Userdata_Close(
              pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].hXvdUserData
              );

           pContext->astMDecInfo[pContext->eSelectedMDec].userdata[pContext->iSelectedMDecCh].hXvdUserData = NULL;
        }
#endif
    }
    else
    {
#if (BFramework_NUM_MVD_CORES > 0)
        BMVD_StopDecode((BMVD_ChannelHandle) hMpgChn);
#endif
    }
}

static const  BAVC_VideoCompressionStd  VideoAVDCmprStdList[] =
{
	BAVC_VideoCompressionStd_eH264,
	BAVC_VideoCompressionStd_eMPEG2,
	BAVC_VideoCompressionStd_eVC1
};

static const  BAVC_VideoCompressionStd  VideoSVDCmprStdList[] =
{
	BAVC_VideoCompressionStd_eH264,
	BAVC_VideoCompressionStd_eMPEG2,
	BAVC_VideoCompressionStd_eVC1,
	BAVC_VideoCompressionStd_eSVC,
	BAVC_VideoCompressionStd_eMVC
};

void BTST_OpenDecoderChannel
    ( BTST_P_Context          *pContext )
{
    if (IS_XVD(pContext->eSelectedMDec))
    {
#if (BFramework_NUM_XVD_CORES >0)
        BXVD_ChannelSettings   channelSettings;
        BXVD_RevisionInfo      sRevInfo;
        BXVD_Handle            hXvd = (BXVD_Handle)pContext->astMDecInfo[pContext->eSelectedMDec].hMDec;
        BXVD_ChannelHandle     hXvdCh;
        uint32_t               ulChoice;
        BERR_Code              err=BERR_SUCCESS;

        /* (1) Open an XVD channel */
        BXVD_GetChannelDefaultSettings(hXvd, pContext->iSelectedMDecCh, &channelSettings);
        if(pContext->bResourceLeakAutoTest) {
            channelSettings.ui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_60Hz;
            channelSettings.eDecodeResolution = BXVD_DecodeResolution_eHD;
        }
        else
        {
        printf("Current channel monitor refresh rate: 0x%x\n", channelSettings.ui32MonitorRefreshRate);
        printf("Please choose new monitor rate: 0) 60Hz; 1) 50Hz; 2) 24Hz; 3) 25 Hz; 4) 30Hz: \n");
        BTST_P_Scanf(pContext, "%u", &ulChoice);
        switch(ulChoice)
        {
            case 4:
                channelSettings.ui32MonitorRefreshRate =
                    BXVD_MONITOR_REFRESH_RATE_30Hz;
                break;
            case 3:
                channelSettings.ui32MonitorRefreshRate =
                    BXVD_MONITOR_REFRESH_RATE_25Hz;
                break;
            case 2:
                channelSettings.ui32MonitorRefreshRate =
                    BXVD_MONITOR_REFRESH_RATE_24Hz;
                break;
            case 1:
                channelSettings.ui32MonitorRefreshRate =
                    BXVD_MONITOR_REFRESH_RATE_50Hz;
                break;
            case 0:
            default:
                channelSettings.ui32MonitorRefreshRate =
                    BXVD_MONITOR_REFRESH_RATE_60Hz;
                break;
        }

        TestError(BXVD_GetRevision(hXvd, &sRevInfo), "BXVD_GetRevision");
        printf("firmware revision: %lx\n", sRevInfo.ulDecoderFwRev);

        printf("Please choose max video decode resolution: HD(0), SD(1), CIF(2), QCIF(3): \n");
        BTST_P_Scanf(pContext, "%u", &ulChoice);

#if BTST_P_SUPPORT_MULTI_DECODE
        channelSettings.eDecodeResolution = BXVD_DecodeResolution_eHD + ulChoice;
        /*channelSettings.eVideoCmprStdList = BAVC_VideoCompressionStd_eH264;
        channelSettings.uiVideoCmprCount = 1;*/
#endif
        }

		if(pContext->astMDecInfo[pContext->eSelectedMDec].bSVD && pContext->bDecodeChan3d)
		{
			channelSettings.peVideoCmprStdList = (BAVC_VideoCompressionStd *) &VideoSVDCmprStdList;
			channelSettings.uiVideoCmprCount = sizeof(VideoSVDCmprStdList) / sizeof(BAVC_VideoCompressionStd);
		}
		else
		{
			channelSettings.peVideoCmprStdList = (BAVC_VideoCompressionStd *) &VideoAVDCmprStdList;
			channelSettings.uiVideoCmprCount = sizeof(VideoAVDCmprStdList) / sizeof(BAVC_VideoCompressionStd);
		}

        TestError(BXVD_OpenChannel(hXvd, &hXvdCh, pContext->iSelectedMDecCh, &channelSettings), "BXVD_OpenChannel");
        pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh] = (BTST_MDEC_ChannelHandle)hXvdCh;
        pContext->astMDecInfo[pContext->eSelectedMDec].aiXptChNum[pContext->iSelectedMDecCh] = g_iXptChNum;

        g_iXptChNum++;

Done:
        return;

#endif
    }
    else
    {
#if (BFramework_NUM_MVD_CORES > 0)
        BMVD_ChannelSettings stMvdChnDefs;
        BMVD_ChannelHandle hMpgChn = (BMVD_ChannelHandle)pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];
        const BFMT_VideoInfo *pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];
        BMEM_HeapInfo stHeapStart, stHeapEnd;

        /* (1) Open a mpeg channel. */
        BMVD_GetChannelDefaultSettings((BMVD_Handle)pContext->astMDecInfo[pContext->eSelectedMDec].hMDec,
            pContext->iSelectedMDecCh, BMVD_ChannelConfig_eVideoHD, &stMvdChnDefs);

        stMvdChnDefs.eYCbCrType = pContext->b422 ?
            BAVC_YCbCrType_e4_2_2 : BAVC_YCbCrType_e4_2_0;
        stMvdChnDefs.eBufferScheme = BMVD_BufferScheme_eFourBufferScheme;

        if (pContext->bSharedMem)
        {
            stMvdChnDefs.hCustomChannelHeap = pContext->hSharedMem;
            printf("Using shared memory for MVD\n");

            BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapStart);
            printf("Before: available shared memory = %d bytes\n", stHeapStart.ulLargestFree);
        }

        BMVD_OpenChannel((BMVD_Handle)pContext->astMDecInfo[pContext->eSelectedMDec].hMDec,
                          &hMpgChn, pContext->iSelectedMDecCh, &stMvdChnDefs);

        pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh] = (BTST_MDEC_ChannelHandle)hMpgChn;
        pContext->astMDecInfo[pContext->eSelectedMDec].aiXptChNum[pContext->iSelectedMDecCh] = g_iXptChNum;
        g_iXptChNum++;

        if (pContext->bSharedMem)
        {
            BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapEnd);
            printf("After : available shared memory = %d bytes\n", stHeapEnd.ulLargestFree);
            printf("MVD memory usage: %d bytes\n",
                stHeapStart.ulLargestFree - stHeapEnd.ulLargestFree);
        }
        #endif
    }
}


void BTST_CloseDecoderChannel
    ( BTST_P_Context          *pContext )
{
    BTST_MDEC_ChannelHandle hMDecCh = pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

	if(!pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh])
		return;

    if (IS_XVD(pContext->eSelectedMDec))
    {
#if (BFramework_NUM_XVD_CORES > 0)
        /* If previously decoded stop it now. */
        if(pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh])
        {
            BXVD_StopDecode((BXVD_ChannelHandle)hMDecCh);
            pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh] = false;
        }

        /* [1] Destroy a mpeg channel. */
        BXVD_CloseChannel((BXVD_ChannelHandle)hMDecCh);
#endif
    }
    else
    {
#if (BFramework_NUM_MVD_CORES > 0)
        /* If previously decoded stop it now. */
        if(pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh])
        {
            BMVD_StopDecode((BMVD_ChannelHandle)hMDecCh);
            pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh] = false;
        }

        BMVD_CloseChannel((BMVD_ChannelHandle)hMDecCh);
#endif
    }

    if(g_iXptChNum) g_iXptChNum--;
    pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh] = NULL;
}


void  BTST_OpenDecoder
    (BTST_P_Context          *pContext)
{
    int mdecIndex;
    void *pTmpPtr;

#if (BFramework_NUM_XVD_CORES > 0)
    BXVD_Settings stXvdDefSettings;

#if BTST_P_SUPPORT_MULTI_DECODE
    BXVD_DecodeMode stCustomDecodeMode[] =
    {
        BXVD_DecodeMode_eCustom,
        BXVD_DecodeMode_eTerminateList
    };
#endif

    BXVD_GetDefaultSettings(&stXvdDefSettings);
    stXvdDefSettings.hFrmMemory = pContext->astMemInfo[0].hMem;

#if BTST_P_SUPPORT_MULTI_DECODE
    stXvdDefSettings.pDecodeModes = stCustomDecodeMode;

    /* Hardcode to always use dual HD/HD size */
    stXvdDefSettings.stFWMemConfig.uiGeneralHeapSize =
        sXVDMemoryTable_Video[0][0].uiContextSize +
        sXVDMemoryTable_Video[0][0].uiContextSize;

    stXvdDefSettings.stFWMemConfig.uiCabacHeapSize=
        sXVDMemoryTable_Video[0][0].uiCabacSize + sXVDMemoryTable_Video[0][0].uiCabacSize;

    stXvdDefSettings.stFWMemConfig.uiPictureHeapSize=
        (sXVDMemoryTable_Video[0][0].uiVideoAtomSize *
        sXVDMemoryTable_Video[0][0].uiVideoAtomCount) +
        (sXVDMemoryTable_Video[0][0].uiVideoAtomSize *
        sXVDMemoryTable_Video[0][0].uiVideoAtomCount);
#endif

    for (mdecIndex = BTST_XVD_0; mdecIndex < BFramework_NUM_XVD_CORES; mdecIndex++)
    {
#if BFramework_NUM_SVD
		pContext->astMDecInfo[mdecIndex].bSVD = (mdecIndex == BTST_XVD_0) ? true : false;
#else
		pContext->astMDecInfo[mdecIndex].bSVD = false;
#endif

		if(pContext->astMDecInfo[mdecIndex].bSVD)
		{
			stXvdDefSettings.stFWMemConfig.uiGeneralHeapSize = 21 * 1024 * 1024;
			stXvdDefSettings.stFWMemConfig.uiCabacHeapSize   = 0;
			stXvdDefSettings.stFWMemConfig.uiPictureHeapSize = 53 * 1024 * 1024;
		}

        stXvdDefSettings.uiAVDInstance = mdecIndex;
        pTmpPtr = &pContext->astMDecInfo[mdecIndex].hMDec;
        BXVD_Open((BXVD_Handle *)pTmpPtr, pContext->hChp, pContext->hReg, NULL,
                  pContext->astMDecInfo[mdecIndex].hHeap, pContext->hInt, &stXvdDefSettings);
    }
#endif

#if (BFramework_NUM_MVD_CORES > 0)
    for (mdecIndex = BTST_MVD_0; mdecIndex < (BTST_MVD_0+BFramework_NUM_MVD_CORES); mdecIndex++)
    {
        pTmpPtr = &pContext->astMDecInfo[mdecIndex].hMDec;
        BMVD_Open((BMVD_Handle *)pTmpPtr, pContext->hChp, pContext->hReg,
                    pContext->astMDecInfo[mdecIndex].hHeap, pContext->hInt, NULL);
    }
#endif
}

void BTST_CloseDecoder
    (BTST_P_Context          *pContext)
{
    int mdecIndex;
#if (BFramework_NUM_XVD_CORES > 0)
    for (mdecIndex = BTST_XVD_0; mdecIndex < BFramework_NUM_XVD_CORES; mdecIndex++)
        TEST_CLOSE_HANDLE(BXVD_Close, pContext->astMDecInfo[mdecIndex].hMDec);
#endif

#if (BFramework_NUM_MVD_CORES > 0)
for (mdecIndex = BTST_MVD_0; mdecIndex < BFramework_NUM_MVD_CORES; mdecIndex++)
    TEST_CLOSE_HANDLE(BMVD_Close, pContext->astMDecInfo[mdecIndex].hMDec);
#endif

}

/* end of file */
