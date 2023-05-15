/***************************************************************************
 *     Copyright (c) 2004-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_os.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 4/27/05 1:15p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_os.h $
 * 
 * 2   4/27/05 1:15p erickson
 * PR15075: use int64_t for 64 bit file offset instead of assuming off_t
 * is 64 bits (it's not on vxworks)
 * 
 * 1   2/7/05 7:57p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   7/14/04 1:45a erickson
 * PR11287: added more docs
 * 
 * Irvine_BSEAVSW_Devel/1   7/14/04 1:36a erickson
 * PR11287: refactor linux-specific code into brutus_linux.cpp for easier
 * porting
 * 
 ***************************************************************************/
#ifndef BRUTUS_OS_H__
#define BRUTUS_OS_H__

/*=*************************************
This file defines a set of functions for operating system specific 
code needed in brutus. It's like an extension to the "kernel interface".
Each OS needs its own implementation of these functions.
****************************************/

/**
Returns disk usage information for whatever disk partition is in use
for the specified path.

Return Values:
0 is success
**/
int b_get_disk_usage(const char *path, unsigned long *total, unsigned long *avail);

/**
Guarantee that all disk writes have been committed to disk.
**/
void b_sync_disk();

/**
Acquire a system-wide mutex. This must work across threads as well as linux application spaces.

Return Values:
0 is success
**/
int b_acquire_system_lock();

/**
Release a system-wide mutex.
**/
void b_release_system_lock();

/**
Delete a file.
This calls b_sync_disk() internally.

Return Values:
0 means either the file did not exist or it was deleted. It does not exist now.
non-zero if file could not be deleted and still exists.
**/
int b_delete_file(const char *filename);

/**
Get the size.

Return Values:
0 is success and size is accurate
non-zero if file does not exist or size could not be obtained.
**/
int b_get_file_size(const char *filename, int64_t *size);

#endif // BRUTUS_OS_H__
