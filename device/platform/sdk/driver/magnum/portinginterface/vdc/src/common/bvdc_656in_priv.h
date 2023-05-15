/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_656in_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 4/7/10 11:23a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_656in_priv.h $
 * 
 * Hydra_Software_Devel/5   4/7/10 11:23a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/4   4/5/10 3:57p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/3   5/10/07 5:31p yuxiaz
 * PR27644, PR29828: 7405 656 in support.
 *
 * Hydra_Software_Devel/2   12/18/06 11:40p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/29/06 1:55p yuxiaz
 * PR24262: Added 656in support for 3563.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/21/06 5:14p yuxiaz
 * PR24262: Use offset for various 656 sub-modules.
 *
 * Hydra_Software_Devel/1   9/14/06 10:00a yuxiaz
 * PR24262: Separate VDC 656 code from VDEC for new 656 core.
 *
 ***************************************************************************/
#ifndef BVDC_656IN_PRIV_H__
#define BVDC_656IN_PRIV_H__

#include "bvdc.h"
#include "bvdc_common_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Defines
 ***************************************************************************/
/* 7038x-ish, 3560x-ish, 7401Ax, 7118, 7440 */
#define BVDC_P_656IN_NEW_VER_0                   (0)
/* 7401Bx:
 * Use EXT_656_TOP_0_ext_656_count */
#define BVDC_P_656IN_NEW_VER_1                   (1)
/* 3563, 7405:
 * Use EXT_656_TOP_0_ext_656_count.
 * No  EXT_656_TOP_0_ext_656_reset */
#define BVDC_P_656IN_NEW_VER_2                   (2)
/* 7400-ish:
 * Use EXT_656_TOP_0_ext_656_short_count & EXT_656_TOP_0_ext_656_long_count */
#define BVDC_P_656IN_NEW_VER_3                   (3)

#if (BVDC_P_SUPPORT_NEW_656_IN_VER == BVDC_P_656IN_NEW_VER_3)
#define BVDC_P_NUM_656IN_SUPPORT                 (2)
#else
#define BVDC_P_NUM_656IN_SUPPORT                 (1)
#endif

/* Trigger offset from the picture height. */
#define BVDC_P_656IN_TRIGGER_OFFSET              (6)


/***************************************************************************
 * Private macros
 ***************************************************************************/

/***************************************************************************
 * Private enums
 ***************************************************************************/
typedef enum BVDC_P_656Id
{
	BVDC_P_656Id_e656In0 = 0,
	BVDC_P_656Id_e656In1
} BVDC_P_656Id;

/***************************************************************************
 * 656 Context
 ***************************************************************************/
typedef struct BVDC_P_656InContext
{
	BDBG_OBJECT(BVDC_656)

	BVDC_Source_Handle             hSource;
	BVDC_P_656Id                   eId;
	uint32_t                       ulOffset;

	bool                           bVideoDetected;

	/* 656 output slow start countdown to avoid prematurely output garbage */
	uint32_t                       ulDelayStart;

	/* 656 frame rate code */
	BAVC_FrameRateCode             eFrameRateCode;

} BVDC_P_656InContext;


/***************************************************************************
 * Private function prototypes
 ***************************************************************************/
BERR_Code BVDC_P_656In_Create
	( BVDC_P_656In_Handle             *ph656In,
	  BVDC_P_656Id                     e656Id,
	  BVDC_Source_Handle               hSource );

void BVDC_P_656In_Destroy
	( BVDC_P_656In_Handle              h656In );

void BVDC_P_656In_Init
	( BVDC_P_656In_Handle              h656In );

void BVDC_P_656In_UpdateStatus_isr
	( BVDC_P_656In_Handle              h656In );

void BVDC_P_656In_GetStatus_isr
	( const BVDC_P_656In_Handle        h656In,
	  bool                            *pbVideoDetected );

void BVDC_P_656In_Bringup_isr
	( BVDC_P_656In_Handle              h656In );

void BVDC_P_656In_BuildRul_isr
	( const BVDC_P_656In_Handle        h656In,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_656IN_PRIV_H__ */
/* End of file. */
