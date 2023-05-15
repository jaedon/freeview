/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_xxx.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <bus.h>


#include "_om_xxx.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

//STATIC omXxxContext_t				*s_context = NULL;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___
STATIC BUS_Result_t proc_om_xxx (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC BUS_Result_t om_xxx_MsgBusCreate (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Add your code ...
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t om_xxx_MsgBusDestroy (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Add your code ...
	return MESSAGE_BREAK;
}


#define _____RPC_INITIALIZATION_____
STATIC HERROR om_xxx_InitRpcFunctions (void)
{
	HINT32			nRpcHandle = 0;
	HERROR			hErr = ERR_FAIL;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}
	//   ----------------------------- //
	//	-------   RPC Methods   ---------//
	//   ----------------------------- //
//	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_XXX_, NULL, (HxRPC_Func_t)om_xxx_type, NULL,	"Activate the xxx module: (void)");


	//   ----------------------------- //
	//	-------   Notifier  Callbakcs ------ //
	//   ----------------------------- //
//	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_XXX_, "ii",	"xxx Activated.\n \t   - (void)\n"	);


	return ERR_OK;
}

#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_xxx (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Debug("omBuilder : eMEVT_BUS_CREATE...\n");
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
		eResult = om_xxx_MsgBusCreate (hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Debug("omBuilder : eMEVT_BUS_DESTROY...\n");
		eResult = om_xxx_MsgBusDestroy (hHandle, p1, p2, p3);
		break;

	// Add your code ...

	default:
		eResult = MESSAGE_PASS;
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

// om_xxx member 간에만 사용해야 함
BUS_Result_t om_xxx_BaseProc (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return proc_om_xxx(message, hHandle, p1, p2, p3);
}

#define ___GLOBAL_FUNCTIONS___
HERROR OM_XXX_Init (void)
{
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_xxx, HANDLE_NULL, 0, 0, 0);

	om_xxx_InitRpcFunctions();

	return ERR_OK;
}



