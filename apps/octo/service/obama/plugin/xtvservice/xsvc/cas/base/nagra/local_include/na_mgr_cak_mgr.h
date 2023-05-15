/*
@file				na_mgr_cak_mgr.h
@brief				na_mgr_cak_mgr.h (Nagra CAK Service)

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef _CA_CAK_MGR_H
#define _CA_CAK_MGR_H


/********************************************************************
  Structure and Type Definition
 ********************************************************************/

typedef enum tagMetaTagType
{
	eTagCableCard 	= 0x00,
	eTagCopyCtlInfo = 0x01,
	eTagUriInfo		= 0x04,
	eMetaTagTyp_Max
} MetaTagType_t;

#define MAX_POPUP_MSG_SIZE	1024

typedef enum tagNaPopupType
{
	ePopup_Message,
	ePopup_Catastrophic,	// NAGRA file system에 문제가 있을 경우에 보내는 TYPE, MESSAGE 미포함, file system delete, STB reboot 동작
	ePopup_SWupgrade,
	ePopup_MessageRemove,
	ePopup_Cmd_Max
} NaPopupType_t;

typedef struct tagNaPopupUI
{
	NaPopupType_t 	eType;
	HUINT8			ucPersistence;
	HUINT32			ulPeriod;
	HUINT8			*pucMsg;	// NULL terminated by caller
} NaPopupUI_t;

typedef void (*TCaPopupCallback)(NaPopupUI_t *pstPopupUi);


/********************************************************************
  Extern Function Declaration
 ********************************************************************/
HERROR NAMGR_CAKMGR_SendPopupMessage(NaPopupUI_t *pstPopup);
HERROR NAMGR_CAKMGR_SendIrdCommand(HUINT8 ucIrdCmd);

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
HERROR NAMGR_CAKMGR_SetParentalControlLevel(void);
#endif

#if defined(CONFIG_DEBUG)
extern HERROR CA_NA_CakTestIrdCmd(HUINT32 nCmd);
#endif

#endif /* _CA_CAK_MGR_H */

