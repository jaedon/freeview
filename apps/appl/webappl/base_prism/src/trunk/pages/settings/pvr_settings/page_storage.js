var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    // file name : pages/settings/pvr_settings/storage.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var _bActive = false;
    var CPgStorage = (function (_super) {
        __extends(CPgStorage, _super);
        function CPgStorage() {
            _super.call(this);
            this.aniConfig = {
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
                }
            };
            this._labelClass = [];
            this._labelList = [];
            this._typeList = [];
            this._idList = [];
            this._isChangePageInSetting = false;
        }
        CPgStorage.prototype.isActive = function () {
            return _bActive;
        };
        CPgStorage.prototype.create = function (aEntity, aPageUrl) {
            var that = this, dsrvRecordings, retArray;
            var i, label;
            _super.prototype.create.call(this);
            $.extend(that, {
                $menu: $('<article>', {
                    'class': 'menu'
                })
            });
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_REC_TITLESTORAGE_ID');
            }
            $('#set').append(that.$menu);
            hx.al.compose(that);
            dsrvRecordings = hx.svc.RECORDINGS;
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
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageAttached', that, function (aPhysicalStorage) {
                that._on_PhysicalStorageAttached(aPhysicalStorage);
            });
            hx.svc.STORAGE_MASS.addEventCb('PhysicalStorageDetached', that, function (aPhysicalStorage) {
                that._on_PhysicalStorageDetached(aPhysicalStorage);
            });
            retArray = hx.svc.STORAGE_MASS.getWholeStorageInfo({
                cb: undefined
            });
            that._labelClass.length = 0;
            that._labelList.length = 0;
            that._idList.length = 0;
            that._typeList.length = 0;
            _bActive = true;
            for (i = 0; i < retArray.length; i += 1) {
                switch (retArray[i].type) {
                    case 'internalPvrHDD':
                        if (hx.config.useInternalPvr === 1) {
                            that._labelClass.push('hdd');
                            that._labelList.push(hx.l('LOC_INTERNALHDD_ID'));
                        }
                        else {
                            that._labelClass.push('usbp');
                            if (!retArray[i].label || retArray[i].label === '') {
                                label = hx.l('LOC_UNKNOWN_ID');
                            }
                            else {
                                label = retArray[i].label;
                            }
                            that._labelList.push(label);
                        }
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'externalPvrHDD':
                        if (hx.config.useInternalPvr === 1) {
                            that._labelClass.push('dtc');
                            that._labelList.push(hx.l('LOC_DETACHHDD_ID'));
                        }
                        else {
                            that._labelClass.push('usb');
                            if (!retArray[i].label || retArray[i].label === '') {
                                label = hx.l('LOC_UNKNOWN_ID');
                            }
                            else {
                                label = retArray[i].label;
                            }
                            that._labelList.push(label);
                        }
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'externalHDD':
                        that._labelClass.push('usb');
                        if (!retArray[i].label || retArray[i].label === '') {
                            label = hx.l('LOC_UNKNOWN_ID');
                        }
                        else {
                            label = retArray[i].label;
                        }
                        that._labelList.push(label);
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'usbMemory':
                        that._labelClass.push('usb');
                        if (!retArray[i].label || retArray[i].label === '') {
                            label = hx.l('LOC_UNKNOWN_ID');
                        }
                        else {
                            label = retArray[i].label;
                        }
                        that._labelList.push(label);
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'sdMemory':
                        that._labelClass.push('sdm');
                        if (!retArray[i].label || retArray[i].label === '') {
                            label = hx.l('LOC_SDCARD_ID');
                        }
                        else {
                            label = retArray[i].label;
                        }
                        that._labelList.push(label);
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'unknown':
                        break;
                    default:
                        break;
                }
            }
            $.extend(that, {
                _strMainmenu: CPgStorage.mainMenu(that._labelList, "dep1 icoStorage", that._labelClass)
            });
            that.$menu.append(that._strMainmenu.$);
            $title.ani({
                style: that.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
            that.$menu.ani({
                style: that.aniConfig.menu.changeIn,
                trans: {
                    dur: 200,
                    delay: 400
                },
                cb: function () {
                    that.append(that._strMainmenu);
                    that._strMainmenu.on();
                    that._isChangePageInSetting = false;
                    if (aPageUrl) {
                        var ulFocusIdx = 0;
                        if (aPageUrl.query) {
                            var i;
                            for (i = 0; i < that._idList.length; i += 1) {
                                if (aPageUrl.query === that._idList[i]) {
                                    that._strMainmenu.setItemFocus(i);
                                    ulFocusIdx = i;
                                }
                            }
                        }
                        if (retArray.length < 1) {
                            that._isChangePageInSetting = true;
                            that.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgSettings',
                                me: that
                            });
                        }
                        else {
                            if (aPageUrl.param === 'createpage') {
                                that._updatePage_Clicked(ulFocusIdx);
                            }
                        }
                    }
                }
            });
        };
        CPgStorage.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            var dsrvRecordings;
            this.$menu.remove();
            _bActive = false;
            hx.svc.STORAGE_MASS.clearEventCb(this);
            dsrvRecordings = hx.svc.RECORDINGS;
            dsrvRecordings.clearEventCb(this, 'RecordingItemAdded');
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
            this.$menu = undefined;
            this._strMainmenu = undefined;
        };
        CPgStorage.prototype._on_PhysicalStorageAttached = function (aPhysicalStorage) {
            this._update_StorageInfo(aPhysicalStorage);
        };
        CPgStorage.prototype._on_PhysicalStorageDetached = function (aPhysicalStorage) {
            var ulStorageCount = 0;
            ulStorageCount = this._update_StorageInfo(aPhysicalStorage);
            if (ulStorageCount < 1) {
                this._isChangePageInSetting = true;
                this.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: 'CPgSettings',
                    me: this
                });
            }
        };
        CPgStorage.prototype._on_alClicked = function (aParam) {
            var data = aParam.alData, bConsumed = false;
            if (data.me === this._strMainmenu) {
                bConsumed = true;
                if (this._labelClass[data.index] === 'hdd') {
                    this._isChangePageInSetting = true;
                    this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgInternalStorage?' + this._idList[data.index],
                        me: this
                    });
                }
                else {
                    this._isChangePageInSetting = true;
                    this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgExternalStorage' + '/' + this._typeList[data.index] + '?' + this._idList[data.index] + '=' + this._labelList[data.index],
                        me: this
                    });
                }
            }
            return bConsumed;
        };
        CPgStorage.prototype._on_alExit = function (aParam) {
            var that = this, data = aParam.alData, bConsumed = false;
            if (data) {
                if (data.me === that._strMainmenu) {
                    bConsumed = true;
                    that._isChangePageInSetting = true;
                    hx.al.decompose(that);
                    that.$menu.ani({
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
        CPgStorage.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    if (aParam.alData) {
                        if (aParam.alData.me === this._strMainmenu) {
                            bConsumed = true;
                            hx.svc.SETTING_UTIL.setFrontPanelString({
                                text: ''
                            });
                            this.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgLiveController',
                                me: this
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        CPgStorage.prototype._update_StorageInfo = function (aPhysicalStorage) {
            var that = this, i, label, retArray;
            retArray = hx.svc.STORAGE_MASS.getWholeStorageInfo({
                cb: undefined
            });
            that._labelClass.length = 0;
            that._labelList.length = 0;
            that._idList.length = 0;
            that._typeList.length = 0;
            for (i = 0; i < retArray.length; i += 1) {
                switch (retArray[i].type) {
                    case 'internalPvrHDD':
                        if (hx.config.useInternalPvr === 1) {
                            that._labelClass.push('hdd');
                            that._labelList.push(hx.l('LOC_INTERNALHDD_ID'));
                        }
                        else {
                            that._labelClass.push('usbp');
                            if (!retArray[i].label || retArray[i].label === '') {
                                label = hx.l('LOC_UNKNOWN_ID');
                            }
                            else {
                                label = retArray[i].label;
                            }
                            that._labelList.push(label);
                        }
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'externalPvrHDD':
                        if (hx.config.useInternalPvr === 1) {
                            that._labelClass.push('dtc');
                            that._labelList.push(hx.l('LOC_DETACHHDD_ID'));
                        }
                        else {
                            that._labelClass.push('usb');
                            if (!retArray[i].label || retArray[i].label === '') {
                                label = hx.l('LOC_UNKNOWN_ID');
                            }
                            else {
                                label = retArray[i].label;
                            }
                            that._labelList.push(label);
                        }
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'externalHDD':
                        that._labelClass.push('usb');
                        if (!retArray[i].label || retArray[i].label === '') {
                            label = hx.l('LOC_UNKNOWN_ID');
                        }
                        else {
                            label = retArray[i].label;
                        }
                        that._labelList.push(label);
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'usbMemory':
                        that._labelClass.push('usb');
                        if (!retArray[i].label || retArray[i].label === '') {
                            label = hx.l('LOC_UNKNOWN_ID');
                        }
                        else {
                            label = retArray[i].label;
                        }
                        that._labelList.push(label);
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'sdMemory':
                        that._labelClass.push('sdm');
                        if (!retArray[i].label || retArray[i].label === '') {
                            label = hx.l('LOC_UNKNOWN_ID');
                        }
                        else {
                            label = retArray[i].label;
                        }
                        that._labelList.push(label);
                        that._typeList.push(retArray[i].type);
                        that._idList.push(retArray[i].id);
                        break;
                    case 'unknown':
                        break;
                    default:
                        break;
                }
            }
            that._strMainmenu.$.remove();
            that._strMainmenu = CPgStorage.mainMenu(that._labelList, "dep1 icoStorage", that._labelClass);
            that.$menu.append(that._strMainmenu.$);
            that.append(that._strMainmenu);
            that._strMainmenu.on();
            return retArray.length;
        };
        CPgStorage.prototype._updatePage_Clicked = function (aIndex) {
            this._isChangePageInSetting = true;
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgExternalStorage' + '/' + this._typeList[aIndex] + '?' + this._idList[aIndex] + '=' + this._labelList[aIndex],
                me: this
            });
        };
        CPgStorage.mainMenu = function (aParam, aParam2, aParam3) {
            var mainmenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': aParam2
                },
                data: aParam,
                data2: aParam3,
                count: aParam.length,
                fx: function (i, o) {
                    o.html(this.data[i]).addClass(this.data2[i]);
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return mainmenu;
        };
        return CPgStorage;
    })(__KERNEL_SETTINGS__);
    return CPgStorage;
});
