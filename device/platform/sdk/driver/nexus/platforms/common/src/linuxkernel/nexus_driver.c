/***************************************************************************
 *     (c)2004-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_driver.c $
 * $brcm_Revision: 84 $
 * $brcm_Date: 7/31/12 11:39a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/common/src/linuxkernel/nexus_driver.c $
 * 
 * 84   7/31/12 11:39a erickson
 * SW7358-173: move linux-specific code out of generic driver
 * 
 * 83   2/21/12 8:50a erickson
 * SW7346-703: add linux 3.3 support
 * 
 * 82   11/15/11 3:19p erickson
 * SW7425-1747: remove old code
 * 
 * 81   9/8/11 3:30p erickson
 * SW7425-1262: use brcm_get_cache_info to learn max L1/L2 cache size
 * 
 * 80   8/22/11 11:20a erickson
 * SW7420-1148: 2618 can use current->euid
 *
 * 79   8/12/11 3:58p jrubio
 * SW7420-1148: 2618 does not support current_ueid, default to true for
 *  root access
 *
 * 78   8/10/11 4:32p erickson
 * SW7420-1148: add trusted boolean to nexus_generic_driver_open to
 *  prevent non-root from initializing nexus
 *
 * 77   8/3/11 5:33p vsilyaev
 * SW7420-1465, SW7405-5221: Route debug output to the special FIFO
 *  instead of syslog buffer
 *
 * 76   7/19/11 6:05p jhaberf
 * SWDTV-7208: using CONFIG_BRCM_CEG for DTV specific code.
 *  CONFIG_BRCM_CEG is defined in autoconf.h for CEG kernels
 *
 * 75   7/19/11 2:59p jhaberf
 * SWDTV-7208: fix compile error for 7344 kernel mode
 *
 * 74   7/18/11 2:36p jhaberf
 * SWDTV-7208: check in on behalf of mstefan. Merge of DTV kernel mode
 *  changes with the main
 *
 * 73   5/16/11 4:19p erickson
 * SW7420-1819: add alternative impl of NEXUS_Platform_P_TerminateProcess
 *
 * 72   5/10/11 12:15p erickson
 * SW7420-1819: fix linux 2.6.18 builds
 *
 * 71   5/4/11 11:24a erickson
 * SW7420-1819: fix 2.6.37
 *
 * 70   5/3/11 5:08p erickson
 * SW7420-1819: repartition so NFE is a standard feature
 *
 ************************************************************/

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/signal.h>
#include <linux/kernel.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
    #include <linux/kconfig.h>
    #include <linux/brcmstb/brcmapi.h> /* bmem_region_info */
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    #include <asm/brcmstb/brcmapi.h> /* bmem_region_info */
#else
    #include <asm/brcmstb/common/brcmstb.h>

    /* These are included in the 2.6.12-4.0 kernel and later. */
    #if HAVE_RSRVD
    extern unsigned long get_RAM_size();
    extern unsigned long get_RSVD_size();
    #endif
#endif

#include "nexus_generic_driver.h"

#ifdef MODULE
int     init_module(void);
void    cleanup_module(void);
#else
#error "Not supported"
#endif

static int      nexus_driver_open(struct inode *inode, struct file *file);
static int      nexus_driver_close(struct inode *inode, struct file * file);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
static int      nexus_driver_ioctl(struct inode *inode, struct file * file, unsigned int cmd, unsigned long arg);
#else
static int      nexus_driver_ioctl(struct file * file, unsigned int cmd, unsigned long arg);
#endif
static int      nexus_driver_mmap(struct file *file, struct vm_area_struct *vma);

static struct file_operations
nexus_driver_fops = {
    owner:          THIS_MODULE,
    read:           NULL,
    write:          NULL,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
    ioctl:          nexus_driver_ioctl,
#else
    unlocked_ioctl: nexus_driver_ioctl,
#endif
    mmap:           nexus_driver_mmap,
    open:           nexus_driver_open,
    release:        nexus_driver_close
};

static char b_env[256]="";
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
module_param_string(config, b_env, sizeof(b_env), 0);
#endif

static void
b_parse_env(char *env)
{
    char *s;
    const char *name;
    const char *value;
    /* traverse  string, and split it to name/value pairs */
    for(s=env, name=env, value=NULL;;s++) {
        switch(*s) {
        case '\0':
            goto done;
        case '=':
            *s = '\0';
            value = s+1;
            break;
        case ' ':
        case ':':
        case ';':
            *s = '\0';
            if (value==NULL) {
                value=s;
            }
            NEXUS_SetEnv(name, value);
            name = s+1;
            value = NULL;
            break;
        default:
            break;
        }
    }
done:
    if(*name) {
        if (value==NULL) {
            value=s;
        }
        NEXUS_SetEnv(name, value);
    }
    return;
}

