#ifdef OIPF
#include "HLcnUpdate.h"
#include "HLcnUpdateEvent.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdarg.h>

#include <dlib.h>
#include "jsplugin.h"
#include "macros.h"
#include "NativeHConfiguration.h"
#include "opllocalsystem.h"
#include "oplchannel.h"


using namespace Ooif;


list<HLcnUpdate*> HLcnUpdate::instances;


HLcnUpdate::HLcnUpdate() : EventTarget()
{
	ENTRY;

	classType = class_name;
	HLcnUpdate::instances.push_back(this);
}


HLcnUpdate::~HLcnUpdate()
{
	ENTRY;
	HLcnUpdate::instances.remove(this);

}

int HLcnUpdate::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(startLcnUpdate,	obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopLcnUpdate,	obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(saveLcnUpdate,	obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getLcnUpdateInfo,	obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeLcnUpdateInfo, obj, "b", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(LcnUpdate);
	return JSP_GET_NOTFOUND;
}

int HLcnUpdate::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;


	INTRINSIC_EVENT(LcnUpdate);

	return JSP_PUT_NOTFOUND;
}


int HLcnUpdate::startLcnUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	OOIF_LOG_DEBUG(" Start LCN UPDATE!!! ");

	//OOIF_LOG_PRINT("\n\n\n[%s,%d] LCN COMMAND(%d) UPDATE START\n\n\n",__FUNCTION__,__LINE__, NUMBER_VALUE(argv[0]));
	native_HLcnUpdate_start(onLcnUpdateEventOPL, NUMBER_VALUE(argv[0]));

	return 0;
}


int HLcnUpdate::stopLcnUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIF_LOG_DEBUG(" Stop LCN UPDATE!!! ");
	//OOIF_LOG_PRINT("\n\n\n[%s,%d] UPDATE STOP\n\n\n",__FUNCTION__,__LINE__);
	native_HLcnUpdate_stop(onLcnUpdateEventOPL);

	return 0;
}

int HLcnUpdate::saveLcnUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	//OOIF_LOG_PRINT("\n\n\n[%s,%d] LCN COMMAND(%d)(%d) SAVE START\n\n\n",__FUNCTION__,__LINE__, NUMBER_VALUE(argv[0]),NUMBER_VALUE(argv[1]));

	OOIF_LOG_DEBUG(" Save LCN UPDATE!!! ");
	native_HLcnUpdate_save(onLcnUpdateEventOPL, NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]));

	return 0;
}

int HLcnUpdate::getLcnUpdateInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	//OOIF_LOG_PRINT("\n\n\n[%s,%d] GET INFO\n\n\n",__FUNCTION__,__LINE__);

	OOIF_LOG_DEBUG(" GET INFO LCN UPDATE!!! ");
	native_HLcnUpdate_getinfo(onLcnUpdateEventOPL);

	return 0;
}

int HLcnUpdate::removeLcnUpdateInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	//OOIF_LOG_PRINT("\n\n\n[%s,%d] REMOVE INFO\n\n\n",__FUNCTION__,__LINE__);

	OOIF_LOG_DEBUG(" REMOVE INFO LCN UPDATE!!! ");
	native_HLcnUpdate_removeInfo(BOOLEAN_VALUE(argv[0]));

	return 0;
}

void HLcnUpdate::onLcnUpdateEvent(humax::LcnUpdateEvent event, humax::LcnUpdateEventInfo *info)
{
	Event	*e = NULL;
	jsplugin_obj	*context;

	for (list<HLcnUpdate*>::iterator it=instances.begin(); it!=instances.end(); it++) {
		HLcnUpdate *instance = *it;

		context = instance->getHost();

		switch (event)
		{
			case	humax::LcnUpdateEvent_Start_OK:
			case	humax::LcnUpdateEvent_Start_FAIL:
				{
					humax::LcnUpdateStart_Data	*params = &info->uStartData;

					e = new HLcnUpdateEvent(context, event, params->numOfList, params->svcList, 0, NULL, 0, NULL);
				}
				break;

			case	humax::LcnUpdateEvent_Save_OK:
			case	humax::LcnUpdateEvent_Save_FAIL:
				{
					humax::LcnUpdateSave_Data	*params = &info->uSaveData;

					e = new HLcnUpdateEvent(context, event, 0, NULL, params->numOfNewSvc, params->newSvc, params->numOfReplacedSvc, params->replacedSvc);
				}
				break;

			case	humax::LcnUpdateEvent_Get_Info:
				{
					humax::LncUpdateInfo_Data	*params = &info->uInfoData;

					e = new HLcnUpdateEvent(context, event, params->svcListName, params->tvNum, params->radioNum, params->lastUpdatedTime, params->lastCheckedTime, params->displayInfo, params->coldbootResult);
				}
				break;

			default:
				break;
		}
		instance->dispatchEvent(e);
	}
}

