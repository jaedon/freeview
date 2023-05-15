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
#define BCHP_PWR_HW_AVD0            0xff000001
#define BCHP_PWR_HW_RAP_OPEN        0xff000002
#define BCHP_PWR_HW_RAP_START       0xff000003
#define BCHP_PWR_HW_BVN             0xff000004
#define BCHP_PWR_HW_BVN_108M        0xff000005
#define BCHP_PWR_HW_VDC_DAC         0xff000006
#define BCHP_PWR_HW_VDC_VEC         0xff000007
#define BCHP_PWR_HW_XPT_108M        0xff000008
#define BCHP_PWR_HW_XPT_XMEMIF      0xff000009
#define BCHP_PWR_HW_HDMI_TX_CLK     0xff00000a
#define BCHP_PWR_HW_HDMI_TX_108M    0xff00000b
#define BCHP_PWR_HW_HDMI_TX_CEC     0xff00000c
#define BCHP_PWR_HW_M2MC            0xff00000d
#define BCHP_PWR_HW_SCD             0xff00000e
#define BCHP_PWR_HW_PLL_AC1         0xff00000f

/* Protoypes */
void BCHP_PWR_P_HW_AVD0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_RAP_OPEN_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_RAP_START_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_BVN_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_BVN_108M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VDC_DAC_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VDC_VEC_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_XPT_108M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_XPT_XMEMIF_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_TX_CLK_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_TX_108M_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_HDMI_TX_CEC_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_M2MC_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_SCD_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_AC1_Control(BCHP_Handle handle, bool activate);

/* This is the link between the public and private interface */
void BCHP_PWR_P_HW_Control(BCHP_Handle handle, const BCHP_PWR_P_Resource *resource, bool activate);

#define BCHP_PWR_P_NUM_NONLEAFS   24
#define BCHP_PWR_P_NUM_NONLEAFSHW 3
#define BCHP_PWR_P_NUM_LEAFS      12
#define BCHP_PWR_P_NUM_ALLNODES   39

#endif