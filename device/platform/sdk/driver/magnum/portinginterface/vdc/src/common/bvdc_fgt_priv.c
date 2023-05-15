/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_fgt_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 8/14/12 4:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_fgt_priv.c $
 * 
 * Hydra_Software_Devel/20   8/14/12 4:34p yuxiaz
 * SW7550-802: Fix coverity errors.
 * 
 * Hydra_Software_Devel/19   11/11/10 7:28p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 * 
 * Hydra_Software_Devel/18   6/18/10 4:38p rpan
 * SW7400-2763: Fixed compile warming/error.
 * 
 * Hydra_Software_Devel/17   6/18/10 4:19p rpan
 * SW7400-2763: New BVN resource management.
 * 
 * Hydra_Software_Devel/16   4/19/10 10:14p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/15   4/7/10 11:30a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/14   4/6/10 8:53a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Remove coverity error
 *
 * Hydra_Software_Devel/13   4/5/10 5:22p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/12   4/5/10 5:14p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/11   4/5/10 4:06p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/10   9/3/09 10:31p pntruong
 * SW3548-2373: Rolled back PR56964.
 *
 * Hydra_Software_Devel/9   7/22/09 3:23p syang
 * PR 56964: take off the "connect drainer, en and then dis" step  in
 * vnet-drain code
 *
 * Hydra_Software_Devel/8   11/25/08 11:12a syang
 * PR 49562: add reset to output channel ping-pong buf of sub-modules and
 * loopbacks and free-channels
 *
 * Hydra_Software_Devel/7   11/20/08 9:31a syang
 * PR 48970: clean-up module disable and drian handling so that RUL dump
 * is easy to understand
 *
 * Hydra_Software_Devel/6   10/27/08 5:37p syang
 * PR 46477: back off SetEnable in in BuildRul_isr according to GetOps (it
 * causes window to disapper on 7400 D0)
 *
 * Hydra_Software_Devel/5   10/24/08 4:07p syang
 * PR 46477: in BuildRul_isr only disable in shadow regs  according to
 * GetOps (some SetEnable_isr does HW reset)
 *
 * Hydra_Software_Devel/4   10/24/08 11:04a syang
 * PR 46477: in BuildRul_isr reset enable bit according to GetOps
 *
 * Hydra_Software_Devel/3   9/11/08 11:27a yuxiaz
 * PR46623: Add the code to enabe the endian swap bit for FGT.
 *
 * Hydra_Software_Devel/2   7/9/08 5:02p tdo
 * PR41932: Add support for VDC 7335B0 changes.
 *
 * Hydra_Software_Devel/1   10/23/07 11:16a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/5   10/22/07 5:16p yuxiaz
 * PR29569: More FGT work.
 *
 * Hydra_Software_Devel/PR29569/4   10/19/07 5:03p yuxiaz
 * PR36290: Add FGT demo mode support
 *
 * Hydra_Software_Devel/PR29569/3   10/17/07 3:27p yuxiaz
 * PR29569: More FGT work.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bvdc.h"
#include "brdc.h"
#include "bvdc_priv.h"
#include "bvdc_resource_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_fgt_priv.h"
#include "bchp_vnet_f.h"

#if (BVDC_P_SUPPORT_FGT)

#include "bchp_fgt_0.h"
#include "bchp_mmisc.h"

BDBG_MODULE(BVDC_FGT);
BDBG_OBJECT_ID(BVDC_FGT);

/***************************************************************************
 *
 */
static void BVDC_P_Fgt_RegInit
	( BVDC_P_Fgt_Handle               hFgt )
{
	/* FGT_0_CONTROL */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CONTROL) &= ~(
#if (BVDC_P_SUPPORT_FGT_VER >= BVDC_P_FGT_VER_1)
		BCHP_MASK(FGT_0_CONTROL, ENDIAN_SWAP        ) |
#endif
		BCHP_MASK(FGT_0_CONTROL, SEI_REP_PERIOD_SIZE) |
		BCHP_MASK(FGT_0_CONTROL, SEI_MODE           ) |
		BCHP_MASK(FGT_0_CONTROL, AVG_UV_POSITION    ) |
		BCHP_MASK(FGT_0_CONTROL, NOISE_GEN_MODE     ));
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CONTROL) |= (
#if (BVDC_P_SUPPORT_FGT_VER >= BVDC_P_FGT_VER_1)
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG)
		BCHP_FIELD_DATA(FGT_0_CONTROL, ENDIAN_SWAP        , 1     ) |
