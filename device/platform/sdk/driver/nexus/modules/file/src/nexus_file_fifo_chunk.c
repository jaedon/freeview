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
 * $brcm_Workfile: nexus_file_fifo_chunk.c $
 * $brcm_Revision: 18 $
 * $brcm_Date: 10/4/12 4:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file/src/nexus_file_fifo_chunk.c $
 * 
 * 18   10/4/12 4:24p mphillip
 * SW7231-1002: Close currently open chunk on close to avoid file
 *  descripter leak
 * 
 * 17   8/29/12 10:12a erickson
 * SW7346-994: don't rely on bcmplayer BNAV_Version detection because it's
 *  not possible for file fifos with AVC
 * 
 * 16   7/23/12 3:58p vsilyaev
 * SW7425-3552: Don't save a file name on the stack
 * 
 * 15   5/29/12 5:31p mward
 * SW7344-333:  Reduce stack use in NEXUS_ChunkedFifoRecord_Create().
 * 
 * 14   4/24/12 5:10p mphillip
 * SW7405-5644: Retire chunks from a previous record if it is re-opened to
 *  record over (mimicking O_TRUNC behavior of other record subsystems)
 * 
 * 13   2/15/12 11:18a erickson
 * SW7405-5609: move unlink system call into posix layer
 * 
 * 12   2/15/12 10:55a erickson
 * SW7425-2282: set BNAV_Player_Settings.changingBounds, improve DBG
 * 
 * 11   1/17/12 5:55p mphillip
 * SW7405-5493: Fix previous checkin
 * 
 * 10   1/16/12 3:39p mphillip
 * SW7405-5493: Correct error order after earlier changes
 * 
 * 9   1/2/12 3:16p mphillip
 * SW7405-5493: Fix O_DIRECT behavior
 * 
 * 8   12/28/11 10:39a mphillip
 * SW7335-1375: Read data fifo header into the correct location in the
 *  structure on fifo files opened only for reading
 * 
 * 7   10/12/11 7:56a dlwin
 * SW7231-352: Added code to silence Coverity 32988 issue.
 * 
 * 6   8/31/11 2:07p vsilyaev
 * SW7231-351: Fixed typo
 * 
 * 5   8/29/11 2:47p vsilyaev
 * SW7231-351: Fixed typo
 * 
 * 4   8/15/11 3:47p mphillip
 * SW7335-1249: Added option to periodically write metadata into a disk
 *  file
 * 
 * 3   2/25/11 12:47p erickson
 * SW7125-818: use BNAV_GetEntrySize
 *
 * 2   5/11/10 11:17a jtna
 * SW7405-4313: Coverity Defect ID:21991 FORWARD_NULL
 *
 * 1   4/22/10 6:57p mphillip
 * SW7405-4251: Merge chunked PVR support from branch to main
 *
 * SW7405-3587/4   4/22/10 12:01p mphillip
 * SW7405-4251: Update chunked PVR after API review, prepare for merge to
 *  main.
 *
 * SW7405-3587/3   4/15/10 11:31a mphillip
 * SW7405-3587: Update with header changes
 *
 * SW7405-3587/2   2/22/10 5:23p mphillip
 * SW7405-3587: Properly reference sub-chunk files
 *
 * SW7405-3587/1   2/4/10 10:21a mphillip
 * SW7405-3587: Chunked fifo
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

BDBG_MODULE(nexus_file_fifo_chunk);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FILE_PERMISSION)
#define OWNER	CONFIG_BCM_FILE_UID
#define GROUP	CONFIG_BCM_FILE_GID
#define PERM	0770
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
#define RECORD_CHUNK_SIZE (1024*47*48*4*10) /*(10*1024*1024)*/
#define LINEAR_ENTRY_MAX 4
#define IDX_COPY_CHUNK_SIZE 128*1024

off_t cur_pos = 0;
off_t data_written = 0;
int wrap_around_point_passed = 0;
int data_copy_completed = 0;
char index_buffer[IDX_COPY_CHUNK_SIZE];
#endif

struct bpvrfifo_entry {
    off_t size; /* size of the entry */
    off_t voff; /* file 'virtual' offset */
    off_t doff; /* disk 'physical' offset */
    unsigned no; /* chunk no */
};

struct bpvrfifo_signature {
    char signature[16];
};

static const struct bpvrfifo_signature bpvrfifo_signature =
{
    "bpvrFiFo 0.3"
};
static void bfile_fifo_write_snapshot_locked(struct NEXUS_ChunkedFifoRecord *file);

#define TEMPLATE_SIZE 128
#define FILE_NAME_SIZE 2048

#define BFIFO_N_ENTRYS  512
#define BFIFO_INVALID_CHUNK ((unsigned)(-1))
struct bpvrfifo_header {
    /* we use dual lock scheme, lock is acquired every time when header table is accessed,  and busy flag is set wherever there is an active transaction, in this manner we allow simultaneous read and write transactions, and effectively serialize trim with both read and write */
    BKNI_MutexHandle lock;
    unsigned busy; /* busy counter, if busy > 0 then there is an active write or read, and trim has to fail */
    unsigned prev_trim;     /* previously trimmed value */
    unsigned trim_gap;      /* Gap between real start of file and next trim point */
    bool chunked;
    bool active;
    unsigned trim_count;
    union {
        struct {
            struct bpvrfifo_signature signature;
            struct bpvrfifo_entry entrys[BFIFO_N_ENTRYS+1];
            enum {
                bpvrfifo_type_master=0, /* master (record file) */
                bpvrfifo_type_snapshot=1, /* read only snapshot file) */
                bpvrfifo_type_zombie=2, /* deleted master file, with active snapshots */
                bpvrfifo_type_unit=3 /* single, not chunked, file */
            } type; /* type of file */
            union {
                struct {
                    unsigned chunk_no[BFIFO_N_ENTRYS];
                    uint16_t chunk_ref_cnt[BFIFO_N_ENTRYS];
                    char chunkTemplate[TEMPLATE_SIZE]; /* template for the chunk name */
                } master;
                struct {
                    char chunkTemplate[TEMPLATE_SIZE]; /* template for the chunk name */
                    char master_file[FILE_NAME_SIZE]; /* name of the master file (only used for delete operations) */
                } snapshot;
            } meta;
        } h;
        uint8_t buf[24*1024]; /* for performance reasons header shall be multiplier of the FS block size */
    } data; /* this is persistent portion of index (e.g. part that is saved on the disk */
};
#define B_ENTRY(f,i) (&(f)->header->data.h.entrys[(i)])
#define b_lock(x) BKNI_AcquireMutex(x->header->lock)
#define b_unlock(x) BKNI_ReleaseMutex(x->header->lock)

struct bfile_io_write_fifo {
    struct bfile_io_write self;
    const struct bpvrfifo_entry *entry_cache; /* cached value of file entry */
    struct bfile_io_write_posix data;
    struct bfile_io_write_posix chunk_data; /* data chunk */
    unsigned chunk_data_no;
    struct bpvrfifo_header *header;
    struct bpvrfifo_header _header;
    off_t offset; /* current virtual offset, also known as a file size */
    off_t size; /* size of the file on a disk */
    off_t start;  /* offset, where writeable area begins */
    unsigned ref_cnt; /* number of references, including itself */
    unsigned next_chunk;
    char chunk_name[FILE_NAME_SIZE];
    struct bfile_io_write_posix data_; /* placeholder */
    struct bfile_io_write_posix chunk_data_;
    bool direct;
    char file_name[FILE_NAME_SIZE];
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    char file_name_old[FILE_NAME_SIZE];
#endif
    struct bfile_io_write_fifo *meta; /* file to save meta data */
    off_t meta_offset;
    off_t meta_size;
    struct NEXUS_ChunkedFifoRecord *writeSnapshotFile;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    unsigned switchtolinear;
    int notswitchedyet;
    int idxnotswitchedyet;
    BNAV_Player_Position first;
    unsigned lastdeletedchunk;
    int chunk_deleted;
    int next_chunk_delta;
    void *parent;	
#endif
};

struct bfile_io_read_fifo {
    struct bfile_io_read self;
    struct bfile_io_read_posix data;
    struct bfile_io_read_posix chunk_data; /* data chunk */
    unsigned chunk_data_no;
    struct bpvrfifo_entry *entry_cache; /* cached value of file entry */
    unsigned trim_count;
    struct bpvrfifo_header *header;
    off_t cur_pos; /* current virtual offset */
    struct bfile_io_write_fifo *writer;
    char chunk_name[FILE_NAME_SIZE];
    struct bfile_io_read_posix data_; /* placeholder */
    struct bfile_io_read_posix chunk_data_; /* placeholder */
    bool direct;
    char file_name[FILE_NAME_SIZE];
    const char *chunkTemplate;
    char master_name[FILE_NAME_SIZE];
    struct bfile_io_read_fifo *meta; /* file to save meta data */
    off_t meta_offset;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    off_t index_first;
    off_t index_last;
    off_t file_end_limit;
    off_t file_start_limit;
    void *parent;
    bool bfromwrite;
#endif
};

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
struct bfile_out_desc {
    struct bfile_io_write_fifo *file;
    unsigned trim_count;
    off_t trim_off;
};

struct NEXUS_ChunkedFifoRecord {
    struct NEXUS_FileRecord self;
    struct bfile_out_desc data;
    struct bfile_out_desc index;
    struct bfile_io_read_fifo *rd_index;
    struct bfile_io_read_fifo *rd_index2;	
    unsigned index_entrysize;
    BNAV_Player_Handle bcm_player;
    NEXUS_TimerHandle timer; /* currently active timer  or NULL */
    NEXUS_Time lastSnapshotTime;
    NEXUS_ChunkedFifoRecordSettings cfg;
    unsigned transition2linearRec;
    unsigned secfromstart;
    off_t cur_idx_off;	
    struct bfile_io_read_posix old_idx_read;
    struct bfile_io_write_posix new_idx_write;
};
#endif

#define B_ENTRY_CLEAR(entry) do{ (entry)->no=BFIFO_INVALID_CHUNK;(entry)->doff=-1;(entry)->voff=0;(entry)->size=0;}while(0)
#define B_ENTRY_IS_EMPTY(entry) ((entry)->no==BFIFO_INVALID_CHUNK)

