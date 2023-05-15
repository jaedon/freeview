/**************************************************************************************/
/**
 * @file oplprogramme.c
 *
 * Defines DVB programme routine
 *
 * @author  ultracat(leecw@humaxdigital.com)
 * @date    2011/11/21
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>


#include "oapi.h"
#include "oplchannel.h"
#include "oplprogramme.h"
#include "oplavcomponent.h"
#include "oplmultiviewcomponent.h"

#if defined(CONFIG_OP_JAPAN)
#include "oplprogramme_japan.h"
#include "oplparser_japan.h"
#endif

#include "oplparser.h"

#include <apk.h>
#include "oplscheduler.h"
#include <thapi.h>
#if defined(CONFIG_OP_FREESAT) || defined(CONFIG_OP_UK_DTT)
#define	CONFIG_OP_FREESAT_TO_JSON
#endif


#define	return_if_void(expr)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	return_if(expr, err)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	goto_if(expr, pos)		do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

typedef struct
{
	DxEvent_t	*event;

	struct cache
	{
		HxTREE_t	*integer;
		HxTREE_t	*string;
		HxTREE_t	*collection;
	} cache;
} Programme_t;


/*
static OPL_BOOL	programme_ServiceIDFilter (OPL_Channel_t channel, void *svcid)
{
	return (OPL_Channel_GetServiceID(channel) == (int)svcid) ? OPL_TRUE : OPL_FALSE;
}
*/
#ifdef	CONFIG_OP_FREESAT
static OPL_BOOL	programme_FsatServiceIDFilter (OPL_Channel_t channel, void *svcid)
{
	return (OPL_Channel_GetFreesatServiceID(channel) == (int)svcid) ? OPL_TRUE : OPL_FALSE;
}
#endif

static OPL_BOOL	programme_HexToInt(const char *s, int *ret)
{
	int		cap;
	unsigned int	val = 0;

	while (*s != '\0')
	{
		cap = toupper(*s);
		if (cap == 'X')
		{
			s++;
			continue;
		}

		if (isdigit(*s))
			val = (val << 4) + *s - '0';
		else if ('A' <= cap && cap <= 'F')
			val = (val << 4) + cap - 'A' + 10;
		else
			return OPL_FALSE;
		s++;
	}
	if (ret)
		*ret = (int)val;
	return OPL_TRUE;
}

static OPL_BOOL	programme_ParseDVBURI (const char *uri, int *onid, int *tsid, int *sid, int *eid, char *tvaID, int *svcuid)
{
	char 	*ret;
	char	field[128];
	unsigned int	adv;

	return_if( strncmp(uri, "dvb://", 6) != 0, OPL_FALSE );
	uri += 6;

	// onid
	return_if( (ret = strchr(uri, '.')) == NULL, OPL_FALSE );
	adv = (unsigned int)ret - (unsigned int)uri;
	if (adv && onid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		return_if( !programme_HexToInt(field, onid), OPL_FALSE );
	}
	uri += adv + 1;

	// tsid
	return_if( (ret = strchr(uri, '.')) == NULL, OPL_FALSE );
	adv = (unsigned int)ret - (unsigned int)uri;
	if (adv && tsid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		return_if( !programme_HexToInt(field, tsid), OPL_FALSE );
	}
	uri += adv + 1;

	// sid
	return_if( (ret = strchr(uri, ';')) == NULL, OPL_FALSE );
	adv = (unsigned int)ret - (unsigned int)uri;
	if (adv && sid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		return_if( !programme_HexToInt(field, sid), OPL_FALSE );
	}
	uri += adv + 1;

	// eid
	if ((ret = strchr(uri, ';')) == NULL)
	{
		return_if( *uri == '\0', OPL_FALSE );
		if (eid)
		{
			return_if( !programme_HexToInt(uri, eid), OPL_FALSE );
		}
		return OPL_TRUE;
	}
	adv = (unsigned int)ret - (unsigned int)uri;
	if (adv && eid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		return_if( !programme_HexToInt(field, eid), OPL_FALSE );
	}
	uri += adv + 1;

	// TVA id
	if ((ret = strchr(uri, ';')) == NULL)
	{
		return_if( *uri == '\0', OPL_FALSE );
		if (tvaID)
		{
			strcpy(tvaID, uri);
		}
		return OPL_TRUE;
	}
	adv = (unsigned int)ret - (unsigned int)uri;
	if (adv && tvaID)
	{
		memcpy(tvaID, uri, adv); tvaID[adv] = '\0';
	}
	uri += adv + 1;

	// service_uid
	return_if( *uri == '\0', OPL_FALSE );
	if (svcuid)
		return_if( !programme_HexToInt(uri, svcuid), OPL_FALSE );
	return OPL_TRUE;
}

#ifdef	CONFIG_OP_FREESAT
static OPL_BOOL	programme_ParseFSATURI (const char *uri, int *sid, int *eid)
{
	char 	*ret;
	char	field[128];
	unsigned int	adv;

	// fsat://<service_id-digit>;<event_id-hex>
	return_if( strncmp(uri, "fsat://", 7) != 0, OPL_FALSE );
	uri += 7;

	// sid
	return_if( (ret = strchr(uri, ';')) == NULL, OPL_FALSE );
	adv = (unsigned int)ret - (unsigned int)uri;
	if (adv && sid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		//return_if( !programme_HexToInt(field, sid), OPL_FALSE );
		return_if( !HLIB_STD_IsInteger((const HCHAR *)field), OPL_FALSE );
		*sid = (int)HLIB_STD_StrToINT32((const HCHAR *)field);
	}
	uri += adv + 1;

	return_if( *uri == '\0', OPL_FALSE );
	if (eid)
		return_if( !programme_HexToInt(uri, eid), OPL_FALSE );
	return OPL_TRUE;
}
#endif

#if defined(CONFIG_OP_FREESAT_TO_JSON)
 char *	OPL_Programme_ToJsonCompatibleString (char *ch, HUINT32 n)
{
//	static const char *	s_skip = "\"\\/bfnrtu";	///< @see json.org

	HUINT32	i;
	char	*buf = NULL;
	HUINT32	pos = 0;
	HUINT32	addedCnt = 0;
	const int maxLen = 100;

#if defined(CONFIG_DEBUG)
//HxLog_Dump(ch,n,0,0);
#endif
	buf = HLIB_STD_MemAlloc(n + maxLen + 3);
	for (i = 0 ; i < n; i++)
	{
		if(addedCnt >= maxLen + 2)
		{
			free(buf);
			return NULL;
		}
		switch (ch[i])
		{
#if 0
			case '\\':

				if (strchr(s_skip, (int)ch[i + 1]))
				{
					buf[pos++] = ch[i];
					buf[pos++] = ch[i+1];
					i++;
					break;
				}
#endif
			case '\\':
			case '\"':
				buf[pos++] = (char)'\\';
				buf[pos++] = ch[i];
				addedCnt++;
				break;

			default:
				buf[pos++] = ch[i];
				break;
		}
	}
	buf[pos] = 0;

#if defined(CONFIG_DEBUG)
//HxLog_Dump(buf,pos,0,0);
#endif
	return buf;
}
#endif

static inline HxLANG_Id_e	programme_GetAudioLanguage (int num)
{
	HCHAR	buf[4];

	if (APK_SCENARIO_GetAudioLanguage(num, buf) != ERR_OK)
		return eLangID_NONE;
	return HLIB_LANG_639ToId(buf);
}

static inline HxLANG_Id_e	programme_GetMainLanguage ()
{
	HCHAR	buf[4];

	if (APK_SCENARIO_GetMainLanguage(buf) != ERR_OK)
		return eLangID_NONE;
	return HLIB_LANG_639ToId(buf);
}

static inline int	programme_GetAudioType (DxEventComponent_t *comp, HxLANG_Id_e language)
{
	if (language != eLangID_NONE && comp->language != language)
		return 0;

	switch (comp->content)
	{
	case 0x02:
		switch(comp->type)
		{
		case 0x01: case 0x02:
			return 0x01;//OPLPROGRAMME_AUDIOTYPE_MONO
		case 0x03:
			return 0x02;//OPLPROGRAMME_AUDIOTYPE_STEREO
		case 0x04: case 0x05: case 0x40: case 0x48:
			return 0x04;//OPLPROGRAMME_AUDIOTYPE_MULIT_CHANNEL
		default:
			break;
		}
		break;

	case 0x04:
		return 0x04;//OPLPROGRAMME_AUDIOTYPE_MULIT_CHANNEL

	case 0x06:
		switch(comp->type)
		{
		case 0x01:
			return 0x01;//OPLPROGRAMME_AUDIOTYPE_MONO
		case 0x03: case 0x43:
			return 0x02;//OPLPROGRAMME_AUDIOTYPE_STEREO
		case 0x05:
			return 0x04;//OPLPROGRAMME_AUDIOTYPE_MULIT_CHANNEL
		default:
			break;
		}
		break;

	default:
		break;
	}
	return 0;
}

static inline int	programme_GetSubtitleType (DxEventComponent_t *comp, HxLANG_Id_e language)
{
	if (language != eLangID_NONE && comp->language != language)
		return 0;

	if (comp->content != 0x03)
		return 0;

	if (comp->type == 0x01
		|| (0x10 <= comp->type && comp->type <= 0x14)
		|| (0x20 <= comp->type && comp->type <= 0x24))
		return (int)comp->type;
	return 0;
}

static const char *	programme_GetGenre (HUINT16 content)
{
	HUINT8 contentNibble =0;

	contentNibble = (HUINT8)(content >> 8);

	static const char	*s_dvbNibble1[] = {
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
	static const char	*s_freesatNibble1[] = {
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
	DxOperator_e	op;

	APK_SCENARIO_GetOperatorType(&op);

	switch(op)
	{
		case eDxOPERATOR_Freesat:
			return HLIB_STD_StrDup(s_freesatNibble1[contentNibble >> 4]);

		case eDxOPERATOR_JCOM:
#if defined(CONFIG_OP_JAPAN)
			return OPL_PROGRAMME_JAPAN_GetGenre( content );
#endif
			break;

		default:
			return OPL_PROGRAMME_GetGenre( content );

			break;
	}

	return HLIB_STD_StrDup(s_dvbNibble1[contentNibble >> 4]);
}

static char *programme_field_drm(Programme_t *thiz)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
	HINT32			sd;
	HULONG 			ulEpgType;
	HUINT8			drm;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);
	if (ulEpgType == eDxEPG_TYPE_IP) {
		drm = thiz->event->extension.ipEpg.drm;
		return_if(drm == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%c", drm);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;
#endif
	return NULL;
}

static char *programme_field_season(Programme_t *thiz)
{
	HINT32			sd;
	HULONG 			ulEpgType;
	HUINT32			season;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);
	if (ulEpgType == eDxEPG_TYPE_IP) {
		season = thiz->event->extension.ipEpg.season;
		return_if(season == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%d", season);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		season = thiz->event->extension.tvtvEpg.season;
		return_if(season == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%d", season);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

static char *programme_field_seriesID(Programme_t *thiz)
{
	HINT32			sd;
	HULONG 		ulEpgType;
	HUINT32			seriesId;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);
	if (ulEpgType == eDxEPG_TYPE_IP) {
		seriesId = thiz->event->extension.ipEpg.seriesId;
		return_if(seriesId == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%d", seriesId);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		seriesId = thiz->event->extension.tvtvEpg.seriesId;
		return_if(seriesId == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%d", seriesId);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

static char *programme_field_thumbnailURL(Programme_t *thiz)
{
	HINT32			sd;
	HULONG 			ulEpgType;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);
	if (ulEpgType == eDxEPG_TYPE_IP) {
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		{
			HCHAR	*p;

#if defined(CONFIG_SUPPORT_IPEPG_CH_LOGO_STORE_HDD)
			p = (HCHAR *)THAPI_GetThumbnailURL((const HCHAR *)thiz->event->extension.ipEpg.thumbnailURL, 0, (const HCHAR *)"/mnt/hd1", FALSE);
#else
			p = (HCHAR *)THAPI_GetThumbnailURL((const HCHAR *)thiz->event->extension.ipEpg.thumbnailURL, 0, (const HCHAR *)"/var/lib/humaxtv_user", FALSE);
#endif
			if (p)
			{
				HLIB_RWSTREAM_Print(sd, "file://%s", p);

				return HLIB_RWSTREAM_CloseWithoutBuf(sd);
			}

			//	THAPI Result == NULL이면 기본 URL을 돌려줌.
			HLIB_RWSTREAM_Close(sd);
			return HLIB_STD_StrDup(thiz->event->extension.ipEpg.thumbnailURL);
		}
//		HLIB_RWSTREAM_Print(sd, "%s", THAPI_GetThumbnailURL(thiz->event->extension.ipEpg.thumbnailURL, 0, "/mnt/hd1"));
//		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		{
			HCHAR	*p;
#if defined(CONFIG_MW_EPG_TVTV_NAND_STORAGE)
			p = (HCHAR *)THAPI_GetThumbnailURL((const HCHAR *)thiz->event->extension.tvtvEpg.thumbnailURL, 0, (const HCHAR *)"/var/lib/humaxtv_user/TVTV/IMAGE", FALSE);
#else
			p = (HCHAR *)THAPI_GetThumbnailURL((const HCHAR *)thiz->event->extension.tvtvEpg.thumbnailURL, 0, (const HCHAR *)"/mnt/hd1/TVTV/IMAGE", FALSE);
#endif
			if (p)
			{
				HLIB_RWSTREAM_Print(sd, "file://%s", p);

				return HLIB_RWSTREAM_CloseWithoutBuf(sd);
			}

			//	THAPI Result == NULL이면 기본 URL을 돌려줌.
			HLIB_RWSTREAM_Close(sd);
			return HLIB_STD_StrDup(thiz->event->extension.tvtvEpg.thumbnailURL);
		}
//		HLIB_RWSTREAM_Print(sd, "%s", THAPI_GetThumbnailURL(thiz->event->extension.ipEpg.thumbnailURL, 0, "/mnt/hd1"));
//		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

static char *programme_field_actor(Programme_t *thiz)
{
	HINT32			sd;
	HULONG 			ulEpgType;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);
	if (ulEpgType == eDxEPG_TYPE_IP){
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.ipEpg.actor);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.tvtvEpg.actor);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

static char *programme_field_actress(Programme_t *thiz)
{
	HINT32			sd;
	HULONG 			ulEpgType;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	if (ulEpgType == eDxEPG_TYPE_IP) {
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.ipEpg.actress);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.tvtvEpg.actress);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

static char *programme_field_director(Programme_t *thiz)
{
	HINT32			sd;
	HULONG 			ulEpgType;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	if (ulEpgType == eDxEPG_TYPE_IP) {
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.ipEpg.director);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.tvtvEpg.director);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);

	}
	(void)thiz;

	return NULL;
}

static char *programme_field_tvtvid(Programme_t *thiz)
{
	HINT32			sd;
	HULONG 			ulEpgType;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%d", thiz->event->extension.tvtvEpg.tvtvId);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);

	}
	(void)thiz;

	return NULL;
}

static char *programme_field_person(Programme_t *thiz)
{
	HINT32			sd;
	HULONG 			ulEpgType;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	if (ulEpgType == eDxEPG_TYPE_IP) {
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.ipEpg.person);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.tvtvEpg.person);
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

HERROR	programme_CheckMultiViewComponentIndex( Programme_t *thiz , HUINT8 ucCompTag, HUINT32 ulCurNum, HINT32 *nRetMultiCompNum )
{
	HERROR						hErr = ERR_FAIL;
#if defined(CONFIG_OP_JAPAN)
	APKS_EventComponentGroupDes_t	*multiViewComp = NULL;
	HUINT32						i =0 , j = 0, k =0, count =0, ulMulCount =0;
	HxVector_t					*descList = NULL;
#endif
	*nRetMultiCompNum = 0;

	if (NULL == thiz)
	{
		HxLOG_Error("[%s] Invalid Argument is NULL\n");
		return ERR_FAIL;
	}

	/* input data */
	HxLOG_Trace();
	return_if(!thiz, ERR_FAIL);
	return_if(thiz->event == NULL, ERR_FAIL);
	return_if(thiz->event->descriptor == NULL, ERR_FAIL);

#if defined(CONFIG_OP_JAPAN)
	descList = OPL_PARSER_JAPAN_ParsingDesc( thiz->event->descriptor, TAG_COMPONENT_GROUP );
	if( descList != NULL )
	{
		count = HLIB_VECTOR_Length(descList);
		if(  count > 0 )
		{/* 1개 뿐이 없다. */
			multiViewComp = (APKS_EventComponentGroupDes_t	*)HLIB_VECTOR_ItemAt( descList , 0 );
			if( (multiViewComp != NULL) && (multiViewComp->ucNumOfGroup > 0))
			{
				for( i =0; i< multiViewComp->ucNumOfGroup; i++ )
				{
					/* copy */
					for( j =0; j< multiViewComp->pstGroup[i].ucNumOfCaUnit; j++ )
					{
						for( k =0; k< multiViewComp->pstGroup[i].pstCaUnit[j].ucNumOfComponent; k++)
						{
							if( ucCompTag == multiViewComp->pstGroup[i].pstCaUnit[j].pucComponentTag[k] )
							{
								if( ulMulCount == ulCurNum )
								{
									*nRetMultiCompNum = i + 1;
									hErr = ERR_OK;
									break;
								}
								else
								{
									ulMulCount++;
								}
							}
						}

						if( k < multiViewComp->pstGroup[i].pstCaUnit[j].ucNumOfComponent )
						{
							break;
						}
					}

					if( j < multiViewComp->pstGroup[i].ucNumOfCaUnit )
					{
						break;
					}
				}

				if( i >= multiViewComp->ucNumOfGroup )
				{/* not found */
					hErr = ERR_FAIL;
				}
			}
		}
	}

	if( descList )
	{
		HLIB_VECTOR_Delete( descList );
	}
#endif

	return hErr;
}

OPL_Collection_t	*programme_GetVideoComponent( Programme_t *thiz )
{
	HINT32				i =0, n =0, tempMultiView =0;
	OPL_Collection_t		*coll = NULL;
	DxEventComponent_t	*videoComp = NULL;
	OplAVComponent_t		*tempAVComp = NULL;
	HERROR				hErr = ERR_FAIL;

	if (NULL == thiz)
	{
		HxLOG_Error("[%s] Invalid Argument is NULL\n");
		return NULL;
	}

	/* input data */
	HxLOG_Trace();
	return_if(!thiz, 0);
	return_if(thiz->event == NULL, 0);
	return_if(thiz->event->component == NULL, 0);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "videocomponent");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);
	return_if(coll == NULL, NULL);

	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		/* get */
		videoComp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (videoComp == NULL)
			continue;

		/* alloc */
		tempAVComp = NULL;
		tempAVComp = (OplAVComponent_t*)HLIB_STD_MemAlloc(sizeof(OplAVComponent_t));
		if( tempAVComp == NULL )
			continue;

		memset( tempAVComp, 0x00, sizeof( OplAVComponent_t ) );

		/* copy */
		tempAVComp->eType = OPLAVCOMPONENT_TYPE_VIDEO;
		tempAVComp->ulComponentTag = videoComp->tag;
		tempAVComp->ulComponentType = videoComp->type;
#if defined(CONFIG_OP_JAPAN)
		tempAVComp->fAspectRatio = OPL_PROGRAMME_JAPAN_GetVideoAspectRatio( tempAVComp->ulComponentType );
#endif
		if( videoComp->text )
		{
			HLIB_STD_StrNCpySafe( tempAVComp->szText,  videoComp->text , OPLAVCOMPONENT_TEXT_SIZE );
		}

		do{
			tempMultiView = 0;
			hErr = programme_CheckMultiViewComponentIndex( thiz, tempAVComp->ulComponentTag, tempAVComp->nMultiViewNum, &tempMultiView );
			if( (hErr == ERR_OK) && (tempMultiView > 0) )
			{
				tempAVComp->szMultiView[tempAVComp->nMultiViewNum][0] = '0' + tempMultiView;
				tempAVComp->szMultiView[tempAVComp->nMultiViewNum++][1] = '\0';
			}
		}while( hErr == ERR_OK );

		/* input */
		(void) OPL_Collection_Add(coll, (void *)tempAVComp);
	}

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, "videocomponent", coll);

	return coll;
}

OPL_Collection_t	*programme_GetAudioComponent( Programme_t *thiz )
{
	OPL_Collection_t				*coll = NULL;
#if defined(CONFIG_OP_JAPAN)
	HxVector_t					*descList = NULL;
	HERROR						hErr = ERR_FAIL;
	OplAVComponent_t				*tempAVComp = NULL;
	APKS_EventAudioComponent_t	*audioComp = NULL;
	HINT32						tempMultiView =0, count = 0, i =0;
	const HCHAR 					*tempLang = NULL;
#endif
	if (NULL == thiz)
	{
		HxLOG_Error("[%s] Invalid Argument is NULL\n");
		return NULL;
	}

	/* input data */
	HxLOG_Trace();
	return_if(!thiz, 0);
	return_if(thiz->event == NULL, 0);
	return_if(thiz->event->descriptor == NULL, 0);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "audiocomponent");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);
	return_if(coll == NULL, NULL);

#if defined(CONFIG_OP_JAPAN)
	descList = OPL_PARSER_JAPAN_ParsingDesc( thiz->event->descriptor, TAG_AUDIO_COMPONENT );
	if( descList != NULL )
	{
		count = HLIB_VECTOR_Length(descList);
		if(  count > 0 )
		{
			for( 	i=0; i< count; i++ )
			{
				audioComp = (APKS_EventAudioComponent_t *)HLIB_VECTOR_ItemAt( descList, i );
				if( audioComp == NULL )
					continue;

				/* alloc */
				tempAVComp = NULL;
				tempAVComp = (OplAVComponent_t*)HLIB_STD_MemAlloc(sizeof(OplAVComponent_t));
				if( tempAVComp == NULL )
					continue;
				memset( tempAVComp, 0x00, sizeof( OplAVComponent_t ) );

				/* copy */
				tempAVComp->eType = OPLAVCOMPONENT_TYPE_AUDIO;
				tempAVComp->ulComponentTag = audioComp->ucComponentTag;
				tempAVComp->ulComponentType = audioComp->ucComponentType;
				tempAVComp->ulStreamType = audioComp->ucStreamType;

				if( (audioComp->ucStreamContent == 0x02) && (audioComp->ucComponentType == 0x40) )
				{/* arib std-b10 6.2 Descriptor coding ( 101 page )*/
					tempAVComp->bAudioDescription = TRUE;
				}
				else
				{
					tempAVComp->bAudioDescription = FALSE;
				}

				switch( audioComp->ucComponentType )
				{
					case 0x01:
					case 0x02:
						tempAVComp->ulAudioChannels = 1;
						break;

					case 0x03:
						tempAVComp->ulAudioChannels = 2;
						break;

					case 0x04:
					case 0x05:
						tempAVComp->ulAudioChannels = 3;
						break;

					case 0x06:
					case 0x07:
						tempAVComp->ulAudioChannels = 4;
						break;

					case 0x08:
					case 0x09:
						tempAVComp->ulAudioChannels = 5;
						break;

					default:
						tempAVComp->ulAudioChannels = 0;
						break;
				}

				if ( audioComp->ucTextCharLen > 0)
				{
					HLIB_STD_StrNCpySafe( tempAVComp->szText , (HCHAR *)audioComp->pucTextChar , OPLAVCOMPONENT_TEXT_SIZE );
				}

				if( audioComp->unISO639LanguageCode )
				{
					tempLang = NULL;
					tempLang = OPL_PROGRAMME_JAPAN_IntTo639( audioComp->unISO639LanguageCode );
					if ( tempLang != NULL )
					{
						HxSTD_StrNCpy( tempAVComp->szLanguage, tempLang,  OPLAVCOMPONENT_LANG_SIZE );
					}
				}

				if( audioComp->unISO639LanguageCode2 )
				{
					tempLang = NULL;
					tempLang = OPL_PROGRAMME_JAPAN_IntTo639( audioComp->unISO639LanguageCode2 );
					if ( tempLang != NULL )
					{
						HxSTD_StrNCpy( tempAVComp->szLanguage2, tempLang,  OPLAVCOMPONENT_LANG_SIZE );
					}
				}

				do{
					tempMultiView = 0;
					hErr = programme_CheckMultiViewComponentIndex( thiz, tempAVComp->ulComponentTag, tempAVComp->nMultiViewNum, &tempMultiView );
					if( (hErr == ERR_OK) && (tempMultiView > 0) )
					{
						tempAVComp->szMultiView[tempAVComp->nMultiViewNum][0] = '0' + tempMultiView;
						tempAVComp->szMultiView[tempAVComp->nMultiViewNum++][1] = '\0';
					}
				}while( hErr == ERR_OK );

				/* input */
				(void) OPL_Collection_Add(coll, (void *)tempAVComp);

				audioComp = NULL;
			}
		}
	}

	if( descList )
	{
		HLIB_VECTOR_Delete( descList );
	}
