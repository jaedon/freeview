// filename : fw/service/HMX/emul/emulUtil.js
/* jslint sloppy:true, nomen:true, vars:true */
/**
* @author ygkim
*/
// Override
gOipfEmul.createChannelList = function (data) {
    // console.log("[EMULATOR] " + 'createChannelList');
    var that = this;

    // that.createAntennaInfoIfNeeded(satelliteName);
    that.channelList = that.channelList || new ChannelList();
    var channelList = that.channelList;
    data.service.sort(function(a,b) {
        return a.lcn - b.lcn;
    });
    $.each(data.service, function (index, s) {
        var channel = new Channel();
        channel.channelType = that.svctypeCnv[s.svctype];
        channel.idType = that.delitypeCnv[s.delitype];

        //channel.ccid = 'ccid:[' + service.svc_uid + ':' + (i + 1) + '].[' + service.onid + '].[' + service.tsid + '].[' +
        // service.sid + ']';
        channel.ccid = 'ccid:' + s.svc_uid + '.' + s.onid + '.' + s.tsid + '.' + s.svcid;
        channel.onid = s.onid;
        channel.tsid = s.tsid;
        channel.sid = s.svcid;
        channel.name = s.name;
        channel.svcSection = s.svcSection;
        channel.isHD = true;
        channel.isAD = true;
        channel.majorChannel = s.lcn;
        channel.minorChannel = 0;
        channel.locked = s.locked;
        channel.manualBlock = false;
        channel.logoURL = s.logo;
        channel.casIDs = [];
        channel.hasTeletext = true;
        channel.hidden = s.hidden;
        channel.selectable = s.selectable;
        //channel.logoURL = 'emul/images/channel/icon_channelbar' + a.channelImageList[Math.floor(Math.random() * 21)] +
        // '_on.png';
        channelList.push(channel);

        // a.antenna[satelliteName].mapChannel[i.svc_uid] = channel;
        that.channelMap[s.svc_uid] = channel;
        that.channelEventMap[s.svc_uid] = [];
    });
    // a.antenna[satelliteName].channelList = channelList;
};

gOipfEmul.getChannel = function (ccid) {
    var a = this, channel;
    $.each(a.channelMap, function (i, v) {
        if (v.ccid === ccid) {
            channel = v;
            return false;
        }
    });
    return channel;
};

