/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_dnr_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/32 $
 * $brcm_Date: 8/15/12 6:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_dnr_priv.h $
 * 
 * Hydra_Software_Devel/32   8/15/12 6:22p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/31   6/15/12 10:33a vanessah
 * SW7435-237: reset vnet channel connections
 *
 * Hydra_Software_Devel/30   3/9/12 12:13p vanessah
 * SW7435-9:  bringup DNR3
 *
 * Hydra_Software_Devel/29   10/20/11 6:50p vanessah
 * SW7425-1572: add DNR-2 support
 *
 * Hydra_Software_Devel/28   9/30/10 4:31p tdo
 * SW7422-50: VDC: Get DNR_H working in 7422
 *
 * Hydra_Software_Devel/27   6/18/10 4:19p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/26   4/7/10 11:29a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/25   4/5/10 4:05p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/24   1/26/10 5:42p yuxiaz
 * SW7405-3839: Remove the workaround for PR38730
 *
 * Hydra_Software_Devel/23   11/19/09 2:02p pntruong
 * SW7408-13: Used more core rev'ing mechanism for ease of portability.
 *
 * Hydra_Software_Devel/22   11/19/09 10:34a rpan
 * SW7468-31: Comply with the VDC version scheme.
 *
 * Hydra_Software_Devel/21   9/10/09 3:02p tdo
 * SW7335-542, SW7335-543:[PQ] Need to change DCR/BNR/MNR to improve PQ
 * for STB
 *
 * Hydra_Software_Devel/20   4/27/09 6:42p syang
 * PR 54447: add back dnr disabling code and add BuildWinId usage
 *
 * Hydra_Software_Devel/19   10/2/08 2:31p tdo
 * PR47349: Swap FILT_CLAMP and DITH_CLAMP
 *
 * Hydra_Software_Devel/18   9/29/08 3:02p tdo
 * PR47349: Always disable DITH in DCR block for 10-bit chips.  Update new
 * DCR config table.
 *
 * Hydra_Software_Devel/17   6/24/08 6:29p tdo
 * PR44113: Seperate REL and LIMIT for HBNR and VBNR.
 *
 * Hydra_Software_Devel/16   12/12/07 5:40p tdo
 * PR34386: Modify formulas to calculate MNR and BNR to remove trembling
 * issue
 *
 * Hydra_Software_Devel/15   9/28/07 2:46p tdo
 * PR33711: Remove global writable data usage
 *
 * Hydra_Software_Devel/14   7/19/07 2:39p tdo
 * PR33034: Modify DNR API to extend MNR/BNR/DCR filter level
 *
 * Hydra_Software_Devel/13   6/27/07 12:33p tdo
 * PR32489: Add support for custom adjust DNR
 *
 * Hydra_Software_Devel/12   5/14/07 4:45p tdo
 * PR28891, PR29658: C0: BVN: Add DCR (digital contour removal) support in
 * DNR block
 *
 * Hydra_Software_Devel/11   4/2/07 11:27a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/10   3/5/07 6:40p syang
 * PR 28298: 1). add RegOffset init; add Acquire/Release
 *
 * Hydra_Software_Devel/9   2/23/07 6:28p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/8   1/10/07 3:30p yuxiaz
 * PR 26313: Change DNR API to source based API: Remove
 * BVDC_Window_SetDnrFilterLevel and BVDC_Window_GetDnrFilterLevel. Add
 * BVDC_Source_SetDnrFilterLevel, BVDC_Source_GetDnrFilterLevel,
 * BVDC_Source_SetSplitScreenMode and BVDC_Source_GetSplitScreenMode.
 *
 * Hydra_Software_Devel/7   1/10/07 10:59a syang
 * PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
 * dnr, mfd, vfd, cap, ...
 *
 * Hydra_Software_Devel/6   1/3/07 5:27p yuxiaz
 * PR25995: Add DNR support for 7400 B0.
 *
 * Hydra_Software_Devel/5   12/18/06 11:39p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/22/06 3:51p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/4   7/31/06 6:10p hongtaoz
 * PR22499: added DCR filter setting as a part of DNR block;
 *
 * Hydra_Software_Devel/3   5/26/06 3:39p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/2   5/19/06 2:14p hongtaoz
 * PR17514: added bypass filter enum for DNR setting; disable extreme
 * filter for 7401A0 due to PR16299; fixed range clamping for DNR
 * settings; added DNR demo mode setting in test api;
 *
 * Hydra_Software_Devel/1   10/31/05 6:24p hongtaoz
 * PR17514: add DNR support in VDC;
 *
 ***************************************************************************/
#ifndef BVDC_DNR_PRIV_H__
#define BVDC_DNR_PRIV_H__

#include "bvdc.h"
#include "bavc.h"
#include "bvdc_common_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_buffer_priv.h"

