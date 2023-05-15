/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_power.c $
 * $brcm_Revision: 40 $
 * $brcm_Date: 3/12/12 4:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_power.c $
 * 
 * 40   3/12/12 4:30p jtna
 * SW7425-2604: set TP1 power during init
 * 
 * 39   2/23/12 6:13p gmohile
 * SW7425-2473 : Fix frontend initialization
 * 
 * 38   2/22/12 2:19p gmohile
 * SW7425-2213 : Re-init frontend after resume from S3
 * 
 * 37   1/18/12 4:12p jtna
 * SW7425-2213: add deep sleep standby to brutus
 * 
 * 36   1/18/12 3:29p jtna
 * SW7425-2213: include correct pmlib header. don't set cpu_divisor.
 * 
 * 35   4/6/11 2:30p gmohile
 * SW7420-1792 : Add power standby support
 * 
 * 34   10/21/10 5:45p gskerl
 * SW7125-666: Added code to run CM statup/shutdown scripts (if they
 * exist)
 * 
 * 33   10/20/10 2:05p gskerl
 * SW7125-237: Added brutus_wait_for_device() to wait for SATA device to
 * appear before trying to mount it
 * 
 * 32   10/1/10 4:22p gskerl
 * SW7468-102: Added mpu (multi-processor) field to the brutus_power_state
 * structure
 * 
 * 31   9/24/10 6:08p gskerl
 * SW7468-102: Added support for 7468
 * 
 * 30   9/23/10 7:34p gskerl
 * SW7468-102: Added unmount/remount of SD flash cards (/dev/mmcblk*
 * devices). Disabled SATA power status change (temporary)
 * 
 * BRUTUS_97468_IBC_PM_DEMO/1   8/26/10 2:27p gskerl
 * SW7468-102: Added unmount/remount of SD flash cards (/dev/mmcblk*
 * devices).
 * Also added some passive standby info messages.
 * Disabled SATA power status change (temporary)
 * 
 * 29   9/9/10 2:17p gskerl
 * SW7125-237: Fixed build problem... Made standby settings specific to
 * 7125
 * 
 * 28   9/8/10 6:35p gskerl
 * SW7125-237: Added (temporary) 15 second delay when exiting passive
 * standby to allow cable modem to boot up
 * 
 * 27   9/7/10 5:46p gskerl
 * SW7125-237: Added code to specify Passive Standby wakeup events
 * 
 * 26   9/2/10 4:15p gskerl
 * SW7125-237: Added code to demonstrate IR key filtering when in Passive
 * Standby. Also some refactoring
 * 
 * 25   8/30/10 4:09p gskerl
 * SW7125-237: Added comments
 * 
 * 24   8/24/10 6:01p gskerl
 * SW7125-237: Added some informational (warning) messages when going into
 * and out of passive standby
 * 
 * 23   8/24/10 2:35p gskerl
 * SW7125-237: Added code to shutdown the cable modem in Passive Standby
 * (for 7125)
 * 
 * 22   8/6/10 5:12p gskerl
 * SW7125-237: Change PAT receive timeout from 5 to 10 seconds
 * 
 * 21   7/22/10 5:09p gskerl
 * SW7125-237: Removed #if USE_NEXUS_WAKEUP_DEVICES so that those devices
 * are always enabled for wakeup (if they exist)
 * 
 * 20   7/19/10 5:56p tokushig
 * SW7420-699: Add IR remote to active standby in brutus.  Only tested on
 * 97125 platform.
 * 
 * 19   7/13/10 10:31a gskerl
 * SW7125-237: Removed erroneous calls to NEXUS_Platform_Pre/PostStandby()
 * and reduced iterations for the Active Standby app
 * 
 * 18   6/25/10 5:30p gskerl
 * SW7125-403: Merged Active Standby changes back to main branch.
 * 
 * BRUTUS_97125_ACTIVE_STANDBY/2   6/25/10 3:46p gskerl
 * SW7125-403: Renamed brutus_standby to brutus_passive_standby
 * 
 * BRUTUS_97125_ACTIVE_STANDBY/1   5/29/10 3:31p gskerl
 * SW7125-403: Modified Brutus for testing Active Standby
 * 
 * 17   4/30/10 11:54a erickson
 * SW7420-532: change to BKNI_Printf
 * 
 * 16   3/18/10 2:49p erickson
 * SW7420-532: add namespace to #define
 *
 * 15   1/28/10 4:38p erickson
 * SW7420-532: refactor nexus platform standby for linux 2.6.31
 *
 * SW7420-70/1   1/21/10 4:36p erickson
 * SW7420-70: refactor
 *
 * 14   12/22/09 4:00p erickson
 * SW7420-70: add POWERSTANDBY_TIMEOUT option to brutus.cfg
 *
 * 13   12/21/09 2:59p erickson
 * SW7420-70: allow some chips to enable POWERSTANDBY_SUPPORT without an
 * external power driver
 *
 * 12   11/13/09 5:06p erickson
 * SW7325-250: use pmlib for linux standby, use nexus for magnum-
 * controlled standby
 *
 * 11   7/29/09 7:00p mward
 * PR57145: On 7400, add a small delay before mount to allow drives to be
 * ready.
 *
 * 10   7/28/09 5:23p mward
 * PR57145: Update power management for 7400 as done for 7335, 7405.
 *
 * 9   7/15/09 12:48p katrep
 * PR55138: Should come out of sleep only on power key
 *
 * 8   7/15/09 11:37a erickson
 * PR56373: add linux kernel mode passive standby support
 *
 * 7   7/14/09 7:39p katrep
 * PR56518: Changed power sequence for BVN and UHF modules
 *
 * 6   7/10/09 4:41p erickson
 * PR56517: mknod should not fail if node already exists
 *
 * 5   7/2/09 5:09p erickson
 * PR56373: rework brutus_power to allow for deferred usb & sata power,
 * deferred mount, deferred videos scan
 *
 * 4   7/1/09 12:53p erickson
 * PR56373: brcm_standby should remount on success or ioctl failure.
 * cleaned up brackets for else statements.
 *
 * 3   7/1/09 11:49a erickson
 * PR56516: disable SATA power down. also removed unsupported ARC id.
 *
 * 2   6/26/09 3:38p erickson
 * PR56373: umount before powering down, mount after powering up
 *
 * 1   6/26/09 2:08p erickson
 * PR56373: rework brutus passive standby to not exit the app
 *
 ***************************************************************************/
