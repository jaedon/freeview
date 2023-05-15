/***************************************************************************
 *     Copyright (c) 2006-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bprofile.h $
 * $brcm_Revision: 16 $
 * $brcm_Date: 5/30/08 7:34p $
 *
 * Module Description:
 *
 * Embeddeble profiler library
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bprofile/bprofile.h $
 * 
 * 16   5/30/08 7:34p vsilyaev
 * PR 42356: Updated code to make it safe for 64-bit environment
 * 
 * 15   7/30/07 5:55p vsilyaev
 * PR 25701: Removed hardcoded configuratio
 * 
 * 14   7/30/07 5:54p vsilyaev
 * PR 32813: Updated to work x86/x86-64 plaforms
 * 
 * 13   12/14/06 4:36p vsilyaev
 * PR 25997: Added more run-time options
 * 
 * 12   12/11/06 6:21p vsilyaev
 * PR 25997: Added mode to eleminate estimated overhead of instrumented
 * code
 * 
 * 11   12/11/06 12:45p vsilyaev
 * PR 25997: Added calibrate routine
 * 
 * 10   12/9/06 12:04p vsilyaev
 * PR 25997: Improved posprocessing of multi-thread profile data
 * 
 * 9   12/8/06 7:23p vsilyaev
 * PR 25997: added option to limit number of output entries
 * 
 * 8   12/7/06 2:49p vsilyaev
 * PR 25997: Don't enable perfrormance counters
 * 
 * 7   12/7/06 2:43p vsilyaev
 * PR 25997: Added fixes for 3.4 GCC compiler
 * 
 * 6   12/5/06 4:31p vsilyaev
 * PR 25997: Added MIPS performance counters
 * 
 * 5   12/5/06 12:00p vsilyaev
 * PR 25997: Added faster, single threaded, probe routine
 * 
 * 4   11/30/06 8:33p vsilyaev
 * PR 25997: Use atomic variable to prevent missing data
 * 
 * 3   11/28/06 4:48p vsilyaev
 * PR 25997: Added multithreaded profiling
 * 
 * 2   11/20/06 4:54p vsilyaev
 * PR 25997: Decoupled profiling and symbol table
 * 
 * 1   11/16/06 5:24p vsilyaev
 * PR 25997: Embeddable profiler
 * 
 *******************************************************************************/
#ifndef __BPROFILE_H__
#define __BPROFILE_H__

/* #define	BPROFILE_CFG_SINGLE_THREAD	1  */

/* #define	BPROFILE_CFG_PERF_COUNTER	1  */

#if BPROFILE_CFG_PERF_COUNTER
#include  "bperf_counter.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bprofile_sample{
	unsigned time;
#if BPROFILE_CFG_PERF_COUNTER
	unsigned counters[BPROFILE_CFG_PERF_COUNTER];
#endif
}bprofile_sample;

typedef struct bprofile_entry {
#ifndef BPROFILE_CFG_SINGLE_THREAD
	unsigned long event_0;
#endif
	unsigned long addr;
	bprofile_sample sample;
} bprofile_entry;

#define B_PROFILE_EVENT_ENTER	0
#define B_PROFILE_EVENT_EXIT	1
#define B_PROFILE_EVENT_MASK 0x03

typedef struct bprofile_probe_info {
 	bprofile_sample overhead; /* overhewad associated with bprofile samping */
} bprofile_probe_info;

typedef const void *bprofile_thread_id;
typedef struct bprofile_sys_iface {
	/* this is a pointer to a function that is used to convert address to the symbol */
	const char *(*get_name)(unsigned long addr, char *buf, size_t buf_len);
	/* this is a pointer to a function that returns opaque handler id from the  stack pointer, NULL means that stack pointer is invalid */
	bprofile_thread_id (*thread_from_stack)(const unsigned long *stack);
	const char *(*thread_name)(bprofile_thread_id thread);
	size_t maxentries; /* maximum number of allocated entries in the profile report */
	size_t show_entries; /* maximum number of printed entries */
	bool split_threads; /* print separate report for each thread */
	bool substract_overhead; /* substract overhead that was cuased by sampling routines */
	bool call_count; /* print number of function calls */
	bool preempt_time; /* print preemption time */
}bprofile_sys_iface;

/* this function activates acquisiton of profiling data */
void bprofile_start(bprofile_entry *table, size_t nelem);
/* this function stops profiling */
int bprofile_stop(void);
/* this function generates profiler report from the acquired profiling data */
size_t bprofile_report_flat(bprofile_entry *table, size_t nentries, const bprofile_sys_iface *sys_iface);
void bprofile_sys_iface_init(bprofile_sys_iface *sys_iface);

/* this funnction is used to calibrate profiler routines, it returns costs of profiling in ticks */
unsigned bprofile_calibrate(bprofile_entry *table, size_t nelem);
/* this function returns current number of accumulated samples */
int bprofile_poll(void); 

void bprofile_get_info(bprofile_probe_info *info);

#ifdef __cplusplus
}
#endif

#endif /* __BPROFILE_H__ */
