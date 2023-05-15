/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_epg.c
	@brief

	Description:  									\n
	Module: node/binding/datatype_convert		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <thapi.h>
#include <apk.h>
#include <rest_bind_api.h>
#include <rest_datatype.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
// Converting Types:
STATIC RestEnumStringTable_t	 s_stRestDataType_EpgType =
{
	(HCHAR*)DxNAMEOF(DxEPG_Type_e),
		
	{
		{	"rf_pf",		eDxEPG_TYPE_PF		},
		{	"rf_sc",		eDxEPG_TYPE_SC		},
		{	"rf_pfex",		eDxEPG_TYPE_PF_EX	},
		{	"rf_scex",		eDxEPG_TYPE_SC_EX	},
		{	"rf",			eDxEPG_TYPE_RF		},
		{	"tvtv",			eDxEPG_TYPE_TVTV	},
		{	"ip",			eDxEPG_TYPE_IP		},
		{	NULL,			0					}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_LangId =
{
	(HCHAR*)DxNAMEOF(HxLANG_Id_e),
		
	{
		{	"alb",		eLangID_Albanian		},
		{	"ara",		eLangID_Arabic			},
		{	"aus",		eLangID_Australian		},
		{	"baq",		eLangID_Basque			},
		{	"bas",		eLangID_Basaa			},
		{	"bul",		eLangID_Bulgarian		},
		{	"cat",		eLangID_Catalan			},
		{	"cze",		eLangID_Czech			},
		{	"dan",		eLangID_Danish			},
		{	"dut",		eLangID_Dutch			},
		{	"eng",		eLangID_English			},
		{	"est",		eLangID_Estonian		},
		{	"fao",		eLangID_Faroese			},
		{	"fin",		eLangID_Finnish			},
		{	"fre",		eLangID_French			},
		{	"ger",		eLangID_German			},
		{	"gla",		eLangID_Gaelic			},
		{	"gle",		eLangID_Irish			},
		{	"glg",		eLangID_Galician		},
		{	"gre",		eLangID_Greek			},
		{	"heb",		eLangID_Hebrew			},
		{	"hin",		eLangID_Hindi			},
		{	"hun",		eLangID_Hungarian		},
		{	"ice",		eLangID_Icelandic		},
		{	"ita",		eLangID_Italian			},
		{	"jpn",		eLangID_Japanese		},
		{	"kor",		eLangID_Korean			},
		{	"lav",		eLangID_Latvian			},
		{	"lit",		eLangID_Lithuanian		},
		{	"nar",		eLangID_Narration		},
		{	"nor",		eLangID_Norwegian		},
		{	"per",		eLangID_Persian			},
		{	"pol",		eLangID_Polish			},
		{	"por",		eLangID_Portuguese		},
		{	"qaa",		eLangID_OriginalAudio	},
		{	"rum",		eLangID_Romanian		},
		{	"rus",		eLangID_Russian			},
		{	"scr",		eLangID_Croatian		},
		{	"slo",		eLangID_Slovak			},
		{	"slv",		eLangID_Slovenian		},
		{	"spa",		eLangID_Spanish			},
		{	"srp",		eLangID_Serbian			},
		{	"swe",		eLangID_Swedish			},
		{	"tha",		eLangID_Thai			},
		{	"tur",		eLangID_Turkish			},
		{	"ukr",		eLangID_Ukrainian		},
		{	"und",		eLangID_Undefined		},
		{	"vie",		eLangID_Vietnamese		},
		{	"wel",		eLangID_Welsh			},
		{	"mis",		eLangID_Miscellaneous	},
		{	"mul",		eLangID_Multiple		},
		{	NULL,			0				}
	}
};

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
#define _____EPG_____
HINT32 rest_datatype_JsonizeEvent (HINT32 nJsonWriter, DxEvent_t *pstEvent, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstEvent)
	{
		HxLOG_Error("pstSvc NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonEnum(nWriter, "type", DxNAMEOF(DxEPG_Type_e), (HINT32)pstEvent->type, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "channelUid", pstEvent->svcuid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "onId", pstEvent->onid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "tsId", pstEvent->tsid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "svcId", pstEvent->svcid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "eventId", pstEvent->eventId, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "startTime", pstEvent->startTime, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "duration", pstEvent->duration, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "runStatus", (HINT32)pstEvent->runStatus, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "freeCaMode", (HINT32)pstEvent->freeCaMode, szSelect);

		//(void)rest_datatype_SetJsonEnum(nWriter, "language", DxNAMEOF(HxLANG_Id_e), (HINT32)pstEvent->language, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "parentalRating", (HINT32)pstEvent->parentalRating, szSelect);

		if (pstEvent->name)
		{
			DxEventString_t *item;
			HxJSONWRITER_Array(nWriter, "name");
			HxVECTOR_EACH(pstEvent->name, DxEventString_t *, item, {
				HxJSONWRITER_ObjectBegin(nWriter);
					rest_datatype_SetJsonEnum(nWriter, "language", DxNAMEOF(HxLANG_Id_e), item->language, szSelect);	
					rest_datatype_SetJsonString(nWriter, "string", item->string, szSelect);
				HxJSONWRITER_ObjectEnd(nWriter);
			});
			HxJSONWRITER_ArrayEnd(nWriter);
			//(void)rest_datatype_SetJsonString(nWriter, "name", pstEvent->name, szSelect);
		}

		if (pstEvent->text)
		{
			DxEventString_t *item;
			HxJSONWRITER_Array(nWriter, "text");
			HxVECTOR_EACH(pstEvent->text, DxEventString_t *, item, {
				HxJSONWRITER_ObjectBegin(nWriter);
					rest_datatype_SetJsonEnum(nWriter, "language", DxNAMEOF(HxLANG_Id_e), item->language, szSelect);	
					rest_datatype_SetJsonString(nWriter, "string", item->string, szSelect);
				HxJSONWRITER_ObjectEnd(nWriter);
			});
			HxJSONWRITER_ArrayEnd(nWriter);
		//	(void)rest_datatype_SetJsonString(nWriter, "text", pstEvent->text, szSelect);
		}

		if (pstEvent->longText)
		{
			DxEventString_t *item;
			HxJSONWRITER_Array(nWriter, "longText");
			HxVECTOR_EACH(pstEvent->longText, DxEventString_t *, item, {
				HxJSONWRITER_ObjectBegin(nWriter);
					rest_datatype_SetJsonEnum(nWriter, "language", DxNAMEOF(HxLANG_Id_e), item->language, szSelect);	
					rest_datatype_SetJsonString(nWriter, "string", item->string, szSelect);
				HxJSONWRITER_ObjectEnd(nWriter);
			});
			HxJSONWRITER_ArrayEnd(nWriter);
			
		//	(void)rest_datatype_SetJsonString(nWriter, "longText", pstEvent->longText, szSelect);
		}

		// Thumbnail Info:
		if (eDxEPG_TYPE_IP == pstEvent->type)
		{
			HCHAR				*szThumbUrl = NULL, *szFileName = NULL;

			szThumbUrl = (HCHAR *)THAPI_GetThumbnailURL((const HCHAR *)pstEvent->extension.ipEpg.thumbnailURL, 0, (const HCHAR *)"/mnt/hd1", FALSE);
			szFileName = HLIB_STD_GetFileNameFromUrl(szThumbUrl);

			if ((NULL != szFileName) && ('\0' != *szFileName))
			{
				RestThumbnailInfo_t  stThumbnailInfo;

				HxSTD_MemSet(&stThumbnailInfo, 0, sizeof(RestThumbnailInfo_t));
				HxSTD_snprintf(stThumbnailInfo.szMimeType, REST_MEDIA_MIME_LEN, "image/jpeg");
				HxSTD_snprintf(stThumbnailInfo.szThumbnailUrl, REST_MEDIA_URL_LEN, "/thumbnails/epg/%s", szFileName);
				
				(void)HxJSONWRITER_StringValue(nJsonWriter, "thumbnail");
				rest_datatype_JsonizeThumbnailInfo(nWriter, &stThumbnailInfo, szSelect);
			}
		}

	#if 0
		// TODO: Thumbnail 정보를 임시로 아래와 같이 함.
		{
			RestThumbnailInfo_t stThumbnailInfo;
			HxSTD_MemSet( &stThumbnailInfo, 0x00, sizeof( RestThumbnailInfo_t ) );
			{
				HxSTD_snprintf( stThumbnailInfo.szMimeType, REST_MEDIA_MIME_LEN, "image/jpeg" );
				HxSTD_snprintf( stThumbnailInfo.szThumbnailUrl, REST_MEDIA_URL_LEN, "/thumbnails/epg/%d.jpg", pstEvent->eventId );
			}
		}
	#endif
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeEvent (HxJSON_t *pstJson, DxEvent_t *pstEvent, HBOOL bClearObj)
{

	if (NULL == pstEvent)
	{
		HxLOG_Error("pstEvent NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstEvent, 0, sizeof(DxEvent_t));
	}

	(void)rest_datatype_GetJsonEnum(pstJson, "type", DxNAMEOF(DxEPG_Type_e), (HINT32 *)&(pstEvent->type));
	(void)rest_datatype_GetJsonInteger(pstJson, "channelUid", (HINT32 *)&(pstEvent->svcuid));
	(void)rest_datatype_GetJsonInteger(pstJson, "onId", (HINT32 *)&(pstEvent->onid));
	(void)rest_datatype_GetJsonInteger(pstJson, "tsId", (HINT32 *)&(pstEvent->tsid));
	(void)rest_datatype_GetJsonInteger(pstJson, "svcId", (HINT32 *)&(pstEvent->svcid));
	(void)rest_datatype_GetJsonInteger(pstJson, "eventId", (HINT32 *)&(pstEvent->eventId));
	(void)rest_datatype_GetJsonUnsigned(pstJson, "startTime", (HUINT32 *)&(pstEvent->startTime));
	(void)rest_datatype_GetJsonUnsigned(pstJson, "duration", (HINT32 *)&(pstEvent->duration));
	(void)rest_datatype_GetJsonInteger(pstJson, "runStatus", (HINT32 *)&(pstEvent->runStatus));
	(void)rest_datatype_GetJsonInteger(pstJson, "freeCaMode", (HINT32 *)&(pstEvent->freeCaMode));
	(void)rest_datatype_GetJsonInteger(pstJson, "parentalRating", (HINT32 *)&(pstEvent->parentalRating));
	//(void)rest_datatype_GetJsonEnum(pstJson, "language", DxNAMEOF(HxLANG_Id_e), (HINT32 *)&(pstEvent->language));

	return ERR_OK;	
}

#define _____INITIALIZATION_____
HERROR rest_datatype_InitEpg (void)
{
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_EpgType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_LangId);

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___

