// file name : fw/service/OIPF/emul/emul.js
/*jslint sloppy:true, nomen:true, vars:true */
console.log("[EMULATOR] " + 'Loaded [OIPFEmul.js]');
/* COMMENT
 * must delete hx.config in emulator scripts but,...
 * * bglee@humaxdigital.com, 21:51, 2014. 3. 22.
 */

var __DEF__ = require("def/config.def");
var hx = {
    config: {
        model: __DEF__.eProductName["FVP4000T"],
        useCAS: "IR"
    },
    log: function(aMessage) {
        console.warn("cannot use hx.log in files for emulation");
        console.log(aMessage);
    }
};

function DOM2EventSource() {
    this._eventListeners = [];
}

DOM2EventSource.prototype = {
    addEventListener: function (eventName, listener, useCapture) {
        this._eventListeners.push({
            'eventName': eventName,
            'listener': listener
        });
    },
    removeEventListener: function (eventName, listener, useCapture) {
        var that = this, idx;
        for (idx = 0; idx < that._eventListeners.length; idx += 1) {
            if (that._eventListeners[idx].eventName === eventName) {
                that._eventListeners.splice(idx, 1);
                break;
            }
        }
    },
    trigger: function () {
        var that = this, idx;
        var args = $.makeArray(arguments);
        var eventName = args.shift();
        for (idx = 0; idx < that._eventListeners.length; idx += 1) {
            if (that._eventListeners[idx].eventName === eventName) {
                that._eventListeners[idx].listener.apply(that, args);
            }
        }
        var handlerName = 'on' + eventName;
        if (typeof that[handlerName] === 'function') {
            that[handlerName].apply(that, args);
        }
    },
    trigger2: function (eventName, args) {
        var a = this, i;
        var evt = {};
        var values = [];
        args = args || [];
        for (i = 0; i < args.length; i++) {
            var argInfo = args[i];
            evt[argInfo.name] = argInfo.value;
            values.push(argInfo.value);
        }
        for (i = 0; i < a._eventListeners.length; i += 1) {
            if (a._eventListeners[i].eventName === eventName) {
                a._eventListeners[i].listener.call(a, evt);
            }
        }
        var handlerName = 'on' + eventName;
        if (a[handlerName]) {
            a[handlerName].apply(a, values);
        }
    },
    setAttribute: function () {
    }
};

// K.1 The Collection template
function Collection() {
}

Collection.prototype = [];
Collection.prototype.item = function (index) {
    return this[index];
};

function StringCollection() {
    return new Collection();
}

// prototype
function ParentalControlManager() {
}

// OIPF 7.1 Object factory API
var oipfObjectFactory = {
    // OIPF 7.1.1 Methods
    isObjectSupported: function () {
        return true;
    },
    // 7.1.1.1 Visual objects
    createVideoBroadcastObject: function () {
        gOipfEmul.videoBroadcastObject = new VideoBroadcast();
        return gOipfEmul.videoBroadcastObject;
    },
    createHumaxVideoBroadcastObject: function () {
        gOipfEmul.videoBroadcastObject = new VideoBroadcast();
        return gOipfEmul.videoBroadcastObject;
    },
    createVideoMpegObject: function () {
        gOipfEmul.avcontrolObject = new AVControl();
        return gOipfEmul.avcontrolObject;
    },
    createStatusViewObject: function () {
    },
    // 7.1.1.2 Non-Visual objects
    createApplicationManagerObject: function () {
        var am = new ApplicationManager();
        oipfObjectFactory.createApplicationManagerObject = function () {
            return am;
        };
        return am;
    },
    createCapabilitiesObject: function () {
    },
    createChannelConfig: function () {
        gOipfEmul.channelConfigObject = new ChannelConfig();
        return gOipfEmul.channelConfigObject;
    },
    createCodManagerObject: function () {
    },
    createConfigurationObject: function () {
        return new Configuration();
    },
    createDownloadManagerObject: function () {
    },
    createDownloadTriggerObject: function () {
    },
    createDrmAgentObject: function () {
        return new DrmAgent();
    },
    createGatewayInfoObject: function () {
    },
    createIMSObject: function () {
    },
    createMDTFObject: function () {
    },
    createNotifSocketObject: function () {
    },
    createParentalControlManagerObject: function () {
        return new ParentalControlManager();
    },
    createRecordingSchedulerObject: function () {
        return gOipfEmul.recordingScheduler;
        //      return new RecordingScheduler();
    },
    createRemoteControlFunctionObject: function () {
    },
    createRemoteManagementObject: function () {
    },
    createSearchManagerObject: function () {
        // TODO: object instance management requirend
        return new SearchManager();
    }
};

// 7.2.1 The application/oipfApplicationManager embedded object
var ApplicationManager = function () {
    var a = this;
    a.onLowMemory = null;
    a.onApplicationLoaded = null;
    a.onApplicationUnloaded = null;
    a.onApplicationLoadError = null;
    a.onWidgetInstallation = null;
    a.onWidgetUninstallation = null;
    ApplicationManager._instances.push(a);

    this._activateDocument = document;
    var that = this;
    this._getActiveDocument = function () {
        return that._activateDocument;
    };
};
ApplicationManager._instances = [];
ApplicationManager.prototype = new DOM2EventSource();
ApplicationManager.prototype.WIDGET_INSTALLATION_STARTED = 0;
ApplicationManager.prototype.WIDGET_INSTALLATION_COMPLETED = 1;
ApplicationManager.prototype.WIDGET_INSTALLATION_FAILED = 2;
ApplicationManager.prototype.WIDGET_UNINSTALLATION_STARTED = 3;
ApplicationManager.prototype.WIDGET_UNINSTALLATION_COMPLETED = 4;
ApplicationManager.prototype.WIDGET_UNINSTALLATION_FAILED = 5;
ApplicationManager.prototype.WIDGET_ERROR_STORAGE_AREA_FULL = 10;
ApplicationManager.prototype.WIDGET_ERROR_DOWNLOAD = 11;
ApplicationManager.prototype.WIDGET_ERROR_INVALID_ZIP_ARCHIVE = 12;
ApplicationManager.prototype.WIDGET_ERROR_INVALID_SIGNATURE = 13;
ApplicationManager.prototype.WIDGET_ERROR_GENERIC = 14;
ApplicationManager.prototype.WIDGET_ERROR_SIZE_EXCEEDED = 15;

ApplicationManager.prototype.widgets = new Collection();

ApplicationManager.prototype.getApplicationVisualizationMode = function () {
    return 1;
};

ApplicationManager.prototype.getOwnerApplication = function (document) {
    var own = new Application(this);
    this.getOwnerApplication = function (document) {
        return own;
    };
    return own;
};

ApplicationManager.prototype.getChildApplications = function (application) {
    return [];
};

ApplicationManager.prototype.gc = function () {
};
ApplicationManager.prototype._installedWidget;
ApplicationManager.prototype.installWidget = function (uri) {
    var a = this;
    function parseUrl(url) {
        var a = document.createElement('a');
        a.href = url;
        return a;
    }

    var splitted = uri.split('/');
    var wd = {
        name: splitted.pop(),
        id: 'linked://' + splitted.pop(),
        downloadURI: uri,
        defaultIcon: [],
        customIcons: []
    };
    setTimeout(function () {
        a.trigger2('WidgetInstallation', [
            {
                name: 'wd',
                value: wd
            }, {
                name: 'state',
                value: a.WIDGET_INSTALLATION_STARTED
            }, {
                name: 'reason',
                value: 0
            }]);
    }, 50);
    setTimeout(function () {
        ApplicationManager.prototype.widgets.push(wd);
        a.trigger2('WidgetInstallation', [
            {
                name: 'wd',
                value: wd
            }, {
                name: 'state',
                value: a.WIDGET_INSTALLATION_COMPLETED
            }, {
                name: 'reason',
                value: 0
            }]);
    }, 100);
};

ApplicationManager.prototype.uninstallWidget = function (wd) {
};

ApplicationManager.prototype.addOIPFApplication = function (wd) {
};

ApplicationManager.prototype.removeOIPFApplication = function (wd) {
};

ApplicationManager.prototype.removeAllOIPFApplications = function () {
};

// OIPF 7.2.2 The Application
var Application = function (appMgr, parent, url) {
    if (!arguments.length) {
        return;
    }
    var a = this;
    a._appMgr = appMgr;
    a._parent = parent;
    a.visible = true;
    a.active = true;
    a.permissions = [];
    a.isPrimaryReceiver = false;
    a.window = false;
    a.privateData = {
        wakeupChannel: null,
        wakeupTime: null,
        wakeupRepeatDays: null,
        wakeupVolume: null,
        prepareWakeupOITF: function (time, repeatDays, ccid, volume) {
            var b = this;
            if (typeof arguments[0] == 'undefined') {
                b.wakeupChannel = null;
                b.wakeupTime = null;
                b.wakeupRepeatDays = null;
                b.wakeupVolume = null;
            } else {
                b.wakeupChannel = ccid;
                b.wakeupTime = time;
                b.wakeupRepeatDays = repeatDays;
                b.wakeupVolume = volume;
            }
        }
    };
    if (url) {
        var prefix = "file:///usr/browser/webapps/";
        url = url.replace(prefix, '');
        a.window = new ApplicationWindow(a, url);
    }
};
Application.prototype = new DOM2EventSource();
Application.prototype.createApplication = function (uri, createChild) {
    if (this._children === undefined) {
        this._children = [];
    }
    var a = this._children;
    var l = a.push(new Application(this._appMgr, this, uri));
    return a[l - 1];
};
Application.prototype.destroyApplication = function () {
    if (this._children === undefined) {
        return;
    }
    var a = this._children, l = a.length, n;
    for (n = 0; n < l; n += 1) {
        this._children[n].destroyApplication();
        delete this._children[n];
    }
};
Application.prototype.startWidget = function (wd, createChild, paramUrl) {
    var a = this;
    return a.createApplication(paramUrl, createChild);
};
Application.prototype.stopWidget = function (wd) {
    var a = this;
    a.destroyApplication();
};
Application.prototype.show = function () {
    var a = this;
    if (a.window) {
        a.visible = true;
        a.window._$iframe.show();
    }
};
Application.prototype.hide = function () {
    var a = this;
    if (a.window) {
        a.visible = false;
        a.window._$iframe.hide();
    }
};
Application.prototype.activateInput = function (b) {
    if (this._parent === undefined) {
        if (b) {
            this._appMgr._activateDocument = document.body;
        }
        return;
    }
    this._parent._requestActviateInput(this, b);
};
Application.prototype._requestActviateInput = function (child, b) {
    if (this._children === undefined) {
        return;
    }
    if (this._activateList === undefined) {
        this._activateList = [];
    }
    var a = this._children, l = a.length, n;
    for (n = 0; n < l; n += 1) {
        if (a[n] === child) {
            var o = this._activateList;
            o.push(child);
            child.window._$iframe.css("z-index", o.length * 100 + 1000);
            if (b) {
                this._appMgr._activateDocument = child.window._$iframe[0].contentDocument;
            }
            break;
        }
    }
};
Application.prototype.deactivateInput = function () {
    if (this._parent === undefined) {
        return;
    }
    this._parent._requestDeactivateInput(this);
};
Application.prototype._requestDeactivateInput = function (child) {
    if (this._children === undefined || this._activateList === undefined || this._activateList.length < 1) {
        return;
    }
    var a = this._children, l = a.length, n;
    for (n = 0; n < l; n += 1) {
        if (a[n] === child) {
            this._activateList = [].concat(a.slice(0, n), a.slice(n + 1, a.length));
            break;
        }
    }
};

/*
Application.prototype.onApplicationActivated = function() {
};
Application.prototype.onApplicationDeactivate = function() {
};
Application.prototype.onApplicationShown = function() {
};
Application.prototype.onApplicationHidden = function() {
};
Application.prototype.onApplicationPrimaryReceiver = function() {
};
Application.prototype.onApplicationNotPrimaryReceiver = function() {
};
Application.prototype.onApplicationTopmost = function() {
};
Application.prototype.onApplicationNotTopmost = function() {
};
Application.prototype.onApplicationDestroyRequest = function() {
};
Application.prototype.onApplicationHibernateRequest = function() {
};
Application.prototype.onKeyPress = function() {
};
Application.prototype.onKeyUp = function() {
};
Application.prototype.onKeyDown = function() {
};
*/
var ApplicationWindow = function (app, url) {
    var a = this;
    if (arguments.length) {
        a.init(app, url);
    }
};
ApplicationWindow.prototype.init = function (app, url) {
    var a = this;
    a._app = app;
    a._messageQueue = [];
    a._loadingDone = false;
    a._$iframe = $("<iframe>", {
        src: url,
        style: "display: none; position:absolute; left:0px; top:0px; width:1280px; height:720px;z-index:1004;",
        frameborder: 0,
        scrolling: "no"
    }).load(function () {
        console.log('application loaded');
        a._loadingDone = true;
        a._app._appMgr.trigger2('ApplicationLoaded', [{
                name: 'appl',
                value: a._app
            }]);
    });
    $('body').append(a._$iframe);
    return this;
};
ApplicationWindow.prototype.postMessage = function (message) {
    var a = this;
    a._messageQueue.push(message);
    a._postMessage();
};
ApplicationWindow.prototype._triggerPostMessage = function (time) {
    var a = this;
    setTimeout(function () {
        a._postMessage();
    }, time);
};
ApplicationWindow.prototype._postMessage = function () {
    var a = this;
    if (!a._loadingDone || !!a._critical) {
        a._triggerPostMessage(100);
        return;
    }
    if (a._messageQueue.length < 1) {
        return;
    }

    a._critical = true;
    var m = a._messageQueue.shift();
    try  {
        var ev = document.createEvent("MessageEvent");
    } catch (e) {
        console.log("[EMULATOR] " + "createEvent error : " + e);
    }

    var origin = window.location.protocol + "//" + window.location.host;
    ev.initMessageEvent("message", true, true, m, origin, 0, window, null);
    a._$iframe[0].contentWindow.dispatchEvent(ev);

    a._critical = false;

    a._triggerPostMessage(10);
};

