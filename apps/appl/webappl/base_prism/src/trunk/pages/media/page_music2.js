///<reference path="../../def/sugar.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "model/model", "./media"], function (require, exports, __MODEL__, __MEDIA__) {
    // file name : pages/media/music2.js
    /*jslint sloppy:true, nomen:true, vars:true */
    var cCPgMusic = (function (_super) {
        __extends(cCPgMusic, _super);
        function cCPgMusic() {
            _super.call(this);
        }
        cCPgMusic.prototype.create = function (aEntity, aInfo) {
            return _super.prototype.create.call(this, __MODEL__.IMediaItem.TContentType.EMusic, aInfo);
        };
        cCPgMusic.prototype.doSetMusicOption = function () {
            var that = this;
        };
        cCPgMusic.prototype.doGetMusicOption = function () {
            var that = this;
        };
        return cCPgMusic;
    })(__MEDIA__);
    return cCPgMusic;
});
