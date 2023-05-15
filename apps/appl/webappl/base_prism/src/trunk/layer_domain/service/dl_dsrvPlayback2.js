/// <reference path="../../def/sugar.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "../../model/model"], function (require, exports, __KERNEL__, __MODEL__) {
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
        STARTED: 0,
        ADD: 1,
        END: 2,
        ERROR: 3
    };
    var fnLog = function (log) {
        hx.log('dsrvPlayback', log);
    };
    var cDLPlayback = (function (_super) {
        __extends(cDLPlayback, _super);
        function cDLPlayback(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mPlayback = this.getPluginInstance('AVController'); // VideoOnDemand
            this.mStandbyItem = null;
            this.mNextAction = null;
            this.mCurrentItem = null;
            this.mediaItemList = null;
            this.mIsBgmOngoing = false;
            //hx.logadd('dsrvPlayback');
            hx.log('dsrvPlayback', 'create');
            this.defineEvents(['PlaySpeedChanged', 'PlayPositionChanged', 'EndOfBufferReached', 'PlaySpeedsArrayChanged', 'ReadyToPlay', 'RepresentationChange', 'PeriodChange', 'ContentPlayStarted', 'ParentalRatingChange', 'DRMRightsError', 'PlayStateStopped', 'PlayStatePlaying', 'PlayStatePaused', 'PlayStateConnecting', 'PlayStateBuffering', 'PlayStateFinished', 'PlayStateError', 'BGMListCreated', 'TrickRestrictModeChanged', 'AudioOutputRestrict', 'PlayStateChange', 'NoValidMusicFile']);
            this.mPlayback.addEventListener('PlaySpeedChanged', function (e) {
                _this.onPlaySpeedChanged(e.speed);
            });
            this.mPlayback.addEventListener('PlayPositionChanged', function (e) {
                _this.onPlayPositionChanged(e.position);
            });
            this.mPlayback.addEventListener('PlaySpeedsArrayChanged', function (e) {
                _this.onPlaySpeedsArrayChanged();
            });
            this.mPlayback.addEventListener('ReadyToPlay', function (e) {
                _this.onReadyToPlay();
            });
            this.mPlayback.addEventListener('RepresentationChange', function (e) {
                _this.onRepresentationChange(e.position);
            });
            this.mPlayback.addEventListener('PeriodChange', function (e) {
                _this.onPeriodChange(e.bandwidths, e.position);
            });
            this.mPlayback.addEventListener('PlayStateChange', function (e) {
                _this.onPlayStateChange();
            });
            this.mPlayback.addEventListener('ParentalRatingChange', function (e) {
                _this.onParentalRatingChange(e.contentID, e.rating, e.DRMSystemID, e.blocked);
            });
            this.mPlayback.addEventListener('DRMRightsError', function (e) {
                _this.onDRMRightsError(e.errorState, e.trickLimit);
            });
            this.mPlayback.addEventListener('TrickRestrictModeChanged', function (e) {
                _this.onTrickRestrictModeChanged();
            });
            this.mPlayback.addEventListener('AudioOutputRestrict', function (e) {
                _this.onAudioOutputRestrict();
            });
        }
        cDLPlayback.prototype.fnTrick = function (speed) {
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
        };
        cDLPlayback.prototype.fnForward = function (speed) {
            if (!speed || speed < 1) {
                speed = 1;
            }
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[fnForward] - speed = {0}', speed));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            return !!this.mPlayback.play(speed);
        };
        cDLPlayback.prototype.fnRewind = function (speed) {
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
        };
        cDLPlayback.prototype.fnGetBuffer = function () {
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
        };
        //API
        cDLPlayback.prototype.play = function (param) {
            var _this = this;
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
            }
            if (this.isMpbOnGoing()) {
                this.mStandbyItem = this.mCurrentItem;
                this.stop();
                setTimeout(function () {
                    _this.play({
                        item: _this.mStandbyItem,
                        isLastPosition: true
                    });
                    _this.mStandbyItem = null;
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
                this.mNextAction = function () {
                    if (position) {
                        position = position * 1000;
                        _this.mPlayback.seek(position);
                    }
                    _this.mNextAction = null;
                };
            }
            return result;
        };
        cDLPlayback.prototype.playBGM = function (param) {
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
            fnLog(hx.util.format('[fnPlayBGM] - items = {0}', isLastPosition));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            result = this.mPlayback.play(1);
            isLastPosition = false;
            return result;
        };
        cDLPlayback.prototype.makeBGM = function () {
            var _this = this;
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
                hx.svc.MEDIA_ITEM_MANAGER.browseContent(path, type, count, ext, sort, rootPath, function (param) {
                    if (mediaItemList.length === 0) {
                        mediaItemList = param.mediaItemList;
                    }
                    else {
                        mediaItemList = mediaItemList.concat(param.mediaItemList);
                    }
                    if (mediaItemList.length === 0) {
                        _this.fireEvent('NoValidMusicFile');
                        return;
                    }
                    if (param.state === ECDS_STATE.END) {
                        _this.createBGMList(mediaItemList);
                    }
                });
            }
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[fnMakeBGM]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
        };
        cDLPlayback.prototype.createBGMList = function (mediaItemList) {
            if (this.mIsBgmOngoing) {
                if (mediaItemList.length !== 0) {
                    this.playBGM({
                        items: mediaItemList[0],
                        isLastPosition: false
                    });
                    //fnPlayBGM(mediaItemList[i]);
                    this.fireEvent('BGMListCreated', {
                        bgmItemList: mediaItemList
                    });
                }
            }
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            for (var i = 0; i < mediaItemList.length; i += 1) {
                fnLog(hx.util.format('[fnCreateBGMList] - mediaItemList = {0}', mediaItemList[i].getTitle()));
            }
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
        };
        cDLPlayback.prototype.pause = function (param) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[fnPause]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            return !!this.mPlayback.play(0);
        };
        cDLPlayback.prototype.resume = function () {
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
        };
        cDLPlayback.prototype.seek = function (param) {
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
        };
        cDLPlayback.prototype.stop = function () {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[fnStop]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            if (this.mPlayback) {
                return this.mPlayback.stop();
            }
            return false;
        };
        cDLPlayback.prototype.stopBGM = function () {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[fnStopBGM]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            //this.mIsBgmOngoing = false;
            if (this.mPlayback) {
                return !!this.mPlayback.stop();
            }
        };
        cDLPlayback.prototype.setData = function (item) {
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
        };
        cDLPlayback.prototype.getData = function () {
            return this.mPlayback.data;
        };
        cDLPlayback.prototype.getPosition = function () {
            var position = this.mPlayback.playPosition;
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[fnGetPosition] - position = {0}', position));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            return position;
        };
        cDLPlayback.prototype.getDuration = function () {
            var duration = this.fnGetBuffer();
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[fnGetDuration] - duration = {0}', duration));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            return duration;
        };
        cDLPlayback.prototype.getSpeed = function () {
            var speed = this.mPlayback.speed;
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[fnGetSpeed] - speed = {0}', speed));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            return speed;
        };
        cDLPlayback.prototype.getThumbnail = function (param) {
            var position = param.position;
            var uri = this.mPlayback.getPlayingThumbnail(position);
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[fnGetThumbnail] - position = {0}', position));
            fnLog(hx.util.format('[fnGetThumbnail] - uri = {0}', uri));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            return uri;
        };
        cDLPlayback.prototype.getSupportedSpeedsArray = function () {
            var i, length, speed, speedsArray = [], playSpeeds = this.mPlayback.playSpeeds;
            length = playSpeeds.length;
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            for (i = 0; i < length; i += 1) {
                speed = playSpeeds.item(i);
                fnLog(hx.util.format('[fnGetSupportedSpeedList] - speed = {0}', speed));
                speedsArray.push(speed);
            }
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            return speedsArray;
        };
        cDLPlayback.prototype.getPlayState = function () {
            var playState = this.mPlayback.playState;
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[fnGetPlayState] - playState = {0}', playState));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            return playState;
        };
        cDLPlayback.prototype.setSpeed = function (param) {
            var speed = param.speed;
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[fnSetSpeed] - speed = {0}', speed));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.mPlayback.play(speed);
            var result;
            return result;
        };
        cDLPlayback.prototype.isMpbOnGoing = function () {
            var result = false;
            var playState = this.mPlayback.playState;
            switch (playState) {
                case EPLAY_STATE_PLAYING:
                case EPLAY_STATE_PAUSED:
                case EPLAY_STATE_CONNECTING:
                case EPLAY_STATE_BUFFERING:
                    result = true;
                    break;
            }
            return result;
        };
        cDLPlayback.prototype.clearTimer = function () {
            return null;
        };
        // EVENT
        cDLPlayback.prototype.onPlaySpeedChanged = function (speed, item) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onPlaySpeedChanged] - speed = {0}', speed));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('PlaySpeedChanged', {
                speed: speed,
                item: this.mCurrentItem
            });
        };
        cDLPlayback.prototype.onPlayPositionChanged = function (position) {
            var buffer = this.fnGetBuffer();
            // fnLog('####################################################################################################');
            // fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onPlayPositionChanged] - position = {0}', position));
            fnLog(hx.util.format('[onPlayPositionChanged] - buffer = {0}', buffer));
            // fnLog('----------------------------------------------------------------------------------------------------');
            // fnLog('####################################################################################################');
            this.fireEvent('PlayPositionChanged', {
                position: Math.floor(position / 1000),
                buffer: buffer
            });
        };
        cDLPlayback.prototype.onEndOfBufferReached = function () {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[onEndOfBufferReached]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('EndOfBufferReached');
        };
        cDLPlayback.prototype.onPlaySpeedsArrayChanged = function () {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[onPlaySpeedsArrayChanged]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            var supportedSpeedsArray = this.getSupportedSpeedsArray();
            this.fireEvent('PlaySpeedsArrayChanged', {
                supportedSpeedsArray: supportedSpeedsArray
            });
        };
        cDLPlayback.prototype.onReadyToPlay = function () {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[onReadyToPlay]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('ReadyToPlay');
        };
        cDLPlayback.prototype.onRepresentationChange = function (position) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onRepresentationChange] - position = {0}', position));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('RepresentationChange', {
                position: position
            });
        };
        cDLPlayback.prototype.onPeriodChange = function (bandwidths, position) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onPeriodChange] - bandwidths = {0}', bandwidths));
            fnLog(hx.util.format('[onPeriodChange] - position = {0}', position));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('PeriodChange', {
                bandwidths: bandwidths,
                position: position
            });
        };
        cDLPlayback.prototype.onPlayStateChange = function () {
            var _this = this;
            var playState = this.mPlayback.playState;
            var error = this.mPlayback.error;
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onPlayStateChange] - playState = {0}', playState));
            fnLog(hx.util.format('[onPlayStateChange] - error = {0}', error));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            var onPlayStateStopped = function () {
                if (_this.mStandbyItem) {
                    _this.play({
                        item: _this.mStandbyItem,
                        isLastPosition: true
                    });
                    _this.mStandbyItem = null;
                }
                _this.fireEvent('PlayStateStopped', {
                    item: _this.mCurrentItem
                });
            };
            var onPlayStatePlaying = function () {
                if (_this.mNextAction) {
                    _this.mNextAction();
                }
                _this.fireEvent('PlayStatePlaying', {
                    item: _this.mCurrentItem
                });
            };
            var onPlayStatePaused = function () {
                _this.fireEvent('PlayStatePaused');
            };
            var onPlayStateConnecting = function () {
                _this.fireEvent('PlayStateConnecting');
            };
            var onPlayStateBuffering = function () {
                _this.fireEvent('PlayStateBuffering');
            };
            var onPlayStateFinished = function () {
                _this.fireEvent('PlayStateFinished', {
                    item: _this.mCurrentItem
                });
            };
            var onPlayStateError = function () {
                if (error != -1) {
                    _this.mNextAction = null;
                    _this.fireEvent('PlayStateError', {
                        error: error
                    });
                }
            };
            switch (playState) {
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
        };
        cDLPlayback.prototype.onParentalRatingChange = function (contentId, rating, systemId, blocked) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onParentalRatingChange] - contentId = {0}', contentId));
            fnLog(hx.util.format('[onParentalRatingChange] - rating = {0}', rating));
            fnLog(hx.util.format('[onParentalRatingChange] - systemId = {0}', systemId));
            fnLog(hx.util.format('[onParentalRatingChange] - blocked = {0}', blocked));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('ParentalRatingChange', {
                contentId: contentId,
                rating: rating,
                systemId: systemId,
                blocked: blocked
            });
        };
        cDLPlayback.prototype.onDRMRightsError = function (errorState, trickLimit) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onDRMRightsError] - errorState = {0}', errorState));
            fnLog(hx.util.format('[onDRMRightsError] - trickLimit = {0}', trickLimit));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('DRMRightsError', {
                errorState: errorState,
                trickLimit: trickLimit
            });
        };
        cDLPlayback.prototype.onTrickRestrictModeChanged = function () {
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
                    trickMode: trickMode
                });
            }
        };
        cDLPlayback.prototype.onAudioOutputRestrict = function () {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[onAudioOutputRestrict]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('AudioOutputRestrict');
        };
        return cDLPlayback;
    })(__KERNEL__);
    return cDLPlayback;
});