#else
		BCHP_FIELD_DATA(FGT_0_CONTROL, ENDIAN_SWAP        , 0     ) |
#endif
#endif
		BCHP_FIELD_DATA(FGT_0_CONTROL, SEI_REP_PERIOD_SIZE, 1     ) |
		BCHP_FIELD_DATA(FGT_0_CONTROL, SEI_MODE           , 0     ) |
		BCHP_FIELD_DATA(FGT_0_CONTROL, AVG_UV_POSITION    , 0     ) |
		BCHP_FIELD_ENUM(FGT_0_CONTROL, NOISE_GEN_MODE     , POST_PROC));

	/* FGT_0_DEMO_SETTING */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_DEMO_SETTING) &= ~(
		BCHP_MASK(FGT_0_DEMO_SETTING, DEMO_BOUNDARY) |
		BCHP_MASK(FGT_0_DEMO_SETTING, DEMO_L_R     ) |
		BCHP_MASK(FGT_0_DEMO_SETTING, DEBUG_MODE   ));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_DEMO_SETTING) |= (
		BCHP_FIELD_DATA(FGT_0_DEMO_SETTING, DEMO_BOUNDARY, 0          ) |
		BCHP_FIELD_ENUM(FGT_0_DEMO_SETTING, DEMO_L_R     , RIGHT      ) |
		BCHP_FIELD_ENUM(FGT_0_DEMO_SETTING, DEBUG_MODE   , DEMO_MODE));

	/* FGT_0_NOISEPAT_BASE_ADRS */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NOISEPAT_BASE_ADRS) &= ~(
		BCHP_MASK(FGT_0_NOISEPAT_BASE_ADRS, NOISE_PAT_BASE_ADRS));
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NOISEPAT_BASE_ADRS) |= (
		BCHP_FIELD_DATA(FGT_0_NOISEPAT_BASE_ADRS, NOISE_PAT_BASE_ADRS,
		hFgt->ulPatTableAddrOffset >> 5));

	/* FGT_0_CUR_Y_CRC_INIT */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_Y_CRC_INIT) &= ~(
		BCHP_MASK(FGT_0_CUR_Y_CRC_INIT, INIT_VALUE));
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_Y_CRC_INIT) |= (
		BCHP_FIELD_DATA(FGT_0_CUR_Y_CRC_INIT, INIT_VALUE, 0xffffffff));

	/* FGT_0_CUR_U_CRC_INIT */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_U_CRC_INIT) &= ~(
		BCHP_MASK(FGT_0_CUR_U_CRC_INIT, INIT_VALUE));
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_U_CRC_INIT) |= (
		BCHP_FIELD_DATA(FGT_0_CUR_U_CRC_INIT, INIT_VALUE, 0xffffffff));

	/* FGT_0_CUR_V_CRC_INIT */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_V_CRC_INIT) &= ~(
		BCHP_MASK(FGT_0_CUR_V_CRC_INIT, INIT_VALUE));
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_V_CRC_INIT) |= (
		BCHP_FIELD_DATA(FGT_0_CUR_V_CRC_INIT, INIT_VALUE, 0xffffffff));

	/* FGT_0_CUR_CRC_MODE */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_CRC_MODE) &= ~(
		BCHP_MASK(FGT_0_CUR_CRC_MODE, CRC_MODE));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_CRC_MODE) |= (
		BCHP_FIELD_DATA(FGT_0_CUR_CRC_MODE, CRC_MODE, 0));

	/* FGT_0_CUR_CRC_ENABLE */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_CRC_ENABLE) &= ~(
		BCHP_MASK(FGT_0_CUR_CRC_ENABLE, CRC_ENABLE));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_CRC_ENABLE) |= (
		BCHP_FIELD_DATA(FGT_0_CUR_CRC_ENABLE, CRC_ENABLE, 0));

	/* FGT_0_CUR_AVG_BASE_ADRS */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_AVG_BASE_ADRS) &= ~(
		BCHP_MASK(FGT_0_CUR_AVG_BASE_ADRS, AVERAGE_BASE_ADRS));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_AVG_BASE_ADRS) |= (
		BCHP_FIELD_DATA(FGT_0_CUR_AVG_BASE_ADRS, AVERAGE_BASE_ADRS, 0));

	/* FGT_0_NXT_HORIZ_CROP */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_HORIZ_CROP) &= ~(
		BCHP_MASK(FGT_0_NXT_HORIZ_CROP, LEFT_CROPPING ) |
		BCHP_MASK(FGT_0_NXT_HORIZ_CROP, RIGHT_CROPPING));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_HORIZ_CROP) |= (
		BCHP_FIELD_DATA(FGT_0_NXT_HORIZ_CROP, LEFT_CROPPING , 0) |
		BCHP_FIELD_DATA(FGT_0_NXT_HORIZ_CROP, RIGHT_CROPPING, 0));

	/* FGT_0_NXT_VERT_CROP */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_VERT_CROP) &= ~(
		BCHP_MASK(FGT_0_NXT_VERT_CROP, TOP_CROPPING) |
		BCHP_MASK(FGT_0_NXT_VERT_CROP, BOT_CROPPING));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_VERT_CROP) |= (
		BCHP_FIELD_DATA(FGT_0_NXT_VERT_CROP, TOP_CROPPING, 0) |
		BCHP_FIELD_DATA(FGT_0_NXT_VERT_CROP, BOT_CROPPING, 0));

	/* FGT_0_NXT_PRNG_POC */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_POC) &= ~(
		BCHP_MASK(FGT_0_NXT_PRNG_POC, POC));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_POC) |= (
		BCHP_FIELD_DATA(FGT_0_NXT_PRNG_POC, POC, 0));

	/* FGT_0_NXT_PRNG_POC_OFFSET */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_POC_OFFSET) &= ~(
		BCHP_MASK(FGT_0_NXT_PRNG_POC_OFFSET, POC_OFFSET));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_POC_OFFSET) |= (
		BCHP_FIELD_DATA(FGT_0_NXT_PRNG_POC_OFFSET, POC_OFFSET, 0));

	/* FGT_0_NXT_PRNG_COLOR_SEL */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_COLOR_SEL) &= ~(
		BCHP_MASK(FGT_0_NXT_PRNG_COLOR_SEL, Y_COLOR_OFFSET_SELECT) |
		BCHP_MASK(FGT_0_NXT_PRNG_COLOR_SEL, U_COLOR_OFFSET_SELECT) |
		BCHP_MASK(FGT_0_NXT_PRNG_COLOR_SEL, V_COLOR_OFFSET_SELECT));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_COLOR_SEL) |= (
		BCHP_FIELD_DATA(FGT_0_NXT_PRNG_COLOR_SEL, Y_COLOR_OFFSET_SELECT, 0) |
		BCHP_FIELD_DATA(FGT_0_NXT_PRNG_COLOR_SEL, U_COLOR_OFFSET_SELECT, 1) |
		BCHP_FIELD_DATA(FGT_0_NXT_PRNG_COLOR_SEL, V_COLOR_OFFSET_SELECT, 2));

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Fgt_Init
	( BVDC_P_Fgt_Handle                hFgt )
{
	BDBG_ENTER(BVDC_P_Fgt_Init);
	BDBG_OBJECT_ASSERT(hFgt, BVDC_FGT);

	/* Initialize state. */
	hFgt->bInitial = true;

	/* Clear out shadow registers. */
	BKNI_Memset((void*)hFgt->aulRegs, 0x0, sizeof(hFgt->aulRegs));

	BVDC_P_Fgt_RegInit(hFgt);

	BDBG_LEAVE(BVDC_P_Fgt_Init);
	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Fgt_BuildRul_DrainVnet_isr
	( BVDC_P_Fgt_Handle              hFgt,
	  BVDC_P_ListInfo               *pList )
{
	/* reset sub and connect the module to a drain */
	BVDC_P_SubRul_StartDrain_isr(&(hFgt->SubRul), pList,
		hFgt->ulResetRegAddr, hFgt->ulResetMask);

	/* disconnect the module from the drain; reset it again; and finally drain
	 * the free-channel or loop-back used to to patch the vnet. */
	BVDC_P_SubRul_FinishDrain_isr(&(hFgt->SubRul), pList,
		hFgt->ulResetRegAddr, hFgt->ulResetMask, 0, 0);

	return;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_P_Fgt_Create
	( BVDC_P_Fgt_Handle          *phFgt,
	  BVDC_P_FgtId                eFgtId,
	  BREG_Handle                 hRegister,
	  BMEM_Handle                 hMem,
	  BVDC_Handle                 hVdc,
	  BVDC_P_Resource_Handle      hResource )
{
	BERR_Code          eResult = BERR_SUCCESS;
	BVDC_P_FgtContext *pFgt;

	BDBG_ENTER(BVDC_P_Fgt_Create);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* check parameters */
	if(eFgtId >= BVDC_P_FgtId_eUnknown)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pFgt = (BVDC_P_FgtContext *) (BKNI_Malloc(sizeof(BVDC_P_FgtContext)));
	if( !pFgt )
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pFgt, 0x0, sizeof(BVDC_P_FgtContext));
	BDBG_OBJECT_SET(pFgt, BVDC_FGT);

	/* Set up and initialization. */
	pFgt->eId                   = eFgtId;
	pFgt->hRegister             = hRegister;
	pFgt->hMem                  = hMem;
	pFgt->hWindow               = NULL;
	pFgt->hVdc                  = hVdc;
	pFgt->ulRegOffset           = 0;

	/* reset address */
	pFgt->ulResetRegAddr = BCHP_MMISC_SOFT_RESET;
	pFgt->ulResetMask    = BCHP_MMISC_SOFT_RESET_FGT_0_MASK;

	eResult = BMEM_Heap_ConvertAddressToOffset(pFgt->hMem,
		hVdc->pucFgtPatTblAddr, &pFgt->ulPatTableAddrOffset);
	if(eResult != BERR_SUCCESS)
		return BERR_TRACE(eResult);

	BVDC_P_Fgt_Init(pFgt);

	/* init the SubRul sub-module */
	BVDC_P_SubRul_Init(&(pFgt->SubRul), BVDC_P_Fgt_MuxAddr(pFgt),
		BVDC_P_Fgt_PostMuxValue(pFgt), BVDC_P_DrainMode_eBack,
		0, hResource);

	*phFgt = pFgt;

	BDBG_LEAVE(BVDC_P_Fgt_Create);
	return eResult;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_P_Fgt_Destroy
	( BVDC_P_Fgt_Handle          hFgt )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Fgt_Destroy);
	BDBG_OBJECT_ASSERT(hFgt, BVDC_FGT);

	BDBG_OBJECT_DESTROY(hFgt, BVDC_FGT);
	BKNI_Free((void*)hFgt);

	BDBG_LEAVE(BVDC_P_Fgt_Destroy);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_P_Fgt_AcquireConnect
	( BVDC_P_Fgt_Handle                 hFgt,
	  BVDC_Window_Handle                hWindow,
	  const BVDC_P_Window_Info         *pCurInfo )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Fgt_AcquireConnect);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);


	 hFgt->hWindow       = hWindow;
	 hFgt->eId           = hWindow->eId;
	 hFgt->pUserSettings = (BVDC_Fgt_Settings *)&pCurInfo->stFgtSettings;


	BDBG_LEAVE(BVDC_P_Fgt_AcquireConnect);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_P_Fgt_ReleaseConnect_isr
	( BVDC_P_Fgt_Handle         *phFgt )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Fgt_ReleaseConnect_isr);

	/* handle validation */
	if (NULL != *phFgt)
	{
		BDBG_OBJECT_ASSERT(*phFgt, BVDC_FGT);

		/* another win might still using it */
		BVDC_P_Resource_ReleaseHandle_isr(
			BVDC_P_SubRul_GetResourceHandle_isr(&(*phFgt)->SubRul),
			BVDC_P_ResourceType_eFgt, (void *)(*phFgt));

		BDBG_ERR(("Release FGT %d", (*phFgt)->eId));

		(*phFgt)->hWindow = NULL;
		/* this makes win to stop calling fgt code */
		*phFgt = NULL;
	}

	BDBG_LEAVE(BVDC_P_Fgt_ReleaseConnect_isr);
	return BERR_TRACE(eResult);
}