#endif

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}

	HLIB_TREE_Insert(thiz->cache.collection, "audiocomponent", coll);

	return coll;
}


OPL_Collection_t	*programme_GetDataComponent( Programme_t *thiz, eOplAVComponentType eType  )
{
	OPL_Collection_t				*coll = NULL;
	HCHAR						*inputString = NULL;
#if defined(CONFIG_OP_JAPAN)
	HxVector_t					*descList = NULL;
	HERROR 						hErr = ERR_FAIL;
	OplAVComponent_t				*tempAVComp = NULL;
	APKS_EventDataContentDes_t		*dataComp = NULL;
	HINT32						 tempMultiView =0, count =0, i=0;
	const HCHAR 					*tempLang = NULL;
#endif

	if (NULL == thiz)
	{
		HxLOG_Error("[%s] Invalid Argument is NULL\n");
		return NULL;
	}

	/* input data */
	HxLOG_Trace();
	return_if(!thiz, 0);
	return_if(thiz->event == NULL, 0);
	return_if(thiz->event->descriptor == NULL, 0);

	switch( eType )
	{
		case OPLAVCOMPONENT_TYPE_DATA:
			inputString = "datacomponent";
			break;
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			inputString = "subtitlecomponent";
			break;

		default:
			return NULL;
			break;
	}

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, inputString);
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);
	return_if(coll == NULL, NULL);

#if defined(CONFIG_OP_JAPAN)
	descList = OPL_PARSER_JAPAN_ParsingDesc( thiz->event->descriptor, TAG_DATA_CONTENTS );
	if( descList != NULL )
	{
		count = HLIB_VECTOR_Length(descList);
		if(  count > 0 )
		{
			for( i =0; i<count; i++ )
			{
				dataComp = (APKS_EventDataContentDes_t *)HLIB_VECTOR_ItemAt(descList, i);
				if( dataComp == NULL )
					continue;

				switch( dataComp->usDataComponentId )
				{
					case eAribDataComId_ARIB_XML_base_multimedia:
					case eAribDataComId_Multimedia_DTT_A:
						if( eType != OPLAVCOMPONENT_TYPE_DATA )
							break;

						if( (dataComp->ucEntryComponent < 0x40) || (dataComp->ucEntryComponent	> 0x4f) )
						{/* 원래 40 ~ 7f 까지인데,, 기존 모델 JC 시리즈가 4f까지만 해서 동일하게 한다. */
							continue;
						}

						tempAVComp = NULL;
						tempAVComp = (OplAVComponent_t*)HLIB_STD_MemAlloc(sizeof(OplAVComponent_t));
						if( tempAVComp == NULL )
							continue;

						memset( tempAVComp, 0x00, sizeof( OplAVComponent_t ) );

						/* copy */
						tempAVComp->eType = OPLAVCOMPONENT_TYPE_DATA;
						tempAVComp->bAssociatedContentFlag = (HBOOL)dataComp->pstAribBxmlInfo->ucAssociatedContentsFlag;
						tempAVComp->bDataEntryComponentFlag = TRUE;
						tempAVComp->ulComponentTag = dataComp->ucEntryComponent;
						tempAVComp->nDigitalCopyControl = OPL_Programme_GetCopyControl( (OPL_ProgrammeHandle)thiz, DIGITAL_COPY_TYPE );

						if( dataComp->ucTextLen > 0 )
						{
							HLIB_STD_StrNCpySafe( tempAVComp->szText,	(HCHAR *)dataComp->pucTextChar , OPLAVCOMPONENT_TEXT_SIZE );
						}

						do{
							tempMultiView = 0;
							hErr = programme_CheckMultiViewComponentIndex( thiz, tempAVComp->ulComponentTag, tempAVComp->nMultiViewNum, &tempMultiView );
							if( (hErr == ERR_OK) && (tempMultiView > 0) )
							{
								tempAVComp->szMultiView[tempAVComp->nMultiViewNum][0] = '0' + tempMultiView;
								tempAVComp->szMultiView[tempAVComp->nMultiViewNum++][1] = '\0';
							}
						}while( hErr == ERR_OK );

						/* input */
						(void) OPL_Collection_Add(coll, (void *)tempAVComp);
						break;

					case eAribDataComId_ARIB_Sub_txt_coding:
						if( eType != OPLAVCOMPONENT_TYPE_SUBTITLE )
							break;

						tempAVComp = NULL;
						tempAVComp = (OplAVComponent_t*)HLIB_STD_MemAlloc(sizeof(OplAVComponent_t));
						if( tempAVComp == NULL )
							continue;

						memset( tempAVComp, 0x00, sizeof( OplAVComponent_t ) );

						/* copy */
						tempAVComp->eType = OPLAVCOMPONENT_TYPE_SUBTITLE;
						tempAVComp->ulComponentTag = dataComp->ucEntryComponent;

						if( (dataComp->pstAribCaptionInfo->ucNumLanguages > 0) && dataComp->pstAribCaptionInfo->punISO639LanguageCode[0] )
						{
							tempLang = NULL;
							tempLang = OPL_PROGRAMME_JAPAN_IntTo639( dataComp->pstAribCaptionInfo->punISO639LanguageCode[0] );
							if ( tempLang != NULL )
							{
								HLIB_STD_StrNCpySafe( tempAVComp->szLanguage, tempLang,  OPLAVCOMPONENT_LANG_SIZE );
							}
						}

						/* input */
						(void) OPL_Collection_Add(coll, (void *)tempAVComp);
						break;

					default:
						break;
				}
			}
		}
	}

	if( descList )
	{
		HLIB_VECTOR_Delete( descList );
	}
#endif

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}

	HLIB_TREE_Insert(thiz->cache.collection, inputString, coll);

	return coll;
}

#if defined(CONFIG_OP_JAPAN)
HINT32 programme_GetPmtRating( Programme_t *thiz)
{
	HERROR						hErr = ERR_FAIL;
	APKE_MediaCtrl_Destination_e	eDest = APK_MEDIACTRL_GetMainViewDestination();
	APKS_MEDIADEC_SIRAWTABLE_t	stSiRawTable;
	HxList_t						*pstSiRawList = NULL;
	HINT32						nRetVal = -1;
	HxVector_t 					*list = NULL;
	HxVector_t					*dataCaList = NULL;
	HUINT32						ulCurTime = 0;
	HINT32						curSvcUid = 0, dataCount =0, i =0;
	APKS_MEDIADEC_SIRAWSEC_t	*pstData= NULL;
	APKS_CaDes_t					*pstCaData = NULL;

	HxLOG_Trace();

	return_if(!thiz, nRetVal);
	return_if(thiz->event == NULL, nRetVal);

	/* 현재 live의 현재 시간인지 확인 */
	curSvcUid = APK_MEDIACTRL_LIVE_GetCurrentMasterServiceUid( eDest ) ;

	ulCurTime = HLIB_STD_GetSystemTime();

	if( 	( thiz->event->svcuid != curSvcUid ) ||
		( thiz->event->startTime >= ulCurTime) ||
		( (thiz->event->startTime + thiz->event->duration) < ulCurTime ))
	{
		return nRetVal;
	}

	memset( &stSiRawTable , 0x00 , sizeof( APKS_MEDIADEC_SIRAWTABLE_t ));
	hErr = APK_MEDIACTRL_LIVE_GetSiRawTable( eDest, eApkMctrlSiTableType_Pmt, &stSiRawTable);
	if( hErr == ERR_OK && stSiRawTable.pstSecList != NULL )
	{

		pstSiRawList = stSiRawTable.pstSecList;

		list = HLIB_VECTOR_NewEx(NULL, 0, NULL, NULL, NULL);
		return_if(list == NULL, nRetVal);

		while( pstSiRawList )
		{
			pstData = (APKS_MEDIADEC_SIRAWSEC_t *)pstSiRawList->data;
			if( pstData == NULL )
				continue;

			hErr = OPL_PARSER_JAPAN_GetPmtProgramDescriptorFromRaw( pstData->pucRawSec,
					TAG_CA , 0 , list );

			if( HLIB_VECTOR_Length( list ) > 0  )
			{
				dataCaList = OPL_PARSER_JAPAN_ParsingDesc( list, TAG_CA );
				if( dataCaList != NULL )
				{
					dataCount = HLIB_VECTOR_Length(dataCaList);
					for( i =0; i< dataCount; i++)
					{
						pstCaData = (APKS_CaDes_t *)HLIB_VECTOR_ItemAt( dataCaList , i );
						if( pstCaData != NULL )
						{
							if(pstCaData->pPrivateDataInfo != NULL)
							{
								if(pstCaData->pPrivateDataInfo->ucPrivateDataLen > 0)
								{
									nRetVal = pstCaData->pPrivateDataInfo->pPrivateData[0];
 									break;
								}
							}

}
						pstCaData = NULL;
					}
				}

				if( dataCaList )
				{
					HLIB_VECTOR_Delete( dataCaList );
				}
				break;
			}

			pstSiRawList = pstSiRawList->next;
		}

		if( list )
			HLIB_VECTOR_Delete(list);
	}
	else
	{
		HxLOG_Error("[%s,%d] get fail si pmt data \n", __FUNCTION__ , __LINE__ );
	}

	HxLOG_Print( "[%s,%d] retval : %d \n" , __FUNCTION__ ,__LINE__, nRetVal );

	return nRetVal;
}


/* spec에 따라 나온결과로 화면표시에 맞게 변경 해준다. */
HINT32 programme_makeCopyControl( AribCpInfo_t stAribCpInfo, eCopyControlType eType)
{
	AribCpOutputInfo_t			stCpOutput;
	HINT32					nRetVal = -1;

	memset( &stCpOutput, 0x00, sizeof( AribCpOutputInfo_t) );

	OPL_PROGRAMME_JAPAN_InterpretCopyProtectInfo( stAribCpInfo , FALSE , &stCpOutput );

	HxLOG_Print( "[%s,%d]analog : %d , digital : %d \n" , __FUNCTION__, __LINE__,stCpOutput.eAnalogOutput, stCpOutput.eDigitalOutput );

	if( eType == ANALOG_COPY_TYPE )
	{
		/* analog setting */
		switch( stCpOutput.eAnalogOutput )
		{
			case ARIB_COPY_FREE:
			case ARIB_COPY_NEVER:
				nRetVal = stCpOutput.eAnalogOutput;
				break;

			case ARIB_COPY_ONCE:
			case ARIB_COPY_NOMORE:
			case ARIB_COPY_10:
				nRetVal = ARIB_COPY_NEVER;
				break;

			default:
				/* 기존 모델에서도 default 상태일때 Free상태로 갖고 있어서 이렇게 처리한다. */
				nRetVal = ARIB_COPY_FREE;
				break;
		}

	}
	else if( eType == DIGITAL_COPY_TYPE )
	{

		switch( stCpOutput.eDigitalOutput  )
		{
			case ARIB_COPY_FREE:
			case ARIB_COPY_ONCE:
			case ARIB_COPY_NEVER:
			case ARIB_COPY_10:
				nRetVal = stCpOutput.eDigitalOutput;
				break;

			default:
				/* 기존 모델에서도 default 상태일때 Free상태로 갖고 있어서 이렇게 처리한다. */
				nRetVal = ARIB_COPY_FREE;
				break;
		}
	}
	else
	{
		nRetVal = -1;
	}

	return nRetVal;
}

HINT32 programme_GetCopyControl(HxVector_t *descriptor , HINT32 svcuid, eCopyControlType eType )
{
	HINT32							nRetVal = -1;
	HERROR							hErr = ERR_FAIL;
	HxVector_t						*dataDigitList = NULL, *dataAvailList = NULL;
	APKS_EventDigitalCopyControl_t		*dataDigitalCopy = NULL;
	APKS_EventContentAvailabilityDes_t	*dataAvailDes = NULL;
	AribCpInfo_t						stAribCpInfo;
	OPL_Channel_t						stChannel;
	DxSvcType_e						eSvcType = eDxSVC_TYPE_TV;
	HINT32							nOrgSvcType = 0, nOnId = 0, digitCount =0, availCount =0;

	HxLOG_Trace();

	return_if(descriptor == NULL, 0);

	memset( &stAribCpInfo, 0x00, sizeof(AribCpInfo_t) );
	memset( &stChannel, 0x00, sizeof( OPL_Channel_t) );

	/* copy control 정보 */
	dataDigitList = OPL_PARSER_JAPAN_ParsingDesc( descriptor, TAG_DIGITAL_COPY_CONTROL );
	if( dataDigitList != NULL )
	{
		digitCount = HLIB_VECTOR_Length(dataDigitList);
		if(  digitCount > 0 )
		{/* 1개 뿐이 없다. */
			dataDigitalCopy = (APKS_EventDigitalCopyControl_t *)HLIB_VECTOR_ItemAt( dataDigitList , 0 );

			if( dataDigitalCopy != NULL )
			{
				stAribCpInfo.ucAnalogRecording = dataDigitalCopy->ucAPSControlData;
				stAribCpInfo.ucCopyControlType =dataDigitalCopy->ucCopyControlType;
				stAribCpInfo.ucDigitalRecording =dataDigitalCopy->ucDigitalRecordingControlData;

}
		}
	}

	if( dataDigitList )
	{
		HLIB_VECTOR_Delete( dataDigitList );
	}

	if( digitCount > 0 ) /* copy 정보가 있을 때만 다음 단계로 흘려줘야 한다. */
	{
		/* svc type 정보 */
		stChannel = OPL_Channel_FindChannelByUID(NULL, (int)svcuid);
		if (stChannel)
		{
			nOrgSvcType = OPL_Channel_GetOrgSvcType( stChannel );
			hErr = OPL_PROGRAMME_JAPAN_GetSvcType((HUINT32)nOrgSvcType, (HINT32 *)&eSvcType);
			if( hErr != ERR_OK )
			{/* error is default TV */
				eSvcType = eDxSVC_TYPE_TV;
			}
			nOnId = OPL_Channel_GetOrgNetworkID( stChannel );
		}
		stAribCpInfo.usSvcType = (HUINT16)eSvcType;

		/* availity 정보 */
		dataAvailList = OPL_PARSER_JAPAN_ParsingDesc( descriptor, TAG_CONTENT_AVAILABILITY );
		if( dataAvailList != NULL )
		{
			availCount = HLIB_VECTOR_Length(dataAvailList);
			if(  availCount > 0 )
			{/* 1개 뿐이 없다. */
				dataAvailDes = (APKS_EventContentAvailabilityDes_t *)HLIB_VECTOR_ItemAt( dataAvailList , 0 );
			}
		}

		if( availCount > 0 && dataAvailDes != NULL )
		{
			stAribCpInfo.ucCopyRestrictionMode = dataAvailDes->ucCopyRestrictionMode;
			stAribCpInfo.ucImageContraintToken = dataAvailDes->ucImageContraintToken;
			stAribCpInfo.ucRetentionMode = dataAvailDes->ucRetentionMode;
			stAribCpInfo.ucRetentionState = dataAvailDes->ucRetentionState;
			stAribCpInfo.ucEncryptionMode = dataAvailDes->ucEncryptionMode;
		}
		else
		{
			if( nOnId == 0xFFFE || nOnId == 0xFFFA ||  nOnId == 0xFFFD || nOnId == 0xFFF9 )
			{/* eJpNetType_CATV || eJpNetType_JCHITS */
				stAribCpInfo.ucCopyRestrictionMode = 0;
			}
			else
			{
				stAribCpInfo.ucCopyRestrictionMode = 1;
			}

			stAribCpInfo.ucImageContraintToken = 1;
			stAribCpInfo.ucRetentionMode = 0;
			stAribCpInfo.ucRetentionState = 7;
			stAribCpInfo.ucEncryptionMode = 1;
		}

		if( dataAvailList )
		{
			HLIB_VECTOR_Delete( dataAvailList );
		}

		nRetVal = programme_makeCopyControl( stAribCpInfo , eType );
	}

	HxLOG_Print("\n[%s,%d]nRetVal : %d \n" , __FUNCTION__, __LINE__, nRetVal);

	return nRetVal;
}

HINT32 programme_GetPmtCopyControl( Programme_t *thiz, eCopyControlType eType )
{
	HERROR						hErr = ERR_FAIL;
	APKE_MediaCtrl_Destination_e	eDest = APK_MEDIACTRL_GetMainViewDestination();
	APKS_MEDIADEC_SIRAWTABLE_t	stSiRawTable;
	HxList_t						*pstSiRawList = NULL;
	HINT32						nRetVal = -1;
	HxVector_t *					list = NULL;
	HUINT32						ulCurTime = 0;
	HINT32						curSvcUid = 0;
	APKS_MEDIADEC_SIRAWSEC_t	*pstData= NULL;

	HxLOG_Trace();

	return_if(!thiz, nRetVal);
	return_if(thiz->event == NULL, nRetVal);

	/* 현재 live의 현재 시간인지 확인 */
	curSvcUid = APK_MEDIACTRL_LIVE_GetCurrentMasterServiceUid( eDest ) ;

	ulCurTime = HLIB_STD_GetSystemTime();

	if( 	( thiz->event->svcuid != curSvcUid ) ||
		( thiz->event->startTime > ulCurTime) ||
		( (thiz->event->startTime + thiz->event->duration) < ulCurTime ))
	{
		return nRetVal;
	}

	memset( &stSiRawTable , 0x00 , sizeof( APKS_MEDIADEC_SIRAWTABLE_t ));
	hErr = APK_MEDIACTRL_LIVE_GetSiRawTable( eDest, eApkMctrlSiTableType_Pmt, &stSiRawTable);
	if( hErr == ERR_OK && stSiRawTable.pstSecList != NULL )
	{

		pstSiRawList = stSiRawTable.pstSecList;

		list = HLIB_VECTOR_NewEx(NULL, 0, NULL, NULL, NULL);
		return_if(list == NULL, nRetVal);

		while( pstSiRawList )
		{
			/* 1개 뿐이다. */
			pstData = (APKS_MEDIADEC_SIRAWSEC_t *)pstSiRawList->data;
			if( pstData == NULL )
				continue;

			hErr = OPL_PARSER_JAPAN_GetPmtProgramDescriptorFromRaw( pstData->pucRawSec,
					TAG_DIGITAL_COPY_CONTROL , 0 , list );

			hErr |= OPL_PARSER_JAPAN_GetPmtProgramDescriptorFromRaw( pstData->pucRawSec,
					TAG_CONTENT_AVAILABILITY , 0 , list );

			if( HLIB_VECTOR_Length( list ) > 0  )
			{
				nRetVal= programme_GetCopyControl( list, thiz->event->svcuid, eType );
			}

			pstSiRawList = pstSiRawList->next;
		}

		if( list )
			HLIB_VECTOR_Delete(list);
	}
	else
	{
		HxLOG_Error("[%s,%d] get fail si pmt data \n", __FUNCTION__ , __LINE__ );
	}

	HxLOG_Print( "[%s,%d] retval : %d \n" , __FUNCTION__ ,__LINE__, nRetVal );

	return nRetVal;
}

HINT32 programme_GetSdtCopyControl( Programme_t *thiz, eCopyControlType eType )
{
	HINT32						nRetVal = -1;
	DxDes_t						*pstDes = NULL;
	OPL_Channel_t					stChannel;
	HINT32						nOnId = 0, nSvcId = 0, nOrgSvcType = 0;
	AribCpInfo_t					stAribCpInfo;
	HERROR						hErr = ERR_FAIL;
	DxSvcType_e					eSvcType = eDxSVC_TYPE_TV;

	HxLOG_Trace();

	return_if(!thiz, nRetVal);
	return_if(thiz->event == NULL, nRetVal);

	memset( &stAribCpInfo, 0x00, sizeof(AribCpInfo_t) );
	memset( &stChannel, 0x00, sizeof( OPL_Channel_t) );

	stChannel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);

	if( stChannel )
	{
		nOnId = OPL_Channel_GetOrgNetworkID( stChannel );
		nSvcId = OPL_Channel_GetServiceID( stChannel );

		pstDes = APK_META_DES_GetWithNetIdAndSvcId( nOnId , nSvcId );
		if( pstDes )
		{
			if( pstDes->bDigitalCopyControlDes )
			{
				stAribCpInfo.ucAnalogRecording = pstDes->astDigitalCopyControlDes[0].ucAPSControlData;
				stAribCpInfo.ucCopyControlType =pstDes->astDigitalCopyControlDes[0].ucCopyControlType;
				stAribCpInfo.ucDigitalRecording =pstDes->astDigitalCopyControlDes[0].ucDigitalRecordingControlData;

				nOrgSvcType = OPL_Channel_GetOrgSvcType( stChannel );
				hErr = OPL_PROGRAMME_JAPAN_GetSvcType((HUINT32)nOrgSvcType, (HINT32 *)&eSvcType);
				if( hErr != ERR_OK )
				{/* error is default TV */
					eSvcType = eDxSVC_TYPE_TV;
				}
				stAribCpInfo.usSvcType = (HUINT16)eSvcType;

				if( nOnId == 0xFFFE || nOnId == 0xFFFA ||  nOnId == 0xFFFD || nOnId == 0xFFF9 )
				{/* eJpNetType_CATV || eJpNetType_JCHITS */
					stAribCpInfo.ucCopyRestrictionMode = 0;
				}
				else
				{
					stAribCpInfo.ucCopyRestrictionMode = 1;
				}

				stAribCpInfo.ucImageContraintToken = 1;
				stAribCpInfo.ucRetentionMode = 0;
				stAribCpInfo.ucRetentionState = 7;
				stAribCpInfo.ucEncryptionMode = 1;

				nRetVal = programme_makeCopyControl( stAribCpInfo , eType );
			}
			APK_META_DES_Delete( pstDes );
		}
	}

	HxLOG_Print("\n[%s,%d]nRetVal : %d \n" , __FUNCTION__, __LINE__, nRetVal);

	return nRetVal;
}
#endif

