/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg.c $
 * $brcm_Revision: Hydra_Software_Devel/67 $
 * $brcm_Date: 10/5/11 12:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/bdbg.c $
 * 
 * Hydra_Software_Devel/67   10/5/11 12:58p vsilyaev
 * SW7405-4477: Fixed typo
 * 
 * Hydra_Software_Devel/66   6/20/11 11:06a vsilyaev
 * SW7405-5221: Fixed Enter/Leave function
 * 
 * Hydra_Software_Devel/65   6/16/11 1:40p vsilyaev
 * SW7425-729,SW7400-3012: Added LOG level
 * 
 * Hydra_Software_Devel/64   6/6/11 3:27p vsilyaev
 * SW7405-4477: Routed all debug output through buffer and use external
 * application to extract and print debug output
 * 
 * Hydra_Software_Devel/63   4/13/11 7:00p vsilyaev
 * SW7405-5221: Route BERR_TRACE through the debug log
 * 
 * Hydra_Software_Devel/62   4/5/11 11:35a vsilyaev
 * SW7405-5221: Fixed typo
 * 
 * Hydra_Software_Devel/61   4/4/11 6:14p vsilyaev
 * SW7405-5221: Added option to forward debug output to FIFO
 * 
 * Hydra_Software_Devel/60   1/5/11 6:31p vsilyaev
 * SW7405-4477: Backed up code that acquires lock for duration of
 * BKNI_Printf
 * 
 * Hydra_Software_Devel/59   11/3/10 10:00a erickson
 * SW7405-4477: add lock/unlock to BDBG_P_TestAndPrint so that task/isr
 * DBG output is not interleaved
 * 
 * Hydra_Software_Devel/58   10/11/10 5:23p jgarrett
 * SW7125-630: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7125-630/2   10/11/10 1:46p mward
 * SW7125-630:  Merge from main.
 * 
 * Hydra_Software_Devel/57   8/3/10 11:41a erickson
 * SW7400-2857: reduce stack usage by BDBG_OBJECT_ASSERT
 *
 * Hydra_Software_Devel/SW7125-630/1   10/1/10 5:47p jgarrett
 * SW7125-630: Removing potential malloc with BDBG_Lock held
 * 
 * Hydra_Software_Devel/56   7/13/10 12:59p vsilyaev
 * SW7420-867, SW7408-106: Properly register new modules in
 * BDBG_P_GetModuleByName
 *
 * Hydra_Software_Devel/55   6/30/10 10:40a vsilyaev
 * SW7420-867: Don't allocate memory while holding DBG_Lock
 *
 * Hydra_Software_Devel/54   6/25/10 6:26p jgarrett
 * SW7408-88: Removing implicit global from BDBG_OBJECT_DESTROY
 *
 * Hydra_Software_Devel/53   10/14/09 6:52p vsilyaev
 * SW7405-3211: Use constant pointers in  BDBG_Object_Assert
 *
 * Hydra_Software_Devel/52   9/22/09 5:08p erickson
 * SW3548-2493: allow compile-time and run-time debug levels to change
 * free from each other.
 *
 * Hydra_Software_Devel/51   7/21/09 12:36p nilesh
 * PR56629: Resolve variable namespace conflict with local scope variables
 * defined in BLST macros
 *
 * Hydra_Software_Devel/50   7/15/09 10:58a nilesh
 * PR56629: Fixed memory leak introduced when reusing debug instance
 *
 * Hydra_Software_Devel/49   7/13/09 11:31a nilesh
 * PR56629: Reuse unregistered debug instances to prevent memory leak
 *
 * Hydra_Software_Devel/48   7/1/09 4:55p vsilyaev
 * PR 53778: Always define bdbg_id__bdbg_invalid
 *
 * Hydra_Software_Devel/47   6/24/09 2:26p mphillip
 * PR54926: Merge DBG extension to /main
 *
 * Hydra_Software_Devel/PR54926/1   5/18/09 6:57p vsilyaev
 * PR 54926: Added interface to capture debug output
 *
 * Hydra_Software_Devel/46   4/10/09 2:51p katrep
 * PR53778: Compiler warning
 *
 * Hydra_Software_Devel/45   4/10/09 11:33a vsilyaev
 * PR 53778: Protect dereference symbols only defined in the debug build
 *
 * Hydra_Software_Devel/44   4/9/09 1:35p vsilyaev
 * PR 53778: Keep a history of free'ed objects for better debug facilities
 *
 * Hydra_Software_Devel/43   4/2/09 11:30a erickson
 * PR53778: integrate BKNI_TRACK_MALLOCS with BDBG_OBJECT_ASSERT for more
 * debug information
 *
 * Hydra_Software_Devel/42   2/19/09 6:45p vsilyaev
 * PR 52320: Added BDBG_RELEASE macro to unwind module from BDBG
 *
 * Hydra_Software_Devel/41   2/12/09 5:17p vsilyaev
 * PR 51891: Removed possible memory allocation in
 * BDBG_P_InstTestAndPrint
 *
 * Hydra_Software_Devel/40   2/3/09 12:42p shyam
 * PR51819 : Add timestamp prints to BDBG_ENTER and LEAVE functions
 *
 * Hydra_Software_Devel/39   1/26/09 10:33a erickson
 * PR51415: rework bdbg_os_priv interface to not suggest BKNI_EventHandle
 * usage in BDBG_P_Lock, which must lock in both task and isr contexts
 *
 * Hydra_Software_Devel/38   1/23/09 2:47p vsilyaev
 * PR 50746: Fixed warnings for 64-bit build
 *
 * Hydra_Software_Devel/37   12/29/08 1:32p erickson
 * PR50746: make BDBG_OBJECT_ASSERT failures more helpful
 *
 * Hydra_Software_Devel/36   8/6/08 10:02a erickson
 * PR45459: fix release build
 *
 * Hydra_Software_Devel/35   7/22/08 4:46p nilesh
 * PR44846: In BDBG_P_InstTestAndPrint(), use the dbg_module name to
 * lookup the internal module instead of using dbg_module directly
 *
 * Hydra_Software_Devel/34   7/22/08 2:03p nilesh
 * PR44846: Instance name is now checked if not null before comparison
 *
 * Hydra_Software_Devel/33   7/22/08 12:30p nilesh
 * PR44846: Added support for setting instance levels on a per module
 * basis
 *
 * Hydra_Software_Devel/32   11/1/07 2:14p vsilyaev
 * PR 36199: Fixed gate for instance level debug output
 *
 * Hydra_Software_Devel/31   10/22/07 2:57p vsilyaev
 * PR 36199: Fixed handling of instances and accounting of module debug
 * level
 *
 * Hydra_Software_Devel/30   10/11/07 1:28p vishk
 * PR 35967: Coverity Defect ID:3955 CHECKED_RETURN
 *
 * Hydra_Software_Devel/29   8/15/07 2:25p vsilyaev
 * PR 34089: Optimized executuion speed and code size
 *
 * Hydra_Software_Devel/28   5/10/07 11:57a vsilyaev
 * PR 25469: Fixed error for release build
 *
 * Hydra_Software_Devel/27   5/4/07 12:14p vsilyaev
 * PR 25469: Added function BDBG_EnumerateAll
 *
 * Hydra_Software_Devel/26   1/30/07 11:03a vsilyaev
 * PR 27425: Proofread documentation and expaned BDBG_OBJECT portion
 *
 * Hydra_Software_Devel/25   9/7/06 5:06p vsilyaev
 * PR 24154: Added simple wildcard support
 *
 * Hydra_Software_Devel/24   7/21/06 11:28a vsilyaev
 * PR 22695: Changes in the BDBG module to make it run-time compatible
 * between debug and release builds
 *
 * Hydra_Software_Devel/23   5/31/06 5:11p vsilyaev
 * PR 21756: Make a copy of the module name if new module descriptor is
 * allocated from the heap
 *
 * Hydra_Software_Devel/22   4/27/06 10:07a vle
 * PR 21065: rework BDBG_LOCK and BDBG_UNLOCK to be OS specific
 * implementation.
 *
 * Hydra_Software_Devel/21   2/28/06 10:50a vsilyaev
 * PR 19917: Fixed typo
 *
 * Hydra_Software_Devel/20   2/27/06 5:12p vsilyaev
 * PR 19917: Added API to check object types at runtime
 *
 * Hydra_Software_Devel/19   11/29/05 3:15p vle
 * PR18319: add timestamp to debug messages
 *
 * Hydra_Software_Devel/18   11/21/05 3:08p vsilyaev
 * PR 18217: Fixed handling of instance level debug messages
 *
 * Hydra_Software_Devel/17   6/28/05 9:03a erickson
 * PR16033: verify that BDBG_Init is calld
 *
 * Hydra_Software_Devel/16   11/13/03 1:53p vsilyaev
 * Added Trace level to use by BDBG_ENTER and BDBG_LEAVE.
 *
 * Hydra_Software_Devel/15   11/13/03 9:33a vsilyaev
 * Tag malloced and statically allocated memory, so only malloced will be
 * free'ed at UnInit time.
 *
 * Hydra_Software_Devel/14   11/12/03 2:23p vsilyaev
 * Use statically alllocated data for internal structures, it solves
 * dilemma of malloc and interrupt.
 *
 * Hydra_Software_Devel/14   11/12/03 2:14p vsilyaev
 * Use statically alllocated data for internal structures, it solves
 * dilemma of malloc and interrupt.
 *
 * Hydra_Software_Devel/13   10/2/03 4:00p erickson
 * removed warning and added explicit typecasts for malloc
 *
 * Hydra_Software_Devel/12   9/5/03 11:13a jasonh
 * Wrapped BSTD_UNUSED around unused arguments.
 *
 * Hydra_Software_Devel/11   4/8/03 6:04p vsilyaev
 * Removed timeout from the BDBG_AcquireMutex.
 *
 * Hydra_Software_Devel/10   4/1/03 7:32p vsilyaev
 * Fixed false trigraph '???' case.
 *
 * Hydra_Software_Devel/9   3/28/03 5:57p vsilyaev
 * Fixed format string for unknown instance.
 *
 * Hydra_Software_Devel/8   3/25/03 8:24p vsilyaev
 * Fixed potentional race conditions in the BDGB_GetModule function.
 *
 * Hydra_Software_Devel/7   3/20/03 1:02p erickson
 * removed unused local variables
 *
 * Hydra_Software_Devel/6   3/14/03 7:52p vsilyaev
 * Updated to work with new list implementation.
 *
 * Hydra_Software_Devel/5   3/12/03 4:39p vsilyaev
 * Integrated with new header file.
 *
 * Hydra_Software_Devel/4   3/11/03 9:54p vsilyaev
 * Update for the new kernel interface.
 *
 * Hydra_Software_Devel/3   3/10/03 6:36p vsilyaev
 * Fixed BDBG_P_PrintError.
 *
 * Hydra_Software_Devel/2   3/10/03 10:43a vsilyaev
 * Integration with berr.h
 *
 * Hydra_Software_Devel/1   3/10/03 10:03a vsilyaev
 * O/S inspecific part of the debug interface.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bdbg_os_priv.h"