#if BVDC_P_SUPPORT_DNR
#include "bchp_dnr_0.h"
#if (BVDC_P_SUPPORT_DNR == 2)
#include "bchp_dnr_1.h"
#endif
#if (BVDC_P_SUPPORT_DNR == 3)
#include "bchp_dnr_2.h"
#endif
#if (BVDC_P_SUPPORT_DNR == 4)
#include "bchp_dnr_3.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Private register cracking macros
 ***************************************************************************/
#if BVDC_P_SUPPORT_DNR
/* VER_0 needs PR35221, PR38730 work-around */
#define BVDC_P_SUPPORT_DNR_VER_0              (0) /* 7325A0, 7335A0, 7405A0 */
/* VER_1 needs PR35221 work-around, PR38730 is fixed */
#define BVDC_P_SUPPORT_DNR_VER_1              (1) /* other 7335, 7405B0, Other 7400 */
/* These > VER_1 fixes above PRs */
#define BVDC_P_SUPPORT_DNR_VER_2              (2) /* 7400E0, 7325B0 */
#define BVDC_P_SUPPORT_DNR_VER_3              (3) /* 7420 */
#define BVDC_P_SUPPORT_DNR_VER_4              (4) /* 3548A0 */
#define BVDC_P_SUPPORT_DNR_VER_5              (5) /* 3548B0*/
#define BVDC_P_SUPPORT_DNR_VER_6              (6) /* 7468, 7408 */
#define BVDC_P_SUPPORT_DNR_VER_7              (7) /* 7422, 7425 */

#define BVDC_P_DNR_MAX_HW_QP_STEPS    (100)
#ifdef BCHP_DNR_4_REG_START
#define BVDC_P_DNR_GET_REG_OFFSET(dnr_id) \
	((BVDC_P_DnrId_eDnr4 ==(dnr_id)) ? (BCHP_DNR_4_REG_START - BCHP_DNR_0_REG_START) \
	:(BVDC_P_DnrId_eDnr3 ==(dnr_id)) ? (BCHP_DNR_3_REG_START - BCHP_DNR_0_REG_START) \
	:(BVDC_P_DnrId_eDnr2 ==(dnr_id)) ? (BCHP_DNR_2_REG_START - BCHP_DNR_0_REG_START) \
	:(BVDC_P_DnrId_eDnr1 ==(dnr_id)) ? (BCHP_DNR_1_REG_START - BCHP_DNR_0_REG_START) \
	:(0))
