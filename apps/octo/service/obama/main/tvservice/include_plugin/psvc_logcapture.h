/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__LOGCAPTURE_PORT_H__
#define	__LOGCAPTURE_PORT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



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
HERROR	psvc_logcapture_InitProduct (void);

HERROR	psvc_logcapture_NotifyUsbInsertion (HUINT32 ulDeviceId);
HERROR	psvc_logcapture_SetTunerGroupID (HUINT32 ulTunerGroupId, HINT32 ulConstellation);
HERROR	psvc_logcapture_SetChannelState (HINT32 nState);
HERROR	psvc_logcapture_SetChannelChangeAction (void);
HERROR	psvc_logcapture_LogFileBackup (void);


#endif /* !__LOGCAPTURE_PORT_H__ */
