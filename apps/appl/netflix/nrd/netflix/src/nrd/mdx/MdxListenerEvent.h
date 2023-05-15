/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MDX_LISTENER_EVENT_H
#define MDX_LISTENER_EVENT_H

#include "MdxLog.h"
#include "MdxError.h"
#include <nrdbase/Variant.h>

#include <cassert>

namespace netflix {

namespace mdx {
using namespace std;

/**
 * @class MdxListenerEvent MdxListenerEvent.h
 * @brief Parent of all events that can be fired to the listeners.
 */
class MdxListenerEvent
{
public:
    virtual ~MdxListenerEvent() {}

    virtual void fireTo(IMdx::Listener* listener) const { NRDP_UNUSED(listener); }
};

/**
 * @class StateChangedEvent MdxListenerEvent.h
 */
class StateChangedEvent : public MdxListenerEvent
{
private:
    IMdx::State state_;
    const string& ssid_;
    int port_;

public:
    StateChangedEvent(IMdx::State state, const string& ssid, int port) : state_(state), ssid_(ssid), port_(port) {}

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "stateChanged: new state = %s, ssid=%s, port=%d",
               (state_ == IMdx::INITIALIZED)? "INITIALIZED" :
               (state_ == IMdx::NOT_INITIALIZED)? "NOT_INITIALIZED" : "<unknown>", ssid_.c_str(),
               port_);

        listener->stateChanged(state_, ssid_, port_);
    }
};

/**
 * @class AdvertisingStateChangedEvent MdxListenerEvent.h
 */
class AdvertisingStateChangedEvent : public MdxListenerEvent
{
private:
    IMdx::AdvertisingState state_;

public:
    AdvertisingStateChangedEvent(IMdx::AdvertisingState state) : state_(state) {}

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "stateChanged: new state = %s",
               (state_ == IMdx::ADVERTISING)? "ADVERTISING" :
               (state_ == IMdx::NOT_ADVERTISING)? "NOT_ADVERTISING" : "<unknown>");

        listener->advertisingStateChanged(state_);
    }
};

/**
 * @class DiscoveryStateChangedEvent MdxListenerEvent.h
 */
class DiscoveryStateChangedEvent : public MdxListenerEvent
{
private:
    IMdx::DiscoveryState state_;

public:
    DiscoveryStateChangedEvent(IMdx::DiscoveryState state) : state_(state) {}

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "stateChanged: new state = %s",
               (state_ == IMdx::DISCOVERING)? "DISCOVERING" :
               (state_ == IMdx::NOT_DISCOVERING)? "NOT_DISCOVERING" : "<unknown>");

        listener->discoveryStateChanged(state_);
    }
};

/**
 * @class DeviceFoundEvent MdxListenerEvent.h
 */
class DeviceFoundEvent : public MdxListenerEvent
{
private:
    const string& serviceType_;
    const string& location_;
    const string& USN_;
    const string& friendlyName_;
    const vector<pair<string, string> >& headers_;
    bool activated_;
    bool paired_;

public:
    DeviceFoundEvent(const string& location,
                     const string& serviceType,
                     const string& USN,
                     const string& friendlyName,
                     const vector<pair<string, string> > &headers,
                     bool activated,
                     bool paired)
        : serviceType_(serviceType)
        , location_(location)
        , USN_(USN)
        , friendlyName_(friendlyName)
        , headers_(headers)
        , activated_(activated)
        , paired_(paired)
    {
    }

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "DeviceFoundEvent %s ", USN_.c_str());
        listener->DeviceFound(location_,
                              serviceType_,
                              USN_,
                              friendlyName_,
                              headers_,
                              activated_,
                              paired_);
    }
};


/**
 * @class DeviceByeBye MdxListenerEvent.h
 */
class DeviceByeByeEvent : public MdxListenerEvent
{
private:
    const string &  serviceType_;
    const string &  USN_;

public:
    DeviceByeByeEvent(const string &serviceType, const string& USN)
        : serviceType_(serviceType)
        , USN_(USN)
    {
    }

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "DeviceByeByeEvent: %s", USN_.c_str());

        listener->DeviceByeBye(serviceType_, USN_);
    }
};

/**
 * @class HttpResponseEvent MdxListenerEvent.h
 */
class HttpResponseEvent : public MdxListenerEvent
{
private:
    const string& url_;
    int method_;
    llong xid_;
    int32_t responseCode_;
    const string& header_;
    const string& body_;

public:
    HttpResponseEvent(const string& url,
                      int method,
                      llong xid,
                      int32_t responseCode,
                      const string& header,
                      const string& body)
        : url_(url)
        , method_(method)
        , xid_(xid)
        , responseCode_(responseCode)
        , header_(header)
        , body_(body)
    {
    }

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "HttpResponseEvent %s ", url_.c_str());
        listener->HttpResponse(url_, method_, xid_, responseCode_, header_, body_);
    }
};

