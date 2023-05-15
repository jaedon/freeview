/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* global nrdp */

nrdp.audio = {
    classname: "AudioBridge",
    get codecs() { return this._syncData.codecs; },
    get urls() { return this._syncData.urls; },
    get capability() { return this._syncData.capability; },

    load: function load(URL, cb) {
        nrdp.audio._fn("load", { URL: URL }, cb);
    },

    unload: function unload(URL, cb) {
        nrdp.audio._fn("unload", { URL: URL }, cb);
    },

    unloadAll: function unloadAll(cb) {
        nrdp.audio._fn("unloadAll", undefined, cb);
    },

    play: function play(URL, volume, cb, fadeInMs, fadeOutMs, fadeInEase, fadeOutEase) {
        nrdp.audio._fn("play", { URL: URL, volume: volume, fadeInMs: fadeInMs || 0, fadeOutMs: fadeOutMs || 0, fadeInEase: fadeInEase || 0, fadeOutEase: fadeOutEase || 0 }, cb);
    },

    stop: function stop(URL, cb, fadeOutMs, fadeOutEase) {
        nrdp.audio._fn("stop", { URL: URL, fadeOutMs: fadeOutMs || 0, fadeOutEase: fadeOutEase || 0 }, cb);
    },
    getVolume: function getVolume(cb) {
        nrdp.audio._fn("getVolume", null, cb);
    },
    setVolume: function setVolume(volume, transitionMs, ease) {
        nrdp.audio._fn("setVolume", { volume: volume, transitionMs: transitionMs || 0, ease: ease || 0 });
    },
    
    get EASE_LINEAR() { return 0; },
    get EASE_IN_CUBIC() { return 1; },
    get EASE_OUT_CUBIC() { return 2; },

    _nextIdx: 1,
    _cbs: {},
    _fn: function _fn(name, args, cb) {
        if (!args) args = {};
        args.id = this._nextIdx++;
        if (cb)
            this._cbs[args.id] = cb;
        nrdp._invoke("audio", name, args);
    },

    _handleEvent: function _handleEvent(event) {
        if (event.data && event.data.id) {
            if (typeof this._cbs[event.data.id] == "function") {
                if (event.data.hasOwnProperty("success")) {
                    var args = [event.data.success, event.data.size];
                    if (event.data.hasOwnProperty("URL"))
                        args.unshift(event.data.URL);
                    this._cbs[event.data.id].apply(undefined, args);
                } else {
                    this._cbs[event.data.id](event.data.data);
                }
                delete this._cbs[event.data.id];
            }
        } else {
            return false;
        }
        return true;
    }
};
