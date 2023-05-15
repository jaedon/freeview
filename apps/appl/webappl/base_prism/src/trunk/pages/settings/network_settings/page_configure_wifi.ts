///<reference path="../../../def/sugar.d.ts" />
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
declare var hx;

// file name : pages/settings/network_settings/configure_wifi.js
/*jslint sloppy:true, nomen:true, vars:true */
class cCPgConfigureWiFi extends __KERNEL_SETTINGS__{
    aniConfig : any;
    apList : any;
    progressMsg : any;
    apProgressMsg : any;
    passwdTmp : any;
    $msg2 : any;
    $res2 : any;
    setItemFocus : any;

    _currentConn : number = undefined;
    _selectedConn : number = undefined;
    _status : string = "";
    _isChangePageInSetting : boolean = false;
    _securityTypeValue : string = hx.l('LOC_NONE_ID');
    _securityValueIndex : number = 0;
    _networkName : string = '';
    _password : string = '';
    _hiddenConnect : boolean = false;
    _hiddenNetAppendFlag : boolean = false;
    _connectBtn : any;
    _cancelBtn : any;
    _backBtn : any;
    _securityMenu : any;
    _passwdInput : any;
    _networkNameInput : any;
    _subList : any;
    _$setHiddenNetStge : any;
    _$optSection : any;
    _securitySubList : any;
    _$inputArea2 : any;
    _inputText : string;
    _inputText2 : string;
    _$securitySubMenu : any;
    _option : any;
    _$form : any;
    _$form2 : any;
    $menu : any;
    _wifitxt : any;
    _msgtxt2 : any;
    _$inputTitle : any;
    _$inputTitle2 : any;
    _$inputList : any;
    _$wrapInput : any;
    _$inputArea : any;
    _$btnWrap : any;
    _$hiddenNetStatus : any;
    _$titleHiddenNetArea : any;
    _$securityMenu : any;
    wirelessSecurityType = ['WEP 64-bit Hex', 'WEP 64-bit ASCII', 'WEP 128-bit Hex', 'WEP 128-bit ASCII', 'WPA-PSK(TKIP)', 'WPA-PSK(AES)', 'WPA2-PSK(TKIP+AES)', 'WPA2-PSK(AES)'];

    wirelessSecurityTypeValue = [[0, 0, 0], [1, 0, 2], [1, 0, 1], [1, 0, 4], [1, 0, 3], [2, 1, 5], [2, 1, 6], [3, 1, 7], [3, 1, 6]];

    constructor() {
        super();
    }

