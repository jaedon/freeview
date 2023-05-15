///<reference path="../../def/sugar.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "model/model", "./media"], function (require, exports, __MODEL__, __MEDIA__) {
    // file name : pages/media/photo2.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgPhoto = (function (_super) {
        __extends(cCPgPhoto, _super);
        function cCPgPhoto() {
            _super.call(this);
        }
        cCPgPhoto.prototype.create = function (aEntity, aInfo) {
            return _super.prototype.create.call(this, __MODEL__.IMediaItem.TContentType.EPhoto, aInfo);
        };
        return cCPgPhoto;
    })(__MEDIA__);
    return cCPgPhoto;
});
