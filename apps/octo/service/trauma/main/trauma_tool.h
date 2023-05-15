
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* �� 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#ifndef	__TRAUMA_TOOL__
#define	__TRAUMA_TOOL__
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define TRAUMA_VOLUME_STEP 				(20)		//prism scenario....
//	BPL - UserDB Key Values.
#define	USERDB_SUBTITLE_DISPLAY						"UDB/SUBTITLE_Display"
#define	USERDB_SUBTITLE_FONT						"UDB/SUBTITLE_Font"
#define	USERDB_SCREENDISP_INFO_DISP_TIME			"UDB/SCREENDISPLAY_InfoDisplayTime"
#define	USERDB_RECORDING_INSTANT_REPLAY_TIME		"UDB/RECORDING_InstantReplayTime"
#define	USERDB_RECORDING_SKIP_FORWARD_TIME			"UDB/RECORDING_SkipForwardTime"
#define	USERDB_RECORDING_TSR_ONOFF					"UDB/RECORDING_TsrOnOff"
#define	USERDB_RECORDING_DESCRAMBLE_ONOFF			"UDB/RECORDING_DescrambleOnOff"
#define	USERDB_RECORDING_AUTODELETE_ONOFF			"UDB/RECORDING_AutoDeleteOnOff"
#define	USERDB_SYSTEM_STANDBY_POWER_MODE			"UDB/SYSTEM_StandByPowerMode"
#define	USERDB_SYSTEM_AUTOMATIC_POWER_DOWN			"UDB/SYSTEM_AutomaticPowerDown"
#define	USERDB_SYSTEM_STANDBY_MODE					"UDB/SYSTEM_Standby_Mode"
#define	USERDB_SYSTEM_B_FIRSTTIME_BOOT				"UDB/SYSTEM_bFirstTimeBoot"
#define	USERDB_SVC_LAST_CHANNEL						"UDB/SVC_LAST_CHANNEL"
#define	USERDB_DLNA_DMS_ACTIVATION					"UDB/DLNA_DMS_Activation"
#define	USERDB_DLNA_DMS_FRIENDLYNAME				"UDB/DLNA_DMS_FriendlyName"
#define	USERDB_DLNA_DMR_ACTIVATION					"UDB/DLNA_DMR_Activation"
#define	USERDB_DLNA_DMR_FRIENDLYNAME				"UDB/DLNA_DMR_FriendlyName"
#define	USERDB_DLNA_DIAL_ACTIVATION					"UDB/DLNA_DIAL_Activation"
#define	USERDB_DLNA_DIAL_FRIENDLYNAME				"UDB/DLNA_DIAL_FriendlyName"
#define	USERDB_DLNA_SATIP_ACTIVATION				"UDB/DLNA_SATIP_Activation"
#define	USERDB_DLNA_SATIP_FRIENDLYNAME				"UDB/DLNA_SATIP_FriendlyName"

#define USERDB_AIRPLAY_ACTIVATION					"UDB/AIRPLAY_Activation"
#define	USERDB_FTP_LAUNCH							"UDB/FTP_Launching"
#define	USERDB_RECORDING_TARGET_STORAGE				"UDB/RECORDING_TargetStorage"
#define	USERDB_SERIES_PLAYBACK						"UDB/SERIES_PLAYBACK"
#define	USERDB_WOONSERVER_ACTIVATION				"UDB/WOONSERVER_Activation"
#define	USERDB_WOONSERVER_FRIENDLYNAME				"UDB/WOONSERVER_FriendlyName"

#define	USERDB_SAMBACLIENT_ACTIVATION				"UDB/SAMBACLIENT_Activation"
#define	USERDB_SAMBASERVER_ACTIVATION				"UDB/SAMBASERVER_Activation"
#define	USERDB_FTPSERVER_ACTIVATION					"UDB/FTPSERVER_Activation"

#define	USERDB_VIDEO_REPEAT_OPTION					"UDB/VIDEO_REPEAT_OPTION"

#define	USERDB_FIRST_ANTENNA_ID						"UDB/FIRST_ANTENNAID"
#define	USERDB_SECOND_ANTENNA_ID					"UDB/SECOND_ANTENNAID"
#define	USERDB_THIRD_ANTENNA_ID						"UDB/THIRD_ANTENNAID"
#define	USERDB_FOURTH_ANTENNA_ID					"UDB/FOURTH_ANTENNAID"

// UK SPECIFIC
#define	USERDB_AUDIO_AUDIO_DESCRIPTION				"UDB/AUDIO_AudioDescription"
#define	USERDB_SYSTEM_AUDIO_BLEEP					"UDB/Setting_Audio_Bleep"
#define	USERDB_HARD_OF_HEARING						"UDB/Hard_Of_Hearing"
#define	USERDB_POSTCODE								"UDB/Postcode"
#define	USERDB_TERMS_AND_CONDITIONS					"UDB/Terms_And_Conditions"


// IP EPG
#define	USERDB_IPEPG_ONOFF							"UDB/IPEPG_OnOff"
#define	USERDB_RP_ONOFF								"UDB/RP_OnOff"
#define	USERDB_RP_PERIOD_START						"UDB/RP_PeriodStart"
#define	USERDB_RP_PERIOD_END						"UDB/RP_PeriodEnd"
#define	USERDB_RP_INTERVAL							"UDB/RP_Interval"
#define	USERDB_RP_REG_CODE							"UDB/RP_RegCode"
#define	USERDB_IPEPG_LASTUPDATETIME					"UDB/IPEPG_UpdateTime"	// IPEPG ������ set �ٸ� process�� ������.
#define	USERDB_IPEPG_DOWNLOADPROGRESS				"UDB/IPEPG_DownProgress"	// IPEPG ������ set �ٸ� process�� ������.
#define USERDB_IPEPG_SVC_COUNT						"UDB/IPEPG_SVC_COUNT"   // IPEPG ������ set �ٸ� process�� ������.
#define USERDB_IPEPG_EVT_COUNT						"UDB/IPEPG_EVT_COUNT"   // IPEPG ������ set �ٸ� process�� ������.

