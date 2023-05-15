/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.videoplayback.js
/*jslint sloppy:true, nomen:true, vars:true */

declare var hx: ifSugar;
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CPLAYBACKDLG__ = require('prismux/prism.dlg.playback');

module mPrism {
    var mHidingTimer = null;
    var mTryMoving = false;
    var mTrySeeking = false;
    var mRequestedSeekPosition = null;
    var mDelayedAction = null;
    var mSkipTimeList = [];
    var mPressedKeyCount = 0;
    var EREPEAT_ALL = "0";
    var EREPEAT_NO = "1";
    var EREPEAT_ONE = "2";
    var EDRM_TRICK_NORESTRICT = 0;
    var EDRM_TRICK_NOSKIP_FF2X = 1;
    var EDRM_TRICK_NOSKIP_FF4X = 2;
    var EDRM_TRICK_NOSKIP_FF8X = 3;
    var EDRM_TRICK_NOSKIP_NOFF = 7;
    var TIMEOUT_PLAYBAR = 3000;
    var TIMEOUT_PLAYBAR_FIRST = 5000;

    export class CVideoPlaybackDlg extends __PRISM_CPLAYBACKDLG__.CPlaybackDlg {
        _originalItems: any[];
        _items: any[];
        _videoPlayer: any;
        _selectedIndex: number;
        _item: any;
        _currentPosition: number;

