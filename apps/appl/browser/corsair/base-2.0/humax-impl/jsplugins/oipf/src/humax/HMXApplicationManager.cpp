
#include <string.h>
#include <stdio.h>
#include <dlib.h>
#include <map>

#include "EmptyEvent.h"
#include "HMXApplicationManager.h"
#include "HumaxApplicationLoadedEvent.h"
#include "HumaxApplicationUnloadedEvent.h"
#include "NativeApplicationManager.h"

using namespace Ooif;

pthread_mutex_t HMXApplicationManager::instancesMutex = PTHREAD_MUTEX_INITIALIZER;
list<HMXApplicationManager*> HMXApplicationManager::instances;

HMXApplicationManager::HMXApplicationManager()
{
    classType = CLASS_HMX_APPLICATIONMANAGER;
	pthread_mutex_lock(&instancesMutex);
    if( instances.size() == 0 ) /* if there are some app mgr instance, do not need to set a callback again */
        native_AppManSetHumaxApplicationListener(HMXApplicationManager::onHumaxApplicationManagerEvent);

    HMXApplicationManager::instances.push_back(this);
    pthread_mutex_unlock(&instancesMutex);
}

HMXApplicationManager::~HMXApplicationManager()
{
    pthread_mutex_lock(&instancesMutex);
    HMXApplicationManager::instances.remove(this);
    if( HMXApplicationManager::instances.size() == 0 ) /* if there is no more than one, now we can delete the callback. */
        native_AppManSetHumaxApplicationListener(NULL);
    pthread_mutex_unlock(&instancesMutex);
}

int HMXApplicationManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
    GET_FUNCTION(createPortalApplication, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(destroyPortalApplication, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(addOIPFApplication, obj, "s", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(removeOIPFApplication, obj, "s", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(removeAllOIPFApplications, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(createHbbTVApplication, obj, "n", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(destroyHbbTVApplication, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(doTeletext, obj, "", JSP_GET_VALUE_CACHE);
    NATIVE_GET_NUMBER("HBBTV_APP_TYPE_AUTOSTART", native_GetHbbTVApplicationType(name, &RETVAL), JSP_GET_VALUE_CACHE);
    NATIVE_GET_NUMBER("HBBTV_APP_TYPE_INDEPENDENT", native_GetHbbTVApplicationType(name, &RETVAL), JSP_GET_VALUE_CACHE);
    NATIVE_GET_NUMBER("HBBTV_APP_TYPE_DIGITALTELETEXT", native_GetHbbTVApplicationType(name, &RETVAL), JSP_GET_VALUE_CACHE);

    /* this is for the humax specified apps like otvs, hbbtv broadcast independent app, full browser and etc */
    GET_FUNCTION(getHumaxApplications, obj, "", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(createHumaxApplication, obj, "ss", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(destroyHumaxApplication, obj, "s", JSP_GET_VALUE_CACHE);
    INTRINSIC_EVENT_GET(HumaxApplicationLoaded);
    INTRINSIC_EVENT_GET(HumaxApplicationUnloaded);

    return EventTarget::getter(obj, name, result);
}

int HMXApplicationManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
    INTRINSIC_EVENT(HumaxApplicationLoaded);
    INTRINSIC_EVENT(HumaxApplicationUnloaded);
	return JSP_PUT_NOTFOUND;
}

int HMXApplicationManager::createPortalApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(0);
    NATIVE_CALL(native_CreatePortalApplication());
    return JSP_CALL_NO_VALUE;
}

int HMXApplicationManager::destroyPortalApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(0);
    NATIVE_CALL(native_DestroyPortalApplication());
    return JSP_CALL_NO_VALUE;
}

int HMXApplicationManager::addOIPFApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(1);
    CHECK_ARGUMENT(0, JSP_TYPE_STRING);
    OOIFBoolean retval;
    NATIVE_CALL(native_AddOIPFApplication(argv[0].u.string, &retval));
    RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

    return JSP_CALL_VALUE;
}

int HMXApplicationManager::removeOIPFApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(1);
    CHECK_ARGUMENT(0, JSP_TYPE_STRING);
    OOIFBoolean retval;
    NATIVE_CALL(native_RemoveOIPFApplication(argv[0].u.string, &retval));
    RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

    return JSP_CALL_NO_VALUE;
}

int HMXApplicationManager::removeAllOIPFApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(0);
    NATIVE_CALL(native_RemoveAllOIPFApplications());
    return JSP_CALL_NO_VALUE;
}

int HMXApplicationManager::createHbbTVApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(1);
    NATIVE_CALL(native_CreateHbbTVApplication(argv[0].u.number));
    return JSP_CALL_NO_VALUE;
}

int HMXApplicationManager::destroyHbbTVApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(0);
    NATIVE_CALL(native_DestroyHbbTVApplication());
    return JSP_CALL_NO_VALUE;
}

int HMXApplicationManager::doTeletext(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(0);
    NATIVE_CALL(native_DoTeletext());
    return JSP_CALL_NO_VALUE;
}

