/*******************************************************************************
*
* File Name    : mgr_3rdparty.h
*
* Description  : 3rd party library ����
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

	/* - Description : OBAMA���� MHEG�� ���� SI monitoring�� �����ߴٰ� MHEG�� �˸��� �޽���
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_MONITORING_STARTED,

	/* - Description : OBAMA���� MHEG�� ���� SI monitoring�� �����ߴٰ� MHEG�� �˸��� �޽���
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_MONITORING_STOPPED,

	/* - Description : CHANNEL CHANGE �޽����� MHEG�� �˸��� �޽���
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : Service Handle
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_SERVICE_STARTED,

	/* - Description : CHANNEL STOPPED �޽����� MHEG�� �˸��� �޽���
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_SERVICE_STOPPED,

	/* - Description : PMT�� �޶��� ���� MHEG�� �˸��� �޽���
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : Service Handle
		   p2 : Related Live Action Id
		   p3 : N.A */
	eMEVT_3RD_MHEG_PMT_CHANGED,

	/* - Description : Demux�� �޶��� ���� MHEG�� �˸��� �޽���
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
 *	[octo_root]/main/plugin/3rdparty_port ���� �����ؾ� �� �Լ�...
 *	===================================================================================
 *	TV Manager Base���� ȣ��Ǵ� �Լ��� ���⿡ �����ϰ�
 *	Function Body�� 3rdparty_port�� �����Ѵ�.
 *
 *	<octo_common.h>, <bus.h>�� ���� �ٸ� Header�� Include ���� �ʴ´�....
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

