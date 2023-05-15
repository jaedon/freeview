/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: framework_board.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 9/4/12 4:32p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/board/97435/framework_board.c $
* 
* Hydra_Software_Devel/4   9/4/12 4:32p vanessah
* SW7425-2509: init vcxo for 7435
* 
* Hydra_Software_Devel/3   3/22/12 10:26a vanessah
* SW7435-9:  Sync with nexus_platform_pinmux.c for VMS board
* 
* Hydra_Software_Devel/2   10/31/11 10:37a mward
* SW7435-9:  Correct a bit field name in UART_ROUTER_SEL.
* 
* Hydra_Software_Devel/1   10/26/11 1:56p mward
* SW7435-9: Add 97435 appframework board.
* 
* Hydra_Software_Devel/10   5/23/11 11:51a jessem
* SW7231-128: Added BCHP_Handle parameter to BTST_VideoShmoo() to support
* power management.
*
***************************************************************************/

/* magnum includes */
#include "bstd.h"
#include "bkni.h"
#include "bchp.h"
#include "bmem.h"

/* framework includes */
#include "framework_board.h"

/* chip specifics */
#include "platform.h"

#ifdef LINUX
/* cache support */
#include <asm/cachectl.h>
#endif

#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_pin_ctrl.h"

/* i2c enabling */
#if (FRAMEWORK_DO_I2C == 1)
#include "bi2c.h"
#include "bchp_gio.h"
#endif

/* fpga program */
#if (FRAMEWORK_DO_FPGA == 1)
#include "bfpga_name.h"
#endif

#if (FRAMEWORK_DO_SHMOO)
#include "btst_video_shmoo.h"
#endif
#include "bchp_vcxo_0_rm.h"
#include "bchp_vcxo_1_rm.h"
#include "bchp_clkgen.h"

BDBG_MODULE( FRAMEWORK );

#define TestError(e, s) {\
	eStatus = e;\
	if (eStatus != BERR_SUCCESS)\
{\
	BDBG_ERR(( s, __FILE__, __LINE__ ));\
	goto Error;\
}\
}


/* the following is copied from BSEAV/api/src/magnum/board/bsettop_bsp_$(BCH_CHIP).h */
#define B_I2C_CHANNEL_HDMI  0

#if (FRAMEWORK_DO_I2C == 1)

/***************************************************************************/
static int BI2C_Init(
					 BFramework_Info    *pFrameworkInfo )
{
	BFrmWorkBoard_Info  *pFrmWorkBoardInfo;
	BI2C_ChannelSettings  i2cChannelDefaultSettings;
	unsigned int  ui;
	BERR_Code eStatus = BERR_SUCCESS;

	pFrmWorkBoardInfo = pFrameworkInfo->hFrmWorkBoard;

	/* Note: in order to enable I2C, the following sun top mux control code
	* is copied from BSEAV/api/src/magnum/board/bsettop_bsp_$CHP_CHIP.c, with
	* "b_board.b$CHP_CHIP.reg" replaced by "pFrameworkInfo->hReg". At the
	* moment we try to make as little modification as possible to this piece
	* of code, so that we could follow up the changes and new addings in
	* bsettop_bsp_$CHP_CHIP.c easily.
	*/

	/* I2C handle */
	TestError( BI2C_Open( &pFrmWorkBoardInfo->hI2c,
		pFrameworkInfo->hChp, pFrameworkInfo->hReg, pFrameworkInfo->hInt, NULL ),
		"ERROR: BI2C_Open: %s:%d" );

	/* determine the number of i2c channels available */
	TestError( BI2C_GetTotalChannels( pFrmWorkBoardInfo->hI2c, &pFrmWorkBoardInfo->uiNumI2c),
		"ERROR: BI2C_GetTotalChannels: %s:%d" );

	/* create register handles for each I2C channel */
	for (ui=0; ui<pFrmWorkBoardInfo->uiNumI2c; ui++)
	{
		/* get default settings for channel */
		TestError( BI2C_GetChannelDefaultSettings( pFrmWorkBoardInfo->hI2c, ui,
			&i2cChannelDefaultSettings),
			"ERROR: BI2C_GetChannelDefaultSettings: %s:%n" );

		if (ui==2) {
			i2cChannelDefaultSettings.clkRate = BI2C_Clk_eClk375Khz; /* decrease speed for I2C connected to the external slots */
		}

		/* create specific i2c handle */
		TestError( BI2C_OpenChannel( pFrmWorkBoardInfo->hI2c,
			pFrmWorkBoardInfo->ahI2cChannels + ui, ui, &i2cChannelDefaultSettings),
			"ERROR: BI2C_OpenChannel: %s:%n" );

		/* create register handle */
		TestError( BI2C_CreateI2cRegHandle(pFrmWorkBoardInfo->ahI2cChannels[ui],
			pFrmWorkBoardInfo->ahI2cRegHandles + ui),
			"ERROR: BI2C_CreateI2cRegHandle: %s:%n" );
	}

Error:
	return eStatus;
}
#endif  /*#if (FRAMEWORK_DO_I2C == 1) */

