/******************************************************************************
 *    (c)2011 Humax Corporation
 *
 * This program is the proprietary software of Humax Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Humax
 * (an "Authorized License").  Except as set forth in an Authorized License, Humax grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Humax expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Humax, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Humax integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bipc_server_test.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/4/11 5:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/lib/ipc/build/bipc_server_test.c $
 * 
 * 1   10/4/11 5:44p vsilyaev
 * SW7425-1364: Reference applicaion IPC and reference server
 * 
 *****************************************************************************/
 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include "malloc.h"
#include <sys/wait.h>
#include <stdlib.h>
#include "vkernel.h"
#include "ipc_test.h"
	 
#define BSTD_UNUSED(x)  { volatile void *bstd_unused; bstd_unused = (void *)&(x); }
	 
#define PrintInfo	printf("\t[SERVER] : "), printf
	 
int ipc_server_callback1(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback1] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 1;

	return rc;
}

int ipc_server_callback2(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback2] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 2;

	return rc;
}

int ipc_server_callback3(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback3] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 3;

	return rc;
}

int ipc_server_callback4(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback4] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 4;

	return rc;
}

int ipc_server_callback5(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback5] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 5;

	return rc;
}

int ipc_server_callback6(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback6] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 6;

	return rc;
}

int ipc_server_callback7(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback7] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 7;

	return rc;
}

int ipc_server_callback8(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback8] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 8;

	return rc;
}

int ipc_server_callback9(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback9] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 9;

	return rc;
}

int ipc_server_callback10(IPC_CALL_EXAMPLE_T *msg, unsigned long size)
{
	int rc;

	PrintInfo("\n");
	PrintInfo("[ipc_server_callback10] msg %s, %d\n", msg->buf, size);
	PrintInfo("\n");

	rc = 10;

	return rc;
}


#undef CHECK
#define CHECK	printf("Check Line : %s(%d) tick[%lu]\n", __FILE__, __LINE__, VK_TIMER_GetSystemTick())
int main(int argc, const char *argv[])
{
	int rc;
	int queueIdMsg;
	int queueIdCallback;
	IPC_MSG_T	msg;

	VK_Init();

	PrintInfo("Start\n");
	rc = VK_IPC_MSG_Open(0, 0, "msg_queue", &queueIdMsg);
	if (rc!=VK_OK)
	{
		return -1;	
	}
	PrintInfo("VK_IPC_MSG_Open queueIdMsg : %d \n", queueIdMsg);

	rc = VK_IPC_MSG_Open(0, 0, "callback_queue", &queueIdCallback);
	if (rc!=VK_OK)
	{
		return -1;	
	}
	PrintInfo("VK_IPC_MSG_Open queueIdCallback : %d \n", queueIdCallback);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_1, ipc_server_callback1);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_2, ipc_server_callback2);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_3, ipc_server_callback3);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_4, ipc_server_callback4);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_5, ipc_server_callback5);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_6, ipc_server_callback6);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_7, ipc_server_callback7);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_8, ipc_server_callback8);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_9, ipc_server_callback9);
	VK_IPC_MSG_RegisterCallback(queueIdCallback, IPC_CALLBACK_ID_10, ipc_server_callback10);
	
	while (1)
	{
		rc = VK_IPC_MSG_ReceiveTimeout(queueIdMsg, &msg, sizeof(IPC_MSG_T), 1000);
		if (rc != VK_OK)
		{
			PrintInfo("VK_IPC_MSG_ReceiveTimeout ERROR rc = %d\n", rc);
			rc = VK_IPC_MSG_Receive(queueIdMsg, &msg, sizeof(IPC_MSG_T));
			if (rc != VK_OK)
			{
				PrintInfo("VK_IPC_MSG_Receive ERROR rc = %d\n", rc);
				continue;
			}
		}

		switch (msg.msg)
		{
		case MSG_PRINT_MESSAGE :
			PrintInfo("<== %s\n", msg.buf);
			break;
		case MSG_PROCESS_END :
			PrintInfo("Process END\n");
			goto endofprocess;
			break;
		default :
			break;
		}
	}

	endofprocess:

	VK_IPC_MSG_Close(queueIdMsg);
	VK_IPC_MSG_Close(queueIdCallback);

	PrintInfo("Done\n");

	return 0;
}


