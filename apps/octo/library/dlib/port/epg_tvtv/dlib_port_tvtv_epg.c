#include <unistd.h>
#include <hlib.h>
#include "_dlib_port_tvtv_epg.h"

#define 	USE_CRC_CHECK
#ifdef CONFIG_MW_EPG_TVTV_NAND_STORAGE
#define	TVTVEPG_REPOSITORY_PATH		"/var/lib/humaxtv_user/TVTV/DATA"
#else
#define	TVTVEPG_REPOSITORY_PATH		"/mnt/hd1/TVTV/DATA"
#define	TVTVEPG_MOUNT_PATH			"/mnt/hd1"
#endif

#define	MAX_TVTVEPG_SERVICE_NUM		16
/* 현재 TVTV EPG 구성
    MAX 5000개 채널까지만 수신 - 그런데 size 문제로인해 현재 130개의 채널만 수신되도록 fix되어 있다.
    14일을 송출하고 있지만 size 문제로인해 7일만 수신하고 있다.
    1day , 1 channel event number : max 30분당 1개 : 24*2 = 48 개
    total event info : 5000 * 14 * 48 = 3360000
*/
#define MAX_EVENT_NUMBER	3360000

// File structure
//
//	{	4 byte		|	4 byte					|	(event count * dlib_epgevt_t) byte	|
//	{	event count	|	CRC of dlib_epgevt_tdata	|	dlib_epgevt_t data					|	raw data
//

#define TVTVEPG_MAKE_KEY(key)	((HUINT32)(key&0x00000000FFFFFFFF))

typedef	struct
{
	HUID	uid;
	HINT32	size;
	HUINT8	data[];
} dlib_epgevt_t;

typedef	struct
{
	HCHAR		*pszUid;

	HBOOL		bIsChanged;
	HUINT32		ulLastUsed;
	HCHAR		*pszRepositoryName;
	HxHASH_t	*pstEventTable;
} dlib_epgsvc_t;

static	HxHASH_t	*s_pstServiceEventTable = NULL;

typedef struct
{
	DxField_t		field;
	HBOOL		(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxEPG_Field_t;


#define	RETURN_IF_VOID(expr)		do{if(expr)return;}while(0)
#define	RETURN_IF(expr, err)		do{if(expr)return (err);}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

#define	D_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return;}}while(0)
#define	D_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define	D_GOTO_IF(expr, pos)		do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

#define	W_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return;}}while(0)
#define	W_RETURN_IF(expr, err)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return (err);}}while(0)
#define	W_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);goto pos;}}while(0)

#define	E_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return;}}while(0)
#define	E_RETURN_IF(expr, err)		do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return (err);}}while(0)
#define	E_GOTO_IF(expr, pos)		do{if(expr){HxLOG_Error("%s failed!!\n", #expr);goto pos;}}while(0)
#define	TVTVEPG_Get16Bit(a) (HUINT16)((((*((HUINT8 *)(a+1)) << 8) | (*((HUINT8*)(a))))))

static HxHASH_t	*	dlib_epg_tvtv_MakeSvcHashTable(void)
{
	s_pstServiceEventTable = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)DLIB_Free_CB, (_HxDestroyNotify)DLIB_Free_CB);
	return s_pstServiceEventTable;
}

static void	dlib_epg_tvtv_Print_Event(DxEvent_t *event);
#if 0
void 	dlib_epg_tvtv_dumpdata(HUINT8 *buf, HUINT32 size)
{
	HUINT32 i = 0;
	HUINT32 index = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usSize = 0;

	HxLOG_Print ("\ndlib_epg_tvtv_dumpdata src:%x size[%d]\n", buf, size);
	// DxTvtvEventBase_t
	HxLOG_Print ("DxTvtvEventBase_t\n");
	for(i = 0; i < sizeof(DxTvtvEventBase_t); i++)
	{
		if(i%16 == 0)
		{
			HxLOG_Print ("\n");
		}
		HxLOG_Print ("%02X ", buf[i]);
	}

	// DxTvtvString
	HxLOG_Print ("\n\nDxTvtvString\n");
	tempBuf = tempBuf + sizeof(DxTvtvEventBase_t);

	for(;i<size; i+=2)
	{
		usSize = TVTVEPG_Get16Bit(&buf[i]);
		HxLOG_Print ("\n\tINDEX[%d] Size [%d][0x%x]", index++, usSize, usSize);

		if(usSize != 0)
		{
			HUINT32 k = 0;
			for(k = 0;k<usSize + 2; k++)
			{
				if(k%16 == 0)
				{
					HxLOG_Print ("\n\t\t");
				}
				HxLOG_Print ("%02X ", buf[i+k]);
			}
		}
		i += usSize;
	}
	HxLOG_Print ("\n");
	HxLOG_Print ("\n");
}
#endif
static HUINT8 * dlib_epg_tvtv_Get_data(HUINT8 *buf, DxTVTVEPG_Text_e eIndex, HUINT32 size)
{
	HUINT32 i = 0;
	HUINT8 *tempBuf = buf;
	HUINT16	usSize = 0;

	tempBuf = tempBuf + sizeof(DxTvtvEventBase_t);

	for(i = 0; i< eDxTVTVEPG_TEXT_MAX; i++)
	{
		usSize = TVTVEPG_Get16Bit(tempBuf);
		if(usSize > size)	HxLOG_Error("dlib_epg_tvtv_Get_data FAIL[%d] [%x]\n", size, usSize);
		tempBuf = tempBuf+sizeof(HUINT16);
		if(eIndex == i)
		{
			if(usSize == 0)
				return NULL;
			else
				return tempBuf;
		}
		tempBuf += usSize;
	}
	return NULL;
}

