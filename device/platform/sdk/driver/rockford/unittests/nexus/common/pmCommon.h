/******************************************************************************
 *    (c)2008 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: pmCommon.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/2/12 2:18p $
 *
 * Module Description: pmCommon - collect commonly used power management wrapper functions into 
 *                                one includable header file for common use.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/common/pmCommon.h $
 * 
 * Hydra_Software_Devel/1   8/2/12 2:18p ahulse
 * SW7425-3613: collect power management functions into header file
******************************************************************************/

#ifndef __PM_COMMON_H__
#define __PM_COMMON_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "pmlib-263x.h"

#include "bstd.h"
#include "bkni.h"

#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_core_utils.h"

/* If BDBG_WRN / BDBG_ERR output is required, uncomment below, but also make sure the inclusion of this
   file (pmCommon.h) is after the declaration of the BDBG_MODULE()
*/
#define BDBG_WRN_TRACE(x)  /* BDBG_WRN(x) */
#define BDBG_ERR_TRACE(x)  /* BDBG_ERR(x) */

#if NEXUS_POWER_MANAGEMENT

#if (BCHP_CHIP == 7231)
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 1
#define MOCA_SUPPORT 0
#define MEMC1_SUPPORT 0
#elif (BCHP_CHIP == 7425)
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 1
#define MOCA_SUPPORT 1
#define MEMC1_SUPPORT 1
#elif (BCHP_CHIP == 7420)
#define MOCA_SUPPORT 1
#define MEMC1_SUPPORT 1
#elif (BCHP_CHIP == 7344 || BCHP_CHIP == 7346)
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 1
#define MOCA_SUPPORT 0
#define MEMC1_SUPPORT 0
#elif (BCHP_CHIP == 7358)
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 0
#define MOCA_SUPPORT 0
#define MEMC1_SUPPORT 0
#elif (BCHP_CHIP == 7552)
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 0
#define MOCA_SUPPORT 0
#define MEMC1_SUPPORT 0
#elif (BCHP_CHIP == 7429)
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 1
#define MOCA_SUPPORT 1
#define MEMC1_SUPPORT 1

#endif

/* this is a simple wrapper around pmlib to make the sample app simpler */
typedef struct pmlib_state_t
{
    bool usb;
    bool enet;
    bool moca;
    bool sata;
    bool tp1;
    bool memc1;
    bool cpu;
    bool ddr;
} pmlib_state_t;

static struct {
    void *brcm_pm_ctx;
    pmlib_state_t g_state;

#define MAX_MOUNTS 16
    struct {
        char device[256];
        char mountpoint[256];
    } mounts[MAX_MOUNTS];
    unsigned total_mounts;
} g_power_state;

#endif

#if NEXUS_POWER_MANAGEMENT

void get_pmlib_state(pmlib_state_t *state)
{
    *state = g_power_state.g_state;
}

int set_pmlib_state(const pmlib_state_t *state)
{
    struct brcm_pm_state pmlib_state;
    BERR_Code rc;

    rc = brcm_pm_get_status(g_power_state.brcm_pm_ctx, &pmlib_state);
    if (rc) { BDBG_ERR_TRACE(("Cannot get PM status")); }

    pmlib_state.usb_status = state->usb;
#if MEMC1_SUPPORT
    pmlib_state.memc1_status = state->memc1;
#else
    pmlib_state.memc1_status = BRCM_PM_UNDEF;
#endif
    pmlib_state.tp1_status = state->tp1;
#if 0 /* TODO: why does this not work? */
    pmlib_state.cpu_divisor = state->cpu?1:8;
#endif
    pmlib_state.ddr_timeout = state->ddr?0:64;

    rc = brcm_pm_set_status(g_power_state.brcm_pm_ctx, &pmlib_state);
    if (rc) { BDBG_ERR_TRACE(("Cannot set PM status\n")); }

    if (state->enet) {
        if (!g_power_state.g_state.enet) {
            system("ifup eth0");
            system("sleep 3");
            system("kill `cat /tmp/udhcpc.eth0.pid`");
        }
    }
    else {
        if (g_power_state.g_state.enet) {
            system("ifdown eth0");
        }
    }

#if MOCA_SUPPORT
    if (state->moca) {
        if (!g_power_state.g_state.moca){
            system("mocactl start");
            system("ifup eth1");
            system("sleep 3");
            system("kill `cat /tmp/udhcpc.eth1.pid`");
        }
    }
    else {
        if (g_power_state.g_state.moca){
            system("ifdown eth1");
            system("mocactl stop");
        }
    }
#endif

    g_power_state.g_state = *state;

    return 0;
}

