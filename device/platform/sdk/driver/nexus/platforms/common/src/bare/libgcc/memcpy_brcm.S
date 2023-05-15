/* Copyright (C) 2002, 2003 Free Software Foundation, Inc.
   Copyright (C) 2010 Broadcom Corporation

   This file is part of the GNU C Library.
   Contributed by Hartvig Ekner <hartvige@mips.com>, 2002.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */
#include "regdef.h"
#include "asm.h"

#ifdef __mips64
#error mips32 code being compiled for mips64!
#endif

/* void *memcpy(void *s1, const void *s2, size_t n);  */

#if defined(__MIPSEB__)
#error big-endian is not supported in Broadcom MIPS Android platform
#  define LWHI	lwl		/* high part is left in big-endian	*/
#  define SWHI	swl		/* high part is left in big-endian	*/
#  define LWLO	lwr		/* low part is right in big-endian	*/
#  define SWLO	swr		/* low part is right in big-endian	*/
#else
#  define LWHI	lwr		/* high part is right in little-endian	*/
#  define SWHI	swr		/* high part is right in little-endian	*/
#  define LWLO	lwl		/* low part is left in little-endian	*/
#  define SWLO	swl		/* low part is left in little-endian	*/
#endif

#define CPUTYPE_4380_LO		0x0002a040
#define CPUTYPE_4380_HI		0x0002a06f

#define CPUTYPE_5000		0x00025a00
#define CPUMASK_5000		0xffffff00

#ifdef __PIC__
	.option	pic2
#endif

	.text

LEAF (memcpy)
	.set	noreorder
#ifdef __PIC__
	.cpload	t9
#endif

#undef L
#define	L(x)	__BMIPS4380_memcpy_##x

	slti	t0, a2, 8		# Less than 8 bytes?		
	bne	t0, zero, L(last8ByteCopy) #  Yes, proceed to process 8 bytes.
	move	v0, a0			# setup exit value before too late

	xor	t0, a1, a0		# find a0/a1 displacement
	andi	t0, 0x3
	beq	t0, zero, L(wordAlign)     # go handle the word-aligned case
	subu	t1, zero, a1
	b	L(unAlignSrcDest)
	subu	a3, zero, a0

	/*********************************************************************
	 * SRC and DEST are Word-Aligned.
	 *********************************************************************/
L(wordAlign):
	andi	t1, 0x3		# a0/a1 are aligned, but r we
	beq	t1, zero, L(intCheck8w) #  starting in middle of a word?
	subu	a2, t1

	LWHI	t0, 0(a1)	# src is in the middle of a word...
	addu	a1, t1
	SWHI	t0, 0(a0)
	addu	a0, t1

L(intCheck8w): 			# SRC is at begin of word
	andi	t0, a2, 0x1ff	# 512 or more bytes left ?
	beq	t0, a2, L(check4w)	#   NO, less than 512, proceed to process 4w/16B
	subu	a3, a2, t0	#   Yes, more than 512, maybe we can use FPU copy

	addu	a3, a0		# a3 = end address of loop
	subu    a3, a3, 0x100
	.align 4
	move	a2, t0		# a2 = what will be left after loop

    	lw	t6,  0(a1)	# Loop taking 32 words at a time

	/*--------------------------------------------------------------------
	 * Integer Copy Loop
	 *--------------------------------------------------------------------*/
