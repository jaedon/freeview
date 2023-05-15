/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvBridge.js
/*jslint nomen:true, vars:true */
//BRIDGE: ['layer_domain/service/dl_dsrvBridge.js'],
declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");

var mPlaybackEnabled = true;
var mTicket = '';

/**************************************************
 * Events
 **************************************************/
class cDLBridge extends __KERNEL__ implements ifSVCBridge {
    constructor(aServiceName: string) {
        super(aServiceName);
        this.defineEvents(['RequestMusicPlayback', 'RequestVideoPlayback', 'RequestPhotoPlayback', 'RequestInstanceRecording', 'RequestStopPlayback']);
    }
    requestPlay(aItems: any[], aStartIndex: number): void {
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
    }
    requestMusicPlayback(aItems: any[], aStartIndex: number): void {
        this.fireEvent('RequestMusicPlayback', {
            items : aItems,
            startIndex : aStartIndex
        });
    }
    requestVideoPlayback(aItems: any[], aStartIndex: number, aKeyCode?: number): void {
        this.require('HISTORY', (dsrvHistory) => {
            var mediaItem = dsrvHistory.findMediaItem({
                id: aItems[0].getFullPath()
            });
            var lastPlayPosition = mediaItem ? mediaItem.getLastPlayPosition() : 0;
            if (lastPlayPosition > 0) {
                aItems[0] = mediaItem;
            }
            this.fireEvent('RequestVideoPlayback', {
                items: aItems,
                startIndex: aStartIndex,
                lastPlayPosition: lastPlayPosition,
                keyCode: aKeyCode
            });
        });
    }
    requestPhotoPlayback(aItem, aStartIndex: number): void {
        this.fireEvent('RequestPhotoPlayback', {
            items : aItem,
            startIndex : aStartIndex
        });
    }
    requestInstanceRecording(aChannel): void {
        this.fireEvent('RequestInstanceRecording', {
            channel : aChannel
        });
    }
    requestStopPlayback(): void {
        this.fireEvent('RequestStopPlayback');
    }
    enablePlayback(aEnabled: boolean): void {
        mPlaybackEnabled = aEnabled;
    }
    isPlayEnabled(): boolean {
        return mPlaybackEnabled;
    }
    setTicket(aTicket): void {
        mTicket = aTicket;
    }
    getTicket(): any {
        return mTicket;
    }
}
export = cDLBridge;
