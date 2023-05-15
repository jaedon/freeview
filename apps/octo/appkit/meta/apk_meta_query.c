/**************************************************************
 *	@file		apk_meta_query.c
 *
 *	http://www.humaxdigital.com
 *
 **************************************************************/
#include	<oapi.h>

#include 	<dapi.h>
#include  	<apk.h>
//#include	"meta_int.h"
#include	"_apk_int.h"

#define	_RETURN_IF_VOID(expr)	do{if(expr)return;}while(0)
#define	_RETURN_IF(expr, err)	do{if(expr)return (err);}while(0)
#define	_GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

#define	_D_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	_D_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	_D_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

#define	_W_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Warning("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	_W_RETURN_IF(expr, err)	do{if(expr){HxLOG_Warning("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	_W_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Warning("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

#define	_E_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Error("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	_E_RETURN_IF(expr, err)	do{if(expr){HxLOG_Error("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	_E_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Error("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

#define	APK_EVENT_GET_STRING(vEventString)	vEventString?((HCHAR*)((DxEventString_t *)HLIB_VECTOR_First(vEventString))->string):"NULL"

#define	META_QUERY_MERGE_PF_TO_IP

#ifdef	META_QUERY_MERGE_PF_TO_IP
#define	META_QUERY_PADDING			(5 * 60)
#else
#define	META_QUERY_PADDING			0
#endif
#define	METAQ_STARTTIME(e)	(e->startTime)
#define METAQ_ENDTIME(e)	(e->startTime + e->duration - 1)

#define	____PRIVATE_TYPE___________________________________________________________________________


typedef struct
{
	HxReturnFunc_t	callback;
	void			*userdata;
	HINT32			option;
	union {
		HxVector_t	*list;
		DxEvent_t	*event;
		void		*result;
	} u;
} _QResult_t;

typedef struct _QUERY	_Query_t;
struct _QUERY
{
	HCHAR		type;			///< type ('&', '|', '!', '\0')

	HCHAR		*field;			///< field
	HCHAR		*value;			///< value

	HCHAR		comparison[12];	///< ==, !=, <, <=, >, >=, contains, exist

	_Query_t	*childs[2];		///< parse tree¿¡¼­ÀÇ ÀÚ½Ä ³ëµå
};

typedef HINT32	(* _Compare_t)(const DxEvent_t *, const DxEvent_t *);
typedef struct
{
	HINT32		ascending;
	HCHAR		field[64];
	_Compare_t	compare;
} _Order_t;

typedef struct
{
	//2 Search Params
	APK_META_QUERY_Target_e		eTarget;		///< query target
	_Query_t				*query;		///< query
	HxVector_t				*svcCons;	///< service constraints
	HxVector_t				*svcOrder;	///< order by
	APK_META_QUERY_Filter_e		eFilter;		///< filter
	HCHAR					*sql;		///< sql

	//2 Search Results
	HINT32					qid;
	HxVector_t				*result;	///< search results
	HUINT32					requestTime;

	//2 Search Notifier
	HUINT32					accessCode;	///< access code
	HUINT32					notifierId;	///< notifier ID
	APK_META_QUERY_Notifier_t	fnNotifier;
	void 					*userdata;
} _QSearch_t;


typedef struct
{
	HxVector_t		*list;
	HINT32			nextQID;
	HINT32			sql;
	HxStack_t		*stack;
	HxQueue_t		*queue;
	HxTREE_t		*tree;
	HxVector_t		*tempVector;
	HxHASH_t	*tempHash;
} _QSearchManager_t;

// ÀÌ¸§ ÀÌ»óÇÏ°Ô Áö¾î¼­ ÁË¼ÛÇÕ´Ï´Ù....
typedef struct
{
	HINT32			eventId;
	HCHAR			szName[256];
} _EventNameId_t;

static _QSearchManager_t *	metaquery_GetManager (void);
// static HUINT32		metaquery_operation_Notifier (HUINT32 accessCode, HxJSON_t param);

static	HUINT32		_ulModuleSemaphoreId = 0;

#ifdef	CONFIG_DEBUG
//#define	METAQ_SEM_DEBUG		3000
#endif

#ifdef	METAQ_SEM_DEBUG
typedef struct
{
	const HCHAR *func;
	HINT32       line;
	HUINT32      tick;
} _QSem_t;

static HINT32	metaquery_dbg_SemLock (_QSem_t *sem, HUINT32 id, const HCHAR *func, HINT32 line)
{
	HINT32	ret;

	ret  = HxSEMT_Get(id);

	sem->func = func;
	sem->line = line;
	sem->tick = HLIB_STD_GetSystemTick();

	return ret;
}

static HINT32	metaquery_dbg_SemUnlock (_QSem_t *sem, HUINT32 id, const HCHAR *func, HINT32 line)
{
	HUINT32	elapsedTick;

	elapsedTick = HLIB_STD_GetSystemTick() - sem->tick;
	if (elapsedTick > METAQ_SEM_DEBUG)
	{
		HxLOG_Print("\n\tMETA Q: The time semaphore was locked (%u msec) by %s(%d).\n\n"
				, elapsedTick
				, sem->func
				, sem->line
			);
	}
	return HxSEMT_Release(id);
}

static _QSem_t	s_qsem;
#define	METAQ_LOCK		metaquery_dbg_SemLock(&s_qsem, _ulModuleSemaphoreId, __FUNCTION__, __LINE__)
#define	METAQ_UNLOCK	metaquery_dbg_SemUnlock(&s_qsem, _ulModuleSemaphoreId, __FUNCTION__, __LINE__)
#else

#define	METAQ_LOCK		do {																				\
							if (0 == _ulModuleSemaphoreId)													\
							{																				\
								HxSEMT_Create(&_ulModuleSemaphoreId, "sametaquery", HxSEMT_FIFO);			\
								if (0 == _ulModuleSemaphoreId)												\
								{																			\
									HxLOG_Critical("!!!! Semaphore ID NULL !!!\n");							\
								}																			\
							}																				\
																											\
							HxSEMT_Get(_ulModuleSemaphoreId);												\
						} while(0)


#define	METAQ_UNLOCK	do {																				\
							if (0 == _ulModuleSemaphoreId)													\
							{																				\
								HxSEMT_Create(&_ulModuleSemaphoreId, "sametaquery", HxSEMT_FIFO);			\
								if (0 == _ulModuleSemaphoreId)												\
								{																			\
									HxLOG_Critical("!!!! Semaphore ID NULL !!!\n"); 						\
								}																			\
							}																				\
																											\
							HxSEMT_Release(_ulModuleSemaphoreId);												\
						} while(0)
#endif


#define	____PRIVATE_API_____________________________________________________________________________

#ifdef	CONFIG_DEBUG
static _QSearch_t *
	metaquery_ToSearch (ApkMetaQHandle_t handle, const char *caller)
{
	_QSearchManager_t	*mgr;

	mgr = metaquery_GetManager();

	if (HLIB_VECTOR_IndexOf(mgr->list, (void *)handle) < 0)
	{
		HxLOG_Error("[%s] invalid handle(0x%X) call by %s\n", __FUNCTION__, (int)handle, caller);
		return NULL;
	}
	return (_QSearch_t *)handle;
}

#define	QSEARCH(h)	metaquery_ToSearch(h, __FUNCTION__)
#else
#define	QSEARCH(h)	((_QSearch_t *)(h))
#endif


static HCHAR *
	metaquery_CheckQuote(const HCHAR *pString)
{
	HINT32 ulQuoteCount;
	HCHAR *pRet = NULL;

	ulQuoteCount = HLIB_STD_StrCountChar (pString, '\'');
	if (ulQuoteCount == 0)
	{
		pRet = APK_StrDup(pString);
		return pRet;
	}

	pRet = APK_Calloc(HxSTD_StrLen(pString)+1+ulQuoteCount);
	if(pRet)
	{
		HLIB_STD_StrReplace(pRet, pString,  "\'","\'\'");
	}
	return pRet;
}

static _Query_t *
	metaquery_AllocQ (const HCHAR *field, const HCHAR *comparison, const HCHAR *value)
{

	_Query_t *q;

	q = (_Query_t *)APK_Calloc(sizeof(_Query_t));
	if (q)
	{
		q->field = APK_StrDup(field);
		q->value = APK_StrDup(value);
		if (comparison)
		{
			HLIB_STD_StrNCpySafe(q->comparison, comparison, 12);
		}
	}
	return q;
}

static _Query_t *
	metaquery_CloneQ (const _Query_t *q)
{
	_Query_t *c;

	if (q == NULL)
		return NULL;

	c = metaquery_AllocQ(q->field, q->comparison, q->value);
	if (c)
	{
		c->type  = q->type;
		c->childs[0] = metaquery_CloneQ(q->childs[0]);
		c->childs[1] = metaquery_CloneQ(q->childs[1]);
	}
	return c;
}

static _Query_t *
	metaquery_ReleaseQ (_Query_t *q)
{
	if (q)
	{
		q->childs[0] = metaquery_ReleaseQ(q->childs[0]);
		q->childs[1] = metaquery_ReleaseQ(q->childs[1]);

		if (q->field)
		{
			APK_Free(q->field);
			q->field = NULL;
		}

		if (q->value)
		{
			APK_Free(q->value);
			q->value = NULL;
		}

		APK_Free(q);
		q = NULL;
	}

	return NULL;
}
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
static _Query_t *
	metaquery_AllocProgrammeIdQ (const HCHAR *field, const HCHAR *comparison, const HCHAR *value)
{
	HCHAR	buf[128];
	HCHAR	*token;
	HINT32	svcuid, eventid;
	HUID	uid;

	HLIB_STD_StrCaseCpy(buf, value, 128);
	token = strchr(buf, '.');
	_E_RETURN_IF(token == NULL, NULL);
	*token++ = '\0';
	_E_RETURN_IF(!HLIB_STD_IsInteger(buf) || !HLIB_STD_IsInteger(token), NULL);

	svcuid  = HLIB_STD_StrToINT32(buf);
	eventid = HLIB_STD_StrToINT32(token);
	uid = DxEVENT_UID(svcuid, eventid);

	HxSTD_PrintToStrN(buf, 128, HUID_FMT, uid);
	return metaquery_AllocQ("uid", comparison, buf);
}
#endif

