/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bcm_driver.c $
 * $brcm_Revision: 44 $
 * $brcm_Date: 10/10/12 11:54a $
 *
 * Module Description:
 * User mode driver with 128 bit interrupt support.
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/usermode/bcm_driver.c $
 * 
 * 44   10/10/12 11:54a erickson
 * SW7445-15: do not re-enable regardless of intStruct.interruptmask,
 * remove INT_REG_WRAP and assume straight mapping
 * 
 * 43   9/21/12 12:00p mward
 * SW7445-45:  Fix compatibility of the BVIRTADDR() macro with 2.6.18.
 * 
 * 42   9/19/12 10:38a erickson
 * SW7445-45: revise 7445 support
 * 
 * 41   9/18/12 4:40p erickson
 * SW7445-45: add 7445 support
 * 
 * 40   8/9/12 10:12a erickson
 * SW7445-15: more general cleanup, remove unused code, remove hardcoded
 * L1 array sizes
 * 
 * 39   8/8/12 5:10p erickson
 * SW7445-15: do not access L1 registers directly
 *
 * 38   8/2/12 8:44a erickson
 * SW7435-290: add linux 3.3 support
 *
 * 37   7/20/12 10:56a ahulse
 * SWDEPRECATED-3811: allow token separated input to proc debug
 *
 * 36   2/17/12 4:39p mward
 * SW7435-37:  Support Linux 3.3.
 *
 * 35   1/25/12 11:13a mward
 * SW7125-1220:  Coverity CID 25900.  Overrun of static array "debug_info"
 * of size 64 at position 64 with index variable "count" in
 * proc_write_debug().
 *
 * 34   12/20/11 3:11p vsilyaev
 * SW7425-2039: Added ioctl to control perfomance counters
 *
 * 33   12/19/11 10:03a erickson
 * SW7420-1690: fix failure condition for non-root client
 *
 * 32   11/2/11 5:07p erickson
 * SW7420-1690: make root user check more generic in brcm_ioctl
 *
 * 31   10/24/11 11:22a erickson
 * SW7420-1690: add bcmdriver non-root mmap support
 *
 * 30   9/15/11 9:31a dlwin
 * SW7335-1355: Fix for Coverity: 26492
 *
 * 29   8/8/11 5:24p gskerl
 * SW7425-1059: Added check of return code from request_irq()
 *
 * 28   6/30/11 3:20p jhaberf
 * SWDTV-7634: updated to use CEG header files for CEG products
 *
 * 27   5/31/11 2:05p erickson
 * SW7420-1907: prevent two instances from both trying to manage
 * interrupts
 *
 * 26   4/20/11 7:32p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 *
 * 25   4/15/11 12:30p nickh
 * SW7425-365: Add 2.6.37 kernel changes
 *
 * 24   3/7/11 4:25p erickson
 * SW7420-1576: fix warnings for kbuild
 *
 * 23   12/22/10 12:24p gracew
 * SW7468-364: Changed hardcoded brcm_ioctl() to atomic structure for
 * generic chips.
 *
 * 22   12/16/10 11:37a jrubio
 * HW7344-90: fix compile issuer with new xks01
 *
 * 20   9/8/10 10:45a erickson
 * SW7420-1007: add /proc/bcmdriver/latency
 *
 * 19   9/1/10 3:07p gmohile
 * SWDEPRECATED-3811 : Add debug support using procfs for usermode
 *
 * 18   3/5/10 3:50p gracew
 * SW7420-580: Added ICAM update.
 *
 * 17   2/4/10 5:36p jgarrett
 * SW7408-57: Adding TP1 hooks for soft audio
 *
 * 16   1/27/10 7:04p jhaberf
 * SW35230-1: Reverting change to bcm_driver.c and umdrv.mod.c as these
 * changes could cause a kernel panic on the 93556. The changes will be
 * checked into 935230 specific branch
 *
 * 15   1/13/10 12:41p jhaberf
 * SW35230-1: Updated bcmdriver for 2.6.28 linux kernel for the 35230 DTV
 * chip
 *
 * 14   8/18/09 10:12a jrubio
 * PR55232: fix 2618
 *
 * 13   8/17/09 3:51p jrubio
 * PR55232: add linux suport for 2630
 *
 * 12   4/15/09 10:09a katrep
 * PR54161: Disable uname -a check for 7405
 *
 * 11   2/9/09 6:47p jrubio
 * PR51629: accidently checked in a typo
 *
 * 10   2/4/09 10:32a jrubio
 * PR51629: add 7336 support
 *
 * 9   11/26/08 4:57p erickson
 * PR49699: Don't use get_RAM_size to calculate nexus heap size and
 * location
 *
 * 8   9/23/08 3:43p erickson
 * PR46950: added BRCM_IOCTL_ATOMIC_UPDATE
 *
 * 7   9/23/08 8:56a erickson
 * PR47217: merge
 *
 * PR47217/1   9/23/08 2:05p dyzhang
 * PR47217: support get tick in bcm_driver
 *
 * 6   9/22/08 12:49p erickson
 * PR45432: added BRCM_IOCTL_GET_DCACHE_LINE_SIZE
 *
 * 5   8/7/08 12:29p katrep
 * PR45473: removed chip dependancy for the common file bcm_driver.c
 *
 * 4   7/8/08 3:15p dlwin
 * PR 44546: Added  bcmdriver support for 7601
 *
 * 3   3/14/08 4:00p qcheng
 * PR40402: Add ICAM support to BCM7325 A0
 *
 * 2   12/14/07 4:40p katrep
 * PR37430: Updated the debug information.
 *
 * 1   11/21/07 1:19p katrep
 * PR37430: Added suppport for upto 128 bit interrupt
 * controller,reoganized the driver code.
 *
 *
 ***************************************************************************/
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#elif ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30) ) && ( LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37) )
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/utsname.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include "bcm_driver.h"
#include "bcmdriver_common.h"
#if B_NETIF_DMA
#include "netif_dma.h"
#endif

/*
* Macros to help debugging
*/
#undef PDEBUG
#undef PWARN
#undef PERR
#undef PINFO

