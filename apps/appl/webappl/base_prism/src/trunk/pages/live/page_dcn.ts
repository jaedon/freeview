///<reference path="../../def/sugar.d.ts" />
// file name : pages/live/dcn.js
/*jslint sloppy:true, nomen:true, vars:true */

/**
 * dcn page
 *
 * _bActive : dcn active status
 * _timerId : timer id handle
 */
import __PAGE_KERNEL__ = require("pages/page_kernel");

declare var hx;
class cCPgDcn extends __PAGE_KERNEL__{
    $wrap : any;
    _$dim : any;
    _dcn : any;
    _timerId : number = -1;
    _bActive : boolean = false;
    _$section : any;

    constructor() {
        super();
    }

    create () : void {
        this._bActive = false;
        this._timerId = -1;

        $.extend(this, {
            _$section : $('#ltv'),
            _$dim : $('<div>')
        });

        this.$wrap = $('#wrap');
        hx.al.compose(this);

        if(hx.config.netflix) {
            hx.svc.NETFLIX.addEventCb('HideChInfo', this, function(aParam) {
                this._on_HideChInfo(aParam);
            });
        }
    }
    destroy () : void {
        this.__clear_dcn();
        this._dcn = undefined;
    }
    isActive () : boolean {
        return this._bActive;
    }
    //visible () : boolean {
    //    return this._bActive;
    //}
    // EVENT HANDLER
    _on_alUpdate (aParam : any ) {
        var data : any = aParam.alData;
        if (data) {
            if (data.result) {
                switch(data.result) {
                    case 'setChannelFail':
                        if (hx.config.useTVStreaming === false) {
                            hx.msg('response', {
                                text : hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                auto_close : true,
                                close_time : 3000
                                //dia_class : string( 'osd' : background dim transaction )
                            });
                        } else {
                            hx.msg('response', {
                                text : hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                auto_close : true,
                                close_time : 3000
                                //dia_class : string( 'osd' : background dim transaction )
                            });
                        }
                        break;
                    case 'setChannelSuccess':
                        break;
                }
                this.__clear_dcn();
            } else {
                // update timer
                if (this._bActive && (this._timerId !== -1)) {
                    clearTimeout(this._timerId);
                     this._timerId = setTimeout(()=> {
                        this.__do_execute(this._dcn.getFocusedChannelId());
                    }, 3000);
                }
            }
        }
    }
    _on_alClicked (aParam : any ) : boolean {
        if (aParam.alData && aParam.alData.index !== -1) {
            this.__do_execute(aParam.alData.ccid);
        }
        return true;
    }
    _on_alKey (aParam : any) : boolean {
        var bConsumed : boolean = false, k : any = hx.key, kc : number = aParam.alKey;
        if (this._bActive) {
            switch(kc) {
                case k.KEY_BACK:
                case k.KEY_ESCAPE:
                    this.__clear_dcn();
                    bConsumed = true;
                    break;

                case k.KEY_UP:
                case k.KEY_DOWN:
                    bConsumed = true;
                    break;
                case k.KEY_CHANNEL_UP:
                case k.KEY_CHANNEL_DOWN:
                case k.KEY_MENU:
                    this.__clear_dcn();
                    bConsumed = true;
                    break;
            }
        }
        return bConsumed;
    }
    _on_alExit (aParam : any) : boolean {
        this.__clear_dcn();
        return true;
    }
    _on_alMessageToPage (aData : any) : void {
        if (aData && aData.query) {
            this['__query_' + aData.query](aData.param, aData.data.group);
        }
    }
    // PRIVATE FUNCTION FOR CONTENT MAKING
    __query_active (aStartNum : number, aGroup : any) : void {
        if (this._bActive === false) {
            this.__make_dcn(aStartNum, aGroup);
            this._bActive = true;
            this.sendEventToEntity({
                alEvent : 'alCheckPageVisible'
            });

            this._timerId = setTimeout(()=> {
                this.__do_execute(this._dcn.getFocusedChannelId());
                //that.__clear_dcn();
            }, 3000);
        }
    }
    __make_dcn (aNum : number, aGroup : any) : void {
        this._dcn = hx.al.component('dcn', {
            'tag' : 'div',
            'attr' : {
                'class' : 'chChange'
            },
            'startnum' : aNum,
            'group' : aGroup
        });

        // append components
        this.append(this._dcn);

        // append elements
        this._$section//
        .prepend(this._$dim//
        .append(this._dcn.$));

        this._dcn.on();
        this._$dim.addClass('dimCHange');
    }
    __clear_dcn () : void {
        if (this._bActive === true) {
            hx.al.decomponent(this._dcn);

            if (this._timerId !== -1) {
                clearTimeout(this._timerId);
                this._timerId = -1;
            }
            this._bActive = false;
            this.sendEventToEntity({
                alEvent : 'alCheckPageVisible'
            });
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgInfoHub?dcn=clear'
            });
        }
        this._$dim.remove();
    }
    __do_execute (aCcid : number) : boolean {
        if (this.getPageVisible('CPgInfoHub')//
        && (this.$wrap.find('#channelList').length !== 0 || this.$wrap.find('#iPlate').length !== 0)) {
            this.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgInfoHub?dcn=' + aCcid
            });
            //this.__clear_dcn();
            //return true;
        }
        var bSet = this.__set_channel();
        if (!bSet) {
            this.__clear_dcn();
        }
        return true;
    }
    __set_channel () : any {
        return this._dcn.setChannel();
    }
    _on_HideChInfo () {
        var that = this;
        hx.log("info", "[CPgDcn][_on_HideChInfo] clear & disable DCN");
        if (that._bActive) {
            that.__clear_dcn();
        }
    }
}
hx.al.CPgDcn = cCPgDcn;
export = cCPgDcn;