static HBOOL	dlib_epg_tvtv_GetTvtvID (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.usTvtvId;
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetName (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	//dlib_epg_tvtv_dumpdata((HUINT8 *)src, size);
	field->u.string = DLIB_StrDup(dlib_epg_tvtv_Get_data(pEvent, eDxTVTVEPG_TEXT_NAME, size));
	return TRUE;
}

static void dlib_epg_tvtv_FreeEventString(DxEventString_t *string)
{
	if (string)
	{
		if (string->string)
			DLIB_Free(string->string);
		DLIB_Free(string);
	}
}

static HxVector_t * dlib_epg_tvtv_GetStringVector (const void *src, HUINT32 size, DxTVTVEPG_Text_e eTextType)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	HCHAR *pString = NULL;
	HxVector_t * vString = NULL;

	RETURN_IF(src == NULL || size == 0, FALSE);
	//dlib_epg_tvtv_dumpdata((HUINT8 *)src, size);
	pString = DLIB_StrDup(dlib_epg_tvtv_Get_data(pEvent, eTextType, size));
	if(pString)
	{
		vString = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_tvtv_FreeEventString, NULL);
		if(vString == NULL)
		{
			HxLOG_Critical("[%s] Out of memory!\n", __FUNCTION__);
			DLIB_Free(pString);
			return NULL;
		}
		else
		{
			DxEventString_t * pstEventString = DLIB_Calloc(sizeof(DxEventString_t));
			pstEventString->string = pString;
			pstEventString->language = eLangID_NONE;		// 기존에도 language가 올라 오지 않음. default NONE으로 setting
			HLIB_VECTOR_Add(vString, pstEventString);
		}
	}
	return vString;
}