#include "bdbg_log.h"
#include "blst_slist.h"
#include "bdbg_priv.h"

/* max number of instances per module */
#define MAX_MODULE_INSTANCES   32
#define BDBG_P_MAX_MODULE_NAME_LENGTH   64

#undef BDBG_SetLevel
#undef BDBG_GetLevel
#undef BDBG_SetModuleLevel
#undef BDBG_GetModuleLevel
#undef BDBG_SetInstanceLevel
#undef BDBG_GetInstanceLevel
#undef BDBG_SetInstanceName
#undef BDBG_EnumerateAll
#undef BDBG_GetModuleInstanceLevel
#undef BDBG_SetModuleInstanceLevel

struct BDBG_DebugInstModule {
      BLST_S_ENTRY(BDBG_DebugInstModule) link;

      BDBG_pDebugModuleFile pModule; /* pointer to module */
      BDBG_Level eLevel;
};

struct BDBG_DebugModuleInst {
      BLST_S_ENTRY(BDBG_DebugModuleInst) link;
      BDBG_Instance handle;
      BDBG_Level level;
      const char *name;
      BDBG_pDebugModuleFile module; /* pointer to module */

      BLST_S_HEAD(BDBG_DebugInstModuleHead, BDBG_DebugInstModule) modules;
};

typedef struct BDBG_P_LogEntry {
    unsigned long tag; /* tag is value of stack pointer and LSB  used to indicate header or body */
    int16_t rc; /* result from printf */
    char str[256-sizeof(uint32_t)-sizeof(void *)-sizeof(int16_t)];
} BDBG_P_LogEntry;


static struct {
    BLST_S_HEAD(BDBG_DebugModuleHead, BDBG_DebugModuleFile) modules;  /* sorted list of modules */
    BLST_S_HEAD(BDBG_DebugInstHead, BDBG_DebugModuleInst) instances;  /* sorted list of instances */
    BLST_S_HEAD(BDBG_DebugUnregInstHead, BDBG_DebugModuleInst) unregistered_instances;  /* list of unregistered instances */
    BDBG_Level level;
    BDBG_Fifo_Handle dbgLog;
} gDbgState = {
   BLST_S_INITIALIZER(BDBG_DebugModuleFile),
   BLST_S_INITIALIZER(BDBG_DebugModuleInst),
   BLST_S_INITIALIZER(BDBG_DebugModuleInst),
   BDBG_eWrn, /* default level is a warning level */
   NULL
};

static const char gDbgPrefix[][4] =
{
   "",
   "...",
   "---",
   "***",
   "###",
   "   "
};


static struct BDBG_DebugModuleInst * BDBG_P_GetInstance(BDBG_Instance handle);
static const char *BDBG_P_GetPrefix(BDBG_Level level);
static BERR_Code BDBG_P_RegisterModuleFile(BDBG_pDebugModuleFile dbg_module);
static bool BDBG_P_TestModule(BDBG_pDebugModuleFile dbg_module, BDBG_Level level);
static BDBG_pDebugModuleFile BDBG_P_GetModuleByName(const char *name);
static BDBG_pDebugModuleFile BDBG_P_GetModuleByName_sync(const char *name, BDBG_pDebugModuleFile module /* optional placeholder */ );
static int BDBG_P_StrCmp(const char *str1, const char *str2);
static BERR_Code BDBG_P_CheckDebugLevel(BDBG_Level level);

