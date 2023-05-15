#ifdef OIPF
#include "HTrdConflict.h"
#include "HTrdConflictEvent.h"


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


using namespace Ooif;


list<HTrdConflict*> HTrdConflict::instances;


HTrdConflict::HTrdConflict() : EventTarget()
{
	ENTRY;

	classType = class_name;
	HTrdConflict::instances.push_back(this);
}


HTrdConflict::~HTrdConflict()
{
	ENTRY;
	HTrdConflict::instances.remove(this);

}




int HTrdConflict::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(setTrdConflict,	obj, "ii", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getTrdConflict,	obj, "i", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(TrdConflict);
	return JSP_GET_NOTFOUND;
}

int HTrdConflict::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;


	INTRINSIC_EVENT(TrdConflict);

	return JSP_PUT_NOTFOUND;
}


int HTrdConflict::getTrdConflict(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIF_LOG_DEBUG(" Get TrdConflict Info ");
	CHECK_ARGUMENT_COUNT(1);
	native_HTrdConflict_GetTrdConflict(onTrdConflictEventOPL,NUMBER_VALUE(argv[0]));

	return 0;
}

int HTrdConflict::setTrdConflict(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIF_LOG_DEBUG(" Set TrdConflict Info : %d %d\n", NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]));
	CHECK_ARGUMENT_COUNT(2);

	NATIVE_CALL(native_HTrdConflict_SetTrdConflict(NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1])));
	return 0;
}

void HTrdConflict::onTrdConflictEvent(humax::TrdConflictEventType_e event, unsigned int trdconflictCount, humax::TrdConflictData_t *info)
{
//	OOIF_LOG_DEBUG(" Received Channel Import Export Message : %d, %d \n ", type, result);
	for (list<HTrdConflict*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();

		switch (event)
		{
			case	humax::eTRDCONFLICT_GET_INFO:
				{
					//humax::TrdConflictList_t	*params = &info->trdconflictInfo;
					char *params = info->trdconflictStr;
					int count = trdconflictCount;

					e = new HTrdConflictEvent(context, event, count, params);
                    if ( e )
                    {
                        (*it)->dispatchEvent(e);
                    }

					//e = new HTrdConflictEvent(context, event, params->ulArrayNum,params->pstTrdConflictedArray);
				}
				break;

			default:
				break;
		}

	}
}

void HTrdConflict::onTrdConflictEventOPL(void *pEventData)
{
	DxTrdConflictData_t *pstTrdConflict = (DxTrdConflictData_t*)pEventData;

	if (pstTrdConflict == NULL)
		return ;

	int		i;
	humax::TrdConflictData_t		trdconflictEventInfo;

	memset(&trdconflictEventInfo, 0, sizeof(humax::TrdConflictData_t));

	switch(pstTrdConflict->eventType)
	{

		case eDxTRDCONFLICT_GET_INFO:
			memset(&trdconflictEventInfo.trdconflictInfo, 0, sizeof(humax::TrdConflictList_t));
//			OOIF_LOG_PRINT("[onTrdConflictEventOPL]  eDxTRDCONFLICT_GET_INFO  pstTrdConflict->ulTrdConflictCount:%d\n",pstTrdConflict->ulTrdConflictCount);

			if(	pstTrdConflict->ulTrdConflictCount == 0)
			{
				onTrdConflictEvent(humax::eTRDCONFLICT_GET_INFO,0/*pstTrdConflict->ulTrdConflictCount*/, &trdconflictEventInfo);
			}
			else 
			{
				
				trdconflictEventInfo.trdconflictInfo.ulArrayNum = pstTrdConflict->trdconflictInfo->ulArrayNum;
				if(trdconflictEventInfo.trdconflictInfo.ulArrayNum >0)
				{
					trdconflictEventInfo.trdconflictInfo.pstTrdConflictedArray = (humax::TrdConflictSvc_t*)HLIB_STD_MemAlloc(sizeof(humax::TrdConflictSvc_t) * trdconflictEventInfo.trdconflictInfo.ulArrayNum);
					memset(trdconflictEventInfo.trdconflictInfo.pstTrdConflictedArray, 0, sizeof(humax::TrdConflictSvc_t) * trdconflictEventInfo.trdconflictInfo.ulArrayNum);
				
					for (i = 0; i < trdconflictEventInfo.trdconflictInfo.ulArrayNum; i++)
					{
						trdconflictEventInfo.trdconflictInfo.pstTrdConflictedArray[i].ulLen = pstTrdConflict->trdconflictInfo->stTrdConflictedArray[i].ulLen;
						strcpy(trdconflictEventInfo.trdconflictInfo.pstTrdConflictedArray[i].szTrdConflictedSvcName, (char *)pstTrdConflict->trdconflictInfo->stTrdConflictedArray[i].szTrdConflictedSvcName);
						strcat(trdconflictEventInfo.trdconflictStr, (char *)pstTrdConflict->trdconflictInfo->stTrdConflictedArray[i].szTrdConflictedSvcName);
						strcat(trdconflictEventInfo.trdconflictStr, ",");
				
					}
				
					onTrdConflictEvent(humax::eTRDCONFLICT_GET_INFO, pstTrdConflict->ulTrdConflictCount,&trdconflictEventInfo);
					
				}
				else if (trdconflictEventInfo.trdconflictInfo.ulArrayNum == 0) {
					onTrdConflictEvent(humax::eTRDCONFLICT_GET_INFO,0/*pstTrdConflict->ulTrdConflictCount*/, &trdconflictEventInfo);
				}

			}
			
			if(pstTrdConflict->trdconflictInfo)
				HLIB_MEM_Free(pstTrdConflict->trdconflictInfo);
	
			HLIB_MEM_Free(pstTrdConflict);

			break;

		default:
			break;
	}
}






#endif // OIPF


#define ____HMX_TrdConflict_functions____

hTrdConflictEventListener global_hTrdConflictListener = NULL;	// global listener variable for channel import export


OOIFReturnCode native_HTrdConflict_GetTrdConflict(hTrdConflictEventListener listener,unsigned int depth)
{
	native_HTrdConflictSetListener(listener);
	OPL_HMXTrdConflictInfo(native_HTrdConflictEventListener,depth);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HTrdConflict_SetTrdConflict(unsigned int depth, unsigned int index)
{
	OPL_HMXSetTrdConflictInfo(depth, index);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HTrdConflictSetListener(hTrdConflictEventListener listener)
{
	global_hTrdConflictListener = listener;
	return OOIF_RETURN_OK;
}

void native_HTrdConflictEventListener(void *pstTrdConflict)
{

	if (global_hTrdConflictListener)
	{
		global_hTrdConflictListener(pstTrdConflict);
	}
}


