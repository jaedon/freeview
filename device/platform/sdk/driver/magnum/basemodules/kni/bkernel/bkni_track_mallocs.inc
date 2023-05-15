/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_track_mallocs.inc $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 6/6/11 3:27p $
 *
 * Module Description:
 *
 * Wrapper over host malloc and free routines
 *
 * Revision History:
 * $brcm_Log: /magnum/basemodules/kni/linuxuser/bkni_track_mallocs.inc $ *
 * 
 * Hydra_Software_Devel/10   6/6/11 3:27p vsilyaev
 * SW7405-4477: Routed all debug output through buffer and use external
 * application to extract and print debug output
 * 
 * Hydra_Software_Devel/9   8/3/10 12:03p erickson
 * SW7400-2857: refactor BKNI_P_GetTrackAllocEntry into separate read-only
 * and read-write functions. this limits worst-case stack usage in
 * BDBG_OBJECT_ASSERT.
 *
 * Hydra_Software_Devel/8   2/5/10 3:24p erickson
 * SW7405-2397: NULL freed pointer
 *
 * Hydra_Software_Devel/7   5/15/09 12:15p jtna
 * PR54398: redefine B_TRACK_ALLOC_LOCK for linuxuser and remove return
 * value check
 *
 * Hydra_Software_Devel/6   4/30/09 4:52p erickson
 * PR54759: fix memleak on error path
 *
 * Hydra_Software_Devel/5   4/21/09 10:49a erickson
 * PR54398: fix warnings
 *
 * Hydra_Software_Devel/4   4/20/09 12:40p jtna
 * PR53778: update for linuxkernel
 *
 * Hydra_Software_Devel/3   4/17/09 6:30p vsilyaev
 * PR 53778: Added description of expected inputs
 *
 * Hydra_Software_Devel/2   4/17/09 6:24p vsilyaev
 * PR 53778: Filter results printed on the table resizing
 *
 * Hydra_Software_Devel/1   4/17/09 5:39p vsilyaev
 * PR 53778: Wrapper over host malloc and free routines
 *
 * $old_brcm_Log: /magnum/basemodules/kni/linuxuser/bkni.c $ *
 *
 * Hydra_Software_Devel/66   4/17/09 5:07p vsilyaev
 * PR 53778: Try to detect double-free blocks and print location where
 * they were allocated and freed
 *
 * Hydra_Software_Devel/65   4/17/09 4:08p vsilyaev
 * PR 53778: Fixed detection of double-free'd blocks
 *
 ***************************************************************************/

/*
   This "macro" expects the following defines as it input:
    B_TRACK_ALLOC_LOCK() - code to acquire lock
    B_TRACK_ALLOC_UNLOCK() - code to release lock
    B_TRACK_ALLOC_ALLOC(size)  - allocates block of the system memory
    B_TRACK_ALLOC_FREE(ptr)  - frees allocated block
    B_TRACK_ALLOC_OS  - string that reflect name of the OS, it's only used in the debug output
*/

#if BKNI_TRACK_MALLOCS
#define  BKNI_GARBLE_MALLOC 1

struct BKNI_TrackAllocEntry {
    const void *mem;
    size_t size;
    const char *file;
    unsigned line;
};

struct BKNI_P_AllocUser {
    unsigned count;
    unsigned size;
    const char *file;
    unsigned line;
};

static struct {
    size_t allocated, freed, peak;
    unsigned allocated_cnt, freed_cnt;
    unsigned table_size;
    struct BKNI_TrackAllocEntry *table;
    struct {
        struct BKNI_TrackAllocEntry alloc;
        struct {
            const char *file;
            unsigned line;
        } free;
    } history[256]; /* FIFO of last freed objects */
    struct BKNI_P_AllocUser alloc_users[16]; /* temporary array used to account frequent users in the event of table resizing */
} g_alloc_state;

static void
b_memset32(void *ptr, size_t size, uint32_t word)
{
   unsigned i;

   if(ptr) {
       for(i=0; i+3 < size; i+=4) {
          *(uint32_t*)(((uint8_t *)ptr)+i)=word;
       }
   }
   return;
}