#if 0
#define BDBG_MSG_IO(i, x) BDBG_INSTANCE_MSG(i, x)
#else
#define BDBG_MSG_IO(i, x)
#endif

#if 0
#define B_DUMP_ENTRY(file, i) BDBG_MSG_IO(file, ("%#x:%u %lld %lld %u", file, i, B_ENTRY(file,i)->voff, B_ENTRY(file,i)->doff, (unsigned)B_ENTRY(file,i)->size))
#else
#define B_DUMP_ENTRY(file, i)
#endif

#if 0
#define B_DUMP_ENTRYS(file) do{unsigned i;for(i=0;i<BFIFO_N_ENTRYS;i++) {B_DUMP_ENTRY(file,i);if(B_ENTRY_IS_EMPTY(B_ENTRY(file,i))) {break;}}}while(0)
#else
#define B_DUMP_ENTRYS(file)
#endif

#ifdef DIRECT_IO_SUPPORT
/* we need to maintain data file aligned by 4096 (for DIRECT I/O) and 188 aligned to keep it valid transport packet */
/* it rounds in 192K */
#define B_DATA_ALIGN  ((188/4)*4096)
#else
#define B_DATA_ALIGN  (188)
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
static const char b_file_chunkTemplate[]="%s.%ld";
#else
static const char b_file_chunkTemplate[]="%s_%d%03d";
#endif

void NEXUS_ChunkedFifoRecord_GetDefaultSettings(
    NEXUS_ChunkedFifoRecordSettings *pSettings /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_ChunkedFifoRecordSettings));
    pSettings->interval = 30;
    pSettings->data.soft = 512*1024*1024;
    pSettings->data.hard = 1024*1024*1024;
    pSettings->data.soft -= pSettings->data.soft % B_DATA_ALIGN;
    pSettings->data.hard -= pSettings->data.hard % B_DATA_ALIGN;
    pSettings->index.soft = 4*1024*1024;
    pSettings->index.hard = 8*1024*1024;
    BKNI_Snprintf(pSettings->chunkTemplate,TEMPLATE_SIZE,"%s",b_file_chunkTemplate);
}

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