static struct BDBG_DebugInstModule * BDBG_P_GetInstanceModule(struct BDBG_DebugModuleInst *pInstance, BDBG_pDebugModuleFile pModule);
static struct BDBG_DebugInstModule * BDBG_P_GetInstanceModule_sync(struct BDBG_DebugModuleInst *pInstance, BDBG_pDebugModuleFile pModule);
static BERR_Code BDBG_P_SetInstanceModuleLevel(struct BDBG_DebugModuleInst *pInstance, BDBG_pDebugModuleFile pModule, BDBG_Level eLevel);
static struct BDBG_DebugModuleInst * BDBG_P_GetInstanceByName(const char *name);
static struct BDBG_DebugModuleInst * BDBG_P_GetInstanceByName_sync(const char *name);
static void BDBG_P_Dequeue_FreeMemory(void);

static const char *
BDBG_P_GetPrefix(BDBG_Level level)
{
   BDBG_CASSERT(sizeof(gDbgPrefix)/sizeof(*gDbgPrefix) == BDBG_P_eLastEntry);
   if (level<BDBG_P_eLastEntry) {
      return gDbgPrefix[level];
   } else {
      return gDbgPrefix[0];
   }
}

static int
BDBG_P_StrCmp(const char *str1, const char *str2)
{
   int ch1, ch2, diff;

   for(;;) {
      ch1 = *str1++;
      ch2 = *str2++;
      if (ch1=='*' || ch2=='*') {
         return 0;
      }
      diff = ch1 - ch2;
      if (diff) {
         return diff;
      } else if (ch1=='\0') {
         return 0;
      }
   }
}

char * BDBG_P_StrnCpy(char *dest, const char *src, size_t num)
{
   char *p = dest;

   /* Always set the last character to NULL in the destination */
   num--;
   dest[num] = '\0';

   while (num-- && (*dest++ = *src++));

   return p;
}

char * BDBG_P_StrChrNul(const char *str, int c)
{
   char *p = (char *) str;
   while ((*p != '\0') && (*p != c)) p++;

   return p;
}

static BERR_Code
BDBG_P_CheckDebugLevel(BDBG_Level level)
{
    if(level>=BDBG_P_eLastEntry) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    return BERR_SUCCESS;
}

static BERR_Code
BDBG_P_RegisterModuleFile(BDBG_pDebugModuleFile dbg_module)
{
   BDBG_pDebugModuleFile module;
   BERR_Code rc = BERR_SUCCESS;
   BDBG_pDebugModuleFile prev;

   BDBG_P_Lock();
   /* test if module has been already registered */
   if (dbg_module->level!=BDBG_P_eUnknown) {
      goto done; /* module was already registered */
   }

   module = BDBG_P_GetModuleByName_sync(dbg_module->name, dbg_module);
   if (!module) {
      rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
      goto done;
   }
   /* check whether new file exists in the module chain */
   for(prev=module; module!=NULL && dbg_module!=module ; module = BLST_S_NEXT(module, link)) {
      if(BDBG_P_StrCmp(dbg_module->name, module->name)!=0) {
         break;
      }
   }
   if(dbg_module!=module) {
      /* file not in the chain, add it */
      /* 1. copy level information */
      dbg_module->level = prev->level;
      dbg_module->module_level = prev->module_level;
      dbg_module->module_print = prev->module_print;
      /* 2. add into the sorted list */
      BLST_S_INSERT_AFTER(&gDbgState.modules, prev, dbg_module, link);
   }
  done:
   BDBG_P_Unlock();
   return rc;
}

static struct BDBG_DebugModuleInst *
BDBG_P_GetInstance_sync(BDBG_Instance handle)
{
   struct BDBG_DebugModuleInst *instance;

   for(instance = BLST_S_FIRST(&gDbgState.instances); instance ; instance = BLST_S_NEXT(instance, link)) {
      if ((uint8_t *)instance->handle >= (uint8_t *)handle) {
         break;
      }
   }
   if(instance && instance->handle!=handle) {
      instance = NULL;
   }
   return instance;
}

static struct BDBG_DebugModuleInst *
BDBG_P_GetInstance(BDBG_Instance handle)
{
   struct BDBG_DebugModuleInst *instance;

   BDBG_P_Lock();
   instance = BDBG_P_GetInstance_sync(handle);
   BDBG_P_Unlock();
   return instance;
}

static bool
BDBG_P_TestModule(BDBG_pDebugModuleFile dbg_module, BDBG_Level level)
{
   BERR_Code rc;

   /*
    * try do a trick don't acquire mutex on first test, and use double test techinques.
    * Note: this technique would have a race condition on the SMP systems
    */
   if (dbg_module->level==BDBG_P_eUnknown) {
      /* register module file in the system */
      rc = BDBG_P_RegisterModuleFile(dbg_module);
      if (rc!=BERR_SUCCESS) {
         return false;
      }
   }
   return (int)level >= dbg_module->level;
}

static size_t
BDBG_P_strlen(const char *str)
{
   size_t len;
   for(len=0;*str++!='\0';len++) { }
   return len;
}

/* should be called with mutex already held */
static BDBG_pDebugModuleFile
BDBG_P_GetModuleByName_sync(const char *name, BDBG_pDebugModuleFile new_module)
{
   BDBG_pDebugModuleFile module, prev;
   int cmp=-1;

   BDBG_CASSERT(sizeof(char)==1); /* prerequesite for the block allocation */

   /* traverse all known modules */
   for(prev=NULL, module = BLST_S_FIRST(&gDbgState.modules); module ; module = BLST_S_NEXT(module, link)) {
      cmp = BDBG_P_StrCmp(name, module->name);
      if(cmp>=0) {
         break;
      }
      prev = module;
   }
   if(cmp==0) { /* exact match */
      goto done;
   }
   /* else insert new module */
   module = new_module;
   if(module==NULL) { goto done; }
   module->module_alloc = false;
   module->module_print = NULL;
   module->level = gDbgState.level;
   module->module_level = gDbgState.level;
   module->name = name;
   if(prev) {
      BLST_S_INSERT_AFTER(&gDbgState.modules, prev, module, link);
   } else {
      BLST_S_INSERT_HEAD(&gDbgState.modules, module, link);
   }

  done:
   return module;
}


static BDBG_pDebugModuleFile
BDBG_P_GetModuleByName(const char *name)
{
    BDBG_pDebugModuleFile old_module;
    BDBG_pDebugModuleFile new_module;
    size_t name_len;
    char *copy_name;

    /* we can't allocate memory inside BDBG_P_GetModuleByName_sync, so try to find existing module first */
    BDBG_P_Lock();
    old_module = BDBG_P_GetModuleByName_sync(name, NULL);
    BDBG_P_Unlock();
    if(old_module) {goto done;}
    /* and if it fails allocate memory, copy string and prepapre to free allocated memory if returned something else */

    name_len=BDBG_P_strlen(name)+1; /* string length plus trailing 0 */
    /* this code should never executed at the interrupt time */
    new_module = BKNI_Malloc(sizeof(*new_module)+name_len);
    if (!new_module) {
        return NULL;
    }
    copy_name = (char *)new_module + sizeof(*new_module);
    BKNI_Memcpy(copy_name, name, name_len); /* copy name */

    BDBG_P_Lock();
    old_module = BDBG_P_GetModuleByName_sync(copy_name, new_module);
    if(old_module==new_module) {
        new_module->module_alloc = true;
    }
    BDBG_P_Unlock();

    if(old_module!=new_module) {
        BKNI_Free(new_module);
    }
done:
    return old_module;
}

