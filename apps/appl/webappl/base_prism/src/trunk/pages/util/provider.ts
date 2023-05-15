/// <reference path="../../def/sugar.d.ts" />
// file name : pages/util/provider.js
/*jslint sloppy:true, nomen:true, vars:true */

//XXX: there was hx.dl;
declare var hx: ifSugar;
import __MODEL__ = require('model/model');

module mProvider {
    export interface IProgrammeParam {
        oipfData?: any;
        mdsData?: any;
        channel?: any;
        launchLocation?: string;
    }
    export enum EOndemandAvailable {
        ENotAvailable = 0,
        EComingSoon,
        EAvailable
    }
    export class CMetadataProvider {
        name;
        thumbnailURL;
        logoURL;
        type;
        dummy;
        pluginObject;
        channel;
        DEFAULT_THUMB_WIDTH = 123;
        DEFAULT_LOGO_WIDTH = 50;

        constructor(aForExtendedClass?: any, name?, thumbnail?: any) {
            this.name = name;
            this.thumbnailURL = thumbnail;
        }
        setType(type) {
            var that = this;
            that.type = type;
        }
        getName() {
            return this.name || '';
        }
        getThumbnail(param?) {
            return this.thumbnailURL;
        }
        getChannelLogo(param?) {
            return this.logoURL;
        }
        getPlayerLogo(updateCb?) {
            return '';
        }
        getContentOwningLogo(param?) {
            return '';
        }
        setThumbnail(aUrl) {
            this.thumbnailURL = aUrl;
        }
        getType(): any {
            return null;
        }
        getStartTime() {
            return 0;
        }
        getEndTime() {
            return 0;
        }
        getDuration() {
            return 0;
        }
        getPercentage() {
            return 0;
        }
        getMediaType() {
            return '';
        }
        hasSchedule() {
            return false;
        }
        hasReminder() {
            return false;
        }
        isRecorded() {
            return false;
        }
        isPlayAvailable() {
            return false;
        }
        isDummy() {
            return !!this.dummy;
        }
        hasSeriesSchedule() {
            return false;
        }
        getUserID() {
            return -1;
        }
        watchNow(aCallback?) {
            return false;
        }
        getChannelNum() {
            return null;
        }
        getAddition() {
            return '';
        }
        isHd() {
            return false;
        }
        getPluginObject() {
            return this.pluginObject;
        }
    }
    export class CProgrammeProvider extends CMetadataProvider {
        programmeID;
        genre;
        startTime;
        duration;
        endTime;
        parentalRating;
        episode;
        totalEpisodes;
        isHD;
        isAD;
        locked;
        shortDescription;
        secondary;
        longDescription;
        seriesID;
        season;
        channel;
        channelType;
        ccid;
        audioType;
        subtitleType;
        subtitleTypes;
        groupCRIDs;
        guidance;
        isGuidance;
        linkage;
        load_thumbnail;
        defaultThumbnail;
        _userGenres;
        recommendationCRID;
        programmeCRID;
        programmeCridMds;
        parsed
        mdsRaw;
        mdsProgrammeID;
        lloc;
        requestThumbUrl;
        requestImgContentType;
        serviceIDRef;
        availableStartTime;
        availableEndTime;
        ondemandDuration;
        templateAitUrl;
        deeplinkUrl;
        fEpgAvailabilityCS;
        mediaAvailabilityCS;
        availability;
        index;
        recordingIdx;  // for backward with recording