#include "brutus_power.h"
#include "bstd.h"
#include "bkni.h"

#include <wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "pmlib-263x.h"
#include "nexus_platform_standby.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_ir_input.h"
#include "nexus_message.h"

#include "bstd.h"
#include "bkni.h"

#include <stdio.h>



BDBG_MODULE(brutus_power);

static int brutus_mount();
static int brutus_umount();

static struct {
    void *pmlibHandle;
    brutus_power_state state;
    int ddr_timeout; /* cached initial value */

#define MAX_MOUNTS 16
    struct {
        char device[256];
        char mountpoint[256];
    } mounts[MAX_MOUNTS];
    unsigned total_mounts;
} g_power_state;


int brutus_deep_sleep(unsigned timeout) 
{
    int rc;       
    NEXUS_PlatformStandbySettings nexusStandbySettings;    
    brutus_power_state power_state;

    BDBG_MSG(("Entering DEEP SLEEP mode"));

    /* put nexus into passive standby */
    NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
    nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eDeepSleep;
    nexusStandbySettings.wakeupSettings.ir = true;
    nexusStandbySettings.wakeupSettings.timeout = timeout;
    rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
    BDBG_ASSERT(!rc);

    brutus_umount();
  
    BDBG_MSG(("Suspending."));

    system("echo mem > /sys/power/state");
    
    BDBG_MSG(("Resuming."));

    brutus_mount();

    NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
    nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eOn;       
    rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
    BDBG_ASSERT(!rc);

    BDBG_MSG(("Exiting DEEP SLEEP mode"));

    return rc;
}


