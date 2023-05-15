/** **********************************************************************************************************
	@file 		pal_media_sub.h

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

/** @brief Media Subtitle Status. */
typedef enum
{
	PAL_MEDIA_SUB_STATUS_RELEASE = 0,
	PAL_MEDIA_SUB_STATUS_SETUP,
	PAL_MEDIA_SUB_STATUS_STOP,
	PAL_MEDIA_SUB_STATUS_PLAY,

} PAL_MEDIA_SUB_Status;

typedef enum
{
	PAL_MEDIA_SUB_Event_Ready = 0,
	PAL_MEDIA_SUB_Event_CheckFileFailed,
	PAL_MEDIA_SUB_Event_Diaplay,
	PAL_MEDIA_SUB_Event_Clear,
} PAL_MEDIA_SUB_Event;

/** @brief Media Subtitle Type. */
typedef enum
{
	PAL_MEDIA_SUB_TYPE_SAMI = 0,
	PAL_MEDIA_SUB_TYPE_SRT,
	PAL_MEDIA_SUB_TYPE_SUBVIEWER_1,
	PAL_MEDIA_SUB_TYPE_SUBVIEWER_2,
	PAL_MEDIA_SUB_TYPE_POWERDIVX,
	PAL_MEDIA_SUB_TYPE_MICRODVD,
	PAL_MEDIA_SUB_TYPE_TMPLAYER,
	PAL_MEDIA_SUB_TYPE_TEXT,
	PAL_MEDIA_SUB_TYPE_SSA,
	PAL_MEDIA_SUB_TYPE_XSUB,

	PAL_MEDIA_SUB_TYPE_NOTHING
} PAL_MEDIA_SUB_Type_t;

typedef HUINT32         OSD_Color_t;        /* True color OSD */

typedef struct tagPAL_MEDIA_SUB_Packet
{
	HxLANG_Id_e    eLangId;                        /**< Packet의 Language. */

	HUINT32         startTime;                      /**< Packet의 Start Time. */
	HUINT32         endTime;                        /**< Packet의 End Time. */
	HUINT32         ulStrNum;                       /**< Packet의 string 개수. */
	HUINT8          bNewLine[PAL_MEDIA_SUB_STR_MAX];        /**< Text Type. */
	OSD_Color_t     fontColor[PAL_MEDIA_SUB_STR_MAX];       /**< Packet의 Color. */
	HUINT8          *pText[PAL_MEDIA_SUB_STR_MAX];          /**< Packet의 Text. */
} PAL_MEDIA_SUB_Packet_t;

typedef struct tagPAL_MEDIA_SUB_STYLE
{
	HUINT32     ulFtSize;
	OSD_Color_t ftColor;
} PAL_MEDIA_SUB_Style_t;

typedef void (*PAL_MEDIA_SUB_EventCallback_t)(PAL_MEDIA_HANDLE phMedia, HUINT32 p1, HUINT8 *p2);

HERROR PAL_MEDIA_SUB_Init(PAL_MEDIA_SUB_EventCallback_t pSubMediaEventCallback);

HERROR PAL_MEDIA_SUB_Create(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_SUB_Destroy(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_SUB_Release(void);

HERROR PAL_MEDIA_SUB_Open(PAL_MEDIA_HANDLE phMedia, HUINT8 *ucFilePath);

HERROR PAL_MEDIA_SUB_Close( PAL_MEDIA_HANDLE phMedia );

HERROR PAL_MEDIA_SUB_SetDisplay(PAL_MEDIA_HANDLE phMedia, HBOOL bEanble);

HERROR PAL_MEDIA_SUB_SetLanguage(PAL_MEDIA_HANDLE phMedia, HUINT32 ulLangIdx);

HERROR PAL_MEDIA_SUB_GetType(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_SUB_Type_t *peFileType);

HERROR PAL_MEDIA_SUB_GetLanguageInfo(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_SUB_LangInfo_t *pstLanguageInfo);

HERROR PAL_MEDIA_SUB_ChangeSyncOffsetTime(PAL_MEDIA_HANDLE phMedia, HINT32 ulMsTime);

HERROR PAL_MEDIA_SUB_GetCurSyncOffsetTime(PAL_MEDIA_HANDLE phMedia, HINT32 *ulMsTime);

HERROR PAL_MEDIA_SUB_GetDisplay(PAL_MEDIA_HANDLE phMedia, HBOOL *bEanble);

