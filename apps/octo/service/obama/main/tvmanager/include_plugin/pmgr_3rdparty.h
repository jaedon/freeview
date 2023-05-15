/*******************************************************************************
*
* File Name    : ox_mgr_3rdparty.h
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

/*	===================================================================================
 *	[octo_root]/plugin/3rdparty_port ���� �����ؾ� �� �Լ�...
 *	===================================================================================
 *	TV Manager Base���� ȣ��Ǵ� �Լ��� ���⿡ �����ϰ�
 *	Function Body�� 3rdparty_port�� �����Ѵ�.
 *
 *	<octo_common.h>, <bus.h>�� ���� �ٸ� Header�� Include ���� �ʴ´�....
 *
 */

#if defined(CONFIG_3RD_RFVOD)
BUS_Callback_t	pmgr_rfvod_GetProc (void);
void			pmgr_rfvod_Init (void);
#endif

#if defined(CONFIG_3RD_BML)
HERROR			pmgr_bml_StartBrowserApp (void);
HERROR			pmgr_bml_Start (void);
HERROR			pmgr_bml_Stop (void);
HERROR			pmgr_bml_Show (void);
HERROR			pmgr_bml_Hide (void);
HERROR			pmgr_bml_SetKeyControlMode (HBOOL bmlKeyControlMode);
#endif

// pmgr_mheg.c
#if defined(CONFIG_3RD_MHEG)
HERROR			pmgr_mheg_Init (void);
#endif

// pmgr_mhegib.c
#if defined(CONFIG_SUPPORT_MHEG_IB)
HERROR			pmgr_mhegib_RequestFileOrData (HUINT16	 usSessionId,
											HUINT32		 nReqType,
											HUINT32		 ulReqNameLen,
											HUINT8		*paucReqName);
HERROR			pmgr_mheg_IbRequestAppAbort (HUINT16 usSessionId, HUINT32 nAbortReqCode);
#endif

#endif // __MGR_3RDPARTY_PORT_H__

