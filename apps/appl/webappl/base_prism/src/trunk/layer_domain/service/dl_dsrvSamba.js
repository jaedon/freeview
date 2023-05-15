/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvSamba.js
/*jslint nomen:true, vars:true */
//    SAMBA: ['layer_domain/service/dl_dsrvSamba.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "../../model/model"], function (require, exports, __KERNEL__, __MODEL__) {
    var ESAMBA_SCAN_STARTED = 0;
    var ESAMBA_SCAN_ADDED = 1;
    var ESAMBA_SCAN_REMOVED = 2;
    var ESAMBA_SCAN_COMPLETE = 3;
    var ESAMBA_SCAN_FAILED = 4;
    var EMOUNT_SUCCEEDED = 10;
    var EMOUNT_FAILED = 11;
    var EUNMOUNT_SUCCEEDED = 12;
    var EUNMOUNT_FAILED = 13;
    var cSambaServer = (function (_super) {
        __extends(cSambaServer, _super);
        function cSambaServer(pluginObject) {
            _super.call(this);
            hx.log('dsrvSamba', 'new CSambaServer()');
            this._pluginObject = pluginObject;
        }
        cSambaServer.prototype.getPluginObject = function () {
            return this._pluginObject;
        };
        cSambaServer.prototype.getId = function () {
            return this.getName() + this.getSharedName();
        };
        cSambaServer.prototype.getName = function () {
            return this._pluginObject.serverName;
        };
        cSambaServer.prototype.getSharedName = function () {
            return this._pluginObject.sharedName;
        };
        cSambaServer.prototype.getIpAddress = function () {
            return this._pluginObject.ip;
        };
        cSambaServer.prototype.getStorageType = function () {
            return __MODEL__.IMediaServer.TStorageType.ESamba;
        };
        cSambaServer.prototype.getStorageCssClass = function () {
            return __MODEL__.IMediaServer.StorageCssClassList[__MODEL__.IMediaServer.TStorageType.ESamba];
        };
        cSambaServer.prototype.getViewType = function () {
            return __MODEL__.IMediaServer.TViewType.EFolderView;
        };
        cSambaServer.prototype.getPath = function () {
            return this._pluginObject.mountPath;
        };
        cSambaServer.prototype.getLocationType = function () {
            return __MODEL__.IMediaServer.TLocationType.ENetwork;
        };
        cSambaServer.prototype.getUserId = function () {
            return this._pluginObject.id;
        };
        cSambaServer.prototype.getPassword = function () {
            return this._pluginObject.password;
        };
        cSambaServer.prototype.setId = function (id) {
            this._pluginObject.id = id;
        };
        cSambaServer.prototype.setPassword = function (password) {
            this._pluginObject.password = password;
        };
        cSambaServer.prototype.isMounted = function () {
            return this._pluginObject.isMounted;
        };
        return cSambaServer;
    })(__MODEL__.CNetworkServer);
    var cDLSamba = (function (_super) {
        __extends(cDLSamba, _super);
        function cDLSamba(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mSamba = this.getPluginInstance('Samba');
            hx.logadd('dsrvSamba');
            hx.log('dsrvSamba', 'create');
            this.defineEvents(['SambaServerAdded', 'SambaServerRemoved', 'MountSucceeded', 'MountFailed', 'UnmountSucceeded', 'UnmountFailed']);
            this.mSamba.addEventListener('ScanResult', function (e) {
                _this.onScanResult(new cSambaServer(e.server), e.state);
            });
            this.mSamba.addEventListener('MountResult', function (e) {
                _this.onMountResult(new cSambaServer(e.server), e.state);
            });
        }
        // API
        cDLSamba.prototype.startScan = function (param) {
            this.mSamba.startScan();
        };
        cDLSamba.prototype.stopScan = function (param) {
            this.mSamba.stopScan();
        };
        cDLSamba.prototype.mount = function (param) {
            this.mSamba.mount(param.server.getPluginObject());
        };
        cDLSamba.prototype.unmount = function (param) {
            this.mSamba.unmount(param.server.getPluginObject());
        };
        // EVENT
        cDLSamba.prototype.onScanResult = function (server, state) {
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
        };
        cDLSamba.prototype.onMountResult = function (server, state) {
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
        };
        return cDLSamba;
    })(__KERNEL__);
    return cDLSamba;
});
