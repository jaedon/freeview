/*************************************************************************************************************
	File            : pl_woons.h
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

#ifndef	__PL_WOON_SERVER_H__
#define	__PL_WOON_SERVER_H__

#define ________________Typedef____________________________________
typedef enum
{
	PL_MXRAS_EVENT_WID_QUERY,							// disable
	PL_MXRAS_EVENT_WID_REGISTER,						// disable
	PL_MXRAS_EVENT_WID_UNREGISTER,						// disable
	PL_MXRAS_EVENT_WID_GENERATE,						// disable

	PL_MXRAS_EVENT_NETWORK_REVERSE_CONNECTION, 			// enable
	PL_MXRAS_EVENT_NETWORK_EXPIRED_WID_OCCURED,			// disable
	PL_MXRAS_EVENT_NETWORK_CS_PUBLIC_IP,   				// disable // ConnectionServer  me Public IP
	PL_MXRAS_EVENT_NETWORK_LOCAL_IP,					// disable
	PL_MXRAS_EVENT_NETWORK_UPNP_PORT_FORWARDING,		// disable
	
	PL_MXRAS_EVENT_NETWORK_INFO_IDX,					// disable
	PL_MXRAS_EVENT_NETWORK_SETUP_IDX,					// disable
	PL_MXRAS_EVENT_NETWORK_REVERSE_IDX,					// disable

	PL_MXRAS_EVENT_WID_QUERY_PROGRASS,					// disable
	PL_MXRAS_EVENT_WID_REGISTER_PROGRASS,				// disable
	PL_MXRAS_EVENT_WID_UNREGISTER_PROGRASS,				// disable
	PL_MXRAS_EVENT_WID_GENERATE_PROGRASS,				// disable
	PL_MXRAS_EVENT_WID_LIST_IDX,						// disable
	PL_MXRAS_EVENT_WID_SET_DESCRIPTION_IDX,				// disable
	
	PL_MXRAS_EVENT_TOKEN,								// disable
	PL_MXRAS_EVENT_ACCOUNT_INFO,						// disable
	PL_MXRAS_EVENT_ACOUNT_IDX,							// disable
	PL_MXRAS_EVENT_FILES,								// disable
	PL_MXRAS_EVENT_METADATA,							// disable
	PL_MXRAS_EVENT_THUMBNAILS,							// disable
	PL_MXRAS_EVENT_FILEOPS_DELETE,						// disable
	PL_MXRAS_EVENT_FILEOPS_RENAME,						// disable
	PL_MXRAS_EVENT_FILEOPS_TRANSCODE_CHK,				// disable
	PL_MXRAS_EVENT_TRANSCODE,							// disable
	PL_MXRAS_EVENT_STREAMMING_SEND,
	PL_MXRAS_EVENT_SEARCH,								// disable
	PL_MXRAS_EVENT_GROUP,								// disable

	PL_MXRAS_EVENT_DEVICE_LIST,							// disable
	PL_MXRAS_EVENT_DEVICE_SET,							// disable
	PL_MXRAS_EVENT_DEVICE_RESET,						// disable
	PL_MXRAS_EVENT_DEVICE_MSEARCH,						// disable
	
	PL_MXRAS_EVENT_PAIRING_INIT,						// disable
	
	PL_MXRAS_EVENT_PROGRAM_VERSION,						// disable
	
	PL_MXRAS_EVENT_NOTIFICATION,						// disable

	PL_MXRAS_EVENT_DEVICE_ADD,							// disable
	PL_MXRAS_EVENT_DEVICE_REMOVE,						// disable
	PL_MXRAS_EVENT_SHARED_BROWSE,
	PL_MXRAS_EVENT_SHARED_SET,
	PL_MXRAS_EVENT_SHARED_RESET,
	PL_MXRAS_EVENT_SHARED_GET_LIST,

	PL_MXRAS_EVENT_PORT_FORWARDING,
	PL_MXRAS_EVENT_WID_SET_PASSWORD_IDX,

	PL_MXRAS_EVENT_WOONSTART_FAILED,					// private for only humax
	PL_MXRAS_EVENT_WOONSERVER_START,					// add by humax
	PL_MXRAS_EVENT_WOONSERVER_STOP,						// add by humax
	PL_MXRAS_EVENT_WOONSERVER_RENAME,						// add by humax
} PL_WOONS_EVENT_e;

typedef enum 
{
	PL_MXRAS_EVENT_OK,
	PL_MXRAS_EVENT_ERROR,
	PL_MXRAS_EVENT_ERROR_CONNECTION_FAIL,
} PL_WOONS_ERROR_CODE_e;

typedef enum
{
	PL_MXRAS_NOTIFICATION_NETWORK_INFO 			= 0,
	PL_MXRAS_NOTIFICATION_NETWORK_REVERSE		= 1,
	PL_MXRAS_NOTIFICATION_DEVICE_LIST,
	PL_MXRAS_NOTIFICATION_DEVICE_SET,
	PL_MXRAS_NOTIFICATION_DEVICE_RESET,
	PL_MXRAS_NOTIFICATION_DEVICE_MESEARCH,
	PL_MXRAS_NOTIFICATION_CODE_PAIRING 			= 1,
} PL_WOONS_NOTIFY_e;

typedef enum
{
	PL_WOONS_IP_MAX_LEN			= 128,
	PL_WOONS_MAX_SRVNAME_LEN	= 64,
	PL_WOONS_PAIRINGID_LEN		= 52,
	PL_WOON_PINCODE_LEN 		= 4,
	PL_WOON_ETH0				= 0,
} PL_WOONS_MAX_e;

typedef struct
{
	void*			hDms;
	void*			hDmsDB;
	void*			hFs;
	HCHAR 			szServerName[PL_WOONS_MAX_SRVNAME_LEN];
	HCHAR 			szPairingId[PL_WOONS_PAIRINGID_LEN];
} PL_WOONS_DmsInfo_t;

typedef struct
{
	HINT32	nState;
	HCHAR 	szLocalIp[PL_WOONS_IP_MAX_LEN];	
	HCHAR 	szExtIp[PL_WOONS_IP_MAX_LEN];		
	HCHAR 	szPubIp[PL_WOONS_IP_MAX_LEN];
	HINT32 	nPubPort; 
	HINT32 	nExtPort;
} PL_WOONS_INFO_t;

typedef void 	(*PL_WOONS_EVENT_CALLBACK)		(PL_WOONS_EVENT_e type, PL_WOONS_ERROR_CODE_e error, void* data);
typedef void 	(*PL_WOONS_COLLECTOR_CALLBACK)	(PL_WOONS_NOTIFY_e notification_code);

#define ________________Public_Interfaces__________________________

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
void	 PL_WOONS_Init(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
void 	PL_WOONS_Deinit(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HBOOL 	PL_WOONS_IsStarted(void);

/* *
 * @brief 
 *
 * @param pstDmsInfo  
 *
 * @return 
 **/ 
