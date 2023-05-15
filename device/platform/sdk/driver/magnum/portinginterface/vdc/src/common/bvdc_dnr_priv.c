/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_dnr_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/80 $
 * $brcm_Date: 6/26/12 5:02p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_dnr_priv.c $
 * 
 * Hydra_Software_Devel/80   6/26/12 5:02p vanessah
 * SW7435-249: get rid of scl/dnr back-to-back enable/disable
 * 
 * Hydra_Software_Devel/79   6/15/12 10:33a vanessah
 * SW7435-237: reset vnet channel connections
 * 
 * Hydra_Software_Devel/78   3/9/12 12:11p vanessah
 * SW7435-9:  bringup DNR3
 * 
 * Hydra_Software_Devel/77   10/20/11 6:50p vanessah
 * SW7425-1572: add DNR-2 support
 * 
 * Hydra_Software_Devel/76   9/2/11 3:55p pntruong
 * SW7425-1165: Updated the number of dnr/lbox/mdi cores for b0.
 *
 * Hydra_Software_Devel/75   1/31/11 4:54p tdo
 * SW7408-234, SW7408-233: Replace the current filter with new Ramp filter
 * to avoid flickering caused by MNR and Adjust Dithering settings in DCR
 * to fix the artifact with pattern dots
 *
 * Hydra_Software_Devel/74   1/24/11 1:25p pntruong
 * SW7408-228: Enable Dithering in DCR for STB chips.
 *
 * Hydra_Software_Devel/73   11/19/10 5:19p darnstein
 * SW7344-14: DNR hardware module is the same as 7422 after all.
 *
 * Hydra_Software_Devel/72   11/18/10 5:42p darnstein
 * SW7344-14: comparing 7408/7468 with 7344/7346/7231, a single register
 * bitfield has jumped from one register to another.
 *
 * Hydra_Software_Devel/71   9/30/10 4:31p tdo
 * SW7422-50: VDC: Get DNR_H working in 7422
 *
 * Hydra_Software_Devel/70   6/18/10 4:19p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/69   5/7/10 7:09p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/68   4/7/10 11:29a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/67   4/5/10 4:04p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/66   3/3/10 5:09p tdo
 * SW7420-603: Change formula of MNR_Merge to fix flicker/blurry issue on
 * HD ESPN golf stream
 *
 * Hydra_Software_Devel/65   2/12/10 4:00p tdo
 * SW7420-578: Need to separate HBNR and VBNR equation in PI
 *
 * Hydra_Software_Devel/64   1/26/10 5:42p yuxiaz
 * SW7405-3839: Remove the workaround for PR38730
 *
 * Hydra_Software_Devel/63   12/23/09 10:12a rpan
 * SW7468-56: Added DNR-H MBNR and VBNR programming.
 *
 * Hydra_Software_Devel/62   12/18/09 3:18p pntruong
 * SW3548-2674: Added feature to customized dnr h/v offset.
 *
 * Hydra_Software_Devel/61   11/19/09 2:02p pntruong
 * SW7408-13: Used more core rev'ing mechanism for ease of portability.
 *
 * Hydra_Software_Devel/60   11/19/09 10:34a rpan
 * SW7468-31: Comply with the VDC version scheme.
 *
 * Hydra_Software_Devel/59   11/18/09 3:05p rpan
 * SW7468-31: Initial work for 7468 vertical block only DNR.
 *
 * Hydra_Software_Devel/58   9/10/09 3:01p tdo
 * SW7335-542, SW7335-543:[PQ] Need to change DCR/BNR/MNR to improve PQ
 * for STB
 *
 * Hydra_Software_Devel/57   9/3/09 10:30p pntruong
 * SW3548-2373: Rolled back PR56964.
 *
 * Hydra_Software_Devel/56   7/21/09 2:17p syang
 * PR 56964: take off the "connect drainer, en and then dis" step  in
 * vnet-drain code
 *
 * Hydra_Software_Devel/55   4/27/09 6:42p syang
 * PR 54447: add back dnr disabling code and add BuildWinId usage
 *
 * Hydra_Software_Devel/54   4/3/09 1:54p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: don't disable DNR when drop off
 * vnet to prevent BVN error on other windows sharing the same DNR;
 *
 * Hydra_Software_Devel/53   11/25/08 11:10a syang
 * PR 49562: add reset to output channel ping-pong buf of sub-modules and
 * loopbacks and free-channels
 *
 * Hydra_Software_Devel/52   11/20/08 9:30a syang
 * PR 48970: clean-up module disable and drian handling so that RUL dump
 * is easy to understand
 *
 * Hydra_Software_Devel/51   10/27/08 5:36p syang
 * PR 46477: back off SetEnable in in BuildRul_isr according to GetOps (it
 * causes window to disapper on 7400 D0)
 *
 * Hydra_Software_Devel/50   10/24/08 6:20p pntruong
 * PR48346: Enable DNR enable mode to handle VDEC short field case.
 *
 * Hydra_Software_Devel/49   10/24/08 4:06p syang
 * PR 46477: in BuildRul_isr only disable in shadow regs  according to
 * GetOps (some SetEnable_isr does HW reset)
 *
 * Hydra_Software_Devel/48   10/24/08 11:04a syang
 * PR 46477: in BuildRul_isr reset enable bit according to GetOps
 *
 * Hydra_Software_Devel/47   10/22/08 1:59p hongtaoz
 * PR48143: only build necessary CAP/DNR/HSCL/SCL RULs when their
 * operational flags are enabled; this fixed SCL/HSCL enable error;
 *
 * Hydra_Software_Devel/46   9/29/08 3:02p tdo
 * PR47349: Always disable DITH in DCR block for 10-bit chips.  Update new
 * DCR config table.
 *
 * Hydra_Software_Devel/45   9/12/08 3:18p tdo
 * PR46745: Use per-picture enable in DNR block for 3548 B0
 *
 * Hydra_Software_Devel/44   6/24/08 6:28p tdo
 * PR44113: Seperate REL and LIMIT for HBNR and VBNR.
 *
 * Hydra_Software_Devel/43   4/23/08 1:23p yuxiaz
 * PR39175: Add 7325 B0 support in VDC.
 *
 * Hydra_Software_Devel/42   2/12/08 7:42p tdo
 * PR38771, PR38777, PR39246: Implement changes for DNR/DCR cores for 3548
 *
 * Hydra_Software_Devel/41   1/16/08 6:50p pntruong
 * PR38778: BVN: Saturation logic error on DCR's dither function.  Include
 * other affected chips.
 *
 * Hydra_Software_Devel/40   1/16/08 3:18p yuxiaz
 * PR38730: Enable software workaround for saturation logic error on DCR's
 * dither function for 7405 and 7325.
 *
 * Hydra_Software_Devel/39   12/12/07 5:40p tdo
 * PR34386: Modify formulas to calculate MNR and BNR to remove trembling
 * issue
 *
 * Hydra_Software_Devel/36   10/10/07 4:10p pntruong
 * PR35910: DNR V_OFFSET Programming Should Follow Source Scanout
 * Polarity.  PR32066: zoneplate pattern cause video flicking/beating
 * when either BNR or MNR is turned on.
 *
 * Hydra_Software_Devel/35   10/1/07 10:59a pntruong
 * PR34153: Improved pc-streaky noise settings from system group.
 *
 * Hydra_Software_Devel/Gorgon_temp_PR34153_20070930/1   9/30/07 11:25a gorgon
 * PR34153:fix PC input streaky noise. The change is based on the scripts
 * sent by Val on 20070928
 *
 * Hydra_Software_Devel/34   9/28/07 2:46p tdo
 * PR33711: Remove global writable data usage
 *
 * Hydra_Software_Devel/33   9/21/07 5:18p pntruong
 * PR34153: Removed build warnings for system without dcr support.
 *
 * Hydra_Software_Devel/32   9/21/07 3:37p pntruong
 * PR35252: BVN: DNR's MNR requires Disabled then Enabled to avoid
 * artifact of blur edge.
 *
 * Hydra_Software_Devel/31   9/10/07 8:55p pntruong
 * PR34153: Adjust dcr/adc/pll setting to reduce pc noise.  Tablelize pll
 * for ease of customize.
 *
 * Hydra_Software_Devel/PR34153/1   8/23/07 12:27a xhuang
 * PR34153:adjust DCR setting to reduce pc noise
 *
 * Hydra_Software_Devel/30   7/19/07 2:38p tdo
 * PR33034: Modify DNR API to extend MNR/BNR/DCR filter level
 *
 * Hydra_Software_Devel/29   6/27/07 12:33p tdo
 * PR32489: Add support for custom adjust DNR
 *
 * Hydra_Software_Devel/28   6/20/07 12:48p pntruong
 * PR28891, PR29658: Updated "todo comment" to prevent confusion.
 *
 * Hydra_Software_Devel/27   6/11/07 7:26p tdo
 * PR31947: provide API to allow user setting the Qp value for DNR
 *
 * Hydra_Software_Devel/26   6/8/07 11:31a syang
 * PR 31849: separate drain from disable for MAD, ANR and CAP
 *
 * Hydra_Software_Devel/25   6/7/07 2:03p pntruong
 * PR31813: Temporary put back capture reset.  Place trigger relative to
 * percentage of vbi area instead of absolute line numbers.
 *
 * Hydra_Software_Devel/24   6/5/07 5:26p syang
 * PR 31840: take off un-used assert in ReleaseConnect to allow ANR/DNR
 * shared by 2 wins
 *
 * Hydra_Software_Devel/23   5/21/07 3:40p tdo
 * PR28891, PR29658: Bringup DCR for C0
 *
 * Hydra_Software_Devel/22   5/14/07 4:45p tdo
 * PR28891, PR29658: C0: BVN: Add DCR (digital contour removal) support in
 * DNR block
 *
 * Hydra_Software_Devel/21   4/2/07 11:27a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/20   3/5/07 6:39p syang
 * PR 28298: 1). add RegOffset init; add Acquire/Release
 *
 * Hydra_Software_Devel/19   2/23/07 6:28p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/18   2/20/07 9:33p albertl
 * PR23117:  Changed 7440 to use new VNET algorithm and fixed DNR compile
 * errors and warnings.
 *
 * Hydra_Software_Devel/17   2/9/07 5:31p syang
 * PR 27054: don't build rul when op=eReleaseHandle. (why we see some
 * module enabled in shadow register when other modules in vnet are not
 * yet???)
 *
 * Hydra_Software_Devel/16   2/6/07 3:35p hongtaoz
 * PR27666: use original source polarity to program DNR;
 *
 * Hydra_Software_Devel/15   1/22/07 3:14p syang
 * PR 27054: 1). only 1 win build rul, 2). reader's modules don't build
 * rul before video sur enabled; 3). writer's modules don't build rul
 * before (GetReconfiguring_isr==true )
 *
 * Hydra_Software_Devel/14   1/10/07 3:30p yuxiaz
 * PR 26313: Change DNR API to source based API: Remove
 * BVDC_Window_SetDnrFilterLevel and BVDC_Window_GetDnrFilterLevel. Add
 * BVDC_Source_SetDnrFilterLevel, BVDC_Source_GetDnrFilterLevel,
 * BVDC_Source_SetSplitScreenMode and BVDC_Source_GetSplitScreenMode.
 *
 * Hydra_Software_Devel/13   1/10/07 10:58a syang
 * PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
 * dnr, mfd, vfd, cap, ...
 *
 * Hydra_Software_Devel/12   1/4/07 3:17p yuxiaz
 * PR25995: Clean up DNR.
 *
 * Hydra_Software_Devel/11   12/28/06 11:10a hongtaoz
 * PR23213, PR23383: DNR demo size should be set according to input size;
 * DNR enable toggling should not erase the demo mode setting;
 *
 * Hydra_Software_Devel/10   12/18/06 11:38p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   11/22/06 3:51p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/16/06 1:56p hongtaoz
 * PR25668: merge in vdc support for 7403;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 3:53p tdo
 * PR 23213, PR 23383: Using new API for demo mode
 *
 * Hydra_Software_Devel/8   8/30/06 11:25p tdo
 * PR 17994, PR 23383:  Add support for DNR demo mode
 *
 * Hydra_Software_Devel/7   8/1/06 3:11p hongtaoz
 * PR22499: RUL only needs to program selected registers;
 *
 * Hydra_Software_Devel/6   7/31/06 6:54p hongtaoz
 * PR22499: temporary workaround for Qp=0 to demo DCR and to tune up;
 *
 * Hydra_Software_Devel/5   7/31/06 6:10p hongtaoz
 * PR22499: added DCR filter setting as a part of DNR block;
 *
 * Hydra_Software_Devel/4   5/26/06 3:38p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/3   5/19/06 2:13p hongtaoz
 * PR17514: added bypass filter enum for DNR setting; disable extreme
 * filter for 7401A0 due to PR16299; fixed range clamping for DNR
 * settings; added DNR demo mode setting in test api;
 *
 * Hydra_Software_Devel/2   5/9/06 1:35p tdo
 * PR 21170: Add support for 7401 B0
 *
 * Hydra_Software_Devel/1   10/31/05 6:24p hongtaoz
 * PR17514: add DNR support in VDC;
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "brdc.h"

