/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_file_posix.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 7/31/12 12:12a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file/src/no_os/nexus_file_posix.c $
 * 
 * 3   7/31/12 12:12a agin
 * SW7425-2821:  sync to
 *  /nexus/modules/file/src/linuxuser/nexus_file_posix.c /main/17
 * 
 * 2   3/18/12 6:12p agin
 * SWNOOS-523:  Support 97429 no-os.
 * 
 * 1   10/7/11 10:19a agin
 * SWNOOS-489:  Relevel 97425 no-os to prerel for B0.
 * 
 * 1   9/7/11 2:06p agin
 * SWNOOS-481:  Relevel 97125 no-os to 20110811 Phase 7.5.
 * 
 * 12   6/6/11 3:27p vsilyaev
 * SW7405-4477: Routed all debug output through buffer and use external
 *  application to extract and print debug output
 * 
 * 11   4/7/11 6:13p vsilyaev
 * SW7425-232: Updated file mux module
 * 
 * 10   4/6/11 6:53p vsilyaev
 * SW7425-232: Removed MuxFileIo API to avoid naming clashes
 * 
 * 9   4/6/11 4:45p vsilyaev
 * SW7425-232: Added MuxFile interface
 * 
 * 8   11/16/10 7:10p ttrammel
 * SW7420-1228: Initial Android check-in SQA fixes.
 * 
 * 7   4/2/10 2:37p erickson
 * SW7405-3833: nexus_file_pvr.h is now part of the public API
 * 
 * 6   1/7/09 9:32p erickson
 * PR49501: allow NEXUS_FileRecord_AppendPosix to append to an index
 *
 * 5   12/3/08 12:33p vsilyaev
 * PR 49501: Added NEXUS_FileRecord_AppendPosix
 *
 * 4   5/16/08 3:31p vsilyaev
 * PR 42365: Flush cache prior to read operation, otherwise there is a
 *  risk that dirtly cache lines would wipeout just read data
 *
 * 3   4/3/08 2:02p vsilyaev
 * PR 39818: Optimized MKV handling
 *
 * 2   2/6/08 4:38p erickson
 * PR39389: set bfile_io_write_posix_open appropriately. updated O_DIRECT
 *  failure comments.
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/5   11/13/07 1:01p vsilyaev
 * PR 37015: Improved use of O_DIRECT in API
 *
 * Nexus_Devel/4   11/13/07 11:53a vsilyaev
 * PR 37015: Use common API to open read/write files
 *
 * Nexus_Devel/3   11/13/07 11:12a vsilyaev
 * PR 37015: Initial checking for timeshifting/FIFO file
 *
 * Nexus_Devel/2   10/12/07 5:11p vsilyaev
 * PR 35824: Added O_DIRECT flag, fixed lock/unlock order
 *
 * Nexus_Devel/1   10/10/07 3:58p vsilyaev
 * PR 35824: File I/O module
 *
 * $copied_brcm_Log: /BSEAV/api/src/pvr/bsettop_fileio.c $
 * $copied_brcm_Revision: 38 $
 * $copied_brcm_Date: 9/24/07 3:52p $
 **************************************************************************/
#include "nexus_file_module.h"
#include "bfile_io.h"
#include "nexus_file_pvr.h"
#include "nexus_file_posix.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/uio.h>

#ifdef B_REFSW_ANDROID
#include <sys/stat.h>
#endif

BDBG_MODULE(nexus_file_posix);
#define BDBG_MSG_TRACE(x) /*BDBG_MSG(x)*/


