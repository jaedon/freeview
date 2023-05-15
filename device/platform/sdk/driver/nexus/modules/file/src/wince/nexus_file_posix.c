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
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/23/12 12:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /brcm_wince/97400/Private/BMSIPTV/nexus/modules/file/src/wince/nexus_file_posix.c $
 * 
 * 2   2/23/12 12:36p rrlee
 * SW7405-5608: Add unlink, fix open compliance
 * 
 * SW7405-5270/2   2/15/12 4:28p rrlee
 * SW7405-5608: Fix file open behavior
 * 
 * SW7405-5270/1   6/1/11 5:35p rrlee
 * SW7405-5270:WinCE build compliance
 * 
 * 1   11/16/10 10:43a katrep
 * SW7405-3705:add wince specific files
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
#include <windows.h>


BDBG_MODULE(nexus_file_posix);
#define BDBG_MSG_TRACE(x) /*BDBG_MSG(x)*/


static ssize_t
posix_read(bfile_io_read_t fd, void *buf, size_t length)
{
    struct bfile_io_read_posix *file = (struct bfile_io_read_posix *) fd;
    DWORD bytesRead=0;

    BDBG_ASSERT(file);

#if 1 /* TBD verify WinCE read invalidates read buffer for all FS */
    if(file->direct) 
        NEXUS_FlushCache((void *)buf, length); 
#endif

    ReadFile(file->hFile, buf, length, &bytesRead, NULL);
    return bytesRead;
}
static off_t
posix_seek(bfile_io_read_t fd, off_t offset, int whence)
{
    struct bfile_io_read_posix *file = (struct bfile_io_read_posix *) fd;
    off_t off;

    off = SetFilePointer(file->hFile, offset, NULL, whence);
    BDBG_MSG_TRACE(("posix_seek: %#lx %lu(%d) -> %ld", (unsigned long)fd, (unsigned long)offset, whence, (long)off));
    return off;
}

static int
posix_bounds(bfile_io_read_t fd, off_t *first, off_t *last)
{
    struct bfile_io_read_posix *file = (struct bfile_io_read_posix *) fd;
    off_t size;
    
    BDBG_ASSERT(file);
    size = GetFileSize(file->hFile, NULL) ;

    *first = 0;
    if (size == (off_t) -1) {
        *last = 0;
        return -1;
    } else {
        *last = size;
        return 0;
    }
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
    wchar_t wname[MAX_PATH];
    DWORD flags = direct ? FILE_FLAG_NO_BUFFERING : 0;

    BDBG_ASSERT(file);
    BDBG_ASSERT(fname);

    file->direct = direct;
    file->hFile = INVALID_HANDLE_VALUE;
    file->self = posix_io_read;
    
    if( -1 == mbstowcs(wname, fname, MAX_PATH)) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    file->hFile = CreateFile(wname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, flags, NULL);


    if (file->hFile == INVALID_HANDLE_VALUE) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return NEXUS_SUCCESS;
}

void
bfile_io_read_posix_close(struct bfile_io_read_posix *file)
{
    BDBG_ASSERT(file);

    if (file->hFile !=INVALID_HANDLE_VALUE) {
        CloseHandle(file->hFile);
        file->hFile = INVALID_HANDLE_VALUE;
    }
    return;
}

