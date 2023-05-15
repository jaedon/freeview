/// <reference path="../../def/sugar.d.ts" />
// file name : layer_domain/service/dl_dsrvCasActionIrEmmForcedOta.js
/*jslint sloppy:true, nomen:true, vars:true */
//    CAS_IR_EMM_FORCED_OTA: ['layer_domain/service/dl_dsrvCasActionIrEmmForcedOta.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

class cDLCasActionIrEmmForcedOta extends __KERNEL__ implements ifSVCCasActionIrEmmForcedOta {
    private oipfCasUtilObj = hx.stbObject.casUtil;
    private localSystemOipfObj:any = this.getPluginInstance('LocalSystem');
    private casActionIrEmmForcedOtaOipfObj:any = this.localSystemOipfObj.casactionIrEmmForcedOta;

    constructor(aServiceName: string) {
        super(aServiceName);
        this._init();
    }

    setListener(): void {
        this._init();
        this.casActionIrEmmForcedOtaOipfObj.setListener();
    }

    releaseListener(): void {
        this._init();
        this.casActionIrEmmForcedOtaOipfObj.releaseListener();
    }

    private _onCasActionIrEmmForcedOtaEvent(): void {
        this.fireEvent_Legacy('CasActionIrEmmForcedOtaEvent', null);
    }

    private _init(): void {
        hx.log("dsvc", "hx.dl.dsrvCasActionIrEmmForcedOta's init() called");

        this.casActionIrEmmForcedOtaOipfObj.onCasActionIrEmmForcedOta = function(param1, param2, param3) {
            this._onCasActionIrEmmForcedOtaEvent();
        };
    }
}
export = cDLCasActionIrEmmForcedOta;