static void
	metaquery_Traverse (_Query_t *q, void (* each) (_Query_t *, void *), void *userdata)
{
	if (q && each)
	{
		if (q->childs[0])
			metaquery_Traverse(q->childs[0], each, userdata);
		if (q->childs[1])
			metaquery_Traverse(q->childs[1], each, userdata);

		each(q, userdata);
	}
}


#if defined(CONFIG_DEBUG)
void metaquery_DumpQuery (_Query_t *q, void *userdata)
{
	HxLOG_Print("<pre-order> query[0x%08X(L:0x%08X,R:0x%08X)]::type:%c (%s(f) %s %s(v))\n",
				(int)q,
				(int)q->childs[0],
				(int)q->childs[1],
				(char)q->type,
				q->field ? q->field : "N/A",
				q->comparison ? q->comparison : "N/A",
				q->value ? q->value : "N/A"
			);
}
#endif

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
static void *
	metaquery_FreeSvcCon (void *data)
{
	if (data)
	{
		APK_Free(data);
		data = NULL;
	}
	return NULL;
}
#endif

static void *
	metaquery_FreeOrder (void *data)
{
	if (data)
	{
		APK_Free(data);
		data = NULL;
	}
	return NULL;
}


static void *
	metaquery_ReleaseQSearch (_QSearch_t *thiz)
{
	if (thiz)
	{
		if (thiz->query)
		{
			metaquery_ReleaseQ(thiz->query);
			thiz->query = NULL;
		}

		if (thiz->svcCons)
		{
			HLIB_VECTOR_Delete(thiz->svcCons);
			thiz->svcCons = NULL;
		}

		if (thiz->svcOrder)
		{
			HLIB_VECTOR_Delete(thiz->svcOrder);
			thiz->svcOrder = NULL;
		}

		if (thiz->result)
		{
			HLIB_VECTOR_Delete(thiz->result);
			thiz->result = NULL;
		}

		if (thiz->sql)
		{
			APK_Free(thiz->sql);
			thiz->sql = NULL;
		}

		APK_Free(thiz);
		thiz = NULL;
	}
	return NULL;
}

#if defined(META_QUERY_MERGE_PF_TO_IP)
static HBOOL
	metaquery_IsOverlapped (const DxEvent_t *lhs, const DxEvent_t *rhs)
{

	if((rhs->type == eDxEPG_TYPE_PF && lhs->type == eDxEPG_TYPE_IP)
		|| (lhs->type == eDxEPG_TYPE_PF && rhs->type == eDxEPG_TYPE_IP)
		|| (rhs->type == eDxEPG_TYPE_PF && lhs->type == eDxEPG_TYPE_TVTV)
		|| (lhs->type == eDxEPG_TYPE_PF && rhs->type == eDxEPG_TYPE_TVTV))
	{

		HUINT32 ulBigStart = 0, ulSmallStart = 0;
		HUINT32 ulBigEnd = 0,ulSmallEnd = 0;

	 	if((METAQ_STARTTIME(rhs) == METAQ_STARTTIME(lhs)) && (METAQ_ENDTIME(lhs) == METAQ_ENDTIME(rhs)))	// rhs s <= lhs s lhs e <= rhs e
			return TRUE;

		if(METAQ_STARTTIME(lhs) > METAQ_STARTTIME(rhs))	// start Time 5ºÐÀÌ³» ÀÎÁö È®ÀÎ.
		{
			ulBigStart = METAQ_STARTTIME(lhs);
			ulSmallStart= METAQ_STARTTIME(rhs);
		}
		else
		{
			ulBigStart = METAQ_STARTTIME(rhs);
			ulSmallStart= METAQ_STARTTIME(lhs);
		}

		if(METAQ_ENDTIME(lhs) > METAQ_ENDTIME(rhs))	// start Time 5ºÐÀÌ³» ÀÎÁö È®ÀÎ.
		{
			ulBigEnd = METAQ_ENDTIME(lhs);
			ulSmallEnd = METAQ_ENDTIME(rhs);
		}
		else
		{
			ulBigEnd = METAQ_ENDTIME(rhs);
			ulSmallEnd = METAQ_ENDTIME(lhs);
		}

		if(ulSmallStart + META_QUERY_PADDING >= ulBigStart
			&& ulSmallEnd + META_QUERY_PADDING >= ulBigEnd)
			return TRUE;
	}
	return FALSE;
}
static HINT32
	metaquery_CompareEventDup (const DxEvent_t *lhs, const DxEvent_t *rhs)
{
	HINT32	diff;
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	diff = lhs->onid - rhs->onid;
	if(diff == 0)
		diff = lhs->tsid - rhs->tsid;
	if(diff == 0)
		diff = lhs->svcid - rhs->svcid;
#else
	diff = lhs->svcuid - rhs->svcuid;
#endif
	if (diff == 0)
	{
		if((METAQ_STARTTIME(rhs) <= METAQ_STARTTIME(lhs)) && (METAQ_ENDTIME(lhs) <= METAQ_ENDTIME(rhs)))	// rhs s <= lhs s lhs e <= rhs e
			return 0;
		else if((METAQ_STARTTIME(lhs) <= METAQ_STARTTIME(rhs)) && (  METAQ_ENDTIME(rhs)) <= METAQ_ENDTIME(lhs))	// lhs s <= rhs s  rhs e <=  lhs e
			return 0;
		else if ((METAQ_STARTTIME(lhs) < METAQ_STARTTIME(rhs)) && (METAQ_STARTTIME(rhs) < METAQ_ENDTIME(lhs))) // lhs s < rhs s  < lhs e
			return 0;
		else if ((METAQ_STARTTIME(lhs) < METAQ_ENDTIME(rhs)) && (METAQ_ENDTIME(rhs) < METAQ_ENDTIME(lhs))) // lhs s < rhs e  < lhs e
			return 0;
		else if (METAQ_STARTTIME(lhs) < METAQ_STARTTIME(rhs))
			return -1;
		else if (METAQ_STARTTIME(lhs) > METAQ_STARTTIME(rhs))
			return +1;

		return 0;
	}
	return diff;
}

#else

static HBOOL
	metaquery_IsOverlapped (const DxEvent_t *lhs, const DxEvent_t *rhs)
{
	if (METAQ_ENDTIME(rhs) < METAQ_STARTTIME(lhs) || METAQ_ENDTIME(lhs) < METAQ_STARTTIME(rhs))
		return FALSE;
	return TRUE;
}


static HINT32
	metaquery_CompareEventDup (const DxEvent_t *lhs, const DxEvent_t *rhs)
{
	HINT32	diff;

	diff = lhs->svcuid - rhs->svcuid;
	if (diff == 0)
	{
		if (METAQ_STARTTIME(lhs) < METAQ_STARTTIME(rhs))
		{
			if (METAQ_ENDTIME(lhs) >= METAQ_ENDTIME(rhs))
				return 0;
			return -1;
		}
		else if (METAQ_STARTTIME(lhs) > METAQ_STARTTIME(rhs))
		{
			if (METAQ_ENDTIME(lhs) <= METAQ_ENDTIME(rhs))
				return 0;
			return +1;
		}
		return 0;
	}
	return diff;
}

static DxEvent_t *
	metaquery_SetPaddingTime (DxEvent_t *event, HINT32 offset)
{
	HUINT32	padding;

	if (offset == 0)
		return event;
	else if (offset < 0)
	{
		padding = (HUINT32)(-offset);
		event->startTime += padding;
		event->duration  -= padding * 2;
	}
	else
	{
		padding = (HUINT32)offset;
		event->startTime -= padding;
		event->duration  += padding * 2;
	}
	return event;
}
#endif

static _QSearchManager_t *
	metaquery_GetManager (void)
{
	static _QSearchManager_t	s_manager = {0,};
	if (s_manager.list == NULL)
	{
		s_manager.list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))metaquery_ReleaseQSearch, NULL);
		HxLOG_Assert(s_manager.list);
		s_manager.sql  = HLIB_RWSTREAM_Open(NULL, 0);
		HxLOG_Assert(s_manager.sql);
		s_manager.stack = HxStack_New(NULL, NULL);
		HxLOG_Assert(s_manager.stack);
		s_manager.queue = HLIB_QUEUE_New(NULL);
		HxLOG_Assert(s_manager.queue);
		s_manager.tree  = HLIB_TREE_New((HxCompareFunc)metaquery_CompareEventDup);
		HxLOG_Assert(s_manager.tree);
		s_manager.tempVector = HLIB_VECTOR_New(NULL, NULL);
		HxLOG_Assert(s_manager.tempVector);
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
		s_manager.tempHash = HLIB_HASH_SelfNew();
#else
		s_manager.tempHash = HLIB_HASH_New(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual);
#endif

		HxLOG_Assert(s_manager.tempHash);

		HLIB_TREE_NodeCache(s_manager.tree, TRUE);
	}
	return &s_manager;
}

static HINT32
	metaquery_NextQID (void)
{
	_QSearchManager_t *mgr = metaquery_GetManager();
	return ++(mgr->nextQID);
}


static HINT32
	metaquery_GetSQLBuf (void)
{
	_QSearchManager_t *mgr = metaquery_GetManager();
	HLIB_RWSTREAM_Rewind(mgr->sql);
	return mgr->sql;
}

static HxStack_t *
	metaquery_GetStack (void)
{
	_QSearchManager_t *mgr = metaquery_GetManager();
	HLIB_VECTOR_RemoveAll((HxVector_t *)mgr->stack);
	return mgr->stack;
}

