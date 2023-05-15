//#define KHRN_HW_MEASURE_BIN_TIME
//#define KHRN_HW_MEASURE_RENDER_TIME
//#define KHRN_HW_BIN_HISTORY_N 1024 /* should be a multiple of 2 */
//#define KHRN_HW_RENDER_HISTORY_N 1024 /* should be a multiple of 2 */
//#define KHRN_HW_BOOM /* for testing the can't-allocate-required-overflow-memory case */
//#define KHRN_HW_QPU_PROFILING_DUMP_PRE
//#define KHRN_HW_QPU_PROFILING_RESET_PRE
//#define KHRN_HW_QPU_PROFILING_DUMP_POST
//#define KHRN_HW_QPU_PROFILING_RESET_POST

/*=============================================================================
Copyright (c) 2009 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Common hardware/simpenrose interface

FILE DESCRIPTION
Drivers use this interface to interact with hardware/simpenrose.
=============================================================================*/

/* if this is defined then we never submit to v3d, but instead pretend that the driver completed instantaneously */
//#define DONT_SUBMIT_TO_V3D
//#define DONT_SUBMIT_TO_V3D_CALL_ISR_DIRECT
#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/common/khrn_options.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"
#include "middleware/khronos/common/2708/khrn_pool_4.h"
#include "middleware/khronos/common/2708/khrn_worker_4.h"
#include "middleware/khronos/common/khrn_hw.h"
#include "middleware/khronos/common/khrn_llat.h"
#include "middleware/khronos/common/khrn_mem.h"
#include "interface/khronos/common/khrn_int_util.h"
#include "middleware/khronos/egl/egl_disp.h"
#ifdef PRINT_CLIST_DEBUG
#include "middleware/khronos/common/2708/khrn_cle_debug_4.h"
#endif

#include "interface/khronos/include/EGL/egl.h"
#include "vcfw/drivers/chip/abstract_v3d.h"

#include <stddef.h> /* for offsetof */
#include <stdio.h>
#ifdef __mips__
#define __USE_GNU
#include <dlfcn.h>
#endif /* __mips__ */

#ifdef ANDROID
#include <cutils/log.h>
#endif

uint32_t s_debugLastBinStart = 0;
uint32_t s_debugLastRenderStart = 0;

/******************************************************************************
hw fifo
******************************************************************************/

/* todo: waits in the fifo can mean the hw is idle even if there's something in
 * the fifo that could be run. does this matter in practice? */

#ifdef WIN32
static uint32_t s_bin_mem_size = 16 * 1024 * 1024;
#else

/* reasonable size for an HD display */
/* in full tiles, but taken as multisample resolution */
#define WIDTH_IN_MS_TILES    (((1920 + (KHRN_HW_TILE_WIDTH  - 1)) / KHRN_HW_TILE_WIDTH  ) * 2)
#define HEIGHT_IN_MS_TILES   (((1080 + (KHRN_HW_TILE_HEIGHT - 1)) / KHRN_HW_TILE_HEIGHT ) * 2)
/* rounded up to 128kb */
#define ROUND_SIZE 100
static uint32_t s_bin_mem_size = 
   (((WIDTH_IN_MS_TILES * HEIGHT_IN_MS_TILES * (KHRN_HW_CL_BLOCK_SIZE_MIN + KHRN_HW_TILE_STATE_SIZE)) + ((ROUND_SIZE * 1024) - 1)) & ~((ROUND_SIZE * 1024) - 1));
#endif

/* we'll try not to allocate many more bin mems than this */
static uint32_t s_bin_mems_n_max = 64;

static bool s_bin_memory_preallocated = false;

/* todo: these assumptions aren't very nice. we use the top of the initial bin
 * mem for tile states and the bottom for initial tile control lists. we can't
 * trash the tile states because the ptb will go horribly wrong if we do */
#define BIN_MEM_TRASHABLE_SIZE ((s_bin_mem_size * KHRN_HW_CL_BLOCK_SIZE_MIN) / (KHRN_HW_CL_BLOCK_SIZE_MIN + KHRN_HW_TILE_STATE_SIZE))

#define MSGS_N 16 /* todo: what should this be? */

typedef enum {
   MSG_DISPLAY_STATE_NOT_DONE,
   MSG_DISPLAY_STATE_DOING,
   MSG_DISPLAY_STATE_DONE_NONE,
   MSG_DISPLAY_STATE_DONE
} MSG_DISPLAY_STATE_T;

typedef struct {
   /* wait for (khrn_worker_get_exit_pos() >= wait_worker_pos) before submitting render */
   uint64_t wait_worker_pos;

   /* wait for !egl_disp_still_on(wait_display_disp_handle, wait_display_pos) before submitting render */
   EGL_DISP_HANDLE_T wait_display_disp_handle; /* EGL_DISP_HANDLE_INVALID means don't wait */
   uint32_t wait_display_pos;

   /* if callback is non-NULL, all of this is valid. otherwise, all of this is
    * invalid */
   bool ready; /* wait for ready before preparing bin or render */
   bool ok; /* this is set to false when we run out of memory. this can happen in khrn_hw_ready, prepare_bin, or supply_overflow */
   bool ok_prepare; /* were we ok in prepare_bin? (if so, we locked the cmem and fixer stuff...) */
   uint8_t *bin_begin, *bin_end, *render_begin, *render_end;
   KHRN_HW_CC_FLAG_T bin_cc, render_cc;
   uint32_t render_n; /* isr will update */
   uint32_t special_0;
   uint32_t bin_mem_size_min; /* initial bin mem (available through fixer) must be at least this big */
   uint32_t actual_user_vpm;
   uint32_t max_user_vpm;
   KHRN_HW_TYPE_T type;
   KHRN_HW_CALLBACK_T callback;
   uint32_t callback_data[8];
   MEM_HANDLE_T bin_mems_head; /* tail is the initial bin mem (possibly not allocated from the pool). use set_bin_mems_next/get_bin_mems_next to manipulate ll */
   void *bin_mem; /* the initial bin mem */

   EGL_DISP_SLOT_HANDLE_T display_slot_handle; /* EGL_DISP_SLOT_HANDLE_INVALID means display nothing */
   volatile MSG_DISPLAY_STATE_T display_state;
#ifdef KHRN_STATS_ENABLE
   uint32_t stats_bin_time;
   uint32_t stats_render_time;
#endif
} MSG_T;

static MEM_HANDLE_T null_render_handle;

#ifdef DONT_SUBMIT_TO_V3D
static MEM_HANDLE_T nosubmit_bin_handle;
static MEM_HANDLE_T nosubmit_binbuff_handle;
#endif

static KHRN_POOL_T bin_mem_pool; /* only used by the llat thread (so no need for locks etc) */
static uint32_t bin_mems_n = 0; /* how many bin mems we currently have allocated from the pool */

static MSG_T msgs[MSGS_N];

static struct {
   MSG_T
      /* last message with wait info filled + 1 (<= (post + 1)) (updated by master task) */
      *wait,
      /* last message posted + 1 (<= wait) (updated by master task, read by llat task) */
      *post,
      /* last message locked in preparation for binning + 1 (<= post) (updated by llat task) */
      *bin_prepared,
      /* last message locked in preparation for rendering + 1 (<= bin_prepared) (updated by llat task) */
      *render_prepared,
      /* last message submitted to binner + 1 (<= bin_prepared, <= (bin_done + 1)) (updated by llat task) */
      *bin_submitted,
      /* last message submitted to renderer + 1 (<= render_prepared, <= (render_done + 1), <= bin_submitted) (updated by llat task) */
      *render_submitted,
      /* last message completed binning + 1 (<= bin_submitted) (updated by isr, read by llat task) */
      *bin_done,
      /* last message completed rendering + 1 (<= render_submitted, <= bin_done) (updated by isr, read by llat task) */
      *render_done,
      /* last message cleaned up on llat task after binning + 1 (<= bin_done) (updated by llat task, read by master task) */
      *bin_cleanup_llat,
      /* last message cleaned up after binning + 1 (<= bin_cleanup_llat) (updated by master task) */
      *bin_cleanup,
      /* last message cleaned up on llat task after rendering + 1 (<= render_done, <= bin_cleanup_llat) (updated by llat task, read by master task) */
      *render_cleanup_llat,
      /* last message cleaned up after rendering + 1 (<= render_cleanup_llat, <= bin_cleanup) (updated by master task) */
      *render_cleanup,
      /* last message submitted to display + 1 (<= render_done) (updated by llat task, read by master task) */
      *display,
      /* last message completed + 1 (<= render_cleanup, <= display) (updated by master task) */
      *done;
} khrn_prod_msg;

/* fifo pos stuff for interlocking with other fifos... */
uint64_t khrn_hw_enter_pos;
uint64_t khrn_hw_bin_exit_pos;
uint64_t khrn_hw_render_exit_pos;

static bool bin_prepared = false, render_prepared = false, fixup_done;
static void *bin_begin, *bin_end, *render_begin, *render_end;

#ifdef PRINT_CLIST_DEBUG
static void *dbg_render_begin, *dbg_render_end;
#endif

static uint32_t bins_submitted, renders_submitted; /* used by llat task */
static uint32_t bins_done, renders_done; /* used by isr */

static volatile bool want_overflow, cancel_want_overflow;

#ifdef KHRN_HW_MEASURE_BIN_TIME
static uint32_t measure_bin_start = 0;
static uint32_t measure_bin_end = 0;
#endif
#ifdef KHRN_HW_MEASURE_RENDER_TIME
static uint32_t measure_render_start = 0;
static uint32_t measure_render_end = 0;
#endif

#ifdef KHRN_HW_BIN_HISTORY_N
uint32_t khrn_hw_bin_history[KHRN_HW_BIN_HISTORY_N] = {0};
uint32_t khrn_hw_bin_history_n = KHRN_HW_BIN_HISTORY_N;
static uint32_t bin_history_i = 0;
#endif
#ifdef KHRN_HW_RENDER_HISTORY_N
uint32_t khrn_hw_render_history[KHRN_HW_RENDER_HISTORY_N] = {0};
uint32_t khrn_hw_render_history_n = KHRN_HW_RENDER_HISTORY_N;
static uint32_t render_history_i = 0;
#endif

static uint32_t llat_i;

static VCOS_MUTEX_T  backtrace_mutex;

static KHRN_DRIVER_COUNTERS_T *s_perf_counters = 0;

static void reset_perf_counters(void);
static void update_perf_counters(void);

