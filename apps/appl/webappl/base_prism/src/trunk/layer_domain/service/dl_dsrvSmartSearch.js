/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvAvCtrl.js
/*jslint sloppy:true, nomen:true, vars:true */
//    SMART_SEARCH: ['layer_domain/service/dl_dsrvSmartSearch.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLSmart_Search = (function (_super) {
        __extends(cDLSmart_Search, _super);
        function cDLSmart_Search(aServiceName) {
            _super.call(this, aServiceName);
            this._keyword = '';
            this._playContent = null;
            this.init();
        }
        cDLSmart_Search.prototype.init = function () {
            hx.logadd('dsrvSmartSearch');
            hx.log('dsrvSmartSearch', 'create');
            $.extend(this, {
                _keyword: '',
                _playContent: null
            });
        };
        cDLSmart_Search.prototype.setPlayContent = function (aData) {
            hx.log('dsrvSmartSearch', hx.util.format('setPlayContent: data.fullPath = {0}', aData.fullPath));
            hx.log('dsrvSmartSearch', hx.util.format('setPlayContent: data.duration = {0}', aData.duration));
            hx.log('dsrvSmartSearch', hx.util.format('setPlayContent: data.title = {0}', aData.title));
            this._playContent = aData;
        };
        cDLSmart_Search.prototype.getPlayContent = function (aData) {
            hx.log('dsrvSmartSearch', hx.util.format('getPlayContent: that._playContent.title = {0}', this._playContent.title));
            hx.log('dsrvSmartSearch', hx.util.format('getPlayContent: that._playContent.fullPath = {0}', this._playContent.fullPath));
            hx.log('dsrvSmartSearch', hx.util.format('getPlayContent: that._playContent.duration = {0}', this._playContent.duration));
            this._playContent.index = 0;
            return this._playContent;
        };
        cDLSmart_Search.prototype.setKeyword = function (aKeyword) {
            this._keyword = aKeyword;
        };
        cDLSmart_Search.prototype.getKeyword = function () {
            return this._keyword;
        };
        return cDLSmart_Search;
    })(__KERNEL__);
    return cDLSmart_Search;
});