static HxQueue_t *
	metaquery_GetQueue (void)
{
	_QSearchManager_t *mgr = metaquery_GetManager();
	HLIB_QUEUE_Clear(mgr->queue);
	return mgr->queue;
}

static HxTREE_t *
	metaquery_GetTree (void)
{
	_QSearchManager_t *mgr = metaquery_GetManager();
	HLIB_TREE_RemoveAll(mgr->tree);
	return mgr->tree;
}

static HxVector_t *
	metaquery_GetVector (void)
{
	_QSearchManager_t *mgr = metaquery_GetManager();
	HLIB_VECTOR_RemoveAll(mgr->tempVector);
	return mgr->tempVector;
}

static HxHASH_t *
	metaquery_GetHash (void)
{
	_QSearchManager_t *mgr = metaquery_GetManager();
	HLIB_HASH_RemoveAll(mgr->tempHash);
	return mgr->tempHash;
}


static const HCHAR *
	metaquery_ValidateField (const HCHAR *in)
{
	static const struct field_map {
		const HCHAR *in;
		const HCHAR *out;
	} s_fieldMap[] = {
		  {"endtime", "end_time"}
		, {"starttime", "start_time"}
		, {"eventname", "name"}
		, {"event_name", "name"}
		, {"event_id", "eventid"}
	};
	static HCHAR	s_buf[256];
	HUINT32 		i, n;

	n = sizeof(s_fieldMap) / sizeof(s_fieldMap[0]);
	for (i = 0 ; i < n ; i++)
	{
		if (HLIB_STD_StrCaseCmp(s_fieldMap[i].in, in) == 0)
			return s_fieldMap[i].out;
	}
	return HLIB_STD_StrCaseCpy(s_buf, in, TRUE);
}


static const HCHAR *
	metaquery_ValidateComparison (const HCHAR *in)
{
	static const struct comp_map {
		const HCHAR *in;
		const HCHAR *out;
	} s_compMap[] = {{"==", "="}, {"!=", "<>"}, {"contains", "LIKE"}};
	HUINT32 		i, n;

	n = sizeof(s_compMap) / sizeof(s_compMap[0]);
	for (i = 0 ; i < n ; i++)
	{
		if (strcmp(s_compMap[i].in, in) == 0)
			return s_compMap[i].out;
	}
	return in;
}



static HCHAR *
	metaquery_QueryToString (_Query_t *query)
{
	const HCHAR	*field;
	const HCHAR	*comparison;
	HCHAR *ret=NULL,*check;

	if (query->field == NULL)
		return NULL;

	field = metaquery_ValidateField(query->field);
	comparison = metaquery_ValidateComparison(query->comparison);

	if (HLIB_STD_IsInteger(query->value))
	{
		return HLIB_STD_FmtString(NULL, 0, "%s %s %s", field, comparison, query->value);
	}
	else
	{
		check = metaquery_CheckQuote(query->value);
		if(check)
		{
			ret =HLIB_STD_FmtString(NULL, 0
					, strcmp(comparison, "LIKE") == 0 ? "%s %s \'%%%s%%\'" : "%s %s \'%s\'"
					, field, comparison, check);
			APK_Free(check);
		}
		return ret;
	}
}

static void
	metaquery_MakeQuerySQL (_Query_t *query, HxQueue_t *queue)
{
	HLIB_QUEUE_Push(queue, query);
}

static HCHAR *
	metaquery_MakeSQL (_QSearch_t *search)
{
	HINT32		sd;
	HxStack_t	*stack;
	HxQueue_t	*queue;
	_Query_t	*query;
	HCHAR		*operand[2];
	HCHAR		*result = NULL;
	_Order_t 	*order;
	DxEPG_Type_e	type;
	HINT32		i, n, uni;
	static const DxEPG_Type_e	s_table[] = {
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME)
#ifdef	META_QUERY_MERGE_PF_TO_IP
		eDxEPG_TYPE_IP, eDxEPG_TYPE_PF, eDxEPG_TYPE_SC
#else
		eDxEPG_TYPE_PF, eDxEPG_TYPE_IP, eDxEPG_TYPE_SC
#endif

#else
#ifdef	META_QUERY_MERGE_PF_TO_IP
		eDxEPG_TYPE_IP, eDxEPG_TYPE_TVTV, eDxEPG_TYPE_PF, eDxEPG_TYPE_SC
#else
		eDxEPG_TYPE_PF, eDxEPG_TYPE_IP, eDxEPG_TYPE_TVTV, eDxEPG_TYPE_SC
#endif
#endif
	};

	sd    = metaquery_GetSQLBuf();
	queue = metaquery_GetQueue();
	stack = metaquery_GetStack();

#ifdef	META_QUERY_MERGE_PF_TO_IP
	switch (search->eFilter)
	{
	case eQUERY_FILTER_PF:  type = eDxEPG_TYPE_PF; break;
	case eQUERY_FILTER_SCH: type = eDxEPG_TYPE_SC; break;
	case eQUERY_FILTER_IP: 	type = eDxEPG_TYPE_IP; 					break;
	case eQUERY_FILTER_TVTV: type = eDxEPG_TYPE_TVTV; 					break;
	case eQUERY_FILTER_RF:	type = eDxEPG_TYPE_PF | eDxEPG_TYPE_SC;	break;
	case eQUERY_FILTER_ALL:
	default:
		type = eDxEPG_TYPE_PF | eDxEPG_TYPE_SC |eDxEPG_TYPE_IP | eDxEPG_TYPE_TVTV;
		break;
	}
#else
	switch (search->eFilter)
	{
	case eQUERY_FILTER_PF:  type = eDxEPG_TYPE_PF; break;
	case eQUERY_FILTER_SCH: type = eDxEPG_TYPE_IP | eDxEPG_TYPE_TVTV | eDxEPG_TYPE_SC; break;
	case eQUERY_FILTER_ALL:
	default:
		type = eDxEPG_TYPE_IP | eDxEPG_TYPE_TVTV | eDxEPG_TYPE_PF | eDxEPG_TYPE_SC;
		break;
	}
#endif

	if (search->svcOrder)
		order = HLIB_VECTOR_First(search->svcOrder);
	else order = NULL;

	n = sizeof(s_table) / sizeof(s_table[0]);
	for (i = 0, uni = 0 ; i < n ; i++)
	{
		if (!(type & s_table[i]))
			continue;

		if (uni++ > 0)
			HLIB_RWSTREAM_Print(sd, " UNION ");
		HLIB_RWSTREAM_Print(sd, "SELECT \'%s\' AS tablename, uid, %s, link, linkdata ,name FROM %s WHERE "
			, DxEPG_NAMEOF(s_table[i]), order ? order->field : "start_time", DxEPG_NAMEOF(s_table[i]));
		if (search->svcCons)
		{
			HINT32	c = 0;
			HINT32	cons = HLIB_RWSTREAM_Open(NULL, 0);

			if (cons)
			{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
				HCHAR * tripleid = NULL;

				HLIB_RWSTREAM_Print(cons, "tripleid IN(");
				HxVECTOR_EACH(search->svcCons, HCHAR *, tripleid, {
					if (c > 0)
						HLIB_RWSTREAM_PutChar(cons, ',');
					HLIB_RWSTREAM_Print(cons, "\'%s\'", tripleid);
					c++;
				});
#else
				HINT32	uid;
				HxLOG_Debug("metaquery_MakeSQL Cons length[%d]\n", HLIB_VECTOR_Length(search->svcCons));
				HLIB_RWSTREAM_Print(cons, "svcuid IN(");
				HxVECTOR_EACH(search->svcCons, HINT32, uid, {
					if (c > 0)
						HLIB_RWSTREAM_PutChar(cons, ',');
					HLIB_RWSTREAM_Print(cons, "%d", uid);
					c++;
				});
#endif
				HLIB_RWSTREAM_Print(cons, ")");
				if (c > 0)
				{
					HLIB_RWSTREAM_Print(sd, "%s AND ", HLIB_RWSTREAM_GetBuf(cons));
				}
				HLIB_RWSTREAM_Close(cons);
			}
		}

		// Query Tree -> Pre order notation
		metaquery_Traverse (search->query, (void (*) (_Query_t *, void *))metaquery_MakeQuerySQL, (void *)queue);

		while ((query = (_Query_t *)HLIB_QUEUE_Pop(queue)) != NULL)
		{
			switch(query->type)
			{
			case '&':
				operand[1] = (HCHAR *)HLIB_STACK_Pop(stack);
				operand[0] = (HCHAR *)HLIB_STACK_Pop(stack);
				result = HLIB_STD_FmtString(NULL, 0, "(%s AND %s)", operand[0], operand[1]);
				break;

			case '|':
				operand[1] = (HCHAR *)HLIB_STACK_Pop(stack);
				operand[0] = (HCHAR *)HLIB_STACK_Pop(stack);
				result = HLIB_STD_FmtString(NULL, 0, "(%s OR %s)", operand[0], operand[1]);
				break;

			case '!':
				operand[0] = (HCHAR *)HLIB_STACK_Pop(stack);
				operand[1] = NULL;
				result = HLIB_STD_FmtString(NULL, 0, "NOT(%s)", operand[0]);
				break;

			default:
				operand[0] = operand[1] = NULL;
				result = metaquery_QueryToString(query);
				break;
			}

			if (operand[0])
			{
				APK_Free(operand[0]);
				operand[0] = NULL;
			}

			if (operand[1])
			{
				APK_Free(operand[1]);
				operand[1] = NULL;
			}

			if (result == NULL)
			{
				HxLOG_Error("%s() Out of memory!\n", __FUNCTION__);
				break;
			}
			HxStack_Push(stack, (void *)APK_StrDup(result));
			HLIB_STD_FreeFmtString(result);
		}

		if (result == NULL || HxStack_Length(stack) != 1)
		{
			while ((result = (HCHAR *)HLIB_STACK_Pop(stack)) != NULL)
			{
				APK_Free(result);
				result = NULL;
			}

			HxLOG_Error("[%s:%d] Syntax Error!!!\n", __HxFILE__, __HxLINE__);
			return NULL;
		}
		result = HLIB_STACK_Pop(stack);
		HLIB_RWSTREAM_Print(sd, "%s", result);
		APK_Free(result); // MOON_leak
	}


	if (order)
		HLIB_RWSTREAM_Print(sd, " ORDER BY %s %s ", order->field, order->ascending > 0 ? "ASC" : "DESC");
	else
		HLIB_RWSTREAM_Print(sd, " ORDER BY start_time ASC ");

	HLIB_RWSTREAM_Print(sd, "LIMIT 500");	// 500°³ Á¦ÇÑ
	return APK_StrDup(HLIB_RWSTREAM_GetBuf(sd));
}

