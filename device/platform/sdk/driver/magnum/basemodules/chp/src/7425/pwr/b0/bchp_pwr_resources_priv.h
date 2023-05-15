/***************************************************************
*
* This file is auto-generated by generate_chp_pwr.pl, based on
* bchp_pwr_resources.txt.
*
* This file contains a list of private power resource IDs that
* represent HW clocks, and function prototypes for controlling
* them.
*
***************************************************************/

#ifndef BCHP_PWR_RESOURCES_PRIV_H__
#define BCHP_PWR_RESOURCES_PRIV_H__

#include "bchp_pwr.h"

/* Private power resource IDs */
#define BCHP_PWR_HW_SVD0_CLK                0xff000001
#define BCHP_PWR_HW_SVD0_PWR                0xff000002
#define BCHP_PWR_HW_AVD1_CLK                0xff000003
#define BCHP_PWR_HW_AVD1_PWR                0xff000004
#define BCHP_PWR_HW_VICE0_CLK               0xff000005
#define BCHP_PWR_HW_VICE0_PWR               0xff000006
#define BCHP_PWR_HW_VEC_AIO                 0xff000007
#define BCHP_PWR_HW_RAAGA                   0xff000008
#define BCHP_PWR_HW_AIO_SRAM                0xff000009
#define BCHP_PWR_HW_AUD_PLL0                0xff00000a
#define BCHP_PWR_HW_AUD_PLL1                0xff00000b
#define BCHP_PWR_HW_AUD_PLL2                0xff00000c
#define BCHP_PWR_HW_RAAGA_SRAM              0xff00000d
#define BCHP_PWR_HW_HDMI_RX0_ALT_216M       0xff00000e
#define BCHP_PWR_HW_HDMI_TX_CLK             0xff00000f
#define BCHP_PWR_HW_BVN                     0xff000010
#define BCHP_PWR_HW_BVN_108M                0xff000011
#define BCHP_PWR_HW_BVN_MCVP_108M           0xff000012
#define BCHP_PWR_HW_BVN_SRAM                0xff000013
#define BCHP_PWR_HW_VDC_DAC                 0xff000014
#define BCHP_PWR_HW_VEC_SRAM                0xff000015
#define BCHP_PWR_HW_VEC_AIO_324M            0xff000016
#define BCHP_PWR_HW_VDC_656_OUT             0xff000017
#define BCHP_PWR_HW_XPT_108M                0xff000018
#define BCHP_PWR_HW_XPT_XMEMIF              0xff000019
#define BCHP_PWR_HW_XPT_RMX                 0xff00001a
#define BCHP_PWR_HW_XPT_SRAM                0xff00001b
#define BCHP_PWR_HW_XPT_WAKEUP              0xff00001c
#define BCHP_PWR_HW_HDMI_TX_SRAM            0xff00001d
#define BCHP_PWR_HW_HDMI_TX_108M            0xff00001e
#define BCHP_PWR_HW_HDMI_TX_CEC             0xff00001f
#define BCHP_PWR_HW_HDMI_RX0_216M           0xff000020
#define BCHP_PWR_HW_HDMI_RX0_FE             0xff000021
#define BCHP_PWR_HW_HDMI_RX0_SRAM           0xff000022
#define BCHP_PWR_HW_HDMI_RX0_CEC_HOTPLUG    0xff000023
#define BCHP_PWR_HW_M2MC                    0xff000024
#define BCHP_PWR_HW_M2MC_SRAM               0xff000025
#define BCHP_PWR_HW_V3D                     0xff000026
#define BCHP_PWR_HW_V3D_SRAM                0xff000027
#define BCHP_PWR_HW_GFX_108M                0xff000028
#define BCHP_PWR_HW_DMA                     0xff000029
#define BCHP_PWR_HW_SCD0                    0xff00002a
#define BCHP_PWR_HW_PLL_SCD_CH0             0xff00002b
#define BCHP_PWR_HW_SCD1                    0xff00002c
#define BCHP_PWR_HW_PLL_SCD_CH1             0xff00002d
#define BCHP_PWR_HW_MDM                     0xff00002e
#define BCHP_PWR_HW_UHF_INPUT               0xff00002f
#define BCHP_PWR_HW_PLL_AVD1_CH2            0xff000030
#define BCHP_PWR_HW_PLL_AVD1_CH3            0xff000031
#define BCHP_PWR_HW_PLL_AVD1_CH0            0xff000032
#define BCHP_PWR_HW_PLL_AVD1_CH1            0xff000033
#define BCHP_PWR_HW_PLL_AVD                 0xff000034
#define BCHP_PWR_HW_PLL_VCXO_PLL0_CH0       0xff000035
#define BCHP_PWR_HW_PLL_SCD                 0xff000036
#define BCHP_PWR_HW_PLL_VCXO_PLL0_CH2       0xff000037
#define BCHP_PWR_HW_PLL_VCXO_PLL0           0xff000038
#define BCHP_PWR_HW_PLL_VCXO_PLL1_CH0       0xff000039
#define BCHP_PWR_HW_PLL_VCXO_PLL1_CH2       0xff00003a
#define BCHP_PWR_HW_PLL_VCXO_PLL1           0xff00003b
#define BCHP_PWR_HW_PLL_VCXO_PLL2_CH0       0xff00003c
#define BCHP_PWR_HW_PLL_VCXO_PLL2_CH2       0xff00003d
#define BCHP_PWR_HW_PLL_VCXO_PLL2           0xff00003e
#define BCHP_PWR_HW_PLL_RAAGA_PLL_CH0       0xff00003f
#define BCHP_PWR_HW_PLL_RAAGA_PLL_CH1       0xff000040
#define BCHP_PWR_HW_PLL_RAAGA               0xff000041

