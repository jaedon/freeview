var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    // file name : pages/settings/pvr_settings/external_storage.js
    /*jslint nomen: true, undef: true, sloppy: true */
    var _bActive = false;
    var CPgExternalStorage = (function (_super) {
        __extends(CPgExternalStorage, _super);
        function CPgExternalStorage() {
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
            this._contextListMap = hx.config.settings.extstorage || {
                title: [''],
                itemId: ['removestorage', 'formatstorage'],
                itemStr: ['LOC_REMOVE_STORAGESAFELY_ID', 'LOC_FORMATSTR_ID'],
                describe: ['', ''],
                itemSub01Str: [],
                itemSub02Str: []
            };
            this._ishdd = false;
            this._isDestroyed = false;
            this._ismadeInfo = false;
            this._strId = undefined;
            this._label = '';
            this._path = '';
            this._bStatusUpdate = false;
            this._isChangePageInSetting = false;
            this._selectMsg = undefined;
        }
        CPgExternalStorage.prototype.isActive = function () {
            return _bActive;
        };
        CPgExternalStorage.prototype.create = function (aEntity, aPageUrl) {
            _super.prototype.create.call(this);
            var that = this;
            var ulItemIdx = 0, retObj;
            $.extend(that, {
                _$setStge: $('<section>', {
                    'class': 'setStge active'
                }),
                _txt: CPgExternalStorage.txt('', 'h1'),
                _$menu: $('<article>', {
                    'class': 'menu'
                }),
                _strMenu: CPgExternalStorage.mainMenu(that._contextListMap.itemStr, that._contextListMap.describe, "dep1"),
                _$box: $('<div>', {
                    'class': 'box stge',
                    'style': 'width : 721px;'
                })
            });
            $('#set').append(that._$setStge.append(that._txt.$.after(that._$menu.append(that._strMenu.$).after(that._$box))));
            that._strId = aPageUrl.query;
            //hx.logadd('CPgExternalStorage');
            if ((aPageUrl.path === '/externalPvrHDD') && (hx.config.useInternalPvr === 1)) {
                that._ishdd = true;
                that._label = hx.l('LOC_DETACHHDD_ID');
                that._txt.$.addClass('ico hdd');
                hx.svc.STORAGE_MASS.addEventCb('FormatProgress', that, function (aState, aProgress, aProgressMax, aErrMessage) {
                    that._on_FormatProgress(aState, aProgress, aProgressMax, aErrMessage);
                });
            }
            else if ((aPageUrl.path === '/externalPvrHDD') && (hx.config.useInternalPvr === 0)) {
                that._ishdd = true;
                that._txt.$.addClass('ico usb');
                hx.svc.STORAGE_MASS.addEventCb('FormatProgress', that, function (aState, aProgress, aProgressMax, aErrMessage) {
                    that._on_FormatProgress(aState, aProgress, aProgressMax, aErrMessage);
                });
            }
            else if (aPageUrl.path === '/externalHDD') {
                that._ishdd = true;
                that._txt.$.addClass('ico usb');
                hx.svc.STORAGE_MASS.addEventCb('FormatProgress', that, function (aState, aProgress, aProgressMax, aErrMessage) {
                    that._on_FormatProgress(aState, aProgress, aProgressMax, aErrMessage);
                });
            }
            else if (aPageUrl.path === '/usbMemory') {
                that._txt.$.addClass('ico usb');
            }
            else if (aPageUrl.path === '/sdMemory') {
                that._txt.$.addClass('ico sdm');
            }
            else if ((aPageUrl.path === '/internalPvrHDD') && (hx.config.useInternalPvr === 0)) {
                that._ishdd = true;
                that._txt.$.addClass('ico usbp');
                hx.svc.STORAGE_MASS.addEventCb('FormatProgress', that, function (aState, aProgress, aProgressMax, aErrMessage) {
                    that._on_FormatProgress(aState, aProgress, aProgressMax, aErrMessage);
                });
            }
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageStatus', that, function (aId, aStatus) {
                that._on_PhysicalStorageStatus(aId, aStatus);
            });
            retObj = hx.svc.STORAGE_MASS.getDetailInfo({
                id: that._strId,
                cb: undefined
            });
            if (aPageUrl.path !== '/externalPvrHDD') {
                that._label = retObj.label;
            }
            else if ((aPageUrl.path === '/externalPvrHDD') && (hx.config.useInternalPvr === 0)) {
                that._label = retObj.label;
            }
            else if ((aPageUrl.path === '/internalPvrHDD') && (hx.config.useInternalPvr === 0)) {
                that._label = retObj.label;
            }
            if (aPageUrl.path === '/usbMemory') {
                if (!that._label || that._label === '') {
                    that._label = hx.l('LOC_UNKNOWN_ID');
                }
            }
            else if (aPageUrl.path === '/sdMemory') {
                if (!that._label || that._label === '') {
                    that._label = hx.l('LOC_UNKNOWN_ID');
                }
            }
            if (!retObj.ismount) {
                ulItemIdx = that._conv_itemID_to_index('removestorage');
                that._strMenu.getItem(ulItemIdx).addClass('dis');
                ulItemIdx = that._conv_itemID_to_index('formatstorage');
                that._strMenu.setItemFocus(ulItemIdx);
            }
            if (hx.config.useInternalPvr === 0) {
                if (aPageUrl.path === '/internalPvrHDD') {
                    ulItemIdx = that._conv_itemID_to_index('selecthddforpvr');
                    that._strMenu.getItem(ulItemIdx).addClass('dis');
                }
                else if ((aPageUrl.path === '/usbMemory') || (aPageUrl.path === '/sdMemory')) {
                    ulItemIdx = that._conv_itemID_to_index('selecthddforpvr');
                    that._strMenu.getItem(ulItemIdx).addClass('dis');
                    ulItemIdx = that._conv_itemID_to_index('deselecthddforpvr');
                    that._strMenu.getItem(ulItemIdx).addClass('dis');
                }
                else {
                    ulItemIdx = that._conv_itemID_to_index('deselecthddforpvr');
                    that._strMenu.getItem(ulItemIdx).addClass('dis');
                }
            }
            that._path = aPageUrl.path;
            that._isDestroyed = false;
            that._showDetailInfo(retObj);
            _bActive = true;
            that._bStatusUpdate = false;
            that._selectMsg = undefined;
            hx.al.compose(that);
            that._$setStge.ani({
                style: that.aniConfig.channelMenu.fadeIn,
                trans: { dur: 200, delay: 100 },
                cb: function () {
                    that.append(that._strMenu);
                    that._createItem();
                    that._strMenu.on();
                    that._isChangePageInSetting = false;
                }
            });
        };
        CPgExternalStorage.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            var that = this;
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
                    // closed all PopUp
                    hx.msg.close('confirm', that._selectMsg);
                    hx.msg.close('progress', that.progressMsg);
                    hx.msg.close('codeConfirm', that._failFormatMsg);
                    hx.msg.close('pin', that.pinMsg);
                    _bActive = false;
                    that._isDestroyed = true;
                    that._ismadeInfo = false;
                    that._ishdd = false;
                    that._strId = undefined;
                    that._bStatusUpdate = false;
                    that._$setStge.remove();
                    that._path = '';
                    that._selectMsg = undefined;
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
        CPgExternalStorage.prototype._createItem = function () {
            var that = this;
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
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageDetached', that, function (aPhysicalStorage) {
                that._on_PhysicalStorageDetached(aPhysicalStorage);
            });
            hx.svc.STORAGE_MASS.addEventCb('DetachProgress', that, function (aState, aErrMessage) {
                that._on_DetachProgress(aState, aErrMessage);
            });
        };
        CPgExternalStorage.prototype._check_ScheduledItems_WillbeStarted = function () {
            var that = this;
            var dsrvRecordings, scheduleList;
            var i, ti, item, sooner = null, nowTime, hasItem = false;
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
                for (; i < ti; i += 1) {
                    item = scheduleList[i];
                    if (sooner.getStartTime() > item.getStartTime() && !item.isReminder() && !item.isPendingSeriesSchedule()) {
                        sooner = item;
                    }
                }
                if (sooner != null) {
                    nowTime = new Date();
                    if ((sooner.getStartTime() - nowTime) < 600000) {
                        hasItem = true;
                    }
                }
            }
            return hasItem;
        };
        CPgExternalStorage.prototype._on_PhysicalStorageDetached = function (aPhysicalStorage) {
            var ulStorageId = undefined, szMsg = '';
            ulStorageId = Number(this._strId);
            // Check Current Storage
            if (ulStorageId === aPhysicalStorage.id) {
                if (hx.il.criticalTasking.crHDDTasking === true) {
                    hx.msg.close('progress', this.progressMsg);
                    hx.il.criticalTasking.crHDDTasking = false;
                    szMsg = hx.l('LOC_USB_REMOVED_ID');
                }
                else {
                    szMsg = hx.l('LOC_STORAGE_REMOVED_SAFELY_ID');
                }
                hx.msg.close('progress', this.progressMsg);
                this._bStatusUpdate = true;
                hx.msg.close('progress', this.removeProgressMsg);
                this._changeToLastChannel();
                if (this._isDestroyed) {
                    return;
                }
                if (this._failFormatMsg !== undefined) {
                    hx.msg.close('codeConfirm', this._failFormatMsg);
                    this._failFormatMsg = undefined;
                }
                if (this._selectMsg !== undefined) {
                    hx.msg.close('confirm', this._selectMsg);
                    this._selectMsg = undefined;
                }
                hx.msg('response', {
                    text: szMsg,
                    auto_close: true,
                    close_time: 3000
                });
                this._isChangePageInSetting = true;
                this.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: 'CPgStorage?' + this._strId,
                    me: this
                });
            }
        };
        CPgExternalStorage.prototype._on_DetachProgress = function (aState, aErrMessage) {
            var ulItemIdx = 0, i, bDisable;
            if (this._isDestroyed) {
                hx.msg.close('progress', this.removeProgressMsg);
                this._changeToLastChannel();
                return;
            }
            if (aErrMessage > 0) {
                hx.msg.close('progress', this.removeProgressMsg);
                this._changeToLastChannel();
                this._strMenu.on();
                ulItemIdx = this._conv_itemID_to_index('removestorage');
                bDisable = this._strMenu[i].hasClass('dis');
                if (bDisable) {
                    ulItemIdx = this._conv_itemID_to_index('formatstorage');
                }
                this._strMenu.setItemFocus(ulItemIdx);
            }
        };
        CPgExternalStorage.prototype._on_FormatProgress = function (aState, aProgress, aProgressMax, aErrMessage) {
            var that = this, dsrvRecordings, ulItemIdx = 0, retObj;
            ulItemIdx = that._conv_itemID_to_index('formatstorage');
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
                    hx.msg('response', {
                        text: hx.l('LOC_STORAGE_FORMATTED_ID'),
                        auto_close: true,
                        close_time: 2000
                    });
                    retObj = hx.svc.STORAGE_MASS.getDetailInfo({
                        id: that._strId,
                        cb: undefined
                    });
                    that._showDetailInfo(retObj);
                    if (1 === hx.config.useIntHDDFormatReboot) {
                        setTimeout(function () {
                            hx.msg('response', {
                                text: hx.l('LOC_MSG_SYSTEM_WILL_RESTART_NOW_ID'),
                                auto_close: true,
                                close_time: 2000
                            });
                        }, 2000);
                        hx.svc.FRONT_PANEL.setFrontPanelText('BOOT');
                        setTimeout(function () {
                            prism.log("**************************************");
                            prism.log("External HDD Format done.. Go Reboot!!");
                            prism.log("**************************************");
                            hx.svc.SETTING_UTIL.doReboot();
                        }, 4000);
                    }
                    else {
                        dsrvRecordings = hx.svc.RECORDINGS;
                        dsrvRecordings.refreshRecordingList();
                    }
                    break;
                case 3:
                case 4:
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
                            hx.msg.close('codeConfirm', that._failFormatMsg);
                            that._strMenu.on();
                            that._strMenu.setItemFocus(ulItemIdx);
                        }
                        else if (_status === 'key_esc') {
                            hx.msg.close('codeConfirm', that._failFormatMsg);
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
                            hx.msg('response', {
                                text: hx.l('LOC_MSG_SYSTEM_WILL_RESTART_NOW_ID'),
                                auto_close: true,
                                close_time: 2000
                            });
                        }, 2000);
                        hx.svc.FRONT_PANEL.setFrontPanelText('BOOT');
                        setTimeout(function () {
                            hx.svc.SETTING_UTIL.doReboot();
                        }, 4000);
                    }
                    break;
            }
            if (aErrMessage > 0) {
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
                    callback_fn: failformat_fn2,
                    dia_class: 'osd'
                });
                function failformat_fn2(_status) {
                    if (_status === hx.l('LOC_OK_ID') || _status === 'key_back') {
                        hx.msg.close('codeConfirm', that._failFormatMsg);
                        that._strMenu.on();
                        that._strMenu.setItemFocus(ulItemIdx);
                    }
                    else if (_status === 'key_esc') {
                        hx.msg.close('codeConfirm', that._failFormatMsg);
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
                        hx.msg('response', {
                            text: hx.l('LOC_MSG_SYSTEM_WILL_RESTART_NOW_ID'),
                            auto_close: true,
                            close_time: 2000
                        });
                    }, 2000);
                    hx.svc.FRONT_PANEL.setFrontPanelText('BOOT');
                    setTimeout(function () {
                        hx.svc.SETTING_UTIL.doReboot();
                    }, 4000);
                }
            }
        };
        CPgExternalStorage.prototype._on_PhysicalStorageStatus = function (aId, aStatus) {
            var that = this;
            var stString, ulItemIdx = 0, bUpdatePage = false;
            var ulStorageId = undefined;
            hx.log('CPgExternalStorage', 'id : ' + aId);
            hx.log('CPgExternalStorage', 'status : ' + aStatus);
            ulStorageId = Number(that._strId);
            // Check Current Storage
            if (ulStorageId === aId) {
                // Check Message statues, because some time duplicate delivery
                if (that._bStatusUpdate === true) {
                    hx.log('CPgExternalStorage', 'Error!! Already check prev message');
                    return;
                }
                switch (aStatus) {
                    case 5:
                        hx.msg.close('progress', that.progressMsg);
                        ulItemIdx = that._conv_itemID_to_index('selecthddforpvr');
                        stString = hx.l('LOC_STORAGE_ASSIGN_PVR_COMPLETE_MSG_ID');
                        that._bStatusUpdate = true;
                        break;
                    case 6:
                        hx.msg.close('progress', that.progressMsg);
                        ulItemIdx = that._conv_itemID_to_index('deselecthddforpvr');
                        stString = hx.l('LOC_STORAGE_DEASSIGN_PVR_COMPLETE_MSG_ID');
                        that._bStatusUpdate = true;
                        break;
                    case 7:
                        hx.msg.close('progress', that.progressMsg);
                        ulItemIdx = that._conv_itemID_to_index('selecthddforpvr');
                        stString = hx.l('LOC_USB_LOADING_ERROR_ID');
                        that._bStatusUpdate = true;
                        break;
                    case 8:
                        hx.msg.close('progress', that.progressMsg);
                        ulItemIdx = that._conv_itemID_to_index('deselecthddforpvr');
                        stString = hx.l('LOC_USB_LOADING_ERROR_ID');
                        that._bStatusUpdate = true;
                        break;
                    case 11:
                    case 14:
                        hx.msg.close('progress', that.removeProgressMsg);
                        that._changeToLastChannel();
                        ulItemIdx = that._conv_itemID_to_index('removestorage');
                        stString = hx.l('LOC_USB_LOADING_ERROR_ID');
                        that._bStatusUpdate = true;
                        break;
                    case 4:
                        if (hx.il.criticalTasking.crHDDTasking === true) {
                            hx.msg.close('progress', that.progressMsg);
                            that._changeToLastChannel();
                            hx.il.criticalTasking.crHDDTasking = false;
                            stString = hx.l('LOC_CANNOT_FORMAT_STORAGE_ID');
                        }
                        else {
                            return;
                        }
                        break;
                    default:
                        hx.log('CPgExternalStorage', 'Skip this status Msg');
                        return;
                }
                that._bStatusUpdate = true;
                that._selectMsg = hx.msg('confirm', {
                    text: stString,
                    auto_close: false,
                    dia_class: 'osd',
                    btn_title_arr: [hx.l('LOC_OK_ID')],
                    callback_fn: msgActionConfirmfn
                });
                function msgActionConfirmfn(_status) {
                    that._bStatusUpdate = false;
                    if (_status === hx.l('LOC_OK_ID')) {
                        that._strMenu.on();
                        that._strMenu.setItemFocus(ulItemIdx);
                        hx.msg.close('confirm', that._selectMsg);
                        that._selectMsg = undefined;
                        that._UpdatePage();
                    }
                    else if (_status === 'key_esc') {
                        that._isChangePageInSetting = true;
                        hx.msg.close('confirm', that._selectMsg);
                        that._selectMsg = undefined;
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgStorage',
                            me: that
                        });
                    }
                    else if (_status === 'key_back') {
                        that._strMenu.on();
                        that._strMenu.setItemFocus(ulItemIdx);
                        hx.msg.close('confirm', that._selectMsg);
                        that._selectMsg = undefined;
                        that._UpdatePage();
                    }
                }
            }
            else {
                hx.log('CPgExternalStorage', '_on_PhysicalStorageStatus() Error!! it is not acton of my HDD : ' + ulStorageId);
            }
        };
        CPgExternalStorage.prototype._on_dlVerifyPassword = function (aParam) {
            var that = this, ulItemIdx = 0;
            ulItemIdx = that._conv_itemID_to_index('formatstorage');
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
                    that._strMenu.setItemFocus(ulItemIdx);
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
                var bPbGoing = hx.svc.PLAYBACK.isMpbOnGoing();
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
                that._strMenu.setItemFocus(ulItemIdx);
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
        CPgExternalStorage.prototype._on_alClicked = function (aParam) {
            var that = this, data = aParam.alData, bConsumed = false, ulItemIdx = 0;
            var szItemId = 'undefined';
            if (data.me === that._strMenu) {
                bConsumed = true;
                szItemId = that._conv_index_to_itemID(data.index);
                if (szItemId === 'removestorage') {
                    ulItemIdx = that._conv_itemID_to_index('removestorage');
                    hx.svc.STORAGE_MASS.doDetach({
                        id: that._strId
                    });
                    that.removeProgressMsg = hx.msg('progress', {
                        text: hx.l('LOC_REMOVING_STORAGE_ID'),
                        auto_close: true,
                        timeout: 20000,
                        dia_class: 'osd',
                        callback_fn: removeProgressCb
                    });
                    function removeProgressCb(_status) {
                        if (_status === 'auto_close') {
                            that._strMenu.on();
                            that._strMenu.setItemFocus(ulItemIdx);
                        }
                    }
                }
                else if (szItemId === 'formatstorage') {
                    //TODO : Format Storage
                    ulItemIdx = that._conv_itemID_to_index('formatstorage');
                    if (that._ishdd === false) {
                        hx.msg('response', {
                            text: hx.l('LOC_STORAGE_CANNOT_FORMATTED_ID'),
                            auto_close: true,
                            close_time: 3000
                        });
                        that._strMenu.setItemFocus(ulItemIdx);
                    }
                    else {
                        var has_HotScheduledItem;
                        ulItemIdx = that._conv_itemID_to_index('formatstorage');
                        has_HotScheduledItem = that._check_ScheduledItems_WillbeStarted();
                        if (has_HotScheduledItem) {
                            that.testMsg = hx.msg('response', {
                                text: hx.l('LOC_FORMAT_NOT_ALLOWED_ID'),
                                auto_close: true,
                                close_time: 3000,
                            });
                            that._strMenu.setItemFocus(ulItemIdx);
                        }
                        else {
                            var selectMsg = hx.msg('confirm', {
                                text: hx.l('LOC_FORMAT_THIS_STORAGE_ID'),
                                auto_close: false,
                                //timeout : 5000,
                                btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                                dia_class: 'osd',
                                btn_last_focus: 1,
                                callback_fn: selectfn
                            });
                        }
                    }
                    function selectfn(_status) {
                        if (_status === hx.l('LOC_YES_ID')) {
                            that.pinMsg = hx.msg('pin', {
                                text: hx.l('LOC_ENTER_PASSWORD_ID'),
                                callback_fn: pinfn,
                                auto_close: false,
                                dia_class: 'osd',
                                enable: true
                            });
                        }
                        else if (_status === hx.l('LOC_NO_ID')) {
                            hx.msg.close('confirm', selectMsg);
                            that._strMenu.on();
                            that._strMenu.setItemFocus(ulItemIdx);
                        }
                        else if (_status === 'key_back') {
                            hx.msg.close('confirm', selectMsg);
                            that._strMenu.on();
                            that._strMenu.setItemFocus(ulItemIdx);
                        }
                        else if (_status === 'key_esc') {
                            hx.msg.close('confirm', selectMsg);
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
                            that._strMenu.setItemFocus(ulItemIdx);
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
                else if (szItemId === 'selecthddforpvr') {
                    if (hx.config.useInternalPvr === 0) {
                        that._selectAssignPvrMsg(aParam);
                    }
                }
                else if (szItemId === 'deselecthddforpvr') {
                    if (hx.config.useInternalPvr === 0) {
                        that._selectDeAssignPvrMsg(aParam);
                    }
                }
            }
            return bConsumed;
        };
        CPgExternalStorage.prototype._on_alExit = function (aParam) {
            var that = this, data = aParam.alData, bConsumed = false;
            if (data) {
                if (data.me === that._strMenu) {
                    bConsumed = true;
                    that._isChangePageInSetting = true;
                    that.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgStorage?' + that._strId,
                        me: that
                    });
                }
            }
            return bConsumed;
        };
        CPgExternalStorage.prototype._on_alKey = function (aParam) {
            var that = this, bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    if (aParam.alData) {
                        bConsumed = true;
                        that._isChangePageInSetting = true;
                        that.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgStorage?' + that._strId,
                            me: that
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
        CPgExternalStorage.prototype._conv_itemID_to_index = function (itemID) {
            var that = this, i = 0, ulIndex = -1;
            for (i = 0; i < that._contextListMap.itemId.length; i++) {
                if (itemID === that._contextListMap.itemId[i]) {
                    ulIndex = i;
                    return ulIndex;
                }
            }
            return ulIndex;
        };
        CPgExternalStorage.prototype._conv_index_to_itemID = function (itemIdx) {
            var that = this, i = 0, itemID = 'undefined';
            for (i = 0; i < that._contextListMap.itemId.length; i++) {
                if (i === itemIdx) {
                    itemID = that._contextListMap.itemId[i];
                    return itemID;
                }
            }
            return itemID;
        };
        CPgExternalStorage.prototype._UpdatePage = function () {
            this._isChangePageInSetting = true;
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgStorage?' + this._strId + '=' + 'createpage',
                me: this
            });
        };
        CPgExternalStorage.prototype._changeToLastChannel = function () {
            // tune to available channel
            var result = hx.svc.AV_CTRL.lastChannel();
            if (result === false) {
                hx.svc.AV_CTRL.availableChannel();
            }
        };
        CPgExternalStorage.prototype._showDetailInfo = function (aRetObj) {
            var that = this, info = [], usePercent = 0, availPercent = 0;
            info.length = 0;
            if (parseFloat(aRetObj.usedSize)) {
                usePercent = Math.floor(parseFloat(aRetObj.usedSize) / parseFloat(aRetObj.totalSize) * 100);
                availPercent = 100 - usePercent;
            }
            else {
                usePercent = 0;
                availPercent = 100;
            }
            info.push(aRetObj.totalSize + 'GB');
            if ((hx.config.useInternalPvr === 0) && (that._path === '/internalPvrHDD')) {
                info.push(aRetObj.reservedSize + 'GB');
            }
            else {
                info.push('external');
            }
            info.push(aRetObj.usedSize + 'GB');
            info.push(aRetObj.availableSize + 'GB');
            if (!that._ismadeInfo) {
                $.extend(this, {
                    _strInfo: CPgExternalStorage.strInfo(info),
                    _strGraph: CPgExternalStorage.strGraph(usePercent, availPercent),
                });
                that._txt.$.html(that._label);
                that._$box.append(that._strInfo.$.after(that._strGraph.$));
            }
            else {
                that._strInfo.$.find('dd').eq(0).html(info[0]);
                if ((hx.config.useInternalPvr === 0) && (that._path === '/internalPvrHDD')) {
                    that._strInfo.$.find('dd').eq(1).html(info[1]);
                }
                that._strInfo.$.find('dd').eq(1).html(info[2]);
                that._strInfo.$.find('dd').eq(2).html(info[3]);
                that._strGraph.$.find('em').eq(0).html(usePercent);
                that._strGraph.$.find('em').eq(1).html(availPercent);
            }
            hx.il.drawCanvas(usePercent, usePercent + availPercent, 77, 75, 60, 'divStorageDlgRight');
            that._ismadeInfo = true;
        };
        CPgExternalStorage.prototype._selectAssignPvrMsg = function (aParam) {
            var that = this, ulItemIdx = 0;
            var i = 0, bFindPairingStroage = false, retArray, strId;
            retArray = hx.svc.STORAGE_MASS.getWholeStorageInfo({
                cb: undefined
            });
            bFindPairingStroage = false;
            for (i = 0; i < retArray.length; i++) {
                if (retArray[i].type === 'internalPvrHDD') {
                    bFindPairingStroage = true;
                    break;
                }
                else {
                    bFindPairingStroage = false;
                }
            }
            if (bFindPairingStroage === false) {
                strId = 'LOC_STORAGE_ASSIGN_PVR_MSG_ID';
            }
            else {
                strId = 'LOC_STORAGE_ASSIGN_ANOTHER_PVR_MSG_ID';
            }
            ulItemIdx = that._conv_itemID_to_index('selecthddforpvr');
            that._selectMsg = hx.msg('confirm', {
                text: hx.l(strId),
                auto_close: false,
                btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                btn_last_focus: 1,
                dia_class: 'osd',
                callback_fn: selectfn
            });
            function selectfn(_status) {
                if (_status === hx.l('LOC_YES_ID')) {
                    hx.msg.close('confirm', that._selectMsg);
                    that._selectMsg = undefined;
                    that._actionAssignPvr();
                }
                else if (_status === hx.l('LOC_NO_ID')) {
                    hx.msg.close('confirm', that._selectMsg);
                    that._selectMsg = undefined;
                    that._strMenu.on();
                    that._strMenu.setItemFocus(ulItemIdx);
                }
                else if (_status === 'key_back') {
                    hx.msg.close('confirm', that._selectMsg);
                    that._selectMsg = undefined;
                    that._strMenu.on();
                    that._strMenu.setItemFocus(ulItemIdx);
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('confirm', that._selectMsg);
                    that._selectMsg = undefined;
                    that._isChangePageInSetting = true;
                    that.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgStorage',
                        me: that
                    });
                }
            }
        };
        CPgExternalStorage.prototype._actionAssignPvr = function () {
            var that = this;
            var ulResult = 0, stString, ulItemIdx = 0;
            that.progressMsg = hx.msg('progress', {
                text: hx.l('LOC_PROCESSING_ID'),
                auto_close: false
            });
            //hx.il.criticalTasking.crHDDTasking = true;
            ulResult = hx.svc.STORAGE_MASS.doSwitchExternalHdd({
                id: that._strId,
                assignPvr: true
            });
        };
        CPgExternalStorage.prototype._selectDeAssignPvrMsg = function (aParam) {
            var that = this, ulItemIdx = 0;
            ulItemIdx = that._conv_itemID_to_index('deselecthddforpvr');
            that._selectMsg = hx.msg('confirm', {
                text: hx.l('LOC_STORAGE_DEASSIGN_PVR_MSG_ID'),
                auto_close: false,
                btn_title_arr: [hx.l('LOC_YES_ID'), hx.l('LOC_NO_ID')],
                btn_last_focus: 1,
                dia_class: 'osd',
                callback_fn: selectfn
            });
            function selectfn(_status) {
                if (_status === hx.l('LOC_YES_ID')) {
                    hx.msg.close('confirm', that._selectMsg);
                    that._selectMsg = undefined;
                    that._actionDeAssignPvr();
                }
                else if (_status === hx.l('LOC_NO_ID')) {
                    hx.msg.close('confirm', that._selectMsg);
                    that._selectMsg = undefined;
                    that._strMenu.on();
                    that._strMenu.setItemFocus(ulItemIdx);
                }
                else if (_status === 'key_back') {
                    hx.msg.close('confirm', that._selectMsg);
                    that._selectMsg = undefined;
                    that._strMenu.on();
                    that._strMenu.setItemFocus(ulItemIdx);
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('confirm', that._selectMsg);
                    that._selectMsg = undefined;
                    that._isChangePageInSetting = true;
                    that.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgStorage',
                        me: that
                    });
                }
            }
        };
        CPgExternalStorage.prototype._actionDeAssignPvr = function () {
            var ulResult = 0, stString, ulItemIdx = 0;
            this.progressMsg = hx.msg('progress', {
                text: hx.l('LOC_PROCESSING_ID'),
                auto_close: false
            });
            //hx.il.criticalTasking.crHDDTasking = true;
            ulResult = hx.svc.STORAGE_MASS.doSwitchExternalHdd({
                id: this._strId,
                assignPvr: false
            });
        };
        CPgExternalStorage.mainMenu = function (aParam, aParam2, aParam3) {
            var mainmenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': aParam3
                },
                data: aParam,
                data2: aParam2,
                count: aParam.length,
                fx: function (i, o) {
                    o.html(hx.l(this.data[i]));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return mainmenu;
        };
        CPgExternalStorage.txt = function (aParam, aParam2) {
            return hx.al.component('text', {
                tag: aParam2,
                attr: {
                    'data-langID': aParam,
                    'class': ''
                }
            });
        };
        CPgExternalStorage.strInfo = function (aParam) {
            return hx.al.component('storageInfo', {
                totalStorage: aParam[0],
                systemUsage: aParam[1],
                userUsage: aParam[2],
                available: aParam[3]
            });
        };
        CPgExternalStorage.strGraph = function (aParam, aParam2) {
            return hx.al.component('storageGraph', {
                used: aParam,
                available: aParam2
            });
        };
        CPgExternalStorage.strPincode = function () {
            return hx.al.component('strPincode', {
                baseText: '-',
                count: 4
            });
        };
        return CPgExternalStorage;
    })(__KERNEL_SETTINGS__);
    return CPgExternalStorage;
});
