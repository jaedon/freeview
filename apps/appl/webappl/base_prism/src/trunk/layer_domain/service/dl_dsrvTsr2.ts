/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvTsr2.js
/*jslint nomen:true, vars:true */
//    TSR: ['layer_domain/service/dl_dsrvTsr2.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

enum eTSR_POSITION {
    EPOSITION_START = 0,
    EPOSITION_CURRENT = 1,
    EPOSITION_END = 2,
};

enum eTSR_REC_STATE {
    EREC_STATE_UNREALIZED = 0,
    EREC_STATE_SCHEDULED = 1,
    EREC_STATE_REC_PRESTART = 2,
    EREC_STATE_REC_ACQUIRING_RESOURCES = 3,
    EREC_STATE_REC_STARTED = 4,
    EREC_STATE_REC_UPDATED = 5,
    EREC_STATE_REC_COMPLETED = 6,
    EREC_STATE_TS_ACQUIRING_RESOURCES = 7,
    EREC_STATE_TS_STARTED = 8,
    EREC_STATE_TSR_BUFFER_COPYEND = 9,
    EREC_STATE_ERROR = 10,
};

enum eTSR_DRM_TRICK {
    EDRM_TRICK_NORESTRICT = 0,
    EDRM_TRICK_NOSKIP_FF2X = 1,
    EDRM_TRICK_NOSKIP_FF4X = 2,
    EDRM_TRICK_NOSKIP_FF8X = 3,
    EDRM_TRICK_NOSKIP_NOFF = 7,
};

enum eTSR_BROADCAST_PLAYSTATE {
    EBROADCAST_PLAYSTATE_UNREALIZED = 0,
    EBROADCAST_PLAYSTATE_CONNECTING = 1,
    EBROADCAST_PLAYSTATE_PRESENTING = 2,
    EBROADCAST_PLAYSTATE_STOPPED = 3,
};

//var util = dl.registerApiGroup('dsrvTsr2');
//var mVideoBroadcast = util.getPluginInstance('VideoBroadcast');
var dsrvEpg = hx.svc.EPG;
var dsrvChlist = hx.svc.CH_LIST;
var dsrvRecordings = hx.svc.RECORDINGS;
var dsrvAvCtrl = hx.svc.AV_CTRL;

class cDLTSR extends __KERNEL__ implements ifSVCTsr {
    private mVideoBroadcast: any = this.getPluginInstance('VideoBroadcast');

    EMAX_DURATION = hx.config.durationTSR;

    mTimer : any = null;
    mStartTime : number = 0;
    mCurrentState : number;
    mAvailableTrick : boolean = true;
    mTrickEnabled : boolean = true;
    mGetOffsetEnabled: boolean = true;
    mIsChannelChanging: boolean = false;
    mChannelChangingTimer: any = -1;

    constructor(aServiceName: string) {
        super(aServiceName);
        var that = this;
        hx.logadd('dsrvTsr2');
        that.defineEvents(['LiveStarted', 'TsrPBStarted', 'LiveStopping', 'PlayStateStopped', 'PlaySpeedChanged', 'PlayPositionChanged', 'PlayBufferChanged', 'RecordingEvent', 'ProgrammesChanged', 'ChannelChangeSucceeded', 'ChannelChangeError', 'TrickRestrictModeChanged', 'TrickError']);

        that.mVideoBroadcast.addEventListener('PlaySpeedChanged', function(e) {
            that.onPlaySpeedChanged(e.speed);
        });
        that.mVideoBroadcast.addEventListener('RecordingEvent', function(e) {
            that.onRecordingEvent(e.state, e.error, e.recordingID);
        });
        that.mVideoBroadcast.addEventListener('PlayStateChange', function(e) {
            that.onPlayStateChange(e.state, e.error);
        });
        that.mVideoBroadcast.addEventListener('ProgrammesChanged', function(e) {
            that.onProgrammesChanged();
        });
        that.mVideoBroadcast.addEventListener('ChannelChangeSucceeded', function(e) {
            that.onChannelChangeSucceeded(e.channel);
        });
        that.mVideoBroadcast.addEventListener('ChannelChangeError', function(e) {
            that.onChannelChangeError(e.channel, e.errorState);
        });
        that.mVideoBroadcast.addEventListener('TrickRestrictModeChanged', function(e) {

            that.onTrickRestrictModeChanged();
        });
        dsrvAvCtrl.addEventCb('evtSetChannel', this, (e) => {
            that.mIsChannelChanging = true;
            var currentChannel = this.mVideoBroadcast.currentChannel;
            if (currentChannel && e && e.channel && (e.channel.ccid === currentChannel.ccid)) {
                that.mIsChannelChanging = false;
            }
            if (that.mChannelChangingTimer > 0) {
                clearTimeout(that.mChannelChangingTimer);
            }
            that.mChannelChangingTimer = setTimeout(() => { // defence code when booting
                that.mIsChannelChanging = false;
            }, 10000);
        });
    }

