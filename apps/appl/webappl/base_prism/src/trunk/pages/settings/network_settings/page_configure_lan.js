var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings"], function (require, exports, __KERNEL_SETTINGS__) {
    // file name : pages/settings/network_settings/configure_lan.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgConfigureLAN = (function (_super) {
        __extends(cCPgConfigureLAN, _super);
        function cCPgConfigureLAN() {
            _super.call(this);
            this._isdhcp = null;
            this.whereIp = 0;
        }
        cCPgConfigureLAN.prototype.create = function () {
            var _this = this;
            _super.prototype.create.call(this);
            var retAddr;
            $.extend(this, {
                $menu: $('<article>', {
                    'class': 'menu wrap'
                }),
                _$contextMenu: $('<div>', {
                    'class': 'dep2'
                }),
                _confLanMainmenu: hx.al.CPgConfigureLAN.mainMenu(hx.al.CPgConfigureLAN.confLanMenu, ['', '0.0.0.0', '0.0.0.0', '0.0.0.0', '0.0.0.0', '0.0.0.0'], "dep1", 'notroll'),
                _confLanSubmenu: hx.al.CPgConfigureLAN.subMenu(hx.al.CPgConfigureLAN.confLanSubmenu),
                _applyBtn: hx.al.CPgConfigureLAN.button('LOC_APPLY_ID'),
                $ipAddr: $('<article>', {
                    'id': 'DIA',
                    'class': 'osd',
                    'style': 'display:none'
                }),
                $pasMulti: $('<div>', {
                    'class': 'pas multi',
                    'style': 'top:253px'
                }),
                _iptxt: hx.al.CPgConfigureLAN.nettxt('LOC_ENTER_THE_IP_ADDRESS_ID'),
                _numdot: hx.al.CPgConfigureLAN.numbers()
            });
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_CONFIGURE_LAN_01_ID');
            }
            hx.svc.SETTING_UTIL.addEventCb('tryConnection', this, function (aResult) {
                _this._on_tryConnection(aResult);
            });
            retAddr = hx.svc.SETTING_UTIL.getallAddress();
            this.drawAddress(retAddr);
            var $set = $('#set');
            $set.append(this.$menu.append(this._$contextMenu.append(this._confLanSubmenu.$).after(this._confLanMainmenu.$.after(this._applyBtn.$))));
            $set.after(this.$ipAddr.append(this.$pasMulti.append(this._iptxt.$.after(this._numdot.$))));
            hx.al.compose(this);
            this._confLanSubmenu.bind(hx.al.CPgConfigureLAN.confLanSubmenu);
            this._confLanSubmenu.draw();
            $title.ani({
                style: this.aniConfig.fadeIn,
                trans: { dur: 100, delay: 300 }
            });
            this.$menu.ani({
                style: this.aniConfig.menu.changeIn,
                trans: { dur: 200, delay: 400 },
                cb: function () {
                    _this.append(_this._confLanMainmenu).append(_this._confLanSubmenu).append(_this._applyBtn).append(_this._numdot._firstNum).append(_this._numdot._secondNum).append(_this._numdot._thirdNum).append(_this._numdot._fourthNum);
                    _this._confLanMainmenu.on();
                    _this._isChangePageInSetting = false;
                }
            });
        };
        cCPgConfigureLAN.prototype.destroy = function () {
            _super.prototype.destroy.call(this);
            hx.svc.SETTING_UTIL.clearEventCb(this);
            this.$menu.remove();
            this.$ipAddr.remove();
            this._isdhcp = null;
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
            this.$menu = undefined;
            this._$contextMenu = undefined;
            this._confLanMainmenu = undefined;
            this._confLanSubmenu = undefined;
            this._applyBtn = undefined;
            this.$ipAddr = undefined;
            this.$pasMulti = undefined;
            this._iptxt = undefined;
            this._numdot = undefined;
        };
        cCPgConfigureLAN.prototype.drawAddress = function (aRetAddr) {
            var i;
            var type = 'DHCP', ipAddr = '', netmask = '', gateway = '', dns1st = '', dns2nd = '';
            if (aRetAddr) {
                type = aRetAddr.type || 'DHCP';
                ipAddr = aRetAddr.ipAddress || '';
                netmask = aRetAddr.netmask || '';
                gateway = aRetAddr.gateway || '';
                dns1st = aRetAddr.dns1st || '';
                dns2nd = aRetAddr.dns2nd || '';
            }
            this._confLanMainmenu.getItem(0).find('em').html(type);
            this._confLanMainmenu.getItem(1).find('em').html(ipAddr);
            this._confLanMainmenu.getItem(2).find('em').html(netmask);
            this._confLanMainmenu.getItem(3).find('em').html(gateway);
            this._confLanMainmenu.getItem(4).find('em').html(dns1st);
            this._confLanMainmenu.getItem(5).find('em').html(dns2nd);
            this._isdhcp = (type === 'DHCP') ? true : false;
            if (this._isdhcp === false) {
                for (i = 1; i < 6; i += 1) {
                    this._confLanMainmenu.getItem(i).removeClass('dis');
                }
            }
        };
        cCPgConfigureLAN.prototype._showIpInput = function () {
            this.$ipAddr.css('display', 'block');
            hx.il.ani.action({
                type: 'in',
                targets: this.$pasMulti
            });
            this._numdot._firstNum.on();
            this._numdot._firstNum.position(0);
        };
        cCPgConfigureLAN.prototype._on_tryConnection = function (aResult) {
            var retAddr;
            if (aResult === 'success') {
                hx.msg.close('progress', this.progressMsg);
                /*
                hx.msg('response', {
                    text : hx.l('LOC_NETWORK_CON_SUCCESS_ID'),
                    auto_close : true,
                    close_time : 3000,
                    dia_class : 'res vaTop'
                });
                */
                retAddr = hx.svc.SETTING_UTIL.getallAddress();
                this.drawAddress(retAddr);
            }
            else if (aResult === 'fail') {
                hx.msg.close('progress', this.progressMsg);
                hx.msg('response', {
                    text: hx.l('LOC_NETWORK_CON_FAILED_ID'),
                    auto_close: true,
                    close_time: 3000,
                    dia_class: 'res vaTop'
                });
            }
            this._isChangePageInSetting = true;
            this.sendEventToEntity({
                alEvent: 'alChangePage',
                target: 'CPgNetworkConnectionStatus',
                me: this
            });
        };
        cCPgConfigureLAN.prototype.drawOneAddr = function (aAddress) {
            var ipArr = [];
            ipArr = aAddress.split(".");
            this._numdot.setFirstNumValue(ipArr[0]);
            this._numdot.setSecondNumValue(ipArr[1]);
            this._numdot.setThirdNumValue(ipArr[2]);
            this._numdot.setFourthNumValue(ipArr[3]);
            this._showIpInput();
        };
        cCPgConfigureLAN.prototype._on_alNumberUnderflow = function (aParam) {
            switch (aParam.alData.me) {
                case this._numdot._firstNum:
                    this._numdot._firstNum.position(0);
                    break;
                case this._numdot._secondNum:
                    this._numdot._firstNum.on();
                    this._numdot._firstNum.position(this._numdot._inputNumCnt - 1);
                    break;
                case this._numdot._thirdNum:
                    this._numdot._secondNum.on();
                    this._numdot._secondNum.position(this._numdot._inputNumCnt - 1);
                    break;
                case this._numdot._fourthNum:
                    this._numdot._thirdNum.on();
                    this._numdot._thirdNum.position(this._numdot._inputNumCnt - 1);
                    break;
            }
        };
        cCPgConfigureLAN.prototype._on_alNumberOverflow = function (aParam) {
            switch (aParam.alData.me) {
                case this._numdot._firstNum:
                    this._numdot._secondNum.on();
                    this._numdot._secondNum.position(0);
                    break;
                case this._numdot._secondNum:
                    this._numdot._thirdNum.on();
                    this._numdot._thirdNum.position(0);
                    break;
                case this._numdot._thirdNum:
                    this._numdot._fourthNum.on();
                    this._numdot._fourthNum.position(0);
                    break;
                case this._numdot._fourthNum:
                    this._numdot._fourthNum.position(this._numdot._inputNumCnt - 1);
                    break;
            }
        };
        cCPgConfigureLAN.prototype._on_algetIpAddr = function (aParam) {
            var number = "";
            this._numdot._firstNum.clear();
            this._numdot._secondNum.clear();
            this._numdot._thirdNum.clear();
            this._numdot._fourthNum.clear();
            number = this._numdot.getAllNumValue();
            switch (this._whereIp) {
                case 1:
                    //IP Address
                    hx.svc.SETTING_UTIL.setIpAddress({
                        address: number
                    });
                    this._confLanMainmenu.getItem(1).find('em').html(number);
                    this.$ipAddr.css('display', 'none');
                    this.$pasMulti.removeClass('aniActive');
                    this._confLanMainmenu.on();
                    this._confLanMainmenu.setItemFocus(1);
                    break;
                case 2:
                    //Netmask
                    hx.svc.SETTING_UTIL.setNetmask({
                        address: number
                    });
                    this._confLanMainmenu.getItem(2).find('em').html(number);
                    this.$ipAddr.css('display', 'none');
                    this.$pasMulti.removeClass('aniActive');
                    this._confLanMainmenu.on();
                    this._confLanMainmenu.setItemFocus(2);
                    break;
                case 3:
                    //Gateway address
                    hx.svc.SETTING_UTIL.setGateway({
                        address: number
                    });
                    this._confLanMainmenu.getItem(3).find('em').html(number);
                    this.$ipAddr.css('display', 'none');
                    this.$pasMulti.removeClass('aniActive');
                    this._confLanMainmenu.on();
                    this._confLanMainmenu.setItemFocus(3);
                    break;
                case 4:
                    //primary DNS
                    hx.svc.SETTING_UTIL.setDns1st({
                        address: number
                    });
                    this._confLanMainmenu.getItem(4).find('em').html(number);
                    this.$ipAddr.css('display', 'none');
                    this.$pasMulti.removeClass('aniActive');
                    this._confLanMainmenu.on();
                    this._confLanMainmenu.setItemFocus(4);
                    break;
                case 5:
                    //secondary DNS
                    hx.svc.SETTING_UTIL.setDns2nd({
                        address: number
                    });
                    this._confLanMainmenu.getItem(5).find('em').html(number);
                    this.$ipAddr.css('display', 'none');
                    this.$pasMulti.removeClass('aniActive');
                    this._confLanMainmenu.on();
                    this._confLanMainmenu.setItemFocus(5);
                    break;
            }
        };
        cCPgConfigureLAN.prototype._on_alClicked = function (aParam) {
            var that = this, data = aParam.alData, bConsumed = false, addr;
            var i;
            if (data.me === that._confLanMainmenu) {
                bConsumed = true;
                that._confLanMainmenu.off();
                if (data.index === 0) {
                    that._confLanMainmenu.getItem(0).addClass('his');
                    that._$contextMenu.addClass('active');
                    that._$contextMenu.ani({
                        style: that.aniConfig.dep2.fadeIn,
                        trans: { dur: 200, delay: 100 },
                        cb: function () {
                            that._confLanSubmenu.on();
                            if (that._isdhcp === false) {
                                that._confLanSubmenu.getItem(0).removeClass('on');
                                that._confLanSubmenu.getItem(1).addClass('on');
                            }
                        }
                    });
                }
                else {
                    that._whereIp = data.index;
                    switch (that._whereIp) {
                        case 1:
                            addr = hx.svc.SETTING_UTIL.getIpAddress();
                            that.drawOneAddr(addr);
                            break;
                        case 2:
                            addr = hx.svc.SETTING_UTIL.getNetmask();
                            that.drawOneAddr(addr);
                            break;
                        case 3:
                            addr = hx.svc.SETTING_UTIL.getGateway();
                            that.drawOneAddr(addr);
                            break;
                        case 4:
                            addr = hx.svc.SETTING_UTIL.getDns1st();
                            that.drawOneAddr(addr);
                            break;
                        case 5:
                            addr = hx.svc.SETTING_UTIL.getDns2nd();
                            that.drawOneAddr(addr);
                            break;
                    }
                }
            }
            else if (data.me === that._confLanSubmenu) {
                bConsumed = true;
                that._confLanSubmenu.off();
                that._$contextMenu.ani({
                    style: that.aniConfig.dep2.fadeOut,
                    trans: { dur: 150, delay: 75 },
                    cb: function () {
                        if (data.index === 0) {
                            for (i = 1; i < 6; i += 1) {
                                that._confLanMainmenu.getItem(i).addClass('dis');
                            }
                            that._confLanMainmenu.getItem(0).find('em').html(hx.l('LOC_DHCP_ID'));
                            that._isdhcp = true;
                        }
                        else if (data.index === 1) {
                            for (i = 1; i < 6; i += 1) {
                                that._confLanMainmenu.getItem(i).removeClass('dis');
                            }
                            that._confLanMainmenu.getItem(0).find('em').html(hx.l('LOC_MANUAL_ID'));
                            that._isdhcp = false;
                        }
                        that._confLanMainmenu.getItem(0).removeClass('his');
                        that._$contextMenu.removeClass('active');
                        that._confLanMainmenu.on();
                    }
                });
            }
            else if (data.me === that._applyBtn) {
                bConsumed = true;
                that.progressMsg = hx.msg('progress', {
                    text: hx.l('LOC_MESG_5986_ID'),
                    _use_backkey: true,
                    auto_close: false,
                    callback_fn: function (_status) {
                        if (_status === 'key_back' || _status === 'key_esc') {
                            hx.svc.SETTING_UTIL.stopConnecting();
                        }
                    }
                });
                if (that._isdhcp === true) {
                    hx.svc.SETTING_UTIL.ConnectionTry();
                }
                else {
                    hx.svc.SETTING_UTIL.manualConfig();
                }
            }
            return bConsumed;
        };
        cCPgConfigureLAN.prototype._on_alScrollup = function (aParam) {
            var data = aParam.alData, bConsumed = false;
            if (data.me === this._confLanMainmenu) {
                bConsumed = true;
                this._applyBtn.on();
                this._applyBtn.$.addClass('on');
            }
            else if (data.me === this._applyBtn) {
                bConsumed = true;
                this._applyBtn.$.removeClass('on');
                this._confLanMainmenu.on();
                if (this._isdhcp === false) {
                    this._confLanMainmenu.setItemFocus(5);
                }
            }
            return bConsumed;
        };
        cCPgConfigureLAN.prototype._on_alScrolldown = function (aParam) {
            var data = aParam.alData, bConsumed = false;
            if (data.me === this._confLanMainmenu) {
                bConsumed = true;
                this._applyBtn.on();
                this._applyBtn.$.addClass('on');
            }
            else if (data.me === this._applyBtn) {
                bConsumed = true;
                this._applyBtn.$.removeClass('on');
                this._confLanMainmenu.on();
            }
            return bConsumed;
        };
        cCPgConfigureLAN.prototype._on_alExit = function (aParam) {
            var _this = this;
            var data = aParam.alData, bConsumed = false;
            if (data) {
                if ((data.me === this._confLanMainmenu) || (data.me === this._applyBtn)) {
                    bConsumed = true;
                    this._isChangePageInSetting = true;
                    hx.al.decompose(this);
                    this.$menu.ani({
                        style: this.aniConfig.menu.changeOut3,
                        trans: { dur: 200, delay: 100 },
                        cb: function () {
                            _this.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgSettings',
                                me: _this
                            });
                        }
                    });
                }
                else if ((data.me === this._numdot._firstNum) || (data.me === this._numdot._secondNum) || (data.me === this._numdot._thirdNum) || (data.me === this._numdot._fourthNum)) {
                    bConsumed = true;
                    this._numdot._firstNum.clear();
                    this._numdot._secondNum.clear();
                    this._numdot._thirdNum.clear();
                    this._numdot._fourthNum.clear();
                    this.$ipAddr.css('display', 'none');
                    this.$pasMulti.removeClass('aniActive');
                    this._confLanMainmenu.on();
                    switch (this._whereIp) {
                        case 1:
                            this._confLanMainmenu.setItemFocus(1);
                            break;
                        case 2:
                            this._confLanMainmenu.setItemFocus(2);
                            break;
                        case 3:
                            this._confLanMainmenu.setItemFocus(3);
                            break;
                        case 4:
                            this._confLanMainmenu.setItemFocus(4);
                            break;
                        case 5:
                            this._confLanMainmenu.setItemFocus(5);
                            break;
                        default:
                            this._confLanMainmenu.setItemFocus(0);
                    }
                }
            }
            return bConsumed;
        };
        cCPgConfigureLAN.prototype._on_alKey = function (aParam) {
            var _this = this;
            var bConsumed = false;
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    if (aParam.alData) {
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
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    if (aParam.alData) {
                        bConsumed = true;
                        if (aParam.alData.me === this._confLanSubmenu) {
                            this._confLanSubmenu.off();
                            this._$contextMenu.ani({
                                style: this.aniConfig.dep2.fadeOut,
                                trans: { dur: 150, delay: 75 },
                                cb: function () {
                                    _this._confLanMainmenu.getItem(0).removeClass('his');
                                    _this._$contextMenu.removeClass('active');
                                    _this._confLanMainmenu.on();
                                }
                            });
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        return cCPgConfigureLAN;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgConfigureLAN = cCPgConfigureLAN;
    hx.al.CPgConfigureLAN.confLanMenu = ['LOC_CONFIP_ID', 'LOC_IPADDR_ID', 'LOC_NETMASK_ID', 'LOC_GATEADDR_ID', 'LOC_PRIDNS_ID', 'LOC_SECONDNS_ID'];
    hx.al.CPgConfigureLAN.confLanSubmenu = ['LOC_DHCP_ID', 'LOC_MANUAL_ID'];
    hx.al.CPgConfigureLAN.numbers = function () {
        var numbers = hx.al.component('NetworkAddressType', {
            attr: {
                'class': 'wrap_pw'
            }
        });
        return numbers;
    };
    hx.al.CPgConfigureLAN.mainMenu = function (aParam, aParam2, aParam3, aParam4) {
        var mainmenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': aParam3
            },
            roll: aParam4,
            data: aParam,
            data2: aParam2,
            count: aParam.length,
            fx: function (i, o) {
                o.html(hx.l(this.data[i])).append($('<em>').html(this.data2[i]));
                if (i > 0) {
                    o.addClass('dis');
                }
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return mainmenu;
    };
    hx.al.CPgConfigureLAN.subMenu = function (aParam) {
        var submenu = hx.al.component('simpleMenu', {
            attr: {
                'style': 'display:block;',
                'class': 'subList'
            },
            currentindex: 0,
            displayCount: aParam.length
        });
        return submenu;
    };
    hx.al.CPgConfigureLAN.button = function (aParam) {
        return (new hx.al.component('button', {
            attr: {
                'class': 'button',
                'data-langID': aParam
            },
            usePointerKey: true
        }));
    };
    hx.al.CPgConfigureLAN.nettxt = function (aParam) {
        return hx.al.component('text', {
            tag: 'p',
            attr: {
                'data-langID': aParam
            }
        });
    };
    hx.al.CPgConfigureLAN.prototype.aniConfig = {
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
    return cCPgConfigureLAN;
});