static MEM_HANDLE_T bin_mem_pool_alloc(bool fail_ok)
{
   MEM_HANDLE_T handle = khrn_pool_alloc(&bin_mem_pool, fail_ok);
   if (handle != MEM_INVALID_HANDLE) { ++bin_mems_n; }
   return handle;
}

static void bin_mem_pool_free(MEM_HANDLE_T handle)
{
   --bin_mems_n;
   khrn_pool_free(&bin_mem_pool, handle);
}

static INLINE void set_bin_mems_next(MEM_HANDLE_T handle, MEM_HANDLE_T next_handle)
{
   mem_set_term(handle, (MEM_TERM_T)(uintptr_t)next_handle);
}

static INLINE MEM_HANDLE_T get_bin_mems_next(MEM_HANDLE_T handle)
{
   return (MEM_HANDLE_T)(uintptr_t)mem_get_term(handle);
}

/* keep a track of what we are processing */
static VCOS_ATOMIC_FLAGS_T irq_flags;

VCOS_MUTEX_T enter_exit_protection;

static INLINE MSG_T *prev_msg(MSG_T *msg)
{
   return (msg == msgs) ? (msgs + (MSGS_N - 1)) : (msg - 1);
}

static INLINE MSG_T *next_msg(MSG_T *msg)
{
   return (msg == (msgs + (MSGS_N - 1))) ? msgs : (msg + 1);
}

static INLINE void advance_msg(MSG_T **msg) /* *msg is read by another task */
{
   khrn_barrier();
   *msg = next_msg(*msg);
}

static INLINE bool more_msgs(MSG_T *msg_a, MSG_T *msg_b) /* msg_b is updated by another task */
{
   if (msg_a != msg_b) {
      khrn_barrier();
      return true;
   }
   return false;
}

static INLINE bool processed_msgs(MSG_T *msg_a, MSG_T *msg_b) /* msg_a is updated by another task */
{
   if (msg_a == msg_b) {
      khrn_barrier();
      return true;
   }
   return false;
}

static void acquire_callback_no_args(void);
static void khrn_hw_isr(uint32_t flags);
void khrn_hw_isr_no_args(void);
static void khrn_hw_llat_callback(void);

bool khrn_hw_init(void)
{
   uint8_t *begin, *p;
#ifdef DONT_SUBMIT_TO_V3D
   uint8_t *nosubmit_bin_begin;
#endif

#define NULL_RENDER_SIZE 22

   /* Get memory heap information */
   {
      BEGL_DriverInterfaces *iface = BEGL_GetDriverInterfaces();
      uint32_t              memStartPhys = 0;
      uint32_t              memEndPhys = 0;
      uint32_t              banksCovered = 0;   /* How many 256MB banks are included? */
      
      if (iface && iface->memInterface && iface->memInterface->GetInfo)
      {
         memStartPhys = iface->memInterface->GetInfo(iface->memInterface->context, BEGL_MemHeapStartPhys);
         memEndPhys = iface->memInterface->GetInfo(iface->memInterface->context, BEGL_MemHeapEndPhys);
      }
      
      if (memEndPhys - memStartPhys > 0)
         banksCovered = 1 + ((0xF0000000 & memEndPhys) >> 28) - ((0xF0000000 & memStartPhys) >> 28);
      
      /* If only one 256MB bank is used, we can dynamically allocate bin memory like we used to.
         If memory can span multiple banks, we need to preallocate all the bin memory. */
      if (banksCovered != 1)
      {
         /* It's possible that our binner memory might come from multiple 256MB banks, so we will have to 
            preallocate and make sure that doesn't happen */
         s_bin_mem_size = 340 * 1024;    /* Large enough for 2048x2048 multi-sampled bin state */
         s_bin_mems_n_max = 20;          /* 6.6MB of bin memory - no more, no less. Hope it's enough! */
         s_bin_memory_preallocated = true;

         if (memEndPhys - memStartPhys > 256 * 1024 * 1024)
#ifdef ANDROID
            LOGD("Warning: The graphics heap is larger than 256MB.\n"
                   "         This has memory & performance implications for OpenGL ES rendering.\n");
#else
            printf("Warning: The graphics heap is larger than 256MB.\n"
                   "         This has memory & performance implications for OpenGL ES rendering.\n");
#endif
         else
#ifdef ANDROID
            LOGD("Warning: The graphics heap straddles a 256MB memory boundary.\n"
                   "         This has memory & performance implications for OpenGL ES rendering.\n");
#else
            printf("Warning: The graphics heap straddles a 256MB memory boundary.\n"
                   "         This has memory & performance implications for OpenGL ES rendering.\n");
#endif
      }
   }

   vcos_mutex_create(&backtrace_mutex, "backtrace_mutex");

#if defined WIN32 && !defined CARBON
   /* Override for windows simulator */
   s_bin_mem_size = 16 * 1024 * 1024;
   s_bin_mems_n_max = 64;
   s_bin_memory_preallocated = false;
#endif

#ifndef DISABLE_OPTION_PARSING
   /* Override if set in environment */
   if (khrn_options.bin_block_size != 0)
      s_bin_mem_size = khrn_options.bin_block_size;
   if (khrn_options.max_bin_blocks != 0)
      s_bin_mems_n_max = khrn_options.max_bin_blocks;
#endif

   /* todo: these assumptions aren't very nice. we use the top of the initial bin
    * mem for tile states and the bottom for initial tile control lists. we can't
    * trash the tile states because the ptb will go horribly wrong if we do */
   vcos_assert(BIN_MEM_TRASHABLE_SIZE >= KHRN_HW_BIN_MEM_GRANULARITY); /* use as overflow memory */

   null_render_handle = mem_alloc_ex(NULL_RENDER_SIZE, 1, (MEM_FLAG_T)(MEM_FLAG_DIRECT | MEM_FLAG_NO_INIT), "khrn_hw_null_render", MEM_COMPACT_DISCARD);
   if (null_render_handle == MEM_INVALID_HANDLE) {
      return false;
   }
   begin = (uint8_t *)mem_lock(null_render_handle);
   p = begin;
   add_byte(&p, KHRN_HW_INSTR_STATE_TILE_RENDERING_MODE);
#define NULL_RENDER_FRAME_OFFSET 1
   vcos_assert((begin + NULL_RENDER_FRAME_OFFSET) == p);
   ADD_WORD(p, 0); /* frame */
   ADD_SHORT(p, KHRN_HW_TILE_WIDTH);
   ADD_SHORT(p, KHRN_HW_TILE_HEIGHT);
   add_byte(&p, (uint8_t)(
      (0 << 0) | /* disable ms mode */
      (0 << 1) | /* disable 64-bit color mode */
      (1 << 2) | /* 32-bit rso */
      (0 << 4))); /* 1x decimate (ie none) */
   add_byte(&p, 0); /* unused */
   add_byte(&p, KHRN_HW_INSTR_WAIT_SEMAPHORE);
#define NULL_RENDER_POST_WAIT_SEMAPHORE_OFFSET 12
   vcos_assert((begin + NULL_RENDER_POST_WAIT_SEMAPHORE_OFFSET) == p);
   add_byte(&p, KHRN_HW_INSTR_STATE_TILE_COORDS);
   add_byte(&p, 0);
   add_byte(&p, 0);
   add_byte(&p, KHRN_HW_INSTR_STORE_GENERAL);
   add_byte(&p, 0); /* no buffer */
   add_byte(&p, 0xe0); /* don't clear any buffers */
   ADD_WORD(p, 1 << 3); /* eof */

   vcos_assert((begin + NULL_RENDER_SIZE) == p);
   mem_unlock(null_render_handle);


#ifdef DONT_SUBMIT_TO_V3D
#define NOSUBMIT_BINBUFF_SIZE 8192 + KHRN_HW_TILE_STATE_SIZE
   nosubmit_binbuff_handle = mem_alloc_ex(NOSUBMIT_BINBUFF_SIZE, 16, (MEM_FLAG_T)(MEM_FLAG_DIRECT | MEM_FLAG_NO_INIT), "khrn_nosubmit_binbuff_handle", MEM_COMPACT_DISCARD);
   if (nosubmit_binbuff_handle == MEM_INVALID_HANDLE) {
      return false;
   }
   nosubmit_bin_begin = (uint8_t *)mem_lock(nosubmit_binbuff_handle);

#define NOSUBMIT_BIN_SIZE 19

   nosubmit_bin_handle = mem_alloc_ex(NOSUBMIT_BIN_SIZE, 1, (MEM_FLAG_T)(MEM_FLAG_DIRECT | MEM_FLAG_NO_INIT), "khrn_nosubmit_bin_handle", MEM_COMPACT_DISCARD);
   if (nosubmit_bin_handle == MEM_INVALID_HANDLE) {
      return false;
   }
   begin = (uint8_t *)mem_lock(nosubmit_bin_handle);
   p = begin;

   add_byte(&p, KHRN_HW_INSTR_STATE_TILE_BINNING_MODE);

   add_word(&p, khrn_hw_addr(nosubmit_bin_begin));
   add_word(&p, NOSUBMIT_BINBUFF_SIZE - KHRN_HW_TILE_STATE_SIZE);
   add_word(&p, khrn_hw_addr(nosubmit_bin_begin + NOSUBMIT_BINBUFF_SIZE - KHRN_HW_TILE_STATE_SIZE));

   add_byte(&p, 1);
   add_byte(&p, 1);
   add_byte(&p,
         0<<0   |    /* Multisample mode */
         0<<1   |    /* 64-bit */
         1<<2   |    /* Auto-initialise tile state data array */
         0<<3   |    /* Tile allocation initial block size (32 bytes) */
         2<<5);      /* Tile allocation block size (32 bytes) */

   add_byte(&p, KHRN_HW_INSTR_START_TILE_BINNING);

   add_byte(&p, KHRN_HW_INSTR_INCR_SEMAPHORE);

   add_byte(&p, KHRN_HW_INSTR_FLUSH);

   vcos_assert((begin + NOSUBMIT_BIN_SIZE) == p);

   mem_unlock(nosubmit_bin_handle);
   mem_unlock(nosubmit_binbuff_handle);
#endif /* DONT_SUBMIT_TO_V3D */

   khrn_pool_init(&bin_mem_pool, s_bin_memory_preallocated,
      s_bin_mem_size, KHRN_HW_BIN_MEM_ALIGN, s_bin_mems_n_max,
      MEM_FLAG_DIRECT, "khrn_hw_bin_mem");

   vcos_atomic_flags_create(&irq_flags);

   /* enter exit pos protection */
   vcos_mutex_create(&enter_exit_protection, "enter_exit_protection");

   khrn_prod_msg.wait = msgs;
   khrn_prod_msg.post = msgs;
   khrn_prod_msg.bin_prepared = msgs;
   khrn_prod_msg.render_prepared = msgs;
   khrn_prod_msg.bin_submitted = msgs;
   khrn_prod_msg.render_submitted = msgs;
   khrn_prod_msg.bin_done = msgs;
   khrn_prod_msg.render_done = msgs;
   khrn_prod_msg.bin_cleanup_llat = msgs;
   khrn_prod_msg.bin_cleanup = msgs;
   khrn_prod_msg.render_cleanup_llat = msgs;
   khrn_prod_msg.render_cleanup = msgs;
   khrn_prod_msg.display = msgs;
   khrn_prod_msg.done = msgs;

   prev_msg(khrn_prod_msg.post)->display_slot_handle = (EGL_DISP_SLOT_HANDLE_T)(EGL_DISP_SLOT_HANDLE_INVALID + 1); /* for khrn_hw_queue_display */

   khrn_hw_enter_pos = 0;
   khrn_hw_bin_exit_pos = 0;
   khrn_hw_render_exit_pos = 0;

   llat_i = khrn_llat_register(khrn_hw_llat_callback);
   vcos_assert(llat_i != (uint32_t)-1);

   return true;
}