static HBOOL	dlib_epg_tvtv_GetStartTime (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulStartTime;
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetEndTime (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulEndTime;
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetSvcUID(DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = (HINT32)pEvent->stBase.ulSvcUid;
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetEventID (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulProgId;
	return TRUE;
}


static HBOOL	dlib_epg_tvtv_GetUniqueID (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulProgId;
	return TRUE;
}


static HBOOL	dlib_epg_tvtv_GetSeriesID (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulSeriesId;
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetTriplet (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)src;
	HCHAR	buf[32];

	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);

	HxSTD_PrintToStrN(buf, 32, "%x.%x.%x"
					, pEvent->stBase.usOnId
					, pEvent->stBase.usTsId
					, pEvent->stBase.usSvcId
				);
	field->u.string = DLIB_StrDup(buf);
	if (field->u.string == NULL)
		return FALSE;
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetActor (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_tvtv_Get_data(pEvent, eDxTVTVEPG_TEXT_ACTOR, size));
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetActress (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_tvtv_Get_data(pEvent, eDxTVTVEPG_TEXT_ACTRESS, size));
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetDirector (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_tvtv_Get_data(pEvent, eDxTVTVEPG_TEXT_DIRECTOR, size));
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetThumbnail (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_tvtv_Get_data(pEvent, eDxTVTVEPG_TEXT_THUMBNAIL, size));
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetPerson (DxField_t *field, const void *src, HUINT32 size)
{
	HUINT8 *pEvent = (HUINT8 *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string = DLIB_StrDup(dlib_epg_tvtv_Get_data(pEvent, eDxTVTVEPG_TEXT_PERSON, size));
	return TRUE;
}

static HBOOL	dlib_epg_tvtv_GetEpisode (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pEvent->stBase.ulEpisodeNum;
	return TRUE;
}

static const DxEPG_Field_t	s_EPG_TVTV_FIELDS[] =
{
	{{eDxTYPE_INTEGER	, "tvtvId"			, FALSE, {0}	}, dlib_epg_tvtv_GetTvtvID}
	, {{eDxTYPE_STRING	, "name"			, TRUE ,	{0}	}, dlib_epg_tvtv_GetName}
	, {{eDxTYPE_INTEGER	, "start_time"		, FALSE, {0}	}, dlib_epg_tvtv_GetStartTime}
	, {{eDxTYPE_INTEGER	, "end_time"		, FALSE, {0}	}, dlib_epg_tvtv_GetEndTime}
	, {{eDxTYPE_INTEGER	, "svcuid"			, TRUE , 	{0}	}, dlib_epg_tvtv_GetSvcUID}
	, {{eDxTYPE_INTEGER	, "eventid"		, TRUE,  {0}	}, dlib_epg_tvtv_GetEventID}
	, {{eDxTYPE_STRING   , "tripleid"  		, TRUE,  {0}	}, dlib_epg_tvtv_GetTriplet}
	, {{eDxTYPE_INTEGER	, "seriesid"		, TRUE, 	{0}	}, dlib_epg_tvtv_GetSeriesID}
	, {{eDxTYPE_STRING	, "person"		, TRUE, 	{0}	}, dlib_epg_tvtv_GetPerson}
	, {{eDxTYPE_INTEGER	, "uniqueid"		, FALSE, {0}	}, dlib_epg_tvtv_GetUniqueID}
	, {{eDxTYPE_INTEGER	, "episode"		, FALSE, {0}	}, dlib_epg_tvtv_GetEpisode}
};

DxEvent_t *	dlib_epg_tvtv_NewEvent (HUID uid, const HUINT8 *src, HUINT32 size)
{
	DxTvtvEvent_t	*tvtv		= NULL;
	DxEvent_t		*event 	= NULL;
	DxField_t field;

	HxLOG_Trace();
	tvtv = (DxTvtvEvent_t *)src;

	RETURN_IF(tvtv == NULL || size == 0 , NULL);
	RETURN_IF(tvtv->stBase.type != eDxEPG_TYPE_TVTV, NULL);
	event = (DxEvent_t *)DLIB_Malloc(sizeof(DxEvent_t));
	RETURN_IF(event == NULL, NULL);
	HxSTD_MemSet(event,0,sizeof(DxEvent_t));

	event->type			= tvtv->stBase.type;
	event->refCount 		= 1;
	event->uid          		= uid;
	event->svcuid 		= tvtv->stBase.ulSvcUid;
	event->tsid 			= tvtv->stBase.usTsId;
	event->onid 			= tvtv->stBase.usOnId;
	event->svcid 			= tvtv->stBase.usSvcId;
	event->eventId		= tvtv->stBase.ulProgId;
	event->startTime 		= tvtv->stBase.ulStartTime;
	event->duration 		= tvtv->stBase.ulEndTime - tvtv->stBase.ulStartTime;
	event->runStatus 		= 0;
	event->freeCaMode 	= TRUE;
	event->parentalRating 	= (HUINT32)tvtv->stBase.ucRating;

	HxSTD_MemSet(event->content,0,sizeof(HUINT16)*DxEVENT_CONTENT); //temp
	event->content[0] 	= (tvtv->stBase.aucContent[0]<<8) & 0xFF00;	// 일단 1개의 content로 운용.

	event->extension.tvtvEpg.tvtvId 		= tvtv->stBase.usTvtvId;
	event->extension.tvtvEpg.seriesId 		= tvtv->stBase.ulSeriesId;
	event->extension.tvtvEpg.episodeNum		= tvtv->stBase.ulEpisodeNum;
	event->extension.tvtvEpg.totalNum 		= tvtv->stBase.ulTotalNum;
	event->extension.tvtvEpg.season 		= tvtv->stBase.ulSeason;
//	event->extension.tvtvEpg.seriesType 		= tvtv->stBase.ucSeriesType;
	HxSTD_MemCopy(event->extension.tvtvEpg.eventLink, tvtv->stBase.astEventLink,sizeof(EVENT_LINK_TVTV_t)*EPG_TVTV_LINK_MAX);
	(void)dlib_epg_tvtv_GetActor (&field, src, size);
	event->extension.tvtvEpg.actor = field.u.string;
	(void)dlib_epg_tvtv_GetActress (&field, src, size);
	event->extension.tvtvEpg.actress = field.u.string;
	(void)dlib_epg_tvtv_GetDirector (&field, src, size);
	event->extension.tvtvEpg.director = field.u.string;
	(void)dlib_epg_tvtv_GetPerson (&field, src, size);
	event->extension.tvtvEpg.person = field.u.string;
	(void)dlib_epg_tvtv_GetThumbnail (&field, src, size);
	event->extension.tvtvEpg.thumbnailURL = field.u.string;

	event->name = dlib_epg_tvtv_GetStringVector (src, size, eDxTVTVEPG_TEXT_NAME);
	event->text = dlib_epg_tvtv_GetStringVector (src, size, eDxTVTVEPG_TEXT_SHORT);
	event->longText = dlib_epg_tvtv_GetStringVector (src, size, eDxTVTVEPG_TEXT_DETAIL);

	dlib_epg_tvtv_Print_Event(event);
	return event;
}
static DxEventString_t *dlib_epg_tvtv_CloneString (DxEventString_t *string)
{
	DxEventString_t	*clone;
	if (string == NULL)
		return NULL;

	clone = (DxEventString_t *)DLIB_MemDup(string, sizeof(DxEventString_t));
	if (clone)
	{
		if(string->string) clone->string = DLIB_StrDup(string->string);
	}
	return clone;
}



DxEvent_t *	dlib_epg_tvtv_CloneEvent (const DxEvent_t *event)
{
	DxEvent_t *clone = NULL;
	HxLOG_Trace();
	clone = (DxEvent_t *)DLIB_MemDup(event, sizeof(DxEvent_t));
	if (clone)
	{
		clone->refCount = 1;

		if (event->name)
			clone->name = HLIB_VECTOR_CloneEx((HxVector_t *)clone->name, (void *(*)(void *))dlib_epg_tvtv_CloneString);
		if (event->text)
			clone->text = HLIB_VECTOR_CloneEx((HxVector_t *)clone->text, (void *(*)(void *))dlib_epg_tvtv_CloneString);
		if (event->longText)
			clone->longText = HLIB_VECTOR_CloneEx((HxVector_t *)clone->longText, (void *(*)(void *))dlib_epg_tvtv_CloneString);

		if(event->extension.tvtvEpg.actor)
			clone->extension.tvtvEpg.actor = DLIB_StrDup(event->extension.tvtvEpg.actor);
		if(event->extension.tvtvEpg.actress)
			clone->extension.tvtvEpg.actress = DLIB_StrDup(event->extension.tvtvEpg.actress);
		if(event->extension.tvtvEpg.director)
			clone->extension.tvtvEpg.director = DLIB_StrDup(event->extension.tvtvEpg.director);

		if(event->extension.tvtvEpg.person)
			clone->extension.tvtvEpg.person = DLIB_StrDup(event->extension.tvtvEpg.person);
		if(event->extension.tvtvEpg.thumbnailURL)
			clone->extension.tvtvEpg.thumbnailURL = DLIB_StrDup(event->extension.tvtvEpg.thumbnailURL);
	}
	return clone;
}

void dlib_epg_tvtv_FreeEvent (DxEvent_t *event)
{
	HxLOG_Trace();

	RETURN_IF_VOID(event == NULL);
	RETURN_IF_VOID(event->type != eDxEPG_TYPE_TVTV);

	if (event->name)			HLIB_VECTOR_Delete(event->name);
	if (event->text)			HLIB_VECTOR_Delete(event->text);
	if (event->longText)		HLIB_VECTOR_Delete(event->longText);

	if (event->item)			HLIB_VECTOR_Delete(event->item);
	if (event->component)		HLIB_VECTOR_Delete(event->component);
	if (event->descriptor)		HLIB_VECTOR_Delete(event->descriptor);
	if (event->linkage)			HLIB_VECTOR_Delete(event->linkage);
	if (event->crid)			HLIB_VECTOR_Delete(event->crid);
	if(event->extension.tvtvEpg.actor)	DLIB_Free(event->extension.tvtvEpg.actor);
	if(event->extension.tvtvEpg.actress)	DLIB_Free(event->extension.tvtvEpg.actress);
	if(event->extension.tvtvEpg.director)	DLIB_Free(event->extension.tvtvEpg.director);
	if(event->extension.tvtvEpg.person)	DLIB_Free(event->extension.tvtvEpg.person);
	if(event->extension.tvtvEpg.thumbnailURL) DLIB_Free(event->extension.tvtvEpg.thumbnailURL);
	DLIB_Free(event);
}

static HUINT32	dlib_epg_tvtv_hash(const void *key)
{
	return (HUINT32)key;
}

static HBOOL	dlib_epg_tvtv_hashvalue_equal(const void *a, const void *b)
{
	if (a == b)
		return TRUE;

	return FALSE;
}

void			DLIB_EPG_TVTV_Init(void)
{
	HxLOG_Debug("TVTV EPG Init..\n");
#if 0
#if 0//defined(CONFIG_MW_EPG_TVTV_DLIB_TEMP)
	// 여기서 만들지 않음.
#else
	if (access(TVTVEPG_REPOSITORY_PATH, R_OK | W_OK) != 0)
		HLIB_DIR_Make(TVTVEPG_REPOSITORY_PATH);
	if (access(TVTVEPG_THUMBNAIL_PATH, R_OK | W_OK) != 0)
		HLIB_DIR_Make(TVTVEPG_THUMBNAIL_PATH);
#endif
#endif
	dlib_epg_tvtv_MakeSvcHashTable();
}

#define DLIB_TVTVEPG_DBFILE CONFIG_PRODUCT_USERDATA_DIR "/tvtvepg/eDxEPG_TYPE_TVTV.db"

const HCHAR *	DLIB_EPG_TVTV_GetDBFullName (HINT32 dupIndex)
{
	if(dupIndex == 0)
		return DLIB_TVTVEPG_DBFILE;
	else
		return NULL;
}


HUINT32			DLIB_EPG_TVTV_GetSyncTime (void)
{
	return 10;
}


HINT32			DLIB_EPG_TVTV_GetFieldCount (void)
{
	return sizeof(s_EPG_TVTV_FIELDS) / sizeof(s_EPG_TVTV_FIELDS[0]);
}

DxField_t *		DLIB_EPG_TVTV_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_EPG_TVTV_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_EPG_TVTV_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_EPG_TVTV_FIELDS[index].getData)
	{
		if (!s_EPG_TVTV_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_EPG_TVTV_FIELDS[index].field.name);
			return NULL;
		}
	}
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}

HERROR			DLIB_EPG_TVTV_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	return ERR_FAIL;
}