static struct BDBG_DebugModuleInst *
BDBG_P_GetInstanceByName_sync(const char *name)
{
   struct BDBG_DebugModuleInst *pInstance;

   /* traverse all known instances */
   for(pInstance = BLST_S_FIRST(&gDbgState.instances); pInstance ; pInstance = BLST_S_NEXT(pInstance, link))
   {
      if (pInstance->name)
      {
         if (BDBG_P_StrCmp(name, pInstance->name) == 0)
         {
            return pInstance;
         }
      }
   }

   return NULL;
}

static struct BDBG_DebugModuleInst *
BDBG_P_GetInstanceByName(const char *name)
{
   struct BDBG_DebugModuleInst * pInstance;

   BDBG_P_Lock();

   pInstance = BDBG_P_GetInstanceByName_sync(name);

   BDBG_P_Unlock();
   return pInstance;
}

#define BDBG_P_Vprintf_module(kind, instance, _level, dbg_module, fmt, ap) do { \
    BDBG_DebugModule_Print module_print = dbg_module->module_print; \
    bool normal_print = instance || (module_print==NULL ? ((int)_level >= dbg_module->level) : ((int)_level >= -dbg_module->level)); \
    if(module_print) { module_print(kind, _level, dbg_module, fmt, ap); } \
    if(normal_print) { BDBG_P_Vprintf_Log(kind, fmt, ap); } \
} while(0)

int
BDBG_P_Vprintf_Log(BDBG_ModulePrintKind kind, const char *fmt, va_list ap)
{
    int rc = 0;
    BDBG_Fifo_Handle dbgLog = gDbgState.dbgLog;
    if(dbgLog) {
        BDBG_Fifo_Token token;
        BDBG_P_LogEntry *log = BDBG_Fifo_GetBuffer(dbgLog, &token);
        if(log) {
            log->tag = (unsigned long)&token | kind;
            rc = BKNI_Vsnprintf(log->str, sizeof(log->str), fmt, ap);
            log->rc = rc;
            BDBG_Fifo_CommitBuffer(&token);
        }
    } else {
	    rc = BKNI_Vprintf(fmt, ap);
        if(kind==BDBG_ModulePrintKind_eHeaderAndBody  || kind==BDBG_ModulePrintKind_eBody ) {
            BKNI_Printf("\n");
        }
    }
    return 0;
}

static void
BDBG_P_PrintHeader(bool instance, BDBG_Level level, BDBG_pDebugModuleFile dbg_module, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    BDBG_P_Vprintf_module(BDBG_ModulePrintKind_eHeader, instance, level, dbg_module, fmt, ap);
    va_end( ap );
    return;
}


static void
BDBG_P_VprintBody(bool instance, BDBG_Level level, BDBG_pDebugModuleFile dbg_module, const char *fmt, va_list ap)
{
    BDBG_P_Vprintf_module(BDBG_ModulePrintKind_eBody, instance, level, dbg_module, fmt, ap);
    return;
}

static void
BDBG_P_PrintTrace(BDBG_pDebugModuleFile dbg_module, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    BDBG_P_Vprintf_module(BDBG_ModulePrintKind_eHeaderAndBody, false, BDBG_eTrace, dbg_module, fmt, ap);
    va_end( ap );
    return;
}


void /* only used externally if BDBG_P_UNWRAP is undefined */
BDBG_P_PrintWithNewLine(const char *fmt, ...)
{
   va_list ap;

   va_start(ap, fmt);
   BDBG_P_Vprintf_Log(BDBG_ModulePrintKind_eBody, fmt, ap);
   va_end( ap );
   return;
}


bool
BDBG_P_TestAndPrint(BDBG_Level level, BDBG_pDebugModuleFile dbg_module, const char *fmt, ...)
{
   if( BDBG_P_TestModule(dbg_module, level)) {
      char timeStamp[20];
      
      BDBG_P_GetTimeStamp(timeStamp, sizeof(timeStamp));
      BDBG_P_PrintHeader(false, level, dbg_module, "%s %s %s: ", BDBG_P_GetPrefix(level), timeStamp, dbg_module->name);
      if(fmt) { /* also print body */
          va_list ap;

          va_start(ap, fmt);
          BDBG_P_VprintBody(false, level, dbg_module, fmt, ap);
          va_end( ap );
      }
      return true;
   }
   return false;
}

bool
BDBG_P_InstTestAndPrint(BDBG_Level level, BDBG_pDebugModuleFile dbg_module, BDBG_Instance handle, const char *fmt, ...)
{
   struct BDBG_DebugInstModule *pInstanceModule = NULL;
   struct BDBG_DebugModuleInst *instance;
   bool module_result = BDBG_P_TestModule(dbg_module, level);
   bool instance_result;

   BDBG_P_Lock();
   instance = BDBG_P_GetInstance_sync(handle);

   if (instance) {
      BDBG_pDebugModuleFile pModule = BDBG_P_GetModuleByName_sync(dbg_module->name, dbg_module);
      if(pModule) {
        pInstanceModule = BDBG_P_GetInstanceModule_sync(instance, pModule);
      }
   }
   BDBG_P_Unlock();

   instance_result = (instance && level >= instance->level) || (pInstanceModule && level >= pInstanceModule->eLevel);
   if(module_result || instance_result) {
      char timeStamp[20];

      BDBG_P_GetTimeStamp(timeStamp, sizeof(timeStamp));
      if(instance && instance->name) {
         BDBG_P_PrintHeader(instance_result, level, dbg_module, "%s %s %s(%s): ", BDBG_P_GetPrefix(level), timeStamp, dbg_module->name, instance->name);
      } else {
         BDBG_P_PrintHeader(instance_result, level, dbg_module, "%s %s %s(%#lx): ", BDBG_P_GetPrefix(level), timeStamp, dbg_module->name, (unsigned long)handle);
      }
      if(fmt) { /* also print body */
          va_list ap;

          va_start(ap, fmt);
          BDBG_P_VprintBody(instance_result, level, dbg_module, fmt, ap);
          va_end( ap );
      }
      return true;
   }
   return false;
}

static BDBG_pDebugModuleFile
BDBG_P_GetModule(BDBG_pDebugModuleFile module)
{
   /* This return value is not checked intentionally */
   /* coverity[check_return] */
   /* coverity[unchecked_value] */
   BDBG_P_TestModule(module, BDBG_eTrace);
   return module;
}

