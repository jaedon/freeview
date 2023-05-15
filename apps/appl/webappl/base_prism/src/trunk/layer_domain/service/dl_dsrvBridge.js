var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "../../model/model"], function (require, exports, __KERNEL__, __MODEL__) {
    /// <reference path="../../def/sugar.d.ts" />
    var mPlaybackEnabled = true;
    var mTicket = '';
    /**************************************************
     * Events
     **************************************************/
    var cDLBridge = (function (_super) {
        __extends(cDLBridge, _super);
        function cDLBridge(aServiceName) {
            _super.call(this, aServiceName);
            this.defineEvents(['RequestMusicPlayback', 'RequestVideoPlayback', 'RequestPhotoPlayback', 'RequestInstanceRecording', 'RequestStopPlayback']);
        }
        cDLBridge.prototype.requestPlay = function (aItems, aStartIndex) {
            var TContentType = __MODEL__.IMediaItem.TContentType;
            var item = aItems[aStartIndex];
            var contentType = item.getContentType();
            switch (contentType) {
                case TContentType.EVideo:
                    this.requestVideoPlayback(aItems, aStartIndex);
                    break;
                case TContentType.EMusic:
                    this.requestMusicPlayback(aItems, aStartIndex);
                    break;
                case TContentType.EPhoto:
                    this.requestPhotoPlayback(aItems, aStartIndex);
                    break;
            }
        };
        cDLBridge.prototype.requestMusicPlayback = function (aItems, aStartIndex) {
            this.fireEvent('RequestMusicPlayback', {
                items: aItems,
                startIndex: aStartIndex
            });
        };
        cDLBridge.prototype.requestVideoPlayback = function (aItems, aStartIndex, aKeyCode) {
            var _this = this;
            this.require('HISTORY', function (dsrvHistory) {
                var mediaItem = dsrvHistory.findMediaItem({
                    id: aItems[0].getFullPath()
                });
                var lastPlayPosition = mediaItem ? mediaItem.getLastPlayPosition() : 0;
                if (lastPlayPosition > 0) {
                    aItems[0] = mediaItem;
                }
                _this.fireEvent('RequestVideoPlayback', {
                    items: aItems,
                    startIndex: aStartIndex,
                    lastPlayPosition: lastPlayPosition,
                    keyCode: aKeyCode
                });
            });
        };
        cDLBridge.prototype.requestPhotoPlayback = function (aItem, aStartIndex) {
            this.fireEvent('RequestPhotoPlayback', {
                items: aItem,
                startIndex: aStartIndex
            });
        };
        cDLBridge.prototype.requestInstanceRecording = function (aChannel) {
            this.fireEvent('RequestInstanceRecording', {
                channel: aChannel
            });
        };
        cDLBridge.prototype.requestStopPlayback = function () {
            this.fireEvent('RequestStopPlayback');
        };
        cDLBridge.prototype.enablePlayback = function (aEnabled) {
            mPlaybackEnabled = aEnabled;
        };
        cDLBridge.prototype.isPlayEnabled = function () {
            return mPlaybackEnabled;
        };
        cDLBridge.prototype.setTicket = function (aTicket) {
            mTicket = aTicket;
        };
        cDLBridge.prototype.getTicket = function () {
            return mTicket;
        };
        return cDLBridge;
    })(__KERNEL__);
    return cDLBridge;
});
