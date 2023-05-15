/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CFilemgrBrowse extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Filemgr.Browse', '/Octopus/Appkit/Filemgr/Browse');
    }

    StartEvent(aCb: (handleId: number, event: number, result: number) => void) {
        this._call( function (iface){
            iface.on('event', function(handleId: number, event: number, result: number) {
                aCb(handleId, event, result);
            });
        });
    }

    Open(aPath: string, aCategory: number, aExtFilter: string, aOptFlags: number, aCb: (aHandle: number) => void) {
        this._call( function (iface){
            iface.Open['finish'] = function(aHandle: number) {
                aCb(aHandle);
            };
            iface.Open(aPath, aCategory, aExtFilter, aOptFlags);
        });
    }

    Close(aHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.Close['finish'] = function() {
                aCb();
            };
            iface.Close(aHandle);
        });
    }

    GetItems(aHandle: number, aIndex: number, aReqCount: number, aOption: number, aCb: () => void) {
        this._call( function (iface){
            iface.GetItems['finish'] = function() {
                aCb();
            };
            iface.GetItems(aHandle, aIndex, aReqCount, aOption);
        });
    }

    ScanItems(aHandle: number, aReqCount: number, aOption: number, aCb: () => void) {
        this._call( function (iface){
            iface.ScanItems['finish'] = function() {
                aCb();
            };
            iface.ScanItems(aHandle, aReqCount, aOption);
        });
    }

    Sort(aHandle: number, aSort: number, aCb: () => void) {
        this._call( function (iface){
            iface.Sort['finish'] = function() {
                aCb();
            };
            iface.Sort(aHandle, aSort);
        });
    }
}

export = CFilemgrBrowse