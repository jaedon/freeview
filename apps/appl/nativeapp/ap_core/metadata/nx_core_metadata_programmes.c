/*
 * (c) 2011-2015 Humax Co., Ltd.
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
 * @file	  		nx_core_metadata_programmes.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <nx_core_metadata_programmes.h>
#include <nx_core_system.h>
#include <thapi.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_RETURN_CHECK_PARAM(expr, err)	do{if(expr == NULL){HxLOG_Debug("[%s is NULL!!\n", #expr);return (err);}}while(0)
#define	NX_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)


/**************************************************************************************************/
#define _____________Programmes_Functions________________
/**************************************************************************************************/
static HxLANG_Id_e nx_metadata_Search_GetMainLanguage(void)
{
	HCHAR buf[4] = {'\0',};

	if (NX_PORT_SYSTEM_GetMainLanguage(buf) != ERR_OK)
		return eLangID_NONE;

	HxLOG_Debug("current language is %s\n", buf);
	return HLIB_LANG_639ToId(buf);
}

static HBOOL	nx_metadata_Search_FinedEventString(void *data, void *language)
{
	DxEventString_t *pstEventString = (DxEventString_t *)data;
	HxLANG_Id_e eLanguage = (HxLANG_Id_e)language;

	if(pstEventString && pstEventString->language == eLanguage)
		return TRUE;
	else
		return FALSE;
}

static HINT32	nx_metadata_programmes_GetSubtitleType(DxEventComponent_t *comp, HxLANG_Id_e language)
{
	if (language != eLangID_NONE && comp->language != language)
		return 0;

	if (comp->content != 0x03)
		return 0;

	if (comp->type == 0x01
		|| (0x10 <= comp->type && comp->type <= 0x14)
		|| (0x20 <= comp->type && comp->type <= 0x24))
	{
		return (int)comp->type;
	}
	return 0;
}

static HINT32	nx_metadata_programmes_GetAudioType (DxEventComponent_t *comp, HxLANG_Id_e language)
{
	if (language != eLangID_NONE && comp->language != language)
		return eNX_META_AUDIO_TYPE_UNKNOW;

	switch (comp->content)
	{
	case 0x02:
		switch(comp->type)
		{
		case 0x01: case 0x02:
			return eNX_META_AUDIO_TYPE_MONO;			//OPLPROGRAMME_AUDIOTYPE_MONO
		case 0x03:
			return eNX_META_AUDIO_TYPE_STEREO;			//OPLPROGRAMME_AUDIOTYPE_STEREO
		case 0x04: case 0x05: case 0x40: case 0x48:
			return eNX_META_AUDIO_TYPE_MULTI_CHANNEL;	//OPLPROGRAMME_AUDIOTYPE_MULIT_CHANNEL
		default:
			break;
		}
		break;

	case 0x04:
		return eNX_META_AUDIO_TYPE_MULTI_CHANNEL;		//OPLPROGRAMME_AUDIOTYPE_MULIT_CHANNEL

	case 0x06:
		switch(comp->type)
		{
		case 0x01:
			return eNX_META_AUDIO_TYPE_MONO;			//OPLPROGRAMME_AUDIOTYPE_MONO
		case 0x03: case 0x43:
			return eNX_META_AUDIO_TYPE_STEREO;			//OPLPROGRAMME_AUDIOTYPE_STEREO
		case 0x05:
			return eNX_META_AUDIO_TYPE_MULTI_CHANNEL;	//OPLPROGRAMME_AUDIOTYPE_MULIT_CHANNEL
		default:
			break;
		}
		break;

	default:
		break;
	}

	return eNX_META_AUDIO_TYPE_UNKNOW;
}