/***************************************************************************/
static void BPinMux_Init( BFramework_Info  *pFrameworkInfo )
{
	/* Note: in order to enable PinMux, the following sun top mux control code
	* is copied from nexus\platforms\$CHP_CHIP\src\Nexus_platform_pinmux.c, with
	* function NEXUS_Platform_P_InitPinmux.
	*/

	BREG_Handle hReg = pFrameworkInfo->hReg;
	uint32_t reg;

	/* AON_GPIO 00..17 set by OS        */
	/* -------------------------------*/


	/* GPIO 000..001 set by OS        */
	/* -------------------------------*/
	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5
	* GPIO_000    : ENET_ACTIVITY(1)
	* GPIO_001    : ENET_LINK(1)
	*/


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5
	* GPIO_002    : UART_RXD_0(1)
	* GPIO_003    : UART_TXD_0(1)
	*/


	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_002)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_003)
		);


	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_002, 1) | /* EMAC0_RX_EN_CTL */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_003, 1);  /* EMAC0_RX_CLK */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, reg);

	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6
	* GPIO_004    : UART_CTS_0(1)
	* GPIO_005    : UART_RTS_0(1)
	* GPIO_006    : UART_RXD_1(1)
	* GPIO_007    : UART_TXD_1(1)
	* GPIO_008    : UART_CTS_1(1)
	* GPIO_009    : UART_RTS_1(1)
	* GPIO_010    : UART_RXD_2(1)
	* GPIO_011    : UART_TXD_2(1)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_004)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_005)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_006)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_007)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_008)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_009)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_010)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_011)
		);

	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_004, 1) | /* EMAC0_RXD_00 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_005, 1) | /* EMAC0_RXD_01 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_006, 1) | /* EMAC0_RXD_02 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_007, 1) | /* EMAC0_RXD_03 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_008, 1) | /* EMAC0_TXD_CLK */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_009, 1) | /* EMAC0_TXD_EN_CTL */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_010, 1) | /* EMAC0_TXD_00 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_011, 1);  /* EMAC0_TXD_01 */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7
	* GPIO_012    : UART_CTS_2(1)
	* GPIO_013    : UART_RTS_2(1)
	* GPIO_014    : PKT_ERROR5(2)
	* GPIO_015    : PKT_VALID5(2)
	* GPIO_016    : PKT_CLK0  (1)
	* GPIO_017    : PKT_DATA0 (1)
	* GPIO_018    : PKT_SYNC0 (1)
	* GPIO_019    : PKT_ERROR0(0)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_012)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_013)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_014)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_015)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_016)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_017)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_018)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_019)
		);

	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_012, 1) | /* EMAC0_TXD_02 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_013, 1) | /* EMAC0_TXD_03 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_014, 2) | /* EMAC0_MDIO */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_015, 2) | /* EMAC0_MDC */
#if (FRAMEWORK_VMS_SFF==1)
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_016, 2) | /* MTSIF0_CLK */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_017, 2) | /* MTSIF0_DATA0 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_018, 2) | /* MTSIF0_SYNC */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_019, 2);  /* MTSIF0_DATA1 */
#else
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_016, 1) | /* PKT_CLK0 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_017, 1) | /* PKT_DATA0 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_018, 1) | /* PKT_SYNC0 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_019, 0);  /* GPIO interrupt for 3112_tuner0*/
#endif

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8
	* GPIO_020    : PKT_VALID0(0)
	* GPIO_021    : PKT_CLK1  (1)
	* GPIO_022    : PKT_DATA1 (1)
	* GPIO_023    : PKT_SYNC1 (1)
	* GPIO_024    : PKT_ERROR1(0)
	* GPIO_025    : PKT_VALID1(1)
	* GPIO_026    : PKT_ERROR2(0)
	* GPIO_027    : PKT_VALID2(0)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_020)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_021)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_022)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_023)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_024)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_025)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_026)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_027)
		);

	reg |=
#if (FRAMEWORK_VMS_SFF==1)
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_020, 2) | /* MTSIF0_VALID */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_021, 2) | /* MTSIF0_DATA2 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_022, 2) | /* MTSIF0_DATA3 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_023, 2) | /* MTSIF0_DATA4 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_024, 2) | /* MTSIF0_DATA5 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_025, 2) | /* MTSIF_ATS_RST */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_026, 2) | /* MTSIF0_DATA6 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_027, 2);  /* MTSIF0_DATA7 */
#else
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_020, 0) | /* GPIO interrupt for 3112_tuner1*/
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_021, 1) | /* PKT_CLK1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_022, 1) | /* PKT_DATA1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_023, 1) | /* PKT_SYNC1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_024, 0) | /* MTSIF0_DATA5 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_025, 1) | /* MTSIF_ATS_RST */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_026, 0) | /* MTSIF0_DATA6 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_027, 0);  /* MTSIF0_DATA7 */
		
