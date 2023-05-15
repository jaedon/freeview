/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  se_papi.c
	@brief

	Description:  									\n
	Module: Pama Secure Handler API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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
#include <vkernel.h>
#include <hlib.h>

#include <se_papi.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HINT32	nClientHandle;
	HINT32	nRPCHandle;
} SEPAPI_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static void		sepapi_event_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	SEPAPI_t	*sepapi = (SEPAPI_t*)userData;

	HLIB_RPC_ProcessRPC(sepapi->nRPCHandle, size, pvData, requestId, nSocketHandle);
}

static void		sepapi_event_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	(void)userData;
	(void)eStatus;
	(void)nClientHandle;
}

static void		sepapi_event_listener(void *arg)
{
	SEPAPI_t	*sepapi = (SEPAPI_t*)arg;

	HLIB_SOCKET_Listen(sepapi->nClientHandle, TRUE, sepapi_event_onPacketReceived, sepapi_event_onStatusChanged, arg);
}

static SEPAPI_t*	sepapi_getInstance(void)
{
	static SEPAPI_t	*s_sepapi = NULL;

	if (s_sepapi == NULL)
	{
		unsigned int	tid;

		s_sepapi = (SEPAPI_t*)HLIB_STD_MemCalloc(sizeof(SEPAPI_t));
		s_sepapi->nClientHandle = HLIB_SOCKET_OpenClientEx(HxSOCKET_UDS, NULL, (void*)SE_PAMA_SERVER, 0);
		s_sepapi->nRPCHandle    = HLIB_RPC_OpenEx(s_sepapi->nClientHandle, HLIB_STD_GetPID(), TRUE);

		VK_TASK_Create(sepapi_event_listener, 70, SIZE_64K, "sepapi_event_listener", (void*)s_sepapi, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);
	}

	return s_sepapi;
}

static void	sepapi_releaseInstance(SEPAPI_t *sepapi)
{
	(void)(sepapi);
}

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
HERROR	SEPAPI_Reboot(HUINT32 ulSettings)
{
#if defined(CONFIG_ENHANCED_SECURITY)
// CONFIG_NO_MICOM_POWER_CONTROL ���� ��쿡��, "reboot" system call�� ȣ���ϴ� ��, apps���� CONFIG_NO_MICOM_POWER_CONTROL�� �� �� ���,
// CONFIG_PROD_CDIG2000C config�� ��ü�Ѵ�.
// CONFIG_NO_MICOM_POWER_CONTROL ������� �ʴ� MICOM�� ����ϴ� ���� obama�� user privilege�� ������ ����, MICOM CMD�� �̿��ؼ� reboot�� �ȴ�.
#if defined(CONFIG_PROD_CDIG2000C) || defined(CONFIG_PROD_HGS1000S)
	SEPAPI_t	*sepapi = sepapi_getInstance();

	HLIB_RPC_Call(sepapi->nRPCHandle, "sepama.reboot", "i", ulSettings);

	sepapi_releaseInstance(sepapi);
#endif
#endif

	return ERR_OK;
}