L(intLoopBack):
    	pref    30,  0x40(a0)
	lw	t5,  0x40(a1)

	lw	t2,  0x4(a1)
	lw	t3,  0x8(a1)
	lw	t4,  0xc(a1)
	sw	t6,  0x0(a0)
	sw	t2,  0x4(a0)
	sw	t3,  0x8(a0)
	sw	t4,  0xc(a0)

	lw	t1,  0x10(a1)
	lw	t2,  0x14(a1)
	lw	t3,  0x18(a1)
	lw	t4,  0x1c(a1)
	sw	t1,  0x10(a0)
	sw	t2,  0x14(a0)
	sw	t3,  0x18(a0)
	sw	t4,  0x1c(a0)

	lw	t1,  0x20(a1)
	lw	t2,  0x24(a1)
	lw	t3,  0x28(a1)
	lw	t4,  0x2c(a1)
	sw	t1,  0x20(a0)
	sw	t2,  0x24(a0)
	sw	t3,  0x28(a0)
	sw	t4,  0x2c(a0)

	lw	t1,  0x30(a1)
	lw	t2,  0x34(a1)
	lw	t3,  0x38(a1)
	lw	t4,  0x3c(a1)
	sw	t1,  0x30(a0)
	sw	t2,  0x34(a0)
	sw	t3,  0x38(a0)
	sw	t4,  0x3c(a0)

    	pref    30,  0x80(a0)
	lw	t6,  0x80(a1)

	lw	t2,  0x44(a1)
	lw	t3,  0x48(a1)
	lw	t4,  0x4c(a1)
	sw	t5,  0x40(a0)
	sw	t2,  0x44(a0)
	sw	t3,  0x48(a0)
	sw	t4,  0x4c(a0)

	lw	t1,  0x50(a1)
	lw	t2,  0x54(a1)
	lw	t3,  0x58(a1)
	lw	t4,  0x5c(a1)
	sw	t1,  0x50(a0)
	sw	t2,  0x54(a0)
	sw	t3,  0x58(a0)
	sw	t4,  0x5c(a0)

	lw	t1,  0x60(a1)
	lw	t2,  0x64(a1)
	lw	t3,  0x68(a1)
	lw	t4,  0x6c(a1)
	sw	t1,  0x60(a0)
	sw	t2,  0x64(a0)
	sw	t3,  0x68(a0)
	sw	t4,  0x6c(a0)

	lw	t1,  0x70(a1)
	lw	t2,  0x74(a1)
	lw	t3,  0x78(a1)
	lw	t4,  0x7c(a1)
	sw	t1,  0x70(a0)
	sw	t2,  0x74(a0)
	sw	t3,  0x78(a0)
	sw	t4,  0x7c(a0)

    	pref    30,  0xc0(a0)
	lw	t5,  0xc0(a1)

	lw	t2,  0x84(a1)
	lw	t3,  0x88(a1)
	lw	t4,  0x8c(a1)
	sw	t6,  0x80(a0)
	sw	t2,  0x84(a0)
	sw	t3,  0x88(a0)
	sw	t4,  0x8c(a0)

	lw	t1,  0x90(a1)
	lw	t2,  0x94(a1)
	lw	t3,  0x98(a1)
	lw	t4,  0x9c(a1)
	sw	t1,  0x90(a0)
	sw	t2,  0x94(a0)
	sw	t3,  0x98(a0)
	sw	t4,  0x9c(a0)

	lw	t1,  0xa0(a1)
	lw	t2,  0xa4(a1)
	lw	t3,  0xa8(a1)
	lw	t4,  0xac(a1)
	sw	t1,  0xa0(a0)
	sw	t2,  0xa4(a0)
	sw	t3,  0xa8(a0)
	sw	t4,  0xac(a0)

	lw	t1,  0xb0(a1)
	lw	t2,  0xb4(a1)
	lw	t3,  0xb8(a1)
	lw	t4,  0xbc(a1)
	sw	t1,  0xb0(a0)
	sw	t2,  0xb4(a0)
	sw	t3,  0xb8(a0)
	sw	t4,  0xbc(a0)

    	pref    30,  0x100(a0)
	lw	t6,  0x100(a1)

	lw	t2,  0xc4(a1)
	lw	t3,  0xc8(a1)
	lw	t4,  0xcc(a1)
	sw	t5,  0xc0(a0)
	sw	t2,  0xc4(a0)
	sw	t3,  0xc8(a0)
	sw	t4,  0xcc(a0)

	lw	t1,  0xd0(a1)
	lw	t2,  0xd4(a1)
	lw	t3,  0xd8(a1)
	lw	t4,  0xdc(a1)
	sw	t1,  0xd0(a0)
	sw	t2,  0xd4(a0)
	sw	t3,  0xd8(a0)
	sw	t4,  0xdc(a0)

	lw	t1,  0xe0(a1)
	lw	t2,  0xe4(a1)
	lw	t3,  0xe8(a1)
	lw	t4,  0xec(a1)
	sw	t1,  0xe0(a0)
	sw	t2,  0xe4(a0)
	sw	t3,  0xe8(a0)
	sw	t4,  0xec(a0)

	lw	t1,  0xf0(a1)
	lw	t2,  0xf4(a1)
	lw	t3,  0xf8(a1)
	lw	t4,  0xfc(a1)
	sw	t1,  0xf0(a0)
	sw	t2,  0xf4(a0)
	sw	t3,  0xf8(a0)
	sw	t4,  0xfc(a0)

	add     a0, a0, 0x100
	bne	a0, a3, L(intLoopBack)
        add     a1, a1, 0x100

        lw      t2,  0x4(a1)
        lw      t3,  0x8(a1)
        lw      t4,  0xc(a1)
        sw      t6,  0x0(a0)
        sw      t2,  0x4(a0)
        sw      t3,  0x8(a0)
        sw      t4,  0xc(a0)

        lw      t1,  0x10(a1)
        lw      t2,  0x14(a1)
        lw      t3,  0x18(a1)
        lw      t4,  0x1c(a1)
        sw      t1,  0x10(a0)
        sw      t2,  0x14(a0)
        sw      t3,  0x18(a0)
        sw      t4,  0x1c(a0)

        lw      t1,  0x20(a1)
        lw      t2,  0x24(a1)
        lw      t3,  0x28(a1)
        lw      t4,  0x2c(a1)
        sw      t1,  0x20(a0)
        sw      t2,  0x24(a0)
        sw      t3,  0x28(a0)
        sw      t4,  0x2c(a0)

        lw      t1,  0x30(a1)
        lw      t2,  0x34(a1)
        lw      t3,  0x38(a1)
        lw      t4,  0x3c(a1)
        sw      t1,  0x30(a0)
        sw      t2,  0x34(a0)
        sw      t3,  0x38(a0)
        sw      t4,  0x3c(a0)

        lw      t1,  0x40(a1)
        lw      t2,  0x44(a1)
        lw      t3,  0x48(a1)
        lw      t4,  0x4c(a1)
        sw      t1,  0x40(a0)
        sw      t2,  0x44(a0)
        sw      t3,  0x48(a0)
        sw      t4,  0x4c(a0)

        lw      t1,  0x50(a1)
        lw      t2,  0x54(a1)
        lw      t3,  0x58(a1)
        lw      t4,  0x5c(a1)
        sw      t1,  0x50(a0)
        sw      t2,  0x54(a0)
        sw      t3,  0x58(a0)
        sw      t4,  0x5c(a0)

        lw      t1,  0x60(a1)
        lw      t2,  0x64(a1)
        lw      t3,  0x68(a1)
        lw      t4,  0x6c(a1)
        sw      t1,  0x60(a0)
        sw      t2,  0x64(a0)
        sw      t3,  0x68(a0)
        sw      t4,  0x6c(a0)

        lw      t1,  0x70(a1)
        lw      t2,  0x74(a1)
        lw      t3,  0x78(a1)
        lw      t4,  0x7c(a1)
        sw      t1,  0x70(a0)
        sw      t2,  0x74(a0)
        sw      t3,  0x78(a0)
        sw      t4,  0x7c(a0)

        lw      t1,  0x80(a1)
        lw      t2,  0x84(a1)
        lw      t3,  0x88(a1)
        lw      t4,  0x8c(a1)
        sw      t1,  0x80(a0)
        sw      t2,  0x84(a0)
        sw      t3,  0x88(a0)
        sw      t4,  0x8c(a0)

        lw      t1,  0x90(a1)
        lw      t2,  0x94(a1)
        lw      t3,  0x98(a1)
        lw      t4,  0x9c(a1)
        sw      t1,  0x90(a0)
        sw      t2,  0x94(a0)
        sw      t3,  0x98(a0)
        sw      t4,  0x9c(a0)

        lw      t1,  0xa0(a1)
        lw      t2,  0xa4(a1)
        lw      t3,  0xa8(a1)
        lw      t4,  0xac(a1)
        sw      t1,  0xa0(a0)
        sw      t2,  0xa4(a0)
        sw      t3,  0xa8(a0)
        sw      t4,  0xac(a0)

        lw      t1,  0xb0(a1)
        lw      t2,  0xb4(a1)
        lw      t3,  0xb8(a1)
        lw      t4,  0xbc(a1)
        sw      t1,  0xb0(a0)
        sw      t2,  0xb4(a0)
        sw      t3,  0xb8(a0)
        sw      t4,  0xbc(a0)

        lw      t1,  0xc0(a1)
        lw      t2,  0xc4(a1)
        lw      t3,  0xc8(a1)
        lw      t4,  0xcc(a1)
        sw      t1,  0xc0(a0)
        sw      t2,  0xc4(a0)
        sw      t3,  0xc8(a0)
        sw      t4,  0xcc(a0)

        lw      t1,  0xd0(a1)
        lw      t2,  0xd4(a1)
        lw      t3,  0xd8(a1)
        lw      t4,  0xdc(a1)
        sw      t1,  0xd0(a0)
        sw      t2,  0xd4(a0)
        sw      t3,  0xd8(a0)
        sw      t4,  0xdc(a0)

        lw      t1,  0xe0(a1)
        lw      t2,  0xe4(a1)
        lw      t3,  0xe8(a1)
        lw      t4,  0xec(a1)
        sw      t1,  0xe0(a0)
        sw      t2,  0xe4(a0)
        sw      t3,  0xe8(a0)
        sw      t4,  0xec(a0)

        lw      t1,  0xf0(a1)
        lw      t2,  0xf4(a1)
        lw      t3,  0xf8(a1)
        lw      t4,  0xfc(a1)
        sw      t1,  0xf0(a0)
        sw      t2,  0xf4(a0)
        sw      t3,  0xf8(a0)
        sw      t4,  0xfc(a0)

	add	a1, a1, 0x100
	add	a0, a0, 0x100

	/*--------------------------------------------------------------------
	 * copy if >16 and <512 bytes left-over
	 *--------------------------------------------------------------------*/
