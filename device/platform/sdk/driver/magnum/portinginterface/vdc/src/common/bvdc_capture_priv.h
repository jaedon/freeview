/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_capture_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/60 $
 * $brcm_Date: 7/31/12 4:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_capture_priv.h $
 * 
 * Hydra_Software_Devel/60   7/31/12 4:55p yuxiaz
 * SW7425-1835: Use RDC slot timestamp for capture and playback timestamp
 * in multi-buffering algo.
 * 
 * Hydra_Software_Devel/59   3/29/12 11:18a tdo
 * SW7435-85: MVP_0 enable error.  Add support for new capture enable
 * control bit.
 *
 * Hydra_Software_Devel/58   12/1/11 4:31p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/SW7425-968/1   11/14/11 3:07p yuxiaz
 * SW7425-968: Added independent source clipping of right window in CAP
 * and SCL.
 *
 * Hydra_Software_Devel/57   11/11/10 7:12p hongtaoz
 * SW7425-51: reduced combo trigger use in VDC to 1 per MFD source;
 *
 * Hydra_Software_Devel/56   9/30/10 4:04p yuxiaz
 * SW7422-39: More on 3D video support in CAP and VFD.
 *
 * Hydra_Software_Devel/55   9/29/10 5:22p yuxiaz
 * SW7422-39: Added 3D video support in CAP and VFD.
 *
 * Hydra_Software_Devel/54   4/7/10 11:26a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/53   4/5/10 3:58p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/52   10/12/09 4:36p rpan
 * SW7405-3159: Moved writer/reader time stamp register from CAP/VFD to
 * RDC scratch registers.
 *
 * Hydra_Software_Devel/51   1/30/09 5:09p yuxiaz
 * PR51713: Reset CAP during DCX mode switch. Clean up DCX software
 * workaround.
 *
 * Hydra_Software_Devel/50   10/3/08 9:23a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/49   9/22/08 4:51p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/48   2/29/08 4:17p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/47   2/22/08 2:31p yuxiaz
 * PR39155: Added de-ringing filters in BVN.
 *
 * Hydra_Software_Devel/46   7/16/07 2:39p yuxiaz
 * PR29842: 7405 mosaic mode bring up.
 *
 * Hydra_Software_Devel/45   4/2/07 11:27a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/44   2/23/07 6:28p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/43   1/13/07 2:00p yuxiaz
 * PR20875: Remove BVDC_P_SUPPORT_BVN_10BIT_422, which is used to test 10
 * bit support on 3560B0.
 *
 * Hydra_Software_Devel/42   1/10/07 10:58a syang
 * PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
 * dnr, mfd, vfd, cap, ...
 *
 * Hydra_Software_Devel/41   12/18/06 11:18p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/16/06 1:59p hongtaoz
 * PR25668: merge in vdc support for 7403;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/28/06 4:15p yuxiaz
 * PR20875: Added 10bit BVN support for 3560 B0.
 *
 * Hydra_Software_Devel/39   8/28/06 1:44p yuxiaz
 * PR20875: Renamed BVDC_P_SUPPORT_10BIT_BVN to
 * BVDC_P_SUPPORT_BVN_10BIT_444. Removed BVDC_P_BVNDataMode, use window
 * and buffer node's pixel format. Integrated with new 10 bit pixel
 * formats.
 *
 * Hydra_Software_Devel/38   8/18/06 2:43p albertl
 * PR23117:  Added 7440 support.
 *
 * Hydra_Software_Devel/37   8/7/06 5:24p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/36   7/7/06 4:13p yuxiaz
 * PR 22296, PR 22108, PR22109: Removed
 * BVDC_Window_InstallLipSyncDelayCallback, added new generic window
 * callback function BVDC_Window_InstallCallback for lip sync.
 *
 * Hydra_Software_Devel/35   6/14/06 1:34p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/34   5/26/06 3:37p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/33   5/15/06 12:23p jessem
 * PR 17530: Added instruction to capture RUL such that the timer register
 * is read during RUL execution time instead of using BTMR_ReadTimer_isr.
 * Added BTMR_TimerRegister field to BVDC_P_Capture_Context.
 *
 * Hydra_Software_Devel/32   5/10/06 3:15p jessem
 * PR 17530: Added BTMR support. BVDC_P_Capture_BuildRul_isr stores
 * timestamp for use by BVDC_P_Window_Writer_isr. BVDC_P_Capture_Create
 * takes in 2 new parameters:BREG_Handle and BTMR_TimerHandle.
 *
 * Hydra_Software_Devel/31   3/7/06 3:58p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/30   1/17/06 4:17p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/29   1/12/06 1:33p hongtaoz
 * PR18233: added mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/2   12/16/05 5:06p hongtaoz
 * PR18233: added simul windows mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/1   12/8/05 6:50p hongtaoz
 * PR18233: add Mosaic mode support (single window working);
 *
 * Hydra_Software_Devel/28   6/28/05 11:01a jasonh
 * PR 16012: Adding support for 7401
 *
 * Hydra_Software_Devel/27   4/15/05 7:58p hongtaoz
 * PR14691: reset video window's path when shutdown;
 *
 * Hydra_Software_Devel/26   4/14/05 10:23a syang
 * PR 14692: re-organize picture's rectangles, fixed duplicated clips in
 * diff modules, changed to do clip as soon as possible, and added
 * capture clipping for C0
 *
 * Hydra_Software_Devel/25   3/17/05 6:35p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/24   11/23/04 8:53p pntruong
 * PR13076, PR11749: Video jitter under heavy system load.  Added RUL
 * execution check to reduce number of programmed registers.
 *
 * Hydra_Software_Devel/23   11/19/04 2:43p yuxiaz
 * PR 10855: Allow user to specify number of capture buffers in VDC
 * default settings. Additional work needed.
 *
 * Hydra_Software_Devel/22   7/6/04 8:28a pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 ***************************************************************************/