/***************************************************************************
 *
 */
BERR_Code BVDC_P_Fgt_SetEnable_isr
	( BVDC_P_Fgt_Handle                 hFgt,
	  bool                              bEnable )
{
	BDBG_ENTER(BVDC_P_Fgt_SetEnable_isr);
	BDBG_OBJECT_ASSERT(hFgt, BVDC_FGT);

	/* FGT_0_CUR_PIC_CTRL */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_PIC_CTRL) &= ~(
		BCHP_MASK(FGT_0_CUR_PIC_CTRL, ENABLE_MODE) |
		BCHP_MASK(FGT_0_CUR_PIC_CTRL, ENABLE     ));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_PIC_CTRL) |=  (
		BCHP_FIELD_DATA(FGT_0_CUR_PIC_CTRL, ENABLE_MODE, 0      ) |
		BCHP_FIELD_DATA(FGT_0_CUR_PIC_CTRL, ENABLE     , bEnable));

	BDBG_LEAVE(BVDC_P_Fgt_SetEnable_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_Fgt_SetPictureRect_isr
	( BVDC_P_Fgt_Handle                 hFgt,
	  const BVDC_P_Rect                *pCurFgtIn,
	  const BVDC_P_Rect                *pNextFgtOut,
	  bool                              bInterlaced )
{
	BDBG_ENTER(BVDC_P_Fgt_SetPictureRect_isr);
	BDBG_OBJECT_ASSERT(hFgt, BVDC_FGT);

	/* FGT_0_CUR_PIC_SIZE */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_PIC_SIZE) &= ~(
		BCHP_MASK(FGT_0_CUR_PIC_SIZE, BVB_HSIZE) |
		BCHP_MASK(FGT_0_CUR_PIC_SIZE, BVB_VSIZE));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_CUR_PIC_SIZE) |=  (
		BCHP_FIELD_DATA(FGT_0_CUR_PIC_SIZE, BVB_HSIZE, pCurFgtIn->ulWidth) |
		BCHP_FIELD_DATA(FGT_0_CUR_PIC_SIZE, BVB_VSIZE, pCurFgtIn->ulHeight >> bInterlaced));

	/* FGT_0_NXT_PIC_SIZE */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PIC_SIZE) &= ~(
		BCHP_MASK(FGT_0_NXT_PIC_SIZE, HSIZE) |
		BCHP_MASK(FGT_0_NXT_PIC_SIZE, VSIZE));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PIC_SIZE) |=  (
		BCHP_FIELD_DATA(FGT_0_NXT_PIC_SIZE, HSIZE, pNextFgtOut->ulWidth) |
		BCHP_FIELD_DATA(FGT_0_NXT_PIC_SIZE, VSIZE, pNextFgtOut->ulHeight >> bInterlaced));

	BDBG_LEAVE(BVDC_P_Fgt_SetPictureRect_isr);
	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code BVDC_P_Fgt_SetInfo_isr
	( BVDC_P_Fgt_Handle                 hFgt,
	  const BVDC_P_PictureNodePtr       pPicture )
{
	bool               bProgressive;
	uint32_t           ulFgtSeiBufAddr;
	BERR_Code          eResult = BERR_SUCCESS;
	const BVDC_P_Rect *pFgtIn, *pFgtOut;
	BVDC_SplitScreenMode      eFgtDemoMode;
	const BVDC_P_Window_Info *pCurInfo;

	BDBG_ENTER(BVDC_P_Fgt_SetInfo_isr);
	BDBG_OBJECT_ASSERT(hFgt, BVDC_FGT);
	BDBG_OBJECT_ASSERT(hFgt->hWindow, BVDC_WIN);
	BDBG_ASSERT(BVDC_P_VNET_USED_FGT(pPicture->stVnetMode));

	pFgtIn  = pPicture->pFgtIn;
	pFgtOut = pPicture->pFgtOut;

	BDBG_ASSERT(pFgtIn->ulWidth  == pFgtOut->ulWidth);
	BDBG_ASSERT(pFgtIn->ulHeight == pFgtOut->ulHeight);
	pCurInfo = &hFgt->hWindow->stCurInfo;
	eFgtDemoMode = pCurInfo->stSplitScreenSetting.eFgt;

	/* FGT_0_DEMO_SETTING */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_DEMO_SETTING) &= ~(
		BCHP_MASK(FGT_0_DEMO_SETTING, DEMO_BOUNDARY) |
		BCHP_MASK(FGT_0_DEMO_SETTING, DEMO_L_R     ) |
		BCHP_MASK(FGT_0_DEMO_SETTING, DEBUG_MODE   ));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_DEMO_SETTING) |= (
		((BVDC_SplitScreenMode_eDisable == eFgtDemoMode)?
		 BCHP_FIELD_ENUM(FGT_0_DEMO_SETTING, DEBUG_MODE, NORMAL_MODE) :
		 BCHP_FIELD_ENUM(FGT_0_DEMO_SETTING, DEBUG_MODE, DEMO_MODE))    |
		((BVDC_SplitScreenMode_eLeft == eFgtDemoMode)?
		 BCHP_FIELD_ENUM(FGT_0_DEMO_SETTING, DEMO_L_R, LEFT) :
		 BCHP_FIELD_ENUM(FGT_0_DEMO_SETTING, DEMO_L_R, RIGHT)) |
		BCHP_FIELD_DATA(FGT_0_DEMO_SETTING, DEMO_BOUNDARY, pFgtIn->ulWidth / 2));

	/* FGT_0_NXT_PIC_CTRL */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PIC_CTRL) &= ~(
		BCHP_MASK(FGT_0_NXT_PIC_CTRL, INPUT_PICTURE_FORMAT) |
		BCHP_MASK(FGT_0_NXT_PIC_CTRL, OUTPUT_PICTURE_FORMAT     ) |
		BCHP_MASK(FGT_0_NXT_PIC_CTRL, TOP_FIELD   ));

	/* TODO: Double check PICTURE_FORMAT setting */
	bProgressive = (pPicture->eDisplayPolarity == BAVC_Polarity_eFrame);
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PIC_CTRL) |= (
		BCHP_FIELD_DATA(FGT_0_NXT_PIC_CTRL, INPUT_PICTURE_FORMAT , bProgressive) |
		BCHP_FIELD_DATA(FGT_0_NXT_PIC_CTRL, OUTPUT_PICTURE_FORMAT, bProgressive) |
		BCHP_FIELD_DATA(FGT_0_NXT_PIC_CTRL, TOP_FIELD            , pPicture->eDisplayPolarity));

	/* FGT_0_NXT_SEI_BASE_ADRS */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_SEI_BASE_ADRS) &= ~(
		BCHP_MASK(FGT_0_NXT_SEI_BASE_ADRS, SEI_MSG_BASE_ADRS));

	/* SEI data is only valid for 1 frame. Use the window copy for now.
	 * Use the data in pPicture later when XVD can hold SEI data longer */
