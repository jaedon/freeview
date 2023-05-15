///<reference path="../../def/sugar.d.ts" />
// file name : pages/live/wide.js
/*jslint sloppy:true, nomen:true, vars:true */
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx;

class cCPgWide extends __PAGE_KERNEL__{
    $_image : any;
    $_text : any;

    _$section : any;
    _curIndex : number = 0;
    _bActivate : boolean = false;
    _timerId : number = -1;
    _aspectRatio : string = '16:9';
    _format : string = 'auto';

    constructor() {
        super();
    }

    create () : void {
        var that = this;
        hx.al.CPgWide.items = {
		    r169 : [['wide r16 auto', hx.l('LOC_AUTO_ID')], ['wide r16 pill', hx.l('LOC_PILLARBOX_ID')], ['wide r16 zoom', hx.l('LOC_ZOOM_ID')]],
		    r43 : [['wide r04 auto', hx.l('LOC_AUTO_ID')], ['wide r04 lbox', hx.l('LOC_LETTERBOX_ID')], ['wide r04 cent', hx.l('LOC_CENTER_ID')]]
		};

        $.extend(that, {
            _$section : $('#ltv')
        });
        hx.al.compose(that);
    }
    destroy () : void {
        this._hide();
    }
    isActive () : boolean {
        return this._bActivate;
    }
    //visible () : boolean {
    //    return this._bActivate;
    //}
    _on_alKey (aParam : any) {
        var ulkeyCode : number = aParam.alKey;
        var bConsumed : boolean = true;
        // not allow other key

        switch (ulkeyCode) {
            case hx.key.KEY_POWER:
                this._hide();
                bConsumed = false;
                break;
            case hx.key.KEY_BACK:
            case hx.key.KEY_ESCAPE:
                this._hide();
                break;
            case hx.key.KEY_SCREEN_MODE_NEXT:
                this._curIndex += 1;
                if (this._curIndex >= hx.al.CPgWide.itemMax) {
                    this._curIndex = 0;
                }
                this._apply();
                this._update();
                break;
            default:
                break;
        }
        return bConsumed;
    }
    _on_alMessageToPage (param : any) : void {
        var i : number;

        if (param.query === 'show') {
            if (this._bActivate === false) {
                var fl : any = hx.al.CPgWide.formatList;
                var res : any = hx.svc.SETTING_UTIL.getPictureInfo();

                this._aspectRatio = res.tvAspectRatio;
                this._format = res.videoDisplayFormat;

                this._curIndex = 0;
                for ( i = 0; i < hx.al.CPgWide.itemMax; i += 1) {
                    if (this._format === fl[i]) {
                        this._curIndex = i;
                        break;
                    }
                }
                this._show();
            }
        }
    }
    _apply () : void {
        var fl : any = hx.al.CPgWide.formatList;
        var format : string;

        format = fl[this._curIndex];

        // set format
        var param : any = {
            'tvAspectRatio' : this._aspectRatio,
            'videoDisplayFormat' : format
        };

        hx.svc.SETTING_UTIL.setPictureInfo(param);

    }
    _show () {
        var item : any;

        if (this._aspectRatio === '16:9') {
            item = hx.al.CPgWide.items.r169[this._curIndex];
        } else {
            item = hx.al.CPgWide.items.r43[this._curIndex];
        }

        // timer reset
        if (this._timerId !== -1) {
            clearTimeout(this._timerId);
            this._timerId = -1;
        }

        // set timer
        this._timerId = setTimeout(()=> {
            this._hide();
        }, 3000);

        // extend chChangeWrap
        $.extend(this, {
            $_image : $('<article>', {
                'class' : item[0]
            }),
            $_text : $('<p>', {
                'text' : item[1]
            })
        });

        // append elements
        this._$section//
        .append(this.$_image//
        .append(this.$_text));

        this._bActivate = true;
        this.sendEventToEntity({
            alEvent : 'alCheckPageVisible'
        });
    }
    _hide () : void {
        // kill timer
        if (this._timerId !== -1) {
            clearTimeout(this._timerId);
            this._timerId = -1;
        }

        // remove subWrap
        if (this._bActivate === true) {
            if (this.$_image !== undefined) {
                this.$_image.remove();
            }
        }

        this._bActivate = false;
        this.sendEventToEntity({
            alEvent : 'alCheckPageVisible'
        });
    }
    _update () : void {
        this._hide();
        this._show();
    }
}
hx.al.CPgWide =  cCPgWide;

hx.al.CPgWide.itemMax = 3;
hx.al.CPgWide.formatList = ['Auto', 'Sqeeze', 'CenterCutOut'];
hx.al.CPgWide.items = {
    r169 : [['wide r16 auto', hx.l('LOC_AUTO_ID')], ['wide r16 pill', hx.l('LOC_PILLARBOX_ID')], ['wide r16 zoom', hx.l('LOC_ZOOM_ID')]],
    r43 : [['wide r04 auto', hx.l('LOC_AUTO_ID')], ['wide r04 lbox', hx.l('LOC_LETTERBOX_ID')], ['wide r04 cent', hx.l('LOC_CENTER_ID')]]
};
export = cCPgWide;