/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bperf_counter.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 1/4/12 3:06p $
 *
 * Module Description:
 *
 * Perfomance counter module
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bprofile/bperf_counter.h $
 * 
 * 23   1/4/12 3:06p vsilyaev
 * SW7425-2039: Read proper counter in the user mode
 * 
 * 22   12/22/11 4:55p vsilyaev
 * SW7425-2039: Added more BMIPS5000 specific counters
 * 
 * 21   12/21/11 3:56p mward
 * SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
 * 
 * 20   12/21/11 2:01p vsilyaev
 * SW7425-2039: Use BDBG_LOG instead of BKNI_Printf
 * 
 * 19   12/20/11 3:24p vsilyaev
 * SW7425-2039: Added support for BMIPS5000
 * 
 * 18   2/26/10 10:34a cnovak
 * SW7325-695: Add 7325 (MIPS 34K), same as 7038 (MIPS R5K).
 * 
 * 17   1/20/10 1:03p vsilyaev
 * SW7408-17: Added 7408 support
 * 
 * 16   9/10/08 4:18p erickson
 * PR45093: add 3548 and 3556 support
 *
 * 15   5/30/08 7:35p vsilyaev
 * PR 42356: Updated code to make it safe for 64-bit environment
 *
 * 14   5/2/08 5:09p vsilyaev
 * PR 42420: Added virtual performance counter
 *
 * 13   4/9/08 2:40p vsilyaev
 * PR 41083: Added linux user performance sampling for BMIPS4380
 *
 * 12   4/8/08 10:38a vsilyaev
 * PR 41083: Fixed linux user mode support
 *
 * 11   11/9/07 1:41p hongtaoz
 * PR36874: fixed BMIPS-4380 performance counters profiling; fixed RAC
 * performance counters config on BMIPS-3300;
 *
 * 10   7/30/07 5:54p vsilyaev
 * PR 32813: Updated to work x86/x86-64 plaforms
 *
 * 9   3/28/07 5:42p vsilyaev
 * PR 26928: Adding 7400 support
 *
 * 7   1/15/07 10:50a vsilyaev
 * PR 25997: Added 7038 performance counters
 *
 * 6   12/22/06 12:03p vsilyaev
 * PR 26792: Added rac_access, rac_prefetch and rac_hits configurations
 * for the peformance counter
 *
 * 5   12/14/06 4:38p vsilyaev
 * PR 25997: Added counter configuration to capture issue rate
 *
 * 4   12/8/06 7:24p vsilyaev
 * PR 25997: Fixed warning
 *
 * 3   12/7/06 2:44p vsilyaev
 * PR 25997: Added fixes for 3.4 GCC compiler
 *
 * 2   12/5/06 11:08a vsilyaev
 * PR 25997: Improved perf counter interface
 *
 * 1   12/1/06 5:58p vsilyaev
 * PR 25997: CPU perfomance counter interface
 *
 *
 *******************************************************************************/
#ifndef __BPERF_COUNTER_H__
#define __BPERF_COUNTER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if BCHP_CHIP==7403 || BCHP_CHIP==7401 || BCHP_CHIP==3560 || BCHP_CHIP==3563 || BCHP_CHIP==3543 || BCHP_CHIP==7002 || BCHP_CHIP==7208 || BCHP_CHIP==7356 || BCHP_CHIP==7468 || BCHP_CHIP==7550 || BCHP_CHIP==7552
#define B_PERF_BMIPS3300 1
#elif BCHP_CHIP==7400 || BCHP_CHIP==7405 || BCHP_CHIP==7335 || BCHP_CHIP==3548 || BCHP_CHIP==3556 || BCHP_CHIP==7408 || BCHP_CHIP==35230 || BCHP_CHIP==7106 || BCHP_CHIP==7125 || BCHP_CHIP==7231 || BCHP_CHIP==7335 || BCHP_CHIP==7336 || BCHP_CHIP==7340 || BCHP_CHIP==7342 || BCHP_CHIP==7413
#define B_PERF_BMIPS4380 1
#elif BCHP_CHIP==7038
#define B_PERF_MIPSR5K	1
#elif BCHP_CHIP==7325
#define B_PERF_MIPS34K	1
#elif BCHP_CHIP==7344 || BCHP_CHIP==7346 || BCHP_CHIP==7420 || BCHP_CHIP==7422 || BCHP_CHIP==7425 || BCHP_CHIP==7428 || BCHP_CHIP==7435
#define B_PERF_BMIPS5000   1 
#else
#define B_PERF_NOT_SUPPORTED 1
#endif

