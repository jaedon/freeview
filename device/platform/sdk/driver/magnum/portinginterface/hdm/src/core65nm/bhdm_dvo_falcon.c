/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdm_dvo.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 1/7/08 12:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/7405/bhdm_dvo.c $
 * 
 * Hydra_Software_Devel/3   1/7/08 12:49p vle
 * PR 37424: Fix compiling error
 * 
 * Hydra_Software_Devel/2   6/4/07 7:56p vle
 * PR 30720: Added DVO support for 7405.
 * 
 * Hydra_Software_Devel/1   5/30/07 4:55p rgreen
 * PR31637,PR30720 :
 * 
 * Hydra_Software_Devel/17   4/29/07 11:11p rgreen
 * PR29747: Add code to update JEIDA mode
 * 
 * Hydra_Software_Devel/16   4/26/07 5:54p rgreen
 * PR29747: Add settings for DVO/LVDS configuration
 * 
 * Hydra_Software_Devel/15   3/26/07 2:46p rgreen
 * PR23340,PR28835:
 * Add Adjust Spread Spectrun for 3563
 * Add support to enable/disable DVO/LVDS port
 * 
 * Hydra_Software_Devel/14   2/22/07 3:35p rgreen
 * PR28001: Add 3563 support for 1366x768p panel
 * 
 * Hydra_Software_Devel/13   2/5/07 8:18p rgreen
 * PR27062: Add workaround for pixel color transitions
 * 
 * Hydra_Software_Devel/12   1/19/07 3:07p rgreen
 * PR27237: Remove functions with references to PWM
 * 
 * Hydra_Software_Devel/11   1/11/07 4:04p rgreen
 * PR27062: Fix bugs with 3563 DVO enable;  add same logic in 3560 for
 * enabling master/slave mode
 * 
 * Hydra_Software_Devel/10   12/12/06 7:45p rgreen
 * PR23267: Add initial LVDS support for 3563
 * 
 * Hydra_Software_Devel/9   12/4/06 9:19p rgreen
 * PR23267: Add EnableDVO Stub for 3563
 * 
 * Hydra_Software_Devel/8   12/4/06 7:27p rgreen
 * PR23267:Add 3563 Support;  Incorporate changes from main
 * Hydra_Software_Devel branch.  This separate bhdm.dvo.c element should
 * have been a link.  Change log added.
 * 
 * Hydra_Software_Devel/7   9/20/06 1:23p rgreen
 * PR23672: Add support for 7401 DVO compilation; remove incorrect invert
 * operation on field data
 * 
 * Hydra_Software_Devel/6   9/19/06 5:37p rgreen
 * PR23672: Add support for 7401 DVO compilation
 * 
 * Hydra_Software_Devel/PR23267/1   11/30/06 2:42p rgreen
 * PR23267:Add 3563 Support;  Incorporate changes from main
 * Hydra_Software_Devel branch.  This separate bhdm.dvo.c element should
 * have been a link.  Change log added.
 * 
 * Hydra_Software_Devel/5   8/22/06 1:50p rgreen
 * PR23267: Merge 3560 code
 * 
 * Hydra_Software_Devel/4   3/23/06 7:55p rgreen
 * PR20327:
 * Add 7438 Compilation Support
 * 
 * Add new compilation configuration macros to remove BCHP_XXX references
 * from source files
 * BHDM_CONFIG_DVO_7038BX_VERSION compilation macro
 * BHDM_CONFIG_88_2__176_4__192_SAMPLE_RATES
 * 
 * Hydra_Software_Devel/3   3/2/06 6:03p rgreen
 * PR16915: Separate Bx/Cx BHDM_EnableDVO port code
 * 
 * Hydra_Software_Devel/2   1/9/06 4:51p rgreen
 * PR18790: Update HDM with C3 defines using new version method.
 * 
 * Hydra_Software_Devel/1   8/30/05 2:09p rgreen
 * PR16915: add dvo port compile time option
 * 
 ***************************************************************************/
 
#include "bhdm.h"
#include "bhdm_priv.h"



#define	BHDM_CHECK_RC( rc, func )	              \
do                                                \
{										          \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										      \
		goto done;							      \
	}										      \
} while(0)


BDBG_MODULE(BHDM) ;


