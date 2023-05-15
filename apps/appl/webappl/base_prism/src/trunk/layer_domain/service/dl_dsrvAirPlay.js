/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvAirPlay.js
/*jslint nomen:true, vars:true */
//    AIR_PLAY: ['layer_domain/service/dl_dsrvAirPlay.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "../../model/model"], function (require, exports, __KERNEL__, __MODEL__) {
    var fnLog = function (log) {
        hx.log('dsrvAirPlay', log);
    };
    var fnSetBufferdRange = function (bufferdRange) {
        fnLog(hx.util.format('fnSetBufferdRange - bufferdRange: {0}', bufferdRange));
    };
    var fnSeekableRange = function (seekableRange) {
        fnLog(hx.util.format('fnSeekableRange - seekableRange: {0}', seekableRange));
    };
    var cDLAirPlay = (function (_super) {
        __extends(cDLAirPlay, _super);
        function cDLAirPlay(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mAirPlay = this.getPluginInstance('AirPlay');
            hx.logadd('dsrvAirPlay');
            hx.log('dsrvAirPlay', 'create');
            this.defineEvents(['RequestPlay', 'RequestStop', 'RequestPause', 'RequestResume', 'RequestSetPosition', 'RequestSetSpeed', 'RequestSetVolume', 'MusicInfoChanged', 'AlbumImgChanged']);
            this.mAirPlay.addEventListener('ReqPlay', function (e) {
                hx.log('debug', 'ReqPlay');
                _this.onRequestPlay(e.url, e.decimal);
            });
            this.mAirPlay.addEventListener('ReqStop', function (e) {
                hx.log('debug', 'ReqStop');
                _this.onRequestStop();
            });
            this.mAirPlay.addEventListener('ReqSetPosition', function (e) {
                hx.log('debug', 'ReqSetPosition');
                _this.onRequestSetPosition(e.decimal);
            });
            this.mAirPlay.addEventListener('ReqSetSpeed', function (e) {
                hx.log('debug', 'ReqSetSpeed');
                _this.onRequestSetSpeed(e.decimal);
            });
            this.mAirPlay.addEventListener('ReqShowPhoto', function (e) {
                hx.log('debug', 'ReqShowPhoto');
                _this.onRequestShowPhoto(e.path);
            });
            this.mAirPlay.addEventListener('ReqAudioPlay', function (e) {
                hx.log('debug', 'ReqAudioPlay');
                _this.onRequestAudioPlay(e.url);
            });
            this.mAirPlay.addEventListener('ReqAudioStop', function (e) {
                hx.log('debug', 'ReqAudioStop');
                _this.onRequestStop();
            });
            this.mAirPlay.addEventListener('ReqAudioFlush', function (e) {
                hx.log('debug', 'ReqAudioFlush');
                _this.onRequestAudioFlush();
            });
            this.mAirPlay.addEventListener('ReqSetMetadata', function (e) {
                hx.log('debug', 'ReqSetMetadata');
                _this.onRequestSetMetadata(e.title, e.album, e.artist);
            });
            this.mAirPlay.addEventListener('ReqSetAlbumart', function (e) {
                hx.log('debug', 'ReqSetAlbumart');
                _this.onRequestSetAlbumart(e.path);
            });
            this.mAirPlay.addEventListener('ReqSetVolume', function (e) {
                hx.log('debug', 'ReqSetVolume');
                _this.onRequestSetVolume(e.decimal);
            });
            this.mAirPlay.start();
        }
        // API
        cDLAirPlay.prototype.start = function (param) {
            fnLog('fnStart');
            this.mAirPlay.start();
        };
        cDLAirPlay.prototype.stop = function (param) {
            fnLog('fnStop');
            this.mAirPlay.stop();
        };
        cDLAirPlay.prototype.setDuration = function (param) {
            var duration = param.duration;
            fnLog(hx.util.format('fnSetDuration - duration: {0}', duration));
            this.mAirPlay.Duration = duration;
        };
        cDLAirPlay.prototype.setPosition = function (param) {
            var position = param.position;
            fnLog(hx.util.format('fnSetPosition - position: {0}', position));
            this.mAirPlay.Position = position * 1000;
        };
        cDLAirPlay.prototype.setRate = function (param) {
            var rate = param.rate;
            fnLog(hx.util.format('fnSetRate - rate: {0}', rate));
            this.mAirPlay.Rate = rate;
        };
        cDLAirPlay.prototype.setPlaybackStatus = function (param) {
            var playbackStatus = param.playbackStatus;
            fnLog(hx.util.format('fnSetPlaybackStatus - playbackStatus: {0}', playbackStatus));
            this.mAirPlay.PlaybackStatus = playbackStatus;
        };
        // EVENT
        cDLAirPlay.prototype.onRequestPlay = function (url, startPosition) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onRequestPlay] - url = {0}', url));
            fnLog(hx.util.format('[onRequestPlay] - startPosition = {0}', startPosition));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            var item = new __MODEL__.cAirPlayItem(url, __MODEL__.IMediaItem.TContentType.EVideo);
            this.fireEvent('RequestPlay', {
                item: item
            });
        };
        cDLAirPlay.prototype.onRequestStop = function () {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[onRequestStop]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('RequestStop');
        };
        cDLAirPlay.prototype.onRequestSetPosition = function (position) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onRequestSetPosition] - position = {0}', position));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('RequestSetPosition', {
                position: Math.floor(position / 1000)
            });
        };
        cDLAirPlay.prototype.onRequestSetSpeed = function (speed) {
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
        };
        cDLAirPlay.prototype.onRequestShowPhoto = function (path) {
            path = 'file://' + path;
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onRequestShowPhoto] - path = {0}', path));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            var item = new __MODEL__.cAirPlayItem(path, __MODEL__.IMediaItem.TContentType.EPhoto);
            this.fireEvent('RequestPlay', {
                item: item
            });
        };
        cDLAirPlay.prototype.onRequestAudioPlay = function (url) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onRequestAudioPlay] - url = {0}', url));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            var item = new __MODEL__.cAirPlayItem(url, __MODEL__.IMediaItem.TContentType.EMusic);
            this.fireEvent('RequestPlay', {
                item: item
            });
        };
        cDLAirPlay.prototype.onRequestAudioFlush = function () {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('[onRequestAudioFlush]');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
        };
        cDLAirPlay.prototype.onRequestSetMetadata = function (title, album, artist) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onRequestSetMetadata] - title = {0}', title));
            fnLog(hx.util.format('[onRequestSetMetadata] - album = {0}', album));
            fnLog(hx.util.format('[onRequestSetMetadata] - artist = {0}', artist));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('MusicInfoChanged', {
                title: title,
                album: album,
                artist: artist
            });
        };
        cDLAirPlay.prototype.onRequestSetAlbumart = function (path) {
            path = 'file://' + path;
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onRequestSetAlbumart] - path = {0}', path));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
            this.fireEvent('AlbumImgChanged', {
                path: path
            });
        };
        cDLAirPlay.prototype.onRequestSetVolume = function (volume) {
            fnLog('####################################################################################################');
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog(hx.util.format('[onRequestSetVolume] - volume = {0}', volume));
            fnLog('----------------------------------------------------------------------------------------------------');
            fnLog('####################################################################################################');
        };
        return cDLAirPlay;
    })(__KERNEL__);
    return cDLAirPlay;
});
