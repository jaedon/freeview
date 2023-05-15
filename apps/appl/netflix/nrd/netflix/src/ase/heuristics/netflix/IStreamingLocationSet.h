/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_STREAMING_LOCATION_SET_H
#define I_STREAMING_LOCATION_SET_H

#include <nrdase/Manifest.h>
#include <nrdnet/NetCommonDataTypes.h>

namespace netflix {
namespace ase {

class ILocationSetMonitor;

/**
 * @class IStreamingLocationSet
 * @brief StreamingLocationSet represents a set of locations for a given serving stream.
 *        It has two subclasses:
 *        * SingleLocation represents a single location
 *        * MultiLocation represents a particular combination of SingleLocations
 *        A StreamingLocationSet maintains or have access to network statistics and network errors.
 *        It controls local failover among ServingUrls.
 */
class IStreamingLocationSet : public enable_shared_from_this<IStreamingLocationSet>
{
public:
    typedef shared_ptr<IStreamingLocationSet> IStreamingLocationSetPtr;

    virtual std::string const& getLocationSetKey() const = 0;
    virtual shared_ptr<ILocationSetMonitor> getLocationSetMonitor() = 0;
    virtual std::string getStreamKey() = 0;

    virtual void getTimeouts(uint32_t& httpConnectTimeout, uint32_t& httpStallTimeout) const = 0;

    virtual bool lowGrade() const = 0;

    /* ---------------------------------------------------------------------------------*/
    /*                      actions                                                     */
    /* ---------------------------------------------------------------------------------*/
    virtual ConstManifestUrlPtr getDownloadableUrl() const = 0;
    virtual uint32_t getPrimaryCdnId() const = 0;

    /* ---------------------------------------------------------------------------------*/
    /*                      report failures                                             */
    /* ---------------------------------------------------------------------------------*/
    /** Report HTTP error for a location. */
    virtual int32_t reportHttpError(
                        ConstManifestUrlPtr pManifestUrl,
                        int32_t errorCode,
                        int32_t httpStatusCode ) = 0;

    virtual bool compare(IStreamingLocationSetPtr pStreamingLocationSet) = 0;

    virtual ~IStreamingLocationSet()
    {
    }
};

typedef IStreamingLocationSet::IStreamingLocationSetPtr IStreamingLocationSetPtr;

}}

#endif