gOipfEmul.createAntennalist = function (data) {
    hx.log("start of createAntennalist");
    var antennaInfos = new AntennaInfoCollection();
    antennaInfos.gotoPosition = function () {
        hx.log("Call antennaInfos.gotoPosition()");
    };
    antennaInfos.drive = function () {
        hx.log("Call antennaInfos.drive()");
    };
    antennaInfos.getAntennaInfo = function () {
        //return this[0];
        return new AntennaInfo();
    };
    antennaInfos.createAntennaInfo = function () {
    };
    antennaInfos.commit = function () {
    };
    antennaInfos.removeAll = function () {
    };
    antennaInfos.createAntennaInfo = function () {
        return 1;
    };

    $.each(data, function () {
        var antennaInfo = new AntennaInfo();
        if (typeof this.antenna_type == 'undefined')
            return;
        antennaInfo.antennaType = { 'lnb': 0, 'diseqc': 1, 'scd': 2, 'smatv': 3, 'monoblock': 4 }[this.antenna_type.toLowerCase()];
        antennaInfo.satelliteType = this.sat_type;
        antennaInfo.satelliteName = this.sat_name;

        /*
        * DBG("******************************************");
        * DBG(antennaInfo.antennaType); DBG(antennaInfo.satelliteType);
        * DBG(antennaInfo.satelliteName);
        */
        var Antinfo = this.antinfo;

        if (Antinfo != null) {
            antennaInfo.lnbFrequency = Antinfo.lnbfreq;
            antennaInfo.lnbVoltage = Antinfo.lnbvoltage;
            antennaInfo.diseqcVersion = { '1.0': 0, '1.1': 1, '1.2': 2, '1.3': 3, '2.0': 4 }[Antinfo.diseqcver];
            antennaInfo.switchInput = 1;

            //Antinfo.switchInput; // ?
            antennaInfo.enable22KHzTone = false;

            // Antinfo.22khztone;
            antennaInfo.toneBurst = Antinfo.toneburst;
            antennaInfo.position = Antinfo.motorpos;
            antennaInfo.longitude = Antinfo.longitude;
            antennaInfo.scdBandNo0 = 0;

            // Antinfo.scdBandNo0;
            antennaInfo.scdBandFreq0 = 0;

            // Antinfo.scdBandFreq0;
            antennaInfo.scdBandNo1 = 0;

            // Antinfo.scdBandNo1;
            antennaInfo.scdBandFreq1 = 0;

            // Antinfo.scdBandFreq1
            antennaInfo.commit = function () {
                hx.log("Call antennaInfo.commit()");
            };

            /*
            * DBG(antennaInfo.lnbFrequency); DBG(antennaInfo.lnbVoltage);
            * DBG(antennaInfo.diseqcVersion); DBG(antennaInfo.switchInput);
            * DBG(antennaInfo.enable22KHzTone); DBG(antennaInfo.toneBurst);
            * DBG(antennaInfo.position); DBG(antennaInfo.longitude);
            * DBG(antennaInfo.scdBandNo0); DBG(antennaInfo.scdBandFreq0);
            * DBG(antennaInfo.scdBandNo1); DBG(antennaInfo.scdBandFreq1);
            */
            antennaInfos.push(antennaInfo);
        }
    });
    LocalSystem.prototype.antennaInfoLists = antennaInfos;

    var signalInfo = new SignalInfo();
    var tuner = new Tuner();
    tuner.signalInfo = signalInfo;
    var tunerCollection = new TunerCollection();
    tunerCollection.push(tuner);
    LocalSystem.prototype.tuners = tunerCollection;
    setInterval(function () {
        signalInfo.strength = parseInt(Math.random() * 100);
        signalInfo.quality = parseInt(Math.random() * 100);
    }, 5000);
    // DBG("end of createAntennalist");
};

gOipfEmul.getProgramCollection = function (channel) {
    var that = this;
    var channelMap = that.channelMap;
    var i, svc_uid = channel.ccid.split(':')[1].split('.')[0];
    var pc = that.channelEventMap[svc_uid];
    if (pc) {
        pc.item = function (index) {
            return this[index];
        };
        return pc;
    }

    return new ProgrammeCollection();
};

gOipfEmul.processEventList = function (data) {
    var that = this;

    $.each(data.epg, function (index, value) {
        // reset the startTime per channel
        var datumTime = new Date();
        datumTime.setMinutes(0, 0, 0);
        var startTime = parseInt(datumTime.getTime() / 1000);
        if (hx.config.useBackwardEpg === true) {
            // set the start time as -2 weeks for backward epg test.
            startTime = startTime - (60 * 60 * 24 * 2);
        }

        var beforeProgrammeID = "";
        $.each(value.event, function (index, value) {
            var svcUid = value.svc_uid;
            var programme = new Programme();
            var channelEventMap = that.channelEventMap;
            var thisChannel = that.channelMap[svcUid];
            if (!thisChannel) {
                return;
            }

            programme.name = value.event_name;
            programme.longName = "";
            programme.description = value.event_text;
            programme.longDescription = value.event_text;
            programme.duration = value.duration;
            programme.channelID = (thisChannel !== undefined) ? thisChannel.ccid : svcUid;
            programme.hasRecording = false;
            programme.channel = (thisChannel !== undefined) ? thisChannel : null;
            programme.startTime = startTime;
            programme.episode = parseInt(Math.random() * 10 + 1, 10).toString();

            //            programme.episode = (Math.floor(Math.random() * 2) % 2) ? '':parseInt(Math.random() * 10 + 1,
            // 10).toString();
            programme.totalEpisodes = 20;
            programme.isHD = (Math.floor(Math.random() * 2) % 2) ? true : false;
            programme.isAD = (Math.floor(Math.random() * 2) % 2) ? true : false;
            programme.programmeID = "dvb://" + thisChannel.onid.toString(16) + '.' + thisChannel.tsid.toString(16) + '.' + thisChannel.sid.toString(16) + ';' + value.event_id + ';;' + svcUid;
            programme.genre = new StringCollection();
            programme.parentalRatings = [];
            programme.subtitleTypes = ["EVU", "DVB"];

            beforeProgrammeID = programme.programmeID;

            programme.guidance = '{"guidance":[{"type":0,"mode":255,"text":"Guidance for HD recordings - PIN protection."}]}';
            programme.linkage = '[{"channel":"ccid:1111.1.1011.11120","event":"7d1","simul":"true"},{"channel":"ccid:2.9018.4134.512","event":"bb9","simul":"false"}]';
            /*var genres = ["Unclassified", "Movie/Drama"
            , "News/Current affairs"
            , "Show/ Game show"
            , "Sports"
            , "Children's/ Youth programmes"
            , "Music/Ballet/Dance"
            , "Arts/Culture (without music)"
            , "Social/Political Issues/ Economics"
            , "Education/Science/Factual Topics"
            , "Leisure hobbies"
            , "Special Characteristics"];
            programme.genre.push(genres[Math.floor(Math.random() * genres.length)]);*/
            var genres = 'a1';
            programme.genre.push(genres);

            programme.seriesID = (value.seriesID === undefined) ? "" : value.seriesID;
            if (value.thumbnailURL) {
                programme.thumbnailURL = value.thumbnailURL;
            }

            // create event map
            that.eventMap[value.event_id] = programme;

            // create channel-event map
            if (channelEventMap[svcUid] === undefined) {
                channelEventMap[svcUid] = [programme];
            } else {
                channelEventMap[svcUid].push(programme);
            }

            startTime += value.duration + 1;

            // GroupCRID
            if (hx.config.useRfCRID === true) {
                programme.groupCRIDs = new StringCollection();
                var CRID = '1234';
                programme.groupCRIDs.push(CRID);
            }
            programme.recommendationCRID = "";
            if (value.recommendationCRID && beforeProgrammeID) {
                programme.recommendationCRID = beforeProgrammeID;
            }
        });
    });
};

