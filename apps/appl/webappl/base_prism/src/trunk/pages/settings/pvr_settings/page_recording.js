var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    // file name : pages/settings/pvr_settings/recording.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgRecording = (function (_super) {
        __extends(cCPgRecording, _super);
        function cCPgRecording() {
            _super.call(this);
            this.TMenuItemValue_OnOff = {
                VALUE_ON: 0,
                VALUE_OFF: 1
            };
            this.TPaddingTimeList = {
                PTIMETVALUE: 0,
                PTIMESTRID: 1
            };
            this._startPadding = 0;
            this._endPadding = 0;
            this._isChangePageInSetting = false;
            this._tsronoff = false;
            this._descrambleonoff = false;
            // All the Padding Time definition with structure format.[Padding Time value,...][Padding Time String ID,....]
            this._paddingTimeListMap = [[0, 180, 300, 600, 1200], ['LOC_OFF_ID', 'LOC_3_MINS_ID', 'LOC_5_MINS_ID', 'LOC_10_MINS_ID', 'LOC_20_MINS_ID']];
        }
        cCPgRecording.prototype.create = function () {
            var that = this;
            _super.prototype.create.call(this);
            $.extend(that, {
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _$contextMenu: $('<div>', {
                    'class': 'dep2'
                }),
                _recordMainMenu: hx.al.CPgRecording.recordMainMenu(that._contextListMap.itemStr, that._contextListMap.describe, 'dep1'),
                _$noti: $('<p>', {
                    'class': 'noti',
                    'html': ''
                })
            });
            // Initial Draw..
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l(that._contextListMap.title);
            }
            that._createItem();
            $('#set').append(that._$menu.append(that._$contextMenu.after(that._recordMainMenu.$)).after(that._$noti));
            hx.al.compose(that);
            $title.ani({
                style: that.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            that._$noti.ani({
                style: that.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                }
            });
            that._$menu.ani({
                style: that.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    that.append(that._recordMainMenu);
                    that._recordMainMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        cCPgRecording.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            this._$menu.remove();
            this._$noti.remove();
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
            this._$menu = undefined;
            this._$contextMenu = undefined;
            this._recordMainMenu = undefined;
            this._$noti = undefined;
        };
        cCPgRecording.prototype._createItem = function () {
            var i = 0, szItemId = 'undefined';
            for (i = 0; i < this._contextListMap.itemId.length; i += 1) {
                szItemId = this._conv_index_to_itemID(i);
                if (szItemId === 'startpadding') {
                    this._display_startPadding(i);
                }
                if (szItemId === 'endpadding') {
                    this._display_endPadding(i);
                }
                if (szItemId === 'descramblestandby') {
                    this._display_descramblestandby(i);
                }
                if (szItemId === 'tsr') {
                    this._display_tsr(i);
                }
            }
        };
        cCPgRecording.prototype._display_startPadding = function (aParam) {
            var ulIndex = 0, $recordMenu;
            this._startPadding = hx.svc.SETTING_UTIL.getPvrStartPadding();
            $recordMenu = this._recordMainMenu.getItem(aParam).find('em');
            ulIndex = this._conv_padtime_to_index(this._startPadding);
            $recordMenu.html(hx.l(this._contextListMap.itemSub01Str[ulIndex]));
        };
        cCPgRecording.prototype._display_endPadding = function (aParam) {
            var ulIndex = 0, $recordMenu;
            this._endPadding = hx.svc.SETTING_UTIL.getPvrEndPadding();
            $recordMenu = this._recordMainMenu.getItem(aParam).find('em');
            ulIndex = this._conv_padtime_to_index(this._endPadding);
            $recordMenu.html(hx.l(this._contextListMap.itemSub02Str[ulIndex]));
        };
        cCPgRecording.prototype._display_descramblestandby = function (aParam) {
            var that = this;
            var ulIndex = 0, $recordMenu;
            that._descrambleonoff = hx.svc.SETTING_UTIL.getDescrambleOnOff();
            $recordMenu = that._recordMainMenu.getItem(aParam).find('em');
            ulIndex = (that._descrambleonoff === false) ? that.TMenuItemValue_OnOff.VALUE_OFF : that.TMenuItemValue_OnOff.VALUE_ON;
            $recordMenu.html(hx.l(that._contextListMap.itemSub03Str[ulIndex]));
        };
        cCPgRecording.prototype._display_tsr = function (aParam) {
            var ulIndex = 0, $recordMenu;
            this._tsronoff = hx.svc.SETTING_UTIL.getpvrTsrOnOff();
            $recordMenu = this._recordMainMenu.getItem(aParam).find('em');
            ulIndex = (this._tsronoff === false) ? this.TMenuItemValue_OnOff.VALUE_OFF : this.TMenuItemValue_OnOff.VALUE_ON;
            $recordMenu.html(hx.l(this._contextListMap.itemSub03Str[ulIndex]));
        };
        cCPgRecording.prototype._conv_padtime_to_index = function (aTime) {
            var that = this;
            var paddingTimeValueList = that._paddingTimeListMap[that.TPaddingTimeList.PTIMETVALUE];
            var paddingTimeStrIDList = that._paddingTimeListMap[that.TPaddingTimeList.PTIMESTRID];
            var supporpaddingTimeLists, DispText = 'LOC_OFF_ID', szItemId;
            var i;
            szItemId = that._conv_index_to_itemID(that._currentSubMenu);
            if (szItemId === 'startpadding') {
                supporpaddingTimeLists = that._contextListMap.itemSub01Str;
            }
            else if (szItemId === 'endpadding') {
                supporpaddingTimeLists = that._contextListMap.itemSub02Str;
            }
            else {
                supporpaddingTimeLists = that._contextListMap.itemSub01Str;
            }
            for (i = 0; i < paddingTimeValueList.length; i += 1) {
                if (aTime === paddingTimeValueList[i]) {
                    DispText = paddingTimeStrIDList[i];
                    break;
                }
            }
            var index = 0;
            for (i = 0; i < supporpaddingTimeLists.length; i += 1) {
                if (DispText === supporpaddingTimeLists[i]) {
                    index = i;
                    break;
                }
            }
            return index;
        };
        cCPgRecording.prototype._conv_index_to_padtime = function (aIndex) {
            var that = this;
            var paddingTimeValueList = that._paddingTimeListMap[that.TPaddingTimeList.PTIMETVALUE];
            var paddingTimeStrIDList = that._paddingTimeListMap[that.TPaddingTimeList.PTIMESTRID];
            var supporpaddingTimeLists, DispText = 'LOC_OFF_ID', szItemId = 'undefined';
            var i;
            var ulIndex = 0;
            var time = 0;
            szItemId = that._conv_index_to_itemID(that._currentSubMenu);
            if (szItemId === 'startpadding') {
                supporpaddingTimeLists = that._contextListMap.itemSub01Str;
            }
            else if (szItemId === 'endpadding') {
                supporpaddingTimeLists = that._contextListMap.itemSub02Str;
            }
            else {
                supporpaddingTimeLists = that._contextListMap.itemSub01Str;
            }
            if (aIndex >= supporpaddingTimeLists.length) {
                ulIndex = 0;
            }
            else {
                ulIndex = aIndex;
            }
            DispText = supporpaddingTimeLists[ulIndex];
            for (i = 0; i < paddingTimeStrIDList.length; i += 1) {
                if (DispText === paddingTimeStrIDList[i]) {
                    time = paddingTimeValueList[i];
                    break;
                }
            }
            return time;
        };
        cCPgRecording.prototype._on_alClicked = function (aParam) {
            var that = this, data = aParam.alData, target = data.me, bConsumed = false;
            var szItemId = 'undefined';
            switch (target) {
                case that._recordMainMenu:
                    bConsumed = true;
                    that._recordMainMenu.off();
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
                            if (szItemId === 'startpadding') {
                                that._setStartPadding(data.index);
                            }
                            else if (szItemId === 'endpadding') {
                                that._setEndPadding(data.index);
                            }
                            else if (szItemId === 'descramblestandby') {
                                that._setDescramblestandbyOnoff(data.index);
                            }
                            else if (szItemId === 'tsr') {
                                that._setTsrOnoff(data.index);
                            }
                        }
                    });
                    break;
            }
            return bConsumed;
        };
        cCPgRecording.prototype._on_alExit = function (aParam) {
            var that = this, bConsumed = false;
            if (aParam.alData) {
                if (aParam.alData.me === this._recordMainMenu) {
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
        cCPgRecording.prototype._on_alChangeFocus = function (aParam) {
            if (aParam.alData.index === 0 && aParam.alData.me === this._recordMainMenu) {
                this._$noti.html('');
            }
            else if (aParam.alData.index === 1 && aParam.alData.me === this._recordMainMenu) {
                this._$noti.html('');
            }
            else if (aParam.alData.index === 2 && aParam.alData.me === this._recordMainMenu) {
                this._$noti.html(hx.l('LOC_DESCRAMBLING_IN_STANDBY__DESC_ID'));
            }
        };
        cCPgRecording.prototype._on_alKey = function (aParam) {
            var that = this, bConsumed = false;
            var menuFocusIndex;
            switch (aParam.alKey) {
                case hx.key.KEY_ESCAPE:
                    if (aParam.alData) {
                        bConsumed = true;
                        $('#set').remove();
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
                        bConsumed = true;
                        if (aParam.alData.me === that._subList) {
                            menuFocusIndex = that._currentSubMenu;
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
                                    that._recordMainMenu.getItem(menuFocusIndex).removeClass('his');
                                    that._recordMainMenu.setItemFocus(menuFocusIndex);
                                    that._recordMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgRecording.prototype._conv_itemID_to_index = function (aItemID) {
            var that = this, i = 0, ulIndex = -1;
            for (i = 0; i < that._contextListMap.itemId.length; i += 1) {
                if (aItemID === that._contextListMap.itemId[i]) {
                    ulIndex = i;
                    return ulIndex;
                }
            }
            return ulIndex;
        };
        cCPgRecording.prototype._conv_index_to_itemID = function (aItemIdx) {
            var that = this, i = 0, itemID = 'undefined';
            for (i = 0; i < that._contextListMap.itemId.length; i += 1) {
                if (i === aItemIdx) {
                    itemID = that._contextListMap.itemId[i];
                    return itemID;
                }
            }
            return itemID;
        };
        cCPgRecording.prototype._makesubList = function (aParam) {
            var that = this, subFocusIndex = 0, szItemId = 'undefined', dataList;
            var topPx;
            szItemId = that._contextListMap.itemId[aParam];
            if (szItemId === 'startpadding') {
                dataList = that._contextListMap.itemSub01Str;
                subFocusIndex = that._conv_padtime_to_index(that._startPadding);
            }
            else if (szItemId === 'endpadding') {
                dataList = that._contextListMap.itemSub02Str;
                subFocusIndex = that._conv_padtime_to_index(that._endPadding);
            }
            else if (szItemId === 'descramblestandby') {
                dataList = that._contextListMap.itemSub03Str;
                subFocusIndex = (that._descrambleonoff === false) ? that.TMenuItemValue_OnOff.VALUE_OFF : that.TMenuItemValue_OnOff.VALUE_ON;
                topPx = '190px';
            }
            else if (szItemId === 'tsr') {
                dataList = that._contextListMap.itemSub03Str;
                subFocusIndex = (that._tsronoff === false) ? that.TMenuItemValue_OnOff.VALUE_OFF : that.TMenuItemValue_OnOff.VALUE_ON;
            }
            that._subList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length,
                top: topPx
            });
            that._$contextMenu.addClass('active');
            that._recordMainMenu.getItem(aParam).addClass('his');
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
        cCPgRecording.prototype._setStartPadding = function (aIndex) {
            var ulItemIdx = this._conv_itemID_to_index('startpadding');
            var $startPaddingSub = this._recordMainMenu.getItem(ulItemIdx).find('em');
            var startPaddingKey = hx.l(this._contextListMap.itemSub01Str[aIndex]);
            $startPaddingSub.html(startPaddingKey);
            this._startPadding = this._conv_index_to_padtime(aIndex);
            hx.svc.SETTING_UTIL.setPvrStartPadding(this._startPadding);
            this._recordMainMenu.getItem(ulItemIdx).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._recordMainMenu.on();
            this._recordMainMenu.setItemFocus(ulItemIdx);
        };
        cCPgRecording.prototype._setEndPadding = function (aIndex) {
            var ulItemIdx = this._conv_itemID_to_index('endpadding');
            var $endPaddingSub = this._recordMainMenu.getItem(ulItemIdx).find('em');
            var endPaddingKey = hx.l(this._contextListMap.itemSub02Str[aIndex]);
            $endPaddingSub.html(endPaddingKey);
            this._endPadding = this._conv_index_to_padtime(aIndex);
            hx.svc.SETTING_UTIL.setPvrEndPadding(this._endPadding);
            this._recordMainMenu.getItem(ulItemIdx).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._recordMainMenu.on();
            this._recordMainMenu.setItemFocus(ulItemIdx);
        };
        cCPgRecording.prototype._setDescramblestandbyOnoff = function (aIndex) {
            var ulItemIdx = this._conv_itemID_to_index('descramblestandby');
            var $descrambleonoffSub = this._recordMainMenu.getItem(ulItemIdx).find('em');
            var descrambleonoffKey = hx.l(this._contextListMap.itemSub03Str[aIndex]);
            $descrambleonoffSub.html(descrambleonoffKey);
            this._descrambleonoff = (aIndex === this.TMenuItemValue_OnOff.VALUE_ON) ? true : false;
            hx.svc.SETTING_UTIL.setDescrambleOnOff(this._descrambleonoff);
            this._recordMainMenu.getItem(ulItemIdx).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._recordMainMenu.on();
            this._recordMainMenu.setItemFocus(ulItemIdx);
        };
        cCPgRecording.prototype._setTsrOnoff = function (aIndex) {
            var ulItemIdx = this._conv_itemID_to_index('tsr');
            var $tsronoffSub = this._recordMainMenu.getItem(ulItemIdx).find('em');
            var tsronoffKey = hx.l(this._contextListMap.itemSub03Str[aIndex]);
            $tsronoffSub.html(tsronoffKey);
            this._tsronoff = (aIndex === this.TMenuItemValue_OnOff.VALUE_ON) ? true : false;
            hx.svc.SETTING_UTIL.setpvrTsrOnOff(this._tsronoff);
            this._recordMainMenu.getItem(ulItemIdx).removeClass('his');
            this._subList.$.remove();
            this._$contextMenu.removeClass('active');
            this._recordMainMenu.on();
            this._recordMainMenu.setItemFocus(ulItemIdx);
        };
        return cCPgRecording;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgRecording = cCPgRecording;
    hx.al.CPgRecording.recordMainMenu = function (aParam, aParam2, aParam3) {
        var recordmainmenu = hx.al.component('settingsList', {
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
        return recordmainmenu;
    };
    hx.al.CPgRecording.recordSubMenu = function (aParam) {
        var recordsubmenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': 'dep2'
            },
            roll: 'roll',
            data: aParam,
            count: aParam.length,
            fx: function (i, o) {
                o.html(hx.l(this.data[i]));
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_BACK]
        });
        return recordsubmenu;
    };
    if (hx.config.settings.recording) {
        hx.al.CPgRecording.prototype._contextListMap = hx.config.settings.recording;
    }
    else {
        hx.al.CPgRecording.prototype._contextListMap = {
            title: ['LOC_RECORDINGOPTIONS_ID'],
            itemId: ['startpadding', 'endpadding'],
            itemStr: ['LOC_START_PADDING_ID', 'LOC_END_PADDING_ID'],
            describe: ['', ''],
            itemSub01Str: ['LOC_OFF_ID', 'LOC_3_MINS_ID', 'LOC_5_MINS_ID', 'LOC_10_MINS_ID', 'LOC_20_MINS_ID'],
            itemSub02Str: ['LOC_OFF_ID', 'LOC_3_MINS_ID', 'LOC_5_MINS_ID', 'LOC_10_MINS_ID', 'LOC_20_MINS_ID']
        };
    }
    hx.al.CPgRecording.prototype.aniConfig = {
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
    return cCPgRecording;
});
