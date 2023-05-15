/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ManifestUtilities.h"

#include <nrdbase/LexicalCast.h>
#include <nrdbase/Log.h>
#include <nrdbase/NFErrorTimedOut.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/ASError.h>

#include <nrdnet/AseUrl.h>

#include <nrdase/PrintManifest.h>


#include "NrdConfiguration.h"

using namespace std;
using namespace netflix;
using namespace netflix::ase;
using namespace netflix::util;


static NFErr badParameter(const char *format, ...)
{
    va_list args;
    va_start (args, format);
    string msg = StringFormatter::vsformat(format, args);
    va_end (args);
    Log::trace(TRACE_UTIL, "Bad Parameter: %s", msg.c_str());
    return NFErr_BadParameter;
}

//////////
//
// createManifestFromVariant
//
//////////
/*

    JSON structure:
    {
        index: int,
        duration: int,
        locations: [
            {
                key: string,
                rank: int,
                level: int,
                weight: int
            }
        ],
        servers: [
            {
                name: string,
                type: string,
                id: int,
                key: string,
                rank: int,
                lowgrade: bool
            }
        ],
        video_tracks: [
            {
                type: int,
                stereo: bool,
                ict: bool,
                max_framerate_value: int,
                max_framerate_scale: int,
                streams: [
                    {
                        downloadable_id: string,
                        urls: [
                            {
                                url: string,
                                cdn_id: int
                            }
                        ],
                        content_profile: string,
                        bitrate: int,
                        framerate_value,
                        framerate_scale
                    }
                ],
            }
        ],
        audio_tracks: [
            {
                type: int,
                track_id: string,
                streams: [
                    {
                        downloadable_id: string,
                        urls: [
                            {
                                url: string,
                                cdn_id: int
                            }
                        ],
                        content_profile: "string",
                        bitrate: int,
                        channels: string,
                    }
                ],
            }
        ]
    }
 */

static NFErr createManifestUrls(ManifestPtr manifest,
                                ManifestStreamPtr stream, const Variant &urls,
                                /*out*/vector<ConstManifestUrlPtr> &manifestUrls)
{
    for (int i=0; i< urls.size(); ++i)
    {
        const Variant &url = urls[i];
        AseUrl aseurl(url.mapValue<string>("url"));
        const uint32_t cdnId = url.mapValue<int>("cdn_id");

        map<uint32_t, ManifestServerPtr> servers = manifest->getServers();
        const string &locationKey = servers[cdnId]->getLocationKey();

        ManifestUrlPtr manifestUrl;
        int aserr = ManifestUrl::construct(stream, aseurl, locationKey, cdnId,
                                           /*out*/manifestUrl);
        if (aserr < 0) return badParameter("couldn't construct video url");

        manifestUrls.push_back(manifestUrl);
    }

    return NFErr_OK;
}

static NFErr constructLocations(ManifestPtr manifest, const Variant &locs)
{
    if (!locs.isArray()) return badParameter("locations is not an array");

    vector<shared_ptr<ManifestServingLocation> > locations;
    for (int i = 0; i < locs.size(); i++) {
        const Variant &loc = locs[i];
        shared_ptr<ManifestServingLocation> location;
        int aserr = ManifestServingLocation::construct(
            loc.mapValue<string>("key"),
            loc.mapValue<int>("rank"),
            loc.mapValue<int>("level"),
            loc.mapValue<int>("weight"),
            location);
        if (aserr < 0)
            return badParameter("couldn't construct manifest serving location");
        locations.push_back(location);
    }
    if (locations.size() == 0)
        return badParameter("locations is an empty array");
    manifest->setServingLocations(locations);

    return NFErr_OK;
}

