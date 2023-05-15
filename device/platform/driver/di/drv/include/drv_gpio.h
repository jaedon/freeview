/**
 * drv_gpio.h
*/

/**
 * @defgroup		DRV_GPIO : gpio driver apis
 * @author		Tae-Hyuk Kang
 * @note			DRV_GPIO APIs
 * @brief			Define DRV_GPIO APIs
 * @file 			drv_gpio.h
*/


#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

#include "nexus_base_types.h"
#include "nexus_gpio.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

typedef enum
{
	GPIO_MODE_READ 				= NEXUS_GpioMode_eInput,
	GPIO_MODE_WRITE 				= NEXUS_GpioMode_eOutputPushPull,
	GPIO_MODE_INTERRUPT 	       	= NEXUS_GpioMode_eInput,
	GPIO_MODE_INVALID 			= NEXUS_GpioMode_eMax
} GPIO_MODE_t;

typedef enum
{
	GPIO_INT_MODE_DISABLED 		= NEXUS_GpioInterrupt_eDisabled,
	GPIO_INT_MODE_RISING_EDGE		= NEXUS_GpioInterrupt_eRisingEdge,
	GPIO_INT_MODE_FALLING_EDGE	= NEXUS_GpioInterrupt_eFallingEdge,
	GPIO_INT_MODE_BOTH_EDGES		= NEXUS_GpioInterrupt_eEdge,
	GPIO_INT_MODE_LOW_LEVEL		= NEXUS_GpioInterrupt_eLow,
	GPIO_INT_MODE_HIGH_LEVEL 		= NEXUS_GpioInterrupt_eHigh,
	GPIO_INT_MODE_INVALID			= NEXUS_GpioInterrupt_eMax
}	GPIO_INT_MODE_t;

typedef enum
{
	GPIO_LOW = NEXUS_GpioValue_eLow,
	GPIO_HI = NEXUS_GpioValue_eHigh,
	GPIO_INVAILD = NEXUS_GpioValue_eMax
}	GPIO_STATE_t;