static HCHAR *nx_metadata_programmes_GetMajorGenreString(HUINT8 major)
{
	// ondk string id 에 대한 것이 마무리 되어야 사용가능.
	// 이것은 default 이다.. op 마다 달라지는것은 추가하면 된다..
	switch(major)
	{
	case 0:		return "All";		//return ONDK_GetString(STR_ALL_ID);
	case 1:		return "Movie";		//ONDK_GetString(STR_MOVIES_ID);
	case 2:		return "News";		//ONDK_GetString(STR_NEWS_ID);
	case 3:		return "Show";		//ONDK_GetString(STR_SHOW_ID);
	case 4:		return "Sports";	//ONDK_GetString(STR_SPORTS_ID);
	case 5:		return "Children";	//ONDK_GetString(STR_CHILDREN_ID);
	case 6:		return "Music";		//ONDK_GetString(STR_MUSIC_ID);
	case 7:		return "Arts";		//ONDK_GetString(STR_ARTS_ID);
	case 8:		return "Social";	//ONDK_GetString(STR_SOCIAL_ID);
	case 9:		return "Education";	//ONDK_GetString(STR_EDUCATION_ID);
	case 10:	return "Leisure";	//ONDK_GetString(STR_LEISURE_ID);
	case 11:	return "Characteristics";	//ONDK_GetString(STR_SPECIAL_CHARACTERISTICS_ID);

	default:	return NULL;
	}
}

static HCHAR *nx_metadata_programmes_GetMinorGenreString(HUINT8 minor)
{
	switch(minor)
	{
	case 0:		return "All";
	case 1:		return "Movie/Drama";
	case 2:		return "Detective/Thriller";
	case 3:		return "Adventure/Western/War";
	case 4:		return "Science Fiction/Fantasy/Horror";
	case 5:		return "Comedy";
	case 6:		return "Soap/Melodrama/Folkloric";
	case 7:		return "Romance";
	case 8:		return "Serious/Classical/Religious/Historical Movie/Drama";
	case 9:		return "Adult Movie/Drama";

	default:	return NULL;
	}
}


static void		nx_metadata_programmes_GetGenreNibble(HUINT16 content, HUINT8 *ucNibblelevel1, HUINT8 *ucNibblelevel2)
{
	HUINT8		ucContentNibble =0, ucUserNibble =0;
	HUINT8		ucMajorNibble =0, ucMinorNibble =0;
	DxOperator_e	eOperator = eDxOPERATOR_None;

	ucContentNibble = (HUINT8)(content >> 8);
	ucUserNibble = (HUINT8)(content | 0xff );
	NX_SYSTEM_GetOperatorType(&eOperator);

	switch (eOperator)
	{
		case eDxOPERATOR_Freesat:
			ucMajorNibble = ucContentNibble;
			break;

		default:
			ucContentNibble = (HUINT8)(content >> 8);
			ucUserNibble = (HUINT8)(content | 0xff ); // Not used

			ucMajorNibble = ucContentNibble >> 4;
			ucMinorNibble = ucContentNibble & 0xf;
			break;
	}

	*ucNibblelevel1 = ucMajorNibble;
	*ucNibblelevel2 = ucMinorNibble;

	HxLOG_Print("[%s,%d] major : %d \n" , __FUNCTION__ , __LINE__, ucMajorNibble);
	HxLOG_Print("[%s,%d] minor : %d \n" , __FUNCTION__ , __LINE__, ucMinorNibble);
}


