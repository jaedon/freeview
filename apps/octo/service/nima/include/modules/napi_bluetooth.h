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

#include "htype.h"
#include "napi_def.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/

typedef void (*NAPI_BTCallback) (HINT32 event, void* data);
/*-----------------------------------------------------------------------------------------------
*	interfaces
*----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

HERROR	NAPI_BT_RegisterCallback(NAPI_BTCallback callback);
HERROR	NAPI_BT_SetEnable(HBOOL enable);
HERROR	NAPI_BT_GetEnable(HBOOL *enable);
HERROR	NAPI_BT_Scan(eNapiBTProfile profile);
HERROR	NAPI_BT_Connect(const HCHAR *bdaddr);
HERROR	NAPI_BT_Disconnect(const HCHAR *bdaddr);
HERROR	NAPI_BT_Remove(const HCHAR *bdaddr);
HERROR	NAPI_BT_SendPinCode(const HCHAR *bdaddr, HCHAR *pincode);
HUINT32 NAPI_BT_GetDeviceNum(void);
HERROR	NAPI_BT_GetDeviceInfo(HUINT32 index, NAPIBTDevInfo_t *info);
HERROR	NAPI_BT_GetHostMacAddrInfo(HCHAR *bdaddr);
HUINT32 NAPI_BT_GetHostMacAddrInfoSize(void);
HINT32 NAPI_BT_GetDevInfoList(HUINT32 dev_count, NAPIBTDevInfo_t *info);


#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_BLUETOOTH_H__ */

