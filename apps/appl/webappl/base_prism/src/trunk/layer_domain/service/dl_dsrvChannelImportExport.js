var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    /// <reference path="../../def/sugar.d.ts" />
    //    CHANNEL_IMPORT_EXPORT: ['layer_domain/service/dl_dsrvChannelImportExport.js'],
    var cDLChannelImportExport = (function (_super) {
        __extends(cDLChannelImportExport, _super);
        function cDLChannelImportExport(aServiceName) {
            _super.call(this, aServiceName);
            this.TErrorType = {
                EErrorNone: 0,
                EErrorFailure: 1,
                EErrorNoUsb: 2,
                EErrorNoFile: 3
            };
            hx.logadd('dsrvChannelImportExport');
            hx.log('dsrvChannelImportExport', 'create');
            this.mOipfLocalSystemObj = this.getPluginInstance('LocalSystem');
            this.mOipfChImExportObj = this.mOipfLocalSystemObj.channelImportExport;
            this.init();
        }
        cDLChannelImportExport.prototype.init = function () {
            var _this = this;
            this.mOipfChImExportObj.onChannelImportExport = function (type, result) {
                hx.log("error", hx.util.format("+++++ ### {0}.onChannelImportExport ### +++++, type : {1}, result : {2}", 'dsrvChannelImportExport', type, result));
                _this.fnOnChannelImportExport(type, result);
            };
        };
        cDLChannelImportExport.prototype.fnOnChannelImportExport = function (type, result) {
            this.fireEvent_Legacy('ChannelImportExport', {
                'type': type,
                "result": result
            });
        };
        cDLChannelImportExport.prototype.startChannelExport = function () {
            this.init();
            this.mOipfChImExportObj.startChannelExport();
        };
        cDLChannelImportExport.prototype.startChannelImport = function () {
            this.init();
            this.mOipfChImExportObj.startChannelImport();
        };
        return cDLChannelImportExport;
    })(__KERNEL__);
    return cDLChannelImportExport;
});
