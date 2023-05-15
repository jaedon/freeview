/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_anr_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/57 $
 * $brcm_Date: 7/17/12 2:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_anr_priv.h $
 * 
 * Hydra_Software_Devel/57   7/17/12 2:06p vanessah
 * SW7435-265: remove duplicate ANR register programming
 * 
 * Hydra_Software_Devel/56   11/2/11 7:10p vanessah
 * SW7425-1115: mcvp code cleanup
 * 
 * Hydra_Software_Devel/55   3/15/11 4:08p tdo
 * SW7425-104: Add logic to drop frame when repeat
 * 
 * Hydra_Software_Devel/54   2/17/11 4:14p tdo
 * SW7425-104: Update the ANR software algorithm for optimal use of new
 * ANR hardware.  Add new agorithm.
 *
 * Hydra_Software_Devel/53   2/16/11 6:05p tdo
 * SW7425-104: Update the ANR software algorithm for optimal use of new
 * ANR hardware.  Refactor.
 *
 * Hydra_Software_Devel/52   12/23/10 5:12p tdo
 * SW7422-148: Refactor MCVP to address chips w/wo MCTF
 *
 * Hydra_Software_Devel/51   12/8/10 5:44p tdo
 * SW7422-36: Correctly program MCVP 3D mode based on source and display
 * orientation
 *
 * Hydra_Software_Devel/50   12/3/10 4:42p tdo
 * SW7422-36: Handle "left only" method for BVN internal data path
 *
 * Hydra_Software_Devel/49   8/26/10 2:40p vanessah
 * SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
 * mode
 *
 * Hydra_Software_Devel/SW7420-965/1   8/20/10 1:25p vanessah
 * SW7420-965:  MCVP game mode support
 *
 * Hydra_Software_Devel/48   6/18/10 4:18p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/47   5/7/10 7:07p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/46   4/7/10 11:23a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/45   4/5/10 3:57p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/44   12/9/09 3:52p pntruong
 * SW3556-880: Need to re-evaluate maaf filter on input change that may
 * not trigger bvn reconfiguration.
 *
 * Hydra_Software_Devel/43   6/4/09 1:59p yuxiaz
 * PR55330, PR55323, PR53197, PR55158, PR55604: Need to use separate CAP
 * and VFD buffers in ANR when DCX is enabled.
 *
 * Hydra_Software_Devel/42   3/17/09 12:58a tdo
 * PR45785, PR45789: Fix compiling errors
 *
 * Hydra_Software_Devel/41   3/16/09 10:44p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/7   3/4/09 6:05p tdo
 * PR45785, PR45789: 7420 ANR is 8-bit
 *
 * Hydra_Software_Devel/7420_mcvp/6   2/26/09 11:17p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/40   2/26/09 6:09p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/7420_mcvp/5   2/24/09 2:01p syang
 * PR 51795:  merge from main branch on 2/24/09
 *
 * Hydra_Software_Devel/39   2/24/09 1:44p syang
 * PR 51795:  separate AlphLowThd value into AlphLowThdMC and
 * AlphLowThdNMC
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/18/09 7:03p syang
 * PR 45785, PR 45789: fix 3556 compile error
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/11/09 11:31a tdo
 * PR 45785, PR 45789: merge from main branch on 2/11/09
 *
 * Hydra_Software_Devel/38   2/10/09 11:25a syang
 * PR 51795: use macro, rather than hard coded 16, for the number of
 * NOISE_LVL_HISTORY to average
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/3/09 7:21p syang
 * PR 45785, PR 45789: merge from main branch on 2/3/09
 *
 * Hydra_Software_Devel/37   2/3/09 4:11p syang
 * PR 51795: change to use averaged snr value
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:29p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/36   1/19/09 8:22p pntruong
 * PR51300: Arrange reading of anr status to not conflict with reset rul.
 *
 * Hydra_Software_Devel/35   1/9/09 2:25p syang
 * PR 50758: call Anr_BuildRul_SrcInit_isr if anr internal PxlFmt changes,
 * rather than according to pPicComRulInfo->PicDirty
 *
 * Hydra_Software_Devel/34   1/8/09 4:10p syang
 * PR 49579, PR 50867: 1). set MC_NM_ K1 to 0 for bypass; 2). set diff
 * AND_0.EDGE_TH and  AND_0.CONTENT_TH for HD and SD; 3). use diff K
 * table for RF, SD, ED, and HD
 *
 * Hydra_Software_Devel/33   11/20/08 3:40p syang
 * PR 49516: 1) Improve the SNR detection algorithm; 2).  Update
 * BVDC_P_AnrKValue s_ulAnrKValue[] table;  as requested
 *
 * Hydra_Software_Devel/32   11/14/08 4:19p syang
 * PR 48839: 1). update as PR requested; 2).  handle RUL loss (without
 * exec)
 *
 * Hydra_Software_Devel/31   10/8/08 12:17p pntruong
 * PR46735: B0: Need SECAM Dr/Db adjustment implemented.
 *
 * Hydra_Software_Devel/30   10/3/08 9:22a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/29   10/1/08 3:56p hongtaoz
 * PR47490, PR46826: disable dithering when ANR is enabled; 3548B0 moved
 * MAD/ANR dither to MAD_SIOB_0 block;
 *
 * Hydra_Software_Devel/28   9/30/08 6:30p hongtaoz
 * PR46738: fixed MAD/ANR memory saving mode;
 *
 * Hydra_Software_Devel/27   9/30/08 4:55p yuxiaz
 * PR46305, PR46307: Video TestFeature1 support: Fix throughput
 * calculation, combine MAD and ANR set TestFeature1 function.
 *
 * Hydra_Software_Devel/26   9/26/08 3:22p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/25   9/24/08 10:03a syang
 * PR 46742, PR 46739:  add maaf, add drain in anr when mad is not used
 *
 * Hydra_Software_Devel/24   9/23/08 1:41p syang
 * PR 46737:  Remove dithering feature from MCTF for 3548 B0
 *
 * Hydra_Software_Devel/23   9/17/08 8:43p pntruong
 * PR47013: B0: Initial vdc support.
 *
 * Hydra_Software_Devel/22   8/13/08 7:08p pntruong
 * PR45658: [PQ] Need to relax the ANR for 1080p@60Hz mode.
 *
 * Hydra_Software_Devel/21   8/12/08 3:32p syang
 * PR 45560: clean-up history effect for both top and bottom field after
 * AND detects a big snr change (likely content change)
 *
 * Hydra_Software_Devel/20   3/19/08 7:34p hongtaoz
 * PR39154, PR39202, PR39203: combined ANR and MAD in memory saving mode
 * on 3548; implemented MAD specific change; updated SCL precision for
 * 3548;
 *
 * Hydra_Software_Devel/19   2/29/08 4:17p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/18   2/22/08 2:31p yuxiaz
 * PR39155: Added de-ringing filters in BVN.
 *
 * Hydra_Software_Devel/17   11/5/07 8:08p pntruong
 * PR36462: PR33011:If CVBS signal doesn't contain burst signal, need S/W
 * assume a default format
 * PR36563:for RF channel, mute the video 20 vysnc when video format
 * change. This is to fix the transition that cause by RF auto detection:
 * PAL-M/PAL-Nc/NTSC
 * PR33797:to fix the transtion and anr hanging up during the switch
 * channel
 * PR36462: decrease the delay for VCR detection
 *
 * Hydra_Software_Devel/16   10/30/07 2:30p rpan
 * PR34798: Removed support for obsoleted versions of ANR.
 *
 * Hydra_Software_Devel/15   7/17/07 5:32p syang
 * PR 33051: avoid hard_start if not necessary
 *
 * Hydra_Software_Devel/14   7/11/07 7:44p syang
 * PR 31944: dynamically adjust alpha_low_thd
 *
 * Hydra_Software_Devel/13   6/22/07 5:13p syang
 * PR 32384: change to use top level HD_ANR_ENABLE_BYPASS for bypass
 *
 * Hydra_Software_Devel/12   6/20/07 5:36p syang
 * PR 31944: 1). add a new user defined struct ptr to ANR configure 2).
 * update SNR calculation code to match Jorge's script
 *
 * Hydra_Software_Devel/11   6/13/07 9:44a syang
 * PR 31944: put ANR K value table inro seperate file; change to use
 * SnDbAdj from 5 level adj
 *
 * Hydra_Software_Devel/10   6/4/07 5:56p syang
 * PR 28550: 1). read AND reg every vsync; 2). user S/N adj change from 2
 * to 3; 3). improve MSG
 *
 * Hydra_Software_Devel/9   5/30/07 5:01p syang
 * PR 28550:  add tune for HD
 *
 * Hydra_Software_Devel/8   5/10/07 9:57a syang
 * PR 28895, PR 28898, PR 29846 , PR 28550: 1). add frame for C0 ANR PI
 * support; 2). move MAD/ANR buf alloc/free to MAD/ANR; 3). remove
 * drainVnet state
 *
 * Hydra_Software_Devel/7   2/28/07 4:48p syang
 * PR 22569: 1). tune anr dynamic seting;  2). add anr demo mode; 3). use
 * hResource in SubRul
 *
 * Hydra_Software_Devel/6   2/23/07 6:27p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/5   2/16/07 3:39p syang
 * PR 22569: 1). added adaptive filter setting; 2). added filter level
 *
 * Hydra_Software_Devel/4   2/1/07 12:12p syang
 * PR 22569: update BuildRul_isr to use pSrcOut
 *
 * Hydra_Software_Devel/3   1/5/07 4:25p syang
 * PR 22569: almost completed coding for new vnet build/shutdown algorithm
 * (by each sub-modules)
 *
 * Hydra_Software_Devel/2   12/22/06 11:49a syang
 * PR 22569:  1st time have simple_vdc compiled successfully
 *
 * Hydra_Software_Devel/1   12/21/06 3:09p syang
 * PR 22569: init version
 *
 ***************************************************************************/
