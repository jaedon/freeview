/***************************************************************************
 *     (c)2005-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_file_fifo.c $
 * $brcm_Revision: 18 $
 * $brcm_Date: 8/28/12 3:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file/src/nexus_file_fifo.c $
 * 
 * 18   8/28/12 3:24p erickson
 * SW7346-994: don't rely on bcmplayer BNAV_Version detection because it's
 *  not possible for file fifos with AVC
 * 
 * 17   2/3/12 4:15p erickson
 * SW7425-2282: set BNAV_Player_Settings.changingBounds, improve DBG
 * 
 * 16   12/28/11 10:39a mphillip
 * SW7335-1375: Read data fifo header into the correct location in the
 *  structure on fifo files opened only for reading
 * 
 * 15   6/28/11 11:07a vsilyaev
 * SW7420-1139: Trim of the metadata offset
 * 
 * 14   5/26/11 7:01p vsilyaev
 * SW7335-1249: Added option to pereiodically write metadata into a disk
 *  file
 * 
 * 13   2/25/11 12:47p erickson
 * SW7125-818: use BNAV_GetEntrySize
 *
 * 12   5/24/10 10:49a jtna
 * SW7550-439: Coverity Defect ID:21994 ATOMICITY
 *
 * 11   4/2/10 2:37p erickson
 * SW7405-3833: nexus_file_pvr.h is now part of the public API
 *
 * 10   11/9/09 6:49p mphillip
 * SW7405-3361: Fix header zeroing when opening for playback without
 *  record
 *
 * 9   9/16/09 6:27p mphillip
 * SW7405-3035: Check in I/O cleanup changes
 *
 * 8   9/15/09 11:19a mphillip
 * SWDEPRECATED-3998: Set trim position to the first available offset when
 *  requested trim location is earlier.
 *
 * 7   6/4/09 5:01p erickson
 * PR54129: align file to 188 for non O_DIRECT. don't align trim offset if
 *  non O DIRECT. fix DBG.
 *
 * 6   3/16/09 10:51a vsilyaev
 * PR 52405:  Fixed race condition in fifo_read
 *
 * 5   3/5/09 5:46p vsilyaev
 * PR 52579: Updated circular FIFO/timeshifting code
 *
 * 4   1/27/09 10:53a erickson
 * PR50367: added memset
 *
 * 3   6/4/08 1:33p rjlewis
 * PR40352: off_t is 32-bits in VxWorks -- use 64.
 *
 * 2   5/12/08 1:27p vsilyaev
 * PR 41567: Use NULL pid (0x1FFF) for video, to make NAV Player happy
 *
 * 1   1/18/08 2:16p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   11/13/07 1:00p vsilyaev
 * PR 37015: Improved use of O_DIRECT in API
 *
 * $old_brcm_Log: /BSEAV/api/src/pvr/bsettop_fileio_fifo.c $
 *
 * 23   11/8/07 2:16p ahulse
 * PR36192: Set up a trim gap so that bcmplayer and other apps never read
 * from index to be trimmed
 *
 * 22   11/1/07 12:10p jackli
 * PR36646: suppress BDBG_MSG_IO
 *
 * 21   10/25/07 2:23p jtna
 * PR35936: Coverity Defect ID:3993 NULL_RETURNS
 *
 * 20   4/12/07 1:58p jackli
 * PR29779: change bpvrfifo_signature
 *
 * 19   4/12/07 1:43p jackli
 * PR29779: error when timeshift buffer > 4G
 *
 * 18   10/4/06 2:02p vsilyaev
 * PR 23826: Added function to initialize bplayback_file structure
 *
 * 17   8/3/06 5:37p rjlewis
 * PR23249: compiler warnings.
 *
 * 16   7/31/06 5:52p jrubio
 * PR22697: Do not open read Index descriptor with O_DIRECT flag.
 *
 * 15   4/7/06 4:50p vsilyaev
 * PR 20680: More fixes in removing bpvr_buf
 *
 * 14   4/7/06 3:50p vsilyaev
 * PR 20680: Added bfile library
 *
 * 13   4/4/06 1:06p mphillip
 * PR20605: Fix a missing free.
 *
 * 12   3/29/06 8:13p vle
 * PR 20512: correct assertion condition when call bpvrfifo_write_release.
 *
 * 10   3/15/06 4:47p mphillip
 * PR18183: Minor typo in previous checkin
 *
 * 9   3/15/06 4:45p mphillip
 * PR18183: Clear the new file structure prior to use.
 *
 * 8   3/7/06 10:06a erickson
 * PR19853: use BNAV_AVC_Entry for VC1 PES
 *
 * 7   12/23/05 4:10p vsilyaev
 * PR 18183: Renamed API functions
 *
 * 5   12/19/05 3:22p rjlewis
 * PR18183: vxworks doesn't have <sys/fcntl.h>
 *
 * 4   11/30/05 3:13p vsilyaev
 * PR 18183: Improved FIFO file support
 *
 * 3   11/21/05 3:42p vsilyaev
 * PR 18183: Renamed size function to bounds, so it would remain useful
 * with trimmed files
 *
 * 2   11/18/05 2:27p vsilyaev
 * PR 18183: Added periodic trimmer
 *
 * 1   11/17/05 4:22p vsilyaev
 * PR 18183: Added circular(FIFO) file
 *
 ***************************************************************************/
#include "nexus_file_module.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "blst_queue.h"
#include "bcmplayer.h"
#include "bcmindexer.h"
#include "bfile_io.h"
#include "nexus_file_pvr.h"
#include "nexus_file_types.h"
#include "nexus_file_posix.h"

BDBG_MODULE(nexus_file_fifo);

struct bpvrfifo_entry {
    off_t size; /* size of the entry */
    off_t voff; /* file 'virtual' offset */
    off_t doff; /* disk 'physical' offset */
};

struct bpvrfifo_signature {
    char signature[16];
};

static const struct bpvrfifo_signature bpvrfifo_signature =
{
    "bpvrFiFo 0.21"
};
static void bfile_fifo_write_snapshot_locked(struct NEXUS_FifoRecord *file);


#define BFIFO_N_ENTRYS  5
struct bpvrfifo_header {
    union {
        struct {
            struct bpvrfifo_signature signature;
            struct bpvrfifo_entry entrys[BFIFO_N_ENTRYS+1];
        } h;
        uint8_t buf[512]; /* for performance reasons header shall be multiplier of the FS block size */
    } data; /* this is persistent portion of index (e.g. part that is saved on the disk */
    /* we use dual lock scheme, lock is acquired every time when header table is accessed,  and busy flag is set wherever there is an active transaction, in this manner we allow simultaneous read and write transactions, and effectively serialize trim with both read and write */
    BKNI_MutexHandle lock;
    unsigned busy; /* busy counter, if busy > 0 then there is an active write or read, and trim has to fail */
    unsigned prev_trim;     /* previously trimmed value */
    unsigned trim_gap;      /* Gap between real start of file and next trim point */
};
#define B_ENTRY(f,i) (&(f)->header->data.h.entrys[(i)])
#define b_lock(x) BKNI_AcquireMutex(x->header->lock)
#define b_unlock(x) BKNI_ReleaseMutex(x->header->lock)

