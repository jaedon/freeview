/*************************************************************************************************************
	File            : dms.h
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


#ifndef	__DMS_H__
#define	__DMS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	________________Header_Files_______________________________
#include <hlib.h>
#include "common.h"
#include "pl_dms.h"

#define ________________Typedef____________________________________
typedef enum {
	eDMS_EVT_STOP = 0x00,
	eDMS_EVT_START,
	eDMS_EVT_READY,

	eDMS_EVT_UNKNOWN,
	eDMS_EVT_MAX,
} DMS_EVENT_e;

typedef enum {
	eDMS_SYS_EVT_STOP,
	eDMS_SYS_EVT_START,

	eDMS_SYS_EVT_UNKNOWN,
	eDMS_SYS_EVT_MAX,
} DMS_SYS_EVENT_e;

typedef struct
{
//	HCHAR	szModelName[32];
	HCHAR	szHwAddr[NETWORK_MACADDR_MAX]; //for UDN

	HCHAR	szExcludeIface[128]; // ex;

	// Scan Path???
} DMS_ProductInfo_t;

typedef struct
{
	HUINT32		unMsgId;
	HCHAR		cUri[1024];
	HINT32		nStreamType;
	HUINT32		unProfileId;
	HBOOL		bTranscoding;
	HCHAR		cUseragent[64];
	HCHAR		cIP[32];
	HCHAR		cHumaxKey[128];
} DMS_Streaming_t;

typedef struct
{
	HCHAR	szFriendlyName[64];

#if 0
	HINT32	nScanCycle;
	HINT32	nScanGap;

	HUINT16 upnp_port;			// UPnP??port
	HUINT16 webserver_port;		// webserver??port
	HUINT16 upnp_dtcp_port;		// dtcp??port
	HUINT16 uploadserver_port;	// upload port
#endif

} DMS_Config_t;

typedef HINT32 (*DMS_EVT_HANDLE_Func)( DMS_EVENT_e eEvt, HINT32 p1, HINT32 p2, HINT32 p3 );

#define ________________Public_Interfaces__________________________

/* *
 * @brief
 *
 * @param pstNetInfo
 *
 * @return
 **/
HERROR	DMS_SetNetworkInfo(const CO_NET_Info_t *pstNetInfo);

/* *
 * @brief
 *
 * @param DMS_EVT_HANDLE_Func
 *
 * @return
 **/
void 	DMS_SetEvtListener(DMS_EVT_HANDLE_Func cbNotify);

/* *
 * @brief
 *
 * @param nEvent
 * @param p1
 * @param p2
 * @param p3
 *
 * @return
 **/
HERROR 	DMS_OnSystemEvt(HINT32 nEvent, HINT32 p1, HINT32 p2, HINT32 p3);

/* *
 * @brief
 *
 * @param pstConfig
 *
 * @return
 **/
HERROR	DMS_Start(DMS_Config_t *pstConfig);

/* *
 * @brief
 *
 * @param void
 *
 * @return
 **/
HERROR	DMS_Stop(void);

HERROR	DMS_SetName( const HCHAR *pszFriendlyName );
/* *
 * @brief
 *
 * @param pvData
 *
 * @return
 **/
HERROR	DMS_GetConfInfo(void *pvData);

HERROR	DMS_SetStorageInfo( const CO_STORAGE_Info_t *pstStoInfo );

HERROR	DMS_StartScan( HLONG nInterval, HLONG nInterval_item );
HERROR	DMS_StopScan(void);

void 	DMS_Initialize( void );

HERROR 	DMS_OnSystemEvent( HOMMA_BUS_Event_e eEvent, void *eDataType, void *pvData  );

/* *
 * @brief				Local Path¸¦ ÅëÇØ URI ¹× Meta Á¤º¸¸¦ ¾ò´Â ÇÔ¼ö
 *
 * @param void
 *
 * @return
 **/
HERROR	DMS_GetFlingInfoFromPath( HCHAR *pPath, HCHAR **ppOutUri, HCHAR **ppOutMeta );

HERROR	DMS_GetFlingInfoOfPVRFromPath( HCHAR *pPath, HCHAR **ppOutUri, HCHAR **ppOutMeta );

HERROR	DMS_GetFlingInfoFromSvcUid( HINT32 hSvc, HCHAR **ppOutUri, HCHAR **ppOutMeta );

HERROR	DMS_GetLiveStreamInfo( HINT32 hSvc, HUINT32 *pnCount, PL_DMS_LiveStreamInfo_t **pList );

HERROR	DMS_UpdateSyncEvent(HUINT32 unMsgId, void *pData);

HBOOL	DMS_IsStart(void);
#ifdef __cplusplus
};
#endif // __cplusplus

#endif // __DMS_H__

/* end od file */
