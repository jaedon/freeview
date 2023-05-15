#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <htype.h>
#include "_memory_debug_int.h"


#define	_____memdbg_type______________________________________________________________

#define	_____memdbg_private______________________________________________________________

#define	_____memdbg_members______________________________________________________________
int _memdbg_GetCallStack (void **retaddrs, int max_size)
{
#if defined(CONFIG_PROD_CPU_BCM_ARM)
	return memdbg_arm_getCallStack(retaddrs, max_size);
#else
	return memdbg_mips_getCallStack(retaddrs, max_size);
#endif
}


int _memdbg_BacktraceSymbols( void *const *array, int size, MemPrintCb_t pfPrintCb)
{
#if defined(CONFIG_PROD_CPU_BCM_ARM)
	return memdbg_arm_backtrace_symbols (array, size, pfPrintCb);
#else
	return memdbg_mips_backtrace_symbols (array, size, pfPrintCb);
#endif
}


