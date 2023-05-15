/*************************************************************************************************************
 	@file 		Programme.cpp
 	Defines 	DVB programme routine
 	@author  	Sangsoo Yoo(ssyoo@humaxdigital.com)
 	@date    	2014/01/01
 	@attention  Copyright (c) 2011 Humax - All rights reserved.

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*************************************************************************************************************/
#include <apk.h>
#include <hlib.h>
#include <thapi.h>

#include "oplscheduler.h"
#include "oplprogramme.hpp"

#define	return_if_void(expr)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	return_if(expr, err)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	goto_if(expr, pos)		do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

OPL_BOOL oplprogramme::programme_HexToInt(const char *s, int *ret)
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

OPL_BOOL	oplprogramme::programme_ParseDVBURI (const char *uri, int *onid, int *tsid, int *sid, int *eid, char *tvaID, int *svcuid)
{
	char 	*ret;
	char	field[128];
	unsigned int	adv;

	return_if( strncmp(uri, "dvb://", 6) != 0, OPL_FALSE );
	uri += 6;

	// onid
	return_if( (ret = strchr((char*)uri, '.')) == NULL, OPL_FALSE );
	adv = (unsigned int)ret - (unsigned int)uri;
	if (adv && onid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		return_if( !programme_HexToInt(field, onid), OPL_FALSE );
	}
	uri += adv + 1;

	// tsid
	return_if( (ret = strchr((char*)uri, '.')) == NULL, OPL_FALSE );
	adv = (unsigned int)ret - (unsigned int)uri;
	if (adv && tsid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		return_if( !programme_HexToInt(field, tsid), OPL_FALSE );
	}
	uri += adv + 1;

	// sid
	adv = (unsigned int)ret - (unsigned int)uri;
	if (adv && sid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		return_if( !programme_HexToInt(field, sid), OPL_FALSE );
	}
	uri += adv + 1;

	// eid
	if ((ret = strchr((char*)uri, ';')) == NULL)
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
	if ((ret = strchr((char*)uri, ';')) == NULL)
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
HxLANG_Id_e oplprogramme::programme_GetAudioLanguage (int num)
{
	HCHAR	buf[4];

	if (APK_SCENARIO_GetAudioLanguage(num, buf) != ERR_OK)
		return eLangID_NONE;
	return HLIB_LANG_639ToId(buf);
}

static HxLANG_Id_e	programme_GetMainLanguage ()
{
	HCHAR	buf[4];

	if (APK_SCENARIO_GetMainLanguage(buf) != ERR_OK)
		return eLangID_NONE;
	return HLIB_LANG_639ToId(buf);
}


static int	programme_GetAudioType (DxEventComponent_t *comp, HxLANG_Id_e language)
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

static int	programme_GetSubtitleType (DxEventComponent_t *comp, HxLANG_Id_e language)
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

const char* oplprogramme::programme_GetGenre (HUINT16 content)
{
	HUINT8 contentNibble =0, userNibble =0;

	contentNibble = (HUINT8)(content >> 8);
	userNibble = (HUINT8)(content | 0xff );

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
	static const char	*s_ukdttNibble1[] = {
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
			break;
		case eDxOPERATOR_BBC:
			return HLIB_STD_StrDup(s_ukdttNibble1[contentNibble >> 4]);

		default:
			return getGenreString( content );
			break;
	}

	return HLIB_STD_StrDup(s_dvbNibble1[contentNibble >> 4]);
}

char* oplprogramme::programme_field_drm()
{
	HINT32			sd;
	HULONG 			ulEpgType;
	HUINT8			drm;
	Programme_t *thiz = (Programme_t*)self;
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = getEpgType();
	if (ulEpgType == eDxEPG_TYPE_IP) {
		drm = thiz->event->extension.ipEpg.drm;
		return_if(drm == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%c", drm);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

char* oplprogramme::programme_field_season()
{
	HINT32			sd;
	HULONG 			ulEpgType;
	HUINT32			season;
	Programme_t *thiz = (Programme_t*)self;
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = getEpgType();
	if (ulEpgType == eDxEPG_TYPE_IP) {
		season = thiz->event->extension.ipEpg.season;
		return_if(season == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%d", season);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		season = thiz->event->extension.tvtvEpg.season;
		return_if(season == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%d", season);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

char* oplprogramme::programme_field_seriesID()
{
	HINT32			sd;
	HULONG 		ulEpgType;
	HUINT32			seriesId;
	Programme_t *thiz = (Programme_t*)self;
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = getEpgType();
	if (ulEpgType == eDxEPG_TYPE_IP) {
		seriesId = thiz->event->extension.ipEpg.seriesId;
		return_if(seriesId == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%d", seriesId);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		seriesId = thiz->event->extension.tvtvEpg.seriesId;
		return_if(seriesId == 0, NULL);

		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%d", seriesId);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

char* oplprogramme::programme_field_thumbnailURL()
{
	HINT32			sd;
	HULONG 			ulEpgType;
	Programme_t *thiz = (Programme_t*)self;
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = getEpgType();
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

				return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
			}

			//	THAPI Result == NULL이면 기본 URL을 돌려줌.
			HLIB_RWSTREAM_Close(sd);
			return HLIB_STD_StrDup(thiz->event->extension.ipEpg.thumbnailURL);
		}
//		HLIB_RWSTREAM_Print(sd, "%s", THAPI_GetThumbnailURL(thiz->event->extension.ipEpg.thumbnailURL, 0, "/mnt/hd1"));
//		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
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

				return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
			}

			//	THAPI Result == NULL이면 기본 URL을 돌려줌.
			HLIB_RWSTREAM_Close(sd);
			return HLIB_STD_StrDup(thiz->event->extension.tvtvEpg.thumbnailURL);
		}
//		HLIB_RWSTREAM_Print(sd, "%s", THAPI_GetThumbnailURL(thiz->event->extension.ipEpg.thumbnailURL, 0, "/mnt/hd1"));
//		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

char* oplprogramme::programme_field_actor()
{
	HINT32			sd;
	HULONG 			ulEpgType;
	Programme_t *thiz = (Programme_t*)self;
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = getEpgType();
	if (ulEpgType == eDxEPG_TYPE_IP){
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.ipEpg.actor);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.tvtvEpg.actor);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

char* oplprogramme::programme_field_actress()
{
	HINT32			sd;
	HULONG 			ulEpgType;
	Programme_t *thiz = (Programme_t*)self;
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = getEpgType();

	if (ulEpgType == eDxEPG_TYPE_IP) {
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.ipEpg.actress);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.tvtvEpg.actress);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

char* oplprogramme::programme_field_director()
{
	HINT32			sd;
	HULONG 			ulEpgType;
	Programme_t *thiz = (Programme_t*)self;
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = getEpgType();

	if (ulEpgType == eDxEPG_TYPE_IP) {
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.ipEpg.director);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.tvtvEpg.director);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);

	}
	(void)thiz;

	return NULL;
}

char* oplprogramme::programme_field_person()
{
	HINT32			sd;
	HULONG 			ulEpgType;
	Programme_t *thiz = (Programme_t*)self;
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);

	ulEpgType = getEpgType();

	if (ulEpgType == eDxEPG_TYPE_IP) {
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.ipEpg.person);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	else if (ulEpgType == eDxEPG_TYPE_TVTV)
	{
		sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		return_if(sd == 0, NULL);

		HLIB_RWSTREAM_Print(sd, "%s", thiz->event->extension.tvtvEpg.person);
		return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	(void)thiz;

	return NULL;
}

HERROR	oplprogramme::programme_CheckMultiViewComponentIndex( HUINT8 ucCompTag, HUINT32 ulCurNum, HINT32 *nRetMultiCompNum )
{
	HERROR						hErr = ERR_FAIL;
	Programme_t *thiz = (Programme_t*)self;
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

	return hErr;
}

OPL_Collection_t* oplprogramme::programme_GetVideoComponent( )
{
	HINT32				i =0, n =0, tempMultiView =0;
	OPL_Collection_t		*coll = NULL;
	DxEventComponent_t	*videoComp = NULL;
	OplAVComponent_t		*tempAVComp = NULL;
	HERROR				hErr = ERR_FAIL;
	Programme_t *thiz = (Programme_t*)self;
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
		if( videoComp->text )
		{
			HLIB_STD_StrNCpySafe( tempAVComp->szText,  videoComp->text , OPLAVCOMPONENT_TEXT_SIZE );
		}

		do{
			tempMultiView = 0;
			hErr = programme_CheckMultiViewComponentIndex( tempAVComp->ulComponentTag, tempAVComp->nMultiViewNum, &tempMultiView );
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
	HLIB_TREE_Insert(thiz->cache.collection, (void*)"videocomponent", coll);

	return coll;
}

OPL_Collection_t* oplprogramme::programme_GetAudioComponent()
{
	OPL_Collection_t				*coll = NULL;
	Programme_t *thiz = (Programme_t*)self;
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

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, (void*)"audiocomponent", coll);

	return coll;
}


OPL_Collection_t* oplprogramme::programme_GetDataComponent( eOplAVComponentType eType  )
{
	OPL_Collection_t				*coll = NULL;
	HCHAR						*inputString = NULL;
	Programme_t *thiz = (Programme_t*)self;

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
			inputString = (HCHAR*)"datacomponent";
			break;
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			inputString = (HCHAR*)"subtitlecomponent";
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

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}

	HLIB_TREE_Insert(thiz->cache.collection, inputString, coll);

	return coll;
}

char* oplprogramme::programme_field_DescriptionItems ()
{
	HINT32	i, n, c;
	HINT32	sd;
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();

	if (thiz->event->item == NULL)
	{
		return NULL;
	}

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	return_if(sd == 0, NULL);

	HLIB_RWSTREAM_Print(sd, "{\"descriptionItems\":[");
	n = HLIB_VECTOR_Length(thiz->event->item);
	for (i = 0, c = 0 ; i < n ; i++)
	{
		DxEventItem_t *item = (DxEventItem_t *)HLIB_VECTOR_ItemAt(thiz->event->item, i);
		if (item == NULL)
			continue;
		if (c > 0) HLIB_RWSTREAM_PutChar(sd, ',');


		if(item->description)
		{
			{
				HLIB_RWSTREAM_Print(sd, "{\"%s\":\"%s\"}"
						, item->description
						, item->item
					);
			}
		}
		c++;
	}
	HLIB_RWSTREAM_Print(sd, "]}");
	return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
}

char *	oplprogramme::programme_field_HardOfHearing ()
{
	Programme_t *thiz = (Programme_t*)self;
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

char *	oplprogramme::programme_field_isAAC ()
{
	Programme_t *thiz = (Programme_t*)self;
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




HCHAR* oplprogramme::programme_FsatDescriptor(HBOOL	bextdesc, HUINT8 desctag)
{
	static const HUINT32		FsatPrivateSpecifierValue = 0x46534154;	//(FSAT)
	const HUINT8				*desc;
	HUINT32					ulPrivateSpecifyValue = 0;
	HBOOL					bPrivateConfirm = FALSE;
	Programme_t *thiz = (Programme_t*)self;

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
						bPrivateConfirm = TRUE;
					}
				}
				else if(desc[0] == 0x0F)
				{
					bPrivateConfirm = FALSE;
				}

			});
		}
	}

	return NULL;
}

