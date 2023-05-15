/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvAvCtrl.js
/*jslint sloppy:true, nomen:true, vars:true */
//    SMART_SEARCH: ['layer_domain/service/dl_dsrvSmartSearch.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

class cDLSmart_Search extends __KERNEL__ implements ifSVCSmartSearch {
    _keyword = '';
    _playContent = null;

    constructor(aServiceName: string) {
        super(aServiceName);
        this.init();
    }

    init () {
        hx.logadd('dsrvSmartSearch');
        hx.log('dsrvSmartSearch', 'create');

        $.extend(this, {
            _keyword : '',
            _playContent : null
        });
    }

    setPlayContent (aData) {
        hx.log('dsrvSmartSearch', hx.util.format('setPlayContent: data.fullPath = {0}', aData.fullPath));
        hx.log('dsrvSmartSearch', hx.util.format('setPlayContent: data.duration = {0}', aData.duration));
        hx.log('dsrvSmartSearch', hx.util.format('setPlayContent: data.title = {0}', aData.title));
        this._playContent = aData;
    }

    getPlayContent (aData) {
        hx.log('dsrvSmartSearch', hx.util.format('getPlayContent: that._playContent.title = {0}', this._playContent.title));
        hx.log('dsrvSmartSearch', hx.util.format('getPlayContent: that._playContent.fullPath = {0}', this._playContent.fullPath));
        hx.log('dsrvSmartSearch', hx.util.format('getPlayContent: that._playContent.duration = {0}', this._playContent.duration));
        this._playContent.index = 0;
        return this._playContent;
    }

    setKeyword (aKeyword) {
        this._keyword = aKeyword;
    }

    getKeyword () {
        return this._keyword;
    }

}
export = cDLSmart_Search;