//MEDIA
gOipfEmul._mediaObject = null;
gOipfEmul.processMediaList = function (json) {
    console.log("[EMULATOR] " + "processMediaList");

    //    var i, item = new HMX_MediaContent();
    var i, item = [];

    for (i in json.list) {
        item[i] = json.list[i];
    }
    this.mediaList = item;
};

/**
* new media list for new API
*/
gOipfEmul.mediaStructure = [];
gOipfEmul.loadMediaList = function (list) {
    var a = this;
    var i, len = list.length;
    var mediaList = a.mediaStructure;
    for (i = 0; i < len; i += 1) {
        mediaList.push(list[i]);
    }
};

//STORAGE
gOipfEmul._storageUtil = null;
gOipfEmul.simulateUsbAttach = function () {
    this._storageUtil.trigger('PhysicalStorageAttached', new HMXPhysicalStorage());
};
gOipfEmul.simulateUsbDetach = function () {
    this._storageUtil.trigger('PhysicalStorageDetached', new HMXPhysicalStorage());
};
gOipfEmul.simulateHddNeedFormat = function () {
    this._storageUtil.trigger('PhysicalStorageStatus', 0, 2);
};

gOipfEmul.processScheduleList = function (json) {
    console.log("[EMULATOR] " + "processScheduleList");
    var that = this;
    if (!that.recordingScheduler) {
        that.recordingScheduler = new RecordingScheduler();
    }
    var list = new ScheduledRecordingCollection();
    var scheduleList = json.meta.property.schedule.list;
    $.each(scheduleList, function () {
        var schedule = this;
        var scheduledRecording = new ScheduledRecording();
        scheduledRecording.startPadding = schedule.record.startpadding;
        scheduledRecording.endPadding = schedule.record.endpadding;
        scheduledRecording.repeatDays = 0;
        scheduledRecording.name = schedule.name;
        scheduledRecording.longName = '';
        scheduledRecording.description = '';
        scheduledRecording.longDescription = '';
        scheduledRecording.startTime = schedule.starttime;
        scheduledRecording.duration = schedule.duration;
        scheduledRecording.channel = that.channelMap[schedule.svcuid];
        if (!scheduledRecording.channel) {
            return;
        }
        if (schedule.eventbase) {
            scheduledRecording.programmeID = "dvb://" + scheduledRecording.channel.onid.toString(16) + '.' + scheduledRecording.channel.tsid.toString(16) + '.' + scheduledRecording.channel.sid.toString(16) + ';' + schedule.eventid + ';;' + schedule.svcuid;
        }
        scheduledRecording.programmeIDType = ScheduledRecording.TProgrammeIDType.EID_TVA_CRID;
        scheduledRecording.episode = 0;
        scheduledRecording.totalEpisodes = 0;
        scheduledRecording.isScheduledAsSeries = schedule.record.seriesrec;
        scheduledRecording.seriesID = schedule.record.seriesid;
        scheduledRecording.parentalRatings = [];
        scheduledRecording.willBeDeleted = !!Math.round(Math.random(Date.now()));
        scheduledRecording.groupCRIDs = schedule.groupCRIDs;

        switch (schedule.action) {
            case 'record':
                scheduledRecording.factoryType = 0;
                break;
            case 'watch':
                scheduledRecording.factoryType = 1;
                break;
            case 'hls':
                scheduledRecording.factoryType = 4;
                break;
        }
        list.push(scheduledRecording);
    });

    that.scheduledRecordingCollection = list;
    //$.merge(that.recordingScheduler.recordings, list);
};

