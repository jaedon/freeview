/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
This is a default implementation of a Nexus platform layer used by V3D.
This illustrates one way in which the abstract memory interface might be
implemented. You can replace this with your own custom version if preferred.
=============================================================================*/

#include "nexus_memory.h"
#include "nexus_platform.h"
#include "nexus_graphics3d.h"
#if NEXUS_HAS_DMA
#include "nexus_dma.h"
#endif

#include "default_nexus.h"

/* Turn on to log memory access pattern to text file from 3D driver */
/*#define LOG_MEMORY_PATTERN*/

#include <EGL/egl.h>

#include <malloc.h>
#include <memory.h>
#include <sys/mman.h>
#include <fcntl.h>

static bool sUseDMA = false;

#ifdef LOG_MEMORY_PATTERN
static FILE *sLogFile = NULL;
#endif

enum
{
   eNO_PROBLEMS            = 0,
   eNOT_HD_GRAPHICS_HEAP   = 1 << 0,
   eCROSSES_256MB          = 1 << 1,
   eGUARD_BANDED           = 1 << 2,
   eWRONG_TYPE             = 1 << 3,
   eCROSSES_1GB            = 1 << 4,
   eTOO_SMALL              = 1 << 5
};

typedef struct 
{
   NEXUS_HeapHandle        heap;
   void                    *heapStartCached;
   uint32_t                heapStartPhys;
   uint32_t                heapSize;

} NXPL_HeapMapping;

typedef struct
{
   NXPL_HeapMapping        heaps[4];
   uint32_t                numHeaps;
   int                     l2_cache_size;
#if NEXUS_HAS_DMA
   BKNI_EventHandle        dmaEvent;
   NEXUS_DmaHandle         dmaHandle;
   NEXUS_DmaJobHandle      dmaJob;
#endif

} NXPL_MemoryData;

#ifdef SINGLE_PROCESS
#define NEXUS_HEAPCONFIG NEXUS_PlatformConfiguration
#else
#define NEXUS_HEAPCONFIG NEXUS_ClientConfiguration
#endif

void _NXPL_RecordExtraHeapMapping(BEGL_MemoryInterface *mem, NEXUS_HEAPHANDLE heap);

/*****************************************************************************
 * Memory interface
 *****************************************************************************/
/* Allocate aligned device memory, and return the cached address, or NULL on failure.*/
static void *MemAlloc(void *context, size_t numBytes, uint32_t alignment)
{
   NXPL_MemoryData               *data = (NXPL_MemoryData*)context;
   void                          *alloced = NULL;
   NEXUS_MemoryAllocationSettings settings;

   NEXUS_Memory_GetDefaultAllocationSettings(&settings);

   settings.alignment = alignment;
   settings.heap = data->heaps[0].heap;  /* Use the primary heap */

   NEXUS_Memory_Allocate(numBytes, &settings, &alloced);

#ifdef LOG_MEMORY_PATTERN
   if (sLogFile)
      fprintf(sLogFile, "A %u %u = %u\n", numBytes, alignment, (uint32_t)alloced);
#endif

   if (alloced == NULL)
   {
#ifndef NDEBUG
      if (data && data->heaps[0].heap)
      {
         NEXUS_MemoryStatus  status;
         NEXUS_Heap_GetStatus(data->heaps[0].heap, &status);

         printf("Platform layer MemAlloc failed : \n");
         printf("total heap size = %d\n", status.size);
         printf("num allocs = %d\n", status.numAllocs);
         printf("free = %d\n", status.free);
         printf("largest free block = %d\n", status.largestFreeBlock);
      }
#endif
      return NULL;
   }
   else
      return alloced;
}

/* Free a previously allocated block of device memory. Pass a cached address.*/
static void MemFree(void *context, void *pCached)
{
#ifdef LOG_MEMORY_PATTERN
   if (sLogFile)
      fprintf(sLogFile, "F %u\n", (uint32_t)pCached);
#endif

#ifndef NDEBUG
   if (pCached == 0)
      printf("NXPL : Trying to free NULL pointer\n");
#endif

   NEXUS_Memory_Free(pCached);
}

