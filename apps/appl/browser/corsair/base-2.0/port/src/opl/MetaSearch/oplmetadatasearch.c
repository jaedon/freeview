/**************************************************************************************/
/**
 * @file oplmetadatasearch.c
 *
 * Defines the metadatasearch structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplmetadatasearch.h"
#include "oplprogramme.h"

#include <hlib.h>
#include <apk.h>





typedef struct
{
	ApkMetaQHandle_t			context;
	OPL_SearchTarget_e	target;

	HBOOL				zombie;
	HxTREE_t			*dependency;
	// TODO: rating constraint

	OPL_SearchListener_t	onSearch;

	OPL_SearchResults_t 	result;				/* async로 동작시 필요할 때 getResult로 만든 offset / count 정보를 유지 못해서 추가  */

	struct monitor {
		int						svcuid;
		OPL_SearchPfMonitor_t	callback;
		void					*userdata;
	} monitor;
} MetadataSearch_t;

typedef struct
{
	HxTREE_t	*instanceMap;
} MetadataSearchManager_t;

#define	____OPL_METADATASEARCH_PRIVATE_____________________________________________________________________

static HINT32	opl_metadatasearch_CompareKey (const void *arg1, const void *arg2)
{
	return (HINT32)arg1 - (HINT32)arg2;
}

static MetadataSearchManager_t *
				opl_metadatasearch_GetManager (void)
{
	static MetadataSearchManager_t	s_manager;
	if (s_manager.instanceMap == NULL)
	{
		s_manager.instanceMap = HLIB_TREE_New((HxCompareFunc)opl_metadatasearch_CompareKey);
	}
	return &s_manager;
}

static MetadataSearch_t *
				opl_metadatasearch_Attach (MetadataSearch_t *self)
{
	MetadataSearchManager_t *mgr = opl_metadatasearch_GetManager();
	if (mgr)
	{
		HLIB_TREE_Insert(mgr->instanceMap, (void *)self, (void *)self);
		return self;
	}
	return NULL;
}

static MetadataSearch_t *
				opl_metadatasearch_Detach (MetadataSearch_t *self)
{
	MetadataSearchManager_t *mgr = opl_metadatasearch_GetManager();
	if (mgr)
	{
		if (HLIB_TREE_Remove(mgr->instanceMap, (void *)self))
			return self;
	}
	return NULL;
}

static MetadataSearch_t *
				opl_metadatasearch_Check (MetadataSearch_t *self)
{
	MetadataSearchManager_t *mgr = opl_metadatasearch_GetManager();
	if (mgr)
	{
		return (MetadataSearch_t *)HLIB_TREE_Lookup(mgr->instanceMap, (void *)self);
	}
	return NULL;
}


static void
	opl_metadatasearch_StartPFMonitor (MetadataSearch_t *thiz, int svcuid)
{
	APK_QUERY_t query;
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	OPL_Channel_t	stChannel;
#endif
	HxLOG_Trace();

	APK_META_QUERY_Abort(thiz->context);

	query = APK_META_QUERY_NewQuery("endtime", ">=", "0");
	APK_META_QUERY_SetQuery(thiz->context, query);
	APK_META_QUERY_DeleteQuery(query);

#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	APK_META_QUERY_AddSvcConstraint(thiz->context, (HINT32)svcuid);
#else
	stChannel = OPL_Channel_FindChannelByUID (NULL, svcuid);
	APK_META_QUERY_AddSvcConstraint(thiz->context, OPL_Channel_GetOrgNetworkID (stChannel), OPL_Channel_GetTransponderID (stChannel), OPL_Channel_GetServiceID (stChannel));
#endif
	APK_META_QUERY_SetFilter(thiz->context, eQUERY_FILTER_PF);

	APK_META_QUERY_Search(thiz->context, FALSE);
}

