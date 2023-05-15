/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_fileio.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 3/6/08 2:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_fileio.h $
 * 
 * 4   3/6/08 2:01p vsilyaev
 * PR 36248: Pass mpeg parameters into the brecord_file_open_network and
 * bfile_posix_out_network
 * 
 * 3   8/9/07 5:51p jgarrett
 * PR 33830: Adding network record prototypes
 * 
 * 2   4/22/05 8:02p vsilyaev
 * PR 14988: Added API to open "chunked" files.
 * 
 * 1   2/7/05 7:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   7/30/03 9:38a erickson
 * added fileio.h
 *
 ***************************************************************************/
#ifndef BSETTOP_FILEIO_H__
#define BSETTOP_FILEIO_H__

#include "bsettop_types.h"
#include "bsettop_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

brecord_file_t bfile_posix_out(const char *fname, const char *indexname);
brecord_file_t bfile_posix_out_network(const char *fname, const char *indexname, const bsocket_params *params, const bstream_mpeg * mpeg_info);
bplayback_file_t bfile_posix_in(const char *fname, const char *indexname);

/* for two functions bellow fname shall be name of already existing directory */
brecord_file_t bfile_chunk_out(const char *fname, const char *indexname, brecord_file_t (*open_out)(const char *fname, const char *indexname));
bplayback_file_t bfile_chunk_in(const char *fname, const char *indexname, bplayback_file_t (*open_in)(const char *fname, const char *indexname));

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_FILEIO_H__ */
