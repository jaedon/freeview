/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: xport.c $
 * $brcm_Revision: Hydra_Software_Devel/94 $
 * $brcm_Date:
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/xport.c $
 * 
 * Hydra_Software_Devel/94   10/15/12 5:18p vanessah
 * SW7425-2501: fix xpt_rave EMU_PREV_BYTE_REMOVE bit set error
 * 
 * Hydra_Software_Devel/93   10/5/12 5:26p vanessah
 * SW7425-2501: remove 7400 and 7550 dependency
 * 
 * Hydra_Software_Devel/92   9/26/12 3:23p nilesh
 * SW7425-2568: Added TSM mode support for playback decode
 * 
 * Hydra_Software_Devel/91   9/12/12 6:07p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/90   8/22/12 12:01p vanessah
 * SW7425-2501: correct 7408 compile error
 * 
 * Hydra_Software_Devel/89   8/17/12 4:08p vanessah
 * SW7425-2501: further cleanup xport file
 * 
 * Hydra_Software_Devel/88   8/16/12 5:47p vanessah
 * SW7425-2501: clean up unnecessary include file
 * 
 * Hydra_Software_Devel/87   7/19/12 12:09p vanessah
 * SW7360-3: 7360 streamer input support
 * 
 * Hydra_Software_Devel/86   7/16/12 4:35p tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/85   7/9/12 5:31p vanessah
 * SW7425-2501: 3548/3556 support removal
 *
 * Hydra_Software_Devel/84   6/19/12 5:35p chengs
 * SW7584-5: Add 7584 support.
 *
 * Hydra_Software_Devel/83   5/23/12 11:33a vanessah
 * SW7360-23: add 7360 a0 support
 *
 * Hydra_Software_Devel/82   5/3/12 1:32p tdo
 * SW7584-21: Add PI/vdc support for 7584
 *
 * Hydra_Software_Devel/81   3/15/12 10:13a tdo
 * SW7435-9: Bringup streamer input for 7435A0 in vdc_test
 *
 * Hydra_Software_Devel/80   3/1/12 7:57p tdo
 * SW7435-9: bringup 7435 A0 vdc_test
 *
 * Hydra_Software_Devel/79   2/15/12 10:00a nilesh
 * SW7425-2217: Merged Pid2Buffer Support
 *
 * Hydra_Software_Devel/SW7425-2217/2   2/1/12 2:43p nilesh
 * SW7425-2217: Fixed PID2Buffer PID remap support
 *
 * Hydra_Software_Devel/SW7425-2217/1   1/26/12 5:29p nilesh
 * SW7425-2217: Added PID2Buffer support
 *
 * Hydra_Software_Devel/78   12/6/11 3:57p tdo
 * SW7435-9: Add support for 7435
 *
 * Hydra_Software_Devel/77   11/30/11 4:28p pntruong
 * SW7429-16: Initial bringup.
 *
 * Hydra_Software_Devel/76   10/12/11 1:30p pntruong
 * SW7429-16: Initial port.
 *
 * Hydra_Software_Devel/75   10/12/11 11:01a pntruong
 * SW7429-16: Initial port.
 *
 * Hydra_Software_Devel/74   4/27/11 4:15p tdo
 * SW7231-133, SW7422-417: Add support for multiple channel decode within
 * a decoder with streamer input and playback  Add option for users to
 * manually force SVD to only decode AVC, this is needed to do multi
 * channel decode with SVD due to memory allocation issue.  Cleanup old
 * code.
 *
 * Hydra_Software_Devel/73   4/22/11 5:41p tdo
 * SW7425-366: Add support for playback of more than 1 file to support
 * dual playback and decode.
 *
 * Hydra_Software_Devel/72   4/15/11 1:54p tdo
 * SW7422-417: Add MVC/SVC support to vdc_test
 *
 * Hydra_Software_Devel/71   4/6/11 6:05p darnstein
 * SW7231-16: fix streamer input band number for 7346.
 *
 * Hydra_Software_Devel/70   4/6/11 4:12p darnstein
 * SW7231-16: fix streamer input band number for 7344.
 *
 * Hydra_Software_Devel/69   1/26/11 3:18p yuxiaz
 * SW7400-2998, SW7425-32: Only reduce cdb/itb buffer size for mosaic
 * mode.
 *
 * Hydra_Software_Devel/68   1/25/11 4:31p yuxiaz
 * SW7400-2998, SW7425-32: Need to flush RAVE context after it is
 * disabled.
 *
 * Hydra_Software_Devel/67   1/25/11 4:05p yuxiaz
 * SW7400-2998, SW7425-32: Flush RAVE context before stopping playback.
 *
 * Hydra_Software_Devel/66   1/18/11 3:42p hongtaoz
 * SW7425-32: reduced cdb/itb buffer size for decoder to allow more mosaic
 * decodes on vdc_test;
 *
 * Hydra_Software_Devel/65   1/18/11 2:01p yuxiaz
 * SW7552-17: Merge from Branch for 7552 support.
 *
 * Hydra_Software_Devel/SW7552-17/1   12/20/10 7:31p xhuang
 * SW7552-17: add 7552 support
 *
 * Hydra_Software_Devel/64   1/6/11 4:29p tdo
 * SW7231-23: Remove include bxpt_priv.h from playback.[ch]
 *
 * Hydra_Software_Devel/63   12/30/10 11:40a vanessah
 * SW7422-125:  XPT rave default setting
 *
 * Hydra_Software_Devel/62   12/14/10 3:58p darnstein
 * SW7231-2: Do ForceValid and SyncDetectEn for 7344 and 7346 and 7231,
 * same as for 7422.
 *
 * Hydra_Software_Devel/61   12/12/10 12:31a hongtaoz
 * SW7425-32: resolve xpt conflict between muxer and decoder;
 *
 * Hydra_Software_Devel/60   12/10/10 2:26p pntruong
 * SW7231-16: Default exclude hsm.
 *
 * Hydra_Software_Devel/59   12/10/10 2:03p hongtaoz
 * SW7425-32: use util to access between file and device memory for emu;
 * tried to separate muxer's xpt channels vs playback decoder's xpt
 * channels;
 *
 * Hydra_Software_Devel/58   12/8/10 4:52p hongtaoz
 * SW7425-32: brng up playback test on emulation;
 *
 * Hydra_Software_Devel/57   12/2/10 3:09p katrep
 * SW7231-16:add more support for 7231
 *
 * Hydra_Software_Devel/56   11/22/10 4:40p darnstein
 * SW7231-16: fix a stupid typo from previous check-in.
 *
 * Hydra_Software_Devel/55   11/18/10 2:37p darnstein
 * SW7231-16: port to 7344 and 7346.
 *
 * Hydra_Software_Devel/54   11/11/10 9:13p tdo
 * SW7422-13: Bring up 7422 streamer input
 *
 * Hydra_Software_Devel/53   11/11/10 12:48p tdo
 * SW7422-13: Add parser band and input band defines for 7422
 *
 * Hydra_Software_Devel/52   11/1/10 2:03p yuxiaz
 * SW7358-17: Added 7358 support to vdc_test.
 *
 * Hydra_Software_Devel/51   10/26/10 6:40p hongtaoz
 * SW7425-32: added XPT compile support for 7425;
 *
 * Hydra_Software_Devel/50   1/14/10 5:44p yuxiaz
 * SW7405-3290: Enable HSM so vdc_test will run after reboot.
 *
 * Hydra_Software_Devel/49   12/15/09 5:51p yuxiaz
 * SW7408-13: Get vdc_test compile for 7408. Clean up compile flags.
 *
 * Hydra_Software_Devel/48   12/4/09 5:21p tdo
 * SW7408-23: Support playback for ES stream
 *
 * Hydra_Software_Devel/47   12/4/09 3:58p tdo
 * SW7408-23: Add playback feature for digital source
 *
 * Hydra_Software_Devel/46   12/1/09 4:47p rpan
 * SW7468-22: Got vdc_test compiled for 7468.
 *
 * Hydra_Software_Devel/45   11/24/09 10:24a rpan
 * SW7468-22: Correct XPT input and parser band assignment.
 *
 * Hydra_Software_Devel/44   11/23/09 4:00p rpan
 * SW7468-22: Initial work for 7468.
 *
 * Hydra_Software_Devel/43   11/17/09 2:05p syang
 * SW7550-70: update input band and parser band
 *
 * Hydra_Software_Devel/42   10/30/09 4:16p yuxiaz
 * SW7405-3290: Config streamer input band on 7405.
 *
 * Hydra_Software_Devel/41   10/14/09 4:49p tdo
 * SW7420-376: Bringup appframework for 7410
 *
 * Hydra_Software_Devel/40   9/1/09 3:22p pntruong
 * SW7125-13: Added initial 7125 support.  Corrected the
 * BTST_XPT_STREAMER_INPUTBAND for 7125.
 *
 * Hydra_Software_Devel/39   8/31/09 5:57p albertl
 * SW7125-13: Added initial 7125 support.
 *
 * Hydra_Software_Devel/38   8/20/09 4:25p tdo
 * PR55219: include transport "band" settings for 7342
 *
 * Hydra_Software_Devel/37   8/13/09 6:18p darnstein
 * PR55219: include transport "band" settings for 7340 and 7342.
 *
 * Hydra_Software_Devel/36   8/4/09 11:07a jrubio
 * PR55232: add 7342/7340 support
 *
 * Hydra_Software_Devel/35   7/31/09 6:49p syang
 * PR 55812:  add 7550 support
 *
 * Hydra_Software_Devel/34   6/16/09 9:08p tdo
 * PR55956: Add VDC support for 7420B0.
 *
 * Hydra_Software_Devel/33   5/21/09 3:13p tdo
 * PR54768: Fully support dual decode with same PID
 *
 * Hydra_Software_Devel/32   5/18/09 2:13p tdo
 * PR54768: Fix compiling error
 *
 * Hydra_Software_Devel/31   5/18/09 11:12a tdo
 * PR54768: Fix decode and channel change issue in vdc_test
 *
 * Hydra_Software_Devel/30   3/9/09 4:41p rpan
 * PR52203: Implemented some stress tests and debugging facility targeting
 * VNET reconfiguration and multi-buffering.
 *
 * Hydra_Software_Devel/29   3/6/09 10:02a rpan
 * PR52750: Fixed 7420 XPT PCR channel number programming.
 *
 * Hydra_Software_Devel/28   2/20/09 6:05p tdo
 * PR50391: Correctly handling PID and RAVE for all chips
 *
 * Hydra_Software_Devel/27   2/18/09 11:48a tdo
 * PR50391: Remove warning
 *
 * Hydra_Software_Devel/26   2/17/09 4:57p tdo
 * PR50391: Correctly support dual decode for 7420 with same pid decoding.
 *
 * Hydra_Software_Devel/25   2/4/09 1:41p tdo
 * PR51627: add vdc_test support for 7336
 *
 * Hydra_Software_Devel/24   2/3/09 9:54p tdo
 * PR50391: Support dual decode with same pid
 *
 * Hydra_Software_Devel/23   12/24/08 2:01p tdo
 * PR50391: Bringup dual decode for 7420
 *
 * Hydra_Software_Devel/22   12/18/08 7:22p tdo
 * PR50391: Bringup streamer input for 7420
 *
 * Hydra_Software_Devel/21   12/2/08 5:42p tdo
 * PR46484: Bringup vdc_test
 *
 * Hydra_Software_Devel/20   11/12/08 6:08p tdo
 * PR48717: init new fields added
 *
 * Hydra_Software_Devel/19   10/10/08 4:44p maivu
 * PR 46960: Added support for ASI streamer card
 *
 * Hydra_Software_Devel/18   10/1/08 11:17a rpan
 * PR46564: Get default PRC config settings so that timebase infor won't
 * be garbage value.
 *
 * Hydra_Software_Devel/17   7/28/08 9:48a rpan
 * PR36501: Save the work for parsing PSI messages.
 *
 * Hydra_Software_Devel/16   7/21/08 1:12p rpan
 * PR36501: Free up allocated RAVE memory before openning a new context.
 * More xpt call clean-up.
 *
 * Hydra_Software_Devel/15   7/18/08 3:22p rpan
 * PR36501: Hook up tuner and demodulator with xport.
 *
 * Hydra_Software_Devel/14   7/18/08 2:31p rpan
 * PR36501: Fixed a couple of transport setup bug and cleaned up the code
 * a bit.
 *
 * Hydra_Software_Devel/13   6/17/08 2:11p maivu
 * PR 35883: Add support for streams with high bitrate (greater than
 * 25Mbps). Since vdc_test does not support PVR features, it should be
 * fine to have it enable all the time, even for low bit rate streams.
 * Only side-effect would be bandwidth limited for PVR functions.
 *
 * Hydra_Software_Devel/12   6/5/08 2:59p yuxiaz
 * PR39807: MPEG bring up on 3548.
 *
 * Hydra_Software_Devel/11   3/20/08 1:40p yuxiaz
 * PR39807: Static buffer bring up on 3548.
 *
 * Hydra_Software_Devel/10   1/15/08 2:56p rpan
 * PR38468: Workaround a 7400 D0 XPT input band HW issue.
 *
 * Hydra_Software_Devel/9   12/14/07 3:54p tdo
 * PR36898: Bringup video source for 7335
 *
 * Hydra_Software_Devel/8   11/10/07 2:51p yuxiaz
 * PR34540: 7325 bringup.
 *
 * Hydra_Software_Devel/7   11/9/07 11:58a yuxiaz
 * PR34540: 7325 bringup.
 *
 * Hydra_Software_Devel/6   10/29/07 2:59p yuxiaz
 * PR34540: Add support for 7325.
 *
 * Hydra_Software_Devel/5   10/16/07 2:47p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   9/21/07 1:53p rpan
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
#include "bmem_debug.h"