void khrn_hw_term(void)
{
   vcos_assert(khrn_prod_msg.done == khrn_prod_msg.wait); /* should have called khrn_hw_wait or equivalent before this */

   khrn_llat_unregister(llat_i);

   khrn_pool_term(&bin_mem_pool);

   mem_release(null_render_handle);

   vcos_mutex_delete(&backtrace_mutex);
   vcos_atomic_flags_delete(&irq_flags);
   vcos_mutex_delete(&enter_exit_protection);

#ifdef DONT_SUBMIT_TO_V3D
   mem_release(nosubmit_binbuff_handle);
   mem_release(nosubmit_bin_handle);
#endif /* DONT_SUBMIT_TO_V3D */
}

/*
   stuff called from the master task
*/

static void msg_init_wait(MSG_T *msg)
{
   msg->wait_worker_pos = 0;
   msg->wait_display_disp_handle = EGL_DISP_HANDLE_INVALID;
}

static void msg_init_nop(MSG_T *msg)
{
   msg->callback = NULL;

   msg->display_slot_handle = EGL_DISP_SLOT_HANDLE_INVALID;
   msg->display_state = MSG_DISPLAY_STATE_NOT_DONE;
}

void *khrn_hw_queue(
   uint8_t *bin_begin, uint8_t *bin_end, KHRN_HW_CC_FLAG_T bin_cc,
   uint8_t *render_begin, uint8_t *render_end, KHRN_HW_CC_FLAG_T render_cc, uint32_t render_n,
   uint32_t special_0, /* used for extra thrsw removing hack. todo: no need for this hack on b0 */
   uint32_t bin_mem_size_min,
   uint32_t actual_user_vpm, uint32_t max_user_vpm,
   KHRN_HW_TYPE_T type,
   KHRN_HW_CALLBACK_T callback,
   uint32_t callback_data_size)
{
   void *callback_data;
   UNUSED_NDEBUG(callback_data_size);

   vcos_assert(callback);

   if (khrn_prod_msg.post == khrn_prod_msg.wait) {
      while (next_msg(khrn_prod_msg.wait) == khrn_prod_msg.done) {
         khrn_sync_master_wait_timeout(-1);
      }
      msg_init_wait(khrn_prod_msg.wait);
      khrn_prod_msg.wait = next_msg(khrn_prod_msg.wait);
   }

   khrn_prod_msg.post->ready = false;
   /* fill in msg_post->ok in khrn_hw_ready */
   /* fill in msg_post->ok_prepare in prepare_bin */
   khrn_prod_msg.post->bin_begin = bin_begin;
   khrn_prod_msg.post->bin_end = bin_end;
   khrn_prod_msg.post->bin_cc = bin_cc;
   khrn_prod_msg.post->render_begin = render_begin;
   khrn_prod_msg.post->render_end = render_end;
   khrn_prod_msg.post->render_cc = render_cc;
   khrn_prod_msg.post->render_n = render_n;
   khrn_prod_msg.post->special_0 = special_0;
   khrn_prod_msg.post->bin_mem_size_min = bin_mem_size_min
      /* the hw rounds up after the initial control lists to the regular alloc
       * size. in the worst case we need this many bytes extra just to leave
       * effectively no memory for the regular control list blocks (if we leave
       * negative memory, things go badly wrong) */
      + (KHRN_HW_CL_BLOCK_SIZE_MAX - KHRN_HW_CL_BLOCK_SIZE_MIN)
      /* plus 2 blocks for the binner to work with until we give it some
       * overflow memory. todo: really? */
      + (2 * KHRN_HW_BIN_MEM_GRANULARITY);
   khrn_prod_msg.post->actual_user_vpm = actual_user_vpm;
   khrn_prod_msg.post->max_user_vpm = max_user_vpm;
   khrn_prod_msg.post->type = type;
   khrn_prod_msg.post->callback = callback;
   callback_data = khrn_prod_msg.post->callback_data;
   vcos_assert(callback_data_size <= sizeof(khrn_prod_msg.post->callback_data));
   /* fill in khrn_prod_msg.post->bin_mems_head/one_too_many and khrn_prod_msg.post->bin_mem in prepare_bin */
   khrn_prod_msg.post->display_slot_handle = EGL_DISP_SLOT_HANDLE_INVALID;
   khrn_prod_msg.post->display_state = MSG_DISPLAY_STATE_NOT_DONE;
   advance_msg(&khrn_prod_msg.post);
   /* don't khrn_hw_notify_llat() yet -- do that when it's ready */

   return callback_data;
}

void khrn_hw_queue_wait_for_worker(uint64_t pos)
{
   /*
      create a new wait message if we don't already have one
   */

   if (khrn_prod_msg.post == khrn_prod_msg.wait) {
      while (next_msg(khrn_prod_msg.wait) == khrn_prod_msg.done) { /* wait until there's room */
         khrn_sync_master_wait_timeout(-1);
      }
      msg_init_wait(khrn_prod_msg.wait);
      khrn_prod_msg.wait = next_msg(khrn_prod_msg.wait);
   }

   /*
      we have an unposted wait message
   */

   vcos_assert(next_msg(khrn_prod_msg.post) == khrn_prod_msg.wait);
   if (pos > khrn_prod_msg.post->wait_worker_pos) {
      khrn_prod_msg.post->wait_worker_pos = pos;
   }
}

void khrn_hw_queue_wait_for_display(EGL_DISP_HANDLE_T disp_handle, uint32_t pos)
{
   /*
      if we've got an unposted wait message but it already has a display wait,
      we need to create another wait message for this display wait. we keep at
      most one unposted wait message, so we need to post the current wait
      message before we can create another
   */

   if ((khrn_prod_msg.post != khrn_prod_msg.wait) &&
      (khrn_prod_msg.post->wait_display_disp_handle != EGL_DISP_HANDLE_INVALID)) {
      msg_init_nop(khrn_prod_msg.post);
      advance_msg(&khrn_prod_msg.post);
      khrn_hw_notify_llat();
   }

   /*
      create a new wait message if we don't already have one
   */

   if (khrn_prod_msg.post == khrn_prod_msg.wait) {
      while (next_msg(khrn_prod_msg.wait) == khrn_prod_msg.done) { /* wait until there's room */
         khrn_sync_master_wait_timeout(-1);
      }
      msg_init_wait(khrn_prod_msg.wait);
      khrn_prod_msg.wait = next_msg(khrn_prod_msg.wait);
   }

   /*
      we have an unposted wait message with no display wait
      fill in the display wait
   */

   vcos_assert(next_msg(khrn_prod_msg.post) == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.post->wait_display_disp_handle == EGL_DISP_HANDLE_INVALID);
   khrn_prod_msg.post->wait_display_disp_handle = disp_handle;
   khrn_prod_msg.post->wait_display_pos = pos;
}

void khrn_hw_queue_display(EGL_DISP_SLOT_HANDLE_T slot_handle)
{
   /* try to put the display in the last posted message */
   MSG_T *msg_post_prev = prev_msg(khrn_prod_msg.post);
   if ((khrn_prod_msg.post == khrn_prod_msg.wait) && /* no unposted wait messages */
      (msg_post_prev->display_slot_handle == EGL_DISP_SLOT_HANDLE_INVALID)) { /* last posted message has no display */
      MSG_DISPLAY_STATE_T state;
      msg_post_prev->display_slot_handle = slot_handle;
      do {
         khrn_barrier();
      } while ((state = msg_post_prev->display_state) == MSG_DISPLAY_STATE_DOING);
      if (state != MSG_DISPLAY_STATE_DONE_NONE) {
         /* not done yet, or done (ours) */
         vcos_assert((state == MSG_DISPLAY_STATE_NOT_DONE) || (state == MSG_DISPLAY_STATE_DONE));
         return;
      } /* else: did no display */
   }

   /* post the display in a new message */
   if (khrn_prod_msg.post == khrn_prod_msg.wait) {
      while (next_msg(khrn_prod_msg.wait) == khrn_prod_msg.done) { /* wait until there's room */
         khrn_sync_master_wait_timeout(-1);
      }
      msg_init_wait(khrn_prod_msg.wait);
      khrn_prod_msg.wait = next_msg(khrn_prod_msg.wait);
   }
   vcos_assert(next_msg(khrn_prod_msg.post) == khrn_prod_msg.wait);
   msg_init_nop(khrn_prod_msg.post);
   khrn_prod_msg.post->display_slot_handle = slot_handle;
   advance_msg(&khrn_prod_msg.post);
   khrn_hw_notify_llat();
}

void khrn_hw_wait(void)
{
   if (khrn_prod_msg.post != khrn_prod_msg.wait) {
      msg_init_nop(khrn_prod_msg.post);
      advance_msg(&khrn_prod_msg.post);
      khrn_hw_notify_llat();
   }
   vcos_assert(khrn_prod_msg.post == khrn_prod_msg.wait);

   while (khrn_prod_msg.done != khrn_prod_msg.wait) {
      khrn_sync_master_wait_timeout(-1);
   }

   /* fifo should be empty now */
   vcos_assert(bin_mems_n == 0);
   vcos_assert(khrn_prod_msg.post == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.bin_prepared == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.render_prepared == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.bin_submitted == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.render_submitted == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.bin_done == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.render_done == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.bin_cleanup_llat == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.bin_cleanup == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.render_cleanup_llat == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.render_cleanup == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.display == khrn_prod_msg.wait);
   vcos_assert(khrn_prod_msg.done == khrn_prod_msg.wait);
   vcos_assert(!bin_prepared);
   vcos_assert(!render_prepared);
   vcos_assert(bins_submitted == bins_done);
   vcos_assert(renders_submitted == renders_done);
}

