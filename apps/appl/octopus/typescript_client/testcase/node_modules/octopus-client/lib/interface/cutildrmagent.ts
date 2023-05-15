/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CUtilDrmagent extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Util.Drmagent', '/Octopus/Appkit/Util/Drmagent');
    }

    StartEvent(aCb: (msgStr: string, msgInt: number, resultCode: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgStr: string, msgInt: number, resultCode: number) {
                aCb(msgStr, msgInt, resultCode);
            });
        });
    }

    SendDrmMessage(aMsgType: string, aMessage: string, aDrmSystemId: string, aMsgId: number, aCb:() => void) {
        this._call(function (iface) {
            iface.SendDrmMessage['finish'] = function () {
                aCb();
            };
            iface.SendDrmMessage(aMsgType, aMessage, aDrmSystemId, aMsgId);
        });
    }

    SendDrmSystemStatusRequest(aDrmSystemId: string, aCb:() => void) {
        this._call(function (iface) {
            iface.SendDrmSystemStatusRequest['finish'] = function () {
                aCb();
            };
            iface.SendDrmSystemStatusRequest(aDrmSystemId);
        });
    }
}

export = CUtilDrmagent