#ifndef BVDC_ANR_PRIV_H__
#define BVDC_ANR_PRIV_H__

#include "bavc.h"
#include "breg_mem.h"      /* Chip register access (memory mapped). */
#include "bvdc_common_priv.h"
#include "bvdc_bufferheap_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_scaler_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * {private}
 *
 * Anr Sub-module Overview:
 *
 */

/*-------------------------------------------------------------------------
 * macro used by anr sub-module
 */
#define BVDC_P_ANR_BUFFER_COUNT                (1)
#define BVDC_P_ANR_NUM_ACCUM                   (1)
#define BVDC_P_SRC_INVALID                     (0xffffffff)

/* ANR Versions */
#define BVDC_P_ANR_VER_0                       (0) /* 3563A0, 3563B0 */
#define BVDC_P_ANR_VER_1                       (1) /* 3563C0, 3563D0*/
#define BVDC_P_ANR_VER_2                       (2) /* 3548Ax */
/* 3548 B0:
 * Removed HD_ANR_MCTF_0_CONT_x_PITCH
 * Removed HD_ANR_MCTF_0_DITHER_*
 * Added HD_ANR_MCTF_0_CONT_0_MAAF_* */
#define BVDC_P_ANR_VER_3                       (3) /* 3548B0 */

/* 7420: 8-bit MTCF/AND */
#define BVDC_P_MANR_VER_1                      (1) /* 7420 */
#define BVDC_P_MANR_VER_2                      (2) /* 7422, 7425 */
#define BVDC_P_MANR_VER_3                      (3) /* 7435 */

