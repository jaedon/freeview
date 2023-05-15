/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvStorageDlna.js
/*jslint vars:true */
/*jslint nomen:true */
//    STORAGE_DLNA: ['layer_domain/service/dl_dsrvStorageDlna.js'],
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
    var eCDSScan;
    (function (eCDSScan) {
        eCDSScan[eCDSScan["DMP_SCAN_STARTED"] = 0] = "DMP_SCAN_STARTED";
        eCDSScan[eCDSScan["DMP_SCAN_ADD"] = 1] = "DMP_SCAN_ADD";
        eCDSScan[eCDSScan["DMP_SCAN_REMOVE"] = 2] = "DMP_SCAN_REMOVE";
        eCDSScan[eCDSScan["DMP_SCAN_COMPLETE"] = 3] = "DMP_SCAN_COMPLETE";
        eCDSScan[eCDSScan["DMP_SCAN_ERROR"] = 4] = "DMP_SCAN_ERROR";
    })(eCDSScan || (eCDSScan = {}));
    var eContentBrowse;
    (function (eContentBrowse) {
        eContentBrowse[eContentBrowse["BROWSE_START"] = 0] = "BROWSE_START";
        eContentBrowse[eContentBrowse["BROWSE_ADD"] = 1] = "BROWSE_ADD";
        eContentBrowse[eContentBrowse["BROWSE_COMPLETE"] = 2] = "BROWSE_COMPLETE";
        eContentBrowse[eContentBrowse["BROWSE_ERROR"] = 3] = "BROWSE_ERROR";
    })(eContentBrowse || (eContentBrowse = {}));
    var cDLStorageDlna = (function (_super) {
        __extends(cDLStorageDlna, _super);
        function cDLStorageDlna(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            //        hx.logadd('dsrvStorageDlna');
            hx.log("dsrvStorageDlna", "initializer service - hx.svc.STORAGE_DLNA");
            hx.log("dsrvStorageDlna", "hx.svc.STORAGE_DLNA's _init() called");
            this._dlna = hx.stbObject.hDlna;
            this._dmp = this._dlna.dmp;
            this._cdsList = [];
            this._dmp.onCDSScan = function (cds, state) {
                hx.log("dsrvStorageDlna", 'OIPF onCDSScan ===> ' + 'cds:' + cds + ' state: ' + state);
                _this.onCDSScan(cds, state);
            };
        }
        //    External API
        cDLStorageDlna.prototype.startScan = function () {
            hx.log('dsrvStorageDlna', 'startScan');
            this._cdsList = [];
            this._dmp.startScan();
        };
        cDLStorageDlna.prototype.stopScan = function () {
            // TODO: have to call
            hx.log('dsrvStorageDlna', 'stopcan');
            this._cdsList.length = 0;
            this._dmp.stopScan();
        };
        cDLStorageDlna.prototype.refreshDms = function (aParam) {
            hx.log('dsrvStorageDlna', 'refreshDms(' + aParam.udn + ')');
            this._dmp.refreshDms(aParam.udn);
        };
        cDLStorageDlna.prototype.browseContent = function (aParam) {
            // TODO: need dms, cid, reqCount, fileExt, sortType
            hx.log('dsrvStorageDlna', 'browseContent()');
            var path, index;
            var cds = this._findCds(aParam.device);
            var reqCount = aParam.reqCount || 16;
            if (!aParam.index) {
                this.cachePath = aParam.path;
                this.cacheIndex = 0;
            }
            else {
                if (this.cacheIndex === aParam.index) {
                    hx.log('dsrvStorageDlna', 'browseContent same index' + index + ' skip browseContent');
                    /*
                     this.fireEvent_Legacy('DlnaContentBrowse', {
                     contents : [],
                     state : this.eContentBrowse.BROWSE_ADD
                     });
                     */
                    this.fireEvent_Legacy('DlnaContentBrowse', {
                        contents: null,
                        state: 2 /* BROWSE_COMPLETE */
                    });
                    return;
                }
                else {
                    this.cacheIndex = aParam.index;
                }
            }
            path = this.cachePath;
            index = this.cacheIndex;
            hx.log('dsrvStorageDlna', 'cds.browseContent(' + path + ', ' + reqCount + ', ' + index + ', 0)');
            cds.browseContent(path, 'ASC_TITLE', index, reqCount);
        };
        //    Event
        cDLStorageDlna.prototype.onCDSScan = function (aCDS, aState) {
            var _this = this;
            hx.log('dsrvStorageDlna', '[onCDScan]' + ' cds: ' + aCDS + ' state: ' + aState);
            var list = this._cdsList;
            var e = eCDSScan;
            if (aState === 1 /* DMP_SCAN_ADD */) {
                aCDS.onContentBrowse = function (contents, state, parentPath) {
                    _this.onContentBrowse(contents, state, parentPath);
                };
                list.push(aCDS);
            }
            else if (aState === 2 /* DMP_SCAN_REMOVE */) {
                hx.log('dsrvStorageDlna', '[onCDScan]' + ' cds: ' + aCDS.udn + ' state: ' + aState);
                this._delCds(aCDS.udn);
            }
            this.fireEvent_Legacy('CDSScan', {
                'cds': aCDS,
                'state': aState
            });
        };
        cDLStorageDlna.prototype.onContentBrowse = function (aContents, aState, aParentPath) {
            hx.log('dsrvStorageDlna', '[onContentBrowse]');
            hx.log('dsrvStorageDlna', '[onContentBrowse] contents:' + aContents);
            hx.log('dsrvStorageDlna', '[onContentBrowse]' + aState);
            var e = eContentBrowse;
            var fullCount = 16;
            //TODO: temporary depence
            if (this.cachePath !== aParentPath) {
                hx.log('error', 'This event is wrong - current : ' + this.cachePath + 'event parent : ' + aParentPath);
            }
            if (aState === 1 /* BROWSE_ADD */) {
                hx.log('dsrvStorageDlna', '[onContentBrowse] if ' + aState);
                hx.log('dsrvStorageDlna', '[onContentBrowse] length ' + aContents.length);
                this.fireEvent_Legacy('DlnaContentBrowse', {
                    contents: aContents,
                    state: aState
                });
                if (aContents.length !== fullCount) {
                    this.fireEvent_Legacy('DlnaContentBrowse', {
                        contents: null,
                        state: 2 /* BROWSE_COMPLETE */
                    });
                }
            }
            else {
                hx.log('dsrvStorageDlna', '[onContentBrowse] else ' + aState);
                this.fireEvent_Legacy('DlnaContentBrowse', {
                    contents: null,
                    state: aState
                });
            }
        };
        cDLStorageDlna.prototype._findCds = function (aUDN, aCB) {
            var list = this._cdsList;
            var i, len, ret = null;
            if (list) {
                for (i = 0, len = list.length; i < len; i += 1) {
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
        };
        cDLStorageDlna.prototype._delCds = function (aUDN) {
            var list = this._cdsList, new_list = [];
            var i, len, ret = null;
            if (list) {
                for (i = 0, len = list.length; i < len; i += 1) {
                    if (list[i].udn === aUDN) {
                        continue;
                    }
                    new_list.push(list[i]);
                }
            }
            this._cdsList = new_list;
        };
        return cDLStorageDlna;
    })(__KERNEL__);
    return cDLStorageDlna;
});
