/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  rest_bind.h
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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
#ifndef __REST_BIND_H__
#define __REST_BIND_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <rest_bind_api.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
// General Error			( 0 ~ 9 )			################################
#define REST_ERR_CODE_OK							( 0 )
#define REST_ERR_CODE_INTERNAL						( 1 )
#define REST_ERR_CODE_NOMEM							( 2 )
#define REST_ERR_CODE_NOTREADY						( 3 )
#define REST_ERR_CODE_NODATA						( 4 )

#define REST_ERR_STRING_OK							"No Error"
#define REST_ERR_STRING_INTERNAL					"Internal Error"
#define REST_ERR_STRING_NOMEM						"No Memory"
#define REST_ERR_STRING_NOTREADY					"Not Ready"
#define REST_ERR_STRING_NODATA						"No Data"

// URL Error				( 10 ~ 19 )		################################
#define REST_ERR_CODE_UNSUPPORTED_API				( 11 )
#define REST_ERR_CODE_PAGE_NOTFOUND					( 12 )
#define REST_ERR_CODE_RESOURCE_NOTFOUND				( 13 )

#define REST_ERR_STRING_UNSUPPORTED_API				"Unsupported API"
#define REST_ERR_STRING_PAGE_NOTFOUND				"Page Not Found"
#define REST_ERR_STRING_RESOURCE_NOTFOUND			"Resource Not Found"

// Parameter Error			( 21 ~49 )		################################
#define REST_ERR_CODE_INVALID_KEY					( 21 )
#define REST_ERR_CODE_INVALID_VALUE					( 22 )
#define REST_ERR_CODE_REQUIRE_MANDATORYKEY			( 23 )
#define REST_ERR_CODE_CMD_FAILED					( 24 )
#define REST_ERR_CODE_UNKNOWN_CMD					( 25 )
#define REST_ERR_CODE_WRONG_SOURCE					( 26 )
#define REST_ERR_CODE_WRONG_SOURCETYPE				( 27 )

#define REST_ERR_STRING_INVALID_KEY					"Invalid Key"
#define REST_ERR_STRING_INVALID_VALUE				"Invalid Value"
#define REST_ERR_STRING_REQUIRE_MANDATORYKEY		"Require Mandatory Key"
#define REST_ERR_STRING_CMD_FAILED					"Command Failed"
#define REST_ERR_STRING_UNKNOWN_CMD					"Unknown Command"
#define REST_ERR_STRING_WRONG_SOURCE				"Wrong Source"
#define REST_ERR_STRING_WRONG_SOURCETYPE			"Wrong Source Type"

// Reserved Error			( 50 ~ 89 )		################################
// ...


// Permission Error		( 90 ~ 99 )		################################
#define REST_ERR_CODE_REQUIRE_PIN					( 91 )
#define REST_ERR_CODE_INVALID_PIN					( 92 )
#define REST_ERR_CODE_INVALID_SESSION				( 93 )
#define REST_ERR_CODE_ACCESS_DENY					( 99 )

#define REST_ERR_STRING_REQUIRE_PIN					"Require PIN Code"
#define REST_ERR_STRING_INVALID_PIN					"Invalid PIN Code"
#define REST_ERR_STRING_INVALID_SESSION				"Invalid Session"
#define REST_ERR_STRING_ACCESS_DENY					"Remote-Access Was Disabled"



// Module Common Error	( 1000 ~ 1099 )	################################

// Channel Error			( 1101 ~ 1199 )	################################
#define REST_ERR_CODE_NO_TUNABLE_INFO				( 1101 )

#define REST_ERR_STRING_NO_TUNABLE_INFO				"Tunable infomation not available"

// Player Error			( 1201 ~ 1299 )	################################

// Scan Error				( 1301 ~ 1399 )	################################

// EPG Error				( 1401 ~ 1499 )	################################

// Schedule Error			( 1501 ~ 1599 )	################################

// Record Error			( 1601 ~ 1699 )	################################

// Settings Error			( 1701 ~ 1799 )	################################

// ...




#define REST_ERR_FORMAT								"%s\"error\":{ \"errCode\":%d, \"errString\":\"%s\" }%s"
#define REST_ERR_NULL								""

#define REST_DUMMY_SESSION_ID						"tempSessionIdString"


// rest_bind_record.c
#define REST_REC_WAIT_UNTIL_SERVICE_REPLIED			"-dont-send-string-now-"

// rest_bind_chanscan.c
#define REST_CHANSCAN_INST_NUM						1

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
// rest_bind_player.c
typedef struct
{
	HUINT32			 ulViewId;
	HUINT32			 ulSessionId;
} RestBindPlayerContext_t;