static int
nexus_driver_open(struct inode *inode, struct file *file)
{
    int rc;
    unsigned minor = MINOR(inode->i_rdev);
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 18)
    rc = nexus_generic_driver_open(minor, &file->private_data, current->pid, current->euid == 0);
#else
    rc = nexus_generic_driver_open(minor, &file->private_data, current->pid, current_euid() == 0);
#endif
    if (rc) return -ENODEV;
    return 0;
}

static int
nexus_driver_close(struct inode *inode, struct file *file)
{
    unsigned minor = MINOR(inode->i_rdev);
    nexus_generic_driver_close(minor, file->private_data, signal_pending(current));
    return 0;
}

static int nexus_driver_mmap(struct file *file, struct vm_area_struct *vma) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    int rc;
    unsigned offset = vma->vm_pgoff << PAGE_SHIFT;

    rc = nexus_generic_driver_validate_mmap(0 /* unused */, file->private_data, offset, vma->vm_end - vma->vm_start);
    if (rc) return -EINVAL;

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

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
static int
nexus_driver_ioctl(struct inode *inode, struct file * file,
                      unsigned int cmd, unsigned long arg)
#else
static int
nexus_driver_ioctl(struct file * file,
                     unsigned int cmd, unsigned long arg)
#endif
{
    int rc;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
    unsigned minor = MINOR(inode->i_rdev);
#else
    unsigned minor = MINOR(file->f_path.dentry->d_inode->i_rdev);

#endif

    current->flags |= PF_NOFREEZE;
    rc = nexus_generic_driver_ioctl(minor, file->private_data, cmd, arg);
    current->flags &= ~PF_NOFREEZE;
    
    if (rc) return -EFAULT;
    return 0;
}

MODULE_LICENSE("Proprietary");

#ifdef MODULE
int
init_module(void)
{
    int rc;
    unsigned i;
    struct nexus_generic_driver_init_settings settings;

    memset(&settings, 0, sizeof(settings));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    for (i=0;i<sizeof(settings.region)/sizeof(settings.region[0]);i++) {
        unsigned long offset, size;
        rc = bmem_region_info(i, &offset, &size);
        if (rc) break;
        settings.region[i].offset = offset;
        settings.region[i].size = size;
    }
#elif HAVE_RSRVD
#define LOWERMEM (256*1024*1024)
    if (get_RAM_size() > LOWERMEM) {
        /* get_RSVD_size() only applies to amount reserved from lower 256, not amount reserved from get_RAM_size() */
        settings.region[0].size = get_RSVD_size();
        settings.region[0].offset = LOWERMEM - settings.region[0].size;
        /* do not program settings.region[1] for upper 256. for 2.6.18, assume that it will be used by the kernel.
        2.6.31 and later gives options for nexus/magnum use. */
    }
    else {
        settings.region[0].size = get_RSVD_size();
        settings.region[0].offset = get_RAM_size() - settings.region[0].size;
    }
#else
#error
#endif

    /* If MEM's alignment is not set to the MIPS L1 and (if present) L2 cache line size,
    we will have cache coherency problems (which lead to major system failures).
    This code verifies that Nexus's MEM configuration is compatible with the MIPS cache line size.
    If this code fails, please check to make sure the Linux kernel is configured right, then modify nexus_core_features.h to match. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    {
        struct brcm_cache_info info;
        brcm_get_cache_info(&info);
        settings.max_dcache_line_size = info.max_writeback;
    }
#else
    settings.max_dcache_line_size = cpu_data[0].dcache.linesz; /* L1 cache */
    if (cpu_data[0].scache.linesz > settings.max_dcache_line_size) {
        settings.max_dcache_line_size = cpu_data[0].scache.linesz; /* L2 cache */
    }
    /* Ignoring cpu_data[0].tcache.linesz for now. */
#endif

    b_parse_env(b_env);
    rc = nexus_generic_driver_init(&settings);
    if (rc) goto error;

    register_chrdev(NEXUS_DRIVER_MAJOR, "nexus", &nexus_driver_fops);

    printk("nexus driver initialized\n");
    return 0;

error:
    return rc;
}

void
cleanup_module(void)
{
    unregister_chrdev(NEXUS_DRIVER_MAJOR, "nexus");
    nexus_generic_driver_uninit();

    printk("nexus driver uninitialized\n");
    return;
}
#else
#error "Not supported"
#endif
