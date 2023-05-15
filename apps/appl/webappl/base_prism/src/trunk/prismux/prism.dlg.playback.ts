/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.playback.js
/*jslint sloppy:true, nomen:true, vars:true */
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

declare var hx: any;
module mPrism {
    export class CPlaybackDlg extends __PRISM_CDIALOG__.CDialog {
        _playSpeedsArray: number[] = [-64, -32, -16, -8, -4, -2, 0, 1, 2, 4, 8, 16, 32, 64];
        _trickRestrictMode = null;
        _trickEnabled: boolean = true;
        _playbackService: any;
        _skipForwardTime: number;
        _skipRewindTime: number;
        _keyPressed: boolean;
        _playItem: any;
        _nextPlayItem: any;
        _operationEnabled: boolean = false;
        constructor() {
            super();
        }
        setPlaybackService(playbackService) {
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
        }
        setOperationEnabled(operationEnabled) {
            this._operationEnabled = operationEnabled;
        }
        getPlaybackService(playbackService) {
            return this._playbackService;
        }
        setSkipForwardTime(skipForwardTime) {
            this._skipForwardTime = skipForwardTime;
        }
        setSkipRewindTime(skipRewindTime) {
            this._skipRewindTime = skipRewindTime;
        }
        setPlaySpeedsArray(playSpeedsArray) {
            this._playSpeedsArray = playSpeedsArray || [];
        }
        setTrickRestrictMode(trickMode) {
            this._trickRestrictMode = trickMode;
        }
        setTrickEnabled(trickEnabled) {
            this._trickEnabled = trickEnabled;
        }
        getNextPlayItem(aItem?) {
            return this._nextPlayItem;
        }
        play(item?, isLastPosition?) {
            var a = this;

            if (item) {
                a._playItem = item;
            }
            if (!a.doPlay(item)) {
                if (a._playbackService) {
                    //a._playbackService.stop();
                    a._playbackService.play({
                        item : item || a._playItem,
                        isLastPosition : isLastPosition
                    });
                }
            }
        }
        doPlay(aItem) {
            return false;
        }
        resume() {
            var a = this;
            if (!a._operationEnabled) {
                return;
            }

            a.doResume();
            if (a._playbackService) {
                a._playbackService.resume();
            }

        }
        doResume() {

        }
        pause() {
            var a = this;
            if (!a._operationEnabled) {
                return;
            }
            a.setSpeed(0);
            a.doPause();
            if (a._playbackService) {
                a._playbackService.pause();
            }
        }
        setSpeed(aSpeed: number) {

        }
        doPause() {

        }
        stop() {
            var a = this;
            a.doStop();
            if (a._playbackService) {
                a._playbackService.stop();
            }
        }
        stopBGM() {
            var a = this;
            if (a._playbackService) {
                a._playbackService.stop();
            }
        }
        doStop() {

        }
        next() {
            var a = this;
            a.doNext();
        }
        doNext() {

        }
        prev() {
            var a = this;
            a.doPrev();
        }
        doPrev() {

        }
        skipForward() {
            var a = this;
            a.doSkipForward(a._skipForwardTime, a._trickRestrictMode);
        }
        doSkipForward(aSkipForwardTime, aTrickRestrictMode) {

        }
        skipRewind() {
            var a = this;
            a.doSkipRewind(a._skipRewindTime, a._trickRestrictMode);
        }
        doSkipRewind(aSkipForwardTime, aTrickRestrictMode) {

        }
        fastForward() {
            var a = this;
            if (!a._operationEnabled) {
                return true;
            }
            var bIsFastPlay:boolean = true;
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

        }
        isFastPlayAvailable () : boolean {
           return   false;
        }
        createFastPlayRestrictMsg () {

        }
        doFastForward(aPlaySpeedsArray, aTrickRestrictMode) {

        }
        fastRewind(item?) {
            var a = this;

            if (!a._operationEnabled) {
                return true;
            }

            var bIsFastPlay:boolean = true;
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
        }
        doFastRewind(aPlaySpeedsArray, aTrickRestrictMode) {

        }
        openTvOption(offset) {
            var a = this;
            if (a.doOpenTvOption) {
                a.doOpenTvOption();
            }
        }
        doOpenTvOption() {

        }
        requestSeek(offset) {
            var a = this;

            if (a._playbackService) {
                a._playbackService.seek({
                    offset : offset
                });
            }
        }
        requestGetSpeed() {
            var a = this;
            if (a._playbackService) {
                return a._playbackService.getSpeed();
            }
            return 1;
        }
        requestSetSpeed(speed) {
            var a = this;
            if (a._playbackService) {
                a._playbackService.setSpeed({
                    speed : speed
                });
            }
        }
        doKeyPlay() {
            var a = this;
            var shown = a.isShown();
            if (!shown) {
                a.show();
            }
            this.resume();
            return true;
        }
        doKeyPause() {
            var a = this;
            var shown = a.isShown();
            if (!shown) {
                a.show();
            }
            this.pause();
            return true;
        }
        doKeyStop() {
            var a = this;
            var shown = a.isShown();
            if (!shown) {
                a.show();
            }
            this.stop();
            return true;
        }
        doKeyPlaySpeedUp() {
            var a = this;
            var shown = a.isShown();
            if (!shown) {
                a.show();
            }
            return this.fastForward();
        }
        doKeyPlaySpeedDown() {
            var a = this;
            var shown = a.isShown();
            if (!shown) {
                a.show();
            }
            return this.fastRewind();
        }
        doKeyRewind() {
            return this.doKeyPlaySpeedDown();
        }
        doKeyFastFwd() {
            return this.doKeyPlaySpeedUp();
        }
        onKeyUp() {
            this._keyPressed = false;
        }
        moveToCurrentPosition() {
            var a = this;
        }

        doGetVideoOption: () => any;
        doGetSupportedSubtitles: () => any[];
        doGetSubtitleFontSize: () => number;
        doGetSubtitleSyncTime: () => number;
        doGetSubtitlePosition: () => number;
        doGetSubtitleDisplay: () => any;
        doGetSubtitleLanguage: () => string;
        doGetSeriesRepeat: () => boolean;
        doSetSubtitleOption: (aParam: any) => number;
        onOpenNotAvailableMsg: () => any;
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.CPlaybackDlg = CPlaybackDlg;
}
export = mPrism;