gOipfEmul.processRecList = function (json) {
    var that = this;

    if (!that.recordingScheduler) {
        that.recordingScheduler = new RecordingScheduler();
    }

    var list = that.recordingScheduler.recordings, recordingList = json.meta.property.reclist['scaned rec items'], logoTbl = {
        7222: 'images/test/thum/ibc_temp/CHLogo/02_ZDF.png',
        238: 'images/test/thum/ibc_temp/CHLogo/04_SAT1.png',
        1111: 'images/test/thum/ibc_temp/CHLogo/07_kabeleins.png',
        284: 'images/test/thum/ibc_temp/CHLogo/08_ArteHD.png'
    };
    $.each(recordingList, function () {
        var rec = this, recording = new Recording();

        recording.startPadding = 0;
        recording.endPadding = 0;
        recording.repeatDays = 0;
        recording.name = rec['display name'];
        recording.longName = '';
        recording.description = rec.event.text;
        recording.longDescription = rec.ext_event_text;
        recording.startTime = rec['start time'];
        recording.duration = rec['org duration'];
        recording.channel = that.channelMap[rec['svc UId']];
        //recording.channel.logoURL = logoTbl[rec['svc UId']];
        recording.programmeID = rec['programmeID'];
        recording.programmeIDType = ScheduledRecording.TProgrammeIDType.EID_TVA_CRID;
        recording.episode = 0;
        recording.totalEpisodes = 0;
        recording.parentalRatings = [{ value: 19 }];
        recording.groupCRIDs = rec.groupCRIDs;

        var recStatus = rec.status;
        switch (recStatus) {
            case "rec_sts_start":
            case "rec_sts_recording":
                recording.state = Recording.TState.ERECORDING_REC_STARTED;
                break;

            case "rec_sts_recorded":
                recording.state = Recording.TState.ERECORDING_REC_COMPLETED;
                break;

            case "rec_sts_incompleted":
                recording.state = Recording.TState.ERECORDING_REC_PARTIALLY_COMPLETED;
                break;

            default:
                recording.state = Recording.TState.ERECORDING_ERROR;
                break;
        }

        var recEndReason = rec['end reason'];
        switch (recEndReason) {
            case "rec_end_hddfull":
                recording.error = Recording.TError.EERROR_INSUFFICIENT_STORAGE;
                break;

            case "rec_end_conflict_tp":
                recording.error = Recording.TError.EERROR_TUNER_CONFLICT;
                break;

            case "rec_end_drm":
                recording.error = Recording.TError.EERROR_REC_DRM_RESTRICTION;
                break;

            case "rec_end_maxnum":
            case "rec_end_maxrecordlistnum":
            case "rec_end_nohdd":
            case "rec_end_nousbhdd":
            case "rec_end_same_svc_recording":
                recording.error = Recording.TError.EERROR_REC_RESOURCE_LIMITATION;
                break;

            case "rec_end_nofail":
                recording.error = -1;

                break;

            default:
                recording.error = Recording.TError.EERROR_REC_UNKNOWN;
                break;
        }
        recording.subtitleTypes = ["EBU", "DVB"];
        recording.hasTeletext = true;
        recording.id = rec.url;
        recording.isManual = !rec.eventbased;
        recording.isManualLocked = rec.userlock;
        recording.doNotDelete = rec.autodeleted;
        recording.saveDays = 0;
        recording.saveEpisodes = 0;
        recording.blocked = rec.blocked;
        if (rec.played) {
            recording.showType = Recording.TShowType.ERECORDING_RERUN;
        } else if (recording.state === Recording.TState.ERECORDING_REC_STARTED) {
            recording.showType = Recording.TShowType.ERECORDING_LIVE;
        } else {
            recording.showType = Recording.TShowType.ERECORDING_FIRSTRUN;
        }
        recording.subtitles = true;
        recording.subtitleLanguages = {};

        // readonly StringCollection
        if (rec['svc video type'] === 'hd') {
            recording.isHD = true;
        } else {
            recording.isHD = false;
        }
        recording.isHD = true;
        recording.isAD = true;
        recording.isWidescreen = true;
        recording.audioType = 4;

        // 1: Mono, 2:: Stereo, 3: Multi-channel
        recording.isMultilingual = true;
        recording.audioLanguages = {};

        // readonly StringCollection
        recording.genres = new StringCollection();
        var genres = ["Unclassified", "Entertainment", "Sport", "Drama"];

        recording.genres.push(genres[Math.floor(Math.random() * genres.length)]);

        // readonly StringCollection
        recording.recordingStartTime = rec['start time'];
        recording.recordingDuration = rec.duration;
        recording.bookmarks = {};

        // readonly BookmarkCollection
        if (rec.locked) {
            recording.locked = true;
        } else {
            recording.locked = false;
        }
        recording.discId = rec.discId;

        // Humax extention
        recording.isScheduledAsSeries = rec.seriesrec;
        recording.seriesID = rec.event.seriesid;
        recording.lastPlayPosition = rec['last view time'];
        recording.thumbnail = rec.thumbnail;
        recording.isScrambled = false;
        recording.willBeDeleted = (Math.floor(Math.random() * 2) % 2) ? true : false;
        recording.doNotDelete = false;
        switch (rec.copyControl) {
            case 'copy ok':
                recording.copyControl = 0;
                break;
            case 'no further copy':
                recording.copyControl = 1;
                break;
            case 'copy once':
                recording.copyControl = 2;
                break;
            case 'copy prohibited':
                recording.copyControl = 3;
                break;
        }

        // getField
        //recording.extEventInfo
        //recording.guidance = '{"guidance":[{"type":0,"mode":255,"text":"Guidance for HD recordings - PIN protection."}]}';

        list.push(recording);
    });
};