static HCHAR *nx_metadata_programmes_GetGenre(HUINT16 content)
{
	HUINT8 		ucContentNibble =0, ucUserNibble =0;
	HUINT8 		ucMajorNibble =0, ucMinorNibble =0;
	HINT32		sd = 0;
	DxOperator_e	op;
#if 0
	HCHAR *dvbNibble1[] = {
		  NULL
		, "Movie/Drama"
		, "News/Current affairs"
		, "Show/ Game show"
		, "Sports"
		, "Children's/ Youth programmes"
		, "Music/Ballet/Dance"
		, "Arts/Culture (without music)"
		, "Social/Political Issues/ Economics"
		, "Education/Science/Factual Topics"
		, "Leisure hobbies"
		, "Special Characteristics"
		, NULL
		, NULL
		, NULL
		, NULL
	};
#endif
	HCHAR *freesatNibble1[] = {
		  NULL
		, "Movie"
		, "News and Factual"
		, "Entertainment"
		, "Sport"
		, "Children's"
		, "Entertainment"
		, "News and Factual"
		, "News and Factual"
		, "Education"
		, "Lifestyle"
		, NULL
		, NULL
		, NULL
		, NULL
		, "Drama"
	};

	ucContentNibble = (HUINT8)(content >> 8);
	ucUserNibble = (HUINT8)(content | 0xff );
	NX_SYSTEM_GetOperatorType(&op);

	switch(op)
	{
		case eDxOPERATOR_Freesat:
			return HLIB_STD_StrDup(freesatNibble1[ucContentNibble >> 4]);

		// TODO: Add hear OP Genre List

		default:
			ucContentNibble = (HUINT8)(content >> 8);
			ucUserNibble = (HUINT8)(content | 0xff ); // Not used

			ucMajorNibble = ucContentNibble >> 4;
			ucMinorNibble = ucContentNibble & 0xf;

			HxLOG_Print("[%s,%d] major : %d \n" , __FUNCTION__ , __LINE__, ucMajorNibble);
			HxLOG_Print("[%s,%d] minor : %d \n" , __FUNCTION__ , __LINE__, ucMinorNibble);

			sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			if( sd == 0 )
			{
				return NULL;
			}

			HxLOG_Info("\n\n\n[%s.%d] GetGenre Major (%x)(%s),  minor (%x)\n\n\n",__FUNCTION__,__LINE__,
								ucMajorNibble, nx_metadata_programmes_GetMajorGenreString(ucMajorNibble), ucMinorNibble);
			HLIB_RWSTREAM_Print(sd, "%s,%s",
							nx_metadata_programmes_GetMajorGenreString(ucMajorNibble),
							nx_metadata_programmes_GetMinorGenreString(ucMinorNibble));

			return (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}

	//return HLIB_STD_StrDup(dvbNibble1[ucContentNibble >> 4]);
}

void NX_METADATA_PROGRAMMES_DeleteProgrammes(NX_Programme_t *pstProgrammes)
{
	if (pstProgrammes)
	{
		if(pstProgrammes->event)
			NX_PORT_EVENT_Delete(pstProgrammes->event);
		NX_APP_Free(pstProgrammes);
		pstProgrammes = NULL;
	}
}

DxEPG_Type_e	NX_METADATA_PROGRAMMES_GetEpgType(NX_Programme_t *pstProgramme)
{
	DxEvent_t 	*pstEvent		= NULL;
	DxEPG_Type_e retEventType 	= eDxEPG_TYPE_NONE;

	NX_RETURN_CHECK_PARAM(pstProgramme, retEventType);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, retEventType);

	pstEvent = pstProgramme->event;
	retEventType = pstEvent->type;

	return retEventType;
}

HERROR NX_METADATA_PROGRAMMES_GetName(NX_Programme_t *pstProgramme, char *desBuf)
{
	DxEvent_t *pstEvent = NULL;
	HxLANG_Id_e language = eLangID_NONE;
	HxVector_t *name = NULL;
	DxEventString_t *eventString = NULL;
	char *retName = NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, ERR_FAIL);

	language = nx_metadata_Search_GetMainLanguage();

	pstEvent = pstProgramme->event;

	if ((pstEvent == NULL) || (pstEvent->name == NULL))
	{
		HxLOG_Error("pstEvent is NULL or name is NULL\n");
		return ERR_FAIL;
	}
	name = pstEvent->name;
	eventString = HLIB_VECTOR_Find(name, nx_metadata_Search_FinedEventString, (void*)language);

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

HERROR NX_METADATA_PROGRAMMES_GetDescription(NX_Programme_t *pstProgramme, char *desBuf)
{
	DxEvent_t *pstEvent = NULL;
	HxLANG_Id_e language = eLangID_NONE;
	HxVector_t *text = NULL;
	DxEventString_t *eventString = NULL;
	char *retText = NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, ERR_FAIL);

	language = nx_metadata_Search_GetMainLanguage();

	pstEvent = pstProgramme->event;

	if ((pstEvent == NULL) || (pstEvent->text == NULL))
	{
		HxLOG_Error("pstEvent is NULL or text is NULL\n");
		return ERR_FAIL;
	}
	text = pstEvent->text;
	eventString = HLIB_VECTOR_Find(text, nx_metadata_Search_FinedEventString, (void*)language);

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