    /**************************************************
     * Functions
     **************************************************/
    fnLog (aLog) {
        hx.log('dsrvTsr2', aLog);
    }
    fnIsActive () {
        var isActive : boolean = false;
        var maxOffset : number = this.fnGetMaxOffset();
        var playbackOffset : number = this.fnGetPlaybackOffset();
        if (maxOffset > 0 && maxOffset >= playbackOffset) {
            isActive = true;
        }
        return isActive;
    }
    fnPlay () {
        this.fnResume();
    }
    fnPause () : boolean {
        if (!this.mVideoBroadcast.currentChannel) {
            return false;
        }
        return !!this.mVideoBroadcast.setSpeed(0);
    }
    fnResume () : boolean {
        if (!this.mVideoBroadcast.currentChannel) {
            return false;
        }
        return !!this.mVideoBroadcast.setSpeed(1);
    }
    fnSeek (aOffset) : boolean {
        var position : number, newOffset : number;
        var bResult : boolean;

        if (!this.fnIsActive()) {
            return false;
        }
        if (!this.mVideoBroadcast.currentChannel) {
            return false;
        }
        if(aOffset === null){
            return false;
        }
        position = this.fnGetPlaybackOffset();
        newOffset = aOffset - position;
        if (newOffset < 0) {
            if (!this.mCurrentState || this.mCurrentState === eTSR_REC_STATE.EREC_STATE_UNREALIZED || this.mCurrentState === eTSR_REC_STATE.EREC_STATE_TSR_BUFFER_COPYEND) {
                this.mGetOffsetEnabled = false;
            }
            this.onLiveStopping();
        }
        bResult = (this.mVideoBroadcast.seek(newOffset, eTSR_POSITION.EPOSITION_CURRENT));
        if (this.mTimer) {
            this.fnResume();
            return !!bResult;
        }
        return false;
    }
    fnTrick (aSpeed) {
        if (!this.fnIsTrickEnabled() && (this.fnGetPlaybackOffset() < this.fnGetMaxOffset())) {
            this.fireEvent('TrickError');
            return false;
        }
        var convSpeed : number = aSpeed || 1;
        if (convSpeed < 0) {
            this.onLiveStopping();
        }
        return !!(this.mVideoBroadcast.setSpeed(convSpeed));
    }
    fnForward (aSpeed) {
        var newSpeed : number = aSpeed;

        if (!this.fnIsActive()) {
            return false;
        }

        if (!aSpeed || aSpeed < 1) {
            newSpeed = 1;
        }
        return !!(this.mVideoBroadcast.setSpeed(aSpeed));
    }
    fnRewind (aSpeed) : boolean {
        if (!this.fnIsActive()) {
            return false;
        }
        var newSpeed : number = (-1) * (1 / aSpeed);
        if (!aSpeed || aSpeed > 1) {
            newSpeed = 1;
        }
        return !!(this.mVideoBroadcast.setSpeed(newSpeed));
    }
    fnStop () : boolean {
        clearInterval(this.mTimer);
        if (!this.fnIsActive()) {
            return false;
        }
        if (!this.mVideoBroadcast.currentChannel) {
            return false;
        }
        this.mTimer = null;
        this.mVideoBroadcast.setSpeed(1);
        return !!(this.mVideoBroadcast.stopTimeshift());
    }
    fnGetData () : any {
        return this.mVideoBroadcast.data;
    }
    fnGetPlaybackOffset () : any {
        if (this.mGetOffsetEnabled) {
            var playbackOffset = this.mVideoBroadcast.playbackOffset;
            return playbackOffset;
        }
    }
    fnGetMaxOffset () : any {
        if (this.mIsChannelChanging) {
            return 0;
        }
        var maxOffset : number = this.mVideoBroadcast.maxOffset;
        return maxOffset;
    }
    fnGetStartOffset () {
        var startOffset : number = this.mVideoBroadcast.startOffset;
        return startOffset;
    }
    fnGetDuration () : number {
        var maxOffset : number = this.getMaxOffset();
        var duration : number = this.EMAX_DURATION - maxOffset;
        return duration;
    }
    fnGetPlayState () : number {
        var playState : number = this.mVideoBroadcast.playState;
        return playState;
    }
    fnGetSpeed () : number {
        if (!this.fnIsTsrOngoing()) {
            return 1;
        }
        var playSpeed : number = this.mVideoBroadcast.playSpeed;
        return playSpeed;
    }
    fnSetSpeed (aSpeed) {
        this.fnTrick(aSpeed);
    }
    fnInitStartTime () {
        var now = new Date().toSeconds();
        var maxOffset : number = this.getMaxOffset();
        this.mStartTime = new Date((now - maxOffset) * 1000).toSeconds();
    }
    fnGetStartTime () {
        return this.mStartTime;
    }
    fnIsTsrOngoing () {
        return (this.mCurrentState === eTSR_REC_STATE.EREC_STATE_TS_ACQUIRING_RESOURCES || this.mCurrentState === eTSR_REC_STATE.EREC_STATE_TS_STARTED);
    }
    fnIsRecordingOngoing() : boolean {
        var i : number, length : number, item : any, recording : any, onGoing : boolean = false;
        var channel : any = dsrvChlist.getCurrentChannel().channel;
        var recordings : any = dsrvRecordings.getCurrentRecordings();
        if (channel && recordings && recordings.length) {
            length = recordings.length;
            for ( i = 0; i < length; i += 1) {
                recording = recordings[i];
                if (recording && recording.getChannelId() === channel.ccid) {
                    onGoing = true;
                    break;
                }
            }
        }
        return onGoing;
    }
    fnIsChaseOngoing () : boolean {
        if (this.fnIsTsrOngoing() && this.fnIsRecordingOngoing()) {
            return true;
        }
        return false;
    }
    fnIsAvailableTrick () : boolean {
        var availableTrick : boolean = false;
        if ((this.fnGetMaxOffset() >= 10) && this.mAvailableTrick) {
            availableTrick =  true;
        }
        return availableTrick;
    }
    fnGetTrickRestrictMode () : any {
        var trickMode : any = this.mVideoBroadcast.trickRestrictMode;
        return trickMode;
    }
    fnIsTrickEnabled () : any {
        var playSpeeds : any = this.mVideoBroadcast.playSpeeds;
        if (playSpeeds.length <= 2) {
            // this.fireEvent('TrickError');
            return false;
        }
        this.fnLog('####################################################################################################');
        this.fnLog('----------------------------------------------------------------------------------------------------');
        this.fnLog(hx.util.format('[fnIsTrickEnabled] - playSpeeds.length = {0}', playSpeeds.length));
        this.fnLog('----------------------------------------------------------------------------------------------------');
        this.fnLog('####################################################################################################');

        return true;
    }
    fnGetCurrentTimeShiftMode () : any {
        var currentTimeShiftMode = this.mVideoBroadcast.currentTimeShiftMode;
        return currentTimeShiftMode;
    }
    /**************************************************
     * Events
     **************************************************/
    //util.defineEvents(['LiveStarted', 'TsrPBStarted', 'LiveStopping', 'PlayStateStopped', 'PlaySpeedChanged', 'PlayPositionChanged', 'PlayBufferChanged', 'RecordingEvent', 'ProgrammesChanged', 'ChannelChangeSucceeded', 'ChannelChangeError', 'TrickRestrictModeChanged', 'TrickError']);
    onLiveStarted () {
        this.fnLog('####################################################################################################');
        this.fnLog('----------------------------------------------------------------------------------------------------');
        this.fnLog('[onLiveStarted]');
        this.fnLog('----------------------------------------------------------------------------------------------------');
        this.fnLog('####################################################################################################');
        if (!hx.svc.AV_CTRL.isLiveStopping()) {
            this.fireEvent('LiveStarted');
        }
    }
    onTsrPBStarted () {
        this.fireEvent('TsrPBStarted');
    }
    onLiveStopping () {
        this.fireEvent('LiveStopping');
    }
    onRecordingStarted () {
        this.mAvailableTrick = false;
        setTimeout(()=> {
            this.mAvailableTrick = true;
        }, 10000);
    }
    onPlaySpeedChanged (speed) {
        if (!this.fnIsTrickEnabled() && speed < 0) {
            this.fnResume();
        } else {
            this.fireEvent('PlaySpeedChanged', {
                speed : speed
            });
        }
    }
    onPlayPositionChanged () {
        var position : number = this.fnGetPlaybackOffset();
        var buffer : number = this.fnGetMaxOffset();
        this.fnLog(hx.util.format('[onPlayPositionChanged] - position = {0}', position));
        this.fnLog(hx.util.format('[onPlayPositionChanged] - buffer = {0}', buffer));
        if (buffer) {
            this.fireEvent('PlayPositionChanged', {
                position : position,
                buffer : buffer
            });
        }
    }
    onPlayBufferChanged () {
        this.fireEvent('PlayBufferChanged', {
            buffer : this.fnGetMaxOffset()
        });
    }
    onRecordingEvent (aState : number, aError : any, aRecordingId : any) {
        this.fnLog('####################################################################################################');
        this.fnLog('----------------------------------------------------------------------------------------------------');
        this.fnLog(hx.util.format('[onRecordingEvent] - state = {0}', aState));
        this.fnLog(hx.util.format('[onRecordingEvent] - error = {0}', aError));
        this.fnLog(hx.util.format('[onRecordingEvent] - recordingId = {0}', aRecordingId));
        this.fnLog('----------------------------------------------------------------------------------------------------');
        this.fnLog('####################################################################################################');
        this.fireEvent('RecordingEvent', {
            state : aState,
            error : aError,
            recordingId : aRecordingId
        });
        this.mCurrentState = aState;
        switch (aState) {
            case eTSR_REC_STATE.EREC_STATE_UNREALIZED:
                this.onLiveStarted();
                break;
            case eTSR_REC_STATE.EREC_STATE_SCHEDULED:
                break;
            case eTSR_REC_STATE.EREC_STATE_REC_PRESTART:
                break;
            case eTSR_REC_STATE.EREC_STATE_REC_ACQUIRING_RESOURCES:
                break;
            case eTSR_REC_STATE.EREC_STATE_REC_STARTED:
                this.onRecordingStarted();
                break;
            case eTSR_REC_STATE.EREC_STATE_REC_UPDATED:
                break;
            case eTSR_REC_STATE.EREC_STATE_REC_COMPLETED:
                break;
            case eTSR_REC_STATE.EREC_STATE_TS_ACQUIRING_RESOURCES:
                break;
            case eTSR_REC_STATE.EREC_STATE_TS_STARTED:
                this.mGetOffsetEnabled = true;
                this.onTsrPBStarted();
                break;
            case eTSR_REC_STATE.EREC_STATE_TSR_BUFFER_COPYEND:
                this.mAvailableTrick = true;
                break;
            case eTSR_REC_STATE.EREC_STATE_ERROR:
                break;
        }
    }
    onProgrammesChanged () {
        this.fnLog('[onProgrammesChanged]');
        var programmes : any = this.mVideoBroadcast.programmes;
        this.fireEvent('ProgrammesChanged', {
            programmes : programmes
        });
    }
    onChannelChangeSucceeded (aChannel) {
        this.mIsChannelChanging = false;
        if (this.mChannelChangingTimer > 0) {
            clearTimeout(this.mChannelChangingTimer);
        }
        this.fireEvent('ChannelChangeSucceeded', {
            channel : aChannel
        });
    }
    onChannelChangeError (aChannel, aErrorState) {
        this.fnLog('[onChannelChangeError]');
        this.mIsChannelChanging = false;
        if (this.mChannelChangingTimer > 0) {
            clearTimeout(this.mChannelChangingTimer);
        }
        this.fireEvent('ChannelChangeError', {
            channel : aChannel,
            errorState : aErrorState
        });
    }
    onTrickRestrictModeChanged () {
        var trickMode : number = -1;
        switch (this.mVideoBroadcast.trickRestrictMode) {
            case 0:
                trickMode = eTSR_DRM_TRICK.EDRM_TRICK_NORESTRICT;
                break;
            case 1:
                trickMode = eTSR_DRM_TRICK.EDRM_TRICK_NOSKIP_FF2X;
                break;
            case 2:
                trickMode = eTSR_DRM_TRICK.EDRM_TRICK_NOSKIP_FF4X;
                break;
            case 3:
                trickMode = eTSR_DRM_TRICK.EDRM_TRICK_NOSKIP_FF8X;
                break;
            case 7:
                trickMode = eTSR_DRM_TRICK.EDRM_TRICK_NOSKIP_NOFF;
                break;
            default:
                break;
        }
        this.fnLog('[onTrickRestrictModeChanged] trickMode : ' + trickMode);
        if (trickMode !== -1) {
            this.fireEvent('TrickRestrictModeChanged', {
                trickMode : trickMode
            });
        }
    }
    onPlayStateChange (aState, aError) {
        this.fnLog('####################################################################################################');
        this.fnLog('----------------------------------------------------------------------------------------------------');
        this.fnLog(hx.util.format('[onPlayStateChange] - state = {0}', aState));
        this.fnLog(hx.util.format('[onPlayStateChange] - error = {0}', aError));
        this.fnLog('----------------------------------------------------------------------------------------------------');
        this.fnLog('####################################################################################################');
        switch(aState) {
            case eTSR_BROADCAST_PLAYSTATE.EBROADCAST_PLAYSTATE_STOPPED:
                this.fireEvent('PlayStateStopped');
                break;
            default :
                break;
        }
    }