#endif

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9
	* GPIO_028    : PKT_DATA3     (1)
	* GPIO_029    : PKT_SYNC3     (1)
	* GPIO_030    : POD2CHIP_MCLKI(0)
	* GPIO_031    : CHIP2POD_MCLKO(0)
	* GPIO_032    : CHIP2POD_SCLK (1)
	* GPIO_033    : POD2CHIP_SDI  (1)
	* GPIO_034    : CHIP2POD_SDO  (1)
	* GPIO_035    : CHIP2POD_SCTL (1)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_028)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_029)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_030)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_031)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_032)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_033)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_034)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_035)
		);
#if (FRAMEWORK_VMS_SFF==1)
	reg |=  
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_028, 0) | /* BCM3128 GPIO interrupt*/
#else
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_028, 1) | /* PKT_DATA3 */
#endif
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_029, 1) | /* PKT_SYNC3 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_030, 0) | /* POD2CHIP_MCLKI */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_031, 0) | /* GPIO_031 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_032, 1) | /* CHIP2POD_SCLK */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_033, 1) | /* POD2CHIP_SDI */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_034, 1) | /* CHIP2POD_SDO */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_035, 1);  /* CHIP2POD_SCTL */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10
	* GPIO_036    : POD2CHIP_MISTRT(1)
	* GPIO_037    : POD2CHIP_MIVAL (0)
	* GPIO_038    : POD2CHIP_MDI0  (1)
	* GPIO_039    : POD2CHIP_MDI1  (1)
	* GPIO_040    : POD2CHIP_MDI2  (1)
	* GPIO_041    : POD2CHIP_MDI3  (5)
	* GPIO_042    : POD2CHIP_MDI4  (5)
	* GPIO_043    : POD2CHIP_MDI5  (1)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_036)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_037)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_038)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_039)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_040)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_041)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_042)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_043)
		);

	reg |=  
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_036, 1) | /* POD2CHIP_MISTRT */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_037, 0) | /* GPIO_37 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_038, 1) | /* POD2CHIP_MDI0 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_039, 1) | /* POD2CHIP_MDI1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_040, 1) | /* POD2CHIP_MDI2 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_041, 5) | /* TP_IN_21  */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_042, 5) | /* TP_OUT_22 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_043, 1);  /* POD2CHIP_MDI5 */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11
	* GPIO_044    : POD2CHIP_MDI6  (1)
	* GPIO_045    : POD2CHIP_MDI7  (1)
	* GPIO_046    : POD2CHIP_MICLK (1)
	* GPIO_047    : CHIP2POD_MOVAL (0)
	* GPIO_048    : CHIP2POD_MOSTRT(1)
	* GPIO_049    : CHIP2POD_MDO0  (1)
	* GPIO_050    : CHIP2POD_MDO1  (1)
	* GPIO_051    : CHIP2POD_MDO2  (1)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_044)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_045)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_046)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_047)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_048)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_049)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_050)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_051)
		);

	reg |=  
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_044, 1) | /* POD2CHIP_MDI6 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_045, 1) | /* POD2CHIP_MDI7 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_046, 1) | /* POD2CHIP_MICLK */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_047, 0) | /* GPIO_47 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_048, 1) | /* CHIP2POD_MOSTRT */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_049, 1) | /* CHIP2POD_MDO0 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_050, 1) | /* CHIP2POD_MDO1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_051, 1);  /* CHIP2POD_MDO2 */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12
	* GPIO_052    : CHIP2POD_MDO3(1)
	* GPIO_053    : CHIP2POD_MDO4(1)
	* GPIO_054    : CHIP2POD_MDO5(1)
	* GPIO_055    : CHIP2POD_MDO6(1)
	* GPIO_056    : CHIP2POD_MDO7(1)
	* GPIO_057    : CHIP2POD_MOCLK(1)
	* GPIO_058    : PKT_CLK2(1)
	* GPIO_059    : PKT_DATA2(1)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_052)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_053)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_054)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_055)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_056)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_057)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_058)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_059)
		);

	reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_052, 1) | /* CHIP2POD_MDO3 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_053, 1) | /* CHIP2POD_MDO4 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_054, 1) | /* CHIP2POD_MDO5 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_055, 1) | /* CHIP2POD_MDO6 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_056, 1) | /* CHIP2POD_MDO7 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_057, 1) | /* CHIP2POD_MOCLK */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_058, 1) | /* PKT_CLK2 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_059, 1);  /* PKT_DATA2 */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13
	* GPIO_060    : PKT_SYNC2    (1)
	* GPIO_061    : PKT_CLK3     (1)
	* GPIO_062    : ALT_TP_IN_25 (0)
	* GPIO_063    : TP_OUT_31    (0)
	* GPIO_064    : PKT_CLK4     (1)
	* GPIO_065    : PKT_DATA4    (1)
	* GPIO_066    : PKT_SYNC4    (1)
	* GPIO_067    : ALT_TP_IN_29 (7)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_060)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_061)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_062)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_063)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_064)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_065)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_066)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_067)
		);

	reg |=
