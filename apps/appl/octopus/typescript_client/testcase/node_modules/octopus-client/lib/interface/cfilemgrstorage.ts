/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CFilemgrStorage extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Filemgr.Storage', '/Octopus/Appkit/Filemgr/Storage');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    Format(aDeviceId: number, aVolume: string, aCb: () => void) {
        this._call( function (iface){
            iface.Format['finish'] = function() {
                aCb();
            };
            iface.Format(aDeviceId, aVolume);
        });
    }

    StartDST(aDeviceId: number, aCb: () => void) {
        this._call( function (iface){
            iface.StartDST['finish'] = function() {
                aCb();
            };
            iface.StartDST(aDeviceId);
        });
    }

    SetDefaultHdd(aStorageId: string, aCb: () => void) {
        this._call( function (iface){
            iface.SetDefaultHdd['finish'] = function() {
                aCb();
            };
            iface.SetDefaultHdd(aStorageId);
        });
    }

    DisablePVRHdd(aStorageId: string, aCb: () => void) {
        this._call( function (iface){
            iface.DisablePVRHdd['finish'] = function() {
                aCb();
            };
            iface.DisablePVRHdd(aStorageId);
        });
    }

    PairingHdd(aStorageId: string, aCb: () => void) {
        this._call( function (iface){
            iface.PairingHdd['finish'] = function() {
                aCb();
            };
            iface.PairingHdd(aStorageId);
        });
    }

    UnPairingHdd(aStorageId: string, aCb: () => void) {
        this._call( function (iface){
            iface.UnPairingHdd['finish'] = function() {
                aCb();
            };
            iface.UnPairingHdd(aStorageId);
        });
    }

    SetName(aStorageId: string, aName: string, aCb: () => void) {
        this._call( function (iface){
            iface.SetName['finish'] = function() {
                aCb();
            };
            iface.SetName(aStorageId, aName);
        });
    }

    SafeRemove(aDeviceId: number, aForced: number, aCb: () => void) {
        this._call( function (iface){
            iface.SafeRemove['finish'] = function() {
                aCb();
            };
            iface.SafeRemove(aDeviceId, aForced);
        });
    }

    GetList(aFilter: number, aCb: (aInfoList: number[]) => void) {
        this._call( function (iface){
            iface.GetList['finish'] = function(data) {
                var infoList = [];
                data.forEach((g) => {
                    infoList.push(g);
                });
                aCb(infoList);
            };
            iface.GetList(aFilter);
        });
    }

    GetSize(aDeviceId: number, aCb: (aTotalKb: number, aAvailableKb: number, aUsedKb: number, aReservedKb: number, aAvailablePvrKb: number) => void) {
        this._call( function (iface){
            iface.GetSize['finish'] = function(data) {
                aCb(data[0], data[1], data[2], data[3], data[4]);
            };
            iface.GetSize(aDeviceId);
        });
    }

    GetPvrReservedSize(aDeviceId: number, aCb: (aReservedKb: number) => void) {
        this._call( function (iface){
            iface.GetPvrReservedSize['finish'] = function(aReservedKb: number) {
                aCb(aReservedKb);
            };
            iface.GetPvrReservedSize(aDeviceId);
        });
    }

    CheckUsbSpeed(aStorageId: string, aCb: () => void) {
        this._call( function (iface){
            iface.CheckUsbSpeed['finish'] = function() {
                aCb();
            };
            iface.CheckUsbSpeed(aStorageId);
        });
    }

    GetDevTypeString(aDeviceType: number, aCb: (aTypeStr: string) => void) {
        this._call( function (iface){
            iface.GetDevTypeString['finish'] = function(aTypeStr: string) {
                aCb(aTypeStr);
            };
            iface.GetDevTypeString(aDeviceType);
        });
    }

    GetFsString(aFsType: number, aCb: (aTypeStr: string) => void) {
        this._call( function (iface){
            iface.GetFsString['finish'] = function(aTypeStr: string) {
                aCb(aTypeStr);
            };
            iface.GetFsString(aFsType);
        });
    }
}

export = CFilemgrStorage