static bool IsValidHeapCachedPtr(void *context, void *pCached, uint32_t *retHeap)
{
   NXPL_MemoryData *data = (NXPL_MemoryData*)context;
   uint32_t        heapIndx = 0;

   // Find the heap
   for (heapIndx = 0; heapIndx < data->numHeaps; heapIndx++)
   {
      if (data->heaps[heapIndx].heapStartCached <= pCached && 
         data->heaps[heapIndx].heapStartCached + data->heaps[heapIndx].heapSize > pCached)
         break;
   }

   if (heapIndx >= data->numHeaps)
      return false;

   *retHeap = heapIndx;

   return true;
}

static uint32_t FindHeapIndexFromCached(void *context, void *pCached, bool warnings)
{
   NXPL_MemoryData *data = (NXPL_MemoryData*)context;
   uint32_t        heapIndx = 0;

   // Find the correct heap
   if (data->numHeaps > 1)
   {
      for (heapIndx = 0; heapIndx < data->numHeaps; heapIndx++)
      {
         if (data->heaps[heapIndx].heapStartCached <= pCached && 
            data->heaps[heapIndx].heapStartCached + data->heaps[heapIndx].heapSize > pCached)
            break;
      }

      if (heapIndx >= data->numHeaps)
      {
         if (warnings)
            printf("NXPL : Cannot find a heap which matches pointer %p\n", pCached);
         heapIndx = 0;
      }
   }

   return heapIndx;
}

static uint32_t FindHeapIndexFromPhysical(void *context, uint32_t offset)
{
   NXPL_MemoryData *data = (NXPL_MemoryData*)context;
   uint32_t        heapIndx = 0;

   // Find the correct heap
   if (data->numHeaps > 1)
   {
      for (heapIndx = 0; heapIndx < data->numHeaps; heapIndx++)
      {
         if (data->heaps[heapIndx].heapStartPhys <= offset && 
            data->heaps[heapIndx].heapStartPhys + data->heaps[heapIndx].heapSize > offset)
            break;
      }

      if (heapIndx >= data->numHeaps)
      {
         printf("NXPL : Cannot find a heap which matches physical offset %0x%08x\n", offset);
         heapIndx = 0;
      }
   }

   return heapIndx;
}

/* Return a physical device memory offset given a cached pointer. Returns 0 on failure.*/
/* This will work for the primary heap and any extra heaps that may have been registered. */
static uint32_t MemConvertCachedToPhysical(void *context, void *pCached)
{
   NXPL_MemoryData   *data = (NXPL_MemoryData*)context;
   uint32_t          heapIndx;

#ifndef NDEBUG
   if (pCached == 0)
      printf("NXPL : Trying to convert NULL pointer\n");
#endif

   heapIndx = FindHeapIndexFromCached(context, pCached, true);

   //printf("NXPL : MemConvertCachedToPhysical %p\n", data->heaps[heapIndx].heapStartPhys + ((uintptr_t)pCached - (uintptr_t)data->heaps[heapIndx].heapStartCached));

   return data->heaps[heapIndx].heapStartPhys + ((uintptr_t)pCached - (uintptr_t)data->heaps[heapIndx].heapStartCached);
}

/* Return a cached memory pointer given a physical device memory offset. Returns 0 on failure.*/
static void *MemConvertPhysicalToCached(void *context, uint32_t offset)
{
   NXPL_MemoryData   *data = (NXPL_MemoryData*)context;
   uint32_t          heapIndx = FindHeapIndexFromPhysical(context, offset);

   return (void*)((uintptr_t)data->heaps[heapIndx].heapStartCached + (offset - data->heaps[heapIndx].heapStartPhys));
}

/* Flush the cache for the given address range.*/
static void MemFlushCache(void *context, void *pCached, size_t numBytes)
{
   NXPL_MemoryData   *data = (NXPL_MemoryData*)context;
   uint32_t          heapIndx, i;
   bool              validPtr = IsValidHeapCachedPtr(context, pCached, &heapIndx);

   if (!validPtr || numBytes > data->heaps[heapIndx].heapSize)
   {
      // Flush the entire cache
      //NEXUS_Memory_FlushCache(0, 0x7FFFFFFF);

      for (i = 0; i < data->numHeaps; i++)
         NEXUS_Memory_FlushCache(data->heaps[i].heapStartCached, data->heaps[i].heapSize);
   }
   else
      NEXUS_Memory_FlushCache(pCached, numBytes);
}