// OIPF
// OIPF 7.3.2 The Application / oipfConfiguration embedded object
function Configuration() {
    var that = this;

    // OIPF 7.3.2 The Configuration class
    // OIPF 7.3.2.1 Properties
    that.preferredAudioLanguage = 'eng';
    that.preferredSubtitleLanguage = 'eng';
    that.preferredMenuLanguage = 'eng';
    if (hx.config.model === 'IR4000HD') {
        that.countryId = 'UAE';
    } else if (hx.config.model === 'HMS1000S') {
        that.countryId = 'DEU';
    } else {
        that.countryId = 'ENG';
    }

    that.regionId = 0;
    that.pvrPolicy = 0;
    that.channelsetupPin = false;
    that.targetStorage = 1;

    // type map = Configuration.TPvrPolicy
    that.pvrSaveEpisodes = 2;
    that.pvrSaveDays = 7;
    that.pvrStartPadding = 300;
    that.pvrEndPadding = 300;
    that.timeShiftEnabled = false;
    that.descrambleOnOff = false;
    that.autoDelete = true;
    that.dayLightSaving = 0;
    that.offsetGMT = 240;
    that.updatetime = false;
}
;

// Configuration Constants
Configuration.TPvrPolicy = {
    ENoRecordingsAreToBeDeleted: 0,
    EOnlyWatchedRecordingsMAYBeDeleted: 1,
    EOnlyRecordingsOlderThanTheSpecifiedThresholdMAYBeDeleted: 2
};
Configuration.prototype = {
    // OIPF 7.3.1.1 Configuration Properties
    configuration: null,
    localSystem: null,
    // OIPF 7.3.2.2 Methods
    getText: function (key) {
        var defaultValue = {
            systemTexts: {
                'no_title': 'No information',
                'no_synopsis': 'No further information available',
                'blocked_title': 'BLOCKED',
                'blocked_synopsis': 'Program blocked by user',
                'manual_recording': 'Manual Recording'
            }
        };
        if (defaultValue.systemTexts.hasOwnProperty(key)) {
            var result = getCookie(key);
            return result === null ? defaultValue.systemTexts[key] : result;
        }
        return '';
    },
    setText: function (key, value) {
        setCookie(key, value, 3);
    },
    setServerActivationTime: function(a,b,c,d) {
        return;
    }
};

// OIPF 7.3.3 The LocalSystem class
function LocalSystem() {
    var that = this;

    // OIPF 7.3.3.2 Properties
    that.deviceID = 'X-HNI-IGI-OITF-DeviceID';
    that.systemReady = true;
    that.vendorName = 'HUMAX';
    switch (hx.config.model) {
        case 'HMS1000S':
            that.modelName = 'HMS1000S';
            that.softwareVersion = 'DESFAB 0.00.03';
            that.ciplusEnabled = true;
            break;
        case 'IR4000HD':
            that.modelName = 'IR4000HD';
            that.softwareVersion = 'MESIBE 0.00.03';
            that.ciplusEnabled = false;
            break;
        case 'HMS1000T':
            that.modelName = 'HMS1000T';
            that.softwareVersion = 'TBD 0.00.01';
            that.ciplusEnabled = false;
            break;
        case 'ICORDPRO':
            that.modelName = 'ICORDPRO';
            that.softwareVersion = 'DESFAB 0.00.03';
            that.ciplusEnabled = true;
            break;
        case __DEF__.eProductName.FVP4000T:
            that.modelName = 'FVP4000T';
            that.softwareVersion = 'TBD 0.00.01';
            that.ciplusEnabled = false;
            break;
    }
    that.hardwareVersion = '1.0.0';
    that.serialNumber = '3633373130363036303030303234';
    that.updatedList = { "uitype": "updatedlist", "uidata": { "otaversion": 1, "listitem": [[{ "pagenum": 1, "type": ["defaultimage"] }, { "index": 0, "stringlength": 2, "string": ["serial number1"] }], [{ "pagenum": 2, "type": ["defaultimage"] }, { "index": 0, "stringlength": 2, "string": ["serial number2"] }], [{ "pagenum": 3, "type": ["defaultimage"] }, { "index": 0, "stringlength": 2, "string": ["serial number3_1"] }, { "index": 1, "stringlength": 2, "string": ["serial number3_2"] }, { "index": 2, "stringlength": 2, "string": ["serial number3_3"] }]] } };
    that.releaseVersion = '2';
    that.majorVersion = '2';
    that.minorVersion = '0';
    that.oipfProfile = '';
    that.pvrEnabled = true;
    that.standbyState = false;
    that.powerState = LocalSystem.TPowerState.EON;
    that.previousPowerState = LocalSystem.TPowerState.EOFF;
    that.timeCurrentPowerState = 500;
    that.volum = 50;
    that.mute = false;
    that.outputs = null;
    that.tuners = null;
    that.networkInterfaces = null;

    //    that.tuners = null;
    that.tvStandard = LocalSystem.TTvStandard.EPAL;
    that.pvrSupport = true;
    that.boxFirstBootStatus = false;
    if (hx.config.useCAS === 'IR') {
        that.irdeto_virtualscw = false;
        that.irdeto_jtag = 'X';
        that.irdeto_cwe = 'X';
        that.irdeto_sboot = 'X';
        that.irdeto_otp = 'X';
        that.irdeto_mancode = '015';
        that.irdeto_hwcode = '070';
        that.irdeto_variant = '011';
        that.irdeto_sysid = '011';
        that.irdeto_sigver = '000';
        that.irdeto_keyver = '001';
    }
//    function CTrdConflict() {
//        this.onTrdConflict = null;
//        var random = Math.floor((Math.random()*9)+1);
//        var DUMMY = "Dummy0";
//        var list = [];
//        for (var i = 0; i < random; i++) {
//            list[i] = DUMMY + i;
//        }
//        this._list = list;
//    }
//    CTrdConflict.prototype.GetTrdConflict = function() {
//        if (!this.onTrdConflict) {
//            throw 'get return data via onTrdConflict';
//        }
//        this.onTrdConflict(this._list.join(","));
//    };
//    this.trdconflict = new CTrdConflict();
}

// LocalSystem Constants
LocalSystem.TPowerState = {
    EOFF: 0,
    EON: 1,
    EPASSIVE_STANDBY: 2,
    EACTIVE_STANDBY: 3,
    EPASSIVE_STANDBY_HIBERNATE: 4
};
LocalSystem.TTvStandard = {
    ENTSC: 1,
    EPAL: 2,
    ESECAM: 3
};

LocalSystem.prototype = new DOM2EventSource();
$.extend(true, LocalSystem.prototype, {
    // OIPF 7.3.3.3 Method
    setScreenSize: function (width, height) {
    },
    setPvrSupport: function (state) {
        this.pvrSupport = state;
    },
    setPowerState: function (type) {
        this.previousPowerState = this.powerState;
        this.powerState = type;
        this.PowerStateChange(type);
    },
    setDigestCredentials: function (protocol, domain, username, password) {
    },
    clearDigestCredentials: function (protocol, domain) {
    },
    onPowerStateChange: function (powerState) {
        this.powerState = powerState;
        // need something to do
    },
    // OIPF 7.3.3.4 Events (?????)
    PowerStateChange: function (powerState) {
        if (this.onPowerStateChange) {
            this.onPowerStateChange(powerState);
        }
    },
    setApplicationReady: function () {
    },
    showMsgbox: function () {
    },
    getStatusOfTVBrowser: function () {
    },
    __fnPowerWakeupKey: function (e) {
        var a = this;
        if (e.type === 'keydown' && e.keyCode === 9001) {
            if (a.powerState === 2) {
                setTimeout(function () {
                    a.setPowerState(1);
                }, 1);
            }
        }
    },
    checkFileExistence: function (uri) {
        return true;
    }
});

// OIPF 7.3.4 NetworkInterface class
function NetworkInterface() {
    var that = this;

    // OIPF 7.3.4.1 Properties
    that.ipAddress = '192.168.0.35';
    //that.macAddress = '00:03:78:D0:58:AD';

    //that.macAddress = '01:02:03:04:05:06';
    that.macAddress = '00:03:78:4B:7D:F5';
    that.connected = true;
    that.enabled = true;
    NetworkInterface.prototype.instances.push(that);
}

NetworkInterface.prototype = new DOM2EventSource();
NetworkInterface.prototype.instances = [];

// OIPF 7.3.5 AVOutput class
function AVOutput() {
    var that = this;

    // OIPF 7.3.5.1 Properties
    that.name = 'all';
    that.type = 'video';

    // Valid values: "audio", "video", "both"
    that.enabled = true;
    that.subtitleEnabled = false;
    that.videoMode = 'normal';

    // Valid values: "normal", "stretch", "zoom"
    that.digitalAudioMode = "uncompressed";

    // Valid values: "ac3", "uncompressed"
    that.audioRange = 'normal';

    // Valid values: "normal", "narrow", "wide"
    that.hdVideoFormat = '480p';

    // Valid values: "480i", "480p", "576i", "576p", "720p", "1080i", "1080p"
    that.tvAspectRatio = '16:9';

    // Valid values: "4:3", "16:9"
    that.supportedVideoModes = {};

    // readonly StringCollection
    that.supportedDigitalAudioModes = {};

    // readonly StringCollection
    that.supportedAudioRanges = {};

    // readonly StringCollection
    // that.supportedHdVideoFormats = {};
    that.supportedHdVideoFormats = new supportedHdVideoFormatCollection();

    // setInterval(function() {
    // if(that.flag) {
    // that.supportedHdVideoFormats = new supportedHdVideoFormatCollection();
    // that.flag = false;
    // } else {
    // that.supportedHdVideoFormats = new supportedHdVideoFormatCollection();
    // that.supportedHdVideoFormats.push('480p');
    // that.supportedHdVideoFormats.push('720p');
    // that.supportedHdVideoFormats.push('Original');
    // that.flag = true;
    // }
    // }, 20000);
    that.supportedHdVideoFormats.push('480p');
    that.supportedHdVideoFormats.push('720p');
    that.supportedHdVideoFormats.push('Original');

    // readonly StringCollection
    that.supportedAspectRatios = {};
    // readonly StringCollection
}

// OIPF 7.3.6 NetworkInterfaceCallection class
function NetworkInterfaceCollection() {
    var netInterface1 = new NetworkInterface(), netInterface2 = new NetworkInterface();

    netInterface2.connected = true;
    netInterface2.networkType = 1;

    if (false) {
        netInterface1.connected = false;
        netInterface1.networkType = 0;
    }

    this.push(netInterface1);
    this.push(netInterface2);
}

NetworkInterfaceCollection.prototype = new Collection();

// OIPF 7.3.7 AVOutputCollection class
function AVOutputCollection() {
}

AVOutputCollection.prototype = new Collection();

// OIPF 7.3.8 TunerCollection class
function TunerCollection() {
}

TunerCollection.prototype = new Collection();

// OIPF 7.3.9 Tuner class
function Tuner() {
    var that = this;

    // OIPF 7.3.9.1 Properties
    that.id = '';
    that.name = '';
    that.idTypes = {};

    // readonly StringCollection
    that.enableTuner = true;
    that.signalInfo = {};

    // readonly [Instance of SignalInfo]
    that.powerOnExternal = true;
}

// OIPF 7.3.10 SignalInfo class
function SignalInfo() {
    var that = this;

    // OIPF 7.3.10.1 Properties
    that.strength = parseInt(Math.random() * 100);

    // Signal strength measured in dBm
    that.quality = parseInt(Math.random() * 100);

    // range from 0 to 100
    that.ber = 0;

    // Bit error rate
    that.snr = 22.3;

    // Signal to noise ratio (dB)
    that.lock = true;
}

// OIPF 7.4 Content download APIs
function DownloadTrigger() {
}

