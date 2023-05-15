#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>

#include <htype.h>
#include <vkernel.h>
#include "_memory_debug_int.h"
#include <execinfo.h>
#include <sys/types.h>
#include <unistd.h>

#define WORD_WIDTH (160) /* it was adjusted due of dli/sli name.(128 -> 17 + 64 + 15+  48 + 15 = approx.160.)*/
static char s_outbuf[WORD_WIDTH];


#define	_____memdbg_type______________________________________________________________

#define	_____memdbg_private______________________________________________________________

static void memdbg_arm_util_findandsetsymbol(void *ra)
{
	int pid = getpid();
	char szCmdBuf[512] = {0,};
	char szFullFilePath[512] = {0,};
	char szFuncName[64] = {0,};
	FILE* DebugFd;
	char *pszFileAndLine = NULL;
	char szFullProcessPath[64];
	int nLen;

	memset(szFullProcessPath, 0x0, sizeof(szFullProcessPath));
	memset(szFullFilePath, 0x0, sizeof(szFullFilePath));
	memset(szFuncName, 0x0, sizeof(szFuncName));

	/* get full path of this process for backtrace */
	snprintf(szCmdBuf, sizeof(szCmdBuf), "/proc/%3d/exe", pid);
	nLen = readlink(szCmdBuf, szFullProcessPath, sizeof(szFullProcessPath));
	if (nLen>0 && nLen<(int)(sizeof(szFullProcessPath)))
	{
		/* if success */
		szFullProcessPath[nLen] = 0;
	}
	szFullProcessPath[64-1] = 0;

	snprintf(szCmdBuf, sizeof(szCmdBuf), "/usr/sbin/addr2line -C -e %s -f -i %lx > /tmp/debuginfo", szFullProcessPath, (long)ra);
	if(VK_SYSTEM_Command(szCmdBuf) != VK_OK)
	{
		return;
	}

	DebugFd = fopen("/tmp/debuginfo", "r");
	if(!DebugFd)
	{
		return;
	}

	fgets(szFuncName, sizeof(szFuncName), DebugFd);
	szFuncName[strcspn(szFuncName, "\n")] = 0;		//remove newline
	fgets(szFullFilePath, sizeof(szFullFilePath), DebugFd);
	szFullFilePath[strcspn(szFullFilePath, "\n")] = 0;		//remove newline

	pszFileAndLine = strrchr(szFullFilePath, '/');
	if (pszFileAndLine != NULL)
	{
		++pszFileAndLine;
	}

	snprintf(s_outbuf,WORD_WIDTH,"%s (%s) [0x%x]",pszFileAndLine, szFuncName,(unsigned int)ra);

	fclose(DebugFd);
}

#define	_____memdbg_members______________________________________________________________
int memdbg_arm_getCallStack (void **retaddrs, int max_size)
{
	return backtrace(retaddrs, max_size);
}





/*
Dl_info,



char  dli_fname [PATH_MAX]
  Filename of defining object.


 void *  dli_fbase
  Load address of that object.


 const char *  dli_sname
  Name of nearest lower symbol.


 void *  dli_saddr
  Exact value of nearest symbol.

*/
int memdbg_arm_backtrace_symbols ( void *const *array, int size, MemPrintCb_t pfPrintCb)
{
	HINT32		cnt;
	char		**btfuncnames = NULL;
	char		*parenthesis = NULL;

	// get symbol info.
	btfuncnames = backtrace_symbols(array, size);
	if (NULL != btfuncnames)
	{
		for (cnt = 0; cnt < size; cnt++)
		{
			parenthesis = NULL;
			parenthesis = strchr(btfuncnames[cnt],'(');

			// case (): fail to find function symbol => use addr2line
			if (parenthesis == NULL || *(parenthesis+1) == ')')
			{
				memset(s_outbuf, 0, sizeof(s_outbuf));
				memdbg_arm_util_findandsetsymbol(array[cnt]);
				pfPrintCb("- Frame (%02d) : ptr(0x%08x) with [%s]\n", cnt, array[cnt], s_outbuf);
			}
			else
			{
				// found function symbol => use backtrace_symbols's string
				pfPrintCb("- Frame (%02d) : ptr(0x%08x) with [%s]\n", cnt, array[cnt], btfuncnames[cnt]);
			}
		}

		free(btfuncnames);

		return 0;
	}


	return 0;
}