static NEXUS_Error
b_pvrfifo_write_sync(struct bfile_io_write_fifo *file)
{
    void *buf;
    off_t off, rc_off;
    int rc;
    NEXUS_Error b_rc = NEXUS_SUCCESS;

    off = file->meta_offset;

    if(file->meta != file) { /* lock meta file if we aren't meta */
        for(;;) {
            b_lock(file->meta);
            if(file->meta->header->busy==0) {
                break;
            }
            b_unlock(file->meta);
            BDBG_WRN(("b_pvrfifo_write_sync: %#lx collision in access to the meta file %#lx", (unsigned long)file, (unsigned long)file->meta));
            BKNI_Sleep(1);
        }
    }
    if(file->meta_size < off+sizeof(file->header->data)) {
        file->meta_size = off+sizeof(file->header->data);
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    /* TEMP (disable)*/
#else
    rc_off = b_wseek(&file->meta->data, off, SEEK_SET);
    if(rc_off != off) {
        b_rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto done;
    }
    buf = &file->header->data;
    rc = b_write(&file->meta->data, buf, sizeof(file->header->data));
    if(rc!=sizeof(file->header->data)) {
        b_rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto done;
    }
#endif
done:
    if(file->meta != file) {
        b_unlock(file->meta);
    }
    return b_rc;
}

static void
b_pvrfifo_write_next_unit(struct bfile_io_write_fifo *file)
{
    struct bpvrfifo_entry *entry;
    unsigned i;

    entry = B_ENTRY(file, 0);
    /* reached the end of file, now is a time to allocate new chunk */
    if (entry->doff == file->start) {
        BDBG_MSG_IO(file, ("write tail"));
        /* try to allocate new entry at the end of file */
        for(entry=NULL,i=1;i<BFIFO_N_ENTRYS;i++) {
            entry = B_ENTRY(file, i);
            if(entry->doff==-1) { /* empty entry */
                if(entry[-1].doff + entry[-1].size < file->size) { /* last entry shall not span to the end of file */
                    entry->voff = file->offset;
                    entry->doff = entry[-1].doff + entry[-1].size;
                    entry->size = 0;
                    entry->no = file->next_chunk;
                } else {
                    BDBG_MSG(("bpvrfifo_write: %#lx no space at the end of file", (unsigned long)file));
                    entry = NULL;
               }
               break;
            }
        }
        if(!entry) {
            BDBG_MSG(("bpvrfifo_write: %#lx overflow", (unsigned long)file));
        }
    } else {
        BDBG_MSG_IO(file, ("write wraparound"));
        for(i=0;i<BFIFO_N_ENTRYS;i++) {  /* find first not empty entry */
            if(B_ENTRY_IS_EMPTY(B_ENTRY(file,i))) {
                break;
            }
        }
        /* new entry _always_ should go on top, so shift old entrys down, so we could keep entries sorted by the 'doff' field */
        for(;i>0;i--) {
            *B_ENTRY(file,i) = *B_ENTRY(file, i-1);
        }
        entry = B_ENTRY(file, 0);
        entry->voff = file->offset;
        entry->doff = file->start;
        entry->size = 0;
        entry->no = file->next_chunk;
    }
    return;
}

static const char *
b_pvrfifo_make_chunk_name(char *buf, size_t buf_size, const char *template, const char *file_name, unsigned chunk_no)
{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    BKNI_Snprintf(buf, buf_size, template,  file_name, chunk_no);
#else
    BKNI_Snprintf(buf, buf_size, template,  file_name, chunk_no/64, chunk_no%64);
#endif
    return buf;
}

static NEXUS_Error
b_pvrfifo_remove_chunk(char *buf, size_t buf_size, const char *template, const char *file_name, unsigned chunk_no)
{
    NEXUS_Error rc;

    BDBG_MSG(("b_pvrfifo_remove_chunk: %s removing chunk %u", file_name, chunk_no));
    b_pvrfifo_make_chunk_name(buf, buf_size, template, file_name, chunk_no);
    rc = bfile_io_posix_unlink(buf);
    if(rc!=0) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    return NEXUS_SUCCESS;
}

static void
b_pvrfifo_register_new_chunk(struct bfile_io_write_fifo *file)
{
    unsigned i;
    BDBG_MSG(("b_pvrfifo_register_new_chunk: %#lx allocating chunk %u", (unsigned long)file, file->next_chunk));
    for(i=0;i<BFIFO_N_ENTRYS;i++) {
        if(file->header->data.h.meta.master.chunk_no[i]==BFIFO_INVALID_CHUNK) {
            file->header->data.h.meta.master.chunk_no[i] = file->next_chunk;
            file->header->data.h.meta.master.chunk_ref_cnt[i] = 1;
            break;
        }
    }
    if(i==BFIFO_N_ENTRYS) {
        BDBG_ERR(("b_pvrfifo_register_new_chunk: %#lx can't allocate space for chunk no %u", (unsigned long)file, (unsigned)file->next_chunk));
    }
    file->next_chunk++;
    return;
}

static void
b_pvrfifo_write_next_chunk(struct bfile_io_write_fifo *file)
{
    struct bpvrfifo_entry *entry;
    unsigned i;

    for(i=0;i<BFIFO_N_ENTRYS;i++) {
        entry = B_ENTRY(file, i);
        if(B_ENTRY_IS_EMPTY(entry)) {
            /* just find empty entry, and allocate new chunk */
            entry->voff = file->offset;
            entry->doff = 0;
            entry->size = 0;
            entry->no = file->next_chunk;
            b_pvrfifo_register_new_chunk(file);
            b_unlock(file);
            b_pvrfifo_write_sync(file);
            b_lock(file);
            break;
        }
    }
    return;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
char SRC_FILE_NAME[FILE_NAME_SIZE];
char DST_FILE_NAME[FILE_NAME_SIZE];
#endif
static ssize_t
bpvrfifo_write(bfile_io_write_t fd, const void *buf_, size_t length)
{
    struct bfile_io_write_fifo *file = (struct bfile_io_write_fifo*) fd;
    ssize_t result;
    const void *buf = buf_;
    unsigned i;
    struct bfile_io_write_posix *write_fd;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    struct bpvrfifo_entry *entry;
    struct bpvrfifo_entry *entry1;
    unsigned entry1_count;
    unsigned rmi, rmj;
    unsigned rm_loop_count;
    static int blogoutput = 0;
    static int recSizeMilestone= 0;
    int  recSizetemp;
    static int notswitchedyet = 0;
#endif

    BDBG_ASSERT(file->header);
    BDBG_MSG_IO(file, (">write: %s %u %lld:%u", file->header->chunked?"chunked":"", file->header->busy, file->offset, (unsigned)length ));
    b_lock(file);
    file->header->busy++;
    file->header->active = true;


#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)

    if(file->switchtolinear == 1)
    {
        entry1_count = 0;
        for(rmi=0;rmi<BFIFO_N_ENTRYS;rmi++) 
        {
            entry1 = B_ENTRY(file, rmi);
            if(!B_ENTRY_IS_EMPTY(entry1)) 
            {
                entry1_count ++;
            }
            else
            {
        	    break;
            }
        }

        if(entry1_count > LINEAR_ENTRY_MAX)
        {
            for(rmi=0;rmi<(entry1_count - LINEAR_ENTRY_MAX);rmi++) 
            {
                entry1 = B_ENTRY(file, rmi);
                for(rmj=0;rmj<BFIFO_N_ENTRYS;rmj++) 
                {
                    if(file->header->data.h.meta.master.chunk_no[rmj]==entry1->no)
        			{
                        /* bingo found chunk */
                        if(file->header->data.h.meta.master.chunk_ref_cnt[rmj]>0) 
                        {
                            file->header->data.h.meta.master.chunk_ref_cnt[rmj]--;
                            if(file->header->data.h.meta.master.chunk_ref_cnt[rmj]==0) 
                            {
                                file->header->data.h.meta.master.chunk_no[rmj]=BFIFO_INVALID_CHUNK;
                            }
                            else
                            {
                        	    BDBG_ERR(("[hee] shall not be happened.\n"));
                                return -1;
                            }
                        }
                        break;
                    }
                }	
                B_ENTRY_CLEAR(entry1);
            } 
        
            for(rmi=0;rmi<BFIFO_N_ENTRYS;rmi++) 
            {
                if (B_ENTRY_IS_EMPTY(B_ENTRY(file,rmi)))
                {
                    for(rmj=rmi+1;rmj<BFIFO_N_ENTRYS;rmj++) 
                    {
                        if (!B_ENTRY_IS_EMPTY(B_ENTRY(file,rmj)))
                        {
                            *B_ENTRY(file, rmi) = *B_ENTRY(file, rmj);
                            B_ENTRY_CLEAR(B_ENTRY(file, rmj));
                            break;
                        }
                    }
                    if (rmj==BFIFO_N_ENTRYS)
					{
                     break;
                    }
                }
            }    
        }	
    }
#endif


    for(result=0;length>0;) {
        size_t to_write = 0;
        ssize_t rc;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
#else
        struct bpvrfifo_entry *entry;
#endif
        off_t boundary = 0;
        off_t rc_off;

        /* coverity[use] */
        for(entry = (struct bpvrfifo_entry *)file->entry_cache, i=0;i<BFIFO_N_ENTRYS;) {
            if(entry==NULL) {
                entry = B_ENTRY(file, i);
                i++;
                if (B_ENTRY_IS_EMPTY(entry)) {
                    entry = NULL;
                    break;
                }
            }
            if(entry->voff + entry->size == file->offset) {
                boundary = file->size;
                if(!file->header->chunked && !B_ENTRY_IS_EMPTY(&entry[1])) {
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
			if(file->switchtolinear == 0)
			{
#endif
                    boundary = entry[1].doff;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
			}
#endif
                }
                to_write = length;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
		if(!(file->switchtolinear == 1 && !file->header->chunked))
		{
                    if(file->notswitchedyet == 0 && file->switchtolinear == 100 && to_write < (boundary - (entry->doff + entry->size)))
                    {
                        BDBG_ERR(("Start moving chunk files."));
                        file->switchtolinear = 1;
                    }
#endif
                if (to_write > boundary - (entry->doff + entry->size)) {
                    to_write = boundary - (entry->doff + entry->size);
                }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
		}
#endif
                BDBG_MSG_IO(file, ( "to_write:%u file->size:%lld entry[1].doff:%lld entry->doff:%lld entry->size:%u", to_write, file->size, entry[1].doff, entry->doff, entry->size)); /* add this */
                if(to_write>0) {
                    break;
                }
            }
            entry = NULL;
        }
        file->entry_cache = entry;
        if (!entry) {
            BDBG_WRN(("can't write to descriptor %#x:%lld", (unsigned)file,file->offset));
            if(result==0) {
                result = -1;
            }
            break;
        }
        if(file->header->chunked) {
            write_fd = &file->chunk_data;
            if(file->chunk_data_no != entry->no) {
                off_t rc;
                if(file->chunk_data_no != BFIFO_INVALID_CHUNK) {
                    bfile_io_write_posix_close(&file->chunk_data);
                    file->chunk_data_no = BFIFO_INVALID_CHUNK;
                }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
                if(file->notswitchedyet == 1)
                {
                    b_pvrfifo_make_chunk_name(file->chunk_name, sizeof(file->chunk_name), file->header->data.h.meta.master.chunkTemplate,  file->file_name, (unsigned)((int)(entry->no)+file->next_chunk_delta));
                }
                else
                {
#endif
                b_pvrfifo_make_chunk_name(file->chunk_name, sizeof(file->chunk_name), file->header->data.h.meta.master.chunkTemplate,  file->file_name, entry->no);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
                }
#endif
                b_unlock(file);
                rc = bfile_io_write_posix_open(write_fd, file->chunk_name, file->direct);
                b_lock(file);
                if(rc) {
                    BDBG_WRN(("can't open file %s", file->chunk_name));
                    if(result==0) {
                        result = -1;
                    }
                    break;
                }
                file->chunk_data_no = entry->no;
            }
        } else {
            write_fd = &file->data;
        }


#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
       if(file->header->chunked) 
	{
		if(file->notswitchedyet == 0 && file->switchtolinear == 1)
		{
			file->notswitchedyet = 1;
			/*close-rename-reopen current record file*/
			bfile_io_write_posix_close(&file->chunk_data);	
			rm_loop_count = (file->chunk_data_no - file->lastdeletedchunk);
			if(file->chunk_deleted == 0) rm_loop_count++;
			for(rmi=0; rmi < rm_loop_count; rmi++)
			{
				if(file->chunk_deleted == 0)
				{
		              	b_pvrfifo_make_chunk_name(SRC_FILE_NAME, sizeof(SRC_FILE_NAME), file->header->data.h.meta.master.chunkTemplate,  file->file_name_old,   file->lastdeletedchunk+rmi);
				}
				else
				{
		              	b_pvrfifo_make_chunk_name(SRC_FILE_NAME, sizeof(SRC_FILE_NAME), file->header->data.h.meta.master.chunkTemplate,  file->file_name_old,   file->lastdeletedchunk+1+rmi);
				}
		        b_pvrfifo_make_chunk_name(DST_FILE_NAME, sizeof(DST_FILE_NAME), file->header->data.h.meta.master.chunkTemplate,  file->file_name,  rmi+1);
                rename(SRC_FILE_NAME,DST_FILE_NAME);
			}
	        b_pvrfifo_make_chunk_name(SRC_FILE_NAME, sizeof(SRC_FILE_NAME), file->header->data.h.meta.master.chunkTemplate,  file->file_name,  0);
			bfile_io_posix_touch(SRC_FILE_NAME);
                    if(file->chunk_deleted == 0)
                        file->next_chunk_delta = 1;
                    else
                        file->next_chunk_delta = -file->lastdeletedchunk;
			b_unlock(file);
			rc = bfile_io_write_posix_openappend(write_fd, DST_FILE_NAME, file->direct);
			b_lock(file);
            if(rc) {
                BDBG_WRN(("can't open file %s", DST_FILE_NAME));
                if(result==0) {
                    result = -1;
                }
                break;
            }			
			data_copy_completed=1;
		}
        }
	 else
 	{
		if(file->idxnotswitchedyet == 0 && file->switchtolinear == 1)
		{
			entry->voff = 0;
			entry->doff = 0;
			entry->size =  ((NEXUS_ChunkedFifoRecordHandle)file->parent)->cur_idx_off;
			file->offset = ((NEXUS_ChunkedFifoRecordHandle)file->parent)->cur_idx_off;
#if 1
			bfile_io_write_posix_close(&file->data);
#else
			BKNI_Memcpy(&gdata_read,&file->data, sizeof(struct bfile_io_write_posix));
			BKNI_Memset(&file->data, 0,  sizeof(struct bfile_io_write_posix));
			file->data.fd = -1;	
#endif
			b_unlock(file);

			rc = bfile_io_write_posix_open(write_fd, file->file_name, file->direct);
		
			b_lock(file);	
	                if(rc) {
	                    BDBG_WRN(("haha can't open file %s", file->file_name));
	                    if(result==0) {
	                        result = -1;
	                    }
	                    break;
	                }
			file->idxnotswitchedyet = 1;
		}
		else if(file->idxnotswitchedyet == 1)
		{

		}
 	}
#endif
        /* BDBG_ASSERT(boundary>0); */
         BDBG_MSG_IO(file, (">writing:%#x [%lld/%lld/%lld/%d] (%u/%u/%lld) %#x", entry->voff, entry->doff, boundary, entry-B_ENTRY(file,0), (unsigned)result, (unsigned)to_write, (unsigned)length, file->offset, (unsigned)buf));
         b_unlock(file);
         rc_off = b_wseek(write_fd, entry->doff + entry->size, SEEK_SET);
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
         rc = b_write(write_fd, buf, to_write);
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

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	if(!(file->switchtolinear == 1 && !file->header->chunked))
	{
#endif
        BDBG_ASSERT(entry->doff+entry->size <= file->size);
        if (entry->doff + entry->size == boundary) {
            file->entry_cache = NULL;
            if(file->header->chunked) {
                b_pvrfifo_write_next_chunk(file);
            } else {
                b_pvrfifo_write_next_unit(file);
            }
        }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    }
#endif
    }
    file->header->busy--;
    B_DUMP_ENTRYS(file);
    BDBG_MSG_IO(file, ("<write: %s %u %lld:%u", file->header->chunked?"chunked":"", file->header->busy, file->offset, (unsigned)length ));
    if(file->writeSnapshotFile) {
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
		/*no snapshot...*/
#else
        bfile_fifo_write_snapshot_locked(file->writeSnapshotFile);
#endif
        file->writeSnapshotFile = NULL;
    }
    b_unlock(file);
    return result;
}

static ssize_t
bpvrfifo_read(bfile_io_read_t fd, void *buf_, size_t length)
{
    struct bfile_io_read_fifo *file = (struct bfile_io_read_fifo*) fd;
    ssize_t result;
    void *buf = buf_;
    unsigned i;
    ssize_t rc;
    struct bfile_io_read_posix *read_fd;


    BDBG_ASSERT(file->header);
    BDBG_MSG_IO(file, (">read:%u %lld:%u", file->header->busy, file->cur_pos, (unsigned)length));
    b_lock(file);
    file->header->busy++;
    for(result=0;length>0;) {
        off_t rc_off, seek_off;
        size_t to_read = length;

        {
            const struct bpvrfifo_entry *entry; /* when lock is released writer could update move entries around, so protect access to the volatile storage */

        if(file->trim_count != file->header->trim_count) {
            file->trim_count = file->header->trim_count;
            file->entry_cache = NULL;
        }

        for(entry=file->entry_cache,i=0;i<BFIFO_N_ENTRYS;) {
            if(entry==NULL) {
                entry = B_ENTRY(file,i);
                i++;
                if (B_ENTRY_IS_EMPTY(entry)) {
                    entry = NULL;
                    break;
                }
            }
            if( entry->voff <= file->cur_pos &&
                entry->voff + entry->size > file->cur_pos) {
                to_read = length;
                if (to_read > entry->size - (file->cur_pos - entry->voff)) {
                    to_read =  entry->size - (file->cur_pos - entry->voff);
                }
                if(to_read > 0) {
                    break;
                }
            }
            entry = NULL;
        }
        file->entry_cache = (struct bpvrfifo_entry *)entry;
        if (!entry) {
            if (result==0) {
                if (B_ENTRY(file, 0)->voff > file->cur_pos) {
                    B_DUMP_ENTRY(file,0);
                    B_DUMP_ENTRY(file,1);
                    BDBG_WRN(("bpvrfifo_read: out of bounds offset %#x,%lld", (unsigned)file,file->cur_pos));
                    result = -1;
                }
            }
            break;
        }
        if(file->header->chunked) {
            read_fd = &file->chunk_data;
            if(file->chunk_data_no != entry->no) {
                NEXUS_Error rc;
                if(file->chunk_data_no != BFIFO_INVALID_CHUNK) {
                    bfile_io_read_posix_close(&file->chunk_data);
                    file->chunk_data_no = BFIFO_INVALID_CHUNK;
                }
                b_pvrfifo_make_chunk_name(file->chunk_name, sizeof(file->chunk_name), file->chunkTemplate,  file->master_name, entry->no);
                b_unlock(file);
                rc = bfile_io_read_posix_open(read_fd, file->chunk_name, file->direct);
                b_lock(file);
                if(rc!=NEXUS_SUCCESS) {
                    BDBG_WRN(("can't open file %s", file->chunk_name));
                    if(result==0) {
                        result = -1;
                    }
                    break;
                }
                file->chunk_data_no = entry->no;
            }
        } else {
            read_fd = &file->data;
        }
        seek_off = entry->doff + (file->cur_pos - entry->voff);
        BDBG_MSG_IO(file, (">reading: [%lld/%lld/%d -> %lld] (%u/%u/%u/%lld) %#x", entry->voff, entry->doff, entry-B_ENTRY(file,0), seek_off, (unsigned)result, (unsigned)to_read, (unsigned)length, file->cur_pos, (unsigned)buf));
        }
        b_unlock(file);
        rc_off = b_rseek(read_fd, seek_off, SEEK_SET);
        b_lock(file);
        BDBG_MSG_IO(file, ("rseek: %lld -> %lld", seek_off, rc_off));
        if (rc_off != seek_off) {
            if (result==0) {
                if (rc_off < 0) {
                    result = (ssize_t)rc_off; /* propagate error */
                } else {
                    BDBG_WRN(("bpvrfifo_read: %#lx can't seek to %lld(instead %lld)", (unsigned long)file, seek_off, rc_off));
                    result = -1;
                }
            }
            break;
        }
        b_unlock(file);
        rc = b_read(read_fd, buf, to_read);
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
    B_DUMP_ENTRYS(file);
    BDBG_MSG_IO(file, ("<read:%u %lld:%u=%d", file->header->busy, file->cur_pos, (unsigned)length, (int)result));

    b_unlock(file);
    return result;
}


static int
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
bpvrfifo_bounds_locked(struct bfile_io_read_fifo *file, off_t *first, off_t *last)
#else
bpvrfifo_bounds_locked(const struct bfile_io_read_fifo *file, off_t *first, off_t *last)
#endif
{
    unsigned i;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	off_t first_org;
	off_t first_doff, last_doff;
	off_t temp;
#endif	

    BDBG_ASSERT(file->header);

    if (B_ENTRY_IS_EMPTY(B_ENTRY(file,0))) {
        *first = *last = 0;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)		
	  first_doff = last_doff = 0; 
#endif
    } else {
        *first = B_ENTRY(file,0)->voff;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	first_doff =  B_ENTRY(file,0)->doff;
#endif
        *last = B_ENTRY(file,0)->voff + B_ENTRY(file,0)->size;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	last_doff = B_ENTRY(file,0)->doff + B_ENTRY(file,0)->size;
#endif
    }

    for(i=1;i<BFIFO_N_ENTRYS;i++) {
        if (B_ENTRY_IS_EMPTY(B_ENTRY(file,i))) {
            break;
        }
        if(B_ENTRY(file,i)->voff >= *last) {
            *last = B_ENTRY(file,i)->voff + B_ENTRY(file,i)->size;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
		last_doff = B_ENTRY(file,i)->doff + B_ENTRY(file,i)->size;
#endif
        }
        if(B_ENTRY(file,i)->voff < *first) {
            *first = B_ENTRY(file,i)->voff;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
		first_doff =  B_ENTRY(file,i)->doff;
#endif	
        }
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	first_org = *first;
#endif
    if( file->header->trim_gap ) {
        *first += (off_t)(file->header->trim_gap);
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	if(last_doff >= first_doff)
	{
		file->index_first = first_doff;
		file->index_last = last_doff;

		if(file->bfromwrite)
		{
			((NEXUS_ChunkedFifoRecordHandle)(file->parent))->cur_idx_off  = (last_doff-first_doff);
		}

	}
	else
	{
		temp = (file->writer->size - first_doff) + (last_doff - 49152);

		file->index_first = first_doff;
		file->index_last = last_doff;
		if(file->bfromwrite)
		{
			((NEXUS_ChunkedFifoRecordHandle)(file->parent))->cur_idx_off = temp;
		}	
	}

	file->file_start_limit = 49152;
	file->file_end_limit = file->writer->size;
#endif
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
    unsigned i,j;
    struct bpvrfifo_entry *entry;
    bool dirty = false;

    BDBG_ASSERT(file->header);
    b_lock(file);
    BDBG_MSG_IO(file, (">trim:%u %lld", file->header->busy, trim_pos));
    file->header->active = true;
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

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
if(!file->header->chunked) {
    for(i=0;i<BFIFO_N_ENTRYS;i++) 
    {
        entry = B_ENTRY(file,i);
    }
  }
#endif
    for(i=0;i<BFIFO_N_ENTRYS;i++) {
        entry = B_ENTRY(file,i);

        if (B_ENTRY_IS_EMPTY(entry)) {
            break;
        } else if( entry->voff >= trim_pos) {
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)		
            /*if all entry matches this condition. no trim required. just return success.*/
            if(i==0)
            {
                result = -1;
                goto done;
            }
#endif
            continue;
        } else if (entry->voff + entry->size > trim_pos ) {
            entry->size -= trim_pos - entry->voff;
            entry->doff += trim_pos - entry->voff;
            entry->voff = trim_pos;
            result = trim_pos;
        } else {
            if (result < entry->voff + entry->size) {
                result = entry->voff + entry->size;
            }
            if(file->header->chunked) {
                /* close current chunk */
                if(file->chunk_data_no == entry->no) {
                    bfile_io_write_posix_close(&file->chunk_data);
                    file->chunk_data_no = BFIFO_INVALID_CHUNK;
                }
                /* XXX it's possible that readers still have chunk opened */
                for(j=0;j<BFIFO_N_ENTRYS;j++) {
                     if(file->header->data.h.meta.master.chunk_no[j]==entry->no) {
                        /* bingo found chunk */
                        if(file->header->data.h.meta.master.chunk_ref_cnt[j]>0) {
                            file->header->data.h.meta.master.chunk_ref_cnt[j]--;
                            if(file->header->data.h.meta.master.chunk_ref_cnt[j]==0) {
#if defined(HUMAX_PLATFORM_BASE)
				b_pvrfifo_make_chunk_name(file->chunk_name, sizeof(file->chunk_name), file->header->data.h.meta.master.chunkTemplate,  file->file_name, entry->no);
#endif								
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
				 if(((NEXUS_ChunkedFifoRecordHandle)(file->parent))->transition2linearRec >= 1)
				 {
                     /*b_pvrfifo_make_chunk_name(file->chunk_name, sizeof(file->chunk_name), file->header->data.h.meta.master.chunkTemplate,	file->file_name_old, entry->no);*/
                     /*bfile_io_posix_unlink(file->chunk_name);*/
					 b_pvrfifo_remove_chunk(file->chunk_name, sizeof(file->chunk_name), file->header->data.h.meta.master.chunkTemplate,  file->file_name_old, entry->no);
				 }
				 else
				 {
#endif
                                b_pvrfifo_remove_chunk(file->chunk_name, sizeof(file->chunk_name), file->header->data.h.meta.master.chunkTemplate,  file->file_name, entry->no);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
				 }
				    file->lastdeletedchunk = entry->no;
				    file->chunk_deleted = 1;
#endif
                                file->header->data.h.meta.master.chunk_no[j]=BFIFO_INVALID_CHUNK;
                            }
                        }
                        break;
                     }
                }
                if(j==BFIFO_N_ENTRYS) {
                    BDBG_ERR(("bpvrfifo_trim: %#lx unknown chunk no %u", (unsigned long)file, (unsigned)entry->no));
                }
            }
            dirty = true;
            B_ENTRY_CLEAR(B_ENTRY(file,i)); /* mark entry as unused */
        }
    }
    /* collapse empty entries */
    for(i=0;i<BFIFO_N_ENTRYS;i++) {
        if (B_ENTRY_IS_EMPTY(B_ENTRY(file,i))) {
            for(j=i+1;j<BFIFO_N_ENTRYS;j++) {
                if (!B_ENTRY_IS_EMPTY(B_ENTRY(file,j))) {
                    *B_ENTRY(file, i) = *B_ENTRY(file, j);
                    B_ENTRY_CLEAR(B_ENTRY(file, j));
                    break;
                }
            }
            if (j==BFIFO_N_ENTRYS) {
                break;
            }
        }
    }
    entry = B_ENTRY(file, 0);
    if(file->header->chunked) {
        if (B_ENTRY_IS_EMPTY(entry)) {
            entry->doff = 0;
            entry->voff = result;
            entry->size = 0;
            entry->no = file->next_chunk;
            b_pvrfifo_register_new_chunk(file);
            dirty = true;
        }
    } else {
        if (B_ENTRY_IS_EMPTY(entry)) {
            /* file was trimmed to bones, create new entry */
            entry->doff = file->start;
            entry->voff = result;
            entry->size = 0;
            entry->no = file->next_chunk;
        }
    }
done:
    B_DUMP_ENTRYS(file);
    file->entry_cache = NULL;
    file->header->trim_count++;
    BDBG_MSG_IO(file, ("<trim:%u %lld", file->header->busy, result));
    if(dirty && file->header->chunked) {
        /* XXX write to the file with acquired lock */
        b_pvrfifo_write_sync(file);
    }
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
        /* special case, which is used to retrieve current position, without bound checking  */
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

static void
b_pvrfifo_header_init(struct bpvrfifo_header *header)
{
    BDBG_ASSERT(header);
    BDBG_CASSERT(sizeof(header->data) == sizeof(header->data.buf)); /* for proper aligment, size of the header shall be equal to size of the buffer */
    BKNI_Memset(header,0,sizeof(*header));
    header->data.h.signature = bpvrfifo_signature;
    header->busy = 0;
    header->prev_trim = 0;
    header->trim_gap = 0;
    header->trim_count = 0;
    header->chunked = false;
    header->active = false;
    return;
}



int
b_file_strcpy(char *dest, size_t buf_len, const char *src)
{
    return BKNI_Snprintf(dest, buf_len, "%s", src);
}

static struct bfile_io_write_fifo *
bpvrfifo_write_open(const char *fname, bool direct, off_t start, struct bfile_io_write_fifo *meta, off_t meta_offset )
{
    struct bfile_io_write_fifo *file =  BKNI_Malloc(sizeof(*file));
    BERR_Code rc;
    unsigned i;

    if (!file) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    if(meta==NULL) {
        meta = file;
    }
    BKNI_Memset(file, 0, sizeof(*file));
    file->direct = direct;
    file->header = &file->_header;
    file->meta = meta;
    file->meta_offset = meta_offset;
    file->meta_size = 0;
    b_pvrfifo_header_init(file->header);
    file->writeSnapshotFile = NULL;
    rc = BKNI_CreateMutex(&file->header->lock);
    if (rc!=BERR_SUCCESS) {
        goto err_lock;
    }

    file->data = file->data_;
    file->chunk_data = file->chunk_data_;
    file->chunk_data_no = BFIFO_INVALID_CHUNK; /* clear chunk file */
    rc = bfile_io_write_posix_open(&file->data, fname, direct);
    if(rc!=NEXUS_SUCCESS) { goto err_open_data; }
    /* initialize data */
    file->self = bpvrfifo_io_write;
    file->start = start;
    file->ref_cnt = 1;
    file->next_chunk = 0;
    file->entry_cache = NULL;
    file->size = 512 * 1024;
    file->header->data.h.type = bpvrfifo_type_unit;
    BKNI_Memcpy(file->header->data.h.meta.master.chunkTemplate, b_file_chunkTemplate, sizeof(b_file_chunkTemplate));
    b_file_strcpy(file->file_name, sizeof(file->file_name), fname); /* save filename for further use */
    for(i=0;i<BFIFO_N_ENTRYS;i++) {
        B_ENTRY_CLEAR(B_ENTRY(file, i));
        file->header->data.h.meta.master.chunk_no[i] = BFIFO_INVALID_CHUNK;
        file->header->data.h.meta.master.chunk_ref_cnt[i] = 0;
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->idxnotswitchedyet = 0;
    file->notswitchedyet = 0;
    file->switchtolinear = 0;
    file->chunk_deleted = 0;
    file->lastdeletedchunk = 0;
    file->next_chunk_delta = 0;
#endif
    B_ENTRY_CLEAR(B_ENTRY(file, BFIFO_N_ENTRYS));
    B_ENTRY(file, 0)->doff = file->start;
    B_ENTRY(file, 0)->no = 0;
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
    if(file->chunk_data_no != BFIFO_INVALID_CHUNK) {
        bfile_io_write_posix_close(&file->chunk_data);
        file->chunk_data_no = BFIFO_INVALID_CHUNK;
    }

    bfile_io_write_posix_close(&file->data);
    bpvrfifo_write_release(file);
    return;
}

static struct bfile_io_read_fifo *
bpvrfifo_read_open(const char *fname, bool direct, struct bfile_io_write_fifo *writer, struct bfile_io_read_fifo *meta, off_t meta_off)
{
    struct bfile_io_read_fifo *file =  BKNI_Malloc(sizeof(*file));
    NEXUS_Error rc;

    if (!file) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    if(meta==NULL) {
        meta = file;
    }
    file->direct = direct;
    file->entry_cache = NULL;
    file->trim_count = 0;
    file->data = file->data_;
    file->chunk_data = file->chunk_data_;
    file->chunk_data_no = BFIFO_INVALID_CHUNK;
    rc = bfile_io_read_posix_open(&file->data, fname, direct);
    if (rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_data; }
    file->writer = writer;
    if (writer) {
        b_lock(writer);
        writer->ref_cnt++;
        file->header = writer->header;
        file->chunkTemplate = writer->header->data.h.meta.master.chunkTemplate;
        BKNI_Memcpy(file->master_name, writer->file_name, sizeof(file->master_name));
        b_unlock(writer);
    } else {
        BERR_Code rc;
        void *buf;
        off_t rc_off;
        ssize_t p_rc;

        file->header = BKNI_Malloc(sizeof(*file->header));
        if(!file->header) {
            BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            goto err_hdr_alloc;
        }
        b_pvrfifo_header_init(file->header);
        rc = BKNI_CreateMutex(&file->header->lock);
        if (rc!=BERR_SUCCESS) {
            BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            goto err_lock;
        }
        rc_off = b_rseek(&meta->data, meta_off, SEEK_SET);
        if(rc_off!=meta_off) {
            BERR_TRACE(NEXUS_UNKNOWN);
            goto err_rseek;
        }
        buf = &file->header->data;
        p_rc = b_read(&meta->data, buf, sizeof(file->header->data));
        if (p_rc!=sizeof(file->header->data) || BKNI_Memcmp(&file->header->data.h.signature, &bpvrfifo_signature, sizeof(bpvrfifo_signature))!=0) {
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto err_meta;
        }
        file->header->chunked = file->header->data.h.type != bpvrfifo_type_unit;
        if(file->header->data.h.type == bpvrfifo_type_snapshot) {
            BKNI_Memcpy(file->master_name, file->header->data.h.meta.snapshot.master_file, sizeof(file->master_name));
            file->chunkTemplate = file->header->data.h.meta.snapshot.chunkTemplate;
        } else {
            b_file_strcpy(file->master_name, sizeof(file->master_name), fname);
            file->chunkTemplate = file->header->data.h.meta.master.chunkTemplate;
        }
    }
    file->self = bpvrfifo_io_read;
    return  file;

err_meta:
err_rseek:
err_lock:
    if(!writer) {
        BKNI_Free(file->header);
    }
err_hdr_alloc:
    bfile_io_read_posix_close(&file->data);
err_open_data:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
static struct bfile_io_read_fifo *
bpvrfifo_read_dup(const char *fname, bool direct, struct bfile_io_write_fifo *writer, struct bfile_io_read_fifo *meta, off_t meta_off, struct bfile_io_read_posix *srcfile)
{
    struct bfile_io_read_fifo *file =  BKNI_Malloc(sizeof(*file));
    NEXUS_Error rc;

    if (!file) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    if(meta==NULL) {
        meta = file;
    }
    file->direct = direct;
    file->entry_cache = NULL;
    file->trim_count = 0;
    file->data = file->data_;
    file->chunk_data = file->chunk_data_;
    file->chunk_data_no = BFIFO_INVALID_CHUNK;
    rc = bfile_io_read_posix_dup(&file->data, srcfile, direct);
    if (rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_open_data; }
    file->writer = writer;	
    if (writer) {
        b_lock(writer);
        writer->ref_cnt++;
        file->header = writer->header;
        file->chunkTemplate = writer->header->data.h.meta.master.chunkTemplate;
        BKNI_Memcpy(file->master_name, writer->file_name, sizeof(file->master_name));
        b_unlock(writer);
    } else {
        BERR_Code rc;
        void *buf;
        off_t rc_off;
        ssize_t p_rc;

        file->header = BKNI_Malloc(sizeof(*file->header));
        if(!file->header) {
            BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            goto err_hdr_alloc;
        }
        b_pvrfifo_header_init(file->header);
        rc = BKNI_CreateMutex(&file->header->lock);
        if (rc!=BERR_SUCCESS) {
            BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            goto err_lock;
        }
        rc_off = b_rseek(&meta->data, meta_off, SEEK_SET);
        if(rc_off!=meta_off) {
            BERR_TRACE(NEXUS_UNKNOWN);
            goto err_rseek;
        }
        buf = &file->header->data;
        p_rc = b_read(&meta->data, buf, sizeof(file->header->data));
        if (p_rc!=sizeof(file->header->data) || BKNI_Memcmp(&file->header->data.h.signature, &bpvrfifo_signature, sizeof(bpvrfifo_signature))!=0) {
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto err_meta;
        }
        file->header->chunked = file->header->data.h.type != bpvrfifo_type_unit;
        if(file->header->data.h.type == bpvrfifo_type_snapshot) {
            BKNI_Memcpy(file->master_name, file->header->data.h.meta.snapshot.master_file, sizeof(file->master_name));
            file->chunkTemplate = file->header->data.h.meta.snapshot.chunkTemplate;
        } else {
            b_file_strcpy(file->master_name, sizeof(file->master_name), fname);
            file->chunkTemplate = file->header->data.h.meta.master.chunkTemplate;
        }
    }
    file->self = bpvrfifo_io_read;
    return  file;

err_meta:
err_rseek:
err_lock:
    if(!writer) {
        BKNI_Free(file->header);
    }
err_hdr_alloc:
    bfile_io_read_posix_close(&file->data);
err_open_data:
    BKNI_Free(file);
err_alloc:
    return NULL;
}


#endif

static bfile_io_read_t
bpvrfifo_read_file(struct bfile_io_read_fifo *file)
{
    return &file->self;
}


static void
bpvrfifo_read_close(struct bfile_io_read_fifo *file)
{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO) /* VFIO */
	if(file->chunk_data.tHandle.tFile.local_nexus.fd >= 0)
#else
	if(file->chunk_data.fd >= 0)
#endif
	{
		bfile_io_read_posix_close(&file->chunk_data);
        file->chunk_data_no = BFIFO_INVALID_CHUNK;
	}
#endif

    if(file->chunk_data_no != BFIFO_INVALID_CHUNK) {
        bfile_io_read_posix_close(&file->chunk_data);
        file->chunk_data_no = BFIFO_INVALID_CHUNK;
    }

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

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
 /*Move these two structure variables to top of this file.*/
#else
struct bfile_out_desc {
    struct bfile_io_write_fifo *file;
    unsigned trim_count;
    off_t trim_off;
};

struct NEXUS_ChunkedFifoRecord {
    struct NEXUS_FileRecord self;
    struct bfile_out_desc data;
    struct bfile_out_desc index;
    struct bfile_io_read_fifo *rd_index;
    unsigned index_entrysize;
    BNAV_Player_Handle bcm_player;
    NEXUS_TimerHandle timer; /* currently active timer  or NULL */
    NEXUS_Time lastSnapshotTime;
    NEXUS_ChunkedFifoRecordSettings cfg;
};
#endif

struct bfile_in_fifo {
    struct NEXUS_FilePlay self;
    struct bfile_io_read_fifo *data;
    struct bfile_io_read_fifo *index;
    struct NEXUS_ChunkedFifoRecord *writer;
};

#define GET_INDEX_FILEIO(fp) (&((NEXUS_ChunkedFifoRecordHandle) fp)->rd_index->self)

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
static unsigned b_get_index_entrysize(NEXUS_ChunkedFifoRecordHandle file)
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
    unsigned index_entrysize = b_get_index_entrysize((NEXUS_ChunkedFifoRecordHandle)fp);

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
    unsigned index_entrysize = b_get_index_entrysize((NEXUS_ChunkedFifoRecordHandle)fp);
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
    unsigned index_entrysize = b_get_index_entrysize((NEXUS_ChunkedFifoRecordHandle)fp);
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
NEXUS_ChunkedFifoPlay_Open(const char *fname, const char *indexname, NEXUS_ChunkedFifoRecordHandle writer)
{
    struct bfile_in_fifo *file;
    NEXUS_Error rc;

    if (fname==NULL) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }
    if (indexname==NULL) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }

    file = BKNI_Malloc(sizeof(*file));
    if (!file) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }
    BKNI_Memset(file, 0, sizeof(*file));

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    bfile_io_posix_touch(fname);
#endif
    file->index = bpvrfifo_read_open(indexname, false, writer?writer->index.file:NULL, NULL, 0);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    if(!file->index)
    	{
                if( writer->rd_index2 != NULL)
		    file->index = bpvrfifo_read_dup(fname, false, writer?writer->index.file:NULL, NULL, 0, & writer->rd_index2->data);
		if (!file->index) { goto err_index;}
    	}
#else
    if (!file->index) { goto err_index;}
#endif
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->index->parent = (void *)file;
	file->index->bfromwrite = false;
#endif
    file->data = bpvrfifo_read_open(fname, true, writer?writer->data.file:NULL, file->index, sizeof(file->index->header->data));
    if (!file->data) { goto err_data;}
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->data->parent = (void *)file;
	file->data->bfromwrite = false;
#endif	
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
    bpvrfifo_read_close(file->data);
err_data:
    bpvrfifo_read_close(file->index);
err_index:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
NEXUS_FilePlayHandle
NEXUS_ChunkedFifoRead_Open(const char *fname, const char *indexname, NEXUS_ChunkedFifoRecordHandle writer, bfile_io_read_t * data, bfile_io_read_t * index)
{
    struct bfile_in_fifo *file;
    NEXUS_Error rc;

    if (fname==NULL) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }
    if (indexname==NULL) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }

    file = BKNI_Malloc(sizeof(*file));
    if (!file) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }
    BKNI_Memset(file, 0, sizeof(*file));


    file->index = bpvrfifo_read_open(indexname, false, writer?writer->index.file:NULL, NULL, 0);
    if (!file->index) { goto err_index;}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->index->parent = (void *)file;
	file->index->bfromwrite = false;
#endif
    file->data = bpvrfifo_read_open(fname, false, writer?writer->data.file:NULL, file->index, sizeof(file->index->header->data));
    if (!file->data) { goto err_data;}
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->data->parent = (void *)file;
	file->data->bfromwrite = false;
#endif	
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
    *index = file->self.file.index;
    *data = file->self.file.data;

    file->self.file.close = bfile_fifo_close_in;

    return &file->self;

err_meta:
    bpvrfifo_read_close(file->data);
err_data:
    bpvrfifo_read_close(file->index);
err_index:
    BKNI_Free(file);
err_alloc:
    return NULL;
}
#else
NEXUS_FilePlayHandle
NEXUS_ChunkedFifoRead_Open(const char *fname, const char *indexname, NEXUS_ChunkedFifoRecordHandle writer, bfile_io_read_t * data, bfile_io_read_t * index)
{
	BSTD_UNUSED(fname);
	BSTD_UNUSED(indexname);
	BSTD_UNUSED(writer);
	BSTD_UNUSED(data);
	BSTD_UNUSED(index);
    return NULL;
}
#endif

static void
bfile_fifo_write_snapshot_locked(struct NEXUS_ChunkedFifoRecord *file)
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
    struct NEXUS_ChunkedFifoRecord *file =  (struct NEXUS_ChunkedFifoRecord *)f;

    if (file->timer) {
        NEXUS_CancelTimer(file->timer);
    }
    if (file->bcm_player) {
        BNAV_Player_Close(file->bcm_player);
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	/*No snapshot writing.*/
    bpvrfifo_read_close(file->rd_index2);
#else
    b_lock(file->index.file);
    bfile_fifo_write_snapshot_locked(file);
    bfile_fifo_write_snapshot_locked(file);
    b_unlock(file->index.file);
#endif
    bpvrfifo_read_close(file->rd_index);
    bpvrfifo_write_close(file->data.file);
    bpvrfifo_write_close(file->index.file);
    BKNI_Free(file);
    return;
}

static void
b_trim_try_player(NEXUS_ChunkedFifoRecordHandle file)
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

/* this functions return true if trim was successful and false otherwise */
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
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)		
            if(rc==-1) desc->trim_count=0;
