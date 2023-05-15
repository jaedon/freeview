/***************************************************************************
 *     Copyright (c) 2005-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_fileio_fifo.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 6/18/10 3:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_fileio_fifo.c $
 * 
 * 4   6/18/10 3:09p erickson
 * SW7550-424: fix PVR_SUPPORT=n
 *
 * 3   6/10/10 4:56p erickson
 * SWDEPRECATED-2726: implement bsettop_fileio_fifo
 *
 * 2   11/12/07 2:34p erickson
 * PR36068: update
 *
 ***************************************************************************/
#include "bsettop_impl.h"
#include "bsettop_fileio_fifo.h"

BDBG_MODULE(fileio_fifo);

bfile_out_fifo_t bfile_fifo_out_create(const char *mpeg_file_name, const char *index_file_name)
{
#if NEXUS_HAS_RECORD
    /* bfile_out_fifo_t is pointing to the same instance as brecord_file_t */
    return (bfile_out_fifo_t)brecord_p_file_open(mpeg_file_name, index_file_name, true);
#else
    BSTD_UNUSED(mpeg_file_name);
    BSTD_UNUSED(index_file_name);
    BERR_TRACE(berr_not_supported);
    return NULL;
#endif
}

brecord_file_t bfile_fifo_out_file(bfile_out_fifo_t file)
{
#if NEXUS_HAS_RECORD
    return (brecord_file_t)file;
#else
    BSTD_UNUSED(file);
    BERR_TRACE(berr_not_supported);
    return NULL;
#endif
}

bresult bfile_fifo_out_position(bfile_out_fifo_t file, bpvr_position *first, bpvr_position *last)
{
#if NEXUS_HAS_RECORD
    NEXUS_Error rc;
    brecord_file_t record_file = (brecord_file_t)file;

    if (!record_file->nFileFifo) {
        return BERR_TRACE(berr_not_supported);
    }

    BDBG_CASSERT(sizeof(bpvr_position) == sizeof(NEXUS_FilePosition));
    rc = NEXUS_FifoRecord_GetPosition(record_file->nFileFifo, (NEXUS_FilePosition *)first, (NEXUS_FilePosition *)last);
    if (rc) return BERR_TRACE(rc);

    return 0;
#else
    BSTD_UNUSED(file);
    BSTD_UNUSED(first);
    BSTD_UNUSED(last);
    return BERR_TRACE(berr_not_supported);
#endif
}

bplayback_file_t bfile_fifo_in_open(const char *mpeg_file_name, const char *index_file_name, bfile_out_fifo_t writer)
{
#if NEXUS_HAS_RECORD
    if (!writer) {
        BERR_TRACE(berr_not_supported);
        return NULL;
    }
    return bplayback_p_file_open(mpeg_file_name, index_file_name, writer);
#else
    BSTD_UNUSED(mpeg_file_name);
    BSTD_UNUSED(index_file_name);
    BSTD_UNUSED(writer);
    BERR_TRACE(berr_not_supported);
    return NULL;
#endif
}

void bfile_fifo_out_get(bfile_out_fifo_t file, bfile_out_settings *settings)
{
#if NEXUS_HAS_RECORD
    brecord_file_t record_file = (brecord_file_t)file;

    if (!record_file->nFileFifo) {
        BERR_TRACE(berr_not_supported);
        return;
    }

    BDBG_CASSERT(sizeof(bfile_out_settings) == sizeof(NEXUS_FifoRecordSettings));
    NEXUS_FifoRecord_GetSettings(record_file->nFileFifo, (NEXUS_FifoRecordSettings *)settings);
#else
    BSTD_UNUSED(file);
    BSTD_UNUSED(settings);
    BERR_TRACE(berr_not_supported);
#endif
}

bresult bfile_fifo_out_set( bfile_out_fifo_t file, const bfile_out_settings *settings)
{
#if NEXUS_HAS_RECORD
    NEXUS_Error rc;
    brecord_file_t record_file = (brecord_file_t)file;

    if (!record_file->nFileFifo) {
        return BERR_TRACE(berr_not_supported);
    }

    BDBG_CASSERT(sizeof(bfile_out_settings) == sizeof(NEXUS_FifoRecordSettings));
    rc = NEXUS_FifoRecord_SetSettings(record_file->nFileFifo, (const NEXUS_FifoRecordSettings *)settings);
    if (rc) return BERR_TRACE(rc);

    return 0;
#else
    BSTD_UNUSED(file);
    BSTD_UNUSED(settings);
    return BERR_TRACE(berr_not_supported);
#endif
}

/**
The following are low-level functions, not required for bfile_fifo_out.
**/

struct bfile_io_write_fifo *bpvrfifo_write_open(const char *fname, int flags, off_t start)
{
    BSTD_UNUSED(fname);
    BSTD_UNUSED(flags);
    BSTD_UNUSED(start);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

bfile_io_write_t bpvrfifo_write_file(struct bfile_io_write_fifo *file)
{
    BSTD_UNUSED(file);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

void bpvrfifo_write_close(struct bfile_io_write_fifo *file)
{
    BSTD_UNUSED(file);
    BSETTOP_ERROR(berr_not_supported);
}

struct bfile_io_read_fifo *bpvrfifo_read_open(const char *fname, int flags, struct bfile_io_write_fifo *writer)
{
    BSTD_UNUSED(fname);
    BSTD_UNUSED(flags);
    BSTD_UNUSED(writer);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

bfile_io_read_t bpvrfifo_read_file(struct bfile_io_read_fifo *file)
{
    BSTD_UNUSED(file);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

void bpvrfifo_read_close(struct bfile_io_read_fifo *file)
{
    BSTD_UNUSED(file);
    BSETTOP_ERROR(berr_not_supported);
}