/* Retrieve some information about the memory system */
static uint32_t MemGetInfo(void *context, BEGL_MemInfoType type)
{
   NXPL_MemoryData   *data = (NXPL_MemoryData*)context;

   switch (type)
   {
   case BEGL_MemHeapStartPhys :
      return data->heaps[0].heapStartPhys;
      break;
   case BEGL_MemHeapEndPhys   :
      return data->heaps[0].heapStartPhys + data->heaps[0].heapSize - 1;
      break;
   case BEGL_MemCacheLineSize :
      return data->l2_cache_size;
      break;

   case BEGL_MemLargestBlock  :
      {
#ifdef LOG_MEMORY_PATTERN
         if (sLogFile)
            fprintf(sLogFile, "B\n");
#endif
         struct NEXUS_MemoryStatus  status;
         NEXUS_Heap_GetStatus(data->heaps[0].heap, &status);
         return status.largestFreeBlock;
         break;
      }

   case BEGL_MemFree          :
      {
#ifdef LOG_MEMORY_PATTERN
         if (sLogFile)
            fprintf(sLogFile, "M\n");
#endif
         struct NEXUS_MemoryStatus  status;
         NEXUS_Heap_GetStatus(data->heaps[0].heap, &status);
         return status.free;
         break;
      }

   }

   return 0;
}

static void DMACompleteCallback(void *pParam, int iParam)
{
   BSTD_UNUSED(iParam);
   BKNI_SetEvent(pParam);
}

static void MemCopy(void *context, void *destCached, const void *srcCached, uint32_t bytes)
{
#if NEXUS_HAS_DMA
   NXPL_MemoryData   *data = (NXPL_MemoryData*)context;

   bool wantDma = false;

   uintptr_t start = (uintptr_t)data->heaps[0].heapStartCached;
   uintptr_t end = (uintptr_t)data->heaps[0].heapStartCached + data->heaps[0].heapSize - 1;

   /* For largish blocks with src & dest in the device memory heap we can DMA */
   if (bytes >= 128 * 1024 && bytes <= 8 * 1024 * 1024 &&
       (uintptr_t)destCached > start && (uintptr_t)destCached < end &&
       (uintptr_t)srcCached > start && (uintptr_t)srcCached < end)
   {
      wantDma = true;
   }

   if (wantDma)
   {
      NEXUS_Error                rc;
      NEXUS_DmaJobBlockSettings  blockSettings;
      NEXUS_DmaJobStatus         jobStatus;

      NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
      blockSettings.pSrcAddr = srcCached;
      blockSettings.pDestAddr = destCached;
      blockSettings.blockSize = bytes;
      blockSettings.cached = true;
      rc = NEXUS_DmaJob_ProcessBlocks(data->dmaJob, &blockSettings, 1);

      /*printf("DMA %d ------------------\n", bytes);*/

      if (rc == NEXUS_DMA_QUEUED) 
      {
         BKNI_WaitForEvent(data->dmaEvent, BKNI_INFINITE);
         NEXUS_DmaJob_GetStatus(data->dmaJob, &jobStatus);
         BDBG_ASSERT(jobStatus.currentState == NEXUS_DmaJobState_eComplete);
      }

      /* See comments in nexus_dma.h for cache flush rationale */
      MemFlushCache(context, destCached, bytes);
   }
   else
#endif
      memcpy(destCached, srcCached, bytes);
}

