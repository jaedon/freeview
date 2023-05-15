/*
 * Copyright (c) 2006 HUMAX Co., Ltd. All rights reserved.
 *
 * This is very confidential.
 * This code must not distributed outside.
 *
 * Author : Dongseok, Shin (dsshin)
 * File : vk_sig.c
 */

/*----------------------------------------------------------------------------------
** Headers
**--------------------------------------------------------------------------------*/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "vkernel.h"

#include <linden_trace.h>

/*----------------------------------------------------------------------------------
** Defines
**--------------------------------------------------------------------------------*/
#define DEBUG_MODULE			TRACE_MODULE_VK

/*----------------------------------------------------------------------------------
** Typedefs
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** Variables
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** Function Declaration
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** Functions
**--------------------------------------------------------------------------------*/
int VK_SIG_SetSigAction(int sig, void (*pfnSignalUserHandler)(int sig, VK_SIG_INFO_t *siginfo_t, void *context))
{

	HAPPY(sig);
	HAPPY(pfnSignalUserHandler);

	return VK_OK;
}

int VK_SIG_SetSigHooking(int sig)
{

	HAPPY(sig);
	return VK_OK;
}

int VK_SIG_Init(void)
{

	return VK_OK;
}
