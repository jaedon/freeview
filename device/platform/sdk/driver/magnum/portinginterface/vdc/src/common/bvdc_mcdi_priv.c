/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_mcdi_priv.c $
* $brcm_Revision: Hydra_Software_Devel/80 $
* $brcm_Date: 9/5/12 2:06p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_mcdi_priv.c $
* 
* Hydra_Software_Devel/80   9/5/12 2:06p vanessah
* SW7425-3864: disable double-buffering in BVN
* 
* Hydra_Software_Devel/79   8/23/12 1:21p vanessah
* SW7429-243: add support for 7429B0/7435B0/7548A0
* 
* Hydra_Software_Devel/78   5/11/12 4:32p vanessah
* SW7346-819: fix compile warnings
* 
* Hydra_Software_Devel/77   5/11/12 1:48p vanessah
* SW7346-819: mcdi xchroma setting depends on source
* 
* Hydra_Software_Devel/76   5/8/12 2:18p vanessah
* SW7435-25: align 7435 MCDI with C model golden value
* 
* Hydra_Software_Devel/75   5/7/12 6:22p vanessah
* SW7418-96: fix the testfeature propogation error
* 
* Hydra_Software_Devel/74   3/29/12 2:24p tdo
* SW7435-85: Fix compiling error
*
* Hydra_Software_Devel/73   3/29/12 11:19a tdo
* SW7435-85: MVP_0 enable error.  Program PPB_x_START_IT_LINE.
*
* Hydra_Software_Devel/72   3/25/12 1:49p vanessah
* SW7552-213: disable 2-2 pulldown in the default setting.
*
* Hydra_Software_Devel/71   3/8/12 5:27p vanessah
* SW7435-31: Sync MADR golden value and removal EoP trigger workaround
*
* Hydra_Software_Devel/70   1/12/12 5:39p vanessah
* SW7425-2162: change 7425 MADR QM buffer allocation
*
* Hydra_Software_Devel/69   11/23/11 4:22p vanessah
* SW7425-1765: sync with 7425B0 golden set
*
* Hydra_Software_Devel/68   9/28/11 4:16p pntruong
* SW7425-969: Refactored mcvp/madr code to be multi-instances friendly.
* Adapt to vnet naming changes.
*
* Hydra_Software_Devel/67   9/23/11 5:37p pntruong
* SW7231-383: Removed warnings.
*
* Hydra_Software_Devel/66   9/23/11 5:16p pntruong
* SW7231-383: Correct programming of game mode and delay callback
* reporting for madr/mcvp.
*
* Hydra_Software_Devel/65   9/23/11 3:16p pntruong
* HW7552-62, HW7428-4, HWBCM7231B0-81, HW7425-938: corrected typo.
*
* Hydra_Software_Devel/64   9/23/11 10:37a pntruong
* HW7552-62, HW7428-4, HWBCM7231B0-81, HW7425-938: Excluded work-arounds
* for chips with fix.
*
* Hydra_Software_Devel/63   8/26/11 4:15p pntruong
* SW7552-43: Deinterlacer hardware requires 3:2 pulldown to be off if
* content is 50hz base.
*
* Hydra_Software_Devel/62   7/19/11 11:44a vanessah
* SW7425-309: fix golden mcdi default value setting
*
* Hydra_Software_Devel/61   7/18/11 8:47a vanessah
* SW7425-309: mcdi default value set
*
* Hydra_Software_Devel/60   7/11/11 6:09p vanessah
* SW7231-291: SW7425-689: change QM default value and sync application
* and PI MADR compression value
*
* Hydra_Software_Devel/59   7/7/11 11:50a vanessah
* SW7425-488: MADR register configuration alignment with architecture
* default. OBTS_CONTROL.CORE
*
* Hydra_Software_Devel/58   6/28/11 12:59p tdo
* SW7420-1967: Video blinking is occurred whenever the channel changes.
* [Deinterlace on]
*
* Hydra_Software_Devel/57   5/12/11 4:24p yuxiaz
* SW7231-74: Added software workaround for MADR alignment hung.
*
* Hydra_Software_Devel/56   5/5/11 4:19p vanessah
* SW7425-309: SW7425-488: set MCDI/MADR to golden value
*
* Hydra_Software_Devel/55   5/5/11 2:20p vanessah
* SW7425-309: SW7425-488: undo the wrong merge of version 54
*
* Hydra_Software_Devel/54   5/5/11 12:03p syang
* SW7420-1768: mcdi/madr can not set freeze bit in 1st 4 fields
*
* Hydra_Software_Devel/53   5/5/11 11:19a vanessah
* SW7425-309: SW7425-488: MADR/MCDI golden value initialization
*
* Hydra_Software_Devel/52   5/4/11 4:03p vanessah
* SW7425-309: Seperate MCDI and MADR initialization
*
* Hydra_Software_Devel/51   4/26/11 6:11p pntruong
* SW7231-74: Added temporary measure by using fixed rate to avoid dcx-
* madr hung.
*
* Hydra_Software_Devel/50   4/25/11 3:49p yuxiaz
* SW7425-348: Added software workaround for MADR alignment.
*
* Hydra_Software_Devel/49   4/19/11 5:35p vanessah
* SW7425-314: change REV32_LOCK_SAT_LEVEL default value from 12 to 28
*
* Hydra_Software_Devel/48   4/18/11 11:48a vanessah
* SW7425-314: change the MIPS reading to RUL one
*
* Hydra_Software_Devel/47   4/18/11 10:35a vanessah
* SW7425-314: change the MADR REV32 ENTER/EXIT level to architecture
* default
*
* Hydra_Software_Devel/46   4/18/11 8:20a vanessah
* SW7425-374: turn on osd for Mcdi debug
*
* Hydra_Software_Devel/45   4/7/11 7:45p vanessah
* SW7425-309: PQ optimization
*
* Hydra_Software_Devel/44   4/7/11 8:31a vanessah
* SW7231-110:enable OBTS register for MAD-R
*
* Hydra_Software_Devel/43   4/7/11 8:24a vanessah
* SW7231-110:enable OBTS register for MAD-R
*
* Hydra_Software_Devel/42   4/6/11 4:45p tdo
* SW7420-965: Use normal delay for game mode off
*
* Hydra_Software_Devel/41   3/31/11 11:43p tdo
* SW7344-36: Playback trick modes cause Video flicker and bands.
*
* Hydra_Software_Devel/40   3/30/11 2:49p tdo
* SW7344-36: Remove warning
*
* Hydra_Software_Devel/39   3/29/11 2:20p tdo
* SW7344-36: Fix compiling error
*
* Hydra_Software_Devel/38   3/29/11 1:51p tdo
* SW7344-36: Playback trick modes cause Video flicker and bands
*
* Hydra_Software_Devel/37   3/4/11 2:21p vanessah
* SW7425-124: mcdi QM_MAPPING_RANGE default value change
*
* Hydra_Software_Devel/36   3/3/11 11:29a vanessah
* SW7425-124: change mcdi default setting
*
* Hydra_Software_Devel/35   2/25/11 10:21a vanessah
* SW7425-124: fix compile error for 7358 755 7231 7346 7344 7420 chip
*
* Hydra_Software_Devel/34   2/24/11 4:30p vanessah
* SW7425-124: change MCDI default value to address PQ issues
*
* Hydra_Software_Devel/33   2/22/11 5:30p vanessah
* SW7231-35: weave artifact for gbeans clip
*
* Hydra_Software_Devel/32   1/10/11 1:33p yuxiaz
* SW7422-174: Fixed build error on 7344.
*
* Hydra_Software_Devel/31   1/7/11 4:46p yuxiaz
* SW7422-174: Added DCX support to MCVP and MADR.
*
* Hydra_Software_Devel/30   1/6/11 9:47a pntruong
* SW7422-174: Initialized madr with compression enabled to match up with
* default rts for reference usage.
*
* Hydra_Software_Devel/29   12/29/10 3:47p pntruong
* SW7422-174: Initial support for siob in madr.
*
* Hydra_Software_Devel/28   12/28/10 5:02p tdo
* SW7422-148: Refactor MCVP to address chips w/wo MCTF
*
* Hydra_Software_Devel/27   12/27/10 3:59p vanessah
* SW7425-58:
* BVN MCDI extra QM register error
*
* Hydra_Software_Devel/26   12/23/10 5:12p tdo
* SW7422-148: Refactor MCVP to address chips w/wo MCTF
*
* Hydra_Software_Devel/25   12/20/10 3:03p vanessah
* SW7425-58:  New MCDI feature support
*
* Hydra_Software_Devel/24   12/17/10 10:18a vanessah
* SW7425-58:  Coverity error fix
*
* Hydra_Software_Devel/23   12/16/10 1:09p vanessah
* SW7425-58:  New MCDI feature support
*
* Hydra_Software_Devel/22   11/24/10 2:15p tdo
* SW7422-52: Fix compiling error
*
* Hydra_Software_Devel/21   11/24/10 2:08p tdo
* SW7422-52: Fix compiling error on 7420
*
* Hydra_Software_Devel/20   11/24/10 1:24p tdo
* SW7422-52: Bring-up MAD-R
*
* Hydra_Software_Devel/19   11/15/10 6:09p tdo
* SW7422-36: Fix compiling warning on 7420
*
* Hydra_Software_Devel/18   11/15/10 5:49p tdo
* SW7422-36: Fix compiling error on 7420
*
* Hydra_Software_Devel/17   11/15/10 4:41p tdo
* SW7422-36: Handle "left only" method for BVN internal data path
*
* Hydra_Software_Devel/16   11/5/10 3:09p vanessah
* SW7422-52:  Two kinds of deinterlacers in the same chip
*
* Hydra_Software_Devel/15   11/3/10 10:24p tdo
* SW7420-1224: Add trick mode setting to MCDI
*
* Hydra_Software_Devel/14   10/18/10 5:04p vanessah
* SW7422-52:  Two deinterlacer on the same chip
*
* Hydra_Software_Devel/13   9/2/10 1:36p vanessah
* SW7422-52:  MAD + MCVP both on 7422
*
* Hydra_Software_Devel/12   8/26/10 4:26p vanessah
* SW7420-965:  Game mode enum consistency
*
* Hydra_Software_Devel/11   8/26/10 2:39p vanessah
* SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
* mode
*
* Hydra_Software_Devel/SW7420-965/2   8/20/10 1:54p vanessah
* SW7420-965:  Eliminate Warning and error for chips earlier than 7420
*
* Hydra_Software_Devel/SW7420-965/1   8/20/10 1:26p vanessah
* SW7420-965:  MCVP game mode support
*
* Hydra_Software_Devel/SW7420-588/1   8/9/10 7:22p vanessah
* SW7420-588:  panscan to zoom video loss
*
* Hydra_Software_Devel/6   5/7/10 7:18p albertl
* SW7125-364: Changed dirty bits to use union structure to avoid type-pun
* warnings
*
* Hydra_Software_Devel/6   5/7/10 7:11p albertl
* SW7125-364: Changed dirty bits to use union structure to avoid type-pun
* warnings.
*
* Hydra_Software_Devel/5   4/7/10 11:32a tdo
* SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
*
* Hydra_Software_Devel/4   4/5/10 4:11p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic
*
* Hydra_Software_Devel/3   2/9/10 2:26p tdo
* SW7420-575: Name change MCDI_* -> MDI_*
*
* Hydra_Software_Devel/2   11/10/09 4:07p syang
* HW7420-814: set IT_FIELD_PHASE_CALC_CONTROL_5.
* ALT_REPF_VETO_LEVEL_ENABLE as 1
*
* Hydra_Software_Devel/1   3/16/09 11:09p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/2   2/12/09 6:16p tdo
* PR 45785, PR 45789: Use correct delay for MCDI
*
* Hydra_Software_Devel/7420_mcvp/1   1/23/09 8:55p syang
* PR 45785, PR 45789: init MCVP implementation
*
***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "brdc.h"