static char *	programme_field_DescriptionItems (Programme_t *thiz)
{
	HINT32	i, n, c;
	HINT32	sd;
#if defined(CONFIG_OP_FREESAT)|| defined (CONFIG_OP_FREESAT_TO_JSON)
	HCHAR	*bufJson_desc = NULL,*bufJson_item = NULL;
#endif

	HxLOG_Trace();

	if (thiz == NULL || thiz->event == NULL || thiz->event->item == NULL)
	{
		return NULL;
	}

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	return_if(sd == 0, NULL);

#if defined(CONFIG_OP_JAPAN)
	HLIB_RWSTREAM_Print(sd, "{\"longDescription\":[");
#else
	HLIB_RWSTREAM_Print(sd, "{\"descriptionItems\":[");
#endif
	n = HLIB_VECTOR_Length(thiz->event->item);
	for (i = 0, c = 0 ; i < n ; i++)
	{
		DxEventItem_t *item = HLIB_VECTOR_ItemAt(thiz->event->item, i);
		if (item == NULL)
			continue;

		if(item->description== NULL || item->item == NULL)
			continue;

#if defined(CONFIG_OP_FREESAT_TO_JSON)
		bufJson_desc = NULL;
		bufJson_desc = OPL_Programme_ToJsonCompatibleString(item->description,HxSTD_StrLen(item->description));
		if(bufJson_desc == NULL)
		{
			continue;
		}
		bufJson_item = NULL;
		bufJson_item = OPL_Programme_ToJsonCompatibleString(item->item,HxSTD_StrLen(item->item));
		if(bufJson_item == NULL)
		{
			continue;
		}
#endif

		if (c > 0) HLIB_RWSTREAM_PutChar(sd, ',');


		if(item->description)
		{
#if defined(CONFIG_OP_FREESAT)
			count= 0;
			if(HxSTD_StrCmp(bufJson_desc, "CastandCrew") == 0)
			{
				HLIB_RWSTREAM_Print(sd, "{\"CastnCrew\":[");
				for(j = 0,k=0; k< HxSTD_StrLen(bufJson_item);k++)
				{
					if(HxSTD_StrNCmp(&bufJson_item[k],",",1) == 0)
					{
						if((count%2) == 0)
						{
							if (j > 0)
								HLIB_RWSTREAM_PutChar(sd, ',');
							HLIB_STD_StrNCpySafe(title , &bufJson_item[j],k-j);
							HLIB_RWSTREAM_Print(sd, "{\"title\":\"%s\"", title);
							count ++;
						}
						else
						{
							HLIB_STD_StrNCpySafe(name , &bufJson_item[j],k-j);
							HLIB_RWSTREAM_Print(sd, ",\"name\":\"%s\"}", name);
							count ++;
						}
						j = k+1;
					}
				}
				if((count%2) != 0)
				{
					HLIB_STD_StrNCpySafe(name , &bufJson_item[j],k-j);
					HLIB_RWSTREAM_Print(sd, ",\"name\":\"%s\"}", name);
				}
				else
				{
					if (j > 0)
						HLIB_RWSTREAM_PutChar(sd, ',');
					HLIB_STD_StrNCpySafe(title , &bufJson_item[j],k-j);
					HLIB_RWSTREAM_Print(sd, "{\"title\":\"%s\",\"name\":\"unknown\"}", title);
				}
				HLIB_RWSTREAM_Print(sd, "]}");

			}
			else if(HxSTD_StrCmp(bufJson_desc, "AlphaSortIndexEventName") == 0)
			{
				HLIB_RWSTREAM_Print(sd, "{\"%s\":[",bufJson_desc);
				for(j = 0,k=0; k< HxSTD_StrLen(bufJson_item);k++)
				{
					if(HxSTD_StrNCmp(&bufJson_item[k],",",1) == 0)
					{
						HLIB_STD_StrNCpySafe(name , &bufJson_item[j],k-j);
						if (j > 0)
							HLIB_RWSTREAM_PutChar(sd, ',');
						HLIB_RWSTREAM_Print(sd, "{\"name\":\"%s\"}",name);
						j = k+1;
					}
				}

				if(j == 0)
				{
					HLIB_RWSTREAM_Print(sd, "{\"name\":\"%s\"}", bufJson_item);
				}
				else
				{
					if (j > 0)
						HLIB_RWSTREAM_PutChar(sd, ',');
					HLIB_STD_StrNCpySafe(name , &bufJson_item[j],k-j);
					HLIB_RWSTREAM_Print(sd, "{\"name\":\"%s\"}", name);
				}

				HLIB_RWSTREAM_Print(sd, "]}");

			}
			else
			{
				HLIB_RWSTREAM_Print(sd, "{\"%s\":\"%s\"}"
						, bufJson_desc
						, bufJson_item
					);
			}
#else
			{
				HLIB_RWSTREAM_Print(sd, "{\"%s\":\"%s\"}"
						, item->description
						, item->item
					);
			}
#endif
		}
		c++;
	}
	HLIB_RWSTREAM_Print(sd, "]}");

#if defined(CONFIG_OP_FREESAT_TO_JSON)
	if(bufJson_desc)
	{
		HLIB_STD_MemFree(bufJson_desc);
	}
	if(bufJson_item)
	{
		HLIB_STD_MemFree(bufJson_item);
	}
#endif
	return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
}

static char *	programme_field_HardOfHearing (Programme_t *thiz)
{
	HxLOG_Trace();

	if (thiz->event->component)
	{
		DxEventComponent_t	*comp;

		HxVECTOR_EACH(thiz->event->component, DxEventComponent_t *, comp, {
			if (comp == NULL)
				continue;

			if (comp->content == 0x02 && comp->type == 0x41)
			{
				return HLIB_STD_StrDup("true");
			}
		});
	}
	return HLIB_STD_StrDup("false");
}

static char *	programme_field_isAAC (Programme_t *thiz)
{
	HxLOG_Trace();

	if (thiz->event->component)
	{
		DxEventComponent_t	*comp;

		HxVECTOR_EACH(thiz->event->component, DxEventComponent_t *, comp, {
			if (comp == NULL)
				continue;

			if (comp->content == 0x04 && comp->type == 0x06)
			{
				return HLIB_STD_StrDup("true");
			}
		});
	}
	return HLIB_STD_StrDup("false");
}




static HCHAR  *programme_FsatDescriptor (Programme_t *thiz , HBOOL	bextdesc, HUINT8 desctag)
{
	static const HUINT32		FsatPrivateSpecifierValue = 0x46534154;	//(FSAT)
	const HUINT8				*desc;
	HUINT32					ulPrivateSpecifyValue = 0;
#ifdef CONFIG_OP_FREESAT
	HBOOL					bPrivateConfirm = FALSE;
	DxEventOnlineAvailInfo_t	onlineavailInfo;
	HINT32					sd;
	HUINT8					ucUtcTime[5];
	UNIXTIME				ulStartDateTime, ulStopDateTime;
#endif

	HxLOG_Trace();

	return_if(thiz == NULL, FALSE);
	return_if(thiz->event == NULL, FALSE);
	return_if(thiz->event->descriptor == NULL, FALSE);

//	TAG_PRIVATE_DATA_INDICATOR			= 0x0F,	/** < Private Data Indicator Descriptor */
//	TAG_PRIVATE_DATA_SPECIFIER			= 0x5F,	/** < Private Data Specifier Descriptor */
//  0xDE
//	0xE0

	if(bextdesc == FALSE)
	{
		if(thiz->event->descriptor)
		{
			HxVECTOR_EACH(thiz->event->descriptor, const HUINT8 *, desc, {
				if (desc == NULL)
					continue;


				if(desc[0] == 0x5F)
				{
					ulPrivateSpecifyValue = HxMACRO_Get32Bit(&(desc[2]));
					if(ulPrivateSpecifyValue == FsatPrivateSpecifierValue)
					{
#ifdef CONFIG_OP_FREESAT
						bPrivateConfirm = TRUE;
#endif
					}
				}
				else if(desc[0] == 0x0F)
				{
#ifdef CONFIG_OP_FREESAT
					bPrivateConfirm = FALSE;
#endif
				}

			});
		}
	}
#ifdef CONFIG_OP_FREESAT	// TODO: sbkim, to be delete util.. search developed
	else
	{

		if(thiz->event->extension.fsateventinfo.ext_descriptor)
		{
			HxVECTOR_EACH(thiz->event->extension.fsateventinfo.ext_descriptor, const HUINT8 *, desc, {
				if (desc == NULL)
					continue;

				if(desc[0] == 0x5F)
				{
					ulPrivateSpecifyValue = HxMACRO_Get32Bit(&(desc[2]));
					if(ulPrivateSpecifyValue == FsatPrivateSpecifierValue)
					{
						bPrivateConfirm = TRUE;
					}
				}
				else if(desc[0] == 0x0F)
				{
					bPrivateConfirm = FALSE;
				}
				if(bPrivateConfirm && (desctag == 0xDE) && (desc[0] == desctag)) //ONLINE AVAIL DESCRIPTOR
				{
					//8bit media_usage
					onlineavailInfo.mediausage = (HUINT8)HxMACRO_Get8Bit(&(desc[2]));

					//40bit available_start_datetime
					memcpy(ucUtcTime, &(desc[3]), 5);
					HLIB_DATETIME_ConvertUnixTimeToDate (ucUtcTime, &ulStartDateTime);
					onlineavailInfo.startdatetime = ulStartDateTime;

					//40bit available_stop_datetime
					memcpy(ucUtcTime, &(desc[8]), 5);
					HLIB_DATETIME_ConvertUnixTimeToDate (ucUtcTime, &ulStopDateTime);
					onlineavailInfo.stopdatetime = ulStopDateTime;

					//1bit available_now
					onlineavailInfo.availablenow = (HBOOL)((HxMACRO_Get8Bit(&(desc[13])) &0x80)>> 7);

					//reserved 7 bits
//					printf("onlineavailInfo.mediausage : %d, onlineavailInfo.availablenow:%d\n",onlineavailInfo.mediausage,onlineavailInfo.availablenow);
					if(onlineavailInfo.availablenow == 0)
						return NULL;

					sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
						return_if(sd == 0, NULL);
						HLIB_RWSTREAM_Print(sd, "{\"onlineavailable\":");
						HLIB_RWSTREAM_Print(sd, "{\"media_usage\":%d,\"availableNow\":\"%s\",\"starttime\":%d,\"stoptime\":%d}"
							, onlineavailInfo.mediausage
							, onlineavailInfo.availablenow? "true" : "false"
							, onlineavailInfo.startdatetime
							, onlineavailInfo.stopdatetime
						);
						HLIB_RWSTREAM_Print(sd, "}");
					return HLIB_RWSTREAM_CloseWithoutBuf(sd);

				}
			});
		}
	}
#endif

	return NULL;
}

static char *	programme_field_isAC3 (Programme_t *thiz)
{
	HxLOG_Trace();


	if (thiz->event->component)
	{
		DxEventComponent_t	*comp;

		HxVECTOR_EACH(thiz->event->component, DxEventComponent_t *, comp, {
			if (comp == NULL)
				continue;
			if (comp->content == 0x04)
			{
				return HLIB_STD_StrDup("true");
			}
		});
	}
	return HLIB_STD_StrDup("false");
}

static char *	programme_field_isAC3Plus (Programme_t *thiz)
{
	HxLOG_Trace();

	if (thiz->event->component)
	{
		DxEventComponent_t	*comp;

		HxVECTOR_EACH(thiz->event->component, DxEventComponent_t *, comp, {
			if (comp == NULL)
				continue;
			if (comp->content == 0x08)
			{
				return HLIB_STD_StrDup("true");
			}
		});
	}
	return HLIB_STD_StrDup("false");
}

static char *	programme_field_RunningStatus (Programme_t *thiz)
{
	char	szStr[32];
	char *result = NULL;

	HxLOG_Trace();
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	HxSTD_snprintf(szStr, 32, "%d", (HINT32)thiz->event->runStatus);
	result = HLIB_STD_StrDup(szStr);


	if(result == NULL)
	{
		(void)thiz;
		return NULL;
	}

	return result;

}

static char *	programme_field_Guidance (Programme_t *thiz)
{
#ifdef	CONFIG_OP_FREESAT
	HINT32			sd;
	OPL_Channel_t 	channel;
	char			*gTypeStr = NULL, *gTextStr = NULL;
	char			*bufJson = NULL;

	HxLOG_Trace();
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	if( thiz->event->extension.fsateventinfo.fsatGuidance.guidancetype == 0)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);
		if(thiz->event->extension.fsateventinfo.fsatGuidance.guidancedata == NULL)
		{
			HLIB_RWSTREAM_Print(sd, "{\"guidance\":[");
			HLIB_RWSTREAM_Print(sd, "{\"type\":%d,\"text\":\"\"}"
				, thiz->event->extension.fsateventinfo.fsatGuidance.guidancetype
			);
			HLIB_RWSTREAM_Print(sd, "]}");
		}
		else
		{
			bufJson = OPL_Programme_ToJsonCompatibleString(thiz->event->extension.fsateventinfo.fsatGuidance.guidancedata,HxSTD_StrLen(thiz->event->extension.fsateventinfo.fsatGuidance.guidancedata));
			if(bufJson == NULL)
			{
				HLIB_RWSTREAM_Close(sd);
				return NULL;
			}
			HLIB_RWSTREAM_Print(sd, "{\"guidance\":[");
			HLIB_RWSTREAM_Print(sd, "{\"type\":%d,\"text\":\"%s\"}"
				, thiz->event->extension.fsateventinfo.fsatGuidance.guidancetype
				,bufJson
			);

			HLIB_RWSTREAM_Print(sd, "]}");


		}
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else
	{
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
		channel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
#else
		channel = OPL_Channel_FindChannelByTriplet (NULL, thiz->event->onid, thiz->event->tsid, thiz->event->svcid);
#endif
		if (channel)
		{
			if(((DxService_t*)channel)->optype != eDxOPTYPE_FREESAT)
				return NULL;

			gTypeStr = OPL_Channel_GetField(channel, "guidancesdttype");
			if(gTypeStr == NULL)
				goto RET_ERR;

			if(strcmp(gTypeStr ,"0") == 0)
			{
				gTextStr = OPL_Channel_GetField(channel, "guidancesdttext");
				sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				if(sd == 0)
					goto RET_ERR;

				HLIB_RWSTREAM_Print(sd, "{\"guidance\":[");
				if(gTextStr == NULL)
				{
					HLIB_RWSTREAM_Print(sd, "{\"type\":%d,\"text\":\"\"}"
					,0
					);
				}
				else
				{
					HLIB_RWSTREAM_Print(sd, "{\"type\":%d,\"text\":\"%s\"}"
					,0
					, gTextStr);

				}
				HLIB_RWSTREAM_Print(sd, "]}");

				if(gTextStr)
				{
					HLIB_STD_MemFree(gTextStr);
				}
				if(gTypeStr)
				{
					HLIB_STD_MemFree(gTypeStr);
				}
				return HLIB_RWSTREAM_CloseWithoutBuf(sd);
			}

		}
	}

RET_ERR:
	if(gTextStr)
	{
		HLIB_STD_MemFree(gTextStr);
	}
	if(gTypeStr)
	{
		HLIB_STD_MemFree(gTypeStr);
	}
	if(bufJson)
	{
		HLIB_STD_MemFree(bufJson);
	}
	(void)thiz;


#endif
	return NULL;
}
#if defined(CONFIG_OP_UK_DTT)
static char *	programme_field_UkDttGuidance (Programme_t *thiz)
{
	HxVector_t						*GuidanceList = NULL;
	APKS_UkDttGuidanceDes_t			*GuidanceDes = NULL;
	HINT32							guidance_count =0;
	HUINT32							i=0;
	Opl_Guidance_t					GuidanceInfo;
	HINT32							sd;
	char							*bufJson = NULL;
	OPL_Channel_t 					channel;
	char							*gTypeStr = NULL, *gTextStr = NULL,*gModeStr = NULL;
	HxLANG_Id_e 					eMenuLangId,eCurLangId,eUndLangId;
	HBOOL							bGuidance_Found = FALSE;


	HxLOG_Trace();

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);



	eMenuLangId = eLangID_MAX;
	eMenuLangId = programme_GetMainLanguage();
	if(eMenuLangId == eLangID_MAX) return NULL;
	eUndLangId = eLangID_English;

	if (thiz->event->type & eDxEPG_TYPE_RF)
	{
		GuidanceList = OPL_PARSER_ParsingDesc( thiz->event->descriptor, eSIxDESCTAG_UKDTT_GUIDANCE ,TRUE);
		if( GuidanceList != NULL )
		{
			guidance_count = HLIB_VECTOR_Length(GuidanceList);

			if(guidance_count <= 0 ) return NULL;


			sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			return_if(sd == 0, NULL);


			for (i = 0;i < guidance_count;i++)
			{
				GuidanceDes = (APKS_UkDttGuidanceDes_t *)HLIB_VECTOR_ItemAt( GuidanceList , i );

				HxSTD_MemSet( &GuidanceInfo, 0x00, sizeof( Opl_Guidance_t ) );
				GuidanceInfo.ucGuidanceType = 0xFF;

				if( GuidanceDes != NULL )
				{
					HxLOG_Print("[%s][%d] GuidanceDes->ucGuidanceType:%d \n",__FUNCTION__,__LINE__,GuidanceDes->ucGuidanceType);
					if(GuidanceDes->ucGuidanceType == 0)
					{
						GuidanceInfo.ucGuidanceType = GuidanceDes->ucGuidanceType;
						GuidanceInfo.ucGuidanceMode = 0xFF;
						if(GuidanceDes->pUkDttGuidanceInfo)
						{
							if( GuidanceDes->pUkDttGuidanceInfo->szIso639LangCode)
							{
								eCurLangId = HLIB_LANG_639ToId((const HCHAR *)(GuidanceDes->pUkDttGuidanceInfo->szIso639LangCode));
								if (eCurLangId == eMenuLangId)
								{
									bGuidance_Found = TRUE;
								}
								else if ((eUndLangId != eLangID_MAX) && (eCurLangId == eUndLangId))
								{
									bGuidance_Found = TRUE;
								}
							}
						}
					}
					else if(GuidanceDes->ucGuidanceType == 1)
					{
						GuidanceInfo.ucGuidanceType = GuidanceDes->ucGuidanceType;
						if(GuidanceDes->pUkDttGuidanceInfo)
						{
							GuidanceInfo.ucGuidanceMode = GuidanceDes->pUkDttGuidanceInfo->ucGuidanceMode;

							if( GuidanceDes->pUkDttGuidanceInfo->szIso639LangCode)
							{
								eCurLangId = HLIB_LANG_639ToId((const HCHAR *)(GuidanceDes->pUkDttGuidanceInfo->szIso639LangCode));
								if (eCurLangId == eMenuLangId)
								{
									bGuidance_Found = TRUE;
								}
								else if ((eUndLangId != eLangID_MAX) && (eCurLangId == eUndLangId))
								{
									bGuidance_Found = TRUE;
								}
							}
						}
					}
					else
					{
						GuidanceInfo.ucGuidanceType = GuidanceDes->ucGuidanceType;
						//Nothing... Empty
					}
				}
				else
				{
					//Nothing... Empty
				}

				if(bGuidance_Found == TRUE)
				{
					HxLOG_Print("[%s][%d]bGuidance_Found: %d\n",__FUNCTION__,__LINE__,bGuidance_Found);
					HLIB_STD_StrUtf8NCpy( (HCHAR *)GuidanceInfo.szIso639LangCode,	(HCHAR *)GuidanceDes->pUkDttGuidanceInfo->szIso639LangCode , ISO639LANG_INFO_LEN );
					HxLOG_Print("[%s][%d]GuidanceDes->pUkDttGuidanceInfo->szIso639LangCode : %s , GuidanceInfo.szIso639LangCode:%s\n",
						__FUNCTION__,__LINE__,GuidanceDes->pUkDttGuidanceInfo->szIso639LangCode,GuidanceInfo.szIso639LangCode);
					HxLOG_Print("[%s][%d]GuidanceDes->pUkDttGuidanceInfo->szGuidance : %s\n", __FUNCTION__,__LINE__,GuidanceDes->pUkDttGuidanceInfo->szGuidance);
					if( GuidanceDes->pUkDttGuidanceInfo->szGuidance)
					{
						HLIB_STD_StrUtf8NCpy( (HCHAR *)GuidanceInfo.szGuidance, (HCHAR *)GuidanceDes->pUkDttGuidanceInfo->szGuidance , GUIDANCE_INFO_LEN );
						bufJson = NULL;
						bufJson = OPL_Programme_ToJsonCompatibleString((HCHAR *)GuidanceInfo.szGuidance,HxSTD_StrLen((HCHAR *)GuidanceInfo.szGuidance));
					}
					break;
				}

				HxLOG_Print("[%s][%d] guidancetype : %d, guidancemode : %d, isolang : %s, text :%s\n",__FUNCTION__,__LINE__,GuidanceInfo.ucGuidanceType,GuidanceInfo.ucGuidanceMode,HxSTD_StrLen((HCHAR *)GuidanceInfo.szIso639LangCode) > 0 ? (HCHAR *)GuidanceInfo.szIso639LangCode:"",bufJson != NULL ? bufJson :"");

			}
			HLIB_RWSTREAM_Print(sd, "{\"guidance\":[");
			HLIB_RWSTREAM_Print(sd, "{\"type\":%d,\"mode\":%d,\"text\":\"%s\"}"
				, GuidanceInfo.ucGuidanceType
				, GuidanceInfo.ucGuidanceMode
				,bufJson != NULL ? bufJson :""
			);
			HLIB_RWSTREAM_Print(sd, "]}");

			if( GuidanceList )
			{
				HLIB_VECTOR_Delete( GuidanceList );
			}

			if(bufJson)
			{
				HLIB_STD_MemFree(bufJson);
			}
			return HLIB_RWSTREAM_CloseWithoutBuf(sd);
		}
	}
	else if (thiz->event->type & eDxEPG_TYPE_IP)
	{
		if (thiz->event->extension.ipEpg.guidance != NULL)
		{
			sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			return_if(sd == 0, NULL);

			HLIB_STD_StrUtf8NCpy( (HCHAR *)GuidanceInfo.szGuidance, (HCHAR *)thiz->event->extension.ipEpg.guidance , GUIDANCE_INFO_LEN );
			bufJson = NULL;
			bufJson = OPL_Programme_ToJsonCompatibleString((HCHAR *)GuidanceInfo.szGuidance,HxSTD_StrLen((HCHAR *)GuidanceInfo.szGuidance));

			GuidanceInfo.ucGuidanceType = 1; // probably 0: urn:dtg:cs:BBFCContentRatingCS:2002, 1:urn:fvc:metadata:cs:ContentRatingCS:2014-07:
			GuidanceInfo.ucGuidanceMode = 0; // probably 0: urn:dtg:metadata:cs:DTGContentWarningCS:2011:G, 1:urn:dtg:metadata:cs:DTGContentWarningCS:2011:W

			HLIB_RWSTREAM_Print(sd, "{\"guidance\":[");
			HLIB_RWSTREAM_Print(sd, "{\"type\":%d,\"mode\":%d,\"text\":\"%s\"}"
				, GuidanceInfo.ucGuidanceType
				, GuidanceInfo.ucGuidanceMode
				,bufJson != NULL ? bufJson :""
			);
			HLIB_RWSTREAM_Print(sd, "]}");

			if(bufJson)
			{
				HLIB_STD_MemFree(bufJson);
			}
			return HLIB_RWSTREAM_CloseWithoutBuf(sd);
		}
	}

// Event에서 찾지 못하면 Channel에서 찾는다.
	{
		HxLOG_Print("[%s][%d] \n",__FUNCTION__,__LINE__);
		//Channel 의 Guidance Info를 가져온다.
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
		channel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
#else
		channel = OPL_Channel_FindChannelByTriplet (NULL, thiz->event->onid, thiz->event->tsid, thiz->event->svcid);
#endif
		if (channel)
		{
			gTypeStr = OPL_Channel_GetField(channel, "guidancesdttype");
			if(gTypeStr == NULL)
			{
				goto END_FUNC;
			}

			if(HxSTD_StrCmp(gTypeStr ,"0") == 0)
			{
				sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				if(sd == 0)
				{
					goto END_FUNC;
				}

				gTextStr = OPL_Channel_GetField(channel, "guidancesdttext");

				HLIB_RWSTREAM_Print(sd, "{\"guidance\":[");
				HLIB_RWSTREAM_Print(sd, "{\"type\":%s,\"mode\":%d,\"text\":\"%s\"}"
					, gTypeStr
					, 0xFF
					,gTextStr != NULL ? gTextStr :""
				);

				HLIB_RWSTREAM_Print(sd, "]}");
				if(gTextStr)
				{
					HLIB_STD_MemFree(gTextStr);
				}

				if(gTypeStr)
				{
					HLIB_STD_MemFree(gTypeStr);
				}
				return HLIB_RWSTREAM_CloseWithoutBuf(sd);
			}
			else if(HxSTD_StrCmp(gTypeStr ,"1") == 0)
			{
				sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				if(sd == 0)
					goto END_FUNC;

				gModeStr = OPL_Channel_GetField(channel, "guidancesdtmode");
				gTextStr = OPL_Channel_GetField(channel, "guidancesdttext");

				HLIB_RWSTREAM_Print(sd, "{\"guidance\":[");
				HLIB_RWSTREAM_Print(sd, "{\"type\":%s,\"mode\":%s,\"text\":\"%s\"}"
					, gTypeStr
					, gModeStr
					,gTextStr != NULL ? gTextStr :""
				);
				HLIB_RWSTREAM_Print(sd, "]}");

				HxLOG_Print("[%s][%d] mode : %s text :%s\n",__FUNCTION__,__LINE__,gModeStr,gTextStr);

				if(gTextStr)
				{
					HLIB_STD_MemFree(gTextStr);
				}
				if(gModeStr)
				{
					HLIB_STD_MemFree(gModeStr);
				}
				if(gTypeStr)
				{
					HLIB_STD_MemFree(gTypeStr);
				}
				return HLIB_RWSTREAM_CloseWithoutBuf(sd);
			}
		}
	}

END_FUNC:
	(void)thiz;
	if (gTypeStr)
	{
		HLIB_STD_MemFree(gTypeStr);
	}
	return NULL;

}
#endif