void HLcnUpdate::onLcnUpdateEventOPL(void *pEventData)
{
	DxLcnUpdateInform_t *pstLcnInform = (DxLcnUpdateInform_t*)pEventData;

	if (pstLcnInform == NULL)
		return ;

	int		i;
	int 	numOfList = 0;
	int	numOfNewSvc = 0, numOfReplacedSvc = 0;

	humax::LcnUpdateEventInfo		lcnUpdateEventInfo;

	memset(&lcnUpdateEventInfo, 0, sizeof(humax::LcnUpdateEventInfo));

	switch(pstLcnInform->eventType)
	{
		case eDxLCNUPDATE_START_OK:
			memset(&lcnUpdateEventInfo.uStartData, 0, sizeof(humax::LcnUpdateStart_Data));

			if(pstLcnInform->lcnUpdateStartData)
				numOfList = pstLcnInform->lcnUpdateStartData->ulSvcListNum;

			//OOIF_LOG_PRINT("\n\n\n[%s,%d] ========= TEST LOG , numOfList : %d ==========\n\n\n",__FUNCTION__,__LINE__, numOfList);

			lcnUpdateEventInfo.uStartData.numOfList = numOfList;

			if (numOfList > 0)
			{
				lcnUpdateEventInfo.uStartData.svcList = (humax::LcnUpdateSvcList_Data*)HLIB_STD_MemAlloc(sizeof(humax::LcnUpdateSvcList_Data) * numOfList);
				if(lcnUpdateEventInfo.uStartData.svcList)
				{
					memset(lcnUpdateEventInfo.uStartData.svcList, 0, sizeof(humax::LcnUpdateSvcList_Data) * numOfList);

					for (i = 0; i < numOfList; i++)
					{
						lcnUpdateEventInfo.uStartData.svcList[i].index = pstLcnInform->lcnUpdateStartData->svcList[i].index;
						strncpy(lcnUpdateEventInfo.uStartData.svcList[i].name, pstLcnInform->lcnUpdateStartData->svcList[i].szSvcListName, sizeof(lcnUpdateEventInfo.uStartData.svcList[i].name));
					}
				}
			}

			onLcnUpdateEvent(humax::LcnUpdateEvent_Start_OK, &lcnUpdateEventInfo);

			if(lcnUpdateEventInfo.uStartData.svcList)
				HLIB_MEM_Free(lcnUpdateEventInfo.uStartData.svcList);

			if(pstLcnInform && pstLcnInform->lcnUpdateStartData != NULL)
				HLIB_MEM_Free(pstLcnInform->lcnUpdateStartData);

			if(pstLcnInform)
				HLIB_MEM_Free(pstLcnInform);
			break;

		case eDxLCNUPDATE_START_FAIL:
			memset(&lcnUpdateEventInfo.uStartData, 0, sizeof(humax::LcnUpdateStart_Data));

			// Make empty info
			lcnUpdateEventInfo.uStartData.numOfList = 0;
			lcnUpdateEventInfo.uStartData.svcList = NULL;

			onLcnUpdateEvent(humax::LcnUpdateEvent_Start_FAIL, &lcnUpdateEventInfo);

			break;
		case eDxLCNUPDATE_SAVE_OK:
			memset(&lcnUpdateEventInfo.uSaveData, 0, sizeof(humax::LcnUpdateSave_Data));

			if (pstLcnInform->lcnUpdateSaveData == NULL)
				break ;

			numOfNewSvc = pstLcnInform->lcnUpdateSaveData->ulNewSvcNum;

			lcnUpdateEventInfo.uSaveData.numOfNewSvc = numOfNewSvc;

			if(numOfNewSvc > 0)
			{
				lcnUpdateEventInfo.uSaveData.newSvc = (humax::LcnUpdateSvc_Data*)HLIB_STD_MemAlloc(sizeof(humax::LcnUpdateSvc_Data) * numOfNewSvc);
				memset(lcnUpdateEventInfo.uSaveData.newSvc, 0, sizeof(humax::LcnUpdateSvc_Data) * numOfNewSvc);

				for (i = 0; i < numOfNewSvc; i++)
				{
					OPL_Channel_MakeCCID(lcnUpdateEventInfo.uSaveData.newSvc[i].ccid, \
					                     sizeof( lcnUpdateEventInfo.uSaveData.newSvc[i].ccid ), \
					                     pstLcnInform->lcnUpdateSaveData->newSvc[i].ulSvcUid, \
					                     pstLcnInform->lcnUpdateSaveData->newSvc[i].usOnId, \
					                     pstLcnInform->lcnUpdateSaveData->newSvc[i].usTsId, \
					                     pstLcnInform->lcnUpdateSaveData->newSvc[i].usSvcId);
					lcnUpdateEventInfo.uSaveData.newSvc[i].lcn = pstLcnInform->lcnUpdateSaveData->newSvc[i].lcn;
					strncpy(lcnUpdateEventInfo.uSaveData.newSvc[i].name, pstLcnInform->lcnUpdateSaveData->newSvc[i].szSvcName, MAX_DBSVC_NAME_LEN);
				}
			}

			numOfReplacedSvc = pstLcnInform->lcnUpdateSaveData->ulReplacedSvcNum;

			lcnUpdateEventInfo.uSaveData.numOfReplacedSvc = numOfReplacedSvc;

			if(numOfReplacedSvc > 0)
			{
				lcnUpdateEventInfo.uSaveData.replacedSvc = (humax::LcnUpdateSvc_Data*)HLIB_STD_MemAlloc(sizeof(humax::LcnUpdateSvc_Data) * numOfReplacedSvc);
				memset(lcnUpdateEventInfo.uSaveData.replacedSvc, 0, sizeof(humax::LcnUpdateSvc_Data) * numOfReplacedSvc);

				for (i = 0; i < numOfReplacedSvc; i++)
				{
					OPL_Channel_MakeCCID(lcnUpdateEventInfo.uSaveData.replacedSvc[i].ccid, \
					                     sizeof(lcnUpdateEventInfo.uSaveData.replacedSvc[i].ccid), \
					                     pstLcnInform->lcnUpdateSaveData->replacedSvc[i].ulSvcUid, \
					                     pstLcnInform->lcnUpdateSaveData->replacedSvc[i].usOnId, \
					                     pstLcnInform->lcnUpdateSaveData->replacedSvc[i].usTsId, \
					                     pstLcnInform->lcnUpdateSaveData->replacedSvc[i].usSvcId);
					lcnUpdateEventInfo.uSaveData.replacedSvc[i].lcn = pstLcnInform->lcnUpdateSaveData->replacedSvc[i].lcn;
					strncpy(lcnUpdateEventInfo.uSaveData.replacedSvc[i].name, pstLcnInform->lcnUpdateSaveData->replacedSvc[i].szSvcName, MAX_DBSVC_NAME_LEN);
				}
			}

			onLcnUpdateEvent(humax::LcnUpdateEvent_Save_OK, &lcnUpdateEventInfo);

			if(lcnUpdateEventInfo.uSaveData.newSvc)
				HLIB_MEM_Free(lcnUpdateEventInfo.uSaveData.newSvc);

			if(lcnUpdateEventInfo.uSaveData.replacedSvc)
				HLIB_MEM_Free(lcnUpdateEventInfo.uSaveData.replacedSvc);

			if(pstLcnInform->lcnUpdateSaveData->newSvc)
				HLIB_MEM_Free(pstLcnInform->lcnUpdateSaveData->newSvc);

			if(pstLcnInform->lcnUpdateSaveData->replacedSvc)
				HLIB_MEM_Free(pstLcnInform->lcnUpdateSaveData->replacedSvc);

			if(pstLcnInform->lcnUpdateSaveData)
				HLIB_MEM_Free(pstLcnInform->lcnUpdateSaveData);

			if(pstLcnInform)
				HLIB_MEM_Free(pstLcnInform);

			break;


		case eDxLCNUPDATE_SAVE_FAIL:
			memset(&lcnUpdateEventInfo.uSaveData, 0, sizeof(humax::LcnUpdateSave_Data));

			// Make empty info
			lcnUpdateEventInfo.uSaveData.numOfNewSvc = 0;
			lcnUpdateEventInfo.uSaveData.numOfReplacedSvc = 0;
			lcnUpdateEventInfo.uSaveData.newSvc = NULL;
			lcnUpdateEventInfo.uSaveData.replacedSvc = NULL;

			onLcnUpdateEvent(humax::LcnUpdateEvent_Save_FAIL, &lcnUpdateEventInfo);
			break;

		case eDxLCNUPDATE_STOP_OK:
		case eDxLCNUPDATE_STOP_FAIL:
			break;

		case eDxLCNUPDATE_GET_INFO:
			memset(&lcnUpdateEventInfo.uInfoData, 0, sizeof(humax::LncUpdateInfo_Data));

			if (pstLcnInform->lcnUpdateInfo == NULL)
				break ;

			lcnUpdateEventInfo.uInfoData.tvNum = pstLcnInform->lcnUpdateInfo->ulTvNum;
			lcnUpdateEventInfo.uInfoData.radioNum = pstLcnInform->lcnUpdateInfo->ulRadioNum;
			lcnUpdateEventInfo.uInfoData.displayInfo = pstLcnInform->lcnUpdateInfo->bDisplayInfo;
			strncpy(lcnUpdateEventInfo.uInfoData.svcListName, pstLcnInform->lcnUpdateInfo->szServiceListName, MAX_LCNUPDATEDINFO_NAME_LEN);
			strncpy(lcnUpdateEventInfo.uInfoData.lastUpdatedTime, pstLcnInform->lcnUpdateInfo->szLastUpdatedTime, 20);
			strncpy(lcnUpdateEventInfo.uInfoData.lastCheckedTime, pstLcnInform->lcnUpdateInfo->szLastCheckedTime, 20);
			lcnUpdateEventInfo.uInfoData.coldbootResult = pstLcnInform->lcnUpdateInfo->ulColdbootResult;

			onLcnUpdateEvent(humax::LcnUpdateEvent_Get_Info, &lcnUpdateEventInfo);

			if(pstLcnInform->lcnUpdateInfo)
				HLIB_MEM_Free(pstLcnInform->lcnUpdateInfo);

			if(pstLcnInform)
				HLIB_MEM_Free(pstLcnInform);

			break;

		default:
			break;
	}
}