#ifndef BVDC_CAPTURE_PRIV_H__
#define BVDC_CAPTURE_PRIV_H__

#include "bvdc.h"
#include "bchp_common.h"
#include "bvdc_common_priv.h"
#include "bchp_cap_0.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_buffer_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Private defines
 ***************************************************************************/
/* 3560, 7401A, 7400A:
 * no BVB_IN_SIZE. PIC_OFFSET seperate from PIC_SIZE -> RX_CTRL block
 */
#define BVDC_P_CAP_VER_0                      (0)
/* 7401B, 7400B, 7403, 7118, 7440:
 * PIC_OFFSET -> BVB_IN_SIZE seperate from PIC_SIZE -> RX_CTRL block
 */
#define BVDC_P_CAP_VER_1                      (1)
/* 3563, 7405:
 * PIC_OFFSET and BVB_IN_SIZE in block: PIC_SIZE -> LINE_CMP_TRIG_1_CFG
 */
#define BVDC_P_CAP_VER_2                      (2)
/* 3548:
 * Dither support added.
 */
#define BVDC_P_CAP_VER_3                      (3)
/* 7422:
 * 3D support added.
 */
#define BVDC_P_CAP_VER_4                      (4)
/* 7425Bx, 7344Bx, 7231Bx, 7346Bx:
 * PIC_OFFSET_R added.
 */
#define BVDC_P_CAP_VER_5                      (5)
/* 7435:
 * CAP_x_CTRL.ENABLE_CTRL added.
 */
#define BVDC_P_CAP_VER_6                      (6)

/***************************************************************************
 * Private register cracking macros
 ***************************************************************************/
