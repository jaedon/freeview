/******************************************************************************
 *    (c)2011 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
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
 * $brcm_Workfile: bipc_client_test.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/4/11 5:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/lib/ipc/build/bipc_client_test.c $
 * 
 * 1   10/4/11 5:44p vsilyaev
 * SW7425-1364: Reference applicaion IPC and reference server
 * 
 *****************************************************************************/

#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "malloc.h"
#include <stdlib.h>

#include "vkernel.h"
#include "ipc_test.h"

#define BSTD_UNUSED(x)  { volatile void *bstd_unused; bstd_unused = (void *)&(x); }

#define VK_IPC_MSG_TESTMESSAGE1 	"# TEST 1.# : 11112345678909999"

#define PrintInfo	printf("[CLIENT] : "), printf

int main(int argc, const char *argv[])
{
	int rc;
	int queueIdMsg;
	int queueIdCallback;
	char buf[64];
	IPC_MSG_T	msg;
	IPC_CALL_EXAMPLE_T	stText;
	unsigned int	result;

	VK_Init();

	rc = VK_IPC_MSG_Open(0, 0, "msg_queue", &queueIdMsg);
	if (rc!=VK_OK)
	{
		PrintInfo("VK_IPC_MSG_Open ERROR!! \n");
		return -1;
	}
	PrintInfo("VK_IPC_MSG_Open queueIdMsg : %d \n", queueIdMsg);

	rc = VK_IPC_MSG_Open(0, 0, "callback_queue", &queueIdCallback);
	if (rc!=VK_OK)
	{
		PrintInfo("VK_IPC_MSG_Open ERROR!! \n");
		return -1;
	}
	PrintInfo("VK_IPC_MSG_Open queueIdMsg : %d \n", queueIdCallback);

	PrintInfo("Start\n");
	if (argc<2)
	{
		strcpy(stText.buf, "call function message 1");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_1, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_1] return value (%d)\n", result);
		strcpy(stText.buf, "call function message 2");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_2, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_2] return value (%d)\n", result);
		strcpy(stText.buf, "call function message 3");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_3, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_3] return value (%d)\n", result);
		strcpy(stText.buf, "call function message 4");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_4, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_4] return value (%d)\n", result);
		strcpy(stText.buf, "call function message 5");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_5, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_5] return value (%d)\n", result);
		strcpy(stText.buf, "call function message 6");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_6, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_6] return value (%d)\n", result);
		strcpy(stText.buf, "call function message 7");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_7, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_7] return value (%d)\n", result);
		strcpy(stText.buf, "call function message 8");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_8, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_8] return value (%d)\n", result);
		strcpy(stText.buf, "call function message 9");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_9, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_9] return value (%d)\n", result);
		strcpy(stText.buf, "call function message 10");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_10, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		PrintInfo("[IPC_CALLBACK_ID_10] return value (%d)\n", result);
	}
	else if (atoi(argv[1])==0)
	{
		PrintInfo("==>VK_IPC_MSG_SendTimeout : MSG_PROCESS_END\n");
		msg.msg = MSG_PROCESS_END;
		VK_IPC_MSG_SendTimeout(queueIdMsg, &msg, sizeof(IPC_MSG_T), 1000);
	}
	else if (atoi(argv[1])==1)
	{
		/*
		* TEST 1
		* Sending the data to the server process
		* Sending the data to the server process
		*/
		PrintInfo(" ==> %s\n", VK_IPC_MSG_TESTMESSAGE1);
		msg.msg = MSG_PRINT_MESSAGE;
		strcpy(msg.buf, VK_IPC_MSG_TESTMESSAGE1);
		rc = VK_IPC_MSG_Send(queueIdMsg, &msg, sizeof(IPC_MSG_T));
		if (rc!=VK_OK)
		{
			PrintInfo("VK_IPC_MSG_Send ERROR!! \n");
			return -1;
		}
	}
	else if(atoi(argv[1])==2)
	{
		/*
		* TEST 2
		* Sending the data to the server process usging time out function
		*/
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_1, &result, &msg, sizeof(IPC_MSG_T));
	}
	else if(atoi(argv[1])==3)
	{
		/*
		* TEST 2
		* Sending the data to the server process usging time out function
		*/
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_2, &result, &msg, sizeof(IPC_MSG_T));
	}
	else if(atoi(argv[1])==4)
	{
		strcpy(stText.buf, "call function message 1");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_1, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		strcpy(stText.buf, "call function message 2");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_2, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		strcpy(stText.buf, "call function message 3");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_3, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		strcpy(stText.buf, "call function message 4");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_4, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		strcpy(stText.buf, "call function message 5");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_5, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		strcpy(stText.buf, "call function message 6");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_6, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		strcpy(stText.buf, "call function message 7");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_7, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		strcpy(stText.buf, "call function message 8");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_8, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		strcpy(stText.buf, "call function message 9");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_9, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
		strcpy(stText.buf, "call function message 10");
		VK_IPC_MSG_Call(queueIdCallback, IPC_CALLBACK_ID_10, &result, &stText, sizeof(IPC_CALL_EXAMPLE_T));
	}


	/*
	* TEST 6
	* Closing the identifier
	*/
	VK_IPC_MSG_Close(queueIdMsg);
	VK_IPC_MSG_Close(queueIdCallback);
	PrintInfo("Done\n");
	return 0;
}



