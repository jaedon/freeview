
#include <octo_common.h>

#include <dlib.h>
#include <dapi.h>
#include <otl.h>

#include "../local_include/_svc_epg.h"
#include "../local_include/_svc_epg_manager.h"
#include "../local_include/_svc_epg_event_dama.h"
#include "../local_include/_svc_epg_query.h"

#include <psvc_epg.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	void		(*fnCallback) (HxVector_t *, void *);
	void		*pvUserData;
	HxVector_t	*pstResult;
} SvcEpg_EventDamaHandler_t;

typedef struct
{
	SvcEpg_Type_b	 bwType;
	HxTREE_t		*pstMap;
} SvcEpg_EventDamaConstraint_t;

typedef struct
{
	HxVector_t	*pstTempVector;
	HUINT8		*pucSectionBuf;
	HINT32		 nSqlBuf;
	HUINT32		 ulSem;
	HxQueue_t	*pstQueue;
	HxStack_t	*pstStack;
	HxVector_t	*pstConstraint;
} SvcEpg_EventDamaContext_t;

/*******************************************************************/
/********************      STATIC Variables     ********************/
/*******************************************************************/
STATIC SvcEpg_EventDamaContext_t s_stEventContext;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define __LOCAL_FUNC____

STATIC SvcEpg_EventDamaContext_t *	svc_epg_Context (void)
{
	if (!s_stEventContext.ulSem)
	{
		if (VK_SEM_Create((unsigned long *)&s_stEventContext.ulSem, "EPG:DaMa+Octo", VK_SUSPENDTYPE_FIFO) != VK_OK)
		{
			HxLOG_Error("VK_SEM_Create() failed..!!\n");
		}
		s_stEventContext.pstTempVector = HLIB_VECTOR_New(NULL, NULL);
		s_stEventContext.pucSectionBuf = (HUINT8 *)HLIB_STD_MemAlloc(8192);
		s_stEventContext.nSqlBuf     = HLIB_RWSTREAM_Open(NULL, 0);
		s_stEventContext.pstStack      = HxStack_New(NULL, NULL);
		s_stEventContext.pstQueue      = HLIB_QUEUE_New(NULL);
	}
	VK_SEM_Get(s_stEventContext.ulSem);
	return &s_stEventContext;
}

STATIC void	svc_epg_ReleaseContext (SvcEpg_EventDamaContext_t *pstCnxt)
{
	if (s_stEventContext.ulSem)
	{
		VK_SEM_Release(s_stEventContext.ulSem);
	}
}
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
STATIC void SvcEpg_MemFreeTripleId(void * mem)
{
	HLIB_STD_MemFree(mem);
}
STATIC HINT32 svc_epg_CmpTripleId(const void *pvData1, const void *pvData2, void *pvUserData)
{
	(void)pvUserData;
	return HxSTD_StrCmp((char *)pvData1, (char *)pvData2);
}
#endif
STATIC SvcEpg_EventDamaConstraint_t *	svc_epg_NewConstraint (SvcEpg_Type_b bwType)
{
	SvcEpg_EventDamaConstraint_t *pstCons;

	pstCons = (SvcEpg_EventDamaConstraint_t *)HLIB_STD_MemAlloc(sizeof(SvcEpg_EventDamaConstraint_t));
	if (pstCons)
	{
		pstCons->bwType = bwType;

#ifdef CONFIG_FUNC_EPG_USE_SVCUID
		pstCons->pstMap  = HLIB_TREE_New(NULL);
#else
		pstCons->pstMap = HLIB_TREE_NewFull(svc_epg_CmpTripleId, NULL,
											 SvcEpg_MemFreeTripleId, NULL);
#endif

		HLIB_TREE_NodeCache(pstCons->pstMap, TRUE);
	}
	return pstCons;
}

STATIC void		svc_epg_FreeConstraint (SvcEpg_EventDamaConstraint_t *pstCons)
{
	if (pstCons)
	{
		if (pstCons->pstMap)
			HLIB_TREE_Destroy(pstCons->pstMap);
		HLIB_STD_MemFree(pstCons);
	}
}

