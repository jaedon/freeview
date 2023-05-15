/*************************************************************
 * @file		vk_p_common_api.c
 * @date		
 * @author		
 * @brief		

 * http://www.humaxdigital.com
 *************************************************************/

#define ___HEADER________________________________________________________________________________________

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <htype.h>
#include <sys/queue.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <errno.h>

#include "vkernel.h"
#include "file_map.h"
#include "inc/vk_p_common_api.h"

#if defined(CONFIG_DEBUG)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define UNW_LOCAL_ONLY
#include "libunwind.h"
#include <dlfcn.h>
#endif

#define ___DEFINE________________________________________________________________________________________

#define VK_P_COMM_DEBUG
#ifdef VK_P_COMM_DEBUG
#define PrintDebug(fmt, ...)    VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)         VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)          VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)    VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)         while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)          while (0) ((int (*)(char *, ...)) 0)
#endif /* VK_P_COMM_DEBUG */

#define COMMON_DONE(ret) {						\
		if(ret) {						\
			PrintError("ERROR %s,%d: %s\n", __FUNCTION__, __LINE__, strerror(errno)); \
			goto done;				\
		}							\
	}

#define ___TYPEDEF_______________________________________________________________________________________

#define ___STATIC_VARIABLES______________________________________________________________________________

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#define	___PRIVATE_API___________________________________________________________________________________

#define	___PUBLIC_API____________________________________________________________________________________

int vk_p_common_GetPid(void)
{
	return (int)getpid();
}

int vk_p_common_GetTid(void)
{
	return (int)syscall(SYS_gettid);
}

int vk_p_common_GetProcessName(int pid, char *buffer, int size)
{
	int rc = VK_ERROR;
	int fd = -1;
	int len;
	char file_path[256];
	char proc_name[32] = {0,};
	
	COMMON_DONE((!buffer || !size));

	VK_snprintf(file_path, sizeof(file_path), "/proc/%3d/comm", pid);
	fd = open(file_path, O_RDONLY);
	COMMON_DONE((fd<0));

	len = read(fd, (char*)proc_name, sizeof(proc_name)-1);
	if (len <= 0)
	{
		VK_snprintf(proc_name, sizeof(proc_name), "%s", "unknown");
		goto done;
	}

	proc_name[len-1] = 0; //remove '\n'
	VK_snprintf(buffer, size, "%s", proc_name);
	rc = VK_OK;
done:
	if (fd >= 0) close(fd);
	return rc;
}

int vk_p_common_GetThreadName(int pid, int tid, char *buffer, int size)
{
	int rc = VK_ERROR;
	int fd = -1;
	int len;
	char file_path[256];
	char thread_name[32];
	
	COMMON_DONE((!buffer || !size));

	VK_snprintf(file_path, sizeof(file_path), "/proc/%3d/task/%3d/comm", pid, tid);
	fd = open(file_path, O_RDONLY);
	COMMON_DONE((fd<0));

	len = read(fd, (char*)thread_name, sizeof(thread_name)-1);
	if (len <= 0)
	{
		VK_snprintf(thread_name, sizeof(thread_name), "%s", "unknown");
		goto done;
	}

	thread_name[len-1] = 0; //remove '\n'
	VK_snprintf(buffer, size, "%s", thread_name);
	rc = VK_OK;

done:
	if (fd >= 0) close(fd);
	return rc;
}

int vk_p_common_PrintReturnFunction(int pid, void *ret_addr, const char *file, unsigned long line)
{
#if defined(CONFIG_DEBUG)
	int rc = VK_ERROR;
	char process_name[32];

	rc = vk_p_common_GetProcessName(pid, process_name, sizeof(process_name));
	COMMON_DONE((rc!=VK_OK));

	PrintError("[%s,%d] process(%d, %s), return address (%p)\n", file, line, pid, process_name, ret_addr);
	PrintError("[%s,%d] Use \"addr2line -e /usr/bin/%s -f %p\"\n", file, line, process_name, ret_addr);
	PrintError("[%s,%d] If \"??:??\" is shown, this return function might be in shared lib.\n", file, line);
	PrintError("[%s,%d] Check /proc/pid/maps and where return addr is, then find which shared lib is. and finally calulate offet('return address' - 'base address of library')\n", file, line);
	PrintError("[%s,%d] Use \"addr2line -e /usr/lib/[shared lib] -f [offset]\"\n", file, line);
	
	rc = VK_OK;
done:
	return rc;
#else
	HAPPY(pid);
	HAPPY(ret_addr);
	HAPPY(file);
	HAPPY(line);
	return VK_OK;
#endif
}