#ifndef IKOS_EMULATION
#include "bxpt.h"
#include "bxpt_pcr.h"
#include "bxpt_pcr_offset.h"
#include "bxpt_rave.h"
#include "bxpt_spid.h"
#include "bxpt_rsbuf.h"

#if (BFramework_NUM_XVD_CORES > 0)
#include "bxvd.h"
#endif
#include "btmr.h"

#if (FRAMEWORK_DO_I2C == 1)
#include "bi2c.h"
#endif

#include "bchp_sun_top_ctrl.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_bvnf_intr2_4.h"

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

#include "playback.h"

void DumpMap( BREG_Handle hReg, BAVC_XptContextMap *CtxMap );
#endif

BDBG_MODULE(vdc_test);

#ifdef VXWORKS
#define USE_INTMGR 1
#include "bcmmips.h"
#endif

/* Input Band and Parser Band assignments
 */
#if BTST_P_SUPPORT_TUNER
/* 3548 only so far */
#define BTST_XPT_DEMOD_INPUTBAND        2
#else
#define BTST_XPT_DEMOD_INPUTBAND        0xFFFFFFF /* A invalid number so that xpt PI can catch the problem */
#endif


/***************************************************************************
 * Global variables
 *
 */

BAVC_XptContextMap       CtxMap[BTST_P_MAX_CHANNELS];
BAVC_XptContextMap       EnhCtxMap[BTST_P_MAX_CHANNELS];