#ifdef CONFIG_FUNC_EPG_USE_SVCUID
STATIC HBOOL	svc_epg_HasConstraint (SvcEpg_EventDamaContext_t *pstCnxt, SvcEpg_Type_b bwType, HINT32 svcuid)
{
	HINT32	i, n;
	SvcEpg_EventDamaConstraint_t	*pstCons;

	if (pstCnxt->pstConstraint == NULL)
		return FALSE;

	n = HLIB_VECTOR_Length(pstCnxt->pstConstraint);
	for (i = 0 ; i < n ; i++)
	{
		pstCons = (SvcEpg_EventDamaConstraint_t *)HLIB_VECTOR_ItemAt(pstCnxt->pstConstraint, i);
		if (!(pstCons->bwType & bwType))
			continue;
		if (HLIB_TREE_Lookup(pstCons->pstMap, (void *)svcuid))
			return TRUE;
	}
	return FALSE;
}
#else
STATIC HBOOL	svc_epg_HasConstraint (SvcEpg_EventDamaContext_t *pstCnxt, SvcEpg_Type_b bwType, HCHAR * triplet)
{
	HINT32	i, n;
	SvcEpg_EventDamaConstraint_t	*pstCons;

	if (pstCnxt->pstConstraint == NULL)
		return FALSE;

	n = HLIB_VECTOR_Length(pstCnxt->pstConstraint);
	for (i = 0 ; i < n ; i++)
	{
		pstCons = (SvcEpg_EventDamaConstraint_t *)HLIB_VECTOR_ItemAt(pstCnxt->pstConstraint, i);
		if (!(pstCons->bwType & bwType))
			continue;
		if (HLIB_TREE_Lookup(pstCons->pstMap, (void *)triplet))
			return TRUE;
	}
	return FALSE;
}

#endif

DxEPG_Type_e	svc_epg_ToDamaType (SvcEpg_Type_b bwType)
{
	switch(bwType)
	{
	case eOxEPG_TYPE_PF:	return eDxEPG_TYPE_PF;
	case eOxEPG_TYPE_SC:	return eDxEPG_TYPE_SC;
	case eOxEPG_TYPE_PF_EX:	return eDxEPG_TYPE_PF_EX;
	case eOxEPG_TYPE_SC_EX:	return eDxEPG_TYPE_SC_EX;
	case eOxEPG_TYPE_IP:	return eDxEPG_TYPE_IP;
	default:
		break;
	}
	return eDxEPG_TYPE_NONE;
}