        constructor(layeredGroupControl) {
            super();
            this._registerEvent(['Show', 'Hide']);
            this.init(layeredGroupControl);
            mRequestedSeekPosition = null;
            this.addClass('videoPlayback');
        }
        setData(items) {
            var a = this;
            a._originalItems = items;
            a._items = items;
            a._videoPlayer.setData(items[this._selectedIndex || 0]);
        }
        setDuration(duration) {
            var a = this;
            a._originalItems[0].setDuration(duration);
            a._items[0].setDuration(duration);
            a._videoPlayer._playItem.setDuration(duration);
            a._videoPlayer.setDuration(duration);
        }
        getCurrentPosition(): number {
            return this._currentPosition;
        }
        setStartIndex(startIndex) {
            this._selectedIndex = startIndex;
        }
        createPlayer(item) {
            var a = this;
            a._item = item;
            a._videoPlayer = prism.createVideoPlayer(item);
        }
        doOpen() {
            var a = this;
            var i, length;
            var initialFocus = 0;
            var childControl = [];
            var repeatOption = a.doGetVideoOption();
            var videoPlayer = a._videoPlayer;
            videoPlayer.addEventHandler('ResumeButtonSelected', videoPlayer, function () {
                a.show();
                a.resume();
            });
            videoPlayer.addEventHandler('PauseButtonSelected', videoPlayer, function () {
                a.show();
                a.pause();
            });
            videoPlayer.addEventHandler('StopButtonSelected', videoPlayer, function () {
                a.close();
            });
            videoPlayer.addEventHandler('PrevButtonSelected', videoPlayer, function () {
                a.prev();
            });
            videoPlayer.addEventHandler('NextButtonSelected', videoPlayer, function () {
                a.next();
            });
            videoPlayer.addEventHandler('SkipForwardButtonSelected', videoPlayer, function () {
                a.resetHidingTimer();
                a.skipForward();
            });
            videoPlayer.addEventHandler('SkipRewindButtonSelected', videoPlayer, function () {
                a.resetHidingTimer();
                a.skipRewind();
            });
            videoPlayer.addEventHandler('FastForwardButtonSelected', videoPlayer, function () {
                a.show();
                a.fastForward();
            });
            videoPlayer.addEventHandler('FastRewindButtonSelected', videoPlayer, function () {
                a.show();
                a.fastRewind();
            });
            videoPlayer.addEventHandler('SubtitleButtonSelected', videoPlayer, function () {
                a.resetHidingTimer();
                var i, length, supportedSubtitleList = [];
                var positionList = [644, 600, 556];
                var fontSizeList = [30, 33, 36];
                var settingDlg = prism.createSettingDlg(a._parent, {
                    timeout: 5000
                });
                var supportedSubtitles: any = a.doGetSupportedSubtitles();
                var language = a.doGetSubtitleLanguage();
                var fontSize = a.doGetSubtitleFontSize();
                var syncTime = a.doGetSubtitleSyncTime();
                var position = a.doGetSubtitlePosition();
                var bDisplaySubttl = a.doGetSubtitleDisplay();

                length = supportedSubtitles.length;
                supportedSubtitleList.push({
                    text: tr('LOC_OFF_ID'),
                    value: '-1'
                });
                var selectedIndex = 0;
                var lang;
                for (i = 0; i < length; i += 1) {
                    lang = supportedSubtitles.item(i);
                    if (lang === language) {
                        selectedIndex = i + 1;
                    }
                    supportedSubtitleList.push({
                        text: hx.il.getSubtitleLanguageText(lang, i + 1),
                        value: lang
                    });
                }
                if (bDisplaySubttl === false) {
                    selectedIndex = 0;
                }
                var settingListItems: any[] = [
                    {
                        type: 'multiSelection',
                        text: tr('LOC_LANGUAGE_ID'),
                        config: {
                            selectionList: supportedSubtitleList.length === 0 ? [
                                {
                                    text: '-1',
                                    value: 1
                                }
                            ] : supportedSubtitleList,
                            activeIndex: selectedIndex
                        }
                    },
                    {
                        type: 'multiSelection',
                        text: tr('LOC_STR_POSITION_ID'),
                        config: {
                            selectionList: [
                                {
                                    text: '-1',
                                    value: 644
                                },
                                {
                                    text: '0',
                                    value: 600
                                },
                                {
                                    text: '+1',
                                    value: 556
                                }
                            ],
                            activeIndex: positionList.indexOf(position) > -1 ? positionList.indexOf(position) : 1
                        }
                    },
                    {
                        type: 'multiSelection',
                        text: tr('LOC_STR_SIZE_ID'),
                        config: {
                            selectionList: [
                                {
                                    text: tr('LOC_STR_SMALL_ID'),
                                    value: 30
                                },
                                {
                                    text: tr('LOC_STR_MEDIUM_ID'),
                                    value: 33
                                },
                                {
                                    text: tr('LOC_STR_LARGE_ID'),
                                    value: 36
                                }
                            ],
                            activeIndex: fontSizeList.indexOf(fontSize) > -1 ? fontSizeList.indexOf(fontSize) : 1
                        }
                    },
                    {
                        type: 'custom',
                        text: tr('LOC_STR_SYNC_ID'),
                        handler: function (a, finished) {
                            a.resetHidingTimer();
                            var syncTime = a.doGetSubtitleSyncTime();
                            var syncDlg = prism.createSyncDlg(a._parent);
                            syncDlg.setSyncTime(syncTime);
                            syncDlg.addEventHandler('MinusSelected', syncDlg, function (syncTime) {
                                settingDlg.resetHidingTimer();
                                a.doSetSubtitleOption({
                                    syncTime: syncTime
                                });
                            });
                            syncDlg.addEventHandler('PlusSelected', syncDlg, function (syncTime) {
                                settingDlg.resetHidingTimer();
                                a.doSetSubtitleOption({
                                    syncTime: syncTime
                                });
                            });
                            syncDlg.open({
                                transition: {
                                    prevLayer: 'fadeOut',
                                    newLayer: 'moveUp'
                                },
                                onClosed: function (aClosedParam) {
                                    a.resetHidingTimer();
                                    if (aClosedParam === 'userAction') {
                                        syncTime = a.doGetSubtitleSyncTime();
                                    }
                                    a.doSetSubtitleOption({
                                        syncTime: syncTime
                                    });
                                    finished(syncTime + ' sec');
                                    if (a._syncDlg) {
                                        delete a._syncDlg;
                                    }
                                }
                            });
                            a._syncDlg = syncDlg;
                        },
                        value: (syncTime === undefined ? 0 : syncTime) + ' sec'
                    }
                ];
                settingDlg.setDrawParam('width', 700);
                settingDlg.setTitleHtml(tr('LOC_SEL_OPTION_VAL_ID'));
                settingDlg.setSettingListItems(settingListItems);
                settingDlg.onItemChanged = function (index, settingItem, changedValue) {
                };
                settingDlg.onSettingsItemSelected = function (param, finished) {
                    var index = param.itemKey;
                    var handler = settingListItems[index].handler;
                    if (handler) {
                        handler(a, finished);
                    }
                };
                settingDlg.onOk = function (changedInfoList) {
                    var i, length, optionIndex, changedInfo;
                    var option: any = {};
                    var optionList = ['language', 'position', 'fontSize', 'syncTime'];
                    length = changedInfoList.length;
                    for (i = 0; i < length; i += 1) {
                        changedInfo = changedInfoList[i];
                        optionIndex = changedInfo.index;
                        option[optionList[optionIndex]] = changedInfo.settingItem.config.selectionList[changedInfo.changedValue].value;
                    }
                    a.doSetSubtitleOption({
                        language: option.language,
                        position: option.position,
                        fontSize: option.fontSize
                    });
                };
                settingDlg.doKeyEscape = function () {
                    return settingDlg.doKeyBack();
                };
                settingDlg.open({
                    transition: {
                        prevLayer: 'moveDown',
                        newLayer: 'moveUp',
                        dontRestore: true
                    },
                    onClosed: function () {
                    }
                });
            });
            childControl.push(videoPlayer);
            a.setChildControl(childControl, initialFocus);
            a._videoPlayer = videoPlayer;
            this._fireEvent("Show");
        }
        isShown() {
            var a = this;
            var shown = false;
            if (a._$self.parent().position().top === 0) {
                shown = true;
            }
            a._shown = shown;
            return shown;
        }
        doPlay(item) {
            var a = this;
            a._videoPlayer.play(item);
            a.resetHidingTimer(TIMEOUT_PLAYBAR_FIRST);
            return false;
        }
        doResume() {
            var a = this;
            a._videoPlayer.resume();
        }
        doPause() {
            var a = this;
            a._videoPlayer.pause();
        }
        doStop() {
            var a = this;
            // a._videoPlayer.stop();
        }
        doNext() {
            var a = this;
            a._videoPlayer.next();
        }
        doPrev() {
            var a = this;
            a._videoPlayer.prev();
        }
        doSkipForward(skipTime, trickRestrictMode) {
            var a = this;
            var duration;
            if (trickRestrictMode) {
                if (!a.isSeekAvailable(trickRestrictMode)) {
                    //a.createSeekRestrictMsg();
                    a.createTrickRestrictMsg();
                    mRequestedSeekPosition = null;
                    return;
                }
            }
            mTryMoving = true;
            if (mPressedKeyCount > 0) {
                duration = a._videoPlayer.getDuration();
                if (duration) {
                    skipTime = Number(duration) / 100;
                }
            }
            a._videoPlayer.skipForward(skipTime);
            mRequestedSeekPosition = Math.floor(a._videoPlayer.getCurrentPosition());
            // prism.log('####################################################################################################');
            // prism.log('----------------------------------------------------------------------------------------------------');
            // prism.log(hx.util.format('[doSkipForward] - mRequestedSeekPosition = {0}', mRequestedSeekPosition));
            // prism.log('----------------------------------------------------------------------------------------------------');
            // prism.log('####################################################################################################');
            mDelayedAction = function () {
                a.requestSeek(mRequestedSeekPosition);
            };
            mPressedKeyCount += 1;
        }
        doSkipRewind(skipTime, trickRestrictMode) {
            var a = this;
            var duration;
            if (trickRestrictMode) {
                if (!a.isSeekAvailable(trickRestrictMode)) {
                    //a.createSeekRestrictMsg();
                    a.createTrickRestrictMsg();
                    mRequestedSeekPosition = null;
                    return;
                }
            }
            mTryMoving = true;
            if (mPressedKeyCount > 0) {
                duration = a._videoPlayer.getDuration();
                if (duration) {
                    skipTime = Number(duration) / 100;
                }
            }
            a._videoPlayer.skipRewind(skipTime);
            mRequestedSeekPosition = Math.floor(a._videoPlayer.getCurrentPosition());
            //         prism.log('####################################################################################################');
            // prism.log('----------------------------------------------------------------------------------------------------');
            // prism.log(hx.util.format('[doSkipRewind] - mRequestedSeekPosition = {0}', mRequestedSeekPosition));
            // prism.log('----------------------------------------------------------------------------------------------------');
            // prism.log('####################################################################################################');
            mDelayedAction = function () {
                a.requestSeek(mRequestedSeekPosition);
            };
            mPressedKeyCount += 1;
        }

