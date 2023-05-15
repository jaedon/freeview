
#ifndef __XSVC_LOGCAPTURE_INT_H__
#define __XSVC_LOGCAPTURE_INT_H__
/*******************************************************************************
* File Name    : _xsvc_logcapture.h
*
* Description  :
*
* Copyright (C) 2010, Humax Co., Ltd. All rights reserved.
*
* Revision History :
*
* Date        Modification                                               Name
* ----        ------------                                             --------
*
*******************************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <util.h>
#include <linkedlist.h>
#include <octo_common.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_logcapture.h>

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
/********************      Functions       *************************/
/*******************************************************************/
HERROR	xsvc_logcapture_Init_Base(void);
HERROR	xsvc_logcapture_Init_JcNgs(void);

HERROR	xsvc_logcapture_GetLogUSBInfo_Base(HUINT32 ulDeviceId);
HERROR	xsvc_logcapture_SetTunerGroupId_Base(HUINT32 ulTunerGroupId, HINT32 nConstellation);
HERROR	xsvc_logcapture_SetChannelState_Base(HINT32 nState);
HERROR	xsvc_logcapture_SetChannelChangeAction_Base(void);
HERROR	xsvc_logcapture_LogFileBackup_Base(void);

HERROR	xsvc_logcapture_NotifyUserDeviceInfo(HUINT8* paucUserAgent, HUINT8* paucContentsURL, HUINT8* paucIP);
void	xsvc_logcapture_GetVideoError(HUINT32 ulDecoderError, HUINT32 ulDisplayError);
void	xsvc_logcapture_SendMsgForNotifyVideoError(HUINT32 ulDecoderError, HUINT32 ulDisplayError);
void	xsvc_logcapture_SendMsgForNotifyAudioError(HUINT32 ulPtsCountError, HUINT32 ulFrameError);

extern void LogPrint(const char *pFormat, ...);


#endif	//_LOG_CAPTURE_H_
