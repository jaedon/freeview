
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
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <assert.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>

#include "vkernel.h"
#include "humax_bsu.h"

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
	return BSU_Command(command);
}