void
BDBG_P_RegisterInstance(BDBG_Instance handle, BDBG_pDebugModuleFile dbg_module)
{
   BDBG_pDebugModuleFile module;
   struct BDBG_DebugModuleInst *instance = NULL;
   struct BDBG_DebugModuleInst *previous, *current;
   struct BDBG_DebugInstModule *pInstanceModule = NULL;

   module = BDBG_P_GetModule(dbg_module);
   if (!module) {
      return;
   }

   /* PR56629: Search for and re-use the previously
    * unregistered instance to prevent memory leak */
   BDBG_P_Lock();
   for(current = BLST_S_FIRST(&gDbgState.unregistered_instances); current ; current = BLST_S_NEXT(current, link))
   {
      if ( ( current->module == module )
           && (current->handle == handle ) )
      {
         /* We found a previously unregistered instance */
         BLST_S_REMOVE(&gDbgState.unregistered_instances, current, BDBG_DebugModuleInst, link);
         instance = current;

         /* Remove and free each module instance */
         while( (pInstanceModule = BLST_S_FIRST(&instance->modules)) != NULL ) {
            BLST_S_REMOVE_HEAD(&instance->modules,link);
            BDBG_P_Unlock();
            BKNI_Free(pInstanceModule);
            BDBG_P_Lock();
         }

         break;
      }
   }

   if ( NULL == instance )
   {
      BDBG_P_Unlock();
      instance = BKNI_Malloc(sizeof(*instance));
      if(!instance) {
         /* too bad */
         return;
      }
      BDBG_P_Lock();
   }

   instance->module = module;
   instance->name = NULL;
   instance->level = module->module_level;
   instance->handle = handle;
   BLST_S_INIT(&instance->modules);

   /* 1. Find spot in the sorted list */
   for(previous=NULL, current = BLST_S_FIRST(&gDbgState.instances); current ; current = BLST_S_NEXT(current, link)) {
      if ((uint8_t *)current->handle >= (uint8_t *)handle) {
         break;
      }
      previous=current;
   }
   if(previous) {
      BLST_S_INSERT_AFTER(&gDbgState.instances, previous, instance, link);
   } else {
      BLST_S_INSERT_HEAD(&gDbgState.instances, instance, link);
   }
   BDBG_P_Unlock();

   return;
}

void
BDBG_P_UnRegisterInstance(BDBG_Instance handle, BDBG_pDebugModuleFile dbg_module)
{
   struct BDBG_DebugModuleInst *instance;

   BSTD_UNUSED(dbg_module);

   instance = BDBG_P_GetInstance(handle);
   if(instance) {
      BDBG_P_Lock();
      BLST_S_REMOVE(&gDbgState.instances, instance, BDBG_DebugModuleInst, link);

      /* Add instance to unregistered instance list to be freed later
       * during BDBG_Uninit() */
      BLST_S_INSERT_HEAD(&gDbgState.unregistered_instances, instance, link);
      BDBG_P_Unlock();
   }
   return;
}

static void
BDBG_P_EnterLeaveFunction(BDBG_pDebugModuleFile dbg_module, const char *function, const char *kind)
{
   if(BDBG_P_TestModule(dbg_module, BDBG_eTrace)) {
      char timeStamp[20];
      BDBG_P_GetTimeStamp(timeStamp, sizeof(timeStamp));

      BDBG_P_PrintTrace(dbg_module, "%s %s %s: %s", gDbgPrefix[BDBG_eTrace], timeStamp, kind, function);
   }
}

void
BDBG_EnterFunction(BDBG_pDebugModuleFile dbg_module, const char *function)
{
   BDBG_P_EnterLeaveFunction(dbg_module, function, "Enter");
   return;
}

void
BDBG_LeaveFunction(BDBG_pDebugModuleFile dbg_module, const char *function)
{
   BDBG_P_EnterLeaveFunction(dbg_module, function, "Leave");
   return;
}


BERR_Code
BDBG_SetLevel(BDBG_Level level)
{
   BERR_Code rc = BDBG_P_CheckDebugLevel(level);
   BDBG_pDebugModuleFile module;

   if (rc!=BERR_SUCCESS) {
      return BERR_TRACE(rc);
   }

   gDbgState.level = level;
   /* traverse all known modules */
   BDBG_P_Lock();
   for(module = BLST_S_FIRST(&gDbgState.modules); module ; module = BLST_S_NEXT(module, link)) {
      module->level = (level < module->module_level)?level:module->module_level; /* update current module level with smallest of global level and module level */
   }
   BDBG_P_Unlock();
   return BERR_SUCCESS;
}

BERR_Code
BDBG_GetLevel(BDBG_Level *level)
{
   *level = gDbgState.level;
   return BERR_SUCCESS;
}



BERR_Code
BDBG_SetInstanceLevel(BDBG_Instance handle, BDBG_Level level)
{
   struct BDBG_DebugModuleInst *instance;

   BERR_Code rc = BDBG_P_CheckDebugLevel(level);

   if (rc!=BERR_SUCCESS) {
      return BERR_TRACE(rc);
   }

   instance = BDBG_P_GetInstance(handle);
   if (!instance) {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }
   instance->level = level;

   return BERR_SUCCESS;
}

BERR_Code
BDBG_GetInstanceLevel(BDBG_Instance handle, BDBG_Level *level)
{
   struct BDBG_DebugModuleInst *instance;

   instance = BDBG_P_GetInstance(handle);
   if (!instance) {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }
   *level = instance->level;

   return BERR_SUCCESS;
}

BERR_Code
BDBG_SetInstanceName(BDBG_Instance handle, const char *name)
{
   struct BDBG_DebugModuleInst *instance;

   instance = BDBG_P_GetInstance(handle);
   if (!instance) {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }
   instance->name = name;

   return BERR_SUCCESS;
}

static struct BDBG_DebugInstModule *
BDBG_P_GetInstanceModule_sync(struct BDBG_DebugModuleInst *pInstance, BDBG_pDebugModuleFile pModule)
{
   struct BDBG_DebugInstModule * pInstanceModule;
   for(pInstanceModule = BLST_S_FIRST(&pInstance->modules); pInstanceModule ; pInstanceModule = BLST_S_NEXT(pInstanceModule, link)) {
      if (pInstanceModule->pModule >= pModule) break;
   }
   if (pInstanceModule && (pInstanceModule->pModule != pModule)) pInstanceModule = NULL;
   return pInstanceModule;
}

static struct BDBG_DebugInstModule *
BDBG_P_GetInstanceModule(struct BDBG_DebugModuleInst *pInstance, BDBG_pDebugModuleFile pModule)
{
   struct BDBG_DebugInstModule * pInstanceModule;

   BDBG_P_Lock();
   pInstanceModule = BDBG_P_GetInstanceModule_sync(pInstance, pModule);
   BDBG_P_Unlock();
   return pInstanceModule;
}

