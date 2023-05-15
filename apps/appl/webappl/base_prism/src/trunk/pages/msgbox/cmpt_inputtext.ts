///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cInputText.js
/* jslint sloppy:true, nomen:true, vars:true */

/*
 * wifi password component
 * @param
 */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class inputtext extends __COMPONENT_KERNEL__ {
    _focuslist: any;
    _input_html: any;
    _next_btn: any;
    _cancel_btn: any;
    _last_focus: number;
    _callback_fn: any;

    constructor() {
        super();
    }

    init() {
    }

    create(aParam) {
        $.extend(this, aParam, {
            _input_html: $('<input class=' + 'wrap_input' + ' type=' + (aParam.type || 'text') + ' maxlength=32' + ' style=' + 'input-format:full;top:304px;>'),
            _cancel_btn: $('<p>', {
                'class': 'button cancel'
            }).text(hx.l('LOC_CANCEL_ID')),
            _next_btn: $('<p>', {
                'class': 'button next'
            }).text(hx.l('LOC_NEXT_ID')),
            _callback_fn: aParam.callback_fn,
            _last_focus: 0
        });
        this._focuslist = [];
        this._focuslist.length = 0;
        this._focuslist.push(this._input_html);
        this._focuslist.push(this._next_btn);
        this._focuslist.push(this._cancel_btn);
        this._create({
            'tag': 'osd',
            'attr': {
                'class': 'pas',
                //'style' : 'top:345px;',
            }
        });
        this.$//
            .append($('<form method="post">')//
                .append(this._input_html).append(this._cancel_btn).append(this._next_btn));
        this.setFocus();
        return this;
    }

    destroy() {
        this._next_btn.blur();
        this._cancel_btn.blur();
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
                    this._input_html.focus();
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

    setFocus(aIndex?: number) {
        var i: number;
        var _length: number = this._focuslist.length;
        aIndex = (aIndex === undefined ? this._last_focus : aIndex);
        if (aIndex >= _length) {
            aIndex = 0;
        } else if (aIndex < 0) {
            aIndex = (_length - 1);
        }
        for (i = 0; i < this._focuslist.length; i += 1) {
            this._focuslist[i].removeClass('on');
        }
        if (aIndex === 0) {
            this._input_html.attr('class', 'wrap_input on');
        } else {
            this._input_html.attr('class', 'wrap_input blind');
        }
        this._focuslist[aIndex].addClass('on');
    }

    __send_timer_refresh(aParam) {
        var focusedList: any = this.$.children('form').children('.on'), i: number, index: number = 0;
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

    __send_event(aParam, aIndex) {
        aParam = (aParam) || {};
        aParam.alEvent = 'alClicked';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData.me = this;
        aParam.alData.callback_fn = this._callback_fn;
        if (aIndex === 1) {
            aParam.alData.input_title = this._next_btn.text();
            aParam.alData.input = this._input_html[0].value;
        } else {
            aParam.alData.input_title = 'key_back';
        }
        this.sendEventToParent(aParam);
    }
}