static void
	opl_metadatasearch_SetResultTotalSize ( MetadataSearch_t *thiz )
{
	HxVector_t			*list = NULL;
	HINT32				i =0, n =0, total =0;

	HxLOG_Trace();

	if (thiz == NULL)
		return;

	switch( thiz->target )
	{
		case eOPL_SEARCH_TARGET_SCHEDULED:
			list = APK_META_QUERY_Result(thiz->context);
			if (list == NULL)
			{
				HxLOG_Debug("[%s:%d] has no result!!!\n", __HxFILE__, __HxLINE__);
			}
			else
			{
				n = HLIB_VECTOR_Length(list);
				for (i = 0 ; i < n ; i++)
				{
					/* spec에 맞게 offset / count 에 맞게 넣어준다. */
					if( (i < thiz->result.offset) || ( i >= ( thiz->result.offset  + thiz->result.count)) )
						continue;

#if defined( CONFIG_OP_JAPAN)
					/* 미정인 이벤트중 마지막 것만 남긴다. 나머진 걸러낸다. */
					if( ( i + 1 )< n)
					{
						DxEvent_t *event = (DxEvent_t *)HLIB_VECTOR_ItemAt(list, (i+1));
						if( event && event->duration == 0 )
						{
							continue;
						}
					}
#endif
					total++;
				}
				HLIB_VECTOR_Delete(list);
			}
			break;

		case eOPL_SEARCH_TARGET_ONDEMAND:
			HxLOG_Debug("[%s:%d] eOPL_SEARCH_TARGET_ONDEMAND!!\n", __HxFILE__, __HxLINE__);
			break;

		default:
			HxLOG_Debug("[%s:%d] Only invalid target!!\n", __HxFILE__, __HxLINE__);
			break;
	}

	thiz->result.totalSize = total;
}


static void
	opl_metadatasearch_EmitPFChanged (MetadataSearch_t *thiz, HxVector_t *result, OPL_SearchPfMonitor_t monitor, void *userdata)
{
	OPL_ProgrammeHandle	pf[32];
	HINT32				c;
	DxEvent_t		*event;

	if (result == NULL)
		return;

	c = 0;
	HxVECTOR_EACH(result, DxEvent_t *, event, {
		if (event == NULL)
			continue;
		pf[c] = OPL_Programme_New("o", APK_EVENT_Incref(event));
		if (pf[c])
			c++;
		if (c == 32)
			break;
	});
	if (monitor)
		monitor(thiz, pf, c, userdata);
	while (c--)
		OPL_Programme_Delete(pf[c]);
}



static void 	opl_metadatasearch_Listener (ApkMetaQHandle_t self, HINT32 event, const HINT32 *args, void *userdata)
{
	MetadataSearch_t 	*thiz;
	OPL_SearchState_e	state;



	thiz = opl_metadatasearch_Check((MetadataSearch_t *)userdata);
	if (thiz == NULL || thiz->context != self)
	{
		HxLOG_Error("[%s:%d] What the..... Fuxx??\n", __HxFILE__, __HxLINE__);
		return;
	}

	switch(event)
	{
	case eQUERY_EVENT_MORE_DATA:
		// searchResult.totalSize 갱신
		opl_metadatasearch_SetResultTotalSize( thiz );
		state = eOPL_SEARCH_STATE_MORE_DATA;
		break;

	case eQUERY_EVENT_FINISHED:
		// searchResult.totalSize 갱신
		opl_metadatasearch_SetResultTotalSize( thiz );
		state = eOPL_SEARCH_STATE_FINISHED;
		break;

	case eQUERY_EVENT_INVALID:
		// searchResult.totalSize 갱신
		opl_metadatasearch_SetResultTotalSize( thiz );
		state = eOPL_SEARCH_STATE_INVALID;
		break;

	case eQUERY_EVENT_NEW_VERSION:	// TODO: 이거...
		state = eOPL_SEARCH_STATE_UPDATED;
		break;
	default:
		state = eOPL_SEARCH_STATE_UNKNOWN;
		return;
	}

	if (thiz->onSearch)
	{
		thiz->onSearch((OPL_HANDLE)thiz, state);
	}
}

static void 	opl_metadatasearch_PfMonitor (ApkMetaQHandle_t self, HINT32 event, const HINT32 *args, void *userdata)
{
	MetadataSearch_t	*thiz;
	HxVector_t			*result;



	thiz = opl_metadatasearch_Check((MetadataSearch_t *)userdata);
	if (thiz == NULL || thiz->context != self)
	{
		HxLOG_Error("[%s:%d] What the..... Fuxx??\n", __HxFILE__, __HxLINE__);
		return;
	}

	if (event == eQUERY_EVENT_NEW_VERSION)
	{
		opl_metadatasearch_StartPFMonitor(thiz, thiz->monitor.svcuid);
	}
	else if (event == eQUERY_EVENT_FINISHED)
	{
		result = APK_META_QUERY_Result(thiz->context);
		if (result)
		{
			opl_metadatasearch_EmitPFChanged(thiz, result, thiz->monitor.callback, thiz->monitor.userdata);
			HLIB_VECTOR_Delete(result);
		}
	}
}

#define	____OPL_METADATASEARCH_PUBLIC______________________________________________________________________


