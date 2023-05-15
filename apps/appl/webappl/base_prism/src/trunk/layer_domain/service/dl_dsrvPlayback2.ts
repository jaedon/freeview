/// <reference path="../../def/sugar.d.ts" />

// file name : fw/service/HMX/dservice/dl_dsrvPlayback2.js
/*jslint nomen:true, vars:true */
//    PLAYBACK: ['layer_domain/service/dl_dsrvPlayback2.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");

var dsrvSettingUtil = hx.svc.SETTING_UTIL;
var dsrvPlaylist = hx.svc.PLAYLIST;
var dsrvFrontPanel = hx.svc.FRONT_PANEL;

var EPLAY_STATE_STOPPED = 0;
var EPLAY_STATE_PLAYING = 1;
var EPLAY_STATE_PAUSED = 2;
var EPLAY_STATE_CONNECTING = 3;
var EPLAY_STATE_BUFFERING = 4;
var EPLAY_STATE_FINISHED = 5;
var EPLAY_STATE_ERROR = 6;

var EDRM_TRICK_NORESTRICT = 0;
var EDRM_TRICK_NOSKIP_FF2X = 1;
var EDRM_TRICK_NOSKIP_FF4X = 2;
var EDRM_TRICK_NOSKIP_FF8X = 3;
var EDRM_TRICK_NOSKIP_NOFF = 7;

var ECDS_STATE = {
    STARTED : 0,
    ADD : 1,
    END : 2,
    ERROR : 3
};
var fnLog = function(log) {
    hx.log('dsrvPlayback', log);
};

