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
 * @file	  		nx_core_pf.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <nx_core_metadata_pf.h>
#include <nx_core_system.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_METADATA_PF_MAX_TIMETEX_TLEN					6


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum
{
	eNxPfType_Present	= 0,
	eNxPfType_Following	= 1,
}nx_pf_type_e;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static 	NX_Metadata_PF_Context_t *stMetaDataPfContext = NULL;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NX_Metadata_PF_Context_t *nx_pf_GetContext(void);
#if defined(CONFIG_DEBUG)
static void		nx_pf_Command_Init(void);
#endif
static HxLANG_Id_e nx_metadata_Pf_GetMainLanguage(void);
static HBOOL	nx_metadata_Pf_FinedEventString(void *data, void *language);
static void		nx_metadata_Pf_PfMonitorCB (HUINT32 svcUid, HxVector_t *pfDataList);
static NX_Programme_t *nx_metadata_Pf_NewProgrammes(HUINT32 svcuid, DxEvent_t *pstEvent);
static void 	nx_metadata_Pf_DeleteProgrammes(NX_Programme_t *pstProgrammes);

// programmes
static NX_Programme_t *nx_metadata_Pf_GetProgramme(nx_pf_type_e ePfType);


/**************************************************************************************************/
#define ______________Private_functions__________________________________________
/**************************************************************************************************/
static NX_Metadata_PF_Context_t *nx_pf_GetContext(void)
{
	if (stMetaDataPfContext == NULL)
	{
		stMetaDataPfContext = (NX_Metadata_PF_Context_t*)NX_APP_Calloc(sizeof(NX_Metadata_PF_Context_t));
	}

	return stMetaDataPfContext;
}

static HxLANG_Id_e	nx_metadata_Pf_GetMainLanguage(void)
{
	HCHAR buf[4] = {'\0',};

	if (NX_PORT_SYSTEM_GetMainLanguage(buf) != ERR_OK)
		return eLangID_NONE;

	HxLOG_Debug("current language is %s\n", buf);
	return HLIB_LANG_639ToId(buf);
}


static HBOOL	nx_metadata_Pf_FinedEventString(void *data, void *language)
{
	DxEventString_t *pstEventString = (DxEventString_t *)data;
	HxLANG_Id_e eLanguage = (HxLANG_Id_e)language;

	if(pstEventString && pstEventString->language == eLanguage)
		return TRUE;
	else
		return FALSE;
}

static void nx_metadata_Pf_DeleteProgrammes(NX_Programme_t *pstProgrammes)
{
	if (pstProgrammes)
	{
		if(pstProgrammes->event)
			NX_PORT_EVENT_Delete(pstProgrammes->event);
		NX_APP_Free(pstProgrammes);
		pstProgrammes = NULL;
	}
}

static NX_Programme_t *nx_metadata_Pf_NewProgrammes(HUINT32 svcuid, DxEvent_t *pstEvent)
{
	NX_Programme_t *pstProgrammes = (NX_Programme_t*)NX_APP_Calloc(sizeof(NX_Programme_t));

	pstProgrammes->svcuid	= svcuid;
	pstProgrammes->event 	= pstEvent;

	return pstProgrammes;
}

static void 	nx_metadata_Pf_PfMonitorCB (HUINT32 svcUid, HxVector_t *pfDataList)
{
	NX_Metadata_PF_Context_t *pstContext = nx_pf_GetContext();
	HINT32  nResultNum = 0,	i = 0;

	nResultNum = HLIB_VECTOR_Length(pfDataList);

	if (nResultNum > 0 )
	{
		while(pstContext->nCurProgrammesNum--)
		{
			nx_metadata_Pf_DeleteProgrammes(pstContext->curProgrammes[pstContext->nCurProgrammesNum]);
		}

		pstContext->nCurProgrammesNum	= nResultNum;

		for (i = 0; i < nResultNum, i < 12; )
		{
			DxEvent_t *pstEvent = (DxEvent_t*)HLIB_VECTOR_ItemAt(pfDataList, i);
			if ((pstEvent) && (pstEvent->svcuid == svcUid))
			{
				pstContext->curProgrammes[i] = (NX_Programme_t*)nx_metadata_Pf_NewProgrammes(svcUid, NX_PORT_EVENT_Incref(pstEvent));
			}
			i++;
		}

		ONDK_GWM_PostMessage(NULL, MSG_CORE_SERVICE_PF_CHANGED_PROGRAMMES, 0, svcUid, 0, 0);
	}
	else
	{
		while(pstContext->nCurProgrammesNum--)
		{
			nx_metadata_Pf_DeleteProgrammes(pstContext->curProgrammes[pstContext->nCurProgrammesNum]);
		}
		pstContext->nCurProgrammesNum = 0;
	}
}