typedef enum
{
	GPIO_ID_VIDEO_SW   = 0,
	GPIO_ID_HW_MUTE,
	GPIO_ID_SCART_IT,
	GPIO_ID_nCI_CD1  ,
	GPIO_ID_nCI_CD2,
	GPIO_ID_CI_RST,
	GPIO_ID_CI_PWR,
	GPIO_ID_nCI_OUTEN,
	GPIO_ID_VIDEO_SEL,
	GPIO_ID_FAN_SPEED,
	GPIO_ID_nCH0_RST,
	GPIO_ID_nCH1_RST,
	GPIO_ID_nCH2_RST,
	GPIO_ID_nCH3_RST,
	GPIO_ID_LOOPTHROUGH,
	GPIO_ID_LED_RADIO,
	GPIO_ID_LED_TV,
	GPIO_ID_SC0_PWR_SEL,
	GPIO_ID_SC1_PWR_SEL,
	GPIO_ID_SC0_UPGRADE_EN,
	GPIO_ID_SC0_DIV,
	GPIO_ID_SC1_DIV,
	GPIO_ID_DCFG_SCANLINE_0,
	GPIO_ID_DCFG_SCANLINE_1,
	GPIO_ID_DCFG_ASPECTRATIO_0,
	GPIO_ID_DCFG_ASPECTRATIO_1,
	GPIO_ID_DCFG_IORP,
	GPIO_ID_MODEM_RESET,
	GPIO_ID_MODEM_DTR,
	GPIO_ID_MODEM_DCD,
	GPIO_ID_SEL_DEMOD,
	GPIO_ID_RST_OFDM,
	GPIO_ID_RST_QAM,
	GPIO_ID_NVRAM_WP,
	GPIO_ID_HW_REV0,
	GPIO_ID_HW_REV1,
	GPIO_ID_HW_REV2,
	GPIO_ID_HW_REV3,
	GPIO_ID_HDD_PWR_CTRL,
	GPIO_ID_ANT_PWR_CHK,
	GPIO_ID_ANT_PWR_EN,
	GPIO_ID_nEXT_HDMI_TX_INT,
	GPIO_ID_nEXT_HDMI_TX_RST,
	GPIO_ID_ANT_EN,
	GPIO_ID_USB_PWR_CHK,
	GPIO_ID_USB_PWR_EN,
	GPIO_ID_SC1_VCC,
	GPIO_ID_SC1_RESET,
	GPIO_ID_SC1_DATA,
	GPIO_ID_FBO_SCART,
	GPIO_ID_SLB1_SCART,
	GPIO_ID_SLB2_SCART,
	GPIO_ID_SD_HD_SEL,
	GPIO_ID_NAND_WP,
	GPIO_ID_UART_SEL,
	GPIO_ID_KEY_STBY,
	GPIO_ID_KEY_CHUP,
	GPIO_ID_KEY_CHDN,
	GPIO_ID_KEY_VOLUP,
	GPIO_ID_KEY_VOLDN,
	GPIO_ID_KEY_SW_RESET,
	GPIO_ID_IRQ_FRONT,
	GPIO_ID_IRQ_FRONT2,
	GPIO_ID_SPI_M_CLK,
	GPIO_ID_SPI_M_MOSI,
	GPIO_ID_SPI_M_MISO,
	GPIO_ID_SPI_M_SS0,

	/* Led */
	GPIO_ID_LED_STBY,
	GPIO_ID_LED_STBY2,
	GPIO_ID_LED_POWER,
	GPIO_ID_LED_POWER2,
	GPIO_ID_LED_WORK,
	GPIO_ID_LED_STBYCTL,
	GPIO_ID_LED_POWERCTL,

	GPIO_ID_CIMAX_INT,
	GPIO_ID_CIMAX_RESET,

	GPIO_ID_HDD_PWR_CTRL_2,
	GPIO_ID_ALIVE, /* JC_NGS_MODIFY */
	GPIO_ID_CI_IRQ_N, /*In brcm platform, CI_IRQ_N is not used. So set it to gpio_in mode.*/
	GPIO_ID_nLNBP0_RST,
	GPIO_ID_LNBP_SW_EN,
	GPIO_ID_RF4CE_SRDY,
	GPIO_ID_RF4CE_RST,
	GPIO_ID_RF4CE_MRDY,
	GPIO_ID_TS_SEL0,
	GPIO_ID_TS_SEL1,
	GPIO_ID_TS_SEL2,
	GPIO_ID_TS_SEL3,
	GPIO_ID_TS_SEL10,
	GPIO_ID_TS_SEL11,
	GPIO_ID_TS_SEL12,
	GPIO_ID_TS_SEL13,
	GPIO_ID_TS_SEL14,
	GPIO_ID_TS_SEL15,
	GPIO_ID_TS_SEL16,
	GPIO_ID_TS_SEL17,
	GPIO_ID_CH_LOOPTHROUGH,
	GPIO_ID_STBY_KEY_FROM_MICOM, /*standby key input from micom in S3 mode*/
	GPIO_ID_MICOM_ISP,
	GPIO_ID_MICOM_RESET,
	GPIO_ID_WOL,  /* WOL signal from external phy to main */
	GPIO_ID_DDR_CLAMPING_CIRCUIT, /*DDR clamping circuit driving pin*/
	GPIO_ID_BOOT_MODE, /* Cold-Warm boot selection. */
	GPIO_ID_CH_RST_N,
	GPIO_ID_CH_IRQ_N,
	GPIO_ID_FAN_CONTROL,
	GPIO_ID_LED_REC_RED,
	GPIO_ID_LED_REC_GREEN,
	GPIO_ID_LED_REC_BLUE,
	GPIO_ID_LED_REC_HIGH_BRIGHTNESS,
	GPIO_ID_LED_REC_LOW_BRIGHTNESS,
	GPIO_ID_LED_NETWORK_RED,
	GPIO_ID_LED_NETWORK_GREEN,
	GPIO_ID_LED_NETWORK_BLUE,
	GPIO_ID_LED_NETWORK_HIGH_BRIGHTNESS,
	GPIO_ID_LED_NETWORK_LOW_BRIGHTNESS,
	GPIO_ID_BBIC_MAC_RESET,
	GPIO_ID_BBIC_MAC_RTD,
	GPIO_ID_BBIC_MAC_WLAN_DISABLE1,
	GPIO_ID_BBIC_MAC_WLAN_DISABLE2,
	GPIO_ID_HUB_RESET,
	GPIO_ID_IP_LED,
	GPIO_ID_CEC,
	GPIO_ID_CM_RST_N,
	GPIO_ID_ENET_LED,
	GPIO_ID_S2_ON,
	GPIO_ID_BT_HOST_WAKE,
	GPIO_ID_WIFI_HOST_WAKE,
	GPIO_ID_5V_STANDBY,
	GPIO_ID_SPI_WP,
	GPIO_ID_WIFI_EN,
	GPIO_ID_MAX
} GPIO_ID_t;