#include "bvdc.h"
#include "bvdc_mcdi_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_compositor_priv.h"

#ifndef BVDC_P_MCDI_DBG_MSG
#define BVDC_P_MCDI_DBG_MSG                       (0)
#endif
#if BVDC_P_MCDI_DBG_MSG
#define BVDC_P_MCDI_MSG                                BDBG_MSG
#else
#define BVDC_P_MCDI_MSG(a)
#endif

/* Nicer formating and ensure correct location for enum indexing. */
#define BVDC_P_MAKE_GAMEMODE_INFO(mode, delay, buffer_cnt)                   \
{                                                                            \
	(BVDC_MadGameMode_##mode), (delay), (buffer_cnt), (#mode)                \
}

/* Mcdi game mode delay tables */
static const BVDC_P_McdiGameModeInfo s_aMcdiGameModeInfo[] =
{
	BVDC_P_MAKE_GAMEMODE_INFO(eOff,                  2, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_2Delay,       2, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_1Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_0Delay,       0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_ForceSpatial, 0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_2Delay,       2, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_1Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_0Delay,       0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_ForceSpatial, 0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_2Delay,       2, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_1Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_0Delay,       0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_ForceSpatial, 0, 4)
};

/* Madr game mode delay tables */
static const BVDC_P_McdiGameModeInfo s_aMadrGameModeInfo[] =
{
	BVDC_P_MAKE_GAMEMODE_INFO(eOff,                  1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_2Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_1Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_0Delay,       0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_ForceSpatial, 0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_2Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_1Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_0Delay,       0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_ForceSpatial, 0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_2Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_1Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_0Delay,       0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_ForceSpatial, 0, 4)
};
#if (BVDC_P_SUPPORT_MCVP)
#if (BVDC_P_SUPPORT_MCDI_VER==1)
#include "bchp_dmisc.h"
#endif
#include "bchp_mdi_top_0.h"
#if (BVDC_P_SUPPORT_MCDI_VER != 0)
#include "bchp_mdi_fcb_0.h"
#endif
#include "bchp_mdi_ppb_0.h"
#include "bchp_mmisc.h"
#include "bchp_bvnf_intr2_5.h"
#if ((BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_2) || BVDC_P_SUPPORT_MADR_VER != 0)
#include "bchp_siob_0.h"
#include "bchp_mdi_fcn_0.h"
#endif


BDBG_MODULE(BVDC_MCDI);
BDBG_OBJECT_ID(BVDC_MDI);

/***************************************************************************
* {private}
*
*/
BERR_Code BVDC_P_Mcdi_Create
	( BVDC_P_Mcdi_Handle           *phMcdi,
	  BVDC_P_McdiId                 eMcdiId,
	  BREG_Handle                   hRegister,
	  BVDC_P_Resource_Handle        hResource )
{
	BVDC_P_McdiContext *pMcdi;

	BDBG_ENTER(BVDC_P_Mcdi_Create);

	BDBG_ASSERT(phMcdi);

	/* (1) Alloc the context. */
	pMcdi = (BVDC_P_McdiContext*)
		(BKNI_Malloc(sizeof(BVDC_P_McdiContext)));
	if(!pMcdi)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pMcdi, 0x0, sizeof(BVDC_P_McdiContext));
	BDBG_OBJECT_SET(pMcdi, BVDC_MDI);

	pMcdi->eId              = eMcdiId;
	pMcdi->hRegister        = hRegister;
	pMcdi->ulRegOffset      = 0;

#if (BVDC_P_SUPPORT_MCVP > 1)
	pMcdi->ulRegOffset      = (eMcdiId - BVDC_P_McdiId_eMcdi0)
		*(BCHP_MDI_TOP_1_REG_START - BCHP_MDI_TOP_0_REG_START);
#endif

	/* init the SubRul sub-module */
	BVDC_P_SubRul_Init(&(pMcdi->SubRul), BVDC_P_Mcdi_MuxAddr(pMcdi),
		BVDC_P_Mcdi_PostMuxValue(pMcdi), BVDC_P_DrainMode_eBack, 0, hResource);

	/* All done. now return the new fresh context to user. */
	*phMcdi = (BVDC_P_Mcdi_Handle)pMcdi;

	BDBG_LEAVE(BVDC_P_Mcdi_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Mcdi_Destroy
	( BVDC_P_Mcdi_Handle             hMcdi )
{
	BDBG_ENTER(BVDC_P_Mcdi_Destroy);
	BDBG_OBJECT_ASSERT(hMcdi, BVDC_MDI);

	/* [1] Free the context. */
	BDBG_ASSERT(NULL == hMcdi->apHeapNode[0]);
	BDBG_OBJECT_DESTROY(hMcdi, BVDC_MDI);
	BKNI_Free((void*)hMcdi);

	BDBG_LEAVE(BVDC_P_Mcdi_Destroy);
	return;
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Mcdi_Init
	( BVDC_P_Mcdi_Handle             hMcdi )
{
	uint32_t  ulReg;

	BDBG_ENTER(BVDC_P_Mcdi_Init);
	BDBG_OBJECT_ASSERT(hMcdi, BVDC_MDI);

#if (BVDC_P_SUPPORT_MCVP_VER >=2)
	ulReg = BREG_Read32(hMcdi->hRegister, BCHP_MDI_TOP_0_HW_CONFIGURATION + hMcdi->ulRegOffset);
	hMcdi->bMadr = BVDC_P_COMPARE_FIELD_NAME(ulReg, MDI_TOP_0_HW_CONFIGURATION, TYPE, MADR);
#else
	BSTD_UNUSED(ulReg);
	hMcdi->bMadr = false;
#endif

	hMcdi->bInitial              = true;
	hMcdi->bEnableOsd            = false;
	hMcdi->ulOsdHpos             = 0;
	hMcdi->ulOsdVpos             = 0;
	hMcdi->bRev32Pulldown        = false;
	hMcdi->bRev22Pulldown        = false; /* SW7552-213: turn off 2:2 pull down */
	hMcdi->usTrickModeStartDelay = 0;

	BKNI_Memset((void*)&hMcdi->stTestFeature1, 0x0, sizeof(hMcdi->stTestFeature1));

	/* Default reference rts, application specific usage can turn on/off as
	 * needed. */
	if(hMcdi->bMadr)
	{
		hMcdi->stTestFeature1.bEnable = true;
		hMcdi->stTestFeature1.ulBitsPerPixel = BVDC_MADR_DCXS_HALFBITS_PER_PIXEL;
	}

	BDBG_LEAVE(BVDC_P_Mcdi_Init);

	return;
}


/***************************************************************************
* {private}
*  MADR src Init
*/
static void BVDC_P_Mcdi_BuildRul_Madr_SrcInit_isr
	( BVDC_P_Mcdi_Handle             hMcdi,
	  BVDC_P_ListInfo               *pList )
{
	uint32_t ulRegOffset;
	uint32_t ulDeviceAddr, ulSize, ulCompression, ulFixedRate;
	int ii, ulBufCount;
#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
	uint32_t   ulReminder, ulBitsPerPixel;
#endif

	BDBG_OBJECT_ASSERT(hMcdi, BVDC_MDI);
	ulRegOffset = hMcdi->ulRegOffset;

	/* Buffer inside each group*/
	ulBufCount = BVDC_P_MAD_QM_FIELD_STORE_COUNT / BVDC_P_MAD_QM_BUFFER_COUNT;

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
	ulCompression = (hMcdi->stTestFeature1.ulBitsPerPixel > BVDC_MADR_DCXS_HALFBITS_PER_PIXEL)
		? BCHP_SIOB_0_DCXS_CFG_COMPRESSION_BPP_11 /* 11 bpp */
		: BCHP_SIOB_0_DCXS_CFG_COMPRESSION_BPP_9; /* 09 bpp */

#if (BVDC_P_MADR_VARIABLE_RATE)
	ulFixedRate = BCHP_SIOB_0_DCXS_CFG_FIXED_RATE_Variable;
#else
	ulFixedRate = BCHP_SIOB_0_DCXS_CFG_FIXED_RATE_Fixed;
#endif

	/* set pic size into reg */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_SIOB_0_DCXS_CFG, ulRegOffset,
		BCHP_FIELD_DATA(SIOB_0_DCXS_CFG, ENABLE,       hMcdi->stTestFeature1.bEnable) |
		BCHP_FIELD_ENUM(SIOB_0_DCXS_CFG, APPLY_QERR,   No_Apply                     ) | /* nominal */
		BCHP_FIELD_DATA(SIOB_0_DCXS_CFG, FIXED_RATE,   ulFixedRate                  ) | /* nominal */
		BCHP_FIELD_DATA(SIOB_0_DCXS_CFG, COMPRESSION,  ulCompression                ));

	/* set MADR ENTER_LOCK_LEVEL/EXIT_LOCK_LEVEL*/
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCN_0_IT_FIELD_PHASE_CALC_CONTROL_0, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCN_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_PHASE_MATCH_THRESH, BVDC_P_MADR_REV32_PHASE_MATCH_THRESH) |
		BCHP_FIELD_DATA(MDI_FCN_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_ENTER_LOCK_LEVEL,   BVDC_P_MADR_REV32_ENTER_LOCK_LEVEL  ) |
		BCHP_FIELD_DATA(MDI_FCN_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_EXIT_LOCK_LEVEL ,   BVDC_P_MADR_REV32_EXIT_LOCK_LEVEL));

	/* set MADR REV32_LOCK_SAT_LEVEL to 28*/
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCN_0_IT_FIELD_PHASE_CALC_CONTROL_2, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCN_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_LOCK_SAT_LEVEL,	  BVDC_P_MADR_REV32_LOCK_SAT_LEVEL) |
		BCHP_FIELD_DATA(MDI_FCN_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_BAD_EDIT_LEVEL ,   BVDC_P_MADR_REV32_BAD_EDIT_LEVEL));

	/* set MADR OBTS                      */
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_FCN_0_OBTS_DECAY, ulRegOffset,
		BVDC_P_REGS_ENTRIES(MDI_FCN_0_OBTS_DECAY, MDI_FCN_0_MODE_CONTROL_0));
	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_FCN_0_OBTS_DECAY  , VALUE, BVDC_P_MADR_OBTS_DECAY);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_FCN_0_OBTS_HOLDOFF, VALUE, BVDC_P_MADR_OBTS_HOLDOFF);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_FCN_0_OBTS_MAX_HOLDOFF, VALUE, BVDC_P_MADR_OBTS_MAX_HOLDOFF);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(MDI_FCN_0_OBTS_CONTROL, VIDEO_ENABLE, ON                  ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_OBTS_CONTROL, IT_ENABLE, ON                     ) |
		BCHP_FIELD_DATA(MDI_FCN_0_OBTS_CONTROL, OFFSET, BVDC_P_MCDI_ZERO          ) |
		BCHP_FIELD_DATA(MDI_FCN_0_OBTS_CONTROL, CORE,   BVDC_P_MADR_OBTS_CTRL_CORE);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, PIC_ALWAYS_AVAIL         , OFF             ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, REV32_BAD_EDIT_PHASE_VETO, ON              ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, ALT_ENTER_LOCK_ENABLE    , OFF             ) |
		BCHP_FIELD_DATA(MDI_FCN_0_MODE_CONTROL_0, BUFFER_SEL               , 1               ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_0 , FIELD_STATE_FIFO) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_1 , IT_BLOCK        ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, HARD_START_SEL           , FREEZE_FRAME    ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, FIELD_G_ENABLE           , ON              ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, FIELD_J_ENABLE           , ON              ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, PIXEL_CAP_ENABLE         , ON              ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, GLOBAL_FIELD_L2_ENABLE   , ON              ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE    , ON              ) |
		BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_0, GLOBAL_FIELD_K2_ENABLE   , ON              );

	/* jira SW 7231-110*/
