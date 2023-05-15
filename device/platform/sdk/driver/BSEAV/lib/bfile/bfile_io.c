/***************************************************************************
 *     Copyright (c) 2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfile_io.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 4/7/06 3:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bfile/bfile_io.c $
 * 
 * 2   4/7/06 3:49p vsilyaev
 * PR 20680: Added bfile library
 * 
 * 1   4/7/06 12:31p vsilyaev
 * PR 20680: Added bfile library
 * 
 * 
 ***************************************************************************/
#include <bstd.h>
#include "bfile_io.h"

int 
bio_default_priority(void *cntx)
{
	BSTD_UNUSED(cntx);
	return 0;
}

void 
bio_read_attach_priority(bfile_io_read_t fd, bio_get_priority priority, void *cntx)
{
	fd->priority.get = priority;
	fd->priority.cntx = cntx;
	return;
}

void 
bio_write_attach_priority(bfile_io_write_t fd, bio_get_priority priority, void *cntx)
{
	fd->priority.get = priority;
	fd->priority.cntx = cntx;
	return;
}

