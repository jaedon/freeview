/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvStorageSamba.js
/*jslint nomen:true, vars:true */
//    STORAGE_SAMBA: ['layer_domain/service/dl_dsrvStorageSamba.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    /*
     *
     * This is template of direct service.
     *
     * If you want to add direct service. please copy and commit and then mofidy.
     *
     */
    var eEventSamba;
    (function (eEventSamba) {
        eEventSamba[eEventSamba["SAMBA_SCAN_STARTED"] = 0] = "SAMBA_SCAN_STARTED";
        eEventSamba[eEventSamba["SAMBA_SCAN_ADD"] = 1] = "SAMBA_SCAN_ADD";
        eEventSamba[eEventSamba["SAMBA_SCAN_REMOVE"] = 2] = "SAMBA_SCAN_REMOVE";
        eEventSamba[eEventSamba["SAMBA_SCAN_COMPLETE"] = 3] = "SAMBA_SCAN_COMPLETE";
        eEventSamba[eEventSamba["SAMBA_SCAN_FAILED"] = 4] = "SAMBA_SCAN_FAILED";
        eEventSamba[eEventSamba["MOUNT_OK"] = 10] = "MOUNT_OK";
        eEventSamba[eEventSamba["MOUNT_FAIL"] = 11] = "MOUNT_FAIL";
        eEventSamba[eEventSamba["UNMOUNT_OK"] = 12] = "UNMOUNT_OK";
        eEventSamba[eEventSamba["UNMOUNT_FAIL"] = 13] = "UNMOUNT_FAIL";
    })(eEventSamba || (eEventSamba = {}));
    var cDLStorageSamba = (function (_super) {
        __extends(cDLStorageSamba, _super);
        function cDLStorageSamba(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            //hx.logadd('dsrvStorageSamba');
            hx.log("dsrvStorageSamba", "initializer service - hx.svc.STORAGE_SAMBA");
            hx.log("dsrvStorageSamba", "hx.svc.STORAGE_SAMBA's init() called");
            this.mSamba = hx.stbObject.hSamba;
            this.mSamba.onScanResult = function (server, state) {
                hx.log("dsrvStorageSamba", 'OIPF onScanResult ===> ' + 'server:' + server + ' state: ' + state);
                _this.onScanResult.apply(_this, arguments);
            };
            this.mSamba.onMountResult = function (server, state) {
                hx.log("dsrvStorageSamba", 'OIPF onMountResult ===> ' + 'server:' + server + ' state: ' + state);
                _this.onMountResult.apply(_this, arguments);
            };
        }
        //    External API Start
        cDLStorageSamba.prototype.startScan = function () {
            hx.log('dsrvStorageSamba', 'startScan');
            //this._list = new Array();
            this.mSamba.startScan();
        };
        cDLStorageSamba.prototype.stopScan = function () {
            hx.log('dsrvStorageSamba', 'stopcan');
            this.mSamba.stopScan();
        };
        cDLStorageSamba.prototype.unmountAll = function () {
            hx.log('dsrvStorageSamba', 'unmountAll');
            this.mSamba.unmountAll();
        };
        cDLStorageSamba.prototype.mount = function (aServer) {
            hx.log('dsrvStorageSamba', 'mount server: ' + aServer.serverName);
            hx.log('dsrvStorageSamba', 'mount id: ' + aServer.id);
            hx.log('dsrvStorageSamba', 'mount passwrd: ' + aServer.password);
            var result = this.mSamba.mount(aServer);
            hx.log('dsrvStorageSamba', 'mount request: ' + result);
            return result;
        };
        cDLStorageSamba.prototype.unmount = function (aServer) {
            hx.log('dsrvStorageSamba', 'unmount server: ' + aServer.serverName);
            var result = this.mSamba.unmount(aServer);
            return result;
        };
        //    Event
        cDLStorageSamba.prototype.onScanResult = function (aServer, aState) {
            hx.log('dsrvStorageSamba', '[onScanResult] : ' + aState);
            if (aState === 1 /* SAMBA_SCAN_ADD */) {
                hx.log('dsrvStorageSamba', '[onScanResult]' + ' server: ' + aServer.serverName + ':' + aServer.sharedName + ' state: ' + aState);
                this.fireEvent_Legacy('SambaScanResult', {
                    'server': aServer,
                    'state': aState
                });
            }
            else if (aState === 2 /* SAMBA_SCAN_REMOVE */) {
                hx.log('dsrvStorageSamba', '[onScanResult] remove' + ' server: ' + aServer.serverName + ':' + aServer.sharedName + ' state: ' + aState);
                this.fireEvent_Legacy('SambaScanResult', {
                    'server': aServer,
                    'state': aState
                });
            }
        };
        cDLStorageSamba.prototype.onMountResult = function (aServer, aState) {
            hx.log('dsrvStorageSamba', '[onMountResult] : ' + aState);
            this.fireEvent_Legacy('SambaMountResult', {
                'server': aServer,
                'state': aState
            });
        };
        return cDLStorageSamba;
    })(__KERNEL__);
    return cDLStorageSamba;
});
