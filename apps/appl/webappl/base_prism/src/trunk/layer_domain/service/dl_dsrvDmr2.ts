/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvDmr2.js
/*jslint nomen:true, vars:true */
//    DMR: ['layer_domain/service/dl_dsrvDmr2.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

var fnLog = function(log) {
    hx.log('dsrvDmr2', '##################################################');
    hx.log('dsrvDmr2', '#####' + log);
    hx.log('dsrvDmr2', '##################################################');
};

class cDLDMR extends __KERNEL__ implements ifSVCDmr {
    private mVideoBroadcast = this.getPluginInstance('VideoBroadcast');
    private mDmr = this.getPluginInstance('DLNA.DMR');
    private mCurrentUri = null;

    constructor(aServiceName: string) {
        super(aServiceName);
        // hx.logadd('dsrvDmr2');
        this.defineEvents(['RequestPlay', 'RequestStop', 'RequestPause', 'RequestSetVolume', 'RequestMute', 'RequestBringIn']);
        this.mDmr.addEventListener('ReqPlay', (e) => {
            this.onRequestPlay(e.item);
        });
        this.mDmr.addEventListener('ReqStop', (e) => {
            this.onRequestStop();
        });
        this.mDmr.addEventListener('ReqPause', (e) => {
            this.onRequestPause();
        });
        this.mDmr.addEventListener('ReqSetVolume', (e) => {
            this.onRequestSetVolume();
        });
        this.mDmr.addEventListener('ReqMute', (e) => {
            this.onRequestMute();
        });
        this.mDmr.addEventListener('ReqBringIn', (e) => {
            this.onRequestBringIn();
        });
        this.mVideoBroadcast.addEventListener('ChannelChangeSucceeded', (e) => {
            this.onChannelChangeSucceeded(e.channel);
        });
        this.mVideoBroadcast.addEventListener('ChannelChangeError', (e) => {
            this.onChannelChangeError(e.channel, e.errorState);
        });
        this.mVideoBroadcast.addEventListener('PlayStateChange', (e) => {
            this.onPlayStateChange(e.state, e.error);
        });
        this.mVideoBroadcast.addEventListener('ParentalRatingChange', (e) => {
            this.onParentalRatingChange(e.contentID, e.rating, e.DRMSystemID, e.blocked);
        });
    }
    start(): void {
        fnLog('fnStart');
        this.mDmr.start();
    }
    stop(): void {
        fnLog('fnStop');
        this.mDmr.stop();
    }
    isStarted(): boolean {
        var started = this.mDmr.started;
        fnLog(hx.util.format('fnIsStarted - started = {0}', started));
        return started;
    }
    isActivated(): boolean {
        var activated = this.mDmr.activated;
        fnLog(hx.util.format('fnIsActivated - activaged = {0}', activated));
        return activated;
    }
    setMedia(uri: string): void {
        var isMedia = false;
        this.mCurrentUri = null;
        if (uri === null) {
            fnLog('fnIsActivated - uri = null !!!!!');
            this.mDmr.setMediaChange(uri);
        } else  if (typeof uri !== "string") {
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
    }
    getCurrentUri(): string {
        return this.mCurrentUri;
    }

    // Event
    onRequestPlay(item) {
        fnLog(hx.util.format('onRequestPlay - item = {0}', item));
        this.fireEvent('RequestPlay', {
            item : item
        });
    }
    onRequestStop() {
        fnLog('onRequestStop');
        this.fireEvent('RequestStop');
    }
    onRequestPause() {
        fnLog('onRequestPause');
        this.fireEvent('RequestPause');
    }
    onRequestSetVolume() {
        fnLog('onRequestSetVolume');
        this.fireEvent('RequestSetVolume');
    }
    onRequestMute() {
        fnLog('onRequestMute');
        this.fireEvent('RequestMute');
    }
    onRequestBringIn() {
        fnLog('onRequestBringIn');
        this.fireEvent('RequestBringIn');
    }
    onChannelChangeSucceeded(channel) {
        if (channel) {
            this.setMedia(channel.ccid);
        }
    }
    onChannelChangeError(channel, errorState) {
        this.setMedia(null);
    }
    onPlayStateChange(state, error) {

    }
    onParentalRatingChange(contentID, rating, DRMSystemID, blocked) {
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
    }
}
export = cDLDMR;