static bool IsValidV3DHeap(uint32_t *resultFlags, NEXUS_HEAPCONFIG *config, NEXUS_HeapHandle heap, 
                           bool has32bitSupport, bool relaxErrors)
{
   /* A valid V3D heap must satisfy all these requirements
      a) Must be HD displayable if in exclusive mode (since it will be used for framebuffers)
      b) Must not cross a 256MB memory boundary (for binner addressing limitation)
      c) Must not have guard banding turned on (simply too slow)
      d) Must have NEXUS_MEMORY_TYPE_APPLICATION_CACHED type flagged
      e) For V3D variants prior to B2, must not cross a 1GB memory boundary
      f) Must be at least 64MB
   */
   NEXUS_MemoryStatus            heapStatus;
   uint32_t                      hdDisplayMemController;
   uint32_t                      h, physStart, physEnd;

   *resultFlags = eNO_PROBLEMS;

#ifdef SINGLE_PROCESS
   /* Get the display heap info */
#ifdef BLURAY_PLATFORM
   NEXUS_Heap_GetStatus(config->heap[0], &heapStatus);
#else
   NEXUS_Heap_GetStatus(NEXUS_Platform_GetFramebufferHeap(0), &heapStatus);
#endif

   hdDisplayMemController = heapStatus.memcIndex;

   /* Get the passed heap's info */
   NEXUS_Heap_GetStatus(heap, &heapStatus);

   /* a) The heap given must be on the same memory controller as the HD display heap 
         But only if in exclusive display mode. */
   if (heapStatus.memcIndex != hdDisplayMemController)
      *resultFlags |= eNOT_HD_GRAPHICS_HEAP;
#else
   /* Get the passed heap's info */
   NEXUS_Heap_GetStatus(heap, &heapStatus);
#endif /* !SINGLE_PROCESS */

   /* d) Check for heap type */
   if ((heapStatus.memoryType & NEXUS_MEMORY_TYPE_APPLICATION_CACHED) == 0)
      *resultFlags |= eWRONG_TYPE;

   physStart = heapStatus.offset;
   physEnd = heapStatus.offset + (heapStatus.size - 1);
   //printf("Start = %p, End = %p\n", physStart, physEnd);

   if (!relaxErrors)
   {
      /* f) Must be at least 64MB */
      if (heapStatus.size < 64 * 1024 * 1024)
         *resultFlags |= eTOO_SMALL;

      /* b) Check for 256MB boundary crossing */
      if ((physStart & 0xF0000000) != (physEnd & 0xF0000000))
         *resultFlags |= eCROSSES_256MB;

      /* c) Check for guard banding */
      if (heapStatus.guardBanding)
         *resultFlags |= eGUARD_BANDED;
   }

   if (!has32bitSupport)
   {
      /* e) Check for 1GB boundary crossing if applicable */
      if ((physStart & 0xC0000000) != (physEnd & 0xC0000000))
         *resultFlags |= eCROSSES_1GB;
   }

   return (*resultFlags == eNO_PROBLEMS);
}

static void ShowNoValidHeapError(const char *msg, uint32_t reasons)
{
   char err[1024];
   char tail[] = "Please contact the 3D team and your chip lead to arrange\n"
                 "creation of a suitable Nexus heap\n";
   sprintf(err, "%s\n\n"
                   "A suitable V3D heap must match these requirements:\n"
                   "a) Must be HD displayable if in exclusive display mode\n"
                   "b) Must not cross a 256MB memory boundary (for binner addressing limit)\n"
                   "c) Must not have guard banding turned on (simply too slow)\n"
                   "d) Must have NEXUS_MEMORY_TYPE_APPLICATION_CACHED type flagged\n"
                   "e) For V3D variants prior to B2, must not cross a 1GB memory boundary\n"
                   "f) Must be at least 64MB\n\n", msg);

   if (reasons != 0)
   {
      char err1[1024] = "Your heap has failed tests: ";
      if (reasons & eNOT_HD_GRAPHICS_HEAP)
         strcat(err1, "a ");
      if (reasons & eCROSSES_256MB)
         strcat(err1, "b ");
      if (reasons & eGUARD_BANDED)
         strcat(err1, "c ");
      if (reasons & eWRONG_TYPE)
         strcat(err1, "d ");
      if (reasons & eCROSSES_1GB)
         strcat(err1, "e ");
      if (reasons & eTOO_SMALL)
         strcat(err1, "f ");
      strcat(err1, "\n\n\n");
      strcat(err, err1);
   }

   strcat(err, tail);

#ifdef ANDROID
   LOGD("%s", err);
#else
   fputs(err, stderr);
#endif /* ANDROID */
}