HERROR			DLIB_EPG_TVTV_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	return ERR_FAIL;
}

HERROR			DLIB_EPG_TVTV_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	return ERR_FAIL;
}

HERROR			DLIB_EPG_TVTV_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	return ERR_FAIL;
}

HERROR			DLIB_EPG_TVTV_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxEvent_t	*event = NULL;

	if (srcSize == sizeof(DxEvent_t) && uid == DxEPG_CLONE_UID)
		event = dlib_epg_tvtv_CloneEvent((const DxEvent_t *)src);
	else
		event = dlib_epg_tvtv_NewEvent(uid, (const HUINT8 *) src, srcSize);

	if (event == NULL)
		return ERR_FAIL;

	*type = event;
	return ERR_OK;
}


void			DLIB_EPG_TVTV_FreeType (void *type)
{
	RETURN_IF_VOID(type == NULL);
	dlib_epg_tvtv_FreeEvent(type);
}

const HCHAR*	DLIB_EPG_TVTV_GetRepositoryPath(void)
{
	return (const HCHAR*)TVTVEPG_REPOSITORY_PATH;
}

static HCHAR * dlib_epg_tvtv_findRemovableCache(void)
{
	static HCHAR pszLink[512] = "";
	HCHAR * psLastLink = NULL;
	HUINT32	ulLast;
	dlib_epgsvc_t	*dlibsvc;
	HxHASH_TableIter_t	iter;


	ulLast = HLIB_STD_GetSystemTick();
	HLIB_HASH_IterInit(&iter, s_pstServiceEventTable);
	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&dlibsvc))
	{
		if (ulLast > dlibsvc->ulLastUsed)
		{
			ulLast = dlibsvc->ulLastUsed;
			psLastLink = dlibsvc->pszUid;
		}
	}

	HxSTD_MemSet(pszLink,0,512); //temp
	HLIB_STD_StrNCpySafe(pszLink, psLastLink, 512);
	return pszLink;
}

