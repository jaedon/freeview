/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvAvCtrl.js
/*jslint nomen:true, vars:true */
/*global hx*/
/*global $*/
//    AV_CTRL: ['layer_domain/service/dl_dsrvAvCtrl.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "pages/util/provider", "def/oipf.def"], function (require, exports, __KERNEL__, __PROVIDER__, __DEF__) {
    var BROADCAST_PLAYSTATE_CONNECTING = 1;
    var BROADCAST_PLAYSTATE_PRESENTING = 2;
    var BROADCAST_PLAYSTATE_STOPPED = 3;
    var REC_STATE_UNREALIZED = 0;
    var REC_STATE_SCHEDULED = 1;
    var REC_STATE_REC_PRESTART = 2;
    var REC_STATE_REC_ACQUIRING_RESOURCES = 3;
    var REC_STATE_REC_STARTED = 4;
    var REC_STATE_REC_UPDATED = 5;
    var REC_STATE_REC_COMPLETED = 6;
    var REC_STATE_TS_ACQUIRING_RESOURCES = 7;
    var REC_STATE_TS_STARTED = 8;
    var REC_STATE_ERROR = 9;
    var PLAY_STATE_STOPPED = 0;
    var PLAY_STATE_PLAYING = 1;
    var PLAY_STATE_PAUSED = 2;
    var PLAY_STATE_CONNECTING = 3;
    var PLAY_STATE_BUFFERING = 4;
    var PLAY_STATE_FINISHED = 5;
    var PLAY_STATE_ERROR = 6;
    var mTSROnGoing = false;
    var mLiveStopping = false;
    var mFCCTimer;
    var FCC_VIDEO_ID_1 = 1;
    var FCC_VIDEO_ID_2 = 2;
    // indicate time shift recording play back is on going
    /**********************************************************
     * Definitions
     **********************************************************/
    var _oipfDef = {
        TAudioSubType: {
            SUBTYPE_NONE: 0,
            SUBTYPE_AUDIO_SINGLEMONO: 1,
            SUBTYPE_AUDIO_DUALMONO: 2,
            SUBTYPE_AUDIO_STEREO: 3,
            SUBTYPE_AUDIO_MULTILINGUAL: 4,
            SUBTYPE_AUDIO_SURROUND: 5,
            SUBTYPE_AUDIO_DOLBY: 6
        },
        TComponentType: {
            COMPONENT_TYPE_VIDEO: 0,
            COMPONENT_TYPE_AUDIO: 1,
            COMPONENT_TYPE_SUBTITLE: 2
        }
    };
    var mUpdateHysteresisTimer = null;
    var mUpdateHysteresisBuffer = [];
    var mbDisplaySubt = true;
    var mLastTimeStamp = 0;
    var mParamBlockAV;
    var mbBlock;
    var mOnId;
    var mTsId;
    var mSId;
    var mChName;
    var cDLAvCtrl = (function (_super) {
        __extends(cDLAvCtrl, _super);
        function cDLAvCtrl(aServiceName) {
            _super.call(this, aServiceName);
            this._prop = ['componentTag', 'pid', 'type', 'encoding', 'encrypted', 'has_aspectRatio', 'aspectRatio', 'language', 'audioDescription', 'audioChannels', 'hearingImpaired', 'subType', 'language2'];
            this.mLastTimeStamp = 0;
            this.videoBroadcast = this.getPluginInstance("VideoBroadcast");
            this.avController = this.getPluginInstance("AVController");
            this.operatorFeature = this.getPluginInstance("OperatorFeature");
            this._updateHysteresisTimer = null;
            this._updateHysteresisBuffer = [];
            this._bDisplaySubt = true;
            this._blockAV = false;
            this._paramBlockAV = null;
            this._blockAvTaskTimer = null;
            this._channelChanging = false;
            this._canNaviChannel = true;
            this._mhegIcsStatus = 0; //0 ICS OFF, 1 ICS ON
            hx.logadd('dsrvAvCtrl');
            hx.log('dsrvAvCtrl', 'create');
            $('#osd').append(this.videoBroadcast);
            this._initEventHandler();
        }
        cDLAvCtrl.prototype.setID = function (data) {
            if (hx.emulMode) {
                $("#" + data.parentID).append($("<div>").attr({
                    id: data.videoID
                }));
                return;
            }
            throw "this.vb ?";
            //        $("#" + data.parentID).append($(this.vb).attr({
            //            id : data.videoID
            //        }));
        };
        cDLAvCtrl.prototype.channelChange = function (param, aTuneInfo) {
            var _this = this;
            if (aTuneInfo === void 0) { aTuneInfo = 0; }
            this.fireEvent('evtSetChannel', {
                'channel': param.channel,
                'tuneInfo': aTuneInfo
            });
            hx.log("debug", "channelChange evtSetChannel fire success!!");
            //update channel group
            if (param.channel && !param.group) {
                var retObj = hx.svc.CH_LIST.getChannel({
                    'group': undefined,
                    'ccid': param.channel.ccid
                });
                param.group = retObj.group;
            }
            hx.svc.WEB_STORAGE.updateLastService({
                'ccid': param.channel.ccid,
                'group': param.group
            });
            hx.log("debug", "channelChange webstorage updatelastservice success!!");
            setTimeout(function () {
                if (aTuneInfo > 0) {
                    hx.log("debug", "tuneinfo: " + parseInt(aTuneInfo.toString(), 16));
                    _this._setChannel({
                        'channel': param.channel
                    }, aTuneInfo);
                }
                else {
                    _this._setChannel({
                        'channel': param.channel,
                        'trickplay': false,
                        'contentAccessDescriptorURL': ""
                    });
                }
            }, 30);
        };
        cDLAvCtrl.prototype.stopCurChannel = function (param) {
            if (param.stopTsr) {
                this.videoBroadcast.stopTimeshift();
            }
            if (param.stopAV) {
                this.videoBroadcast.stop();
            }
        };
        cDLAvCtrl.prototype.isLiveOnGoing = function () {
            var playState = this.videoBroadcast.playState;
            if (playState === BROADCAST_PLAYSTATE_CONNECTING || playState === BROADCAST_PLAYSTATE_PRESENTING) {
                return true;
            }
            return false;
        };
        cDLAvCtrl.prototype.isMpbOnGoing = function () {
            var vodPlatState = this.avController.playState;
            if (vodPlatState === PLAY_STATE_PLAYING || vodPlatState === PLAY_STATE_PAUSED || vodPlatState === PLAY_STATE_CONNECTING || vodPlatState === PLAY_STATE_BUFFERING) {
                return true;
            }
            return false;
        };
        cDLAvCtrl.prototype.isLiveStopping = function () {
            return mLiveStopping;
        };
        cDLAvCtrl.prototype.blockAV = function (param) {
            var _this = this;
            this._blockAV = true;
            this._paramBlockAV = param;
            hx.log("dsrvAvCtrl", "blockAV");
            var fnDoVideoOnDemandTask = function () {
                _this.require('PLAYBACK', function (dsrvPlayback) {
                    dsrvPlayback.clearEventCb(_this, 'PlayStateStopped');
                });
                if (_this._blockAvTaskTimer) {
                    clearTimeout(_this._blockAvTaskTimer);
                    _this._blockAvTaskTimer = null;
                }
                if (_this._paramBlockAV && _this._paramBlockAV.cb) {
                    _this._paramBlockAV.cb();
                    delete _this._paramBlockAV.cb;
                    _this._paramBlockAV = null;
                }
            };
            var fnDoVideoBroadcastTask = function () {
                _this.require('TSR', function (dsrvTsr2) {
                    dsrvTsr2.clearEventCb(_this, 'LiveStarted');
                });
                if (_this._blockAvTaskTimer) {
                    clearTimeout(_this._blockAvTaskTimer);
                    _this._blockAvTaskTimer = null;
                }
                if (_this._paramBlockAV && _this._paramBlockAV.cb) {
                    _this._paramBlockAV.cb();
                    delete _this._paramBlockAV.cb;
                    _this._paramBlockAV = null;
                }
            };
            var fnRegistVideoOnDemandTask = function () {
                _this.require('PLAYBACK', function (dsrvPlayback) {
                    dsrvPlayback.addEventCb('PlayStateStopped', _this, function (e) {
                        fnDoVideoOnDemandTask();
                    });
                });
                _this._blockAvTaskTimer = setTimeout(fnDoVideoOnDemandTask, 2000);
            };
            var fnRegistVideoBroadcastTask = function () {
                _this.require('TSR', function (dsrvTsr2) {
                    dsrvTsr2.addEventCb('LiveStarted', _this, function (e) {
                        fnDoVideoBroadcastTask();
                    });
                });
                _this._blockAvTaskTimer = setTimeout(fnDoVideoBroadcastTask, 2000);
            };
            if (this._blockAvTaskTimer) {
                clearTimeout(this._blockAvTaskTimer);
                this._blockAvTaskTimer = null;
            }
            if (mTSROnGoing) {
                mLiveStopping = true;
                setTimeout(function () {
                    mLiveStopping = false;
                }, 5000);
                this.videoBroadcast.stop();
                fnRegistVideoBroadcastTask();
            }
            else {
                if (this.videoBroadcast.playState === BROADCAST_PLAYSTATE_STOPPED) {
                    this.avController.stop();
                    fnRegistVideoOnDemandTask();
                }
                else {
                    this.videoBroadcast.stop();
                    fnRegistVideoBroadcastTask();
                }
            }
            this.fireEvent('BlockAV');
        };
        cDLAvCtrl.prototype.unBlockAV = function () {
            hx.log("dsrvAvCtrl", "unBlockAV");
            this._blockAV = false;
            this._paramBlockAV = null;
            if (this._blockAvTaskTimer) {
                clearTimeout(this._blockAvTaskTimer);
                this._blockAvTaskTimer = null;
            }
        };
        cDLAvCtrl.prototype.availableChannel = function (group) {
            var result = false;
            var listO;
            var retObj;
            var o2;
            var channel;
            var group;
            // get recording channel
            listO = hx.svc.RECORDINGS.getCurrentRecordings();
            if (listO && listO.length > 0) {
                if (!!listO[0]) {
                    o2 = hx.svc.CH_LIST.getChannelByCcidExceptALL({
                        'ccid': listO[0].getChannelId()
                    });
                    if (o2.channel !== null && o2.group !== null) {
                        channel = o2.channel;
                        group = o2.group;
                    }
                }
            }
            // get first channel
            if (undefined === channel || undefined === group) {
                retObj = hx.svc.CH_LIST.getAvailableFirstChannel({
                    group: group
                });
                channel = retObj.channel;
                group = retObj.group;
            }
            // change
            if (undefined !== channel && undefined !== group) {
                this.channelChange({
                    'channel': channel,
                    'group': group
                });
                result = true;
            }
            else {
                result = false;
            }
            return result;
        };
        cDLAvCtrl.prototype.lastChannel = function () {
            var ccid;
            var group;
            var retObj;
            var retObjChCnt = 0;
            var result = false;
            retObj = hx.svc.WEB_STORAGE.getLastService();
            ccid = retObj.ccid;
            group = retObj.group;
            hx.log("error", "[lastChannel] lastChannel");
            if (group !== null) {
                retObjChCnt = hx.svc.CH_LIST.getChannelCount({
                    group: group
                });
                if (retObjChCnt.chCnt === 0) {
                    hx.log("dsrvAvCtrl", "[lastChannel] error : not found service on the channel list : " + group);
                }
            }
            if (retObjChCnt.chCnt > 0 && ccid !== null && group !== null) {
                var o2 = hx.svc.CH_LIST.getChannel({
                    'group': group,
                    'ccid': ccid
                });
                if (o2.changeable === true && o2.channel) {
                    hx.svc.AV_CTRL.channelChange({
                        'channel': o2.channel,
                        'group': o2.group
                    });
                    result = true;
                }
            }
            /*
             * MDS request URL
             * */
            var uri = hx.svc.SETTING_UTIL.getPrefixUrl();
            hx.log('error', 'lastChannel create cahngeLastService uri : ' + uri);
            hx.svc.AV_CTRL._eventSource._fireEvent("NetworkUriChanged", uri);
            return result;
        };
        cDLAvCtrl.prototype.bindToChannel = function () {
            return this.videoBroadcast.bindToCurrentChannel();
        };
        cDLAvCtrl.prototype.isMhegIcsOnGoing = function () {
            return (this._mhegIcsStatus == 1);
        };
        cDLAvCtrl.prototype.naviChannel = function (param) {
            var _this = this;
            var timeOut = 500;
            //300;
            var fun;
            var newSvc;
            var getFunction;
            var retLastSvc;
            var bRet = true;
            if (param && param.direction === 'block') {
                this._canNaviChannel = false;
            }
            else if (param && param.direction === 'unBlock') {
                this._canNaviChannel = true;
            }
            if (!this._canNaviChannel) {
                hx.log('error', 'naviChannel [_canNaviChannel] FAIL');
                return true;
            }
            if (param.direction === 'next') {
                getFunction = 'getNextChannel';
                retLastSvc = hx.svc.WEB_STORAGE.getLastService();
            }
            else if (param.direction === 'prev') {
                getFunction = 'getPrevChannel';
                retLastSvc = hx.svc.WEB_STORAGE.getLastService();
            }
            else if (param.direction === 'back') {
                getFunction = 'getBackChannel';
                retLastSvc = hx.svc.WEB_STORAGE.getBackService();
            }
            else {
                return true;
            }
            var group = retLastSvc.group;
            var ccid = retLastSvc.ccid;
            if (ccid !== null && group !== null) {
                newSvc = hx.svc.CH_LIST[getFunction]({
                    'ccid': ccid,
                    'group': group
                });
                if (newSvc.result === true && newSvc.changeable === true && newSvc.channel) {
                    //------------- Hysteresis ----------------
                    hx.svc.WEB_STORAGE.updateLastService({
                        'ccid': newSvc.channel.ccid,
                        'group': group
                    });
                    this.fireEvent('evtSetChannel', {
                        'channel': newSvc.channel
                    });
                    this._updateHysteresisBuffer.push(newSvc.channel);
                    // fast channel change - if user try to change over 2000msec later then action fast response
                    var diff = new Date().getTime() - this.mLastTimeStamp;
                    if (Math.abs(diff) >= 3000) {
                        timeOut = 150;
                    }
                    //hx.log('error', 'time out : ' + timeOut + '   diff : ' + diff);
                    fun = function () {
                        var len = _this._updateHysteresisBuffer.length;
                        if (len > 0) {
                            var Popchannel = _this._updateHysteresisBuffer.pop();
                            _this._setChannel({
                                'channel': Popchannel,
                                'trickplay': false,
                                'contentAccessDescriptorURL': ""
                            });
                            _this._updateHysteresisBuffer = [];
                        }
                        var curChannel = hx.svc.CH_LIST.getCurrentChannel().channel;
                        if (curChannel) {
                            mOnId = curChannel.onid;
                            mTsId = curChannel.tsid;
                            mSId = curChannel.sid;
                            mChName = curChannel.name;
                            if (hx.svc.STATISTICS) {
                                setTimeout(function () {
                                    var result = hx.svc.EPG.getProgrammes({
                                        isOnAir: true,
                                        filter: "P/F",
                                        channel: curChannel,
                                        listener: _this,
                                        notUI: true,
                                        cb: function (result) {
                                            if (result[0][0]) {
                                                var programmeID = result[0][0].programmeID, tmp, eventID;
                                                var evtName = '';
                                                evtName = result[0][0].name;
                                                if (programmeID) {
                                                    tmp = programmeID.split(';');
                                                    if (tmp.length > 1) {
                                                        eventID = parseInt(tmp[1], 16).toString(10);
                                                    }
                                                }
                                                hx.svc.STATISTICS.statisticsLog({
                                                    "CODE": "ELWS",
                                                    "TRS": "R",
                                                    "ONID": mOnId,
                                                    "TSID": mTsId,
                                                    "SID": mSId,
                                                    "EID": eventID,
                                                    "CHNAME": mChName,
                                                    "EVTNAME": evtName
                                                });
                                                hx.log("dsrvAvCtrl", hx.util.format('statisticsLog - ONID: {0}, TSID: {2}, SID: {3}, EID: {4}, CHNAME: {5}', mOnId, mTsId, mSId, eventID, mChName));
                                            }
                                            result = undefined;
                                        }
                                    });
                                }, 5000);
                            }
                        }
                    };
                    clearTimeout(this._updateHysteresisTimer);
                    this._updateHysteresisTimer = setTimeout(fun, timeOut);
                    this.mLastTimeStamp = new Date().getTime();
                }
                else {
                    bRet = false;
                }
            }
            else {
                if (param.direction === 'next' || param.direction === 'prev') {
                    // tune to available channel
                    hx.log("error", "##### ERROR [dsrvAvCtrl] not found ccid or group : try to change available channel");
                    this.availableChannel();
                }
            }
            return bRet;
        };
        cDLAvCtrl.prototype.getVideoBroadcastCurrentChannel = function () {
            return this.videoBroadcast.currentChannel;
        };
        cDLAvCtrl.prototype.isChannelPlaying = function () {
            return !!this.videoBroadcast.currentChannel;
        };
        cDLAvCtrl.prototype.isIdle = function () {
            if ((this.videoBroadcast.currentChannel) || this._isPbPlaying()) {
                return false;
            }
            return true;
        };
        cDLAvCtrl.prototype.getAVComponent = function () {
            var result;
            result = {
                audio: this._getAudioComponent(),
                subtitle: this._getSubtitleComponent()
            };
            return result;
        };
        cDLAvCtrl.prototype.getAudioComponent = function () {
            var result;
            result = {
                audio: this._getAudioComponent()
            };
            return result;
        };
        cDLAvCtrl.prototype.getSubtitleComponent = function () {
            var result;
            result = {
                subtitle: this._getSubtitleComponent()
            };
            return result;
        };
        cDLAvCtrl.prototype.setAudioComponent = function (param) {
            this._setAudioComponent(param);
        };
        cDLAvCtrl.prototype.setSubtitleComponent = function (param) {
            this._setSubtitleComponent(param);
        };
        cDLAvCtrl.prototype.offSubtitleComponent = function (param) {
            this._offSubtitleComponent();
        };
        cDLAvCtrl.prototype.getCurAudio = function () {
            var audio = null;
            var message = '';
            var encoding = '';
            var p = this._getAudioComponent();
            if (p !== undefined) {
                audio = p;
            }
            if (audio === null || audio.list === null || audio.list.length === 0) {
                message = hx.l('LOC_NOAUDIODATA_ID');
            }
            else {
                var cur = audio.selected;
                var target = null;
                var len = audio.list.length;
                var targetindex = audio.index;
                var aName, szLang2;
                // check targetindex
                if (targetindex === null) {
                    targetindex = 0;
                }
                else {
                    if (targetindex >= len) {
                        targetindex = 0;
                    }
                }
                if (targetindex !== null) {
                    target = audio.list[targetindex];
                }
                // make UI string
                if (target !== null) {
                    var soundidx = targetindex + 1;
                    aName = (target.language !== null) ? hx.il.iso639toString(target.language) : hx.l('LOC_SOUND_ID') + ' ' + soundidx;
                    if (aName.length <= 3) {
                        aName = hx.l('LOC_SOUND_ID') + ' ' + soundidx;
                    }
                    message = aName;
                    if (target.subType === _oipfDef.TAudioSubType.SUBTYPE_AUDIO_DUALMONO) {
                        hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]getCurAudio (subType : " + target.subType + " szLang2 : " + target.language2 + ") ");
                        szLang2 = (target.language2 !== null) ? hx.il.iso639toString(target.language2) : hx.l('LOC_SOUND_ID') + ' ' + (soundidx + 1);
                        message = message + " / " + szLang2;
                    }
                    if (target.encoding) {
                        encoding = target.encoding;
                    }
                }
            }
            return {
                'message': message,
                'encoding': encoding
            };
        };
        cDLAvCtrl.prototype.getCurAudiodescription = function () {
            var audio = null;
            var message = '';
            var encoding = '';
            var p = this._getAudioComponent();
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            if (p !== undefined) {
                audio = p;
            }
            if (audio === null || audio.list === null || audio.list.length === 0) {
                message = hx.l('LOC_NOAUDIODATA_ID');
            }
            else {
                var cur = audio.selected;
                // make UI string
                if (cur.audioDescription) {
                    var _audioDescription;
                    var retObjAudio = dsrvSettingUtil.getAudioInfo({
                        cb: undefined
                    });
                    _audioDescription = retObjAudio.audioDescription;
                    if (_audioDescription === true) {
                        message = hx.l('LOC_AUDIO_DESCRIPTION_ID') + ' : ' + hx.l('LOC_SET_ENGLISH_ID');
                    }
                    else {
                        message = hx.l('LOC_AUDIO_DESCRIPTION_ID') + ' : ' + hx.l('LOC_OFF_ID');
                    }
                }
                else {
                    message = hx.l('LOC_NO_AUDIO_DESCRIPTION_ID');
                }
            }
            return {
                'message': message,
                'encoding': encoding
            };
        };
        cDLAvCtrl.prototype.getCurSubtitle = function () {
            var subtitle = null;
            var message = '';
            var subType = 0;
            var hardofHearing = false;
            var p = this._getSubtitleComponent();
            if (p !== undefined) {
                subtitle = p;
            }
            if (subtitle === null || subtitle.list === null || subtitle.list.length === 0) {
                message = hx.l('LOC_NOSUBTITLE_ID');
            }
            else {
                var cur = subtitle.selected;
                var target = null;
                var len = subtitle.list.length;
                var targetindex = subtitle.index;
                // check tartgetindex
                if ((targetindex !== null) || (targetindex !== undefined)) {
                    if ((targetindex < 0) || (len <= targetindex)) {
                        targetindex = null;
                    }
                    if (targetindex !== null) {
                        target = subtitle.list[targetindex];
                    }
                }
                // make UI string
                if (target === null) {
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_OFF_ID');
                }
                else {
                    var sName;
                    var subtidx = targetindex + 1;
                    sName = (target.language !== null) ? hx.il.iso639toString(target.language) : hx.l('LOC_SUBTITLE_ID') + ' ' + subtidx;
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + sName;
                    if (target.subType) {
                        subType = target.subType;
                    }
                    if (target.hearingImpaired) {
                        hardofHearing = true;
                    }
                }
            }
            return {
                'message': message,
                'subType': subType,
                'hardofHearing': hardofHearing
            };
        };
        cDLAvCtrl.prototype.getCurSubtitleonoff = function () {
            var subtitle = null;
            var message = '';
            var subType = 0;
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var _isSubtitleDisplay = dsrvSettingUtil.getSubtitleEnabled();
            var hardofHearing = false;
            var hardofHearingEnabled = dsrvSettingUtil.getHardofHearing();
            var p = this._getSubtitleComponent();
            if (p !== undefined) {
                subtitle = p;
            }
            if (subtitle === null || subtitle.list === null || subtitle.list.length === 0) {
                message = hx.l('LOC_NOSUBTITLE_ID');
            }
            else {
                var cur = subtitle.selected;
                var target = null;
                var len = subtitle.list.length;
                var targetindex = subtitle.index;
                var targetlang;
                var i;
                var sName;
                var subtidx;
                // toggle
                if (_isSubtitleDisplay === true) {
                    targetlang = hx.svc.SETTING_UTIL.getLanguage().LangSubTtl; //get lang
                    for (i = 0; i < len; i++) {
                        console.log("  subtitle.list[i].language = " + subtitle.list[i].language);
                        if (subtitle.list[i].language === targetlang) {
                            if (subtitle.list[i].hearingImpaired === hardofHearingEnabled) {
                                targetindex = i;
                                break;
                            }
                        }
                    }
                    if (targetindex === null && hardofHearingEnabled === false) {
                        for (i = 0; i < len; i++) {
                            if (subtitle.list[i].language === targetlang) {
                                targetindex = i;
                                break;
                            }
                        }
                    }
                    if (targetindex !== null) {
                        target = subtitle.list[targetindex];
                        console.log(" targetlang " + targetlang + " targetindex " + targetindex + " len " + len);
                        subtidx = targetindex + 1;
                        sName = (target.language !== null) ? hx.il.iso639toString(target.language) : hx.l('LOC_SUBTITLE_ID') + ' ' + subtidx;
                        message = hx.l('LOC_SUBTITLE_ID') + ' : ' + sName;
                        if (target.subType) {
                            subType = target.subType;
                        }
                        if (target.hearingImpaired) {
                            hardofHearing = true;
                        }
                    }
                    else {
                        message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_OFF_ID');
                    }
                }
                else {
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_OFF_ID');
                }
            }
            return {
                'message': message,
                'subType': subType,
                'hardofHearing': hardofHearing
            };
        };
        cDLAvCtrl.prototype.toggleAudio = function () {
            var audio = null;
            var message = '';
            var encoding = '';
            var p = this._getAudioComponent();
            if (p !== undefined) {
                audio = p;
            }
            if (audio === null || audio.list === null || audio.list.length === 0) {
                message = hx.l('LOC_NOAUDIODATA_ID');
            }
            else {
                var cur = audio.selected;
                var target = null;
                var len = audio.list.length;
                var targetindex = audio.index;
                var aName, szLang2;
                // toggle
                if (targetindex === null) {
                    targetindex = 0;
                }
                else {
                    targetindex += 1;
                    if (targetindex >= len) {
                        targetindex = 0;
                    }
                }
                if (targetindex !== null) {
                    target = audio.list[targetindex];
                }
                // set
                if (target !== null) {
                    this._setAudioComponent({
                        'off': cur,
                        'on': target
                    });
                    var soundidx = targetindex + 1;
                    aName = (target.language !== null) ? hx.il.iso639toString(target.language) : hx.l('LOC_SOUND_ID') + ' ' + soundidx;
                    if (aName.length <= 3) {
                        aName = hx.l('LOC_SOUND_ID') + ' ' + soundidx;
                    }
                    message = aName;
                    if (target.subType === _oipfDef.TAudioSubType.SUBTYPE_AUDIO_DUALMONO) {
                        hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]toggleAudio (subType : " + target.subType + " szLang2 : " + target.language2 + ") ");
                        szLang2 = (target.language2 !== null) ? hx.il.iso639toString(target.language2) : hx.l('LOC_SOUND_ID') + ' ' + (soundidx + 1);
                        message = message + " / " + szLang2;
                    }
                    if (target.encoding) {
                        encoding = target.encoding;
                    }
                }
            }
            return {
                'message': message,
                'encoding': encoding
            };
        };
        cDLAvCtrl.prototype.toggleAudiodescription = function () {
            var audio = null;
            var message = '';
            var encoding = '';
            var p = this._getAudioComponent();
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            if (p !== undefined) {
                audio = p;
            }
            if (audio === null || audio.list === null || audio.list.length === 0) {
                message = hx.l('LOC_NOAUDIODATA_ID');
            }
            else {
                var list = p.list;
                var i;
                var bAudioDescription = false;
                for (i = 0; i < list.length; i++) {
                    if (list[i].audioDescription) {
                        bAudioDescription = true;
                        break;
                    }
                }
                var cur = audio.selected;
                // make UI string
                if (bAudioDescription) {
                    var _audioModeHdmi, _audioModePcm, _audioType, _lipsyncDelay, _audioDescription;
                    var retObjAudio = dsrvSettingUtil.getAudioInfo({
                        cb: undefined
                    });
                    _audioModeHdmi = retObjAudio.audioModeHdmi;
                    _audioModePcm = retObjAudio.audioModePcm;
                    _audioType = retObjAudio.audioType;
                    _lipsyncDelay = retObjAudio.lipSyncDelay;
                    _audioDescription = retObjAudio.audioDescription;
                    if (_audioDescription === true) {
                        _audioDescription = false;
                        message = hx.l('LOC_AUDIO_DESCRIPTION_ID') + ' : ' + hx.l('LOC_OFF_ID');
                    }
                    else {
                        _audioDescription = true;
                        message = hx.l('LOC_AUDIO_DESCRIPTION_ID') + ' : ' + hx.l('LOC_SET_ENGLISH_ID');
                    }
                    dsrvSettingUtil.setAudioInfo({ audioDescription: _audioDescription, audioModePcm: _audioModePcm, audioModeHdmi: _audioModeHdmi, audioType: _audioType, lipSyncDelay: _lipsyncDelay });
                }
                else {
                    message = hx.l('LOC_NO_AUDIO_DESCRIPTION_ID');
                }
            }
            return {
                'message': message,
                'encoding': encoding
            };
        };
        cDLAvCtrl.prototype.toggleSubtitle = function () {
            var subtitle = null;
            var message = '';
            var subType = '';
            var hardofHearing = false;
            var p = this._getSubtitleComponent();
            if (p !== undefined) {
                subtitle = p;
            }
            if (subtitle === null || subtitle.list === null || subtitle.list.length === 0) {
                message = hx.l('LOC_NOSUBTITLE_ID');
            }
            else {
                var cur = subtitle.selected;
                var target = null;
                var len = subtitle.list.length;
                var targetindex = subtitle.index;
                // toggle
                if (targetindex === null) {
                    targetindex = 0;
                }
                else {
                    targetindex += 1;
                    if (targetindex >= len) {
                        targetindex = null;
                    }
                }
                if (targetindex !== null) {
                    target = subtitle.list[targetindex];
                }
                // set
                if (target === null) {
                    this._offSubtitleComponent();
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_OFF_ID');
                }
                else {
                    var sName;
                    this._setSubtitleComponent({
                        'off': cur,
                        'on': target
                    });
                    var subtidx = targetindex + 1;
                    sName = (target.language !== null) ? hx.il.iso639toString(target.language) : hx.l('LOC_SUBTITLE_ID') + ' ' + subtidx;
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + sName;
                    if (target.subType) {
                        subType = target.subType;
                    }
                    if (target.hearingImpaired) {
                        hardofHearing = true;
                    }
                }
            }
            return {
                'message': message,
                'subType': subType,
                'hardofHearing': hardofHearing
            };
        };
        cDLAvCtrl.prototype.toggleSubtitleonoff = function () {
            var subtitle = null;
            var message = '';
            var subType = '';
            var p = this._getSubtitleComponent();
            var dsrvSettingUtil = hx.svc.SETTING_UTIL;
            var hardofHearing = false;
            var hardofHearingEnabled = dsrvSettingUtil.getHardofHearing();
            if (p !== undefined) {
                subtitle = p;
            }
            if (subtitle === null || subtitle.list === null || subtitle.list.length === 0) {
                message = hx.l('LOC_NOSUBTITLE_ID');
            }
            else {
                var cur = subtitle.selected;
                var target = null;
                var len = subtitle.list.length;
                var targetindex = subtitle.index;
                var targetlang;
                var i;
                var sName;
                var subtidx;
                // toggle
                if (cur !== null) {
                    dsrvSettingUtil.setSubtitleEnabled(false);
                    this._offSubtitleComponent();
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_OFF_ID');
                }
                else {
                    targetlang = dsrvSettingUtil.getLanguage().LangSubTtl; //get lang
                    for (i = 0; i < len; i++) {
                        console.log("  subtitle.list[i].language = " + subtitle.list[i].language);
                        if (subtitle.list[i].language === targetlang) {
                            if (subtitle.list[i].hearingImpaired === hardofHearingEnabled) {
                                targetindex = i;
                                break;
                            }
                        }
                    }
                    if (targetindex === null && hardofHearingEnabled === false) {
                        for (i = 0; i < len; i++) {
                            if (subtitle.list[i].language === targetlang) {
                                targetindex = i;
                                break;
                            }
                        }
                    }
                    if (targetindex !== null) {
                        target = subtitle.list[targetindex];
                        console.log(" targetlang " + targetlang + " targetindex " + targetindex + " len " + len);
                        subtidx = targetindex + 1;
                        sName = (target.language !== null) ? hx.il.iso639toString(target.language) : hx.l('LOC_SUBTITLE_ID') + ' ' + subtidx;
                        message = hx.l('LOC_SUBTITLE_ID') + ' : ' + sName;
                    }
                    else {
                        target = subtitle.list[0];
                        subtidx = targetindex + 1;
                        sName = (target.language !== null) ? hx.il.iso639toString(target.language) : hx.l('LOC_SUBTITLE_ID') + ' ' + subtidx;
                        message = hx.l('LOC_SUBTITLE_ID') + ' : ' + sName;
                    }
                    dsrvSettingUtil.setSubtitleEnabled(true);
                    dsrvSettingUtil.setHardofHearing(hardofHearingEnabled);
                    this._setSubtitleComponent({
                        'off': null,
                        'on': target
                    });
                    if (target.subType) {
                        subType = target.subType;
                    }
                    if (target.hearingImpaired) {
                        hardofHearing = true;
                    }
                }
            }
            return {
                'message': message,
                'subType': subType,
                'hardofHearing': hardofHearing
            };
        };
        cDLAvCtrl.prototype.toggleMediaSubtitle = function () {
            var subtitle = [];
            var message = '';
            var subType = '';
            var supportLangs = this.avController.SubtitleSupportLanguages;
            if (supportLangs) {
                var idx;
                for (idx = 0; idx < supportLangs.length; idx += 1) {
                    subtitle.push(supportLangs[idx]);
                }
            }
            if (subtitle.length === 0) {
                message = hx.l('LOC_NOSUBTITLE_ID');
            }
            else {
                var cur = this.avController.SubtitleLanguage;
                var target = null, targetindex = null;
                var len = subtitle.length;
                if (this._bDisplaySubt) {
                    targetindex = subtitle.indexOf(cur);
                }
                else {
                    this.avController.SubtitleDisplay = this._bDisplaySubt = true;
                }
                // toggle
                if (targetindex === null) {
                    targetindex = 0;
                }
                else {
                    targetindex += 1;
                    if (targetindex >= len) {
                        targetindex = null;
                    }
                }
                if (targetindex !== null) {
                    target = subtitle[targetindex];
                }
                // set
                if (target === null) {
                    this.avController.SubtitleDisplay = this._bDisplaySubt = false;
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_OFF_ID');
                }
                else {
                    this.avController.SubtitleLanguage = target;
                    var subtIdx = targetindex + 1;
                    var sName = this._getSubtitleLanguageStr(target, subtIdx);
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + sName;
                }
            }
            return {
                'message': message,
                'subType': subType
            };
        };
        cDLAvCtrl.prototype.getMediaHasSubtitle = function () {
            return this.avController.hasSubtitle;
        };
        cDLAvCtrl.prototype.getMediaSubtitleDisplay = function () {
            var bDisplay = false;
            bDisplay = this.avController.SubtitleDisplay;
            if (bDisplay !== this._bDisplaySubt) {
                this._bDisplaySubt = bDisplay;
            }
            return this._bDisplaySubt;
            //return this.avController.SubtitleDisplay;
        };
        cDLAvCtrl.prototype.getMediaSubtitleLanguage = function () {
            return this.avController.SubtitleLanguage;
        };
        cDLAvCtrl.prototype.getMediaSubtitleSyncTime = function () {
            return this.avController.SubtitleSyncTime;
        };
        cDLAvCtrl.prototype.getMediaSubtitleFontSize = function () {
            return this.avController.SubtitleFontSize;
        };
        cDLAvCtrl.prototype.getMediaSubtitleTextPosition = function () {
            return this.avController.SubtitleTextPosition;
        };
        cDLAvCtrl.prototype.getMediaSubtitleSupportLanguages = function () {
            return this.avController.SubtitleSupportLanguages;
        };
        cDLAvCtrl.prototype.setMediaSubtitleDisplay = function (data) {
            this.avController.SubtitleDisplay = this._bDisplaySubt = data.display;
        };
        cDLAvCtrl.prototype.setMediaSubtitleLanguage = function (data) {
            this.avController.SubtitleLanguage = data.language;
        };
        cDLAvCtrl.prototype.setMediaSubtitleSyncTime = function (data) {
            this.avController.SubtitleSyncTime = data.syncTime;
        };
        cDLAvCtrl.prototype.setMediaSubtitleFontSize = function (data) {
            this.avController.SubtitleFontSize = data.size;
        };
        cDLAvCtrl.prototype.setMediaSubtitleTextPosition = function (data) {
            this.avController.SubtitleTextPosition = data.position;
        };
        cDLAvCtrl.prototype.getSupportedSpeedsArray = function () {
            var idx, len, SpeedsArray = [], oipfNumCollectionSpeeds = this.videoBroadcast.playSpeeds;
            hx.log("dsrvAvCtrl", "[getSupportedSpeedsArray] oipfNumCollectionSpeeds : [" + oipfNumCollectionSpeeds + ']');
            len = oipfNumCollectionSpeeds.length;
            hx.log("dsrvAvCtrl", "[getSupportedSpeedsArray] len : [" + len + ']');
            for (idx = 0; idx < len; idx += 1) {
                SpeedsArray.push(oipfNumCollectionSpeeds.item(idx));
                hx.log("dsrvAvCtrl", "[getSupportedSpeedsArray] idx : [" + idx + ']\'s value : ' + SpeedsArray[idx]);
            }
            return SpeedsArray;
        };
        cDLAvCtrl.prototype.setRequestPincodeResult = function (aPincodeResult) {
            this.videoBroadcast.resultPincode(aPincodeResult);
            hx.log("dsrvAvCtrl", "[setRequestPincodeResult]: " + aPincodeResult);
        };
        cDLAvCtrl.prototype.getProgrammePresent = function () {
            return this.videoBroadcast.programmes[0];
        };
        cDLAvCtrl.prototype.getChannelChanging = function () {
            return this._channelChanging;
        };
        cDLAvCtrl.prototype.setChannelChanging = function (param) {
            this._channelChanging = param;
        };
        cDLAvCtrl.prototype.requestThumbnail = function (param) {
            return this.videoBroadcast.requestThumbnail(param.path, param.position);
        };
        // ####################################################################################################
        // Private Methods
        cDLAvCtrl.prototype._initEventHandler = function () {
            var _this = this;
            this.defineEvents(['evtSetChannel', 'BlockAV', 'ChannelChangeSucceeded', 'ChannelChangeError', 'PlayStateChange', 'ParentalRatingChange', 'ProgrammesChanged', 'DRMRightsError', 'CasMessageNotify', 'CasUiNotify', 'PlaySpeedsArrayChanged', 'audioChange', 'RctInfoChanged', 'NCDChanged', 'RequestPincode', 'RequestMhegTune', 'RequestMhegInterrupt', 'MhegIcsStatus', 'NetworkUriChanged', 'HideNetflixPW']);
            // Finalize AV stop if there was TSR stop requested
            var fnCheckStopCurChannel = function () {
                if (_this._paramBlockAV) {
                    hx.log("dsrvAvCtrl", "blocking AV");
                    _this.videoBroadcast.stop();
                    if (_this._paramBlockAV.cb) {
                        _this._paramBlockAV.cb();
                        delete _this._paramBlockAV.cb;
                    }
                    _this._paramBlockAV = null;
                    return true;
                }
                return false;
            };
            var onChannelChangeSucceeded = function (channel) {
                hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]ChannelChangeSucceeded");
                var isReservation = _this._checkLastService(channel);
                if (!fnCheckStopCurChannel()) {
                    _this.fireEvent('ChannelChangeSucceeded', {
                        channel: channel,
                        isRsv: isReservation
                    });
                    hx.svc.EPG.retrieveLiveProgrammes({
                        channel: channel
                    });
                }
            };
            this.videoBroadcast.onChannelChangeSucceeded = onChannelChangeSucceeded;
            var onChannelChangeError = function (channel, errorState) {
                /*
                 * errorState : it is defined in oplvideobroadcasthandle_def.h's eOplPlaybackError
                 *          000 : OPLBROADCAST_CHANNEL_ERROR_CHANNEL_NOT_SUPPORTED
                 *          001 : OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE
                 *          002 : OPLBROADCAST_CHANNEL_ERROR_TUNER_LOCK
                 *          003 : OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK
                 *          004 : OPLBROADCAST_CHANNEL_ERROR_ENCRYPTED_CHANNEL
                 *          005 : OPLBROADCAST_CHANNEL_ERROR_UNKNOWN_CHANNEL
                 *          006 : OPLBROADCAST_CHANNEL_ERROR_CHANNEL_SWITCH_INTERRUPTED
                 *          007 : OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE_DUETO_RECORD
                 *          008 : OPLBROADCAST_CHANNEL_ERROR_CANNOT_RESOLVE_IP_CHANNEL
                 *          009 : OPLBROADCAST_CHANNEL_ERROR_BANDWIDTH
                 *          010 : OPLBROADCAST_CHANNEL_ERROR_CANNOT_SUPPORT_CHANNEL_LIST
                 *          011 : OPLBROADCAST_CHANNEL_ERROR_INSUFFICIENT_RESOURCE
                 *          012 : OPLBROADCAST_CHANNEL_ERROR_NOT_FOUND_IN_TRANSPORT
                 *          // HUMAX defined area START
                 *          013 : OPLBROADCAST_CHANNEL_ERROR_MOTOR_MOVING
                 *          014 : OPLBROADCAST_CHANNEL_ERROR_CAS_BLOCK
                 *          015 : OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED
                 *          016 : OPLBROADCAST_CHANNEL_ERROR_AV_UNDERRUN
                 *          017 : OPLBROADCAST_CHANNEL_ERROR_HDMI_BLOCK
                 *          018 : OPLBROADCAST_CHANNEL_ERROR_7DAY_TIMER_BLOCK
                 *          019 : OPLBROADCAST_CHANNEL_ERROR_NO_VIDEO
                 *          020 : OPLBROADCAST_CHANNEL_ERROR_NO_AV
                 *          021 : OPLBROADCAST_CHANNEL_ERROR_NOT_RUNNING
                 *          022 : OPLBROADCAST_CHANNEL_ERROR_NO_EIT
                 *          023 : OPLBROADCAST_CHANNEL_ERROR_5_TIMES_PIN_FAIL
                 *          024 : OPLBROADCAST_CHANNEL_ERROR_60DAY_FTA_LIMIT
                 *          025 : OPLBROADCAST_CHANNEL_ERROR_EMPTY_CHANNEL
                 *          026 : OPLBROADCAST_CHANNEL_ERROR_NO_ERROR
                 *          // HUMAX defined area END
                 *          100 : OPLBROADCAST_CHANNEL_ERROR_UNDEFINED
                 *          // JLABS defined area...
                 *          400 : OPLBROADCAST_CHANNEL_ERROR_TSMF_LOW
                 *          500 : OPLBROADCAST_CHANNEL_ERROR_NO_SERVICE_IN_PAT
                 *          501 : OPLBROADCAST_CHANNEL_ERROR_PMT_TIMEOUT
                 *          502 : OPLBROADCAST_CHANNEL_ERROR_NO_PAT_IN_TEMP_SVC
                 *          503 : OPLBROADCAST_CHANNEL_ERROR_PAT_TIMEOUT
                 */
                hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]ChannelChangeError (errorState : " + errorState + ")");
                var isReservation = _this._checkLastService(channel);
                if (errorState === 14) {
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE": "ERCAS",
                            "TRS": "R",
                            "CAS_ERROR": errorState
                        });
                    }
                }
                if (!fnCheckStopCurChannel()) {
                    _this.fireEvent('ChannelChangeError', {
                        channel: channel,
                        errorState: errorState,
                        isRsv: isReservation
                    });
                }
            };
            this.videoBroadcast.onChannelChangeError = onChannelChangeError;
            var onRecordingEvent = function (state, error, recordingId) {
                mTSROnGoing = ((state === REC_STATE_TS_ACQUIRING_RESOURCES) || (state === REC_STATE_TS_STARTED));
                switch (state) {
                    case REC_STATE_UNREALIZED:
                        mLiveStopping = false;
                        break;
                }
            };
            this.videoBroadcast.addEventListener('RecordingEvent', function (e) {
                onRecordingEvent(e.state, e.error, e.recordingID);
            });
            var PlayStateChange = function (state, error) {
                /*
                 * state : it is defined in NativeVideoBroadcast.h's BroadcastPlaystate
                 *          0 : BROADCAST_PLAYSTATE_UNREALIZED
                 *          1 : BROADCAST_PLAYSTATE_CONNECTING
                 *          2 : BROADCAST_PLAYSTATE_PRESENTING
                 *          3 : BROADCAST_PLAYSTATE_STOPPED
                 * error : it is same value in ChannelChangeError's errorState
                 */
                hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]PlayStateChange (state : " + state + " error : " + error + ") ");
                _this.fireEvent('PlayStateChange', {
                    'state': state,
                    'error': error,
                    'from': 'VideoBroadcast'
                });
            };
            this.videoBroadcast.onPlayStateChange = PlayStateChange;
            var ParentalRatingChange = function (contentID, rating, DRMSystemID, blocked) {
                hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]ParentalRatingChange (contentID : " + contentID + " , rating : " + rating + " , DRMSystemID : " + DRMSystemID + " , blocked : " + blocked);
                _this.fireEvent('ParentalRatingChange', {
                    contentID: contentID,
                    rating: rating,
                    DRMSystemID: DRMSystemID,
                    blocked: blocked
                });
            };
            this.videoBroadcast.onParentalRatingChange = ParentalRatingChange;
            var ProgrammesChanged = function () {
                hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]ProgrammesChanged");
                hx.log("debug", "##### [dsrvAvCtrl]ProgrammesChanged");
                var programmeItem = new __PROVIDER__.CProgrammeProvider({
                    oipfData: _this.videoBroadcast.programmes[0]
                });
                //hx.log("warn", programmeItem.toString());
                _this.fireEvent('ProgrammesChanged', programmeItem);
            };
            this.videoBroadcast.onProgrammesChanged = ProgrammesChanged;
            // 7.13.6 extensions for DRM right error
            // 7.14.6 Extensions to A/V Control object for DRM rights errors
            var DRMRightsError = function (errorState, contentID, DRMSystemID, rightsIssuerURL) {
                hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]DRMRightsError : errorState : " + errorState + "contentID : " + contentID);
                _this.fireEvent('DRMRightsError', {
                    errorState: errorState,
                    contentID: contentID,
                    DRMSystemID: DRMSystemID,
                    rightsIssuerURL: rightsIssuerURL
                });
            };
            this.videoBroadcast.onDRMRightsError = DRMRightsError;
            var CasMessageNotify = function (module, slot, seesionHandle, message) {
                hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]CasMessageNotify : module : " + module + ", slot : " + slot + ", sessionHandle :  " + seesionHandle + ", message " + message + ' ##END##');
                _this.fireEvent('CasMessageNotify', {
                    param: {
                        'module': module,
                        'slot': slot,
                        'seesionHandle': seesionHandle,
                        'uiData': JSON.parse(message)
                    }
                });
            };
            this.videoBroadcast.onCasMessageNotify = CasMessageNotify;
            var CasUiNotify = function (module, slot, sessionHandle) {
                hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]CasUiNotify : module : " + module + ", slot : " + slot + "sessionHandle : " + sessionHandle + ' ##END##');
                _this.fireEvent('CasUiNotify', {
                    param: {
                        'module': module,
                        'slot': slot,
                        'sessionHandle': sessionHandle
                    }
                });
            };
            this.videoBroadcast.onCasUiNotify = CasUiNotify;
            var PlaySpeedsArrayChangedEvent = function () {
                hx.log("dsrvAvCtrl", "[dsrvAvCtrl]PlaySpeedsArrayChanged");
                _this.fireEvent('PlaySpeedsArrayChanged', {
                    from: 'VideoBroadcast'
                });
            };
            this.videoBroadcast.onPlaySpeedsArrayChanged = PlaySpeedsArrayChangedEvent;
            var RctInfoChanged = function (aLinkCount, aRctInfo) {
                hx.log('dsrvAvCtrl', '**********************************************');
                hx.log('dsrvAvCtrl', 'RctInfoChanged aLinkCount=' + aLinkCount);
                hx.log('dsrvAvCtrl', 'RctInfoChanged aRctInfo=' + aRctInfo);
                if (aRctInfo) {
                    hx.log('dsrvAvCtrl', '******************  RCT INFO  ****************');
                    hx.log('dsrvAvCtrl', 'totalLinkCnt=[' + aRctInfo.totalLinkCnt + '] serviceId=[' + aRctInfo.serviceId + '] yearOffset=[' + aRctInfo.yearOffset + '] aRctInfo.lengt=[' + aRctInfo.length + ']');
                    hx.log('dsrvAvCtrl', 'imageIconInfo=[' + aRctInfo.imageIconInfo);
                    if (aRctInfo.imageIconInfo) {
                        hx.log('debug', '=> iconId=[' + aRctInfo.imageIconInfo.iconId + '] transPortMode=[' + aRctInfo.imageIconInfo.transPortMode + '] positionFlag=[' + aRctInfo.imageIconInfo.positionFlag + ']');
                        hx.log('debug', '=> coordinateSystem=[' + aRctInfo.imageIconInfo.coordinateSystem + '] horizontalOrigin=[' + aRctInfo.imageIconInfo.horizontalOrigin + '] verticalOrigin=[' + aRctInfo.imageIconInfo.verticalOrigin + ']');
                        hx.log('debug', '=> iconType=[' + aRctInfo.imageIconInfo.iconType + '] dataByteSize=[' + aRctInfo.imageIconInfo.dataByteSize + ']');
                        //                            hx.log('debug', '=> dataByte=['+aRctInfo.imageIconInfo.dataByte+']');
                        hx.log('debug', '=> iconUrl=[' + aRctInfo.imageIconInfo.iconUrl + ']');
                    }
                    var i, len = aRctInfo.length;
                    for (i = 0; i < len; i++) {
                        hx.log('dsrvAvCtrl', '****************  RCT INFO [' + i + ']  *************');
                        hx.log('dsrvAvCtrl', '=> linkType=[' + aRctInfo[i].linkType + '], howRelatedSchemId=[' + aRctInfo[i].howRelatedSchemId + ']');
                        hx.log('dsrvAvCtrl', '=> termId=[' + aRctInfo[i].termId + '], groupId=[' + aRctInfo[i].groupId + '], precedence=[' + aRctInfo[i].precedence + ']');
                        hx.log('dsrvAvCtrl', '=> mediaUri=' + aRctInfo[i].mediaUri);
                        hx.log('dsrvAvCtrl', '=> proTextNum=[' + aRctInfo[i].proTextNum + '], promotionalTextLangCode=[' + aRctInfo[i].promotionalTextLangCode + ']');
                        hx.log('dsrvAvCtrl', '=> promotionalText=[' + aRctInfo[i].promotionalText + ']');
                        hx.log('debug', '=> isDefaultIcon=[' + aRctInfo[i].isDefaultIcon + '], iconId=[' + aRctInfo[i].iconId + ']');
                        hx.log('dsrvAvCtrl', '=> dvbBinLocator=' + aRctInfo[i].dvbBinLocator);
                        if (aRctInfo[i].dvbBinLocator) {
                            hx.log('dsrvAvCtrl', '=> => identifierType=[' + aRctInfo[i].dvbBinLocator.identifierType + '], scheTimeReliability=[' + aRctInfo[i].dvbBinLocator.scheTimeReliability + ']');
                            hx.log('dsrvAvCtrl', '=> => startDate=[' + aRctInfo[i].dvbBinLocator.startDate + '], startTime=[' + aRctInfo[i].dvbBinLocator.startTime + '], duration=[' + aRctInfo[i].dvbBinLocator.duration + ']');
                            hx.log('dsrvAvCtrl', '=> => inlineSvc=[' + aRctInfo[i].dvbBinLocator.inlineSvc + '], dvbSvcTripletId=[' + aRctInfo[i].dvbBinLocator.dvbSvcTripletId + ']');
                            hx.log('dsrvAvCtrl', '=> => tsId=[' + aRctInfo[i].dvbBinLocator.tsId + '], onId=[' + aRctInfo[i].dvbBinLocator.onId + '], svcId=[' + aRctInfo[i].dvbBinLocator.svcId + ']');
                            hx.log('dsrvAvCtrl', '=> => evtId=[' + aRctInfo[i].dvbBinLocator.evtId + '], tvaId=[' + aRctInfo[i].dvbBinLocator.tvaId + '], component=[' + aRctInfo[i].dvbBinLocator.component + ']');
                            hx.log('dsrvAvCtrl', '=> => earlyStartWin=[' + aRctInfo[i].dvbBinLocator.earlyStartWin + '], lateEndWin=[' + aRctInfo[i].dvbBinLocator.lateEndWin + ']');
                        }
                    }
                }
                hx.log('dsrvAvCtrl', '**********************************************');
                _this.fireEvent('RctInfoChanged', aLinkCount, aRctInfo);
            };
            this.videoBroadcast.onRctInfoChanged = RctInfoChanged;
            var NCDChanged = function (aStartTime, aDuration, aMessage) {
                hx.log("info", "NCDChanged");
                //TODO: need to wrappin
                hx.log("info", aStartTime);
                hx.log("info", aDuration);
                hx.log("info", aMessage);
                _this.fireEvent('NCDChanged', aStartTime, aDuration, aMessage);
            };
            this.videoBroadcast.onNCDChanged = NCDChanged;
            this.operatorFeature.onNCDChanged = NCDChanged;
            var RequestPincode = function (aMessage) {
                hx.log("info", "RequestPincode");
                hx.log("info", aMessage);
                _this.fireEvent('RequestPincode', aMessage);
            };
            this.videoBroadcast.onRequestPincode = RequestPincode;
            this.videoBroadcast.onRequestMhegTune = function (aChannel, aServiceID, aTuneMode) {
                hx.log("debug", "name: " + aChannel.name + "ccid: " + aChannel.ccid + " aServiceID: " + aServiceID + " aTuneMode: " + aTuneMode + "[" + __DEF__.eMhegTuneInfo[aTuneMode] + "]");
                _this.fireEvent('RequestMhegTune', aChannel, aServiceID, aTuneMode);
            };
            this.videoBroadcast.onRequestMhegInterrupt = function (aIsInterruputable) {
                hx.log("debug", "[event] RequestMhegInterrupt: " + aIsInterruputable);
                _this.fireEvent("RequestMhegInterrupt", aIsInterruputable);
            };
            this.videoBroadcast.onMhegIcsStatus = function (param) {
                hx.log("debug", "[event] onMhegIcsStatus: " + param);
                _this._mhegIcsStatus = param;
                _this.fireEvent("MhegIcsStatus", param);
            };
            this.videoBroadcast.onNetworkUriChanged = function (uri) {
                hx.log("debug", "[event] onNetworkUriChanged: " + uri);
                _this.fireEvent("NetworkUriChanged", uri);
            };
        };
        cDLAvCtrl.prototype._setAVComponent = function (off, on) {
            if (off !== null) {
                this._unSetComponent(off);
            }
            if (on !== null) {
                hx.log('dsrvAvCtrl', '[dsrvAvCtrl] changeAVComponent()');
                this._setComponent(on);
            }
        };
        cDLAvCtrl.prototype._unSetComponent = function (param) {
            var cmpt = param;
            if (!cmpt) {
                return;
            }
            if (param._wrapper !== undefined) {
                cmpt = param._wrapper;
            }
            var avStatus = this._isPbPlaying();
            if (avStatus) {
                this.avController.unselectComponent(cmpt);
            }
            else {
                this.videoBroadcast.unselectComponent(cmpt);
            }
        };
        cDLAvCtrl.prototype._setComponent = function (param) {
            var cmpt = param;
            if (!cmpt) {
                return;
            }
            if (param._wrapper !== undefined) {
                cmpt = param._wrapper;
            }
            if (this._isPbPlaying() === true) {
                if (this.avController.selectComponent) {
                    this.avController.selectComponent(cmpt);
                }
            }
            else {
                if (this.videoBroadcast.selectComponent) {
                    this.videoBroadcast.selectComponent(cmpt);
                }
            }
        };
        cDLAvCtrl.prototype.__getComponent = function (param) {
            var cmpt = null;
            var cmptActive = null;
            var list = null;
            var selected = null;
            var index = null;
            var len;
            var i;
            var avStatus = this._isPbPlaying();
            // get list
            if (avStatus) {
                cmpt = this.avController.getComponents(param);
            }
            else {
                cmpt = this.videoBroadcast.getComponents(param);
            }
            len = cmpt.length;
            if (len > 0) {
                list = [];
                for (i = 0; i < len; i += 1) {
                    var item = cmpt.item(i);
                    list.push(this._copy(item));
                }
            }
            if (param === 2 && list !== null) {
                // [D-Book9] All receivers shall be capable of decoding and presenting DVB subtitles in accordance with EN 300 743.
                // For this, priority of DVB subtitle should be higher than EBU.
                // (DVB: 201, EBU: 202)
                var EOrderType = {
                    201: 1,
                    202: 2,
                    undefined: 3
                };
                list.sort(function (a, b) {
                    return (EOrderType[a.subType] || EOrderType.undefined) - (EOrderType[b.subType] || EOrderType.undefined);
                });
            }
            // get selected
            if (avStatus) {
                cmptActive = this.avController.getCurrentActiveComponents(param);
            }
            else {
                cmptActive = this.videoBroadcast.getCurrentActiveComponents(param);
            }
            len = cmptActive.length;
            if (list !== null && len > 0) {
                if (cmptActive !== null && len > 0) {
                    var sel = cmptActive.item(0);
                    selected = this._copy(sel);
                }
            }
            // find index
            if (list !== null && selected !== null) {
                for (i = 0; i < list.length; i += 1) {
                    if (selected.pid === list[i].pid && selected.language === list[i].language && selected.componentTag === list[i].componentTag) {
                        index = i;
                        break;
                    }
                }
            }
            var result = {
                list: list,
                selected: selected,
                index: index
            };
            return result;
        };
        cDLAvCtrl.prototype._getAudioComponent = function () {
            return this.__getComponent(1);
        };
        cDLAvCtrl.prototype._setAudioComponent = function (param) {
            var off = param.off;
            var on = param.on;
            this._setAVComponent(off, on);
            this.fireEvent('audioChange', {
                'item': param.on
            });
        };
        cDLAvCtrl.prototype._getSubtitleComponent = function () {
            var ret;
            ret = this.__getComponent(2);
            return ret;
        };
        cDLAvCtrl.prototype._setSubtitleComponent = function (param) {
            var off = param.off;
            var on = param.on;
            this._setAVComponent(off, on);
        };
        cDLAvCtrl.prototype._offSubtitleComponent = function () {
            var avStatus = this._isPbPlaying();
            if (avStatus) {
                this.avController.offComponent(2, -1);
            }
            else {
                this.videoBroadcast.offComponent(2, -1);
            }
        };
        cDLAvCtrl.prototype._isPbPlaying = function () {
            var avStatus = this.avController.playState;
            //0 Live 1 Play 3 Connecting
            if (avStatus >= 1 && avStatus <= 3) {
                return true;
            }
            return false;
        };
        cDLAvCtrl.prototype._copy = function (avcomponent) {
            var p = this._prop;
            var c = {};
            var n;
            for (n = 0; n < p.length; n += 1) {
                c[p[n]] = avcomponent[p[n]];
            }
            c._wrapper = avcomponent;
            return c;
        };
        cDLAvCtrl.prototype._setChannel = function (param, aTuneInfo) {
            if (aTuneInfo === void 0) { aTuneInfo = 0; }
            hx.log('dsrvAvCtrl', '##### [dsrvAvCtrl] 1 setChannel << CCID >> : ' + param.channel.ccid + ' <<num>> : ' + param.channel.majorChannel + ' <<name>> :' + param.channel.name);
            hx.log('dsrvAvCtrl', 'param.channel.manualBlock: ' + param.channel.manualBlock);
            //this.videoBroadcast.requestAudioPath();
            this._channelChanging = true;
            if (aTuneInfo > 0) {
                this.videoBroadcast.setChannelWithTuneInfo(param.channel, aTuneInfo);
            }
            else {
                this.videoBroadcast.setChannel(param.channel, param.trickplay, param.contentAccessDescriptorURL);
            }
            this.fnSetFastChannelChange();
        };
        cDLAvCtrl.prototype.fnSetFastChannelChange = function () {
            var _this = this;
            // fast channel change set
            if (!!hx.config.CONFIG_FCC_NUM) {
                var fun = function () {
                    var lastSvc, nextSvc, prevSvc;
                    if (!!_this.videoBroadcast.setChannelFCC) {
                        lastSvc = hx.svc.WEB_STORAGE.getLastService();
                        if (!lastSvc || !lastSvc.ccid || !lastSvc.group) {
                            return;
                        }
                        // previous - video id 1
                        if (hx.config.CONFIG_FCC_NUM > 0) {
                            nextSvc = hx.svc.CH_LIST.getNextChannel({
                                'ccid': lastSvc.ccid,
                                'group': lastSvc.group
                            });
                            if (!!nextSvc && !!nextSvc.result && !!nextSvc.changeable && !!nextSvc.channel) {
                                hx.log('dsrvAvCtrl', '##### [dsrvAvCtrl] NEXT setChannelFCC << CCID >> : ' + nextSvc.channel.ccid + ' <<num>> : ' + nextSvc.channel.majorChannel + ' <<name>> :' + nextSvc.channel.name);
                                _this.videoBroadcast.setChannelFCC(nextSvc.channel, FCC_VIDEO_ID_1);
                            }
                        }
                        // next - video id 2
                        if (hx.config.CONFIG_FCC_NUM > 1) {
                            prevSvc = hx.svc.CH_LIST.getPrevChannel({
                                'ccid': lastSvc.ccid,
                                'group': lastSvc.group
                            });
                            if (!!prevSvc && !!prevSvc.result && !!prevSvc.changeable && !!prevSvc.channel) {
                                hx.log('dsrvAvCtrl', '##### [dsrvAvCtrl] PREV setChannelFCC << CCID >> : ' + prevSvc.channel.ccid + ' <<num>> : ' + prevSvc.channel.majorChannel + ' <<name>> :' + prevSvc.channel.name);
                                _this.videoBroadcast.setChannelFCC(prevSvc.channel, FCC_VIDEO_ID_2);
                            }
                        }
                    }
                };
                clearTimeout(mFCCTimer);
                mFCCTimer = setTimeout(fun, 1000);
            }
        };
        cDLAvCtrl.prototype._checkLastService = function (channel) {
            /* This function checks channel changing from MW.
             */
            var ccid;
            var group;
            var diff;
            var lastSvc;
            var retObj;
            var o2;
            if (!channel) {
                return false;
            }
            lastSvc = hx.svc.WEB_STORAGE.getLastService();
            diff = (new Date().getTime()) - lastSvc.timeStamp;
            hx.log('dsrvAvCtrl', '#####[_checkLastService] diff : ' + diff + 'mSec / cur ccid: ' + channel.ccid + ' / last ccid:' + lastSvc.ccid);
            if (channel.ccid === lastSvc.ccid || diff < 5000) {
                return false;
            }
            ccid = channel.ccid;
            group = lastSvc.group;
            // save service if new changed service
            retObj = hx.svc.CH_LIST.getChannel({
                'group': lastSvc.group,
                'ccid': channel.ccid
            });
            if (retObj.result === false) {
                o2 = hx.svc.CH_LIST.getAvailableGroupForChannel({
                    'channel': channel
                });
                if (o2.channel !== null && o2.group !== null) {
                    ccid = o2.channel.ccid;
                    group = o2.group;
                }
            }
            hx.log('dsrvAvCtrl', '#####[_checkLastService] TUNE From MW : ccid: ' + ccid + ' group:' + group);
            hx.svc.WEB_STORAGE.updateLastService({
                'ccid': ccid,
                'group': group
            });
            return true;
        };
        cDLAvCtrl.prototype._getSubtitleLanguageStr = function (langCode, idx) {
            var retStr = "";
            switch (langCode) {
                case 'en-EN':
                case 'en-US':
                    retStr = hx.l('LOC_LANG_ENGLISH_ID');
                    break;
                case 'de-DE':
                    retStr = hx.l('LOC_LANG_GERMAN_ID');
                    break;
                case 'ko-KR':
                    retStr = hx.l('LOC_LANG_KOREAN_ID');
                    break;
                default:
                    retStr = hx.l('LOC_SUBTITLE_ID') + ' ' + idx;
                    break;
            }
            return retStr;
        };
        return cDLAvCtrl;
    })(__KERNEL__);
    return cDLAvCtrl;
});