#define DEBUG
#ifdef DEBUG
    #define PERR(fmt, args...) printk( KERN_ERR "BCMDRV: " fmt, ## args)
    #define PWARN(fmt, args...) printk( KERN_WARNING "BCMDRV: " fmt, ## args)
    #define PINFO(fmt, args...) printk( KERN_INFO "BCMDRV: " fmt, ## args)
    #define PDEBUG(fmt, args...) printk( KERN_DEBUG "BCMDRV: " fmt, ## args)
#else  
    #define PERR(fmt, args...) 
    #define PWARN(fmt, args...) 
    #define PINFO(fmt, args...) 
    #define PDEBUG(fmt, args...) 
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)
#include "linux/brcmstb/brcmstb.h"
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
#include "asm/brcmstb/brcmstb.h"
#else
#define BVIRTADDR(offset) (KSEG1 + 0x10000000 + offset)

#endif

#define PINFO_TRACE(fmt, args...) /* PINFO(fmt, ## args) */

/*
  Since smart card is part of the UPG and to support smart card beast
  service, we has to create high status and mask here. The high mask
  and high status is only used by software IRQ that created by Linux
  kernel. When we report this high status back to application, we have
  to map it to one of the low status. For example, when we receive virtual
  IRQ 40 and 41 for SCA and SCB, we need to map it to UPG IRQ 10.
  This design is also based on the assumption that we will not receive IRQ
  10 in this user mode driver. We will receive IRQ 40 and 41 instead and
  then map it to 10.
*/
typedef struct
{
    unsigned long   reportedStatus[BCM_INTC_MAX_SIZE]; /* Interrupts that have been reported by driver to calling code */
    unsigned long   globalmask[BCM_INTC_MAX_SIZE];     /* HW interrupts that the driver controls */
    unsigned long   status[BCM_INTC_MAX_SIZE];         /* HW interrupts we've received. This is equivalent to reading L1 status bits, but via kernel. */
}intmgr_t;


#define BCM_DRIVER_REVISION strrchr("$brcm_Revision: 44 $",':')+2
#define MODULE_NAME "bcmdriver"
#define INFINITE 0xFFFFFFFF
#define BRCM_MAJOR 30 /* Major device number */

#ifdef HAVE_RSRVD
/* don't call get_RAM_size(). it will return total memory on the board, even though upper memory may be unusable. e.g. 512M board may only have 256M usable.
this may need to change in the future. */
#define BOARD_MEM_SIZE       (get_RAM_size()>(256*1024*1024)?(256*1024*1024):get_RAM_size())
#define KERNEL_MEM_SIZE     ( BOARD_MEM_SIZE - get_RSVD_size() )
#else
/* default to these values for now */
#define KERNEL_MEM_SIZE     (32*1024*1024)
#define BOARD_MEM_SIZE      (256*1024*1024)
#endif
#if defined(HUMAX_PLATFORM_BASE)
#if (PLATFORM == 97425)
#define BCM7425_UPG_MANAGED_BITS 0x00000040
#endif
#endif

static int  brcm_stb_init(void);
static int  brcm_open(struct inode *inode, struct file *file);
static int  brcm_close(struct inode * inode, struct file * file);
static int  brcm_mmap(struct file *file, struct vm_area_struct *vma);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
static long  brcm_ioctl(struct file * file,unsigned int cmd, unsigned long arg);
#else
static int  brcm_ioctl(struct inode *inode, struct file * file,unsigned int cmd, unsigned long arg);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
static irqreturn_t brcm_interrupt(int irq, void *dev_id);
#else
static irqreturn_t brcm_interrupt(int irq, void *dev_id, struct pt_regs *regs);
#endif
static void brcm_enable_irq(intmgr_t *p_intmgr, unsigned long Mask[]);


int gBcmDebuglevel=0;
int gBcmDebug=0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)
static spinlock_t gSpinLock = __SPIN_LOCK_UNLOCKED(gSpinLock);
#else
static spinlock_t gSpinLock = SPIN_LOCK_UNLOCKED;
#endif
static wait_queue_head_t g_WaitQueue;
static intmgr_t g_intmgr;
static int upg_irq_num=10;
#if defined(HUMAX_PLATFORM_BASE)
#if (PLATFORM == 97425)
static struct proc_dir_entry *umdrv_dir, *version_file, *interrupt_file, *latency_file, *interruptL2_file, *meminfo_file, *debug_file;
#else
static struct proc_dir_entry *umdrv_dir, *version_file, *interrupt_file, *latency_file, *meminfo_file, *debug_file;
#endif
#else
static struct proc_dir_entry *umdrv_dir, *version_file, *interrupt_file, *latency_file, *meminfo_file, *debug_file;
#endif
#define DEBUG_INFO_LEN 256
char debug_info[DEBUG_INFO_LEN];

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)
    #include <linux/brcmstb/brcmapi.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    #include <asm/brcmstb/brcmapi.h>
#else
#include "asm/brcmstb/common/brcmstb.h"
#ifndef BRCM_MAGNUM_SPINLOCK
static spinlock_t g_magnum_spinlock = SPIN_LOCK_UNLOCKED;
#else
extern spinlock_t g_magnum_spinlock;
#endif
#define brcm_magnum_spinlock g_magnum_spinlock
#endif


#if 1
void print_int_status(void)
{
    if (gBcmDebug) {
        unsigned i;
        unsigned long flags;
        spin_lock_irqsave(&gSpinLock, flags);
        for(i=0;i<g_sChipConfig.IntcSize;i++) {
            PWARN("L1 W%d: mask %08lx,stat %08lx\n", i, g_intmgr.globalmask[i], g_intmgr.status[i]);
        }
        spin_unlock_irqrestore(&gSpinLock, flags);    
    }
}
#else
#define print_int_status()
#endif


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
module_param(gBcmDebug,int,0000);
#else
MODULE_PARM(gBcmDebug,"0-1i");
#endif

static struct file_operations brcm_fops = {
    owner:      THIS_MODULE,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
    unlocked_ioctl:      brcm_ioctl,
#else
    ioctl:      brcm_ioctl,
#endif
    open:       brcm_open,
    release:    brcm_close,
    mmap:       brcm_mmap
};

