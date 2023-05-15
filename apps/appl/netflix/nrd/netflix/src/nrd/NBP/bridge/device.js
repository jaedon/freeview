/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* global nrdp */

nrdp.device = {
    classname: "DeviceBridge",
    _path: "device",
    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    get VOLUME_NONE() { return 0; },
    get VOLUME_SPEAKER() { return 1; },
    get VOLUME_STREAM() { return 2; },

    // Drm types
    get PLAY_READY() { return 0; },
    get SESSION_DH1() { return 1; },
    get WIDEVINE_CEF() { return 2; },

    get drmType() { return this._syncData.capability.drmType; },

    get currentViewMode() { return this._syncData.currentViewMode; },
    get availableViewModes() { return this._syncData.availableViewModes; },
    get softwareVersion() { return this._syncData.softwareVersion; },
    get certificationVersion() { return this._syncData.certificationVersion; },
    get deviceModel() { return this._syncData.deviceModel; },
    get ESNPrefix() { return this._syncData.ESNPrefix; },
    get SDKVersion() { return this._syncData.SDKVersion; },
    get ESN() { return this._syncData.ESN; },
    get language() { return this._syncData.language; },
    get friendlyName() { return this._syncData.friendlyName; },
    get startupTags() { return this._syncData.startupTags; },
    get volumeControlType() { return this._syncData.volumeControlType; },
    get volumeStep() { return this._syncData.volumeStep; },
    get mute() { return this._syncData.mute; },
    set mute(m) { nrdp._setProperty("device", "mute", m); },

    get capability() { return this._syncData.capability; },
    get videoOutput() { return this._syncData.videoOutput; },
    get supportedVideoOutput() { return this._syncData.supportedVideoOutput; },
    get activeVideoOutput() { return this._syncData.activeVideoOutput; },
    get dnslist() { return this._syncData.dnslist; },
    get iflist() { return this._syncData.iflist; },
    get ipversions() { return this._syncData.ipversions; },

    get UIVersion() { return this._syncData.UIVersion; },
    set UIVersion(version) { nrdp._setProperty("device", "UIVersion", version); },
    addLibrary: function addLibrary(name, version, component) {
        nrdp._invoke("device", "addLibrary", { name: name, version: version, component: component });
    },
    setUIVersion: function setUIVersion(version) { nrdp._setProperty("device", "UIVersion", version); },

    get UILanguages() {
        return this._syncData.UILanguages;
    },
    set UILanguages(langs) {
        langs = Array.isArray(langs) ? langs : [];
        var val = langs.join(",");
        nrdp._setProperty("device", "UILanguages", val);
    },

    isScreensaverOn: function isScreensaverOn() { return this._syncData.screensaverOn; },

    factoryReset: function factoryReset(cb) {
        nrdp.storage._clearAll();
        nrdp.device._fn("factoryReset", null, cb);
    },
    setViewMode: function setViewMode(viewMode) {
        nrdp._invoke("device", "setViewMode", {viewMode : viewMode});
    },

    getVolume: function getVolume(cb) {
        nrdp.device._fn("getVolume", null, cb);
    },
    setVolume : function(volume) {
        nrdp._invoke("device", "setVolume",
                      {"targetVolume": volume});
    },

    setRegistered: function setRegistered(value) {
        nrdp._setProperty("device", "registered", !!value);
    },
    get registered() { return this._syncData.registered; },

    getSignatures: function getSignatures(cb) {
        nrdp.device._fn("getSignatures", null, cb);
    },

    getUptime: function getUptime(cb) {
        nrdp.device._fn("getUptime", null, cb);
    },

    getSystemValue: function getSystemValue(key, cb) {
        nrdp.device._fn("getSystemValue", {key : key}, cb);
    },

    getDisplaySize: function getDisplaySize(cb) {
        nrdp.device._fn("getDisplaySize", null, cb);
    },

    /*
     * dual video
     */

    // codec profiles
    get CODEC_NOT_AVAILABLE() { return 0;},
    get AVC_MPL30() {return 1;},
    get AVC_MPL31() {return 2;},
    get AVC_MPL40() {return 3;},
    get AVC_SHPL30() {return 4;},
    get AVC_SHPL31() {return 5;},
    get AVC_SHPL40() {return 6;},
    get HEVC_MAIN10_L20() {return 7;},
    get HEVC_MAIN10_L21() {return 8;},
    get HEVC_MAIN10_L30() {return 9;},
    get HEVC_MAIN10_L31() {return 10;},
    get HEVC_MAIN10_L40() {return 11;},
    get HEVC_MAIN10_L41() {return 12;},
    get HEVC_MAIN10_L50() {return 13;},
    get HEVC_MAIN10_L51() {return 14;},

    // zorder
    get ZORDER_NOT_AVAILABLE() {return 0;},
    get ZORDER_ANY() {return 1;},
    get ZORDER_TOP_ONLY() {return 2;},
    get ZORDER_BOTTOM_ONLY() {return 3;},

    get maxVideoPipelines(){
        return this._syncData.maxVideoPipelines;
    },

    get getVideoPipelineCapabilities(){
        return this._syncData.videoPipelineCapabilities;
    },

    getRemainingVideoPipelineCapability:
    function getRemainingVideoPipelineCapability(videoPipelineCapabilites, cb) {
        nrdp.device._fn("getRemainingVideoPipelineCapability",
                        {videoPipelineCapabilites:videoPipelineCapabilites},
                        cb);
    },

    disableDolbyVisionELComposing: function disableDolbyVisionELComposing(disable, cb) {
        nrdp.device._fn("disableDolbyVisionELComposing", {disable:disable}, cb);
    },

    _nextIdx: 1,
    _cbs: {},
    _fn: function _fn(name, args, cb) {
        if (!args) args = {};
        args.idx = this._nextIdx++;
        if (cb)
            this._cbs[args.idx] = cb;
        nrdp._invoke("device", name, args);
    },
    _handleEvent: function _handleEvent(event) {
        if (event.data && event.data.idx) {
            if (typeof this._cbs[event.data.idx] == "function") {
                this._cbs[event.data.idx](event.data.data);
                delete this._cbs[event.data.idx];
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
            if (property == "screensaverOn") {
                evt = {
                    type: "screensaverchange"
                };
            } else if (property == "capability") {
                evt = {
                    type: "capabilitychange",
                    old: this.capability
                };
            } else if (property == "videoOutput") {
                evt = {
                    type: "videooutputchange",
                    old: this.videoOutput
                };
            } else if (property == "language") {
                evt = {
                    type: "languagechange",
                    old: this.language
                };
            } else if (property == "currentViewMode") {
                evt = {
                    type: "viewmodechange"
                };
            } else if (property == "iflist" ) {
                evt = {
                    type: "networkchange"
                };
            } else if (property.match(/^volume/) || property == "mute") {
                evt = {
                    type: "volumechange",
                    oldvolume: this.volume,
                    oldmute: this.mute
                };
            }
        }

        this._syncData[property] = value;

        if (evt) {
            nrdp._callEventListeners(this, evt);
        }
    }
};
