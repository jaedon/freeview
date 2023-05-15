/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_hotplug.h
	@brief	  umma_hotplug 내에서 내부적으로 공유되는 선언 및 함수

	Description: 		\n

	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__INT_HOTPLUG_H__
#define	__INT_HOTPLUG_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define UMMA_HOTPLUG_EVT_START 		0
#define UMMA_HOTPLUG_CMD_START		0x100
#define UMMA_HOTPLUG_MSG_START		0x200

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eHOTPLUG_EVT_NONE = UMMA_HOTPLUG_EVT_START,
	eHOTPLUG_EVT_CONNECTED,
	eHOTPLUG_EVT_DISCONNECTED,
	eHOTPLUG_EVT_LAST
}UMMA_HOTPLUG_EVENT_e;

typedef enum
{
	eHOTPLUG_SIGNAL_NONE		= UMMA_HOTPLUG_CMD_START,
	eHOTPLUG_SIGNAL_INIT,

	eHOTPLUG_SIGNAL_APPREADY,
	eHOTPLUG_SIGNAL_SHUTDOWN,
	eHOTPLUG_SIGNAL_SHUTDOWNDONE,
	eHOTPLUG_SIGNAL_OPERATION,
	eHOTPLUG_SIGNAL_LAST
} UMMA_HOTPLUG_SIGNAL_e;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#endif /* !__INT_HOTPLUG_H__ */