/**************************************************************************************************/
#define _____________Programmes_Functions__________________________
/**************************************************************************************************/
static NX_Programme_t *nx_metadata_Pf_GetProgramme(nx_pf_type_e ePfType)
{
	NX_Metadata_PF_Context_t *pstContext = nx_pf_GetContext();
	NX_Programme_t *pstProgramme = NULL;

	if (pstContext->nCurProgrammesNum <= 0)
		return NULL;

	pstProgramme = pstContext->curProgrammes[ePfType];

	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return NULL;
	}
	return pstProgramme;
}


/**************************************************************************************************/
#define _____________Public_functions____________________________________________
/**************************************************************************************************/
HERROR	NX_METADATA_PF_Init(void)
{
#if defined(CONFIG_DEBUG)
	nx_pf_Command_Init();
#endif

	return ERR_OK;
}

HERROR	NX_METADATA_PF_SearchPF(NX_Channel_t *pstChannel)
{
	NX_MetadataSearch_t *pstMetaSearch = NULL;

	if (pstChannel == NULL)
	{
		HxLOG_Debug("\n pstChannel Data is NULL!\n");
		return ERR_FAIL;
	}

	pstMetaSearch = NX_METADATA_SEARCH_New(eNX_META_SEARCH_TARGET_SCHEDULED);
	if (pstMetaSearch == NULL)
	{
		HxLOG_Debug("\n pstMetaSearch Data is NULL!\n");
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("\n Call NX_METADATA_SEARCH_SearchPF!\n");

		NX_METADATA_SEARCH_SearchPF(pstMetaSearch, pstChannel->svcUid, nx_metadata_Pf_PfMonitorCB, NULL);
	}
	return ERR_OK;
}

NX_Programme_t	*NX_METADATA_PF_GetPF(NX_Channel_t *pstChannel)
{
	NX_Programme_t	*pstProgrammes = NULL;
	HINT32  nResultNum = 0,	i = 0;
	HxVector_t * pfDataList = NULL;

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	DxService_t *findSvc	= NULL;
	findSvc = NX_CHANNEL_FindServiceBySvcUid(pstChannel->svcUid);
	if (findSvc == NULL)
		return NULL;
	pfDataList = NX_PORT_META_QUERY_GetPF(findSvc->onid, findSvc->tsid, findSvc->svcid);
#else
	pfDataList = NX_PORT_META_QUERY_GetPF(pstChannel->svcUid);
#endif

	if (pfDataList == NULL)
		return NULL;

	nResultNum = HLIB_VECTOR_Length(pfDataList);

	if (nResultNum > 0 )
	{
		for (i = 0; i < nResultNum, i < 12; )
		{
			DxEvent_t *pstEvent = (DxEvent_t*)HLIB_VECTOR_ItemAt(pfDataList, i);
			if ((pstEvent) && (pstEvent->svcuid == pstChannel->svcUid))
			{
				pstProgrammes =	(NX_Programme_t*)nx_metadata_Pf_NewProgrammes(pstChannel->svcUid, NX_PORT_EVENT_Incref(pstEvent));
				break;
			}
			i++;
		}
	}
	return pstProgrammes;
}

HERROR	NX_METADATA_PF_DeleteProgrammes(NX_Programme_t *pstProgramme)
{
	if (pstProgramme)
	{
		nx_metadata_Pf_DeleteProgrammes(pstProgramme);
	}

	return ERR_OK;
}


