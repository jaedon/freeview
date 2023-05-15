#ifndef __CORSAIR_NATIVE_PRISM_MANAGER__
#define __CORSAIR_NATIVE_PRISM_MANAGER__

#include "corsair.h"
#include "corsair_hbbtv_manager.h"
#include "corsair_native_prism_event_receiver.h"

class CorsairNativePrismMsgSender;

typedef enum
{
    NativePrism_Started,
    NativePrism_Stopped
} NativePrism_MsgType;

class CorsairNativePrismManager : public ICorsairNativePrismEventListener
{
public:

    static CorsairNativePrismManager* getInstance()
    {
        if( NULL == instance ) instance = new CorsairNativePrismManager();
        return instance; 
    }

    static void destroyInstance()
    {
        if( instance )
        {
            delete instance;
            instance = NULL;
        }
    }

    bool onReqStartApplication(const char *url);
    bool onReqStopApplication();
    bool sendMessageToNativePrism(NativePrism_MsgType msg, void* data);
        
private:
    CorsairNativePrismManager();
    CorsairNativePrismManager(const CorsairNativePrismManager& instance);
    ~CorsairNativePrismManager();

    static CorsairNativePrismManager* instance; 

    CorsairNativePrismEventReceiver *m_native_event_receiver;
    CorsairNativePrismMsgSender *m_native_msg_sender;
};
#endif