/* globals shared with muxer */
BXPT_Rave_Handle         g_hRave = NULL;
BXPT_Handle              g_hXpt  = NULL;

extern BTST_P_Context *g_pContext;
extern int g_iXptChNum;

#ifndef IKOS_EMULATION

/* All related handles are static so that they may be shared between main
   task and keystroke handler, as necessary. */
struct {
    BREG_Handle              hReg;
    BXPT_Handle              hXpt;
    BXPT_Rave_Handle         hRave;

    Playback_Handle          hPb[MAX_PLAYBACKS];

    BXPT_RaveCx_Handle       hCtx[BTST_P_MAX_CHANNELS];
    bool                     bRaveCtxAllocated[BTST_P_MAX_CHANNELS];

    BXPT_RaveCx_Handle       hEnhCtx[BTST_P_MAX_CHANNELS];
    bool                     bEnhRaveCtxAllocated[BTST_P_MAX_CHANNELS];

    int                      iRavePidChan[BTST_P_MAX_CHANNELS];
    uint16_t                 uiPid[BTST_P_MAX_CHANNELS];

    int                      iEnhRavePidChan[BTST_P_MAX_CHANNELS];
    uint16_t                 uiEnhPid[BTST_P_MAX_CHANNELS];

    BXPT_PcrOffset_Handle    hPcrOffset[BTST_P_MAX_CHANNELS];
    BXPT_PCR_Handle          hPcr[BTST_P_MAX_CHANNELS];

#if BTST_P_SUPPORT_HSM
    BHSM_Handle              hHsm;
    BHSM_ChannelHandle       hHsmCh[BHSM_HwModule_eMax];
#endif

    BTST_P_Xpt_Src           eCurrentXptSrc;
    BXPT_RaveCx_Handle       hCurrentRaveCtx;
    BXPT_RaveCx_Handle       hCurrentEnhRaveCtx;
} handles;

static BXPT_Rave_AvSettings AvCtxCfg = {
    BAVC_StreamType_eTsMpeg,
    BAVC_ItbEsType_eAvcVideo,
    BAVC_StreamType_eEs,
    {
        {0xFE, 0x00, false, true},
        {0x00, 0x00, false, false},
        {0x00, 0x00, false, false},
        {0x00, 0x00, false, false}
    },
    0xEF,
    0xBD,
    0, 0, 0, 0,
    false,
    false,
    false,
    false,
#if BXPT_HAS_BPP_SEARCH
    false,
#endif
    false,
    0xBE, 0xBE,
    true,
};

BMEM_Handle ghMem;


/***************************************************************************
 ***************************************************************************/
