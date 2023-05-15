/// <reference path="../../def/sugar.d.ts" />
// file name : pages/install/page05_wifi.js
/*jslint sloppy:true, nomen:true, vars:true */
/* DESIGN : design_result/02_WIZ/WIZ_301.html */
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx: any;

class cCPgInstall05 extends __PAGE_KERNEL__ {
    reponseMsg : any;
    progressMsg : any;
    aniConfig : any;
    apList : any;
    apProgressMsg : any;

    _wifi_arr = [];
    _securityTypeValue : string = hx.l('LOC_NONE_ID');
    _securityValueIndex : number = 0;
    _networkName : string = '';
    _password : string = '';
    _showItemIndex = 0;
    _isGoWizardPage : boolean;
    _ul : any;
    _passwdInput : any;
    _connectBtn : any;
    _cancelBtn : any;
    _securityMenu : any;
    _containerEl : HTMLElement;
    _stringEl : HTMLElement;
    _bottomDimEl : HTMLElement;
    _stepEl : HTMLElement;
    _$setHiddenNetStge : any;
    _option : any;
    _currentConn : any;
    _inputText : string;
    _inputText2 : string;
    _securitySubList : any;
    _$securitySubMenu : any;
    _networkNameInput : any;
    _$article : any;
    _$title : any;
    _$optSection : any;
    _$hArea : any;
    _status : any;
    _$form : any;
    _$form2 : any;
    _$btnWrap : any;
    _$inputArea : any;
    _$inputArea2 : any;
    _$inputTitle : any;
    _$inputTitle2 : any;
    _backProcess : any;
    _$titleHiddenNetArea : any;
    _$hiddenNetStatus : any;
    _$securityMenu : any;
    _$wrapInput : any;
    _$inputList : any;
    _curState : any;
    _isHidden: boolean;
    wirelessSecurityType = ['WEP 64-bit Hex', 'WEP 64-bit ASCII', 'WEP 128-bit Hex', 'WEP 128-bit ASCII', 'WPA-PSK(TKIP)', 'WPA-PSK(AES)', 'WPA2-PSK(TKIP+AES)', 'WPA2-PSK(AES)'];
    wirelessSecurityTypeValue = [[0, 0, 0], [1, 0, 2], [1, 0, 1], [1, 0, 4], [1, 0, 3], [2, 1, 5], [2, 1, 6], [3, 1, 7], [3, 1, 6]];

    SKIP_THIS_STEP: number = 0; // CONST

