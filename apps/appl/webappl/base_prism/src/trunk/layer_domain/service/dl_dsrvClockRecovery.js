/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvClockRecovery.js
//    CLOCK_RECOVERY: ['layer_domain/service/dl_dsrvClockRecovery.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLClockRecovery = (function (_super) {
        __extends(cDLClockRecovery, _super);
        function cDLClockRecovery(aServiceName) {
            _super.call(this, aServiceName);
            this._oipfLocalSystemObj = this.getPluginInstance('LocalSystem');
            this._oipfClockRecoverObj = this._oipfLocalSystemObj.clockRecovery;
            this.eventHandlers = [];
            hx.logadd('dsrvClockRecovery');
            hx.log("dsrvClockRecovery", "initializer service - hx.dl.dsrvClockRecovery");
            this.eventHandlers = [];
            this._init();
        }
        /*********** External API Start ****************/
        cDLClockRecovery.prototype.startClockRecovery = function () {
            hx.log("dsrvClockRecovery", "startClockRecovery() called");
            this._init();
            this._oipfClockRecoverObj.startClockRecovery();
        };
        /*********** External API End ****************/
        /*********** Internal Function Start ****************/
        cDLClockRecovery.prototype._onClockRecoveryEvent = function (aBatchType, aBatchProcType, aErr) {
            hx.log("dsrvClockRecovery", "_onClockRecoveryEvent() called");
            // TODO : Add check batch proc ..
            //if (batchProcType === 64)
            this.fireEvent('ClockRecoveryEvent', null);
        };
        cDLClockRecovery.prototype._init = function () {
            var _this = this;
            hx.log("dsrvClockRecovery", "hx.dl.dsrvClockRecovery's _init() called");
            this._oipfClockRecoverObj.onClockRecovery = function (batchType, batchProcType, err) {
                hx.log("error", hx.util.format("+++++ ### {0}.onClockRecovery ### +++++, batchType : {1}, batchProcType : {2}, err : {3}", 'dsrvClockRecovery', batchType, batchProcType, err));
                _this._onClockRecoveryEvent(batchType, batchProcType, err);
            };
        };
        return cDLClockRecovery;
    })(__KERNEL__);
    return cDLClockRecovery;
});
