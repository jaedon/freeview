/*************************************************************************************************************
	File            : dmr.h
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

#ifndef	__DMR_H__
#define	__DMR_H__

#define	________________Header_Files_______________________________ 
#include "common.h"
#include "pl_common.h"
#include "pl_dmr.h"


#define ________________Private_Mecro_and_Defines__________________
#define	DMR_NAME_MAX		512
#define	DMR_URI_MAX			512
#define	DMR_UDN_MAX			48
#define DMR_TITLE_MAX		128

typedef	enum {
	eDMR_StatusEvent_STOP=0x0,
	eDMR_StatusEvent_START,
	eDMR_StatusEvent_READY,
	eDMR_StatusEvent_UNKNOWN,
	eDMR_StatusEvent_MAX,
} DMR_StatusEvent_e;

typedef	enum {
	eDMR_UPnPAction_SETURI=0,
	eDMR_UPnPAction_PLAY,            
	eDMR_UPnPAction_STOP,            
	eDMR_UPnPAction_PAUSE,            
	eDMR_UPnPAction_SEEK,            
	eDMR_UPnPAction_VOLUME,       
	eDMR_UPnPAction_MUTE,
	eDMR_UPnPAction_BRINGIN, /* non-upnp action, but request current media URI to update fling by webapp */
} DMR_UPnPAction_e;

/* DMC에 전달할 Event 정의 */
typedef enum {
	eDMR_UPnPState_Playing							= ePL_DMR_UPnPState_Playing,
	eDMR_UPnPState_Stopped							= ePL_DMR_UPnPState_Stopped,
	eDMR_UPnPState_Paused							= ePL_DMR_UPnPState_Paused,
	eDMR_UPnPState_Transitioning					= ePL_DMR_UPnPState_Transitioning,
	eDMR_UPnPState_CurrentDuration					= ePL_DMR_UPnPState_CurrentDuration,	// value is long
	eDMR_UPnPState_CurrentTrackDuration				= ePL_DMR_UPnPState_CurrentTrackDuration,	// value is long
	eDMR_UPnPState_Volume							= ePL_DMR_UPnPState_Volume,	// value is unsigned short 0 ~ 100
	eDMR_UPnPState_Mute								= ePL_DMR_UPnPState_Mute,	// value is int 0 or 1
	eDMR_UPnPState_CurrentDuration_NotInvoke		= ePL_DMR_UPnPState_CurrentDuration_NotInvoke,
} DMR_UPnPState_e;

typedef struct {
	HCHAR	szManufacturer[ PL_DMR_NAME_SHORT ];
	HCHAR	szManufacturerUrl[ PL_DMR_URL_MAX ];
	HCHAR	szModelName[ PL_DMR_NAME_MIDDLE ];
	HCHAR	szModelDescription[ PL_DMR_NAME_MAX ];
	HCHAR	szModelNumber[ PL_DMR_NAME_SHORT ];
	HCHAR	szModelUrl[ PL_DMR_URL_MAX ];
} DMR_ProductInfo_t;


typedef void (*DMR_UPnPActionCallback_t)(HINT32 param1, HINT32 param2, HINT32 param3); /* DMR_UPnPAction_e */
typedef HINT32 (*DMR_EVT_HANDLE_Func)( DMR_StatusEvent_e eEvt, HINT32 p1, HINT32 p2, HINT32 p3 );


#define ________________Public_Functions___________________________

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
void DMR_Initialize(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR 	DMR_ModuleInit(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/
HERROR 	DMR_ModuleTerm(void);


/* *
 * @brief 
 *
 * @param cb  
 *
 * @return 
 **/ 
HERROR	DMR_RegisterUPnPActionCallback(DMR_UPnPActionCallback_t cb);


/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	DMR_UnregisterUPnPActionCallback(void);


/* *
 * @brief 
 *
 * @param pstProductInfo  
 *
 * @return 
 **/ 
HERROR	DMR_Start( HCHAR *pszFriendlyName );

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR 	DMR_Stop(void);


/* *
 * @brief 
 *
 * @param pDmrName  
 *
 * @return 
 **/ 
HERROR 	DMR_SetName(HCHAR *pDmrName);

/* *
 * @brief 
 *
 * @param eState  
 * @param pValue  
 *
 * @return 
 **/ 
HERROR 	DMR_SetUPnPStatus(DMR_UPnPState_e eState, void *pValue);
HERROR	DMR_GetUPnPStatus(DMR_UPnPState_e *eState);

/* *
 * @brief 
 *
 * @param pstNetworkInfo  
 *
 * @return 
 **/
HERROR 	DMR_SetNetworkInfo(CO_NET_Iface_t * pstNetworkInfo);


/* *
 * @brief 
 *
 * @param ullTime  
 *
 * @return 
 **/ 
HERROR	DMR_SetMediaDuration(HUINT64 ullTime);

/* *
 * @brief 
 *
 * @param ullTime  
 *
 * @return 
 **/ 
HERROR	DMR_SetMediaPlayingPosition(HUINT64 ullTime);

/* *
 * @brief 
 *
 * @param pItem  
 *
 * @return 
 **/ 
HERROR	DMR_GetCurrentMedia(CO_MediaInstence_t **pItem);

HBOOL	DMR_IsStart(void);

/* Fling용 URI 및 MetaData를 Set 할수 있는 API */
HERROR	DMR_SetFlingInfomation( HCHAR *pUri, HCHAR *pCurrentMeta );

/* 현재 설정된 Fling용 URI를 확인하기 위한 함수. 호출부에서 메모리 해제 해야 함 !!!! */
HCHAR* 	DMR_GetFlingURI( void );

/* 현재 설정된 Fling용 Meta를 확인하기 위한 함수. 호출부에서 메모리 해제 해야 함 !!!! */
HCHAR* 	DMR_GetFlingMeta( void );

HERROR 	DMR_OnSystemEvent( HOMMA_BUS_Event_e eEvent, void *eDataType, void *pvData  );
void 	DMR_SetEvtListener( DMR_EVT_HANDLE_Func cbNotify );
#endif	/* __DMR_H__ */
/* end od file */

