// filename : fw/service/HMX/emul/emul.js
/* jslint sloppy:true, nomen:true, vars:true */
console.log("[EMULATOR] " + 'Loaded [HMXEmul.js]');

$.extend(true, oipfObjectFactory, {
    createHumaxConfigurationObject: function () {
        // TODO: object instance management required
        return new Configuration();
    },
    createMediaSearchManagerObject: function () {
        gOipfEmul._mediaObject = new HMX_Media();
        return gOipfEmul._mediaObject;
    },
    createFileSystemManagerObject: function () {
        gOipfEmul._fileSystemObject = new HMX_FileSystem();
        return gOipfEmul._fileSystemObject;
    },
    createHumaxStorageUtilObject: function () {
        gOipfEmul._storageUtil = new HMXStorageUtil();
        return gOipfEmul._storageUtil;
    },
    createHumaxCasUtil: function () {
        gOipfEmul.casUtil = new HMXCasUtil();
        return gOipfEmul.casUtil;
    },
    createHumaxOperatorFeatureObject: function () {
        gOipfEmul._operatorFeature = new HMXOperatorFeature();
        return gOipfEmul._operatorFeature;
    },
    createHumaxOperatorFreesatObject: function () {
        gOipfEmul._operatorFreesat = new HMXOperatorFreesat();
        return gOipfEmul._operatorFreesat;
    },
    createParentalControlManagerObject: function () {
        gOipfEmul._pcManager = new ParentalControlManager();
        gOipfEmul._pcManager.parentalRatingSchemes = gOipfEmul.getDefaultParentalRatingScheme();
        return gOipfEmul._pcManager;
    },
    /**
    * SAMBA, dLNA, ContentBrowser, WOON
    */
    createDlnaManagerObject: function () {
        console.log("[EMULATOR] " + 'createDlnaManagerObject()');
        gOipfEmul._HDlna = new HDlna();
        return gOipfEmul._HDlna;
    },
    createSambaManagerObject: function () {
        console.log("[EMULATOR] " + 'createSambaManagerObject()');
        gOipfEmul._HSamba = new HSamba();
        return gOipfEmul._HSamba;
    },
    createFtpManagerObject: function () {
        console.log("[EMULATOR] " + 'createFtpManagerObject()');
        gOipfEmul._HFtp = new HFtp();
        return gOipfEmul._HFtp;
    },
    createContentManagerObject: function () {
        console.log("[EMULATOR] " + 'createContentManagerObject()');
        gOipfEmul._HContentManager = new HContentManager();
        return gOipfEmul._HContentManager;
    },
    createWoonManagerObject: function () {
        console.log("[EMULATOR] " + 'createWoonManagerObject()');
        gOipfEmul._HWoon = new HWoon();
        return gOipfEmul._HWoon;
    },
    createBluetoothManagerObject: function () {
        console.log("[EMULATOR] " + 'createBluetoothManagerObject()');
        gOipfEmul._HBluetoothManager = new HBluetoothManager();
        return gOipfEmul._HBluetoothManager;
    },
    createHumaxVideoOnDemandObject: function () {
        return window.oipfObjectFactory.createVideoMpegObject();
    },
    createDialManagerObject: function () {
        console.log("[EMULATOR] " + 'creatDialManagerObject()');
        gOipfEmul._HDialManager = new HDial();
        return gOipfEmul._HDialManager;
    },
    createSatipManagerObject: function () {
        console.log("[EMULATOR] " + 'creatSatipManagerObject()');
        gOipfEmul._HSatipManager = new HSatip();
        return gOipfEmul._HSatipManager;
    },
    createAirplayManagerObject: function () {
        console.log("[EMULATOR] " + 'createAirplayManagerObject()');
        gOipfEmul._HAirplay = new HAirplay();
        return gOipfEmul._HAirplay;
    },
    createAlexaHelperManagerObject: function () {
        console.log("[EMULATOR] " + 'createAlexaHelperManagerObject()');
        gOipfEmul._HAlexaHelper = new HAlexaHelper();
        return gOipfEmul._HAlexaHelper;
    }
});

// Extend ApplicationManager Object
$.extend(true, ApplicationManager.prototype, {
    addOIPFApplication: function (wd) {
        return true;
    },
    removeOIPFApplication: function (wd) {
    },
    removeAllOIPFApplications: function (wd) {
    },
    startPortalApplication: function () {
    },
    stopPortalApplication: function () {
    },
    createHbbTVApplication: function () {
    },
    destroyHbbTVApplication: function () {
    },
    createPortalApplication: function() {
    }
});


ApplicationManager.prototype.getOwnerApplication = function (document) {
    var own = new Application2(this);
    this.getOwnerApplication = function (document) {
        return own;
    };
    return own;
};
ApplicationManager.prototype._sendKeyEventToTopMostApp = function (keyEvent) {
    var a = this;
    if (a._topMostApp) {
        return a._topMostApp._sendKeyEvent(keyEvent);
    }
    return false;
};
ApplicationManager.prototype.destroyHbbTVApplication = function () {
};
ApplicationManager.prototype.createHbbTVApplication = function () {
};
ApplicationManager.prototype.destroyHumaxApplication = function () {
};

var Application2 = function (appMgr, parent, url) {
    var a = this;
    a._appMgr = appMgr;
    a._parent = parent;
    a._url = url;
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
        tvtvWakeupTime: null,
        isTvtvStandbyEnable: null,
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
        },
        prepareWakeupTvtvUpdate: function (time, isTvtvStandbyEnable) {
            var b = this;
            if (typeof arguments[0] == 'undefined') {
                b.tvtvWakeupTime = null;
                b.isTvtvStandbyEnable = true;
            } else {
                b.tvtvWakeupTime = time;
                b.isTvtvStandbyEnable = isTvtvStandbyEnable;
            }
        }
    };
    if (url) {
        a._window = new ApplicationWindow2(a, url);
        var eIframe = a._window._$iframe[0];
        a.window = eIframe.contentWindow;
    }
};
Application2.prototype = new Application();
Application2.prototype.createApplication = function (uri, createChild) {
    if (this._children === undefined) {
        this._children = [];
    }
    var a = this._children;
    var l = a.push(new Application2(this._appMgr, this, uri));
    return a[l - 1];
};
Application2.prototype.destroyApplication = function () {
    var a = this;
    Application.prototype.destroyApplication.call(a);
    a._window.destroy();
};
Application2.prototype._sendKeyEvent = function (keyEvent) {
    var a = this;
    if (a._window && a._url && a._url.indexOf('3rdParty') != 0) {
        var eIframe = a._window._$iframe[0];
        eIframe.contentWindow.postMessage(JSON.stringify(keyEvent), '*');
        return true;
    }
    return false;
};
Application2.prototype.activateInput = function (b) {
    var a = this;
    /*
    Application.prototype.activateInput.call(a, b);
    $(a._appMgr._activateDocument.body).keydown(function (e) {
        if (a._parent) {
            a._parent.trigger2('KeyDown', [
                {
                    name: 'which',
                    value: e.which
                }, {
                    name: 'keyCode',
                    value: e.which
                }]);
        }
    });
    */
    a._appMgr._topMostApp = a;
};
Application.prototype._requestActviateInput = function (child, b) {
    return
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
            child._window._$iframe.css("z-index", o.length * 100 + 1000);
            if (b) {
                this._appMgr._activateDocument = child._window._$iframe[0].contentDocument;
            }
            break;
        }
    }
};
Application2.prototype.deactivateInput = function () {
    var a = this;
    Application.prototype.deactivateInput.call(a);
    var $iframe = a._window._$iframe;
    $iframe.css('z-index', '');
};
Application.prototype.show = function () {
    var a = this;
    if (a._window) {
        a.visible = true;
        a._window._$iframe.show();
    }
};
Application.prototype.hide = function () {
    var a = this;
    if (a._window) {
        a.visible = false;
        a._window._$iframe.hide();
    }
};

var ApplicationWindow2 = function (app, url) {
    var a = this;
    if (arguments.length) {
        a.init(app, url);
    }
};

ApplicationWindow2.prototype = new ApplicationWindow();
ApplicationWindow2.prototype.init = function (app, url) {
    var a = this;
    ApplicationWindow.prototype.init.call(a, app, url);
    $('body').prepend(a._$iframe);
};
ApplicationWindow2.prototype.destroy = function () {
    var a = this;
    a._$iframe.remove();
};
ApplicationWindow2.prototype.postMessage = function (message, baseUrl) {
    var a = this;
    if (baseUrl && baseUrl.indexOf('file://') == 0) {
        var eIframe = a._$iframe[0];

        //setTimeout(function() {
        eIframe.contentWindow.postMessage(message, baseUrl);
        //}, 1);
    } else {
        ApplicationWindow.prototype.postMessage.call(a, message, baseUrl);
    }
};

// Extend Configuration Object
$.extend(true, Configuration.prototype.configuration, {
    // Added Properties
    autoDelete: true,
    instantReplayTime: 15,
    skipForwardTime: 60,
    operatorId: 8,
    subtitleFontType: 0,
    infoDisplayTimeout: 5,
    enableSeriesPlay: 'false',
    enableIpEPG: 'false',
    EPGType: 0,
    ipepgLastUpdateTime: '1000000000',
    tvtvEpgLastUpdateTime: '30000',
    enableRemoteRecord: 'false',
    satRecordUpdateInterval: 0,
    updatePeriodRemoteRecord: '0:86400',
    updateIntervalRemoteRecord: '1800',
    regcodeRemoteRecord: '19750717',
    firstTimeBoot: false,
    standbyPowerMode: 0,
    TYPE_SUBTITLE_FONT_FIXED: 0,
    TYPE_SUBTITLE_FONT_VARIABLE: 1,
    // preferredMenuLanguage : 'jpn',  //TODO
    preferredMenuLanguage: 'eng',
    offsetGMT: 240,
    daylightsaving: 0,
    satRecordPriority: 0,
    noOtaAction: 'false',
    noOtaVer: '',
    laterOtaCount: 0,
    guidancepolicy: 0,
    // Added Method
    getField: function (arg) {
        var result;

        if (arg === 'post_code') {
            this.post_cdee = result = 'dummy post code';
        } else if (arg == 'enableSeriesPlay') {
            result = this.enableSeriesPlay;
        } else if (arg == 'enableIPEPG') {
            result = this.enableIpEPG;
        } else if (arg == 'EPGType') {
            result = this.EPGType;
        } else if (arg == 'enableRemoteRecord') {
            result = this.enableRemoteRecord;
        } else if (arg == 'satRecordUpdateInterval') {
            result = this.satRecordUpdateInterval;
        } else if (arg == 'updatePeriodRemoteRecord') {
            result = this.updatePeriodRemoteRecord;
        } else if (arg == 'updateIntervalRemoteRecord') {
            result = this.updateIntervalRemoteRecord;
        } else if (arg == 'registrationCodeRemoteRecord') {
            result = this.regcodeRemoteRecord;
        } else if (arg == 'ipepgLastUpdateTime') {
            result = this.ipepgLastUpdateTime;
        } else if (arg == 'tvtvEpgLastUpdateTime') {
            result = this.tvtvEpgLastUpdateTime;
        } else if (arg == 'satRecordUpdateInterval') {
            result = this.satRecordUpdateInterval;
        } else if (arg == 'satRecordPriority') {
            result = (this.satRecordPriority) ? "true" : "false";
        } else if (arg == 'offsetGMT') {
            result = this.offsetGMT;
        } else if (arg == 'daylightsaving') {
            result = this.daylightsaving;
        } else if (arg == 'updatetime') {
            result = this.updatetime;
        } else if (arg == 'tvtvUpdateTime') {
            result = this.tvtvUpdateTime;
        } else if (arg == 'no_ota_action') {
            result = this.noOtaAction;
        } else if (arg == 'no_ota_swversion') {
            result = this.noOtaVer;
        } else if (arg == 'later_ota_count') {
            result = this.laterOtaCount;
        } else {
            result = 'undefined getField argument';
        }

        return result;
    },
    setField: function (key, value) {
        switch (key) {
            case 'post_code':
                this.post_code = value;
                break;
            case 'grabkey':
                this.grabkey = value;
                break;
            case 'ungrabkey':
                this.ungrabkey = value;
                break;
            case 'enableSeriesPlay':
                this.enableSeriesPlay = value;
                break;
            case 'enableIPEPG':
                this.enableIpEPG = value;
                break;
            case 'EPGType':
                this.EPGType = value;
                break;
            case 'enableRemoteRecord':
                this.enableRemoteRecord = value;
                break;
            case 'satRecordUpdateInterval':
                this.satRecordUpdateInterval = value;
                break;
            case 'satRecordPriority':
                this.satRecordPriority = value;
                break;
            case 'updatePeriodRemoteRecord':
                this.updatePeriodRemoteRecord = value;
                break;
            case 'updateIntervalRemoteRecord':
                this.updateIntervalRemoteRecord = value;
                break;
            case 'offsetGMT':
                this.offsetGMT = value;
                break;
            case 'daylightsaving':
                this.daylightsaving = value;
                break;
            case 'updatetime':
                this.updatetime = value;
                break;
            case 'no_ota_action':
                this.noOtaAction = value;
                break;
            case 'no_ota_swversion':
                this.noOtaVer = value;
                break;
            case 'later_ota_count':
                this.laterOtaCount = value;
                break;
        }
    },
    doFactoryDefault: function () {
        console.log("[EMULATOR] " + 'Configuration.prototype.doFactoryDefault');
    },
    getCamNameBySlot: function (value) {
        return 'Test CAM # ' + value;
    }
});