static ssize_t
posix_read(bfile_io_read_t fd, void *buf, size_t length)
{
    struct bfile_io_read_posix *file = (struct bfile_io_read_posix *) fd;
    ssize_t rc;

#if 0
    if(file->direct) {
        NEXUS_FlushCache((void *)buf, length); /* PR:31841 O_DIRECT doesn't flush CPU cache */
    }
    rc = read(file->fd, buf, length);
    BDBG_MSG_TRACE(("posix_read: %#lx %#lx %u:%u -> %d", (unsigned long)fd, (unsigned long)buf, (unsigned)(lseek(file->fd, 0, SEEK_CUR)-rc), (unsigned)length, (int)rc));

    if (rc<0 && file->direct==true) {
        int flags;
        BDBG_WRN(("Error during O_DIRECT read; removing O_DIRECT flag. Performance will suffer greatly. buf=%x, length=%d", buf, length));
        file->direct = false;
        flags = fcntl(file->fd, F_GETFL);
        if (flags!=-1) {
            flags &= ~O_DIRECT;
            if (fcntl(file->fd, F_SETFL, flags)==0) {
                rc = read(file->fd, buf, length);
                goto read_done;
            }
        }
        BDBG_ERR(("Can't remove O_DIRECT flag"));
    }
#endif
read_done:
    return rc;
}

static off_t
posix_seek(bfile_io_read_t fd, off_t offset, int whence)
{
    struct bfile_io_read_posix *file = (struct bfile_io_read_posix *) fd;
    off_t off;

#if 0
    off = lseek(file->fd,offset,whence);
    BDBG_MSG_TRACE(("posix_seek: %#lx %lu(%d) -> %ld", (unsigned long)fd, (unsigned long)offset, whence, (long)off));
#endif
    return off;
}

static int
posix_bounds(bfile_io_read_t fd, off_t *first, off_t *last)
{
#if 0
    struct bfile_io_read_posix *file = (struct bfile_io_read_posix *) fd;
    struct stat sb;

    *first = 0;
    *last = 0;
    if (fstat(file->fd, &sb)<0) {
        return -1;
    }
    *last=sb.st_size;
#endif
    return 0;
}

static const struct bfile_io_read posix_io_read = {
    posix_read,
    posix_seek,
    posix_bounds,
    BIO_DEFAULT_PRIORITY
};

NEXUS_Error
bfile_io_read_posix_open(struct bfile_io_read_posix *file, const char *fname, bool direct)
{
#if 0
    unsigned flags = O_RDONLY;
    NEXUS_Error rc;

    BDBG_ASSERT(file);
    BDBG_ASSERT(fname);
    file->direct = direct;
    flags |= O_LARGEFILE;
#ifdef DIRECT_IO_SUPPORT
    if(direct) {
        flags |= O_DIRECT;
    }
#endif
    file->fd = open(fname, flags, 0666);
#ifdef DIRECT_IO_SUPPORT
    if (file->fd<0) {
        if (file->direct) {
            BDBG_WRN(("Error during O_DIRECT read open; removing O_DIRECT flag. Performance will suffer greatly."));
            flags &= ~O_DIRECT;
            file->direct = false;
        }
        file->fd = open(fname, flags, 0666);
    }
#endif
    if (file->fd<0) { rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_open; }
    fcntl(file->fd, FD_CLOEXEC, 1);
    file->self = posix_io_read;
    return NEXUS_SUCCESS;

err_open:
    return rc;
#else
    return NEXUS_SUCCESS;
#endif
}

void
bfile_io_read_posix_close(struct bfile_io_read_posix *file)
{
    if(file->fd>0) {
        close(file->fd);
        file->fd = -1;
    }
    return;
}


