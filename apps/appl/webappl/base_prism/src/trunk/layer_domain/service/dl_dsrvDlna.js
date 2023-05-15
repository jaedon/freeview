/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvDlna.js
/*jslint nomen:true, vars:true */
//    DLNA: ['layer_domain/service/dl_dsrvDlna.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "model/model"], function (require, exports, __KERNEL__, __MODEL__) {
    var DMP_SCAN_STARTED = 0;
    var DMP_SCAN_ADD = 1;
    var DMP_SCAN_REMOVE = 2;
    var DMP_SCAN_COMPLETE = 3;
    var DMP_SCAN_FAILED = 4;
    var BROWSE_START = 0;
    var BROWSE_ADD = 1;
    var BROWSE_COMPLETE = 2;
    var BROWSE_ERROR = 3;
    var BROWSING_COUNT = 16;
    var SEARCH_ADD = 0;
    var SEARCH_ERROR = 1;
    var KEYWORD_SEARCH_STARTED = 0;
    var KEYWORD_SEARCH_STOPPED = 1;
    var KEYWORD_SEARCH_LIMIT = 100;
    var KEYWORD_SEARCH_COUNT = 10;
    var TDmpState = {
        EStopped: 0,
        EStarted: 1
    };
    var cDlnaCds = (function (_super) {
        __extends(cDlnaCds, _super);
        function cDlnaCds(pluginObject, aDlDlna) {
            var _this = this;
            _super.call(this);
            hx.log('dsrvDlna', 'new CDlnaCds()');
            this._dlDlna = aDlDlna;
            var cid, sort, index, count;
            this._browsingItems = [];
            this._searchedItems = [];
            this._pluginObject = pluginObject;
            this._pluginObject.onContentBrowse = function (contents, state) {
                if (state === BROWSE_ADD) {
                    var i, length, item, items, contentType, objectType;
                    items = [];
                    length = contents.length;
                    if (length) {
                        for (i = 0; i < length; i += 1) {
                            item = new __MODEL__.cDlnaItem(contents.item(i));
                            contentType = item.getContentType();
                            objectType = item.getObjectType();
                            if (contentType === _this._dlDlna.mContentType || objectType === 'folder') {
                                items.push(item);
                            }
                        }
                        if (length === BROWSING_COUNT) {
                            _this._browseInfo.index += BROWSING_COUNT;
                            cid = _this._browseInfo.cid;
                            sort = _this._browseInfo.sort;
                            index = _this._browseInfo.index;
                            count = _this._browseInfo.count;
                            _this._pluginObject.browseContent(cid, sort, index, count);
                        }
                    }
                    _this._browsingItems.concat(items);
                    _this._dlDlna.fireEvent('BrowsingItemAdded', items);
                }
            };
            this._pluginObject.onContentSearch = function (searchId, totalCount, contents, state) {
                hx.log('dsrvDlna', hx.util.format('onContentSearch() - searchId = {0}, state = {1}', searchId, state));
                if (state === SEARCH_ADD && _this._searchId === searchId) {
                    var i, length = contents.length;
                    var udn = _this.getUdn();
                    var item, items = [];
                    var objectType = '';
                    var mediaType;
                    if (length) {
                        for (i = 0; i < length; i += 1) {
                            item = new __MODEL__.cDlnaItem(contents.item(i));
                            mediaType = item.getObjectType();
                            if (mediaType === 'video' || mediaType === 'music' || mediaType === 'photo') {
                                _this._searchedItems.push(item);
                                items.push(item);
                            }
                        }
                    }
                    if (_this._dlDlna.mSearchedCdsList.indexOf(udn) === -1 && _this._searchedItems.length > 0) {
                        hx.log('dsrvDlna', hx.util.format('SearchedCdsAdded() - udn = {0}', udn));
                        _this._dlDlna.mSearchedCdsList.push(udn);
                        _this._dlDlna.fireEvent('SearchedCdsAdded', _this);
                    }
                    else if (items.length > 0) {
                        hx.log('dsrvDlna', 'SearchedItemAdded()');
                        _this._dlDlna.fireEvent('SearchedItemAdded', _this, items);
                    }
                    if (contents.length === KEYWORD_SEARCH_COUNT && _this._searchedItems.length < KEYWORD_SEARCH_LIMIT) {
                        _this.searchContent(_this._keyword);
                    }
                }
            };
        }
        cDlnaCds.prototype.getId = function () {
            return this._pluginObject.udn;
        };
        cDlnaCds.prototype.browseContent = function (cid) {
            var sort = 'ASC_TITLE';
            var index = 0;
            var count = BROWSING_COUNT;
            this._browsingItems = [];
            this._browseInfo = {
                cid: cid,
                sort: sort,
                index: index,
                count: count
            };
            this._pluginObject.browseContent(cid, sort, index, count);
        };
        cDlnaCds.prototype.searchContent = function (keyword) {
            var cid = '0';
            var category = 'title';
            var sort = 'ASC_TITLE';
            var index = 0;
            var count = KEYWORD_SEARCH_COUNT;
            this._index = this._index !== undefined ? this._index += count : 0;
            this._keyword = keyword;
            this._dlDlna.mKeyword = keyword;
            hx.log('dsrvDlna', hx.util.format('searchContent() - category = {0}, keyword = {1}, sort = {2}, index = {3}, count = {4}', category, this._keyword, sort, this._index, count));
            this._searchId = this._pluginObject.searchContent(cid, category, this._keyword, sort, this._index, count);
            hx.log('dsrvDlna', hx.util.format('searchContent() - searchId = {0}', this._searchId));
            return this._searchId;
        };
        cDlnaCds.prototype.getUdn = function () {
            return this._pluginObject.udn;
        };
        cDlnaCds.prototype.getName = function () {
            return this._pluginObject.friendlyName;
        };
        cDlnaCds.prototype.getFriendlyName = function () {
            return this._pluginObject.friendlyName;
        };
        cDlnaCds.prototype.getIpAddress = function () {
            return this._pluginObject.ipAddress;
        };
        cDlnaCds.prototype.getBrowsingItems = function () {
            return this._browsingItems;
        };
        cDlnaCds.prototype.getSearchedItems = function () {
            return this._searchedItems;
        };
        cDlnaCds.prototype.getStorageType = function () {
            return __MODEL__.IMediaServer.TStorageType.EDlna;
        };
        cDlnaCds.prototype.getStorageCssClass = function () {
            return __MODEL__.IMediaServer.StorageCssClassList[__MODEL__.IMediaServer.TStorageType.EDlna];
        };
        cDlnaCds.prototype.getViewType = function () {
            return __MODEL__.IMediaServer.TViewType.EFolderView;
        };
        cDlnaCds.prototype.getPath = function () {
            return 0;
            // 0 : root;
        };
        cDlnaCds.prototype.getLocationType = function () {
            return __MODEL__.IMediaServer.TLocationType.ENetwork;
        };
        cDlnaCds.prototype.hasSearchedItems = function () {
            return this._searchedItems.length > 0;
        };
        cDlnaCds.prototype.isMounted = function () {
            return true;
        };
        return cDlnaCds;
    })(__MODEL__.CNetworkServer);
    var cDLDlna = (function (_super) {
        __extends(cDLDlna, _super);
        function cDLDlna(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mDmp = this.getPluginInstance('DLNA.DMP');
            this.mBrowsingCdsMap = {};
            this.mSearchedCdsList = [];
            this.mCdsList = [];
            this.mKeyword = '';
            this.mContentType = null;
            this.mKeywordSearchState = KEYWORD_SEARCH_STOPPED;
            hx.logadd('dsrvDlna');
            hx.log('dsrvDlna', 'create');
            this.defineEvents(['BrowsingCdsAdded', 'BrowsingCdsRemoved', 'SearchedCdsAdded', 'SearchedItemAdded', 'BrowsingItemAdded']);
            var onCDSScan = function (cds, state) {
                var dlnaCds = new cDlnaCds(cds, _this);
                switch (state) {
                    case DMP_SCAN_STARTED:
                        hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_STARTED');
                        break;
                    case DMP_SCAN_ADD:
                        hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_ADD');
                        _this.fireEvent('BrowsingCdsAdded', dlnaCds);
                        break;
                    case DMP_SCAN_REMOVE:
                        hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_REMOVE');
                        _this.fireEvent('BrowsingCdsRemoved', dlnaCds);
                        break;
                    case DMP_SCAN_COMPLETE:
                        hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_COMPLETE');
                        break;
                    case DMP_SCAN_FAILED:
                        hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_FAILED');
                        break;
                }
            };
            this.mDmp.addEventListener('CDSScan', function (e) {
                hx.log('dsrvDlna', 'CDSScan()');
                onCDSScan(e.cds, e.state);
            });
            this.addEventCb('BrowsingCdsAdded', this, function (dlnaCds) {
                hx.log('dsrvDlna', 'BrowsingCdsAdded()');
                _this.mBrowsingCdsMap[dlnaCds.getUdn()] = dlnaCds;
                _this.mCdsList.push(dlnaCds);
                if (_this.mKeywordSearchState === KEYWORD_SEARCH_STARTED) {
                    dlnaCds.searchContent(_this.mKeyword);
                }
            });
            this.addEventCb('BrowsingCdsRemoved', this, function (dlnaCds) {
                hx.log('dsrvDlna', 'BrowsingCdsRemoved()');
                delete _this.mBrowsingCdsMap[dlnaCds.getUdn()];
            });
        }
        cDLDlna.prototype.startScan = function () {
            hx.log('dsrvDlna', 'startScan()');
            this.mBrowsingCdsMap = {};
            if (this.getState() === TDmpState.EStarted) {
                this.stopScan();
            }
            this.mDmp.startScan();
        };
        cDLDlna.prototype.stopScan = function () {
            hx.log('dsrvDlna', 'stopScan()');
            this.mDmp.stopScan();
        };
        cDLDlna.prototype.browseContent = function (param) {
            hx.log('dsrvDlna', 'browseContent()');
            var server = param.server;
            var folder = param.folder;
            var contentType = param.contentType;
            this.mContentType = contentType;
            server.browseContent(folder.getPath());
        };
        cDLDlna.prototype.getState = function () {
            hx.log('dsrvDlna', 'getState()');
            return this.mDmp.state;
        };
        cDLDlna.prototype.startKeywordSearch = function (param) {
            var keyword = param.keyword;
            hx.log('dsrvDlna', hx.util.format('startKeywordSearch() - keyword = {0}', keyword));
            this.mKeyword = keyword;
            this.mSearchedCdsList = [];
            var i, length;
            var cds;
            this.mCdsList = this.getCdsList();
            length = this.mCdsList.length;
            for (i = 0; i < length; i += 1) {
                cds = this.mCdsList[i];
                cds.searchContent(keyword);
            }
            this.mKeywordSearchState = KEYWORD_SEARCH_STARTED;
        };
        cDLDlna.prototype.stopKeywordSearch = function () {
            hx.log('dsrvDlna', 'stopKeywordSearch()');
            this.mKeyword = '';
            this.mKeywordSearchState = KEYWORD_SEARCH_STOPPED;
        };
        cDLDlna.prototype.getCdsList = function () {
            hx.log('dsrvDlna', 'getCdsList()');
            var i, length;
            var cds, cdsList = this.mDmp.getAvailableCdsList();
            var dlnaCds, dlnaCdsList = [];
            length = cdsList.length;
            if (length) {
                for (i = 0; i < length; i += 1) {
                    cds = cdsList.item(i);
                    dlnaCds = new cDlnaCds(cds, this);
                    dlnaCdsList.push(dlnaCds);
                }
            }
            return dlnaCdsList;
        };
        return cDLDlna;
    })(__KERNEL__);
    return cDLDlna;
});
