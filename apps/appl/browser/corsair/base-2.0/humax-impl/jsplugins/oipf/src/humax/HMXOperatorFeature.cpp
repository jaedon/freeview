/*
 * HMXOperatorFeature.cpp
 *
 *  Created on: 2012. 03. 15.
 *      Author: mspark
 */

#include <string.h>
#include <dlib.h>

#include "macros.h"
#include "OOIFUtils.h"
#include "opllocalsystem.h"
#include "HMXOperatorFeature.h"
#include "NCDNotifyBroadcastEvent.h"
// OPL
using namespace Ooif;
list<HMXOperatorFeature*> HMXOperatorFeature::instances;

HMXOperatorFeature::HMXOperatorFeature()
{
	OOIF_LOG_PRINT("HMXOperatorFeature::HMXOperatorFeature\n");
	classType = class_name;
	HMXOperatorFeature::instances.push_back(this);
}

HMXOperatorFeature::~HMXOperatorFeature()
{
	OOIF_LOG_PRINT("HMXOperatorFeature::~HMXOperatorFeature\n");
	HMXOperatorFeature::instances.remove(this);
}

int HMXOperatorFeature::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	OOIF_LOG_PRINT("HMXOperatorFeature::getter [%s]\n", name);
	GET_STRING("name", "OperatorFeature", JSP_GET_VALUE);
	GET_FUNCTION(listenHomeTp, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setNcdNotifyData,	obj, "i", JSP_GET_VALUE_CACHE);	
	GET_FUNCTION(getNcdNetworkMsg,	obj, "i", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(OperatorFeature);
	return JSP_GET_NOTFOUND;
	//return ObjectInstance::getter(obj, name, result);
}

int HMXOperatorFeature::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	OOIF_LOG_PRINT("HMXOperatorFeature::setter [%s]\n", name);
	INTRINSIC_EVENT(NCDChanged);
	return JSP_GET_NOTFOUND;
	//return ObjectInstance::setter(obj, name, value);
}

int HMXOperatorFeature::listenHomeTp(
		jsplugin_obj *this_obj,
		jsplugin_obj *function_obj,
		int argc,
		jsplugin_value *argv,
		jsplugin_value *result)
{
	OOIF_LOG_PRINT("HMXOperatorFeature::listenHomeTp\n");
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean listen = TYPE_BOOLEAN(argv[0]);

	if(listen == TRUE)
	{
		OPL_LocalSystem_ExecuteCommand(0);
	}

	RETURN_BOOLEAN(listen, JSP_GET_VALUE);
}
#define ____HMX_NcdInfo_functions____

hOperatorFeatureListener global_hOperatorFeatureListener = NULL;	// global listener variable for channel import export
int HMXOperatorFeature::getNcdNetworkMsg(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIF_LOG_PRINT(" Get NcdInfo Info ");
	CHECK_ARGUMENT_COUNT(1);
	native_HMXOperatorFeauture_GetNcdNetworkMsg(onNcdInfoEventOPL,NUMBER_VALUE(argv[0]));

	return 0;
}
void HMXOperatorFeature::onNcdInfoEvent(humax::NcdInfoEventType_e event, humax::NcdNetworkInfo_t *info)
{
	for (list<HMXOperatorFeature*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();

		switch (event)
		{
			case	humax::eNCDEVENT_GET_INFO:
				{
					char *params = (char *)info->ncdnetworkMsg;
					OOIF_LOG_PRINT("[onNcdInfoEvent] info->ncdnetworkMsg : %s , params:%s\n",info->ncdnetworkMsg,params);
					e = new NCDNotifyBroadcastEvent(context, 0, 0, params);
                    if ( e )
                    {
                        (*it)->dispatchEvent(e);
                    }
				}
				break;

			default:
				break;
		}
	}
}

void HMXOperatorFeature::onNcdInfoEventOPL(void *pEventData)
{
	DxNcdNetworkInfo_t *pstNcdInfo = (DxNcdNetworkInfo_t*)pEventData;
	humax::NcdNetworkInfo_t		ncdInfoEventInfo;

	memset(&ncdInfoEventInfo, 0, sizeof(humax::NcdNetworkInfo_t));

	if(pstNcdInfo == NULL) 
	{
		onNcdInfoEvent(humax::eNCDEVENT_GET_INFO, &ncdInfoEventInfo);
		return;
	}
	OOIF_LOG_PRINT("[onNcdInfoEventOPL ] pstNcdInfo->eventType:%d\n", pstNcdInfo->eventType);
	switch(pstNcdInfo->eventType)
	{

		case eDxTRDCONFLICT_GET_INFO:
			memset(&ncdInfoEventInfo, 0, sizeof(humax::NcdNetworkInfo_t));
			if(HxSTD_StrLen((char *)pstNcdInfo->szNcdnetworkMsg) > 0)
			{
				HxSTD_StrNCpy(ncdInfoEventInfo.ncdnetworkMsg, (char *)pstNcdInfo->szNcdnetworkMsg,HxSTD_StrLen((char *)pstNcdInfo->szNcdnetworkMsg));
				onNcdInfoEvent(humax::eNCDEVENT_GET_INFO, &ncdInfoEventInfo);
				if(pstNcdInfo)
				{
					HLIB_MEM_Free(pstNcdInfo);
				}

			}
			else
			{
				onNcdInfoEvent(humax::eNCDEVENT_GET_INFO, &ncdInfoEventInfo);
				
			}
			break;

		default:
			break;
	}
}

int HMXOperatorFeature::setNcdNotifyData(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	unsigned int flag = NUMBER_VALUE(argv[0]);
	OOIF_LOG_DEBUG(" InitNcdNotifyData !!  flag:%d ", flag);
	OPL_HMXOperatorFeauture_SetNcdNotifyData(flag);
	
	return 0;
}
OOIFReturnCode native_HMXOperatorFeautureSetListener(hOperatorFeatureListener listener)
{
	global_hOperatorFeatureListener = listener;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HMXOperatorFeauture_GetNcdNetworkMsg(hOperatorFeatureListener listener,unsigned int param1)
{
	native_HMXOperatorFeautureSetListener(listener);
	OPL_HMXOperatorFeauture_GetNcdInfo(native_HMXOperatorFeautureNcdEventListener,param1);
	return OOIF_RETURN_OK;
}
void native_HMXOperatorFeautureNcdEventListener(void *pstNcdInfo)
{

	if (global_hOperatorFeatureListener)
	{
		global_hOperatorFeatureListener(pstNcdInfo);
	}
}


