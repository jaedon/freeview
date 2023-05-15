/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvAirPlay.js
/*jslint nomen:true, vars:true */
//    AIR_PLAY: ['layer_domain/service/dl_dsrvAirPlay.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");

var fnLog = function(log) {
    hx.log('dsrvAirPlay', log);
};
var fnSetBufferdRange = function(bufferdRange) {
    fnLog(hx.util.format('fnSetBufferdRange - bufferdRange: {0}', bufferdRange));
};
var fnSeekableRange = function(seekableRange) {
    fnLog(hx.util.format('fnSeekableRange - seekableRange: {0}', seekableRange));
};

class cDLAirPlay extends __KERNEL__ implements ifSVCAirPlay {
    mAirPlay = this.getPluginInstance('AirPlay');
    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvAirPlay');
        hx.log('dsrvAirPlay', 'create');
        this.defineEvents(['RequestPlay', 'RequestStop', 'RequestPause', 'RequestResume', 'RequestSetPosition', 'RequestSetSpeed', 'RequestSetVolume', 'MusicInfoChanged', 'AlbumImgChanged']);
        this.mAirPlay.addEventListener('ReqPlay', (e) => {
            hx.log('debug', 'ReqPlay');
            this.onRequestPlay(e.url, e.decimal);
        });
        this.mAirPlay.addEventListener('ReqStop', (e) => {
            hx.log('debug', 'ReqStop');
            this.onRequestStop();
        });
        this.mAirPlay.addEventListener('ReqSetPosition', (e) => {
            hx.log('debug', 'ReqSetPosition');
            this.onRequestSetPosition(e.decimal);
        });
        this.mAirPlay.addEventListener('ReqSetSpeed', (e) => {
            hx.log('debug', 'ReqSetSpeed');
            this.onRequestSetSpeed(e.decimal);
        });
        this.mAirPlay.addEventListener('ReqShowPhoto', (e) => {
            hx.log('debug', 'ReqShowPhoto');
            this.onRequestShowPhoto(e.path);
        });
        this.mAirPlay.addEventListener('ReqAudioPlay', (e) => {
            hx.log('debug', 'ReqAudioPlay');
            this.onRequestAudioPlay(e.url);
        });
        this.mAirPlay.addEventListener('ReqAudioStop', (e) => {
            hx.log('debug', 'ReqAudioStop');
            this.onRequestStop();
        });
        this.mAirPlay.addEventListener('ReqAudioFlush', (e) => {
            hx.log('debug', 'ReqAudioFlush');
            this.onRequestAudioFlush();
        });
        this.mAirPlay.addEventListener('ReqSetMetadata', (e) => {
            hx.log('debug', 'ReqSetMetadata');
            this.onRequestSetMetadata(e.title, e.album, e.artist);
        });
        this.mAirPlay.addEventListener('ReqSetAlbumart', (e) => {
            hx.log('debug', 'ReqSetAlbumart');
            this.onRequestSetAlbumart(e.path);
        });
        this.mAirPlay.addEventListener('ReqSetVolume', (e) => {
            hx.log('debug', 'ReqSetVolume');
            this.onRequestSetVolume(e.decimal);
        });
        this.mAirPlay.start();
    }
    // API
    start(param): void {
        fnLog('fnStart');
        this.mAirPlay.start();
    }
    stop(param): void {
        fnLog('fnStop');
        this.mAirPlay.stop();
    }
    setDuration(param): void {
        var duration = param.duration;
        fnLog(hx.util.format('fnSetDuration - duration: {0}', duration));
        this.mAirPlay.Duration = duration;
    }
    setPosition(param): void {
        var position = param.position;
        fnLog(hx.util.format('fnSetPosition - position: {0}', position));
        this.mAirPlay.Position = position * 1000;
    }
    setRate(param): void {
        var rate = param.rate;
        fnLog(hx.util.format('fnSetRate - rate: {0}', rate));
        this.mAirPlay.Rate = rate;
    }
    setPlaybackStatus(param): void {
        var playbackStatus = param.playbackStatus;
        fnLog(hx.util.format('fnSetPlaybackStatus - playbackStatus: {0}', playbackStatus));
        this.mAirPlay.PlaybackStatus = playbackStatus;
    }
    // EVENT
    onRequestPlay(url, startPosition) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onRequestPlay] - url = {0}', url));
        fnLog(hx.util.format('[onRequestPlay] - startPosition = {0}', startPosition));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        var item = new __MODEL__.cAirPlayItem(url, __MODEL__.IMediaItem.TContentType.EVideo);
        this.fireEvent('RequestPlay', {
            item : item
        });
    }
    onRequestStop() {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[onRequestStop]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('RequestStop');
    }
    onRequestSetPosition(position) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onRequestSetPosition] - position = {0}', position));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('RequestSetPosition', {
            position : Math.floor(position / 1000)
        });
    }
    onRequestSetSpeed(speed) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onRequestSetSpeed] - speed = {0}', speed));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        if (speed === 100) {
            this.fireEvent('RequestResume');
        }
        else if (speed === 0) {
            this.fireEvent('RequestPause');
        }
    }
    onRequestShowPhoto(path) {
        path = 'file://' + path;
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onRequestShowPhoto] - path = {0}', path));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        var item = new __MODEL__.cAirPlayItem(path, __MODEL__.IMediaItem.TContentType.EPhoto);
        this.fireEvent('RequestPlay', {
            item : item
        });
    }
    onRequestAudioPlay(url) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onRequestAudioPlay] - url = {0}', url));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        var item = new __MODEL__.cAirPlayItem(url, __MODEL__.IMediaItem.TContentType.EMusic);
        this.fireEvent('RequestPlay', {
            item : item
        });
    }
    onRequestAudioFlush() {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('[onRequestAudioFlush]');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
    }
    onRequestSetMetadata(title, album, artist) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onRequestSetMetadata] - title = {0}', title));
        fnLog(hx.util.format('[onRequestSetMetadata] - album = {0}', album));
        fnLog(hx.util.format('[onRequestSetMetadata] - artist = {0}', artist));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('MusicInfoChanged', {
            title : title,
            album : album,
            artist : artist
        });
    }
    onRequestSetAlbumart(path) {
        path = 'file://' + path;
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onRequestSetAlbumart] - path = {0}', path));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
        this.fireEvent('AlbumImgChanged', {
            path : path
        });
    }
    onRequestSetVolume(volume) {
        fnLog('####################################################################################################');
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog(hx.util.format('[onRequestSetVolume] - volume = {0}', volume));
        fnLog('----------------------------------------------------------------------------------------------------');
        fnLog('####################################################################################################');
    }
}
export = cDLAirPlay;