char* oplprogramme::programme_field_isAC3 ()
{
	Programme_t *thiz = (Programme_t*)self;
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

char* oplprogramme::programme_field_isAC3Plus ()
{
	Programme_t *thiz = (Programme_t*)self;
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

char* oplprogramme::programme_field_RunningStatus()
{
	Programme_t *thiz = (Programme_t*)self;
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

char* oplprogramme::programme_field_Guidance()
{
	return NULL;
}
char* oplprogramme::programme_field_OnlineAvailability()
{
	char *result = NULL;
	Programme_t *thiz = (Programme_t*)self;
	HxLOG_Trace();
	return_if(!thiz, NULL);
	return_if(thiz->event == NULL, NULL);


	result = programme_FsatDescriptor (TRUE, 0xDE);
	if(result == NULL)
	{
		(void)thiz;
		return NULL;
	}
	return result;

}

char* oplprogramme::programme_field_HasAudioDescription()
{
	Programme_t *thiz = (Programme_t*)self;
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
	return HLIB_STD_StrDup("false");
}

char* oplprogramme::programme_field_HasSigned()
{
	Programme_t *thiz = (Programme_t*)self;
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
	return HLIB_STD_StrDup("false");
}

char* oplprogramme::programme_field_HDLinkage()
{
	HINT32				i, n;
	DxEventLinkage_t	*hdlinkageinfo;
	HINT32	sd;
	char	channelID[256],ProgrammeID[256];
	OPL_Channel_t	channel;
	HBOOL			simulcast;
	HINT32			evtid;
	HINT32			svcuid;
	Programme_t *thiz = (Programme_t*)self;



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
	return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);

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

static HBOOL	OPL_Programme_FinedEventString(void *data, void *language)
{
	DxEventString_t *pstEventString = (DxEventString_t *)data;

	if(pstEventString && pstEventString->language == *((HxLANG_Id_e *)language))
		return TRUE;
	else
		return FALSE;
}


void oplprogramme::initFunctionMap()
{
	map_pFunction["descriptionItems"] = &oplprogramme::programme_field_DescriptionItems;
	map_pFunction["hardOfHearing"] = &oplprogramme::programme_field_HardOfHearing;
	map_pFunction["isAAC"] = &oplprogramme::programme_field_isAAC;
	map_pFunction["isAC3"] = &oplprogramme::programme_field_isAC3;
	map_pFunction["isAC3P"] = &oplprogramme::programme_field_isAC3Plus;
	map_pFunction["runningstatus"] = &oplprogramme::programme_field_RunningStatus;
	map_pFunction["drm"] = &oplprogramme::programme_field_drm;
	map_pFunction["season"] = &oplprogramme::programme_field_season;
	map_pFunction["seriesID"] = &oplprogramme::programme_field_seriesID;
	map_pFunction["thumbnailURL"] = &oplprogramme::programme_field_thumbnailURL;
	map_pFunction["actor"] = &oplprogramme::programme_field_actor;
	map_pFunction["actress"] = &oplprogramme::programme_field_actress;
	map_pFunction["director"] = &oplprogramme::programme_field_director;
	map_pFunction["person"] = &oplprogramme::programme_field_person;
		// for freesat
	map_pFunction["freesat/guidance"] = &oplprogramme::programme_field_Guidance;
	map_pFunction["freesat/hasAudioDescription"] = &oplprogramme::programme_field_HasAudioDescription;
	map_pFunction["freesat/hasSigned"] = &oplprogramme::programme_field_HasSigned;
	map_pFunction["freesat/HDLinkage"] = &oplprogramme::programme_field_HDLinkage;
	map_pFunction["freesat/onlineavailability"] = &oplprogramme::programme_field_OnlineAvailability;
}
oplprogramme::oplprogramme()
{
	initFunctionMap();

	HxLOG_Trace();

	self = (Programme_t *)HLIB_STD_MemCalloc(sizeof(Programme_t));
	return_if_void(self == NULL);

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
FAILED:
	return;
}

oplprogramme::~oplprogramme()
{
	Programme_t *thiz = (Programme_t*)self;
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

oplprogramme::oplprogramme(oplprogramme& ref)
{
	Programme_t *ref_thiz;
	initFunctionMap();

	HxLOG_Trace();

	self = (Programme_t *)HLIB_STD_MemCalloc(sizeof(Programme_t));
	return_if_void(self == NULL);

	ref_thiz = (Programme_t *)ref.self;
	self->event = ref_thiz->event;

	goto_if(self->event == NULL, FAILED);
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	goto_if(OPL_Channel_FindChannelByUID(NULL, self->event->svcuid) == NULL, FAILED);
#else
	goto_if(OPL_Channel_FindChannelByTriplet(NULL, self->event->onid, self->event->tsid, self->event->svcid) == NULL, FAILED);
#endif

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
FAILED:
	return;
}

bool oplprogramme::operator ==(oplprogramme& other)
{
	Programme_t	*lhs = (Programme_t *)self;
	Programme_t	*rhs = (Programme_t *)other.self;

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

const HINT32 oplprogramme::getEventId()
{
	Programme_t *thiz = (Programme_t*)self;
	HxLOG_Trace();
	return_if(!self, 0);

	if (thiz->event )
	{
		return thiz->event->eventId;
	}
	return -1;
}


const char* oplprogramme::getName()
{
	Programme_t *thiz = (Programme_t*)self;
	char	*name;

	HxLOG_Trace();
	return_if(!self, NULL);

	name = (char *)HLIB_TREE_Lookup(thiz->cache.string, "name");
		if (name == NULL && thiz->event && thiz->event->name != NULL)
		{
			DxEventString_t * eventString = NULL;
			HxLANG_Id_e language = programme_GetMainLanguage();
			eventString = static_cast<DxEventString_t *>(HLIB_VECTOR_Find (thiz->event->name, OPL_Programme_FinedEventString, (void *)&language));

			if(eventString == NULL){
				eventString = static_cast<DxEventString_t *> (HLIB_VECTOR_First(thiz->event->name));

			}
			if(eventString && eventString->string)
			{
				name = eventString->string;
			}
			else
				name = NULL;
#ifdef	CONFIG_DEBUG
		DxEPG_Type_e eEpgType = (DxEPG_Type_e)getEpgType();
		HxLOG_Debug( HxANSI_COLOR_PURPLE("[%s] [epgType:%s, 0x%x]  name:[%s] id:[%d]  svcId:[%d] svcUid:[%d]!!\n") , __FUNCTION__,
			eEpgType == eDxEPG_TYPE_IP 		? (const char*)"eDxEPG_TYPE_IP" 		:
			eEpgType == eDxEPG_TYPE_PF 		? (const char*)"eDxEPG_TYPE_PF" 		:
			eEpgType == eDxEPG_TYPE_SC 		? (const char*)"eDxEPG_TYPE_SC" 		:
			eEpgType == eDxEPG_TYPE_PF_EX	? (const char*)"eDxEPG_TYPE_PF_EX" 	:
			eEpgType == eDxEPG_TYPE_SC_EX	? (const char*)"eDxEPG_TYPE_SC_EX" 	:
			eEpgType == eDxEPG_TYPE_TVTV	? (const char*)"eDxEPG_TYPE_TVTV" 	: (const char*)"OTHER"
				,
			eEpgType, name, thiz->event->eventId , thiz->event->svcid, thiz->event->svcuid);
#endif
	}
	return name;
}


const char*	oplprogramme::getDescription()
{
	Programme_t *thiz = (Programme_t*)self;
	char	*text;

	HxLOG_Trace();
	return_if(!self, NULL);

	text = (char *)HLIB_TREE_Lookup(thiz->cache.string, "description");
	if (text == NULL && thiz->event && thiz->event->text != NULL)
	{
		DxEventString_t * eventString = NULL;
		HxLANG_Id_e language = programme_GetMainLanguage();

		eventString = static_cast<DxEventString_t *> (HLIB_VECTOR_Find (thiz->event->text, OPL_Programme_FinedEventString, (void *)language));
		if(eventString == NULL)
			eventString = static_cast<DxEventString_t *> (HLIB_VECTOR_First(thiz->event->text));
		if(eventString && eventString->string)
		{
			text = eventString->string;
		}
		else
			text = NULL;
	}

	return text;
}

unsigned long oplprogramme::getStartTime()
{
	Programme_t *thiz = (Programme_t*)self;
	unsigned long	value = 0;

	HxLOG_Trace();
	return_if(!self, 0);

	if (!HLIB_TREE_LookupExtended(thiz->cache.integer, "startTime", NULL, (void **)&value))
	{
		if (thiz->event)
			value = (unsigned long)thiz->event->startTime;
	}
	return value;
}

unsigned long oplprogramme::getDuration()
{
	Programme_t *thiz = (Programme_t*)self;
	unsigned long	value = 0;

	HxLOG_Trace();
	return_if(!self, 0);

	if (!HLIB_TREE_LookupExtended(thiz->cache.integer, "duration", NULL, (void **)&value))
	{
		if (thiz->event)
		{
			value = (unsigned long)thiz->event->duration;
		}
	}
	return value;
}

const char*	oplprogramme::getChannelID()
{
	Programme_t *thiz = (Programme_t*)self;
	char	*text;

	HxLOG_Trace();
	return_if(!self, NULL);

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
					HLIB_TREE_Insert(thiz->cache.string, (void *)"channelID", (void *)text);
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

const int oplprogramme::getParentalRating()
{
	Programme_t *thiz = (Programme_t*)self;
	int			    rating  = 0;

	HxLOG_Trace();
	return_if(!self, 0);
	return_if(thiz->event == NULL, 0);

	rating = thiz->event->parentalRating;

	return rating;
}


void	oplprogramme::setName(const char* name)
{
	Programme_t *thiz = (Programme_t*)self;
	HxLOG_Trace();
	return_if_void(!self || !name);

	if (HLIB_TREE_Lookup(thiz->cache.string, "name"))
		HLIB_TREE_Replace(thiz->cache.string, (void *)"name", HLIB_STD_StrDup(name));
	else
		HLIB_TREE_Insert(thiz->cache.string, (void *)"name", HLIB_STD_StrDup(name));
}

void	oplprogramme::setDescription(const char* description)
{
	Programme_t *thiz = (Programme_t*)self;
	HxLOG_Trace();
	return_if_void(!self || !description);

	if (HLIB_TREE_Lookup(thiz->cache.string, "description"))
		HLIB_TREE_Replace(thiz->cache.string, (void *)"description", HLIB_STD_StrDup(description));
	else
		HLIB_TREE_Insert(thiz->cache.string, (void *)"description", HLIB_STD_StrDup(description));
}

void	oplprogramme::setLongDescription(const char* longDescription)
{
	Programme_t *thiz = (Programme_t*)self;
	HxLOG_Trace();
	return_if_void(!self || !longDescription);

	if (HLIB_TREE_Lookup(thiz->cache.string, "longDescription"))
		HLIB_TREE_Replace(thiz->cache.string, (void *)"longDescription", HLIB_STD_StrDup(longDescription));
	else
		HLIB_TREE_Insert(thiz->cache.string, (void *)"longDescription", HLIB_STD_StrDup(longDescription));
}

void	oplprogramme::setStartTime(unsigned long startTime)
{
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "startTime", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, (void *)"startTime", (void *)startTime);
	else
		HLIB_TREE_Insert(thiz->cache.integer, (void *)"startTime", (void *)startTime);

}

void	oplprogramme::setDuration(unsigned long duration)
{
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "duration", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, (void *)"duration", (void *)duration);
	else
		HLIB_TREE_Insert(thiz->cache.integer, (void *)"duration", (void *)duration);
}

void	oplprogramme::setChannelID(const char* channelID)
{
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();
	return_if_void(!self || !channelID);

	if (HLIB_TREE_Lookup(thiz->cache.string, "channelID"))
		HLIB_TREE_Replace(thiz->cache.string, (void *)"channelID", HLIB_STD_StrDup(channelID));
	else
		HLIB_TREE_Insert(thiz->cache.string, (void *)"channelID", HLIB_STD_StrDup(channelID));
}

void	oplprogramme::setEpisode(int episode)
{
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "episode", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, (void *)"episode", (void *)episode);
	else
		HLIB_TREE_Insert(thiz->cache.integer, (void *)"episode", (void *)episode);
}

void	oplprogramme::setTotalEpisodes(int totalEpisodes)
{
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "totalEpisodes", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, (void *)"totalEpisodes", (void *)totalEpisodes);
	else
		HLIB_TREE_Insert(thiz->cache.integer, (void *)"totalEpisodes", (void *)totalEpisodes);
}

void	oplprogramme::setProgrammeID(const char* programmeID)
{
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();
	return_if_void(!self || !programmeID);

	if (HLIB_TREE_Lookup(thiz->cache.string, "programmeID"))
		HLIB_TREE_Replace(thiz->cache.string, (void *)"programmeID", HLIB_STD_StrDup(programmeID));
	else
		HLIB_TREE_Insert(thiz->cache.string, (void *)"programmeID", HLIB_STD_StrDup(programmeID));
}

void	oplprogramme::setProgrammeIDType(int programmeIDType)
{
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();
	return_if_void(!self);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "programmeIDType", NULL, NULL))
		HLIB_TREE_Replace(thiz->cache.integer, (void *)"programmeIDType", (void *)programmeIDType);
	else
		HLIB_TREE_Insert(thiz->cache.integer, (void *)"programmeIDType", (void *)programmeIDType);
}

