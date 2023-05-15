/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfile_buffered.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 1/14/10 1:03p $
 *
 * Module Description:
 *
 * Block based cached file I/O
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bfile/bfile_buffered.c $
 * 
 * 2   1/14/10 1:03p vsilyaev
 * SW3548-2711: Fixed memory overrun
 * 
 * 1   9/18/08 3:25p vsilyaev
 * PR 47105: Layered buffer cache that provides cached interface for any
 * file descriptor
 * 
 *******************************************************************************/

#include "bstd.h"
#include "bfile_buffered.h"
#include "bkni.h"


BDBG_MODULE(bfile_buffered);

BDBG_OBJECT_ID(bfile_buffered);

struct bfile_buffered_proxy {
	struct bfile_io_read file; /* shall be the first member */
    bfile_buffered_t parent;
};

struct bfile_buffered {
	struct bfile_io_read file; /* shall be the first member */
    BDBG_OBJECT(bfile_buffered)
    bfile_io_read_t fd;
    off_t pos;
    bfile_buffer_t buffer;
    bfile_buffered_cfg cfg;
    struct bfile_buffered_proxy proxy_fd;
    uint8_t buf[1]; /* variable size buffer */
};


void 
bfile_buffered_default_cfg(bfile_buffered_cfg *cfg)
{
    BDBG_ASSERT(cfg);
    cfg->nsegs = 8;
    cfg->buf_len = cfg->nsegs * (BIO_BLOCK_SIZE*4);
    return;
}

static ssize_t 
b_file_buffered_proxy_read(bfile_io_read_t fd, void *buf, size_t length)
{
	bfile_buffered_t file= ((struct bfile_buffered_proxy *)fd)->parent;
	BDBG_OBJECT_ASSERT(file, bfile_buffered);
    if(file->fd) {
        return file->fd->read(file->fd, buf, length);
    }
    return -1;
}

static off_t
b_file_buffered_proxy_seek(bfile_io_read_t fd, off_t offset, int whence)
{
	bfile_buffered_t file= ((struct bfile_buffered_proxy *)fd)->parent;
	BDBG_OBJECT_ASSERT(file, bfile_buffered);
    if(file->fd) {
        return file->fd->seek(file->fd, offset, whence);
    }
    return -1;
}

static int
b_file_buffered_proxy_bounds(bfile_io_read_t fd, off_t *first, off_t *last)
{
	bfile_buffered_t file= ((struct bfile_buffered_proxy *)fd)->parent;
	BDBG_OBJECT_ASSERT(file, bfile_buffered);
    if(file->fd) {
        return file->fd->bounds(file->fd, first, last);
    }
    *first = *last = 0;
    return -1;
}

static const struct bfile_io_read b_file_buffered_proxy_ops = {
	b_file_buffered_proxy_read,
	b_file_buffered_proxy_seek,
	b_file_buffered_proxy_bounds,
	BIO_DEFAULT_PRIORITY
};

static ssize_t 
b_file_buffered_read(bfile_io_read_t fd, void *buf, size_t length)
{
	bfile_buffered_t file=(void *)fd;
	BDBG_OBJECT_ASSERT(file, bfile_buffered);
    if(file->fd) {
        bfile_buffer_result result;
        batom_t data = bfile_buffer_read(file->buffer, file->pos, length, &result);
        if(data) {
            size_t len;
            batom_cursor cursor;

            batom_cursor_from_atom(&cursor, data);
            len = batom_cursor_copy(&cursor, buf, length);
            file->pos += len;
            batom_release(data);
            return (ssize_t) len;
        }
    }
    return -1;
}

static off_t
b_file_buffered_seek(bfile_io_read_t fd, off_t offset, int whence)
{
	bfile_buffered_t file=(void *)fd;
	BDBG_OBJECT_ASSERT(file, bfile_buffered);
    if(file->fd) {
        off_t first, last;
        int rc;

        switch(whence) {
        case SEEK_CUR:
        case SEEK_END:
            rc = file->fd->bounds(file->fd, &first, &last);
            if(rc<0) { goto error;}
            if(whence==SEEK_END) {
                offset = last + offset;
            } else {
                offset = file->pos + offset;
            }
            if(offset<first) {
                offset = first;
            } else if (offset>last) {
                offset = last;
            }
            /* keep going */
        case SEEK_SET:
        default:
            file->pos = offset;
            break;
        }
        return offset;
    }
error:
    return -1;
}

static int
b_file_buffered_bounds(bfile_io_read_t fd, off_t *first, off_t *last)
{
	bfile_buffered_t file=(void *)fd;
	BDBG_OBJECT_ASSERT(file, bfile_buffered);
    if(file->fd) {
        return file->fd->bounds(file->fd, first, last);
    }
    *first = *last = 0;
    return -1;
}

static const struct bfile_io_read b_file_buffered_ops = {
	b_file_buffered_read,
	b_file_buffered_seek,
	b_file_buffered_bounds,
	BIO_DEFAULT_PRIORITY
};


bfile_buffered_t 
bfile_buffered_create(batom_factory_t factory, bfile_buffered_cfg *cfg)
{
    bfile_buffered_t  file;
    bfile_buffer_cfg buffer_cfg;
    BERR_Code rc;

    BDBG_ASSERT(factory);
    BDBG_ASSERT(cfg);

    file = BKNI_Malloc(sizeof(*file)+cfg->buf_len);
    if(!file) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BDBG_OBJECT_INIT(file, bfile_buffered);
    file->cfg = *cfg;
    file->fd = NULL;
    file->pos = 0;
    file->proxy_fd.parent = file;
    file->proxy_fd.file = b_file_buffered_proxy_ops;
    file->file = b_file_buffered_ops;

    bfile_buffer_default_cfg(&buffer_cfg);
    buffer_cfg.nsegs = cfg->nsegs;
    buffer_cfg.buf_len = cfg->buf_len;
    buffer_cfg.buf = &file->buf;
    buffer_cfg.fd = &file->proxy_fd.file;
    file->buffer = bfile_buffer_create(factory, &buffer_cfg);
    if(!file->buffer) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_buffer;}

    return file;
err_buffer:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

void 
bfile_buffered_destroy(bfile_buffered_t file)
{
    BDBG_OBJECT_ASSERT(file, bfile_buffered);

    bfile_buffer_destroy(file->buffer);
    BDBG_OBJECT_DESTROY(file, bfile_buffered);
    BKNI_Free(file);
    return;
}


bfile_io_read_t 
bfile_buffered_attach(bfile_buffered_t file, bfile_io_read_t fd)
{
    BDBG_OBJECT_ASSERT(file, bfile_buffered);
    BDBG_ASSERT(fd);

    if(file->fd) {
        BDBG_ERR(("bfile_buffered_attach: %#lx already attached file %#lx", (unsigned long)file, (unsigned long)fd));
    }

    file->pos = 0;
    file->fd = fd;
    return &file->file;
}

void 
bfile_buffered_detach(bfile_buffered_t file)
{
    BDBG_OBJECT_ASSERT(file, bfile_buffered);
    if(!file->fd) {
        BDBG_ERR(("bfile_buffered_detach: %#lx no attached file", (unsigned long)file));
    }
    bfile_buffer_clear(file->buffer);
    file->fd = NULL;
    file->pos = 0;
    return;
}


bfile_buffer_t 
bfile_buffered_get_buffer(bfile_buffered_t file)
{
    BDBG_OBJECT_ASSERT(file, bfile_buffered);
    return file->buffer;
}