#define EPG_LINKAGE_INFO_MAX 10
static char *	programme_field_Linkage (Programme_t *thiz)
{
    HxVector_t						*LinkageList = NULL;
    HINT32							linkage_count =0;
#if defined(CONFIG_OP_UK_DTT)
	HxVector_t						*LinkagePrivateList = NULL;
	HINT32                          linkage_private_count= 0;
#endif
	APKS_LinkageDes_t			*LinkageDes = NULL;


	HUINT16 	usTargetEventId;
	HUINT8		ucCurrent_position = 0;
	HUINT8		ucloop_length;
	//HUINT8		ubTarget_listed;  // warning
	HUINT8		ubEvent_simulcast;
	HUINT8		ucLink_type;
	HUINT8		ucTarget_id_type;
	HUINT8		ubOnid_flag;
	HUINT8		ubSid_flag;
#if defined(CONFIG_OP_UK_DTT)
	HUINT16 	usUserDefinedId;
#endif
	HUINT16 	usTargetStreamId = 0;
	HUINT16 	usTargetNetworkId = 0;
	HUINT16 	usTargetServiceId = 0;
	HINT32							sd;
	HUINT32		i=0;
	HUINT32		j=0;
	char	channelID[256],ProgrammeID[256];
	OPL_Channel_t	channel;
	HINT32			svcuid;
	HBOOL			bPrivateConfirm = FALSE;

	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	return_if(sd == 0, NULL);
	HLIB_RWSTREAM_Print(sd, "[");

	LinkageList = OPL_PARSER_ParsingDesc( thiz->event->descriptor, eSIxDESCTAG_LINKAGE ,FALSE);
	if( LinkageList != NULL )
	{
		linkage_count = HLIB_VECTOR_Length(LinkageList);

		for(i=0; i<linkage_count; i++)
		{
			if(i >= EPG_LINKAGE_INFO_MAX) break;
			LinkageDes = (APKS_LinkageDes_t *)HLIB_VECTOR_ItemAt( LinkageList , i );
			if (LinkageDes == NULL)
				continue;

#if defined(CONFIG_OP_SES)
			/* DVB SI spec(2015.10) : Defines linkage type from 0x0E to 0x1F about extended_event_linkage_info.
			*  But ETSI 300 468 SI spec(2014.05) : Defines linkage type only 0x0E about extened_event_linkage_info.
			*  If ETSI would be updated, must match this codes into updated ETSI SI Spec.
			*/
			if((LinkageDes->ucLinkType == 0x0D) || ((LinkageDes->ucLinkType >= 0x0E) && (LinkageDes->ucLinkType <= 0x1F)))
#else
			if((LinkageDes->ucLinkType == 0x0D) || (LinkageDes->ucLinkType == 0x0E))
#endif
			{
				if (
					(LinkageDes != NULL) &&
					(LinkageDes->pPrivateDataInfo != NULL) &&
					(LinkageDes->pPrivateDataInfo->ucPrivateDataLen == 3) &&
					(LinkageDes->pPrivateDataInfo->pPrivateData != NULL)
				)
				{
					if(LinkageDes->usOrgNetId == 0 && LinkageDes->usTsId == 0 && LinkageDes->usSvcId== 0)
						continue;

					channel = OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), LinkageDes->usOrgNetId, LinkageDes->usTsId, LinkageDes->usSvcId);
					if(channel == NULL)
						continue;

					usTargetEventId = HxMACRO_Get16Bit(LinkageDes->pPrivateDataInfo->pPrivateData);
					svcuid = OPL_Channel_GetUniqueID(channel);
					OPL_Channel_GetCCID(channel, channelID, 256);
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
					OPL_Programme_MakeProgrammeID (ProgrammeID, 256, OPL_Channel_GetOrgNetworkID(channel), OPL_Channel_GetTransponderID(channel), OPL_Channel_GetServiceID(channel), usTargetEventId);
#else
					OPL_Programme_MakeProgrammeID(ProgrammeID, 256, svcuid, usTargetEventId);
#endif
					ubEvent_simulcast = (HxMACRO_Get8Bit(LinkageDes->pPrivateDataInfo->pPrivateData+2) >> 6) & 0x1;
					if (j > 0)
						HLIB_RWSTREAM_PutChar(sd, ',');

					HLIB_RWSTREAM_Print(sd, "{\"channel\":\"%s\",\"event\":\"%s\",\"simul\":%s}"
						, channelID
						, ProgrammeID
						, ubEvent_simulcast ? "true" : "false"
					);
					j++;

				}
				else if((LinkageDes != NULL) &&
#if defined (CONFIG_OP_SES)
					((LinkageDes->ucLinkType >= 0x0E) && (LinkageDes->ucLinkType <= 0x1F)) &&
#else
					(LinkageDes->ucLinkType == 0x0E) &&
#endif
					(LinkageDes->pPrivateDataInfo != NULL) &&
					(LinkageDes->pPrivateDataInfo->pPrivateData != NULL)
					)
				{
#if defined(CONFIG_OP_UK_DTT)
					LinkagePrivateList = OPL_PARSER_ParsingDesc( thiz->event->descriptor, eSIxDESCTAG_LINKAGE ,TRUE);
					if(LinkagePrivateList != NULL)
					{
						linkage_private_count = HLIB_VECTOR_Length(LinkagePrivateList);
						if(linkage_private_count > 0 ) bPrivateConfirm = TRUE;

						HLIB_VECTOR_Delete(LinkagePrivateList);
					}
#else
					// Do the spec's private check here (DEFAULT: privateConfirmed = FALSE)
					bPrivateConfirm = FALSE;
#endif
					ucloop_length = HxMACRO_Get8Bit(LinkageDes->pPrivateDataInfo->pPrivateData);
					ucCurrent_position++;

					while(ucloop_length > ucCurrent_position)
					{
						usTargetStreamId = LinkageDes->usTsId;
						usTargetNetworkId = LinkageDes->usOrgNetId;
						usTargetServiceId = LinkageDes->usSvcId;

						usTargetEventId = HxMACRO_Get16Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position);
						ucCurrent_position = ucCurrent_position +2;
						// warning
						//ubTarget_listed = (HUINT8)((HxMACRO_Get8Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position) >> 7) & 0x1);
						ubEvent_simulcast = (HUINT8)((HxMACRO_Get8Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position) >> 6) & 0x1);
						ucLink_type = (HUINT8)((HxMACRO_Get8Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position) >> 4) & 0x3);
						ucTarget_id_type = (HUINT8)((HxMACRO_Get8Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position) >> 2) & 0x3);
						ubOnid_flag = (HUINT8)((HxMACRO_Get8Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position) >> 1) & 0x1);
						ubSid_flag = (HUINT8)((HxMACRO_Get8Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position) ) & 0x1);

						ucCurrent_position++;

						if (ucTarget_id_type == 3)
						{
							//All services matched with user_defined_id
#if defined(CONFIG_OP_UK_DTT)
							usUserDefinedId = HxMACRO_Get16Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position);
#endif
							ucCurrent_position = ucCurrent_position + 2;
						}
						else
						{
							if(ucTarget_id_type == 1)
							{
								//target_transport_stream_id
								usTargetStreamId = HxMACRO_Get16Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position);
								ucCurrent_position = ucCurrent_position + 2;
							}

							if(ubOnid_flag)
							{
								//target_original_network_id
								usTargetNetworkId = HxMACRO_Get16Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position);
								ucCurrent_position = ucCurrent_position + 2;
							}
							if(ubSid_flag)
							{
								//target_service_id
								usTargetServiceId = HxMACRO_Get16Bit(LinkageDes->pPrivateDataInfo->pPrivateData + ucCurrent_position);
								ucCurrent_position = ucCurrent_position + 2;
							}
						}
						if(usTargetStreamId == 0 && usTargetNetworkId == 0 && usTargetServiceId== 0)
							continue;

						if ((ucTarget_id_type == 3) && (bPrivateConfirm == FALSE))
						{
							;//Do noting
						}
						else
						{
							if(ucTarget_id_type == 3)
							{
#if defined(CONFIG_OP_UK_DTT)
								channel = OPL_Channel_FindChannelByNumber(OPL_Channel_GetManager(),usUserDefinedId,0);
#else
								HxLOG_Error("unhandled user defined value. \n");
								channel = NULL;
#endif
								if(channel == NULL)
									continue;
								svcuid = OPL_Channel_GetUniqueID(channel);
								OPL_Channel_GetCCID(channel, channelID, 256);
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
								OPL_Programme_MakeProgrammeID (ProgrammeID, 256, OPL_Channel_GetOrgNetworkID(channel), OPL_Channel_GetTransponderID(channel), OPL_Channel_GetServiceID(channel), usTargetEventId);
#else
								OPL_Programme_MakeProgrammeID(ProgrammeID, 256, svcuid, usTargetEventId);
#endif

								if (j > 0)
									HLIB_RWSTREAM_PutChar(sd, ',');

								HLIB_RWSTREAM_Print(sd, "{\"channel\":\"%s\",\"event\":\"%s\",\"simul\":%s ,\"link_type\":%d }"
									, channelID
									, ProgrammeID
									, ubEvent_simulcast ? "true" : "false",ucLink_type

								);
								j++;
							}
							else
							{
								channel = OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), usTargetNetworkId, usTargetStreamId, usTargetServiceId);
								if(channel == NULL)
									continue;
								svcuid = OPL_Channel_GetUniqueID(channel);
								OPL_Channel_GetCCID(channel, channelID, 256);
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
								OPL_Programme_MakeProgrammeID (ProgrammeID, 256, OPL_Channel_GetOrgNetworkID(channel), OPL_Channel_GetTransponderID(channel), OPL_Channel_GetServiceID(channel), usTargetEventId);
#else
								OPL_Programme_MakeProgrammeID(ProgrammeID, 256, svcuid, usTargetEventId);
#endif

								if (j > 0)
									HLIB_RWSTREAM_PutChar(sd, ',');

								HLIB_RWSTREAM_Print(sd, "{\"channel\":\"%s\",\"event\":\"%s\",\"simul\":%s,\"link_type\":%d}"
									, channelID
									, ProgrammeID
									, ubEvent_simulcast ? "true" : "false",ucLink_type
								);
								j++;
							}

						}
					}
					ucCurrent_position = 0;
				}
			}
		}
	}

	HLIB_RWSTREAM_Print(sd, "]");
	return HLIB_RWSTREAM_CloseWithoutBuf(sd);

}




static char *	programme_field_ShortEventPromotional (Programme_t *thiz)
{
#ifdef	CONFIG_OP_FREESAT
	HINT32		sd;

	HxLOG_Trace();
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	if(thiz->event->extension.fsateventinfo.fsatShortEventPromo.szShortEventPromo)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);
		HLIB_RWSTREAM_Print(sd, "{\"shorteventpromo\":[");
		HLIB_RWSTREAM_Print(sd, "{\"text\":\"%s\"}"
			, thiz->event->extension.fsateventinfo.fsatShortEventPromo.szShortEventPromo
		);
		HLIB_RWSTREAM_Print(sd, "]}");

		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
#endif
	(void)thiz;
	return NULL;
}


static char *	programme_field_OnlineAvailability (Programme_t *thiz)
{
	char *result = NULL;

	HxLOG_Trace();
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);


	result = programme_FsatDescriptor (thiz , TRUE, 0xDE);
	if(result == NULL)
	{
		(void)thiz;
		return NULL;
	}
	return result;

}

static char *	programme_field_MediaLocatorInfo(Programme_t	*thiz)
{
#if defined(CONFIG_OP_FREESAT)
	/*
	0x00 Reserved for future use
	0x01 episode/image
	0x02 series/image
	0x03 brand/image
	0x04 event/catchup
	0x05 event/restart
	0x06 to 0x0F reserved for future use
	0x10 promotional/image
	0x11 promotional/media
	0x12 to 0xFF reserved for future use
	Table 12 : freesat media usage
	media_URI_

	*/

	HINT32				i, n;
	DxEventMediaLocatorInfo_t	*medialocatorinfo;
	HINT32	sd;

	HxLOG_Trace();
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);
	if(thiz->event->extension.fsateventinfo.fsatMediaLocatorInfo)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);
		HLIB_RWSTREAM_Print(sd, "{\"medialocatorinfo\":[");

		n = HLIB_VECTOR_Length(thiz->event->extension.fsateventinfo.fsatMediaLocatorInfo);
		for(i=0; i<n; i++)
		{
			medialocatorinfo = (DxEventMediaLocatorInfo_t *)HLIB_VECTOR_ItemAt(thiz->event->extension.fsateventinfo.fsatMediaLocatorInfo, i);
			if (medialocatorinfo == NULL)
				continue;

			if (i > 0)
				HLIB_RWSTREAM_PutChar(sd, ',');

			HLIB_RWSTREAM_Print(sd, "{\"media_usage\":%d,\"media_type\":%d,\"mediauri\":\"%s\"}"
				, medialocatorinfo->mediausage
				, medialocatorinfo->mediatype
				, medialocatorinfo->URI_char
			);
//		printf("[programme_field_MediaLocatorInfo] medialocatorinfo->mediausage : %d, medialocatorinfo->URI_char:%s\n",medialocatorinfo->mediausage,medialocatorinfo->URI_char);
		}

		HLIB_RWSTREAM_Print(sd, "]}");
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else
#endif
	{
		(void)thiz;
		return NULL;
	}
}

static char *	programme_field_MediaLocatorInfoThumnail(Programme_t	*thiz)
{
#if defined(CONFIG_OP_FREESAT)
	HINT32				i, n;
	DxEventMediaLocatorInfo_t	*medialocatorinfo;
	HINT32	sd;

	HxLOG_Trace();
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);
	if(thiz->event->extension.fsateventinfo.fsatMediaLocatorInfo)
	{
		n = HLIB_VECTOR_Length(thiz->event->extension.fsateventinfo.fsatMediaLocatorInfo);
		for(i=0; i<n; i++)
		{
			medialocatorinfo = (DxEventMediaLocatorInfo_t *)HLIB_VECTOR_ItemAt(thiz->event->extension.fsateventinfo.fsatMediaLocatorInfo, i);
			if (medialocatorinfo == NULL)
				continue;

			if( medialocatorinfo->mediausage >= 0x01 && medialocatorinfo->mediausage <=0x03)
			{
				sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				return_if(sd == 0, NULL);
				HLIB_RWSTREAM_Print(sd, "{\"thumbnail\":[");
				HLIB_RWSTREAM_Print(sd, "{\"media_usage\":%d,\"media_type\":%d,\"mediauri\":\"%s\"}"
					, medialocatorinfo->mediausage
					, medialocatorinfo->mediatype
					, medialocatorinfo->URI_char
				);

				HLIB_RWSTREAM_Print(sd, "]}");

				return HLIB_RWSTREAM_CloseWithoutBuf(sd);
			}
		}
	}
#endif
	{
		(void)thiz;
		return NULL;
	}
}

static char *	programme_field_HasAudioDescription (Programme_t *thiz)
{
	if (thiz->event->type & eDxEPG_TYPE_RF)
	{
#if defined(CONFIG_OP_UK_DTT)
		OxMediaPlay_Component_t stComponet;
		HERROR hError = ERR_FAIL;
		HUINT32 ulNum = 0, ulIndex;
		HUINT32 ulMainViewId ;
		HBOOL	bAudioDescription = FALSE;

		APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId);

		hError = APK_MEDIA_PLAY_GetComponentNum(ulMainViewId, eOxMP_COMPONENT_AUDIO, &ulNum);
		if(hError == ERR_OK && ulNum>0)
		{
			for(ulIndex = 0; ulIndex<ulNum ; ulIndex++)
			{
				hError = APK_MEDIA_PLAY_GetComponent( ulMainViewId, eOxMP_COMPONENT_AUDIO, ulIndex, &stComponet);
				if(hError == ERR_OK)
				{
					bAudioDescription =  OAPI_AUDIO_IS_AD_BM_TYPE(stComponet.stAudio.astLangInfo[0].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
					bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE(stComponet.stAudio.astLangInfo[1].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
					bAudioDescription |= OAPI_AUDIO_IS_AD_RM_TYPE(stComponet.stAudio.astLangInfo[0].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
					bAudioDescription |= OAPI_AUDIO_IS_AD_RM_TYPE(stComponet.stAudio.astLangInfo[1].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
				}
			}
		}

		if(bAudioDescription)
			return HLIB_STD_StrDup("true");
		else
			return HLIB_STD_StrDup("false");
#else
		if (thiz->event->component)
		{
			DxEventComponent_t	*comp;

			HxVECTOR_EACH(thiz->event->component, DxEventComponent_t *, comp, {
				if (comp == NULL)
					continue;

				if (comp->content == 0x02 && (comp->type == 0x40 || comp->type == 0x48))
				{
					return HLIB_STD_StrDup("true");
				}
				else if(comp->content == 0x04 && (((comp->type & 0x38) >> 3) == 2))
				{
					return HLIB_STD_StrDup("true");
				}
			});
		}
#endif
	}
	else if (thiz->event->type & eDxEPG_TYPE_IP)
	{
		if (thiz->event->extension.ipEpg.component & eDxIPEPG_UKDTT_COMPONENT_AUDIO_DESCRIPTION)
		{
			return HLIB_STD_StrDup("true");
		}
	}
	return HLIB_STD_StrDup("false");
}

static char *	programme_field_HasSigned (Programme_t *thiz)
{
	if (thiz->event->type & eDxEPG_TYPE_RF)
	{
		if (thiz->event->component)
		{
			DxEventComponent_t	*comp;

			HxVECTOR_EACH(thiz->event->component, DxEventComponent_t *, comp, {
				if (comp == NULL)
					continue;

				if (comp->content == 0x03 && comp->type == 0x30)
				{
					return HLIB_STD_StrDup("true");
				}
			});
		}
	}
	else if (thiz->event->type & eDxEPG_TYPE_IP)
	{
		if (thiz->event->extension.ipEpg.component & eDxIPEPG_UKDTT_COMPONENT_SIGNED)
		{
			return HLIB_STD_StrDup("true");
		}
	}
	return HLIB_STD_StrDup("false");
}

static char *	programme_field_HDLinkage (Programme_t *thiz)
{
	HINT32				i, n;
	DxEventLinkage_t	*hdlinkageinfo;
	HINT32	sd;
	char	channelID[256],ProgrammeID[256];
	OPL_Channel_t	channel;
	HBOOL			simulcast;
	HINT32			evtid;
	HINT32			svcuid;




	HxLOG_Trace();
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->linkage == NULL, NULL);

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	return_if(sd == 0, NULL);
	HLIB_RWSTREAM_Print(sd, "{\"hdlinkageinfo\":[");

	n = HLIB_VECTOR_Length(thiz->event->linkage);
	for(i=0; i<n; i++)
	{
		hdlinkageinfo = (DxEventLinkage_t *)HLIB_VECTOR_ItemAt(thiz->event->linkage, i);
		if (hdlinkageinfo == NULL)
			continue;

		if(hdlinkageinfo->linkageType != 0x0D) continue;

		if (i > 0)
			HLIB_RWSTREAM_PutChar(sd, ',');

		channel = OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), hdlinkageinfo->onid, hdlinkageinfo->tsid, hdlinkageinfo->svcid);

		if(channel == NULL)
			continue;
		if(((DxService_t*)channel)->optype != eDxOPTYPE_FREESAT)
			continue;

		evtid = HxMACRO_Get16Bit(hdlinkageinfo->privateData);
		svcuid = OPL_Channel_GetUniqueID(channel);
		simulcast = (HxMACRO_Get8Bit(hdlinkageinfo->privateData+2) & 0x40) >> 6;

		OPL_Channel_GetCCID(channel, channelID, 256);
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
		OPL_Programme_MakeProgrammeID (ProgrammeID, 256, OPL_Channel_GetOrgNetworkID(channel), OPL_Channel_GetTransponderID(channel), OPL_Channel_GetServiceID(channel), evtid);
#else
		OPL_Programme_MakeProgrammeID(ProgrammeID, 256, svcuid, evtid);
#endif
		simulcast = (HxMACRO_Get8Bit(hdlinkageinfo->privateData+2) & 0x40) >> 6;

		HLIB_RWSTREAM_Print(sd, "{\"channel\":\"%s\",\"event\":\"%s\",\"simul\":\"%s\"}"
			, channelID
			, ProgrammeID
			, simulcast ? "true" : "false"
		);

	}

	HLIB_RWSTREAM_Print(sd, "]}");
	return HLIB_RWSTREAM_CloseWithoutBuf(sd);

}

static DxEventCRID_t *	programme_GetGroupCRID(Programme_t	*thiz, const HCHAR *cridData)
{
	HINT32				i, n;
	DxEventCRID_t	*contentIdentifier;

	HxLOG_Trace();
	return_if(!thiz, NULL);
	return_if(!cridData, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->crid == NULL, NULL);

	n = HLIB_VECTOR_Length(thiz->event->crid);
	for(i=0; i<n; i++)
	{
		contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(thiz->event->crid, i);
		if (contentIdentifier == NULL)
			continue;
		if(strcmp(contentIdentifier->crid, cridData) == 0)
		{
			return contentIdentifier;
		}
	}
	return NULL;
}

void * programme_CloneSiDescriptor(void *src)
{
	void *clone;
	HUINT8 *desc = (HUINT8 *)src;
	HUINT8	descSize;

	if (desc)
	{
		if (desc[0] != 0x4D && desc[0] != 0x4E)
		{
			HxLOG_Error("invalid si descriptor. tag=(0x%x) \n",desc[0]);
			return NULL;
		}

		descSize = desc[1] + 2; // desc[1]: descriptor length, 2: descriptor tag 1 + descriptor length 1
		clone = (void *)HLIB_MEM_MemDup(desc, descSize);
		if (clone != NULL)
		{
			return clone;
		}
	}

	return NULL;
}

STATIC HBOOL	OPL_Programme_FinedEventString(void *data, void *language)
{
	DxEventString_t *pstEventString = (DxEventString_t *)data;
	HxLANG_Id_e eLanguage = (HxLANG_Id_e)language;

	if(pstEventString && pstEventString->language == eLanguage)
		return TRUE;
	else
		return FALSE;
}


OPL_ProgrammeHandle	OPL_Programme_New (const char *signature, ...)
{
	Programme_t	*self;
	va_list ap;

	HxLOG_Trace();

	self = (Programme_t *)HLIB_STD_MemCalloc(sizeof(Programme_t));
	return_if(self == NULL, NULL);

	if (signature)
	{
		va_start(ap, signature);
		if (HLIB_STD_StrCaseCmp(signature, "o") == 0)
		{
			self->event = va_arg(ap, DxEvent_t *);

			goto_if(self->event == NULL, FAILED_WITH_VACLOSE);
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
			goto_if(OPL_Channel_FindChannelByUID(NULL, self->event->svcuid) == NULL, FAILED_WITH_VACLOSE);
#else
			goto_if(OPL_Channel_FindChannelByTriplet(NULL, self->event->onid, self->event->tsid, self->event->svcid) == NULL, FAILED_WITH_VACLOSE);
#endif


		}
		else
		{
			HxLOG_Debug("[%s] Unknown signature(%s)!\n", __FUNCTION__, signature);
		}
		va_end(ap);
	}

	// init cache
	self->cache.integer = HLIB_TREE_NewFull(
				(HxCompareDataFunc)HLIB_STD_StrCaseCmp, NULL, NULL, NULL);
	goto_if(self->cache.integer == NULL, FAILED);

	self->cache.string = HLIB_TREE_NewFull(
				(HxCompareDataFunc)HLIB_STD_StrCaseCmp, NULL, NULL, (HxDestroyNotify)HLIB_STD_MemFree_CB);
	goto_if(self->cache.string == NULL, FAILED);

	self->cache.collection = HLIB_TREE_NewFull(
				(HxCompareDataFunc)HLIB_STD_StrCaseCmp, NULL, NULL, (HxDestroyNotify)OPL_Collection_Delete);
	goto_if(self->cache.collection == NULL, FAILED);

	return (OPL_ProgrammeHandle)self;

FAILED_WITH_VACLOSE:
	va_end(ap);
FAILED:
	OPL_Programme_Delete(self);
	return NULL;
}

void			OPL_Programme_Delete (OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();

	if (thiz)
	{
		if (thiz->event)
			APK_EVENT_Delete(thiz->event);
		if (thiz->cache.integer)
			HLIB_TREE_Destroy(thiz->cache.integer);
		if (thiz->cache.string)
			HLIB_TREE_Destroy(thiz->cache.string);
		if (thiz->cache.collection)
			HLIB_TREE_Destroy(thiz->cache.collection);
		HLIB_STD_MemFree(thiz);
	}
}

OPL_ProgrammeHandle	OPL_Programme_Clone (OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	Programme_t	*clone;

	HxLOG_Trace();

	clone = OPL_Programme_New("o", APK_EVENT_Incref(thiz->event));
	return_if(clone == NULL, NULL);

	return (OPL_ProgrammeHandle)clone;
}