struct bfile_io_write_fifo {
    struct bfile_io_write self;
    struct bfile_io_write_posix data;
    struct bpvrfifo_header *header;
    struct bpvrfifo_header _header;
    off_t offset; /* current virtual offset, also known as a file size */
    off_t size; /* size of the file on a disk */
    off_t start;  /* offset, where writeable area begins */
    unsigned ref_cnt; /* number of references, including itself */
    struct NEXUS_FifoRecord *writeSnapshotFile;
};

struct bfile_io_read_fifo {
    struct bfile_io_read self;
    struct bfile_io_read_posix data;
    struct bpvrfifo_header *header;
    off_t cur_pos; /* current virtual offset */
    struct bfile_io_write_fifo *writer;
};

#define B_ENTRY_CLEAR(f,i) do{ B_ENTRY(f,i)->doff=-1;B_ENTRY(f,i)->voff=0;B_ENTRY(f,i)->size=0;}while(0)
#define B_ENTRY_IS_EMPTY(f,i) (B_ENTRY(f,i)->doff==-1)

#if 0
#define BDBG_MSG_IO(i, x) BDBG_INSTANCE_MSG(i, x)
#else
#define BDBG_MSG_IO(i, x)
#endif

#if 0
#define B_DUMP_ENTRY(file) do{unsigned i;for(i=0;i<BFIFO_N_ENTRYS;i++) {BDBG_MSG_IO(file, ("%#x:%u %lld %lld %u", file, i, B_ENTRY(file,i)->voff, B_ENTRY(file,i)->doff, (unsigned)B_ENTRY(file,i)->size));}}while(0)
#else
#define B_DUMP_ENTRY(file)
#endif


static ssize_t
b_read(struct bfile_io_read_posix *fd, void *buf, size_t count)
{
    return fd->self.read(&fd->self, buf, count);
}

static off_t
b_rseek(struct bfile_io_read_posix *fd, off_t offset, int whence)
{
    return fd->self.seek(&fd->self, offset, whence);
}

static ssize_t
b_write(struct bfile_io_write_posix *fd, const void *buf, size_t count)
{
    return fd->self.write(&fd->self, buf, count);
}

static off_t
b_wseek(struct bfile_io_write_posix *fd, off_t offset, int whence)
{
    return bfile_io_write_posix_seek(fd, offset, whence);
}

static ssize_t
bpvrfifo_write(bfile_io_write_t fd, const void *buf_, size_t length)
{
    struct bfile_io_write_fifo *file = (struct bfile_io_write_fifo*) fd;
    ssize_t result;
    const void *buf = buf_;
    unsigned i;

    BDBG_ASSERT(file->header);
    BDBG_MSG_IO(file, (">write:%u %lld:%u", file->header->busy, file->offset, (unsigned)length ));
    b_lock(file);
    file->header->busy++;
    for(result=0;length>0;) {
        size_t to_write = length;
        ssize_t rc;
        struct bpvrfifo_entry *entry;
        off_t boundary;
        off_t rc_off;



        for(entry=NULL,i=0;i<BFIFO_N_ENTRYS;i++) {
            if (B_ENTRY_IS_EMPTY(file,i)) {
                break;
            }
            if(B_ENTRY(file,i)->voff + B_ENTRY(file,i)->size == file->offset) {
                entry = B_ENTRY(file, i);
                break;
            }
        }
        if (!entry) {
            BDBG_WRN(("can't write on descriptor %#x:%lld", (unsigned)file,file->offset));
            result = -1;
            break;
        }
        if (entry[1].doff!=-1) {
            boundary = entry[1].doff;
        } else {
            boundary = file->size;
        }
        if (to_write > boundary - (entry->doff + entry->size)) {
           to_write = boundary - (entry->doff + entry->size);
        }
        if (to_write<=0) {
            break;
        }
        BDBG_MSG_IO(file, (">writing:%#x [%lld/%lld/%lld/%d] (%u/%u/%lld) %#x", entry->voff, entry->doff, boundary, entry-B_ENTRY(file,0), (unsigned)result, (unsigned)to_write, (unsigned)length, file->offset, (unsigned)buf));
        b_unlock(file);
        rc_off = b_wseek(&file->data, entry->doff + entry->size, SEEK_SET);
        b_lock(file);
        BDBG_MSG_IO(file, ("wseek:%#x %lld -> %lld", entry->doff + entry->size, rc_off));
        if (rc_off != entry->doff + entry->size) {
            if (result==0) {
                if (rc_off < 0) {
                    result = (ssize_t)rc_off; /* propagate error */
                } else {
                    result = -1;
                }
            }
            break;
        }
        b_unlock(file);
        rc = b_write(&file->data, buf, to_write);
        b_lock(file);
        BDBG_MSG_IO(file, ("<writing:%#x [%lld/%lld/%lld/%d] (%u/%u/%lld) %#x->%d", entry->voff, entry->doff, boundary, entry-B_ENTRY(file,0), (unsigned)result, (unsigned)to_write, (unsigned)length, file->offset, (unsigned)buf, (int)rc));
        if (rc<=0) {
            if  (result==0) {
                result = rc; /* propagate error */
            }
            break;
        }
        BDBG_ASSERT((size_t)rc <= to_write);
        result += rc;
        length -= (size_t) rc;
        buf = (const uint8_t *)buf + rc;
        entry->size = entry->size + rc;
        file->offset = file->offset + rc;
        BDBG_ASSERT(entry->doff+entry->size <= file->size);
        if (entry->doff + entry->size == file->size) {
            entry = B_ENTRY(file, 0);
            /* reached the end of file, now is a time to allocate new chunk */
            if (entry->doff == file->start) {
                BDBG_MSG(("write overflow"));
            } else {
                BDBG_MSG_IO(file, ("write wraparound"));
                if (entry->doff != -1)  {
                    BDBG_ASSERT(B_ENTRY(file,BFIFO_N_ENTRYS-1)->doff==-1);
                    /* new entry _always_ should go on top, so shift old entrys down, so we could keep entries sorted by the 'doff' field */
                    for(i=BFIFO_N_ENTRYS-1;i>0;i--) {
                        *B_ENTRY(file,i) = *B_ENTRY(file, i-1);
                    }
                }
                entry->voff = file->offset;
                entry->doff = file->start;
                entry->size = 0;
            }
        }
    }
    file->header->busy--;
    B_DUMP_ENTRY(file);
    BDBG_MSG_IO(file, ("<write:%u %lld:%u -> %d", file->header->busy, file->offset, (unsigned)length, (int)result));
    if(file->writeSnapshotFile) {
        bfile_fifo_write_snapshot_locked(file->writeSnapshotFile);
        file->writeSnapshotFile = NULL;
    }
    b_unlock(file);
    return result;
}