static HINT32		metaquery_GetEventTypePriority (DxEPG_Type_e type)
{
	switch(type)
	{
	case eDxEPG_TYPE_PF:
	case eDxEPG_TYPE_PF_EX:
		return 100;

	case eDxEPG_TYPE_SC:
	case eDxEPG_TYPE_SC_EX:
		return 10;

	case eDxEPG_TYPE_TVTV:
		return 500;

	case eDxEPG_TYPE_IP:
		return 1000;

	default:
		break;
	}
	return 0;
}
#if !defined(META_QUERY_MERGE_PF_TO_IP)
static HxVector_t *	metaquery_RemoveDuplication (_QSearchManager_t *mgr, HxVector_t *list)
{
	HINT32		i, n;
	DxEvent_t	*evt;
	DxEvent_t	*dup;
	HxTREE_t	*dupTree;
	HxVector_t	*result = NULL;

	dupTree = metaquery_GetTree();
	result  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void*(*)(void*))APK_EVENT_Delete, NULL);
	_E_RETURN_IF(result == NULL, NULL);

        if ( list )
        {
        	n = HLIB_VECTOR_Length(list);
        	for (i = 0 ; i < n ; i++)
        	{
        		evt = (DxEvent_t *)HLIB_VECTOR_ItemAt(list, i);
        		dup = (DxEvent_t *)HLIB_TREE_Lookup(dupTree, (void *)metaquery_SetPaddingTime(evt, META_QUERY_PADDING));
        		metaquery_SetPaddingTime(evt, -META_QUERY_PADDING);
        		if (dup && metaquery_IsOverlapped(dup, evt))
        		{
        			HINT32	diff;
        			HxLOG_Debug("%s() event [%s:%s] & [%s:%s] duplicated!\n", __FUNCTION__
        				, DxEPG_NAMEOF(dup->type), APK_EVENT_GET_STRING(dup->name), DxEPG_NAMEOF(evt->type), evt->name);

        			// DLIB_Print(DxEPG_NAMEOF(dup->type), stdout, dup);
        			// DLIB_Print(DxEPG_NAMEOF(evt->type), stdout, evt);

        			diff = metaquery_GetEventTypePriority(dup->type) - metaquery_GetEventTypePriority(evt->type);
        			if (diff == 0)
        			{
        				DLIB_FreeType(DxEPG_NAMEOF(evt->type), evt);
        				continue;
        			}
        			else if (diff > 0)
        			{
        				DLIB_FreeType(DxEPG_NAMEOF(evt->type), evt);
        				continue;
        			}
        			else //if (diff < 0)
        			{
        				HLIB_VECTOR_Detach(result, HLIB_VECTOR_IndexOf(result, dup));
        				HLIB_TREE_Remove(dupTree, (void *)dup);
        				DLIB_FreeType(DxEPG_NAMEOF(dup->type), dup);
        			}
        		}
        		HLIB_TREE_Insert(dupTree, (void *)evt, evt);
        		HLIB_VECTOR_Add(result, (void*)evt);
        	}
        	if (HLIB_VECTOR_Length(result) == 0)
        	{
        		HLIB_VECTOR_Delete(result);
        		result = NULL;
        	}
        }
	return result;
}

#else

HBOOL dump_EventTime(void *data1)
{

	HxDATETIME_t ipStartTime, ipEndTime;
	DxEvent_t	*pEvent = (DxEvent_t	*)data1;
	if(pEvent == NULL)
		return FALSE;
	HLIB_DATETIME_ConvertUnixTimeToDateTime(pEvent->startTime, &ipStartTime);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(pEvent->startTime+pEvent->duration, &ipEndTime);
	HxLOG_Debug("dump_EventTime %s[%02d:%02d ~ %02d:%02d][%s]\n"
					,  DxEPG_NAMEOF(pEvent->type)
					,ipStartTime.stTime.ucHour, ipStartTime.stTime.ucMinute
					,ipEndTime.stTime.ucHour, ipEndTime.stTime.ucMinute
					,APK_EVENT_GET_STRING(pEvent->name));
	return FALSE;
}

static HxVector_t *	metaquery_RemoveDuplication (_QSearchManager_t *mgr, HxVector_t *list)
{
	HINT32		i, n;
	DxEvent_t	*evt;
	DxEvent_t	*dup;
	HxTREE_t	*dupTree;
	HxVector_t	*result = NULL;

	dupTree = metaquery_GetTree();
	result  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void*(*)(void*))APK_EVENT_Delete, NULL);
	_E_RETURN_IF(result == NULL, NULL);

        if ( list )
        {
        	n = HLIB_VECTOR_Length(list);
        	for (i = 0 ; i < n ; i++)
        	{
        		evt = (DxEvent_t *)HLIB_VECTOR_ItemAt(list, i);

        		do  // util without dup event
        		{
        			dup = (DxEvent_t *)HLIB_TREE_Lookup(dupTree, evt);

        			if (dup)
        			{
        				HINT32	diff;
        				HxLOG_Debug("%s() event [%s:%s] & [%s:%s] duplicated!\n", __FUNCTION__
        					, DxEPG_NAMEOF(dup->type), APK_EVENT_GET_STRING(dup->name), DxEPG_NAMEOF(evt->type), APK_EVENT_GET_STRING(evt->name));

        				//dump_EventTime(dup);
        				//dump_EventTime(evt);
        				// DLIB_Print(DxEPG_NAMEOF(dup->type), stdout, dup);
        				// DLIB_Print(DxEPG_NAMEOF(evt->type), stdout, evt);

        				diff = metaquery_GetEventTypePriority(dup->type) - metaquery_GetEventTypePriority(evt->type);

        				if(metaquery_IsOverlapped (dup, evt) == TRUE)
        				{
        					if(dup->type == eDxEPG_TYPE_PF && evt->type == eDxEPG_TYPE_IP)
        						evt->parentalRating = dup->parentalRating;
        					else if(dup->type == eDxEPG_TYPE_PF && evt->type == eDxEPG_TYPE_TVTV)
        						evt->parentalRating = dup->parentalRating;
        					else
        						dup->parentalRating = evt->parentalRating;
        					HxLOG_Debug("SAME event diff type\n\n");
        				}

         				if (diff >= 0)	// °°Àº Á¾·ùÀÇ eventÀÇ °æ¿ì »èÁ¦ ÇÑ´Ù.
        				{
         					HxLOG_Debug("Result Duf Main\n");
        					DLIB_FreeType(DxEPG_NAMEOF(evt->type), evt);
        					evt = NULL;
        					break;
        				}
        				else
        				{
        					HLIB_VECTOR_Detach(result, HLIB_VECTOR_IndexOf(result, dup));
        					HLIB_TREE_Remove(dupTree, (void *)dup);
         					HxLOG_Debug("Result Duf Sub\n");
        					DLIB_FreeType(DxEPG_NAMEOF(dup->type), dup);
        				}
         			}
        		}while(dup != NULL);

        		if(evt != NULL)
        		{
        			HLIB_TREE_Insert(dupTree, (void *)evt, evt);
        			HLIB_VECTOR_Add(result, (void*)evt);
        		}

        	}
        	if (HLIB_VECTOR_Length(result) == 0)
        	{
        		HLIB_VECTOR_Delete(result);
        		result = NULL;
        	}
        }
	return result;
}
#endif

static HxVector_t	*metaquery_MakeResult (const void *data, HUINT32 size)
{
	HxVector_t		*list;
	DxEvent_t		*event;
	DAxIterator_t	iter;

	if (data && size)
	{
		list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void*(*)(void*))APK_EVENT_Delete, NULL);
		if (list)
		{
			DAPI_InitIterator(&iter, data, size, 0);
			while (DAPI_NextIterator(&iter))
			{
				event = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
				if (event)
				{
					HLIB_VECTOR_Add(list, (void *)event);
				}
			}
			HxVECTOR_DELETE_IF_EMPTY(list);
		}
	} else list = NULL;
	return list;
}

