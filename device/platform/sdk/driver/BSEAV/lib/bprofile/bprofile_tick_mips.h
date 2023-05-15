/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bprofile_tick_mips.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 12/22/11 2:30p $
 *
 * Module Description:
 *
 * Embeddeble profiler library
 * 		MIPS Data acquisition module
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bprofile/bprofile_tick_mips.h $
 * 
 * 7   12/22/11 2:30p vsilyaev
 * SW7425-2039: Added support for 7425
 * 
 * 6   6/12/08 2:48p vsilyaev
 * PR 42356: Updated to work with emulated performance counters
 * 
 * 5   5/2/08 5:12p vsilyaev
 * PR 42420: Pipelined perf counter reads
 * 
 * 4   4/9/08 2:40p vsilyaev
 * PR 41083: Added linux user performance sampling for BMIPS4380
 * 
 * 3   11/9/07 1:41p hongtaoz
 * PR36874: fixed BMIPS-4380 performance counters profiling; fixed RAC
 * performance counters config on BMIPS-3300;
 * 
 * 2   1/15/07 10:50a vsilyaev
 * PR 25997: Added 7038 performance counters
 * 
 * 1   12/13/06 7:48p vsilyaev
 * PR 25997: Generic MIPS profile sampler
 * 
 *******************************************************************************/
/* MIPS32 performance sampling routine */

#ifndef b_gettick
BSTD_INLINE unsigned __attribute__((no_instrument_function))
b_gettick(void)
{ 
	unsigned b_mips_tick;
	__asm__ __volatile__(
			".set\tmips32\n\t"
			"mfc0\t%0, $9, 0\n\t"
			".set\tpop\n\t"
			: "=r" (b_mips_tick));
	return b_mips_tick;				
}
#endif

BSTD_INLINE void __attribute__((no_instrument_function))
b_sample(bprofile_sample *sample)
{
        unsigned time;
#if BPROFILE_CFG_PERF_COUNTER
        unsigned data[BPROFILE_CFG_PERF_COUNTER];
#endif
        time = b_gettick();

#if BPROFILE_CFG_PERF_COUNTER
#if BPROFILE_CFG_PERF_COUNTER <= BPERF_N_COUNTERS
		data[0] = b_perf_read_one(0);
#if BPROFILE_CFG_PERF_COUNTER > 1
		data[1] = b_perf_read_one(1);
#endif
#if BPROFILE_CFG_PERF_COUNTER > 2
		data[2] = b_perf_read_one(2);
#endif
#if BPROFILE_CFG_PERF_COUNTER > 3
		data[3] = b_perf_read_one(3);
#endif
#else /* BPERF_N_COUNTERS <= BPROFILE_CFG_PERF_COUNTER */
#error "Not supported"
#endif
#endif /* BPROFILE_CFG_PERF_COUNTER */
		sample->time = time;
#if BPROFILE_CFG_PERF_COUNTER
		sample->counters[0] = data[0];
# if BPROFILE_CFG_PERF_COUNTER > 1
		sample->counters[1] = data[1];
# endif
# if BPROFILE_CFG_PERF_COUNTER > 2
		sample->counters[2] = data[2];
# endif
# if BPROFILE_CFG_PERF_COUNTER > 2
		sample->counters[3] = data[3];
# endif
#endif
		return;
}