// rest_bind_channel.c
typedef struct
{
	HUINT32					 ulSemId;

	RestChanListStatus_t	 stChanListStatus;

	// DAPI notifier ID
	HUINT32					 ulSvcNotifier;
	HUINT32					 ulTsNotifier;
	HUINT32					 ulAntNotifier;

	// 실제 데이터 보관용
	HxVector_t				*pstSvcVector;
	HxVector_t				*pstTsVector;

	// sorting용 Vector
	HxVector_t				*pstSvcLcnVector;

	//
	//   for the Tunable Check 
	//
	HUINT32					 ulSemForTunable;			// Tunable 정보용 Semaphore

	HUINT32					 ulLatestCheckSession;
	HUINT32					 ulCurrentCheckSession;
	
	HUINT32					 ulTunableInfoNum;
	RestTsTunable_t			*pstTunableArray;			// Tunable 정보 

	HUINT32					 ulUpdatingTunableNum;
	RestTsTunable_t			*pstUpdatingTunableArray;	// Tunable 정보 수정용:
} RestBindChanContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/
// rest_bind.c
extern HINT32	rest_bind_OpenJsonWriter (void);
extern HCHAR *	rest_bind_CloseJsonWriter (HINT32 nJsonWriter);
extern HINT32	rest_bind_OpenRwstreamWriter (void);
extern HCHAR *	rest_bind_CloseRwstreamWriter (HINT32 nRwstream);
extern HCHAR *	rest_bind_MakeErrorString (HINT32 nErrCode, HCHAR *szErrString, HBOOL bMakeObject);
extern HERROR	rest_bind_GetNumberValueFromJson (HxJSON_t jsObj, HCHAR *szItem, HINT32 *pnNumber);
extern HERROR	rest_bind_GetStringValueFromJson (HxJSON_t jsObj, HCHAR *szItem, HCHAR **pszString);
extern HINT32	rest_bind_MakeErrorJsWriter (HINT32 nJsWriter, HINT32 nErrCode, HCHAR *szErrString);

// rest_bind_common.c
extern HERROR rest_bind_common_Init (void);

// rest_bind_channel.c
extern RestBindChanContext_t *	rest_bind_chan_GetContext (void);
extern HERROR					rest_bind_chan_Init (void);
extern HERROR					rest_bind_chan_ExecNotifier (HCHAR *szEvent, HCHAR *szPayLoad);

// rest_bind_channel_tunable.c
extern HERROR					rest_bind_tunable_Init (void);
extern HERROR					rest_bind_tunable_CheckTunableTps (void);

// rest_bind_player.c
extern HERROR	rest_bind_play_Init (void);
extern RestBindPlayerContext_t *	rest_bind_play_GetContext (HUINT32 ulViewId);
extern void		rest_bind_play_FreePlayerStatus (RestPlayerStatus_t *pstStatus);
extern RestPlayerStatus_t*			rest_bind_play_GetPlayerStatus (HUINT32 ulPlayerId);
extern void		rest_bind_play_NotifyApkMediaPlay (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData);

// rest_bind_player_live.c
extern HCHAR *	rest_bind_play_NotifyApkMediaPlay_Live (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData);

// rest_bind_scan.c
extern HERROR	rest_bind_scan_Init (void);

// rest_bind_scan_channels.c
extern HCHAR *	rest_bind_scan_chan_CmdGet (RestInstance_t *pstInst, RestScanChanParams_t *pstParam, HxJSON_t jsArgs);
extern HCHAR *	rest_bind_scan_chan_CmdPut (RestInstance_t *pstInst, RestScanChanParams_t *pstParam, RestScanCmd_e eScanCmd);


// rest_bind_epg.c
extern HERROR	rest_bind_epg_Init (void);

// rest_bind_schedule.c
extern HERROR	rest_bind_schedule_Init (void);

// rest_bind_record.c
extern HERROR	rest_bind_record_Init (void);

// rest_bind_recorder.c
extern HERROR	rest_bind_recorder_Init (void);

// rest_bind_sysenv.c
extern HERROR	rest_bind_sysenv_Init (void);

// rest_bind_settings.c
extern HERROR	rest_bind_setting_Init (void);

// rest_bind_pincode.c
extern HERROR	rest_bind_pincode_Init (void);

// rest_bind_tuner.c
extern HERROR	rest_bind_tuner_Init (void);

#endif	// __REST_BIND_H__
