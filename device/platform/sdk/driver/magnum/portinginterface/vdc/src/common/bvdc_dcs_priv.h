/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_dcs_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 9/4/12 2:10p $
 *
 * Module Description:
 *   Header file for DCS support
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_dcs_priv.h $
 * 
 * Hydra_Software_Devel/6   9/4/12 2:10p darnstein
 * SW7125-1124: DCS only: use microcode instruction register 252 for
 * confirmation of VEC updates.
 * 
 * Hydra_Software_Devel/5   7/17/12 7:05p darnstein
 * SW7425-3474: DCS: try much harder to guarantee that previous RUL update
 * was successful.
 * 
 * Hydra_Software_Devel/4   1/19/12 11:32a darnstein
 * SW7125-1124: DCS: control VEC hardware with fewer register writes. Per
 * Nutan and Ali Syed.
 * 
 * Hydra_Software_Devel/3   1/12/12 12:38p darnstein
 * SW7125-1124: Implement two-stage transition between DCS ON and DCS OFF.
 * This works around a hardware issue.
 * 
 * Hydra_Software_Devel/2   1/11/12 3:06p darnstein
 * SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
 * production.
 * 
 * Hydra_Software_Devel/SW7125-1124/13   1/6/12 1:45p darnstein
 * SW7125-1124: MC_0 and MC_1 start addresses now stay the same for DCS on
 * and off. MC_3 start address also stays the same. MC_3 is enabled and
 * disabled by writing to TG_CONFIG.MC_ENABLES.
 * 
 * Hydra_Software_Devel/SW7125-1124/12   1/4/12 5:39p darnstein
 * SW7125-1124: Implement three new microcode arrays for DCS 480I.
 * 
 * Hydra_Software_Devel/SW7125-1124/11   1/4/12 4:35p darnstein
 * SW7125-1124: implement experimental toggle of MC_ENABLES. This improves
 * access to buffered VEC registers.
 * 
 * Hydra_Software_Devel/SW7125-1124/10   12/28/11 2:40p darnstein
 * SW7125-1124: When turning DCS on and off, change IT_CONFIG.MC_ENABLES
 * to enable only those microcontrollers that are needed. This is an
 * experiment, and may be backed out later.
 * 
 * Hydra_Software_Devel/SW7125-1124/9   12/22/11 2:20p darnstein
 * SW7125-1124: DCS: fix incorrect IT MC start addresses for 576I, DCS
 * OFF.
 * 
 * Hydra_Software_Devel/SW7125-1124/8   12/21/11 7:32p darnstein
 * SW7125-1124: Fix bug in debugging scratch registers. Implement Nutan's
 * suggestion for DCS access control. Integrate all new microcode just
 * posted today.
 * 
 * Hydra_Software_Devel/SW7125-1124/7   12/20/11 6:47p darnstein
 * SW7125-1124: DCS: Simplify logic a bit. Add diagnostics.
 * 
 * Hydra_Software_Devel/SW7125-1124/6   12/19/11 4:55p darnstein
 * SW7125-1124: simplify DCS access method. A few bug fixes too.
 * 
 * Hydra_Software_Devel/SW7125-1124/5   12/12/11 7:46p darnstein
 * SW7125-1124: DCS Hybrid+ software ready for debugging.
 * 
 * Hydra_Software_Devel/SW7125-1124/4   12/9/11 9:44p darnstein
 * SW7125-1124: need to add a dirty bit. Checking in DCS work first.
 * 
 * Hydra_Software_Devel/SW7125-1124/3   12/8/11 2:07p darnstein
 * SW7125-1124: simplifications made possible by Nutan's new microcode
 * address scheme.
 * 
 * Hydra_Software_Devel/SW7125-1124/2   12/6/11 8:15p darnstein
 * SW7125-1124: Finish DCS code swapping logic.
 * 
 * Hydra_Software_Devel/SW7125-1124/1   11/15/11 7:05p darnstein
 * SW7125-1124: simplify somewhat.
 * 
 ***************************************************************************/

#ifndef BVDC_DCS_PRIV_H__
#define BVDC_DCS_PRIV_H__

