/***************************************************************************
 *     Copyright (c) 2002-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: umdrv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/16/06 4:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/appframework/src/linuxuser/umdrv.h $
 * 
 * Hydra_Software_Devel/1   6/16/06 4:08p maivu
 * PR 22212: Move to board directory
 * 
 * Irvine_BSEAVSW_Devel/7   2/9/05 3:25p erickson
 * PR13958: added INT_RESET
 * 
 * Irvine_BSEAVSW_Devel/6   2/7/05 3:07p erickson
 * PR14045: added BRCM_IOCTL_CHANGE_REQUEST to wake up isr thread and
 * immediately apply mask changes
 * 
 * Irvine_BSEAVSW_Devel/5   11/25/03 4:57p marcusk
 * PR8740: Unified structure and collapsed driver to work on all
 * platforms.  Currently tested on 97038 and 97115 platforms.
 * 
 * Irvine_BSEAVSW_Devel/4   10/22/03 1:40p dlwin
 * Added hooks for W1 interrupts for 7038, has not been tested.
 * 
 * Irvine_HDDemo_Devel/3   9/12/02 9:39a aram
 * BRCM_IOCTL_DEBUG_LEVEL was added
 * 
 * Irvine_HDDemo_Devel/2   8/13/02 5:49p marcusk
 * Use copy_to_user and copy_from_user to pass data.
 * Did some minor cleanup (removed unused functions, structures, and
 * includes).
 * Added standard copyright notice.
 * Display driver version based upon clearcase version information.
 * 
 ***************************************************************************/
#ifndef __PVRDRIVER_H__
#define __PVRDRIVER_H__

/* 
 * This structure and IOCTL is used to support platforms with 32 and 64 bits of
 * L1 interrupts.  
 * NOTE: For platforms that only support 32 bits the W1 mask and status
 * will not be copied to and from user space.  This is to keep legacy applications
 * binary compatible with the new version of this driver.
 */
typedef struct s_bcm_linux_dd_interrupt
{
    int     timeout;
    unsigned char waitforallints;
	unsigned long interruptW0mask;
	unsigned long interruptW0status;
    unsigned long interruptW1mask;		/* ignore for 32 bit interrupt systems */
    unsigned long interruptW1status;	/* ignore for 32 bit interrupt systems */
}tbcm_linux_dd_interrupt;


#define BRCM_IOCTL_WAIT_FOR_INTERRUPTS      1011 /* use the tbcm_linux_dd_interrupt structure with this IOCTL */
#define BRCM_IOCTL_DEBUG_LEVEL              1012
#define BRCM_IOCTL_CHANGE_REQUEST           1013
#define BRCM_IOCTL_INT_RESET                1014

/*
 *	The following definitions and structures are restricted to the kernel build
 *	User app's include this file but they don't need the folowing definitions
 */
#ifdef __KERNEL__

/* utility function */
void brcm_get_dev_info(struct inode *inode, unsigned int *devnum, unsigned int *devmode);

#endif /* __KERNEL__ */

#endif /* __PVRDRIVER_H__ */

