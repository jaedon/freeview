/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvClockRecovery.js
//    CLOCK_RECOVERY: ['layer_domain/service/dl_dsrvClockRecovery.js'],

/*
 *
 * This is template of direct service.
 *
 * If you want to add direct service. please copy and commit and then modify.
 *
 */
declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

class cDLClockRecovery extends __KERNEL__ implements ifSVCClockRecovery {
    private _oipfLocalSystemObj: any = this.getPluginInstance('LocalSystem');
    private _oipfClockRecoverObj = this._oipfLocalSystemObj.clockRecovery;

    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvClockRecovery');
        hx.log("dsrvClockRecovery", "initializer service - hx.dl.dsrvClockRecovery");
        this.eventHandlers = [];
        this._init();
    }
    eventHandlers = [];
    /*********** External API Start ****************/

    startClockRecovery () {

        hx.log("dsrvClockRecovery", "startClockRecovery() called");

        this._init();
        this._oipfClockRecoverObj.startClockRecovery();
    }

    /*********** External API End ****************/

    /*********** Internal Function Start ****************/
    _onClockRecoveryEvent (aBatchType: any, aBatchProcType: any, aErr: any) {
        hx.log("dsrvClockRecovery", "_onClockRecoveryEvent() called");
        // TODO : Add check batch proc ..
        //if (batchProcType === 64)
        this.fireEvent('ClockRecoveryEvent', null);
    }

    _init () {
        hx.log("dsrvClockRecovery", "hx.dl.dsrvClockRecovery's _init() called");

        this._oipfClockRecoverObj.onClockRecovery = (batchType, batchProcType, err)=> {
            hx.log("error", hx.util.format("+++++ ### {0}.onClockRecovery ### +++++, batchType : {1}, batchProcType : {2}, err : {3}", 'dsrvClockRecovery', batchType, batchProcType, err));
            this._onClockRecoveryEvent(batchType, batchProcType, err);
        };
    }

    /*********** Internal Function End ****************/
}
export =  cDLClockRecovery;