OPL_HANDLE	OPL_METADATASEARCH_New (OPL_SearchTarget_e target)
{
	MetadataSearch_t 	*self;
	APK_META_QUERY_Target_e	eTarget;

	HxLOG_Trace();

	switch (target)
	{
	case eOPL_SEARCH_TARGET_SCHEDULED: eTarget = eQUERY_TARGET_EPG; break;
	case eOPL_SEARCH_TARGET_ONDEMAND:  eTarget = eQUERY_TARGET_VOD; break;
	default:
		HxLOG_Error("[%s:%d] Unknown Target(%d)!!\n", __HxFILE__, __HxLINE__, target);
		return NULL;
	}

	self = (MetadataSearch_t *)HLIB_STD_MemCalloc(sizeof(MetadataSearch_t));
	if (self == NULL)
	{
		HxLOG_Error("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	self->target  = target;
	self->context = APK_META_QUERY_New(eTarget);
	if (self->context == NULL)
	{
		HLIB_STD_MemFree(self);
		HxLOG_Error("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	self->dependency = HLIB_TREE_New((HxCompareFunc)opl_metadatasearch_CompareKey);
	if (self->dependency == NULL)
	{
		APK_META_QUERY_Delete(self->context);
		HLIB_STD_MemFree(self);
		HxLOG_Error("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}
	return (OPL_HANDLE)opl_metadatasearch_Attach(self);
}

void		OPL_METADATASEARCH_Delete (OPL_HANDLE self)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz)
	{
		if (HLIB_TREE_Nnodes(thiz->dependency) > 0)
		{
#ifdef	CONFIG_DEBUG
			if (!thiz->zombie)
				HxLOG_Debug("\t>>> [0x%08X] is infected. (ZOMBIE)\n", (int)thiz);
#endif
			thiz->zombie = TRUE;
			return;
		}
#ifdef	CONFIG_DEBUG
		if (thiz->zombie)
		{
			HxLOG_Debug("\t>>> [0x%08X] ZOMBIE is dead.\n", (int)thiz);
		}
#endif
		opl_metadatasearch_Detach(thiz);

		if (thiz->result.items)
			OPL_Collection_Delete(thiz->result.items);

		if (thiz->context)
			APK_META_QUERY_Delete(thiz->context);

		if (thiz->dependency)
			HLIB_TREE_Destroy(thiz->dependency);

		HLIB_STD_MemFree(thiz);
	}
}

void		OPL_METADATASEARCH_AddDependency (OPL_HANDLE self, OPL_ProgrammeHandle programme)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz)
	{
		HLIB_TREE_Insert(thiz->dependency, (void *)programme, (void *)programme);
	}
}

void		OPL_METADATASEARCH_RemoveDependency (OPL_HANDLE self, OPL_ProgrammeHandle programme)
{
	MetadataSearchManager_t	*mgr;
	MetadataSearch_t		*thiz;
	void					*iter;
	MetadataSearch_t		*garbage[20];
	HINT32					c = 0;

	HxLOG_Trace();

	if (self)
	{
		thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);
		if (thiz && HLIB_TREE_Remove(thiz->dependency, (void *)programme))
		{
			if (thiz->zombie)
				OPL_METADATASEARCH_Delete((OPL_HANDLE)thiz);
		}
		return;
	}

	mgr = opl_metadatasearch_GetManager();
	for ( iter = HLIB_TREE_Iterator(mgr->instanceMap)
		; iter
		; iter = HLIB_TREE_NextIterator(iter))
	{
		thiz = (MetadataSearch_t *)HLIB_TREE_IteratorValue(iter);
		if (thiz == NULL)
			continue;

		if (HLIB_TREE_Remove(thiz->dependency, (void *)programme))
		{
			if (thiz->zombie && c < 20)
				garbage[c++] = thiz;
		}
	}
	// 로직상 garbage는 1개만 나오지만, 그래도 혹시 모르니 20개 정도 잡아서 처리
	while (c--)
	{
		OPL_METADATASEARCH_Delete((OPL_HANDLE)garbage[c]);
	}
}

OPL_SearchTarget_e
			OPL_METADATASEARCH_GetTarget (OPL_HANDLE self)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	if (thiz)
	{
		return thiz->target;
	}
	return eOPL_SEARCH_TARGET_UNKNOWN;
}

void		OPL_METADATASEARCH_SetListener (OPL_HANDLE self, OPL_SearchListener_t onSearch)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz)
	{
		thiz->onSearch = onSearch;
		APK_META_QUERY_SetNotifier(thiz->context, opl_metadatasearch_Listener, (void *)thiz);
	}
}