#if (BVDC_P_SUPPORT_MADR_VER >= BVDC_P_MADR_VER_2)
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_OBTS_CONTROL_0, ulRegOffset,
		BCHP_FIELD_ENUM(MDI_PPB_0_OBTS_CONTROL_0, OBTS_CHROMA_FILTER_ENABLE, ON) |
		BCHP_FIELD_ENUM(MDI_PPB_0_OBTS_CONTROL_0, OBTS_FILTER_ENABLE,        ON));
#endif
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_LA_SCALE_0, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_LA_SCALE_0, DEGREE_7, BVDC_P_MADR_LA_SCALE_0_DEGREE) |
		BCHP_FIELD_DATA(MDI_PPB_0_LA_SCALE_0, DEGREE_6, BVDC_P_MADR_LA_SCALE_0_DEGREE) |
		BCHP_FIELD_DATA(MDI_PPB_0_LA_SCALE_0, DEGREE_5, BVDC_P_MADR_LA_SCALE_0_DEGREE) |
		BCHP_FIELD_DATA(MDI_PPB_0_LA_SCALE_0, DEGREE_4, BVDC_P_MADR_LA_SCALE_0_DEGREE) |
		BCHP_FIELD_DATA(MDI_PPB_0_LA_SCALE_0, DEGREE_3, BVDC_P_MADR_LA_SCALE_0_DEGREE) |
		BCHP_FIELD_DATA(MDI_PPB_0_LA_SCALE_0, DEGREE_2, BVDC_P_MADR_LA_SCALE_0_DEGREE) |
		BCHP_FIELD_DATA(MDI_PPB_0_LA_SCALE_0, DEGREE_1, BVDC_P_MADR_LA_SCALE_0_DEGREE) |
		BCHP_FIELD_DATA(MDI_PPB_0_LA_SCALE_0, DEGREE_0, BVDC_P_MADR_LA_SCALE_0_DEGREE));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_XCHROMA_CONTROL_4, ulRegOffset,
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_4, IT_CHROMA_SEL, IT_DIRECT)                    |
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_4, SIMILAR_MAX  , BVDC_P_MADR_XCHROMA_CTRL_MAX) |
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_4, MAX_XCHROMA  , BVDC_P_MADR_XCHROMA_CTRL_MAX));
#else
	BSTD_UNUSED(ulFixedRate);
	BSTD_UNUSED(ulCompression);
#endif

#if (BVDC_P_MADR_HSIZE_WORKAROUND)
	if(hMcdi->ulHSize % 4)
	{
		BDBG_ERR(("BVN hang with bad HSIZE (not multiple of 4): %d",
			hMcdi->ulHSize));
	}
#endif

#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
	ulReminder = BVDC_P_MADR_GET_REMAINDER(hMcdi->ulHSize, hMcdi->ulVSize);
	if(BVDC_P_MADR_BAD_ALIGNMENT(ulReminder))
	{
		BDBG_ERR(("BVN hang with bad MDI_TOP_0_SRC_PIC_SIZE: %dx%d: %d",
			hMcdi->ulHSize, hMcdi->ulVSize, ulReminder));
	}

	ulBitsPerPixel = BVDC_P_MADR_DCXS_COMPRESSION(hMcdi->stTestFeature1.ulBitsPerPixel);
#if (BVDC_P_MADR_VARIABLE_RATE)
	ulReminder = BVDC_P_MADR_GET_VARIABLE_RATE_REMAINDER(hMcdi->ulHSize,
		hMcdi->ulVSize, ulBitsPerPixel);

	if(BVDC_P_MADR_VARIABLE_RATE_BAD_ALIGNMENT(ulReminder, ulBitsPerPixel))
	{
		BDBG_ERR(("BVN hang with bad MDI_TOP_0_SRC_PIC_SIZE for variable rate: %dx%dx%d: %d",
			hMcdi->ulHSize, hMcdi->ulVSize, ulBitsPerPixel, ulReminder));
	}
#else
	ulReminder = BVDC_P_MADR_GET_FIX_RATE_REMAINDER(hMcdi->ulHSize,
		hMcdi->ulVSize, ulBitsPerPixel);

	if(BVDC_P_MADR_FIX_RATE_BAD_ALIGNMENT(ulReminder, ulBitsPerPixel))
	{
		BDBG_WRN(("BVN hang with bad MDI_TOP_0_SRC_PIC_SIZE for fix rate: %dx%dx%d: %d",
			hMcdi->ulHSize, hMcdi->ulVSize, ulBitsPerPixel, ulReminder));
	}
#endif
#endif

	/* set pic size into reg */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_TOP_0_SRC_PIC_SIZE, ulRegOffset,
		BCHP_FIELD_DATA(MDI_TOP_0_SRC_PIC_SIZE, HSIZE, hMcdi->ulHSize) |
		BCHP_FIELD_DATA(MDI_TOP_0_SRC_PIC_SIZE, VSIZE, hMcdi->ulVSize));

	/* set bufs addr into reg */
#if (BVDC_P_MADR_VER_7 <= BVDC_P_SUPPORT_MADR_VER)
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_TOP_0_PIXEL_FIELD_MSTART_0, ulRegOffset,
		BVDC_P_REGS_ENTRIES(MDI_TOP_0_PIXEL_FIELD_MSTART_0, MDI_TOP_0_PIXEL_FIELD_MSTART_3));
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[0]); /* PIXEL_FIELD_MSTART_0 */
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[1]); /* PIXEL_FIELD_MSTART_1 */
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[2]); /* PIXEL_FIELD_MSTART_2 */
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[3]); /* PIXEL_FIELD_MSTART_3 */
#else
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_TOP_0_PIXEL_FIELD_MSTART_0, ulRegOffset,
		BVDC_P_REGS_ENTRIES(MDI_TOP_0_PIXEL_FIELD_MSTART_0, MDI_TOP_0_PIXEL_FIELD_MSTART_2));
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[0]); /* PIXEL_FIELD_MSTART_0 */
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[1]); /* PIXEL_FIELD_MSTART_1 */
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[2]); /* PIXEL_FIELD_MSTART_2 */
#endif
	ulDeviceAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apQmHeapNode[0]);
	ulSize = hMcdi->apQmHeapNode[0]->pHeapInfo->ulBufSize / ulBufCount;

#if (BVDC_P_SUPPORT_MADR)
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_TOP_0_QM_FIELD_MSTART_1, ulRegOffset,
		BVDC_P_REGS_ENTRIES(MDI_TOP_0_QM_FIELD_MSTART_1, MDI_TOP_0_QM_FIELD_MSTART_4));
	for(ii = 0; ii < ulBufCount; ii++)
	{
		*pList->pulCurrent++ = ulDeviceAddr;
		/* Make sure address is 32 byte alligned */
		ulDeviceAddr += ulSize;
		ulDeviceAddr = (ulDeviceAddr + 31) & BVDC_P_QM_ADDR_MASK;
	}
#else
	BSTD_UNUSED(ii);
#endif
}


