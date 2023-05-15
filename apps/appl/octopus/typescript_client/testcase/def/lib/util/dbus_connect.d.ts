/// <reference path="../../../def/node.d.ts" />
export declare class CDBusInterface {
    private _dbusClass;
    private _dbusHandle;
    private _Destination;
    private _Path;
    private _eventTimer;
    constructor(aDestination: string, aPath: string);
    public _call(aCb: Function): void;
    public _startEvent(aCb: Function): void;
}