BERR_Code
BDBG_P_SetInstanceModuleLevel(struct BDBG_DebugModuleInst *pInstance, BDBG_pDebugModuleFile pModule, BDBG_Level eLevel)
{
   struct BDBG_DebugInstModule *pInstanceModule, *pCurInstanceModule, *pPrevInstanceModule;

   pInstanceModule = BDBG_P_GetInstanceModule(pInstance, pModule);

   if (pInstanceModule == NULL)
   {
      /* Add instance to instance module list */
      pInstanceModule = BKNI_Malloc(sizeof(struct BDBG_DebugInstModule));
      if (pInstanceModule)
      {
         pInstanceModule->pModule = pModule;

         BDBG_P_Lock();

         /* 1. Find spot in the sorted list */
         for(pPrevInstanceModule=NULL, pCurInstanceModule = BLST_S_FIRST(&pInstance->modules); pCurInstanceModule ; pCurInstanceModule = BLST_S_NEXT(pCurInstanceModule, link)) {
            if ((uint8_t *)pCurInstanceModule->pModule >= (uint8_t *)pModule) {
               break;
            }
            pPrevInstanceModule=pCurInstanceModule;
         }
         if(pPrevInstanceModule) {
            BLST_S_INSERT_AFTER(&pInstance->modules, pPrevInstanceModule, pInstanceModule, link);
         } else {
            BLST_S_INSERT_HEAD(&pInstance->modules, pInstanceModule, link);
         }

         BDBG_P_Unlock();
      }
   }

   if (pInstanceModule != NULL)
   {
      pInstanceModule->eLevel = eLevel;
      return BERR_SUCCESS;
   }
   else
   {
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
   }
}