#include "bvdc.h"
#include "bvdc_dnr_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_subrul_priv.h"

#if BVDC_P_SUPPORT_DNR
#include "bchp_mmisc.h"

BDBG_MODULE(BVDC_DNR);
BDBG_OBJECT_ID(BVDC_DNR);

#define BVDC_P_MNR_LIMIT_FILTER_SHIFT             (4)
#define BVDC_P_MNR_LIMIT_ALPHA_SHIFT              (3)

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Dnr_Create
	( BVDC_P_Dnr_Handle            *phDnr,
	  BVDC_P_DnrId                  eDnrId,
	  BVDC_P_Resource_Handle        hResource )
{
	uint32_t ulDnrIdx;
	BVDC_P_DnrContext *pDnr;

	BDBG_ENTER(BVDC_P_Dnr_Create);

	BDBG_ASSERT(phDnr);
	BDBG_ASSERT(eDnrId < BVDC_P_DnrId_eUnknown);

	/* (1) Alloc the context. */
	pDnr = (BVDC_P_DnrContext*)
		(BKNI_Malloc(sizeof(BVDC_P_DnrContext)));
	if(!pDnr)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pDnr, 0x0, sizeof(BVDC_P_DnrContext));
	BDBG_OBJECT_SET(pDnr, BVDC_DNR);

	pDnr->eId              = eDnrId;
	ulDnrIdx = (eDnrId - BVDC_P_DnrId_eDnr0);

	/* DNR reset address */
#if BVDC_P_SUPPORT_NEW_SW_INIT
	pDnr->ulResetRegAddr = BCHP_MMISC_SW_INIT;
	pDnr->ulResetMask    = BCHP_MMISC_SW_INIT_DNR_0_MASK << (ulDnrIdx);
	pDnr->ulVnetResetAddr = BCHP_MMISC_VNET_B_CHANNEL_SW_INIT;
	pDnr->ulVnetResetMask = BCHP_MMISC_VNET_B_CHANNEL_SW_INIT_DNR_0_MASK << (ulDnrIdx);