HERROR 	PL_WOONS_Start(PL_WOONS_DmsInfo_t	*pstDmsInfo);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR 	PL_WOONS_Stop(void);

/* *
 * @brief 
 *
 * @param listener  
 *
 * @return 
 **/ 
void 	PL_WOONS_AddEventCallback(PL_WOONS_EVENT_CALLBACK listener);

/* *
 * @brief 
 *
 * @param listener  
 *
 * @return 
 **/ 
void 	PL_WOONS_AddCollectorCallback(PL_WOONS_COLLECTOR_CALLBACK listener);

/* *
 * @brief 
 *
 * @param pszServerName  
 *
 * @return 
 **/ 
HERROR 	PL_WOONS_RenameServer(HCHAR *pszServerName);

/*-----------------------------------------------------------------------------
 *   Debug Function
 *-----------------------------------------------------------------------------*/

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HBOOL PL_WOONS_InitPairingId(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR *PL_WOONS_GetPairingId(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR *PL_WOONS_GetPinCode(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR PL_WOONS_ShowNetworkInfo(void);

/* *
 * @brief Reverse Connection Test -  Reverse connection test를 요청한다
 * 			
 *
 * @param void  
 *
 * @return Async return - through eventlistener
 **/ 
void PL_WOONS_TestConnection(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR* PL_WOONS_GetLocalIp(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR* PL_WOONS_GetExternalIp(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR* PL_WOONS_GetPublicIp(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/
HINT16 PL_WOONS_GetExternalPort(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HINT16 PL_WOONS_GetPublicPort(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
PL_WOONS_INFO_t* PL_WOONS_GetServerInfo(void);

/* *
 * @brief Add Woon ID - Connection 서버에 WID를 등록 및 저장 한다.
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR* PL_WOONS_GetWoonId(void);

/* *
 * @brief 
 *
 * @param pszWoonID  
 *
 * @return 
 **/ 
HERROR PL_WOONS_AddWoonID( HCHAR *pszWoonID, HCHAR *pszPassword );

/* *
 * @brief Delete Woon ID - WID 삭제
 *
 * @param pWoonID  
 *
 * @return 
 **/
HERROR PL_WOONS_DelWoonID(HCHAR *pWoonID);

/* *
 * @brief PrHINT32 DMS List
 *
 * @param void  
 *
 * @return 
 **/
void PL_WOONS_GetDeviceList(void);

/* *
 * @brief Get Woon ID List - WoonID 리스트 확인 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR PL_WOONS_GetWoonIDList(void);
#endif
/* end od file */