#define BVDC_P_SUPPORT_ANR_MAAF                \
    ((BVDC_P_SUPPORT_ANR_VER >= BVDC_P_ANR_VER_3) && (BVDC_P_SUPPORT_ANR))

/***************************************************************************
 * K values look-up
 ***************************************************************************/
/*-------------------------------------------------------------------------
 * K value struct
 */
typedef struct
{
	uint16_t                           ulAlphLowThdMC;  /*MC_ALPHA_LOW_THRESHOLD*/
	uint16_t                           ulAlphLowThdNMC; /* NMC_ALPHA_LOW_THRESHOLD */
	uint16_t                           ulMcK0;       /* K0 for MC filter alpha */
	uint16_t                           ulMcK1;       /* K1 for MC filter alpha */
	uint16_t                           ulNonMcK0;    /* K0 for non-MC filter alpha */
	uint16_t                           ulNonMcK1;    /* K1 for non-MC filter alpha */
	uint16_t                           ulMcK0_CH;    /* K0 for chroma MC filter alpha */
	uint16_t                           ulMcK1_CH;    /* K1 for chroma MC filter alpha */
	uint16_t                           ulNonMcK0_CH; /* K0 for chroma non-MC filter alpha */
	uint16_t                           ulNonMcK1_CH; /* K1 for chroma non-MC filter alpha */
	uint16_t                           ulFinalK0;    /* K0 for blending results from MC and non-MC filter */
	uint16_t                           ulFinalK1;    /* K1 for blending results from MC and non-MC filter */

	/* an explicit boolean to switch MCTF filter ON/OFF; */
	bool                               bBypassFilter;

} BVDC_P_AnrKValue;


