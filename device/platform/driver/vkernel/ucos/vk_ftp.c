
/********************************************************************
* Workfile   : vk_ftp.c
* Project    :
* Author     :
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

int VK_TCP_socket(void)
{
	return BSU_TCP_socket();
}


int VK_TCP_connect(int s, unsigned char * dest, unsigned short port)
{
	return BSU_TCP_connect(s, dest, port);
}

int VK_TCP_close(int s)
{
	return BSU_TCP_close(s);
}

int VK_TCP_send(int s, unsigned char * buf, int len)
{
	return BSU_TCP_send(s, buf, len);
}

int VK_TCP_recv(int s, unsigned char * buf, int len)
{
	return BSU_TCP_recv(s, buf, len);
}

int VK_TCP_status(int s, int *connflag, int *rxready, int *rxeof)
{
	return BSU_TCP_status(s, connflag, rxready, rxeof);
}


