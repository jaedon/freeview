/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_cdxa.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/14/10 6:08p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_cdxa.h $
 * 
 * 1   7/14/10 6:08p vsilyaev
 * SW3556-1131: Added basic support for CDXA format
 * 
 *******************************************************************************/
#ifndef _BMEDIA_CDXA_H__
#define _BMEDIA_CDXA_H__

#include "bmedia_probe_impl.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern const bmedia_probe_format_desc bcdxa_probe;


typedef struct bcdxa_file *bcdxa_file_t;
bcdxa_file_t bcdxa_file_create(bfile_io_read_t fd);
void bcdxa_file_destroy(bcdxa_file_t fd);
bfile_io_read_t bcdxa_file_get_file_interface(bcdxa_file_t fd);

#ifdef __cplusplus
}
#endif


#endif /* _BMEDIA_CDXA_H__ */