HERROR NX_METADATA_PROGRAMMES_GetLongDescription(NX_Programme_t *pstProgramme, char *desBuf)
{
	DxEvent_t *pstEvent = NULL;
	HxLANG_Id_e language = eLangID_NONE;
	HxVector_t *text = NULL;
	DxEventString_t *eventString = NULL;
	char *retText = NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, ERR_FAIL);

	language = nx_metadata_Search_GetMainLanguage();

	pstEvent = pstProgramme->event;

	if ((pstEvent == NULL) || (pstEvent->text == NULL))
	{
		HxLOG_Error("pstEvent is NULL or text is NULL\n");
		return ERR_FAIL;
	}
	text = pstEvent->longText;

	if (text == NULL)
	{
		HxLOG_Error("long description text is NULL!!\n");
		return ERR_OK;
	}
	eventString = HLIB_VECTOR_Find(text, nx_metadata_Search_FinedEventString, (void*)language);

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

HINT32 NX_METADATA_PROGRAMMES_GetEpisode(NX_Programme_t *pstProgramme)
{
	HINT32	i = 0, n = 0, value = 0;
	DxEvent_t		*pstEvent = NULL;
	DxEventItem_t 	*item	= NULL;
	DxEPG_Type_e	epgType = NX_METADATA_PROGRAMMES_GetEpgType(pstProgramme);

	NX_RETURN_CHECK_PARAM(pstProgramme, value);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, value);

	pstEvent = pstProgramme->event;
	if (pstEvent == NULL)
	{
		NX_APP_Error("Error!!! pstEvent = NULL \n");
		return	value;
	}

	if (epgType == eDxEPG_TYPE_IP) {
		value = pstEvent->extension.ipEpg.episodeNum;
		return	value;
	}

#if defined(CONFIG_MW_EPG_TVTV)
	if (epgType == eDxEPG_TYPE_TVTV)
	{
		value = pstEvent->extension->tvtvEpg.episodeNum;
		return	value;
	}
#endif

	if (pstEvent->item)
	{
		n = HLIB_VECTOR_Length(pstEvent->item);
		for (i = 0; i < n ; i++)
		{
			item = (DxEventItem_t *)HLIB_VECTOR_ItemAt(pstEvent->item, i);
			if (item == NULL)
				continue;

			if (item->description)
			{
				if (HxSTD_StrCmp(item->description, (char*)NX_CORE_METADATA_EPISODE_DATA) == 0)
				{
					value = atoi(item->item);
					return value;
				}
			}
		}
	}
	return value;
}


HUINT8	NX_METADATA_PROGRAMMES_GetGenreNibbleLevel1(NX_Programme_t *pstProgramme)
{
	HINT32		i = 0;
	HUINT16		ulContent = 0;
	HUINT8		ucNibblelevel1 = 0, ucNibblelevel2 = 0;
	DxEvent_t	*pstEvent = NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, ucNibblelevel1);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, ucNibblelevel1);

	pstEvent = pstProgramme->event;
	for (i = 0; i < DxEVENT_CONTENT; i++)
	{
		ulContent = pstEvent->content[i];
		if (ulContent == 0)
			continue;

		nx_metadata_programmes_GetGenreNibble(ulContent, &ucNibblelevel1, &ucNibblelevel2);
		break;
	}

	return	ucNibblelevel1;
}


HUINT8	NX_METADATA_PROGRAMMES_GetGenreNibbleLevel2(NX_Programme_t *pstProgramme)
{
	HINT32		i = 0;
	HUINT16 	ulContent = 0;
	HUINT8		ucNibblelevel1 = 0, ucNibblelevel2 = 0;
	DxEvent_t	*pstEvent = NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, ucNibblelevel1);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, ucNibblelevel1);

	pstEvent = pstProgramme->event;
	for (i = 0; i < DxEVENT_CONTENT; i++)
	{
		ulContent = pstEvent->content[i];
		if (ulContent == 0)
			continue;

		nx_metadata_programmes_GetGenreNibble(ulContent, &ucNibblelevel1, &ucNibblelevel2);
		break;
	}

	return	ucNibblelevel2;
}


