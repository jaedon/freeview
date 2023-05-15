/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvDmr2.js
/*jslint nomen:true, vars:true */
//    DMR: ['layer_domain/service/dl_dsrvDmr2.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var fnLog = function (log) {
        hx.log('dsrvDmr2', '##################################################');
        hx.log('dsrvDmr2', '#####' + log);
        hx.log('dsrvDmr2', '##################################################');
    };
    var cDLDMR = (function (_super) {
        __extends(cDLDMR, _super);
        function cDLDMR(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mVideoBroadcast = this.getPluginInstance('VideoBroadcast');
            this.mDmr = this.getPluginInstance('DLNA.DMR');
            this.mCurrentUri = null;
            // hx.logadd('dsrvDmr2');
            this.defineEvents(['RequestPlay', 'RequestStop', 'RequestPause', 'RequestSetVolume', 'RequestMute', 'RequestBringIn']);
            this.mDmr.addEventListener('ReqPlay', function (e) {
                _this.onRequestPlay(e.item);
            });
            this.mDmr.addEventListener('ReqStop', function (e) {
                _this.onRequestStop();
            });
            this.mDmr.addEventListener('ReqPause', function (e) {
                _this.onRequestPause();
            });
            this.mDmr.addEventListener('ReqSetVolume', function (e) {
                _this.onRequestSetVolume();
            });
            this.mDmr.addEventListener('ReqMute', function (e) {
                _this.onRequestMute();
            });
            this.mDmr.addEventListener('ReqBringIn', function (e) {
                _this.onRequestBringIn();
            });
            this.mVideoBroadcast.addEventListener('ChannelChangeSucceeded', function (e) {
                _this.onChannelChangeSucceeded(e.channel);
            });
            this.mVideoBroadcast.addEventListener('ChannelChangeError', function (e) {
                _this.onChannelChangeError(e.channel, e.errorState);
            });
            this.mVideoBroadcast.addEventListener('PlayStateChange', function (e) {
                _this.onPlayStateChange(e.state, e.error);
            });
            this.mVideoBroadcast.addEventListener('ParentalRatingChange', function (e) {
                _this.onParentalRatingChange(e.contentID, e.rating, e.DRMSystemID, e.blocked);
            });
        }
        cDLDMR.prototype.start = function () {
            fnLog('fnStart');
            this.mDmr.start();
        };
        cDLDMR.prototype.stop = function () {
            fnLog('fnStop');
            this.mDmr.stop();
        };
        cDLDMR.prototype.isStarted = function () {
            var started = this.mDmr.started;
            fnLog(hx.util.format('fnIsStarted - started = {0}', started));
            return started;
        };
        cDLDMR.prototype.isActivated = function () {
            var activated = this.mDmr.activated;
            fnLog(hx.util.format('fnIsActivated - activaged = {0}', activated));
            return activated;
        };
        cDLDMR.prototype.setMedia = function (uri) {
            var isMedia = false;
            this.mCurrentUri = null;
            if (uri === null) {
                fnLog('fnIsActivated - uri = null !!!!!');
                this.mDmr.setMediaChange(uri);
            }
            else if (typeof uri !== "string") {
                throw 'uri must string type';
            }
            else {
                fnLog(hx.util.format('this.setMedia - uri = {0}', uri));
                isMedia = uri.indexOf('//') > -1;
                if (isMedia) {
                    this.mCurrentUri = uri;
                }
                this.mDmr.setMediaChange(isMedia ? uri : 'hdvb://' + uri);
            }
        };
        cDLDMR.prototype.getCurrentUri = function () {
            return this.mCurrentUri;
        };
        // Event
        cDLDMR.prototype.onRequestPlay = function (item) {
            fnLog(hx.util.format('onRequestPlay - item = {0}', item));
            this.fireEvent('RequestPlay', {
                item: item
            });
        };
        cDLDMR.prototype.onRequestStop = function () {
            fnLog('onRequestStop');
            this.fireEvent('RequestStop');
        };
        cDLDMR.prototype.onRequestPause = function () {
            fnLog('onRequestPause');
            this.fireEvent('RequestPause');
        };
        cDLDMR.prototype.onRequestSetVolume = function () {
            fnLog('onRequestSetVolume');
            this.fireEvent('RequestSetVolume');
        };
        cDLDMR.prototype.onRequestMute = function () {
            fnLog('onRequestMute');
            this.fireEvent('RequestMute');
        };
        cDLDMR.prototype.onRequestBringIn = function () {
            fnLog('onRequestBringIn');
            this.fireEvent('RequestBringIn');
        };
        cDLDMR.prototype.onChannelChangeSucceeded = function (channel) {
            if (channel) {
                this.setMedia(channel.ccid);
            }
        };
        cDLDMR.prototype.onChannelChangeError = function (channel, errorState) {
            this.setMedia(null);
        };
        cDLDMR.prototype.onPlayStateChange = function (state, error) {
        };
        cDLDMR.prototype.onParentalRatingChange = function (contentID, rating, DRMSystemID, blocked) {
            var that = this;
            fnLog(hx.util.format('onParentalRatingChange - rating = {0}, blocked = {1}', rating, blocked));
            if (blocked) {
                this.setMedia(null);
            }
            else {
                var currentChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                if (currentChannel) {
                    this.setMedia(currentChannel.ccid);
                }
                else {
                    this.setMedia(null);
                }
            }
        };
        return cDLDMR;
    })(__KERNEL__);
    return cDLDMR;
});
