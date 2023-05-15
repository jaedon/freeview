/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvStorageUtil.js
/*jslint nomen:true, vars:true */
//    STORAGE_UTIL: ['layer_domain/service/dl_dsrvStorageUtil.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    //var util = dl.registerApiGroup('dsrvStorageUtil');
    //var mStorageUtil = util.getPluginInstance('StorageUtil');
    var TDetachProgress = {
        EDetachProgressNone: 0,
        EDetachProgressOnGoing: 1,
        EDetachProgressFinished: 2,
        EDetachProgressError: 3
    };
    var cDLStorageUtil = (function (_super) {
        __extends(cDLStorageUtil, _super);
        function cDLStorageUtil(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mStorageUtil = this.getPluginInstance('StorageUtil');
            hx.logadd('dsrvStorageUtil');
            this.defineEvents(['PhysicalStorageAttached', 'PhysicalStorageDetached', 'PhysicalStorageStatus', 'DetachProgressOnGoing', 'DetachProgressFinished', 'DetachProgressError']);
            this.mStorageUtil.addEventListener('PhysicalStorageAttached', function (e) {
                _this.onPhysicalStorageAttached(e.physicalStorage);
            });
            this.mStorageUtil.addEventListener('PhysicalStorageDetached', function (e) {
                _this.onPhysicalStorageDetached(e.physicalStorage);
            });
            this.mStorageUtil.addEventListener('PhysicalStorageStatus', function (e) {
                _this.onPhysicalStorageStatus(e.id, e.state);
            });
        }
        cDLStorageUtil.prototype.fnLog = function (aLog) {
            hx.log('dsrvStorageUtil', aLog);
        };
        cDLStorageUtil.prototype.fnGetStorage = function (aPhysicalStorage) {
            var i, length;
            var logicalStorage, logicalStorages;
            var kind, usage, id, pid, label, pvr, strInterface;
            var path;
            id = aPhysicalStorage.id;
            kind = aPhysicalStorage.kind;
            usage = aPhysicalStorage.usage;
            strInterface = aPhysicalStorage.interfaceType;
            var storage = {
                id: id,
                partitions: []
            };
            logicalStorages = aPhysicalStorage.getLogicalStorages();
            length = logicalStorages.length;
            if (length === 0) {
                storage.partitions.push({
                    id: id,
                    kind: kind,
                    usage: usage
                });
            }
            else {
                for (i = 0; i < length; i += 1) {
                    logicalStorage = logicalStorages.item(i);
                    pid = logicalStorage.id;
                    label = logicalStorage.label;
                    path = logicalStorage.path;
                    pvr = logicalStorage.pvr;
                    if ((kind !== 1 || usage !== 1 || pvr !== 0) && (strInterface !== 1 || kind !== 1 || usage !== 3 || pvr !== 0)) {
                        storage.partitions.push({
                            id: id,
                            kind: kind,
                            usage: usage,
                            pid: pid,
                            label: label,
                            path: path,
                            pvr: pvr
                        });
                    }
                }
            }
            return storage;
        };
        cDLStorageUtil.prototype.fnGetStorages = function () {
            var i, length, storages = [];
            var physicalStorage, physicalStorages = this.mStorageUtil.getPhysicalStorages();
            length = physicalStorages.length;
            for (i = 0; i < length; i += 1) {
                physicalStorage = physicalStorages.item(i);
                if (physicalStorage.ismount) {
                    storages.push(this.fnGetStorage(physicalStorage));
                }
            }
            return storages;
        };
        cDLStorageUtil.prototype.fnDetach = function (aId) {
            var _this = this;
            var i, length;
            var mPhysicalStorages = [];
            var physicalStorage, physicalStorages = this.mStorageUtil.getPhysicalStorages();
            length = physicalStorages.length;
            var onDetachProgress = function (state, errMsg) {
                _this.fnLog(hx.util.format('onDetachProgress() - state: {0}, errMsg: {1}', state, errMsg));
                switch (state) {
                    case TDetachProgress.EDetachProgressNone:
                        break;
                    case TDetachProgress.EDetachProgressOnGoing:
                        _this.fireEvent('DetachProgressOnGoing');
                        break;
                    case TDetachProgress.EDetachProgressFinished:
                        _this.fireEvent('DetachProgressFinished');
                        break;
                    case TDetachProgress.EDetachProgressError:
                        _this.fireEvent('DetachProgressError', {
                            errMsg: errMsg
                        });
                        break;
                }
            };
            mPhysicalStorages = [];
            for (i = 0; i < length; i += 1) {
                physicalStorage = physicalStorages.item(i);
                if (physicalStorage.id === aId) {
                    mPhysicalStorages.push(physicalStorage);
                    physicalStorage.onDetachProgress = onDetachProgress;
                    physicalStorage.detach();
                    break;
                }
            }
        };
        cDLStorageUtil.prototype.fnRename = function (aId, aName) {
            var i, length;
            var physicalStorage, physicalStorages = this.mStorageUtil.getPhysicalStorages();
            length = physicalStorages.length;
            for (i = 0; i < length; i += 1) {
                physicalStorage = physicalStorages.item(i);
                if (physicalStorage.id === aId) {
                    physicalStorage.setStorageName(physicalStorage.uuid, aName);
                    break;
                }
            }
        };
        /**************************************************
         * Events
         **************************************************/
        cDLStorageUtil.prototype.onPhysicalStorageAttached = function (aPhysicalStorage) {
            var storage = this.fnGetStorage(aPhysicalStorage);
            this.fnLog(hx.util.format('onPhysicalStorageAttached() - id: {0}', storage.id));
            //checkDetachableStorage
            if (aPhysicalStorage.kind === 1 && (aPhysicalStorage.usage === 2 || aPhysicalStorage.usage === 4)) {
                if (hx.config.useDetachableStorage) {
                    hx.svc.SETTING_UTIL.checkDetachableStorage();
                }
            }
            else {
                this.fireEvent('PhysicalStorageAttached', {
                    storage: storage,
                    physicalStorage: aPhysicalStorage
                });
            }
        };
        cDLStorageUtil.prototype.onPhysicalStorageDetached = function (aPhysicalStorage) {
            var storage = this.fnGetStorage(aPhysicalStorage);
            this.fnLog(hx.util.format('onPhysicalStorageDetached() - id: {0}', storage.id));
            this.fireEvent('PhysicalStorageDetached', {
                storage: storage
            });
        };
        cDLStorageUtil.prototype.onPhysicalStorageStatus = function (aId, aState) {
            this.fnLog(hx.util.format('onPhysicalStorageStatus() - id: {0}, state: {1}', aId, aState));
            this.fireEvent('PhysicalStorageStatus', {
                id: aId,
                status: aState
            });
        };
        cDLStorageUtil.prototype.getStorages = function () {
            return this.fnGetStorages();
        };
        cDLStorageUtil.prototype.detach = function (aParam) {
            this.fnDetach(aParam.id);
        };
        cDLStorageUtil.prototype.rename = function (aParam) {
            this.fnRename(aParam.id, aParam.name);
        };
        return cDLStorageUtil;
    })(__KERNEL__);
    return cDLStorageUtil;
});