// Extends LocalSystem Object
$.extend(true, LocalSystem.prototype, {
    // Added Properties
    hmx_outputs: null,
    hmx_audios: null,
    networkManager: new NetworkManager(),
    antennaInfoLists: null,
    frontPanel: null,
    // Added Method
    registerEventListener: function () {
    },
    unregisterEventListener: function () {
    },
    getField: function (arg) {
        var result;
        switch (arg) {
            case 'system_id':
                var eModel = ENV.listModel;
                switch (ENV.model) {
                    case eModel.FVP4000T:
                        result = '9010.7D31';
                        //result = '0000.0000';
                        break;
                    case eModel.HMS1000S:
                        result = '809C.5010';
                        break;
                    case eModel.IR4000HD:
                        result = '809C.3040';
                        break;
                    case eModel.DEFAULT:
                        result = '0000.0000';
                        break;
                }
                break;
            case 'loader_version':
                if (hx.config.model === 'IR4000HD') {
                    result = 'IRDETO 3.2.0';
                } else if (hx.config.model === 'HMS1000S') {
                    result = 'DESFAB 0.04';
                } else {
                    result = 'HUMAX 0.00';
                }
                break;
            case 'update_date':
                result = '07 OCT 2012';
                break;
            case 'ciplus_enable':
                result = 'enable';
                break;
            case 'ciplus_version':
                result = '1.00.00';
                break;
            case 'cislot_count':
                result = '2';
                break;
            case 'micom_version':
                result = '0.00.00';
                break;
            case 'rcu_customcode':
                result = '1';
                break;
            case 'url_portal':
                result = 'http://test.humaxtvportal.com';
                break;
            case 'url_rp':
                result = 'http://test.humaxtvportal.com';
                break;
            case 'irdeto_jtag':
                result = this.irdeto_jtag;
                break;
            case 'irdeto_cwe':
                result = this.irdeto_cwe;
                break;
            case 'irdeto_sboot':
                result = this.irdeto_sboot;
                break;
            case 'irdeto_otp':
                result = this.irdeto_otp;
                break;
            case 'irdeto_mancode':
                result = this.irdeto_mancode;
                break;
            case 'irdeto_hwcode':
                result = this.irdeto_hwcode;
                break;
            case 'irdeto_variant':
                result = this.irdeto_variant;
                break;
            case 'irdeto_sysid':
                result = this.irdeto_sysid;
                break;
            case 'irdeto_sigver':
                result = this.irdeto_sigver;
                break;
            case 'irdeto_keyver':
                result = this.irdeto_keyver;
                break;
            case 'irdeto_virtualscw':
                result = this.irdeto_virtualscw;
                break;
            case 'irdeto_control_byte':
                result = '2';
                break;
            case 'fusing_info':
                result = '{"FUSINGINFO" : [{"NAME" : "SCS", "VALUE" : 1} , {"NAME" : "JTAG", "VALUE" : 1} , {"NAME" : "OTP", "VALUE" : 1} , {"NAME" : "CWE", "VALUE" : 1}] }';
                break;
            case 'nand_badblocks':
                result = '{"NANDBADBLOCKS" : [{"NAME" : "LOADER", "VALUE" : 2} , {"NAME" : "UBI", "VALUE" : 10} , {"NAME" : "UBIFS", "VALUE" : 3}]}';
                break;
            case 'nagra_fusing_info':
                result = '{"FUSINGINFO":[{"NAME":"SCS", "VALUE":0},{"NAME":"JTAG", "VALUE":0},{"NAME":"OTP", "VALUE":0},{"NAME":"CWE", "VALUE":0}]}';
                break;
            case 'fvc_networkuriprefix':
                result = 'https://auth-ctv.cp-test.digitaluk.co.uk';
                //result = 'https://auth-ctv.digitaluk.co.uk';
                break;
            case 'network_ids':
                result = '{"NETWORK_ID": 12339}';
                break;
            default:
                console.log('undefined getField argument');
                result = "";
                break;
        }
        return result;
    },
    setField: function (key, value) {
        switch (key) {
            case 'irdeto_virtualscw':
                this.irdeto_virtualscw = value;
                break;
        }
    }
});

// Extends RecordingScheduler Object
$.extend(true, RecordingScheduler.prototype, {
    _PVREvent: function (state, recording, error, conflicts, recConflict, alternatives, altevents, conflictGroupInfo) {
        var a = this, fn = this.onPVREvent, param;
        if (fn) {
            fn.apply(this, [state, recording, error, conflicts, recConflict, alternatives, altevents, conflictGroupInfo]);
        }
        if (a.PVREventListener) {
            param = {
                state: state,
                recording: recording,
                error: error,
                conflicts: conflicts,
                recConflict: recConflict,
                alternatives: alternatives,
                altevents: altevents,
                conflictGroupInfo: conflictGroupInfo
            };
            a.PVREventListener(param);
        }
    },
    _FileOpEvent: function (ticket, state) {
        var a = this, fn = this.onFileOpEvent, param;
        if (fn) {
            fn(ticket, state);
        }
        if (a.FileOpEventListener) {
            param = {
                ticket: ticket,
                state: state
            };
            a.FileOpEventListener(param);
        }
    },
    addEventListener: function (eventName, listener) {
        var a = this;
        a[eventName + 'Listener'] = listener;
    },
    record: function (programme, factory) {
        var that = this;
        var list = gOipfEmul.scheduledRecordingCollection;
        var scheduledRecording = new ScheduledRecording();
        scheduledRecording.startPadding = 0;
        scheduledRecording.endPadding = 0;
        scheduledRecording.repeatDays = 0;
        scheduledRecording.name = programme.name;
        scheduledRecording.longName = programme.longName;
        scheduledRecording.description = programme.description;
        scheduledRecording.longDescription = programme.longDescription;
        scheduledRecording.startTime = programme.startTime;
        scheduledRecording.duration = programme.duration;
        scheduledRecording.channel = programme.channel;
        scheduledRecording.programmeID = programme.programmeID;
        scheduledRecording.programmeIDType = programme.programmeIDType;
        scheduledRecording.episode = programme.episode;
        scheduledRecording.totalEpisodes = programme.totalEpisodes;
        scheduledRecording.isScheduledAsSeries = false;
        scheduledRecording.seriesID = 0;
        scheduledRecording.parentalRatings = programme.parentalRatings;

        // Humax extention
        scheduledRecording.factoryType = factory;

        setTimeout(function () {
            list.push(scheduledRecording);

            //            that._PVREvent(11, scheduledRecording, null, list);	//RECORDING_ERROR
            that._PVREvent(1, scheduledRecording);
        }, 1);
        //RECORDING_SCHEDULED
    },
    recordAt: function (startTime, duration, repeatDays, channelID, factory) {
        var that = this;
        var list = gOipfEmul.scheduledRecordingCollection;
        var scheduledRecording = new ScheduledRecording();
        scheduledRecording.startPadding = 0;
        scheduledRecording.endPadding = 0;
        scheduledRecording.repeatDays = 0;
        scheduledRecording.name = 'TBR' + startTime;
        scheduledRecording.startTime = startTime;
        scheduledRecording.duration = duration;
        scheduledRecording.channel = gOipfEmul.getChannel(channelID);
        scheduledRecording.isScheduledAsSeries = false;
        scheduledRecording.seriesID = 0;

        // Humax extention
        scheduledRecording.factoryType = factory;

        setTimeout(function () {
            list.push(scheduledRecording);

            //            that._PVREvent(11, scheduledRecording, null, list);   //RECORDING_ERROR
            that._PVREvent(1, scheduledRecording);
        }, 1);

        //RECORDING_SCHEDULED
        return scheduledRecording;
    },
    getScheduledRecordings: function () {
        return gOipfEmul.scheduledRecordingCollection;
    },
    remove: function (recording) {
        var that = this, list, i, ti, pgId, facType;
        if (recording.id) {
            list = gOipfEmul.recordingScheduler.recordings;
            setTimeout(function () {
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    if (list[i].id === recording.id) {
                        list.splice(i, 1);
                        that._PVREvent(2, recording);

                        break;
                    }
                }
            }, 1);
        } else {
            list = gOipfEmul.recordingScheduler.recordings;
            setTimeout(function () {
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    if ((list[i].startTime === recording.startTime) && (list[i].duration === recording.duration) && (list[i].factoryType === recording.factoryType)) {
                        list.splice(i, 1);
                        break;
                    }
                }
            }, 1);

            list = gOipfEmul.scheduledRecordingCollection;
            pgId = recording.programmeID;
            facType = recording.factoryType;

            for (i = 0, ti = list.length; i < ti; i += 1) {
                if ((list[i].startTime === recording.startTime) && (list[i].duration === recording.duration) && (list[i].factoryType === recording.factoryType)) {
                    list.splice(i, 1);
                    that._PVREvent(2, recording);

                    break;
                }
            }
        }
    },
    // Added Properties
    recordings: new RecordingCollection(),
    discInfo: {
        free: 10,
        total: 95,
        reserved: 5
    },
    // Added Method
    recordConfirm: function () {
    },
    //7.10.4.2 Methods
    getRecording: function (id) {
        var list = gOipfEmul.recordingScheduler.recordings;
        var recording, i, length = list.length;

        for (i = 0; i < length; i += 1) {
            if (list[i].id === id) {
                recording = list[i];
            }
        }

        return recording;
    },
    getCurrentRecordings: function () {
        var list = new Collection();
        list.push(gOipfEmul.recordingScheduler.recordings[0]);
        return list;
    },
    stop: function (recording) {
        var that = this;
        setTimeout(function () {
            recording.state = 7;

            //RECORDING_REC_COMPLETED
            recording.showType = 1;
            that._PVREvent(6, recording);

            //RECORDING_REC_UPDATED
            setTimeout(function () {
                that._PVREvent(7, recording);

                //RECORDING_REC_COMPLETED
                setTimeout(function () {
                    var slist = gOipfEmul.scheduledRecordingCollection;
                    $.each(slist, function (i, v) {
                        if ((v.start === recording.start) && (v.factoryType === recording.factoryType) && (v.channel.ccid === recording.channel.ccid)) {
                            that._PVREvent(2, v);

                            //RECORDING_DEL_SCHEDULED
                            gOipfEmul.scheduledRecordingCollection.splice(i, 1);
                            return false;
                        }
                    });
                }, 100);
            }, 100);
        }, 100);
    },
    copyRecording: function (recording, storageId) {
        //'RECORDING_FILEOP_STATE_ING', //0
        //'RECORDING_FILEOP_STATE_COMPLETE', //1
        //'RECORDING_FILEOP_STATE_ERROR', //2
        //'RECORDING_FILEOP_STATE_UNKNOWN', //3
        //'RECORDING_FILEOP_STATE_STOP' //4
        var that = this, ticketNumber = recording.recordingStartTime;
        setTimeout(function () {
            that._FileOpEvent(ticketNumber, 0);
            setTimeout(function () {
                that._FileOpEvent(ticketNumber, 1);
            }, 7000);
        }, 100);
        console.log("[EMULATOR] " + '[recordingScheduler][copyRecording] ' + recording.id + ' storage ' + storageId);
        return ticketNumber;
    },
    moveRecording: function (recording, storageId) {
        //'RECORDING_FILEOP_STATE_ING', //0
        //'RECORDING_FILEOP_STATE_COMPLETE', //1
        //'RECORDING_FILEOP_STATE_ERROR', //2
        //'RECORDING_FILEOP_STATE_UNKNOWN', //3
        //'RECORDING_FILEOP_STATE_STOP' //4
        var that = this, ticketNumber = recording.recordingStartTime;
        setTimeout(function () {
            that._FileOpEvent(ticketNumber, 0);
            setTimeout(function () {
                that._FileOpEvent(ticketNumber, 1);
            }, 7000);
        }, 100);
        console.log("[EMULATOR] " + '[recordingScheduler][moveRecording] ' + recording.id + ' storage ' + storageId);
        return ticketNumber;
    },
    removeRecording: function (recording) {
        console.log("[EMULATOR] " + '[recordingScheduler][removeRecording] ' + recording.id);
    },
    stopFileOp: function (ticket) {
        console.log("[EMULATOR] " + '[recordingScheduler][stopFileOp] ' + ticket);
        var that = this;
        setTimeout(function () {
            that._FileOpEvent(ticket, 4);
        }, 100);
    },
    setSeriesLock: function (seriesID, bLock) {
        console.log("[EMULATOR] " + '[recordingScheduler][setSeriesLock] ' + seriesID + ' bLock ' + bLock);
    },
    startSatRecord: function () {
    },
    recordCrid: function(aFactoryType, aCRID, aTitle, aChannelID) {
        console.log("[EMULATOR] " + '[recordCrid]');
    }
});

$.extend(true, Recording.prototype, {
    // Added Properties
    // Added Method
    getField: function () {
        return null;
    }
});

$.extend(true, MetadataSearch.prototype, {
    addConstraint: function (field, value) {
        var that = this;

        that._constraintField = field;
        that._constraintValue = value;
    }
});

function FTPConfig() {
    this.ftpStatus = 0;
}

function HMX_NetworkKDMSConfig() {
    this.actionStatus = true;
}

HMX_NetworkKDMSConfig.prototype = {
    start: function () {
    },
    stop: function () {
    }
};

function HMX_NetworkAccessPoint() {
    var that = this;
    that.essid = 'AP 01';
    that.encrypted = 0;
    that.authen = 0;
    that.secure = 0;
    that.waveStrength = 80;
    that.isConnected = 0;
}

function HMX_NetworkPPPoEConfig() {
    var that = this;
    that.id = 'aaa';
    that.password = 'bbb';
}