OPL_BOOL		OPL_Programme_Equals (OPL_ProgrammeHandle self, OPL_ProgrammeHandle other)
{
	Programme_t	*lhs = (Programme_t *)self;
	Programme_t	*rhs = (Programme_t *)other;

	if (lhs == rhs)
		return OPL_TRUE;
	if (lhs == NULL || rhs == NULL)
		return OPL_FALSE;
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	if (lhs->event->svcuid != rhs->event->svcuid)
		return OPL_FALSE;
#else
	if (lhs->event->onid != rhs->event->onid || lhs->event->tsid != rhs->event->tsid || lhs->event->svcid != rhs->event->svcid)
		return OPL_FALSE;
#endif
	if (lhs->event->eventId != rhs->event->eventId)
		return OPL_FALSE;
	return OPL_TRUE;
}

HINT32	OPL_Programme_GetEventId(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if(!self, (HINT32)NULL);

	if (thiz->event )
	{
		return thiz->event->eventId;
	}
	return -1;
}


const char *	OPL_Programme_GetName(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	char	*name;

	HxLOG_Trace();
	return_if(!self, NULL);

	name = (char *)HLIB_TREE_Lookup(thiz->cache.string, "name");

	if (name == NULL && thiz->event && thiz->event->name != NULL)
	{
		DxEventString_t *eventString = NULL;
		HxLANG_Id_e language = programme_GetMainLanguage();

		eventString = HLIB_VECTOR_Find (thiz->event->name, OPL_Programme_FinedEventString, (void *)language);
		if(eventString == NULL)
			eventString = HLIB_VECTOR_First(thiz->event->name);
		if(eventString && eventString->string)
		{
			name = eventString->string;
		}
		else
			name = NULL;

#ifdef	CONFIG_DEBUG
		DxEPG_Type_e eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);
		HxLOG_Debug( HxANSI_COLOR_PURPLE("[%s] [epgType:%s, 0x%x]  name:[%s] id:[%d]  svcId:[%d] svcUid:[%d]!!\n") , __FUNCTION__,
			eEpgType == eDxEPG_TYPE_IP 		? (const char*)"eDxEPG_TYPE_IP" 		:
			eEpgType == eDxEPG_TYPE_PF 		? (const char*)"eDxEPG_TYPE_PF" 		:
			eEpgType == eDxEPG_TYPE_SC 		? (const char*)"eDxEPG_TYPE_SC" 		:
			eEpgType == eDxEPG_TYPE_PF_EX	? (const char*)"eDxEPG_TYPE_PF_EX" 	:
			eEpgType == eDxEPG_TYPE_SC_EX	? (const char*)"eDxEPG_TYPE_SC_EX" 	:
			eEpgType == eDxEPG_TYPE_TVTV	? (const char*)"eDxEPG_TYPE_TVTV" 	: (const char*)"OTHER"
				,
			eEpgType,name, thiz->event->eventId , thiz->event->svcid, thiz->event->svcuid);
#endif
	}
	return name;
}


const char *	OPL_Programme_GetDescription(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	char	*text;

	HxLOG_Trace();
	return_if(!self, NULL);

	text = (char *)HLIB_TREE_Lookup(thiz->cache.string, "description");

	if (text == NULL && thiz->event && thiz->event->text != NULL)
	{
		DxEventString_t *eventString = NULL;
		HxLANG_Id_e language = programme_GetMainLanguage();

		eventString = HLIB_VECTOR_Find (thiz->event->text, OPL_Programme_FinedEventString, (void *)language);
		if(eventString == NULL)
			eventString = HLIB_VECTOR_First(thiz->event->text);
		if(eventString && eventString->string)
		{
			text = eventString->string;
		}
		else
			text = NULL;
	}
	return text;
}

unsigned long	OPL_Programme_GetStartTime(OPL_ProgrammeHandle self)
{
	Programme_t		*thiz = (Programme_t *)self;
	unsigned long	value = 0;

	HxLOG_Trace();
	return_if(!self, 0);

	if (!HLIB_TREE_LookupExtended(thiz->cache.integer, "startTime", NULL, (void **)&value))
	{
		if (thiz->event)
		{
			value = (unsigned long)thiz->event->startTime;
#if 0 //DEBUG
            HxDATETIME_Time_t	ulStartTime,ulCurTime;
			unsigned long CurTime = HLIB_STD_GetSystemTime();
			HLIB_DATETIME_ConvertUnixTimeToTime (CurTime, &ulCurTime);
			printf("[OPL_Programme_GetStartTime]svcid:%d ,runstatus:%d eventid:%d ulCurTime[%02d:%02d] \n",thiz->event->svcid,thiz->event->runStatus,thiz->event->eventId,ulCurTime.ucHour,ulCurTime.ucMinute);
			HLIB_DATETIME_ConvertUnixTimeToTime (value, &ulStartTime);
			printf("[OPL_Programme_GetStartTime] value: %d ulStartTime[%02d:%02d] \n",value,ulStartTime.ucHour,ulStartTime.ucMinute);

#endif
		}

	}
	return value;
}

unsigned long	OPL_Programme_GetDuration(OPL_ProgrammeHandle self)
{
	Programme_t		*thiz = (Programme_t *)self;
	unsigned long	value = 0;

	HxLOG_Trace();
	return_if(!self, 0);

	if (!HLIB_TREE_LookupExtended(thiz->cache.integer, "duration", NULL, (void **)&value))
	{
		if (thiz->event)
		{
			value = (unsigned long)thiz->event->duration;
#if defined(CONFIG_OP_JAPAN)
			if( value == 0 )
			{/* undefine event */
				value = 0x00ffffff;
			}
#endif
#if 0 //DEBUG
            HxDATETIME_Time_t	ulDuration;
			unsigned long ulCurTime = HLIB_STD_GetSystemTime();
			HLIB_DATETIME_ConvertUnixTimeToTime (value, &ulDuration);
			printf("[OPL_Programme_GetStartTime] value:%d ulDuration[%02d:%02d] \n",value,ulDuration.ucHour,ulDuration.ucMinute);

#endif

		}
	}
	return value;
}

const char *	OPL_Programme_GetChannelID(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = NULL;
	char	*text;

	HxLOG_Trace();
	return_if(!self, NULL);

	thiz = (Programme_t *)self;

	text = (char *)HLIB_TREE_Lookup(thiz->cache.string, "channelID");
	if (text == NULL && thiz->event != NULL)
	{
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
		OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
#else
		OPL_Channel_t channel = OPL_Channel_FindChannelByTriplet(NULL, thiz->event->onid, thiz->event->tsid, thiz->event->svcid);
#endif
		if (channel)
		{
			char	buf[128];

			if (OPL_Channel_GetCCID(channel, buf, 128))
			{
				text = HLIB_STD_StrDup(buf);
				if (text)
					HLIB_TREE_Insert(thiz->cache.string, "channelID", (void *)text);
			}
		}
	}
#ifdef	CONFIG_DEBUG
	if (text == NULL)
	{
		HxLOG_Warning("Cannot find channel ID\n");
	}
#endif
	return text;
}

int		OPL_Programme_GetParentalRating(OPL_ProgrammeHandle self)
{
	Programme_t      *thiz = (Programme_t *)self;
	int			    rating  = 0;
#if defined( CONFIG_OP_JAPAN )
	int			    pmtRating =0, eitRating =0;
#endif

	HxLOG_Trace();
	return_if(!self, 0);
	return_if(thiz->event == NULL, 0);

#if defined( CONFIG_OP_JAPAN )
	pmtRating = programme_GetPmtRating( thiz );
	eitRating = thiz->event->parentalRating;

	if( pmtRating <= 0 )
	{/* pmt에 해당 안되거나 값이 없는 경우이므로 eit 로만 */
		rating = eitRating;
	}
	else
	{/* 둘다 값이 있는 경우 강한 값이 우선이다. - 스펙에 정확하게 명시된 것은 없으나 내부적으로 정의 */
		rating = ( pmtRating >= eitRating ) ? pmtRating : eitRating;
	}
#else
	rating = thiz->event->parentalRating;
#endif
	return rating;
}


void			OPL_Programme_SetName(OPL_ProgrammeHandle self, const char* name)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self || !name);

	if (HLIB_TREE_Lookup(thiz->cache.string, "name"))
		HLIB_TREE_Replace(thiz->cache.string, "name", HLIB_STD_StrDup(name));
	else
		HLIB_TREE_Insert(thiz->cache.string, "name", HLIB_STD_StrDup(name));
}

void			OPL_Programme_SetDescription(OPL_ProgrammeHandle self, const char* description)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self || !description);

	if (HLIB_TREE_Lookup(thiz->cache.string, "description"))
		HLIB_TREE_Replace(thiz->cache.string, "description", HLIB_STD_StrDup(description));
	else
		HLIB_TREE_Insert(thiz->cache.string, "description", HLIB_STD_StrDup(description));
}

void			OPL_Programme_SetLongDescription(OPL_ProgrammeHandle self, const char* longDescription)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self || !longDescription);

	if (HLIB_TREE_Lookup(thiz->cache.string, "longDescription"))
		HLIB_TREE_Replace(thiz->cache.string, "longDescription", HLIB_STD_StrDup(longDescription));
	else
		HLIB_TREE_Insert(thiz->cache.string, "longDescription", HLIB_STD_StrDup(longDescription));
}

void			OPL_Programme_SetStartTime(OPL_ProgrammeHandle self, unsigned long startTime)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "startTime", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, "startTime", (void *)startTime);
	else
		HLIB_TREE_Insert(thiz->cache.integer, "startTime", (void *)startTime);

}

void			OPL_Programme_SetDuration(OPL_ProgrammeHandle self, unsigned long duration)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "duration", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, "duration", (void *)duration);
	else
		HLIB_TREE_Insert(thiz->cache.integer, "duration", (void *)duration);
}

void			OPL_Programme_SetChannelID(OPL_ProgrammeHandle self, const char* channelID)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self || !channelID);

	if (HLIB_TREE_Lookup(thiz->cache.string, "channelID"))
		HLIB_TREE_Replace(thiz->cache.string, "channelID", HLIB_STD_StrDup(channelID));
	else
		HLIB_TREE_Insert(thiz->cache.string, "channelID", HLIB_STD_StrDup(channelID));
}

void			OPL_Programme_SetEpisode(OPL_ProgrammeHandle self, int episode)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "episode", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, "episode", (void *)episode);
	else
		HLIB_TREE_Insert(thiz->cache.integer, "episode", (void *)episode);
}

void			OPL_Programme_SetTotalEpisodes(OPL_ProgrammeHandle self, int totalEpisodes)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "totalEpisodes", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, "totalEpisodes", (void *)totalEpisodes);
	else
		HLIB_TREE_Insert(thiz->cache.integer, "totalEpisodes", (void *)totalEpisodes);
}

void			OPL_Programme_SetProgrammeID(OPL_ProgrammeHandle self, const char* programmeID)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self || !programmeID);

	if (HLIB_TREE_Lookup(thiz->cache.string, "programmeID"))
		HLIB_TREE_Replace(thiz->cache.string, "programmeID", HLIB_STD_StrDup(programmeID));
	else
		HLIB_TREE_Insert(thiz->cache.string, "programmeID", HLIB_STD_StrDup(programmeID));
}

void			OPL_Programme_SetProgrammeIDType(OPL_ProgrammeHandle self, int programmeIDType)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "programmeIDType", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, "programmeIDType", (void *)programmeIDType);
	else
		HLIB_TREE_Insert(thiz->cache.integer, "programmeIDType", (void *)programmeIDType);
}

const char *	OPL_Programme_GetLongDescription(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	char	*text;

	HxLOG_Trace();
	return_if(!self, NULL);

	text = (char *)HLIB_TREE_Lookup(thiz->cache.string, "longDescription");
	if (text)
	{
		return text;
	}
	return_if(thiz->event == NULL, NULL);

#if defined(CONFIG_OP_JAPAN)
	/* 일본향에선 longText 대신 item/ description 정보가 이쪽으로 간다. */
	text = NULL;
	text = programme_field_DescriptionItems( thiz );
	if( text )
	{
		HLIB_TREE_Insert(thiz->cache.string, "longDescription", text);
	}
	return text;
#else
	if (text == NULL && thiz->event && thiz->event->longText != NULL)
	{
		DxEventString_t *eventString = NULL;
		HxLANG_Id_e language = programme_GetMainLanguage();

		eventString = HLIB_VECTOR_Find (thiz->event->longText, OPL_Programme_FinedEventString, (void *)language);
		if(eventString == NULL)
			eventString = HLIB_VECTOR_First(thiz->event->longText);
		if(eventString && eventString->string)
		{
			text = eventString->string;
		}
		else
			text = NULL;
	}
	return text;
#endif
}

const char *	OPL_Programme_GetProgrammeID(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	char	*text;
	char	buf[128];

	HxLOG_Trace();
	return_if(!self, NULL);

	text = (char *)HLIB_TREE_Lookup(thiz->cache.string, "programmeID");
	if (text)
	{
		return text;
	}
	return_if(thiz->event == NULL, NULL);

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	OPL_Programme_MakeProgrammeID (buf, 128, thiz->event->onid, thiz->event->tsid, thiz->event->svcid, thiz->event->eventId);
#else
	OPL_Programme_MakeProgrammeID(buf, 128, (int)thiz->event->svcuid, (int)thiz->event->eventId);
#endif
	text = HLIB_STD_StrDup(buf);
	if (text)
	{
		HLIB_TREE_Insert(thiz->cache.string, "programmeID", text);
	}
	return text;
}

int				OPL_Programme_GetEpisode(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	int			value = 0;
	HINT32	i, n, c;
	HULONG 		ulEpgType;

	HxLOG_Trace();
	return_if(!self, 0);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);
	if (ulEpgType == eDxEPG_TYPE_IP) {
		value = thiz->event->extension.ipEpg.episodeNum;
		return value;
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		value = thiz->event->extension.tvtvEpg.episodeNum;
		return value;
	}
	(void)thiz;

	if (thiz == NULL || thiz->event == NULL || thiz->event->item == NULL)
	{
		return 0;
	}
	n = HLIB_VECTOR_Length(thiz->event->item);
	for (i = 0, c = 0 ; i < n ; i++)
	{
		DxEventItem_t *item = HLIB_VECTOR_ItemAt(thiz->event->item, i);
		if (item == NULL)
			continue;
		if(item->description)
		{
			if (strcmp(item->description, "EpisodeNumber") == 0)
			{
				value = atoi(item->item);
				return value;
			}
		}
		c++;
	}
	return value;
}

int				OPL_Programme_GetSeason(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	int			value = 0;
	HULONG 		ulEpgType;

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_HDR3000T) || (CONFIG_OP_SES) || defined(CONFIG_OP_UK_DTT)
	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);
	if (ulEpgType == eDxEPG_TYPE_IP) {
		value = thiz->event->extension.ipEpg.season;
		return value;
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		value = thiz->event->extension.tvtvEpg.season;
		return value;
	}
	(void)thiz;
#endif

	return value;
}

int				OPL_Programme_GetTotalEpisodes(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	int			value = 0;
	HULONG		ulEpgType;

	HxLOG_Trace();
	return_if(!self, 0);

	ulEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);
	if (ulEpgType == eDxEPG_TYPE_IP) {
		value = thiz->event->extension.ipEpg.totalNum;
		return value;
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		value = thiz->event->extension.tvtvEpg.totalNum;
		return value;
	}
	(void)thiz;

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "totalEpisodes", NULL, (void **)&value))
	{
		return value;
	}

	// TODO: NOT AVAILABLE

	return 0;
}

int				OPL_Programme_GetProgrammeIDType(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	int			value = 0;
	const char	*id;

	HxLOG_Trace();
	return_if(!self, 1);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "programmeIDType", NULL, (void **)&value))
	{
		return value;
	}

	id = OPL_Programme_GetProgrammeID(self);
	if (id)
	{
		return OPL_Programme_GetIDTypeFromProgrammeID(id);
	}
	return eOPL_ProgrammeID_DVB_EVENT;

}



int				OPL_Programme_GetShowType(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if(thiz == NULL, 0);

	// TODO: NOT IMPLEMENTED YET!!!

	return 0;
}


int				OPL_Programme_GetAudioType(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	int			type = 0;
#if defined(CONFIG_OP_JAPAN)
	OPL_Collection_t		*coll = NULL;
	int 					count = 0;
	OplAVComponent_t		*tempAudio = NULL;
#else
	HINT32		i, n;
	HxLANG_Id_e	language;
	DxEventComponent_t	*comp;
#endif
	HxLOG_Trace();
	return_if(!self, 0);

#if defined(CONFIG_OP_JAPAN)
{
	/* 일본향 음성은 다른곳에 들어가 있음 */
	coll = programme_GetAudioComponent( thiz );
	if( coll )
	{
		count = OPL_Collection_Length( coll );
		if( count > 1 )
		{
			type = 4; /* multi channel audio */
		}
		else if( count == 1 )
		{
			tempAudio = (OplAVComponent_t *)OPL_Collection_ItemAt( coll, 0 );
			if( tempAudio != NULL )
			{
				switch( tempAudio->ulComponentType )
				{
					case 1:	/* single mono */
					case 2:	/* dual mono */
						type = 1;
						break;
					case 3:	/* stereo */
						type = 2;
						break;
					default:
						break;
				}
			}
		}
		else
		{
			HxLOG_Error("[%s] Invalid Audio \n");
		}
	}
}
#else
	return_if(thiz->event == NULL, 0);
	return_if(thiz->event->component == NULL, 0);

	language = programme_GetAudioLanguage(0);

	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (comp == NULL)
			continue;

		type |= programme_GetAudioType(comp, language);
	}
#endif

	return type;
}

int		OPL_Programme_GetSubtitleType(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	HINT32 type = 0;
#if defined( CONFIG_OP_JAPAN)
	OPL_Collection_t	*coll = NULL;
	int 			count = 0;
#else
	HINT32		i, n;
	DxEventComponent_t	*comp;
#endif

	HxLOG_Trace();
	return_if(!self, OPL_FALSE);

#if defined(CONFIG_OP_JAPAN)
{
	/* 일본향 자막은 다른곳에 들어가 있음 */
	if(0){
		coll = programme_GetDataComponent( thiz , OPLAVCOMPONENT_TYPE_SUBTITLE );
		if( coll )
		{
			count = OPL_Collection_Length( coll );
			if( count > 0 )
			{
				return OPL_TRUE;
			}
		}
	}
}
#else
	return_if(thiz->event == NULL, OPL_FALSE);
	return_if(thiz->event->component == NULL, OPL_FALSE);

	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (comp == NULL)
			continue;
		type= programme_GetSubtitleType(comp, eLangID_NONE);
		if (type)
			return type;
	}
#endif

	return OPL_FALSE;
}

OPL_BOOL		OPL_Programme_IsBlocked(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	HERROR		hErr = ERR_FAIL;
	OPL_Channel_t	channel = NULL;
	HBOOL		bSvcLock = FALSE, bRatingLock = FALSE, bRetLock = FALSE;
	DxRatingAge_e	nRating =0, nMenuRating =0;

	HxLOG_Trace();
	return_if(thiz == NULL, OPL_FALSE);
	return_if(thiz->event == NULL, OPL_FALSE);

	/* svc lock check */
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	channel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
#else
	channel = OPL_Channel_FindChannelByTriplet(NULL, thiz->event->onid, thiz->event->tsid, thiz->event->svcid);
#endif

	bSvcLock = OPL_Channel_IsManualBlocked( channel );

	/* rating lock check */
	nRating = OPL_Programme_GetParentalRating( self );
	hErr = APK_SCENARIO_GetMaturityRating(  &nMenuRating );
	if( (hErr == ERR_OK) && (nRating > nMenuRating) )
	{
		bRatingLock = TRUE;
 	}

	/* make return vlaue */
	bRetLock = bSvcLock | bRatingLock;

	HxLOG_Print("[%s,%d] svcLock : %d , nRating : %d, nMenuRating : %d, bRatingLock : %d , bRetLock : %d \n",
					__FUNCTION__, __LINE__ , bSvcLock , nRating, nMenuRating, bRatingLock , bRetLock );

	return (OPL_BOOL)bRetLock;
}

OPL_BOOL		OPL_Programme_HasSubtitle(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
#if defined( CONFIG_OP_JAPAN)
	OPL_Collection_t	*coll = NULL;
	int 			count = 0;
#else
	HINT32		i, n;
	DxEventComponent_t	*comp;
#endif

	HxLOG_Trace();
	return_if(!self, OPL_FALSE);

	if (thiz->event->type & eDxEPG_TYPE_RF)
	{
#if defined(CONFIG_OP_JAPAN)
{
	/* 일본향 자막은 다른곳에 들어가 있음 */
	coll = programme_GetDataComponent( thiz , OPLAVCOMPONENT_TYPE_SUBTITLE );
	if( coll )
	{
		count = OPL_Collection_Length( coll );
		if( count > 0 )
		{
			return OPL_TRUE;
		}
	}
}
#else
	return_if(thiz->event == NULL, OPL_FALSE);
	return_if(thiz->event->component == NULL, OPL_FALSE);

	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (comp == NULL)
			continue;

		if (programme_GetSubtitleType(comp, eLangID_NONE))
			return OPL_TRUE;
	}
#endif
	}
	else if (thiz->event->type & eDxEPG_TYPE_IP)
	{
		if (thiz->event->extension.ipEpg.component & eDxIPEPG_UKDTT_COMPONENT_DVB_SUBTITLE
			|| thiz->event->extension.ipEpg.component & eDxIPEPG_UKDTT_COMPONENT_EBU_SUBTITLE)
		{
			return OPL_TRUE;
		}

	}

	return OPL_FALSE;
}


OPL_Collection_t		*OPL_Programme_GetSubtitleTypes(OPL_ProgrammeHandle self)
{
//	Programme_t	*thiz = (Programme_t *)self;
	OPL_Collection_t	*coll = NULL;
//	HINT32		i, n;
//	DxEventComponent_t	*comp;

#if defined(CONFIG_OP_UK_DTT)
	OxMediaPlay_Component_t stComponet;
	HERROR hError = ERR_FAIL;
	HUINT32 ulNum = 0, ulIndex;
	HUINT32	ulMainViewId ;

	APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId);

	hError = APK_MEDIA_PLAY_GetComponentNum(ulMainViewId, eOxMP_COMPONENT_SUBTITLE, &ulNum);
	if(hError == ERR_OK && ulNum>0)
	{
		coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
		return_if(coll == NULL, NULL);

		for(ulIndex = 0; ulIndex<ulNum ; ulIndex++)
		{
			hError = APK_MEDIA_PLAY_GetComponent( ulMainViewId, eOxMP_COMPONENT_SUBTITLE, ulIndex, &stComponet);
			if(hError == ERR_OK)
			{
				switch(stComponet.stSubtitle.eSubtitleType)
				{
				case eOxMP_SUBTITLETYPE_Ebu:
					(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup("EBU"));
					break;
				case eOxMP_SUBTITLETYPE_Dvb:
					(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup("DVB"));
					break;

				case eOxMP_SUBTITLETYPE_Media:
				case eOxMP_SUBTITLETYPE_Unknown:
				default:
					break;
				}
			}
		}

		if (OPL_Collection_Length(coll) == 0)
		{
			OPL_Collection_Delete(coll);
			return NULL;
		}
	}

#else

	HxLOG_Trace();
	return_if(!self, OPL_FALSE);

	return_if(thiz->event == NULL, OPL_FALSE);
	return_if(thiz->event->component == NULL, OPL_FALSE);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "subtitletypes");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (comp == NULL)
			continue;

		switch(comp->type)
		{

		case 0x02://TELETEXT_SUBTITLE :		/* EBU subtitle */
		case 0x05://TTX_SBTL_PAGE_HERARING_IMPAIRED :
			(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup("EBU"));
			break;
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
			(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup("DVB"));
			break;
		}
	}

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}

	HLIB_TREE_Insert(thiz->cache.collection, "subtitletypes", coll);

#endif
	return coll;
}

