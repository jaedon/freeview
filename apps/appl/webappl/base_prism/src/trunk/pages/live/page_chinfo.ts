///<reference path="../../def/sugar.d.ts" />
// file name : pages/live/chinfo.js
/*jslint sloppy:true, nomen:true, vars:true */
import __PAGE_KERNEL__ = require("pages/page_kernel");
import __DEF__ = require("def/oipf.def");
import __PROVIDER__ = require('pages/util/provider');
declare var hx;
declare var dateFormat;

class cCPgChInfo extends __PAGE_KERNEL__{
    $_grp2 : any;
    $_progress : any;
    $_num : any;
    $_svc : any;
    $_evt : any;
    $_st : any;
    $_et : any;
    $_dim : any;
    $_match : any;
    $_chChangeWrap : any;
    $_grp : any;

    _$section : any;
    _bActivate : boolean;
    _channel: any = null;
    _tuneInfo: any = null;
    _event : string = '';
    _st : number = 0;
    _et : number = 0;
    _percentage : number = 0;
    _timeOutID : number = -1;
    _OSDtimeOut : number = 3; // seconds
    _getEPGTimer : number = -1;

    constructor() {
        super();
    }

    init () {
        hx.logadd('CPgChInfo');
    }
    create () : void {
        this._bActivate  = false;

        $.extend(this, {
            _$section : $('#ltv'),
            $_dim : $('<div>', {
                'class' : 'dimCHange'
            }),
            $_chChangeWrap : $('<div>', {
                'class' : 'chChange'
            }),
            $_match : $('<dl>', {
                'class' : 'match'
            }),
            $_num : $('<span>', {
                'text' : ''
            }),
            $_svc : $('<dt>', {
                'text' : ''
            }),
            $_evt : $('<dd>', {
                'text' : ''
            }),
            $_progress : $('<dl>', {
                'class' : 'progress'
            }),
            $_st : $('<dd>', {
                'class' : 'time srt',
                'text' : ''
            }),
            $_et : $('<dd>', {
                'class' : 'time end',
                'text' : ''
            }),
            $_grp : $('<dd>', {
                'class' : 'graph'
            }),
            $_grp2 : $('<span>', {
                'class' : 'live'
            })
        });
        hx.al.compose(this);
    
        hx.svc.AV_CTRL.addEventCb('evtSetChannel', this, (aParam) => {
            this._on_evtSetChannel(aParam);
        });
        hx.svc.AV_CTRL.addEventCb('ProgrammesChanged', this, (aParam) => {
            this._on_ProgrammesChanged(aParam);
        });

        if(hx.config.netflix) {
            hx.svc.NETFLIX.addEventCb('HideChInfo', this, function(aParam) {
                this._on_HideChInfo(aParam);
            });
        }

        this._OSDtimeOut = hx.svc.SETTING_UTIL.getInfoDisplayTimeout();
    }
    destroy ()  : void {
        hx.svc.AV_CTRL.clearEventCb(this);
        hx.svc.NETFLIX.clearEventCb(this);
        this._hide();
    }
    isActive ()  : boolean {
        return this._bActivate;
    }
    _show ()  : void {
        if (hx.config.netflix) {
            if(hx.svc.NETFLIX.isRunning()) {
                return;
            }
        }
        var ulnum : number = 0;
        var cname : string = '', ChInfo : any;
        if (this._OSDtimeOut === 0) {// off
            return;
        }
        if (this._channel !== null) {
            ChInfo = this._channel;
            ulnum = ChInfo.majorChannel;
            cname = ChInfo.name;
        }
        // time out
        this._updateTimeOut();

        // update data
        this._update();

        this._$section//
        .append(this.$_dim.append(this.$_chChangeWrap//
        .append(this.$_match//
            .append(this.$_num)//
            .append(this.$_svc)//
            .append(this.$_evt)//
            .append(this.$_progress
                .append(this.$_st)
                .append(this.$_et)
                .append(this.$_grp.append(this.$_grp2)))
        )));
        this._bActivate = true;
    }
    _hide () : void {
        clearTimeout(this._timeOutID);
        clearTimeout(this._getEPGTimer);
        if (this.$_dim !== undefined) {
            this.$_dim.remove();
        }
        this._bActivate = false;
    }
    _update () : void {
        if (hx.config.netflix) {
            if(hx.svc.NETFLIX.isRunning()) {
                return;
            }
        }
        var ulnum  : number = 0;
        var cname : string = '';
        if (this._channel !== null) {
            ulnum = this._channel.majorChannel;
            cname = this._channel.name;
        }
        this.$_num.text(hx.il.makeDigitNumber(ulnum));
        this.$_svc.text(cname);
        this.$_evt.text(this._event);
        this.$_st.text(dateFormat(new Date(this._st * 1000), "HH:MM"));
        this.$_et.text(dateFormat(new Date(this._et * 1000), "HH:MM"));
        this.$_grp2.width(this._percentage + '%');
        
        if(this._event === '') {
            this.$_progress[0].style.display = 'none';
        } else {
            this.$_progress[0].style.display = 'block';
        }
    }
    _updateTimeOut ()  : void {
        var ultimeout : number = this._OSDtimeOut;

        if (ultimeout === undefined) {
            ultimeout = 5;
        }
        ultimeout += 1;
        // additional time

        // kill timer
        if (this._timeOutID !== -1) {
            clearTimeout(this._timeOutID);
            this._timeOutID = -1;
        }
        // set timer
        this._timeOutID = setTimeout(()=> {
            this._hide();
        }, ultimeout * 1000);
    }
    _on_alKey (aParam : any )  : boolean {
        var ulkeyCode  : number = aParam.alKey;
        var k  : any = hx.key;
        var bConsumed  : boolean = false;

        switch (ulkeyCode) {
            case k.KEY_0:
            case k.KEY_1:
            case k.KEY_2:
            case k.KEY_3:
            case k.KEY_4:
            case k.KEY_5:
            case k.KEY_6:
            case k.KEY_7:
            case k.KEY_8:
            case k.KEY_9:
            case k.KEY_SCREEN_MODE_NEXT:
                // bypassing numeric key for DCN
                if (this._bActivate) {
                    this._hide();
                    bConsumed = false;
                }
                break;
            case k.KEY_GUIDE :
            case k.KEY_ON_DEMAND:
            case k.KEY_ESCAPE:
            case k.KEY_PLAYER:
            //case k.KEY_BACK:
            case k.KEY_OK:
                if (this._bActivate) {
                    this._hide();
                    bConsumed = false;
                }
                break;
            default:
                break;
        }
        return bConsumed;
    }
    _callEpg (aChannel : any, aTuneInfo : __DEF__.eMhegTuneInfo) : void {
        // clear event
        this._event = '';
        this._st = 0;
        this._et = 0;
        this._percentage = 0;
        var result = hx.svc.EPG.getLiveProgrammes({
            'channel' : aChannel
        });
        var evtUpdated : boolean = false;
        if (this.getCenterPageName() !== 'CPgLiveController') {
            return;
        }
        if (result) {
            var prog : any = result[0][0];
            if (prog) {
                this._event = prog.name;
                this._st = prog.getStartTime();
                this._et = prog.getEndTime();
                this._percentage = prog.getPercentage();
                evtUpdated = true;
            }
        }
        if (aTuneInfo ? !(aTuneInfo & __DEF__.eMhegTuneInfo.TUNEMODE_NO_IPLATE) : true) {
            if (evtUpdated === true) {
                if (this._bActivate === true) {
                    this._update();
                } else {
                    this._show();
                }
            }
        }
    }
    _updateEpg (aEvent: __PROVIDER__.CProgrammeProvider) : void {
        if (!aEvent) {
            return;
        }
        this._event = aEvent.getProgrammeName();
        this._st = aEvent.getStartTime();
        this._et = aEvent.getEndTime();
        this._percentage = aEvent.getPercentage();
        if (this._tuneInfo ? !(this._tuneInfo & __DEF__.eMhegTuneInfo.TUNEMODE_NO_IPLATE) : true) {
            this._update();
        }
    }
    _on_evtSetChannel (aParam : any ) : void  {
        var ulgetEPGtime : number = (this._OSDtimeOut > 4) ? 3000 : 0;
        if (!aParam || aParam === null) {
            return;
        }
        if (this.getCenterPageName() !== 'CPgLiveController') {
            clearTimeout(this._getEPGTimer);
            return;
        }
        this._channel = aParam.channel;
        this._tuneInfo = aParam.tuneInfo;
        this._event = '';
        this._st = 0;
        this._et = 0;
        this._percentage = 0;
        clearTimeout(this._getEPGTimer);
        this._getEPGTimer = setTimeout(() => {
            if (this.getCenterPageName() !== 'CPgLiveController') {
                return;
            }
            //this._callEpg(this._channel, aParam.tuneInfo);
        }, ulgetEPGtime);
        if (aParam.tuneInfo ? !(aParam.tuneInfo & __DEF__.eMhegTuneInfo.TUNEMODE_NO_IPLATE) : true) {
            if (this._bActivate === true) {
                this._update();
                this._updateTimeOut();
            } else {
                this._show();
            }
        }
    }
    _on_ProgrammesChanged(aParam : any) : void {
        if (this.getCenterPageName() !== 'CPgLiveController') {
            return;
        }
        if (this._bActivate) {
            if (aParam && aParam instanceof __PROVIDER__.cPdeProgrammeItem) {
                this._updateEpg(new __PROVIDER__.CProgrammeProvider({oipfData : aParam.getSourceItem()}));
            } else {
                this._callEpg(this._channel, this._tuneInfo);
            }
        }
    }
    _on_alMessageToPage (aParam) : void {
        if (aParam.query === 'update') {
            var o = hx.svc.WEB_STORAGE.getLastService();
            if (o.ccid !== null && o.group !== null) {
                var o2 : any = hx.svc.CH_LIST.getChannel({
                        ccid : o.ccid,
                        group : o.group
                });
                if (o2 && o2.channel) {
                    this._on_evtSetChannel({
                        channel : o2.channel
                    });
                }
            }
        }
    }
    _on_HideChInfo () {
        var that = this;
        hx.log("info", "[CPgChInfo][_on_HideChInfo] hide!!");
        if (that._bActivate) {
            that._hide();
        }
    }
}
hx.al.CPgChInfo =  cCPgChInfo;
export =  cCPgChInfo;