static void brcm_enable_irq(intmgr_t *p_intmgr, unsigned long Mask[])
{
    unsigned i, irq;
    unsigned long flags;
    spin_lock_irqsave(&gSpinLock, flags);
    for (i=0,irq=1;i<g_sChipConfig.IntcSize;i++) {
        unsigned bitmask;
        for (bitmask=1;bitmask;bitmask <<= 1,irq++) {
            if ((Mask[i] & bitmask) && g_sChipConfig.pIntTable[irq].disabled && !g_sChipConfig.pIntTable[irq].l2share_func) {
                if (gBcmDebug) {
                    PWARN("Re-enabling %s irq (linux %d)\n", g_sChipConfig.pIntTable[irq].name, irq );
                }
                enable_irq(irq);
                g_sChipConfig.pIntTable[irq].disabled = 0;
            }
        }
    }
    spin_unlock_irqrestore(&gSpinLock, flags);
}

/**
Reenable all interrupts which we manage where status is cleared.
This can happen because when cores are reset and they reset their
status bits. */
static void brcm_reenable_irq(intmgr_t *p_intmgr)
{
    int irq;
    for (irq = 1; irq < g_sChipConfig.maxNumIrq; ++irq)
    {
        unsigned long flags;
        spin_lock_irqsave(&gSpinLock, flags);
        if (g_sChipConfig.pIntTable[irq].disabled) {
            if(gBcmDebug)
                PINFO("Re-enabling %s irq (linux %d)\n", g_sChipConfig.pIntTable[irq].name, irq );
            enable_irq(irq);
            g_sChipConfig.pIntTable[irq].disabled = 0;
        }
        spin_unlock_irqrestore(&gSpinLock, flags);
    }
}

static int g_jiffies_isr = 0;   /* time when an L1 top half was received */
static int g_jiffies_user = 0;  /* time when usermode task retrieves the pending L1's */
static struct {
    unsigned cnt;
    unsigned total, min, max;
} g_latency;

#define get_jiffies() (jiffies)

/****************************************************************
* brcm_interrupt
*
* INPUTS:   irq, dev_id - see linux kernel api
* OUTPUTS:  None
* RETURNS:  None
* FUNCTION: 7xxx interrupt handler
*
****************************************************************/
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
static irqreturn_t brcm_interrupt(int irq, void *dev_id)
#else
static irqreturn_t brcm_interrupt(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
    unsigned index = (irq-1)/32;
    unsigned bit = (irq-1)%32;
    unsigned long flags;
    l2share_func_t l2share_func = g_sChipConfig.pIntTable[irq].l2share_func;

    if (!g_jiffies_isr) {
        g_jiffies_isr = get_jiffies(); /* measure the time from first receipt of an L1 */
    }

    if(gBcmDebug)
        PINFO("Got %s irq (linux %d) %d\n", g_sChipConfig.pIntTable[irq].name, irq,
                       (int)g_sChipConfig.pIntTable[irq].numInter );

    spin_lock_irqsave(&gSpinLock, flags);
    g_intmgr.status[index] |= 1<<bit;
    if (l2share_func)
    {
        unsigned long triggeredInts;
        l2share_func(irq, &triggeredInts);
    }
    else
    {
        if(gBcmDebug)
            PINFO("Disabling %s irq (linux %d) %d\n", g_sChipConfig.pIntTable[irq].name, irq,
                           (int)g_sChipConfig.pIntTable[irq].numInter );
        disable_irq_nosync(irq);
        g_sChipConfig.pIntTable[irq].disabled = 1;
    }
    spin_unlock_irqrestore(&gSpinLock, flags);
    print_int_status();

    g_sChipConfig.pIntTable[irq].numInter++;

    wake_up_interruptible(&g_WaitQueue);

    return IRQ_HANDLED;
}

/****************************************************************
 * brcm_get_dev_info - Extracts information from the minor device number
****************************************************************/
void brcm_get_dev_info(struct inode *inode, unsigned int *devnum,
                       unsigned int *devmode)
{
    *devnum = MINOR(inode->i_rdev);
    *devmode = 0;
}


static int proc_read_interrupts(char *buf, char **start, off_t offset,int count, int *eof, void *data)
{
    int irq, len = 0, i;

    for (i=0, irq=1;i<g_sChipConfig.IntcSize;i++) {
        unsigned bitmask;
        for (bitmask=1;bitmask;bitmask <<= 1,irq++) {
            if (g_intmgr.globalmask[i] & bitmask) {
                unsigned temp_len;
                temp_len = snprintf(buf+len, 0, "%s %ld\n", g_sChipConfig.pIntTable[irq].name,g_sChipConfig.pIntTable[irq].numInter);  /* returns the number of characters needed, including \n but excluding \0 */
                len += snprintf(buf+len, temp_len+1, "%s %ld\n", g_sChipConfig.pIntTable[irq].name,g_sChipConfig.pIntTable[irq].numInter);
            }
        }
    }

    *eof = 1;

    return len;
}

static int proc_write_interrupts(struct file *file,const char *buffer,unsigned long count,void *data)
{
    int irq;

    for (irq=1; irq< g_sChipConfig.maxNumIrq; irq++)
    {
        g_sChipConfig.pIntTable[irq].numInter=0;
    }

    return irq;
}

static int proc_read_latency(char *buf, char **start, off_t offset,int count, int *eof, void *data)
{
    int len = 0;
    len += snprintf(buf+len, 64, "interrupts: %d\n", g_latency.cnt);
    len += snprintf(buf+len, 64, "avg: %d\n", g_latency.cnt?g_latency.total/g_latency.cnt:0);
    len += snprintf(buf+len, 64, "max: %d\n", g_latency.max);
    len += snprintf(buf+len, 64, "min: %d\n", g_latency.min);
    *eof = 1;
    return len;
}

static int proc_write_latency(struct file *file,const char *buffer,unsigned long count,void *data)
{
    /* reset counts. don't parse incoming data. just consume it all. */
    memset(&g_latency, 0, sizeof(g_latency));
    return count;
}

