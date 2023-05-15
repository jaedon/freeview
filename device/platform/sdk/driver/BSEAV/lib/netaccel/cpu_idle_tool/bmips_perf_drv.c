/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmips_perf_drv.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 1/11/12 3:47p $
 *
 * $brcm_Log: /BSEAV/lib/netaccel/cpu_idle_tool/bmips_perf_drv.c $
 * 
 * 9   1/11/12 3:47p ssood
 * SW7425-452: extend bcm_perf_drv to report network stats on both
 * Ethernet & MoCA interfaces
 * 
 * 8   10/19/11 4:36p mward
 * SW7125-1119: Change DEBUG to B_REFSW_DEBUG.
 * 
 * 7   5/23/11 6:04p ssood
 * SW7425-452: change the include path of autoconf.h file
 * 
 * 6   5/13/11 1:34p katrep
 * SW7420-404:release the interface during module exit
 * 
 * 5   11/24/09 10:12a ssood
 * SW7420-404: code cleanup after porting cpu idle stats to Zephyr b0
 * 
 *************************************************************/ 
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
#include <generated/autoconf.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/smp.h>
#include <linux/timer.h>
#include <linux/sysfs.h>
#include <asm/processor.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/etherdevice.h>

#include "bmips_idle_stats.h"

extern unsigned long loops_per_jiffy;
extern int bmips_perf_stats_init(void);
extern void bmips_perf_stats_exit(void);
extern void setup_perfc_measurements(int);
extern void bperfc_measure_hook(int type, unsigned int off_usec);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
extern struct net init_net;
#endif

int g_cpu_idle_stats = 3; //prints stats every 3sec
module_param(g_cpu_idle_stats, int, S_IRUGO);

char *g_eth_if_name = NULL; //ethernet i/f to print stats on
module_param(g_eth_if_name, charp, 0);

int g_perfc_type = 1; //type of performance counter
module_param(g_perfc_type, int, S_IRUGO);

struct net_device *g_dev = NULL;
struct net_device *g_dev1 = NULL;

typedef struct idle_info_t {
    int initialized;
    struct timer_list timer;
    unsigned long begin_jiffies;
    unsigned long end_jiffies;
    int  count;
    long long strm_bytes;
    int     avg_load;
    atomic_t in_bytes_total;
    atomic_t in_bytes_total_prev;
    atomic_t in_filtered_bytes;
    atomic_t out_bytes;
    atomic_t out_bytes_total;
    atomic_t out_bytes_total_prev;
    atomic_t retrans;
    atomic_t xmit_timer_count;
} idle_info_t;
idle_info_t g_idle;
idle_info_t *idle = &g_idle;

