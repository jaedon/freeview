/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilSwupdate extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Swupdate', '/Octopus/Appkit/Util/Swupdate');
    }

    StartEvent(aCb: (type: number, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(type: number, param1: number, param2: number, param3: number) {
                aCb(type, param1, param2, param3);
            });
        });
    }

    Create(aSwUpdateSpec: number, aCb: (aHandle: number) => void) {
        this._call( function (iface){
            iface.Create['finish'] = function(aHandle: number) {
                aCb(aHandle);
            };
            iface.Create(aSwUpdateSpec);
        });
    }

    CreateManualConnect(aSwUpdateSpec: number, aTunningInfo: tdata.TTransponderInfo ,aCb: (aHandle: number) => void) {
        this._call( function (iface){
            iface.CreateManualConnect['finish'] = function(aHandle: number) {
                aCb(aHandle);
            };
            iface.CreateManualConnect(aSwUpdateSpec, aTunningInfo);
        });
    }

    StartDetect(aHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.StartDetect['finish'] = function() {
                aCb();
            };
            iface.StartDetect(aHandle);
        });
    }

    StopDetect(aHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.StopDetect['finish'] = function() {
                aCb();
            };
            iface.StopDetect(aHandle);
        });
    }

    StartDownload(aHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.StartDownload['finish'] = function() {
                aCb();
            };
            iface.StartDownload(aHandle);
        });
    }

    StopDownload(aHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.StopDownload['finish'] = function() {
                aCb();
            };
            iface.StopDownload(aHandle);
        });
    }

    StartInstall(aHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.StartInstall['finish'] = function() {
                aCb();
            };
            iface.StartInstall(aHandle);
        });
    }

    StopInstall(aHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.StopInstall['finish'] = function() {
                aCb();
            };
            iface.StopInstall(aHandle);
        });
    }

    Cancel(aHandle: number, aCb: () => void) {
        this._call( function (iface){
            iface.Cancel['finish'] = function() {
                aCb();
            };
            iface.Cancel(aHandle);
        });
    }
}

export = CUtilSwupdate