#else
	pDnr->ulResetRegAddr = BCHP_MMISC_SOFT_RESET;
	pDnr->ulResetMask    = BCHP_MMISC_SOFT_RESET_DNR_0_MASK << (ulDnrIdx);
#if BCHP_MMISC_VNET_B_CHANNEL_RESET
	pDnr->ulVnetResetAddr = BCHP_MMISC_VNET_B_CHANNEL_RESET;
	pDnr->ulVnetResetMask = BCHP_MMISC_VNET_B_CHANNEL_RESET_DNR_0_RESET_MASK << (ulDnrIdx);
#endif
#endif

	pDnr->ulRegOffset = BVDC_P_DNR_GET_REG_OFFSET(eDnrId);

	/* init the SubRul sub-module */
	BVDC_P_SubRul_Init(&(pDnr->SubRul), BVDC_P_Dnr_MuxAddr(pDnr),
		BVDC_P_Dnr_PostMuxValue(pDnr), BVDC_P_DrainMode_eBack, 0, hResource);

	/* All done. now return the new fresh context to user. */
	*phDnr = (BVDC_P_Dnr_Handle)pDnr;

	BDBG_LEAVE(BVDC_P_Dnr_Create);

	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Dnr_Destroy
	( BVDC_P_Dnr_Handle             hDnr )
{
	BDBG_ENTER(BVDC_P_Dnr_Destroy);
	BDBG_OBJECT_ASSERT(hDnr, BVDC_DNR);

	BDBG_OBJECT_DESTROY(hDnr, BVDC_DNR);
	/* [1] Free the context. */
	BKNI_Free((void*)hDnr);

	BDBG_LEAVE(BVDC_P_Dnr_Destroy);

	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Dnr_Init
	( BVDC_P_Dnr_Handle             hDnr )
{
	BDBG_ENTER(BVDC_P_Dnr_Init);
	BDBG_OBJECT_ASSERT(hDnr, BVDC_DNR);

	/* Clear out shadow registers. */
	BKNI_Memset((void*)hDnr->aulRegs, 0x0, sizeof(hDnr->aulRegs));

#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
	/* Initial value */
	BVDC_P_DNR_GET_REG_DATA(DNR_0_EXT_FILT_CTRL) =
		BCHP_FIELD_ENUM(DNR_0_EXT_FILT_CTRL, ENABLE, DISABLE);
#endif

	/* Initialize state. */
	hDnr->bInitial      = true;
	hDnr->ulUpdateAll   = BVDC_P_RUL_UPDATE_THRESHOLD;
	hDnr->ulFilterLimit = 0;

	BDBG_LEAVE(BVDC_P_Dnr_Init);

	return;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Dnr_AcquireConnect
 *
 * It is called by BVDC_Window_Validate after changing from diable dnr to
 * enable dnr.
 */
BERR_Code BVDC_P_Dnr_AcquireConnect
	( BVDC_P_Dnr_Handle             hDnr,
	  BVDC_Source_Handle            hSource )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Dnr_AcquireConnect);


	hDnr->hSource = hSource;
	BVDC_P_Dnr_Init(hDnr);

	BDBG_LEAVE(BVDC_P_Dnr_AcquireConnect);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Dnr_ReleaseConnect_isr
 *
 * It is called after window decided that dnr is no-longer used by HW in its
 * vnet mode (i.e. it is really shut down and teared off from vnet).
 */
BERR_Code BVDC_P_Dnr_ReleaseConnect_isr
	( BVDC_P_Dnr_Handle         *phDnr )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Dnr_ReleaseConnect_isr);

	/* handle validation */
	if (NULL != *phDnr)
	{
		BDBG_OBJECT_ASSERT(*phDnr, BVDC_DNR);

		/* another win might still using it */
		BVDC_P_Resource_ReleaseHandle_isr(
			BVDC_P_SubRul_GetResourceHandle_isr(&(*phDnr)->SubRul),
			BVDC_P_ResourceType_eDnr, (void *)(*phDnr));

		/* this makes win to stop calling dnr code */
		*phDnr = NULL;
	}

	BDBG_LEAVE(BVDC_P_Dnr_ReleaseConnect_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Dnr_BuildRul_DrainVnet_isr
 *
 * called by BVDC_P_Dnr_BuildRul_isr after resetting to drain the module and
 * its pre-patch FreeCh or LpBack
 */
static void BVDC_P_Dnr_BuildRul_DrainVnet_isr
	( BVDC_P_Dnr_Handle              hDnr,
	  BVDC_P_ListInfo               *pList )
{

	BDBG_OBJECT_ASSERT(hDnr, BVDC_DNR);

	/* reset sub and connect the module to a drain */
	BVDC_P_SubRul_StartDrain_isr(&(hDnr->SubRul), pList,
		hDnr->ulResetRegAddr, hDnr->ulResetMask);

	/* disconnect the module from the drain; reset it again; drain the
	 * free-channel or loop-back used to to patch the vnet; and also
	 * reset the channel ping-pong buf of the module and the patcher. */
	 
	BVDC_P_SubRul_FinishDrain_isr(&(hDnr->SubRul), pList,
		hDnr->ulResetRegAddr, hDnr->ulResetMask,
		hDnr->ulVnetResetAddr, hDnr->ulVnetResetMask);
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Dnr_BuildRul_isr
	( const BVDC_P_Dnr_Handle       hDnr,
	  BVDC_P_ListInfo              *pList,
	  BVDC_P_State                  eVnetState,
	  BVDC_P_PicComRulInfo         *pPicComRulInfo )
{
	uint32_t  ulRulOpsFlags;

	BDBG_ENTER(BVDC_P_Dnr_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hDnr, BVDC_DNR);

	/* currently this is only for vnet building / tearing-off*/

	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hDnr->SubRul), pPicComRulInfo->eWin, eVnetState, pList->bLastExecuted);

	if ((0 == ulRulOpsFlags) ||
		(ulRulOpsFlags & BVDC_P_RulOp_eReleaseHandle))
		return;
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_SubRul_DropOffVnet_isr(&(hDnr->SubRul), pList);
		BVDC_P_Dnr_SetEnable_isr(hDnr, false);
		BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DNR_TOP_CTRL, pList->pulCurrent);
	}

	/* If rul failed to execute last time we'd re reprogrammed possible
	 * missing registers. */
	if((!pList->bLastExecuted) || (hDnr->bInitial))
	{
		hDnr->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}

	/* reset DNR */
	if(hDnr->bInitial)
	{
		BVDC_P_BUILD_RESET(pList->pulCurrent,
			hDnr->ulResetRegAddr, hDnr->ulResetMask);
		hDnr->bInitial = false;
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eEnable)
	{
		if(hDnr->ulUpdateAll)
		{
			BDBG_MSG(("DNR update RUL"));

#if (BVDC_P_SUPPORT_DNR_VER <= BVDC_P_SUPPORT_DNR_VER_1)
			/* PR35221: Disabled/modify/afterward */
			BVDC_P_DNR_GET_REG_DATA(DNR_0_DNR_TOP_CTRL) &= ~(
				BCHP_MASK(DNR_0_DNR_TOP_CTRL, DNR_ENABLE));
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DNR_TOP_CTRL, pList->pulCurrent);
			BVDC_P_DNR_GET_REG_DATA(DNR_0_DNR_TOP_CTRL) |=  (
				BCHP_FIELD_ENUM(DNR_0_DNR_TOP_CTRL, DNR_ENABLE, ENABLE));
#endif

#if (BVDC_P_SUPPORT_DNR_VER >= BVDC_P_SUPPORT_DNR_VER_6)
			BVDC_P_DNR_BLOCK_WRITE_TO_RUL(DNR_0_SRC_PIC_SIZE, DNR_0_MNR_CONFIG, pList->pulCurrent);
#else
			BVDC_P_DNR_BLOCK_WRITE_TO_RUL(DNR_0_LINE_STORE_CONFIG, DNR_0_EXT_FILT_CONFIG, pList->pulCurrent);
#endif
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DNR_DEMO_SETTING, pList->pulCurrent);
#if (BVDC_P_SUPPORT_DNR_VER == BVDC_P_SUPPORT_DNR_VER_7)
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DNR_DEMO_CTRL, pList->pulCurrent);
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DNR_VIDEO_MODE, pList->pulCurrent);
#endif
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DCR_CTRL, pList->pulCurrent);
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DCR_FILT_LIMIT, pList->pulCurrent);
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DCR_FILT_CONFIG, pList->pulCurrent);
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DCR_DITH_ORDER_PATTERN, pList->pulCurrent);
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DCR_DITH_ORDER_VALUE, pList->pulCurrent);
#if (BVDC_P_SUPPORT_DNR_VER == BVDC_P_SUPPORT_DNR_VER_0)
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DCR_DITH_ORDER_VALUE, pList->pulCurrent);
#endif
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DCR_DITH_RANDOM_PATTERN, pList->pulCurrent);
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DCR_DITH_RANDOM_VALUE, pList->pulCurrent);
			BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DCR_DITH_OUT_CTRL, pList->pulCurrent);
			hDnr->ulUpdateAll--;
		}

		/* must be the last, and every field */
		BVDC_P_DNR_WRITE_TO_RUL(DNR_0_SRC_SCAN_SETTING, pList->pulCurrent);
		BVDC_P_DNR_WRITE_TO_RUL(DNR_0_DNR_TOP_CTRL,     pList->pulCurrent);

		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			BVDC_P_SubRul_JoinInVnet_isr(&(hDnr->SubRul), pList);
		}
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
	{
		BVDC_P_Dnr_BuildRul_DrainVnet_isr(hDnr, pList);
	}

	BDBG_LEAVE(BVDC_P_Dnr_BuildRul_isr);

	return;
}

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Dnr_SetInfo_isr
	( BVDC_P_Dnr_Handle                hDnr,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	uint32_t ulSrcVSize, ulSrcHSize;
	uint32_t ulMnrQp, ulBnrQp, ulDcrQp;
	uint32_t ulAdjQp, ulVOffset, ulHOffset;
	uint32_t ulSliderValue;
	uint32_t ulDitherEn, ulDcrFiltEn;
	uint32_t ulBright0, ulBright1, ulBright2, ulFilterClamp;
	const BVDC_P_Rect  *pScanOut = &pPicture->hBuffer->hWindow->stCurInfo.hSource->stScanOut;
	BVDC_P_Source_Info *pSrcInfo;
	BVDC_P_DcrCfgEntry *pDcrCfg;
	BVDC_P_BnrCfgEntry *pBnrCfg;
	BVDC_P_MnrCfgEntry *pMnrCfg;
	const BVDC_Dnr_Settings *pDnrSettings;

	BDBG_ENTER(BVDC_P_Dnr_SetInfo_isr);
	BDBG_OBJECT_ASSERT(hDnr, BVDC_DNR);

	ulSrcVSize = (pPicture->pDnrIn->ulHeight);
	ulSrcHSize = (pPicture->pDnrIn->ulWidth);

	/* the Dnr source picture size is the feeder's scanout size */
	if(BAVC_Polarity_eFrame!=pPicture->PicComRulInfo.eSrcOrigPolarity)
	{
		ulSrcVSize = (ulSrcVSize + 1) / BVDC_P_FIELD_PER_FRAME;
	}

	pSrcInfo     = &hDnr->hSource->stCurInfo;
	pDnrSettings = &hDnr->hSource->stCurInfo.stDnrSettings;

	ulAdjQp = (pDnrSettings->ulQp != 0) ?
		pDnrSettings->ulQp : pPicture->ulAdjQp;

	/* setup DNR filters */
	ulMnrQp = (pDnrSettings->eMnrMode == BVDC_FilterMode_eEnable)
		? BVDC_P_DNR_CLAMP(0, BVDC_P_DNR_MAX_HW_QP_STEPS, (ulAdjQp *
		 (pDnrSettings->iMnrLevel + BVDC_QP_ADJUST_STEPS)/ BVDC_QP_ADJUST_STEPS))
		: 0;

	ulBnrQp = (pDnrSettings->eBnrMode == BVDC_FilterMode_eEnable)
		? BVDC_P_DNR_CLAMP(0, BVDC_P_DNR_MAX_HW_QP_STEPS, (ulAdjQp *
		 (pDnrSettings->iBnrLevel + BVDC_QP_ADJUST_STEPS)/ BVDC_QP_ADJUST_STEPS))
		: 0;

	/* No need to support Qp-dependent DCR adjustment. */
	ulDcrQp = (pDnrSettings->eDcrMode == BVDC_FilterMode_eEnable)
		? BVDC_P_DNR_CLAMP(0, BVDC_P_DNR_MAX_HW_QP_STEPS,
		 ((BVDC_P_DNR_MAX_HW_QP_STEPS / 2) *
		 (pDnrSettings->iDcrLevel + BVDC_QP_ADJUST_STEPS)/ BVDC_QP_ADJUST_STEPS))
		: 0;

	/* Every Vsync settings */
	/* panscan offset within an 8x8 block */
	BVDC_P_DNR_GET_REG_DATA(DNR_0_SRC_SCAN_SETTING) &= ~(
		  BCHP_MASK(DNR_0_SRC_SCAN_SETTING, H_OFFSET)
		| BCHP_MASK(DNR_0_SRC_SCAN_SETTING, V_OFFSET)
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
		| BCHP_MASK(DNR_0_SRC_SCAN_SETTING, SRC_FORMAT)
		| BCHP_MASK(DNR_0_SRC_SCAN_SETTING, SRC_FIELD_ID)
#endif
		);

	/* Internal computed or user computed */
	if(!pDnrSettings->bUserOffset)
	{
		ulVOffset = pScanOut->lTop  & 7;
		ulHOffset = pScanOut->lLeft & 7;
	}
	else
	{
		ulVOffset = pDnrSettings->ulVertOffset & 7;
		ulHOffset = pDnrSettings->ulHorzOffset & 7;
	}

	if(BAVC_Polarity_eTopField == pPicture->PicComRulInfo.eSrcOrigPolarity)
	{
		ulVOffset = (ulVOffset + 1) / 2;
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
		BVDC_P_DNR_GET_REG_DATA(DNR_0_SRC_SCAN_SETTING) |= (
			BCHP_FIELD_ENUM(DNR_0_SRC_SCAN_SETTING, SRC_FORMAT,   FIELD) |
			BCHP_FIELD_ENUM(DNR_0_SRC_SCAN_SETTING, SRC_FIELD_ID, TOP  ));
#endif
	}
	else if(BAVC_Polarity_eBotField == pPicture->PicComRulInfo.eSrcOrigPolarity)
	{
		ulVOffset = (ulVOffset + 0) / 2;
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
		BVDC_P_DNR_GET_REG_DATA(DNR_0_SRC_SCAN_SETTING) |= (
			BCHP_FIELD_ENUM(DNR_0_SRC_SCAN_SETTING, SRC_FORMAT,   FIELD) |
			BCHP_FIELD_ENUM(DNR_0_SRC_SCAN_SETTING, SRC_FIELD_ID, BOTTOM));
#endif
	}
	else
	{
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
		BVDC_P_DNR_GET_REG_DATA(DNR_0_SRC_SCAN_SETTING) |= (
			BCHP_FIELD_ENUM(DNR_0_SRC_SCAN_SETTING, SRC_FORMAT,   FRAME) |
			BCHP_FIELD_ENUM(DNR_0_SRC_SCAN_SETTING, SRC_FIELD_ID, TOP  ));
#endif
	}

	BVDC_P_DNR_GET_REG_DATA(DNR_0_SRC_SCAN_SETTING) |= (
		BCHP_FIELD_DATA(DNR_0_SRC_SCAN_SETTING, V_OFFSET, ulVOffset) |
		BCHP_FIELD_DATA(DNR_0_SRC_SCAN_SETTING, H_OFFSET, ulHOffset));

	/* Source size and destination size.   Detecting dynamics format (size)
	 * change.  Base on these information we can further bypass
	 * unnecessary computations. */
	if((hDnr->ulMnrQp != ulMnrQp) ||
	   (hDnr->ulBnrQp != ulBnrQp) ||
	   (hDnr->ulDcrQp != ulDcrQp) ||
	   (pSrcInfo->stDirty.stBits.bDnrAdjust) ||
	   !BVDC_P_DNR_COMPARE_FIELD_DATA(DNR_0_SRC_PIC_SIZE, HSIZE, ulSrcHSize) ||
	   !BVDC_P_DNR_COMPARE_FIELD_DATA(DNR_0_SRC_PIC_SIZE, VSIZE, ulSrcVSize) ||
	   !BVDC_P_DNR_COMPARE_FIELD_DATA(DNR_0_DNR_TOP_CTRL, DNR_ENABLE, 1))
	{
		/* Always re-enable after set info. */
		uint32_t ulDnrDemoEnable, ulDnrDemoSide, ulDnrDemoBoundary;

		ulDnrDemoEnable = (pSrcInfo->stSplitScreenSetting.eDnr == BVDC_SplitScreenMode_eDisable) ? 0 :1;
		ulDnrDemoSide = (pSrcInfo->stSplitScreenSetting.eDnr == BVDC_SplitScreenMode_eLeft) ?
			BCHP_DNR_0_DNR_DEMO_SETTING_DEMO_L_R_LEFT : BCHP_DNR_0_DNR_DEMO_SETTING_DEMO_L_R_RIGHT;
		ulDnrDemoBoundary = ulSrcHSize / 2;

		BVDC_P_DNR_GET_REG_DATA(DNR_0_DNR_TOP_CTRL) =
#if (BVDC_P_SUPPORT_DNR_VER == BVDC_P_SUPPORT_DNR_VER_5)
			BCHP_FIELD_DATA(DNR_0_DNR_TOP_CTRL, DNR_ENABLE_MODE, 1) |
#endif
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_7)
			BCHP_FIELD_DATA(DNR_0_DNR_TOP_CTRL, DNR_DEMO_ENABLE, ulDnrDemoEnable) |
#endif
			BCHP_FIELD_ENUM(DNR_0_DNR_TOP_CTRL, DNR_ENABLE, ENABLE);

#if (BVDC_P_SUPPORT_DNR_VER == BVDC_P_SUPPORT_DNR_VER_7)
		BVDC_P_DNR_GET_REG_DATA(DNR_0_DNR_DEMO_CTRL) =
			BCHP_FIELD_DATA(DNR_0_DNR_DEMO_CTRL, DNR_DEMO_ENABLE, ulDnrDemoEnable);

		BVDC_P_DNR_GET_REG_DATA(DNR_0_DNR_VIDEO_MODE) =
			BCHP_FIELD_DATA(DNR_0_DNR_VIDEO_MODE, BVB_VIDEO, pPicture->eSrcOrientation);
#endif

		BVDC_P_DNR_GET_REG_DATA(DNR_0_DNR_DEMO_SETTING) &= ~(
			BCHP_MASK(DNR_0_DNR_DEMO_SETTING, DEMO_L_R) |
			BCHP_MASK(DNR_0_DNR_DEMO_SETTING, DEMO_BOUNDARY));

		BVDC_P_DNR_GET_REG_DATA(DNR_0_DNR_DEMO_SETTING) |=  (
			BCHP_FIELD_DATA(DNR_0_DNR_DEMO_SETTING, DEMO_L_R, ulDnrDemoSide) |
			BCHP_FIELD_DATA(DNR_0_DNR_DEMO_SETTING, DEMO_BOUNDARY, ulDnrDemoBoundary));

		pSrcInfo->stDirty.stBits.bDnrAdjust = false;
		hDnr->ulMnrQp = ulMnrQp;
		hDnr->ulBnrQp = ulBnrQp;
		hDnr->ulDcrQp = ulDcrQp;
		hDnr->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;

		/* Set src size. */
		BVDC_P_DNR_GET_REG_DATA(DNR_0_SRC_PIC_SIZE) &= ~(
			BCHP_MASK(DNR_0_SRC_PIC_SIZE, HSIZE) |
			BCHP_MASK(DNR_0_SRC_PIC_SIZE, VSIZE));

		BVDC_P_DNR_GET_REG_DATA(DNR_0_SRC_PIC_SIZE) |=  (
			BCHP_FIELD_DATA(DNR_0_SRC_PIC_SIZE, HSIZE, ulSrcHSize) |
			BCHP_FIELD_DATA(DNR_0_SRC_PIC_SIZE, VSIZE, ulSrcVSize));

#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
#ifndef BCHP_DNR_0_LINE_STORE_CONFIG_MOD_WIDTH_MASK
		/* line store config */
		BVDC_P_DNR_GET_REG_DATA(DNR_0_LINE_STORE_CONFIG) &=
			~(BCHP_MASK(DNR_0_LINE_STORE_CONFIG, LS_MODE));

		if(ulSrcHSize <= BFMT_NTSC_WIDTH)
		{
			BVDC_P_DNR_GET_REG_DATA(DNR_0_LINE_STORE_CONFIG) |=
				(BCHP_FIELD_ENUM(DNR_0_LINE_STORE_CONFIG, LS_MODE, SD));
		}
		else
		{
			BVDC_P_DNR_GET_REG_DATA(DNR_0_LINE_STORE_CONFIG) |=
				(BCHP_FIELD_ENUM(DNR_0_LINE_STORE_CONFIG, LS_MODE, HD));
		}
#else
		/* line store config */
		BVDC_P_DNR_GET_REG_DATA(DNR_0_LINE_STORE_CONFIG) &= ~(
			BCHP_MASK(DNR_0_LINE_STORE_CONFIG, LS_MODE) |
			BCHP_MASK(DNR_0_LINE_STORE_CONFIG, MOD_WIDTH));

		if(ulSrcHSize <= BFMT_NTSC_WIDTH)
		{
			BVDC_P_DNR_GET_REG_DATA(DNR_0_LINE_STORE_CONFIG) |=  (
				BCHP_FIELD_ENUM(DNR_0_LINE_STORE_CONFIG, LS_MODE, SD) |
				BCHP_FIELD_DATA(DNR_0_LINE_STORE_CONFIG, MOD_WIDTH, ulSrcHSize));
		}
		else
		{
			BVDC_P_DNR_GET_REG_DATA(DNR_0_LINE_STORE_CONFIG) |=  (
				BCHP_FIELD_ENUM(DNR_0_LINE_STORE_CONFIG, LS_MODE, HD) |
				BCHP_FIELD_DATA(DNR_0_LINE_STORE_CONFIG, MOD_WIDTH, (ulSrcHSize + 2)/3));
		}
#endif
#endif

		/* Block Noise Reduction (BNR) */
		pBnrCfg = (BVDC_P_BnrCfgEntry *)BVDC_P_Dnr_GetBnrCfg_isr(ulBnrQp,
			pPicture->PicComRulInfo.eSrcOrigPolarity,
			pSrcInfo->pFmtInfo, pDnrSettings->pvUserInfo);

		/* If Get funtion returns NULL pointer, generate the BNR config table */
		/* locally, otherwise, copying the BNR config table to the context */
		if(!pBnrCfg)
		{
			pBnrCfg = &hDnr->stBnrCfg;
			pBnrCfg->ulHBnr = (ulBnrQp > 0);
			pBnrCfg->ulVBnr = (ulBnrQp > 0);
			pBnrCfg->ulLrLimit = (ulBnrQp == 0) ? 0 : BVDC_P_DNR_CLAMP(2, 14, 2 + (ulBnrQp >> 2) );
			pBnrCfg->ulSmallGrid = (pPicture->PicComRulInfo.eSrcOrigPolarity != BAVC_Polarity_eFrame);
#if (BVDC_P_SUPPORT_DNR_VER >= BVDC_P_SUPPORT_DNR_VER_3)
			pBnrCfg->ulHRel = (ulBnrQp == 0) ? 0 :
				BVDC_P_DNR_CLAMP(4 - (uint32_t)(pDnrSettings->iBnrLevel + 100) / 150, BCHP_GET_FIELD_DATA(~0, DNR_0_VBNR_CONFIG, VBNR_REL),
				2 + 14 / ulBnrQp);
			pBnrCfg->ulVRel = (ulBnrQp == 0) ? 0 :
				BVDC_P_DNR_CLAMP(4 - (uint32_t)(pDnrSettings->iBnrLevel + 100) / 150, BCHP_GET_FIELD_DATA(~0, DNR_0_VBNR_CONFIG, VBNR_REL),
				2 + 14 / ulBnrQp);
#else
			pBnrCfg->ulHRel = (ulBnrQp == 0) ? 0 :
				BVDC_P_DNR_CLAMP(4 - (uint32_t)(pDnrSettings->iBnrLevel + 100) / 150, BCHP_GET_FIELD_DATA(~0, DNR_0_BNR_CONFIG, BNR_REL),
				2 + 14 / ulBnrQp);
			pBnrCfg->ulVRel = (ulBnrQp == 0) ? 0 :
				BVDC_P_DNR_CLAMP(4 - (uint32_t)(pDnrSettings->iBnrLevel + 100) / 150, BCHP_GET_FIELD_DATA(~0, DNR_0_BNR_CONFIG, BNR_REL),
				2 + 14 / ulBnrQp);
#endif
			pBnrCfg->ulHLimit = BVDC_P_DNR_CLAMP(0, 80 + 25 * (uint32_t)((pDnrSettings->iBnrLevel + 100) / 150), (7 * ulBnrQp / 2));
			pBnrCfg->ulVLimit = BVDC_P_DNR_CLAMP(0, 80 + 25 * (uint32_t)((pDnrSettings->iBnrLevel + 100) / 150), (7 * ulBnrQp / 2));
		}
		else
		{
			hDnr->stBnrCfg = *pBnrCfg;
			pBnrCfg = &hDnr->stBnrCfg;
		}

		/* STREAKY_NOISE::DITHER adjust BNR */
		if(pSrcInfo->bPcInput && pSrcInfo->pPcInCfg->bDitAdj)
		{
			pBnrCfg->ulHBnr = 0;
			pBnrCfg->ulVBnr = 0;
		}
		BVDC_P_DNR_GET_REG_DATA(DNR_0_BNR_CTRL) = (
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
			BCHP_FIELD_DATA(DNR_0_BNR_CTRL, HBNR_ENABLE, pBnrCfg->ulHBnr) |
#endif
			BCHP_FIELD_DATA(DNR_0_BNR_CTRL, VBNR_ENABLE, pBnrCfg->ulVBnr));
#if (BVDC_P_SUPPORT_DNR_VER >= BVDC_P_SUPPORT_DNR_VER_3)
		/* TODO: program with the correct settings for VBNR and HBNR REL and LIMIT */
		BVDC_P_DNR_GET_REG_DATA(DNR_0_VBNR_CONFIG) &= ~(
			BCHP_MASK(DNR_0_VBNR_CONFIG, VBNR_LR_LIMIT) |
			BCHP_MASK(DNR_0_VBNR_CONFIG, VBNR_REL) |
			BCHP_MASK(DNR_0_VBNR_CONFIG, VBNR_LIMIT));
		BVDC_P_DNR_GET_REG_DATA(DNR_0_VBNR_CONFIG) |=
			BCHP_FIELD_DATA(DNR_0_VBNR_CONFIG, VBNR_LR_LIMIT, pBnrCfg->ulLrLimit) |
			BCHP_FIELD_DATA(DNR_0_VBNR_CONFIG, VBNR_REL,      pBnrCfg->ulVRel)     |
			BCHP_FIELD_DATA(DNR_0_VBNR_CONFIG, VBNR_LIMIT,    pBnrCfg->ulVLimit);
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
		BVDC_P_DNR_GET_REG_DATA(DNR_0_HBNR_CONFIG) &= ~(
			BCHP_MASK(DNR_0_HBNR_CONFIG, HBNR_SMALL_GRID) |
			BCHP_MASK(DNR_0_HBNR_CONFIG, HBNR_REL) |
			BCHP_MASK(DNR_0_HBNR_CONFIG, HBNR_LIMIT));
		BVDC_P_DNR_GET_REG_DATA(DNR_0_HBNR_CONFIG) |=
			BCHP_FIELD_DATA(DNR_0_HBNR_CONFIG, HBNR_SMALL_GRID, pBnrCfg->ulSmallGrid) |
			BCHP_FIELD_DATA(DNR_0_HBNR_CONFIG, HBNR_REL,        pBnrCfg->ulHRel)     |
			BCHP_FIELD_DATA(DNR_0_HBNR_CONFIG, HBNR_LIMIT,      pBnrCfg->ulHLimit);
#endif
#else
		BVDC_P_DNR_GET_REG_DATA(DNR_0_BNR_CONFIG) &= ~(
			BCHP_MASK(DNR_0_BNR_CONFIG, VBNR_LR_LIMIT) |
			BCHP_MASK(DNR_0_BNR_CONFIG, BNR_REL) |
			BCHP_MASK(DNR_0_BNR_CONFIG, BNR_LIMIT) |
			BCHP_MASK(DNR_0_BNR_CONFIG, HBNR_SMALL_GRID));
		BVDC_P_DNR_GET_REG_DATA(DNR_0_BNR_CONFIG) |=
			BCHP_FIELD_DATA(DNR_0_BNR_CONFIG, VBNR_LR_LIMIT,   pBnrCfg->ulLrLimit) |
			BCHP_FIELD_DATA(DNR_0_BNR_CONFIG, BNR_REL,         pBnrCfg->ulHRel)     |
			BCHP_FIELD_DATA(DNR_0_BNR_CONFIG, BNR_LIMIT,       pBnrCfg->ulHLimit)   |
			BCHP_FIELD_DATA(DNR_0_BNR_CONFIG, HBNR_SMALL_GRID, pBnrCfg->ulSmallGrid);
#endif

		/* Mosquito Noise Reduction (MNR) */
		pMnrCfg = (BVDC_P_MnrCfgEntry *)BVDC_P_Dnr_GetMnrCfg_isr(ulMnrQp,
			ulSrcHSize, pSrcInfo->pFmtInfo, pDnrSettings->pvUserInfo);

		/* If Get funtion returns NULL pointer, generate the MNR config table */
		/* locally, otherwise, copying the MNR config table to the context */
		if(!pMnrCfg)
		{
			uint32_t ulMnrMerge;

			pMnrCfg = &hDnr->stMnrCfg;
			pMnrCfg->ulMnr = (ulMnrQp > 0);
			pMnrCfg->ulSpot = (ulMnrQp > 10);
			ulSliderValue = (uint32_t)((pDnrSettings->iMnrLevel + BVDC_QP_ADJUST_STEPS)/BVDC_QP_ADJUST_STEPS)/2;

			ulMnrMerge = ulMnrQp / 6 + ((ulSrcHSize > BFMT_NTSC_WIDTH) ? 1 : 0);

			pMnrCfg->ulMerge = BVDC_P_DNR_CLAMP(0,
				BVDC_P_MIN(BCHP_GET_FIELD_DATA(~0, DNR_0_MNR_CONFIG, MNR_MERGE), 4),
				ulMnrMerge);

			pMnrCfg->ulRel = BVDC_P_DNR_CLAMP(12 - (uint32_t)((pDnrSettings->iMnrLevel + 100) / 150),
				BVDC_P_MIN(BCHP_GET_FIELD_DATA(~0, DNR_0_MNR_CONFIG, MNR_REL), 100),
				40 - (ulMnrQp * 7) / 2);

			/* PR32066: zoneplate pattern cause video flicking/beating when either
			 * BNR or MNR is turned on. */
			/*
			pMnrCfg->ulLimit = BVDC_P_DNR_CLAMP(0,
				BCHP_GET_FIELD_DATA(~0, DNR_0_MNR_CONFIG, MNR_LIMIT),
				ulMnrQp * 14 / 10); */
			/* PR34386: */
			/*pMnrCfg->ulLimit = 0;*/
			pMnrCfg->ulLimit = BVDC_P_DNR_CLAMP(0, 50 + 7 * (uint32_t)((pDnrSettings->iMnrLevel + 100) / 150), ((ulMnrQp > 3)? ulMnrQp-3 : 0) * 15 / 10);
			if(hDnr->ulFilterLimit == 0)
			{
				hDnr->ulFilterLimit = pMnrCfg->ulLimit * 100;
			}
			if(pMnrCfg->ulLimit * 10000 > hDnr->ulFilterLimit * 110)
			{
				hDnr->ulFilterLimit = hDnr->ulFilterLimit * 110;
			}
			else if(pMnrCfg->ulLimit * 10000 < hDnr->ulFilterLimit * 98)
			{
				hDnr->ulFilterLimit = hDnr->ulFilterLimit * 98;
			}
			else
			{
				hDnr->ulFilterLimit = pMnrCfg->ulLimit * 10000;
			}

			pMnrCfg->ulLimit = (hDnr->ulFilterLimit + 5000) / 10000;
			hDnr->ulFilterLimit = hDnr->ulFilterLimit / 100;
		}
		else
		{
			hDnr->stMnrCfg = *pMnrCfg;
			pMnrCfg = &hDnr->stMnrCfg;
		}

		/* STREAKY_NOISE::DITHER adjust MNR */
		if(pSrcInfo->bPcInput && pSrcInfo->pPcInCfg->bDitAdj)
		{
			pMnrCfg->ulMnr = 0;
		}

		BVDC_P_DNR_GET_REG_DATA(DNR_0_MNR_CTRL) =
			BCHP_FIELD_DATA(DNR_0_MNR_CTRL, MNR_ENABLE, pMnrCfg->ulMnr);

		BVDC_P_DNR_GET_REG_DATA(DNR_0_MNR_CONFIG) &= ~(
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
			BCHP_MASK(DNR_0_MNR_CONFIG, MNR_SPOT) |
#endif
#if (BVDC_P_SUPPORT_DNR_VER == BVDC_P_SUPPORT_DNR_VER_7)
			BCHP_MASK(DNR_0_MNR_CONFIG, MNR_FILTER_SIZE) |
#endif
			BCHP_MASK(DNR_0_MNR_CONFIG, MNR_MERGE) |
			BCHP_MASK(DNR_0_MNR_CONFIG, MNR_REL) |
#if (BVDC_P_SUPPORT_DNR_VER >= BVDC_P_SUPPORT_DNR_VER_2)
			BCHP_MASK(DNR_0_MNR_CONFIG, MNR_BLOCK_BOUND) |
#endif
			BCHP_MASK(DNR_0_MNR_CONFIG, MNR_LIMIT));

		BVDC_P_DNR_GET_REG_DATA(DNR_0_MNR_CONFIG) |= (
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
			BCHP_FIELD_DATA(DNR_0_MNR_CONFIG, MNR_SPOT,  pMnrCfg->ulSpot ) |
#endif
#if (BVDC_P_SUPPORT_DNR_VER == BVDC_P_SUPPORT_DNR_VER_7)
			BCHP_FIELD_DATA(DNR_0_MNR_CONFIG, MNR_FILTER_SIZE, 0         ) |
#endif
			BCHP_FIELD_DATA(DNR_0_MNR_CONFIG, MNR_MERGE, pMnrCfg->ulMerge) |
			BCHP_FIELD_DATA(DNR_0_MNR_CONFIG, MNR_REL,   pMnrCfg->ulRel  ) |
#if (BVDC_P_SUPPORT_DNR_VER >= BVDC_P_SUPPORT_DNR_VER_2)
			BCHP_FIELD_DATA(DNR_0_MNR_CONFIG, MNR_BLOCK_BOUND, 1         ) |
#endif
			BCHP_FIELD_DATA(DNR_0_MNR_CONFIG, MNR_LIMIT, pMnrCfg->ulLimit));

		pDcrCfg = (BVDC_P_DcrCfgEntry *)BVDC_P_Dnr_GetDcrCfg_isr(ulDcrQp,
			pSrcInfo->pFmtInfo, pDnrSettings->pvUserInfo);
		hDnr->stDcrCfg = *pDcrCfg;
		pDcrCfg = &hDnr->stDcrCfg;

		/* STREAKY_NOISE::DITHER adjust MNR */
		if(pSrcInfo->bPcInput && pSrcInfo->pPcInCfg->bDitAdj)
		{
			ulDitherEn = 1;
			pDcrCfg->ulDithClamp = 0xff;
			if(pSrcInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1600x1200p_60Hz)
			{
				pDcrCfg->ulRandomA = 1;
				pDcrCfg->ulRandomB = 3;
				pDcrCfg->ulRandomC = 5;
				pDcrCfg->ulRandomD = 7;
			}
			else
			{
				pDcrCfg->ulRandomA = 1;
				pDcrCfg->ulRandomB = 2;
				pDcrCfg->ulRandomC = 3;
				pDcrCfg->ulRandomD = 4;
			}
		}
		else
		{
#if (BVDC_P_SUPPORT_DNR_VER == BVDC_P_SUPPORT_DNR_VER_0)
			ulDitherEn = 0;
#else
			ulDitherEn = pSrcInfo->bPcInput ? 0 : (ulDcrQp > 0);
#endif
		}
		if(pSrcInfo->bPcInput && pSrcInfo->pPcInCfg->bDcrAdj)
		{
			ulDcrFiltEn = 1;
			pDcrCfg->ulFilt3Limit = 0x3;
			pDcrCfg->ulFilt2Limit = 0x3;
			pDcrCfg->ulFilt1Limit = 0x4;
			pDcrCfg->ulFilt0Limit = 0x5;
			ulBright0 = 0;
			ulBright1 = 0;
			ulBright2 = 0;
			ulFilterClamp = 0x6;
		}
		else
		{
			ulDcrFiltEn = pSrcInfo->bPcInput ? 0 : (ulDcrQp > 0);
			ulBright0 = 0;
			ulBright1 = 1;
			ulBright2 = 0;
			ulFilterClamp = 8;
		}

		/* PR47349: always disable DITH in DCR block for 10-bit chips */
#if (BVDC_P_SUPPORT_BVN_10BIT_444)
		ulDitherEn = 0;
#endif

		BVDC_P_DNR_GET_REG_DATA(DNR_0_DCR_CTRL) =
			BCHP_FIELD_DATA(DNR_0_DCR_CTRL, DITH_ENABLE, ulDitherEn  ) |
			BCHP_FIELD_DATA(DNR_0_DCR_CTRL, FILT_ENABLE, ulDcrFiltEn );
		BVDC_P_DNR_GET_REG_DATA(DNR_0_DCR_FILT_LIMIT) =
#if (BVDC_P_SUPPORT_DNR_VER < BVDC_P_SUPPORT_DNR_VER_6)
			BCHP_FIELD_DATA(DNR_0_DCR_FILT_LIMIT, FILT_3_LIMIT, pDcrCfg->ulFilt3Limit) |
#endif
			BCHP_FIELD_DATA(DNR_0_DCR_FILT_LIMIT, FILT_2_LIMIT, pDcrCfg->ulFilt2Limit) |
			BCHP_FIELD_DATA(DNR_0_DCR_FILT_LIMIT, FILT_1_LIMIT, pDcrCfg->ulFilt1Limit) |
			BCHP_FIELD_DATA(DNR_0_DCR_FILT_LIMIT, FILT_0_LIMIT, pDcrCfg->ulFilt0Limit);
		BVDC_P_DNR_GET_REG_DATA(DNR_0_DCR_FILT_CONFIG) =
			BCHP_FIELD_DATA(DNR_0_DCR_FILT_CONFIG, BRIGHT_0,   ulBright0    ) |
			BCHP_FIELD_DATA(DNR_0_DCR_FILT_CONFIG, BRIGHT_1,   ulBright1    ) |
			BCHP_FIELD_DATA(DNR_0_DCR_FILT_CONFIG, BRIGHT_2,   ulBright2    ) |
			BCHP_FIELD_DATA(DNR_0_DCR_FILT_CONFIG, FILT_CLAMP, pDcrCfg->ulFiltClamp);

		BVDC_P_DNR_GET_REG_DATA(DNR_0_DCR_DITH_ORDER_PATTERN) =
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_ORDER_PATTERN, ALTERNATE_Y, 0) |
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_ORDER_PATTERN, ALTERNATE_X, 1) |
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_ORDER_PATTERN, INVERT_Y,    0) |
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_ORDER_PATTERN, INVERT_X,    0) |
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_ORDER_PATTERN, AUTO_DITH,   1);

		BVDC_P_DNR_GET_REG_DATA(DNR_0_DCR_DITH_ORDER_VALUE) =
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_ORDER_VALUE, ORDER_A, pDcrCfg->ulOrderA) |
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_ORDER_VALUE, ORDER_B, pDcrCfg->ulOrderB);

		BVDC_P_DNR_GET_REG_DATA(DNR_0_DCR_DITH_RANDOM_VALUE) =
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_RANDOM_VALUE, RANDOM_A, pDcrCfg->ulRandomA) |
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_RANDOM_VALUE, RANDOM_B, pDcrCfg->ulRandomB) |
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_RANDOM_VALUE, RANDOM_C, pDcrCfg->ulRandomC) |
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_RANDOM_VALUE, RANDOM_D, pDcrCfg->ulRandomD);
		BVDC_P_DNR_GET_REG_DATA(DNR_0_DCR_DITH_OUT_CTRL) =
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_OUT_CTRL, DITH_CLAMP, pDcrCfg->ulDithClamp);
#if (BVDC_P_SUPPORT_DNR_VER == BVDC_P_SUPPORT_DNR_VER_0)
		BVDC_P_DNR_GET_REG_DATA(DNR_0_DCR_DITH_ORDER_VALUE) =
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_ORDER_VALUE, ORDER_A, 0) |
			BCHP_FIELD_DATA(DNR_0_DCR_DITH_ORDER_VALUE, ORDER_B, 0);