DownloadTrigger.prototype = {
    // OIPF 7.4.1.1 Method
    registerDownload: function (contentAccessDownloadDescriptor, downloadStart) {
    },
    registerDownloadURL: function (URL, contentType, downloadStart) {
    },
    checkDownloadPossible: function (sizeInBytes) {
    },
    // OIPF 7.4.2 Extensions to application / oipfDownloadTrigger
    registerDownloadFromCRID: function (CRID, IMI, downloadStart) {
    }
};

// OIPF 7.4.3 The application / oipfDownloadManager embedded object
function DownloadManager() {
    this.discInfo = {};
}

DownloadManager.prototype = new DOM2EventSource();
$.extend(true, DownloadManager.prototype, {
    // OIPF 7.4.3.3 Methods
    pause: function (download) {
        return true;
    },
    resume: function (download) {
        return true;
    },
    remove: function (download) {
        return true;
    },
    getDownloads: function (id) {
        return new DownloadCollection();
    },
    createFilteredList: function (currentDomain, states) {
        return new DownloadCollection();
    },
    updateRegisteredDownload: function (download, newURL) {
        return true;
    },
    // OIPF 7.4.3.4 Events
    DownloadStateChange: function (item, state, reason) {
        if (this.onDownloadStateChange) {
            this.onDownloadStateChange(item, state, reason);
        }
    }
});

// OIPF 7.4.4 The Download class
function Download() {
    // OIPF 7.4.4.2 Properties
    var that = this;
    that.totalSize = 0;
    that.state = Download.TState.EDOWNLOAD_FAILED;
    that.reason = 0;
    that.amountDownloaded = 0;
    that.currentBitrate = 0;
    that.name = '';
    that.id = '';
    that.contentURL = 'thhp://localhost/';
    that.description = '';
    that.parentalRatings = {};
    that.drmControl = {};
    that.startTime = new Date();
    that.timeElapsed = 0;
    that.timeRemaining = 100;
    that.transferType = 'playable_download';
    that.originSite = '';
    that.originSiteName = '';
    that.contentID = '';
    that.iconURL = '';
}

// OIPF 7.4.4.1 Constants
Download.TState = {
    EDOWNLOAD_COMPLETED: 1,
    EDOWNLOAD_IN_PROGRESS: 2,
    EDOWNLOAD_PAUSED: 4,
    EDOWNLOAD_FAILED: 8,
    EDOWNLOAD_NOT_STARTED: 16,
    EDOWNLOAD_STALLED: 32
};

// OIPF 7.4.5 DownloadCollection class
function DownloadCollection() {
}

DownloadCollection.prototype = new Collection();

// OIPF 7.4.6 The DRMControlInformation class
function DRMControlInformation() {
    // OIPF 7.4.6.1 Properties
    var that = this;
    that.drmType = 'urn:dvb:casystemid:19188';
    that.rightsIssuerURL = '';
    that.silentRightsURL = '';
    that.drmContentID = '';
    that.previewRightsURL = '';
    that.drmPrivateData = '';
    that.doNotRecord = true;
    that.doNotTimeShift = true;
}

// OIPF 7.4.7 The DRMControlInfoCollection class
function DRMControlInfoCollection() {
}

DRMControlInfoCollection.prototype = new Collection();

// OIPF 7.5 Content On Demand Metadata APIs
// 7.5.1 The application/oipfCodManager embedded object
function CODManager() {
    // OIPF 7.5.1.1 Properties
    this.catalogues = {};
}

CODManager.prototype = new DOM2EventSource();

$.extend(true, CODManager.prototype, {
    // OIPF 7.5.1.2 Events
    ContentCatalogueEvent: function (action) {
        var fn = this.onContentCatalogueEvent;
        if (fn) {
            fn(action);
        }
    },
    ContentAction: function (action, result, item, catalogue) {
        var fn = this.onContentAction;
        if (fn) {
            fn(action, result, item, catalogue);
        }
    }
});

// OIPF 7.5.2 The CatalogueCollection class
function CatalogueCollection() {
}

CatalogueCollection.prototype = new Collection();

// OIPF 7.5.3 The ContentCatalogue class
function ContentCatalogue() {
    var that = this;

    // OIPF 7.5.3.1 Properties
    that.name = '';
    that.rootFolder = {};
}

ContentCatalogue.prototype = {
    // OIPF 7.5.3.2 Methods
    getPurchaseHistory: function () {
        return new CODFolder();
    }
};

// OIPF 7.5.4 The ContentCatalogueEvent class
// OIPF 7.5.5 The CODFolder class
function CODFolder() {
    var that = this;
    that.type = 0;
    that.uri = '';
    that.name = '';
    that.description = '';
    that.thumbnailUri = '';
    that.length = 0;
    that.currentPage = 1;
    that.pageSize = 10;
    that.totalSize = 100;
}

CODFolder.prototype = {
    // OIPF 7.5.5.2 Methods
    item: function () {
        return {};
    },
    getPage: function (page, pageSize) {
    },
    abort: function () {
    }
};

// OIPF 7.5.6 The CODAsset class
function CODAsset() {
    var that = this;

    // OIPF 7.5.6.1 Properties
    that.type = 1;
    that.uri = '';
    that.name = '';
    that.description = '';
    that.genres = {};
    that.parentalRating = {};
    that.blocked = false;
    that.locked = false;
    that.thumbnailUri = '';
    that.price = '';
    that.rentalPeriod = 1;
    that.playCount = 1;
    that.duration = 1;
    that.previewUri = '';
    that.bookmarks = {};
}

CODAsset.prototype = {
    // OIPF 7.5.6.2 Methods
    isReady: function () {
        return true;
    },
    lookupMetadata: function (key) {
        return {};
    }
};

// OIPF 7.5.7 The CODService class
function CODService() {
    var that = this;

    // OIPF 7.5.7.1 Properties
    that.length = 10;
    that.currentPage = 1;
    that.pageSize = 10;
    that.totalSize = 100;
    that.type = 2;
    that.uid = '';
    that.uri = '';
    that.name = '';
    that.description = '';
    that.thumbnailUri = '';
    that.previewUri = '';
}

CODService.prototype = {
    // OIPF 7.5.7.1.1
    item: function (index) {
        return {};
    },
    getPage: function (page, pageSize) {
    },
    abort: function () {
    },
    isReady: function () {
        return true;
    },
    lookupMetadata: function (key) {
        return {};
    }
};

// OIPF 7.6 Content Service Protection API
// OIPF 7.6.1 application / oipfDrmAgent embedded object
function DrmAgent() {
}

DrmAgent.TDRMSystemStatus = {
    EREADY: 0,
    EUNKNOWN: 1,
    EINITIALISING: 2,
    EERROR: 3
};

DrmAgent.prototype = new DOM2EventSource();
$.extend(true, DrmAgent.prototype, {
    // OIPF 7.6.1.2 Methods
    sendDRMMessage: function (msgType, msg, DRMSystemID) {
        return '';
    },
    DRMSystemStatus: function (DRMSystemID) {
        return DrmAgent.TDRMSystemStatus.EREADY;
    },
    // OIPF 7.6.1.3 Events
    DRMMessageResult: function (msgID, resultMsg, resultCode) {
        var fn = this.onDRMMessageResult;
        if (fn) {
            fn(msgID, resultMsg, resultCode);
        }
    },
    DRMSystemStatusChange: function (DRMSystemID) {
        var fn = this.onDRMSystemStatusChange;
        if (fn) {
            fn(DRMSystemID);
        }
    },
    DRMSystemMessage: function (msg, DRMSystemID) {
        var fn = this.onDRMSystemMessage;
        if (fn) {
            fn(msg, DRMSystemID);
        }
    }
});

// 7.7 Gateway Discovery and Control APIs
// 7.7.1 The application / oipfGatewayInfo embedded object
function GatewayInfo() {
    var that = this;

    // OIPF 7.7.1.1 Properties
    that.isIGSupported = true;
    that.isAGSupported = true;
    that.isCSPGCIPlusSupported = true;
    that.isCSPGDTCPSupported = true;
    that.isIGDiscovered = true;
    that.isAGDiscovered = true;
    that.isCSPGCIPlusDiscovered = true;
    that.igURL = '';
    that.agURL = '';
    that.cspgDTCPURL = '';
    that.interval = 10;
    that.CSPGCIPlusDRMType = {};
}

GatewayInfo.prototype = {
    // OIPF 7.7.1.2 Methods
    isIGSupportedMethod: function (MethodName) {
    },
    // OIPF 7.7.1.3 Events
    DiscoverIG: function () {
        var fn = this.onDiscoverIG;
        if (fn) {
            fn();
        }
    },
    DiscoverAG: function () {
        var fn = this.onDiscoverAG;
        if (fn) {
            fn();
        }
    },
    DiscoverCSPGCIPlus: function () {
        var fn = this.onDiscoverCSPGCIPlus;
        if (fn) {
            fn();
        }
    },
    DiscoverCSPGDTCP: function () {
        var fn = this.onDiscoverCSPGDTCP;
        if (fn) {
            fn();
        }
    }
};

// OIPF 7.8 Communication Services APIs
// 7.8.1 The application / oipfCommunicationServices embedded object
function CommunicationServices() {
    // OIPF 7.8.1.2 Properties
    this.currentUser = {};
}

CommunicationServices.prototype = new DOM2EventSource();

// OIPF 7.8.1.1 Constants
CommunicationServices.TState = {
    ESTATE_REGISTERED: 0,
    ESTATE_REGISTERED_SUBSCRIPTION_PENDING: 1,
    ESTATE_REGISTERED_SUBSCRIPTION_ACTIVE: 2,
    ESTATE_DEREGISTERED: 3,
    ESTATE_FAILURE: 4
};

CommunicationServices.prototype = {
    // OIPF 7.8.1.3 Methods
    getRegisteredUsers: function () {
        return new UserDataCollection();
    },
    registerUser: function (userId, pin) {
    },
    deRegisterUser: function (userId) {
        return true;
    },
    getAllUesrs: function () {
        return new UserDataCollection();
    },
    setUser: function (userId) {
        return true;
    },
    subscribeNotification: function (featureTagCollection, performUserRegistration) {
    },
    unsubscribeNotification: function () {
    },
    // OIPF 7.8.1.4 Events
    NotificationResult: function (resultMsg) {
        var fn = this.onNotificationResult;
        if (fn) {
            fn(resultMsg);
        }
    },
    Notification: function (callId, contact, from, to) {
        var fn = this.onNotification;
        if (fn) {
            fn(callId, contact, from, to);
        }
    },
    RegistrationContextUpdate: function (user, state, errorCode) {
        var fn = this.onRegistrationContextUpdate;
        if (fn) {
            fn(user, state, errorCode);
        }
    }
};

// OIPF 7.8.2 Extensions to application/oipfCommunicationServices for presence and messaging services
// OIPF 7.8.2.1 Properties
$.extend(true, CommunicationServices.prototype, {
    // OIPF 7.8.2.2 Methods
    openChatSession: function (toURI) {
        return 1;
    },
    sendMessageInSession: function (cid, msg) {
    },
    closeChatSession: function (cid) {
    },
    sendMessage: function (toURI, msg) {
    },
    setStatus: function (state) {
    },
    subscribeToStatus: function (remoteURI) {
    },
    getContacts: function () {
        return new ContactCollection();
    },
    allowContact: function (remoteURI) {
    },
    blockContact: function (remoteURI) {
    },
    createContactList: function (contactListUri, contacts) {
        return true;
    },
    addToContactList: function (contactListUri, member) {
        return true;
    },
    removeFromContactList: function (contactListUri, member) {
        return true;
    },
    deleteContactList: function (contactListUri) {
        return true;
    },
    allowAllContacts: function (domain) {
    },
    blockAllContacts: function (domain) {
    },
    // OIPF 7.8.2.3 Events
    IcomingMessage: function (fromURI, msg, cid) {
        var fn = this.onIncomingMessage;
        if (fn) {
            fn(fromURI, msg, cid);
        }
    },
    ContactStatusChange: function (remoteURI, present) {
        var fn = this.onContactStatusChange;
        if (fn) {
            fn(remoteURI, present);
        }
    },
    NewWatcher: function (remoteURI) {
        var fn = this.onNewWatcher;
        if (fn) {
            fn(remoteURI);
        }
    }
});

// 7.9.1 The application/oipfParentalControlManager embedded object
function ParentalControlManager() {
    var that = this;

    // 7.9.1.1 Properties
    that.parentalRatingSchemes = new ParentalRatingSchemeCollection();

    //{};
    that.parentalRatingSchemes.push(new ParentalRatingScheme());

    that.isPINEntryLocked = true;
}

var tempPcPIN = '0000';

