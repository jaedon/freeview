/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_CONNECTION_CONFIGURATION_H
#define I_CONNECTION_CONFIGURATION_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdnet/AseTimeVal.h>

namespace netflix {
namespace ase {

class IAseConfigParameter;
struct TrackConfiguration;

class IConnectionConfiguration
{
public:
    typedef shared_ptr<IConnectionConfiguration> IConnectionConfigurationPtr;

public:
    virtual ~IConnectionConfiguration() {}

    /** Re-configure the connection configuration based on new config parameters */
    virtual void configure( shared_ptr<IAseConfigParameter> pConfigParameters ) = 0;

    /** Indicate pipeline detection result */
    virtual void setPipelineSupported( bool pipeline ) = 0;

    /** Get the next re-evaluation time */
    virtual AseTimeVal getNextTimeout() = 0;

    /** Handle the timeout event */
    virtual void processTimeout() = 0;

    /** Get the configuration for a media type */
    virtual TrackConfiguration const& getConfiguration( MediaType mediaType ) const = 0;

    /** Notify that http pipelining is instable */
    virtual void reportPipeliningInstable() = 0;

protected:
    static const int DEFAULT_MAX_CONNECTIONS = 1;
    static const int DEFAULT_TARGET_CONNECTIONS = 1;
    static const int DEFAULT_SOCKET_BUFFER = 0; // use default in http library
};

typedef IConnectionConfiguration::IConnectionConfigurationPtr IConnectionConfigurationPtr;

}} /* namespace netflix::ase */

#endif /* I_CONNECTION_CONFIGURATION_H */
