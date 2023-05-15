/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CSysinfoTuner extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Sysinfo.Tuner', '/Octopus/Appkit/Sysinfo/Tuner');
    }

    GetDeviceNum(aCb: (aDeviceNumber: number) => void) {
        this._call( function (iface){
            iface.GetDeviceNum['finish'] = function(aDeviceNumber: number) {
                aCb(aDeviceNumber);
            };
            iface.GetDeviceNum();
        });
    }

    GetDevType(aIndex: number, aCb: (aDeliveryType: number) => void) {
        this._call( function (iface){
            iface.GetDevType['finish'] = function(aDeliveryType: number) {
                aCb(aDeliveryType);
            };
            iface.GetDevType(aIndex);
        });
    }

    IsLocked(aIndex: number, aCb: (aLocked: number) => void) {
        this._call( function (iface){
            iface.IsLocked['finish'] = function(aLocked: number) {
                aCb(aLocked);
            };
            iface.IsLocked(aIndex);
        });
    }

    GetSignalLevel(aIndex: number, aCb: (aSignalLevel: number) => void) {
        this._call( function (iface){
            iface.GetSignalLevel['finish'] = function(aSignalLevel: number) {
                aCb(aSignalLevel);
            };
            iface.GetSignalLevel(aIndex);
        });
    }

    GetSignalQuality(aIndex: number, aCb: (aSignalQuality: number) => void) {
        this._call( function (iface){
            iface.GetSignalQuality['finish'] = function(aSignalQuality: number) {
                aCb(aSignalQuality);
            };
            iface.GetSignalQuality(aIndex);
        });
    }

    GetAgc(aIndex: number, aCb: (aAgc: number) => void) {
        this._call( function (iface){
            iface.GetAgc['finish'] = function(aAgc: number) {
                aCb(aAgc);
            };
            iface.GetAgc(aIndex);
        });
    }

    GetBer(aIndex: number, aCb: (aBer: number) => void) {
        this._call( function (iface){
            iface.GetBer['finish'] = function(aBer: number) {
                aCb(aBer);
            };
            iface.GetBer(aIndex);
        });
    }

    GetSnr(aIndex: number, aCb: (aSnr: number) => void) {
        this._call( function (iface){
            iface.GetSnr['finish'] = function(aSnr: number) {
                aCb(aSnr);
            };
            iface.GetSnr(aIndex);
        });
    }

    GetInputPower(aIndex: number, aCb: (aInputPower: number) => void) {
        this._call( function (iface){
            iface.GetInputPower['finish'] = function(aInputPower: number) {
                aCb(aInputPower);
            };
            iface.GetInputPower(aIndex);
        });
    }

    GetTuneParam(aIndex: number, aCb: (aTunerParam: number) => void) {
        this._call( function (iface){
            iface.GetTuneParam['finish'] = function(aTunerParam: number) {
                aCb(aTunerParam);
            };
            iface.GetTuneParam(aIndex);
        });
    }

    GetTunerIdByTripleId(aOnId: number, aTsId: number, aSvcId: number, aCb: (aTunerId: number) => void) {
        this._call( function (iface){
            iface.GetTunerIdByTripleId['finish'] = function(aTunerId: number) {
                aCb(aTunerId);
            };
            iface.GetTunerIdByTripleId(aOnId, aTsId, aSvcId);
        });
    }
}

export = CSysinfoTuner