void khrn_hw_cleanup(void)
{
   while (more_msgs(khrn_prod_msg.bin_cleanup, khrn_prod_msg.bin_cleanup_llat)) {
      if (khrn_prod_msg.bin_cleanup->callback) {
         if (khrn_prod_msg.bin_cleanup->ok) {
#ifdef KHRN_STATS_ENABLE
            khrn_stats_record_add(KHRN_STATS_HW_BIN, khrn_prod_msg.bin_cleanup->stats_bin_time);
#endif
            khrn_prod_msg.bin_cleanup->callback(KHRN_HW_CALLBACK_REASON_BIN_FINISHED, khrn_prod_msg.bin_cleanup->callback_data, NULL);
         }
      }
      khrn_prod_msg.bin_cleanup = next_msg(khrn_prod_msg.bin_cleanup);
   }

   while ((khrn_prod_msg.render_cleanup != khrn_prod_msg.bin_cleanup) &&
      more_msgs(khrn_prod_msg.render_cleanup, khrn_prod_msg.render_cleanup_llat)) {
      if (khrn_prod_msg.render_cleanup->callback) {
         if (khrn_prod_msg.render_cleanup->ok_prepare) {
#ifdef KHRN_STATS_ENABLE
            khrn_stats_record_add(KHRN_STATS_HW_RENDER, khrn_prod_msg.render_cleanup->stats_render_time);
#endif
            khrn_prod_msg.render_cleanup->callback(KHRN_HW_CALLBACK_REASON_UNFIXUP, khrn_prod_msg.render_cleanup->callback_data, NULL);
         }
         khrn_prod_msg.render_cleanup->callback(khrn_prod_msg.render_cleanup->ok ? KHRN_HW_CALLBACK_REASON_RENDER_FINISHED : KHRN_HW_CALLBACK_REASON_OUT_OF_MEM, khrn_prod_msg.render_cleanup->callback_data, NULL);
      }
      khrn_prod_msg.render_cleanup = next_msg(khrn_prod_msg.render_cleanup);
   }

   while ((khrn_prod_msg.done != khrn_prod_msg.render_cleanup) &&
      more_msgs(khrn_prod_msg.done, khrn_prod_msg.display)) {
      khrn_prod_msg.done = next_msg(khrn_prod_msg.done);
   }
}

/*
   isr
*/

#ifdef CARBON
typedef struct CARBON_STATISTICS_S_
{
   unsigned __int64 calls;
   unsigned __int64 runtime;
   unsigned __int64 realtime;
   unsigned __int64 transactions[4][8][16];
} carbon_statistics_t;

void v3d_carbonStatistics(void *p);
#endif

static bool waiting_for_acquire_callback = false;
uint32_t acquire_n = 0;

void khrn_hw_isr_no_args(void)
{
   /* signal we picked up an IRQ */
   vcos_atomic_flags_or(&irq_flags, 1);
   /* kick the worker */
   khrn_hw_notify_llat();
}

extern void do_reg_dump(void);

static void do_irq(void)
{
   uint32_t flags;

   flags = vcos_atomic_flags_get_and_clear(&irq_flags);

   if (flags)
   {
      if (khrn_workarounds.GFXH724)
      {
         flags =  v3d_read(INTCTL);
         flags &= v3d_read(INTENA);
         flags &= v3d_read(INTCTL);
      }
      else
         flags =  v3d_read(INTCTL) >> 16;

      if (flags)
         khrn_hw_isr(flags);


      /* special case, for L2 handling can remove */
      if (khrn_workarounds.GFXH724)
      {
         flags =  v3d_read(INTCTL);
         flags &= v3d_read(INTENA);
         flags &= v3d_read(INTCTL);
      }
      else
         flags =  v3d_read(INTCTL) >> 16;

      if (flags)
      {
         vcos_atomic_flags_or(&irq_flags, 1);
         khrn_hw_notify_llat();
      }
      /* end of special case */

      /* re-enable L1 callbacks */
      v3d_write(INTDONE, 0);
   }
}

static void khrn_hw_isr(uint32_t flags)
{
   bool notify_master = false;
   bool notify_llat = false;
   bool notify_worker = false;

   v3d_write(INTCTL, flags);
   if (flags & (1 << 2)) {
      v3d_write(INTDIS, 1 << 2); /* this interrupt will be forced high until we supply some memory... */
   }
   v3d_full_memory_barrier();

   if (flags & (1 << 1)) {
      /* bin flush done */
#ifndef DONT_SUBMIT_TO_V3D_CALL_ISR_DIRECT
      uint32_t n = (v3d_read(BFC) - bins_done) & 0xff; /* BFC has 8-bit count */
#else
      uint32_t n = 1;
#endif
      bins_done += n;

      vcos_assert(n == 1);
      vcos_assert(next_msg(khrn_prod_msg.bin_done) == khrn_prod_msg.bin_submitted);
      vcos_assert(khrn_prod_msg.bin_done->callback);
      vcos_assert(khrn_prod_msg.bin_done->ok_prepare);

      cancel_want_overflow = true;
#ifdef KHRN_HW_MEASURE_BIN_TIME
      measure_bin_end = vcos_getmicrosecs();
#endif
#ifdef KHRN_STATS_ENABLE
      khrn_prod_msg.bin_done->stats_bin_time += vcos_getmicrosecs();
#endif
#ifdef KHRN_HW_BIN_HISTORY_N
      khrn_hw_bin_history[bin_history_i++] = vcos_getmicrosecs();
      /* use khrn_hw_bin_history_n instead of KHRN_HW_BIN_HISTORY_N to force
       * khrn_hw_bin_history_n to be linked in */
      if (bin_history_i == khrn_hw_bin_history_n) { bin_history_i = 0; }
#endif
      advance_msg(&khrn_prod_msg.bin_done);
      khrn_hw_advance_bin_exit_pos();
      notify_master = true; /* potentially waiting for advance_bin_exit_pos() */
      notify_llat = true; /* submit next, cleanup llat */
      notify_worker = true; /* potentially waiting for advance_bin_exit_pos() */
   } else {
      if (flags & (1 << 2)) {
         /* binner out of memory */
         notify_llat = true; /* supply overflow */
      }

      if (flags & (1 << 3)) {
         /* overflow memory taken */
         want_overflow = true;
         notify_llat = true; /* supply overflow */
      }
   }

   if (flags & (1 << 0)) {
#ifndef DONT_SUBMIT_TO_V3D_CALL_ISR_DIRECT
      /* render frame done */
      uint32_t n = (v3d_read(RFC) - renders_done) & 0xff; /* RFC has 8-bit count */
#else
      uint32_t n = 1;
#endif /* DONT_SUBMIT_TO_V3D_CALL_ISR_DIRECT */
#ifdef CARBON
      int i, j;
      carbon_statistics_t cs;
      const char ufname[8][32] =
      {
         "V3D_AXI_ID_L2C",
         "V3D_AXI_ID_CLE",
         "V3D_AXI_ID_PTB",
         "V3D_AXI_ID_PSE",
         "V3D_AXI_ID_VCD",
         "V3D_AXI_ID_VDW",
         "spare - ???",
         "V3D_AXI_ID_TLB"
      };

      memset(&cs, 0, sizeof(carbon_statistics_t));
      /* fake write to IDENT0 */
      v3d_write(IDENT0, (uint32_t)&cs);

      printf("read transaction breakdown (UNCACHED)\n--------------------------\n");
      for(i = 0; i < 8; i++)
      {
         for(j = 0; j < 16; j++)
         {
            if (cs.transactions[0][i][j])
            {
               printf("ID = %3.3d (%s), size = %3.3d, count = %16.16lld\n", i, ufname[i], j, cs.transactions[0][i][j]);
            }
         }
      }

      printf("read transaction breakdown (CACHED)\n--------------------------\n");
      for(i = 0; i < 8; i++)
      {
         for(j = 0; j < 16; j++)
         {
            if (cs.transactions[2][i][j])
            {
               printf("ID = %3.3d (%s), size = %3.3d, count = %16.16lld\n", i, ufname[i], j, cs.transactions[2][i][j]);
            }
         }
      }

      printf("write transaction breakdown (LOW PRIORITY) \n---------------------------\n");
      for(i = 0; i < 8; i++)
      {
         for(j = 0; j < 16; j++)
         {
            if (cs.transactions[1][i][j])
            {
               printf("ID = %3.3d (%s), size = %3.3d, count = %16.16lld\n", i, ufname[i], j, cs.transactions[1][i][j]);
            }
         }
      }

      printf("write transaction breakdown (HIGH PRIORITY) \n---------------------------\n");
      for(i = 0; i < 8; i++)
      {
         for(j = 0; j < 16; j++)
         {
            if (cs.transactions[3][i][j])
            {
               printf("ID = %3.3d (%s), size = %3.3d, count = %16.16lld\n", i, ufname[i], j, cs.transactions[3][i][j]);
            }
         }
      }
#endif

      renders_done += n;

      if (n) { /* if a frame completes between writing to INTCTL and reading from RFC we'll pass through the isr again and n might be 0 */
         vcos_assert((next_msg(khrn_prod_msg.render_done) == khrn_prod_msg.render_submitted) &&
            khrn_prod_msg.render_done->callback && khrn_prod_msg.render_done->ok_prepare &&
            (khrn_prod_msg.render_done->render_n >= n));
         khrn_prod_msg.render_done->render_n -= n;
         if (!khrn_prod_msg.render_done->render_n) {
            /* if the final frame completed between writing to INTCTL and
             * reading from RFC, the render frame done interrupt will be high.
             * leaving it high would be bad */
            v3d_write(INTCTL, 1 << 0);
            v3d_full_memory_barrier();
#ifdef KHRN_HW_MEASURE_RENDER_TIME
            measure_render_end = vcos_getmicrosecs();
#endif
#ifdef KHRN_STATS_ENABLE
            khrn_prod_msg.render_done->stats_render_time += vcos_getmicrosecs();
#endif
#ifdef KHRN_HW_RENDER_HISTORY_N
            khrn_hw_render_history[render_history_i++] = vcos_getmicrosecs();
            /* use khrn_hw_render_history_n instead of KHRN_HW_RENDER_HISTORY_N
             * to force khrn_hw_render_history_n to be linked in */
            if (render_history_i == khrn_hw_render_history_n) { render_history_i = 0; }
#endif
            if (khrn_prod_msg.render_done->callback) 
            {
               khrn_prod_msg.render_done->callback(KHRN_HW_CALLBACK_REASON_RENDER_FINISHED_ISR,
                                                   khrn_prod_msg.render_done->callback_data,
                                                   NULL);
            }

            advance_msg(&khrn_prod_msg.render_done);
            khrn_hw_advance_render_exit_pos();
            notify_master = true; /* potentially waiting for advance_render_exit_pos() */
            notify_llat = true; /* submit next, cleanup llat, display */
            notify_worker = true; /* potentially waiting for advance_render_exit_pos() */
         }
      }
   }

   if (notify_master) { khrn_sync_notify_master(); }
   if (notify_llat) { khrn_hw_notify_llat(); }
   if (notify_worker) { khrn_worker_notify(); }
}

