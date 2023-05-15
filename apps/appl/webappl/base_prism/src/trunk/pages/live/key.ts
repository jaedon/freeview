///<reference path="../../def/sugar.d.ts" />
// file name : pages/live/key.js
/*jslint sloppy:true, nomen:true, vars:true */
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx;

class cCPgkey extends __PAGE_KERNEL__{

    constructor() {
        super();
    }

    create () : void {
        hx.al.compose(this);
    }
    destroy () : void {
    }
    isActive ()  : boolean {
        return true;
    }
    _on_alKey (aParam : any)  : boolean {
        var lkeyCode : number = aParam.alKey;
        var bkeyRepeat : boolean  = aParam.alRepeat;
        var bConsumed : boolean = false;
        var i : number = 0;
        var item : any = [hx.key.KEY_LEFT, hx.key.KEY_RIGHT, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_CHANNEL_UP, hx.key.KEY_CHANNEL_DOWN];
//        var item : any = hx.al.CPgkeyR.item;

        // allows some repeat keys
        if (bkeyRepeat === true) {
            bConsumed = true;
            for ( i = 0; i < item.length; i += 1) {
                if (item[i] === lkeyCode) {
                    bConsumed = false;
                    break;
                }
            }
        }
        return bConsumed;
    }
}
hx.al.CPgkey =  cCPgkey;
export =  cCPgkey;
//hx.al.CPgkeyR.item = [hx.key.KEY_LEFT, hx.key.KEY_RIGHT, hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_CHANNEL_UP, hx.key.KEY_CHANNEL_DOWN];
