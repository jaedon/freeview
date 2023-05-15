/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_port_db.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PORT_DB_H__
#define __NX_PORT_DB_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/

#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <nx_port_type.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
// TODO: Web-App Corsair 쪽에 BPLDB.h 에 있는 값을 사용한다. 없는 것만 추가해서 사용한다.

// Base
#define	USERDB_SUBTITLE_DISPLAY						"UDB/SUBTITLE_Display"
#define	USERDB_SUBTITLE_FONT						"UDB/SUBTITLE_Font"
#define	USERDB_SCREENDISP_INFO_DISP_TIME			"UDB/SCREENDISPLAY_InfoDisplayTime"
#define	USERDB_SYSTEM_STANDBY_POWER_MODE			"UDB/SYSTEM_StandByPowerMode"
#define	USERDB_SYSTEM_AUTOMATIC_POWER_DOWN			"UDB/SYSTEM_AutomaticPowerDown"
#define	USERDB_SYSTEM_STANDBY_MODE					"UDB/SYSTEM_Standby_Mode"
#define	USERDB_SYSTEM_B_FIRSTTIME_BOOT				"UDB/SYSTEM_bFirstTimeBoot"
#define	USERDB_SYSTEM_POWER_STATE					"UDB/SYSTEM_POWER_STATE"
#define	USERDB_SVC_LAST_CHANNEL						"UDB/SVC_LAST_CHANNEL"
#define	USERDB_FAVOURITE1_NAME_ID					"UDB/FAVOURITE1_NAME"
#define	USERDB_FAVOURITE2_NAME_ID					"UDB/FAVOURITE2_NAME"
#define	USERDB_FAVOURITE3_NAME_ID					"UDB/FAVOURITE3_NAME"
#define	USERDB_FAVOURITE4_NAME_ID					"UDB/FAVOURITE4_NAME"
#define	USERDB_FAVOURITE5_NAME_ID					"UDB/FAVOURITE5_NAME"
#define	USERDB_FAVOURITE1_NAME_UPDATE_FLAG_ID		"UDB/FAVOURITE1_UPDATE_FLAG"
#define	USERDB_FAVOURITE2_NAME_UPDATE_FLAG_ID		"UDB/FAVOURITE2_UPDATE_FLAG"
#define	USERDB_FAVOURITE3_NAME_UPDATE_FLAG_ID		"UDB/FAVOURITE3_UPDATE_FLAG"
#define	USERDB_FAVOURITE4_NAME_UPDATE_FLAG_ID		"UDB/FAVOURITE4_UPDATE_FLAG"
#define	USERDB_FAVOURITE5_NAME_UPDATE_FLAG_ID		"UDB/FAVOURITE5_UPDATE_FLAG"


#define	USERDB_SEARCH_OPT_NETWORK_SEARCH			"UDB/SEARCH_OPT_NETWORK_SEARCH"
#define	USERDB_SEARCH_OPT_SCRAMBLE_TYPE				"UDB/SEARCH_OPT_SCRAMBLE_TYPE"
#define	USERDB_SEARCH_OPT_CHANNEL_TYPE				"UDB/SEARCH_OPT_CHANNEL_TYPE"

// Base - Satellite
#define	USERDB_FIRST_ANTENNA_ID						"UDB/FIRST_ANTENNAID"
#define	USERDB_SECOND_ANTENNA_ID					"UDB/SECOND_ANTENNAID"
#define	USERDB_THIRD_ANTENNA_ID						"UDB/THIRD_ANTENNAID"
#define	USERDB_FOURTH_ANTENNA_ID					"UDB/FOURTH_ANTENNAID"

#define	USERDB_FIFTH_ANTENNA_ID						"UDB/FIFTH_ANTENNAID"
#define	USERDB_SIXTH_ANTENNA_ID						"UDB/SIXTH_ANTENNAID"
#define	USERDB_SEVENTH_ANTENNA_ID					"UDB/SEVENTH_ANTENNAID"
#define	USERDB_EIGHTH_ANTENNA_ID					"UDB/EIGHTH_ANTENNAID"

#define	USERDB_ANTSET_ANTENNATYPE_ID				"UDB/ANTSET_ANTENNATYPE_ID"