        constructor(aParam : IProgrammeParam) {
            super(null);
            if (!aParam) {
                hx.log('error', '[CProgrammeProvider] cannot create a new prograrmme provider coz (!aParam)');
                return;
            }
            var that = this;
            var oipfData = aParam.oipfData || null;
            var mdsData = aParam.mdsData || null;
            var channel = aParam.channel || null;
            if (oipfData) { // forward
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
        hasRecording() {
            var that = this;
            if (that.pluginObject) {
                hx.log('error', '[hasRecording] this is not Backward!!');
                return false;
            }   // only for backward
            if (!that.getProgrammeID()) {
                hx.log('debug', '[hasRecording] this is not EBR!!');
                return false;
                // only for EBR
            }
            if (that.recordingIdx === undefined) {
                var recordingList = hx.svc.RECORDINGS.getRecordingList();
                if (recordingList && recordingList.length) {
                    that.recordingIdx = recordingList.indexOf(that);
                } else {
                    hx.log('debug', '[hasRecording] recordingList is not prepared yet');
                }
            }
            return !!(that.recordingIdx >= 0);
        }
        getMdsData() {
            return this.mdsRaw ? {
                mdsData : this.mdsRaw,
                launchLocation : this.lloc
            } : null;
        }
        updateMdsData(aTargetEl, aData) {
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
                        if (!that.pluginObject) {   // for only backward
                            if (ondemandData.isHD) {
                                that.isHD = ondemandData.isHD;
                            }
                            if (ondemandData.audioPurpose) {
                                that.isAD = ondemandData.audioPurpose;
                            }
                        }
                        if (ondemandData.programURL) {
                            params = hx.svc.SETTING_UTIL.getNetworkId().map(function(nid) {
                                return 'nids[]=' + nid;
                            });
                            params.push('lloc=' + that.lloc);
                            that.deeplinkUrl = [
                                ondemandData.programURL,
                                params.join('&')
                            ].join(ondemandData.programURL.indexOf('?') !== -1 ? '&' : '?');
                        }
                        if (ondemandData.auxiliaryURL) {
                            params = hx.svc.SETTING_UTIL.getNetworkId().map(function(nid) {
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
                        if (!that.pluginObject) {   // for only backward
                            if (programData.genre) {
                                that.genre = (that.genre && that.genre.length) ? that.genre : programData.genre;
                            }
                            if (programData.parentalGuidance && programData.parentalGuidance.explanatoryText) {
                                that.guidance = that.guidance || {
                                        text : programData.parentalGuidance.explanatoryText
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
        }
        setMdsData(aParam) {
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
        }
        getTemplateAitUrl() {
            return this.templateAitUrl;
        }
        getDeeplinkUrl () {
            return this.deeplinkUrl;
        }
        checkTemplateAIT(xml) {
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
            if (version <= 131 && type === 'application/vnd.hbbtv.xhtml+xml') { // B2C Spec (v1.0) 7.1 Device Criteria available version 1.3.1
                var now = new Date().getTime() / 1000;
                var fnCheckAvailabilityWindow = function(startAvail, endAvail, mediaAvailabilityCS) {
                    if (startAvail < now && now <= endAvail) { // within availability window
                        if (mediaAvailabilityCS) {
                            return EOndemandAvailable.EAvailable;
                        } else {
                            return EOndemandAvailable.EComingSoon;
                        }
                    } else if ((now <= startAvail) && ((startAvail - 7200) < now)) {
                        return EOndemandAvailable.EComingSoon;
                    } else {
                        return EOndemandAvailable.ENotAvailable;
                    }
                }
                if (that.endTime <= now) {  // backward
                    that.availability = fnCheckAvailabilityWindow(that.availableStartTime, that.availableEndTime, that.mediaAvailabilityCS);
                } else {    // forward
                    if (that.fEpgAvailabilityCS) {
                        that.availability = fnCheckAvailabilityWindow(that.availableStartTime, that.availableEndTime, that.mediaAvailabilityCS);
                    } else {
                        that.availability = EOndemandAvailable.ENotAvailable;
                    }
                }
            }
        }
        getAvailability() {
            return this.availability;
        }
        getPluginObject() {
            return this.pluginObject;
        }
        getProgrammeName() {
            var that = this;
            return that.name;
        }
        hasGuidance() {
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
            } else {
                return !!that.isGuidance;
            }
            return false;
        }
        getGuidance() {
            var that = this;
            hx.log('debug', "[CProgrammeProvider] getGuidance---- text : " + (that.guidance ? that.guidance.text : ""));
            hx.log('debug', "[CProgrammeProvider] getGuidance---- type : " + (that.guidance ? that.guidance.type : ""));
            hx.log('debug', "[CProgrammeProvider] getGuidance---- mode : " + (that.guidance ? that.guidance.mode : ""));
            return that.guidance;
        }
        getParentalRatings() {
            var that = this;
            return that.parentalRating;
        }
        getThumbnailURL(noDefault?) {
            var that = this;
            return noDefault ? that.thumbnailURL : (that.thumbnailURL || that.getDefaultThumbnail());
        }
        getMdsSchedule(aUpdateCb?) {
            var that = this;
            if (that.getMdsData()) {
                if (aUpdateCb) {
                    aUpdateCb();
                }
                return;
            }
            if (that.getServiceId()) {
                hx.svc.IP_EPG.getSchedules(that.getServiceId(), that.getStartTime() * 1000, 6).then((res) => {
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
                                    mdsData : data,
                                    launchLocation : 'epg'
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
                }, () => {
                    if (aUpdateCb) {
                        aUpdateCb();
                    }
                });
            } else {
                if (aUpdateCb) {
                    aUpdateCb();
                }
            }
        }
        getThumbnail(param) {
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
            hx.svc.IP_EPG.getThumbnail(a.requestThumbUrl, width, {"content-type" : a.requestImgContentType}).then((data) => {
                if (data) {
                    updateCb(a.thumbnailURL = data);
                } else {
                    updateCb(defaultPath ? defaultPath + a.getDefaultThumbnail() : a.getDefaultThumbnail());
                }

            }, () => {
                updateCb(defaultPath ? defaultPath + a.getDefaultThumbnail() : a.getDefaultThumbnail());
            });
        }
        getChannelLogo(param) {
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
            } else {
                hx.svc.IP_EPG.getChannelLogo(a.getServiceId(), width).then((data) => {
                    updateCb(a.logoURL = data || '');
                }, () => {
                    updateCb('');
                });
            }
        }
        getType() {
            return (this.getChannelType() === 0) ? hx.l('LOC_TV_PROGRAMME_ID') : hx.l('LOC_RADIO_PROGRAMME');
        }
        getDefaultThumbnail() {
            return this.getGenreImage() || __MODEL__.TDefaultThumbnailList.DEFAULT;
        }
        getSecondary() {
            return this.secondary || '';
        }
        getShortDescription() {
            var that = this;
            return that.shortDescription || '';
        }
        getLongDescription() {
            var that = this;
            return that.longDescription || '';
        }
        getStartTime() {
            var that = this;
            return that.startTime || 0;
        }
        getEndTime() {
            var that = this;
            return that.endTime;
        }
        getDuration() {
            var that = this;
            return that.duration || 0;
        }
        getPercentage() {
            var that = this;
            var now = (new Date()).getTime() / 1000;
            return parseInt(((now - that.getStartTime()) / that.getDuration() * 100) + "", 10);
        }
        getPercentageByTime(now) {
            var that = this;
            return parseInt(((now - that.getStartTime()) / (that.getDuration() * 100)) + "", 10);
        }
        isLive() {
            var that = this;
            var now = (new Date()).getTime() / 1000;
            var st = that.getStartTime();
            var et = that.getStartTime() + that.getDuration();
            return (now >= st && now <= et) ? true : false;
        }
        isPast() {
            var that = this;
            var now = (new Date()).getTime() / 1000;
            var st = that.getStartTime();
            var et = that.getStartTime() + that.getDuration();
            return (et < now) ? true : false;
        }
        getChannelInstance() {
            var that = this;
            return that.channel;
        }
        getChannelType() {
            var that = this;
            return that.channelType;
        }
        getChannelNum() {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().majorChannel : null;
        }
        getChannelName() {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().name : null;
        }
        setStartTime(startTime) {
            this.startTime = startTime;
            return this;
        }
        setDuration(duration) {
            this.duration = duration;
            return this;
        }
        setEndTime(endTime) {
            this.endTime = endTime;
            return this;
        }
        setChannelInformation(channel) {
            if (channel) {
                this.channelType = channel.channelType;
                this.thumbnailURL = this.defaultThumbnail = (this.channelType === 1) ? __MODEL__.TDefaultThumbnailList.RADIO : __MODEL__.TDefaultThumbnailList.TV;
                this.ccid = channel.ccid;
            }
            return this;
        }
        setProgrammeName(name) {
            this.name = name;
            return this;
        }
        isRecording() {
            var that = this;
            var isRecording = false;
            var programme = that.pluginObject;
            if (programme) {
                var recording = programme.recording;
                if (recording) {
                    if (recording.id && (recording.showType === 0/*LIVE*/)) {/*recording*/
                        isRecording = true;
                    }
                }
            }
            return isRecording;
        }
        isPlayAvailable() {
            return !!(this.getAvailability() === EOndemandAvailable.EAvailable);
        }
        hasSeriesSchedule() {
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
        }
        hasSchedule() {
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
        }
        hasReminder() {
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
        }
        getObjectType() {
            return 'EPG_DATA';
        }
        getGenreImage() {
            var that = this;
            var i, length, major, minor, word, content = that.genre;
            // priority of genre image from RF is higher than from MDS
            if (content && !(content instanceof Array)) {    // for RF
                for (i = 0, length = content.length; i < length; i += 1) {
                    word = content[i] || '';
                    major = parseInt(word.charAt(0), 16) - 1;
                    minor = parseInt(word.charAt(1), 16);
                    return hx.il.genreList.getGenreImage(major);
                }
            } else if (content && content.length) {        // for MDS
                return hx.il.genreList.getGenreImage(content[0]);
            }
            return null;
        }
        getGenre() {
            var that = this;
            var i, length, major, minor, word, genres = [], contents = that.genre || [];
            var userGenre;
            var majorNibbles = hx.il.genreList.getGenreList('major',false);
            var minorNibbles = hx.il.genreList.getGenreList('minor',false);
            function validateStringNumericInRange(value, min, max) {
                return (!isNaN(value) && value > min && value < max);
            }
            if (contents && !(contents instanceof Array)) {    // for RF
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
        }
        watchNow(callback?) {
            var that = this;
            var channel = that.getChannelInstance();
            var comsumed = false;
            if (channel && that.isPlayAvailable()) {
                var bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                        channel : channel
                });
                if (bChangeable === false) {
                    if (hx.config.useTVStreaming === false) {
                        hx.msg('response', {
                            text : hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                            auto_close : true,
                            close_time : 3000
                        });
                    } else {
                        hx.msg('response', {
                            text : hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                            auto_close : true,
                            close_time : 3000
                        });
                    }
                    return;
                }
                if (callback) {
                    callback(true);
                }
                hx.svc.AV_CTRL.channelChange({
                        channel : channel,
                        group : (channel.channelType === 0) ? 'TV' : 'RADIO'
                });
                comsumed = true;
            } else {
                if (callback) {
                    callback();
                    comsumed = true;
                }
            }
            return comsumed;
        }
        getRecommendationCRID() {
            var that = this;
            return that.recommendationCRID || null;
        }
        setProgrammeID(programmeId) {
            this.programmeID = programmeId;
            return this;
        }
        getProgrammeID() {
            var that = this;
            return that.programmeID || that.mdsProgrammeID;
        }
        getMdsProgrammeID() {
            return this.mdsProgrammeID || null;
        }
        getProgrammeCRID() {
            var that = this;
            return that.programmeCRID || null;
        }
        getProgrammeCridMds() {
            return this.programmeCridMds || null;
        }
        getGroupCRIDs() {
            var that = this;
            return that.groupCRIDs || null;
        }
        getSeriesID() {
            var that = this;
            return that.getGroupCRIDs() ? that.getGroupCRIDs()[0] : null;
        }
        getServiceId() {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().sid : null;
        }
        getServiceIDRef() {
            return this.serviceIDRef || null;
        }
        isHd() {
            var that = this;
            return that.isHD || false;
        }
        isAd() {
            var that = this;
            return that.isAD || false;
        }
        isLocked() {
            var that = this;
            return that.locked;
        }
        getHDLinkage(): ifFreeviewLinkage[] {
            var that = this;
            if (that.linkage === undefined && that.pluginObject) {
                that.linkage = that.pluginObject.getField("freeview/linkage");
                if (that.linkage) {
                    that.linkage = JSON.parse(that.linkage);
                }
            }
            return that.linkage || null;
        }
        getChannelId() {
            var that = this;
            return that.ccid;
        }
        isMultiAudio() {
            var that = this;
            if (that.audioType === undefined && that.pluginObject) {
                that.audioType = that.pluginObject.audioType;
            }
            if (that.audioType === 4) {
                return true;
            }
            return false;
        }
        getSubtitleType() {
            var that = this;
            if (that.subtitleType === undefined && that.pluginObject) {
                that.subtitleType = that.pluginObject.subtitleType;
            }
            return that.subtitleType || null;
        }
        getSubtitleTypes() {
            var that = this;
            if (that.subtitleTypes === undefined && that.pluginObject) {
                that.subtitleTypes = that.pluginObject.subtitleTypes;
            }
            return that.subtitleTypes || null;
        }
        isOnAir() {
            return false;
        }
    }
    export class CRecordingProvider extends CMetadataProvider {
        pluginObject;
        //recording is not plugin object, but use same name as other provider
        channel;
        startTime;
        duration;
        originStartTime;
        originDuration;
        endTime;
        recordingStartTime;
        recordingDuration;
        recordingEndTime;
        lastPlayPosition;
        isManualLocked;
        programmeID;
        showType;
        uid;
        id;
        seriesID;
        state;
        error;
        isNull;
        ccid;
        episode;
        season;
        isHD;
        isAD;
        shortDescription;
        longDescription;
        genre;
        parentalRating;
        guidance;
        _userGenres;

        constructor(recording?) {
            super(null, recording.name || recording.channel.name, recording.thumbnail);
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
                        for ( i = 0, len = genres.length; i < len; i += 1) {
                            this.genre.push(genres[i]);
                        }
                    }
                    if ((parentalRatings !== null) && (parentalRatings !== undefined)) {
                        this.parentalRating = [];
                        for ( i = 0, len = parentalRatings.length; i < len; i += 1) {
                            this.parentalRating.push(parentalRatings[i]);
                        }
                    }
                }
            }
        }
        getStartTime() {
            return this.recordingStartTime || 0;
        }
        getEndTime() {
            return this.recordingEndTime || 0;
        }
        getThumbnail() {
            var that = this;
            return that.thumbnailURL || that.getDefaultThumbnail();
        }
        getChannelLogo(param) {
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
            } else {
                hx.svc.IP_EPG.getChannelLogo(a.getServiceId(), width).then((data) => {
                    if (data) {
                        if (updateCb) {
                            updateCb(data);
                        }
                        a.logoURL = data;
                    }
                }, () => {
                    if (updateCb) {
                        updateCb('');
                    }
                });
            }
        }
        getDefaultThumbnail() {
            var that = this;
            var defaultThumbnail = __MODEL__.TDefaultThumbnailList.RECORDING;
            if (that.state === __MODEL__.eTRecordingState.ERECORDING_REC_PARTIALLY_COMPLETED && that.pluginObject.showType === __MODEL__.eTRecordingShowType.EFIRST_RUN) {
                defaultThumbnail = __MODEL__.TDefaultThumbnailList.REC_FAIL;
            }
            return defaultThumbnail;
        }
        getGenreImage() {
            var that = this;
            var i, length, major, minor, word, content = that.genre || [];
            for ( i = 0, length = content.length; i < length; i += 1) {
                word = content[i] || '';
                major = parseInt(word.charAt(0), 16) - 1;
                minor = parseInt(word.charAt(1), 16);
                return hx.il.genreList.getGenreImage(major);
            }
            return null;
        }
        getDispDuration() {
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
            } else {
                str.push('0m');
            }

            return str.join('');
        }
        getShortDescription() {
            return this.shortDescription;
        }
        getLongDescription() {
            return this.longDescription;
        }
        getDuration() {
            return this.duration;
        }
        getUserID() {
            return this.uid;
        }
        isRecording() {
            var that = this;
            if (that.showType === 'LIVE') {
                return true;
            }

            return false;
        }
        isRecorded() {
            var that = this;
            if (that.id && that.showType !== 'LIVE') {
                return true;
            }

            return false;
        }
        hasSeriesSchedule() {
            var that = this;
            if (that.type === 'SCHEDULE' && that.pluginObject.isScheduledAsSeries) {
                return true;
            }
            return false;
        }
        hasSchedule() {
            var that = this;
            if (that.type === 'SCHEDULE' && that.pluginObject.factoryType === 'RECORDING') {
                return true;
            }
            return false;
        }
        hasReminder() {
            var that = this;
            if (that.type === 'SCHEDULE' && that.pluginObject.factoryType === 'REMIND') {
                return true;
            }
            return false;
        }
        getChannelNum() {
            var that = this;
            return that.channel ? that.channel.majorChannel : '';
        }
        getServiceId() {
            return this.channel ? this.channel.sid : null;
        }
        getRecordingID() {
            return this.id;
        }
        getGenre() {
            var that = this;
            var i, length, major, minor, word, genres = [], content = that.genre || [];
            var userGenre;
            var majorNibbles = ['LOC_GENRE_MOVIE_ID', 'LOC_GENRE_NEWS_ID', 'LOC_GENRE_SHOW_ID', 'LOC_GENRE_SPORTS_ID', 'LOC_GENRE_CHILDREN_ID', 'LOC_GENRE_MUSIC_ID', 'LOC_GENRE_ARTS_ID', 'LOC_GENRE_SOCIAL_ID', 'LOC_GENRE_EDUCATION_ID', 'LOC_GENRE_LEISURE_ID', 'LOC_GENRE_SPECIAL_CHARACTER_ID'],
            // //
                minorNibbles = [['LOC_GENRE_MOVIE_DRAMA_ID', 'LOC_GENRE_DETECTIVE_THRILLER_ID', 'LOC_GENRE_ADVENTURE_WESTERN_WAR_ID', 'LOC_GENRE_SF_FANSTASY_HORROR_ID', 'LOC_GENRE_COMEDY_ID', 'LOC_GENRE_SOAP_MELO_FOLKLORIC_ID', 'LOC_GENRE_ROMANCE_ID', 'LOC_GENRE_SERIOUS_DRAMA_ID', 'LOC_GENRE_ADULT_MOVIE_DRAMA_ID'], ['LOC_GENRE_NEWS_CURRENT_AFFAIRS_ID', 'LOC_GENRE_NEWS_WEATHER_REPORT_ID', 'LOC_GENRE_NEWS_MAGAZINE_ID', 'LOC_GENRE_DOCUMENTARY_ID', 'LOC_GENRE_INTERVIEW_ID'], ['LOC_GENRE_SHOW_GAME_ID', 'LOC_GENRE_SHOW_QUIZ_CONTEST_ID', 'LOC_GENRE_VARIETY_SHOW_ID', 'LOC_GENRE_TALK_SHOW_ID'], ['LOC_GENRE_SPECIAL_EVENTS_ID', 'LOC_GENRE_SPORTS_MAGAZINE_ID', 'LOC_GENRE_FOOTBALL_SOCCER_ID', 'LOC_GENRE_TENNIS_SQUASH_ID', 'LOC_GENRE_TEAM_SPROTS_ID', 'LOC_GENRE_ATHLETICS_ID', 'LOC_GENRE_MOTOR_SPORT_ID', 'LOC_GENRE_WATER_SPORT_ID', 'LOC_GENRE_WINTER_SPORTS_ID', 'LOC_GENRE_EQUESTRIAN_ID', 'LOC_GENRE_MARTIAL_SPORTS_ID'], ['LOC_GENRE_CHILDREN_YOUTH_ID', 'LOC_GENRE_PRESCHOOL_CHILDREN_ID', 'LOC_GENRE_6TO4_ENTERTAIN_ID', 'LOC_GENRE_10TO16_ENTERTAIN_ID', 'LOC_GENRE_INFO_EDU_SCHOOL_ID', 'LOC_GENRE_CARTOONS_PUPPETS_ID'], ['LOC_GENRE_MUSIC_DANCE_ID', 'LOC_GENRE_ROCK_POP_ID', 'LOC_GENRE_CLASSIC_ID', 'LOC_GENRE_FOLK_TRADITIONAL_ID', 'LOC_GENRE_JAZZ_ID', 'LOC_GENRE_MUSICAL_OPERA_ID', 'LOC_GENRE_BALLET_ID'], ['LOC_GENRE_ARTS_CULTURE_ID', 'LOC_GENRE_PREFORMING_ARTS_ID', 'LOC_GENRE_FINE_ARTS_ID', 'LOC_GENRE_RELIGION_ID', 'LOC_GENRE_POP_TRADITIONAL_ID', 'LOC_GENRE_LITERATURE_ID', 'LOC_GENRE_FILM_CINEMA_ID', 'LOC_GENRE_EXPERIMENTAL_ID', 'LOC_GENRE_BROADCASTING_PRESS_ID', 'LOC_GENRE_NEW_MEDIA_ID', 'LOC_GENRE_ARTS_MAGAZINE_ID', 'LOC_GENRE_FASHION_ID'], ['LOC_GENRE_SOCIAL_ECONOMICS_ID', 'LOC_GENRE_MAGAZINE_ID', 'LOC_GENRE_ADVISORY_ID', 'LOC_GENRE_REMARKABLE_ID'], ['LOC_GENRE_EDU_SCIENCE_FACTUAL_ID', 'LOC_GENRE_ENVIRONMENT_ID', 'LOC_GENRE_TECH_SCIENCE_ID', 'LOC_GENRE_MEDI_PHYSI_PSYCH_ID', 'LOC_GENRE_FOREIGN_ID', 'LOC_GENRE_SOCIAL_SPIRITUAL_ID', 'LOC_GENRE_FURTHER_EDU_ID', 'LOC_GENRE_LANGUAGE_ID'], ['LOC_GENRE_LEISURE_HOBBIES_ID', 'LOC_GENRE_TOURISM_TRAVEL_ID', 'LOC_GENRE_HANDICRAFT', 'LOC_GENRE_MOTORING_ID', 'LOC_GENRE_FITNESS_HEALTH_ID', 'LOC_GENRE_COOKING_ID', 'LOC_GENRE_SHOPPING_ID', 'LOC_GENRE_GARDENING_ID'], ['LOC_GENRE_ORIGIN_LANG_ID', 'LOC_GENRE_BLACK_WHITE_ID', 'LOC_GENRE_UNPUBLISHED_ID', 'LOC_GENRE_LIVE_BROADCAST_ID']];

            function validateStringNumericInRange(value, min, max) {
                return (!isNaN(value) && value > min && value < max);
            }

            for ( i = 0, length = content.length; i < length; i += 1) {
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
                    if(that._userGenres.indexOf(userGenre) === -1){
                        that._userGenres.push(userGenre);
                    }
                }
            }