int wait_for_device(char *device_path, unsigned long sleepTime_in_ms)
{
    int rc = 0;

    if (index( device_path, ':') == NULL   ) {  /* If device has ":", then it's an NFS path... skip it. */
        unsigned long ms;
        unsigned long ms_interval = 200;
        unsigned long ms_max = sleepTime_in_ms;
        int rc;
        struct stat statbuf;

        for (ms=0 ; ms<ms_max ; ms+=ms_interval) {
            rc = stat(device_path, &statbuf);
            if (rc == 0) break;
            if (ms == 0) BKNI_Printf("Waiting for device %s to exist...\n", device_path);
            BKNI_Sleep(ms_interval);
        }
        if (ms >= ms_max) {
            BKNI_Printf("Device %s not found after %lu ms\n", device_path, ms );
            return 1; /* return error code */
        }
        else if (ms > 0) {
            BKNI_Printf("Device %s appeared after %lu ms\n", device_path, ms );
        }
    }

    return rc;
}

int umount_all(void)
{
    FILE *f;

    g_power_state.total_mounts = 0;

    f = fopen("/proc/mounts", "r+");
    if (!f) { return -1; }
    while (!feof(f) && g_power_state.total_mounts < MAX_MOUNTS) {
        char buf[256];
        char *c1, *c2;
        if (fgets(buf, 256, f)) {
            if (strstr(buf, "/dev/sd") == buf || strstr(buf, " nfs ") || strstr(buf, "/dev/mmcblk") ) {
                if ( strstr(buf, " nfs ") && g_power_state.g_state.enet ) {
                    continue; /* Production system won't have nfs, but if have nfs don't unmount nfs !! */
                }

                c1 = strchr(buf, ' ');
                if (!c1) { continue; }

                *c1 = 0;
                strcpy(g_power_state.mounts[g_power_state.total_mounts].device, buf);
                c1++;
                c2 = strchr(c1, ' ');
                if (!c2) { continue; }

                *c2 = 0;
                strcpy(g_power_state.mounts[g_power_state.total_mounts].mountpoint, c1);

                BKNI_Printf("Found mount point %d: %s %s\n", g_power_state.total_mounts, g_power_state.mounts[g_power_state.total_mounts].device, g_power_state.mounts[g_power_state.total_mounts].mountpoint);


                snprintf(buf, 256, "umount %s", g_power_state.mounts[g_power_state.total_mounts].mountpoint);
                if (!system(buf)) {
                    g_power_state.total_mounts++;
               }
            }
        }
    }
    fclose(f);

    return 0;
}

int mount_all(void)
{
    unsigned i;

    for (i=0; i<g_power_state.total_mounts; i++) {
        char buf[256];
        int rc;

        rc = wait_for_device( g_power_state.mounts[i].device, 10000 );
        if (rc) { /* Device isn't there or not ready... */
            BKNI_Printf("Can't mount device \n", g_power_state.mounts[i].device );
            continue;
        }

        snprintf(buf, 256, "mount %s %s", g_power_state.mounts[i].device, g_power_state.mounts[i].mountpoint);
        BKNI_Printf("%s\n", buf);
        system(buf);
    }
    g_power_state.total_mounts = 0;

    return 0;
}