static bool CheckHeap(NXPL_MemoryData *data, uint32_t *reasons, NEXUS_HEAPCONFIG *config, bool has32bitSupport, 
                      bool relaxErrors, uint32_t heapType, const char *name)
{
   char err[256];

   // Check without relaxation first
   if (IsValidV3DHeap(reasons, config, NEXUS_Platform_GetFramebufferHeap(heapType), has32bitSupport, false))
   {
      /*printf("V3D using NEXUS_Platform_GetFramebufferHeap(%s)\n", name);*/
      data->heaps[0].heap = NEXUS_Platform_GetFramebufferHeap(heapType);
      return true;
   }
   else
   {
      sprintf(err, "NEXUS_Platform_GetFramebufferHeap(%s) is not valid for V3D use", name);
      ShowNoValidHeapError(err, *reasons);

      // Check relaxed if allowed
      if (relaxErrors)
      {
         if (IsValidV3DHeap(reasons, config, NEXUS_Platform_GetFramebufferHeap(heapType), has32bitSupport, true))
         {
            /*printf("V3D using relaxed NEXUS_Platform_GetFramebufferHeap(%s)\n", name);*/
            data->heaps[0].heap = NEXUS_Platform_GetFramebufferHeap(heapType);
            return true;
         }
      }
      else
      {
         BDBG_ERR(("Cannot continue"));
         return false;
      }
   }

   return false;
}

static void DebugHeap(NEXUS_HeapHandle heap)
{
   NEXUS_MemoryStatus   status;
   NEXUS_Error          rc;

   rc = NEXUS_Heap_GetStatus(heap, &status);
   BDBG_ASSERT(!rc);
   
   printf("MEMC%d, physical addr 0x%08x, size %9d (0x%08x), alignment %2d, base ptr %p, high water %9d, guardbanding? %c\n",
         status.memcIndex, status.offset, status.size, status.size, status.alignment, status.addr,
         status.highWatermark,
         status.guardBanding?'y':'n');
}

static bool CalculateBestHeaps(NXPL_MemoryData *data, BEGL_HWInterface *hwIface)
{
   /* We will analyze all available heaps and use a priority mechanism to select the
      best match for V3D to use. 
      
      if (!relaxErrors)
      {
         1) use NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE) if it meets requirements
         2) otherwise, fail
      }
      else
      {
         1) use NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE) if it meets requirements
         2) use relaxed NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE) if it meets requirements (and relaxation allowed)
         3) search for any suitable heap without relaxation
         4) search for any suitable heap with relaxation (if relaxation allowed)
      }

      See IsValidV3DHeap() for a list of V3D heap requirements.
   */
   NEXUS_HEAPCONFIG              config;
   uint32_t                      h;
   NEXUS_HeapHandle              fbHeap;
   int32_t                       fbHeapIndex = -1;
   NEXUS_MemoryStatus            heapStatus;
   unsigned                      fbMemc;
   int32_t                       bestScore = 0;
   uint32_t                      bestHeap = 0;
   uint32_t                      reasons;
   bool                          relaxErrors = false;
   char                          *val;
   bool                          has32bitSupport = false;

   /* We first need to see if 32-bit addressing is supported by this version of the V3D core */
   uint32_t ident0   = hwIface->ReadStatus(hwIface->context, NEXUS_Graphics3dCommandId_eIDENT0);
   uint32_t ident1   = hwIface->ReadStatus(hwIface->context, NEXUS_Graphics3dCommandId_eIDENT1);
   uint32_t techRev  = (ident0 >> 24);
   uint32_t revision = (ident1 & 0xF);

   if (ident0 == 0 || ident1 == 0)
      fprintf(stderr, "*** ERROR : couldn't read IDENT registers. ***\n");
   else
   {
      if (techRev >= 3 && revision >= 2)
      {
         /* This is B2 or better */
         has32bitSupport = true;
      }
   }

#if defined(DIRECTFB)
   /* in these systems, the memory is provided by the window manager */
   relaxErrors = true;
#else
   /* ONLY to be used as an emergency workaround */
   val = getenv("V3D_RELAX_HEAP_ERRORS");
   if (val != NULL && (val[0] == 't' || val[0] == 'T' || val[0] == '1' || val[0] == 'y'|| val[0] == 'Y'))
      relaxErrors = true;
#endif

#ifdef SINGLE_PROCESS
   NEXUS_Platform_GetConfiguration(&config);
#else
   NEXUS_Platform_GetClientConfiguration(&config);
#endif /* !SINGLE_PROCESS */

   /* ONLY to be used as an emergency workaround */
   val = getenv("V3D_FORCE_HEAP_INDEX");
   if (val != NULL)
   {
      uint32_t indx = atoi(val);
      if (indx < NEXUS_MAX_HEAPS)
      {
         printf("V3D USING FORCED HEAP INDEX %d\n", indx);
         if (!IsValidV3DHeap(&reasons, &config, config.heap[indx], has32bitSupport, false))
            ShowNoValidHeapError("Using FORCED heap index :", reasons);

         data->heaps[0].heap = config.heap[indx];
         return true;
      }
   }

#ifdef SINGLE_PROCESS

#if defined(NEXUS_GRAPHICS_HEAP)
   if (CheckHeap(data, &reasons, &config, has32bitSupport, relaxErrors, NEXUS_GRAPHICS_HEAP, "NEXUS_GRAPHICS_HEAP"))
      return true;
#elif defined(NEXUS_OFFSCREEN_SURFACE)
   if (CheckHeap(data, &reasons, &config, has32bitSupport, relaxErrors, NEXUS_OFFSCREEN_SURFACE, "NEXUS_OFFSCREEN_SURFACE"))
      return true;
#else
   if (CheckHeap(data, &reasons, &config, has32bitSupport, relaxErrors, 0, "0"))
      return true;
#endif

   if (relaxErrors)
#endif /* SINGLE_PROCESS */
   {
      /* 3) search all heaps for one that meets non-relaxed requirements */
      for (h = 0; h < NEXUS_MAX_HEAPS; h++)
      {
         if (config.heap[h])
         {
            if (IsValidV3DHeap(&reasons, &config, config.heap[h], has32bitSupport, false))
            {
               /*
               printf("V3D using heap index %d\n", h);
               DebugHeap(config.heap[h]);
               */
               data->heaps[0].heap = config.heap[h];
               return true;
            }
         }
      }

      if (relaxErrors)
      {
         /* 4) search all heaps for one that meets relaxed requirements */
         for (h = 0; h < NEXUS_MAX_HEAPS; h++)
         {
            if (config.heap[h])
            {
               if (IsValidV3DHeap(&reasons, &config, config.heap[h], has32bitSupport, true))
               {
                  /*
                  printf("V3D using relaxed heap index %d\n", h);
                  DebugHeap(config.heap[h]);
                  */
                  data->heaps[0].heap = config.heap[h];
                  return true;
               }
            }
         }
      }

      ShowNoValidHeapError("Failed to find any suitable heap for V3D", 0);
   }

   return false;
}

