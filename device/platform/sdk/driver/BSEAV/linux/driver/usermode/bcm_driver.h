/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bcm_driver.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 12/20/11 3:11p $
 *
 * Module Description:
 * User mode driver with 128 bit interrupt support.
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/usermode/bcm_driver.h $
 * 
 * 9   12/20/11 3:11p vsilyaev
 * SW7425-2039: Added ioctl to control perfomance counters
 * 
 * 8   10/24/11 11:22a erickson
 * SW7420-1690: add bcmdriver non-root mmap support
 * 
 * 7   12/22/10 12:25p gracew
 * SW7468-364: Added atomic structure to avoid hardcoding in brcm_ioctl()
 * for generic chips.
 * 
 * 6   9/1/10 3:07p gmohile
 * SWDEPRECATED-3811 : Add debug support using procfs for usermode
 * 
 * 5   2/4/10 5:36p jgarrett
 * SW7408-57: Adding TP1 hooks for soft audio
 * 
 * 4   9/23/08 3:43p erickson
 * PR46950: added BRCM_IOCTL_ATOMIC_UPDATE
 *
 * 3   9/23/08 8:56a erickson
 * PR47217: merge
 *
 * PR47217/1   9/23/08 2:05p dyzhang
 * PR47217: support get tick in bcm_driver
 *
 * 2   9/22/08 12:49p erickson
 * PR45432: added BRCM_IOCTL_GET_DCACHE_LINE_SIZE
 *
 * 1   11/21/07 1:19p katrep
 * PR37430: Added suppport for upto 128 bit interrupt
 * controller,reoganized the driver code.
 *
 *
 ***************************************************************************/
#ifndef BCMDRIVER_H
#define BCMDRIVER_H

#define BCM_INTC_MAX_SIZE   4    /* maximum interrupt controller size
                                    in words supported by this interface */

typedef struct s_bcm_linux_dd_interrupt
{
    int     timeout;
    unsigned char waitforallints;
    unsigned long interruptmask[BCM_INTC_MAX_SIZE];
    unsigned long interruptstatus[BCM_INTC_MAX_SIZE];

} tbcm_linux_dd_interrupt;

typedef struct s_bcm_linux_dd_ticks
{
    unsigned int jiffies;
    unsigned int timerFreq;
} tbcm_linux_dd_ticks;

typedef struct s_bcm_linux_dd_atomic_update
{
    uint32_t reg; /* register offset */
    uint32_t mask; /* set 1 for bits that will be modified */
    uint32_t value; /* value only applies for bits where mask is 1 */
} t_bcm_linux_dd_atomic_update;

/* all updates will happen in one atomic block with interrupts disabled */
#define BCM_MULTI_ATOMIC_UPDATE_NUM 4
typedef struct s_bcm_linux_dd_multi_atomic_update
{
    struct {
        uint32_t reg; /* register offset (use 0 for unused) */
        uint32_t mask; /* set 1 for bits that will be modified */
        uint32_t value; /* value only applies for bits where mask is 1 */
    } set[BCM_MULTI_ATOMIC_UPDATE_NUM];
} t_bcm_linux_dd_multi_atomic_update;

typedef struct s_bcm_linux_rw_reserved
{
    uint32_t address;       /* Physical Address */
    uint32_t value;         /* Value - out if read, in if write */
} t_bcm_linux_rw_reserved;

typedef struct s_bcm_linux_rw_core_register
{
    uint32_t offset;        /* Offset to core register from core base */
    uint32_t value;         /* Value - out if read, in if write */
} t_bcm_linux_rw_core_register;

#define BRCM_IOCTL_WAIT_FOR_INTERRUPTS      1011 /* use the tbcm_linux_dd_interrupt structure with this IOCTL */
#define BRCM_IOCTL_DEBUG_LEVEL              1012
#define BRCM_IOCTL_CHANGE_REQUEST           1013
#define BRCM_IOCTL_INT_RESET                1014
/* 1015 has been deprecated. The number can be reused later. */
#define BRCM_IOCTL_GET_TICKS                1016 /* use the tbcm_linux_dd_ticks structure with this IOCTL */
#define BRCM_IOCTL_GET_DCACHE_LINE_SIZE     1017
#define BRCM_IOCTL_ATOMIC_UPDATE            1018 /* use the t_bcm_linux_dd_atomic_update structure with this IOCTL */
#define BRCM_IOCTL_READ_RESERVED            1019 /* use the t_bcm_linux_rw_reserved structure with this IOCTL */
#define BRCM_IOCTL_WRITE_RESERVED           1020 /* use the t_bcm_linux_rw_reserved structure with this IOCTL */
#define BRCM_IOCTL_READ_CORE_REGISTER       1021 /* use the t_bcm_linux_rw_core_register structure with this IOCTL */
#define BRCM_IOCTL_WRITE_CORE_REGISTER      1022 /* use the t_bcm_linux_rw_core_register structure with this IOCTL */
#define BRCM_IOCTL_READ_CMT_CONTROL         1023 /* pass a uint32_t pointer that will be filled with the value of the register */
#define BRCM_IOCTL_WRITE_CMT_CONTROL        1024 /* pass a uint32_t value to be written to the register */
#define BRCM_IOCTL_GET_DEBUG                1025
#define BRCM_IOCTL_MULTI_ATOMIC_UPDATE      1026 /* use t_bcm_linux_dd_multi_atomic_update with this IOCTL */

#define BCM_MAX_HEAPS 16
typedef struct t_bcm_linux_mmap_access
{
    unsigned pid; /* process id of client */
    struct {
        unsigned offset; /* set all to zero to revoke access */
        unsigned size;
    } heap[BCM_MAX_HEAPS];
} t_bcm_linux_mmap_access;
#define BRCM_IOCTL_SET_MMAP_ACCESS          1027 /* use t_bcm_linux_mmap_access with this IOCTL */

typedef struct t_bcm_linux_write_cp0_25
{
    unsigned    select; 
    unsigned    value;
} t_bcm_linux_write_cp0_25;
#define BRCM_IOCTL_WRITE_CP0_25             1028 /* use t_bcm_linux_write_cp0_25 with this IOCTL */

/*
 *  The following definitions and structures are restricted to the kernel build
 *  User app's include this file but they don't need the folowing definitions
 */
#ifdef __KERNEL__
/* utility function */
void brcm_get_dev_info(struct inode *inode, unsigned int *devnum, unsigned int *devmode);
#endif

#endif /* BCMDRIVER_H */

