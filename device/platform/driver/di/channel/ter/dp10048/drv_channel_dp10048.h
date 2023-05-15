/******************************************************************************

File Name   : drv_channel.h

Description : Channel driver header  : channel driver specific functions

******************************************************************************/
#ifndef _TDA_10048_H_
#define _TDA_10048_H_


/*=============================================================================
	Includes
=============================================================================*/

/*=============================================================================
	Definitions
=============================================================================*/

/*=============================================================================
	Function Prototype
=============================================================================*/
HBOOL DP10048_CheckLock ( int nChannelId, HUINT32 unWorkingID );

void DRV_CH_EnableI2c (HUINT8 unitId);
void DRV_CH_DisableI2c (HUINT8 unitId);
/*=============================================================================
	Definitions for TDA10048 Reg Addr
=============================================================================*/
#define DEMOD_10048_REG_IDENTITY				0x00
#define DEMOD_10048_REG_VERSION				0x01

#define DEMOD_10048_REG_DSP_CODE_CPT		0x0C
#define DEMOD_10048_REG_DSP_CODE_IN			0x0E

#define DEMOD_10048_REG_IN_CONF1				0x10
#define DEMOD_10048_REG_IN_CONF2				0x11
#define DEMOD_10048_REG_IN_CONF3				0x12
#define DEMOD_10048_REG_OUT_CONF1			0x14
#define DEMOD_10048_REG_OUT_CONF2			0x15
#define DEMOD_10048_REG_OUT_CONF3			0x16
#define DEMOD_10048_REG_AUTO					0x18

#define DEMOD_10048_REG_SYNC_STATUS			0x1A
#define DEMOD_10048_REG_SCAN_CPT				0x1C

#define DEMOD_10048_REG_CONF_C4_1			0x1E
#define DEMOD_10048_REG_CONF_C4_2			0x1F

#define DEMOD_10048_REG_CODE_IN_RAM			0x20

#define DEMOD_10048_REG_CHANNEL_INFO1_R		0x22
#define DEMOD_10048_REG_CHANNEL_INFO2_R		0x23
#define DEMOD_10048_REG_CHANNEL_INFO1		0x24
#define DEMOD_10048_REG_CHANNEL_INFO2		0x25
#define DEMOD_10048_REG_TIME_ERROR_R			0x26
#define DEMOD_10048_REG_TIME_ERROR			0x27
#define DEMOD_10048_REG_FREQ_ERROR_LSB_R	0x28
#define DEMOD_10048_REG_FREQ_ERROR_MSB_R	0x29
#define DEMOD_10048_REG_FREQ_ERROR_LSB		0x2A
#define DEMOD_10048_REG_FREQ_ERROR_MSB		0x2B

#define DEMOD_10048_REG_IT_SEL				0x30
#define DEMOD_10048_REG_IT_STAT				0x32

#define DEMOD_10048_REG_DSP_ADR_LSB			0x3C
#define DEMOD_10048_REG_DSP_ADR_MSB			0x3D
#define DEMOD_10048_REG_DSP_REG_LSB			0x3E
#define DEMOD_10048_REG_DSP_REG_MSB			0x3F

#define DEMOD_10048_REG_CONF_TRISTATE1		0x44
#define DEMOD_10048_REG_CONF_TRISTATE2		0x45
#define DEMOD_10048_REG_CONF_POLARITY		0x46

#define DEMOD_10048_REG_GPIO_SP_DS0			0x48
#define DEMOD_10048_REG_GPIO_SP_DS1			0x49
#define DEMOD_10048_REG_GPIO_SP_DS2			0x4A
#define DEMOD_10048_REG_GPIO_SP_DS3			0x4B
#define DEMOD_10048_REG_GPIO_OUT_SEL			0x4C
#define DEMOD_10048_REG_GPIO_SELECT			0x4D

#define DEMOD_10048_REG_IC_MODE				0x4E

#define DEMOD_10048_REG_CONF_XO				0x50
#define DEMOD_10048_REG_CONF_PLL1			0x51
#define DEMOD_10048_REG_CONF_PLL2 			0x52
#define DEMOD_10048_REG_CONF_PLL3			0x53

#define DEMOD_10048_REG_CONF_ADC				0x54
#define DEMOD_10048_REG_CONF_ADC2			0x55
#define DEMOD_10048_REG_CONF_C1_1			0x60
#define DEMOD_10048_REG_CONF_C1_3			0x62

#define DEMOD_10048_REG_AGC_CONF 			0x70
#define DEMOD_10048_REG_AGC_THRESHOLD 		0x72
#define DEMOD_10048_REG_AGC_THRESHOLD_MSB 	0x73
#define DEMOD_10048_REG_AGC_RENORM 			0x74
#define DEMOD_10048_REG_AGC_GAINS 			0x76
#define DEMOD_10048_REG_AGC_TUN_MIN 			0x78
#define DEMOD_10048_REG_AGC_TUN_MAX 		0x79
#define DEMOD_10048_REG_AGC_IF_MIN 			0x7A
#define DEMOD_10048_REG_AGC_IF_MAX 			0x7B
#define DEMOD_10048_REG_AGC_TUN_LEVEL		0x7E
#define DEMOD_10048_REG_AGC_IF_LEVEL			0x7F
#define DEMOD_10048_REG_DIG_AGC_LEVEL		0x81

#define DEMOD_10048_REG_FREQ_PHY2_MSB		0x87
#define DEMOD_10048_REG_FREQ_PHY2_LSB		0x86

