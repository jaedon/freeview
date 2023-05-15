///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cProgressBar.js
/* jslint sloppy:true, nomen:true, vars:true */

/*
 * message component
 * @param
 */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class progressBar extends __COMPONENT_KERNEL__ {
    _progress: any;
    _msg_type: string;
    _contents: any;
    _progress_bar_1: any;
    _progress_bar_2: any;
    _btn_title: string;
    _callback_fn: any;

    constructor() {
        super();
    }

    init() {
        // TODO:
    }

    create(aParam) {
        hx.logadd('cProgressBar');
        hx.log('cProgressBar', 'create');
        $.extend(this, aParam, {
            _contents: $('<p>', {
                'text': aParam.text,
            }),
            _progress: $('<dl>', {
                'class': 'progress type4'
            }),
            _progress_bar_1: $('<dd>', {
                'class': 'graph'
            }),
            _progress_bar_2: $('<span>', {
                'width': '0%'
            }),
            '_msg_type': aParam._msg_type,
            '_btn_title': aParam.btn_title,
        });
        var _attr: any = {
            'class': 'pro line1',
            'style': 'top:310px;'
        };
        if (this._msg_type === 'progressBarBtn' || this._msg_type === 'progressBtn') {
            $.extend(this, aParam, {
                '_callback_fn': aParam.callback_fn
            });
            _attr = {
                'class': 'pro vaBot'
            };
        }
        this._create({
            'tag': 'div',
            'attr': _attr
        });
        if (this._msg_type === 'progressBtn') {
            this.$.append(this._contents);
        } else {
            this.$.append(this._contents)//
                .append(this._progress//
                    .append(this._progress_bar_1//
                        .append(this._progress_bar_2)));
        }
        if (this._msg_type === 'progressBarBtn' || this._msg_type === 'progressBtn') {
            this.$.append($('<p>', {
                'class': 'button',
                //'text' : 'stop'
                'text': this._btn_title
            }));
        }
        this.$.focus(()=> {
            this._doFocus(0);
        });
        return this;
    }

    destroy() {
        this.$.remove();
    }

    // EVENT HANDLER
    _on_alKey(aParam) {
        var bConsumed: boolean = false;
        var kc: number = aParam.alKey;
        switch (kc) {
            case hx.key.KEY_UP:
            case hx.key.KEY_DOWN:
            case hx.key.KEY_LEFT:
            case hx.key.KEY_RIGHT:
                bConsumed = true;
                break;
            case hx.key.KEY_OK:
                if (this._msg_type == 'progressBarBtn' || this._msg_type == 'progressBtn') {
                    this.__send_event(aParam);
                }
                bConsumed = true;
                break;
        }
        return bConsumed;
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
    _doFocus(aIndex) {
        var btn = this.$.children('.button');
        btn.eq(aIndex).focus();
    }

    __send_event(aParam) {
        aParam = (aParam) || {};
        aParam.alEvent = 'alClicked';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData = (aParam.alData) || {};
        aParam.alData.me = this;
        if (this._msg_type == 'progressBarBtn' || this._msg_type == 'progressBtn') {
            aParam.alData.input_title = this._btn_title;
            aParam.alData.callback_fn = this._callback_fn;
        }
        this.sendEventToParent(aParam);
    }
}