gOipfEmul.createCasUiData = function (uiData) {
    var that = this;
    that.casUiData = uiData;
};
gOipfEmul.processCasUiOpenSession = function (module, slot, type) {
    var that = this;
    var test1 = { "uitype": "menulist", "uidata": { "attribute": ["bottom", "subtitle", "title"], "title": ["CI%2B%201.3%20Reference%20Module"], "subtitle": ["Generic%20Status%20Reporting"], "bottom": ["Press%20Ok%20-%20Exit%20to%20quit"], "focusindex": 0, "startindex": 0, "listitem": [{ "index": 0, "type": "str", "stringlength": 26, "string": ["Authentication%20in%20progress"] }] }, "castype": "ci", "slotnumber": 1 };
    var test2 = { "uitype": "menulist", "uidata": { "attribute": ["bottom", "subtitle", "title"], "title": ["CI%2B%201.3%20Reference%20Module"], "subtitle": ["Generic%20Error%20Reporting"], "bottom": ["Press%20Ok%20-%20Exit%20to%20quit"], "focusindex": 0, "startindex": 0, "listitem": [{ "index": 0, "type": "str", "stringlength": 22, "string": ["Authentication%20failure"] }, { "index": 1, "type": "str", "stringlength": 24, "string": ["Certificate%20syntax%20error"] }, { "index": 2, "type": "str", "stringlength": 35, "string": ["Processing%20limited%20to%20clear%20content"] }] }, "castype": "ci", "slotnumber": 1 };
    switch (type) {
        case 3:
            that.casUiCurrentPos = 'root';

            //that.casUiCurrentPos = 'native_confirm_newstring2';
            setTimeout(function () {
                that.casUtil.trigger('CasUiEvent', module, slot, type, 3, 5, JSON.stringify(that.casUiData[that.casUiCurrentPos]));
                //that.casUtil.trigger('CasUiEvent', module, slot, 5, JSON.stringify(that.casUiData[that.casUiCurrentPos]));
                //that.casUtil.trigger('CasUiEvent', module, slot, type, JSON.stringify(test1));
                //that.casUtil.trigger('CasUiEvent', module, slot, type, JSON.stringify(test2));
            }, '2500');
            break;

        case 4:
            that.casUiCurrentPos = 'root';
            setTimeout(function () {
                that.casUtil.trigger('CasUiEvent', module, slot, type, 3, 5, JSON.stringify(that.casUiData[that.casUiCurrentPos]));
            }, '500');
            break;

        case 5:
            break;
    }
};
gOipfEmul.processCasUiCloseSession = function (module, slot, type) {
    var that = this;
    switch (type) {
        case 3:
            that.casUiCurrentPos = 'quit';
            break;

        case 4:
            that.casUiCurrentPos = 'quit';
            break;

        case 5:
            break;
    }
};
gOipfEmul.processCasUiSendResponse = function (module, slot, type, jsonData) {
    var that = this, inputJson = JSON.parse(jsonData);
    switch (type) {
        case 5:

        case 3:
            if (inputJson.inputtype !== undefined) {
                switch (inputJson.inputtype) {
                    case 'number':
                        if (inputJson.inputdata) {
                            console.log("[EMULATOR] " + "inputJson.inputdata.number " + inputJson.inputdata.number);
                            if (inputJson.inputdata.numberstring === "1234") {
                                that.casUiCurrentPos = 'pin_success';
                            } else {
                                that.casUiCurrentPos = 'pin_invalid';
                            }
                        }
                        break;
                    case 'etc':
                        if (inputJson.inputdata) {
                            if (inputJson.inputdata.etctype === 'previous') {
                                if (that.casUiCurrentPos === 'root') {
                                    that.casUiCurrentPos = 'quit';
                                } else {
                                    that.casUiCurrentPos = 'root';
                                }
                            }
                        }
                        break;
                    case 'selection':
                        if (inputJson.inputdata) {
                            that.casUiCurrentPos = 'node_' + inputJson.inputdata.itemidx;
                        }
                        break;
                }
            }

            setTimeout(function () {
                that.casUtil.trigger('CasUiEvent', module, slot, 3, 3, 5, JSON.stringify(that.casUiData[that.casUiCurrentPos]));
            }, '500');
            break;

        case 4:
            break;
    }
};