void vk_p_common_ShowBackTrace(char* pucProcessName, void *oldcontext)
{
#if defined(CONFIG_DEBUG)
	unw_word_t Offset;
	unw_word_t pc;
	unw_cursor_t stCursor;
	unw_context_t* pstContext;
	unw_context_t stContext;
	unw_proc_info_t stProcInfo;
	Dl_info stDynInfo;
	unsigned int pid = vk_p_common_GetPid();
	char szCmdBuf[512] = {0,};
	char szProcessName[32] = {0,};
	char szFileName[512] = {0,};
	char szFuncName[64] = {0,};
	char szFullProcessPath[64];
	int nLineNum=0;
	int	fd;
	int	nLen = 0;
	int DebugFd;
	int nRet;
	char *pLine;

	if(oldcontext && pucProcessName)
	{
		VK_snprintf(szProcessName, sizeof(szProcessName), "%s", pucProcessName);		
		pstContext = (unw_context_t*)oldcontext;
	}
	else
	{
		/* get process name from /proc */
		VK_snprintf(szCmdBuf, sizeof(szCmdBuf), "/proc/%3d/comm", pid);
		fd = open(szCmdBuf, O_RDONLY);
		if (fd >= 0)
		{
			nLen = read(fd, (char*)szProcessName, sizeof(szProcessName)-1);
			close(fd);
		}

		if (nLen <=0 )
		{
			VK_snprintf(szProcessName, sizeof(szProcessName), "unknown");
		}
		else
		{
			/* when we read strings from comm file, last character value is '\n'. delete it */
			szProcessName[nLen-1] = 0;
		}

		pstContext = &stContext;
		unw_getcontext(pstContext);
	}

	/* get full path of this process for backtrace */
	VK_snprintf(szCmdBuf, sizeof(szCmdBuf), "/proc/%3d/exe", pid);
	nLen = readlink(szCmdBuf, szFullProcessPath, sizeof(szFullProcessPath));
	if (nLen>0 && nLen<(int)(sizeof(szFullProcessPath)))
	{
		/* if success */
		szFullProcessPath[nLen] = 0;
	}
	else
	{
		/* if fail to read link, set default process path */
		VK_snprintf(szFullProcessPath, sizeof(szFullProcessPath), "/usr/bin/%s", szProcessName);
		szFullProcessPath[sizeof(szFullProcessPath)-1] = 0;
	}

	unw_init_local(&stCursor, pstContext);

	VK_SYSTEM_Command("echo \"**********************************************************************\n\" >> "VK_SIG_LOGFILE);
	VK_SYSTEM_Command("echo \"Call Trace:\n\" >> "VK_SIG_LOGFILE);
	do
	{
		VK_memset(szFileName, 0x0, sizeof(szFileName));
		VK_memset(szFuncName, 0x0, sizeof(szFuncName));

		unw_get_reg(&stCursor, UNW_REG_IP, &pc);
		unw_get_proc_name(&stCursor, szFuncName, sizeof(szFuncName), &Offset);

		VK_snprintf(szCmdBuf, sizeof(szCmdBuf), "/usr/sbin/addr2line -C -e %s -f -i %lx > /tmp/debuginfo", szFullProcessPath, (long)pc);
		if(VK_SYSTEM_Command(szCmdBuf) != VK_OK)
		{
			VK_snprintf(szFileName, sizeof(szFileName), "unknown");
			nLineNum = 0;
		}
		else
		{
			DebugFd = open("/tmp/debuginfo", O_RDONLY);
			if(DebugFd < 0)
			{
				VK_snprintf(szFileName, sizeof(szFileName), "unknown");
				nLineNum = 0;
			}
			else
			{
				nLen = read(DebugFd, szCmdBuf, sizeof(szCmdBuf));
				if (nLen < 0) continue;
				close(DebugFd);

				nLen = (nLen >= (int)sizeof(szCmdBuf)) ? (int)(sizeof(szCmdBuf)-1):nLen;
				szCmdBuf[nLen] = 0;

				pLine = VK_strchr(szCmdBuf, (int)'\n');
				pLine = (!pLine) ? szCmdBuf : pLine+1;

				if(*pLine != '?')
				{
					pLine = VK_strchr(pLine, ':');
					if (pLine)
					{
						*pLine++ = 0;
						VK_strncpy(szFileName, szCmdBuf, sizeof(szFileName)-1);
						nLineNum= strtol(pLine, NULL, 10);
					}
				}
				else
				{
					if(unw_get_proc_info(&stCursor, &stProcInfo))
					{
						VK_snprintf(szFileName, sizeof(szFileName), "unknown");
						nLineNum = 0;
					}
					else
					{
						if(dladdr((void *)((long)stProcInfo.start_ip + (long)Offset), &stDynInfo) && stDynInfo.dli_fname &&	*stDynInfo.dli_fname)
						{
							VK_snprintf(szFileName, sizeof(szFileName), "%s", stDynInfo.dli_fname);
							nLineNum = 0;
						}
						else
						{
							VK_snprintf(szFileName, sizeof(szFileName), "unknown");
							nLineNum = 0;
						}
					}
				}				
			}
		}
#if defined(CONFIG_ARM)
		VK_snprintf(szCmdBuf, sizeof(szCmdBuf), " echo \"[0x%x] %s in file %s line %d\n\" >> %s", pc, szFuncName, szFileName, nLineNum, VK_SIG_LOGFILE);
#else
		VK_snprintf(szCmdBuf, sizeof(szCmdBuf), " echo \"[0x%llx] %s in file %s line %d\n\" >> %s", pc, szFuncName, szFileName, nLineNum, VK_SIG_LOGFILE);
#endif
		VK_SYSTEM_Command(szCmdBuf);		
	}while((nRet = unw_step (&stCursor)) > 0);

	VK_SYSTEM_Command("echo \"**********************************************************************\n\" >> "VK_SIG_LOGFILE);
	VK_SYSTEM_Command("cat "VK_SIG_LOGFILE);
#else
	HAPPY(pucProcessName);
	HAPPY(oldcontext);
#endif
}