const char*	oplprogramme::getLongDescription()
{
	Programme_t *thiz = (Programme_t*)self;
	char	*text;

	HxLOG_Trace();
	return_if(!self, NULL);

	text = (char *)HLIB_TREE_Lookup(thiz->cache.string, "longDescription");
	if (text)
	{
		return text;
	}
	return_if(thiz->event == NULL, NULL);

	if (text == NULL && thiz->event && thiz->event->longText != NULL)
	{
		DxEventString_t * eventString = NULL;
		HxLANG_Id_e language = programme_GetMainLanguage();

		eventString = static_cast<DxEventString_t *> (HLIB_VECTOR_Find (thiz->event->longText, OPL_Programme_FinedEventString, (void *)language));
		if(eventString == NULL)
			eventString = static_cast<DxEventString_t *> (HLIB_VECTOR_First(thiz->event->longText));
		if(eventString && eventString->string)
		{
			text = eventString->string;
		}
		else
			text = NULL;
	}
	return text;
}

const char*	oplprogramme::getProgrammeID()
{
	Programme_t *thiz = (Programme_t*)self;
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
	makeProgrammeID(buf, 128, (int)thiz->event->svcuid, (int)thiz->event->eventId);
#endif
	text = HLIB_STD_StrDup(buf);
	if (text)
	{
		HLIB_TREE_Insert(thiz->cache.string, (void *)"programmeID", text);
	}
	return text;
}