class cDLPlayback extends __KERNEL__ implements ifSVCPlayback {
    private mPlayback = this.getPluginInstance('AVController'); // VideoOnDemand
    private mStandbyItem = null;
    private mNextAction: Function = null;
    private mCurrentItem = null;
    private mediaItemList = null;
    mIsBgmOngoing = false;
    constructor(aServiceName: string) {
        super(aServiceName);
        //hx.logadd('dsrvPlayback');
        hx.log('dsrvPlayback', 'create');

        this.defineEvents(['PlaySpeedChanged', 'PlayPositionChanged', 'EndOfBufferReached', 'PlaySpeedsArrayChanged', 'ReadyToPlay', 'RepresentationChange', 'PeriodChange', 'ContentPlayStarted', 'ParentalRatingChange', 'DRMRightsError', 'PlayStateStopped', 'PlayStatePlaying', 'PlayStatePaused', 'PlayStateConnecting', 'PlayStateBuffering', 'PlayStateFinished', 'PlayStateError', 'BGMListCreated', 'TrickRestrictModeChanged','AudioOutputRestrict' , 'PlayStateChange', 'NoValidMusicFile']);

        this.mPlayback.addEventListener('PlaySpeedChanged', (e) => {
            this.onPlaySpeedChanged(e.speed);
        });
        this.mPlayback.addEventListener('PlayPositionChanged', (e) => {
            this.onPlayPositionChanged(e.position);
        });
        this.mPlayback.addEventListener('PlaySpeedsArrayChanged', (e) => {
            this.onPlaySpeedsArrayChanged();
        });
        this.mPlayback.addEventListener('ReadyToPlay', (e) => {
            this.onReadyToPlay();
        });
        this.mPlayback.addEventListener('RepresentationChange', (e) => {
            this.onRepresentationChange(e.position);
        });
        this.mPlayback.addEventListener('PeriodChange', (e) => {
            this.onPeriodChange(e.bandwidths, e.position);
        });
        this.mPlayback.addEventListener('PlayStateChange', (e) => {
            this.onPlayStateChange();
        });
        this.mPlayback.addEventListener('ParentalRatingChange', (e) => {
            this.onParentalRatingChange(e.contentID, e.rating, e.DRMSystemID, e.blocked);
        });
        this.mPlayback.addEventListener('DRMRightsError', (e) => {
            this.onDRMRightsError(e.errorState, e.trickLimit);
        });
        this.mPlayback.addEventListener('TrickRestrictModeChanged', (e) => {
            this.onTrickRestrictModeChanged();
        });
        this.mPlayback.addEventListener('AudioOutputRestrict', (e) => {
            this.onAudioOutputRestrict();
        });
    }
    private fnTrick(speed) {
        var position = this.getPosition();
        var buffer = this.fnGetBuffer();
        speed = speed || 1;
        if (position >= buffer) {
            this.onEndOfBufferReached();
        }
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnTrickPlay] - speed = {0}', speed));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return !!this.mPlayback.play(speed);
    }
    private fnForward(speed) {
        if (!speed || speed < 1) {
            speed = 1;
        }
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnForward] - speed = {0}', speed));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return !!this.mPlayback.play(speed);
    }
    private fnRewind(speed) {
        var newSpeed = (-1) * (1 / speed);
        if (!speed || speed > 1) {
            newSpeed = 1;
        }
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnRewind] - speed = {0}', speed));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return !!this.mPlayback.play(newSpeed);
    }
    private fnGetBuffer() {
        var buffer = this.mPlayback.playTime;
        if (this.mCurrentItem) {
            if (this.mCurrentItem instanceof __MODEL__.IRecordingItem) {
                if (this.mCurrentItem.isRecordingOnGoing() !== true) {
                    if (this.mCurrentItem && this.mCurrentItem.getRecordingDuration) {
                        buffer = this.mCurrentItem.getRecordingDuration() * 1000;
                    }
                }
            }
        }
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnGetBuffer] - buffer = {0}', buffer));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return buffer;
    }
    //API
    play(param): any {
        var item = param.item, isLastPosition = param.isLastPosition;
        var a = this;
        var result;
        var position = 0;
        if (item) {
            this.setData(item);
        }
        if (isLastPosition) {
            if (this.mCurrentItem) {
                position = this.mCurrentItem.getLastPlayPosition ? this.mCurrentItem.getLastPlayPosition() : 0;
            }
            /*if (position > mCurrentItem.getDuration()) {
             fnStop();
             }*/
        }
        if (this.isMpbOnGoing()) {
            this.mStandbyItem = this.mCurrentItem;
            this.stop();
            setTimeout(() => {
                this.play({
                    item: this.mStandbyItem,
                    isLastPosition: true
                });
                this.mStandbyItem = null;
            }, 2000);
            return;
        }

        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnPlay] - position = {0}', position));
        fnLog(hx.util.format('[fnPlay] - isLastPosition = {0}', isLastPosition));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        result = this.mPlayback.play(1);

        if (isLastPosition) {
            this.mNextAction = () => {
                if (position) {
                    position = position * 1000;
                    this.mPlayback.seek(position);
                }
                this.mNextAction = null;
            };
        }
        return result;
    }
    playBGM(param): any {
//        return fnPlayBGM(param.items , param.isLastPosition);
        var items = param.items, isLastPosition = param.isLastPosition;
        var a = this;
        var result;
        var position = 0;
        if (items) {
            this.setData(items);
        }
        if (this.isMpbOnGoing()) {
            this.mStandbyItem = this.mCurrentItem;
            this.stop();
            return;
        }
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnPlayBGM] - items = {0}', items.getTitle()));
        fnLog(hx.util.format('[fnPlayBGM] - items = {0}', isLastPosition ));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        result = this.mPlayback.play(1);
        isLastPosition = false;
        return result;
    }
    makeBGM(): any {
        var path, type, sort, count, ext, rootPath, mediaItemList = [];
        /*if (fnIsMpbOnGoing()) {
         fnStop();
         return;
         }debugger; */
        this.mIsBgmOngoing = true;
        var playlist = dsrvPlaylist.getPlaylist(__MODEL__.IMediaItem.TContentType.EMusic);
        if (playlist && playlist.length) {
            this.createBGMList(playlist);
        }
        else {
            path = '/mnt/hd2/Media/Music';
            type = __MODEL__.IMediaServer.TViewType.EOneDepthView;
            sort = 0;
            count = 16;
            ext = __MODEL__.IMediaItem.ExtensionList[__MODEL__.IMediaItem.TContentType.EMusic];
            rootPath = path;
            hx.svc.MEDIA_ITEM_MANAGER.browseContent(path, type, count, ext, sort, rootPath, (param) => {
                    if (mediaItemList.length === 0) {
                        mediaItemList = param.mediaItemList;
                    } else {
                        mediaItemList = mediaItemList.concat(param.mediaItemList);
                    }

                    if (mediaItemList.length === 0) {
                        this.fireEvent('NoValidMusicFile');
                        return;
                    }
                    if (param.state === ECDS_STATE.END) {
                        this.createBGMList(mediaItemList);
                    }
                }
            );
        }
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[fnMakeBGM]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
    }
    createBGMList(mediaItemList): void {
        if (this.mIsBgmOngoing) {
            if (mediaItemList.length !== 0) {
                this.playBGM({
                    items: mediaItemList[0],
                    isLastPosition: false
                });
                //fnPlayBGM(mediaItemList[i]);
                this.fireEvent('BGMListCreated', {
                    bgmItemList : mediaItemList
                });
            }
        }
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        for ( var i = 0; i < mediaItemList.length; i += 1) {
            fnLog(hx.util.format('[fnCreateBGMList] - mediaItemList = {0}', mediaItemList[i].getTitle()));
        }
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
    }
    pause(param): boolean {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[fnPause]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return !!this.mPlayback.play(0);
    }
    resume(): boolean {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[fnResume]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        var speed = this.getSpeed();
        if (speed === 1) {
            return true;
        }
        return !!this.mPlayback.play(1);
    }
    seek(param): boolean {
        var offset = param.offset;
        var buffer;
        buffer = this.fnGetBuffer();
        if (offset * 1000 >= buffer) {
            this.onEndOfBufferReached();
        }
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnSeek] - offset = {0}', offset));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.setSpeed(1);
        return !!this.mPlayback.seek(offset * 1000);
    }
    stop(): boolean {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[fnStop]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        if (this.mPlayback) {
            return this.mPlayback.stop();
        }
        return false;
    }
    stopBGM(): boolean {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[fnStopBGM]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        //this.mIsBgmOngoing = false;
        if (this.mPlayback) {
            return !!this.mPlayback.stop();
        }
    }
    setData(item): void {
        var fullPath = item.getFullPath();
        var playPath = fullPath.indexOf('//') > -1 ? fullPath : 'file:/' + fullPath;
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnSetData] - fullPath = {0}', fullPath));
        fnLog(hx.util.format('[fnSetData] - playPath = {0}', playPath));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.mPlayback.setSource(playPath);
        this.mCurrentItem = item;
    }
    getData(): any {
        return this.mPlayback.data;
    }
    getPosition(): any {
        var position = this.mPlayback.playPosition;
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnGetPosition] - position = {0}', position));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return position;
    }
    getDuration(): any {
        var duration = this.fnGetBuffer();
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnGetDuration] - duration = {0}', duration));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return duration;
    }
    getSpeed(): any {
        var speed = this.mPlayback.speed;
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnGetSpeed] - speed = {0}', speed));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return speed;
    }
    getThumbnail(param): any {
        var position = param.position;
        var uri = this.mPlayback.getPlayingThumbnail(position);
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnGetThumbnail] - position = {0}', position));
        fnLog(hx.util.format('[fnGetThumbnail] - uri = {0}', uri));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return uri;
    }
    getSupportedSpeedsArray(): any {
        var i, length, speed, speedsArray = [], playSpeeds = this.mPlayback.playSpeeds;
        length = playSpeeds.length;
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        for ( i = 0; i < length; i += 1) {
            speed = playSpeeds.item(i);
            fnLog(hx.util.format('[fnGetSupportedSpeedList] - speed = {0}', speed));
            speedsArray.push(speed);
        }
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return speedsArray;
    }
    getPlayState(): any {
        var playState = this.mPlayback.playState;
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnGetPlayState] - playState = {0}', playState));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        return playState;
    }
    setSpeed(param): any {
        var speed = param.speed;
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[fnSetSpeed] - speed = {0}', speed));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.mPlayback.play(speed);
        var result;
        return result;
    }
    isMpbOnGoing(): boolean {
        var result = false;
        var playState = this.mPlayback.playState;
        switch(playState) {
            case EPLAY_STATE_PLAYING:
            case EPLAY_STATE_PAUSED:
            case EPLAY_STATE_CONNECTING:
            case EPLAY_STATE_BUFFERING:
                result = true;
                break;
        }
        return result;
    }
    clearTimer(): any {
        return null;
    }
    // EVENT
    onPlaySpeedChanged(speed , item?) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onPlaySpeedChanged] - speed = {0}', speed));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('PlaySpeedChanged', {
            speed : speed,
            item : this.mCurrentItem
        });
    }
    onPlayPositionChanged(position) {
        var buffer = this.fnGetBuffer();
        // fnLog('####################################################################################################');
        // fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onPlayPositionChanged] - position = {0}', position));
        fnLog(hx.util.format('[onPlayPositionChanged] - buffer = {0}', buffer));
        // fnLog('----------------------------------------------------------------------------------------------------');
        // fnLog('####################################################################################################');
        this.fireEvent('PlayPositionChanged', {
            position : Math.floor(position / 1000),
            buffer : buffer
        });
    }
    onEndOfBufferReached() {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[onEndOfBufferReached]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('EndOfBufferReached');
    }
    onPlaySpeedsArrayChanged() {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[onPlaySpeedsArrayChanged]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        var supportedSpeedsArray = this.getSupportedSpeedsArray();
        this.fireEvent('PlaySpeedsArrayChanged', {
            supportedSpeedsArray : supportedSpeedsArray
        });
    }
    onReadyToPlay() {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[onReadyToPlay]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('ReadyToPlay');
    }
    onRepresentationChange(position) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onRepresentationChange] - position = {0}', position));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('RepresentationChange', {
            position : position
        });
    }
    onPeriodChange(bandwidths, position) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onPeriodChange] - bandwidths = {0}', bandwidths));
        fnLog(hx.util.format('[onPeriodChange] - position = {0}', position));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('PeriodChange', {
            bandwidths : bandwidths,
            position : position
        });
    }
    onPlayStateChange() {
        var playState = this.mPlayback.playState;
        var error = this.mPlayback.error;
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onPlayStateChange] - playState = {0}', playState));
        fnLog(hx.util.format('[onPlayStateChange] - error = {0}', error));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        var onPlayStateStopped = () => {
            if (this.mStandbyItem) {
                this.play({
                    item: this.mStandbyItem,
                    isLastPosition: true
                });
                this.mStandbyItem = null;
            }
            this.fireEvent('PlayStateStopped', {
                item : this.mCurrentItem
            });
        };
        var onPlayStatePlaying = () => {
            if (this.mNextAction) {
                this.mNextAction();
            }
            this.fireEvent('PlayStatePlaying', {
                item : this.mCurrentItem
            });
        };
        var onPlayStatePaused = () => {
            this.fireEvent('PlayStatePaused');
        };
        var onPlayStateConnecting = () => {
            this.fireEvent('PlayStateConnecting');
        };
        var onPlayStateBuffering = () => {
            this.fireEvent('PlayStateBuffering');
        };
        var onPlayStateFinished = () => {
            this.fireEvent('PlayStateFinished', {
                item : this.mCurrentItem
            });
        };
        var onPlayStateError = () => {
            if (error != -1) {
                this.mNextAction = null;
                this.fireEvent('PlayStateError', {
                    error : error
                });
            }
        };
        switch(playState) {
            case EPLAY_STATE_STOPPED:
                onPlayStateStopped();
                break;
            case EPLAY_STATE_PLAYING:
                onPlayStatePlaying();
                break;
            case EPLAY_STATE_PAUSED:
                onPlayStatePaused();
                break;
            case EPLAY_STATE_CONNECTING:
                onPlayStateConnecting();
                break;
            case EPLAY_STATE_BUFFERING:
                onPlayStateBuffering();
                break;
            case EPLAY_STATE_FINISHED:
                onPlayStateFinished();
                break;
            case EPLAY_STATE_ERROR:
                onPlayStateError();
                break;
        }
    }
    onParentalRatingChange(contentId, rating, systemId, blocked) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onParentalRatingChange] - contentId = {0}', contentId));
        fnLog(hx.util.format('[onParentalRatingChange] - rating = {0}', rating));
        fnLog(hx.util.format('[onParentalRatingChange] - systemId = {0}', systemId));
        fnLog(hx.util.format('[onParentalRatingChange] - blocked = {0}', blocked));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('ParentalRatingChange', {
            contentId : contentId,
            rating : rating,
            systemId : systemId,
            blocked : blocked
        });
    }
    onDRMRightsError(errorState, trickLimit) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onDRMRightsError] - errorState = {0}', errorState));
        fnLog(hx.util.format('[onDRMRightsError] - trickLimit = {0}', trickLimit));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('DRMRightsError', {
            errorState : errorState,
            trickLimit : trickLimit
        });
    }
    onTrickRestrictModeChanged() {
        var trickMode;
        switch (this.mPlayback.trickRestrictMode) {
            case 0:
                trickMode = EDRM_TRICK_NORESTRICT;
                break;
            case 1:
                trickMode = EDRM_TRICK_NOSKIP_FF2X;
                break;
            case 2:
                trickMode = EDRM_TRICK_NOSKIP_FF4X;
                break;
            case 3:
                trickMode = EDRM_TRICK_NOSKIP_FF8X;
                break;
            case 7:
                trickMode = EDRM_TRICK_NOSKIP_NOFF;
                break;
            default:
                break;
        }
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[onTrickRestrictModeChanged] trickMode : ' + trickMode);
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        if (trickMode) {
            this.fireEvent('TrickRestrictModeChanged', {
                trickMode : trickMode
            });
        }
    }

    onAudioOutputRestrict() {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[onAudioOutputRestrict]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('AudioOutputRestrict');
    }

}
export = cDLPlayback;