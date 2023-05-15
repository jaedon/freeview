#ifdef OIPF
#include "HSoftwareUpdate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdarg.h>
#include <hlib.h>
#include <dlib.h>
#include <hapi.h>

#include "jsplugin.h"

#include "macros.h"

#include "NativeHConfiguration.h"

#include "oplswupdate.h"
#include "HSwUpdateEvent.h"

#include "HSwupDetectEvent.h"
#include "HSwupUpdateEvent.h"

using namespace Ooif;

//##########################################

list<HSoftwareUpdate*> HSoftwareUpdate::instances;

// 아래 두 static 은 oplswupdate 에서 관리되어야 하지만 현재는 여기다가 그냥 정의해 사용.
// 옮겨야함.
_OPL_SWUPDATE_Info_t HSoftwareUpdate::s_stBplSwupdateInfo;

DxSwUpdate_DetectResultTuning_t	HSoftwareUpdate::s_stTunningInfo;

HSoftwareUpdate::HSoftwareUpdate() : EventTarget()
{
	ENTRY;
    
	classType = class_name;
	HSoftwareUpdate::instances.push_back(this);
}


HSoftwareUpdate::~HSoftwareUpdate()
{
	ENTRY;
	HSoftwareUpdate::instances.remove(this);

}

int HSoftwareUpdate::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
    ENTRY;

    // Detection Method.
    GET_FUNCTION(startDetect, 	obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(cancelDetect, 	obj, "", JSP_GET_VALUE_CACHE);
    // SW Download Method.
    GET_FUNCTION(startDownload, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(cancelDownload,obj, "", JSP_GET_VALUE_CACHE);
    // SW Installation Method.
    GET_FUNCTION(startInstall, 	obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(cancelInstall, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(restartSystem, obj, "", JSP_GET_VALUE_CACHE);

    GET_FUNCTION(swupdateInit, obj, "", JSP_GET_VALUE_CACHE);

    // Detection Method with type, Type 별로 OTA를 실행한다.
    GET_FUNCTION(startDetectWithType,	obj, "n", JSP_GET_VALUE_CACHE);

    // deprecated things...
    GET_FUNCTION(startUpdate, 	obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(cancelUpdate, 	obj, "", JSP_GET_VALUE_CACHE);

    // manual detection method.
    GET_FUNCTION(startManualDetect, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(cancelManualDetect,obj, "", JSP_GET_VALUE_CACHE);

    // start s/w update state
    GET_FUNCTION(startSwupdateState, obj, "nn", JSP_GET_VALUE_CACHE);

    GET_NUMBER("Frequency", 	s_stTunningInfo.ulFrequency, 	JSP_GET_VALUE_CACHE);
    GET_NUMBER("SymbolRate", 	s_stTunningInfo.ulSymbolRate, 	JSP_GET_VALUE_CACHE);
    GET_NUMBER("Polarization", 	s_stTunningInfo.nPolarization, 	JSP_GET_VALUE_CACHE);
    GET_NUMBER("TransSpec", 	s_stTunningInfo.nTransSpec, 	JSP_GET_VALUE_CACHE);
    GET_NUMBER("PskMod", 		s_stTunningInfo.nPskMod, 		JSP_GET_VALUE_CACHE);
    GET_NUMBER("FecCodeRate", 	s_stTunningInfo.nFecCodeRate, 	JSP_GET_VALUE_CACHE);

    // flag for checking bootup humax ota version
    NATIVE_GET_STRING("verBootupHumaxOta", native_HSwUpdateGetVerBootupHumaxOta(&RETVAL), JSP_GET_VALUE);

    INTRINSIC_EVENT_GET(DetectEvent);
    INTRINSIC_EVENT_GET(UpdateProgress);

    // network Id for OTA detection
    NATIVE_GET_NUMBER("networkId", native_HSwUpdateGetNetworkId(&RETVAL), JSP_GET_VALUE);

    return JSP_GET_NOTFOUND;
}

int HSoftwareUpdate::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
    ENTRY;

    INTRINSIC_EVENT(UpdateEvent);
    INTRINSIC_EVENT(DetectEvent);
    INTRINSIC_EVENT(UpdateProgress);

    // network Id for OTA detection
    NATIVE_SET("networkId", native_HSwUpdateSetNetworkId(NUMBER_VALUE(value[0])));

    return JSP_PUT_NOTFOUND;
}

void HSoftwareUpdate::hapi_broadcastIsOtaSignal(bool isOta)
{
	HAPI_BroadcastSignal("signal:onOtaStateChanged", 0, "i", isOta);
}

int HSoftwareUpdate::swupdateInit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	native_HSoftwareUpdateInit(onSwupdateEvent);
	return 0;
}

// OTA를 여러 Type 별로 진행이 가능 하도록 하기위해 startDetect()를 변형 하여 만듦.
int HSoftwareUpdate::startDetectWithType(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int nOtaType = -1;

	if ( s_stBplSwupdateInfo.hSwupdate )		// s/w date 중이면 아래에 새 instance 를 만들라고 할수없다.
		return 0;

	if(argc < 1) // argument 개수를 체크 ...
		return 0;

	nOtaType = (int)NUMBER_VALUE(argv[0]);
	OOIF_LOG_DEBUG("Ota type is %d \n", nOtaType);

	s_stBplSwupdateInfo.hSwupdate	=	OPL_Swupdate_New_withType(nOtaType);

	if(s_stBplSwupdateInfo.hSwupdate)					// if swupdate module info create is successed
	{
		OOIF_LOG_DEBUG(" Start Detect FOR SoftwareUpdate ");
		native_HSoftwareUpdate_StartDetect(onSwupdateEvent, (hSwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
	}
	else
	{
		OOIF_LOG_DEBUG(" FAILED Start Detect FOR SoftwareUpdate ");
	}
	return 0;
}

int HSoftwareUpdate::startDetect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	if ( s_stBplSwupdateInfo.hSwupdate )		// s/w date 중이면 아래에 새 instance 를 만들라고 할수없다.
		return 0;

	s_stBplSwupdateInfo.hSwupdate	=	OPL_Swupdate_New();

	if(s_stBplSwupdateInfo.hSwupdate)					// if swupdate module info create is successed
	{
		OOIF_LOG_DEBUG(" Start Detect FOR SoftwareUpdate ");
		native_HSoftwareUpdate_StartDetect(onSwupdateEvent, (hSwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
	}
	else
	{
		OOIF_LOG_DEBUG(" FAILED Start Detect FOR SoftwareUpdate ");
	}
	return 0;
}

int HSoftwareUpdate::startSwupdateState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int nState = -1;

	if(argc < 1 || s_stBplSwupdateInfo.hSwupdate)
		return 0;

	nState = (int)NUMBER_VALUE(argv[1]);
	OOIF_LOG_DEBUG("Ota state is %d \n", nState);
	switch(nState)
	{
		case eDxSWUPDATE_STEP_Install:
			{
				int nOtaType = -1;

				nOtaType = (int)NUMBER_VALUE(argv[0]);
				OOIF_LOG_DEBUG("Ota type is %d \n", nOtaType);

				s_stBplSwupdateInfo.hSwupdate	=	OPL_Swupdate_New_withType(nOtaType);

				if(s_stBplSwupdateInfo.hSwupdate)					// if swupdate module info create is successed
				{
					OOIF_LOG_DEBUG(" Start s/w update state install.... \n");
					hapi_broadcastIsOtaSignal(true);
					native_HSoftwareUpdate_StartInstall(onSwupdateEvent, (hSwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
				}
				else
				{
					OOIF_LOG_WARNING(" FAILED Start s/w update state install!! ");
				}
			}
			break;

		default:
			break;
	}
	return 0;
}

int HSoftwareUpdate::cancelDetect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	if (s_stBplSwupdateInfo.hSwupdate)
	{
		hapi_broadcastIsOtaSignal(false);
		OPL_StopSwupdateDetect((OPL_SwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
		s_stBplSwupdateInfo.hSwupdate = NULL;
	}
	return 0;
}

int HSoftwareUpdate::startManualDetect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	if (s_stBplSwupdateInfo.hSwupdate)
	{
		OPL_StopSwupdateDetect((OPL_SwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
		s_stBplSwupdateInfo.hSwupdate = NULL;
	}

	if (NUMBER_VALUE(argv[0]) == eDxDELIVERY_TYPE_SAT)
	{
		s_stBplSwupdateInfo.hSwupdate	=	OPL_StartSwupdateManualDetectForSat(
															NUMBER_VALUE(argv[1]) ,	/* Frequency */
															NUMBER_VALUE(argv[2]) ,	/* Polarization */
															NUMBER_VALUE(argv[3]) , /* Symbol Rate */
															NUMBER_VALUE(argv[4]) , /* Transmission */
															NUMBER_VALUE(argv[5]) , /* psk mode */
															NUMBER_VALUE(argv[6])   /* FEC Code Rate */
															);
	}
	else if (NUMBER_VALUE(argv[0]) == eDxDELIVERY_TYPE_CAB)
	{
		s_stBplSwupdateInfo.hSwupdate	=	OPL_StartSwupdateManualDetectForCab(
															NUMBER_VALUE(argv[1]) ,	/* Frequency */
															NUMBER_VALUE(argv[2]) ,	/* Constellation */
															NUMBER_VALUE(argv[3]) 	/* Symbol Rate */
															);
	}
	else if (NUMBER_VALUE(argv[0]) == eDxDELIVERY_TYPE_TER)
	{
		// TODO:
	}

	if(s_stBplSwupdateInfo.hSwupdate)					// if swupdate module info create is successed
	{
		OOIF_LOG_DEBUG(" Start Manual Detect FOR SoftwareUpdate ");
		native_HSoftwareUpdate_StartDetect(onSwupdateEvent, (hSwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
	}
	return 0;
}

int HSoftwareUpdate::cancelManualDetect(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	return 0;
}

int HSoftwareUpdate::startDownload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int nOtaType = -1;

	nOtaType = (int)NUMBER_VALUE(argv[0]);

	s_stBplSwupdateInfo.hSwupdate	=	OPL_Swupdate_New_withType(nOtaType);

	if (s_stBplSwupdateInfo.hSwupdate)
	{
		OOIF_LOG_DEBUG(" Start Update FOR SoftwareUpdate ");
		hapi_broadcastIsOtaSignal(true);
		native_HSoftwareUpdate_StartDownload(onSwupdateEvent, (hSwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
	}
	return 0;
}

int HSoftwareUpdate::cancelDownload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	if (s_stBplSwupdateInfo.hSwupdate)
	{
		hapi_broadcastIsOtaSignal(false);
		OPL_StopSwupdateDownload((OPL_SwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
		s_stBplSwupdateInfo.hSwupdate = NULL;
	}
	return 0;
}

int HSoftwareUpdate::startInstall(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	if (s_stBplSwupdateInfo.hSwupdate)
	{
		OOIF_LOG_DEBUG(" Start Update FOR SoftwareUpdate ");
		hapi_broadcastIsOtaSignal(true);
		native_HSoftwareUpdate_StartInstall(onSwupdateEvent, (hSwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
	}
	return 0;
}

int HSoftwareUpdate::cancelInstall(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	if (s_stBplSwupdateInfo.hSwupdate)
	{
		hapi_broadcastIsOtaSignal(false);
		OPL_StopSwupdateInstall((OPL_SwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
		s_stBplSwupdateInfo.hSwupdate = NULL;
	}
	return 0;
}

int HSoftwareUpdate::restartSystem(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OPL_SystemRestartSwUpdate();
	s_stBplSwupdateInfo.hSwupdate =	NULL;

	return 0;
}

void HSoftwareUpdate::onSwupdateEvent(DxSwUpdate_Event_e eEvent, DxSwUpdate_EventData_t *pstEventData)
{
	OOIF_LOG_DEBUG(" Received SoftwareUpdate Message : 0X%x ", eEvent);

	for (list<HSoftwareUpdate*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();

		switch(eEvent)
		{
			case eDxSWUPDATE_EVENT_DetectSuccess:
			case eDxSWUPDATE_EVENT_DetectSuccessInLive:
			case eDxSWUPDATE_EVENT_DetectFail:
			case eDxSWUPDATE_EVENT_DownloadSuccess:
			case eDxSWUPDATE_EVENT_DownloadProgress:
			case eDxSWUPDATE_EVENT_DownloadFail:
			case eDxSWUPDATE_EVENT_InstallSuccess:
			case eDxSWUPDATE_EVENT_InstallProgress:
			case eDxSWUPDATE_EVENT_InstallFail:
				e = new HSwUpdateEvent(context, pstEventData);
				break;

			case eDxSWUPDATE_EVENT_Erorr:
			case eDxSWUPDATE_EVENT_Finish:
			case eDxSWUPDATE_EVENT_ConnectSuccess:
			case eDxSWUPDATE_EVENT_ConnectFail:
				e = new HSwUpdateEvent(context, pstEventData);

				// new si 이후 새로운 구조에서는, 매 state 마다 , handle 을 새로 만들어야 한다.
				s_stBplSwupdateInfo.hSwupdate = NULL;
				break;

			default:
				break;
		}

		(*it)->dispatchEvent(e);
	}
}

int HSoftwareUpdate::startUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	if (s_stBplSwupdateInfo.hSwupdate)
	{
		OOIF_LOG_DEBUG(" Start Update FOR SoftwareUpdate ");
		hapi_broadcastIsOtaSignal(true);
		native_HSoftwareUpdate_StartDownload(onSwupdateEvent, (hSwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
	}
	return 0;
}

int HSoftwareUpdate::cancelUpdate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	if (s_stBplSwupdateInfo.hSwupdate)
	{
		hapi_broadcastIsOtaSignal(false);
		OPL_StopSwupdateUpdate((OPL_SwupdateHandle)s_stBplSwupdateInfo.hSwupdate);
		s_stBplSwupdateInfo.hSwupdate = NULL;
	}
	return 0;
}

#endif // OIPF

#define ___CSwUpdateInfo_Class___

CSwUpdateInfo::CSwUpdateInfo(DxSwUpdate_EventData_t *pstEventInfo)
{
	classType = CLASS_SOFTWARE_UPDATEINFO;

	m_EventInfo = (DxSwUpdate_EventData_t *)HLIB_STD_MemAlloc(sizeof(DxSwUpdate_EventData_t));
	memcpy(m_EventInfo, pstEventInfo, sizeof(DxSwUpdate_EventData_t));
}


CSwUpdateInfo::~CSwUpdateInfo()
{
	if (m_EventInfo)
		HLIB_STD_MemFree(m_EventInfo);
}

int CSwUpdateInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(getInfo, obj, "s", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int CSwUpdateInfo::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

int	CSwUpdateInfo::getInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	const	char	*type;

	CHECK_ARGUMENT_COUNT(1);

	CSwUpdateInfo	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, CSwUpdateInfo);

	type = STRING_VALUE(argv[0]);

	if (instance->m_EventInfo)
	{
		DxSwUpdate_EventData_t *pstEvent = instance->m_EventInfo;

		if (strstr(type, "event")) {
			RETURN_NUMBER(pstEvent->eEvent, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "source_type") == 0) {
			RETURN_NUMBER(pstEvent->eSourceType, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "signal_type") == 0) {
			RETURN_NUMBER(pstEvent->eSignalType, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "method_type") == 0) {
			RETURN_NUMBER(pstEvent->eMethodType, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "state") == 0) {
			RETURN_NUMBER(pstEvent->eState, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "current_version") == 0) {
			RETURN_STRING(pstEvent->unEventData.stDetectEvent.szCurrentVer, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "found_version") == 0) {
			RETURN_STRING(pstEvent->unEventData.stDetectEvent.szFoundVer, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "progress") == 0) {
			RETURN_NUMBER(pstEvent->unEventData.stProgressEvent.nProgress, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "progress_max") == 0) {
			RETURN_NUMBER(pstEvent->unEventData.stProgressEvent.nProgressMax, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "ota_type") == 0) {
			RETURN_NUMBER(pstEvent->unExEventData.stExSwupTypeData.ucSwupType, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "start_time") == 0) {
			RETURN_NUMBER(pstEvent->unExEventData.stExSwupTypeData.ulStartTime, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "end_time") == 0) {
			RETURN_NUMBER(pstEvent->unExEventData.stExSwupTypeData.ulEndTime, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "ir_dntime") == 0) {
			RETURN_NUMBER(pstEvent->unExEventData.stExSwupIrdetoData.nDnTimeInSec, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "ir_seq_no") == 0) {
			RETURN_NUMBER(pstEvent->unExEventData.stExSwupIrdetoData.ucLoadSeqNumber, JSP_CALL_VALUE);
		}
		else if (strcmp(type, "ir_tableid") == 0) {
			RETURN_NUMBER(pstEvent->unExEventData.stExSwupIrdetoData.ucTableId, JSP_CALL_VALUE);
		}
	}

	RETURN_UNDEFINED(JSP_CALL_VALUE);
}


#define ____HMX_SoftwareUpdate_functions____

hSwupdateEventListener global_hSwupdateListener = NULL;				// global listener variable for software upate

OOIFReturnCode native_HSoftwareUpdateInit(hSwupdateEventListener listener)
{
	native_HSwUpdateSetListener(listener);
	OPL_SwupdateInit(native_HSwupdateEventListener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HSoftwareUpdate_StartDetect(hSwupdateEventListener listener, hSwupdateHandle hSwupHandle)
{
	HSoftwareUpdate::hapi_broadcastIsOtaSignal(true);
	native_HSwUpdateSetListener(listener);
	OPL_StartSwupdateDetect(native_HSwupdateEventListener, hSwupHandle);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HSoftwareUpdate_StartDownload(hSwupdateEventListener listener, hSwupdateHandle hSwupHandle)
{
	native_HSwUpdateSetListener(listener);
	OPL_StartSwupdateDownload(native_HSwupdateEventListener, hSwupHandle);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HSoftwareUpdate_StartInstall(hSwupdateEventListener listener, hSwupdateHandle hSwupHandle)
{
	native_HSwUpdateSetListener(listener);
	OPL_StartSwupdateInstall(native_HSwupdateEventListener, hSwupHandle);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HSwUpdateSetListener(hSwupdateEventListener listener)
{
	global_hSwupdateListener = listener;
	return OOIF_RETURN_OK;
}

// ro String version of Bootup Humax OTA
OOIFReturnCode native_HSwUpdateGetVerBootupHumaxOta(const char **retval)
{
	if (NULL == retval)
		return OOIF_RETURN_VALUE_NULL;

	static char szVerBootupHumaxOta[SWUPDATE_VER_LEN];

	memset(szVerBootupHumaxOta,0,SWUPDATE_VER_LEN);

	OPL_StartSwupdateGetVersionBootupHumaxOta(szVerBootupHumaxOta);

	*retval = szVerBootupHumaxOta;

	return OOIF_RETURN_OK;
}

// Integer regionId
OOIFReturnCode native_HSwUpdateGetNetworkId(OOIFNumber *retval)
{
    if (NULL == retval)
    {
    	return OOIF_RETURN_VALUE_UNDEFINED;
    }

    int networkId = 0;

    OPL_SwUpdateGetNetworkId(&networkId);

    *retval = (OOIFNumber)networkId;

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_HSwUpdateSetNetworkId(OOIFNumber value)
{

    OPL_SwUpdateSetNetworkId((int)value);

    return OOIF_RETURN_OK;
}

void native_HSwupdateEventListener(DxSwUpdate_EventData_t *pstEventData)
{
	if ( !pstEventData )
	{
		OOIF_LOG_DEBUG(" EventData is NULL\n ");
		return ;
	}

	if (global_hSwupdateListener)
	{
		DxSwUpdate_EventData_t *pstEvent = (DxSwUpdate_EventData_t*)pstEventData;
		DxSwUpdate_EventData_t eventInfo;

		memset(&eventInfo, 0, sizeof(DxSwUpdate_EventData_t));
		memcpy(&eventInfo, pstEvent, sizeof(DxSwUpdate_EventData_t));

		// 이전 구조로 인해,,,, 앞에 param0 event type 은 일단 유지..
		global_hSwupdateListener(eventInfo.eEvent, &eventInfo);
	}
}