HxVector_t *NX_METADATA_PROGRAMMES_GetGenre(NX_Programme_t *pstProgramme)
{
	HINT32 i = 0;
	HUINT16	word;
	HCHAR *text;
	DxEvent_t	*pstEvent	= NULL;
	HxVector_t *retGenre = NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, NULL);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, NULL);

	pstEvent = pstProgramme->event;

	retGenre = HLIB_VECTOR_New(NULL, NULL);

	for (i = 0; i < DxEVENT_CONTENT; i++)
	{
		word = pstEvent->content[i];
		if (word == 0)
			continue;

		text = nx_metadata_programmes_GetGenre(word);
		if (text == NULL)
			continue;

		HLIB_VECTOR_Add(retGenre, (void *)HLIB_STD_StrDup(text));
		NX_APP_Free((void *)text);
	}

	if (HLIB_VECTOR_Length(retGenre) == 0)
	{
		HxLOG_Debug("genre data is NULL!\n");
		HLIB_VECTOR_Delete(retGenre);
		return NULL;
	}

	return retGenre;
}

HBOOL NX_METADATA_PROGRAMMES_IsHD(NX_Programme_t *pstProgramme)
{
	DxEvent_t *pstEvent = NULL;
	HINT32		i, n;
	DxEventComponent_t	*comp;

	NX_RETURN_CHECK_PARAM(pstProgramme, FALSE);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, FALSE);

	pstEvent = pstProgramme->event;

	if (pstEvent->type == eDxEPG_TYPE_SC || pstEvent->type == eDxEPG_TYPE_PF)
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

HUINT32 NX_METADATA_PROGRAMMES_GetStartTime(NX_Programme_t *pstProgramme)
{
	HUINT32		value 		= 0;
	DxEvent_t 	*pstEvent	= NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, value);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, value);

	pstEvent = pstProgramme->event;

	if (pstEvent)
	{
		value = (HUINT32)pstEvent->startTime;
	}
	return value;
}

HUINT32	NX_METADATA_PROGRAMMES_GetDuration(NX_Programme_t *pstProgramme)
{
	HUINT32		value 		= 0;
	DxEvent_t 	*pstEvent	= NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, value);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, value);

	pstEvent = pstProgramme->event;

	if (pstEvent)
	{
		value = (HUINT32)pstEvent->duration;
	}
	return value;
}

HINT32 	NX_METADATA_PROGRAMMES_GetEventID(NX_Programme_t *pstProgramme)
{
	HINT32		value 		= -1;
	DxEvent_t 	*pstEvent	= NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, value);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, value);

	pstEvent = pstProgramme->event;

	if (pstEvent)
	{
		value = (HINT32)pstEvent->eventId;
	}
	return value;
}

HINT32 	NX_METADATA_PROGRAMMES_GetSvcUID(NX_Programme_t *pstProgramme)
{
	HINT32		value 		= -1;
	DxEvent_t 	*pstEvent	= NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, value);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, value);

	pstEvent = pstProgramme->event;

	if (pstEvent)
	{
		value = (HINT32)pstEvent->svcuid;
	}
	return value;
}

HINT32	NX_METADATA_PROGRAMMES_GetParentalRating(NX_Programme_t *pstProgramme)
{
	HINT32 rating = 0;
	DxEvent_t 	*pstEvent	= NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, rating);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, rating);

	pstEvent = pstProgramme->event;

	if (pstEvent)
	{
		rating = pstEvent->parentalRating;
	}
	return rating;
}

HINT32	NX_METADATA_PROGRAMMES_GetSubtitleType(NX_Programme_t *pstProgramme)
{
	HINT32 	retSubType = 0;
	HINT32	i = 0, n = 0;
	DxEvent_t	*pstEvent	= NULL;
	DxEventComponent_t	*comp	= NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, retSubType);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, retSubType);

	pstEvent = pstProgramme->event;
	if (pstEvent->component)
	{
		n = HLIB_VECTOR_Length(pstEvent->component);
		for (i = 0; i < n ; i++)
		{
			comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(pstEvent->component, i);
			if (comp == NULL)
				continue;

			retSubType = nx_metadata_programmes_GetSubtitleType(comp, eLangID_NONE);
			if (retSubType != 0)
				return retSubType;
		}
	}
	return retSubType;
}