#if defined(HUMAX_PLATFORM_BASE)
#if (PLATFORM == 97425)
static int proc_read_interruptsL2(char *buf, char **start, off_t offset,int count, int *eof, void *data)
{
 	int irq = 0, len = 0, temp_len = 0;
	volatile unsigned int *l2_base = (volatile unsigned long *)0xB0406784;
	
	 for (irq=0;irq<sizeof(interruptName7425L2)/sizeof(s_InteruptTable);irq++)
    	{
       	 if ( *l2_base & BCM7425_UPG_MANAGED_BITS )
        	{
			temp_len = snprintf(buf+len, 0, "%s %ld\n", interruptName7425L2.name,interruptName7425L2.numInter);  /* returns the number of characters needed, including \n but excluding \0 */
			len += snprintf(buf+len, temp_len+1, "%s %ld\n", interruptName7425L2.name,interruptName7425L2.numInter);
       	 }
	
}
	*eof = 1;

    return len;
}


static int proc_write_interruptsL2(struct file *file,const char *buffer,unsigned long count,void *data)
{
    int irq;

    interruptName7425L2.numInter=0;
    
    return irq;
}
#endif
#endif

static int proc_read_version(char *page, char **start,off_t off, int count,int *eof, void *data)
{
    int len;

    /* Note that "snprintf" returns the number of characters that would have been printed if there was enough room.
     * This means, for example, that in some cases it might be useful to call "snprintf" twice:
     * 1.> Once with N equal to zero. "snprintf" will not write any output,
     * but it will return how many characters would have been needed to hold the output.
     * You could use this value to "malloc" sufficient room to hold the output string.
     * 2.> Once with N equal to the return value from the first call, plus one.
     * If you have allocated sufficient space for the output string, this outputs the entire output string.
     */
    len = snprintf(page, 0, "%s\n", BCM_DRIVER_REVISION);
    len = snprintf(page, (len +1), "%s\n", BCM_DRIVER_REVISION);

    return len;
}

static int proc_read_meminfo(char *page, char **start,off_t off, int count,int *eof, void *data)
{
    int len, temp_len;
    static int kernelMem = -1;
    static int boardMem = -1;


    if ( -1 == kernelMem )
    {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
        unsigned long start = 0, len = 0;
        bmem_region_info(0, &start, &len);
        kernelMem = start;
        boardMem = start + len;
#else
        kernelMem = KERNEL_MEM_SIZE;
        boardMem = BOARD_MEM_SIZE;
#endif
    }

    len = snprintf(page, 0, "Kernel memory %d \n", kernelMem); /* returns the number of characters needed */
    len = snprintf(page, (len+1), "Kernel memory %d \n", kernelMem);

    temp_len = snprintf(page+len, 0, "Reserved Av memory %d \n", (boardMem-kernelMem));
    temp_len = snprintf(page+len, (temp_len+1), "Reserved Av memory %d \n", (boardMem-kernelMem));

    len += temp_len;

    return len;
}

static int proc_read_debug(char *page, char **start,off_t off, int count,int *eof, void *data)
{
    return 0;
}

static int proc_write_debug(struct file *file, const char *buffer, unsigned long count, void *data)
{
    if(count>=DEBUG_INFO_LEN) {
        /* SW7335-1355, Coverity: 26492 */
        count = (DEBUG_INFO_LEN-1);
        printk("text echo'd to debug exceeds max length, truncating. Max len=%d\n", DEBUG_INFO_LEN);
    }


    if (copy_from_user(debug_info, buffer, count))
    {
        count = -1;
        goto end;
    }

    if (count && debug_info[count-1] == '\n')
        debug_info[count - 1] = 0;
    else
        debug_info[count] = 0;

end:
    return count;
}

/* only one open instance of bcm_driver can manage interrupts.
other instances are allowed, but only for other functions. */
static struct file *g_interrupt_file = NULL;

/****************************************************************
* brcm_open(struct inode *inode, struct file *file)
****************************************************************/
static int brcm_open(struct inode *inode, struct file *file)
{
    int err = 0;
    unsigned int devnum, devmode;

    /* This must be the first operation to avoid auto-unload */

    brcm_get_dev_info(inode, &devnum, &devmode);

    if (gBcmDebug)
        printk("brcm_open devnum=%d devmode=%d\n",devnum,devmode);

    if ( devnum != 0 )
    {
        return -EFAULT;
    }

    return err;
}

/****************************************************************
* brcm_close(struct inode *inode, struct file *file)
****************************************************************/
static int brcm_close(struct inode *inode, struct file *file)
{
    if (file == g_interrupt_file) {
        g_interrupt_file = NULL;
    }
    wake_up_interruptible(&g_WaitQueue);
    return 0;
}

static unsigned b_get_euid(void)
{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 18)
    return current->euid;
#else
    return current_euid();
#endif
}

#define BRCM_MAX_CLIENTS 16
static t_bcm_linux_mmap_access g_mmap_access[BRCM_MAX_CLIENTS];

static int brcm_mmap(struct file *file, struct vm_area_struct *vma)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    int rc;
    unsigned offset = vma->vm_pgoff << PAGE_SHIFT;
    unsigned size = vma->vm_end - vma->vm_start;

    if (b_get_euid() != 0) {
        /* validate access for non-root clients */
        unsigned i;
        for (i=0;i<BRCM_MAX_CLIENTS;i++) {
            if (g_mmap_access[i].pid == current->pid) {
                /* verify mmap is within a range */
                unsigned j;
                for (j=0;j<BCM_MAX_HEAPS;j++) {
                    if (offset >= g_mmap_access[i].heap[j].offset && offset + size <= g_mmap_access[i].heap[j].offset + g_mmap_access[i].heap[j].size) {
                        /* it's good */
                        break;
                    }
                }
                if (j<BCM_MAX_HEAPS) {
                    break;
                }
                else {
                    i = BRCM_MAX_CLIENTS; /* pid can only be stored once, so set failure condition */
                    break;
                }
            }
        }
        if (i == BRCM_MAX_CLIENTS) {
            PERR("non-root mmap not allowed by server\n");
            return -EACCES;
        }
    }

    rc = remap_pfn_range(vma,
        vma->vm_start,
        vma->vm_pgoff,
        vma->vm_end - vma->vm_start,
        vma->vm_page_prot);
    if (rc) {
        return -EAGAIN;
    }

    return rc;