static NFErr constructServers(ManifestPtr manifest, const Variant &servs)
{
    if (!servs.isArray())
        return badParameter("servers is not an array");

    map<uint32_t, shared_ptr<ManifestServer> > serverMap;
    for (int i = 0; i < servs.size(); i++) {
        const Variant &serv = servs[i];
        const uint32_t id = serv.mapValue<int>("id");
        shared_ptr<ManifestServer> server;
        int aserr = ManifestServer::construct(
            serv.mapValue<string>("name"),
            serv.mapValue<string>("type"),
            id,
            serv.mapValue<string>("key"),
            serv.mapValue<int>("rank"),
            serv.mapValue<bool>("lowgrade", 0, false),
            /*out*/server);
        if (aserr < 0)
            return badParameter("couldn't construct manifest server");
        serverMap[id] = server;
    }
    manifest->setServers(serverMap);
    return NFErr_OK;
}

static NFErr constructStream(/*out*/ ManifestStreamPtr &stream,
                             ManifestPtr manifest,
                             ManifestTrackPtr track,
                             const Variant &st)
{
    bool ok;

    const Variant &urls = st.value("urls", &ok);
    if (!ok || !urls.isArray())
        return badParameter("urls is bad or missing");

    string prof = st.mapValue<string>("content_profile", &ok);
    if (!ok) return badParameter("content_profile is bad or missing");

    ContentProfile profile = stringToProfile(prof);
    if (profile == UNKNOWN_PROFILE)
        return badParameter("content_profile is unknown");

    string dlid = st.mapValue<string>("downloadable_id", &ok);
    if (!ok) return badParameter("downloadable_id is bad or missing");

    uint32_t bitrate = st.mapValue<uint32_t>("bitrate", &ok);
    if (!ok) return badParameter("bitrate is bad or missing");

    int aserr = ManifestStream::construct(track, stream);
    if (aserr < 0) return badParameter("couldn't construct manifest stream");

    stream->setContentProfile(profile);
    stream->setStreamKey(ManifestUtilities::encodeStreamKey(dlid, profile));
    stream->setMimeType(MimeType(profileToMimeType(profile)));
    stream->setMimeCodec(MimeCodec(profileToMimeCodec(profile)));
    stream->setNominalBitrate(bitrate);
    vector<shared_ptr<const ManifestUrl> > manifestUrls;
    NFErr err = createManifestUrls(manifest, stream, urls, /*out*/manifestUrls);
    if (!err.ok())
        return err;
    stream->setUrls(manifestUrls);

    track->addStream(stream);

    return NFErr_OK;
}

