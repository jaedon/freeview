#if !(defined(WIN32) || defined(__mips__))
#define KHRN_LLAT_OTHER_CORE
#endif

/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Low-latency thread

FILE DESCRIPTION
Implementation.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/common/khrn_options.h"
#include "interface/khronos/common/khrn_int_util.h"
#include "middleware/khronos/common/khrn_hw.h"
#include "middleware/khronos/common/khrn_llat.h"

#include "vcfw/drivers/chip/abstract_v3d.h"

#ifdef ANDROID
#include <cutils/log.h>
#endif

#ifdef __linux__
#define __USE_GNU
#include <dlfcn.h>
#endif /* __linux__ */

extern uint32_t s_debugLastBinStart;
extern uint32_t s_debugLastRenderStart;
extern uint32_t acquire_n;

#include <errno.h>
extern void khrn_print_backtrace(void);
extern void khrn_hw_isr_no_args(void);

#define CALLBACKS_N 5
#define THREAD_PRIORITY VCOS_THREAD_PRI_ABOVE_NORMAL
#define THREAD_STACK_SIZE 8192 /* todo: how big does the stack need to be? */

static bool inited = false;
static KHRN_LLAT_CALLBACK_T callbacks[CALLBACKS_N];
vcos_static_assert(CALLBACKS_N < 32); /* 32 flags. */
static VCOS_ATOMIC_FLAGS_T event_flags;
static VCOS_EVENT_T event;
static VCOS_THREAD_T thread;

static void process(uint32_t flags)
{
   vcos_assert(inited);

   while (flags) {
      uint32_t i;
      KHRN_LLAT_CALLBACK_T callback;

      i = _msb(flags);
      flags &= ~(1 << i);
      vcos_assert(i < CALLBACKS_N);
      callback = callbacks[i];
      vcos_assert(callback);
      callback();
   }
}

static bool gpu_recover(void)
{
   /* So, for some obscure reason the GPU has locked up - let's try to sort things out and carry on */
   bool binInProgress = false;
   bool rdrInProgress = false;
   int  pendingBins = 0;
   int  pendingRdrs = 0;
   uint32_t flags;

   /* First see if we have a pending IRQ that just hasn't fired for some reason */
   if (khrn_workarounds.GFXH724)
   {
      flags =  v3d_read(INTCTL);
      flags &= v3d_read(INTENA);
      flags &= v3d_read(INTCTL);
   }
   else
      flags = v3d_read(INTCTL) >> 16;

   if (flags)
   {
#ifdef LOGD
      LOGD("GPU Recover : ISR lost - firing manually\n");
#else
      printf("GPU Recover : ISR lost - firing manually\n");
#endif
      /* Manually fire the ISR handler */
      khrn_hw_isr_no_args();
      return true;
   }

#ifdef LOGD
   LOGD("GPU Fatal Error\n");
#else
   printf("GPU Fatal Error\n");
#endif
   return false;
}

static void lockup_clif_dump(void)
{
#ifndef DISABLE_OPTION_PARSING
   uint32_t cols = 0;
   uint32_t lastProg = 0;
   static bool dumped = false;

   if (!khrn_options.clif_dump_on_lock)
      return;

   if (!dumped)
   {
      FILE *fp = fopen("lockup_dump.clif", "w");
      FILE *md = fopen("memdump.bin", "w");

      if (fp == NULL || md == NULL)
      {
         printf("Could not open lockup_dump.clif for writing\n");
         if (fp != NULL)
            fclose(fp);
         if (md != NULL)
            fclose(md);
      }
      else
      {
         uint32_t *start = (uint32_t*)mem_min_cached_addr();
         uint32_t *end = (uint32_t*)mem_max_cached_addr();
         uint32_t physStart = mem_map_cached_to_physical((void*)mem_min_cached_addr());
         uint32_t progress = 0;
         uint32_t lastProg = 0;
         uint32_t bytes = (uintptr_t)end - (uintptr_t)start;

         mem_flush_cache(); /* Flush & invalidate cache */

         dumped = true;
         printf("Creating lockup_dump.clif for debugging\n");

         fprintf(fp, "@set_memory_base 0x%08X\n", physStart);
         fprintf(fp, "#memory end = 0x%08X\n", mem_map_cached_to_physical((void*)mem_max_cached_addr()));

         fprintf(fp, "@createbuf memory\n");
         fprintf(fp, "@format file 0 0x%08X memdump.bin\n", bytes);

         fwrite(start, 1, bytes, md);

         if (v3d_read(CT0CA) == v3d_read(CT0EA))
            fprintf(fp, "#");

         fprintf(fp, "@set_bin_start_end [memory+0x%08X] [memory+0x%08X]\n", 
            s_debugLastBinStart, v3d_read(CT0EA));

         if (v3d_read(CT0CA) != v3d_read(CT0EA))
           fprintf(fp, "\n# Lockup was at %p\n", (void *)v3d_read(CT0CA));

         if (v3d_read(CT1CA) == v3d_read(CT1EA))
            fprintf(fp, "#");

         fprintf(fp, "@set_render_start_end [memory+0x%08X] [memory+0x%08X]\n",
            s_debugLastRenderStart, v3d_read(CT1EA));

         if (v3d_read(CT1CA) != v3d_read(CT1EA))
            fprintf(fp, "\n# Lockup was at %p\n", (void *)v3d_read(CT1CA));

         printf("\nDone\n");
         fclose(md);
         fclose(fp);
         _exit(0);
      }
   }
#endif
}

