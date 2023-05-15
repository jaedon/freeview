
/********************************************************************
* Workfile   : vk_system.c
* Project    : Kraken
* Author     : khwoo
* Description: Commands Template
*
*                                 Copyright (c) 2009 HUMAX Co., Ltd.
*                                               All rights reserved.
********************************************************************/

#define _______________________________________________________________________
#define ____HEADER_FILES____
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include "vkernel.h"

#define _______________________________________________________________________
#define ____MACRO_DEFINITION____
#define DEBUG_MODULE			TRACE_MODULE_VK_SEM
#define DEFAULT_MAX_FD_SIZE		1024

#ifdef SYSTEM_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else 
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)				
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)		
#endif /* SYSTEM_DEBUG */


#define _______________________________________________________________________
#define ____EXTERNAL_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____EXTERNAL_FUNCTION_DEFINITION____


#define _______________________________________________________________________
#define ____TYPE_DEFINITION____


#define _______________________________________________________________________
#define ____GLOBAL_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____STATIC_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____STATIC_FUNCTION_DEFINITION____

#define _______________________________________________________________________
#define ____STATIC_FUNCTION_BODY____

#define _______________________________________________________________________
#define ____GLOBAL_FUNCTION_BODY____


/********************************************************************
function:  VK_SYSTEM_Command
description :
argument:
return:
*********************************************************************/
int VK_SYSTEM_Command(const char *command)
{
	int pid, status;

	if (command == NULL)
	{
		PrintError("command == 0\n",__func__, __LINE__);
		return 1;
	}

	pid = vfork();
	if (pid == -1)
	{
		PrintError("pid == -1\n",__func__, __LINE__);
		if (errno == EAGAIN)
		{
			PrintError("errno = EAGAIN\n",__func__, __LINE__);
		}
		if (errno == ENOMEM)
		{
			PrintError("errno = ENOMEM\n",__func__, __LINE__);
		}
		return VK_ERROR;
	}

	if (pid == 0)
	{
		char *argv[4];
		int max_fd,fd_inherit,rc;

		max_fd = getdtablesize();
		if(max_fd == -1){
			max_fd = DEFAULT_MAX_FD_SIZE;
		}

		for(fd_inherit=3; fd_inherit<max_fd; fd_inherit++)   /*check descriptor size and skip stdin,out,err*/
		{
			rc = close(fd_inherit);
#ifdef SYSTEM_DEBUG
			if(!rc){
				PrintDebug("closing fd %d, pid=%d\n", fd_inherit, getpid());
			}
#endif
		}

		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = (char *)command;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);
		exit(127);
	}

	do
	{
		if (waitpid(pid, &status, 0) == -1)
		{
			PrintError("pid == -1, errno = %d\n",__func__, __LINE__, errno);
			if (errno != EINTR)
				return VK_ERROR;
		}
		else
		{
			return (status == 0) ? VK_OK : VK_ERROR;
		}
	} while(1);
	return VK_ERROR;
}