#if (defined(LINUX) || defined(__linux__)) && !defined(__KERNEL__)
#define B_PERF_LINUX    1
#undef B_PERF_MIPSR5K
#undef B_PERF_BMIPS3300
#undef B_PERF_MIPS34K
#endif


#if B_PERF_BMIPS3300
#define BPERF_N_COUNTERS	4
#define BPERF_SAMPLE_INITIALIZER {{0,0,0,0}}


#define b_perf_read_one(sel)	__extension__			\
({ unsigned int b_perf_read_res;						\
		__asm__ __volatile__(".set\tpush\n\t"			\
			".set\tmips32\n\t"							\
			"mfc0\t%0, $25, " #sel "\n\t"			\
			".set\tpop\n\t"							\
			: "=r" (b_perf_read_res));					\
	b_perf_read_res;									\
})

BSTD_INLINE unsigned bperf_sample_diff(unsigned stop, unsigned start)
{
	return start-stop; /* perfomance timers are count down */
}

#elif B_PERF_BMIPS4380
#define BPERF_N_COUNTERS	4
#define BPERF_SAMPLE_INITIALIZER {{0,0,0,0}}

#define B_PERF_BASE_ADDR    0x11F20000ul
#define B_PERF_BASE_LEN     256

#ifndef b_perf_4380_base
/* MIPS kernel mode mapping */
#define b_perf_4380_base  ((volatile uint32_t *)(B_PERF_BASE_ADDR|0xA0000000ul))
#endif
#define b_perf_read_one(sel)	b_perf_4380_base[4+sel]

BSTD_INLINE unsigned bperf_sample_diff(unsigned stop, unsigned start)
{
	return start-stop; /* perfomance timers are count down */
}

#elif defined(B_PERF_MIPSR5K) || defined(B_PERF_MIPS34K)
#define BPERF_N_COUNTERS	2
#define BPERF_SAMPLE_INITIALIZER {{0,0}}
#define b_perf_read_one(sel)	__extension__			\
({ unsigned int b_perf_read_res;						\
		__asm__ __volatile__(".set\tpush\n\t"			\
			".set\tmips32\n\t"							\
			"mfc0\t%0, $25, " #sel "\n\t"			\
			".set\tpop\n\t"							\
			: "=r" (b_perf_read_res));					\
	b_perf_read_res;									\
})

BSTD_INLINE unsigned bperf_sample_diff(unsigned stop, unsigned start)
{
	return stop-start;
}
#elif defined(B_PERF_BMIPS5000)
#define BPERF_N_COUNTERS	5
#define BPERF_SAMPLE_INITIALIZER {{0,0,0,0,0}}

#if (defined(LINUX) || defined(__linux__)) && !defined(__KERNEL__)
#define b_perf_write_cfg b_perf_write_cfg
extern void b_perf_write_cfg(unsigned select, unsigned data);
/* using RDHWR for the Linux user mode */
#define b_perf_read_one_rdhwr(sel)	__extension__			\
({ unsigned int b_perf_read_res;						\
		__asm__ __volatile__(".set\tpush\n\t"			\
			".set\tmips32r2\n\t"							\
			"rdhwr \t%0, $" #sel "\n\t"			\
			".set\tpop\n\t"							\
			: "=r" (b_perf_read_res));					\
	b_perf_read_res;									\
})
#define b_perf_read_one(sel) ((sel==0)?b_perf_read_one_rdhwr(24):((sel==1)?b_perf_read_one_rdhwr(25):((sel==2)?b_perf_read_one_rdhwr(26):((sel==3)?b_perf_read_one_rdhwr(27):b_perf_read_one_rdhwr(2)))))
#else 
/* othwewise use Coprocessor 0 instructions */                                                                                                                                       
#define b_perf_read_one_cp0(sel)	__extension__			\
({ unsigned int b_perf_read_res;						\
		__asm__ __volatile__(".set\tpush\n\t"			\
			".set\tmips32\n\t"							\
			"mfc0\t%0, $25, " #sel "\n\t"			\
			".set\tpop\n\t"							\
			: "=r" (b_perf_read_res));					\
	b_perf_read_res;									\
})