#endif
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
    NEXUS_ChunkedFifoRecordHandle file = file_;
    unsigned timeout = B_TRIM_TIMER;
    long firstIndex, lastIndex;
    BNAV_Player_Position first, last;
    int rc;
    unsigned long interval;
    NEXUS_Time start, stop;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	bool indextrimres = false;
    struct bfile_io_write_fifo *file2;

	if(file->transition2linearRec >= 3)
	{
		file2 = (struct bfile_io_write_fifo *)(&file->data.file->self);
		file2->switchtolinear = 100;
		file2 = (struct bfile_io_write_fifo *)(&file->index.file->self);
		file2->switchtolinear = 1;	
		file->timer = NULL;
		goto stop_heartbeating_and_trim;
	}
#endif

    NEXUS_Time_Get(&start);
    if(file->cfg.snapshotInterval != 0 && (NEXUS_Time_Diff(&start, &file->lastSnapshotTime)/1000)>=(long)file->cfg.snapshotInterval) {
        b_lock(file->index.file);
        file->index.file->writeSnapshotFile = file; /* we can't write data right there since it could overlap with running write operation and would corrupt the file write pointer */
        b_unlock(file->index.file);
        file->lastSnapshotTime = start;
    }
    if (file->index.trim_count || file->data.trim_count) {
        /* trim index first */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	indextrimres = b_try_trim_file(&file->index);
	if(indextrimres != true)
	{
            timeout = B_TRIM_TRY_TIMER;
            goto done;
        }
#else
        if (!b_try_trim_file(&file->index)) {
            timeout = B_TRIM_TRY_TIMER;
            goto done;
        }
#endif
        if (!b_try_trim_file(&file->data)) {
            timeout = B_TRIM_TRY_TIMER;
            goto done;
        }
        /* this effectively limits rate of succesful trims to one each B_TRIM_TIMER */
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

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	file2 = (struct bfile_io_write_fifo *)(&file->data.file->self);
	BKNI_Memcpy(&file2->first, &first, sizeof(BNAV_Player_Position));
#endif
    BDBG_INSTANCE_MSG(file, ("position %ld:%lu:%lld %ld:%lu:%lld ",
        firstIndex, first.timestamp,B_GET_OFF_T(&first), lastIndex, last.timestamp, B_GET_OFF_T(&last)));

    interval = file->cfg.interval*1000;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)

	if(file->transition2linearRec == 1)
	{
		 long index;
	        off_t off;

			if(file->secfromstart >= 1)
			{
		        index = BNAV_Player_FindIndexFromTimestamp(file->bcm_player, first.timestamp + ((file->secfromstart)*1000));
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
		        off = index*file->index_entrysize;
		        BDBG_MSG(("trimming index old %lld new %lld", (int64_t)file->index.trim_off, (int64_t)off));
		        file->index.trim_off = off;
		        file->index.trim_count = 1;

		        file->index.file->header->trim_gap = (index - file->index.file->header->prev_trim)*file->index_entrysize;
		        file->index.file->header->prev_trim = last.index;

		        off = B_GET_OFF_T(&first);
	#ifdef DIRECT_IO_SUPPORT
		        off -= off % B_DATA_ALIGN;
	#endif
		        BDBG_MSG(("trimming data old %lld new %lld", (int64_t)file->data.trim_off, (int64_t)off));
		        file->data.trim_off = off;
		        file->data.trim_count = 1;
			}
	        timeout = B_TRIM_TRY_TIMER; /* do a trim on the next timer callout */
               file->transition2linearRec = 2;

	}
	else if(file->transition2linearRec == 2)
	{
		timeout = B_TRIM_TRY_TIMER;
		file->transition2linearRec = 3;
	}
	else
	{
#endif
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

        off = B_GET_OFF_T(&first);
#ifdef DIRECT_IO_SUPPORT
        off -= off % B_DATA_ALIGN;
#endif
        BDBG_MSG(("trimming data old %lld new %lld", (int64_t)file->data.trim_off, (int64_t)off));
        file->data.trim_off = off;
        file->data.trim_count = 1;

        timeout = B_TRIM_TRY_TIMER; /* do a trim on the next timer callout */
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
	}