#include "bvdc_dcs.h"
#include "bvdc_display_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BVDC_P_SMOOTH_DCS

/* These values refer to microcontroller programs in register array
 * IT_n_MICRO_INSTRUCTION */
#define BVDC_P_DCS_MC0_ON (0)
#define BVDC_P_DCS_MC0_OFF (0) /* NTSC ONLY */
#define BVDC_P_DCS_MC1_ON (20)
#define BVDC_P_DCS_MC1_OFF (20) /* NTSC ONLY */
#define BVDC_P_DCS_MC2 (90)
#define BVDC_P_DCS_MC3 (160)
#define BVDC_P_DCS_MC6 (170)
#define BVDC_P_DCS_SELFLOOP (89)
#define BVDC_P_DCS_SEGSTART BVDC_P_DCS_MC2
#define BVDC_P_DCS_SEGLEN (256 - BVDC_P_DCS_SEGSTART)
#define BVDC_P_DCS_CONFIRMER (252)

#define BVDC_P_DCS_ON_ADDRA_NTSC                                        \
(                                                                       \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_3_START_ADDR, BVDC_P_DCS_MC3   ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_2_START_ADDR, BVDC_P_DCS_MC2   ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_1_START_ADDR, BVDC_P_DCS_MC1_ON) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_0_START_ADDR, BVDC_P_DCS_MC0_ON)   \
)
#define BVDC_P_DCS_ON_ADDRA_PAL BVDC_P_DCS_ON_ADDRA_NTSC

#define BVDC_P_DCS_ON_ADDRB_NTSC                                          \
(                                                                         \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_6_START_ADDR, BVDC_P_DCS_MC6     ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_5_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_4_START_ADDR, BVDC_P_DCS_SELFLOOP)   \
)
#define BVDC_P_DCS_ON_ADDRB_PAL BVDC_P_DCS_ON_ADDRB_NTSC

#define BVDC_P_DCS_OFF_ADDRA_NTSC                                         \
(                                                                         \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_3_START_ADDR, BVDC_P_DCS_MC3     ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_2_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_1_START_ADDR, BVDC_P_DCS_MC1_OFF ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_0_START_ADDR, BVDC_P_DCS_MC0_OFF )   \
)
/* Programming note: there is no MC_0 DCS_OFF microcode for PAL. */
#define BVDC_P_DCS_OFF_ADDRA_PAL                                          \
(                                                                         \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_3_START_ADDR, BVDC_P_DCS_MC3     ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_2_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_1_START_ADDR, BVDC_P_DCS_MC1_ON  ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_0_START_ADDR, BVDC_P_DCS_MC0_ON  )   \
)

#define BVDC_P_DCS_OFF_ADDRB_NTSC                                         \
(                                                                         \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_6_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_5_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_4_START_ADDR, BVDC_P_DCS_SELFLOOP)   \
)
#define BVDC_P_DCS_OFF_ADDRB_PAL BVDC_P_DCS_OFF_ADDRB_NTSC

#define BVDC_P_DCS_ON_ADDRA_PROGRSV                                       \
(                                                                         \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_3_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_2_START_ADDR, BVDC_P_DCS_MC2     ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_1_START_ADDR, BVDC_P_DCS_MC1_ON  ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_0_START_ADDR, BVDC_P_DCS_MC0_ON  )   \
)

#define BVDC_P_DCS_ON_ADDRB_PROGRSV                                       \
(                                                                         \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_6_START_ADDR, BVDC_P_DCS_MC6     ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_5_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_4_START_ADDR, BVDC_P_DCS_SELFLOOP)   \
)

#define BVDC_P_DCS_OFF_ADDRA_PROGRSV                                      \
(                                                                         \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_3_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_2_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_1_START_ADDR, BVDC_P_DCS_MC1_ON  ) | \
	BCHP_FIELD_DATA(IT_0_ADDR_0_3,MC_0_START_ADDR, BVDC_P_DCS_MC0_ON  )   \
)

