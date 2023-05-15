/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  se_pama.c
	@brief

	Description:  									\n
	Module: Pama Secure handler Main 				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <unistd.h>
#include <sys/reboot.h>

#include "vkernel.h"
//#include "di_power.h"
#include "hlib.h"
#include "se_papi.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
static HINT32	s_nServerHandle;
static HINT32	s_nRPCHandle;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static HERROR sepama_rpc_Reboot(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32	ulSettings;

	HxLOG_Assert(nArgc == 1);

	ulSettings = (HUINT32)HxOBJECT_INT(apArgv[0]);

	// 리부팅할 때, 시스템 리소스가 해제되고 나서, 리부팅이 이뤄져야 한다.
	// DI_POWER_SetMainChipReset() 함수는 시스템 리소스 해제하는 코드를 포함하고 있지만,
	// user privilege의 obama를 통해 호출되면, "reboot" system call을 수행하지 못한다.
	// 그래서, user privilege의 obama가 DI_POWER_SetMainChipReset() 함수를 호출해서, 시스템 리소스 해제하는 코드를 수행할 때까지,
	// se_pama가 VK_TASK_Sleep으로 대기하고, "reboot" system call을 수행하도록 한다.
	// DI_POWER_SetMainChipReset()를 shared library로 분리하는 것도 고려했으나, 일이 커져서 이렇게 처리한다.
#if 1
	sync();
	reboot(RB_AUTOBOOT);
#else
	DI_POWER_SetMainChipReset();
#endif

	(void)(ulSettings);
	return ERR_OK;
}

static void		sepama_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	HLIB_RPC_ProcessRPC(s_nRPCHandle, size, pvData, requestId, nSocketHandle);
}

static void		sepama_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	if (eStatus == HxSOCKET_REMOVE_CLIENT)
	{
		HLIB_RPC_RemoveClientHandle(s_nRPCHandle, nClientHandle);
	}
}

HINT32	main(HINT32 argc, char ** argv)
{
	VK_Init();

#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();
#endif

	s_nServerHandle = HLIB_SOCKET_OpenServer(HxSOCKET_UDS, (void*)SE_PAMA_SERVER);
	s_nRPCHandle    = HLIB_RPC_OpenEx(s_nServerHandle, HLIB_STD_GetPID(), TRUE);

	HLIB_RPC_RegisterCall(s_nRPCHandle, "sepama.reboot", "i", sepama_rpc_Reboot, NULL, "sepama.reboot(int settings)");

	while (1)
		HLIB_SOCKET_Listen(s_nServerHandle, TRUE, sepama_onPacketReceived, sepama_onStatusChanged, NULL);

	return 0;
}


