///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cCancelRecording.js
/* jslint sloppy:true, nomen:true, vars:true */

/*
 * message component
 * @param
 */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class cancelRecording extends __COMPONENT_KERNEL__ {
    _list: any;
    _list_arr: any;
    _title: string;
    _text: string;
    _text2: string;
    _last_focus: number;
    _callback_fn: any;

    constructor() {
        super();
    }
    create(aParam) {
        hx.logadd('cCancelRecording');
        hx.log('cCancelRecording', 'create');
        var i: number;
        $.extend(this, aParam, {
            _title: $('<h1>', {
                'text': aParam.title
            }),
            _text: $('<p>', {
                'text': aParam.text
            }),
            _list: $('<article>', {
                'class': 'stlist canlist icoRec'
            }),
            _list_arr: aParam.list_arr,
            _callback_fn: aParam.callback_fn,
            _msg_type: aParam._msg_type,
            _last_focus: 0
        });
        this._create({
            'tag': 'div',
            'attr': {
                'class': 'con vaBot'
            }
        });
        if (typeof (this._list_arr) === 'string') {
            this._list_arr = [this._list_arr];
        }
        for (i = 0; i < this._list_arr.length; i += 1) {
            var _o = this._list_arr[i];
            if (_o._subListNum === undefined || _o._subListNum === null) {
                _o._subListNum = 0;
            }
            this._list.append(this.setRow(_o._one, _o._two, _o._three, _o._subListNum));
        }
        if (aParam.text2) {
            this._text2 = $('<p>', {
                'style' : "margin : 10 0 -15"
            });
            this._text2[0].innerHTML = '<b>' + aParam.text2;
            this.$.append(this._title).append(this._text).append(this._text2).append(this._list);
        } else {
            this.$.append(this._title).append(this._text).append(this._list);
        }
        this.$.focus(()=> {
            this.setFocus();
        });
        hx.il.ani.action({
            type: 'in',
            targets: this.$
        });
        return this;
    }

    destroy() {
        this.$.remove();
    }

    setRow(aText1: string, aText2: string, aStatus: any, aSubListNum: number) {
        var that = this, i: number, _o: string;
        var tmp: any = $('<p>', {
            'class': 'button'
        });
        if (aSubListNum > 1) {
            var text1: string = ('<b style="max-width:200px;">' + aText1 + '</b>');
            var programText: string = (aSubListNum - 1 === 1) ? hx.l('LOC_PROGRAMME_ID') : hx.l('LOC_PROGRAMMES_ID');
            var subListText: string = ('<b class="point">' + hx.l('LOC_AND_ID') + ' ' + (aSubListNum - 1) + ' ' + programText + '</b>');
            var text2: string = (' ' + '<b class="value" style="max-width:460px;">' + aText2 + ' ' + subListText + '</b>');
            _o = text1 + text2;
        } else {
            _o = ('<b style="max-width:660px;">' + aText1);
            if (aText2) {
                _o += aText2;
            }
            _o += '</b>';
        }
        tmp.html(_o);
        if (aText2 && aSubListNum < 2) {
            if (aStatus) {
                tmp.append($('<span>', {
                    'class': aStatus
                }));
            }
        }
        return tmp;
    }

    setFocus(aIdx?: number) {
        aIdx = (aIdx === undefined ? this._last_focus : aIdx);
        if (this._last_focus !== aIdx) {
            this._list.children('p').eq(this._last_focus).removeClass('on');
        }
        this._list.children('p').eq(aIdx).addClass('on');
    }

    // EVENT HANDLER
    _on_alKey(aParam: any) {
        var tmp: any, index: number;
        var bConsumed: boolean = false;
        var kc: number = aParam.alKey;
        var _list_arr: any = this._list_arr;
        switch (kc) {
            case hx.key.KEY_UP:
                tmp = (this._list.children('.on').prevAll('.button').length - 1);
                if (tmp < 0) {
                    tmp = _list_arr.length - 1;
                }
                this.setFocus(tmp);
                bConsumed = true;
                break;
            case hx.key.KEY_DOWN:
                tmp = (this._list.children('.on').prevAll('.button').length + 1);
                if (tmp >= _list_arr.length) {
                    tmp = 0;
                }
                this.setFocus(tmp);
                bConsumed = true;
                break;
            case hx.key.KEY_OK:
                this.__send_event(aParam);
                bConsumed = true;
                break;
            case hx.key.KEY_LEFT:
            case hx.key.KEY_RIGHT:
            case hx.key.KEY_STOP:
            case hx.key.KEY_CHANNEL_UP:
            case hx.key.KEY_CHANNEL_DOWN:
            case hx.key.KEY_RECORD:
                bConsumed = true;
                break;
            case hx.key.KEY_BACK:
            case hx.key.KEY_POWER:
            case hx.key.KEY_ESCAPE:
                return false;
                break;
        }
        this.__send_timer_refresh(aParam);
        return bConsumed;
    }

    __send_timer_refresh(aParam) {
        var index: number;
        this._last_focus = (this._list.children('.on').prevAll('.button').length);
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
        var child: any = this._list.children('p');
        aParam = (aParam) || {};
        aParam.alEvent = 'alClicked';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        for (index = 0; index < child.length; index += 1) {
            if (child.eq(index).is('.on')) {
                aParam.alData.input = this._list_arr[index];
                break;
            }
        }
        aParam.alData = (aParam.alData) || {};
        aParam.alData.me = this;
        aParam.alData.input_title = 'stop';
        aParam.alData.callback_fn = this._callback_fn;
        this.sendEventToParent(aParam);
    }
}