/*
   llat callback
*/

static void acquire_callback_no_args(void)
{
   waiting_for_acquire_callback = false;
   khrn_hw_notify_llat();
}

static bool acquire_abstract_v3d(BEGL_HWAcquireSettings *acqSettings)
{
   /* Note: reference counting is done at the lowest level to ensure multi-process behaves
      correctly. Do not reference count here to decide whether to acquire or not. */
   bool ok = false;
   BEGL_DriverInterfaces *iface = BEGL_GetDriverInterfaces();

   if (iface && iface->hwInterface && acqSettings)
      ok = iface->hwInterface->Acquire(iface->hwInterface->context, acqSettings);

   return ok;
}

static void release_abstract_v3d(BEGL_HWReleaseSettings *relSettings)
{
      BEGL_DriverInterfaces *iface = BEGL_GetDriverInterfaces();
      if (iface && iface->hwInterface)
      {
         iface->hwInterface->Release(iface->hwInterface->context, relSettings);
      }
}

static bool acquire_v3d(uint32_t actual_user_vpm, uint32_t max_user_vpm)
{
#ifdef __linux__
   struct timeval curTime;
   unsigned int nowMs;
#endif
   BEGL_HWAcquireSettings acqSettings;

   bool ok = false;

   if (waiting_for_acquire_callback) {
      return false;
   }

   if (khrn_options.no_bin_render_overlap)
   {
      /* Prevent overlapped binning and rendering */
      if (acquire_n > 0)
         return false;
   }

   khrn_barrier();

   /* todo: actual_user_vpm is ignored if forcing a reset. not sure if that's
    * the right thing to do. also, not really sure on this whole actual_user_vpm
    * thing in the first place */
   memset(&acqSettings, 0, sizeof(BEGL_HWAcquireSettings));
   acqSettings.shaderType = BEGL_HWShaderTypeMain;
   acqSettings.ISR = khrn_hw_isr_no_args;
   acqSettings.tryAcquireAgain = acquire_callback_no_args;
   acqSettings.actualVPM = actual_user_vpm;
   acqSettings.minMaxVPM = max_user_vpm;
   acqSettings.forceReset = 0;

   ok = acquire_abstract_v3d(&acqSettings);
   if (!ok)
   {
      waiting_for_acquire_callback = true;
      return false;
   }

#ifdef __linux__
   gettimeofday(&curTime, NULL);
   nowMs = curTime.tv_usec / 1000;
   nowMs += curTime.tv_sec * 1000;

   khrn_driver_counters()->in_time = nowMs;
#endif

   if (acquire_n++ == 0) {
      v3d_write(BFC, 1); /* writing 1 clears */
      v3d_write(RFC, 1);
      bins_submitted = 0;
      renders_submitted = 0;
      bins_done = 0;
      renders_done = 0;

#ifndef CARBON
      vcos_assert(!(v3d_read(INTCTL) & ((1 << 0) | (1 << 1)))); /* render/frame done interrupts should be low... */
#endif
      v3d_write(INTCTL, (1 << 0) | (1 << 1)); /* clear them just incase */
      v3d_write(INTENA,
         (1 << 0) | /* render frame done */
         (1 << 1)); /* bin flush done */

      reset_perf_counters();
   }

   return true;
}

static void release_v3d(uint32_t max_user_vpm)
{
#ifndef DONT_SUBMIT_TO_V3D_CALL_ISR_DIRECT
   if (acquire_n == 1) {
      /* we don't think the hardware is busy */
      vcos_assert(!((bins_submitted - v3d_read(BFC)) & 0xff));
      vcos_assert(!((renders_submitted - v3d_read(RFC)) & 0xff));
      /* the hardware doesn't think it's busy */
      vcos_assert(!(v3d_read(PCS) & ((1 << 0) | (1 << 2))));
   }
#endif /* DONT_SUBMIT_TO_V3D_CALL_ISR_DIRECT */

   update_perf_counters();

   {
#ifdef __linux__
      struct timeval curTime;
      unsigned int nowMs;
#endif
      BEGL_HWReleaseSettings relSettings;
      memset(&relSettings, 0, sizeof(BEGL_HWReleaseSettings));
      relSettings.shaderType = BEGL_HWShaderTypeMain;
      release_abstract_v3d(&relSettings);

#ifdef __linux__
      gettimeofday(&curTime, NULL);
      nowMs = curTime.tv_usec / 1000;
      nowMs += curTime.tv_sec * 1000;

      /* dont add if the counters have been reset inbetween a acquire/release */
      if (khrn_driver_counters()->in_time != 0)
      {
         khrn_driver_counters()->total_time += (nowMs - khrn_driver_counters()->in_time);
      }
#endif

   }

   acquire_n--;
}

static void prepare_bin(void)
{
   while (!bin_prepared && more_msgs(khrn_prod_msg.bin_prepared, khrn_prod_msg.post)) {
      if (khrn_prod_msg.bin_prepared->callback) {
         if (!khrn_prod_msg.bin_prepared->ready) {
            break;
         }
         khrn_barrier();
         if (khrn_prod_msg.bin_prepared->ok) {
            bool bin_mem_from_pool;
            uint32_t bin_mem_size;

            /* acquiring here means we maybe have the hw on for a little longer
             * than necessary, but also means we won't keep stuff locked while
             * waiting to acquire. todo: what if we want to run user shaders as
             * part of fixup (which we might want to do for gl)? then we have a
             * potential deadlock... */
            if (!acquire_v3d(khrn_prod_msg.bin_prepared->actual_user_vpm, khrn_prod_msg.bin_prepared->max_user_vpm)) {
               break;
            }

            bin_mem_from_pool = s_bin_mem_size >= khrn_prod_msg.bin_prepared->bin_mem_size_min;
            bin_mem_size = bin_mem_from_pool ? s_bin_mem_size : khrn_prod_msg.bin_prepared->bin_mem_size_min;

            if (s_bin_memory_preallocated)
            {
               vcos_assert(bin_mem_from_pool);
               bin_mem_from_pool = true;
            }

            khrn_prod_msg.bin_prepared->bin_mems_head = bin_mem_from_pool ? bin_mem_pool_alloc(true) :
               /* pool bin mems aren't big enough. allocate separately. todo: is this going to hurt? */
               /* mem_alloc_ex() rounds up to the nearest cache line size, adding (+1) byte to the allocation size gives us another line.
                  the binner was found to be able to prefetch of the end of the block, causing ARC read violations */
               mem_alloc_ex(bin_mem_size + 1, KHRN_HW_BIN_MEM_ALIGN, (MEM_FLAG_T)(MEM_FLAG_DIRECT | MEM_FLAG_NO_INIT), "khrn_hw_bin_mem (too cool for pool)", MEM_COMPACT_DISCARD);
            if (khrn_prod_msg.bin_prepared->bin_mems_head == MEM_INVALID_HANDLE) {
               release_v3d(khrn_prod_msg.bin_prepared->max_user_vpm); /* todo: avoid acquiring/releasing so much when waiting for bin mem? */

               if (bin_mems_n != 0) {
                  /* wait for one to be freed (this makes sense even for the separate allocation mode as we can discard bin mems)... */
                  break;
               }
               khrn_prod_msg.bin_prepared->ok = false;
               khrn_prod_msg.bin_prepared->ok_prepare = false;
            } else {
               uint32_t specials[4];
               khrn_prod_msg.bin_prepared->ok_prepare = true;
               set_bin_mems_next(khrn_prod_msg.bin_prepared->bin_mems_head, MEM_INVALID_HANDLE);
               khrn_prod_msg.bin_prepared->bin_mem = mem_lock(khrn_prod_msg.bin_prepared->bin_mems_head);
               specials[0] = khrn_prod_msg.bin_prepared->special_0;
               specials[1] = khrn_hw_addr(khrn_prod_msg.bin_prepared->bin_mem);
               specials[2] = khrn_hw_addr(khrn_prod_msg.bin_prepared->bin_mem) + bin_mem_size;
               specials[3] = bin_mem_size;
               bin_begin = khrn_prod_msg.bin_prepared->bin_begin;
               bin_end = khrn_prod_msg.bin_prepared->bin_end;
               fixup_done = false;
               khrn_prod_msg.bin_prepared->callback(KHRN_HW_CALLBACK_REASON_FIXUP, khrn_prod_msg.bin_prepared->callback_data, specials);
               khrn_hw_flush_dcache(); /* todo: flush only what needs to be flushed (stuff in fixup callback might be assuming this flush too!) */
               bin_prepared = true;
            }
         } else {
            khrn_prod_msg.bin_prepared->ok_prepare = false;
         }
      }

      khrn_prod_msg.bin_prepared = next_msg(khrn_prod_msg.bin_prepared);
   }
}

static void prepare_render(void)
{
   while (!render_prepared && (khrn_prod_msg.render_prepared != khrn_prod_msg.bin_prepared)) {
      if (khrn_prod_msg.render_prepared->callback && khrn_prod_msg.render_prepared->ok_prepare) {
         render_begin = khrn_prod_msg.render_prepared->render_begin;
         render_end = khrn_prod_msg.render_prepared->render_end;
         khrn_hw_flush_dcache(); /* todo: flush only what needs to be flushed */
         render_prepared = true;
      }
      khrn_prod_msg.render_prepared = next_msg(khrn_prod_msg.render_prepared);
   }
}

/* hw_address is just ANY device memory pointer.  We already have checked that device memory
   heaps are all allocated from the same 1GB partition, so the top two bits should be 
   consistant */
