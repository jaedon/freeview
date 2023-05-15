/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CCasNagra extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Cas.Nagra', '/Octopus/Appkit/Cas/Nagra');
    }

    StartEvent(aCb: (param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(param1: number, param2: number, param3: number) {
                aCb(param1, param2, param3);
            });
        });
    }

    IsCWE(aCb: (aCWE: number) => void) {
        this._call( function (iface){
            iface.IsCWE['finish'] = function(aCWE: number) {
                aCb(aCWE);
            };
            iface.IsCWE();
        });
    }

    IsJTAG(aCb: (aJTAG: number) => void) {
        this._call( function (iface){
            iface.IsJTAG['finish'] = function(aJTAG: number) {
                aCb(aJTAG);
            };
            iface.IsJTAG();
        });
    }

    IsSBOOT(aCb: (aSboot: number) => void) {
        this._call( function (iface){
            iface.IsSBOOT['finish'] = function(aSboot: number) {
                aCb(aSboot);
            };
            iface.IsSBOOT();
        });
    }

    IsOTP(aCb: (aSOTP: number) => void) {
        this._call( function (iface){
            iface.IsOTP['finish'] = function(aSOTP: number) {
                aCb(aSOTP);
            };
            iface.IsOTP();
        });
    }

    GetNUID(aCb: (aNuid: string) => void) {
        this._call( function (iface){
            iface.GetNUID['finish'] = function(aNuid: string) {
                aCb(aNuid);
            };
            iface.GetNUID();
        });
    }

    GetCASN(aCb: (aCasn: string) => void) {
        this._call( function (iface){
            iface.GetCASN['finish'] = function(aCasn: string) {
                aCb(aCasn);
            };
            iface.GetCASN();
        });
    }

    GetFusingInfo(aCb: (aScs: number, aJtag: number, aOtp: number, aCwe: number) => void) {
        this._call( function (iface){
            iface.GetFusingInfo['finish'] = function(data) {
                aCb(data[0], data[1], data[2], data[3]);
            };
            iface.GetFusingInfo();
        });
    }

    IsSCInserted(aCb: (aScinserted: number) => void) {
        this._call( function (iface){
            iface.IsSCInserted['finish'] = function(aScinserted: number) {
                aCb(aScinserted);
            };
            iface.IsSCInserted();
        });
    }

    VerifyPincode(aPinStr: string, aCb: (aIsOk: number) => void) {
        this._call( function (iface){
            iface.VerifyPincode['finish'] = function(aIsOk: number) {
                aCb(aIsOk);
            };
            iface.VerifyPincode(aPinStr);
        });
    }

    GetAccessibleState(aCb: (aAccessState: number, aRemainTime: number, aRetryCount: number, aIsOk: number) => void) {
        this._call( function (iface){
            iface.GetAccessibleState['finish'] = function(data) {
                aCb(data[0], data[1], data[2], data[3]);
            };
            iface.GetAccessibleState();
        });
    }

    VerifyPincodeGetAccessibleState(aPinStr: string, aCb: (aAccessState: number, aRemainTime: number, aRetryCount: number, aIsOk: number) => void) {
        this._call( function (iface){
            iface.VerifyPincodeGetAccessibleState['finish'] = function(data) {
                aCb(data[0], data[1], data[2], data[3]);
            };
            iface.VerifyPincodeGetAccessibleState(aPinStr);
        });
    }

    SetPincode(aOldPinStr: string, aNewPinStr: string, aCb: (aIsOk: number) => void) {
        this._call( function (iface){
            iface.SetPincode['finish'] = function(aIsOk: number) {
                aCb(aIsOk);
            };
            iface.SetPincode(aOldPinStr, aNewPinStr);
        });
    }
}

export = CCasNagra