/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#ifndef	__SVC_LOGCAPTURE_H__
#define	__SVC_LOGCAPTURE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>

/** @brief local header file  */

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define CHANNEL_STATE_NO_SIGNAL		-1

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief Download event messages */
enum
{
	eSEVT_LOGCAPTURE = eSEVT_LOGCAPTURE_START,


	eSEVT_LOGCAPTURE_CHANNEL_INFO = (eSEVT_LOGCAPTURE_START+100),
	eSEVT_LOGCAPTURE_CHANNEL_STATE,
	eSEVT_LOGCAPTURE_CHANNEL_CHANGE,
	eSEVT_LOGCAPTURE_CAS_ERROR,
	eSEVT_LOGCAPTURE_LOGFILE_BACKUP,

	eSEVT_LOGCAPTURE_END  /* enum 끝 표시. 사용하는 메시지 아님. */
};

typedef enum
{
	eLOGCAPTURE_PVR_STATUS_PB_RECV_PAT = 0,
	eLOGCAPTURE_PVR_STATUS_PB_RECV_PMT,

} SvcLogcapture_PvrStatus_e;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR	SVC_LogCapture_Init (void);
HERROR	SVC_LogCapture_NotifyUsbInsertion (HUINT32 ulDeviceId);
HERROR	SVC_LogCapture_SetTunerGroupID (HUINT32 nTunerGroupId,HINT32 ulConstellation);
HERROR	SVC_LogCapture_SetChannelState (HINT32 nState);
HERROR	SVC_LogCapture_SetChannelChangeAction (void);
HERROR	SVC_LogCapture_LogFileBackup (void);
void	SVC_LogCapture_PrintInfoAll (void);

#endif