#if (FRAMEWORK_VMS_SFF==1)
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_060, 2) | /* MTSIF1_DATA0 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_061, 2) | /* MTSIF1_SYNC */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_062, 2) | /* MTSIF1_DATA1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_063, 2) | /* MTSIF1_VALID */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_064, 2) | /* MTSIF1_DATA2 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_065, 2) | /* MTSIF1_DATA3 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_066, 2) | /* MTSIF1_DATA4 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_067, 2);  /* MTSIF1_ATS_INC */
#else
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_060, 1) | /* PKT_SYNC2 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_061, 1) | /* PKT_CLK3 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_062, 0) | /* GPIO interrupt for tuner4 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_063, 0) | /* GPIO interrupt for tuner5*/
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_064, 1) | /* PKT_CLK4 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_065, 1) | /* PKT_DATA4 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_066, 1) | /* PKT_SYNC4 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_067, 7);  /* UART1->UART_DEBUG6_RX->ALT_TP_IN_29 */
#endif
	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14
	* GPIO_068    : TP_OUT_27 (7)
	* GPIO_069    : PKT_CLK5  (1)
	* GPIO_070    : PKT_DATA5 (1)
	* GPIO_071    : PKT_SYNC5 (1)
	* GPIO_072    : PKT_CLK6  (1)
	* GPIO_073    : PKT_DATA6 (1)
	* GPIO_074    : PKT_SYNC6 (1)
	* GPIO_075    : PKT_ERROR6(1)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_068)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_069)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_070)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_071)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_072)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_073)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_074)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_075)
		);

	reg |=