static const char *
b_shorten_filename(const char *pFileName)
{
    const char *s;
    unsigned i;

    if(pFileName==NULL) {
        return "unknown";
    }
    for(s=pFileName;*s != '\0';s++) { } /* search forward */

    for(i=0;s!=pFileName;s--) { /* search backward */
        if(*s=='/' || *s=='\\') {
            i++;
            if(i>4) {
                return s+1;
            }
        }
    }
    return pFileName;
}


static unsigned
b_alloc_hashkey(const void *mem)
{
    unsigned long hash = (unsigned long)mem;
    /* calculate a hash */
    hash = (hash ^ 61) ^ (hash>> 16);
    hash += (hash << 3);
    hash ^= (hash >> 4);
    hash *= 0x66635119; /* 1717784857  some prime number */
    hash ^= (hash >> 15);
    return hash;
}

static struct BKNI_TrackAllocEntry *
BKNI_P_GetTrackAllocEntry_one(const void *mem, const void *match, struct BKNI_TrackAllocEntry *entries, unsigned table_size)
{
    unsigned i;
    unsigned index;
    for(index=i=b_alloc_hashkey(mem)%table_size;i<2*table_size;i++,index++) {
        if(index>=table_size) {
            index = 0;
        }
        if(entries[index].mem==match) {
#if 0
            /* some profiling code to measure effectivness of the hash lookup */
            static unsigned max_count=0;
            static unsigned total_count=0;
            unsigned count = i-(b_alloc_hashkey(mem)%table_size);
            total_count+=count+1;
            if(count>max_count) {
                unsigned avg = (100*total_count)/(g_alloc_state.allocated_cnt+g_alloc_state.freed_cnt+1);
                max_count=count;
                BDBG_P_PrintString("BKNI_GetTrackAllocEntry: scan_count:%u(%u.%02u avg) addr:%#lx key:%u elements:%u(%u)\n", count,  avg/100, avg%100, (unsigned long)mem, (b_alloc_hashkey(mem)%table_size), g_alloc_state.allocated_cnt-g_alloc_state.freed_cnt, table_size);
            }
#endif
            return  entries+index;
        }
    }
    return NULL;
}

static struct BKNI_TrackAllocEntry *
BKNI_P_GetTrackAllocEntry_resize(const struct BKNI_TrackAllocEntry *entries, unsigned table_size, unsigned new_table_size)
{
    unsigned i;
    struct BKNI_TrackAllocEntry *new_entries;

    new_entries = B_TRACK_ALLOC_ALLOC(new_table_size*sizeof(*new_entries));
    if(new_entries==NULL) { goto error;}

    for(i=0;i<new_table_size;i++) {
        new_entries[i].mem = NULL;
        new_entries[i].size = 0;
        new_entries[i].file = NULL;
        new_entries[i].line = 0;
    }
    if(entries) {
        struct BKNI_P_AllocUser *alloc_users = g_alloc_state.alloc_users;
        const unsigned alloc_users_size = sizeof(g_alloc_state.alloc_users)/sizeof(g_alloc_state.alloc_users[0]);

        BDBG_P_PrintString("BKNI_P_GetTrackAllocEntry_resize: resizing from %u->%u\n", table_size, new_table_size);
        b_memset32(alloc_users, sizeof(*alloc_users)*alloc_users_size, 0);
        for(i=0;i<table_size;i++) {
            if(entries[i].mem) {
                struct BKNI_TrackAllocEntry *entry = BKNI_P_GetTrackAllocEntry_one(entries[i].mem, NULL, new_entries, new_table_size);
                unsigned j;
                if(!entry) {
                    goto error;
                }
                *entry = entries[i];
                for(j=0;j<alloc_users_size;j++) {
                    if(alloc_users[j].count==0) {
                        /* found empty slot, just use it */
                        alloc_users[j].count = 1;
                        alloc_users[j].size = entry->size;
                        alloc_users[j].file = entry->file;
                        alloc_users[j].line = entry->line;
                        break;
                    } else if(alloc_users[j].file == entry->file && alloc_users[j].line == entry->line) {
                        /* found existing entry, update it and move it up to keep array sorted */
                        alloc_users[j].count++;
                        alloc_users[j].size += entry->size;
                        for(;j>0;j--) {
                            struct BKNI_P_AllocUser tmp;
                            if(alloc_users[j].count <= alloc_users[j-1].count) {
                                break;
                            }
                            tmp = alloc_users[j];
                            alloc_users[j] = alloc_users[j-1];
                            alloc_users[j-1] = tmp;
                        }
                        break;
                    }
                }
                if(j==alloc_users_size) { /* no empty slots */
                    j=alloc_users_size/2; /* wipe out half of old entries */
                    alloc_users[j].count = 1;
                    alloc_users[j].size = entry->size;
                    alloc_users[j].file = entry->file;
                    alloc_users[j].line = entry->line;
                    for(j=j+1;j<alloc_users_size;j++) {
                        alloc_users[j].count = 0;
                        alloc_users[j].size = 0;
                        alloc_users[j].file = 0;
                        alloc_users[j].line = 0;
                    }
                }
            }
        }
        BDBG_P_PrintString("BKNI_Malloc(%s) top users:\n%10s, %7s, filename:line\n", B_TRACK_ALLOC_OS,  "blocks", "bytes");
        for(i=0;i<alloc_users_size;i++) {
            if(alloc_users[i].count==0) {
                break;
            }
            if(alloc_users[i].count>=table_size/16) {
                BDBG_P_PrintString("%#10u, %7u, %s:%u\n", alloc_users[i].count, alloc_users[i].size,b_shorten_filename(alloc_users[i].file), alloc_users[i].line);
            }
        }
    }
    return new_entries;
error:
    if (new_entries) {
        B_TRACK_ALLOC_FREE(new_entries);
    }
    BDBG_P_PrintString("BKNI_P_GetTrackAllocEntry_resize: can't reallocate alloc table %u->%u\n", table_size, new_table_size);
    return NULL;
}