L(check4w): andi    t0, a2, 0xf		# 16 or more bytes left?
        beq     t0, a2, L(check1w)		#   NO, less than 16, proceed to check1w (4bytes loop)
        subu    a3, a2, t0              #   Yes, handle them in 16 bytes loop.

        addu    a3, a1                  # a3 = end address.
        move    a2, t0

L(loop4w):	lw	t0, 0(a1)		# loop for 16 bytes/4 words at a time.
	lw	t1, 4(a1)
	lw	t2, 8(a1)
	lw	t3, 0xc(a1)
	sw	t0, 0(a0)
	sw	t1, 4(a0)
	sw	t2, 8(a0)
	addiu	a0, 16
	addiu	a1, 16
	bne	a1, a3, L(loop4w)
	sw	t3, -4(a0)

L(check1w): andi    t0, a2, 0x3		# 4 or more bytes left?
        beq     t0, a2, L(last8ByteCopy)	#   NO, less than 4 bytes, proceed to process 3 bytes
        subu    a3, a2, t0              #   Yes, handle them 1 word at a time
        addu    a3, a1                  # a3 = end address.
        move    a2, t0

L(loop1w):	lw	t0, 0(a1)		# loop 4 bytes/1 word at a time.
	addiu	a0, 4
	addiu	a1, 4
	bne	a1, a3, L(loop1w)
	sw	t0, -4(a0)