#if (FRAMEWORK_VMS_SFF==1)
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_068, 2);  /* MTSIF1_ATS_RST */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_069, 2);  /* MTSIF1_DATA6 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_070, 2);  /* MTSIF1_DATA7 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_071, 2);  /* MTSIF1_DATA5 */
#else
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_068, 7) | /* UART1->UART_DEBUG6_TX->TP_OUT_27 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_069, 1) | /* PKT_CLK5 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_070, 1) | /* PKT_DATA5 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_071, 1) | /* PKT_SYNC5 */
#endif
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_072, 1) | /* PKT_CLK6 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_073, 1) | /* PKT_DATA6 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_074, 1) | /* PKT_SYNC6 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_075, 1);  /* PKT_ERROR6 */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15
	* GPIO_076    : PKT_VALID6(1)
	* GPIO_077    : PKT_CLK7(1)
	* GPIO_078    : PKT_DATA7(1)
	* GPIO_079    : PKT_SYNC7(1)
	* GPIO_080    : linux owns it for UART0
	* GPIO_081    : linux owns it for UART0
	* GPIO_082    : CODEC_SDI(1)
	* GPIO_083    : CODEC_SCLK(1)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_076)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_077)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_078)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_079)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_082)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_083)
		);


	reg |=  
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_076, 1) | /* PKT_VALID6 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_077, 1) | /* PKT_CLK7 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_078, 1) | /* PKT_DATA7 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_079, 1) | /* PKT_SYNC7 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_082, 0) | /* GPIO interrupt for 3128 daughter card. */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_083, 1);  /* CODEC_SCLK */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16
	* GPIO_084    : CODEC_FSYNCB(1)
	* GPIO_085    : CODEC_MCLK(1)
	* GPIO_086    : CODEC_SDO(1)
	* GPIO_087    : MOCA_ACTIVITY(1)
	* GPIO_088    : MOCA_LINK(1)
	* GPIO_089    : SC0_IO(1)
	* GPIO_090    : SC0_CLK(1)
	* GPIO_091    : SC0_RST(1)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_084)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_085)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_086)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_087)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_088)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_089)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_090)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_091)
		);

	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_084, 1) | /* CODEC_FSYNCB */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_085, 1) | /* CODEC_MCLK */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_086, 1) | /* CODEC_SDO */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_087, 1) | /* MOCA_ACTIVITY */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_088, 1) | /* MOCA_LINK */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_089, 1) | /* SC0_IO */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_090, 1) | /* SC0_CLK */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_091, 1);  /* SC0_RST */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17
	* GPIO_092    : SC0_PRES(1)
	* GPIO_093    : SC0_VCC(1)
	* GPIO_094    : SC1_IO(1)
	* GPIO_095    : SC1_CLK(1)
	* GPIO_096    : SC1_RST(1)
	* GPIO_097    : SC1_PRES(1)
	* GPIO_098    : SC1_VCC(1)
	* GPIO_103    : RMX_DATA0(3)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_092)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_093)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_094)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_095)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_096)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_097)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_098)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_103)
		);

	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_092, 1) | /* SC0_PRES */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_093, 1) | /* SC0_VCC */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_094, 1) | /* SC1_IO */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_095, 1) | /* SC1_CLK */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_096, 1) | /* SC1_RST */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_097, 1) | /* SC1_PRES */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_098, 1) | /* SC1_VCC */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_103, 3);  /* RMX_DATA0 */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18
	* GPIO_104    : RMX_SYNC0(3)
	* GPIO_105    : RMX_CLK1(3)
	* GPIO_106    : RMX_DATA1(3)
	* GPIO_107    : RMX_SYNC1(3)
	* GPIO_108    : UHF_LNA_PWRDN(3)
	* GPIO_109    : PWM_2(3)
	* GPIO_110    : BT_CLK_26087(3)
	* SGPIO_00    : BSC_M2_SCL(1)
	*/

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18);

	reg &= ~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_104)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_105)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_106)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_107)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_108)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_109)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_110)  |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00)
		);

	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_104, 3) | /* RMX_SYNC0 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_105, 3) | /* RMX_CLK1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_106, 3) | /* RMX_DATA1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_107, 3) | /* RMX_SYNC1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_108, 3) | /* UHF_LNA_PWRDN */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_109, 3) | /* PWM_2 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_110, 3) | /* BT_CLK_26087 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00, 1);  /* BSC_M2_SCL */

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, reg);


	/* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19
	* SGPIO_01    : BSC_M2_SDA(1)
	* SGPIO_02    : BSC_M3_SCL(1)
	* SGPIO_03    : BSC_M3_SDA(1)
	*/
	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19);
	reg &=~(
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01 ) |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02 ) |
		BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03 )
		);

	reg |=(
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01, 1 ) |  /* BSC_M2_SDA */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02, 1 ) |  /* BSC_M3_SCL */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03, 1 )
		);

	BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, reg);


	/* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0
	* AON_GPIO_00    : LED_KD_0(1)
	* AON_GPIO_01    : LED_KD_1(1)
	* AON_GPIO_02    : LED_KD_2(1)
	* AON_GPIO_03    : LED_KD_3(1)
	* AON_GPIO_04    : LED_LS_0(1)
	* AON_GPIO_05    : LED_LS_1(2)
	* AON_GPIO_06    : LED_LS_2(2)
	*/
	reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0);
	reg &=~(
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_02 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_06 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_07 )
		);

	reg |=(
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_07, 1 ) |  /* LED_LS_3 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00, 1 ) |  /* LED_KD_0 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01, 1 ) |  /* LED_KD_1 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_02, 1 ) |  /* LED_KD_2 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03, 1 ) |  /* LED_KD_3 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04, 1 ) |  /* LED_LS_0 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05, 2 ) |  /* LED_LS_1 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_06, 2 )    /* LED_LS_2 */
		);

	BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, reg);

	/* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1
	* AON_GPIO_07    : LED_LS_3(1)
	* AON_GPIO_08    : LED_LS_4(0)
	* AON_GPIO_09    : LED_LD_0(1)
	* AON_GPIO_10    : LED_LD_1(1)
	* AON_GPIO_11    : LED_LD_2(1)
	* AON_GPIO_12    : LED_LD_3(1)
	* AON_GPIO_13    : LED_LD_4(1)
	* AON_GPIO_14    : LED_LD_5(1)
	*/
	reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1);
	reg &=~(
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_09 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_13 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_14 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_15 )
		);

	reg |=(
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_15, 1 ) |	/* LED_LD_6 */
#if (FRAMEWORK_VMS_SFF==1)
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08, 1 ) |	/* RFM Channel Select */
#else
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08, 0 ) |	/* LED_LS_4 */
#endif
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_09, 1 ) |	/* LED_LD_0 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10, 1 ) |	/* LED_LD_1 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11, 1 ) |	/* LED_LD_2 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12, 1 ) |	/* LED_LD_3 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_13, 1 ) |	/* LED_LD_4 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_14, 1 )	/* LED_LD_5 */
		);

	BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, reg);


	/* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2
	* AON_GPIO_15    : LED_LD_6(1)
	* AON_GPIO_16    : LED_LD_7(1)
	* AON_GPIO_17    : LED_OUT(1)
	* GPIO_099       : PWM_0(1)
	* GPIO_100       : PWM_1(1)
	* GPIO_101       : IR_INT(1)
	* GPIO_102       : RMX_CLK0(3)
	* AON_SGPIO_00   : BSC_M0_SCL(1)
	*/
	reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2);
	reg &=~(
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_16 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_17 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_099 )    |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_100 )    |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_101 )    |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_102 )    |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00 )|
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_01 )
		);

	reg |=(
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_01, 1 ) |  /* BSC_M0_SDA */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_16,  1 ) |  /* LED_LD_7 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_17,  1 ) |  /* LED_OUT */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_099,     1 ) |  /* PWM_0 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_100,     1 ) |  /* PWM_1 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_101,     1 ) |  /* IR_INT */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_102,     3 ) |  /* RMX_CLK0 */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00, 1 )   /* BSC_M0_SCL */
		);

	BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, reg);


	/* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3
	* AON_SGPIO_02   : BSC_S1_SCL(2)
	* AON_SGPIO_03   : BSC_S1_SDA(2)
	* AON_SGPIO_04   : BSC_M0_SDA(1)
	* AON_SGPIO_05   : BSC_M0_SDA(1)
	*/

	reg &=~(
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02 ) |
		BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03 )
		);

	reg |=(
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04, 1 ) |  /* BSC_M4_SCL */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05, 1 ) |  /* BSC_M4_SDA */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02, 2 ) |  /* BSC_S1_SCL */
		BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03, 2 )    /* BSC_S1_SDA */
		);

	BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, reg);


	/* Configure the AVD UARTS to debug mode.  AVD0_OL -> UART1, AVD1_OL -> UART2. */
	reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
	reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_6_cpu_sel) | BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_5_cpu_sel));
	reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_6_cpu_sel, SVD0_OL);
	reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_5_cpu_sel, AUDIO_FP0);
	BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,reg);

	reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
	reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
	reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_SYS);
	BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, reg);

	return ;

}