    create () {
        super.create();
        var that = this;
        var netConnectedInfo : any;

        $.extend(that, {
            $menu : $('<article>', {
                'class' : 'menu wrap'
            }),
            _wifitxt : hx.al.CPgConfigureWiFi.nettxt('LOC_SELECT_WIRELESS_NETWORK_ID'),

            $msg2 : $('<article>', {
                'id' : 'DIA',
                'style' : 'display:none'
            }),
            $res2 : $('<div>', {
                'class' : 'pro vaTop line1'
            }),
            _$optSection : $('<section>', {
                'id' : 'opt'
            }),
            _option : hx.al.CPgConfigureWiFi.option('LOC_HIDDEN_NETWORK_ID'),
            _msgtxt2 : hx.al.CPgConfigureWiFi.nettxt('LOC_MESG_5986_ID'),
            _$setHiddenNetStge : $('<section>', {
                'class' : 'setStge'
            }),
            _$hiddenNetStatus : $('<h1>', {
                'html' : hx.l('LOC_HIDDEN_NETWORK_ID')
            }),
            _$titleHiddenNetArea : $('<p>', {
                'class' : 'tit area1 point',
                'html' : hx.l('LOC_ENTER_THE_NAME_HIDDEN_NETWORK_ID')
            }),
            _$securityMenu : $('<article>', {
                'class' : 'menu'
            }),
            _$securitySubMenu : $('<section>', {
                'class' : 'dep2'
            }),
            _$wrapInput : $('<div>', {
                'class' : 'content-wrapper',
                'style' : 'top:100px;'
            }),
            _$inputList : $('<dl>'),
            _$inputTitle : $('<dt>', {
                'html' : hx.l('LOC_NETWORK_NAME_ID').toUpperCase()
            }),
            _$inputArea : $('<dd>', {
                'class' : 'point'
            }),
            _$form : $('<form>', {
                'method' : 'post',
            }),
            _$inputList2 : $('<dl>'),
            _$inputTitle2 : $('<dt>', {
                'html' : hx.l('LOC_PASSWORD_ID')
            }),
            _$inputArea2 : $('<dd>', {
                'class' : 'point'
            }),
            _$form2 : $('<form>', {
                'method' : 'post',
            }),
            _$btnWrap : $('<div>', {
                'class' : 'wrap_btn'
            }),
            _networkNameInput : hx.al.CPgConfigureWiFi.input({
                id : 'satInput',
                type : 'text',
                style : 'full'
            }),
            _passwdInput : hx.al.CPgConfigureWiFi.input({
                id : 'satInput2',
                type : 'password',
                style : 'full'
            }),
            _securityMenu : hx.al.CPgConfigureWiFi.securityTypeMenu('LOC_SECURITY_TYPE_ID'),
            _connectBtn : hx.al.CPgConfigureWiFi.button('LOC_CONNECT_ID'),
            _cancelBtn : hx.al.CPgConfigureWiFi.button('LOC_CANCEL_ID'),
        });

        that._$setHiddenNetStge.append(that._$hiddenNetStatus).append(that._$titleHiddenNetArea).append(that._$securityMenu.append(that._$securitySubMenu.after(that._securityMenu.$)).append(that._$wrapInput.append(that._$inputList.append(that._$inputTitle).append(that._$inputArea.append(that._$form.append(that._networkNameInput.$))).append(that._$inputTitle2).append(that._$inputArea2.append(that._$form2.append(that._passwdInput.$)))))).append(that._$btnWrap.append(that._connectBtn.$).append(that._cancelBtn.$));

        var $title : any = $('#set .title');
        if ($title[0]) {
            $title[0].children[0].innerHTML = hx.l('LOC_CONFIGURE_WLAN_01_ID');
        }
        $title.ani({
            style : that.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });

        var $set : any = $('#set');
        $set.append(that.$menu.append(that._wifitxt.$)).append(that._$setHiddenNetStge);
        //$set.after(that.$msg2.append(that.$res2.append(that._msgtxt2.$)));

        hx.al.compose(that);

        that._inputText = '';
        that._networkNameInput.$.val('');
        that._networkNameInput.$.bind('input', function() {
            that._inputText = $(this).val();
        });

        that._inputText2 = '';
        that._passwdInput.$.val('');
        that._passwdInput.$.bind('input', function() {
            that._inputText2 = $(this).val();
        });

        if (that._securityTypeValue === undefined)
            that._securityTypeValue = hx.l('LOC_NONE_ID');

        netConnectedInfo = hx.svc.SETTING_UTIL.getNetConnectedInfo();
        //TODO remove false
        if (!hx.config.testWifiOnDebug && netConnectedInfo.wireDevState === 'Connected') {
            that._wifitxt.$.html(hx.l('LOC_NET_NOT_DETECTED_ID'));
            $set.addClass('dimHbtm');
            that._backBtn = hx.al.CPgConfigureWiFi.button('LOC_BACK_ID');
            that.$menu.append(that._backBtn.$);
            that._backBtn.$.addClass('on');
            that.$menu.ani({
                style : that.aniConfig.menu.changeIn,
                trans : {
                    dur : 200,
                    delay : 400
                },
                cb : function() {
                    that.append(that._backBtn);
                    that._backBtn.on();
                }
            });
        } else {
            that.$menu.ani({
                style : that.aniConfig.menu.changeIn,
                trans : {
                    dur : 200,
                    delay : 400
                }
            });
            that.apProgressMsg = hx.msg('progress', {
                text : hx.l('LOC_SEARCHING_AVAIL_NETWORK_ID'),
                _use_backkey : false,
                auto_close : false,
                callback_fn : cancel_fn
                //timeout : 5000,
            });
            function cancel_fn(_status) {
                /*
                 if (_status === 'key_back') {
                 hx.svc.SETTING_UTIL.clearEventCb(that);
                 hx.svc.SETTING_UTIL.stopApScanning();
                 hx.msg.close('progress', that.apProgressMsg);
                 that._wifitxt.$.html(hx.l('LOC_NET_NOT_DETECTED_ID'));
                 $set.addClass('dimHbtm');
                 that._backBtn = hx.al.CPgConfigureWiFi.button('LOC_BACK_ID');
                 that.$menu.append(that._backBtn.$);
                 that.append(that._backBtn);
                 that._backBtn.on();
                 that._backBtn.$.addClass('on');
                 } else if (_status === 'key_esc') {
                 hx.svc.SETTING_UTIL.clearEventCb(that);
                 hx.msg.close('progress', that.apProgressMsg);
                 hx.svc.SETTING_UTIL.setFrontPanelString({
                 text : ''
                 });
                 that.sendEventToEntity({
                 alEvent : 'alChangePage',
                 target : 'CPgLiveController',
                 me : that
                 });
                 }
                 */
            }

            hx.svc.SETTING_UTIL.addEventCb('tryConnection', that, (aResult) => {
                hx.log("debug", "tryConnection - result: " + aResult);
                that._on_tryConnection(aResult);
            });
            hx.svc.SETTING_UTIL.addEventCb('eventWifiScanningResult', that, (aParam) => {
                hx.log("debug", "eventWifiScanningResult - result: " + aParam);
                hx.log("debug", "eventWifiScanningResult - count: " + aParam.apList.length);
                that._on_eventWifiScanningResult(aParam);
            });
            hx.svc.SETTING_UTIL.addEventCb('wirelessLinkStatus', that, (aResult) => {
                hx.log("debug", "wirelessLinkStatus - result: " + aResult);
                that._on_wirelessLinkStatus(aResult);
            });
            hx.svc.SETTING_UTIL.apScanning();
        }
        that._isChangePageInSetting = false;
    }
    destroy () {
        super.destroy();
        hx.svc.SETTING_UTIL.clearEventCb(this);
        this._status = 'notting';
        this._currentConn = undefined;
        this._selectedConn = undefined;
        $('#set').removeClass('dimHbtm');
        this._$optSection.remove();
        this._$setHiddenNetStge.remove();
        this.$msg2.remove();
        this.$menu.remove();
        if (!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
        if (this.progressMsg) {
            hx.msg.close('progress', this.progressMsg);
            this.progressMsg = null;
        }
        if (this.apProgressMsg) {
            hx.msg.close('progress', this.apProgressMsg);
            this.apProgressMsg = null;
        }
        if (this.passwdTmp) {
            hx.msg.close('passwd', this.passwdTmp);
            this.passwdTmp = null;
        }
        this.$menu = undefined;
        this._wifitxt = undefined;
        this.$msg2 = undefined;
        this.$res2 = undefined;
        this._msgtxt2 = undefined;
        this._currentConn = undefined;
        this._selectedConn = undefined;
        this._hiddenNetAppendFlag = false;
        this._securityTypeValue = undefined;
    }
    _on_tryConnection (aResult : string) {
        if (this.progressMsg) {
            hx.msg.close('progress', this.progressMsg);
            this.progressMsg = null;
        }
        if (this.apProgressMsg) {
            hx.msg.close('progress', this.apProgressMsg);
            this.apProgressMsg = null;
        }
        if (aResult === 'success') {
            this._isChangePageInSetting = true;
            hx.al.decompose(this);
            this.$menu.ani({
                style : this.aniConfig.menu.changeOut3,
                trans : {
                    dur : 200,
                    delay : 100
                },
                cb : ()=> {
                    this.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgSettings',
                        me : this
                    });
                }
            });
        }
    }
    _on_eventWifiScanningResult (aParam) {
        var apNum : number, i : number;
        if (this.apProgressMsg) {
            hx.msg.close('progress', this.apProgressMsg);
            this.apProgressMsg = null;
        }
        if (aParam.apList.length === 0) {// apList none
            this._wifitxt.$.html(hx.l('LOC_NET_NOT_DETECTED_ID'));
            $('#set').addClass('dimHbtm');
            this._backBtn = hx.al.CPgConfigureWiFi.button('LOC_BACK_ID');
            this.$menu.append(this._backBtn.$);
            this.append(this._backBtn);
            this._backBtn.on();
            this._backBtn.$.addClass('on');
        } else {
            var $set = $('#set');
            $set.after(this._$optSection.append(this._option.$));
            this.append(this._option);
            this.apList = aParam.apList;
            this._makesubList(this.apList);

            if (aParam.apList.length > 6) {
                this.$menu.append($('<div>', {
                    'class' : 'dimComBottom'
                }));
            }

            for ( i = 0; i < this.apList.length; i += 1) {
                if (this.apList[i].isConnected === 1) {//connected
                    this._selectedConn = i;
                    break;
                }
            }
        }
    }
    _on_wirelessLinkStatus (aResult : string) {
        var conObj : any;
        var curObj : any;
        if (this.progressMsg) {
            hx.msg.close('progress', this.progressMsg);
            this.progressMsg = null;
        }
        this._status = 'notting';
        if (aResult === 'success') {
            /*
             hx.msg('response', {
             text : hx.l('LOC_NETWORK_CON_SUCCESS_ID'),
             auto_close : true,
             close_time : 3000,
             dia_class : 'res vaTop'
             });
             */
            conObj = this._subList.getConnectedItem();
            curObj = this._subList.getfocusedItem();
            if (conObj) {
                conObj[0].classList.remove('conn');
            }

            if (this._hiddenConnect) {
                var connectedInfo = hx.svc.SETTING_UTIL.getNetConnectedInfo();

                if (!this._hiddenNetAppendFlag) {
                    if (this._selectedConn !== undefined) {
                        if (this.apList[this._selectedConn]) {
                            this.apList[this._selectedConn].isConnected = 0;
                        }
                    }
                    this._selectedConn = 0;
                    this.apList.unshift(connectedInfo.wirelessDevInfo);
                    this.apList[0].hidden = true;
                    this._hiddenNetAppendFlag = true;
                }
                this._subList.bind(this.apList);
                this._subList.draw();
                this.setItemFocus(this._selectedConn);

            } else {
                if (curObj) {
                    curObj.children[0].children[0].classList.add('conn');
                }

                if (this._selectedConn !== undefined) {
                    if (this.apList[this._selectedConn]) {
                        this.apList[this._selectedConn].isConnected = 0;
                    }
                }
                this._selectedConn = this._currentConn;
                if (this._selectedConn !== undefined) {
                    if (this.apList[this._selectedConn]) {
                        this.apList[this._selectedConn].isConnected = 1;
                    }
                }
                this._subList.bind(this.apList);
            }
            this._isChangePageInSetting = true;
            this.sendEventToEntity({
                alEvent : 'alChangePage',
                target : 'CPgNetworkConnectionStatus',
                me : this
            });

        } else {
            hx.msg('response', {
                text : hx.l('LOC_NETWORK_CON_FAILED_ID'),
                auto_close : true,
                close_time : 3000,
                dia_class : 'res vaTop'
            });
        }
    }
    _makeHiddenDlg (aParam?) {
        this._$setHiddenNetStge.addClass('active');
        this._$setHiddenNetStge.ani({
            style : this.aniConfig.channelMenu.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            },
            cb : ()=> {
                this.append(this._securityMenu).append(this._networkNameInput).append(this._passwdInput).append(this._connectBtn).append(this._cancelBtn);
                this._securityMenu.on();
            }
        });

        this._$form.ani({
            style : this.aniConfig.channelMenu.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            },
        });
        this._$form2.ani({
            style : this.aniConfig.channelMenu.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            },
        });

        this._inputText = '';
        this._inputText2 = '';

        var $input = $('#satInput');
        $input.val('');
        $input.bind('input', function() {
            this._inputText = $(this).val();
        });

        var $input2 = $('#satInput2');
        $input2.val('');
        $input2.bind('input', function() {
            this._inputText2 = $(this).val();
        });

        var $securityValue = this._securityMenu.getItem(0).find('em');
        $securityValue.html(this._securityTypeValue);
    }
    _on_alClicked (aParam) : boolean {
        var that = this, data = aParam.alData, bConsumed : boolean = false, i : number = 0;
        var dataList : any = [];
        var j : number, len : number;

        if (data.me === that._backBtn) {
            bConsumed = true;
            that._isChangePageInSetting = true;
            hx.al.decompose(that);
            that.$menu.ani({
                style : that.aniConfig.menu.changeOut3,
                trans : {
                    dur : 200,
                    delay : 100
                },
                cb : function() {
                    that.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgSettings',
                        me : that
                    });
                }
            });
        } else if (data.me === that._subList) {
            bConsumed = true;

            var apObj = that._subList.getfocusedItem();

            if (apObj) {
                for ( i = 0; i < that.apList.length; i += 1) {
                    if (apObj.innerText === that.apList[i].apname) {
                        break;
                    }
                }
            }

            that._currentConn = i;
            if ((that._currentConn > that.apList.length - 1) || that._currentConn < 0 || !apObj) {
                that._currentConn = 0;
            }

            that._hiddenConnect = false;

            if (that.apList[that._currentConn].secure === "true") {
                that.passwdTmp = hx.msg('passwd', {
                    text : hx.l('LOC_ENTER_PASSWORD_WIRELESS_ID'), //
                    responseText : false, //hx.l('LOC_TYPE_PASSWORD_ID')
                    wifi : that.apList[that._currentConn].apname, //wifi
                    type : 'password',
                    auto_close : false,
                    dia_class : 'osd',
                    callback_fn : fn,
                });
                function fn(_status, _input) {
                    if (_status === 'key_back' || _status === 'key_esc') {
                        hx.msg.close('passwd', that.passwdTmp);
                    } else if (_status === hx.l('LOC_OK_ID')) {
                        if (_input) {
                            that._status = "connecting";
                            that.progressMsg = hx.msg('progress', {
                                text : hx.l('LOC_MESG_5986_ID'),
                                _use_backkey : true,
                                auto_close : false,
                                callback_fn : function(_status) {
                                    if (_status === 'key_back' || _status === 'key_esc') {
                                        hx.svc.SETTING_UTIL.wirelessStopConnecting();
                                    }
                                }
                            });
                            hx.svc.SETTING_UTIL.applyConnectToAP({
                                index : that._currentConn,
                                password : _input.password
                            });
                        }
                        hx.msg.close('passwd', that.passwdTmp);
                    } else if (_status === 'close') {
                    }
                }

            } else {
                that._status = "connecting";
                that.progressMsg = hx.msg('progress', {
                    text : hx.l('LOC_MESG_5986_ID'),
                    _use_backkey : true,
                    auto_close : false,
                    callback_fn : function(_status) {
                        if (_status === 'key_back' || _status === 'key_esc') {
                            hx.svc.SETTING_UTIL.wirelessStopConnecting();
                        }
                    }
                });
                hx.svc.SETTING_UTIL.applyConnectToAP({
                    index : that._currentConn
                });
            }
        } else if (data.me === that._securityMenu) {
            bConsumed = true;
            that._makeSecuritySubList(this._securityValueIndex);
        } else if (data.me === that._securitySubList) {
            bConsumed = true;
            len = this.wirelessSecurityType.length + 1;

            dataList.push(hx.l('LOC_NONE_ID'));
            for ( j = 0; j < len; j++) {
                dataList.push(this.wirelessSecurityType[j]);
                if (data.result === dataList[j]) {
                    that._securityTypeValue = dataList[j];
                    that._securityValueIndex = j;
                    break;
                }
            }
            var $securityValue : any = that._securityMenu.getItem(0).find('em');
            $securityValue.html(that._securityTypeValue);

            that._securitySubList.off();
            that._$securitySubMenu.ani({
                style : that.aniConfig.dep2.fadeOut,
                trans : {
                    dur : 150,
                    delay : 75
                },
                cb : function() {
                    that._securitySubList.$.remove();
                    that._$securitySubMenu.removeClass('active');
                    that._securityMenu.getItem(0).removeClass('his');
                    that._securityMenu.setItemFocus(0);
                    that._securityMenu.on();
                }
            });
        } else if (data.me === that._connectBtn || data.me === that._cancelBtn) {
            bConsumed = true;
            if (data.me === that._connectBtn) {
                var paramValue = this.wirelessSecurityTypeValue;
                var index = that._securityValueIndex;

                that._hiddenConnect = true;

                if (that._inputText.length === 0 || that._inputText.indexOf(' ') === 0) {
                    hx.msg('response', {
                        text : hx.l('LOC_ENTER_NETWORK_NAME_ID'),
                        auto_close : true,
                        close_time : 3000,
                        dia_class : 'res vaTop'
                    });
                    return bConsumed;
                } else {
                    that._networkName = that._inputText;
                    that._password = that._inputText2;
                    that._status = "connecting";
                    that.progressMsg = hx.msg('progress', {
                        text : hx.l('LOC_MESG_5986_ID'),
                        _use_backkey : true,
                        auto_close : false,
                        callback_fn : function(_status) {
                            if (_status === 'key_back' || _status === 'key_esc') {
                                hx.svc.SETTING_UTIL.wirelessStopConnecting();
                            }
                        }
                    });
                    hx.svc.SETTING_UTIL.applyConnectToAP({
                        password : that._password,
                        ap : {
                            'essid' : that._networkName,
                            'secure' : paramValue[index][0],
                            'authen' : paramValue[index][1],
                            'encrypt' : paramValue[index][2]
                        }
                    });
                }
            } else if (data.me === that._cancelBtn) {
            }
            that._$setHiddenNetStge.ani({
                style : that.aniConfig.channelMenu.fadeOut,
                trans : {
                    dur : 200,
                    delay : 100
                },
                cb : function() {
                    var $set = $('#set');
                    that._$setHiddenNetStge.removeClass('active');
                    $set.after(that._$optSection.append(that._option.$));
                    that._securityMenu.off();
                    that._connectBtn.$.removeClass('on');
                    that._cancelBtn.$.removeClass('on');
                    that._connectBtn.off();
                    that._cancelBtn.off();
                    that._subList.on();
                }
            });
        }
        return bConsumed;
    }
    _on_alKey (aParam) : boolean {
        var that = this, i, data : any = aParam.alData, e : number = aParam.alKey, bConsumed : boolean = false;
        switch(e) {
            case hx.key.KEY_BACK:
            case hx.key.KEY_LEFT:
                bConsumed = true;
                if (that._networkNameInput.$.hasClass('on') || that._passwdInput.$.hasClass('on')) {
                    that._networkNameInput.$.removeClass('on');
                    that._passwdInput.$.removeClass('on');
                    that._$setHiddenNetStge.ani({
                        style : that.aniConfig.channelMenu.fadeOut,
                        trans : {
                            dur : 200,
                            delay : 100
                        },
                        cb : function() {
                            var $set = $('#set');
                            that._$setHiddenNetStge.removeClass('active');
                            $set.after(that._$optSection.append(that._option.$));
                            that._securityMenu.off();
                            that._connectBtn.$.removeClass('on');
                            that._cancelBtn.$.removeClass('on');
                            that._connectBtn.off();
                            that._cancelBtn.off();
                            that._subList.on();
                        }
                    });
                } else if (that._$securitySubMenu.hasClass('active')) {
                    if (aParam.alData.me === that._securitySubList) {
                        that._securitySubList.off();
                        that._$securitySubMenu.ani({
                            style : that.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : function() {
                                that._securitySubList.$.remove();
                                that._$securitySubMenu.removeClass('active');
                                that._securityMenu.getItem(0).removeClass('his');
                                that._securityMenu.setItemFocus(0);
                                that._securityMenu.on();
                            }
                        });
                    }
                } else {
                    that._isChangePageInSetting = true;
                    hx.al.decompose(that);
                    that.$menu.ani({
                        style : that.aniConfig.menu.changeOut3,
                        trans : {
                            dur : 200,
                            delay : 100
                        },
                        cb : function() {
                            that.sendEventToEntity({
                                alEvent : 'alChangePage',
                                target : 'CPgSettings',
                                me : that
                            });
                        }
                    });
                }
                break;
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
            case hx.key.KEY_ESCAPE:
                if (data) {
                    bConsumed = true;
                    hx.svc.SETTING_UTIL.setFrontPanelString({
                        text : ''
                    });
                    that.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgLiveController',
                        me : that
                    });
                }
                break;
            case hx.key.KEY_OPTION:
                if (data) {
                    bConsumed = true;
                    if (!(that._$setHiddenNetStge.hasClass('active'))) {
                        that._subList.off();
                        that._$optSection.remove();
                        that._makeHiddenDlg();
                    }
                }
                break;
            case hx.key.KEY_UP:
                bConsumed = true;
                if (that._networkNameInput.$.hasClass('on')) {
                    that._networkNameInput.$.removeClass('on');
                    that._securityMenu.on();
                } else if (that._passwdInput.$.hasClass('on')) {
                    that._passwdInput.$.removeClass('on');
                    that._networkNameInput.$.addClass('on');
                }
                break;
            case hx.key.KEY_DOWN:
                bConsumed = true;
                if (that._networkNameInput.$.hasClass('on')) {
                    that._networkNameInput.$.removeClass('on');
                    that._passwdInput.$.addClass('on');
                } else if (that._passwdInput.$.hasClass('on')) {
                    that._passwdInput.$.removeClass('on');
                    that._connectBtn.$.addClass('on');
                    that._connectBtn.on();
                }
                break;
            case hx.key.KEY_OK:
                if (that._networkNameInput.$.hasClass('on')) {
                    that._networkNameInput.$.blur();
                    that._networkNameInput.$.focus();
                    bConsumed = true;
                } else if (that._passwdInput.$.hasClass('on')) {
                    that._passwdInput.$.blur();
                    that._passwdInput.$.focus();
                    bConsumed = true;
                }
                break;
        }
        return bConsumed;
    }
    _on_alExit (param) : boolean {
        var that = this, data = param.alData, bConsumed : boolean = false;
        if (data) {
            if (data.me === that._backBtn) {
                bConsumed = true;
                that._isChangePageInSetting = true;
                hx.al.decompose(that);
                that.$menu.ani({
                    style : that.aniConfig.menu.changeOut3,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : function() {
                        that.sendEventToEntity({
                            alEvent : 'alChangePage',
                            target : 'CPgSettings',
                            me : that
                        });
                    }
                });
            } else if (data.me === that._securityMenu || data.me === that._connectBtn || data.me === that._cancelBtn) {
                bConsumed = true;
                that._$setHiddenNetStge.ani({
                    style : that.aniConfig.channelMenu.fadeOut,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb : function() {
                        var $set = $('#set');
                        that._$setHiddenNetStge.removeClass('active');
                        $set.after(that._$optSection.append(that._option.$));
                        that._securityMenu.off();
                        that._connectBtn.$.removeClass('on');
                        that._cancelBtn.$.removeClass('on');
                        that._connectBtn.off();
                        that._cancelBtn.off();
                        that._subList.on();
                    }
                });
            }
        }
        return bConsumed;
    }
    _on_alScrollRight (aParam) : boolean {
        var data : any = aParam.alData, target : any = data.me, bConsumed : boolean = false;

        switch (target) {
            case this._connectBtn:
                bConsumed = true;
                this._connectBtn.$.removeClass('on');
                this._connectBtn.off();
                this._cancelBtn.$.addClass('on');
                this._cancelBtn.on();
                break;
            case this._cancelBtn:
                bConsumed = true;
                this._cancelBtn.$.removeClass('on');
                this._cancelBtn.off();
                this._connectBtn.$.addClass('on');
                this._connectBtn.on();
                break;
        }
        return bConsumed;
    }
    _on_alScrollLeft (aParam) : boolean {
        var data = aParam.alData, target = data.me, bConsumed : boolean = false;

        switch (target) {
            case this._connectBtn:
                bConsumed = true;
                this._connectBtn.$.removeClass('on');
                this._connectBtn.off();
                this._cancelBtn.$.addClass('on');
                this._cancelBtn.on();
                break;
            case this._cancelBtn:
                bConsumed = true;
                this._cancelBtn.$.removeClass('on');
                this._cancelBtn.off();
                this._connectBtn.$.addClass('on');
                this._connectBtn.on();
                break;
        }
        return bConsumed;
    }
    _on_alScrollup (aParam) : boolean {
        var bConsumed : boolean = false;
        if (aParam.alData.me === this._connectBtn || aParam.alData.me === this._cancelBtn) {
            bConsumed = true;
            if (aParam.alData.me === this._connectBtn) {
                this._connectBtn.$.removeClass('on');
                this._connectBtn.off();
            } else if (aParam.alData.me === this._cancelBtn) {
                this._cancelBtn.$.removeClass('on');
                this._cancelBtn.off();
            }
            this._passwdInput.$.addClass('on');
        } else if (aParam.alData.me === this._securityMenu) {
            bConsumed = true;
            this._securityMenu.off();
            this._connectBtn.$.addClass('on');
            this._connectBtn.on();
        }
        return bConsumed;
    }
    _on_alScrolldown (aParam) : boolean {
        var bConsumed : boolean = false;
        if (aParam.alData.me === this._securityMenu) {
            bConsumed = true;
            this._securityMenu.off();
            this._networkNameInput.$.addClass('on');
        } else if (aParam.alData.me === this._connectBtn || aParam.alData.me === this._cancelBtn) {
            bConsumed = true;
            if (aParam.alData.me === this._connectBtn) {
                this._connectBtn.$.removeClass('on');
                this._connectBtn.off();
            } else if (aParam.alData.me === this._cancelBtn) {
                this._cancelBtn.$.removeClass('on');
                this._cancelBtn.off();
            }
            this._securityMenu.on();
        }
        return bConsumed;
    }
    _makesubList (aApList) {
        var index : number, subFocusIndex : number = 0;
        var dataList : any = aApList;
        //apList;
        this._subList = hx.al.component('dynamicMenu', {
            attr : {
                'style' : 'display:block;'
            },
            type : 'wireless',
            currentindex : subFocusIndex,
            displayCount : dataList.length
        });
        this.$menu.addClass('active');
        this.append(this._subList);
        this.$menu.append(this._subList.$);
        this._subList.on();
        this._subList.bind(dataList);
        this._subList.draw();
    }
    _makeSecuritySubList (aParam : number) {
        var index : number, subFocusIndex : number = 0;
        var dataList : any = [];
        var j : number, len : number = this.wirelessSecurityType.length;

        dataList.push(hx.l('LOC_NONE_ID'));
        for ( j = 0; j < len; j++) {
            dataList.push(this.wirelessSecurityType[j]);
        }

        this._securitySubList = hx.al.component('simpleMenu', {
            attr : {
                'style' : 'display:block;',
                'class' : 'subList'
            },
            currentindex : aParam,
            displayCount : dataList.length,
            _isNotId : true
        });
        this._$securitySubMenu.addClass('active');
        this._securityMenu.getItem(0).addClass('his');
        this._$securitySubMenu.append(this._securitySubList.$);
        this._securitySubList.bind(dataList);
        this._securitySubList.draw();
        this._$securitySubMenu.ani({
            style : this.aniConfig.dep2.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            },
            cb : ()=> {
                this.append(this._securitySubList);
                this._securitySubList.on();
            }
        });
    }
}
hx.al.CPgConfigureWiFi =  cCPgConfigureWiFi;