#else
    /* only tested on 2.6 */
    return -ENODEV;
#endif
}

static void brcm_init_mmap_access(void)
{
    memset(g_mmap_access, 0, sizeof(g_mmap_access));
}
static int brcm_set_mmap_access(const t_bcm_linux_mmap_access *access)
{
    unsigned open_slot = BRCM_MAX_CLIENTS;
    unsigned i;

    if (access->pid == 0) {
        /* root user is automatically granted access. don't bother storing.
        then we can use pid == 0 as indicator of unused slot. */
        return 0;
    }

    /* look for pid or open space */
    for (i=0;i<BRCM_MAX_CLIENTS;i++) {
        if (g_mmap_access[i].pid == access->pid) {
            unsigned j;
            for (j=0;j<BCM_MAX_HEAPS;j++) {
                if (access->heap[j].offset) break;
            }
            if (j == BCM_MAX_HEAPS) {
                /* if all offsets are zero, clear slot. no access. */
                g_mmap_access[i].pid = 0;
                PINFO_TRACE("remove pid %d mmap\n", access->pid);
            }
            else {
                /* update slot */
                g_mmap_access[i] = *access;
                PINFO_TRACE("update pid %d mmap\n", access->pid);
            }
            return 0;
        }
        else if (!g_mmap_access[i].pid && open_slot == BRCM_MAX_CLIENTS) {
            open_slot = i;
        }
    }
    if (open_slot < BRCM_MAX_CLIENTS) {
        g_mmap_access[open_slot] = *access;
        PINFO_TRACE("add pid %d mmap\n", access->pid);
        return 0;
    }
    /* too many clients. either increase BRCM_MAX_CLIENTS or convert to dynamic list. */
    return -1;
}