int	oplprogramme::getEpisode()
{
	Programme_t *thiz = (Programme_t*)self;
	int			value = 0;
	HINT32	i, n, c;
	HULONG 		ulEpgType;
	HxLOG_Trace();
	return_if(!self, 0);

	ulEpgType = getEpgType();
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

	if (thiz->event->item == NULL)
	{
		return 0;
	}
	n = HLIB_VECTOR_Length(thiz->event->item);
	for (i = 0, c = 0 ; i < n ; i++)
	{
		DxEventItem_t *item = (DxEventItem_t *)HLIB_VECTOR_ItemAt(thiz->event->item, i);
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

int	oplprogramme::getTotalEpisodes()
{
	Programme_t *thiz = (Programme_t*)self;
	int			value = 0;
	HULONG		ulEpgType;

	HxLOG_Trace();
	return_if(!self, 0);

	ulEpgType = getEpgType();
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

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, (void *)"totalEpisodes", NULL, (void **)&value))
	{
		return value;
	}

	// TODO: NOT AVAILABLE

	return 0;
}

int	oplprogramme::getProgrammeIDType()
{
	Programme_t *thiz = (Programme_t*)self;
	int			value = 0;
	const char	*id;

	HxLOG_Trace();
	return_if(!self, 1);

	if (HLIB_TREE_LookupExtended(thiz->cache.integer, "programmeIDType", NULL, (void **)&value))
	{
		return value;
	}

	id = getProgrammeID();
	if (id)
	{
		return getIDTypeFromProgrammeID(id);
	}
	return eOPL_ProgrammeID_DVB_EVENT;

}