#define b_perf_read_tick()	__extension__			\
({ unsigned int b_perf_read_res;						\
		__asm__ __volatile__(".set\tpush\n\t"			\
			".set\tmips32\n\t"							\
			"mfc0\t%0, $9, 0\n\t"			\
			".set\tpop\n\t"							\
			: "=r" (b_perf_read_res));					\
	b_perf_read_res;									\
})

#define b_perf_read_one(sel) ((sel==0)?b_perf_read_one_cp0(1):((sel==1)?b_perf_read_one_cp0(3):((sel==2)?b_perf_read_one_cp0(5):((sel==3)?b_perf_read_one_cp0(7):b_perf_read_tick()))))
#endif

BSTD_INLINE unsigned bperf_sample_diff(unsigned stop, unsigned start)
{
	return ((stop<<1)-(start<<1))>>1; /* HW doesn't update most significant bit */
}

#elif B_PERF_LINUX
#define BPERF_N_COUNTERS	1
#define BPERF_SAMPLE_INITIALIZER {{0}}

BSTD_INLINE unsigned bperf_sample_diff(unsigned stop, unsigned start)
{
	return stop-start;
}
extern unsigned b_perf_read_time(void);
#define b_perf_read_one(sel)	b_perf_read_time()

#else /* BPEFF_ */
BSTD_INLINE unsigned bperf_sample_diff(unsigned stop, unsigned start)
{
	return stop-start;
}

#define BPERF_N_COUNTERS	1
#define BPERF_SAMPLE_INITIALIZER {{0}}
#endif /* BPERF_ */

typedef struct bperf_counter_mode {
	const char *counter_names[BPERF_N_COUNTERS];
	unsigned config[4]; /* opaque HW configuration */
} bperf_counter_mode;

typedef struct bperf_sample {
	unsigned data[BPERF_N_COUNTERS];
} bperf_sample;


extern const  bperf_counter_mode bperf_counter_dcache;
extern const  bperf_counter_mode bperf_counter_icache;
extern const  bperf_counter_mode bperf_counter_instructions;
#if  B_PERF_BMIPS3300 || B_PERF_BMIPS4380
#define BPERF_COUNTER_HAS_RAC 1
extern const  bperf_counter_mode bperf_counter_rac_access;
extern const  bperf_counter_mode bperf_counter_rac_prefetch;
extern const  bperf_counter_mode bperf_counter_rac_hit;
#endif
#if B_PERF_BMIPS5000
#define bperf_counter_idle bperf_counter_idle
extern const  bperf_counter_mode bperf_counter_idle;
#define bperf_counter_memory bperf_counter_memory
extern const  bperf_counter_mode bperf_counter_memory;
#define bperf_counter_memory_cycles bperf_counter_memory_cycles
extern const  bperf_counter_mode bperf_counter_memory_cycles;
#endif

int b_perf_init(const bperf_counter_mode *mode);


const bperf_counter_mode *bperf_get_mode(void);

void bperf_print(const bperf_counter_mode *mode, const bperf_sample *stop, const bperf_sample *start);

BSTD_INLINE void __attribute__((no_instrument_function))
b_perf_read(bperf_sample *sample)
{
#if  BPERF_N_COUNTERS >= 4
    unsigned data0, data1, data2, data3;
#if  BPERF_N_COUNTERS >= 5
    unsigned data4;
#endif
    data0 = b_perf_read_one(0);
	data1 = b_perf_read_one(1);
	data2 = b_perf_read_one(2);
	data3 = b_perf_read_one(3);
#if  BPERF_N_COUNTERS >= 5
	data4 = b_perf_read_one(4);
#endif
	sample->data[0] = data0;
	sample->data[1] = data1;
	sample->data[2] = data2;
	sample->data[3] = data3;
#if  BPERF_N_COUNTERS >= 5
	sample->data[4] = data4;
#endif
#elif defined(B_PERF_MIPSR5K) || defined(B_PERF_MIPS34K)
    unsigned data0, data1;
    data0 = b_perf_read_one(1);
	data1 = b_perf_read_one(3);
	sample->data[0] = data0;
	sample->data[1] = data1;
#elif B_PERF_LINUX
	sample->data[0] = b_perf_read_one();
#else
	BSTD_UNUSED(sample);
#endif
	return;
}

#ifndef bperf_read
#define bperf_read b_perf_read
#endif

#ifndef bperf_init
#define bperf_init b_perf_init
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BPERF_COUNTER_H__ */

