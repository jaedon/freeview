/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "QALocationMonitorBridge.h"

#include "../MediaControl/IAdaptiveStreamingPlayer.h"
#include <nrd/Request.h>
#include <nrd/Response.h>
#include <nrd/NrdApplication.h>

#include <sstream>

#include <nrdbase/Log.h>

using namespace std;
using namespace netflix;


// ---------------------------------------------------------------------------
// Helpers


// ---------------------------------------------------------------------------
// Listener

QALocationMonitorBridge::Listener::Listener(QALocationMonitorBridge *bridge) : mBridge(bridge)
{
}

QALocationMonitorBridge::Listener::~Listener()
{
}

void QALocationMonitorBridge::Listener::operator()( Variant const& event )
{
    mBridge->sendEvent( "monitorEvent", event );
}

// ---------------------------------------------------------------------------

Variant QALocationMonitorBridge::start(const Variant &/*data*/)
{
    if ( nrdApp()->player() )
        nrdApp()->player()->setNetworkMonitorListener( mListener );
    return Variant("nferr", NFErr().toString());
}

Variant QALocationMonitorBridge::stop(const Variant &/*data*/)
{
    if ( nrdApp()->player() )
        nrdApp()->player()->setNetworkMonitorListener( ase::INetworkMonitorListenerPtr() );
    return Variant("nferr", NFErr().toString());
}

Variant QALocationMonitorBridge::getHistory()
{
    Variant state;
    if ( nrdApp()->player() )
    {
        state = nrdApp()->player()->getNetworkMonitorState();
        Log::debug(TRACE_NBP,"QALocationMonitorBridge::getHistory() returning %s", state.toJSON().c_str() );
    }
    return state;
}

Variant QALocationMonitorBridge::setHistory( Variant const& data )
{
    if ( nrdApp()->player() )
    {
        Log::debug(TRACE_NBP,"QALocationMonitorBridge::setHistory(%s)", data.toJSON().c_str() );
        nrdApp()->player()->setNetworkMonitorState( data );
    }

    return Variant("nferr", NFErr().toString());
}

// ---------------------------------------------------------------------------

#if 0
#define QALocationMonitorPropertyList(Property)
    Property(history, NfObject::ReadWrite)
#endif

//PropertyTable(QALocationMonitorBridge, QALocationMonitorPropertyList, properties)

#define QALocationMonitorMethodList(Method)     \
    Method(getHistory)                          \
    Method(setHistory)                          \
    Method(start)                               \
    Method(stop)

MethodTable(QALocationMonitorBridge, QALocationMonitorMethodList);

// ---------------------------------------------------------------------------

QALocationMonitorBridge::QALocationMonitorBridge()
    : NfObject("locationmonitor",
               Methods::QALocationMonitorBridge::methods, Methods::QALocationMonitorBridge::MethodCount),
      mListener()
{
    mListener.reset(new Listener(this));
}

QALocationMonitorBridge::~QALocationMonitorBridge()
{
}

Variant QALocationMonitorBridge::invoke(int method, const Variant &data)
{
    Variant vm;

    switch (method) {
    case Methods::QALocationMonitorBridge::start: {
        vm = start(data);
        break; }
    case Methods::QALocationMonitorBridge::stop: {
        vm = stop(data);
        break; }
    case Methods::QALocationMonitorBridge::getHistory: {
        vm = getHistory();
        break; }
    case Methods::QALocationMonitorBridge::setHistory: {
        vm = setHistory(data["data"]);
        break; }
    default:
        return NfObject::invoke(method, data);
    }

    bool ok;
    const int idx = data.mapValue<int>("IDX", &ok);
    if (ok) {
        Variant rdata;
        rdata["IDX"] = idx;
        rdata["data"] = vm;
        Log::debug(TRACE_NBP,"QALocationMonitorBridge::invoke( %d, %s ) 1 returning %s",
                                    method,
                                    data.toJSON().c_str(),
                                    rdata.toJSON().c_str() );
        parent()->sendEvent(name(), rdata);
    } else {
        Log::debug(TRACE_NBP,"QALocationMonitorBridge::invoke( %d, %s ) 2 returning %s",
                                    method,
                                    data.toJSON().c_str(),
                                    vm.toJSON().c_str() );
        parent()->sendEvent(name(), vm);
    }



    return true;
}

/*
bool QALocationMonitorBridge::getProperty(int index, Variant *result) const
{
    if ( index != Properties::QALocationMonitorBridge::history || !result )
        return false;

    *result = nrdApp()->player()->getNetworkMonitorState();

    return true;
}

bool QALocationMonitorBridge::setProperty(int index, const Variant &value)
{
    if ( index != Properties::QALocationMonitorBridge::history )
        return false;

    nrdApp()->player()->setNetworkMonitorState( value );

    return true;
}
*/