// 이 함수는 나중에 수정해야함.. 2015.05.13 by mykim
HINT32	NX_METADATA_PROGRAMMES_GetSubtitleTypes(NX_Programme_t *pstProgramme)
{
	HINT32	i = 0, n = 0;
	DxEvent_t	*pstEvent	= NULL;
	DxEventComponent_t	*comp	= NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, 0);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, 0);

	pstEvent = pstProgramme->event;
	if (pstEvent->component)
	{
		n = HLIB_VECTOR_Length(pstEvent->component);
		for (i = 0; i < n ; i++)
		{
			comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(pstEvent->component, i);
			if (comp == NULL)
				continue;

			switch(comp->type)
			{

			case 0x02://TELETEXT_SUBTITLE :		/* EBU subtitle */
			case 0x05://TTX_SBTL_PAGE_HERARING_IMPAIRED :
				return (HINT32)comp->type;
			 /*
		     *  DVB subtitle의 type 값은 Component descriptor의 Stream_context(0x03) & componet_type Table
		     *  값을 따른다. 여기서 componet_type 0x01, 0x02는 EBU subtitle이고, 0x10~0x13, 0x20~0x23은 DVB로
		     *  되어 있는데 실제 EBU subtitle은 Subtitle descriptor를 통해서 들어 오는 것이 아니라 Teletext
		     *  descriptor에서 type value 0x02로 들어 오는 경우만 있었으므로 위에 0x01, 0x02s는 component descriptor에
		     *  값과 다른 의미다 일단 0x2만 들어온다고 보고 이 경우만 처리하게 한다.
		     *  0x05(Teletext subtitle page for hearing impaired people)에 대한 처리도 추가 한다.
		     */
			/* Normal */
			case 0x10 :
			case 0x11 :
			case 0x12 :
			case 0x13 :
			case 0x14 : 	/* Freesat HD stream에서 여기로 들어 옴. */

			/* For the hard of hearing */
			case 0x20 :
			case 0x21 :
			case 0x22 :
			case 0x23 :
			case 0x24 :
				return (HINT32)comp->type;
			}
		}
	}
	return 0;
}

HINT32 	NX_METADATA_PROGRAMMES_GetAudioType(NX_Programme_t *pstProgramme)
{

	HINT32	type = 0x00;
	HINT32	i = 0, n = 0;
	DxEvent_t	*pstEvent	= NULL;
	HxLANG_Id_e	language;
	DxEventComponent_t	*comp	= NULL;

	NX_RETURN_CHECK_PARAM(pstProgramme, type);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, type);

	pstEvent = pstProgramme->event;

	if (pstEvent->component)
	{
		HINT32 retValue = 0;
		NX_SYSTEM_GetAudioLanuage(&retValue);
		language = (HxLANG_Id_e)retValue;

		n = HLIB_VECTOR_Length(pstEvent->component);
		for (i = 0 ; i < n ; i++)
		{
			comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(pstEvent->component, i);
			if (comp == NULL)
				continue;

			type |= nx_metadata_programmes_GetAudioType(comp, language);
		}
	}
	return type;
}


#define ___________PROGRAMMES_EXTENSION___________
HERROR	NX_METADATA_PROGRAMMES_GetDrm(NX_Programme_t *pstProgramme, HCHAR *desBuf)
{
	DxEvent_t	*pstEvent	= NULL;
	HUINT8			drm;
	DxEPG_Type_e	epgType = eDxEPG_TYPE_NONE;

	NX_RETURN_IF(desBuf == NULL, ERR_FAIL);
	NX_RETURN_CHECK_PARAM(pstProgramme, ERR_FAIL);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, ERR_FAIL);

	epgType = NX_METADATA_PROGRAMMES_GetEpgType(pstProgramme);

	pstEvent = pstProgramme->event;

	if (epgType == eDxEPG_TYPE_IP) {
		drm = pstEvent->extension.ipEpg.drm;
		NX_RETURN_IF(drm == 0, ERR_FAIL);

		HxSTD_sprintf(desBuf, "%c", drm);
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR	NX_METADATA_PROGRAMMES_GetActor(NX_Programme_t *pstProgramme, HCHAR *desBuf)
{
	DxEvent_t	*pstEvent	= NULL;
	DxEPG_Type_e	epgType = eDxEPG_TYPE_NONE;

	NX_RETURN_IF(desBuf == NULL, ERR_FAIL);
	NX_RETURN_CHECK_PARAM(pstProgramme, ERR_FAIL);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, ERR_FAIL);

	epgType = NX_METADATA_PROGRAMMES_GetEpgType(pstProgramme);

	pstEvent = pstProgramme->event;

	if (epgType == eDxEPG_TYPE_IP)
	{
		HxSTD_sprintf(desBuf, "%s", pstEvent->extension.ipEpg.actor);
		return ERR_OK;
	}

#if defined(CONFIG_MW_EPG_TVTV)
	if (epgType == eDxEPG_TYPE_TVTV)
	{
		HxSTD_sprintf(desBuf, "%s", pstEvent->extension.tvtvEpg.actor);
		return ERR_OK;
	}
#endif

	return ERR_FAIL;
}

