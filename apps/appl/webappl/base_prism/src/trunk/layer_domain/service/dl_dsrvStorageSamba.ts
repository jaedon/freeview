/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvStorageSamba.js
/*jslint nomen:true, vars:true */
//    STORAGE_SAMBA: ['layer_domain/service/dl_dsrvStorageSamba.js'],

declare var hx: ifSugar;

import __KERNEL__ = require("./dl_kernel");

/*
 *
 * This is template of direct service.
 *
 * If you want to add direct service. please copy and commit and then mofidy.
 *
 */

enum eEventSamba {
    SAMBA_SCAN_STARTED,
    SAMBA_SCAN_ADD,
    SAMBA_SCAN_REMOVE,
    SAMBA_SCAN_COMPLETE,
    SAMBA_SCAN_FAILED,
    MOUNT_OK = 10,
    MOUNT_FAIL = 11,
    UNMOUNT_OK = 12,
    UNMOUNT_FAIL = 13
}

class cDLStorageSamba extends __KERNEL__ implements ifSVCStorageSamba {
    private mSamba;

    constructor(aServiceName: string) {
        super(aServiceName);
        //hx.logadd('dsrvStorageSamba');

        hx.log("dsrvStorageSamba", "initializer service - hx.svc.STORAGE_SAMBA");
        hx.log("dsrvStorageSamba", "hx.svc.STORAGE_SAMBA's init() called");

        this.mSamba = hx.stbObject.hSamba;
        this.mSamba.onScanResult = (server, state) => {
            hx.log("dsrvStorageSamba", 'OIPF onScanResult ===> ' + 'server:' + server + ' state: ' + state);
            this.onScanResult.apply(this, arguments);
        };
        this.mSamba.onMountResult = (server, state) => {
            hx.log("dsrvStorageSamba", 'OIPF onMountResult ===> ' + 'server:' + server + ' state: ' + state);
            this.onMountResult.apply(this, arguments);
        };
    }
//    External API Start
    startScan(): void {
        hx.log('dsrvStorageSamba', 'startScan');
        //this._list = new Array();
        this.mSamba.startScan();
    }
    stopScan(): void {
        hx.log('dsrvStorageSamba', 'stopcan');
        this.mSamba.stopScan();
    }
    unmountAll(): void {
        hx.log('dsrvStorageSamba', 'unmountAll');
        this.mSamba.unmountAll();
    }
    mount(aServer): any {
        hx.log('dsrvStorageSamba', 'mount server: ' + aServer.serverName);
        hx.log('dsrvStorageSamba', 'mount id: ' + aServer.id);
        hx.log('dsrvStorageSamba', 'mount passwrd: ' + aServer.password);
        var result = this.mSamba.mount(aServer);
        hx.log('dsrvStorageSamba', 'mount request: ' + result);
        return result;
    }
    unmount(aServer): any {
        hx.log('dsrvStorageSamba', 'unmount server: ' + aServer.serverName);
        var result = this.mSamba.unmount(aServer);
        return result;
    }
//    Event
    onScanResult(aServer, aState): void {
        hx.log('dsrvStorageSamba', '[onScanResult] : ' + aState);
        if (aState === eEventSamba.SAMBA_SCAN_ADD) {
            hx.log('dsrvStorageSamba', '[onScanResult]' + ' server: ' + aServer.serverName + ':' + aServer.sharedName + ' state: ' + aState);
            this.fireEvent_Legacy('SambaScanResult', {
                'server' : aServer,
                'state' : aState
            });
        } else if (aState === eEventSamba.SAMBA_SCAN_REMOVE) {
            hx.log('dsrvStorageSamba', '[onScanResult] remove' + ' server: ' + aServer.serverName + ':' + aServer.sharedName + ' state: ' + aState);
            this.fireEvent_Legacy('SambaScanResult', {
                'server' : aServer,
                'state' : aState
            });
        }
    }
    onMountResult(aServer, aState): void {
        hx.log('dsrvStorageSamba', '[onMountResult] : ' + aState);
        this.fireEvent_Legacy('SambaMountResult', {
            'server' : aServer,
            'state' : aState
        });
    }
}
export = cDLStorageSamba;