/*-------------------------------------------------------------------------
 * ANR status
 */
typedef struct
{
	bool                               bEnaError;  /* set when get re-enable error */

} BVDC_P_AnrStatus;


/****************************************************************************
 * Anr dirty bits to makr RUL building and executing dirty.
 */
typedef union
{
	struct
	{
		uint32_t                           bSize           : 1;
		uint32_t                           bPxlFmt         : 1;
		uint32_t                           bPrevBypass     : 1;
	} stBits;

	uint32_t aulInts [BVDC_P_DIRTY_INT_ARRAY_SIZE];
} BVDC_P_AnrDirtyBits;

#define BVDC_P_NOISE_LVL_AVG_SIZE      16
#define BVDC_P_NOISE_NUM_AVG_SIZE      8
/*-------------------------------------------------------------------------
 * anr main context
 */
typedef struct BVDC_P_AnrContext
{
	BDBG_OBJECT(BVDC_ANR)

	/* anr Id */
	BVDC_P_AnrId                       eId;
	uint32_t                           ulRegOffset;

	/* static info from creating */
	BREG_Handle                        hRegister;

	/* from acquireConnect */
	BVDC_Heap_Handle                   hHeap;
	BVDC_P_WindowId                    eWinId;

	/* buffers */
	BVDC_P_HeapNodePtr                 apHeapNode[4 * BVDC_P_ANR_BUFFER_COUNT];
	uint32_t                           ulHSize;
	uint32_t                           ulVSize;
	uint32_t                           ulAndThdScl;  /* due to diff size for diff video fmt */
	int32_t                            lSnDb;
	bool                               bMemSaving;
	bool                               bEnableMaaf;  /* enable for SECAM input only */
	bool                               bBypassFilter;
	BPXL_Format                        ePxlFmt;
	BVDC_P_TestFeature1_Settings       stTestFeature1;

	/* for user config and src video info */
	BVDC_P_Source_InfoPtr              pCurSrcInfo;
	const BVDC_Anr_Settings           *pAnrSetting;
	BVDC_SplitScreenMode               eDemoMode;

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext               SubRul;

	/* vsync cntr for cleaning up history effect after big anr change */
	uint32_t                           ulCleanCntr;

	/* for averaged NOISE_LEVEL_HISTORY */
	uint32_t                           aulNoiseLevelHistory[BVDC_P_NOISE_LVL_AVG_SIZE]; /* last 16 noiseLevelHistory */
	uint32_t                           ulIndex; /* current index to the above array */
	uint32_t                           ulNoiseLevelHistorySum;  /* sum of the above array */

	/* set as change is marked, clear after built into RUL */
	BVDC_P_AnrDirtyBits                stSwDirty;

	/* set after built into RUL, clear after executed */
	BVDC_P_AnrDirtyBits                stHwDirty;

	/* 422 to 444 up sampler setting */
	BVDC_422To444UpSampler             stUpSampler;
	/* 444 to 422 down sampler setting */
	BVDC_444To422DnSampler             stDnSampler;

	/* PR46735: B0: Need SECAM Dr/Db adjustment implemented */
	uint32_t                           ulAutoCtrlReg;

	/* for convenience of setting trick mode */
	uint32_t                           ulTopCtrlReg;

	/* MCTF BvbStatus */
	uint32_t                           ulBvbStatus;

	BVDC_P_BufferHeapId                eBufHeapId;
	bool                               bSplitBuf;

	/* Additional parameters for updated algorithm */
	uint32_t                           ulNoiseNumHistorySum;
	uint32_t                           aulNoiseNumHistory[BVDC_P_NOISE_NUM_AVG_SIZE];
	uint32_t                           ulIndex2;
	uint8_t                            uPreviousLowNoise;
} BVDC_P_AnrContext;


