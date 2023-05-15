#include "corsair_native_prism_msg_sender.h"
#include "hapi.h"

CorsairNativePrismMsgSender::CorsairNativePrismMsgSender()
{

}

CorsairNativePrismMsgSender::~CorsairNativePrismMsgSender()
{

}

bool CorsairNativePrismMsgSender::responseApplicationStarted()
{
    HAPI_BroadcastSignal("signal:onApplicationStarted", 0, "i", true); 
    return true; 
}

bool CorsairNativePrismMsgSender::responseApplicationStopped()
{
    HAPI_BroadcastSignal("signal:onApplicationStopped", 0, "i", true); 
    return true; 
}