/**************************************************************************************************/
#define _____________Programmes_Functions________________
/**************************************************************************************************/
HERROR	NX_PF_PROGRAMMES_GetName(HUINT32 svcUid, char *desBuf)
{
	NX_Programme_t *pstProgramme = NULL;
	DxEvent_t *pstEvent = NULL;
	HxLANG_Id_e language = eLangID_NONE;
	HxVector_t *name = NULL;
	DxEventString_t *eventString = NULL;
	char *retName = NULL;

	pstProgramme = nx_metadata_Pf_GetProgramme(eNxPfType_Present);
	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	if (pstProgramme->svcuid != svcUid)
	{
		HxLOG_Warning("Not Matching current svcuid in programme info! (programmes svcuid(%ld), source svcuid(%ld)\n\n\n",
					pstProgramme->svcuid, svcUid);
		return ERR_FAIL;
	}

	language = nx_metadata_Pf_GetMainLanguage();

	pstEvent = pstProgramme->event;

	if ((pstEvent == NULL) || (pstEvent->name == NULL))
	{
		HxLOG_Warning("pstEvent is NULL or name is NULL\n");
		return ERR_FAIL;
	}
	name = pstEvent->name;
	eventString = HLIB_VECTOR_Find(name, nx_metadata_Pf_FinedEventString, (void*)language);

	if (eventString == NULL)
		eventString = HLIB_VECTOR_First(name);

	if (eventString && eventString->string)
	{
		retName = eventString->string;
		HxLOG_Warning("event name : %s\n", retName);
		HLIB_STD_StrUtf8NCpy(desBuf, retName, EPG_HUMAX_NAME_LEN);
	}

	return ERR_OK;
}

HERROR	NX_PF_PROGRAMMES_GetDescription(HUINT32 svcUid, char *desBuf)
{
	NX_Programme_t *pstProgramme = NULL;
	DxEvent_t *pstEvent = NULL;
	HxLANG_Id_e language = eLangID_NONE;
	HxVector_t *text = NULL;
	DxEventString_t *eventString = NULL;
	char *retText = NULL;

	pstProgramme = nx_metadata_Pf_GetProgramme(eNxPfType_Present);
	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	language = nx_metadata_Pf_GetMainLanguage();
	pstEvent = pstProgramme->event;

	if ((pstEvent == NULL) || (pstEvent->text == NULL))
	{
		HxLOG_Error("pstEvent is NULL or text is NULL\n");
		return ERR_FAIL;
	}

	if (pstProgramme->svcuid != svcUid)
	{
		HxLOG_Debug("Not Matching current svcuid in programme info! (programmes svcuid(%ld), event svcuid (%ld), source svcuid(%ld)\n\n\n",
					pstProgramme->svcuid, pstEvent->svcuid, svcUid);
		return ERR_FAIL;
	}

	text = pstEvent->text;
	eventString = HLIB_VECTOR_Find(text, nx_metadata_Pf_FinedEventString, (void*)language);

	if (eventString == NULL)
		eventString = HLIB_VECTOR_First(text);

	if (eventString && eventString->string)
	{
		retText = eventString->string;
		HxLOG_Debug("event text : %s\n", retText);
		HLIB_STD_StrUtf8NCpy(desBuf, retText, EPG_HUMAX_SHORT_LEN);
	}

	return ERR_OK;
}

HERROR		NX_PF_PROGRAMMES_GetNameTime(HUINT32 svcUid, HUINT8 *pBufName, HUINT8 *pBufSTime, HUINT8 *pBufETime, HUINT32 *pProgress)
{
	HCHAR 				*retName = NULL;
	HERROR				hErr = ERR_FAIL;
	HUINT32				ulCurTime = 0, ulTime = 0, diff = 0, diffRef = 0;
	DxEvent_t 			*pstEvent = NULL;
	HxLANG_Id_e 		language = eLangID_NONE;
	HxVector_t 			*name = NULL;
	DxEventString_t 	*eventString = NULL;
	NX_Programme_t 		*pstProgramme = NULL;
	HxDATETIME_Time_t 	StartTime, EndTime;


	NX_APP_Assert(pBufName);
	NX_APP_Assert(pBufSTime);
	NX_APP_Assert(pBufETime);
	NX_APP_Assert(pProgress);

	pstProgramme = nx_metadata_Pf_GetProgramme(eNxPfType_Present);
	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	if (pstProgramme->svcuid != svcUid)
	{
		HxLOG_Debug("Not Matching current svcuid in programme info! (programmes svcuid(%ld), source svcuid(%ld)\n\n\n",
					pstProgramme->svcuid, svcUid);
		return ERR_FAIL;
	}

	language = nx_metadata_Pf_GetMainLanguage();

	pstEvent = pstProgramme->event;

	if ((pstEvent == NULL) || (pstEvent->name == NULL))
	{
		HxLOG_Error("pstEvent is NULL or name is NULL\n");
		return ERR_FAIL;
	}

	name = pstEvent->name;
	eventString = HLIB_VECTOR_Find(name, nx_metadata_Pf_FinedEventString, (void*)language);

	if (eventString == NULL)
	{
		eventString = HLIB_VECTOR_First(name);
	}

	// name
	if (eventString && eventString->string)
	{
		retName = eventString->string;
		HxLOG_Debug("event name : %s\n", retName);
		HLIB_STD_StrUtf8NCpy((HCHAR*)pBufName, (HCHAR*)retName, EPG_HUMAX_NAME_LEN);
	}

	// start time
	ulTime = pstEvent->startTime;
	HxSTD_memset(&StartTime, 0x00, sizeof(HxDATETIME_Time_t));
	HLIB_DATETIME_ConvertUnixTimeToTime(ulTime, &StartTime);
	HxSTD_snprintf((char*)pBufSTime, NX_METADATA_PF_MAX_TIMETEX_TLEN, (char*)"%02d:%02d", StartTime.ucHour, StartTime.ucMinute);

	// end time
	ulTime = pstEvent->startTime + pstEvent->duration;
	HxSTD_memset(&EndTime, 0x00, sizeof(HxDATETIME_Time_t));
	HLIB_DATETIME_ConvertUnixTimeToTime(ulTime, &EndTime);
	HxSTD_snprintf((char*)pBufETime, NX_METADATA_PF_MAX_TIMETEX_TLEN, (char*)"%02d:%02d", EndTime.ucHour, EndTime.ucMinute);

	// progressed percent
	hErr = NX_SYSTEM_GetTime(&ulCurTime);
	if(hErr == ERR_OK)
	{
		diffRef = pstEvent->duration;
		if (diffRef == 0)
		{
			HxLOG_Error("pstEvent's duration is 0\n");
			return ERR_FAIL;
		}
		diff = ulCurTime - pstEvent->startTime;
		if(diff > diffRef) diff = diffRef;
		*pProgress = (HUINT32)(diff * 100 / diffRef);
	}

	return ERR_OK;
}


