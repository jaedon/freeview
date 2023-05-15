/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_fgt_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 6/18/10 4:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_fgt_priv.h $
 * 
 * Hydra_Software_Devel/5   6/18/10 4:20p rpan
 * SW7400-2763: New BVN resource management.
 * 
 * Hydra_Software_Devel/4   4/7/10 11:30a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/3   4/5/10 4:06p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/2   9/11/08 11:27a yuxiaz
 * PR46623: Add the code to enabe the endian swap bit for FGT.
 *
 * Hydra_Software_Devel/1   10/23/07 11:16a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/2   10/17/07 3:27p yuxiaz
 * PR29569: More FGT work.
 *
 ***************************************************************************/
#ifndef BVDC_FGT_PRIV_H__
#define BVDC_FGT_PRIV_H__

#include "bvdc_common_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_window_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Private defines
 ***************************************************************************/
/* 7400Dx, 7405Ax, 7325Ax, 7335Ax: basic FGT support */
#define BVDC_P_FGT_VER_0                            (0)
/* 7400E0, 7325Bx, 7335Bx: basic FGT support + ENDIAN_SWAP bit */
#define BVDC_P_FGT_VER_1                            (1)

/***************************************************************************
 * Private macros
 ***************************************************************************/
#define BVDC_P_FGT_GET_REG_IDX(reg) \
	((BCHP##_##reg - BCHP_FGT_0_REG_START) / sizeof(uint32_t))

/* Get register data */
#define BVDC_P_FGT_GET_REG_DATA(reg) \
	(hFgt->aulRegs[BVDC_P_FGT_GET_REG_IDX(reg)])

/* Set register data */
#define BVDC_P_FGT_SET_REG_DATA(reg, data) \
	(BVDC_P_FGT_GET_REG_DATA(reg) = (uint32_t)(data))

/* Get field */
#define BVDC_P_FGT_GET_FIELD_NAME(reg, field) \
	((BVDC_P_FGT_GET_REG_DATA(reg) & BCHP##_##reg##_##field##_MASK) >> \
	BCHP##_##reg##_##field##_SHIFT)

/* Compare field */
#define BVDC_P_FGT_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_FGT_GET_FIELD_NAME(reg, field)==(data))

#define BVDC_P_FGT_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_FGT_GET_FIELD_NAME(reg, field)==BCHP##_##reg##_##field##_##name)

#define BVDC_P_FGT_WRITE_TO_RUL(reg, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BCHP##_##reg + hFgt->ulRegOffset); \
	*addr_ptr++ = BVDC_P_FGT_GET_REG_DATA(reg); \
}

/* This macro does a block write into RUL */
#define BVDC_P_FGT_BLOCK_WRITE_TO_RUL(from, to, pulCurrent) do { \
	uint32_t ulBlockSize = \
		BVDC_P_REGS_ENTRIES(from, to);\
	*pulCurrent++ = BRDC_OP_IMMS_TO_REGS( ulBlockSize ); \
	*pulCurrent++ = BRDC_REGISTER(BCHP##_##from + hFgt->ulRegOffset); \
	BKNI_Memcpy((void*)pulCurrent, \
		(void*)&(hFgt->aulRegs[BVDC_P_FGT_GET_REG_IDX(from)]), \
		ulBlockSize * sizeof(uint32_t)); \
	pulCurrent += ulBlockSize; \
} while (0)

#if (BVDC_P_SUPPORT_FGT)
/* number of registers in one block. */
#define BVDC_P_FGT_REGS_COUNT    \
	BVDC_P_REGS_ENTRIES(FGT_0_REG_START, FGT_0_REG_END)
#else
#define BVDC_P_FGT_REGS_COUNT   1
#endif

/***************************************************************************
 * Private data structures
 ***************************************************************************/
typedef struct BVDC_P_FgtContext
{
	BDBG_OBJECT(BVDC_FGT)

	/* fgt Id */
	BVDC_P_FgtId                     eId;
	uint32_t                         ulRegOffset;
	uint32_t                         aulRegs[BVDC_P_FGT_REGS_COUNT];

	bool                             bInitial;
	uint32_t                         ulResetRegAddr;
	uint32_t                         ulResetMask;

	/* static info from creating */
	BREG_Handle                      hRegister;
	BMEM_Handle                      hMem;
	BVDC_Handle                      hVdc;

	/* Which window it connect to */
	BVDC_Window_Handle               hWindow;

	/* Noise pattern table address */
	uint32_t                         ulPatTableAddrOffset;

	/* user config */
	BVDC_Fgt_Settings               *pUserSettings;

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext             SubRul;

} BVDC_P_FgtContext;


/***************************************************************************
 * private functions
 ***************************************************************************/
#define BVDC_P_Fgt_MuxAddr(hFgt)           (BCHP_VNET_F_FGT_0_SRC + (hFgt)->eId * sizeof(uint32_t))
#define BVDC_P_Fgt_PostMuxValue(hFgt)      (BCHP_VNET_B_CAP_0_SRC_SOURCE_FGT_0 + (hFgt)->eId)

#define BVDC_P_Fgt_SetVnet_isr(hFgt, ulSrcMuxValue, eVnetPatchMode) \
   BVDC_P_SubRul_SetVnet_isr(&((hFgt)->SubRul), ulSrcMuxValue, eVnetPatchMode)

#define BVDC_P_Fgt_UnsetVnet_isr(hFgt) \
  BVDC_P_SubRul_UnsetVnet_isr(&((hFgt)->SubRul))


BERR_Code BVDC_P_Fgt_Create
	( BVDC_P_Fgt_Handle *               phFgt,
	  BVDC_P_FgtId                      eFgtId,
	  BREG_Handle                       hRegister,
	  BMEM_Handle                       hMem,
	  BVDC_Handle                       hVdc,
	  BVDC_P_Resource_Handle            hResource );

BERR_Code BVDC_P_Fgt_Destroy
	( BVDC_P_Fgt_Handle                 hFgt );

BERR_Code BVDC_P_Fgt_AcquireConnect
	( BVDC_P_Fgt_Handle                 hFgt,
	  BVDC_Window_Handle                hWindow,
	  const BVDC_P_Window_Info         *pCurInfo );

BERR_Code BVDC_P_Fgt_ReleaseConnect_isr
	( BVDC_P_Fgt_Handle                *phFgt );

BERR_Code BVDC_P_Fgt_SetInfo_isr
	( BVDC_P_Fgt_Handle                 hFgt,
	  const BVDC_P_PictureNodePtr       pPicture );

void BVDC_P_Fgt_BuildRul_isr
	( BVDC_P_Fgt_Handle                 hFgt,
	  BVDC_P_ListInfo                  *pList,
	  BVDC_P_State                      eVnetState,
	  BVDC_P_PicComRulInfo             *pPicComRulInfo );

void BVDC_P_Fgt_LoadPatTable
	( uint8_t                          *pucFgtPatTblAddr );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_FGT_PRIV_H__ */
/* End of file. */
