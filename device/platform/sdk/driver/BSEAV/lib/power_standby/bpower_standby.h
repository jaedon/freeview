/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpower_standby.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 8/9/10 3:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/power_standby/bpower_standby.h $
 * 
 * 6   8/9/10 3:26p dlwin
 * SW3548-3032: Merged CEC wakeup implementation from customer branch
 * code.
 * 
 * 5   5/1/09 10:16a maivu
 * PR 54724: NEXUS_Platform_Uninit will now uninit bkni and bdbg
 * 
 * 4   4/30/09 1:40p maivu
 * PR 54724: Added comments for bpower_standby_create
 * 
 * 3   2/11/09 2:00p maivu
 * PR 51679: Add PC/VGA wakeup support
 * 
 * 2   2/11/09 10:39a maivu
 * PR 51963: Sync up with latest name changes in pmdriver. Added support
 * for generic GPIO wakeup
 * 
 * 1   10/29/08 3:59p maivu
 * PR 48036: power standby library, based off power_standby app in
 * rockford. Initial support for 3548/3556. Tested and confirmed working
 * on 93549B0
 * 
 ******************************************************************/
#ifndef BPOWER_STANDBY_H__
#define BPOWER_STANDBY_H__

#include "bstd.h"
#include "bchp_common.h"
#include "breg_mem.h"
#include "bkni.h"
#if BCHP_CHIP == 7400
#include "bchp_memc_0.h"
#include "bchp_memc_1.h"
#include "bchp_memc_2.h"
#include "bchp_memc_0_ddr.h"
#include "bchp_memc_1_ddr.h"
#include "bchp_memc_2_ddr.h"
#include "bchp_px3d.h"
#elif BCHP_CHIP == 7405
#include "bchp_memc_0_1.h"
#include "bchp_memc_1_1.h"
#include "bchp_memc_0_ddr.h"
#include "bchp_memc_1_ddr.h"
#elif BCHP_CHIP == 7325
#include "bchp_memc_0_1.h"
#include "bchp_memc_1_1.h"
#include "bchp_memc_0_ddr.h"
#include "bchp_memc_1_ddr.h"
#elif BCHP_CHIP == 7335
#include "bchp_memc_0_1.h"
#include "bchp_memc_1_1.h"
#include "bchp_memc_0_ddr.h"
#include "bchp_memc_1_ddr.h"
#elif (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
#include "bchp_memc_0.h"
#include "bchp_memc_0_1.h"
#include "bchp_memc_0_ddr.h"
#include "bchp_px3d.h"
#else
#error "Unsupported chip!"
#endif

#include "bpwr.h"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include "pmlib.h"
#include "brcmpmdrv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BPOWER_STANDBY_SUCCESS         0
#define BPOWER_STANDBY_FAILURE         1

/**
Summary:
Settings for power standby
**/
typedef struct bpower_standby_settings{
	bool bPassive;		/* true=passive standby, false=active standby */
    bool bWake_on_CEC; /* true= wakeup on CEC activity */
	int hours;         /* timeout hours */
	int minutes;       /* timeout minutes */
	int seconds;       /* timeout seconds */
} bpower_standby_settings;

typedef struct bpower_standby *bpower_standby_t;

/**
Summary:
Wakeup from standby event
**/
typedef enum bpower_standby_wakeup {
	bpower_standby_wakeup_eIR=0,     /* IR remote power button */
	bpower_standby_wakeup_eKeypad,   /* keypad power button */
	bpower_standby_wakeup_eGpio,     /* GPIO activity */
	bpower_standby_wakeup_eTimeout,  /* timeout expires */
	bpower_standby_wakeup_eVGA,      /* PC/VGA activity */
	bpower_standby_wakeup_eCEC,      /* CEC activity */
	bpower_standby_wakeup_eUnknown   /* unknown event */
} bpower_standby_wakeup;

/**
Summary:
 Wakeup info
**/
typedef struct bpower_standby_wakeup_settings{
	bpower_standby_wakeup eWakeupEvent;  /* Wakeup event */
	unsigned int          gpioExtIrq;    /* GPIO trigger (ext_irq_b_#) */
	unsigned int          cec_addr;      /* cec wake up address */
} bpower_standby_wakeup_settings;

typedef struct bpower_standby{
	unsigned long flags;
	unsigned long events;
	unsigned long ext_irq_mask;   /* CG_INTR2_CPU_STATUS */

	BPWR_Handle hPwr;
    BREG_Handle hReg;
    BCHP_Handle hChp;
	bpower_standby_settings settings;
	struct brcm_pm_state state;
	void *brcm_pm_ctx;
	int ddr_timeout; 
	BSTD_Module *activeModuleList;
	BSTD_Module *passiveModuleList;
	uint32_t activeModuleCnt;
	uint32_t passiveModuleCnt;
	bpower_standby_wakeup_settings wakeup;
} bpower_standby;

/*
Summary:
Returns default settings
*/
void bpower_standby_get_default_settings( 
	bpower_standby_settings *settings		/* [out] default power standby settings */
	);

/*
Summary:
Create a power standby instance

Note, BKNI_Init must be executed before calling this function

Returns:
	bpower_standby_t - error if null
*/
bpower_standby_t bpower_standby_create(
	const bpower_standby_settings *power_settings    /* [in] power standby settings */
	);

/*
Summary:
Destroy a power standby instance
*/
void bpower_standby_destroy(
	bpower_standby_t power_standby        /* [in] power standby handle */
	);

/*
Summary:
Enter power stand-by mode

Returns:
	BPOWER_STANDBY_SUCCESS
	BPOWER_STANDBY_FAILURE
*/
int bpower_standby_shutdown(
	bpower_standby_t power_standby      /* [in] power standby handle */
	);

/*
Summary:
Wakeup from power stand-by mode

Returns:
	BPOWER_STANDBY_SUCCESS
	BPOWER_STANDBY_FAILURE
	wakeup - wakeup info returned
*/
int bpower_standby_resume(
	bpower_standby_t power_standby,	      /* [in] power standby handle */
	bpower_standby_wakeup_settings *wakeup   /* [out] standby wakeup info */
	);


#ifdef __cplusplus
}
#endif

#endif /* BPOWER_STANDBY_H__ */