/***************************************************************************
* {private}
*  MCDI src Init
*/
static void BVDC_P_Mcdi_BuildRul_Mcdi_SrcInit_isr
	( BVDC_P_Mcdi_Handle             hMcdi,
	  BVDC_P_ListInfo               *pList )
{
	uint32_t ulRegOffset;
	uint32_t ulDeviceAddr, ulSize, ulModeCtrl;
	int ii, ulBufCount;
	int ulRegNum;
	bool bIsHD=false;
#if (BVDC_P_SUPPORT_MCDI_VER != 0)
	BVDC_Deinterlace_ChromaSettings *pChromaSettings = hMcdi->pChromaSettings;
#endif
#if (BVDC_P_MCDI_VER_2 <= BVDC_P_SUPPORT_MCDI_VER)
	uint32_t ulData;
#endif

	BDBG_OBJECT_ASSERT(hMcdi, BVDC_MDI);
	ulRegOffset = hMcdi->ulRegOffset;

	/* Buffer inside each group*/
	ulBufCount = BVDC_P_MCDI_QM_FIELD_STORE_COUNT / BVDC_P_MCDI_QM_BUFFER_COUNT;

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
#if (BVDC_P_SUPPORT_MCDI_VER)
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_SIOB_0_DCX_PRED_CFG, ulRegOffset,
		BVDC_P_REGS_ENTRIES(SIOB_0_DCX_PRED_CFG, SIOB_0_DCX_COMPR_CFG1));

	/* SIOB_0_DCX_PRED_CFG */
	*pList->pulCurrent++ = (
		BCHP_FIELD_DATA(SIOB_0_DCX_PRED_CFG, ENABLE,          hMcdi->stTestFeature1.bEnable    ) |
		BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, CONVERT_RGB,     Disable ) |
		BCHP_FIELD_DATA(SIOB_0_DCX_PRED_CFG, PREDICTION_MODE, hMcdi->stTestFeature1.ulPredictionMode) |
		BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, EDGE_PRED_ENA,   Enable  ) |
		BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, LEFT_PRED_ENA,   Enable  ) |
		BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, ABCD_PRED_ENA,   Enable  ) |
		BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, LS_PRED_ENA,     Enable  ));

	/* SIOB_0_DCX_COMPR_CFG1 */
	*pList->pulCurrent++ = (
		BCHP_FIELD_DATA(SIOB_0_DCX_COMPR_CFG1, PIXELS_PER_GROUP,  hMcdi->stTestFeature1.ulPixelPerGroup) |
		BCHP_FIELD_DATA(SIOB_0_DCX_COMPR_CFG1, TGT_OFFSET_HI,                                      0xfa) |
		BCHP_FIELD_DATA(SIOB_0_DCX_COMPR_CFG1, TGT_OFFSET_LO,                                        12) |
		BCHP_FIELD_DATA(SIOB_0_DCX_COMPR_CFG1, TGT_BPG,           hMcdi->stTestFeature1.ulBitsPerGroup));
#endif
#endif

	/* set pic size into reg */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_TOP_0_SRC_PIC_SIZE, ulRegOffset,
		BCHP_FIELD_DATA(MDI_TOP_0_SRC_PIC_SIZE, HSIZE, hMcdi->ulHSize) |
		BCHP_FIELD_DATA(MDI_TOP_0_SRC_PIC_SIZE, VSIZE, hMcdi->ulVSize));
	bIsHD = (hMcdi->ulHSize >= BFMT_720P_WIDTH);

#if (BVDC_P_SUPPORT_MCDI_VER == BVDC_P_MCDI_VER_0)
	ulRegNum = BVDC_P_REGS_ENTRIES(MDI_TOP_0_PIXEL_FIELD_MSTART_0, MDI_TOP_0_PIXEL_FIELD_MSTART_2);
#else
	ulRegNum = BVDC_P_REGS_ENTRIES(MDI_TOP_0_PIXEL_FIELD_MSTART_0, MDI_TOP_0_PIXEL_FIELD_MSTART_3);
#endif

	/* set bufs addr into reg */
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_TOP_0_PIXEL_FIELD_MSTART_0, ulRegOffset, ulRegNum);
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[0]); /* PIXEL_FIELD_MSTART_0 */
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[1]); /* PIXEL_FIELD_MSTART_1 */
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[2]); /* PIXEL_FIELD_MSTART_2 */
#if (BVDC_P_SUPPORT_MCDI_VER != 0)
	*pList->pulCurrent++ = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apHeapNode[3]); /* PIXEL_FIELD_MSTART_2 */
#endif


#if (BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_3)
	ulModeCtrl =
		BCHP_FIELD_ENUM(MDI_FCB_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE, ON) |
		BCHP_FIELD_ENUM(MDI_FCB_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE, ON) |
		BCHP_FIELD_ENUM(MDI_FCB_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE, ON);

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_MODE_CONTROL_0, ulRegOffset, ulModeCtrl);
#endif

#if (BVDC_P_SUPPORT_MCDI_VER < BVDC_P_MCDI_VER_3)
	BSTD_UNUSED(ulModeCtrl);
#endif

	ulDeviceAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(
		hMcdi->apQmHeapNode[0]);
	ulSize = hMcdi->apQmHeapNode[0]->pHeapInfo->ulBufSize / ulBufCount;

#if (BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_2)
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_TOP_0_QM_FIELD_MSTART_1, ulRegOffset,
		BVDC_P_REGS_ENTRIES(MDI_TOP_0_QM_FIELD_MSTART_1, MDI_TOP_0_QM_FIELD_MSTART_4));
	for(ii = 0; ii < ulBufCount; ii++)
	{
		*pList->pulCurrent++ = ulDeviceAddr;
		/* Make sure address is 32 byte alligned */
		ulDeviceAddr += ulSize;
		ulDeviceAddr = (ulDeviceAddr + 31) & (BVDC_P_QM_ADDR_MASK);
	}

#if (BVDC_P_SUPPORT_MCDI_VER > BVDC_P_MCDI_VER_2)
	ulDeviceAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hMcdi->apQmHeapNode[1]);
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_TOP_0_QM_FIELD_MSTART_5, ulRegOffset,
		BVDC_P_REGS_ENTRIES(MDI_TOP_0_QM_FIELD_MSTART_5, MDI_TOP_0_QM_FIELD_MSTART_0));

	for(ii = 0; ii < ulBufCount; ii++)
	{
		*pList->pulCurrent++ = ulDeviceAddr;
		/* Make sure address is 32 byte alligned */
		ulDeviceAddr += ulSize;
		ulDeviceAddr = (ulDeviceAddr + 31) & (BVDC_P_QM_ADDR_MASK);
	}
#endif
#else
	BSTD_UNUSED(ii);
#endif

	/* setting ALT_REPF_VETO_LEVEL_ENABLE to 1, due to HW-7420:
	* Low angle test stream "3 lines jaggies" showes broken jaggies once in a while */
#if (BVDC_P_SUPPORT_MCDI_VER != 0)
#if (BVDC_P_SUPPORT_MCDI_VER==1)
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_5, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_5, REV22_LOCK_SAT_LEVEL,        32) |
		BCHP_FIELD_DATA(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_5, PCC_NONMATCH_MATCH_RATIO,     8) |
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_5, ALT_ENTER_LOCK_ENABLE,      OFF) |
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_5, ALT_REPF_VETO_LEVEL_ENABLE,  ON) |
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_5, REV32_BAD_EDIT_PHASE_VETO,   ON) |
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_5, BAD_EDIT_DETECT_ENABLE,      ON) |
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_5, BAD_WEAVE_DETECT_ENABLE,     ON));
#endif

	/* setting according to Jim Tseng's email:
	MDI_PPB_0_XCHROMA_CONTROL_1.PIC_422_MA = 0 (MA_CHROMA)
	MDI_PPB_0_XCHROMA_CONTROL_1.PIC_422_IT = 0 (MA_CHROMA)
	MDI_PPB_0_XCHROMA_CONTROL_1.FIELD_420_MA = 0 (MA_CHROMA)
	MDI_PPB_0_XCHROMA_CONTROL_1.FIELD_420_IT = 0 (MA_CHROMA)
	MDI_PPB_0_XCHROMA_CONTROL_1.FRAME_420_MA = 0 (MA_CHROMA)
	MDI_PPB_0_XCHROMA_CONTROL_1.FRAME_420_IT = 0 (MA_CHROMA)
	*/

	/* MCDI Golden value setting   */
	/* FCB Module */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_MC_DECISION_CONTROL_0, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_MC_DECISION_CONTROL_0, PCC_TOTAL_THRESH, BVDC_P_MCDI_FCB_MC_CTRL_PCC_TOTAL_THRESH));

