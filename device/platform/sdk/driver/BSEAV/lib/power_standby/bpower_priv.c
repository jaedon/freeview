/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpower_priv.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 8/9/10 3:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/power_standby/bpower_priv.c $
 * 
 * 14   8/9/10 3:26p dlwin
 * SW3548-3032: Merged CEC wakeup implementation from customer branch
 * code.
 * 
 * 13   2/9/10 7:38p dlwin
 * SW3548-2775: If the time-out for wakeup is set to 0, then disable
 * wakeup from timer.
 * 
 * 12   12/21/09 5:24p mward
 * SW7125-138:  Add 7125.
 * 
 * 11   11/9/09 4:25p erickson
 * SWLINUX-1418: improve error messages
 *
 * 10   6/17/09 11:09a maivu
 * PR 56083: Add compile option to enable pause before going into standby
 *
 * 9   4/30/09 1:17p maivu
 * PR 54724: Do not init/uninit BKNI. Leave it to Nexus or app to handle
 * this. Nexus does not uninit bkni.
 *
 * 8   2/11/09 2:00p maivu
 * PR 51679: Add PC/VGA wakeup support
 *
 * 7   2/11/09 10:40a maivu
 * PR 51963: Sync up with latest name changes in pmdriver. Added support
 * for generic GPIO wakeup
 *
 * 6   2/2/09 3:50p maivu
 * PR 50308: Don't execute BDBG_Init/Uninit, let apps call it
 *
 * 6   2/2/09 3:49p maivu
 * PR 50308: Don't execute BDBG_Init/Uninit, let apps call it
 *
 * 5   1/15/09 9:25a jrubio
 * PR50406: add 7325
 *
 * 4   12/17/08 2:31p maivu
 * PR 50308: Unlink dhcp before power up Ethernet
 *
 * 3   11/6/08 3:36p maivu
 * PR 48698: Remove BSTD_ADS_ID and BSTD_QDS_ID, replace with BSTD_VSB_ID
 * to match with latest PWR PI support
 *
 * 2   11/5/08 11:12a maivu
 * PR 48036: Initialize table counter, to power down PWR correctly
 *
 * 1   10/29/08 4:00p maivu
 * PR 48036: power standby library, based off power_standby app in
 * rockford. Initial support for 3548/3556. Tested and confirmed working
 * on 93549B0
 *
 ***************************************************************************/

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
#include "bchp_cg_intr2.h"
#else
#error "Unsupported chip!"
#endif

#include "bpwr.h"

#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include "bpower_standby.h"
#include "pmlib.h"
#include "brcmpmdrv.h"

BDBG_MODULE(bpower_priv);

/*
 * Note: g_testActiveModuleList is not necessarily representative of active standby.
 * Active Standby should be integrated into the real application carefully, as it
 * requires an understanding of which resources are currently in use.
 *
 * The module list chosen here is solely for testing with transport still active.
 *
 * THIS IS ONLY A TEST MODE -- REAL APPLICATIONS WOULD INTEGRATE ACTIVE STANDBY.
 */
#if (BCHP_CHIP==7400)
BSTD_Module g_testActiveModuleList[] =
{
    BSTD_RFM_ID,
    BSTD_RAP_ID,
    BSTD_HDM_ID,
    BSTD_ARC_ID,
    BSTD_XVD_ID,
    BSTD_GRC_ID,
    BSTD_P3D_ID,
    BSTD_VBI_ID,
    BSTD_VDC_ID
};
uint32_t g_ulTestActiveModuleCnt = sizeof(g_testActiveModuleList)/sizeof(BSTD_Module);
BSTD_Module g_passiveModuleList[] =
{
    BSTD_RFM_ID,
    BSTD_RAP_ID,
    BSTD_HDM_ID,
    BSTD_ARC_ID,
    BSTD_XVD_ID,
    BSTD_XPT_ID,
    BSTD_GRC_ID,
    BSTD_P3D_ID,
    BSTD_VBI_ID,
    BSTD_VDC_ID,
    BSTD_SCD_ID
};
uint32_t g_ulPassiveModuleCnt = sizeof(g_passiveModuleList)/sizeof(BSTD_Module);
int g_flags =
    PPMD_IOCTL_FLAGS_SET_UPG_TIMER_DURATION |
    PPMD_IOCTL_FLAGS_MEMORY_SELF_REFRESH | PPMD_IOCTL_FLAGS_SELF_REFRESH_W_PADS;