static void clear_caches(KHRN_HW_CC_FLAG_T cc, uint32_t hw_address)
{
   if (cc & KHRN_HW_CC_FLAG_L2) {
      v3d_write(L2CACTL, 1 << 2);
   }
   if (cc & (KHRN_HW_CC_FLAG_IC | KHRN_HW_CC_FLAG_UC | KHRN_HW_CC_FLAG_TU)) {
      v3d_write(SLCACTL,
         ((cc & KHRN_HW_CC_FLAG_IC) ? 0x000000ff : 0) |
         ((cc & KHRN_HW_CC_FLAG_UC) ? 0x0000ff00 : 0) |
         ((cc & KHRN_HW_CC_FLAG_TU) ? 0xffff0000 : 0));
   }

   if (!khrn_workarounds.HAS_32BIT_SUPPORT)
   {
      /* take the top 2 bits and push them to bits 2 & 3 of the register */
      hw_address = (hw_address >> 28) & 0xC;
      if (hw_address) {
         /* if the address map is other than 0 turn the enable on (bit 1) */
         hw_address |= 2;
      }

      /* toggle the reset (bit 0) */
      v3d_write(CACHE_CTRL, hw_address | 1);
      v3d_write(CACHE_CTRL, hw_address | 0);
   }
   else
   {
      /* B2 revs going forward support 32bit address (ex binner) so leave the address remap off */
      /* toggle the reset (bit 0) */
      v3d_write(CACHE_CTRL, 1);
      v3d_write(CACHE_CTRL, 0);
   }

   mem_flush_cache();
}

static void submit_bin(bool *notify_master, bool *notify_worker, bool *notify_llat)
{
   while (processed_msgs(khrn_prod_msg.bin_done, khrn_prod_msg.bin_submitted) &&
      (khrn_prod_msg.bin_submitted != khrn_prod_msg.bin_prepared)
      ) {
      KHRN_HW_CC_FLAG_T cc;
      bool hw = !!khrn_prod_msg.bin_submitted->callback;
      bool ok = hw && khrn_prod_msg.bin_submitted->ok;
      vcos_assert(khrn_prod_msg.bin_submitted->ok == khrn_prod_msg.bin_submitted->ok_prepare);
      if (ok && !fixup_done) {
         break;
      }
      khrn_barrier();
      cc = khrn_prod_msg.bin_submitted->bin_cc;
      khrn_prod_msg.bin_submitted = next_msg(khrn_prod_msg.bin_submitted);
      if (ok && khrn_hw_alias_normal(bin_begin) != NULL) {
#ifdef DONT_SUBMIT_TO_V3D
         uint8_t *p;
#endif
         uint32_t hw_bin_begin;
         vcos_assert(khrn_prod_msg.bin_submitted == khrn_prod_msg.bin_prepared);
         vcos_assert(bin_prepared);
         /* reset the ptb alloc stuff:
          * - not out of memory
          * - no overflow memory
          * - binner-out-of-memory and (not a0) overflow-memory-taken interrupts
          *   enabled but low */
         v3d_write(INTDIS, 1 << 3);
         v3d_write(BPOS, 2 * KHRN_HW_BIN_MEM_GRANULARITY); /* 2 blocks is enough to force out of memory low in the worst case (need both clip + cl memory) */
         v3d_write(BPOS, 0);
         /* the BPOS write doesn't take effect immediately: wait for the oom bit
          * to go low... */
         while (v3d_read(PCS) & (1 << 8)) ;
         v3d_write(INTCTL, (1 << 2) | (1 << 3));
         v3d_write(INTENA, (1 << 2) | (1 << 3));
         want_overflow = true;
         cancel_want_overflow = false;
         ++bins_submitted;
#ifdef KHRN_HW_MEASURE_BIN_TIME
         printf("%d\n", measure_bin_end - measure_bin_start);
         measure_bin_start = vcos_getmicrosecs();
#endif
#ifdef KHRN_STATS_ENABLE
         khrn_prod_msg.bin_done->stats_bin_time = -vcos_getmicrosecs();
#endif
#ifdef KHRN_HW_BIN_HISTORY_N
         khrn_hw_bin_history[bin_history_i++] = vcos_getmicrosecs();
         vcos_assert(bin_history_i != KHRN_HW_BIN_HISTORY_N);
#endif
         v3d_full_memory_barrier();

         hw_bin_begin = khrn_hw_addr(bin_begin);
         clear_caches(cc, hw_bin_begin);

#ifdef PRINT_CLIST_DEBUG
         {
            static int i = 0;
            i++;

            if (1) /*(uintptr_t)bin_end - (uintptr_t)bin_begin > 3000)*/
            {
               FILE *fp;
               char fname[64];
               sprintf(fname, "b%d_%08x.dat", i, hw_bin_begin);
               fp = fopen(fname, "w");
               if (fp != NULL)
               {
                  control_list_debug_file(fp);
                  control_list_debug_dump(bin_begin, bin_end, 1);
                  fclose(fp);
                  control_list_debug_file(NULL);
               }
            }
         }
#endif

         s_debugLastBinStart = hw_bin_begin;

#ifndef DONT_SUBMIT_TO_V3D
         v3d_write(CT0CA, hw_bin_begin);
#ifndef DONT_SUBMIT_TO_V3D_CALL_ISR_DIRECT
         v3d_write(CT0EA, khrn_hw_addr(bin_end));
#else
         khrn_hw_isr(2);   /* binner done */
#endif /* DONT_SUBMIT_TO_V3D_CALL_ISR_DIRECT */
#else
         p = (uint8_t *)mem_lock(nosubmit_bin_handle);
         v3d_write(CT0CA, khrn_hw_addr(p));
         v3d_write(CT0EA, khrn_hw_addr(p + NOSUBMIT_BIN_SIZE));
         mem_unlock(nosubmit_bin_handle);
#endif
         bin_prepared = false;
         
      } else {
         advance_msg(&khrn_prod_msg.bin_done);
         if (ok) {
            bin_prepared = false;
            *notify_llat = true;
         }
         if (hw) {
            khrn_hw_advance_bin_exit_pos();
            *notify_master = true;
            *notify_worker = true;
         }
      }
   }
}

static void submit_render(bool *notify_master, bool *notify_worker)
{
   while (processed_msgs(khrn_prod_msg.render_done, khrn_prod_msg.render_submitted) &&
      (khrn_prod_msg.render_submitted != khrn_prod_msg.render_prepared) &&
      /* todo: this is needed for both the hw-2422 workaround and to wait for
       * fixup. might be clearer to move this into the ifdef and explicitly wait
       * for fixup... */
      (khrn_prod_msg.render_submitted != khrn_prod_msg.bin_submitted) &&
      (khrn_worker_get_exit_pos() >= khrn_prod_msg.render_submitted->wait_worker_pos) &&
      ((khrn_prod_msg.render_submitted->wait_display_disp_handle == EGL_DISP_HANDLE_INVALID) ||
      !egl_disp_still_on(khrn_prod_msg.render_submitted->wait_display_disp_handle, khrn_prod_msg.render_submitted->wait_display_pos))
      /* If bin/render overlap is disabled, check no binning is in progress */
      && (!khrn_options.no_bin_render_overlap || (processed_msgs(khrn_prod_msg.bin_done, khrn_prod_msg.bin_submitted)))
      ) {
      bool hw = !!khrn_prod_msg.render_submitted->callback;
      bool ok = hw && khrn_prod_msg.render_submitted->ok_prepare; /* we want to put the render list through iff we put the bin list through to keep the hw semaphore stuff in sync */
      KHRN_HW_CC_FLAG_T cc = khrn_prod_msg.render_submitted->render_cc;

      if (ok && khrn_prod_msg.render_submitted->callback)
      {
         khrn_prod_msg.render_submitted->callback(KHRN_HW_CALLBACK_REASON_RENDER_SUBMIT, 
                                                  khrn_prod_msg.render_submitted->callback_data, NULL);
      }

      khrn_prod_msg.render_submitted = next_msg(khrn_prod_msg.render_submitted);

      if (ok) {
#ifdef DONT_SUBMIT_TO_V3D
         uint8_t * p;
#endif
         uint32_t hw_render_begin;
         vcos_assert(khrn_prod_msg.render_submitted == khrn_prod_msg.render_prepared);
         vcos_assert(render_prepared);
         if (!khrn_prod_msg.render_done->ok) {
            /* we gave up on this frame. we still want to put a render list
             * through (to keep the hw semaphore stuff in sync), but we can just
             * put the "null" render list through (just does a wait semaphore
             * and eof) */
            uint8_t *p = (uint8_t *)mem_lock(null_render_handle); /* this gets unlocked in cleanup_render_llat */
            khrn_prod_msg.render_done->render_n = 1; /* only going to have 1 eof */
            render_begin = p;
            render_end = p + NULL_RENDER_SIZE;
         }
         renders_submitted += khrn_prod_msg.render_done->render_n;
#ifdef KHRN_HW_MEASURE_RENDER_TIME
         printf(",%d\n", measure_render_end - measure_render_start);
         measure_render_start = vcos_getmicrosecs();
#endif
#ifdef KHRN_STATS_ENABLE
         khrn_prod_msg.render_done->stats_render_time = -vcos_getmicrosecs();
#endif
#ifdef KHRN_HW_RENDER_HISTORY_N
         khrn_hw_render_history[render_history_i++] = vcos_getmicrosecs();
         vcos_assert(render_history_i != KHRN_HW_RENDER_HISTORY_N);
#endif
         v3d_full_memory_barrier();
         hw_render_begin = khrn_hw_addr(render_begin);
         clear_caches(cc, hw_render_begin);

#ifdef PRINT_CLIST_DEBUG
         {
            static int i = 0;
            i++;

            dbg_render_begin = render_begin;
            dbg_render_end = render_end;

            if (0)/*(uintptr_t)render_end - (uintptr_t)render_begin == 341)*/
            {
               FILE *fp;
               char fname[64];

               sprintf(fname, "r%d_%08x.dat", i, hw_render_begin);
               fp = fopen(fname, "w");
               if (fp != NULL)
               {
                  control_list_debug_file(fp);
                  control_list_debug_dump(render_begin, render_end, 1);
                  fclose(fp);
                  control_list_debug_file(NULL);
               }
            }
         }
#endif

         s_debugLastRenderStart = hw_render_begin;

#ifndef DONT_SUBMIT_TO_V3D
         v3d_write(CT1CA, hw_render_begin);
#ifndef DONT_SUBMIT_TO_V3D_CALL_ISR_DIRECT
         v3d_write(CT1EA, khrn_hw_addr(render_end));
#else
         khrn_hw_isr(1);      /* render done */
#endif
#else
         p = (uint8_t *)mem_lock(null_render_handle);
         v3d_write(CT1CA, khrn_hw_addr(p));
         v3d_write(CT1EA, khrn_hw_addr(p + NULL_RENDER_SIZE));
         mem_unlock(null_render_handle);
#endif

         render_prepared = false;
      } else {
         khrn_prod_msg.render_done = next_msg(khrn_prod_msg.render_done);
         if (hw) {
            khrn_hw_advance_render_exit_pos();
            *notify_master = true;
            *notify_worker = true;
         }
      }
   }
}

