#include "nexus_platform.h"
/*#include "nexus_gpio_module.h"*/
#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_pin_ctrl.h"
#include "bchp_aon_ctrl.h"

#include "humax_pinmux.h"

#ifdef CONFIG_OS_UCOS
const char g_humaxGpioFunctionTable[HUMAX_NUM_GPIO_PINS] = {
	0x01,
	0x03, 0x7E, 0xF6,
	0x02, 0x81, 0x08,
	0xFF, 0x00, 0x00,
	0x44, 0x04, 
};

HUMAX_GpioTable g_humaxGpioTable[HUMAX_NUM_GPIO_PINS] = {

     {BCHP_AON_CTRL_RESET_CTRL, BCHP_AON_CTRL_RESET_CTRL_front_panel_reset_enable_MASK, BCHP_AON_CTRL_RESET_CTRL_front_panel_reset_enable_SHIFT},

     {0x04172C8, 0XFF0000, 16}, // IODIR_LO
     {0x04172C8, 0xFF00, 8},
     {0x04172C8, 0xFF, 0},

     {0x04172C4, 0xFF0000, 16}, // DATA_LO
     {0x04172C4, 0xEF00, 8},	// E -> readonly bit
     {0x04172C4, 0xFF, 0},

     {0x040A768, 0xFF0000, 16}, // IODIR_EXT_HI
     {0x040A768, 0xFF00, 8},
     {0x040A768, 0xFF, 0},

     {0x040A764, 0x4400, 8}, // DATA_EXT_HI
     {0x040A764, 0x04, 0},
};

#else
 