int HMXApplicationManager::getHumaxApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
    char **apps = NULL;
    int count = 0;
    NATIVE_CALL(native_AppManGetHumaxApplications(&count, &apps));
    RETURN_OBJECT(this_obj, OOIFUtils::CreateStringCollection(count, apps), result, JSP_CALL_VALUE);

    return JSP_CALL_VALUE;
}

int HMXApplicationManager::createHumaxApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(2);
    NATIVE_CALL(native_AppManCreateHumaxApplication(STRING_VALUE(argv[0]), STRING_VALUE(argv[1])));
    return JSP_CALL_NO_VALUE;
}

int HMXApplicationManager::destroyHumaxApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
    CHECK_ARGUMENT_COUNT(1);
    NATIVE_CALL(native_AppManDestroyHumaxApplication(argv[0].u.string));
    return JSP_CALL_NO_VALUE;
}

const char* HMXApplicationManager::GetEventName(ApplicationManagerEvent event)
{
    switch (event)
    {
    case MANAGER_EVENT_LOW_MEMORY:
        return "MANAGER_EVENT_LOW_MEMORY";
    case MANAGER_EVENT_APPLICATION_LOADED:
        return "MANAGER_EVENT_APPLICATION_LOADED";
    case MANAGER_EVENT_APPLICATION_UNLOADED:
        return "MANAGER_EVENT_APPLICATION_UNLOADED";
    case MANAGER_EVENT_APPLICATION_ACTIVATED:
        return "MANAGER_EVENT_APPLICATION_ACTIVATED";
    case MANAGER_EVENT_APPLICATION_DEACTIVATED:
        return "MANAGER_EVENT_APPLICATION_DEACTIVATED";
    case MANAGER_EVENT_APPLICATION_SHOWN:
        return "MANAGER_EVENT_APPLICATION_SHOWN";
    case MANAGER_EVENT_APPLICATION_HIDDEN:
        return "MANAGER_EVENT_APPLICATION_HIDDEN";
    case MANAGER_EVENT_APPLICATION_PRIMARY_RECEIVER:
        return "MANAGER_EVENT_APPLICATION_PRIMARY_RECEIVER";
    case MANAGER_EVENT_APPLICATION_NOT_PRIMARY_RECEIVER:
        return "MANAGER_EVENT_APPLICATION_NOT_PRIMARY_RECEIVER";
    case MANAGER_EVENT_APPLICATION_TOPMOST:
        return "MANAGER_EVENT_APPLICATION_TOPMOST";
    case MANAGER_EVENT_APPLICATION_NOT_TOPMOST:
        return "MANAGER_EVENT_APPLICATION_NOT_TOPMOST";
    case MANAGER_EVENT_APPLICATION_DESTROY_REQUEST:
        return "MANAGER_EVENT_APPLICATION_DESTROY_REQUEST";
    case MANAGER_EVENT_APPLICATION_HIBERNATE_REQUEST:
        return "MANAGER_EVENT_APPLICATION_HIBERNATE_REQUEST";
    default:
        return "UNKNOWN";
    }
}
//
void HMXApplicationManager::onHumaxApplicationManagerEvent(int event, const char *app_name)
{
    Event *e = NULL;
    std::map<EventTarget*, Event*> targets;

    OOIF_LOG_DEBUG("ApplicationManager event received: %s, app_name: %p", GetEventName((ApplicationManagerEvent)event), app_name);

    // Iterate over all instances and dispatch the event
    pthread_mutex_lock(&instancesMutex);

    for (list<HMXApplicationManager*>::iterator it=instances.begin(); it!=instances.end(); it++)
    {
        jsplugin_obj *context = (*it)->getHost();
        EventTarget *target;

        switch (event)
        {
            case MANAGER_EVENT_APPLICATION_LOADED:
                e = new HumaxApplicationLoadedEvent(app_name, context);
                target = *it;
                break;
            case MANAGER_EVENT_APPLICATION_UNLOADED:
                e = new HumaxApplicationUnloadedEvent(app_name, context);
                target = *it;
                break;
			case MANAGER_EVENT_APPLICATION_NAGRA_PRM_START:
				e = new EmptyEvent(context, "NagraPrmStarted");
				target = *it;
				break;
			case MANAGER_EVENT_APPLICATION_NAGRA_PRM_STOP:
				e = new EmptyEvent(context, "NagraPrmStopped");
				target = *it;
				break;

            default:
                break;
        }
        if (!targets.insert(std::make_pair(target, e)).second)
        {
            OOIF_LOG_DEBUG("delete event");
            delete e; // target was not added (because it is a duplicate), thus freeing mem.
        }

    }
    for (std::map<EventTarget*, Event*>::iterator it = targets.begin(); it != targets.end(); ++it)
    {
        if ( it->first && it->second )
            it->first->dispatchEvent(it->second);
    }

    pthread_mutex_unlock(&instancesMutex);
}