static NFErr constructVideoTracks(ManifestPtr manifest, const Variant &tracks,
                                  bool drmOnly, /*out*/bool &fastplayPossible)
{
    if (!tracks.isArray())
        return badParameter("video_tracks is not an array");

    fastplayPossible = false;

    int addedTracks = 0;
    for (int i=0; i<tracks.size(); ++i)
    {
        const Variant &track = tracks[i];
        if (!track.isStringMap())
            return badParameter("%d element of video_tracks is not a map", i);

        // construct the ManifestTrack
        shared_ptr<ManifestTrack> mtrack;
        int aserr = ManifestTrack::construct(manifest, /*out*/mtrack);
        if (aserr < 0) return badParameter("couldn't construct manifest track");
        mtrack->setMediaType(MEDIA_VIDEO);

        const Variant &streams = track["streams"];
        if (!streams.isArray())
            return badParameter("video_track %d streams is not array", i);

        bool ok;
        bool stereo = track.mapValue<bool>("stereo", &ok);
        if (ok)
            mtrack->setStereoVideo(stereo);

        int maxFrameRateValue = track.mapValue<int>("max_framerate_value", &ok);
        if (ok)
        {
            int maxFrameRateScale = track.mapValue<int>("max_framerate_scale", &ok);
            if (ok)
                mtrack->setMaxVideoFrameRate(maxFrameRateValue, maxFrameRateScale);
        }

        int disabledStreams = 0;
        for (int j=0; j<streams.size(); ++j)
        {
            const Variant &stream = streams[j];
            if (!stream.isStringMap())
                return badParameter("video_track %d stream %d is not a map", i, j);

            // construct ManifestStream
            ManifestStreamPtr mstream;
            NFErr err = constructStream(/*out*/mstream, manifest, mtrack, stream);
            if (!err.ok())
                return err;

            int rw = stream.mapValue<int>("res_w", &ok);
            if (!ok) return badParameter("missing res_w");
            int rh = stream.mapValue<int>("res_h", &ok);
            if (!ok) return badParameter("missing res_h");
            mstream->setVideoSize(rw, rh);

            int pw = stream.mapValue<int>("pix_w", &ok);
            if (!ok) return badParameter("missing pix_w");
            int ph = stream.mapValue<int>("pix_h", &ok);
            if (!ok) return badParameter("missing pix_h");
            mstream->setPixelAspectRatio(pw, ph);

            bool hd = stream.mapValue<bool>("hd", &ok);
            if (ok)
                mstream->setHD(hd);

            bool superhd = stream.mapValue<bool>("superhd", &ok);
            if (ok)
                mstream->setSuperHD(superhd);

            int framerateValue = stream.mapValue<int>("framerate_value", &ok);
            if (ok)
            {
                int framerateScale = stream.mapValue<int>("framerate_scale", &ok);
                if (ok)
                    mstream->setVideoFrameRate(framerateValue, framerateScale);
            }

            ContentProfile profile = mstream->getContentProfile();
            bool isDrm = (profileDrmType(profile) != NO_DRM);
            if (!isDrm) // clear stream
            {
                if (drmOnly)
                {
                    NTRACE(TRACE_UTIL, "disabling clear video stream %s",
                           mstream->getStreamKey().c_str());
                    mstream->setEnabled(false);
                    ++disabledStreams;
                }
                else
                    fastplayPossible = true;
            }
        }

        if (disabledStreams == streams.size())
        {
            Log::warn(TRACE_UTIL, "disabled all streams (%d) from video_track",
                      disabledStreams);
        }

        manifest->addTrack(mtrack);
        ++addedTracks;
    }

    if (!addedTracks)
    {
        Log::warn(TRACE_UTIL, "no video tracks added");
        return badParameter("no video tracks/streams added");
    }

    // are we allowing fastplay?
    if (!drmOnly && fastplayPossible)
    {
        NTRACE(TRACE_UTIL, "disabling drm streams");
        const std::vector<ManifestTrackPtr> &tracks = manifest->getTracks();
        std::vector<ManifestTrackPtr>::const_iterator tit;
        for (tit = tracks.begin(); tit != tracks.end(); ++tit)
        {
            const std::vector<ManifestStreamPtr> &streams = (*tit)->getStreams();
            std::vector<ManifestStreamPtr>::const_iterator sit;
            int streamsKept = 0;
            for (sit = streams.begin(); sit != streams.end(); ++sit)
            {
                ContentProfile profile = (*sit)->getContentProfile();
                if (profileDrmType(profile) != NO_DRM)
                {
                    NTRACE(TRACE_UTIL, "disabling drm stream %s",
                           (*sit)->getStreamKey().c_str());
                    (*sit)->setEnabled(false);
                }
                else
                    ++streamsKept;
            }

            if (!streamsKept)
            {
                Log::warn(TRACE_UTIL, "no streams kept in track %s",
                          (*tit)->getTrackKey().c_str());
                return badParameter("no clear streams in videotrack");
            }
        }
    }

    return NFErr_OK;
}

