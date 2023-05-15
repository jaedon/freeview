/*******************************************************************************
*
* File Name    : mgr_common.h
*
* Description  : 각종 core application 의 함수 Proto type 및 data type들을 정의한 헤더 파일
*
*
* Copyright (C) 2003, Humax Co., Ltd. All rights reserved.
*
* Revision History :
*
* Date        Modification                                               Name
* ----        ------------                                             --------
* 2009/03/18	createded											dymoon
*
*******************************************************************************/

#ifndef	__MGR_COMMON_H__
#define	__MGR_COMMON_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <math.h>
#include <octo_common.h>
#include <bus.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3)		(void)(hAction); (void)(p1); (void)(p2); (void)(p3)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	void	*list;
	HINT32	count;
} _datalist_t;

/*
	Manager 들의 Priority
*/
enum
{
	APP_ACTION_MGR_PRIORITY = 10,				// Normal Action Manager, Standby Action Manager, ColdBoot 만이 이 Priority를 가져야 한다.
	APP_PORTING_MGR_PRIORITY = 11,
												// 다른 Manager는 절대 이 Priority를 가져서는 안된다.

	APP_MONITORING_MGR_PRIORITY = 18,			// Normal/Batch Action Manager 이외에 상시 동작하면서 Monitoring 하는 Manager들은 특별한 이유가 없으면
												// 이 Priority를 사용하도록 한다.
	APP_HDD_PWR_MONITOR_PRIORITY,
	APP_PVR_MGR_PRIORITY,
	APP_BML_MGR_PRIORITY,
	APP_ACTION_PRIORITY,
	APP_UIMGR_PRIORITY,
	APP_MENUROOT_PRIORITY,
	APP_MHEG_MGR_PRIORITY,
	APP_RSV_MGR_PRIORITY,
	APP_CAS_PRIORITY,				// plugin
	APP_CAS_PPV_PRIORITY,			// plugin
	APP_TELETEXT_PRIOTITY,			// plugin
	APP_PVR_PRIORITY,
	APP_DEFAULT_PRIORITY = BUS_DEFAULT_PRIORITY,
	APP_DIALOGBOX_PRIORITY, 			// 각종 Dialog Box 표시를 위해 DEFAULT 보다 높은 Priority.
	APP_CAS_HIGH_PRIORITY,
	APP_LOGCAPTURE_PRIORITY,

	APP_SNMP_MIB_PRIORITY,			// plugin	// SNMP MIB는 App 동작 상태를 감시해야한다. 가장 높아야한다.
	// APP Priority 는 BUS_MAX_PRIORITY 보다 많을 수 없다.
	APP_MAX_PRIORITY = BUS_MAX_PRIORITY
};



#define eOEVT_METASVCLIST_START	(eOEVT_OM_START | 0x00001000)
#define eOEVT_LCNUPDATE_START	(eOEVT_OM_START | 0x00002000)
#define eOEVT_CAS_START			(eOEVT_OM_START | 0x00003000)
#define eOEVT_METARECLIST_START	(eOEVT_OM_START | 0x00004000)
#define eOEVT_OAPIMANAGER_END	(eOEVT_OM_START | 0x000FF000) // 30101~301FF


