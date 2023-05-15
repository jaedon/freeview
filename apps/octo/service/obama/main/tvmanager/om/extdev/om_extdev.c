 /*******************************************************************
	 File Description
 ********************************************************************/
 /**
	 @file	   om_extdev.c
	 @brief	   external device control module (e.g. MIC, Joystick, keyboard, mouse...)

	 Description:									 \n
	 Module: TVMANAGER / OAPI Manager				 \n

	 Copyright (c) 2013 HUMAX Co., Ltd. 			 \n
	 All rights reserved.							 \n
 */

 /*
 * Character encoding.
 *
 * MS949
 *
 * This source file that uses MS949 encoding.
 * MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
 * MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
 * Quelldatei, die MS949-Codierung verwendet.
 *
 */
 /*
  * (c) 2011-2013 Humax Co., Ltd.
  * This program is produced by Humax Co., Ltd. ("Humax") and
  * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
  * non-assignable, non-transferable and non-exclusive license to use this Software.
  * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
  * you agree to the responsibility to take all reasonable efforts to protect the any information
  * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
  * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
  * If you have no authorized license, discontinue using this Software immediately.
  *
  * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
  * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
  * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
  * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

  * This License is effective until terminated. You may terminate this License at any time by destroying all copies
  * of the Software including all documentation. This License will terminate immediately without notice from Humax
  * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
  * of the Software and all documentation.

  * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
  * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
  * in the Republic of Korea.
 */

 /*******************************************************************/
 /********************		Header Files	*************************/
 /*******************************************************************/
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <db_param.h>

#if defined(CONFIG_PROD_DEVICE_MIC)
#include <svc_mic.h>
#endif
 /*******************************************************************/
 /********************		Defines 		*************************/
 /*******************************************************************/


 /*******************************************************************/
 /********************		Typedef 		*************************/
 /*******************************************************************/

STATIC HERROR om_extdev_NotifyChangedVolume(HINT32 nVolume)
{
 	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_EXTDEV_MIC_onVolumeChanged, "i", nVolume);
	}

	return hErr;
}

STATIC void om_extdev_SetMicVolumeToDb(HUINT32 ulVolume)
{
#if defined(CONFIG_PROD_DEVICE_MIC)
	HERROR		hErr;
	HINT32	nValue = (HINT32)ulVolume;
	HxLOG_Warning("!!!MIC SetVolume (nValue : %d)!!!\n", nValue);

	SVC_MIC_SetVolume(nValue);
	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_LAST_MIC_VOLUME, (HUINT32)nValue, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_SetItem(eDB_PARAM_ITEM_LAST_MIC_VOLUME) Error!!!\n");
	}
	om_extdev_NotifyChangedVolume(nValue);
#endif
}

STATIC HUINT32 om_extdev_GetMicVolumeFromDb(void)
{
	HUINT32		ulVolume = 0;
#if defined(CONFIG_PROD_DEVICE_MIC)
	HERROR		hErr;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LAST_MIC_VOLUME, (HUINT32 *)&ulVolume, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LAST_MIC_VOLUME) Error!!!\n");
	}
#endif
	return ulVolume;
}

STATIC HERROR om_extdev_MicStart(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
#if defined(CONFIG_PROD_DEVICE_MIC)
	HxLOG_Warning("!!!MIC START!!!\n");
	SVC_MIC_Start();

#endif
	return ERR_OK;
}



STATIC HERROR om_extdev_MicStop(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
#if defined(CONFIG_PROD_DEVICE_MIC)
	HxLOG_Warning("!!!MIC STOP!!!\n");
	SVC_MIC_Stop();

#endif

	return ERR_OK;

}



STATIC HERROR om_extdev_MicSetVolume(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32	nValue;
	HxLOG_Assert(argc == 1);

	nValue = (HUINT32)HxOBJECT_INT(argv[0]);

	om_extdev_SetMicVolumeToDb((HUINT32)nValue);
	return ERR_OK;
}

STATIC HERROR om_extdev_MicReqInit(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
#if defined(CONFIG_PROD_DEVICE_MIC)
	HINT32	nValue = 0;
	nValue = (HINT32)om_extdev_GetMicVolumeFromDb();

	om_extdev_NotifyChangedVolume(nValue);
	HxLOG_Warning("!!!MIC Initial Volume Request!!!\n");
#endif

	return ERR_OK;

}


STATIC void	 om_extdev_InitRpcFunctions (void)
{
 	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	 hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	 if(hErr == ERR_OK)
	 {
		 // REGISTER RPC Method & Callback
		 hErr = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_EXTDEV_MIC_START, NULL, (HxRPC_Func_t)om_extdev_MicStart, NULL , "MIC Start" );
		 hErr = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_EXTDEV_MIC_STOP, NULL, (HxRPC_Func_t)om_extdev_MicStop, NULL , "MIC Stop" );
		 hErr = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_EXTDEV_MIC_SETVOLUME, "i", (HxRPC_Func_t)om_extdev_MicSetVolume, NULL , "Set MIC Volume" );
		 hErr = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_EXTDEV_MIC_REQINIT, NULL, (HxRPC_Func_t)om_extdev_MicReqInit, NULL , "Req Initial MIC Volume" );
		 hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_EXTDEV_MIC_onVolumeChanged, "i",
					"Mic Volume : (HINT32 nVolume)"
				);


		 if(hErr != ERR_OK)
		 {
			HxLOG_Error("HLIB_RPC_RegisterCall() fail! %s \n", RPC_OBAMA_EXTDEV_MIC_onVolumeChanged);
		 }
	 }
	 else
	 {
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() fail! \n");
	 }

 }

STATIC BUS_Result_t	Proc_om_extdev(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eRet = MESSAGE_PASS;
	//HBOOL	bFinish = FALSE;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			om_extdev_InitRpcFunctions();
			break;

		default:
			break;
	}

	(void)p1;
	(void)p2;
	(void)p3;
	return eRet;
}

void	OM_EXTDEV_Init (void)
{
	HxLOG_Trace();

	BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, Proc_om_extdev, 0, 0, 0, 0);
}

