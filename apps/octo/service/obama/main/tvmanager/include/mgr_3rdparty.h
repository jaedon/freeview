/*******************************************************************************
*
* File Name    : mgr_3rdparty.h
*
* Description  : 3rd party library 관련
*
*
* Copyright (C) 2012, Humax Co., Ltd. All rights reserved.
*
* Revision History :
*
* Date        Modification                                               Name
* ----        ------------                                             --------
*
*******************************************************************************/

#ifndef	__MGR_3RDPARTY_PORT_H__
#define	__MGR_3RDPARTY_PORT_H__

#include <octo_common.h>
#include <bus.h>

enum
{
	eMEVT_3RD		= eMEVT_3RD_START,

	// for MHEG
	eMEVT_3RD_MHEG_HEAD,

	/* - Description : OBAMA에서 MHEG을 위한 SI monitoring을 시작했다고 MHEG에 알리는 메시지
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_MONITORING_STARTED,

	/* - Description : OBAMA에서 MHEG을 위한 SI monitoring을 정지했다고 MHEG에 알리는 메시지
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_MONITORING_STOPPED,

	/* - Description : CHANNEL CHANGE 메시지를 MHEG에 알리는 메시지
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : Service Handle
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_SERVICE_STARTED,

	/* - Description : CHANNEL STOPPED 메시지를 MHEG에 알리는 메시지
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_SERVICE_STOPPED,

	/* - Description : PMT가 달라진 것을 MHEG에 알리는 메시지
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : Service Handle
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_PMT_CHANGED,

	/* - Description : Demux가 달라진 것을 MHEG에 알리는 메시지
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : DemuxID
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_DEMUX_CHANGED,


	eMEVT_3RD_MHEG_TAIL,
	eMEVT_3RD_END
};


/*	===================================================================================
 *	[octo_root]/main/plugin/3rdparty_port 에서 구현해야 할 함수...
 *	===================================================================================
 *	TV Manager Base에서 호출되는 함수는 여기에 정의하고
 *	Function Body는 3rdparty_port에 구현한다.
 *
 *	<octo_common.h>, <bus.h>이 외의 다른 Header를 Include 하지 않는다....
 *
 */

BUS_Callback_t	MGR_RFVOD_GetProc (void);
void			MGR_RFVOD_Init (void);

HERROR			MGR_BML_StartBrowserApp (void);
HERROR			MGR_BML_Start (void);
HERROR			MGR_BML_Stop (void);
HERROR			MGR_BML_Show (void);
HERROR			MGR_BML_Hide (void);
HERROR			MGR_BML_SetKeyControlMode (HBOOL bKeyControlMode);

// mgr_mheg.c
HERROR			MGR_MHEG_Init (void);

// mgr_mhegib.c
HERROR			MGR_MHEGIB_RequestFileOrData (HUINT16	 usSessionId,
											HUINT32		 nReqType,
											HUINT32		 ulReqNameLen,
											HUINT8		*paucReqName);

HERROR			MGR_MHEGIB_RequestAppAbort (HUINT16 usSessionId, HUINT32 nAbortReqCode);

#endif // __MGR_3RDPARTY_PORT_H__