// Using for Service Lists
CLcnUpdateSvcList::CLcnUpdateSvcList(int _numOfSvcList, humax::LcnUpdateSvcList_Data*_svcList)
{
	classType = CLASS_LCNUPDATESVCLIST;

	m_numOfSvcList = _numOfSvcList;
	svcList = NULL;

	if (_numOfSvcList > 0)
	{
		svcList = (humax::LcnUpdateSvcList_Data*)HLIB_STD_MemAlloc(sizeof(humax::LcnUpdateSvcList_Data) * _numOfSvcList);
		memcpy(svcList, _svcList, _numOfSvcList * sizeof(humax::LcnUpdateSvcList_Data));
	}
}

CLcnUpdateSvcList::~CLcnUpdateSvcList()
{
	if (m_numOfSvcList > 0)
		HLIB_STD_MemFree(svcList);
}

int CLcnUpdateSvcList::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER("length", m_numOfSvcList, JSP_GET_VALUE);
	GET_FUNCTION(getInfo, obj, "ns", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int	CLcnUpdateSvcList::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

int	CLcnUpdateSvcList::getInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		index;
	const	char	*type;

	CHECK_ARGUMENT_COUNT(2);

	CLcnUpdateSvcList	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, CLcnUpdateSvcList);

	index = NUMBER_VALUE(argv[0]);
	type = STRING_VALUE(argv[1]);

	if (index < instance->m_numOfSvcList)
	{
		humax::LcnUpdateSvcList_Data	*info = &(instance->svcList[index]);

		if (strstr(type, "name")) {
			RETURN_STRING(info->name, JSP_CALL_VALUE);
		}
		else if (strstr(type, "index")) {
			RETURN_NUMBER(info->index, JSP_CALL_VALUE);
		}
	}

	RETURN_UNDEFINED(JSP_CALL_VALUE);
}

