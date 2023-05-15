
#ifndef HMXAPPLICATIONMANAGER_H_
#define HMXAPPLICATIONMANAGER_H_

#include "ObjectInstance.h"
#include "HDataTypes.h"

#include "macros.h"
#include "OOIFUtils.h"
#include "opllocalsystem.h"
#include "NativeApplicationManager.h"

typedef void (applicationManagerHumaxAppListener)(int event, const char *app_name);

namespace Ooif
{
class HMXApplicationManager : public EventTarget 
{ 
    public:
        HMXApplicationManager();
        ~HMXApplicationManager();

        static void onHumaxApplicationManagerEvent(int event, const char *app_name);

    private:
        virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
        virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

        static int createPortalApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int destroyPortalApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int addOIPFApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int removeOIPFApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int removeAllOIPFApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int createHbbTVApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int destroyHbbTVApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int doTeletext(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int getHumaxApplications(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int createHumaxApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static int destroyHumaxApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
        static const char *GetEventName(ApplicationManagerEvent event);

		static const ClassType class_name = CLASS_HMX_APPLICATIONMANAGER;

        static list<HMXApplicationManager*> instances;
        static pthread_mutex_t instancesMutex;
};
}
#endif