static ssize_t
posix_write(bfile_io_write_t fd, const void *buf, size_t length)
{
    struct bfile_io_write_posix *file = (struct bfile_io_write_posix *) fd;
    DWORD bytesWritten=0;

#if 1 /* TBD verify WinCE read invalidates read buffer for all FS */
    if(file->direct) 
        NEXUS_FlushCache((void *)buf, length); 
#endif
    WriteFile(file->hFile, buf, length, &bytesWritten, NULL);

    return bytesWritten;

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
    off = SetFilePointer(file->hFile, offset, NULL, whence);
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
local_bfile_io_write_posix_open(struct bfile_io_write_posix *file, const char *fname, bool direct, bool append)
{
    wchar_t wname[MAX_PATH];
    DWORD disposition, flags;

    BDBG_ASSERT(file);
    BDBG_ASSERT(fname);

    file->direct = direct;
    file->hFile = INVALID_HANDLE_VALUE;
    file->self = posix_io_write;
    
    if( -1 == mbstowcs(wname, fname, MAX_PATH)) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* NB- OPEN_ALWAYS Opens the file, if it exists. If the file does not exist, 
      the function creates the file as if dwCreationDisposition were CREATE_NEW. */
    disposition = append ? OPEN_ALWAYS :  CREATE_ALWAYS;

    flags = direct ? FILE_FLAG_NO_BUFFERING : 0;

    file->hFile = CreateFile(wname, GENERIC_WRITE, 0, NULL, disposition, flags, NULL);

    if (file->hFile == INVALID_HANDLE_VALUE) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* move to EOF if append */
    if(append) {
        SetFilePointer(file->hFile, 0, NULL, FILE_END); 
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error
bfile_io_write_posix_open(struct bfile_io_write_posix *file, const char *fname, bool direct)
{
    return local_bfile_io_write_posix_open(file, fname, direct, false);
}

NEXUS_Error
bfile_io_write_posix_append(struct bfile_io_write_posix *file, const char *fname, bool direct)
{
    return local_bfile_io_write_posix_open(file, fname, direct, true);
}
void
bfile_io_write_posix_close(struct bfile_io_write_posix *file)
{
    BDBG_ASSERT(file);

    if (file->hFile !=INVALID_HANDLE_VALUE) {
        CloseHandle(file->hFile);
        file->hFile = INVALID_HANDLE_VALUE;
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

NEXUS_FileRecordHandle
NEXUS_FileRecord_OpenPosix(const char *fname, const char *indexname)
{
    NEXUS_Error rc;
    struct bfile_out_posix *file =  BKNI_Malloc(sizeof(*file));
    if (!file) { rc  = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }

    if (fname) {
        rc = bfile_io_write_posix_open(&file->data, fname, true);
        if (rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_data; }
        file->self.data = &file->data.self;
    } else {
        file->self.data = NULL;
    }

    if (indexname) {
        rc = bfile_io_write_posix_open(&file->index, indexname, false);
        if (rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_index; }
        file->self.index = &file->index.self;
    } else {
        file->self.index = NULL;
    }

    file->self.close = posix_close_out;

    return &file->self;

err_open_index:
    CloseHandle(file->data.hFile);
err_open_data:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

NEXUS_FileRecordHandle
NEXUS_FileRecord_AppendPosix(const char *fname, const char *indexname)
{
    NEXUS_Error rc;
    struct bfile_out_posix *file;

    file =  BKNI_Malloc(sizeof(*file));
    if (!file) { rc  = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }

    if (fname) {
        rc = bfile_io_write_posix_append(&file->data, fname, true);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_data; }
        file->self.data = &file->data.self;

    } else {
        file->self.data = NULL;
    }

    if (indexname) {
        rc = bfile_io_write_posix_append(&file->index, indexname, false);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_index; }
        file->self.index = &file->index.self;
    } else {
        file->self.index = NULL;
    }

    file->self.close = posix_close_out;

    return &file->self;

err_open_index:
    CloseHandle(file->data.hFile);
err_open_data:
    BKNI_Free(file);
err_alloc:
    return NULL;
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


NEXUS_FilePlayHandle
NEXUS_FilePlay_OpenPosix(const char *fname, const char *indexname)
{
    NEXUS_Error rc;
    struct bfile_in_posix *file =  BKNI_Malloc(sizeof(*file));

    if (!file) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }

    rc = bfile_io_read_posix_open(&file->data, fname, true);
    if (rc!=NEXUS_SUCCESS) {
        BDBG_ERR(("Unable to open input file: '%s'", fname));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_open_data;
    }

    if (indexname) {
        rc = bfile_io_read_posix_open(&file->index, indexname, false);
        if (rc!=NEXUS_SUCCESS) {
            BDBG_ERR(("Unable to open index file: '%s'", indexname));
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
    return NULL;
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
    if (file->hFile !=INVALID_HANDLE_VALUE) {
        CloseHandle(file->hFile);
        file->hFile = INVALID_HANDLE_VALUE;
    }
    BDBG_OBJECT_DESTROY(file, bfile_io_mux_posix);
    BKNI_Free(file);
    return;
}

static ssize_t
bfile_io_mux_posix_read(bfile_io_mux_t fd, off_t offset, void *buf, size_t length)
{
    DWORD bytesRead=0;
    struct bfile_io_mux_posix *file = (struct bfile_io_mux_posix *)fd;
    BDBG_OBJECT_ASSERT(file, bfile_io_mux_posix);

#if 1 /* TBD verify WinCE read invalidates read buffer for all FS */
    if(file->direct) 
        NEXUS_FlushCache((void *)buf, length); 
#endif
    if(INVALID_SET_FILE_POINTER!=SetFilePointer(file->hFile, offset, NULL, FILE_BEGIN)) {
        if(ReadFile(file->hFile, buf, length, &bytesRead, NULL)) {
            goto read_done;
        }
    }
    BDBG_ERR(("Seek or read error"));
read_done:
    return (ssize_t)bytesRead;
}

static ssize_t
bfile_io_mux_posix_write(bfile_io_mux_t fd, off_t offset, const void *buf, size_t length)
{
    DWORD bytesWritten=0;
    struct bfile_io_mux_posix *file = (struct bfile_io_mux_posix *)fd;
    BDBG_OBJECT_ASSERT(file, bfile_io_mux_posix);
#if 1 /* TBD verify WinCE read invalidates read buffer for all FS */
    if(file->direct) 
        NEXUS_FlushCache((void *)buf, length); 
#endif
    if(INVALID_SET_FILE_POINTER!=SetFilePointer(file->hFile, offset, NULL, FILE_BEGIN)) {
        if(WriteFile(file->hFile, buf, length, &bytesWritten, NULL)) {
            goto write_done;
        }
    }
    BDBG_ERR(("Seek or write error"));
write_done:
    return (ssize_t)bytesWritten;
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
    wchar_t wname[MAX_PATH];
    DWORD cfFlags = direct ? FILE_FLAG_NO_BUFFERING : 0;
    BDBG_ASSERT(file);
    BDBG_ASSERT(fname);
    BDBG_OBJECT_INIT(file, bfile_io_mux_posix);
    file->direct = direct;
    if( -1 == mbstowcs(wname, fname, MAX_PATH)) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    file->hFile = CreateFile(wname, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, cfFlags, NULL);

    if (file->hFile == INVALID_HANDLE_VALUE) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error bfile_io_posix_unlink(const char *fname)
{
    wchar_t wname[MAX_PATH];
    BDBG_ASSERT(fname);

    if( (-1 == mbstowcs(wname, fname, MAX_PATH)) || ! DeleteFile(wname)) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    return NEXUS_SUCCESS;
}

