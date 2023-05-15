/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_656in_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 8/13/12 3:06p $
 *
 * Module Description:
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_656in_priv.c $
 * 
 * Hydra_Software_Devel/20   8/13/12 3:06p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 * 
 * Hydra_Software_Devel/19   11/11/10 7:27p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 * 
 * Hydra_Software_Devel/18   5/7/10 7:07p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 * 
 * Hydra_Software_Devel/17   4/19/10 10:10p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/16   4/7/10 11:23a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/15   4/5/10 3:57p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/14   4/17/09 4:31p yuxiaz
 * PR54135: When change window size from full screen, soucepending
 * callback won't fire.
 *
 * Hydra_Software_Devel/13   4/17/09 11:57a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/12   4/10/09 6:33p hongtaoz
 * PR54064: source pending callback must fire after window shutdown;
 *
 * Hydra_Software_Devel/11   12/18/08 8:52p pntruong
 * PR50502: Used correct display/source rate for computing the number of
 * buffers.
 *
 * Hydra_Software_Devel/11   12/18/08 8:45p pntruong
 * PR50502: Used correct display/source rate for computing the number of
 * buffers.
 *
 * Hydra_Software_Devel/10   6/18/08 3:05p syang
 * PR35549: 1). src-pending when new win is created; 2). don't do
 * BufferHeap_AllocateBuffers as bReConfigSrc
 *
 * Hydra_Software_Devel/9   5/8/08 5:53p pntruong
 * PR35549: Added more preparations for source pending.
 *
 * Hydra_Software_Devel/8   5/7/08 11:28a pntruong
 * PR35549: Hook up the source callback event first so that app can start
 * using it while adding the internal pending.
 *
 * Hydra_Software_Devel/7   3/26/08 1:13p hongtaoz
 * PR35398, PR40895: added ulVsyncRate and removed bGameModeDelayLags in
 * the window lipsync callback structure; added BFMT_FREQ_FACTOR to unify
 * Vsync rate and pixel frequency calculation;
 *
 * Hydra_Software_Devel/6   2/21/08 4:59p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/5   5/10/07 3:21p syang
 * PR 30617: add bStartFeed to src for muting writer
 *
 * Hydra_Software_Devel/4   3/28/07 11:14p pntruong
 * PR28395: Provide API to customize VDEC video quality.  Cleanup dirty
 * meanings.
 *
 * Hydra_Software_Devel/3   1/9/07 6:19p pntruong
 * PR23222: VDEC - ATSC input support.  Correct the system clock for vdec.
 *
 * Hydra_Software_Devel/2   12/18/06 11:39p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   11/21/06 4:17p pntruong
 * PR26068: Sometimes, high level may miss the Generic callback.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/19/06 10:58a pntruong
 * PR23225: VDEC - PC input.  Added code to program the pll clock for
 * different pc input format.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/29/06 1:55p yuxiaz
 * PR24262: Added 656in support for 3563.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/21/06 5:14p yuxiaz
 * PR24262: Use offset for various 656 sub-modules.
 *
 * Hydra_Software_Devel/1   9/14/06 9:59a yuxiaz
 * PR24262: Separate VDC 656 code from VDEC for new 656 core.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"
#include "bvdc_priv.h"
#include "bvdc_656in_priv.h"
#include "bvdc_window_priv.h"

/***************************************************************************/
/* Has some support for at least one VDEC? */
#if (BVDC_P_SUPPORT_NEW_656_IN_VER)

#include "bvdc_displayfmt_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_656in_priv.h"

#include "bchp_ext_656_top_0.h"
#if (BVDC_P_NUM_656IN_SUPPORT > 1)
#include "bchp_ext_656_top_1.h"
#endif

BDBG_MODULE(BVDC_656);
BDBG_OBJECT_ID(BVDC_656);

/***************************************************************************
 *
 */