OPL_BOOL		OPL_Programme_IsHD(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	HINT32		i, n;
	DxEventComponent_t	*comp;

	HxLOG_Trace();
	return_if(!self, OPL_FALSE);
	return_if(thiz->event == NULL, OPL_FALSE);

	if (thiz->event->type & eDxEPG_TYPE_RF)
	{
		return_if(thiz->event->component == NULL, OPL_FALSE);
		n = HLIB_VECTOR_Length(thiz->event->component);
		for (i = 0 ; i < n ; i++)
		{
			comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
			if (comp == NULL)
				continue;

#if defined(CONFIG_OP_JAPAN)
			if(( comp->type >= 0xB1 && comp->type <= 0xB4 ) /* 1080i */
				|| ( comp->type >= 0xC1 && comp->type <= 0xC4 ) /* 720p */
				|| ( comp->type >= 0xE1 && comp->type <= 0xE4 ) /* 1080p */)
			{
				return OPL_TRUE;
			}
#else
			if (comp->content == 0x01)
			{
				if (0x09 <= comp->type && comp->type <= 0x10)
					return OPL_TRUE;
			}
			else if (comp->content == 0x05)
			{
				if (comp->type == 0x0B
					|| comp->type == 0x0C
					|| comp->type == 0x0F
					|| comp->type == 0x10)
					return OPL_TRUE;
			}
#endif
		}
	}
	else if (thiz->event->type & eDxEPG_TYPE_IP)
	{
		if (thiz->event->extension.ipEpg.component & eDxIPEPG_UKDTT_COMPONENT_HD)
		{
			return OPL_TRUE;
		}
	}
	return OPL_FALSE;
}

OPL_BOOL		OPL_Programme_IsMultilingual(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
#if defined(CONFIG_OP_JAPAN)
	OPL_Collection_t		*coll = NULL;
	int 					count = 0;
	OplAVComponent_t		*tempAudio = NULL;
#else
	HINT32		i, n;
	HxLANG_Id_e	language;
	DxEventComponent_t	*comp;
#endif


	HxLOG_Trace();
	return_if(!self, OPL_FALSE);
#if defined(CONFIG_OP_JAPAN)
	/* 일본향 음성은 다른곳에 들어가 있음 */
	coll = programme_GetAudioComponent( thiz );
	if( coll )
	{
		count = OPL_Collection_Length( coll );
		if( count > 1 )
		{
			return OPL_TRUE;
		}
		else if( count == 1 )
		{
			tempAudio = (OplAVComponent_t *)OPL_Collection_ItemAt( coll, 0 );
			if( tempAudio != NULL )
			{
				switch( tempAudio->ulComponentType )
				{
					case 2: /* dual mono */
						return OPL_TRUE;
						break;

					case 1: /* single mono */
					case 3: /* stereo */
					default:
						break;
				}
			}
		}
		else
		{
			HxLOG_Error("[%s] Invalid Audio \n");
		}
	}
#else
	return_if(thiz->event == NULL, OPL_FALSE);
	return_if(thiz->event->component == NULL, OPL_FALSE);

	language = eLangID_NONE;
	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (comp == NULL)
			continue;

		if (programme_GetAudioType(comp, eLangID_NONE) == 0)
			continue;

		if (language == eLangID_NONE)
			language = comp->language;
		else if (language != comp->language)
			return OPL_TRUE;
	}
#endif
	return OPL_FALSE;

}

OPL_BOOL		OPL_Programme_IsLocked(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	HERROR		hErr = ERR_FAIL;
	OPL_Channel_t	channel = NULL;
	HBOOL		bSvcLock = FALSE, bRatingLock = FALSE, bInputPass = FALSE, bRetLock = FALSE;
	HINT32		nRating =0, nMenuRating =0;
	DxRatingAge_e eRatingAge;

	HxLOG_Trace();
	return_if(thiz == NULL, OPL_FALSE);
	return_if(thiz->event == NULL, OPL_FALSE);

	/* svc lock check */
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	channel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
#else
	channel = OPL_Channel_FindChannelByTriplet(NULL, thiz->event->onid, thiz->event->tsid, thiz->event->svcid);
#endif

	bSvcLock = OPL_Channel_IsManualBlocked( channel );

	/* rating lock check */
	nRating = OPL_Programme_GetParentalRating( self );
	hErr = APK_SCENARIO_GetMaturityRating(  &eRatingAge );
	nMenuRating = (HINT32)eRatingAge;
	if( (hErr == ERR_OK) && (nRating > nMenuRating) )
	{
		bRatingLock = TRUE;
 	}

	/* check input password */
	bInputPass = APK_SCENARIO_PINCODE_GetTemporaryBlock();

	/* make return vlaue */
	bRetLock = (bInputPass == FALSE) ? bInputPass : bSvcLock | bRatingLock;

	HxLOG_Print("[%s,%d] svcLock : %d , nRating : %d, nMenuRating : %d, bRatingLock : %d , bRetLock : %d, bInputPass : %d \n",
				__FUNCTION__, __LINE__, bSvcLock , nRating, nMenuRating, bRatingLock , bRetLock, bInputPass);

	return (OPL_BOOL)bRetLock;
}

OPL_BOOL		OPL_Programme_HasRecording(OPL_ProgrammeHandle self)
{
	if (OPL_Programme_GetRecording(self))
		return OPL_TRUE;
	return OPL_FALSE;
}

#if defined(CONFIG_OP_UK_DTT)

#define EPG_CONTENTS_MAX 2
OPL_Collection_t	*OPL_Programme_GetGenre(OPL_ProgrammeHandle self)
{

	Programme_t			*thiz = (Programme_t *)self;
	OPL_Collection_t	*coll;
	const char			*text;
	HINT32				i;
	HUINT16				word;
	HxVector_t			*ContentList = NULL;
	HINT32				content_count =0;
	APKS_ContentDes_t	*ContentDes = NULL;
	APKS_ContentInfo_t 	*pContentInfo = NULL;
	HUINT32				nCount= 0;

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "genre");
	if (coll)
		return coll;
	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

	if (thiz->event->type & eDxEPG_TYPE_RF)
	{
		ContentList = OPL_PARSER_ParsingDesc( thiz->event->descriptor, eSIxDESCTAG_CONTENT ,FALSE);
		if( ContentList != NULL )
		{
			content_count = HLIB_VECTOR_Length(ContentList);

			for (i = 0;i < content_count;i++)
			{
				ContentDes 	= (APKS_ContentDes_t *)HLIB_VECTOR_ItemAt( ContentList , i );
				pContentInfo 	= ContentDes->pContentInfo;
				while (pContentInfo)
				{
					word = thiz->event->content[nCount++] = (HUINT16)(pContentInfo->ucContentNibble << 8) | (pContentInfo->ucUserNibble);

					if (i >= EPG_CONTENTS_MAX)
					{
						break;
					}

					text = programme_GetGenre(word);
					if (text == NULL)
						continue;

					(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(text));
					HLIB_STD_MemFree((void *)text);

					pContentInfo = pContentInfo->pNextInfo;
				}
			}

			if( ContentList )
			{
				HLIB_VECTOR_Delete( ContentList );
			}
		}
	}
	else if (thiz->event->type & eDxEPG_TYPE_IP)
	{
		word = thiz->event->content[0];
		if (word != 0)
		{
			text = programme_GetGenre(word);
			if (text != NULL)
			{
				(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(text));
				HLIB_STD_MemFree((void *)text);
			}
		}
	}
	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}

	HLIB_TREE_Insert(thiz->cache.collection, "genre", coll);
	return coll;

}
#else
OPL_Collection_t	*OPL_Programme_GetGenre(OPL_ProgrammeHandle self)
{
	Programme_t			*thiz = (Programme_t *)self;
	OPL_Collection_t	*coll;
	const char			*text;
	HINT32				i;
	HUINT16				word;

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "genre");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

	for (i = 0 ; i <DxEVENT_CONTENT ; i++)
	{
		word = thiz->event->content[i];
		if (word == 0)
			continue;

		text = programme_GetGenre(word);
		if (text == NULL)
			continue;

		(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(text));
		HLIB_STD_MemFree((void *)text);
	}
	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, "genre", coll);
	return coll;
}

#endif

OPL_Collection_t	*OPL_Programme_GetAudioLanguages(OPL_ProgrammeHandle self)
{
	Programme_t		*thiz = (Programme_t *)self;
	HINT32			i, n;
	OPL_Collection_t		*coll;
#if defined(CONFIG_OP_JAPAN)
	OPL_Collection_t		*audioColl = NULL;
	OplAVComponent_t 	*audioComp = NULL;
	HUINT32				strLen =0;
	HCHAR				tempString[OPLAVCOMPONENT_LANG_SIZE];
#else
	const HCHAR			*text;
	DxEventComponent_t	*comp;
#endif

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->component == NULL, NULL);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "audioLanguages");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

#if defined(CONFIG_OP_JAPAN)
	/* 일본향 음성은 다른곳에 들어가 있음 */
	audioColl = programme_GetAudioComponent( thiz );
	if( audioColl )
	{
		n = OPL_Collection_Length( audioColl );
		for( i =0; i<n; i++ )
		{
			audioComp = (OplAVComponent_t *)OPL_Collection_ItemAt( audioColl, i );
			if (audioComp == NULL)
				continue;

			HxSTD_MemSet( tempString, 0x00 , OPLAVCOMPONENT_LANG_SIZE );
			strLen = HxSTD_StrLen(audioComp->szLanguage);
			if( strLen > 0 )
			{
				(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(audioComp->szLanguage));
			}

			HxSTD_MemSet( tempString, 0x00 , OPLAVCOMPONENT_LANG_SIZE );
			strLen = HxSTD_StrLen(audioComp->szLanguage2);
			if( strLen > 0 )
			{
				(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(audioComp->szLanguage2));
			}
		}
	}
#else
	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (comp == NULL)
			continue;

		if (programme_GetAudioType(comp, eLangID_NONE) == 0)
			continue;

		text = HLIB_LANG_IdTo639(comp->language);
		if (text == NULL)
			continue;
		(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(text));
	}
#endif

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, "audioLanguages", coll);
	return coll;
}

OPL_Collection_t	*OPL_Programme_GetAudioComponent(OPL_ProgrammeHandle self)
{

	Programme_t		*thiz = (Programme_t *)self;
	HINT32							i, n;
	OPL_Collection_t				*coll = NULL;
	DxEventComponent_t				*comp = NULL;

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->component == NULL, NULL);

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (comp == NULL)
			continue;

		if (programme_GetAudioType(comp,eLangID_NONE) == 0)
			continue;

		(void) OPL_Collection_Add(coll, (void *)comp);
	}
	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}

	return coll;
}


OPL_Collection_t	*OPL_Programme_GetSubtitleLanguages(OPL_ProgrammeHandle self)
{
	Programme_t		*thiz = (Programme_t *)self;
	HINT32			i, n;
	OPL_Collection_t	*coll;
#if defined(CONFIG_OP_JAPAN)
	OPL_Collection_t		*subtitleColl = NULL;
	OplAVComponent_t		*subtitleComp = NULL;
	HUINT32 				strLen =0;
	HCHAR				tempString[OPLAVCOMPONENT_LANG_SIZE];
#else
	const HCHAR			*text;
	DxEventComponent_t	*comp;
#endif

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->component == NULL, NULL);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "subtitleLanguages");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

#if defined(CONFIG_OP_JAPAN)
	/* 일본향 subtitle은 다른곳에 들어가 있음 */
	subtitleColl = programme_GetDataComponent( thiz , OPLAVCOMPONENT_TYPE_SUBTITLE );
	if( subtitleColl )
	{
		n = OPL_Collection_Length( subtitleColl );
		for( i =0; i<n; i++ )
		{
			subtitleComp = (OplAVComponent_t *)OPL_Collection_ItemAt( subtitleColl, i );
			if (subtitleComp == NULL)
				continue;

			HxSTD_MemSet( tempString, 0x00 , OPLAVCOMPONENT_LANG_SIZE );

			strLen = HxSTD_StrLen(subtitleComp->szLanguage);
			if( strLen > 0 )
			{
				(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(subtitleComp->szLanguage));
			}
		}
	}
#else
	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (comp == NULL)
			continue;

		if (programme_GetSubtitleType(comp, eLangID_NONE) == 0)
			continue;

		text = HLIB_LANG_IdTo639(comp->language);
		if (text == NULL)
			continue;
		(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(text));
	}
#endif

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, "subtitleLanguages", coll);
	return coll;
}

OPL_Channel_t		OPL_Programme_GetChannel(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if(!self, 0);
	return_if(thiz->event == NULL, 0);

#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	return OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
#else
	return OPL_Channel_FindChannelByTriplet(NULL, thiz->event->onid, thiz->event->tsid, thiz->event->svcid);
#endif
}


OPL_HANDLE		OPL_Programme_GetScheduledRecording(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	const char	*id;

	HxLOG_Trace();
	return_if(thiz == NULL, NULL);

	id = OPL_Programme_GetProgrammeID(self);
	return_if(id == NULL, NULL);

	return OPL_Scheduler_FindScheduleExceptRecording(OPL_Scheduler_GetInstance(), id);
}


OPL_HANDLE		OPL_Programme_GetRecording(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	OPL_ScheduledItem_t *ret;
	const char	*id;

	HxLOG_Trace();
	return_if(thiz == NULL, NULL);

	id = OPL_Programme_GetProgrammeID(self);
	return_if(id == NULL, NULL);

	ret= OPL_Scheduler_FindSchedule(OPL_Scheduler_GetInstance(), id);
	return_if(ret, ret);

	{
		int	i, c;
		OPL_ScheduledItem_t *item;
		UNIXTIME uxPro_StartTime, uxPro_EndTime;
		UNIXTIME uxSch_StartTime, uxSch_EndTime;
		HCHAR			*pChannelId= NULL;

		uxPro_StartTime = OPL_Programme_GetStartTime(thiz);
		uxPro_EndTime = uxPro_StartTime + OPL_Programme_GetDuration(thiz);

		pChannelId= (HCHAR *)OPL_Programme_GetChannelID(thiz);

		if(HxSTD_StrEmpty(pChannelId))
			return NULL;

		c = OPL_Scheduler_CountScheduledProgrammes(OPL_Scheduler_GetInstance());
		for (i = 0 ; i < c ; i++)
		{
			item = OPL_Scheduler_GetScheduledProgrammeAt(OPL_Scheduler_GetInstance(), i);
			if (item == NULL)
				continue;
			HxLOG_Debug("[%s:%d]type[%d],EventBase[%d]\n",__FUNCTION__,__LINE__,item->type,item->eventbased );
			HxLOG_Debug("[%s:%d] channel[%s]- [%s]\n",__FUNCTION__,__LINE__,pChannelId,item->channelID);

			if(item->type == eOPL_SCHEDULED_PRO_RECORD && item->eventbased == FALSE && HxSTD_StrCmp(pChannelId,item->channelID)==0)
			{
				uxSch_StartTime = item->startTime;
				uxSch_EndTime = uxSch_StartTime + item->duration;
				HxLOG_Debug("[%s:%d][%d - %d][%d - %d]\n",__FUNCTION__,__LINE__,uxPro_StartTime, uxPro_EndTime,uxSch_StartTime,uxSch_EndTime);

				if( (uxSch_StartTime < uxPro_StartTime && uxPro_StartTime < uxSch_EndTime)  || //programme start_time 이 schedule 시간의 중간에 낀경우
					(uxSch_StartTime < uxPro_EndTime && uxPro_EndTime < uxSch_EndTime) ||    //programme end_ime 이 schedule 시간의 중간에 낀경우
					(uxSch_StartTime > uxPro_StartTime && uxPro_EndTime > uxSch_EndTime)) // programme 빨리 시작 되었으나, schedule 보다 일찍 끈나는 경우
				{
					HxLOG_Debug("[%s:%d]Find Timebase Rec..!!!!\n",__FUNCTION__,__LINE__);
					return item;
				}
			}
		}
	}
	return  NULL;
}

OPL_Collection_t*		OPL_Programme_GetRecordings(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	const char	*id;

	HxLOG_Trace();
	return_if(thiz == NULL, NULL);

	id = OPL_Programme_GetProgrammeID(self);
	return_if(id == NULL, NULL);

	return OPL_Scheduler_FindSchedules(OPL_Scheduler_GetInstance(), id);
}



OPL_Collection_t	*OPL_Programme_GetGroupCRIDs(OPL_ProgrammeHandle self)
{
	Programme_t			*thiz = (Programme_t *)self;
	OPL_Collection_t		*coll;
	HINT32				i, n;
	DxEventCRID_t	*contentIdentifier;

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->crid == NULL, NULL);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "groupCRIDs");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

	n = HLIB_VECTOR_Length(thiz->event->crid);
	for(i=0; i<n; i++)
	{
		contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(thiz->event->crid, i);
		if (contentIdentifier == NULL || contentIdentifier->type != DxCRID_TYPE_SERIES)
			continue;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		{
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
			HCHAR				szWholeCRID[DxCRID_LEN] = {0, }; // default authority + data
			OPL_Channel_t	    stChannel;
			char	szDefaultAuth[DxDEFAULT_AUTHORITY_LEN] = {0,};
			stChannel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
#else
			stChannel = OPL_Channel_FindChannelByTriplet (NULL, thiz->event->onid, thiz->event->tsid, thiz->event->svcid);
#endif

			if (stChannel != NULL )
			{
				OPL_Channel_GetdefaultAuth(stChannel, szDefaultAuth);
			}

			if ( !(HxSTD_StrEmpty(contentIdentifier->crid)) )
			{
				// if auth exists && crid doesn't contain auth
				if ( !(HxSTD_StrEmpty(szDefaultAuth)) && contentIdentifier->crid[0] == '/')
				{
					HxSTD_snprintf(szWholeCRID, DxCRID_LEN, "%s%s",szDefaultAuth, contentIdentifier->crid);
				}
				else
				{
					HxSTD_snprintf(szWholeCRID, DxCRID_LEN, "%s", contentIdentifier->crid);
				}
				HxLOG_Debug("svcuid = %d, \n\tname( %s ), eventid = %d, szWholeCRID = %s\n", (int)thiz->event->svcuid, OPL_Programme_GetName(self), (int)thiz->event->eventId, szWholeCRID);
			}
			HLIB_STD_StrUpper(szWholeCRID);	// sch, rec과 동일하게 case를 upper로 맞춘다.
			(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(szWholeCRID));
		}
#else
		HxLOG_Debug("OPL-> [%s]\n",contentIdentifier->crid);
		(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(contentIdentifier->crid));
#endif
	}
	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, "groupCRIDs", coll);
	return coll;
}

OPL_BOOL		OPL_Programme_GetCRIDSearchFlag(OPL_ProgrammeHandle self)
{
	Programme_t 		*thiz = (Programme_t *)self;
	OPL_BOOL			bRet = OPL_FALSE;

	HxLOG_Trace();
	return_if(!self, OPL_FALSE);
	return_if(thiz->event == NULL, OPL_FALSE);
	return_if(thiz->event->crid == NULL, OPL_FALSE);

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME) && defined(CONFIG_OP_TDC)
    DxEventCRID_t		*contentIdentifier;
    HINT32				i, n;
	n = HLIB_VECTOR_Length(thiz->event->crid);
	for(i = 0; i < n; i++)
	{
		contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(thiz->event->crid, i);
		if (contentIdentifier != NULL && contentIdentifier->type == DxCRID_TYPE_SERIES)
		{
			if( contentIdentifier->search_flag == DxEPG_CRID_SEARCH_FLAG_SAMESERVICE )
			{
				bRet = OPL_TRUE;
			}
			break;
		}
	}
#endif

	return bRet;
}


const char *	OPL_Programme_GetProgrammeCRID(OPL_ProgrammeHandle self)
{
	Programme_t			*thiz = (Programme_t *)self;
	HINT32				i, n;
	DxEventCRID_t	*contentIdentifier;
	HCHAR*				szCrid = NULL;
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	HCHAR				szWholeCRID[DxCRID_LEN] = {0, }; // default authority + data
	OPL_Channel_t	stChannel;
	char	szDefaultAuth[DxDEFAULT_AUTHORITY_LEN] = {0,};
#endif
#endif

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->crid == NULL, NULL);

	szCrid = (char *)HLIB_TREE_Lookup(thiz->cache.string, "programmeCRID");
	if (szCrid)
	{
		return szCrid;
	}

	n = HLIB_VECTOR_Length(thiz->event->crid);
	for(i=0; i<n; i++)
	{
		contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(thiz->event->crid, i);
		if (contentIdentifier != NULL && contentIdentifier->type == DxCRID_TYPE_PROG)
		{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
			stChannel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
#else
			stChannel = OPL_Channel_FindChannelByTriplet (NULL, thiz->event->onid, thiz->event->tsid, thiz->event->svcid);
#endif

			if (stChannel != NULL )
			{
				OPL_Channel_GetdefaultAuth(stChannel, szDefaultAuth);
			}

			if ( !(HxSTD_StrEmpty(contentIdentifier->crid)) )
			{
				if ( !(HxSTD_StrEmpty(szDefaultAuth)) && contentIdentifier->crid[0] == '/')
				{
					HxSTD_snprintf(szWholeCRID, DxCRID_LEN, "%s%s",szDefaultAuth, contentIdentifier->crid);
				}
				else
				{
					HxSTD_snprintf(szWholeCRID, DxCRID_LEN, "%s", contentIdentifier->crid);
				}
				HLIB_STD_StrUpper(szWholeCRID);
				szCrid = HLIB_STD_StrDup(szWholeCRID);
				break;
			}
#else
			szCrid =  HLIB_STD_StrDup(contentIdentifier->crid);
			break;
#endif
		}
	}
	if (szCrid == NULL)
	{
		return NULL;
	}

	HLIB_TREE_Insert(thiz->cache.string, "programmeCRID", szCrid);
	return szCrid;
}

const char *	OPL_Programme_GetRecommendationCRID(OPL_ProgrammeHandle self)
{
	Programme_t 		*thiz = (Programme_t *)self;
	HINT32				i, n;
	DxEventCRID_t	*contentIdentifier;

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->crid == NULL, NULL);

	n = HLIB_VECTOR_Length(thiz->event->crid);
	for(i=0; i<n; i++)
	{
		contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(thiz->event->crid, i);
		if (contentIdentifier != NULL && contentIdentifier->type == DxCRID_TYPE_RECOMMEND)
		{
			return HLIB_STD_StrDup(contentIdentifier->crid);
		}
	}
	return NULL;

}

static HBOOL _OPL_Programme_GetMediaDeeplinkData(HxVector_t *pMediaVector, HUINT32 * pulExpireDate, HCHAR **ppucUrl, HCHAR **ppucID)
{
	DxEventMedia_t		* 	pMediaDeeplink 	= NULL;
	DxEventMediaUrl_t	* 	pMediaUrl 		= NULL;
	HBOOL 					bFound 			= FALSE;
	HINT32					i = 0, j = 0, len = 0, len2 = 0;

	// -------------------------
	return_if(pMediaVector == NULL, FALSE);
        
	len = HLIB_VECTOR_Length(pMediaVector);
	for( i=0 ; i<len ; i++)
	{
		pMediaDeeplink = (DxEventMedia_t*)HLIB_VECTOR_ItemAt(pMediaVector, i);
		if(NULL == pMediaDeeplink || NULL == pMediaDeeplink->pcMediaId || NULL == pMediaDeeplink->mediaUrlList )
			continue;

		len2 = HLIB_VECTOR_Length(pMediaDeeplink->mediaUrlList);
		for(j = 0; j < len2; j++)
		{
			pMediaUrl = (DxEventMediaUrl_t*)HLIB_VECTOR_ItemAt(pMediaDeeplink->mediaUrlList, j );
			if(NULL == pMediaUrl || NULL == pMediaUrl->pucUrl || NULL == pMediaUrl->pucTarget)
				continue;
			if( 0 != HxSTD_StrNCmp((HCHAR *)pMediaUrl->pucTarget , "STB", 3) )
				continue;

			bFound = TRUE;
			break;
		}

		if(TRUE == bFound)
		{
			if(NULL != pulExpireDate)
			{
				*pulExpireDate = pMediaDeeplink->ulExpireDate;
				HxLOG_Debug("[%s] (deeplinkMediaExpireDate:%d)  .!!\n", __FUNCTION__, pMediaDeeplink->ulExpireDate);
			}
			if(NULL != ppucUrl)
			{
				*ppucUrl = HLIB_STD_StrDup((HCHAR *)pMediaUrl->pucUrl);
				HxLOG_Debug("[%s] (deeplinkMediaUrl:%s) .!!\n", __FUNCTION__, pMediaUrl->pucUrl);
			}
			if(NULL != ppucID)
			{
				*ppucID = HLIB_STD_StrDup(pMediaDeeplink->pcMediaId);
				HxLOG_Debug("[%s] (deeplinkMediaID:%s) .!!\n", __FUNCTION__, pMediaDeeplink->pcMediaId);
			}
			break;
		}
	}
	// -------------------------
	return bFound;
}

