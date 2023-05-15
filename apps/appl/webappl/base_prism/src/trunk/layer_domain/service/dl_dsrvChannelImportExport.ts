// file name : layer_domain/service/dl_dsrvChannelImportExport.js
/// <reference path="../../def/sugar.d.ts" />
declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
//    CHANNEL_IMPORT_EXPORT: ['layer_domain/service/dl_dsrvChannelImportExport.js'],

class cDLChannelImportExport extends __KERNEL__ implements ifSVCChannelImportExport {
    TErrorType = {// refer DxImportExportErrorType_e @ xmgr_setting_import_export_mesat.c
        EErrorNone : 0,
        EErrorFailure : 1,
        EErrorNoUsb : 2,
        EErrorNoFile : 3
    };
    private mOipfLocalSystemObj;
    private mOipfChImExportObj;
    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvChannelImportExport');
        hx.log('dsrvChannelImportExport', 'create');
        this.mOipfLocalSystemObj = this.getPluginInstance('LocalSystem');
        this.mOipfChImExportObj = this.mOipfLocalSystemObj.channelImportExport;
        this.init();
    }
    private init() {
        this.mOipfChImExportObj.onChannelImportExport = (type, result) => {
            hx.log("error", hx.util.format("+++++ ### {0}.onChannelImportExport ### +++++, type : {1}, result : {2}", 'dsrvChannelImportExport', type, result));
            this.fnOnChannelImportExport(type, result);
        };
    }
    private fnOnChannelImportExport(type, result) {
        this.fireEvent_Legacy('ChannelImportExport', {
            'type' : type,
            "result" : result
        });
    }
    startChannelExport(): void {
        this.init();
        this.mOipfChImExportObj.startChannelExport();
    }
    startChannelImport(): void {
        this.init();
        this.mOipfChImExportObj.startChannelImport();
    }
}
export = cDLChannelImportExport;