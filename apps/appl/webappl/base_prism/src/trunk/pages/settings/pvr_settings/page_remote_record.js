var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    //file name : pages/settings/pvr_settings/remote_record.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgRemoteRecord = (function (_super) {
        __extends(cCPgRemoteRecord, _super);
        function cCPgRemoteRecord() {
            _super.call(this);
            this.TMenuList = {
                REMOTERECORDING: 0,
                UPDATEPERIOD: 1,
                UPDATEINTERVAL: 2
            };
            this.dsrvTVPortal = hx.svc.TV_PORTAL;
            this._isRemoteRecordingOn = false;
            this._updatePeriod = 'All Day';
            this._updateInterval = 30;
            this._isIpEpgOn = false;
            this._currentSubMenu = 0;
            this._isChangePageInSetting = false;
            this._contextListMap = [
                ['LOC_REMOTERECORDING_ID', 'LOC_UPDATE_PERIOD_ID', 'LOC_UPDATE_INTERVAL_ID'],
                ['LOC_ON_ID', 'LOC_OFF_ID'],
                ['LOC_SETPERIOD_ID', 'LOC_ALLDAY_ID'],
                ['LOC_10_MINS_ID', 'LOC_20_MINS_ID', 'LOC_30_MINS_ID', 'LOC_40_MINS_ID', 'LOC_50_MINS_ID']
            ];
        }
        cCPgRemoteRecord.prototype.create = function () {
            _super.prototype.create.call(this);
            var that = this;
            var $DrawItem, value, tmp;
            $.extend(this, {
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _$contextMenu: $('<div>', {
                    'class': 'dep2'
                }),
                _remoteRecordMainMenu: hx.al.CPgRemoteRecord.remoteRecordMainMenu(that._contextListMap[0]),
                _$noti: $('<p>', {
                    'class': 'noti',
                    'html': 'Agree to the Privacy Policy first to use Remote Recording.' + '<br>' + 'When the Remote Recording menu is set to On, the receiver can receive the scheduled data that you have scheduled the programmes remotely by using tablet or PC.'
                })
            });
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_REMOTE_RECORD_ID');
            }
            that._isRemoteRecordingOn = hx.svc.SETTING_UTIL.getEnableRemoteRec();
            if (that._isRemoteRecordingOn === false) {
                that._remoteRecordMainMenu.getItem(1).addClass('dis');
                that._remoteRecordMainMenu.getItem(2).addClass('dis');
            }
            value = (that._isRemoteRecordingOn === true) ? 0 : 1;
            $DrawItem = that._remoteRecordMainMenu.getItem(0).find('em');
            $DrawItem.html(hx.l(that._contextListMap[1][value]));
            that._updatePeriod = hx.svc.SETTING_UTIL.getUpdatePeriodRemoteRec();
            tmp = that._conv_updatePeriod_to_string(that._updatePeriod);
            $DrawItem = that._remoteRecordMainMenu.getItem(1).find('em');
            $DrawItem.html(tmp);
            that._updateInterval = hx.svc.SETTING_UTIL.getUpdateIntervalRemoteRec();
            that._isIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
            value = that._conv_updateInterval_to_index(that._updateInterval);
            $DrawItem = that._remoteRecordMainMenu.getItem(2).find('em');
            $DrawItem.html(hx.l(that._contextListMap[3][value]));
            $('#set').append(that._$menu.append(that._$contextMenu.after(that._remoteRecordMainMenu.$)).after(that._$noti));
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
                    that.append(that._remoteRecordMainMenu);
                    that._remoteRecordMainMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        cCPgRemoteRecord.prototype.destroy = function () {
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
            this._remoteRecordMainMenu = undefined;
            this._$noti = undefined;
        };
        cCPgRemoteRecord.prototype._conv_updateInterval_to_index = function (aTime) {
            var index;
            switch (aTime) {
                case 20:
                    index = 1;
                    break;
                case 30:
                    index = 2;
                    break;
                case 40:
                    index = 3;
                    break;
                case 50:
                    index = 4;
                    break;
                case 10:
                    index = 0;
                    break;
                default:
                    index = 0;
                    break;
            }
            return index;
        };
        cCPgRemoteRecord.prototype._conv_index_to_updateInterval = function (aIndex) {
            var time;
            switch (aIndex) {
                case 1:
                    time = 20;
                    break;
                case 2:
                    time = 30;
                    break;
                case 3:
                    time = 40;
                    break;
                case 4:
                    time = 50;
                    break;
                case 0:
                    time = 10;
                    break;
                default:
                    time = 10;
                    break;
            }
            return time;
        };
        cCPgRemoteRecord.prototype._conv_updatePeriod_to_string = function (aUpdatePeriod) {
            var that = this, value = aUpdatePeriod;
            var startTimeHour, startTimeMinute, endTimeHour, endTimeMinute, str_value;
            //startDate = new Date(1970, 1, 1, (value.start_time/3600), (value.start_time%3600), 0 ),
            //endDate = new Date(1970, 1, 1, (value.end_time/3600), (value.end_time%3600), 0 ),
            //str_value = startDate.format("HH:MM") + ' - ' + endDate.format("HH:MM");
            startTimeHour = Math.floor(value.start_time / 3600);
            if (startTimeHour < 10) {
                startTimeHour = '0' + (value.start_time / 3600);
            }
            startTimeMinute = Math.floor(((value.start_time % 3600) / 60));
            if (startTimeMinute < 10) {
                startTimeMinute = '0' + (value.start_time % 3600) / 60;
            }
            endTimeHour = Math.floor(value.end_time / 3600);
            if (endTimeHour < 10) {
                endTimeHour = '0' + (value.end_time / 3600);
            }
            endTimeMinute = Math.floor((value.end_time % 3600) / 60);
            if (endTimeMinute < 10) {
                endTimeMinute = '0' + (value.end_time % 3600) / 60;
            }
            str_value = startTimeHour + ':' + startTimeMinute + ' - ' + endTimeHour + ':' + endTimeMinute;
            if (str_value === '00:00 - 24:00') {
                str_value = "All Day";
            }
            that._updatePeriod = str_value;
            return str_value;
        };
        cCPgRemoteRecord.prototype._conv_updatePeriod_to_secondValueString = function (aUpdatePeriod) {
            var startHour = aUpdatePeriod.substring(0, 2) * 60 * 60, startMinute = aUpdatePeriod.substring(3, 5) * 60, endHour = aUpdatePeriod.substring(8, 10) * 60 * 60, endMinute = aUpdatePeriod.substring(11, 13) * 60;
            //var that = this, value = updatePeriod,
            var startTime = startHour + startMinute, endTime = endHour + endMinute;
            return {
                start_time: startTime,
                end_time: endTime
            };
        };
        cCPgRemoteRecord.prototype._on_alClicked = function (aParam) {
            var that = this, data = aParam.alData, target = data.me, index, bConsumed = false;
            var tmp, netConnectedInfo;
            switch (target) {
                case that._remoteRecordMainMenu:
                    bConsumed = true;
                    if (data.index === that.TMenuList.REMOTERECORDING || data.index === that.TMenuList.UPDATEPERIOD || data.index === that.TMenuList.UPDATEINTERVAL) {
                        that._remoteRecordMainMenu.off();
                        that._makesubList(data.index);
                    }
                    that._currentSubMenu = data.index;
                    break;
                case that._subList:
                    bConsumed = true;
                    that._subList.off();
                    that._$contextMenu.ani({
                        style: that.aniConfig.dep2.fadeOut,
                        trans: { dur: 150, delay: 75 },
                        cb: function () {
                            that._$contextMenu.removeClass('active');
                            if (that._currentSubMenu === that.TMenuList.REMOTERECORDING) {
                                that._setRemoteRecording(data.index);
                            }
                            else if (that._currentSubMenu === that.TMenuList.UPDATEPERIOD) {
                                that._setUpdatePeriod(data.index);
                            }
                            else if (that._currentSubMenu === that.TMenuList.UPDATEINTERVAL) {
                                that._setUpdateInterval(data.index);
                            }
                        }
                    });
                    break;
            }
            return bConsumed;
        };
        cCPgRemoteRecord.prototype._on_alChangeFocus = function (aParam) {
            if (aParam.alData.index === 0 && aParam.alData.me === this._remoteRecordMainMenu) {
                this._$noti.html('When the Remote Recording menu is set to On, the receiver can receive the scheduled data that you have scheduled the programmes remotely by using tablet or PC.');
            }
            else if (aParam.alData.index === 1 && aParam.alData.me === this._remoteRecordMainMenu) {
                this._$noti.html('');
            }
            else if (aParam.alData.index === 2 && aParam.alData.me === this._remoteRecordMainMenu) {
                this._$noti.html('');
            }
        };
        cCPgRemoteRecord.prototype._on_alExit = function (aParam) {
            var bConsumed = false;
            if (aParam.alData) {
                if (aParam.alData.me === this._remoteRecordMainMenu) {
                    bConsumed = true;
                    this._gotoSettingPage();
                }
            }
            return bConsumed;
        };
        cCPgRemoteRecord.prototype._on_alKey = function (aParam) {
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
                                    that._remoteRecordMainMenu.getItem(menuFocusIndex).removeClass('his');
                                    that._remoteRecordMainMenu.setItemFocus(menuFocusIndex);
                                    that._remoteRecordMainMenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgRemoteRecord.prototype._makesubList = function (aParam) {
            var that = this, index, subFocusIndex;
            var dataList;
            if (aParam === that.TMenuList.REMOTERECORDING) {
                index = 1;
                subFocusIndex = (that._isRemoteRecordingOn === true) ? 0 : 1;
            }
            else if (aParam === that.TMenuList.UPDATEPERIOD) {
                index = 2;
                subFocusIndex = (that._updatePeriod === 'All Day') ? 1 : 0;
            }
            else if (aParam === that.TMenuList.UPDATEINTERVAL) {
                index = 3;
                subFocusIndex = that._conv_updateInterval_to_index(that._updateInterval);
            }
            dataList = that._contextListMap[index];
            that._subList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length
            });
            that._$contextMenu.addClass('active');
            that._remoteRecordMainMenu.getItem(aParam).addClass('his');
            that._$contextMenu.append(that._subList.$);
            that._subList.bind(dataList);
            that._subList.draw();
            that._$contextMenu.ani({
                style: that.aniConfig.dep2.fadeIn,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    that.append(that._subList);
                    that._subList.on();
                }
            });
        };
        cCPgRemoteRecord.prototype._setRemoteRecording = function (aIndex) {
            var that = this, _onConfirm;
            if (aIndex === 0) {
                if (that._isIpEpgOn === true && that._isRemoteRecordingOn === false) {
                    that._isRemoteRecordingOn = true;
                    hx.svc.SETTING_UTIL.setEnableRemoteRec(true);
                    that._remoteRecordMainMenu.getItem(1).removeClass('dis');
                    that._remoteRecordMainMenu.getItem(2).removeClass('dis');
                    _showRemoteRecordingSet(that._isRemoteRecordingOn);
                }
                else if (that._isIpEpgOn === false && that._isRemoteRecordingOn === false) {
                    _onConfirm = hx.msg('confirm', {
                        text: hx.l('LOC_SET_REMOTE_RECORDING_ON_ID'),
                        auto_close: false,
                        dia_class: 'osd',
                        btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                        callback_fn: _onCallback
                    });
                }
                else {
                    _showRemoteRecordingSet(that._isRemoteRecordingOn);
                }
            }
            else if (aIndex === 1) {
                that._isRemoteRecordingOn = false;
                hx.svc.SETTING_UTIL.setEnableRemoteRec(false);
                that._remoteRecordMainMenu.getItem(1).addClass('dis');
                that._remoteRecordMainMenu.getItem(2).addClass('dis');
                _showRemoteRecordingSet(that._isRemoteRecordingOn);
            }
            function _onCallback(_status) {
                if (_status === hx.l('LOC_YES_ID')) {
                    that._isIpEpgOn = true;
                    hx.svc.SETTING_UTIL.setEnableIPEPG(true);
                    that._isRemoteRecordingOn = true;
                    hx.svc.SETTING_UTIL.setEnableRemoteRec(true);
                    that._remoteRecordMainMenu.getItem(1).removeClass('dis');
                    that._remoteRecordMainMenu.getItem(2).removeClass('dis');
                    _showRemoteRecordingSet(that._isRemoteRecordingOn);
                }
                else if (_status === hx.l('LOC_NO_ID') || _status === 'key_back') {
                    _showRemoteRecordingSet(that._isRemoteRecordingOn);
                }
                else if (_status === 'key_esc') {
                    that._gotoSettingPage();
                }
            }
            function _showRemoteRecordingSet(isRemoteRecordingOn) {
                var index = (isRemoteRecordingOn) ? 0 : 1;
                var $remoteRecordingSub = that._remoteRecordMainMenu.getItem(0).find('em');
                var remoteRecordingKey = hx.l(that._contextListMap[1][index]);
                $remoteRecordingSub.html(remoteRecordingKey);
                that._remoteRecordMainMenu.getItem(0).removeClass('his');
                that._subList.$.remove();
                that._$contextMenu.removeClass('active');
                that._remoteRecordMainMenu.on();
            }
        };
        cCPgRemoteRecord.prototype._setUpdatePeriod = function (aIndex) {
            var that = this, convUpdatePeriod = {}, inputPeriod;
            if (aIndex === 0) {
                inputPeriod = hx.msg('inputtime', {
                    text: hx.l('LOC_ENTER_START_END_TIME_ID'),
                    auto_close: false,
                    dia_class: 'osd',
                    enable: true,
                    callback_fn: periodfn
                });
                function periodfn(_status, _input) {
                    if (_status === 'stop') {
                        var periodString = _input.substring(0, 2) + ':' + _input.substring(2, 4) + ' - ' + _input.substring(4, 6) + ':' + _input.substring(6, 8);
                        if (_input.substring(0, 2).length !== 2 || _input.substring(2, 4).length !== 2 || _input.substring(4, 6).length !== 2 || _input.substring(6, 8).length !== 2) {
                            hx.msg.close('inputtime', inputPeriod);
                            _removeUpdatePeriodList();
                            hx.msg('response', {
                                text: "Invalid time values",
                                auto_close: true,
                                close_time: 3000,
                                dia_class: 'res vaTop'
                            });
                        }
                        else if (_input.substring(0, 4) === _input.substring(4, 8)) {
                            // it is same time case..
                            hx.msg.close('inputtime', inputPeriod);
                            _removeUpdatePeriodList();
                            hx.msg('response', {
                                text: "Invalid time values",
                                auto_close: true,
                                close_time: 3000,
                                dia_class: 'res vaTop'
                            });
                        }
                        else {
                            // _input.substring(6, 8), 10))
                            var $updatePeriodSub = that._remoteRecordMainMenu.getItem(1).find('em');
                            $updatePeriodSub.html(periodString);
                            convUpdatePeriod = that._conv_updatePeriod_to_secondValueString(periodString);
                            hx.svc.SETTING_UTIL.setUpdatePeriodRemoteRec(convUpdatePeriod);
                            hx.msg.close('inputtime', inputPeriod);
                            _removeUpdatePeriodList();
                        }
                    }
                    else if (_status === 'key_back') {
                        hx.msg.close('pin', inputPeriod);
                        _removeUpdatePeriodList();
                    }
                    else if (_status === 'key_esc') {
                        hx.msg.close('pin', inputPeriod);
                        that._gotoSettingPage();
                    }
                }
            }
            else if (aIndex === 1) {
                var $updatePeriodSub = that._remoteRecordMainMenu.getItem(1).find('em');
                var updatePeriodKey = hx.l(that._contextListMap[2][1]);
                that._updatePeriod = updatePeriodKey;
                $updatePeriodSub.html(updatePeriodKey);
                hx.svc.SETTING_UTIL.setUpdatePeriodRemoteRec({
                    start_time: 0,
                    end_time: 86400
                });
                _removeUpdatePeriodList();
            }
            function _removeUpdatePeriodList() {
                that._remoteRecordMainMenu.getItem(1).removeClass('his');
                that._subList.$.remove();
                that._$contextMenu.removeClass('active');
                that._remoteRecordMainMenu.on();
                that._remoteRecordMainMenu.setItemFocus(1);
            }
        };
        cCPgRemoteRecord.prototype._setUpdateInterval = function (aIndex) {
            var that = this;
            var $updateIntervalSub = that._remoteRecordMainMenu.getItem(2).find('em');
            var updateIntervalKey = hx.l(that._contextListMap[3][aIndex]);
            $updateIntervalSub.html(updateIntervalKey);
            that._updateInterval = that._conv_index_to_updateInterval(aIndex);
            hx.svc.SETTING_UTIL.setUpdateIntervalRemoteRec(that._updateInterval);
            that._remoteRecordMainMenu.getItem(2).removeClass('his');
            that._subList.$.remove();
            that._$contextMenu.removeClass('active');
            that._remoteRecordMainMenu.on();
            that._remoteRecordMainMenu.setItemFocus(2);
        };
        cCPgRemoteRecord.prototype._gotoSettingPage = function () {
            var that = this;
            that._isChangePageInSetting = true;
            hx.al.decompose(that);
            that._$noti.ani({
                style: that.aniConfig.menu.changeOut3,
                trans: { dur: 200, delay: 100 }
            });
            that._$menu.ani({
                style: that.aniConfig.menu.changeOut3,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    that.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgSettings',
                        me: that
                    });
                }
            });
        };
        return cCPgRemoteRecord;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgRemoteRecord = cCPgRemoteRecord;
    hx.al.CPgRemoteRecord.remoteRecordMainMenu = function (aParam) {
        return hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': 'dep1'
            },
            fx: function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>'));
            },
            data: aParam,
            count: aParam.length,
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
    };
    hx.al.CPgRemoteRecord.remoteRecordSubMenu = function (aParam) {
        var subMenu = hx.al.component('settingsList', {
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
        return subMenu;
    };
    hx.al.CPgRemoteRecord.prototype.aniConfig = {
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
    return cCPgRemoteRecord;
});
