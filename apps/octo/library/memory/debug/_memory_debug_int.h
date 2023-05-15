#ifndef __MEMORY_DEBUG_INT_H__
#define __MEMORY_DEBUG_INT_H__

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
#define MEMDBG_BACKTRACE_FRAME_NUM			(16)
#define MEMDBG_BACKTRACE
//#define MEMDBG_BACKTRACE_USE_HUMAX_FIX /*backtrace_symbols() �� ���� ����. �ʿ��ϸ� �� define ����, make file�� #ccflags-y += -D_GNU_SOURCE  �� Ǯ� ���. */

#if defined(CONFIG_DEBUG_ARM_BACKTRACE)
#define MEMDBG_BACKTRACE_USE_HUMAX_FIX
#endif

typedef int (*MemPrintCb_t) (const HCHAR *format, ...);

HUINT32		_memdbg_GetSystemTick(void);
const HCHAR *	_memdbg_AscTime (HUINT32 unixtime, HCHAR *timebuf, HUINT32 n);
const HCHAR *	_memdbg_RmPath(const HCHAR *file);
int _memdbg_GetCallStack (void **retaddrs, int max_size);
int _memdbg_BacktraceSymbols( void *const *array, int size, MemPrintCb_t pfPrintCb);

#if defined(CONFIG_PROD_CPU_BCM_ARM)
int memdbg_arm_getCallStack (void **retaddrs, int max_size);
int memdbg_arm_backtrace_symbols ( void *const *array, int size, MemPrintCb_t pfPrintCb);
#else
int memdbg_mips_getCallStack (void **retaddrs, int max_size);
int memdbg_mips_backtrace_symbols ( void *const *array, int size, MemPrintCb_t pfPrintCb);
#endif


#endif /* __MEMORY_DEBUG_H__ */