#endif
done:
    file->timer = NEXUS_ScheduleTimer(timeout, b_trim_timer, file);  /* schedule another heartbeat for the trim */
    NEXUS_Time_Get(&stop);
    BDBG_MSG(("trim_timer(%#x) %ldms", (unsigned)file , NEXUS_Time_Diff(&stop, &start)));

    if (!file->timer) {
        BDBG_ERR(("trim heartbeat has died"));
        return;
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
stop_heartbeating_and_trim:
#endif
    return;
}


NEXUS_ChunkedFifoRecordHandle
NEXUS_ChunkedFifoRecord_Create(const char *fname, const char *indexname)
{
    NEXUS_ChunkedFifoRecordHandle file;
    NEXUS_Error rc;

    if (fname==NULL) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }
    if (indexname==NULL) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); return NULL; }

    file = BKNI_Malloc(sizeof(*file));
    if (!file) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_alloc; }

    BKNI_Memset(file, 0, sizeof *file);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->transition2linearRec = 0;
#endif

    /* Check to see if the index already exists */
    /* If the index already exists, then this will be treated as a re-use of the same index and
     * chunk directory.  The index will be opened, and the existing chunks on disk will be deleted.
     *
     * This causes NEXUS_ChunkedFifoRecord_Create() to behave as if O_TRUNC was passed, mimicking
     * the behavior of NEXUS_PosixFileRecord_Create()
     */
    {
        struct bfile_io_read_fifo *rd_index;

        rd_index = bpvrfifo_read_open(indexname, false, NULL, NULL, 0);
        if (rd_index) {
            struct bpvrfifo_header *index_header, *data_header;
            ssize_t rc;
            bool read_error = false;
            int i;

            index_header = BKNI_Malloc(sizeof(*index_header));
            data_header = BKNI_Malloc(sizeof(*data_header));

            if (index_header) {
                b_rseek(&rd_index->data, 0, SEEK_SET);
                rc = b_read(&rd_index->data, &index_header->data, sizeof(index_header->data));
                if (rc!=sizeof(index_header->data) || index_header->data.h.signature.signature[0]!=bpvrfifo_signature.signature[0]) {
                    rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
                    BDBG_MSG(("Index file exists, unable to match signature"));
                    read_error = true;
                }
            }
            else {
                read_error = true;
            }
            if (data_header) {
                b_rseek(&rd_index->data, sizeof(data_header->data), SEEK_SET);
                rc = b_read(&rd_index->data, &data_header->data, sizeof(data_header->data));
                if (rc!=sizeof(data_header->data) || data_header->data.h.signature.signature[0]!=bpvrfifo_signature.signature[0]) {
                    rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
                    BDBG_MSG(("Index file exists, unable to match signature"));
                    read_error = true;
                }
            }
            else {
                read_error = true;
            }

            if (!read_error) {
                for (i=0; i < BFIFO_N_ENTRYS; i++) {
                    if (data_header->data.h.meta.master.chunk_ref_cnt[i] > 0) {
                        b_pvrfifo_make_chunk_name(rd_index->chunk_name, sizeof(rd_index->chunk_name), data_header->data.h.meta.master.chunkTemplate, fname, data_header->data.h.meta.master.chunk_no[i]);
                        BDBG_MSG(("Removing '%s'",rd_index->chunk_name));
                        unlink(rd_index->chunk_name);
                    }
                }
            }
            bpvrfifo_read_close(rd_index);
            if (index_header) {
                BKNI_Free(index_header);
            }
            if (data_header) {
                BKNI_Free(data_header);
            }
        }
    }
    /* end of O_TRUNC behavior */

    /* Continue with chunked fifo record creation */
    file->cfg.interval = 30;
    file->cfg.snapshotInterval = 0;
    file->cfg.data.soft = 512*1024*1024;
    file->cfg.data.hard = 1024*1024*1024;
    file->cfg.data.soft -= file->cfg.data.soft % B_DATA_ALIGN;
    file->cfg.data.hard -= file->cfg.data.hard % B_DATA_ALIGN;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->cfg.index.soft = 200*1024*1024;
    file->cfg.index.hard = 400*1024*1024;