#ifndef DISABLE_OPTION_PARSING
#ifndef WIN32
unsigned int v3d_debug_read_pc(int, int) __attribute__ ((weak));
#endif
void do_reg_dump(void)
{
#ifdef ANDROID
   LOGD("\nGPU Recovery Register Dump:\n"
#else
   printf("\nGPU Recovery Register Dump:\n"
#endif /* ANDROID */
      "MIN_PHYS  %08X,  MAX_PHYS %08X\n"
      "INTCTL    %08X,  INTENA   %08X\n"
      "BIN_START %08X,  CT0CA    %08X,  CT0EA  %08X,  CT0CS  %08X\n"
      "RDR_START %08X,  CT1CA    %08X,  CT1EA  %08X,  CT1CS  %08X\n"
      "CT00RA0   %08X,  CT0LC    %08X,  CT0PC  %08X\n"
      "CT01RA0   %08X,  CT1LC    %08X,  CT1PC  %08X\n"
      "PCS       %08X,  BFC      %08X,  RFC    %08X\n"
      "BPCA      %08X,  BPCS     %08X,  BPOA   %08X,  BPOS   %08X\n"
      "VPMBASE   %08X,  VPACNTL  %08X\n"
      "L2CACTL   %08X,  SLCACTL  %08X\n"
      "DBQITC    %08X,  DBQITE   %08X\n"
      "SCRATCH   %08X\n"
      "IDENT0    %08X,  IDENT1   %08X,  IDENT2 %08X,  IDENT3 %08X\n"
      "ERRSTAT   %08X,  DBGE     %08X,  FDBGO  %08X,  FDBGB  %08X\n"
      "FDBGR     %08X,  FDBGS    %08X,  BXCF   %08X\n\n",

      mem_map_cached_to_physical((void*)mem_min_cached_addr()),
      mem_map_cached_to_physical((void*)mem_max_cached_addr()),
      v3d_read(INTCTL),
      v3d_read(INTENA),
      s_debugLastBinStart,
      v3d_read(CT0CA),
      v3d_read(CT0EA),
      v3d_read(CT0CS),
      s_debugLastRenderStart,
      v3d_read(CT1CA),
      v3d_read(CT1EA),
      v3d_read(CT1CS),
      v3d_read(CT00RA0),
      v3d_read(CT0LC),
      v3d_read(CT0PC),
      v3d_read(CT01RA0),
      v3d_read(CT1LC),
      v3d_read(CT1PC),
      v3d_read(PCS),
      v3d_read(BFC),
      v3d_read(RFC),
      v3d_read(BPCA),
      v3d_read(BPCS),
      v3d_read(BPOA),
      v3d_read(BPOS),
      v3d_read(VPMBASE),
      v3d_read(VPACNTL),
      v3d_read(L2CACTL),
      v3d_read(SLCACTL),
      v3d_read(DBQITC),
      v3d_read(DBQITE),
      v3d_read(SCRATCH),
      v3d_read(IDENT0),
      v3d_read(IDENT1),
      v3d_read(IDENT2),
      v3d_read(IDENT3),
      v3d_read(ERRSTAT),
      v3d_read(DBGE),
      v3d_read(FDBGO),
      v3d_read(FDBGB),
      v3d_read(FDBGR),
      v3d_read(FDBGS),
      v3d_read(BXCF)
      );

   v3d_write(DBCFG, 0x01); /* enable debugging */

#if defined(__linux__) && defined(__GNUC__)
   if (v3d_debug_read_pc)
   {
      uint32_t qpu = 0;
      for (qpu = 0; qpu < 12; qpu++)
      {
         unsigned int pc0 = v3d_debug_read_pc(qpu, 0);
         unsigned int pc1 = v3d_debug_read_pc(qpu, 1);

         /* v3d_debug_read_pc doesnt exist on ANDROID, so these dont need converting to LOGD */
         printf("QPU %2d : ", qpu);
         printf("PC0 = %08X, PC1 = %08X\n", pc0, pc1);
      }
   }
#endif /* __linux__ && __GNUC__ */

   v3d_write(DBCFG, 0x00); /* disable debugging */
}
#endif

static void *khrn_llat_main(void *p)
{
   UNUSED(p);

#ifdef PRINT_CLIST_DEBUG
   static bool dumpedAfterLock = false;
#endif

   for (;;) {
      uint32_t flags;

#if defined(__linux__) && defined(__GNUC__)

      bool done = false;
      uint32_t   timeoutCount = 0;
      uint32_t   prevCT0CA = 0xFFFFFFFF;
      uint32_t   prevCT1CA = 0xFFFFFFFF;

      while (!done)
      {
         struct timespec ts;
         int s;
         clock_gettime(CLOCK_REALTIME, &ts);

         ts.tv_nsec += (1000000 * 20);   /* 20ms */
         if (ts.tv_nsec >= 1000000000)    /* Handle overflow */
         {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec++;
         }

         while ((s = sem_timedwait(&event, &ts)) == -1 && errno == EINTR)
            continue;       /* Restart if interrupted by handler */
         if (s == -1)
         {
            if (errno != ETIMEDOUT)
               done = true;
            else
            {
               if (acquire_n > 0)
               {
                  /* When we see the first timeout, clear the instruction and uniform caches for all slices immediately */
                  if (khrn_workarounds.GFXH16)
                  {
                     v3d_write(SLCACTL, 0xF0F);
                  }
                  /* Is this a genuine lockup? - use a 2 second timeout */
                  timeoutCount++;
                  if (timeoutCount > 100 && prevCT0CA == v3d_read(CT0CA) && prevCT1CA == v3d_read(CT1CA))
                  {
                     /* At least the second time we've hit the timeout, and the progress registers show we
                        haven't made any progress. */

#ifndef DISABLE_OPTION_PARSING
                     if (khrn_options.reg_dump_on_lock)
                        do_reg_dump();
#endif
                     lockup_clif_dump();

#ifdef PRINT_CLIST_DEBUG
                     if (!dumpedAfterLock)
                     {
                        FILE *fp;
                        char fname[64];
                        sprintf(fname, "LOCKED_%08x.dat", (uintptr_t)khrn_hw_addr(dbg_render_begin));
                        fp = fopen(fname, "w");
                        if (fp != NULL)
                        {
                           control_list_debug_file(fp);
                           control_list_debug_dump((void *)khrn_hw_addr(dbg_render_begin), (void *)khrn_hw_addr(dbg_render_end), 1);
                           fclose(fp);
                           control_list_debug_file(NULL);
                        }

                        dumpedAfterLock = true;
                     }
#endif

                     /* do a backtrace, if available */
                     if (khrn_options.reg_dump_on_lock)
                        khrn_print_backtrace();

                     /* Try to recover */
                     done = gpu_recover();

                     /* Reset statics */
                     timeoutCount = 0;
                     prevCT0CA = prevCT1CA = 0;
                  }

                  /* Remember these */
                  prevCT0CA = v3d_read(CT0CA);
                  prevCT1CA = v3d_read(CT1CA);
               }
               else
               {
                  timeoutCount++;
                  if (timeoutCount > 250)
                  {
                     /* do a backtrace, if available */
                     if (khrn_options.reg_dump_on_lock)
                     {
#ifdef ANDROID
                        LOGD("TIMEOUT WAITING FOR LLAT\n");
#else
                        if (khrn_options.gl_error_assist)
                           printf("TIMEOUT WAITING FOR LLAT\n");
#endif
                     }

                     timeoutCount = 0;
                  }
               }
            }
         }
         else
            done = true;
      }
#else
      vcos_event_wait(&event);
#endif
      flags = vcos_atomic_flags_get_and_clear(&event_flags);
      khrn_barrier();

      if (flags & (1 << 31)) {
         vcos_assert(flags == (uint32_t)(1 << 31)); /* all callbacks should have been unregistered by now */
         return NULL;
      }

      process(flags);
   }
}

bool khrn_llat_init(void)
{
   uint32_t i;
   VCOS_THREAD_ATTR_T attr;

   vcos_assert(!inited);

   for (i = 0; i != CALLBACKS_N; ++i) {
      callbacks[i] = NULL;
   }

   if (vcos_atomic_flags_create(&event_flags) != VCOS_SUCCESS) {
      return false;
   }

   if (vcos_event_create(&event, "khrn_llat_event") != VCOS_SUCCESS) {
      vcos_atomic_flags_delete(&event_flags);
      return false;
   }

   vcos_thread_attr_init(&attr);
   vcos_thread_attr_setpriority(&attr, THREAD_PRIORITY);
#ifdef KHRN_LLAT_OTHER_CORE
   switch (vcos_thread_get_affinity(vcos_thread_current())) {
   case VCOS_AFFINITY_CPU0: vcos_thread_attr_setaffinity(&attr, VCOS_AFFINITY_CPU1); break;
   case VCOS_AFFINITY_CPU1: vcos_thread_attr_setaffinity(&attr, VCOS_AFFINITY_CPU0); break;
   }
#endif
   vcos_thread_attr_setstacksize(&attr, THREAD_STACK_SIZE);
   if (vcos_thread_create(&thread, "khrn_llat_thread", &attr, khrn_llat_main, NULL) != VCOS_SUCCESS) {
      vcos_event_delete(&event);
      vcos_atomic_flags_delete(&event_flags);
      return false;
   }

   inited = true;

   return true;
}

void khrn_llat_term(void)
{
#ifndef NDEBUG
   uint32_t i;
#endif

   vcos_assert(inited);

   khrn_llat_notify(31); /* ask the thread to exit */
   vcos_thread_join(&thread, NULL);

   vcos_event_delete(&event);

   vcos_assert(!vcos_atomic_flags_get_and_clear(&event_flags));
   vcos_atomic_flags_delete(&event_flags);

#ifndef NDEBUG
   for (i = 0; i != CALLBACKS_N; ++i) {
      vcos_assert(!callbacks[i]);
   }
#endif

   inited = false;
}

uint32_t khrn_llat_register(KHRN_LLAT_CALLBACK_T callback)
{
   uint32_t i;

   vcos_assert(inited);

   for (i = 0; i != CALLBACKS_N; ++i) {
      if (!callbacks[i]) {
         callbacks[i] = callback;
         return i;
      }
   }

   UNREACHABLE(); /* increase CALLBACKS_N? */
   return -1;
}

void khrn_llat_unregister(uint32_t i)
{
   vcos_assert(inited);

   vcos_assert(callbacks[i]);
   callbacks[i] = NULL;
}

void khrn_llat_notify(uint32_t i)
{
   vcos_assert(inited);

   khrn_barrier();
   vcos_atomic_flags_or(&event_flags, 1 << i);
   vcos_event_signal(&event);
}

static bool waited;

static void wait_callback(void)
{
   khrn_barrier();
   waited = true;
   khrn_sync_notify_master();
}

void khrn_llat_wait(void)
{
   uint32_t i = khrn_llat_register(wait_callback), j;
   for (j = 0; j != 2; ++j) {
      waited = false;
      khrn_llat_notify(i);
      while (!waited) {
         khrn_sync_master_wait_timeout(-1);
      }
      khrn_barrier();
   }
   khrn_llat_unregister(i);
}