// 7.9.1.2 Methods
ParentalControlManager.prototype = {
    setParentalControlStatus: function (pcPIN, enable) {
        // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
        return 0;
    },
    getParentalControlStatus: function () {
        return true;
    },
    getBlockUnrated: function () {
        return true;
    },
    setParentalControlPIN: function (oldPcPIN, newPcPIN) {
        // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
        if (tempPcPIN === '----') {
            tempPcPIN = oldPcPIN;
        } else if (tempPcPIN != oldPcPIN) {
            return 1;
        }

        if (tempPcPIN == oldPcPIN) {
            tempPcPIN = newPcPIN;
            return 0;
        } else {
            return 1;
        }
    },
    unlockWithParentalControlPIN: function (pcPIN, target) {
        // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked, 3: Invalid object
        return 0;
    },
    verifyParentalControlPIN: function (pcPIN) {
        // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
        if (tempPcPIN === pcPIN) {
            return 0;
        } else {
            return 1;
        }
    },
    setBlockUnrated: function (pcPIN, block) {
        // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
        return 0;
        // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
    },
    resetParentalControlPIN: function (pcPIN) {
        return 0;
    }
};

//OIPF 7.9.2 The ParentalRatingScheme class
function ParentalRatingScheme() {
    var that = this;

    // OIPF 7.9.2.1 Properties
    that.name = "dvb-si";
    that.threshold = new ParentalRating();
    //{};
    // ParentalRating
}

ParentalRatingScheme.prototype = new Collection();
$.extend(true, ParentalRatingScheme.prototype, {
    indexOf: function (ratingValue) {
        return 0;
    },
    iconUri: function (index) {
        return '';
    },
    changeThreshold: function (value) {
        this.threshold.value = value;
    }
});

//OIPF 7.9.3 The ParentalRatingSchemeCollection class
function ParentalRatingSchemeCollection() {
}

$.extend(true, ParentalRatingSchemeCollection, {});

ParentalRatingSchemeCollection.prototype = new Collection();

//OIPF 7.9.3.1 Method
$.extend(true, ParentalRatingSchemeCollection.prototype, {
    addParentalRatingScheme: function (name, values) {
        var newScheme = new ParentalRatingScheme();

        this.push(newScheme);

        return newScheme;
    },
    getParentalRatingScheme: function (name) {
        var i, len;

        len = this.length;
        for (i = 0; i < len; i += 1) {
            var Scheme = this.item(i);
            if (Scheme.name == name)
                return Scheme;
        }
        return null;
    }
});

//OIPF The ParentalRating calss
function ParentalRating() {
}
;

$.extend(true, ParentalRating.prototype, {
    name: '',
    scheme: '',
    value: 0,
    labels: 1,
    region: 'KR'
});

//OIPF 7.9.5 The ParentalRatingCollection
function ParentalRatingCollection() {
}
;

ParentalRatingCollection.prototype = new Collection();

//OIPF 7.9.5.1 Methods
ParentalRatingCollection.prototype.addParentalRating = function (scheme, name, value, labels, region) {
    var pr = new ParentalRating();
    pr.scheme = scheme;
    pr.name = name;
    pr.value = value;
    pr.labels = labels;
    pr.region = region;

    this.push(pr);
};

//7.10 Scheduled Recording APIs
//7.10.1 The application/oipfRecordingScheduler embedded object
function RecordingScheduler() {
}
;

//7.10.1.1 Methods
RecordingScheduler.prototype = {
    record: function (programme) {
        return new ScheduledRecording();
    },
    recordAt: function (startTime, duration, repeatDays, channelID) {
        return new ScheduledRecording();
    },
    getScheduledRecordings: function () {
        //return new ScheduledRecordingCollection();
        return gOipfEmul.scheduledRecordingCollection;
    },
    getChannelConfig: function () {
        return new ChannelConfig();
    },
    remove: function (recording) {
        var cnt, index = gOipfEmul.scheduledRecordingCollection.length;
        for (cnt = 0; cnt < index; cnt += 1) {
            if (gOipfEmul.scheduledRecordingCollection[cnt].programmeID === recording.programmeID) {
                var pop;
                for (pop = cnt; pop <= index; pop += 1) {
                    gOipfEmul.scheduledRecordingCollection[pop] = gOipfEmul.scheduledRecordingCollection[pop + 1];
                }
                gOipfEmul.scheduledRecordingCollection.length--;
                break;
            }
        }
    },
    createProgrammeObject: function () {
        return new Programme();
    },
    getRecordingList: function() {
        return gOipfEmul.recordingScheduler.recordings;
    },
    getScheduleList: function() {
        return gOipfEmul.scheduledRecordingCollection;
    }
};

//7.10.4 Extension to application/oipfRecordingScheduler for control of recordings
$.extend(true, RecordingScheduler.prototype, {
    //7.10.4.1 Properties
    recordings: {},
    discInfo: {},
    //7.10.4.2 Methods
    getRecording: function (id) {
        return new Recording();
    },
    stop: function (recording) {
    },
    refresh: function () {
    },
    //7.10.4.3 Events
    PVREvent: function (state, recording) {
    }
});

//7.10.2 The ScheduledRecording class
var ScheduledRecording = function () {
    var that = this;
    that.startPadding = 0;
    that.endPadding = 0;
    that.repeatDays = 0;
    that.name = '';
    that.longName = '';
    that.description = '';
    that.longDescription = '';
    that.startTime = 0;
    that.duration = 0;
    that.channel = null;
    that.programmeID = '';
    that.programmeIDType = ScheduledRecording.TProgrammeIDType.EID_TVA_CRID;
    that.episode = 0;
    that.totalEpisodes = 0;
    that.parentalRatings = {};
    that.groupCRIDs = {};
};

//7.10.2.1 Constants
ScheduledRecording.TProgrammeIDType = {
    EID_TVA_CRID: 0,
    EID_DVB_EVENT: 1,
    EID_TVA_GROUP_CRID: 2
};

//7.10.3 The ScheduledRecordingCollection class
function ScheduledRecordingCollection() {
}
;

ScheduledRecordingCollection.prototype = new Collection();

//7.10.5 The Recording class
function Recording() {
    var that = this;
    that.state = Recording.TState.ERECORDING_REC_COMPLETED;
    that.error = Recording.TError.EERROR_REC_RESOURCE_LIMITATION;
    that.id = "";
    that.isManual = true;
    that.doNotDelete = true;
    that.saveDays = 0;
    that.saveEpisodes = 0;
    that.blocked = true;
    that.showType = 0;

    // 0: live, 1: first-run show, 2: rerun
    that.subtitles = true;
    that.subtitleLanguages = {};

    // readonly StringCollection
    that.isHD = true;
    that.isAD = true;
    that.isWidescreen = true;
    that.audioType = 4;

    // 1: Mono, 2:: Stereo, 3: Multi-channel
    that.isMultilingual = true;
    that.audioLanguages = {};

    // readonly StringCollection
    that.genres = {};

    // readonly StringCollection
    that.recordingStartTime = Math.ceil(new Date().getTime() / 1000);
    that.recordingDuration = 1000;
    that.bookmarks = new BookmarkCollection();

    // readonly BookmarkCollection
    that.locked = false;
    that.groupCRIDs = {};
}
;

Recording.prototype = new ScheduledRecording();

//Recording.prototype.constructor = new Recording();
//7.10.5.1 Constants
Recording.TState = {
    ERECORDING_UNREALIZED: 0,
    ERECORDING_SCHEDULED: 1,
    ERECORDING_DEL_SCHEDULED: 2,
    ERECORDING_REC_PRESTART: 3,
    ERECORDING_REC_ACQUIRING_RESOURCES: 4,
    ERECORDING_REC_STARTED: 5,
    ERECORDING_REC_UPDATED: 6,
    ERECORDING_REC_COMPLETED: 7,
    ERECORDING_REC_PARTIALLY_COMPLETED: 8,
    ERECORDING_TS_ACQUIRING_RESOURCES: 9,
    ERECORDING_TS_STARTED: 10,
    ERECORDING_ERROR: 11,
    ERECORDING_REC_START_ERROR: 12
};
Recording.TError = {
    EERROR_REC_RESOURCE_LIMITATION: 0,
    EERROR_INSUFFICIENT_STORAGE: 1,
    EERROR_TUNER_CONFLICT: 2,
    EERROR_REC_DRM_RESTRICTION: 3,
    EERROR_REC_UNKNOWN: 4,
    EERROR_TS_RESOURCE_LIMITATION: 5,
    EERROR_TS_DRM_RESTRICTION: 6,
    EERROR_HAVE_ALTERNATIVE: 7,
    EERROR_TS_UNKNOWN: 8
};
Recording.TShowType = {
    ERECORDING_LIVE: 0,
    ERECORDING_FIRSTRUN: 1,
    ERECORDING_RERUN: 2
};

// 7.10.6 The RecordingCollection class
function RecordingCollection() {
}

RecordingCollection.prototype = new Collection();

// 7.10.8 Bookmark class
function Bookmark() {
}

// 7.10.9 BookmarkCollection class
function BookmarkCollection() {
    var that = this;
    var b = new Bookmark();
    b.time = this.length;
    b.name = '_' + this.length;
    that.push(b);
    return b;
}

BookmarkCollection.prototype = new Collection();

// 7.12 Metadata APIs
// 7.12.1 The application/oipfSearchManager embedded object
function SearchManager() {
    var that = this;
    this.addEventListener("MetadataSearch", this.MetadataSearch, false);

    gOipfEmul.searchManagerObject = that;

    // 7.12.1.1 Properties
    that.guideDaysAvailable = 1;
}

SearchManager.prototype = new DOM2EventSource();

$.extend(true, SearchManager.prototype, {
    // 7.12.1.2 Events
    MetadataSearch: function (search, state) {
        if (this.onMetadataSearch) {
            this.onMetadataSearch(search, state);
        }
    },
    MetadataUpdate: function (action, info, object) {
        if (this.onMetadataUpdate) {
            this.onMetadataUpdate(action, info, object);
        }
    },
    // 7.12.1.3 Methods
    createSearch: function (searchTarget) {
        var search = new MetadataSearch(this, searchTarget);

        //search.id = (new Date()).getMilliseconds().toString();
        search.id = Math.random().toString();
        return search;
    },
    getChannelConfig: function () {
        return new ChannelConfig();
    }
});

// 7.12.2 The MetadataSearch class
function MetadataSearch(searchManager, searchTarget) {
    var that = this;
    that.result = new SearchResults(that);
    that._searchManager = searchManager;
    that._channelConstraintList = [];
    that.query = null;
    that.searchTarget = searchTarget;
}
;

MetadataSearch.prototype = {
    // 7.12.2.1 Properties
    query: {},
    result: {},
    parent: {},
    // 7.12.2.2 Methods
    setQuery: function (query) {
        var that = this;
        that.query = query;
        /*
        if (a.searchTarget == 1) {// scheduled content
        var programmeArray = a.result._item.filter(function (programme) {
        var result = false;
        eval("result = ((" + query.data + ") ? true : false)");
        return result;
        });
        var programmes = new ProgrammeCollection();
        for (var i = 0; i < programmeArray.length; i++) {
        //this is test source query not work well
        programmes.push(programmeArray[i]);
        }
        a.result._item = programmes;
        }
        else if (a.searchTarget == 2) { // on demand content
        }
        */
    },
    addRatingConstraint: function (scheme, threshold) {
    },
    addCurrentRatingConstraint: function () {
    },
    addChannelConstraint: function () {
        // void addChannelConstraint (ChannelList channels)
        // void addChannelConstraint (Channel channel)
        var that = this;

        if (arguments.length == 1) {
            var arg = arguments[0];

            if (arg instanceof Channel) {
                that._channelConstraintList.push(arg);
            } else if (arg instanceof Array) {
                var channels = [];
                $.each(arg, function () {
                    channels.push(this);
                });
                that._channelConstraintList = channels;
            }
        }
    },
    orderBy: function (field, ascending) {
        var that = this;
        that.result._item.sort(function (a, b) {
            var a1 = a[field], b1 = b[field];
            if (a1 == b1)
                return 0;
            return a1 > b1 ? 1 : -1;
        });
        if (!ascending) {
            that.result._item.reverse();
        }
    },
    createQuery: function (field, comparison, value) {
        var query = new Query();
        if (comparison == undefined || comparison == null) {
            query.data = field;
        } else {
            var rightOperand = value.toString();
            var leftOperand = "";

            switch (field) {
                case 'endTime':
                    leftOperand = "(programme['startTime'] + programme['duration'])";
                    break;

                default:
                    leftOperand = "programme['" + field + "']";
            }

            if (typeof value !== "number") {
                leftOperand = "String(" + leftOperand + ").toLowerCase()";
                rightOperand = "'" + rightOperand.toLowerCase() + "'";
            }

            switch (comparison) {
                case 0:
                    query.data = leftOperand + " == " + rightOperand;
                    break;
                case 1:
                    query.data = leftOperand + " != " + rightOperand;
                    break;
                case 2:
                    query.data = leftOperand + " > " + rightOperand;
                    break;
                case 3:
                    query.data = leftOperand + " >= " + rightOperand;
                    break;
                case 4:
                    query.data = leftOperand + " < " + rightOperand;
                    break;
                case 5:
                    query.data = leftOperand + " <= " + rightOperand;
                    break;
                case 6:
                    query.data = leftOperand + ".indexOf(" + rightOperand + ") >= 0";
                    break;
                case 7:
                    query.data = "typeof " + leftOperand + " = 'undefined'";
                    break;
            }
        }
        return (this.query = query);
    },
    findProgrammesFromStream: function (channel, startTime, count) {
        var that = this;
        var q = that.createQuery("startTime", 3, parseInt(startTime).toString());
        that.setQuery(q);
        that.addChannelConstraint(channel);
    }
};