HMX_NetworkPPPoEConfig.prototype.getID = function () {
    var a = this;
    return a.id;
};
HMX_NetworkPPPoEConfig.prototype.getPassword = function () {
    var a = this;
    return a.password;
};
HMX_NetworkPPPoEConfig.prototype.setID = function (newid) {
    var a = this;
    a.id = newid;
    console.log("[EMULATOR] " + 'new id is :' + a.id);
};
HMX_NetworkPPPoEConfig.prototype.setPassword = function (newpw) {
    var a = this;
    a.password = newpw;
    console.log("[EMULATOR] " + 'new pw is :' + a.password);
};
HMX_NetworkPPPoEConfig.prototype.connect = function () {
    var a = this;
    a.onPPPoEStatusChanged('connect');
};
HMX_NetworkPPPoEConfig.prototype.disconnect = function () {
    var a = this;
    a.onPPPoEStatusChanged('disconnect');
};
HMX_NetworkPPPoEConfig.prototype.onPPPoEStatusChanged = function (state) {
    console.log("[EMULATOR] " + 'PPPoEStatusChanged :' + state);
};
function HMX_NetworkWiFiConfig() {
    var that = this;
    that._processing = {
        AP_SCAN: 0,
        AP_CONNECT: 1
    };
    that._task = 0;
    that._handleTimer = null;

    that._accessPointUpdated = function () {
        var accessPoint = new HMX_NetworkAccessPoint();

        if (that._task === that._processing.AP_SCAN) {
            that.onAccessPointUpdated(0, accessPoint);

            accessPoint = new HMX_NetworkAccessPoint();
            accessPoint.essid = 'AP 1';
            accessPoint.waveStrength = 40;
            accessPoint.secure = 0;
            accessPoint.authen = 0;
            that.onAccessPointUpdated(0, accessPoint);

            accessPoint2 = new HMX_NetworkAccessPoint();
            accessPoint2.essid = 'AP 2';
            accessPoint2.waveStrength = 40;
            accessPoint2.secure = 1;
            accessPoint2.authen = 0;
            accessPoint2.isConnected = 1;
            that.onAccessPointUpdated(0, accessPoint2);

            accessPoint3 = new HMX_NetworkAccessPoint();
            accessPoint3.essid = 'AP 3';
            accessPoint3.waveStrength = 40;
            accessPoint3.secure = 1;
            accessPoint3.authen = 0;
            that.onAccessPointUpdated(0, accessPoint3);

            accessPoint4 = new HMX_NetworkAccessPoint();
            accessPoint4.essid = 'AP 4';
            accessPoint4.waveStrength = 40;
            accessPoint4.secure = 0;
            accessPoint4.authen = 0;
            that.onAccessPointUpdated(0, accessPoint4);

            accessPoint5 = new HMX_NetworkAccessPoint();
            accessPoint5.essid = 'AP 5';
            accessPoint5.waveStrength = 40;
            accessPoint5.secure = 0;
            accessPoint5.authen = 0;
            that.onAccessPointUpdated(0, accessPoint5);

            accessPoint6 = new HMX_NetworkAccessPoint();
            accessPoint6.essid = 'AP 6';
            accessPoint6.waveStrength = 40;
            accessPoint6.secure = 0;
            accessPoint6.authen = 0;
            that.onAccessPointUpdated(0, accessPoint6);

            accessPoint7 = new HMX_NetworkAccessPoint();
            accessPoint7.essid = 'AP 7';
            accessPoint7.waveStrength = 40;
            accessPoint7.secure = 1;
            accessPoint7.authen = 0;
            that.onAccessPointUpdated(0, accessPoint7);

            accessPoint8 = new HMX_NetworkAccessPoint();
            accessPoint8.essid = 'AP 8';
            accessPoint8.waveStrength = 20;
            accessPoint8.secure = 0;
            accessPoint8.authen = 0;
            that.onAccessPointUpdated(0, accessPoint8);

            accessPoint9 = new HMX_NetworkAccessPoint();
            accessPoint9.essid = 'AP 9';
            accessPoint9.waveStrength = 80;
            accessPoint9.secure = 1;
            accessPoint9.authen = 0;
            that.onAccessPointUpdated(0, accessPoint9);

            that.onAccessPointUpdated(1, null);
        } else {
            that.onAccessPointUpdated(3, accessPoint);
        }

        if (that._handleTimer) {
            clearInterval(that._handleTimer);
            that._handleTimer = null;
        }
    };
}

HMX_NetworkWiFiConfig.prototype = new DOM2EventSource();

HMX_NetworkWiFiConfig.prototype.apScanning = function () {
    var a = this;
    a._task = a._processing.AP_SCAN;
    a._handleTimer = setInterval(a._accessPointUpdated, 3000);
};
HMX_NetworkWiFiConfig.prototype.applyConnectToAP = function (ap, passwd) {
    var a = this;
    a._task = a._processing.AP_CONNECT;
    setInterval(a.onAccessPointUpdated(3, null), 3000);
};
HMX_NetworkWiFiConfig.prototype.getConnectedAPInfo = function () {
    return new HMX_NetworkAccessPoint();
};
HMX_NetworkWiFiConfig.prototype.onAccessPointUpdated = function (state, ap) {
};

function NetworkManager() {
    this.defaultDevIndex = 0;
}

NetworkManager.prototype.getFTPConfig = function () {
    var ftp = new FTPConfig();
    ftp.ftpStatus = 0;
    return ftp;
};
NetworkManager.prototype.getDMSConfig = function () {
    return new HMX_NetworkDMSConfig();
};
NetworkManager.prototype.getWifiConfig = function () {
    return new HMX_NetworkWiFiConfig();
};
NetworkManager.prototype.onNetworkStatusChanged = function (state) {
};
NetworkManager.prototype.getPPPoEConfig = function () {
    return new HMX_NetworkPPPoEConfig();
};
function AntennaInfoCollection() {
}

AntennaInfoCollection.prototype = new Collection();
$.extend(true, AntennaInfoCollection.prototype, {
    commit: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.commit');
    },
    refresh: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.refresh');
    },
    createAntennaInfo: function () {
        return 1;
    },
    getAntennaInfo: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.getAntennaInfo');
        return new AntennaInfo();
    },
    removeAll: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.removeAll');
    },
    getConnectionType: function () {
        return Math.floor(Math.random() * 3);
    },
    setConnectionType: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.setConnectionType');
    },
    gotoPosition: function (position) {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.gotoPosition - position : ' + position);
    },
    storePosition: function (position) {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.storePosition - position : ' + position);
    },
    recalculate: function (position) {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.recalculate - position : ' + position);
    },
    stop: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.stop');
    },
    setLimit: function (direction) {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.setLimit - direction : ' + direction);
    },
    enableLimit: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.enableLimit');
    },
    disableLimit: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.disableLimit');
    },
    gotoDegree: function (degree) {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.gotoDegree - degree : ' + degree);
    },
    gotoSatellite: function (longitude) {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.gotoSatellite - longitude : ' + longitude);
    },
    drive: function (direction, mode, step) {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.drive - direction : ' + direction + ', mode : ' + mode + ', step : ' + step);
    },
    getMyLatitude: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.getMyLatitude');
        return -85;
    },
    getMyLongitude: function () {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.getMyLongitude');
        return 502;
    },
    setMyPosition: function (longitude, latitude) {
        console.log("[EMULATOR] " + 'AntennaInfoCollection.setMyPosition - longitude : ' + longitude + ', latitude : ' + latitude);
    }
});

function AntennaInfo() {
    var that = this;

    // From HUMAX Elec. in England
    that.antennaType = 1;

    // LNB 0, diseqc 1, SCD 2,
    that.antennaId = 0;
    that.satelliteType = 'ASTRA_2';
    that.satelliteName = 'ASTRA_2';
    that.lnbFrequency = 4294967295;
    that.lnbVoltage = 0;
    that.diseqcVersion = 0;
    that.switchInput = 1;
    that.enable22KHzTone = false;
    that.toneBurst = 0;
    that.position = 0;
    that.longitude = 0;
    that.scdBandNo0 = 0;
    that.scdBandNo1 = 1;
    that.scdBandNo2 = 2;
    that.scdBandNo3 = 3;
    that.scdBandFreq0 = 0;
    that.scdBandFreq1 = 0;
    that.scdBandFreq2 = 0;
    that.scdBandFreq3 = 0;
}

AntennaInfo.prototype = {
    SATTYPE_LIST: '[AMOS_2&3, APSTAR_2R, ARABSAT_5A, ASIASAT_3S, ASIASAT_5, ASTRA_1, ASTRA_2, ASTRA_3A&3B, ASTRA_4A, ATLANTIC_BIRD_1, ATLANTIC_BIRD_2, ATLANTIC_BIRD_3, BADR_4&6, EUROBIRD_1, EUROBIRD_3, EUROBIRD_9A, EUTELSAT_W2A, EUTELSAT_W3A, EUTELSAT_W4&W7, EUTELSAT_W6, EXPRESS_AM1, EXPRESS_AM22, HELLAS_SAT2, HISPASAT_1C&1D, HOTBIRD, INSAT_3A&4B, INTELSAT_10-02, INTELSAT_12, INTELSAT_7&10, INTELSAT_901, INTELSAT_902, INTELSAT_904, INTELSAT_906, INTELSAT_907, NILESAT_101&102, NSS_12, NSS_7, TELSTAR_12, THOR_5&6, TURKSAT_2A&3A, YAMAL_201, ARABSAT_2B, EUTELSAT_W1, EUTELSAT_W2, NSS_703, SIRIUS, TELECOM_2D, EUROBIRD_9, INTELSAT_802, THOR_3_5, INSAT_2E4A, INTELSAT_17, YAHLIVE, USERANT_1,USERANT_2,USERANT_3,USERANT_4, USERANT_5]',
    TYPE_LNB: 0,
    TYPE_DISEQC: 1,
    TYPE_SCD: 2,
    TYPE_SMATV: 3,
    TYPE_MONOBLOCK: 4,
    LNBFREQ_UNIVERSAL: -1,
    LNBVOLT_STD: 0,
    LNBVOLT_HIGH: 1,
    DISEQC_1_0: 0,
    DISEQC_1_1: 1,
    DISEQC_1_2: 2,
    DISEQC_1_3: 3,
    DISEQC_2_0: 4,
    SW_N: 0,
    SW_A: 1,
    SW_B: 2,
    SW_C: 3,
    SW_D: 4,
    TONEBURST_A: 1,
    TONEBURST_B: 2,
    // Method
    commit: function () {
    },
    refresh: function () {
    }
};

$.extend(true, NetworkInterface.prototype, {
    _handleTimer: null,
    netmask: "255.255.255.0",
    gateway: "192.168.0.1",
    dns1st: "10.0.0.5",
    dns2nd: "10.0.0.19",
    networkType: 0,
    dhcpOn: true,
    dnsAuto: true,
    _linkStatusChanged: function () {
        var that = this;
        if (that.onLinkStatusChanged) {
            that.onLinkStatusChanged(1);
        }
        clearInterval(that._handleTimer);
        that._handleTimer = null;
    },
    checkInternetConnection: function() {
        return true;
    },
    applyConfigure: function () {
        this._handleTimer = setInterval(this.onLinkStatusChanged(1), 3000);
    },
    onLinkStatusChanged: function (state) {
    },
    PPPoEStatusChanged: function (state) {
    }
});

function HMX_Output() {
    var that = this;

    that.name = "hmx_output";
    that.tvAspectRatio = "16:9";
    that.videoDisplayFormat = "Auto";
    that.supportedResolutions = new Collection();
    that.supportedResolutions.push('720p');
    that.supportedResolutions.push('1080i');
    that.supportedResolutions.push('1080p');

    that.audioDescriptionEnabled = false;
    that.subtitleEnabled = false;
    that.hardofHearing = false;
    that.curResolution = "720p";
    that.supportedTvScartFormats = ["RGB", "CVBS"];
}

function HmxOutputCollection() {
    var that = this, i;

    for (i = 0; i < 5; i += 1) {
        that.push(new HMX_Output());
    }
}

HmxOutputCollection.prototype = new Collection();

function HMX_Audios() {
    var that = this;

    that.name = "audio_hdmi";
    that.type = "audio";
    that.enabled = true;
    that.maxVolume = 50;
    that.minVolume = 0;
    that.lipSyncDelay = 0;
    that.soundMode = "Stereo";
    that.modeDigitalAudioOutput = 'PCM';
    that.codecDigitalAudioOutput = 0;
    that.transcodingEnabled = true;
}

function HMX_Audios2() {
    var that = this;

    that.name = "audio_spdif";
    that.type = "audio";
    that.enabled = true;
    that.maxVolume = 50;
    that.minVolume = 0;
    that.lipSyncDelay = 0;
    that.soundMode = "Stereo";
    that.modeDigitalAudioOutput = 'Multi Channel';
    that.codecDigitalAudioOutput = 0;
    that.transcodingEnabled = true;
}

function HmxAudioCollection() {
    var that = this;
    that.push(new HMX_Audios());
    that.push(new HMX_Audios2());
}

HmxAudioCollection.prototype = new Collection();

// Extend VideoBroadcast
VideoBroadcast.TChannelChangeError = {
    EChannelNotSupportedByTuner: 0,
    ECannotTuneToGivenTransportStream: 1,
    ETunerLockedByOtherObject: 2,
    EParentalLockOnChannel: 3,
    EEncryptedChannelKeyModuleMissing: 4,
    EUnknownChannel: 5,
    EChannelSwitchInterrupted: 6,
    ECurrentlyBeingRecorded: 7,
    ECannotResolveURI: 8,
    EInsufficientBandwidth: 9,
    EChannelCannotBeChanged: 10,
    EInsufficientResources: 11,
    ESpecifiedChannelNotFound: 12,
    EUnidentifiedError: 100
};

VideoBroadcast.TRecordingError = {
    EERROR_REC_RESOURCE_LIMITATION: 0,
    EERROR_INSUFFICIENT_STORAGE: 1,
    EERROR_TUNER_CONFLICT: 2,
    EERROR_REC_DRM_RESTRICTION: 3,
    EERROR_REC_UNKNOWN: 4,
    EERROR_TS_RESOURCE_LIMITATION: 5,
    EERROR_TS_DRM_RESTRICTION: 6,
    EERROR_TS_UNKNOWN: 7
};

