var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    // file name : pages/settings/pvr_settings/internal_storage.js
    /*jslint nomen: true, undef: true, sloppy: true */
    var _bActive = false;
    var CPgInternalStorage = (function (_super) {
        __extends(CPgInternalStorage, _super);
        function CPgInternalStorage() {
            _super.call(this);
            this.aniConfig = {
                channelMenu: {
                    fadeIn: {
                        start: {
                            'opacity': 0,
                            'top': '20px'
                        },
                        end: {
                            'opacity': 1,
                            'top': 0
                        }
                    },
                    fadeOut: {
                        start: {
                            'opacity': 1,
                            'top': 0
                        },
                        end: {
                            'opacity': 0,
                            'top': '20px'
                        }
                    }
                }
            };
            this._isDestroyed = false;
            this._ismadeInfo = false;
            this._strId = undefined;
            this._isChangePageInSetting = false;
        }
        CPgInternalStorage.prototype.isActive = function () {
            return _bActive;
        };
        CPgInternalStorage.prototype.create = function (aEntity, aPageUrl) {
            var that = this, info = [], usePercent = 0, availPercent = 0, retObj;
            _super.prototype.create.call(this);
            $.extend(that, {
                _$setStge: $('<section>', {
                    'class': 'setStge active'
                }),
                _txt: CPgInternalStorage.txt('LOC_INTERNALHDD_ID', 'h1'),
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _strMenu: CPgInternalStorage.mainMenu(['LOC_HDDTEST_ID', 'LOC_FORMATSTR_ID'], "dep1"),
                _$box: $('<div>', {
                    'class': 'box stge',
                    'style': 'width : 721px;'
                })
            });
            $('#set').append(that._$setStge.append(that._txt.$.after(that._$menu.append(that._strMenu.$).after(that._$box))));
            that._strId = aPageUrl.query;
            retObj = hx.svc.STORAGE_MASS.getDetailInfo({
                id: that._strId,
                cb: undefined
            });
            if (parseFloat(retObj.totalSize)) {
                usePercent = Math.floor((parseFloat(retObj.reservedSize) + parseFloat(retObj.usedSize)) / parseFloat(retObj.totalSize) * 100);
                availPercent = 100 - usePercent;
            }
            else {
                usePercent = 0;
                availPercent = 100;
            }
            info.push(retObj.totalSize + 'GB');
            info.push(retObj.reservedSize + 'GB');
            info.push(retObj.usedSize + 'GB');
            info.push(retObj.availableSize + 'GB');
            if (!that._ismadeInfo) {
                $.extend(that, {
                    _strInfo: CPgInternalStorage.strInfo(info),
                    _strGraph: CPgInternalStorage.strGraph(usePercent, availPercent)
                });
                that._$box.append(that._strInfo.$.after(that._strGraph.$));
            }
            else {
                that._strInfo.$.find('dd').eq(0).html(info[0]);
                that._strInfo.$.find('dd').eq(1).html(info[1]);
                that._strInfo.$.find('dd').eq(2).html(info[2]);
                that._strInfo.$.find('dd').eq(3).html(info[3]);
                that._strGraph.$.find('em').eq(0).html(usePercent);
                that._strGraph.$.find('em').eq(1).html(availPercent);
            }
            hx.il.drawCanvas(usePercent, usePercent + availPercent, 77, 75, 60, 'divStorageDlgRight');
            that._ismadeInfo = true;
            that._isDestroyed = false;
            _bActive = true;
            hx.al.compose(that);
            that._$setStge.ani({
                style: that.aniConfig.channelMenu.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    that.append(that._strMenu);
                    var dsrvRecordings = hx.svc.RECORDINGS;
                    dsrvRecordings.addEventCb('RecordingItemAdded', that, function (param) {
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text: ''
                        });
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: that
                        });
                    });
                    hx.svc.STORAGE_MASS.addEventCb('FormatProgress', that, function (aState, aProgress, aProgressMax, aErrMessage) {
                        that._on_FormatProgress(aState, aProgress, aProgressMax, aErrMessage);
                    });
                    hx.svc.STORAGE_MASS.addEventCb('RecoveryProgress', that, function (aState, aProgress, aProgressMax, aErrMessage) {
                        that._on_RecoveryProgress(aState, aProgress, aProgressMax, aErrMessage);
                    });
                    that._strMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        CPgInternalStorage.prototype.destroy = function () {
            var that = this;
            _super.prototype.destroy.call(this);
            hx.svc.STORAGE_MASS.clearEventCb(that);
            hx.al.decompose(that);
            that._$setStge.ani({
                style: that.aniConfig.channelMenu.fadeOut,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    var dsrvRecordings = hx.svc.RECORDINGS;
                    dsrvRecordings.clearEventCb(that, 'RecordingItemAdded');
                    that._isDestroyed = true;
                    _bActive = false;
                    that._ismadeInfo = false;
                    that._strId = undefined;
                    that._$setStge.remove();
                    if (!that._isChangePageInSetting) {
                        $('#set').remove();
                    }
                    else {
                        that._isChangePageInSetting = false;
                    }
                    that._$setStge = undefined;
                    that._txt = undefined;
                    that._$menu = undefined;
                    that._strMenu = undefined;
                    that._$box = undefined;
                }
            });
        };
        CPgInternalStorage.prototype._check_ScheduledItems_WillbeStarted = function () {
            var dsrvRecordings, scheduleList, i, j, ti, item;
            var sooner = null, nowTime, hasItem = false;
            dsrvRecordings = hx.svc.RECORDINGS;
            scheduleList = dsrvRecordings.getScheduledList();
            if (scheduleList.length) {
                for (i = 0, ti = scheduleList.length; i < ti; i += 1) {
                    item = scheduleList[i];
                    if (!item.isReminder() && !item.isPendingSeriesSchedule()) {
                        sooner = item;
                        break;
                    }
                }
                for (j = i; j < ti; j += 1) {
                    item = scheduleList[j];
                    if (sooner.getStartTime() > item.getStartTime() && !item.isReminder() && !item.isPendingSeriesSchedule()) {
                        sooner = item;
                    }
                }
                if (sooner !== null) {
                    nowTime = new Date();
                    if ((sooner.getStartTime() - nowTime) < 600000) {
                        hasItem = true;
                    }
                }
            }
            return hasItem;
        };
        CPgInternalStorage.prototype._on_FormatProgress = function (aState, aProgress, aProgressMax, aErrMessage) {
            var that = this, info = [], usePercent = 0, availPercent = 0, dsrvRecordings, retObj;
            if (that._isDestroyed) {
                return;
            }
            switch (aState) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    //EFormatProgressFinished
                    hx.msg.close('progress', that.progressMsg);
                    if (1 !== hx.config.useIntHDDFormatReboot) {
                        that._changeToLastChannel();
                    }
                    hx.il.criticalTasking.crHDDTasking = false;
                    retObj = hx.svc.STORAGE_MASS.getDetailInfo({
                        id: that._strId,
                        cb: undefined
                    });
                    if (parseFloat(retObj.totalSize)) {
                        usePercent = Math.floor((parseFloat(retObj.reservedSize) + parseFloat(retObj.usedSize)) / parseFloat(retObj.totalSize) * 100);
                        availPercent = 100 - usePercent;
                    }
                    else {
                        usePercent = 0;
                        availPercent = 100;
                    }
                    info.push(retObj.totalSize + 'GB');
                    info.push(retObj.reservedSize + 'GB');
                    info.push(retObj.usedSize + 'GB');
                    info.push(retObj.availableSize + 'GB');
                    if (!that._ismadeInfo) {
                        $.extend(this, {
                            _strInfo: CPgInternalStorage.strInfo(info),
                            _strGraph: CPgInternalStorage.strGraph(usePercent, availPercent)
                        });
                        that._$box.append(that._strInfo.$.after(that._strGraph.$));
                    }
                    else {
                        that._strInfo.$.find('dd').eq(0).html(info[0]);
                        that._strInfo.$.find('dd').eq(1).html(info[1]);
                        that._strInfo.$.find('dd').eq(2).html(info[2]);
                        that._strInfo.$.find('dd').eq(3).html(info[3]);
                        that._strGraph.$.find('em').eq(0).html(usePercent);
                        that._strGraph.$.find('em').eq(1).html(availPercent);
                    }
                    hx.il.drawCanvas(usePercent, usePercent + availPercent, 77, 75, 60, 'divStorageDlgRight');
                    that._ismadeInfo = true;
                    hx.msg('response', {
                        text: hx.l('LOC_STORAGE_FORMATTED_ID'),
                        auto_close: true,
                        close_time: 3000
                    });
                    if (1 === hx.config.useIntHDDFormatReboot) {
                        setTimeout(function () {
                            prism.log("**************************************");
                            prism.log("Internal HDD Format done.. Go Reboot!!");
                            prism.log("**************************************");
                            hx.svc.SETTING_UTIL.doReboot();
                        }, 3000);
                    }
                    else {
                        dsrvRecordings = hx.svc.RECORDINGS;
                        dsrvRecordings.refreshRecordingList();
                    }
                    break;
                case 3:
                    //EFormatProgressError
                    hx.msg.close('progress', that.progressMsg);
                    if (1 !== hx.config.useIntHDDFormatReboot) {
                        that._changeToLastChannel();
                    }
                    hx.il.criticalTasking.crHDDTasking = false;
                    that._failFormatMsg = hx.msg('codeConfirm', {
                        first_line: '',
                        regcode: hx.l('LOC_FAILED_HDD_FORMAT_ID'),
                        nolinejump: true,
                        second_line: hx.l('LOC_CANNOT_FORMAT_STORAGE_ID'),
                        auto_close: false,
                        btn_title_arr: [hx.l('LOC_OK_ID')],
                        callback_fn: failformat_fn,
                        dia_class: 'osd'
                    });
                    function failformat_fn(_status) {
                        if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                            hx.msg.close('confirm', that._failFormatMsg);
                            that._strMenu.on();
                            that._strMenu.setItemFocus(1);
                        }
                        else if (_status === 'key_esc') {
                            hx.msg.close('confirm', that._failFormatMsg);
                            that._isChangePageInSetting = true;
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgStorage?' + that._strId,
                                me: that
                            });
                        }
                    }
                    if (1 === hx.config.useIntHDDFormatReboot) {
                        setTimeout(function () {
                            hx.svc.SETTING_UTIL.doReboot();
                        }, 3000);
                    }
                    break;
            }
            if (aErrMessage > 0) {
                hx.msg.close('progress', that.progressMsg);
                if (1 === hx.config.useIntHDDFormatReboot) {
                    that._changeToLastChannel();
                }
                hx.il.criticalTasking.crHDDTasking = false;
                that._failFormatMsg = hx.msg('codeConfirm', {
                    first_line: '',
                    regcode: hx.l('LOC_FAILED_HDD_FORMAT_ID'),
                    nolinejump: true,
                    second_line: hx.l('LOC_CANNOT_FORMAT_STORAGE_ID'),
                    auto_close: false,
                    btn_title_arr: [hx.l('LOC_OK_ID')],
                    callback_fn: failformat_fn2,
                    dia_class: 'osd'
                });
                function failformat_fn2(_status) {
                    if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                        hx.msg.close('confirm', that._failFormatMsg);
                        that._strMenu.on();
                        that._strMenu.setItemFocus(1);
                    }
                    else if (_status === 'key_esc') {
                        hx.msg.close('confirm', that._failFormatMsg);
                        that._isChangePageInSetting = true;
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgStorage?' + that._strId,
                            me: that
                        });
                    }
                }
                if (1 === hx.config.useIntHDDFormatReboot) {
                    setTimeout(function () {
                        hx.svc.SETTING_UTIL.doReboot();
                    }, 3000);
                }
            }
        };
        CPgInternalStorage.prototype._on_RecoveryProgress = function (aState, aProgress, aProgressMax, aErrMessage) {
            var that = this, dummy;
            if (that._isDestroyed) {
                return;
            }
            if ((aProgress !== 0) && (aState !== 2)) {
                dummy = 0;
            }
            else if (aState === 2) {
                hx.msg.close('progress', that.progress);
                that._changeToLastChannel();
                hx.il.criticalTasking.crHDDTasking = false;
                that._strMenu.on();
                that._passHddMsg = hx.msg('codeConfirm', {
                    first_line: '',
                    regcode: hx.l('LOC_PASSED_HDDTEST_ID'),
                    nolinejump: true,
                    second_line: hx.l('LOC_NO_DEFECTS_ID'),
                    auto_close: false,
                    btn_title_arr: [hx.l('LOC_OK_ID')],
                    callback_fn: pass_fn,
                    dia_class: 'osd'
                });
                function pass_fn(_status) {
                    if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                        hx.msg.close('confirm', that._passHddMsg);
                    }
                    else if (_status === 'key_esc') {
                        hx.msg.close('confirm', that._passHddMsg);
                        that._isChangePageInSetting = true;
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgStorage?' + that._strId,
                            me: that
                        });
                    }
                }
            }
            if (aErrMessage > 0) {
                //TODO : error
                hx.msg.close('progress', that.progress);
                that._changeToLastChannel();
                hx.il.criticalTasking.crHDDTasking = false;
                that._failedHddMsg = hx.msg('codeConfirm', {
                    first_line: '',
                    regcode: hx.l('LOC_FAILED_HDDTEST_ID'),
                    nolinejump: true,
                    second_line: hx.l('LOC_SELECT_FORMAT_STORAGE_ID'),
                    auto_close: false,
                    btn_title_arr: [hx.l('LOC_OK_ID')],
                    callback_fn: fail_fn,
                    dia_class: 'osd'
                });
                function fail_fn(_status) {
                    if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                        hx.msg.close('confirm', that._failedHddMsg);
                    }
                    else if (_status === 'key_esc') {
                        hx.msg.close('confirm', that._failedHddMsg);
                        that._isChangePageInSetting = true;
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgStorage?' + that._strId,
                            me: that
                        });
                    }
                }
            }
        };
        CPgInternalStorage.prototype._on_dlVerifyPassword = function (aParam) {
            var that = this, bPbGoing;
            function _pinfn(_status, _input) {
                if (_status === 'stop') {
                    that._settedPassword = _input;
                    hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                        password: _input,
                        cb: function (retObj) {
                            that._on_dlVerifyPassword(retObj);
                        }
                    });
                }
                else if (_status === 'key_back') {
                    hx.msg.close('pin', that.pinMsg);
                    that._strMenu.on();
                    that._strMenu.setItemFocus(1);
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('pin', that.pinMsg);
                    that._isChangePageInSetting = true;
                    that.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgStorage?' + that._strId,
                        me: that
                    });
                }
            }
            // 0: PIN is correct, 1: PIN is incorrect, 2: PIN entry is locked
            if (aParam.result === 0) {
                hx.il.criticalTasking.crHDDTasking = true;
                hx.msg.close('pin', that.pinMsg);
                bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing();
                if (bPbGoing) {
                    hx.svc.PLAYBACK.stop();
                }
                hx.svc.AV_CTRL.stopCurChannel({
                    stopAV: 1,
                    stopTsr: 1
                });
                // DMR uri set null ('bring in client' do not have to work)
                hx.svc.DMR.setMedia(null);
                hx.svc.STORAGE_MASS.doFormat({
                    id: that._strId
                });
                that.progressMsg = hx.msg('progress', {
                    text: hx.l('LOC_PROCESSING_ID'),
                    auto_close: false
                });
                hx.msg.close('pin', that.pinMsg);
                that._strMenu.on();
                that._strMenu.setItemFocus(1);
            }
            else if (aParam.result === 1) {
                hx.msg.close('pin', that.pinMsg);
                that.pinMsg = hx.msg('pin', {
                    text: hx.l('LOC_ENTER_PASSWORD_ID'),
                    callback_fn: _pinfn,
                    auto_close: false,
                    dia_class: 'osd',
                    enable: true
                });
            }
        };
        CPgInternalStorage.prototype._on_alClicked = function (aParam) {
            var that = this, data = aParam.alData, bConsumed = false, bPbGoing, has_HotScheduledItem;
            if (data.me === that._strMenu) {
                bConsumed = true;
                if (data.index === 0) {
                    //TODO : HDD Test
                    hx.il.criticalTasking.crHDDTasking = true;
                    bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing();
                    if (bPbGoing) {
                        hx.svc.PLAYBACK.stop();
                    }
                    hx.svc.AV_CTRL.stopCurChannel({
                        stopAV: 1,
                        stopTsr: 1
                    });
                    // DMR uri set null ('bring in client' do not have to work)
                    hx.svc.DMR.setMedia(null);
                    hx.svc.STORAGE_MASS.doCleanUp({
                        id: that._strId
                    });
                    that.progress = hx.msg('progress', {
                        text: hx.l('LOC_PROCESSING_TAKE_MINUTES_ID'),
                        auto_close: false
                    });
                }
                else if (data.index === 1) {
                    has_HotScheduledItem = that._check_ScheduledItems_WillbeStarted();
                    if (has_HotScheduledItem) {
                        that.testMsg = hx.msg('response', {
                            text: hx.l('LOC_FORMAT_NOT_ALLOWED_ID'),
                            auto_close: true,
                            close_time: 3000
                        });
                        that._strMenu.setItemFocus(1);
                    }
                    else {
                        that.selectMsg = hx.msg('confirm', {
                            text: hx.l('LOC_FORMAT_SYSTEM_RESTART_ID'),
                            auto_close: false,
                            //timeout : 5000,
                            btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                            dia_class: 'osd',
                            callback_fn: twoDepthSelect
                        });
                        $('#DIA .con .button').eq(0).removeClass('on');
                        //default is NO btn
                        $('#DIA .con .button').eq(1).addClass('on');
                    }
                    /*
                     function oneDepthSelect(_status) {
                     if (_status === hx.l('LOC_YES_ID')) {
                     hx.msg.close('confirm', that.selectMsg);
                     setTimeout(function(){
                     that.selectMsgTwo = hx.msg('confirm', {
                     text : hx.l('LOC_FORMAT_WISH_TO_CONTINUE_ID'),
                     auto_close : false,
                     //timeout : 5000,
                     btn_title_arr : [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                     dia_class : 'osd',
                     callback_fn : twoDepthSelect
                     });
                     $('#DIA .con .button').eq(0).removeClass('on');
                     //default is NO btn
                     $('#DIA .con .button').eq(1).addClass('on');
                     }, 0);
                     } else if (_status === hx.l('LOC_NO_ID')) {
                     hx.msg.close('confirm', that.selectMsg);
                     that._strMenu.on();
                     that._strMenu.setItemFocus(1);
                     } else if (_status === 'key_back') {
                     hx.msg.close('confirm', that.selectMsg);
                     that._strMenu.on();
                     that._strMenu.setItemFocus(1);
                     } else if (_status === 'key_esc') {
                     hx.msg.close('confirm', that.selectMsg);
                     that._isChangePageInSetting = true;
                     that.sendEventToEntity({
                     alEvent : 'alChangePage',
                     target : 'CPgStorage',
                     me : that
                     });
                     }
                     }
                     */
                    function twoDepthSelect(_status) {
                        if (_status === hx.l('LOC_YES_ID')) {
                            hx.msg.close('confirm', that.selectMsgTwo);
                            that.pinMsg = hx.msg('pin', {
                                text: hx.l('LOC_ENTER_PASSWORD_ID'),
                                callback_fn: pinfn,
                                auto_close: false,
                                dia_class: 'osd',
                                enable: true
                            });
                        }
                        else if (_status === hx.l('LOC_NO_ID')) {
                            hx.msg.close('confirm', that.selectMsgTwo);
                            that._strMenu.on();
                            that._strMenu.setItemFocus(1);
                        }
                        else if (_status === 'key_back') {
                            hx.msg.close('confirm', that.selectMsgTwo);
                            that._strMenu.on();
                            that._strMenu.setItemFocus(1);
                        }
                        else if (_status === 'key_esc') {
                            hx.msg.close('confirm', that.selectMsgTwo);
                            that._isChangePageInSetting = true;
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgStorage?' + that._strId,
                                me: that
                            });
                        }
                    }
                    function pinfn(_status, _input) {
                        if (_status === 'stop') {
                            that._settedPassword = _input;
                            hx.svc.PARENTAL_CTRL.verifyParentalControlPIN({
                                password: _input,
                                cb: function (retObj) {
                                    that._on_dlVerifyPassword(retObj);
                                }
                            });
                        }
                        else if (_status === 'key_back') {
                            hx.msg.close('pin', that.pinMsg);
                            that._strMenu.on();
                            that._strMenu.setItemFocus(1);
                        }
                        else if (_status === 'key_esc') {
                            hx.msg.close('pin', that.pinMsg);
                            that._isChangePageInSetting = true;
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgStorage?' + that._strId,
                                me: that
                            });
                        }
                    }
                }
            }
            return bConsumed;
        };
        CPgInternalStorage.prototype._on_alExit = function (aParam) {
            var data = aParam.alData, bConsumed = false;
            if (data) {
                if (data.me === this._strMenu) {
                    bConsumed = true;
                    this._isChangePageInSetting = true;
                    this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgStorage?' + this._strId,
                        me: this
                    });
                }
            }
            return bConsumed;
        };
        CPgInternalStorage.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    bConsumed = true;
                    if (aParam.alData) {
                        this._isChangePageInSetting = true;
                        this.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgStorage?' + this._strId,
                            me: this
                        });
                    }
                    break;
                case hx.key.KEY_POWER:
                    if (hx.il.isCriticalTasking()) {
                        bConsumed = true;
                    }
                    break;
            }
            return bConsumed;
        };
        CPgInternalStorage.prototype._changeToLastChannel = function () {
            var result;
            // tune to available channel
            result = hx.svc.AV_CTRL.lastChannel();
            if (result === false) {
                hx.svc.AV_CTRL.availableChannel();
            }
        };
        CPgInternalStorage.mainMenu = function (aParam, aParam2) {
            var mainmenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': aParam2
                },
                data: aParam,
                count: aParam.length,
                fx: function (i, o) {
                    o.html(hx.l(this.data[i]));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return mainmenu;
        };
        CPgInternalStorage.txt = function (aParam, aParam2) {
            return hx.al.component('text', {
                tag: aParam2,
                attr: {
                    'data-langID': aParam,
                    'class': 'ico hdd'
                }
            });
        };
        CPgInternalStorage.strInfo = function (aParam) {
            return hx.al.component('storageInfo', {
                totalStorage: aParam[0],
                systemUsage: aParam[1],
                userUsage: aParam[2],
                available: aParam[3]
            });
        };
        CPgInternalStorage.strGraph = function (aParam, aParam2) {
            return hx.al.component('storageGraph', {
                used: aParam,
                available: aParam2
            });
        };
        return CPgInternalStorage;
    })(__KERNEL_SETTINGS__);
    return CPgInternalStorage;
});
