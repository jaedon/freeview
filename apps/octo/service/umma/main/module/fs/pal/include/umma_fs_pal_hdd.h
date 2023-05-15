/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	umma_fs_pal_hdd.h
	@brief 		driver interfaces for internal storage

	Description: 		\n
	Module: pal/include			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __UMMA_FS_PAL_HDD_H__
#define	__UMMA_FS_PAL_HDD_H__

#include <htype.h>

typedef enum
{
	PAL_HDD_PORT_NUM_NONE = 0,
	PAL_HDD_PORT_NUM_1,
	PAL_HDD_PORT_NUM_2,
	PAL_HDD_PORT_NUM_3,
	PAL_HDD_PORT_NUM_MAX
} PAL_HDD_PortNum_t ;


HERROR	UMMA_FS_PAL_HDD_RegisterCallback (PAL_FS_Event_Callback_Func pfEvtCallback);

int 	UMMA_FS_PAL_HDD_Init(void);
int	UMMA_FS_PAL_HDD_PWR_On(void);
int	UMMA_FS_PAL_HDD_PWR_Off(void);
HERROR UMMA_FS_PAL_HDD_ControlPower(PAL_HDD_PortNum_t ePortnum, HBOOL bOn);

#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
int 	UMMA_FS_PAL_HDD_GetEncryptionKey(PAL_HDD_PortNum_t ePortNum, HINT8 *pKey);
#endif

void UMMA_FS_PAL_HDD_Check2ndIntHdd(void);
void UMMA_FS_PAL_HDD_2ndIntHddDetected(HBOOL bExist);

#endif /* __UMMA_FS_PAL_HDD_H__ */