static HERROR	dlib_epg_tvtv_DeleteAllFile(void)
{
	// Remove Event Info
	dlib_epgsvc_t	*dlibsvc;
	HxHASH_TableIter_t	iter;

	HLIB_HASH_IterInit(&iter, s_pstServiceEventTable);
	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&dlibsvc))
	{
		HLIB_HASH_RemoveAll(dlibsvc->pstEventTable);
		DLIB_Free(dlibsvc->pszRepositoryName);
		DLIB_Free(dlibsvc->pszUid);
	}

	HLIB_HASH_RemoveAll(s_pstServiceEventTable);
	s_pstServiceEventTable = NULL;

	// path 관련 및 삭제 내용은 생각해보자..mw하는지..여기서하는지.
	if(HLIB_DIR_Delete(TVTVEPG_REPOSITORY_PATH) == ERR_FAIL)
	{
		HxLOG_Critical("TVTVEPG File delete Fail\n");
	}
	if(HLIB_DIR_Delete(TVTVEPG_THUMBNAIL_PATH) == ERR_FAIL)
	{
		HxLOG_Critical("TVTVEPG File delete Fail\n");
	}


	if (access(TVTVEPG_REPOSITORY_PATH, R_OK | W_OK) != 0)
		HLIB_DIR_Make(TVTVEPG_REPOSITORY_PATH);
	if (access(TVTVEPG_THUMBNAIL_PATH, R_OK | W_OK) != 0)
		HLIB_DIR_Make(TVTVEPG_THUMBNAIL_PATH);

	dlib_epg_tvtv_MakeSvcHashTable();
	return ERR_OK;
	// Remove Service Info
	// Delete All File
}

static HERROR	dlib_epg_tvtv_closeFile(dlib_epgsvc_t	*svcInfo)
{
	HxLOG_Trace();
	RETURN_IF(svcInfo == NULL, ERR_FAIL);

	if (svcInfo->bIsChanged && HLIB_HASH_Size(svcInfo->pstEventTable))
	{
		//	Write data to file.
		FILE				*file = NULL;
		HxHASH_TableIter_t	iter;
		dlib_epgevt_t		*dlibEvt;
		HINT32				evtKey, length, ulCRC;
		HUINT8				*p, *pBuffer;

		length = HLIB_HASH_Size(svcInfo->pstEventTable);
		p = pBuffer = (HUINT8*)DLIB_Malloc(sizeof(dlib_epgevt_t) * length);
		HLIB_HASH_IterInit(&iter, svcInfo->pstEventTable);
		while (HLIB_HASH_IterNext(&iter, (void**)&evtKey, (void**)&dlibEvt))
		{
			HxSTD_MemCopy(p, dlibEvt, sizeof(dlib_epgevt_t));
			p += sizeof(dlib_epgevt_t);
		}
#if defined(USE_CRC_CHECK)
		ulCRC = HLIB_MATH_GetCrc32(pBuffer, sizeof(dlib_epgevt_t) * length);
		HxLOG_Debug("data is changed --> write it number(%d) ulCRC(%x) Name[%s]\n", length, ulCRC, svcInfo->pszRepositoryName);
#endif
		if (access(TVTVEPG_REPOSITORY_PATH, R_OK | W_OK) != 0)
			HLIB_DIR_Make(TVTVEPG_REPOSITORY_PATH);

		if (access(TVTVEPG_THUMBNAIL_PATH, R_OK | W_OK) != 0)
			HLIB_DIR_Make(TVTVEPG_THUMBNAIL_PATH);


		file = fopen(svcInfo->pszRepositoryName, "wb");
		HxLOG_Debug("pszRepositoryName %d name[%s]\n", file, svcInfo->pszRepositoryName);

		if(file) fwrite(&length, 4, 1, file);

#if defined(USE_CRC_CHECK)
		if(file) (void)fseek(file, 8, SEEK_SET);
#endif

		if(file) fwrite(pBuffer, sizeof(dlib_epgevt_t) * length, 1, file);
		DLIB_Free(pBuffer);

		HLIB_HASH_IterInit(&iter, svcInfo->pstEventTable);
		while (HLIB_HASH_IterNext(&iter, (void*)&evtKey, (void**)&dlibEvt))
		{
			fwrite(dlibEvt, sizeof(dlib_epgevt_t) + dlibEvt->size, 1, file);
		}
#if defined(USE_CRC_CHECK)
		if(file)
		{
			if(fseek(file, 4, SEEK_SET) != 0)
			{
				fclose(file);
				goto GO_EXIT;
			}
		}
		if(file) fwrite(&ulCRC, 4, 1, file);
#endif
		if(file) fclose(file);

	}
	else if(HLIB_HASH_Size(svcInfo->pstEventTable) == 0)
	{
		if ( access(svcInfo->pszRepositoryName, F_OK) != -1 )
		{
			HxLOG_Debug("Data is NULL delete file[%s]\n", svcInfo->pszRepositoryName);
			unlink(svcInfo->pszRepositoryName);
		}
	}

GO_EXIT:
	HLIB_HASH_RemoveAll(svcInfo->pstEventTable);
	DLIB_Free(svcInfo->pszRepositoryName);
	DLIB_Free(svcInfo->pszUid);
	return ERR_OK;
}

static void		dlib_epg_tvtv_closeOldFile(void)
{
	dlib_epgsvc_t	*svcInfo;
	HCHAR *	pszLink = NULL;

	HxLOG_Trace();

	pszLink = dlib_epg_tvtv_findRemovableCache();
	svcInfo = (dlib_epgsvc_t*)HLIB_HASH_Lookup(s_pstServiceEventTable, pszLink);
	if(svcInfo)
	{
		HxLOG_Debug("oldest service event info : [%s]\n", pszLink);
		dlib_epg_tvtv_closeFile(svcInfo);
		HLIB_HASH_Remove(s_pstServiceEventTable, (void*)pszLink);
	}
}


