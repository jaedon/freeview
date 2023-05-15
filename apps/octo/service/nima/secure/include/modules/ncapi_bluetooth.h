/** **********************************************************************************************************
	File 		: napi_bluetooth.h
	author 		: 
	comment		: BlueTooth
	date    	: 2013/05/23
	attention 	: 
	
	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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
*********************************************************************************************************** */


#ifndef __NAPI_BLUETOOTH_H__
#define __NAPI_BLUETOOTH_H__

#include "ncapi.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
#define NCAPI_BT_NAME_MAX 				(248)
#define NCAPI_BT_BDADDR_LEN 				(6)
#define NCAPI_BT_CLASS_LEN 				(3)
#define NCAPI_BT_DEV_MAX 				(20)
#define NCAPI_BT_PINCODE_LEN			(16)

typedef enum {
	eNCAPI_BT_EVENT_INSERTED,
	eNCAPI_BT_EVENT_EXTRACTED,
	eNCAPI_BT_EVENT_ENABLE,
	eNCAPI_BT_EVENT_DISABLE,
	eNCAPI_BT_EVENT_SCAN_SUCCESS,
	eNCAPI_BT_EVENT_SCAN_FAIL,
	eNCAPI_BT_EVENT_AUTHEN_SUCCESS,
	eNCAPI_BT_EVENT_AUTHEN_FAIL,
	eNCAPI_BT_EVENT_PASSKEY,
	eNCAPI_BT_EVENT_CONNECT_SUCCESS,
	eNCAPI_BT_EVENT_CONNECT_FAIL,
	eNCAPI_BT_EVENT_REMOVED,
	eNCAPI_BT_EVENT_REMOTE_CMD,
	eNCAPI_BT_EVENT_MAX
} eNcapiBTEvent;

typedef enum {
	eNCAPI_BT_Profile_Unknown	= 0x00000000,
	eNCAPI_BT_Profile_A2DP		= 0x00040000, 	
	eNCAPI_BT_Profile_AVRCP		= 0x00080000,	
	eNCAPI_BT_Profile_HID		= 0x00100000,	
	eNCAPI_BT_Profile_All		= 0x1FFFFFFF,
	eNCAPI_BT_Profile_Auto		= 0x80000000,
} eNcapiBTProfile;

typedef enum {
	eNCAPI_BT_DEV_NOT_PAIRED			= 0,
	eNCAPI_BT_DEV_CONNECTED,
	eNCAPI_BT_DEV_DISCONNECTED
} eNcapiBTStatus;

typedef struct
{
	HCHAR				name[NCAPI_BT_NAME_MAX];
	HUINT8				bdaddr[NCAPI_BT_BDADDR_LEN];
	HUINT8				dev_class[NCAPI_BT_CLASS_LEN];
	eNcapiBTStatus		status;
	HUINT32				available_profile;
} __attribute__((packed)) NCAPI_BTDevInfo_t;

typedef struct
{
	HBOOL				enable;
	NCAPI_BTDevInfo_t	host_info;
	HINT32				passkey;
	HUINT8				connected_bdaddr[NCAPI_BT_BDADDR_LEN];
	HUINT32				count;
	NCAPI_BTDevInfo_t	dev_list[NCAPI_BT_DEV_MAX];
} __attribute__((packed)) NCAPI_BTInfo_t;

typedef void (*NCAPI_BTCallback) (eNcapiBTEvent event, NCAPI_BTInfo_t *data);

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern HERROR NCAPI_BT_Init(void);
extern HERROR NCAPI_BT_Deinit(void);
extern HERROR NCAPI_BT_RegisterCallback(NCAPI_BTCallback callback);
extern HERROR NCAPI_BT_UnregisterCallback(NCAPI_BTCallback callback);
extern HERROR NCAPI_BT_SetEnable(HBOOL enable);
extern HERROR NCAPI_BT_StartScan(eNcapiBTProfile profile);
extern HERROR NCAPI_BT_SendPinCode(HUINT8 *bdaddr, HUINT8 *pinCode);
extern HERROR NCAPI_BT_Connect(HUINT8 *bdaddr);
extern HERROR NCAPI_BT_Disconnect(HUINT8 *bdaddr);
extern HERROR NCAPI_BT_Remove(HUINT8 *bdaddr);
extern HERROR NCAPI_BT_OpenProfile(HUINT8 *bdaddr, eNcapiBTProfile profile);
extern HERROR NCAPI_BT_CloseProfile(HUINT8 *bdaddr, eNcapiBTProfile profile);
extern HERROR NCAPI_BT_GetHostInfo(NCAPI_BTDevInfo_t *info);
extern HERROR NCAPI_BT_GetDevList(NCAPI_BTInfo_t *info);
extern HERROR NCAPI_BT_Remove_DB(void);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_BLUETOOTH_H__ */