#ifdef DIRECT_IO_SUPPORT
/* we need to maintain data file aligned by 4096 (for DIRECT I/O) and 188 aligned to keep it valid transport packet */
/* it rounds in 192K */
#define B_DATA_ALIGN  ((188/4)*4096)
#else
#define B_DATA_ALIGN  (188)
#endif

static ssize_t
bpvrfifo_read(bfile_io_read_t fd, void *buf_, size_t length)
{
    struct bfile_io_read_fifo *file = (struct bfile_io_read_fifo*) fd;
    ssize_t result;
    void *buf = buf_;
    unsigned i;
    ssize_t rc;


    BDBG_ASSERT(file->header);
    BDBG_MSG_IO(file, (">read:%u %lld:%u", file->header->busy, file->cur_pos, (unsigned)length));
    b_lock(file);
    file->header->busy++;
    for(result=0;length>0;) {
        off_t rc_off, seek_off;
        size_t to_read = length;

        {
            const struct bpvrfifo_entry *entry; /* when lock is released writer could update move entries around, so protect access to the volatile storage */


            for(entry=NULL,i=0;i<BFIFO_N_ENTRYS;i++) {
                if (B_ENTRY(file,i)->doff==-1) {
                    break;
                }
                if( B_ENTRY(file,i)->voff <= file->cur_pos &&
                    B_ENTRY(file,i)->voff + B_ENTRY(file, i)->size > file->cur_pos) {
                    entry = B_ENTRY(file,i);
                    break;
                }
            }
            if (!entry) {
                if (result==0) {
                    if (B_ENTRY(file, 0)->voff > file->cur_pos) {
                        BDBG_WRN(("out of bounds offset %#x,%lld", (unsigned)file,file->cur_pos));
                        result = -1;
                    }
                }
                break;
            }
            if (to_read > entry->size - (file->cur_pos - entry->voff)) {
                to_read =  entry->size - (file->cur_pos - entry->voff);
            }
            seek_off = entry->doff + (file->cur_pos - entry->voff);
            BDBG_MSG_IO(file, (">reading: [%lld/%lld/%d -> %lld] (%u/%u/%u/%lld) %#x", entry->voff, entry->doff, entry-B_ENTRY(file,0), seek_off, (unsigned)result, (unsigned)to_read, (unsigned)length, file->cur_pos, (unsigned)buf));
        }
        b_unlock(file);
        rc_off = b_rseek(&file->data, seek_off, SEEK_SET);
        b_lock(file);
        BDBG_MSG_IO(file, ("rseek: %lld -> %lld", seek_off, rc_off));
        if (rc_off != seek_off) {
            if (result==0) {
                if (rc_off < 0) {
                    result = (ssize_t)rc_off; /* propagate error */
                } else {
                    /* coverity[use] */
                    BDBG_WRN(("bpvrfifo_read: %#lx can't seek to %lld(instead %lld)", (unsigned long)file, seek_off, rc_off));
                    result = -1;
                }
            }
            break;
        }
        b_unlock(file);
        rc = b_read(&file->data, buf, to_read);
        b_lock(file);
        BDBG_MSG_IO(file, ("<reading: %lld (%u/%u/%u/%lld) %#x->%d", seek_off, (unsigned)result, (unsigned)to_read, (unsigned)length, file->cur_pos, (unsigned)buf, (int)rc));
        if (rc!=(ssize_t)to_read) {
            if (result==0) {
                result = rc; /* propagate error */
            }
            break;
        }
        BDBG_ASSERT((size_t)rc <= to_read);
        result += rc;
        buf = (uint8_t *)buf + rc;
        length -= (size_t) rc;
        file->cur_pos = file->cur_pos + rc;
    }
    file->header->busy--;
    B_DUMP_ENTRY(file);
    BDBG_MSG_IO(file, ("<read:%u %lld:%u=%d", file->header->busy, file->cur_pos, (unsigned)length, (int)result));

    b_unlock(file);
    return result;
}


static int
bpvrfifo_bounds_locked(const struct bfile_io_read_fifo *file, off_t *first, off_t *last)
{
    unsigned i;

    BDBG_ASSERT(file->header);

    if (B_ENTRY_IS_EMPTY(file,0)) {
        *first = *last = 0;
    } else {
        *first = B_ENTRY(file,0)->voff;
        *last = B_ENTRY(file,0)->voff + B_ENTRY(file,0)->size;
    }

    for(i=1;i<BFIFO_N_ENTRYS;i++) {
        if (B_ENTRY_IS_EMPTY(file,i)) {
            break;
        }
        if(B_ENTRY(file,i)->voff >= *last) {
            *last = B_ENTRY(file,i)->voff + B_ENTRY(file,i)->size;
        }
        if(B_ENTRY(file,i)->voff < *first) {
            *first = B_ENTRY(file,i)->voff;
        }
    }

    if( file->header->trim_gap ) {
        *first += (off_t)(file->header->trim_gap);
    }


    return 0;
}

static int
bpvrfifo_bounds(bfile_io_read_t fd, off_t *first, off_t *last )
{
    struct bfile_io_read_fifo *file = (struct bfile_io_read_fifo *) fd;
    int rc;

    BDBG_ASSERT(file->header);
    b_lock(file); /* don't need to set busy because lock isn't released */

    BDBG_MSG_IO(file, (">bounds"));
    rc = bpvrfifo_bounds_locked(file, first, last);
    BDBG_MSG_IO(file, ("<bounds: %lld..%lld", *first, *last));
    b_unlock(file);
    return rc;
}