#define eMEVT_EMMUPDATE_START 		(eMEVT_MGR_START | 0x00001000)
#define eMEVT_COLDBOOT_START		(eMEVT_MGR_START | 0x00002000)
#define eMEVT_SWUP_START			(eMEVT_MGR_START | 0x00003000)
#define eMEVT_SETTING_START			(eMEVT_MGR_START | 0x00004000)
#define eMEVT_OUTPUT_START			(eMEVT_MGR_START | 0x00005000)
#define eMEVT_ACTION_START			(eMEVT_MGR_START | 0x00006000)
#define eMEVT_PG_START				(eMEVT_MGR_START | 0x00007000)
#define eMEVT_LIVE_START				(eMEVT_MGR_START | 0x00008000)
#define eMEVT_SEARCH_START			(eMEVT_MGR_START | 0x00009000)
#define eMEVT_RECORDER_START		(eMEVT_MGR_START | 0x0000A000)
#define eMEVT_PLAYBACK_START		(eMEVT_MGR_START | 0x0000B000)
#define eMEVT_MEDIA_START 			(eMEVT_MGR_START | 0x0000C000)
#define eMEVT_TIME_START				(eMEVT_MGR_START | 0x0000D000)
#define eMEVT_STORAGE_START			(eMEVT_MGR_START | 0x0000E000)
#define eMEVT_HOMECHANNEL_START	(eMEVT_MGR_START | 0x0000F000)
#define eMEVT_LOGCAPTURE_START		(eMEVT_MGR_START | 0x00010000)
#define eMEVT_SVCLIST_START			(eMEVT_MGR_START | 0x00011000)
#define eMEVT_EPG_START				(eMEVT_MGR_START | 0x00012000)
#define eMEVT_3RDPARTY_START		(eMEVT_MGR_START | 0x00013000)
#define eMEVT_NETWORK_START			(eMEVT_MGR_START | 0x00014000)
#define eMEVT_SCHEDULE_START		(eMEVT_MGR_START | 0x00015000)
#define eMEVT_SATIP_START 			(eMEVT_MGR_START | 0x00016000)
#define eMEVT_DRMAGENT_START		(eMEVT_MGR_START | 0x00017000)
#define eMEVT_MHEG_START			(eMEVT_MGR_START | 0x00018000)
#define eMEVT_CAS_START				(eMEVT_MGR_START | 0x00019000)
#define eMEVT_SATRECORD_START 		(eMEVT_MGR_START | 0x0001A000)
#define eMEVT_NTLS_START 			(eMEVT_MGR_START | 0x0001B000)
#define eMEVT_BATCH_START 			(eMEVT_MGR_START | 0x0001C000)
#define eMEVT_3RD_START 			(eMEVT_MGR_START | 0x0001D000)
#define eMEVT_RC_START 				(eMEVT_MGR_START | 0x0001E000)
#define eMEVT_PVRMGR_START 			(eMEVT_MGR_START | 0x0001F000)
#define eMEVT_HBBTV_START 			(eMEVT_MGR_START | 0x00020000)

#define eMEVT_MGR_END 				(eMEVT_MGR_START | 0x000FF000) // 10201~102FF




/**************************************
*	각 module 간 event를 정의..
*	mgr_action.h 의 eMEVT_ACTION_NOTIFY_RESULT 의 parameter 롤 사용하세요.
*	다른 용도로 사용하면 절대로 안 됩니다....
*/
typedef enum
{
	// sw update event
	eMGR_EVENT_SWUP_CONNECT_FINISH,
	eMGR_EVENT_SWUP_DETECT_FINISH,
	eMGR_EVENT_SWUP_DOWNLOAD_FINISH,
	eMGR_EVENT_SWUP_INSTALL_FINISH,
	eMGR_EVENT_SWUP_FINISH,
	eMGR_EVENT_SWUP_ERROR,

	eMGR_EVENT_SWUP_DETECT_SUCCESS,
	eMGR_EVENT_SWUP_DETECT_FAIL,
	eMGR_EVENT_SWUP_DOWNLOAD_SUCCESS,
	eMGR_EVENT_SWUP_DOWNLOAD_FAIL,
	eMGR_EVENT_SWUP_INSTALL_SUCCESS,
	eMGR_EVENT_SWUP_INSTALL_FAIL,

}Mgr_Event_e;


#if ((eMEVT_MGR_END) >= (eMEVT_XMGR_START))
#error "MSG_MGR_XXX Maxmum Value exceeded"
#endif




/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*
	New Cording Convention Functions
*/

#endif // __MGR_COMMON_H__