/**
 * @class InterfaceChangedEvent MdxListenerEvent.h
 */
class InterfaceChangedEvent : public MdxListenerEvent
{
private:
    const string &oldInterface_;
    const string &newInterface_;
    bool connected_;

public:
    InterfaceChangedEvent(const string &oldInterface, const string &newInterface, bool connected)
        : oldInterface_(oldInterface)
        , newInterface_(newInterface)
        , connected_(connected)
    {
    }

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "InterfaceChangedEvent %s %s %s", oldInterface_.c_str(), newInterface_.c_str(), connected_ ? "connected" : "disconnected");
        listener->interfaceChanged(oldInterface_, newInterface_, connected_);
    }
};

/**
 * @class IncomingMessagedEvent MdxListenerEvent.h
 */
class IncomingMessageEvent : public MdxListenerEvent
{
private:
    const string& url_;
    Variant& headers_;
    const string& body_;

public:
    IncomingMessageEvent(const string& url, Variant& headers, const string& body)
        : url_(url)
        , headers_(headers)
        , body_(body)
    {
    }

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "IncomingMessageEvent %s ", url_.c_str());
        listener->IncomingMessage(url_, headers_, body_);
    }
 };

/**
 * @class IncomingMessagedEvent MdxListenerEvent.h
 */
class WebSocketMessageEvent : public MdxListenerEvent
{
private:
    Variant& headers_;
    const string& body_;

public:
    WebSocketMessageEvent(Variant& headers, const string& body)
        : headers_(headers)
        , body_(body)
    {
    }

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "WebSocketMessageEvent %s ", body_.c_str());
        listener->WebSocketMessage(headers_, body_);
    }
};

 /**
  * @class ErrorEvent MdxListenerEvent.h
  */
 class ErrorEvent : public MdxListenerEvent
 {
 private:
     MdxErrorCode error_;

 public:
     ErrorEvent(MdxErrorCode error) : error_(error)
     {
     }

     virtual void fireTo(IMdx::Listener *listener) const
     {
         MDXERROR("error: Error = %s", toString(error_));
         listener->error(error_);
     }
 };

 /**
  * @class DialDeviceInfoReponseEvent MdxListenerEvent.h
  */
 class DialDeviceInfoResponseEvent : public MdxListenerEvent
 {
 private:
     const string &url_;
     const string &USN_;
     const string &serviceType_;
     const Variant &responseHeaders_;
     const Variant &deviceDesc_;

 public:

     DialDeviceInfoResponseEvent(const std::string &url,
                                 const std::string &USN,
                                 const std::string &serviceType,
                                 const Variant &responseHeaders,
                                 const Variant &deviceDesc)
         : url_(url)
         , USN_(USN)
         , serviceType_(serviceType)
         , responseHeaders_(responseHeaders)
         , deviceDesc_(deviceDesc)
     {
     }

     virtual void fireTo(IMdx::Listener *listener) const
     {
         listener->DialDeviceInfoResponse(url_, USN_, serviceType_, responseHeaders_, deviceDesc_);
     }
 };

 /**
  * @class WebSocketConnectEvent MdxListenerEvent.h
  */
 class WebSocketConnectEvent : public MdxListenerEvent
 {
 private:
     const std::string &mHost;

 public:
     WebSocketConnectEvent(const std::string &host)
         : mHost(host)
     {
     }

     virtual void fireTo(IMdx::Listener *listener) const
     {
         NTRACE(TRACE_MDXLISTENER, "Websocket Connect %s", mHost.c_str());
         listener->webSocketConnect(mHost);
     }
 };

 /**
  * @class WebSocketDisconnectEvent MdxListenerEvent.h
  */
 class WebSocketDisconnectEvent : public MdxListenerEvent
 {
 private:
     const std::string &mHost;

 public:
     WebSocketDisconnectEvent(const std::string &host)
        : mHost(host)
    {
    }

     virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "Websocket Disconnect %s", mHost.c_str());
        listener->webSocketDisconnect(mHost);
    }
};

/**
 * @class WebSocketSentEvent MdxListenerEvent.h
 */
class WebSocketSentEvent : public MdxListenerEvent
{
private:
    const std::string & mHost;
    ullong            mXid;

public:
    WebSocketSentEvent(const std::string &host, ullong xid)
        : mHost(host)
        , mXid(xid)
    {
    }

    virtual void fireTo(IMdx::Listener *listener) const
    {
        NTRACE(TRACE_MDXLISTENER, "Websocket Response %s %llu", mHost.c_str(), mXid);
        listener->webSocketSent(mHost, mXid);
    }
};


}} // namespace netflix::mdx

#endif // MEDIACONTROL_LISTENER_EVENT_H