L(last8ByteCopy):	blez	a2, L(last8BCExit)	# handle last 8 bytes, one byte at a time.
	addu	a3, a2, a1

L(last8BCLoopBack): lb	t0, 0(a1)	# last 8 bytes copy loop.
	addiu	a0, 1
	addiu	a1, 1
	bne	a1, a3, L(last8BCLoopBack)
	sb	t0, -1(a0)

L(last8BCExit):	
	jr	$31			# return to caller.
	nop



	/*********************************************************************
	 * SRC and DEST are NOT Aligned.
	 *********************************************************************/
L(unAlignSrcDest):				# SRC and DEST are NOT aligned.
	andi	a3, 0x3			# Is DEST word aligned?
	beq	a3, zero, L(uaCheck512)	#   YES, DEST is word-aligned, SW may be used.
					#   NO, DEST is NOT word-aligned, has to adjust.

	subu	a2, a3			# a2 = number of bytes left

	LWHI	t0, 0(a1)		# DEST is NOT word aligned...
	LWLO	t0, 3(a1)		#   adjust so DEST will be aligned.
	addu	a1, a3
	SWHI	t0, 0(a0)
	addu	a0, a3
L(uaCheck512):				# DEST is word-aligned.
	andi	t0, a2, 0x1ff		# 512 or more bytes left ?
	beq	t0, a2, L(uaCheck4w)	#   No, less than 512, cannot execute "pref"
	subu	a3, a2, t0		#   Yes, more than 512, loop & "pref"    

	addu	a3, a0			# a3 = end address of loop
	subu    a3, a3, 0x100
    	.align 4
	move	a2, t0			# a2 = what will be left after loop
    	LWHI	t6,  0(a1)		# Loop taking 32 words at a time

	/*--------------------------------------------------------------------
	 * SRC and DEST are NOT Aligned, >512B, copy using LW/SW WITH pref
	 *--------------------------------------------------------------------*/
	add	t7, a0, 0x300		# prefetch dest 2 line size ahead.
