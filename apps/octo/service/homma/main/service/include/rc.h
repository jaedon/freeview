/*************************************************************************************************************
	File            : rc.h
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

#ifndef	__RC_H__
#define	__RC_H__

#define	________________Header_Files_______________________________
#include "common.h"
#include "pl_common.h"
#include "pl_rc.h"

#define ________________Private_Mecro_and_Defines__________________
typedef	enum {
	RC_CLIENT_PAIRED,
	RC_CLIENT_UNPAIRED,
	RC_CLIENT_MESSAGE,
	RC_LAUNCH_APP,
	RC_SET_FRIENDLY_NAME,
} RC_Event_e;

typedef enum {
	eRc_State_Stopped,
	eRc_State_Playing,
} RC_State_e;

typedef struct {
	HCHAR 	szName[128];
	HCHAR 	szData[2048];	// [message | url]
	//HUINT32	status; 
} RC_EventInfo_t;

typedef void (*RC_EventCallback_t) (RC_Event_e evt, void *data1, void *data2);

#define ________________Public_Functions___________________________

HERROR 	RC_Init(HCHAR *pszFriendlyName);

HERROR	RC_Deinit(void);

HERROR	RC_SetApplicationURL(void);

HERROR	RC_SetFriendlyName(const HCHAR *pszFriendlyName);

HERROR	RC_UnpairClient(void);

HERROR 	RC_SetPowerStatus(HBOOL bOperating);


HERROR	RC_RegisterCallback(RC_EventCallback_t cb);

HERROR	RC_UnregisterCallback(void);

#if 0
HERROR	RC_RegisterApplication(const HCHAR *name);

HERROR	RC_UnregisterApplication(const HCHAR *name);

HERROR	RC_ApplicationStatusUpdate(const HCHAR *name, DIAL_State_e status);
#endif

HBOOL	RC_IsStart(void);

#endif	/* __DMR_H__ */
/* end od file */