void		OPL_METADATASEARCH_SetQuery (OPL_HANDLE self, const OPL_HANDLE query)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz && thiz->context)
	{
		APK_META_QUERY_SetQuery(thiz->context, (const APK_QUERY_t)query);
	}
}

OPL_HANDLE	OPL_METADATASEARCH_GetQuery (OPL_HANDLE self)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz && thiz->context)
	{
		return (OPL_HANDLE)APK_META_QUERY_GetQuery(thiz->context);
	}
	return NULL;
}


void		OPL_METADATASEARCH_AddRatingConstraint (OPL_HANDLE self, void *scheme, int threshold)
{
	HxLOG_Error("[%s:%d] NOT IMPLEMENTED YET.....\n", __HxFILE__, __HxLINE__);
}

void		OPL_METADATASEARCH_ClearRatingConstraint (OPL_HANDLE self)
{
	HxLOG_Error("[%s:%d] NOT IMPLEMENTED YET.....\n", __HxFILE__, __HxLINE__);
}

void		OPL_METADATASEARCH_AddChannelConstraint (OPL_HANDLE self, OPL_Channel_t channel)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz && thiz->context && channel)
	{
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
		APK_META_QUERY_AddSvcConstraint(thiz->context, (HINT32)OPL_Channel_GetUniqueID(channel));
#else
		APK_META_QUERY_AddSvcConstraint(thiz->context, OPL_Channel_GetOrgNetworkID (channel), OPL_Channel_GetTransponderID (channel), OPL_Channel_GetServiceID (channel));
#endif
	}
}

void		OPL_METADATASEARCH_AddTargetConstraint (OPL_HANDLE self, const char *target)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz && thiz->context)
	{
		if (strcmp(target, "P/F") == 0)
			APK_META_QUERY_SetFilter(thiz->context, eQUERY_FILTER_PF);
		else if (strcmp(target, "schedule") == 0)
			APK_META_QUERY_SetFilter(thiz->context, eQUERY_FILTER_SCH);
		else if (strcmp(target, "rfepg") == 0)
			APK_META_QUERY_SetFilter(thiz->context, eQUERY_FILTER_RF);
		else if (strcmp(target, "ipepg") == 0)
			APK_META_QUERY_SetFilter(thiz->context, eQUERY_FILTER_IP);
		else
			APK_META_QUERY_SetFilter(thiz->context, eQUERY_FILTER_ALL);
	}
}

void		OPL_METADATASEARCH_ClearChannelConstraint (OPL_HANDLE self)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz && thiz->context)
	{
		APK_META_QUERY_ClearSvcConstraint(thiz->context);
	}
}

void		OPL_METADATASEARCH_OrderBy (OPL_HANDLE self, const char *field, OPL_BOOL ascending)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();
	HxLOG_Assert(thiz);

	if (thiz && thiz->context)
	{
		APK_META_QUERY_OrderBy(thiz->context, field, ascending ? TRUE : FALSE);
	}
}


OPL_BOOL	OPL_METADATASEARCH_Search (OPL_HANDLE self)
{
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);
	HERROR			err;

	HxLOG_Trace();

	if (thiz == NULL || thiz->context == NULL)
		return OPL_FALSE;

	err = APK_META_QUERY_Search(thiz->context, FALSE);
	if (err != ERR_OK)
	{
		HxLOG_Error("[%s:%d] APK_META_QUERY_Search() return error!\n", __HxFILE__, __HxLINE__);
		return OPL_FALSE;
	}
	return OPL_TRUE;
}


OPL_SearchResults_t	*OPL_METADATASEARCH_GetResults (OPL_HANDLE self)
{
	MetadataSearch_t 	*thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz == NULL || thiz->context == NULL)
		return NULL;

	thiz->result.search = (OPL_HANDLE)thiz;
	/*
	if (!OPL_METADATASEARCH_UpdateResults(result))
	{
		HxLOG_Debug("[%s:%d] Cannot update search results!\n", __HxFILE__, __HxLINE__);
		HLIB_STD_MemFree(result);
		return NULL;
	}
	*/

	return &thiz->result;
}


void		OPL_METADATASEARCH_ReleaseResults (OPL_SearchResults_t *result)
{
	HxLOG_Trace();
	/* do nothing */
}

