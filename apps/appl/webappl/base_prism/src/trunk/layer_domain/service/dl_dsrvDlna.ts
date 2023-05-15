/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvDlna.js
/*jslint nomen:true, vars:true */
//    DLNA: ['layer_domain/service/dl_dsrvDlna.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("model/model");

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
    EStopped : 0,
    EStarted : 1
};

class cDlnaCds extends __MODEL__.CNetworkServer {
    _browsingItems;
    _searchedItems;
    _pluginObject;
    _searchId;
    _index;
    _browseInfo: any;
    _keyword;
    _dlDlna: cDLDlna;
    constructor(pluginObject, aDlDlna) {
        super();
        hx.log('dsrvDlna', 'new CDlnaCds()');
        this._dlDlna = aDlDlna;
        var cid, sort, index, count;
        this._browsingItems = [];
        this._searchedItems = [];
        this._pluginObject = pluginObject;
        this._pluginObject.onContentBrowse = (contents, state) => {
            if (state === BROWSE_ADD) {
                var i, length, item, items, contentType, objectType;
                items = [];
                length = contents.length;
                if (length) {
                    for ( i = 0; i < length; i += 1) {
                        item = new __MODEL__.cDlnaItem(contents.item(i));
                        contentType = item.getContentType();
                        objectType = item.getObjectType();
                        if (contentType === this._dlDlna.mContentType || objectType === 'folder') {
                            items.push(item);
                        }
                    }
                    if (length === BROWSING_COUNT) {
                        this._browseInfo.index += BROWSING_COUNT;
                        cid = this._browseInfo.cid;
                        sort = this._browseInfo.sort;
                        index = this._browseInfo.index;
                        count = this._browseInfo.count;
                        this._pluginObject.browseContent(cid, sort, index, count);
                    }
                }
                this._browsingItems.concat(items);
                this._dlDlna.fireEvent('BrowsingItemAdded', items);
            }
        };
        this._pluginObject.onContentSearch = (searchId, totalCount, contents, state) => {
            hx.log('dsrvDlna', hx.util.format('onContentSearch() - searchId = {0}, state = {1}', searchId, state));
            if (state === SEARCH_ADD && this._searchId === searchId) {
                var i, length = contents.length;
                var udn = this.getUdn();
                var item, items = [];
                var objectType = '';
                var mediaType;
                if (length) {
                    for ( i = 0; i < length; i += 1) {
                        item = new __MODEL__.cDlnaItem(contents.item(i));
                        mediaType = item.getObjectType();
                        if (mediaType === 'video' || mediaType === 'music' || mediaType === 'photo') {
                            this._searchedItems.push(item);
                            items.push(item);
                        }
                    }
                }
                if (this._dlDlna.mSearchedCdsList.indexOf(udn) === -1 && this._searchedItems.length > 0) {
                    hx.log('dsrvDlna', hx.util.format('SearchedCdsAdded() - udn = {0}', udn));
                    this._dlDlna.mSearchedCdsList.push(udn);
                    this._dlDlna.fireEvent('SearchedCdsAdded', this);
                } else if (items.length > 0) {
                    hx.log('dsrvDlna', 'SearchedItemAdded()');
                    this._dlDlna.fireEvent('SearchedItemAdded', this, items);
                }
                if (contents.length === KEYWORD_SEARCH_COUNT && this._searchedItems.length < KEYWORD_SEARCH_LIMIT) {
                    this.searchContent(this._keyword);
                }
            }
        };
    }
    getId(): any {
        return this._pluginObject.udn;
    }
    browseContent(cid): void {
        var sort = 'ASC_TITLE';
        var index = 0;
        var count = BROWSING_COUNT;
        this._browsingItems = [];
        this._browseInfo = {
            cid : cid,
            sort : sort,
            index : index,
            count : count
        };
        this._pluginObject.browseContent(cid, sort, index, count);
    }
    searchContent(keyword): any {
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
    }
    getUdn(): any {
        return this._pluginObject.udn;
    }
    getName(): any {
        return this._pluginObject.friendlyName;
    }
    getFriendlyName(): any {
        return this._pluginObject.friendlyName;
    }
    getIpAddress(): any {
        return this._pluginObject.ipAddress;
    }
    getBrowsingItems(): any {
        return this._browsingItems;
    }
    getSearchedItems(): any {
        return this._searchedItems;
    }
    getStorageType(): number {
        return __MODEL__.IMediaServer.TStorageType.EDlna;
    }
    getStorageCssClass(): string {
        return __MODEL__.IMediaServer.StorageCssClassList[__MODEL__.IMediaServer.TStorageType.EDlna];
    }
    getViewType(): any {
        return __MODEL__.IMediaServer.TViewType.EFolderView;
    }
    getPath(): number {
        return 0;
        // 0 : root;
    }
    getLocationType(): number {
        return __MODEL__.IMediaServer.TLocationType.ENetwork;
    }
    hasSearchedItems(): boolean {
        return this._searchedItems.length > 0;
    }
    isMounted(): boolean {
        return true;
    }
}
class cDLDlna extends __KERNEL__ implements ifSVCDlna {
    mDmp = this.getPluginInstance('DLNA.DMP');
    mBrowsingCdsMap = {};
    mSearchedCdsList = [];
    mCdsList = [];
    mKeyword = '';
    mContentType = null;
    mKeywordSearchState = KEYWORD_SEARCH_STOPPED;
    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvDlna');
        hx.log('dsrvDlna', 'create');
        this.defineEvents(['BrowsingCdsAdded', 'BrowsingCdsRemoved', 'SearchedCdsAdded', 'SearchedItemAdded', 'BrowsingItemAdded']);
        var onCDSScan = (cds, state) => {
            var dlnaCds = new cDlnaCds(cds, this);
            switch(state) {
                case DMP_SCAN_STARTED:
                    hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_STARTED');
                    break;
                case DMP_SCAN_ADD:
                    hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_ADD');
                    this.fireEvent('BrowsingCdsAdded', dlnaCds);
                    break;
                case DMP_SCAN_REMOVE:
                    hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_REMOVE');
                    this.fireEvent('BrowsingCdsRemoved', dlnaCds);
                    break;
                case DMP_SCAN_COMPLETE:
                    hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_COMPLETE');
                    break;
                case DMP_SCAN_FAILED:
                    hx.log('dsrvDlna', 'onCDSScan() - DMP_SCAN_FAILED');
                    break;
            }
        };
        this.mDmp.addEventListener('CDSScan', function(e) {
            hx.log('dsrvDlna', 'CDSScan()');
            onCDSScan(e.cds, e.state);
        });
        this.addEventCb('BrowsingCdsAdded', this, (dlnaCds) => {
            hx.log('dsrvDlna', 'BrowsingCdsAdded()');
            this.mBrowsingCdsMap[dlnaCds.getUdn()] = dlnaCds;
            this.mCdsList.push(dlnaCds);
            if (this.mKeywordSearchState === KEYWORD_SEARCH_STARTED) {
                dlnaCds.searchContent(this.mKeyword);
            }
        });
        this.addEventCb('BrowsingCdsRemoved', this, (dlnaCds) => {
            hx.log('dsrvDlna', 'BrowsingCdsRemoved()');
            delete this.mBrowsingCdsMap[dlnaCds.getUdn()];
        });
    }
    startScan(): void {
        hx.log('dsrvDlna', 'startScan()');
        this.mBrowsingCdsMap = {};
        if (this.getState() === TDmpState.EStarted) {
            this.stopScan();
        }
        this.mDmp.startScan();
    }
    stopScan(): void {
        hx.log('dsrvDlna', 'stopScan()');
        this.mDmp.stopScan();
    }
    browseContent(param): void {
        hx.log('dsrvDlna', 'browseContent()');
        var server = param.server;
        var folder = param.folder;
        var contentType = param.contentType;
        this.mContentType = contentType;
        server.browseContent(folder.getPath());
    }
    getState(): any {
        hx.log('dsrvDlna', 'getState()');
        return this.mDmp.state;
    }
    startKeywordSearch(param): void {
        var keyword = param.keyword;
        hx.log('dsrvDlna', hx.util.format('startKeywordSearch() - keyword = {0}', keyword));
        this.mKeyword = keyword;
        this.mSearchedCdsList = [];
        var i, length;
        var cds;
        this.mCdsList = this.getCdsList();
        length = this.mCdsList.length;
        for ( i = 0; i < length; i += 1) {
            cds = this.mCdsList[i];
            cds.searchContent(keyword);
        }
        this.mKeywordSearchState = KEYWORD_SEARCH_STARTED;
    }
    stopKeywordSearch(): void {
        hx.log('dsrvDlna', 'stopKeywordSearch()');
        this.mKeyword = '';
        this.mKeywordSearchState = KEYWORD_SEARCH_STOPPED;
    }
    getCdsList(): any {
        hx.log('dsrvDlna', 'getCdsList()');
        var i, length;
        var cds, cdsList = this.mDmp.getAvailableCdsList();
        var dlnaCds, dlnaCdsList = [];
        length = cdsList.length;
        if (length) {
            for ( i = 0; i < length; i += 1) {
                cds = cdsList.item(i);
                dlnaCds = new cDlnaCds(cds, this);
                dlnaCdsList.push(dlnaCds);
            }
        }
        return dlnaCdsList;
    }
}
export = cDLDlna;