#if (BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_2)

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_1, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_1, REV22_LOCK_SAT_LEVEL    ,32) |
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_1, PCC_NONMATCH_MATCH_RATIO, 6));

	/* BLOCK: +++ */
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, ulRegOffset,
		BVDC_P_REGS_ENTRIES(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_2));

	/* set MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0*/
	*pList->pulCurrent++ = (
#if (BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_5)
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_BW_STAIR_ENABLE,                   ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_BW_FEATH_ENABLE,                   ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_BW_FEATH_ENABLE,                   ON)|
#endif
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_BW_PCC_INC_ENABLE,                 ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_SAME_FRAME_ENABLE,                OFF)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_BW_LG_MAX_ENABLE,                  ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_BW_LG_FF_ENABLE,                   ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_BW_LG_ENABLE,                      ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_BW_LG_FF_ENABLE,                   ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_BW_LG_ENABLE,                      ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_NOINC_ENABLE,                      ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REPF_FL_ENABLE,                          ON)|
		BCHP_FIELD_DATA(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, USE_RANGE_PCC,             BVDC_P_MCDI_ZERO)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, ALT_ENTER_LOCK_ENABLE,                   ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, ALT_REPF_VETO_LEVEL_ENABLE,              ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_BAD_EDIT_PHASE_VETO,               ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, BAD_EDIT_DETECT_ENABLE,                  ON)|
		BCHP_FIELD_ENUM(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_0, BAD_WEAVE_DETECT_ENABLE,                 ON));

	/* set MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_1*/
	ulData = bIsHD?
	   (BCHP_FIELD_DATA(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_VETO_LEVEL,         BVDC_P_MCDI_REV32_REPF_VETO_LEVEL_HD) |
		BCHP_FIELD_DATA(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_PIX_CORRECT_LEVEL,   BVDC_P_MCDI_REV32_REPF_PIX_LEVEL_HD)):
	   (BCHP_FIELD_DATA(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_VETO_LEVEL,         BVDC_P_MCDI_REV32_REPF_VETO_LEVEL_SD) |
		BCHP_FIELD_DATA(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_PIX_CORRECT_LEVEL,   BVDC_P_MCDI_REV32_REPF_PIX_LEVEL_SD));
	*pList->pulCurrent++ = ulData;

	/* set MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_2*/
	ulData = bIsHD? BCHP_FIELD_DATA(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_BAD_EDIT_LEVEL, BVDC_P_MCDI_REV32_BAD_EDIT_LEVEL_HD):
	                BCHP_FIELD_DATA(MDI_FCB_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_BAD_EDIT_LEVEL, BVDC_P_MCDI_REV32_BAD_EDIT_LEVEL_SD);
	*pList->pulCurrent++ = ulData;
	/* BLOCK: --- */

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV32_IT_FIELD_PHASE_CALC_CONTROL_4, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV32_IT_FIELD_PHASE_CALC_CONTROL_4, REPF_FL_RATIO, BVDC_P_MCDI_REV32_REPF_FL_RATIO)|
		BCHP_FIELD_DATA(MDI_FCB_0_REV32_IT_FIELD_PHASE_CALC_CONTROL_4, REPF_FL_MIN,   BVDC_P_MCDI_REV32_REPF_FL_MIN));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV32_IT_FIELD_PHASE_CALC_CONTROL_6, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV32_IT_FIELD_PHASE_CALC_CONTROL_6, REV32_BW_LG_PCC_RATIO, BVDC_P_MCDI_REV32_BW_LG_PCC_RATIO)|
		BCHP_FIELD_DATA(MDI_FCB_0_REV32_IT_FIELD_PHASE_CALC_CONTROL_6, REV32_BW_LG_PCC_MIN,   BVDC_P_MCDI_REV32_BW_LG_PCC_MIN));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV32_IT_FIELD_PHASE_CALC_CONTROL_4_MCDI, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV32_IT_FIELD_PHASE_CALC_CONTROL_4_MCDI, REPF_FL_RATIO, BVDC_P_MCDI_REV32_REPF_FL_RATIO)|
		BCHP_FIELD_DATA(MDI_FCB_0_REV32_IT_FIELD_PHASE_CALC_CONTROL_4_MCDI, REPF_FL_MIN,   BVDC_P_MCDI_REV32_REPF_FL_MIN));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_0, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_NONMATCH_MATCH_RATIO, BVDC_P_MCDI_REV22_NONMATCH_MATCH_RATIO)|
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_ENTER_LOCK_LEVEL,         BVDC_P_MCDI_REV22_ENTER_LOCK_LEVEL)|
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_0, REV22_EXIT_LOCK_LEVEL,           BVDC_P_MCDI_REV22_EXIT_LOCK_LEVEL));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_3, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_3, MIN_USABLE_PCC,           BVDC_P_MCDI_REV22_MIN_USABLE_PCC)|
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_3, PW_MATCH_MULTIPLIER, BVDC_P_MCDI_REV22_PW_MATCH_MULTIPLIER));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_3_MCDI, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_3_MCDI, MIN_USABLE_PCC,           BVDC_P_MCDI_REV22_MIN_USABLE_PCC_MCDI)|
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_3_MCDI, PW_MATCH_MULTIPLIER, BVDC_P_MCDI_REV22_PW_MATCH_MULTIPLIER_MCDI));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_8, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_8, REV22_ALMOST_LOCK_LEVEL, BVDC_P_MCDI_REV22_ALMOST_LOCK_LEVEL));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_9, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_9, REV22_BW_LG_PCC_RATIO, BVDC_P_MCDI_REV22_BW_LG_PCC_RATIO)|
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_9, REV22_BW_LG_PCC_MIN,   BVDC_P_MCDI_REV22_BW_LG_PCC_MIN));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_11, ulRegOffset,
		BCHP_FIELD_DATA(MDI_FCB_0_REV22_IT_FIELD_PHASE_CALC_CONTROL_11, REV22_BW_LG_PCC_MAXIMUM, BVDC_P_MCDI_REV22_BW_LG_PCC_MAXIMUM));

	/* Bad weave */
	ulData = bIsHD ?
		(BCHP_FIELD_DATA(MDI_FCB_0_BWV_CONTROL_1, LUMA_32_THRESH,         BVDC_P_MCDI_BWV_LUMA32_THD_HD) |
		 BCHP_FIELD_DATA(MDI_FCB_0_BWV_CONTROL_1, LUMA_32_AVG_THRESH, BVDC_P_MCDI_BWV_LUMA32_AVG_THD_HD)):
		(BCHP_FIELD_DATA(MDI_FCB_0_BWV_CONTROL_1, LUMA_32_THRESH,         BVDC_P_MCDI_BWV_LUMA32_THD_SD) |
		 BCHP_FIELD_DATA(MDI_FCB_0_BWV_CONTROL_1, LUMA_32_AVG_THRESH, BVDC_P_MCDI_BWV_LUMA32_AVG_THD_SD));


	/* MDI_FCB_0_BWV_CONTROL_3 */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_BWV_CONTROL_1, ulRegOffset, ulData);

	/* MDI_FCB_0_BWV_CONTROL_3_MCDI */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_BWV_CONTROL_1_MCDI, ulRegOffset, ulData);


	ulData = BCHP_FIELD_DATA(MDI_FCB_0_BWV_CONTROL_3, TKR_PCC_MULT,            BVDC_P_MCDI_BWV_TKR_PCC_MULT) |
			(bIsHD ? BCHP_FIELD_DATA(MDI_FCB_0_BWV_CONTROL_3, TKR_MIN_REPF_VETO_LEVEL, BVDC_P_MCDI_BWV_TKR_VETO_LEVEL_HD) :
			         BCHP_FIELD_DATA(MDI_FCB_0_BWV_CONTROL_3, TKR_MIN_REPF_VETO_LEVEL, BVDC_P_MCDI_BWV_TKR_VETO_LEVEL_SD));

	/* MDI_FCB_0_BWV_CONTROL_3 */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_BWV_CONTROL_3, ulRegOffset, ulData);

	/* MDI_FCB_0_BWV_CONTROL_3_MCDI */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_BWV_CONTROL_3_MCDI, ulRegOffset, ulData);

	/* PPB Module */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MC_BLEND, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MC_BLEND, K0, BVDC_P_MCDI_MC_BLEND_K0));

#if (BVDC_P_MCDI_VER_3 == BVDC_P_SUPPORT_MCDI_VER )
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MEMC_CTRL, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MEMC_CTRL, SCRATCH,                           BVDC_P_MCDI_ZERO)|
		BCHP_FIELD_DATA(MDI_PPB_0_MEMC_CTRL, USE_IT_4MCDI,                       BVDC_P_MCDI_ONE)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, CHROMA_COST_ENABLE,                             OFF)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, FORCE_WEAVE,                                    OFF)|
		BCHP_FIELD_DATA(MDI_PPB_0_MEMC_CTRL, LAMDA,                  BVDC_P_MCDI_MEMC_CTRL_LAMDA));
#endif

#if (BVDC_P_MCDI_VER_4 <= BVDC_P_SUPPORT_MCDI_VER )
	/* MDI_FCB_0_SCENE_CHANGE_CTRL */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_SCENE_CHANGE_CTRL, ulRegOffset,
		(BCHP_FIELD_ENUM(MDI_FCB_0_SCENE_CHANGE_CTRL, SCENE_CHANGE_ENABLE,								 ON) |
		 BCHP_FIELD_DATA(MDI_FCB_0_SCENE_CHANGE_CTRL, SCENE_CHANGE_THRESHOLD,			 BVDC_P_MCDI_SC_THD) |
		 BCHP_FIELD_DATA(MDI_FCB_0_SCENE_CHANGE_CTRL, SCENE_CHANGE_COUNTER_PRE_THR, 		BVDC_P_MCDI_ONE) |
		 BCHP_FIELD_DATA(MDI_FCB_0_SCENE_CHANGE_CTRL, SCENE_CHANGE_COUNTER_POST_THR, BVDC_P_MCDI_SC_PST_THD) |
		 BCHP_FIELD_DATA(MDI_FCB_0_SCENE_CHANGE_CTRL, SCENE_CHANGE_POST_THR,			BVDC_P_MCDI_SC_THD)));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MEMC_CTRL, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MEMC_CTRL, SCRATCH,							BVDC_P_MCDI_ZERO)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, ME_3D_REINIT_BIAS_AT_LRVIEW_EDGE,				  ON)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, INT_SOURCE_STEP3_ENABLE,						  ON)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, STEP3_ENABLE,									  ON)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, STEP1_ENABLE,									  ON)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, ME_3D_LIMIT_1D,								  ON)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, GMV_IS_ATTRACTOR_AT_EDGE,						  ON)|
		BCHP_FIELD_DATA(MDI_PPB_0_MEMC_CTRL, USE_IT_4MCDI,						 BVDC_P_MCDI_ONE)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, CHROMA_COST_ENABLE,							 OFF)|
		BCHP_FIELD_ENUM(MDI_PPB_0_MEMC_CTRL, FORCE_WEAVE,									 OFF)|
		BCHP_FIELD_DATA(MDI_PPB_0_MEMC_CTRL, LAMDA, 				 BVDC_P_MCDI_MEMC_CTRL_LAMDA));
#endif
#endif

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MC_COST_CTRL_02, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_02, SCRATCH,                        BVDC_P_MCDI_ZERO)|
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_02, PCC_CNT_THD_1,         BVDC_P_MCDI_PCC_CNT_THD_1)|
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_02, PCC_CNT_THD_0,         BVDC_P_MCDI_PCC_CNT_THD_0)|
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_02, PCC_CNT_THD,             BVDC_P_MCDI_PCC_CNT_THD)|
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_02, ZERO_PCC_CORE_THD, BVDC_P_MCDI_ZERO_PCC_CORE_THD)|
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_02, MC_PCC_CORE_THD,     BVDC_P_MCDI_MC_PCC_CORE_THD));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MC_COST_CTRL_04, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_04, SCRATCH,    BVDC_P_MCDI_ZERO)|
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_04, EDGE_K2, BVDC_P_MCDI_EDGE_K2)|
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_04, EDGE_K1, BVDC_P_MCDI_EDGE_K1)|
		BCHP_FIELD_DATA(MDI_PPB_0_MC_COST_CTRL_04, EDGE_K0, BVDC_P_MCDI_EDGE_K0));

	ulRegNum = BVDC_P_REGS_ENTRIES(MDI_PPB_0_MC_MC_THD, MDI_PPB_0_MC_VM_THD);
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_PPB_0_MC_MC_THD, ulRegOffset, ulRegNum);

	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_PPB_0_MC_MC_THD, MC_THD_HIGH, BVDC_P_MCDI_MC_MC_THD_HIGH)|
							BCHP_FIELD_DATA(MDI_PPB_0_MC_MC_THD, MC_THD_LOW, BVDC_P_MCDI_MC_MC_THD_LOW);

	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_PPB_0_MC_ZERO_THD_0,  ZERO_THD_LOW_1, BVDC_P_MCDI_ZERO_THD_LOW_1)|
							BCHP_FIELD_DATA(MDI_PPB_0_MC_ZERO_THD_0, ZERO_THD_LOW_0, BVDC_P_MCDI_ZERO_THD_LOW_0);

	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_PPB_0_MC_ZERO_THD_1,  ZERO_THD_HIGH,  BVDC_P_MCDI_ZERO_THD_HIGH);

	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_PPB_0_MC_SHIFT_MC_THD,  SHIFT_MC_THD_1, BVDC_P_MCDI_SHIFT_MC_THD_1)|
							BCHP_FIELD_DATA(MDI_PPB_0_MC_SHIFT_MC_THD, SHIFT_MC_THD_0, BVDC_P_MCDI_SHIFT_MC_THD_0);

	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_PPB_0_MC_EDGE_THD_0,  EDGE_THD_LOW_1, BVDC_P_MCDI_EDGE_THD_LOW_1)|
							BCHP_FIELD_DATA(MDI_PPB_0_MC_EDGE_THD_0, EDGE_THD_LOW_0, BVDC_P_MCDI_EDGE_THD_LOW_0);

	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_PPB_0_MC_EDGE_THD_1,  EDGE_THD_HIGH_1, BVDC_P_MCDI_EDGE_THD_HIGH_1)|
							BCHP_FIELD_DATA(MDI_PPB_0_MC_EDGE_THD_1, EDGE_THD_HIGH_0, BVDC_P_MCDI_EDGE_THD_HIGH_0);

	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_PPB_0_MC_EDGE_THD_2,  EDGE_THD_HIGH_2, BVDC_P_MCDI_EDGE_THD_HIGH_2);

	*pList->pulCurrent++ = BCHP_FIELD_DATA(MDI_PPB_0_MC_VM_THD, VM_THD_1, BVDC_P_MCDI_VM_THD_1)|
							BCHP_FIELD_DATA(MDI_PPB_0_MC_VM_THD, VM_THD_0, BVDC_P_MCDI_VM_THD_0);

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MC_SHIFT_ZERO_THD, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MC_SHIFT_ZERO_THD,  SHIFT_ZERO_THD, BVDC_P_MCDI_SHIFT_ZERO_THD));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MC_MOVIE_MC_THD, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MC_MOVIE_MC_THD,  MOVIE_MC_THD, BVDC_P_MCDI_MOVIE_MC_THD));