int	oplprogramme::getShowType()
{
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();
	return_if(thiz == NULL, 0);

	// TODO: NOT IMPLEMENTED YET!!!

	return 0;
}


int	oplprogramme::getAudioType()
{
	Programme_t *thiz = (Programme_t*)self;
	int			type = 0;
	HINT32		i, n;
	HxLANG_Id_e	language;
	DxEventComponent_t	*comp;
	HxLOG_Trace();
	return_if(!self, 0);

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

	return type;
}

int	oplprogramme::getSubtitleType()
{
	Programme_t *thiz = (Programme_t*)self;
	HINT32 type = 0;
	HINT32		i, n;
	DxEventComponent_t	*comp;

	HxLOG_Trace();
	return_if(!self, OPL_FALSE);

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

	return OPL_FALSE;
}

OPL_BOOL oplprogramme::IsBlocked()
{
	Programme_t *thiz = (Programme_t*)self;
	HERROR		hErr = ERR_FAIL;
	OPL_Channel_t	channel = NULL;
	HBOOL		bSvcLock = FALSE, bRatingLock = FALSE, bRetLock = FALSE;
	DxRatingAge_e	nRating =(DxRatingAge_e)0, nMenuRating =(DxRatingAge_e)0;

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
	nRating = (DxRatingAge_e)getParentalRating();
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

OPL_BOOL oplprogramme::HasSubtitle()
{
	Programme_t *thiz = (Programme_t*)self;
	HINT32		i, n;
	DxEventComponent_t	*comp;

	HxLOG_Trace();
	return_if(!self, OPL_FALSE);

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

	return OPL_FALSE;
}


OPL_Collection_t* oplprogramme::getSubtitleTypes()
{
	Programme_t *thiz = (Programme_t*)self;
	OPL_Collection_t	*coll = NULL;
	HINT32		i, n;
	DxEventComponent_t	*comp;

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
	return coll;
}

OPL_BOOL oplprogramme::IsHD()
{
	Programme_t *thiz = (Programme_t*)self;
	HINT32		i, n;
	DxEventComponent_t	*comp;

	HxLOG_Trace();
	return_if(!self, OPL_FALSE);
	return_if(thiz->event == NULL, OPL_FALSE);
	return_if(thiz->event->component == NULL, OPL_FALSE);

	n = HLIB_VECTOR_Length(thiz->event->component);
	for (i = 0 ; i < n ; i++)
	{
		comp = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(thiz->event->component, i);
		if (comp == NULL)
			continue;

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
	}

	return OPL_FALSE;
}

OPL_BOOL oplprogramme::IsMultilingual()
{
	Programme_t *thiz = (Programme_t*)self;
	HINT32		i, n;
	HxLANG_Id_e	language;
	DxEventComponent_t	*comp;

	HxLOG_Trace();
	return_if(!self, OPL_FALSE);
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
	return OPL_FALSE;

}

OPL_BOOL oplprogramme::IsLocked()
{
	Programme_t *thiz = (Programme_t*)self;
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
	nRating = getParentalRating();
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

OPL_BOOL oplprogramme::HasRecording()
{
	if (getRecording())
		return OPL_TRUE;
	return OPL_FALSE;
}


OPL_Collection_t* oplprogramme::getGenre()
{
	Programme_t *thiz = (Programme_t*)self;
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
	HLIB_TREE_Insert(thiz->cache.collection, (void *)"genre", coll);
	return coll;
}

OPL_Collection_t* oplprogramme::getAudioLanguages()
{
	Programme_t *thiz = (Programme_t*)self;
	HINT32			i, n;
	OPL_Collection_t		*coll;
	const HCHAR			*text;
	DxEventComponent_t	*comp;

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->component == NULL, NULL);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "audioLanguages");
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

		if (programme_GetAudioType(comp, eLangID_NONE) == 0)
			continue;

		text = HLIB_LANG_IdTo639(comp->language);
		if (text == NULL)
			continue;
		(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(text));
	}

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, (void*)"audioLanguages", coll);
	return coll;
}

