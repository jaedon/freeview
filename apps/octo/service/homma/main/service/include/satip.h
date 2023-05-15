/*************************************************************************************************************
	File            : satip.h
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

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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


#ifndef	__SATIP_H__
#define	__SATIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	________________Header_Files_______________________________ 
#include <hlib.h>
#include "common.h"

#define ________________Typedef____________________________________
typedef enum {
	eSATIP_EVT_STOP = 0x00,
	eSATIP_EVT_START,
	eSATIP_EVT_READY,

	eSATIP_EVT_UNKNOWN,
	eSATIP_EVT_MAX,
} SATIP_EVENT_e;

typedef enum {
	eSATIP_SYS_EVT_STOP,
	eSATIP_SYS_EVT_START,

	eSATIP_SYS_EVT_UNKNOWN,
	eSATIP_SYS_EVT_MAX,
} SATIP_SYS_EVENT_e;

typedef struct 
{
	HCHAR	szFriendlyName[64];

	HUINT16 upnp_port;			// UPnP??port 
	HUINT16 webserver_port;		// webserver??port

} SATIP_Config_t;

typedef HINT32 (*SATIP_EVT_HANDLE_Func)( SATIP_EVENT_e eEvt, HINT32 p1, HINT32 p2, HINT32 p3 );

#define ________________Public_Interfaces__________________________

HERROR	SATIP_SetNetworkInfo(const CO_NET_Info_t *pstNetInfo);

void 	SATIP_SetEvtListener( SATIP_EVT_HANDLE_Func cbNotify );

HERROR	SATIP_OnSystemEvt(HINT32 nEvent, HINT32 p1, HINT32 p2, HINT32 p3);

HERROR	SATIP_Start( const HCHAR *pszFriendlyName);

HERROR	SATIP_Stop(void);

HERROR	SATIP_OnSystemEvent( HOMMA_BUS_Event_e eEvent, void *eDataType, void *pvData  );

HBOOL	SATIP_IsStart(void);

HERROR	SATIP_SetFriendlyName(HCHAR *pszFriendlyName);
#ifdef __cplusplus
};
#endif // __cplusplus

#endif // __SATIP_H__

/* end od file */
