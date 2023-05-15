var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "layer_domain/service/dl_dsrvNetflix"], function (require, exports, __PAGE_KERNEL__, __NETFLIX__) {
    ///<reference path="../../def/sugar.d.ts" />
    var cCPgMessage = (function (_super) {
        __extends(cCPgMessage, _super);
        function cCPgMessage(aServiceName) {
            _super.call(this, aServiceName);
        }
        cCPgMessage.prototype.create = function () {
            var that = this;
            hx.logadd('CPgMessage');
            $.extend(that, {
                _article: hx.al.component('frame', {
                    _focusException: true,
                    'tag': 'article',
                    'attr': {
                        'id': 'DIA'
                    }
                }),
                $wrap: $('#wrap')
            });
            //that.$wrap = $('#wrap');
            that.$wrap.append(that._article.$);
            hx.al.compose(that).append(that._article);
            that._article.on();
            that._dialogList = [];
            //define
            var pgMessage = that;
            var exceptionDlg = 0;
            hx.msg = function (name, param) {
                //pgMessage._removeDialog(0);
                if (hx.config.netflix) {
                    if (hx.svc.NETFLIX.isRunning()) {
                        if (param.validOnNetflix === undefined) {
                            hx.log("info", "[hx.msg] .... return due to run Netflix !");
                            return;
                        }
                        if (param.validOnNetflix === true) {
                            hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.OPEN_WITHOUT_KEY);
                        }
                    }
                }
                if (param.dia_class !== undefined) {
                    that._article.$.attr('class', param.dia_class);
                }
                return pgMessage._appendDialog(name, param);
            };
            hx.msg.close = function (handle, dialog) {
                pgMessage._removeDialog(handle, dialog, 'close');
            };
            hx.msg.addExceptionalDlg = function () {
                exceptionDlg += 1;
            };
            hx.msg.removeExceptionalDlg = function () {
                if (exceptionDlg > 0) {
                    exceptionDlg -= 1;
                }
            };
            hx.msg.initExceptionalDlg = function () {
                exceptionDlg = 0;
            };
            hx.msg.progress = function (handler, total, index, dialog) {
                pgMessage._changeProgressBar(handler, total, index, dialog);
            };
            hx.msg.isOpend = function () {
                var ret = that._visible; // is opend user interaction dilalog??
                if (!ret) {
                    if (exceptionDlg) {
                        ret = true;
                    }
                }
                return ret;
            };
            hx.msg.isOpendWithName = function (name) {
                var i, _list = that._dialogList;
                var visibleCnt = 0;
                for (i = 0; i < _list.length; i += 1) {
                    if (_list[i].name === name) {
                        visibleCnt += 1;
                    }
                }
                return (visibleCnt) ? true : false;
            };
        };
        cCPgMessage.prototype.destroy = function () {
            this._article.off();
        };
        cCPgMessage.prototype._updateVisible = function () {
            var i, _list = this._dialogList;
            var visibleCnt = 0;
            for (i = 0; i < _list.length; i += 1) {
                var o = _list[i].cmpt;
                if (o) {
                    if (o._visible === true) {
                        visibleCnt += 1;
                    }
                }
            }
            var oldVisible = this._visible;
            if (visibleCnt > 0) {
                this._visible = true;
            }
            else {
                this._visible = false;
            }
            if (oldVisible !== this._visible) {
                this.sendEventToEntity({
                    alEvent: 'alCheckPageVisible'
                });
            }
            hx.log('CPgMessage', 'messagebox _updateVisible = ' + visibleCnt);
        };
        // EVENT HANDLER
        cCPgMessage.prototype._on_alKey = function (aParam) {
            var that = this;
            if (!that._dialogList || !that._dialogList.length) {
                return false;
            }
            var i, tmp, index;
            var bConsumed = false;
            var kc = aParam.alKey;
            switch (kc) {
                case hx.key.KEY_POWER:
                    if (!aParam.alData) {
                        hx.log('error', '[messagebox][alKey]no param.alData');
                        hx.log('error', '[messagebox][alKey]no param.alData');
                    }
                    else {
                        tmp = 'key_power';
                        for (i = 0; i < that._dialogList.length; i += 1) {
                            o = that._dialogList[i];
                            if (o.cmpt === aParam.alData.me) {
                                that._removeDialog(o.name, o.cmpt, tmp);
                                bConsumed = false;
                                break;
                            }
                        }
                    }
                    break;
                case hx.key.KEY_BACK:
                    if (!aParam.alData) {
                        hx.log('error', '[messagebox][alKey]no param.alData');
                        hx.log('error', '[messagebox][alKey]no param.alData');
                    }
                    else {
                        tmp = 'key_back';
                        for (i = 0; i < that._dialogList.length; i += 1) {
                            var o = that._dialogList[i];
                            if (o.cmpt === aParam.alData.me) {
                                if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) {
                                    /* A fingerprint doesn't close by the exit key. */
                                    if (o.name === 'attrDispMsg' && (typeof o.cmpt.isFp == 'function') && (o.cmpt.isFp() == true))
                                        continue;
                                }
                                if (hx.config.useNetflixPW && hx.svc.NETFLIX.getPwStatus()) {
                                    that.sendEventToEntity({
                                        alEvent: 'alMessageToPage',
                                        target: 'CPgMain/CloseNetflixPW',
                                        me: that
                                    });
                                    that.sendEventToEntity({
                                        alEvent: 'alMessageToPage',
                                        target: 'CPgLiveController?showpin=pin'
                                    });
                                    bConsumed = true;
                                }
                                else if (o.name !== 'pinPC') {
                                    that._removeDialog(o.name, o.cmpt, tmp);
                                    bConsumed = true;
                                }
                                break;
                            }
                        }
                    }
                    break;
                case hx.key.KEY_ESCAPE:
                    if (!aParam.alData) {
                        hx.log('error', '[messagebox][alKey]no param.alData');
                        hx.log('error', '[messagebox][alKey]no param.alData');
                    }
                    else {
                        tmp = 'key_esc';
                        for (i = 0; i < that._dialogList.length; i += 1) {
                            var o = that._dialogList[i];
                            if (o.cmpt === aParam.alData.me) {
                                if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) {
                                    /* A fingerprint doesn't close by the exit key. */
                                    if (o.name === 'attrDispMsg' && (typeof o.cmpt.isFp == 'function') && (o.cmpt.isFp() == true))
                                        continue;
                                }
                                if (hx.config.useNetflixPW && hx.svc.NETFLIX.getPwStatus()) {
                                    that.sendEventToEntity({
                                        alEvent: 'alMessageToPage',
                                        target: 'CPgMain/CloseNetflixPW',
                                        me: that
                                    });
                                    that.sendEventToEntity({
                                        alEvent: 'alMessageToPage',
                                        target: 'CPgLiveController?showpin=pin'
                                    });
                                    bConsumed = true;
                                }
                                else {
                                    that._removeDialog(o.name, o.cmpt, tmp);
                                    bConsumed = true;
                                }
                                break;
                            }
                        }
                    }
                    break;
                case hx.key.KEY_OK:
                    if (!aParam.alData) {
                        hx.log('error', '[messagebox][alKey]no param.alData');
                        hx.log('error', '[messagebox][alKey]no param.alData');
                    }
                    else {
                        tmp = 'key_ok';
                        for (i = 0; i < that._dialogList.length; i += 1) {
                            var o = that._dialogList[i];
                            if (o.cmpt === aParam.alData.me) {
                                if (o.name === 'forcedMailPopup') {
                                    that._removeDialog(o.name, o.cmpt, tmp);
                                    bConsumed = true;
                                }
                                if (o.name === 'attrDispMsg') {
                                    // if it is fingerprint, skip the ok message
                                    if ((typeof o.cmpt.isFp == 'function') && (o.cmpt.isFp() == true))
                                        continue;
                                    // otherwise, close it since it is a forced attributed display message.
                                    that._removeDialog(o.name, o.cmpt, tmp);
                                    bConsumed = true;
                                }
                                break;
                            }
                        }
                    }
                    break;
                case hx.key.KEY_MENU:
                case hx.key.KEY_CHANNEL_UP:
                case hx.key.KEY_CHANNEL_DOWN:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_PLAYER:
                case hx.key.KEY_ON_DEMAND:
                    if (!aParam.alData) {
                        hx.log('error', '[messagebox][alKey]no param.alData');
                        hx.log('error', '[messagebox][alKey]no param.alData');
                    }
                    else {
                        tmp = kc;
                        for (i = 0; i < that._dialogList.length; i += 1) {
                            var o = that._dialogList[i];
                            if (o.cmpt === aParam.alData.me) {
                                if (o.cmpt._useOtherKey) {
                                    that._removeDialog(o.name, o.cmpt, tmp);
                                    bConsumed = false; //pass key to live
                                }
                                break;
                            }
                        }
                    }
                    break;
            }
            return bConsumed;
        };
        cCPgMessage.prototype._on_alClicked = function (aParam) {
            var i, ti;
            for (i = 0, ti = this._dialogList.length; i < ti; i += 1) {
                var dialog = this._dialogList[i];
                if (dialog) {
                    if (dialog.cmpt === aParam.alData.me) {
                        this['__clicked_' + dialog.name](aParam);
                    }
                }
            }
            return true;
        };
        cCPgMessage.prototype._on_alMessageToPage = function (aParam) {
            this._set_time_out(aParam);
        };
        // PRIVATE FUNCTION FOR CONTENT MAKING
        cCPgMessage.prototype._hasDialog = function () {
            return ($("#wrap").children().length > 0);
        };
        cCPgMessage.prototype._appendDialog = function (aNname, aParam) {
            var that = this;
            var dialog = that['__make_' + aNname](aParam);
            that.append(dialog);
            if (!!aParam.attr) {
                that._article.$.addClass(aParam.attr);
            }
            switch (aNname) {
                case 'status':
                case 'dolby':
                case 'dolbyPlus':
                case 'notAvable':
                    that.$wrap.append(dialog.$);
                    that._dialogList.push({
                        'name': aNname,
                        'cmpt': dialog,
                        //'timer' : timer,
                        'param': aParam
                    });
                    break;
                default:
                    that._article.$.append(dialog.$);
                    that._dialogList.push({
                        'name': aNname,
                        'cmpt': dialog,
                        //'timer' : timer,
                        'param': aParam
                    });
                    dialog.on();
                    break;
            }
            this._modifyCSS(dialog, aNname);
            // if there is doDraw function, then do it!
            if (typeof dialog.doDraw == 'function') {
                //after appending the dialog to DOM, process the doDraw function, since some properties(eg, height) are available after the dialog on DOM.
                dialog.doDraw();
            }
            //return 0;
            if ((aParam.auto_close === undefined ? aParam.auto_close = true : aParam.auto_close)) {
                if (!aParam.timeout) {
                    aParam.timeout = 3000;
                }
                //clearTimeout(dialog._timerId);
                dialog._timerId = setTimeout(function () {
                    that._removeDialog(aNname, dialog, 'auto_close');
                    if (hx.config.netflix) {
                        if (hx.svc.NETFLIX.isRunning()) {
                            if (aParam.validOnNetflix === true) {
                                hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.CLOSE);
                            }
                        }
                    }
                }, aParam.timeout);
            }
            //animation
            function _doAnimation($target) {
                for (i = 0; i < $target.length; i += 1) {
                    if ($target.eq(i).hasClass('aniActive')) {
                        continue;
                    }
                    else {
                        $selectedTarget = $target.eq(i);
                        break;
                    }
                }
                if ($selectedTarget) {
                    hx.il.ani.action({
                        type: 'in',
                        targets: $selectedTarget
                    });
                }
            }
            var $target, i, $selectedTarget;
            switch (aNname) {
                case 'response':
                case 'progress':
                case 'recordStatus':
                    $target = $('#DIA .vaTop');
                    _doAnimation($target);
                    break;
                case 'confirm':
                case 'codeConfirm':
                case 'ssuDialog':
                case 'remindDialog':
                case 'checkDialog':
                case 'select':
                case 'selectChlist':
                    hx.svc.SETTING_UTIL.showMsgbox();
                    $target = $('#DIA .vaBot');
                    _doAnimation($target);
                    break;
                case 'pin':
                case 'pinPC':
                case 'twopin':
                case 'inputtime':
                case 'inputonetime':
                case 'inputtext':
                case 'passwd':
                    hx.svc.SETTING_UTIL.showMsgbox();
                    $target = $('#DIA .pas');
                    _doAnimation($target);
                    break;
                case 'chUpdate':
                    hx.svc.SETTING_UTIL.showMsgbox();
                    $target = $('#DIA .chUpdate');
                    _doAnimation($target);
                    hx.svc.SETTING_UTIL.showMsgbox();
                    break;
                default:
                    hx.svc.SETTING_UTIL.showMsgbox();
            }
            that._updateVisible();
            if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) {
                that._moveFingerprintOnTopOsd();
            }
            hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
            return dialog;
        };
        cCPgMessage.prototype._set_time_out = function (aParam) {
            var _this = this;
            var i, ti;
            for (i = 0, ti = this._dialogList.length; i < ti; i += 1) {
                var dialog = this._dialogList[i];
                if (dialog.cmpt === aParam.alData.me) {
                    if ((dialog.param.auto_close === undefined ? dialog.param.auto_close = true : dialog.param.auto_close)) {
                        clearTimeout(dialog.cmpt._timerId);
                        dialog.cmpt._timerId = setTimeout(function () {
                            _this._removeDialog(dialog.name, dialog.cmpt, 'auto_close');
                        }, dialog.param.timeout);
                    }
                    break;
                }
            }
            return true;
        };
        //_timerId : 0,
        cCPgMessage.prototype._modifyCSS = function (aDialog, aName) {
            var _h, _t, tmp;
            if (aName === 'status') {
                _h = parseInt(aDialog.$.css('height'));
                _t = '';
                tmp = _h / 34;
                _t = 310 - (17 * (tmp - 1));
                aDialog.$.css('top', (_t + 'px'));
            }
            else if (aName === 'pin' || aName === 'pinPC') {
                _h = parseInt(aDialog.$.css('height'));
                _t = 0;
                if (_h <= 121) {
                    _t = 270;
                }
                else if (_h <= 162) {
                    _t = 253;
                }
                else if (_h <= 196) {
                    _t = 236;
                }
                else {
                    _t = 219;
                }
                aDialog.$.css('top', (_t + 'px'));
            }
            else if (aName === 'progress') {
                _h = parseInt(aDialog.$.css('height'));
                if (_h > 34) {
                    aDialog.$.attr('class', 'pro vaTop');
                }
            }
            else if (aName === 'progressBar') {
                _h = parseInt(aDialog.$.css('height'));
                _t = '';
                tmp = (_h - 18) / 34;
                _t = 310 - (17 * (tmp - 1));
                aDialog.$.css('top', (_t + 'px'));
                if (_t > 310) {
                    aDialog.$.attr('class', 'pro');
                }
            }
            else if (aName === 'progressBtn') {
                _h = parseInt(aDialog.$.css('height'));
                if (_h <= 123) {
                    aDialog.$.attr('class', 'pro vaBot line1');
                }
                else {
                    aDialog.$.attr('class', 'pro vaBot');
                }
            }
        };
        cCPgMessage.prototype._removeDialog = function (aHandle, aDialog, _msg) {
            var that = this;
            var i, _list = that._dialogList;
            for (i = 0; i < _list.length; i += 1) {
                var o = _list[i];
                if (o.name === aHandle && o.cmpt == aDialog) {
                    var result = o.cmpt;
                    if (o.param.callback_fn && _msg) {
                        o.param.callback_fn(_msg);
                    }
                    result.pickout();
                    _list.splice(i, 1);
                    break;
                }
            }
            if (_list.length > 0) {
                if (aHandle !== 'status') {
                    var bFocused = false;
                    for (i = _list.length - 1; i >= 0; i -= 1) {
                        if (bFocused) {
                            break;
                        }
                        if (_list[i]) {
                            switch (_list[i].name) {
                                case 'status':
                                case 'dolby':
                                case 'dolbyPlus':
                                case 'notAvable':
                                    break;
                                default:
                                    _list[i].cmpt.on();
                                    bFocused = true;
                                    break;
                            }
                        }
                    }
                }
            }
            else {
                that._article.$.attr('class', '');
            }
            that._updateVisible();
            hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        };
        cCPgMessage.prototype._changeProgressBar = function (aHandler, aTotal, aIndex, aDialog) {
            var i, ti;
            for (i = 0, ti = this._dialogList.length; i < ti; i += 1) {
                var o = this._dialogList[i];
                if (o.name === aHandler && o.cmpt === aDialog) {
                    aTotal = aTotal - 0;
                    aIndex = aIndex - 0;
                    var tmp = ((100 * aIndex) / aTotal);
                    if (tmp > 100) {
                        tmp = 100;
                    }
                    if (tmp < 0) {
                        tmp = 0;
                    }
                    o.cmpt._progress_bar_2.css('width', tmp + '%');
                    break;
                }
            }
        };
        cCPgMessage.prototype._moveFingerprintOnTopOsd = function () {
            var i = 0;
            var bFpFind = false;
            for (i = 0; i < this._dialogList.length; i += 1) {
                var o = this._dialogList[i];
                if (o.name === 'attrDispMsg' && (typeof o.cmpt.isFp == 'function') && (o.cmpt.isFp() == true)) {
                    bFpFind = true;
                    break;
                }
            }
            /* 2. If fingerrpint exist and it is not the last one, then move it at the end.(Move it on the top of OSD) */
            if (bFpFind && i != (this._dialogList.length - 1)) {
                var o = this._dialogList[i];
                // move fingerprint ui on the top
                this._article.$.detach(o.cmpt.$);
                this._article.$.append(o.cmpt.$);
                // move fingerprint object to the end of the list.
                this._dialogList.splice(i, 1);
                this._dialogList.push({
                    'name': o.name,
                    'cmpt': o.cmpt,
                    //'timer' : timer,
                    'param': o.param
                });
            }
        };
        /**message list start*******************************************************/
        cCPgMessage.prototype.__make_response = function (aParam) {
            var message = hx.al.component('response', {
                '_msg_type': 'response',
                'text': aParam.text,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_confirm = function (aParam) {
            var message = hx.al.component('confirm', {
                '_visible': true,
                '_msg_type': 'confirm',
                'text': aParam.text,
                'html': aParam.html,
                'first_line': aParam.first_line,
                'useOtherKey': aParam.useOtherKey,
                'isCenter': aParam.isCenter,
                'doNotUseBackExitKey': aParam.doNotUseBackExitKey,
                'btn_title_arr': aParam.btn_title_arr,
                'btn_last_focus': aParam.btn_last_focus,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_codeConfirm = function (aParam) {
            var message = hx.al.component('confirm', {
                '_visible': true,
                '_msg_type': 'codeConfirm',
                'text': aParam.text,
                'html': aParam.html,
                'showRegi': aParam.showRegi,
                'thirdLineFont': aParam.thirdLineFont,
                'first_line': aParam.first_line,
                'regcode': aParam.regcode,
                'nolinejump': aParam.nolinejump,
                'second_line': aParam.second_line,
                'btn_title_arr': aParam.btn_title_arr,
                'callback_fn': aParam.callback_fn,
            });
            return message;
        };
        cCPgMessage.prototype.__make_greenButton = function (aParam) {
            var message = hx.al.component('greenButton', {
                '_visible': true,
                'programmes': aParam.programmes,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_watchinHDButton = function (aParam) {
            var message = hx.al.component('watchinHDButton', {
                '_visible': true,
                'linkage': aParam.linkage,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_select = function (aParam) {
            var message = hx.al.component('confirm', {
                '_visible': true,
                '_msg_type': 'select',
                'text': aParam.text,
                'btn_title_arr': aParam.btn_title_arr,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_progress = function (aParam) {
            var message = hx.al.component('response', {
                '_msg_type': 'progress',
                '_use_backkey': aParam._use_backkey,
                'text': aParam.text,
                'enable_key': aParam.enable_key
            });
            return message;
        };
        cCPgMessage.prototype.__make_progressBar = function (aParam) {
            var message = hx.al.component('progressBar', {
                '_msg_type': 'progressBar',
                'text': aParam.text,
                //'auto_close' : param.auto_close, //true(default), false
                //'timeout' : param.timeout, //number (default:3)
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_progressBarBtn = function (aParam) {
            var message = hx.al.component('progressBar', {
                '_visible': true,
                '_msg_type': 'progressBarBtn',
                'text': aParam.text,
                'btn_title': aParam.btn_title,
                //'auto_close' : param.auto_close, //true(default), false
                //'timeout' : param.timeout, //number (default:3)
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_progressBtn = function (aParam) {
            var message = hx.al.component('progressBar', {
                '_visible': true,
                '_msg_type': 'progressBtn',
                'text': aParam.text,
                'btn_title': aParam.btn_title,
                //'auto_close' : param.auto_close, //true(default), false
                //'timeout' : param.timeout, //number (default:3)
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_ssuDialog = function (aParam) {
            var message = hx.al.component('confirm', {
                '_visible': true,
                '_msg_type': 'ssuDialog',
                'title': aParam.title,
                'text': aParam.text,
                'text1': aParam.text1,
                'html': aParam.html,
                'first_line_arr': aParam.first_line_arr,
                'second_line_arr': aParam.second_line_arr,
                'third_line_arr': aParam.third_line_arr,
                'btn_title_arr': aParam.btn_title_arr,
                //'auto_close' : param.auto_close, //true(default), false
                //'timeout' : param.timeout, //number (default:3)
                'doNotUseBackExitKey': aParam.doNotUseBackExitKey,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_remindDialog = function (aParam) {
            var message = hx.al.component('confirm', {
                '_visible': true,
                '_msg_type': 'remindDialog',
                'text': aParam.text,
                'first_line': aParam.first_line,
                'second_line_arr': aParam.second_line_arr,
                'btn_title_arr': aParam.btn_title_arr,
                //'auto_close' : param.auto_close, //true(default), false
                //'timeout' : param.timeout, //number (default:3)
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_checkDialog = function (aParam) {
            var message = hx.al.component('confirm', {
                '_visible': true,
                '_msg_type': 'checkDialog',
                'title': aParam.title,
                'text': aParam.text,
                'html': aParam.html,
                'checkText': aParam.checkText,
                'btn_title_arr': aParam.btn_title_arr,
                'timeout': aParam.timeout,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_status = function (aParam) {
            var message = hx.al.component('cMessage', {
                'text': aParam.text,
                'type': 'status',
                _focusException: true
            });
            return message;
        };
        cCPgMessage.prototype.__make_pin = function (aParam) {
            var pin = hx.al.component('pin', {
                '_visible': true,
                'type': 'pin',
                'tag': 'div',
                'attr': {
                    'class': 'pas'
                },
                'text': aParam.text,
                'html': aParam.html,
                'first_line': aParam.first_line,
                'second_line': aParam.second_line,
                'enable': aParam.enable,
                'callback_fn': aParam.callback_fn
            });
            return pin;
        };
        cCPgMessage.prototype.__make_pinPC = function (aParam) {
            var pin = hx.al.component('pinPC', {
                //'_visible' : true,
                'type': 'pin',
                'tag': 'div',
                'attr': {
                    'class': 'pas'
                },
                'text': aParam.text,
                'enable': aParam.enable,
                'callback_fn': aParam.callback_fn,
                'auto_close': aParam.auto_close,
                'timeout': aParam.timeout
            });
            return pin;
        };
        cCPgMessage.prototype.__make_twopin = function (aParam) {
            var pin = hx.al.component('twopin', {
                '_visible': true,
                'type': 'pin',
                'tag': 'div',
                'attr': {
                    'class': 'pas',
                    'style': 'top: 253px'
                },
                'text': aParam.text,
                'enable': aParam.enable,
                'callback_fn': aParam.callback_fn
            });
            return pin;
        };
        cCPgMessage.prototype.__make_inputtime = function (aParam) {
            var inputtime = hx.al.component('inputtime', {
                '_visible': true,
                'type': 'pin',
                'tag': 'div',
                'attr': {
                    'class': 'pas multi',
                    'style': 'top: 253px'
                },
                'text': aParam.text,
                'enable': aParam.enable,
                'callback_fn': aParam.callback_fn
            });
            return inputtime;
        };
        cCPgMessage.prototype.__make_inputonetime = function (aParam) {
            var inputonetime = hx.al.component('inputonetime', {
                '_visible': true,
                'type': 'pin',
                'tag': 'div',
                'attr': {
                    'class': 'pas multi',
                    'style': 'top: 253px'
                },
                'text': aParam.text,
                'enable': aParam.enable,
                'callback_fn': aParam.callback_fn
            });
            return inputonetime;
        };
        cCPgMessage.prototype.__make_inputtext = function (aParam) {
            var input = hx.al.component('inputtext', {
                '_visible': true,
                'type': aParam.type,
                'callback_fn': aParam.callback_fn
            });
            return input;
        };
        cCPgMessage.prototype.__make_passwd = function (aParam) {
            var pin = hx.al.component('passwd', {
                '_visible': true,
                'wifi': aParam.wifi,
                'responseText': aParam.responseText,
                'text': aParam.text,
                'string': aParam.string,
                'type': aParam.type,
                'callback_fn': aParam.callback_fn,
                'maxLength': aParam.maxLength ? 35 : 32
            });
            return pin;
        };
        cCPgMessage.prototype.__make_lipsync = function (aParam) {
            this._article.$.addClass('lipSync');
            var lipsync = hx.al.component('lipsync', {
                //'type' : 'pin',
                'tag': 'dl',
                'attr': {
                    'class': 'progress type5'
                },
                'auto_close': aParam.auto_close,
                'lipsyncDelay': aParam.lipsyncDelay,
                //'text' : param.text,
                //'enable' : param.enable,
                'callback_fn': aParam.callback_fn
            });
            return lipsync;
        };
        cCPgMessage.prototype.__make_subtitleSync = function (aParam) {
            this._article.$.addClass('subtitleSync');
            var subtitleSync = hx.al.component('subtitleSync', {
                'tag': 'dl',
                'attr': {
                    'class': 'progress type5'
                },
                'auto_close': aParam.auto_close,
                'syncDelay': aParam.syncDelay,
                'callback_fn': aParam.callback_fn
            });
            return subtitleSync;
        };
        /**message list recording policy*******************************************************/
        cCPgMessage.prototype.__make_recordStatus = function (aParam) {
            var message = hx.al.component('response', {
                '_visible': true,
                '_msg_type': 'recordStatus',
                'recIcon': aParam.withIcon,
                'text': aParam.text,
                'text_detail': aParam.text_detail //string
            });
            return message;
        };
        cCPgMessage.prototype.__make_recordDuration = function (aParam) {
            var message = hx.al.component('recordDuration', {
                '_visible': true,
                '_msg_type': 'recordDuration',
                'title': aParam.title,
                'text': aParam.text,
                'p_text': aParam.p_text,
                'f_arrow': aParam.first_arrow,
                'f_text': aParam.first_text,
                'f_step': aParam.first_step,
                's_show': aParam.second_show,
                's_arrow': aParam.second_arrow,
                's_text': aParam.second_text,
                's_time': aParam.second_time,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_cancelRecording = function (aParam) {
            var message = hx.al.component('cancelRecording', {
                '_visible': true,
                '_msg_type': 'cancelRecording',
                'title': aParam.title,
                'text': aParam.text,
                'list_arr': aParam.list_arr,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_recordingConflict = function (aParam) {
            var message = hx.al.component('cancelRecording', {
                '_visible': true,
                '_msg_type': 'recordingConflict',
                'title': aParam.title,
                'text': aParam.text,
                'text2': aParam.text2,
                'list_arr': aParam.list_arr,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_recordingSubConflict = function (aParam) {
            var message = hx.al.component('recordingSubConflict', {
                '_visible': true,
                'title': aParam.title,
                'text': aParam.text,
                'list_arr': aParam.list_arr,
                'btn_title_arr': aParam.btn_title_arr,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_selectMmi = function (aParam) {
            var message = hx.al.component('cimmi', {
                '_visible': true,
                'title': aParam.title,
                'type': aParam.type,
                'uidata': aParam.uidata,
                'callback_fn': aParam.callback_fn,
                'fxInput': aParam.fxInput
            });
            return message;
        };
        cCPgMessage.prototype.__make_chUpdate = function (aParam) {
            var message = hx.al.component('chupdate', {
                '_visible': true,
                'btn_title_arr': aParam.btn_title_arr,
                'msg_type': aParam.msg_type,
                'chInfos': aParam.chInfos,
                'title_string': aParam.title_string,
                'text_string': aParam.text_string,
                'descript_string': aParam.descript_string,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_selectChlist = function (aParam) {
            var message = hx.al.component('selectchlist', {
                '_visible': true,
                'title_string': aParam.title_string,
                'text_string': aParam.text_string,
                'listInfos': aParam.listInfos,
                'callback_fn': aParam.callback_fn
            });
            return message;
        };
        cCPgMessage.prototype.__make_dolby = function (aParam) {
            return hx.al.component('Notify', {
                html: '<p id="dolby" class="ico"></p>',
                position: {
                    top: 40,
                    right: 60
                },
                'auto_close': aParam.auto_close,
                'timeout': aParam.timeout
            });
        };
        cCPgMessage.prototype.__make_dolbyPlus = function (aParam) {
            return hx.al.component('Notify', {
                html: '<p id="dolbyPlus" class="ico"></p>',
                position: {
                    top: 40,
                    right: 60
                },
                'auto_close': aParam.auto_close,
                'timeout': aParam.timeout
            });
        };
        cCPgMessage.prototype.__make_notAvable = function (aParam) {
            return hx.al.component('Notify', {
                html: '<p id="notAvable" class="ico"></p>',
                position: {
                    top: 40,
                    right: 60
                },
                'auto_close': aParam.auto_close,
                'timeout': aParam.timeout
            });
        };
        cCPgMessage.prototype.__make_toggleAudio = function (aParam) {
            var message = hx.al.component('responseAVC', {
                '_msg_type': 'audio',
                'text': aParam.text,
                'encoding': aParam.encoding,
                'streoL': aParam.streoL,
                'streoR': aParam.streoR
            });
            return message;
        };
        cCPgMessage.prototype.__make_toggleSubtitle = function (aParam) {
            var message = hx.al.component('responseAVC', {
                '_msg_type': 'subtitle',
                'text': aParam.text,
                'subType': aParam.subType,
                'hardofHearing': aParam.hardofHearing //string
            });
            return message;
        };
        cCPgMessage.prototype.__make_toggleAudiodescription = function (aParam) {
            var message = hx.al.component('responseAVC', {
                '_msg_type': 'audiodescription',
                'text': aParam.text,
                'AudioDescription': aParam.AudioDescription
            });
            return message;
        };
        cCPgMessage.prototype.__make_toggleSubtitleonoff = function (aParam) {
            var message = hx.al.component('responseAVC', {
                '_msg_type': 'subtitle',
                'text': aParam.text,
                'subType': aParam.subType,
                'hardofHearing': aParam.hardofHearing //string
            });
            return message;
        };
        cCPgMessage.prototype.__make_forcedMailPopup = function (aParam) {
            var message = hx.al.component('cCasIrMailDisPopup', {
                'msgGroup': aParam.msgGroup,
                'date': aParam.date,
                'time': aParam.time,
                'text': aParam.text
            });
            return message;
        };
        cCPgMessage.prototype.__make_naMailPopup = function (aParam) {
            var message = hx.al.component('cCasNaMailDisPopup', {
                'date': aParam.date,
                'time': aParam.time,
                'text': aParam.text
            });
            return message;
        };
        cCPgMessage.prototype.__make_attrDispMsg = function (aParam) {
            var message = hx.al.component('cCasIrAttrDispMsg', {
                'msgType': aParam.msgType,
                'flashing': aParam.flashing,
                'banner': aParam.banner,
                'coverage_percent': aParam.coverage_percent,
                'date': aParam.date,
                'time': aParam.time,
                'text': aParam.text
            });
            return message;
        };
        /**message list end*******************************************************/
        /**message event start*******************************************************/
        cCPgMessage.prototype.__clicked_confirm = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title);
            }
            this._removeDialog('confirm', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_select = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title);
            }
            this._removeDialog('select', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_progressBarBtn = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title);
            }
            this._removeDialog('progressBarBtn', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_progressBtn = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title);
            }
            this._removeDialog('progressBtn', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_codeConfirm = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title);
            }
            this._removeDialog('codeConfirm', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_greenButton = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            var programme = aParam.alData.programme;
            if (callback_fn) {
                callback_fn(input_title, programme);
            }
            //this._removeDialog('greenButton', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_watchinHDButton = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            var linkage = aParam.alData.linkage;
            if (callback_fn) {
                callback_fn(input_title, linkage);
            }
            //this._removeDialog('watchinHDButton', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_ssuDialog = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title);
            }
            this._removeDialog('ssuDialog', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_remindDialog = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title);
            }
            this._removeDialog('remindDialog', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_checkDialog = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            var bChecked = aParam.alData.bChecked;
            if (callback_fn) {
                callback_fn(input_title, {
                    'bChecked': bChecked
                });
            }
            if (input_title !== 'checkBox') {
                this._removeDialog('checkDialog', aParam.alData.me);
            }
        };
        cCPgMessage.prototype.__clicked_selectMmi = function (aParam) {
            this._removeDialog('selectMmi', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_chUpdate = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title);
            }
            this._removeDialog('chUpdate', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_selectChlist = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var index = aParam.alData.index;
            if (callback_fn) {
                callback_fn(input_title, index);
            }
            this._removeDialog('selectChlist', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_inputtext = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input = aParam.alData.input;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title, {
                    'input': input
                });
            }
            this._removeDialog('inputtext', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_passwd = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var password = aParam.alData.password;
            var wifi = aParam.alData.wifi;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title, {
                    'password': password,
                    'wifi': wifi
                });
            }
            this._removeDialog('passwd', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_lipsync = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title, aParam.alData.lipsyncDelay);
            }
            if (input_title !== 'left' && input_title !== 'right') {
                this._removeDialog('lipsync', aParam.alData.me);
            }
        };
        cCPgMessage.prototype.__clicked_subtitleSync = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            if (callback_fn) {
                callback_fn(input_title, aParam.alData.syncDelay);
            }
            if (input_title !== 'left' && input_title !== 'right') {
                this._removeDialog('subtitleSync', aParam.alData.me);
            }
        };
        /**message event as-is*******************************************************/
        cCPgMessage.prototype.__clicked_pin = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var input = aParam.alData.input;
            if (callback_fn) {
                callback_fn(input_title, input);
            }
        };
        cCPgMessage.prototype.__clicked_pinPC = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var input = aParam.alData.input;
            if (callback_fn) {
                callback_fn(input_title, input);
            }
        };
        cCPgMessage.prototype.__clicked_twopin = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var input = aParam.alData.input;
            if (callback_fn) {
                callback_fn(input_title, input);
            }
        };
        cCPgMessage.prototype.__clicked_inputtime = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var input = aParam.alData.input;
            if (callback_fn) {
                callback_fn(input_title, input);
            }
        };
        cCPgMessage.prototype.__clicked_inputonetime = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var input = aParam.alData.input;
            if (callback_fn) {
                callback_fn(input_title, input);
            }
        };
        /**message event recording policy*******************************************************/
        cCPgMessage.prototype.__clicked_cancelRecording = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var input = aParam.alData.input;
            if (callback_fn) {
                callback_fn(input_title, input);
            }
            this._removeDialog('cancelRecording', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_recordDuration = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var input = aParam.alData.input;
            var _close = aParam.alData.close;
            if (callback_fn) {
                callback_fn(input_title, input);
            }
            if (_close) {
                this._removeDialog('recordDuration', aParam.alData.me);
            }
        };
        cCPgMessage.prototype.__clicked_recordingConflict = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var input = aParam.alData.input;
            if (callback_fn) {
                callback_fn(input_title, input);
            }
            this._removeDialog('recordingConflict', aParam.alData.me);
        };
        cCPgMessage.prototype.__clicked_recordingSubConflict = function (aParam) {
            var callback_fn = aParam.alData.callback_fn;
            var input_title = aParam.alData.input_title;
            var input = aParam.alData.input;
            if (callback_fn) {
                callback_fn(input_title, input);
            }
            this._removeDialog('recordingSubConflict', aParam.alData.me);
        };
        /**message event end*******************************************************/
        cCPgMessage.prototype.visible = function () {
            var ret = _super.prototype.visible.call(this);
            if (!ret) {
                var pgMain = hx.al.entity.getRepository().get("CPgMain");
                if (pgMain) {
                    ret = __PAGE_KERNEL__.prototype.visible.call(pgMain);
                }
                if (!ret) {
                    var pgLiveController = hx.al.entity.getRepository().get("CPgLiveController");
                    if (pgLiveController) {
                        ret = __PAGE_KERNEL__.prototype.visible.call(pgLiveController);
                    }
                }
            }
            return ret;
        };
        return cCPgMessage;
    })(__PAGE_KERNEL__);
    return cCPgMessage;
});
