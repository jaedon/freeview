///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cPasswd.js
/* jslint sloppy:true, nomen:true, vars:true */

/*
 * wifi password component
 * @param
 */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class passwd extends __COMPONENT_KERNEL__ {
    _passwd_text: any;
    _response_text: string;
    _focuslist: any;
    _last_focus: number;
    _ok_btn: any;
    _no_btn: any;
    _string: string;
    _wifi: any;
    _contents: any;
    _callback_fn: any;
    _default_text: string;
    _type: string;
    _firstOK: boolean;

    constructor() {
        super();
    }

    init() {
    }

    create(aParam) {
        var type: string = 'text';
        var maxLength: number = 32;
        //if (aParam.type) {
        this._type = aParam.type || type;
        //}
        if (aParam.maxLength) {
            maxLength = aParam.maxLength;
        }
        $.extend(this, aParam, {
            _contents: $('<p>', {
                'text': aParam.text
            }),
            _wifi: aParam.wifi,
            _response_text: aParam.responseText,
            _passwd_text: $('<input class=' + 'wrap_input id=' + 'ms' + ' type=' + type + ' maxlength=' + maxLength + ' style=' + 'input-format:full;>'),
            _ok_btn: this.__make_btn(hx.l('LOC_OK_ID')),
            _no_btn: this.__make_btn(hx.l('LOC_CANCEL_ID')),
            _callback_fn: aParam.callback_fn,
            _last_focus: 0
        });
        if (!aParam.string) {
            this._string = hx.l('LOC_PASSWORD_ID');
        } else {
            this._string = aParam.string;
        }
        this._focuslist = [];
        this._focuslist.length = 0;
        this._focuslist.push(this._passwd_text);
        this._focuslist.push(this._ok_btn);
        this._focuslist.push(this._no_btn);
        this._create({
            'tag': 'div',
            'attr': {
                'class': 'pas',
                'style': 'top:225px;'
            }
        });
        this.$//
            .append($('<form method="post">')//
                .append(this._contents)//
                .append($('<p class="inputPw">').html(this._string)).append(this._passwd_text).append(this._ok_btn).append(this._no_btn));
        //that.$.focus(function() {
        this.setFocus();
        this._default_text = hx.l('LOC_PRESS_OK_ID');
        this._passwd_text[0].value = this._default_text;
        //});
        this._firstOK = true;
        return this;
    }

    destroy() {
        this._ok_btn.blur();
        this._no_btn.blur();
        this.$.remove();
    }

    __make_btn(aTxt) {
        return $('<p class="button">').text(aTxt);
    }

    _on_alKey(aParam) {
        var result: boolean = false,
            _key: number = aParam.alKey,
            focusedList: any = this.$.children('form').children('.on'), i: number, index: number = 0;
        switch (_key) {
            case hx.key.KEY_UP:
                for (i = 0; i < this._focuslist.length; i += 1) {
                    if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                        index = i;
                        break;
                    }
                }
                if (i === 0) {
                    this.setFocus(index += 1);
                } else if (i === 1 || i === 2) {
                    this.setFocus(0);
                }
                result = true;
                break;
            case hx.key.KEY_DOWN:
                for (i = 0; i < this._focuslist.length; i += 1) {
                    if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                        index = i;
                        break;
                    }
                }
                if (i === 0) {
                    this.setFocus(index += 1);
                } else if (i === 1 || i === 2) {
                    this.setFocus(0);
                }
                result = true;
                break;
            case hx.key.KEY_LEFT:
            case hx.key.KEY_RIGHT:
                for (i = 0; i < this._focuslist.length; i += 1) {
                    if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                        index = i;
                        break;
                    }
                }
                if (i === 1) {
                    this.setFocus(index += 1);
                } else if (i === 2) {
                    this.setFocus(index -= 1);
                } else {
                    return true;
                }
                result = true;
                break;
            case hx.key.KEY_OK:
                for (i = 0; i < this._focuslist.length; i += 1) {
                    if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                        index = i;
                        break;
                    }
                }
                if (index === 0) {
                    if (this._firstOK) {
                        this._passwd_text[0].value = '';
                        this._passwd_text[0].type = this._type;
                        this._firstOK = false;
                    }
                    this._input_pass();
                    this._passwd_text.blur();
                    this._passwd_text.focus();
                } else {
                    this.__send_event(aParam, index);
                }
                result = true;
                break;
            case hx.key.KEY_BACK:
            case hx.key.KEY_POWER:
            case hx.key.KEY_ESCAPE:
                return false;
                break;
        }
        //that.__send_timer_refresh(param);
        return result;
    }

    setFocus(index?: number) {
        var i: number,
            _length: number = this._focuslist.length;
        index = (index === undefined ? this._last_focus : index);
        if (index >= _length) {
            index = 0;
        } else if (index < 0) {
            index = (_length - 1);
        }
        for (i = 0; i < this._focuslist.length; i += 1) {
            this._focuslist[i].removeClass('on');
        }
        if (index === 0) {
            this._passwd_text.attr('class', 'wrap_input on');
        } else {
            this._passwd_text.attr('class', 'wrap_input blind');
        }
        this._focuslist[index].addClass('on');
    }

    _input_pass() {
        var tmp: any;
        if (false) {    //that._response_text    It will make a problem
            tmp = hx.msg('response', {
                text: this._response_text, //'Please type in a password and press enter.',
                auto_close: true, //change false~!!!!!
                timeout: 2000,
                dia_class: 'osd',
                callback_fn: fn
            });
        }
        function fn(_status) {
        }
    }

    __send_timer_refresh(aParam) {
        var focusedList = this.$.children('form').children('.on'), i: number, index: number = 0;
        for (i = 0; i < this._focuslist.length; i += 1) {
            if (focusedList.get(0).outerHTML === this._focuslist[i].get(0).outerHTML) {
                index = i;
                break;
            }
        }
        this._last_focus = index;
        aParam = (aParam) || {};
        aParam.alEvent = 'alMessageToPage';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData.me = this;
        this.sendEventToParent(aParam);
    }

    __send_event(aParam, aIndex: number) {
        aParam = (aParam) || {};
        aParam.alEvent = 'alClicked';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData.me = this;
        aParam.alData.callback_fn = this._callback_fn;
        if (aIndex === 1) {
            aParam.alData.input_title = this._ok_btn.text();
            aParam.alData.password = (this._passwd_text[0].value === this._default_text) ? '': this._passwd_text[0].value;
            aParam.alData.wifi = this._wifi;
        } else {
            aParam.alData.input_title = 'key_back';
        }
        this.sendEventToParent(aParam);
    }
}
