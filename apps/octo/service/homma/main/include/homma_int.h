/*************************************************************************************************************
	File            : homma_int.h
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


#ifndef	__HOMMA_INT_H__
#define	__HOMMA_INT_H__

#define	________________Header_Files_______________________________
#include <hlib.h>
#include <homapi.h>

#define ________________Defines____________________________________
//#define HOMMA_SIGNAL_UDS     "/tmp/.homma_signal.tmp"

#define ________________Typedef____________________________________
typedef struct
{
//	HULONG          ulSema;
//	HUINT32         ulMsgQId;
	HINT32          server;
} homma_context_t;

typedef enum {

	eSYS_EVENT_NOT = 0x00,
	eSYS_EVENT_MS_MOUNT = (1 << 16),
	eSYS_EVENT_MS_UNMOUNT,
	eSYS_EVENT_FS_MOVE_DONE,
	eSYS_EVENT_FS_COPY_DONE,
	eSYS_EVENT_FS_DELETE_DONE,
	eSYS_EVENT_FS_RECORD_DONE,

	eSYS_EVENT_NET_IP_SETUP,
	eSYS_EVENT_NET_LAN_UNPLUGGED,
	eSYS_EVENT_NET_LAN_PLUGGED,
	eSYS_EVENT_NET_WLAN_CONNECT

} RO_SYS_Event_e;

/*-----------------------------------------------------------------------------
 *  API Protocol
 *-----------------------------------------------------------------------------*/
typedef enum
{
	HOMxCommType_MESSAGE = 0x0a,
	HOMxCommType_EVENT,
} eHOMxCommType;

typedef enum
{
	HOMxDataOperation_GET,
	HOMxDataOperation_SET,
	HOMxDataOperation_STATUS,
} HOMxDataOperation;

typedef enum
{
	HOMxNotify_Event_Start,
	HOMxNotify_Event_Stop,
	HOMxNotify_Event_Status,
} eHOMxNotifyOperation;

typedef struct
{
//	HUINT32	magic;
	HINT8	type;

	HINT8	service;
	HINT8	operation;
	HUINT32	access;

	HINT8	cmd; /* HOMxDMP_Command_e */
	HCHAR	notifier[64];

	HUINT32	dlen;
	HINT8	*data;
}
#if defined(WIN32)
_hmpacket_t;
#else
__attribute__((packed)) _hmpacket_t;
#endif

typedef struct
{
	HINT8	cmd; /* HOMxDMP_Command_e */
	HINT32	size;
	HINT8	*data;
} comm_msg_data_t;

#define ________________Public_Interfaces__________________________
/* CMD API */
HINT32   roma_cmdtool_dmp(void *szArgList);
#endif
