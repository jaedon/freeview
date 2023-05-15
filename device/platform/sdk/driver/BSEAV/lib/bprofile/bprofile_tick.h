/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bprofile_tick.h $
 * $brcm_Revision: 16 $
 * $brcm_Date: 12/22/11 2:30p $
 *
 * Module Description:
 *
 * Embeddeble profiler library
 * 		Data acquisition module
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bprofile/bprofile_tick.h $
 * 
 * 16   12/22/11 2:30p vsilyaev
 * SW7425-2039: Added support for 7425
 * 
 * 15   6/13/08 11:51a vsilyaev
 * PR 42356: Fixed compilation in the standalone environment
 * 
 * 14   6/12/08 2:48p vsilyaev
 * PR 42356: Updated to work with emulated performance counters
 * 
 * 13   5/30/08 7:34p vsilyaev
 * PR 42356: Updated code to make it safe for 64-bit environment
 * 
 * 12   4/9/08 2:40p vsilyaev
 * PR 41083: Added linux user performance sampling for BMIPS4380
 * 
 * 11   3/28/07 5:42p vsilyaev
 * PR 26928: Adding 7400 support
 * 
 * 10   1/15/07 10:50a vsilyaev
 * PR 25997: Added 7038 performance counters
 * 
 * 9   12/13/06 7:43p vsilyaev
 * PR 25997: Added support for MIPS3300
 * 
 * 8   12/7/06 4:33p vsilyaev
 * PR 25997: Fixed lowercase defined
 * 
 * 7   12/7/06 2:43p vsilyaev
 * PR 25997: Added fixes for 3.4 GCC compiler
 * 
 * 6   12/5/06 11:58a vsilyaev
 * PR 25997: Added faster, single threaded, probe routine
 * 
 * 5   12/1/06 4:18p vsilyaev
 * PR 25997: Improved time accuracy
 * 
 * 4   11/27/06 11:54a vsilyaev
 * PR 25997: Fixed compile issue for the Linux
 * 
 * 3   11/20/06 4:54p vsilyaev
 * PR 25997: Decoupled profiling and symbol table
 * 
 * 2   11/16/06 6:59p vsilyaev
 * PR 25997: Added UCOS support
 * 
 * 1   11/16/06 5:24p vsilyaev
 * PR 25997: Embeddable profiler
 * 
 *******************************************************************************/

#include "bperf_counter.h"
#if (defined(LINUX) || defined(__linux__)) && !defined(__KERNEL__)
#define b_get_stack(stack) do{unsigned sp; *(stack)=(unsigned long)&sp;} while(0)

#if B_PERF_BMIPS4380 || B_PERF_BMIPS5000
/* perfomance counters are memory mapped and could be accessed from the user space */

/* in linux user space we can't access cycle counter from C0 */
/* so use 4-th performance counter, and it shall be mapped to the MIPS cycle counter */
#if B_PERF_BMIPS4380 
#define b_gettick() (~(b_perf_read_one(3)))
#elif B_PERF_BMIPS5000
#define b_gettick() b_perf_read_one(4)
#else
#error "Not supported"
#endif
#include "bprofile_tick_mips.h"
/* this variable shall be initialized to number of ticks in 100us */ 
extern unsigned  b_ticks_100us;
#define b_tick2ms_init() unsigned clock__=b_ticks_100us
#define b_tick2_100us(tick) ((tick)/(clock__))

#else /* B_PERF_BMIPS4380 */
#include <sys/time.h>
BSTD_INLINE unsigned __attribute__((no_instrument_function))
b_gettick(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec+((unsigned)tv.tv_sec)*1000000;
}

BSTD_INLINE void __attribute__((no_instrument_function))
b_sample(bprofile_sample *sample)
{
	sample->time = b_gettick();
	return ;
}
#define b_tick2ms_init()
#define b_tick2_100us(tick) ((tick)/100)

#endif /* B_PERF_BMIPS4380  */


#elif defined(CONFIG_UCOS)
#include "bprofile_tick_mips.h"
#define b_get_stack(stack) do{unsigned sp; *(stack)=(unsigned long)&sp;} while(0)

#define b_tick2ms_init() unsigned clock__=(g_running_clock.clock_freq/10000);
#define b_tick2_100us(tick) ((tick)/(clock__))
#elif defined(LINUX) && defined(__KERNEL__) 
#include "bperf_counter.h"
#if B_PERF_BMIPS3300
/* linux kernel adjusts MIPS C0 counter, therefore it's not monotonic counter and not good source for interval measurements */
/* for BCM MIPS300 we could use 4-th performance counter, and it shall be mapped to the MIPS cycle counter */
/*#define b_gettick(void) (~(b_perf_read_one(3))) */
#endif
#include "bprofile_tick_mips.h"
#include "batomic.h"
/* in the Linux there is no dedicated task for the interrupt handler, therefore special trick is required to mark the interrupt context */
extern batomic_t b_in_isr; /* this is mask, where the most significant bit is cleared/set in the entry/exit point of the ISR routine */
BSTD_INLINE void __attribute__((no_instrument_function))
b_get_stack(unsigned long *stack) 
{
	unsigned sp; 
	*(stack)=((unsigned long)batomic_get(&b_in_isr))&(unsigned long)&sp;
}

/* this variable shall be initialized to number of ticks in 100us */ 
extern unsigned  b_ticks_100us;
#define b_tick2ms_init() unsigned clock__=b_ticks_100us
#define b_tick2_100us(tick) ((tick)/(clock__))
#else
#warning "Not supported"
#define b_get_stack(stack) do{unsigned sp; *(stack)=(unsigned long)&sp;} while(0)
#define b_sample(s) (void)
#define b_tick2ms_init() (void)
#define b_tick2_100us(tick) (tick)
#endif

#define b_tick2ms(n) (b_tick2_100us(n)/10)