const char g_humaxGpioFunctionTable[HUMAX_NUM_GPIO_PINS] = {
	/*/////////////   0 ///////////////*/
	HUMAX_GPIO_FUNCTION_ALT1,	/*//0  */  /* MAC0_MII_TX_ERR */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//1  */  /* MAC0_MII_RX_ERR */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//2  */  /* MAC0_MII_TX_CLK */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//3  */  /* MAC0_MII_TX_EN */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//4  */  /* MAC0_MII_TX_D0 */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//5  */  /* MAC0_MII_TX_D1 */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//6  */  /* MAC0_MII_TX_D2 */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//7  */  /* MAC0_MII_TX_D3 */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//8  */  /* MAC0_MII_RX_CLK */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//9  */  /* MAC0_MII_RX_EN */
	/*//////////////  10 //////////////*/
	HUMAX_GPIO_FUNCTION_ALT1,	/*//10  */  /* MAC0_MII_RX_D0 */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//11  */  /* MAC0_MII_RX_D1 */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//12  */  /* MAC0_MII_RX_D2 */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//13  */  /* MAC0_MII_RX_D3 */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//14  */  /* MAC0_MII_MDIO */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//15  */  /* MAC0_MII_MDC */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//16  */  /* MAC0_MII_IRQ */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//17  */  /* MAC0_MII_CRS */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//18  */  /* MAC0_MII_COL */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//19  */  /* N.C. */
	/*//////////////  20 //////////////*/
	HUMAX_GPIO_FUNCTION_NCTP,	/*//20  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//21  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//22  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//23  */  /* BT_LINK */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//24  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//25  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//26  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_ALT3,	/*//27  */  /* SoC_to_Enc_UART_RX */
	HUMAX_GPIO_FUNCTION_ALT3,	/*//28  */  /* SoC_to_Enc_UART_TX */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//29  */  /* SoC_to_BT_RESETn */
	/*//////////////  30 //////////////*/
	HUMAX_GPIO_FUNCTION_NCTP,	/*//30  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//31  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//32  */  /* PLD_RESET */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//33  */  /* PLD_TMS */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//34  */  /* PLD_TCK */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//35  */  /* PLD_TDI */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//36  */  /* PLD_TDO */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//37  */  /* RESET#_ENCODER */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//38  */  /* RESET#_9575 */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//39  */  /* N.C. */
	/*//////////////  40 //////////////*/
	HUMAX_GPIO_FUNCTION_GPIO,	/*//40  */  /* INT_U1_9575 */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//41  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//42  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_ALT3,	/*//43  */  /* SoC_to_UEI_UART_RX */
	HUMAX_GPIO_FUNCTION_ALT3,	/*//44  */  /* SoC_to_UEI_UART_TX */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//45  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//46  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//47  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//48  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//49  */  /* N.C. */
	/*//////////////  50 //////////////*/
	HUMAX_GPIO_FUNCTION_NCTP,	/*//50  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//51  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//52  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//53  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//54  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//55  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//56  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//57  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//58  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//59  */  /* N.C. */
	/*//////////////  60 //////////////*/
	HUMAX_GPIO_FUNCTION_NCTP,	/*//60 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//61 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//62 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//63 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//64 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//65 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//66 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//67 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//68 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//69 */  /* N.C. */
	/*//////////////  70 //////////////*/
	HUMAX_GPIO_FUNCTION_NCTP,	/*//70 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//71 */  /* TSO_CLK */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//72 */  /* TSO_DATA0 */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//73 */  /* TSO_SYNC */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//74 */  /* TSO_ERROR */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//75 */  /* TSO_DVALID */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//76 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//77 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//78 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//79 */  /* N.C. */
	/*//////////////  80 //////////////*/ 
	HUMAX_GPIO_FUNCTION_GPIO,	/*//80 */  /* IR_JTAG_TCK */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//81 */  /* IR_JTAG_TMS */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//82 */  /* IR_JTAG_TDO */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//83 */  /* IR_JTAG_TDI */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//84 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//85 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//86 */  /* SPEED_FAN */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//87 */  /* FAN_PWM */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//88 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//89 */  /* N.C. */
	/*//////////////  90 //////////////*/
	HUMAX_GPIO_FUNCTION_GPIO,	/*//90 */  /* LOGO_LED_B */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//91 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//92 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//93 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//94 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//95 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//96 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//97 */  /* N.C. */ 
	HUMAX_GPIO_FUNCTION_GPIO,	/*//98 */  /* LOGO_LED_R */ 
	HUMAX_GPIO_FUNCTION_NCTP,	/*//99 */  /* N.C. */
	/*////////////// 100 //////////////*/
	HUMAX_GPIO_FUNCTION_NCTP,	/*//100 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//101 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//102 */  /* LOGO_LED_G */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//103 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//104 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//105 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//106 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//107 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//108 */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//109 */  /* N.C. */
	/*////////////// 110 //////////////*/	
	HUMAX_GPIO_FUNCTION_ALT2,	/*//110 */  /* GPHY_ACTIVITY */
	HUMAX_GPIO_FUNCTION_ALT2,	/*//111 */  /* GPHY_LINK */
	
	/*////////////// SGPIO 00 (0~5)//////////////*/	
	HUMAX_GPIO_FUNCTION_ALT1,	/*//0  */  /* HDMI_SPLITTER SCL */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//1  */  /* HDMI_SPLITTER_SDA */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//2  */  /* BSC_M4_SCL */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//3  */  /* BSC_M4_SDA */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//4  */  /* N.C */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//5  */  /* N.C */
	
	/*////////////// AON_GPIO 00(0~17)//////////////*/	
	HUMAX_GPIO_FUNCTION_NCTP,	/*//0  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//1  */  /* WIFI_WAKE1 */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//2  */  /* BT_HOST_WAKE */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//3  */  /* BT_DEV_WAKE */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//4  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//5  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//6  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//7  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//8  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//9  */  /* WIFI_WAKE0 */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//10  */ /* WIFI_LED1 */

	HUMAX_GPIO_FUNCTION_NCTP,	/*//11  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//12  */  /* INT_U1_PLD */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//13  */  /* WIFI_LED0 */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//14  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//15  */  /* S3_STANDBY_COLD */
	HUMAX_GPIO_FUNCTION_NCTP,	/*//16  */  /* N.C. */
	HUMAX_GPIO_FUNCTION_GPIO,	/*//17  */  /* STANDBY_LED */

	/*////////////// AON_SGPIO (0~5)//////////////*/	
	HUMAX_GPIO_FUNCTION_ALT2,	/*//  */ /* HDMI0_TX_7444_SCL */
	HUMAX_GPIO_FUNCTION_ALT2,	/*//  */ /* HDMI0_TX_7444_SDA */
	HUMAX_GPIO_FUNCTION_ALT3,	/*//  */ /* HDMI_RX_BSC_SCL */
	HUMAX_GPIO_FUNCTION_ALT3,	/*//  */ /* HDMI_RX_BSC_SDA */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//  */ /* LSCL */
	HUMAX_GPIO_FUNCTION_ALT1,	/*//  */ /* LSDA */
};

