/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvTsr2.js
/*jslint nomen:true, vars:true */
//    TSR: ['layer_domain/service/dl_dsrvTsr2.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var eTSR_POSITION;
    (function (eTSR_POSITION) {
        eTSR_POSITION[eTSR_POSITION["EPOSITION_START"] = 0] = "EPOSITION_START";
        eTSR_POSITION[eTSR_POSITION["EPOSITION_CURRENT"] = 1] = "EPOSITION_CURRENT";
        eTSR_POSITION[eTSR_POSITION["EPOSITION_END"] = 2] = "EPOSITION_END";
    })(eTSR_POSITION || (eTSR_POSITION = {}));
    ;
    var eTSR_REC_STATE;
    (function (eTSR_REC_STATE) {
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_UNREALIZED"] = 0] = "EREC_STATE_UNREALIZED";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_SCHEDULED"] = 1] = "EREC_STATE_SCHEDULED";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_REC_PRESTART"] = 2] = "EREC_STATE_REC_PRESTART";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_REC_ACQUIRING_RESOURCES"] = 3] = "EREC_STATE_REC_ACQUIRING_RESOURCES";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_REC_STARTED"] = 4] = "EREC_STATE_REC_STARTED";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_REC_UPDATED"] = 5] = "EREC_STATE_REC_UPDATED";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_REC_COMPLETED"] = 6] = "EREC_STATE_REC_COMPLETED";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_TS_ACQUIRING_RESOURCES"] = 7] = "EREC_STATE_TS_ACQUIRING_RESOURCES";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_TS_STARTED"] = 8] = "EREC_STATE_TS_STARTED";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_TSR_BUFFER_COPYEND"] = 9] = "EREC_STATE_TSR_BUFFER_COPYEND";
        eTSR_REC_STATE[eTSR_REC_STATE["EREC_STATE_ERROR"] = 10] = "EREC_STATE_ERROR";
    })(eTSR_REC_STATE || (eTSR_REC_STATE = {}));
    ;
    var eTSR_DRM_TRICK;
    (function (eTSR_DRM_TRICK) {
        eTSR_DRM_TRICK[eTSR_DRM_TRICK["EDRM_TRICK_NORESTRICT"] = 0] = "EDRM_TRICK_NORESTRICT";
        eTSR_DRM_TRICK[eTSR_DRM_TRICK["EDRM_TRICK_NOSKIP_FF2X"] = 1] = "EDRM_TRICK_NOSKIP_FF2X";
        eTSR_DRM_TRICK[eTSR_DRM_TRICK["EDRM_TRICK_NOSKIP_FF4X"] = 2] = "EDRM_TRICK_NOSKIP_FF4X";
        eTSR_DRM_TRICK[eTSR_DRM_TRICK["EDRM_TRICK_NOSKIP_FF8X"] = 3] = "EDRM_TRICK_NOSKIP_FF8X";
        eTSR_DRM_TRICK[eTSR_DRM_TRICK["EDRM_TRICK_NOSKIP_NOFF"] = 7] = "EDRM_TRICK_NOSKIP_NOFF";
    })(eTSR_DRM_TRICK || (eTSR_DRM_TRICK = {}));
    ;
    var eTSR_BROADCAST_PLAYSTATE;
    (function (eTSR_BROADCAST_PLAYSTATE) {
        eTSR_BROADCAST_PLAYSTATE[eTSR_BROADCAST_PLAYSTATE["EBROADCAST_PLAYSTATE_UNREALIZED"] = 0] = "EBROADCAST_PLAYSTATE_UNREALIZED";
        eTSR_BROADCAST_PLAYSTATE[eTSR_BROADCAST_PLAYSTATE["EBROADCAST_PLAYSTATE_CONNECTING"] = 1] = "EBROADCAST_PLAYSTATE_CONNECTING";
        eTSR_BROADCAST_PLAYSTATE[eTSR_BROADCAST_PLAYSTATE["EBROADCAST_PLAYSTATE_PRESENTING"] = 2] = "EBROADCAST_PLAYSTATE_PRESENTING";
        eTSR_BROADCAST_PLAYSTATE[eTSR_BROADCAST_PLAYSTATE["EBROADCAST_PLAYSTATE_STOPPED"] = 3] = "EBROADCAST_PLAYSTATE_STOPPED";
    })(eTSR_BROADCAST_PLAYSTATE || (eTSR_BROADCAST_PLAYSTATE = {}));
    ;
    //var util = dl.registerApiGroup('dsrvTsr2');
    //var mVideoBroadcast = util.getPluginInstance('VideoBroadcast');
    var dsrvEpg = hx.svc.EPG;
    var dsrvChlist = hx.svc.CH_LIST;
    var dsrvRecordings = hx.svc.RECORDINGS;
    var dsrvAvCtrl = hx.svc.AV_CTRL;
    var cDLTSR = (function (_super) {
        __extends(cDLTSR, _super);
        function cDLTSR(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mVideoBroadcast = this.getPluginInstance('VideoBroadcast');
            this.EMAX_DURATION = hx.config.durationTSR;
            this.mTimer = null;
            this.mStartTime = 0;
            this.mAvailableTrick = true;
            this.mTrickEnabled = true;
            this.mGetOffsetEnabled = true;
            this.mIsChannelChanging = false;
            this.mChannelChangingTimer = -1;
            var that = this;
            hx.logadd('dsrvTsr2');
            that.defineEvents(['LiveStarted', 'TsrPBStarted', 'LiveStopping', 'PlayStateStopped', 'PlaySpeedChanged', 'PlayPositionChanged', 'PlayBufferChanged', 'RecordingEvent', 'ProgrammesChanged', 'ChannelChangeSucceeded', 'ChannelChangeError', 'TrickRestrictModeChanged', 'TrickError']);
            that.mVideoBroadcast.addEventListener('PlaySpeedChanged', function (e) {
                that.onPlaySpeedChanged(e.speed);
            });
            that.mVideoBroadcast.addEventListener('RecordingEvent', function (e) {
                that.onRecordingEvent(e.state, e.error, e.recordingID);
            });
            that.mVideoBroadcast.addEventListener('PlayStateChange', function (e) {
                that.onPlayStateChange(e.state, e.error);
            });
            that.mVideoBroadcast.addEventListener('ProgrammesChanged', function (e) {
                that.onProgrammesChanged();
            });
            that.mVideoBroadcast.addEventListener('ChannelChangeSucceeded', function (e) {
                that.onChannelChangeSucceeded(e.channel);
            });
            that.mVideoBroadcast.addEventListener('ChannelChangeError', function (e) {
                that.onChannelChangeError(e.channel, e.errorState);
            });
            that.mVideoBroadcast.addEventListener('TrickRestrictModeChanged', function (e) {
                that.onTrickRestrictModeChanged();
            });
            dsrvAvCtrl.addEventCb('evtSetChannel', this, function (e) {
                that.mIsChannelChanging = true;
                var currentChannel = _this.mVideoBroadcast.currentChannel;
                if (currentChannel && e && e.channel && (e.channel.ccid === currentChannel.ccid)) {
                    that.mIsChannelChanging = false;
                }
                if (that.mChannelChangingTimer > 0) {
                    clearTimeout(that.mChannelChangingTimer);
                }
                that.mChannelChangingTimer = setTimeout(function () {
                    that.mIsChannelChanging = false;
                }, 10000);
            });
        }
        /**************************************************
         * Functions
         **************************************************/
        cDLTSR.prototype.fnLog = function (aLog) {
            hx.log('dsrvTsr2', aLog);
        };
        cDLTSR.prototype.fnIsActive = function () {
            var isActive = false;
            var maxOffset = this.fnGetMaxOffset();
            var playbackOffset = this.fnGetPlaybackOffset();
            if (maxOffset > 0 && maxOffset >= playbackOffset) {
                isActive = true;
            }
            return isActive;
        };
        cDLTSR.prototype.fnPlay = function () {
            this.fnResume();
        };
        cDLTSR.prototype.fnPause = function () {
            if (!this.mVideoBroadcast.currentChannel) {
                return false;
            }
            return !!this.mVideoBroadcast.setSpeed(0);
        };
        cDLTSR.prototype.fnResume = function () {
            if (!this.mVideoBroadcast.currentChannel) {
                return false;
            }
            return !!this.mVideoBroadcast.setSpeed(1);
        };
        cDLTSR.prototype.fnSeek = function (aOffset) {
            var position, newOffset;
            var bResult;
            if (!this.fnIsActive()) {
                return false;
            }
            if (!this.mVideoBroadcast.currentChannel) {
                return false;
            }
            if (aOffset === null) {
                return false;
            }
            position = this.fnGetPlaybackOffset();
            newOffset = aOffset - position;
            if (newOffset < 0) {
                if (!this.mCurrentState || this.mCurrentState === 0 /* EREC_STATE_UNREALIZED */ || this.mCurrentState === 9 /* EREC_STATE_TSR_BUFFER_COPYEND */) {
                    this.mGetOffsetEnabled = false;
                }
                this.onLiveStopping();
            }
            bResult = (this.mVideoBroadcast.seek(newOffset, 1 /* EPOSITION_CURRENT */));
            if (this.mTimer) {
                this.fnResume();
                return !!bResult;
            }
            return false;
        };
        cDLTSR.prototype.fnTrick = function (aSpeed) {
            if (!this.fnIsTrickEnabled() && (this.fnGetPlaybackOffset() < this.fnGetMaxOffset())) {
                this.fireEvent('TrickError');
                return false;
            }
            var convSpeed = aSpeed || 1;
            if (convSpeed < 0) {
                this.onLiveStopping();
            }
            return !!(this.mVideoBroadcast.setSpeed(convSpeed));
        };
        cDLTSR.prototype.fnForward = function (aSpeed) {
            var newSpeed = aSpeed;
            if (!this.fnIsActive()) {
                return false;
            }
            if (!aSpeed || aSpeed < 1) {
                newSpeed = 1;
            }
            return !!(this.mVideoBroadcast.setSpeed(aSpeed));
        };
        cDLTSR.prototype.fnRewind = function (aSpeed) {
            if (!this.fnIsActive()) {
                return false;
            }
            var newSpeed = (-1) * (1 / aSpeed);
            if (!aSpeed || aSpeed > 1) {
                newSpeed = 1;
            }
            return !!(this.mVideoBroadcast.setSpeed(newSpeed));
        };
        cDLTSR.prototype.fnStop = function () {
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
        };
        cDLTSR.prototype.fnGetData = function () {
            return this.mVideoBroadcast.data;
        };
        cDLTSR.prototype.fnGetPlaybackOffset = function () {
            if (this.mGetOffsetEnabled) {
                var playbackOffset = this.mVideoBroadcast.playbackOffset;
                return playbackOffset;
            }
        };
        cDLTSR.prototype.fnGetMaxOffset = function () {
            if (this.mIsChannelChanging) {
                return 0;
            }
            var maxOffset = this.mVideoBroadcast.maxOffset;
            return maxOffset;
        };
        cDLTSR.prototype.fnGetStartOffset = function () {
            var startOffset = this.mVideoBroadcast.startOffset;
            return startOffset;
        };
        cDLTSR.prototype.fnGetDuration = function () {
            var maxOffset = this.getMaxOffset();
            var duration = this.EMAX_DURATION - maxOffset;
            return duration;
        };
        cDLTSR.prototype.fnGetPlayState = function () {
            var playState = this.mVideoBroadcast.playState;
            return playState;
        };
        cDLTSR.prototype.fnGetSpeed = function () {
            if (!this.fnIsTsrOngoing()) {
                return 1;
            }
            var playSpeed = this.mVideoBroadcast.playSpeed;
            return playSpeed;
        };
        cDLTSR.prototype.fnSetSpeed = function (aSpeed) {
            this.fnTrick(aSpeed);
        };
        cDLTSR.prototype.fnInitStartTime = function () {
            var now = new Date().toSeconds();
            var maxOffset = this.getMaxOffset();
            this.mStartTime = new Date((now - maxOffset) * 1000).toSeconds();
        };
        cDLTSR.prototype.fnGetStartTime = function () {
            return this.mStartTime;
        };
        cDLTSR.prototype.fnIsTsrOngoing = function () {
            return (this.mCurrentState === 7 /* EREC_STATE_TS_ACQUIRING_RESOURCES */ || this.mCurrentState === 8 /* EREC_STATE_TS_STARTED */);
        };
        cDLTSR.prototype.fnIsRecordingOngoing = function () {
            var i, length, item, recording, onGoing = false;
            var channel = dsrvChlist.getCurrentChannel().channel;
            var recordings = dsrvRecordings.getCurrentRecordings();
            if (channel && recordings && recordings.length) {
                length = recordings.length;
                for (i = 0; i < length; i += 1) {
                    recording = recordings[i];
                    if (recording && recording.getChannelId() === channel.ccid) {
                        onGoing = true;
                        break;
                    }
                }
            }
            return onGoing;
        };
        cDLTSR.prototype.fnIsChaseOngoing = function () {
            if (this.fnIsTsrOngoing() && this.fnIsRecordingOngoing()) {
                return true;
            }
            return false;
        };
        cDLTSR.prototype.fnIsAvailableTrick = function () {
            var availableTrick = false;
            if ((this.fnGetMaxOffset() >= 10) && this.mAvailableTrick) {
                availableTrick = true;
            }
            return availableTrick;
        };
        cDLTSR.prototype.fnGetTrickRestrictMode = function () {
            var trickMode = this.mVideoBroadcast.trickRestrictMode;
            return trickMode;
        };
        cDLTSR.prototype.fnIsTrickEnabled = function () {
            var playSpeeds = this.mVideoBroadcast.playSpeeds;
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
        };
        cDLTSR.prototype.fnGetCurrentTimeShiftMode = function () {
            var currentTimeShiftMode = this.mVideoBroadcast.currentTimeShiftMode;
            return currentTimeShiftMode;
        };
        /**************************************************
         * Events
         **************************************************/
        //util.defineEvents(['LiveStarted', 'TsrPBStarted', 'LiveStopping', 'PlayStateStopped', 'PlaySpeedChanged', 'PlayPositionChanged', 'PlayBufferChanged', 'RecordingEvent', 'ProgrammesChanged', 'ChannelChangeSucceeded', 'ChannelChangeError', 'TrickRestrictModeChanged', 'TrickError']);
        cDLTSR.prototype.onLiveStarted = function () {
            this.fnLog('####################################################################################################');
            this.fnLog('----------------------------------------------------------------------------------------------------');
            this.fnLog('[onLiveStarted]');
            this.fnLog('----------------------------------------------------------------------------------------------------');
            this.fnLog('####################################################################################################');
            if (!hx.svc.AV_CTRL.isLiveStopping()) {
                this.fireEvent('LiveStarted');
            }
        };
        cDLTSR.prototype.onTsrPBStarted = function () {
            this.fireEvent('TsrPBStarted');
        };
        cDLTSR.prototype.onLiveStopping = function () {
            this.fireEvent('LiveStopping');
        };
        cDLTSR.prototype.onRecordingStarted = function () {
            var _this = this;
            this.mAvailableTrick = false;
            setTimeout(function () {
                _this.mAvailableTrick = true;
            }, 10000);
        };
        cDLTSR.prototype.onPlaySpeedChanged = function (speed) {
            if (!this.fnIsTrickEnabled() && speed < 0) {
                this.fnResume();
            }
            else {
                this.fireEvent('PlaySpeedChanged', {
                    speed: speed
                });
            }
        };
        cDLTSR.prototype.onPlayPositionChanged = function () {
            var position = this.fnGetPlaybackOffset();
            var buffer = this.fnGetMaxOffset();
            this.fnLog(hx.util.format('[onPlayPositionChanged] - position = {0}', position));
            this.fnLog(hx.util.format('[onPlayPositionChanged] - buffer = {0}', buffer));
            if (buffer) {
                this.fireEvent('PlayPositionChanged', {
                    position: position,
                    buffer: buffer
                });
            }
        };
        cDLTSR.prototype.onPlayBufferChanged = function () {
            this.fireEvent('PlayBufferChanged', {
                buffer: this.fnGetMaxOffset()
            });
        };
        cDLTSR.prototype.onRecordingEvent = function (aState, aError, aRecordingId) {
            this.fnLog('####################################################################################################');
            this.fnLog('----------------------------------------------------------------------------------------------------');
            this.fnLog(hx.util.format('[onRecordingEvent] - state = {0}', aState));
            this.fnLog(hx.util.format('[onRecordingEvent] - error = {0}', aError));
            this.fnLog(hx.util.format('[onRecordingEvent] - recordingId = {0}', aRecordingId));
            this.fnLog('----------------------------------------------------------------------------------------------------');
            this.fnLog('####################################################################################################');
            this.fireEvent('RecordingEvent', {
                state: aState,
                error: aError,
                recordingId: aRecordingId
            });
            this.mCurrentState = aState;
            switch (aState) {
                case 0 /* EREC_STATE_UNREALIZED */:
                    this.onLiveStarted();
                    break;
                case 1 /* EREC_STATE_SCHEDULED */:
                    break;
                case 2 /* EREC_STATE_REC_PRESTART */:
                    break;
                case 3 /* EREC_STATE_REC_ACQUIRING_RESOURCES */:
                    break;
                case 4 /* EREC_STATE_REC_STARTED */:
                    this.onRecordingStarted();
                    break;
                case 5 /* EREC_STATE_REC_UPDATED */:
                    break;
                case 6 /* EREC_STATE_REC_COMPLETED */:
                    break;
                case 7 /* EREC_STATE_TS_ACQUIRING_RESOURCES */:
                    break;
                case 8 /* EREC_STATE_TS_STARTED */:
                    this.mGetOffsetEnabled = true;
                    this.onTsrPBStarted();
                    break;
                case 9 /* EREC_STATE_TSR_BUFFER_COPYEND */:
                    this.mAvailableTrick = true;
                    break;
                case 10 /* EREC_STATE_ERROR */:
                    break;
            }
        };
        cDLTSR.prototype.onProgrammesChanged = function () {
            this.fnLog('[onProgrammesChanged]');
            var programmes = this.mVideoBroadcast.programmes;
            this.fireEvent('ProgrammesChanged', {
                programmes: programmes
            });
        };
        cDLTSR.prototype.onChannelChangeSucceeded = function (aChannel) {
            this.mIsChannelChanging = false;
            if (this.mChannelChangingTimer > 0) {
                clearTimeout(this.mChannelChangingTimer);
            }
            this.fireEvent('ChannelChangeSucceeded', {
                channel: aChannel
            });
        };
        cDLTSR.prototype.onChannelChangeError = function (aChannel, aErrorState) {
            this.fnLog('[onChannelChangeError]');
            this.mIsChannelChanging = false;
            if (this.mChannelChangingTimer > 0) {
                clearTimeout(this.mChannelChangingTimer);
            }
            this.fireEvent('ChannelChangeError', {
                channel: aChannel,
                errorState: aErrorState
            });
        };
        cDLTSR.prototype.onTrickRestrictModeChanged = function () {
            var trickMode = -1;
            switch (this.mVideoBroadcast.trickRestrictMode) {
                case 0:
                    trickMode = 0 /* EDRM_TRICK_NORESTRICT */;
                    break;
                case 1:
                    trickMode = 1 /* EDRM_TRICK_NOSKIP_FF2X */;
                    break;
                case 2:
                    trickMode = 2 /* EDRM_TRICK_NOSKIP_FF4X */;
                    break;
                case 3:
                    trickMode = 3 /* EDRM_TRICK_NOSKIP_FF8X */;
                    break;
                case 7:
                    trickMode = 7 /* EDRM_TRICK_NOSKIP_NOFF */;
                    break;
                default:
                    break;
            }
            this.fnLog('[onTrickRestrictModeChanged] trickMode : ' + trickMode);
            if (trickMode !== -1) {
                this.fireEvent('TrickRestrictModeChanged', {
                    trickMode: trickMode
                });
            }
        };
        cDLTSR.prototype.onPlayStateChange = function (aState, aError) {
            this.fnLog('####################################################################################################');
            this.fnLog('----------------------------------------------------------------------------------------------------');
            this.fnLog(hx.util.format('[onPlayStateChange] - state = {0}', aState));
            this.fnLog(hx.util.format('[onPlayStateChange] - error = {0}', aError));
            this.fnLog('----------------------------------------------------------------------------------------------------');
            this.fnLog('####################################################################################################');
            switch (aState) {
                case 3 /* EBROADCAST_PLAYSTATE_STOPPED */:
                    this.fireEvent('PlayStateStopped');
                    break;
                default:
                    break;
            }
        };
        cDLTSR.prototype.play = function () {
            return this.fnPlay();
        };
        cDLTSR.prototype.pause = function () {
            return this.fnPause();
        };
        cDLTSR.prototype.resume = function () {
            return this.fnResume();
        };
        cDLTSR.prototype.seek = function (aParam) {
            return this.fnSeek(aParam.offset);
        };
        cDLTSR.prototype.trick = function (aParam) {
            return this.fnTrick(aParam.speed);
        };
        cDLTSR.prototype.forward = function (aParam) {
            return this.fnForward(aParam.speed);
        };
        cDLTSR.prototype.rewind = function (aParam) {
            return this.fnRewind(aParam.speed);
        };
        cDLTSR.prototype.stop = function () {
            return this.fnStop();
        };
        cDLTSR.prototype.getData = function () {
            return this.fnGetData();
        };
        cDLTSR.prototype.getPlaybackOffset = function () {
            return this.fnGetPlaybackOffset();
        };
        cDLTSR.prototype.getMaxOffset = function () {
            return this.fnGetMaxOffset();
        };
        cDLTSR.prototype.getStartOffset = function () {
            return this.fnGetStartOffset();
        };
        cDLTSR.prototype.getPlayState = function () {
            return this.fnGetPlayState();
        };
        cDLTSR.prototype.getSpeed = function () {
            return this.fnGetSpeed();
        };
        cDLTSR.prototype.setSpeed = function (aParam) {
            this.fnSetSpeed(aParam.speed);
        };
        cDLTSR.prototype.initStartTime = function () {
            this.fnInitStartTime();
        };
        cDLTSR.prototype.getStartTime = function () {
            return this.fnGetStartTime();
        };
        cDLTSR.prototype.isTsrOngoing = function () {
            return this.fnIsTsrOngoing();
        };
        cDLTSR.prototype.isRecordingOngoing = function () {
            return this.fnIsRecordingOngoing();
        };
        cDLTSR.prototype.isChaseOngoing = function () {
            return this.fnIsChaseOngoing();
        };
        cDLTSR.prototype.isAvailableTrick = function () {
            return this.fnIsAvailableTrick();
        };
        cDLTSR.prototype.isTrickEnabled = function () {
            return this.fnIsTrickEnabled();
        };
        cDLTSR.prototype.getCurrentTimeShiftMode = function () {
            return this.fnGetCurrentTimeShiftMode();
        };
        cDLTSR.prototype.clearTimer = function () {
            clearInterval(this.mTimer);
            this.mTimer = null;
        };
        cDLTSR.prototype.setTimer = function (time) {
            var _this = this;
            clearInterval(this.mTimer);
            this.mTimer = setInterval(function () {
                if (_this.mGetOffsetEnabled) {
                    _this.onPlayPositionChanged();
                    _this.onPlayBufferChanged();
                }
            }, time || 1000);
        };
        cDLTSR.prototype.getTrickRestrictMode = function () {
            return this.fnGetTrickRestrictMode();
        };
        return cDLTSR;
    })(__KERNEL__);
    return cDLTSR;
});