#else
    file->cfg.index.soft = 4*1024*1024;
    file->cfg.index.hard = 8*1024*1024;
#endif

    file->cfg.index.chunkSize = 0;
    BKNI_Memcpy(file->cfg.chunkTemplate, b_file_chunkTemplate, sizeof(b_file_chunkTemplate));

    NEXUS_Time_Get(&file->lastSnapshotTime);

    file->index.trim_count = 0;
    file->index.file = bpvrfifo_write_open(indexname, false, 2*sizeof(file->data.file->header->data), NULL, 0);
    if (!file->index.file) { goto err_index;}
    file->index.file->size = file->cfg.index.soft;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->index.file->parent = (void *)file;
#endif
    /* Do not set O_DIRECT flag for index */
    file->rd_index = bpvrfifo_read_open(indexname, false, file->index.file, NULL, 0);
    if (!file->rd_index) { goto err_rd_index;}
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->rd_index->parent = (void *)file;
	file->rd_index->bfromwrite = true;
    file->rd_index2 = bpvrfifo_read_open(indexname, false, file->index.file, NULL, 0);
    if (!file->rd_index2) { goto err_rd_index2;}	
#endif
    file->data.trim_count = 0;
    /* we save meta data in the index file, it's used because data usually is O_DIRECT, e.g. meta writes are slower and it puts constraints on the alligment of in memory meta buffer */
    file->data.file = bpvrfifo_write_open(fname, true, 0, file->index.file, sizeof(file->index.file->header->data));
    if (!file->data.file) { goto err_data;}
    file->data.file->size = file->cfg.data.soft;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    file->data.file->parent = (void *)file;
