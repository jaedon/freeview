/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CMediaAirplay extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Media.Airplay', '/Octopus/Appkit/Media/Airplay');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number) {
                aCb(msgInt, msgStr, param1, param2);
            });
        });
    }

    Start(aCb: () => void) {
        this._call( function (iface){
            iface.Start['finish'] = function() {
                aCb();
            };
            iface.Start();
        });
    }

    Stop(aCb: () => void) {
        this._call( function (iface){
            iface.Stop['finish'] = function() {
                aCb();
            };
            iface.Stop();
        });
    }

    SetPlaybackInfo(aPlaybackStatus: number, aDuration: number, aStartPosition: number, aBufferedRange: number, aSeekableRange: number, aRate: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPlaybackInfo['finish'] = function() {
                aCb();
            };
            iface.SetPlaybackInfo((aPlaybackStatus, aDuration, aStartPosition, aBufferedRange, aSeekableRange, aRate));
        });
    }
}

export = CMediaAirplay