BEGL_MemoryInterface *NXPL_CreateMemInterface(BEGL_HWInterface *hwIface)
{
   NXPL_MemoryData *data;
   BEGL_MemoryInterface *mem = (BEGL_MemoryInterface*)malloc(sizeof(BEGL_MemoryInterface));

   // Read the use DMA environment variable
   char *val = getenv("V3D_USE_DMA");
   if (val != NULL && (val[0] == 't' || val[0] == 'T' || val[0] == '1' || val[0] == 'y'|| val[0] == 'Y'))
      sUseDMA = true;

#ifdef LOG_MEMORY_PATTERN
   sLogFile = fopen("MemoryLog.txt", "w");
#endif

   if (mem != NULL)
   {
      data = (NXPL_MemoryData*)malloc(sizeof(NXPL_MemoryData));
      memset(mem, 0, sizeof(BEGL_MemoryInterface));

      if (data != NULL)
      {
         NEXUS_MemoryStatus memStatus;
         int                rac_size, l2_size;
         FILE               *fp;

#ifndef SINGLE_PROCESS
         NEXUS_ClientConfiguration  clientConfig;
#endif
         memset(data, 0, sizeof(NXPL_MemoryData));

         data->numHeaps = 1;

         mem->context = (void*)data;
         mem->Alloc = MemAlloc;
         mem->Free = MemFree;
         mem->FlushCache = MemFlushCache;
         mem->ConvertCachedToPhysical = MemConvertCachedToPhysical;
         mem->ConvertPhysicalToCached = MemConvertPhysicalToCached;
         mem->GetInfo = MemGetInfo;

         if (sUseDMA)
            mem->MemCopy = MemCopy;

         if (!CalculateBestHeaps(data, hwIface))
            goto error1;

         NEXUS_Heap_GetStatus(data->heaps[0].heap, &memStatus);
         data->heaps[0].heapStartCached = memStatus.addr;
         data->heaps[0].heapStartPhys = memStatus.offset;
         data->heaps[0].heapSize = memStatus.size;

#ifndef NDEBUG
         printf("NXPL : NXPL_CreateMemInterface() INFO.\nVirtual (cached) %p, Physical %p, Size %p\n",
            data->heaps[0].heapStartCached,
            data->heaps[0].heapStartPhys,
            data->heaps[0].heapSize);
#endif

         /* grab the cache sizes, get them from the linux kernel via the sysfs interface */
         fp = fopen("/sys/bus/platform/devices/brcmstb/cache_max_writeback", "r");
         if (fp != NULL)
         {
            /* read cache_max_writeback, if available */
            fscanf(fp, "%d", &l2_size);
#ifndef NDEBUG
            printf("NXPL : cache line size = %d\n", l2_size);
#endif
            fclose(fp);
         }

         fp = fopen("/sys/bus/platform/devices/brcmstb/cache_max_writethrough", "r");
         if (fp != NULL)
         {
            /* read cache_max_writeback, if available */
            fscanf(fp, "%d", &rac_size);
#ifndef NDEBUG
            printf("NXPL : RAC line size = %d\n", rac_size);
#endif
            fclose(fp);
         }

         /* take the max of the two */
         data->l2_cache_size = (l2_size > rac_size) ? l2_size : rac_size;

#if NEXUS_HAS_DMA
         if (sUseDMA)
         {
            /* Setup for DMA */
            NEXUS_DmaJobSettings       jobSettings;
            NEXUS_DmaJobHandle         dmaJob;
            NEXUS_DmaJobBlockSettings  blockSettings;
            NEXUS_DmaJobStatus         jobStatus;

            data->dmaHandle = NEXUS_Dma_Open(0, NULL);
            BKNI_CreateEvent(&data->dmaEvent);

            NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
            jobSettings.numBlocks = 1;
            jobSettings.completionCallback.callback = DMACompleteCallback;
            jobSettings.completionCallback.context = data->dmaEvent;

            data->dmaJob = NEXUS_DmaJob_Create(data->dmaHandle, &jobSettings);
         }
#endif
      }
      else
      {
         goto error0;
      }
   }
   return mem;

error1:
   free(data);

error0:
   free(mem);
   return NULL;
}