L(uaLoopBack):	
    	pref    30,  0x40(a0)
	LWHI    t5,  0x40(a1)

	LWHI	t2,  0x4(a1)
	LWHI	t3,  0x8(a1)
	LWHI	t4,  0xc(a1)

    	LWLO	t6,  3(a1)
	LWLO	t2,  0x7(a1)
	LWLO	t3,  0xb(a1)
	LWLO	t4,  0xf(a1)

	sw	t6,  0x0(a0)
	sw	t2,  0x4(a0)
	sw	t3,  0x8(a0)
	sw	t4,  0xc(a0)

	# preload source
        bge     t7, a3, L(uaSkip)
	add	t7, t7, 0x100
        lb      zero, 0x300(a1)
L(uaSkip):
	LWHI	t1,  0x10(a1)
	LWHI	t2,  0x14(a1)
	LWHI	t3,  0x18(a1)
	LWHI	t4,  0x1c(a1)
	LWLO	t1,  0x13(a1)
	LWLO	t2,  0x17(a1)
	LWLO	t3,  0x1b(a1)
	LWLO	t4,  0x1f(a1)

	sw	t1,  0x10(a0)
	sw	t2,  0x14(a0)
	sw	t3,  0x18(a0)
	sw	t4,  0x1c(a0)

	LWHI	t1,  0x20(a1)
	LWHI	t2,  0x24(a1)
	LWHI	t3,  0x28(a1)
	LWHI	t4,  0x2c(a1)
	LWLO	t1,  0x23(a1)
	LWLO	t2,  0x27(a1)
	LWLO	t3,  0x2b(a1)
	LWLO	t4,  0x2f(a1)

	sw	t1,  0x20(a0)
	sw	t2,  0x24(a0)
	sw	t3,  0x28(a0)
	sw	t4,  0x2c(a0)

	LWHI	t1,  0x30(a1)
	LWHI	t2,  0x34(a1)
	LWHI	t3,  0x38(a1)
	LWHI	t4,  0x3c(a1)
	LWLO	t1,  0x33(a1)
	LWLO	t2,  0x37(a1)
	LWLO	t3,  0x3b(a1)
	LWLO	t4,  0x3f(a1)

	sw	t1,  0x30(a0)
	sw	t2,  0x34(a0)
	sw	t3,  0x38(a0)
	sw	t4,  0x3c(a0)

    	pref    30,  0x80(a0)
	LWHI    t6,  0x80(a1)

	LWHI	t2,  0x44(a1)
	LWHI	t3,  0x48(a1)
	LWHI	t4,  0x4c(a1)
    	LWLO	t5,  0x43(a1)
	LWLO	t2,  0x47(a1)
	LWLO	t3,  0x4b(a1)
	LWLO	t4,  0x4f(a1)

	sw	t5,  0x40(a0)
	sw	t2,  0x44(a0)
	sw	t3,  0x48(a0)
	sw	t4,  0x4c(a0)

	LWHI	t1,  0x50(a1)
	LWHI	t2,  0x54(a1)
	LWHI	t3,  0x58(a1)
	LWHI	t4,  0x5c(a1)
	LWLO	t1,  0x53(a1)
	LWLO	t2,  0x57(a1)
	LWLO	t3,  0x5b(a1)
	LWLO	t4,  0x5f(a1)

	sw	t1,  0x50(a0)
	sw	t2,  0x54(a0)
	sw	t3,  0x58(a0)
	sw	t4,  0x5c(a0)

	LWHI	t1,  0x60(a1)
	LWHI	t2,  0x64(a1)
	LWHI	t3,  0x68(a1)
	LWHI	t4,  0x6c(a1)
	LWLO	t1,  0x63(a1)
	LWLO	t2,  0x67(a1)
	LWLO	t3,  0x6b(a1)
	LWLO	t4,  0x6f(a1)

	sw	t1,  0x60(a0)
	sw	t2,  0x64(a0)
	sw	t3,  0x68(a0)
	sw	t4,  0x6c(a0)

	LWHI	t1,  0x70(a1)
	LWHI	t2,  0x74(a1)
	LWHI	t3,  0x78(a1)
	LWHI	t4,  0x7c(a1)
	LWLO	t1,  0x73(a1)
	LWLO	t2,  0x77(a1)
	LWLO	t3,  0x7b(a1)
	LWLO	t4,  0x7f(a1)

	sw	t1,  0x70(a0)
	sw	t2,  0x74(a0)
	sw	t3,  0x78(a0)
	sw	t4,  0x7c(a0)

    	pref    30,  0xc0(a0)
	LWHI    t5,  0xc0(a1)

	LWHI	t2,  0x84(a1)
	LWHI	t3,  0x88(a1)
	LWHI	t4,  0x8c(a1)
    	LWLO	t6,  0x83(a1)
	LWLO	t2,  0x87(a1)
	LWLO	t3,  0x8b(a1)
	LWLO	t4,  0x8f(a1)

	sw	t6,  0x80(a0)
	sw	t2,  0x84(a0)
	sw	t3,  0x88(a0)
	sw	t4,  0x8c(a0)

	LWHI	t1,  0x90(a1)
	LWHI	t2,  0x94(a1)
	LWHI	t3,  0x98(a1)
	LWHI	t4,  0x9c(a1)
	LWLO	t1,  0x93(a1)
	LWLO	t2,  0x97(a1)
	LWLO	t3,  0x9b(a1)
	LWLO	t4,  0x9f(a1)

	sw	t1,  0x90(a0)
	sw	t2,  0x94(a0)
	sw	t3,  0x98(a0)
	sw	t4,  0x9c(a0)

	LWHI	t1,  0xa0(a1)
	LWHI	t2,  0xa4(a1)
	LWHI	t3,  0xa8(a1)
	LWHI	t4,  0xac(a1)
	LWLO	t1,  0xa3(a1)
	LWLO	t2,  0xa7(a1)
	LWLO	t3,  0xab(a1)
	LWLO	t4,  0xaf(a1)

	sw	t1,  0xa0(a0)
	sw	t2,  0xa4(a0)
	sw	t3,  0xa8(a0)
	sw	t4,  0xac(a0)

	LWHI	t1,  0xb0(a1)
	LWHI	t2,  0xb4(a1)
	LWHI	t3,  0xb8(a1)
	LWHI	t4,  0xbc(a1)
	LWLO	t1,  0xb3(a1)
	LWLO	t2,  0xb7(a1)
	LWLO	t3,  0xbb(a1)
	LWLO	t4,  0xbf(a1)

	sw	t1,  0xb0(a0)
	sw	t2,  0xb4(a0)
	sw	t3,  0xb8(a0)
	sw	t4,  0xbc(a0)

    	pref    30,  0x100(a0)
	LWHI    t6,  0x100(a1)

	LWHI	t2,  0xc4(a1)
	LWHI	t3,  0xc8(a1)
	LWHI	t4,  0xcc(a1)
    	LWLO	t5,  0xc3(a1)
	LWLO	t2,  0xc7(a1)
	LWLO	t3,  0xcb(a1)
	LWLO	t4,  0xcf(a1)

	sw	t5,  0xc0(a0)
	sw	t2,  0xc4(a0)
	sw	t3,  0xc8(a0)
	sw	t4,  0xcc(a0)

	LWHI	t1,  0xd0(a1)
	LWHI	t2,  0xd4(a1)
	LWHI	t3,  0xd8(a1)
	LWHI	t4,  0xdc(a1)
	LWLO	t1,  0xd3(a1)
	LWLO	t2,  0xd7(a1)
	LWLO	t3,  0xdb(a1)
	LWLO	t4,  0xdf(a1)

	sw	t1,  0xd0(a0)
	sw	t2,  0xd4(a0)
	sw	t3,  0xd8(a0)
	sw	t4,  0xdc(a0)

	LWHI	t1,  0xe0(a1)
	LWHI	t2,  0xe4(a1)
	LWHI	t3,  0xe8(a1)
	LWHI	t4,  0xec(a1)
	LWLO	t1,  0xe3(a1)
	LWLO	t2,  0xe7(a1)
	LWLO	t3,  0xeb(a1)
	LWLO	t4,  0xef(a1)

	sw	t1,  0xe0(a0)
	sw	t2,  0xe4(a0)
	sw	t3,  0xe8(a0)
	sw	t4,  0xec(a0)

	LWHI	t1,  0xf0(a1)
	LWHI	t2,  0xf4(a1)
	LWHI	t3,  0xf8(a1)
	LWHI	t4,  0xfc(a1)
	LWLO	t1,  0xf3(a1)
	LWLO	t2,  0xf7(a1)
	LWLO	t3,  0xfb(a1)
	LWLO	t4,  0xff(a1)

	sw	t1,  0xf0(a0)
	sw	t2,  0xf4(a0)
	sw	t3,  0xf8(a0)
	sw	t4,  0xfc(a0)

	add     a0, a0, 0x100
	bne		a0, a3, L(uaLoopBack)
	add     a1, a1, 0x100

	addu    a3, 0x100	# add 0x100 back

	#
	# copy loop 32 words at a time.
	#