BERR_Code
BDBG_GetModuleInstanceLevel(const char *name, BDBG_Instance handle, BDBG_Level *level)
{
   struct BDBG_DebugModuleInst *pInstance;
   BDBG_pDebugModuleFile pModule;
   struct BDBG_DebugInstModule *pInstanceModule;

   /* Find the instance struct using the handle */
   pInstance = BDBG_P_GetInstance(handle);
   if (!pInstance)
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Find the module struct using the name */
   pModule = BDBG_P_GetModuleByName(name);
   if (!pModule) {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Fine the instance module struct using the instance and module */
   pInstanceModule = BDBG_P_GetInstanceModule(pInstance, pModule);
   if (!pInstanceModule)
   {
      /* An instance module level wasn't set, so we return the
       * instance's level instead */
      *level = pInstance->level;
   }
   else
   {
      *level = pInstanceModule->eLevel;
   }

   return BERR_SUCCESS;
}

BERR_Code
BDBG_SetModuleInstanceLevel(const char *name, BDBG_Instance handle, BDBG_Level level)
{
   struct BDBG_DebugModuleInst *pInstance;
   BDBG_pDebugModuleFile pModule;

   /* Find the instance struct using the handle */
   pInstance = BDBG_P_GetInstance(handle);
   if (!pInstance)
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Find the module struct using the name */
   pModule = BDBG_P_GetModuleByName(name);
   if (!pModule) {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Set instance module level using the instance and module */
   return BDBG_P_SetInstanceModuleLevel(pInstance, pModule, level);
}

BERR_Code
BDBG_SetModuleLevel(const char *name, BDBG_Level level)
{
   BDBG_pDebugModuleFile module;
   struct BDBG_DebugModuleInst *pInstance;
   char module_name[BDBG_P_MAX_MODULE_NAME_LENGTH];
   char *instance_name = NULL;
   BERR_Code rc = BDBG_P_CheckDebugLevel(level);

   if (rc!=BERR_SUCCESS) {
      return BERR_TRACE(rc);
   }

   BDBG_P_StrnCpy(module_name, name, BDBG_P_MAX_MODULE_NAME_LENGTH);
   instance_name = BDBG_P_StrChrNul(module_name, ':');
   if (*instance_name != '\0')
   {
      *instance_name = '\0';
      instance_name++;
   }

   pInstance = BDBG_P_GetInstanceByName(instance_name);

   if (pInstance)
   {
      module = BDBG_P_GetModuleByName(module_name);
      if (!module) {
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      BDBG_P_SetInstanceModuleLevel(pInstance, module, level);
   }
   else
   {
      module = BDBG_P_GetModuleByName(name);
      if (!module) {
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      /* We set the module level */
      /* traverse all files of given module */
      BDBG_P_Lock();
      for(; module ; module = BLST_S_NEXT(module, link)) {
         if(BDBG_P_StrCmp(name, module->name)!=0) {
            break;
         }
         module->module_level = level;
         module->level = (gDbgState.level < level)?gDbgState.level:level; /* update current module level with smallest  of global level and module level */
      }
      BDBG_P_Unlock();
   }

   return BERR_SUCCESS;
}

BERR_Code
BDBG_GetModuleLevel(const char *name, BDBG_Level *level)
{
   BDBG_pDebugModuleFile module;
   struct BDBG_DebugModuleInst *pInstance;
   struct BDBG_DebugInstModule *pInstanceModule = NULL;
   char module_name[BDBG_P_MAX_MODULE_NAME_LENGTH];
   char *instance_name = NULL;

   BDBG_P_StrnCpy(module_name, name, BDBG_P_MAX_MODULE_NAME_LENGTH);
   instance_name = BDBG_P_StrChrNul(module_name, ':');
   if (*instance_name != '\0')
   {
      *instance_name = '\0';
      instance_name++;
   }

   pInstance = BDBG_P_GetInstanceByName(instance_name);

   if (pInstance)
   {
      module = BDBG_P_GetModuleByName(module_name);
      if (!module) {
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      /* Fine the instance module struct using the instance and module */
      pInstanceModule = BDBG_P_GetInstanceModule(pInstance, module);
      if (!pInstanceModule)
      {
         /* An instance module level wasn't set, so we return the
          * instance's level instead */
         *level = pInstance->level;
      }
      else
      {
         *level = pInstanceModule->eLevel;
      }
   }
   else
   {
      module = BDBG_P_GetModuleByName(name);
      if (!module) {
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
      *level = module->level;
   }
   return BERR_SUCCESS;
}

BERR_Code
BDBG_Init(void)
{
   BDBG_P_InitializeTimeStamp();
   return BDBG_P_OsInit();
}

void
BDBG_Uninit(void)
{
   BDBG_pDebugModuleFile module ;
   struct BDBG_DebugModuleInst *instance;
   struct BDBG_DebugInstModule *pInstanceModule = NULL;

   while( (module = BLST_S_FIRST(&gDbgState.modules)) != NULL ) {
      BLST_S_REMOVE_HEAD(&gDbgState.modules,link);
      if (module->module_alloc) {
         BKNI_Free(module);
      }
   }
   while( (instance = BLST_S_FIRST(&gDbgState.instances)) != NULL ) {
      BLST_S_REMOVE_HEAD(&gDbgState.instances,link);

      /* Remove and free each module instance */
      while( (pInstanceModule = BLST_S_FIRST(&instance->modules)) != NULL ) {
         BLST_S_REMOVE_HEAD(&instance->modules,link);
         BKNI_Free(pInstanceModule);
      }

      BKNI_Free(instance);
   }

   while( (instance = BLST_S_FIRST(&gDbgState.unregistered_instances)) != NULL ) {
      BLST_S_REMOVE_HEAD(&gDbgState.unregistered_instances,link);

      /* Remove and free each module instance */
      while( (pInstanceModule = BLST_S_FIRST(&instance->modules)) != NULL ) {
         BLST_S_REMOVE_HEAD(&instance->modules,link);
         BKNI_Free(pInstanceModule);
      }

      BKNI_Free(instance);
   }
   BDBG_P_Dequeue_FreeMemory();

   BDBG_P_OsUninit();

   return;
}

/* BDBG_OBJECT_ID(bdbg_invalid); */
static const char bdbg_id__bdbg_invalid[]="invalid";


void
BDBG_Object_Init(void *ptr, size_t size, struct bdbg_obj *obj, const char *id)
{
   unsigned i;

   if ( NULL == id )
   {
       id = bdbg_id__bdbg_invalid;
   }

   for(i=0; i+3 < size; i+=4) {
      *(uint32_t*)(((uint8_t *)ptr)+i)=0xDEADBEEF;
   }
   obj->bdbg_obj_id=id;
   return;
}

void
BDBG_Object_Assert(const void *ptr, size_t size, const struct bdbg_obj *obj, const char *id, const char *file, unsigned line) {
    static char message[128]; /* if used, this will be terminal. so use a static array to avoid stack blowout. */

    BSTD_UNUSED(size);

    if (ptr && obj->bdbg_obj_id==id) {
        return;
    }

#if BKNI_TRACK_MALLOCS
    if(ptr) {
        BKNI_MallocEntryInfo entry;

        if(BKNI_GetMallocEntryInfo(ptr, &entry)==BERR_SUCCESS) {
            message[0] = '\0';
            if(!entry.alive) {
                BKNI_Snprintf(message, sizeof(message), "and freed at %s:%u", entry.free_file, entry.free_line);
            }
            BDBG_P_PrintString("BDBG_OBJECT_ASSERT on object %#lx (%u:%u bytes) was allocated at %s:%u %s\n", (unsigned long)ptr, entry.size, size, entry.malloc_file, entry.malloc_line, message);
        }
    }
#endif
    message[0] = '\0';
    if(ptr==NULL) {
        BKNI_Snprintf(message, sizeof(message), "NULL pointer was used as %s", id);
    } else {
        if (obj->bdbg_obj_id == bdbg_id__bdbg_invalid) {
            BKNI_Snprintf(message, sizeof(message), "Recycled pointer was used %s:%#lx", id, (unsigned long)ptr);
        } else {
            /* This can be caused by a closed or otherwise invalid handle */
            BKNI_Snprintf(message, sizeof(message), "Bad object of expected type %s:%#lx (%#lx:%#lx)", id, (unsigned long)ptr, (unsigned long)obj->bdbg_obj_id, (unsigned long)id);
        }
    }

    BDBG_P_AssertFailed(message, file, line);
    return;
}

void
BDBG_EnumerateAll(void (*callback)(void *cntx, const char *module, BDBG_Instance instance, const char *inst_name), void *cntx)
{
   struct BDBG_DebugModuleInst *instance=NULL;
   BDBG_pDebugModuleFile module;
   const char *last_name;

   BDBG_P_Lock();
   for(last_name=NULL, module = BLST_S_FIRST(&gDbgState.modules); module ; module = BLST_S_NEXT(module, link)) {
      if(last_name==NULL || BDBG_P_StrCmp(last_name, module->name)!=0) {
         last_name = module->name;
         callback(cntx, module->name, NULL, NULL);
      }
   }
   for(instance = BLST_S_FIRST(&gDbgState.instances); instance ; instance = BLST_S_NEXT(instance, link)) {
      callback(cntx, instance->module->name, instance->handle, instance->name);
   }
   BDBG_P_Unlock();
   return ;
}

void
BDBG_P_Release(BDBG_pDebugModuleFile dbg_module)
{
   BDBG_P_Lock();
   if(dbg_module->level!=BDBG_P_eUnknown) {
        BLST_S_REMOVE(&gDbgState.modules, dbg_module, BDBG_DebugModuleFile, link);
        dbg_module->level = BDBG_P_eUnknown;
   }
   BDBG_P_Unlock();
   return;
}


BERR_Code
BDBG_SetModulePrintFunction(const char *name, BDBG_DebugModule_Print module_print)
{
#if defined(BDBG_P_UNWRAP)
    BDBG_pDebugModuleFile module;

    module = BDBG_P_GetModuleByName(name);
    if (!module) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* We set the module level */
    /* traverse all files of given module */
    BDBG_P_Lock();
    for(; module ; module = BLST_S_NEXT(module, link)) {
        if(BDBG_P_StrCmp(name, module->name)!=0) {
            break;
        }
        module->level = (gDbgState.level < module->module_level)?gDbgState.level:module->module_level; /* update current module level with smallest  of global level and module level */
        module->module_print = module_print;
        if(module_print) {
            module->level = -module->level; /* make it negative so it'd always pass test */
        }
    }
    BDBG_P_Unlock();

    return BERR_SUCCESS;
#else
    BSTD_UNUSED(name);
    BSTD_UNUSED(module_print);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

typedef struct BDBG_P_Dequeue_Context {
    bool used;
    uint16_t hdr_len;
    uint32_t tag;
    char *header_buf;
    size_t header_buf_size;
} BDBG_P_Dequeue_Context;
#define BDBG_P_DEQUEUE_MAX_CONTEXTS 16
static struct {
    unsigned last_used;
    unsigned nodata_count;
    BDBG_P_Dequeue_Context contexts[BDBG_P_DEQUEUE_MAX_CONTEXTS];
} BDBG_P_Dequeue_State;

#define BDBG_P_CONTEXT_THRESHOLD    256

static BDBG_P_Dequeue_Context *BDBG_P_Dequeue_FindContext(unsigned long tag)
{
    unsigned i;
    for(i=0;i<BDBG_P_Dequeue_State.last_used;i++) {
        BDBG_P_Dequeue_Context *context = BDBG_P_Dequeue_State.contexts+i;
        if(context->used) {
            long diff = tag - context->tag;
            if( -BDBG_P_CONTEXT_THRESHOLD < diff && diff < BDBG_P_CONTEXT_THRESHOLD) {
                return context;
            }
        }
    }
    return NULL;
}

static BDBG_P_Dequeue_Context *BDBG_P_Dequeue_FindFree(size_t hdr_len)
{
    unsigned i;
    BDBG_P_Dequeue_Context *context=NULL;

    for(i=0;i<BDBG_P_Dequeue_State.last_used;i++) {
        BDBG_P_Dequeue_Context *cur_context = BDBG_P_Dequeue_State.contexts+i;
        if(!cur_context->used) {
            context = cur_context;
            break;
        }
    }
    if(context == NULL) {
        if(BDBG_P_Dequeue_State.last_used >= BDBG_P_DEQUEUE_MAX_CONTEXTS) {
            return NULL;
        }
        context = BDBG_P_Dequeue_State.contexts+BDBG_P_Dequeue_State.last_used;
        BDBG_P_Dequeue_State.last_used++;
        context->used = false;
        context->tag = 0;
        context->header_buf = NULL;
        context->header_buf_size = 0;
    }
    if(context->header_buf_size < hdr_len)  {
        if(context->header_buf) {
            context->header_buf_size = 0;
            BKNI_Free(context->header_buf);
        }
        context->header_buf = BKNI_Malloc(hdr_len);
        if(context->header_buf==NULL) {
            return NULL;
        }
        context->header_buf_size = hdr_len;
    }
    return context;
}

static void BDBG_P_Dequeue_FreeMemory(void)
{
    unsigned i;
    for(i=0;i<BDBG_P_Dequeue_State.last_used;i++) {
        BDBG_P_Dequeue_Context *context = BDBG_P_Dequeue_State.contexts+i;
        if(context->header_buf) {
            BKNI_Free(context->header_buf);
            context->used = false;
            context->header_buf = NULL;
            context->header_buf_size = 0;
        }
    }
    BDBG_P_Dequeue_State.nodata_count = 0;
    BDBG_P_Dequeue_State.last_used = 0;
    return;
}

typedef struct BDBG_P_StrBuf {
    char *str;
    size_t size;
    unsigned len;
}BDBG_P_StrBuf;


static void BDBG_P_StrBuf_Printf( BDBG_P_StrBuf *buf, const char *fmt, ...)
{
    int rc;
    size_t size = buf->size - buf->len;
    va_list ap;
    va_start(ap, fmt);
    rc = BKNI_Vsnprintf(buf->str, size, fmt, ap);
    va_end( ap );
    if(rc>0) {
        buf->len = (unsigned)rc<size  ? buf->len + rc : buf->size - 1;
    }
    return;
}

#if 0
static void BDBG_P_StrBuf_PrintChar( BDBG_P_StrBuf *buf, char c)
{
    if(buf->len + 1 > buf->size) {
        buf->str[buf->len]=c;
        buf->len++;
        buf->str[buf->len]='\0';
    }
    return;
}
#endif

static void BDBG_P_StrBuf_Memcpy( BDBG_P_StrBuf *buf, const void *b, size_t len)
{
    if(buf->len + len + 1 < buf->size) {
        /* buffer large enough, do nothing, keep going */
    } else if(buf->len + 1 < buf->size) {
        /* there is a space for at least one character */
        len = buf->size  - (buf->len + 1);
    } else {
        /* there is no space */
        return;
    }
    BKNI_Memcpy(buf->str+buf->len, b, len);
    buf->len += len;
    buf->str[buf->len]='\0';
    return;
}

static void BDBG_P_StrBuf_AddLog( BDBG_P_StrBuf *buf, const BDBG_P_LogEntry *log)
{
    size_t len;
    if(log->rc>0) {
        len = (unsigned)log->rc < sizeof(log->str) ? (unsigned)log->rc : sizeof(log->str)-1;
        BDBG_P_StrBuf_Memcpy(buf, log->str, len);
    }
}

static void BDBG_P_Dequeue_Flush(BDBG_P_StrBuf *buf)
{
    unsigned i;

    BDBG_P_StrBuf_Printf(buf, "___  OVERFLOW ___");
    for(i=0;i<BDBG_P_Dequeue_State.last_used;i++) {
        BDBG_P_Dequeue_Context *context = BDBG_P_Dequeue_State.contexts+i;
        if(context->used) {
            BDBG_P_StrBuf_Printf(buf, "\n___ %s ### OVERFLOW ###");
        }
    }
    return;
}

static void
BDBG_P_Dequeue_FilterString(char *str)
{
    /* remove control character */
    for(;;) {
        char ch = *str;
        if(ch=='\0') {
            break;
        }
        switch(ch) {
        case '\r':
        case '\n':
        case '\b':
        case '\f':
        case '\a':
        case 27: /* escape */
        case 127: /* delete */
            *str = ' ';
            break;
        default:
            break;
        }
        str++;
    }
}

BERR_Code 
BDBG_Log_Dequeue(BDBG_FifoReader_Handle logReader, unsigned *timeout, char *str, size_t str_size, size_t *str_len)
{
    BERR_Code rc;
    BDBG_ModulePrintKind kind;
    BDBG_P_LogEntry logEntry;
    BDBG_P_Dequeue_Context *context;
    BDBG_P_StrBuf buf;
    size_t hdr_len;

    BDBG_ASSERT(timeout);
    BDBG_ASSERT(str);
    BDBG_ASSERT(str_len);

    *timeout = 0;
    *str_len = 0;
    *str = '\0';
    buf.str = str;
    buf.len = 0;
    buf.size = str_size;
    rc = BDBG_FifoReader_Read(logReader, &logEntry, sizeof(logEntry));
    switch(rc) {
    case BERR_SUCCESS:
        break;
    case BERR_FIFO_NO_DATA:
        *timeout = 5;
        if(BDBG_P_Dequeue_State.nodata_count<16) {
            BDBG_P_Dequeue_State.nodata_count++;
            if(BDBG_P_Dequeue_State.nodata_count<4) {
                *timeout = 0;
            }
            *timeout = 1;
        }
        return BERR_SUCCESS;
    case BERR_FIFO_BUSY:
        *timeout = 1;
        return BERR_SUCCESS;
    case BERR_FIFO_OVERFLOW:
        BDBG_P_Dequeue_Flush(&buf);
        *str_len = buf.len;
        BDBG_FifoReader_Resync(logReader);
        BDBG_P_Dequeue_State.nodata_count = 0;
        return BERR_SUCCESS;
    default:
        return BERR_TRACE(rc);
    }
    BDBG_P_Dequeue_State.nodata_count = 0;
    kind = logEntry.tag & 0x03;
    switch(kind) {
    case BDBG_ModulePrintKind_eString:
    case BDBG_ModulePrintKind_eHeaderAndBody:
        BDBG_P_Dequeue_FilterString(logEntry.str);
        BDBG_P_StrBuf_AddLog(&buf, &logEntry);
        break;
    case BDBG_ModulePrintKind_eHeader:
        context = BDBG_P_Dequeue_FindContext(logEntry.tag);
        if(context) {
            BDBG_P_StrBuf_Printf(&buf, "___ %s ### MISSING ###", context->header_buf);
            context->used = false;
        }
        if(logEntry.rc>0) {
            hdr_len = (unsigned)logEntry.rc < sizeof(logEntry.str) ? (unsigned)logEntry.rc +1 : sizeof(logEntry.str);
            context = BDBG_P_Dequeue_FindFree(hdr_len);
            if(context) {
                context->hdr_len = hdr_len;
                context->used = true;
                context->tag = logEntry.tag;
                BKNI_Memcpy(context->header_buf, logEntry.str, hdr_len);
            }
        } 
        break;
    case BDBG_ModulePrintKind_eBody:
        context = BDBG_P_Dequeue_FindContext(logEntry.tag);
        if(context) {
            BDBG_P_Dequeue_FilterString(logEntry.str);
            BDBG_P_StrBuf_Memcpy(&buf, context->header_buf, context->hdr_len-1);
            BDBG_P_StrBuf_AddLog(&buf, &logEntry);
            context->used = false;
        } else {
            BDBG_P_StrBuf_Printf(&buf, "___ ### MISSING ### %s", logEntry.str);
        }
        break;
    default:
        break;
    }
    *str_len = buf.len;
    return BERR_SUCCESS;
}

void 
BDBG_Log_GetElementSize( size_t *elementSize )
{
    BDBG_ASSERT(elementSize);
    *elementSize = sizeof(BDBG_P_LogEntry);
    return;
}

void
BDBG_Log_SetFifo(BDBG_Fifo_Handle fifo)
{
    gDbgState.dbgLog = fifo;
    return;
}