/*************************************************************************************************************
* brcm_ioctl(struct inode *inode, struct file * file,
*               unsigned int cmd, unsigned long arg)
*************************************************************************************************************/
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
static long brcm_ioctl(struct file * file, unsigned int cmd, unsigned long arg)
#else
static int brcm_ioctl(struct inode *inode, struct file * file, unsigned int cmd, unsigned long arg)
#endif
{
    int result;
    int i;

#if B_NETIF_DMA
    result = bnetif_ioctl(inode, file, cmd, arg);
    if (result!=-ENOSYS) {
        return  result;
    }
#endif
    result = 0;

    /* non-root users can mmap, but cannot ioctl. if non-root ioctl's are added, allow them here. */
    if (b_get_euid() != 0) {
        PERR("access limited to root user\n");
        return -EACCES;
    }

    switch(cmd)
    {
    case BRCM_IOCTL_WAIT_FOR_INTERRUPTS:
        {
            tbcm_linux_dd_interrupt intStruct;
            unsigned long flags;
            int measured_latency = -1; /* not set */

            if (g_jiffies_user) {
                int now = get_jiffies();
                if (now >= g_jiffies_user) { /* protect against wrap. may be zero. */
                    measured_latency = now - g_jiffies_user;
                    g_jiffies_user = 0;
                }
            }

            if( copy_from_user( &intStruct, (void *)arg, sizeof(tbcm_linux_dd_interrupt) ) )
            {
                result = -EFAULT;
                PERR("copy_from_user failed!\n");
                break;
            }

            if(gBcmDebug) {
                char buf[64], *ptr = buf;
                for (i=0;i<g_sChipConfig.IntcSize;i++) {
                    ptr += snprintf(ptr, 64, " W%d=0x%08lX", i, intStruct.interruptmask[i]);
                }
                PWARN("waitfor%s with timeOut=%d\n", buf, intStruct.timeout);
            }     

            for(i=0;i<g_sChipConfig.IntcSize;i++)
            {
                if((intStruct.interruptmask[i] | g_intmgr.globalmask[i]) != g_intmgr.globalmask[i])
                {
                    PERR("Invalid wait irpt mask[%d]=0x%08lX,global mask[%d]=0x%08lX\n",i,intStruct.interruptmask[i],i,g_intmgr.globalmask[i]);
                    /* mask off the invalid bits */
                    intStruct.interruptmask[i] &= g_intmgr.globalmask[i];
                }

            }
            /* Re-enable any interrupts that were previously reported and handled */
            brcm_enable_irq(&g_intmgr, intStruct.interruptmask);

            print_int_status();

            {
                unsigned long ticks;
                wait_queue_t wq_entry;
                unsigned pending;

                result = 0;
                if (intStruct.timeout != 0 && intStruct.timeout != INFINITE) {
                    ticks = (intStruct.timeout * HZ) / 1000;
                }
                else {
                    ticks = MAX_SCHEDULE_TIMEOUT;
                }

                init_waitqueue_entry(&wq_entry, current);
                add_wait_queue(&g_WaitQueue, &wq_entry);

                /* Be sure to go half asleep before checking condition. */
                /* Otherwise we have a race condition between when we   */
                /* check the condition and when we call schedule().     */
                set_current_state(TASK_INTERRUPTIBLE);

                spin_lock_irqsave(&gSpinLock, flags);
                pending = 0;
                for (i=0;i<g_sChipConfig.IntcSize;i++) {
                    pending = pending || (intStruct.interruptmask[i] & g_intmgr.status[i]);
                }
                spin_unlock_irqrestore(&gSpinLock, flags);
                if (!pending) {
                    if(schedule_timeout(ticks) == 0)
                        result = -EIO;
                    if (signal_pending(current))
                        result = -EIO;
                }
                current->state = TASK_RUNNING;
                remove_wait_queue(&g_WaitQueue, &wq_entry);
            }

            spin_lock_irqsave(&gSpinLock, flags);
            for (i=0;i<g_sChipConfig.IntcSize;i++) {
                intStruct.interruptstatus[i] = g_intmgr.status[i] & intStruct.interruptmask[i];
                g_intmgr.reportedStatus[i] = intStruct.interruptstatus[i];
                g_intmgr.status[i] &= ~intStruct.interruptmask[i];
            }
            spin_unlock_irqrestore(&gSpinLock, flags);

            if (g_jiffies_isr) {
                unsigned now = get_jiffies();
                g_jiffies_user = g_jiffies_isr; /* move this value to the second stage of the pipeline */
                /* only account if measured_latency is set and g_jiffies_isr did not wrap. */
                if (measured_latency >= 0 && now >= g_jiffies_isr) {
                    g_jiffies_isr = 0;
                    g_latency.cnt++;
                    g_latency.total += measured_latency;
                    if (measured_latency > g_latency.max) {
                        g_latency.max = measured_latency;
                    }
                    if (measured_latency < g_latency.min) {
                        g_latency.min = measured_latency;
                    }
                }
            }

            print_int_status();

            if( copy_to_user( (void *)arg, &intStruct, sizeof(tbcm_linux_dd_interrupt) ) )
            {
                result = -EFAULT;
                PERR("copy_to_user failed!\n");
                break;
            }

            if(gBcmDebug) {
                char buf[64], *ptr = buf;
                for (i=0;i<g_sChipConfig.IntcSize;i++) {
                    ptr += snprintf(ptr, 64, " W%d=0x%08lX", i, intStruct.interruptstatus[i]);
                }
                PWARN("returning interrupt%s\n", buf);
            }
        }         
        break;

    case BRCM_IOCTL_CHANGE_REQUEST:
        /* The application would like to make a change right now. It doesn't
        have to wait until the next L1 interrupt. This aborts the current
        wait and causes BRCM_IOCTL_WAIT_FOR_INTERRUPTS to return immediately. */
        print_int_status();
        wake_up_interruptible(&g_WaitQueue);
        break;

    case BRCM_IOCTL_INT_RESET:
        /* If the software resets the L2 (and thus L1) interrupts, you may
        need to reenabled all L1 interrupts so that you don't get stuck. */
        /* nexus calls BRCM_IOCTL_INT_RESET immediately after open, so it's an
        ideal spot to check for duplicate interrupt management. */
        if (!g_interrupt_file) {
            g_interrupt_file = file;
        }
        else if (file != g_interrupt_file) {
            PERR("can't manage interrupts from two instances\n");
            return -EFAULT;
        }
        brcm_reenable_irq(&g_intmgr);
        break;

    case BRCM_IOCTL_DEBUG_LEVEL:
        gBcmDebuglevel = (int)arg;
        PWARN(" debug level is %d\n",  gBcmDebuglevel);
        break;

    case BRCM_IOCTL_GET_DCACHE_LINE_SIZE:
        {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
            struct brcm_cache_info info;

            brcm_get_cache_info(&info);
            put_user(info.max_writeback, (unsigned __user *)arg);
#else
            unsigned max_dcache_line_size;
            max_dcache_line_size = cpu_data[0].dcache.linesz; /* L1 cache */
            if (cpu_data[0].scache.linesz > max_dcache_line_size) {
                max_dcache_line_size = cpu_data[0].scache.linesz; /* L2 cache */
            }
            copy_to_user((void *)arg, &max_dcache_line_size, sizeof(max_dcache_line_size));
#endif
        }
        break;

    case BRCM_IOCTL_ATOMIC_UPDATE:
        {
            unsigned long flags = 0;
            uint32_t value;
            t_bcm_linux_dd_atomic_update atomic_update_data;
            copy_from_user(&atomic_update_data, (void*)arg, sizeof(atomic_update_data));

            /* this spinlock synchronizes with any kernel use of a set of shared registers.
            see BREG_P_CheckAtomicRegister in magnum/basemodules/reg/breg_mem.c for the list of registers. */
            spin_lock_irqsave(&brcm_magnum_spinlock, flags);

            /* read/modify/write */
            value = *(volatile uint32_t*) BVIRTADDR(atomic_update_data.reg);
            value &= ~atomic_update_data.mask;
            value |= atomic_update_data.value;
            *(volatile uint32_t*) BVIRTADDR(atomic_update_data.reg) = value;
            spin_unlock_irqrestore(&brcm_magnum_spinlock, flags);
        }
        break;
#if BICM_ICAM2


    case BRCM_IOCTL_MULTI_ATOMIC_UPDATE:
        {
            unsigned long flags = 0;
            t_bcm_linux_dd_multi_atomic_update data;
            copy_from_user(&data, (void*)arg, sizeof(data));

            /* this spinlock synchronizes with any kernel use of a set of shared registers.
            see BREG_P_CheckAtomicRegister in magnum/basemodules/reg/breg_mem.c for the list of registers. */
            spin_lock_irqsave(&brcm_magnum_spinlock, flags);

            for (i=0;i<BCM_MULTI_ATOMIC_UPDATE_NUM;i++) {
        /*printk("\nset[%d], addr=0x%x, value=0x%x\n",i, data.set[i].reg, data.set[i].value);       */
                if (data.set[i].reg) {
                    uint32_t value;
                    /* read/modify/write */
                    value = *(volatile uint32_t*) BVIRTADDR(data.set[i].reg);
                    value &= ~data.set[i].mask;
                    value |= data.set[i].value;
                    *(volatile uint32_t*) BVIRTADDR(data.set[i].reg) = value;
                }
                }
            spin_unlock_irqrestore(&brcm_magnum_spinlock, flags);

        }
        break;
#endif

    case BRCM_IOCTL_GET_TICKS:
        {
            tbcm_linux_dd_ticks tick;
            tick.jiffies = get_jiffies();
            tick.timerFreq = HZ;
            if (copy_to_user((void *)arg, &tick, sizeof(tbcm_linux_dd_ticks)))
            {
                result = -EFAULT;
                PERR("copy_to_user failed!\n");
                break;
            }
            break;
        }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    case BRCM_IOCTL_READ_RESERVED:
    case BRCM_IOCTL_WRITE_RESERVED:
        {
            t_bcm_linux_rw_reserved rw_reserved;
            volatile uint32_t *pMem;
            if (copy_from_user(&rw_reserved, (void *)arg, sizeof(rw_reserved)))
            {
                result = -EFAULT;
                PERR("RESERVED copy_from_user failed!\n");
                break;
            }
            if ( rw_reserved.address & 0x3 )
            {
                result = -EINVAL;
                PERR("Can only r/w reserved memory on 32-bit boundaries\n");
                break;
            }
            pMem = (volatile uint32_t *)phys_to_virt(rw_reserved.address);
            if ( cmd == BRCM_IOCTL_READ_RESERVED )
            {
                rw_reserved.value = *pMem;
                if ( copy_to_user((void *)arg, &rw_reserved, sizeof(rw_reserved)) )
                {
                    result = -EFAULT;
                    PERR("BRCM_IOCTL_READ_RESERVED copy_to_user failed!\n");
                    break;
                }
            }
            else
            {
                *pMem = rw_reserved.value;
                dma_cache_wback_inv((uint32_t)pMem, 4);
            }
        }
        break;
#ifdef CONFIG_BMIPS4380
    case BRCM_IOCTL_READ_CORE_REGISTER:
    case BRCM_IOCTL_WRITE_CORE_REGISTER:
        {
            t_bcm_linux_rw_core_register rw_reg;
            volatile uint32_t *pReg;
            if (copy_from_user(&rw_reg, (void *)arg, sizeof(rw_reg)))
            {
                result = -EFAULT;
                PERR("CORE_REGISTER copy_from_user failed!\n");
                break;
            }
            if ( rw_reg.offset & 0x3 || rw_reg.offset > BMIPS_RELO_VECTOR_CONTROL_1 )
            {
                result = -EINVAL;
                PERR("Invalid core register offset %#x\n", rw_reg.offset);
                break;
            }
            pReg = (volatile uint32_t *)(BMIPS_GET_CBR() + rw_reg.offset);
            if ( cmd == BRCM_IOCTL_READ_CORE_REGISTER )
            {
                rw_reg.value = *pReg;
                if ( copy_to_user((void *)arg, &rw_reg, sizeof(rw_reg)) )
                {
                    result = -EFAULT;
                    PERR("BRCM_IOCTL_READ_CORE_REGISTER copy_to_user failed!\n");
                    break;
                }
            }
            else
            {
                *pReg = rw_reg.value;
            }
        }
        break;
    case BRCM_IOCTL_READ_CMT_CONTROL:
        {
            uint32_t regval;
            regval = read_c0_brcm_cmt_ctrl();
            if ( copy_to_user((void *)arg, &regval, sizeof(regval)) )
            {
                result = -EFAULT;
                PERR("BRCM_IOCTL_READ_CMT_CONTROL copy_to_user failed!\n");
                break;
            }
        }
        break;
    case BRCM_IOCTL_WRITE_CMT_CONTROL:
        write_c0_brcm_cmt_ctrl(arg);
        break;
#endif  /* 4380 */
#endif  /* >= 2.6.30 */
    case BRCM_IOCTL_GET_DEBUG:
        if ( copy_to_user((void *)arg, debug_info, sizeof(debug_info)) )
            {
            result = -EFAULT;
            PERR("BRCM_IOCTL_SET_DEBUG copy_to_user failed!\n");
            break;
            }
        memset(debug_info, 0, sizeof(debug_info));
        break;

    case BRCM_IOCTL_SET_MMAP_ACCESS:
        {
            t_bcm_linux_mmap_access access;
            if (copy_from_user(&access, (void*)arg, sizeof(access))) {
                result = -EFAULT;
                PERR("BRCM_IOCTL_SET_MMAP_ACCESS copy_from_user failed!\n");
                break;
            }
            result = brcm_set_mmap_access(&access);
            if (result) {
                PERR("brcm_set_mmap_access failed: %d\n", result);
                result = -ENOMEM;
            }
        }
        break;
#if defined CONFIG_BMIPS5000 || defined CONFIG_CPU_BMIPS5000
    case BRCM_IOCTL_WRITE_CP0_25:
        {
            t_bcm_linux_write_cp0_25 cp0_25;
            if (copy_from_user(&cp0_25, (void*)arg, sizeof(cp0_25))) {
                result = -EFAULT;
                PERR("BRCM_IOCTL_WRITE_CP0_25 copy_from_user failed!\n");
                break;
            }
#define bcm_write_perf(sel, value)                  \
{       __asm__ __volatile__(".set\tpush\n\t"           \
            ".set\tmips32\n\t"                          \
            "mtc0\t%0, $25, " #sel "\n\t"   \
            ".set\tpop\n\t"                         \
            : /* none */                                \
            : "r" ((unsigned int)value));               \
}
            switch(cp0_25.select) {
            case 0: bcm_write_perf(0, cp0_25.value);break;
            case 1: bcm_write_perf(1, cp0_25.value);break;
            case 2: bcm_write_perf(2, cp0_25.value);break;
            case 3: bcm_write_perf(3, cp0_25.value);break;
            case 4: bcm_write_perf(4, cp0_25.value);break;
            case 5: bcm_write_perf(5, cp0_25.value);break;
            case 6: bcm_write_perf(6, cp0_25.value);break;
            case 7: bcm_write_perf(7, cp0_25.value);break;
            default:
                result = -EINVAL;
                PERR("BRCM_IOCTL_WRITE_CP0_25 invalid select:%u\n", cp0_25.select);
            }
        }
        break;
#endif

    default:
        result = -ENOSYS;
        break;
    }

    return result;
}

