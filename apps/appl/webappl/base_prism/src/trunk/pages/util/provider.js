/// <reference path="../../def/sugar.d.ts" />
// file name : pages/util/provider.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'model/model'], function (require, exports, __MODEL__) {
    var mProvider;
    (function (mProvider) {
        (function (EOndemandAvailable) {
            EOndemandAvailable[EOndemandAvailable["ENotAvailable"] = 0] = "ENotAvailable";
            EOndemandAvailable[EOndemandAvailable["EComingSoon"] = 1] = "EComingSoon";
            EOndemandAvailable[EOndemandAvailable["EAvailable"] = 2] = "EAvailable";
        })(mProvider.EOndemandAvailable || (mProvider.EOndemandAvailable = {}));
        var EOndemandAvailable = mProvider.EOndemandAvailable;
        var CMetadataProvider = (function () {
            function CMetadataProvider(aForExtendedClass, name, thumbnail) {
                this.DEFAULT_THUMB_WIDTH = 123;
                this.DEFAULT_LOGO_WIDTH = 50;
                this.name = name;
                this.thumbnailURL = thumbnail;
            }
            CMetadataProvider.prototype.setType = function (type) {
                var that = this;
                that.type = type;
            };
            CMetadataProvider.prototype.getName = function () {
                return this.name || '';
            };
            CMetadataProvider.prototype.getThumbnail = function (param) {
                return this.thumbnailURL;
            };
            CMetadataProvider.prototype.getChannelLogo = function (param) {
                return this.logoURL;
            };
            CMetadataProvider.prototype.getPlayerLogo = function (updateCb) {
                return '';
            };
            CMetadataProvider.prototype.getContentOwningLogo = function (param) {
                return '';
            };
            CMetadataProvider.prototype.setThumbnail = function (aUrl) {
                this.thumbnailURL = aUrl;
            };
            CMetadataProvider.prototype.getType = function () {
                return null;
            };
            CMetadataProvider.prototype.getStartTime = function () {
                return 0;
            };
            CMetadataProvider.prototype.getEndTime = function () {
                return 0;
            };
            CMetadataProvider.prototype.getDuration = function () {
                return 0;
            };
            CMetadataProvider.prototype.getPercentage = function () {
                return 0;
            };
            CMetadataProvider.prototype.getMediaType = function () {
                return '';
            };
            CMetadataProvider.prototype.hasSchedule = function () {
                return false;
            };
            CMetadataProvider.prototype.hasReminder = function () {
                return false;
            };
            CMetadataProvider.prototype.isRecorded = function () {
                return false;
            };
            CMetadataProvider.prototype.isPlayAvailable = function () {
                return false;
            };
            CMetadataProvider.prototype.isDummy = function () {
                return !!this.dummy;
            };
            CMetadataProvider.prototype.hasSeriesSchedule = function () {
                return false;
            };
            CMetadataProvider.prototype.getUserID = function () {
                return -1;
            };
            CMetadataProvider.prototype.watchNow = function (aCallback) {
                return false;
            };
            CMetadataProvider.prototype.getChannelNum = function () {
                return null;
            };
            CMetadataProvider.prototype.getAddition = function () {
                return '';
            };
            CMetadataProvider.prototype.isHd = function () {
                return false;
            };
            CMetadataProvider.prototype.getPluginObject = function () {
                return this.pluginObject;
            };
            return CMetadataProvider;
        })();
        mProvider.CMetadataProvider = CMetadataProvider;
        var CProgrammeProvider = (function (_super) {
            __extends(CProgrammeProvider, _super);
            function CProgrammeProvider(aParam) {
                _super.call(this, null);
                if (!aParam) {
                    hx.log('error', '[CProgrammeProvider] cannot create a new prograrmme provider coz (!aParam)');
                    return;
                }
                var that = this;
                var oipfData = aParam.oipfData || null;
                var mdsData = aParam.mdsData || null;
                var channel = aParam.channel || null;
                if (oipfData) {
                    that.pluginObject = oipfData;
                    that.name = oipfData.name;
                    that.programmeID = oipfData.programmeID;
                    that.genre = oipfData.genre;
                    that.startTime = oipfData.startTime;
                    that.duration = oipfData.duration;
                    that.endTime = that.startTime + that.duration;
                    that.parentalRating = oipfData.parentalRating;
                    that.isHD = oipfData.isHD;
                    that.locked = oipfData.locked;
                    that.shortDescription = oipfData.description;
                    that.longDescription = oipfData.longDescription;
                    that.channel = oipfData.channel;
                    that.channelType = that.channel ? that.channel.channelType : null;
                    that.ccid = that.channel ? that.channel.ccid : null;
                    that.groupCRIDs = oipfData.groupCRIDs;
                    that.guidance = oipfData.getField("freeview/guidance") || null;
                    if (that.guidance) {
                        that.guidance = JSON.parse(that.guidance).guidance[0];
                    }
                    that.isAD = oipfData.getField ? oipfData.getField("freeview/hasAudioDescription") : null;
                    that.programmeCRID = oipfData.programmeCRID;
                    that.logoURL = '';
                }
                if (channel) {
                    that.channel = channel;
                }
                if (mdsData) {
                    that.setMdsData(aParam);
                }
            }
            CProgrammeProvider.prototype.hasRecording = function () {
                var that = this;
                if (that.pluginObject) {
                    hx.log('error', '[hasRecording] this is not Backward!!');
                    return false;
                } // only for backward
                if (!that.getProgrammeID()) {
                    hx.log('debug', '[hasRecording] this is not EBR!!');
                    return false;
                }
                if (that.recordingIdx === undefined) {
                    var recordingList = hx.svc.RECORDINGS.getRecordingList();
                    if (recordingList && recordingList.length) {
                        that.recordingIdx = recordingList.indexOf(that);
                    }
                    else {
                        hx.log('debug', '[hasRecording] recordingList is not prepared yet');
                    }
                }
                return !!(that.recordingIdx >= 0);
            };
            CProgrammeProvider.prototype.getMdsData = function () {
                return this.mdsRaw ? {
                    mdsData: this.mdsRaw,
                    launchLocation: this.lloc
                } : null;
            };
            CProgrammeProvider.prototype.updateMdsData = function (aTargetEl, aData) {
                var that = this;
                if (!that.mdsRaw) {
                    that.mdsRaw = {};
                }
                if (aTargetEl && aData) {
                    that.mdsRaw[aTargetEl] = aData;
                }
                switch (aTargetEl) {
                    case 'ondemand':
                        var ondemandData = hx.svc.IP_EPG.parse.element.ondemand(aData);
                        if (ondemandData) {
                            var params = null;
                            if (ondemandData.serviceIDRef) {
                                that.serviceIDRef = ondemandData.serviceIDRef;
                            }
                            if (ondemandData.startOfAvailability) {
                                that.availableStartTime = ondemandData.startOfAvailability;
                            }
                            if (ondemandData.endOfAvailability) {
                                that.availableEndTime = ondemandData.endOfAvailability;
                            }
                            if (ondemandData.publishedDuration) {
                                that.ondemandDuration = ondemandData.publishedDuration;
                            }
                            if (ondemandData.fEpgAvailabilityCS) {
                                that.fEpgAvailabilityCS = ondemandData.fEpgAvailabilityCS;
                            }
                            if (ondemandData.mediaAvailabilityCS) {
                                that.mediaAvailabilityCS = ondemandData.mediaAvailabilityCS;
                            }
                            if (!that.pluginObject) {
                                if (ondemandData.isHD) {
                                    that.isHD = ondemandData.isHD;
                                }
                                if (ondemandData.audioPurpose) {
                                    that.isAD = ondemandData.audioPurpose;
                                }
                            }
                            if (ondemandData.programURL) {
                                params = hx.svc.SETTING_UTIL.getNetworkId().map(function (nid) {
                                    return 'nids[]=' + nid;
                                });
                                params.push('lloc=' + that.lloc);
                                that.deeplinkUrl = [
                                    ondemandData.programURL,
                                    params.join('&')
                                ].join(ondemandData.programURL.indexOf('?') !== -1 ? '&' : '?');
                            }
                            if (ondemandData.auxiliaryURL) {
                                params = hx.svc.SETTING_UTIL.getNetworkId().map(function (nid) {
                                    return 'nids[]=' + nid;
                                });
                                params.push('lloc=' + that.lloc);
                                that.templateAitUrl = [
                                    ondemandData.auxiliaryURL,
                                    params.join('&')
                                ].join(ondemandData.auxiliaryURL.indexOf('?') !== -1 ? '&' : '?');
                            }
                        }
                        break;
                    case 'schedule':
                        var scheduleData = hx.svc.IP_EPG.parse.element.scheduleEvent(aData);
                        if (scheduleData) {
                            if (!that.startTime) {
                                that.startTime = scheduleData.startTime.valueOf() / 1000;
                            }
                            if (!that.duration) {
                                that.duration = scheduleData.duration;
                            }
                            if (!that.endTime) {
                                that.endTime = that.startTime + that.duration;
                            }
                            if (scheduleData.programURL) {
                                that.mdsProgrammeID = scheduleData.programURL;
                            }
                            if (!that.programmeCRID && scheduleData.programmeCrid) {
                                that.programmeCRID = scheduleData.programmeCrid.split('crid://')[1].toUpperCase();
                            }
                            if (!that.groupCRIDs && scheduleData.seriesCrid) {
                                that.groupCRIDs = [scheduleData.seriesCrid.split('crid://')[1].toUpperCase()];
                            }
                        }
                        break;
                    case 'program':
                        var programData = hx.svc.IP_EPG.parse.element.programInformation(aData);
                        if (programData) {
                            if (programData.title) {
                                if (programData.title.main) {
                                    that.name = that.name || programData.title.main;
                                }
                                if (programData.title.secondary) {
                                    that.shortDescription = that.secondary = programData.title.secondary;
                                }
                            }
                            if (programData.synopsis) {
                                that.longDescription = programData.synopsis;
                            }
                            if (programData.icon) {
                                if (programData.icon.url) {
                                    that.requestThumbUrl = programData.icon.url;
                                }
                                if (programData.icon.type) {
                                    that.requestImgContentType = programData.icon.type;
                                }
                            }
                            if (programData.crid) {
                                that.programmeCridMds = programData.crid;
                            }
                            if (!that.pluginObject) {
                                if (programData.genre) {
                                    that.genre = (that.genre && that.genre.length) ? that.genre : programData.genre;
                                }
                                if (programData.parentalGuidance && programData.parentalGuidance.explanatoryText) {
                                    that.guidance = that.guidance || {
                                        text: programData.parentalGuidance.explanatoryText
                                    };
                                }
                                if (programData.parentalGuidance && programData.parentalGuidance.isGuidance) {
                                    that.isGuidance = that.isGuidance || programData.parentalGuidance.isGuidance;
                                }
                            }
                            that.index = programData.index;
                        }
                        break;
                    default:
                        break;
                }
            };
            CProgrammeProvider.prototype.setMdsData = function (aParam) {
                if (!aParam || !aParam.mdsData) {
                    hx.log('error', '[CProgrammeProvider][setMdsData] (!aParam || !aParam.mdsData)');
                    return;
                }
                // referenced B2C spec(1.4) - A.2 Event Metadata
                var that = this;
                var mdsData = aParam.mdsData;
                that.lloc = aParam.launchLocation || 'epg';
                if (mdsData.ondemand) {
                    that.updateMdsData('ondemand', mdsData.ondemand);
                }
                if (mdsData.schedule) {
                    that.updateMdsData('schedule', mdsData.schedule);
                }
                if (mdsData.program) {
                    that.updateMdsData('program', mdsData.program);
                }
            };
            CProgrammeProvider.prototype.getTemplateAitUrl = function () {
                return this.templateAitUrl;
            };
            CProgrammeProvider.prototype.getDeeplinkUrl = function () {
                return this.deeplinkUrl;
            };
            CProgrammeProvider.prototype.checkTemplateAIT = function (xml) {
                if (!xml) {
                    hx.log('error', '[CProgrammeProvider] FAIL!!! checkTemplateAIT : argu(xml) is null');
                    return;
                }
                var that = this;
                //check hbbtv application
                var type = xml.querySelector('OtherApp').textContent;
                //check version
                var major = xml.querySelector('versionMajor').textContent;
                var minor = xml.querySelector('versionMinor').textContent;
                var micro = xml.querySelector('versionMicro').textContent;
                var version = (major + minor + micro) * 1;
                if (version <= 131 && type === 'application/vnd.hbbtv.xhtml+xml') {
                    var now = new Date().getTime() / 1000;
                    var fnCheckAvailabilityWindow = function (startAvail, endAvail, mediaAvailabilityCS) {
                        if (startAvail < now && now <= endAvail) {
                            if (mediaAvailabilityCS) {
                                return 2 /* EAvailable */;
                            }
                            else {
                                return 1 /* EComingSoon */;
                            }
                        }
                        else if ((now <= startAvail) && ((startAvail - 7200) < now)) {
                            return 1 /* EComingSoon */;
                        }
                        else {
                            return 0 /* ENotAvailable */;
                        }
                    };
                    if (that.endTime <= now) {
                        that.availability = fnCheckAvailabilityWindow(that.availableStartTime, that.availableEndTime, that.mediaAvailabilityCS);
                    }
                    else {
                        if (that.fEpgAvailabilityCS) {
                            that.availability = fnCheckAvailabilityWindow(that.availableStartTime, that.availableEndTime, that.mediaAvailabilityCS);
                        }
                        else {
                            that.availability = 0 /* ENotAvailable */;
                        }
                    }
                }
            };
            CProgrammeProvider.prototype.getAvailability = function () {
                return this.availability;
            };
            CProgrammeProvider.prototype.getPluginObject = function () {
                return this.pluginObject;
            };
            CProgrammeProvider.prototype.getProgrammeName = function () {
                var that = this;
                return that.name;
            };
            CProgrammeProvider.prototype.hasGuidance = function () {
                var that = this;
                if (that.pluginObject) {
                    var guidance = that.getGuidance();
                    if (guidance && ((guidance.type === 0) || (guidance.type === 1 && guidance.mode === 1))) {
                        // guidance_type | guidance_mode | Meaning
                        // ----------------------------------------------
                        //      0 1      |     n/a 1     | Unsuitable for viewing outside "watershed hours" without access controls.
                        //       1       |       0       | Warnings/guidance (e.g. Strobe lighting), but suitable for viewing anytime.
                        //      2 3      |      n/a      | Reserved for future use.
                        // ----------------------------------------------
                        return true;
                    }
                }
                else {
                    return !!that.isGuidance;
                }
                return false;
            };
            CProgrammeProvider.prototype.getGuidance = function () {
                var that = this;
                hx.log('debug', "[CProgrammeProvider] getGuidance---- text : " + (that.guidance ? that.guidance.text : ""));
                hx.log('debug', "[CProgrammeProvider] getGuidance---- type : " + (that.guidance ? that.guidance.type : ""));
                hx.log('debug', "[CProgrammeProvider] getGuidance---- mode : " + (that.guidance ? that.guidance.mode : ""));
                return that.guidance;
            };
            CProgrammeProvider.prototype.getParentalRatings = function () {
                var that = this;
                return that.parentalRating;
            };
            CProgrammeProvider.prototype.getThumbnailURL = function (noDefault) {
                var that = this;
                return noDefault ? that.thumbnailURL : (that.thumbnailURL || that.getDefaultThumbnail());
            };
            CProgrammeProvider.prototype.getMdsSchedule = function (aUpdateCb) {
                var that = this;
                if (that.getMdsData()) {
                    if (aUpdateCb) {
                        aUpdateCb();
                    }
                    return;
                }
                if (that.getServiceId()) {
                    hx.svc.IP_EPG.getSchedules(that.getServiceId(), that.getStartTime() * 1000, 6).then(function (res) {
                        var info = hx.svc.IP_EPG.parse.xml.schedule(res);
                        var schedules = Array.prototype.slice.call(info.schedules);
                        var crids = info.crids;
                        var length = schedules ? schedules.length : 0;
                        var parsedId;
                        var onNowLegacyId, mdsLegacyId;
                        var mdsEvent;
                        var schedule = null, crid = null, data = null;
                        parsedId = hx.svc.IP_EPG.parse.util.programIdParser(that.getProgrammeID());
                        onNowLegacyId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                        if (onNowLegacyId) {
                            for (var i = 0; i < length; i++) {
                                schedule = schedules[i];
                                mdsEvent = hx.svc.IP_EPG.parse.element.scheduleEvent(schedule);
                                parsedId = hx.svc.IP_EPG.parse.util.programIdParser(mdsEvent.programURL);
                                mdsLegacyId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                                if (onNowLegacyId === mdsLegacyId) {
                                    crid = schedule.querySelector('Program').getAttribute('crid');
                                    data = crids[crid];
                                    data.schedule = schedule;
                                    that.setMdsData({
                                        mdsData: data,
                                        launchLocation: 'epg'
                                    });
                                    if (aUpdateCb) {
                                        aUpdateCb();
                                    }
                                    return;
                                }
                            }
                        }
                        if (aUpdateCb) {
                            aUpdateCb();
                        }
                    }, function () {
                        if (aUpdateCb) {
                            aUpdateCb();
                        }
                    });
                }
                else {
                    if (aUpdateCb) {
                        aUpdateCb();
                    }
                }
            };
            CProgrammeProvider.prototype.getThumbnail = function (param) {
                // updateCb : a callback to update UI(thumbnail) after receiving the image from server.
                // width : a width of needed thumbnail.
                // defaultPath : a path where default thumbnail is.
                var a = this;
                if (!param) {
                    //hx.log('error', '[CProgrammeProvider] getThumbnail fail!!! coz param is null');
                    return a.thumbnailURL || a.getDefaultThumbnail();
                }
                var updateCb = param.updateCb || null;
                var width = param.width || a.DEFAULT_THUMB_WIDTH;
                var defaultPath = param.defaultPath || '';
                if (!updateCb) {
                    return a.thumbnailURL || (defaultPath ? defaultPath + a.getDefaultThumbnail() : a.getDefaultThumbnail());
                }
                if (!a.requestThumbUrl) {
                    hx.log('error', '[CProgrammeProvider] getThumbnail fail!!! coz requestThumbUrl is null');
                    updateCb(defaultPath ? defaultPath + a.getDefaultThumbnail() : a.getDefaultThumbnail());
                    return a.thumbnailURL || (defaultPath ? defaultPath + a.getDefaultThumbnail() : a.getDefaultThumbnail());
                }
                if (hx.il.checkMdsUrl(a.thumbnailURL, width)) {
                    updateCb(a.thumbnailURL);
                    return a.thumbnailURL;
                }
                hx.svc.IP_EPG.getThumbnail(a.requestThumbUrl, width, { "content-type": a.requestImgContentType }).then(function (data) {
                    if (data) {
                        updateCb(a.thumbnailURL = data);
                    }
                    else {
                        updateCb(defaultPath ? defaultPath + a.getDefaultThumbnail() : a.getDefaultThumbnail());
                    }
                }, function () {
                    updateCb(defaultPath ? defaultPath + a.getDefaultThumbnail() : a.getDefaultThumbnail());
                });
            };
            CProgrammeProvider.prototype.getChannelLogo = function (param) {
                var a = this;
                if (!param) {
                    hx.log('error', '[CProgrammeProvider] getChannelLogo fail!!! coz param is null');
                    return '';
                }
                if (!a.getServiceId()) {
                    hx.log('error', '[CProgrammeProvider] getChannelLogo fail!!! coz !a.getServiceId()');
                    return '';
                }
                var updateCb = param.updateCb || null;
                var width = param.width || a.DEFAULT_LOGO_WIDTH;
                if (!updateCb || !width) {
                    hx.log('error', '[CProgrammeProvider] getChannelLogo fail!!! coz !updateCb || !width');
                    return '';
                }
                if (a.logoURL) {
                    return a.logoURL;
                }
                else {
                    hx.svc.IP_EPG.getChannelLogo(a.getServiceId(), width).then(function (data) {
                        updateCb(a.logoURL = data || '');
                    }, function () {
                        updateCb('');
                    });
                }
            };
            CProgrammeProvider.prototype.getType = function () {
                return (this.getChannelType() === 0) ? hx.l('LOC_TV_PROGRAMME_ID') : hx.l('LOC_RADIO_PROGRAMME');
            };
            CProgrammeProvider.prototype.getDefaultThumbnail = function () {
                return this.getGenreImage() || __MODEL__.TDefaultThumbnailList.DEFAULT;
            };
            CProgrammeProvider.prototype.getSecondary = function () {
                return this.secondary || '';
            };
            CProgrammeProvider.prototype.getShortDescription = function () {
                var that = this;
                return that.shortDescription || '';
            };
            CProgrammeProvider.prototype.getLongDescription = function () {
                var that = this;
                return that.longDescription || '';
            };
            CProgrammeProvider.prototype.getStartTime = function () {
                var that = this;
                return that.startTime || 0;
            };
            CProgrammeProvider.prototype.getEndTime = function () {
                var that = this;
                return that.endTime;
            };
            CProgrammeProvider.prototype.getDuration = function () {
                var that = this;
                return that.duration || 0;
            };
            CProgrammeProvider.prototype.getPercentage = function () {
                var that = this;
                var now = (new Date()).getTime() / 1000;
                return parseInt(((now - that.getStartTime()) / that.getDuration() * 100) + "", 10);
            };
            CProgrammeProvider.prototype.getPercentageByTime = function (now) {
                var that = this;
                return parseInt(((now - that.getStartTime()) / (that.getDuration() * 100)) + "", 10);
            };
            CProgrammeProvider.prototype.isLive = function () {
                var that = this;
                var now = (new Date()).getTime() / 1000;
                var st = that.getStartTime();
                var et = that.getStartTime() + that.getDuration();
                return (now >= st && now <= et) ? true : false;
            };
            CProgrammeProvider.prototype.isPast = function () {
                var that = this;
                var now = (new Date()).getTime() / 1000;
                var st = that.getStartTime();
                var et = that.getStartTime() + that.getDuration();
                return (et < now) ? true : false;
            };
            CProgrammeProvider.prototype.getChannelInstance = function () {
                var that = this;
                return that.channel;
            };
            CProgrammeProvider.prototype.getChannelType = function () {
                var that = this;
                return that.channelType;
            };
            CProgrammeProvider.prototype.getChannelNum = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().majorChannel : null;
            };
            CProgrammeProvider.prototype.getChannelName = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().name : null;
            };
            CProgrammeProvider.prototype.setStartTime = function (startTime) {
                this.startTime = startTime;
                return this;
            };
            CProgrammeProvider.prototype.setDuration = function (duration) {
                this.duration = duration;
                return this;
            };
            CProgrammeProvider.prototype.setEndTime = function (endTime) {
                this.endTime = endTime;
                return this;
            };
            CProgrammeProvider.prototype.setChannelInformation = function (channel) {
                if (channel) {
                    this.channelType = channel.channelType;
                    this.thumbnailURL = this.defaultThumbnail = (this.channelType === 1) ? __MODEL__.TDefaultThumbnailList.RADIO : __MODEL__.TDefaultThumbnailList.TV;
                    this.ccid = channel.ccid;
                }
                return this;
            };
            CProgrammeProvider.prototype.setProgrammeName = function (name) {
                this.name = name;
                return this;
            };
            CProgrammeProvider.prototype.isRecording = function () {
                var that = this;
                var isRecording = false;
                var programme = that.pluginObject;
                if (programme) {
                    var recording = programme.recording;
                    if (recording) {
                        if (recording.id && (recording.showType === 0)) {
                            isRecording = true;
                        }
                    }
                }
                return isRecording;
            };
            CProgrammeProvider.prototype.isPlayAvailable = function () {
                return !!(this.getAvailability() === 2 /* EAvailable */);
            };
            CProgrammeProvider.prototype.hasSeriesSchedule = function () {
                var that = this;
                var isSeriesRecording = false;
                var programme = that.pluginObject;
                if (programme) {
                    // scheduledrecording
                    var recording = programme.recording;
                    if (recording && recording.factoryType === 0) {
                        if (recording.startTime <= that.getStartTime() && recording.isScheduledAsSeries) {
                            isSeriesRecording = true;
                        }
                    }
                }
                return isSeriesRecording;
            };
            CProgrammeProvider.prototype.hasSchedule = function () {
                var that = this;
                var isRecSchedule = false;
                var programme = that.pluginObject;
                if (programme) {
                    // scheduledrecording
                    var recording = programme.recording;
                    if (recording && recording.factoryType === 0) {
                        if (recording.startTime <= that.getStartTime() && !recording.isScheduledAsSeries) {
                            isRecSchedule = true;
                        }
                    }
                }
                return isRecSchedule;
            };
            CProgrammeProvider.prototype.hasReminder = function () {
                var that = this;
                var isReminder = false;
                var programme = that.pluginObject;
                if (programme) {
                    // scheduledrecording
                    var recording = programme.recording;
                    if (recording && recording.factoryType === 1) {
                        isReminder = true;
                    }
                }
                return isReminder;
            };
            CProgrammeProvider.prototype.getObjectType = function () {
                return 'EPG_DATA';
            };
            CProgrammeProvider.prototype.getGenreImage = function () {
                var that = this;
                var i, length, major, minor, word, content = that.genre;
                // priority of genre image from RF is higher than from MDS
                if (content && !(content instanceof Array)) {
                    for (i = 0, length = content.length; i < length; i += 1) {
                        word = content[i] || '';
                        major = parseInt(word.charAt(0), 16) - 1;
                        minor = parseInt(word.charAt(1), 16);
                        return hx.il.genreList.getGenreImage(major);
                    }
                }
                else if (content && content.length) {
                    return hx.il.genreList.getGenreImage(content[0]);
                }
                return null;
            };
            CProgrammeProvider.prototype.getGenre = function () {
                var that = this;
                var i, length, major, minor, word, genres = [], contents = that.genre || [];
                var userGenre;
                var majorNibbles = hx.il.genreList.getGenreList('major', false);
                var minorNibbles = hx.il.genreList.getGenreList('minor', false);
                function validateStringNumericInRange(value, min, max) {
                    return (!isNaN(value) && value > min && value < max);
                }
                if (contents && !(contents instanceof Array)) {
                    for (i = 0, length = contents.length; i < length; i += 1) {
                        word = contents[i] || '';
                        major = parseInt(word.charAt(0), 16) - 1;
                        minor = parseInt(word.charAt(1), 16);
                        if (validateStringNumericInRange(major, -1, majorNibbles.length)) {
                            if (validateStringNumericInRange(minor, -1, minorNibbles.length)) {
                                genres.push(hx.l(minorNibbles[major][minor]));
                            }
                            if (that._userGenres === null || that._userGenres === undefined) {
                                that._userGenres = [];
                            }
                            //that._userGenres.push(hx.l(majorNibbles[major]));
                            if (majorNibbles[major] !== '') {
                                userGenre = hx.l(majorNibbles[major]);
                            }
                            if (that._userGenres.indexOf(userGenre) === -1) {
                                that._userGenres.push(userGenre);
                            }
                        }
                    }
                    return genres.length ? genres : that._userGenres;
                }
                return contents;
            };
            CProgrammeProvider.prototype.watchNow = function (callback) {
                var that = this;
                var channel = that.getChannelInstance();
                var comsumed = false;
                if (channel && that.isPlayAvailable()) {
                    var bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                        channel: channel
                    });
                    if (bChangeable === false) {
                        if (hx.config.useTVStreaming === false) {
                            hx.msg('response', {
                                text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                auto_close: true,
                                close_time: 3000
                            });
                        }
                        else {
                            hx.msg('response', {
                                text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                auto_close: true,
                                close_time: 3000
                            });
                        }
                        return;
                    }
                    if (callback) {
                        callback(true);
                    }
                    hx.svc.AV_CTRL.channelChange({
                        channel: channel,
                        group: (channel.channelType === 0) ? 'TV' : 'RADIO'
                    });
                    comsumed = true;
                }
                else {
                    if (callback) {
                        callback();
                        comsumed = true;
                    }
                }
                return comsumed;
            };
            CProgrammeProvider.prototype.getRecommendationCRID = function () {
                var that = this;
                return that.recommendationCRID || null;
            };
            CProgrammeProvider.prototype.setProgrammeID = function (programmeId) {
                this.programmeID = programmeId;
                return this;
            };
            CProgrammeProvider.prototype.getProgrammeID = function () {
                var that = this;
                return that.programmeID || that.mdsProgrammeID;
            };
            CProgrammeProvider.prototype.getMdsProgrammeID = function () {
                return this.mdsProgrammeID || null;
            };
            CProgrammeProvider.prototype.getProgrammeCRID = function () {
                var that = this;
                return that.programmeCRID || null;
            };
            CProgrammeProvider.prototype.getProgrammeCridMds = function () {
                return this.programmeCridMds || null;
            };
            CProgrammeProvider.prototype.getGroupCRIDs = function () {
                var that = this;
                return that.groupCRIDs || null;
            };
            CProgrammeProvider.prototype.getSeriesID = function () {
                var that = this;
                return that.getGroupCRIDs() ? that.getGroupCRIDs()[0] : null;
            };
            CProgrammeProvider.prototype.getServiceId = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().sid : null;
            };
            CProgrammeProvider.prototype.getServiceIDRef = function () {
                return this.serviceIDRef || null;
            };
            CProgrammeProvider.prototype.isHd = function () {
                var that = this;
                return that.isHD || false;
            };
            CProgrammeProvider.prototype.isAd = function () {
                var that = this;
                return that.isAD || false;
            };
            CProgrammeProvider.prototype.isLocked = function () {
                var that = this;
                return that.locked;
            };
            CProgrammeProvider.prototype.getHDLinkage = function () {
                var that = this;
                if (that.linkage === undefined && that.pluginObject) {
                    that.linkage = that.pluginObject.getField("freeview/linkage");
                    if (that.linkage) {
                        that.linkage = JSON.parse(that.linkage);
                    }
                }
                return that.linkage || null;
            };
            CProgrammeProvider.prototype.getChannelId = function () {
                var that = this;
                return that.ccid;
            };
            CProgrammeProvider.prototype.isMultiAudio = function () {
                var that = this;
                if (that.audioType === undefined && that.pluginObject) {
                    that.audioType = that.pluginObject.audioType;
                }
                if (that.audioType === 4) {
                    return true;
                }
                return false;
            };
            CProgrammeProvider.prototype.getSubtitleType = function () {
                var that = this;
                if (that.subtitleType === undefined && that.pluginObject) {
                    that.subtitleType = that.pluginObject.subtitleType;
                }
                return that.subtitleType || null;
            };
            CProgrammeProvider.prototype.getSubtitleTypes = function () {
                var that = this;
                if (that.subtitleTypes === undefined && that.pluginObject) {
                    that.subtitleTypes = that.pluginObject.subtitleTypes;
                }
                return that.subtitleTypes || null;
            };
            CProgrammeProvider.prototype.isOnAir = function () {
                return false;
            };
            return CProgrammeProvider;
        })(CMetadataProvider);
        mProvider.CProgrammeProvider = CProgrammeProvider;
        var CRecordingProvider = (function (_super) {
            __extends(CRecordingProvider, _super);
            function CRecordingProvider(recording) {
                _super.call(this, null, recording.name || recording.channel.name, recording.thumbnail);
                var plgObj;
                if (recording) {
                    this.pluginObject = recording;
                    //recording is not plugin object, but use same name as other provider
                    this.channel = recording.channel;
                    this.name = recording.name || this.channel.name;
                    this.startTime = recording.recStart;
                    this.duration = recording.recDuration;
                    this.originStartTime = recording.startTime;
                    this.originDuration = recording.duration;
                    this.endTime = this.startTime + this.duration;
                    this.recordingStartTime = recording.recordingStartTime;
                    this.recordingDuration = recording.recordingDuration;
                    this.recordingEndTime = this.recordingStartTime + this.recordingDuration;
                    this.lastPlayPosition = recording.lastPlayPosition;
                    this.shortDescription = recording.description;
                    this.longDescription = recording.longDescription;
                    this.isManualLocked = recording.isManualLocked;
                    this.programmeID = recording.programmeID;
                    this.thumbnailURL = recording.thumbnail;
                    this.showType = recording.showType;
                    this.uid = recording._uid;
                    this.id = recording.id;
                    this.seriesID = recording.seriesID;
                    this.state = recording.state;
                    this.error = recording.error;
                    this.isNull = false;
                    this.ccid = this.channel ? this.channel.ccid : '';
                    this.type = recording.type;
                    plgObj = recording.pluginObject;
                    if (plgObj) {
                        this.episode = plgObj.episode;
                        this.season = plgObj.season;
                        this.isHD = plgObj.isHD;
                        this.isAD = plgObj.isAD;
                        this.longDescription = plgObj.longDescription;
                        var i, len, genres = plgObj.genres, parentalRatings = plgObj.parentalRatings;
                        if ((genres !== null) && (genres !== undefined)) {
                            this.genre = [];
                            for (i = 0, len = genres.length; i < len; i += 1) {
                                this.genre.push(genres[i]);
                            }
                        }
                        if ((parentalRatings !== null) && (parentalRatings !== undefined)) {
                            this.parentalRating = [];
                            for (i = 0, len = parentalRatings.length; i < len; i += 1) {
                                this.parentalRating.push(parentalRatings[i]);
                            }
                        }
                    }
                }
            }
            CRecordingProvider.prototype.getStartTime = function () {
                return this.recordingStartTime || 0;
            };
            CRecordingProvider.prototype.getEndTime = function () {
                return this.recordingEndTime || 0;
            };
            CRecordingProvider.prototype.getThumbnail = function () {
                var that = this;
                return that.thumbnailURL || that.getDefaultThumbnail();
            };
            CRecordingProvider.prototype.getChannelLogo = function (param) {
                var a = this;
                if (!param) {
                    hx.log('error', '[CRecordingProvider] getChannelLogo fail!!! coz param is null');
                    return '';
                }
                if (!a.getServiceId()) {
                    hx.log('error', '[CRecordingProvider] getChannelLogo fail!!! coz !a.getServiceId()');
                    return '';
                }
                var updateCb = param.updateCb || null;
                var width = param.width || a.DEFAULT_LOGO_WIDTH;
                if (!updateCb || !width) {
                    hx.log('error', '[CRecordingProvider] getChannelLogo fail!!! coz !updateCb || !width');
                    return '';
                }
                if (a.logoURL) {
                    return a.logoURL;
                }
                else {
                    hx.svc.IP_EPG.getChannelLogo(a.getServiceId(), width).then(function (data) {
                        if (data) {
                            if (updateCb) {
                                updateCb(data);
                            }
                            a.logoURL = data;
                        }
                    }, function () {
                        if (updateCb) {
                            updateCb('');
                        }
                    });
                }
            };
            CRecordingProvider.prototype.getDefaultThumbnail = function () {
                var that = this;
                var defaultThumbnail = __MODEL__.TDefaultThumbnailList.RECORDING;
                if (that.state === 8 /* ERECORDING_REC_PARTIALLY_COMPLETED */ && that.pluginObject.showType === 1 /* EFIRST_RUN */) {
                    defaultThumbnail = __MODEL__.TDefaultThumbnailList.REC_FAIL;
                }
                return defaultThumbnail;
            };
            CRecordingProvider.prototype.getGenreImage = function () {
                var that = this;
                var i, length, major, minor, word, content = that.genre || [];
                for (i = 0, length = content.length; i < length; i += 1) {
                    word = content[i] || '';
                    major = parseInt(word.charAt(0), 16) - 1;
                    minor = parseInt(word.charAt(1), 16);
                    return hx.il.genreList.getGenreImage(major);
                }
                return null;
            };
            CRecordingProvider.prototype.getDispDuration = function () {
                var that = this;
                var t = this.recordingDuration;
                var hour = 0, min = 0, str = [];
                if (t >= 60) {
                    t = t / 60;
                    hour = parseInt((t / 60) + "", 10);
                    min = parseInt((t % 60) + "", 10);
                    if (hour > 0) {
                        str.push(hour + 'h ');
                    }
                    str.push(min + 'm');
                }
                else {
                    str.push('0m');
                }
                return str.join('');
            };
            CRecordingProvider.prototype.getShortDescription = function () {
                return this.shortDescription;
            };
            CRecordingProvider.prototype.getLongDescription = function () {
                return this.longDescription;
            };
            CRecordingProvider.prototype.getDuration = function () {
                return this.duration;
            };
            CRecordingProvider.prototype.getUserID = function () {
                return this.uid;
            };
            CRecordingProvider.prototype.isRecording = function () {
                var that = this;
                if (that.showType === 'LIVE') {
                    return true;
                }
                return false;
            };
            CRecordingProvider.prototype.isRecorded = function () {
                var that = this;
                if (that.id && that.showType !== 'LIVE') {
                    return true;
                }
                return false;
            };
            CRecordingProvider.prototype.hasSeriesSchedule = function () {
                var that = this;
                if (that.type === 'SCHEDULE' && that.pluginObject.isScheduledAsSeries) {
                    return true;
                }
                return false;
            };
            CRecordingProvider.prototype.hasSchedule = function () {
                var that = this;
                if (that.type === 'SCHEDULE' && that.pluginObject.factoryType === 'RECORDING') {
                    return true;
                }
                return false;
            };
            CRecordingProvider.prototype.hasReminder = function () {
                var that = this;
                if (that.type === 'SCHEDULE' && that.pluginObject.factoryType === 'REMIND') {
                    return true;
                }
                return false;
            };
            CRecordingProvider.prototype.getChannelNum = function () {
                var that = this;
                return that.channel ? that.channel.majorChannel : '';
            };
            CRecordingProvider.prototype.getServiceId = function () {
                return this.channel ? this.channel.sid : null;
            };
            CRecordingProvider.prototype.getRecordingID = function () {
                return this.id;
            };
            CRecordingProvider.prototype.getGenre = function () {
                var that = this;
                var i, length, major, minor, word, genres = [], content = that.genre || [];
                var userGenre;
                var majorNibbles = ['LOC_GENRE_MOVIE_ID', 'LOC_GENRE_NEWS_ID', 'LOC_GENRE_SHOW_ID', 'LOC_GENRE_SPORTS_ID', 'LOC_GENRE_CHILDREN_ID', 'LOC_GENRE_MUSIC_ID', 'LOC_GENRE_ARTS_ID', 'LOC_GENRE_SOCIAL_ID', 'LOC_GENRE_EDUCATION_ID', 'LOC_GENRE_LEISURE_ID', 'LOC_GENRE_SPECIAL_CHARACTER_ID'], 
                // //
                minorNibbles = [['LOC_GENRE_MOVIE_DRAMA_ID', 'LOC_GENRE_DETECTIVE_THRILLER_ID', 'LOC_GENRE_ADVENTURE_WESTERN_WAR_ID', 'LOC_GENRE_SF_FANSTASY_HORROR_ID', 'LOC_GENRE_COMEDY_ID', 'LOC_GENRE_SOAP_MELO_FOLKLORIC_ID', 'LOC_GENRE_ROMANCE_ID', 'LOC_GENRE_SERIOUS_DRAMA_ID', 'LOC_GENRE_ADULT_MOVIE_DRAMA_ID'], ['LOC_GENRE_NEWS_CURRENT_AFFAIRS_ID', 'LOC_GENRE_NEWS_WEATHER_REPORT_ID', 'LOC_GENRE_NEWS_MAGAZINE_ID', 'LOC_GENRE_DOCUMENTARY_ID', 'LOC_GENRE_INTERVIEW_ID'], ['LOC_GENRE_SHOW_GAME_ID', 'LOC_GENRE_SHOW_QUIZ_CONTEST_ID', 'LOC_GENRE_VARIETY_SHOW_ID', 'LOC_GENRE_TALK_SHOW_ID'], ['LOC_GENRE_SPECIAL_EVENTS_ID', 'LOC_GENRE_SPORTS_MAGAZINE_ID', 'LOC_GENRE_FOOTBALL_SOCCER_ID', 'LOC_GENRE_TENNIS_SQUASH_ID', 'LOC_GENRE_TEAM_SPROTS_ID', 'LOC_GENRE_ATHLETICS_ID', 'LOC_GENRE_MOTOR_SPORT_ID', 'LOC_GENRE_WATER_SPORT_ID', 'LOC_GENRE_WINTER_SPORTS_ID', 'LOC_GENRE_EQUESTRIAN_ID', 'LOC_GENRE_MARTIAL_SPORTS_ID'], ['LOC_GENRE_CHILDREN_YOUTH_ID', 'LOC_GENRE_PRESCHOOL_CHILDREN_ID', 'LOC_GENRE_6TO4_ENTERTAIN_ID', 'LOC_GENRE_10TO16_ENTERTAIN_ID', 'LOC_GENRE_INFO_EDU_SCHOOL_ID', 'LOC_GENRE_CARTOONS_PUPPETS_ID'], ['LOC_GENRE_MUSIC_DANCE_ID', 'LOC_GENRE_ROCK_POP_ID', 'LOC_GENRE_CLASSIC_ID', 'LOC_GENRE_FOLK_TRADITIONAL_ID', 'LOC_GENRE_JAZZ_ID', 'LOC_GENRE_MUSICAL_OPERA_ID', 'LOC_GENRE_BALLET_ID'], ['LOC_GENRE_ARTS_CULTURE_ID', 'LOC_GENRE_PREFORMING_ARTS_ID', 'LOC_GENRE_FINE_ARTS_ID', 'LOC_GENRE_RELIGION_ID', 'LOC_GENRE_POP_TRADITIONAL_ID', 'LOC_GENRE_LITERATURE_ID', 'LOC_GENRE_FILM_CINEMA_ID', 'LOC_GENRE_EXPERIMENTAL_ID', 'LOC_GENRE_BROADCASTING_PRESS_ID', 'LOC_GENRE_NEW_MEDIA_ID', 'LOC_GENRE_ARTS_MAGAZINE_ID', 'LOC_GENRE_FASHION_ID'], ['LOC_GENRE_SOCIAL_ECONOMICS_ID', 'LOC_GENRE_MAGAZINE_ID', 'LOC_GENRE_ADVISORY_ID', 'LOC_GENRE_REMARKABLE_ID'], ['LOC_GENRE_EDU_SCIENCE_FACTUAL_ID', 'LOC_GENRE_ENVIRONMENT_ID', 'LOC_GENRE_TECH_SCIENCE_ID', 'LOC_GENRE_MEDI_PHYSI_PSYCH_ID', 'LOC_GENRE_FOREIGN_ID', 'LOC_GENRE_SOCIAL_SPIRITUAL_ID', 'LOC_GENRE_FURTHER_EDU_ID', 'LOC_GENRE_LANGUAGE_ID'], ['LOC_GENRE_LEISURE_HOBBIES_ID', 'LOC_GENRE_TOURISM_TRAVEL_ID', 'LOC_GENRE_HANDICRAFT', 'LOC_GENRE_MOTORING_ID', 'LOC_GENRE_FITNESS_HEALTH_ID', 'LOC_GENRE_COOKING_ID', 'LOC_GENRE_SHOPPING_ID', 'LOC_GENRE_GARDENING_ID'], ['LOC_GENRE_ORIGIN_LANG_ID', 'LOC_GENRE_BLACK_WHITE_ID', 'LOC_GENRE_UNPUBLISHED_ID', 'LOC_GENRE_LIVE_BROADCAST_ID']];
                function validateStringNumericInRange(value, min, max) {
                    return (!isNaN(value) && value > min && value < max);
                }
                for (i = 0, length = content.length; i < length; i += 1) {
                    word = content[i] || '';
                    major = parseInt(word.charAt(0), 16) - 1;
                    minor = parseInt(word.charAt(1), 16);
                    hx.log('srvRec', 'genre major : ' + major + ', minor : ' + minor);
                    if (validateStringNumericInRange(major, -1, majorNibbles.length)) {
                        if (validateStringNumericInRange(minor, -1, minorNibbles.length)) {
                            genres.push(hx.l(minorNibbles[major][minor]));
                        }
                        if (that._userGenres === null) {
                            that._userGenres = [];
                        }
                        //that._userGenres.push(hx.l(majorNibbles[major]));
                        userGenre = hx.l(majorNibbles[major]);
                        if (that._userGenres.indexOf(userGenre) === -1) {
                            that._userGenres.push(userGenre);
                        }
                    }
                }
                return genres;
            };
            CRecordingProvider.prototype.isNullProgramme = function () {
                return this.isNull !== false;
            };
            CRecordingProvider.prototype.getObjectType = function () {
                return 'RECORDING';
            };
            CRecordingProvider.prototype.getType = function () {
                return 'Recording';
            };
            CRecordingProvider.prototype.isPlayAvailable = function () {
                return true;
            };
            CRecordingProvider.prototype.watchNow = function (callback) {
                var that = this;
                if (callback && that.isPlayAvailable()) {
                    callback(that);
                    return true;
                }
                return false;
            };
            CRecordingProvider.prototype.isHd = function () {
                return this.isHD;
            };
            CRecordingProvider.prototype.getGuidance = function () {
                var that = this;
                if (that.guidance === undefined) {
                    that.guidance = that.pluginObject.getField("freeview/guidance");
                }
                if (that.guidance) {
                    var guidance = JSON.parse(that.guidance);
                    hx.log('debug', "[CRecordingProvider] getGuidance---- text : " + (guidance.guidance ? guidance.guidance[0].text : ""));
                    hx.log('debug', "[CRecordingProvider] getGuidance---- type : " + (guidance.guidance ? guidance.guidance[0].type : ""));
                    hx.log('debug', "[CRecordingProvider] getGuidance---- mode : " + (guidance.guidance ? guidance.guidance[0].mode : ""));
                    return guidance.guidance ? guidance.guidance[0] : "";
                }
                else {
                    return "";
                }
            };
            return CRecordingProvider;
        })(CMetadataProvider);
        mProvider.CRecordingProvider = CRecordingProvider;
        var CChannelProvider = (function (_super) {
            __extends(CChannelProvider, _super);
            function CChannelProvider(channel) {
                _super.call(this, null, channel.name, channel.logoURL);
                if (channel) {
                    this.pluginObject = channel;
                    this.name = channel.name;
                    this.channelType = channel.channelType;
                    this.logoURL = channel.logoURL;
                    this.ccid = channel.ccid;
                    this.majorChannel = channel.majorChannel;
                    this.isHD = channel.isHD;
                }
            }
            CChannelProvider.prototype.getType = function () {
                var that = this;
                if (that.isHD) {
                    return "HDTV";
                }
                else {
                    return (that.channelType === 0) ? "TV" : "Radio";
                }
            };
            CChannelProvider.prototype.getChannelNum = function () {
                return this.majorChannel;
            };
            CChannelProvider.prototype.isPlayAvailable = function () {
                return false;
            };
            CChannelProvider.prototype.watchNow = function (callback) {
                var that = this;
                var channel = that.pluginObject;
                if (channel) {
                    if (callback) {
                        callback(that);
                    }
                    var bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                        channel: channel
                    });
                    if (bChangeable === false) {
                        if (hx.config.useTVStreaming === false) {
                            hx.msg('response', {
                                text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                auto_close: true,
                                close_time: 3000
                            });
                        }
                        else {
                            hx.msg('response', {
                                text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                auto_close: true,
                                close_time: 3000
                            });
                        }
                        return;
                    }
                    hx.svc.AV_CTRL.channelChange({
                        channel: channel,
                        group: (channel.channelType === 0) ? 'TV' : 'RADIO'
                    });
                    return true;
                }
                return false;
            };
            CChannelProvider.prototype.getServiceId = function () {
                return this.pluginObject.sid;
            };
            CChannelProvider.prototype.getThumbnail = function (param) {
                var that = this;
                var updateCb = param ? (param.updateCb || null) : null;
                var width = param ? (param.width || that.DEFAULT_THUMB_WIDTH) : null;
                if (that.logoURL) {
                    return that.logoURL;
                }
                if (updateCb && width) {
                    hx.svc.IP_EPG.getChannelLogo(that.getServiceId(), width).then(function (data) {
                        if (data) {
                            updateCb(data);
                            that.logoURL = data;
                        }
                    });
                }
                return that.thumbnailURL;
            };
            CChannelProvider.prototype.isHd = function () {
                return this.isHD;
            };
            return CChannelProvider;
        })(CMetadataProvider);
        mProvider.CChannelProvider = CChannelProvider;
        var CHumaxContentProvider = (function (_super) {
            __extends(CHumaxContentProvider, _super);
            function CHumaxContentProvider(content) {
                this.VIDEO = 'ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|mpg|vob|divx|mpeg';
                this.PHOTO = 'jpg|jpeg|bmp|gif|tif|png';
                this.MUSIC = 'mp3|wav';
                //XXX: XXjklsjdfasdXXXXfiajfoiajsfojewo1!XXX!!
                if (!content) {
                    throw 'content needed!';
                }
                var hdAudio = hx.config.CONFIG_HD_AUDIO;
                if (hdAudio) {
                    this.MUSIC += '|flac';
                }
                this._content = content;
                this.contentType = content.contentType;
                this.fullPath = content.fullPath && content.fullPath.indexOf('file:/') > -1 ? content.fullPath : 'file:/'.concat(content.fullPath);
                this.hasSubitem = content.hasSubitem;
                this.date = content.date;
                this.duration = content.duration;
                this.lastViewTime = content.lastViewTime;
                this.thumbnailUrl = content.thumbnailUrl;
                this.resolution = content.resolution;
                this.thumbResolution = content.thumbResolution;
                this.pictureDate = content.pictureDate;
                this.album = content.album;
                this.artist = content.artist;
                this.watched = content.watched;
                this.storageCategory = content.storageCategory;
                this.title = this.fullPath.substr(this.fullPath.lastIndexOf('/') + 1, this.fullPath.length - 1);
                _super.call(this, null, this.title, this.thumbnailUrl);
            }
            CHumaxContentProvider.prototype.getPluginObject = function () {
                return this._content;
            };
            CHumaxContentProvider.prototype.getType = function () {
                var that = this;
                var STORAGE_TYPE = [hx.l('LOC_HDD_ID'), hx.l('LOC_USB_ID'), hx.l('LOC_SDCARD_ID'), hx.l('LOC_UNKNOWN_ID'), hx.l('LOC_UNKNOWN_ID')];
                var storage = STORAGE_TYPE[that.storageCategory];
                return storage;
            };
            CHumaxContentProvider.prototype.getMediaType = function () {
                var that = this;
                var lastIndex = that.fullPath ? that.fullPath.lastIndexOf('.') : -1;
                var extension = '';
                var contentType = '';
                if (lastIndex !== -1) {
                    extension = that.fullPath.substr(lastIndex + 1).toLowerCase();
                    if (that.VIDEO.indexOf(extension) > -1) {
                        contentType = hx.l('LOC_VIDEO_ID');
                    }
                    else if (that.PHOTO.indexOf(extension) > -1) {
                        contentType = hx.l('LOC_PHOTO_ID');
                    }
                    else if (that.MUSIC.indexOf(extension) > -1) {
                        contentType = hx.l('LOC_GENRE_MUSIC_ID');
                    }
                    return contentType;
                }
                return '';
            };
            CHumaxContentProvider.prototype.isPlayAvailable = function () {
                return true;
            };
            CHumaxContentProvider.prototype.watchNow = function (callback) {
                var that = this;
                if (callback && that.isPlayAvailable()) {
                    callback(that);
                    return true;
                }
                return false;
            };
            return CHumaxContentProvider;
        })(CMetadataProvider);
        mProvider.CHumaxContentProvider = CHumaxContentProvider;
        var CApplicationProvider = (function (_super) {
            __extends(CApplicationProvider, _super);
            function CApplicationProvider(application) {
                var that = this;
                if (application) {
                    that.id = application.appid;
                    that.name = application.name;
                    // that.localURI = application.localURI;
                    that.downloadURI = application.contenturl;
                    // that.defaultIcon = application.defaultIcon;
                    that.customIcons = application.icons;
                    that.pluginObject = application;
                }
                _super.call(this, null, that.name, (this.customIcons && this.customIcons[3]) ? this.customIcons[3].Url : '/');
            }
            CApplicationProvider.prototype.getType = function () {
                return hx.l('LOC_APPLICATION_ID');
            };
            CApplicationProvider.prototype.watchNow = function (callback) {
                var handled = false;
                if (callback) {
                    callback();
                    handled = true;
                }
                return true;
            };
            return CApplicationProvider;
        })(CMetadataProvider);
        mProvider.CApplicationProvider = CApplicationProvider;
        var CWebContentProvider = (function (_super) {
            __extends(CWebContentProvider, _super);
            function CWebContentProvider(content) {
                _super.call(this, null, content.Title, content.Images[0] ? content.Images[0].ImageURL : '');
                this._TYPE = ['WEB'];
                if (content) {
                    this.country = content.Country;
                    this.duration = content.Duration;
                    this.expireDate = content.ExpireDate;
                    this.genre = content.Genre;
                    this.geoRestrict = content.Geo_restrict;
                    this.id = content.ID;
                    this.images = content.Images;
                    this.keywords = content.KeyWords;
                    this.lang = content.Lang;
                    this.longText = content.LongText;
                    this.medias = content.Medias;
                    this.persons = content.Persons;
                    this.pubDate = content.PubDate;
                    this.rating = content.Rating;
                    this.ratingType = content.RatingType;
                    this.seasonId = content.SeasonId;
                    this.seriesCount = content.SeriesCount;
                    this.seriesId = content.SeriesId;
                    this.seriesNumber = content.SeriesNumber;
                    this.seriesTitle = content.SeriesTitle;
                    this.shortText = content.ShortText;
                    this.title = this.name = content.Title;
                    this.thumbnailUrl = this.images[0] ? this.images[0].ImageURL : '';
                    this.transmitDate = content.TransmitDate;
                    this.type = '';
                    this.dummy = !!content.dummy;
                }
            }
            CWebContentProvider.prototype.getType = function () {
                var that = this;
                return that.type;
            };
            CWebContentProvider.prototype.isPlayAvailable = function () {
                return !this.dummy;
            };
            CWebContentProvider.prototype.watchNow = function (callback) {
                var that = this;
                if (callback && that.isPlayAvailable()) {
                    callback(that);
                    return true;
                }
                return false;
            };
            return CWebContentProvider;
        })(CMetadataProvider);
        mProvider.CWebContentProvider = CWebContentProvider;
        var CDlnaContentProvider = (function (_super) {
            __extends(CDlnaContentProvider, _super);
            function CDlnaContentProvider(content) {
                _super.call(this, null, content.getTitle(), content.getThumbnailUrl());
                this.title = content.getTitle();
                this.thumbnailUrl = content.getThumbnailUrl();
                this.duration = content.getDuration();
                this.fullPath = this.importURI = content.getImportUrl();
                this.byteSeekAble = content.isByteSeekAvailable();
                this.resolution = content.getResolution();
                this._content = content;
            }
            CDlnaContentProvider.prototype.getPluginObject = function () {
                return this._content;
            };
            CDlnaContentProvider.prototype.getType = function () {
                var that = this;
                return that.type;
            };
            CDlnaContentProvider.prototype.getMediaType = function () {
                var that = this;
                var objectType = that._content.getObjectType();
                var str = '';
                if (objectType === 'video') {
                    str = hx.l('LOC_VIDEO_ID');
                }
                else if (objectType === 'photo') {
                    str = hx.l('LOC_PHOTO_ID');
                }
                else if (objectType === 'music') {
                    str = hx.l('LOC_GENRE_MUSIC_ID');
                }
                return str;
            };
            CDlnaContentProvider.prototype.isPlayAvailable = function () {
                return true;
            };
            return CDlnaContentProvider;
        })(CMetadataProvider);
        mProvider.CDlnaContentProvider = CDlnaContentProvider;
        var COnDemandProvider = (function (_super) {
            __extends(COnDemandProvider, _super);
            function COnDemandProvider(aData, aLaunchLocation) {
                if (!aData) {
                    hx.log('error', '[COnDemandProvider] !aData');
                    return;
                }
                _super.call(this, {
                    mdsData: aData,
                    launchLocation: aLaunchLocation || 'search'
                });
                this.type = "ondemand";
            }
            COnDemandProvider.prototype.getTemplateAitUrl = function () {
                return this.templateAitUrl;
            };
            COnDemandProvider.prototype.checkTemplateAIT = function (xml) {
                if (!xml) {
                    hx.log('error', '[COnDemandProvider] FAIL!!! checkTemplateAIT : argu(xml) is null');
                    return false;
                }
                //check hbbtv application
                var type = xml.querySelector('OtherApp').textContent;
                //check version
                var major = xml.querySelector('versionMajor').textContent;
                var minor = xml.querySelector('versionMinor').textContent;
                var micro = xml.querySelector('versionMicro').textContent;
                var version = (major + minor + micro) * 1;
                if (version <= 131 && type === 'application/vnd.hbbtv.xhtml+xml') {
                    return true;
                }
                hx.log('debug', '[COnDemandProvider] FAIL!!! checkTemplateAIT : [' + version + '][' + type + '] / ' + this.name);
                return false;
            };
            COnDemandProvider.prototype.getDuration = function () {
                return this.ondemandDuration || 0;
            };
            COnDemandProvider.prototype.getAvailableEndTime = function () {
                return this.availableEndTime || 0;
            };
            COnDemandProvider.prototype.getAddition = function () {
                return this.secondary || '';
            };
            COnDemandProvider.prototype.getType = function () {
                return this.type;
            };
            COnDemandProvider.prototype.getContentOwningLogo = function (param) {
                // updateCb : a callback to update UI(ContentOwningLogo) after receiving the image from server.
                // width : a width of needed thumbnail.
                var a = this;
                if (!param) {
                    hx.log('error', '[COnDemandProvider] getContentOwningLogo fail!!! coz param is null');
                    return '';
                }
                var updateCb = param.updateCb || null;
                var width = param.width || a.DEFAULT_LOGO_WIDTH;
                if (!a.getServiceIDRef()) {
                    hx.log('error', '[COnDemandProvider] getContentOwningLogo fail!!! coz serviceIDRef is null');
                    if (updateCb) {
                        updateCb('');
                    }
                    return '';
                }
                if (hx.il.checkMdsUrl(a.contentOwningLogoUrl, width)) {
                    return a.contentOwningLogoUrl;
                }
                else {
                    hx.svc.IP_EPG.getContentOwningLogo(a.getServiceIDRef(), width).then(function (data) {
                        a.contentOwningLogoUrl = data || '';
                        if (updateCb) {
                            updateCb(a.contentOwningLogoUrl);
                        }
                    }, function () {
                        if (updateCb) {
                            updateCb('');
                        }
                    });
                }
            };
            COnDemandProvider.prototype.getDefaultThumbnail = function () {
                return __MODEL__.TDefaultThumbnailList.FREEVIEW; //freeivew image
            };
            COnDemandProvider.prototype.isPlayAvailable = function () {
                var a = this;
                var bAvailable = true;
                var timeCurrent = Math.floor(new Date().getTime() / 1000);
                if (a.availableStartTime >= timeCurrent || a.availableEndTime < timeCurrent) {
                    hx.log('debug', '[COnDemandProvider] !isPlayAvailable');
                    bAvailable = false;
                }
                return bAvailable;
            };
            return COnDemandProvider;
        })(CProgrammeProvider);
        mProvider.COnDemandProvider = COnDemandProvider;
        var fnStringReplaceAll = function (sourceString) {
            var convertedString;
            if (sourceString) {
                convertedString = sourceString.replace(/\n/g, '<br>');
            }
            return convertedString;
        };
        var isSameProgrammeVsProgramme = function (programmeItem1, programmeItem2) {
            return (programmeItem1.getId() === programmeItem2.getId() && programmeItem1.getChannelId() === programmeItem2.getChannelId());
        };
        var isSameProgrammeVsRecording = function (programmeItem, recordingItem) {
            if ((programmeItem.getChannelId() === recordingItem.getChannelId()) && recordingItem.isRecordingOnGoing() && programmeItem.isOnAir()) {
                return true;
            }
            return !!(hx.il.getCompareID(programmeItem) === hx.il.getCompareID(recordingItem));
        };
        var fnGetSeriesID = function (aGroupCRID) {
            var series_id = aGroupCRID.split("/");
            series_id.shift();
            series_id = series_id.join("");
            var idxCut = series_id.lastIndexOf("#");
            if (idxCut > -1) {
                series_id = series_id.substr(0, idxCut);
            }
            return '/' + series_id;
        };
        var isSameProgrammeVsScheduled = function (programmeItem, scheduledItem) {
            if (scheduledItem.isTimebased()) {
                var startTime = programmeItem.getStartTime().getTime();
                var endTime = startTime + programmeItem._item.duration * 1000;
                var scheduledStartTime = scheduledItem.getStartTime().getTime();
                var scheduledEndTime = scheduledStartTime + scheduledItem.getOipfScheduledInstance().duration * 1000;
                var ret = false;
                if (((scheduledStartTime < startTime && startTime < scheduledEndTime) || (scheduledStartTime < endTime && endTime < scheduledEndTime) || (scheduledStartTime > startTime && endTime > scheduledEndTime)) && (programmeItem.getChannelId() === scheduledItem.getChannelId())) {
                    if (scheduledItem.isStreaming() === true) {
                        hx.log('dsrvDetailInfo2', '[isSameProgrammeVsScheduled] Streaming Item');
                        ret = false;
                    }
                    else {
                        hx.log('dsrvDetailInfo2', '[isSameProgrammeVsScheduled] same scheduled Item');
                        ret = true;
                    }
                }
                else {
                    ret = false;
                }
                return ret;
            }
            if (scheduledItem.isScheduledAsSeries() && !(ENV.listOP.UKDTT === ENV.op)) {
                //hx.log('dsrvDetailInfo2', '[isSameProgrammeVsScheduled] scheduled(series) - programmeItem : ' + programmeItem);
                //hx.log('dsrvDetailInfo2', '[isSameProgrammeVsScheduled] scheduled(series) - scheduledItem : ' + scheduledItem);
                if (programmeItem.getChannelId() === scheduledItem.getChannelId()) {
                    if (scheduledItem.getSeriesId() > 0 && programmeItem.getSeriesId() === scheduledItem.getSeriesId()) {
                        return true;
                    }
                    else if (((scheduledItem.getSeriesId() === "" || scheduledItem.getSeriesId() === undefined) && scheduledItem.getGroupCRIDs().length > 0) && ((programmeItem.getSeriesId() === "" || programmeItem.getSeriesId() === undefined) && programmeItem.getGroupCRIDs().length > 0)) {
                        var i, j;
                        for (i = 0; i < scheduledItem.getGroupCRIDs().length; i += 1) {
                            for (j = 0; j < programmeItem.getGroupCRIDs().length; j += 1) {
                                if (programmeItem.getGroupCRIDs().item(j) === scheduledItem.getGroupCRIDs().item(i)) {
                                    return true;
                                }
                            }
                        }
                    }
                    else {
                        if ((ENV.listOP.AUSDTT === ENV.op)) {
                            if (scheduledItem.getGroupCRIDs().length === 0 && programmeItem.getGroupCRIDs().length === 0) {
                                if (scheduledItem.getTitle() === programmeItem.getTitle()) {
                                    return true;
                                }
                            }
                        }
                    }
                }
                return false;
            }
            //hx.log('dsrvDetailInfo2', '[isSameProgrammeVsScheduled] scheduled(not series) - programmeItem : ' + programmeItem);
            //hx.log('dsrvDetailInfo2', '[isSameProgrammeVsScheduled] scheduled(not series) - scheduledItem : ' + scheduledItem);
            return (programmeItem.getId() === scheduledItem.getProgrammeID() && programmeItem.getChannelId() === scheduledItem.getChannelId());
        };
        var isSameRecordingVsRecording = function (recordingItem1, recordingItem2) {
            return recordingItem1.getId() === recordingItem2.getId();
        };
        var isSameRecordingVsScheduled = function (recordingItem, scheduledItem) {
            if (scheduledItem.isTimebased()) {
                return recordingItem.getStartTime() === scheduledItem.getStartTime() && recordingItem.getChannelId() === scheduledItem.getChannelId();
            }
            return recordingItem.getProgrammeID() === scheduledItem.getProgrammeID();
        };
        var isSameScheduledVsScheduled = function (scheduledItem1, scheduledItem2) {
            if (scheduledItem1.isTimebased()) {
                return (scheduledItem1.getStartTime().getTime() === scheduledItem2.getStartTime().getTime()) && (scheduledItem1.getChannelId() === scheduledItem2.getChannelId());
            }
            return scheduledItem1.getProgrammeID() === scheduledItem2.getProgrammeID();
        };
        var _oipfDef = {
            TScheduledFactoryType: {
                ERECORDING: 0,
                EREMIND: 1,
                EHTTP_LIVE_STREAMING: 4,
                ESATRECORD: 8,
                ESERIES_RECORDING: 10
            }
        };
        var cPdeOndemandItem = (function (_super) {
            __extends(cPdeOndemandItem, _super);
            function cPdeOndemandItem(item) {
                _super.call(this);
                this._item = item || {};
            }
            cPdeOndemandItem.prototype.getSourceItem = function () {
                return this._item;
            };
            cPdeOndemandItem.prototype.toString = function () {
                var ret = 'PDE ITEM: type=' + this.getType() + ' id=' + this.getId() + ' seriesId=' + this.getSeriesId() + ' programmeId=' + this.getProgrammeID();
                ret += '\nPDE ITEM: title=' + this.getTitle() + ' channelId=' + this.getChannelId() + ' channelName=' + this.getChannelName();
                return ret;
            };
            cPdeOndemandItem.prototype.getId = function () {
                return this._item.getProgrammeID();
            };
            cPdeOndemandItem.prototype.getStartTime = function () {
                return this._item.getStartTime() ? new Date(this._item.getStartTime() * 1000) : null;
            };
            cPdeOndemandItem.prototype.getEndTime = function () {
                if (this._item.startTime && this._item.duration) {
                    var startTime = this._item.startTime * 1000;
                    var duration = this._item.duration * 1000;
                    return new Date(startTime + duration);
                }
                else {
                    return null;
                }
            };
            cPdeOndemandItem.prototype.getTitle = function () {
                return this._item.getName() || '';
            };
            cPdeOndemandItem.prototype.getSecondary = function () {
                return this._item.getSecondary() || '';
            };
            cPdeOndemandItem.prototype.getThumbnailUrl = function (updateCb) {
                return this._item ? this._item.getThumbnail({
                    updateCb: updateCb,
                    width: this.getThumbWidth()
                }) : '';
            };
            cPdeOndemandItem.prototype.getLogoUrl = function (updateCb) {
                return this._item ? this._item.getContentOwningLogo({
                    updateCb: updateCb,
                    width: this.getLogoWidth()
                }) : '';
            };
            cPdeOndemandItem.prototype.getChannelInstance = function () {
                return null;
            };
            cPdeOndemandItem.prototype.getChannelId = function () {
                return '';
            };
            cPdeOndemandItem.prototype.getChannelNo = function () {
                return '';
            };
            cPdeOndemandItem.prototype.getChannelName = function () {
                return '';
            };
            cPdeOndemandItem.prototype.getLongDescription = function () {
                return this._item.getLongDescription() || '';
            };
            cPdeOndemandItem.prototype.getDescription = function () {
                return '';
            };
            cPdeOndemandItem.prototype.getSeriesId = function () {
                return this._item.getProgrammeCRID() || null;
            };
            cPdeOndemandItem.prototype.getSeasonNo = function () {
                return null;
            };
            cPdeOndemandItem.prototype.getEpisodeNo = function () {
                return null;
            };
            cPdeOndemandItem.prototype.getGenreList = function () {
                return this._item.getGenre();
            };
            cPdeOndemandItem.prototype.getParentalRating = function () {
                var parentalRatings = this._item.getParentalRatings();
                if (!parentalRatings) {
                    return -1;
                }
                return (parentalRatings[0] ? parentalRatings[0].value : -1);
            };
            cPdeOndemandItem.prototype.getCasIDs = function () {
                return [];
            };
            cPdeOndemandItem.prototype.getLastPlayPosition = function () {
                return null;
            };
            cPdeOndemandItem.prototype.getProgrammeID = function () {
                return this._item.getProgrammeID() || null;
            };
            cPdeOndemandItem.prototype.hasSchedule = function () {
                return false;
            };
            cPdeOndemandItem.prototype.hasSeriesSchedule = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isReminder = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isOnAir = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isPast = function () {
                return false;
            };
            cPdeOndemandItem.prototype.getGroupCRIDs = function () {
                return [];
            };
            cPdeOndemandItem.prototype.getProgrammeCRID = function () {
                return null;
            };
            cPdeOndemandItem.prototype.getRecommendationCRID = function () {
                return null;
            };
            cPdeOndemandItem.prototype.hasGroupCRIDs = function () {
                return false;
            };
            cPdeOndemandItem.prototype.getKeywordQuery = function () {
                return '';
            };
            cPdeOndemandItem.prototype.isSeries = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isPlayable = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isNew = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isHD = function () {
                return this._item.isHd();
            };
            cPdeOndemandItem.prototype.isAD = function () {
                return this._item.isAd();
            };
            cPdeOndemandItem.prototype.isDRM = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isLocked = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isKept = function () {
                // TODO: implement this
                return false;
            };
            cPdeOndemandItem.prototype.setKeep = function (aKeep) {
                // TODO: implement this
            };
            cPdeOndemandItem.prototype.isWillbedeleted = function () {
                // TODO: implement this
                return false;
            };
            cPdeOndemandItem.prototype.isRecordingOnGoing = function () {
                return false;
            };
            cPdeOndemandItem.prototype.hasRecording = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isTimebased = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isSeriesButNotRecoredAsSeries = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isScrambled = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isStreaming = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isDeeplink = function () {
                return false;
            };
            cPdeOndemandItem.prototype.isMultiAudio = function () {
                return false;
            };
            cPdeOndemandItem.prototype.getDeeplinkAppName = function () {
                return this._item.getName() || '';
            };
            cPdeOndemandItem.prototype.getDeeplinkUrl = function () {
                return this._item.getDeeplinkUrl() || '';
            };
            cPdeOndemandItem.prototype.getSubtitleType = function () {
                return null;
            };
            cPdeOndemandItem.prototype.getSubtitleTypes = function () {
                return [];
            };
            cPdeOndemandItem.prototype.hasTeletext = function () {
                return false;
            };
            cPdeOndemandItem.prototype.hasGuidance = function () {
                return this._item.hasGuidance() || false;
            };
            cPdeOndemandItem.prototype.getGuidance = function () {
                return this._item.getGuidance() || null;
            };
            cPdeOndemandItem.prototype.getHDLinkage = function () {
                return null;
            };
            cPdeOndemandItem.prototype.isSame = function (p) {
                return false;
            };
            cPdeOndemandItem.prototype.getDuration = function () {
                return this._item.getDuration() || 0;
            };
            cPdeOndemandItem.prototype.getAvailableEndTime = function () {
                return this._item.getAvailableEndTime() || 0;
            };
            return cPdeOndemandItem;
        })(__MODEL__.IPdeOndemandItem);
        mProvider.cPdeOndemandItem = cPdeOndemandItem;
        var cPdeProgrammeItem = (function (_super) {
            __extends(cPdeProgrammeItem, _super);
            function cPdeProgrammeItem(programmeItem) {
                _super.call(this);
                this._thumbnailUrl = '';
                this._logoUrl = '';
                this._item = programmeItem || new CProgrammeProvider(null);
            }
            cPdeProgrammeItem.prototype.getSourceItem = function () {
                return this._item;
            };
            cPdeProgrammeItem.prototype.getPluginObject = function () {
                return this._item.getPluginObject();
            };
            cPdeProgrammeItem.prototype.getMdsData = function () {
                return this._item.getMdsData();
            };
            cPdeProgrammeItem.prototype.setMdsData = function (aParam) {
                this._item.setMdsData(aParam);
            };
            cPdeProgrammeItem.prototype.toString = function () {
                var ret = 'PDE ITEM: type=' + this.getType() + ' id=' + this.getId() + ' seriesId=' + this.getSeriesId() + ' programmeId=' + this.getProgrammeID();
                ret += '\nPDE ITEM: title=' + this.getTitle() + ' channelId=' + this.getChannelId() + ' channelName=' + this.getChannelName();
                return ret;
            };
            cPdeProgrammeItem.prototype.getId = function () {
                return this._item.getProgrammeID() || this._item.getProgrammeCridMds();
            };
            cPdeProgrammeItem.prototype.getStartTime = function () {
                return this._item.getStartTime() ? new Date(this._item.getStartTime() * 1000) : null;
            };
            cPdeProgrammeItem.prototype.getEndTime = function () {
                if (this._item.getStartTime() && this._item.getDuration()) {
                    var startTime = this._item.getStartTime() * 1000;
                    var duration = this._item.getDuration() * 1000;
                    return new Date(startTime + duration);
                }
                else {
                    return null;
                }
            };
            cPdeProgrammeItem.prototype.getTitle = function () {
                return this._item.getName();
            };
            cPdeProgrammeItem.prototype.getSecondary = function () {
                return this._item.getSecondary() || '';
            };
            cPdeProgrammeItem.prototype.getThumbnailUrl = function (updateCb) {
                var a = this;
                if (!updateCb) {
                    hx.log('error', '[cPdeProgrammeItem] getThumbnailUrl fail!!! coz updateCb is null');
                    return a._thumbnailUrl || a.getDefaultThumbnail();
                }
                if (!a._item.requestThumbUrl) {
                    hx.log('error', '[cPdeProgrammeItem] getThumbnailUrl fail!!! coz !a._item.requestThumbUrl');
                    return a._thumbnailUrl || a.getDefaultThumbnail();
                }
                if (hx.il.checkMdsUrl(a._thumbnailUrl, a.getThumbWidth())) {
                    return a._thumbnailUrl;
                }
                hx.svc.IP_EPG.getThumbnail(a._item.requestThumbUrl, a.getThumbWidth(), { "content-type": a._item.requestImgContentType }).then(function (data) {
                    if (data) {
                        updateCb(a._thumbnailUrl = data);
                    }
                    else {
                        updateCb(a.getDefaultThumbnail());
                    }
                }, function () {
                    updateCb(a.getDefaultThumbnail());
                });
            };
            cPdeProgrammeItem.prototype.getDefaultThumbnail = function () {
                return this.getGenreImage() || __MODEL__.TDefaultThumbnailList.DEFAULT;
            };
            cPdeProgrammeItem.prototype.getGenreImage = function () {
                return this._item.getGenreImage();
            };
            cPdeProgrammeItem.prototype.getChannelInstance = function () {
                var channel = this._item.channel || {};
                return channel;
            };
            cPdeProgrammeItem.prototype.getChannelId = function () {
                var channel = this._item.channel || {};
                return channel.ccid;
            };
            cPdeProgrammeItem.prototype.getServiceId = function () {
                var channel = this._item.channel || {};
                return channel.sid;
            };
            cPdeProgrammeItem.prototype.getChannelNo = function () {
                var channel = this._item.channel || {};
                return channel.majorChannel;
            };
            cPdeProgrammeItem.prototype.getChannelName = function () {
                var channel = this._item.channel || {};
                return channel.name;
            };
            cPdeProgrammeItem.prototype.getLogoUrl = function (updateCb) {
                var a = this;
                if (!updateCb) {
                    hx.log('error', '[cPdeProgrammeItem] getLogoUrl fail!!! coz updateCb is null');
                    return a._logoUrl;
                }
                if (!a.getServiceId()) {
                    hx.log('error', '[cPdeProgrammeItem] getLogoUrl fail!!! coz !a.getServiceId()');
                    return a._logoUrl;
                }
                if (hx.il.checkMdsUrl(a._logoUrl, a.getLogoWidth())) {
                    return a._logoUrl;
                }
                hx.svc.IP_EPG.getChannelLogo(a.getServiceId(), a.getLogoWidth()).then(function (data) {
                    if (data) {
                        updateCb(a._logoUrl = data);
                    }
                });
            };
            cPdeProgrammeItem.prototype.getLongDescription = function () {
                return fnStringReplaceAll(this._item.getLongDescription());
            };
            cPdeProgrammeItem.prototype.getDescription = function () {
                return fnStringReplaceAll(this._item.getShortDescription());
            };
            cPdeProgrammeItem.prototype.getSeriesId = function () {
                return this._item.groupCRIDs[0];
            };
            cPdeProgrammeItem.prototype.getSeasonNo = function () {
                var seasonNo = this._item.season;
                //getField("season");
                return seasonNo && seasonNo > 0 ? seasonNo : null;
            };
            cPdeProgrammeItem.prototype.getEpisodeNo = function () {
                var episodeNo = this._item.episode;
                return episodeNo && episodeNo > 0 ? episodeNo : null;
            };
            cPdeProgrammeItem.prototype.getGenreList = function () {
                return this._item.getGenre();
            };
            cPdeProgrammeItem.prototype.getParentalRating = function () {
                var parentalRatings = this._item.getParentalRatings();
                if (!parentalRatings) {
                    return -1;
                }
                return (parentalRatings[0] ? parentalRatings[0].value : -1);
            };
            cPdeProgrammeItem.prototype.getCasIDs = function () {
                var i, length, channel;
                channel = this._item.channel || {};
                return channel.casIDs || [];
            };
            cPdeProgrammeItem.prototype.getLastPlayPosition = function () {
                return 0;
            };
            cPdeProgrammeItem.prototype.getProgrammeID = function () {
                return this._item.getProgrammeID();
            };
            cPdeProgrammeItem.prototype.hasSchedule = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.hasSeriesSchedule = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.isReminder = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.isOnAir = function () {
                var startTime = this.getStartTime();
                var endTime = this.getEndTime();
                if (!startTime || !endTime) {
                    return false;
                }
                var now = (new Date().getTime());
                return (startTime.getTime() < now && endTime.getTime() > now);
            };
            cPdeProgrammeItem.prototype.isPast = function () {
                var ret = false;
                var isTsr = hx.svc.TSR.isTsrOngoing();
                var now = (new Date().getTime());
                var startTime = this.getStartTime().getTime();
                var endTime = this.getEndTime().getTime();
                if (startTime < now && endTime > now) {
                    if (isTsr) {
                        ret = true;
                    }
                }
                else if (startTime < now && endTime < now) {
                    ret = true;
                }
                return ret;
            };
            cPdeProgrammeItem.prototype.getGroupCRIDs = function () {
                return this._item.getGroupCRIDs();
            };
            cPdeProgrammeItem.prototype.getProgrammeCRID = function () {
                return this._item.getProgrammeCRID();
            };
            cPdeProgrammeItem.prototype.getRecommendationCRID = function () {
                return this._item.getRecommendationCRID();
            };
            cPdeProgrammeItem.prototype.hasGroupCRIDs = function () {
                if (this._item && this._item.groupCRIDs && this._item.groupCRIDs.length && this._item.groupCRIDs.length > 0) {
                    return true;
                }
                return false;
            };
            cPdeProgrammeItem.prototype.getKeywordQuery = function () {
                var groupCRIDs, i, keyword;
                var groupCRID_datapart;
                if (hx.config.useRfCRID === true) {
                    if (this.hasGroupCRIDs() === true) {
                        groupCRIDs = this.getGroupCRIDs();
                        for (i = 0; i < groupCRIDs.length; i += 1) {
                            groupCRID_datapart = fnGetSeriesID(groupCRIDs[i]);
                            if (i === 0) {
                                keyword = hx.util.format("keyword %% sCRID:%{0}, ", groupCRID_datapart);
                            }
                            else {
                                keyword = keyword + hx.util.format(" OR keyword %% sCRID:%{0}, ", groupCRID_datapart);
                            }
                        }
                    }
                    return keyword;
                }
                return '';
            };
            cPdeProgrammeItem.prototype.setDummyGroup = function () {
                this._dummyGroup = true;
            };
            cPdeProgrammeItem.prototype.isSeries = function () {
                var isSeries = false, i = 0, GroupCRIDs, crid;
                if (hx.config.useRfCRID === true && this.hasGroupCRIDs() === true) {
                    isSeries = true;
                }
                if (hx.config.useRfCRID === true && this._dummyGroup === true) {
                    return true;
                }
                return isSeries;
            };
            cPdeProgrammeItem.prototype.isPlayable = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.isNew = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.isHD = function () {
                return this._item.isHd();
            };
            cPdeProgrammeItem.prototype.isAD = function () {
                return this._item.isAd();
            };
            cPdeProgrammeItem.prototype.isLocked = function () {
                return this._item.isLocked();
            };
            cPdeProgrammeItem.prototype.isKept = function () {
                // TODO: implement this
                return false;
            };
            cPdeProgrammeItem.prototype.setKeep = function (aKeep) {
                // TODO: implement this
            };
            cPdeProgrammeItem.prototype.isWillbedeleted = function () {
                // TODO: implement this
                return false;
            };
            cPdeProgrammeItem.prototype.isRecordingOnGoing = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.hasRecording = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.isTimebased = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.isSeriesButNotRecoredAsSeries = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.isScrambled = function () {
                return false;
            };
            cPdeProgrammeItem.prototype.isStreaming = function () {
                var ret = false;
                if (this._oipfScheduledObj.factoryType === _oipfDef.TScheduledFactoryType.EHTTP_LIVE_STREAMING) {
                    ret = true;
                }
                return ret;
            };
            cPdeProgrammeItem.prototype.isDeeplink = function () {
                var ret = false, currTime = 0, date;
                if ((ENV.listOP.AUSDTT === ENV.op) || (ENV.listOP.UKDTT === ENV.op)) {
                    if (this.isPast()) {
                        var expireDate = this._item.getPluginObject().mediaDeeplinkExpireDate;
                        if (expireDate) {
                            date = new Date();
                            currTime = date.toSeconds();
                            if (currTime < expireDate) {
                                ret = true;
                            }
                        }
                    }
                }
                //return true;    // for emul debug
                return ret;
            };
            cPdeProgrammeItem.prototype.isMultiAudio = function () {
                return this._item.isMultiAudio();
            };
            cPdeProgrammeItem.prototype.getDeeplinkAppName = function () {
                return this.getTitle();
            };
            cPdeProgrammeItem.prototype.getDeeplinkUrl = function () {
                return this._item.getDeeplinkUrl();
            };
            cPdeProgrammeItem.prototype.getAvailability = function () {
                return this._item.getAvailability();
            };
            cPdeProgrammeItem.prototype.getSubtitleType = function () {
                return this._item.getSubtitleType();
            };
            cPdeProgrammeItem.prototype.getSubtitleTypes = function () {
                return this._item.getSubtitleTypes();
            };
            cPdeProgrammeItem.prototype.hasTeletext = function () {
                if (this._item.channel) {
                    return this._item.channel.hasTeletext;
                }
                return false;
            };
            cPdeProgrammeItem.prototype.hasGuidance = function () {
                return this._item.hasGuidance();
            };
            cPdeProgrammeItem.prototype.getGuidance = function () {
                return this._item.getGuidance();
            };
            cPdeProgrammeItem.prototype.getHDLinkage = function () {
                return this._item.getHDLinkage();
            };
            cPdeProgrammeItem.prototype.isSame = function (b) {
                var _this = this;
                var matcherType;
                if (this instanceof cPdeProgrammeItem && b instanceof cPdeProgrammeItem) {
                    matcherType = 'pp';
                }
                else if (this instanceof cPdeProgrammeItem && b instanceof cPdeRecordingItem) {
                    matcherType = 'pr';
                }
                else if (this instanceof cPdeRecordingItem && b instanceof cPdeProgrammeItem) {
                    matcherType = 'rp';
                }
                else if (this instanceof cPdeProgrammeItem && b instanceof cPdeScheduledItem) {
                    matcherType = 'ps';
                }
                else if (this instanceof cPdeScheduledItem && b instanceof cPdeProgrammeItem) {
                    matcherType = 'sp';
                }
                else if (this instanceof cPdeRecordingItem && b instanceof cPdeRecordingItem) {
                    matcherType = 'rr';
                }
                else if (this instanceof cPdeRecordingItem && b instanceof cPdeScheduledItem) {
                    matcherType = 'rs';
                }
                else if (this instanceof cPdeScheduledItem && b instanceof cPdeRecordingItem) {
                    matcherType = 'sr';
                }
                else if (this instanceof cPdeScheduledItem && b instanceof cPdeScheduledItem) {
                    matcherType = 'ss';
                }
                else {
                    throw "INVALID PARAM";
                }
                var matcher = {
                    'pp': function () {
                        return isSameProgrammeVsProgramme(_this, b);
                    },
                    'pr': function () {
                        return isSameProgrammeVsRecording(_this, b);
                    },
                    'ps': function () {
                        return isSameProgrammeVsScheduled(_this, b);
                    },
                    'rp': function () {
                        return isSameProgrammeVsRecording(b, _this);
                    },
                    'sp': function () {
                        return isSameProgrammeVsScheduled(b, _this);
                    },
                    'rr': function () {
                        return isSameRecordingVsRecording(_this, b);
                    },
                    'rs': function () {
                        return isSameRecordingVsScheduled(_this, b);
                    },
                    'sr': function () {
                        return isSameRecordingVsScheduled(b, _this);
                    },
                    'ss': function () {
                        return isSameScheduledVsScheduled(_this, b);
                    }
                };
                return matcher[matcherType]();
            };
            return cPdeProgrammeItem;
        })(__MODEL__.IPdeProgrammeItem);
        mProvider.cPdeProgrammeItem = cPdeProgrammeItem;
        var cPdeRecordingItem = (function (_super) {
            __extends(cPdeRecordingItem, _super);
            function cPdeRecordingItem(prismRecordingItem) {
                _super.call(this);
                if (prismRecordingItem instanceof __MODEL__.COipfRecordingItem) {
                    this._prismRecordingItem = prismRecordingItem;
                }
                else {
                    this._prismRecordingItem = new __MODEL__.COipfRecordingItem(prismRecordingItem.pluginObject);
                }
            }
            cPdeRecordingItem.prototype.getSourceItem = function () {
                return this._prismRecordingItem;
            };
            cPdeRecordingItem.prototype.toString = function () {
                return 'PDE ITEM: type=' + this.getType() + ' id=' + this.getId() + ' seriesId=' + this.getSeriesId() + ' programmeId=' + this.getProgrammeID();
            };
            cPdeRecordingItem.prototype.getId = function () {
                return this._prismRecordingItem.getId();
            };
            cPdeRecordingItem.prototype.getStartTime = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return new Date(that._refProgrammeItem.getStartTime() * 1000);
                }
                return that._prismRecordingItem.getStartTime();
            };
            cPdeRecordingItem.prototype.getEndTime = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return new Date(that._refProgrammeItem.getEndTime() * 1000);
                }
                var duration = that._prismRecordingItem.getDuration();
                var startTime = that._prismRecordingItem.getStartTime();
                return new Date((startTime.getTime() + duration * 1000) + 30000);
            };
            cPdeRecordingItem.prototype.getTitle = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return that._refProgrammeItem.getName();
                }
                return that._prismRecordingItem.getTitle();
            };
            cPdeRecordingItem.prototype.getSecondary = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return that._refProgrammeItem.getSecondary();
                }
                return that.getDescription();
            };
            cPdeRecordingItem.prototype.getThumbnailUrl = function (updateCb) {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    if (!updateCb) {
                        hx.log('error', '[cPdeRecordingItem] getThumbnailUrl fail!!! coz updateCb is null');
                        return that._thumbnailUrl;
                    }
                    if (!that._refProgrammeItem.requestThumbUrl) {
                        hx.log('error', '[cPdeRecordingItem] getThumbnailUrl fail!!! coz !a._item.requestThumbUrl');
                        return that._thumbnailUrl;
                    }
                    if (hx.il.checkMdsUrl(that._thumbnailUrl, that.getThumbWidth())) {
                        return that._thumbnailUrl;
                    }
                    hx.svc.IP_EPG.getThumbnail(that._refProgrammeItem.requestThumbUrl, that.getThumbWidth(), { "content-type": that._refProgrammeItem.requestImgContentType }).then(function (data) {
                        if (data) {
                            updateCb(that._thumbnailUrl = data);
                        }
                    }, function () {
                    });
                }
                var thumbnailUrl = that._prismRecordingItem.getThumbnailUrl();
                if (!thumbnailUrl) {
                    if (updateCb) {
                        setTimeout(function () {
                            hx.log('error', 'update Thumbnail for recording item after setTimeout 1000');
                            updateCb(that._prismRecordingItem.getThumbnailUrl());
                        }, 1000);
                    }
                    thumbnailUrl = that._prismRecordingItem.isBroken() ? __MODEL__.TDefaultThumbnailList.REC_FAIL : __MODEL__.TDefaultThumbnailList.RECORDING;
                }
                return thumbnailUrl;
            };
            cPdeRecordingItem.prototype.getChannelInstance = function () {
                return this._prismRecordingItem.getChannelInstance();
            };
            cPdeRecordingItem.prototype.getChannelId = function () {
                return this._prismRecordingItem.getChannelId();
            };
            cPdeRecordingItem.prototype.getServiceId = function () {
                return this._prismRecordingItem.getServiceId();
            };
            cPdeRecordingItem.prototype.getChannelNo = function () {
                return this._prismRecordingItem.getChannelNo();
            };
            cPdeRecordingItem.prototype.getChannelName = function () {
                return this._prismRecordingItem.getChannelName();
            };
            cPdeRecordingItem.prototype.getLogoUrl = function (updateCb) {
                var that = this;
                var logoURL = that._prismRecordingItem.getLogoUrl(that.getLogoWidth());
                if (!logoURL && updateCb) {
                    hx.svc.IP_EPG.getChannelLogo(that.getServiceId(), that.getLogoWidth()).then(function (data) {
                        updateCb(data);
                    });
                }
                return logoURL;
            };
            cPdeRecordingItem.prototype.getLongDescription = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return that._refProgrammeItem.getLongDescription();
                }
                return fnStringReplaceAll(that._prismRecordingItem.getLongDescription());
            };
            cPdeRecordingItem.prototype.getDescription = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return that._refProgrammeItem.getShortDescription();
                }
                return fnStringReplaceAll(that._prismRecordingItem.getShortDescription());
            };
            cPdeRecordingItem.prototype.getSeriesId = function () {
                return this._prismRecordingItem.getSeriesId();
            };
            cPdeRecordingItem.prototype.getGenreList = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return that._refProgrammeItem.getGenre();
                }
                return that._prismRecordingItem.getGenre();
            };
            cPdeRecordingItem.prototype.getParentalRating = function () {
                var that = this;
                var parentalRating;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    parentalRating = that._refProgrammeItem.getParentalRatings();
                }
                else {
                    parentalRating = that._prismRecordingItem.getParentalRatings();
                }
                if (!parentalRating) {
                    return -1;
                }
                parentalRating = parentalRating[0];
                return (parentalRating ? parentalRating.value : -1);
            };
            cPdeRecordingItem.prototype.getCasIDs = function () {
                return this._prismRecordingItem.getCasIDs();
            };
            cPdeRecordingItem.prototype.getLastPlayPosition = function () {
                return this._prismRecordingItem.getLastPlayPosition();
            };
            cPdeRecordingItem.prototype.getOipfRecordingInstance = function () {
                return this._prismRecordingItem.getInstance();
            };
            cPdeRecordingItem.prototype.getProgrammeID = function () {
                return this._prismRecordingItem.getProgrammeID();
            };
            cPdeRecordingItem.prototype.hasSchedule = function () {
                return false;
            };
            cPdeRecordingItem.prototype.hasSeriesSchedule = function () {
                return false;
            };
            cPdeRecordingItem.prototype.isReminder = function () {
                return false;
            };
            cPdeRecordingItem.prototype.isOnAir = function () {
                return this._prismRecordingItem.isRecordingOnGoing();
            };
            cPdeRecordingItem.prototype.isPast = function () {
                return false;
            };
            cPdeRecordingItem.prototype.isSeries = function () {
                return this._prismRecordingItem.isSeries();
            };
            cPdeRecordingItem.prototype.isPendingSeriesSchedule = function () {
                var that = this;
                var endPadding = that._prismRecordingItem.getEndPadding();
                var endTime = new Date((that.getEndTime() + endPadding) * 1000);
                var now = new Date();
                return ((endTime.getTime() < now.getTime()) && (that._prismRecordingItem.isScheduledAsSeries()));
            };
            cPdeRecordingItem.prototype.isPlayable = function () {
                return true;
            };
            cPdeRecordingItem.prototype.isNew = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return false;
                }
                return that._prismRecordingItem.isNew();
            };
            cPdeRecordingItem.prototype.wasWatching = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return false;
                }
                return that._prismRecordingItem.wasWatching();
            };
            cPdeRecordingItem.prototype.isHD = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return that._refProgrammeItem.isHd();
                }
                return that._prismRecordingItem.isHd();
            };
            cPdeRecordingItem.prototype.isAD = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return that._refProgrammeItem.isAd();
                }
                return that._prismRecordingItem.isAd();
            };
            cPdeRecordingItem.prototype.isEncrypted = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return false;
                }
                return that._prismRecordingItem.isEncrypted();
            };
            cPdeRecordingItem.prototype.isDRM = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return false;
                }
                return that._prismRecordingItem.isNoCopy();
            };
            cPdeRecordingItem.prototype.isLocked = function () {
                return this._prismRecordingItem.isLocked();
            };
            cPdeRecordingItem.prototype.isKept = function () {
                return this._prismRecordingItem.isKept();
            };
            cPdeRecordingItem.prototype.setKeep = function (aKeep) {
                this._prismRecordingItem.setKeep(aKeep);
            };
            cPdeRecordingItem.prototype.isWillbedeleted = function () {
                return this._prismRecordingItem.isWillbedeleted();
            };
            cPdeRecordingItem.prototype.isRecordingOnGoing = function () {
                return this._prismRecordingItem.isRecordingOnGoing();
            };
            cPdeRecordingItem.prototype.hasRecording = function () {
                return true;
            };
            cPdeRecordingItem.prototype.isTimebased = function () {
                return this._prismRecordingItem.getProgrammeID() ? false : true;
            };
            cPdeRecordingItem.prototype.isSeriesButNotRecoredAsSeries = function () {
                var that = this;
                return that._prismRecordingItem.isSeries() && !that._prismRecordingItem.isScheduledAsSeries();
            };
            cPdeRecordingItem.prototype.isScheduledAsSeries = function () {
                return this._prismRecordingItem.isScheduledAsSeries();
            };
            cPdeRecordingItem.prototype.isScrambled = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return false;
                }
                return that._prismRecordingItem.isScrambled();
            };
            cPdeRecordingItem.prototype.isStreaming = function () {
                var ret = false;
                return ret;
            };
            cPdeRecordingItem.prototype.isMultiAudio = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return that._refProgrammeItem.isMultiAudio();
                }
                if (that._prismRecordingItem.getAudioType() === 4) {
                    return true;
                }
                return false;
            };
            cPdeRecordingItem.prototype.hasGroupCRIDs = function () {
                if (this.getGroupCRIDs().length > 0) {
                    return true;
                }
                return false;
            };
            cPdeRecordingItem.prototype.getKeywordQuery = function () {
                var that = this;
                var groupCRIDs, i, keyword;
                var groupCRID_datapart;
                if (hx.config.useRfCRID === true) {
                    if (that.hasGroupCRIDs() === true) {
                        groupCRIDs = that.getGroupCRIDs();
                        for (i = 0; i < groupCRIDs.length; i += 1) {
                            groupCRID_datapart = fnGetSeriesID(groupCRIDs[i]);
                            if (i === 0) {
                                keyword = hx.util.format("keyword %% sCRID:%{0}, ", groupCRID_datapart);
                            }
                            else {
                                keyword = keyword + hx.util.format(" OR keyword %% sCRID:%{0}, ", groupCRID_datapart);
                            }
                        }
                    }
                    return keyword;
                }
            };
            cPdeRecordingItem.prototype.getGroupCRIDs = function () {
                return this._prismRecordingItem.getGroupCRIDs();
            };
            cPdeRecordingItem.prototype.getProgrammeCRID = function () {
                return this._prismRecordingItem.getProgrammeCRID();
            };
            cPdeRecordingItem.prototype.getRecommendationCRID = function () {
                var that = this;
                if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === 2 /* EAvailable */)) {
                    return that._refProgrammeItem.getRecommendationCRID();
                }
                return false;
                //return this._prismRecordingItem.getRecommendationCRID();
            };
            cPdeRecordingItem.prototype.setRefProgrammeItem = function (refProgrammeItem) {
                this._refProgrammeItem = refProgrammeItem;
            };
            cPdeRecordingItem.prototype.getRefProgrammeItem = function () {
                if (this._refProgrammeItem) {
                    return this._refProgrammeItem;
                }
                return null;
            };
            cPdeRecordingItem.prototype.getEventIDFromProgrammeID = function () {
                var eventID = null, tmp = null;
                var programmeID = this.getProgrammeID();
                if (programmeID) {
                    tmp = programmeID.split(';');
                    if (tmp.length > 1) {
                        eventID = parseInt(tmp[1], 16).toString(10);
                    }
                }
                return eventID;
            };
            // for Deeplink
            cPdeRecordingItem.prototype.isDeeplink = function () {
                var ret = false;
                return ret;
            };
            cPdeRecordingItem.prototype.getDeeplinkAppName = function () {
                var that = this;
                var ret = null;
                if (that._refProgrammeItem) {
                    ret = that._refProgrammeItem.getName();
                }
                return ret;
            };
            cPdeRecordingItem.prototype.getDeeplinkUrl = function () {
                var ret = null;
                if (this._refProgrammeItem) {
                    ret = this._refProgrammeItem.getDeeplinkUrl();
                }
                return ret;
            };
            cPdeRecordingItem.prototype.getSubtitleType = function () {
                var type = 0;
                return type;
            };
            cPdeRecordingItem.prototype.getSubtitleTypes = function () {
                return this._prismRecordingItem.getSubtitleTypes();
            };
            cPdeRecordingItem.prototype.hasTeletext = function () {
                var channel;
                channel = this._prismRecordingItem.getChannelInstance();
                if (!channel) {
                    return false;
                }
                return channel.hasTeletext;
            };
            cPdeRecordingItem.prototype.setEpisodeLock = function (param) {
                var lock = param, oipfRecording;
                oipfRecording = this._prismRecordingItem.getInstance();
                oipfRecording.isManualLocked = lock;
            };
            cPdeRecordingItem.prototype.hasGuidance = function () {
                var that = this;
                //if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                //    return that._refProgrammeItem.hasGuidance();
                //}
                return that._prismRecordingItem.hasGuidance();
            };
            cPdeRecordingItem.prototype.getGuidance = function () {
                var that = this;
                //if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                //    return that._refProgrammeItem.getGuidance();
                //}
                return that._prismRecordingItem.getGuidance();
            };
            cPdeRecordingItem.prototype.getHDLinkage = function () {
                return false;
            };
            cPdeRecordingItem.prototype.isSame = function (b) {
                var _this = this;
                var matcherType;
                if (this instanceof cPdeProgrammeItem && b instanceof cPdeProgrammeItem) {
                    matcherType = 'pp';
                }
                else if (this instanceof cPdeProgrammeItem && b instanceof cPdeRecordingItem) {
                    matcherType = 'pr';
                }
                else if (this instanceof cPdeRecordingItem && b instanceof cPdeProgrammeItem) {
                    matcherType = 'rp';
                }
                else if (this instanceof cPdeProgrammeItem && b instanceof cPdeScheduledItem) {
                    matcherType = 'ps';
                }
                else if (this instanceof cPdeScheduledItem && b instanceof cPdeProgrammeItem) {
                    matcherType = 'sp';
                }
                else if (this instanceof cPdeRecordingItem && b instanceof cPdeRecordingItem) {
                    matcherType = 'rr';
                }
                else if (this instanceof cPdeRecordingItem && b instanceof cPdeScheduledItem) {
                    matcherType = 'rs';
                }
                else if (this instanceof cPdeScheduledItem && b instanceof cPdeRecordingItem) {
                    matcherType = 'sr';
                }
                else if (this instanceof cPdeScheduledItem && b instanceof cPdeScheduledItem) {
                    matcherType = 'ss';
                }
                else {
                    throw "INVALID PARAM";
                }
                var matcher = {
                    'pp': function () {
                        return isSameProgrammeVsProgramme(_this, b);
                    },
                    'pr': function () {
                        return isSameProgrammeVsRecording(_this, b);
                    },
                    'ps': function () {
                        return isSameProgrammeVsScheduled(_this, b);
                    },
                    'rp': function () {
                        return isSameProgrammeVsRecording(b, _this);
                    },
                    'sp': function () {
                        return isSameProgrammeVsScheduled(b, _this);
                    },
                    'rr': function () {
                        return isSameRecordingVsRecording(_this, b);
                    },
                    'rs': function () {
                        return isSameRecordingVsScheduled(_this, b);
                    },
                    'sr': function () {
                        return isSameRecordingVsScheduled(b, _this);
                    },
                    'ss': function () {
                        return isSameScheduledVsScheduled(_this, b);
                    }
                };
                return matcher[matcherType]();
            };
            return cPdeRecordingItem;
        })(__MODEL__.IPdeRecordingItem);
        mProvider.cPdeRecordingItem = cPdeRecordingItem;
        var cPdeScheduledItem = (function (_super) {
            __extends(cPdeScheduledItem, _super);
            function cPdeScheduledItem(scheduledItem) {
                _super.call(this);
                this._prismScheduledItem = scheduledItem;
            }
            cPdeScheduledItem.prototype._getProgramme = function () {
                var _this = this;
                var schedule = this._prismScheduledItem;
                hx.svc.EPG.getProgrammes({
                    channel: schedule.getChannelInstance(),
                    clause: "programmeID == " + this.getProgrammeID(),
                    notUI: true,
                    cb: function (result) {
                        if (result[0][0]) {
                            _this._programme = new __MODEL__.COipfScheduledItem(result[0][0]);
                        }
                        result = undefined;
                        schedule = undefined;
                    }
                });
            };
            cPdeScheduledItem.prototype.getSourceItem = function () {
                return this._prismScheduledItem;
            };
            cPdeScheduledItem.prototype.toString = function () {
                return 'PDE ITEM: type=' + this.getType() + ' id=' + this.getId() + ' seriesId=' + this.getSeriesId() + ' programmeId=' + this.getProgrammeID();
            };
            cPdeScheduledItem.prototype.getId = function () {
                return this._prismScheduledItem.getId();
            };
            cPdeScheduledItem.prototype.getStartTime = function () {
                return this._prismScheduledItem.getStartTime();
            };
            cPdeScheduledItem.prototype.getEndTime = function () {
                var that = this;
                var duration = that._prismScheduledItem.getDuration();
                var startTime = that._prismScheduledItem.getStartTime();
                return new Date(startTime.getTime() + duration * 1000);
            };
            cPdeScheduledItem.prototype.getTitle = function () {
                return this._prismScheduledItem.getTitle();
            };
            cPdeScheduledItem.prototype.getThumbnailUrl = function () {
                return this._prismScheduledItem.getThumbnailUrl();
            };
            cPdeScheduledItem.prototype.getChannelInstance = function () {
                return this._prismScheduledItem.getChannelInstance();
            };
            cPdeScheduledItem.prototype.getChannelId = function () {
                return this._prismScheduledItem.getChannelId();
            };
            cPdeScheduledItem.prototype.getServiceId = function () {
                return this._prismScheduledItem.getServiceId();
            };
            cPdeScheduledItem.prototype.getChannelNo = function () {
                return this._prismScheduledItem.getChannelNo();
            };
            cPdeScheduledItem.prototype.getChannelName = function () {
                return this._prismScheduledItem.getChannelName();
            };
            cPdeScheduledItem.prototype.getLogoUrl = function (updateCb) {
                var that = this;
                var logoURL = that._prismScheduledItem.getLogoUrl(that.getLogoWidth());
                if (!logoURL && updateCb) {
                    hx.svc.IP_EPG.getChannelLogo(that.getServiceId(), that.getLogoWidth()).then(function (data) {
                        updateCb(data);
                    });
                }
                return logoURL;
            };
            cPdeScheduledItem.prototype.getLongDescription = function () {
                var programme = this._programme;
                var ret = "";
                if (programme) {
                    ret = this._programme.getLongDescription();
                }
                return fnStringReplaceAll(ret);
            };
            cPdeScheduledItem.prototype.getDescription = function () {
                var programme = this._programme;
                var ret = "";
                if (programme) {
                    ret = this._programme.getShortDescription();
                }
                return fnStringReplaceAll(ret);
            };
            cPdeScheduledItem.prototype.getSeriesId = function () {
                return this._prismScheduledItem.getSeriesId();
            };
            cPdeScheduledItem.prototype.getSeasonNo = function () {
                // TODO: implement this
                return null;
            };
            cPdeScheduledItem.prototype.getEpisodeNo = function () {
                // TODO: implement this
                return null;
            };
            cPdeScheduledItem.prototype.getGenreList = function () {
                // TODO: implement this
                return [];
            };
            cPdeScheduledItem.prototype.getParentalRating = function () {
                var parentalRating;
                var parentalRatings = this._prismScheduledItem._property.parentalRatings;
                if (parentalRatings !== undefined) {
                    parentalRating = parentalRatings[0];
                }
                return (parentalRating ? parentalRating.value : -1);
            };
            cPdeScheduledItem.prototype.getCasIDs = function () {
                var channel = this._prismScheduledItem.getChannelInstance();
                return channel.casIDs || [];
            };
            cPdeScheduledItem.prototype.getLastPlayPosition = function () {
                return 0;
            };
            cPdeScheduledItem.prototype.getOipfScheduledInstance = function () {
                return this._prismScheduledItem.getInstance();
            };
            cPdeScheduledItem.prototype.getProgrammeID = function () {
                return this._prismScheduledItem.getProgrammeID();
            };
            cPdeScheduledItem.prototype.hasSchedule = function () {
                return true;
            };
            cPdeScheduledItem.prototype.hasSeriesSchedule = function () {
                // TODO: implement this
                return null;
            };
            cPdeScheduledItem.prototype.isPendingSeriesSchedule = function () {
                var endTime = new Date((this._prismScheduledItem._property.startTime + this._prismScheduledItem._property.duration + this._prismScheduledItem._property.endPadding) * 1000);
                var now = new Date();
                return ((endTime.getTime() < now.getTime()) && (this.getOipfScheduledInstance().isScheduledAsSeries));
            };
            cPdeScheduledItem.prototype.isReminder = function () {
                return this.getOipfScheduledInstance().factoryType === _oipfDef.TScheduledFactoryType.EREMIND;
            };
            cPdeScheduledItem.prototype.isOnAir = function () {
                var now = new Date();
                return this.getStartTime().getTime() < now.getTime();
            };
            cPdeScheduledItem.prototype.isPast = function () {
                return false;
            };
            cPdeScheduledItem.prototype.isSeries = function () {
                return this._prismScheduledItem.isSeries();
            };
            cPdeScheduledItem.prototype.isPlayable = function () {
                return false;
            };
            cPdeScheduledItem.prototype.isNew = function () {
                return false;
            };
            cPdeScheduledItem.prototype.isHD = function () {
                var channel = this._prismScheduledItem.getChannelInstance();
                return channel.isHD;
            };
            cPdeScheduledItem.prototype.isAD = function () {
                //return this._item.getField("freeview/hasAudioDescription");
                return false;
            };
            cPdeScheduledItem.prototype.isDRM = function () {
                // TODO: implement this
                return false;
            };
            cPdeScheduledItem.prototype.isLocked = function () {
                // TODO: implement this
                return false;
            };
            cPdeScheduledItem.prototype.isKept = function () {
                // TODO: implement this
                return false;
            };
            cPdeScheduledItem.prototype.setKeep = function (aKeep) {
                // TODO: implement this
            };
            cPdeScheduledItem.prototype.isWillbedeleted = function () {
                // TODO: implement this
                return false;
            };
            cPdeScheduledItem.prototype.isRecordingOnGoing = function () {
                return false;
            };
            cPdeScheduledItem.prototype.hasRecording = function () {
                return false;
            };
            cPdeScheduledItem.prototype.isTimebased = function () {
                return this._prismScheduledItem.getProgrammeID() ? false : true;
            };
            cPdeScheduledItem.prototype.isSeriesButNotRecoredAsSeries = function () {
                var that = this;
                return that._prismScheduledItem.isSeries() && !that._prismScheduledItem.isScheduledAsSeries();
            };
            cPdeScheduledItem.prototype.isScheduledAsSeries = function () {
                return this._prismScheduledItem.isScheduledAsSeries();
            };
            cPdeScheduledItem.prototype.isScrambled = function () {
                return false;
            };
            cPdeScheduledItem.prototype.isStreaming = function () {
                return this._prismScheduledItem.isStreaming();
            };
            cPdeScheduledItem.prototype.isMultiAudio = function () {
                var that = this;
                var audioType = that._prismScheduledItem.getAudioType();
                if (audioType === 4) {
                    return true;
                }
                return false;
            };
            cPdeScheduledItem.prototype.hasGroupCRIDs = function () {
                var that = this;
                if (that.getGroupCRIDs() && that.getGroupCRIDs().length > 0) {
                    return true;
                }
                return false;
            };
            cPdeScheduledItem.prototype.getKeywordQuery = function () {
                var that = this;
                var groupCRIDs, i, keyword;
                var groupCRID_datapart;
                if (hx.config.useRfCRID === true) {
                    if (that.hasGroupCRIDs() === true) {
                        groupCRIDs = that.getGroupCRIDs();
                        for (i = 0; i < groupCRIDs.length; i += 1) {
                            groupCRID_datapart = fnGetSeriesID(groupCRIDs[i]);
                            if (i === 0) {
                                keyword = hx.util.format("keyword %% sCRID:%{0}, ", groupCRID_datapart);
                            }
                            else {
                                keyword = keyword + hx.util.format(" OR keyword %% sCRID:%{0}, ", groupCRID_datapart);
                            }
                        }
                    }
                    else {
                        if (that.getProgrammeID() !== null) {
                            keyword = " programmeId == " + that.getProgrammeID();
                        }
                    }
                    return keyword;
                }
            };
            cPdeScheduledItem.prototype.getGroupCRIDs = function () {
                return this._prismScheduledItem.getGroupCRIDs();
            };
            cPdeScheduledItem.prototype.getProgrammeCRID = function () {
                return this._prismScheduledItem.getProgrammeCRID();
            };
            cPdeScheduledItem.prototype.getRecommendationCRID = function () {
                return false;
                //var oipfScheduled = this._prismScheduledItem.getInstance();
                //return oipfScheduled.recommendationCRID;
            };
            // for Deeplink
            cPdeScheduledItem.prototype.isDeeplink = function () {
                var ret = false;
                return ret;
            };
            cPdeScheduledItem.prototype.getDeeplinkAppName = function () {
                var ret = null;
                return ret;
            };
            cPdeScheduledItem.prototype.getDeeplinkUrl = function () {
                var ret = null;
                return ret;
            };
            cPdeScheduledItem.prototype.getSubtitleType = function () {
                var type = 0;
                return type;
            };
            cPdeScheduledItem.prototype.getSubtitleTypes = function () {
                var types;
                if (this._item === undefined) {
                    return "";
                }
                types = this._item.subtitleTypes;
                return types;
            };
            cPdeScheduledItem.prototype.hasTeletext = function () {
                if (this._item === undefined) {
                    return false;
                }
                return this._item.channel.hasTeletext;
            };
            cPdeScheduledItem.prototype.hasGuidance = function () {
                var that = this;
                var guidance = that.getGuidance();
                if (guidance && ((guidance.type === 0) || (guidance.type === 1 && guidance.mode === 1))) {
                    // guidance_type | guidance_mode | Meaning
                    // ----------------------------------------------
                    //      0 1      |     n/a 1     | Unsuitable for viewing outside "watershed hours" without access controls.
                    //       1       |       0       | Warnings/guidance (e.g. Strobe lighting), but suitable for viewing anytime.
                    //      2 3      |      n/a      | Reserved for future use.
                    // ----------------------------------------------
                    return true;
                }
                return false;
            };
            cPdeScheduledItem.prototype.getGuidance = function () {
                var ret = null;
                var programme = this._prismScheduledItem;
                if (programme) {
                    ret = programme.getGuidance() || null;
                    if (ret) {
                        hx.log('debug', "[cPdeScheduledItem] getGuidance---- text : " + ret.text);
                        hx.log('debug', "[cPdeScheduledItem] getGuidance---- type : " + ret.type);
                        hx.log('debug', "[cPdeScheduledItem] getGuidance---- mode : " + ret.mode);
                    }
                }
                return ret;
            };
            cPdeScheduledItem.prototype.getHDLinkage = function () {
                return false;
            };
            cPdeScheduledItem.prototype.isSame = function (b) {
                var _this = this;
                var matcherType;
                if (this instanceof cPdeProgrammeItem && b instanceof cPdeProgrammeItem) {
                    matcherType = 'pp';
                }
                else if (this instanceof cPdeProgrammeItem && b instanceof cPdeRecordingItem) {
                    matcherType = 'pr';
                }
                else if (this instanceof cPdeRecordingItem && b instanceof cPdeProgrammeItem) {
                    matcherType = 'rp';
                }
                else if (this instanceof cPdeProgrammeItem && b instanceof cPdeScheduledItem) {
                    matcherType = 'ps';
                }
                else if (this instanceof cPdeScheduledItem && b instanceof cPdeProgrammeItem) {
                    matcherType = 'sp';
                }
                else if (this instanceof cPdeRecordingItem && b instanceof cPdeRecordingItem) {
                    matcherType = 'rr';
                }
                else if (this instanceof cPdeRecordingItem && b instanceof cPdeScheduledItem) {
                    matcherType = 'rs';
                }
                else if (this instanceof cPdeScheduledItem && b instanceof cPdeRecordingItem) {
                    matcherType = 'sr';
                }
                else if (this instanceof cPdeScheduledItem && b instanceof cPdeScheduledItem) {
                    matcherType = 'ss';
                }
                else {
                    throw "INVALID PARAM";
                }
                var matcher = {
                    'pp': function () {
                        return isSameProgrammeVsProgramme(_this, b);
                    },
                    'pr': function () {
                        return isSameProgrammeVsRecording(_this, b);
                    },
                    'ps': function () {
                        return isSameProgrammeVsScheduled(_this, b);
                    },
                    'rp': function () {
                        return isSameProgrammeVsRecording(b, _this);
                    },
                    'sp': function () {
                        return isSameProgrammeVsScheduled(b, _this);
                    },
                    'rr': function () {
                        return isSameRecordingVsRecording(_this, b);
                    },
                    'rs': function () {
                        return isSameRecordingVsScheduled(_this, b);
                    },
                    'sr': function () {
                        return isSameRecordingVsScheduled(b, _this);
                    },
                    'ss': function () {
                        return isSameScheduledVsScheduled(_this, b);
                    }
                };
                return matcher[matcherType]();
            };
            return cPdeScheduledItem;
        })(__MODEL__.IPdeScheduledItem);
        mProvider.cPdeScheduledItem = cPdeScheduledItem;
        var cPdeProgrammeScheduledItem = (function (_super) {
            __extends(cPdeProgrammeScheduledItem, _super);
            function cPdeProgrammeScheduledItem(aProgramme, oipfScheduledObj) {
                _super.call(this, aProgramme);
                this._item = aProgramme;
                this._oipfScheduledObj = oipfScheduledObj;
            }
            cPdeProgrammeScheduledItem.prototype.getType = function () {
                return __MODEL__.IProgrammeDetailItem.type.scheduled;
            };
            cPdeProgrammeScheduledItem.prototype.hasSchedule = function () {
                return true;
            };
            cPdeProgrammeScheduledItem.prototype.hasSeriesSchedule = function () {
                return this._oipfScheduledObj.isScheduledAsSeries;
            };
            cPdeProgrammeScheduledItem.prototype.getRefProgrammeItem = function () {
                if (this._item) {
                    return this._item;
                }
                return null;
            };
            cPdeProgrammeScheduledItem.prototype.isReminder = function () {
                return this._oipfScheduledObj.factoryType === _oipfDef.TScheduledFactoryType.EREMIND;
            };
            cPdeProgrammeScheduledItem.prototype.isTimebased = function () {
                return this._oipfScheduledObj.programmeID ? false : true;
            };
            cPdeProgrammeScheduledItem.prototype.isSeriesScheduled = function () {
                return this._oipfScheduledObj.isScheduledAsSeries;
            };
            cPdeProgrammeScheduledItem.prototype.isSeriesButNotRecoredAsSeries = function () {
                return this.isSeries() && !this._oipfScheduledObj.isScheduledAsSeries;
            };
            cPdeProgrammeScheduledItem.prototype.getOipfScheduledInstance = function () {
                return this._oipfScheduledObj;
            };
            return cPdeProgrammeScheduledItem;
        })(cPdeProgrammeItem);
        mProvider.cPdeProgrammeScheduledItem = cPdeProgrammeScheduledItem;
    })(mProvider || (mProvider = {}));
    return mProvider;
});
