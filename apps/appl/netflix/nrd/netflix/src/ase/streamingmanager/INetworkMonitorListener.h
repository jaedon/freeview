/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_NETWORK_MONITOR_LISTENER_H
#define I_NETWORK_MONITOR_LISTENER_H

#include <nrdbase/Variant.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdase/AseCommonDataTypes.h>
#include "ILocationSetMonitor.h"

#include <string>

namespace netflix {
namespace ase {

/** A listener for changes in network monitor history
 *
 * event = { "#mandatory" : [ "event", "monitors" ],
 *           "event" : "enum(load|store|update)",
 *           "monitors" : { "#any" : monitor }
 *          }
 *
 * monitor = { "#choice" : [ simplemonitor, locationsetmonitor ] }
 *
 */
class INetworkMonitorListener
{
public:
    typedef shared_ptr<INetworkMonitorListener> INetworkMonitorListenerPtr;

    virtual void operator()( Variant const& data ) = 0;

    virtual ~INetworkMonitorListener() {}
};

typedef INetworkMonitorListener::INetworkMonitorListenerPtr INetworkMonitorListenerPtr;

}}

#endif