int g_events =
    PPMD_IOCTL_EVENT_IR |
    PPMD_IOCTL_EVENT_KEYPAD |
    PPMD_IOCTL_EVENT_GPIO |
    PPMD_IOCTL_EVENT_TIMEOUT;
#elif (BCHP_CHIP==7405)
BSTD_Module g_testActiveModuleList[] =
{
    BSTD_RFM_ID,
    BSTD_RAP_ID,
    BSTD_HDM_ID,
    BSTD_ARC_ID,
    BSTD_XVD_ID,
    BSTD_GRC_ID,
    BSTD_VBI_ID,
    BSTD_VDC_ID,
    BSTD_UHF_ID
};
uint32_t g_ulTestActiveModuleCnt = sizeof(g_testActiveModuleList)/sizeof(BSTD_Module);
BSTD_Module g_passiveModuleList[] =
{
    BSTD_RFM_ID,
    BSTD_RAP_ID,
    BSTD_HDM_ID,
    BSTD_ARC_ID,
    BSTD_XVD_ID,
    BSTD_XPT_ID,
    BSTD_GRC_ID,
    BSTD_VBI_ID,
    BSTD_VDC_ID,
    BSTD_UHF_ID,
    BSTD_SCD_ID
};
uint32_t g_ulPassiveModuleCnt = sizeof(g_passiveModuleList)/sizeof(BSTD_Module);
int g_flags =
    PPMD_IOCTL_FLAGS_SET_UPG_TIMER_DURATION |
    PPMD_IOCTL_FLAGS_MEMORY_SELF_REFRESH | PPMD_IOCTL_FLAGS_SELF_REFRESH_W_PADS;
int g_events =
    PPMD_IOCTL_EVENT_IR |
    PPMD_IOCTL_EVENT_KEYPAD |
    PPMD_IOCTL_EVENT_GPIO |
    PPMD_IOCTL_EVENT_TIMEOUT;
#elif (BCHP_CHIP==7325) || (BCHP_CHIP==7335)
BSTD_Module g_testActiveModuleList[] =
{
    BSTD_RFM_ID,
    BSTD_RAP_ID,
    BSTD_HDM_ID,
    BSTD_ARC_ID,
    BSTD_XVD_ID,
    BSTD_GRC_ID,
    BSTD_VBI_ID,
    BSTD_UPG_ID,
    BSTD_MDM_ID,
    BSTD_VDC_ID,
#if (BCHP_CHIP == 7335)
    BSTD_AST0_ID,
    BSTD_AST1_ID,
    BSTD_QPSK_ID,
    BSTD_FTM_ID
#else
    BSTD_AST_ID,
    BSTD_QPSK_ID,
    BSTD_FTM_ID
#endif
};
uint32_t g_ulTestActiveModuleCnt = sizeof(g_testActiveModuleList)/sizeof(BSTD_Module);
BSTD_Module g_passiveModuleList[] =
{
    BSTD_RFM_ID,
    BSTD_RAP_ID,
    BSTD_HDM_ID,
    BSTD_ARC_ID,
    BSTD_XVD_ID,
    BSTD_XPT_ID,
    BSTD_GRC_ID,
    BSTD_VBI_ID,
    BSTD_VDC_ID,
    BSTD_UPG_ID,
    BSTD_MDM_ID,
    BSTD_SCD_ID,
#if (BCHP_CHIP == 7335)
    BSTD_AST0_ID,
    BSTD_AST1_ID,
    BSTD_QPSK_ID,
    BSTD_FTM_ID
#else
    BSTD_AST_ID,
    BSTD_QPSK_ID,
    BSTD_FTM_ID
#endif
};
uint32_t g_ulPassiveModuleCnt = sizeof(g_passiveModuleList)/sizeof(BSTD_Module);
int g_flags =
    PPMD_IOCTL_FLAGS_SET_UPG_TIMER_DURATION |
    PPMD_IOCTL_FLAGS_MEMORY_SELF_REFRESH | PPMD_IOCTL_FLAGS_SELF_REFRESH_W_PADS;
