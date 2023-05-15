/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilExtdev extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Extdev', '/Octopus/Appkit/Util/Extdev');
    }

    StartEvent(aCb: (type: number, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(type: number, param1: number, param2: number, param3: number) {
                aCb(type, param1, param2, param3);
            });
        });
    }

    MIC_Start(aCb: () => void) {
        this._call( function (iface){
            iface.MIC_Start['finish'] = function() {
                aCb();
            };
            iface.MIC_Start();
        });
    }

    MIC_Stop(aCb: () => void) {
        this._call( function (iface){
            iface.MIC_Stop['finish'] = function() {
                aCb();
            };
            iface.MIC_Stop();
        });
    }

    MIC_SetVolume(aValue: number, aCb: () => void) {
        this._call( function (iface){
            iface.MIC_SetVolume['finish'] = function() {
                aCb();
            };
            iface.MIC_SetVolume(aValue);
        });
    }

    MIC_GetVolume(aCb: (aValue: number) => void) {
        this._call( function (iface){
            iface.MIC_GetVolume['finish'] = function(aValue: number) {
                aCb(aValue);
            };
            iface.MIC_GetVolume();
        });
    }

    MIC_GetCount(aCb: (aValue: number) => void) {
        this._call( function (iface){
            iface.MIC_GetCount['finish'] = function(aValue: number) {
                aCb(aValue);
            };
            iface.MIC_GetCount();
        });
    }
}

export = CUtilExtdev