const char *	OPL_Programme_GetMediaDeeplinkID(OPL_ProgrammeHandle self)
{
	Programme_t			* thiz = (Programme_t *)self;
	DxEPG_Type_e		eEpgType = eDxEPG_TYPE_NONE;
	HBOOL				bFound = FALSE;
	char 				* pucID = NULL;

	HxLOG_Trace();
	return_if(NULL == self, NULL);
	return_if(NULL == thiz->event, NULL);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, NULL);
	return_if(thiz->event->extension.ipEpg.mediaList == NULL, NULL);

	pucID = (char *)HLIB_TREE_Lookup(thiz->cache.string, "deeplinkMediaID");
	if(pucID)
	{
		return pucID;
	}

	// ---------------------------------------------
	bFound = _OPL_Programme_GetMediaDeeplinkData(thiz->event->extension.ipEpg.mediaList, NULL, NULL, &pucID);
	// ---------------------------------------------
	if(TRUE == bFound && NULL != pucID)
	{
		HLIB_TREE_Insert(thiz->cache.string, "deeplinkMediaID", (void *)pucID);
	}
	return pucID;
}

const char *	OPL_Programme_GetMediaDeeplinkUrl(OPL_ProgrammeHandle self)
{
	Programme_t			* thiz = (Programme_t *)self;
	DxEPG_Type_e		eEpgType = eDxEPG_TYPE_NONE;
	HBOOL				bFound = FALSE;
	char 				* pucUrl = NULL;

	HxLOG_Trace();
	return_if(NULL == self, NULL);
	return_if(NULL == thiz->event, NULL);


	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, NULL);
	return_if(thiz->event->extension.ipEpg.mediaList == NULL, NULL);

	pucUrl = (char *)HLIB_TREE_Lookup(thiz->cache.string, "deeplinkMediaUrl");
	if(pucUrl)
	{
		return pucUrl;
	}

	// ---------------------------------------------
	bFound = _OPL_Programme_GetMediaDeeplinkData(thiz->event->extension.ipEpg.mediaList, NULL, &pucUrl, NULL);
	// ---------------------------------------------
	if(TRUE == bFound && NULL != pucUrl)
	{
		HLIB_TREE_Insert(thiz->cache.string, "deeplinkMediaUrl", (void *)pucUrl);
	}
	return pucUrl;
}


unsigned long	OPL_Programme_GetMediaDeeplinkExpireDate(OPL_ProgrammeHandle self)
{
	Programme_t			* 	thiz 		= (Programme_t *)self;
	HUINT32					ulExpireDate= 0;
	DxEPG_Type_e		  	eEpgType 	= eDxEPG_TYPE_NONE;
	HBOOL 					bFound 		= FALSE;

	HxLOG_Trace();
	return_if(NULL == self, (unsigned long)NULL);
	return_if(NULL == thiz->event, (unsigned long)NULL);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, (unsigned long)NULL);
	return_if(thiz->event->extension.ipEpg.mediaList == NULL, (unsigned long)NULL);


	if (!HLIB_TREE_LookupExtended(thiz->cache.integer, "deeplinkMediaExpireDate", NULL, (void **)&ulExpireDate))
	{
		// ---------------------------------------------
		bFound = _OPL_Programme_GetMediaDeeplinkData(thiz->event->extension.ipEpg.mediaList, &ulExpireDate, NULL, NULL);
		// ---------------------------------------------
		if(TRUE == bFound)
		{
			HLIB_TREE_Insert(thiz->cache.integer, "deeplinkMediaExpireDate", (void *)ulExpireDate);
		}
	}
	return ulExpireDate;
}

static HBOOL _opl_programme_GetOnDemandData (DxEventOnDemand_t *pOnDemand, HCHAR **ppucProgramUrl, HCHAR **ppucAuxiliaryUrl, HCHAR **ppucDeliveryMode)
{
	DxEventOnDemandString_t	*pOnDemandString = NULL;
	HINT32					i = 0, len = 0;

	if (pOnDemand == NULL ||  pOnDemand->onDemandStringList == NULL)
		return FALSE;

	// -------------------------
	len = HLIB_VECTOR_Length(pOnDemand->onDemandStringList);
	for( i=0 ; i<len ; i++)
	{
		pOnDemandString = (DxEventOnDemandString_t*)HLIB_VECTOR_ItemAt(pOnDemand->onDemandStringList, i);
		if (NULL == pOnDemandString || NULL == pOnDemandString->pucString)
			continue;

		if (pOnDemandString->ulStringType == eDxIPEPG_UKDTT_TEXT_OD_PROGRAM_URL
			&& ppucProgramUrl != NULL)
		{
			*ppucProgramUrl = HLIB_STD_StrDup((HCHAR *)pOnDemandString->pucString);
			return TRUE;
		}
		if (pOnDemandString->ulStringType == eDxIPEPG_UKDTT_TEXT_OD_AUXILIARY_URL
			&& ppucAuxiliaryUrl != NULL)
		{
			*ppucAuxiliaryUrl = HLIB_STD_StrDup((HCHAR *)pOnDemandString->pucString);
			return TRUE;
		}
		if (pOnDemandString->ulStringType == eDxIPEPG_UKDTT_TEXT_OD_DELIVERY_MODE
			&& ppucDeliveryMode != NULL)
		{
			*ppucDeliveryMode = HLIB_STD_StrDup((HCHAR *)pOnDemandString->pucString);
			return TRUE;
		}

	}
	// -------------------------
	return FALSE;
}

unsigned long	OPL_Programme_GetOnDemandStartOfAvailability(OPL_ProgrammeHandle self)
{
	Programme_t			* 	thiz 		= (Programme_t *)self;
	DxEPG_Type_e		  	eEpgType 	= eDxEPG_TYPE_NONE;

	HxLOG_Trace();
	return_if(NULL == self, 0);
	return_if(NULL == thiz->event, 0);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, 0);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, 0);

	return thiz->event->extension.ipEpg.onDemand->ulStartOfAvailability;
}

unsigned long	OPL_Programme_GetOnDemandEndOfAvailability(OPL_ProgrammeHandle self)
{
	Programme_t			* 	thiz 		= (Programme_t *)self;
	DxEPG_Type_e		  	eEpgType 	= eDxEPG_TYPE_NONE;

	HxLOG_Trace();
	return_if(NULL == self, 0);
	return_if(NULL == thiz->event, 0);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, 0);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, 0);

	return thiz->event->extension.ipEpg.onDemand->ulEndOfAvailability;
}

unsigned long	OPL_Programme_GetOnDemandPublishedDuration(OPL_ProgrammeHandle self)
{
	Programme_t			* 	thiz 		= (Programme_t *)self;
	DxEPG_Type_e		  	eEpgType 	= eDxEPG_TYPE_NONE;

	HxLOG_Trace();
	return_if(NULL == self, 0);
	return_if(NULL == thiz->event, 0);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, 0);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, 0);

	return thiz->event->extension.ipEpg.onDemand->ulPublishedDuration;
}
OPL_BOOL	OPL_Programme_GetOnDemandIsFree(OPL_ProgrammeHandle self)
{
	Programme_t			* 	thiz 		= (Programme_t *)self;
	DxEPG_Type_e		  	eEpgType 	= eDxEPG_TYPE_NONE;

	HxLOG_Trace();
	return_if(NULL == self, OPL_FALSE);
	return_if(NULL == thiz->event, OPL_FALSE);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, OPL_FALSE);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, OPL_FALSE);

	if (thiz->event->extension.ipEpg.onDemand->ucFree == 1)
	{
		return OPL_TRUE;
	}

	return OPL_FALSE;
}

OPL_BOOL	OPL_Programme_GetOnDemandIsHD(OPL_ProgrammeHandle self)
{
	Programme_t			* 	thiz 		= (Programme_t *)self;
	DxEPG_Type_e		  	eEpgType 	= eDxEPG_TYPE_NONE;

	HxLOG_Trace();
	return_if(NULL == self, OPL_FALSE);
	return_if(NULL == thiz->event, OPL_FALSE);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, OPL_FALSE);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, OPL_FALSE);

	if (thiz->event->extension.ipEpg.onDemand->ucIsHD == 1)
	{
		return OPL_TRUE;
	}

	return OPL_FALSE;
}

OPL_BOOL	OPL_Programme_GetOnDemandIsMediaAvailable(OPL_ProgrammeHandle self)
{
	Programme_t			* 	thiz 		= (Programme_t *)self;
	DxEPG_Type_e		  	eEpgType 	= eDxEPG_TYPE_NONE;

	HxLOG_Trace();
	return_if(NULL == self, OPL_FALSE);
	return_if(NULL == thiz->event, OPL_FALSE);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, OPL_FALSE);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, OPL_FALSE);

	if (thiz->event->extension.ipEpg.onDemand->ulOnDemandGenre & eDxIPEPG_UKDTT_GENRE_MEDIA)
	{
		return OPL_TRUE;
	}

	return OPL_FALSE;
}

OPL_BOOL	OPL_Programme_GetOnDemandIsFEPGAvailable(OPL_ProgrammeHandle self)
{
	Programme_t			* 	thiz 		= (Programme_t *)self;
	DxEPG_Type_e		  	eEpgType 	= eDxEPG_TYPE_NONE;

	HxLOG_Trace();
	return_if(NULL == self, OPL_FALSE);
	return_if(NULL == thiz->event, OPL_FALSE);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, OPL_FALSE);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, OPL_FALSE);

	if (thiz->event->extension.ipEpg.onDemand->ulOnDemandGenre & eDxIPEPG_UKDTT_GENRE_FEPG)
	{
		return OPL_TRUE;
	}

	return OPL_FALSE;
}

const char *	OPL_Programme_GetOnDemandProgramUrl(OPL_ProgrammeHandle self)
{
	Programme_t			* thiz = (Programme_t *)self;
	DxEPG_Type_e		eEpgType = eDxEPG_TYPE_NONE;
	HBOOL				bFound = FALSE;
	char 				*programUrl = NULL;

	HxLOG_Trace();
	return_if(NULL == self, NULL);
	return_if(NULL == thiz->event, NULL);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, NULL);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, NULL);

	programUrl = (char *)HLIB_TREE_Lookup(thiz->cache.string, "onDemandProgramUrl");
	if(programUrl)
	{
		return programUrl;
	}

	// ---------------------------------------------
	bFound = _opl_programme_GetOnDemandData(thiz->event->extension.ipEpg.onDemand, &programUrl, NULL, NULL);
	// ---------------------------------------------
	if (TRUE == bFound && NULL != programUrl)
	{
		HLIB_TREE_Insert(thiz->cache.string, "onDemandProgramUrl", (void *)programUrl);
	}
	return programUrl;
}

const char *	OPL_Programme_GetOnDemandAuxiliaryUrl(OPL_ProgrammeHandle self)
{
	Programme_t			* thiz = (Programme_t *)self;
	DxEPG_Type_e		eEpgType = eDxEPG_TYPE_NONE;
	HBOOL				bFound = FALSE;
	char 				*auxiliaryUrl = NULL;

	HxLOG_Trace();
	return_if(NULL == self, NULL);
	return_if(NULL == thiz->event, NULL);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, NULL);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, NULL);

	auxiliaryUrl = (char *)HLIB_TREE_Lookup(thiz->cache.string, "onDemandAuxiliaryUrl");
	if(auxiliaryUrl)
	{
		return auxiliaryUrl;
	}

	// ---------------------------------------------
	bFound = _opl_programme_GetOnDemandData(thiz->event->extension.ipEpg.onDemand, NULL, &auxiliaryUrl, NULL);
	// ---------------------------------------------
	if (TRUE == bFound && NULL != auxiliaryUrl)
	{
		HLIB_TREE_Insert(thiz->cache.string, "onDemandAuxiliaryUrl", (void *)auxiliaryUrl);
	}
	return auxiliaryUrl;
}

const char *	OPL_Programme_GetOnDemandDeliveryMode(OPL_ProgrammeHandle self)
{
	Programme_t			* thiz = (Programme_t *)self;
	DxEPG_Type_e		eEpgType = eDxEPG_TYPE_NONE;
	HBOOL				bFound = FALSE;
	char 				*deliveryMode = NULL;

	HxLOG_Trace();
	return_if(NULL == self, NULL);
	return_if(NULL == thiz->event, NULL);

	eEpgType = OPL_Programme_GetEpgType((OPL_ProgrammeHandle)thiz);

	return_if(eDxEPG_TYPE_IP != eEpgType, NULL);
	return_if(thiz->event->extension.ipEpg.onDemand == NULL, NULL);

	deliveryMode= (char *)HLIB_TREE_Lookup(thiz->cache.string, "onDemandDeliveryMode");
	if(deliveryMode)
	{
		return deliveryMode;
	}

	// ---------------------------------------------
	bFound = _opl_programme_GetOnDemandData(thiz->event->extension.ipEpg.onDemand, NULL, NULL, &deliveryMode);
	// ---------------------------------------------
	if (TRUE == bFound && NULL != deliveryMode)
	{
		HLIB_TREE_Insert(thiz->cache.string, "onDemandDeliveryMode", (void *)deliveryMode);
	}
	return deliveryMode;
}
OPL_Collection_t	*OPL_Programme_GetGroupCRIDsAndType(OPL_ProgrammeHandle self)
{

	Programme_t			*thiz = (Programme_t *)self;
	OPL_Collection_t		*coll = NULL;
#ifdef OP_AUS_DTT
	DxEventCRID_t	*contentIdentifier = NULL;
#endif

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->crid == NULL, NULL);

#ifdef OP_AUS_DTT
	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "groupCRIDsAndType");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);
	return_if(coll == NULL, NULL);

	HxLOG_Print( "[%s,%d]thiz->event->crid length[%d]\n" , __FUNCTION__, __LINE__ ,HLIB_VECTOR_Length(thiz->event->crid));
	n = HLIB_VECTOR_Length(thiz->event->crid);
	for(i=0; i<n; i++)
	{
		OPL_ScheduledTvAnyTimetCRID_t crid;
		HxSTD_MemSet(&crid,0x0,sizeof(DxRSV_TvAnyTimeScheduledCRID));
		contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(thiz->event->crid, i);
		if (contentIdentifier == NULL)
			continue;
		HxLOG_Print( "[%s,%d][%d] : type= 0x%x, crid = %s\n" , __FUNCTION__, __LINE__ ,i,contentIdentifier->type,contentIdentifier->crid );

		crid.ucCRIDType = contentIdentifier->type;
		HxSTD_StrNCpy(crid.szCRID, contentIdentifier->crid, OPL_SCHEDULED_CRID_LEN);
		(void) OPL_Collection_Add(coll, (void *)HLIB_MEM_MemDup(&crid,sizeof(OPL_ScheduledTvAnyTimetCRID_t)));
	}
	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, "groupCRIDsAndType", coll);
#endif

    return coll;
}


const char *	OPL_Programme_GetIMI(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();

	(void)thiz;
	// TODO: NOT IMPLEMENTED YET

	return NULL;
}

const char *	OPL_Programme_GetField(OPL_ProgrammeHandle self, const char *field)
{
	Programme_t	*thiz = (Programme_t *)self;
	char		*text = NULL;

	static const struct field {
		const char *field;
		char * (* getter) (Programme_t *);
	} s_fields[] = {
		  {"descriptionItems", programme_field_DescriptionItems}
		, {"hardOfHearing", programme_field_HardOfHearing}
		, {"isAAC", programme_field_isAAC}
		, {"isAC3", programme_field_isAC3}
		, {"isAC3P", programme_field_isAC3Plus}
		, {"runningstatus", programme_field_RunningStatus}
		, {"drm", programme_field_drm}
		, {"season", programme_field_season}
		, {"seriesID", programme_field_seriesID}
		, {"thumbnailURL", programme_field_thumbnailURL}
		, {"actor", programme_field_actor}
		, {"actress", programme_field_actress}
		, {"director", programme_field_director}
		, {"tvtvid", programme_field_tvtvid}
		, {"person", programme_field_person}
		, {"linkage", programme_field_Linkage}
		// for freesat
#if defined(CONFIG_OP_FREESAT)
		, {"freesat/guidance", programme_field_Guidance}
		, {"freesat/hasAudioDescription", programme_field_HasAudioDescription}
		, {"freesat/hasSigned", programme_field_HasSigned}
		, {"freesat/HDLinkage", programme_field_HDLinkage}
		, {"freesat/onlineavailability", programme_field_OnlineAvailability}
		, {"freesat/medialocationinfo", programme_field_MediaLocatorInfo}
		, {"freesat/medialocationinfo/thumbnail", programme_field_MediaLocatorInfoThumnail}
		, {"freesat/shorteventpromotional", programme_field_ShortEventPromotional}

#endif
		//for freeview
#if defined(CONFIG_OP_UK_DTT)
		, {"freeview/guidance", programme_field_UkDttGuidance}
		, {"freeview/hasAudioDescription", programme_field_HasAudioDescription}
		, {"freeview/linkage", programme_field_Linkage}
#endif

	};
	unsigned int	i, n;

	HxLOG_Trace();
	return_if(!self || field == NULL, NULL);
	return_if(thiz->event == NULL, NULL);

	text = (char *)HLIB_TREE_Lookup(thiz->cache.string, field);
	if (text)
		return text;

	n = sizeof(s_fields) / sizeof(s_fields[0]);
	for (i = 0 ; i < n ; i++)
	{

		if (strcmp(s_fields[i].field, field) == 0)
		{
			text = s_fields[i].getter(thiz);

			if (text)
				HLIB_TREE_Insert(thiz->cache.string, (void *)s_fields[i].field, text);
			return text;
		}
	}

	//if (HLIB_STD_StrCaseStartWith(field, "crid://"))	// 음... 체크했음 좋겠는데...
	{
		DxEventCRID_t *crid = programme_GetGroupCRID(thiz, field);
		if (crid)
		{
			HCHAR	szStr[32];

			HxSTD_snprintf(szStr, 32, "0x%x", (HINT32)crid->type);
			text = HLIB_STD_StrDup(szStr);
			if (text)
				HLIB_TREE_Insert(thiz->cache.string, (void *)crid->crid, text);
			return text;
		}
	}

	HxLOG_Warning("%s(field:%s) is NOT implemented yet...!!\n", __FUNCTION__, field);
	return NULL;
}

OPL_Collection_t	*OPL_Programme_GetSiDescription(OPL_ProgrammeHandle self, int tag, int tagExtention)
{
	Programme_t 			*thiz = (Programme_t *)self;
	HCHAR				*tagString= NULL;
	OPL_Collection_t		*coll;
	HINT32			  	sd;
#if defined(CONFIG_OP_JAPAN)
	HxVector_t			*descList = NULL;
	HINT32			  	sd;
	HINT32			  	i =0, j=0, count =0;
	APKS_EventGroupDes_t *eventGroup = NULL;
#endif

	tagExtention = tagExtention;
	switch( tag )
	{
#if defined(CONFIG_OP_JAPAN)
		case TAG_EVENT_GROUP:
			tagString = "eventgroup";
			break;
#endif
		case 0x4D:
			tagString="shortevent";
			break;
		case 0x4E:
			tagString="extendedevent";
			break;
		default:
			return NULL;
	}

	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
#if defined(CONFIG_OP_JAPAN)
	return_if(thiz->event->descriptor == NULL, NULL);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, tagString);
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

	descList = OPL_PARSER_JAPAN_ParsingDesc( thiz->event->descriptor, TAG_EVENT_GROUP );
	if( descList != NULL )
	{
		count = HLIB_VECTOR_Length(descList);
		if(  count > 0 )
		{
			for( 	i=0; i<count; i++ )
			{
				eventGroup= HLIB_VECTOR_ItemAt(descList, i);
				if( eventGroup == NULL )
					continue;

				sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				if(sd == 0)
					continue;

				HLIB_RWSTREAM_Print(sd , "{\"descriptor_tag\" : %d, " , eventGroup->ucDesTag );
				HLIB_RWSTREAM_Print(sd , " \"group_type\" : %d, " , eventGroup->ucGroupType );
				HLIB_RWSTREAM_Print(sd , " \"event_count\" : %d, " , eventGroup->ucEventCount );
				if( eventGroup->ucEventCount > 0 )
				{
					HLIB_RWSTREAM_Print(sd, " \"service_id\" : [" );
					for( j =0; j<eventGroup->ucEventCount; j++ )
					{
						if (j > 0)
							HLIB_RWSTREAM_PutChar(sd, ',');
						HLIB_RWSTREAM_Print(sd, "%d" , eventGroup->pusServiceId[j] );
					}
					HLIB_RWSTREAM_Print(sd, "], " );

					HLIB_RWSTREAM_Print(sd, "\"event_id\" : [" );
					for( j =0; j<eventGroup->ucEventCount; j++ )
					{
						if (j > 0)
							HLIB_RWSTREAM_PutChar(sd, ',');
						HLIB_RWSTREAM_Print(sd, "%d" , eventGroup->pusEventId[j] );
					}
					HLIB_RWSTREAM_Print(sd, "], " );
				}
				HLIB_RWSTREAM_Print(sd , " \"numloop\" : %d, " , eventGroup->ucNumLoop );

				if( eventGroup->pstOtherNetworkInfo )
				{
					HLIB_RWSTREAM_Print(sd, " \"other_network_info\" : [" );
					for( j =0; j< eventGroup->ucNumLoop ; j++)
					{
						if (j > 0)
							HLIB_RWSTREAM_PutChar(sd, ',');
						HLIB_RWSTREAM_Print(sd, "{ \"original_network_id\" : %d, " , eventGroup->pstOtherNetworkInfo[j].usOrigNetworkId );
						HLIB_RWSTREAM_Print(sd, " \"transport_stream_id\" : %d, " , eventGroup->pstOtherNetworkInfo[j].usTransStreamId );
						HLIB_RWSTREAM_Print(sd, " \"service_id\" : %d, " , eventGroup->pstOtherNetworkInfo[j].usServiceId );
						HLIB_RWSTREAM_Print(sd, " \"event_id\" : %d } " , eventGroup->pstOtherNetworkInfo[j].nEventId );
					}
					HLIB_RWSTREAM_Print(sd, "], " );
				}
				HLIB_RWSTREAM_Print(sd , "\"private_data_byte_len\" : %d" , eventGroup->ucPrivateDataByteLen );
				if( eventGroup->ucPrivateDataByteLen > 0 )
				{
					HLIB_RWSTREAM_Print(sd , ", \"private_data_byte\" : %s" , eventGroup->pucDataByte );
				}
				HLIB_RWSTREAM_Print(sd , "}" );

				HxLOG_Print( "[%s,%d] [group info data] \n %s \n" , __FUNCTION__, __LINE__, (HCHAR *)HLIB_RWSTREAM_GetBuf(sd) );

				(void) OPL_Collection_Add(coll, (void *)HLIB_RWSTREAM_CloseWithoutBuf(sd));
				sd = 0;
			}
		}
	}

	if( descList )
	{
		HLIB_VECTOR_Delete( descList );
	}
#else
	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, tagString);
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))programme_CloneSiDescriptor);
	return_if(coll == NULL, NULL);

	/* raw epg -> parsing(dlib) -> raw(opl) ...*/
	{

		HUINT8	descTag = 0;
		HUINT8	descLength = 0;
		HUINT8	descNumAndLastDescNum = 0;	// descNum 4bit + last descNum 4bit = 8bit
//		HxLANG_Id_e langId = 0;
		HUINT8	*pIso639 = NULL;
		HUINT8	iso639[4] = {0,};
		HUINT8	itemLength = 0;
		HUINT8	textLength = 0;
		HUINT8*	text = NULL;
		HxVector_t *textList = NULL;

		if (tag == 0x4D)
		{
			textList = thiz->event->text;
		}
		else if (tag == 0x4E)
		{
			textList = thiz->event->longText;
		}
		if (textList != NULL)
		{
			DxEventString_t *eventString = NULL;
			HxLANG_Id_e language = programme_GetMainLanguage();

			eventString = HLIB_VECTOR_Find (textList, OPL_Programme_FinedEventString, (void *)language);
			if(eventString == NULL)
				eventString = HLIB_VECTOR_First(textList);
			if(eventString && eventString->string)
			{
				pIso639 = HLIB_LANG_IdTo639(eventString->language);
				if (pIso639 != NULL)
				{
					HxSTD_snprintf(iso639,4,"%s",pIso639);
					HLIB_STD_StrUpper(iso639);
				}
				text = eventString->string;
				textLength = HLIB_STD_StrNLen(text,256);
			}
			else
			{
				text = NULL;
			}
		}
		if (text != NULL)
		{
			descTag = tag;
			descLength = 6+textLength;	//descNum 1+Lang 3+ itemLength 1 + textlength 1 = 6
			descNumAndLastDescNum = 0;
			itemLength = 0;

			sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			if(sd)
			{
				HLIB_RWSTREAM_Print(sd, "%c%c%c%3s%c%c%s" ,descTag,descLength,descNumAndLastDescNum,iso639,itemLength,textLength,text);
				(void) OPL_Collection_Add(coll, (void *)HLIB_RWSTREAM_CloseWithoutBuf(sd));
			}
			sd = 0;
		}
	}
