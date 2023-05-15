///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cNotify.js
/* jslint sloppy:true, nomen:true, vars:true */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");
import __UI_COMPONENT__ = require("pages/util/iUiComponent");

export class Notify extends __COMPONENT_KERNEL__ {
    position: any = null;
    delay: number = 5000;
    auto_close: boolean = true;
    html: any;
    hide: any;
    timer: any;

    constructor() {
        super();
        $.extend(this, __UI_COMPONENT__.uiComponent.visibility);
    }

    create(param) {
        this.position = {
            top: 0,
            left: 0
        };
        $.extend(this, param);
        //hx.logadd('cConfirm');
        hx.log('cNotify', 'create');
        if (param.timeout !== undefined) {
            this.delay = param.timeout;
        }
        if (param.auto_close !== undefined) {
            this.auto_close = param.auto_close;
        }
        this.$ = $('<div class="notify" style="position:absolute;top:40px;right:60px;z-index:1000">' + this.html + '</div>');
    }

    destroy() {
        this.hide();
        this.$.remove();
        this.$ = null;
    }

    _onShow() {
        var that = this;
        this.$.css(this.position);
        if (this.auto_close === true) {
            this.timer = setTimeout(function () {
                that.hide();
            }, this.delay)
        }
    }

    _onHide() {
        this.$[0].innerHTML = '';
    }
}