// 7.12.3 The Query class
function Query() {
    this.data = "";
}
;

// 7.12.3.1 Methods
Query.prototype = {
    and: function (query) {
        this.data = "(" + this.data + " && " + query.data + ")";
        return this;
    },
    or: function (query) {
        this.data = "(" + this.data + " || " + query.data + ")";
        return this;
    },
    not: function () {
        this.data = "!(" + this.data + ")";
        return this;
    }
};

// 7.12.4 The SearchResults class
function SearchResults(metadataSearch) {
    var that = this;
    that._item = [];
    that._metadataSearch = metadataSearch;
    that.length = 0;

    // 7.12.4.1 Properties
    //that.length = 0;
    that.offset = 0;
    that.totalSize = 0;
    that.parent = {};
}

// 7.12.4.2 Methods
SearchResults.prototype = {
    item: function (index) {
        return this._item[index];
    },
    getResults: function (offset, count) {
        var that = this;
        var ms = that._metadataSearch;
        var searchTarget = ms.searchTarget;
        var channels;
        var idx, idx2;
        var cnt;

        if (ms._channelConstraintList.length === 0) {
            channels = ms._searchManager.getChannelConfig().channelList;
        } else {
            channels = ms._channelConstraintList;
        }

        var channelLength = channels.length;

        if (searchTarget === 1) {
            var isOnAir = (ms._constraintField === 'target' && ms._constraintValue === 'P/F');
            var quotas = Math.ceil(count / channelLength);
            var pushCount = quotas;
            var query = ms.query;
            //if (query && query.data.length > 0 && (eval(query.data)) === false) {
            //    return false;
            //}
            for (idx = 0; idx < channelLength; idx += 1) {
                var channel = channels[idx];
                var pc = gOipfEmul.getProgramCollection(channel);
                var pcLength = pc.length;
                var onAirCount = 0;
                cnt = 0;

                for (idx2 = 0; idx2 < pcLength; idx2 += 1) {
                    if (pc[idx2] === undefined) {
                        break;
                    }

                    var programme = pc[idx2];
                    that._item.push(programme);
                    cnt += 1;
                    if (isOnAir) {
                        onAirCount += 1;
                        if (onAirCount > 5) {
                            pcLength = idx2;
                        }
                    }

                    if (cnt >= 10) {
                        break;
                    }
                }
            }

            that.length = that._item.length;

            if (isOnAir) {
                ms._constraintField = '';
                ms._constraintValue = '';
            }

            if (offset !== -1) {
                that._item = that._item.slice(offset, count + offset);
                that.offset = offset;
            }
        } else {
            for (idx = 0; idx < Math.min(channelLength, 10); idx += 1) {
                that._item.push(channels[idx]);
            }
        }

        that.totalSize = that.length = that._item.length;

        setTimeout(function () {
            that._metadataSearch._searchManager.trigger("MetadataSearch", that._metadataSearch, 0);
        }, 3);

        return false;
    },
    abort: function () {
        this._item = new ProgrammeCollection();
    },
    update: function () {
        var a = this;
    }
};

// OIPF 7.13 Scheduled content and hybrid tuner APIs
// OIPF 7.13.1 The video / broadcast embedded object
function VideoBroadcast() {
    var that = this;
    that.channelConfig = new ChannelConfig();
    that.playbackOffset = 0;
    that.maxOffset = 0;
    that.playSpeed = 0;

    // OIPF 7.13.1.2 Properties
    that.type = 'video/broadcast';
    that.width = 0;
    that.height = 0;
    that.fullScreen = true;
    that.data = '';
    that.playState = 2;
    if (gOipfEmul.channelList) {
        that.currentChannel = gOipfEmul.channelList[0];
    } else {
        that.currentChannel = new Channel();
    }
}

VideoBroadcast.TPlayState = {
    EUnrealized: 0,
    EConnecting: 1,
    EPresenting: 2,
    EStopped: 3
};

VideoBroadcast.TRecordingState = {
    ERECORDING_UNREALIZED: 0,
    ERECORDING_SCHEDULED: 1,
    ERECORDING_REC_PRESTART: 2,
    ERECORDING_REC_ACQUIRING_RESOURCES: 3,
    ERECORDING_REC_STARTED: 4,
    ERECORDING_REC_UPDATED: 5,
    ERECORDING_REC_COMPLETED: 6,
    ERECORDING_TS_ACQUIRING_RESOURCES: 7,
    ERECORDING_TS_STARTED: 8,
    ERECORDING_ERROR: 9
};

VideoBroadcast.prototype = new DOM2EventSource();
$.extend(true, VideoBroadcast.prototype, {
    // OIPF 7.13.1.3 Methods
    getChannelConfig: function () {
        return this.channelConfig;
    },
    bindToCurrentChannel: function () {
        // this.setChannel(ChannelConfig.prototype.channelList.item(0), false, '');
        // this.trigger('ChannelChangeSucceeded', this.currentChannel, 0);
    },
    createChannelObject: function (idType, dsd, sid) {
        // Channel createChannelObject (idType, dsd, sid);
        // Channel createChannelObject (idType, onid, tsid, sid, sourceID, ipBroadcastID);
        var channelList = this.channelConfig.channelList;
        this.currentChannelIndex = 0;
        $.each(channelList, function () {
        });
        return channelList[this.currentChannelIndex];
    },
    setChannel: function (channel, trickplay, contentAccessDescriptorURL) {
        // void setChannel (channel, trickplay, contentAccessDescriptorURL);
        // void setChannel (channel, trickplay, contentAccessDescriptorURL, offset);
        var that = this;

        //var channel = arguments[0];
        /*
        var channel = arguments[0];
        var channelList = this.channelConfig.channelList;
        a.currentChannel = channelList[0];
        a.currentChannelIndex = 0;
        $.each(channelList, function (i, val) {
        if (channel.ccid == val.ccid) {
        a.currentChannel = val;
        a.currentChannelIndex = i;
        }
        });
        */
        // gOipfEmul.setChannel(channel);
        that.currentChannel = channel;
        console.dir('setChannel: ' + channel);
        that.ChannelChangeSucceeded(channel);
        that.programmes = gOipfEmul.getProgramCollection(that.currentChannel);

        // setTimeout(function() {
        // that.trigger("ChannelChangeSucceeded", that.currentChannel, 0);
        // }, 1000);
        var baseURL = 'images/test/temp/';
        var bgList = ["bg_Hub_01.jpg", "bg_LiveTV.jpg", "Ferrari_248_F1.jpg", "porsche-911.jpg"];
        var video = document.getElementById('dLive');
        var i = (channel.majorChannel % 4);
        if (video !== undefined && video !== null) {
            video.style.background = 'url(' + baseURL + bgList[i] + ')';
        }
    },
    prevChannel: function () {
        var that = this;

        /*
        var channelList = a.channelConfig.channelList;
        a.programmes = new ProgrammeCollection();
        a.currentChannelIndex =
        a.currentChannelIndex == channelList.length - 1
        ? 0
        : a.currentChannelIndex + 1;
        a.currentChannel = channelList[a.currentChannelIndex];
        $.each(VideoBroadcast.prototype.programmes, function (i, programme) {
        if (a.currentChannel.ccid == programme.channelID)
        a.programmes.push(programme);
        });
        */
        //  gOipfEmul.prevChannel();
        setTimeout(function () {
            that.trigger("ChannelChangeSucceeded", that.currentChannel, 0);
        }, 1000);
    },
    nextChannel: function () {
        var a = this;

        /*
        var channelList = a.channelConfig.channelList;
        a.programmes = new ProgrammeCollection();
        a.currentChannelIndex =
        a.currentChannelIndex == 0
        ? channelList.length - 1
        : a.currentChannelIndex - 1;
        a.currentChannel = channelList[a.currentChannelIndex];
        $.each(VideoBroadcast.prototype.programmes, function (i, programme) {
        if (a.currentChannel.ccid == programme.channelID)
        a.programmes.push(programme);
        });
        */
        //  gOipfEmul.nextChannel();
        setTimeout(function () {
            a.trigger("ChannelChangeSucceeded", a.currentChannel, 0);
        }, 1000);
    },
    stop: function () {
        var video = document.getElementById('dLive');
        if (video !== undefined && video !== null) {
            video.style.background = '';
        }
    },
    setFullScreen: function (fullscreen) {
    },
    setVolume: function (volume) {
    },
    getVolume: function () {
        return 0;
    },
    release: function () {
    },
    // OIPF 7.13.1.4 Events
    focus: function () {
        var fn = this.onfocus;
        if (fn) {
            fn();
        }
    },
    blur: function () {
        var fn = this.onblur;
        if (fn) {
            fn();
        }
    },
    FullScreenChange: function () {
        var fn = this.onFullScreenChange;
        if (fn) {
            fn();
        }
    },
    ChannelChangeError: function (channel, errorType) {
        var fn = this.onChannelChangeError;
        if (fn) {
            fn(channel, errorType);
        }
    },
    ChannelChangeSucceeded: function (channel) {
        var that = this, fn = that.onChannelChangeSucceeded;

        that.trigger2('ChannelChangeSucceeded', [{
                name: 'channel',
                value: channel
            }]);

        if (fn) {
            fn(channel);
            that.ProgrammesChanged();
            that.setTsrOffsetTimer(true);
        }
    },
    PlayStateChange: function () {
        var fn = this.onPlayStateChange;
        if (fn) {
            fn();
        }
    }
});

// OIPF 7.13.2 Extensions to video / broadcast for recording and time-shift
// OIPF 7.13.2.1 Additional constants for video / broadcast object
// OIPF 7.13.2.2 Additional properties for video / broadcast object
$.extend(true, VideoBroadcast.prototype, {
    playbackOffset: 0,
    maxOffset: 0,
    recordingState: VideoBroadcast.TRecordingState.ERECORDING_REC_COMPLETED,
    playPosition: 1,
    playSpeed: 0,
    playSpeeds: [],
    timeShiftMode: 0,
    currentTimeShiftMode: 0,
    // OIPF 7.13.2.3 Additional methods for video / broadcast object
    setTsrOffsetTimer: function (bChange) {
        var that = this;
        if (bChange) {
            that.maxOffset = 0;
            that.playbackOffset = 0;
        }
        clearInterval(that._tsrMaxOffsetTimer);
        that._tsrMaxOffsetTimer = null;
        clearInterval(that._tsrPlayOffsetTimer);
        that._tsrPlayOffsetTimer = null;
        that._tsrMaxOffsetTimer = setInterval(function () {
            that.maxOffset += 1;
        }, 1000);
        that._tsrPlayOffsetTimer = setInterval(function () {
            that.playbackOffset += 1;
        }, 1000);
    },
    _setStartMaxOffsetTimer: function () {
        var that = this;
        clearInterval(that._tsrMaxOffsetTimer);
        that._tsrMaxOffsetTimer = null;
        that._tsrMaxOffsetTimer = setInterval(function () {
            that.maxOffset += 1;
            that.PlayPositionChanged(that.playbackOffset);
        }, 1000);
    },
    _setStartPlayOffsetTimer: function () {
        var that = this;
        clearInterval(that._tsrPlayOffsetTimer);
        that._tsrPlayOffsetTimer = null;
        that._tsrPlayOffsetTimer = setInterval(function () {
            that.playbackOffset += 1;
            that.PlayPositionChanged(that.playbackOffset);
        }, 1000);
    },
    recordNow: function (duration) {
        return '';
    },
    stopRecording: function () {
    },
    pause: function () {
        var that = this;
        var err = false;
        that.playSpeed = err ? 1 : 0;
        that.playbackOffset = that.maxOffset - 3;
        that.PlaySpeedChanged(that.playSpeed);
        that.PlayPositionChanged(0);
        clearInterval(that._tsrPlayOffsetTimer);
        that._tsrPlayOffsetTimer = null;
        return true;
    },
    resume: function () {
        var that = this;
        that._setStartPlayOffsetTimer();
        return true;
    },
    setSpeed: function (speed) {
        var that = this;
        that.playSpeed = speed;
        clearInterval(that._tsrPlayOffsetTimer);
        if ((that.playbackOffset === that.maxOffset) && speed === 0) {
            that.pause();
        } else if (speed === 0) {
            clearInterval(that._tsrPlayOffsetTimer);
            that._tsrPlayOffsetTimer = null;
        } else if (speed == 1) {
            that.resume();
        } else {
            clearInterval(that._tsrPlayOffsetTimer);
            that._tsrPlayOffsetTimer = null;
            that._tsrPlayOffsetTimer = setInterval(function () {
                that.playbackOffset += that.playSpeed;
                if (that.playbackOffset < 0) {
                    that.playbackOffset = 0;
                    that.playSpeed = 0;
                    clearInterval(that._tsrPlayOffsetTimer);
                    that._tsrPlayOffsetTimer = null;
                }
                that.PlayPositionChanged(that.playbackOffset);
            }, 1000);
        }
        return true;
    },
    seek: function (offset, reference) {
        var that = this;
        if (reference === 0) {
            that.playbackOffset = offset;
        } else if (reference == 1) {
            that.playbackOffset += offset;
        }
        return true;
    },
    stopTimeshift: function () {
        var that = this;
        clearInterval(that._tsrPlayOffsetTimer);
        that._tsrPlayOffsetTimer = null;
        clearInterval(that._tsrMaxOffsetTimer);
        that._tsrMaxOffsetTimer = null;
        that.playSpeed = 1;
        that.playbackOffset = that.maxOffset;
        that.PlaySpeedChanged(1);
        that.setTsrOffsetTimer(false);
        return true;
    },
    /*setChannel : function(channel, trickplay, contentAccessDescriptorURL, offset) {
    },*/
    // OIPF 7.13.2.4 Events
    RecordingEvent: function (state, error, recordingId) {
        var fn = this.onRecordingEvent;
        if (fn) {
            fn(state, error, recordingId);
        }
    },
    PlaySpeedChanged: function (position) {
        var fn = this.onPlaySpeedChanged;
        if (fn) {
            fn(position);
        }
    },
    PlayPositionChanged: function (position) {
        var fn = this.onPlayPositionChanged;
        if (fn) {
            fn(position);
        }
    },
    PlaySpeedsArrayChanged: function () {
        var fn = this.onPlaySpeedsArrayChanged;
        if (fn) {
            fn();
        }
    }
});