#endif

		BDBG_MSG(("-------------------------"));
		BDBG_MSG(("Dnr[%d]         : %dx%d", hDnr->eId - BVDC_P_DnrId_eDnr0,
			pPicture->pDnrIn->ulWidth,  pPicture->pDnrIn->ulHeight));
		BDBG_MSG(("Dnr[%d]'offset  : %dx%d", hDnr->eId - BVDC_P_DnrId_eDnr0,
			ulHOffset, ulVOffset));
		BDBG_MSG(("ulAdjQp           : %d", ulAdjQp));
		BDBG_MSG(("ulBnrQp           : %d", ulBnrQp));
		BDBG_MSG(("ulMnrQp           : %d", ulMnrQp));
		BDBG_MSG(("ulDcrQp           : %d", ulDcrQp));
		BDBG_MSG(("SrcPolarity       : %d", pPicture->PicComRulInfo.eSrcOrigPolarity));
	}

	BDBG_LEAVE(BVDC_P_Dnr_SetInfo_isr);

	return BERR_SUCCESS;
}

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Dnr_SetEnable_isr
	( BVDC_P_Dnr_Handle                hDnr,
	  bool                             bEnable )
{
	BDBG_OBJECT_ASSERT(hDnr, BVDC_DNR);

	if(!BVDC_P_DNR_COMPARE_FIELD_DATA(DNR_0_DNR_TOP_CTRL, DNR_ENABLE, bEnable))
	{
		BVDC_P_DNR_GET_REG_DATA(DNR_0_DNR_TOP_CTRL) &= ~(
			BCHP_MASK(DNR_0_DNR_TOP_CTRL, DNR_ENABLE));
		BVDC_P_DNR_GET_REG_DATA(DNR_0_DNR_TOP_CTRL) |=  bEnable;
		hDnr->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}

	return BERR_SUCCESS;
}