#if (BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_2)
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_START_IT_LINE, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_START_IT_LINE, VALUE, hMcdi->ulVSize - 1));
#endif

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_XCHROMA_CONTROL_0, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_0, CHROMA_MOTION_ENABLE,           pChromaSettings->eChroma422MotionAdaptiveMode )|
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_0, CHROMA_422_MOTION_MODE,         pChromaSettings->eChroma422MotionMode         )|
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_0, CHROMA_FIELD_420_MOTION_MODE,   pChromaSettings->eChroma420MotionMode         )|
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_0, IT_PPRFM_CR_ENABLE,             ON                                            )|
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_0, IT_PPUFM_CR_ENABLE,             ON                                            )|
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_0, CHROMA_422_MA_EDGE_DET_MODE,    pChromaSettings->bChromaField420EdgeDetMode   )|
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_0, CHROMA_FIELD_420_INITIAL_PHASE, pChromaSettings->bChromaField420InitPhase     )|
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_0, CHROMA_FIELD_420_INV_METHOD,    pChromaSettings->eChromaField420InvMethod     ));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_XCHROMA_CONTROL_1, ulRegOffset,
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_1, PIC_422_MA,   MA_CHROMA)|
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_1, PIC_422_IT,   IT_DIRECT)|
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_1, FIELD_420_MA,   INT_420)|
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_1, FIELD_420_IT,   INT_420)|
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_1, FRAME_420_MA, MA_CHROMA)|
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_1, FRAME_420_IT, IT_DIRECT));

#if (BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_2)
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MVD_K1_VALUE, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MVD_K1_VALUE, SMALL, BVDC_P_MVD_K1_VALUE));
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_XCHROMA_CONTROL_6, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_6, MA_420_422_BLEND_STRENGTH,   BVDC_P_MCDI_MA_420422_BLEND_STRENGTH)|
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_6, IT_420_422_BLEND_STRENGTH,   BVDC_P_MCDI_IT_420422_BLEND_STRENGTH)|
		BCHP_FIELD_ENUM(MDI_PPB_0_XCHROMA_CONTROL_6, IT_PPREG_CR_ENABLE,                                            ON)|
		BCHP_FIELD_DATA(MDI_PPB_0_XCHROMA_CONTROL_6, IT_420_422_BLEND_MODE,                            BVDC_P_MCDI_ONE));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_GMV_NUM_THD, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_GMV_NUM_THD, VALUE,                             BVDC_P_MCDI_GMV_NUM_THD));
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MH_MAPPING_VALUE, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MH_MAPPING_VALUE, VALUE_3, BVDC_P_MCDI_MH_MAPPING_VALUE_3)|
		BCHP_FIELD_DATA(MDI_PPB_0_MH_MAPPING_VALUE, VALUE_2, BVDC_P_MCDI_MH_MAPPING_VALUE_2)|
		BCHP_FIELD_DATA(MDI_PPB_0_MH_MAPPING_VALUE, VALUE_1, BVDC_P_MCDI_MH_MAPPING_VALUE_1)|
		BCHP_FIELD_DATA(MDI_PPB_0_MH_MAPPING_VALUE, VALUE_0,               BVDC_P_MCDI_ZERO));

#if (BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_3)

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MH_MAPPING_VALUE_STATIONARY, ulRegOffset,
		BCHP_FIELD_DATA(MDI_PPB_0_MH_MAPPING_VALUE_STATIONARY, VALUE_3, BVDC_P_MCDI_MH_MAPPING_VALUE_3)|
		BCHP_FIELD_DATA(MDI_PPB_0_MH_MAPPING_VALUE_STATIONARY, VALUE_2, BVDC_P_MCDI_MH_MAPPING_VALUE_2)|
		BCHP_FIELD_DATA(MDI_PPB_0_MH_MAPPING_VALUE_STATIONARY, VALUE_1, BVDC_P_MCDI_MH_MAPPING_VALUE_1)|
		BCHP_FIELD_DATA(MDI_PPB_0_MH_MAPPING_VALUE_STATIONARY, VALUE_0,               BVDC_P_MCDI_ZERO));


	ulRegNum = BVDC_P_REGS_ENTRIES(MDI_PPB_0_MA_MC_BLEND_CTRL_0, MDI_PPB_0_MA_MC_BLEND_CTRL_1);
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MDI_PPB_0_MA_MC_BLEND_CTRL_0, ulRegOffset, ulRegNum);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MDI_PPB_0_MA_MC_BLEND_CTRL_0, STAT_THRESH_GMV_CHOSEN,      BVDC_P_MCDI_STAT_THRESH_GMV_CHOSEN)|
		BCHP_FIELD_DATA(MDI_PPB_0_MA_MC_BLEND_CTRL_0, STAT_THRESH_GMV_NOTCHOSEN,   BVDC_P_MCDI_STAT_THRESH_GMV_CHOSEN)|
		BCHP_FIELD_DATA(MDI_PPB_0_MA_MC_BLEND_CTRL_0, STAT_THRESH,                            BVDC_P_MCDI_STAT_THRESH);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MDI_PPB_0_MA_MC_BLEND_CTRL_1, STAT_SCALE_GMV_CHOSEN,	    BVDC_P_MCDI_STAT_SCALE_GMV_CHOSEN)|
		BCHP_FIELD_DATA(MDI_PPB_0_MA_MC_BLEND_CTRL_1, STAT_SCALE_GMV_NOTCHOSEN,     BVDC_P_MCDI_STAT_SCALE_GMV_CHOSEN)|
		BCHP_FIELD_DATA(MDI_PPB_0_MA_MC_BLEND_CTRL_1, STAT_SCALE,                              BVDC_P_MCDI_STAT_SCALE);
#endif
#endif
#endif
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Mcdi_BuildRul_SrcInit_isr
	( BVDC_P_Mcdi_Handle             hMcdi,
	  BVDC_P_ListInfo               *pList )
{
	if(hMcdi->bMadr)
	{
		BVDC_P_Mcdi_BuildRul_Madr_SrcInit_isr(hMcdi, pList);
	}
	else
	{
		BVDC_P_Mcdi_BuildRul_Mcdi_SrcInit_isr(hMcdi, pList);
	}

	return;
}


