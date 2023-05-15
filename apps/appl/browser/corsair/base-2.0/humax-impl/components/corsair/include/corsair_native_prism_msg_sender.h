#ifndef __CORSAIR_NATIVE_PRISM_MSG_SENDER__
#define __CORSAIR_NATIVE_PRISM_MSG_SENDER__


class CorsairNativePrismMsgSender 
{
public:
    CorsairNativePrismMsgSender();
    virtual ~CorsairNativePrismMsgSender();

    virtual bool responseApplicationStarted();
    virtual bool responseApplicationStopped(); 
};
#endif