/****************************************************************
* brcm_stb_init(void)
****************************************************************/
static int brcm_stb_init(void)
{
    unsigned i, irq;
    int rv = 0;
    /* Leave interrupt disabled while we are handling  it */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    unsigned long flags = IRQF_DISABLED;
#else
    unsigned long flags = SA_INTERRUPT;
#endif

    PINFO("Initializing bcmdriver version $ %s\n", BCM_DRIVER_REVISION);

    memset(&g_intmgr,0,sizeof(intmgr_t));
    brcm_init_mmap_access();

    /* Make sure to init this first, as we may get an interrupt as soon as we request it */
    init_waitqueue_head(&g_WaitQueue);

    PINFO("Total intc words=%d,Total Irqs=%d\n",g_sChipConfig.IntcSize,g_sChipConfig.maxNumIrq);

    /* Request interrupts that we manage */
    for (i=0,irq=1;i<g_sChipConfig.IntcSize;i++) {
        unsigned bitmask;
        g_intmgr.globalmask[i] = 0;
        for (bitmask=1;bitmask;bitmask <<= 1,irq++) {
            if ( g_sChipConfig.pIntTable[irq].manageInt & INT_ENABLE_MASK ) {
                unsigned long new_flags;
    
                if (gBcmDebug)
                    PWARN("Requesting %s irq (linux %d)\n", g_sChipConfig.pIntTable[irq].name,irq);
    
                new_flags = flags;
                if ( g_sChipConfig.pIntTable[irq].manageInt & INT_SHARABLE_MASK )
                {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
                    new_flags |= IRQF_SHARED;
#else
                    new_flags |= SA_SHIRQ;
#endif
                }
                /* Last parameter is don't care */
                if (request_irq(irq, brcm_interrupt, new_flags, g_sChipConfig.pIntTable[i].name, (void*)irq+1))
                {
                    PERR("request_irq failed, irq:%d name:%s\n", irq, g_sChipConfig.pIntTable[irq].name);
                }
    
                g_intmgr.globalmask[i] |= bitmask;
            }
        }
    }

    print_int_status();

    /* Register driver last in case of errors during initialization */
    if (register_chrdev(BRCM_MAJOR,"brcm", &brcm_fops))
    {
        PERR("Unable to get major %d\n", BRCM_MAJOR);
        goto error;
    }
    /* create directory */
    umdrv_dir = proc_mkdir(MODULE_NAME, NULL);
    if (umdrv_dir == NULL)
    {
        rv = -ENOMEM;
        goto error;
    }
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
    umdrv_dir->owner = THIS_MODULE;
#endif

    version_file = create_proc_read_entry("version",0444, umdrv_dir,proc_read_version,NULL);
    if (version_file == NULL)
    {
        rv  = -ENOMEM;
        goto no_version;
    }
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
    version_file->owner = THIS_MODULE;
#endif

    interrupt_file = create_proc_entry("interrupts", 0644, umdrv_dir);
    if (interrupt_file == NULL)
    {
        rv = -ENOMEM;
        goto no_version;
    }
    interrupt_file->read_proc = proc_read_interrupts;
    interrupt_file->write_proc = proc_write_interrupts;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
    interrupt_file->owner = THIS_MODULE;
#endif

    latency_file = create_proc_entry("latency", 0644, umdrv_dir);
    if (latency_file == NULL)
    {
        rv = -ENOMEM;
        goto no_version;
    }
    latency_file->read_proc = proc_read_latency;
    latency_file->write_proc = proc_write_latency;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
    latency_file->owner = THIS_MODULE;
#endif

    meminfo_file = create_proc_entry("meminfo", 0644, umdrv_dir);
    if (meminfo_file == NULL)
    {
        rv = -ENOMEM;
        goto no_version;
    }
    meminfo_file->read_proc = proc_read_meminfo;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
    meminfo_file->owner = THIS_MODULE;
#endif

#if defined(HUMAX_PLATFORM_BASE)
#if (PLATFORM == 97425)
	interruptL2_file = create_proc_entry("interruptsL2", 0644, umdrv_dir);
	if (interruptL2_file == NULL)
	{
		rv = -ENOMEM;
		goto no_version;
	}
	interruptL2_file->read_proc = proc_read_interruptsL2;
	interruptL2_file->write_proc = proc_write_interruptsL2;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
	interruptL2_file->owner = THIS_MODULE;
#endif
#endif
#endif

    debug_file = create_proc_entry("debug", 0644, umdrv_dir);
    if (debug_file == NULL)
    {
        rv = -ENOMEM;
        goto no_version;
    }
    debug_file->read_proc = proc_read_debug;
    debug_file->write_proc = proc_write_debug;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
    debug_file->owner = THIS_MODULE;
#endif

    PINFO("Initialization complete\n");

    return 0;

no_version:
    remove_proc_entry(MODULE_NAME, NULL);

error:
    return rv;
}

