/***************************************************************************
 *     Copyright (c) 2003-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_3250.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 5/26/06 3:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/3250/bint_3250.c $
 * 
 * Hydra_Software_Devel/21   5/26/06 3:20p albertl
 * PR21392:  Removed code accessing timers directly.
 * 
 * Hydra_Software_Devel/20   3/20/06 6:25p dlwin
 * PR10596: Fixed compiler warnings.
 * 
 * Hydra_Software_Devel/19   4/6/05 2:50p albertl
 * PR10596: Updated chip specific settings with NULL function pointer
 * placeholder to reflect change in BINT_Settings interface.
 * 
 * Hydra_Software_Devel/18   4/5/05 1:50p dlwin
 * PR 14696: Fixed warning for unused variables
 * 
 * Hydra_Software_Devel/17   1/22/04 5:32p vsilyaev
 * PR 9449: Changed interript shift to match position of level 1 IRQ bit
 * on 97038 board.
 * 
 * Hydra_Software_Devel/16   12/30/03 2:55p dlwin
 * PR 9117: Completed testing of new interrupt manger.
 * 
 * Hydra_Software_Devel/15   12/29/03 5:06p dlwin
 * PR 9117: Fixed compiler errors.
 * 
 * Hydra_Software_Devel/14   12/29/03 4:16p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 * 
 * Hydra_Software_Devel/13   12/23/03 4:45p vsilyaev 
 * PR 8985: 3250 IRQ has a offset of 1 from in a top level register. 
 * Use private MASK and STATUS. 
 *  
 * Hydra_Software_Devel/12   12/18/03 2:37p marcusk
 * PR8985: Removed bint_priv.h since it is no longer needed.
 * 
 * Hydra_Software_Devel/11   12/18/03 2:08p marcusk
 * PR8985: Refactored to use single ISR() routine. Removed reserved names.
 * Placed all platform specific defines in bint_plat.h
 * 
 * Hydra_Software_Devel/10   12/16/03 5:56p dlwin
 * PR 9019: Fixed a problem with handling interrupt that were not enabled,
 * but generated an interrupt. It should be ignored, not  generate
 * asserts.
 * 
 * Hydra_Software_Devel/9   11/7/03 4:54p dlwin
 * Added addition debug message.
 * 
 * Hydra_Software_Devel/8   10/18/03 1:42p dlwin
 * Checkout interrupt handling for Bcm3250.  Corrected for  problems with
 * the original code designed.
 * 
 * Hydra_Software_Devel/7   10/7/03 1:32p dlwin
 * Correct for function prototype
 * 
 * Hydra_Software_Devel/6   10/2/03 6:58p dlwin
 * Changed of function from Bcm3250 to 3250.
 * 
 * Hydra_Software_Devel/5   9/30/03 7:40p dlwin
 * Removed warnings when compiled for debug.
 * 
 * Hydra_Software_Devel/4   9/30/03 5:37p dlwin
 * Updated to support newer BINT interface.
 * 
 * Hydra_Software_Devel/3   9/26/03 8:52a dlwin
 * Updated to 'bchp_qam.h' instead of 'bchp_3250.h'
 * 
 * Hydra_Software_Devel/2   9/9/03 1:17p dlwin
 * Corrected problem with incorrectly indexing into the table.
 * 
 * Hydra_Software_Devel/1   8/29/03 12:04p dlwin
 * Initial version
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bchp_qam.h"
#include "bint_3250.h"

BDBG_MODULE(interruptinterface_3250);

#define BINT_P_STATUS	GL_BCM3250_IRQ_PENDING
#define BINT_P_MASK		GL_BCM3250_IRQ_MASK

static void BINT_P_3250_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_3250_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_3250_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_3250_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );

static const BINT_P_IntMap bint_3250A0[] =
{
	/* Since all Bcm3250 interrupt are mapped to one L1 interrupt, therefore
	   table only contains one entry. The function that uses this table
	   is hardcoded to support only one entry, the first entry. */
	{ 33, BCM3250_IRQ_ID, 0, "3250L1" }, 
	{ -1, 0, 0, NULL }
};

static const BINT_Settings bcm3250IntSettings =
{
	NULL,				/* Bcm3250 doesn't support S/W produced interrupts */
	NULL,				/* Bcm3250 doesn't support S/W produced interrupts */
	BINT_P_3250_SetMask,
	BINT_P_3250_ClearMask,
	BINT_P_3250_ReadMask,
	BINT_P_3250_ReadStatus,
	bint_3250A0,
	"3250"
};

/***************************************************************************
*	Not supported on Bcm3250
* static void BINT_P_3250SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
* {
* 	BINT_SET_INT(regHandle, baseAddr, shift);
* 	return;
* }
* 
* static void BINT_P_3250ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
* {
* 	BINT_CLEAR_INT(regHandle, baseAddr, shift);
* 	return;
* }
***************************************************************************/

static void BINT_P_3250_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	uint16_t usVal;

	BSTD_UNUSED(baseAddr);

	usVal = BREG_Read16( regHandle, BINT_P_MASK );
	usVal &= ~(1 << (shift));
	BREG_Write16( regHandle, BINT_P_MASK, usVal);
}

static void BINT_P_3250_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	uint16_t usVal;

	BSTD_UNUSED(baseAddr);

	usVal = BREG_Read16( regHandle, BINT_P_MASK );
	usVal |= (1 << (shift));
	BREG_Write16( regHandle, BINT_P_MASK, usVal);
}

const BINT_Settings *BINT_3250_GetSettings( void )
{
	return &bcm3250IntSettings;
}

static uint32_t BINT_P_3250_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
{
	BSTD_UNUSED(baseAddr);

	return ~((uint32_t)BREG_Read16( regHandle, BINT_P_MASK )) & 0x0000FFFF;
}

static uint32_t BINT_P_3250_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
{
	BSTD_UNUSED(baseAddr);

	return ((uint32_t)BREG_Read16( regHandle, BINT_P_STATUS )) & 0x0000FFFF;
}