static off_t
bpvrfifo_trim(bfile_io_write_t fd, off_t trim_pos)
{
    struct bfile_io_write_fifo *file = (struct bfile_io_write_fifo *) fd;
    off_t result;
    off_t min_voff = 0;
    unsigned i,j;

    BDBG_ASSERT(file->header);
    b_lock(file);
    BDBG_MSG_IO(file, (">trim:%u %lld", file->header->busy, trim_pos));
    result = 0;
    if (file->header->busy) {
        goto done;
    }

    /* trim_pos = (trim_pos) & (~(4096-1)); */
    if (trim_pos>file->offset) {
        trim_pos = file->offset;
    } else if (trim_pos<0) {
        trim_pos = 0;
    }

    for(i=0;i<BFIFO_N_ENTRYS;i++) {
        if (B_ENTRY_IS_EMPTY(file,i)) {
            break;
        } else if( B_ENTRY(file,i)->voff >= trim_pos) {
            if (min_voff==0 || min_voff > B_ENTRY(file,i)->voff)
                min_voff = B_ENTRY(file,i)->voff;
            continue;
        } else if (B_ENTRY(file, i)->voff + B_ENTRY(file,i)->size > trim_pos ) {
            B_ENTRY(file, i)->size -= trim_pos - B_ENTRY(file,i)->voff;
            B_ENTRY(file, i)->doff += trim_pos - B_ENTRY(file,i)->voff;
            B_ENTRY(file, i)->voff = trim_pos;
            result = trim_pos;
        } else {
            if (result < B_ENTRY(file,i)->voff + B_ENTRY(file,i)->size) {
                result = B_ENTRY(file,i)->voff + B_ENTRY(file,i)->size;
            }
            B_ENTRY_CLEAR(file,i); /* mark entry as unused */
        }
    }
    if (!result && trim_pos <= min_voff)
        result = min_voff;
    /* collapse empty entries */
    for(i=0;i<BFIFO_N_ENTRYS;i++) {
        if (B_ENTRY_IS_EMPTY(file,i)) {
            for(j=i+1;j<BFIFO_N_ENTRYS;j++) {
                if (!B_ENTRY_IS_EMPTY(file,j)) {
                    *B_ENTRY(file, i) = *B_ENTRY(file, j);
                    B_ENTRY_CLEAR(file, j);
                    break;
                }
            }
            if (j==BFIFO_N_ENTRYS) {
                break;
            }
        }
    }
    if (B_ENTRY_IS_EMPTY(file,0)) {
        /* file was trimed to  bones, create new entry */
        B_ENTRY(file, 0)->doff = file->start;
        B_ENTRY(file, 0)->voff = result;
        B_ENTRY(file, 0)->size = 0;
    }
done:
    B_DUMP_ENTRY(file);
    BDBG_MSG_IO(file, ("<trim:%u %lld", file->header->busy, result));
    b_unlock(file);
    return result;
}

static off_t
bpvrfifo_seek(bfile_io_read_t fd, off_t offset, int whence)
{
    struct bfile_io_read_fifo *file = (struct bfile_io_read_fifo *) fd;
    off_t size,begin;
    int rc;

    BDBG_MSG_IO(file,(">seek: %lld %d", offset, whence));
    b_lock(file);

    if (whence==SEEK_CUR && offset==0) {
        /* special case, which is used to  rerrieve current position, without bound checking  */
        offset = file->cur_pos;
        goto done;
    }
    rc = bpvrfifo_bounds_locked(file, &begin, &size);
    if (rc<0) {
        offset = -1;
        goto error;
    }
    /* locate where file begins */
    switch(whence) {
    case SEEK_CUR:
        offset = file->cur_pos + offset;
        break;
    case SEEK_END:
        offset = size + offset;
        break;
    case SEEK_SET:
        break;
    default:
        BDBG_ERR(("unknown seek whence %d", whence));
        goto error;
    }
    if (offset == file->cur_pos) {
        goto done;
    }
    if (offset < 0 || offset >= size) {
        BDBG_WRN(("<seek out of bounds 0..%lld..%lld", offset, size));
        offset = -1;
        goto error;
    }
    if (offset < begin) {
        offset = begin;
    }

    /* just assign new offset */
done:
    file->cur_pos = offset;
error:
    BDBG_MSG_IO(file, ("<seek: %lld", offset));
    b_unlock(file);
    return offset;
}

static const struct bfile_io_write bpvrfifo_io_write = {
    bpvrfifo_write,
    bpvrfifo_trim,
    BIO_DEFAULT_PRIORITY
};

static const struct bfile_io_read bpvrfifo_io_read = {
    bpvrfifo_read,
    bpvrfifo_seek,
    bpvrfifo_bounds,
    BIO_DEFAULT_PRIORITY
};

static struct bfile_io_write_fifo *
bpvrfifo_write_open(const char *fname, bool direct, off_t start )
{
    struct bfile_io_write_fifo *file =  BKNI_Malloc(sizeof(*file));
    BERR_Code rc;
    unsigned i;

    if (!file) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    BKNI_Memset(file, 0, sizeof(*file));
    file->header = &file->_header;
    file->header->data.h.signature = bpvrfifo_signature;
    file->writeSnapshotFile = NULL;
    rc = BKNI_CreateMutex(&file->header->lock);
    if (rc!=BERR_SUCCESS) {
        goto err_lock;
    }

    rc = bfile_io_write_posix_open(&file->data, fname, direct);
    if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_data; }
    /* initialize data */
    file->self = bpvrfifo_io_write;
    file->start = start;
    file->ref_cnt = 1;
    file->size = 512 * 1024;
    for(i=0;i<BFIFO_N_ENTRYS;i++) {
        B_ENTRY_CLEAR(file, i);
    }
    B_ENTRY(file, 0)->doff = file->start;
    return file;

err_open_data:
    BKNI_DestroyMutex(file->header->lock);
err_lock:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

static bfile_io_write_t
bpvrfifo_write_file(struct bfile_io_write_fifo *file)
{
    return &file->self;
}

/* decrement reference counter and once it reaches 0 then release memory */
static void
bpvrfifo_write_release(struct bfile_io_write_fifo *file)
{
    bool last;

    b_lock(file);
    BDBG_ASSERT(file->header);
    BDBG_MSG_IO(file, ("release:%u:%u", file->ref_cnt, file->header->busy));
    BDBG_ASSERT(file->ref_cnt);
    file->ref_cnt--;
    last = file->ref_cnt==0;
    BDBG_ASSERT(!last || file->header->busy==0);
    b_unlock(file);
    if (last) {
        BKNI_DestroyMutex(file->header->lock);
        BKNI_Free(file);
    }
    return;
}

static void
bpvrfifo_write_close(struct bfile_io_write_fifo *file)
{
    bfile_io_write_posix_close(&file->data);
    bpvrfifo_write_release(file);
    return;
}