// OIPF 7.13.3 Extensions to video / broadcast for access to EIT p/f
$.extend(true, VideoBroadcast.prototype, {
    programmes: null,
    // OIPF 7.13.3.1 Events
    ProgrammesChanged: function () {
        var fn = this.onProgrammesChanged;
        if (fn) {
            fn();
        }
    }
});

// 7.13.4 Extensions to video/broadcast for playback of selected components
// 7.13.5 Extensions to video/broadcast for parental ratings errors
$.extend(true, VideoBroadcast.prototype, {
    // 7.13.5.1 Events
    ParentalRatingChange: function (contentID, rating, DRMSystemID, blocked) {
        var fn = this.onParentalRatingChange;
        if (fn) {
            fn(contentID, rating, DRMSystemID, blocked);
        }
    },
    ParentalRatingError: function (contentID, ratings, DRMSystemID) {
        var fn = this.onParentalRatingError;
        if (fn) {
            fn(contentID, ratings, DRMSystemID);
        }
    }
});

// 7.13.6 Extensions to video/broatcast for DRM rights errors
$.extend(true, VideoBroadcast.prototype, {
    DRMRightsError: function (errorState, contentID, DRMSystemID, rightsIssuerURL) {
        var fn = this.onDRMRightsError;
        if (fn) {
            fn(errorState, contentID, DRMSystemID, rightsIssuerURL);
        }
    }
});

// 7.13.7 Extensions to video/broadcast for channel scan
// 7.13.8 Extensions to video/broadcast for current channel information
// 7.13.8.1 Properties
$.extend(true, VideoBroadcast.prototype, {
    currentChannel: null
});

// 7.13.9 Extenstions to video/broadcast for creating channel lists form SD&S fragments
// 7.13.10 The ChannelConfig class
function ChannelConfig() {
    var that = this;

    // 7.13.10.1 Properties
    that.channelList = gOipfEmul.channelList;

    //    that.favouriteLists = {};
    that.currentFavouriteList = {};

    // readonly FavouriteList
    that.currentChannel = {};
}

ChannelConfig.prototype = new DOM2EventSource();
$.extend(true, ChannelConfig.prototype, {
    // 7.13.10.2 Methods
    createFilteredList: function (blocked, favourite, hidden, favouriteListID) {
        return this.channelList;
    },
    onChannelScan: function (a, b, c, d, e, f, g, h, i) {
    },
    onChannelListUpdate: function () {
    },
    controlScan: function (operation) {
        var that = this;
        var handler = {
            load: function () {
                console.log("[EMULATOR] " + '[ChannelConfig : controlScan] load');
                that.onChannelListUpdate();
            },
            save: function () {
                console.log("[EMULATOR] " + '[ChannelConfig : controlScan] save');
            },
            pause: function () {
                console.log("[EMULATOR] " + '[ChannelConfig : controlScan] pause');
            },
            resume: function () {
                console.log("[EMULATOR] " + '[ChannelConfig : controlScan] resume');
            },
            loadDefChannels: function () {
                console.log("[EMULATOR] " + '[ChannelConfig : controlScan] loadDefChannels');
            }
        }[operation]();
    },
    removeChannel: function () {
        console.log("[EMULATOR] " + '[ChannelConfig : controlScan] loadDefChannels');
    },
    removeChannelList: function() {
        console.log("[EMULATOR] " + '[ChannelConfig : removeChannelList');
    },
    //Emul Timer
    timers: [],
    startScan: function (options, scanParameters) {
        var that = this;
        var CSearchedInfo = function () {
        };
        CSearchedInfo.prototype = [];
        CSearchedInfo.prototype.getInfo = function (index, type) {
            return this[index][type];
        };
        that.timers.push(setTimeout(function () {
            var searchedInfo = new CSearchedInfo();
            var tpInfo = new DVBSChannelScanParameters();
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel1'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel1 long long long long logn'
            });
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel2'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel2'
            });

            tpInfo.antennaId = 1;
            tpInfo.startFrequency = 10744;
            tpInfo.endFrequency = 10744;
            tpInfo.modulationModes = 2;
            tpInfo.symbolRate = '27500';
            tpInfo.polarisation = 2;
            tpInfo.codeRate = 'AUTO';
            that.onChannelScan(0, 20, 0, 80, 0, 0, 4, 1, searchedInfo, tpInfo);
        }, 400));
        that.timers.push(setTimeout(function () {
            var searchedInfo = new CSearchedInfo();
            var tpInfo = new DVBSChannelScanParameters();
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel3'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel3'
            });
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel4'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel4'
            });

            tpInfo.antennaId = 1;
            tpInfo.startFrequency = 10832;
            tpInfo.endFrequency = 10832;
            tpInfo.modulationModes = 2;
            tpInfo.symbolRate = '22000';
            tpInfo.polarisation = 1;
            tpInfo.codeRate = 'AUTO';
            that.onChannelScan(0, 40, 0, 0, 0, 0, 4, 2, searchedInfo, tpInfo);
        }, 400));
        that.timers.push(setTimeout(function () {
            var searchedInfo = new CSearchedInfo();
            var tpInfo = new DVBSChannelScanParameters();
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel5'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel5'
            });
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel6'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel6'
            });

            tpInfo.antennaId = 1;
            tpInfo.startFrequency = 12266;
            tpInfo.endFrequency = 12266;
            tpInfo.modulationModes = 1;
            tpInfo.symbolRate = '27500';
            tpInfo.polarisation = 2;
            tpInfo.codeRate = 'AUTO';
            that.onChannelScan(0, 60, 0, 0, 0, 0, 4, 3, searchedInfo, tpInfo);
        }, 600));
        that.timers.push(setTimeout(function () {
            var searchedInfo = new CSearchedInfo();
            var tpInfo = new DVBSChannelScanParameters();
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel7'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel7'
            });
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel8'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel8'
            });

            tpInfo.antennaId = 1;
            tpInfo.startFrequency = 11000;
            tpInfo.endFrequency = 11000;
            tpInfo.modulationModes = 1;
            tpInfo.symbolRate = '27500';
            tpInfo.polarisation = 2;
            tpInfo.codeRate = 'AUTO';
            that.onChannelScan(0, 80, 0, 0, 0, 0, 4, 4, searchedInfo, tpInfo);
        }, 800));
        that.timers.push(setTimeout(function () {
            var searchedInfo = new CSearchedInfo();
            var tpInfo = new DVBSChannelScanParameters();
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel9'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel9'
            });
            searchedInfo.push({
                channelType: 1,
                casType: 0,
                lcn: 0,
                name: 'Dummy TV Channel10'
            });
            searchedInfo.push({
                channelType: 2,
                casType: 0,
                lcn: 0,
                name: 'Dummy Radio Channel10'
            });

            tpInfo.antennaId = 1;
            tpInfo.startFrequency = 13000;
            tpInfo.endFrequency = 13000;
            tpInfo.modulationModes = 1;
            tpInfo.symbolRate = '27500';
            tpInfo.polarisation = 2;
            tpInfo.codeRate = 'AUTO';
            that.onChannelScan(0, 100, 0, 0, 0, 0, 4, 5, searchedInfo, tpInfo);
        }, 1000));
    },
    stopScan: function () {
        console.log("[EMULATOR] " + 'ChannelConfig.stopScan ');
        var idx, that = this;
        for (idx = 0; idx < that.timers.length; idx += 1) {
            clearInterval(that.timers[idx]);
        }
    },
    createChannelList: function (bdr) {
        return this.channelList;
    },
    createChannelObject: function (idType, onid, tsid, sid, sourceID, ipBroadcastID) {
    },
    createChannelScanParametersObject: function (idType) {
        return new ChannelScanParameters();
    },
    // OIPF 7.13.10.3
    ChannelScan: function (type, progress, frequency, signalStrength, channelNumber, channelType, channelCount, transponderCount) {
        var fn = this.onChannelScan;
        if (fn) {
            fn(type, progress, frequency, signalStrength, channelNumber, channelType, channelCount, transponderCount);
        }
    },
    ChannelListUpdate: function () {
        var fn = this.onChannelListUpdate;
        if (fn) {
            fn();
        }
    },
    createChannelScanOptionsObject: function () {
        return new ChannelScanOption();
    }
});

// for test oipf.metadata.js  --copy from freesat start--- if don't need this. remove this. it's ok
ChannelConfig.prototype.startDiseqcConnection = function () {
    DBG("ChannelConfig.startDiseqcConnection");
    setTimeout("gEngine.videoBroadcast.getChannelConfig().onChannelScan(4,0,0,0,0,0,0,0);", 1000);
    setTimeout("gEngine.videoBroadcast.getChannelConfig().onChannelScan(4,100,0,0,0,0,1,0);", 2000);
};
ChannelConfig.prototype.commitDiseqcConnection = function () {
    DBG("ChannelConfig.commitDiseqcConnection");
};
ChannelConfig.prototype.startAntennaConnection = function () {
    DBG("ChannelConfig.startAntennaConnection");
    setTimeout("gEngine.videoBroadcast.getChannelConfig().onChannelScan(4,0,0,0,0,0,0,0);", 1000);
    setTimeout("gEngine.videoBroadcast.getChannelConfig().onChannelScan(4,100,0,0,0,0,0,0);", 2000);
};
ChannelConfig.prototype.stopAntennaConnection = function () {
    DBG("ChannelConfig.stopAntennaConnection");
};

// for test oipf.metadata.js  --copy from freesat end--- if don't need this. remove this. it's ok
// OIPF 7.13.11 The ChannelList class
function ChannelList(channelCollection) {
    $.extend(true, ChannelList.prototype, channelCollection);
}

ChannelList.prototype = new Collection();

$.extend(true, ChannelList.prototype, {
    // OIPF 7.13.11.1 Methods
    getChannel: function (channelID) {
        var idx, that = this;
        for (idx = 0; idx < that.length; idx += 1) {
            var channel = that.item(idx);
            if (channel.ccid == channelID) {
                return channel;
            }
        }
        return that.item(0);
        // return null;
    },
    getChannelByTriplet: function (onid, tsid, sid) {
        var idx, that = this;
        for (idx = 0; idx < that.length; idx += 1) {
            var channel = that.item(idx);
            if (channel.onid == onid && channel.tsid == tsid && channel.sid == sid) {
                return channel;
            }
        }
        return null;
    },
    getChannelBySourceID: function (sourceID) {
        var idx, that = this;
        for (idx = 0; idx < that.length; idx += 1) {
            var channel = that.item(idx);
            if (channel.sourceID == sourceID) {
                return channel;
            }
        }
        return null;
    }
});

// OIPF 7.13.12 The Channel class
// OIPF 7.13.12.1 Constants
Channel.TChannelType = {
    ETYPE_TV: 0,
    ETYPE_RADIO: 1,
    ETYPE_OTHER: 2
};
Channel.TIdType = {
    EID_ANALOG: 0,
    EID_DVB_C: 10,
    EID_DVB_S: 11,
    EID_DVB_T: 12,
    EID_DVB_SI_DIRECT: 13,
    EID_DVB_C2: 14,
    EID_DVB_S2: 15,
    EID_DVB_T2: 16,
    EID_ISDB_C: 20,
    EID_ISDB_S: 21,
    EID_ISDB_T: 22,
    EID_ATSC_T: 30,
    EID_IPTV_SDS: 40,
    EID_IPTV_URI: 41
};