$.extend(true, VideoBroadcast.prototype, {
    checkChannelChangable: function () {
        return true;
    },
    _recordingEvent: function (state, error, recordingID) {
        var fn = this.onRecordingEvent;
        if (fn) {
            fn(state, error, recordingID);
        }
    },
    getTimeShiftMode: function () {
        return this.timeShiftMode;
    },
    setTimeShiftMode: function (activate) {
        this.timeShiftMode = activate;
    },
    recordNow: function (duration) {
        'use strict';

        //this._recordingEvent(4);
        var that = this;
        var slist = gOipfEmul.scheduledRecordingCollection;
        var programme = this.programmes[0];
        var currentTime = parseInt(new Date().getTime() / 1000, 10);
        var channel = that.currentChannel;

        var scheduledRecItem = new ScheduledRecording();
        scheduledRecItem.programmeID = '';
        scheduledRecItem.name = '';
        scheduledRecItem.startTime = currentTime;

        //Math.floor((new Date().valueOf())/1000);
        scheduledRecItem.duration = duration;
        scheduledRecItem.factoryType = 0;
        scheduledRecItem.isSeries = false;
        scheduledRecItem.seriesID = '';
        scheduledRecItem.repeatDays = 0x7F;
        scheduledRecItem.channel = channel;
        if (!!programme) {
            scheduledRecItem.channel = programme.channel;
        }
        scheduledRecItem.repeatDays = 0;

        setTimeout(function () {
            var srIdx = slist.push(scheduledRecItem) - 1;
            var rsObj = gOipfEmul.recordingScheduler;

            rsObj._PVREvent(1, slist[srIdx]);

            setTimeout(function () {
                rsObj._PVREvent(3, slist[srIdx]);

                setTimeout(function () {
                    var recording = {};
                    $.extend(true, recording, slist[srIdx]);

                    recording.id = 'HPVR://mnt/hd2/My Video/_20120604_22' + Math.floor(Math.random() * 100) + '.hjm';
                    recording.state = 5;

                    //RECORDING_REC_STARTED
                    recording.error = undefined;
                    recording.isManual = true;
                    recording.thumbnail = '';
                    recording.recordingStartTime = recording.startTime;
                    recording.recordingDuration = 0;
                    recording.showType = 0;
                    recording.lastPlayPosition = 0;
                    recording.locked = false;
                    recording.repeatDays = 0x40;
                    recording.discId = "5678";
                    recording.isScheduledAsSeries = true;
                    recording.seriesID = "1377350";
                    if (!!programme) {
                        recording.genres = programme.genre;
                    }
                    var rlist = gOipfEmul.recordingScheduler.recordings;
                    var i, crList = new ScheduledRecordingCollection();
                    for (i = 0; i < rlist.length; i += 1) {
                        if (rlist[i].showType === 0) {
                            crList.push(rlist[i]);
                        }
                    }
                    if (crList.length >= 2) {
                        var conflictInfo = new Collection();
                        rsObj._PVREvent(11, recording, null, crList, null, null, null, conflictInfo);
                        //RECORDING_ERROR
                    } else {
                        rlist.push(recording);

                        rsObj._PVREvent(5, recording);

                        //RECORDING_REC_STARTED
                        setTimeout(function () {
                            if (!!programme) {
                                recording.programmeID = programme.programmeID;

                                //'dvb://1.453.4463;7b27;;8';
                                recording.name = programme.name;
                                recording.description = programme.description;
                                recording.longDescription = programme.longDescription;
                            } else {
                                recording.name = 'testtest1';
                            }

                            //'Der letzte Bulle';
                            recording.isManual = false;
                            recording.recordingDuration = 30;
                            rsObj._PVREvent(6, recording);

                            //RECORDING_REC_UPDATED
                            setTimeout(function () {
                                recording.recordingDuration = 39;
                                rsObj._PVREvent(6, recording);

                                //RECORDING_REC_UPDATED
                                setTimeout(function () {
                                    recording.recordingDuration = 59;
                                    rsObj._PVREvent(6, recording);
                                    //RECORDING_REC_UPDATED
                                }, 2000);
                            }, 1000);
                        }, 1000);
                    }
                }, 1000);
            }, 100);
        }, 100);
    },
    recordDelayed: function (startOffset, duration) {
        this.recordNow(duration);
    },
    resultPincode: function (aResult) {
        console.log("resultPincode: " + aResult);
    },
    requestThumbnail: function(aPath, lastPlayPosition) {
        return "";
    }
});

Channel.THMXType = {
    ETYPE_ALL: 100,
    ECAS_FTA: 0,
    ECAS_NAGRA: 1,
    ECAS_IRDETO: 2,
    ECAS_NDS: 3,
    ECAS_VIACCESS: 4,
    ECAS_CONAX: 5,
    ECAS_MEDIAGUARD: 6,
    ECAS_CRYPTOWORKS: 7,
    ECAS_JPBCAS: 10,
    ECAS_JPCCAS: 11
};

$.extend(true, Channel.prototype, {
    TYPE_ALL: Channel.THMXType.ETYPE_ALL,
    CAS_FTA: Channel.THMXType.ECAS_FTA,
    CAS_NAGRA: Channel.THMXType.ECAS_NAGRA,
    CAS_IRDETO: Channel.THMXType.ECAS_IRDETO,
    CAS_NDS: Channel.THMXType.ECAS_NDS,
    CAS_VIACCESS: Channel.THMXType.ECAS_VIACCESS,
    CAS_CONAX: Channel.THMXType.ECAS_CONAX,
    CAS_MEDIAGUARD: Channel.THMXType.ECAS_MEDIAGUARD,
    CAS_CRYPTOWORKS: Channel.THMXType.ECRYPTOWORKS,
    CAS_JPBCAS: Channel.THMXType.ECAS_JPBCAS,
    CAS_JPCCAS: Channel.THMXType.ECAS_JPCCAS,
    getField: function (fieldId) {
        'use strict';
        if (fieldId === 'selectable') {
            return 'false';
        }
        return '';
    }
});

// Extend ChannelConfig
$.extend(true, ChannelConfig.prototype, {
    commitScan: function () {
        console.log("[EMULATOR] " + 'ChannelConfig.prototype.commitScan ');
    },
    startTune: function () {
        console.log("[EMULATOR] " + 'ChannelConfig.prototype.startTune ');
    },
    stopTune: function () {
        console.log("[EMULATOR] " + 'ChannelConfig.prototype.stopTune ');
    },
    reloadChannelList: function () {
        console.log("[EMULATOR] " + 'ChannelConfig.prototype.reloadChannelList ');
    },
    createChannelScanOptionsObject: function () {
        return new ChannelScanOption();
    },
    startDiseqcConnection: function () {
        console.log("[EMULATOR] " + 'ChannelConfig.startDiseqcConnection');
        // setTimeout("gEngine.videoBroadcast.getChannelConfig().onChannelScan(4,0,0,0,0,0,0,0);", 1000);
        // setTimeout("gEngine.videoBroadcast.getChannelConfig().onChannelScan(4,100,0,0,0,0,1,0);", 2000);
    },
    commitDiseqcConnection: function () {
        console.log("[EMULATOR] " + 'ChannelConfig.commitDiseqcConnection');
    },
    startAntennaConnection: function () {
        console.log("[EMULATOR] " + 'ChannelConfig.startAntennaConnection');
        // setTimeout("gEngine.videoBroadcast.getChannelConfig().onChannelScan(4,0,0,0,0,0,0,0);", 1000);
        // setTimeout("gEngine.videoBroadcast.getChannelConfig().onChannelScan(4,100,0,0,0,0,0,0);", 2000);
    },
    stopAntennaConnection: function () {
        console.log("[EMULATOR] " + 'ChannelConfig.stopAntennaConnection');
    }
});

// Extend ChannelScanParameters class
$.extend(true, ChannelScanParameters.prototype, {
    addChannelScanParam: function (params) {
    }
});

LocalSystem.prototype.hmx_outputs = new HmxOutputCollection();
LocalSystem.prototype.hmx_audios = new HmxAudioCollection();

LocalSystem.prototype.frontPanel = {
    setString: function (str, a, b, c) {
    },
    setLed: function (ledId, bEnable, bExit, bClear) {
    },
    setAnimation: function (fpItemType, param1, param2, param3) {
    },
    setCurrentMediaInform: function () {
    }
};

LocalSystem.prototype.swupdate = {
    pstEventData: {
        getInfo: function (type) {
            switch (type) {
                case 'event':
                    return 2;
                case 'source_type':
                    return 2;
                case 'progress':
                    return 131312312;
                case 'progress_max':
                    return 80273222343;
                case 'current_version':
                    return 'DESNAD 0.144.02';
                case 'found_version':
                    return 'DESNAD 1.00.00';
                case 'ota_type':
                    return 3;
                case 'start_time':
                    return 1392270025675;
                case 'end_time':
                    return 1392270025675;
            }
        }
    },
    _randomTrueFalse: function () {
        return Math.floor(Math.random() * 2) == 1 ? true : false;
    },
    timer: null,
    Frequency: 11482,
    Polarization: 0,
    SymbolRate: 22000,
    TransSpec: 0,
    FecCodeRate: 0,
    PskMod: 1,
    otaSource: 1,
    verBootupHumaxOta: "STANDBYMESIBE1.00.00MESIBE0.00.00",
    startDetect: function () {
        this.onDetectEvent(this.otaSource, 1, 'FHTCP 1.00.00', 'FHTCP 1.00.01', 0);
    },
    startDetectWithType: function (otaType) {
        this.onUpdateEvent(this.pstEventData);

        if (otaType === 6) {
            this.onDetectEvent(this.otaSource, 2, 'FHTCP 1.00.00', 'FHTCP 1.00.01', 16254);
            // EEventDetectSuccess : 2
        } else {
            this.onDetectEvent(this.otaSource, 2, 'FHTCP 1.00.00', 'FHTCP 1.00.01', 0);
            // EEventDetectSuccess : 2
        }
    },
    startUpdate: function () {
        var a = this, progress = 0;
        a.timer = setInterval(function () {
            progress += 5;
            a.onUpdateProgress(a.otaSource, 2, progress, 100, 0);
            if (progress == 100) {
                clearInterval(a.timer);
            }
        }, 1000);
    },
    swupdateInit: function () {
    },
    startDownload: function () {
        var a = this, progress = 0;
        a.timer = setInterval(function () {
            progress += 5;
            a.onUpdateProgress(a.otaSource, 6, progress, 100, 0);

            //  EEventDownloadProgress : 6
            if (progress == 100) {
                clearInterval(a.timer);
            }
        }, 1000);
    },
    startSwupdateState: function (otaSpec, otaStep) {
        var a = this, progress = 0;
        a.timer = setInterval(function () {
            progress += 5;
            a.onUpdateProgress(0, 9, progress, 100, 0);

            //  EEventInstallProgress : 9
            if (progress == 100) {
                clearInterval(a.timer);
            }
        }, 200);
        // it takes shorter time than download, because it is just install.
    },
    startManualDetect: function (fr, pz, sr, tm, fec) {
        //DBG('frequency : '+fr+ ' polarization : '+pz+ ' symbolrate : '+sr+ ' transmission : '+tm+ ' fec : '+fec);
        var that = this;
        setTimeout(function () {
            that.onDetectEvent(that.otaSource, 1, '0', '1', 0);
        }, 3000);
    },
    cancelDetect: function () {
        clearInterval(this.timer);
    },
    cancelUpdate: function () {
        clearInterval(this.timer);
    },
    cancelDownload: function () {
        clearInterval(this.timer);
    },
    onUpdateProgress: function (source, type, progress, progressMax, errMsg) {
        //DBG('[onUpdateProgress] type: '+type+' progress: '+progress+' progressMax:'+progressMax+' errMsg:'+errMsg);
        return progress / progressMax * 100;
    },
    onDetectEvent: function (source, state, progress, progressMax, errMsg) {
        //DBG('[onDetectEvent] state: '+state+' progress: '+progress+' progressMax:'+progressMax+' errMsg:'+errMsg);
    },
    onUpdateEvent: function (pstEventData) {
    },
    restartSystem: function () {
        //DBG("REBOOT");
    }
};

LocalSystem.prototype.channelImportExport = {
    startChannelImport: function (otaType) {
        this.onChannelImportExport(1, 1);
        // import(1) & success (1)
    },
    startChannelExport: function () {
        this.onChannelImportExport(2, 1);
        // export(2) & success (1)
    }
};

LocalSystem.prototype.clockRecovery = {
    startClockRecovery: function () {
        this.onClockRecovery(1, 0, 0);
    }
};

var CLcnUpdatedInfo = function () {
};
CLcnUpdatedInfo.prototype = [];
CLcnUpdatedInfo.prototype.getInfo = function (index, type) {
    return this[index][type];
};

//var lcnEventType = hx.il.getEnum('LcnEventType');

LocalSystem.prototype.lcnupdate = {
    startLcnUpdate: function () {
        var that = this;
        var lcnUpdatedInfo = new CLcnUpdatedInfo();
        lcnUpdatedInfo.push({
            name: 'Germany Lcn List',
            index: 1
        });
        lcnUpdatedInfo.push({
            name: 'Brazil Lcn List',
            index: 2
        });
        lcnUpdatedInfo.push({
            name: 'Korea Lcn List',
            index: 3
        });
        lcnUpdatedInfo.push({
            name: 'Korea Lcn List2',
            index: 4
        });
        lcnUpdatedInfo.push({
            name: 'Korea Lcn List3',
            index: 5
        });
        lcnUpdatedInfo.push({
            name: 'Korea Lcn List4',
            index: 6
        });
        lcnUpdatedInfo.push({
            name: 'Korea Lcn List5',
            index: 7
        });
        lcnUpdatedInfo.push({
            name: 'Korea Lcn List6',
            index: 8
        });
        lcnUpdatedInfo.push({
            name: 'Korea Lcn List7',
            index: 9
        });
        lcnUpdatedInfo.push({
            name: 'Korea Lcn List8',
            index: 10
        });
        lcnUpdatedInfo.push({
            name: 'Korea Lcn List9',
            index: 11
        });

//        that._eventTimeout = setTimeout(function () {
//            that.onLcnUpdate(lcnEventType['STARTOK'], lcnUpdatedInfo.length, lcnUpdatedInfo, 0, null, 0, null);
//        }, 3000);
    },
    stopLcnUpdate: function () {
        var that = this;
        clearTimeout(that._eventTimeout);
    },
    saveLcnUpdate: function (searchMode, index) {
        var that = this;
        var newSvc = new CLcnUpdatedInfo();
        newSvc.push({
            name: 'arte HD',
            lcn: 101,
            ccid: 'ccid:1.1.1073.28205'
        });
        newSvc.push({
            name: 'KBS',
            lcn: 102,
            ccid: 'ccid:2.1.1073.28206'
        });
        newSvc.push({
            name: 'MBC',
            lcn: 103,
            ccid: 'ccid:3.1.1073.28221'
        });
        newSvc.push({
            name: 'BBC 1 London',
            lcn: 104,
            ccid: 'ccid:4.1.1073.28224'
        });
        newSvc.push({
            name: 'BBC 2 England',
            lcn: 105,
            ccid: 'ccid:5.1.1073.28225'
        });

        var replacedSvc = new CLcnUpdatedInfo();
        replacedSvc.push({
            name: 'replace1',
            lcn: 106,
            ccid: 'ccid:6.1.1073.28226'
        });
        replacedSvc.push({
            name: 'replace2',
            lcn: 107,
            ccid: 'ccid:7.1.1073.28227'
        });
        replacedSvc.push({
            name: 'replace3',
            lcn: 108,
            ccid: 'ccid:8.1.1073.28228'
        });
        replacedSvc.push({
            name: 'replace4',
            lcn: 109,
            ccid: 'ccid:9.1.1073.28229'
        });
        that.dbIsInfo = true;
        that._eventTimeout = setTimeout(function () {
            that.onLcnUpdate(lcnEventType['SAVEOK'], 0, null, newSvc.length, newSvc, replacedSvc.length, replacedSvc);
        }, 3000);
        that._updateEventTimeout = setTimeout(function () {
            gOipfEmul.videoBroadcastObject.channelConfig.onChannelListUpdate();
            clearTimeout(that._updateEventTimeout);
        }, 5000);
    },
    getLcnUpdateInfo: function () {
        var that = this;

        that._eventTimeout = setTimeout(function () {
            that.onLcnUpdate(lcnEventType['GETINFO'], that.dbListName, that.dbTvNum, that.dbRadioNum, that.dbLastUpdate, that.dbLastCheck, that.dbIsInfo);
        }, 500);
    },
    removeLcnUpdateInfo: function (sync) {
        var that = this;
        that.dbIsInfo = false;
    },
    dbListName: 'Astra Lcn List',
    dbTvNum: 12,
    dbRadioNum: 7,
    dbLastUpdate: '13:06:07:01',
    dbLastCheck: '13:06:07:06',
    dbIsInfo: true
};

