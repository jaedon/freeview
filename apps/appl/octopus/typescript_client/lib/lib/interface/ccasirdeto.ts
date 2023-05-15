/**
 * Created by hsseo on 14. 5. 12.
 */



import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CCasIrdeto extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Cas.Irdeto', '/Octopus/Appkit/Cas/Irdeto');
    }

    StartEvent(aCb: (param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(param1: number, param2: number, param3: number) {
                aCb(param1, param2, param3);
            });
        });
    }

    SetVirtualSecureCwMode(aVirtualSecureCwMode: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetVirtualSecureCwMode['finish'] = function() {
                aCb();
            };
            iface.SetVirtualSecureCwMode(aVirtualSecureCwMode);
        });
    }

    GetVirtualSecureCwMode(aCb: (aVirtualSecureCwMode: number) => void) {
        this._call( function (iface){
            iface.GetVirtualSecureCwMode['finish'] = function(aVirtualSecureCwMode: number) {
                aCb(aVirtualSecureCwMode);
            };
            iface.GetVirtualSecureCwMode();
        });
    }

    GetIrdetoClientVersionName(aCb: (aVersionName: string) => void) {
        this._call( function (iface){
            iface.GetIrdetoClientVersionName['finish'] = function(aVersionName: string) {
                aCb(aVersionName);
            };
            iface.GetIrdetoClientVersionName();
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

    GetIrdetoManCode(aCb: (aManCode: string) => void) {
        this._call( function (iface){
            iface.GetIrdetoManCode['finish'] = function(aManCode: string) {
                aCb(aManCode);
            };
            iface.GetIrdetoManCode();
        });
    }

    GetIrdetoHwCode(aCb: (aHwcode: string) => void) {
        this._call( function (iface){
            iface.GetIrdetoHwCode['finish'] = function(aHwcode: string) {
                aCb(aHwcode);
            };
            iface.GetIrdetoHwCode();
        });
    }

    GetIrdetoVariant(aCb: (aVariant: string) => void) {
        this._call( function (iface){
            iface.GetIrdetoVariant['finish'] = function(aVariant: string) {
                aCb(aVariant);
            };
            iface.GetIrdetoVariant();
        });
    }

    GetIrdetoSysId(aCb: (aSysId: string) => void) {
        this._call( function (iface){
            iface.GetIrdetoSysId['finish'] = function(aSysId: string) {
                aCb(aSysId);
            };
            iface.GetIrdetoSysId();
        });
    }

    GetIrdetoKeyVer(aCb: (aKeyVer: string) => void) {
        this._call( function (iface){
            iface.GetIrdetoKeyVer['finish'] = function(aKeyVer: string) {
                aCb(aKeyVer);
            };
            iface.GetIrdetoKeyVer();
        });
    }

    GetIrdetoSigVer(aCb: (aSigVer: string) => void) {
        this._call( function (iface){
            iface.GetIrdetoSigVer['finish'] = function(aSigVer: string) {
                aCb(aSigVer);
            };
            iface.GetIrdetoSigVer();
        });
    }

    GetControlByte(aCb: (aControlByte: number) => void) {
        this._call( function (iface){
            iface.GetControlByte['finish'] = function(aControlByte: number) {
                aCb(aControlByte);
            };
            iface.GetControlByte();
        });
    }
}

export = CCasIrdeto