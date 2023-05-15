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



#ifndef __NOVAAPI_H__
#define __NOVAAPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	________________Header_Files_______________________________


#define RPC_NOVAAPI_START                   "novaapi.start"
#define RPC_NOVAAPI_STOP                    "novaapi.stop"
#define RPC_NOVAAPI_SET_FRIENDLYNAME        "novaapi.name"

#define RPC_NOVA_EVENT                      "nova.OnEvent"

typedef enum
{
	eNOVAAPI_SATIP_Event_Started,
	eNOVAAPI_SATIP_Event_Stopped,
	eNOVAAPI_SATIP_Event_Status,
} NOVAAPI_Event_e;

typedef	HERROR	(*NOVAAPI_EventCallback)(NOVAAPI_Event_e eEvt, HUINT32 ulParam2, HUINT32 ulParam3);


void	NOVAAPI_SATIP_Init( void );
void	NOVAAPI_SATIP_DeInit( void );
void	NOVAAPI_SATIP_RegisterNotifier( NOVAAPI_EventCallback pfnObjectHandler );
void	NOVAAPI_SATIP_UnRegisterNotifier( void );
HERROR	NOVAAPI_SATIP_Start( HCHAR *pFriendlyName);
HERROR	NOVAAPI_SATIP_Stop( void );
HERROR	NOVAAPI_SATIP_SetFriendlyName(const  HCHAR *pFriendlyName);



#define ________________Public_Interfaces__________________________
#ifdef	__cplusplus
}
#endif

#endif //end of __NOVAAPI_H__