emulColdBootDoneTick = 0;
LocalSystem.prototype.coldBootDone = {
    getColdBootDone: function () {
        var a = this;
        emulColdBootDoneTick += 1;
        setTimeout(function () {
            if (!!a.onColdBootDone) {
                a.onColdBootDone();
            }
        }, 500);
        if (emulColdBootDoneTick > 1) {
            return true;
        }
        return false;
    },
    releaseColdBootDoneListener: function () {
    }
};

LocalSystem.prototype.casactionIrEmmForcedOta = {
    setListener: function () {
    },
    releaseListener: function () {
    }
};
var CTrdConflict = function(aScenario) {
    var scenario = [
        {
            "GBR": {
                "Primary1": {
                    "Secondrary1": {
                        "Tertiaray1Note1": {

                        }
                    }
                },
                "Primary2": {
                    "SecondraryWrong": {

                    }
                },
                "PrimaryWrong": {

                }
            },
            "IMN": {
                "Secondrary1": {
                    "Tertiaray1Note1": {

                    }
                }
            }
        },
        {
            "Undefined Scenario": {

            }
        },
        {

        }
    ][aScenario || 0];
    var eDxTRDCONFLICT_UNKNOWN = 0;
    var eDxTRDCONFLICT_GET_INFO = 1;
    var eDxTRDCONFLICT_SET_INFO = 2;
    var _depth = [];
    var onTrdConflict = null;

    var fnGetTrdConflict = function() {
        var way = scenario;
        var i;
        for (i = 0; i < _depth.length; i++) {
            way = way[_depth[i]];
        }
        var keys = Object.keys(way);
        var trdInfo = keys.join(",");
        if (keys.length) {
            trdInfo += ",";
        }

        if (onTrdConflict) {
            onTrdConflict(eDxTRDCONFLICT_GET_INFO, keys.length, trdInfo);
        }
    }
    var fnSetTrdConflict = function(aDepthIndex, aIndex) {
        if (!(typeof aDepthIndex === "number" && aDepthIndex >= 0)) {
            throw "incorrect aDepthIndex: " + aDepthIndex;
        }
        var way = scenario;
        var i;
        if (_depth.length > aDepthIndex + 1) {
            _depth.length = aDepthIndex;
        }
        for (i = 0; i < _depth.length; i++) {
            way = way[_depth[i]];
        }
        var keys = Object.keys(way);
        var selectedData = keys[aIndex];
        if (!selectedData) {
            throw "incorrect aDepthIndex: " + aIndex;
        }
        _depth.push(selectedData);
    }
    var ret = {
        getTrdConflict: fnGetTrdConflict,
        setTrdConflict: fnSetTrdConflict
    }
    Object.defineProperty(ret, "onTrdConflict", {
        get: function () {
            return onTrdConflict;
        },
        set: function (aFunction) {
            onTrdConflict = aFunction;
        },
        enumerable: true,
        configurable: true
    });
    return ret;
}
LocalSystem.prototype.trdconflict = new CTrdConflict(2);

function HMXOperatorFeature() {
    this._notifyData = 0;
}
HMXOperatorFeature.prototype.setNCDNotifyData = function(aNeedToShowAgain) {
    console.log('HMXOPeratorFeature: setNCDNotifyData - value : ' + aNeedToShowAgain);
    this._notifyData = aNeedToShowAgain;
}
HMXOperatorFeature.prototype.getNCDNetworkMsg = function(aDummyValue) {
    console.log('HMXOPeratorFeature: getNCDNetworkMsg - : ' + aDummyValue);
}

// HUMAX Medialist DAE extensions (from FREESAT-G2)
var HMX_MediaContent = function () {
    var a = this;
    a.fileType = 0;
    a.fullPath = '';
    a.hasSubitem = false;
    a.date = '';
    a.duration = 0;
    a.playPosition = 0;
    a.thumbnailUrl = '';
    a.resolution = '';
    a.pictureDate = '';
    a.album = '';
    a.artist = '';
    a.watched = 0;
};

var HMX_Media = function () {
    var a = this;
    a.hmx_MediaScan = new HMX_MediaScan();
    a.hmx_MediaDoFile = new HMX_MediaDoFile();
    //a.createSearchDLNA = new HMX_DMSSearcher();
};

var HMX_MediaScan = function () {
    var a = this;
    a._indexer = 0;
    a._pageSize = 7;
    a._items = [];
    a._mediaList = gOipfEmul.mediaList;
    a._dirBrowseEvent = function () {
        var length = a._items.length;
        if (a._indexer < length) {
            //a.trigger("MediaContentSearch", a.MediaContentSearch, '0', a._items[a._indexer]);
            a.DirectorySearch('0', a._items[a._indexer]);
        } else {
            if (a._handleTimer) {
                clearInterval(a._handleTimer);
            }
        }
        a._indexer++;
    };
    /*
    a.mediaBrowse = function(path, sortOrder, pageSize, fileExt) {
    var index, a = this;
    a._path = path;
    a._sortOrder = sortOrder;
    a._pageSize = pageSize;
    a._fileExt = fileExt;
    a._indexer = 0;
    a._items = [];
    a._folderCount = 2;
    //decided by mediaVideoList.json
    a._fileCount = 10;
    //decided by mediaVideoList.json
    var length = a._folderCount + a._fileCount;
    for ( index = 0; index < length; index++) {
    //var item = a._genMediaContent(index, path, sortOrder, fileExt);
    item = gOipfEmul.mediaList;
    a._items.push(item[index]);
    }
    a._handleTimer = setInterval(a._mediaBrowseEvent, 500);
    };
    */
};

HMX_MediaScan.prototype = new DOM2EventSource();
HMX_MediaScan.prototype._mediaBrowseEvent = function () {
    var a = this;
    var length = a._items.length;
    var i;
    var items = new Collection();
    console.log('k' + (a._indexer < length));
    if (a._indexer < length) {
        var pageSizeLimitEnd = false;

        var endLimit = length;
        if (length < a._pageSize) {
            pageSizeLimitEnd = true;
            endLimit = a._pageSize;
        }

        //        for ( i = 0; i < a._pageSize; i++) {
        var between = length - a._indexer;
        var nLoop = between > 8 ? 8 : between;
        for (i = 0; i < nLoop; i++) {
            items.push(a._items[a._indexer]);
            a._indexer++;
        }

        /*
        if (pageSizeLimitEnd) {
        items.push(null);
        }
        */
        a.MediaContentSearch(items);
    } else {
        console.log(a._handleTimer);
        if (a._handleTimer) {
            clearInterval(a._handleTimer);
            a._handleTimer = null;
        }
    }
};
HMX_MediaScan.prototype._photoFilter = '.jpg;.jpeg;.bmp;.gif;.tif';
HMX_MediaScan.prototype._photoFolder = 'images/test/temp_photo/';

HMX_MediaScan.prototype.mediaBrowse = function (path, sortOrder, pageSize, fileExt) {
    var index, a = this;
    var i, len = a._mediaList.length;
    var path = path[path.length - 1] !== '/' ? path.concat('/') : path;
    var nPath = path.length;
    var item, itemPath;
    var filter = fileExt.split(';');
    var lenFilter = filter.length;
    var fnFilter = function (ext) {
        var i;
        var ret = false;
        for (i = 0; i < lenFilter; i += 1) {
            if (filter[i] === ext) {
                ret = true;
            }
        }
        return ret;
    };
    a._path = path;
    a._sortOrder = sortOrder;
    a._pageSize = pageSize;
    a._fileExt = fileExt;
    a._indexer = 0;
    a._items = [];
    a._folderCount = 2;

    //decided by mediaVideoList.json
    a._fileCount = 10;

    for (i = 0; i < len; i += 1) {
        item = a._mediaList[i];
        itemPath = item.fullPath;
        if (itemPath.substring(0, nPath) === path) {
            if (itemPath.substring(nPath).match('/') === null) {
                if (item.fileType === 0) {
                    if (!fnFilter('.' + itemPath.split('.')[length])) {
                        continue;
                    }
                    if (fileExt === a._photoFilter) {
                        itemPath.substr(0, nPath).concat(a._photoFolder).concat(itemPath.substr(nPath));
                    }
                }
                a._items.push(item);
            }
        }
    }
    if (a._handleTimer) {
        clearInterval(a._handleTimer);
    }
    a._handleTimer = setInterval(function () {
        a._mediaBrowseEvent();
    }, 500);
};

/* Freesat Original Code -> HMS is not using this method.
HMX_MediaScan.prototype._genMediaContent = function(index, path, sortOrder, fileExt) {
var a = this;
var indexer = index;
var mediaTypeEnum = {
Video : 'video',
Photo : 'photo',
Music : 'music'
};
var mediaType = '';
var item = new HMX_MediaContent();
if (fileExt.indexOf('.avi') >= 0) {
mediaType = mediaTypeEnum.Video;
ext = '.avi';
} else if (fileExt.indexOf('.jpg') >= 0) {
mediaType = mediaTypeEnum.Photo;
ext = '.jpg';
} else if (fileExt.indexOf('.mp3') >= 0) {
mediaType = mediaTypeEnum.Music;
ext = '.mp3';
}
var folderCount = a._folderCount, fileCount = a._fileCount;
if (indexer < folderCount) {
item.fileType = 1;
item.fullPath = path + '/' + 'dummy ' + indexer.toString();
a.hasSubitem = true;
} else if (indexer >= folderCount && indexer < fileCount + folderCount) {
item.fileType = 0;
item.fullPath = path + '/' + 'dummy ' + indexer.toString() + ext;
} else if (indexer >= fileCount + folderCount - 1) {
//item.fileType = 1;
//item.fullPath = '_END_OF_SCAN_';
item = null;
if (a._handlerTimer) {
clearInterval(a._handlerTimer);
}
}
if (item == null) {
return item;
}
switch (mediaType) {
case mediaTypeEnum.Video:
item.duration = Math.floor(Math.random() * 59) * 60 * 1000;
item.playPosition = Math.floor(Math.random() * item.duration);
item.thumbnailUrl = 'images/thum/video.jpg';
//path + '/' + 'dummy ' + indexer.toString() + '-thumb.jpg';
item.resolution = '';
item.pictureDate = '';
item.album = '';
item.artist = '';
item.watched = Math.floor(Math.random() * 3);
break;
case mediaTypeEnum.Photo:
item.thumbnailUrl = path + '/' + 'dummy ' + indexer.toString() + '-thumb.jpg';
item.resolution = Math.floor(Math.random() * 9999) + 'X' + Math.floor(Math.random()) * 9999;
item.pictureDate = new Date().toDateString();
break;
case mediaTypeEnum.Music:
item.duration = Math.floor(Math.random() * 59) * 10 * 1000;
item.playPosition = Math.floor(Math.random() * item.duration);
item.thumbnailUrl = path + '/' + 'dummy ' + indexer.toString() + '-thumb.jpg';
item.album = 'dummy ' + indexer.toString() + ' Album';
item.artist = 'dummy ' + indexer.toString() + ' Artist';
break;
default:
break;
}
return item;
};
*/
HMX_MediaScan.prototype.MediaContentSearch = function (content) {
    if (this.onMediaContentSearch)
        this.onMediaContentSearch(content);
};

HMX_MediaScan.prototype.onMediaContentSearch = function (content) {
};

HMX_MediaScan.prototype.fileOpDo = function (operationType, sync_mode, url_count, urls, rename, target_dir) {
    var a = this;
    var processingTime = 3000;
    if (url_count && url_count > 3) {
        processingTime = url_count * 1000;
    }
    var handle = setTimeout(function () {
        a.trigger("FileOpComplete", 1, operationType);
    }, processingTime);
    //    if (this.onFileOpComplete) {
    //        var handle = setTimeout(this.onFileOpComplete, processingTime, 1, operationType);
    //    }
};

HMX_MediaScan.prototype.dirBrowse = function (path, sortOrder, fileExt) {
    var index, a = this;
    a._path = path;
    a._sortOrder = sortOrder;
    a._fileExt = fileExt;
    a._indexer = 0;
    a._items = [];
    a._folderCount = 10;
    a._fileCount = 0;
    var length = a._folderCount + a._fileCount;
    for (index = 0; index < length; index++) {
        var item = a._genMediaContent(index, path, sortOrder, fileExt);
        a._items.push(item);
    }
    a._handleTimer = setInterval(a._dirBrowseEvent, 500);
};

HMX_MediaScan.prototype.DirectorySearch = function (returnVal, content) {
    if (this.onDirectorySearch)
        this.onDirectorySearch(returnVal, content);
};

HMX_MediaScan.prototype.onDirectorySearch = function (returnVal, content) {
};

HMX_MediaScan.prototype.isDirExist = function (url) {
    return true;
};

HMX_MediaScan.prototype.getVideoPlayPosition = function (szPath) {
    return 100;
};

HMX_MediaScan.prototype.getVideoDuration = function (szURL) {
    return 300;
};

HMX_MediaScan.prototype.getVideoWatched = function (szURL) {
    return 0;
};

HMX_MediaScan.prototype.getVideoThumbnailUrl = function (szURL) {
    return '../images/programme/ex_tv_s1.png';
};

