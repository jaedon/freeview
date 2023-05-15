/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CScenarioPincode extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Scenario.Pincode', '/Octopus/Appkit/Scenario/Pincode');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    Check(aPinString: string, aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.Check['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.Check(aPinString);
        });
    }

    Verify(aPinString: string, aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.Verify['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.Verify(aPinString);
        });
    }

    Set(aOldPinString: string, aNewPinString: string, aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.Set['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.Set(aOldPinString, aNewPinString);
        });
    }

    Reset(aNewPinString: string, aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.Reset['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.Reset(aNewPinString);
        });
    }

    SetTemporaryBlock(aPinString: string, aBlocked: number, aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.SetTemporaryBlock['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.SetTemporaryBlock(aPinString, aBlocked);
        });
    }

    GetTemporaryBlock(aCb: (aBlocked: number) => void) {
        this._call( function (iface){
            iface.GetTemporaryBlock['finish'] = function(aBlocked: number) {
                aCb(aBlocked);
            };
            iface.GetTemporaryBlock();
        });
    }
}

export = CScenarioPincode