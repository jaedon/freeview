var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg'], function (require, exports, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var CPlaybackDlg = (function (_super) {
            __extends(CPlaybackDlg, _super);
            function CPlaybackDlg() {
                _super.call(this);
                this._playSpeedsArray = [-64, -32, -16, -8, -4, -2, 0, 1, 2, 4, 8, 16, 32, 64];
                this._trickRestrictMode = null;
                this._trickEnabled = true;
                this._operationEnabled = false;
            }
            CPlaybackDlg.prototype.setPlaybackService = function (playbackService) {
                hx.log("warn" + "#####");
                hx.log("warn", "is engine TSR?: " + (playbackService instanceof hx.svc.TSR.constructor));
                hx.log("debug", "set playback engine: " + (playbackService instanceof hx.svc.PLAYBACK.constructor));
                hx.log("debug", "set tsr engine: " + (playbackService instanceof hx.svc.TSR.constructor));
                hx.log("warn" + arguments.callee);
                var a = this;
                if (a._playbackService) {
                    a._playbackService.clearTimer();
                }
                this._playbackService = playbackService;
            };
            CPlaybackDlg.prototype.setOperationEnabled = function (operationEnabled) {
                this._operationEnabled = operationEnabled;
            };
            CPlaybackDlg.prototype.getPlaybackService = function (playbackService) {
                return this._playbackService;
            };
            CPlaybackDlg.prototype.setSkipForwardTime = function (skipForwardTime) {
                this._skipForwardTime = skipForwardTime;
            };
            CPlaybackDlg.prototype.setSkipRewindTime = function (skipRewindTime) {
                this._skipRewindTime = skipRewindTime;
            };
            CPlaybackDlg.prototype.setPlaySpeedsArray = function (playSpeedsArray) {
                this._playSpeedsArray = playSpeedsArray || [];
            };
            CPlaybackDlg.prototype.setTrickRestrictMode = function (trickMode) {
                this._trickRestrictMode = trickMode;
            };
            CPlaybackDlg.prototype.setTrickEnabled = function (trickEnabled) {
                this._trickEnabled = trickEnabled;
            };
            CPlaybackDlg.prototype.getNextPlayItem = function (aItem) {
                return this._nextPlayItem;
            };
            CPlaybackDlg.prototype.play = function (item, isLastPosition) {
                var a = this;
                if (item) {
                    a._playItem = item;
                }
                if (!a.doPlay(item)) {
                    if (a._playbackService) {
                        //a._playbackService.stop();
                        a._playbackService.play({
                            item: item || a._playItem,
                            isLastPosition: isLastPosition
                        });
                    }
                }
            };
            CPlaybackDlg.prototype.doPlay = function (aItem) {
                return false;
            };
            CPlaybackDlg.prototype.resume = function () {
                var a = this;
                if (!a._operationEnabled) {
                    return;
                }
                a.doResume();
                if (a._playbackService) {
                    a._playbackService.resume();
                }
            };
            CPlaybackDlg.prototype.doResume = function () {
            };
            CPlaybackDlg.prototype.pause = function () {
                var a = this;
                if (!a._operationEnabled) {
                    return;
                }
                a.setSpeed(0);
                a.doPause();
                if (a._playbackService) {
                    a._playbackService.pause();
                }
            };
            CPlaybackDlg.prototype.setSpeed = function (aSpeed) {
            };
            CPlaybackDlg.prototype.doPause = function () {
            };
            CPlaybackDlg.prototype.stop = function () {
                var a = this;
                a.doStop();
                if (a._playbackService) {
                    a._playbackService.stop();
                }
            };
            CPlaybackDlg.prototype.stopBGM = function () {
                var a = this;
                if (a._playbackService) {
                    a._playbackService.stop();
                }
            };
            CPlaybackDlg.prototype.doStop = function () {
            };
            CPlaybackDlg.prototype.next = function () {
                var a = this;
                a.doNext();
            };
            CPlaybackDlg.prototype.doNext = function () {
            };
            CPlaybackDlg.prototype.prev = function () {
                var a = this;
                a.doPrev();
            };
            CPlaybackDlg.prototype.doPrev = function () {
            };
            CPlaybackDlg.prototype.skipForward = function () {
                var a = this;
                a.doSkipForward(a._skipForwardTime, a._trickRestrictMode);
            };
            CPlaybackDlg.prototype.doSkipForward = function (aSkipForwardTime, aTrickRestrictMode) {
            };
            CPlaybackDlg.prototype.skipRewind = function () {
                var a = this;
                a.doSkipRewind(a._skipRewindTime, a._trickRestrictMode);
            };
            CPlaybackDlg.prototype.doSkipRewind = function (aSkipForwardTime, aTrickRestrictMode) {
            };
            CPlaybackDlg.prototype.fastForward = function () {
                var a = this;
                if (!a._operationEnabled) {
                    return true;
                }
                var bIsFastPlay = true;
                var casIds, item = a._playItem;
                if (item && item.getCasIDs) {
                    casIds = item.getCasIDs();
                    if (casIds.length > 0) {
                        return false;
                    }
                }
                bIsFastPlay = a.isFastPlayAvailable();
                if (bIsFastPlay === false) {
                    a.createFastPlayRestrictMsg();
                    return true;
                }
                a.doFastForward(a._playSpeedsArray, a._trickRestrictMode);
                return true;
            };
            CPlaybackDlg.prototype.isFastPlayAvailable = function () {
                return false;
            };
            CPlaybackDlg.prototype.createFastPlayRestrictMsg = function () {
            };
            CPlaybackDlg.prototype.doFastForward = function (aPlaySpeedsArray, aTrickRestrictMode) {
            };
            CPlaybackDlg.prototype.fastRewind = function (item) {
                var a = this;
                if (!a._operationEnabled) {
                    return true;
                }
                var bIsFastPlay = true;
                var casIds;
                a._playItem = item || a._playItem;
                if (a._playItem && a._playItem.getCasIDs) {
                    casIds = a._playItem.getCasIDs();
                    if (casIds.length > 0) {
                        return false;
                    }
                }
                bIsFastPlay = a.isFastPlayAvailable();
                if (bIsFastPlay === false) {
                    a.createFastPlayRestrictMsg();
                    return true;
                }
                a.doFastRewind(a._playSpeedsArray, a._trickRestrictMode);
                return true;
            };
            CPlaybackDlg.prototype.doFastRewind = function (aPlaySpeedsArray, aTrickRestrictMode) {
            };
            CPlaybackDlg.prototype.openTvOption = function (offset) {
                var a = this;
                if (a.doOpenTvOption) {
                    a.doOpenTvOption();
                }
            };
            CPlaybackDlg.prototype.doOpenTvOption = function () {
            };
            CPlaybackDlg.prototype.requestSeek = function (offset) {
                var a = this;
                if (a._playbackService) {
                    a._playbackService.seek({
                        offset: offset
                    });
                }
            };
            CPlaybackDlg.prototype.requestGetSpeed = function () {
                var a = this;
                if (a._playbackService) {
                    return a._playbackService.getSpeed();
                }
                return 1;
            };
            CPlaybackDlg.prototype.requestSetSpeed = function (speed) {
                var a = this;
                if (a._playbackService) {
                    a._playbackService.setSpeed({
                        speed: speed
                    });
                }
            };
            CPlaybackDlg.prototype.doKeyPlay = function () {
                var a = this;
                var shown = a.isShown();
                if (!shown) {
                    a.show();
                }
                this.resume();
                return true;
            };
            CPlaybackDlg.prototype.doKeyPause = function () {
                var a = this;
                var shown = a.isShown();
                if (!shown) {
                    a.show();
                }
                this.pause();
                return true;
            };
            CPlaybackDlg.prototype.doKeyStop = function () {
                var a = this;
                var shown = a.isShown();
                if (!shown) {
                    a.show();
                }
                this.stop();
                return true;
            };
            CPlaybackDlg.prototype.doKeyPlaySpeedUp = function () {
                var a = this;
                var shown = a.isShown();
                if (!shown) {
                    a.show();
                }
                return this.fastForward();
            };
            CPlaybackDlg.prototype.doKeyPlaySpeedDown = function () {
                var a = this;
                var shown = a.isShown();
                if (!shown) {
                    a.show();
                }
                return this.fastRewind();
            };
            CPlaybackDlg.prototype.doKeyRewind = function () {
                return this.doKeyPlaySpeedDown();
            };
            CPlaybackDlg.prototype.doKeyFastFwd = function () {
                return this.doKeyPlaySpeedUp();
            };
            CPlaybackDlg.prototype.onKeyUp = function () {
                this._keyPressed = false;
            };
            CPlaybackDlg.prototype.moveToCurrentPosition = function () {
                var a = this;
            };
            return CPlaybackDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CPlaybackDlg = CPlaybackDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.CPlaybackDlg = CPlaybackDlg;
    })(mPrism || (mPrism = {}));
    return mPrism;
});