/* this performs a lookup for the pointer. if it already exists, it is reused. otherwise, it is created.
if the existing table is filled, a larger table is created. */
static struct BKNI_TrackAllocEntry *
BKNI_P_CreateTrackAllocEntry(const void *mem)
{
    struct BKNI_TrackAllocEntry *entries = g_alloc_state.table;
    struct BKNI_TrackAllocEntry *entry=NULL;
    if(entries) {
        unsigned table_size =  g_alloc_state.table_size;
        entry = BKNI_P_GetTrackAllocEntry_one(mem, NULL, entries, table_size);
        if(!entry) { /* if were looking for the empty entry */
            unsigned new_table_size =  2*table_size;
            struct BKNI_TrackAllocEntry *new_entries = BKNI_P_GetTrackAllocEntry_resize(entries, table_size, new_table_size);
            B_TRACK_ALLOC_FREE(entries);
            entries = new_entries;
            g_alloc_state.table = new_entries;
            if(new_entries) {
                g_alloc_state.table_size = new_table_size;
                entry = BKNI_P_GetTrackAllocEntry_one(mem, NULL, entries, new_table_size);
            } else {
                g_alloc_state.table_size = 0;
            }
        }
    }
    return entry;
}

/* this is a read-only lookup. if the entry does not exist, it will return NULL. */
static struct BKNI_TrackAllocEntry *
BKNI_P_GetTrackAllocEntry(const void *mem)
{
    struct BKNI_TrackAllocEntry *entries = g_alloc_state.table;
    struct BKNI_TrackAllocEntry *entry = NULL;
    if(entries) {
        unsigned table_size = g_alloc_state.table_size;
        entry = BKNI_P_GetTrackAllocEntry_one(mem, mem, entries, table_size);
    }
    return entry;
}


