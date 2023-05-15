///<reference path="../../def/sugar.d.ts" />
// file name : pages/live/live.js
/*jslint sloppy:true, nomen:true, vars:true */
declare var hx: ifSugar;
import __PAGE_KERNEL__ = require("pages/page_kernel");

/** CPgLive */

class cCPgLive extends __PAGE_KERNEL__ {
    $wrap : any;
    $liveScreen : any;
    $frame : any;

    constructor(aServiceName: string) {
        super(aServiceName);
    }

    create () {
        $.extend(this, {
            $wrap : $('#wrap'),
            $liveScreen : $('<div>', {
                'id' : 'dLive'
            }),
            $frame : $('<section>', {
                'id' : 'ltv'
            })
        });
        if (hx.emulMode) {
            this.$wrap.append(this.$liveScreen);
        }
        this.$wrap.append(this.$frame);

        hx.al.compose(this);
    }
    destroy () : void {
    }
}

export = cCPgLive;