// Using for Service of new or replaced
CLcnUpdateSvc::CLcnUpdateSvc(int _numOfSvc, humax::LcnUpdateSvc_Data*_svc)
{
	classType = CLASS_LCNUPDATESVC;

	m_numOfSvc = _numOfSvc;
	svc = NULL;

	if (_numOfSvc > 0)
	{
		svc = (humax::LcnUpdateSvc_Data*)HLIB_STD_MemAlloc(sizeof(humax::LcnUpdateSvc_Data) * m_numOfSvc);
		memcpy(svc, _svc, m_numOfSvc * sizeof(humax::LcnUpdateSvc_Data));
	}
}

CLcnUpdateSvc::~CLcnUpdateSvc()
{
	if (m_numOfSvc > 0)
		HLIB_STD_MemFree(svc);
}

int CLcnUpdateSvc::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER("length", m_numOfSvc, JSP_GET_VALUE);
	GET_FUNCTION(getInfo, obj, "ns", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int	CLcnUpdateSvc::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

int	CLcnUpdateSvc::getInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		index;
	const	char	*type;

	CHECK_ARGUMENT_COUNT(2);

	CLcnUpdateSvc	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, CLcnUpdateSvc);

	index = NUMBER_VALUE(argv[0]);
	type = STRING_VALUE(argv[1]);

	if (index < instance->m_numOfSvc)
	{
		humax::LcnUpdateSvc_Data	*info = &(instance->svc[index]);

		if (strstr(type, "name")) {
			RETURN_STRING(info->name, JSP_CALL_VALUE);
		}
		else if (strstr(type, "lcn")) {
			RETURN_NUMBER(info->lcn, JSP_CALL_VALUE);
		}
		else if (strstr(type, "ccid")) {
			RETURN_STRING(info->ccid, JSP_CALL_VALUE);
		}
	}

	RETURN_UNDEFINED(JSP_CALL_VALUE);
}


