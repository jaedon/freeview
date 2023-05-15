/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CFilemgrSambaclient extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Filemgr.Sambaclient', '/Octopus/Appkit/Filemgr/Sambaclient');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    StartScan(aCb: () => void) {
        this._call( function (iface){
            iface.StartScan['finish'] = function() {
                aCb();
            };
            iface.StartScan();
        });
    }

    StopScan(aCb: () => void) {
        this._call( function (iface){
            iface.StopScan['finish'] = function() {
                aCb();
            };
            iface.StopScan();
        });
    }

    Mount(aSharedFolderInfo: number, aCb: () => void) {
        this._call( function (iface){
            iface.Mount['finish'] = function() {
                aCb();
            };
            iface.Mount(aSharedFolderInfo);
        });
    }

    UnMount(aSharedFolderInfo: number, aCb: () => void) {
        this._call( function (iface){
            iface.UnMount['finish'] = function() {
                aCb();
            };
            iface.UnMount(aSharedFolderInfo);
        });
    }

    UnmountAll(aCb: () => void) {
        this._call( function (iface){
            iface.UnmountAll['finish'] = function() {
                aCb();
            };
            iface.UnmountAll();
        });
    }
}

export = CFilemgrSambaclient