///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cConfirm.js
/* jslint sloppy:true, nomen:true, vars:true */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class confirm extends __COMPONENT_KERNEL__ {
    text1: string;
    _last_focus: number;
    _msg_type: string;
    _$checkBox: any;
    _btn_title_arr: any;
    _callback_fn: any;
    _prevIdx: number;
    _doNotUseBackExitKey: any;
    _useOtherKey: any;
    _table: any;
    _checkText: string;
    _$text1: any;
    _contents: any;
    _title: string;

    constructor() {
        super();
    }
    create(aParam) {
        //hx.logadd('cConfirm');
        hx.log('cConfirm', 'create');
        var index: number;
        $.extend(this, aParam, {
            _contents: $('<p>', {
                'html': aParam.text
            }),
            _btn_title_arr: aParam.btn_title_arr,
            _callback_fn: aParam.callback_fn,
            _msg_type: aParam._msg_type,
            _last_focus: 0,
            _useOtherKey: aParam.useOtherKey,
            _checkText: aParam.checkText,
            _doNotUseBackExitKey: aParam.doNotUseBackExitKey
        });
        if (aParam.btn_last_focus) {
            this._last_focus = aParam.btn_last_focus;
        }
        if (aParam.html) {
            this._contents.html(aParam.html);
        }
        if (this._msg_type === 'codeConfirm') {
            if (aParam.second_line) {
                var secondLine: string = '';
                if (aParam.nolinejump) {
                    secondLine = aParam.second_line;
                } else {
                    secondLine = '<br>' + aParam.second_line;
                }
                this._contents.html(aParam.first_line).append($('<em>', {
                    'class': 'regiCode'
                }).text(aParam.regcode)).append($('<p>').html(secondLine));
            }
            if (aParam.html) {
                this._contents.html(aParam.html);
            }
            if (aParam.showRegi) {
                this._contents.css('font-size', '36px');
                this._contents.find('p').css('margin-top', '15px').css('font-size', '28px');
            }  else if (aParam.thirdLineFont) {
                this._contents.css('font-size', '34px').css('color', '#FFFF00');
                this._contents.find('p').css('margin-top', '15px').css('font-size', '26px').css('color', 'white');
            }
        }
        if (this._msg_type === 'ssuDialog' || this._msg_type === 'remindDialog' || this._msg_type === 'checkDialog') {
            $.extend(this, aParam, {
                _title: $('<h1>', {
                    'text': aParam.title
                }),
                _table: $('<dl>', {
                    'class': 'swInfo box'
                })
            });
        }
        var _cls: string = 'con vaBot';
        if (this._msg_type === 'select') {
            _cls = 'con vaBot btn2';
        } else if (this._msg_type === 'remindDialog') {
            _cls = 'con vaBot tmr line1';
        }
        if (aParam.isCenter) {
            _cls = '';
        }
        this._create({
            'tag': 'div',
            'attr': {
                'class': _cls
            }
        });
        if (aParam.isCenter) {
            this.$[0].style.top = "236px";
        }
        if (this._msg_type === 'ssuDialog' || this._msg_type === 'checkDialog') {
            if (aParam.title) {
                this.$.append(this._title);
            }
        }
        this.$.append(this._contents);
        if (this._msg_type === 'ssuDialog' || this._msg_type === 'remindDialog') {
            var _first: any = aParam.first_line_arr;
            if (this._msg_type === 'remindDialog') {
                _first = aParam.first_line;
            } else {
                this._$text1 = $('<p>', {
                    'text': this.text1
                });
                if (this.text1) {
                    this.$.append(this._$text1);
                }
            }
            var _second: any = aParam.second_line_arr;
            if (_first != null) {
                this._set_ssuDialog_list(_first, this._table);
            }
            if (_second != null) {
                this._set_ssuDialog_list(_second, this._table);
            }
            var _third: any = aParam.third_line_arr;
            if (_third != null) {
                this._set_ssuDialog_list(_third, this._table);
            }
            if (_first != null || _second != null) {
                this.$.append(this._table.append());
            }
        }
        if (this._msg_type === 'checkDialog') { // check box
            this._$checkBox = $('<p>', {
                'class': 'checkBox',
            });
            var html: string = '<img src = "images/112_DIA_Check_Off.png"/> ' + this._checkText;
            this._$checkBox.html(html);
            this.$.append($('<br>'));
            this.$.append(this._$checkBox);
        }
        for (index = 0; index < this._btn_title_arr.length; index += 1) {
            this.$.append(this._set_button(this._btn_title_arr[index]));
        }
        //if ((param.auto_close === undefined ? param.auto_close = true : param.auto_close)) {
        //    this._setClosedTimer(param);
        //}
        this.$.focus(()=> {
            this._doFocus();
        });
        return this;
    }

    setFocus(aIndex: number) {
        this._doFocus(aIndex);
    }

    resetTimeout() {
        this.__send_timer_refresh();
    }

    _set_ssuDialog_list(_arr, _obj) {
        var i: number, tmp: string;
        if (typeof (_arr) === 'string') {
            _arr = [_arr];
        }
        for (i = 0; i < _arr.length; i += 1) {
            if (i % 2 === 0) {
                tmp = '<dt>';
            } else {
                tmp = '<dd>';
            }
            _obj.append($(tmp, {
                'text': _arr[i]
            }));
        }
    }

    destroy() {
        this.$.remove();
    }

    _set_button(txt) {
        return $('<p>', {
            'class': 'button',
            'text': txt
        });
    }

    // EVENT HANDLER
    _on_alKey(aParam: any): boolean {
        var tmp: number, index: number;
        var bConsumed: boolean = false;
        var kc: number = aParam.alKey;
        switch (kc) {
            case hx.key.KEY_UP:
                if ((this._msg_type === 'confirm') || (this._btn_title_arr.length < 2)) {
                    this.__send_timer_refresh(aParam);
                    return true;
                } else if (this._msg_type === 'checkDialog') {
                    if (this._$checkBox.hasClass('on')) {
                        this._$checkBox.removeClass('on');
                        this._doFocus(this._prevIdx);
                    } else {
                        for (index = 0; index < this._btn_title_arr.length; index += 1) {
                            var $btn = this.$.children('.button').eq(index);
                            if ($btn.is('.on')) {
                                this._prevIdx = index;
                                $btn.removeClass('on');
                                break;
                            }
                        }
                        this._$checkBox.addClass('on');
                    }
                    this.__send_timer_refresh(aParam);
                    return true;
                }
                for (index = 0; index < this._btn_title_arr.length; index += 1) {
                    if (this.$.children('.button').eq(index).is('.on')) {
                        tmp = index - 1;
                        if (tmp < 0) {
                            tmp = (this._btn_title_arr.length - 1);
                        }
                        this._doFocus(tmp);
                        break;
                    }
                }
                bConsumed = true;
                break;
            case hx.key.KEY_DOWN:
                if ((this._msg_type === 'confirm') || (this._btn_title_arr.length < 2)) {
                    this.__send_timer_refresh(aParam);
                    return true;
                } else if (this._msg_type === 'checkDialog') {
                    if (this._$checkBox.hasClass('on')) {
                        this._$checkBox.removeClass('on');
                        this._doFocus(this._prevIdx);
                    } else {
                        for (index = 0; index < this._btn_title_arr.length; index += 1) {
                            var $btn = this.$.children('.button').eq(index);
                            if ($btn.is('.on')) {
                                this._prevIdx = index;
                                $btn.removeClass('on');
                                break;
                            }
                        }
                        this._$checkBox.addClass('on');
                    }
                    this.__send_timer_refresh(aParam);
                    return true;
                }
                for (index = 0; index < this._btn_title_arr.length; index += 1) {
                    if (this.$.children('.button').eq(index).is('.on')) {
                        tmp = index + 1;
                        if (tmp >= this._btn_title_arr.length) {
                            tmp = 0;
                        }
                        this._doFocus(tmp);
                        break;
                    }
                }
                bConsumed = true;
                break;
            case hx.key.KEY_LEFT:
                if ((this._msg_type === 'select') || (this._btn_title_arr.length < 2)) {
                    this.__send_timer_refresh(aParam);
                    return true;
                }
                for (index = 0; index < this._btn_title_arr.length; index += 1) {
                    if (this.$.children('.button').eq(index).is('.on')) {
                        tmp = index - 1;
                        if (tmp < 0) {
                            tmp = (this._btn_title_arr.length - 1);
                        }
                        this._doFocus(tmp);
                        break;
                    }
                }
                bConsumed = true;
                break;
            case hx.key.KEY_RIGHT:
                if ((this._msg_type === 'select') || (this._btn_title_arr.length < 2)) {
                    this.__send_timer_refresh(aParam);
                    return true;
                }
                for (index = 0; index < this._btn_title_arr.length; index += 1) {
                    if (this.$.children('.button').eq(index).is('.on')) {
                        tmp = index + 1;
                        if (tmp >= this._btn_title_arr.length) {
                            tmp = 0;
                        }
                        this._doFocus(tmp);
                        break;
                    }
                }
                bConsumed = true;
                break;
            case hx.key.KEY_OK:
                this.__send_event(aParam);
                bConsumed = true;
                break;
            case hx.key.KEY_POWER:
                return false;
            case hx.key.KEY_BACK:
            case hx.key.KEY_ESCAPE:
                if (!this._doNotUseBackExitKey) {
                    return false;
                } else {
                    return true;
                }
                break;
            case hx.key.KEY_MENU:
            case hx.key.KEY_CHANNEL_UP:
            case hx.key.KEY_CHANNEL_DOWN:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_PLAYER:
            case hx.key.KEY_ON_DEMAND:
                if (this._useOtherKey) {
                    return false;
                }
                break;
        }
        this.__send_timer_refresh(aParam);
        return bConsumed;
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
    _doFocus(aIndex?: any) {
        var btn = this.$.children('.button');
        btn.removeClass('on');
        btn.eq(aIndex === undefined ? this._last_focus : aIndex).addClass('on');
    }

    __send_timer_refresh(aParam?: any) {
        var index: number;
        this._last_focus = this.$.find('.on').prevAll('.focusable').length;
        aParam = (aParam) || {};
        aParam.alEvent = 'alMessageToPage';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData.me = this;
        this.sendEventToParent(aParam);
    }

    __send_event(aParam: any) {
        var index: number;
        aParam = (aParam) || {};
        aParam.alEvent = 'alClicked';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        if (this._msg_type === 'checkDialog') {
            if (this._$checkBox.hasClass('on')) {
                var checkImg = this._$checkBox.find('img');
                if (checkImg.hasClass('on')) {
                    checkImg.removeClass('on');
                    checkImg.attr('src', 'images/112_DIA_Check_Off.png');
                    aParam.alData.bChecked = false;
                } else {
                    checkImg.addClass('on');
                    checkImg.attr('src', 'images/112_DIA_Check_On.png');
                    aParam.alData.bChecked = true;
                }
                aParam.alData.input_title = 'checkBox';
            }
        }
        for (index = 0; index < this._btn_title_arr.length; index += 1) {
            if (this.$.children('.button').eq(index).is('.on')) {
                aParam.alData.input = index;
                aParam.alData.input_title = this._btn_title_arr[index];
                break;
            }
        }
        aParam.alData.me = this;
        aParam.alData.callback_fn = this._callback_fn;
        this.sendEventToParent(aParam);
    }
}
