#include "corsair_native_prism_event_receiver.h"


ICorsairNativePrismEventListener* CorsairNativePrismEventReceiver::m_listener = NULL;

CorsairNativePrismEventReceiver::CorsairNativePrismEventReceiver(ICorsairNativePrismEventListener *listener)
{ 
    HAPI_ConnectSignal("signal:onReqStartApplication",  NULL, CorsairNativePrismEventReceiver::onReqStartApplication);
    HAPI_ConnectSignal("signal:onReqStopApplication",  NULL, CorsairNativePrismEventReceiver::onReqStopApplication); 

    m_listener = listener; 
}

CorsairNativePrismEventReceiver::~CorsairNativePrismEventReceiver()
{
    HAPI_DisconnectSignal("signal:onReqStartApplication", CorsairNativePrismEventReceiver::onReqStartApplication);
    HAPI_DisconnectSignal("signal:onReqStopApplication", CorsairNativePrismEventReceiver::onReqStopApplication); 

    m_listener = NULL; 
}

HERROR CorsairNativePrismEventReceiver::onReqStartApplication(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    // TODO : parse apArgv[]
    // Check : HxLOG_Assert(nArgc == 1);
    // Integer : (void*)HxOBJECT_INT(apArgv[0]);
    // String : (char*)HxOBJECT_STR(apArgv[0]) 

    HxLOG_Assert(nArgc == 1);
    
    return m_listener->onReqStartApplication((char*)HxOBJECT_STR(apArgv[0]));
}

HERROR CorsairNativePrismEventReceiver::onReqStopApplication(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{

    return m_listener->onReqStopApplication(); 
}