int g_events =
    PPMD_IOCTL_EVENT_IR |
    PPMD_IOCTL_EVENT_KEYPAD |
    PPMD_IOCTL_EVENT_GPIO |
    PPMD_IOCTL_EVENT_TIMEOUT;
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
BSTD_Module g_testActiveModuleList[] =
{   /* not supporting active-standby */
0
};
/*
  BSTD_HDM_ID: HDMI Receiver 0 and 1
  BSTD_VSB_ID: NTSC/PAL
     - blocks will be powered down/up: DS_AFE -> DS CLK -> DS PLL -> IFD -> NTSC video decoder/VBI deoder (VDEC Front/Back, PC VBI)
    - blocks will be powered down/up: DS_AFE -> DS CLK -> DS PLL(DS_THD) -> AP -> OB
  BSTD_VIF_ID: CBVS/COmponent/PC (Video Input Frontend)
    - blocks will be powered down/up: VAFE4 -> NTSC video decoder/VBI deoder (VDEC Front/Back, PC VBI)
  BSTD_RAP_ID: audio output and its clock
  BSTD_VDC_ID and BSTD_VBI_ID: video output and its clock
  BSTD_XPT_ID: transport
  BSTD_XVD_ID and BSTD_ARC_ID: decorder
*/
uint32_t g_ulTestActiveModuleCnt = sizeof(g_testActiveModuleList)/sizeof(BSTD_Module);
BSTD_Module g_passiveModuleList[] =
{
    BSTD_ARC_ID,
    BSTD_XVD_ID,
    BSTD_GRC_ID,
    BSTD_HDM_ID,
    BSTD_P3D_ID,
    BSTD_RAP_ID,
    BSTD_SCD_ID,
    BSTD_VBI_ID,
    BSTD_VDC_ID,
    BSTD_XPT_ID,
    BSTD_VSB_ID,
    BSTD_VIF_ID
};
uint32_t g_ulPassiveModuleCnt = sizeof(g_passiveModuleList)/sizeof(BSTD_Module);
int g_flags =
    PPMD_IOCTL_FLAGS_SET_UPG_TIMER_DURATION |
    PPMD_IOCTL_FLAGS_MEMORY_SELF_REFRESH | PPMD_IOCTL_FLAGS_SELF_REFRESH_W_PADS;
int g_events =
    PPMD_IOCTL_EVENT_IR|
    PPMD_IOCTL_EVENT_KEYPAD |
    PPMD_IOCTL_EVENT_GPIO |
    PPMD_IOCTL_EVENT_VGA |
    PPMD_IOCTL_EVENT_TIMEOUT;

#elif (BCHP_CHIP==7125)
BSTD_Module g_testActiveModuleList[] =
{
    BSTD_RAP_ID,
    BSTD_HDM_ID,
    BSTD_ARC_ID,
    BSTD_XVD_ID,
    BSTD_GRC_ID,
    BSTD_VBI_ID,
    BSTD_VDC_ID
};
uint32_t g_ulTestActiveModuleCnt = sizeof(g_testActiveModuleList)/sizeof(BSTD_Module);
BSTD_Module g_passiveModuleList[] =
{
    BSTD_RAP_ID,
    BSTD_HDM_ID,
    BSTD_ARC_ID,
    BSTD_XVD_ID,
    BSTD_XPT_ID,
    BSTD_GRC_ID,
    BSTD_VBI_ID,
    BSTD_VDC_ID,
    BSTD_SCD_ID
};
uint32_t g_ulPassiveModuleCnt = sizeof(g_passiveModuleList)/sizeof(BSTD_Module);
int g_flags = PPMD_IOCTL_FLAGS_SET_UPG_TIMER_DURATION;
int g_events =
    PPMD_IOCTL_EVENT_IR |
    PPMD_IOCTL_EVENT_KEYPAD |
    PPMD_IOCTL_EVENT_GPIO |
    PPMD_IOCTL_EVENT_TIMEOUT;
#else
#error "Unsupported chip!"
#endif