function Channel() {
    var that = this;

    // OIPF 7.13.12.2 Properties
    that.channelType = 0;
    that.idType = 0;
    that.ccid = '';
    that.tunerID = '';
    that.onid = 0;
    that.tsid = 0;
    that.sid = 0;
    that.sourceID = 0;
    that.freq = 0;
    that.cni = 0;
    that.name = '';
    that.majorChannel = 0;
    that.minorChannel = 0;
    that.dsd = '';
    that.favourite = false;
    that.favIDs = {};
    that.logoURL = null;

    // readonly StringCollection
    that.locked = false;
    that.manualBlock = false;
    that.ipBroadcastID = '';
    that.channelMaxBitRate = 0;
    that.channelTTR = 0;

    that.TYPE_TV = Channel.TChannelType.ETYPE_TV;
    that.TYPE_RADIO = Channel.TChannelType.ETYPE_RADIO;
    that.TYPE_OTHER = Channel.TChannelType.ETYPE_OTHER;
    that.ID_ANALOG = Channel.TIdType.EID_ANALOG;
    that.ID_DVB_C = Channel.TIdType.EID_DVB_C;
    that.ID_DVB_S = Channel.TIdType.EID_DVB_S;
    that.ID_DVB_T = Channel.TIdType.EID_DVB_T;
    that.ID_DVB_T = Channel.TIdType.EID_DVB_SI_DIRECT;
    that.ID_DVB_T = Channel.TIdType.EID_DVB_C2;
    that.ID_DVB_T = Channel.TIdType.EID_DVB_S2;
    that.ID_DVB_T = Channel.TIdType.EID_DVB_T2;
    that.ID_DVB_T = Channel.TIdType.EID_ISDB_C;
    that.ID_DVB_T = Channel.TIdType.EID_ISDB_S;
    that.ID_DVB_T = Channel.TIdType.EID_ISDB_T;
    that.ID_DVB_T = Channel.TIdType.EID_ATSC_T;
    that.ID_DVB_T = Channel.TIdType.EID_IPTV_SDS;
    that.ID_DVB_T = Channel.TIdType.EID_IPTV_URI;
}

// OIPF 7.13.12.3 Metadata extensions to Channel
$.extend(true, Channel.prototype, {
    // OIPF 7.13.12.3.1 Properties
    longName: '',
    description: '',
    authorised: true,
    genre: {},
    logoURL: './images/ch_Logo1.png',
    hidden: false,
    // OIPF 7.13.12.3.2 Methods
    getField: function (fieldId) {
        var that = this;
        var fieldValue = {
            'selectable': 'true',
            'operatorId': 8,
            'fsatsvcId': that.fsat_svcid
        };
        if (typeof fieldValue[fieldId] == 'undefined') {
            return '';
        }
        return fieldValue[fieldId];
    },
    getLogo: function (width, height) {
        var n = Math.floor(Math.random() * 2);
        if (n == 0) {
            return './images/ch_logo.png';
        } else {
            return './images/ch_Logo1.png';
        }
    }
});

// OIPF 7.13.13 The FavouriteListCollection class
function FavouriteListCollection() {
}

FavouriteListCollection.prototype = new Collection();

// OIPF 7.13.13.1 Method
$.extend(true, FavouriteListCollection.prototype, {
    getFavouriteList: function (favID) {
        var idx, that = this;
        for (idx = 0; idx < that.length; idx += 1) {
            var favoriteList = that[idx];
            if (favoriteList.favID === favID) {
                return favoriteList;
            }
        }
        return null;
    }
});

// OIPF 7.13.13.2 Extensions to FavouriteListCollection
$.extend(true, FavouriteListCollection.prototype, {
    createFavouriteList: function (name) {
        var a = this;
        var newFav = null;
        if (a.length) {
            var favID = (a[a.length - 1].favID) + 1;
            newFav = new FavouriteList(favID, name);
        } else {
            newFav = new FavouriteList('fav://D', name);
        }

        if (newFav) {
            a.push(newFav);
        }
        return newFav;
    },
    remove: function (index) {
        var that = this;
        if (that[index] !== null) {
            that.pop(index);
            return true;
        }
        return false;
    },
    commit: function () {
        return true;
    },
    activateFavouriteList: function (favID) {
        var idx, that = this;
        for (idx = 0; idx < that.length; idx += 1) {
            var favoriteList = that[idx];
            if (favoriteList.favID === favID) {
                ChannelConfig.prototype.currentFavouriteList = favoriteList;
                return true;
            }
        }
        return false;
    }
});

// OIPF 7.13.14 The FavouriteList class
function FavouriteList(favID, name) {
    // OIPF 7.13.14.1 Properties
    this.favID = favID;
    this.name = name;
}

FavouriteList.prototype = new Collection();

$.extend(true, FavouriteList.prototype, {
    // OIPF 7.13.14.2 Methods
    getChannel: function (channelID) {
        return new Channel();
    },
    getChannelByTriplet: function (onid, tsid, sid) {
        return new Channel();
    },
    getChannelBySourceID: function (sourceID) {
        return new Channel();
    }
});

// OIPF 7.13.14.3 Extensions to FavouriteList
$.extend(true, FavouriteList.prototype, {
    insertBefore: function (index, ccid) {
        var a = this;
        var n;
        var chlist = gOipfEmul.channelList;
        var len = chlist.length;

        for (n = 0; n < len; n += 1) {
            if (chlist[n].ccid === ccid) {
                var channel = chlist[n];
                a.push(channel);
                break;
            }
        }
        return true;
    },
    remove: function (index) {
        var a = this;
        var n;
        var chlist = gOipfEmul.channelList;
        var len = chlist.length;

        a.splice(index, 1);

        return true;
    },
    commit: function () {
        return true;
    }
});

// OIPF 7.13.15 The ChannelScanEvent class
// OIPF 7.13.16 the ChannelScanOptions class
function ChannelScanOptions() {
    var that = this;
    that.channelType = Channel.TChannelType.ETYPE_TV;
    that.replaceExisting = true;
}

// OIPF 7.13.17 The ChannelScanParameters class
function ChannelScanParameters() {
}

ChannelScanParameters.prototype.addChannelScanParam = function (params) {
};
var ChannelScanOption = function () {
};

// OIPF 7.13.18 The DVBTChannelScanParameters class
function DVBTChannelScanParameters() {
    var that = this;

    // OIPF 7.13.18.1 Properties
    that.startFrequency = 7000;
    that.endFrequency = 7000;
    that.raster = 7000;
    that.ofdm = 'MODE_1k';

    // Valid values: "MODE_1k", "MODE_2k", "MODE_3k", "MODE_4k", "MODE_8k", "MODE_16k", "MODE_32k"
    that.modulationModes = 1;

    // Valid values: 1(QPSK), 4(QAM16), 8(QAM32), 16(QAM64), 32(QAM128), 64(QAM256)
    that.bandwidth = 'BAND_1.7MHz';
    // Valid values: "BAND_1.7MHz", "BAND_5MHz", "BAND_6MHz", "BAND_7MHz", "BAND_8MHz", "BAND_10MHz"
}

// OIPF 7.13.19 The DVBSChannelScanParameters class
function DVBSChannelScanParameters() {
    var that = this;

    // OIPF 7.13.19.1 Properties
    that.antennaId = 1;
    that.startFrequency = 10729;
    that.endFrequency = 10729;
    that.modulationModes = 1;

    // Valid values: 1(QPSK), 2(8PSK), 4(QAM16)
    that.symbolRate = '22000';
    that.polarisation = 1;

    // Valid values: 1, 2, 4, 8
    that.codeRate = '3/4';
    // Valid values: '3/4', '5/6'
}

// OIPF 7.13.20 The DVBCChannelScanParameters class
function DVBCChannelScanParameters() {
    var that = this;

    // OIPF 7.13.20.1 Properties
    that.startFrequency = 7000;
    that.endFrequency = 7000;
    that.raster = 7000;
    that.startNetworkScanOnNIT = true;
    that.modulationModes = 4;
    that.symbolRate = '';
    that.networkId = '';
}

// OIPF 7.13.21 Extensions to video/broadcast for synchronization
$.extend(true, DVBCChannelScanParameters.prototype, {
    addStreamEventListener: function (targetURL, eventName, listener) {
    },
    removeStreamEventListener: function (eventURL, eventName, listener) {
    }
});

// OIPF 7.13.21.1 The StreamEvent class
function StreamEvent() {
    var that = this;
    that.eventName = '';
    that.data = '';
    that.text = '';
    that.status = '';
}

// for test oipf.metadata.js  --copy from freesat start--- if don't need this. remove this. it's ok
//7.16 Shared Utility classes and features
// 7.14 Media playback APIs
var TVodPlayState = {
    EVOD_PLAY_STATE_STOPPED: 0,
    EVOD_PLAY_STATE_PLAYING: 1,
    EVOD_PLAY_STATE_PAUSED: 2,
    EVOD_PLAY_STATE_CONNECTING: 3,
    EVOD_PLAY_STATE_BUFFERING: 4,
    EVOD_PLAY_STATE_FINISHED: 5,
    EVOD_PLAY_STATE_ERROR: 6
};

//OIPF 7.14.1 The CEA 2014 A/V Control embedded object
function AVControl() {
    var that = this;
    that.playPosition = 0;
    that.speed = 0;
    that.playState = 0;

    // CEA-2014-A Properties
    that.data = '';
    that.playPosition = 1;
    that.playTime = 1000 * 60 * 60;
    that.playState = 0;
    that.error = 0;
    that.nrTracks = 1;
    that.currentTrackIndex = 1;
    that.persist = true;
    that.width = 1;
    that.height = 1;
    that.fullScreen = true;
}

AVControl.prototype = new DOM2EventSource();

$.extend(true, AVControl.prototype, {
    _changePlayState: function (playState) {
        var that = this;
        if (that.playState !== playState) {
            that.playState = playState;
            that.PlayStateChanged();
            return true;
        }
        return false;
    },
    _changeSpeed: function (speed) {
        var that = this;
        if (that.speed != speed) {
            that.speed = speed;
            clearInterval(that._posTimer);
            if (speed == 0) {
            } else {
                that._posTimer = setInterval(function () {
                    var pos = that.playPosition + (that.speed * 1000);
                    if (that.playTime < pos) {
                        pos = that.playTime;
                        that._changeSpeed(0);
                        that._changePlayState(TVodPlayState.EVOD_PLAY_STATE_FINISHED);
                    } else if (pos < 0) {
                        pos = 0;
                        that._changeSpeed(0);
                        that._changePlayState(TVodPlayState.EVOD_PLAY_STATE_PAUSED);
                    }
                    that._changePlayPosition(pos);
                }, 1000);
            }
            that.PlaySpeedChanged(speed);
            return true;
        }
        return false;
    },
    _changePlayPosition: function (playPosition) {
        var that = this;
        if (that.playPosition !== playPosition) {
            that.playPosition = playPosition;
            that.PlayPositionChanged(playPosition);
            return true;
        }
        return false;
    },
    // CEA-2014-A Methods
    play: function (speed) {
        var that = this;

        if (that._requirePIN) {
            that.error = 3;
            that._changePlayState(TVodPlayState.EVOD_PLAY_STATE_ERROR);
            return false;
        }

        that._playTimer = setTimeout(function () {
            if (that._changeSpeed(speed)) {
                if (speed === 0) {
                    that._changePlayState(TVodPlayState.EVOD_PLAY_STATE_PAUSED);
                } else {
                    that._changePlayState(TVodPlayState.EVOD_PLAY_STATE_PLAYING);
                }
            }
        }, 100);
        return true;
    },
    stop: function () {
        var that = this;
        that._changeSpeed(0);
        that._changePlayPosition(0);
        that._changePlayState(TVodPlayState.EVOD_PLAY_STATE_STOPPED);
        return true;
    },
    seek: function (pos) {
        var a = this;
        a._changePlayPosition(pos);
        return true;
    },
    setVolume: function (volume) {
        return true;
    },
    next: function () {
        return true;
    },
    previous: function () {
        return true;
    },
    getSinkProtocolInfo: function () {
        return "";
    },
    getTransportStateVariables: function (stateVariableList) {
        return "";
    },
    setTransportStateVariables: function (stateVariableValuePairs) {
        return "";
    },
    setFullScreen: function (fullscreen) {
    },
    // CEA-2014-A Events
    PlayStateChange: function () {
        if (this.onPlayStateChange) {
            this.onPlayStateChange();
        }
    },
    FullScreenChange: function () {
        if (this.onFullScreenChange) {
            this.onFullScreenChange();
        }
    },
    focus: function () {
        if (this.onfocus) {
            this.onfocus();
        }
    },
    blur: function () {
        if (this.onblur) {
            this.onblur();
        }
    }
});