#ifdef KHRN_HW_BOOM
static uint32_t boom_n = 0;
#endif

static void supply_overflow(void)
{
   if (want_overflow && !cancel_want_overflow) {
      MSG_T *msg = prev_msg(khrn_prod_msg.bin_submitted); /* can't use khrn_prod_msg.bin_done -- it might have advanced... */
      void *bin_mem;
      uint32_t bin_mem_size;

      /* we only set want_overflow in response to an overflow-memory-taken
       * interrupt. we only supply more overflow memory in this function, and we
       * reset want_overflow just before supplying it */
      vcos_assert(v3d_read(BPOS) == 0);

      if (msg->ok) {
         /* we can't wait for bin mems here -- a stalled bin can stall a render
          * if it's holding too much vpm */
         MEM_HANDLE_T handle;
#ifdef KHRN_HW_BOOM
         if (++boom_n >= 64) {
            /* bwahahahaha */
            handle = MEM_INVALID_HANDLE;
         } else
#endif
         handle = bin_mem_pool_alloc(false);

         if (handle == MEM_INVALID_HANDLE) {
            if (/* the ptb isn't actually out of memory -- we might be ok */
               /* This condition causes a lock up, so don't do it. We'll just drop the frame.
               !(v3d_read(PCS) & (1 << 8)) 
               ||
               */
               /* the ptb has managed to finish. this is possible as the ptb
                * tries to allocate memory in advance and will signal out of
                * memory if it can't, even if the memory might not be needed */
               !((bins_submitted - v3d_read(BFC)) & 0xff)){ 
                  /* BFC has 8-bit count */
                  return;
            }

#ifdef KHRN_HW_BOOM
            /* only reset here to make sure we hit this case -- it's the tricky one */
            boom_n = 0;
#endif

            /* we're pretty sure at this point that we're going to give up on
             * this frame... */

            if (khrn_prod_msg.render_submitted == khrn_prod_msg.bin_submitted) {
               /* we've already submitted the render list; we need to switch
                * over to the null render list... */

               /* this should be an uncached read, as the HW can change it under our feet */
               uint8_t *p;

               /* wait for the render cle to hit the wait semaphore instruction
                * (we always follow the wait semaphore instruction with a marker
                * instruction) */
               do {

                  p = (uint8_t *)khrn_hw_unaddr(v3d_read(CT1CA));
                  /* read as a uint8_t, only 1 byte flush required */
                  khrn_hw_flush_dcache_range(p, 1);

                  if (!((bins_submitted - v3d_read(BFC)) & 0xff)) {
                     /* the ptb has managed to finish... */
                     return;
                  }
               } while (*(volatile uint8_t *)p != KHRN_HW_INSTR_MARKER); /* assuming there are no prims or markers before the wait semaphore (prims might look like a marker) */

               /* stop the render cle just after the wait semaphore instruction
                * (it should be stalled there) */

               v3d_write(CT1CS, 1 << 5); /* stop */
               if (!((bins_submitted - v3d_read(BFC)) & 0xff)) {
                  /* the ptb has managed to finish... */
                  v3d_write(CT1CS, 1 << 4); /* go */
                  return;
               }

               /* switch over to the null render list */
               p = (uint8_t *)mem_lock(null_render_handle); /* this gets unlocked in cleanup_render_llat */

               vcos_assert(msg->render_n > 0);
               msg->render_n = 1; /* only going to have 1 eof */
               v3d_full_memory_barrier();
               v3d_write(CT1CA, khrn_hw_addr(p + NULL_RENDER_POST_WAIT_SEMAPHORE_OFFSET));
               v3d_write(CT1EA, khrn_hw_addr(p + NULL_RENDER_SIZE));
               v3d_write(CT1CS, 1 << 4); /* go */
            } /* else: we haven't submitted the render list; we'll submit the null render list when we get around to it */

            /* by this point, we're sure we're going to give up on this frame.
             * the binner might actually be able to finish without any more
             * memory, but we have no way of knowing */
            msg->ok = false;

            /* to allow the ptb to finish, we feed it the initial bin mem over
             * and over. todo: we could also try to cut the bin control list
             * short, but i'm not convinced we can easily do that safely */
            bin_mem = msg->bin_mem;
            bin_mem_size = BIN_MEM_TRASHABLE_SIZE;
         } else {
            /* successfully allocated another bin mem... */
            set_bin_mems_next(handle, msg->bin_mems_head);
            msg->bin_mems_head = handle;

            bin_mem = mem_lock(handle);
            bin_mem_size = s_bin_mem_size;
         }
      } else {
         /* we've given up on this frame. keep feeding the ptb the initial bin
          * mem to allow it to finish */

         bin_mem = msg->bin_mem;
         bin_mem_size = BIN_MEM_TRASHABLE_SIZE;
      }

      want_overflow = false; /* might write to this right after writing to BPOS, so make sure we clear it first */
      v3d_full_memory_barrier();

      vcos_assert(bin_mem_size >= KHRN_HW_BIN_MEM_GRANULARITY); /* ptb malloc won't take anything less than this */
      v3d_write(BPOA, khrn_hw_addr(bin_mem));
      v3d_write(BPOS, bin_mem_size);

      /* we've just given the hw some memory, which should have removed any
       * out-of-memory condition, so we can reenable the interrupt */
      if (!(v3d_read(INTENA) & (1 << 2))) {
         v3d_write(INTCTL, 1 << 2);
         v3d_write(INTENA, 1 << 2);
      }
   }
}

static void cleanup_bin_llat(bool *notify_master)
{
   while (more_msgs(khrn_prod_msg.bin_cleanup_llat, khrn_prod_msg.bin_done)) {
      if (khrn_prod_msg.bin_cleanup_llat->callback) {
         khrn_prod_msg.bin_cleanup_llat->callback(khrn_prod_msg.bin_cleanup_llat->ok ?
            KHRN_HW_CALLBACK_REASON_BIN_FINISHED_LLAT :
            KHRN_HW_CALLBACK_REASON_OUT_OF_MEM_LLAT,
            khrn_prod_msg.bin_cleanup_llat->callback_data,
            NULL);
      }
      advance_msg(&khrn_prod_msg.bin_cleanup_llat);
      *notify_master = true;
   }
}

static void cleanup_render_llat(bool *notify_master)
{
   while ((khrn_prod_msg.render_cleanup_llat != khrn_prod_msg.bin_cleanup_llat) &&
      more_msgs(khrn_prod_msg.render_cleanup_llat, khrn_prod_msg.render_done)) {
      if (khrn_prod_msg.render_cleanup_llat->callback && khrn_prod_msg.render_cleanup_llat->ok_prepare) {
         MEM_HANDLE_T handle, next_handle;
         release_v3d(khrn_prod_msg.render_cleanup_llat->max_user_vpm);

         if (!khrn_prod_msg.render_cleanup_llat->ok) {
            mem_unlock(null_render_handle); /* locked in supply_overflow or submit_render */
         }
         handle = khrn_prod_msg.render_cleanup_llat->bin_mems_head;
         while (handle != MEM_INVALID_HANDLE) {
            next_handle = get_bin_mems_next(handle);
            mem_unlock(handle);
            if (/* last handle (first allocated) */
               (next_handle == MEM_INVALID_HANDLE) &&
               /* too big for pool bin mems (ie must have allocated separately) */
               (s_bin_mem_size < khrn_prod_msg.render_cleanup_llat->bin_mem_size_min)) {
               mem_release(handle);
            } else {
               bin_mem_pool_free(handle);
            }
            handle = next_handle;
         }
      }
      advance_msg(&khrn_prod_msg.render_cleanup_llat);
      *notify_master = true;
   }
}

static void display(bool *notify_master)
{
   while (more_msgs(khrn_prod_msg.display, khrn_prod_msg.render_done)) {
      EGL_DISP_SLOT_HANDLE_T slot_handle;
      _di(); /* getting interrupted here would be bad */
      khrn_prod_msg.display->display_state = MSG_DISPLAY_STATE_DOING;
      khrn_barrier();
      slot_handle = khrn_prod_msg.display->display_slot_handle;
      khrn_prod_msg.display->display_state = (slot_handle == EGL_DISP_SLOT_HANDLE_INVALID) ?
         MSG_DISPLAY_STATE_DONE_NONE : MSG_DISPLAY_STATE_DONE;
      _ei();
      if (slot_handle != EGL_DISP_SLOT_HANDLE_INVALID) {
         egl_disp_ready(slot_handle, false);
      }
      advance_msg(&khrn_prod_msg.display);
      *notify_master = true;
   }
}

static void khrn_hw_llat_callback(void)
{
   bool notify_master = false, notify_worker = false, notify_llat = false;

   /* stuff to keep hardware going (and put stuff on the display) first */
   do_irq();
   supply_overflow();
   prepare_bin();
   submit_bin(&notify_master, &notify_worker, &notify_llat);
   prepare_render();
   submit_render(&notify_master, &notify_worker);
   display(&notify_master);

   cleanup_bin_llat(&notify_master);
   cleanup_render_llat(&notify_master);

   if (notify_master) { khrn_sync_notify_master(); }
   if (notify_llat) { khrn_hw_notify_llat(); }
   if (notify_worker) { khrn_worker_notify(); }
}

/*
   stuff called from anywhere
*/

void khrn_hw_ready(bool ok, void *callback_data) /* if ok is false, callback will later be called with KHRN_HW_CALLBACK_REASON_OUT_OF_MEM_LLAT/KHRN_HW_CALLBACK_REASON_OUT_OF_MEM */
{
   MSG_T *msg = (MSG_T *)((uint8_t *)callback_data - offsetof(MSG_T, callback_data));
   vcos_assert((msg >= msgs) && (msg < (msgs + MSGS_N)));
   vcos_assert(msg->callback);
   msg->ok = ok;
   khrn_barrier();
   vcos_assert(!msg->ready);
   msg->ready = true;
   khrn_hw_notify_llat();
}