OPL_Collection_t* oplprogramme::getAudioComponent()
{
	Programme_t *thiz = (Programme_t*)self;
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


OPL_Collection_t* oplprogramme::getSubtitleLanguages()
{
	Programme_t *thiz = (Programme_t*)self;
	HINT32			i, n;
	OPL_Collection_t	*coll;
	const HCHAR			*text;
	DxEventComponent_t	*comp;

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->component == NULL, NULL);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "subtitleLanguages");
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

		if (programme_GetSubtitleType(comp, eLangID_NONE) == 0)
			continue;

		text = HLIB_LANG_IdTo639(comp->language);
		if (text == NULL)
			continue;
		(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(text));
	}

	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, (void*)"subtitleLanguages", coll);
	return coll;
}

OPL_Channel_t oplprogramme::getChannel()
{
	Programme_t *thiz = (Programme_t*)self;

	HxLOG_Trace();
	return_if(!self, 0);
	return_if(thiz->event == NULL, 0);

#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	return OPL_Channel_FindChannelByUID(NULL, (int)thiz->event->svcuid);
#else
	return OPL_Channel_FindChannelByTriplet(NULL, thiz->event->onid, thiz->event->tsid, thiz->event->svcid);
#endif
}


OPL_HANDLE oplprogramme::getScheduledRecording()
{
	Programme_t *thiz = (Programme_t*)self;
	const char	*id;

	HxLOG_Trace();
	return_if(thiz == NULL, NULL);

	id = getProgrammeID();
	return_if(id == NULL, NULL);

	return OPL_Scheduler_FindScheduleExceptRecording(OPL_Scheduler_GetInstance(), id);
}


OPL_HANDLE oplprogramme::getRecording()
{
	Programme_t *thiz = (Programme_t*)self;
	const char	*id;

	HxLOG_Trace();
	return_if(thiz == NULL, NULL);

	id = getProgrammeID();
	return_if(id == NULL, NULL);

	return OPL_Scheduler_FindSchedule(OPL_Scheduler_GetInstance(), id);
}


OPL_Collection_t* oplprogramme::getRecordings()
{
	Programme_t	*thiz = (Programme_t *)self;
	const char	*id;

	HxLOG_Trace();
	return_if(thiz == NULL, NULL);

	id = getProgrammeID();
	return_if(id == NULL, NULL);

	return OPL_Scheduler_FindSchedules(OPL_Scheduler_GetInstance(), id);
}



OPL_Collection_t* oplprogramme::getGroupCRIDs()
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
		(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(contentIdentifier->crid));
	}
	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}
	HLIB_TREE_Insert(thiz->cache.collection, (void*)"groupCRIDs", coll);
	return coll;
}


HBOOL oplprogramme::programme_getMediaDeeplinkData(HxVector_t *pMediaVector, HUINT32 * pulExpireDate, HCHAR **ppucUrl, HCHAR **ppucID)
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

