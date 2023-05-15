/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfile_cache.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 6/25/10 12:00p $
 *
 * Module Description:
 *
 * Linear file cache
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bfile/bfile_cache.h $
 * 
 * 9   6/25/10 12:00p vsilyaev
 * SW3548-2995: Separate requested reserve size and minimal size for
 * succesive parsing
 * 
 * 8   4/8/10 6:53p vsilyaev
 * SW7405-3773: Use 64-bit integer for segment size
 * 
 * 7   1/13/10 5:06p vsilyaev
 * SW3556-913: Remeber file I/O related errors and propagate them
 * 
 * 6   1/12/10 7:57p vsilyaev
 * SW3556-913: Separate data and index reads, allow incomplete data reads
 * 
 * 5   1/11/10 7:35p vsilyaev
 * SW3556-913: Added function to differentiate between end of cached data
 * and file error
 * 
 * 4   11/23/09 4:11p vsilyaev
 * SW7405-3368: Added parsing of MKV attachments
 * 
 * 3   8/18/08 12:41p gmohile
 * PR 42817 : Add support for AVI2.0
 * 
 * 2   3/3/08 12:22p vsilyaev
 * PR 39818: Increased segment size
 * 
 * 1   4/29/07 12:55a vsilyaev
 * PR 28631: File cache utilities
 * 
 * 
 *******************************************************************************/
#ifndef _BFILE_CACHE_H__
#define _BFILE_CACHE_H__

#include "bfile_io.h"
#include "bfile_buffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bfile_segment {
	off_t start;
	uint64_t len;
} bfile_segment;

void bfile_segment_clear(bfile_segment *segment);
void bfile_segment_set(bfile_segment *segment, off_t start, uint64_t len);
bool bfile_segment_test(const bfile_segment *segment);

typedef struct bfile_cache *bfile_cache_t;

bfile_cache_t bfile_cache_create(bfile_io_read_t fd, const bfile_segment *segment, size_t buf_size, size_t atom_size);
void bfile_cache_destroy(bfile_cache_t  cache);
int bfile_cache_seek(bfile_cache_t cache, size_t off);
const uint8_t *bfile_cache_next(bfile_cache_t cache);
void bfile_cache_clear( bfile_cache_t  cache);
bool bfile_cache_is_file_error(bfile_cache_t cache);
void bfile_segment_reset( bfile_cache_t  cache, bfile_segment *segment);

typedef enum bfile_segment_async_result {
	bfile_segment_async_result_success,
	bfile_segment_async_result_async,
	bfile_segment_async_result_eof,
	bfile_segment_async_result_no_data,
	bfile_segment_async_result_error
} bfile_segment_async_result;

typedef struct bfile_cached_segment {
	batom_cursor cursor; /* cursor that points to current parsing point in the accumulator */
	uint64_t accum_offset;
	batom_accum_t accum;
	size_t min_read_size;
	bfile_buffer_t buffer;
	bfile_segment segment; 
	bfile_buffer_result last_read_result;
	struct {
		size_t accum_size;
		size_t load_size;
		void *cntx;
		void (*read_complete)(void *, bfile_segment_async_result result);
	} async;
} bfile_cached_segment;


int bfile_cached_segment_init(bfile_cached_segment *segment, bfile_buffer_t buffer, batom_factory_t factory, size_t min_read_size);
void bfile_cached_segment_shutdown(bfile_cached_segment *segment);
void bfile_cached_segment_seek(bfile_cached_segment *segment, uint64_t offset);
uint64_t bfile_cached_segment_tell(bfile_cached_segment *segment);
void bfile_cached_segment_set(bfile_cached_segment *segment, off_t start, uint64_t len);
void bfile_cached_segment_async_read_complete(void *cntx, batom_t atom, bfile_buffer_result result);
bfile_segment_async_result bfile_cached_segment_async_reserve(bfile_cached_segment *segment, size_t reserve_size, void (*read_complete)(void *, bfile_segment_async_result ), void *cntx);
bool bfile_cached_segment_reserve(bfile_cached_segment *segment, size_t reserve_size);
bool bfile_cached_segment_reserve_custom_buffer(bfile_cached_segment *segment, size_t reserve_size, bfile_buffer_t buffer);
bool bfile_cached_segment_reserve_min(bfile_cached_segment *segment, size_t reserve_size, size_t min_size);
bool bfile_cached_segment_reserve_custom_buffer_min(bfile_cached_segment *segment, size_t reserve_size, size_t min_size, bfile_buffer_t buffer);



#ifdef __cplusplus
}
#endif


#endif /* _BFILE_CACHE_H__ */


