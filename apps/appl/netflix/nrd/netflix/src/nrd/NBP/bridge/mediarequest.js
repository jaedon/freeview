/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp */

nrdp.mediarequest = {
    _path: "mediarequest",

    addEventListener: function addEventListener(evt, listener)
    {
        nrdp._addEventListener(this, evt, listener);
    },
    removeEventListener: function removeEventListener(evt, listener)
    {
        nrdp._removeEventListener(this, evt, listener);
    },

    // MediaType constants: matches AseCommonDataType.h
    get MEDIA_AUDIO() { return 0; },
    get MEDIA_VIDEO() { return 1; },
    get MEDIA_TEXT() { return 2; },
    get MEDIA_HEADERS() { return 3; },

    // ResponseTypes for how a MediaRequest returns its data: matches MediaRequestBridge.h
    get RESPONSE_DATABUFFER() { return 0; },
    get RESPONSE_STREAM() { return 1; },

    // mediaSource
    // @return "sourceid" id of created mediaSource
    createMediaSource: function(cb, player)
    {
        return this._fn("mediaSourceCreate", {"player": player}, cb);
    },

    destroyMediaSource: function(sourceid, cb)
    {
        return this._fn("mediaSourceDestroy", {"sourceid": sourceid}, cb);
    },

    // mediaBufferPool
    createMediaBufferPool: function(cb)
    {
        return this._fnAsync("mediaBufferPoolCreate", {}, cb);
    },

    // sourceBuffer
    // @param[in] sourceid, the mediaSourceId to create buffers for
    // @param[in] mediatypes, array of mediatypes to create
    // @return "bufferids" array of bufferid's created
    createSourceBuffers: function(sourceid, mediatypes, cb)
    {
        return this._fn("sourceBuffersCreate",
                        {
                            "sourceid": sourceid,
                            "mediatypes": mediatypes
                        }, cb);
    },

    // NOT NEEDED
    attachDrmHeaderToSourceBuffer: function(){},

    // DEPRECATED
    attachHeaderToSourceBuffer: function(header, streamId, bufferId, contentId,
                                         contentProfile, bitrate, frameRateValue,
                                         frameRateScale, cb, sourceId)
    {
        return this.attachDataToSourceBuffer( header, sourceId, bufferId, cb );
    },

    attachDataToSourceBuffer: function( data, sourceId, bufferId, cb )
    {
        return this._fn("sourceBufferAttachData",
            {
                "sourceid": sourceId,
                "bufferid": bufferId,
                "data": data
            }, cb);
    },

    attachRequestToSourceBuffer: function(requestid, bufferid, cb, sourceId)
    {
        return this._fn("sourceBufferAttachRequest",
            {
                "sourceid": sourceId,
                "bufferid": bufferid,
                "requestid": requestid
            }, cb);
    },

    sourceBufferSetTimestampOffset: function(sourceId, bufferId, timeOffset, timescale, cb)
    {
      return this._fn("sourceBufferSetTimestampOffset",
                      {
                          "sourceid": sourceId,
                          "bufferid": bufferId,
                          "timeoffset": timeOffset,
                          "timescale": timescale
                      }, cb );
    },

    // downloadTrack's
    // @param[in] mediatypes, array of mediatypes to add
    // @return "trackids" array of trackid's added
    addDownloadTracks: function(configs, cb)
    {
        return this._fnAsync("downloadTracksCreate", {"configs": configs}, cb);
    },

    // @param[in] trackid, the trackid to pause
    pauseDownloadTrack: function(trackid, cb)
    {
        return this._fnAsync("downloadTrackPause", {"trackid": trackid}, cb);
    },

    // @param[in] trackid, the trackid to resume
    resumeDownloadTrack: function(trackid, cb)
    {
        return this._fnAsync("downloadTrackResume", {"trackid": trackid}, cb);
    },

    // @param[in] trackid, the trackid to reconfigure
    // @param[in] config, the config object
    reconfigureDownloadTrack: function(trackid, config, cb)
    {
        return this._fnAsync("downloadTrackReconfigure", {"trackid": trackid, "config": config}, cb);
    },

    // @param[in] trackid, the trackid to remove
    removeDownloadTracks: function(trackids, cb)
    {
        return this._fnAsync("downloadTracksDestroy", {"trackids": trackids}, cb);
    },

    // MediaRequest's
    createRequest: function(requestId, trackId, url, byteStart, byteEnd, responseType,
                            ptsStart, ptsEnd, cb)
    {
        return this._fn("requestCreate",
            {
                "requestid": requestId,
                "trackid" : trackId,
                "url" : url,
                "start" : byteStart,
                "end" : byteEnd,
                "responsetype" : responseType,
                "ptsStart" : ptsStart,
                "ptsEnd" : ptsEnd
            }, cb);
    },

    abortRequest: function(requestid, cb)
    {
        return this._fn("requestAbort", {"requestid" : requestid}, cb);
    },

    removeRequest: function(requestid, cb)
    {
        return this._fn("requestRemove", {"requestid": requestid}, cb);
    },

    swapRequestURL: function(requestid, url, cb)
    {
        return this._fn("requestSwapURL",
                        {
                            "requestid" : requestid,
                            "url" : url
                        },
                        cb);
    },

/*
    getRequestData: function(requestid, cb)
    {
        if ( this._requestData[ requestid ] ) {
            cb( { "success" : true, "data" : this._requestData[ requestid ] } );
            delete this._requestData[ requestid ];
        } else {
            cb( { "success" : false, "error" : -3 } );
        }

        return 0;
    },
*/

    endOfStream: function(bufferId, cb, sourceId)
    {
        return this._fn("endOfStream",
                        {
                            "sourceid" : sourceId,
                            "bufferid" : bufferId
                        },
                        cb);
    },
    
    parseSegmentIndex: function( data, anchor )
    {
        return this._fn("parseSegmentIndex", { "data" : data, "anchor" : anchor } );
    },

    // private
    _nextIdx: 1,
    _cbs: {},
    // XXX Temporarily, we will keep the request data here when it is returned in oncomplete
    //     events for backwards compatibility with JS-ASE in the 9/23 push
    //     This can be removed after 10/7, where this is handled properly in NRDJS
    //_requestData: { },


    _fn: function(name, params, cb)
    {
        if (!params) params = {};

        var ret = nrdp._invoke("mediarequest", name, params);

        if ( cb ) {
            cb( ret );
            return undefined;
        }
        return ret;
    },

    _fnAsync: function(name, params, cb)
    {
        if (!params) params = {};
        params.idx = this._nextIdx++;
        this._cbs[params.idx] = cb;
        nrdp._invoke("mediarequest", name, params);
        return params.idx;
    },

    _handleEvent: function(event)
    {
        var cb, idx;

        // handle invoke callbacks
        if (event.name == "result")
        {
            idx = event.data.idx;

            if (typeof this._cbs[idx] == 'function')
            {
                cb = this._cbs[idx];
                delete event.data.idx;
                cb(event.data);
            }
            delete this._cbs[idx];

            return true;
        }

        //if (event.name == "oncomplete") {
        //    this._requestData[ event.data["requestId"] ] = event.data["data"];
        //}

        // handle progress events
        if ((event.name == "onloadstart") ||
            (event.name == "onfirstbyte") ||
            (event.name == "onprogress") ||
            (event.name == "oncomplete") ||
            (event.name == "onerror") ||
            (event.name == "downloadpaused") ||
            (event.name == "downloadresumed") ||
            (event.name == "pipelinedetection") ||
            (event.name == "networkfailing") ||
            (event.name == "TransportReporter" ))
        {
            nrdp._callEventListeners(this, event.data);
            return true;
        }

        // unhandled
        return false;
    }
};
