/**
	@file     log_capture_main.c
	@brief    log_capture_main.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_logcapture.h>

#include "local_include/_xsvc_logcapture.h"

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

// LogCapture Main에서 호출하는 함수...
HERROR	psvc_logcapture_InitProduct(void)
{
	HERROR hErr;

	hErr = ERR_FAIL;

#if defined(CONFIG_MW_LOGCAPTURE)
	hErr = xsvc_logcapture_Init_JcNgs();
#else

#endif
	return hErr;
}

HERROR psvc_logcapture_NotifyUsbInsertion(HUINT32 ulDeviceId)
{
	return xsvc_logcapture_GetLogUSBInfo_Base(ulDeviceId);
}

HERROR psvc_logcapture_SetTunerGroupID(HUINT32 ulTunerGroupId, HINT32 ulConstellation)
{
	return xsvc_logcapture_SetTunerGroupId_Base(ulTunerGroupId, ulConstellation);
}

HERROR psvc_logcapture_SetChannelState(HINT32 nState)
{
	return xsvc_logcapture_SetChannelState_Base(nState);
}

HERROR psvc_logcapture_SetChannelChangeAction(void)
{
	return xsvc_logcapture_SetChannelChangeAction_Base();
}

HERROR psvc_logcapture_LogFileBackup(void)
{
	return xsvc_logcapture_LogFileBackup_Base();
}


