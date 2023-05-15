/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ILOCATION_SET_MONITOR_FACTORY_H
#define ILOCATION_SET_MONITOR_FACTORY_H

#include <string>

namespace netflix {
namespace ase {

class IAseConfigParameter;
typedef shared_ptr<IAseConfigParameter> IAseConfigParameterPtr;

class ILocationSetMonitor;
typedef shared_ptr<ILocationSetMonitor> ILocationSetMonitorPtr;

class IThroughputPredictor;
typedef shared_ptr<IThroughputPredictor> IThroughputPredictorPtr;

/** Abstract base class for LocationSetMonitor factory
 *
 *  The factory creates and stores location set monitors and maintains the level and global aggregates
 */
class ILocationSetMonitorFactory
{
public:

    typedef shared_ptr<ILocationSetMonitorFactory> ILocationSetMonitorFactoryPtr;

    virtual bool configure( IAseConfigParameterPtr pConfigParameter ) = 0;

    virtual int getType() const = 0;

    virtual ILocationSetMonitorPtr getLocationSetMonitor( std::string const& locationSetKey ) = 0;

    virtual ILocationSetMonitorPtr getLocationSetMonitor( std::string const& locationSetKey, uint32_t level ) = 0;

    virtual void getLocationSetMonitors( std::vector<ILocationSetMonitorPtr>& monitors ) const = 0;

    virtual IThroughputPredictorPtr getGlobalAggregate() = 0;

    virtual IThroughputPredictorPtr getLevelAggregate( uint32_t level ) = 0;

    virtual ~ILocationSetMonitorFactory() {}
};

typedef ILocationSetMonitorFactory::ILocationSetMonitorFactoryPtr ILocationSetMonitorFactoryPtr;

}}

#endif
