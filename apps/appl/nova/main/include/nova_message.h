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

#ifndef ____NOVA_MESSAGE_H____
#define ____NOVA_MESSAGE_H____


#define MSG_NOVA_MSG_START          0x00010000
#define MSG_NOVA_RPC_START          0x00020000

typedef enum
{
	MGS_NOVA_SYSTEM_START                   = MSG_NOVA_MSG_START,
	MGS_NOVA_SYSTEM_POWER_STATUS_CHANGE,
	MGS_NOVA_SYSTEM_NETWORK_STATUS_CHANGE,
	MGS_NOVA_SYSTEM_FACTORY_DEFAULT,

	MGS_NOVA_RPC_START                      = MSG_NOVA_RPC_START,
	MGS_NOVA_RPC_STOP,
	MGS_NOVA_RPC_SET_FRIENDLYNAME,

	MGS_NOVA_END
}NOVA_MsgType_t;


typedef struct{
	NOVA_MsgType_t		message;
	HINT32				param1;
	HINT32				param2;
	HINT32				param3;
	HINT32				param4;
}NOVA_MSG_t;

extern void NOVA_MSG_Init(void);
extern void NOVA_MSG_Send(NOVA_MSG_t *pstMsg);

#endif