// Extend
$.extend(true, gOipfEmul, {
    channelMap: {},
    eventMap: {},
    channelEventMap: {},
    recordingScheduler: null,
    scheduledRecordingCollection: null,
    casUtil: null,
    casUiData: [],
    casUiCurrentPos: 'root',
    constraint: {}
});

gOipfEmul.createAVComponentCollection = function (data) {
    var that = this, avComponentCollection = new AVComponentCollection(), activeComponents = [];

    $.each(data, function (i, obj) {
        var avComponent = new AVComponent(), typeString, type, p;

        for (p in obj) {
            if (obj.hasOwnProperty(p)) {
                avComponent[p] = obj[p];
            }
        }
        avComponentCollection.push(avComponent);

        for (typeString in AVControl.TComponentType) {
            type = AVControl.TComponentType[typeString];
            if (!activeComponents[type] && type === avComponent.type) {
                activeComponents[type] = new AVComponentCollection();
                activeComponents[type].push(obj);
            }
        }
    });
    that._activeAVComponents = activeComponents;
    that._avComponents = avComponentCollection;
};

gOipfEmul.getAVComponentCollection = function (componentType) {
    var that = this;
    var components = that._avComponents;
    var avComponents = new AVComponentCollection();
    $.each(components, function (i, component) {
        if (component.type === componentType) {
            avComponents.push(component);
        }
    });
    return avComponents;
};

