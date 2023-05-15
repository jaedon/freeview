///<reference path="../../../def/sugar.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    //file name : pages/settings/pvr_settings/internal_storage.js
    /*
     * jslint nomen: true, undef: true, sloppy: true
     * */
    var cCPgHiddenSetting = (function (_super) {
        __extends(cCPgHiddenSetting, _super);
        function cCPgHiddenSetting(aEvent) {
            _super.call(this, aEvent);
            this.TMenuList = {
                MENU_PASSWORD_RESET: 0,
                MENU_FUSING_INFO: 1,
                MENU_DB_IN: 2,
                MENU_DB_OUT: 3,
                MENU_LOG_OUT: 4,
                MENU_TEST: 5,
                MENU_WIFI_TEST: 6,
                MENU_SHOW_SERIES_INFO: 7
            };
            this.TMenuItemStrIDList = {
                MENU_ITEM_STRID: 0
            };
            this.THiddenAction = {
                ACTION_HIDDEN_NONE: 0,
                ACTION_HIDDEN_ALL: 1,
                ACTION_HIDDEN_PWRESET: 2
            };
            this.TMenuItemValue_OnOff = {
                VALUE_ON: 0,
                VALUE_OFF: 1
            };
            this.TMenuItemValue_UpdateTime = {
                VALUE_VARIABLE: 0,
                VALUE_FIXED: 1
            };
            this.TMenuItemHiddenFuingIRList = {
                H_ITEM_SN: 0,
                H_ITEM_LANGUAGE: 1,
                H_ITEM_SBOOT: 2,
                H_ITEM_JTAG: 3,
                H_ITEM_SCW: 4,
                H_ITEM_OTP: 5,
                H_ITEM_IR_DEV: 6,
                H_ITEM_IR_KEY: 7,
                H_ITEM_C_RM_FS: 8,
                H_ITEM_M_ID: 9,
                H_ITEM_END_ID: 10
            };
            this._isDestroyed = false;
            this._ulActionType = undefined;
            this._isChangePageInSetting = false;
            this._bActive = false;
            this._copyCount = 0;
            this._contextListMap = [['Reset Password', 'Fusing Info', 'DB in', 'DB out', 'Log out', 'Test', 'Wifi Test', "Series Info"], ['LOC_ON_ID', 'LOC_OFF_ID'], ['LOC_VARIABLE_ID', 'LOC_FIXED_ID']];
        }
        cCPgHiddenSetting.prototype.create = function (aEntity, aPageUrl) {
            hx.logadd("hiddenMenu");
            this._ulActionType = parseInt(aPageUrl.query, 10);
            if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_PWRESET) {
                $.extend(this, {
                    _$setStge: $('<section>', {
                        'class': 'setStge active'
                    }),
                    _title: this._txt('LOC_HIDDEN_SETTINGS_ID', 'h1'),
                    _$menu: $('<article>', {
                        'class': 'menu'
                    }),
                    _$contextMenu: $('<div>', {
                        'class': 'dep2'
                    }),
                    _hiddenMainMenu: this._mainMenu([this._contextListMap[this.TMenuItemStrIDList.MENU_ITEM_STRID][this.TMenuList.MENU_PASSWORD_RESET]])
                });
            }
            else if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_ALL) {
                $.extend(this, {
                    _$setStge: $('<section>', {
                        'class': 'setStge active'
                    }),
                    _title: this._txt('LOC_HIDDEN_SETTINGS_ID', 'h1'),
                    _$menu: $('<article>', {
                        'class': 'menu'
                    }),
                    _$contextMenu: $('<div>', {
                        'class': 'dep2'
                    }),
                    _hiddenMainMenu: this._mainMenu(this._contextListMap[this.TMenuItemStrIDList.MENU_ITEM_STRID])
                });
            }
            this._initMainItemValue();
            $('#set').append(this._$setStge.append(this._title.$.after(this._$menu.append(this._$contextMenu.after(this._hiddenMainMenu.$)))));
            hx.al.compose(this).append(this._hiddenMainMenu);
            this._isDestroyed = false;
            this._bActive = true;
            this._hiddenMainMenu.on();
            this._isChangePageInSetting = false;
            /*
             hx.svc.MEDIA_ITEM_MANAGER.addEventListener({
             event : 'ContentBrowse',
             objTarget : that
             });
             */
            hx.svc.MEDIA_ITEM_MANAGER.addEventCb('ContentOperate', this, this._on_ContentOperate);
        };
        cCPgHiddenSetting.prototype.destroy = function () {
            hx.svc.MEDIA_ITEM_MANAGER.clearEventCb(this);
            this._isDestroyed = true;
            this._bActive = false;
            this._ulActionType = undefined;
            this._$setStge.remove();
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
        };
        cCPgHiddenSetting.prototype.isActive = function () {
            return this._bActive;
        };
        cCPgHiddenSetting.prototype._initMainItemValue = function () {
            /*
             if(that._ulActionType === that.THiddenAction.ACTION_HIDDEN_ALL)
             {
             var noWork;
             }
             */
        };
        cCPgHiddenSetting.prototype._on_alClicked = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
            switch (target) {
                case this._hiddenMainMenu:
                    bConsumed = true;
                    if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_PWRESET) {
                        this._keyActionforHiddenPwReset(aParam);
                    }
                    else if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_ALL) {
                        this._keyActionforHiddenAll(aParam);
                    }
                    break;
                case this._subList:
                    if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_ALL) {
                        bConsumed = true;
                        this._$contextMenu.removeClass('active');
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgHiddenSetting.prototype._on_alExit = function (param) {
            var data = param.alData, bConsumed = false;
            if (data.me === this._hiddenMainMenu) {
                bConsumed = true;
                this._isChangePageInSetting = true;
                this.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: 'CPgSystem',
                    me: this
                });
            }
            return bConsumed;
        };
        cCPgHiddenSetting.prototype._on_alKey = function (aParam) {
            var that = this, bConsumed = false, menuFocusIndex;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_PLAYER:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    bConsumed = true;
                    that._isChangePageInSetting = true;
                    that.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgSystem',
                        me: that
                    });
                    break;
                case hx.key.KEY_POWER:
                    if (hx.il.isCriticalTasking()) {
                        bConsumed = true;
                    }
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    bConsumed = true;
                    if (aParam.alData.me === that._subList) {
                        menuFocusIndex = that._currentSubMenu;
                        that._subList.$.remove();
                        that._$contextMenu.removeClass('active');
                        that._hiddenMainMenu.getItem(menuFocusIndex).removeClass('his');
                        that._hiddenMainMenu.setItemFocus(menuFocusIndex);
                        that._hiddenMainMenu.on();
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgHiddenSetting.prototype._on_ContentOperate = function (aParams) {
            this.onLocalContentOperate(aParams.ticket, aParams.result);
        };
        cCPgHiddenSetting.prototype._keyActionforHiddenPwReset = function (aParam) {
            var data = aParam.alData;
            if (data.index === 0) {
                this._actionPasswordReset(aParam);
            }
        };
        cCPgHiddenSetting.prototype._keyActionforHiddenAll = function (aParam) {
            var data = aParam.alData;
            switch (data.index) {
                case this.TMenuList.MENU_PASSWORD_RESET:
                    this._actionPasswordReset(aParam);
                    break;
                case this.TMenuList.MENU_FUSING_INFO:
                    this._actionFusingInfo(aParam);
                    break;
                case this.TMenuList.MENU_DB_IN:
                    this._actionDBIn(aParam);
                    break;
                case this.TMenuList.MENU_DB_OUT:
                    this._actionDBOut(aParam);
                    break;
                case this.TMenuList.MENU_LOG_OUT:
                    this._actionLOGOut(aParam);
                    break;
                case this.TMenuList.MENU_TEST:
                    this._actionTest(aParam);
                    break;
                case this.TMenuList.MENU_WIFI_TEST:
                    this._actionWifiTest(aParam);
                    break;
                case this.TMenuList.MENU_SHOW_SERIES_INFO:
                    this._actionShowSeriesInfo(aParam);
                    break;
            }
        };
        cCPgHiddenSetting.prototype._actionPasswordReset = function (aParam) {
            var idx = 0;
            hx.svc.PARENTAL_CTRL.setDefaultParentalControlPIN('0000');
            if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_ALL) {
                idx = this.TMenuList.MENU_PASSWORD_RESET;
            }
            this._hiddenMainMenu.on();
            this._hiddenMainMenu.setItemFocus(idx);
            hx.msg('response', {
                text: 'Initialized to "0000".',
                auto_close: true,
                close_time: 3000,
                dia_class: 'res vaTop'
            });
        };
        cCPgHiddenSetting.prototype._actionShowSeriesInfo = function (aParam) {
            var idx = 0, lRet = 0, message;
            hx.config.showSeriesInfo = !hx.config.showSeriesInfo;
            if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_ALL) {
                idx = this.TMenuList.MENU_SHOW_SERIES_INFO;
            }
            this._hiddenMainMenu.on();
            this._hiddenMainMenu.setItemFocus(idx);
            if (hx.config.showSeriesInfo === true) {
                message = "Show Series Info";
            }
            else {
                message = "Hide Series Info";
            }
            hx.msg('response', {
                text: message,
                auto_close: true,
                close_time: 3000,
                dia_class: 'res vaTop'
            });
        };
        cCPgHiddenSetting.prototype._actionFusingInfo = function (aParam) {
            var that = this, confirmMsg, szText = '', hiddenInfo = hx.svc.SETTING_UTIL.getHiddenInfo(), serialNumber = hiddenInfo.serialNumber, lag = hiddenInfo.language, secureBoot = hiddenInfo.scs, jtag = hiddenInfo.jtag, OTPLock = hiddenInfo.otp, secureCW = hiddenInfo.cwe, nandBB = hx.svc.SETTING_UTIL.getNandBadBlocks(), nandBBString = 'NBB LDR:' + nandBB.loader + ' UBI:' + nandBB.ubi + ' UBIFS:' + nandBB.ubifs + '</br>';
            function convertSN(hex) {
                var str = '', i;
                for (i = 0; i < hex.length; i += 2) {
                    str += String.fromCharCode(parseInt(hex.substr(i, 2), 16));
                }
                return str;
            }
            /*
            if (ENV.listCAS.IR === ENV.cas) {
                szText = that._getIRFusinInfo();
            }
            */
            confirmMsg = hx.msg('codeConfirm', {
                html: nandBBString + 'Language : ' + lag + '</br>SerialNumber : ' + convertSN(serialNumber) + '</br>S-BOOT : ' + ((secureBoot === 1) ? 'O' : 'X') + '</br>JTAG : ' + ((jtag === 1) ? 'O' : 'X') + '</br>S-CW : ' + ((secureCW === 1) ? 'O' : 'X') + '</br>OTP : ' + ((OTPLock === 1) ? 'O' : 'X'),
                auto_close: false,
                btn_title_arr: [hx.l('LOC_OK_ID')],
                dia_class: 'osd',
                nolinejump: false,
                callback_fn: msgActionConfirmfn
            });
            function msgActionConfirmfn(_status) {
                var idx;
                if (that._ulActionType === that.THiddenAction.ACTION_HIDDEN_ALL) {
                    idx = that.TMenuList.MENU_FUSING_INFO;
                }
                if (_status === hx.l('LOC_OK_ID')) {
                    hx.msg.close('codeConfirm', confirmMsg);
                    that._hiddenMainMenu.setItemFocus(idx);
                    that._hiddenMainMenu.on();
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('codeConfirm', confirmMsg);
                    that._hiddenMainMenu.setItemFocus(idx);
                    that._hiddenMainMenu.on();
                }
                else if (_status === 'key_back') {
                    hx.msg.close('codeConfirm', confirmMsg);
                    that._hiddenMainMenu.setItemFocus(idx);
                    that._hiddenMainMenu.on();
                }
            }
        };
        /*
        _getIRFusinInfo () {
            var that = this, szFusingInfo, aritem = [], szText = '', szLanguage = '';
            var ucSpace = '&nbsp;', szTextLine1 = '', szTextLine2 = '', szTextLine3 = '', szTextLine4 = '', szTextLine5 = '', szTextLine6 = '', szTextLine7 = '', szTextLine8 = '';
    
            szFusingInfo = hx.svc.SETTING_UTIL.getIRFusinInfo();
    
            aritem = ['S N : ', 'L a n g u a g e : ', 'S - B O O T : ', 'J T A G : ', 'S- C W : ', ' O T P : ', 'I R - D E V : ', 'I R - K E Y : ', 'C - R M - F S : ', 'M - ID :'];
            szTextLine1 = aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_SN] + szFusingInfo.serialNumber;
            szLanguage = szFusingInfo.language;
            szTextLine2 = aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_LANGUAGE] + szLanguage.toUpperCase();
            szTextLine3 = aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_SBOOT] + szFusingInfo.sboot + ucSpace + ucSpace + ucSpace + aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_JTAG] + szFusingInfo.jtag;
            szTextLine4 = aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_SCW] + szFusingInfo.scw + ucSpace + ucSpace + ucSpace + ucSpace + ucSpace + aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_OTP] + szFusingInfo.otp;
            szTextLine5 = aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_IR_DEV] + szFusingInfo.ir_mancode + ' ' + szFusingInfo.ir_hwcode + ' ' + szFusingInfo.ir_variant;
            szTextLine6 = aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_IR_KEY] + szFusingInfo.ir_sysid + ' ' + szFusingInfo.ir_sigver + ' ' + szFusingInfo.ir_keyver;
            szTextLine7 = aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_C_RM_FS];
            szTextLine8 = aritem[that.TMenuItemHiddenFuingIRList.H_ITEM_M_ID];
    
            szText = szTextLine1 + '<br>' + szTextLine2 + '<br>' + szTextLine3 + '<br>' + szTextLine4 + '<br>' + szTextLine5 + '<br>' + szTextLine6 + '<br>' + szTextLine7 + '<br>' + szTextLine8;
    
            return szText;
        }   */
        cCPgHiddenSetting.prototype._actionDBIn = function (aParam) {
            var idx = 0, lRet = 0;
            lRet = hx.svc.MEDIA_ITEM_MANAGER.getPluginInstance("ContentManager").copyContent({
                src: '/media/drive1/humaxtv/dlogger/',
                dest: '/var/lib/humaxtv/',
                ext: ''
            });
            lRet = hx.svc.MEDIA_ITEM_MANAGER.getPluginInstance("ContentManager").copyContent({
                src: '/media/drive1/humaxtv_backup/dlogger/',
                dest: '/var/lib/humaxtv_backup/',
                ext: ''
            });
            if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_ALL) {
                idx = this.TMenuList.MENU_DB_IN;
            }
            this._hiddenMainMenu.on();
            this._hiddenMainMenu.setItemFocus(idx);
            hx.msg('response', {
                text: 'Database IN',
                auto_close: true,
                close_time: 3000,
                dia_class: 'res vaTop'
            });
        };
        cCPgHiddenSetting.prototype._actionDBOut = function (aParam) {
            var idx = 0, lRet = 0;
            lRet = hx.svc.MEDIA_ITEM_MANAGER.getPluginInstance("ContentManager").copyContent({
                src: '/var/lib/humaxtv/dlogger/',
                dest: '/media/drive1/humaxtv/',
                ext: ''
            });
            lRet = hx.svc.MEDIA_ITEM_MANAGER.getPluginInstance("ContentManager").copyContent({
                src: '/var/lib/humaxtv_backup/dlogger/',
                dest: '/media/drive1/humaxtv_backup/',
                ext: ''
            });
            if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_ALL) {
                idx = this.TMenuList.MENU_DB_OUT;
            }
            this._hiddenMainMenu.on();
            this._hiddenMainMenu.setItemFocus(idx);
            hx.msg('response', {
                text: 'Database OUT',
                auto_close: true,
                close_time: 3000,
                dia_class: 'res vaTop'
            });
        };
        cCPgHiddenSetting.prototype._actionLOGOut = function (aParam) {
            var idx = 0, lRet = 0;
            lRet = hx.svc.MEDIA_ITEM_MANAGER.getPluginInstance("ContentManager").copyContent({
                src: '/var/lib/humaxtv_user/log/',
                dest: '/media/drive1/',
                ext: ''
            });
            if (this._ulActionType === this.THiddenAction.ACTION_HIDDEN_ALL) {
                idx = this.TMenuList.MENU_LOG_OUT;
            }
            this._hiddenMainMenu.on();
            this._hiddenMainMenu.setItemFocus(idx);
            hx.msg('response', {
                text: 'LOG OUT',
                auto_close: true,
                close_time: 3000,
                dia_class: 'res vaTop'
            });
        };
        cCPgHiddenSetting.prototype._actionTest = function (aParam) {
            var appName = "Test APP", appUrl = '/media/drive1/testapp/testpage.h' + 'tml', fnDestroyApp;
            // hide full path
            if (hx.emulMode) {
                location.href = 'test/testpage.html';
            }
            else {
                fnDestroyApp = function (appInfo) {
                    hx.svc.APP_MGR.destroyApplication({
                        name: appName
                    });
                };
                hx.svc.APP_MGR.createApplication({
                    name: appName,
                    url: appUrl,
                    onKeyDown: function (appInfo, keyCode) {
                        hx.log('error', 'Child key down: ' + keyCode);
                        if (keyCode === hx.key.KEY_ESCAPE) {
                            fnDestroyApp(appInfo);
                        }
                    }
                });
            }
        };
        cCPgHiddenSetting.prototype._actionWifiTest = function (aParam) {
            var that = this, wifi = hx.svc.SETTING_UTIL.getNetConnectedInfo(), status = wifi.wirelessDevState, ssid = 'O', channel = 'x', ip = hx.svc.SETTING_UTIL.getIpAddress().mIpAddress, rssi = wifi.waveStrength, noise = 'x';
            var confirmMsg;
            confirmMsg = hx.msg('codeConfirm', {
                html: 'STATUS : ' + status + '</br>RSSI : ' + rssi,
                auto_close: false,
                btn_title_arr: [hx.l('LOC_OK_ID')],
                dia_class: 'osd',
                nolinejump: false,
                callback_fn: msgActionConfirmfn
            });
            function msgActionConfirmfn(_status) {
                var idx;
                if (that._ulActionType === that.THiddenAction.ACTION_HIDDEN_ALL) {
                    idx = that.TMenuList.MENU_WIFI_TEST;
                }
                if (_status === hx.l('LOC_OK_ID')) {
                    hx.msg.close('codeConfirm', confirmMsg);
                    that._hiddenMainMenu.setItemFocus(idx);
                    that._hiddenMainMenu.on();
                }
                else if (_status === 'key_esc') {
                    hx.msg.close('codeConfirm', confirmMsg);
                    that._hiddenMainMenu.setItemFocus(idx);
                    that._hiddenMainMenu.on();
                }
                else if (_status === 'key_back') {
                    hx.msg.close('codeConfirm', confirmMsg);
                    that._hiddenMainMenu.setItemFocus(idx);
                    that._hiddenMainMenu.on();
                }
            }
        };
        cCPgHiddenSetting.prototype.onLocalContentOperate = function (aTicket, aResult) {
            var focusedItem, focusedItemTxt;
            hx.log('hiddenMenu', '[hidden_onLocalContentOperate] ticket: ' + aTicket);
            hx.log('hiddenMenu', '[hidden_onLocalContentOperate] result: ' + aResult);
            //a._ticketManager.updateResult(ticket, result);
            focusedItem = this._hiddenMainMenu.getFocusedItem();
            focusedItemTxt = focusedItem[0].textContent;
            if (aResult === 3) {
                hx.msg('response', {
                    text: 'Fail',
                    auto_close: true,
                    close_time: 3000,
                    dia_class: 'res vaTop'
                });
            }
            if (aResult === 2 && focusedItemTxt === "DB in") {
                this._copyCount += 1;
            }
            if (this._copyCount === 2 && focusedItemTxt === "DB in") {
                hx.svc.SETTING_UTIL.doReboot();
            }
        };
        cCPgHiddenSetting.prototype._mainMenu = function (aParam) {
            var mainmenu = hx.al.component('settingsList', {
                itemAttr: {},
                attr: {
                    'class': 'dep1'
                },
                data: aParam,
                count: aParam.length,
                fx: function (i, o) {
                    o.html(this.data[i]).append($('<em>'));
                },
                keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
            });
            return mainmenu;
        };
        cCPgHiddenSetting.prototype._txt = function (aParam, aParam2) {
            return hx.al.component('text', {
                tag: aParam2,
                attr: {
                    'data-langID': aParam
                }
            });
        };
        cCPgHiddenSetting.prototype._makesubList = function (aParam) {
            var ulSubItemListindex = 0, subFocusIndex = 0;
            var dataList = this._contextListMap[ulSubItemListindex];
            this._subList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: subFocusIndex,
                displayCount: dataList.length
            });
            this._$contextMenu.addClass('active');
            this._hiddenMainMenu.getItem(aParam).addClass('his');
            this.append(this._subList);
            this._$contextMenu.append(this._subList.$);
            this._subList.on();
            this._subList.bind(dataList);
            this._subList.draw();
        };
        return cCPgHiddenSetting;
    })(__PAGE_KERNEL__);
    return cCPgHiddenSetting;
});
