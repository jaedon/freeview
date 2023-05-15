///<reference path="../../def/sugar.d.ts" />
// file name : pages/msgbox/messagebox.js
/*jslint sloppy:true, nomen:true, vars:true */
declare var hx: ifSugar;
import __PAGE_KERNEL__ = require("pages/page_kernel");
import __NETFLIX__ = require("layer_domain/service/dl_dsrvNetflix");

class cCPgMessage extends __PAGE_KERNEL__ {
    $wrap : any;
    _article : any;
    _dialogList : any;

    constructor(aServiceName: string) {
        super(aServiceName);
    }

    create () {
        var that = this;
        hx.logadd('CPgMessage');

        $.extend(that, {
            _article : hx.al.component('frame', {
                _focusException: true,
                'tag' : 'article',
                'attr' : {
                    'id' : 'DIA'
                }
            }),
            $wrap : $('#wrap')
        });

        //that.$wrap = $('#wrap');
        that.$wrap.append(that._article.$);
        hx.al.compose(that).append(that._article);

        that._article.on();
        that._dialogList = [];

        //define
        var pgMessage = that;
        var exceptionDlg : number = 0;
        hx.msg = function(name, param) {
            //pgMessage._removeDialog(0);
            if (hx.config.netflix) {
                if(hx.svc.NETFLIX.isRunning()) {
                    if (param.validOnNetflix === undefined) {
                        hx.log("info","[hx.msg] .... return due to run Netflix !");
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

        hx.msg.close = function(handle, dialog) {
            pgMessage._removeDialog(handle, dialog, 'close');
        };
        hx.msg.addExceptionalDlg = function() {
            exceptionDlg += 1;
        };
        hx.msg.removeExceptionalDlg = function() {
            if (exceptionDlg > 0) {
                exceptionDlg -= 1;
            }
        };
        hx.msg.initExceptionalDlg = function() {
            exceptionDlg = 0;
        };
        hx.msg.progress = function(handler, total, index, dialog) {
            pgMessage._changeProgressBar(handler, total, index, dialog);
        };
        hx.msg.isOpend = function() {
           var ret = that._visible; // is opend user interaction dilalog??

            if (!ret) {
                if (exceptionDlg) {
                    ret = true;
                }
            }
            return ret;
        };
        hx.msg.isOpendWithName = function(name) {
            var i, _list = that._dialogList;
            var visibleCnt = 0;
            for ( i = 0; i < _list.length; i += 1) {
                if(_list[i].name === name) {
                    visibleCnt += 1;
                }
            }
            return (visibleCnt) ? true : false;
        };
    }
    destroy () {
        this._article.off();
    }
    _updateVisible () {
        var i : number, _list : any = this._dialogList;
        var visibleCnt : number = 0;
        for ( i = 0; i < _list.length; i += 1) {
            var o = _list[i].cmpt;
            if (o) {
                if(o._visible === true) {
                    visibleCnt += 1;
                }
            }
        }

        var oldVisible = this._visible;
        if(visibleCnt > 0) {
            this._visible = true;
        } else {
            this._visible = false;
        }
        if(oldVisible !== this._visible) {
            this.sendEventToEntity({
                alEvent : 'alCheckPageVisible'
            });
        }
        hx.log('CPgMessage', 'messagebox _updateVisible = ' + visibleCnt);
    }
    // EVENT HANDLER
    _on_alKey (aParam) {
        var that = this;
        if (!that._dialogList || !that._dialogList.length) {
            return false;
        }

        var i : number, tmp : any, index : number;
        var bConsumed = false;
        var kc : number = aParam.alKey;
        //exit/back/standby key
        switch (kc) {
            case hx.key.KEY_POWER:
                if (!aParam.alData) {
                    hx.log('error', '[messagebox][alKey]no param.alData');
                    hx.log('error', '[messagebox][alKey]no param.alData');
                } else {
                    tmp = 'key_power';
                    for ( i = 0; i < that._dialogList.length; i += 1) {
                        o = that._dialogList[i];
                        if(o.cmpt === aParam.alData.me) {
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
                } else {
                    tmp = 'key_back';
                    for ( i = 0; i < that._dialogList.length; i += 1) {
                        var o = that._dialogList[i];
                        if (o.cmpt === aParam.alData.me) {
        					if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) {
                                /* A fingerprint doesn't close by the exit key. */
                                if (o.name === 'attrDispMsg' && (typeof o.cmpt.isFp == 'function') && (o.cmpt.isFp() == true))
                                    continue;
                            }
                            if(hx.config.useNetflixPW && hx.svc.NETFLIX.getPwStatus()){
                                that.sendEventToEntity({
                                    alEvent: 'alMessageToPage',
                                    target: 'CPgMain/CloseNetflixPW',
                                    me : that
                                });
                                that.sendEventToEntity({
                                    alEvent : 'alMessageToPage',
                                    target : 'CPgLiveController?showpin=pin'
                                });
                                bConsumed = true;
                            } else if (o.name !== 'pinPC') {
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
                } else {
                    tmp = 'key_esc';
                    for ( i = 0; i < that._dialogList.length; i += 1) {
                        var o = that._dialogList[i];
                        if (o.cmpt === aParam.alData.me) {
        					if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) {
                                /* A fingerprint doesn't close by the exit key. */
                                if (o.name === 'attrDispMsg' && (typeof o.cmpt.isFp == 'function') && (o.cmpt.isFp() == true))
                                    continue;
                            }
                            if(hx.config.useNetflixPW && hx.svc.NETFLIX.getPwStatus()){
                                that.sendEventToEntity({
                                    alEvent: 'alMessageToPage',
                                    target: 'CPgMain/CloseNetflixPW',
                                    me : that
                                });
                                that.sendEventToEntity({
                                    alEvent : 'alMessageToPage',
                                    target : 'CPgLiveController?showpin=pin'
                                });
                                bConsumed = true;
                            }else {
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
                } else {
                    tmp = 'key_ok';
                    for ( i = 0; i < that._dialogList.length; i += 1) {
                        var o = that._dialogList[i];
                        if (o.cmpt === aParam.alData.me) {
                            if (o.name === 'forcedMailPopup') {
                                that._removeDialog(o.name, o.cmpt, tmp);
                                bConsumed = true;
                            }
                            if (o.name === 'attrDispMsg') {
                                // if it is fingerprint, skip the ok message
                                if((typeof o.cmpt.isFp == 'function') && (o.cmpt.isFp() == true))
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
                } else {
                    tmp = kc;
                    for ( i = 0; i < that._dialogList.length; i += 1) {
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
    }
    _on_alClicked (aParam) : boolean {
        var i : number, ti : number;
        for ( i = 0, ti = this._dialogList.length; i < ti; i += 1) {
            var dialog = this._dialogList[i];
            if (dialog) {
                if (dialog.cmpt === aParam.alData.me) {
                    this['__clicked_' + dialog.name](aParam);
                }
            }
        }
        return true;
    }
    _on_alMessageToPage (aParam) {
        this._set_time_out(aParam);
    }
    // PRIVATE FUNCTION FOR CONTENT MAKING
    _hasDialog () {
        return ($("#wrap").children().length > 0);
    }
    _appendDialog (aNname, aParam) {
        var that = this;
        var dialog = that['__make_' + aNname](aParam);

        that.append(dialog);

        if (!!aParam.attr) {
            that._article.$.addClass(aParam.attr);
        }

        switch(aNname) {
            case 'status':
            case 'dolby':
            case 'dolbyPlus':
            case 'notAvable':
                that.$wrap.append(dialog.$);
                that._dialogList.push({
                    'name' : aNname,
                    'cmpt' : dialog,
                    //'timer' : timer,
                    'param' : aParam
                });
                break;
            default:
                that._article.$.append(dialog.$);
                that._dialogList.push({
                    'name' : aNname,
                    'cmpt' : dialog,
                    //'timer' : timer,
                    'param' : aParam
                });
                dialog.on();
                break;
        }

        this._modifyCSS(dialog, aNname);

        // if there is doDraw function, then do it!
        if(typeof dialog.doDraw == 'function') {
                //after appending the dialog to DOM, process the doDraw function, since some properties(eg, height) are available after the dialog on DOM.
                dialog.doDraw();
        }

        //return 0;
        if ((aParam.auto_close === undefined ? aParam.auto_close = true : aParam.auto_close)) {
            if (!aParam.timeout) {
                aParam.timeout = 3000;
            }
            //clearTimeout(dialog._timerId);
            dialog._timerId = setTimeout(function() {
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
            for( i=0; i<$target.length; i+=1) {
                if($target.eq(i).hasClass('aniActive')) {
                    continue;
                } else {
                    $selectedTarget = $target.eq(i);
                    break;
                }
            }
            if ($selectedTarget) {
                hx.il.ani.action({
                    type:'in',
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
            case 'checkDialog' :
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

		if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) { // Move fingerprint on the top of OSD
            that._moveFingerprintOnTopOsd();
        }
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        return dialog;
    }
    _set_time_out (aParam) {
        var i : number, ti : number;
        for ( i = 0, ti = this._dialogList.length; i < ti; i += 1) {
            var dialog = this._dialogList[i];
            if (dialog.cmpt === aParam.alData.me) {
                if ((dialog.param.auto_close === undefined ? dialog.param.auto_close = true : dialog.param.auto_close)) {
                    clearTimeout(dialog.cmpt._timerId);
                    dialog.cmpt._timerId = setTimeout(()=> {
                        this._removeDialog(dialog.name, dialog.cmpt, 'auto_close');
                    }, dialog.param.timeout);
                }
                break;
            }
        }
        return true;
    }
    //_timerId : 0,
    _modifyCSS (aDialog, aName) {
        var _h : number, _t : any, tmp : number;
        if (aName === 'status') {//change css top
            _h = parseInt(aDialog.$.css('height'));
            _t = '';
            tmp = _h / 34;
            _t = 310 - (17 * (tmp - 1));
            aDialog.$.css('top', (_t + 'px'));
        } else if (aName === 'pin' || aName === 'pinPC') {
            _h = parseInt(aDialog.$.css('height'));
            _t = 0;

            if (_h <= 121) {
                _t = 270;
            } else if (_h <= 162) {
                _t = 253;
            } else if (_h <= 196) {
                _t = 236;
            } else {
                _t = 219;
            }

            aDialog.$.css('top', (_t + 'px'));
        } else if (aName === 'progress') {
            _h = parseInt(aDialog.$.css('height'));
            if (_h > 34) {
                aDialog.$.attr('class', 'pro vaTop');
            }
        } else if (aName === 'progressBar') {
            _h = parseInt(aDialog.$.css('height'));
            _t = '';
            tmp = (_h - 18) / 34;
            _t = 310 - (17 * (tmp - 1));
            aDialog.$.css('top', (_t + 'px'));
            if (_t > 310) {
                aDialog.$.attr('class', 'pro');
            }
        } else if (aName === 'progressBtn') {
            _h = parseInt(aDialog.$.css('height'));
            if (_h <= 123) {
                aDialog.$.attr('class', 'pro vaBot line1');
            } else {
                aDialog.$.attr('class', 'pro vaBot');
            }
        }
    }
    _removeDialog (aHandle, aDialog, _msg?) {
        var that = this;
        var i : number, _list : any = that._dialogList;
        for ( i = 0; i < _list.length; i += 1) {
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
                for ( i = _list.length - 1; i >= 0; i -= 1) {
                    if (bFocused) {
                        break;
                    }
                    if (_list[i]) {
                        switch(_list[i].name) {
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
        } else {
            that._article.$.attr('class', '');
        }
       that._updateVisible();
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
    }
    _changeProgressBar (aHandler, aTotal, aIndex, aDialog) {
        var i : number, ti : number;
        for ( i = 0, ti = this._dialogList.length; i < ti; i += 1) {
            var o : any = this._dialogList[i];
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
    }
    _moveFingerprintOnTopOsd () {
        var i : number = 0;
        var bFpFind : boolean = false;
        /* 1. Find Fingerprint. */
        for ( i = 0; i < this._dialogList.length; i += 1) {
            var o = this._dialogList[i];
            if (o.name === 'attrDispMsg' && (typeof o.cmpt.isFp == 'function') && (o.cmpt.isFp() == true)) {
                bFpFind = true;
                break;
            }
        }
        /* 2. If fingerrpint exist and it is not the last one, then move it at the end.(Move it on the top of OSD) */
        if( bFpFind && i != (this._dialogList.length -1)){ // it could make a problem, but that shall disappear when all dialog are removed.
            var o = this._dialogList[i];
            // move fingerprint ui on the top
            this._article.$.detach(o.cmpt.$);
            this._article.$.append(o.cmpt.$);
            // move fingerprint object to the end of the list.
            this._dialogList.splice(i, 1);
            this._dialogList.push({
                'name' : o.name,
                'cmpt' : o.cmpt,
                //'timer' : timer,
                'param' : o.param
            });
        }
    }
    /**message list start*******************************************************/
    __make_response (aParam) {
        var message : any = hx.al.component('response', {
            '_msg_type' : 'response',
            'text' : aParam.text, //string
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_confirm (aParam) {
        var message : any = hx.al.component('confirm', {
            '_visible' : true,
            '_msg_type' : 'confirm',
            'text' : aParam.text,
            'html' : aParam.html,
            'first_line' : aParam.first_line,
            'useOtherKey' : aParam.useOtherKey,
            'isCenter' : aParam.isCenter,
            'doNotUseBackExitKey' : aParam.doNotUseBackExitKey,
            'btn_title_arr' : aParam.btn_title_arr, //string array
            'btn_last_focus' : aParam.btn_last_focus,
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_codeConfirm (aParam) {
        var message = hx.al.component('confirm', {
            '_visible' : true,
            '_msg_type' : 'codeConfirm',
            'text' : aParam.text,
            'html' : aParam.html,
            'showRegi' : aParam.showRegi,
            'thirdLineFont' : aParam.thirdLineFont,
            'first_line' : aParam.first_line,
            'regcode' : aParam.regcode,
            'nolinejump' : aParam.nolinejump,
            'second_line' : aParam.second_line,
            'btn_title_arr' : aParam.btn_title_arr, //string array
            'callback_fn' : aParam.callback_fn,
        });
        return message;
    }
    __make_greenButton (aParam) {
        var message = hx.al.component('greenButton', {
            '_visible' : true,
            'programmes' : aParam.programmes,
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_watchinHDButton (aParam) {
        var message = hx.al.component('watchinHDButton', {
            '_visible' : true,
            'linkage' : aParam.linkage,
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_select (aParam) {
        var message : any = hx.al.component('confirm', {
            '_visible' : true,
            '_msg_type' : 'select',
            'text' : aParam.text,
            'btn_title_arr' : aParam.btn_title_arr, //string array
            'callback_fn' : aParam.callback_fn
            //'auto_close' : param.auto_close, //true(default), false
            //'timeout' : param.timeout,            //number (default:3)
            //'dia_class' : param.dia_class     //string('dim')
        });
        return message;
    }
    __make_progress (aParam) {
        var message = hx.al.component('response', {
            '_msg_type' : 'progress',
            '_use_backkey' : aParam._use_backkey,
            'text' : aParam.text, //string
            'enable_key' : aParam.enable_key
            //'auto_close' : param.auto_close, //true(default), false
            //'timeout' : param.timeout,        //number (default:3000)
            //'dia_class' : param.dia_class     //string('dim')
        });
        return message;
    }
    __make_progressBar (aParam) {
        var message : any = hx.al.component('progressBar', {
            '_msg_type' : 'progressBar',
            'text' : aParam.text, //string
            //'auto_close' : param.auto_close, //true(default), false
            //'timeout' : param.timeout, //number (default:3)
            'callback_fn' : aParam.callback_fn
            //'dia_class' : param.dia_class     //string('dim')
        });
        return message;
    }
    __make_progressBarBtn (aParam) {
        var message : any = hx.al.component('progressBar', {
            '_visible' : true,
            '_msg_type' : 'progressBarBtn',
            'text' : aParam.text, //string
            'btn_title' : aParam.btn_title,
            //'auto_close' : param.auto_close, //true(default), false
            //'timeout' : param.timeout, //number (default:3)
            'callback_fn' : aParam.callback_fn
            //'dia_class' : param.dia_class     //string('dim')
        });
        return message;
    }
    __make_progressBtn (aParam) {
        var message : any = hx.al.component('progressBar', {
            '_visible' : true,
            '_msg_type' : 'progressBtn',
            'text' : aParam.text, //string
            'btn_title' : aParam.btn_title,
            //'auto_close' : param.auto_close, //true(default), false
            //'timeout' : param.timeout, //number (default:3)
            'callback_fn' : aParam.callback_fn
            //'dia_class' : param.dia_class     //string('dim')
        });
        return message;
    }
    __make_ssuDialog (aParam) {
        var message : any = hx.al.component('confirm', {
            '_visible' : true,
            '_msg_type' : 'ssuDialog',
            'title' : aParam.title,
            'text' : aParam.text,
            'text1' : aParam.text1,
            'html' : aParam.html,
            'first_line_arr' : aParam.first_line_arr,
            'second_line_arr' : aParam.second_line_arr,
            'third_line_arr' : aParam.third_line_arr,
            'btn_title_arr' : aParam.btn_title_arr, //string array
            //'auto_close' : param.auto_close, //true(default), false
            //'timeout' : param.timeout, //number (default:3)
            'doNotUseBackExitKey' : aParam.doNotUseBackExitKey,
            'callback_fn' : aParam.callback_fn
            //'dia_class' : param.dia_class     //string('dim')
        });
        return message;
    }
    __make_remindDialog (aParam) {
        var message : any = hx.al.component('confirm', {
            '_visible' : true,
            '_msg_type' : 'remindDialog',
            'text' : aParam.text,
            'first_line' : aParam.first_line,
            'second_line_arr' : aParam.second_line_arr,
            'btn_title_arr' : aParam.btn_title_arr, //string array
            //'auto_close' : param.auto_close, //true(default), false
            //'timeout' : param.timeout, //number (default:3)
            'callback_fn' : aParam.callback_fn
            //'dia_class' : param.dia_class     //string('dim')
        });
        return message;
    }
    __make_checkDialog (aParam) {
        var message : any = hx.al.component('confirm', {
            '_visible' : true,
            '_msg_type' : 'checkDialog',
            'title' : aParam.title,
            'text' : aParam.text,
            'html' : aParam.html,
            'checkText' : aParam.checkText,
            'btn_title_arr' : aParam.btn_title_arr, //string array
            'timeout' : aParam.timeout, //number (default:3)
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_status (aParam) {
        var message : any = hx.al.component('cMessage', {
            'text' : aParam.text,
            'type' : 'status',
            _focusException: true

        });
        return message;
    }
    __make_pin (aParam) {
        var pin : any = hx.al.component('pin', {
            '_visible' : true,
            'type' : 'pin',
            'tag' : 'div',
            'attr' : {
                'class' : 'pas'
            },
            'text' : aParam.text,
            'html' : aParam.html,
            'first_line' : aParam.first_line,
            'second_line' : aParam.second_line,
            'enable' : aParam.enable,
            'callback_fn' : aParam.callback_fn
        });
        return pin;
    }
    __make_pinPC (aParam) {
        var pin : any = hx.al.component('pinPC', {
            //'_visible' : true,
            'type' : 'pin',
            'tag' : 'div',
            'attr' : {
                'class' : 'pas'
            },
            'text' : aParam.text,
            'enable' : aParam.enable,
            'callback_fn' : aParam.callback_fn,
            'auto_close' : aParam.auto_close,
            'timeout' : aParam.timeout
        });
        return pin;
    }
    __make_twopin (aParam) {
        var pin : any = hx.al.component('twopin', {
            '_visible' : true,
            'type' : 'pin',
            'tag' : 'div',
            'attr' : {
                'class' : 'pas',
                'style' : 'top: 253px'
            },
            'text' : aParam.text,
            'enable' : aParam.enable,
            'callback_fn' : aParam.callback_fn
        });
        return pin;
    }
    __make_inputtime (aParam) {
        var inputtime : any = hx.al.component('inputtime', {
            '_visible' : true,
            'type' : 'pin',
            'tag' : 'div',
            'attr' : {
                'class' : 'pas multi',
                'style' : 'top: 253px'
            },
            'text' : aParam.text,
            'enable' : aParam.enable,
            'callback_fn' : aParam.callback_fn
        });
        return inputtime;
    }
    __make_inputonetime (aParam) {
        var inputonetime : any = hx.al.component('inputonetime', {
            '_visible' : true,
            'type' : 'pin',
            'tag' : 'div',
            'attr' : {
                'class' : 'pas multi',
                'style' : 'top: 253px'
            },
            'text' : aParam.text,
            'enable' : aParam.enable,
            'callback_fn' : aParam.callback_fn
        });
        return inputonetime;
    }
    __make_inputtext (aParam) {
        var input : any = hx.al.component('inputtext', {
            '_visible' : true,
            'type' : aParam.type,
            'callback_fn' : aParam.callback_fn
        });
        return input;
    }
    __make_passwd (aParam) {
        var pin : any = hx.al.component('passwd', {
            '_visible' : true,
            'wifi' : aParam.wifi,
            'responseText' : aParam.responseText,
            'text' : aParam.text,
            'string' : aParam.string,
            'type' : aParam.type,
            'callback_fn' : aParam.callback_fn,
            'maxLength' : aParam.maxLength ? 35 : 32
        });
        return pin;
    }
    __make_lipsync (aParam) {
        this._article.$.addClass('lipSync');
        var lipsync : any = hx.al.component('lipsync', {
            //'type' : 'pin',
            'tag' : 'dl',
            'attr' : {
                'class' : 'progress type5'
            },
            'auto_close' : aParam.auto_close,
            'lipsyncDelay' : aParam.lipsyncDelay,
            //'text' : param.text,
            //'enable' : param.enable,
            'callback_fn' : aParam.callback_fn
        });
        return lipsync;
    }
    __make_subtitleSync (aParam) {
        this._article.$.addClass('subtitleSync');
        var subtitleSync : any = hx.al.component('subtitleSync', {
            'tag' : 'dl',
            'attr' : {
                'class' : 'progress type5'
            },
            'auto_close' : aParam.auto_close,
            'syncDelay' : aParam.syncDelay,
            'callback_fn' : aParam.callback_fn
        });
        return subtitleSync;
    }
    /**message list recording policy*******************************************************/
    __make_recordStatus (aParam) {
        var message : any = hx.al.component('response', {
            '_visible' : true,
            '_msg_type' : 'recordStatus',
            'recIcon' : aParam.withIcon,
            'text' : aParam.text, //string
            'text_detail' : aParam.text_detail //string
        });
        return message;
    }
    __make_recordDuration (aParam) {
        var message : any = hx.al.component('recordDuration', {
            '_visible' : true,
            '_msg_type' : 'recordDuration',
            'title' : aParam.title,
            'text' : aParam.text,
            'p_text' : aParam.p_text,
            'f_arrow' : aParam.first_arrow,
            'f_text' : aParam.first_text,
            'f_step' : aParam.first_step,
            's_show' : aParam.second_show,
            's_arrow' : aParam.second_arrow,
            's_text' : aParam.second_text,
            's_time' : aParam.second_time,
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_cancelRecording (aParam) {
        var message : any = hx.al.component('cancelRecording', {
            '_visible' : true,
            '_msg_type' : 'cancelRecording',
            'title' : aParam.title,
            'text' : aParam.text,
            'list_arr' : aParam.list_arr,
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_recordingConflict (aParam) {
        var message : any = hx.al.component('cancelRecording', {
            '_visible' : true,
            '_msg_type' : 'recordingConflict',
            'title' : aParam.title,
            'text' : aParam.text,
            'text2' : aParam.text2,
            'list_arr' : aParam.list_arr,
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_recordingSubConflict (aParam) {
        var message : any = hx.al.component('recordingSubConflict', {
            '_visible' : true,
            'title' : aParam.title,
            'text' : aParam.text,
            'list_arr' : aParam.list_arr,
            'btn_title_arr' : aParam.btn_title_arr, //string array
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_selectMmi (aParam) {
        var message : any = hx.al.component('cimmi', {
            '_visible' : true,
            'title' : aParam.title,
            'type' : aParam.type,
            'uidata' : aParam.uidata,
            'callback_fn' : aParam.callback_fn,
            'fxInput' : aParam.fxInput
        });
        return message;
    }
    __make_chUpdate (aParam) {
        var message : any = hx.al.component('chupdate', {
            '_visible' : true,
            'btn_title_arr' : aParam.btn_title_arr,
            'msg_type' : aParam.msg_type,     //new or replaced
            'chInfos' : aParam.chInfos,
            'title_string' : aParam.title_string,
            'text_string' : aParam.text_string,
            'descript_string' : aParam.descript_string,
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_selectChlist (aParam) {
        var message = hx.al.component('selectchlist', {
            '_visible' : true,
            'title_string' : aParam.title_string,
            'text_string' : aParam.text_string,
            'listInfos' : aParam.listInfos,
            'callback_fn' : aParam.callback_fn
        });
        return message;
    }
    __make_dolby (aParam) {
        return hx.al.component('Notify', {
            html : '<p id="dolby" class="ico"></p>',
            position : {
                top : 40,
                right : 60
            },
            'auto_close' : aParam.auto_close,
            'timeout' : aParam.timeout
        });
    }
    __make_dolbyPlus (aParam) {
        return hx.al.component('Notify', {
            html : '<p id="dolbyPlus" class="ico"></p>',
            position : {
                top : 40,
                right : 60
            },
            'auto_close' : aParam.auto_close,
            'timeout' : aParam.timeout
        });
    }
    __make_notAvable (aParam) {
        return hx.al.component('Notify', {
            html : '<p id="notAvable" class="ico"></p>',
            position : {
                top : 40,
                right : 60
            },
            'auto_close' : aParam.auto_close,
            'timeout' : aParam.timeout
        });
    }
    __make_toggleAudio (aParam) {
        var message : any = hx.al.component('responseAVC', {
            '_msg_type' : 'audio',
            'text' : aParam.text, //string
            'encoding' : aParam.encoding,
            'streoL' : aParam.streoL,
            'streoR' : aParam.streoR
        });
        return message;
    }
    __make_toggleSubtitle (aParam) {
        var message : any = hx.al.component('responseAVC', {
            '_msg_type' : 'subtitle',
            'text' : aParam.text, //string
            'subType' : aParam.subType, //string
            'hardofHearing' : aParam.hardofHearing //string
        });
        return message;
    }
    __make_toggleAudiodescription (aParam) {
        var message : any = hx.al.component('responseAVC', {
            '_msg_type' : 'audiodescription',
            'text' : aParam.text, //string
            'AudioDescription' : aParam.AudioDescription
        });
        return message;
    }
    __make_toggleSubtitleonoff (aParam) {
        var message : any = hx.al.component('responseAVC', {
            '_msg_type' : 'subtitle',
            'text' : aParam.text, //string
            'subType' : aParam.subType, //string
            'hardofHearing' : aParam.hardofHearing //string
        });
        return message;
    }
    __make_forcedMailPopup (aParam) {
        var message : any = hx.al.component('cCasIrMailDisPopup', {
             'msgGroup': aParam.msgGroup,
             'date' : aParam.date,
             'time' : aParam.time,
             'text' : aParam.text
        });
        return message;
    }
    __make_naMailPopup (aParam) {
        var message : any = hx.al.component('cCasNaMailDisPopup', {
             'date' : aParam.date,
             'time' : aParam.time,
             'text' : aParam.text
        });
        return message;
    }
    __make_attrDispMsg (aParam) {
        var message : any = hx.al.component('cCasIrAttrDispMsg', {
             'msgType' : aParam.msgType,
             'flashing': aParam.flashing,
             'banner'  : aParam.banner,
             'coverage_percent': aParam.coverage_percent,
             'date' : aParam.date,
             'time' : aParam.time,
             'text' : aParam.text
        });
        return message;
    }
    /**message list end*******************************************************/

    /**message event start*******************************************************/
    __clicked_confirm (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title);
        }
        this._removeDialog('confirm', aParam.alData.me);
    }
    __clicked_select (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title);
        }
        this._removeDialog('select', aParam.alData.me);
    }
    __clicked_progressBarBtn (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title);
        }
        this._removeDialog('progressBarBtn', aParam.alData.me);
    }
    __clicked_progressBtn (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title);
        }
        this._removeDialog('progressBtn', aParam.alData.me);
    }
    __clicked_codeConfirm (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title);
        }
        this._removeDialog('codeConfirm', aParam.alData.me);
    }
    __clicked_greenButton (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        var programme = aParam.alData.programme;
        if (callback_fn) {
            callback_fn(input_title, programme);
        }
        //this._removeDialog('greenButton', aParam.alData.me);
    }
    __clicked_watchinHDButton (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        var linkage = aParam.alData.linkage;
        if (callback_fn) {
            callback_fn(input_title, linkage);
        }
        //this._removeDialog('watchinHDButton', aParam.alData.me);
    }
    __clicked_ssuDialog (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title);
        }
        this._removeDialog('ssuDialog', aParam.alData.me);
    }
    __clicked_remindDialog (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title);
        }
        this._removeDialog('remindDialog', aParam.alData.me);
    }
    __clicked_checkDialog (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        var bChecked = aParam.alData.bChecked;
        if (callback_fn) {
            callback_fn(input_title, {
                'bChecked' : bChecked
            });
        }
        if (input_title !== 'checkBox') {
            this._removeDialog('checkDialog', aParam.alData.me);
        }
    }
    __clicked_selectMmi (aParam) {
        this._removeDialog('selectMmi', aParam.alData.me);
    }
    __clicked_chUpdate (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title);
        }
        this._removeDialog('chUpdate', aParam.alData.me);
    }
    __clicked_selectChlist (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        var index = aParam.alData.index;
        if (callback_fn) {
            callback_fn(input_title, index);
        }
        this._removeDialog('selectChlist', aParam.alData.me);
    }
    __clicked_inputtext (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input = aParam.alData.input;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title, {
                'input' : input
            });
        }
        this._removeDialog('inputtext', aParam.alData.me);
    }
    __clicked_passwd (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var password = aParam.alData.password;
        var wifi = aParam.alData.wifi;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title, {
                'password' : password,
                'wifi' : wifi
            });
        }
        this._removeDialog('passwd', aParam.alData.me);
    }
    __clicked_lipsync (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title, aParam.alData.lipsyncDelay);
        }
        if (input_title !== 'left' && input_title !== 'right') {
            this._removeDialog('lipsync', aParam.alData.me);
        }
    }
    __clicked_subtitleSync (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        if (callback_fn) {
            callback_fn(input_title, aParam.alData.syncDelay);
        }
        if (input_title !== 'left' && input_title !== 'right') {
            this._removeDialog('subtitleSync', aParam.alData.me);
        }
    }
    /**message event as-is*******************************************************/
    __clicked_pin (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        var input = aParam.alData.input;

        if (callback_fn) {
            callback_fn(input_title, input);
        }
    }
    __clicked_pinPC (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        var input = aParam.alData.input;

        if (callback_fn) {
            callback_fn(input_title, input);
        }
    }
    __clicked_twopin (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        var input = aParam.alData.input;

        if (callback_fn) {
            callback_fn(input_title, input);
        }
    }
    __clicked_inputtime (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        var input = aParam.alData.input;

        if (callback_fn) {
            callback_fn(input_title, input);
        }
    }
    __clicked_inputonetime (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        var input = aParam.alData.input;

        if (callback_fn) {
            callback_fn(input_title, input);
        }
    }
    /**message event recording policy*******************************************************/
    __clicked_cancelRecording (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        var input = aParam.alData.input;
        if (callback_fn) {
            callback_fn(input_title, input);
        }
        this._removeDialog('cancelRecording', aParam.alData.me);
    }
    __clicked_recordDuration (aParam) {
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
    }
    __clicked_recordingConflict (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        var input = aParam.alData.input;
        if (callback_fn) {
            callback_fn(input_title, input);
        }
        this._removeDialog('recordingConflict', aParam.alData.me);
    }
    __clicked_recordingSubConflict (aParam) {
        var callback_fn = aParam.alData.callback_fn;
        var input_title = aParam.alData.input_title;
        var input = aParam.alData.input;
        if (callback_fn) {
            callback_fn(input_title, input);
        }
        this._removeDialog('recordingSubConflict', aParam.alData.me);
    }
    /**message event end*******************************************************/
    visible() {
        var ret = super.visible();
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
    }
}
export = cCPgMessage;