STATIC HINT32		svc_epg_PutSectionToDama (SvcEpg_EventDamaContext_t *pstCnxt
							, SvcEpg_Type_b bwType
							, HINT32 nAntennaId
							, DxDeliveryType_e bwDeliveryType
							, const HUINT8 *pucSection
						)
{
	DbSvc_Condition_t	cond;
	HINT32			svcuid[16];
	HINT32			count;
	HUINT8			*eventBuf, *rawData;
	HINT32			length;
	HINT32			loopLen;
	HINT32			eventLen;
	const HUINT8	*loop;
	DxEPG_Type_e	dxtype;
	DxEPG_Header_t	header;
	const HCHAR		*temp[3];
	HERROR			hErr;
	DxOperator_e	eOperatorType;

	HUINT32			ulCount = 0;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}

	DB_SVC_InitSvcFindCondition(&cond);
	if (nAntennaId)
		cond.nAntIdx  = nAntennaId;
	cond.nOrgDeliType = (HINT32)bwDeliveryType;
	cond.nSvcId = (HINT32)get16bit(&(pucSection[3/*eit.service_id*/]));
	cond.nTsId  = (HINT32)get16bit(&(pucSection[8/*eit.tsid*/]));
	cond.nOnId  = (HINT32)get16bit(&(pucSection[10/*eit.onid*/]));
	//HxLOG_Debug("cond.nAntIdx[%x], cond.nSvcId[%x], cond.nTsId[%x], cond.nOnId[%x]\n",cond.nAntIdx, cond.nSvcId, cond.nTsId, cond.nOnId);

	count = DB_SVC_FindSvcUidList(&cond, svcuid, 16);
	D_RETURN_IF(count == 0, 0);

	dxtype   = svc_epg_ToDamaType(bwType);
	rawData  = pstCnxt->pucSectionBuf;
	eventBuf = rawData + sizeof(DxEPG_Header_t);

	HxSTD_memset(&header, 0, sizeof(DxEPG_Header_t));
	header.type = dxtype;


	{
		HERROR			hErr;
		HxLANG_Id_e 	eLangId;

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eLangId, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
		}

		temp[0] = HLIB_LANG_IdTo639(eLangId);
		temp[1] = OTL_CONV_Operator2String(eOperatorType);
		temp[2] = bwDeliveryType == eDxDELIVERY_TYPE_CAB ? "cab"
				: bwDeliveryType == eDxDELIVERY_TYPE_TER ? "ter" : "sat";
		HxSTD_PrintToStrN(header.spec, DxEPG_SPEC_LEN, "%s:%s:%s"
			, temp[0] ? temp[0] : " "
			, temp[1] ? temp[1] : " "
			, temp[2] ? temp[2] : " "
		);
	}

	// t(__t) readme, if you can~!!
	// event information
	HxSTD_memcpy(eventBuf, pucSection, 14/*eit.table_id ~ eit.last_table_id*/);
	loop   = pucSection + 14/*eit.table_id ~ eit.last_table_id*/;
	// event loop length
	length = (HINT32)get12bit(&(pucSection[1/*eit.section_length*/]))
	       - 4/*eit.CRC bytes*/ - (14/*eit.table_id ~ eit.last_table_id*/ - 3/*eit.table_id ~ eit.section_length*/);
	for (; length >= 12/*eit.event.event_id ~ eit.event.descriptors_loop_length*/ ;)
	{
		loopLen  = (HINT32)get12bit(&(loop[10/*eit.event.descriptor_loop_length*/]));
		length	-= (12/*eit.event.event_id ~ eit.event.descriptors_loop_length*/ + loopLen);
		{
			HxSTD_memcpy(eventBuf + 14/*eit.table_id ~ eit.last_table_id*/
				, loop
				, 12/*eit.event.event_id ~ eit.event.descriptors_loop_length*/ + loopLen);

			eventLen = 14/*eit.table_id ~ eit.last_table_id*/
					 - 3/*eit.table_id ~ eit.section_length*/
					 + 12/*eit.event.event_id ~ eit.event.descriptors_loop_length*/
					 + loopLen
					 + 4/*eit.CRC bytes*/;
			eventBuf[1] = (HUINT8)((eventLen >> 8) & 0x0F);
			eventBuf[2] = (HUINT8)((eventLen >> 0) & 0xFF);

			// reset CRC 32
			HxSTD_memset(&(eventBuf[3/*eit.table_id ~ eit.section_length*/ + eventLen - 4/*eit.CRC bytes*/]), 0, 4/*eit.CRC bytes*/);
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
		{
			HINT32 i;
			for (i = 0 ; i < count ; i++)
			{
				if (svc_epg_HasConstraint(pstCnxt, bwType, svcuid[i]))
				{
					//DbSvc_Info_t	skip;
					//DB_SVC_GetServiceInfo(svcuid[i], &skip);
					//HxLOG_Print(HxANSI_COLOR_GREEN("\tSKIP: %d.%s(%d)\n"), skip.usLcn, skip.szSvcName, svcuid[i]);
					continue;
				}
				header.uid = DxEVENT_UID(svcuid[i], (loop[0] << 8 | loop[1]));
				HxSTD_memcpy(rawData, &header, sizeof(DxEPG_Header_t));
				//HxLOG_Debug("nOnId[%x] nSvcId[%x] nTsId[%x] loop0[%x] loop[1][%x] uid[%lld]\n",cond.nOnId, cond.nSvcId, cond.nTsId, loop[0], loop[1], header.uid);
				DAPI_Set(
				//DAPI_LazySet(
					  DxEPG_NAMEOF(dxtype)
					, header.uid
					, rawData
					, sizeof(DxEPG_Header_t) + 3/*eit.table_id ~ eit.section_length*/ + eventLen
					//, 100
				);
			}
		}
#else
			{
				HCHAR	buf[32];
				HxSTD_PrintToStrN(buf, 32, "%x.%x.%x", cond.nOnId, cond.nTsId, cond.nSvcId);

				if (svc_epg_HasConstraint(pstCnxt, bwType, buf))
				{
					//DbSvc_Info_t	skip;
					//DB_SVC_GetServiceInfo(svcuid[i], &skip);
					//HxLOG_Print(HxANSI_COLOR_GREEN("\tSKIP: %d.%s(%d)\n"), skip.usLcn, skip.szSvcName, svcuid[i]);
					continue;
				}

				cond.nSvcId = (HINT32)get16bit(&(pucSection[3/*eit.service_id*/]));
				cond.nTsId	= (HINT32)get16bit(&(pucSection[8/*eit.tsid*/]));
				cond.nOnId	= (HINT32)get16bit(&(pucSection[10/*eit.onid*/]));


				header.uid =	(((HUID)cond.nOnId<<48) | (((HUID)cond.nTsId)<<32) | ((HUID)cond.nSvcId)<<16 | (((HUID)loop[0] << 8) | (HUID)loop[1]));
				//HxLOG_Debug("nOnId[%x] nSvcId[%x] nTsId[%x] loop0[%x] loop[1][%x] uid[%lld]\n",cond.nOnId, cond.nSvcId, cond.nTsId, loop[0], loop[1], header.uid);
				HxSTD_memcpy(rawData, &header, sizeof(DxEPG_Header_t));

				DAPI_Set(
				//DAPI_LazySet(
					  DxEPG_NAMEOF(dxtype)
					, header.uid
					, rawData
					, sizeof(DxEPG_Header_t) + 3/*eit.table_id ~ eit.section_length*/ + eventLen
					//, 100
				);
			}
#endif
		}
		ulCount++;
		loop	+= (12/*eit.event.event_id ~ eit.event.descriptors_loop_length*/ + loopLen);
	}
	return ulCount;
}

