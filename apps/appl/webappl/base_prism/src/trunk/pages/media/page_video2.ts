///<reference path="../../def/sugar.d.ts" />

declare var hx;
import __MODEL__ = require("model/model");
import __MEDIA__ = require("./media");
// file name : pages/media/video2.js
/*jslint sloppy:true, nomen:true, vars:true */
class cCPgVideo extends __MEDIA__ {
    constructor() {
        super();
    }
    create (aEntity, aInfo) {
        return super.create(__MODEL__.IMediaItem.TContentType.EVideo, aInfo);
    }
}
export = cCPgVideo;