BERR_Code InitXPT
(
    const BFramework_Info    *pFrameworkInfo
)
{
    BERR_Code err;
    BXPT_DefaultSettings      Defaults;
    BXPT_Rave_ChannelSettings RaveDefaults;
    BCHP_Handle               hChp = pFrameworkInfo->hChp;
    BMEM_Handle               hMem = pFrameworkInfo->hMem;

    BINT_Handle               hInt = pFrameworkInfo->hInt;
    int                       i;

    BKNI_Memset((void*)&handles, 0, sizeof handles);
    ghMem = hMem;
    handles.hReg = pFrameworkInfo->hReg;

    for (i=0; i < BTST_P_MAX_CHANNELS; i++)
    {
        handles.hCtx[i] = NULL;
        handles.bRaveCtxAllocated[i] = false;
        handles.uiPid[i] = 0;
        handles.iRavePidChan[i] = -1;

        handles.hEnhCtx[i] = NULL;
        handles.bEnhRaveCtxAllocated[i] = false;
        handles.uiEnhPid[i] = 0;
        handles.iEnhRavePidChan[i] = -1;
    }

    /* Rave specific initialization. Set USE_RAVE == 1 to enable. */
    TestError(BXPT_GetDefaultSettings(&Defaults, hChp), "BXPT_GetDefaultSettings");
    TestError(BXPT_Open(&handles.hXpt, hChp, handles.hReg, hMem, hInt, &Defaults), "BXPT_Open");
    g_hXpt = handles.hXpt;

    for (i=0; i < MAX_PLAYBACKS; i++)
    {
        /* Playback support */
        handles.hPb[i] = Init_Playback(handles.hXpt, hMem, hInt, i);
    }

#if BXPT_HAS_IB_PID_PARSERS
{
    BXPT_InputBandConfig InputBandConfig;

    BXPT_GetInputBandConfig( handles.hXpt, BFramework_XPT_STREAMER_INPUTBAND, &InputBandConfig );
    InputBandConfig.SyncDetectEn = true;
    InputBandConfig.ForceValid = true;
    BXPT_SetInputBandConfig( handles.hXpt, BFramework_XPT_STREAMER_INPUTBAND, &InputBandConfig );
}

    TestError(BXPT_SetParserDataSource(handles.hXpt, BFramework_XPT_PARSERBAND_0,
            BXPT_DataSource_eInputBand, BFramework_XPT_STREAMER_INPUTBAND),"BXPT_SetParserDataSource");
    TestError(BXPT_SetParserEnable(handles.hXpt, BFramework_XPT_PARSERBAND_0, true), "BXPT_SetParserEnable");
#endif

    handles.eCurrentXptSrc = BTST_XPT_SRC_Streamer;

    TestError(BXPT_Rave_GetChannelDefaultSettings(handles.hXpt,BTST_XVD_RAVE_CHANNEL_ID_BASE,&RaveDefaults),
               "BXPT_Rave_GetChannelDefaultSettings");

    TestError(BXPT_Rave_OpenChannel(handles.hXpt, &handles.hRave, BTST_XVD_RAVE_CHANNEL_ID_BASE, &RaveDefaults),
               "BXPT_Rave_OpenChannel");
    g_hRave = handles.hRave;

#if(BXPT_HAS_RSBUF)
    /* Needed for streams with bitrate > 25Mbps. Can enable all time, as this is a test app only */
    TestError(BXPT_RsBuf_SetBandDataRate(handles.hXpt, BTST_XVD_RAVE_CHANNEL_ID_BASE, 31666995, 188),
               "BXPT_RsBuf_SetBandDataRate");
#endif

/* On security-programmed part, need to open HSM to enable Rave */
#if BTST_P_SUPPORT_HSM
{
    BERR_Code            err;
    BHSM_Settings        hsmSetting;
    BHSM_ChannelSettings hsmChnlSetting;
    BHSM_SetMiscBitsIO_t setMiscBitsIO;
    BHSM_InitKeySlotIO_t keyslot_io;

    /* Start HSM init */
    TestError(BHSM_GetDefaultSettings(&hsmSetting, pFrameworkInfo->hChp), "BHSM_GetDefaultSettings");

    TestError(BHSM_Open(&handles.hHsm, pFrameworkInfo->hReg,
        pFrameworkInfo->hChp, pFrameworkInfo->hInt, &hsmSetting), "BHSM_Open");

    TestError(BHSM_GetChannelDefaultSettings(handles.hHsm, BHSM_HwModule_eCmdInterface1, &hsmChnlSetting),
        "BHSM_GetChannelDefaultSettings");

    TestError(BHSM_Channel_Open(handles.hHsm,
                                   &handles.hHsmCh[BHSM_HwModule_eCmdInterface1],
                                    BHSM_HwModule_eCmdInterface1,
                                    &hsmChnlSetting), "BHSM_Channel_Open");

    TestError(BHSM_Channel_Open(handles.hHsm,
                                   &handles.hHsmCh[BHSM_HwModule_eCmdInterface2],
                                    BHSM_HwModule_eCmdInterface2,
                                    &hsmChnlSetting), "BHSM_Channel_Open");

    BKNI_Memset(&keyslot_io, 0, sizeof(keyslot_io));
    keyslot_io.unKeySlotType0Num = 10;
    keyslot_io.unKeySlotType1Num = 7;
    keyslot_io.unKeySlotType2Num = 11;
    keyslot_io.unKeySlotType3Num = 5;
    keyslot_io.unKeySlotType4Num = 6;
    keyslot_io.unKeySlotType5Num = 2;
    keyslot_io.unKeySlotType6Num = 8;
    /* Disregard errors, as this can only be run once per board boot. */
    BHSM_InitKeySlot(handles.hHsm, &keyslot_io);

    setMiscBitsIO.setMiscBitsSubCmd = BCMD_SetMiscBitsSubCmd_eRaveBits;
    setMiscBitsIO.bEnableWriteIMem = 1;
    setMiscBitsIO.bEnableReadIMem = 1;
    setMiscBitsIO.bEnableReadDMem = 1;
    setMiscBitsIO.bEnableEncBypass = 0;
    setMiscBitsIO.bDisableClear = 1;
    TestError(BHSM_SetMiscBits(handles.hHsm, &setMiscBitsIO), "BHSM_SetMiscBits");
}
#endif

Done:
    return BERR_SUCCESS;
}

/***************************************************************************
 ***************************************************************************/
