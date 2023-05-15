//file name : pages/tvguide/cGuideDcn.js
/*jslint sloppy:true, nomen:true, vars:true*/
///<reference path="../../def/sugar.d.ts" />
declare var hx;
import __KEYNAVIGATOR__ = require("pages/component/cmpt_keynavigator");

class cGuideDcn extends __KEYNAVIGATOR__ {
    isActive = false;
    cmptDcn = null;
    $: JQuery;
    _$dim: JQuery;
    _timerId: any;
    constructor (){
        super();
        this.$ = $('#ltv');
        this._$dim = $('<div/>');
        this._timerId = null;
    }
    show(num, group) {
        var p = {};

        this.isActive = true;
        this._create(num, group);
    }
    clearDcn() {
        this._destroy();
        this.isActive = false;
    }
    getFocusedChannelInfo() {
        var param: any = {};

        param.alData = this.cmptDcn.getFocusedChannelInfo();
        return param;
    }
    _create(aNum, aGroup) {
        this.cmptDcn = hx.al.component('dcn', {
            'tag' : 'div',
            'attr' : {
                'class' : 'chChange'
            },
            'startnum' : aNum,
            'group' : aGroup,
            'frontPanelText' : '-TV GUIDE-'
        });

        this.$.prepend(this._$dim.append(this.cmptDcn.$));
        this._$dim.addClass('dimCHange');

        this._timerId = setTimeout(() => {
            this.clearDcn();
        }, 5000);
    }
    _destroy() {
        hx.al.decomponent(this.cmptDcn);
        if (this._timerId !== null) {
            clearTimeout(this._timerId);
            this._timerId = null;
        }

        this.$.removeClass('aniActive');
        this._$dim.remove();
        this.$.addClass('aniOut');
    }
}
export = cGuideDcn;