L(uaRemain64LoopBack):
    	LWHI	t6,  0(a1)		# Loop taking 32 words at a time
	LWHI	t2,  0x4(a1)
	LWHI	t3,  0x8(a1)
	LWHI	t4,  0xc(a1)
    	LWLO	t6,  3(a1)
	LWLO	t2,  0x7(a1)
	LWLO	t3,  0xb(a1)
	LWLO	t4,  0xf(a1)

	sw	t6,  0x0(a0)
	sw	t2,  0x4(a0)
	sw	t3,  0x8(a0)
	sw	t4,  0xc(a0)

	LWHI	t6,  0x10(a1)
	LWHI	t2,  0x14(a1)
	LWHI	t3,  0x18(a1)
	LWHI	t4,  0x1c(a1)
	LWLO	t6,  0x13(a1)
	LWLO	t2,  0x17(a1)
	LWLO	t3,  0x1b(a1)
	LWLO	t4,  0x1f(a1)

	sw	t6,  0x10(a0)
	sw	t2,  0x14(a0)
	sw	t3,  0x18(a0)
	sw	t4,  0x1c(a0)

	addiu	a0,  0x20
	bne	a0,  a3, L(uaRemain64LoopBack)
	addiu	a1,  0x20

    	addu    a3,  a2

	/*--------------------------------------------------------------------
	 * SRC and DEST are NOT Aligned, <512B, copy using LW/SW WITHOUT pref
	 *--------------------------------------------------------------------*/