static struct bfile_io_read_fifo *
bpvrfifo_read_open(const char *fname, bool direct, struct bfile_io_write_fifo *writer)
{
    struct bfile_io_read_fifo *file =  BKNI_Malloc(sizeof(*file));
    NEXUS_Error rc;

    if (!file) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    rc = bfile_io_read_posix_open(&file->data, fname, direct);
    if (rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_data; }
    file->writer = writer;
    file->self = bpvrfifo_io_read;
    if (writer) {
        file->header = writer->header;
        b_lock(file);
        file->writer->ref_cnt++;
        b_unlock(file);
    } else {
        BERR_Code rc;

        file->header = BKNI_Malloc(sizeof(*file->header));
        BKNI_Memset(file->header, 0, sizeof(*file->header));
        file->header->data.h.signature = bpvrfifo_signature;
        rc = BKNI_CreateMutex(&file->header->lock);
        if (rc!=BERR_SUCCESS) {
            goto err_lock;
        }
    }
    return  file;

err_lock:
    bfile_io_read_posix_close(&file->data);
err_open_data:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

static bfile_io_read_t
bpvrfifo_read_file(struct bfile_io_read_fifo *file)
{
    return &file->self;
}


static void
bpvrfifo_read_close(struct bfile_io_read_fifo *file)
{
    bfile_io_read_posix_close(&file->data);
    if(file->writer) {
        bpvrfifo_write_release(file->writer);
    } else {
        BKNI_DestroyMutex(file->header->lock);
        BKNI_Free(file->header);
    }
    BKNI_Free(file);
    return;
}

#define B_TRIM_TIMER    5000
#define B_TRIM_TRY_TIMER    30

struct bfile_out_desc {
    struct bfile_io_write_fifo *file;
    unsigned trim_count;
    off_t trim_off;
};

struct NEXUS_FifoRecord {
    struct NEXUS_FileRecord self;
    struct bfile_out_desc data;
    struct bfile_out_desc index;
    struct bfile_io_read_fifo *rd_index;
    unsigned index_entrysize;
    BNAV_Player_Handle bcm_player;
    NEXUS_TimerHandle timer; /* currently active timer  or NULL */
    NEXUS_Time lastSnapshotTime;
    NEXUS_FifoRecordSettings cfg;
};

struct bfile_in_fifo {
    struct NEXUS_FilePlay self;
    struct bfile_io_read_fifo *data;
    struct bfile_io_read_fifo *index;
    struct NEXUS_FifoRecord *writer;
};

#define GET_INDEX_FILEIO(fp) (&((NEXUS_FifoRecordHandle) fp)->rd_index->self)

static long
bp_read(void *buffer, long size, long count, void *fp )
{
    bfile_io_read_t f = GET_INDEX_FILEIO(fp);

    return (long)f->read(f, buffer, (size_t)(count*size));
}

static long
bp_tell( void *fp )
{
    bfile_io_read_t f = GET_INDEX_FILEIO(fp);
    return (long)f->seek(f, 0, SEEK_CUR);
}

#if 0
static int
bp_seek( void *fp, long offset, int origin )
{
    bfile_io_read_t f = GET_INDEX_FILEIO(fp);
    off_t rc;
    rc = f->seek(f, (off_t)offset, origin);
    if ( rc == (off_t)-1) {
        return -1;
    }
    return 0;
}
#endif

/* return entrysize once first NAV entry is written. if no entry, return default value. */
static unsigned b_get_index_entrysize(NEXUS_FifoRecordHandle file)
{
    if (!file->index_entrysize) {
        int rc;
        BNAV_Entry navEntry;
        rc = NEXUS_P_ReadFirstNavEntry(&file->rd_index->self, &navEntry);
        if (!rc) {
            file->index_entrysize = BNAV_GetEntrySize(BNAV_get_version(&navEntry));
        }
    }
    return file->index_entrysize?file->index_entrysize:sizeof(BNAV_VersionOriginal);
}

static int
bp_bounds(BNAV_Player_Handle handle, void *fp, long *firstIndex, long *lastIndex)
{
    bfile_io_read_t f = GET_INDEX_FILEIO(fp);
    off_t first, last;
    unsigned index_entrysize = b_get_index_entrysize((NEXUS_FifoRecordHandle)fp);

    f->bounds(f, &first, &last);

    BSTD_UNUSED(handle);
    *firstIndex = first/index_entrysize;
    *lastIndex = (last-1)/index_entrysize;

    return 0;
}

static int
trim_bp_bounds( BNAV_Player_Handle handle, void *fp, long *firstIndex, long *lastIndex )
{
    bfile_io_read_t fd = GET_INDEX_FILEIO(fp);
    off_t first, last;
    unsigned index_entrysize = b_get_index_entrysize((NEXUS_FifoRecordHandle)fp);
    int rc;

    struct bfile_io_read_fifo *file = (struct bfile_io_read_fifo *)fd;
    BDBG_ASSERT(file->header);
    b_lock(file);
    rc = bpvrfifo_bounds_locked(file, &first, &last );
    if( file->header->trim_gap ) {
        /* Untrimmed file start is first offset minus trim gap */
        first -= (off_t)(file->header->trim_gap);
    }
    b_unlock(file);


    BSTD_UNUSED(handle);
    *firstIndex = first/index_entrysize;
    *lastIndex = (last-1)/index_entrysize;

    return 0;
}

static int
trim_bp_seek( void *fp, long offset, int origin )
{
    bfile_io_read_t f = GET_INDEX_FILEIO(fp);
    unsigned index_entrysize = b_get_index_entrysize((NEXUS_FifoRecordHandle)fp);
    long saveOffset = 0;
    struct bfile_io_read_fifo *file = (struct bfile_io_read_fifo *)f;
    off_t rc;

    if( file->header->trim_gap ) {
        BNAV_Player_Handle dummyHandle = NULL;
        long firstIdx, lastIdx;

        /* Find beginning trimmed area */
        trim_bp_bounds( dummyHandle, fp, &firstIdx, &lastIdx );
        saveOffset = firstIdx*index_entrysize;

        if( offset && offset < firstIdx*(int)index_entrysize ) {
            /* But don't adjust for a tell request ( offset=0 , origin=SEEK_SET ) */
            offset = firstIdx*index_entrysize;
        }
    }

    rc = f->seek(f, (off_t)offset, origin);
    if ( rc == (off_t)-1) {
        return -1;
    }


    if ( offset==0 && saveOffset ) {
        /* Fix up cur_pos to actual trim area beginning */
        b_lock(file);
        file->cur_pos = saveOffset;
        b_unlock(file);
    }
    else if ( offset < file->cur_pos ) {
        /* If get in here , it means trimming is happening */
        b_lock(file);
        file->cur_pos = offset;
        b_unlock(file);
    }

    return 0;
}


static void
bfile_fifo_close_in(NEXUS_FilePlayHandle f)
{
    struct bfile_in_fifo *file =  (struct bfile_in_fifo *)f;

    BDBG_ASSERT(file->data);
    BDBG_ASSERT(file->index);
    bpvrfifo_read_close(file->data);
    bpvrfifo_read_close(file->index);
    BKNI_Free(file);
    return;
}

NEXUS_FilePlayHandle
NEXUS_FifoPlay_Open(const char *fname, const char *indexname, NEXUS_FifoRecordHandle writer)
{
    struct bfile_in_fifo *file;
    NEXUS_Error rc;

    if (fname==NULL) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }
    if (indexname==NULL) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }

    file = BKNI_Malloc(sizeof(*file));
    if (!file) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }
    BKNI_Memset(file, 0, sizeof(*file));

    file->data = bpvrfifo_read_open(fname, true, writer?writer->data.file:NULL);
    if (!file->data) { goto err_data;}

    file->index = bpvrfifo_read_open(indexname, false, writer?writer->index.file:NULL);
    if (!file->index) { goto err_index;}

    if (writer==NULL) {
        struct bpvrfifo_header *buf;
        ssize_t rc;

        buf = file->index->header;
        b_rseek(&file->index->data, 0, SEEK_SET);
        rc = b_read(&file->index->data, &buf->data, sizeof(buf->data));
        if (rc!=sizeof(buf->data) || buf->data.h.signature.signature[0]!=bpvrfifo_signature.signature[0]) {
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto err_meta;
        }

        buf = file->data->header;
        b_rseek(&file->index->data, sizeof(buf->data), SEEK_SET);
        rc = b_read(&file->index->data, &buf->data, sizeof(buf->data));
        if (rc!=sizeof(buf->data) || buf->data.h.signature.signature[0]!=bpvrfifo_signature.signature[0]) {
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto err_meta;
        }
    }

    file->self.file.index = bpvrfifo_read_file(file->index);
    file->self.file.data = bpvrfifo_read_file(file->data);

    file->self.file.close = bfile_fifo_close_in;

    return &file->self;