static void		dlib_epg_tvtv_closeAllFile(void)
{
	dlib_epgsvc_t	*dlibsvc;
	HxHASH_TableIter_t	iter;
	HxLOG_Trace();

	HLIB_HASH_IterInit(&iter, s_pstServiceEventTable);

	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&dlibsvc))
	{
		dlib_epg_tvtv_closeFile(dlibsvc);
	}
	HLIB_HASH_RemoveAll(s_pstServiceEventTable);
	s_pstServiceEventTable = NULL;
	dlib_epg_tvtv_MakeSvcHashTable();
}


static dlib_epgsvc_t*	dlib_epg_tvtv_openFile(const HCHAR *pszLink)
{
	FILE	*file;
	HINT32	length = 0, i, ulCRC, ulOldCRC, nRead;
	dlib_epgsvc_t	*svcInfo;
	dlib_epgevt_t	*evtInfo, *e, *evt;
	static	HBOOL	bIsMounted = FALSE;

	HxLOG_Trace();
#ifdef CONFIG_MW_EPG_TVTV_NAND_STORAGE
#else
	if(bIsMounted == FALSE)
	{
		if(HLIB_DIR_IsMountedPath (TVTVEPG_MOUNT_PATH))
			bIsMounted = TRUE;
		else
			return NULL;
	}
#endif

	svcInfo = (dlib_epgsvc_t*)DLIB_Calloc(sizeof(dlib_epgsvc_t));
	svcInfo->pszRepositoryName = (HCHAR*)DLIB_Calloc(512);
	svcInfo->pszUid = (HCHAR*)DLIB_Calloc(512);

	HxSTD_snprintf(svcInfo->pszRepositoryName, 511, "%s/%s.dat", TVTVEPG_REPOSITORY_PATH, pszLink);
	HxSTD_snprintf(svcInfo->pszUid, 511, "%s", pszLink);

	//HxLOG_Print ("pszRepositoryName[%s] [%s]\n", svcInfo->pszRepositoryName, pszLink);

	svcInfo->ulLastUsed = HLIB_STD_GetSystemTick();

	svcInfo->pstEventTable = HLIB_HASH_NewFull(dlib_epg_tvtv_hash, dlib_epg_tvtv_hashvalue_equal, NULL, (_HxDestroyNotify)DLIB_Free_CB);

	HLIB_HASH_Insert(s_pstServiceEventTable, (void*)DLIB_StrDup(pszLink), (void*)svcInfo);

	HxLOG_Debug("svcInfo RepositoryName : [%s]\n", svcInfo->pszRepositoryName);
	file = fopen(svcInfo->pszRepositoryName, "rb");
	if (file == NULL)
		return svcInfo;

	nRead = fread(&length, 4, 1, file);
	if(nRead < 1)
	{
		HxLOG_Error("Error : Get Length\n");
		fclose(file);
		return svcInfo;
	}

#if defined(USE_CRC_CHECK)
	nRead = fread(&ulOldCRC, 4, 1, file);
	if(nRead < 1)
	{
		HxLOG_Error("Error : Get Old Crc\n");
		fclose(file);
		return svcInfo;
	}
#endif

	if( (length < 0) || (ulOldCRC < 0) )
	{
		fclose(file);
		return svcInfo;
	}

	if( length > MAX_EVENT_NUMBER )
	{
		fclose(file);
		return svcInfo;
	}

	evtInfo = (dlib_epgevt_t*)DLIB_Malloc(sizeof(dlib_epgevt_t) * length);
	nRead = fread(evtInfo, (HUINT32)(sizeof(dlib_epgevt_t) * length), 1, file);
	if(nRead < 1)
	{
		HxLOG_Error("Error : Get Event Info\n");
		DLIB_Free(evtInfo);
		fclose(file);
		return svcInfo;
	}
	e = evtInfo;

#if defined(USE_CRC_CHECK)
	ulCRC = HLIB_MATH_GetCrc32((HUINT8 *)evtInfo, sizeof(dlib_epgevt_t) * length);

	if(ulOldCRC != ulCRC)
	{
		HxLOG_Critical("CRC FAIL OLDCRC[%x] CRC[%x]\n", ulOldCRC, ulCRC);
		DLIB_Free(evtInfo);
		fclose(file);
	}
	else
#endif
	{
		HxLOG_Debug("link[%s] number of event [%d] ulCRC[%x]\n", pszLink, length, ulCRC);
		for (i = 0; i < length; i++)
		{
			HINT32	size = sizeof(dlib_epgevt_t) + e->size;

			nRead = 0;
			evt = (dlib_epgevt_t*)DLIB_Calloc(size);
			nRead = fread(evt, 1, size, file);
			e++;

			if (nRead == size)
			{
				HLIB_HASH_Insert(svcInfo->pstEventTable, (void*)TVTVEPG_MAKE_KEY(evt->uid), (void*)evt);
			}
			else
			{
				HxLOG_Error("fail to get tvtv epg info [%d/%d]\n", nRead, size);
				DLIB_Free(evt);
			}
		}
		DLIB_Free(evtInfo);
		fclose(file);
	}
	return svcInfo;
}

