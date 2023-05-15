/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   \n
	@brief	   \n

	Description:   \n
	Module: \n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__SERVER_SECURE_HANDLER_H__
#define	__SERVER_SECURE_HANDLER_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <vkernel.h>
#include <secma_main.h>

#include "di_hdd.h"
#include "di_usb.h"
#include "di_usb_wireless.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HINT32 SECMA_HANDLE_DiInit(void);

extern HINT32 SECMA_HANDLE_Init(IpcSecMsgType eMsgType);
extern HINT32 SECMA_HANDLE_InitRegisterFunctions(IpcSecMsgType eMsgType);
extern HINT32 SECMA_HANDLE_FindMsgByType(IpcSecMsgType eMsgType, IpcSecManager_t *pstSecMgr);
extern HINT32 SECMA_HANDLE_FindMsgByTarget(HUINT8 *pucTarget, IpcSecManager_t *pstSecMgr);


#endif /* !__SERVER_SECURE_HANDLER_H__ */