err_meta:
    bpvrfifo_read_close(file->index);
err_index:
    bpvrfifo_read_close(file->data);
err_data:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_USB_FLASH_RECORD)
NEXUS_FilePlayHandle
NEXUS_FifoPlay_OpenForUsbFlash(const char *fname, const char *indexname, NEXUS_FifoRecordHandle writer)
{

    struct bfile_in_fifo *file;
    NEXUS_Error rc;

    if (fname==NULL) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }
    if (indexname==NULL) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }

    file = BKNI_Malloc(sizeof(*file));
    if (!file) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }
    BKNI_Memset(file, 0, sizeof(*file));

    file->data = bpvrfifo_read_open(fname, false, writer?writer->data.file:NULL);
    if (!file->data) { goto err_data;}

    file->index = bpvrfifo_read_open(indexname, false, writer?writer->index.file:NULL);
    if (!file->index) { goto err_index;}

    if (writer==NULL) {
        struct bpvrfifo_header *buf;
        ssize_t rc;

        buf = file->index->header;
        b_rseek(&file->index->data, 0, SEEK_SET);
        rc = b_read(&file->index->data, &buf->data, sizeof(buf->data));
        if (rc!=sizeof(buf->data) || buf->data.h.signature.signature[0]!=bpvrfifo_signature.signature[0]) {
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto err_meta;
        }

        buf = file->data->header;
        b_rseek(&file->index->data, sizeof(buf->data), SEEK_SET);
        rc = b_read(&file->index->data, &buf->data, sizeof(buf->data));
        if (rc!=sizeof(buf->data) || buf->data.h.signature.signature[0]!=bpvrfifo_signature.signature[0]) {
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto err_meta;
        }
    }

    file->self.file.index = bpvrfifo_read_file(file->index);
    file->self.file.data = bpvrfifo_read_file(file->data);

    file->self.file.close = bfile_fifo_close_in;

    return &file->self;

err_meta:
    bpvrfifo_read_close(file->index);
err_index:
    bpvrfifo_read_close(file->data);
err_data:
    BKNI_Free(file);
err_alloc:
    return NULL;
}
#else
NEXUS_FilePlayHandle
NEXUS_FifoPlay_OpenForUsbFlash(const char *fname, const char *indexname, NEXUS_FifoRecordHandle writer)
{
    BSTD_UNUSED(fname);
    BSTD_UNUSED(indexname);
    BSTD_UNUSED(writer);
    return NULL;
}
#endif /* HUMAX_PLATFORM_BASE */

static void
bfile_fifo_write_snapshot_locked(struct NEXUS_FifoRecord *file)
{
    const struct bpvrfifo_header *buf;

    buf = file->index.file->header;
    b_wseek(&file->index.file->data, 0, SEEK_SET);
    b_write(&file->index.file->data, &buf->data, sizeof(buf->data));

    buf = file->data.file->header;
    b_wseek(&file->index.file->data, sizeof(buf->data), SEEK_SET);
    b_write(&file->index.file->data, &buf->data, sizeof(buf->data));
    return;
}

static void
bfile_fifo_close_out(NEXUS_FileRecordHandle f)
{
    struct NEXUS_FifoRecord *file =  (struct NEXUS_FifoRecord *)f;

    if (file->timer) {
        NEXUS_CancelTimer(file->timer);
    }
    if (file->bcm_player) {
        BNAV_Player_Close(file->bcm_player);
    }
    b_lock(file->index.file);
    bfile_fifo_write_snapshot_locked(file);
    bfile_fifo_write_snapshot_locked(file);
    b_unlock(file->index.file);


    bpvrfifo_read_close(file->rd_index);
    bpvrfifo_write_close(file->data.file);
    bpvrfifo_write_close(file->index.file);
    BKNI_Free(file);
    return;
}

static void
b_trim_try_player(NEXUS_FifoRecordHandle file)
{
    if (file->bcm_player==NULL) {
        BNAV_DecoderFeatures features = {1,1,1,1};
        BNAV_Player_Settings cfg;
        BNAV_Entry navEntry;
        int rc;

        BNAV_Player_GetDefaultSettings(&cfg);
        
        rc = NEXUS_P_ReadFirstNavEntry(&file->rd_index->self, &navEntry);
        if (!rc) {
            cfg.navVersion = BNAV_get_version(&navEntry);
        }

        cfg.videoPid = 0x1FFF; /* since BNAV_Player doesn't like 0 */
        cfg.filePointer = file;
        cfg.normalPlayBufferSize = 1024*128;
        cfg.decoderFeatures = features;
        cfg.readCb = bp_read;
        cfg.tellCb = bp_tell;
        cfg.seekCb   = trim_bp_seek;
        cfg.boundsCb = trim_bp_bounds;
        cfg.changingBounds = true; /* force a bounds callback */
        cfg.transportTimestampEnabled = false;
        if (BNAV_Player_Open(&file->bcm_player, &cfg)!=0) {
            return;
        }
    }
    return;
}

/* this functions return true if trim was successfull and false otherwise */
static bool
b_try_trim_file(struct bfile_out_desc *desc)
{
    off_t rc;

    if (desc->trim_count==0) {
        return true;
    }
    if (desc->trim_off>0) {
        BDBG_MSG((">try_trim %#x:%u %lld", (unsigned)desc->file, desc->trim_count, desc->trim_off));
        rc = desc->file->self.trim(&desc->file->self, desc->trim_off);
        BDBG_MSG(("<try_trim %#x:%u %lld:%lld", (unsigned)desc->file, desc->trim_count, desc->trim_off, rc));
        if (rc == 0) {
            desc->trim_count++;
            return false;
        }
        if (rc<0) {
            BDBG_WRN(("trim has failed"));
            return true;
        }
    }
    desc->trim_count = 0;
    return true;
}

#define B_GET_OFF_T(p) ((p)->offsetLo|(((uint64_t)(p)->offsetHi)<<32))

