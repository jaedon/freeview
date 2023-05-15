/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* global nrdp */

nrdp.texttospeech = {
    classname: "TextToSpeechBridge",
    _path: "texttospeech",

    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    get languages() { return this._syncData.languages; },
    get language() { return this._syncData.language; },
    set language(v) { nrdp._setProperty("texttospeech", "language", v); },

    get voices() { return this._syncData.voices; },
    get voice() { return this._syncData.voice; },

    get rate() { return this._syncData.rate; },
    get pitch() { return this._syncData.pitch; },

    get capabilities() { return this._syncData.capabilities; },

    VERBOSITY_LOW: 0, VERBOSITY_HIGH: 1,
    get verbosity() { return this._syncData.verbosity; },

    say: function say(text, cb) {
        if (typeof text === "string")
            nrdp.texttospeech._fn("say", { text: text }, { cb: cb, time: nrdp.mono() });
        else if (cb)
            cb("said", text);
    },

    stopAndFlush: function stopAndFlush(cb) {
        nrdp.texttospeech._fn("stopAndFlush", {}, cb);
    },

    silence: function silence(ms, cb) {
        if (ms < 0) {
            if (cb)
                cb("said");
        } else {
            nrdp.texttospeech._fn("silence", { ms: ms }, cb);
        }
    },

    _nextIdx: 1,
    _cbs: {},
    _fn: function _fn(name, args, cb) {
        if (!args) args = {};
        args.id = this._nextIdx++;
        if (cb)
            this._cbs[args.id] = cb;
        nrdp._invoke("texttospeech", name, args);
    },

    _handleEvent: function _handleEvent(event) {
        if (event.data && event.data.id) {
            var cb = this._cbs[event.data.id];
            if (typeof cb === "function") {
                cb(event.name, event.data.data);
                if (event.name !== "saying")
                    delete this._cbs[event.data.id];
            } else if (typeof cb === "object" && cb.hasOwnProperty("time")) {
                if (event.name !== "saying")
                    delete this._cbs[event.data.id];
                if (typeof cb.cb === "function") {
                    if (event.data.data === undefined)
                        event.data.data = {};
                    event.data.data.elapsed = nrdp.mono() - cb.time;
                    cb.cb(event.name, event.data.data);
                }
            }
        } else {
            return false;
        }
        return true;
    },

    _syncData: {},
    _updateProperty: function _updateProperty(property, value) {
        var evt;
        if (nrdp.isReady) {
            if (property == "capabilities") {
                evt = {
                    type: "capabilities",
                    old: this.capabilities
                };
            } else if (property == "language") {
                evt = {
                    type: "language",
                    old: this.language
                };
            } else if (property == "voice") {
                evt = {
                    type: "voice",
                    old: this.voice
                };
            } else if (property == "rate") {
                evt = {
                    type: "rate",
                    old: this.rate
                };
            } else if (property == "pitch") {
                evt = {
                    type: "pitch",
                    old: this.pitch
                };
            } else if (property == "verbosity") {
                evt = {
                    type: "verbosity",
		    old: this.verbosity
		};
	    }
        }

        this._syncData[property] = value;

        if (evt) {
            nrdp._callEventListeners(this, evt);
        }
    }
};
