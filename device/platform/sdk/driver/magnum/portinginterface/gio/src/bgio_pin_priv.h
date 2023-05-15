/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgio_pin_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 4/24/12 3:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/gio/7435/bgio_pin_priv.h $
 * 
 * Hydra_Software_Devel/13   4/24/12 3:01p tdo
 * SW7584-10: Add PI/gio support for 7584
 *
 * Hydra_Software_Devel/12   6/17/11 3:00p tdo
 * SWDTV-7592: add BDBG_OBJECT_ASSERT for BGIO
 *
 * Hydra_Software_Devel/11   6/16/11 5:59p tdo
 * SWDTV-7592: add BDBG_OBJECT_ASSERT for BGIO.
 *
 * Hydra_Software_Devel/10   6/16/11 11:26a tdo
 * SWDTV-7292: Re-org GIO code
 *
 * Hydra_Software_Devel/9   11/17/10 9:02a tdo
 * SW7231-11: Add GIO PI support for 7231/7344/7346
 *
 * Hydra_Software_Devel/8   11/17/10 12:37a tdo
 * SW7231-11: Undo previous check-in, waiting for the rest of changeset
 * check-in clear (due to mastership)
 *
 * Hydra_Software_Devel/7   11/16/10 11:56p tdo
 * SW7231-11: Add GIO PI support for 7231/7344/7346
 *
 * Hydra_Software_Devel/6   4/8/08 5:47p tdo
 * PR41205: Add _isr functions to avoid deadlock
 *
 * Hydra_Software_Devel/5   2/27/08 11:54a tdo
 * PR34956: Re-organize GPIO pin  mux and add 3548 support.
 *
 * Hydra_Software_Devel/4   9/12/07 5:58p syang
 * PR 30391, PR 32351: BGIO only init for the pin created by BGIO to ovoid
 * override configures by other sw entity; BGIO read from HW reg (no more
 * sw buffering);
 *
 * Hydra_Software_Devel/3   2/24/04 7:20p syang
 * PR 9785: more api function implementations are added
 *
 * Hydra_Software_Devel/2   2/20/04 4:53p syang
 * PR 9785: check in before clearcase upgrade world wise
 *
 * Hydra_Software_Devel/1   2/20/04 11:23a syang
 * PR 9785: init version
 *
 ***************************************************************************/
#ifndef BGIO_PIN_PRIV_H__
#define BGIO_PIN_PRIV_H__

#include "blst_list.h"
#include "bgio.h"
#include "bgio_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BGIO_PIN);

#define BGIO_P_PIN_SET_BLACK_MAGIC(handle) \
	BGIO_GENERIC_SET_BLACK_MAGIC((handle), BGIO_P_Pin_Context)

#define BGIO_P_NULL_REG          0
#define BGIO_P_GIO_REG           1

/***************************************************************************
 * Pin Context
 */
typedef struct BGIO_P_Pin_Context
{
	BDBG_OBJECT(BGIO_PIN)

	uint32_t   ulBlackMagic;   /* Black magic for handle validation */

	/* created from this handle */
	BGIO_Handle  hGpio;

	/* pin info */
	BGIO_PinId     ePinId;
	BGIO_PinType   ePinType;
	BGIO_IntrMode  eIntrMode;

	/* struct {pNext, pPre} for linking */
	BLST_D_ENTRY(BGIO_P_Pin_Context) Link;

} BGIO_P_Pin_Context;

/***************************************************************************
 * Pin mux control for one pin
 */
typedef struct BGIO_P_PinMux
{
	BGIO_PinId ePinId;        /* pin number */
	uint32_t   ulReg;         /* register addr offset */
	uint32_t   ulBitMask;     /* bit mask for this pin in the reg */
	uint32_t   ulValue;       /* mux value set for this pin in the reg */

} BGIO_P_PinMux;

/***************************************************************************
 * Pin mux control setting in GIO register
 */
typedef struct BGIO_P_PinSet
{
	BGIO_PinId   eSetLo;         /* First pin of Set Lo */
	BGIO_PinId   eSetHi;         /* First pin of Set Hi */
	BGIO_PinId   eSetExt;        /* First pin of Set Ext */
	BGIO_PinId   eSetExtHi;      /* First pin of Set Ext Hi */
	BGIO_PinId   eSetExt2;       /* First pin of Set Ext2 */
	uint32_t     ulSetSgio;      /* The set number where sgio pins are located */
	uint32_t     ulSetSgioShift; /* Number of bit shift in sgio set */
} BGIO_P_PinSet;

/***************************************************************************
 *
 * API support functions
 *
 ***************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_Create(
	BGIO_Handle           hGpio,
	BGIO_PinId            ePinId,
	BGIO_Pin_Handle *     phPin );

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_Destroy(
	BGIO_Pin_Handle       hPin );

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_SetType(
	BGIO_Pin_Handle       hPin,
	BGIO_PinType          ePinType,
	bool                  bInIsr );

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_GetValue(
	BGIO_Pin_Handle       hPin,
	BGIO_PinValue *       pePinValue );

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_SetValue(
	BGIO_Pin_Handle       hPin,
	BGIO_PinValue         ePinValue,
	bool                  bInIsr );

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_SetIntrMode(
	BGIO_Pin_Handle       hPin,
	BGIO_IntrMode         eIntrMode,
	bool                  bInIsr );

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_ClearIntrStatus(
	BGIO_Pin_Handle       hPin,
	bool                  bInIsr );

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_GetIntrStatus(
	BGIO_Pin_Handle       hPin,
	bool *                pbFire );

/***************************************************************************
 *
 */
const BGIO_P_PinMux * BGIO_P_GetPinMux(
	BGIO_PinId            ePinId );

const BGIO_P_PinSet * BGIO_P_GetPinMapping( void );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BGIO_PIN_PRIV_H__ */

/* end of file */
