/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.control.player.js
/*jslint sloppy:true, nomen:true, vars:true */

import __MODEL__ = require('model/model');
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');
declare var hx: ifSugar;

module mPrism {
    var mRepeatOptionClassList = ['all', 'no', 'one'];
    var mPlayModeClassList = ['off', 'on'];
    var mRepeatMode = 0;
    var mPlayMode = 0;
    var $state;
    var $position;
    var $track;
    var $current;
    var $buffer;
    var $pointer;
    var $pointerTime;
    var $duration;
    var $divisionPoint;
    var $btnPlay = null;
    var $repeat = $('<div>');
    var $shuffle = $('<div>');
    var mRepeatOption: string;
    var mPlaybarAreaDrawer = {
        state : function(index, $item) {
            $state = $('<div>', {
                'id' : 'state'
            });
            $item.removeClass('-f');
            $item.append($state);
        },
        position : function(index, $item) {
            $position = $('<div>', {
                'class' : 'position text'
            });
            $position.text('00:00');
            $item.removeClass('-f');
            $item.append($position);
        },
        musicStartTime : function(index, $item) {
            $position = $('<div>', {
                'class' : 'position text'
            });
            $position.text('00:00:00');
            $item.removeClass('-f');
            $item.append($position);
        },
        split : function(index, $item) {
            var $split = $('<div>', {
                'class' : 'split'
            });
            $item.removeClass('-f');
            $item.append($split);
        },
        musicPlaybar : function(index, $item) {
            $track = $('<div>', {
                'class' : 'track'
            });
            $current = $('<div>', {
                'class' : 'current'
            });
            $item.append($track.append($current));
            $item.addClass('playbar');
            $item.removeClass('-f');
        },
        videoPlaybar : function(index, $item) {
            $track = $('<div>', {
                'class' : 'track'
            });
            $current = $('<div>', {
                'class' : 'current'
            });
            $buffer = $('<div>', {
                'class' : 'buffer',
                'width' : '100%'
            });
            $pointer = $('<div>', {
                'class' : 'pointer'
            });
            $pointerTime = $('<div>', {
                'class' : 'pointerTime',
                'text' : '00:00:00'
            });
            $track.append($buffer).append($current.append($pointer.append($pointerTime))).appendTo($item);
            $item.addClass('playbar');
        },
        duration : function(index, $item) {
            $duration = $('<div>', {
                'class' : 'duration text'
            });
            $duration.text('00:00');
            $item.removeClass('-f');
            $item.append($duration);
        },
        musicDuration : function(index, $item) {
            $duration = $('<div>', {
                'class' : 'duration text'
            });
            $duration.text('00:00:00');
            $item.removeClass('-f');
            $item.append($duration);
        },
        repeat : function(index, $item) {
            $repeat = $('<div>');
            $repeat.addClass('repeat ' + mRepeatOptionClassList[mRepeatOption]);
            $item.removeClass('-f');
            $item.append($repeat);
        },
        shuffle : function(index, $item) {
            $shuffle = $('<div>');
            $shuffle.addClass('shuffle ' + mPlayModeClassList[mPlayMode]);
            $item.removeClass('-f');
            $item.append($shuffle);
        },
        divisionPoint : function() {
            $divisionPoint = $('<div>');
            $divisionPoint.addClass('division');
        }
    };
    var fnSecondsToTime = function(seconds, format) {
        return new Date(1970, 1, 1, 0, 0, seconds).format(format);
    };
    export class CPlayerControl extends __CONTROLS__.CVerticalGroupControl {
        _buttonInfoList: any;
        _playbarAreaDrawers: Function[];
        _focusedItemIndex: number;
        _operationDisabled: boolean;
        _buttonLayout: any;
        _playItem: any;
        _currentPosition: any;
        _buffer: number;
        _showTitleTimer: any;

