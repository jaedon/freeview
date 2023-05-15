/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.musicplayback.js
/*jslint sloppy:true, nomen:true, vars:true */

import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CPLAYBACKDLG__ = require('prismux/prism.dlg.playback');

module mPrism {
    var EDIRECTION_LEFT = 0;
    var EDIRECTION_RIGHT = 1;
    var EREPEAT_ALL = 0;
    var EREPEAT_NO = 1;
    var EREPEAT_ONE = 2;
    var ESHUFFLE_OFF = 0;
    var ESHUFFLE_ON = 1;
    var mDirection = EDIRECTION_RIGHT;
    var mMusicOption;
    export class CMusicPlaybackDlg extends __PRISM_CPLAYBACKDLG__.CPlaybackDlg {
        _selectedIndex: number;
        _originalItems: any[];
        _items: any[];
        _musicSlider: any;
        _musicPlayer: any;
        _curItem: any;
        constructor(layeredGroupControl: __CONTROLS__.CLayeredGroupControl) {
            super();
            var a = this;
            a.init(layeredGroupControl);
            a.addClass('musicPlayback');
            a._selectedIndex = 0;
        }
        _rearrangeData(playMode) {
            var a = this;
            var curItem = a._originalItems[a._selectedIndex];
            var rearrange = function(item) {
                var oldItems = a._originalItems;
                var i, length = oldItems.length;
                for ( i = 0; i < length; i += 1) {
                    if (item.getFullPath() === oldItems[i].getFullPath()) {
                        a._selectedIndex = i;
                        break;
                    }
                }
                var index = a._selectedIndex;
                var newItems = [];
                for ( i = 0; i < length; i += 1) {
                    if (index > length - 1) {
                        index = 0;
                    }
                    newItems.push(oldItems[index]);
                    index += 1;
                }
                return newItems;
            };
            a._items = a._originalItems;
            if (Number(playMode) === ESHUFFLE_ON) {
                a._items.sort(function() {
                    return (Math.round(Math.random()) - 0.5);
                });
            }
            if (a._musicSlider) {
                curItem = a._musicSlider._rollDataProvider.curItem();
            }
            if (a._musicPlayer) {
                var playItem = a._musicPlayer.getPlayItem();
                if (playItem) {
                    curItem = playItem;
                }
            }
            a._items = rearrange(curItem);
        }
        setData(items) {
            this._originalItems = items;
            this._items = items;
        }
        setStartIndex(startIndex) {
            this._selectedIndex = startIndex;
        }
        doOpen() {
            var a = this;
            var i, length;
            var initialFocus = 1;
            var childControl = [];
            var musicSlider = new CCarouselControl(null);
            var musicPlayer = prism.createMusicPlayer();
            var operationDisabled = false;
            mMusicOption = a.doGetMusicOption();
            musicPlayer.setRepeatOption(mMusicOption.repeatOption);
            musicPlayer.setPlayMode(mMusicOption.playMode);
            if (a._items && a._items.length) {
                operationDisabled = !a._items[0].isOperationEnabled();
                musicPlayer.disableOperation(operationDisabled);
                if (operationDisabled) {
                    a.addClass('disabled');
                    a.doKeyOk = function() {
                        return true;
                    };
                }
            }
            a._rearrangeData(mMusicOption.playMode);
            musicSlider.addClass('musicSlider');
            musicSlider.setData(a._items);
            // musicSlider.setCurrentIndex(a._selectedIndex);
            musicSlider.setDrawParam({
                viewCount : 7,
                anchorIndex : 3,
                itemWidth : 166,
                itemHeight : 100,
                anchorWidth : 166,
                transparentAnchor : false,
                align : 'horizontal',
                animation : true,
                animationInterval : 0.5,
                maxKeyQueueCount : 0
            });
            musicSlider.setAnchorDrawer(function(param) {
                var anchorItem = param.currentItem;
                param.$anchor.addClass('muAnchorItem');
                param.$anchor.html(anchorItem);
            });
            musicSlider.setDataDrawer(function(param) {
                var $item = param.$item;
                var item = param.item;
                var src = item.getThumbnailUrl();
                var $box = $('<figure>');
                var $img = $('<img>', {
                    'src' : src,
                    'style' : 'display:none;'
                });
                if (!item.isOperationEnabled()) {
                    $item.removeClass('-f');
                }
                $img[0].onload = function() {
                    $img.fadeIn(500);
                };
                $box.append($img).appendTo($item);
            });
            musicSlider.onFocusStartToChange = function(param) {
                var $self = musicSlider._$self;
                if (mDirection === EDIRECTION_LEFT) {
                    $self.find('.-carousel-dist3.-r').fadeOut(500);
                } else {
                    $self.find('.-carousel-dist3.-l').fadeOut(500);
                }
            };
            musicSlider.onFocusChanged = function(param) {
                var item = param.item;
                var title = item.getTitle();
                param.$anchor.html('<div class="title">' + title + '</div>');
            };
            musicSlider.onItemSelected = function(param) {
                var item = param.item;
                if (item.isOperationEnabled()) {
                    a.play(item);
                }
            };
            musicSlider.doKeyOption = function() {
                if (a._items && a._items.length > 0) {
                    if (!a._items[0].isOperationEnabled()) {
                        return;
                    }
                }
                mMusicOption = a.doGetMusicOption();
                var settingDlg = prism.createSettingDlg(a._parent);
                settingDlg.setDrawParam('width', 700);
                settingDlg.setTitleHtml(tr('LOC_SEL_OPTION_VAL_ID'));
                settingDlg.setSettingListItems([{
                    type : 'multiSelection',
                    text : tr('LOC_REPEAT_OPTION_ID'),
                    config : {
                        selectionList : [{
                            text : tr('LOC_REPEAT_ALL_ID'),
                            value : 0
                        }, {
                            text : tr('LOC_ONCE_ID'),
                            value : 1
                        }, {
                            text : tr('LOC_REPEAT_1_ID'),
                            value : 2
                        }],
                        activeIndex : mMusicOption.repeatOption
                    }
                }, {
                    type : 'multiSelection',
                    text : tr('LOC_PLAY_MODE_ID'),
                    config : {
                        selectionList : [{
                            text : tr('LOC_NORMAL_ID'),
                            value : 0
                        }, {
                            text : tr('LOC_SHUFFLE_ID'),
                            value : 1
                        }],
                        activeIndex : mMusicOption.playMode
                    }
                }]);
                settingDlg.onItemChanged = function(index, settingItem, changedValue) {
                };
                settingDlg.onOk = function(changedInfoList) {
                    var i, length, optionIndex, optionValue, changedInfo, option: any = {};
                    var options = ['repeatOption', 'playMode'];
                    mMusicOption = a.doGetMusicOption();
                    option.repeatOption = mMusicOption.repeatOption !== undefined ? mMusicOption.repeatOption : 0;
                    option.playMode = mMusicOption.playMode !== undefined ? mMusicOption.playMode : 0;
                    length = changedInfoList.length;
                    for ( i = 0; i < length; i += 1) {
                        changedInfo = changedInfoList[i];
                        optionIndex = changedInfo.index;
                        optionValue = changedInfo.settingItem.config.selectionList[changedInfo.changedValue].value;
                        if (optionValue !== undefined) {
                            option[options[optionIndex]] = optionValue;
                            mMusicOption[options[optionIndex]] = optionValue;
                        }
                    }
                    a.doSetMusicOption(option);
                    a._musicPlayer.setRepeatOption(option.repeatOption);
                    a._musicPlayer.setPlayMode(option.playMode);
                    a._rearrangeData(option.playMode);
                    a._musicSlider.setData(a._items);
                    a._musicSlider.draw();
                };
                settingDlg.doKeyEscape = function() {
                    return settingDlg.doKeyBack();
                };
                settingDlg.open({
                    transition : {
                        newLayer : 'fadeIn'
                    },
                    dimm : true
                });
                return true;
            };
            musicSlider.doKeyLeft = function() {
                mDirection = EDIRECTION_LEFT;
                CCarouselControl.prototype.doKeyLeft.call(this);
                return true;
            };
            musicSlider.doKeyRight = function() {
                mDirection = EDIRECTION_RIGHT;
                CCarouselControl.prototype.doKeyRight.call(this);
                return true;
            };
            musicPlayer.doKeyOption = function() {
                return musicSlider.doKeyOption();
            };
            musicPlayer.addEventHandler('ResumeButtonSelected', musicPlayer, function() {
                a.resume();
            });
            musicPlayer.addEventHandler('PauseButtonSelected', musicPlayer, function() {
                a.pause();
            });
            musicPlayer.addEventHandler('StopButtonSelected', musicPlayer, function() {
                a.stop();
            });
            musicPlayer.addEventHandler('PrevButtonSelected', musicPlayer, function() {
                var repeatOption = Number(mMusicOption.repeatOption);
                if(repeatOption === EREPEAT_ONE) {
                    a.play(a._curItem);
                } else {
                    if (repeatOption === EREPEAT_NO) {
                        if (musicSlider.getCurrentIndex() === 0) {
                            a.close();
                            return;
                        }
                    }
                    musicSlider.doKeyLeft();
                    this._curItem = musicSlider._rollDataProvider.curItem();
                    a.play(this._curItem);
                }
            });
            musicPlayer.addEventHandler('NextButtonSelected', musicPlayer, function() {
                var repeatOption = Number(mMusicOption.repeatOption);
                if(repeatOption === EREPEAT_ONE) {
                    a.play(a._curItem);
                } else {
                    if (repeatOption === EREPEAT_NO) {
                        if (musicSlider.getCurrentIndex() === musicSlider._rollDataProvider.length() - 1) {
                            a.close();
                            return;
                        }
                    }
                    musicSlider.doKeyRight();
                    this._curItem = musicSlider._rollDataProvider.curItem();
                    a.play(this._curItem);
                }
            });
            var optionMenu = new CHorizontalLayoutControl(null);
            optionMenu.setItemDrawers([
                function(index, $item) {
                    var $optionMenuInd = $('<div>', {
                        'class' : 'optionMenuInd'
                    });
                    $optionMenuInd.text(tr('LOC_STR_PLAY_MODE_ID'));
                    $item.append($optionMenuInd);
                    $item.removeClass('-f');
                }]);
            childControl.push(musicSlider);
            childControl.push(musicPlayer);
            childControl.push(optionMenu);
            a.setChildControl(childControl, initialFocus);
            a.setActiveFocus(true);
            a._musicSlider = musicSlider;
            a._musicPlayer = musicPlayer;
        }
        doSetMusicOption(aParam) {
            throw "do Implement";
        }
        doGetMusicOption() {
            throw "do Implement";
            return false;
        }
        doKeyStop() {
            this.close();
            return true;
        }
        doKeyEscape() {
            this.close();
            return false;
        }
        doHide() {
            this.close();
        }
        doPlay(item) {
            var a = this;
            a._musicPlayer.play(item);
            return false;
        }
        doResume() {
            var a = this;
            a._musicPlayer.resume();
        }
        doPause() {
            var a = this;
            a._musicPlayer.pause();
        }
        doStop() {
            var a = this;
        }
        doNext() {
            var a = this;
            a._musicPlayer.next();
        }
        doPrev() {
            var a = this;
            a._musicPlayer.prev();
        }
        doFastForward() {
            var a = this;
            a._musicSlider.doKeyRight();
            a.play(a._musicSlider.getCurrentItem());
            return true;
        }
        doFastRewind() {
            var a = this;
            a._musicSlider.doKeyLeft();
            a.play(a._musicSlider.getCurrentItem());
            return true;
        }
        setSpeed(speed) {
            var a = this;
            a._musicPlayer.setSpeed(speed);
        }
        setPosition(position) {
            var a = this;
            a._musicPlayer.setStartTime(position);
            a._musicPlayer.setPosition(position);
        }
        getPrevPlayItem(item?) {
            var a = this;
            var finished = false;
            var repeatOption = Number(mMusicOption.repeatOption);
            var slider = a._musicSlider;
            var provider = slider._rollDataProvider;
            var cur = 0;
            switch (repeatOption) {
                case EREPEAT_ALL:
                    slider.doKeyLeft();
                    this._curItem = provider.curItem();
                    break;
                case EREPEAT_NO:
                    if (slider.getCurrentIndex() === 0) {
                        this._curItem = null;
                        break;
                    }
                    slider.doKeyLeft();
                    cur = provider.cur();
                    if (cur !== (provider.length() - 1)) {
                        this._curItem = provider.curItem();
                    }
                    break;
                case EREPEAT_ONE:
                    this._curItem = item;
                    break;
            }
            return this._curItem;
        }
        getNextPlayItem(item?) {
            var a = this;
            var finished = false;
            var repeatOption = Number(mMusicOption.repeatOption);
            var slider = a._musicSlider;
            var provider = slider._rollDataProvider;
            var cur = 0;
            switch (repeatOption) {
                case EREPEAT_ALL:
                    slider.doKeyRight();
                    this._curItem = provider.curItem();
                    break;
                case EREPEAT_NO:
                    if (slider.getCurrentIndex() === provider.length() - 1) {
                        this._curItem = null;
                        break;
                    }
                    slider.doKeyRight();
                    cur = provider.cur();
                    if (cur !== 0) {
                        this._curItem = provider.curItem();
                    }
                    break;
                case EREPEAT_ONE:
                    this._curItem = item;
                    break;
            }
            return this._curItem;
        }

        audioRestrict() {
            var a = this;
            a._musicPlayer.audioRestrict();
        }
        updateMusicInfo(title, album, artist) {
            var a = this;
            $('.musicSlider .muAnchorItem .title').text(title);
        }
        updateAlbumImg(path) {
            var a = this;
            var $img = $('.musicSlider .-carousel-dist0').find('img');
            if ($img.length) {
                $img[0].onload = function() {
                    $img.fadeIn(500);
                };
                $img.attr('src', path);
            }
        }
        isFastPlayAvailable() {
            return true;
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createMusicPlaybackDlg = function(layeredGroupControl) {
        var musicPlayback = new CMusicPlaybackDlg(layeredGroupControl);
        return musicPlayback;
    };
}
export = mPrism;