    constructor() {
        super();
    }
    create (aEntity, aPageUrl) {
        hx.logadd('CPgInstall05');
        var that = this;
        var nowStep : string;

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(false);

        that._backProcess = false;
        that._curState = {
            isApScannig : false,
            isApConnecting : false
        };

        that.apProgressMsg = hx.msg('progress', {
            text : hx.l('LOC_SEARCHING_AVAIL_NETWORK_ID'),
            _use_backkey : true,
            dia_class : 'osd',
            auto_close : true,
            timeout : 40000,
            callback_fn : cancel_fn
            //timeout : 5000,
        });
        function cancel_fn(_status : string) {
            if (_status === 'key_back' && !that._backProcess) {
                that._backProcess = true;
                hx.svc.SETTING_UTIL.clearEventCb(that);
                hx.svc.SETTING_UTIL.stopApScanning();
                that._curState.isApScannig = false;
                hx.msg.close('progress', that.apProgressMsg);
                that._go_next_page_fail();
            } else if (_status === 'auto_close') {
                that._curState.isApScannig = false;
                hx.msg.close('progress', that.apProgressMsg);
                hx.svc.SETTING_UTIL.clearEventCb(that);
                that._stringEl.innerHTML = hx.l('LOC_NET_NOT_DETECTED_ID');
                that._wifi_arr.push({
                    '_name' : hx.l('LOC_SKIP_ID'),
                    '_value' : '',
                    '_lock' : false
                });
                that._ul = that._set_list();
                that._$article.append(that._ul.$);
                that.append(that._ul);
                that._ul.on();
                hx.il.ani.action({
                    targets : [that._$article],
                    cls : 'aniFadeIn',
                    style : 'left:0'
                });
            }
        }

        hx.svc.SETTING_UTIL.addEventCb('eventWifiScanningResult', that, (aParam) => {
            that._on_eventWifiScanningResult(aParam);
        });
        hx.svc.SETTING_UTIL.addEventCb('wirelessLinkStatus', that, (aResult) => {
            that._on_wirelessLinkStatus(aResult);
        });
        hx.svc.SETTING_UTIL.apScanning();
        that._curState.isApScannig = true;

        $.extend(that, {
            _containerEl : document.querySelector('#wiz'),
            _$article : $('<article>', {
                'class' : 'cont'
            }),
            _stringEl : document.createElement('p'),
            _bottomDimEl : document.createElement('div'),
            _$hArea : $('<h1>', {
                'class' : 'hArea'
            }),
            _$title : $('<span>', {
                'class' : 'tit',
                'html' : hx.l('LOC_NETWORK_CONNECTION_ID').toUpperCase()
            }),
            _stepEl : document.createElement('span'),
            _$optSection : $('<section>', {
                'id' : 'opt'
            }),
            _option : hx.al.CPgInstall05.option('LOC_HIDDEN_NETWORK_ID'),
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
                'method' : 'post'
            }),
            _$inputList2 : $('<dl>'),
            _$inputTitle2 : $('<dt>', {
                'html' : hx.l('LOC_PASSWORD_ID')
            }),
            _$inputArea2 : $('<dd>', {
                'class' : 'point'
            }),
            _$form2 : $('<form>', {
                'method' : 'post'
            }),
            _$btnWrap : $('<div>', {
                'class' : 'wrap_btn'
            }),
            _networkNameInput : hx.al.CPgInstall05.input({
                id : 'satInput',
                type : 'text',
                style : 'full'
            }),
            _passwdInput : hx.al.CPgInstall05.input({
                id : 'satInput2',
                type : 'password',
                style : 'full'
            }),
            _securityMenu : hx.al.CPgInstall05.securityTypeMenu('LOC_SECURITY_TYPE_ID'),
            _connectBtn : hx.al.CPgInstall05.button('LOC_CONNECT_ID'),
            _cancelBtn : hx.al.CPgInstall05.button('LOC_CANCEL_ID')
        });

        that._$setHiddenNetStge.append(that._$hiddenNetStatus).append(that._$titleHiddenNetArea).append(that._$securityMenu.append(that._$securitySubMenu.after(that._securityMenu.$)).append(that._$wrapInput.append(that._$inputList.append(that._$inputTitle).append(that._$inputArea.append(that._$form.append(that._networkNameInput.$))).append(that._$inputTitle2).append(that._$inputArea2.append(that._$form2.append(that._passwdInput.$)))))).append(that._$btnWrap.append(that._connectBtn.$).append(that._cancelBtn.$));

        if (ENV.op === ENV.listOP.SES) {
            nowStep = 'step1';
        } else if (ENV.op === ENV.listOP.MIDDLE_EAST) {
            nowStep = 'step3';
        } else {
            //nowStep = 'step2';
            nowStep = hx.al.CPgInstall05.items.step;
        }
        that._stepEl.classList.add(nowStep);
        that._stringEl.innerHTML = hx.l('LOC_SELECT_WIRELESS_NETWORK_ID');
        that._bottomDimEl.classList.add('cov2');

        /////////////////append To DOM
        if (!that._containerEl) {
            that._containerEl = document.createElement('section');
            that._containerEl.id = 'wiz';
            document.querySelector('#wrap').appendChild(that._containerEl);
        }
        that._$hArea[0].appendChild(that._$title[0]);
        that._$hArea[0].appendChild(that._stepEl);
        that._$article[0].appendChild(that._stringEl);
        that._containerEl.style.display = 'none';
        that._containerEl.appendChild(that._$hArea[0]);
        that._containerEl.appendChild(that._$article[0]);
        that._containerEl.appendChild(that._bottomDimEl);
        that._containerEl.appendChild(that._$setHiddenNetStge[0]);
        that._containerEl.style.display = 'block';

		if (that._securityTypeValue === undefined)
        	that._securityTypeValue = hx.l('LOC_NONE_ID');

        /////////////Do Initial Script
        hx.il.ani.action({
            targets : that._$hArea,
            cls : 'aniFadeIn'
        });

        that._isGoWizardPage = false;
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

        var bResult = false;
        bResult = hx.svc.SETTING_UTIL.checkWirelessConnection();

        hx.log('CPgInstall05', '###### checkWirelessConnection ####### bResult : ' + bResult);
        if (bResult === false) {
            if (that.apProgressMsg) {
                that._backProcess = true;
                hx.svc.SETTING_UTIL.clearEventCb(that);
                hx.svc.SETTING_UTIL.stopApScanning();
                hx.msg.close('progress', that.apProgressMsg);
                that._go_next_page_fail();
            }
        }
    }
    destroy () {
        hx.svc.SETTING_UTIL.clearEventCb(this);

        if (this._containerEl) {
            if (!this._isGoWizardPage) {
                if (this._curState.isApScannig) {
                    hx.svc.SETTING_UTIL.stopApScanning();
                }
                if (this._curState.isApConnecting) {
                    hx.svc.SETTING_UTIL.wirelessStopConnecting();
                }
                if (this.apProgressMsg) {
                    hx.msg.close('progress', this.apProgressMsg);
                }
                if (this.progressMsg) {
                    hx.msg.close('progress', this.progressMsg);
                }
                this._containerEl.parentElement.removeChild(this._containerEl);
            } else {
                this._containerEl.style.display = 'none';
                this._containerEl.removeChild(this._$hArea[0]);
                this._containerEl.removeChild(this._$article[0]);
                this._containerEl.removeChild(this._bottomDimEl);
                this._containerEl.style.display = 'block';
            }
            this._containerEl = undefined;
        }

        if(this._$optSection && this._$optSection.remove)
        {
            this._$optSection.remove();
        }

        this._$article = undefined;
        this._stringEl = undefined;
        this._bottomDimEl = undefined;
        this._$hArea = undefined;
        this._$title = undefined;
        this._stepEl = undefined;
        this._ul = undefined;
        this._$optSection = undefined;
        this._option = undefined;
        this._securityTypeValue = undefined;

        var diaEl : any = document.querySelector('#DIA');
        if (diaEl) {
            diaEl.classList.remove('osd');
        }
        this._wifi_arr = [];
        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(true);
    }
    _on_eventWifiScanningResult (aParam) : void {
        var listData : any = [], i : number, apData : any = {};

        hx.msg.close('progress', this.apProgressMsg);
        this._curState.isApScannig = false;
        if (aParam.apList.length === 0) {// apList none
            hx.svc.SETTING_UTIL.clearEventCb(this);
            this._stringEl.innerHTML = hx.l('LOC_NET_NOT_DETECTED_ID');
            this._wifi_arr.push({
                '_name' : hx.l('LOC_SKIP_ID'),
                '_value' : '',
                '_lock' : false
            });
            this._ul = this._set_list();
            this._$article.append(this._ul.$);
            this.append(this._ul);
            this._ul.on();

            hx.il.ani.action({
                targets : [this._$article],
                cls : 'aniFadeIn',
                style : 'left:0'
            });
        } else {
            this.apList = aParam.apList;
            this._wifi_arr.push({
                '_name' : hx.l('LOC_SKIP_ID'),
                '_value' : '',
                '_lock' : false
            });

            for ( i = 0; i < this.apList.length; i += 1) {
                this._wifi_arr.push({
                    '_name' : this.apList[i].apname,
                    '_value' : String(this.apList[i].strength),
                    '_lock' : this.apList[i].secure
                });
            }

            this._ul = this._set_list();
            this._$article.append(this._ul.$);
            this.append(this._ul);
            this._ul.on();

            document.querySelector('#wrap').appendChild(this._$optSection[0]).appendChild(this._option.$[0]);
            this.append(this._option);

            hx.il.ani.action({
                targets : [this._$article],
                cls : 'aniFadeIn',
                style : 'left:0'
            });
        }
    }
    _on_wirelessLinkStatus (aResult) : void {
        hx.log("debug', _on_wirelessLinkStatus - : aResult: " + aResult);
        hx.log("debug', _on_wirelessLinkStatus - : isApConnecting: " + this._curState.isApConnecting);
        if (this._curState.isApConnecting) {
            hx.msg.close('progress', this.progressMsg);
            this._curState.isApConnecting = false;
            this._status = 'notting';
            if (aResult === 'success') {
                this._connect_success_wifi();
            } else {
                this._connect_fail_wifi();
            }
        }
    }
    _makeHiddenDlg (param?) : void {
        var that = this;

        that._$setHiddenNetStge.addClass('active');
        that._$setHiddenNetStge.ani({
            style : that.aniConfig.channelMenu.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            },
            cb : ()=> {
                that.append(that._securityMenu).append(that._networkNameInput).append(that._passwdInput).append(that._connectBtn).append(that._cancelBtn);
                that._securityMenu.on();
            }
        });

        that._$form.ani({
            style : that.aniConfig.channelMenu.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            }
        });
        that._$form2.ani({
            style : that.aniConfig.channelMenu.fadeIn,
            trans : {
                dur : 200,
                delay : 100
            }
        });

        that._inputText = '';
        that._inputText2 = '';

        var $input = $('#satInput');
        $input.val('');
        $input.bind('input', function() {
            that._inputText = $(this).val();
        });

        var $input2 = $('#satInput2');
        $input2.val('');
        $input2.bind('input', function() {
            that._inputText2 = $(this).val();
        });

        var $securityValue = that._securityMenu.getItem(0).find('em');
        $securityValue.html(that._securityTypeValue);
    }
    _on_alKey (aParam) : boolean {
        var idx : number;
        var _key : number = aParam.alKey;
        var bConsumed : boolean = false;
        switch(_key) {
            case hx.key.KEY_BACK:
            case hx.key.KEY_LEFT:
                bConsumed = true;
                if (this._networkNameInput.$.hasClass('on') || this._passwdInput.$.hasClass('on')) {
                    this._networkNameInput.$.removeClass('on');
                    this._passwdInput.$.removeClass('on');
                    this._$setHiddenNetStge.ani({
                        style : this.aniConfig.channelMenu.fadeOut,
                        trans : {
                            dur : 200,
                            delay : 100
                        },
                        cb : ()=> {
                            this._$setHiddenNetStge.removeClass('active');
                            document.querySelector('#wrap').appendChild(this._$optSection[0]).appendChild(this._option.$[0]);
                            this._ul.on();
                            this._ul.setItemFocus(this._showItemIndex);
                        }
                    });
                } else if (this._$securitySubMenu.hasClass('active')) {
                    if (aParam.alData.me === this._securitySubList) {
                        this._securitySubList.off();
                        this._$securitySubMenu.ani({
                            style : this.aniConfig.dep2.fadeOut,
                            trans : {
                                dur : 150,
                                delay : 75
                            },
                            cb : ()=> {
                                this._securitySubList.$.remove();
                                this._$securitySubMenu.removeClass('active');
                                this._securityMenu.getItem(0).removeClass('his');
                                this._securityMenu.setItemFocus(0);
                                this._securityMenu.on();
                            }
                        });
                    }
                } else if (!this._backProcess) {
                    this._go_pre_page();
                }
                break;
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
            case hx.key.KEY_OPTION:
                bConsumed = true;
                if (!(this._$setHiddenNetStge.hasClass('active'))) {
                    this._isHidden = true;
                    this._showItemIndex = this._ul.getShowItemFocus();
                    this._ul.off();
                    document.querySelector('#wrap').removeChild(this._$optSection[0]);
                    this._makeHiddenDlg();
                }
                break;
            case hx.key.KEY_UP:
                bConsumed = true;
                if (this._networkNameInput.$.hasClass('on')) {
                    this._networkNameInput.$.removeClass('on');
                    this._securityMenu.on();
                } else if (this._passwdInput.$.hasClass('on')) {
                    this._passwdInput.$.removeClass('on');
                    this._networkNameInput.$.addClass('on');
                }
                break;
            case hx.key.KEY_DOWN:
                bConsumed = true;
                if (this._networkNameInput.$.hasClass('on')) {
                    this._networkNameInput.$.removeClass('on');
                    this._passwdInput.$.addClass('on');
                } else if (this._passwdInput.$.hasClass('on')) {
                    this._passwdInput.$.removeClass('on');
                    this._connectBtn.$.addClass('on');
                    this._connectBtn.on();
                }
                break;
            case hx.key.KEY_OK:
                if (this._networkNameInput.$.hasClass('on')) {
                    this._networkNameInput.$.blur();
                    this._networkNameInput.$.focus();
                    bConsumed = true;
                } else if (this._passwdInput.$.hasClass('on')) {
                    this._passwdInput.$.blur();
                    this._passwdInput.$.focus();
                    bConsumed = true;
                }
                break;
        }
        return bConsumed;
    }
    _on_alClicked (aParam) : boolean {
        var that = this;
        var bConsumed : boolean = false, idx : number;
        var dataList = [];
        var j : number, len : number;
        var currentFocus: number = 0;

        if (aParam.alData.me === this._ul) {
            bConsumed = true;
            currentFocus = this._ul.getItemFocus();
            this._currentConn = currentFocus - 1; /* because of SKIP THIS STEP */
            if (currentFocus === that.SKIP_THIS_STEP) {
                this._go_next_page_fail();
                return bConsumed;
            }

            idx = this._ul.getShowItemFocus();
            var _li = this._ul.$.children().eq(idx);
            _li.attr('class', 'on his');
            var tmp = $('<div>').html(aParam.alData.result);
            if (tmp.find('.lock').length === 0) {
                //skip password~!!!!
                this._connect_start_wifi();
            } else {
                //input password~!!!!
                this._input_pass(_li.text());
            }
        } else if (aParam.alData.me === this._securityMenu) {
            bConsumed = true;
            this._makeSecuritySubList(this._securityValueIndex);
        } else if (aParam.alData.me === this._securitySubList) {
            bConsumed = true;

            len = this.wirelessSecurityType.length + 1;
            dataList.push(hx.l('LOC_NONE_ID'));
            for ( j = 0; j < len; j++) {
            	dataList.push(this.wirelessSecurityType[j]);
                if (aParam.alData.result === dataList[j]) {
                    this._securityTypeValue = dataList[j];
                    this._securityValueIndex = j;
                    break;
                }
            }

            var $securityValue = this._securityMenu.getItem(0).find('em');
            $securityValue.html(this._securityTypeValue);

            this._securitySubList.off();
            this._$securitySubMenu.ani({
                style : this.aniConfig.dep2.fadeOut,
                trans : {
                    dur : 150,
                    delay : 75
                },
                cb : ()=> {
                    this._securitySubList.$.remove();
                    this._$securitySubMenu.removeClass('active');
                    this._securityMenu.getItem(0).removeClass('his');
                    this._securityMenu.setItemFocus(0);
                    this._securityMenu.on();
                }
            });
        } else if (aParam.alData.me === this._connectBtn || aParam.alData.me === this._cancelBtn) {
            bConsumed = true;
            if (aParam.alData.me === this._connectBtn) {
                var paramValue : any = this.wirelessSecurityTypeValue;
                var index : number = this._securityValueIndex;
                if (this._inputText.length === 0 || this._inputText.indexOf(' ') === 0) {
                    hx.msg('response', {
                        text : hx.l('LOC_ENTER_NETWORK_NAME_ID'),
                        auto_close : true,
                        close_time : 3000,
                        dia_class : 'res vaTop'
                    });
                    return bConsumed;
                } else {
                    this._networkName = this._inputText;
                    this._password = this._inputText2;
                    this._status = "connecting";
                    this.progressMsg = hx.msg('progress', {
                        text : hx.l('LOC_MESG_5986_ID'),
                        _use_backkey : true,
                        auto_close : false,
                        callback_fn : (_status)=> {
                            if (_status === 'key_back' || _status === 'key_esc') {
                                hx.svc.SETTING_UTIL.wirelessStopConnecting();
                            }
                            that._curState.isApConnecting = false;
                        }
                    });
                    hx.svc.SETTING_UTIL.applyConnectToAP({
                            password : this._password,
                            ap : {
                                'essid' : this._networkName,
                                'secure' : paramValue[index][0],
                                'authen' : paramValue[index][1],
                                'encrypt' : paramValue[index][2]
                            }
                    });
                    this._curState.isApConnecting = true;
                }
            } else if (aParam.alData.me === this._cancelBtn) {
                this._isHidden = false;
            }
            this._$setHiddenNetStge.ani({
                style : this.aniConfig.channelMenu.fadeOut,
                trans : {
                    dur : 200,
                    delay : 100
                },
                cb : ()=> {
                    this._$setHiddenNetStge.removeClass('active');
                    document.querySelector('#wrap').appendChild(this._$optSection[0]).appendChild(this._option.$[0]);
                    this._securityMenu.off();
                    this._connectBtn.$.removeClass('on');
                    this._cancelBtn.$.removeClass('on');
                    this._connectBtn.off();
                    this._cancelBtn.off();
                    this._ul.on();
                    this._ul.setItemFocus(this._showItemIndex);
                }
            });
        }
        return bConsumed;
    }
    _on_alExit (aParam) : boolean {
        var data = aParam.alData, bConsumed : boolean = false, idx : number;
        if (data) {
            if (data.me === this._securityMenu || data.me === this._connectBtn || data.me === this._cancelBtn) {
                bConsumed = true;
                this._$setHiddenNetStge.ani({
                    style : this.aniConfig.channelMenu.fadeOut,
                    trans : {
                        dur : 200,
                        delay : 100
                    },
                    cb :()=> {
                        this._$setHiddenNetStge.removeClass('active');
                        document.querySelector('#wrap').appendChild(this._$optSection[0]).appendChild(this._option.$[0]);
                        this._securityMenu.off();
                        this._connectBtn.$.removeClass('on');
                        this._cancelBtn.$.removeClass('on');
                        this._connectBtn.off();
                        this._cancelBtn.off();
                        this._ul.on();
                        this._ul.setItemFocus(this._showItemIndex);
                    }
                });
            }
        }
        return bConsumed;
    }
    _on_alScrollRight (aParam) : boolean {
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
    _on_alScrollLeft (param) : boolean {
        var data = param.alData, target = data.me, bConsumed : boolean = false;

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
        var that = this, bConsumed : boolean = false;
        if (aParam.alData.me === that._connectBtn || aParam.alData.me === that._cancelBtn) {
            bConsumed = true;
            if (aParam.alData.me === that._connectBtn) {
                that._connectBtn.$.removeClass('on');
                that._connectBtn.off();
            } else if (aParam.alData.me === that._cancelBtn) {
                that._cancelBtn.$.removeClass('on');
                that._cancelBtn.off();
            }
            that._passwdInput.$.addClass('on');
        } else if (aParam.alData.me === that._securityMenu) {
            bConsumed = true;
            that._securityMenu.off();
            that._connectBtn.$.addClass('on');
            that._connectBtn.on();
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
    _set_list () : any {
        var i : number;
        var _arr = this._wifi_arr;
        var _result = [];
        var viewCount : number;

        for ( i = 0; i < _arr.length; i += 1) {
            var o = _arr[i];
            var tmp : string = '';
            if (i !== this.SKIP_THIS_STEP) {
                tmp += (o._name + '<em><span class="signal int' + o._value + '"></span>');
                if (o._lock === 'true') {
                    tmp += ('<span class="lock"></span>');
                }
                tmp += ('</em>');
            } else {
                tmp = o._name;
            }
            _result.push(tmp);
        }

        if (_result.length > 5) {
            viewCount = 6;
        } else {
            viewCount = _result.length + 1;
        }
        return hx.al.component('wizList', {
            ul_attr : {
                'class' : 'net'
            },
            arr : _result,
            viewCount : viewCount
        });
    }
    _input_pass (aName) {
        var that = this, idx : number, _li : any, passwdTmp : any;

        passwdTmp = hx.msg('passwd', {
            text : hx.l('LOC_ENTER_PASSWORD_WIRELESS_ID'),
            responseText : false,
            wifi : aName,
            type : 'password',
            auto_close : false,
            timeout : 1000,
            dia_class : 'osd',
            callback_fn : fn
        });
        function fn(_status, _input) {
            if (_status === 'key_back' || _status === 'key_esc') {
                idx = that._ul.getShowItemFocus();
                _li = that._ul.$.children().eq(idx);
                _li.attr('class', 'focusable');
                that._ul.setItemFocus(idx);
            } else if (_status === hx.l('LOC_OK_ID')) {
                if (_input) {
                    that._status = "connecting";
                    that.progressMsg = hx.msg('progress', {
                        text : hx.l('LOC_MESG_5986_ID'),
                        _use_backkey : true,
                        dia_class : 'osd',
                        auto_close : false,
                        callback_fn : (_status)=> {
                            if (_status === 'key_back' || _status === 'key_esc') {
                                hx.svc.SETTING_UTIL.wirelessStopConnecting();
                                that._curState.isApConnecting = false;
                            }
                        }
                    });
                    hx.svc.SETTING_UTIL.applyConnectToAP({
                            index : that._currentConn,
                            password : _input.password
                    });
                    that._curState.isApConnecting = true;
                }
                hx.msg.close('passwd', passwdTmp);
            } else if (_status === 'close') {
                idx = that._ul.getShowItemFocus();
                _li = that._ul.$.children().eq(idx);
                _li.attr('class', 'focusable');
                that._ul.setItemFocus(idx);
            }
        }
    }
    _connect_start_wifi (aStatus?, aInput?) {
        //start wifi connect~!!!!!!
        var that = this;
        var idx = this._ul.getShowItemFocus();
        var _li = this._ul.$.children().eq(idx);
        //check _input~!!!!(wifi name, wifi password)
        this.progressMsg = hx.msg('progress', {
            text : hx.l('LOC_MESG_5986_ID'),
            auto_close : false, //change false~!!!!!
            dia_class : 'osd',
            _use_backkey : true,
            callback_fn : (aStatus : string)=> {
                if (aStatus === 'key_back' || aStatus === 'key_esc') {
                    hx.svc.SETTING_UTIL.wirelessStopConnecting();
                    that._curState.isApConnecting = false;
                    _li.attr('class', 'focusable');
                    that._ul.setItemFocus(idx);
                }
            }
        });
        hx.svc.SETTING_UTIL.applyConnectToAP({
                index : this._currentConn
        });
        that._curState.isApConnecting = true;
    }
    _connect_success_wifi () : void {
        //success wifi connect~!!!!!!
        this._isHidden = false;
        this._go_next_page();
    }
    _connect_fail_wifi () {
        var idx : number = this._ul.getShowItemFocus();
        var _li : any = this._ul.$.children().eq(idx);
        var responseText: string;
        if (this.reponseMsg) {
            hx.msg.close('response', this.reponseMsg);
        }
        if (this._isHidden) {
            responseText = hx.l('LOC_HIDDEN_NETWORK_NOT_FOUND_ID');
        } else {
            responseText = hx.l('LOC_NETWORK_CON_FAILED_ID');
        }
        this.reponseMsg = hx.msg('response', {
            text : responseText,
            auto_close : true,
            close_time : 3000,
            dia_class : 'res vaTop'
        });
        this._isHidden = false;
        _li.attr('class', 'focusable');
        this._ul.setItemFocus(idx);
    }
    _go_next_page () : void {
        if (this._isGoWizardPage === false) {
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(hx.al.CPgInstall05.items.nextPage + '?show=success', false);
        }
    }
    _go_next_page_fail () : void {
        if (this._isGoWizardPage === false) {
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni('CPgInstall08_1', false);
        }
    }
    _go_pre_page () : void {
        if (this._isGoWizardPage === false) {
            this._isGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(hx.al.CPgInstall05.items.prevPage + '?prev', true);
        }
    }
    _fadeOutAni (aTargetPage, aIsGoPrev : boolean) {
        var that = this, left : string;

        if (aIsGoPrev) {
            left = '20px';
        } else {
            left = '-20px';
        }

        if (that._ul) {
            hx.il.ani.action({
                targets : that._$article,
                cls : 'aniFadeOut',
                style : 'left:' + left
            });
            hx.il.ani.action({
                targets : that._$title,
                cls : 'aniFadeOut',
                cb : function() {
                    that.sendEventToEntity({
                        alEvent : 'alChangeInstallPage',
                        target : aTargetPage,
                        me : that
                    });
                }
            });
        } else {
            hx.il.ani.action({
                targets : that._$title,
                cls : 'aniFadeOut',
                cb : function() {
                    that.sendEventToEntity({
                        alEvent : 'alChangeInstallPage',
                        target : aTargetPage,
                        me : that
                    });
                }
            });
        }
    }
    _makeSecuritySubList (aParam : number) {
        var index : number, subFocusIndex : number = 0;
        var dataList = [];
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
hx.al.CPgInstall05 = cCPgInstall05;
/********************      Static Variables     ********************/
if (hx.config.wzd.page05) {
    hx.al.CPgInstall05.items = hx.config.wzd.page05;
} else {
    hx.al.CPgInstall05.items = {
        itemStr : [''],
        nextPage : 'CPgInstall06',
        prevPage : 'CPgInstall04',
        step : 'step2'
    };
}

hx.al.CPgInstall05.button = function(aParam) {
    return (new hx.al.component('button', {
        attr : {
            'class' : 'button',
            'data-langID' : aParam
        },
        upper : true,
        usePointerKey : true
    }));
};

hx.al.CPgInstall05.option = function(aParam) {
    return (new hx.al.component('button', {
        attr : {
            'class' : 'tit',
            'data-langID' : aParam
        },
        usePointerKey : true
    }));
};

hx.al.CPgInstall05.securityTypeMenu = function(aParam) {
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

hx.al.CPgInstall05.input = function(aParam) {
    return hx.al.component('input', {
        attr : {
            'id' : aParam.id,
            'type' : aParam.type,
            'style' : aParam.style
        },
    });
};

hx.al.CPgInstall05.prototype.aniConfig = {
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
export = cCPgInstall05;