static ssize_t
posix_write(bfile_io_write_t fd, const void *buf, size_t length)
{
#if 0
    struct bfile_io_write_posix *file = (struct bfile_io_write_posix *) fd;
    ssize_t rc;

#ifdef FILEIO_TIMING
    struct timeval diff, end, begin,tmp,prev;
    gettimeofday(&begin, NULL);
    prev = file->st.cur;
#endif

    rc = write(file->fd, (void*)buf, length);
    BDBG_MSG_TRACE(("posix_write: %#lx %#lx %u:%u -> %d", (unsigned long)fd, (unsigned long)buf, (unsigned)(lseek(file->fd, 0, SEEK_CUR)-rc), (unsigned)length, (int)rc));

    if (rc < 0 && (errno == ENOSPC || errno == EIO)) {
        BDBG_ERR(("%s:%d: Critical file error!, errno %d", __FILE__, __LINE__, errno));
        goto write_done;
    }

    if (rc<0 && file->direct==true) {
        int flags;
        BDBG_WRN(("Error during O_DIRECT write; removing O_DIRECT flag. Performance will suffer greatly. buf=%x, length=%d", buf, length));
        file->direct = false;
        flags = fcntl(file->fd, F_GETFL);
        if (flags!=-1) {
            flags &= ~O_DIRECT;
            if (fcntl(file->fd, F_SETFL, flags)==0) {
                rc = write(file->fd, buf, length);
                goto write_done;
            }
        }
        BDBG_ERR(("Can't remove O_DIRECT flag"));
    }
write_done:

#ifdef FILEIO_TIMING
    gettimeofday(&end, NULL);
    timersub(&end, &begin, &diff);
    timeradd(&file->st.cur, &diff, &tmp);
    file->st.cur = tmp;
    if (diff.tv_usec > 50000 || diff.tv_sec>0) {
        int msec_total, msec;
        fprintf(stderr, "(%p) bs=%d WHOA! time=%ld msec, # of good=%d", &file->st, length, diff.tv_usec/1000, file->st.good_cnt);
        timersub(&prev, &file->st.last_good, &diff);
        msec = (diff.tv_usec/1000) + (diff.tv_sec*1000);
        if (msec<=0) msec=1;
        timersub(&file->st.cur, &file->st.begin, &diff);
        msec_total = (diff.tv_usec/1000) + (diff.tv_sec*1000);
        fprintf(stderr, "(from good %d msec %dKBytes/sec, total %dKBytes/sec)\n", msec,
               (int)(((long long)(length/1000)*(file->st.good_cnt)*1024)/(msec)),
               (int)(((long long)(length/1000)*(file->st.writes+1)*1024)/(msec_total))
               );
          file->st.good_cnt=0;
          file->st.last_good=file->st.cur;
    } else {
        file->st.good_cnt++;
    }
    file->st.writes++;
#endif
    return rc;
#else
	return 1;
#endif
}

static off_t
posix_trim(bfile_io_write_t fd, off_t trim_pos)
{
    BSTD_UNUSED(fd);
    BSTD_UNUSED(trim_pos);
    return 0;
}

off_t
bfile_io_write_posix_seek(struct bfile_io_write_posix *file, off_t offset, int whence)
{
    off_t off;
    BDBG_ASSERT(file);
    off = lseek(file->fd, offset, whence);
    BDBG_MSG_TRACE(("posix_seek(wr): %#lx %lu(%d) -> %ld", (unsigned long)file->fd, (unsigned long)offset, whence, (long)off));
    return off;
}

static const struct bfile_io_write posix_io_write = {
    posix_write,
    posix_trim,
    BIO_DEFAULT_PRIORITY
};




struct bfile_out_posix {
    struct NEXUS_FileRecord self;
    struct bfile_io_write_posix data, index;
};

static NEXUS_Error
b_file_io_write_posix_open(struct bfile_io_write_posix *file, const char *fname, unsigned flags, bool direct, bool sync)
{
#if 0
    NEXUS_Error rc;

    BDBG_ASSERT(file);
    BDBG_ASSERT(fname);
    file->direct = direct;
    flags |= O_LARGEFILE;
    if(sync) {
        flags |= O_SYNC;
    }
#ifdef DIRECT_IO_SUPPORT
    if(direct) {
        flags |= O_DIRECT;
    }
#endif
    file->fd = open(fname, flags, 0666);
#ifdef DIRECT_IO_SUPPORT
    if (file->fd<0) {
        if (file->direct) {
            BDBG_WRN(("Error during O_DIRECT write open; removing O_DIRECT flag. Performance will suffer greatly."));
            flags &= ~O_DIRECT;
            file->direct = false;
        }
        file->fd = open(fname, flags, 0666);
    }
#endif
    if (file->fd<0) { rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_open; }
    fcntl(file->fd, FD_CLOEXEC, 1);
    file->self = posix_io_write;
#ifdef FILEIO_TIMING
    file->st.begin.tv_usec = file->st.begin.tv_sec = 0;
    file->st.cur=file->st.last_good=file->data.st.begin;
    file->st.writes = file->st.good_cnt = 0;
#endif
    return NEXUS_SUCCESS;

err_open:
    return rc;
#else
    return NEXUS_SUCCESS;
#endif
}