const char* oplprogramme::getMediaDeeplinkID()
{
	Programme_t			* thiz = (Programme_t *)self;
	DxEPG_Type_e		eEpgType = eDxEPG_TYPE_NONE;
	HBOOL				bFound = FALSE;
	char 				* pucID = NULL;

	HxLOG_Trace();
	return_if(NULL == self, NULL);
	return_if(NULL == thiz->event, NULL);

	eEpgType = (DxEPG_Type_e)getEpgType();

	return_if(eDxEPG_TYPE_IP != eEpgType, NULL);
	return_if(thiz->event->extension.ipEpg.mediaList == NULL, NULL);

	pucID = (char *)HLIB_TREE_Lookup(thiz->cache.string, "deeplinkMediaID");
	if(pucID)
	{
		return pucID;
	}

	// ---------------------------------------------
	bFound = programme_getMediaDeeplinkData(thiz->event->extension.ipEpg.mediaList, NULL, NULL, &pucID);
	// ---------------------------------------------
	if(TRUE == bFound && NULL != pucID)
	{
		HLIB_TREE_Insert(thiz->cache.string, (void*)"deeplinkMediaID", (void *)pucID);
	}
	return pucID;
}

const char* oplprogramme::getMediaDeeplinkUrl()
{
	Programme_t			* thiz = (Programme_t *)self;
	DxEPG_Type_e		eEpgType = eDxEPG_TYPE_NONE;
	HBOOL				bFound = FALSE;
	char 				* pucUrl = NULL;

	HxLOG_Trace();
	return_if(NULL == self, NULL);
	return_if(NULL == thiz->event, NULL);

	eEpgType = (DxEPG_Type_e)getEpgType();

	return_if(eDxEPG_TYPE_IP != eEpgType, NULL);
	return_if(thiz->event->extension.ipEpg.mediaList == NULL, NULL);

	pucUrl = (char *)HLIB_TREE_Lookup(thiz->cache.string, "deeplinkMediaUrl");
	if(pucUrl)
	{
		return pucUrl;
	}

	// ---------------------------------------------
	bFound = programme_getMediaDeeplinkData(thiz->event->extension.ipEpg.mediaList, NULL, &pucUrl, NULL);
	// ---------------------------------------------
	if(TRUE == bFound && NULL != pucUrl)
	{
		HLIB_TREE_Insert(thiz->cache.string, (void*)"deeplinkMediaUrl", (void *)pucUrl);
	}
	return pucUrl;
}


unsigned long oplprogramme::getMediaDeeplinkExpireDate()
{
	Programme_t			* 	thiz 		= (Programme_t *)self;
	HUINT32					ulExpireDate= 0;
	DxEPG_Type_e		  	eEpgType 	= eDxEPG_TYPE_NONE;
	HBOOL 					bFound 		= FALSE;

	HxLOG_Trace();
	return_if(NULL == self, 0);
	return_if(NULL == thiz->event, 0);

	eEpgType = (DxEPG_Type_e)getEpgType();

	return_if(eDxEPG_TYPE_IP != eEpgType, 0);
	return_if(thiz->event->extension.ipEpg.mediaList == NULL, 0);


	if (!HLIB_TREE_LookupExtended(thiz->cache.integer, "deeplinkMediaExpireDate", NULL, (void **)&ulExpireDate))
	{
		// ---------------------------------------------
		bFound = programme_getMediaDeeplinkData(thiz->event->extension.ipEpg.mediaList, &ulExpireDate, NULL, NULL);
		// ---------------------------------------------
		if(TRUE == bFound)
		{
			HLIB_TREE_Insert(thiz->cache.integer, (void*)"deeplinkMediaExpireDate", (void *)ulExpireDate);
		}
	}
	return ulExpireDate;
}


OPL_Collection_t* oplprogramme::getGroupCRIDsAndType()
{
	Programme_t			*thiz = (Programme_t *)self;
	OPL_Collection_t		*coll;
	HINT32				i, n;
	DxEventCRID_t	*contentIdentifier;

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->crid == NULL, NULL);

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
	HLIB_TREE_Insert(thiz->cache.collection, (void*)"groupCRIDsAndType", coll);
	return coll;
}


const char* oplprogramme::getIMI()
{
	Programme_t	*thiz = (Programme_t *)self;

	HxLOG_Trace();

	(void)thiz;
	// TODO: NOT IMPLEMENTED YET

	return NULL;
}

const char*	 oplprogramme::getField(const char *field_name)
{
	Programme_t	*thiz = (Programme_t *)self;
	char		*text = NULL;

	HxLOG_Trace();
	return_if(!self || !field_name, NULL);
	return_if(thiz->event == NULL, NULL);

	text = (char *)HLIB_TREE_Lookup(thiz->cache.string, field_name);
	if (text)
		return text;

	if( map_pFunction[field_name])
	{
		text = (this->*map_pFunction[field_name])();

		if (text)
			HLIB_TREE_Insert(thiz->cache.string, (void *)field_name, text);
		return text;
	}

	//if (HLIB_STD_StrCaseStartWith(field, "crid://"))	// 음... 체크했음 좋겠는데...
	{
		DxEventCRID_t *crid = programme_GetGroupCRID(thiz, field_name);
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

	HxLOG_Warning("%s(field:%s) is NOT implemented yet...!!\n", __FUNCTION__, field_name);
	return NULL;
}

OPL_Collection_t* oplprogramme::getSiDescription(int tag, int tagExtention)
{
#if !defined(CONFIG_OP_JAPAN)
	return NULL;
#else
	Programme_t 			*thiz = (Programme_t *)self;
	HCHAR				*tagString= NULL;
	OPL_Collection_t		*coll;

	tagExtention = tagExtention;
	switch( tag )
	{
		default:
			return NULL;
	}

	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->descriptor == NULL, NULL);

	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, tagString);
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

	HLIB_TREE_Insert(thiz->cache.collection, tagString, coll);

	return coll;
#endif
}

