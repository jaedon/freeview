/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_fileio.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/12/07 2:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_fileio.c $
 * 
 * 2   11/12/07 2:34p erickson
 * PR36068: update
 * 
 ***************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(fileio);

brecord_file_t bfile_posix_out(const char *fname, const char *indexname)
{
    BSTD_UNUSED(fname);
    BSTD_UNUSED(indexname);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

brecord_file_t bfile_posix_out_network(const char *fname, const char *indexname, const bsocket_params *params)
{
    BSTD_UNUSED(fname);
    BSTD_UNUSED(indexname);
    BSTD_UNUSED(params);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

bplayback_file_t bfile_posix_in(const char *fname, const char *indexname)
{
    BSTD_UNUSED(fname);
    BSTD_UNUSED(indexname);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

brecord_file_t bfile_chunk_out(const char *fname, const char *indexname, brecord_file_t (*open_out)(const char *fname, const char *indexname))
{
    BSTD_UNUSED(fname);
    BSTD_UNUSED(indexname);
    BSTD_UNUSED(open_out);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

bplayback_file_t bfile_chunk_in(const char *fname, const char *indexname, bplayback_file_t (*open_in)(const char *fname, const char *indexname))
{
    BSTD_UNUSED(fname);
    BSTD_UNUSED(indexname);
    BSTD_UNUSED(open_in);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