BERR_Code StartXPT
(
    BTST_P_Context           *pContext,
    char                     *fn,
    BAVC_StreamType           eStreamType,
    uint16_t                  ui16VideoPid,
    uint16_t                  ui16PcrPid,
    uint16_t                  ui16EnhPid,
    int                       uiChannelNum
)
{
	
    BTST_MDEC_Id              eMpgDecoder;
    bool                      bPlayback;
    BTST_P_Xpt_Src            eXptSrc;
    BERR_Code err;
    BXPT_PCR_XptStreamPcrCfg  pcrCfg;
    BXPT_PcrOffset_Settings   pcrOffsetSettings;
	BAVC_CdbItbConfig CdbItb_XVD;
    BXPT_Rave_AvSettings      stAvCfg;
	BVDC_Display_Settings *pDispSettings = &pContext->stDispSettings[pContext->iSelectedComp];
	BVDC_DisplayTg      eMasterTg = pDispSettings->eMasterTg;
    eMpgDecoder = pContext->eSelectedMDec;
    bPlayback   = pContext->bPlayback;
    eXptSrc     = pContext->eXptSrc;

	if(g_pContext->bEnableMosaicMode)
	{
		CdbItb_XVD.Cdb.Length = 3*1024*1024;
		CdbItb_XVD.Itb.Length = 131072;
	}
	else
	{
		CdbItb_XVD.Cdb.Length = 8*1024*1024;
		CdbItb_XVD.Itb.Length = 8*1024*1024;
	}
	CdbItb_XVD.Cdb.Alignment     = 8;
	CdbItb_XVD.Cdb.LittleEndian  = false;
	CdbItb_XVD.Itb.Alignment     = 8;
	CdbItb_XVD.Itb.LittleEndian  = false;
	CdbItb_XVD.UsePictureCounter = false;

#if BXPT_HAS_IB_PID_PARSERS
    if (eXptSrc == BTST_XPT_SRC_Streamer)
    {
        TestError(BXPT_SetParserDataSource(handles.hXpt, BFramework_XPT_PARSERBAND_0 + uiChannelNum,
            BXPT_DataSource_eInputBand, BFramework_XPT_STREAMER_INPUTBAND),"BXPT_SetParserDataSource");
        TestError(BXPT_SetParserEnable(handles.hXpt, BFramework_XPT_PARSERBAND_0 + uiChannelNum, true), "BXPT_SetParserEnable");
    }
#endif
    handles.eCurrentXptSrc = eXptSrc;

    /* Get stream type from user input */
    AvCtxCfg.InputFormat = eStreamType;
	AvCtxCfg.Transcoding = BTST_P_USED_STG(eMasterTg);

	/* Set up RAVE ctx */
    handles.hCurrentRaveCtx = handles.hCtx[uiChannelNum];
    if (handles.bRaveCtxAllocated[uiChannelNum])
    {
        TestError(BXPT_Rave_FreeContext(handles.hCurrentRaveCtx),
                   "BXPT_Rave_FreeContext");
        handles.bRaveCtxAllocated[uiChannelNum] = false;
    }

    TestError(BXPT_Rave_AllocContext(handles.hRave, BXPT_RaveCx_eAv, &CdbItb_XVD, &handles.hCtx[uiChannelNum]),
                   "BXPT_Rave_AllocContext");

 
    handles.bRaveCtxAllocated[uiChannelNum] = true;

	/* Set up Enhanced RAVE ctx only if Enhanced PID is set */
	if(ui16EnhPid != 0x1FFF)
	{
	    handles.hCurrentEnhRaveCtx = handles.hEnhCtx[uiChannelNum];
	    if (handles.bEnhRaveCtxAllocated[uiChannelNum])
	    {
	        TestError(BXPT_Rave_FreeContext(handles.hCurrentEnhRaveCtx),
	                   "BXPT_Rave_FreeContext");
	        handles.bEnhRaveCtxAllocated[uiChannelNum] = false;
	    }


        TestError(BXPT_Rave_AllocContext(handles.hRave, BXPT_RaveCx_eAv, &CdbItb_XVD, &handles.hEnhCtx[uiChannelNum]),
                       "BXPT_Rave_AllocContext");
	    handles.bEnhRaveCtxAllocated[uiChannelNum] = true;
	}

    /* get default threshold */
    TestError(BXPT_Rave_GetAvConfig(handles.hCtx[uiChannelNum], &stAvCfg), "BXPT_Rave_GetAvConfig");
    AvCtxCfg.CdbLowerThreshold = stAvCfg.CdbLowerThreshold;
    AvCtxCfg.CdbUpperThreshold = stAvCfg.CdbUpperThreshold;
    AvCtxCfg.ItbLowerThreshold = stAvCfg.ItbLowerThreshold;
    AvCtxCfg.ItbUpperThreshold = stAvCfg.ItbUpperThreshold;
	AvCtxCfg.BandHoldEn        = true;

    if(handles.uiPid[uiChannelNum] != ui16VideoPid && handles.uiPid[uiChannelNum] != 0)
    {
        TestError(BXPT_FreePidChannel(handles.hXpt, uiChannelNum), "BXPT_EnablePidChannel");
    }
    if(handles.iRavePidChan[uiChannelNum] != -1)
        TestError(BXPT_Rave_RemovePidChannel(handles.hCtx[uiChannelNum], handles.iRavePidChan[uiChannelNum]), "BXPT_Rave_RemovePidChannel");

    handles.uiPid[uiChannelNum] = ui16VideoPid;
    if(bPlayback)
    {
        TestError(BXPT_ConfigurePidChannel(handles.hXpt,
                                           uiChannelNum * 2,
                                           ui16VideoPid,
                                           BXPT_PB_PARSER(BTST_XVD_PB_CHANNEL_ID_BASE + uiChannelNum)),
                  "BXPT_ConfigurePidChannel");
    }
    else
    {
        TestError(BXPT_ConfigurePidChannel(handles.hXpt,
                                           uiChannelNum * 2,
                                           ui16VideoPid,
                                           BFramework_XPT_PARSERBAND_0 + uiChannelNum),
                  "BXPT_ConfigurePidChannel");
    }
    TestError(BXPT_EnablePidChannel(handles.hXpt, uiChannelNum * 2), "BXPT_EnablePidChannel");
    TestError(BXPT_Rave_AddPidChannel(handles.hCtx[uiChannelNum], uiChannelNum * 2, false), "BXPT_Rave_AddPidChannel");
    handles.iRavePidChan[uiChannelNum] = uiChannelNum * 2;

	if(ui16EnhPid != 0x1FFF)
	{
	    if(handles.uiEnhPid[uiChannelNum] != ui16EnhPid && handles.uiEnhPid[uiChannelNum] != 0)
	    {
	        TestError(BXPT_FreePidChannel(handles.hXpt, uiChannelNum * 2 + 1), "BXPT_EnablePidChannel");
	    }
	    if(handles.iEnhRavePidChan[uiChannelNum] != -1)
	        TestError(BXPT_Rave_RemovePidChannel(handles.hEnhCtx[uiChannelNum], handles.iEnhRavePidChan[uiChannelNum]), "BXPT_Rave_RemovePidChannel");

	    handles.uiEnhPid[uiChannelNum] = ui16EnhPid;
        if(bPlayback)
        {
            TestError(BXPT_ConfigurePidChannel(handles.hXpt,
                                               uiChannelNum * 2 + 1,
                                               ui16EnhPid,
                                               BXPT_PB_PARSER(BTST_XVD_PB_CHANNEL_ID_BASE) + uiChannelNum),
                      "BXPT_ConfigurePidChannel");
        }
        else
        {
            TestError(BXPT_ConfigurePidChannel(handles.hXpt,
                                               uiChannelNum * 2 + 1,
                                               ui16EnhPid,
                                               BFramework_XPT_PARSERBAND_0 + uiChannelNum),
                      "BXPT_ConfigurePidChannel");
        }

        TestError(BXPT_EnablePidChannel(handles.hXpt, uiChannelNum * 2 + 1), "BXPT_EnablePidChannel");
        TestError(BXPT_Rave_AddPidChannel(handles.hEnhCtx[uiChannelNum], uiChannelNum * 2 + 1, false), "BXPT_Rave_AddPidChannel");
        handles.iEnhRavePidChan[uiChannelNum] = uiChannelNum * 2 + 1;
    }

    TestError(BXPT_Rave_SetAvConfig(handles.hCtx[uiChannelNum], &AvCtxCfg), "BXPT_Rave_SetAvConfig");
    TestError(BXPT_Rave_EnableContext(handles.hCtx[uiChannelNum]), "BXPT_Rave_EnableContext");
    TestError(BXPT_Rave_GetContextRegisters(handles.hCtx[uiChannelNum], &CtxMap[uiChannelNum]),
        "BXPT_Rave_GetContextRegisters");

	if(ui16EnhPid != 0x1FFF)
	{
	    TestError(BXPT_Rave_SetAvConfig(handles.hEnhCtx[uiChannelNum], &AvCtxCfg), "BXPT_Rave_SetAvConfig");
	    TestError(BXPT_Rave_EnableContext(handles.hEnhCtx[uiChannelNum]), "BXPT_Rave_EnableContext");
	    TestError(BXPT_Rave_GetContextRegisters(handles.hEnhCtx[uiChannelNum], &EnhCtxMap[uiChannelNum]),
	        "BXPT_Rave_GetContextRegisters");
	}

    if(handles.hPcr[uiChannelNum] == NULL)
    {
        BXPT_PCR_DefSettings      pcrSettings;
        BXPT_PcrOffset_Defaults   pcrOffsetDefaults;

        TestError(BXPT_PCR_GetChannelDefSettings(handles.hXpt, uiChannelNum, &pcrSettings),
                   "BXPT_PCR_GetChannelDefSettings");
        TestError(BXPT_PCR_Open(handles.hXpt, uiChannelNum, &pcrSettings, &handles.hPcr[uiChannelNum]),
                   "BXPT_PCR_Open");

        TestError(BXPT_PcrOffset_GetChannelDefSettings(handles.hXpt, uiChannelNum, &pcrOffsetDefaults),
                   "BXPT_PcrOffset_GetChannelDefSettings");
        pcrOffsetDefaults.UsePcrTimeBase = true;
        pcrOffsetDefaults.WhichPcr = uiChannelNum;
        TestError(BXPT_PcrOffset_Open(handles.hXpt, uiChannelNum,  &pcrOffsetDefaults, &handles.hPcrOffset[uiChannelNum]),
                  "BXPT_PcrOffset_Open");
    }

    /* Setup PCR pid */
    TestError(BXPT_PCR_GetStreamPcrConfig(handles.hPcr[uiChannelNum], &pcrCfg),
               "BXPT_PCR_GetStreamPcrConfig");
	
#if (!BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL)
    pcrCfg.eStreamSelect = BXPT_DataSource_eInputBand;
    pcrCfg.Pid = ui16PcrPid;
    if (eXptSrc == BTST_XPT_SRC_Streamer)
        pcrCfg.WhichSource = BFramework_XPT_STREAMER_INPUTBAND;
    else /*  BTST_XPT_SRC_Demodulator */
        pcrCfg.WhichSource = BTST_XPT_DEMOD_INPUTBAND;
#else
    BSTD_UNUSED(ui16PcrPid);
    pcrCfg.PidChannel = uiChannelNum;
#endif
    TestError(BXPT_PCR_SetStreamPcrConfig(handles.hPcr[uiChannelNum], &pcrCfg),
               "BXPT_PCR_SetStreamPcrConfig");

    TestError(BXPT_PcrOffset_GetSettings(handles.hPcrOffset[uiChannelNum], &pcrOffsetSettings),
               "BXPT_PcrOffset_GetSettings");
    pcrOffsetSettings.CountMode = BXPT_PcrOffset_StcCountMode_eMod300;
    pcrOffsetSettings.PidChannelNum = uiChannelNum;
    TestError(BXPT_PcrOffset_SetSettings(handles.hPcrOffset[uiChannelNum], &pcrOffsetSettings),
              "BXPT_PcrOffset_SetSettings");

   if ( 0 != g_pContext->uiNumValidPid2Buf )
   {
      unsigned i;

      BAVC_CdbItbConfig CdbItb = { { 512 * 1024, 4, true }, { 512 * 1024, 4, true }, false };

      for ( i = 0; i < g_pContext->uiNumValidPid2Buf; i++ )
      {
         BXPT_Rave_RecordSettings RecCtxCfg;

         g_pContext->pid2buf[i].uiPIDChannel = g_iXptChNum++;
         BXPT_Rave_AllocContext( g_hRave, BXPT_RaveCx_eRecord, &CdbItb, (BXPT_RaveCx_Handle*) &g_pContext->pid2buf[i].hRaveCtx );
         BXPT_Rave_AddPidChannel( g_pContext->pid2buf[i].hRaveCtx, g_pContext->pid2buf[i].uiPIDChannel, false );

         BXPT_Rave_GetRecordConfig( g_pContext->pid2buf[i].hRaveCtx, &RecCtxCfg );
         RecCtxCfg.OutputFormat = BAVC_StreamType_eTsMpeg;
         RecCtxCfg.UseTimeStamps = false;
         RecCtxCfg.TsInitEn = false;
         RecCtxCfg.StreamIdHi = 0xEF;
         RecCtxCfg.StreamIdLo = 0xBD;
         RecCtxCfg.CdbUpperThreshold = 0x400;
         RecCtxCfg.CdbLowerThreshold = 0x400;
         RecCtxCfg.ItbUpperThreshold = 0x400;
         RecCtxCfg.ItbLowerThreshold = 0x400;
         BXPT_Rave_SetRecordConfig( g_pContext->pid2buf[i].hRaveCtx, &RecCtxCfg );

         /* Setup PID remap */
         if ( g_pContext->pid2buf[i].uiOutputPID != g_pContext->pid2buf[i].uiInputPID )
         {
            BXPT_Spid_ConfigureChannel(
               handles.hXpt,
               g_pContext->pid2buf[i].uiPIDChannel,
               g_pContext->pid2buf[i].uiInputPID,
               BXPT_Spid_eChannelMode_Remap
               );
         }

         BXPT_Rave_EnableContext( g_pContext->pid2buf[i].hRaveCtx );

         if(bPlayback)
         {
            BXPT_ConfigurePidChannel(
               handles.hXpt,
               g_pContext->pid2buf[i].uiPIDChannel,
               g_pContext->pid2buf[i].uiOutputPID,
               BXPT_PB_PARSER(BTST_XVD_PB_CHANNEL_ID_BASE + uiChannelNum)
               );
         }
         else
         {
            BXPT_ConfigurePidChannel(
               handles.hXpt,
               g_pContext->pid2buf[i].uiPIDChannel,
               g_pContext->pid2buf[i].uiInputPID,
               BFramework_XPT_PARSERBAND_0 + uiChannelNum
               );
         }
         BXPT_EnablePidChannel( handles.hXpt, g_pContext->pid2buf[i].uiPIDChannel );
      }
   }

	/* Stop playback if switch back to streamer input */
	Playback_DoStopPlay(handles.hPb[uiChannelNum]);
	Close_PlaybackChan(handles.hPb[uiChannelNum]);

	if(bPlayback)
	{
		Open_PlaybackChan(handles.hXpt, handles.hPb[uiChannelNum], uiChannelNum);
		if(fn == NULL)
			Playback_DoStartPlay(handles.hPb[uiChannelNum], "videos/spiderman.mpg", BXPT_PB_SYNC_MPEG_BLIND, false);
		else
			Playback_DoStartPlay(handles.hPb[uiChannelNum], fn, BXPT_PB_SYNC_MPEG_BLIND, (eStreamType == BAVC_StreamType_eEs) ? true : false);
	}

Done:
    return BERR_SUCCESS;
}

