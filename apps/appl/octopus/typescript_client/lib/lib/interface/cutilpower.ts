/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilPower extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Power', '/Octopus/Appkit/Util/Power');
    }

    StartEvent(aCb: (param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(param1: number, param2: number, param3: number) {
                aCb(param1, param2, param3);
            });
        });
    }

    GetWakeupReason(aCb: (aWakeupReason: number) => void) {
        this._call( function (iface){
            iface.GetWakeupReason['finish'] = function(aWakeupReason: number) {
                aCb(aWakeupReason);
            };
            iface.GetWakeupReason();
        });
    }

    GetWakeupSecondReason(aCb: (aWakeupSecondReason: number) => void) {
        this._call( function (iface){
            iface.GetWakeupSecondReason['finish'] = function(aWakeupSecondReason: number) {
                aCb(aWakeupSecondReason);
            };
            iface.GetWakeupSecondReason();
        });
    }

    SetStandbyPowerMode(aPowerMode: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetStandbyPowerMode['finish'] = function() {
                aCb();
            };
            iface.SetStandbyPowerMode(aPowerMode);
        });
    }

    GetStandbyPowerMode(aCb: (aPowerMode: number) => void) {
        this._call( function (iface){
            iface.GetStandbyPowerMode['finish'] = function(aPowerMode: number) {
                aCb(aPowerMode);
            };
            iface.GetStandbyPowerMode();
        });
    }

    IsStandbyMode(aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.IsStandbyMode['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.IsStandbyMode();
        });
    }

    GoingToReboot(aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.GoingToReboot['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.GoingToReboot();
        });
    }

    GoingToShutdown(aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.GoingToShutdown['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.GoingToShutdown();
        });
    }

    GoingToOperate(aCb: (aReval: number) => void) {
        this._call( function (iface){
            iface.GoingToOperate['finish'] = function(aReval: number) {
                aCb(aReval);
            };
            iface.GoingToOperate();
        });
    }

    RequestShutDown(aCb: () => void) {
        this._call( function (iface){
            iface.RequestShutDown['finish'] = function() {
                aCb();
            };
            iface.RequestShutDown();
        });
    }

    RequestReboot(aCb: () => void) {
        this._call( function (iface){
            iface.RequestReboot['finish'] = function() {
                aCb();
            };
            iface.RequestReboot();
        });
    }

    RequestOperation(aCb: () => void) {
        this._call( function (iface){
            iface.RequestOperation['finish'] = function() {
                aCb();
            };
            iface.RequestOperation();
        });
    }

    WaitShutDown(aWaitTime: number, aCb: () => void) {
        this._call( function (iface){
            iface.WaitShutDown['finish'] = function() {
                aCb();
            };
            iface.WaitShutDown(aWaitTime);
        });
    }
}

export = CUtilPower