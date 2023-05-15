/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvSamba.js
/*jslint nomen:true, vars:true */
//    SAMBA: ['layer_domain/service/dl_dsrvSamba.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");

var ESAMBA_SCAN_STARTED = 0;
var ESAMBA_SCAN_ADDED = 1;
var ESAMBA_SCAN_REMOVED = 2;
var ESAMBA_SCAN_COMPLETE = 3;
var ESAMBA_SCAN_FAILED = 4;

var EMOUNT_SUCCEEDED = 10;
var EMOUNT_FAILED = 11;
var EUNMOUNT_SUCCEEDED = 12;
var EUNMOUNT_FAILED = 13;

class cSambaServer extends __MODEL__.CNetworkServer {
    private _pluginObject;
    constructor(pluginObject) {
        super();
        hx.log('dsrvSamba', 'new CSambaServer()');
        this._pluginObject = pluginObject;
    }
    getPluginObject() {
        return this._pluginObject;
    }
    getId() {
        return this.getName() + this.getSharedName();
    }
    getName() {
        return this._pluginObject.serverName;
    }
    getSharedName() {
        return this._pluginObject.sharedName;
    }
    getIpAddress() {
        return this._pluginObject.ip;
    }
    getStorageType() {
        return __MODEL__.IMediaServer.TStorageType.ESamba;
    }
    getStorageCssClass() {
        return __MODEL__.IMediaServer.StorageCssClassList[__MODEL__.IMediaServer.TStorageType.ESamba];
    }
    getViewType() {
        return __MODEL__.IMediaServer.TViewType.EFolderView;
    }
    getPath() {
        return this._pluginObject.mountPath;
    }
    getLocationType() {
        return __MODEL__.IMediaServer.TLocationType.ENetwork;
    }
    getUserId() {
        return this._pluginObject.id;
    }
    getPassword() {
        return this._pluginObject.password;
    }
    setId(id) {
        this._pluginObject.id = id;
    }
    setPassword(password) {
        this._pluginObject.password = password;
    }
    isMounted() {
        return this._pluginObject.isMounted;
    }
}

class cDLSamba extends __KERNEL__ implements ifSVCSamba {
    private mSamba = this.getPluginInstance('Samba');
    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvSamba');
        hx.log('dsrvSamba', 'create');
        this.defineEvents(['SambaServerAdded', 'SambaServerRemoved', 'MountSucceeded', 'MountFailed', 'UnmountSucceeded', 'UnmountFailed']);
        this.mSamba.addEventListener('ScanResult', (e) => {
            this.onScanResult(new cSambaServer(e.server), e.state);
        });
        this.mSamba.addEventListener('MountResult', (e) => {
            this.onMountResult(new cSambaServer(e.server), e.state);
        });
    }
    // API
    startScan(param) {
        this.mSamba.startScan();
    }
    stopScan(param) {
        this.mSamba.stopScan();
    }
    mount(param) {
        this.mSamba.mount(param.server.getPluginObject());
    }
    unmount(param) {
        this.mSamba.unmount(param.server.getPluginObject());
    }
    // EVENT
    onScanResult(server, state) {
        switch (state) {
            case ESAMBA_SCAN_STARTED:
                hx.log('dsrvSamba', 'onScanResult() - SAMBA_SCAN_STARTED');
                break;
            case ESAMBA_SCAN_ADDED:
                hx.log('dsrvSamba', 'onScanResult() - SAMBA_SCAN_ADDED');
                this.fireEvent('SambaServerAdded', server);
                break;
            case ESAMBA_SCAN_REMOVED:
                hx.log('dsrvSamba', 'onScanResult() - SAMBA_SCAN_REMOVED');
                this.fireEvent('SambaServerRemoved', server);
                break;
            case ESAMBA_SCAN_COMPLETE:
                hx.log('dsrvSamba', 'onScanResult() - SAMBA_SCAN_COMPLETE');
                break;
            case ESAMBA_SCAN_FAILED:
                hx.log('dsrvSamba', 'onScanResult() - SAMBA_SCAN_FAILED');
                break;
        }
    }
    onMountResult(server, state) {
        var a = this;
        switch (state) {
            case EMOUNT_SUCCEEDED:
                hx.log('dsrvSamba', 'onMountResult() - MOUNT_SUCCEEDED');
                this.fireEvent('MountSucceeded', server);
                break;
            case EMOUNT_FAILED:
                hx.log('dsrvSamba', 'onMountResult() - MOUNT_FAILED');
                this.fireEvent('MountFailed', server);
                break;
            case EUNMOUNT_SUCCEEDED:
                hx.log('dsrvSamba', 'onMountResult() - UNMOUNT_SUCCEEDED');
                this.fireEvent('UnmountSucceeded', server);
                break;
            case EUNMOUNT_FAILED:
                hx.log('dsrvSamba', 'onMountResult() - UNMOUNT_FAILED');
                this.fireEvent('UnmountFailed', server);
                break;
        }
    }
}
export = cDLSamba;