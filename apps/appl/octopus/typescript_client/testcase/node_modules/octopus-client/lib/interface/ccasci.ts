/**
 * Created by hsseo on 14. 5. 12.
 */



import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CCasCi extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Cas.Ci', '/Octopus/Appkit/Cas/Ci');
    }

    StartEvent(aCb: (param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(param1: number, param2: number, param3: number) {
                aCb(param1, param2, param3);
            });
        });
    }

    StartMMIEvent(aCb: (msgType: number, userData: number[]) => void) {
        this._call( function (iface){
            iface.on('mmi_event', function(msgType: number, userData: number[]) {
                aCb(msgType, userData);
            });
        });
    }

    IsSupportedCI(aCb: (aSupported: number) => void) {
        this._call( function (iface){
            iface.IsSupportedCI['finish'] = function(aSupported: number) {
                aCb(aSupported);
            };
            iface.IsSupportedCI();
        });
    }

    IsSupportedCIPlus(aCb: (aSupported: number) => void) {
        this._call( function (iface){
            iface.IsSupportedCIPlus['finish'] = function(aSupported: number) {
                aCb(aSupported);
            };
            iface.IsSupportedCIPlus();
        });
    }

    GetCIPlusVersionName(aCb: (aVersionName: string) => void) {
        this._call( function (iface){
            iface.GetCIPlusVersionName['finish'] = function(aVersionName: string) {
                aCb(aVersionName);
            };
            iface.GetCIPlusVersionName();
        });
    }

    GetSlotCount(aCb: (aSlotCount: number) => void) {
        this._call( function (iface){
            iface.GetSlotCount['finish'] = function(aSlotCount: number) {
                aCb(aSlotCount);
            };
            iface.GetSlotCount();
        });
    }

    GetCamNumberBySlot(aSlot: number, aCb: (aCamNumber: number) => void) {
        this._call( function (iface){
            iface.GetCamNumberBySlot['finish'] = function(aCamNumber: number) {
                aCb(aCamNumber);
            };
            iface.GetCamNumberBySlot(aSlot);
        });
    }

    GetCamStatusByIndex(aSlot: number, aCb: (aCamStatus: number) => void) {
        this._call( function (iface){
            iface.GetCamStatusByIndex['finish'] = function(aCamStatus: number) {
                aCb(aCamStatus);
            };
            iface.GetCamStatusByIndex(aSlot);
        });
    }

    GetCamNameByIndex(aSlot: number, aCb: (aCamName: string) => void) {
        this._call( function (iface){
            iface.GetCamNameByIndex['finish'] = function(aCamName: string) {
                aCb(aCamName);
            };
            iface.GetCamNameByIndex(aSlot);
        });
    }

    RequestFileorData(aSessionId: number, aReqType: number, aReqName: string, aCb: () => void) {
        this._call( function (iface){
            iface.RequestFileorData['finish'] = function() {
                aCb();
            };
            iface.RequestFileorData(aSessionId, aReqType, aReqName);
        });
    }

    RequestAppAbort(aSessionId: number, aAbortReqCode: number, aCb: () => void) {
        this._call( function (iface){
            iface.RequestAppAbort['finish'] = function() {
                aCb();
            };
            iface.RequestAppAbort(aSessionId, aAbortReqCode);
        });
    }
}

export = CCasCi