static NFErr constructAudioTracks(ManifestPtr manifest, const Variant &tracks)
{
    if (!tracks.isArray())
        return badParameter("audio_tracks is not an array");

    for (int i=0; i<tracks.size(); ++i)
    {
        const Variant &track = tracks[i];
        if (!track.isStringMap())
            return badParameter("%d element of audio_tracks is not a map", i);

        // construct the ManifestTrack
        shared_ptr<ManifestTrack> mtrack;
        int aserr = ManifestTrack::construct(manifest, /*out*/mtrack);
        if (aserr < 0) return badParameter("couldn't construct manifest track");
        mtrack->setMediaType(MEDIA_AUDIO);

        bool ok = false;
        string track_id = track.mapValue<string>("track_id", &ok);
        if (!ok) return badParameter("track_id is bad or missing");
        mtrack->setTrackKey(track_id);

        const Variant &streams = track["streams"];
        if (!streams.isArray())
            return badParameter("audio_track %d streams is not array", i);

        for (int j=0; j<streams.size(); ++j)
        {
            const Variant &stream = streams[j];
            if (!stream.isStringMap())
                return badParameter("audio_track %d stream %d is not a map", i, j);

            // construct ManifestStream
            ManifestStreamPtr mstream;
            NFErr err = constructStream(/*out*/mstream, manifest, mtrack, stream);
            if (!err.ok())
                return err;

            // disable streams outside the bitrate range
            uint32_t bitrate = mstream->getNominalBitrate();
            if ((bitrate < NrdConfiguration::minAudioBitrate()) ||
                (bitrate > NrdConfiguration::maxAudioBitrate()))
            {
                NTRACE(TRACE_UTIL, "disabling stream because bitrate %d is out of range",
                       bitrate);
                mstream->setEnabled(false);
            }

            // determine stream's channels
            string chan = stream.mapValue<string>("channels", &ok);
            if (!ok) return badParameter("channels is bad or missing");

            vector<int> channels;
            if (chan == "2.0") {
                channels.push_back(2);
            } else if (chan == "5.1") {
                channels.push_back(6);
            } else
                return badParameter("unknown channels %s", chan.c_str());
            mstream->setAudioChannels(channels);
        }

        manifest->addTrack(mtrack);
    }

    return NFErr_OK;
}

NFErr ManifestUtilities::createManifestFromVariant(
    /*out*/shared_ptr<Manifest> &manifest, /*out*/bool &fastplayPossible,
    const Variant &var, bool drmOnly)
{
    fastplayPossible = false;
    if (!var.isStringMap())
        return badParameter("manifest param is not a map");

    bool ok;

    // construct manifest
    ullong duration = var.mapValue<int>("duration", &ok);
    if (!ok) return badParameter("duration is bad or missing");

    AseTimeInterval dur(duration, 1000);
    int aserr = Manifest::construct(dur, manifest);
    if (aserr < 0) return badParameter("couldn't construct manifest");

    std::string movieId(var.mapValue<string>("movieId"));
    manifest->setContentId(movieId);
    manifest->setManifestIndex(var.mapValue<int>("index", 0, 0));

    // construct locations
    NFErr err = constructLocations(manifest, var["locations"]);
    if (!err.ok()) return err;

    // construct servers
    err = constructServers(manifest, var["servers"]);
    if (!err.ok()) return err;

    // construct video track
    err = constructVideoTracks(manifest, var["video_tracks"], drmOnly,
                               /*out*/fastplayPossible);
    if (!err.ok()) return err;

    // audio tracks
    err = constructAudioTracks(manifest, var["audio_tracks"]);
    if (!err.ok()) return err;

    manifest->sortStreams(ManifestStream::NominalBitrateCompare());

    NTRACES(TRACE_UTIL, *manifest);

    return NFErr_OK;
}

//////////
//
// StreamKey
//
// format of StreamKey is <profile>:<downloadableid>
//
//////////
string ManifestUtilities::encodeStreamKey(const string &downloadableId,
                                          ContentProfile profile)
{
    return StringFormatter::sformat("%d %s", profile, downloadableId.c_str());
}

bool ManifestUtilities::parseStreamKey(/*out*/string &downloadableId,
                                       /*out*/ContentProfile &profile,
                                       const std::string &streamKey)
{
    vector<string> tokens;
    StringTokenizer::tokenize(streamKey, tokens, " ");
    if (tokens.size() != 2)
        return false;

    profile = (ContentProfile)lexical_cast<int>(tokens[0]);
    downloadableId = tokens[1];

    return true;
}
