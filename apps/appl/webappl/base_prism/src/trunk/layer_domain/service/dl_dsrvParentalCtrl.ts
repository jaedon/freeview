/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvParentalCtrl.js
/* jslint sloppy:true, nomen:true, vars:true */
//    PARENTAL_CTRL: ['layer_domain/service/dl_dsrvParentalCtrl.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

class cDLParentalCtrl extends __KERNEL__ implements ifSVCParentalCtrl {
    private parentalRatingSchemes;
    private parentalRatingScheme;
    private ratingValue;
    private threshold;
    private _OIPFParentalControlManager = this.getPluginInstance("ParentalControlManager");
    TNaScPinState = {
        ECORRECT: 0,
        EINCORRECT_1st: 1,
        EINCORRECT_2nd: 2,
        EBLOCKED: 3
    };
    constructor(aServiceName: string) {
        super(aServiceName);
        hx.log("dsvc", "initializer service - hx.svc.PARENTAL_CTRL ");
        hx.logadd("dsrvParentalCtrl");
        hx.log("dsvc", "hx.svc.PARENTAL_CTRL 's init() called");
    }
    getParentalRatingValue(): any {
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
    }
    setParentalRatingValue(value): void {
        this.parentalRatingScheme.changeThreshold(value);
        hx.log("dsvc", "changed RatingValue : " + value);
    }
    verifyParentalControlPIN(params): any {
        var ret: any = {};
        //if verifying is OK, veryfyResult is 0.
        ret.result = this._OIPFParentalControlManager.verifyParentalControlPIN(params.password);

        if (params.cb !== undefined) {
            setTimeout(params.cb(ret), 0);
        }

        return ret;
    }
    setParentalControlPIN(data): any {
        var ret = this._OIPFParentalControlManager.setParentalControlPIN(data.oldPcPIN, data.newPcPIN);

        return ret;
    }
    setDefaultParentalControlPIN(data): any {
        var ret = this._OIPFParentalControlManager.resetParentalControlPIN(data);

        return ret;
    }
    setParentalControlStatus(data): any {
        //if verifying is OK, veryfyResult is 0.
        var ret = this._OIPFParentalControlManager.setParentalControlStatus(data.pcPIN, data.enable);

        return ret;
    }
    getParentalControlStatus(data): any {
        var ret = this._OIPFParentalControlManager.getParentalControlStatus();
        return ret;
    }
    compareTwoPassword(params): any {
        var ret;
        if (params.firstInputtedPassword === params.secondInputtedPassword) {
            this._OIPFParentalControlManager.setParentalControlPIN(params.settedPassword, params.firstInputtedPassword);
            ret = true;
        } else {
            ret = false;
        }
        if (params.cb !== undefined) {
            setTimeout(params.cb(ret), 0);
        }
        return ret;
    }
    setChannelsetupPin(value): void {
        var confObj = this.getPluginInstance("Configuration");

        hx.log("dsvc", "setChannelsetupPin : " + value);
        confObj.channelsetupPin = value;
    }
    getChannelsetupPin(): any {
        var confObj = this.getPluginInstance("Configuration");

        hx.log("dsvc", "getChannelsetupPin : " + confObj.channelsetupPin);

        return confObj.channelsetupPin;
    }
    setReSetParentalControlPIN(data): boolean {
        var ret = this._OIPFParentalControlManager.resetParentalControlPIN(data.newPcPIN);
        if (ret === 0) {
            return true;
        }
        return false;
    }
    isNagraSmartCardInserted(): boolean {
        var ret = this._OIPFParentalControlManager.isNagraSmartCardInserted();
        hx.log('dsrvParentalCtrl', hx.util.format('[isNagraSmartCardInserted] bSCInserted : {0}', ret));
        return ret;
    }
    setNargaSmartCardPIN(param): any {
        hx.log('dsrvParentalCtrl', hx.util.format('[setNargaSmartCardPIN] oldPin : {0} / newPin : {1}', param.oldPin, param.newPin));
        var ret = this._OIPFParentalControlManager.setNargaSmartCardPIN(param.oldPin, param.newPin);
        return ret;
    }
    getNargaSmartCardPINRetryCount(): any {
        var ret = this._OIPFParentalControlManager.getNargaSmartCardPINRetryCount();
        hx.log('dsrvParentalCtrl', hx.util.format('[getNargaSmartCardPINRetryCount] retryCnt : {0}', ret));
        return ret;
    }
    getNargaSmartCardPINRemainTime(): any{
        var ret = this._OIPFParentalControlManager.getNargaSmartCardPINRemainTime();
        hx.log('dsrvParentalCtrl', hx.util.format('[getNargaSmartCardPINRemainTime] remineTime : {0}', ret));
        return ret;
    }
    verifyNargaSmartCardPIN(param): any {
        hx.log('dsrvParentalCtrl', hx.util.format('[verifyNargaSmartCardPIN] inputPin : {0}', param));
        var ret = this._OIPFParentalControlManager.verifyNargaSmartCardPIN(param);
        hx.log('dsrvParentalCtrl', hx.util.format('[verifyNargaSmartCardPIN] retryCnt : {0}', ret));
        return ret;
    }
}
export = cDLParentalCtrl;