#endif
    file->self.index = bpvrfifo_write_file(file->index.file);
    file->self.data = bpvrfifo_write_file(file->data.file);

    file->self.close = bfile_fifo_close_out;

    file->index_entrysize = 0; /* we don't know until first NAV entry is written. use b_get_index_entrysize. */
    file->bcm_player = NULL;

    file->index.trim_count = file->data.trim_count = 0;
    file->index.trim_off = file->data.trim_off = 0;

    /* timer shall be the last call in the initialization sequence */
    file->timer = NEXUS_ScheduleTimer(B_TRIM_TIMER, b_trim_timer, file);  /* schedule heartbeat for the trim */
    if (!file->timer) { goto err_timer; }

    return file;

err_timer:
    bpvrfifo_write_close(file->data.file);
err_data:
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)	
    bpvrfifo_read_close(file->rd_index2);
err_rd_index2:
#endif
    bpvrfifo_read_close(file->rd_index);
err_rd_index:
    bpvrfifo_write_close(file->index.file);
err_index:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

NEXUS_Error
bfile_io_write_set(struct bfile_io_write_fifo *file, const NEXUS_ChunkedFifoRecordLimit *cfg, const char *template)
{
    BDBG_ASSERT(file);
    BDBG_ASSERT(cfg);

    if(file->header->active && (file->header->chunked != (cfg->chunkSize!=0))) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    for(;;) {
        b_lock(file);
        if(file->header->busy==0) {
            break;
        }
        b_unlock(file);
        BDBG_WRN(("bfile_io_write_fifo: %#lx collision in access to the file", (unsigned long)file));
        BKNI_Sleep(1);
    }

    file->header->data.h.type = cfg->chunkSize != 0 ? bpvrfifo_type_master : bpvrfifo_type_unit;
    if(!file->header->chunked && cfg->chunkSize != 0) {
        B_ENTRY(file, 0)->doff = 0; /* data in chunk always starts from 0 */
        B_ENTRY(file, 0)->no = file->next_chunk;
        b_pvrfifo_register_new_chunk(file);
    }

    file->header->chunked = cfg->chunkSize != 0;
    if(template) {
        b_file_strcpy(file->header->data.h.meta.master.chunkTemplate, sizeof(file->header->data.h.meta.master.chunkTemplate), template);
    }
    if(file->header->chunked) {
        file->size = cfg->chunkSize;
    } else {
        file->size = cfg->soft;
    }
    b_unlock(file);
    return NEXUS_SUCCESS;
}


NEXUS_FileRecordHandle
NEXUS_ChunkedFifoRecord_GetFile(NEXUS_ChunkedFifoRecordHandle file)
{
    return &file->self;
}

NEXUS_Error
NEXUS_ChunkedFifoRecord_GetPosition(NEXUS_ChunkedFifoRecordHandle file, NEXUS_FilePosition *first, NEXUS_FilePosition *last)
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
NEXUS_ChunkedFifoRecord_GetSettings(NEXUS_ChunkedFifoRecordHandle file, NEXUS_ChunkedFifoRecordSettings *settings)
{
    /* settings are read only, so wa don't need synchronization here */
    *settings = file->cfg;
    return;
}


