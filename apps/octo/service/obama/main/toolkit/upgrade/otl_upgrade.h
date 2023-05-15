/*******************************************************************
	File Description
********************************************************************/
/**
	@file	otl_upgrade.h
	@brief	otl_upgrade.h

	Description:													\n
	Module:															\n

	Copyright (c) 2011 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

#ifndef	__OTL_UPGRADE_H__
#define	__OTL_UPGRADE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include "octo_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Defines         *******************/
/*******************************************************************/
#define UPGRADE_HANDLE_INVALID		HANDLE_NULL

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eUPGRADE_TARGET_Nvram	= 0,
	eUPGRADE_TARGET_File,
	eUPGRADE_TARGET_DB,
	eUPGRADE_TARGET_UserDefined,
} OtlUpgrade_Target_e;

typedef HERROR		(* OtlUpgrade_InitCb_t)(void);
typedef HERROR		(* OtlUpgrade_GetUserDataCb_t)(void **ppvUserData);
typedef HERROR		(* OtlUpgrade_SetUserDataCb_t)(void  *pvUserData);
typedef HBOOL		(* OtlUpgrade_CheckCompleteCb_t)(void *pvUserData);
typedef HERROR		(* OtlUpgrade_CreateHandleCb_t)(HUINT32 ulTag, void *pvUserData);
typedef HERROR		(* OtlUpgrade_DestoryHandleCb_t)(void *pvUserData);
typedef HINT32		(* OtlUpgrade_ReadCb_t)(void *pvUserData, HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize);
typedef HINT32		(* OtlUpgrade_WriteCb_t)(void *pvUserData, const HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize);

typedef struct tagUpgradeInfo
{
	OtlUpgrade_Target_e				 eTarget;
	HUINT8							 szFilePath[1024];		// if (eTarget == eUPGRADE_TARGET_File)
	void							*pvUserData;
	HUINT32							 ulDataSize;
	OtlUpgrade_InitCb_t				 fnInit;
	OtlUpgrade_GetUserDataCb_t		 fnGetUserData;
	OtlUpgrade_SetUserDataCb_t		 fnSetUserData;
	OtlUpgrade_CheckCompleteCb_t	 fnCheckComplete;
	OtlUpgrade_CreateHandleCb_t		 fnOpenTarget;
	OtlUpgrade_DestoryHandleCb_t	 fnCloseTarget;
	OtlUpgrade_ReadCb_t				 fnRead;
	OtlUpgrade_WriteCb_t			 fnWrite;
} OtlUpgrade_Info_t;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HERROR		OTL_UPGRADE_Init(void);

Handle_t	OTL_UPGRADE_Open(HUINT32 ulTag, OtlUpgrade_Info_t *pstInfo);
HERROR		OTL_UPGRADE_Close(Handle_t hUpgrade);

HERROR		OTL_UPGRADE_GetUserData(Handle_t hUpgrade, void **ppvUserData);
HERROR		OTL_UPGRADE_SetUserData(Handle_t hUpgrade, void *pvUserData);

HINT32		OTL_UPGRADE_Read(Handle_t hUpgrade, HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize);
HINT32		OTL_UPGRADE_Write(Handle_t hUpgrade, const HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize, HBOOL *pbComplete);
HBOOL		OTL_UPGRADE_CheckComplete(Handle_t hUpgrade);

#endif	// __OTL_UPGRADE_H__
/*********************** End of File ******************************/

