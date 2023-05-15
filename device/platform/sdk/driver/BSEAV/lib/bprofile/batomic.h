/***************************************************************************
 *     Copyright (c) 2006-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: batomic.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 7/30/07 5:54p $
 *
 * Module Description:
 *
 * Atomic operations
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bprofile/batomic.h $
 * 
 * 4   7/30/07 5:54p vsilyaev
 * PR 32813: Updated to work x86/x86-64 plaforms
 * 
 * 3   12/13/06 7:47p vsilyaev
 * PR 25997: Don't instrument atomic functions
 * 
 * 2   12/7/06 2:44p vsilyaev
 * PR 25997: Added fixes for 3.4 GCC compiler
 * 
 * 1   11/30/06 8:31p vsilyaev
 * PR 25997: Automic variable
 * 
 * 
 *******************************************************************************/
#ifndef __BATOMIC_H__
#define __BATOMIC_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct batomic_t {
	volatile long atomic;
} batomic_t;

#define BATOMIC_INIT(v) {(v)}

BSTD_INLINE void __attribute__((no_instrument_function))
batomic_set(batomic_t *a, long val) {
	a->atomic = val;
}

BSTD_INLINE long __attribute__((no_instrument_function))
batomic_get(const batomic_t *a) {
	return a->atomic;
}

BSTD_INLINE long __attribute__((no_instrument_function))
batomic_add_return(batomic_t *a, long val)
{
	unsigned long result;
#if defined(__mips__)
	unsigned long temp;
	__asm__ __volatile__(
		"	.set	mips32					\n"
		"1:	ll	%1, %2		# b_atomic_add_return\n"
		"	addu	%0, %1, %3				\n"
		"	sc	%0, %2					\n"
		"	beqz	%0, 1b					\n"
		"	addu	%0, %1, %3				\n"
		"	.set	mips0					\n"
		: "=&r" (result), "=&r" (temp), "=m" (a->atomic)
		: "Ir" (val), "m" (a->atomic)
		: "memory");
#else
	a->atomic += val;
	result = a->atomic;
#endif
	return result;
}


#ifdef __cplusplus
}
#endif

#endif /* __BATOMIC_H__ */