BERR_Code StopXPT_Pcr(
    int                       iWhichPcr )
{
    BERR_Code     err = BERR_SUCCESS;

    TEST_CLOSE_HANDLE(BXPT_PcrOffset_Close, handles.hPcrOffset[iWhichPcr]);
    TEST_CLOSE_HANDLE(BXPT_PCR_Close, handles.hPcr[iWhichPcr]);
    return err;
}

/***************************************************************************
 ***************************************************************************/
BERR_Code ShutdownXPT( void )
{
    BERR_Code     err = BERR_SUCCESS;
    unsigned int  i;

    for ( i = 0; i < g_pContext->uiNumValidPid2Buf; i++ )
    {
       if ( NULL != g_pContext->pid2buf[i].hRaveCtx )
       {
          BXPT_Rave_RemovePidChannel( g_pContext->pid2buf[i].hRaveCtx, g_pContext->pid2buf[i].uiPIDChannel );
          BXPT_Rave_DisableContext( g_pContext->pid2buf[i].hRaveCtx );
          BXPT_Rave_FreeContext( g_pContext->pid2buf[i].hRaveCtx );
          g_pContext->pid2buf[i].hRaveCtx = NULL;
       }
    }

    /* Rave specific shutdown; assume each RAVE only has one pid channel; */
    for (i = 0; i < (unsigned int)BTST_P_MAX_CHANNELS; i++)
    {
        if(handles.hCtx[i] != NULL)
        {
            TestError(BXPT_Rave_RemovePidChannel(handles.hCtx[i], handles.iRavePidChan[i]), "BXPT_Rave_RemovePidChannel");
            handles.iRavePidChan[i] = -1;
            TestError(BXPT_Rave_DisableContext(handles.hCtx[i]), "BXPT_Rave_DisableContext");
            TestError(BXPT_Rave_FlushContext(handles.hCtx[i]), "BXPT_Rave_FlushContext");

            TEST_CLOSE_HANDLE(BXPT_Rave_FreeContext, handles.hCtx[i]);

            TEST_CLOSE_HANDLE(BXPT_PcrOffset_Close, handles.hPcrOffset[i]);
            TEST_CLOSE_HANDLE(BXPT_PCR_Close, handles.hPcr[i]);
        }

        if(handles.hEnhCtx[i] != NULL)
        {
            TestError(BXPT_Rave_RemovePidChannel(handles.hEnhCtx[i], handles.iEnhRavePidChan[i]), "BXPT_Rave_RemovePidChannel");
            handles.iEnhRavePidChan[i] = -1;
            TestError(BXPT_Rave_DisableContext(handles.hEnhCtx[i]), "BXPT_Rave_DisableContext");
            TestError(BXPT_Rave_FlushContext(handles.hEnhCtx[i]), "BXPT_Rave_FlushContext");

            TEST_CLOSE_HANDLE(BXPT_Rave_FreeContext, handles.hEnhCtx[i]);
        }
    }

    for (i = 0; i < (unsigned int)MAX_PLAYBACKS; i++)
    {
        Playback_DoStopPlay(handles.hPb[i]);
        Close_PlaybackChan(handles.hPb[i]);
        Playback_Shutdown(handles.hPb[i]);
    }

    TEST_CLOSE_HANDLE(BXPT_Rave_CloseChannel, handles.hRave);
    TEST_CLOSE_HANDLE(BXPT_Close, handles.hXpt);

#if BTST_P_SUPPORT_HSM
    /* Free HSM */
    TestError(BHSM_Channel_Close(handles.hHsmCh[BHSM_HwModule_eCmdInterface1]),
        "BHSM_Channel_Close BHSM_HwModule_eCmdInterface1");

    TestError(BHSM_Channel_Close(handles.hHsmCh[BHSM_HwModule_eCmdInterface2]),
        "BHSM_Channel_Close BHSM_HwModule_eCmdInterface1");

    TestError(BHSM_Close(handles.hHsm), "BHSM_Close");
#endif


Done:
    printf("ShutdownXPT\n");
    return err;
}