#if 0
/* we can use this clock to accurately measure if timer runs accurately on the tickless kernel */
#include "asm/brcmstb/brcmstb.h"
struct wktmr_time start;
#endif
static void perf_timer_routine( unsigned long data)
{
    idle_info_t *idle = (idle_info_t *) data;
    struct net_device_stats *stats = NULL;
    struct net_device_stats *stats1 = NULL;

    idle->count++;
    if (g_dev) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
        stats = &g_dev->stats;
        if (g_dev1)
            stats1 = &g_dev1->stats;
#else
        stats = g_dev->get_stats(g_dev);
#endif
        atomic_set(&idle->out_bytes_total, stats->tx_bytes/g_cpu_idle_stats);
        if (stats1)
            atomic_add(stats1->tx_bytes/g_cpu_idle_stats, &idle->out_bytes_total);
        atomic_set(&idle->in_bytes_total, stats->rx_bytes/g_cpu_idle_stats);
        if (stats1)
            atomic_add(stats1->rx_bytes/g_cpu_idle_stats, &idle->in_bytes_total);
    }

    if (idle->initialized) {
        idle->end_jiffies = jiffies;
        if ((idle->end_jiffies - idle->begin_jiffies) < (g_cpu_idle_stats*HZ)) {
            goto out;
        }
#if 0
        printk("elapsed time in 27Mhz %ld\n", wktmr_elapsed(&start));
#endif
#ifndef PERFC_INSTRUMENT_KERNEL
        /* snapshot the end of perf counters */
        bperfc_measure_hook(1, 0);
#endif

        //if (idle->avg_load > 20)
        {
            bperfc_measure_hook(2, 0);
            bmips_idle_stats_print();
#ifdef B_REFSW_DEBUG
            printk(" delta jiffies %lu ", idle->end_jiffies - idle->begin_jiffies);
#endif
            if (stats)
#if 0
                printk(" In-Filt Mbps %u.%d, In-Tot Mbps %u.%d, Out-Filt Mbps %u.%1d Out-Tot Mpbs %u.%1u",
                    8*atomic_read(&idle->in_filtered_bytes) / 1000000, 
                    8*atomic_read(&idle->in_filtered_bytes) % 10, 
                    8*((atomic_read(&idle->in_bytes_total) - atomic_read(&idle->in_bytes_total_prev))) / 1000000, 
                    8*((atomic_read(&idle->in_bytes_total) - atomic_read(&idle->in_bytes_total_prev))) % 10, 
                    8*atomic_read(&idle->out_bytes) / 1000000,
                    8*atomic_read(&idle->out_bytes) % 10,
                    8*((atomic_read(&idle->out_bytes_total) - atomic_read(&idle->out_bytes_total_prev))) / 1000000,
                    8*((atomic_read(&idle->out_bytes_total) - atomic_read(&idle->out_bytes_total_prev))) % 10);
#else
                printk(" In-Tot Mbps %u.%d, Out-Tot Mpbs %u.%1u",
                    8*((atomic_read(&idle->in_bytes_total) - atomic_read(&idle->in_bytes_total_prev))) / 1000000, 
                    8*((atomic_read(&idle->in_bytes_total) - atomic_read(&idle->in_bytes_total_prev))) % 10, 
                    8*((atomic_read(&idle->out_bytes_total) - atomic_read(&idle->out_bytes_total_prev))) / 1000000,
                    8*((atomic_read(&idle->out_bytes_total) - atomic_read(&idle->out_bytes_total_prev))) % 10);
#endif
            printk("\n");
        }
    } else {
        /* (!idle->initialized) */
        bmips_idle_stats_print();
        idle->begin_jiffies = jiffies;
        idle->initialized = 1;
        atomic_set(&idle->out_bytes, 0);
    }
    
    atomic_set(&idle->xmit_timer_count,0);
    idle->begin_jiffies = jiffies;

    idle->strm_bytes += (atomic_read(&idle->out_bytes));
    atomic_set(&idle->out_bytes, 0);
    atomic_set(&idle->in_filtered_bytes, 0);
    atomic_set(&idle->out_bytes_total_prev, atomic_read(&idle->out_bytes_total));
    atomic_set(&idle->in_bytes_total_prev, atomic_read(&idle->in_bytes_total));

out:
    /* restart this timer */
    mod_timer(&idle->timer, jiffies + g_cpu_idle_stats * (HZ/100));
#ifndef PERFC_INSTRUMENT_KERNEL
    /* snaptop the start of perf counters */
    bperfc_measure_hook(0, 0);
#endif

#if 0
    wktmr_read(&start);
#endif

}

int bmips_perf_drv_init(void)
{
    if (g_cpu_idle_stats <= 0 || g_cpu_idle_stats > 10)
        g_cpu_idle_stats = 1;
    printk("%s: Printing CPU IDLE Stats Every %d sec for interfaces: ", __FUNCTION__, g_cpu_idle_stats);
    if (g_eth_if_name) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
        if (strstr(g_eth_if_name, "eth0")) {
            g_dev = dev_get_by_name(&init_net, "eth0");
            if (g_dev)
                printk("eth0, ");
        }
        if (strstr(g_eth_if_name, "eth1")) {
            g_dev1 = dev_get_by_name(&init_net, "eth1");
            if (g_dev1)
                printk("eth1");
        }
        printk("\n");
#else
        g_dev = dev_get_by_name(g_eth_if_name);
#endif
    }
    else 
        printk("\n");

    if (bmips_idle_stats_init())
        return (-1);
    if (bmips_perf_stats_init())
        return (-1);
    /* what type of MIPS performance stats to measure */
    setup_perfc_measurements(g_perfc_type);

    /* Setup the timer to periodically print the stats */
    init_timer(&idle->timer);
    idle->timer.data = (unsigned long)idle;
    idle->timer.function = perf_timer_routine;
    idle->timer.expires = jiffies + HZ*g_cpu_idle_stats;
    add_timer(&idle->timer);

    return (0);
}

void bmips_perf_drv_exit(void)
{
    printk("%s: Exit\n", __FUNCTION__);
    del_timer_sync(&idle->timer);
    bmips_idle_stats_exit();
    bmips_perf_stats_exit();
	if(g_dev)
	{
		dev_put(g_dev);
		g_dev=NULL;
	}
	if(g_dev1)
	{
		dev_put(g_dev1);
		g_dev1=NULL;
	}
	
	
}

module_init(bmips_perf_drv_init);
module_exit(bmips_perf_drv_exit);

MODULE_LICENSE("Proprietary");