void *bpower_p_brcm_mem_init(void) {

    uint32_t base = 0xb00000;
    uint32_t offset=BCHP_PHYSICAL_OFFSET;
    void *base_mem;
    int fd = open("/dev/mem", O_RDWR|O_SYNC);

    if (fd<0) {
        perror("/dev/mem");
        exit(1);
    }
#if BCHP_CHIP==7400
    base = (BCHP_PX3D_AddedRegSets + 4);
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
    base = (BCHP_PX3D_TIME_STATUS + 4);
#elif BCHP_CHIP == 7405
    base = BCHP_MEM_DMA_SECURE_REG_END;
#endif

    offset = BCHP_PHYSICAL_OFFSET;
    base_mem = mmap(NULL, base, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
    if (base_mem==MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    return base_mem;
}

unsigned int bpower_p_get_gpio_irq(unsigned long gpio_mask)
{
    unsigned int gpio_irq=0;

#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
    if (gpio_mask & BCHP_CG_INTR2_CPU_STATUS_EXT_IRQ_B_0_INTR_MASK) {
        gpio_irq = 0;
    }
    else if (gpio_mask & BCHP_CG_INTR2_CPU_STATUS_EXT_IRQ_B_1_INTR_MASK) {
        gpio_irq = 1;
    }
    else if (gpio_mask & BCHP_CG_INTR2_CPU_STATUS_EXT_IRQ_B_2_INTR_MASK) {
        gpio_irq = 2;
    }
    else if (gpio_mask & BCHP_CG_INTR2_CPU_STATUS_EXT_IRQ_B_3_INTR_MASK) {
        gpio_irq = 3;
    }
    else if (gpio_mask & BCHP_CG_INTR2_CPU_STATUS_EXT_IRQ_B_4_INTR_MASK) {
        gpio_irq = 4;
    }
    else if (gpio_mask & BCHP_CG_INTR2_CPU_STATUS_EXT_IRQ_B_5_INTR_MASK) {
        gpio_irq = 5;
    }
    else {
        gpio_irq = 6;
    }
#endif

    return gpio_irq;

}

int bpower_p_brcm_standby(bpower_standby_t power_standby)
{
    int count;
    int help;
    int fd, err;
    int upg_duration;
    struct timeval now, then, diff;
    PPMD_IOCTL ppmd_ioctl_arg;

    /* default settings */
    upg_duration = 2;
    err = 0;
    help = 0;

    /* Passive standby */
    if (power_standby->settings.bPassive) {
        /* open the device node */
        fd = open("/dev/brcmpmdrv", O_RDWR);
        if (fd<0) {
            BDBG_ERR(("Driver open failed, did you create the device node? errno=%d", errno));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
        }

        /* Check if the application wants to disable timer wake-up event */
        if( power_standby->settings.hours == 0 && power_standby->settings.minutes == 0 && power_standby->settings.seconds == 0 ) {
            /* disable the UPG Timer wake-up event */
            ppmd_ioctl_arg.flags = (power_standby->flags & ~PPMD_IOCTL_FLAGS_SET_UPG_TIMER_DURATION);
            ppmd_ioctl_arg.events = power_standby->events;
        }
        else {
            err = gettimeofday(&now, NULL);
            if (err) {
                BDBG_ERR(("gettimeofday failed: %d", err));
                return BERR_TRACE(BPOWER_STANDBY_FAILURE);
            }

            /* Copy current time to alarm time and add in user input */
            memcpy(&then, &now, sizeof(struct timeval));
            count = (power_standby->settings.hours*3600) +
                (power_standby->settings.minutes*60) + power_standby->settings.seconds;

            if ((power_standby->flags&PPMD_IOCTL_FLAGS_SET_UPG_TIMER_DURATION)==0)
                upg_duration = PPMD_IOCTL_DEFAULT_UPG_DURATION;

            then.tv_sec = count;

            /* Setup ioctl structure */
            ppmd_ioctl_arg.flags = power_standby->flags;
            ppmd_ioctl_arg.events = power_standby->events;
            if (power_standby->settings.bWake_on_CEC) {
                ppmd_ioctl_arg.events |= PPMD_IOCTL_EVENT_CEC;
            }
            memcpy(&ppmd_ioctl_arg.end_time, &then, sizeof(struct timeval));
            ppmd_ioctl_arg.upg_timer_duration = upg_duration;
        }
#ifdef ENABLE_STANDBY_PAUSE
        /* For debugging purpose only */
        printf("Waiting for key press before calling PPMD_IOCTL_ACTIVATE_SLEEP...\n");
        getchar();
#endif

        fflush(stdout);
        /* Call ioctl */
        err = ioctl(fd, PPMD_IOCTL_ACTIVATE_SLEEP, &ppmd_ioctl_arg);
        if (err) {
            BDBG_ERR(("Ioctl failed, probably caused by a driver conflict with bcmdriver: errno=%d", errno));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
        }

        /* Print time difference */
        err = gettimeofday(&then, NULL);
        if (err) {
            BDBG_ERR(("gettimeofday failed: %d", err));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
        }
        timersub(&then, &now, &diff);

        BDBG_MSG(("Wakeup event was from "));
        switch (ppmd_ioctl_arg.wake_event) {
        case PPMD_IOCTL_EVENT_IR:
            power_standby->wakeup.eWakeupEvent = bpower_standby_wakeup_eIR;
            BDBG_MSG(("IR remote power button"));
            break;
        case PPMD_IOCTL_EVENT_KEYPAD:
            power_standby->wakeup.eWakeupEvent = bpower_standby_wakeup_eKeypad;
            BDBG_MSG(("keypad power button"));
            break;
        case PPMD_IOCTL_EVENT_GPIO:
            power_standby->wakeup.eWakeupEvent = bpower_standby_wakeup_eGpio;
            power_standby->wakeup.gpioExtIrq = bpower_p_get_gpio_irq(ppmd_ioctl_arg.ext_irq_mask);
            BDBG_MSG(("GPIO ext_irq_b_#%d activity",power_standby->wakeup.gpioExtIrq));
            break;
        case PPMD_IOCTL_EVENT_VGA:
            power_standby->wakeup.eWakeupEvent = bpower_standby_wakeup_eVGA;
            BDBG_MSG(("PC/VGA activity"));
            break;
        case PPMD_IOCTL_EVENT_CEC:
			power_standby->wakeup.eWakeupEvent = bpower_standby_wakeup_eCEC;
			power_standby->wakeup.cec_addr = ppmd_ioctl_arg.cec_addr;
            BDBG_MSG(("CEC activity"));
            break;
        case PPMD_IOCTL_EVENT_TIMEOUT:
            power_standby->wakeup.eWakeupEvent = bpower_standby_wakeup_eTimeout;
            BDBG_MSG(("standby timer expiring"));
            break;
        default:
            power_standby->wakeup.eWakeupEvent = bpower_standby_wakeup_eUnknown;
            BDBG_MSG(("an unknown source"));
            break;
        }

        close(fd);
    }
    else {
        err = gettimeofday(&now, NULL);
        if (err) {
            BDBG_ERR(("gettimeofday failed: %d", err));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
        }

        /* Copy current time to alarm time and add in user input */
        memcpy(&then, &now, sizeof(struct timeval));
        count = (power_standby->settings.hours*3600) + (power_standby->settings.minutes*60) + power_standby->settings.seconds;
        then.tv_sec = count;

        BKNI_Sleep(count * 1000); /* Sleep for count milliseconds */
    }

    return BPOWER_STANDBY_SUCCESS;
}


int bpower_standby_p_os_powerdown(bpower_standby_t power_standby)
{
    int rc;

    power_standby->brcm_pm_ctx = brcm_pm_init();
    if (!power_standby->brcm_pm_ctx) {
        BDBG_ERR(("Can't open PM context"));
        return BERR_TRACE(BPOWER_STANDBY_FAILURE);
    }
    rc = brcm_pm_get_status(power_standby->brcm_pm_ctx, &power_standby->state);
    if (rc) {
        BDBG_ERR(("Can't get PM state: %d", rc));
        brcm_pm_close(power_standby->brcm_pm_ctx);
        power_standby->brcm_pm_ctx = NULL;
        return BERR_TRACE(BPOWER_STANDBY_FAILURE);
    }

    power_standby->ddr_timeout = power_standby->state.ddr_timeout;

    if (power_standby->state.enet_status != BRCM_PM_UNDEF) {
        power_standby->state.enet_status = 0;
        rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
        if (rc) {
            BDBG_ERR(("Unable to power down device [ENET]: %d", rc));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
        }
    }
    else {
        BDBG_MSG(("Don't touch [ENET]"));
    }

    if (power_standby->state.usb_status != BRCM_PM_UNDEF) {
        power_standby->state.usb_status = 0;
        rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
        if (rc) {
            BDBG_ERR(("Unable to power down device [USB]: %d", rc));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
        }
    }
    else {
        BDBG_MSG(("Don't touch [USB]"));
    }

    if (power_standby->settings.bPassive) {

#if (BCHP_CHIP==7400) || (BCHP_CHIP==7405) || (BCHP_CHIP == 7335)
        if (power_standby->state.sata_status != BRCM_PM_UNDEF) {
            power_standby->state.sata_status = 0;
            rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
            if (rc) {
                BDBG_ERR(("Unable to power down device [SATA]: %d", rc));
                return BERR_TRACE(BPOWER_STANDBY_FAILURE);
            }
        }
        else {
            BDBG_MSG(("Don't touch [SATA]"));
        }
#endif
#if  BCHP_CHIP != 7325
        if (power_standby->state.tp1_status != BRCM_PM_UNDEF) {
            power_standby->state.tp1_status = 0;
            rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
            if (rc) {
                BDBG_ERR(("Unable to power down device [TP1]: %d", rc));
                return BERR_TRACE(BPOWER_STANDBY_FAILURE);
            }
            BDBG_MSG(("TP1 Shutdown successful"));
        }
        else {
            BDBG_MSG(("Don't touch [TP1]"));
        }

        power_standby->state.cpu_divisor = 8;
        power_standby->state.ddr_timeout = 64; /* can also be 10 */
        rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
        if (rc) {
            BDBG_ERR(("Unable to power down device [CPU/DDR]: %d", rc));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
        }
#endif
    }

    return BPOWER_STANDBY_SUCCESS;
}

int bpower_standby_p_os_powerup(bpower_standby_t power_standby)
{
    int rc;

    if (!power_standby->brcm_pm_ctx) {
        BDBG_ERR(("No PM context"));
        return BERR_TRACE(BPOWER_STANDBY_FAILURE);
    }

    rc = brcm_pm_get_status(power_standby->brcm_pm_ctx, &power_standby->state);
    if (rc) {
        BDBG_ERR(("Can't get PM state: %d", rc));
        brcm_pm_close(power_standby->brcm_pm_ctx);
        power_standby->brcm_pm_ctx = NULL;
        return BERR_TRACE(BPOWER_STANDBY_FAILURE);
    }

    if (power_standby->settings.bPassive) {
#if (BCHP_CHIP != 7325)
        power_standby->state.cpu_divisor = 1;
        power_standby->state.ddr_timeout = power_standby->ddr_timeout;
        rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
        if (rc) {
            BDBG_MSG(("Unable to power up device[CPU/DDR]: %d", rc));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
        }
        if (power_standby->state.tp1_status != BRCM_PM_UNDEF) {
            power_standby->state.tp1_status = 1;
            rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
            if (rc) {
                BDBG_ERR(("Unable to power up device[TP1]: %d", rc));
            }
        }
        else {
            BDBG_MSG(("Don't touch [TP1]"));
        }
#endif
#if (BCHP_CHIP==7400) || (BCHP_CHIP==7405) || (BCHP_CHIP == 7335)
        if (power_standby->state.sata_status != BRCM_PM_UNDEF) {
            power_standby->state.sata_status = 1;
            rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
            if (rc) {
                BDBG_ERR(("Unable to power up device[SATA]: %d", rc));
                return BERR_TRACE(BPOWER_STANDBY_FAILURE);
            }
        }
        else {
            BDBG_MSG(("Don't touch [SATA]"));
        }
#endif
    }

    if (power_standby->state.usb_status != BRCM_PM_UNDEF) {
        power_standby->state.usb_status = 1;
        rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
        if (rc) {
            BDBG_ERR(("Unable to power up device[USB]: %d", rc));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
        }
    }
    else {
        BDBG_MSG(("Don't touch [USB]"));
    }

    unlink("/etc/config/dhcpc/dhcpcd-eth0.pid");

    if (power_standby->state.enet_status != BRCM_PM_UNDEF) {
        power_standby->state.enet_status = 1;
        rc = brcm_pm_set_status(power_standby->brcm_pm_ctx, &power_standby->state);
        if (rc) {
            BDBG_ERR(("Unable to power up device[ENET]: %d", rc));
            return BERR_TRACE(BPOWER_STANDBY_FAILURE);
       }
    }
    else {
        BDBG_MSG(("Don't touch [ENET]"));
    }
    brcm_pm_close(power_standby->brcm_pm_ctx);

    return BPOWER_STANDBY_SUCCESS;
}

void bpower_standby_p_init(bpower_standby_t power_standby)
{
    void *base;
    BPWR_Settings pwr_settings;
    BERR_Code eStatus;

    power_standby->events = g_events;
    power_standby->flags = g_flags;
    power_standby->activeModuleList = &g_testActiveModuleList[0];
    power_standby->passiveModuleList = &g_passiveModuleList[0];
    power_standby->passiveModuleCnt = g_ulPassiveModuleCnt;
    power_standby->activeModuleCnt = g_ulTestActiveModuleCnt;

    BDBG_MSG(("Enter bpower_standby_p_init"));

    base = bpower_p_brcm_mem_init();
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
    BREG_Open(&power_standby->hReg, base, BCHP_PX3D_TIME_STATUS+4);
#elif BCHP_CHIP == 7405
    BREG_Open(&power_standby->hReg, base, BCHP_MEM_DMA_SECURE_REG_END);
#else
    BREG_Open(&power_standby->hReg, base, 0x0120027C);
#endif

    /* This is currently not actually referenced by PWR.  If this changes, a real handle will be needed.
     * However, BCHP_Open7400() (and its ilk) currently reset some cores.  This could cause issues with
     * in-progress decodes during a suspend.
     */
    power_standby->hChp = (void *)1;

#if BCHP_CHIP == 7400
    BPWR_GetDefaultSettings(&pwr_settings);
    pwr_settings.ulMemc0StartAddr = BCHP_MEMC_0_CLIENT_INFO_0;
    pwr_settings.ulMemc0ClientCount = 64;
    pwr_settings.ulMemc1StartAddr = BCHP_MEMC_1_CLIENT_INFO_0;
    pwr_settings.ulMemc1ClientCount = 16;
    pwr_settings.ulMemc2StartAddr = BCHP_MEMC_2_CLIENT_INFO_0;
    pwr_settings.ulMemc2ClientCount = 16;
#elif BCHP_CHIP == 7405
    BPWR_GetDefaultSettings(&pwr_settings);
    pwr_settings.ulMemc0StartAddr = BCHP_MEMC_0_1_CLIENT_INFO_0;
    pwr_settings.ulMemc0ClientCount = 128;
    /* todo for UMA mode this should be NULL */
    pwr_settings.ulMemc1StartAddr = BCHP_MEMC_1_1_CLIENT_INFO_0;
    pwr_settings.ulMemc1ClientCount = 128;
    pwr_settings.ulMemc2StartAddr = 0;
    pwr_settings.ulMemc2ClientCount = 0;
#elif BCHP_CHIP == 7335
    BPWR_GetDefaultSettings(&pwr_settings);
    pwr_settings.ulMemc0StartAddr = BCHP_MEMC_0_1_CLIENT_INFO_0;
    pwr_settings.ulMemc0ClientCount = 128;
    /* todo for UMA mode this should be NULL */
    pwr_settings.ulMemc1StartAddr = BCHP_MEMC_1_1_CLIENT_INFO_0;
    pwr_settings.ulMemc1ClientCount = 128;
    pwr_settings.ulMemc2StartAddr = 0;
    pwr_settings.ulMemc2ClientCount = 0;
#elif BCHP_CHIP == 7325
    BPWR_GetDefaultSettings(&pwr_settings);
    pwr_settings.ulMemc0StartAddr = BCHP_MEMC_0_1_CLIENT_INFO_0;
    pwr_settings.ulMemc0ClientCount = 128;
    /* todo for UMA mode this should be NULL */
    pwr_settings.ulMemc1StartAddr = BCHP_MEMC_1_1_CLIENT_INFO_0;
    pwr_settings.ulMemc1ClientCount = 128;
    pwr_settings.ulMemc2StartAddr = 0;
    pwr_settings.ulMemc2ClientCount = 0;
#elif (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
    BPWR_GetDefaultSettings(&pwr_settings);
    pwr_settings.ulMemc0StartAddr = BCHP_MEMC_0_1_CLIENT_INFO_0;
    pwr_settings.ulMemc0ClientCount = 128;
    pwr_settings.ulMemc1StartAddr = 0;
    pwr_settings.ulMemc1ClientCount = 0;
    pwr_settings.ulMemc2StartAddr = 0;
    pwr_settings.ulMemc2ClientCount = 0;

#else
    /* error chip not supported */
#endif

    /* Initialize PWR PI */
    eStatus = BPWR_Open(&power_standby->hPwr, power_standby->hChp, power_standby->hReg, &pwr_settings);

}

