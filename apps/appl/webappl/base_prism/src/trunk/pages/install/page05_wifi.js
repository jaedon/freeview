var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cCPgInstall05 = (function (_super) {
        __extends(cCPgInstall05, _super);
        function cCPgInstall05() {
            _super.call(this);
            this._wifi_arr = [];
            this._securityTypeValue = hx.l('LOC_NONE_ID');
            this._securityValueIndex = 0;
            this._networkName = '';
            this._password = '';
            this._showItemIndex = 0;
            this.wirelessSecurityType = ['WEP 64-bit Hex', 'WEP 64-bit ASCII', 'WEP 128-bit Hex', 'WEP 128-bit ASCII', 'WPA-PSK(TKIP)', 'WPA-PSK(AES)', 'WPA2-PSK(TKIP+AES)', 'WPA2-PSK(AES)'];
            this.wirelessSecurityTypeValue = [[0, 0, 0], [1, 0, 2], [1, 0, 1], [1, 0, 4], [1, 0, 3], [2, 1, 5], [2, 1, 6], [3, 1, 7], [3, 1, 6]];
            this.SKIP_THIS_STEP = 0; // CONST
        }
        cCPgInstall05.prototype.create = function (aEntity, aPageUrl) {
            hx.logadd('CPgInstall05');
            var that = this;
            var nowStep;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(false);
            that._backProcess = false;
            that._curState = {
                isApScannig: false,
                isApConnecting: false
            };
            that.apProgressMsg = hx.msg('progress', {
                text: hx.l('LOC_SEARCHING_AVAIL_NETWORK_ID'),
                _use_backkey: true,
                dia_class: 'osd',
                auto_close: true,
                timeout: 40000,
                callback_fn: cancel_fn
            });
            function cancel_fn(_status) {
                if (_status === 'key_back' && !that._backProcess) {
                    that._backProcess = true;
                    hx.svc.SETTING_UTIL.clearEventCb(that);
                    hx.svc.SETTING_UTIL.stopApScanning();
                    that._curState.isApScannig = false;
                    hx.msg.close('progress', that.apProgressMsg);
                    that._go_next_page_fail();
                }
                else if (_status === 'auto_close') {
                    that._curState.isApScannig = false;
                    hx.msg.close('progress', that.apProgressMsg);
                    hx.svc.SETTING_UTIL.clearEventCb(that);
                    that._stringEl.innerHTML = hx.l('LOC_NET_NOT_DETECTED_ID');
                    that._wifi_arr.push({
                        '_name': hx.l('LOC_SKIP_ID'),
                        '_value': '',
                        '_lock': false
                    });
                    that._ul = that._set_list();
                    that._$article.append(that._ul.$);
                    that.append(that._ul);
                    that._ul.on();
                    hx.il.ani.action({
                        targets: [that._$article],
                        cls: 'aniFadeIn',
                        style: 'left:0'
                    });
                }
            }
            hx.svc.SETTING_UTIL.addEventCb('eventWifiScanningResult', that, function (aParam) {
                that._on_eventWifiScanningResult(aParam);
            });
            hx.svc.SETTING_UTIL.addEventCb('wirelessLinkStatus', that, function (aResult) {
                that._on_wirelessLinkStatus(aResult);
            });
            hx.svc.SETTING_UTIL.apScanning();
            that._curState.isApScannig = true;
            $.extend(that, {
                _containerEl: document.querySelector('#wiz'),
                _$article: $('<article>', {
                    'class': 'cont'
                }),
                _stringEl: document.createElement('p'),
                _bottomDimEl: document.createElement('div'),
                _$hArea: $('<h1>', {
                    'class': 'hArea'
                }),
                _$title: $('<span>', {
                    'class': 'tit',
                    'html': hx.l('LOC_NETWORK_CONNECTION_ID').toUpperCase()
                }),
                _stepEl: document.createElement('span'),
                _$optSection: $('<section>', {
                    'id': 'opt'
                }),
                _option: hx.al.CPgInstall05.option('LOC_HIDDEN_NETWORK_ID'),
                _$setHiddenNetStge: $('<section>', {
                    'class': 'setStge'
                }),
                _$hiddenNetStatus: $('<h1>', {
                    'html': hx.l('LOC_HIDDEN_NETWORK_ID')
                }),
                _$titleHiddenNetArea: $('<p>', {
                    'class': 'tit area1 point',
                    'html': hx.l('LOC_ENTER_THE_NAME_HIDDEN_NETWORK_ID')
                }),
                _$securityMenu: $('<article>', {
                    'class': 'menu'
                }),
                _$securitySubMenu: $('<section>', {
                    'class': 'dep2'
                }),
                _$wrapInput: $('<div>', {
                    'class': 'content-wrapper',
                    'style': 'top:100px;'
                }),
                _$inputList: $('<dl>'),
                _$inputTitle: $('<dt>', {
                    'html': hx.l('LOC_NETWORK_NAME_ID').toUpperCase()
                }),
                _$inputArea: $('<dd>', {
                    'class': 'point'
                }),
                _$form: $('<form>', {
                    'method': 'post'
                }),
                _$inputList2: $('<dl>'),
                _$inputTitle2: $('<dt>', {
                    'html': hx.l('LOC_PASSWORD_ID')
                }),
                _$inputArea2: $('<dd>', {
                    'class': 'point'
                }),
                _$form2: $('<form>', {
                    'method': 'post'
                }),
                _$btnWrap: $('<div>', {
                    'class': 'wrap_btn'
                }),
                _networkNameInput: hx.al.CPgInstall05.input({
                    id: 'satInput',
                    type: 'text',
                    style: 'full'
                }),
                _passwdInput: hx.al.CPgInstall05.input({
                    id: 'satInput2',
                    type: 'password',
                    style: 'full'
                }),
                _securityMenu: hx.al.CPgInstall05.securityTypeMenu('LOC_SECURITY_TYPE_ID'),
                _connectBtn: hx.al.CPgInstall05.button('LOC_CONNECT_ID'),
                _cancelBtn: hx.al.CPgInstall05.button('LOC_CANCEL_ID')
            });
            that._$setHiddenNetStge.append(that._$hiddenNetStatus).append(that._$titleHiddenNetArea).append(that._$securityMenu.append(that._$securitySubMenu.after(that._securityMenu.$)).append(that._$wrapInput.append(that._$inputList.append(that._$inputTitle).append(that._$inputArea.append(that._$form.append(that._networkNameInput.$))).append(that._$inputTitle2).append(that._$inputArea2.append(that._$form2.append(that._passwdInput.$)))))).append(that._$btnWrap.append(that._connectBtn.$).append(that._cancelBtn.$));
            if (ENV.op === ENV.listOP.SES) {
                nowStep = 'step1';
            }
            else if (ENV.op === ENV.listOP.MIDDLE_EAST) {
                nowStep = 'step3';
            }
            else {
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
                targets: that._$hArea,
                cls: 'aniFadeIn'
            });
            that._isGoWizardPage = false;
            hx.al.compose(that);
            that._inputText = '';
            that._networkNameInput.$.val('');
            that._networkNameInput.$.bind('input', function () {
                that._inputText = $(this).val();
            });
            that._inputText2 = '';
            that._passwdInput.$.val('');
            that._passwdInput.$.bind('input', function () {
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
        };
        cCPgInstall05.prototype.destroy = function () {
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
                }
                else {
                    this._containerEl.style.display = 'none';
                    this._containerEl.removeChild(this._$hArea[0]);
                    this._containerEl.removeChild(this._$article[0]);
                    this._containerEl.removeChild(this._bottomDimEl);
                    this._containerEl.style.display = 'block';
                }
                this._containerEl = undefined;
            }
            if (this._$optSection && this._$optSection.remove) {
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
            var diaEl = document.querySelector('#DIA');
            if (diaEl) {
                diaEl.classList.remove('osd');
            }
            this._wifi_arr = [];
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(true);
        };
        cCPgInstall05.prototype._on_eventWifiScanningResult = function (aParam) {
            var listData = [], i, apData = {};
            hx.msg.close('progress', this.apProgressMsg);
            this._curState.isApScannig = false;
            if (aParam.apList.length === 0) {
                hx.svc.SETTING_UTIL.clearEventCb(this);
                this._stringEl.innerHTML = hx.l('LOC_NET_NOT_DETECTED_ID');
                this._wifi_arr.push({
                    '_name': hx.l('LOC_SKIP_ID'),
                    '_value': '',
                    '_lock': false
                });
                this._ul = this._set_list();
                this._$article.append(this._ul.$);
                this.append(this._ul);
                this._ul.on();
                hx.il.ani.action({
                    targets: [this._$article],
                    cls: 'aniFadeIn',
                    style: 'left:0'
                });
            }
            else {
                this.apList = aParam.apList;
                this._wifi_arr.push({
                    '_name': hx.l('LOC_SKIP_ID'),
                    '_value': '',
                    '_lock': false
                });
                for (i = 0; i < this.apList.length; i += 1) {
                    this._wifi_arr.push({
                        '_name': this.apList[i].apname,
                        '_value': String(this.apList[i].strength),
                        '_lock': this.apList[i].secure
                    });
                }
                this._ul = this._set_list();
                this._$article.append(this._ul.$);
                this.append(this._ul);
                this._ul.on();
                document.querySelector('#wrap').appendChild(this._$optSection[0]).appendChild(this._option.$[0]);
                this.append(this._option);
                hx.il.ani.action({
                    targets: [this._$article],
                    cls: 'aniFadeIn',
                    style: 'left:0'
                });
            }
        };
        cCPgInstall05.prototype._on_wirelessLinkStatus = function (aResult) {
            hx.log("debug', _on_wirelessLinkStatus - : aResult: " + aResult);
            hx.log("debug', _on_wirelessLinkStatus - : isApConnecting: " + this._curState.isApConnecting);
            if (this._curState.isApConnecting) {
                hx.msg.close('progress', this.progressMsg);
                this._curState.isApConnecting = false;
                this._status = 'notting';
                if (aResult === 'success') {
                    this._connect_success_wifi();
                }
                else {
                    this._connect_fail_wifi();
                }
            }
        };
        cCPgInstall05.prototype._makeHiddenDlg = function (param) {
            var that = this;
            that._$setHiddenNetStge.addClass('active');
            that._$setHiddenNetStge.ani({
                style: that.aniConfig.channelMenu.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    that.append(that._securityMenu).append(that._networkNameInput).append(that._passwdInput).append(that._connectBtn).append(that._cancelBtn);
                    that._securityMenu.on();
                }
            });
            that._$form.ani({
                style: that.aniConfig.channelMenu.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                }
            });
            that._$form2.ani({
                style: that.aniConfig.channelMenu.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                }
            });
            that._inputText = '';
            that._inputText2 = '';
            var $input = $('#satInput');
            $input.val('');
            $input.bind('input', function () {
                that._inputText = $(this).val();
            });
            var $input2 = $('#satInput2');
            $input2.val('');
            $input2.bind('input', function () {
                that._inputText2 = $(this).val();
            });
            var $securityValue = that._securityMenu.getItem(0).find('em');
            $securityValue.html(that._securityTypeValue);
        };
        cCPgInstall05.prototype._on_alKey = function (aParam) {
            var _this = this;
            var idx;
            var _key = aParam.alKey;
            var bConsumed = false;
            switch (_key) {
                case hx.key.KEY_BACK:
                case hx.key.KEY_LEFT:
                    bConsumed = true;
                    if (this._networkNameInput.$.hasClass('on') || this._passwdInput.$.hasClass('on')) {
                        this._networkNameInput.$.removeClass('on');
                        this._passwdInput.$.removeClass('on');
                        this._$setHiddenNetStge.ani({
                            style: this.aniConfig.channelMenu.fadeOut,
                            trans: {
                                dur: 200,
                                delay: 100
                            },
                            cb: function () {
                                _this._$setHiddenNetStge.removeClass('active');
                                document.querySelector('#wrap').appendChild(_this._$optSection[0]).appendChild(_this._option.$[0]);
                                _this._ul.on();
                                _this._ul.setItemFocus(_this._showItemIndex);
                            }
                        });
                    }
                    else if (this._$securitySubMenu.hasClass('active')) {
                        if (aParam.alData.me === this._securitySubList) {
                            this._securitySubList.off();
                            this._$securitySubMenu.ani({
                                style: this.aniConfig.dep2.fadeOut,
                                trans: {
                                    dur: 150,
                                    delay: 75
                                },
                                cb: function () {
                                    _this._securitySubList.$.remove();
                                    _this._$securitySubMenu.removeClass('active');
                                    _this._securityMenu.getItem(0).removeClass('his');
                                    _this._securityMenu.setItemFocus(0);
                                    _this._securityMenu.on();
                                }
                            });
                        }
                    }
                    else if (!this._backProcess) {
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
                    }
                    else if (this._passwdInput.$.hasClass('on')) {
                        this._passwdInput.$.removeClass('on');
                        this._networkNameInput.$.addClass('on');
                    }
                    break;
                case hx.key.KEY_DOWN:
                    bConsumed = true;
                    if (this._networkNameInput.$.hasClass('on')) {
                        this._networkNameInput.$.removeClass('on');
                        this._passwdInput.$.addClass('on');
                    }
                    else if (this._passwdInput.$.hasClass('on')) {
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
                    }
                    else if (this._passwdInput.$.hasClass('on')) {
                        this._passwdInput.$.blur();
                        this._passwdInput.$.focus();
                        bConsumed = true;
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgInstall05.prototype._on_alClicked = function (aParam) {
            var _this = this;
            var that = this;
            var bConsumed = false, idx;
            var dataList = [];
            var j, len;
            var currentFocus = 0;
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
                }
                else {
                    //input password~!!!!
                    this._input_pass(_li.text());
                }
            }
            else if (aParam.alData.me === this._securityMenu) {
                bConsumed = true;
                this._makeSecuritySubList(this._securityValueIndex);
            }
            else if (aParam.alData.me === this._securitySubList) {
                bConsumed = true;
                len = this.wirelessSecurityType.length + 1;
                dataList.push(hx.l('LOC_NONE_ID'));
                for (j = 0; j < len; j++) {
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
                    style: this.aniConfig.dep2.fadeOut,
                    trans: {
                        dur: 150,
                        delay: 75
                    },
                    cb: function () {
                        _this._securitySubList.$.remove();
                        _this._$securitySubMenu.removeClass('active');
                        _this._securityMenu.getItem(0).removeClass('his');
                        _this._securityMenu.setItemFocus(0);
                        _this._securityMenu.on();
                    }
                });
            }
            else if (aParam.alData.me === this._connectBtn || aParam.alData.me === this._cancelBtn) {
                bConsumed = true;
                if (aParam.alData.me === this._connectBtn) {
                    var paramValue = this.wirelessSecurityTypeValue;
                    var index = this._securityValueIndex;
                    if (this._inputText.length === 0 || this._inputText.indexOf(' ') === 0) {
                        hx.msg('response', {
                            text: hx.l('LOC_ENTER_NETWORK_NAME_ID'),
                            auto_close: true,
                            close_time: 3000,
                            dia_class: 'res vaTop'
                        });
                        return bConsumed;
                    }
                    else {
                        this._networkName = this._inputText;
                        this._password = this._inputText2;
                        this._status = "connecting";
                        this.progressMsg = hx.msg('progress', {
                            text: hx.l('LOC_MESG_5986_ID'),
                            _use_backkey: true,
                            auto_close: false,
                            callback_fn: function (_status) {
                                if (_status === 'key_back' || _status === 'key_esc') {
                                    hx.svc.SETTING_UTIL.wirelessStopConnecting();
                                }
                                that._curState.isApConnecting = false;
                            }
                        });
                        hx.svc.SETTING_UTIL.applyConnectToAP({
                            password: this._password,
                            ap: {
                                'essid': this._networkName,
                                'secure': paramValue[index][0],
                                'authen': paramValue[index][1],
                                'encrypt': paramValue[index][2]
                            }
                        });
                        this._curState.isApConnecting = true;
                    }
                }
                else if (aParam.alData.me === this._cancelBtn) {
                    this._isHidden = false;
                }
                this._$setHiddenNetStge.ani({
                    style: this.aniConfig.channelMenu.fadeOut,
                    trans: {
                        dur: 200,
                        delay: 100
                    },
                    cb: function () {
                        _this._$setHiddenNetStge.removeClass('active');
                        document.querySelector('#wrap').appendChild(_this._$optSection[0]).appendChild(_this._option.$[0]);
                        _this._securityMenu.off();
                        _this._connectBtn.$.removeClass('on');
                        _this._cancelBtn.$.removeClass('on');
                        _this._connectBtn.off();
                        _this._cancelBtn.off();
                        _this._ul.on();
                        _this._ul.setItemFocus(_this._showItemIndex);
                    }
                });
            }
            return bConsumed;
        };
        cCPgInstall05.prototype._on_alExit = function (aParam) {
            var _this = this;
            var data = aParam.alData, bConsumed = false, idx;
            if (data) {
                if (data.me === this._securityMenu || data.me === this._connectBtn || data.me === this._cancelBtn) {
                    bConsumed = true;
                    this._$setHiddenNetStge.ani({
                        style: this.aniConfig.channelMenu.fadeOut,
                        trans: {
                            dur: 200,
                            delay: 100
                        },
                        cb: function () {
                            _this._$setHiddenNetStge.removeClass('active');
                            document.querySelector('#wrap').appendChild(_this._$optSection[0]).appendChild(_this._option.$[0]);
                            _this._securityMenu.off();
                            _this._connectBtn.$.removeClass('on');
                            _this._cancelBtn.$.removeClass('on');
                            _this._connectBtn.off();
                            _this._cancelBtn.off();
                            _this._ul.on();
                            _this._ul.setItemFocus(_this._showItemIndex);
                        }
                    });
                }
            }
            return bConsumed;
        };
        cCPgInstall05.prototype._on_alScrollRight = function (aParam) {
            var data = aParam.alData, target = data.me, bConsumed = false;
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
        };
        cCPgInstall05.prototype._on_alScrollLeft = function (param) {
            var data = param.alData, target = data.me, bConsumed = false;
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
        };
        cCPgInstall05.prototype._on_alScrollup = function (aParam) {
            var that = this, bConsumed = false;
            if (aParam.alData.me === that._connectBtn || aParam.alData.me === that._cancelBtn) {
                bConsumed = true;
                if (aParam.alData.me === that._connectBtn) {
                    that._connectBtn.$.removeClass('on');
                    that._connectBtn.off();
                }
                else if (aParam.alData.me === that._cancelBtn) {
                    that._cancelBtn.$.removeClass('on');
                    that._cancelBtn.off();
                }
                that._passwdInput.$.addClass('on');
            }
            else if (aParam.alData.me === that._securityMenu) {
                bConsumed = true;
                that._securityMenu.off();
                that._connectBtn.$.addClass('on');
                that._connectBtn.on();
            }
            return bConsumed;
        };
        cCPgInstall05.prototype._on_alScrolldown = function (aParam) {
            var bConsumed = false;
            if (aParam.alData.me === this._securityMenu) {
                bConsumed = true;
                this._securityMenu.off();
                this._networkNameInput.$.addClass('on');
            }
            else if (aParam.alData.me === this._connectBtn || aParam.alData.me === this._cancelBtn) {
                bConsumed = true;
                if (aParam.alData.me === this._connectBtn) {
                    this._connectBtn.$.removeClass('on');
                    this._connectBtn.off();
                }
                else if (aParam.alData.me === this._cancelBtn) {
                    this._cancelBtn.$.removeClass('on');
                    this._cancelBtn.off();
                }
                this._securityMenu.on();
            }
            return bConsumed;
        };
        cCPgInstall05.prototype._set_list = function () {
            var i;
            var _arr = this._wifi_arr;
            var _result = [];
            var viewCount;
            for (i = 0; i < _arr.length; i += 1) {
                var o = _arr[i];
                var tmp = '';
                if (i !== this.SKIP_THIS_STEP) {
                    tmp += (o._name + '<em><span class="signal int' + o._value + '"></span>');
                    if (o._lock === 'true') {
                        tmp += ('<span class="lock"></span>');
                    }
                    tmp += ('</em>');
                }
                else {
                    tmp = o._name;
                }
                _result.push(tmp);
            }
            if (_result.length > 5) {
                viewCount = 6;
            }
            else {
                viewCount = _result.length + 1;
            }
            return hx.al.component('wizList', {
                ul_attr: {
                    'class': 'net'
                },
                arr: _result,
                viewCount: viewCount
            });
        };
        cCPgInstall05.prototype._input_pass = function (aName) {
            var that = this, idx, _li, passwdTmp;
            passwdTmp = hx.msg('passwd', {
                text: hx.l('LOC_ENTER_PASSWORD_WIRELESS_ID'),
                responseText: false,
                wifi: aName,
                type: 'password',
                auto_close: false,
                timeout: 1000,
                dia_class: 'osd',
                callback_fn: fn
            });
            function fn(_status, _input) {
                if (_status === 'key_back' || _status === 'key_esc') {
                    idx = that._ul.getShowItemFocus();
                    _li = that._ul.$.children().eq(idx);
                    _li.attr('class', 'focusable');
                    that._ul.setItemFocus(idx);
                }
                else if (_status === hx.l('LOC_OK_ID')) {
                    if (_input) {
                        that._status = "connecting";
                        that.progressMsg = hx.msg('progress', {
                            text: hx.l('LOC_MESG_5986_ID'),
                            _use_backkey: true,
                            dia_class: 'osd',
                            auto_close: false,
                            callback_fn: function (_status) {
                                if (_status === 'key_back' || _status === 'key_esc') {
                                    hx.svc.SETTING_UTIL.wirelessStopConnecting();
                                    that._curState.isApConnecting = false;
                                }
                            }
                        });
                        hx.svc.SETTING_UTIL.applyConnectToAP({
                            index: that._currentConn,
                            password: _input.password
                        });
                        that._curState.isApConnecting = true;
                    }
                    hx.msg.close('passwd', passwdTmp);
                }
                else if (_status === 'close') {
                    idx = that._ul.getShowItemFocus();
                    _li = that._ul.$.children().eq(idx);
                    _li.attr('class', 'focusable');
                    that._ul.setItemFocus(idx);
                }
            }
        };
        cCPgInstall05.prototype._connect_start_wifi = function (aStatus, aInput) {
            //start wifi connect~!!!!!!
            var that = this;
            var idx = this._ul.getShowItemFocus();
            var _li = this._ul.$.children().eq(idx);
            //check _input~!!!!(wifi name, wifi password)
            this.progressMsg = hx.msg('progress', {
                text: hx.l('LOC_MESG_5986_ID'),
                auto_close: false,
                dia_class: 'osd',
                _use_backkey: true,
                callback_fn: function (aStatus) {
                    if (aStatus === 'key_back' || aStatus === 'key_esc') {
                        hx.svc.SETTING_UTIL.wirelessStopConnecting();
                        that._curState.isApConnecting = false;
                        _li.attr('class', 'focusable');
                        that._ul.setItemFocus(idx);
                    }
                }
            });
            hx.svc.SETTING_UTIL.applyConnectToAP({
                index: this._currentConn
            });
            that._curState.isApConnecting = true;
        };
        cCPgInstall05.prototype._connect_success_wifi = function () {
            //success wifi connect~!!!!!!
            this._isHidden = false;
            this._go_next_page();
        };
        cCPgInstall05.prototype._connect_fail_wifi = function () {
            var idx = this._ul.getShowItemFocus();
            var _li = this._ul.$.children().eq(idx);
            var responseText;
            if (this.reponseMsg) {
                hx.msg.close('response', this.reponseMsg);
            }
            if (this._isHidden) {
                responseText = hx.l('LOC_HIDDEN_NETWORK_NOT_FOUND_ID');
            }
            else {
                responseText = hx.l('LOC_NETWORK_CON_FAILED_ID');
            }
            this.reponseMsg = hx.msg('response', {
                text: responseText,
                auto_close: true,
                close_time: 3000,
                dia_class: 'res vaTop'
            });
            this._isHidden = false;
            _li.attr('class', 'focusable');
            this._ul.setItemFocus(idx);
        };
        cCPgInstall05.prototype._go_next_page = function () {
            if (this._isGoWizardPage === false) {
                this._isGoWizardPage = true;
                hx.al.decompose(this);
                this._fadeOutAni(hx.al.CPgInstall05.items.nextPage + '?show=success', false);
            }
        };
        cCPgInstall05.prototype._go_next_page_fail = function () {
            if (this._isGoWizardPage === false) {
                this._isGoWizardPage = true;
                hx.al.decompose(this);
                this._fadeOutAni('CPgInstall08_1', false);
            }
        };
        cCPgInstall05.prototype._go_pre_page = function () {
            if (this._isGoWizardPage === false) {
                this._isGoWizardPage = true;
                hx.al.decompose(this);
                this._fadeOutAni(hx.al.CPgInstall05.items.prevPage + '?prev', true);
            }
        };
        cCPgInstall05.prototype._fadeOutAni = function (aTargetPage, aIsGoPrev) {
            var that = this, left;
            if (aIsGoPrev) {
                left = '20px';
            }
            else {
                left = '-20px';
            }
            if (that._ul) {
                hx.il.ani.action({
                    targets: that._$article,
                    cls: 'aniFadeOut',
                    style: 'left:' + left
                });
                hx.il.ani.action({
                    targets: that._$title,
                    cls: 'aniFadeOut',
                    cb: function () {
                        that.sendEventToEntity({
                            alEvent: 'alChangeInstallPage',
                            target: aTargetPage,
                            me: that
                        });
                    }
                });
            }
            else {
                hx.il.ani.action({
                    targets: that._$title,
                    cls: 'aniFadeOut',
                    cb: function () {
                        that.sendEventToEntity({
                            alEvent: 'alChangeInstallPage',
                            target: aTargetPage,
                            me: that
                        });
                    }
                });
            }
        };
        cCPgInstall05.prototype._makeSecuritySubList = function (aParam) {
            var _this = this;
            var index, subFocusIndex = 0;
            var dataList = [];
            var j, len = this.wirelessSecurityType.length;
            dataList.push(hx.l('LOC_NONE_ID'));
            for (j = 0; j < len; j++) {
                dataList.push(this.wirelessSecurityType[j]);
            }
            this._securitySubList = hx.al.component('simpleMenu', {
                attr: {
                    'style': 'display:block;',
                    'class': 'subList'
                },
                currentindex: aParam,
                displayCount: dataList.length,
                _isNotId: true
            });
            this._$securitySubMenu.addClass('active');
            this._securityMenu.getItem(0).addClass('his');
            this._$securitySubMenu.append(this._securitySubList.$);
            this._securitySubList.bind(dataList);
            this._securitySubList.draw();
            this._$securitySubMenu.ani({
                style: this.aniConfig.dep2.fadeIn,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    _this.append(_this._securitySubList);
                    _this._securitySubList.on();
                }
            });
        };
        return cCPgInstall05;
    })(__PAGE_KERNEL__);
    hx.al.CPgInstall05 = cCPgInstall05;
    /********************      Static Variables     ********************/
    if (hx.config.wzd.page05) {
        hx.al.CPgInstall05.items = hx.config.wzd.page05;
    }
    else {
        hx.al.CPgInstall05.items = {
            itemStr: [''],
            nextPage: 'CPgInstall06',
            prevPage: 'CPgInstall04',
            step: 'step2'
        };
    }
    hx.al.CPgInstall05.button = function (aParam) {
        return (new hx.al.component('button', {
            attr: {
                'class': 'button',
                'data-langID': aParam
            },
            upper: true,
            usePointerKey: true
        }));
    };
    hx.al.CPgInstall05.option = function (aParam) {
        return (new hx.al.component('button', {
            attr: {
                'class': 'tit',
                'data-langID': aParam
            },
            usePointerKey: true
        }));
    };
    hx.al.CPgInstall05.securityTypeMenu = function (aParam) {
        var securityTypeMainMenu = hx.al.component('settingsList', {
            itemAttr: {},
            attr: {
                'class': 'dep1'
            },
            roll: 'notroll',
            data: aParam,
            count: 1,
            fx: function (i, o) {
                o.html(hx.l(this.data)).append($('<em>'));
            },
            keyTable: [hx.key.KEY_OK, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_BACK, hx.key.KEY_ESCAPE, hx.key.KEY_RIGHT, hx.key.KEY_LEFT]
        });
        return securityTypeMainMenu;
    };
    hx.al.CPgInstall05.input = function (aParam) {
        return hx.al.component('input', {
            attr: {
                'id': aParam.id,
                'type': aParam.type,
                'style': aParam.style
            },
        });
    };
    hx.al.CPgInstall05.prototype.aniConfig = {
        fadeIn: {
            start: {
                'opacity': 0
            },
            end: {
                'opacity': 1
            }
        },
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
    return cCPgInstall05;
});
