/*************************************************************************************************************
	File 		: woons.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/24
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Â¡Â°HumaxÂ¡Â±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED Â¡Â°AS ISÂ¡Â± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

#ifndef	__WOON_SERVER_H__
#define	__WOON_SERVER_H__

#include "pl_woons.h"

#define ________________Typedef____________________________________
typedef	enum {
	WOONSERVER,
	WOONSERVER_STARTED,
	WOONSERVER_STOPPED,
	WOONSERVER_RENAMED,
	WOONSERVER_TESTCONNECTION_RESULT,
	WOONSERVER_NOTIFY_PINCODE,

	WOONSERVER_EndOfEvent,
} WOONS_Event_e;

typedef enum
{
	NOTIFICATION_NETWORK_INFO 		= 0,
	NOTIFICATION_NETWORK_REVERSE	= 1,
	NOTIFICATION_DEVICE_LIST,
	NOTIFICATION_DEVICE_SET,
	NOTIFICATION_DEVICE_RESET,
	NOTIFICATION_DEVICE_MESEARCH,
	NOTIFICATION_CODE_PAIRING 		= 1,
} WOONS_NOTIFICATION_e;

typedef enum
{
	eWOONS_MAX_UDN_LEN 		= 256,
	eWOONS_MAX_WID_LEN 		= 125,
	eWOONS_MAX_SRVNAME_LEN	= 64,
	eWOONS_MAX_STATUS_LEN	= 256
} WOONS_Const;

typedef void 	(*WOONS_COLLECTOR_AP_CALLBACK) 	(WOONS_NOTIFICATION_e notification_code);
typedef void 	(*WOONS_EVENT_CALLBACK) 		(WOONS_Event_e	eEvt, HINT32 p1, HINT32 p2, HINT32 p3);

typedef struct
{
	char szUDN[eWOONS_MAX_UDN_LEN];
	char szWid[eWOONS_MAX_WID_LEN];
	char szServerName[eWOONS_MAX_SRVNAME_LEN];
	void *hDms;
	void *hDmsDb;
	void *hFs;
} WoonServer_ModelInfo_t;

#define WOON_SERVER_INFO_t	PL_WOONS_INFO_t

#define ________________Public_Interfaces__________________________
/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	WOONS_Init(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	WOONS_Deinit(void);


/* *
 * @brief 
 *
 * @param apEventListener  
 *
 * @return 
 **/
void 	WOONS_RegisterEventCallback(WOONS_EVENT_CALLBACK apEventListener);

/* *
 * @brief Start WOON Server
 *
 * @param szFriendlyName [IN] : Option to initilize Woon Server
 * 								if NULL, will set default value. or
 * 								it must be set dms, dmsdb handle. 
 *
 * @return if success, it will return eWOONS_Result_Ok.
 * 			or return eWOONS_Result_Error / eWOONS_Result_AlreadyStarted 
 **/
//HERROR	WOONS_Start(const char	*szFriendlyName);
HERROR 	WOONS_Start(const char *szFriendlyName, const char *szMac);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HBOOL	WOONS_IsStarted(void);

/* *
 * @brief Stop Woon Server
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	WOONS_Stop(void);

/* *
 * @brief Add Event Listener (To Receive "Async Event result")
 *
 * @param apEventListener [IN] : Add listener to callbac 
 *
 * @return 
 **/ 
void	 WOONS_AddEventCallback(WOONS_EVENT_CALLBACK apEventListener);

/* *
 * @brief 
 *
 * @param apCollectorListener  
 *
 * @return 
 **/
void 	WOONS_AddCollectorCallback(WOONS_COLLECTOR_AP_CALLBACK apCollectorListener);


/* *
 * @brief  Do Test Connection
 * 			- EVENT_AP_CALLBACK will be receive Test Result Event
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	WOONS_TestConnection(void);

/* *
 * @brief ¿ÜºÎ¿¡¼­ Á¢¼ÓÇÏ±â À§ÇÑ WOON ID ¾ò±â
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR* 	WOONS_GetWoonId(void);

/* *
 * @brief Get External / Internal IP Address,Port Number
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR* 	WOONS_GetLocalIp(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR*	WOONS_GetExternalIp(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HINT16	WOONS_GetExternalPort(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HCHAR*	WOONS_GetPublicIp(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HINT16 	WOONS_GetPublicPort(void);


/* *
 * @brief Rename Woon Server Name
 *
 * @param pszServerName  Server Name to rename 
 *
 * @return 
 **/ 
HERROR	WOONS_Rename(HCHAR *pszServerName);

/* *
 * @brief 
 *
 * @param pszWid  
 *
 * @return 
 **/ 
HERROR	WOONS_AddWoonId(HCHAR *pszWid);

/* *
 * @brief 
 *
 * @param pszWid  
 *
 * @return 
 **/ 
HERROR	WOONS_DelWoonId(HCHAR *pszWid);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
void	WOONS_ShowNetworkInfo(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
void	WOONS_ShowWidList(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
void	WOONS_GetDeviceList(void);

HERROR WOON_GetServerInfo(WOON_SERVER_INFO_t *pInfo);
#endif
/* end od file */

