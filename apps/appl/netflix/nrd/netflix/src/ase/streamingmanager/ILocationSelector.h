/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef I_LOCATION_SELECTOR_H
#define I_LOCATION_SELECTOR_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/Manifest.h>
#include <nrdase/IMediaConsumer.h>
#include <nrdbase/AseErrorCodes.h>

#include <string>

namespace netflix {

struct NetworkInterfaceInfo;

namespace ase {
class IStreamingLocationSet;

class IAseConfigParameter;
typedef shared_ptr<IAseConfigParameter> IAseConfigParameterPtr;

/**
 * @class ILocationSelector
 *
 * @brief the class that implements the serving location selection
 */
class ILocationSelector : public enable_shared_from_this<ILocationSelector>
{
public:
    typedef shared_ptr<ILocationSelector> ILocationSelectorPtr;

    virtual void configure( IAseConfigParameterPtr pConfig ) = 0;

    enum LocationSelectionRequestSource
    {
        VIDEO_DOWNLOADING = 0,
        AUDIO_DOWNLOADING,
        HEADER_DOWNLOADING,
        HEURISTICS
    };

    static bool locationSelectionFailed(AseErrorCode errorCode);

    /**
     * obtain the preferred location set's monitor given a stream key
     */
    virtual AseErrorCode getPreferredLocationSet(   std::string const& streamKey,
                                                    LocationSelectionRequestSource requestSource,
                                                    shared_ptr<IStreamingLocationSet>& pStreamingLocationSet ) = 0;

    /**
     * add a new manifest
     */
    virtual void addManifest(ManifestPtr pManifest) = 0;

    /**
     * remove a manifest from the location selector
     */
    virtual void removeManifest(ManifestPtr pManifest) = 0;

    /**
     * set fast play mode
     */
    virtual void setFastplay(bool fastplay) = 0;

    /** Notify a change on media consumer state. */
    virtual void notifyMediaConsumerState(
                        IMediaConsumer::State state ) = 0;

    /** Notify the media playback state.
     *
     * @param playableLength [In] The current playable length, in ms.
     * @param goodQuality [In] Whether the playback quality is good. Good
     *                         quality indicates the domain selector
     *                         could switch to low grade CDNs.
     */
    virtual void notifyMediaPlaybackState(
                            uint32_t playableLength,
                            bool goodQuality ) = 0;

    /** Check if the domain selection is complete. */
    virtual bool locationSelectionComplete() const = 0;

    /** Check if the domain selection  is in the low grade mode. */
    virtual bool lowGradeMode() const = 0;

    virtual bool bitrateSwitchFreeze() const = 0;

    virtual void networkFailing(    AseErrorCode            latestStatus,
                                    AseUrl::HostPort const& destination,
                                    AseTimeVal const*       pLatestErrorTime,
                                    AseTimeVal const&       latestDataReceived ) = 0;

    virtual ~ILocationSelector()
    {
    }

};

typedef ILocationSelector::ILocationSelectorPtr ILocationSelectorPtr;

inline
bool ILocationSelector::locationSelectionFailed(AseErrorCode errorCode)
{
    if ( (errorCode >= AS_LAST_DOMAIN_SELECTION_FAILURE) &&
            (errorCode <= AS_DOMAIN_SELECTION_FAILURE) )
        return true;
    else
        return false;

}

}}

#endif