/***************************************************************************
* {private}
*
*/
/* TODO:  for 7420 ANR is after HSCL, not before, take care pic rect seeting,
* how to hard-start delay to scl left, top? */
void BVDC_P_Mcdi_BuildRul_SetEnable_isr
	( BVDC_P_Mcdi_Handle             hMcdi,
	  BVDC_P_ListInfo               *pList,
	  bool                           bEnable,
	  bool                           bRepeat,
	  BAVC_Polarity                  eSrcNxtFldId,
	  BAVC_FrameRateCode             eFrameRate,
	  BFMT_Orientation               eOrientation,
	  bool                           bInit )
{
	uint32_t  ulRegOffset;
	uint32_t  ulModeSel, ulFldType, ulTrickModeSel, ulXmaEnum, ulXmaMode, ulWeaveMode, ulQMRange;

	BDBG_OBJECT_ASSERT(hMcdi, BVDC_MDI);
	ulRegOffset = hMcdi->ulRegOffset;

	if (bEnable)
	{
		bool bRev32Pulldown;
		uint32_t ulModeCtrl0 = 0;

		bRev32Pulldown = (
			(hMcdi->bRev32Pulldown) &&
			(BAVC_FrameRateCode_e25 != eFrameRate));

		/* due to HW requirement, FIELD_FREEZE can not be set for the first 4 fields */
		if (bInit || hMcdi->stSwDirty.stBits.bSize)
		{
			hMcdi->usTrickModeStartDelay = BVDC_P_MCDI_TRICK_MODE_START_DELAY;
		}

#if (BVDC_P_SUPPORT_MCDI_VER != 0)
		if(!hMcdi->bMadr)
		{
			ulModeSel = (bInit || hMcdi->stSwDirty.stBits.bSize)?
				BCHP_FIELD_ENUM(MDI_FCB_0_MODE_CONTROL_1, MODE_SEL, HARD_START) :
				BCHP_FIELD_ENUM(MDI_FCB_0_MODE_CONTROL_1, MODE_SEL, NORMAL);

			ulTrickModeSel = (bRepeat && (0 == hMcdi->usTrickModeStartDelay)) ?
				BCHP_FIELD_ENUM(MDI_FCB_0_MODE_CONTROL_1, TRICK_MODE_SEL, FIELD_FREEZE) :
				BCHP_FIELD_ENUM(MDI_FCB_0_MODE_CONTROL_1, TRICK_MODE_SEL, OFF);

			ulFldType = (BAVC_Polarity_eBotField == eSrcNxtFldId)?
				BCHP_FIELD_ENUM(MDI_FCB_0_MODE_CONTROL_1, FIELD_D_TYPE, BOTTOM) :
				BCHP_FIELD_ENUM(MDI_FCB_0_MODE_CONTROL_1, FIELD_D_TYPE, TOP);
			ulXmaEnum = (BVDC_P_ChromaType_eChroma422 == hMcdi->eChromaType)?
				BCHP_MDI_FCB_0_MODE_CONTROL_1_CHROMA_MODE_SEL_CHROMA_422:
				((BVDC_P_ChromaType_eField420 == hMcdi->eChromaType) ?
				BCHP_MDI_FCB_0_MODE_CONTROL_1_CHROMA_MODE_SEL_CHROMA_FIELD_420 :
				BCHP_MDI_FCB_0_MODE_CONTROL_1_CHROMA_MODE_SEL_CHROMA_FRAME_420);
			ulXmaMode = BCHP_FIELD_DATA(MDI_FCB_0_MODE_CONTROL_1, CHROMA_MODE_SEL, ulXmaEnum);

			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_MODE_CONTROL_1, ulRegOffset,
				ulModeSel | ulFldType | ulTrickModeSel|ulXmaMode);
		}
#else
		BSTD_UNUSED(ulXmaMode);
		BSTD_UNUSED(ulXmaEnum);
#endif

#if ((BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_2) || BVDC_P_SUPPORT_MADR)
		ulModeSel = (bInit || hMcdi->stSwDirty.stBits.bSize)?
			BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_1, MODE_SEL, HARD_START) :
			BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_1, MODE_SEL, NORMAL);

		ulTrickModeSel = (bRepeat && (0 == hMcdi->usTrickModeStartDelay)) ?
			BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_1, TRICK_MODE_SEL, FIELD_FREEZE) :
			BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_1, TRICK_MODE_SEL, OFF);

		ulFldType = (BAVC_Polarity_eBotField == eSrcNxtFldId)?
			BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_1, FIELD_B_TYPE, BOTTOM) :
			BCHP_FIELD_ENUM(MDI_FCN_0_MODE_CONTROL_1, FIELD_B_TYPE, TOP);

		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCN_0_MODE_CONTROL_1, ulRegOffset,
			ulModeSel | ulFldType | ulTrickModeSel);
		ulWeaveMode = BCHP_FIELD_ENUM(MDI_PPB_0_MOTION_CAL_CONTROL, WEAVE_MOTION_ENABLE, ON)   |
					BCHP_FIELD_ENUM(MDI_PPB_0_MOTION_CAL_CONTROL, MA_SUBSAMPLING_ENABLE, ON)   |
					BCHP_FIELD_DATA(MDI_PPB_0_MOTION_CAL_CONTROL, WEAVE_DETECT_THR,      0x10) |
					BCHP_FIELD_DATA(MDI_PPB_0_MOTION_CAL_CONTROL, WEAVE_MOTION_THR_LOW,  0x16);

		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_MOTION_CAL_CONTROL, ulRegOffset,
			ulWeaveMode);

		if(hMcdi->bMadr)
		{
			ulQMRange = BCHP_FIELD_DATA(MDI_PPB_0_QM_MAPPING_RANGE, VALUE_3, BVDC_P_MADR_QM_MAPPING_RANGE_VALUE_3)  |
				        BCHP_FIELD_DATA(MDI_PPB_0_QM_MAPPING_RANGE, VALUE_2, BVDC_P_MADR_QM_MAPPING_RANGE_VALUE_2)  |
				        BCHP_FIELD_DATA(MDI_PPB_0_QM_MAPPING_RANGE, VALUE_1, BVDC_P_MADR_QM_MAPPING_RANGE_VALUE_1);
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_PPB_0_QM_MAPPING_RANGE, ulRegOffset,
				ulQMRange);
		}
#else
		BSTD_UNUSED(ulWeaveMode);
		BSTD_UNUSED(ulQMRange);
#endif

#if (BVDC_P_SUPPORT_MCDI_VER != 0)
		if(!hMcdi->bMadr)
		{
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCB_0_IT_OUTPUT_CONTROL, ulRegOffset,
				((hMcdi->bRev22Pulldown)
				? BCHP_FIELD_ENUM(MDI_FCB_0_IT_OUTPUT_CONTROL, AUTOREV22_ENABLE,ON)
				: BCHP_FIELD_ENUM(MDI_FCB_0_IT_OUTPUT_CONTROL, AUTOREV22_ENABLE,OFF)) |
				((bRev32Pulldown)
				? BCHP_FIELD_ENUM(MDI_FCB_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE,ON)
				: BCHP_FIELD_ENUM(MDI_FCB_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE,OFF)) |
				BCHP_FIELD_ENUM(MDI_FCB_0_IT_OUTPUT_CONTROL, PPUFM_MODE, AUTO)|
				BCHP_FIELD_ENUM(MDI_FCB_0_IT_OUTPUT_CONTROL, PPRFM_MODE, ON));
		}
#endif

#if ((BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_2) || BVDC_P_SUPPORT_MADR)
		if(hMcdi->bMadr)
		{
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_FCN_0_IT_OUTPUT_CONTROL, ulRegOffset,
				((hMcdi->bRev22Pulldown)
				? BCHP_FIELD_ENUM(MDI_FCN_0_IT_OUTPUT_CONTROL, AUTOREV22_ENABLE,ON)
				: BCHP_FIELD_ENUM(MDI_FCN_0_IT_OUTPUT_CONTROL, AUTOREV22_ENABLE,OFF)) |
				((bRev32Pulldown)
				? BCHP_FIELD_ENUM(MDI_FCN_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE,ON)
				: BCHP_FIELD_ENUM(MDI_FCN_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE,OFF)));
		}
#endif
		/* MDI_TOP_0_MODE_CONTROL_0: disable double buffering */
		ulModeCtrl0 |=
			BCHP_FIELD_ENUM(MDI_TOP_0_MODE_CONTROL_0, UPDATE_SEL, ALWAYS_UPDATE);

		/* MDI_TOP_0_MODE_CONTROL_0: optimal settings */
#if ((BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_4) || \
	 (BVDC_P_SUPPORT_MADR_VER >= BVDC_P_MADR_VER_5))
		ulModeCtrl0 |=
			BCHP_FIELD_ENUM(MDI_TOP_0_MODE_CONTROL_0, FEEDER_PRE_FETCH, ON);
#endif
#if (BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_4)
		ulModeCtrl0 |=
			BCHP_FIELD_ENUM(MDI_TOP_0_MODE_CONTROL_0, WAIT_DMA_DONE, ON);
#endif

#if ((BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_5) || \
	 (BVDC_P_SUPPORT_MADR_VER >= BVDC_P_MADR_VER_6))
		ulModeCtrl0 |=
			BCHP_FIELD_ENUM(MDI_TOP_0_MODE_CONTROL_0, SUSPEND_EOP, ON);
#endif

		/* Debug OSD, Orientation, Gamemode handling */
		ulModeCtrl0 |= hMcdi->bEnableOsd ?
			BCHP_FIELD_ENUM(MDI_TOP_0_MODE_CONTROL_0, ON_SCREEN_STATUS_ENABLE, ON) :
			BCHP_FIELD_ENUM(MDI_TOP_0_MODE_CONTROL_0, ON_SCREEN_STATUS_ENABLE, OFF);

		if(hMcdi->bEnableOsd)
		{
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_TOP_0_OSD_POSITION, ulRegOffset,
				BCHP_FIELD_DATA(MDI_TOP_0_OSD_POSITION, HPOS, hMcdi->ulOsdHpos) |
				BCHP_FIELD_DATA(MDI_TOP_0_OSD_POSITION, VPOS, hMcdi->ulOsdVpos));
		}

#if ((BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_2) || BVDC_P_SUPPORT_MADR)
		ulModeCtrl0 |=
			BCHP_FIELD_DATA(MDI_TOP_0_MODE_CONTROL_0, BVB_VIDEO, eOrientation);
#else
		BSTD_UNUSED(eOrientation);
#endif
		switch(hMcdi->eGameMode)
		{
		default:
		case(BVDC_MadGameMode_e4Fields_2Delay):
		case (BVDC_MadGameMode_eOff):
			ulModeCtrl0 |=
				BCHP_FIELD_ENUM(MDI_TOP_0_MODE_CONTROL_0, LOW_DELAY_COUNT, NORMAL_DELAY);
			break;
		case(BVDC_MadGameMode_e4Fields_1Delay):
			ulModeCtrl0 |=
				BCHP_FIELD_ENUM(MDI_TOP_0_MODE_CONTROL_0, LOW_DELAY_COUNT, ONE_FIELD_DELY);
			break;
		case(BVDC_MadGameMode_e4Fields_0Delay):
			ulModeCtrl0 |=
				BCHP_FIELD_ENUM(MDI_TOP_0_MODE_CONTROL_0, LOW_DELAY_COUNT, ZERO_FIELD_DELAY);
			break;
		}

		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_TOP_0_MODE_CONTROL_0, ulRegOffset, ulModeCtrl0);
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_TOP_0_ENABLE_CONTROL, ulRegOffset,
			BCHP_FIELD_ENUM(MDI_TOP_0_ENABLE_CONTROL, ENABLE, ON));

		/* used by BeHardStart_isr */
		hMcdi->bInitial = false;

		/* cnt for masking-out freeze bit with 1st 4 field */
		if (hMcdi->usTrickModeStartDelay > 0)
		{
			hMcdi->usTrickModeStartDelay --;
		}
	}
	else
	{
		/* disable mcdi */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MDI_TOP_0_ENABLE_CONTROL, ulRegOffset,
			BCHP_FIELD_ENUM(MDI_TOP_0_ENABLE_CONTROL, ENABLE, OFF));

		/* used by BeHardStart_isr */
		hMcdi->bInitial = true;
	}
}

/***************************************************************************
* {private}
*
*/
bool BVDC_P_Mcdi_BeHardStart_isr
	( BVDC_P_Mcdi_Handle                 hMcdi )
{
	uint32_t ulHwDirty;

	BDBG_OBJECT_ASSERT(hMcdi, BVDC_MDI);
	ulHwDirty = hMcdi->stHwDirty.stBits.bSize;
	return (hMcdi->bInitial || ulHwDirty);
}

/***************************************************************************
* {private}
*
* BVDC_P_Mcdi_GetUserConf_isr
*
* called by BVDC_P_MCVP_SetInfo_isr at every src or vec vsync (depending on
* whether reader side or writer side is using this module)
*
*/