        moveToCurrentPosition () {
            var a = this;
            if (mRequestedSeekPosition) {
                a.requestSeek(mRequestedSeekPosition);
            }
        }

        onKeyUp() {
            if (mDelayedAction) {
                mTryMoving = false;
                mDelayedAction();
                mDelayedAction = null;
                mPressedKeyCount = 0;
            }
        }
        doFastForward(playSpeedsArray, trickRestrictMode) {
            var a = this;
            var currentSpeed = a.requestGetSpeed();
            if (trickRestrictMode) {
                if (!a.isTrickAvailable(currentSpeed, trickRestrictMode)) {
                    a.createTrickRestrictMsg();
                    return;
                }
            }
            var currentIndex = playSpeedsArray.indexOf(currentSpeed);
            var nextSpeed = playSpeedsArray[currentIndex += 1];
            if (nextSpeed !== undefined) {
                nextSpeed = nextSpeed < 2 ? 2 : nextSpeed;
            } else {
                nextSpeed = playSpeedsArray.indexOf(2) > -1 ? 2 : 1;
            }
            a.requestSetSpeed(nextSpeed);
        }
        doFastRewind(playSpeedsArray, trickRestrictMode) {
            var a = this;
            var currentSpeed = a.requestGetSpeed();
            var currentIndex = playSpeedsArray.indexOf(currentSpeed);
            var nextSpeed = playSpeedsArray[currentIndex -= 1];
            if (nextSpeed !== undefined) {
                nextSpeed = nextSpeed > -2 ? -2 : nextSpeed;
            } else {
                nextSpeed = playSpeedsArray.indexOf(-2) > -1 ? -2 : 1;
            }
            a.requestSetSpeed(nextSpeed);
        }
        setSpeed(speed) {
            var a = this;
            a._videoPlayer.setSpeed(speed);
        }
        setBuffer(buffer) {
            var a = this;
            a._videoPlayer.setBuffer(buffer);
        }
        setStartTime(startTime) {
            var a = this;
            a._videoPlayer.setStartTime(startTime);
        }
        setPosition(position) {
            var a = this;
            var movable = false;

            if (position - a._currentPosition === 1) {
                movable = true;
            }
            if (!mTryMoving && !mTrySeeking) {
                if (!mRequestedSeekPosition || movable || (mRequestedSeekPosition && mRequestedSeekPosition === position)) {
                    a._videoPlayer.setPosition(position);
                    //prism.log('####################################################################################################');
                    //prism.log('----------------------------------------------------------------------------------------------------');
                    prism.log(hx.util.format('[setPosition] - position = {0}', position));
                    //prism.log('----------------------------------------------------------------------------------------------------');
                    //prism.log('####################################################################################################');
                    mRequestedSeekPosition = null;
                }
            }
            mTrySeeking = false;
            if ((a.requestGetSpeed() !== 1 && (position - a._currentPosition !== 1)) || !mHidingTimer) {
                a.resetHidingTimer();
            }
            a._currentPosition = position;
        }
        getPrevPlayItem(item?) {
            var a = this;
            var prevIndex = null;
            var prevPlayItem = null;
            var repeatOption = a.doGetVideoOption();
            var seriesRepeat = a.doGetSeriesRepeat();
            if (a._items.length === undefined) {
                prevPlayItem = null;
            } else {
                if (a._items.length > a._selectedIndex) {
                    if (item.getFullPath().indexOf('HPVR://') === -1) {
                        switch (repeatOption.repeatOption) {
                            case EREPEAT_ALL:
                                if (a._selectedIndex === 0) {
                                    a._selectedIndex = prevIndex = a._items.length - 1;
                                    prevPlayItem = a._items[prevIndex];
                                } else {
                                    prevIndex = a._selectedIndex -= 1;
                                    prevPlayItem = a._items[prevIndex];
                                }
                                break;
                            case EREPEAT_NO:
                                if (a._selectedIndex > 0) {
                                    prevIndex = a._selectedIndex -= 1;
                                    prevPlayItem = a._items[prevIndex];
                                }
                                break;
                            case EREPEAT_ONE:
                                prevIndex = a._selectedIndex;
                                prevPlayItem = a._items[prevIndex];
                                break;
                        }
                    } else {
                        if (seriesRepeat === true) {
                            prevIndex = a._selectedIndex -= 1;
                            prevPlayItem = a._items[prevIndex];
                        } else {
                            prevPlayItem = null;
                        }
                    }
                }
            }
            return prevPlayItem;
        }
        getNextPlayItem(item?) {
            var a = this;
            var nextIndex = null;
            var nextPlayItem = null;
            var repeatOption = a.doGetVideoOption();
            var seriesRepeat = a.doGetSeriesRepeat();
            if (a._items.length === undefined) {
                nextPlayItem = null;
            } else {
                if (a._items.length > a._selectedIndex) {
                    if (item.getFullPath().indexOf('HPVR://') === -1) {
                        switch (repeatOption.repeatOption) {
                            case EREPEAT_ALL:
                                if (a._items.length === a._selectedIndex + 1) {
                                    a._selectedIndex = nextIndex = 0;
                                    nextPlayItem = a._items[nextIndex];
                                } else {
                                    nextIndex = a._selectedIndex += 1;
                                    nextPlayItem = a._items[nextIndex];
                                }
                                break;
                            case EREPEAT_NO:
                                nextIndex = a._selectedIndex += 1;
                                nextPlayItem = a._items[nextIndex];
                                break;
                            case EREPEAT_ONE:
                                nextIndex = a._selectedIndex;
                                nextPlayItem = a._items[nextIndex];
                                break;
                        }
                    } else {
                        if (seriesRepeat === true) {
                            nextIndex = a._selectedIndex += 1;
                            nextPlayItem = a._items[nextIndex];
                        } else {
                            nextPlayItem = null;
                        }
                    }
                }
            }
            return nextPlayItem;
        }
        addDivisionPoint(programmes) {
            var a = this;
            a._videoPlayer.addDivisionPoint(programmes);
        }
        changeToLive() {
            this._videoPlayer.changeToLive();
        }
        tsrPBStarted() {
            this._videoPlayer.tsrPBStarted();
        }
        doKeyBack() {
            var a = this;
            var shown = a.isShown();
            var consumed = true;
            if (shown) {
                a.hide();
            } else {
                if (this._videoPlayer instanceof prism.CTsrPlayerControl) {
                    consumed = false;
                } else {
                    a.close();
                }
            }
            return consumed;
        }
        doKeyEscape() {
            var a = this;
            var shown = a.isShown();
            var consumed = true;
            if (shown) {
                a.hide();
            } else {
                if (!(this._videoPlayer instanceof prism.CTsrPlayerControl)) {
                    a.close();
                }
                consumed = false;
            }
            return consumed;
        }
        doKeyStop() {
            var a = this;
            a.close();
            this._fireEvent("Hide");
            return true;
        }
        doKey(keyCode?) {
            var a = this;
            a._keyPressed = true;
            var consumed = false;
            if (a.isShown()) {
                a.resetHidingTimer();
                switch (keyCode) {
                    case CONTROLS_CONFIG.KEY_UP:
                    case CONTROLS_CONFIG.KEY_DOWN:
                    case CONTROLS_CONFIG.KEY_LEFT:
                    case CONTROLS_CONFIG.KEY_RIGHT:
                    case CONTROLS_CONFIG.KEY_OK:
                        CVerticalGroupControl.prototype.doKey.call(a, keyCode);
                        consumed = true;
                        break;
                    case CONTROLS_CONFIG.KEY_PAGEUP:
                    case CONTROLS_CONFIG.KEY_PAGEDOWN:
                        break;
                    default:
                        consumed = CVerticalGroupControl.prototype.doKey.call(a, keyCode);
                        break;
                }
                if (keyCode === CONTROLS_CONFIG.KEY_PLAY) {
                    a._videoPlayer.resetFocus('pause');
                }
            } else {
                if (a.isKeyEnabled()) {
                    a.resetHidingTimer();
                    switch (keyCode) {
                        case CONTROLS_CONFIG.KEY_LEFT:
                        case CONTROLS_CONFIG.KEY_RIGHT:
                            if (!(this._videoPlayer instanceof prism.CTsrPlayerControl)) {
                                a.doShow();
                                consumed = true;
                            }
                            break;
                        case CONTROLS_CONFIG.KEY_PLAY:
                            a._videoPlayer.doNavi('u');
                        case CONTROLS_CONFIG.KEY_PAUSE:
                        case CONTROLS_CONFIG.KEY_STOP:
                        case CONTROLS_CONFIG.KEY_REWIND:
                        case CONTROLS_CONFIG.KEY_FAST_FWD:
                        case CONTROLS_CONFIG.KEY_PLAY_SPEED_UP:
                        case CONTROLS_CONFIG.KEY_PLAY_SPEED_DOWN:
                        case CONTROLS_CONFIG.KEY_BACK:
                        case CONTROLS_CONFIG.KEY_ESCAPE:
                            consumed = CVerticalGroupControl.prototype.doKey.call(a, keyCode);
                            break;
                    }
                }
            }
            if (keyCode === CONTROLS_CONFIG.KEY_PAUSE) {
                a._videoPlayer.resetFocus('pause');
            }
            return consumed;
        }
        doShow() {
            var a = this;
            var shown = a.isShown();
            if (!shown) {
                if (this._videoPlayer instanceof prism.CTsrPlayerControl) {
                    hx.svc.TSR.clearTimer();
                    hx.svc.TSR.setTimer(1000);
                }
                var $layer = a._$self.parent();
                $layer.css('-o-transition', 'top .3s linear');
                $layer.css('top', a._$self.height() + 'px');
                $layer.anim({
                    'top': '0px'
                });
                a._videoPlayer.showTitle();
            }
            this._fireEvent("Show");
            a.resetHidingTimer();
        }
        doHide() {
            var a = this;
            var shown = a.isShown();
            if (shown) {
                var $layer = a._$self.parent();
                $layer.anim({
                    'top': a._$self.height() + 'px'
                });
                if (this._videoPlayer instanceof prism.CTsrPlayerControl) {
                    hx.svc.TSR.clearTimer();
                    hx.svc.TSR.setTimer(30000);
                }
            }
            this._fireEvent("Hide");
        }
        resetHidingTimer(aTimeout) {
            var a = this;
            clearTimeout(mHidingTimer);
            mHidingTimer = setTimeout(function () {
                if (a.requestGetSpeed() !== 0) {
                    mRequestedSeekPosition = null;
                    a.doHide();
                }
            }, aTimeout || TIMEOUT_PLAYBAR);
        }
        isKeyEnabled() {
            var i, $page, keyEnabled = true;
            var $pages = $('#wrap').find('> .show').not('#main, #-test-infoHub');
            if ($pages.length > 0) {
                keyEnabled = false;
            }
            return keyEnabled;
        }
        isSeekAvailable(trickRestrictMode) {
            return ((trickRestrictMode === EDRM_TRICK_NORESTRICT) || (trickRestrictMode === null)) || false;
        }
        isTrickAvailable(currentSpeed, trickRestrictMode) {
            var bAvailable = true;
            switch (trickRestrictMode) {
                case EDRM_TRICK_NORESTRICT:
                    break;
                case EDRM_TRICK_NOSKIP_FF2X:
                    bAvailable = (currentSpeed < 2) || false;
                    break;
                case EDRM_TRICK_NOSKIP_FF4X:
                    bAvailable = (currentSpeed < 4) || false;
                    break;
                case EDRM_TRICK_NOSKIP_FF8X:
                    bAvailable = (currentSpeed < 8) || false;
                    break;
                case EDRM_TRICK_NOSKIP_NOFF:
                    bAvailable = false;
                    break;
            }
            return bAvailable;
        }
        createTrickRestrictMsg() {
            var a = this;
            var notificationDlg = prism.createNotificationDlg(a._parent);
            notificationDlg.setTextHtml(tr('LOC_TRICK_LIMIT_ID'));
            notificationDlg.setTimeout(3000);
            notificationDlg.open();
        }