#define BVDC_P_Dnr_PostMuxValue(hDnr) \
	((0 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_0 : \
	 (1 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_1 : \
	 (2 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_2 : \
	 (3 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_3 : \
						 BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_4 )
#elif BCHP_DNR_3_REG_START
#define BVDC_P_DNR_GET_REG_OFFSET(dnr_id) \
	((BVDC_P_DnrId_eDnr3 ==(dnr_id)) ? (BCHP_DNR_3_REG_START - BCHP_DNR_0_REG_START) \
	:(BVDC_P_DnrId_eDnr2 ==(dnr_id)) ? (BCHP_DNR_2_REG_START - BCHP_DNR_0_REG_START) \
	:(BVDC_P_DnrId_eDnr1 ==(dnr_id)) ? (BCHP_DNR_1_REG_START - BCHP_DNR_0_REG_START) \
	:(0))
#define BVDC_P_Dnr_PostMuxValue(hDnr) \
	((0 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_0 : \
	 (1 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_1 : \
	 (2 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_2 : \
						 BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_3 )
#elif BCHP_DNR_2_REG_START
#define BVDC_P_DNR_GET_REG_OFFSET(dnr_id) \
	((BVDC_P_DnrId_eDnr2 ==(dnr_id)) ? (BCHP_DNR_2_REG_START - BCHP_DNR_0_REG_START) \
	:(BVDC_P_DnrId_eDnr1 ==(dnr_id)) ? (BCHP_DNR_1_REG_START - BCHP_DNR_0_REG_START) \
	:(0))
#define BVDC_P_Dnr_PostMuxValue(hDnr) \
	((0 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_0 : \
	 (1 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_1 : \
                         BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_2 )
#elif BCHP_DNR_1_REG_START
#define BVDC_P_DNR_GET_REG_OFFSET(dnr_id) \
	((BVDC_P_DnrId_eDnr1 ==(dnr_id)) ? (BCHP_DNR_1_REG_START - BCHP_DNR_0_REG_START) \
	:(0))
#define BVDC_P_Dnr_PostMuxValue(hDnr) \
	((0 == (hDnr->eId))? BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_0 : \
                         BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_1 )
#else
#define BVDC_P_DNR_GET_REG_OFFSET(dnr_id) (0)
#define BVDC_P_Dnr_PostMuxValue(hDnr) BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_0
#endif

#define BVDC_P_DNR_GET_REG_IDX(reg) \
	((BCHP##_##reg - BCHP_DNR_0_REG_START) / sizeof(uint32_t))

/* Get/Set reg data */
#define BVDC_P_DNR_GET_REG_DATA(reg) \
	(hDnr->aulRegs[BVDC_P_DNR_GET_REG_IDX(reg)])
#define BVDC_P_DNR_SET_REG_DATA(reg, data) \
	(BVDC_P_DNR_GET_REG_DATA(reg) = (uint32_t)(data))

/* Get with index. */
#define BVDC_P_DNR_GET_REG_DATA_I(reg, idx) \
	(hDnr->aulRegs[BVDC_P_DNR_GET_REG_IDX(reg) + (idx)])

/* Get field */
#define BVDC_P_DNR_GET_FIELD_NAME(reg, field) \
	(BVDC_P_GET_FIELD(BVDC_P_DNR_GET_REG_DATA(reg), reg, field))

/* Compare field */
#define BVDC_P_DNR_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_COMPARE_FIELD_DATA(BVDC_P_DNR_GET_REG_DATA(reg), reg, field, (data)))
#define BVDC_P_DNR_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_COMPARE_FIELD_NAME(BVDC_P_DNR_GET_REG_DATA(reg), reg, field, name))

/* This macro does a write into RUL (write, addr, data). 3 dwords. */
#define BVDC_P_DNR_WRITE_TO_RUL(reg, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BCHP##_##reg + hDnr->ulRegOffset); \
	*addr_ptr++ = BVDC_P_DNR_GET_REG_DATA(reg); \
}

/* This macro does a block write into RUL */
#define BVDC_P_DNR_BLOCK_WRITE_TO_RUL(from, to, pulCurrent) \
do { \
	uint32_t ulBlockSize = \
		BVDC_P_REGS_ENTRIES(from, to);\
	*pulCurrent++ = BRDC_OP_IMMS_TO_REGS( ulBlockSize ); \
	*pulCurrent++ = BRDC_REGISTER(BCHP##_##from + hDnr->ulRegOffset); \
	BKNI_Memcpy((void*)pulCurrent, \
		(void*)&(hDnr->aulRegs[BVDC_P_DNR_GET_REG_IDX(from)]), \
		ulBlockSize * sizeof(uint32_t)); \
	pulCurrent += ulBlockSize; \
} while(0)

/* number of registers in one block. */
#define BVDC_P_DNR_REGS_COUNT    \
	BVDC_P_REGS_ENTRIES(DNR_0_REG_START, DNR_0_REG_END)
#else
#define BVDC_P_DNR_REGS_COUNT   1
#endif

/* Miscellaneous constants */
#define BVDC_P_Dnr_LAST UINT32_C(-1)

/* [lo,hi] */
#define BVDC_P_DNR_CLAMP(low, hi, x) (((int)(x) <= (int)(low)) ? (low) : (((int)(x) > (int)(hi)) ? (hi) : (x)))

#define BVDC_P_Dnr_MuxAddr(hDnr)      (BCHP_VNET_F_DNR_0_SRC + (hDnr)->eId * sizeof(uint32_t))


#define BVDC_P_Dnr_SetVnet_isr(hDnr, ulSrcMuxValue, eVnetPatchMode) \
   BVDC_P_SubRul_SetVnet_isr(&((hDnr)->SubRul), ulSrcMuxValue, eVnetPatchMode)

#define BVDC_P_Dnr_UnsetVnet_isr(hDnr) \
  BVDC_P_SubRul_UnsetVnet_isr(&((hDnr)->SubRul))

#define BVDC_P_Dnr_SetRulBuildWinId_isr(hDnr, eWinId) \
    BVDC_P_SubRul_SetRulBuildWinId_isr(&((hDnr)->SubRul), eWinId)

/***************************************************************************
 * DNR private data structures
 ***************************************************************************/
typedef struct
{
	uint32_t                        ulFilt0Limit;
	uint32_t                        ulFilt1Limit;
	uint32_t                        ulFilt2Limit;
	uint32_t                        ulFilt3Limit;
	uint32_t                        ulFiltClamp;
	uint32_t                        ulRandomA;
	uint32_t                        ulRandomB;
	uint32_t                        ulRandomC;
	uint32_t                        ulRandomD;
	uint32_t                        ulDithClamp;
	uint32_t                        ulOrderA;
	uint32_t                        ulOrderB;
} BVDC_P_DcrCfgEntry;

typedef struct
{
	uint32_t                        ulHBnr;      /* DNR_0.BNR_CTRL.HBNR_ENABLE */
	uint32_t                        ulVBnr;      /* DNR_0.BNR_CTRL.VBNR_ENABLE */
	uint32_t                        ulSmallGrid; /* DNR_0.BNR_CONFIG.HBNR_SMALL_GRID */
	uint32_t                        ulLrLimit;   /* DNR_0.BNR_CONFIG.VBNR_LR_LIMIT */
	uint32_t                        ulHRel;       /* DNR_0.HBNR_CONFIG.HBNR_REL */
	uint32_t                        ulHLimit;     /* DNR_0.HBNR_CONFIG.HBNR_LIMIT */
	uint32_t                        ulVRel;       /* DNR_0.VBNR_CONFIG.VBNR_REL */
	uint32_t                        ulVLimit;     /* DNR_0.VBNR_CONFIG.VBNR_LIMIT */
} BVDC_P_BnrCfgEntry;

typedef struct
{
	uint32_t                        ulMnr;       /* DNR_0.MNR_CTRL.MNR_ENABLE */
	uint32_t                        ulSpot;      /* DNR_0.MNR_CONFIG.MNR_SPOT */
	uint32_t                        ulMerge;     /* DNR_0.MNR_CONFIG.MNR_MERGE */
	uint32_t                        ulRel;       /* DNR_0.MNR_CONFIG.MNR_REL */
	uint32_t                        ulLimit;     /* DNR_0.MNR_CONFIG.MNR_LIMIT */
} BVDC_P_MnrCfgEntry;

typedef struct BVDC_P_DnrContext
{
	BDBG_OBJECT(BVDC_DNR)

	BVDC_Source_Handle             hSource;

	/* flag initial state, require reset; */
	bool                           bInitial;
	uint32_t                       ulResetRegAddr;
	uint32_t                       ulResetMask;
	uint32_t                       ulVnetResetAddr;
	uint32_t                       ulVnetResetMask;

	/* flag for changes */
	uint32_t                       ulUpdateAll;

	/* adj_Qp */
	uint32_t                       ulMnrQp;
	uint32_t                       ulBnrQp;
	uint32_t                       ulDcrQp;

	BVDC_P_BnrCfgEntry             stBnrCfg;
	BVDC_P_MnrCfgEntry             stMnrCfg;
	BVDC_P_DcrCfgEntry             stDcrCfg;

	uint32_t                       ulFilterLimit;

	/* private fields. */
	BVDC_P_DnrId                   eId;
	uint32_t                       ulRegOffset; /* in case of multiple instances */
	uint32_t                       aulRegs[BVDC_P_DNR_REGS_COUNT];

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext           SubRul;

} BVDC_P_DnrContext;


/***************************************************************************
 * DNR private functions
 ***************************************************************************/
BERR_Code BVDC_P_Dnr_Create
	( BVDC_P_Dnr_Handle            *phDnr,
	  BVDC_P_DnrId                  eDnrId,
	  BVDC_P_Resource_Handle        hResource );

void BVDC_P_Dnr_Destroy
	( BVDC_P_Dnr_Handle             hDnr );

void BVDC_P_Dnr_Init
	( BVDC_P_Dnr_Handle             hDnr );

BERR_Code BVDC_P_Dnr_AcquireConnect
	( BVDC_P_Dnr_Handle             hDnr,
	  BVDC_Source_Handle            hSource );

BERR_Code BVDC_P_Dnr_ReleaseConnect_isr
	( BVDC_P_Dnr_Handle            *phDnr );

void BVDC_P_Dnr_BuildRul_isr
	( const BVDC_P_Dnr_Handle       hDnr,
	  BVDC_P_ListInfo              *pList,
	  BVDC_P_State                  eVnetState,
	  BVDC_P_PicComRulInfo         *pPicComRulInfo );

BERR_Code BVDC_P_Dnr_SetInfo_isr
	( BVDC_P_Dnr_Handle             hDnr,
	  const BVDC_P_PictureNodePtr   pPicture );

BERR_Code BVDC_P_Dnr_SetEnable_isr
	( BVDC_P_Dnr_Handle             hDnr,
	  bool                          bEnable );

const BVDC_P_DcrCfgEntry* BVDC_P_Dnr_GetDcrCfg_isr
	( uint32_t                       ulDcrQp,
	  const BFMT_VideoInfo          *pFmtInfo,
	  void                          *pvUserInfo );

const BVDC_P_BnrCfgEntry* BVDC_P_Dnr_GetBnrCfg_isr
	( uint32_t                       ulBnrQp,
	  BAVC_Polarity                  eSrcOrigPolarity,
	  const BFMT_VideoInfo          *pFmtInfo,
	  void                          *pvUserInfo );

const BVDC_P_MnrCfgEntry* BVDC_P_Dnr_GetMnrCfg_isr
	( uint32_t                       ulMnrQp,
	  uint32_t                       ulSrcHSize,
	  const BFMT_VideoInfo          *pFmtInfo,
	  void                          *pvUserInfo );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_DNR_PRIV_H__ */
/* End of file. */