#define USERDB_THUMBNAIL_ONOFF                      "UDB/THUMBNAIL_OnOff"

// TVTV EPG
#define	USERDB_TVTVEPG_LASTUPDATETIME				"UDB/TVTVEPG_UpdateTime"

// SAT RECORD
#define	USERDB_SATRECORD_INTERVAL					"UDB/SATRECORD_Interval"
#define	USERDB_SATRECORD_PRIORITY					"UDB/SATRECORD_Priority"

// SAT IP
#define	USERDB_SATIP_NOTIFICATION_ONOFF				"UDB/SATIP_Noti_OnOff"

// HBBTV
#define	USERDB_HBBTV_ENABLE							"UDB/HBBTV_Enable"

// SES OTA
#define	USERDB_NO_OTA_ACTION							"UDB/No_OTA_Action"
#define	USERDB_NO_OTA_SWVER							"UDB/No_OTA_SwVersion"
#define	USERDB_LATER_OTA_COUNT						"UDB/Later_OTA_Count"

// Jlabs Specific
//#define	USERDB_JLABS_SETUP_CONFIGURATION_LAUNCH_MODE	"JLABSUDB/SetupConfigurationLaunchMode"
#define	USERDB_JLABS_TUNING_RANGE						"JLABSUDB/TuningRange"
#define	USERDB_JLABS_TIMEBAR_DISP_MODE					"JLABSUDB/TimebarDispMode"
#define	USERDB_JLABS_I_PLATE_DISP_ENABLE					"JLABSUDB/IplateDispEnable"
#define 	USERDB_JLABS_DTT_LAST_CHANNEL					"JLABSUDB/DTT_LAST_CHANNEL"
#define 	USERDB_JLABS_BS_LAST_CHANNEL						"JLABSUDB/BS_LAST_CHANNEL"
#define 	USERDB_JLABS_CATV_LAST_CHANNEL					"JLABSUDB/CATV_LAST_CHANNEL"
#define 	USERDB_JLABS_CS1_LAST_CHANNEL					"JLABSUDB/CS1_LAST_CHANNEL"
#define 	USERDB_JLABS_CS2_LAST_CHANNEL					"JLABSUDB/CS2_LAST_CHANNEL"
#define 	USERDB_JLABS_JCHITS_LAST_CHANNEL					"JLABSUDB/JCHITS_LAST_CHANNEL"
#if		0		//	NOT USED --> WILL BE DELETED....
#define	USERDB_SUBTITLE_TYPE						"/UDB/SUBTITLE_Type"
#define	USERDB_TIMER_POWER_ON_FLAG					"/UDB/TIMER_PowerOnFlag"
#define	USERDB_TIMER_POWER_ON_HOUR					"/UDB/TIMER_PowerOnHour"
#define	USERDB_TIMER_POWER_ON_MIN					"/UDB/TIMER_PowerOnMin"
#define	USERDB_TIMER_POWER_ON_CH_NUM				"/UDB/TIMER_PowerOnChNum"
#define	USERDB_TIMER_POWER_ON_CH_NAME				"/UDB/TIMER_PowerOnChName"
#define	USERDB_TIMER_POWER_ON_VOLUME				"/UDB/TIMER_PowerOnVolume"
#define	USERDB_TIMER_POWER_ON_REPEAT				"/UDB/TIMER_PowerOnRepeat"
#define	USERDB_TIMER_POWER_OFF_FLAG					"/UDB/TIMER_PowerOffFlag"
#define	USERDB_TIMER_POWER_OFF_HOUR					"/UDB/TIMER_PowerOffHour"
#define	USERDB_TIMER_POWER_OFF_MIN					"/UDB/TIMER_PowerOffMin"
#define	USERDB_AUDIO_DIGITAL_AUDIO_SELECT			"/UDB/AUDIO_DigitalAudioSelect"
#define	USERDB_AUDIO_LIP_SYNC						"/UDB/AUDIO_LipSync"
#define	USERDB_SCREENDISP_TRANSPARENCY				"/UDB/SCREENDISPLAY_Transparency"
#define	USERDB_PARENTALCTRL_RATING					"/UDB/PARENTALCTRL_Rating"
#define	USERDB_PARENTALCTRL_PASSWORD				"/UDB/PARENTALCTRL_Password"
#define	USERDB_SVC_CHLIST_TYPE						"/UDB/SVC_CHLIST_TYPE"
#define	USERDB_SVC_FILTER							"/UDB/SVC_FILTER"
#define	USERDB_SVC_INDEX							"/UDB/SVC_INDEX"
#define	USERDB_HDMI_CEC								"/UDB/HDMICEC"
#endif

// OIPF Power Control Specific
#define	USERDB_OIPF_POWER_STATE				"UDB/OIPF_POWER_STATE"
#define	USERDB_OIPF_PREVIOUS_POWER_STATE	"UDB/OIPF_PREVIOUS_POWER_STATE"
#define	USERDB_OIPF_TIME_CUR_POWER_STATE	"UDB/OIPF_TIME_CUR_POWER_STATE"

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

#endif //__TRAUMA_TOOL__