#endif
	HLIB_TREE_Insert(thiz->cache.collection, tagString, coll);

	return coll;
}

int				OPL_Programme_GetEventIDFromProgrammeID (const char *programmeID)
{
	int	eventID = 0;

	// dvb://<original_network_id>.[<transport_stream>].<service_id>;<event_id>[;;<service_unique_id>]
	if (programme_ParseDVBURI(programmeID, NULL, NULL, NULL, &eventID, NULL, NULL))
		return eventID;

#if defined(CONFIG_OP_FREESAT)
	if (programme_ParseFSATURI(programmeID, NULL, &eventID))
		return eventID;
#endif

	HxLOG_Error("[%s] Invalid ProgrammeID: %s\n", programmeID);
	return 0;
}

int				OPL_Programme_GetSvcUIDFromProgrammeID (const char *programmeID)
{
	int	id = 0;

	// dvb://<original_network_id>.[<transport_stream>].<service_id>;<event_id>[;;<service_unique_id>]
	if (programme_ParseDVBURI(programmeID, NULL, NULL, NULL, NULL, NULL, &id))
		return id;

#if defined(CONFIG_OP_FREESAT)
	if (programme_ParseFSATURI(programmeID, &id, NULL))
	{
		OPL_Channel_t	*list;
		int				count;

		count = OPL_Channel_GetFilteredChannelList(NULL, &list, programme_FsatServiceIDFilter, (void *)id);
		if (count > 0)
		{
			id = OPL_Channel_GetUniqueID(list[0]);
			while (count--)
				OPL_Channel_Delete(list[count]);
			HLIB_STD_MemFree(list);
			return id;
		}
	}
#endif

	HxLOG_Error("[%s] Invalid ProgrammeID: %s\n", programmeID);
	return 0;
}

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
void			OPL_Programme_MakeProgrammeID (char *buf, unsigned int n, int onid, int tsid, int svcid, int eventId)
{
	OPL_Channel_t	channel;

	// dvb://<original_network_id>.[<transport_stream>].<service_id>;<event_id>[;;<service_unique_id>]
	channel = OPL_Channel_FindChannelByTriplet(NULL, onid, tsid, svcid);
	if (channel)
	{
		HxSTD_PrintToStrN(buf, n
			, "dvb://%x.%x.%x;%x;;%x"
			, onid
			, tsid
			, svcid
			, eventId
			, OPL_Channel_GetUniqueID (channel)
		);
	}
	else
	{
		HxSTD_PrintToStrN(buf, n
			, "dvb://..;%x;;%x"
			, eventId
			, OPL_Channel_GetUniqueID (channel)
		);
	}
}
#else
void			OPL_Programme_MakeProgrammeID (char *buf, unsigned int n, int svcuid, int eventId)
{
	OPL_Channel_t	channel;

	// dvb://<original_network_id>.[<transport_stream>].<service_id>;<event_id>[;;<service_unique_id>]
	channel = OPL_Channel_FindChannelByUID(NULL, svcuid);
	if (channel)
	{
		HxSTD_PrintToStrN(buf, n
			, "dvb://%x.%x.%x;%x;;%x"
			, OPL_Channel_GetOrgNetworkID(channel)
			, OPL_Channel_GetTransponderID(channel)
			, OPL_Channel_GetServiceID(channel)
			, eventId
			, svcuid
		);
	}
	else
	{
		HxSTD_PrintToStrN(buf, n
			, "dvb://..;%x;;%x"
			, eventId
			, svcuid
		);
	}
}
#endif



int				OPL_Programme_GetIDTypeFromProgrammeID (const char *programmeID)
{
	/**
	 * Programme.programmeIDType property
	 *
	 * ID_TVA_CRID			0
	 *    - dvb://<original_network_id>.[<transport_stream>].<service_id>;[<event_id>];<TVA_id>[;<service_unique_id>]
	 *
	 * ID_DVB_EVENT			1
	 *    - dvb://<original_network_id>.[<transport_stream>].<service_id>;<event_id>[;;<service_unique_id>]
	 *
	 * ID_TVA_GROUP_CRID	2
	 * ID_FSAT_EVENT		16	//'F'
	 *    - fsat://<service_id-digit>;<event_id-hex>
	 */
	if (HLIB_STD_StrStartWith(programmeID, "dvb://"))
		return eOPL_ProgrammeID_DVB_EVENT;	// ID_DVB_EVENT

	if (HLIB_STD_StrStartWith(programmeID, "crid://"))
		return eOPL_ProgrammeID_TVA_GROUP_CRID;

#ifdef	CONFIG_OP_FREESAT
	if (HLIB_STD_StrStartWith(programmeID, "fsat://"))
		return eOPL_ProgrammeID_FSAT_EVENT;
#endif

	HxLOG_Critical("%s(%s) Unknown programmeIDType!!!\n", __FUNCTION__, programmeID);
	return eOPL_ProgrammeID_DVB_EVENT;
}


OPL_BOOL		OPL_Programme_GetUniqueIDsFromProgrammeID (const char *programmeID, int *svcuid, int *eventid)
{
	if (programme_ParseDVBURI(programmeID, NULL, NULL, NULL, eventid, NULL, svcuid))
		return OPL_TRUE;

#ifdef	CONFIG_OP_FREESAT
	{
		int	fsatSvcId;

		if (programme_ParseFSATURI(programmeID, &fsatSvcId, eventid))
		{
			if (svcuid)
			{
				OPL_Channel_t	*list;
				int				count;

				count = OPL_Channel_GetFilteredChannelList(NULL, &list
									, programme_FsatServiceIDFilter, (void *)fsatSvcId);
				if (count == 0)
					return OPL_FALSE;

				*svcuid = OPL_Channel_GetUniqueID(list[0]);
				while (count--)
					OPL_Channel_Delete(list[count]);
				HLIB_STD_MemFree(list);
			}
			return OPL_TRUE;
		}
	}
#endif
	return OPL_FALSE;
}

OPL_BOOL		OPL_Programme_GetTrippleIDFromProgrammeID (const char *programmeID, int *onid, int *tsid, int *svcid)
{
	if (programme_ParseDVBURI(programmeID, onid, tsid, svcid, NULL, NULL, NULL))
		return OPL_TRUE;

	return OPL_FALSE;
}

OPL_Collection_t	*OPL_Programme_GetComponents (OPL_ProgrammeHandle self, eOplAVComponentType eType )
{
	Programme_t 		*thiz = (Programme_t *)self;
	OPL_Collection_t	*coll = NULL;

	HxLOG_Trace();
	if (NULL == thiz)
	{
		HxLOG_Error("[%s] Invalid Argument is NULL\n");
		return NULL;
	}

	switch (eType)
	{
	case OPLAVCOMPONENT_TYPE_VIDEO:
		coll = programme_GetVideoComponent(thiz);
		break;

	case OPLAVCOMPONENT_TYPE_AUDIO:
		coll = programme_GetAudioComponent (thiz);
		break;

	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		coll = programme_GetDataComponent (thiz, eType);
		break;

	case OPLAVCOMPONENT_TYPE_DATA:
		coll = programme_GetDataComponent(thiz, eType);
		break;

	default:
		HxLOG_Error("[%s] Invalid Component Type : %d \n", eType);
		return NULL;
	}

	HxLOG_Trace();
	return coll;
}

HERROR	OPL_Programme_GetMultiViewComponent( OPL_ProgrammeHandle self , HxVector_t *descList )
{
	Programme_t 				*thiz = (Programme_t *)self;
#if defined( CONFIG_OP_JAPAN)
	HxVector_t 				*retDescList = NULL;
#endif

	descList = NULL;

	if (NULL == thiz)
	{
		HxLOG_Error("[%s] Invalid Argument is NULL\n");
		return ERR_FAIL;
	}

	/* input data */
	HxLOG_Trace();
	return_if(!thiz, ERR_FAIL);
	return_if(thiz->event == NULL, ERR_FAIL);
	return_if(thiz->event->descriptor == NULL, ERR_FAIL);

#if defined( CONFIG_OP_JAPAN)
	retDescList = OPL_PARSER_JAPAN_ParsingDesc( thiz->event->descriptor, TAG_COMPONENT_GROUP );
	descList = retDescList; // prevent mj , unsed pointer value
#endif
	/* 반드시 받는 쪽에서 descList 해제 해 주어야 한다. !!!! */
	return ERR_OK;
}

int OPL_Programme_GetCopyControl(OPL_ProgrammeHandle self, eCopyControlType eType )
{
	Programme_t 		*thiz = (Programme_t *)self;
#if defined(CONFIG_OP_JAPAN)
	HINT32			nCopyControl = -1;
#endif
	HINT32			eRetVal = 0;

	return_if(!thiz, 0);
	return_if(thiz->event == NULL, 0);

#if defined(CONFIG_OP_JAPAN)
	/* PMT 1순위 처리 */
	nCopyControl = programme_GetPmtCopyControl(thiz, eType );

	/* EIT 2순위 처리 */
	if( nCopyControl == -1 )
	{
		return_if(thiz->event->descriptor == NULL, 0);
		nCopyControl = programme_GetCopyControl( thiz->event->descriptor, thiz->event->svcuid, eType );
	}

	/* SDT 3순위 처리 */
	if( nCopyControl == -1 )
	{
		nCopyControl = programme_GetSdtCopyControl( thiz, eType );
	}

	switch( nCopyControl )
	{
		case ARIB_COPY_FREE:
			eRetVal = OUTPUT_COPY_FREE;
			break;

		case ARIB_COPY_ONCE:
		case ARIB_COPY_10:
			eRetVal = OUTPUT_COPY_1_OR_10;
			break;

		case ARIB_OUTPUT_NEVER:
			eRetVal = OUTPUT_NEVER;
			break;

		case ARIB_COPY_NOMORE:
		case ARIB_COPY_NEVER:
		case ARIB_COPY_FREE_WITH_EPN:
			eRetVal = OUTPUT_COPY_NEVER;
			break;

		default:
			eRetVal = OUTPUT_COPY_FREE;
			break;
	}
#endif

	return eRetVal;
}

OPL_BOOL		OPL_Programme_GetIs3D(OPL_ProgrammeHandle self)
{
#if defined(CONFIG_OP_JAPAN)
	Programme_t			*thiz = (Programme_t *)self;
#endif
	OPL_BOOL			bRetVal = OPL_FALSE;

#if defined(CONFIG_OP_JAPAN)
	HINT32				i =0;
	HUINT16				word = 0;

	HxLOG_Trace();
	return_if(!self, bRetVal);
	/* input data */
	HxLOG_Trace();
	return_if(!thiz, OPL_FALSE);
	return_if(thiz->event == NULL, OPL_FALSE);

	for (i = 0 ; i <DxEVENT_CONTENT ; i++)
	{
		word = thiz->event->content[i];
		if (word == 0)
			continue;

		if( word == 0xe020 )
		{/* spec trb15 2분책 - 부록B , 표 B-1, p4-328 */
			bRetVal = OPL_TRUE;
			break;
		}
	}
#endif

	return bRetVal;
}

OPL_BOOL		OPL_Programme_GetIsCharged(OPL_ProgrammeHandle self)
{
	Programme_t	*thiz = (Programme_t *)self;
	OPL_BOOL	bRetVal = OPL_FALSE;

	HxLOG_Trace();
	return_if(!self, bRetVal);

	if (thiz->event && thiz->event->freeCaMode )
	{
		bRetVal = OPL_TRUE;
	}

	return bRetVal;
}

const char	*OPL_Programme_GetContractVerificationInfo(OPL_ProgrammeHandle self)
{
	Programme_t					*thiz = (Programme_t *)self;
	HINT32			  			sd;
	HCHAR						*info = NULL;
#if defined(CONFIG_OP_JAPAN)
	APKS_CaContractInfoDes_t		*pstCaContractInfo = NULL;
	HINT32						count =0, i=0;
	HCHAR						*data = NULL;
	HBOOL						bMakeData = FALSE;
	HxVector_t					*descList = NULL;
	DxDes_t 						*pstDes = NULL;
	OPL_Channel_t					stChannel;
	HINT32						nOnId = 0, nSvcId = 0;
#endif
	if (NULL == thiz)
	{
		HxLOG_Error("[%s] Invalid Argument is NULL\n");
		return NULL;
	}
	info = (char *)HLIB_TREE_Lookup(thiz->cache.string, "contractverificationinfo");
	if( info == NULL )
	{
		/* input data */
		HxLOG_Trace();
		return_if(!thiz, 0);
		return_if(thiz->event == NULL, 0);
		return_if(thiz->event->descriptor == NULL, 0);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

#if defined(CONFIG_OP_JAPAN)
		descList = OPL_PARSER_JAPAN_ParsingDesc( thiz->event->descriptor, TAG_CA_CONTRACT_INFOMATION );
		if( descList != NULL )
		{
			count = HLIB_VECTOR_Length(descList);
			if(  count > 0 )
			{
				for( i=0; i< count; i++)
				{
					pstCaContractInfo = (APKS_CaContractInfoDes_t *)HLIB_VECTOR_ItemAt(descList, i);

					if( pstCaContractInfo == NULL )
						continue;

					bMakeData = TRUE;
					HLIB_RWSTREAM_Print(sd , "{\"length\" : %d, " , pstCaContractInfo->ucContractVerificationInfoLength );
					HLIB_RWSTREAM_Print(sd , "\"data\" : " );
					if( (pstCaContractInfo->ucContractVerificationInfoLength > 0) && pstCaContractInfo->pucContractVerificationInfo )
					{
						data = (HCHAR *)HLIB_STD_MemAlloc(pstCaContractInfo->ucContractVerificationInfoLength *2);

						if(data == NULL)
							return NULL;

						HLIB_MATH_EncodeBase64(data, pstCaContractInfo->pucContractVerificationInfo, pstCaContractInfo->ucContractVerificationInfoLength );

						if( data != NULL )
						{
							HLIB_RWSTREAM_Print(sd , "\"%s\"", data );
							HLIB_STD_MemFree( data );
						}
					}
					HLIB_RWSTREAM_Print(sd , "}" );
				}
			}

			HLIB_VECTOR_Delete( descList );
		}

		if( count <= 0 )
		{/* EIT에 없으면 SDT로 */
			memset( &stChannel, 0x00, sizeof( OPL_Channel_t) );

			stChannel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);

			if( stChannel )
			{
				nOnId = OPL_Channel_GetOrgNetworkID( stChannel );
				nSvcId = OPL_Channel_GetServiceID( stChannel );

				pstDes = APK_META_DES_GetWithNetIdAndSvcId( nOnId , nSvcId );
				if( pstDes )
				{
					if( pstDes->ucNumCaContractInfoDes > 0 )
					{
						bMakeData = TRUE;
						HLIB_RWSTREAM_Print(sd , "{\"length\" : %d, " , pstDes->astCaContractInfoDes->ucContractVerificationInfoLength );
						HLIB_RWSTREAM_Print(sd , "\"data\" : " );
						if( (pstDes->astCaContractInfoDes->ucContractVerificationInfoLength > 0) && pstDes->astCaContractInfoDes->aucContractVerificationInfo )
						{
							data = (HCHAR *)HLIB_STD_MemAlloc(pstDes->astCaContractInfoDes->ucContractVerificationInfoLength *2);

							if(data == NULL)
								return NULL;

							HLIB_MATH_EncodeBase64(data, pstDes->astCaContractInfoDes->aucContractVerificationInfo, pstDes->astCaContractInfoDes->ucContractVerificationInfoLength );

							if( data != NULL )
							{
								HLIB_RWSTREAM_Print(sd , "\"%s\"", data );
								HLIB_STD_MemFree( data );
							}
						}
						HLIB_RWSTREAM_Print(sd , "}" );
					}

					APK_META_DES_Delete( pstDes );
				}
			}
		}

		if( bMakeData == FALSE )
		{/* default value */
			HLIB_RWSTREAM_Print(sd , "{\"length\" : %d, " , 0 );
			HLIB_RWSTREAM_Print(sd , "\"data\" : \"\"}" );
		}
#endif

		info = HLIB_RWSTREAM_CloseWithoutBuf(sd);
		if( info != NULL )
		{
			HLIB_TREE_Insert(thiz->cache.string, "contractverificationinfo", (void *)info);
		}
	}

	HxLOG_Print( "[%s,%d] data : %s \n" , __FUNCTION__ ,__LINE__ ,info);

	return info;
}

int		OPL_Programme_GetRecordingStatus(OPL_ProgrammeHandle self)
{
	Programme_t *thiz = (Programme_t *)self;
	const char	*id;
	OPL_ScheduledItem_t *item = NULL;
	int			retVal = 0;

	HxLOG_Trace();
	return_if(thiz == NULL, retVal);

	id = OPL_Programme_GetProgrammeID(self);
	return_if(id == NULL, retVal);

	item = OPL_Scheduler_FindSchedule(OPL_Scheduler_GetInstance(), id);
	if( item )
	{
		switch( item->type )
		{
#ifdef JLABS
			case eOPL_SCHEDULED_PRO_DLNA:
				retVal = 3;
				break;
#endif
			case eOPL_SCHEDULED_PRO_RECORD:
				retVal = 1;
				break;

			case eOPL_SCHEDULED_PRO_WATCH:
				retVal = 2;
				break;

			case eOPL_SCHEDULED_PRO_UNKNOWN:
			default:
				return 0;
		}
	}

	return retVal;
}

unsigned long OPL_Programme_GetEpgType(OPL_ProgrammeHandle self)
{
	Programme_t      *thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if(!self, 0);
	return_if(thiz->event == NULL, 0);

	return thiz->event->type;
}

const char *	OPL_Programme_GetProgrammeIDSubtype(OPL_ProgrammeHandle self)
{
	Programme_t *thiz = (Programme_t *)self;
	char	*text;

	HxLOG_Trace();
	return_if(!self, NULL);

	text = (char *)HLIB_TREE_Lookup(thiz->cache.string, "programmeIDSubtype");
	if (text)
	{
		return text;
	}
	return_if(thiz->event == NULL, NULL);

	text = HLIB_STD_StrDup(DxEPG_NAMEOF(thiz->event->type));
	if (text)
	{
		HLIB_TREE_Insert(thiz->cache.string, "programmeIDSubtype", text);
	}
	return text;
}

OPL_Collection_t	*OPL_Programme_GetSharedProgrammeIds(OPL_ProgrammeHandle self)
{
	Programme_t 			*thiz = (Programme_t *)self;
	OPL_Collection_t		*coll;
#if defined(CONFIG_OP_JAPAN)
	HxVector_t			*descList = NULL;
	HINT32			  	sd =0, nOnId =0, nTsId =0;
	HINT32			  	i =0, j=0, count =0;
	APKS_EventGroupDes_t *eventGroup = NULL;
	OPL_Channel_t			stChannel = NULL;
#endif

	HxLOG_Trace();

	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->descriptor == NULL, NULL);

	/* svc type 정보 */
#if	defined(CONFIG_OP_JAPAN)
	stChannel = OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
	if (stChannel)
	{
		nOnId = OPL_Channel_GetOrgNetworkID( stChannel );
		nTsId = OPL_Channel_GetTransponderID( stChannel );
	}
#endif

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "sharedprogrammeids");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

#if defined(CONFIG_OP_JAPAN)
	descList = OPL_PARSER_JAPAN_ParsingDesc( thiz->event->descriptor, TAG_EVENT_GROUP );
	if( descList != NULL )
	{
		count = HLIB_VECTOR_Length(descList);
		if(  count > 0 )
		{
			for( 	i=0; i<count; i++ )
			{
				eventGroup= HLIB_VECTOR_ItemAt(descList, i);
				if( eventGroup == NULL )
					continue;

				if( (eventGroup->ucEventCount > 0) && (eventGroup->ucGroupType == 0x01) )
				{/* 공유로만 처리한다. */
					for( j =0; j<eventGroup->ucEventCount; j++ )
					{
						sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
						if(sd == 0)
							continue;

						HLIB_RWSTREAM_Print(sd, "%x,%x,%x,%x" , nOnId, nTsId, eventGroup->pusServiceId[j], eventGroup->pusEventId[j] );

						HxLOG_Print( "[%s,%d] [group info data] \n %s \n" , __FUNCTION__, __LINE__, (HCHAR *)HLIB_RWSTREAM_GetBuf(sd) );

						(void) OPL_Collection_Add(coll, (void *)HLIB_RWSTREAM_CloseWithoutBuf(sd));
						sd = 0;
					}
				}

			}
		}
	}

	if( descList )
	{
		HLIB_VECTOR_Delete( descList );
	}

#endif

	HLIB_TREE_Insert(thiz->cache.collection, "sharedprogrammeids", coll);

	return coll;
}

#define TEMP_STRING_SIZE 100
OPL_ProgrammeHandle	OPL_Programme_GetSharedProgrammeInfo(const char* pszSharedProgrammeId)
{
	HINT32					i =0;
	HINT32					nOnId =0, nTsId =0, nSvcId =0, nEvtId =0, nSvcUid =0;
	OPL_ProgrammeHandle		sharedProgramme = NULL;
	HCHAR 					*tok = NULL;
	HCHAR					szTemp[TEMP_STRING_SIZE];
	OPL_Channel_t				sharedChannel = NULL;
	DxEvent_t				*sharedEvent = NULL;

	HxLOG_Trace();

	return_if(pszSharedProgrammeId == NULL, NULL);

	/* 공유 내용 파싱.. */
	memset( szTemp , 0x00 , TEMP_STRING_SIZE );
	strncpy( szTemp, pszSharedProgrammeId, (TEMP_STRING_SIZE-1) );

	HxLOG_Print( "[%s,%d] id : %s , szTemp : %s \n" , __FUNCTION__ , __LINE__, pszSharedProgrammeId , szTemp);

	tok = strtok(szTemp,",");
	while(tok != NULL)
	{
		switch( i )
		{
			case 0:
				nOnId = HLIB_STD_HexToInt( tok );
				break;
			case 1:
				nTsId = HLIB_STD_HexToInt( tok );
				break;
			case 2:
				nSvcId = HLIB_STD_HexToInt( tok );
				break;
			case 3:
				nEvtId = HLIB_STD_HexToInt( tok );
				break;
			default:
				break;
		}

		tok = strtok(NULL,",");
		i++;
	}
	HxLOG_Print( "[%s,%d] 0x%x , 0x%x, 0x%x, 0x%x \n" , __FUNCTION__ , __LINE__ , nOnId , nTsId, nSvcId, nEvtId);

	/* 해당 채널을 찾아 uid를 얻는다. */
	sharedChannel = OPL_Channel_FindChannelByTriplet( OPL_Channel_GetManager() , nOnId, nTsId, nSvcId );
	return_if(sharedChannel == NULL, NULL);
	nSvcUid = OPL_Channel_GetUniqueID(sharedChannel);
	return_if(nSvcUid == 0 , NULL );

	/* uid를 통해서 evt id로 programme 을 얻어온다. */
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	sharedEvent = APK_META_QUERY_GetEventWithIds( nOnId , nTsId, nSvcId, nEvtId );
#else
	sharedEvent = APK_META_QUERY_GetEventWithIds( nSvcUid, nEvtId );
#endif
	sharedProgramme = OPL_Programme_New("o", APK_EVENT_Incref(sharedEvent));
	if( sharedProgramme == NULL )
	{
		APK_EVENT_Delete( sharedEvent );
		return NULL;
	}

	return sharedProgramme;
}

HCHAR *OPL_PROGRAMME_GetGenre (HUINT16 content)
{
	HUINT8 			contentNibble =0;
	HUINT8 			ucMajorNibble =0, ucMinorNibble =0;
	HINT32			sd = 0;

	contentNibble = (HUINT8)(content >> 8);

	ucMajorNibble = contentNibble >> 4;
	ucMinorNibble = contentNibble & 0xf;

	HxLOG_Print("[%s,%d] major : %d \n" , __FUNCTION__ , __LINE__, ucMajorNibble);
	HxLOG_Print("[%s,%d] minor : %d \n" , __FUNCTION__ , __LINE__, ucMinorNibble);

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if( sd == 0 )
		return NULL;

	HLIB_RWSTREAM_Print(sd, "%x%x",  ucMajorNibble, ucMinorNibble );

	return (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
}