NEXUS_Error
bfile_io_write_posix_open(struct bfile_io_write_posix *file, const char *fname, bool direct)
{
    return b_file_io_write_posix_open(file, fname, O_CREAT|O_TRUNC|O_WRONLY, direct, false);
}

void
bfile_io_write_posix_close(struct bfile_io_write_posix *file)
{
    if(file->fd>0) {
        close(file->fd);
        file->fd = -1;
    }
    return;
}

static void
posix_close_out(struct NEXUS_FileRecord *file)
{
    struct bfile_out_posix *posix=  (struct bfile_out_posix *)file;

    BDBG_MSG(("close_out %#x", (unsigned)file));
    if(file->index) {
        bfile_io_write_posix_close(&posix->index);
    }
    bfile_io_write_posix_close(&posix->data);
    BKNI_Free(posix);
    return;
}

NEXUS_FileRecordHandle NEXUS_FileRecord_Open(const NEXUS_FileRecordOpenSettings *pSettings)
{
    NEXUS_Error rc;
    struct bfile_out_posix *file;
    unsigned flags;

    if(!pSettings) {rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_parameter;}
    flags =  pSettings->append ? O_CREAT|O_APPEND|O_WRONLY : O_CREAT|O_TRUNC|O_WRONLY;


    file =  BKNI_Malloc(sizeof(*file));
    if (!file) { rc  = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }

    if (pSettings->data.filename) {
        rc = b_file_io_write_posix_open(&file->data, pSettings->data.filename, flags, pSettings->data.directIo, pSettings->data.sync);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_data; }
        file->self.data = &file->data.self;
    } else {
        file->self.data = NULL;
    }

    if (pSettings->index.filename) {
        rc = b_file_io_write_posix_open(&file->index, pSettings->index.filename, flags, false, pSettings->data.sync);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_index; }
        file->self.index = &file->index.self;
    } else {
        file->self.index = NULL;
    }

    file->self.close = posix_close_out;

    return &file->self;

err_open_index:
    close(file->data.fd);
err_open_data:
    BKNI_Free(file);
err_alloc:
err_parameter:
    return NULL;
}

NEXUS_FileRecordHandle
NEXUS_FileRecord_OpenPosix(const char *fname, const char *indexname)
{
    NEXUS_FileRecordOpenSettings settings;

    NEXUS_FileRecord_GetDefaultOpenSettings(&settings);
    settings.data.filename = fname;
    settings.index.filename = indexname;
    return NEXUS_FileRecord_Open(&settings);
}

NEXUS_FileRecordHandle
NEXUS_FileRecord_AppendPosix(const char *fname, const char *indexname)
{
    NEXUS_FileRecordOpenSettings settings;
    NEXUS_FileRecord_GetDefaultOpenSettings(&settings);
    settings.append = true;
    settings.data.filename = fname;
    settings.index.filename = indexname;
    return NEXUS_FileRecord_Open(&settings);
}

struct bfile_in_posix {
    struct NEXUS_FilePlay self;
    struct bfile_io_read_posix data, index;
};

static void
posix_close_in(struct NEXUS_FilePlay *file)
{
    struct bfile_in_posix *posix=  (struct bfile_in_posix *)file;

    BDBG_MSG(("close_in %#x", (unsigned)file));
    if (file->file.index) {
        bfile_io_read_posix_close(&posix->index);
    }
    bfile_io_read_posix_close(&posix->data);
    BKNI_Free(posix);
    return;
}