#ifdef	CONFIG_DEBUG

STATIC HINT32 svc_epg_CommandQuery (void *arg)
{
	SvcEpg_Query_t	*pstQuery;
//	HxVector_t		*list;
	HUINT32			time;
	HxVector_t		*pstSvcList;
	HINT32			count;
	DbSvc_Info_t		*svcInfo;
	void			*garbage;

	pstSvcList = HLIB_VECTOR_New(NULL, NULL);
	garbage = svcInfo = DB_SVC_GetAllSvcInfo(&count);
	while (count--)
	{
		HLIB_VECTOR_Add(pstSvcList, (void *)(HINT32)DbSvc_GetSvcIdx(svcInfo));
		svcInfo++;
	}
	DB_SVC_FreeServiceInfoList(NULL, NULL, garbage);

	time  = HLIB_STD_GetSystemTime();
	/* 현재 시간 이후의 모든 Event를 검색 */
	pstQuery = svc_epgquery_NewWithInteger( eOxEPG_FIELD_START_TIME, eOxEPG_COMPARISON_LT, time );
	// TODO: 새로 구현해야 됩니다..
	//list  = SVC_EPG_SearchEx(eOxEPG_TYPE_PF|eOxEPG_TYPE_SC, pstSvcList, pstQuery, NULL, NULL);
	svc_epgquery_Delete(pstQuery);
	HLIB_VECTOR_Delete(pstSvcList);

#if 0
	if (list)
	{
		HxVector_t			*list;
		SvcEpg_EventInfo_t 	*event;
		FILE *fp;
		fp = fopen("ox_query.log", "w");
		HxVECTOR_EACH(list, SvcEpg_EventInfo_t *, event, {
			//svc_epgevent_Print(event, fp);
		});
		fclose(fp);
		HLIB_CMD_Printf("%s() success\n", __FUNCTION__);
	}
	else
		HLIB_CMD_Printf("%s() fail\n", __FUNCTION__);
#endif

	return 0;
}

STATIC void	svc_epg_Command(void)
{
#define	cmd		"OxEPG"
	HLIB_CMD_RegisterWord(cmd
			, svc_epg_CommandQuery
			, "query"	// word
			, "query"	// help
			, "query"	// usage
		);
}
#endif

#define __GLOVAL_FUNC____

HERROR	svc_epg_Init (void)
{
	SvcEpg_EventDamaContext_t *pstCnxt;

	HxLOG_Trace();

	pstCnxt = svc_epg_Context();
	svc_epg_ReleaseContext(pstCnxt);

#ifdef	CONFIG_DEBUG
	svc_epg_Command();
#endif
	return ERR_OK;
}



HINT32		svc_epg_PutSection (SvcEpg_Type_b bwType, HINT32 nAntennaId, DxDeliveryType_e bwDeliveryType, const HUINT8 *pucSection)
{
	SvcEpg_EventDamaContext_t *pstCnxt;
	HINT32			ret;

	HxLOG_Trace();
	E_RETURN_IF(pucSection == NULL, ERR_FAIL );

	pstCnxt = svc_epg_Context();
	ret = svc_epg_PutSectionToDama(pstCnxt, bwType, nAntennaId, bwDeliveryType, pucSection);
	svc_epg_ReleaseContext(pstCnxt);

	return ret;
}

