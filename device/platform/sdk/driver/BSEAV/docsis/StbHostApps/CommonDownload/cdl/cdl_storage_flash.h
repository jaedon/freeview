/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cdl_storage_flash.h $
 * $brcm_Revision: PROD_DSG_REFSW_Devel/1 $
 * $brcm_Date: 7/15/08 3:26p $
 *
 * Module Description:  OpenCable Common Download storage interface. Functions
 *                      in this module provides image storage
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/trinity/src/cdl/cdl_storage_flash.h $
 * 
 * PROD_DSG_REFSW_Devel/1   7/15/08 3:26p jackli
 * PR38602: restructure code
 * 
 * PROD_DSG_REFSW_Devel/2   5/1/08 3:20p jackli
 * PR38602: add host_download_control, and ATP related env variables
 * 
 * PROD_DSG_REFSW_Devel/1   1/8/08 4:34p jackli
 * PR38602: add common dowload support
 * 
 * 
 ***************************************************************************/
#ifndef CDL_STORAGE_H
#define CDL_STORAGE_H

#define CDL_FILE_TYPE_INVALID  0  /* invalid type */
#define CDL_FILE_TYPE_ECMBOOT  1  /* eCM bootloader */
#define CDL_FILE_TYPE_DOCSIS0  2  /* DOCSIS0 image */
#define CDL_FILE_TYPE_CFE      3  /* eSTB bootloader CFE */
#define CDL_FILE_TYPE_KERNEL   4  /* eSTB kernel */
#define CDL_FILE_TYPE_ROOTFS   5  /* eSTB root file system */


void * cdl_storage_flash_open(const char *pathname, int flags) ;
int cdl_storage_flash_close(void * h) ;
int cdl_storage_flash_read(void * h, void * buf, size_t count);
int cdl_storage_flash_write(void * h, const void * buf, size_t count);
int cdl_storage_flash_lseek(void * h, int offset, int whence);
int cdl_storage_flash_expand(void * h, char * fn, int sec_header_len);
#endif  /* CDL_STORAGE_H */