NEXUS_FilePlayHandle NEXUS_FilePlay_Open(const NEXUS_FilePlayOpenSettings *pSettings)
{
    NEXUS_Error rc;
    struct bfile_in_posix *file; 

    if(!pSettings) {rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_parameter;}
    if(pSettings->data.filename == NULL) {rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_parameter;}
    
    file =  BKNI_Malloc(sizeof(*file));
    if (!file) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }

    rc = bfile_io_read_posix_open(&file->data, pSettings->data.filename, pSettings->data.directIo);
    if (rc!=NEXUS_SUCCESS) {
        BDBG_ERR(("Unable to open input file: '%s', errno %d", pSettings->data.filename, errno));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_open_data;
    }

    if (pSettings->index.filename) {
        rc = bfile_io_read_posix_open(&file->index, pSettings->index.filename, false);
        if (rc!=NEXUS_SUCCESS) {
            BDBG_ERR(("Unable to open index file: '%s', errno %d", pSettings->index.filename, errno));
            rc = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto err_open_index;
        }
        file->self.file.index = &file->index.self;
    } else {
        file->self.file.index = NULL;
    }
    file->self.file.data = &file->data.self;
    file->self.file.close = posix_close_in;

    return &file->self;

err_open_index:
    bfile_io_read_posix_close(&file->data);
err_open_data:
    BKNI_Free(file);
err_alloc:
err_parameter:
    return NULL;
}

NEXUS_FilePlayHandle
NEXUS_FilePlay_OpenPosix(const char *fname, const char *indexname)
{
    NEXUS_FilePlayOpenSettings settings;

    NEXUS_FilePlay_GetDefaultOpenSettings(&settings);
    settings.data.filename = fname;
    settings.index.filename = indexname;
    return NEXUS_FilePlay_Open(&settings);
}

void
NEXUS_FilePlay_Close(NEXUS_FilePlayHandle file)
{
    BDBG_ASSERT(file);
    file->file.close(file);
    return;
}

void
NEXUS_FileRecord_Close(NEXUS_FileRecordHandle file)
{
    BDBG_ASSERT(file);
    file->close(file);
    return;
}

BDBG_OBJECT_ID(bfile_io_mux_posix);

static void
bfile_io_mux_posix_close(bfile_io_mux_t fd)
{
    struct bfile_io_mux_posix *file = (struct bfile_io_mux_posix *)fd;
    BDBG_OBJECT_ASSERT(file, bfile_io_mux_posix);
    if(file->fd>0) {
        close(file->fd);
        file->fd = -1;
    }
    BDBG_OBJECT_DESTROY(file, bfile_io_mux_posix);
    BKNI_Free(file);
    return;
}

static bool
b_file_io_mux_try_clear_odirect(struct bfile_io_mux_posix *file, const struct iovec *vec)
{
#if 0
    int flags;
    BSTD_UNUSED(vec);
    BDBG_WRN(("Error during O_DIRECT I/O; removing O_DIRECT flag. Performance will suffer greatly. buf=%x, length=%d", vec[0].iov_base, vec[0].iov_len));
    file->direct = false;
    flags = fcntl(file->fd, F_GETFL);
    if (flags!=-1) {
        flags &= ~O_DIRECT;
        if (fcntl(file->fd, F_SETFL, flags)==0) {
            return true;
        }
    }
    BDBG_ERR(("Can't remove O_DIRECT flag"));
    return false;
#else
    return true;
#endif
}


