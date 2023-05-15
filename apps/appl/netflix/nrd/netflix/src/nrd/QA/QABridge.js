/* global nrdp */

(function(){

nrdp.qa = {
    addEventListener: function(e, l) { nrdp._addEventListener("qa", e, l); },
    removeEventListener: function(e, l) { nrdp._removeEventListener("qa", e, l); },

    init: function(cb, url) { nrdp.gibbon.init(cb, url); }, //compatibility

    login: function(cb) {
        function actHandler(evt) {
            if (evt.cookies && evt.cookies[0] && evt.cookies[1])
                nrdp.registration.activationTokens = evt.cookies.reduce(function(o, e) {
                    var m = e.split(";")[0].split("=");
                    o[m[0].replace(/Test/, "")] = m[1];
                    return o;
                }, {});
            nrdp.registration.removeEventListener("activate", actHandler);
            if (cb) cb(evt.cookies ? true : false);
        }

        function activate() {
            if (!nrdp.nrdjs || !nrdp.registration.registered) {
                nrdp.registration.addEventListener("activate", actHandler);
                nrdp.registration.emailActivate(nrdp.gibbon.email, nrdp.gibbon.password);
            } else {
                if (cb) cb(true);
            }
        }

        function selectDA() {
            if (!nrdp.registration.currentDeviceAccount) {
                nrdp.registration.selectDeviceAccount(nrdp.registration.deviceAccounts[0].accountKey, activate);
            } else {
                activate();
            }
        }

        function createDA() {
            if (!nrdp.registration.deviceAccounts.length) {
                nrdp.registration.createDeviceAccount(selectDA);
            } else {
                selectDA();
            }
        }

        function verifyRegistration() {
            if (!nrdp.registration) {
                nrdp.gibbon.loadScript({url: "http://ppd.builds.test.netflix.net/view/NRDJS/job/PPD-NRDJS-master/lastSuccessfulBuild/artifact/origin/master/nrdjs.min.js"}, 
                                       function() { nrdp.js.init(createDA); });
            } else {
                createDA();
            }
        }

        nrdp.qa.init(verifyRegistration);
    },

    play: function(movieId) {
        nrdp.qa.login(function() {
            nrdp.media.openAndPlay({movieId: movieId, trackerId: 999});
        });
    },

    _nextIdx: 1,
    _cbs: {},

    _invoke: function(object, func, args, cb) {
        if (!args) args = {};
        args.IDX = this._nextIdx++;
        this._cbs[args.IDX] = cb;
        nrdp._invoke(object, func, args);
    },

    _handleEvent: function(event) {
        if (!event.data || !event.data.IDX)
            return false;

        if (typeof this._cbs[event.data.IDX] == "function") {
            this._cbs[event.data.IDX](event.data.data);
            delete this._cbs[event.data.IDX];
        }

        return true;
    }

};

nrdp.qa.device = {
    addEventListener: function(e, l) { nrdp._addEventListener("qa.device", e, l); },
    removeEventListener: function(e, l) { nrdp._removeEventListener("qa.device", e, l); },

    _handleEvent: function(event) {
        var evt = {
            type: event.name,
            data: event.data
        };
        nrdp._callEventListeners("qa.device", evt);
        return true;
    },

    configureModelGroupKey: function(esn, kpe, kph, cb) {
        nrdp.qa._invoke("qa.device", "configureModelGroupKey",{
                        esn: esn,
                        kpe: kpe,
                        kph: kph},
                        cb);
    },
    configurePreshared: function(esn, kpe, kph, cb) {
        nrdp.qa._invoke("qa.device", "configurePreshared",{
                        esn: esn,
                        kpe: kpe,
                        kph: kph},
                        cb);
    },
    configureX509: function(cert, key, cb) {
        nrdp.qa._invoke("qa.device", "configureX509",{
                        cert: cert,
                        key: key},
                        cb);
    },
    getAuthenticationType: function(cb) {
        nrdp.qa._invoke("qa.device", "getAuthenticationType", null, cb);
    },
    getIpConnectivityMode: function(cb) {
        nrdp.qa._invoke("qa.device", "getIpConnectivityMode", null, cb);
    },
    sendCommand: function(command, cb) {
        nrdp.qa._invoke("qa.device", "sendCommand",{
                        command: command},
                        cb);
    },
    setCapability: function(capability, value, cb) {
        nrdp.qa._invoke("qa.device", "setCapability",{
                        capability: capability,
                        value: value},
                        cb);
    },
    toggleScreensaver: function(cb) {
        nrdp.qa._invoke("qa.device", "toggleScreensaver", null, cb);
    },
    setIpConnectivityMode: function(mode, cb) {
        nrdp.qa._invoke("qa.device", "setIpConnectivityMode", {ipConnectivityMode:mode}, cb);
    },
    setLanguage: function(language, cb) {
        nrdp.qa._invoke("qa.device", "setLanguage",{
                        language: language},
                        cb);
    },
    setProfiles: function(profiles, cb) {
        nrdp.qa._invoke("qa.device", "setProfiles",{
                        profiles: profiles},
                        cb);
    },
    setVideoOutputResolution: function(width, height, cb) {
        nrdp.qa._invoke("qa.device", "setVideoOutputResolution",{
                        width: width,
                        height: height},
                        cb);
    },
    setActiveVideoOutput: function(videoOutputStates, cb){
      nrdp.qa._invoke("qa.device", "setActiveVideoOutput",{
                      videoOutputStates: videoOutputStates},
                      cb);
    },
    setSupportedVideoOutput: function(videoOutputInfos, cb){
      nrdp.qa._invoke("qa.device", "setSupportedVideoOutput",{
                      videoOutputInfos: videoOutputInfos},
                      cb);
    },
    setVolumeControlType: function(volumeControlType, cb){
      nrdp.qa._invoke("qa.device", "setVolumeControlType",{
                      volumeControlType: volumeControlType},
                      cb);   
    },
    setVideoPipelineCapabilities: function(videoPipelineCapabilities, cb){
      nrdp.qa._invoke("qa.device", "setVideoPipelineCapabilities", 
                      {videoPipelineCapabilities:videoPipelineCapabilities},
                      cb);
    },
    setVideoProfileEnvelope: function(maxHEVCProfileEnvelope, maxAVCProfileEnvelope){
      nrdp.qa._invoke("qa.device", "setVideoProfileEnvelope", {
          maxHEVCProfileEnvelope:maxHEVCProfileEnvelope,
          maxAVCProfileEnvelope:maxAVCProfileEnvelope});
    },
    getVideoProfileEnvelope: function() {
        return nrdp._invoke("qa.device", "getVideoProfileEnvelope");
    },
    setDolbyVisionRendering: function(renderDolbyVisionBL, renderDolbyVisionEL){
        nrdp.qa._invoke("qa.device", "setDolbyVisionRendering", {
            renderDolbyVisionBL:renderDolbyVisionBL,
            renderDolbyVisionEL:renderDolbyVisionEL});
    },
    getDolbyVisionRendering: function() {
        return nrdp._invoke("qa.device", "getDolbyVisionRendering");
    },
    setDolbyVisionFileDump: function(fileName) {
        nrdp.qa._invoke("qa.device", "setDolbyVisionFileDump", {fileName:fileName});
    }
};

nrdp.qa.locationmonitor = {
    addEventListener: function(e, l) { nrdp._addEventListener("qa.locationmonitor", e, l); this._startorstop(); },
    removeEventListener: function(e, l) { nrdp._removeEventListener("qa.locationmonitor", e, l); this._startorstop(); },

    _startorstop: function() {
        if ( nrdp._hasEventListener( "qa.locationmonitor", "monitorEvent" ) ) {
            this.start();
        } else {
            this.stop();
        }
    },

    _handleEvent: function(event) {
        var evt = {
            type: event.name,
            data: event.data
        };
        nrdp._callEventListeners("qa.locationmonitor", evt);
        return true;
    },

    getHistory: function( cb ) {
        return nrdp.qa._invoke("qa.locationmonitor", "getHistory", null, cb );
    },

    setHistory: function( v ) {
        return nrdp.qa._invoke("qa.locationmonitor", "setHistory", { data: v }, null );
    },

    start: function() {
        nrdp.qa._invoke("qa.locationmonitor", "start", null, null );
    },
    stop: function() {
        nrdp.qa._invoke("qa.locationmonitor", "stop", null, null );
    }
};

nrdp.qa.texttospeech = {
    setVoice: function(v) {
	nrdp.qa._invoke("qa.texttospeech", "setVoice", { voice: v });
    },
    setPitchRatioHigh: function(v) {
        nrdp.qa._invoke("qa.texttospeech", "setPitchRatioHigh", { ratio: v });
    },
    setPitchRatioLow: function(v) {
        nrdp.qa._invoke("qa.texttospeech", "setPitchRatioLow", { ratio: v });
    },
    setRateRatioHigh: function(v) {
        nrdp.qa._invoke("qa.texttospeech", "setRateRatioHigh", { ratio: v });
    },
    setRateRatioLow: function(v) {
        nrdp.qa._invoke("qa.texttospeech", "setRateRatioLow", { ratio: v });
    }
};

})();
