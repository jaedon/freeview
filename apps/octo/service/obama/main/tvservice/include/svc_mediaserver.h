/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 	svc_mediaserver.h
	@brief		MediaServer Operaiont in Obama

	Description: 	defines common APIs for Mediaserver operations.\n
	Module: 		svc/mediaserver			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_MEDIASERVER_H__
#define	__SVC_MEDIASERVER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


void   SVC_MediaServer_RINGBUF_Signal(HxRingBuf_SignalType_e eType, Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus);
HERROR SVC_MediaServer_Watchdog_Add(Handle_t hRingBuf, DxSchedule_t *pstSchedule);
HERROR SVC_MediaServer_Watchdog_RemoveByRingbuf(Handle_t hRingBuf);
HERROR SVC_MediaServer_Watchdog_RemoveBySamaSlot(HINT32 nSamaSlot);
HERROR SVC_MediaServer_Watchdog_UpdateMsgIdByRingbuf(Handle_t hRingBuf, DxSchedule_t *pstSchedule);




#endif /* !__SVC_MEDIASERVER_H__ */
