/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <svc_logcapture.h>

#include <psvc_logcapture.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*
	OP : J:COM
	func_init :
	Error Case 및 SET 동작에 오류를 일으킬 만한 조건이 충족되면 "/var/lib/humaxt/" 위치에
	Log 를 남긴다.

*/
HERROR	SVC_LogCapture_Init(void)
{
	psvc_logcapture_InitProduct();

	return ERR_OK;
}


//AP 에서 USB Insertion Notify 받음
HERROR	SVC_LogCapture_NotifyUsbInsertion(HUINT32 ulDeviceId)
{
	HxLOG_Debug("#### SVC_LogCapture_NotifyUsbInsertion() ulDeviceId(%d)\r\n",ulDeviceId);

	return psvc_logcapture_NotifyUsbInsertion(ulDeviceId);
}

HERROR	SVC_LogCapture_SetTunerGroupID(HUINT32 ulTunerGroupId, HINT32 ulConstellation)
{
	HxLOG_Debug("#### SVC_LogCapture_SetTunerGroupID \r\n");

	return psvc_logcapture_SetTunerGroupID(ulTunerGroupId, ulConstellation);
}

HERROR	SVC_LogCapture_SetChannelState(HINT32 nState)
{
	HxLOG_Debug("#### SVC_LogCapture_SetChannelState \r\n");

	return psvc_logcapture_SetChannelState(nState);
}

HERROR	SVC_LogCapture_SetChannelChangeAction(void)
{
	return psvc_logcapture_SetChannelChangeAction();
}

HERROR	SVC_LogCapture_LogFileBackup(void)
{
	return psvc_logcapture_LogFileBackup();
}

void	SVC_LogCapture_PrintInfoAll()
{
	HxLOG_Debug("#### SVC_LogCapture_PrintInfoAll \r\n");
}

