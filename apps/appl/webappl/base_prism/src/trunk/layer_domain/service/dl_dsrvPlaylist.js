/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvPlaylist.js
/*jslint sloppy:true, nomen:true, vars:true */
//    PLAYLIST: ['layer_domain/service/dl_dsrvPlaylist.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", "../../model/model"], function (require, exports, __KERNEL__, __MODEL__) {
    var cPlayListInfoList = (function (_super) {
        __extends(cPlayListInfoList, _super);
        function cPlayListInfoList(dService) {
            _super.call(this);
            this.dServcie = dService;
        }
        cPlayListInfoList.prototype.findItem = function (playlistItem) {
            var i;
            for (i = 0; i < this.length; i += 1) {
                if (this[i].isEqual(playlistItem)) {
                    return i;
                }
            }
            return -1;
        };
        cPlayListInfoList.prototype.findItemByTypeAndId = function (type, id) {
            var i, item;
            for (i = 0; i < this.length; i += 1) {
                item = this[i];
                if (item.getType() === type && item.getId() === id) {
                    return i;
                }
            }
            return -1;
        };
        cPlayListInfoList.prototype.add = function (playlistItem) {
            var index = this.findItem(playlistItem);
            if (index === -1) {
                this.unshift(playlistItem);
            }
            else {
                this.splice(index, 1);
                this.unshift(playlistItem);
            }
            var serialized = this.getSerialized();
            this.dServcie.saveData('playlist', serialized);
        };
        cPlayListInfoList.prototype.remove = function (type, id) {
            var index = this.findItemByTypeAndId(type, id);
            if (index !== -1) {
                this.splice(index, 1);
            }
        };
        cPlayListInfoList.prototype.getSerialized = function () {
            var serialized = [], i;
            var item;
            for (i = 0; i < this.length; i += 1) {
                item = this[i].serialize();
                serialized.push(item);
            }
            return serialized;
        };
        return cPlayListInfoList;
    })(__MODEL__.cArray);
    var cDLPlayList = (function (_super) {
        __extends(cDLPlayList, _super);
        function cDLPlayList(aServiceName) {
            _super.call(this, aServiceName);
            hx.logadd('cDLPlayList');
            hx.log('cDLPlayList', 'create');
            this.playlistInfoList = new cPlayListInfoList(this);
            this.fnRestoreFromLocalStorage();
        }
        cDLPlayList.prototype.fnStoreToLocalStorage = function () {
            this.saveData('playlist', this.playlistInfoList.getSerialized());
        };
        cDLPlayList.prototype.fnRestoreFromLocalStorage = function () {
            var data = this.readData('playlist');
            if (data) {
                this.playlistInfoList.length = 0;
                for (var i = 0, length = data.length, item = null; i < length; i += 1) {
                    item = data[i];
                    switch (item.type) {
                        case __MODEL__.THistoryType.Media:
                            this.playlistInfoList.push((new __MODEL__.cMediaPlaybackPlaylistItem(null)).deSerialize(item));
                            break;
                        default:
                            break;
                    }
                }
            }
        };
        // API
        cDLPlayList.prototype.add = function (param) {
            var item = param.item;
            this.playlistInfoList.add(new __MODEL__.cMediaPlaybackPlaylistItem(item));
        };
        cDLPlayList.prototype.remove = function (param) {
            var type = param.type, id = param.id;
            this.playlistInfoList.remove(type, id);
        };
        cDLPlayList.prototype.getPlaylist = function (contentType) {
            var playlist = [];
            var length = this.playlistInfoList.length;
            for (var i = 0; i < length; i += 1) {
                var info = this.playlistInfoList[i];
                if (info.getContentType() === contentType) {
                    playlist.push(info);
                }
            }
            return playlist;
        };
        return cDLPlayList;
    })(__KERNEL__);
    return cDLPlayList;
});