NEXUS_Error
NEXUS_ChunkedFifoRecord_SetSettings(NEXUS_ChunkedFifoRecordHandle file, const NEXUS_ChunkedFifoRecordSettings *settings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
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
    file->cfg.data.chunkSize -= file->cfg.data.chunkSize % B_DATA_ALIGN;
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

    rc = bfile_io_write_set(file->index.file, &file->cfg.index, settings->chunkTemplate);
    if(rc!=NEXUS_SUCCESS) {
        goto err_index;
    }

    rc = bfile_io_write_set(file->data.file, &file->cfg.data, settings->chunkTemplate);
    if(rc!=NEXUS_SUCCESS) {
        goto err_data;
    }

    return NEXUS_SUCCESS;
err_data:
err_index:
    return rc;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
void
/*NEXUS_ChunkedFifoRecord_StopTrimming(NEXUS_ChunkedFifoRecordHandle file, int timeinsec)*/
NEXUS_ChunkedFifoRecord_StopTrimming(NEXUS_ChunkedFifoRecordHandle file, NEXUS_ChunkedDelayedRecordInfo *delayedRecInfo)
{
    NEXUS_Error rc;
    char *file0 = NULL;
    struct bfile_io_write_fifo *file2; 
    struct bfile_io_write_posix wrFd;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FILE_PERMISSION)
    mode_t umask_now;
#endif

    /* settings are read only, so we don't need synchronization here */
    mkdir(delayedRecInfo->delayedRecDestDir);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FILE_PERMISSION)
    chown(delayedRecInfo->delayedRecDestDir, OWNER, GROUP);
    chmod(delayedRecInfo->delayedRecDestDir, PERM);
#endif
    file0 = BKNI_Malloc(FILE_NAME_SIZE);
    if(file0 == NULL)
    {
	BDBG_ERR(("malloc error."));
    }
    else
    {
        BKNI_Memset(file0, 0x0, FILE_NAME_SIZE);
	 BKNI_Memcpy(file0, delayedRecInfo->delayedRecDestDataFile, strlen(delayedRecInfo->delayedRecDestDataFile)+1);
	 strncat(file0, ".0\0", 3);
#if 0
        rc = bfile_io_write_posix_open(&wrFd, file0 , 0);		
        if(rc)
        {
            BDBG_ERR(("[read] can't create file %s", file0));
        }
        else
    	{
            bfile_io_write_posix_close(&wrFd);
    	}
#else
		rc = bfile_io_posix_touch(file0);
		if(rc != NEXUS_SUCCESS)
		{
			BDBG_ERR(("[posix_touch] 2nd try."));
			rc = bfile_io_posix_touch(file0);
			if(rc != NEXUS_SUCCESS)
				BDBG_ERR(("[posix_touch] 2nd try also failed."));
		}
#endif
	BKNI_Free(file0);
    }
	
    data_copy_completed = 0;
    file->secfromstart = (unsigned)delayedRecInfo->timeinsec;
    file2 = (struct bfile_io_write_fifo *)(&file->data.file->self);
    BKNI_Memcpy(file2->file_name_old, file2->file_name, FILE_NAME_SIZE);
    BKNI_Memset(file2->file_name, 0x0, FILE_NAME_SIZE);
    BKNI_Memcpy(file2->file_name, delayedRecInfo->delayedRecDestDataFile, strlen(delayedRecInfo->delayedRecDestDataFile)+1);
    /*le2->switchtolinear = 1;*/
    file2 = (struct bfile_io_write_fifo *)(&file->index.file->self);
    BKNI_Memcpy(file2->file_name_old, file2->file_name, FILE_NAME_SIZE);
    BKNI_Memset(file2->file_name, 0x0, FILE_NAME_SIZE);
    BKNI_Memcpy(file2->file_name, delayedRecInfo->delayedRecDestIndexFile, strlen(delayedRecInfo->delayedRecDestIndexFile)+1);	
    /*file2->switchtolinear = 1;*/
    file->transition2linearRec = 1;

    return;
}

NEXUS_Error
NEXUS_ChunkedFifoRecord_DelayedIndexCopy_Init(NEXUS_ChunkedFifoRecordHandle file)
{
    NEXUS_Error rc;
    struct bfile_io_write_fifo *fifoInfo;
    fifoInfo = (struct bfile_io_write_fifo *)(&file->index.file->self);

    if(fifoInfo->switchtolinear == 1)
    {
        if(fifoInfo ->idxnotswitchedyet == 1)
        {
            rc = bfile_io_read_posix_open(&file->old_idx_read, fifoInfo->file_name_old , fifoInfo->direct);		
            if(rc)
            {
            	if(file->rd_index2 != NULL)
            	{
	                rc = bfile_io_read_posix_dup(&file->old_idx_read, &file->rd_index2->data , fifoInfo->direct);
		        if(rc)
		        {
		                BDBG_ERR(("[read] can't open,dup file %s", fifoInfo->file_name_old));
		                return NEXUS_UNKNOWN;
		        }
                 }
                 else
                 {
	                BDBG_ERR(("[read] can't open file %s", fifoInfo->file_name_old));
                        return NEXUS_UNKNOWN;               
                 }
            }
            rc = bfile_io_write_posix_openappend(&file->new_idx_write, fifoInfo->file_name, fifoInfo->direct);	
            if(rc)
            {
                BDBG_ERR(("[write] can't open file %s", fifoInfo->file_name ));
                bfile_io_read_posix_close(&file->old_idx_read);
                return NEXUS_UNKNOWN;
            }
            fifoInfo->idxnotswitchedyet = 2;
            cur_pos = 0;
            return NEXUS_SUCCESS;
        }
    }
    else
    {
    	/*printf("still in fifo mode.\n");*/
    }

	return NEXUS_NOT_AVAILABLE;
}

int
NEXUS_ChunkedFifoRecord_CopyData(NEXUS_ChunkedFifoRecordHandle file)
{
    off_t rc_off;
    ssize_t rc;
    off_t copylen;
    struct bfile_io_write_fifo *writefifo;
    struct bfile_io_read_fifo *readfifo;
    writefifo = (struct bfile_io_write_fifo *)(&file->index.file->self);
    readfifo = (struct bfile_io_read_fifo *)(&file->rd_index->self);

	if(cur_pos == 0)
	{
		cur_pos = readfifo->index_first;
		data_written = 0;
		wrap_around_point_passed = 0;
	}

    if(wrap_around_point_passed < 2)
	{
    	if(readfifo->index_last < readfifo->index_first) /*wrap around*/
    	{	
    		if(wrap_around_point_passed == 0)
    		{
    			if(cur_pos < readfifo->file_end_limit)
    			{
    				copylen = readfifo->file_end_limit -cur_pos;
           			if(copylen > IDX_COPY_CHUNK_SIZE)
    				{
           					copylen = IDX_COPY_CHUNK_SIZE;
    				}						
    				rc_off =b_rseek(&file->old_idx_read, cur_pos, SEEK_SET);
    				if(rc_off != cur_pos)
    				{
    
    				}
    				rc = b_read(&file->old_idx_read, index_buffer, copylen);
    				if(rc != copylen)
    				{
    
    				}
    				rc = b_write(&file->new_idx_write, index_buffer, copylen);
    				if(rc != copylen)
    				{
    
    				}
    				cur_pos += copylen;
    				data_written +=copylen;
    
    			}
    			else
    			{
    				cur_pos = readfifo->file_start_limit;
    				wrap_around_point_passed = 1;
    			}
    		}
    		else if(wrap_around_point_passed == 1)
    		{
    			if(cur_pos < readfifo->index_last)
    			{
    				copylen = readfifo->index_last -cur_pos;
           			if(copylen > IDX_COPY_CHUNK_SIZE)
    				{
           					copylen = IDX_COPY_CHUNK_SIZE;
    				}						
    				rc_off =b_rseek(&file->old_idx_read, cur_pos, SEEK_SET);
    				if(rc_off != cur_pos)
    				{
    
    				}
    				rc = b_read(&file->old_idx_read, index_buffer, copylen);
    				if(rc != copylen)
    				{
    
    				}
    				rc = b_write(&file->new_idx_write, index_buffer, copylen);
    				if(rc != copylen)
    				{
    
    				}
    				cur_pos += copylen;
    				data_written +=copylen;
    			}
    			else
    			{
    				wrap_around_point_passed = 2;
    			}					
    		}
    		else
    		{
    
    		}
    	}
    	else /*No wrap around*/
    	{
    		if(data_written < file->cur_idx_off)
    		{
    			copylen = (file->cur_idx_off - data_written);
           		if(copylen > IDX_COPY_CHUNK_SIZE)
           		{
           			copylen = IDX_COPY_CHUNK_SIZE;
           		}
    			rc_off =b_rseek(&file->old_idx_read, cur_pos, SEEK_SET);
    			if(rc_off != cur_pos)
    			{
    
    			}
    			rc = b_read(&file->old_idx_read, index_buffer, copylen);
    			if(rc != copylen)
    			{
    
    			}
    			rc = b_write(&file->new_idx_write, index_buffer, copylen);
    			if(rc != copylen)
    			{
    
    			}
    			cur_pos += copylen;						
    			data_written +=copylen;
    		}
    		else
    		{
    			wrap_around_point_passed = 2;
    		}		
    	}
	}
	if(wrap_around_point_passed == 2 && data_copy_completed == 1)
	{
		return 0;
	}
	else
	{
		return -1;
	}

}

NEXUS_Error
NEXUS_ChunkedFifoRecord_DelayedIndexCopy_Uninit(NEXUS_ChunkedFifoRecordHandle file)
{
	bfile_io_read_posix_close(&file->old_idx_read);
	bfile_io_write_posix_close(&file->new_idx_write);
	cur_pos = 0;
	data_copy_completed = 0;
	
	return NEXUS_SUCCESS;
}
#else
void
NEXUS_ChunkedFifoRecord_StopTrimming(NEXUS_ChunkedFifoRecordHandle file, NEXUS_ChunkedDelayedRecordInfo *delayedRecInfo)
{
	BSTD_UNUSED(file);
	BSTD_UNUSED(delayedRecInfo);
    return;
}

NEXUS_Error
NEXUS_ChunkedFifoRecord_DelayedIndexCopy_Init(NEXUS_ChunkedFifoRecordHandle file)
{
	BSTD_UNUSED(file);
	return NEXUS_SUCCESS;
}

int
NEXUS_ChunkedFifoRecord_CopyData(NEXUS_ChunkedFifoRecordHandle file)
{
	BSTD_UNUSED(file);
	return 0;
}

NEXUS_Error
NEXUS_ChunkedFifoRecord_DelayedIndexCopy_Uninit(NEXUS_ChunkedFifoRecordHandle file)
{
	BSTD_UNUSED(file);
	return NEXUS_SUCCESS;
}
#endif