static void		metaquery_OnSearchResultReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		iter;
	HINT32				i, n;
	_QSearchManager_t	*mgr;
	HxVector_t			*result;
	DxEvent_t			*event;
	_QSearch_t			*search = NULL;
	APK_META_QUERY_Notifier_t	fnNotifier;
	void				*listenud = NULL;

	HxLOG_Trace();
	HxLOG_Debug("%s(name:%s, uid:"HUID_FMT", data:0x%X, size:%u, qid:%d) time:%s(%u)\n", __FUNCTION__, name, uid, data, size, (int)userdata, HLIB_DATETIME_UnixTimeToString(HLIB_STD_GetSystemTime(), NULL, 0), HLIB_STD_GetSystemTick());


	METAQ_LOCK;
	mgr = metaquery_GetManager();
	n   = HLIB_VECTOR_Length(mgr->list);
	for (i = 0 ; i < n ; i++)
	{
		search = (_QSearch_t *)HLIB_VECTOR_ItemAt(mgr->list, i);
		if (search->qid == (HINT32)userdata)
			break;
	}
	if (i == n)
	{
		HxLOG_Debug("%s(qid:%d) Cannot find search instance! maybe aborted...\n", __FUNCTION__, (int)userdata);
		METAQ_UNLOCK;
		return;
	}
	fnNotifier = search->fnNotifier;
	listenud = search->userdata;
	if (search->result)
	{
		HLIB_VECTOR_Delete(search->result);
	}
	search->result = NULL;

	if (size == 0)
	{
		METAQ_UNLOCK;
		if (fnNotifier)
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			stApkEvtArgs.pfnFreeFunc = NULL;
			stApkEvtArgs.ulArgCount = 5;
			stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
			stApkEvtArgs.apArgV[1] = (void *)search;
			stApkEvtArgs.apArgV[2] = (void *)eQUERY_EVENT_FINISHED;
			stApkEvtArgs.apArgV[3] = (void *)NULL;
			stApkEvtArgs.apArgV[4] = (void *)listenud;

			APK_EVENT_Emit(&stApkEvtArgs);
		}
		return;
	}

	result = metaquery_GetVector();

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		HxLOG_Debug("\t<%d> %s[%08X], 0x%X(%u)\n", iter.iteration, iter.name, iter.uid, iter.data, iter.size);
		event = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (event)
		{
			HLIB_VECTOR_Add(result, (void *)event);
			HxLOG_Debug("\t>>> %d.%d.%d.%d.%d.%d.%X.%s.%s\n", event->type, event->refCount, event->svcuid, event->onid, event->tsid, event->svcid, event->eventId, APK_EVENT_GET_STRING(event->name), event->text);
		}
	}

	search->result = metaquery_RemoveDuplication(mgr, result);

	HxLOG_Print("%s() time:%s(%u), build done! count:%d\n", __FUNCTION__, HLIB_DATETIME_UnixTimeToString(HLIB_STD_GetSystemTime(), NULL, 0), HLIB_STD_GetSystemTick(), HLIB_VECTOR_Length(result));
	HxLOG_Print("[01;35m Real Count:%d [00;39;49m\n", (search->result) ? HLIB_VECTOR_Length(search->result) : 0 );


	METAQ_UNLOCK;

	if (fnNotifier)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 5;
		stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
		stApkEvtArgs.apArgV[1] = (void *)search;
		stApkEvtArgs.apArgV[2] = (void *)eQUERY_EVENT_FINISHED;
		stApkEvtArgs.apArgV[3] = (void *)NULL;
		stApkEvtArgs.apArgV[4] = (void *)listenud;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}



static void	metaquery_OnDataReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	HxVector_t		*list;
	DxEvent_t		*event;
	_QResult_t		*result;

	HxLOG_Trace();

	result = (_QResult_t *)userdata;
	list   = metaquery_MakeResult(data, size);
	if (list == NULL)
	{
		if (result->callback)
		{
			result->callback(ERR_FAIL, result->userdata);
			APK_Free(result);
			return;
		}
		result->u.result = NULL;
		return;
	}

	switch(result->option)
	{
	case 1:
		event = (DxEvent_t *)HLIB_VECTOR_First(list);
		APK_EVENT_Incref(event);
		HLIB_VECTOR_Delete(list);
		result->u.event = event;
		break;

	default:
		result->u.list = list;
		break;
	}

	if (result->callback)
	{
		result->callback(ERR_OK, result->userdata, result->u.result);
		APK_Free(result);
	}
}

static void	metaquery_OnEpgRfUpdated (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
#define	__QSIZE	32
	HINT32				i, n, j, c;
	_QSearch_t			*search;
	_QSearchManager_t	*mgr;
	const HUID 			*uidlist;
	struct updateQ {
		_QSearch_t			*search;
		APK_META_QUERY_Notifier_t	fnNotifier;
		void 				*userdata;
	} updateQ[__QSIZE];
	HINT32				length;
	HUID				evtuid;
	HxHASH_t		*hash;
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	HCHAR				* triple = NULL;
#else
	HINT32				svcuid;
#endif
	APK_META_QUERY_Filter_e	eFilter;
	APK_EVENT_EMIT_ARGS	stApkEvtArgs;

	HxLOG_Debug("%s(%s, uid:"HUID_FMT", data:0x%X, size:%u, userdata:0x%X\n", __FUNCTION__, name, uid, data, size, userdata);

	if (size == 0)
		return;

	if (HxSTD_StrCmp(name, DxNAMEOF(eDxEPG_TYPE_PF)) == 0)
		eFilter = eQUERY_FILTER_PF;
	else
		eFilter = eQUERY_FILTER_SCH;

	METAQ_LOCK;
	hash    = metaquery_GetHash();
	uidlist = (const HUID *)data;
	for (c = (HINT32)(size / sizeof(HUID)) ; c-- ; uidlist++)
	{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
		HCHAR tripleBuf[32]="";
		evtuid = *uidlist;
		HxSTD_PrintToStrN(tripleBuf, 32, "%x.%x.%x", DxEVENT_UID_ONID(evtuid), DxEVENT_UID_TSID(evtuid), DxEVENT_UID_SVCID(evtuid));

		if(HLIB_HASH_Lookup(hash, (void *)tripleBuf) == NULL)
		{
			HxLOG_Debug("tripleBuf ==> update triple data [%s]\n", tripleBuf);
			HLIB_HASH_Insert(hash, (void *)APK_StrDup(tripleBuf), (void *)tripleBuf);
		}
#else
		evtuid = *uidlist;
		svcuid = ((HUID)evtuid) >> 32;

		HLIB_HASH_Insert(hash, (void *)svcuid, (void *)svcuid);
#endif
	}

#ifdef	CONFIG_DEBUG
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
{
	DxService_t			*svc;
	HxHASH_TableIter_t	iter;

	HLIB_HASH_IterInit(&iter, hash);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&svcuid))
	{
		svc = APK_META_SVC_GetService(svcuid);
		if (svc)
		{
			HxLOG_Debug(HxANSI_COLOR_GREEN("\t%s UPDATED in %d.%s(uid:%d)\n"), name, svc->lcn, svc->name, svc->uid);
			APK_META_SVC_Delete(svc);
		}
	}
}
#endif
#endif

	mgr  = metaquery_GetManager();
	n    = HLIB_VECTOR_Length(mgr->list);
	for (i = 0, length = 0 ; i < n && length < __QSIZE ; i++)
	{
		search  = (_QSearch_t *)HLIB_VECTOR_ItemAt(mgr->list, i);
		if (search == NULL || search->svcCons == NULL)
			continue;

		if (search->eFilter != eQUERY_FILTER_ALL && search->eFilter != eFilter)
			continue;

		c = HLIB_VECTOR_Length(search->svcCons);
		for (j = 0 ; j < c ; j++)
		{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
			triple = (HCHAR *)HLIB_VECTOR_ItemAt(search->svcCons, j);
			HxLOG_Debug("svcCons ==> update triple data [%s]\n", triple);
			if (HLIB_HASH_Lookup(hash, (void *)triple))
			{
				HxLOG_Debug("=============> real update triple data [%s]\n", triple);
				if (search->fnNotifier)
				{
					updateQ[length].search   = search;
					updateQ[length].fnNotifier = search->fnNotifier;
					updateQ[length].userdata = search->userdata;
					length++;
				}
				break;
			}
#else
			svcuid = (HINT32)HLIB_VECTOR_ItemAt(search->svcCons, j);
			if (HLIB_HASH_Lookup(hash, (void *)svcuid))
			{
				if (search->fnNotifier)
				{
					updateQ[length].search   = search;
					updateQ[length].fnNotifier = search->fnNotifier;
					updateQ[length].userdata = search->userdata;
					length++;
				}
				break;
			}
#endif
		}
	}
	METAQ_UNLOCK;

	for (i = 0 ; i < length ; i++)
	{
		HxLOG_Debug("%s(%s,...): updated(%X)\n", __FUNCTION__, name, (int)updateQ[i].search);

		HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 5;
		stApkEvtArgs.apArgV[0] = (void *)updateQ[i].fnNotifier;
		stApkEvtArgs.apArgV[1] = (void *)updateQ[i].search;
		stApkEvtArgs.apArgV[2] = (void *)eQUERY_EVENT_NEW_VERSION;
		stApkEvtArgs.apArgV[3] = (void *)NULL;
		stApkEvtArgs.apArgV[4] = (void *)updateQ[i].userdata;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

static void	metaquery_OnEventDataReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	_EventNameId_t		*pEventNameId = (_EventNameId_t *)userdata;
	DxEvent_t			*pstTempEvent = NULL;
	DAxIterator_t		iter;
	HUINT32 			duration = 0;

	HxLOG_Trace();

	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		return;
	}

	// find next item.
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstTempEvent = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (pstTempEvent)
		{
			if (pstTempEvent->name && pstTempEvent->duration > duration)
			{
				HxLOG_Debug("name = %s, duration = %d\n", APK_EVENT_GET_STRING(pstTempEvent->name), pstTempEvent->duration);
				HxSTD_MemCopy(pEventNameId->szName, APK_EVENT_GET_STRING(pstTempEvent->name), 256);
				pEventNameId->eventId = pstTempEvent->eventId;
				duration = pstTempEvent->duration;
			}
			DLIB_FreeType(iter.name, pstTempEvent);
		}
	}
}