static ssize_t
bfile_io_mux_posix_read(bfile_io_mux_t fd, off_t offset, const NEXUS_FileMuxIoReadAtom *atoms, unsigned atom_count)
{
#if 0
    ssize_t rc;
    off_t rc_offset;
    struct bfile_io_mux_posix *file = (struct bfile_io_mux_posix *)fd;
    BDBG_OBJECT_ASSERT(file, bfile_io_mux_posix);
    NEXUS_ASSERT_FIELD(NEXUS_FileMuxIoReadAtom, base, struct iovec, iov_base);
    NEXUS_ASSERT_FIELD(NEXUS_FileMuxIoReadAtom, len, struct iovec, iov_len);
    NEXUS_ASSERT_STRUCTURE(NEXUS_FileMuxIoReadAtom, struct iovec);
    for(;;) {
        rc = -1;
        rc_offset = lseek(file->fd, offset, SEEK_SET);
        if(rc_offset == offset)  {
            rc = readv(file->fd, (const struct iovec *) atoms, atom_count);
        }
        if (rc<0 && file->direct==true) {
            if(b_file_io_mux_try_clear_odirect(file, (const struct iovec *)atoms)) {
                continue;
            }
        }
        break;
    }
    return rc;
#else
	return 1;
#endif
}

static ssize_t
bfile_io_mux_posix_write(bfile_io_mux_t fd, off_t offset, const NEXUS_FileMuxIoWriteAtom *atoms, unsigned atom_count)
{
#if 0
    ssize_t rc;
    off_t rc_offset;
    struct bfile_io_mux_posix *file = (struct bfile_io_mux_posix *)fd;
    BDBG_OBJECT_ASSERT(file, bfile_io_mux_posix);
    NEXUS_ASSERT_FIELD(NEXUS_FileMuxIoWriteAtom, base, struct iovec, iov_base);
    NEXUS_ASSERT_FIELD(NEXUS_FileMuxIoWriteAtom, len, struct iovec, iov_len);
    NEXUS_ASSERT_STRUCTURE(NEXUS_FileMuxIoWriteAtom, struct iovec);
    for(;;) {
        rc = -1;
        rc_offset = lseek(file->fd, offset, SEEK_SET);
        if(rc_offset == offset)  {
            rc = writev(file->fd, (const struct iovec *) atoms, atom_count);
        }
        if (rc<0 && file->direct==true) {
            if(b_file_io_mux_try_clear_odirect(file, (const struct iovec *)atoms)) {
                continue;
            }
        }
        break;
    }
    return rc;
#else
	return 1;
#endif
}


static const struct NEXUS_MuxFile posix_io_mux = {
    {
        bfile_io_mux_posix_read,
        bfile_io_mux_posix_write,
        BIO_DEFAULT_PRIORITY
    },
    bfile_io_mux_posix_close
};

NEXUS_Error
b_file_io_mux_posix_open(struct bfile_io_mux_posix *file, const char *fname, unsigned flags, bool direct)
{
#if 0
    NEXUS_Error rc;

    BDBG_ASSERT(file);
    BDBG_ASSERT(fname);
    BDBG_OBJECT_INIT(file, bfile_io_mux_posix);
    file->direct = direct;
    flags |= O_CREAT | O_TRUNC | O_RDWR;
    flags |= O_LARGEFILE;
#ifdef DIRECT_IO_SUPPORT
    if(direct) {
        flags |= O_DIRECT;
    }
#endif
    file->fd = open(fname, flags, 0666);
#ifdef DIRECT_IO_SUPPORT
    if (file->fd<0) {
        if (file->direct) {
            BDBG_WRN(("Error during O_DIRECT write open; removing O_DIRECT flag. Performance will suffer greatly."));
            flags &= ~O_DIRECT;
            file->direct = false;
        }
        file->fd = open(fname, flags, 0666);
    }
#endif
    if (file->fd<0) { rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_open; }
    fcntl(file->fd, FD_CLOEXEC, 1);
    file->self = posix_io_mux;
    return NEXUS_SUCCESS;

err_open:
    return rc;
#else
    return NEXUS_SUCCESS;
#endif
}

NEXUS_Error bfile_io_posix_unlink(const char *fname)
{
    return NEXUS_SUCCESS;
}