static int __init __init_module(void)
{
    int ret=0;

#if B_NETIF_DMA
    init_bnetif_dma();
#endif
    ret = brcm_stb_init();

    return ret;
}
module_init(__init_module);

static void __exit __cleanup_module(void)
{
    int i;

    PINFO("Cleanup_modules...\n");
#if B_NETIF_DMA
    cleanup_bnetif_dma();
#endif

    for ( i = 1; i <  g_sChipConfig.maxNumIrq; i++ )
    {
        if ( g_sChipConfig.pIntTable[i].manageInt & INT_ENABLE_MASK )
        {
            if (gBcmDebug)
                PWARN("Freeing %s irq (linux %d)\n", g_sChipConfig.pIntTable[i].name, i );

            /* if the function to disable the interrupt is provided call it now */
            if(g_sChipConfig.pIntTable[i].mask_func)
                g_sChipConfig.pIntTable[i].mask_func(0,NULL);

            free_irq(i, (void*)i+1);
        }
    }

    unregister_chrdev(BRCM_MAJOR, "brcm");

    remove_proc_entry("debug", umdrv_dir);
    remove_proc_entry("meminfo", umdrv_dir);
    remove_proc_entry("interrupts", umdrv_dir);
#if defined(HUMAX_PLATFORM_BASE)
#if (PLATFORM == 97425)
	remove_proc_entry("interruptsL2", umdrv_dir);
#endif
#endif
    remove_proc_entry("latency", umdrv_dir);
    remove_proc_entry("version", umdrv_dir);
    remove_proc_entry(MODULE_NAME, NULL);

    PINFO("Cleanup complete\n");
}

module_exit(__cleanup_module);


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,18)
MODULE_LICENSE("Proprietary");
#endif
/* End of file */
