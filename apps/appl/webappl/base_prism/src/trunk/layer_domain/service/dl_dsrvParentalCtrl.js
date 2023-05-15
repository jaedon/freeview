/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvParentalCtrl.js
/* jslint sloppy:true, nomen:true, vars:true */
//    PARENTAL_CTRL: ['layer_domain/service/dl_dsrvParentalCtrl.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLParentalCtrl = (function (_super) {
        __extends(cDLParentalCtrl, _super);
        function cDLParentalCtrl(aServiceName) {
            _super.call(this, aServiceName);
            this._OIPFParentalControlManager = this.getPluginInstance("ParentalControlManager");
            this.TNaScPinState = {
                ECORRECT: 0,
                EINCORRECT_1st: 1,
                EINCORRECT_2nd: 2,
                EBLOCKED: 3
            };
            hx.log("dsvc", "initializer service - hx.svc.PARENTAL_CTRL ");
            hx.logadd("dsrvParentalCtrl");
            hx.log("dsvc", "hx.svc.PARENTAL_CTRL 's init() called");
        }
        cDLParentalCtrl.prototype.getParentalRatingValue = function () {
            var RatingValue;
            this.parentalRatingSchemes = this._OIPFParentalControlManager.parentalRatingSchemes;
            this.parentalRatingScheme = this.parentalRatingSchemes.getParentalRatingScheme("dvb-si");
            if (this.parentalRatingScheme === null) {
                return 2;
            }
            hx.log("dsvc", "parentalRatingScheme.name : " + this.parentalRatingScheme.name);
            this.threshold = this.parentalRatingScheme.threshold;
            RatingValue = this.threshold.value;
            hx.log("dsvc", "RatingValue : " + RatingValue);
            this.ratingValue = RatingValue;
            return RatingValue;
        };
        cDLParentalCtrl.prototype.setParentalRatingValue = function (value) {
            this.parentalRatingScheme.changeThreshold(value);
            hx.log("dsvc", "changed RatingValue : " + value);
        };
        cDLParentalCtrl.prototype.verifyParentalControlPIN = function (params) {
            var ret = {};
            //if verifying is OK, veryfyResult is 0.
            ret.result = this._OIPFParentalControlManager.verifyParentalControlPIN(params.password);
            if (params.cb !== undefined) {
                setTimeout(params.cb(ret), 0);
            }
            return ret;
        };
        cDLParentalCtrl.prototype.setParentalControlPIN = function (data) {
            var ret = this._OIPFParentalControlManager.setParentalControlPIN(data.oldPcPIN, data.newPcPIN);
            return ret;
        };
        cDLParentalCtrl.prototype.setDefaultParentalControlPIN = function (data) {
            var ret = this._OIPFParentalControlManager.resetParentalControlPIN(data);
            return ret;
        };
        cDLParentalCtrl.prototype.setParentalControlStatus = function (data) {
            //if verifying is OK, veryfyResult is 0.
            var ret = this._OIPFParentalControlManager.setParentalControlStatus(data.pcPIN, data.enable);
            return ret;
        };
        cDLParentalCtrl.prototype.getParentalControlStatus = function (data) {
            var ret = this._OIPFParentalControlManager.getParentalControlStatus();
            return ret;
        };
        cDLParentalCtrl.prototype.compareTwoPassword = function (params) {
            var ret;
            if (params.firstInputtedPassword === params.secondInputtedPassword) {
                this._OIPFParentalControlManager.setParentalControlPIN(params.settedPassword, params.firstInputtedPassword);
                ret = true;
            }
            else {
                ret = false;
            }
            if (params.cb !== undefined) {
                setTimeout(params.cb(ret), 0);
            }
            return ret;
        };
        cDLParentalCtrl.prototype.setChannelsetupPin = function (value) {
            var confObj = this.getPluginInstance("Configuration");
            hx.log("dsvc", "setChannelsetupPin : " + value);
            confObj.channelsetupPin = value;
        };
        cDLParentalCtrl.prototype.getChannelsetupPin = function () {
            var confObj = this.getPluginInstance("Configuration");
            hx.log("dsvc", "getChannelsetupPin : " + confObj.channelsetupPin);
            return confObj.channelsetupPin;
        };
        cDLParentalCtrl.prototype.setReSetParentalControlPIN = function (data) {
            var ret = this._OIPFParentalControlManager.resetParentalControlPIN(data.newPcPIN);
            if (ret === 0) {
                return true;
            }
            return false;
        };
        cDLParentalCtrl.prototype.isNagraSmartCardInserted = function () {
            var ret = this._OIPFParentalControlManager.isNagraSmartCardInserted();
            hx.log('dsrvParentalCtrl', hx.util.format('[isNagraSmartCardInserted] bSCInserted : {0}', ret));
            return ret;
        };
        cDLParentalCtrl.prototype.setNargaSmartCardPIN = function (param) {
            hx.log('dsrvParentalCtrl', hx.util.format('[setNargaSmartCardPIN] oldPin : {0} / newPin : {1}', param.oldPin, param.newPin));
            var ret = this._OIPFParentalControlManager.setNargaSmartCardPIN(param.oldPin, param.newPin);
            return ret;
        };
        cDLParentalCtrl.prototype.getNargaSmartCardPINRetryCount = function () {
            var ret = this._OIPFParentalControlManager.getNargaSmartCardPINRetryCount();
            hx.log('dsrvParentalCtrl', hx.util.format('[getNargaSmartCardPINRetryCount] retryCnt : {0}', ret));
            return ret;
        };
        cDLParentalCtrl.prototype.getNargaSmartCardPINRemainTime = function () {
            var ret = this._OIPFParentalControlManager.getNargaSmartCardPINRemainTime();
            hx.log('dsrvParentalCtrl', hx.util.format('[getNargaSmartCardPINRemainTime] remineTime : {0}', ret));
            return ret;
        };
        cDLParentalCtrl.prototype.verifyNargaSmartCardPIN = function (param) {
            hx.log('dsrvParentalCtrl', hx.util.format('[verifyNargaSmartCardPIN] inputPin : {0}', param));
            var ret = this._OIPFParentalControlManager.verifyNargaSmartCardPIN(param);
            hx.log('dsrvParentalCtrl', hx.util.format('[verifyNargaSmartCardPIN] retryCnt : {0}', ret));
            return ret;
        };
        return cDLParentalCtrl;
    })(__KERNEL__);
    return cDLParentalCtrl;
});