static void BVDC_P_656In_BuildVsyncRul_isr
	( BVDC_P_656In_Handle              h656In,
	  BVDC_P_ListInfo                 *pList )
{
	if(h656In->bVideoDetected)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_host_enable + h656In->ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_host_enable, HOST_ENABLE, 0x1 );
	}

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_656In_BuildResetRul_Isr
	( const BVDC_P_656In_Handle        h656In,
	  BVDC_P_ListInfo                 *pList)
{

#if (BVDC_P_SUPPORT_NEW_656_IN_VER != BVDC_P_656IN_NEW_VER_2)
	/* EXT_656_TOP_0_ext_656_reset (W) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_reset + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_reset, reset, 1);

	/* wait for 3 clocks */
	*pList->pulCurrent++ = BRDC_OP_NOP();
	*pList->pulCurrent++ = BRDC_OP_NOP();
	*pList->pulCurrent++ = BRDC_OP_NOP();

	/* EXT_656_TOP_0_ext_656_reset (W) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_reset + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_reset, reset, 0);
#endif

	/* EXT_656_TOP_0_ext_656_control (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_control + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_control, reserved0,               0       ) |
#if (BVDC_P_SUPPORT_NEW_656_IN_VER == BVDC_P_656IN_NEW_VER_1)
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_control, reserved_for_eco1,       0       ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_control, reserved_for_eco2,       0x7     ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_control, reserved_for_eco3,       0       ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_control, bvb_replace_ff,          DISABLED) |

#elif (BVDC_P_SUPPORT_NEW_656_IN_VER == BVDC_P_656IN_NEW_VER_2)
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_control, reserved_for_eco1,       0       ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_control, reserved_for_eco2,       0x7     ) |

#elif (BVDC_P_SUPPORT_NEW_656_IN_VER == BVDC_P_656IN_NEW_VER_3)
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_control, field_1_count_enable,    ENABLED ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_control, field_0_count_enable,    ENABLED ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_control, itu656_sav_msb_polarity, ONE     ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_control, hibernation_enabled,     DISABLED) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_control, bvb_replace_ff,          DISABLED) |

#endif
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_control, data_width,              0       ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_control, bvb_output_enabled,      ENABLED );


#if(BVDC_P_SUPPORT_BVN_10BIT_444)
	/* Up sample 656 to 444 */
	/* EXT_656_TOP_0_ext_656_conv_control (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_conv_control + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_conv_control, reserved0,          0       ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_conv_control, FILTER_SELECT,      TEN_TAP ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_conv_control, reserved_for_eco1,  0       ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_conv_control, SUPPRESSION_MODE,   NORMAL  ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_conv_control, SUPPRESSION_ENABLE, ENABLED );
#endif

	/* EXT_656_TOP_0_ext_656_host_legacy (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_host_legacy + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_host_legacy , reserved0,           0) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_host_legacy , reserved1,           0) |
#if (BVDC_P_SUPPORT_NEW_656_IN_VER == BVDC_P_656IN_NEW_VER_3)
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_host_legacy , USE_HOST_EN_DIS,     0) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_host_legacy , USE_DISCARD_PICTURE, 0) |
#endif
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_host_legacy , USE_LEGACY_TRIGGER,  0);


	/* EXT_656_TOP_0_ext_656_interrupt (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_interrupt + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_interrupt, reserved0, 0   ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_interrupt, field_1,   0x16) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_interrupt, reserved1, 0   ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_interrupt, field_0,   0x16);

	/* EXT_656_TOP_0_ext_656_trigger (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_trigger + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_trigger , reserved0,      0      ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_trigger , field_1_enable, ENABLED) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_trigger , field_1,        0x6    ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_trigger , reserved1,      0      ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_trigger , field_0_enable, ENABLED) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_trigger , field_0,        0x6);

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_656In_BuildFormatRul_Isr
	( const BVDC_P_656In_Handle        h656In,
	  BVDC_P_ListInfo                 *pList)
{
	uint32_t                     ulVSize, ulHSize;
	uint32_t                     ulAutoRepeat, ulPicDelay;
	const BFMT_VideoInfo        *pFmtInfo;

	BDBG_OBJECT_ASSERT(h656In->hSource, BVDC_SRC);

	/* Current settings */
	pFmtInfo = h656In->hSource->stCurInfo.pFmtInfo;

	BDBG_ASSERT(
		VIDEO_FORMAT_IS_PAL(pFmtInfo->eVideoFmt) ||
		VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt));

	ulHSize = pFmtInfo->ulDigitalWidth / 2;
	ulVSize = (pFmtInfo->bInterlaced)
		? (pFmtInfo->ulDigitalHeight / BVDC_P_FIELD_PER_FRAME)
		: (pFmtInfo->ulDigitalHeight);

	/* New trigger mode.  How often to repeat the trigger in sysclk cycle unit */
	ulAutoRepeat = (BVDC_P_108_SYSCLK / pFmtInfo->ulVertFreq) * BFMT_FREQ_FACTOR;
	ulPicDelay   = (ulAutoRepeat / pFmtInfo->ulScanHeight) * BVDC_P_656IN_TRIGGER_OFFSET;

