#ifndef __CORSAIR_NATIVE_PRISM_EVENT_RECEIVER__
#define __CORSAIR_NATIVE_PRISM_EVENT_RECEIVER__

#include "hapi.h"

class ICorsairNativePrismEventListener
{
public:
    virtual bool onReqStartApplication(const char *url) = 0;
    virtual bool onReqStopApplication() = 0; 
};

class CorsairNativePrismEventReceiver 
{
public:
    CorsairNativePrismEventReceiver(ICorsairNativePrismEventListener *listener);
    virtual ~CorsairNativePrismEventReceiver();

    static HERROR onReqStartApplication(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
    static HERROR onReqStopApplication(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);

private:
    static ICorsairNativePrismEventListener *m_listener; 
};
#endif
