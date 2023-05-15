/*************************************************************************************************************
	File            : pl_dmr.h
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

	THE SOFTWARE IS PROVIDED ����AS IS���� AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

#ifndef	__PL_DMR_H__
#define	__PL_DMR_H__

#define	________________Header_Files_______________________________
#include "common.h"

#define ________________Defines____________________________________
#define	PL_DMR_NAME_MAX				( 512 )
#define PL_DMR_NAME_MIDDLE			( 128 )
#define PL_DMR_NAME_SHORT			( 64 )
#define	PL_DMR_URL_MAX				( 512 )

#define	PL_DMR_UDN_LEN				( 36 )			/* UDN�� ���� 8-4-4-4-12�̹Ƿ� 36�̸� �ȴ�. */
#define	PL_DMR_UDN_MAX				( 48 )			/* Prefix�� ������ ����� �ִ� Length */

#define ________________Typedef____________________________________
/* DMR ���� ���� ���� */
typedef enum {
	ePL_DMR_Scope_Internal = 1,		// �ڱ� �ڽŸ� ���
	ePL_DMR_Scope_All,				// ��� IP ���
	ePL_DMR_Scope_External,			// �ܺθ� ���
} PL_DMR_Scope_e;

/* DMC�� ������ Event ���� */
typedef enum {
	ePL_DMR_UPnPState_Playing						= 1,
	ePL_DMR_UPnPState_Stopped						= 2,
	ePL_DMR_UPnPState_Paused						= 3,
	ePL_DMR_UPnPState_Transitioning					= 4,
	ePL_DMR_UPnPState_CurrentDuration				= 20,	// value is long
	ePL_DMR_UPnPState_CurrentTrackDuration			= 21,	// value is long
	ePL_DMR_UPnPState_Volume						= 300,	// value is unsigned short 0 ~ 100
	ePL_DMR_UPnPState_Mute							= 301,	// value is int 0 or 1
	ePL_DMR_UPnPState_CurrentDuration_NotInvoke		= 3003,
} PL_DMR_UPnPState_e;

typedef struct {
	HCHAR	szManufacturer[ PL_DMR_NAME_SHORT ];
	HCHAR	szManufacturerUrl[ PL_DMR_URL_MAX ];
	HCHAR	szModelName[ PL_DMR_NAME_MIDDLE ];
	HCHAR	szModelDescription[ PL_DMR_NAME_MAX ];
	HCHAR	szModelNumber[ PL_DMR_NAME_SHORT ];
	HCHAR	szModelUrl[ PL_DMR_URL_MAX ];
} PL_DMR_ProductInfo_t;

typedef struct {
	PL_DMR_Scope_e    	eScope;
	HINT32				nPortNumber;
	HCHAR				szUDN[ PL_DMR_UDN_MAX ];
	HCHAR				szFriendlyName[ PL_DMR_NAME_MAX ];
	HCHAR				szSerialNumber[ PL_DMR_NAME_SHORT ];
	HCHAR				*pProtocolInfo;
} PL_DMR_ServiceInfo_t;

typedef enum
{
	ePL_DMR_AVT_EVT_SETURI,
	ePL_DMR_AVT_EVT_PLAY,
	ePL_DMR_AVT_EVT_STOP,
	ePL_DMR_AVT_EVT_PAUSE,
	ePL_DMR_AVT_EVT_SEEK,
	ePL_DMR_AVT_EVT_MUTE,
	ePL_DMR_AVT_EVT_VOLUME,
	ePL_DMR_AVT_EVT_BRINGIN /* non-upnp action, but request current media URI to update fling by webapp */
} PL_DMR_AVT_EVT_e;

typedef HINT32 (*PL_DMR_AVT_EventCallback) (HINT32 evt, HINT32 p1, HINT32 p2);

#define ________________Public_Functions___________________________
/* *
 * @brief AVTransport Callback ����
 *
 * @param cb
 *
 * @return
 **/
HERROR	PL_DMR_RegisterAVTEventCallback(PL_DMR_AVT_EventCallback cb);

/* *
 * @brief DMR �ʱ�ȭ, ���� ���� ����
 *
 * @param pServiceInfo
 *
 * @return
 **/
HERROR	PL_DMR_InitDmr(HCHAR *pszFriendlyName, HUINT32 nPort, PL_DMR_Scope_e eScope) ;

/* *
 * @brief DMR ���� ���� - ����
 *
 * @return
 **/
HERROR	PL_DMR_Start( void );

/* *
 * @brief DMR ���� ���� - �ߴ�
 *
 * @return
 **/
HERROR	PL_DMR_Stop( void );


/* DMR Friendly Name ���� */
HERROR	PL_DMR_SetName( HCHAR *pDmrName );

/* *
 * @brief DMR ���� ���� ����
 *
 * @param eScope
 *
 * @return
 **/
HERROR 	PL_DMR_SetServiceScope(PL_DMR_Scope_e eScope);

/* *
 * @brief  ���� DMR�� PB State�� ���� - DMC�� ����
 *
 * @param eState
 * @param value
 *
 * @return
 **/
HERROR 	PL_DMR_SetUPnPState(PL_DMR_UPnPState_e eState, void *value);
HERROR	PL_DMR_GetUpnpState(PL_DMR_UPnPState_e *eUPnPState);

/* *
 * @brief
 *
 * @param ullTime
 *
 * @return
 **/
HERROR	PL_DMR_SetMediaDuration(HUINT64 ullTime);

/* *
 * @brief
 *
 * @param ullTime
 *
 * @return
 **/
HERROR	PL_DMR_SetMediaPlayingPosition(HUINT64 ullTime);

/* *
 * @brief
 *
 * @param item
 *
 * @return
 **/
HERROR	PL_DMR_GetCurrentItem(CO_MediaInstence_t **item);

/* Fling�� URI �� MetaData�� Set �Ҽ� �ִ� API */
HERROR 	PL_DMR_SetFlingInfo( HCHAR *pUri, HCHAR *pCurrentMeta );

/* ���� ������ Fling�� URI�� Ȯ���ϱ� ���� �Լ�. ȣ��ο��� �޸� ���� �ؾ� �� !!!! */
HCHAR* 			PL_DMR_GetFlingURI( void );

/* ���� ������ Fling�� Meta�� Ȯ���ϱ� ���� �Լ�. ȣ��ο��� �޸� ���� �ؾ� �� !!!! */
HCHAR* 			PL_DMR_GetFlingMeta( void );

HERROR		PL_DMR_RequestBringInURL(void);

#endif
/* end od file */