/***************************************************************************
Summary:
Configure Vcxo for platform
Description:
The core module must be initialized for this to be called
***************************************************************************/
static void BVcxo_Init( BFramework_Info  *pFrameworkInfo )
{
#if 1
	BREG_Handle hRegister = pFrameworkInfo->hReg;
	uint32_t ui32RegVal;
	int index;

	for ( index = 0; index < 3; index++ )
	{
		unsigned offset = (BCHP_VCXO_1_RM_CONTROL - BCHP_VCXO_0_RM_CONTROL)*index;

		ui32RegVal = BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, RESET, 1);
		BREG_Write32(hRegister, BCHP_VCXO_0_RM_CONTROL+offset, ui32RegVal);

		/* assert reset */
		if ( index == 0 )
		{
			ui32RegVal = BCHP_MASK(CLKGEN_PLL_VCXO0_PLL_RESET, RESETD);
			BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_PLL_VCXO0_PLL_RESET, ui32RegVal, ui32RegVal);
		}
		else if ( index == 1 )
		{
			ui32RegVal = BCHP_MASK(CLKGEN_PLL_VCXO1_PLL_RESET, RESETD);
			BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_PLL_VCXO1_PLL_RESET, ui32RegVal, ui32RegVal);
		}
		else
		{
			ui32RegVal = BCHP_MASK(CLKGEN_PLL_VCXO2_PLL_RESET, RESETD);
			BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_PLL_VCXO2_PLL_RESET, ui32RegVal, ui32RegVal);
		}

		ui32RegVal = BCHP_FIELD_DATA(VCXO_0_RM_RATE_RATIO, DENOMINATOR, 32);
		BREG_Write32(hRegister, BCHP_VCXO_0_RM_RATE_RATIO+offset, ui32RegVal);

		ui32RegVal =
			BCHP_FIELD_DATA(VCXO_0_RM_SAMPLE_INC, NUMERATOR, 29) |
			BCHP_FIELD_DATA(VCXO_0_RM_SAMPLE_INC, SAMPLE_INC, 3);
		BREG_Write32(hRegister, BCHP_VCXO_0_RM_SAMPLE_INC+offset, ui32RegVal);

		ui32RegVal = BCHP_FIELD_DATA(VCXO_0_RM_PHASE_INC, PHASE_INC, 0x7d634);
		BREG_Write32(hRegister, BCHP_VCXO_0_RM_PHASE_INC+offset, ui32RegVal);

		BREG_Write32(hRegister, BCHP_VCXO_0_RM_INTEGRATOR+offset, 0x0);

		ui32RegVal =
			BCHP_FIELD_DATA(VCXO_0_RM_FORMAT, SHIFT, 2) |
			BCHP_FIELD_DATA(VCXO_0_RM_FORMAT, STABLE_COUNT, 10000);
		BREG_Write32(hRegister, BCHP_VCXO_0_RM_FORMAT+offset, ui32RegVal);

		ui32RegVal =
			BCHP_FIELD_DATA(VCXO_0_RM_OFFSET, OFFSET_ONLY, 0) |
			BCHP_FIELD_DATA(VCXO_0_RM_OFFSET, OFFSET, 0x10000000);
		BREG_Write32(hRegister, BCHP_VCXO_0_RM_OFFSET+offset, ui32RegVal);

		ui32RegVal =
			BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, TIMEBASE, index) |	/* Timebase 0 */
			BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, FREE_RUN, 0) |
			BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, DITHER, 1) |
			BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, DIRECT_GAIN, 2) |
			BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, INT_GAIN, 4) |
			BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, RESET, 0);
		BREG_Write32(hRegister, BCHP_VCXO_0_RM_CONTROL+offset, ui32RegVal);

		/* Deassert reset */
		if ( index == 0 )
		{
			ui32RegVal = BCHP_MASK(CLKGEN_PLL_VCXO0_PLL_RESET, RESETD);
			BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_PLL_VCXO0_PLL_RESET, ui32RegVal, 0);
		}
		else if ( index == 1 )
		{
			ui32RegVal = BCHP_MASK(CLKGEN_PLL_VCXO1_PLL_RESET, RESETD);
			BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_PLL_VCXO1_PLL_RESET, ui32RegVal, 0);
		}
		else
		{
			ui32RegVal = BCHP_MASK(CLKGEN_PLL_VCXO2_PLL_RESET, RESETD);
			BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_PLL_VCXO2_PLL_RESET, ui32RegVal, 0);
		}
	}