/* this function called from the timer context */
static void
b_trim_timer(void *file_)
{
    NEXUS_FifoRecordHandle file = file_;
    unsigned timeout = B_TRIM_TIMER;
    long firstIndex, lastIndex;
    BNAV_Player_Position first, last;
    int rc;
    unsigned long interval;
    NEXUS_Time start, stop;

    NEXUS_Time_Get(&start);
    if(file->cfg.snapshotInterval != 0 && (NEXUS_Time_Diff(&start, &file->lastSnapshotTime)/1000)>=(long)file->cfg.snapshotInterval) {
        b_lock(file->index.file);
        file->index.file->writeSnapshotFile = file; /* we can't write data right there since it could overlap with running write operation and would corrupt the file write pointer */
        b_unlock(file->index.file);
        file->lastSnapshotTime = start;
    }

    if (file->index.trim_count || file->data.trim_count) {
        /* trim index first */
        if (!b_try_trim_file(&file->index)) {
            timeout = B_TRIM_TRY_TIMER;
            goto done;
        }
        if (!b_try_trim_file(&file->data)) {
            timeout = B_TRIM_TRY_TIMER;
            goto done;
        }
        /* this effectively limits rate of succesfull trims to one each B_TRIM_TIMER */
        goto done;
    }

    b_trim_try_player(file);
    if(file->bcm_player==NULL) {
        BDBG_WRN(("index file is empty, trim can't be activated"));
        goto done;
    }
    rc = trim_bp_bounds(file->bcm_player, file, &firstIndex, &lastIndex);
    if (rc!=0) {
        BDBG_WRN(("can't obtain file bounds"));
        goto done;
    }
    rc = BNAV_Player_GetPositionInformation(file->bcm_player, firstIndex, &first);
    if (rc!=0) {
        BDBG_WRN(("can't obtain first position %d", firstIndex));
        rc = BNAV_Player_GetPositionInformation(file->bcm_player, firstIndex+1, &first);
        if (rc!=0) {
            BDBG_WRN(("..2nd Attempt also failed !"));
            goto done;
        }
    }
    rc = BNAV_Player_GetPositionInformation(file->bcm_player, lastIndex, &last);
    if (rc!=0) {
        BDBG_WRN(("can't obtain last position %d", lastIndex));
        goto done;
    }

    BDBG_INSTANCE_MSG(file, ("position %ld:%lu:%lld %ld:%lu:%lld ",
        firstIndex, first.timestamp,B_GET_OFF_T(&first), lastIndex, last.timestamp, B_GET_OFF_T(&last)));

    interval = file->cfg.interval*1000;

    if (last.timestamp - first.timestamp > interval) {
        long index;
        off_t off;
        unsigned index_entrysize = b_get_index_entrysize(file);

        index = BNAV_Player_FindIndexFromTimestamp(file->bcm_player, last.timestamp - interval);
        if (index<0) {
            BDBG_WRN(("can't obtain new index"));
            goto done;
        }
        last = first; /* needed for debuging to save previous first */
        rc = BNAV_Player_GetPositionInformation(file->bcm_player, index, &first);
        if (rc!=0) {
            BDBG_WRN(("can't obtain new position"));
            goto done;
        }
        BDBG_MSG(("trimming from %d:%u:%lld to %d:%u:%lld", last.index, last.timestamp, B_GET_OFF_T(&last), index, first.timestamp, B_GET_OFF_T(&first)));
        off = index*index_entrysize;
        BDBG_MSG(("trimming index old %lld new %lld", (int64_t)file->index.trim_off, (int64_t)off));
        file->index.trim_off = off;
        file->index.trim_count = 1;

        file->index.file->header->trim_gap = (index - file->index.file->header->prev_trim)*index_entrysize;

        file->index.file->header->prev_trim = last.index;

        off = B_GET_OFF_T(&first) - first.metadataOffset;
#ifdef DIRECT_IO_SUPPORT
        off -= off % B_DATA_ALIGN;
#endif
        BDBG_MSG(("trimming data old %lld new %lld", (int64_t)file->data.trim_off, (int64_t)off));
        file->data.trim_off = off;
        file->data.trim_count = 1;

        timeout = B_TRIM_TRY_TIMER; /* do a trim on the next timer callout */
    }
done:
    file->timer = NEXUS_ScheduleTimer(timeout, b_trim_timer, file);  /* schedulle another heartbeat for the trim */
    NEXUS_Time_Get(&stop);
    BDBG_MSG(("trim_timer(%#x) %ldms", (unsigned)file , NEXUS_Time_Diff(&stop, &start)));

    if (!file->timer) {
        BDBG_ERR(("trim heartbeat has died"));
        return;
    }

    return;
}


NEXUS_FifoRecordHandle
NEXUS_FifoRecord_Create(const char *fname, const char *indexname)
{
    NEXUS_FifoRecordHandle file;
    NEXUS_Error rc;

    if (fname==NULL) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }
    if (indexname==NULL) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }

    file = BKNI_Malloc(sizeof(*file));
    if (!file) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_alloc; }

    BKNI_Memset(file, 0, sizeof *file);

    file->cfg.interval = 30;
    file->cfg.snapshotInterval = 0;
    file->cfg.data.soft = 512*1024*1024;
    file->cfg.data.hard = 1024*1024*1024;
    file->cfg.data.soft -= file->cfg.data.soft % B_DATA_ALIGN;
    file->cfg.data.hard -= file->cfg.data.hard % B_DATA_ALIGN;
    file->cfg.index.soft = 4*1024*1024;
    file->cfg.index.hard = 8*1024*1024;

    file->data.trim_count = 0;

    NEXUS_Time_Get(&file->lastSnapshotTime);

    file->data.file = bpvrfifo_write_open(fname, true, 0);
    if (!file->data.file) { goto err_data;}
    file->data.file->size = file->cfg.data.soft;

    file->index.trim_count = 0;
    file->index.file = bpvrfifo_write_open(indexname, false, 2*sizeof(file->data.file->header->data));
    if (!file->index.file) { goto err_index;}
    file->index.file->size = file->cfg.index.soft;

    file->rd_index = bpvrfifo_read_open(indexname, false, file->index.file);
    if (!file->rd_index) { goto err_rd_index;}


    file->self.index = bpvrfifo_write_file(file->index.file);
    file->self.data = bpvrfifo_write_file(file->data.file);

    file->self.close = bfile_fifo_close_out;

    file->index_entrysize = 0; /* we don't know until first NAV entry is written. use b_get_index_entrysize. */
    file->bcm_player = NULL;

    file->index.trim_count = file->data.trim_count = 0;
    file->index.trim_off = file->data.trim_off = 0;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_USB_FLASH_RECORD)
    file->self.bRecordWriteSizeLimit = true;
#endif

    /* timer shall be the last call in the initialization sequence */
    file->timer = NEXUS_ScheduleTimer(B_TRIM_TIMER, b_trim_timer, file);  /* schedulle heartbeat for the trim */
    if (!file->timer) { goto err_timer; }

    return file;

err_timer:
    bpvrfifo_read_close(file->rd_index);
err_rd_index:
    bpvrfifo_write_close(file->index.file);