#if (BVDC_P_SUPPORT_NEW_656_IN_VER <= BVDC_P_656IN_NEW_VER_2)
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_count + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_count , reserved0, 0    ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_count , vsize,     ulVSize) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_count , reserved1, 0    ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_count , hsize,     ulHSize);
#else
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_short_count + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_short_count , reserved0           , 0        ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_short_count , vsize_compare_enable, ENABLED  ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_short_count , vsize               , ulVSize-1) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_short_count , reserved1           , 0        ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_short_count , hsize_compare_enable, ENABLED  ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_short_count , hsize               , ulHSize-1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_long_count + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_long_count , reserved0           , 0        ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_long_count , vsize_compare_enable, ENABLED  ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_long_count , vsize               , ulVSize  ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_long_count , reserved1           , 0        ) |
		BCHP_FIELD_ENUM(EXT_656_TOP_0_ext_656_long_count , hsize_compare_enable, ENABLED  ) |
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_long_count , hsize               , ulHSize  );
#endif

	/* EXT_656_TOP_0_ext_656_auto_repeat (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_auto_repeat + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_auto_repeat , AUTO_REPEAT, ulAutoRepeat);

	/* EXT_656_TOP_0_ext_656_picture_delay (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_EXT_656_TOP_0_ext_656_picture_delay + h656In->ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_picture_delay, PICTURE_DELAY, ulPicDelay );

	BDBG_MSG(("New entries added by 656 Build Format RUL"));
	return;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_P_656In_Create
	( BVDC_P_656In_Handle             *ph656In,
	  BVDC_P_656Id                     e656Id,
	  BVDC_Source_Handle               hSource )
{
	BVDC_P_656InContext   *p656In;

	BDBG_ENTER(BVDC_P_656In_Create);
	BDBG_ASSERT(ph656In);

	/* BDBG_SetModuleLevel("BVDC_656", BDBG_eMsg);  */

	/* Get Source context */
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* The handle will be NULL if create fails. */
	*ph656In = NULL;

	/* (1) Alloc the context. */
	p656In = (BVDC_P_656InContext*)
		(BKNI_Malloc(sizeof(BVDC_P_656InContext)));
	if(!p656In)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)p656In, 0x0, sizeof(BVDC_P_656InContext));
	BDBG_OBJECT_SET(p656In, BVDC_656);

	/* Store the id & hRegister for activating the triggers. */
	p656In->eId                   = e656Id;
	p656In->hSource               = hSource;

#if (BVDC_P_NUM_656IN_SUPPORT > 1)
	/* Getting the offset of various 656 sub-modules.  The offset is
	 * from the 656_0.*/
	if(p656In->eId ==  BVDC_P_656Id_e656In1)
	{
		p656In->ulOffset = BCHP_EXT_656_TOP_1_ext_656_rev_id
			- BCHP_EXT_656_TOP_0_ext_656_rev_id;
	}
