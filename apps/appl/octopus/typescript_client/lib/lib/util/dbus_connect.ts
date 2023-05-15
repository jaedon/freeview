/**
 * Created by hsseo on 14. 5. 9.
 */

/// <reference path="../../def/node.d.ts" />

var newdbus = require('dbus');

var sAddress = 'tcp:host=10.0.12.150,port=55884';

export class CDBusInterface {
    private _dbusClass;
    private _dbusHandle;
    private _Destination;
    private _Path;
    private _eventTimer;
    constructor(aDestination: string, aPath: string) {
        var dbusClass = new newdbus();
        var dbusHandle = dbusClass.getBus('open', sAddress);

        this._dbusHandle = dbusHandle;
        this._dbusClass = dbusClass;
        this._Destination = aDestination;
        this._Path = aPath;
    }

    _call(aCb: Function) {
        this._dbusHandle.getInterface(this._Destination, this._Path, this._Destination, function (err, iface) {
            aCb(iface);
        });
    }

    _startEvent(aCb: Function) {
        this._dbusHandle.getInterface(this._Destination, this._Path, this._Destination, function (err, iface) {
            this._eventTimer = setTimeout(function() {aCb(iface);}, 1000);
        });
    }
}
