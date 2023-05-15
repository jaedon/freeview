/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_appmgr.h
	@brief	  \n

	Description:	\n
	Module:	\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	_____MGR_APP_MGR_H_____
#define	_____MGR_APP_MGR_H_____

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eAppRegType_NONE					= 0,

	eAppRegType_OpSpec,
	eAppRegType_Country,
	eAppRegType_MenuLang,
	eAppRegType_AudioLang,
	eAppRegType_PinCode,

	eAppRegType_END
} OxMgrAppRegType_e;


/*******************************************************************/
/********************      Internal Functions Definition     ********************/
/*******************************************************************/
HERROR MGR_APP_GetRegistryValueByActionId (HUINT32 ulActionId, OxMgrAppRegType_e eRegType, void *pvData);


#endif /* _____MGR_APP_MGR_H_____ */

