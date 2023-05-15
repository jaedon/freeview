/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvStorageDlna.js
/*jslint vars:true */
/*jslint nomen:true */
//    STORAGE_DLNA: ['layer_domain/service/dl_dsrvStorageDlna.js'],

declare var hx: ifSugar;

import __KERNEL__ = require("./dl_kernel");

/*
 *
 * This is template of direct service.
 *
 * If you want to add direct service. please copy and commit and then mofidy.
 *
 */

enum eCDSScan {
    DMP_SCAN_STARTED,
    DMP_SCAN_ADD,
    DMP_SCAN_REMOVE,
    DMP_SCAN_COMPLETE,
    DMP_SCAN_ERROR
}
enum eContentBrowse {
    BROWSE_START,
    BROWSE_ADD,
    BROWSE_COMPLETE,
    BROWSE_ERROR
}

class cDLStorageDlna extends __KERNEL__ implements ifSVCStorageDlna {
    private _dlna;
    private _dmp;
    private _cdsList;
    private cachePath;
    private cacheIndex;

    constructor(aServiceName: string) {
        super(aServiceName);
        //        hx.logadd('dsrvStorageDlna');
        hx.log("dsrvStorageDlna", "initializer service - hx.svc.STORAGE_DLNA");
        hx.log("dsrvStorageDlna", "hx.svc.STORAGE_DLNA's _init() called");

        this._dlna = hx.stbObject.hDlna;
        this._dmp = this._dlna.dmp;
        this._cdsList = [];

        this._dmp.onCDSScan = (cds, state) => {
            hx.log("dsrvStorageDlna", 'OIPF onCDSScan ===> ' + 'cds:' + cds + ' state: ' + state);
            this.onCDSScan(cds, state);
        };
    }
//    External API
    startScan(): void {
        hx.log('dsrvStorageDlna', 'startScan');
        this._cdsList = [];
        this._dmp.startScan();
    }
    stopScan(): void {
        // TODO: have to call
        hx.log('dsrvStorageDlna', 'stopcan');
        this._cdsList.length = 0;
        this._dmp.stopScan();
    }
    refreshDms(aParam): void {
        hx.log('dsrvStorageDlna', 'refreshDms(' + aParam.udn + ')');
        this._dmp.refreshDms(aParam.udn);
    }
    browseContent(aParam): void {
        // TODO: need dms, cid, reqCount, fileExt, sortType
        hx.log('dsrvStorageDlna', 'browseContent()');
        var path, index;
        var cds = this._findCds(aParam.device);
        var reqCount = aParam.reqCount || 16;
        if (!aParam.index) {
            this.cachePath = aParam.path;
            this.cacheIndex = 0;
        } else {
            if (this.cacheIndex === aParam.index) {
                hx.log('dsrvStorageDlna', 'browseContent same index' + index + ' skip browseContent');
                /*
                 this.fireEvent_Legacy('DlnaContentBrowse', {
                 contents : [],
                 state : this.eContentBrowse.BROWSE_ADD
                 });
                 */
                this.fireEvent_Legacy('DlnaContentBrowse', {
                    contents : null,
                    state : eContentBrowse.BROWSE_COMPLETE
                });
                return;
            } else {
                this.cacheIndex = aParam.index;
            }
        }
        path = this.cachePath;
        index = this.cacheIndex;
        hx.log('dsrvStorageDlna', 'cds.browseContent(' + path + ', ' + reqCount + ', ' + index + ', 0)');
        cds.browseContent(path, 'ASC_TITLE', index, reqCount);
    }
//    Event
    onCDSScan(aCDS, aState): void {
        hx.log('dsrvStorageDlna', '[onCDScan]' + ' cds: ' + aCDS + ' state: ' + aState);
        var list = this._cdsList;
        var e = eCDSScan;

        if (aState === e.DMP_SCAN_ADD) {
            aCDS.onContentBrowse = (contents, state, parentPath) => {
                this.onContentBrowse(contents, state, parentPath);
            };
            list.push(aCDS);
        } else if (aState === e.DMP_SCAN_REMOVE) {
            hx.log('dsrvStorageDlna', '[onCDScan]' + ' cds: ' + aCDS.udn + ' state: ' + aState);
            this._delCds(aCDS.udn);
        }
        this.fireEvent_Legacy('CDSScan', {
            'cds' : aCDS,
            'state' : aState
        });
    }
    onContentBrowse(aContents, aState, aParentPath): void {
        hx.log('dsrvStorageDlna', '[onContentBrowse]');
        hx.log('dsrvStorageDlna', '[onContentBrowse] contents:' + aContents);
        hx.log('dsrvStorageDlna', '[onContentBrowse]' + aState);
        var e = eContentBrowse;
        var fullCount = 16;
        //TODO: temporary depence
        if (this.cachePath !== aParentPath) {
            hx.log('error', 'This event is wrong - current : ' + this.cachePath + 'event parent : ' + aParentPath);
        }
        if (aState === e.BROWSE_ADD) {
            hx.log('dsrvStorageDlna', '[onContentBrowse] if ' + aState);
            hx.log('dsrvStorageDlna', '[onContentBrowse] length ' + aContents.length);
            this.fireEvent_Legacy('DlnaContentBrowse', {
                contents : aContents,
                state : aState
            });
            if (aContents.length !== fullCount) {
                this.fireEvent_Legacy('DlnaContentBrowse', {
                    contents : null,
                    state : e.BROWSE_COMPLETE
                });
            }
        } else {
            hx.log('dsrvStorageDlna', '[onContentBrowse] else ' + aState);
            this.fireEvent_Legacy('DlnaContentBrowse', {
                contents : null,
                state : aState
            });
        }
    }
    private _findCds(aUDN, aCB?): any {
        var list = this._cdsList;
        var i, len, ret = null;
        if (list) {
            for ( i = 0, len = list.length; i < len; i += 1) {
                if (list[i].udn === aUDN) {
                    ret = list[i];
                    if (aCB) {
                        aCB(i);
                    }
                    break;
                }
            }
        }
        return ret;
    }
    private _delCds(aUDN): void {
        var list = this._cdsList, new_list = [];
        var i, len, ret = null;

        if (list) {
            for ( i = 0, len = list.length; i < len; i += 1) {
                if (list[i].udn === aUDN) {
                    continue;
                }
                new_list.push(list[i]);
            }
        }
        this._cdsList = new_list;
    }
}

export = cDLStorageDlna;
