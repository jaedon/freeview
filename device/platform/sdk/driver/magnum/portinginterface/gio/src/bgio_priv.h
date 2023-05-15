/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgio_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 6/17/11 3:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/gio/7038/bgio_priv.h $
 * 
 * Hydra_Software_Devel/10   6/17/11 3:01p tdo
 * SWDTV-7592: add BDBG_OBJECT_ASSERT for BGIO
 *
 * Hydra_Software_Devel/9   6/16/11 5:59p tdo
 * SWDTV-7592: add BDBG_OBJECT_ASSERT for BGIO.
 *
 * Hydra_Software_Devel/8   4/11/11 9:27a tdo
 * SWBLURAY-23686: Add missing include file.
 *
 * Hydra_Software_Devel/7   4/8/11 9:40a tdo
 * SWBLURAY-23686: Add GIO PortingInterface support for Blast (7640) chip
 *
 * Hydra_Software_Devel/6   4/8/08 5:48p tdo
 * PR41205: Add _isr functions to avoid deadlock
 *
 * Hydra_Software_Devel/5   9/13/07 4:11p syang
 * PR 30391, PR 32351: clean up OpenDrain pin data set records when it is
 * destroied or changed to diff type
 *
 * Hydra_Software_Devel/4   9/12/07 5:57p syang
 * PR 30391, PR 32351: BGIO only init for the pin created by BGIO to ovoid
 * override configures by other sw entity; BGIO read from HW reg (no more
 * sw buffering);
 *
 * Hydra_Software_Devel/3   7/21/06 11:42a syang
 * PR 22789: added support for gio control set *_EXT_HI and more pins with
 * control set *_EXT, added suuport for 7118, 3563 and 7440
 *
 * Hydra_Software_Devel/2   2/24/04 7:21p syang
 * PR 9785: more api function implementations are added
 *
 * Hydra_Software_Devel/1   2/20/04 11:22a syang
 * PR 9785: init version
 *
 ***************************************************************************/
#ifndef BGIO_PRIV_H__
#define BGIO_PRIV_H__

#include "bgio.h"
#include "bgio_macro.h"
#include "bgio_pin_priv.h"
#include "blst_list.h"
#include "bchp_common.h"
#ifdef BCHP_GIO_AON_REG_START
#include "bchp_gio_aon.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BGIO);

#define BGIO_P_MAIN_SET_BLACK_MAGIC(handle) \
	BGIO_GENERIC_SET_BLACK_MAGIC((handle), BGIO_P_Context)

/* define PinContext head struct */
typedef struct BGIO_P_Pin_Context_Head BGIO_P_Pin_Context_Head;
BLST_D_HEAD(BGIO_P_Pin_Context_Head, BGIO_P_Pin_Context);

#define BGIO_P_BIT_MASK(bit)           (1 << (bit))
#define BGIO_P_REG_BASE                BCHP_GIO_ODEN_LO
#define BGIO_P_REG_LOW_TOP             BCHP_GIO_STAT_LO
#define BGIO_P_NUM_LOW_REGS            8
#ifdef BCHP_GIO_AON_ODEN_LO
#define BGIO_P_AON_BASE                BCHP_GIO_AON_ODEN_LO
#else
#define BGIO_P_AON_BASE                0
#endif
#if (BCHP_CHIP==7400)
#define BGIO_P_NUM_CTRL_SETS           4
#else
#define BGIO_P_NUM_CTRL_SETS           3
#endif

/***************************************************************************
 * BGIO main Context
 */
typedef struct BGIO_P_Context
{
	BDBG_OBJECT(BGIO)

	uint32_t   ulBlackMagic;   /* Black magic for handle validation */

	/* handed down from up level sw */
	BCHP_Handle    hChip;
	BREG_Handle    hRegister;

	/* records of values set to the data bits of open drain pins */
	uint32_t  aulOpenDrainSet[BGIO_P_NUM_LOW_REGS * BGIO_P_NUM_CTRL_SETS];

	/* link list for managed pin records */
	BGIO_P_Pin_Context_Head  PinHead;

} BGIO_P_Context;

/***************************************************************************
 *
 * API functions
 *
 ***************************************************************************/

/*--------------------------------------------------------------------------
 * To be called to init HW registers as BGIO openning
 */
BERR_Code BGIO_P_InitReg(
	BGIO_P_Context *      pGpio,
	BREG_Handle           hRegister );

/***************************************************************************
 *
 * Utility functions
 *
 ***************************************************************************/

/*--------------------------------------------------------------------------
 * To be called by BGIO_P_WritePinRegBit and BGIO_P_ReadPinRegBit to calculate the
 * register offset relative to BGIO_P_REG_BASE and the bit offset
 * relative to bit 0, based on pin ID.
 */
BERR_Code BGIO_P_CalcPinRegAndBit(
	BGIO_PinId            ePinId,
	uint32_t              ulRegLow,        /* corresponding reg_low */
	uint32_t *            pulRegOffset,
	uint32_t *            pulBitOffset );

/*--------------------------------------------------------------------------
 * To be called to write the GPIO pin's bit into one register
 */
BERR_Code BGIO_P_WritePinRegBit(
	BGIO_Handle           hGpio,
	BGIO_PinId            ePinId,
	BGIO_PinType          ePinType,
	uint32_t              ulRegLow,
	BGIO_PinValue         ePinValue,
	bool                  bInIsr );

/*--------------------------------------------------------------------------
 * To be called to write the GPIO pin's bit into one register
 */
BERR_Code BGIO_P_ReadPinRegBit(
	BGIO_Handle           hGpio,
	BGIO_PinId            ePinId,
	uint32_t              ulRegLow,
	BGIO_PinValue *       pePinValue );

/***************************************************************************
 * To be called to add a pin handle into the pin list in BGIO's main
 * context
 */
BERR_Code BGIO_P_AddPinToList(
	BGIO_Handle           hGpio,
	BGIO_Pin_Handle       hPin );

/***************************************************************************
 * To be called to remove a pin handle from the pin list in BGIO's main
 * context
 */
BERR_Code BGIO_P_RemovePinFromList(
	BGIO_Handle           hGpio,
	BGIO_Pin_Handle       hPin );

/*--------------------------------------------------------------------------
 * To be called to get the pin handle for a PinId from the pin list in
 * BGIO's main context. NULL returned if it does not exist.
 */
BGIO_Pin_Handle BGIO_P_GetPinHandle(
	BGIO_Handle           hGpio,
	BGIO_PinId            ePinId );

/***************************************************************************
 * To be called to get the register handle
 */
BREG_Handle BGIO_P_GetRegisterHandle(
	BGIO_Handle  hGpio );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BGIO_PRIV_H__ */

/* end of file */