#define BVDC_P_DCS_OFF_ADDRB_PROGRSV                                      \
(                                                                         \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_6_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_5_START_ADDR, BVDC_P_DCS_SELFLOOP) | \
	BCHP_FIELD_DATA(IT_0_ADDR_4_6,MC_4_START_ADDR, BVDC_P_DCS_SELFLOOP)   \
)

#define BVDC_P_DCS_PSV_OFFSET \
	((BCHP_VF_0_POS_SYNC_VALUES - BCHP_VF_0_FORMAT_ADDER)/4)
#define BVDC_P_DCS_PSE_OFFSET \
	((BCHP_VF_0_POS_SYNC_AMPLITUDE_EXTN - BCHP_VF_0_FORMAT_ADDER)/4)
#define BVDC_P_DCS_NSE_OFFSET \
	((BCHP_VF_0_NEG_SYNC_AMPLITUDE_EXTN - BCHP_VF_0_FORMAT_ADDER)/4)
#define BVDC_P_DCS_ST0_OFFSET \
	((BCHP_VF_0_SYNC_TRANS_0 - BCHP_VF_0_FORMAT_ADDER)/4)
typedef struct
{
	uint32_t PosSyncValues;
	uint32_t PosSyncExtn;
	uint32_t NegSyncExtn;
	uint32_t SyncTrans0;
}
BVDC_P_DCS_VFvalues;

const BVDC_P_DCS_VFvalues* BVDC_P_DCS_GetVFvalues_isr
(
	BFMT_VideoFmt eVideoFmt,
	bool bCompositePresent,
	BVDC_DCS_Mode eDcsMode
);
void BVDC_P_DCS_VF_Update_isr(
	const BVDC_P_DCS_VFvalues* pVfValues, 
	uint32_t                   ulVfOffset, 
	uint32_t**                 ppulRul);

typedef enum
{
	BVDC_P_DCS_State_eUndefined = 0,    /* No DCS for current video format */
	BVDC_P_DCS_State_eExecuteTop,       /* Executing top half              */
	BVDC_P_DCS_State_eExecuteBot,       /* Executing bottom half           */
	BVDC_P_DCS_State_eGoingTop,         /* Transitioning to top half       */
	BVDC_P_DCS_State_eGoingBot          /* Transitioning to bottom half    */

} BVDC_P_DCS_State;

struct BVDC_P_DCS_UpdateInfo
{
	BVDC_DCS_Mode    eDcsModeLoaded;
	BVDC_P_DCS_State eDcsState;
	bool             bDcsSetLock;
};

void BVDC_P_DCS_StateInit_isr (BVDC_Display_Handle pDisplay);

void BVDC_P_DCS_StateFault_isr (
	BVDC_Display_Handle pDisplay);

void BVDC_P_DCS_StateUpdate_isr ( 
	BVDC_Display_Handle pDisplay,
	BAVC_Polarity       eFieldPolarity,
	BVDC_P_ListInfo*    pList
);

void BVDC_P_DCS_Check_isr ( 
	BVDC_Display_Handle pDisplay,
	BAVC_Polarity       eFieldPolarity,
	int                 index,
	BVDC_P_ListInfo*    pList
);

const uint32_t* BVDC_P_DCS_P_GetItTable_isr
(
	BFMT_VideoFmt eVideoFmt,
	bool bCompositePresent,
	BVDC_DCS_Mode eDcsMode
);
const uint32_t* BVDC_P_DCS_P_GetRamTableMv_isr
(
	BFMT_VideoFmt eVideoFmt,
	BVDC_DCS_Mode eDcsMode
);
uint32_t BVDC_P_DCS_P_GetItConfigMv_isr
(
	BFMT_VideoFmt eVideoFmt,
	bool          bDcsOn
);
void BVDC_P_DCS_IT_ON_OFF_isr(
	uint32_t      ulItOffset, 
	BFMT_VideoFmt eVideoFmt,
	bool          bDcsOn,
	uint32_t** ppulRul);
void BVDC_P_DCS_IT_Final_ON_OFF_isr(
	uint32_t      ulItOffset, 
	BFMT_VideoFmt eVideoFmt,
	bool          bDcsOn,
	uint32_t** ppulRul);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_DCS_PRIV_H__ */
/* End of file. */