void *
BKNI_Malloc_tagged(size_t size, const char *file, unsigned line)
{
    void *mem;
    ASSERT_NOT_CRITICAL();

#ifdef __KERNEL__
#ifdef BKNI_METRICS_ENABLED
    g_metrics.totalMemoryAllocated += size;
    g_metrics.totalMallocs++;
#endif
#endif
    mem = B_TRACK_ALLOC_ALLOC(size);

    if(mem) {
        struct BKNI_TrackAllocEntry *entry;
        size_t used;
#if BKNI_GARBLE_MALLOC
        b_memset32(mem, size, 0xDEADDA7A);
#endif
        B_TRACK_ALLOC_LOCK();
        entry = BKNI_P_CreateTrackAllocEntry(mem);
        if(entry) {
            entry->mem = mem;
            entry->size = size;
            entry->file = file;
            entry->line = line;
        }
        g_alloc_state.allocated += size;
        g_alloc_state.allocated_cnt ++;
        used = g_alloc_state.allocated - g_alloc_state.freed;
        if(used>g_alloc_state.peak) {
            g_alloc_state.peak = used;
        }
        B_TRACK_ALLOC_UNLOCK();
    } else {
        /*
        BKNI_DumpMallocs_Size(0);
        */
        BDBG_P_PrintString("BKNI_Alloc(%u): returned NULL at %s:%u\n", size, b_shorten_filename(file), line);
        BKNI_Fail();
    }
    return mem;
}

void
BKNI_Free_tagged(void *ptr, const char *file, unsigned line)
{
    ASSERT_NOT_CRITICAL();

    if(ptr) {
        struct BKNI_TrackAllocEntry *entry;
        size_t size=0;
        const unsigned history_size = sizeof(g_alloc_state.history)/sizeof(g_alloc_state.history[0]);
        B_TRACK_ALLOC_LOCK();
        entry = BKNI_P_GetTrackAllocEntry(ptr);
        if(entry) {
            unsigned history_index=g_alloc_state.freed_cnt%history_size;
            g_alloc_state.history[history_index].alloc = *entry;
            g_alloc_state.history[history_index].free.file = file;
            g_alloc_state.history[history_index].free.line = line;
            g_alloc_state.freed += entry->size;
            g_alloc_state.freed_cnt++;
            size = entry->size;
            entry->mem = NULL;
        } else if(g_alloc_state.table!=NULL) {
            unsigned i;
            BDBG_P_PrintString("BKNI_Free of unknown ptr: %#lx, %s:%u\n", (unsigned long)ptr, b_shorten_filename(file), line);
            for(i=0;i<history_size;i++) {
                if(g_alloc_state.history[i].alloc.mem == ptr) {
                    BDBG_P_PrintString("ptr: %#lx was previously allocated at %s:%u and freed at %s:%u\n", (unsigned long)ptr, b_shorten_filename(g_alloc_state.history[i].alloc.file), g_alloc_state.history[i].alloc.line, b_shorten_filename(g_alloc_state.history[i].free.file), g_alloc_state.history[i].free.line);
                }
            }
            BKNI_Fail();
        }
        B_TRACK_ALLOC_UNLOCK();
#if BKNI_GARBLE_MALLOC
        b_memset32(ptr, size, 0xDA7ADEAD);
#endif
    } else {
        BDBG_P_PrintString("BKNI_Free of NULL ptr: %#lx, %s:%u\n", (unsigned long)ptr, b_shorten_filename(file), line);
        BKNI_Fail();
    }
    B_TRACK_ALLOC_FREE(ptr);
    return;
}



#define BKNI_DUMP_SIZE_THRESHOLD        (1024)

static void
BKNI_DumpMallocs_Size(size_t threshold)
{
    unsigned i;
    bool header;
    struct BKNI_TrackAllocEntry *entries = g_alloc_state.table;

    B_TRACK_ALLOC_LOCK();
    for (header=false,i=0;entries && i<g_alloc_state.table_size;i++) {
        if ((entries[i].mem) &&
            (entries[i].size > threshold)) {
            if(!header) {
                header=true;
                BDBG_P_PrintString("BKNI_Malloc(%s) report:\n%10s, %7s, filename:line\n", B_TRACK_ALLOC_OS, "address", "size");
            }
            BDBG_P_PrintString("%#10lx, %7u, %s:%u\n",
                (unsigned long)entries[i].mem,
                entries[i].size,
                b_shorten_filename(entries[i].file),
                entries[i].line);
        }
    }
    B_TRACK_ALLOC_UNLOCK();
    BDBG_P_PrintString("BKNI_Malloc(%s): allocated:%u(%u) freed:%u(%u) peak:%u\n", B_TRACK_ALLOC_OS, g_alloc_state.allocated, g_alloc_state.allocated_cnt, g_alloc_state.freed, g_alloc_state.freed_cnt, g_alloc_state.peak);
    return;
}