HERROR	NX_PF_PROGRAMMES_GetNameByProgramme(NX_Programme_t *pstProgramme, char *desBuf)
{
	DxEvent_t *pstEvent = NULL;
	HxLANG_Id_e language = eLangID_NONE;
	HxVector_t *name = NULL;
	DxEventString_t *eventString = NULL;
	char *retName = NULL;

	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	language = nx_metadata_Pf_GetMainLanguage();

	pstEvent = pstProgramme->event;

	if ((pstEvent == NULL) || (pstEvent->name == NULL))
	{
		HxLOG_Error("pstEvent is NULL or name is NULL\n");
		return ERR_FAIL;
	}
	name = pstEvent->name;
	eventString = HLIB_VECTOR_Find(name, nx_metadata_Pf_FinedEventString, (void*)language);

	if (eventString == NULL)
		eventString = HLIB_VECTOR_First(name);

	if (eventString && eventString->string)
	{
		retName = eventString->string;
		HxLOG_Debug("event name : %s\n", retName);
		HLIB_STD_StrUtf8NCpy(desBuf, retName, EPG_HUMAX_NAME_LEN);
	}

	return ERR_OK;
}

HERROR	NX_PF_PROGRAMMES_GetDescriptionByProgramme(NX_Programme_t *pstProgramme, char *desBuf)
{
	DxEvent_t *pstEvent = NULL;
	HxLANG_Id_e language = eLangID_NONE;
	HxVector_t *text = NULL;
	DxEventString_t *eventString = NULL;
	char *retText = NULL;

	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	language = nx_metadata_Pf_GetMainLanguage();

	pstEvent = pstProgramme->event;

	if ((pstEvent == NULL) || (pstEvent->text == NULL))
	{
		HxLOG_Error("pstEvent is NULL or text is NULL\n");
		return ERR_FAIL;
	}
	text = pstEvent->text;
	eventString = HLIB_VECTOR_Find(text, nx_metadata_Pf_FinedEventString, (void*)language);

	if (eventString == NULL)
		eventString = HLIB_VECTOR_First(text);

	if (eventString && eventString->string)
	{
		retText = eventString->string;
		HxLOG_Debug("event text : %s\n", retText);
		HLIB_STD_StrUtf8NCpy(desBuf, retText, EPG_HUMAX_SHORT_LEN);
	}

	return ERR_OK;
}

