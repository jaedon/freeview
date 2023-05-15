/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilDatastreamer extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Datastreamer', '/Octopus/Appkit/Util/Datastreamer');
    }

    StartEvent(aCb: (data: number[]) => void) {
        this._call( function (iface){
            iface.on('event', function(data: number[]) {
                aCb(data);
            });
        });
    }

    ConnectDataStreamer(aDataType: string, aCb: () => void) {
        this._call( function (iface){
            iface.ConnectDataStreamer['finish'] = function() {
                aCb();
            };
            iface.ConnectDataStreamer(aDataType);
        });
    }

    DisconnectDataStreamer(aDataType: string, aCb: () => void) {
        this._call( function (iface){
            iface.DisconnectDataStreamer['finish'] = function() {
                aCb();
            };
            iface.DisconnectDataStreamer(aDataType);
        });
    }
}

export = CUtilDatastreamer