#endif

	/* (2) All done. now return the new fresh context to user. */
	*ph656In = (BVDC_P_656In_Handle)p656In;
	BVDC_P_656In_Init(*ph656In);

	BDBG_LEAVE(BVDC_P_656In_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_656In_Destroy
	( BVDC_P_656In_Handle              h656In )
{
	BDBG_ENTER(BVDC_P_656In_Destroy);
	BDBG_OBJECT_ASSERT(h656In, BVDC_656);

	BDBG_OBJECT_DESTROY(h656In, BVDC_656);
	/* [1] Release context in system memory */
	BKNI_Free((void*)h656In);

	BDBG_LEAVE(BVDC_P_656In_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_656In_Init
	( BVDC_P_656In_Handle              h656In )
{
	BDBG_ENTER(BVDC_P_656In_Init);
	BDBG_OBJECT_ASSERT(h656In, BVDC_656);

	/* 656In states */
	h656In->bVideoDetected    = false;
	h656In->ulDelayStart      = BVDEC_P_BVB_OUT_SLOW_START_COUNT;
	h656In->eFrameRateCode    = BAVC_FrameRateCode_e29_97;

	BDBG_LEAVE(BVDC_P_656In_Init);
	return;
}


/***************************************************************************
 * {private}
 *
 * Update Vdec status.
 */
void BVDC_P_656In_UpdateStatus_isr
	( BVDC_P_656In_Handle              h656In )
{
	bool                      bVideoDetected;
	BVDC_P_Source_Info       *pCurInfo;
	BVDC_P_Source_DirtyBits  *pCurDirty;

	BDBG_OBJECT_ASSERT(h656In, BVDC_656);
	BDBG_OBJECT_ASSERT(h656In->hSource, BVDC_SRC);
	pCurInfo  = &h656In->hSource->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;

	/* Video is probably garbage when sourc is unplugpged, and need vec
	 * to drive triggers. */
	bVideoDetected =
		(BVDC_P_TriggerCtrl_eSource == h656In->hSource->eTrigCtrl) ? true : false;
	if(h656In->bVideoDetected != bVideoDetected)
	{
		if(bVideoDetected)
		{
			BDBG_MSG(("656in detects input"));
			h656In->ulDelayStart = BVDEC_P_BVB_OUT_SLOW_START_COUNT;
			h656In->eFrameRateCode = VIDEO_FORMAT_IS_525_LINES(pCurInfo->pFmtInfo->eVideoFmt)?
				BAVC_FrameRateCode_e29_97 : BAVC_FrameRateCode_e25;
		}
		h656In->bVideoDetected = bVideoDetected;
		pCurDirty->stBits.bVideoDetected = BVDC_P_DIRTY;
		h656In->hSource->bStartFeed = bVideoDetected;
	}

	return;
}


/***************************************************************************
 * {private}
 *
 * Get Vdec status.
 */
void BVDC_P_656In_GetStatus_isr
	( const BVDC_P_656In_Handle        h656In,
	  bool                            *pbVideoDetected )
{
	BDBG_OBJECT_ASSERT(h656In, BVDC_656);

	/* (2) VDEC's input video detected? */
	if(pbVideoDetected)
	{
		*pbVideoDetected = h656In->bVideoDetected && (!h656In->ulDelayStart);
	}

	return;
}


/***************************************************************************
 *
 */
void BVDC_P_656In_Bringup_isr
	( const BVDC_P_656In_Handle        h656In )
{
	uint32_t               i;
	BRDC_List_Handle       hList;
	BRDC_Slot_Handle       hSlot;
	BVDC_P_ListInfo        stList, *pList;

	BDBG_OBJECT_ASSERT(h656In, BVDC_656);
	BDBG_OBJECT_ASSERT(h656In->hSource, BVDC_SRC);

	/* Just build on the top field only!  This is a force execution. */
	BVDC_P_SRC_NEXT_RUL(h656In->hSource, BAVC_Polarity_eTopField);
	hSlot = BVDC_P_SRC_GET_SLOT(h656In->hSource, BAVC_Polarity_eTopField);
	hList = BVDC_P_SRC_GET_LIST(h656In->hSource, BAVC_Polarity_eTopField);

	/* This will arm all the slot with trigger executions. */
	for(i = 0; i < h656In->hSource->ulSlotUsed; i++)
	{
		BRDC_Slot_ExecuteOnTrigger_isr(h656In->hSource->ahSlot[i],
			h656In->hSource->aeTrigger[i], true);
	}

	/* Get the list infos */
	pList = &stList;
	BVDC_P_ReadListInfo_isr(pList, hList);

	/* Build drian RUL .  Only need to build it once. */
	BVDC_P_Drain_BuildRul_isr(&h656In->hSource->stDrain, pList);

	/* Build reset RUL */
	BVDC_P_656In_BuildResetRul_Isr(h656In, pList);

	/* Build format RUL */
	BVDC_P_656In_BuildFormatRul_Isr(h656In, pList);

	/* Build vsync RUL */
	BVDC_P_656In_BuildVsyncRul_isr(h656In, pList);

	/* Update the list */
	BVDC_P_WriteListInfo_isr(pList, hList);

	/* Initial bringup list */
	BRDC_Slot_SetList_isr(hSlot, hList);
	BRDC_Slot_Execute_isr(hSlot);

	BDBG_MSG(("Bringup source[%d].", h656In->hSource->eId));
	return;

}

/***************************************************************************
 *
 */
void BVDC_P_656In_BuildRul_isr
	( const BVDC_P_656In_Handle        h656In,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId )
{
	BVDC_P_Source_Info       *pCurInfo;
	BVDC_P_Source_DirtyBits  *pCurDirty, *pOldDirty;

	BDBG_OBJECT_ASSERT(h656In, BVDC_656);
	BDBG_OBJECT_ASSERT(h656In->hSource, BVDC_SRC);

	pOldDirty = &h656In->hSource->astOldDirty[eFieldId];
	pCurInfo  = &h656In->hSource->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;

	/* Clear old dirty bits. */
	if(BVDC_P_IS_DIRTY(pOldDirty))
	{
		if(!pList->bLastExecuted)
		{
			BVDC_P_OR_ALL_DIRTY(pCurDirty, pOldDirty);
		}
		else
		{
			BVDC_P_CLEAN_ALL_DIRTY(pOldDirty);
		}
	}

	/* Let's see what we need to build. */
	if(BVDC_P_IS_DIRTY(pCurDirty) || h656In->hSource->bDeferSrcPendingCb)
	{
		BDBG_MSG(("eVideoFmt      = %s, dirty = %d",
			pCurInfo->pFmtInfo->pchFormatStr, pCurDirty->stBits.bInputFormat));
		BDBG_MSG(("pfGenCallback  = %d, dirty = %d",
			pCurInfo->pfGenericCallback, pCurDirty->stBits.bGenCallback));
		BDBG_MSG(("bVideoDetected = %d, dirty = %d",
			h656In->bVideoDetected, pCurDirty->stBits.bVideoDetected));
		BDBG_MSG(("eFrameRateCode = %d, dirty = %d,",
			h656In->eFrameRateCode, pCurDirty->stBits.bFrameRateCode));
		BDBG_MSG(("------------------------------intP%d", eFieldId));

		/* Note: The order is important because build NTSC/PAL contains a
		 * reset, so it must precede other build ruls. */
		if(pCurDirty->stBits.bInputFormat)
		{
			h656In->hSource->ulVertFreq = pCurInfo->pFmtInfo->ulVertFreq;
			BVDC_P_656In_BuildResetRul_Isr(h656In, pList);
			BVDC_P_656In_BuildFormatRul_Isr(h656In, pList);
			BVDC_P_Drain_BuildFormatRul_isr(&h656In->hSource->stDrain,
				&h656In->hSource->stScanOut, pCurInfo->pFmtInfo, pList);
		}

		/* Callback when following things changes:
		 * 1) Macrovision detection
		 * 2) New format detected
		 * 3) Video status */
		if((pCurInfo->pfGenericCallback) &&
		   ((pCurDirty->stBits.bGenCallback)   ||
		    (pCurDirty->stBits.bInputFormat)   ||
		    (pCurDirty->stBits.bVideoDetected) ||
		    (pCurDirty->stBits.bFrameRateCode) ||
		    (h656In->hSource->bDeferSrcPendingCb) ||
		    (pCurDirty->stBits.bAddWin && pCurInfo->eResumeMode)))
		{
			BVDC_Source_CallbackData *pCbData = &h656In->hSource->stSourceCbData;
			BVDC_Source_DirtyBits *pCbDirty = &pCbData->stDirty;

			/* Clear dirty bits */
			BVDC_P_CB_CLEAN_ALL_DIRTY(pCbDirty);

			/* Issue src pending call back when shutdown BVN completed. */
			if(h656In->hSource->bDeferSrcPendingCb)
			{
				BVDC_P_Source_CheckAndIssueCallback(h656In->hSource, pCbDirty);
			}

			/* Make sure the callback happen at least once with status info,
			 * NOT the source pending on first
			 * installation of callback to report the current status. */
			if(pCurDirty->stBits.bGenCallback)
			{
				/* Which one triggers callback? */
				pCbDirty->bActive     = BVDC_P_DIRTY;
				pCbDirty->bFmtInfo    = BVDC_P_DIRTY;
				pCbDirty->bFrameRate  = BVDC_P_DIRTY;
				h656In->hSource->bDeferSrcPendingCb = true;
			}
			else
			{
				/* Which one triggers callback? */
				pCbDirty->bActive     = pCurDirty->stBits.bVideoDetected;
				pCbDirty->bFmtInfo    = pCurDirty->stBits.bInputFormat;
				pCbDirty->bFrameRate  = pCurDirty->stBits.bFrameRateCode;

				/* defer source pending callback until all its windows are shutdown! */
				if((pCurDirty->stBits.bInputFormat || pCurDirty->stBits.bAddWin) &&
				   (pCurInfo->eResumeMode))
				{
					h656In->hSource->bDeferSrcPendingCb = true;
				}
			}

			/* callback only if something changed */
			if(BVDC_P_CB_IS_DIRTY(pCbDirty))
			{
				/* Update Callback data */
				pCbData->bActive        = h656In->bVideoDetected;
				pCbData->pFmtInfo       = pCurInfo->pFmtInfo;
				pCbData->eFrameRateCode = h656In->eFrameRateCode;

				pCurInfo->pfGenericCallback(pCurInfo->pvGenericParm1,
					pCurInfo->iGenericParm2, (void*)pCbData);
			}
		}

		/* Clear dirty bits. */
		*pOldDirty = *pCurDirty;
		BVDC_P_CLEAN_ALL_DIRTY(pCurDirty);
	}

	/* This get build every vsync. */
	BVDC_P_656In_BuildVsyncRul_isr(h656In, pList);

	/* Countdown before start feeding out */
	if(h656In->ulDelayStart)
	{
		h656In->ulDelayStart--;
	}

	return ;
}
#endif

/* End of File */
