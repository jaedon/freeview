/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_power.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 1/18/12 4:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_power.h $
 * 
 * 7   1/18/12 4:12p jtna
 * SW7425-2213: add deep sleep standby to brutus
 * 
 * 6   4/6/11 2:30p gmohile
 * SW7420-1792 : Add power standby support
 * 
 * 5   10/1/10 11:27a gskerl
 * SW7468-102: Added mpu (multi-processor) field to the brutus_power_state
 * structure
 * 
 * 4   6/25/10 5:31p gskerl
 * SW7125-403: Merged Active Standby changes back to main branch.
 * 
 * BRUTUS_97125_ACTIVE_STANDBY/2   6/25/10 3:46p gskerl
 * SW7125-403: Renamed brutus_standby to brutus_passive_standby
 * 
 * BRUTUS_97125_ACTIVE_STANDBY/1   5/29/10 3:35p gskerl
 * SW7125-403: Modified Brutus for testing Active Standby
 * 
 * 3   12/22/09 4:00p erickson
 * SW7420-70: add POWERSTANDBY_TIMEOUT option to brutus.cfg
 *
 * 2   7/2/09 5:09p erickson
 * PR56373: rework brutus_power to allow for deferred usb & sata power,
 * deferred mount, deferred videos scan
 *
 * 1   6/26/09 2:08p erickson
 * PR56373: rework brutus passive standby to not exit the app
 *
 ***************************************************************************/
#ifndef BRUTUS_POWER_H__
#define BRUTUS_POWER_H__

#include "bstd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Set initial power state to start brutus the first time

Description:
This function is not called when coming out of standby
**/
int brutus_init_power_state(void);

/**
Summary:
Clean shutdown of brutus

Description:
This function is not called when going into standby
**/
void brutus_uninit_power_state(void);

/**
Summary:
Brutus power state encompasses OS and Magnum managed power.

Description:
For each setting, true means powered up, false means powered down.
**/
typedef struct brutus_power_state {
    bool usb;
    bool ethernet;
    bool moca;
    bool sata;
    bool tp1;
    bool cpu;
    bool ddr;
} brutus_power_state;

/**
Summary:
Get the current state
**/
void brutus_get_power_state(brutus_power_state *state);

/**
Summary:
Power up or down cores

Description:
The user is responsible for powering up/down cores in a way that works with the application.
For example:
- before you umount (mounts=false), you should close all files on that mount
- before you power down sata, you should umount
- don't power down ethernet if your app if running across an nfs mount
- don't power down sata if you have a sata-based root fs
**/
int brutus_set_power_state(const brutus_power_state *state);

/**
Summary:
Bring CPU to minimum state and wait for user input
**/
int brutus_passive_standby(
    unsigned timeout /* seconds. 0 means no timeout. */
    );

int brutus_active_standby(
    unsigned timeout /* seconds. 0 means no timeout. */
    );

int brutus_deep_sleep(
    unsigned timeout /* seconds. 0 means no timeout. */
    );

int brutus_resume(void);

/**
Summary:
Print current power status per core to the console
**/
void brutus_print_power_status();

#ifdef __cplusplus
}
#endif

#endif
