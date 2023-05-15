############################################################
#     (c)2011 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile:  $
# $brcm_Revision:  $
# $brcm_Date:  $
#
# Module Description:
#   This file contains the stub implementation of the bare os calls.
#
# Revision History:
#
# Created: 06/03/2011 by Troy Trammel
#
# $brcm_Log:  $
# 
############################################################

.data
.align 4
.globl pReverse_Jumptable
pReverse_Jumptable: .space 4


#
# This is the firmware implementation of this function
# pReverse_Jumptable is set in the driver in nexus_driver_firmware.c
.text
.globl b_get_bare_os
b_get_bare_os:
lw $2,pReverse_Jumptable
j $31

# Start of stub functions
.globl malloc
.set noreorder
malloc:
lw $9,pReverse_Jumptable
lw $8,0($9)
j $8
nop
.set reorder

.globl free
.set noreorder
free:
lw $9,pReverse_Jumptable
lw $8,4($9)
j $8
nop
.set reorder

.globl print_debug
.set noreorder
print_debug:
lw $9,pReverse_Jumptable
lw $8,8($9)
j $8
nop
.set reorder

.globl current_tick
.set noreorder
current_tick:
lw $9,pReverse_Jumptable
lw $8,12($9)
j $8
nop
.set reorder

.globl tick_diff
.set noreorder
tick_diff:
lw $9,pReverse_Jumptable
lw $8,16($9)
j $8
nop
.set reorder

.globl mmap
.set noreorder
mmap:
lw $9,pReverse_Jumptable
lw $8,20($9)
j $8
nop
.set reorder

.globl unmmap
.set noreorder
unmmap:
lw $9,pReverse_Jumptable
lw $8,24($9)
j $8
nop
.set reorder

.globl dcache_flush
.set noreorder
dcache_flush:
lw $9,pReverse_Jumptable
lw $8,28($9)
j $8
nop
.set reorder

.globl delay
.set noreorder
delay:
lw $9,pReverse_Jumptable
lw $8,32($9)
j $8
nop
.set reorder

.globl sleep
.set noreorder
sleep:
lw $9,pReverse_Jumptable
lw $8,36($9)
j $8
nop
.set reorder

.globl lock_create
.set noreorder
lock_create:
lw $9,pReverse_Jumptable
lw $8,40($9)
j $8
nop
.set reorder

.globl lock_acquire
.set noreorder
lock_acquire:
lw $9,pReverse_Jumptable
lw $8,44($9)
j $8
nop
.set reorder

.globl lock_try_acquire
.set noreorder
lock_try_acquire:
lw $9,pReverse_Jumptable
lw $8,48($9)
j $8
nop
.set reorder

.globl lock_release
.set noreorder
lock_release:
lw $9,pReverse_Jumptable
lw $8,52($9)
j $8
nop
.set reorder

.globl lock_destroy
.set noreorder
lock_destroy:
lw $9,pReverse_Jumptable
lw $8,56($9)
j $8
nop
.set reorder

.globl signal_create
.set noreorder
signal_create:
lw $9,pReverse_Jumptable
lw $8,60($9)
j $8
nop
.set reorder

.globl signal_wait
.set noreorder
signal_wait:
lw $9,pReverse_Jumptable
lw $8,64($9)
j $8
nop
.set reorder

.globl signal_set
.set noreorder
signal_set:
lw $9,pReverse_Jumptable
lw $8,68($9)
j $8
nop
.set reorder

.globl signal_destroy
.set noreorder
signal_destroy:
lw $9,pReverse_Jumptable
lw $8,72($9)
j $8
nop
.set reorder

.globl copy_from_process
.set noreorder
copy_from_process:
lw $9,pReverse_Jumptable
lw $8,76($9)
j $8
nop
.set reorder

.globl copy_to_process
.set noreorder
copy_to_process:
lw $9,pReverse_Jumptable
lw $8,80($9)
j $8
nop
.set reorder

.globl thread_create
.set noreorder
thread_create:
lw $9,pReverse_Jumptable
lw $8,84($9)
j $8
nop
.set reorder

.globl thread_destroy
.set noreorder
thread_destroy:
lw $9,pReverse_Jumptable
lw $8,88($9)
j $8
nop
.set reorder

.globl getenv
.set noreorder
getenv:
lw $9,pReverse_Jumptable
lw $8,92($9)
j $8
nop
.set reorder

.globl setenv
.set noreorder
setenv:
lw $9,pReverse_Jumptable
lw $8,96($9)
j $8
nop
.set reorder

.globl terminate_process
.set noreorder
terminate_process:
lw $9,pReverse_Jumptable
lw $8,100($9)
j $8
nop
.set reorder

.globl atomic_update
.set noreorder
atomic_update:
lw $9,pReverse_Jumptable
lw $8,104($9)
j $8
nop
.set reorder

.globl connect_interrupt
.set noreorder
connect_interrupt:
lw $9,pReverse_Jumptable
lw $8,108($9)
j $8
nop
.set reorder

.globl enable_interrupt
.set noreorder
enable_interrupt:
lw $9,pReverse_Jumptable
lw $8,112($9)
j $8
nop
.set reorder

.globl disable_interrupt
.set noreorder
disable_interrupt:
lw $9,pReverse_Jumptable
lw $8,116($9)
j $8
nop
.set reorder

.globl disconnect_interrupt
.set noreorder
disconnect_interrupt:
lw $9,pReverse_Jumptable
lw $8,120($9)
j $8
nop
.set reorder

.globl get_interrupt_lock
.set noreorder
get_interrupt_lock:
lw $9,pReverse_Jumptable
lw $8,124($9)
j $8
nop
.set reorder
