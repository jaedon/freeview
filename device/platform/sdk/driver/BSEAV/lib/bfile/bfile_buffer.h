/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfile_buffer.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 6/2/09 7:38p $
 *
 * Module Description:
 *
 * Block based cached file I/O
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bfile/bfile_buffer.h $
 * 
 * 6   6/2/09 7:38p vsilyaev
 * PR 55417: Added support for read method returning no data or completing
 * with a partial read
 * 
 * 5   12/18/08 7:04p vsilyaev
 * PR 47854: Improved handling of conditions where asynchronous read
 * exhaust allocated buffer
 * 
 * 4   11/26/08 3:56p vsilyaev
 * PR 47650: Added function to get bounds
 * 
 * 3   9/18/08 3:27p vsilyaev
 * PR 47105: Added support for layered buffer cache
 * 
 * 2   6/14/07 12:49p vsilyaev
 * PR 32162: Serializes async disk I/O for bfile_buffer. Fixed unitilized
 * ref_cnt in bfile_buffer.c
 * 
 * 1   5/16/07 9:53p vsilyaev
 * PR 28631: Added block based file cache/buffer
 * 
 *******************************************************************************/
#ifndef _BFILE_BUFFER_H__
#define _BFILE_BUFFER_H__

#include "bfile_io.h"
#include "bioatom.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bfile_buffer *bfile_buffer_t;

typedef enum bfile_buffer_overflow_action {
    bfile_buffer_overflow_action_wait,
    bfile_buffer_overflow_action_abort
} bfile_buffer_overflow_action;

typedef struct bfile_buffer_cfg {
	bool async;
	unsigned nsegs;
	size_t buf_len;
	void *buf;
	void *sync_cnxt;
	bfile_io_read_t fd;
	void (*async_read)(void *sync_cnxt, bfile_io_read_t fd, void *buf, size_t length, void (*read_cont)(void *cont, ssize_t size), void *cntx);
    bfile_buffer_overflow_action (*async_overflow)(void *sync_cnxt);
} bfile_buffer_cfg;

typedef enum bfile_buffer_result {
	bfile_buffer_result_ok,
	bfile_buffer_result_read_error,
	bfile_buffer_result_eof,
    bfile_buffer_result_no_data, /* temporary no data in the file */
	bfile_buffer_result_async,
	bfile_buffer_result_buffer_overflow
} bfile_buffer_result;

void bfile_buffer_default_cfg(bfile_buffer_cfg *cfg);
bfile_buffer_t bfile_buffer_create(batom_factory_t factory, bfile_buffer_cfg *cfg);
void bfile_buffer_clear(bfile_buffer_t buf);
void bfile_buffer_destroy(bfile_buffer_t buf);
	
batom_t bfile_buffer_async_read(bfile_buffer_t buf, off_t off, size_t length, bfile_buffer_result *result, void (*read_complete)(void *, batom_t, bfile_buffer_result ), void *cntx);
batom_t bfile_buffer_read(bfile_buffer_t buf, off_t off, size_t length, bfile_buffer_result *result);
int bfile_buffer_get_bounds(bfile_buffer_t buf, off_t *first, off_t *last);

#ifdef __cplusplus
}
#endif


#endif /* _BFILE_BUFFER_H__ */