#else
	BSTD_UNUSED(pFrameworkInfo);
#endif
}

#if (FRAMEWORK_DO_FPGA == 1)

/***************************************************************************/
#define  CPU_PHYS_FPGA_BASE  REG_FPGA_PBASE

static int BFPGA_Init(
					  const BSystem_Info *pSysInfo,
					  BFramework_Info    *pFrameworkInfo )
{
	BERR_Code   eStatus = BERR_SUCCESS, eDummy;
	BFPGA_Settings  fpgaDefSettings;
	BFrmWorkBoard_Info  *pFrmWorkBoardInfo;

	pFrmWorkBoardInfo = pFrameworkInfo->hFrmWorkBoard;
	pFrmWorkBoardInfo->hFpgaReg = NULL;
	pFrmWorkBoardInfo->hFpga = NULL;

	/* The following FPGA programming code is copied from bsettop_p_boot_fpga
	* in bsettop_board.c. We replaced the error check with TestError,
	* b_board.hFpga with pFrmWorkBoardInfo->hFpga, b_board.fpga.reg with
	* pFrmWorkBoardInfo->hFpgaReg, b_board.sys.fpga.addr with
	* pSysInfo->bregFpgaBaseAddr, . */

	/* open fpga handle */
	BSTD_UNUSED(pSysInfo);
	TestError( BFPGA_GetDefaultSettings( &fpgaDefSettings ),
		"ERROR: BFPGA_GetDefaultSettings: %s:%d" );
	TestError( BFPGA_Open(&pFrmWorkBoardInfo->hFpga, NULL, pFrmWorkBoardInfo->ahI2cRegHandles[4], &fpgaDefSettings ),
		"ERROR: BFPGA_OPEN: %s:%d" );

	TestError( BFPGA_Reset( pFrmWorkBoardInfo->hFpga	),
		"ERROR: BFPGA_RESET: %s:%d" );

	/* Use same config as 97401. */
	TestError( BFPGA_SetTsOutput( pFrmWorkBoardInfo->hFpga, BFPGA_OutputSelect_ePkt0, BFPGA_TsSelect_eQam_Ds1),
		"ERROR: BFPGA_SetTsOutput: %s:%d" );

	TestError( BFPGA_SetTsOutput( pFrmWorkBoardInfo->hFpga, BFPGA_OutputSelect_ePkt1, BFPGA_TsSelect_eMb3517),
		"ERROR: BFPGA_SetTsOutput: %s:%d" );

	TestError( BFPGA_SetTsOutput( pFrmWorkBoardInfo->hFpga, BFPGA_OutputSelect_ePkt3, BFPGA_TsSelect_eSlot1_Ts4),
		"ERROR: BFPGA_SetTsOutput: %s:%d" );

	/* temp had to remove for 97455 for 3255. don't know why. */
	TestError( BFPGA_SetTsOutput( pFrmWorkBoardInfo->hFpga, BFPGA_OutputSelect_ePkt2, BFPGA_TsSelect_eLvds_1),
		"ERROR: BFPGA_SetTsOutput: %s:%d" );

	eDummy = BDBG_SetModuleLevel("BFPGA_NAME", BDBG_eWrn);
	BFPGA_DumpConfiguration( pFrmWorkBoardInfo->hFpga );

Error:
	return eStatus;
}
/* #if (FRAMEWORK_DO_FPGA == 1) */
#endif