#define	____PUBLIC_API______________________________________________________________________________
void	debug_DxEvent (FILE *fp, DxEvent_t *event, const HCHAR *msg)
{
	HINT32				i;

	fprintf(fp,
		"\nDxEvent_t(0x%X) refCount:%d, MSG[%s]\n"
		"\ttype:%s\n"
		"\tsvcuid:%d\n"
		"\teventId:0x%X\n"
		"\tstartTime:%s, duration:%usec\n"
		"\trunning status:0x%X, freeCAmode:%d\n"
		"\tname:\'%s\'\n"
		"\ttext:\'%s\'\n"
		"\tlongText:\'%s\'\n"
		"\tparantal rating:%d\n"
		, (int)event, event->refCount, msg
		, DxEPG_NAMEOF(event->type)
		, event->svcuid
		, event->eventId
		, HLIB_DATETIME_UnixTimeToString((UNIXTIME)event->startTime, NULL, 0), event->duration
		, event->runStatus, event->freeCaMode
		, APK_EVENT_GET_STRING(event->name)
		, APK_EVENT_GET_STRING(event->text)
		, APK_EVENT_GET_STRING(event->longText)
		, event->parentalRating
	);
	fprintf(fp, "\tcontent:");
	for (i = 0 ; i < DxEVENT_CONTENT ; i++)
		fprintf(fp, " %04X", event->content[i]);
	fprintf(fp, "\n");

	if (event->item)
	{
		DxEventItem_t *item;
		fprintf(fp, "\titem(0x%X):", (int)event->item);
		HxVECTOR_EACH(event->item, DxEventItem_t *, item, {
			fprintf(fp, "\t\t%s[%s]\n", item->description, item->item);
		});
		fprintf(fp, "\n");
	}

	if (event->linkage)
	{
		DxEventLinkage_t *item;
		fprintf(fp, "\tlinkage(0x%X):", (int)event->linkage);
		HxVECTOR_EACH(event->linkage, DxEventLinkage_t *, item, {
			fprintf(fp, "\t\t%d.%d.%d type:%d\n", item->onid, item->tsid, item->svcid, item->linkageType);
		});
		fprintf(fp, "\n");
	}

	if (event->component)
	{
		DxEventComponent_t *item;
		fprintf(fp, "\tcomponent(0x%X):", (int)event->component);
		HxVECTOR_EACH(event->component, DxEventComponent_t *, item, {
			fprintf(fp, "\t\tcontent:%d, type:%d, tag:%d, text:%s\n", item->content, item->type, item->tag, item->text);
		});
		fprintf(fp, "\n");
	}

	if (event->descriptor)
	{
		const HUINT8 *item;
		fprintf(fp, "\tdescriptor(0x%X):", (int)event->descriptor);
		HxVECTOR_EACH(event->descriptor, const HUINT8 *, item, {
			fprintf(fp, "\t\ttag:0x%X, length:%u\n", item[0], item[1]);
		});
		fprintf(fp, "\n");
	}


}

DxEvent_t *	APK_EVENT_Incref (DxEvent_t *event)
{
	if (event)
	{
		event->refCount++;
	}
	return event;
}

void			APK_EVENT_Delete (DxEvent_t *event)
{
	if (event && --event->refCount <= 0)
	{
		DLIB_FreeType(DxEPG_NAMEOF(event->type), event);
	}
}


DxEvent_t *	APK_EVENT_Clone (DxEvent_t *e)
{
	if (e == NULL)
		return NULL;

	return DLIB_NewType(DxEPG_NAMEOF(e->type), DxEPG_CLONE_UID, e, sizeof(DxEvent_t));
}

APK_QUERY_t		APK_META_QUERY_NewQuery (const HCHAR *field, const HCHAR *comparison, const HCHAR *value)
{
	HxLOG_Trace();
	HxLOG_Assert(field && comparison);
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	if (HLIB_STD_StrCaseCmp(field, "programmeID") == 0)
	{
		return (APK_QUERY_t)metaquery_AllocProgrammeIdQ(field, comparison, value);
	}
#endif
	return (APK_QUERY_t)metaquery_AllocQ(field, comparison, value);
}

void			APK_META_QUERY_DeleteQuery (APK_QUERY_t q)
{
	HxLOG_Trace();

	(void)metaquery_ReleaseQ((_Query_t *)q);
}

APK_QUERY_t		APK_META_QUERY_CloneQuery (const APK_QUERY_t q)
{
	HxLOG_Trace();

	return metaquery_CloneQ((const _Query_t *)q);
}

APK_QUERY_t		APK_META_QUERY_And (const APK_QUERY_t q1, const APK_QUERY_t q2)
{
	_Query_t *q;

	HxLOG_Trace();
	HxLOG_Assert(q1 && q2);

	q = metaquery_AllocQ(NULL, NULL, NULL);
	if (q)
	{
		q->type = '&';
		q->childs[0] = metaquery_CloneQ((_Query_t *)q1);
		q->childs[1] = metaquery_CloneQ((_Query_t *)q2);
	}
	return (APK_QUERY_t)q;
}

APK_QUERY_t		APK_META_QUERY_Or (const APK_QUERY_t q1, const APK_QUERY_t q2)
{
	_Query_t *q;

	HxLOG_Trace();
	HxLOG_Assert(q1 && q2);

	q = metaquery_AllocQ(NULL, NULL, NULL);
	if (q)
	{
		q->type = '|';
		q->childs[0] = metaquery_CloneQ((_Query_t *)q1);
		q->childs[1] = metaquery_CloneQ((_Query_t *)q2);
	}
	return (APK_QUERY_t)q;
}

APK_QUERY_t		APK_META_QUERY_Not (const APK_QUERY_t q1)
{
	_Query_t *q;

	HxLOG_Trace();
	HxLOG_Assert(q1);

	q = metaquery_AllocQ(NULL, NULL, NULL);
	if (q)
	{
		q->type = '!';
		q->childs[0] = metaquery_CloneQ((_Query_t *)q1);
		q->childs[1] = NULL;
	}
	return (APK_QUERY_t)q;
}


ApkMetaQHandle_t		APK_META_QUERY_New (APK_META_QUERY_Target_e eTarget)
{
	_QSearch_t			*self;
	_QSearchManager_t	*mgr;

	HxLOG_Trace();

	self = (_QSearch_t *)APK_Calloc(sizeof(_QSearch_t));
	_E_RETURN_IF(self == NULL, NULL);

	METAQ_LOCK;
	mgr  = metaquery_GetManager();
	HLIB_VECTOR_Add(mgr->list, (void *)self);

	self->qid    = metaquery_NextQID();
	self->eTarget = eTarget;
	METAQ_UNLOCK;

	return (ApkMetaQHandle_t)self;
}



void			APK_META_QUERY_Delete (ApkMetaQHandle_t self)
{
	_QSearch_t			*thiz;
	_QSearchManager_t	*mgr;
	HINT32				index;

	HxLOG_Trace();

	APK_META_QUERY_Abort(self);

	METAQ_LOCK;

	thiz = QSEARCH(self);
	if (thiz == NULL)
	{
		METAQ_UNLOCK;
		return;
	}
	mgr   = metaquery_GetManager();
	index = HLIB_VECTOR_IndexOf(mgr->list, (void *)thiz);
	if (index < 0)
	{
		METAQ_UNLOCK;
		return;
	}
	HLIB_VECTOR_Remove(mgr->list, index);
	METAQ_UNLOCK;
}


HERROR			APK_META_QUERY_SetQuery (ApkMetaQHandle_t self, const APK_QUERY_t query)
{
	_QSearch_t	*thiz;

	HxLOG_Trace();

	METAQ_LOCK;

	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	if (thiz->query)
		metaquery_ReleaseQ(thiz->query);

	thiz->query = metaquery_CloneQ((const _Query_t *)query);

#if defined(CONFIG_DEBUG) && 0	// Dump SetQuery!
	metaquery_Traverse(thiz->query, metaquery_DumpQuery, NULL);
#endif

	METAQ_UNLOCK;
	return ERR_OK;
}

APK_QUERY_t		APK_META_QUERY_GetQuery (ApkMetaQHandle_t self)
{
	_QSearch_t	*thiz;
	_Query_t	*query;

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	query = metaquery_CloneQ(thiz->query);
	METAQ_UNLOCK;

	return (APK_QUERY_t)query;
}

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
HERROR			APK_META_QUERY_AddSvcConstraint (ApkMetaQHandle_t self, HINT32 onid, HINT32 tsid, HINT32 svcid)
{
	_QSearch_t	*thiz;
	HCHAR	buf[32];

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	if (thiz->svcCons == NULL)
	{

		thiz->svcCons = HLIB_VECTOR_NewEx(NULL, 0, NULL, metaquery_FreeSvcCon, NULL);
		if (thiz->svcCons == NULL)
		{
			HxLOG_Critical("[%s:%d] Out of memory!!\n", __HxFILE__, __HxLINE__);
			METAQ_UNLOCK;
			return ERR_FAIL;
		}
	}

	HxSTD_PrintToStrN(buf, 32, "%x.%x.%x", onid, tsid, svcid);
	(void) HLIB_VECTOR_Add(thiz->svcCons,APK_StrDup(buf));
	METAQ_UNLOCK;
	return ERR_OK;
}
#else

HERROR			APK_META_QUERY_AddSvcConstraint (ApkMetaQHandle_t self, HINT32 svcuid)
{
	_QSearch_t	*thiz;

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	if (svcuid == 0)
	{
		HxLOG_Warning("[%s:%d] Invalid parameter!!\n", __HxFILE__, __HxLINE__);
		METAQ_UNLOCK;
		return ERR_FAIL;
	}

	if (thiz->svcCons == NULL)
	{
		thiz->svcCons = HLIB_VECTOR_New(NULL, NULL);
		if (thiz->svcCons == NULL)
		{
			HxLOG_Critical("[%s:%d] Out of memory!!\n", __HxFILE__, __HxLINE__);
			METAQ_UNLOCK;
			return ERR_FAIL;
		}
	}
	HxLOG_Debug("APK_META_QUERY_AddSvcConstraint[%d]\n", svcuid);
	if (HLIB_VECTOR_IndexOf(thiz->svcCons, (void *)svcuid) < 0)
		HLIB_VECTOR_Add(thiz->svcCons, (void *)svcuid);

	METAQ_UNLOCK;
	return ERR_OK;
}
#endif