/***************************************************************************
 ***************************************************************************/
void DumpMap( BREG_Handle hReg, BAVC_XptContextMap *CtxMap )
{
    printf( "CDB_Read (0x%lX) is 0x%lX\n",
                    (unsigned long) CtxMap->CDB_Read,
                    (unsigned long) BREG_Read32( hReg, CtxMap->CDB_Read ));
    printf( "CDB_Base (0x%lX) is 0x%lX\n",
                    (unsigned long) CtxMap->CDB_Base,
                    (unsigned long) BREG_Read32( hReg, CtxMap->CDB_Base ));
    printf( "CDB_Wrap (0x%lX) is 0x%lX\n",
                    (unsigned long) CtxMap->CDB_Wrap,
                    (unsigned long) BREG_Read32( hReg, CtxMap->CDB_Wrap ));
    printf( "CDB_Valid (0x%lX) is 0x%lX\n",
                    (unsigned long) CtxMap->CDB_Valid,
                    (unsigned long) BREG_Read32( hReg, CtxMap->CDB_Valid ));

    printf( "ITB_Read (0x%lX) is 0x%lX\n",
                    (unsigned long) CtxMap->ITB_Read,
                    (unsigned long) BREG_Read32( hReg, CtxMap->ITB_Read ));
    printf( "ITB_Base (0x%lX) is 0x%lX\n",
                    (unsigned long) CtxMap->ITB_Base,
                    (unsigned long) BREG_Read32( hReg, CtxMap->ITB_Base ));
    printf( "ITB_Wrap (0x%lX) is 0x%lX\n",
                    (unsigned long) CtxMap->ITB_Wrap,
                    (unsigned long) BREG_Read32( hReg, CtxMap->ITB_Wrap ));
    printf( "ITB_Valid (0x%lX) is 0x%lX\n",
                    (unsigned long) CtxMap->ITB_Valid,
                    (unsigned long) BREG_Read32( hReg, CtxMap->ITB_Valid ));
}