static dlib_epgsvc_t*	dlib_epg_tvtv_getSvcInfo(const HCHAR *pszLink)
{
	dlib_epgsvc_t	*pstSvcInfo;

	pstSvcInfo = (dlib_epgsvc_t*)HLIB_HASH_Lookup(s_pstServiceEventTable, (void*)pszLink);

	if (pstSvcInfo == NULL)
	{
		//	check cache count and close it if needs
		HxLOG_Debug("dlib_epg_tvtv_getSvcInfo pstSvcInfo NULL size[%d]\n", HLIB_HASH_Size(s_pstServiceEventTable));
		if (HLIB_HASH_Size(s_pstServiceEventTable) > MAX_TVTVEPG_SERVICE_NUM)
			dlib_epg_tvtv_closeOldFile();

		pstSvcInfo = dlib_epg_tvtv_openFile(pszLink);
	}
	else
	{
		HxLOG_Debug("dlib_epg_tvtv_getSvcInfo It is a pstSvcInfo size[%d]\n", HLIB_HASH_Size(s_pstServiceEventTable));
	}

	return pstSvcInfo;
}

HERROR	dlib_epg_tvtv_read_file(dlib_epgsvc_t *pstSvcInfo, HUID evtUid, HINT32 *pnSize, void **ppvData)
{
	HUINT8			*pucData;
	dlib_epgevt_t	*evtInfo;

	if(pstSvcInfo == NULL)
		return ERR_FAIL;

	evtInfo = HLIB_HASH_Lookup(pstSvcInfo->pstEventTable, (void*)TVTVEPG_MAKE_KEY(evtUid));
	if (evtInfo == NULL)
		return ERR_FAIL;

	*pnSize = evtInfo->size;
	pucData = (HUINT8*)DLIB_Malloc(*pnSize);
	HxSTD_MemCopy(pucData, evtInfo->data, *pnSize);
	*ppvData = (void*)pucData;

	pstSvcInfo->ulLastUsed = HLIB_STD_GetSystemTick();
	return ERR_OK;
}

HERROR	dlib_epg_tvtv_write_file(dlib_epgsvc_t *pstSvcInfo, HUID evtUid, HINT32 nSize, const void *pvData)
{
	dlib_epgevt_t	*evtInfo;

	if(pstSvcInfo == NULL)
		return ERR_FAIL;

	evtInfo = HLIB_HASH_Lookup(pstSvcInfo->pstEventTable, (void*)TVTVEPG_MAKE_KEY(evtUid));
	if(nSize == 0)
	{
		//HxLOG_Debug("remove Event UID[%lld].\n", evtUid);
		if(evtInfo) HLIB_HASH_Remove(pstSvcInfo->pstEventTable, (void*)TVTVEPG_MAKE_KEY(evtUid));
		pstSvcInfo->bIsChanged = TRUE;
		pstSvcInfo->ulLastUsed = HLIB_STD_GetSystemTick() - 2*60*60;
	}
	else
	{
		if (evtInfo)
		{
			if ((evtInfo->size == nSize) && (HxSTD_MemCmp(evtInfo->data, pvData, nSize) == 0))
			{
				//HxLOG_Debug("same data comming...\n");
				return ERR_OK;
			}
			HLIB_HASH_Remove(pstSvcInfo->pstEventTable, (void*)TVTVEPG_MAKE_KEY(evtUid));
		}

		evtInfo = (dlib_epgevt_t*)DLIB_Malloc(sizeof(dlib_epgevt_t) + nSize);
		evtInfo->size = nSize;
		evtInfo->uid = evtUid;
		HxSTD_MemCopy(evtInfo->data, pvData, nSize);
		HLIB_HASH_Insert(pstSvcInfo->pstEventTable, (void*)TVTVEPG_MAKE_KEY(evtUid), (void*)evtInfo);

		pstSvcInfo->bIsChanged = TRUE;
		pstSvcInfo->ulLastUsed = HLIB_STD_GetSystemTick() - 2*60*60;
	}

	return ERR_OK;
}


HERROR			DLIB_EPG_TVTV_ReadData(HUID uid, const HCHAR *pszLink, void **ppvData, HUINT32 *pnSize)
{
	HUID			evtUid;
	dlib_epgsvc_t	*pstSvcInfo;

	evtUid = uid;
	pstSvcInfo = dlib_epg_tvtv_getSvcInfo(pszLink);


	HxLOG_Debug("readData : uid[%lld] pszLink[%s]\n", evtUid, pszLink);
	return dlib_epg_tvtv_read_file(pstSvcInfo, evtUid, pnSize, ppvData);
}

HERROR			DLIB_EPG_TVTV_WriteData(HUID uid, const HCHAR *pszLink, const void *pvData, HUINT32 size)
{
	HUID			evtUid;
	dlib_epgsvc_t	*pstSvcInfo;
	evtUid = uid;
	//HxLOG_Debug("writeData : uid[%lld] pvData[%08x] size[%d] pszLink[%s]\n", evtUid, pvData, size, pszLink);
	if(uid == -1 && size == 0)
	{
		return dlib_epg_tvtv_DeleteAllFile();
	}
	else
	{
		pstSvcInfo = dlib_epg_tvtv_getSvcInfo(pszLink);

		return dlib_epg_tvtv_write_file(pstSvcInfo, evtUid, size, pvData);
	}
}

HERROR			DLIB_EPG_TVTV_LinkName(HUID uid, const void *pvData, HUINT32 size, void *name, HUINT32 nameSize)
{
	DxTvtvEvent_t * pEvent = (DxTvtvEvent_t *)pvData;

	RETURN_IF(pvData == NULL || size == 0 || name == NULL || nameSize == 0, FALSE);
#if 1
HxSTD_PrintToStrN(name, nameSize, "%x"
					, pEvent->stBase.ulSvcUid
				);
#else
	HxSTD_PrintToStrN(name, nameSize, "%x.%x.%x"
					, pEvent->stBase.usOnId
					, pEvent->stBase.usTsId
					, pEvent->stBase.usSvcId
				);
#endif
	return TRUE;
}


