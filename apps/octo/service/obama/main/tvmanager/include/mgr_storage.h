/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_storage.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_STORAGE_H___
#define	___MGR_STORAGE_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <svc_fs.h>
#include <mgr_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	/* events comes from UMMA FS */
	eMEVT_STORAGE			= eMEVT_STORAGE_START,

	/* - Description :  Storage Plugged
	   - Parameters Usage :
		   handle : N.A.
		   p1 : eDevIdx
		   p2 : Is Internal : internal (p2 == 1), external (p2 == 0)
		   p3 : 0 */
	eMEVT_STORAGE_NOTIFY_PLUGGED,

	/* - Description :	Storage Unplugged
	   - Parameters Usage :
		   handle : N.A.
		   p1 : eDevIdx
		   p2 : Is Internal : internal (p2 == 1), external (p2 == 0)
		   p3 : ulPvrIdx */
	eMEVT_STORAGE_NOTIFY_UNPLUGGED,

	/* - Description :  Storage Plugged
	   - Parameters Usage :
		   handle : N.A.
		   p1 : eDevIdx
		   p2 : Is Internal : internal (p2 == 1), external (p2 == 0)
		   p3 : 0 */
	eMEVT_STORAGE_NOTIFY_FORMAT_PROCESS,

	/* - Description :  Storage Plugged
	   - Parameters Usage :
		   handle : N.A.
		   p1 : eDevIdx
		   p2 : Is Internal : internal (p2 == 1), external (p2 == 0)
		   p3 : 0 */
	eMEVT_STORAGE_NOTIFY_FORMAT_DONE,

	/* - Description :  Storage Plugged
	   - Parameters Usage :
		   handle : N.A.
		   p1 : eDevIdx
		   p2 : Is Internal : internal (p2 == 1), external (p2 == 0)
		   p3 : 0 */
	eMEVT_STORAGE_NOTIFY_FORMAT_FAIL,

	/* - Description :  PVR Storage is activated
	   - Parameters Usage :
		   handle : N.A.
		   p1 : eDevIdx
		   p2 : Is Internal : internal (p2 == 1), external (p2 == 0)
		   p3 : ulPvrIdx*/
	eMEVT_STORAGE_NOTIFY_PVR_ENABLED,

	/* - Description :  PVR Storage is activated
	   - Parameters Usage :
		   handle : N.A.
		   p1 : eDevIdx
		   p2 : Is Internal : internal (p2 == 1), external (p2 == 0)
		   p3 : ulPvrIdx */
	eMEVT_STORAGE_NOTIFY_PVR_DISABLED,


	/* - Description : storage device 연결 알림
	   - Parameters Usage :
		   handle : TBD
		   p1 : eDevIdx
		   p2 : bInternal
		   p3 : bPvr 	*/
	eMEVT_UMMA_NOTIFY_STORAGE_CONNECTED,

	/* - Description : storage device 제거 알림
	   - Parameters Usage :
		   handle : TBD
		   p1: eDevIdx
		   p2: ulPvrIdx
		   p3: SvcFs_DevInfo_t			*/
	eMEVT_UMMA_NOTIFY_STORAGE_DISCONNECTED,


	/* - Description : PVR On 알림
	   - Parameters Usage :
		   handle : TBD
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_UMMA_NOTIFY_STORAGE_PVR_ENABLED,

	/* - Description : PVR 기능 Off 알림
	   - Parameters Usage :
		   handle : TBD
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_UMMA_NOTIFY_STORAGE_PVR_DISABLED,


	/* - Description : PVR이 아니면서 recording list를 들고 있는 device On 알림
	   - Parameters Usage :
		   handle : TBD
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_UMMA_NOTIFY_STORAGE_RECORDING_ENABLED,

	/* - Description : PVR이 아니면서 recording list를 들고 있는 device Off 알림
	   - Parameters Usage :
		   handle : TBD
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_UMMA_NOTIFY_STORAGE_RECORDING_DISABLED,

	/* - Description : Storage Device를 Format 하라고 명령하는 메시지.
	   - Parameters Usage :
		   handle : NULL
		   p1 : eDevIdx
		   p2 : TBD
		   p3 : eDevIdx */
	eMEVT_STORAGE_FORMAT_DEVICE,

        eMEVT_META_RECLIST_HDD_SCANABLE,
	eMEVT_META_RECLIST_HDD_REMOVED,

} MgrStorage_Message_e;

typedef	enum
{
	ePS_State_Null,
	ePS_Waiting_Off,
	ePS_Off,
	ePS_Waiting_On,
	ePS_On
} MgrStorage_PowerState_e;

HERROR MGR_STORAGE_Start(void);

/*		Samba Client
 */
HERROR	MGR_SAMBACLIENT_Init (void);
HERROR	MGR_SAMBACLIENT_Deinit (void);
HERROR	MGR_SAMBACLIENT_ReqStartScan (void);
HERROR	MGR_SAMBACLIENT_ReqStopScan (void);
HERROR	MGR_SAMBACLIENT_Mount (void *pSharedFolderInfo);
HERROR	MGR_SAMBACLIENT_UnMount (void *pSharedFolderInfo);
HERROR	MGR_SAMBACLIENT_UnMountAll (void);

/*		mgr_storage_power.c
 */
BUS_Result_t MGR_STORAGE_IntHddSetPowerState(MgrStorage_PowerState_e ePowerState);
HERROR		MGR_STORAGE_IntHddPowerMonitorStart(void);

#endif /* !___MGR_STORAGE_H___ */