HMX_MediaScan.prototype.getSpaceInfo = function (szURL) {
    return 4000;
};

//File Operations
//type(DELETE : 0, RENAME : 1, COPY : 2, MakeDIR : 3, CANCEL : 4), sync_mode: ? , url_count, urls, rename, target_dir
//urls format :
//fileOpDo(0, 0, 5, '/media/drive1/0.txt</media/drive1/1.txt</media/drive1/2.txt</media/drive1/3.txt</media/drive1/4.txt', '/',
// '/'); /* delete */
//fileOpDo(1, 0, 1, '/media/drive1/2.txt', '/media/drive1/rename_2', '/'); /* rename */
//fileOpDo(2, 0, 5, '/media/drive1/0.txt</media/drive1/1.txt</media/drive1/2.txt</media/drive1/3.txt</media/drive1/4.txt', '/',
// '/media/drive1/new_target/'); /* copy */
//EVENT : FileOpComplete
//hmx_MediaDoFile
var HMX_MediaDoFile = function () {
};
HMX_MediaDoFile.prototype = new DOM2EventSource();

HMX_MediaDoFile.prototype.fileOpDo = function (operationType, sync_mode, url_count, urls, rename, target_dir) {
    var a = this;
    var processingTime = 3000;
    if (url_count && url_count > 3) {
        processingTime = url_count * 1000;
    }
    var handle = setTimeout(function () {
        a.trigger("FileOpComplete", 1, operationType);
    }, processingTime);
    //    if (this.onFileOpComplete) {
    //        var handle = setTimeout(this.onFileOpComplete, processingTime, 1, operationType);
    //    }
};

HMX_MediaDoFile.prototype.FileOpComplete = function (type, state) {
    if (this.onFileOpComplete)
        this.onFileOpComplete(result, methodType);
};

//onFileOpComplete
HMX_MediaDoFile.prototype.onFileOpComplete = function (type, state) {
};

// 7.14.11 Extensions to A/V Control object / HUMAX ext subtitle
var subtitleSupportLanguages = new Collection();
subtitleSupportLanguages.push('English');
subtitleSupportLanguages.push('Korean');
subtitleSupportLanguages.push('Spanish');
subtitleSupportLanguages.push('Russian');
subtitleSupportLanguages.push('Franch');

$.extend(true, AVControl.prototype, {
    hasSubtitle: true,
    SubtitleLanguage: 'English',
    SubtitleSupportLanguages: subtitleSupportLanguages,
    SubtitleFontSize: 36,
    SubtitleTextPosition: 600,
    SubtitleSyncTime: 0
});

/***************************************************************************
* Logical storage
* Holds information of the logical storage
* @class
* @property {number} Unique ID for logical storage
* @property {boolean} available Represents availability
* @property {boolean} pvr Represents current logical storage is used for PVR
* @property {number} availableSize Total space
* @property {number} usedSize Total space
* @property {number} reservedSize Total space
* @property {number} totalSize Total space
* @property {string} formatType File system type
* @property {string} path Mount path
* @property {string} label Storage label
*/
var HMXLogicalStorage = function () {
    var a = this;
    a.id = 0;
    a.available = false;
    a.pvr = false;
    a.availableSize = 0;
    a.usedSize = 0;
    a.reservedSize = 0;
    a.totalSize = 0;
    a.formatType = "";
    a.path = "";
    a.label = "";
};

/**
* Format logical storage
* @function
* @param {string} formatType File system type
* @param {string} label Storage label
*/
HMXLogicalStorage.prototype.format = function (formatType, label) {
};

/**
* Deliver format progress information
* @event
* @param {number} status Status enum, 0: None, 1: Completed, 2: Detaching, 3: Error
* @param {number} progress Number indicating format progress
* @param {number} progressMax Maximum number for format progress
* @param {string} errMessage Error message only valid if status is 3 (Error)
*/
HMXLogicalStorage.prototype.onFormatProgress = function (status, progress, progressMax, errMessage) {
};
var HMXLogicalStorageCollection = function () {
};
HMXLogicalStorageCollection.prototype = new Collection();

/****************************************************************************
* Physical strage
* Holds information of the physical storage
* @class
* @property {number} id Unique ID for physical storage
* @property {number} interface Interface, 0:EInterfaceUnknown, 1:EInterfaceSATA, 2:EInterfaceUSB
* @property {number} type Type, 0:ETypeUnknown, 1:ETypeInternal, 2:ETypeExternal
* @property {number} kind Kind, 0:EKindUnknown, 1:EKindHDD, 2:EKindUSBMemory, 3:EKindSDMemory
* @property {number} usage Usage, 0:EUsageUnkown, 1:EUsagePVR, 2:EUsageStorage
* @property {string} uuid UUID for the physical storage
* @property {number} availableSize Total space
* @property {number} usedSize Total space
* @property {number} reservedSize Total space
* @property {number} totalSize Total space
*/
var HMXPhysicalStorage = function () {
    var a = this;
    a.id = 0;

    //a.interfaceType = 0;
    a.type = 0;
    a.kind = 0;
    a.usage = 0;
    a.pairing = 1;
    a.uuid = "";
    a.availableSize = 300 * 1024 * 1024;
    a.availablePvrSize = 300 * 1024 * 1024;
    a.usedSize = 10 * 1024 * 1024;
    a.reservedSize = 0;
    a.totalSize = 1024 * 1024 * 1024;
};
HMXPhysicalStorage.prototype.EInterfaceUnknown = 0;
HMXPhysicalStorage.prototype.EInterfaceSATA = 1;
HMXPhysicalStorage.prototype.EInterfaceUSB = 2;

// OplPhysicalStorageData::TType
HMXPhysicalStorage.prototype.ETypeUnknown = 0;
HMXPhysicalStorage.prototype.ETypeInternal = 1;
HMXPhysicalStorage.prototype.ETypeExternal = 2;

// OplPhysicalStorageData::TKind
HMXPhysicalStorage.prototype.EKindUnknown = 0;
HMXPhysicalStorage.prototype.EKindHDD = 1;
HMXPhysicalStorage.prototype.EKindUSBMemory = 2;
HMXPhysicalStorage.prototype.EKindSDMemory = 3;

// OplPhysicalStorageData::TUsage
HMXPhysicalStorage.prototype.EUsageUnkown = 0;
HMXPhysicalStorage.prototype.EUsagePVR = 1;
HMXPhysicalStorage.prototype.EUsageStorage = 2;

var HMXLogicalStorage = function () {
    var a = this;

    if (hx.config.useInternalPvr === 1) {
        a.isPVR = false;
    } else {
        a.isPVR = true;
    }
    a.id = 0;
    a.label = '';
    a.formatType = 'NTFS';
    a.path = '';
    a.isAvailable = 1;
};

/**
* Get logical storage instances
* @function
* @returns {HMXLogicalStorageCollection}
*/
HMXPhysicalStorage.prototype.driveLabel = 1;
HMXPhysicalStorage.prototype.getLogicalStorages = function () {
    var makeIsPVR = function () {
        var ret = false;
        return ret;
    };
    var makeIsAvailable = function () {
        var ret = true;
        return ret;
    };
    var ret = new HMXLogicalStorageCollection();
    var i, length = 2;
    var item = new HMXLogicalStorage();
    item.id = this.uniqueId;

    //item.path = item.label = ['/media/drive1', '/media/drvie2'][Math.floor(Math.random() * 2)];
    item.path = item.label = '/media/drive' + HMXPhysicalStorage.prototype.driveLabel;
    item.isPVR = makeIsPVR();
    item.isAvailable = makeIsAvailable();
    ret.push(item);
    HMXPhysicalStorage.prototype.driveLabel++;
    return ret;
};
HMXPhysicalStorage.prototype.uniqueId = 0;

/**
* Format all logical storages with the new label
* @function
*/
HMXPhysicalStorage.prototype.formatAll = function (label) {
    var time, i = 0;
    var a = this;
    time = setInterval(Event, 100);
    function Event() {
        if (i++ == 10) {
            clearInterval(time);
            a.onFormatProgress(2, 100, 100, 0);
        } else {
            a.onFormatProgress(0, i * 10, 100, 0);
        }
    }
};

/**
* Detach physical storage from STB device
* @function
*/
HMXPhysicalStorage.prototype.detach = function () {
    var time, i = 0;
    var a = this;
    if (a.onDetachProgress != null) {
        time = setInterval(Event, 100);
        function Event() {
            if (i++ == 10) {
                clearInterval(time);
                a.onDetachProgress(2, 100, 100, 0);
            } else {
                a.onDetachProgress(0, i * 10, 100, 0);
            }
        }
    }
};

/**
* Check disk and recover if required
* @function
*/
HMXPhysicalStorage.prototype.recovery = function () {
    var a = this;
    var time, i = 0;
    time = setInterval(Event, 500);
    function Event() {
        if (i++ == 10) {
            clearInterval(time);
            a.onRecoveryProgress(2, 100, 100, 0);
        } else {
            a.onRecoveryProgress(0, i * 10, 100, 0);
        }
    }
};

HMXPhysicalStorage.prototype.setSwitchHddForPvr = function (ucStorageId, assign) {
    var a = this;
    var ret = 0;

    return ret;
};

HMXPhysicalStorage.prototype.onFormatProgress = null;
HMXPhysicalStorage.prototype.onRecoveryProgress = null;

/**
* Deliver format progress information
* @event
* @param {number} status Status enum, 0: None, 1: Completed, 2: Detaching, 3: Error
* @param {number} progress Number indicating format progress
* @param {number} progressMax Maximum number for format progress
* @param {string} errMessage Error message only valid if status is 3 (Error)
*/
HMXLogicalStorage.prototype.onFormatProgress = function (status, progress, progressMax, errMessage) {
};

/**
* Deliver detach progress information
* @event
* @param {number} status Status enum, 0: None, 1: Completed, 2: Detaching, 3: Error
* @param {string} errMessage Error message only valid if status is 3 (Error)
*/
HMXLogicalStorage.prototype.onDetachProgress = function (status, errMessage) {
};

/**
* Deliver recovery progress information
* @event
* @param {number} status Status enum, 0: None, 1: Completed, 2: Detaching, 3: Error
* @param {number} progress Number indicating format progress
* @param {number} progressMax Maximum number for format progress
* @param {string} errMessage Error message only valid if status is 3 (Error)
*/
HMXLogicalStorage.prototype.onRecoveryProgress = function (status, progress, progressMax, errMessage) {
};
var HMXPhysicalStorageCollection = function () {
};
HMXPhysicalStorageCollection.prototype = new Collection();

/****************************************************************************
* Storage utilties
* @class
*/
var HMXStorageUtil = function () {
};

/**
* Deliever newly attached physical storage
* @event
* @param {HMXPhysicalStorage} physicalStorage Newly attached physical storage
*/
HMXStorageUtil.prototype = new DOM2EventSource();
HMXStorageUtil.prototype.onPhysicalStorageAttached = function (physicalStorage) {
};

/**
* Deliever detached physical storage
* @event
* @param {HMXPhysicalStorage} physicalStorage Detached physical storage
*/
HMXStorageUtil.prototype.onPhysicalStorageDetached = function (physicalStorage) {
};

/**
* Get physical storage instances
* @function
* @returns {HMXPhysicalStorageCollection}
*/
HMXStorageUtil.prototype.getPhysicalStorages = function () {
    var ret = new HMXPhysicalStorageCollection();
    if (hx.config.useInternalPvr === 1) {
        var ps1 = new HMXPhysicalStorage();
        ps1.id = 0;
        ps1.ismount = true;
        ps1.kind = 1;
        ps1['type'] = 1;
        ps1.label = 'Label For HDD';
        ps1['type'] = 1;
        ps1['interface'] = 1;
        ps1['usage'] = 1;
        ps1['uuid'] = '1234';
        ret.push(ps1);
        var ps2 = new HMXPhysicalStorage();
        ps2.id = 1;
        ps2['type'] = 2;
        ps2.kind = 1;
        ps2.ismount = true;
        ps2['type'] = 2;
        ps2.label = 'Label for detachable hdd';
        ps2['interface'] = 2;
        ps2['usage'] = 2;
        ps2['uuid'] = '5678';
        ret.push(ps2);
        var ps3 = new HMXPhysicalStorage();
        ps3.id = 2;
        ps3['type'] = 2;
        ps3.kind = 3;
        ps3.ismount = true;
        ps3['type'] = 2;
        ps3.label = 'MS';
        ps3['interface'] = 2;
        ps3['usage'] = 3;
        ret.push(ps3);
        var ps4 = new HMXPhysicalStorage();
        ps4.id = 3;
        ps4['type'] = 2;
        ps4.ismount = true;
        ps4.kind = 2;
        ps4['type'] = 2;
        ps4.label = 'Label for USB stick';
        ps4['interface'] = 2;
        ret.push(ps4);
    } else {
        var ps1 = new HMXPhysicalStorage();
        ps1.id = 0;
        ps1.ismount = true;
        ps1.pairing = 1;
        ps1['interface'] = 2;

        // EInterfaceUSB
        ps1['type'] = 1;

        // ETypeInternal
        ps1.kind = 1;

        // EKindHDD
        ps1['usage'] = 1;

        // EUsagePVR
        ps1['uuid'] = '1234';
        ps1.label = 'Pairing HDD';
        ret.push(ps1);

        var ps2 = new HMXPhysicalStorage();
        ps2.id = 1;
        ps2.ismount = true;
        ps2.pairing = 0;
        ps2['interface'] = 2;
        ps2['type'] = 2;
        ps2.kind = 1;
        ps2['usage'] = 3;
        ps2['uuid'] = '5678';
        ps2.label = 'External hdd';
        ret.push(ps2);

        var ps3 = new HMXPhysicalStorage();
        ps3.id = 2;
        ps3.ismount = true;
        ps3.pairing = 0;
        ps3['interface'] = 2;
        ps3['type'] = 2;
        ps3.kind = 2;
        ps3['usage'] = 3;
        ps3['uuid'] = '0000';
        ps3.label = 'USB Flash';
        ret.push(ps3);

        HMXPhysicalStorage.prototype.driveLabel = 1;
    }

    return ret;
};

/**
* Get supported file system type
* @function
* @returns {HMXPhysicalStorageCollection}
*/
HMXStorageUtil.prototype.supportedFormatTypes = function () {
    return ["ntfs", "ext3"];
};

