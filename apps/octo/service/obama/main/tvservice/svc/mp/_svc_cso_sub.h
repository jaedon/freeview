/** **********************************************************************************************************
	@file 		SVC_CSO_SUB.h

	@date		2012/10/22
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#include <isosvc.h>
#include "svc_mplayercso.h"

#define SVC_CSO_SUB_STR_MAX					64
#define SVC_CSO_SUB_LANGUAGE_MAX			8
#define SVC_CSO_SUB_LANGUAGE_STRLEN_MAX		32

#define SVC_CSO_SUB_STRING_MAX				128
#define SVC_CSO_SUB_LINE_MAX				8


/** @brief Media Language Info. */
typedef struct tagSvcCsoSub_LangInfo
{
	HUINT32         ulTotalLang;
	HUINT32         ulCurIdx;
	HINT8           szDispLang[SVC_CSO_SUB_LANGUAGE_MAX][SVC_CSO_SUB_LANGUAGE_STRLEN_MAX];
	HINT8           szName[SVC_CSO_SUB_LANGUAGE_MAX][SVC_CSO_SUB_LANGUAGE_STRLEN_MAX];
} SvcCsoSub_LangInfo_t;

typedef struct tagSvcCsoSub_DisplayPacket
{
	HUINT32			ulFontSize;
	HUINT32			ulTextPosition;
	HUINT32         ulStrNum;
	HUINT8          pText[SVC_CSO_SUB_LINE_MAX][SVC_CSO_SUB_STR_MAX];
} SvcCsoSub_Display_Packet_t;

/** @brief Media Subtitle Status. */
typedef enum
{
	eSVC_CSO_SUB_STATUS_RELEASE = 0,
	eSVC_CSO_SUB_STATUS_SETUP,
	eSVC_CSO_SUB_STATUS_STOP,
	eSVC_CSO_SUB_STATUS_PLAY,

} SvcCsoSub_Status_e;

typedef enum
{
	eSVC_CSO_SUB_Event_Ready = 0,
	eSVC_CSO_SUB_Event_CheckFileFailed,
	eSVC_CSO_SUB_Event_Diaplay,
	eSVC_CSO_SUB_Event_Clear,
} SvcCsoSub_Event_e;

/** @brief Media Subtitle Type. */
typedef enum
{
	eSVC_CSO_SUB_TYPE_SAMI = 0,
	eSVC_CSO_SUB_TYPE_SRT,
	eSVC_CSO_SUB_TYPE_SUBVIEWER_1,
	eSVC_CSO_SUB_TYPE_SUBVIEWER_2,
	eSVC_CSO_SUB_TYPE_POWERDIVX,
	eSVC_CSO_SUB_TYPE_MICRODVD,
	eSVC_CSO_SUB_TYPE_TMPLAYER,
	eSVC_CSO_SUB_TYPE_TEXT,
	eSVC_CSO_SUB_TYPE_SSA,
	eSVC_CSO_SUB_TYPE_XSUB,

	eSVC_CSO_SUB_TYPE_NOTHING
} SvcCsoSub_Type_e;

typedef HUINT32         OSD_Color_t;        /* True color OSD */

typedef struct tagSvcCsoSub_Packet
{
	HxLANG_Id_e    eLangId;                        /**< Packet의 Language. */

	HUINT32         startTime;                      /**< Packet의 Start Time. */
	HUINT32         endTime;                        /**< Packet의 End Time. */
	HUINT32         ulStrNum;                       /**< Packet의 string 개수. */
	HUINT8          bNewLine[SVC_CSO_SUB_STR_MAX];        /**< Text Type. */
	OSD_Color_t     fontColor[SVC_CSO_SUB_STR_MAX];       /**< Packet의 Color. */
	HUINT8          *pText[SVC_CSO_SUB_STR_MAX];          /**< Packet의 Text. */
} SvcCsoSub_Packet_t;

typedef struct tagSvcCsoSub_Style
{
	HUINT32     ulFtSize;
	OSD_Color_t ftColor;
} SvcCsoSub_Style_t;

typedef void (*SvcCsoSub_EventCallback_t)(SvcCso_t *player, HUINT32 p1, HUINT8 *p2);

HERROR SVC_CSO_SUB_Init(SvcCsoSub_EventCallback_t pSubMediaEventCallback);

HERROR SVC_CSO_SUB_Create(SvcCso_t *player);

HERROR SVC_CSO_SUB_Destroy(SvcCso_t *player);

HERROR SVC_CSO_SUB_Release(void);

HERROR SVC_CSO_SUB_Open(SvcCso_t *player, HUINT8 *ucFilePath);

HERROR SVC_CSO_SUB_Close( SvcCso_t *player );

HERROR SVC_CSO_SUB_SetDisplay(SvcCso_t *player, HBOOL bEanble);

HERROR SVC_CSO_SUB_SetLanguage(SvcCso_t *player, HUINT32 ulLangIdx);

HERROR SVC_CSO_SUB_GetType(SvcCso_t *player, SvcCsoSub_Type_e *peFileType);

HERROR SVC_CSO_SUB_GetLanguageInfo(SvcCso_t *player, SvcCsoSub_LangInfo_t *pstLanguageInfo);

HERROR SVC_CSO_SUB_ChangeSyncOffsetTime(SvcCso_t *player, HINT32 ulMsTime);

HERROR SVC_CSO_SUB_GetCurSyncOffsetTime(SvcCso_t *player, HINT32 *ulMsTime);

HERROR SVC_CSO_SUB_GetDisplay(SvcCso_t *player, HBOOL *bEanble);