#define BVDC_P_CAP_GET_REG_IDX(reg) \
	((BCHP##_##reg - BCHP_CAP_0_REG_START) / sizeof(uint32_t))

/* Get/Set reg data */
#define BVDC_P_CAP_GET_REG_DATA(reg) \
	(hCapture->aulRegs[BVDC_P_CAP_GET_REG_IDX(reg)])
#define BVDC_P_CAP_SET_REG_DATA(reg, data) \
	(BVDC_P_CAP_GET_REG_DATA(reg) = (uint32_t)(data))

/* Get field */
#define BVDC_P_CAP_GET_FIELD_NAME(reg, field) \
	(BVDC_P_GET_FIELD(BVDC_P_CAP_GET_REG_DATA(reg), reg, field))

/* Compare field */
#define BVDC_P_CAP_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_COMPARE_FIELD_DATA(BVDC_P_CAP_GET_REG_DATA(reg), reg, field, (data)))
#define BVDC_P_CAP_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_COMPARE_FIELD_NAME(BVDC_P_CAP_GET_REG_DATA(reg), reg, field, name))

/* This macro does a write into RUL (write, addr, data). 3 dwords. */
#define BVDC_P_CAP_WRITE_TO_RUL(reg, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BCHP##_##reg + hCapture->ulRegOffset); \
	*addr_ptr++ = BVDC_P_CAP_GET_REG_DATA(reg); \
}

/* This macro does a block write into RUL */
#define BVDC_P_CAP_BLOCK_WRITE_TO_RUL(from, to, pulCurrent) \
do { \
	uint32_t ulBlockSize = \
		BVDC_P_REGS_ENTRIES(from, to);\
	*pulCurrent++ = BRDC_OP_IMMS_TO_REGS( ulBlockSize ); \
	*pulCurrent++ = BRDC_REGISTER(BCHP##_##from + hCapture->ulRegOffset); \
	BKNI_Memcpy((void*)pulCurrent, \
		(void*)&(hCapture->aulRegs[BVDC_P_CAP_GET_REG_IDX(from)]), \
		ulBlockSize * sizeof(uint32_t)); \
	pulCurrent += ulBlockSize; \
} while(0)

/* number of registers in one block. */
#define BVDC_P_CAP_REGS_COUNT      \
	BVDC_P_REGS_ENTRIES(CAP_0_REG_START, CAP_0_REG_END)

#define BVDC_P_Capture_MuxAddr(hCap)   (BCHP_VNET_B_CAP_0_SRC + (hCap)->eId * sizeof(uint32_t))
#define BVDC_P_Capture_SetVnet_isr(hCap, ulSrcMuxValue, eVnetPatchMode) \
   BVDC_P_SubRul_SetVnet_isr(&((hCap)->SubRul), ulSrcMuxValue, eVnetPatchMode)
#define BVDC_P_Capture_UnsetVnet_isr(hCap) \
   BVDC_P_SubRul_UnsetVnet_isr(&((hCap)->SubRul))

/***************************************************************************
 * Capture private data sturctures
 ***************************************************************************/
typedef enum BVDC_P_Capture_Trigger
{
	BVDC_P_Capture_Trigger_eDisabled = 0,
	BVDC_P_Capture_Trigger_eBvb,
	BVDC_P_Capture_Trigger_eLineCmp = 2
} BVDC_P_Capture_Trigger;

/* BVN path capture data mode */
typedef enum BVDC_P_Capture_DataMode
{
	BVDC_P_Capture_DataMode_e8Bit422 = 0,
	BVDC_P_Capture_DataMode_e10Bit422,
	BVDC_P_Capture_DataMode_e10Bit444,

	BVDC_P_Capture_DataMode_eMaxCount

} BVDC_P_Capture_DataMode;


typedef struct BVDC_P_CaptureContext
{
	BDBG_OBJECT(BVDC_CAP)

	/* flag initial state, requires reset; */
	bool                           bInitial;
	uint32_t                       ulResetRegAddr;
	uint32_t                       ulResetMask;

	/* private fields. */
	BVDC_P_CaptureId               eId;
	BRDC_Trigger                   eTrig;
	uint32_t                       ulRegOffset; /* CAP_0, CAP_1, and etc. */
	uint32_t                       aulRegs[BVDC_P_CAP_REGS_COUNT];

	/* A register handle.  Triggers need to be enable by host writes.
	 * A memory handle to do address/offset converting. */
	BREG_Handle                    hRegister;
	BRDC_Handle 				   hRdc;

	/* Keeps track of when ISR executed */
	BTMR_TimerHandle               hTimer;
	BTMR_TimerRegisters			   stTimerReg;
	uint32_t                       ulTimestamp;

#if (!BVDC_P_USE_RDC_TIMESTAMP)
	/* a capture block's scratch register */
	uint32_t                       ulTimestampRegAddr;
#endif

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext           SubRul;

	BVDC_444To422DnSampler         stDnSampler;

	/* Dither */
	BVDC_P_DitherSetting           stDither;

	/* Data mode */
	BVDC_P_Capture_DataMode        eCapDataMode;

#if (BVDC_P_DCX_CAP_RESET_WORKAROUND)
	BVDC_P_TestFeature1_Settings   stTestFeature1;
#endif
} BVDC_P_CaptureContext;


/***************************************************************************
 * Capture private functions
 ***************************************************************************/
BERR_Code BVDC_P_Capture_Create
	( BVDC_P_Capture_Handle           *phCapture,
	  BRDC_Handle					   hRdc,
	  BREG_Handle                      hRegister,
	  BVDC_P_CaptureId                 eCaptureId,
	  BTMR_TimerHandle                 hTimer,
	  BVDC_P_Resource_Handle           hResource );

void BVDC_P_Capture_Destroy
	( BVDC_P_Capture_Handle            hCapture );

void BVDC_P_Capture_Init
	( BVDC_P_Capture_Handle            hCapture );

void BVDC_P_Capture_BuildRul_isr
	( const BVDC_P_Capture_Handle      hCapture,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState,
	  BVDC_P_PicComRulInfo            *pPicComRulInfo );

BERR_Code BVDC_P_Capture_SetBuffer_isr
	( BVDC_P_Capture_Handle            hCapture,
	  uint32_t                         ulDeviceAddr,
	  uint32_t                         ulDeviceAddr_R,
	  uint32_t                         ulPitch );

BERR_Code BVDC_P_Capture_SetEnable_isr
	( BVDC_P_Capture_Handle            hCapture,
	  bool                             bEnable );

BERR_Code BVDC_P_Capture_SetInfo_isr
	( BVDC_P_Capture_Handle            hCapture,
	  BVDC_Window_Handle               hWindow,
	  const BVDC_P_PictureNodePtr      pPicture,
	  uint32_t                         ulPictureIdx,
	  bool                             bLastPic );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_CAPTURE_PRIV_H__ */
/* End of file. */