hx.al.CPgConfigureWiFi.nettxt = function(aParam) {
    return hx.al.component('text', {
        tag : 'p',
        attr : {
            'data-langID' : aParam
        }
    });
};

hx.al.CPgConfigureWiFi.button = function(aParam) {
    return (new hx.al.component('button', {
        attr : {
            'class' : 'button',
            'data-langID' : aParam
        },
        upper : true,
        usePointerKey : true
    }));
};

hx.al.CPgConfigureWiFi.option = function(aParam) {
    return (new hx.al.component('button', {
        attr : {
            'class' : 'tit',
            'data-langID' : aParam
        },
        usePointerKey : true
    }));
};

hx.al.CPgConfigureWiFi.securityTypeMenu = function(aParam) {
    var securityTypeMainMenu = hx.al.component('settingsList', {
        itemAttr : {},
        attr : {
            'class' : 'dep1'
        },
        roll : 'notroll',
        data : aParam,
        count : 1,
        fx : function(i, o) {
            o.html(hx.l(this.data)).append($('<em>'));
        },
        keyTable : [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
    });
    return securityTypeMainMenu;
};

hx.al.CPgConfigureWiFi.input = function(aParam) {
    return hx.al.component('input', {
        attr : {
            'id' : aParam.id,
            'type' : aParam.type,
            'style' : aParam.style
        },
    });
};

hx.al.CPgConfigureWiFi.prototype.aniConfig = {
    fadeIn : {
        start : {
            'opacity' : 0
        },
        end : {
            'opacity' : 1
        }
    },
    channelMenu : {
        fadeIn : {
            start : {
                'opacity' : 0,
                'top' : '20px'
            },
            end : {
                'opacity' : 1,
                'top' : 0
            }
        },
        fadeOut : {
            start : {
                'opacity' : 1,
                'top' : 0
            },
            end : {
                'opacity' : 0,
                'top' : '20px'
            }
        }
    },
    dep2 : {
        fadeIn : {
            start : {
                'opacity' : 0,
                'left' : '795px'
            },
            end : {
                'opacity' : 1,
                'left' : '775px'
            }
        },
        fadeOut : {
            start : {
                'opacity' : 1,
                'left' : '775px'
            },
            end : {
                'opacity' : 0,
                'left' : '795px'
            }
        }
    },
    menu : {
        changeIn : {
            start : {
                'opacity' : 0,
                'left' : '20px'
            },
            end : {
                'opacity' : 1,
                'left' : 0
            }
        },
        changeOut3 : {
            start : {
                'opacity' : 1,
                'left' : 0
            },
            end : {
                'opacity' : 0,
                'left' : '20px'
            }
        }
    }
};
export = cCPgConfigureWiFi;