        constructor() {
            super();
        }
        setButtonInfoList(buttonInfoList) {
            this._buttonInfoList = buttonInfoList;
        }
        setPlaybarAreaDrawers(playbarAreaDrawers) {
            this._playbarAreaDrawers = playbarAreaDrawers;
        }
        setFocusedItemByKey(index) {
            this._focusedItemIndex = index;
        }
        disableOperation(disabled) {
            this._operationDisabled = disabled;
        }
        draw() {
            var a = this;
            var buttonLayout, buttonDrawer, buttonDrawers = [];
            var titleLayout, playbarLayout;
            titleLayout = new CHorizontalLayoutControl(null);
            titleLayout.setItemDrawers([
                function(index, $item) {
                    $item.addClass('title');
                    $item.removeClass('-f');
                    setTimeout(function() {
                        $item.addClass('hidden');
                    }, 4000);
                }]);
            playbarLayout = new CHorizontalLayoutControl(null);
            playbarLayout.setItemDrawers(a._playbarAreaDrawers);
            playbarLayout.setDrawParam({
                padding : 15
            });
            buttonLayout = new CHorizontalLayoutControl(null);
            buttonDrawer = function(index, $item) {
                var buttonInfo = a._buttonInfoList[index];
                $item.addClass('btn').html(buttonInfo.text);
                $item.addClass(buttonInfo['class']);
                if (a._operationDisabled) {
                    $item.removeClass('-f');
                }
                if (buttonInfo.selected) {
                    return true;
                }
            };
            if (a._buttonInfoList) {
                var i;
                for ( i = 0; i < a._buttonInfoList.length; i += 1) {
                    buttonDrawers.push(buttonDrawer);
                }
            }
            buttonLayout.setItemDrawers(buttonDrawers);
            buttonLayout.setDrawParam({
                padding : 15
            });
            buttonLayout.onItemSelected = function(index, $item) {
                var fn = null;
                if (a._buttonInfoList) {
                    fn = a._buttonInfoList[index].fn;
                }
                if (fn) {
                    setTimeout(function() {
                        fn($item);
                    }, 1);
                }
            };
            a.setChildControl([titleLayout, playbarLayout, buttonLayout], this._focusedItemIndex !== undefined ? this._focusedItemIndex : 2);
            a.setDrawParam({
                align : 'hcenter',
                childAlign : 'hcenter',
                padding : 0,
                margins : [63, 50, 60]
            });
            super.draw.call(a);
            a._buttonLayout = buttonLayout;
            if (a._operationDisabled) {
                a.addClass('disabled');
                $position.text('--:--');
            }
        }
        showTitle() {
            var a = this;
            var $title = $('.player .title');
            $title.text(a._playItem.getTitle());
            $title.removeClass('hidden');
            if (this._showTitleTimer) {
                clearTimeout(this._showTitleTimer);
                this._showTitleTimer = null;
            }
            this._showTitleTimer = setTimeout(function() {
                $title.addClass('hidden');
                this._showTitleTimer = null;
            }, 3000);
        }
        setData(item) {
            var a = this;
            var duration : number;
            a._playItem = item || a._playItem;
            var $title = $('.player .title');
            $title.text(a._playItem.getTitle());
            hx.log('error','prism.control.player Data getContentType : '+ a._playItem.getContentType());
            if (a._playItem.getContentType() === 1) {
                var dlnaItem = item.getFullPath().indexOf('hdlna://');
                var airPlayItem = item.getFullPath().indexOf('hairplay://');
                if ((dlnaItem === -1 ) && (airPlayItem === -1)) {
                    var sampleRate = item.getSampleRate();
                    var bitRate = item.getBitRate();
                    hx.log('error','prism.control.player setData sampleRate : '+sampleRate +' bitRate : '+bitRate);
                    if(sampleRate!=undefined && bitRate!=undefined){
                        hx.log('error','prism.control.player setData TEST1');
                        var $formatInfo = $('<div>');
                        $title.after($formatInfo.html(hx.util.format('<div class ="formatInfo">{0}kbps / {1}kHz</div>', Math.floor(bitRate / 1024), (sampleRate / 1000).toFixed(1))));
                    }
                }
            }

            duration = 0;
            if(a._playItem instanceof __MODEL__.IRecordingItem){
                duration = a._playItem.getRecordingDuration();
            } else {
                duration = a._playItem.getDuration();
            }
            a.setDuration(duration);
        }

        audioRestrict (){
            var a = this;
            var $title = $('.player .title');
            var $restrictInfo = $('<div>');
            var muteMsg = tr('LOC_HDAUDIO_MUTE_MSG_ID');
            $title.text(a._playItem.getTitle());
            $title.after($restrictInfo.html(hx.util.format('<div class = "restrictInfo">{0}', muteMsg)));
            //$title.append($restrictInfo.html(String.prototype.format('<div class = "restrictInfo">{0}', muteMsg)));
        }