void BVDC_P_Mcdi_GetUserConf_isr
		( BVDC_P_Mcdi_Handle               hMcdi,
		BVDC_P_Deinterlace_Settings       *pMadSettings)
{
	hMcdi->eGameMode      = pMadSettings->eGameMode;
	hMcdi->bRev22Pulldown = pMadSettings->bReverse22Pulldown;
	hMcdi->bRev32Pulldown = pMadSettings->bReverse32Pulldown;
	hMcdi->stTestFeature1.bEnable = pMadSettings->stTestFeature1.bEnable;
	hMcdi->stTestFeature1.ulBitsPerPixel = pMadSettings->stTestFeature1.ulBitsPerPixel;
	hMcdi->pChromaSettings               = &pMadSettings->stChromaSettings;
	return;
}
/***************************************************************************
* {private}
*
* BVDC_P_Mcdi_Init_Chroma_DynamicDefault
*
* called by BVDC_P_Mcvp_AcquireConnect at acquire connection 
* See Also:
* BVDC_P_Mad_Init_Chroma_DynamicDefault
* Section 7.3~7.4 in
* http://www.sj.broadcom.com/projects/dvt/Chip_Architecture/Video/Released/mad-IT_ph3.pdf
*/
void BVDC_P_Mcdi_Init_Chroma_DynamicDefault
		(BVDC_P_Mcdi_Handle                 hMcdi,
		BVDC_Deinterlace_ChromaSettings   *pChromaSettings,
		const BFMT_VideoInfo               *pFmtInfo,
		bool                                bMfdSrc)
{

#if (BVDC_P_SUPPORT_MCDI_VER)
	BDBG_ASSERT(pChromaSettings);

	if(hMcdi->bMadr) return;

	/* MAD_0_MODE_CONTROL_0.CHROMA_FIELD_420_EDGE_DET_MODE */
	pChromaSettings->bChromaField420EdgeDetMode = true;
	/* MAD_0_MODE_CONTROL_0.CHROMA_FIELD_420_INITIAL_PHASE */
	pChromaSettings->bChromaField420InitPhase = false;
	/* MAD_0_MODE_CONTROL_0.CHROMA_FIELD_420_INV_METHOD */
	pChromaSettings->eChromaField420InvMethod   = !bMfdSrc;

	if(VIDEO_FORMAT_IS_PAL(pFmtInfo->eVideoFmt))
	{
		/* MDI_PPB_0_XCHROMA_CONTROL_1.PIC_422_MA */
		pChromaSettings->eChroma422MotionAdaptiveMode =
			BCHP_MDI_PPB_0_XCHROMA_CONTROL_1_PIC_422_MA_INT_420;
		/* MDI_PPB_0_XCHROMA_CONTROL_1.PIC_422_IT */
		pChromaSettings->eChroma422InverseTelecineMode =
			BCHP_MDI_PPB_0_XCHROMA_CONTROL_1_PIC_422_IT_INT_420;
	}
	else
	{
		/* MDI_PPB_0_XCHROMA_CONTROL_1.PIC_422_MA */
		pChromaSettings->eChroma422MotionAdaptiveMode = 
			BCHP_MDI_PPB_0_XCHROMA_CONTROL_1_PIC_422_MA_INT_420;

		/* MDI_PPB_0_XCHROMA_CONTROL_1.PIC_422_IT */
		pChromaSettings->eChroma422InverseTelecineMode = 
			bMfdSrc ? 
			BCHP_MDI_PPB_0_XCHROMA_CONTROL_1_PIC_422_IT_IT_DIRECT:
			BCHP_MDI_PPB_0_XCHROMA_CONTROL_1_PIC_422_IT_MA_CHROMA;
	}

	if((!bMfdSrc) &&
		(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt) || VIDEO_FORMAT_IS_PAL(pFmtInfo->eVideoFmt)))
	{
		/* MDI_PPB_0_XCHROMA_CONTROL_0.CHROMA_422_MOTION_MODE */
		pChromaSettings->eChroma422MotionMode = 
			BCHP_MDI_PPB_0_XCHROMA_CONTROL_0_CHROMA_422_MOTION_MODE_XCHROMA_AWARE;

		/* MDI_PPB_0_XCHROMA_CONTROL_0.CHROMA_FIELD_420_MOTION_MODE */
		pChromaSettings->eChroma420MotionMode = 
			BCHP_MDI_PPB_0_XCHROMA_CONTROL_0_CHROMA_422_MOTION_MODE_XCHROMA_AWARE;
	}
	else
	{
		/* MDI_PPB_0_XCHROMA_CONTROL_0.CHROMA_422_MOTION_MODE */
		pChromaSettings->eChroma422MotionMode = 
			BCHP_MDI_PPB_0_XCHROMA_CONTROL_0_CHROMA_422_MOTION_MODE_DIGITAL_CHROMA;

		/* MDI_PPB_0_XCHROMA_CONTROL_0.CHROMA_FIELD_420_MOTION_MODE */
		pChromaSettings->eChroma420MotionMode = 
			BCHP_MDI_PPB_0_XCHROMA_CONTROL_0_CHROMA_422_MOTION_MODE_DIGITAL_CHROMA;
	}

	
	
	/* MDI_PPB_0_XCHROMA_CONTROL_1.FIELD_420_MA */
	pChromaSettings->eChroma420MotionAdaptiveMode =
		BCHP_MDI_PPB_0_XCHROMA_CONTROL_1_FIELD_420_MA_INT_420;
#else
	BSTD_UNUSED(hMcdi);
	BSTD_UNUSED(pChromaSettings);
	BSTD_UNUSED(pFmtInfo);
	BSTD_UNUSED(bMfdSrc);
#endif
	
}


/***************************************************************************/
/* No support for any mcdi */
#else
BERR_Code BVDC_P_Mcdi_Create
	( BVDC_P_Mcdi_Handle           *phMcdi,
	 BVDC_P_McdiId                 eMcdiId,
	 BREG_Handle                   hRegister,
	 BVDC_P_Resource_Handle        hResource )
{
	BSTD_UNUSED(phMcdi);
	BSTD_UNUSED(hRegister);
	BSTD_UNUSED(eMcdiId);
	BSTD_UNUSED(hResource);
	return BERR_SUCCESS;
}

void BVDC_P_Mcdi_Destroy
	( BVDC_P_Mcdi_Handle             hMcdi )
{
	BSTD_UNUSED(hMcdi);
}


/***************************************************************************
* Return the user set of mcdi chroma
*/
void BVDC_P_Mcdi_GetUserChroma_isr
	( BVDC_P_Mcdi_Handle               hMcdi,
	 BVDC_Deinterlace_ChromaSettings *pChromaSettings )
{
	BSTD_UNUSED(hMcdi);
	BSTD_UNUSED(pChromaSettings);
}

/***************************************************************************
* Return the user set of mcdi motino
*/
void BVDC_P_Mcdi_GetUserMotion_isr
	( BVDC_P_Mcdi_Handle                 hMcdi,
	 BVDC_Deinterlace_MotionSettings   *pMotionSettings )
{
	BSTD_UNUSED(hMcdi);
	BSTD_UNUSED(pMotionSettings);
}
/***************************************************************************
* Return the user set of mcdi 3:2 pulldown
*/
void BVDC_P_Mcdi_GetUserReverse32_isr
	( BVDC_P_Mcdi_Handle                  hMcdi,
	 BVDC_Deinterlace_Reverse32Settings *pRev32Settings )
{
	BSTD_UNUSED(hMcdi);
	BSTD_UNUSED(pRev32Settings);
}
/***************************************************************************
* Return the user set of mcdi chroma
*/
void BVDC_P_Mcdi_GetUserReverse22_isr
	( BVDC_P_Mcdi_Handle                  hMcdi,
	 BVDC_Deinterlace_Reverse22Settings *pRev22Settings )
{
	BSTD_UNUSED(hMcdi);
	BSTD_UNUSED(pRev22Settings);
}

void BVDC_P_Mcdi_SetVnetAllocBuf_isr
	( BVDC_P_Mcdi_Handle               hMcdi,
	 uint32_t                         ulSrcMuxValue,
	 BVDC_P_VnetPatch                 eVnetPatchMode,
	 BVDC_P_BufferHeapId              eBufHeapId,
	 uint16_t                         usPixelBufferCnt )
{
	BSTD_UNUSED(hMcdi);
	BSTD_UNUSED(ulSrcMuxValue);
	BSTD_UNUSED(eVnetPatchMode);
	BSTD_UNUSED(eBufHeapId);
	BSTD_UNUSED(usPixelBufferCnt);
}

void BVDC_P_Mcdi_UnsetVnetFreeBuf_isr
	( BVDC_P_Mcdi_Handle          hMcdi )
{
	BSTD_UNUSED(hMcdi);
}

bool BVDC_P_Mcdi_BeHardStart_isr
	( BVDC_P_Mcdi_Handle                 hMcdi )
{

	BSTD_UNUSED(hMcdi);
	return false;
}

void BVDC_P_Mcdi_GetUserConf_isr
	( BVDC_P_Mcdi_Handle               hMcdi,
	  BVDC_P_Deinterlace_Settings     *pMadSettings)
{
	BSTD_UNUSED(hMcdi);
	BSTD_UNUSED(pMadSettings);
}

void BVDC_P_Mcdi_Init_Chroma_DynamicDefault
		(BVDC_P_Mcdi_Handle                 hMcdi,
		BVDC_Deinterlace_ChromaSettings   *pChromaSettings,
		const BFMT_VideoInfo               *pFmtInfo,
		bool                                bMfdSrc)
{
	BSTD_UNUSED(hMcdi);
	BSTD_UNUSED(pChromaSettings);
	BSTD_UNUSED(pFmtInfo);
	BSTD_UNUSED(bMfdSrc);
}
#endif /* MCDI SUPPORT */
/***************************************************************************
*
*/
uint16_t BVDC_P_Mcdi_GetVsyncDelayNum_isr
	( BVDC_P_Mcdi_Handle               hMcdi,
	  BVDC_MadGameMode                 eGameMode)
{
#if (BVDC_P_SUPPORT_MADR_VER < BVDC_P_MADR_VER_7)
	/* SW7231-383: MADR has only 1 delay */
	return (hMcdi->bMadr) ? 1 : s_aMcdiGameModeInfo[eGameMode].usDelay;
#else
	/* SW7231-383: MADR has only 1 delay */
	return (hMcdi->bMadr) ? s_aMadrGameModeInfo[eGameMode].usDelay :
		s_aMcdiGameModeInfo[eGameMode].usDelay;
#endif
}

/***************************************************************************
*
*/
uint16_t BVDC_P_Mcdi_GetPixBufCnt_isr
	( BVDC_P_Mcdi_Handle               hMcdi,
	  BVDC_MadGameMode                 eGameMode)
{
#if (BVDC_P_SUPPORT_MADR_VER < BVDC_P_MADR_VER_7)
	/* SW7231-383: MADR uses only 3 buffers */
	return (hMcdi->bMadr) ? 3 : s_aMcdiGameModeInfo[eGameMode].usPixelBufferCnt;
#else
	/* HW7435-222: MADR uses 4 buffers */
	return (hMcdi->bMadr) ? s_aMadrGameModeInfo[eGameMode].usPixelBufferCnt :
		s_aMcdiGameModeInfo[eGameMode].usPixelBufferCnt;
#endif
}

/* End of file. */