int brutus_passive_standby(unsigned timeout) 
{
    int rc;       
    NEXUS_PlatformStandbySettings nexusStandbySettings;    
    brutus_power_state power_state;

    BDBG_MSG(("Entering PASSIVE STANDBY mode"));

    /* put nexus into passive standby */
    NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
    nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_ePassive;
    nexusStandbySettings.wakeupSettings.ir = true;
    nexusStandbySettings.wakeupSettings.timeout = timeout;
    rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
    BDBG_ASSERT(!rc);

    brutus_umount();
  
    BDBG_MSG(("Suspending."));
    
    rc = brcm_pm_suspend(g_power_state.pmlibHandle, BRCM_PM_STANDBY);    
    if (rc) return BERR_TRACE(rc);
    
    BDBG_MSG(("Resuming."));

    brutus_mount();

    NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
    nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eOn;    
    rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
    BDBG_ASSERT(!rc);

    BDBG_MSG(("Exiting PASSIVE STANDBY mode"));

    return rc;
}


int brutus_active_standby(unsigned timeout) 
{
    int rc;
    NEXUS_PlatformStandbySettings nexusStandbySettings;    
    brutus_power_state power_state;
    
    BSTD_UNUSED(timeout);
    
    BDBG_MSG(("Entering ACTIVE STANDBY mode"));

    NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
    nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eActive;    
    rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
    BDBG_ASSERT(!rc);
    
#if BCHP_CHIP != 7408
    brutus_get_power_state(&power_state);
    power_state.cpu = false;
    power_state.ddr = false;
    rc = brutus_set_power_state(&power_state);
#endif   

    return rc;
}

int brutus_resume(void)
{
    int rc;
    NEXUS_PlatformStandbySettings nexusStandbySettings;    
    brutus_power_state power_state;

    brutus_get_power_state(&power_state);
    power_state.usb = true;
    power_state.sata = true;
    power_state.ethernet = true;
    power_state.cpu = true;
    power_state.tp1 = true;
    power_state.ddr = true;
    power_state.moca = true;
    if (brutus_set_power_state(&power_state)) {
	BDBG_ERR(("unable to power up\n"));
    }

    NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
    nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eOn;    
    rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
    BDBG_ASSERT(!rc);
} 

int brutus_init_power_state(void)
{
    BERR_Code rc;
    brutus_power_state local_state;

    BKNI_Memset(&g_power_state, 0, sizeof(g_power_state));

    g_power_state.pmlibHandle = brcm_pm_init();
    if (!g_power_state.pmlibHandle) {
        return BERR_TRACE(-1);
    }
    
    brutus_get_power_state(&local_state);
    local_state.usb = true;
    local_state.ethernet = true;
    local_state.moca = true;
    local_state.sata = true;
    local_state.tp1 = true;
    local_state.cpu = true;
    local_state.ddr = true;
    return brutus_set_power_state(&local_state);
}

void brutus_uninit_power_state(void)
{
    brcm_pm_close(g_power_state.pmlibHandle);
}

static int brutus_umount()
{
    FILE *f;

    g_power_state.total_mounts = 0;

    f = fopen("/proc/mounts", "r+");
    if (!f) return -1;
    while (!feof(f) && g_power_state.total_mounts < MAX_MOUNTS) {
        char buf[256];
        char *c1, *c2;
        if (fgets(buf, 256, f)) {
            if (strstr(buf, "/dev/sd") == buf || strstr(buf, " nfs ") || strstr(buf, "/dev/mmcblk") ) {
                c1 = strchr(buf, ' ');
                if (!c1) continue;

                *c1 = 0;
                strcpy(g_power_state.mounts[g_power_state.total_mounts].device, buf);
                c1++;
                c2 = strchr(c1, ' ');
                if (!c2) continue;

                *c2 = 0;
                strcpy(g_power_state.mounts[g_power_state.total_mounts].mountpoint, c1);

                printf("found mount point %d: %s %s\n", g_power_state.total_mounts, g_power_state.mounts[g_power_state.total_mounts].device, g_power_state.mounts[g_power_state.total_mounts].mountpoint);

                snprintf(buf, 256, "umount %s", g_power_state.mounts[g_power_state.total_mounts].mountpoint);
                if (!system(buf)) {
                    g_power_state.total_mounts++;
                }
            }
        }
    }
    fclose(f);
    BDBG_MSG(("unmounted %d mount points", g_power_state.total_mounts));
}