BERR_Code BHDM_DVO_P_EnableDvoPort(
    BHDM_Handle hHDMI,		/* [in] HDMI handle */
   BHDM_OutputFormat eOutputFormat /* [in] format to use on Output Port */
) 
{
	BERR_Code      rc = BERR_SUCCESS;
	uint32_t Register ;
	uint8_t iByPassHdmiPort ;
	uint8_t ui24BitMode  ;
	
	
	BDBG_ENTER(BHDM_DVO_P_EnableDvoPort) ;
	BDBG_ASSERT( hHDMI );

	/*****  Enable DVO Port *****/
	iByPassHdmiPort = 1 ;  /* DVO Port */
	ui24BitMode = 0 ;
	BDBG_WRN(("Enabling DVO Port; HDMI output DISABLED")) ;

	if (eOutputFormat != BHDM_OutputFormat_e12BitDVOMode)
	{
		BDBG_WRN(("12 Bit DVO Only Supported; Enabling for 12 Bit DVO"));
		rc = BERR_INVALID_PARAMETER;
		goto done;
	}


	Register = BREG_Read32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10) ;
	Register &= 
		~ (BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_086)  /* dvo0_de */
		|  BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_087));  /* dvo0_clk_n */
	Register |= 
		(BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_086, 3)  /* dvo0_de */
		|  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_087, 3));  /* dvo0_clk_n */
	BREG_Write32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, Register) ;


	Register = BREG_Read32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11) ;
	Register &= 
		~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_088)	/* dvo0_00 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_089)  /* dvo0_01 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_090)  /* dvo0_02 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_091)  /* dvo0_03 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094)  /* dvo0_04 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_095)  /* dvo0_05 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_096)  /* dvo0_06 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_097)  /* dvo0_07 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_093)  /* dvo0_hsync */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_092)) ; /* dvo0_vsync */
	Register |= 
		(BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_088, 3)  /* dvo0_00 */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_089, 3)  /* dvo0_01 */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_090, 3)  /* dvo0_02 */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_091, 3)  /* dvo0_03 */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_094, 3)  /* dvo0_04 */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_095, 3)  /* dvo0_05 */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_096, 3)  /* dvo0_06 */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_097, 3)  /* dvo0_07 */		
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_093, 3)  /* dvo0_hsync */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_092, 3)) ; /* dvo0_vsync */		
	BREG_Write32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, Register) ;


	Register = BREG_Read32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12) ;
	Register &= 
		~ (BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_098)  /* dvo0_08 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_099)   /* dvo0_09 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_100)   /* dvo0_10 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101)   /* dvo0_11 */
		| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102)) ; /* dvo0_clk_p */		
	Register |= 		
		(BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_098, 3)  /* dvo0_08*/
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_099, 3)	/* dvo0_09 */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_100, 3)	 /* dvo0_10*/
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101, 3)	 /* dvo0_11 */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102, 3)) ; /* dvo0_clk_p */
	BREG_Write32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, Register) ;

	/* DVO Timings Adjusments per simulation */
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_DVO_TIMING_ADJUST_D, 0xF7AA4435) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_DVO_TIMING_ADJUST_C,  0x8888C5AA) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_DVO_TIMING_ADJUST_B,  0x88888888) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_DVO_TIMING_ADJUST_A,  0x00063633) ;

	
	/* set the dvo/dvi mode */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_MISC_CONTROL) ;
	Register &= ~(
		  BCHP_MASK(HDMI_MISC_CONTROL, IS_24_BIT_MODE) 
		|BCHP_MASK(HDMI_MISC_CONTROL, DVO_ESEL)) ;
	Register |= 
		  BCHP_FIELD_DATA(HDMI_MISC_CONTROL, IS_24_BIT_MODE, ui24BitMode) 
		| BCHP_FIELD_DATA(HDMI_MISC_CONTROL, DVO_ESEL, 0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_MISC_CONTROL, Register) ;


	/* by pass the HDMI port; enable the DVO port */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_ENCODER_CTL) ;
	Register &= ~BCHP_MASK(HDMI_ENCODER_CTL, ENCODER_BYPASS) ;
	Register |= BCHP_FIELD_DATA(HDMI_ENCODER_CTL, ENCODER_BYPASS, iByPassHdmiPort) ; 
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_ENCODER_CTL, Register) ; 

done:
	BDBG_LEAVE(BHDM_DVO_P_EnableDvoPort);
	return rc ;
	
}