#define DEMOD_10048_REG_TIME_INVWREF_LSB	0x88
#define DEMOD_10048_REG_TIME_INVWREF_MSB	0x89
#define DEMOD_10048_REG_TIME_WREF_LSB		0x8A
#define DEMOD_10048_REG_TIME_WREF_MID1		0x8B
#define DEMOD_10048_REG_TIME_WREF_MID2		0x8C
#define DEMOD_10048_REG_TIME_WREF_MSB		0x8D

#define DEMOD_10048_REG_NP_OUT				0xA2
#define DEMOD_10048_REG_CELL_ID_LSB			0xA4
#define DEMOD_10048_REG_CELL_ID_MSB 			0xA5
#define DEMOD_10048_REG_EXTTPS_ODD			0xAA
#define DEMOD_10048_REG_EXTTPS_EVEN			0xAB
#define DEMOD_10048_REG_TPS_LENGTH			0xAC

#define DEMOD_10048_FREE_REG_1				0xB2
#define DEMOD_10048_FREE_REG_2				0xB3

#define DEMOD_10048_REG_CONF_C3_1			0xC0

#define DEMOD_10048_REG_CVBER_CTL			0xC2
#define DEMOD_10048_REG_CBER_NMAX_LSB		0xC4
#define DEMOD_10048_REG_CBER_NMAX_MSB		0xC5
#define DEMOD_10048_REG_CBER_LSB				0xC6
#define DEMOD_10048_REG_CBER_MSB				0xC7
#define DEMOD_10048_REG_VBER_LSB				0xC8
#define DEMOD_10048_REG_VBER_MID				0xC9
#define DEMOD_10048_REG_VBER_MSB				0xCA
#define DEMOD_10048_REG_CVBER_LUT			0xCC
#define DEMOD_10048_REG_UNCOR_CTRL			0xCD
#define DEMOD_10048_REG_UNCOR_CPT_LSB		0xCE
#define DEMOD_10048_REG_UNCOR_CPT_MSB		0xCF

#define DEMOD_10048_REG_SOFT_IT_C3			0xD6
#define DEMOD_10048_REG_CONF_TS2				0xE0
#define DEMOD_10048_REG_CONF_TS1				0xE1


/*=============================================================================
  DEFINE BITS  
=============================================================================*/
#define TDA10048_OUTGI_BIT              0x04
#define TDA10048_CRYST_100PPM_BIT       0x02
#define TDA10048_AUTO_STARTLOCK_BIT     0x40
#define TDA10048_AUTO_AUTOOFFSET_BIT    0x10
#define TDA10048_AUTO_HPLP_BIT          0x08
#define TDA10048_AUTO_AUTO2K8K_BIT      0x04
#define TDA10048_AUTO_AUTOGI_BIT        0x02
#define TDA10048_AUTO_AUTOTPS_BIT       0x01
#define TDA10048_CONFC4_DSPWD_BIT       0x20
#define TDA10048_CONFC4_BPI2CTUNEEP_BIT 0x02
#define TDA10048_CONFC4_ICSTDBY_BIT     0x01
#define TDA10048_CONFC4_BOOT_EEPROM_BIT 0x04
#define TDA10048_CONFC4_BOOT_HOST_BIT   0x00
#define TDA10048_CONFC4_START_BOOT_BIT  0x08
#define TDA10048_CONFC1_USEPK_BIT       0x80
#define TDA10048_CONFC1_INVQ_BIT        0x20
#define TDA10048_INCONF1_CNF2K4K8K_BIT  0x03
#define TDA10048_OUTCONF1_CNF2K4K8KF_BIT 0x03
#define TDA10048_SOFT_IT_C3_ITVBER_BIT  0x02
#define TDA10048_SOFT_IT_C3_ITCBER_BIT  0x01
#define TDA10048_AGCCONF_USEAGCTUN_BIT  0x20
#define TDA10048_AGCCONF_AUTOAGC_BIT    0x10
#define TDA10048_AGCCONF_INIT_BIT       0x06
#define TDA10048_AGCCONF_AGCCONF2_BIT   0x04
#define TDA10048_CONFPOLA_AGCTUN_BIT    0x02
#define TDA10048_CONFPOLA_AGCIF_BIT     0x01
#define TDA10048_CONFPOLA_GPIO0_BIT     0x01
#define TDA10048_CONFTS1_DEGIG_BIT      0x80
#define TDA10048_CONFTS2_SLSB_BIT       0x20
#define TDA10048_CONFTS2_SBUS_BIT       0x10
#define TDA10048_CONFTS2_POCLK_BIT      0x01
#define TDA10048_CONFTS2_PSYNC_LENGTH_BIT 0x40
#define TDA10048_SYNCSTATUS_FEL_BIT     0x10
#define TDA10048_SYNCSTATUS_DSPREADY_BIT 0x40
#define TDA10048_TRISTATE1_ZTS_BIT      0x40
#define TDA10048_GPIOSELECT_GPIOIN0_BIT 0x08
#define TDA10048_CONFADC_ADCPWDN_BIT    0x40
#define TDA10048_UNCORCPT_CLBUNC_BIT    0x80
#define TDA10048_INCONF1_CNF2K_BIT      0x00
#define TDA10048_INCONF1_CNF4K_BIT      0x02
#define TDA10048_INCONF1_CNF8K_BIT      0x01
#define TDA10048_CONFC4_START_BOOT_HOST_BIT 0x08
#define TDA10048_UNCOR_PER_BIT          0x01
#define TDA10048_CLCBER_BIT             0x01
#define TDA10048_CLVBER_BIT             0x02
#define TDA10048_CONF_PLL1_AUTOPLL_BIT  0x08
#define TDA10048_CONFTR1_CLRI2C_BIT     0x01
#define TDA10048_ZAP_SCAN_BIT           0x20


#endif	/* DDI_CHANNEL_10048_H */

/* EOF */


