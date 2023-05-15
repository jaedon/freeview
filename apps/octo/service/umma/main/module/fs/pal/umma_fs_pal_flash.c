/**
	@file     	umma_fs_pal_flash.c
	@brief    	connection to flash

	Description: 	monitor flash connection.	\n
	Module: 		pal/fs			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/types.h>

#include "se_uapi.h"

#include "vkernel.h"
#include "htype.h"

#ifdef CONFIG_DEBUG
#define	_DEBUG_
//#define	_DEBUG_ENTER_EXIT_
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Type Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions         *************************/
/*******************************************************************/
HERROR PAL_FS_FLASH_Mount(void)
{
	SEUAPI_MountFlashPartition();
	return ERR_OK;
}
HERROR PAL_FS_FLASH_Unmount(void)
{
	SEUAPI_UnmountFlashPartition();
	return ERR_OK;
}
