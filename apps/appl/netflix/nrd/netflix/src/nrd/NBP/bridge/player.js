/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* global nrdp */

nrdp.player = {
    classname: "PlayerBridge",
    _path: "player",
    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    get allowNrdpVideoSwitch() { return this._syncData.allowNrdpVideoSwitch; },

    get bufferPoolSize() { return this._syncData.bufferPoolSize; },

    get currentAudioTrack() { return this._syncData.currentAudioTrack; },
    set currentAudioTrack(trackid) { nrdp._setProperty("player", "currentAudioTrack", trackid); },

    get displayAspectRatio() { return this._syncData.displayAspectRatio; },

    get duration() { return this._syncData.duration; },

    get gopSize() { return this._syncData.gopSize; },

    get maxStreamingBuffer() { return this._syncData.maxStreamingBuffer; },

    get networkProfile() { return this._syncData.networkProfile; },
    set networkProfile(t) { nrdp._setProperty("player", "networkProfile", t); },

    get state() { return this._syncData.state; },

    // IAdaptiveStreamingPlayer state
    get OPENING() { return 0; },
    get PLAYING() { return 1; },
    get PAUSED() { return 2; },
    get STOPPED() { return 3; },
    get CLOSED() { return 4; },

    // NetworkProfile
    get WIRED() { return 0; },
    get WIFI() { return 1; },
    get MOBILE() { return 2; },

    // MediaType
    get MEDIA_UNKNOWN() { return -1; },
    get MEDIA_AUDIO() { return 0; },
    get MEDIA_VIDEO() { return 1; },
    get MEDIA_TEXT() { return 2; },

    // AudioTrackType
    get UNKNOWN_AUDIO() { return -1; },
    get PRIMARY_AUDIO() { return 0; },
    get COMMENTARY_AUDIO() { return 1; },
    get ASSISTIVE_AUDIO() { return 2; },

    addManifest: function addManifest(movieId, manifest) {
        nrdp._invoke("player", "addManifest", {movieId: movieId, manifest: manifest});
    },

    bringVideoToFront: function bringVideoToFront() { nrdp._invoke("player", "bringVideoToFront"); },

    cacheFlush: function cacheFlush(args) {
        nrdp._invoke("player", "cacheFlush", args);
    },

    cacheList: function cacheList(args) {
        nrdp._invoke("player", "cacheList", args);
    },

    cachePrepare: function cachePrepare(drmOnly, manifest, videoType, audioTrack) {
        var args = {
            'drmOnly': drmOnly,
            'manifest': manifest,
            'videoType': videoType,
            'audioTrack': audioTrack
        };
        nrdp._invoke("player", "cachePrepare", args);
    },

    cacheSetSize: function cacheSetSize(maxItems) {
        var args = {'maxItems' : maxItems};
        nrdp._invoke("player", "cacheSetSize", args);
    },

    close: function close() { nrdp._invoke("player", "close"); },

    externalIpAddressChange: function externalIpAddressChange(ipaddr) {
        nrdp._invoke("player", "externalIpAddressChange", {ipaddr: ipaddr});
    },

    getBufferRange: function getBufferRange() { nrdp._invoke("player", "getBufferRange"); },

    obtainPlaybackStat: function obtainPlaybackStat() { nrdp._invoke("player", "obtainPlaybackStat"); },
    obtainStreamingStat: function obtainStreamingStat() { nrdp._invoke("player", "obtainStreamingStat"); },

    open: function open(args) {
        nrdp._invoke("player", "open", args);
    },

    pause: function pause() { nrdp._invoke("player", "pause"); },

    play: function play(pts) {
        var args = {};
        if (pts != undefined && pts != null)
            args.pts = pts;
        nrdp._invoke("player", "play", args);
    },

    provideLicense: function provideLicense(license) { nrdp._invoke("player", "provideLicense", {license: license}); },

    sendVideoToBack: function sendVideoToBack() { nrdp._invoke("player", "sendVideoToBack"); },

    setStreamingBuffer: function setStreamingBuffer(powerSaving, maxBufferLen, minBufferLen) {
        nrdp._invoke("player", "setStreamingBuffer",
                     {powerSaving: powerSaving,
                      maxBufferLen: maxBufferLen,
                      minBufferLen: minBufferLen});
    },

    setVideoBitrateRanges: function setVideoBitrateRanges(ranges) {
        nrdp._invoke("player", "setVideoBitrateRanges", {ranges: ranges});
    },

    setVideoWindow: function setVideoWindow(x, y, width, height, transitionDuration) {
        var args = {
            x: x,
            y: y,
            width: width,
            height: height,
            transitionDuration: transitionDuration
        };
        nrdp._invoke("player", "setVideoWindow", args);
    },

    skip: function skip(ms) { nrdp._invoke("player", "skip", {pts:ms}); },

    stop: function stop() { nrdp._invoke("player", "stop"); },

    swim: function swim(pts, absolute, fuzz, allowRebuffer) {
        var args = { pts: pts };
        if (typeof absolute == "boolean")
            args.absolute = absolute;
        if (typeof fuzz == "number" && !isNaN(fuzz))
            args.fuzz = fuzz;
        if (typeof allowRebuffer == "boolean")
            args.allowRebuffer = allowRebuffer;
        nrdp._invoke("player", "swim", args);
    },

    unpause: function unpause() { nrdp._invoke("player", "unpause"); },

    // player volume
    // getVolume is returned in an event type "volume"
    getVolume : function(cb) {
        nrdp._invoke("player", "getVolume");
    },

    setVolume : function(volume, transition, ease) {
        nrdp._invoke("player", "setVolume",
                     {"targetVolume": volume, "transitionDuration": transition, "ease": ease});
    },

    // audio track switches
    startAudioTrackSwitch: function() {
        nrdp._invoke("player", "startAudioTrackSwitch", {});
    },

    completeAudioTrackSwitch: function() {
        nrdp._invoke("player", "completeAudioTrackSwitch", {});
    },

    notifyBufferingComplete: function notifyBufferingComplete(pts)
    {
        nrdp._invoke("player", "notifyBufferingComplete", {"pts": pts});
    },

    _handleEvent: function _handleEvent(event) {
        for (var prop in event.data._propups) {
            this._syncData[prop] = event.data._propups[prop];
        }
        if (event.name == "IASPlayer" || event.name == "PlaybackReporter") {
            event.data.time = event.time;
            nrdp._callEventListeners(this, event.data);
            return true;
        } else {
            return false;
        }
    }
};