/****************************************************************************
* CAS
* @class
* @property {}
*/
var HMXCasUtil = function () {
};
HMXCasUtil.prototype = new DOM2EventSource();
$.extend(true, HMXCasUtil.prototype, {
    _sessionHandle: 0,
    openSession: function (module, slot, type) {
        console.log("[EMULATOR] " + '[HMXCasUtil][openSession] module ' + module + ' slot ' + slot + ' type ' + type);
        gOipfEmul.processCasUiOpenSession(module, slot, type);
        this._sessionHandle += 1;
        return this._sessionHandle;
    },
    closeSession: function (module, slot, type, sessionHandle) {
        console.log("[EMULATOR] " + '[HMXCasUtil][closeSession] module ' + module + ' slot ' + slot + ' type ' + type);
        gOipfEmul.processCasUiCloseSession(module, slot);
    },
    sendUiResponse: function (module, slot, type, sessionHandle, jsonData) {
        console.log("[EMULATOR] " + '[HMXCasUtil][sendUiResponse] module ' + module + ' slot ' + slot + ' type ' + type + ' jsonData ' + jsonData);
        gOipfEmul.processCasUiSendResponse(module, slot, type, jsonData);
    },
    isSCInserted: function () {
        console.log("[EMULATOR] " + '[HMXCasUtil][isSCInserted]');
        return false;
    }
});

// HUMAX Extension of 7.9 Parental rating and parental control APIs
$.extend(true, ParentalControlManager.prototype, {
    _scPin: "0000",
    _retryCnt: 0,
    _remainTime: 600,
    isNagraSmartCardInserted: function () {
        return true;
    },
    setNargaSmartCardPIN: function (oldPin, newPin) {
        this._scPin = newPin;
        return true;
    },
    getNargaSmartCardPINRetryCount: function () {
        return this._retryCnt;
    },
    getNargaSmartCardPINRemainTime: function () {
        return this._remainTime;
    },
    verifyNargaSmartCardPIN: function (inputPin) {
        var that = this;
        if (that._scPin === inputPin) {
            that._retryCnt = 0;
        } else {
            that._retryCnt += 1;
        }
        return that._retryCnt;
    }
});

$.extend(true, ParentalRatingScheme.prototype, {
    changeThreshold: function (ratingString) {
        var ratingValue = Number(ratingString);
        var i, len;

        len = this.length;
        for (i = 0; i < len; i += 1) {
            var PRating = this.item(i);
            if (PRating.value == ratingValue) {
                this.threshold = PRating;
                return 1;
            }
        }

        return 0;
    }
});

//Bluetooth
function HBluetoothDevice() {
    var that = this;
    that.id = 1;
    that.name = 'kkk';
    that.status = 1;
    that.supportedProfile = 0;
}
;

function HBluetoothDeviceCollection() {
    var that = this, dev1 = new HBluetoothDevice(), dev2 = new HBluetoothDevice(), dev3 = new HBluetoothDevice(), dev4 = new HBluetoothDevice(), dev5 = new HBluetoothDevice(), dev6 = new HBluetoothDevice(), dev7 = new HBluetoothDevice(), dev8 = new HBluetoothDevice(), dev9 = new HBluetoothDevice();

    that.push(dev1);
    dev2.name = "go";
    dev2.status = 2;
    that.push(dev2);
    dev3.name = "go2";
    dev3.status = 0;
    that.push(dev3);
    dev4.name = "go3";
    dev4.status = 0;
    that.push(dev4);
    dev5.name = "go4";
    dev5.status = 1;
    that.push(dev5);
    dev6.name = "go5";
    dev6.status = 2;
    that.push(dev6);
    dev7.name = "go6";
    dev7.status = 2;
    that.push(dev7);
    dev8.name = "go7";
    dev8.status = 0;
    that.push(dev8);
    dev9.name = "go8";
    dev9.status = 0;
    that.push(dev9);
}
;
HBluetoothDeviceCollection.prototype = new Collection();

function HBluetoothManager() {
    var that = this;
    that.enabled = true;
}
;
HBluetoothManager.prototype = new DOM2EventSource();

HBluetoothManager.prototype.startScan = function () {
    var that = this;
    setTimeout(function () {
        that.onStateUpdate(0, 0, 0, 0);
    }, 3000);
};
HBluetoothManager.prototype.stopScan = function () {
};
HBluetoothManager.prototype.connect = function (index) {
    var that = this;
    setTimeout(function () {
        that.onStateUpdate(3, 0, 0, 0);
    }, 3000);
};
HBluetoothManager.prototype.disconnect = function (index) {
    var that = this;
    setTimeout(function () {
        that.onStateUpdate(4, 0, 0, 0);
    }, 3000);
};
HBluetoothManager.prototype.removeAt = function (index) {
    var that = this;
};
HBluetoothManager.prototype.setPincode = function (index, pincode) {
    var that = this;
    setTimeout(function () {
        that.onStateUpdate(1, 0, 0, 0);
    }, 3000);
};

HBluetoothManager.prototype.getMacAddress = function () {
    var that = this;
    return "01:02:80:82:53:4";
};

HBluetoothManager.prototype.getDeviceList = function () {
    var that = this, devCollection;
    devCollection = new HBluetoothDeviceCollection();
    return devCollection;
};

// Woon
function HWoon() {
    var that = this;
    that.server = new HWoonServer();
    that.client = new HWoonClient();
}
;

var HWoonServer = function () {
};
HWoonServer.prototype = new DOM2EventSource();
HWoonServer.prototype = {
    started: false,
    friendlyName: "UltraCatWoon",
    start: function () {
        this.started = true;
        return true;
    },
    stop: function () {
        this.started = false;
        return true;
    },
    testConnection: function () {
        this.started = true;
    }
};

var HWoonClient = function () {
};
HWoonClient.prototype = new DOM2EventSource();
HWoonClient.prototype = {
    startScan: function () {
    },
    stopScan: function () {
    },
    addFriend: function () {
    },
    removeFriend: function () {
    },
    getWoonFriendList: function () {
    }
};

// dLNA
function HDlna() {
    var that = this;
    that.dmp = new HDlnaDmp();
    that.dms = new HDlnaDms();
    that.dmr = new HDlnaDmr();
}
;

var HDlnaDms = function () {
};
HDlnaDms.prototype = new DOM2EventSource();
HDlnaDms.prototype = {
    started: false,
    friendlyName: "UltraCatDMS",
    start: function () {
        this.started = true;
        return true;
    },
    stop: function () {
        this.started = false;
        return true;
    }
};

var HDlnaDmp = function () {
};
HDlnaDmp.prototype = new DOM2EventSource();
$.extend(true, HDlnaDmp.prototype, {
    _aCds: [],
    onCDSScan: function (cds, state) {
        console.log("[EMULATOR] " + 'prototype.onCDSScan');
    },
    _makeCDSScan: function () {
        var a = this;
        var aCds = a._aCds;
        var cb = a.onCDSScan;
        var handler = [
            function () {
                return new HDlnaCds();
            },
            function () {
                var ret = new HDlnaCds();
                aCds.push(ret);
                return ret;
            },
            function () {
                return aCds.pop();
            },
            function () {
                return new HDlnaCds();
            }];
        if (!aCds) {
            aCds = new HDlnaCdsCollection(2);
            a._aCds = aCds;
        }
        if (cb) {
            var i, state;
            for (i = 2, state = 0; i <= 5; i += 1, state += 1) {
                //                a = undefined;
                (function (state) {
                    setTimeout(function () {
                        var cds = handler[state]();

                        //if (state !== 2) {
                        cb(cds, state);

                        //}
                        if (cds) {
                            a.trigger2("CDSScan", [
                                {
                                    name: 'cds',
                                    value: cds
                                }, {
                                    name: 'state',
                                    value: state
                                }]);
                        }
                    }, state * 500);
                })(state);
            }
        }
    },
    startScan: function (userSetReturnValue) {
        var a = this;
        var ret = !!userSetReturnValue && true;
        console.log("[EMULATOR] " + 'startScan');
        a._makeCDSScan();
        return ret;
    },
    stopScan: function (userSetReturnValue) {
        var ret = !!userSetReturnValue && true;
        console.log("[EMULATOR] " + 'stopScan');
        return ret;
    },
    getAvailableCdsList: function () {
        return this._aCds;
    }
});
var HDlnaCdsCollection = function (userSetCdsCount) {
    var a = this;
    var i, count, bArgument = !!userSetCdsCount;
    count = bArgument ? userSetCdsCount : 10;
    for (i = 0; i < count; i += 1) {
        a.push(new HDlnaCds());
    }
};
HDlnaCdsCollection.prototype = new Collection();
var HDlnaCds = function () {
    var a = this;
    a.udn = 'dummy udn';
    a.friendlyName = 'dLNA server';
    a.ipAddress = '100.200.100.200';
    a.onContentBrowse = function (cdsContentCollection, state) {
    };
    a.onContentSearch = function (searchId, totalCount, contents, state) {
    };
};

HDlnaCds.prototype = {
    browseContent: function (cid, reqItemCount, fileExt, sortType) {
        var a = this;
        var cb = a.onContentBrowse;
        if (cb) {
            var i, state;
            cb(null, 0);
            cb(new HDlnaCdsContentCollection(), 1);
            cb(null, 2);
            // for ( i = 3, state = 0; i <= 5; i += 1, state += 1) {
            // cb(new HDlnaCdsContentCollection(i), state);
            // }
        }
    },
    searchContent: function (cid, category, keyword, sort, index, count) {
        var a = this;
        var cb = a.onContentSearch;
        if (cb) {
            var i, state;
            cb(1, 10, new HDlnaCdsContentCollection(), 1);
        }
    }
};
var HDlnaCdsContentCollection = function () {
    var a = this;
    var list = gOipfEmul.mediaStructure;
    var i, len = list.length > 10 ? 10 : list.length;
    for (i = 0; i < len; i += 1) {
        a.push(new HDlnaCdsContent(list[i]));
    }
};

HDlnaCdsContentCollection.prototype = new Collection();

var HDlnaCdsContent = function (param) {
    var a = this;
    $.extend(true, a, HDlnaCdsContent.prototype);
    if (param) {
        $.extend(a, param);
    }
};

HDlnaCdsContent.prototype = {
    objectID: 'string',
    'class': 'string',
    restricted: false,
    objectType: 1,
    importURI: 'http://dummy.com',
    thumbnailUrl: null,
    title: 'string',
    date: 'date',
    duration: 'string',
    resolution: 'string',
    digitalCopyControl: 'string',
    genre: 'string',
    childCount: 0,
    album: 'string',
    artist: 'string'
};
var HDlnaDmr = function () {
};
HDlnaDmr.prototype = new DOM2EventSource();
$.extend(true, HDlnaDmr.prototype, {
    activated: false,
    started: false,
    friendlyName: null,
    onReqPlay: function (item, playSpeed) {
    },
    onReqStop: function () {
    },
    onReqPause: function () {
    },
    onReqSeek: function (position) {
    },
    onReqSetVolume: function (volume) {
    },
    onReqMute: function (mute) {
    },
    start: function () {
        var a = this;
        a.started = true;
        a.activated = true;
    },
    stop: function () {
        var a = this;
        a.started = false;
        a.activated = false;
    },
    setMediaChange: function (currentURI) {
    },
    isActivated: function () {
        return this.activated;
    },
    isStarted: function () {
        return this.started;
    }
});