// 7.14.3 Extensions to A/V Control object for trickmodes
$.extend(true, AVControl.prototype, {
    // 7.14.3.1 Properties
    playSpeeds: [],
    oitfSourceIPAddress: "",
    oitfSourcePortAddress: "",
    oitfNoRTSPSessionControl: true,
    oitfRTSPSessionId: "",
    // 7.14.3.2 Events
    PlaySpeedChanged: function (speed) {
        if (this.onPlaySpeedChanged) {
            this.onPlaySpeedChanged(speed);
        }
    },
    PlayPositionChanged: function (position) {
        if (this.onPlayPositionChanged) {
            this.onPlayPositionChanged(position);
        }
    },
    PlaySpeedsArrayChanged: function () {
        if (this.onPlaySpeedsArrayChanged) {
            this.onPlaySpeedsArrayChanged();
        }
    }
});

// 7.14.4 Extensions to A/V Control object for playback of selected components
// 7.14.5 Extensions to A/V Control object for parental rating errors
$.extend(true, AVControl.prototype, {
    // 7.14.5.1 Events
    ParentalRatingChange: function (contentID, rating, DRMSystemID, blocked) {
        if (this.onParentalRatingChange) {
            this.onParentalRatingChange(contentID, rating, DRMSystemID, blocked);
        }
    },
    ParentalRatingError: function (contentID, ratings, DRMSystemID) {
        if (this.onParentalRatingError) {
            this.onParentalRatingError(contentID, ratings, DRMSystemID);
        }
    }
});

// 7.14.6 Extensions to A/V Control object for DRM rights errors
$.extend(true, AVControl.prototype, {
    DRMRightsError: function (errorState, contentID, DRMSystemID, rightsIssuerURL) {
        if (this.onDRMRightsError) {
            this.onDRMRightsError(errorState, contentID, DRMSystemID, rightsIssuerURL);
        }
    }
});

// 7.14.7 Extensions to A/V Control object for playing media objects
$.extend(true, AVControl.prototype, {
    setSource: function (id) {
        var that = this;
        that.error = 0;

        //var recording = gOipfEmul.findRecording(id);
        //that._changeSpeed(0);
        that._changePlayPosition(0);
        that._changePlayState(TVodPlayState.EVOD_PLAY_STATE_STOPPED);

        that.playTime = 60 * 60 * 1000;
        that.data = id;
        return true;
    }
});

// 7.14.8 Extensions to A/V Control object for UI feedback of buffering A/V content
$.extend(true, AVControl.prototype, {
    // 7.14.8.1 Properties
    readyToPlay: true,
    availableRepresentationsBandwidth: [],
    currentRepresentation: 1,
    maxRepresentation: 1,
    minRepresentation: 0,
    supportedStrategies: {},
    // 7.14.8.2 Methods
    getAvailablePlayTime: function (fromPlayPosition) {
        return 1;
    },
    setBufferingStrategy: function (name) {
        return true;
    },
    setRepresentationStrategy: function (maxBandwidth, minBandwidth, position) {
        return true;
    },
    // 7.14.8.3 Events
    ReadyToPlay: function () {
        if (this.onReadyToPlay) {
            this.onReadyToPlay();
        }
    },
    RepresentationChange: function (bandwidth, position) {
        if (this.onRepresentationChange) {
            this.onRepresentationChange(bandwidth, position);
        }
    },
    PeriodChange: function (bandwidths, position) {
        if (this.onPeriodChange) {
            this.onPeriodChange(bandwidths, position);
        }
    }
});

// 7.14.9 DOM2 events for A/V Control object
$.extend(true, AVControl.prototype, {
    PlayStateChanged: function () {
        this.trigger2('PlayStateChange');
        /*
        if (this.onPlayStateChange) {
        this.onPlayStateChange();
        }
        */
    },
    FullScreenChange: function () {
        if (this.onFullScreenChange) {
            this.onFullScreenChange();
        }
    }
});

// 7.14.10 Playback of audio from memory
// 7.14.11 Extensions to A/V Control object for media queuing
$.extend(true, AVControl.prototype, {
    queue: function (uri) {
        return true;
    },
    getVolume: function () {
        return 1;
    }
});

//7.16.1 The StringCollection class
function StringCollection() {
}

StringCollection.prototype = new Collection();

//7.16.2 The Programme class
function Programme() {
    var that = this;

    // 7.16.2.2 Properties
    that.name = "Star Trek: DS9";
    that.longName = "Star Trek: Deep Space Nine";
    that.description = "an episode synopsis";
    that.longDescription = "an episode synopsis";

    // Date.getTime() : Returns the number of milliseconds since midnight Jan 1, 1970
    that.startTime = Math.ceil((new Date()).getTime() / 1000);
    that.duration = 1000;

    //Programme.prototype.endTime = Math.ceil(((new Date()) - (new Date(1970, 1, 1))) / 1000) + 1000;
    that.channelID = "1";
    that.episode = 1;
    that.totalEpisodes = 20;
    that.programmeID = "";
    that.programmeIDType = Programme.TProgrammeIDType.EID_TVA_CRID;
    that.IMI = "";
    that.groupCRIDs = {};
    that.parentalRating = new ParentalRatingCollection();

    // 7.16.2.3 Metadata extensions to Programme
    // 7.16.2.3.1 Properties
    that.channel = {};
    that.blocked = false;
    that.showType = 1;

    // 1: live, 2: first-run show, 3: rerun
    that.subtitles = true;
    that.isHD = true;
    that.isAD = true;

    // VerticalSize has a value greater than 576 - true  otherwise - false
    that.audioType = 4;

    // 1: mono, 2: stereo, 3: multi-channel
    that.isMultilingual = true;
    that.genre = {};
    that.hasRecording = false;
    that.audioLanguages = {};
    that.subtitleLanguages = {};
    that.locked = false;

    // 7.16.2.5 Recoding extensions to Programme
    that.recording = null;

    that.scheduledRecording = new ScheduledRecording();
}

//7.16.2.1 Constants
Programme.TProgrammeIDType = {
    EID_TVA_CRID: 0,
    EID_DVB_EVENT: 1,
    EID_TVA_GROUP_GRID: 2
};

// 7.16.2.3.2 Methods
Programme.prototype = {
    getField: function (fieldId) {
        if (fieldId == 'itemDesChar') {
            var sb = new CStringBuilder();
            sb.append('{"itemDesChar":');
            sb.append('{"CastnCrew": ');
            sb.append('[');
            sb.append('{"title":"Actor","name":"Paul Grwith"},');
            sb.append('{"title":"Actress","name":"Elizabeth Daily"},');
            sb.append('{"title":"Director","name":"George Miller"},');
            sb.append('{"title":"Laa-Laa","name":"Nikky Smedley"},');
            sb.append('{"title":"Actress","name":"Elizabeth Daily"},');
            sb.append('{"title":"Actor","name":"Paul Grwith"},');
            sb.append('{"title":"Director","name":"George Miller"},');
            sb.append('{"title":"Laa-Laa","name":"Nikky Smedley"}');
            sb.append('],');
            sb.append('"BrandName":"xxx",');
            sb.append('"EpisodeNumber":"3",');
            sb.append('"EpisodeName":"yyyyy",');
            sb.append('"SeriesNumber": "3",');
            sb.append('"SeriesName": "zzzzz"');
            sb.append('}');
            sb.append('}');
            return sb.toString();
        } else if (fieldId == '0x32') {
            return '0x32';
        } else if (fieldId === 'seriesID') {
            return Math.floor((Math.random() * 100));
        }
        return "";
    }
};

// 7.16.2.4 DVB-SI extensions to Programme
$.extend(true, Programme.prototype, {
    getSIDescriptors: function (descriptorTag, descriptorTagExtension) {
        return new StringCollection();
    }
});

//OIPF 7.16.3 The ProgrammeCollection class
function ProgrammeCollection() {
}
;

ProgrammeCollection.prototype = new Collection();

// for test oipf.metadata.js  --copy from freesat end --- if don't need this. remove this. it's ok
// 7.16.5 Extensions for palyback of selected media components
// 7.16.5.1 Media playback extensions
// 7.16.5.1.1 Contants
AVControl.TComponentType = {
    ECOMPONENT_TYPE_VIDEO: 0,
    ECOMPONENT_TYPE_AUDIO: 1,
    ECOMPONENT_TYPE_SUBTITLE: 2
};

$.extend(true, AVControl.prototype, {
    // 7.16.5.1.2 Properties
    // 7.16.5.1.3 Methods
    getComponents: function (componentType) {
        return gOipfEmul.getAVComponentCollection(componentType);
    },
    getCurrentActiveComponents: function (componentType) {
        return gOipfEmul.getActiveAVComponentCollection(componentType);
    },
    selectComponent: function () {
        // void selectComponent (AVComponent component)
        // void selectComponent (Integer componentType)
    },
    unselectComponent: function () {
        // void unselectComponent (AVComponent component)
        // void unselectComponent (Integer componentType)
    },
    offComponent: function () {
    },
    // 7.16.5.1.4 Events
    SelectedComponentChange: function (componentType) {
        if (this.onSelectedComponentChange) {
            this.onSelectedComponentChange(componentType);
        }
    }
});

// 7.16.5.1.1 Contants
VideoBroadcast.TComponentType = {
    ECOMPONENT_TYPE_VIDEO: 0,
    ECOMPONENT_TYPE_AUDIO: 1,
    ECOMPONENT_TYPE_SUBTITLE: 2
};

$.extend(true, VideoBroadcast.prototype, {
    // 7.16.5.1.2 Properties
    // 7.16.5.1.3 Methods
    getComponents: function (componentType) {
        return gOipfEmul.getAVComponentCollection(componentType);
    },
    getCurrentActiveComponents: function (componentType) {
        return gOipfEmul.getActiveAVComponentCollection(componentType);
    },
    selectComponent: function (component) {
        // void selectComponent (AVComponent component)
        var that = this;
        var type = component.type;
        var components = that.getCurrentActiveComponents(type);
        components.push(component);
    },
    unselectComponent: function (component) {
        // void unselectComponent (AVComponent component)
        var that = this;
        var type = component.type;
        var components = that.getCurrentActiveComponents(type);
        $.each(components, function (i, c) {
            if (c !== undefined && c.componentTag === component.componentTag && c.pid === component.pid) {
                components.splice(i, 1);
            }
        });
    },
    offComponent: function (componentType, p) {
        var that = this;
        var type = componentType;
        var components = that.getCurrentActiveComponents(type);
        $.each(components, function (i, c) {
            if (c !== undefined) {
                components.splice(i, 1);
            }
        });
    },
    // 7.16.5.1.4 Events
    SelectedComponentChange: function (componentType) {
        if (this.onSelectedComponentChange) {
            this.onSelectedComponentChange(componentType);
        }
    }
});

//OIPF 7.16.5.2
function AVComponent() {
    var that = this;
    that.componentTag = 1;
    that.pid = 1;
    that.type = AVControl.TComponentType.ECOMPONENT_TYPE_VIDEO;
    that.encoding = '';
    that.encrypted = true;
}
;

//OIPF 7.16.5.3 The AVVideoComponent class
function AVVideoComponent() {
    // 7.16.5.3.1 Properties
    this.aspectRatio = 1;
}
;

//OIPF 7.16.5.4 The AVAudioComponent class
function AVAudioComponent() {
    var that = this;

    // 7.16.5.4.1 Properties
    that.language = "kor";
    that.audioDescription = true;
    that.audioChannels = 7;
}
;

//OIPF 7.16.5.5 The AVSubtitleComponent class
function AVSubtitleComponent() {
    var that = this;

    // 7.16.5.5.1 Properties
    that.language = "kor";
    that.hearingImpaired = true;
}
;

//OIPF 7.16.5.6 The AVComponentCollection class
var AVComponentCollection = function () {
};
AVComponentCollection.prototype = new Collection();

function supportedHdVideoFormatCollection() {
}
;

supportedHdVideoFormatCollection.prototype = new StringCollection();

/* Lazy init */
(function () {
    // Init LocalSystem
    var configuration = new Configuration();
    var localSystem = new LocalSystem();
    var avOutputCollection = new AVOutputCollection();
    var idx;

    for (idx = 0; idx < 2; idx += 1) {
        avOutputCollection.push(new AVOutput());
    }
    ;
    localSystem.outputs = avOutputCollection;

    var signalInfo = new SignalInfo();
    var tuner = new Tuner();
    tuner.signalInfo = signalInfo;
    var tunerCollection = new TunerCollection();
    tunerCollection.push(tuner);
    localSystem.tuners = tunerCollection;
    setInterval(function () {
        signalInfo.strength = parseInt(Math.random() * 100);
        signalInfo.quality = parseInt(Math.random() * 100);
    }, 5000);

    $('body').keydown(function (e) {
        localSystem.__fnPowerWakeupKey(e);
    });

    Configuration.prototype.localSystem = localSystem;

    Configuration.prototype.localSystem.networkInterfaces = new NetworkInterfaceCollection();

    Configuration.prototype.configuration = configuration;

    // for test oipf.metadata.js  --copy from freesat --- if don't need this. remove this. it's ok
    VideoBroadcast.prototype.programmes = new ProgrammeCollection();
    VideoBroadcast.prototype.currentChannel = new Channel();
    ChannelConfig.prototype.favouriteLists = new FavouriteListCollection();
})(window);
//# sourceMappingURL=emul.js.map