L(uaCheck4w): andi    t0, a2, 0xf		# 16 or more bytes left?
        beq     t0, a2, L(uaCheck1w)	#   NO, <16 bytes, proceed to process 1w
        subu    a3, a2, t0		#   Yes, >16, copy 16 bytes at a time.

        addu    a3, a1                  # a3 = end address.
        move    a2, t0

L(ua4wLoopBack):				# loop 16 bytes/4 words at a time.
        LWHI    t0, 0(a1)
        LWHI    t1, 4(a1)
        LWHI    t2, 8(a1)
        LWHI    t3, 0xc(a1)
        LWLO    t0, 3(a1)
        LWLO    t1, 7(a1)
        LWLO    t2, 0xb(a1)
        LWLO    t3, 0xf(a1)
        sw      t0, 0(a0)
        sw      t1, 4(a0)
        sw      t2, 8(a0)
        addiu   a0, 16
        addiu   a1, 16
        bne     a1, a3, L(ua4wLoopBack)
        sw      t3, -4(a0)

L(uaCheck1w): andi	t0, a2, 0x3		# 4 or more bytes left?
        beq     t0, a2, L(last8ByteCopy)	#   NO, <4 bytes, proceed to 8-bytes-copy
	subu	a3, a2, t0

	addu	a3, a0			#   YES, >4 bytes, can use LW/SW.

L(uaRemain):
	LWHI	t1, 0(a1)		# copy 1 word/4 bytes at a time.
	LWLO	t1, 3(a1)
	addiu	a0, 4
	addiu	a1, 4
	bne	a0, a3, L(uaRemain)
	sw	t1, -4(a0)

	b	L(last8ByteCopy)		# handle anything that may be left.
	move	a2, t0

	.set	reorder
END (memcpy)