            return genres;
        }
        isNullProgramme() {
            return this.isNull !== false;
        }
        getObjectType() {
            return 'RECORDING';
        }
        getType() {
            return 'Recording';
        }
        isPlayAvailable() {
            return true;
        }
        watchNow(callback?) {
            var that = this;

            if (callback && that.isPlayAvailable()) {
                callback(that);
                return true;
            }

            return false;
        }
        isHd() {
            return this.isHD;
        }
        getGuidance() {
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
            } else {
                return "";
            }
        }
    }
    export class CChannelProvider extends CMetadataProvider {
        channelType;
        ccid;
        majorChannel;
        isHD;
        constructor(channel) {
            super(null, channel.name, channel.logoURL);
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
        getType() {
            var that = this;
            if (that.isHD) {
                return "HDTV";
            } else {
                return (that.channelType === 0) ? "TV" : "Radio";
            }
        }
        getChannelNum() {
            return this.majorChannel;
        }
        isPlayAvailable() {
            return false;
        }
        watchNow(callback?) {
            var that = this;
            var channel = that.pluginObject;
            if (channel) {
                if (callback) {
                    callback(that);
                }
                var bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                        channel : channel
                });
                if (bChangeable === false) {
                    if (hx.config.useTVStreaming === false) {
                        hx.msg('response', {
                            text : hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                            auto_close : true,
                            close_time : 3000
                        });
                    } else {
                        hx.msg('response', {
                            text : hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                            auto_close : true,
                            close_time : 3000
                        });
                    }
                    return;
                }
                hx.svc.AV_CTRL.channelChange({
                        channel : channel,
                        group : (channel.channelType === 0) ? 'TV' : 'RADIO'
                });
                return true;
            }
            return false;
        }
        getServiceId() {
            return this.pluginObject.sid;
        }
        getThumbnail(param?) {
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
        }
        isHd() {
            return this.isHD;
        }
    }
    export class CHumaxContentProvider extends CMetadataProvider {
        VIDEO = 'ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|mpg|vob|divx|mpeg';
        PHOTO = 'jpg|jpeg|bmp|gif|tif|png';
        MUSIC = 'mp3|wav';
        _content;
        contentType;
        fullPath;
        hasSubitem;
        date;
        duration;
        lastViewTime;
        thumbnailUrl;
        resolution;
        thumbResolution;
        pictureDate;
        album;
        artist;
        watched;
        storageCategory;
        title;

        constructor(content?) {
            //XXX: XXjklsjdfasdXXXXfiajfoiajsfojewo1!XXX!!
            if (!content) {
                throw 'content needed!';
            }
            var hdAudio = hx.config.CONFIG_HD_AUDIO;
            if (hdAudio) {
                this.MUSIC +='|flac';
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
            super(null, this.title, this.thumbnailUrl);
        }
        getPluginObject() {
            return this._content;
        }
        getType() {
            var that = this;
            var STORAGE_TYPE = [hx.l('LOC_HDD_ID'), hx.l('LOC_USB_ID'), hx.l('LOC_SDCARD_ID'), hx.l('LOC_UNKNOWN_ID')// 'SAMBA'
                , hx.l('LOC_UNKNOWN_ID') // 'UNKNOWN'
            ];
            var storage = STORAGE_TYPE[that.storageCategory];
            return storage;
        }
        getMediaType() {
            var that = this;
            var lastIndex = that.fullPath ? that.fullPath.lastIndexOf('.') : -1;
            var extension = '';
            var contentType = '';
            if (lastIndex !== -1) {
                extension = that.fullPath.substr(lastIndex + 1).toLowerCase();
                if (that.VIDEO.indexOf(extension) > -1) {
                    contentType = hx.l('LOC_VIDEO_ID');
                } else if (that.PHOTO.indexOf(extension) > -1) {
                    contentType = hx.l('LOC_PHOTO_ID');
                } else if (that.MUSIC.indexOf(extension) > -1) {
                    contentType = hx.l('LOC_GENRE_MUSIC_ID');
                }

                return contentType;
            }
            return '';
        }
        isPlayAvailable() {
            return true;
        }
        watchNow(callback?) {
            var that = this;

            if (callback && that.isPlayAvailable()) {
                callback(that);
                return true;
            }

            return false;
        }
    }
    export class CApplicationProvider extends CMetadataProvider {
        id;
        localURI;
        downloadURI;
        defaultIcon;
        customIcons;
        constructor(application?) {
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
            super(null, that.name, (this.customIcons && this.customIcons[3]) ? this.customIcons[3].Url : '/');
        }
        getType() {
            return hx.l('LOC_APPLICATION_ID');
        }
        watchNow(callback?) {
            var handled = false;
            if (callback) {
                callback();
                handled = true;
            }
            return true;
        }
    }
    export class CWebContentProvider extends CMetadataProvider {
        _TYPE = ['WEB'];
        country;
        duration;
        expireDate;
        genre;
        geoRestrict;
        id;
        images;
        keywords;
        lang;
        longText;
        medias;
        persons;
        pubDate;
        rating;
        ratingType;
        seasonId;
        seriesCount;
        seriesId;
        seriesNumber;
        seriesTitle;
        shortText;
        title;
        thumbnailUrl;
        transmitDate;
        type;
        dummy;
        constructor(content?) {
            super(null, content.Title, content.Images[0] ? content.Images[0].ImageURL : '');
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
        getType() {
            var that = this;
            return that.type;
        }
        isPlayAvailable() {
            return !this.dummy;
        }
        watchNow(callback) {
            var that = this;

            if (callback && that.isPlayAvailable()) {
                callback(that);
                return true;
            }
            return false;
        }
    }
    export class CDlnaContentProvider extends CMetadataProvider {
        title;
        thumbnailUrl;
        duration;
        fullPath;
        byteSeekAble;
        resolution;
        _content;
        importURI;
        constructor(content) {
            super(null, content.getTitle(), content.getThumbnailUrl());
            this.title = content.getTitle();
            this.thumbnailUrl = content.getThumbnailUrl();
            this.duration = content.getDuration();
            this.fullPath = this.importURI = content.getImportUrl();
            this.byteSeekAble = content.isByteSeekAvailable();
            this.resolution = content.getResolution();
            this._content = content;
        }
        getPluginObject() {
            return this._content;
        }
        getType() {
            var that = this;
            return that.type;
        }
        getMediaType() {
            var that = this;
            var objectType = that._content.getObjectType();
            var str = '';
            if (objectType === 'video') {
                str = hx.l('LOC_VIDEO_ID');
            } else if (objectType === 'photo') {
                str = hx.l('LOC_PHOTO_ID');
            } else if (objectType === 'music') {
                str = hx.l('LOC_GENRE_MUSIC_ID');
            }
            return str;
        }
        isPlayAvailable() {
            return true;
        }
    }
    export class COnDemandProvider extends CProgrammeProvider {
        contentOwningLogoUrl;
        constructor(aData, aLaunchLocation?) {
            if (!aData) {
                hx.log('error', '[COnDemandProvider] !aData');
                return;
            }
            super({
                mdsData : aData,
                launchLocation : aLaunchLocation || 'search'
            });
            this.type = "ondemand";
        }
        getTemplateAitUrl() {
            return this.templateAitUrl;
        }
        checkTemplateAIT(xml) {
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
            if (version <= 131 && type === 'application/vnd.hbbtv.xhtml+xml') { // B2C Spec (v1.0) 7.1 Device Criteria available version 1.3.1
                return true;
            }
            hx.log('debug', '[COnDemandProvider] FAIL!!! checkTemplateAIT : [' + version + '][' + type + '] / ' + this.name);
            return false;
        }
        getDuration() {
            return this.ondemandDuration || 0;
        }
        getAvailableEndTime() {
            return this.availableEndTime || 0;
        }
        getAddition() {
            return this.secondary || '';
        }
        getType() {
            return this.type;
        }
        getContentOwningLogo(param) {
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
            } else {
                hx.svc.IP_EPG.getContentOwningLogo(a.getServiceIDRef(), width).then((data) => {
                    a.contentOwningLogoUrl = data || '';
                    if (updateCb) {
                        updateCb(a.contentOwningLogoUrl);
                    }
                }, () => {
                    if (updateCb) {
                        updateCb('');
                    }
                });
            }
        }
        getDefaultThumbnail() {
            return __MODEL__.TDefaultThumbnailList.FREEVIEW;  //freeivew image
        }
        isPlayAvailable() {
            var a = this;
            var bAvailable = true;
            var timeCurrent = Math.floor(new Date().getTime() / 1000);
            if (a.availableStartTime >= timeCurrent || a.availableEndTime < timeCurrent) {
                hx.log('debug', '[COnDemandProvider] !isPlayAvailable');
                bAvailable = false;
            }
            return bAvailable;
        }
    }
    var fnStringReplaceAll = function(sourceString) {
        var convertedString;
        if (sourceString) {
            convertedString = sourceString.replace(/\n/g, '<br>');
        }
        return convertedString;
    };
    var isSameProgrammeVsProgramme = function(programmeItem1, programmeItem2) {
        return (programmeItem1.getId() === programmeItem2.getId() && programmeItem1.getChannelId() === programmeItem2.getChannelId());
    };
    var isSameProgrammeVsRecording = function(programmeItem, recordingItem) {
        if ((programmeItem.getChannelId() === recordingItem.getChannelId()) && recordingItem.isRecordingOnGoing() && programmeItem.isOnAir()) {
            return true;
        }
        return !!(hx.il.getCompareID(programmeItem) === hx.il.getCompareID(recordingItem));
    };
    var fnGetSeriesID = (aGroupCRID) => {
        var series_id = aGroupCRID.split("/");
        series_id.shift();
        series_id = series_id.join("");
        var idxCut = series_id.lastIndexOf("#");
        if (idxCut > -1) {
            series_id = series_id.substr(0, idxCut);
        }
        return '/'+series_id;
    };
    var isSameProgrammeVsScheduled = function(programmeItem, scheduledItem) {
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
                } else {
                    hx.log('dsrvDetailInfo2', '[isSameProgrammeVsScheduled] same scheduled Item');
                    ret = true;
                }
            } else {
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
                } else if (((scheduledItem.getSeriesId() === "" || scheduledItem.getSeriesId() === undefined ) && scheduledItem.getGroupCRIDs().length > 0) && ((programmeItem.getSeriesId() === "" || programmeItem.getSeriesId() === undefined ) && programmeItem.getGroupCRIDs().length > 0)) {
                    var i, j;

                    for ( i = 0; i < scheduledItem.getGroupCRIDs().length; i += 1) {
                        for ( j = 0; j < programmeItem.getGroupCRIDs().length; j += 1) {
                            if (programmeItem.getGroupCRIDs().item(j) === scheduledItem.getGroupCRIDs().item(i)) {
                                return true;
                            }
                        }
                    }
                } else {
                    if ((ENV.listOP.AUSDTT === ENV.op) ) {
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
    var isSameRecordingVsRecording = function(recordingItem1, recordingItem2) {
        return recordingItem1.getId() === recordingItem2.getId();
    };
    var isSameRecordingVsScheduled = function(recordingItem, scheduledItem) {
        if (scheduledItem.isTimebased()) {
            return recordingItem.getStartTime() === scheduledItem.getStartTime() && recordingItem.getChannelId() === scheduledItem.getChannelId();
        }
        return recordingItem.getProgrammeID() === scheduledItem.getProgrammeID();
    };
    var isSameScheduledVsScheduled = function(scheduledItem1, scheduledItem2) {
        if (scheduledItem1.isTimebased()) {
            return (scheduledItem1.getStartTime().getTime() === scheduledItem2.getStartTime().getTime())//
                && (scheduledItem1.getChannelId() === scheduledItem2.getChannelId());
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
    export class cPdeOndemandItem extends __MODEL__.IPdeOndemandItem {
        _item: COnDemandProvider;
        constructor(item) {
            super();
            this._item = item || {};
        }
        getSourceItem() {
            return this._item;
        }
        toString() {
            var ret = 'PDE ITEM: type=' + this.getType() + ' id=' + this.getId() + ' seriesId=' + this.getSeriesId() + ' programmeId=' + this.getProgrammeID();
            ret += '\nPDE ITEM: title=' + this.getTitle() + ' channelId=' + this.getChannelId() + ' channelName=' + this.getChannelName();
            return ret;
        }
        getId() {
            return this._item.getProgrammeID();
        }
        getStartTime() {
            return this._item.getStartTime() ? new Date(this._item.getStartTime() * 1000) : null;
        }
        getEndTime() {
            if (this._item.startTime && this._item.duration) {
                var startTime = this._item.startTime * 1000;
                var duration = this._item.duration * 1000;
                return new Date(startTime + duration);
            } else {
                return null;
            }
        }
        getTitle() {
            return this._item.getName() || '';
        }
        getSecondary() {
            return this._item.getSecondary() || '';
        }
        getThumbnailUrl(updateCb?) {
            return this._item ? this._item.getThumbnail({
                updateCb : updateCb,
                width : this.getThumbWidth()
            }) : '';
        }
        getLogoUrl(updateCb?) {
            return this._item ? this._item.getContentOwningLogo({
                updateCb : updateCb,
                width : this.getLogoWidth()
            }) : '';
        }
        getChannelInstance() {
            return null;
        }
        getChannelId() {
            return '';
        }
        getChannelNo() {
            return '';
        }
        getChannelName() {
            return '';
        }
        getLongDescription() {
            return this._item.getLongDescription() || '';
        }
        getDescription() {
            return '';
        }
        getSeriesId() {
            return this._item.getProgrammeCRID() || null;
        }
        getSeasonNo() {
            return null;
        }
        getEpisodeNo() {
            return null;
        }
        getGenreList() {
            return this._item.getGenre();
        }
        getParentalRating() {
            var parentalRatings = this._item.getParentalRatings();
            if (!parentalRatings) {
                return -1;
            }
            return ( parentalRatings[0] ? parentalRatings[0].value : -1);
        }
        getCasIDs() {
            return [];
        }
        getLastPlayPosition() {
            return null;
        }
        getProgrammeID() {
            return this._item.getProgrammeID() || null;
        }
        hasSchedule() {
            return false;
        }
        hasSeriesSchedule() {
            return false;
        }
        isReminder() {
            return false;
        }
        isOnAir() {
            return false;
        }
        isPast() {
            return false;
        }
        getGroupCRIDs() {
            return [];
        }
        getProgrammeCRID() {
            return null;
        }
        getRecommendationCRID() {
            return null;
        }
        hasGroupCRIDs() {
            return false;
        }
        getKeywordQuery() {
            return '';
        }
        isSeries() {
            return false;
        }
        isPlayable() {
            return false;
        }
        isNew() {
            return false;
        }
        isHD() {
            return this._item.isHd();
        }
        isAD() {
            return this._item.isAd();
        }
        isDRM() {
            return false;
        }
        isLocked() {
            return false;
        }
        isKept() {
            // TODO: implement this
            return false;
        }
        setKeep(aKeep) {
            // TODO: implement this
        }
        isWillbedeleted() {
            // TODO: implement this
            return false;
        }
        isRecordingOnGoing() {
            return false;
        }
        hasRecording() {
            return false;
        }
        isTimebased() {
            return false;
        }
        isSeriesButNotRecoredAsSeries() {
            return false;
        }
        isScrambled() {
            return false;
        }
        isStreaming() {
            return false;
        }
        isDeeplink() {
            return false;
        }
        isMultiAudio() {
            return false;
        }
        getDeeplinkAppName() {
            return this._item.getName() || '';
        }
        getDeeplinkUrl() {
            return this._item.getDeeplinkUrl() || '';
        }
        getSubtitleType() {
            return null;
        }
        getSubtitleTypes() {
            return [];
        }
        hasTeletext() {
            return false;
        }
        hasGuidance() {
            return this._item.hasGuidance() || false;
        }
        getGuidance() {
            return this._item.getGuidance() || null;
        }
        getHDLinkage(): ifFreeviewLinkage[] {
            return null;
        }
        isSame(p?) {
            return false;
        }
        getDuration() {
            return this._item.getDuration() || 0;
        }
        getAvailableEndTime() {
            return this._item.getAvailableEndTime() || 0;
        }
    }
    export class cPdeProgrammeItem extends __MODEL__.IPdeProgrammeItem {
        _item: CProgrammeProvider;
        _dummyGroup: any;
        _oipfScheduledObj: any;
        _thumbnailUrl: string = '';
        _logoUrl: string = '';
        constructor(programmeItem? : CProgrammeProvider) {
            super();
            this._item = programmeItem || new CProgrammeProvider(null);
        }
        getSourceItem() {
            return this._item;
        }
        getPluginObject() {
            return this._item.getPluginObject();
        }
        getMdsData() {
            return this._item.getMdsData();
        }
        setMdsData(aParam) {
            this._item.setMdsData(aParam);
        }
        toString() {
            var ret = 'PDE ITEM: type=' + this.getType() + ' id=' + this.getId() + ' seriesId=' + this.getSeriesId() + ' programmeId=' + this.getProgrammeID();
            ret += '\nPDE ITEM: title=' + this.getTitle() + ' channelId=' + this.getChannelId() + ' channelName=' + this.getChannelName();
            return ret;
        }
        getId() {
            return this._item.getProgrammeID() || this._item.getProgrammeCridMds();
        }
        getStartTime() {
            return this._item.getStartTime() ? new Date(this._item.getStartTime() * 1000) : null;
        }
        getEndTime() {
            if (this._item.getStartTime() && this._item.getDuration()) {
                var startTime = this._item.getStartTime() * 1000;
                var duration = this._item.getDuration() * 1000;
                return new Date(startTime + duration);
            } else {
                return null;
            }
        }
        getTitle() {
            return this._item.getName();
        }
        getSecondary() {
            return this._item.getSecondary() || '';
        }
        getThumbnailUrl(updateCb?) {
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
            hx.svc.IP_EPG.getThumbnail(a._item.requestThumbUrl, a.getThumbWidth(), {"content-type" : a._item.requestImgContentType}).then((data) => {
                if (data) {
                    updateCb(a._thumbnailUrl = data);
                } else {
                    updateCb(a.getDefaultThumbnail());
                }

            }, () => {
                updateCb(a.getDefaultThumbnail());
            });
        }
        getDefaultThumbnail() {
            return this.getGenreImage() || __MODEL__.TDefaultThumbnailList.DEFAULT;
        }
        getGenreImage() {
            return this._item.getGenreImage();
        }
        getChannelInstance() {
            var channel = this._item.channel || {};
            return channel;
        }
        getChannelId() {
            var channel = this._item.channel || {};
            return channel.ccid;
        }
        getServiceId() {
            var channel = this._item.channel || {};
            return channel.sid;
        }
        getChannelNo() {
            var channel = this._item.channel || {};
            return channel.majorChannel;
        }
        getChannelName() {
            var channel = this._item.channel || {};
            return channel.name;
        }
        getLogoUrl(updateCb?) {
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
            hx.svc.IP_EPG.getChannelLogo(a.getServiceId(), a.getLogoWidth()).then((data) => {
                if (data) {
                    updateCb(a._logoUrl = data);
                }
            });
        }
        getLongDescription() {
            return fnStringReplaceAll(this._item.getLongDescription());
        }
        getDescription() {
            return fnStringReplaceAll(this._item.getShortDescription());
        }
        getSeriesId() {
            return this._item.groupCRIDs[0];
        }
        getSeasonNo() {
            var seasonNo = this._item.season;
            //getField("season");
            return seasonNo && seasonNo > 0 ? seasonNo : null;
        }
        getEpisodeNo() {
            var episodeNo = this._item.episode;
            return episodeNo && episodeNo > 0 ? episodeNo : null;
        }
        getGenreList() {
            return this._item.getGenre();
        }
        getParentalRating() {
            var parentalRatings = this._item.getParentalRatings();
            if (!parentalRatings) {
                return -1;
            }
            return ( parentalRatings[0] ? parentalRatings[0].value : -1);
        }
        getCasIDs() {
            var i, length, channel;
            channel = this._item.channel || {};
            return channel.casIDs || [];
        }
        getLastPlayPosition() {
            return 0;
        }
        getProgrammeID() {
            return this._item.getProgrammeID();
        }
        hasSchedule() {
            return false;
        }
        hasSeriesSchedule() {
            return false;
        }
        isReminder() {
            return false;
        }
        isOnAir() {
            var startTime = this.getStartTime();
            var endTime = this.getEndTime();
            if (!startTime || !endTime) {
                return false;
            }
            var now = (new Date().getTime());
            return (startTime.getTime() < now && endTime.getTime() > now);
        }
        isPast() {
            var ret = false;
            var isTsr = hx.svc.TSR.isTsrOngoing();
            var now = (new Date().getTime());
            var startTime = this.getStartTime().getTime();
            var endTime = this.getEndTime().getTime();
            if (startTime < now && endTime > now) {
                if (isTsr) {
                    ret = true;
                }
            } else if (startTime < now && endTime < now) {
                ret = true;
            }
            return ret;
        }
        getGroupCRIDs() {
            return this._item.getGroupCRIDs();
        }
        getProgrammeCRID() {
            return this._item.getProgrammeCRID();
        }
        getRecommendationCRID() {
            return this._item.getRecommendationCRID();
        }
        hasGroupCRIDs() {
            if (this._item && this._item.groupCRIDs && this._item.groupCRIDs.length && this._item.groupCRIDs.length > 0) {
                return true;
            }
            return false;
        }
        getKeywordQuery() {
            var groupCRIDs, i, keyword;
            var groupCRID_datapart;
            if (hx.config.useRfCRID === true) {
                if (this.hasGroupCRIDs() === true) {
                    groupCRIDs = this.getGroupCRIDs();
                    for (i = 0; i < groupCRIDs.length; i += 1) {
                        groupCRID_datapart = fnGetSeriesID(groupCRIDs[i]);
                        if (i === 0) {
                            keyword = hx.util.format("keyword %% sCRID:%{0}, ", groupCRID_datapart);
                        } else {
                            keyword = keyword + hx.util.format(" OR keyword %% sCRID:%{0}, ", groupCRID_datapart);
                        }
                    }
                }
                return keyword;
            }
            return '';
        }
        setDummyGroup() {
            this._dummyGroup = true;
        }
        isSeries() {
            var isSeries = false, i = 0, GroupCRIDs, crid;
            if (hx.config.useRfCRID === true && this.hasGroupCRIDs() === true) {
                isSeries = true;
                /*
                 if (hx.log.exist('dsrvDetailInfo2') === true) {
                 GroupCRIDs = this.getGroupCRIDs();
                 if (GroupCRIDs) {
                 for ( i = 0; i < GroupCRIDs.length; i += 1) {
                 crid = GroupCRIDs.item(i);
                 hx.log('dsrvDetailInfo2', '[crid(' + i + ')]' + crid);
                 }
                 }
                 }// hx.log.exist
                 */
            }
            if (hx.config.useRfCRID === true && this._dummyGroup === true) {
                return true;
            }
            return isSeries;
        }
        isPlayable() {
            return false;
        }
        isNew() {
            return false;
        }
        isHD() {
            return this._item.isHd();
        }
        isAD() {
            return this._item.isAd();
        }
        isLocked() {
            return this._item.isLocked();
        }
        isKept() {
            // TODO: implement this
            return false;
        }
        setKeep(aKeep) {
            // TODO: implement this
        }
        isWillbedeleted() {
            // TODO: implement this
            return false;
        }
        isRecordingOnGoing() {
            return false;
        }
        hasRecording() {
            return false;
        }
        isTimebased() {
            return false;
        }
        isSeriesButNotRecoredAsSeries() {
            return false;
        }
        isScrambled() {
            return false;
        }
        isStreaming() {
            var ret = false;
            if (this._oipfScheduledObj.factoryType === _oipfDef.TScheduledFactoryType.EHTTP_LIVE_STREAMING) {
                ret = true;
            }
            return ret;
        }
        isDeeplink() {
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
        }
        isMultiAudio() {
            return this._item.isMultiAudio();
        }
        getDeeplinkAppName() {
            return this.getTitle();
        }
        getDeeplinkUrl() {
            return this._item.getDeeplinkUrl();
        }
        getAvailability() {
            return this._item.getAvailability();
        }
        getSubtitleType() {
            return this._item.getSubtitleType();
        }
        getSubtitleTypes() {
            return this._item.getSubtitleTypes();
        }
        hasTeletext() {
            if (this._item.channel) {
                return this._item.channel.hasTeletext;
            }
            return false;
        }
        hasGuidance() {
            return this._item.hasGuidance();
        }
        getGuidance() {
            return this._item.getGuidance();
        }
        getHDLinkage(): ifFreeviewLinkage[] {
            return this._item.getHDLinkage();
        }
        isSame(b) {
            var matcherType;
            if ( this instanceof cPdeProgrammeItem && b instanceof cPdeProgrammeItem) {
                matcherType = 'pp';
            } else if ( this instanceof cPdeProgrammeItem && b instanceof cPdeRecordingItem) {
                matcherType = 'pr';
            } else if ( this instanceof cPdeRecordingItem && b instanceof cPdeProgrammeItem) {
                matcherType = 'rp';
            } else if ( this instanceof cPdeProgrammeItem && b instanceof cPdeScheduledItem) {
                matcherType = 'ps';
            } else if ( this instanceof cPdeScheduledItem && b instanceof cPdeProgrammeItem) {
                matcherType = 'sp';
            } else if ( this instanceof cPdeRecordingItem && b instanceof cPdeRecordingItem) {
                matcherType = 'rr';
            } else if ( this instanceof cPdeRecordingItem && b instanceof cPdeScheduledItem) {
                matcherType = 'rs';
            } else if ( this instanceof cPdeScheduledItem && b instanceof cPdeRecordingItem) {
                matcherType = 'sr';
            } else if ( this instanceof cPdeScheduledItem && b instanceof cPdeScheduledItem) {
                matcherType = 'ss';
            } else {
                throw "INVALID PARAM";
            }

            var matcher = {
                'pp' : () => {
                    return isSameProgrammeVsProgramme(this, b);
                },
                'pr' : () => {
                    return isSameProgrammeVsRecording(this, b);
                },
                'ps' : () => {
                    return isSameProgrammeVsScheduled(this, b);
                },
                'rp' : () => {
                    return isSameProgrammeVsRecording(b, this);
                },
                'sp' : () => {
                    return isSameProgrammeVsScheduled(b, this);
                },
                'rr' : () => {
                    return isSameRecordingVsRecording(this, b);
                },
                'rs' : () => {
                    return isSameRecordingVsScheduled(this, b);
                },
                'sr' : () => {
                    return isSameRecordingVsScheduled(b, this);
                },
                'ss' : () => {
                    return isSameScheduledVsScheduled(this, b);
                }
            };
            return matcher[matcherType]();
        }
    }
    export class cPdeRecordingItem extends __MODEL__.IPdeRecordingItem {
        _prismRecordingItem: any;
        _refProgrammeItem: CProgrammeProvider;
        _thumbnailUrl;
        constructor(prismRecordingItem) {
            super();
            if (prismRecordingItem instanceof __MODEL__.COipfRecordingItem) {
                this._prismRecordingItem = prismRecordingItem;
            } else {
                this._prismRecordingItem = new __MODEL__.COipfRecordingItem(prismRecordingItem.pluginObject);
            }
        }
        getSourceItem() {
            return this._prismRecordingItem;
        }
        toString() {
            return 'PDE ITEM: type=' + this.getType() + ' id=' + this.getId() + ' seriesId=' + this.getSeriesId() + ' programmeId=' + this.getProgrammeID();
        }
        getId() {
            return this._prismRecordingItem.getId();
        }
        getStartTime() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return new Date(that._refProgrammeItem.getStartTime() * 1000);
            }
            return that._prismRecordingItem.getStartTime();
        }
        getEndTime() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return new Date(that._refProgrammeItem.getEndTime() * 1000);
            }
            var duration = that._prismRecordingItem.getDuration();
            var startTime = that._prismRecordingItem.getStartTime();
            return new Date((startTime.getTime() + duration * 1000) + 30000);
        }
        getTitle() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return that._refProgrammeItem.getName();
            }
            return that._prismRecordingItem.getTitle();
        }
        getSecondary() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return that._refProgrammeItem.getSecondary();
            }
            return that.getDescription();
        }
        getThumbnailUrl(updateCb?) {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
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
                hx.svc.IP_EPG.getThumbnail(that._refProgrammeItem.requestThumbUrl, that.getThumbWidth(), {"content-type" : that._refProgrammeItem.requestImgContentType}).then((data) => {
                    if (data) {
                        updateCb(that._thumbnailUrl = data);
                    }
                }, () => {});
            }
            var thumbnailUrl = that._prismRecordingItem.getThumbnailUrl();
            if (!thumbnailUrl) {
                if (updateCb) {
                    setTimeout(() => {
                        hx.log('error', 'update Thumbnail for recording item after setTimeout 1000');
                        updateCb(that._prismRecordingItem.getThumbnailUrl());
                    }, 1000);
                }
                thumbnailUrl = that._prismRecordingItem.isBroken() ? __MODEL__.TDefaultThumbnailList.REC_FAIL : __MODEL__.TDefaultThumbnailList.RECORDING;
            }
            return thumbnailUrl;
        }
        getChannelInstance() {
            return this._prismRecordingItem.getChannelInstance();
        }
        getChannelId() {
            return this._prismRecordingItem.getChannelId();
        }
        getServiceId() {
            return this._prismRecordingItem.getServiceId();
        }
        getChannelNo() {
            return this._prismRecordingItem.getChannelNo();
        }
        getChannelName() {
            return this._prismRecordingItem.getChannelName();
        }
        getLogoUrl(updateCb?) {
            var that = this;
            var logoURL = that._prismRecordingItem.getLogoUrl(that.getLogoWidth());
            if (!logoURL && updateCb) {
                hx.svc.IP_EPG.getChannelLogo(that.getServiceId(), that.getLogoWidth()).then(function (data) {
                    updateCb(data);
                });
            }
            return logoURL;
        }
        getLongDescription() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return that._refProgrammeItem.getLongDescription();
            }
            return fnStringReplaceAll(that._prismRecordingItem.getLongDescription());
        }
        getDescription() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return that._refProgrammeItem.getShortDescription();
            }
            return fnStringReplaceAll(that._prismRecordingItem.getShortDescription());
        }
        getSeriesId() {
            return this._prismRecordingItem.getSeriesId();
        }
        getGenreList() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return that._refProgrammeItem.getGenre();
            }
            return that._prismRecordingItem.getGenre();
        }
        getParentalRating() {
            var that = this;
            var parentalRating;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                parentalRating = that._refProgrammeItem.getParentalRatings();
            } else {
                parentalRating = that._prismRecordingItem.getParentalRatings();
            }
            if (!parentalRating) {
                return -1;
            }
            parentalRating = parentalRating[0];
            return ( parentalRating ? parentalRating.value : -1);
        }
        getCasIDs() {
            return this._prismRecordingItem.getCasIDs();
        }
        getLastPlayPosition() {
            return this._prismRecordingItem.getLastPlayPosition();
        }
        getOipfRecordingInstance() {
            return this._prismRecordingItem.getInstance();
        }
        getProgrammeID() {
            return this._prismRecordingItem.getProgrammeID();
        }
        hasSchedule() {
            return false;
        }
        hasSeriesSchedule() {
            return false;
        }
        isReminder() {
            return false;
        }
        isOnAir() {
            return this._prismRecordingItem.isRecordingOnGoing();
        }
        isPast() {
            return false;
        }
        isSeries() {
            return this._prismRecordingItem.isSeries();
        }
        isPendingSeriesSchedule() {
            var that = this;
            var endPadding = that._prismRecordingItem.getEndPadding();
            var endTime = new Date((that.getEndTime() + endPadding) * 1000);
            var now = new Date();
            return (( endTime.getTime() < now.getTime() ) && (that._prismRecordingItem.isScheduledAsSeries()));
        }
        isPlayable() {
            return true;
        }
        isNew() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return false;
            }
            return that._prismRecordingItem.isNew();
        }
        wasWatching() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return false;
            }
            return that._prismRecordingItem.wasWatching();
        }
        isHD() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return that._refProgrammeItem.isHd();
            }
            return that._prismRecordingItem.isHd();
        }
        isAD() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return that._refProgrammeItem.isAd();
            }
            return that._prismRecordingItem.isAd();
        }
        isEncrypted() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return false;
            }
            return that._prismRecordingItem.isEncrypted();
        }
        isDRM() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return false;
            }
            return that._prismRecordingItem.isNoCopy();
        }
        isLocked() {
            return this._prismRecordingItem.isLocked();
        }
        isKept() {
            return this._prismRecordingItem.isKept();
        }
        setKeep(aKeep) {
            this._prismRecordingItem.setKeep(aKeep);
        }
        isWillbedeleted() {
            return this._prismRecordingItem.isWillbedeleted();
        }
        isRecordingOnGoing() {
            return this._prismRecordingItem.isRecordingOnGoing();
        }
        hasRecording() {
            return true;
        }
        isTimebased() {
            return this._prismRecordingItem.getProgrammeID() ? false : true;
        }
        isSeriesButNotRecoredAsSeries() {
            var that = this;
            return that._prismRecordingItem.isSeries() && !that._prismRecordingItem.isScheduledAsSeries();
        }
        isScheduledAsSeries() {
            return this._prismRecordingItem.isScheduledAsSeries();
        }
        isScrambled() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return false;
            }
            return that._prismRecordingItem.isScrambled();
        }
        isStreaming() {
            var ret = false;
            return ret;
        }
        isMultiAudio() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return that._refProgrammeItem.isMultiAudio();
            }
            if (that._prismRecordingItem.getAudioType() === 4) {
                return true;
            }
            return false;
        }
        hasGroupCRIDs() {
            if (this.getGroupCRIDs().length > 0) {
                return true;
            }
            return false;
        }
        getKeywordQuery() {
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
                        } else {
                            keyword = keyword + hx.util.format(" OR keyword %% sCRID:%{0}, ", groupCRID_datapart);
                        }
                    }
                }
                return keyword;
            }
        }
        getGroupCRIDs() {
            return this._prismRecordingItem.getGroupCRIDs();
        }
        getProgrammeCRID() {
            return this._prismRecordingItem.getProgrammeCRID();
        }
        getRecommendationCRID() {
            var that = this;
            if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
                return that._refProgrammeItem.getRecommendationCRID();
            }
            return false;
            //return this._prismRecordingItem.getRecommendationCRID();
        }
        setRefProgrammeItem(refProgrammeItem) {
            this._refProgrammeItem = refProgrammeItem;
        }
        getRefProgrammeItem() {
            if (this._refProgrammeItem) {
                return this._refProgrammeItem;
            }
            return null;
        }
        getEventIDFromProgrammeID() {
            var eventID = null, tmp = null;
            var programmeID = this.getProgrammeID();
            if (programmeID) {
                tmp = programmeID.split(';');
                if (tmp.length > 1) {
                    eventID = parseInt(tmp[1], 16).toString(10);
                }
            }
            return eventID;
        }
        // for Deeplink
        isDeeplink() {
            var ret = false;
            return ret;
        }
        getDeeplinkAppName() {
            var that = this;
            var ret = null;
            if (that._refProgrammeItem) {
                ret = that._refProgrammeItem.getName();
            }
            return ret;
        }
        getDeeplinkUrl() {
            var ret = null;
            if (this._refProgrammeItem) {
                ret = this._refProgrammeItem.getDeeplinkUrl();
            }
            return ret;
        }
        getSubtitleType() {
            var type = 0;
            return type;
        }
        getSubtitleTypes() {
            return this._prismRecordingItem.getSubtitleTypes();
        }
        hasTeletext() {
            var channel;
            channel = this._prismRecordingItem.getChannelInstance();
            if (!channel) {
                return false;
            }
            return channel.hasTeletext;
        }
        setEpisodeLock(param) {
            var lock = param, oipfRecording;
            oipfRecording = this._prismRecordingItem.getInstance();
            oipfRecording.isManualLocked = lock;
        }
        hasGuidance() {
            var that = this;
            //if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
            //    return that._refProgrammeItem.hasGuidance();
            //}
            return that._prismRecordingItem.hasGuidance();
        }
        getGuidance() {
            var that = this;
            //if (that._refProgrammeItem && (that._refProgrammeItem.getAvailability() === EOndemandAvailable.EAvailable)) {
            //    return that._refProgrammeItem.getGuidance();
            //}
            return that._prismRecordingItem.getGuidance();
        }
        getHDLinkage() {
            return false;
        }
        isSame(b) {
            var matcherType;
            if ( this instanceof cPdeProgrammeItem && b instanceof cPdeProgrammeItem) {
                matcherType = 'pp';
            } else if ( this instanceof cPdeProgrammeItem && b instanceof cPdeRecordingItem) {
                matcherType = 'pr';
            } else if ( this instanceof cPdeRecordingItem && b instanceof cPdeProgrammeItem) {
                matcherType = 'rp';
            } else if ( this instanceof cPdeProgrammeItem && b instanceof cPdeScheduledItem) {
                matcherType = 'ps';
            } else if ( this instanceof cPdeScheduledItem && b instanceof cPdeProgrammeItem) {
                matcherType = 'sp';
            } else if ( this instanceof cPdeRecordingItem && b instanceof cPdeRecordingItem) {
                matcherType = 'rr';
            } else if ( this instanceof cPdeRecordingItem && b instanceof cPdeScheduledItem) {
                matcherType = 'rs';
            } else if ( this instanceof cPdeScheduledItem && b instanceof cPdeRecordingItem) {
                matcherType = 'sr';
            } else if ( this instanceof cPdeScheduledItem && b instanceof cPdeScheduledItem) {
                matcherType = 'ss';
            } else {
                throw "INVALID PARAM";
            }

            var matcher = {
                'pp' : () => {
                    return isSameProgrammeVsProgramme(this, b);
                },
                'pr' : () => {
                    return isSameProgrammeVsRecording(this, b);
                },
                'ps' : () => {
                    return isSameProgrammeVsScheduled(this, b);
                },
                'rp' : () => {
                    return isSameProgrammeVsRecording(b, this);
                },
                'sp' : () => {
                    return isSameProgrammeVsScheduled(b, this);
                },
                'rr' : () => {
                    return isSameRecordingVsRecording(this, b);
                },
                'rs' : () => {
                    return isSameRecordingVsScheduled(this, b);
                },
                'sr' : () => {
                    return isSameRecordingVsScheduled(b, this);
                },
                'ss' : () => {
                    return isSameScheduledVsScheduled(this, b);
                }
            };
            return matcher[matcherType]();
        }
    }
    export class cPdeScheduledItem extends __MODEL__.IPdeScheduledItem {
        _prismScheduledItem: __MODEL__.COipfScheduledItem;
        _programme: __MODEL__.COipfScheduledItem;
        _item: any;
        constructor(scheduledItem) {
            super();
            this._prismScheduledItem = scheduledItem;
        }
        private _getProgramme() {
            var schedule = this._prismScheduledItem;
            hx.svc.EPG.getProgrammes({
                channel: schedule.getChannelInstance(),
                clause: "programmeID == " + this.getProgrammeID(),
                notUI: true,
                cb: (result) => {
                    if (result[0][0]) {
                        this._programme = new __MODEL__.COipfScheduledItem(result[0][0]);
                    }
                    result = undefined;
                    schedule = undefined;
                }
            })
        }
        getSourceItem() {
            return this._prismScheduledItem;
        }
        toString() {
            return 'PDE ITEM: type=' + this.getType() + ' id=' + this.getId() + ' seriesId=' + this.getSeriesId() + ' programmeId=' + this.getProgrammeID();
        }
        getId() {
            return this._prismScheduledItem.getId();
        }
        getStartTime() {
            return this._prismScheduledItem.getStartTime();
        }
        getEndTime() {
            var that = this;
            var duration = that._prismScheduledItem.getDuration();
            var startTime = that._prismScheduledItem.getStartTime();
            return new Date(startTime.getTime() + duration * 1000);
        }
        getTitle() {
            return this._prismScheduledItem.getTitle();
        }
        getThumbnailUrl() {
            return this._prismScheduledItem.getThumbnailUrl();
        }
        getChannelInstance() {
            return this._prismScheduledItem.getChannelInstance();
        }
        getChannelId() {
            return this._prismScheduledItem.getChannelId();
        }
        getServiceId() {
            return this._prismScheduledItem.getServiceId();
        }
        getChannelNo() {
            return this._prismScheduledItem.getChannelNo();
        }
        getChannelName() {
            return this._prismScheduledItem.getChannelName();
        }
        getLogoUrl(updateCb?) {
            var that = this;
            var logoURL = that._prismScheduledItem.getLogoUrl(that.getLogoWidth());
            if (!logoURL && updateCb) {
                hx.svc.IP_EPG.getChannelLogo(that.getServiceId(), that.getLogoWidth()).then(function (data) {
                    updateCb(data);
                });
            }
            return logoURL;
        }
        getLongDescription() {
            var programme = this._programme;
            var ret = "";
            if (programme) {
                ret = this._programme.getLongDescription();
            }
            return fnStringReplaceAll(ret);
        }
        getDescription() {
            var programme = this._programme;
            var ret = "";
            if (programme) {
                ret = this._programme.getShortDescription();
            }
            return fnStringReplaceAll(ret);
        }
        getSeriesId() {
            return this._prismScheduledItem.getSeriesId();
        }
        getSeasonNo() {
            // TODO: implement this
            return null;
        }
        getEpisodeNo() {
            // TODO: implement this
            return null;
        }
        getGenreList() {
            // TODO: implement this
            return [];
        }
        getParentalRating() {
            var parentalRating;
            var parentalRatings = this._prismScheduledItem._property.parentalRatings;
            if (parentalRatings !== undefined) {
                parentalRating = parentalRatings[0];
            }
            return ( parentalRating ? parentalRating.value : -1);
        }
        getCasIDs() {
            var channel = this._prismScheduledItem.getChannelInstance();
            return channel.casIDs || [];
        }
        getLastPlayPosition() {
            return 0;
        }
        getOipfScheduledInstance() {
            return this._prismScheduledItem.getInstance();
        }
        getProgrammeID() {
            return this._prismScheduledItem.getProgrammeID();
        }
        hasSchedule() {
            return true;
        }
        hasSeriesSchedule() {
            // TODO: implement this
            return null;
        }
        isPendingSeriesSchedule() {
            var endTime = new Date((this._prismScheduledItem._property.startTime + this._prismScheduledItem._property.duration + this._prismScheduledItem._property.endPadding) * 1000);
            var now = new Date();
            return (( endTime.getTime() < now.getTime() ) && (this.getOipfScheduledInstance().isScheduledAsSeries));
        }
        isReminder() {
            return this.getOipfScheduledInstance().factoryType === _oipfDef.TScheduledFactoryType.EREMIND;
        }
        isOnAir() {
            var now = new Date();
            return this.getStartTime().getTime() < now.getTime();
        }
        isPast() {
            return false;
        }
        isSeries() {
            return this._prismScheduledItem.isSeries();
        }
        isPlayable() {
            return false;
        }
        isNew() {
            return false;
        }
        isHD() {
            var channel = this._prismScheduledItem.getChannelInstance();
            return channel.isHD;
        }
        isAD() {
            //return this._item.getField("freeview/hasAudioDescription");
            return false;
        }
        isDRM() {
            // TODO: implement this
            return false;
        }
        isLocked() {
            // TODO: implement this
            return false;
        }
        isKept() {
            // TODO: implement this
            return false;
        }
        setKeep(aKeep) {
            // TODO: implement this
        }
        isWillbedeleted() {
            // TODO: implement this
            return false;
        }
        isRecordingOnGoing() {
            return false;
        }
        hasRecording() {
            return false;
        }
        isTimebased() {
            return this._prismScheduledItem.getProgrammeID() ? false : true;
        }
        isSeriesButNotRecoredAsSeries() {
            var that = this;
            return that._prismScheduledItem.isSeries() && !that._prismScheduledItem.isScheduledAsSeries();
        }
        isScheduledAsSeries() {
            return this._prismScheduledItem.isScheduledAsSeries();
        }
        isScrambled() {
            return false;
        }
        isStreaming() {
            return this._prismScheduledItem.isStreaming();
        }
        isMultiAudio() {
            var that = this;
            var audioType = that._prismScheduledItem.getAudioType();
            if (audioType === 4) {
                return true;
            }
            return false;
        }
        hasGroupCRIDs() {
            var that = this;
            if (that.getGroupCRIDs() && that.getGroupCRIDs().length > 0) {
                return true;
            }
            return false;
        }
        getKeywordQuery() {
            var that = this;
            var groupCRIDs, i, keyword;
            var groupCRID_datapart;
            if (hx.config.useRfCRID === true) {
                if (that.hasGroupCRIDs() === true) {
                    groupCRIDs = that.getGroupCRIDs();
                    for (i = 0; i < groupCRIDs.length; i += 1) {
                        groupCRID_datapart = fnGetSeriesID(groupCRIDs[i]);
                        if (i === 0) {
                            keyword = hx.util.format("keyword %% sCRID:%{0}, ",groupCRID_datapart);
                        } else {
                            keyword = keyword + hx.util.format(" OR keyword %% sCRID:%{0}, ", groupCRID_datapart);
                        }
                    }
                } else {
                    if (that.getProgrammeID() !== null) {
                        keyword = " programmeId == " + that.getProgrammeID();
                    }
                }
                return keyword;
            }
        }
        getGroupCRIDs() {
            return this._prismScheduledItem.getGroupCRIDs();
        }
        getProgrammeCRID() {
            return this._prismScheduledItem.getProgrammeCRID();
        }
        getRecommendationCRID() {
            return false;
            //var oipfScheduled = this._prismScheduledItem.getInstance();
            //return oipfScheduled.recommendationCRID;
        }
        // for Deeplink
        isDeeplink() {
            var ret = false;
            return ret;
        }
        getDeeplinkAppName() {
            var ret = null;
            return ret;
        }
        getDeeplinkUrl() {
            var ret = null;
            return ret;
        }
        getSubtitleType() {
            var type = 0;
            return type;
        }
        getSubtitleTypes() {
            var types;
            if (this._item === undefined) {
                return "";
            }
            types = this._item.subtitleTypes;
            return types;
        }
        hasTeletext() {
            if (this._item === undefined) {
                return false;
            }
            return this._item.channel.hasTeletext;
        }
        hasGuidance() {
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
        }
        getGuidance() {
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
        }
        getHDLinkage() {
            return false;
        }
        isSame(b) {
            var matcherType;
            if ( this instanceof cPdeProgrammeItem && b instanceof cPdeProgrammeItem) {
                matcherType = 'pp';
            } else if ( this instanceof cPdeProgrammeItem && b instanceof cPdeRecordingItem) {
                matcherType = 'pr';
            } else if ( this instanceof cPdeRecordingItem && b instanceof cPdeProgrammeItem) {
                matcherType = 'rp';
            } else if ( this instanceof cPdeProgrammeItem && b instanceof cPdeScheduledItem) {
                matcherType = 'ps';
            } else if ( this instanceof cPdeScheduledItem && b instanceof cPdeProgrammeItem) {
                matcherType = 'sp';
            } else if ( this instanceof cPdeRecordingItem && b instanceof cPdeRecordingItem) {
                matcherType = 'rr';
            } else if ( this instanceof cPdeRecordingItem && b instanceof cPdeScheduledItem) {
                matcherType = 'rs';
            } else if ( this instanceof cPdeScheduledItem && b instanceof cPdeRecordingItem) {
                matcherType = 'sr';
            } else if ( this instanceof cPdeScheduledItem && b instanceof cPdeScheduledItem) {
                matcherType = 'ss';
            } else {
                throw "INVALID PARAM";
            }

            var matcher = {
                'pp' : () => {
                    return isSameProgrammeVsProgramme(this, b);
                },
                'pr' : () => {
                    return isSameProgrammeVsRecording(this, b);
                },
                'ps' : () => {
                    return isSameProgrammeVsScheduled(this, b);
                },
                'rp' : () => {
                    return isSameProgrammeVsRecording(b, this);
                },
                'sp' : () => {
                    return isSameProgrammeVsScheduled(b, this);
                },
                'rr' : () => {
                    return isSameRecordingVsRecording(this, b);
                },
                'rs' : () => {
                    return isSameRecordingVsScheduled(this, b);
                },
                'sr' : () => {
                    return isSameRecordingVsScheduled(b, this);
                },
                'ss' : () => {
                    return isSameScheduledVsScheduled(this, b);
                }
            };
            return matcher[matcherType]();
        }
    }
    export class cPdeProgrammeScheduledItem extends cPdeProgrammeItem {
        _oipfScheduledObj: any;
        constructor(aProgramme, oipfScheduledObj) {
            super(aProgramme);
            this._item = aProgramme;
            this._oipfScheduledObj = oipfScheduledObj;
        }
        getType() {
            return __MODEL__.IProgrammeDetailItem.type.scheduled;
        }
        hasSchedule() {
            return true;
        }
        hasSeriesSchedule() {
            return this._oipfScheduledObj.isScheduledAsSeries;
        }
        getRefProgrammeItem() {
            if (this._item) {
                return this._item;
            }
            return null;
        }
        isReminder() {
            return this._oipfScheduledObj.factoryType === _oipfDef.TScheduledFactoryType.EREMIND;
        }
        isTimebased() {
            return this._oipfScheduledObj.programmeID ? false : true;
        }
        isSeriesScheduled() {
            return this._oipfScheduledObj.isScheduledAsSeries;
        }
        isSeriesButNotRecoredAsSeries() {
            return this.isSeries() && !this._oipfScheduledObj.isScheduledAsSeries;
        }
        getOipfScheduledInstance() {
            return this._oipfScheduledObj;
        }
    }
}

export = mProvider;