/**
* HContentManager
*/
var HContentManager = function () {
    var a = this;
    a._list = gOipfEmul.mediaStructure;
    a._rootPath = '';
    a.state = 0;
    a.onContentBrowse = function (contents, state) {
    };
    a.onContentOperate = function (fileName, result) {
    };
};
HContentManager.prototype = new DOM2EventSource();
$.extend(true, HContentManager.prototype, {
    /**
    * private
    */
    _list: null,
    _getMediaType: function (ext) {
        var mediaType = ['3gp|3g2|mov|flv|swf|m2ts|rm|ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|mpg|vob|divx|mpeg|mpe|dat|ogm|ogg|rmvb|smi|sami|smil|srt|sub|txt|ass|ssa|psb', 'mp3|wav|f4a|m4a', 'jpg|jpeg|bmp|gif|tif'].indexOf(ext);
        return mediaType;
    },
    _1depthBrowse: function (browsePath, reqItemCount, ext, sort) {
        console.log("[EMULATOR] " + '[HContentManager._1depthBrowse]');
        var a = this;
        var list = a._list;
        var fileOnly = a._rootPath !== browsePath;
        var i, len = list.length, count = 0;
        var ret = new HContentCollection();
        a.trigger("ContentBrowse", {
            contents: null,
            state: 0
        });

        for (i = 0; i < len; i += 1) {
            if (!fileOnly && list[i].fileType === 1) {
                ret.push(list[i]);
                count += 1;
            } else {
                if (list[i].fileType === 1) {
                    continue;
                }
                if (list[i].fullPath.match(browsePath) && list[i].fullPath.substring(browsePath.length + 1).match('/') === null) {
                    if (list[i].contentType === a._getMediaType(ext)) {
                        ret.push(list[i]);
                        count += 1;
                    }
                }
            }
            if (count === reqItemCount) {
                //TODO have to define state
                a.trigger("ContentBrowse", {
                    contents: ret,
                    state: 1
                });

                // a.onContentBrowse(ret, 1);
                count = 0;
                ret.length = 0;
            }
        }
        if (count > 0) {
            a.trigger("ContentBrowse", {
                contents: ret,
                state: 1
            });
            // a.onContentBrowse(ret, 1);
        }
        a.trigger("ContentBrowse", {
            contents: null,
            state: 2
        });
        // a.onContentBrowse(null, 2);
    },
    _folderBrowse: function (browsePath, reqItemCount, mediaType, sort) {
        console.log("[EMULATOR] " + '[HContentManager._folderBrowse]');
        var a = this;
        var list = a._list;
        var path = a._rootPath + browsePath;
        var i, len = list.length, count = 0;
        var ret = new HContentCollection();
        for (i = 0; i < len; i += 1) {
            if (list[i].fileType === 1) {
                ret.push(list[i]);
                count += 1;
            } else {
                if (mediaType.indexOf(list[i].contentType)) {
                    ret.push(list[i]);
                    count += 1;
                }
            }
            if (count === reqItemCount) {
                //TODO have to define state
                a.trigger("ContentBrowse", {
                    contents: ret,
                    state: 0
                });

                // a.onContentBrowse(ret, 0);
                count = 0;
                ret.length = 0;
            }
        }
        if (count > 0) {
            a.trigger("ContentBrowse", {
                contents: ret,
                state: 0
            });
            // a.onContentBrowse(ret, 0);
        }
    },
    /**
    * public
    */
    autoCopy: function (contentType, src, dest) {
        console.log("[EMULATOR] " + '[HContentManager.autoCopy]');
        return true;
    },
    copyContent: function (src, dest) {
        console.log("[EMULATOR] " + '[HContentManager.copyContent]');
        var a = this;
        var ticket = 11;
        setTimeout(function () {
            a.onContentOperate(ticket, 1);
        }, 0);
        setTimeout(function () {
            a.onContentOperate(ticket, 2);
        }, 30 * 1000);
        return ticket;
    },
    deleteContent: function (target) {
        console.log("[EMULATOR] " + '[HContentManager.deleteContent]');
        var a = this;
        var ticket = 10;
        setTimeout(function () {
            a.onContentOperate(ticket, 4);
        }, 0);
        setTimeout(function () {
            a.onContentOperate(ticket, 5);
        }, 3 * 1000);
        return ticket;
    },
    moveContent: function (src, dest) {
        console.log("[EMULATOR] " + '[HContentManager.moveContent]');
        var a = this;
        var ticket = 12;
        setTimeout(function () {
            a.onContentOperate(ticket, 7);
        }, 0);
        setTimeout(function () {
            a.onContentOperate(ticket, 8);
        }, 3 * 1000);
        return ticket;
    },
    makeDir: function (src, dest) {
        console.log("[EMULATOR] " + '[HContentManager.makeDir]');
        return true;
    },
    removeDir: function (folderName) {
        console.log("[EMULATOR] " + '[HContentManager.removeDir]');
        return true;
    },
    hasFile: function (path) {
        console.log("[EMULATOR] " + '[HContentManager.hasFile]');
        return true;
    },
    isExist: function (path) {
        console.log("[EMULATOR] " + '[HContentManager.removeDir]');
        return true;
    },
    stopFileOperation: function () {
        console.log("[EMULATOR] " + '[HContentManager.stopFileOperation]');
        return true;
    },
    reScan: function () {
        console.log("[EMULATOR] " + '[HContentManager.stopFileOperation]');
        return true;
    },
    extensionMonitor: function () {
        console.log("[EMULATOR] " + '[HContentManager.extensionMonitor]');
        return true;
    },
    browseContent: function (browsePath, browseType, reqItemCount, ext, sort, rootPath) {
        console.log("[EMULATOR] " + '[HContentManager.browseContent]');
        var a = this;
        if (rootPath) {
            a._rootPath = rootPath;
        }

        //TODO convert ext to media type
        if (browseType === 0) {
            console.log("[EMULATOR] " + '[HContentManager.browseContent] 1depth browsing is not implement yet');
            a._1depthBrowse(browsePath, reqItemCount, ext, sort);
        } else if (browseType === 1) {
            a._folderBrowse(browsePath, reqItemCount, ext, sort);
        }
    },
    searchContent: function (path, category, keyword, count, sort) {
        var a = this;
        a._1depthBrowse('/media/drive7/Media/Music', count, IMediaItem.ExtensionList[IMediaItem.TContentType.EMusic], sort);
    },
    requestScanNotify: function (path) {
        console.log("[EMULATOR] " + '[HContentManager.requestScanNotify]');
        return true;
    }
});
var HContentCollection = function () {
};
HContentCollection.prototype = new Collection();

var HContent = function () {
    var a = this;
};

$.extend(true, Programme.prototype, {
    getField: function (fieldId) {
        'use strict';
        if (fieldId === 'seriesID') {
            return this.seriesID;
        } else if (fieldId === 'thumbnailURL') {
            return this.thumbnailURL;
        } else if (fieldId === 'person') {
            return 'Park, Min Sik;Hong, Kill DDong;One Beeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeean;Two Beeeean;';
        }  else if (fieldId === 'freeview/guidance') {
            return this.guidance;
        }  else if (fieldId === 'freeview/linkage') {
            return this.linkage;
        }

        return '';
    }
});
var HSamba = function () {
    var a = this;
    a.CONST = {
        SAMBA_SCAN_STARTED: 0,
        SAMBA_SCAN_ADD: 1,
        SAMBA_SCAN_REMOVE: 2,
        SAMBA_SCAN_COMPLETE: 3,
        SAMBA_SCAN_FAILED: 4,
        MOUNT_OK: 10,
        MOUNT_FAIL: 11,
        UNMOUNT_OK: 12,
        UNMOUNT_FAIL: 13
    };
    var server = new HSambaServer2();
    Object.defineProperty(this, "server", {
        value: server,
        writable: false
    });
};
HSamba.prototype = new DOM2EventSource();
$.extend(true, HSamba.prototype, {
    startScan: function () {
        var a = this;
        var i;
        var e = a.CONST;
        var countServer = 10;
        var execute_end = function () {
            a.onScanResult(null, a.CONST.SAMBA_SCAN_COMPLETE);
        };
        var execute = function (i, max_count) {
            setTimeout(function () {
                console.log("[EMULATOR] " + 'exec add timeout i : ' + i);
                a.trigger("ScanResult", {
                    server: new HSambaServer(i, '0.0.0.' + i, i),
                    state: a.CONST.SAMBA_SCAN_ADD
                });

                // a.onScanResult(new HSambaServer(i, '0.0.0.' + i, i), a.CONST.SAMBA_SCAN_ADD);
                i += 1;
                if (i < max_count) {
                    execute(i, max_count);
                } else {
                    execute_end();
                }
            }, 1000);
        };

        a.trigger("ScanResult", {
            server: null,
            state: 0
        });

        // a.onScanResult(null, 0);
        execute(0, countServer);
    },
    stopScan: function () {
    },
    mount: function (server) {
        var a = this;

        /*
        if (server.isMounted === false) {
        server.isMounted = true;
        } else {
        server.isMounted = false;
        }
        */
        server.isMounted = false;
        a.trigger("MountResult", {
            server: server,
            state: a.CONST.MOUNT_FAIL
        });

        // a.onMountResult(server, a.CONST.MOUNT_FAIL);
        //a.mount = function() {
        //};
        return true;
    },
    unmount: function () {
    },
    onScanResult: function () {
        console.log("[EMULATOR] " + 'onScanResult === null');
    },
    onMountResult: function () {
        console.log("[EMULATOR] " + 'onMountResult === null');
    }
});

var HSambaServer = function (name, ip, path) {
    var a = this;
    console.log("[EMULATOR] " + 'name : ' + name + ' ip : ' + ip + ' path : ' + path);
    a.serverName = name;
    a.sharedName = 'dummy Shared Name';
    a.ip = ip;
    a.isMounted = false;
    a.log = 'no log';
    a.mountPath = '/mnt/smb' + path;
    a.id = '';
    a.password = '';
};

/*
 NATIVE_GET_STRING("serverName", native_SambaServer_GetName(identifier, &RETVAL), JSP_GET_VALUE);
 NATIVE_GET_STRING("workgroup", native_SambaServer_GetWorkgroup(identifier, &RETVAL), JSP_GET_VALUE);
 NATIVE_GET_BOOLEAN("started", native_SambaServer_IsStarted(identifier, &RETVAL), JSP_GET_VALUE);

 GET_FUNCTION(start, 			obj, "", JSP_GET_VALUE_CACHE);
 GET_FUNCTION(stop, 				obj, "", JSP_GET_VALUE_CACHE);
 GET_FUNCTION(setPassword, 		obj, "sss", JSP_GET_VALUE_CACHE);
 GET_FUNCTION(isValidPassword, 	obj, "ss", JSP_GET_VALUE_CACHE);
 GET_FUNCTION(setShareFolder, 	obj, "sss", JSP_GET_VALUE_CACHE);
 GET_FUNCTION(getShareFolder, 	obj, "s", JSP_GET_VALUE_CACHE);
 GET_FUNCTION(setPublicAccess, 	obj, "sb", JSP_GET_VALUE_CACHE);
 GET_FUNCTION(getPublicAccess, 	obj, "s", JSP_GET_VALUE_CACHE);

 GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
 GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

 GET_OBJECT("users", obj, getUsers(), JSP_GET_VALUE );
 INTRINSIC_EVENT_GET(StateUpdated);

 return JSP_GET_NOTFOUND;
 */
function HSambaServer2() {
    function StringCollection() {
        this.push("admin");
    }
    StringCollection.prototype = new Collection();
    Object.defineProperties(this, {
        "started": {
            value: false,
            writable: false
        },
        "users": {
            value: new StringCollection(),
            writable: false
        }
    });
    this.serverName = "serverName property";
    this.workgroup = "workgroup property";
    this.onStateUpdated = null;
    this.STOP = 0;
    this.START = 1;
    this._tree = null;
    var self = this;
//    require(["emul/humax.ext/json/directory_structure"], function(aDirectoryStructure) {
    require(["text!emul/humax.ext/json/directory_structure.json"], function(aDirectoryStructure) {
        self._tree = JSON.parse(aDirectoryStructure);
    });
}
HSambaServer2.prototype = new DOM2EventSource();
HSambaServer2.prototype.start = function() {
    Object.defineProperty(this, "started", {
        value: true,
        writable: false
    })
    if (this.onStateUpdated) {
        this.onStateUpdate(this.START);
    }
};
HSambaServer2.prototype.stop = function() {
    Object.defineProperty(this, "started", {
        value: false,
        writable: false
    })
    if (this.onStateUpdated) {
        this.onStateUpdate(this.STOP);
    }
};
HSambaServer2.prototype.setPassword = function() {};
HSambaServer2.prototype.isValidPassword = function() {};
HSambaServer2.prototype.setShareFolder = function() {};
HSambaServer2.prototype.getShareFolder = function(aUserID) {
    var ret = null;
    for (var i = 0; i < this.users.length; i++) {
        if (aUserID === this.users[i]) {
            return Object.keys(this._tree);
        }
    }
};
HSambaServer2.prototype.setPublicAccess = function() {};
HSambaServer2.prototype.getPublicAccess = function() {};


var HDial = function () {
    var a = this;
    a.server = new HDialServer();
};
var HDialServer = function () {
    var a = this;
};
HDialServer.prototype = new DOM2EventSource();
$.extend(true, HDialServer.prototype, {
    _registeredApps: [],
    started: true,
    friendlyName: '',
    start: function () {
        var a = this;
        setTimeout(function () {
            // a.trigger("DialStatusEvent", {
            // status : 0
            // });
        }, 5000);
        setTimeout(function () {
            // a.trigger("ReqLaunch", {
            // name : 'YouTube',
            // url : 'http://www.google.com'
            // });
        }, 7000);
    },
    stop: function () {
        setTimeout(function () {
            a.trigger("StatusEvent", {
                status: 1
            });
        }, 7000);
    },
    registApplication: function (name) {
        this._registeredApps.push(name);
    },
    unregistApplication: function (name) {
        var a = this;
        var registeredApps = a._registeredApps;
        var index = registeredApps.indexOf(name);
        if (index > -1) {
            registeredApps.splice(index, 1);
        }
    },
    updateApplicationState: function () {
    }
});
var HSatip = function () {
    var a = this;
    a.server = new HSatipServer();
    a.client = new HSatipClient();
};
var HSatipServer = function () {
    var a = this;
};

function Netflix() {
    hx.log('info', '[Emul]Netflix Constructor #####');
    this._cb = null;
};
$.extend(true,Netflix.prototype,{
    contstructor: Netflix,
    registerStatusChanged: function (cb) {
        this._cb = cb;
        hx.log('info', '[Emul]Netflix:registerStatusChanged');
    },
    startWithTypeUrl: function (type, url) {
        hx.log('info', '[Emul]Netflix:startWithTypeUrl');
    },
    startWithType: function (type) {
        hx.log('info', '[Emul]Netflix:startWithType');
    },
    stopWithReason: function (reason) {
        hx.log('info', '[Emul]Netflix:stopWithReason');
    },
    notifyOverlayState: function (state) {
        hx.log('info', '[Emul]Netflix:notifyOverlayState');
    }
});

HSatipServer.prototype = new DOM2EventSource();
$.extend(true, HSatipServer.prototype, {
    started: true,
    friendlyName: '',
    start: function () {
        var a = this;
        setTimeout(function () {
        }, 5000);
        setTimeout(function () {
        }, 7000);
    },
    stop: function () {
        setTimeout(function () {
            a.trigger("StatusEvent", {
                status: 1
            });
        }, 7000);
    }
});
var HSatipClient = function () {
    var a = this;
};
HSatipClient.prototype = new DOM2EventSource();
var HAirplay = function () {
    var a = this;
};
HAirplay.prototype = new DOM2EventSource();
$.extend(true, HAirplay.prototype, {
    start: function () {
    },
    stop: function () {
    }
});

var HFtp = function () {
    var a = this;
    this.server = new HFtpServer();
};
HFtp.prototype = new DOM2EventSource();
$.extend(true, HFtp.prototype, {
    start: function () {
    },
    stop: function () {
    },
    setPassword: function () {
    },
    isValidPassword: function () {
    },
    setShareFolder: function () {
    },
    getShareFolder: function () {
    }
});

var HFtpServer = function() {
    this.serverName = "";
    this.started = false;
    this.port = 0;
    this.users = "";
}
HFtpServer.prototype = new DOM2EventSource();

HFtpServer.prototype.start = function() {};
HFtpServer.prototype.stop = function() {};
HFtpServer.prototype.setPassword = function(param1, param2, param3) {};
HFtpServer.prototype.isValidPassword = function(param1, param2) {};
HFtpServer.prototype.setShareFolder = function(param1, param2, param3) {};
HFtpServer.prototype.getShareFolder = function(param1) {};
HFtpServer.prototype.setPublicAccess = function(param1, param2) {};
HFtpServer.prototype.getPublicAccess = function(param1) {};

var HAlexaHelper = function() {
    var a = this;
}
HAlexaHelper.prototype = new DOM2EventSource();
$.extend(true, HAlexaHelper.prototype, {
    processData: function (data) {
    }
});