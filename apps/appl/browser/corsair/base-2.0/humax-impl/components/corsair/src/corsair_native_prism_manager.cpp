#include "corsair_native_prism_manager.h"
#include "corsair_native_prism_msg_sender.h"


CorsairNativePrismManager::CorsairNativePrismManager()
    : m_native_event_receiver(new CorsairNativePrismEventReceiver(this)) \
    , m_native_msg_sender(new CorsairNativePrismMsgSender())
{

}

CorsairNativePrismManager::~CorsairNativePrismManager()
{
    if( m_native_event_receiver )
    {
        delete m_native_event_receiver;
        m_native_event_receiver = NULL;
    }

    if( m_native_msg_sender )
    {
        delete m_native_msg_sender; 
        m_native_msg_sender = NULL;
    }
}

bool CorsairNativePrismManager::onReqStartApplication(const char *url)
{
    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if( !hbbtv_manager ) return false;

    hbbtv_manager->action(new CorsairHbbTVBIAppStartAction(url, hbbtv_manager));

    return true;
}

bool CorsairNativePrismManager::onReqStopApplication()
{
    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if( !hbbtv_manager ) return false;

    hbbtv_manager->action(new CorsairHbbTVStopAction(CorsairHbbTVApp::HBBTV_APPTYPE_BI, hbbtv_manager));

    return true; 
}

bool CorsairNativePrismManager::sendMessageToNativePrism(NativePrism_MsgType msg, void* data)
{
    switch (msg)
    {
        case NativePrism_Started :
            m_native_msg_sender->responseApplicationStarted();
            break;
        case NativePrism_Stopped :
            m_native_msg_sender->responseApplicationStopped();
            break;
        default :
            break;
    }

    return true;
}