void khrn_hw_fixup_done(bool in_callback, void *callback_data)
{
   UNUSED(callback_data);

   khrn_barrier();
   vcos_assert(!fixup_done);
   fixup_done = true;
   if (!in_callback) {
      khrn_hw_notify_llat();
   }
}

void khrn_hw_notify_llat(void)
{
   khrn_llat_notify(llat_i);
}

#ifdef __mips__
int backtrace_mips32(void **buffer, int size)
{
   unsigned int *p;
   unsigned int *ra = __builtin_return_address(0);
   unsigned int *sp = __builtin_frame_address(0);
   size_t ra_offset;
   size_t stack_size = 0;
   int depth;
   void *x;

   if (!((buffer != NULL) && (size > 0)))
      return 0;

   for (p = (unsigned int *)backtrace_mips32; !stack_size; p++)
   {
      if((*p & 0xffff0000) == 0x27bd0000) /* addiu sp,sp */
         stack_size = abs((short)(*p & 0xffff));
      else if (*p == 0x03e00008) /* jr, ra (got to the end of the function) */
         break;
   }

   /* oops, didnt find the stack */
   if (stack_size)
   {
      sp = (unsigned int *)((unsigned int)sp + stack_size);

      for (depth = 0; depth < size && ra; depth++)
      {
         buffer[depth] = ra;

         ra_offset = 0;
         stack_size = 0;

         for (p = ra; !ra_offset || !stack_size; p--)
         {
            switch (*p & 0xffff0000)
            {
               case 0x27bd0000:  /* addiu sp,sp */
                   stack_size = abs((short)(*p & 0xffff));
                   break;
               case 0xafbf0000:  /* sw ra,???? */
                   ra_offset = (short)(*p & 0xffff);
                   break;
               case 0x3c1c0000:  /* lui gp, 0 */
                   return depth + 1;
               default:
                   break;
            }
         }

         ra = *(unsigned int **)((unsigned int)sp + ra_offset);
         sp = (unsigned int *)((unsigned int)sp + stack_size);
      }

      return depth;
   }
   else
      return 0;
}
#endif /* __mips__ */

void khrn_print_backtrace(void)
{
#ifdef __mips__
   vcos_mutex_lock(&backtrace_mutex);

   /* do a backtrace, if available */
   void *array[255];
   int size;
   int i;

   size = backtrace_mips32(array, sizeof(array));

#ifdef ANDROID
   LOGD("Obtained %d stack frames.\n", size);
#else
   printf("Obtained %d stack frames.\n", size);
   printf("to see more backtrace info, set PROFILING?=1 in V3DDriver.mk.\n");
#endif /* ANDROID */
   for (i = 0; i < size; i++)
   {
      Dl_info dlinfo;
      dladdr(array[i], &dlinfo);
#ifdef ANDROID
      LOGD("%s, %s, %p\n", dlinfo.dli_fname, dlinfo.dli_sname, dlinfo.dli_saddr);
#else
      printf("%s, %s, %p\n", dlinfo.dli_fname, dlinfo.dli_sname, dlinfo.dli_saddr);
#endif /* ANDROID */
   }

   vcos_mutex_unlock(&backtrace_mutex);
#endif /* __mips__ */
}

#ifdef __linux__
bool khrn_event_wait_timeout(void * sem, int ms)
{
   bool done = false;
   bool timed_out = false;

   while (!done)
   {
      struct timespec ts;
      int s;
      clock_gettime(CLOCK_REALTIME, &ts);
      /* add on two seconds */
      ts.tv_sec += 2;

      while ((s = sem_timedwait(sem, &ts)) == -1 && errno == EINTR)
         continue;       /* Restart if interrupted by handler */
      if (s == -1)
      {
         if (errno == ETIMEDOUT)
         {
            /* timed out, print a backtrace */
            khrn_print_backtrace();

            /* -1 represents infinity */
            if (ms != -1)
            {
               ms -= 2000;
               if (ms <= 0)
               {
                  timed_out = true;
                  done = true;
               }
            }
         }
      }
      else
      {
         /* s == 0, so semaphore was taken */
         done = true;
      }
   }

   return !timed_out;
}
#endif /* __linux__ */

static void reset_perf_counters(void)
{
   if (s_perf_counters)
   {
      /* clear the performance counters */
      v3d_write(PCTRC, 0xFFFF);

      /* set up the counters */
      if (s_perf_counters->hw_group_active == 0)
      {
         v3d_write(PCTRS0,  13);
         v3d_write(PCTRS1,  14);
         v3d_write(PCTRS2,  15);
         v3d_write(PCTRS3,  16);
         v3d_write(PCTRS4,  17);
         v3d_write(PCTRS5,  18);
         v3d_write(PCTRS6,  19);
         v3d_write(PCTRS7,  20);
         v3d_write(PCTRS8,  21);
         v3d_write(PCTRS9,  22);
         v3d_write(PCTRS10, 23);
         v3d_write(PCTRS11, 24);
         v3d_write(PCTRS12, 25);
         v3d_write(PCTRS13, 28);
         v3d_write(PCTRS14, 29);
      }
      else if (s_perf_counters->hw_group_active == 1)
      {
         v3d_write(PCTRS0,  0);
         v3d_write(PCTRS1,  1);
         v3d_write(PCTRS2,  2);
         v3d_write(PCTRS3,  3);
         v3d_write(PCTRS4,  4);
         v3d_write(PCTRS5,  5);
         v3d_write(PCTRS6,  6);
         v3d_write(PCTRS7,  7);
         v3d_write(PCTRS8,  8);
         v3d_write(PCTRS9,  9);
         v3d_write(PCTRS10, 10);
         v3d_write(PCTRS11, 11);
         v3d_write(PCTRS12, 12);
         v3d_write(PCTRS13, 26);
         v3d_write(PCTRS14, 27);
      }

      /* enable counters */
      v3d_write(PCTRE, 0x80007FFF);

#ifndef CARBON
      if (s_perf_counters->l3c_group_active == 0)
      {
         v3d_write(PM_SEL, 1);
         v3d_write(PM_SEL, 0);
      }
      else if (s_perf_counters->l3c_group_active == 1)
      {
         v3d_write(PM_SEL, 1 | (1 << 2));
         v3d_write(PM_SEL, (1 << 2));
      }
#endif
   }
}

void khrn_hw_register_perf_counters(KHRN_DRIVER_COUNTERS_T *counters)
{
   s_perf_counters = counters;
}

void khrn_hw_unregister_perf_counters(void)
{
   s_perf_counters = 0;
}

static void update_perf_counters(void)
{
   if (s_perf_counters)
   {
      if (s_perf_counters->hw_group_active == 0)
      {
         s_perf_counters->hw.hw_0.qpu_cycles_idle                    += v3d_read(PCTR0);
         s_perf_counters->hw.hw_0.qpu_cycles_vert_shade              += v3d_read(PCTR1);
         s_perf_counters->hw.hw_0.qpu_cycles_frag_shade              += v3d_read(PCTR2);
         s_perf_counters->hw.hw_0.qpu_cycles_exe_valid               += v3d_read(PCTR3);
         s_perf_counters->hw.hw_0.qpu_cycles_wait_tmu                += v3d_read(PCTR4);
         s_perf_counters->hw.hw_0.qpu_cycles_wait_scb                += v3d_read(PCTR5);
         s_perf_counters->hw.hw_0.qpu_cycles_wait_vary               += v3d_read(PCTR6);
         s_perf_counters->hw.hw_0.qpu_icache_hits                    += v3d_read(PCTR7);
         s_perf_counters->hw.hw_0.qpu_icache_miss                    += v3d_read(PCTR8);
         s_perf_counters->hw.hw_0.qpu_ucache_hits                    += v3d_read(PCTR9);
         s_perf_counters->hw.hw_0.qpu_ucache_miss                    += v3d_read(PCTR10);
         s_perf_counters->hw.hw_0.tmu_total_quads                    += v3d_read(PCTR11);
         s_perf_counters->hw.hw_0.tmu_cache_miss                     += v3d_read(PCTR12);
         s_perf_counters->hw.hw_0.l2c_hits                           += v3d_read(PCTR13);
         s_perf_counters->hw.hw_0.l2c_miss                           += v3d_read(PCTR14);
      }
      else if (s_perf_counters->hw_group_active == 1)
      {
         s_perf_counters->hw.hw_1.fep_valid_prims                    += v3d_read(PCTR0);
         s_perf_counters->hw.hw_1.fep_valid_prims_no_pixels          += v3d_read(PCTR1);
         s_perf_counters->hw.hw_1.fep_earlyz_clipped_quads           += v3d_read(PCTR2);
         s_perf_counters->hw.hw_1.fep_valid_quads                    += v3d_read(PCTR3);
         s_perf_counters->hw.hw_1.tlb_quads_no_stencil_pass_pixels   += v3d_read(PCTR4);
         s_perf_counters->hw.hw_1.tlb_quads_no_z_stencil_pass_pixels += v3d_read(PCTR5);
         s_perf_counters->hw.hw_1.tlb_quads_z_stencil_pass_pixels    += v3d_read(PCTR6);
         s_perf_counters->hw.hw_1.tlb_quads_all_pixels_zero_cvg      += v3d_read(PCTR7);
         s_perf_counters->hw.hw_1.tlb_quads_all_pixels_nonzero_cvg   += v3d_read(PCTR8);
         s_perf_counters->hw.hw_1.tlb_quads_valid_pixels_written     += v3d_read(PCTR9);
         s_perf_counters->hw.hw_1.ptb_prims_viewport_discarded       += v3d_read(PCTR10);
         s_perf_counters->hw.hw_1.ptb_prims_needing_clip             += v3d_read(PCTR11);
         s_perf_counters->hw.hw_1.pse_prims_reverse_discarded        += v3d_read(PCTR12);
         s_perf_counters->hw.hw_1.vpm_cycles_vdw_stalled             += v3d_read(PCTR13);
         s_perf_counters->hw.hw_1.vpm_cycles_vcd_stalled             += v3d_read(PCTR14);
      }

#ifndef CARBON
      if (s_perf_counters->l3c_group_active == 0)
      {
         s_perf_counters->l3c.l3c_read_bw_0 += v3d_read(BW_CNT);
         s_perf_counters->l3c_mem.l3c_mem_read_bw_0 += v3d_read(BW_MEM_CNT);
      }
      else if (s_perf_counters->l3c_group_active == 1)
      {
         s_perf_counters->l3c.l3c_write_bw_1 += v3d_read(BW_CNT);
         s_perf_counters->l3c_mem.l3c_mem_write_bw_1 += v3d_read(BW_MEM_CNT);
      }
#endif

      reset_perf_counters();
   }
}

