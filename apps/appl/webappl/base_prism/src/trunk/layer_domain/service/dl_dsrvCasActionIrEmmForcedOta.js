/// <reference path="../../def/sugar.d.ts" />
// file name : layer_domain/service/dl_dsrvCasActionIrEmmForcedOta.js
/*jslint sloppy:true, nomen:true, vars:true */
//    CAS_IR_EMM_FORCED_OTA: ['layer_domain/service/dl_dsrvCasActionIrEmmForcedOta.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLCasActionIrEmmForcedOta = (function (_super) {
        __extends(cDLCasActionIrEmmForcedOta, _super);
        function cDLCasActionIrEmmForcedOta(aServiceName) {
            _super.call(this, aServiceName);
            this.oipfCasUtilObj = hx.stbObject.casUtil;
            this.localSystemOipfObj = this.getPluginInstance('LocalSystem');
            this.casActionIrEmmForcedOtaOipfObj = this.localSystemOipfObj.casactionIrEmmForcedOta;
            this._init();
        }
        cDLCasActionIrEmmForcedOta.prototype.setListener = function () {
            this._init();
            this.casActionIrEmmForcedOtaOipfObj.setListener();
        };
        cDLCasActionIrEmmForcedOta.prototype.releaseListener = function () {
            this._init();
            this.casActionIrEmmForcedOtaOipfObj.releaseListener();
        };
        cDLCasActionIrEmmForcedOta.prototype._onCasActionIrEmmForcedOtaEvent = function () {
            this.fireEvent_Legacy('CasActionIrEmmForcedOtaEvent', null);
        };
        cDLCasActionIrEmmForcedOta.prototype._init = function () {
            hx.log("dsvc", "hx.dl.dsrvCasActionIrEmmForcedOta's init() called");
            this.casActionIrEmmForcedOtaOipfObj.onCasActionIrEmmForcedOta = function (param1, param2, param3) {
                this._onCasActionIrEmmForcedOtaEvent();
            };
        };
        return cDLCasActionIrEmmForcedOta;
    })(__KERNEL__);
    return cDLCasActionIrEmmForcedOta;
});
