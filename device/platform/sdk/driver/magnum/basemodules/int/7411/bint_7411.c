/***************************************************************************
 *	   Copyright (c) 2003-2006, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7411.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 5/26/06 3:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7411/bint_7411.c $
 * 
 * Hydra_Software_Devel/9   5/26/06 3:19p albertl
 * PR21392:  Removed code accessing timers directly.
 * 
 * Hydra_Software_Devel/8   2/15/06 5:30p vsilyaev
 * PR 19693: Added support for acquiring interrupt rate
 * 
 * Hydra_Software_Devel/7   4/6/05 2:05p albertl
 * PR10596: Updated chip specific settings with NULL function pointer
 * placeholder to reflect change in BINT_Settings interface.
 * 
 * Hydra_Software_Devel/6   1/24/05 4:53p vsilyaev
 * PR 12333: Removed dependency on 97395 board.
 * 
 * Hydra_Software_Devel/5   1/14/05 4:14p marcusk
 * PR13827: Updated code to use BARC (tested with XVD_VDC_TEST)
 * 
 * Hydra_Software_Devel/4   11/24/04 6:17p darnstein
 * PR 12333:  Interrupts from 7411 seem to be working now.  I caused the
 * main ISR BXVD_P_PPBReady_isr() to return without doing any work.  This
 * ISR seems to have a problem.
 * 
 * Hydra_Software_Devel/3   9/14/04 3:34p darnstein
 * PR 10943: Move BCM7411_IRQ_ID to a header file.
 * 
 * Hydra_Software_Devel/2   8/26/04 5:12p maivu
 * PR 12333: Fixed build error
 * 
 * Hydra_Software_Devel/1   8/19/04 6:37p marcusk
 * PR 12333: Initial version.
 * 
 ***************************************************************************/
#include "bstd.h"
#include "barc.h"
#include "bchp_7411.h"
#include "bint_7411.h"

BDBG_MODULE(int_7411);

/* TODO: Move definition to 7411 specific chip interface */
#define BINT_P_STATUS	0x00100f24

static void BINT_P_7411_ClearInt( 
	BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7411_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7411_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7411_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_7411_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );

static const BINT_P_IntMap bint_7411B0[] =
{
	/* Since all Bcm7411 interrupt are mapped to one L1 interrupt, therefore
	   table only contains one entry. The function that uses this table
	   is hardcoded to support only one entry, the first entry. */
	{ BINT_7411_IRQ_0, BCM7411_IRQ_BASE_ADDRESS, 0, "IRQ_0" }, 
	{ -1, 0, 0, NULL }
};

static const BINT_Settings bcm7411IntSettings =
{
	NULL,				/* Not supported */
	BINT_P_7411_ClearInt,
	BINT_P_7411_SetMask,
	BINT_P_7411_ClearMask,
	BINT_P_7411_ReadMask,
	BINT_P_7411_ReadStatus,
	bint_7411B0,
	"7411"
};

/***************************************************************************
*	Not supported on Bcm7411
* static void BINT_P_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
* {
*	BINT_SET_INT(regHandle, baseAddr, shift);
*	return;
* }
***************************************************************************/
 
static void BINT_P_7411_ClearInt( 
	BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	uint32_t ulVal;

	BSTD_UNUSED(baseAddr);

	ulVal = 1 << shift;
	BARC_Reg_Write32_isr( (BARC_Handle)regHandle, BINT_P_STATUS, ulVal);
	return;
}

static void BINT_P_7411_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	/* 7411 does not support host programmable interrupt masks */
	BSTD_UNUSED(regHandle);
	BSTD_UNUSED(baseAddr);
	BSTD_UNUSED(shift);
	return;
}

static void BINT_P_7411_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	/* 7411 does not support host programmable interrupt masks */
	BSTD_UNUSED(regHandle);
	BSTD_UNUSED(baseAddr);
	BSTD_UNUSED(shift);
	return;
}

const BINT_Settings *BINT_7411_GetSettings( void )
{
	return &bcm7411IntSettings;
}

static uint32_t BINT_P_7411_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
{
	/* 7411 does not support host programmable interrupt masks */
	BSTD_UNUSED(regHandle);
	BSTD_UNUSED(baseAddr);
	return 0xFFFFFFFF;
}

static uint32_t BINT_P_7411_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
{
	uint32_t status;
	BSTD_UNUSED(baseAddr);
	status = BARC_Reg_Read32_isr( (BARC_Handle)regHandle, BINT_P_STATUS );
	return status;
}