HERROR DLIB_EPG_TVTV_SyncData(void)
{
	dlib_epg_tvtv_closeAllFile();
	return ERR_OK;
}
static void	dlib_epg_tvtv_Print_Event(DxEvent_t *event)
{
#if (0) //for debugging
	HINT32		i;

	if(event == NULL)
		return;

	HxLOG_Print(
			"\nDxEvent_t(0x%X) refCount:%d\n"
			"\ttype:%s\n"
			"\tsvcuid:%d"
			"\teventId:0x%X\n"
			"\tstartTime:%s, duration:%usec\n"
			"\trunning status:0x%X, freeCAmode:%d\n"
			"\tname:\'%s\'\n"
			"\ttext:\'%s\'\n"
			"\tlongText:\'%s\'\n"
			"\tparantal rating:%d\n"
			, (int)event, event->refCount
			, DxEPG_NAMEOF(event->type)
			, event->svcuid
			, event->eventId
			, HLIB_DATETIME_UnixTimeToString((UNIXTIME)event->startTime, NULL, 0), event->duration
			, event->runStatus, event->freeCaMode
			, DxEPG_EVENT_GET_STRING(event->name)
			, DxEPG_EVENT_GET_STRING(event->text)
			, DxEPG_EVENT_GET_STRING(event->longText)
			, event->parentalRating
		);

	HxLOG_Print("\tcontent:");
	for (i = 0 ; i < DxEVENT_CONTENT ; i++)
		HxLOG_Print (" %04X", event->content[i]);
	HxLOG_Print("\n");

	HxLOG_Print(
			"\tSeriesId : %d, Season : %d\n"
			"\tEpisode : %d / %d\n"
			"\tlink[0] %s  id : %d\n"
			"\tlink[1] %s  id : %d\n"
			"\tthumbnail : %s\n"
			"\tactor      : %s\n"
			"\tactress 	 : %s\n"
			"\tdirector   : %s\n"
			"\tperson     : %s\n"
			,event->extension.tvtvEpg.seriesId
			,event->extension.tvtvEpg.season
			,event->extension.tvtvEpg.episodeNum
			,event->extension.tvtvEpg.totalNum
			/*,(event->extension.tvtvEpg.seriesType == 1)?"using Series ID":"using CRID"*/
			,(event->extension.tvtvEpg.eventLink[0].ucType == 1)?"Same":"Next", event->extension.tvtvEpg.eventLink[0].ulProgId
			,(event->extension.tvtvEpg.eventLink[1].ucType == 1)?"Same":"Next", event->extension.tvtvEpg.eventLink[1].ulProgId
			,event->extension.tvtvEpg.thumbnailURL
			,event->extension.tvtvEpg.actor
			,event->extension.tvtvEpg.actress
			,event->extension.tvtvEpg.director
			,event->extension.tvtvEpg.person
		);
#endif
}


void 			DLIB_EPG_TVTV_Print_Event(FILE *fp, const void *t)
{
	DxEvent_t	*event = (DxEvent_t *)t;
	HINT32		i;

	if(event == NULL || fp == NULL)
		return;

	fprintf(fp,
			"\nDxEvent_t(0x%X) refCount:%d\n"
			"\ttype:%s\n"
			"\tsvcuid:%d"
			"\teventId:0x%X\n"
			"\tstartTime:%s, duration:%usec\n"
			"\trunning status:0x%X, freeCAmode:%d\n"
			"\tname:\'%s\'\n"
			"\ttext:\'%s\'\n"
			"\tlongText:\'%s\'\n"
			"\tparantal rating:%d\n"
			, (int)event, event->refCount
			, DxEPG_NAMEOF(event->type)
			, event->svcuid
			, event->eventId
			, HLIB_DATETIME_UnixTimeToString((UNIXTIME)event->startTime, NULL, 0), event->duration
			, event->runStatus, event->freeCaMode
			, DxEPG_EVENT_GET_STRING(event->name)
			, DxEPG_EVENT_GET_STRING(event->text)
			, DxEPG_EVENT_GET_STRING(event->longText)
			, event->parentalRating
		);

	fprintf(fp, "\tcontent:");
	for (i = 0 ; i < DxEVENT_CONTENT ; i++)
		fprintf(fp, " %04X", event->content[i]);
	fprintf(fp, "\n");

	fprintf(fp,
			"\tSeriesId : %d, Season : %d\n"
			"\tEpisode : %d / %d\n"
			"\tlink[0] %s  id : %d\n"
			"\tlink[1] %s  id : %d\n"
			"\tthumbnail : %s\n"
			"\tactor      : %s\n"
			"\tactress 	 : %s\n"
			"\tdirector   : %s\n\n"
			,event->extension.tvtvEpg.seriesId
			,event->extension.tvtvEpg.season
			,event->extension.tvtvEpg.episodeNum
			,event->extension.tvtvEpg.totalNum
			/*,(event->extension.tvtvEpg.seriesType == 1)?"using Series ID":"using CRID"*/
			,(event->extension.tvtvEpg.eventLink[0].ucType == 1)?"Same":"Next", event->extension.tvtvEpg.eventLink[0].ulProgId
			,(event->extension.tvtvEpg.eventLink[1].ucType == 1)?"Same":"Next", event->extension.tvtvEpg.eventLink[1].ulProgId
			,event->extension.tvtvEpg.thumbnailURL
			,event->extension.tvtvEpg.actor
			,event->extension.tvtvEpg.actress
			,event->extension.tvtvEpg.director
		);
}