gOipfEmul.getActiveAVComponentCollection = function (componentType) {
    var that = this;
    var components = that._activeAVComponents;
    activeComponents = components[componentType];
    if (!activeComponents) {
        activeComponents = new AVComponentCollection();
    }
    return activeComponents;
};

gOipfEmul.getDefaultParentalRatingScheme = function () {
    var i, that = this;
    var defaultParentalRatingScheme = new ParentalRatingScheme();
    that.defaultParentalRatingSchemeCollection = new ParentalRatingSchemeCollection();

    that.ParentalRatingThreshold = new ParentalRating();
    that.ParentalRatingThreshold.name = '4';
    that.ParentalRatingThreshold.scheme = "dvb-si";
    that.ParentalRatingThreshold.value = 4;

    // this is DVB value, + 3 is actual display value.
    that.ParentalRatingThreshold.labels = 1;
    that.ParentalRatingThreshold.region = "KR";

    defaultParentalRatingScheme.name = "dvb-si";
    defaultParentalRatingScheme.threshold = that.ParentalRatingThreshold;

    // In case of View All
    var TmpParentalRating = new ParentalRating;
    TmpParentalRating.name = 'View All';
    TmpParentalRating.scheme = "dvb-si";
    TmpParentalRating.value = 0;

    // this is DVB value, + 3 is actual display value.
    TmpParentalRating.labels = 1;
    TmpParentalRating.region = "KR";
    defaultParentalRatingScheme.push(TmpParentalRating);

    // In case of 7
    var TmpParentalRating1 = new ParentalRating;
    TmpParentalRating1.name = '7';
    TmpParentalRating1.scheme = "dvb-si";
    TmpParentalRating1.value = 4;

    // this is DVB value, + 3 is actual display value.
    TmpParentalRating1.labels = 1;
    TmpParentalRating1.region = "KR";
    defaultParentalRatingScheme.push(TmpParentalRating1);

    // In case of 12
    var TmpParentalRating2 = new ParentalRating;
    TmpParentalRating2.name = '12';
    TmpParentalRating2.scheme = "dvb-si";
    TmpParentalRating2.value = 9;

    // this is DVB value, + 3 is actual display value.
    TmpParentalRating2.labels = 1;
    TmpParentalRating2.region = "KR";
    defaultParentalRatingScheme.push(TmpParentalRating2);

    // In case of 15
    var TmpParentalRating3 = new ParentalRating;
    TmpParentalRating3.name = '15';
    TmpParentalRating3.scheme = "dvb-si";
    TmpParentalRating3.value = 12;

    // this is DVB value, + 3 is actual display value.
    TmpParentalRating3.labels = 1;
    TmpParentalRating3.region = "KR";
    defaultParentalRatingScheme.push(TmpParentalRating3);

    // In case of 18
    var TmpParentalRating4 = new ParentalRating;
    TmpParentalRating4.name = '18';
    TmpParentalRating4.scheme = "dvb-si";
    TmpParentalRating4.value = 15;

    // this is DVB value, + 3 is actual display value.
    TmpParentalRating4.labels = 1;
    TmpParentalRating4.region = "KR";
    defaultParentalRatingScheme.push(TmpParentalRating4);

    // In case of Lock All
    var TmpParentalRating5 = new ParentalRating;
    TmpParentalRating5.name = 'Lock All';
    TmpParentalRating5.scheme = "dvb-si";
    TmpParentalRating5.value = 127;

    // this is DVB value, + 3 is actual display value.
    TmpParentalRating5.labels = 1;
    TmpParentalRating5.region = "KR";
    defaultParentalRatingScheme.push(TmpParentalRating5);
    that.defaultParentalRatingSchemeCollection.push(defaultParentalRatingScheme);

    return that.defaultParentalRatingSchemeCollection;
};

gOipfEmul.simulateNetworkAvail = function (available) {
    var instances = NetworkInterface.prototype.instances, i, max, netIntf;
    for (i = 0, max = instances.length; i < max; i++) {
        netIntf = instances[i];
        netIntf.enabled = available;
        netIntf.trigger2('LinkStatusChanged', [{ name: 'linkStatus', value: available ? 1 : 0 }]);
    }
};
//# sourceMappingURL=emulUtil.js.map