#if 0
	ulFgtSeiBufAddr = pPicture->ulFgtSeiBufAddr >> 5;
#else
	ulFgtSeiBufAddr = hFgt->hWindow->ulFgtSeiBufAddr >> 5;
#endif
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_SEI_BASE_ADRS) |= (
		BCHP_FIELD_DATA(FGT_0_NXT_SEI_BASE_ADRS, SEI_MSG_BASE_ADRS,
		ulFgtSeiBufAddr));

	/* FGT_0_NXT_PRNG_POC */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_POC) &= ~(
		BCHP_MASK(FGT_0_NXT_PRNG_POC, POC));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_POC) |= (
		BCHP_FIELD_DATA(FGT_0_NXT_PRNG_POC, POC, pPicture->ulPicOrderCnt));

	/* FGT_0_NXT_PRNG_POC_OFFSET */
	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_POC_OFFSET) &= ~(
		BCHP_MASK(FGT_0_NXT_PRNG_POC_OFFSET, POC_OFFSET));

	BVDC_P_FGT_GET_REG_DATA(FGT_0_NXT_PRNG_POC_OFFSET) |= (
		BCHP_FIELD_DATA(FGT_0_NXT_PRNG_POC_OFFSET, POC_OFFSET, pPicture->ulIdrPicID));

	BVDC_P_Fgt_SetEnable_isr(hFgt, true);
	BVDC_P_Fgt_SetPictureRect_isr(hFgt, pFgtIn, pFgtOut, !bProgressive);

	BDBG_LEAVE(BVDC_P_Fgt_SetInfo_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
void BVDC_P_Fgt_BuildRul_isr
	( BVDC_P_Fgt_Handle                 hFgt,
	  BVDC_P_ListInfo                  *pList,
	  BVDC_P_State                      eVnetState,
	  BVDC_P_PicComRulInfo             *pPicComRulInfo )
{
	uint32_t                 ulRulOpsFlags;
	BVDC_P_Window_Info      *pCurInfo;
	BVDC_P_Window_DirtyBits *pCurDirty;

	BDBG_OBJECT_ASSERT(hFgt, BVDC_FGT);
	BDBG_OBJECT_ASSERT(hFgt->hWindow, BVDC_WIN);

	pCurInfo  = &hFgt->hWindow->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;

	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hFgt->SubRul), pPicComRulInfo->eWin, eVnetState, pList->bLastExecuted);

	if ((0 == ulRulOpsFlags) || (ulRulOpsFlags & BVDC_P_RulOp_eReleaseHandle))
	{
		return;
	}
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_SubRul_DropOffVnet_isr(&(hFgt->SubRul), pList);
		BVDC_P_Fgt_SetEnable_isr(hFgt, false);
		BVDC_P_FGT_WRITE_TO_RUL(FGT_0_CUR_PIC_CTRL, pList->pulCurrent);
	}

	/* reset FGT */
	if(hFgt->bInitial)
	{
		BDBG_MSG(("Reset FGT "));
		BVDC_P_BUILD_RESET(pList->pulCurrent,
			hFgt->ulResetRegAddr, hFgt->ulResetMask);
		hFgt->bInitial = false;
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eEnable)
	{
		BVDC_P_FGT_BLOCK_WRITE_TO_RUL(FGT_0_CONTROL,
			FGT_0_CUR_PIC_SIZE, pList->pulCurrent);

		BVDC_P_FGT_BLOCK_WRITE_TO_RUL(FGT_0_CUR_Y_CRC_INIT,
			FGT_0_NXT_PIC_SIZE, pList->pulCurrent);

		/* must be the last, and every field */
		BVDC_P_FGT_WRITE_TO_RUL(FGT_0_CUR_PIC_CTRL, pList->pulCurrent);

		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			BVDC_P_SubRul_JoinInVnet_isr(&(hFgt->SubRul), pList);
		}
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
	{
		BVDC_P_Fgt_BuildRul_DrainVnet_isr(hFgt, pList);
	}

}