#define	USERDB_ANTSET_ANTINFO_SLOT0					"UDB/ANTSET_ANTINFO_SLOT0"
#define	USERDB_ANTSET_ANTINFO_SLOT1					"UDB/ANTSET_ANTINFO_SLOT1"
#define	USERDB_ANTSET_ANTINFO_SLOT2					"UDB/ANTSET_ANTINFO_SLOT2"
#define	USERDB_ANTSET_ANTINFO_SLOT3					"UDB/ANTSET_ANTINFO_SLOT3"
#define	USERDB_ANTSET_ANTINFO_SLOT4					"UDB/ANTSET_ANTINFO_SLOT4"
#define	USERDB_ANTSET_ANTINFO_SLOT5					"UDB/ANTSET_ANTINFO_SLOT5"
#define	USERDB_ANTSET_ANTINFO_SLOT6					"UDB/ANTSET_ANTINFO_SLOT6"
#define	USERDB_ANTSET_ANTINFO_SLOT7					"UDB/ANTSET_ANTINFO_SLOT7"

#define	USERDB_ANTSET_TP_SLOT0						"UDB/ANTSET_TP_SLOT0"
#define	USERDB_ANTSET_TP_SLOT1						"UDB/ANTSET_TP_SLOT1"
#define	USERDB_ANTSET_TP_SLOT2						"UDB/ANTSET_TP_SLOT2"
#define	USERDB_ANTSET_TP_SLOT3						"UDB/ANTSET_TP_SLOT3"
#define	USERDB_ANTSET_TP_SLOT4						"UDB/ANTSET_TP_SLOT4"
#define	USERDB_ANTSET_TP_SLOT5						"UDB/ANTSET_TP_SLOT5"
#define	USERDB_ANTSET_TP_SLOT6						"UDB/ANTSET_TP_SLOT6"
#define	USERDB_ANTSET_TP_SLOT7						"UDB/ANTSET_TP_SLOT7"
#define	USERDB_ANTSET_TP_TEST						"UDB/ANTSET_TP_SLOT_TEST"

// Base - SES OTA
#define	USERDB_NO_OTA_ACTION						"UDB/No_OTA_Action"
#define	USERDB_NO_OTA_SWVER							"UDB/No_OTA_SwVersion"
#define	USERDB_LATER_OTA_COUNT						"UDB/Later_OTA_Count"


// PVR
#define	USERDB_RECORDING_INSTANT_REPLAY_TIME		"UDB/RECORDING_InstantReplayTime"
#define	USERDB_RECORDING_SKIP_FORWARD_TIME			"UDB/RECORDING_SkipForwardTime"
#define	USERDB_RECORDING_TSR_ONOFF					"UDB/RECORDING_TsrOnOff"
#define	USERDB_RECORDING_DESCRAMBLE_ONOFF			"UDB/RECORDING_DescrambleOnOff"
#define	USERDB_RECORDING_AUTODELETE_ONOFF			"UDB/RECORDING_AutoDeleteOnOff"
#define	USERDB_RECORDING_TARGET_STORAGE				"UDB/RECORDING_TargetStorage"
#define	USERDB_SERIES_PLAYBACK						"UDB/SERIES_PLAYBACK"

// Media
#define	USERDB_VIDEO_REPEAT_OPTION					"UDB/VIDEO_REPEAT_OPTION"

// Network
#define	USERDB_DLNA_DMS_ACTIVATION					"UDB/DLNA_DMS_Activation"
#define	USERDB_DLNA_DMS_FRIENDLYNAME				"UDB/DLNA_DMS_FriendlyName"
#define	USERDB_DLNA_DMR_ACTIVATION					"UDB/DLNA_DMR_Activation"
#define	USERDB_DLNA_DMR_FRIENDLYNAME				"UDB/DLNA_DMR_FriendlyName"
#define	USERDB_DLNA_DIAL_ACTIVATION					"UDB/DLNA_DIAL_Activation"
#define	USERDB_DLNA_DIAL_FRIENDLYNAME				"UDB/DLNA_DIAL_FriendlyName"
#define	USERDB_DLNA_SATIP_ACTIVATION				"UDB/DLNA_SATIP_Activation"
#define	USERDB_DLNA_SATIP_FRIENDLYNAME				"UDB/DLNA_SATIP_FriendlyName"