int oplprogramme::getEventIDFromProgrammeID (const char *programmeID)
{
	int	eventID = 0;

	// dvb://<original_network_id>.[<transport_stream>].<service_id>;<event_id>[;;<service_unique_id>]
	if (programme_ParseDVBURI(programmeID, NULL, NULL, NULL, &eventID, NULL, NULL))
		return eventID;

	HxLOG_Error("[%s] Invalid ProgrammeID: %s\n", programmeID);
	return 0;
}

int oplprogramme::getSvcUIDFromProgrammeID (const char *programmeID)
{
	int	id = 0;

	// dvb://<original_network_id>.[<transport_stream>].<service_id>;<event_id>[;;<service_unique_id>]
	if (programme_ParseDVBURI(programmeID, NULL, NULL, NULL, NULL, NULL, &id))
		return id;

	HxLOG_Error("[%s] Invalid ProgrammeID: %s\n", programmeID);
	return 0;
}

void oplprogramme::makeProgrammeID (char *buf, unsigned int n, int onid, int tsid, int svcid, int eventId)
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

void oplprogramme::makeProgrammeID (char *buf, unsigned int n, int svcuid, int eventId)
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


int oplprogramme::getIDTypeFromProgrammeID (const char *programmeID)
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

	HxLOG_Critical("%s(%s) Unknown programmeIDType!!!\n", __FUNCTION__, programmeID);
	return eOPL_ProgrammeID_DVB_EVENT;
}


OPL_BOOL  oplprogramme::getUniqueIDsFromProgrammeID (const char *programmeID, int *svcuid, int *eventid)
{
	if (programme_ParseDVBURI(programmeID, NULL, NULL, NULL, eventid, NULL, svcuid))
		return OPL_TRUE;
	return OPL_FALSE;
}


OPL_Collection_t* oplprogramme::getComponents (eOplAVComponentType eType )
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
		coll = programme_GetVideoComponent();
		break;

	case OPLAVCOMPONENT_TYPE_AUDIO:
		coll = programme_GetAudioComponent();
		break;

	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		coll = programme_GetDataComponent(eType);
		break;

	case OPLAVCOMPONENT_TYPE_DATA:
		coll = programme_GetDataComponent(eType);
		break;

	default:
		HxLOG_Error("[%s] Invalid Component Type : %d \n", eType);
		return NULL;
	}

	HxLOG_Trace();
	return coll;
}

HERROR oplprogramme::getMultiViewComponent( HxVector_t *descList )
{
	Programme_t *thiz = (Programme_t*)self;
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

	/* 반드시 받는 쪽에서 descList 해제 해 주어야 한다. !!!! */
	return ERR_OK;
}

int  oplprogramme::getCopyControl( eCopyControlType eType )
{
	Programme_t *thiz = (Programme_t*)self;
	HINT32			eRetVal = 0;

	return_if(!thiz, 0);
	return_if(thiz->event == NULL, 0);

	return eRetVal;
}

OPL_BOOL oplprogramme::getIs3D()
{
	OPL_BOOL			bRetVal = OPL_FALSE;

	return bRetVal;
}

OPL_BOOL oplprogramme::getIsCharged()
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

const char* oplprogramme::getContractVerificationInfo()
{
	Programme_t					*thiz = (Programme_t *)self;
	HINT32			  			sd;
	HCHAR						*info = NULL;

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

		info = (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
		if( info != NULL )
		{
			HLIB_TREE_Insert(thiz->cache.string, (void*)"contractverificationinfo", (void *)info);
		}
	}

	HxLOG_Print( "[%s,%d] data : %s \n" , __FUNCTION__ ,__LINE__ ,info);

	return info;
}

int oplprogramme::getRecordingStatus()
{
	Programme_t *thiz = (Programme_t *)self;
	const char	*id;
	OPL_ScheduledItem_t *item = NULL;
	int			retVal = 0;

	HxLOG_Trace();
	return_if(thiz == NULL, retVal);

	id = getProgrammeID();
	return_if(id == NULL, retVal);

	item = OPL_Scheduler_FindSchedule(OPL_Scheduler_GetInstance(), id);
	if( item )
	{
		switch( item->type )
		{
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

unsigned long oplprogramme::getEpgType()
{
	Programme_t      *thiz = (Programme_t *)self;

	HxLOG_Trace();
	return_if(!self, 0);
	return_if(thiz->event == NULL, 0);

	return thiz->event->type;
}

OPL_Collection_t* oplprogramme::getSharedProgrammeIds()
{
	Programme_t 			*thiz = (Programme_t *)self;
	OPL_Collection_t		*coll;

	HxLOG_Trace();

	return_if(!self, NULL);
	return_if(thiz->event == NULL, NULL);
	return_if(thiz->event->descriptor == NULL, NULL);

	/* svc type 정보 */
	coll = (OPL_Collection_t *)HLIB_TREE_Lookup(thiz->cache.collection, "sharedprogrammeids");
	if (coll)
		return coll;

	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);

	HLIB_TREE_Insert(thiz->cache.collection, (void*)"sharedprogrammeids", coll);

	return coll;
}

#define TEMP_STRING_SIZE 100
OPL_ProgrammeHandle oplprogramme::getSharedProgrammeInfo(const char* pszSharedProgrammeId)
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
	sharedProgramme = new oplprogramme();//OPL_Programme_New("o", APK_EVENT_Incref(sharedEvent));
	if( sharedProgramme == NULL )
	{
		APK_EVENT_Delete( sharedEvent );
		return NULL;
	}

	return sharedProgramme;
}

HCHAR* oplprogramme::getGenreString (HUINT16 content)
{
	HUINT8 			contentNibble =0, userNibble =0;
	HUINT8 			ucMajorNibble =0, ucMinorNibble =0;
	HINT32			sd = 0;

	contentNibble = (HUINT8)(content >> 8);
	userNibble = (HUINT8)(content | 0xff ); // Not used

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