#else
/***************************************************************************/
/* No support for any FGT */
BERR_Code BVDC_P_Fgt_Create
	( BVDC_P_Fgt_Handle          *phFgt,
	  BVDC_P_FgtId                eFgtId,
	  BREG_Handle                 hRegister,
	  BMEM_Handle                 hMem,
	  BVDC_Handle                 hVdc,
	  BVDC_P_Resource_Handle      hResource )
{
	BSTD_UNUSED(phFgt);
	BSTD_UNUSED(eFgtId);
	BSTD_UNUSED(hRegister);
	BSTD_UNUSED(hMem);
	BSTD_UNUSED(hVdc);
	BSTD_UNUSED(hResource);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Fgt_Destroy
	( BVDC_P_Fgt_Handle                 hFgt )
{
	BSTD_UNUSED(hFgt);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Fgt_AcquireConnect
	( BVDC_P_Fgt_Handle                 hFgt,
	  BVDC_Window_Handle                hWindow,
	  const BVDC_P_Window_Info         *pCurInfo )
{
	BSTD_UNUSED(hFgt);
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pCurInfo);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Fgt_ReleaseConnect_isr
	( BVDC_P_Fgt_Handle                *phFgt )
{
	BSTD_UNUSED(phFgt);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Fgt_SetInfo_isr
	( BVDC_P_Fgt_Handle                 hFgt,
	  const BVDC_P_PictureNodePtr       pPicture )
{
	BSTD_UNUSED(hFgt);
	BSTD_UNUSED(pPicture);
	return BERR_SUCCESS;
}

void BVDC_P_Fgt_BuildRul_isr
	( BVDC_P_Fgt_Handle                 hFgt,
	  BVDC_P_ListInfo                  *pList,
	  BVDC_P_State                      eVnetState,
	  BVDC_P_PicComRulInfo             *pPicComRulInfo )
{
	BSTD_UNUSED(hFgt);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eVnetState);
	BSTD_UNUSED(pPicComRulInfo);
	return;
}

#endif


/* End of file. */