void			APK_META_QUERY_ClearSvcConstraint (ApkMetaQHandle_t self)
{
	_QSearch_t	*thiz;

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	if (thiz->svcCons)
	{
		HLIB_VECTOR_RemoveAll(thiz->svcCons);
	}
	METAQ_UNLOCK;
}


void		APK_META_QUERY_OrderBy (ApkMetaQHandle_t self, const HCHAR *field, HBOOL ascending)
{
	_QSearch_t	*thiz;
	_Order_t	order;

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	if (field == NULL)	// if NULL, Any currently-set order SHALL be cleared
	{
		if (thiz->svcOrder)
			HLIB_VECTOR_RemoveAll(thiz->svcOrder);
		METAQ_UNLOCK;
		return;
	}

	if (thiz->svcOrder == NULL)
	{
		thiz->svcOrder = HLIB_VECTOR_NewEx(NULL, 0, NULL, metaquery_FreeOrder, NULL);
		if (thiz->svcOrder == NULL)
		{
			HxLOG_Error("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
			METAQ_UNLOCK;
			return;
		}
	}

	HLIB_STD_StrNCpySafe(order.field, metaquery_ValidateField(field), 64);
	order.ascending = ascending ? 1 : -1;

        if (thiz->svcOrder)
                HLIB_VECTOR_Add(thiz->svcOrder, APK_MemDup(&order, sizeof(_Order_t)));
	METAQ_UNLOCK;
}


void			APK_META_QUERY_SetFilter (ApkMetaQHandle_t self, APK_META_QUERY_Filter_e eFilter)
{
	_QSearch_t	*thiz;

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	thiz->eFilter = eFilter;
	METAQ_UNLOCK;
}

void			APK_META_QUERY_SetNotifier (ApkMetaQHandle_t self, APK_META_QUERY_Notifier_t fnNotifier, void *userdata)
{
	_QSearch_t	*thiz;

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	thiz->fnNotifier = fnNotifier;
	thiz->userdata = userdata;

	METAQ_UNLOCK;
}

HERROR			APK_META_QUERY_Search (ApkMetaQHandle_t self, HBOOL direct)
{
	_QSearch_t	*thiz;
	HINT32		qid;
	HCHAR		*query;
	HUINT32		rid;

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	qid   = thiz->qid;
	query = metaquery_MakeSQL(thiz);
	thiz->requestTime = HLIB_STD_GetSystemTick();

	if (thiz->sql)
	{
		APK_Free(thiz->sql);
		thiz->sql = NULL;
	}

	thiz->sql = query;
	METAQ_UNLOCK;

	if (query)
	{
		HxLOG_Debug("%s(qid:%d, %s) time:%s(%u)\n", __FUNCTION__, qid, query, HLIB_DATETIME_UnixTimeToString(HLIB_STD_GetSystemTime(), NULL, 0), HLIB_STD_GetSystemTick());

		rid = DAPI_Query(query, metaquery_OnSearchResultReceived, (void *)qid);
	} else rid = 0;

	if (rid && direct)
		return DAPI_Wait(10000, rid) == DERR_OK ? ERR_OK : ERR_FAIL;
	return rid ? ERR_OK : ERR_FAIL;
}

HERROR			APK_META_QUERY_Abort (ApkMetaQHandle_t self)
{
	_QSearch_t	*thiz;

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	thiz->qid = metaquery_NextQID();
	if (thiz->query)
	{
		metaquery_ReleaseQ(thiz->query);
		thiz->query = NULL;
	}
	if (thiz->svcCons)
		HLIB_VECTOR_RemoveAll(thiz->svcCons);
	if (thiz->svcOrder)
		HLIB_VECTOR_RemoveAll(thiz->svcOrder);
	if (thiz->sql)
	{
		APK_Free(thiz->sql);
		thiz->sql = NULL;
	}
	if (thiz->result)
	{
		HLIB_VECTOR_Delete(thiz->result);
		thiz->result = NULL;
	}
	METAQ_UNLOCK;
	return ERR_OK;
}


HxVector_t *	APK_META_QUERY_Result (ApkMetaQHandle_t self)
{
	_QSearch_t	*thiz;
	HxVector_t	*clone;

	HxLOG_Trace();

	METAQ_LOCK;
	thiz = QSEARCH(self);
	HxLOG_Assert(thiz);

	if (thiz->result)
	{
		clone = HLIB_VECTOR_CloneEx(thiz->result, (void *(*)(void *))APK_EVENT_Clone);
	}
	else
	{
		clone = NULL;
	}
	METAQ_UNLOCK;

	return clone;
}

HERROR APK_META_QUERY_SetIpEvent (HUID uid, void * data, HINT32 size)
{
	return DAPI_LazySet(DxNAMEOF(eDxEPG_TYPE_IP), uid, data, size, 100);
}

HERROR APK_META_QUERY_ResetIpEvent (HUID uid)
{
	return DAPI_Reset(DxNAMEOF(eDxEPG_TYPE_IP), uid);
}

HERROR APK_META_QUERY_ResetIpEventTable (void)
{
	return DAPI_ResetTable(DxNAMEOF(eDxEPG_TYPE_IP));
}

HERROR APK_META_QUERY_SyncIpEventTable (void)
{
	return DAPI_Sync(DxNAMEOF(eDxEPG_TYPE_IP));
}


HERROR APK_META_QUERY_DeletIpEventByEndTime(HUINT32 endTime)
{
	HINT32		sd;

	if(endTime == 0)
		return ERR_FAIL;

	METAQ_LOCK;
	sd = metaquery_GetSQLBuf();
	HLIB_RWSTREAM_Print(sd
			, "DELETE FROM %s WHERE end_time<=%d"
			, DxNAMEOF(eDxEPG_TYPE_IP), endTime);
	DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), NULL, NULL);
	METAQ_UNLOCK;
	return ERR_OK;
}


HERROR APK_META_QUERY_SetIpSvc (HUID uid, void * data, HINT32 size)
{
	DAPI_Set(DxNAMEOF(DxIpService_t), uid, data, size);
	DAPI_Sync(DxNAMEOF(DxIpService_t));
	return ERR_OK;
}

HERROR APK_META_QUERY_ResetIpSvc (HUID uid)
{
	return DAPI_Reset(DxNAMEOF(DxIpService_t), uid);
}

HERROR APK_META_QUERY_ResetIpSvcTable (void)
{
	return DAPI_ResetTable(DxNAMEOF(DxIpService_t));
}

HERROR APK_META_QUERY_SyncIpSvcTable (void)
{
	return DAPI_Sync(DxNAMEOF(DxIpService_t));
}


HERROR APK_META_QUERY_SetIpLogo (HUID uid, void * data, HINT32 size)
{
	return DAPI_LazySet(DxNAMEOF(DxChannelLogo_t), uid, data, size, 100);
}

HERROR APK_META_QUERY_ResetIpLogo (HUID uid)
{
	return DAPI_Reset(DxNAMEOF(DxChannelLogo_t), uid);
}

HERROR APK_META_QUERY_ResetIpLogoTable (void)
{
	return DAPI_ResetTable(DxNAMEOF(DxChannelLogo_t));
}

HERROR APK_META_QUERY_SyncIpLogoTable (void)
{
	return DAPI_Sync(DxNAMEOF(DxChannelLogo_t));
}

//////////////////////////////////////////////////////////////
HERROR APK_META_QUERY_ResetTVTVEventTable (void)
{
	return DAPI_ResetTable(DxNAMEOF(eDxEPG_TYPE_TVTV));
}

HERROR APK_META_QUERY_ResetTVTVSvcTable (void)
{
	return DAPI_ResetTable(DxNAMEOF(DxTvtvService_t));
}

HERROR APK_META_QUERY_SyncTVTVSvcTable (void)
{
	return DAPI_Sync(DxNAMEOF(DxTvtvService_t));
}

HERROR APK_META_QUERY_ResetFvcExpiryTable (void)
{
#if defined(CONFIG_OP_UK_DTT)
	return DAPI_ResetTable(DxNAMEOF(DxFvcExpiry_t));
#else
	return ERR_FAIL;
#endif
}

HERROR APK_META_QUERY_SyncFvcExpiryTable (void)
{
#if defined(CONFIG_OP_UK_DTT)
	return DAPI_Sync(DxNAMEOF(DxFvcExpiry_t));
#else
	return ERR_FAIL;
#endif
}


//////////////////////////////////////////////////////////////

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
DxEvent_t *	APK_META_QUERY_GetEvent (DxEPG_Type_e types, HINT32 onid, HINT32 tsid, HINT32 svcid, HINT32 eventid, HxReturnFunc_t ret, void *userdata)
{
	HUINT32			reqId;
	_QResult_t		result;
	_QResult_t		*tempresult = NULL;
	HINT32			i, n;
	HINT32			sd;
	static const DxEPG_Type_e	s_table[] = {
		eDxEPG_TYPE_IP, eDxEPG_TYPE_TVTV, eDxEPG_TYPE_PF, eDxEPG_TYPE_SC
	};
	HCHAR	buf[32];
	HxSTD_PrintToStrN(buf, 32, "%x.%x.%x"
					, onid
					, tsid
					, svcid
				);

	HxLOG_Trace();

	METAQ_LOCK;
	sd = metaquery_GetSQLBuf();
	n  = sizeof(s_table) / sizeof(s_table[0]);

	for (i = 0 ; i < n ; i++)
	{
		if (!(s_table[i] & types))
			continue;

		if (i > 0)
			HLIB_RWSTREAM_Print(sd, " UNION ");
		HLIB_RWSTREAM_Print(sd
			, "SELECT \'%s\' AS tablename, uid, link, linkdata FROM %s WHERE tripleid=\'%s\' & eventid=%lld"
			, DxEPG_NAMEOF(s_table[i]), DxEPG_NAMEOF(s_table[i]), buf, eventid
		);
	}
	if (HLIB_RWSTREAM_GetOffset(sd) == 0)
	{
		METAQ_UNLOCK;
		return NULL;
	}

	memset(&result, 0, sizeof(_QResult_t));
	result.option = 1;

	if (ret)
	{
		result.callback = ret;
		result.userdata = userdata;
		tempresult = APK_MemDup(&result, sizeof(_QResult_t));
		reqId = DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), metaquery_OnDataReceived, tempresult);
		if (reqId == 0)
		{
			if(tempresult)
				APK_Free(tempresult);
		}
		METAQ_UNLOCK;
		return NULL;
	}

	reqId = DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), metaquery_OnDataReceived, (void *)&result);
	METAQ_UNLOCK;

	if (reqId == 0 || DAPI_Wait(5000, reqId) == DERR_TIMEOUT)
	{
		return NULL;
	}
	return result.u.event;
}