static int brutus_wait_for_device( char *device_path, unsigned long timeout_in_ms )
{
    int     rc = 0;

    if (index( device_path, ':') == NULL   ) {  /* If device has ":", then it's an NFS path... skip it. */
        unsigned long   ms;
        unsigned long   ms_interval = 200;
        unsigned long   ms_max = timeout_in_ms;
        int             rc;
        struct stat     statbuf;

        for (ms=0 ; ms<ms_max ; ms+=ms_interval) {
            rc = stat(device_path, &statbuf);
            if (rc == 0) break;
            if (ms == 0) BKNI_Printf("Waiting for device %s to exist...\n", device_path);
            BKNI_Sleep(ms_interval);
        }
        if (ms >= ms_max) {
            BKNI_Printf("Device %s not found after %lu ms\n", device_path, ms );
            return 1;   /* return error code */
        }
        else if (ms > 0) {
            BKNI_Printf("Device %s appeared after %lu ms\n", device_path, ms );
        }
    }

    return rc;
}


static int brutus_mount()
{
    unsigned i;

    BDBG_MSG(("mounting %d mount points", g_power_state.total_mounts));
    for (i=0;i<g_power_state.total_mounts;i++) {
        char buf[256];
        int rc;

        rc = brutus_wait_for_device( g_power_state.mounts[i].device, 10000 );
        if (rc ) {    /* Device isn't there or not ready... */
            BKNI_Printf("Can't mount device \n", g_power_state.mounts[i].device );
            continue;
        }

        snprintf(buf, 256, "mount %s %s", g_power_state.mounts[i].device, g_power_state.mounts[i].mountpoint);
        printf("%s\n", buf);
        system(buf);
    }
    g_power_state.total_mounts = 0;
}

void brutus_print_power_status()
{
    BKNI_Printf(
    "\n"
    "Brutus power status: "
    "USB=%s,"
    "SATA=%s,"
    "ENET=%s,"
    "CPU=%s,"
    "TPI=%s,"
    "DDR=%s,"
    "MOCA=%s\n",
        g_power_state.state.usb?"on":"off",
        g_power_state.state.sata?"on":"off",
        g_power_state.state.ethernet?"on":"off",
        g_power_state.state.cpu?"full":"reduced",
        g_power_state.state.tp1?"on":"off",
        g_power_state.state.ddr?"full":"reduced", 
        g_power_state.state.moca?"on":"off"
        );
}

void brutus_get_power_state(brutus_power_state *state)
{
    *state = g_power_state.state;
}

int brutus_set_power_state(const brutus_power_state *state)
{
    struct brcm_pm_state pmlib_state;
    BERR_Code rc;

    if (brcm_pm_get_status(g_power_state.pmlibHandle, &pmlib_state) != 0) {
        BDBG_WRN(("Can't get PM state\n"));        
    }

    if (g_power_state.ddr_timeout == 0) {
        g_power_state.ddr_timeout = pmlib_state.ddr_timeout;
    }    

    /* pmlib */
    if (pmlib_state.usb_status != BRCM_PM_UNDEF) {
        pmlib_state.usb_status = state->usb;
    }

    if (pmlib_state.sata_status != BRCM_PM_UNDEF) {
        pmlib_state.sata_status = state->sata;
    }

    if (pmlib_state.enet_status != BRCM_PM_UNDEF) {
        pmlib_state.enet_status = state->ethernet;
    }
    if (state->ddr || state->cpu) {
        #if 0
        pmlib_state.cpu_divisor = 1;   /* full speed */
        #endif
        pmlib_state.ddr_timeout = g_power_state.ddr_timeout;
    }
    else {
        #if 0
        pmlib_state.cpu_divisor = 8;   /* 1/8 speed */
        #endif
        pmlib_state.ddr_timeout = 64;  /* can also be 10 */

    }

    if (pmlib_state.tp1_status != BRCM_PM_UNDEF) {
        pmlib_state.tp1_status = state->tp1;
    }

    if (brcm_pm_set_status(g_power_state.pmlibHandle, &pmlib_state) != 0) {
        BDBG_WRN(("pmlib error"));
    }    

    g_power_state.state = *state;

    brutus_print_power_status();

    return 0;
}

