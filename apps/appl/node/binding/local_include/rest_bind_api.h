/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  rest_bind_api.h
	@brief

	Description:  									\n
	Module: node/binding/datatype		\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
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
#ifndef __REST_BIND_API_H__
#define __REST_BIND_API_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define REST_BIND_CHANNEL_TRANSFER_MAX		300
#define REST_BIND_TS_TRANSFER_MAX			50
#define REST_BIND_EPG_DURATION_MAX			(6 * 60 * 60)			// 6 hours

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef void (*RestBind_Notifier_t) (char *szEventType,
											char *szPayload);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
/*
	rest_bind.c
*/
extern int		REST_BIND_Init (void);

/*
	rest_bind_common.c
*/
// GET http://127.0.0.1:37564/stb-api/v1/time
extern char *	REST_BIND_COMMON_TIME_Get (char *szSessionId, char *szArgJson);


/*
	rest_bind_channel.c
*/
extern int		REST_BIND_CHAN_RegisterNotifier (RestBind_Notifier_t pfNotifier);
extern int		REST_BIND_CHAN_UnregisterNotifier (RestBind_Notifier_t pfNotifier);
// GET http://127.0.0.1/stb-api/v1/channels
extern char *	REST_BIND_CHAN_GetChannels (char *szSessionId, char *szArgJson);
// GET http://127.0.0.1/stb-api/v1/channels/1
extern char *	REST_BIND_CHAN_GetChannelOne (char *szSessionId, int nChannelUid, char *szArgJson);
// GET http://127.0.0.1/stb-api/v1/transponders
extern char *	REST_BIND_CHAN_GetTransponders (char *szSessionId, char *szArgJson);
// GET http://127.0.0.1/stb-api/v1/antennas
extern char *	REST_BIND_CHAN_GetAntennas (char *szSessionId, char *szArgJson);

/*
	rest_bind_player.c
*/
extern int		REST_BIND_PLAYER_RegisterNotifier (RestBind_Notifier_t pfNotifier);
extern int		REST_BIND_PLAYER_UnregisterNotifier (RestBind_Notifier_t pfNotifier);
extern char *	REST_BIND_PLAYER_Get (char *szSessionId, char *szArgJson);
extern char *	REST_BIND_PLAYER_GetOne (char *szSessionId, int nPlayerId, char *szArgJson);
extern char *	REST_BIND_PLAYER_PutOne (char *szSessionId, int nPlayerId, char *szJsPayload);

/*
	rest_bind_chanscan.c
*/
extern int		REST_BIND_SCAN_RegisterNotifier (RestBind_Notifier_t pfNotifier);
extern int		REST_BIND_SCAN_UnregisterNotifier (RestBind_Notifier_t pfNotifier);
// GET http://127.0.0.1:37564/stb-api/v1/channel-scanners
extern char *	REST_BIND_SCAN_Get (char *szSessionId, char *szArgJson);
// PUT http://127.0.0.1:37564/stb-api/v1/channel-scanners
extern char *	REST_BIND_SCAN_Put (char *szSessionId, char *szPayload);

/*
	rest_bind_epg.c
*/
extern int		REST_BIND_EPG_RegisterNotifier (RestBind_Notifier_t pfNotifier);
extern int		REST_BIND_EPG_UnregisterNotifier (RestBind_Notifier_t pfNotifier);
// http://127.0.0.1/stb-api/v1/epg
extern char *	REST_BIND_EPG_Get (char *szSessionId, char *szArgJson);

/*
	rest_bind_schedule.c
*/
extern int		REST_BIND_SCHEDULE_RegisterNotifier (RestBind_Notifier_t pfNotifier);
extern int		REST_BIND_SCHEDULE_UnregisterNotifier (RestBind_Notifier_t pfNotifier);
// POST http://127.0.0.1:37564/stb-api/v1/schedules
extern char *	REST_BIND_SCHEDULE_Post (char *szSessionId, char *szPayload);
// PUT http://127.0.0.1:37564/stb-api/v1/schedules/1
extern char *	REST_BIND_SCHEDULE_PutOne (char *szSessionId, int nScheduleUid, char *szPayload);
// GET http://127.0.0.1:37564/stb-api/v1/schedules
extern char *	REST_BIND_SCHEDULE_Get (char *szSessionId, char *szArgJson);
// GET http://127.0.0.1:37564/stb-api/v1/schedules/1
extern char *	REST_BIND_SCHEDULE_GetOne (char *szSessionId, int nScheduleUid, char *szArgJson);
// DELETE http://127.0.0.1:37564/stb-api/v1/schedules/1
extern char *	REST_BIND_SCHEDULE_DeleteOne (char *szSessionId, int nScheduleUid);

/*
	rest_bind_record.c
*/
extern int		REST_BIND_RECORD_RegisterNotifier (RestBind_Notifier_t pfNotifier);
extern int		REST_BIND_RECORD_UnregisterNotifier (RestBind_Notifier_t pfNotifier);
// POST http://127.0.0.1:37564/stb-api/v1/records
extern char *	REST_BIND_RECORD_Post (char *szSessionId, char *szPayload);
// GET http://127.0.0.1:37564/stb-api/v1/records
extern char *	REST_BIND_RECORD_Get (char *szSessionId, char *szArgJson);
// GET http://127.0.0.1:37564/stb-api/v1/records/1
extern char *	REST_BIND_RECORD_GetOne (char *szSessionId, int nRecordUid, char *szArgJson);
// DELETE http://127.0.0.1:37564/stb-api/v1/records/1
extern char *	REST_BIND_RECORD_DeleteOne (char *szSessionId, int nScheduleUid);

/*
	rest_bind_sysenv.c
*/
extern int		REST_BIND_SYSENV_RegisterNotifier (RestBind_Notifier_t pfNotifier);
extern int		REST_BIND_SYSENV_UnregisterNotifier (RestBind_Notifier_t pfNotifier);
extern char *	REST_BIND_SYSENV_Get (char *szSessionId, char *szArgJson);
extern char *	REST_BIND_SYSENV_Put (char *szSessionId, char *szPayload);

/*
	rest_bind_settings.c
*/
extern int		REST_BIND_SETTINGS_RegisterNotifier (RestBind_Notifier_t pfNotifier);
extern int		REST_BIND_SETTINGS_UnregisterNotifier (RestBind_Notifier_t pfNotifier);
// GET http://127.0.0.1:37564/stb-api/v1/settings
extern char *	REST_BIND_SETTINGS_Get (char *szSessionId, char *szArgJson);
// PUT http://127.0.0.1:37564/stb-api/v1/settings
extern char *	REST_BIND_SETTINGS_Put (char *szSessionId, char *szPayload);

/*
	rest_bind_pincode.c
*/
extern int		REST_BIND_PINCODE_VerifyPincode (char *szPinCode);

/*
	rest_bind_tuner.c
*/
extern int 		REST_BIND_TUNER_RegisterNotifier (RestBind_Notifier_t pfNotifier);
extern int 		REST_BIND_TUNER_UnregisterNotifier (RestBind_Notifier_t pfNotifier);
// GET http://127.0.0.1:37564/stb-api/v1/tuners
extern char *	REST_BIND_TUNER_Get (char *szSessionId, char *szArgJson);
// GET http://127.0.0.1:37564/stb-api/v1/tuners/0
extern char *	REST_BIND_TUNER_GetOne (char *szSessionId, int nTunerId, char *szArgJson);




#endif	// __REST_BIND_API_H__