DxEvent_t *	APK_META_QUERY_GetEventWithIds (HINT32 onid, HINT32 tsid, HINT32 svcid, HINT32 eventid)
{
	return APK_META_QUERY_GetEvent(eDxEPG_TYPE_ALL, onid, tsid, svcid, eventid, NULL, NULL);
}


DxEvent_t *	APK_META_QUERY_GetIpEvent (HINT32 onid, HINT32 tsid, HINT32 svcid, HINT32 eventid)
{
	// TODO: ÀÌ ÇÔ¼ö Áö¿ö¾ßÁö...
	return APK_META_QUERY_GetEvent(eDxEPG_TYPE_IP, onid, tsid, svcid, eventid, NULL, NULL);
}

HxVector_t * APK_META_QUERY_GetPF (HINT32 onid, HINT32 tsid, HINT32 svcid)
{
	_QResult_t	result;
	HUINT32		id;
	HCHAR		query[256];
	HCHAR	buf[32];
	HxSTD_PrintToStrN(buf, 32, "%x.%x.%x"
					, onid
					, tsid
					, svcid
				);
	HxLOG_Trace();

	HxSTD_PrintToStrN(query, 256
		, "SELECT * FROM %s WHERE tripleid=\'%s\' ORDER BY start_time ASC"
		, DxNAMEOF(eDxEPG_TYPE_PF)
		, buf
	);

	memset(&result, 0, sizeof(_QResult_t));
	HxLOG_Debug("%s query[%s]\n",__FUNCTION__, query);
	id = DAPI_Query(query, metaquery_OnDataReceived, (void *)&result);
	if (id == 0 || DAPI_Wait(10000, id) == DERR_TIMEOUT)
		return NULL;

	return result.u.list;
}

#else

HERROR APK_META_QUERY_FindEventName(HUINT32 svcuid, HUINT32 startTime, HUINT32 endTime, HCHAR *name, HINT32 eventId)
{
	HUINT32 		reqId;
	HINT32			i, n;
	HINT32			sd;
	_EventNameId_t	stEventNameId = {0,};
	HCHAR           *buf;

	static const DxEPG_Type_e	s_table[] = {
		eDxEPG_TYPE_PF, eDxEPG_TYPE_IP, eDxEPG_TYPE_SC
	};

	n  = sizeof(s_table) / sizeof(s_table[0]);
	sd = metaquery_GetSQLBuf();

	HxLOG_Debug("svcuid = %d, startTime = %d, endTime = %d\n", svcuid, startTime, endTime);

	for (i = 0 ; i < n ; i++)
	{
		if (i > 0)
			HLIB_RWSTREAM_Print(sd, " UNION ");

			HLIB_RWSTREAM_Print(sd, "SELECT \'%s\' AS tablename, uid, link, linkdata FROM ",
				DxEPG_NAMEOF(s_table[i]));
			HLIB_RWSTREAM_Print(sd, "(SELECT * FROM (SELECT uid, link, linkdata, MAX(start_time, %d) AS s, MIN(end_time, %d) AS e ",
				DxEPG_NAMEOF(s_table[i]),startTime,endTime);
			HLIB_RWSTREAM_Print(sd, "FROM %s WHERE svcuid = %d AND end_time >= %d AND start_time <= %d ORDER BY (e - s) DESC) LIMIT 1)",
				DxEPG_NAMEOF(s_table[i]), svcuid, startTime, endTime);

			// HLIB_RWSTREAM_Print(sd, "SELECT \'%s\' AS tablename, uid, link, linkdata FROM %s WHERE svcuid = %d AND end_time >= %d AND start_time <= %d"
		}


	if (HLIB_RWSTREAM_GetOffset(sd) == 0)
	{
		HxLOG_Error("Event name was not found: INVALID QUERYSTRING !!!!!. \n");
		return ERR_FAIL;
	}

	buf = HLIB_RWSTREAM_GetBuf(sd);
	HxLOG_Debug("query: %s \n",buf);

	reqId = DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), metaquery_OnEventDataReceived, (void *)&stEventNameId);
	if (reqId == 0 || DAPI_Wait(5000, reqId) == DERR_TIMEOUT)
	{
		HxLOG_Error("Event name was not found: TIMEOUT !!!!!. \n");
		return ERR_FAIL;
	}

	if(HxSTD_StrLen(stEventNameId.szName) > 0)
	{
		HxLOG_Debug("Event name was found: %s. \n", stEventNameId.szName);
		HxSTD_MemCopy(name, stEventNameId.szName, 256);
		eventId = stEventNameId.eventId;
	}
	else
	{
		HxLOG_Error("Event name was not found: NO RESULTS !!!!!. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

DxEvent_t *	APK_META_QUERY_GetEvent (DxEPG_Type_e types, HUID uid, HxReturnFunc_t ret, void *userdata)
{
	HUINT32			reqId;
	_QResult_t		result;
	HINT32			i, n;
	HINT32			sd;
	static const DxEPG_Type_e	s_table[] = {
		eDxEPG_TYPE_IP, eDxEPG_TYPE_TVTV, eDxEPG_TYPE_PF, eDxEPG_TYPE_SC
	};

	HxLOG_Trace();

	METAQ_LOCK;
	sd = metaquery_GetSQLBuf();
	n  = sizeof(s_table) / sizeof(s_table[0]);

	for (i = 0 ; i < n ; i++)
	{
		if (!(s_table[i] & types))
			continue;

		if (i > 0)
			HLIB_RWSTREAM_Print(sd, " UNION ");
		HLIB_RWSTREAM_Print(sd
			, "SELECT \'%s\' AS tablename, uid, link, linkdata FROM %s WHERE uid=%lld"
			, DxEPG_NAMEOF(s_table[i]), DxEPG_NAMEOF(s_table[i]), uid
		);
	}
	if (HLIB_RWSTREAM_GetOffset(sd) == 0)
	{
		METAQ_UNLOCK;
		return NULL;
	}

	memset(&result, 0, sizeof(_QResult_t));
	result.option = 1;

	if (ret)
	{
		result.callback = ret;
		result.userdata = userdata;
		DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), metaquery_OnDataReceived, APK_MemDup(&result, sizeof(_QResult_t)));
		METAQ_UNLOCK;
		return NULL;
	}
	reqId = DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), metaquery_OnDataReceived, (void *)&result);
	METAQ_UNLOCK;

	if (reqId == 0 || DAPI_Wait(5000, reqId) == DERR_TIMEOUT)
	{
		return NULL;
	}
	return result.u.event;
}

DxEvent_t *	APK_META_QUERY_GetEventWithIds (HINT32 svcuid, HINT32 eventid)
{
	return APK_META_QUERY_GetEvent(eDxEPG_TYPE_ALL, DxEVENT_UID(svcuid, eventid), NULL, NULL);
}


DxEvent_t *	APK_META_QUERY_GetIpEvent (HINT32 svcuid, HINT32 eventid)
{
	// TODO: ÀÌ ÇÔ¼ö Áö¿ö¾ßÁö...
	return APK_META_QUERY_GetEvent(eDxEPG_TYPE_IP, DxEVENT_UID(svcuid, eventid), NULL, NULL);
}

HxVector_t * APK_META_QUERY_GetPF (HINT32 svcuid)
{
	_QResult_t	result;
	HUINT32		id;
	HCHAR		query[256];

	HxLOG_Trace();

	HxSTD_PrintToStrN(query, 256
		, "SELECT * FROM %s WHERE svcuid=%d ORDER BY start_time ASC"
		, DxNAMEOF(eDxEPG_TYPE_PF)
		, svcuid
	);

	memset(&result, 0, sizeof(_QResult_t));
	id = DAPI_Query(query, metaquery_OnDataReceived, (void *)&result);
	if (id == 0 || DAPI_Wait(10000, id) == DERR_TIMEOUT)
		return NULL;

	return result.u.list;
}
#endif

#define	____PROTECTED_API___________________________________________________________________________

void	APK_META_QUERY_Init(HBOOL	bEnableNotifier)
{
	HxLOG_Trace();

	if (bEnableNotifier == TRUE)
	{
		DAPI_AddTableNotifier(DxNAMEOF(eDxEPG_TYPE_PF), metaquery_OnEpgRfUpdated, NULL);
		DAPI_AddTableNotifier(DxNAMEOF(eDxEPG_TYPE_SC), metaquery_OnEpgRfUpdated, NULL);
		DAPI_AddNotifier(DxNAMEOF(eDxEPG_TYPE_IP), 0, metaquery_OnEpgRfUpdated, NULL);
	}
	HxSEMT_Create(&_ulModuleSemaphoreId, "sametaquery", HxSEMT_FIFO);
}

void	APK_META_QUERY_DeInit(void)
{
	HxLOG_Trace();

	HxSEMT_Destroy(_ulModuleSemaphoreId);
}


