/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nova_int.h>
/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/
#define	BUS_MSGQ_SIZE	128

/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/
static HLONG s_ulMsgId =0 ;
/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/
static void nova_msg_divide(NOVA_MSG_t *pstMsgData)
{

	printf(HxANSI_COLOR_GREEN("[RECV]msg:0x%x,p1:%x,p2:%x,p3:%x,p4:%x\n"),
		pstMsgData->message,pstMsgData->param1,pstMsgData->param2,pstMsgData->param3,pstMsgData->param4);


	if( pstMsgData->message & MGS_NOVA_SYSTEM_START)
		NOVA_SYS_Msg(pstMsgData);
	else if( pstMsgData->message & MSG_NOVA_RPC_START)
		NOVA_RPC_Msg(pstMsgData);
	else
		HxLOG_Warning("[PASS] msg 0x%x",pstMsgData->message);

}

static void nova_msg_task (void *arg)
{
	NOVA_MSG_t stMsgData;

	while ( TRUE )
	{
		while (APK_EVENT_Dispatch());

		HxSTD_MemSet(&stMsgData,0x0,sizeof(NOVA_MSG_t));
		if(VK_MSG_ReceiveTimeout( s_ulMsgId, &stMsgData, sizeof(NOVA_MSG_t),50) == VK_OK)
		{
			nova_msg_divide(&stMsgData);
		}
		else
		{
			NovaspreadServerProcess();
		}
	}
}

/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/
void NOVA_MSG_Init(void)
{
	HLONG ulTaskId = 0;

	if ( VK_OK != VK_MSG_Create( BUS_MSGQ_SIZE, sizeof(NOVA_MSG_t), "nova_msg", &s_ulMsgId, VK_SUSPENDTYPE_FIFO ))
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return;
	}

	if ( VK_OK != VK_TASK_Create( nova_msg_task, VK_TASK_PRIORITY_MW_REF, SIZE_256K,"nova_msg_task", NULL, &ulTaskId, 0 ))
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return;
	}

	if ( VK_OK != VK_TASK_Start(ulTaskId))
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return;
	}

	HxLOG_Debug("************** NOVA_MSG_Init Done ******************\n");
}


void NOVA_MSG_Send(NOVA_MSG_t *pstMsg)
{
	NOVA_MSG_t stSend;

	if(pstMsg == NULL)
	{
		HxLOG_Critical("pstMsg == null!!!\n");
		return;
	}

	HxSTD_MemCopy(&stSend,pstMsg,sizeof(NOVA_MSG_t));
	HxLOG_Debug(HxANSI_COLOR_YELLOW("[SEND]msg:0x%x,p1:%x,p2:%x,p3:%x,p4:%x\n"),
		stSend.message,stSend.param1,stSend.param2,stSend.param3,stSend.param4);


	(void)VK_MSG_SendTimeout(s_ulMsgId, &stSend, sizeof(NOVA_MSG_t), 0);
}

/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/