#define	USERDB_AIRPLAY_ACTIVATION					"UDB/AIRPLAY_Activation"
#define	USERDB_FTP_LAUNCH							"UDB/FTP_Launching"
#define	USERDB_WOONSERVER_ACTIVATION				"UDB/WOONSERVER_Activation"
#define	USERDB_WOONSERVER_FRIENDLYNAME				"UDB/WOONSERVER_FriendlyName"

#define	USERDB_SAMBACLIENT_ACTIVATION				"UDB/SAMBACLIENT_Activation"
#define	USERDB_SAMBASERVER_ACTIVATION				"UDB/SAMBASERVER_Activation"
#define	USERDB_FTPSERVER_ACTIVATION					"UDB/FTPSERVER_Activation"


// UK SPECIFIC
#define	USERDB_AUDIO_AUDIO_DESCRIPTION				"UDB/AUDIO_AudioDescription"
#define	USERDB_SYSTEM_AUDIO_BLEEP					"UDB/Setting_Audio_Bleep"
#define	USERDB_HARD_OF_HEARING						"UDB/Hard_Of_Hearing"
#define	USERDB_POSTCODE								"UDB/Postcode"


// IP EPG
#define	USERDB_IPEPG_ONOFF							"UDB/IPEPG_OnOff"
#define	USERDB_RP_ONOFF								"UDB/RP_OnOff"
#define	USERDB_RP_PERIOD_START						"UDB/RP_PeriodStart"
#define	USERDB_RP_PERIOD_END						"UDB/RP_PeriodEnd"
#define	USERDB_RP_INTERVAL							"UDB/RP_Interval"
#define	USERDB_RP_REG_CODE							"UDB/RP_RegCode"
#define	USERDB_IPEPG_LASTUPDATETIME					"UDB/IPEPG_UpdateTime"	// IPEPG 에서만 set 다른 process는 참조만.
#define	USERDB_IPEPG_DOWNLOADPROGRESS				"UDB/IPEPG_DownProgress"	// IPEPG 에서만 set 다른 process는 참조만.
#define	USERDB_IPEPG_SVC_COUNT						"UDB/IPEPG_SVC_COUNT"	// IPEPG 에서만 set 다른 process는 참조만.
#define	USERDB_THUMBNAIL_ONOFF                      "UDB/THUMBNAIL_OnOff"


// TVTV EPG
#define	USERDB_TVTVEPG_LASTUPDATETIME				"UDB/TVTVEPG_UpdateTime"


// SAT RECORD
#define	USERDB_SATRECORD_INTERVAL					"UDB/SATRECORD_Interval"
#define	USERDB_SATRECORD_PRIORITY					"UDB/SATRECORD_Priority"


// SAT IP
#define	USERDB_SATIP_NOTIFICATION_ONOFF				"UDB/SATIP_Noti_OnOff"


// HBBTV
#define	USERDB_HBBTV_ENABLE							"UDB/HBBTV_Enable"


#define	USERDB_INSTALLTION_PINCOCE					"UDB/INSTALL_PINCOCE"


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
HERROR 	NX_PORT_DB_Remove(const HCHAR *pszKey);
HERROR	NX_PORT_DB_SetInt(const HCHAR *pszKey, HINT32 nValue);
HERROR	NX_PORT_DB_GetInt(const HCHAR *pszKey, HINT32 *pnValue);
HERROR	NX_PORT_DB_SetStr(const HCHAR *pszKey, const HCHAR *pszString);
HERROR	NX_PORT_DB_GetStr(const HCHAR *pszKey, HCHAR *pszString, HINT32 nMax);
/*
* APK_DB_SetInt_SYNC
* @Desc
* Install Wizard 종료후 APPKIT_UDB_MODULE 를 세팅을 DAPI_LazySet 으로 날리게 되면
* dama 에게 전달 1초 , dama 의 DB sync 2초 의 시간이 소비된다.
* 이 시간안에 A/c Power on off 를 하게 되면 APPKIT_UDB_MODULE 는 세팅되지 않는다..
* 아래와 같은 함수를Install Wizard  종료후인 USERDB_SYSTEM_B_FIRSTTIME_BOOT 호출에서 만 사용 하도록 함 ..
* SYNC 납용을 하면 dama 에 부담을 준다.
*/
HERROR	NX_PORT_DB_SetInt_SYNC(const HCHAR *pszKey, HINT32 nValue);

HERROR	NX_PORT_DB_Default(void);

#endif // __NX_PORT_DB_H__