void		OPL_METADATASEARCH_AbortResults (OPL_SearchResults_t *result)
{
	HxLOG_Trace();

	if (result)
	{
		OPL_METADATASEARCH_Abort(result->search);

		if (result->items)
		{
			OPL_Collection_Delete(result->items);
			result->items = NULL;
		}
		result->offset    = 0;
		result->length    = 0;
		result->totalSize = 0;
		result->count     = 0;
	}
}

OPL_BOOL	OPL_METADATASEARCH_UpdateResults (OPL_SearchResults_t *result)
{
	MetadataSearch_t 	*thiz;
	HxVector_t			*list;
	HINT32				i, n;
	OPL_ProgrammeHandle	programme;

	HxLOG_Trace();

	if (result == NULL)
		return OPL_FALSE;

	thiz = opl_metadatasearch_Check((MetadataSearch_t *)result->search);
	if (thiz == NULL || thiz->context == NULL)
		return OPL_FALSE;

	if (thiz->target != eOPL_SEARCH_TARGET_SCHEDULED)
	{
		HxLOG_Debug("[%s:%d] Only scheduled data are available!!\n", __HxFILE__, __HxLINE__);
		return OPL_FALSE;
	}

	list = APK_META_QUERY_Result(thiz->context);
	if (list == NULL)
	{
		HxLOG_Debug("[%s:%d] has no result!!!\n", __HxFILE__, __HxLINE__);
		return OPL_FALSE;
	}

	if (result->items)
	{
		OPL_Collection_Delete(result->items);
	}

	result->items = OPL_Collection_NewEx((void (*)(void *))OPL_Programme_Delete, (void *(*)(void *))OPL_Programme_Clone);
	if (result->items == NULL)
	{
		HxLOG_Critical("[%s:%d] out of memory!!!\n", __HxFILE__, __HxLINE__);
		return OPL_FALSE;
	}

	n = HLIB_VECTOR_Length(list);
	for (i = 0 ; i < n ; i++)
	{
		/* spec에 맞게 offset / count 에 맞게 넣어준다. */
		if( (i < result->offset) || ( i >= ( result->offset  + result->count)) )
			continue;

#if defined( CONFIG_OP_JAPAN)
		/* 미정인 이벤트중 마지막 것만 남긴다. 나머진 걸러낸다. */
		if( ( i + 1 )< n)
		{
			DxEvent_t *event = (DxEvent_t *)HLIB_VECTOR_ItemAt(list, (i+1));
			if( event && event->duration == 0 )
			{
				continue;
			}
		}
#endif
		programme = OPL_Programme_New("o", APK_EVENT_Incref(HLIB_VECTOR_ItemAt(list, i)));
		if (programme)
			OPL_Collection_Add(result->items, (void *)programme);
	}
	HLIB_VECTOR_Delete(list);

	result->length    = OPL_Collection_Length(result->items);
	result->totalSize = result->length;

	return OPL_TRUE;
}

void				OPL_METADATASEARCH_Abort (OPL_HANDLE self)
{
	//- abort any outstanding request for results
	//- all items are removed
	MetadataSearch_t *thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);

	HxLOG_Trace();

	if (thiz == NULL || thiz->context == NULL)
		return;

	(void) APK_META_QUERY_Abort(thiz->context);
}


void		OPL_METADATASEARCH_SearchPF (OPL_HANDLE self, int svcuid, OPL_SearchPfMonitor_t monitor, void *userdata)
{
	MetadataSearch_t *thiz;

	HxLOG_Trace();
	HxLOG_Assert(svcuid && monitor);

	thiz = opl_metadatasearch_Check((MetadataSearch_t *)self);
	if (thiz)
	{
		thiz->monitor.svcuid   = svcuid;
		thiz->monitor.callback = monitor;
		thiz->monitor.userdata = userdata;

		APK_META_QUERY_SetNotifier(thiz->context, opl_metadatasearch_PfMonitor, (void *)thiz);

		opl_metadatasearch_StartPFMonitor(thiz, svcuid);
	}
	else	// direct search
	{
		HxVector_t *list;
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
		OPL_Channel_t	stChannel;
		stChannel = OPL_Channel_FindChannelByUID (NULL, svcuid);
		list = APK_META_QUERY_GetPF(OPL_Channel_GetOrgNetworkID (stChannel), OPL_Channel_GetTransponderID (stChannel), OPL_Channel_GetServiceID (stChannel));
#else
		list = APK_META_QUERY_GetPF(svcuid);
#endif
		if (list)
		{
			opl_metadatasearch_EmitPFChanged(NULL, list, monitor, userdata);
			HLIB_VECTOR_Delete(list);
		}
	}
}