        isFastPlayAvailable () {
            var a = this;
            var curObj = undefined;
            var ulChannelType = 0;

            if (a._playItem === undefined) {
                //type : (channel.channelType === 0) ? 'TV' : 'RADIO'
                curObj = hx.svc.CH_LIST.getCurrentChannel();
                if (curObj) {
                    ulChannelType = curObj.channel.channelType;
                } else {
                    ulChannelType = 0;
                }

                if (a._item.isChase() === true) {
                    // Chase play
                    if (a._item._recording === undefined) {
                        if (ulChannelType === 1) {
                            return false;
                        }
                    } else {
                        if (a._item._recording.getChannelType() === 1) {
                            return false;
                        }
                    }
                } else {
                    // TSR PlayBack
                    if (ulChannelType === 1) {
                        return false;
                    }
                }
            } else {
                // Recording playback
                if (a._playItem.isRadio) {
                    if (a._playItem.isRadio() === true) {
                        return false;
                    }
                }
            }
            return	true;
        }

        createFastPlayRestrictMsg () {
            var a = this;

            a.onOpenNotAvailableMsg();
        }
        createSeekRestrictMsg() {
            var a = this;
            var btnY = tr('LOC_YES_ID');
            var btnN = tr('LOC_NO_ID');
            var textHtml = tr('LOC_SEEK_NOTALLOWED_DRM_ID');
            var callbackfn = function (index) {
                if (index === 0) {
                    // play from the beginning of this programme
                }
            };
            a.hide();
            var btnDlg = prism.createButtonDlg(a._parent, {
                textHtml: textHtml,
                buttonList: [btnY, btnN],
                timeOutValue: 30000
            }, callbackfn);
            btnDlg.open();
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createVideoPlaybackDlg = function (layeredGroupControl) {
        var videoPlayback = new CVideoPlaybackDlg(layeredGroupControl);
        return videoPlayback;
    };
}
export = mPrism;