HBOOL	NX_PF_PROGRAMMES_IsHDByProgramme(NX_Programme_t *pstProgramme)
{
	DxEvent_t *pstEvent = NULL;
	HINT32		i, n;
	DxEventComponent_t	*comp;

	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	pstEvent = pstProgramme->event;

	NX_APP_Debug("\n\n\n\n[%s.%d] pstEvetn TYPE : %d \n\n\n\n",__FUNCTION__,__LINE__, pstEvent->type);

	if (pstEvent->type == eDxEPG_TYPE_RF || pstEvent->type == eDxEPG_TYPE_PF)
	{
		if (pstEvent->component == NULL)
		{
			HxLOG_Debug("programme component is NULL\n");
			return FALSE;
		}
		n = HLIB_VECTOR_Length(pstEvent->component);
		for ( i = 0 ; i < n ; i++)
		{
			comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(pstEvent->component, i);
			if (comp == NULL)
				continue;

			if (comp->content == 0x01)
			{
				if (0x09 <= comp->type && comp->type <= 0x10)
					return TRUE;
			}
			else if (comp->content == 0x05)
			{
				if (comp->type == 0x0B
					||	comp->type == 0x0C
					||	comp->type == 0x0F
					||	comp->type == 0x10)
					return TRUE;
			}
		}
	}
#if 0
	else if (pstEvent->type & eDxEPG_TYPE_IP)
	{
		if (pstEvent->extension.ipEpg.component & eDxIPEPG_UKDTT_COMPONENT_HD)
			return TRUE;
	}
#endif

	return FALSE;
}

HUINT32 	NX_PF_PROGRAMMES_GetStartTimeByProgramme(NX_Programme_t *pstProgramme)
{
	HUINT32		value 		= 0;
	DxEvent_t 	*pstEvent	= NULL;

	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	pstEvent = pstProgramme->event;
	if (pstEvent)
	{
		value = (HUINT32)pstEvent->startTime;
	}

	return value;
}

HUINT32		NX_PF_PROGRAMMES_GetDurationByProgramme(NX_Programme_t *pstProgramme)
{
	HUINT32		value 		= 0;
	DxEvent_t 	*pstEvent	= NULL;

	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	pstEvent = pstProgramme->event;
	if (pstEvent)
	{
		value = (HUINT32)pstEvent->duration;
	}

	return value;
}

HERROR		NX_PF_PROGRAMMES_GetEventDispInfo(NX_Programme_t *pstProgramme, HUINT8 *pBufName, HUINT8 *pBufSTime, HUINT8 *pBufETime, HUINT32 *pProgress)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32 			ulCurTime = 0, ulTime = 0, diff = 0, diffRef = 0;
	DxEvent_t			*pstEvent = NULL;
	HxDATETIME_Time_t	StartTime, EndTime;


	NX_APP_Assert(pBufName);
	NX_APP_Assert(pBufSTime);
	NX_APP_Assert(pBufETime);
	NX_APP_Assert(pProgress);

	if (pstProgramme == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	pstEvent = pstProgramme->event;
	if (pstEvent == NULL)
	{
		HxLOG_Error("pstProgramme is NULL!\n");
		return ERR_FAIL;
	}

	// name
	NX_PF_PROGRAMMES_GetNameByProgramme(pstProgramme, (char*)pBufName);

	// start time
	ulTime = pstEvent->startTime;
	HxSTD_memset(&StartTime, 0x00, sizeof(HxDATETIME_Time_t));
	HLIB_DATETIME_ConvertUnixTimeToTime(ulTime, &StartTime);
	HxSTD_snprintf((char*)pBufSTime, NX_METADATA_PF_MAX_TIMETEX_TLEN, (char*)"%02d:%02d", StartTime.ucHour, StartTime.ucMinute);

	// end time
	ulTime = pstEvent->startTime + pstEvent->duration;
	HxSTD_memset(&EndTime, 0x00, sizeof(HxDATETIME_Time_t));
	HLIB_DATETIME_ConvertUnixTimeToTime(ulTime, &EndTime);
	HxSTD_snprintf((char*)pBufETime, NX_METADATA_PF_MAX_TIMETEX_TLEN, (char*)"%02d:%02d", EndTime.ucHour, EndTime.ucMinute);

	// progressed percent
	hErr = NX_SYSTEM_GetTime(&ulCurTime);
	if (hErr == ERR_OK)
	{
		diffRef = pstEvent->duration;
		diff = ulCurTime - pstEvent->startTime;
		if(diff > diffRef) diff = diffRef;
		*pProgress = (HUINT32)(diff * 100 / diffRef);
	}

	return ERR_OK;
}


#define ______________RELATED_PF_COMMAND____________________
/***************************************************************************
Related PF Command
***************************************************************************/
#if defined(CONFIG_DEBUG)

static void		nx_pf_Command_Init(void)
{
#define cmdHandle		"nxpfcmd"

}

#endif		// #if defined(CONFIG_DEBUG)



