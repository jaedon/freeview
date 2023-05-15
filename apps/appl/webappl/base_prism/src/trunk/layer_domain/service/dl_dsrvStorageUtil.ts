/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvStorageUtil.js
/*jslint nomen:true, vars:true */
//    STORAGE_UTIL: ['layer_domain/service/dl_dsrvStorageUtil.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

//var util = dl.registerApiGroup('dsrvStorageUtil');
//var mStorageUtil = util.getPluginInstance('StorageUtil');


var TDetachProgress = {
    EDetachProgressNone : 0,
    EDetachProgressOnGoing : 1,
    EDetachProgressFinished : 2,
    EDetachProgressError : 3
};

class cDLStorageUtil extends __KERNEL__ implements ifSVCStorageUtil {
    private mStorageUtil = this.getPluginInstance('StorageUtil');

    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvStorageUtil');
        this.defineEvents(['PhysicalStorageAttached', 'PhysicalStorageDetached', 'PhysicalStorageStatus', 'DetachProgressOnGoing', 'DetachProgressFinished', 'DetachProgressError']);

        this.mStorageUtil.addEventListener('PhysicalStorageAttached', (e) => {
            this.onPhysicalStorageAttached(e.physicalStorage);
        });
        this.mStorageUtil.addEventListener('PhysicalStorageDetached', (e) => {
            this.onPhysicalStorageDetached(e.physicalStorage);
        });
        this.mStorageUtil.addEventListener('PhysicalStorageStatus', (e) => {
            this.onPhysicalStorageStatus(e.id, e.state);
        });
    }

    fnLog (aLog) : void {
        hx.log('dsrvStorageUtil', aLog);
    }
    fnGetStorage (aPhysicalStorage) {
        var i : number, length : number;
        var logicalStorage : any, logicalStorages : any;
        var kind : number, usage : number, id : any, pid : any, label : any, pvr : number, strInterface : number;
        var path;

        id = aPhysicalStorage.id;
        kind = aPhysicalStorage.kind;
        usage = aPhysicalStorage.usage;
        strInterface = aPhysicalStorage.interfaceType;
        var storage : any = {
            id : id,
            partitions : []
        };

        logicalStorages = aPhysicalStorage.getLogicalStorages();
        length = logicalStorages.length;
        if (length === 0) {
            storage.partitions.push({
                id : id,
                kind : kind,
                usage : usage
            });
        } else {
            for ( i = 0; i < length; i += 1) {
                logicalStorage = logicalStorages.item(i);
                pid = logicalStorage.id;
                label = logicalStorage.label;
                path = logicalStorage.path;
                pvr = logicalStorage.pvr;
                if ((kind !== 1 || usage !== 1 || pvr !== 0) && (strInterface !== 1 || kind !== 1 || usage !==3 || pvr !== 0)) { //do not push internal not pvr partition
                    storage.partitions.push({
                        id : id,
                        kind : kind,
                        usage : usage,
                        pid : pid,
                        label : label,
                        path : path,
                        pvr : pvr
                    });
                }
            }
        }
        return storage;
    }
    fnGetStorages () {
        var i : number, length : number, storages : any = [];
        var physicalStorage : any, physicalStorages : any = this.mStorageUtil.getPhysicalStorages();
        length = physicalStorages.length;
        for ( i = 0; i < length; i += 1) {
            physicalStorage = physicalStorages.item(i);
            if (physicalStorage.ismount) {
                storages.push(this.fnGetStorage(physicalStorage));
            }
        }
        return storages;
    }

    fnDetach (aId) : void {
        var i : number, length : number;
        var mPhysicalStorages : any = [];
        var physicalStorage : any, physicalStorages : any = this.mStorageUtil.getPhysicalStorages();
        length = physicalStorages.length;
        var onDetachProgress = (state, errMsg) => {
            this.fnLog(hx.util.format('onDetachProgress() - state: {0}, errMsg: {1}', state, errMsg));
            switch (state) {
                case TDetachProgress.EDetachProgressNone:
                    break;
                case TDetachProgress.EDetachProgressOnGoing:
                    this.fireEvent('DetachProgressOnGoing');
                    break;
                case TDetachProgress.EDetachProgressFinished:
                    this.fireEvent('DetachProgressFinished');
                    break;
                case TDetachProgress.EDetachProgressError:
                    this.fireEvent('DetachProgressError', {
                        errMsg : errMsg
                    });
                    break;
            }
        };
        mPhysicalStorages = [];
        for ( i = 0; i < length; i += 1) {
            physicalStorage = physicalStorages.item(i);
            if (physicalStorage.id === aId) {
                mPhysicalStorages.push(physicalStorage);
                physicalStorage.onDetachProgress = onDetachProgress;
                physicalStorage.detach();
                break;
            }
        }
    }
    fnRename (aId, aName) : void {
        var i : number, length : number;
        var physicalStorage : any, physicalStorages : any = this.mStorageUtil.getPhysicalStorages();
        length = physicalStorages.length;
        for ( i = 0; i < length; i += 1) {
            physicalStorage = physicalStorages.item(i);
            if (physicalStorage.id === aId) {
                physicalStorage.setStorageName(physicalStorage.uuid, aName);
                break;
            }
        }
    }

    /**************************************************
     * Events
     **************************************************/

    onPhysicalStorageAttached (aPhysicalStorage) : void {
        var storage : any = this.fnGetStorage(aPhysicalStorage);
        this.fnLog(hx.util.format('onPhysicalStorageAttached() - id: {0}', storage.id));
        //checkDetachableStorage
        if (aPhysicalStorage.kind === 1 && (aPhysicalStorage.usage === 2 || aPhysicalStorage.usage === 4)) {
            if (hx.config.useDetachableStorage) {
                hx.svc.SETTING_UTIL.checkDetachableStorage();
            }
        } else {
            this.fireEvent('PhysicalStorageAttached', {
                storage : storage,
                physicalStorage : aPhysicalStorage
            });
        }
    }

    onPhysicalStorageDetached (aPhysicalStorage) : void {
        var storage = this.fnGetStorage(aPhysicalStorage);
        this.fnLog(hx.util.format('onPhysicalStorageDetached() - id: {0}', storage.id));
        this.fireEvent('PhysicalStorageDetached', {
            storage : storage
        });
    }
    onPhysicalStorageStatus (aId, aState) : void {
        this.fnLog(hx.util.format('onPhysicalStorageStatus() - id: {0}, state: {1}', aId, aState));
        this.fireEvent('PhysicalStorageStatus', {
            id : aId,
            status : aState
        });
    }

    getStorages () {
        return this.fnGetStorages();
    }
    detach (aParam) : void {
        this.fnDetach(aParam.id);
    }
    rename (aParam) : void {
        this.fnRename(aParam.id, aParam.name);
    }
}
export = cDLStorageUtil;

