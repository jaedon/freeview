/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	umma_fs_pal_flash.h
	@brief 		driver interfaces for flash

	Description: 		\n
	Module: pal/include			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __UMMA_FS_PAL_FLASH_H__
#define	__UMMA_FS_PAL_FLASH_H__

#include <htype.h>
#include "umma_fs_pal_fs.h"

HERROR PAL_FS_FLASH_Mount(void);
HERROR PAL_FS_FLASH_Unmount(void);
#endif /* __UMMA_FS_PAL_FLASH_H__ */