        play(item) {
            var a = this;
            var duration = 0;
            var $btn = $('.btn.play');
            $state.removeClass('pause');
            $btn.removeClass('play').addClass('pause');
            if (item) {
                a.setData(item);
            }
        }
        pause() {
            var a = this;
            var $btn = $('.btn.pause');
            $state.addClass('pause');
            $btn.removeClass('pause').addClass('play');
        }
        resume() {
            var a = this;
            var $btn = $('.btn.play');
            $state.removeClass('pause');
            $btn.removeClass('play').addClass('pause');
        }
        skipForward(skipTime) {
            var a = this;
            var currentPosition = a._currentPosition;
            var buffer = a._buffer;
            var nextPosition = currentPosition + skipTime;
            if(nextPosition > buffer){
                nextPosition = buffer;
            }

            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE" : "EPS",
                    "TRS" : "I",
                    "SOURCE" : a._playItem instanceof __MODEL__.CTsrItem ? "Live" : a._playItem instanceof __MODEL__.IRecordingItem ? "Recording" : "File",
                    "STATUS" : "skipForward",
                    "VIEWPOINT" : a._currentPosition,
                    "NEXTPOINT" : nextPosition
                });
            }

            a.setPosition(isNaN(nextPosition) ? 0 : nextPosition);
        }
        skipRewind(skipTime) {
            var a = this;
            var currentPosition = a._currentPosition;
            var nextPosition = currentPosition - skipTime;
            if(skipTime > currentPosition){
                nextPosition = 0;
            }

            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE" : "EPS",
                    "TRS" : "I",
                    "SOURCE" : a._playItem instanceof __MODEL__.CTsrItem ? "Live" : a._playItem instanceof __MODEL__.IRecordingItem ? "Recording" : "File",
                    "STATUS" : "skipRewind",
                    "VIEWPOINT" : a._currentPosition,
                    "NEXTPOINT" : nextPosition
                });
            }

            a.setPosition(isNaN(nextPosition) ? 0 : nextPosition);
        }
        getPlayItem() {
            return this._playItem;
        }
        getPrevPlayItem() {
            return null;
        }
        getNextPlayItem() {
            //TODO: check below
//            return this._nextPlayItem;
            return null;
        }
        doKeyLeft() {
            var a = this;
            var $item = a.getActiveFocusInfo().$item;
            if ($item && $item.hasClass('playbar')) {
                a._fireEvent('SkipRewindButtonSelected');
                return true;
            }
            return false;
        }
        doKeyRight() {
            var a = this;
            var $item = a.getActiveFocusInfo().$item;
            var $stop = a._$self.find('.btn.stop');
            var $sub = a._$self.find('.btn.sub');
            if ($item) {
                if ($item.hasClass('playbar')) {
                    a._fireEvent('SkipForwardButtonSelected');
                    return true;
                }
                if ($item.hasClass('ff') && $stop.length && !$stop.hasClass('-f')) {
                    return true;
                }
                if ($item.hasClass('stop') && $sub.length && !$sub.hasClass('-f')) {
                    return true;
                }
            }
            return false;
        }
        doKeyDown() {
            var a = this;
            var $item = a.getActiveFocusInfo().$item;
            var $btn = a._$self.find('.btn');
            var $stop = a._$self.find('.btn.stop');
            var $sub = a._$self.find('.btn.sub');
            if ($item) {
                if ($item.hasClass('playbar') && $stop.length && !$stop.hasClass('-f')) {
                    if (!$btn.hasClass('-fd')) {
                        a._buttonLayout.doNavi('l');
                    }
                }
                if ($item.hasClass('playbar') && $sub.length && !$sub.hasClass('-f')) {
                    if (!$btn.hasClass('-fd')) {
                        a._buttonLayout.doNavi('l');
                    }
                }
            }
            return false;
        }
        getCurrentPosition() {
            var a = this;
            var position = a._currentPosition;
            return position;
        }
        setRepeatOption(repeatOption) {
            var a = this;
            var i, length = mRepeatOptionClassList.length;
            for ( i = 0; i < length; i += 1) {
                $repeat.removeClass(mRepeatOptionClassList[i]);
            }
            $repeat.addClass(mRepeatOptionClassList[repeatOption]);
            mRepeatOption = repeatOption;
        }
        setPlayMode(playMode) {
            var a = this;
            var i, length = mPlayModeClassList.length;
            for ( i = 0; i < length; i += 1) {
                $shuffle.removeClass(mPlayModeClassList[i]);
            }
            $shuffle.addClass(mPlayModeClassList[playMode]);
            mPlayMode = playMode;
        }
        setSpeed(speed) {
            var klass = '';
            var $btn = $('.btn.play');
            if ($btn.length === 0) {
                $btn = $('.btn.pause');
            }
            $btn.removeClass('play').removeClass('pause');
            if (speed === 0) {
                $btn.addClass('play');
                klass = 'pause';
            } else if (speed === 1) {
                klass = '';
                $btn.addClass('pause');
            } else {
                $btn.addClass('play');
                if (speed < 0) {
                    klass = 'fr';
                } else {
                    klass = 'ff';
                }
                klass += ('0' + Math.abs(speed).toString()).slice(-2);
            }
            $state.removeClass().addClass(klass);
            this.resetFocus(klass);
        }
        resetFocus(klass) {
            var buttonInfo, buttonInfoList = this._buttonInfoList;
            var i, length = buttonInfoList.length;
            for (i = 0; i < length; i += 1) {
                buttonInfo = buttonInfoList[i];
                if (klass.indexOf(buttonInfo['class']) === 0) {
                    this._buttonLayout.setFocusedItem(i, true);
                    this.doNavi('d');
                    break;
                }
            }
        }
        setBuffer(buffer) {
            var a = this;
            var startTime, startOffset;
            var item = a._playItem;
            var duration;
            if(item instanceof __MODEL__.IRecordingItem){
                if (item.isRecordingOnGoing() !== true) {
                    duration = item.getRecordingDuration();
                } else {
                    duration = buffer;
                    a.setDuration(duration);
                }
            }else{
                duration = item.getDuration();
            }
            var current = 0;
            a._buffer = Math.max((buffer || 0), (a._buffer || 0));
            if (buffer) {
                /*if (buffer > duration) {
                 duration = buffer;
                 a.setDuration(duration);
                 }*/
                current = (a._buffer / duration) * 100;
                current = current > 100 ? 100 : current;
                $buffer.css('width', current + '%');
            }
        }
        setStartTime(startTime) {
            var a = this;
            var date, timeText;
            var item = a._playItem;
            date = new Date(1970, 1, 1, 0, 0, 0, startTime * 1000);
            if ( item instanceof __MODEL__.IRecordingItem) {
                timeText = date.format('HH:MM');
            } else {
                timeText = date.format('HH:MM:ss');
            }
            $position.text(timeText);
        }
        setPosition(position) {
            var a = this;
            var date, timeText;
            var item = a._playItem;
            var current = 0;
            var buffer = a._buffer;
            var duration = a.getDuration();
            if (duration) {
                if (position > buffer) {
                    position = buffer;
                } else if (position < 0) {
                    position = 0;
                }
                current = (position / duration) * 100;
                if (current > 100) {
                    a._currentPosition = duration;
                    return;
                }
                $current.css('width', current + '%');
                timeText = fnSecondsToTime(position, 'HH:MM:ss');
                if ($pointerTime) {
                    $pointerTime.text(timeText);
                }
            }
            a._currentPosition = position;
        }
        getDuration() {
            var a = this;
            var item = a._playItem;
            var duration;
            var buffer = a._buffer;
            if (item instanceof __MODEL__.IRecordingItem) {
                if (item.isRecordingOnGoing() !== true) {
                    duration = item.getRecordingDuration();
                } else {
                    duration = buffer;
                }
            } else {
                duration = item.getDuration();
            }
            if (Number(duration) === 0) {
                duration = buffer || 0;
                a.setDuration(duration);
            }
            return duration;
        }
        setDuration(duration) {
            var a = this;
            var item = a._playItem;
            var timeText;
            if ( item instanceof __MODEL__.IRecordingItem) {
                timeText = fnSecondsToTime(duration, 'HH:MM');
            } else {
                $position.text('00:00:00');
                timeText = fnSecondsToTime(duration, 'HH:MM:ss');
            }
            $duration.text(timeText);
        }
        addDivisionPoint(programmes) {
            var a = this;
            var changePos = $buffer.width();
            var $point = $divisionPoint.clone();
            $point.css('left', changePos + 'px');
            $buffer.append($point);
        }
        changeToLive() {
            var a = this;
            a.addClass('live');
            a._$self.find('.stop').removeClass('-f');
            // a._buttonLayout.draw();
        }
        tsrPBStarted() {
            var a = this;
            a._$self.removeClass('live');
            a._$self.find('.stop').addClass('-f');
            // a._buttonLayout.draw();
        }
    }
    export class CBasicPlayerControl extends CPlayerControl {
        constructor(item) {
            super();
            var a = this;
            super.init.call(this, null);
            a._registerEvent(['PlayButtonSelected', 'PauseButtonSelected', 'ResumeButtonSelected', 'PrevButtonSelected', 'NextButtonSelected', 'SkipForwardButtonSelected', 'SkipRewindButtonSelected', 'FastForwardButtonSelected', 'FastRewindButtonSelected', 'StopButtonSelected', 'SubtitleButtonSelected']);
            a.addClass('player');
            a._playItem = item || a._playItem;
        }
        setData(item) {
            var a = this;
            var duration : number;
            a._playItem = item || a._playItem;
            var $title = $('.player .title');
            $title.text(a._playItem.getTitle());
            duration = 0;
            a._buffer = 0;
            if(a._playItem instanceof __MODEL__.IRecordingItem){
                duration = a._playItem.getRecordingDuration();
                duration += 30;
            } else {
                duration = a._playItem.getDuration();
            }
            a.setDuration(duration);
        }
    }
    export class CTsrPlayerControl extends CPlayerControl {
        _currentStartTime: any;
        constructor(item) {
            super();
            var a = this;
            super.init.call(a, null);
            a._registerEvent(['PlayButtonSelected', 'PauseButtonSelected', 'ResumeButtonSelected', 'PrevButtonSelected', 'NextButtonSelected', 'SkipForwardButtonSelected', 'SkipRewindButtonSelected', 'FastForwardButtonSelected', 'FastRewindButtonSelected', 'StopButtonSelected', 'SubtitleButtonSelected']);
            a.addClass('player');
            a._playItem = item || a._playItem;
        }
        setPosition(position) {
            var a = this;
            var date, timeText;
            var item = a._playItem;
            var current = 0;
            var buffer = a._buffer;
            var duration = item.getDuration();
            if (!$state.hasClass('pause')) {
                if (duration) {
                    if (position > buffer) {
                        position = buffer;
                    } else if (position < 0) {
                        position = 0;
                    }
                    if (position < buffer) {
                        a.tsrPBStarted();
                    } else {
                        a.changeToLive();
                    }
                    var startTime = item.getStartTime();
                    // prism.log('####################################################################################################');
                    // prism.log('----------------------------------------------------------------------------------------------------');
                    // prism.log(hx.util.format('[setPosition] - position = {0}', position));
                    // prism.log(hx.util.format('[setPosition] - buffer = {0}', buffer));
                    // prism.log(hx.util.format('[setPosition] - startTime = {0}', startTime));
                    // prism.log('----------------------------------------------------------------------------------------------------');
                    // prism.log('####################################################################################################');
                    if ((position !== a._currentPosition || buffer === position || a._currentStartTime !== startTime ) && position !== 0 && position !== 1 && buffer !== 0) {
                        date = new Date((startTime + position) * 1000);
                        timeText = date.format('HH:MM:ss');
                        current = (position / duration) * 100;
                        $current.css('width', (current > 100 ? 100 : current) + '%');
                        if ($pointerTime) {
                            $pointerTime.text(timeText);
                        }
                    }
                }
            }
            a._currentPosition = position;
            a._currentStartTime = startTime;
        }
        setBuffer(buffer) {
            var a = this;
            super.setBuffer.call(a, buffer);
            var startTime, startOffset, duration;
            var item = a._playItem;
            startTime = item.getStartTime();
            a.setStartTime(startTime);
            duration = item.getDuration();
            a.setDuration(duration);
        }
        setStartTime(startTime) {
            var a = this;
            var date, timeText;
            var item = a._playItem;
            date = new Date(startTime * 1000);
            timeText = date.format('HH:MM');
            $position.text(timeText);
        }
        setDuration(duration) {
            var a = this;
            var item = a._playItem;
            var startTime = item.getStartTime();
            var date = new Date((startTime + duration) * 1000);
            var timeText = date.format('HH:MM');
            $duration.text(timeText);
        }
    }
    export class CMusicPlayerControl extends CPlayerControl {
        constructor(item) {
            super();
            var a = this;
            super.init.call(this, null);
            a._registerEvent(['PlayButtonSelected', 'PauseButtonSelected', 'ResumeButtonSelected', 'PrevButtonSelected', 'NextButtonSelected', 'SkipForwardButtonSelected', 'SkipRewindButtonSelected', 'FastForwardButtonSelected', 'FastRewindButtonSelected', 'StopButtonSelected', 'SubtitleButtonSelected']);
            a.addClass('player');
            a._playItem = item || a._playItem;
        }
        setStartTime(startTime) {
            var a = this;
            var date, timeText;
            var item = a._playItem;
            if (item.isOperationEnabled()) {
                date = new Date(1970, 1, 1, 0, 0, 0, startTime * 1000);
                timeText = date.format('HH:MM:ss');
                $position.text(timeText);
            } else {
                $position.text('--:--');
            }
        }
        setDuration(duration) {
            var a = this;
            var item = a._playItem;
            var timeText;
            if (item.isOperationEnabled()) {
                timeText = fnSecondsToTime(duration, 'HH:MM:ss');
                $duration.text(timeText);
            } else {
                $duration.text('--:--');
            }
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createMusicPlayer = function(item) {
        var playerControl = new CMusicPlayerControl(item);
        playerControl.setPlaybarAreaDrawers([mPlaybarAreaDrawer.state, mPlaybarAreaDrawer.musicStartTime, mPlaybarAreaDrawer.musicPlaybar, mPlaybarAreaDrawer.musicDuration, mPlaybarAreaDrawer.repeat, mPlaybarAreaDrawer.shuffle]);
        playerControl.setButtonInfoList([{
            'text' : '',
            'class' : 'prev',
            'fn' : function($item) {
                playerControl._fireEvent('PrevButtonSelected');
            }
        }, {
            'selected' : true,
            'text' : '',
            'class' : 'pause',
            'fn' : function($item) {
                if ($item.hasClass('pause')) {
                    $item.removeClass('pause').addClass('play');
                    playerControl._fireEvent('PauseButtonSelected');
                } else {
                    $item.removeClass('play').addClass('pause');
                    playerControl._fireEvent('ResumeButtonSelected');
                }
            }
        }, {
            'text' : '',
            'class' : 'next',
            'fn' : function($item) {
                playerControl._fireEvent('NextButtonSelected');
            }
        }]);
        return playerControl;
    };
    prism.createVideoPlayer = function(item) {
        var playerControl;
        if ( item instanceof __MODEL__.CTsrItem) {
            playerControl = new CTsrPlayerControl(item);
        } else {
            playerControl = new CBasicPlayerControl(item);
        }
        var buttonInfoList = [];
        buttonInfoList.push({
            'text' : '',
            'class' : 'fr',
            'fn' : function($item) {
                playerControl._fireEvent('FastRewindButtonSelected');
            }
        });
        buttonInfoList.push({
            'selected' : true,
            'text' : '',
            'class' : 'pause',
            'fn' : function($item) {
                if ($item.hasClass('pause')) {
                    playerControl._fireEvent('PauseButtonSelected');
                } else {
                    playerControl._fireEvent('ResumeButtonSelected');
                }
            }
        });
        buttonInfoList.push({
            'text' : '',
            'class' : 'ff',
            'fn' : function($item) {
                playerControl._fireEvent('FastForwardButtonSelected');
            }
        });
        if ( item instanceof __MODEL__.CTsrItem) {
            playerControl.addClass('live');
            if ( item.isChase()) {
                playerControl.addClass('chase');
            }
            buttonInfoList.push({
                'text' : tr('LOC_GO_TO_LIVE_BTN_UPPER_CASE_ID'),
                'class' : 'stop',
                'fn' : function($item) {
                    playerControl._fireEvent('StopButtonSelected');
                }
            });
        } else {
            buttonInfoList.push({
                'text' : tr('LOC_STOP_ID'),
                'class' : 'stop',
                'fn' : function($item) {
                    playerControl._fireEvent('StopButtonSelected');
                }
            });
            if (!(item instanceof __MODEL__.IRecordingItem) && !(item instanceof __MODEL__.CNetworkMediaItem)) {
                buttonInfoList.push({
                    'text' : tr('LOC_SUB_BTN_ID'),
                    'class' : 'sub',
                    'fn' : function($item) {
                        playerControl._fireEvent('SubtitleButtonSelected');
                    }
                });
            }
        }
        playerControl.setPlaybarAreaDrawers([mPlaybarAreaDrawer.state, mPlaybarAreaDrawer.split, mPlaybarAreaDrawer.position, mPlaybarAreaDrawer.videoPlaybar, mPlaybarAreaDrawer.duration, mPlaybarAreaDrawer.divisionPoint]);
        playerControl.setButtonInfoList(buttonInfoList);
        playerControl.setFocusedItemByKey(1);
        return playerControl;
    };
    prism.CTsrPlayerControl = CTsrPlayerControl;
}
export = mPrism;