HUINT32 NX_METADATA_PROGRAMMES_GetSeason(NX_Programme_t *pstProgramme)
{
	DxEvent_t 	*pstEvent		= NULL;
	HUINT32		unRetSeason		= 0;
	DxEPG_Type_e	epgType = NX_METADATA_PROGRAMMES_GetEpgType(pstProgramme);

	NX_RETURN_CHECK_PARAM(pstProgramme, unRetSeason);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, unRetSeason);

	pstEvent = pstProgramme->event;

	if (epgType == eDxEPG_TYPE_IP)
	{
		unRetSeason = pstEvent->extension.ipEpg.season;
	}

#if defined(CONFIG_MW_EPG_TVTV)
	if (epgType == eDxEPG_TYPE_TVTV)
	{
		unRetSeason = pstEvent->extension.tvtvEpg.season;
	}
#endif

	return	unRetSeason;
}

HUINT32	NX_METADATA_PROGRAMMES_GetSeriesID(NX_Programme_t *pstProgramme)
{
	DxEvent_t	*pstEvent	= NULL;
	HUINT32			ulSeriesId = 0;
	DxEPG_Type_e	epgType = eDxEPG_TYPE_NONE;

	NX_RETURN_CHECK_PARAM(pstProgramme, ERR_FAIL);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, ERR_FAIL);

	epgType = NX_METADATA_PROGRAMMES_GetEpgType(pstProgramme);

	pstEvent = pstProgramme->event;

	if (epgType == eDxEPG_TYPE_IP)
	{
		return pstEvent->extension.ipEpg.seriesId;
	}
#if defined(CONFIG_MW_EPG_TVTV)
	if (epgType == eDxEPG_TYPE_TVTV)
	{
		return pstEvent->extension.tvtvEpg.seriesId;
	}
#endif

	return	ulSeriesId;
}


HERROR	NX_METADATA_PROGRAMMES_GetThumbnailURL(NX_Programme_t *pstProgramme, HCHAR *desBuf)
{
	DxEvent_t	*pstEvent	= NULL;
	DxEPG_Type_e	epgType = eDxEPG_TYPE_NONE;

	NX_RETURN_IF(desBuf == NULL, ERR_FAIL);
	NX_RETURN_CHECK_PARAM(pstProgramme, ERR_FAIL);
	NX_RETURN_CHECK_PARAM(pstProgramme->event, ERR_FAIL);

	epgType = NX_METADATA_PROGRAMMES_GetEpgType(pstProgramme);

	pstEvent = pstProgramme->event;

	if (epgType == eDxEPG_TYPE_IP)
	{
#if !defined(WIN32)
		HCHAR *p = (HCHAR*)THAPI_GetThumbnailURL((const HCHAR *)pstEvent->extension.ipEpg.thumbnailURL, 0, (const HCHAR *)"/mnt/hd1", FALSE);
		HxSTD_sprintf(desBuf, "file://%s", p);
#endif
		return ERR_OK;
	}

#if defined(CONFIG_MW_EPG_TVTV)
	if (epgType == eDxEPG_TYPE_TVTV)
	{
#if !defined(WIN32)
		HCHAR *p = (HCHAR*)THAPI_GetThumbnailURL((const HCHAR *)pstEvent->extension.tvtvEpg.thumbnailURL, 0, (const HCHAR *)"/mnt/hd1/TVTV/IMAGE", FALSE);
		HxSTD_sprintf(desBuf, "file://%s", p);
#endif
		return ERR_OK;
	}
#endif

	return ERR_FAIL;
}


