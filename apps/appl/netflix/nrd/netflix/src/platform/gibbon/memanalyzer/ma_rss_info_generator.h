/* (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 U.S. copyright and other applicable intellectual property laws and
 may not be copied without the express permission of Netflix, Inc.,
 which reserves all rights.  Reuse of any of this content for any
 purpose without the permission of Netflix, Inc. is strictly
 prohibited.
*/

#ifndef MA_RSS_INFO_GENERATOR_H_
#define MA_RSS_INFO_GENERATOR_H_


// **************************************************************
// Environment Variables
// MA_PORT (1234)               : Which port to use for telnet
// MA_LINES_IN_SMAPS (14)       : Number of lines in between maps line inside of smaps
// MA_REPORT_PAGE_NUM_TH (2)    : RSS that is bigger than this value x page_size (4K) will be reported
// MA_MALLOC_SIZE_TH (10240)    : Backtrace will be generated for the malloc with the size > this threshold (anything less than 8192 is recommended)
// MA_KEEP_LD_PRELOAD (not set) : Set this to something if you would like to keep LD_PRELOAD setting
// ************

//#define ONLY_SUPPORT_TOTAL_SMAPS
#define ENABLE_RB_TREE
#define DEMANGLE_CALLSTACK
#define READ_SMAPS_DIRECTLY  // somehow enabling this will reduced the unreported memory usage

#define RECORD_FREE // Enabling this will can take up lots of CPU cycles (>40% in Xeon 2GHz machine)
                      // Very interesting findings (not confirmed):
                      // When I enable this, I see more DFB usages and Others(unreported) to be negative... (probably a bug in my code)
                      // Initially, by disabling this I thought that the "freed" memory, which is not detached from the process yet will show up as owned by someone (DFB)
                      //            by disabling it, it will show up as owner_not_found or possible_holes_in_heap
                      //            by comparing the above two results, it may be possible to confirm that the memory free is not catching up..
                      // BUT, now that I see negative value in Others(unreported), something is wrong.
                      //     --> Negative Others(unreported) means, mincore returns bigger memory than smaps.
//#define COPY_SUPPORTED_IN_PROC
//#define ENABLE_MEMLOC
//#define DUMP_LOGS_INTO_FILE
#ifdef ENABLE_MEMLOC
#define MY_MLOCK(...) mlock(__VA_ARGS__)
#else
#define MY_MLOCK(...)
#endif

#define MAX_TRACE_ENTRIES (1024*100)
#define MAX_CHUNK_SIZE (1024*1024*1000)
#define MAX_OWNER_STRING_LEN (5*1024)
#define MAX_OWNERS_IN_RSS_CHUNK (4096*10)
#define MAX_MAPS_ENTRIES (1024)
#define MAX_STACKTRACE_STRING (5120)
#define MAX_TOTAL_CALLSTACK_LENGTH (MAX_STACKTRACE_STRING*MAX_TRACE_ENTRIES/10)
// telnet server related defines
#define ERROR -1
#define MAX_CLIENTS 1
#define MAX_DATA 1024


// debug printfs
#define DPF(...) printf(__VA_ARGS__)
#ifdef DISABLE_TELNET_SERVER
#define MY_PRINT(...) printf(__VA_ARGS__); if ( g_fout ) fprintf(g_fout, __VA_ARGS__);
#else
//#define MY_PRINT(...) printf(__VA_ARGS__); if ( g_fout ) fprintf(g_fout, __VA_ARGS__); if (g_accepted) { sprintf(g_tmp_line, __VA_ARGS__);send(g_accepted, g_tmp_line, strlen(g_tmp_line), 0);}
//#define MY_PRINT(...) if ( g_fout ) fprintf(g_fout, __VA_ARGS__); if (g_accepted) { sprintf(g_tmp_line, __VA_ARGS__);send(g_accepted, g_tmp_line, strlen(g_tmp_line), 0);}
#define MY_PRINT(...) if (g_accepted) { sprintf(g_tmp_line, __VA_ARGS__);send(g_accepted, g_tmp_line, strlen(g_tmp_line), 0);}
#endif
#define DPF2(...) printf(__VA_ARGS__)

//#define DPF3(...) printf(__VA_ARGS__)
#define DPF3(...)
//#define DPF4(...) printf(__VA_ARGS__)
#define DPF4(...)

//#define DEBUG_DESCREPANCY
#ifdef DEBUG_DESCREPANCY
#define DPF5(...) printf(__VA_ARGS__)
#else
#define DPF5(...)
#endif

void ma_print_call_stack(int print_freed_ones, unsigned int begin, unsigned int end, int printf_only);
//int mincore(void *addr, size_t length, unsigned char *vec);

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
void* __libc_malloc(size_t size) __attribute__((weak));
void* __libc_free(void *ptr) __attribute__((weak));
void* __libc_alloca(size_t size) __attribute__((weak));
void* __libc_realloc(void *ptr, size_t size) __attribute__((weak));
void* __libc_calloc(size_t nmemb, size_t size) __attribute__((weak));
char* __cxa_demangle (const char *__mangled_name, char *__output_buffer, size_t *__length, int *__status)  __attribute__((weak));
#endif

enum Alloctype {
    Alloctype_Malloc = 0,
    Alloctype_Calloc,
    Alloctype_Realloc,
    Alloctype_Mmap,
    Alloctype_Mmap64,
    Alloctype_Maps,
    Alloctype_Own,
    Alloctype_Alloca,
    Alloctype_None,
    Alloctype_Max
};

enum RSSSourceType {
    RSSSourceType_Smaps = 0,
    RSSSourceType_Status,
    RSSSourceType_Statm,
    RSSSourceType_Total
};

enum SMAPSBreakdown {
    SMAPSBreakdown_SharedClean = 0,
    SMAPSBreakdown_SharedDirty,
    SMAPSBreakdown_PrivateClean,
    SMAPSBreakdown_PrivateDirty
};

typedef struct {
    unsigned int start_addr;
    unsigned int size;
    int trace_size;
    int freed;
    char* callstack;
    enum Alloctype type;
} TRACES;

typedef struct {
    unsigned int start_addr;
    unsigned int end_addr;
    unsigned int seg_size;
    unsigned int vsize;
    unsigned int rss;
    char path_name[256];
} MAPS_DATA;

typedef struct {
    unsigned int index;
    unsigned int rss;
    unsigned int s_addr;
    unsigned int e_addr;
} MA_OWNERS;


#ifdef ENABLE_RB_TREE
#define RB_INIT rb_tree_init
#define RB_INSERT rb_insert
#define MY_MALLOC_INIT my_malloc_init
#else
#define RB_INIT(...)
#define RB_INSERT(...)
#define MY_MALLOC_INIT(...)
#endif


#endif // MA_RSS_INFO_GENERATOR_H_