HINT32		svc_epg_DeleteFromDama (svcEpg_ServiceInfo_t	*pstBaseInfo, DxDeliveryType_e delivery, HINT32 nAntennaId)
{
	SvcEpg_EventDamaContext_t	*pstCnxt;
	HINT32						ret = ERR_FAIL;
	SvcEpg_Type_b				bwType = eOxEPG_TYPE_NONE;
	DxEPG_Type_e				dxtype = eDxEPG_TYPE_NONE;

	DbSvc_Condition_t	cond;
	HINT32			svcuid[16];
	HINT32			count, i;


	HxLOG_Trace();
	if(pstBaseInfo == NULL)
	{
		HxLOG_Debug("DeleteDama Fail\n");
		return ret;
	}

	DB_SVC_InitSvcFindCondition(&cond);
	if (nAntennaId)
		cond.nAntIdx  = nAntennaId;
	cond.nOrgDeliType = (HINT32)delivery;
	cond.nSvcId =pstBaseInfo->usSvcId;;
	cond.nTsId  = pstBaseInfo->usTsId;
	cond.nOnId  = pstBaseInfo->usOnId;

	count = DB_SVC_FindSvcUidList(&cond, svcuid, 16);
	if(count == 0)
	{
		HxLOG_Debug("DeleteDama Fail count = 0\n");
		return ret;
	}


	pstCnxt = svc_epg_Context();

	ret = psvc_epg_GetEpgTypeByTableId(pstBaseInfo->ucTableId, &bwType);
	dxtype = svc_epg_ToDamaType(bwType);



	if(ret == ERR_OK && dxtype != eDxEPG_TYPE_NONE)
	{
		HLIB_RWSTREAM_Rewind(pstCnxt->nSqlBuf);
		HLIB_RWSTREAM_Print(pstCnxt->nSqlBuf, "DELETE FROM %s WHERE svcuid IN( ", DxEPG_NAMEOF(dxtype));

		for (i = 0 ; i < count ; i++)
		{
			if (i > 0)
				HLIB_RWSTREAM_PutChar(pstCnxt->nSqlBuf, ',');
			HLIB_RWSTREAM_Print(pstCnxt->nSqlBuf, "%d", svcuid[i]);
		}
		HLIB_RWSTREAM_Print(pstCnxt->nSqlBuf, ")");
		HxLOG_Debug("\'%s\'\n", HLIB_RWSTREAM_GetBuf(pstCnxt->nSqlBuf));

		DAPI_Query(HLIB_RWSTREAM_GetBuf(pstCnxt->nSqlBuf), NULL, NULL);
	}
	else
	{

		HxLOG_Debug("DeleteDama Fail ret:%d ,dxtype:%d \n",ret,dxtype);
	}
	svc_epg_ReleaseContext(pstCnxt);

	return ret;
}

void	svc_epg_SetServiceConstraint(SvcEpg_Type_b bwType, const HxVector_t *pstSvcList)
{
	HINT32							i, n;
	SvcEpg_EventDamaContext_t		*pstCnxt = NULL;
	SvcEpg_EventDamaConstraint_t	*pstCons = NULL;
	HxVector_t						*list;

	HxLOG_Trace();

	pstCnxt = svc_epg_Context();
	if (pstCnxt->pstConstraint == NULL)
	{
		pstCnxt->pstConstraint = HLIB_VECTOR_NewEasy(0, (HxFreeFunc_t)svc_epg_FreeConstraint, NULL);
		if (pstCnxt->pstConstraint == NULL)
			goto OUT_OF_MEMORY;
	}

	n = HLIB_VECTOR_Length(pstCnxt->pstConstraint);
	for (i = 0 ; i < n ; i++)
	{
		pstCons = (SvcEpg_EventDamaConstraint_t *)HLIB_VECTOR_ItemAt(pstCnxt->pstConstraint, i);
		if (pstCons) {
			if (pstCons->bwType == bwType)
				break;
		}
	}

	if (pstSvcList == NULL)
	{
		if (i == n)
			goto DO_NOTHING;
		HLIB_VECTOR_Remove(pstCnxt->pstConstraint, i);
	}
	else
	{
		if (i == n)
		{
			pstCons = svc_epg_NewConstraint(bwType);
			if (pstCons == NULL)
				goto OUT_OF_MEMORY;
			HLIB_VECTOR_Add(pstCnxt->pstConstraint, (void *)pstCons);
		}
		else
		{
			if (pstCons)
				HLIB_TREE_RemoveAll(pstCons->pstMap);
		}

		list = (HxVector_t *)pstSvcList;
		{
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
			HINT32				uid;
			if (pstCons) {
				HxVECTOR_EACH(list, HINT32, uid, {
					HLIB_TREE_Insert(pstCons->pstMap, (void *)uid, (void *)TRUE);
				});
			}
#else
			HCHAR				*pszTripleId;
			if (pstCons) {
				HxVECTOR_EACH(list, HCHAR *, pszTripleId, {
					HLIB_TREE_Insert(pstCons->pstMap, (void *)HLIB_MEM_StrDup(pszTripleId), (void *)TRUE);
				});
			}
#endif
		}
	}

DO_NOTHING:
OUT_OF_MEMORY:
	if (pstCons)
		svc_epg_ReleaseContext(pstCnxt);
}