#else

/***************************************************************************
 * Stub function for chipset with no DNR.
 */
BERR_Code BVDC_P_Dnr_Create
	( BVDC_P_Dnr_Handle            *phDnr,
	  BVDC_P_DnrId                  eDnrId,
	  BVDC_P_Resource_Handle        hResource )
{
	BSTD_UNUSED(phDnr);
	BSTD_UNUSED(eDnrId);
	BSTD_UNUSED(hResource);
	return BERR_SUCCESS;
}

void BVDC_P_Dnr_Destroy
	( BVDC_P_Dnr_Handle             hDnr )
{
	BSTD_UNUSED(hDnr);
	return;
}

void BVDC_P_Dnr_Init
	( BVDC_P_Dnr_Handle             hDnr )
{
	BSTD_UNUSED(hDnr);
	return;
}

BERR_Code BVDC_P_Dnr_AcquireConnect
	( BVDC_P_Dnr_Handle             hDnr,
	  BVDC_Source_Handle            hSource )
{
	BSTD_UNUSED(hDnr);
	BSTD_UNUSED(hSource);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Dnr_ReleaseConnect_isr
	( BVDC_P_Dnr_Handle            *phDnr )
{
	BSTD_UNUSED(phDnr);
	return BERR_SUCCESS;
}

void BVDC_P_Dnr_BuildRul_isr
	( const BVDC_P_Dnr_Handle       hDnr,
	  BVDC_P_ListInfo              *pList,
	  BVDC_P_State                  eVnetState,
	  BVDC_P_PicComRulInfo         *pPicComRulInfo )
{
	BSTD_UNUSED(hDnr);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eVnetState);
	BSTD_UNUSED(pPicComRulInfo);
	return;
}

BERR_Code BVDC_P_Dnr_SetInfo_isr
	( BVDC_P_Dnr_Handle                hDnr,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	BSTD_UNUSED(hDnr);
	BSTD_UNUSED(pPicture);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Dnr_SetEnable_isr
	( BVDC_P_Dnr_Handle                hDnr,
	  bool                             bEnable )
{
	BSTD_UNUSED(hDnr);
	BSTD_UNUSED(bEnable);
	return BERR_SUCCESS;
}

#endif

/* End of file. */
