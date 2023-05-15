///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cResponse.js
/*jslint sloppy:true, nomen:true, vars:true */

/*
 * message component
 * @param
 */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class response extends __COMPONENT_KERNEL__{
    $: JQuery;
    recIcon: any;
    text_detail: any;
    _block_key: boolean;
    _msg_type: string;
    _contents: any;
    _use_backkey: boolean;

    constructor() {
        super();
    }

    init() {
        // TODO:
    }

    create(aParam) {
        hx.logadd('cResponse');
        hx.log('cResponse', 'create');
        this._block_key = false;
        $.extend(this, aParam, {
            _contents: $('<p>', {
                'html': aParam.text
            })
        });
        var _attr: any = {
            'class': 'res vaTop'
        };
        if (this._msg_type === 'progress') {
            this._block_key = !aParam.enable_key;
            _attr = {
                'class': 'pro vaTop line1'
            };
        } else if (this._msg_type === 'recordStatus') {
            if (this.recIcon) {
                _attr = {
                    'class': 'rps ' + this.recIcon + ' vaTop'
                };
            } else {
                _attr = {
                    'class': 'rps vaTop'
                };
            }
        }
        this._create({
            'tag': 'div',
            'attr': _attr
        });
        this.$.append(this._contents);
        if (this._msg_type === 'recordStatus' && this.text_detail) {
            this.$.append($('<p>', {
                'class': 'prd',
                'html': this.text_detail
            }));
        }
        //if((param.auto_close == undefined ? param.auto_close = true : param.auto_close)){
        //	this._setClosedTimer(param);
        //}
        return this;
    }

    destroy() {
        this.$.remove();
    }

    // EVENT HANDLER
    _on_alKey(aParam): boolean {
        var _key: number = aParam.alKey;
        var bConsumed: boolean = false;
        if (this._block_key) {
            if (_key === hx.key.KEY_BACK && this._use_backkey) {
                return false;
            } else {
                return true;
            }
        }
        /*
         switch (_key) {
         case hx.key.KEY_UP:
         case hx.key.KEY_DOWN:
         case hx.key.KEY_LEFT:
         case hx.key.KEY_RIGHT:
         case hx.key.KEY_OK:
         bConsumed = true;
         break;
         }*/
        return bConsumed;
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
    /*_timerId : 0,
     _setClosedTimer : function (param) {
     var a = this;
     if(!param.timeout){
     param.timeout = 3000;
     }
     clearTimeout(a._timerId);
     a._timerId = setTimeout(function () {
     hx.msg.close(param._msg_type, a);
     }, param.timeout );
     }*/
}
