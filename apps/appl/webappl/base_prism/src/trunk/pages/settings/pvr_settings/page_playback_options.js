var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    // file name : pages/settings/pvr_settings/playback_options.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgPlaybackOptions = (function (_super) {
        __extends(cCPgPlaybackOptions, _super);
        function cCPgPlaybackOptions() {
            _super.call(this);
            this.TMenuItemValue_OnOff = {
                VALUE_ON: 0,
                VALUE_OFF: 1
            };
            this._seriesPlay = true;
            this._instantReplayTime = 15;
            this._skipForwardTime = 120;
            this._isChangePageInSetting = false;
        }
        cCPgPlaybackOptions.prototype.create = function () {
            var that = this;
            _super.prototype.create.call(this);
            $.extend(that, {
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _$contextMenu: $('<div>', {
                    'class': 'dep2'
                }),
                _playbackOptionMainMenu: hx.al.CPgPlaybackOptions.playbackOptionMainMenu(that._contextListMap.itemStr, that._contextListMap.describe, 'dep1')
            });
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l(that._contextListMap.title);
            }
            that._createItem();
            $('#set').append(that._$menu.append(that._$contextMenu.after(that._playbackOptionMainMenu.$)));
            hx.al.compose(that);
            $title.ani({
                style: that.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            that._$menu.ani({
                style: that.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    that.append(that._playbackOptionMainMenu);
                    that._playbackOptionMainMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        cCPgPlaybackOptions.prototype._createItem = function () {
            var i = 0, szItemId = 'undefined';
            for (i = 0; i < this._contextListMap.itemId.length; i += 1) {
                szItemId = this._conv_index_to_itemID(i);
                if (szItemId === 'seriesplay') {
                    this._display_seriesPlay(i);
                }
                if (szItemId === 'instantreplaytime') {
                    this._display_instantReplayTime(i);
                }
                if (szItemId === 'skipforwardtime') {
                    this._display_skipForwardTime(i);
                }
            }
        };
        cCPgPlaybackOptions.prototype._display_seriesPlay = function (aParam) {
            var ret = 0, $DrawItem;
            this._seriesPlay = hx.svc.SETTING_UTIL.getEnableSeriesPlayback();
            ret = (this._seriesPlay === false) ? this.TMenuItemValue_OnOff.VALUE_OFF : this.TMenuItemValue_OnOff.VALUE_ON;
            $DrawItem = this._playbackOptionMainMenu.getItem(aParam).find('em');
            $DrawItem.html(hx.l(this._contextListMap.itemSub01Str[ret]));
        };
        cCPgPlaybackOptions.prototype._display_instantReplayTime = function (aParam) {
            var ret = 0, $DrawItem;
            this._instantReplayTime = hx.svc.SETTING_UTIL.getInstantReplayTime();
            ret = this._conv_instReplaytime_to_index(this._instantReplayTime);
            $DrawItem = this._playbackOptionMainMenu.getItem(aParam).find('em');
            $DrawItem.html(hx.l(this._contextListMap.itemSub02Str[ret]));
        };
        cCPgPlaybackOptions.prototype._display_skipForwardTime = function (aParam) {
            var ret = 0, $DrawItem;
            this._skipForwardTime = hx.svc.SETTING_UTIL.getSkipForwardTime();
            ret = this._conv_SkipForwardtime_to_index(this._skipForwardTime);
            $DrawItem = this._playbackOptionMainMenu.getItem(aParam).find('em');
            $DrawItem.html(hx.l(this._contextListMap.itemSub03Str[ret]));
        };
        cCPgPlaybackOptions.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            this._$menu.remove();
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
            this._$menu = undefined;
            this._$contextMenu = undefined;
            this._playbackOptionMainMenu = undefined;
        };
        cCPgPlaybackOptions.prototype._conv_instReplaytime_to_index = function (aTime) {
            var index;
            switch (aTime) {
                case 15:
                    index = 1;
                    break;
                case 30:
                    index = 2;
                    break;
                case 7:
                    index = 0;
                    break;
                default:
                    index = 0;
                    break;
            }
            return index;
        };
        cCPgPlaybackOptions.prototype._conv_index_to_instReplaytime = function (aIndex) {
            var time;
            switch (aIndex) {
                case 1:
                    time = 15;
                    break;
                case 2:
                    time = 30;
                    break;
                case 0:
                    time = 7;
                    break;
                default:
                    time = 7;
                    break;
            }
            return time;
        };
        cCPgPlaybackOptions.prototype._conv_SkipForwardtime_to_index = function (aTime) {
            var index;
            switch (aTime) {
                case 60:
                    index = 1;
                    break;
                case 120:
                    index = 2;
                    break;
                case 240:
                    index = 3;
                    break;
                case 30:
                    index = 0;
                    break;
                default:
                    index = 0;
                    break;
            }
            return index;
        };
        cCPgPlaybackOptions.prototype._conv_index_to_SkipForwardtime = function (aIndex) {
            var time;
            switch (aIndex) {
                case 1:
                    time = 60;
                    break;
                case 2:
                    time = 120;
                    break;
                case 3:
                    time = 240;
                    break;
                case 0:
                    time = 30;
                    break;
                default:
                    time = 30;
                    break;
            }
            return time;
        };
        cCPgPlaybackOptions.prototype._on_dlSettingUtil_getEnableSeriesPlayback = function (aParam) {
            var ulOnOff;
            switch (aParam.alParam) {
                case true:
                    ulOnOff = this.TMenuItemValue_OnOff.VALUE_ON;
                    break;
                case false:
                    ulOnOff = this.TMenuItemValue_OnOff.VALUE_OFF;
                    break;
            }
            var ulItemIdx = this._conv_itemID_to_index('seriesplay');
            var $bSeriesPlay = this._playbackOptionMainMenu.getItem(ulItemIdx).find('em');
            $bSeriesPlay.html(hx.l(this._contextListMap.itemSub01Str[ulOnOff]));
        };
        cCPgPlaybackOptions.prototype._on_alClicked = function (aParam) {
            var that = this, data = aParam.alData, target = data.me, bConsumed = false;
            var szItemId = 'undefined';
            switch (target) {
                case that._playbackOptionMainMenu:
                    bConsumed = true;
                    that._playbackOptionMainMenu.off();
                    that._makesubList(data.index);
                    that._currentSubMenu = data.index;
                    break;
                case that._subList:
                    bConsumed = true;
                    that._subList.off();
                    that._$contextMenu.ani({
                        style: that.aniConfig.dep2.fadeOut,
                        trans: {
                            dur: 150,
                            delay: 75
                        },
                        cb: function () {
                            that._$contextMenu.removeClass('active');
                            szItemId = that._conv_index_to_itemID(that._currentSubMenu);
                            if (szItemId === 'seriesplay') {
                                that._setSeriesPlay(data.index);
                            }
                            else if (szItemId === 'instantreplaytime') {
                                that._setInstantReplayTime(data.index);
                            }
                            else if (szItemId === 'skipforwardtime') {
                                that._setSkipForwardTime(data.index);
                            }
                        }
                    });
                    break;
            }
            return bConsumed;
        };
        cCPgPlaybackOptions.prototype._on_alExit = function (aParam) {
            var that = this, bConsumed = false;
            if (aParam.alData) {
                if (aParam.alData.me === this._playbackOptionMainMenu) {
                    bConsumed = true;
                    that._isChangePageInSetting = true;
                    hx.al.decompose(that);
                    that._$menu.ani({
                        style: that.aniConfig.menu.changeOut3,
                        trans: {
                            dur: 200,
                            delay: 100
                        },
                        cb: function () {
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgSettings',
                                me: that
                            });
                        }
                    });
                }
            }
            return bConsumed;
        };
        cCPgPlaybackOptions.prototype._on_alKey = function (aParam) {
            var that = this, bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_ESCAPE:
                    if (aParam.alData) {
                        bConsumed = true;
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: ''
                        });
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: that
                        });
                    }
                    break;
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    if (aParam.alData) {
                        if (aParam.alData.me === that._subList) {
                            bConsumed = true;
                            var menuFocusIndex = that._currentSubMenu;
                            that._subList.off();
                            that._$contextMenu.ani({
                                style: that.aniConfig.dep2.fadeOut,
                                trans: {
                                    dur: 150,
                                    delay: 75
                                },
                                cb: function () {
                                    that._subList.$.remove();
                                    that._$contextMenu.removeClass('active');
                                    that._playbackOptionMainMenu.getItem(menuFocusIndex).removeClass('his');
                                    that._playbackOptionMainMenu.setItemFocus(menuFocusIndex);
                                    that._playbackOptionMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgPlaybackOptions.prototype._conv_itemID_to_index = function (aItemID) {
            var i = 0, ulIndex = -1;
            for (i = 0; i < this._contextListMap.itemId.length; i += 1) {
                if (aItemID === this._contextListMap.itemId[i]) {
                    ulIndex = i;
                    return ulIndex;
                }
            }
            return ulIndex;
        };
        cCPgPlaybackOptions.prototype._conv_index_to_itemID = function (aItemIdx) {
            var i = 0, itemID = 'undefined';
            for (i = 0; i < this._contextListMap.itemId.length; i += 1) {
                if (i === aItemIdx) {
                    itemID = this._contextListMap.itemId[i];
                    return itemID;
                }
            }
            return itemID;
        };
        cCPgPlaybackOptions.prototype._makesubList = function (aParam) {
            var that = this, index, subFocusIndex, szItemId = 'undefined', dataList;
            szItemId = that._contextListMap.itemId[aParam];
            if (szItemId === 'seriesplay') {
                dataList = that._contextListMap.itemSub01Str;
                subFocusIndex = (that._seriesPlay === true) ? that.TMenuItemValue_OnOff.VALUE_ON : that.TMenuItemValue_OnOff.VALUE_OFF;
            }
            else if (szItemId === 'instantreplaytime') {
                dataList = that._contextListMap.itemSub02Str;
                subFocusIndex = that._conv_instReplaytime_to_index(that._instantReplayTime);
            }
            else if (szItemId === 'skipforwardtime') {
                dataList = that._contextListMap.itemSub03Str;
                subFocusIndex = that._conv_SkipForwardtime_to_index(that._skipForwardTime);
            }
            that._subList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length
            });
            that._$contextMenu.addClass('active');
            that._playbackOptionMainMenu.getItem(aParam).addClass('his');
            that._$contextMenu.append(that._subList.$);
            that._subList.bind(dataList);
            that._subList.draw();
            that._$contextMenu.ani({
                style: that.aniConfig.dep2.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    that.append(that._subList);
                    that._subList.on();
                }
            });
        };
        cCPgPlaybackOptions.prototype._setSeriesPlay = function (aIndex) {
            var ulItemIdx = this._conv_itemID_to_index('seriesplay');
            var $seriesPlaySub = this._playbackOptionMainMenu.getItem(ulItemIdx).find('em');
            var seriesPlayKey = hx.l(this._contextListMap.itemSub01Str[aIndex]);
            $seriesPlaySub.html(seriesPlayKey);
            this._seriesPlay = (aIndex === 0) ? true : false;
            hx.svc.SETTING_UTIL.setEnableSeriesPlayback(this._seriesPlay);
            this._playbackOptionMainMenu.getItem(ulItemIdx).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._playbackOptionMainMenu.on();
            this._playbackOptionMainMenu.setItemFocus(ulItemIdx);
        };
        cCPgPlaybackOptions.prototype._setInstantReplayTime = function (aIndex) {
            var ulItemIdx = this._conv_itemID_to_index('instantreplaytime');
            var $instantReplayTimeSub = this._playbackOptionMainMenu.getItem(ulItemIdx).find('em');
            var instantReplayTimeKey = hx.l(this._contextListMap.itemSub02Str[aIndex]);
            $instantReplayTimeSub.html(instantReplayTimeKey);
            this._instantReplayTime = this._conv_index_to_instReplaytime(aIndex);
            hx.svc.SETTING_UTIL.setInstantReplayTime(this._instantReplayTime);
            this._playbackOptionMainMenu.getItem(ulItemIdx).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._playbackOptionMainMenu.on();
            this._playbackOptionMainMenu.setItemFocus(ulItemIdx);
        };
        cCPgPlaybackOptions.prototype._setSkipForwardTime = function (aIndex) {
            var ulItemIdx = this._conv_itemID_to_index('skipforwardtime');
            var $skipForwardTimeSub = this._playbackOptionMainMenu.getItem(ulItemIdx).find('em');
            var skipForwardTimeKey = hx.l(this._contextListMap.itemSub03Str[aIndex]);
            $skipForwardTimeSub.html(skipForwardTimeKey);
            this._skipForwardTime = this._conv_index_to_SkipForwardtime(aIndex);
            hx.svc.SETTING_UTIL.setSkipForwardTime(this._skipForwardTime);
            this._playbackOptionMainMenu.getItem(ulItemIdx).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._playbackOptionMainMenu.on();
            this._playbackOptionMainMenu.setItemFocus(ulItemIdx);
        };
        return cCPgPlaybackOptions;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgPlaybackOptions = cCPgPlaybackOptions;
    if (hx.config.settings.playbackoption) {
        hx.al.CPgPlaybackOptions.prototype._contextListMap = hx.config.settings.playbackoption;
    }
    else {
        hx.al.CPgPlaybackOptions.prototype._contextListMap = {
            title: ['LOC_PLAYBACK_OPTIONS_ID'],
            itemId: ['seriesplay', 'instantreplaytime', 'skipforwardtime'],
            itemStr: ['LOC_SERIESPLAY_ID', 'LOC_INSTANT_REPLAY_TIME_ID', 'LOC_SKIP_FORWARD_TIME_ID'],
            describe: ['', '', ''],
            itemSub01Str: ['LOC_ON_ID', 'LOC_OFF_ID'],
            itemSub02Str: ['LOC_7_SEC_ID', 'LOC_15_SEC_ID', 'LOC_30_SEC_ID'],
            itemSub03Str: ['LOC_30_SEC_ID', 'LOC_60_SEC_ID', 'LOC_120_SEC_ID', 'LOC_240_SEC_ID']
        };
    }
    hx.al.CPgPlaybackOptions.playbackOptionMainMenu = function (aParam, aParam2, aParam3) {
        var playbackOptionmainmenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': aParam3
            },
            data: aParam,
            data2: aParam2,
            count: aParam.length,
            fx: function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>').html(this.data2[i]));
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return playbackOptionmainmenu;
    };
    hx.al.CPgPlaybackOptions.playbackOptionSubMenu = function (aParam) {
        var playbackOptionsubmenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': 'dep2'
            },
            data: aParam,
            count: aParam.length,
            fx: function (i, o) {
                o.html(hx.l(this.data[i]));
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_BACK]
        });
        return playbackOptionsubmenu;
    };
    hx.al.CPgPlaybackOptions.prototype.aniConfig = {
        fadeIn: {
            start: {
                'opacity': 0
            },
            end: {
                'opacity': 1
            }
        },
        menu: {
            changeIn: {
                start: {
                    'opacity': 0,
                    'left': '20px'
                },
                end: {
                    'opacity': 1,
                    'left': 0
                }
            },
            changeOut3: {
                start: {
                    'opacity': 1,
                    'left': 0
                },
                end: {
                    'opacity': 0,
                    'left': '20px'
                }
            }
        },
        dep2: {
            fadeIn: {
                start: {
                    'opacity': 0,
                    'left': '795px'
                },
                end: {
                    'opacity': 1,
                    'left': '775px'
                }
            },
            fadeOut: {
                start: {
                    'opacity': 1,
                    'left': '775px'
                },
                end: {
                    'opacity': 0,
                    'left': '795px'
                }
            }
        }
    };
    return cCPgPlaybackOptions;
});