/*************************
*	gpio: 0~111 :112
*	sgpio: 0~5 : 6
*	aon_gpio: 0~17 : 18
*	aon_sgpio: 0~5 : 6
*************************/
HUMAX_GpioTable g_humaxGpioTable[HUMAX_NUM_GPIO_PINS] = {
     {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_000_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_000_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_001_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_001_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_002_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_002_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_003_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_003_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_004_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_004_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_005_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_005_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_006_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_006_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_007_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_007_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_008_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_008_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_009_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_009_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_010_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_010_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_011_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_011_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_012_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_012_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_013_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_013_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_014_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_014_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_015_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_015_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_016_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_016_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_017_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_017_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_018_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_018_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_019_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_019_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_020_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_020_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_021_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_021_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_022_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_022_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_023_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_023_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_024_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_024_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_025_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_025_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_026_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_026_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_027_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_027_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_028_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_028_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_029_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_029_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_030_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_030_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_031_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_031_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_032_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_032_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_033_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_033_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_034_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_034_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_035_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_035_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_036_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_036_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_037_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_037_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_038_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_038_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_039_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_039_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_040_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_040_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_041_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_041_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_042_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_042_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_043_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_043_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_044_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_044_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_045_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_045_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_046_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_046_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_047_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_047_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_048_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_048_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_049_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_049_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_050_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_050_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_051_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_051_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_052_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_052_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_053_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_053_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_054_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_054_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_055_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_055_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_056_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_056_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_057_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_057_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_058_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_058_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_059_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_059_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_060_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_060_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_061_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_061_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_062_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_062_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_063_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_063_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_064_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_064_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_065_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_065_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_066_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_066_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_067_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_067_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_068_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_068_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_069_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_069_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_070_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_070_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_071_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_071_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_072_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_072_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_073_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_073_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_074_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_074_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_075_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_075_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_076_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_076_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_077_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_077_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_078_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_078_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_079_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_079_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_080_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_080_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_081_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_081_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_082_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_082_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_083_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_083_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_084_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_084_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_085_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_085_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_086_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_086_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_087_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_087_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_088_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_088_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_089_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_089_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_090_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_090_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_091_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_091_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_092_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_092_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_093_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_093_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_094_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_094_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_095_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_095_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_096_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_096_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_097_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19_gpio_097_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_098_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_098_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_099_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_099_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_100_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_100_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_101_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_101_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_102_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_102_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_103_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_103_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_104_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_104_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_105_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20_gpio_105_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_106_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_106_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_107_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_107_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_108_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_108_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_109_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_109_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_110_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_110_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_111_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_gpio_111_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_00_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_00_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_01_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_01_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22_sgpio_02_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22_sgpio_02_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22_sgpio_03_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22_sgpio_03_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22_sgpio_04_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22_sgpio_04_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22_sgpio_05_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22_sgpio_05_SHIFT},

    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_06_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_06_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_07_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_07_SHIFT},

    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_SHIFT},

    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_SHIFT},

    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_01_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_01_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_02_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_02_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_03_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_03_SHIFT},
		{BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_04_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_04_SHIFT},
		{BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_05_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_05_SHIFT}
};
#endif