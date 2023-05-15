/* (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 U.S. copyright and other applicable intellectual property laws and
 may not be copied without the express permission of Netflix, Inc.,
 which reserves all rights.  Reuse of any of this content for any
 purpose without the permission of Netflix, Inc. is strictly
 prohibited.
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// < How to compile >
// gcc ma_rss_info_generator.c -o libma_rss_info_generator.so -fPIC -ldl -shared -lpthread -D_REENTRANT

//#define ONLY_SUPPORT_TOTAL_SMAPS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>
#include <signal.h>
#include <malloc.h>
#include <pthread.h>
#include <ctype.h>
#include <execinfo.h>

#include "red_black_tree/red_black_tree.h"
#include "ma_rss_info_generator.h"


#ifndef ONLY_SUPPORT_TOTAL_SMAPS
void* __libc_malloc(size_t size) __attribute__((weak));
void* __libc_free(void *ptr) __attribute__((weak));
void* __libc_alloca(size_t size) __attribute__((weak));
void* __libc_realloc(void *ptr, size_t size) __attribute__((weak));
void* __libc_calloc(size_t nmemb, size_t size) __attribute__((weak));
char * 	__cxa_demangle (const char *__mangled_name, char *__output_buffer, size_t *__length, int *__status)  __attribute__((weak));
#endif


#ifdef ENABLE_RB_TREE
static RB_TREE* g_rb_tree_ptr;
static uint32_t g_rb_mem_pool_addr;
static uint32_t g_rb_mem_pool_size;
#endif

void ma_print_call_stack(int print_freed_ones, unsigned int begin, unsigned int end, int printf_only);


const char g_alloc_type_string[Alloctype_Max][8] = {
    "malloc ",
    "calloc ",
    "realloc",
    "mmap   ",
    "mmap64 ",
    "maps   ",
    "own    ",
    "alloca ",
    "none   "};

// override & internal controls
static pthread_mutex_t g_mutex;
static int g_page_size = 4*1024;
static const int g_thread_stack_size = 8024*1024;
static int g_initialized = 0;
static int g_record_enabled = 0;
static FILE* g_fout = NULL;
static MAPS_DATA g_maps_entries[MAX_MAPS_ENTRIES];
#ifndef ONLY_SUPPORT_TOTAL_SMAPS
static pthread_mutex_t g_trace_mutex;
static pthread_mutex_t g_record_mutex;
static void * (*g_mmap)(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
static void * (*g_mmap64)(void *addr, size_t length, int prot, int flags, int fd, off64_t offset);
static int (*g_munmap)(void *addr, size_t len);
static int g_count = 0;
static int g_callstack_offset = 0;
static unsigned char g_vec[MAX_CHUNK_SIZE / 4 / 1024];
static char g_owner[MAX_OWNER_STRING_LEN];
static TRACES g_trace[MAX_TRACE_ENTRIES];
static MA_OWNERS g_owners[MAX_OWNERS_IN_RSS_CHUNK];
static char g_callstacks[MAX_TOTAL_CALLSTACK_LENGTH];
int g_freed_cnt = 0;
#endif

#ifdef DUMP_LOGS_INTO_FILE
static char* g_maps_out_file = "maps_analyzer_result.txt";
#endif

// telnet server
#ifndef DISABLE_TELNET_SERVER
static pthread_t g_telnet_thread;
static pthread_attr_t g_thread_attr;
static int sock=0, g_accepted=0;
static char g_tmp_line[2056];
#endif


// configurations
static int g_page_cnt_th_for_rss = 2; // RSS that is bigger than this value x page_size (4K) will be reported
static unsigned int g_malloc_trace_th = 10*1024; // backtrace will be generated for the malloc with the size > this threshold
static int g_verbose = 0;

enum MEMSOURCE {
    MEMSOURCE_UNKNOWN = 0,
    MEMSOURCE_MAPS
};

int ma_get_total_rss(int pid, enum RSSSourceType type);
int ma_process_smaps_file(int pid);

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
static void print_mincore_error()
{
    switch ( errno ){
    case (EAGAIN):
        DPF2("EAGAIN : kernel is temporarily out of resources.\n");
        break;
    case (EFAULT):
        DPF2("EFAULT : vec points to an invalid address.\n");
        break;
    case (EINVAL):
        DPF2("EINVAL : addr is not a multiple of the page size.\n");
        break;
    case (ENOMEM):
        DPF2("ENOMEM : addr to addr + length contained unmapped memory.\n");
        break;
    default:
        DPF2("Something else.\n");
        break;
    }
}
#endif

static unsigned int my_pow(unsigned int base, unsigned int pow)
{
    int i;
    unsigned int result = 1;
    for ( i = 0 ; i < (int) pow ; i++ ) {
        result *= base;
    }
    return result;
}

static unsigned int hex_str_to_uint(char* hex_str)
{
    int length = strlen(hex_str);
    char one_digit[2];
    unsigned int result = 0;
    unsigned int tmp = 0;
    char c;
    int i, hex_str_idx;
    for ( hex_str_idx=0,i = length - 1; i >= 0; hex_str_idx++,i-- ) {
        c = hex_str[hex_str_idx];
        if ( c != '0' && c != 'x' ) {
            one_digit[0] = hex_str[hex_str_idx];
            one_digit[1] = '\0';

            if ( isdigit(one_digit[0]) ) {
                tmp = (unsigned int) ( atoi(one_digit) );
            } else if ( isalpha(one_digit[0]) ){
                one_digit[0] = tolower(one_digit[0]);
                tmp = (int) one_digit[0] - (int) 'a' + 10;
            }
            result += tmp*(my_pow(16, i));
        }
    }
    return result;
}

#ifndef DISABLE_TELNET_SERVER
static void cleanupSockets(int signal) {
    (void) signal; // for compiler warning

    if ( g_accepted ) {
        close(g_accepted);
        g_accepted = 0;
    }
    if ( sock ) {
        close(sock);
        sock = 0;
    }
}

static void run_simple_telnet_server(int port){
    unsigned int sockaddr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client;
    struct sockaddr_in server;
    int opt;
    int data_len;
    char data[MAX_DATA];
    char line[512];
    int running = 1;

    // open socket
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
        perror("[MemAnalyzer] Failed opening socket! ");
        exit(-1);
    }

    opt = 1;
    if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[MemAnalyzer] Failed setting SO_REUSEADDR! ");
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&server.sin_zero, 8);


    // bind the server
    if((bind(sock, (struct sockaddr*) &server, sockaddr_len)) == ERROR) {
        perror("[MemAnalyzer] Failed to bind");
#if 0
        close(sock);
        exit(-1);
#else
        // Hack to work around the case when libmaps_analyzer.so gets loaded more than once...
        // this hack is no longer needed with unsetting LD_PRELOAD
        if ( g_accepted == 0 ) {
            close(sock);
        }
#endif
        return;
    }

    // listen on socket
    if((listen(sock, MAX_CLIENTS)) == ERROR) {
        perror("[MemAnalyzer] Failed to listen");
        exit(11);
    }

    DPF("[MemAnalyzer] Starting telnet on port %d\n", port);
    while (running) {
        if((g_accepted = accept(sock, (struct sockaddr *)&client, &sockaddr_len)) == ERROR) {
            perror("[MemAnalyzer] Failed in accept");
            exit(-1);
        }

        data_len = 1;

        while(data_len) {
            data_len = recv(g_accepted, data, MAX_DATA, 0);

            if(data_len) {
                // send(g_accepted, data, data_len, 0);
                //DPF("data_len = %d\n", data_len);
                //assert (data_len >= 2);
                //if ( data_len < 2 ) {
                //    data_len = 0;
                //    continue;
                //}
                data[data_len-2] = '\0'; // [FIXME} not really sure why this is needed... probably line feed and cariage return...

                // use strstr instead of strcmp because sometimes multiple commands gets queued up.
                if ( strstr(data, "quit") != NULL ) {
                    MY_PRINT("disabling the interposer\n");
                    g_record_enabled = 0;
                    running = 0;
                    break;
                } else if ( strstr(data, "help") != NULL ) {
                    strcpy(line, "=====\nhelp : prints this message\nrss(v)  : prints rss report based on maps (verbose)\nsrss(v)  : print srss report based on smaps (verbose)\nquit : closes the telnet connection and the server\n===\n");
                    send(g_accepted, line, strlen(line), 0);
                } else if ( strstr(data, "status") != NULL ) {
                    ma_get_total_rss(getpid(), RSSSourceType_Status);
                } else if ( strstr(data, "stat") != NULL ) {
                    ma_get_total_rss(getpid(), RSSSourceType_Statm);
                } else if ( strstr(data, "totalsmaps") != NULL ) {
                    ma_get_total_rss(getpid(), RSSSourceType_Smaps);
#ifndef ONLY_SUPPORT_TOTAL_SMAPS
                } else if ( strstr(data, "srssv") != NULL ) {
                    g_verbose=1;
                    ma_process_smaps_file(getpid());
                    g_verbose=0;
                } else if ( strstr(data, "srss") != NULL ) {
                    g_verbose=0;
                    ma_process_smaps_file(getpid());
                } else if ( strstr(data, "printa") != NULL ) {
                    ma_print_call_stack(1, 0, 0, 0);
                } else if ( strstr(data, "print") != NULL ) {
                    ma_print_call_stack(0, 0, 0, 0);
#endif
                } else if ( strstr(data, "trim") != NULL ) {
                    malloc_trim((size_t) 0);
                } else {
                    sprintf(line, "command not supported! : %s\n", data);
                    send(g_accepted, line, strlen(line), 0);
                }

            }
        }

        DPF("[MemAnalyzer] Client disconnected\n");

        close(g_accepted);
        g_accepted = 0;

    }


    DPF("[MemAnalyzer] Closing telnet server!!\n");
    close(sock);
    sock = 0;
}

static void * run_telnet_server(void *arg)
{
    (void) arg; // for gcc warning
    char *port_c = getenv("MA_PORT");
    int port = 1234;
    if ( port_c ) {
        port = atoi(port_c);
    }

    signal(SIGSEGV, cleanupSockets);
    signal(SIGBUS, cleanupSockets);
    signal(SIGABRT, cleanupSockets);
    signal(SIGINT, cleanupSockets);
    signal(SIGPIPE, cleanupSockets);

    run_simple_telnet_server(port);

    return (void*) NULL;

}

#endif // DISABLE_TELNET_SERVER

static void get_next_rss_chunk(int* next_index, int *page_nums, unsigned char* rss_bitmap, int start_idx, int bitmap_size)
{
   int idx;
   int n_index = bitmap_size;
   int p_nums = 0;

   for ( idx = start_idx; idx < bitmap_size; idx++ ) {
       if ( rss_bitmap[idx] == 1 ) {
           n_index = idx;
           break;
       }
   }
   if ( n_index != bitmap_size ) {
       while ( idx < bitmap_size && rss_bitmap[idx]) {
          idx++;
          p_nums++;
       }
   }
   *next_index = n_index;
   *page_nums = p_nums;

}

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
static int already_counted(unsigned int* s_addr, unsigned int* e_addr, MA_OWNERS* p_owners, int owners_idx)
{
    int i;
    int counted = 0;
    unsigned int sa = *s_addr;
    unsigned int ea = *e_addr;

    // work on this!!! there may be bug in this loop
    for ( i = 0; i < owners_idx; i++ ) {

        // avoid double count
        if ( ( sa >= p_owners[i].s_addr ) && ( ea <= p_owners[i].e_addr ) ) {
            counted = 1;
            break;
        }

#if 1
        if ( sa == p_owners[i].s_addr ) {
            sa = p_owners[i].e_addr;
        }
        if ( ea == p_owners[i].e_addr ) {
            ea = p_owners[i].s_addr;
        }
#endif

#if 1
        if ( sa < p_owners[i].s_addr ) {
            if ( ea > p_owners[i].e_addr ) {
                // [HACK]
                ea = p_owners[i].s_addr + ea - p_owners[i].e_addr;
            } else if ( ea > p_owners[i].s_addr ) {
                ea = p_owners[i].s_addr;
            }
        }
        if ( ea > p_owners[i].e_addr ) {
            if ( sa < p_owners[i].s_addr ) {
                // [HACK]
                sa =  p_owners[i].e_addr - (p_owners[i].s_addr - sa);
            } else if ( sa < p_owners[i].e_addr ) {
                sa = p_owners[i].e_addr;
            }
        }
#endif

    }

    *s_addr = sa;
    *e_addr = ea;
    return counted;
}

static int find_owners(MA_OWNERS* p_owners, int owners_start_idx, unsigned int addr, int rss_segment_size) {
    int owners_idx = owners_start_idx;
    int idx, max_idx = g_count;
    unsigned int rss_start_addr = addr;
    unsigned int rss_end_addr = addr + rss_segment_size;
    unsigned int owner_s_addr;
    unsigned int owner_e_addr;

//    for ( idx = 0; idx < max_idx ; idx++ ) {
    for ( idx = max_idx - 1; idx >= 0 ; idx-- ) {
        unsigned int s_addr = g_trace[idx].start_addr;
        unsigned int e_addr = g_trace[idx].start_addr + g_trace[idx].size;

        // skip freed ones
        if ( g_trace[idx].freed ) continue;

        // skip the entry that has no overlap
        if ( e_addr < rss_start_addr || s_addr > rss_end_addr ) continue;

        // [HACK]: Without this, somehow it will be double counting and reports much more RSS size than smaps report....
        if ( already_counted(&s_addr, &e_addr, p_owners, owners_idx) ) continue;


        // find if there is any overlap
        if ( ( (s_addr >= rss_start_addr) && (s_addr < rss_end_addr) ) ||
             ( (s_addr <= rss_start_addr) && (e_addr > rss_start_addr) )
        ) {

           if ( s_addr < rss_start_addr ) {
                owner_s_addr = rss_start_addr;
            } else {
                owner_s_addr = s_addr;
            }

            if ( e_addr < rss_end_addr ) {
                owner_e_addr = e_addr;
            } else {
                owner_e_addr = rss_end_addr;
            }

            assert( owners_idx < MAX_OWNERS_IN_RSS_CHUNK );

            p_owners[owners_idx].index = idx;
            p_owners[owners_idx].rss = owner_e_addr - owner_s_addr;
            p_owners[owners_idx].s_addr = owner_s_addr;
            p_owners[owners_idx].e_addr = owner_e_addr;

            owners_idx++;
        }
    }
    return (owners_idx - owners_start_idx);

}

static int analyze_owners(unsigned int addr, unsigned char* rss_bitmap, int bitmap_size)
{
    int idx = 0;
    int next_idx = 0;
    int page_nums = 0;
    int num_of_owners = 0;
    unsigned int rss_chunk_s_addr;

    DPF4("freed count = %d\n", g_freed_cnt);
    while ( idx < bitmap_size ) {
        // seek to the rss chunks in mincore bitmap
        get_next_rss_chunk(&next_idx, &page_nums, rss_bitmap, idx, bitmap_size);

        // increment the index by the chunk size for the next run
        DPF4("idx = %d : next_idx = %d : %d : page_nums = %d\n", idx, next_idx, bitmap_size, page_nums);
        idx = next_idx + page_nums;

        // compute the address of the beginning of the chunk
        rss_chunk_s_addr = addr + next_idx * g_page_size;

        // find owners of the chunk
        num_of_owners += find_owners(g_owners, num_of_owners, rss_chunk_s_addr, page_nums * g_page_size);

    }

    return num_of_owners;
}

static void my_demangle(char* out, char* input)
{
    const char* delim = "()+";
    char *base_input = strdup(input);
    char *saveptr;
    char *second;
    char *demangled, *demangled_tmp;
    int status = 0;

    // strtok_r simply replaces the delimiter with null charactor
    strtok_r(base_input, delim, &saveptr);
    second = strtok_r(NULL, delim, &saveptr);
#ifdef DEMANGLE_CALLSTACK
    demangled = __cxa_demangle(second, NULL, NULL, &status);
#else
    demangled = second;
#endif
    // ---- EXAMPLE ----
    //   base_input : netflix(_ZNK7netflix11application6gibbon15SurfaceDecoders7surfaceERKNS_4base10DataBufferEib+0x6a) [0x831a57a]
    //   first result of strtok_r : netflix
    //   second     : _ZNK7netflix11application6gibbon15SurfaceDecoders7surfaceERKNS_4base10DataBufferEib
    //   demangled  : netflix::application::gibbon::SurfaceDecoders::surface(netflix::base::DataBuffer const&, int, bool)
    //   out        : netflix::application::gibbon::SurfaceDecoders::surface

    if ( status == 0 ) {
        if ( g_verbose ) {
            strcpy(out, demangled);
        } else {
            demangled_tmp = demangled;
            strcpy(out, strtok_r(demangled_tmp, delim, &saveptr));
        }
#ifdef DEMANGLE_CALLSTACK
        // free the returned string of __cxa_demangle
        if ( demangled ) free (demangled);
#endif
    } else {
        strcpy(out, base_input);
    }

    // free the returned string of strdup
    if ( base_input ) free (base_input);

    return;
}
#endif // ONLY_SUPPORT_TOTAL_SMAPS

static void print_rss_analysis_line(int rss_page_cnt, unsigned int rss, char* p_owner, unsigned int page_size, unsigned int start_addr, unsigned int end_addr)
{
    int do_print = rss_page_cnt >= g_page_cnt_th_for_rss;

    if ( do_print ) {
        if ( g_verbose ) {
            MY_PRINT("%10d bytes ( %4d x %d ) in 0x%08x - 0x%08x ( %10d )  ", rss, rss / page_size, page_size, start_addr, end_addr, end_addr-start_addr);
            MY_PRINT(" %s \n", p_owner);
        } else {
            MY_PRINT("%10d  ", rss);
            MY_PRINT("%s\n", p_owner);
        }
    }

}

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
static void generate_concatenated_callstack(char* out, int begin_depth, int end_depth, int trace_size, char** callstack)
{
    int cs_idx;
    int offset = 0;

    int cs_max = (trace_size - 1)<end_depth?(trace_size - 1):end_depth;
    for ( cs_idx = begin_depth; cs_idx <= cs_max ; cs_idx++ ) {
        my_demangle(&out[offset], basename(callstack[cs_idx]));
        offset = strlen(out);
        sprintf(&out[offset], ";");
        offset++;
    }
}


static int get_resident_size(void* addr, unsigned int segment_size, size_t page_size, char* path_name, int* source, int size_from_smaps)
{
    unsigned int aligned_addr;
    unsigned int modified_seg_size;
    int i, rss_bitmap_size=0, owner_idx;
    int result;
    unsigned int cnt=0;
    int num_of_owners, owners_idx;
    unsigned int size;
    unsigned int heap_entry_in_maps = 0;
    unsigned int somehow_labeled_as_stack = 0;
    unsigned int anonymously_mapped = 0;


    // align the addr
    aligned_addr = (unsigned int) addr & ~(page_size-1);
    modified_seg_size = segment_size + (aligned_addr - (unsigned int) addr);
    rss_bitmap_size = (modified_seg_size + (page_size-1)) / page_size;

    // check rss using mincore
    result = mincore((void*) aligned_addr, modified_seg_size, g_vec);
    if ( result == -1 ) {
        printf("addr = 0x%x : a_addr = 0x%x : size = %d : a_size = %d\n", (unsigned int) addr, aligned_addr, segment_size, modified_seg_size);
        print_mincore_error();
        //cnt = rss_bitmap_size;
    }

    // count rss pages
    DPF4("g_vec!!\n");
    assert ( rss_bitmap_size < MAX_CHUNK_SIZE / 4 / 1024 );
    for ( i = 0; i < rss_bitmap_size; i++){
        DPF4("%d", g_vec[i]);
        if ( g_vec[i] & 1 ) {
            cnt++;
        }
    }
    DPF4("\n");

    size = size_from_smaps;

    anonymously_mapped = (strcmp(path_name, "") == 0);
    heap_entry_in_maps = (strcmp(path_name, "[heap]") == 0);
    somehow_labeled_as_stack = (strstr(path_name, "[stack") != NULL);

    if ( !(anonymously_mapped || heap_entry_in_maps || somehow_labeled_as_stack) ) {
        // Just read off the owner identified by smaps
        sprintf(g_owner, "%s %s", g_alloc_type_string[Alloctype_Maps], basename(path_name));
        *source = MEMSOURCE_MAPS;

        print_rss_analysis_line(cnt, size, g_owner, g_page_size, aligned_addr, aligned_addr+modified_seg_size);
    } else {
        // Analyze the owner
        int rss_from_analysis, rss_total_from_analysis = 0;

#ifdef REPORT_GENERAL_HEAP_AS_WELL
        if ( heap_entry_in_maps ) {
            sprintf(g_owner, "%s %s", g_alloc_type_string(Alloctype_Maps), basename(path_name));
            *source = MEMSOURCE_MAPS;
            print_rss_analysis_line(cnt, size, g_owner, g_page_size, aligned_addr, aligned_addr+modified_seg_size);
        }
#endif

        DPF4("\n\nStart!\n");
        DPF4("rss = %d\n", size);
        DPF4("0x%08x to 0x%08x\n", aligned_addr, aligned_addr+modified_seg_size);

        DPF5("\n\nStart!\n");

        // analyze owners (there can be multiple owners in one maps/smaps segment )
        num_of_owners = analyze_owners((unsigned int) addr, g_vec, rss_bitmap_size);

        if ( num_of_owners == 0 ) {
            strcpy(g_owner, "none    no_owner_found_in_rss_chunk");
            *source = MEMSOURCE_UNKNOWN;
            print_rss_analysis_line(cnt, size, g_owner, g_page_size, aligned_addr, aligned_addr+modified_seg_size);
        } else {
#ifdef CHECK_HOLES_IN_RSS_CHUNK
            int holesIdx = 0;
            unsigned int start_addr_of_holes[1024];
            unsigned int end_addr_of_holes[1024];
#endif

            for ( owners_idx = 0; owners_idx < num_of_owners; owners_idx++ ) {

#ifdef CHECK_HOLES_IN_RSS_CHUNK
                if ( owners_idx > 0 ) {
                    if ( (int) (g_owners[owners_idx].s_addr - g_owners[owners_idx-1].e_addr) > 0 ) {
                        start_addr_of_holes[holesIdx] = g_owners[owners_idx-1].e_addr;
                        end_addr_of_holes[holesIdx] = g_owners[owners_idx].s_addr;
                        holesIdx++;
                    }
                }
#endif
                owner_idx = g_owners[owners_idx].index;
                rss_from_analysis = g_owners[owners_idx].rss;

                rss_total_from_analysis += rss_from_analysis;

                if ( g_trace[owner_idx].callstack ) {
                    // generate owner info along with with allocation type
                    sprintf(g_owner, "%s %s", g_alloc_type_string[g_trace[owner_idx].type], g_trace[owner_idx].callstack);
                } else {
                    // this should not happen
                    strcpy(g_owner, "none     no_call_stack_avaialble");
                    *source = MEMSOURCE_UNKNOWN;
                }


                //print_rss_analysis_line(rss_from_analysis/g_page_size, rss_from_analysis, g_owner, g_page_size, g_trace[owner_idx].start_addr, g_trace[owner_idx].start_addr + g_trace[owner_idx].size);
                print_rss_analysis_line(rss_from_analysis/g_page_size, rss_from_analysis, g_owner, g_page_size, g_owners[owners_idx].s_addr, g_owners[owners_idx].e_addr);

            } // end of owners_idx for loop

#ifdef CHECK_HOLES_IN_RSS_CHUNK
            if ( holesIdx > 0 ) {
                int k;
                MY_PRINT("<<HOLES>>\n");
                for ( k = 0; k < holesIdx; k++) {
                    MY_PRINT("0x%08x - 0x%08x : %u bytes\n", start_addr_of_holes[k], end_addr_of_holes[k], end_addr_of_holes[k] - start_addr_of_holes[k] );
                }
                MY_PRINT("\n");
            }
#endif

            if ( heap_entry_in_maps ) {
                int diff = size - rss_total_from_analysis;
                print_rss_analysis_line(diff/g_page_size, diff, "maps    possible_holes_in_heap", g_page_size, 0, 0);
            } else {
                int diff = size - rss_total_from_analysis;
                print_rss_analysis_line(diff/g_page_size, diff, "none    possible_holes_in_rss", g_page_size, 0, 0);
            }

#ifdef DEBUG_DESCREPANCY
            if ( ( size - rss_total_from_analysis ) > 25*4*1024 ) {
                int print_freed_callstack = 1;
                DPF5("@@@@@@@@@@@ rss (smaps, mincore, mine) = %d, %d, %d\n", size, cnt*page_size,rss_total_from_analysis);
                DPF5("between 0x%08x and 0x%08x ( %d )\n", aligned_addr, aligned_addr+modified_seg_size, modified_seg_size);
                ma_print_call_stack(print_freed_callstack, aligned_addr, aligned_addr+modified_seg_size, 1);
                DPF5("\n");
            }
#endif
        }
    }

    return cnt * page_size;

}
#endif // ONLY_SUPPORT_TOTAL_SMAPS

static int parse_maps_line(char* line, MAPS_DATA* out){
    int scanned_item = 0;
    int it_was_address_line = 0;
    char start_addr[256];
    char end_addr[256];
    out->path_name[0] = '\0';

    scanned_item = sscanf(line, "%[^-]%*c%s%*s%*s%*s%*s%s", start_addr, end_addr, out->path_name);
    if ( scanned_item >= 2 ) {
        //DPF("%s-%s : %s\n", start_addr, end_addr, out->path_name);
        out->start_addr = hex_str_to_uint(start_addr);
        out->end_addr = hex_str_to_uint(end_addr);
        out->seg_size = out->end_addr - out->start_addr;
        it_was_address_line = 1;
    }
    //DPF("0x%x-0x%x %s\n", out->start_addr, out->end_addr, out->path_name);

    return it_was_address_line;
}

static int get_rss_from_status(FILE* fin, MAPS_DATA* out) {
    char line[512];
    char field[32], unit[16];
    int size;

    while ( fgets(line, 512, fin) ) {
        sscanf(line, "%s%u%s", field, &size, unit);
        //printf("%s %u %s\n", field, size, unit);
        if ( strcmp(unit, "kB") != 0 ) {
            //printf("status size unit is not kB!! : unit = %s\n", unit);
        } else if ( strcmp(field, "VmHWM:") == 0 ) {
            out->vsize = size * 1024;
        } else if ( strcmp(field, "VmRSS:") == 0 ) {
            out->rss = size * 1024;
        }
    }

    return out->rss;
}


static int get_rss_from_statm(FILE* fin, MAPS_DATA* out) {
    int share, text, lib, data, dt;
    int num, resident = 0, size = 0;

#if 0

       /proc/[pid]/statm
              Provides information about memory usage, measured in pages.
              The columns are:

                  size       (1) total program size
                             (same as VmSize in /proc/[pid]/status)
                  resident   (2) resident set size
                             (same as VmRSS in /proc/[pid]/status)
                  share      (3) shared pages (i.e., backed by a file)
                  text       (4) text (code)
                  lib        (5) library (unused in Linux 2.6)
                  data       (6) data + stack
                  dt         (7) dirty pages (unused in Linux 2.6)
#endif
    num = fscanf(fin, "%d %d %d %d %d %d %d", &size, &resident, &share, &text, &lib, &data, &dt);
    if ( num > 0 ) {
        printf("%d %d %d %d %d %d %d\n", size, resident, share, text, lib, data, dt);
    }
    out->vsize = size * g_page_size;
    out->rss = resident * g_page_size;

    return resident;
}



#ifndef ONLY_SUPPORT_TOTAL_SMAPS
void print_total_rss(unsigned int total_rss, unsigned int* rss_breakdown)
{
    (void) total_rss;     // for compiler warning
    (void) rss_breakdown; // for compiler warning
    DPF4("%6u MB : TotalRSS : ( %u MB + %u MB + %u MB )\n",
               ((total_rss / 1024) + 512) / 1024,
               ((rss_breakdown[MEMSOURCE_MAPS] / 1024) + 512) / 1024,
               (((total_rss - rss_breakdown[MEMSOURCE_MAPS] - rss_breakdown[MEMSOURCE_UNKNOWN]) / 1024 ) + 512 ) / 1024,
               ((rss_breakdown[MEMSOURCE_UNKNOWN] / 1024 ) + 512) / 1024);
}
#endif

#define EXECUTE_SYSTEM_COMMAND(CMD) { \
    int result = system(CMD); \
    if ( result == -1 ) { \
        ;DPF4("[MemAnalyzer] Failed executing system command: %s\n", CMD); \
    } else  { \
        ;DPF4("[MemAnalyzer] SUCCESS executing system command ***************************************: %s\n", CMD); \
    } \
}

int ma_get_total_rss(int pid, enum RSSSourceType type)
{
    char cmdLine[256];
    char line[MAX_MAPS_ENTRIES];
    char tmpFileName[32];
    char memResourceFileName[64];
    FILE* fin;

    unsigned int size_smaps, total_rss_from_smaps = 0;
    unsigned int total_rss_breakdown_from_smaps[MEMSOURCE_MAPS+1] = {0};

    //g_record_enabled = 0;
    // open file
    strcpy(tmpFileName, "/tmp/tmprss");

    switch (type ) {
    case (RSSSourceType_Smaps):
        strcpy(memResourceFileName, "smaps");
        break;
    case (RSSSourceType_Status):
        strcpy(memResourceFileName, "status");
        break;
    case (RSSSourceType_Statm):
        strcpy(memResourceFileName, "statm");
        break;
    default:
        strcpy(memResourceFileName, "smaps");
        break;
    }

#ifdef COPY_SUPPORTED_IN_PROC // some platform does not support cp for proc virtual file system
    sprintf(cmdLine, "rm -f %s;cp -f /proc/%d/%s %s", tmpFileName, pid, memResourceFileName, tmpFileName);
#else
    sprintf(cmdLine, "rm -f %s;cat /proc/%d/%s > %s", tmpFileName, pid, memResourceFileName, tmpFileName);
#endif
    EXECUTE_SYSTEM_COMMAND(cmdLine);

    if ( (fin = fopen(tmpFileName, "r")) == NULL ) {
        DPF("[MemAnalyzer] Failed opening %s\n", tmpFileName);
        return 1;
    }

#ifdef DUMP_LOGS_INTO_FILE
    sprintf(cmdLine, "rm -f %s", g_maps_out_file);
    EXECUTE_SYSTEM_COMMAND(cmdLine);

    if ( (g_fout = fopen(g_maps_out_file, "w")) == NULL ) {
        DPF("[MemAnalyzer] Failed opening %s\n", g_maps_out_file);
        return 1;
    }
#endif

    // start reading
    //fgets(line, 511, fin); // skip the first line
    switch (type) {
    case (RSSSourceType_Smaps ):
    {
        MAPS_DATA* leading_ptr = &g_maps_entries[0];
        MAPS_DATA* ptr = &g_maps_entries[0];
        int total_smaps_breakdown[SMAPSBreakdown_PrivateDirty+1] = {0};

        while ( fgets(line, MAX_MAPS_ENTRIES - 1, fin) ) {
            //DPF("line[%d] = %s", i, line);


            if ( parse_maps_line(line, leading_ptr) ) {
                ptr = leading_ptr;
                leading_ptr++;
            } else {

                char field[32], unit[16];
                int size;

                sscanf(line, "%s%u%s", field, &size, unit);
                //printf("%s %u %s\n", field, size, unit);
                if ( strcmp(unit, "kB") != 0 ) {
                    printf("smaps size unit is not kB!! : unit = %s\n", unit);
                } else if ( strcmp(field, "Size:") == 0 ) {
                    ptr->vsize = size * 1024;
                } else if ( strcmp(field, "Rss:") == 0 ) {
                    size_smaps = ptr->rss = size * 1024;
                    total_rss_from_smaps += size_smaps;
                } else if( strcmp(field, "Shared_Clean:") == 0 ) {
                    total_smaps_breakdown[SMAPSBreakdown_SharedClean] += size * 1024;
                } else if ( strcmp(field, "Shared_Dirty:") == 0 ) {
                    total_smaps_breakdown[SMAPSBreakdown_SharedDirty] += size * 1024;
                } else if ( strcmp(field, "Private_Clean:") == 0 ) {
                    total_smaps_breakdown[SMAPSBreakdown_PrivateClean] += size * 1024;
                } else if ( strcmp(field, "Private_Dirty:") == 0 ) {
                    total_smaps_breakdown[SMAPSBreakdown_PrivateDirty] += size * 1024;
                }

            }

        }
        MY_PRINT("%10u  maps   SharedClean\n", total_smaps_breakdown[SMAPSBreakdown_SharedClean]);
        MY_PRINT("%10u  maps   SharedDirty\n", total_smaps_breakdown[SMAPSBreakdown_SharedDirty]);
        MY_PRINT("%10u  maps   PrivateClean\n", total_smaps_breakdown[SMAPSBreakdown_PrivateClean]);
        MY_PRINT("%10u  maps   PrivateDirty\n", total_smaps_breakdown[SMAPSBreakdown_PrivateDirty]);

        break;
    }
    case (RSSSourceType_Status):
    {
        int i = 0;
        while ( fgets(line, MAX_MAPS_ENTRIES - 1, fin) ) {
            MAPS_DATA* ptr = &g_maps_entries[i];
            //DPF("line[%d] = %s", i, line);

            parse_maps_line(line, ptr);
            size_smaps = get_rss_from_status(fin, ptr);
            total_rss_from_smaps += size_smaps;
            i++;
        }
        break;
    }
    case (RSSSourceType_Statm):
    {
        int i = 0;
        MAPS_DATA* ptr = &g_maps_entries[i];
        size_smaps = get_rss_from_statm(fin, ptr);
        total_rss_from_smaps += size_smaps;
        break;
    }
    default:
        break;
    }

    //g_record_enabled = 1;

    if ( fin ) {
        fclose(fin);
    }

    MY_PRINT("%10u  total   smaps_total\n", total_rss_from_smaps);
#ifndef ONLY_SUPPORT_TOTAL_SMAPS
    print_total_rss(total_rss_from_smaps, total_rss_breakdown_from_smaps);
#endif
    if ( g_fout ) {
        fclose(g_fout);
        DPF3("g_fout = %xn", (unsigned int) g_fout);
        g_fout = NULL;
    }

    return 0;

}

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
int ma_process_smaps_file(int pid)
{
    char line[MAX_MAPS_ENTRIES];
    char tmpFileName[32];
    FILE* fin;
    MAPS_DATA* leading_ptr = &g_maps_entries[0];
    MAPS_DATA* ptr = &g_maps_entries[0];
    unsigned int size_mincore, size_smaps, total_rss_from_mincore = 0, total_rss_from_smaps = 0;
    unsigned int total_rss_breakdown_from_mincore[MEMSOURCE_MAPS+1] = {0};
    unsigned int total_rss_breakdown_from_smaps[MEMSOURCE_MAPS+1] = {0};
    int source = MEMSOURCE_UNKNOWN;

    //g_record_enabled = 0;
    // open file

#ifdef READ_SMAPS_DIRECTLY

    sprintf(tmpFileName, "/proc/%d/smaps", pid);

#else // READ_SMAPS_DIRECTLY
    char cmdLine[256];

    strcpy(tmpFileName, "/tmp/tmp_smaps");


// [FIXME] somehow these system commands starts failing soon after the mem analyzer gets launched..
#if 1
#ifdef COPY_SUPPORTED_IN_PROC // some platform does not support cp for proc virtual file system
    sprintf(cmdLine, "rm -f %s;cp -f /proc/%d/%s %s", tmpFileName, pid, "smaps", tmpFileName);
#else
    sprintf(cmdLine, "rm -f %s;cat /proc/%d/%s > %s", tmpFileName, pid, "smaps", tmpFileName);
    #endif

    EXECUTE_SYSTEM_COMMAND(cmdLine);
#else
    sprintf(cmdLine, "rm -f %s", tmpFileName);
    EXECUTE_SYSTEM_COMMAND(cmdLine);

    sprintf(cmdLine, "cat  /proc/%d/%s > %s", pid, "smaps", tmpFileName);
    EXECUTE_SYSTEM_COMMAND(cmdLine);

    sprintf(cmdLine, "chmod 777 %s", tmpFileName);
    EXECUTE_SYSTEM_COMMAND(cmdLine);
#endif

#endif // READ_SMAPS_DIRECTLY
    if ( (fin = fopen(tmpFileName, "r")) == NULL ) {
        DPF("[MemAnalyzer] Failed opening %s\n", tmpFileName);
        return 1;
    }

#ifdef DUMP_LOGS_INTO_FILE
    sprintf(cmdLine, "rm -f %s", g_maps_out_file);
    EXECUTE_SYSTEM_COMMAND(cmdLine);
    if ( (g_fout = fopen(g_maps_out_file, "w")) == NULL ) {
        DPF("[MemAnalyzer] Failed opening %s\n", g_maps_out_file);
        return 1;
    }
#endif

    // start reading
    //fgets(line, 511, fin); // skip the first line
    while ( fgets(line, MAX_MAPS_ENTRIES - 1, fin) ) {
        //DPF("line[%d] = %s", i, line);
        if ( parse_maps_line(line, leading_ptr) ) {
//printf("line = %s\n", line);
             ptr = leading_ptr;
             leading_ptr++;
        } else {
            char field[32], unit[16];
            int size;
//printf("[2]line = %s\n", line);

            sscanf(line, "%s%u%s", field, &size, unit);
            //printf("%s %u %s\n", field, size, unit);
            if ( strcmp(unit, "kB") != 0 ) {
                printf("smaps size unit is not kB!! : unit = %s\n", unit);
            } else if ( strcmp(field, "Size:") == 0 ) {
                ptr->vsize = size * 1024;
            } else if ( strcmp(field, "Rss:") == 0 ) {
                size_smaps = ptr->rss = size * 1024;

                // get mincore based size and determine source type
                size_mincore = get_resident_size((void*) ptr->start_addr, ptr->seg_size, g_page_size, ptr->path_name, &source, size_smaps);

                // update smaps based values
                total_rss_from_smaps += size_smaps;
                total_rss_breakdown_from_smaps[source] += size_smaps;

                // update mincore based values
                total_rss_from_mincore += size_mincore;
                total_rss_breakdown_from_mincore[source] += size_mincore;
            }
        }

    }

    //g_record_enabled = 1;

    if ( fin ) {
        fclose(fin);
        fin = NULL;
    }

    MY_PRINT("%10u  total   smaps_total\n", total_rss_from_smaps);
    print_total_rss(total_rss_from_smaps, total_rss_breakdown_from_smaps);

    if ( g_fout ) {
        fclose(g_fout);
        DPF3("g_fout = %xn", (unsigned int) g_fout);
        g_fout = NULL;
    }

    return 0;

}
#endif

#ifndef ONLY_SUPPORT_TOTAL_SMAPS

static void free_trace_info()
{
    g_record_enabled = 0;
    printf("0x%x : g_vec\n", (unsigned int) g_vec);
    printf("0x%x : g_owner\n", (unsigned int) g_owner);
    printf("0x%x : g_trace\n", (unsigned int) g_trace);
    printf("0x%x : g_maps_entries\n", (unsigned int) g_maps_entries);
}


void ma_dump_callstack(int idx)
{
    void *trace[32];
    int trace_size;
    int concatenated_callstack_size;
    char** callstack;

    DPF3("calling backtrace from dump_call_stack\n");

    trace_size = backtrace(trace, 32);

    DPF3("calling backtrace_symbols\n");

    callstack = (char**) backtrace_symbols(trace, trace_size);

    generate_concatenated_callstack(&g_callstacks[g_callstack_offset], 3, 12, trace_size, callstack);

    g_trace[idx].callstack = &g_callstacks[g_callstack_offset];

    concatenated_callstack_size = strlen(&g_callstacks[g_callstack_offset]);
    g_callstacks[g_callstack_offset+concatenated_callstack_size] = '\0';
    g_callstack_offset += concatenated_callstack_size + 1;

    // it is a bit too late to check it here...
    assert(g_callstack_offset < MAX_TOTAL_CALLSTACK_LENGTH );

    DPF3("calling backtrace from dump_call_stack -- done\n");

    g_trace[idx].trace_size = trace_size;

    if ( callstack) {
        free ( callstack );
    }
}
#endif // ONLY_SUPPORT_TOTAL_SMAPS

#define SET_CONFIG(VAR, ENV_VAR) \
    { \
        char* tmp = getenv(ENV_VAR);\
        if ( tmp ) VAR = atoi(tmp);\
    }

static void set_configuration()
{
    if ( getenv("MA_KEEP_LD_PRELOAD") == 0) {
        unsetenv("LD_PRELOAD");
    }

    g_page_size = sysconf(_SC_PAGESIZE);

    SET_CONFIG(g_page_cnt_th_for_rss, "MA_REPORT_PAGE_NUM_TH");
    SET_CONFIG(g_malloc_trace_th, "MA_MALLOC_SIZE_TH");
    SET_CONFIG(g_verbose, "MA_VERBOSE");

}

#ifndef BUILD_AS_STATIC_LIB
void ma_detructor(void) __attribute__ ((destructor));
#endif
void ma_destructor(void)
{

#ifndef DISABLE_TELNET_SERVER
    cleanupSockets(0);
#endif

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
    free_trace_info();
#endif
}

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
static void record_call_stack(enum Alloctype type, unsigned int addr, unsigned int size);
static void remove_call_stack(unsigned int addr, unsigned int size);
#endif

void register_own_pointers()
{
#ifndef ONLY_SUPPORT_TOTAL_SMAPS
    record_call_stack(Alloctype_Own, (unsigned int) g_vec, sizeof(unsigned char) * ( MAX_CHUNK_SIZE / 4 / 1024 ));
    record_call_stack(Alloctype_Own, (unsigned int) g_owner, sizeof(char) * MAX_OWNER_STRING_LEN);
    record_call_stack(Alloctype_Own, (unsigned int) g_owners, sizeof(MA_OWNERS) * MAX_OWNERS_IN_RSS_CHUNK);
    record_call_stack(Alloctype_Own, (unsigned int) g_trace, sizeof(TRACES) * MAX_TRACE_ENTRIES);
    record_call_stack(Alloctype_Own, (unsigned int) g_maps_entries, sizeof(MAPS_DATA) * MAX_MAPS_ENTRIES);
    record_call_stack(Alloctype_Own, (unsigned int) g_callstacks, sizeof(char) * MAX_TOTAL_CALLSTACK_LENGTH);
#ifdef ENABLE_RB_TREE
    record_call_stack(Alloctype_Own, g_rb_mem_pool_addr, g_rb_mem_pool_size);
#endif
#endif
}

#ifndef BUILD_AS_STATIC_LIB
void ma_constructor(void) __attribute__ ((constructor));
#endif
void ma_constructor(void)
{

    DPF("[MemAnalyzer] constructor called from pid = %d\n", getpid());

    pthread_mutex_init(&g_mutex, NULL);

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
    pthread_mutex_init(&g_trace_mutex, NULL);
    pthread_mutex_init(&g_record_mutex, NULL);
    pthread_mutex_lock(&g_mutex);
#endif
    if ( g_initialized == 0 ) {
#ifndef ONLY_SUPPORT_TOTAL_SMAPS
        g_mmap = (void*(*)(void *,size_t,int,int,int,off_t)) dlsym(RTLD_NEXT, "mmap");
        g_mmap64 = (void*(*)(void *,size_t,int,int,int,off64_t)) dlsym(RTLD_NEXT, "mmap64");
        g_munmap = (int(*)(void *,size_t)) dlsym(RTLD_NEXT, "munmap");
#endif
        set_configuration();

        RB_INIT(&g_rb_tree_ptr);
        MY_MALLOC_INIT(&g_rb_mem_pool_addr, &g_rb_mem_pool_size); // must be called before register_own_pointers

        register_own_pointers();

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
        DPF("sizeof(g_vec) = %d @ 0x%08x-0x%08x\n", sizeof(g_vec), (unsigned int) g_vec, (unsigned int) g_vec + sizeof(g_vec));
        DPF("sizeof(g_owner) = %d @ 0x%08x-0x%08x\n", sizeof(g_owner), (unsigned int) g_owner, (unsigned int) g_owner + sizeof(g_owner));
        DPF("sizeof(g_owners) = %d @ 0x%08x-0x%08x\n", sizeof(g_owners), (unsigned int) g_owners, (unsigned int) g_owners + sizeof(g_owners));
        DPF("sizeof(g_trace) = %d @ 0x%08x-0x%08x\n", sizeof(g_trace), (unsigned int) g_trace, (unsigned int) g_trace + sizeof(g_trace));
#endif
        DPF("sizeof(g_maps_entries) = %d @ 0x%08x-0x%08x\n", sizeof(g_maps_entries), (unsigned int) g_maps_entries, (unsigned int) g_maps_entries + sizeof(g_maps_entries));

        g_initialized++;
        g_record_enabled = 1;

#ifndef DISABLE_TELNET_SERVER
        // start the telnet server thread
        pthread_attr_init(&g_thread_attr);
        pthread_attr_setstacksize(&g_thread_attr, g_thread_stack_size);

        pthread_create(&g_telnet_thread, NULL, &run_telnet_server, NULL);
#endif
    }

    pthread_mutex_unlock(&g_mutex);

}

#ifndef ONLY_SUPPORT_TOTAL_SMAPS
void ma_print_call_stack(int print_freed_ones, unsigned int begin, unsigned int end, int printf_only)
{
    int i, i_max = g_count;
    DPF5("g_count = %d\n", g_count);
    //pthread_mutex_lock(&g_trace_mutex);
    for ( i = 0; i < i_max; i++ ) {
        if ( ( (begin == 0) || ( ( begin <= g_trace[i].start_addr) && ( end > g_trace[i].start_addr) ) ) &&
             ( (end   == 0) || ( g_trace[i].start_addr < end ) )
           ) {

            if ( print_freed_ones || g_trace[i].freed == 0 ){
                if ( printf_only ) {
                    printf("[ %5d] %10d @ 0x%08x : %s ( %s ) - %s\n", i, g_trace[i].size, g_trace[i].start_addr, g_trace[i].callstack, g_alloc_type_string[g_trace[i].type], g_trace[i].freed?"freed":"not_freed");
                } else {
                    MY_PRINT("[ %5d] %10d @ 0x%08x : %s ( %s ) - %s\n", i, g_trace[i].size, g_trace[i].start_addr, g_trace[i].callstack, g_alloc_type_string[g_trace[i].type], g_trace[i].freed?"freed":"not_freed");
                }
            }
        }
    }
    //pthread_mutex_unlock(&g_trace_mutex);
}


static void record_call_stack(enum Alloctype type, unsigned int addr, unsigned int size)
{
    TRACES* p_trace_top = g_trace;
    TRACES* p_trace;
    int idx;

    assert(g_count < MAX_TRACE_ENTRIES);

    pthread_mutex_lock(&g_trace_mutex);
    idx = g_count;
    ma_dump_callstack(idx);
    g_count++;
//    pthread_mutex_unlock(&g_trace_mutex);

    p_trace = &p_trace_top[idx];

    // set
    p_trace->type = type;
    p_trace->start_addr = addr;
    p_trace->size = size;
    p_trace->freed = 0;

    RB_INSERT( g_rb_tree_ptr, addr, (void*) p_trace );

    pthread_mutex_unlock(&g_trace_mutex);

}

static TRACES* find_entry(unsigned int addr)
{
    TRACES* p_trace = NULL;
#ifdef ENABLE_RB_TREE
    RB_NODE* entry = rb_search(g_rb_tree_ptr, addr);
    if ( entry != NULL ) {
        p_trace = (TRACES*) entry->list.top->ptr;
    }
#else // ENABLE_RB_TREE
    TRACES* p_trace_top = g_trace;
    int idx, idx_max;

    //pthread_mutex_lock(&g_trace_mutex);
    idx_max = g_count;
    //pthread_mutex_unlock(&g_trace_mutex);

    for ( idx = 0 ; idx < idx_max ; idx++ ) {
        // skip freed ones
        if ( p_trace_top[idx].freed ) continue;

        if ( p_trace_top[idx].start_addr == addr ) {
            p_trace = &p_trace_top[idx];
        }
    }
#endif // ENABLE_RB_TREE
    return p_trace;

}

// this function does not a work properly for munmap...
static void remove_call_stack(unsigned int addr, unsigned int size){
    TRACES* p_trace;
    if ( addr == (unsigned int) NULL ) return;

    //pthread_mutex_lock(&g_trace_mutex);
    p_trace = find_entry ( addr );
    //pthread_mutex_unlock(&g_trace_mutex);

    if ( p_trace != NULL ) {
        if ( size == 0 ) {
            p_trace->freed = 1;
            g_freed_cnt++;
        } else {
            if (p_trace->size <= size ) {
                // reaches here
                p_trace->freed = 1;
                g_freed_cnt++;
            } else {
                // reaches here with nrdp3.2...
                //printf("kita!!!! : size = %d : outof %d\n", size, p_trace->size);
                p_trace->size -= size;
                p_trace->start_addr += size;
                //exit(1);
            }
        }
    }
}


__inline int should_record(size_t size) { return (g_record_enabled && size > g_malloc_trace_th); }

void * malloc(size_t size)
{
    void * p = NULL;

    DPF3("malloc called with size = %d\n", size);

    if (size) { p = __libc_malloc(size); }

    DPF3("malloc returned 0x%x (%d)\n", (unsigned int) p, size);

    MY_MLOCK((__const void *) p, size);

    if ( should_record(size) ) {
        record_call_stack(Alloctype_Malloc, (unsigned int) p, size);
    }

    return p;
}
#ifdef RECORD_FREE
void free(void *ptr)
{
    __libc_free(ptr);

    if ( g_record_enabled ) {
        remove_call_stack((unsigned int) ptr, 0);
    }
}
#endif

void* calloc(size_t num, size_t size)
{
    void * p = NULL;

    DPF3("calloc called with size = %d\n", size);

    if (size) { p = __libc_calloc(num, size); }
    MY_MLOCK((__const void *) p, size);

    DPF3("calloc returned 0x%x (%d)\n",(unsigned int)  p, size);

    if ( should_record(size) ) {
        record_call_stack(Alloctype_Calloc, (unsigned int) p, size);
    }

    return p;
}


void* realloc(void* ptr, size_t size)
{
    void * p = NULL;

    DPF3("ralloc called with size = %d\n", size);

    if (size) { p = __libc_realloc(ptr, size); }

    DPF3("ralloc returned 0x%x (%d)\n", (unsigned int) p, size);

    MY_MLOCK((__const void *) p, size);

    if ( g_record_enabled ) {
#if 0
        if ( size > 1024 ) { // [HACK] So many chunks get realloced in resource cache (DataBuffer) ~= 8KB
            adjust_call_stacks(g_trace, "ralloc", (unsigned int) ptr, (unsigned int) p, size);
        }
#else
        if ( size > g_malloc_trace_th ) {
//        if ( size > 6 * 1024 ) { // [HACK] So many chunks get realloced in resource cache (DataBuffer) ~= 8KB
            remove_call_stack((unsigned int) ptr, 0);
            record_call_stack(Alloctype_Realloc, (unsigned int) p, size);
        }
#endif
    }

    return p;
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    void * p = NULL;

    DPF3("mmap called with size %d\n", length);

    p = g_mmap(addr,length,prot,flags,fd,offset);
    //p = __libc_mmap(addr,length,prot,flags,fd,offset); // this does not work..

    DPF3("mmap returned 0x%x (%d)\n", (unsigned int) p, length);

    MY_MLOCK((__const void *) p, length);

    if ( g_record_enabled ) {
        record_call_stack(Alloctype_Mmap, (unsigned int) p, length);
    }

    return p;
}

void *mmap64(void *addr, size_t length, int prot, int flags, int fd, off64_t offset)
{
    void * p = NULL;

    DPF3("mmap called with size %d\n", length);

    p = g_mmap64(addr,length,prot,flags,fd,offset);
    //p = __libc_mmap64(addr,length,prot,flags,fd,offset); // this does not work..

    DPF3("mmap returned 0x%x (%d)\n", (unsigned int) p, length);

    MY_MLOCK((__const void *) p, length);

    if ( g_record_enabled ) {
        record_call_stack(Alloctype_Mmap64, (unsigned int) p, length);
    }

    return p;
}

int munmap(void *addr, size_t len)
{
    int result;

    DPF3("munmap called with size %d\n", len);

    result = g_munmap(addr,len);

    DPF3("mummap returned 0x%x (%d)\n", (unsigned int) addr, len);

    if ( g_record_enabled ) {
        remove_call_stack((unsigned int) addr, len);
    }

    return result;

}

void * alloca(size_t size)
{
    void * p = NULL;

    printf("alloca is not supported (size = %d)\n", size);
    exit(1);

    if (size) { p = __libc_alloca(size); }

    mlock ((__const void *) p, size);

    if ( g_record_enabled && size > g_malloc_trace_th) {
        record_call_stack(Alloctype_Alloca, (unsigned int) p, size);
    } else {
    }

    return p;
}

#endif // ONLY_SUPPORT_TOTAL_SMAPS