typedef enum
{
	GPIO_PINMUX_NCTP = -1,
	GPIO_PINMUX_GPIO = 0,
	GPIO_PINMUX_ALT1,
	GPIO_PINMUX_ALT2,
	GPIO_PINMUX_ALT3,
	GPIO_PINMUX_ALT4,
	GPIO_PINMUX_ALT5,
	GPIO_PINMUX_ALT6,
	GPIO_PINMUX_ALT7,
	GPIO_PINMUX_ALT8,
	GPIO_PINMUX_ALT9,
	GPIO_PINMUX_MAX
} GPIO_PINMUX_FUNCTION ;


typedef struct
{
	GPIO_ID_t                      	gpio_id;
	NEXUS_GpioHandle 		gpio_handle;
	NEXUS_GpioType			gpio_type;
	unsigned 			       	gpio_number;
	GPIO_MODE_t				gpio_Mode;
	GPIO_INT_MODE_t		    	gpio_IntrMode;
	GPIO_STATE_t			gpio_init_state;

} GPIO_ConfigInfo_t;

typedef struct
{
	unsigned int addr;
	unsigned int mask;
	unsigned int shift;
	unsigned int value;
} GPIO_TSConfigInfo_t;

typedef enum {
	DRV_GPIO_STANDBY_STATE_LIGHT_SLEEP = 0,
	DRV_GPIO_STANDBY_STATE_DEEP_SLEEP,
	DRV_GPIO_STANDBY_STATE_MAX
} DRV_GPIO_STANDBY_STATE_E;


typedef void (*GPIO_CALLBACK_t)(void *context, int param);


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error DRV_GPIO_Init(void);
DRV_Error DRV_GPIO_Read(GPIO_ID_t pin, GPIO_STATE_t *val);
DRV_Error DRV_GPIO_Write(GPIO_ID_t pin, GPIO_STATE_t val);
DRV_Error DRV_GPIO_ChangePinmux(GPIO_ID_t gpio_id, GPIO_PINMUX_FUNCTION eType);
DRV_Error DRV_GPIO_INTR_Enable(GPIO_ID_t pinId);
DRV_Error DRV_GPIO_INTR_Disable(GPIO_ID_t pinId);
DRV_Error DRV_GPIO_INTR_Clear(GPIO_ID_t pinId);
DRV_Error DRV_GPIO_INTR_RegisterEventNotify(
                                                            GPIO_ID_t pinId,
                                                            GPIO_CALLBACK_t pfnGpioEventCallback,
                                                            void *pParam1, int pParam2);
DRV_Error DRV_GPIO_ChangeMode(GPIO_ID_t gpio_id, GPIO_MODE_t eMode, GPIO_STATE_t eState);

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
DRV_Error DRV_GPIO_PM_Resume(void);
DRV_Error DRV_GPIO_PM_Suspend(DRV_GPIO_STANDBY_STATE_E state);
#endif

#endif /* !__DRV_GPIO_H__ */

