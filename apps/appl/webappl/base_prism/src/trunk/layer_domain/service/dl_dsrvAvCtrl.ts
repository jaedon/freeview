/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvAvCtrl.js
/*jslint nomen:true, vars:true */
/*global hx*/
/*global $*/
//    AV_CTRL: ['layer_domain/service/dl_dsrvAvCtrl.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("model/model");
import __PROVIDER__ = require("pages/util/provider");
import __DEF__ = require("def/oipf.def");

interface _ifAudioDescript {
    list: any[];
    selected: any[];
    index: number;
}
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

class cDLAvCtrl extends __KERNEL__ implements ifSVCAvCtrl {
    private _prop = ['componentTag', 'pid', 'type', 'encoding', 'encrypted', 'has_aspectRatio', 'aspectRatio', 'language', 'audioDescription', 'audioChannels', 'hearingImpaired', 'subType', 'language2'];
    private mLastTimeStamp = 0;
    private videoBroadcast = this.getPluginInstance("VideoBroadcast");
    private avController = this.getPluginInstance("AVController");
    private operatorFeature = this.getPluginInstance("OperatorFeature");
    private _updateHysteresisTimer: number = null;
    private _updateHysteresisBuffer: any[] = [];
    private _bDisplaySubt: boolean = true;
    private _blockAV = false;
    private _paramBlockAV = null;
    private _blockAvTaskTimer = null;
    private _channelChanging: boolean = false;
    private _canNaviChannel : boolean = true;
    private _mhegIcsStatus : number = 0;//0 ICS OFF, 1 ICS ON

    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvAvCtrl');
        hx.log('dsrvAvCtrl', 'create');
        $('#osd').append(this.videoBroadcast);
        this._initEventHandler();
    }

    setID(data): void {
        if (hx.emulMode) {
            $("#" + data.parentID).append($("<div>").attr({
                id: data.videoID
            }));
            return;
        }
        throw "this.vb ?"
//        $("#" + data.parentID).append($(this.vb).attr({
//            id : data.videoID
//        }));
    }

    channelChange(param, aTuneInfo: number = 0): void {
        this.fireEvent('evtSetChannel', {
            'channel': param.channel,
            'tuneInfo': aTuneInfo
        });

        hx.log("debug", "channelChange evtSetChannel fire success!!");
        //update channel group
        if(param.channel && !param.group){
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
        setTimeout(() => {
            if (aTuneInfo > 0) {
                hx.log("debug", "tuneinfo: " + parseInt(aTuneInfo.toString(), 16));
                this._setChannel({
                    'channel': param.channel
                }, aTuneInfo);
            } else {
                this._setChannel({
                    'channel': param.channel,
                    'trickplay': false,
                    'contentAccessDescriptorURL': ""
                });
            }
        }, 30);
    }

    stopCurChannel(param): void {
        if (param.stopTsr) {
            this.videoBroadcast.stopTimeshift();
        }
        if (param.stopAV) {
            this.videoBroadcast.stop();
        }
    }

    isLiveOnGoing(): boolean {
        var playState = this.videoBroadcast.playState;
        if (playState === BROADCAST_PLAYSTATE_CONNECTING || playState === BROADCAST_PLAYSTATE_PRESENTING) {
            return true;
        }
        return false;
    }

    isMpbOnGoing(): boolean {
        var vodPlatState = this.avController.playState;
        if (vodPlatState === PLAY_STATE_PLAYING || vodPlatState === PLAY_STATE_PAUSED || vodPlatState === PLAY_STATE_CONNECTING || vodPlatState === PLAY_STATE_BUFFERING) {
            return true;
        }
        return false;
    }

    isLiveStopping(): boolean {
        return mLiveStopping;
    }

    blockAV(param): void {
        this._blockAV = true;
        this._paramBlockAV = param;
        hx.log("dsrvAvCtrl", "blockAV");
        var fnDoVideoOnDemandTask = () => {
            this.require('PLAYBACK', (dsrvPlayback) => {
                dsrvPlayback.clearEventCb(this, 'PlayStateStopped');
            });
            if (this._blockAvTaskTimer) {
                clearTimeout(this._blockAvTaskTimer);
                this._blockAvTaskTimer = null;
            }
            if (this._paramBlockAV && this._paramBlockAV.cb) {
                this._paramBlockAV.cb();
                delete this._paramBlockAV.cb;
                this._paramBlockAV = null;
            }
        };
        var fnDoVideoBroadcastTask = () => {
            this.require('TSR', (dsrvTsr2) => {
                dsrvTsr2.clearEventCb(this, 'LiveStarted');
            });
            if (this._blockAvTaskTimer) {
                clearTimeout(this._blockAvTaskTimer);
                this._blockAvTaskTimer = null;
            }
            if (this._paramBlockAV && this._paramBlockAV.cb) {
                this._paramBlockAV.cb();
                delete this._paramBlockAV.cb;
                this._paramBlockAV = null;
            }
        };
        var fnRegistVideoOnDemandTask = () => {
            this.require('PLAYBACK', (dsrvPlayback) => {
                dsrvPlayback.addEventCb('PlayStateStopped', this, (e) => {
                    fnDoVideoOnDemandTask();
                });
            });
            this._blockAvTaskTimer = setTimeout(fnDoVideoOnDemandTask, 2000);
        };
        var fnRegistVideoBroadcastTask = () => {
            this.require('TSR', (dsrvTsr2) => {
                dsrvTsr2.addEventCb('LiveStarted', this, (e) => {
                    fnDoVideoBroadcastTask();
                });
            });
            this._blockAvTaskTimer = setTimeout(fnDoVideoBroadcastTask, 2000);
        };
        if (this._blockAvTaskTimer) {
            clearTimeout(this._blockAvTaskTimer);
            this._blockAvTaskTimer = null;
        }
        if (mTSROnGoing) {
            mLiveStopping = true;
            setTimeout(function() {
                mLiveStopping = false;
            }, 5000);
            this.videoBroadcast.stop();
            fnRegistVideoBroadcastTask();
        } else {
            if (this.videoBroadcast.playState === BROADCAST_PLAYSTATE_STOPPED) {
                this.avController.stop();
                fnRegistVideoOnDemandTask();
            } else {
                this.videoBroadcast.stop();
                fnRegistVideoBroadcastTask();
            }
        }
        this.fireEvent('BlockAV');
    }

    unBlockAV(): void {
        hx.log("dsrvAvCtrl", "unBlockAV");
        this._blockAV = false;
        this._paramBlockAV = null;
        if (this._blockAvTaskTimer) {
            clearTimeout(this._blockAvTaskTimer);
            this._blockAvTaskTimer = null;
        }
    }

    availableChannel(group?): boolean {
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
        } else {
            result = false;
        }
        return result;
    }

    lastChannel(): boolean {
        var ccid;
        var group;
        var retObj;
        var retObjChCnt: any = 0;
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
        hx.log('error','lastChannel create cahngeLastService uri : '+uri);
        (<any>hx.svc.AV_CTRL)._eventSource._fireEvent("NetworkUriChanged", uri);

        return result;
    }

    bindToChannel(){
        return this.videoBroadcast.bindToCurrentChannel();
    }

    isMhegIcsOnGoing() : boolean{
        return (this._mhegIcsStatus == 1);
    }

    naviChannel(param): boolean {
        var timeOut = 500;
        //300;
        var fun;
        var newSvc;
        var getFunction;
        var retLastSvc;
        var bRet = true;

        if(param && param.direction ==='block'){
            this._canNaviChannel = false;
        }else if(param && param.direction ==='unBlock'){
            this._canNaviChannel = true;
        }

        if(!this._canNaviChannel){
            hx.log('error','naviChannel [_canNaviChannel] FAIL');
            return true;
        }

        if (param.direction === 'next') {
            getFunction = 'getNextChannel';
            retLastSvc = hx.svc.WEB_STORAGE.getLastService();
        } else if (param.direction === 'prev') {
            getFunction = 'getPrevChannel';
            retLastSvc = hx.svc.WEB_STORAGE.getLastService();
        } else if (param.direction === 'back') {
            getFunction = 'getBackChannel';
            retLastSvc = hx.svc.WEB_STORAGE.getBackService();
        } else {
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
                fun = () => {
                    var len = this._updateHysteresisBuffer.length;
                    if (len > 0) {
                        var Popchannel = this._updateHysteresisBuffer.pop();
                        this._setChannel({
                            'channel': Popchannel,
                            'trickplay': false,
                            'contentAccessDescriptorURL': ""
                        });
                        this._updateHysteresisBuffer = [];
                    }
                    var curChannel = hx.svc.CH_LIST.getCurrentChannel().channel;
                    if (curChannel) {
                        mOnId = curChannel.onid;
                        mTsId = curChannel.tsid;

                        mSId = curChannel.sid;
                        mChName = curChannel.name;

                        if (hx.svc.STATISTICS) {
                            setTimeout(() => {
                                var result = hx.svc.EPG.getProgrammes({
                                    isOnAir : true,
                                    filter: "P/F",
                                    channel : curChannel,
                                    listener : this,
                                    notUI: true,
                                    cb : function(result) {
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
                                                "CODE" : "ELWS", //EVENT_LIVE_WATCH_START
                                                "TRS" : "R",
                                                "ONID" : mOnId,
                                                "TSID" : mTsId,
                                                "SID" : mSId,
                                                "EID" : eventID,
                                                "CHNAME" : mChName,
                                                "EVTNAME" : evtName
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
            } else {
                bRet = false;
            }
        } else {
            if (param.direction === 'next' || param.direction === 'prev') {
                // tune to available channel
                hx.log("error", "##### ERROR [dsrvAvCtrl] not found ccid or group : try to change available channel");
                this.availableChannel()
            }
        }
        return bRet
    }

    getVideoBroadcastCurrentChannel(): any {
        return this.videoBroadcast.currentChannel;
    }

    isChannelPlaying(): boolean {
        return !!this.videoBroadcast.currentChannel;
    }

    isIdle(): boolean {
        if ((this.videoBroadcast.currentChannel) || this._isPbPlaying()) {
            return false;
        }
        return true;
    }

    getAVComponent(): any {
        var result;
        result = {
            audio: this._getAudioComponent(),
            subtitle: this._getSubtitleComponent()
        };
        return result;
    }

    getAudioComponent(): any {
        var result;
        result = {
            audio: this._getAudioComponent()
        };
        return result;
    }

    getSubtitleComponent(): any {
        var result;
        result = {
            subtitle: this._getSubtitleComponent()
        };
        return result;
    }

    setAudioComponent(param): void {
        this._setAudioComponent(param);
    }

    setSubtitleComponent(param): void {
        this._setSubtitleComponent(param);
    }

    offSubtitleComponent(param?): void {
        this._offSubtitleComponent();
    }

    getCurAudio(): any {
        var audio = null;
        var message = '';
        var encoding = '';
        var p = this._getAudioComponent();
        if (p !== undefined) {
            audio = p;
        }
        if (audio === null || audio.list === null || audio.list.length === 0) {
            message = hx.l('LOC_NOAUDIODATA_ID');
        } else {
            var cur = audio.selected;
            var target = null;
            var len = audio.list.length;
            var targetindex = audio.index;
            var aName, szLang2;
            // check targetindex
            if (targetindex === null) {
                targetindex = 0;
            } else {
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
                if (target.subType === _oipfDef.TAudioSubType.SUBTYPE_AUDIO_DUALMONO) {// Support Dual Mono
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
    }

    getCurAudiodescription(): any {
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
        } else {
            var cur = audio.selected;

            // make UI string
            if ( cur.audioDescription ) {
                var _audioDescription;
                var retObjAudio = dsrvSettingUtil.getAudioInfo({
                    cb : undefined
                });
                _audioDescription = retObjAudio.audioDescription;
                if( _audioDescription === true ){
                    message = hx.l('LOC_AUDIO_DESCRIPTION_ID') + ' : ' + hx.l('LOC_SET_ENGLISH_ID');
                } else {
                    message = hx.l('LOC_AUDIO_DESCRIPTION_ID') + ' : ' + hx.l('LOC_OFF_ID');
                }
            } else {
                message = hx.l('LOC_NO_AUDIO_DESCRIPTION_ID');
            }
        }
        return {
            'message': message,
            'encoding': encoding
        };
    }

    getCurSubtitle(): any {
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
        } else {
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
            } else {
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
    }

    getCurSubtitleonoff(): any {
        var subtitle = null;
        var message = '';
        var subType = 0;
        var dsrvSettingUtil = hx.svc.SETTING_UTIL;
        var _isSubtitleDisplay = dsrvSettingUtil.getSubtitleEnabled();
        var hardofHearing = false;
        var hardofHearingEnabled : boolean = dsrvSettingUtil.getHardofHearing();
        var p = this._getSubtitleComponent();
        if (p !== undefined) {
            subtitle = p;
        }
        if (subtitle === null || subtitle.list === null || subtitle.list.length === 0) {
            message = hx.l('LOC_NOSUBTITLE_ID');
        } else  {
            var cur = subtitle.selected;
            var target = null;
            var len : number = subtitle.list.length;
            var targetindex : number = subtitle.index;
            var targetlang : string;
            var i : number;
            var sName : string;
            var subtidx : number;
            // toggle
            if(_isSubtitleDisplay === true)  {
                targetlang = hx.svc.SETTING_UTIL.getLanguage().LangSubTtl;//get lang
                for (i=0; i < len ; i++) {
                    console.log ("  subtitle.list[i].language = " + subtitle.list[i].language);
                    if(subtitle.list[i].language === targetlang){
                        if(subtitle.list[i].hearingImpaired === hardofHearingEnabled){
                            targetindex = i;
                            break;
                        }
                    }
                }
                if (targetindex === null && hardofHearingEnabled === false){
                    for (i=0; i < len ; i++) {
                        if(subtitle.list[i].language === targetlang){
                            targetindex = i;
                            break;
                        }
                    }
                }
                if (targetindex !== null) {
                    target = subtitle.list[targetindex];
                    console.log (" targetlang " + targetlang + " targetindex " + targetindex + " len " + len) ;
                    subtidx = targetindex + 1;
                    sName = (target.language !== null) ? hx.il.iso639toString(target.language) : hx.l('LOC_SUBTITLE_ID') + ' ' + subtidx;
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + sName;
                    if (target.subType) {
                        subType = target.subType;
                    }
                    if (target.hearingImpaired) {
                        hardofHearing = true;
                    }
                }  else {
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_OFF_ID');
                }
            } else {
                message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_OFF_ID');
            }
        }
        return {
            'message': message,
            'subType': subType,
            'hardofHearing': hardofHearing
        };
    }

    toggleAudio(): any {
        var audio = null;
        var message = '';
        var encoding = '';
        var p = this._getAudioComponent();
        if (p !== undefined) {
            audio = p;
        }
        if (audio === null || audio.list === null || audio.list.length === 0) {
            message = hx.l('LOC_NOAUDIODATA_ID');
        } else {
            var cur = audio.selected;
            var target = null;
            var len = audio.list.length;
            var targetindex = audio.index;
            var aName, szLang2;
            // toggle
            if (targetindex === null) {
                targetindex = 0;
            } else {
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
                if (target.subType === _oipfDef.TAudioSubType.SUBTYPE_AUDIO_DUALMONO) {// Support Dual Mono
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
    }

    toggleAudiodescription(): any {
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
        } else {
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
                    cb : undefined
                });
                _audioModeHdmi = retObjAudio.audioModeHdmi;
                _audioModePcm = retObjAudio.audioModePcm;
                _audioType = retObjAudio.audioType;
                _lipsyncDelay = retObjAudio.lipSyncDelay;
                _audioDescription = retObjAudio.audioDescription;

                if( _audioDescription === true ){
                    _audioDescription = false;
                    message = hx.l('LOC_AUDIO_DESCRIPTION_ID') + ' : ' + hx.l('LOC_OFF_ID');
                } else {
                    _audioDescription = true;
                    message = hx.l('LOC_AUDIO_DESCRIPTION_ID') + ' : ' + hx.l('LOC_SET_ENGLISH_ID');
                }
                dsrvSettingUtil.setAudioInfo({audioDescription : _audioDescription, audioModePcm : _audioModePcm, audioModeHdmi : _audioModeHdmi, audioType : _audioType, lipSyncDelay : _lipsyncDelay});

            } else {
                message = hx.l('LOC_NO_AUDIO_DESCRIPTION_ID');
            }
        }
        return {
            'message': message,
            'encoding': encoding
        };
    }

    toggleSubtitle(): any {
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
        } else {
            var cur = subtitle.selected;
            var target = null;
            var len = subtitle.list.length;
            var targetindex = subtitle.index;
            // toggle
            if (targetindex === null) {
                targetindex = 0;
            } else {
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
            } else {
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
    }

    toggleSubtitleonoff(): any {
        var subtitle = null;
        var message = '';
        var subType = '';
        var p = this._getSubtitleComponent();
        var dsrvSettingUtil = hx.svc.SETTING_UTIL;
        var hardofHearing = false;
        var hardofHearingEnabled : boolean = dsrvSettingUtil.getHardofHearing();

        if (p !== undefined) {
            subtitle = p;
        }
        if (subtitle === null || subtitle.list === null || subtitle.list.length === 0) {
            message = hx.l('LOC_NOSUBTITLE_ID');
        } else {
            var cur = subtitle.selected;
            var target = null;
            var len = subtitle.list.length;
            var targetindex = subtitle.index;
            var targetlang : string;
            var i;
            var sName : string;
            var subtidx : number;
            // toggle
            if (cur !== null) {
                dsrvSettingUtil.setSubtitleEnabled(false);
                this._offSubtitleComponent();
                message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_OFF_ID');
            } else {
                targetlang = dsrvSettingUtil.getLanguage().LangSubTtl;//get lang
                for (i=0; i < len ; i++) {
                    console.log ("  subtitle.list[i].language = " + subtitle.list[i].language);
                    if(subtitle.list[i].language === targetlang){
                        if(subtitle.list[i].hearingImpaired === hardofHearingEnabled){
                            targetindex = i;
                            break;
                        }
                    }
                }
                if (targetindex === null && hardofHearingEnabled === false){
                    for (i=0; i < len ; i++) {
                        if(subtitle.list[i].language === targetlang){
                            targetindex = i;
                            break;
                        }
                    }
                }
                if (targetindex !== null) {
                    target = subtitle.list[targetindex];
                    console.log (" targetlang " + targetlang + " targetindex " + targetindex + " len " + len) ;
                    subtidx = targetindex + 1;
                    sName = (target.language !== null) ? hx.il.iso639toString(target.language) : hx.l('LOC_SUBTITLE_ID') + ' ' + subtidx;
                    message = hx.l('LOC_SUBTITLE_ID') + ' : ' + sName;
                }  else {
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

                //message = hx.l('LOC_SUBTITLE_ID') + ' : ' + hx.l('LOC_ON_ID');
                /*
                 if (subtitle.nextValue || subtitle.prevValue) {
                 dsrvSettingUtil.setSubtitleEnabled(!!subtitle.nextValue);
                 if (!subtitle.nextValue) {
                 dsrvAvCtrl.offSubtitleComponent();
                 }      */
            }
        }
        return {
            'message': message,
            'subType': subType,
            'hardofHearing': hardofHearing
        };
    }

    toggleMediaSubtitle(): any {
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
        } else {
            var cur = this.avController.SubtitleLanguage;
            var target = null, targetindex = null;
            var len = subtitle.length;
            if (this._bDisplaySubt) {
                targetindex = subtitle.indexOf(cur);
            } else {
                this.avController.SubtitleDisplay = this._bDisplaySubt = true;
            }
            // toggle
            if (targetindex === null) {
                targetindex = 0;
            } else {
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
            } else {
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
    }

    getMediaHasSubtitle(): any {
        return this.avController.hasSubtitle;
    }

    getMediaSubtitleDisplay(): any {
        var bDisplay = false;

        bDisplay = this.avController.SubtitleDisplay;
        if (bDisplay !== this._bDisplaySubt) {
            this._bDisplaySubt = bDisplay;
        }
        return this._bDisplaySubt;
        //return this.avController.SubtitleDisplay;
    }

    getMediaSubtitleLanguage(): any {
        return this.avController.SubtitleLanguage;
    }

    getMediaSubtitleSyncTime(): any {
        return this.avController.SubtitleSyncTime;
    }

    getMediaSubtitleFontSize(): any {
        return this.avController.SubtitleFontSize;
    }

    getMediaSubtitleTextPosition(): any {
        return this.avController.SubtitleTextPosition;
    }

    getMediaSubtitleSupportLanguages(): any {
        return this.avController.SubtitleSupportLanguages;
    }

    setMediaSubtitleDisplay(data): void {
        this.avController.SubtitleDisplay = this._bDisplaySubt = data.display;
    }

    setMediaSubtitleLanguage(data): void {
        this.avController.SubtitleLanguage = data.language;
    }

    setMediaSubtitleSyncTime(data): void {
        this.avController.SubtitleSyncTime = data.syncTime;
    }

    setMediaSubtitleFontSize(data): void {
        this.avController.SubtitleFontSize = data.size;
    }

    setMediaSubtitleTextPosition(data): void {
        this.avController.SubtitleTextPosition = data.position;
    }

    getSupportedSpeedsArray(): any[] {
        var idx, len, SpeedsArray = [], oipfNumCollectionSpeeds = this.videoBroadcast.playSpeeds;
        hx.log("dsrvAvCtrl", "[getSupportedSpeedsArray] oipfNumCollectionSpeeds : [" + oipfNumCollectionSpeeds + ']');
        len = oipfNumCollectionSpeeds.length;
        hx.log("dsrvAvCtrl", "[getSupportedSpeedsArray] len : [" + len + ']');
        for (idx = 0; idx < len; idx += 1) {
            SpeedsArray.push(oipfNumCollectionSpeeds.item(idx));
            hx.log("dsrvAvCtrl", "[getSupportedSpeedsArray] idx : [" + idx + ']\'s value : ' + SpeedsArray[idx]);
        }
        return SpeedsArray;
    }

    setRequestPincodeResult(aPincodeResult: boolean): void {
        this.videoBroadcast.resultPincode(aPincodeResult);
        hx.log("dsrvAvCtrl", "[setRequestPincodeResult]: " + aPincodeResult);
    }

    getProgrammePresent() {
        return this.videoBroadcast.programmes[0];
    }

    getChannelChanging(): boolean {
        return this._channelChanging;
    }

    setChannelChanging(param: boolean): void {
        this._channelChanging = param;
    }

    requestThumbnail(param): any {
        return this.videoBroadcast.requestThumbnail(param.path, param.position);
    }
    // ####################################################################################################
    // Private Methods
    private _initEventHandler(): void {
        this.defineEvents(['evtSetChannel', 'BlockAV', 'ChannelChangeSucceeded', 'ChannelChangeError', 'PlayStateChange', 'ParentalRatingChange', 'ProgrammesChanged',
            'DRMRightsError', 'CasMessageNotify', 'CasUiNotify', 'PlaySpeedsArrayChanged', 'audioChange', 'RctInfoChanged', 'NCDChanged', 'RequestPincode', 'RequestMhegTune', 'RequestMhegInterrupt','MhegIcsStatus' ,'NetworkUriChanged', 'HideNetflixPW']);
        // Finalize AV stop if there was TSR stop requested
        var fnCheckStopCurChannel = () => {
            if (this._paramBlockAV) {
                hx.log("dsrvAvCtrl", "blocking AV");
                this.videoBroadcast.stop();
                if (this._paramBlockAV.cb) {
                    this._paramBlockAV.cb();
                    delete this._paramBlockAV.cb;
                }
                this._paramBlockAV = null;
                return true;
            }
            return false;
        };
        var onChannelChangeSucceeded = (channel) => {
            hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]ChannelChangeSucceeded");
            var isReservation = this._checkLastService(channel);
            if (!fnCheckStopCurChannel()) {
                this.fireEvent('ChannelChangeSucceeded', {
                    channel: channel,
                    isRsv: isReservation
                });
                hx.svc.EPG.retrieveLiveProgrammes({
                    channel: channel
                });
            }
        };
        this.videoBroadcast.onChannelChangeSucceeded = onChannelChangeSucceeded;
        var onChannelChangeError = (channel, errorState) => {
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
            var isReservation = this._checkLastService(channel);
            if (errorState === 14) {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "ERCAS", //ERROR_CAS
                        "TRS": "R",
                        "CAS_ERROR" : errorState
                    });
                }
            }
            if (!fnCheckStopCurChannel()) {
                this.fireEvent('ChannelChangeError', {
                    channel: channel,
                    errorState: errorState,
                    isRsv: isReservation
                });
            }
        };
        this.videoBroadcast.onChannelChangeError = onChannelChangeError;
        var onRecordingEvent = (state, error, recordingId) => {
            mTSROnGoing = ((state === REC_STATE_TS_ACQUIRING_RESOURCES) || (state === REC_STATE_TS_STARTED));
            switch (state) {
                case REC_STATE_UNREALIZED:
                    mLiveStopping = false;
                    break;
            }
        };
        this.videoBroadcast.addEventListener('RecordingEvent', (e) => {
            onRecordingEvent(e.state, e.error, e.recordingID);
        });
        var PlayStateChange = (state, error) => {
            /*
             * state : it is defined in NativeVideoBroadcast.h's BroadcastPlaystate
             *          0 : BROADCAST_PLAYSTATE_UNREALIZED
             *          1 : BROADCAST_PLAYSTATE_CONNECTING
             *          2 : BROADCAST_PLAYSTATE_PRESENTING
             *          3 : BROADCAST_PLAYSTATE_STOPPED
             * error : it is same value in ChannelChangeError's errorState
             */
            hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]PlayStateChange (state : " + state + " error : " + error + ") ");
            this.fireEvent('PlayStateChange', {
                'state': state,
                'error': error,
                'from': 'VideoBroadcast'
            });
        };
        this.videoBroadcast.onPlayStateChange = PlayStateChange;
        var ParentalRatingChange = (contentID, rating, DRMSystemID, blocked) => {
            hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]ParentalRatingChange (contentID : " + contentID + " , rating : " + rating + " , DRMSystemID : " + DRMSystemID + " , blocked : " + blocked);
            this.fireEvent('ParentalRatingChange', {
                contentID: contentID,
                rating: rating,
                DRMSystemID: DRMSystemID,
                blocked: blocked
            });
        };
        this.videoBroadcast.onParentalRatingChange = ParentalRatingChange;
        var ProgrammesChanged = () => {
            hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]ProgrammesChanged");
            hx.log("debug", "##### [dsrvAvCtrl]ProgrammesChanged");
            var programmeItem = new __PROVIDER__.CProgrammeProvider({
                oipfData: this.videoBroadcast.programmes[0]
            });
            //hx.log("warn", programmeItem.toString());
            this.fireEvent('ProgrammesChanged', programmeItem);
        };
        this.videoBroadcast.onProgrammesChanged = ProgrammesChanged;
        // 7.13.6 extensions for DRM right error
        // 7.14.6 Extensions to A/V Control object for DRM rights errors
        var DRMRightsError = (errorState, contentID, DRMSystemID, rightsIssuerURL) => {
            hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]DRMRightsError : errorState : " + errorState + "contentID : " + contentID);
            this.fireEvent('DRMRightsError', {
                errorState: errorState,
                contentID: contentID,
                DRMSystemID: DRMSystemID,
                rightsIssuerURL: rightsIssuerURL
            });
        };
        this.videoBroadcast.onDRMRightsError = DRMRightsError;
        var CasMessageNotify = (module, slot, seesionHandle, message) => {
            hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]CasMessageNotify : module : " + module + ", slot : " + slot + ", sessionHandle :  " + seesionHandle + ", message " + message + ' ##END##');
            this.fireEvent('CasMessageNotify', {
                param: {
                    'module': module,
                    'slot': slot,
                    'seesionHandle': seesionHandle,
                    'uiData': JSON.parse(message)
                }
            });
        };
        this.videoBroadcast.onCasMessageNotify = CasMessageNotify;
        var CasUiNotify = (module, slot, sessionHandle) => {
            hx.log("dsrvAvCtrl", "##### [dsrvAvCtrl]CasUiNotify : module : " + module + ", slot : " + slot + "sessionHandle : " + sessionHandle + ' ##END##');
            this.fireEvent('CasUiNotify', {
                param: {
                    'module': module,
                    'slot': slot,
                    'sessionHandle': sessionHandle
                }
            });
        };
        this.videoBroadcast.onCasUiNotify = CasUiNotify;
        var PlaySpeedsArrayChangedEvent = () => {
            hx.log("dsrvAvCtrl", "[dsrvAvCtrl]PlaySpeedsArrayChanged");
            this.fireEvent('PlaySpeedsArrayChanged', {
                from: 'VideoBroadcast'
            });
        };
        this.videoBroadcast.onPlaySpeedsArrayChanged = PlaySpeedsArrayChangedEvent;
        var RctInfoChanged = (aLinkCount: number, aRctInfo: RctLinkInfoCollection) => {
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
            this.fireEvent('RctInfoChanged', aLinkCount, aRctInfo);
        };
        this.videoBroadcast.onRctInfoChanged = RctInfoChanged;
        var NCDChanged = (aStartTime, aDuration, aMessage) => {
            hx.log("info", "NCDChanged");
            //TODO: need to wrappin
            hx.log("info", aStartTime);
            hx.log("info", aDuration);
            hx.log("info", aMessage);
            this.fireEvent('NCDChanged', aStartTime, aDuration, aMessage);
        };
        this.videoBroadcast.onNCDChanged = NCDChanged;
        this.operatorFeature.onNCDChanged = NCDChanged;
        var RequestPincode = (aMessage) => {
            hx.log("info", "RequestPincode");
            hx.log("info", aMessage);

            this.fireEvent('RequestPincode', aMessage);
        };
        this.videoBroadcast.onRequestPincode = RequestPincode;
        this.videoBroadcast.onRequestMhegTune = (aChannel: Channel, aServiceID: number, aTuneMode: __DEF__.eMhegTuneInfo) => {
            hx.log("debug", "name: " + aChannel.name + "ccid: " + aChannel.ccid  + " aServiceID: " + aServiceID + " aTuneMode: " + aTuneMode + "[" + __DEF__.eMhegTuneInfo[aTuneMode] + "]");
            this.fireEvent('RequestMhegTune', aChannel, aServiceID, aTuneMode);
        };
        this.videoBroadcast.onRequestMhegInterrupt = (aIsInterruputable: number) => {
            hx.log("debug", "[event] RequestMhegInterrupt: " + aIsInterruputable);
            this.fireEvent("RequestMhegInterrupt", aIsInterruputable);
        };

        this.videoBroadcast.onMhegIcsStatus = (param) => {
            hx.log("debug", "[event] onMhegIcsStatus: " + param);
            this._mhegIcsStatus = param;
            this.fireEvent("MhegIcsStatus", param);
        };

        this.videoBroadcast.onNetworkUriChanged = (uri) => {
            hx.log("debug", "[event] onNetworkUriChanged: " + uri);
            this.fireEvent("NetworkUriChanged", uri);
        }
    }

    private _setAVComponent(off, on): void {
        if (off !== null) {
            this._unSetComponent(off);
        }
        if (on !== null) {
            hx.log('dsrvAvCtrl', '[dsrvAvCtrl] changeAVComponent()');
            this._setComponent(on);
        }
    }

    private _unSetComponent(param): void {
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
        } else {
            this.videoBroadcast.unselectComponent(cmpt);
        }
    }

    private _setComponent(param): void {
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
        } else {
            if (this.videoBroadcast.selectComponent) {
                this.videoBroadcast.selectComponent(cmpt);
            }
        }
    }

    private __getComponent(param): _ifAudioDescript {
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
        } else {
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
        if (param === 2 /*subtitle */ && list !== null) {
            // [D-Book9] All receivers shall be capable of decoding and presenting DVB subtitles in accordance with EN 300 743.
            // For this, priority of DVB subtitle should be higher than EBU.
            // (DVB: 201, EBU: 202)
            var EOrderType = {
                201 : 1,
                202 : 2,
                undefined : 3
            };
            list.sort(function(a, b) {
                return (EOrderType[a.subType] || EOrderType.undefined) - (EOrderType[b.subType] || EOrderType.undefined);
            })
        }
        // get selected
        if (avStatus) {
            cmptActive = this.avController.getCurrentActiveComponents(param);
        } else {
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
    }

    private _getAudioComponent(): _ifAudioDescript {
        return this.__getComponent(1);
    }

    private _setAudioComponent(param): void {
        var off = param.off;
        var on = param.on;
        this._setAVComponent(off, on);
        this.fireEvent('audioChange', {
            'item': param.on
        });
    }

    private _getSubtitleComponent(): any {
        var ret;
        ret = this.__getComponent(2);
        return ret;
    }

    private _setSubtitleComponent(param): void {
        var off = param.off;
        var on = param.on;
        this._setAVComponent(off, on);
    }

    private _offSubtitleComponent(): void {
        var avStatus = this._isPbPlaying();
        if (avStatus) {
            this.avController.offComponent(2, -1);
        } else {
            this.videoBroadcast.offComponent(2, -1);
            // 2: subtitle -1: index
        }
    }

    private _isPbPlaying(): boolean {
        var avStatus = this.avController.playState;
        //0 Live 1 Play 3 Connecting
        if (avStatus >= 1 && avStatus <= 3) {
            return true;
        }
        return false;
    }

    private _copy(avcomponent): any {
        var p = this._prop;
        var c: any = {};
        var n;
        for (n = 0; n < p.length; n += 1) {
            c[p[n]] = avcomponent[p[n]];
        }
        c._wrapper = avcomponent;
        return c;
    }

    private _setChannel(param, aTuneInfo: __DEF__.eMhegTuneInfo = 0): void {
        hx.log('dsrvAvCtrl', '##### [dsrvAvCtrl] 1 setChannel << CCID >> : ' + param.channel.ccid + ' <<num>> : ' + param.channel.majorChannel + ' <<name>> :' + param.channel.name);
        hx.log('dsrvAvCtrl', 'param.channel.manualBlock: '+ param.channel.manualBlock);
        //this.videoBroadcast.requestAudioPath();
        this._channelChanging = true;
        if (aTuneInfo > 0) {
            this.videoBroadcast.setChannelWithTuneInfo(param.channel, aTuneInfo);
        } else {
            this.videoBroadcast.setChannel(param.channel, param.trickplay, param.contentAccessDescriptorURL);
        }
        this.fnSetFastChannelChange();
    }

    private fnSetFastChannelChange(): void {
        // fast channel change set
        if (!!hx.config.CONFIG_FCC_NUM) {
            var fun = () => {
                var lastSvc, nextSvc, prevSvc;
                if (!!this.videoBroadcast.setChannelFCC) {
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
                            this.videoBroadcast.setChannelFCC(nextSvc.channel, FCC_VIDEO_ID_1);
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
                            this.videoBroadcast.setChannelFCC(prevSvc.channel, FCC_VIDEO_ID_2);
                        }
                    }
                }
            };
            clearTimeout(mFCCTimer);
            mFCCTimer = setTimeout(fun, 1000);
        }
    }

    private _checkLastService(channel): boolean {
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
        if (channel.ccid === lastSvc.ccid || diff < 5000) {// 5sec
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
    }

    private _getSubtitleLanguageStr(langCode, idx): string {
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
    }
}
export = cDLAvCtrl;