void
BKNI_DumpMallocs(void)
{
    BKNI_DumpMallocs_Size(BKNI_DUMP_SIZE_THRESHOLD);
}

BERR_Code
BKNI_GetMallocEntryInfo( const void *mem, struct BKNI_MallocEntryInfo *entry)
{
    const struct BKNI_TrackAllocEntry *allocated;

    entry->free_file = NULL;
    entry->free_line = 0;
    entry->alive = true;
    B_TRACK_ALLOC_LOCK();
    allocated = BKNI_P_GetTrackAllocEntry(mem);
    if(!allocated) {
        unsigned i;
        for(i=0;i<sizeof(g_alloc_state.history)/sizeof(g_alloc_state.history[0]);i++) {
            if(g_alloc_state.history[i].alloc.mem == mem) {
                allocated = &g_alloc_state.history[i].alloc;
                entry->free_file = g_alloc_state.history[i].free.file;
                entry->free_line = g_alloc_state.history[i].free.line;
                entry->alive = false;
                break;
            }
        }
    }
    if(allocated) {
        entry->mem = allocated->mem;
        entry->size = allocated->size;
        entry->malloc_file = allocated->file;
        entry->malloc_line = allocated->line;
    }
    B_TRACK_ALLOC_UNLOCK();
    if(allocated) {
        entry->malloc_file = b_shorten_filename(entry->malloc_file);
        entry->free_file = b_shorten_filename(entry->free_file);
        return BERR_SUCCESS;
    }
    return BERR_NOT_SUPPORTED;
}

#undef BKNI_Malloc
void *
BKNI_Malloc(size_t size)
{
    return BKNI_Malloc_tagged(size, NULL, 0);
}

#undef BKNI_Free
void
BKNI_Free(void *ptr)
{
    BKNI_Free_tagged(ptr, NULL, 0);
    return;
}

static void
BKNI_P_TrackAlloc_Init(void)
{
    b_memset32(&g_alloc_state, sizeof(g_alloc_state), 0);
    g_alloc_state.table_size = 4096;
    g_alloc_state.table = BKNI_P_GetTrackAllocEntry_resize(NULL, 0, g_alloc_state.table_size);
    return;
}

static void
BKNI_P_TrackAlloc_Uninit(void)
{
    BKNI_DumpMallocs_Size(0);
    if(g_alloc_state.table) {
        B_TRACK_ALLOC_FREE(g_alloc_state.table);
        g_alloc_state.table = NULL;
    }
    return;
}

#else /* BKNI_TRACK_MALLOCS */

void *
BKNI_Malloc(size_t size)
{
    void *ptr;
    ASSERT_NOT_CRITICAL();
    ptr = B_TRACK_ALLOC_ALLOC(size);
    return ptr;
}

void
BKNI_Free(void *ptr)
{
    ASSERT_NOT_CRITICAL();
    B_TRACK_ALLOC_FREE(ptr);
    return;
}

void *
BKNI_Malloc_tagged(size_t size, const char *file, unsigned line)
{
    BSTD_UNUSED(file);
    BSTD_UNUSED(line);
    return BKNI_Malloc(size);
}

void
BKNI_Free_tagged(void *ptr, const char *file, unsigned line)
{
    BSTD_UNUSED(file);
    BSTD_UNUSED(line);
    BKNI_Free(ptr);
    return;
}

BERR_Code BKNI_GetMallocEntryInfo( const void *mem, struct BKNI_MallocEntryInfo *entry)
{
    BSTD_UNUSED(mem);
    BSTD_UNUSED(entry);
    return BERR_NOT_SUPPORTED;
}

static void
BKNI_P_TrackAlloc_Init(void)
{
    return;
}

static void
BKNI_P_TrackAlloc_Uninit(void)
{
    return;
}

void
BKNI_DumpMallocs(void)
{
    return;
}

#endif /* BKNI_TRACK_MALLOCS */