/* Protoypes */
void BCHP_PWR_P_HW_SVD0_CLK_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_SVD0_PWR_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_AVD1_CLK_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_AVD1_PWR_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VICE0_CLK_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VICE0_PWR_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VEC_AIO_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_RAAGA_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_AIO_SRAM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_AUD_PLL0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_AUD_PLL1_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_AUD_PLL2_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_RAAGA_SRAM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_RX0_ALT_216M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_TX_CLK_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_BVN_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_BVN_108M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_BVN_MCVP_108M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_BVN_SRAM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VDC_DAC_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VEC_SRAM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VEC_AIO_324M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VDC_656_OUT_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_XPT_108M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_XPT_XMEMIF_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_XPT_RMX_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_XPT_SRAM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_XPT_WAKEUP_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_TX_SRAM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_TX_108M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_TX_CEC_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_RX0_216M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_RX0_FE_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_RX0_SRAM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_RX0_CEC_HOTPLUG_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_M2MC_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_M2MC_SRAM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_V3D_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_V3D_SRAM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_GFX_108M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_DMA_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_SCD0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_SCD_CH0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_SCD1_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_SCD_CH1_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_MDM_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_UHF_INPUT_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_AVD1_CH2_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_AVD1_CH3_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_AVD1_CH0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_AVD1_CH1_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_AVD_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_VCXO_PLL0_CH0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_SCD_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_VCXO_PLL0_CH2_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_VCXO_PLL0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_VCXO_PLL1_CH0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_VCXO_PLL1_CH2_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_VCXO_PLL1_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_VCXO_PLL2_CH0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_VCXO_PLL2_CH2_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_VCXO_PLL2_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_RAAGA_PLL_CH0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_RAAGA_PLL_CH1_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_RAAGA_Control(BCHP_Handle handle, bool activate);

/* This is the link between the public and private interface */
void BCHP_PWR_P_HW_Control(BCHP_Handle handle, const BCHP_PWR_P_Resource *resource, bool activate);

#define BCHP_PWR_P_NUM_NONLEAFS   56
#define BCHP_PWR_P_NUM_NONLEAFSHW 29
#define BCHP_PWR_P_NUM_LEAFS      36
#define BCHP_PWR_P_NUM_ALLNODES   121

#endif