/*************************************************************************************************************
	File            : dial.h
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¢®¡ÆAS IS¢®¡¾ AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#ifndef	__DIAL_H__
#define	__DIAL_H__

#define	________________Header_Files_______________________________ 
#include "common.h"
#include "pl_common.h"
#include "pl_dial.h"

#define ________________Private_Mecro_and_Defines__________________
typedef	enum {
	DIAL_APP_LAUNCH,
	DIAL_APP_STATUS,
	DIAL_APP_STOP,
} DIAL_Event_e;

typedef enum {
	eDial_State_Stopped,
	eDial_State_Playing,
} DIAL_State_e;

typedef struct {
	HCHAR 	szApplicationName[128];
	HCHAR 	szUrl[DIAL_MAX_PAYLOAD + 1];
	HUINT32	status;
} DIAL_EventInfo_t;

typedef void (*DIAL_EventCallback_t) (DIAL_Event_e evt, const HCHAR * appname, void * data);

#define ________________Public_Functions___________________________

HERROR 	DIAL_Init(HCHAR *pszFriendlyName);
HERROR	DIAL_Deinit(void);
HERROR	DIAL_SetApplicationURL(void);
HERROR	DIAL_SetFriendlyName(const HCHAR *pszFriendlyName);
HERROR	DIAL_RegisterCallback(DIAL_EventCallback_t cb);
HERROR	DIAL_UnregisterCallback(void);
HERROR	DIAL_RegisterApplication(const HCHAR *name);
HERROR	DIAL_UnregisterApplication(const HCHAR *name);
HERROR	DIAL_ApplicationStatusUpdate(const HCHAR *name, DIAL_State_e status);
HBOOL	DIAL_IsStart(void);
HERROR	DIAL_SetCookie(const HCHAR * cookie);
HERROR DIAL_GetFriendlyName(HCHAR * pFriendlyName, HUINT32 size);
HERROR DIAL_GetAdditionalDataUrl(HCHAR * pAdditionalDataUrl, HUINT32 size, HCHAR * appName);

#endif