/***************************************************************************
 * private functions
 ***************************************************************************/
#if (BVDC_P_SUPPORT_ANR)
#define BVDC_P_Anr_MuxAddr(hAnr)           (BCHP_VNET_F_ANR_0_SRC + (hAnr)->eId * sizeof(uint32_t))
#define BVDC_P_Anr_PostMuxValue(hAnr)      (BCHP_VNET_B_CAP_0_SRC_SOURCE_ANR_0 + (hAnr)->eId)
#else
#define BVDC_P_Anr_MuxAddr(hAnr)           (0)
#define BVDC_P_Anr_PostMuxValue(hAnr)      (0)
#endif

#define BVDC_P_Anr_SetUsrSettingPtr(hAnr, pAnrSetting) \
    (hAnr)->pAnrSetting = (pAnrSetting)
#define BVDC_P_Anr_SetSrcInfo(hAnr, pSrcInfo) \
    (hAnr)->pCurSrcInfo = (pSrcInfo)

#define BVDC_P_Anr_SetVnet_isr(hAnr, ulSrcMuxValue, eVnetPatchMode) \
   BVDC_P_SubRul_SetVnet_isr(&((hAnr)->SubRul), ulSrcMuxValue, eVnetPatchMode)
#define BVDC_P_Anr_UnsetVnet_isr(hAnr) \
   BVDC_P_SubRul_UnsetVnet_isr(&((hAnr)->SubRul))

#define BVDC_P_Anr_SetBufPxlFmt_isr(hAnr, eNewPxlFmt) \
   if ((hAnr)->ePxlFmt != (eNewPxlFmt)) { \
	   (hAnr)->ePxlFmt = (eNewPxlFmt); \
	   (hAnr)->stSwDirty.stBits.bPxlFmt = BVDC_P_DIRTY; \
   }


#define BVDC_P_Anr_SetBufNodes_isr(hAnr, ppHeapNode, bShareCap) \
   do { \
	   int ii; \
       (hAnr)->bMemSaving = (bShareCap); \
	   for (ii=0; ii<(int)(2 * BVDC_P_ANR_BUFFER_COUNT); ii++) \
		   (hAnr)->apHeapNode[ii] = (ppHeapNode)[ii]; \
   } while (0)

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_Create
 *
 * called by BVDC_Open only
 */
BERR_Code BVDC_P_Anr_Create
	( BVDC_P_Anr_Handle *              phAnr,
	  BVDC_P_AnrId                     eAnrId,
	  BREG_Handle                      hRegister,
	  BVDC_P_Resource_Handle           hResource );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_Destroy
 *
 * called by BVDC_Close only
 */
BERR_Code BVDC_P_Anr_Destroy
	( BVDC_P_Anr_Handle                hAnr );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_AcquireConnect
 *
 * It is called by BVDC_Window_Validate after changing from diable anr to
 * enable anr.
 */