int active_standby(unsigned sleepTime)
{
    NEXUS_PlatformStandbySettings standbySettings;
    pmlib_state_t pmlib_state;
    NEXUS_Error rc;

    BDBG_WRN_TRACE(("Enter S1"));
    NEXUS_Platform_GetStandbySettings(&standbySettings);
    standbySettings.mode = NEXUS_PlatformStandbyMode_eActive;
    /* no need to set wakeup timeout, because there's nothing to be woken up */
    rc = NEXUS_Platform_SetStandbySettings(&standbySettings);
    BDBG_ASSERT(!rc);

    get_pmlib_state(&pmlib_state);
    pmlib_state.usb = false;
    pmlib_state.enet = true; /* must be left on if running over NFS */
    pmlib_state.moca = false;
    pmlib_state.tp1 = false;
    pmlib_state.cpu = false;
    pmlib_state.ddr = false;
    pmlib_state.memc1 = false;
    rc = set_pmlib_state(&pmlib_state);
    if (rc!=0) { return rc; }

    BKNI_Sleep(sleepTime*1000); /* stay in S1 for a bit... */

    BDBG_WRN_TRACE(("Exit S1"));
    get_pmlib_state(&pmlib_state);
    pmlib_state.usb = true;
    pmlib_state.enet = true;
    pmlib_state.moca = false;
    pmlib_state.tp1 = true;
    pmlib_state.cpu = true;
    pmlib_state.ddr = true;
    pmlib_state.memc1 = true;
    set_pmlib_state(&pmlib_state);
    if (rc!=0) { return rc; }

    return 0;
}

int passive_standby(unsigned sleepTime)
{
    NEXUS_PlatformStandbySettings standbySettings;
    NEXUS_PlatformStandbyStatus standbyStatus;
    NEXUS_Error rc;

    BDBG_WRN_TRACE(("Enter S2"));

    NEXUS_Platform_GetStandbySettings(&standbySettings);
    standbySettings.mode = NEXUS_PlatformStandbyMode_ePassive;
    standbySettings.wakeupSettings.timeout = sleepTime;
    rc = NEXUS_Platform_SetStandbySettings(&standbySettings);
    BDBG_ASSERT(!rc);

    umount_all();
    brcm_pm_suspend(g_power_state.brcm_pm_ctx, BRCM_PM_STANDBY); /* put MIPS to sleep */

    /* now in standby */

    BDBG_WRN_TRACE(("Exit S2"));
    NEXUS_Platform_GetStandbyStatus(&standbyStatus); /* this executes after we've woken up */
    if (!standbyStatus.wakeupStatus.timeout) {
        BDBG_ERR_TRACE(("S2 wakeup did not happen from timeout"));
        return -1;
    }

    mount_all();

    return 0;
}

int deep_sleep(unsigned sleepTime)
{
    NEXUS_PlatformStandbySettings standbySettings;
    NEXUS_PlatformStandbyStatus standbyStatus;
    NEXUS_Error rc;

    BDBG_WRN_TRACE(("Enter S3"));

    NEXUS_Platform_GetStandbySettings(&standbySettings);
    standbySettings.mode = NEXUS_PlatformStandbyMode_eDeepSleep;
    standbySettings.wakeupSettings.timeout = sleepTime;
    rc = NEXUS_Platform_SetStandbySettings(&standbySettings);
    BDBG_ASSERT(!rc);

#if S3_WARM_BOOT
    system("echo mem > /sys/power/state"); /* warm boot. no cold boot support in this app because it can't be tested automatically */
#endif

    BDBG_WRN_TRACE(("Exit S3"));
    NEXUS_Platform_GetStandbyStatus(&standbyStatus);
    if (!standbyStatus.wakeupStatus.timeout) {
        BDBG_ERR_TRACE(("S3 wakeup did not happen from timeout"));
        return -1;
    }

    return 0;
}

int resume(void)
{
    NEXUS_PlatformStandbySettings standbySettings;
    NEXUS_Error rc;

    BDBG_WRN_TRACE(("Resume"));
    NEXUS_Platform_GetStandbySettings(&standbySettings);
    standbySettings.mode = NEXUS_PlatformStandbyMode_eOn;
    rc = NEXUS_Platform_SetStandbySettings(&standbySettings);
    BDBG_ASSERT(!rc);

    return 0;
}
#endif

#endif /* __PM_COMMON_H__ */