err_index:
    bpvrfifo_write_close(file->data.file);
err_data:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_USB_FLASH_RECORD)
NEXUS_FifoRecordHandle
NEXUS_FifoRecord_CreateForUsbFlash(const char *fname, const char *indexname)
{
    NEXUS_FifoRecordHandle file;
    NEXUS_Error rc;

    if (fname==NULL) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }
    if (indexname==NULL) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }

    file = BKNI_Malloc(sizeof(*file));
    if (!file) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_alloc; }

    BKNI_Memset(file, 0, sizeof *file);

    file->cfg.interval = 30;
    file->cfg.snapshotInterval = 0;
    file->cfg.data.soft = 512*1024*1024;
    file->cfg.data.hard = 1024*1024*1024;
    file->cfg.data.soft -= file->cfg.data.soft % B_DATA_ALIGN;
    file->cfg.data.hard -= file->cfg.data.hard % B_DATA_ALIGN;
    file->cfg.index.soft = 4*1024*1024;
    file->cfg.index.hard = 8*1024*1024;

    file->data.trim_count = 0;

    NEXUS_Time_Get(&file->lastSnapshotTime);

    file->data.file = bpvrfifo_write_open(fname, true, 0);
    if (!file->data.file) { goto err_data;}
    file->data.file->size = file->cfg.data.soft;

    file->index.trim_count = 0;
    file->index.file = bpvrfifo_write_open(indexname, false, 2*sizeof(file->data.file->header->data));
    if (!file->index.file) { goto err_index;}
    file->index.file->size = file->cfg.index.soft;

    file->rd_index = bpvrfifo_read_open(indexname, false, file->index.file);
    if (!file->rd_index) { goto err_rd_index;}


    file->self.index = bpvrfifo_write_file(file->index.file);
    file->self.data = bpvrfifo_write_file(file->data.file);

    file->self.close = bfile_fifo_close_out;

    file->index_entrysize = 0; /* we don't know until first NAV entry is written. use b_get_index_entrysize. */
    file->bcm_player = NULL;

    file->index.trim_count = file->data.trim_count = 0;
    file->index.trim_off = file->data.trim_off = 0;

    file->self.bRecordWriteSizeLimit = false;

    /* timer shall be the last call in the initialization sequence */
    file->timer = NEXUS_ScheduleTimer(B_TRIM_TIMER, b_trim_timer, file);  /* schedulle heartbeat for the trim */
    if (!file->timer) { goto err_timer; }

    return file;

err_timer:
    bpvrfifo_read_close(file->rd_index);
err_rd_index:
    bpvrfifo_write_close(file->index.file);
err_index:
    bpvrfifo_write_close(file->data.file);
err_data:
    BKNI_Free(file);
err_alloc:
    return NULL;

}
#else
NEXUS_FifoRecordHandle
NEXUS_FifoRecord_CreateForUsbFlash(const char *fname, const char *indexname)
{
    BSTD_UNUSED(fname);
    BSTD_UNUSED(indexname);
    return NULL;
}
#endif /* HUMAX_PLATFORM_BASE */

NEXUS_FileRecordHandle
NEXUS_FifoRecord_GetFile(NEXUS_FifoRecordHandle file)
{
    return &file->self;
}

NEXUS_Error
NEXUS_FifoRecord_GetPosition(NEXUS_FifoRecordHandle file, NEXUS_FilePosition *first, NEXUS_FilePosition *last)
{
    long firstIndex, lastIndex;
    BNAV_Player_Position pos;
    int rc;

    b_trim_try_player(file);
    if(file->bcm_player==NULL) {
        BDBG_MSG(("index file is empty, position can't be extracted"));
        /* fill them with zeros */
        if(first) {
            first->mpegFileOffset = 0;
            first->indexOffset = NEXUS_FILE_INVALID_POSITION;
            first->timestamp = NEXUS_FILE_INVALID_POSITION;
        }
        if(last) { /* fill them with zeros */
            last->mpegFileOffset = 0;
            last->indexOffset = NEXUS_FILE_INVALID_POSITION;
            last->timestamp = NEXUS_FILE_INVALID_POSITION;
        }
        return NEXUS_SUCCESS;
    }
    rc = bp_bounds(file->bcm_player, file, &firstIndex, &lastIndex);
    if (rc!=0) { return BERR_TRACE(NEXUS_UNKNOWN); }
    if (first) {
        rc = BNAV_Player_GetPositionInformation(file->bcm_player, firstIndex, &pos);
        if (rc!=0) { return BERR_TRACE(NEXUS_UNKNOWN); }
        first->indexOffset = pos.index;
        first->timestamp = pos.timestamp;
        first->mpegFileOffset = B_GET_OFF_T(&pos);
    }
    if (last) {
        rc = BNAV_Player_GetPositionInformation(file->bcm_player, lastIndex, &pos);
        if (rc!=0) { return BERR_TRACE(NEXUS_UNKNOWN); }
        last->indexOffset = pos.index;
        last->timestamp = pos.timestamp;
        last->mpegFileOffset = B_GET_OFF_T(&pos);
    }
    return NEXUS_SUCCESS;
}

void
NEXUS_FifoRecord_GetSettings(NEXUS_FifoRecordHandle file, NEXUS_FifoRecordSettings *settings)
{
    /* settings are read only, so wa don't need synchronization here */
    *settings = file->cfg;
    return;
}


NEXUS_Error
NEXUS_FifoRecord_SetSettings(NEXUS_FifoRecordHandle file, const NEXUS_FifoRecordSettings *settings)
{
    if (
        settings->index.soft <= 2*sizeof(*file->data.file->header) ||
        settings->index.soft >= 2*1024*1024*(off_t)1024 ||
        settings->index.hard <= 2*sizeof(*file->data.file->header) ||
        settings->index.hard >= 2*1024*1024*(off_t)1024 ||
        settings->index.hard < settings->index.soft ||
        settings->data.soft <= B_DATA_ALIGN ||
        settings->data.hard <= B_DATA_ALIGN ||
        settings->data.hard < settings->index.soft )
    {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    /* we don't need synchronization here either */
    file->cfg = *settings;
    file->cfg.data.soft -= file->cfg.data.soft % B_DATA_ALIGN;
    file->cfg.data.hard -= file->cfg.data.hard % B_DATA_ALIGN;
    BDBG_INSTANCE_MSG(file, ("fifo_out_set: %u index(%lld:%lld) data(%lld:%lld)", file->cfg.interval, file->cfg.data.soft, file->cfg.data.hard, file->cfg.index.soft, file->cfg.index.hard));
    /* XXX we should truncate file somewhere, sometime */
    b_lock(file->index.file);
    file->index.file->size = file->cfg.index.soft;
    b_unlock(file->index.file);
    b_lock(file->data.file);
    file->data.file->size = file->cfg.data.soft;
    b_unlock(file->data.file);
    return NEXUS_SUCCESS;
}

