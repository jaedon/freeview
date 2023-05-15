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
 * @file	  		nx_common.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef _NX_CORE_MESSAGE_H_
#define _NX_CORE_MESSAGE_H_


#include <ondk.h>


enum
{
	MSG_CORE_XXX	=	MSG_AP_CORE_START,

	/*
	Service 관련 message 류.
	*/
	MSG_CORE_SERVICE_PF_CHANGED_PROGRAMMES,
	MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES,
	MSG_CORE_SERVICE_EPG_JUMPTODATE_UPDATE,
	MSG_CORE_SERVICE_EVT_SIGNAL_STATE,
	MSG_CORE_SERVICE_EVT_ANTENNA_CONNECTION,
	MSG_CORE_SERVICE_EVT_DISEQCDETECTION,
	MSG_CORE_SERVICE_EVT_SEARCH_INFO_UPDATE,
	MSG_CORE_SERVICE_EVT_RELOAD_CHANNELLIST_ALL,

	MSG_CORE_CHANNEL_EVT_STATE_UPDATED,			// Channel Load Event

	/*
	System 정보 관련 message 휴.
	*/
	MSG_CORE_SWUP_NOTIFY,
	MSG_CORE_IMPORT_EXPORT_NOTIFY,

	/*
	Guide 관련 message 류.
	*/
	MSG_CORE_GUIDE_DONE_WATCH_LIVE_TV,			// detail 정보창에서 watch tv 가 눌린후 실행할 액션을 위해..

	/*
	Schedule 관련 message 류.
	*/
	MSG_CORE_SCHEDULE_NOTIFY,
	MSG_CORE_SCHEDULE_UPDATE,

	/*
	Storage 관련 message 류.
	*/
	MSG_CORE_STORAGE_EVT_NONE,
	MSG_CORE_STORAGE_DEVICE_CHANGE,
	MSG_CORE_STORAGE_NEED_FORMAT,
	MSG_CORE_STORAGE_FORMAT_PROGRESS,
	MSG_CORE_STORAGE_FORMAT_FAILED,
	MSG_CORE_STORAGE_FORMAT_DONE,
	MSG_CORE_STORAGE_DST_STATE,
	MSG_CORE_STORAGE_OVER_CURRENT,
	MSG_CORE_STORAGE_OVER_CURRENT_DONE,
	MSG_CORE_STORAGE_FAILED_UNMOUNT,
	MSG_CORE_STORAGE_STARTED_CHECKDISK,
	MSG_CORE_STORAGE_CHECK_USB_SPEED_DONE,
	MSG_CORE_STORAGE_PAIRING_FAILED,
	MSG_CORE_STORAGE_PAIRING_DONE,
	MSG_CORE_STORAGE_UNPAIRING_FAILED,
	MSG_CORE_STORAGE_UNPAIRING_DONE,
	MSG_CORE_STORAGE_SETNAME_FAILED,
	MSG_CORE_STORAGE_SETNAME_DONE,
	MSG_CORE_STORAGE_SAFE_REMOVE_FAILED,

	/*
	Media 관련 message 류.
	*/
	MSG_CORE_MEDIA_BROWSE_DONE,
	MSG_CORE_MEDIA_OPERATION_DONE,
	MSG_CORE_MEDIA_OPERATION_ERROR,
	/*
	player 관련 message 류.
	*/
	MSG_CORE_PLAYER_STOPPED_TSR_PLAYBACK,
	MSG_CORE_PLAYER_PLAY_STATE_CHANGED,
	MSG_CORE_PLAYER_PLAY_SPEED_CHANGED,
	MSG_CORE_PLAYER_PLAY_SUPPORTED_PLAY_SPEED_CHANGED,
	MSG_CORE_PLAYER_PLAY_ERROR,
	MSG_CORE_PLAYER_PLAY_BUFFER_STATE_CHANGED,
	MSG_CORE_PLAYER_COMPONENT_SUBTITLE_CHANGED,

	/*
	DLNA Message
	*/
#if	defined(CONFIG_MW_MM_DLNA)
	MSG_CORE_DLNA_DMS_CHANGE,
	MSG_CORE_DLNA_CONTENTS_BRWOSE_DONE,
#endif

    /*
    Contents Message
    */
#if defined(CONFIG_OP_SHAHID_APPS) // RES_ : CONTENTS -> APP
	MSG_CORE_CONTENTS_SHAHID_DATA_DB_UPDATED,
	MSG_CORE_CONTENTS_SHAHID_CAST_PLAY,
	MSG_CORE_CONTENTS_SHAHID_CAST_STOP,
#endif
	MSG_CORE_MAX
};


#endif