void NXPL_DestroyMemInterface(BEGL_MemoryInterface *mem)
{
   if (mem != NULL)
   {
      if (mem->context != NULL)
      {
         NXPL_MemoryData *data = (NXPL_MemoryData*)mem->context;

#if NEXUS_HAS_DMA
         if (sUseDMA)
         {
            if (data->dmaJob)
               NEXUS_DmaJob_Destroy(data->dmaJob);
            if (data->dmaHandle)
               NEXUS_Dma_Close(data->dmaHandle);
            if (data->dmaEvent)
               BKNI_DestroyEvent(data->dmaEvent);
         }
#endif
         free(mem->context);
      }

      memset(mem, 0, sizeof(BEGL_MemoryInterface));
      free(mem);

#ifdef LOG_MEMORY_PATTERN
      if (sLogFile)
      {
         fclose(sLogFile);
         sLogFile = NULL;
      }
#endif
   }
}

/* Return the primary memory heap */
NEXUS_HeapHandle NXPL_MemHeap(BEGL_MemoryInterface *mem)
{
   if (mem != NULL)
   {
      NXPL_MemoryData *data = (NXPL_MemoryData*)mem->context;
      return data->heaps[0].heap;
   }
   return NULL;
}

/* Called internally if multiple heaps are being used. Not currently used. */
void _NXPL_RecordExtraHeapMapping(BEGL_MemoryInterface *mem, NEXUS_HEAPHANDLE heap)
{
   if (mem != NULL)
   {
      NXPL_MemoryData      *data = (NXPL_MemoryData*)mem->context;
      NEXUS_MemoryStatus   memStatus;
      uint32_t             heapIndx;

      /* Check that heap isn't already recorded, or the same as our primary off-screen heap */
      for (heapIndx = 0; heapIndx < data->numHeaps; heapIndx++)
         if (data->heaps[heapIndx].heap == heap)
            return;

      data->heaps[data->numHeaps].heap = heap;

      NEXUS_Heap_GetStatus(data->heaps[data->numHeaps].heap, &memStatus);
      data->heaps[data->numHeaps].heapStartCached = memStatus.addr;
      data->heaps[data->numHeaps].heapStartPhys = memStatus.offset;
      data->heaps[data->numHeaps].heapSize = memStatus.size;

      /* TODO - Do we need an uncached mapping for 'extra' heaps??? */

      data->numHeaps++;
   }
}