BERR_Code BVDC_P_Anr_AcquireConnect
	( BVDC_P_Anr_Handle                hAnr,
	  BVDC_Heap_Handle                 hHeap,
	  BVDC_P_WindowId                  eWinId,
	  const BVDC_Anr_Settings         *pAnrSetting,
	  BVDC_P_Source_InfoPtr            pCurSrcInfo);

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_ReleaseConnect_isr
 *
 * It is called after window decided that anr is no-longer used by HW in its
 * vnet mode (i.e. it is really shut down and teared off from vnet).
 */
BERR_Code BVDC_P_Anr_ReleaseConnect_isr
	( BVDC_P_Anr_Handle               *phAnr );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_SetVnetAllocBuf_isr
 *
 * Called by BVDC_P_*_BuildRul_isr to setup for joinning into vnet (including
 * optionally acquiring loop-back) and allocate buffers
 */
void BVDC_P_Anr_SetVnetAllocBuf_isr
	( BVDC_P_Anr_Handle                hAnr,
	  uint32_t                         ulSrcMuxValue,
	  BVDC_P_VnetPatch                 eVnetPatchMode,
	  BVDC_P_BufferHeapId              eBufHeapId,
	  uint32_t                         ulBufCnt,
	  bool                             bMemSaving );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_UnsetVnetFreeBuf_isr
 *
 * called by BVDC_P_Window_UnsetWriter(Reader)Vnet_isr to to release the
 * potentially used loop-back, and free buffers
 */
void BVDC_P_Anr_UnsetVnetFreeBuf_isr
	( BVDC_P_Anr_Handle                hAnr );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_SetDemoMode
 *
 * called by BVDC_Window_ApplyChanges to set anr demo mode
 */
BERR_Code BVDC_P_Anr_SetDemoMode_isr
	( BVDC_P_Anr_Handle                hAnr,
	  BVDC_SplitScreenMode             eDemoMode );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_BuildRul_isr
 *
 * called by BVDC_Window_BuildRul_isr at every src or vec vsync (depending on
 * whether reader side or writer side is using this module)
 *
 * Input:
 *    eVnetState - reader or writer window/vnet state
 *    pPicComRulInfo - the PicComRulInfo that is the shared Picture info by
 *      all sub-modules when they build rul.
 */
void BVDC_P_Anr_BuildRul_isr(
	BVDC_P_Anr_Handle                  hAnr,
	BVDC_P_ListInfo                   *pList,
	BVDC_P_State                       eVnetState,
	BVDC_P_PictureNode                *pPicture );

/*-------------------------------------------------------------------------
 * look up K value struct
 */
const BVDC_P_AnrKValue * BVDC_P_Anr_GetKValue_isr(
	uint32_t                           ulSnrDb,
	BVDC_P_CtInput                     eCtInputType,
	const BFMT_VideoInfo              *pFmtInfo,
	void                              *pvUserInfo );

/*-------------------------------------------------------------------------
 *
 */
void BVDC_P_Anr_BuildRul_SrcInit_isr(
	BVDC_P_AnrContext             *pAnr,
	BVDC_P_ListInfo               *pList,
	BVDC_P_PictureNode            *pPicture );

/*-------------------------------------------------------------------------
 *
 */
void BVDC_P_Anr_BuildRul_SetEnable_isr(
	BVDC_P_AnrContext             *pAnr,
	BVDC_P_PictureNode            *pPicture,
	BVDC_P_ListInfo               *pList,
	bool                           bEnable,
	BAVC_Polarity                  eSrcNxtFldId,
	bool                           bRepeat,
	bool                           bInit );

/*-------------------------------------------------------------------------
 *
 */
void BVDC_P_Anr_BuildRul_StatisRead_isr
	( BVDC_P_AnrContext             *pAnr,
	  BVDC_P_ListInfo               *pList,
	  bool                           bInit);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_ANR_PRIV_H__ */
/* End of file. */