void* GetXPTPcrOffset( int index )
{
   return handles.hPcrOffset[index];
}

#if BTST_P_SUPPORT_TUNER
#define TS_PAT_PID 0x0000
#define TS_NIT_PID 0x0010

void BTST_P_Xpt_PSI(BTST_P_Context  *pContext )
{
    unsigned int inputBand, parserBand, bank, filterNum;
    BXPT_Filter filter;
    BXPT_PsiMessageSettings psiMsgSettings;
    bool override=false;
    BXPT_PidPsiConfig psiConfig;
    void *msgBuf_cached;
    uint8_t *msgBuf = NULL;
    uint32_t msgSize;
    bool moreData;
    int i, j;
    BERR_Code err;

    if (pContext->eXptSrc == BTST_XPT_SRC_Demodulator)
        inputBand = BTST_XPT_DEMOD_INPUTBAND;
    else
        inputBand = BFramework_XPT_STREAMER_INPUTBAND;

    parserBand = BFramework_XPT_PARSERBAND_0;

    if (pContext->eXptSrc != handles.eCurrentXptSrc)
    {
        TestError(BXPT_SetParserDataSource(handles.hXpt, parserBand,
            BXPT_DataSource_eInputBand, inputBand),"BXPT_SetParserDataSource");
        TestError(BXPT_SetParserEnable(handles.hXpt, parserBand, true), "BXPT_SetParserEnable");

        handles.eCurrentXptSrc = pContext->eXptSrc;
    }

    printf("paserBand = %d, inputBand = %d\n", parserBand, inputBand);
    /* parse PAT */
    TestError(BXPT_ConfigurePidChannel(handles.hXpt, 0, TS_PAT_PID, parserBand),
                   "BXPT_ConfigurePidChannel");

    TestError(BXPT_EnablePidChannel(handles.hXpt, 0), "BXPT_EnablePidChannel");


    /* Allocate message buffer */

    TestError(BXPT_SetPidChannelBuffer(handles.hXpt, 0, (void *)msgBuf, BXPT_MessageBufferSize_e4kB), "BXPT_SetPidChannelBuffer");
    bank = 0;
    TestError(BXPT_AllocPSIFilter(handles.hXpt, bank, &filterNum), "BXPT_AllocPSIFilter");

    BKNI_Memset(filter.Mask, 0xFF, sizeof(filter.Mask));
    BKNI_Memset(filter.Exclusion, 0xFF, sizeof(filter.Exclusion));
    BKNI_Memset(filter.Coeficient, 0x00, sizeof(filter.Coeficient));

    TestError(BXPT_SetFilter(handles.hXpt, bank, filterNum, &filter), "BXPT_SetFilter");
    TestError(BXPT_GetPidChannelPsiSettings( handles.hXpt, 0, &override, &psiConfig ), "BXPT_GetPidChannelPsiSettings");

    psiConfig.PsfCrcDis = true;
    override = true;

    TestError(BXPT_SetPidChannelPsiSettings( handles.hXpt, 0, override, &psiConfig ), "BXPT_SetPidChannelPsiSettings");

    psiMsgSettings.Pid = TS_PAT_PID;
    psiMsgSettings.Band = inputBand;
    psiMsgSettings.CrcDisable = true;
    psiMsgSettings.Bank = 0;
    psiMsgSettings.FilterEnableMask = 0xff;

    TestError(BXPT_StartPsiMessageCapture(handles.hXpt, 0, &psiMsgSettings), "BXPT_StartPsiMessageCapture");

    BKNI_Sleep(5000);

    BXPT_CheckBuffer(handles.hXpt, 0, &msgBuf, &msgSize, &moreData );

    BMEM_Heap_ConvertAddressToCached(ghMem, msgBuf, &msgBuf_cached);

    BMEM_Heap_FlushCache(ghMem, msgBuf_cached, msgSize);


    printf("Captured PSI buffer, msgBuf = 0x%x, msgBuf_cached = 0x%x: \n", (int) msgBuf, (int) msgBuf_cached);

    for (i = 0; i < (int)msgSize; i+=8)
    {
        for (j = 0; j < 8; j++)
        {
            printf("%02x ", *(((unsigned char *)msgBuf_cached)+i+j));
        }
        printf("\n");
    }

Done:
    return;
}

#endif
#endif

/* end of file */
