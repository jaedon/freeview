/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cdl_bootinfo_nvram.h $
 * $brcm_Revision: PROD_DSG_REFSW_Devel/1 $
 * $brcm_Date: 7/15/08 3:24p $
 *
 * Module Description:  OpenCable Common Download storage interface. Functions
 *                      in this module provides nvram access.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/trinity/src/cdl/cdl_bootinfo_nvram.h $
 * 
 * PROD_DSG_REFSW_Devel/1   7/15/08 3:24p jackli
 * PR38602: restructure code
 * 
 ***************************************************************************/
#ifndef CDL_BOOTINFO_NVRAM_H
#define CDL_BOOTINFO_NVRAM_H

int cdl_bootinfo_nvram_open(char *dev_name);
int cdl_bootinfo_nvram_close();
int cdl_bootinfo_nvram_just_started();
int cdl_bootinfo_nvram_ready_to_reboot();
int cdl_bootinfo_nvram_get_new_storage(char * fn_kernel, char * fn_bootloader);
char * cdl_bootinfo_nvram_get_dev_name();
int cdl_bootinfo_nvram_setenv(char * varname, char * val);
int cdl_bootinfo_nvram_getenv(char * varname, char * val);
int cdl_bootinfo_nvram_printenv(void);
#endif  /* CDL_BOOTINFO_NVRAM_H */