#endif // OIPF


#define ____HMX_LcnUpdate_functions____

hLcnUpdateEventListener global_hLcnUpdateEventListener = NULL;	// global listener variable for lcn update


OOIFReturnCode native_HLcnUpdate_start(hLcnUpdateEventListener listener, HUINT32 searchMode)
{
	native_HLcnUpdateSetListener(listener);
	OPL_HMXStartLcnUpdate(native_HLcnUpdateEventListener, searchMode);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HLcnUpdate_stop(hLcnUpdateEventListener listener)
{
	native_HLcnUpdateSetListener(listener);
	OPL_HMXStopLcnUpdate(native_HLcnUpdateEventListener);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HLcnUpdate_save(hLcnUpdateEventListener listener, HUINT32 searchMode, HUINT32 index)
{
	native_HLcnUpdateSetListener(listener);
	OPL_HMXSaveLcnUpdate(native_HLcnUpdateEventListener, searchMode, index);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HLcnUpdate_getinfo(hLcnUpdateEventListener listener)
{
	native_HLcnUpdateSetListener(listener);
	OPL_HMXGetLcnUpdateInfo(native_HLcnUpdateEventListener);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HLcnUpdate_removeInfo(OOIFBoolean sync)
{
	//OOIF_LOG_PRINT("\n\n\n[%s,%d] NATIVE REMOVE INFO\n\n\n",__FUNCTION__,__LINE__);
	OPL_HMXRemoveLcnUpdateInfo(sync);
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_HLcnUpdateSetListener(hLcnUpdateEventListener listener)
{
	global_hLcnUpdateEventListener = listener;
	return OOIF_RETURN_OK;
}


void native_HLcnUpdateEventListener(void *pstLcnInform)
{
	if (global_hLcnUpdateEventListener)
	{
		global_hLcnUpdateEventListener(pstLcnInform);
	}
}