/***************************************************************************/
int BFrmWorkBoard_Init(
					   const BSystem_Info *pSysInfo,
					   BFramework_Info    *pFrameworkInfo )
{
	BERR_Code             eStatus = BERR_SUCCESS;
	BFrmWorkBoard_Info *pFrmWorkBoardInfo;
	BMEM_Heap_Settings mem_heap_settings;

	/* allocate board specific frame work context */
	pFrmWorkBoardInfo = (BFrmWorkBoard_Info *)
		(BKNI_Malloc(sizeof(BFrmWorkBoard_Info)));
	if(NULL == pFrmWorkBoardInfo)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BKNI_Memset(pFrmWorkBoardInfo, 0x0, sizeof(BFrmWorkBoard_Info));
	pFrameworkInfo->hFrmWorkBoard = pFrmWorkBoardInfo;

#if (FRAMEWORK_DO_SHMOO)
	BTST_VideoShmoo( pFrameworkInfo->hMem, pFrameworkInfo->hChp, pFrameworkInfo->hReg );
#endif

	BPinMux_Init( pFrameworkInfo );

#if (FRAMEWORK_DO_I2C == 1)
	/* setup i2c */
	BI2C_Init( pFrameworkInfo );
#endif

#if (FRAMEWORK_DO_FPGA == 1)
	/* open FPGA handle and init fpga vdc */
	BFPGA_Init(pSysInfo, pFrameworkInfo);
#else
	BSTD_UNUSED(pSysInfo);
#endif

	BVcxo_Init(pFrameworkInfo);

	/* Initialize other chips here */

	/* Memory heap for 16-bit DDR_1: 1-on-1 mapping */
	mem_heap_settings.eSafetyConfig = BMEM_CONFIG_FASTEST;
	mem_heap_settings.eBookKeeping  = BMEM_BOOKKEEPING_SYSTEM;
	mem_heap_settings.uiAlignment = 7;

#ifdef LINUX
	mem_heap_settings.pCachedAddress = pSysInfo->bmemCachedAddressU;
#endif
	TestError( BMEM_Heap_Create(pFrameworkInfo->hMemModule,
		pSysInfo->bmemAddressU, pSysInfo->bmemOffsetU, pSysInfo->bmemSizeU, &mem_heap_settings,
		&pFrmWorkBoardInfo->hMemcU),
		"ERROR: BMEM_Heap_Create: %s:%d" );

#ifdef LINUX
	mem_heap_settings.pCachedAddress = pSysInfo->bmemCachedAddress1;
#endif
	TestError( BMEM_Heap_Create(pFrameworkInfo->hMemModule,
		pSysInfo->bmemAddress1, pSysInfo->bmemOffset1, pSysInfo->bmemSize1, &mem_heap_settings,
		&pFrmWorkBoardInfo->hMemc1),
		"ERROR: BMEM_Heap_Create: %s:%d" );

	/* Success */
	return 0;

Error:
	/* Free resources and return error */
	BFrmWorkBoard_Uninit( pFrameworkInfo );
	return eStatus;
}

void BFrmWorkBoard_Uninit( BFramework_Info *pFrameworkInfo )
{
	BFrmWorkBoard_Info  *pFrmWorkBoardInfo;
#if (FRAMEWORK_DO_I2C == 1)
	unsigned int ui;
#endif

	pFrmWorkBoardInfo = pFrameworkInfo->hFrmWorkBoard;
	if (NULL == pFrmWorkBoardInfo )
		return;

	if ( pFrmWorkBoardInfo->hMemc1)
	{
		BMEM_Heap_Destroy(pFrmWorkBoardInfo->hMemc1);
	}

#if (FRAMEWORK_DO_I2C == 1)
	for (ui=0; ui<pFrmWorkBoardInfo->uiNumI2c; ui++)
	{
		BI2C_CloseI2cRegHandle(pFrmWorkBoardInfo->ahI2cRegHandles[ui]);
		BI2C_CloseChannel(pFrmWorkBoardInfo->ahI2cChannels[ui]);

	}
	if ( pFrmWorkBoardInfo->hI2c )
	{
		BI2C_Close(pFrmWorkBoardInfo->hI2c);
	}
#endif


#if (FRAMEWORK_DO_FPGA == 1)
	if ( pFrmWorkBoardInfo->hFpgaReg )
	{
		BREG_Close(pFrmWorkBoardInfo->hFpgaReg);
	}
	if (pFrmWorkBoardInfo->hFpga )
	{
		BFPGA_Close(pFrmWorkBoardInfo->hFpga);
	}
#endif

	BKNI_Free( pFrmWorkBoardInfo );
	pFrameworkInfo->hFrmWorkBoard = NULL;
}

/* End of file */
