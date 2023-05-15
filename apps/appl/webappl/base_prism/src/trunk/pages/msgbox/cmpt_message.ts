///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cMessage.js
/* jslint sloppy:true, nomen:true, vars:true */

/*
 * cMessage component
 * @param
 */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class cMessage extends __COMPONENT_KERNEL__ {
    _contents: any;

    constructor() {
        super();
    }

    init() {
        // TODO:
    }

    create(aParam) {
        hx.logadd('cMessage');
        hx.log('cMessage', 'create');
        $.extend(this, aParam, {
            _contents: $('<p>', {
                'html': aParam.text,
            })
        });
        var attr: any = {};
        attr['id'] = 'sta';
        attr['class'] = 'zSta';
        //TODO attr.class= 'sta'
        attr['style'] = 'top:310px';
        //TODO attr.style = 'top:310px';
        this._create({
            'tag': 'div',
            'attr': attr
        });
        this.$.append(this._contents);
        return this;
    }

    destroy() {
        this.$.remove();
    }

    // EVENT HANDLER
    _on_alKey(aParam): boolean {
        return false;
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
}