    play () {
        return this.fnPlay();
    }
    pause () {
        return this.fnPause();
    }
    resume () {
        return this.fnResume();
    }
    seek (aParam) {
        return this.fnSeek(aParam.offset);
    }
    trick (aParam) {
        return this.fnTrick(aParam.speed);
    }
    forward (aParam) {
        return this.fnForward(aParam.speed);
    }
    rewind (aParam) {
        return this.fnRewind(aParam.speed);
    }
    stop () {
        return this.fnStop();
    }
    getData () {
        return this.fnGetData();
    }
    getPlaybackOffset () {
        return this.fnGetPlaybackOffset();
    }
    getMaxOffset () {
        return this.fnGetMaxOffset();
    }
    getStartOffset () {
        return this.fnGetStartOffset();
    }
    getPlayState () {
        return this.fnGetPlayState();
    }
    getSpeed () {
        return this.fnGetSpeed();
    }
    setSpeed (aParam) {
        this.fnSetSpeed(aParam.speed);
    }
    initStartTime () {
        this.fnInitStartTime();
    }
    getStartTime () {
        return this.fnGetStartTime();
    }
    isTsrOngoing () {
        return this.fnIsTsrOngoing();
    }
    isRecordingOngoing () {
        return this.fnIsRecordingOngoing();
    }
    isChaseOngoing () {
        return this.fnIsChaseOngoing();
    }
    isAvailableTrick (){
        return this.fnIsAvailableTrick();
    }
    isTrickEnabled () {
        return this.fnIsTrickEnabled();
    }
    getCurrentTimeShiftMode () {
        return this.fnGetCurrentTimeShiftMode();
    }
    clearTimer () {
        clearInterval(this.mTimer);
        this.mTimer = null;
    }
    setTimer (time?) {
        clearInterval(this.mTimer);
        this.mTimer = setInterval(()=> {
            if (this.mGetOffsetEnabled) {
                this.onPlayPositionChanged();
                this.onPlayBufferChanged();
            }
        }, time || 1000);
    }
    getTrickRestrictMode () {
        return this.fnGetTrickRestrictMode();
    }
}
export =   cDLTSR;

