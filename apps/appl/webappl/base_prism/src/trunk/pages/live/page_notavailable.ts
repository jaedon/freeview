///<reference path="../../def/sugar.d.ts" />
// file name : pages/live/notavailable.js
/*jslint sloppy:true, nomen:true, vars:true */
import __PAGE_KERNEL__ = require("pages/page_kernel");

declare var hx;

class cCPgNotAvailable extends __PAGE_KERNEL__{
    $na : any;

    _multiCustomMsgHandle : any = null;
    _notAvailableIconHandle : any = null;
    _timer : number = -1;

    constructor() {
        super();
    }

    create () : void {
        $.extend(this, {
            $na : $('<div class="notify" style="position:absolute;top:40px;right:60px;z-index:1000">' + '<p id="notAvable" class="ico"></p>' + '</div>')
        });
		
        hx.al.compose(this);
        hx.logadd('CPgNotAvailable');
    }
    destroy () : void {
		this._notAvailableIconHide();
    }
    _on_alKey (aParam : any) : boolean {
        var k : any = hx.key;
        switch( aParam.alKey ) {
            case k.KEY_UP:
            case k.KEY_DOWN:
            case k.KEY_LEFT:
            case k.KEY_RIGHT:
            case k.KEY_MENU:
            case k.KEY_SEARCH:
            case k.KEY_ESCAPE:
            case k.KEY_POWER :
            case k.KEY_NETFLIX:
                break;
            default :
                hx.log('CPgNotAvailable', 'not consummed key : ' + aParam.alKey);
                this._notAvailableIconShow();
        }
        return false;
    }
    _notAvailableIconShow () : void {
        this._notAvailableIconHide();
        if (this._timer !== -1) {
            clearTimeout(this._timer);
            this._timer = -1;
        }
		$('#wrap').append(this.$na);
        this._timer = setTimeout(()=> { this._notAvailableIconHide(); }, 1500);
    }
    _notAvailableIconHide () : void {
        this.$na.remove();
    }
    _on_alMessageToPage (param : any) : void {
        if (param.query === 'show') {
            this._notAvailableIconShow();
        } else if (param.query === 'hide') {
            this._notAvailableIconHide();
        }
    }
}
hx.al.CPgNotAvailable =  cCPgNotAvailable;
export = cCPgNotAvailable;
