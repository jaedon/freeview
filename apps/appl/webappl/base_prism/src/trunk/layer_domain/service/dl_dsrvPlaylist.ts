/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvPlaylist.js
/*jslint sloppy:true, nomen:true, vars:true */
//    PLAYLIST: ['layer_domain/service/dl_dsrvPlaylist.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");

class cPlayListInfoList extends __MODEL__.cArray<any> {
    dServcie;
    constructor(dService) {
        super();
        this.dServcie = dService;
    }
    findItem(playlistItem) {
        var i;
        for ( i = 0; i < this.length; i += 1) {
            if (this[i].isEqual(playlistItem)) {
                return i;
            }
        }
        return -1;
    }
    findItemByTypeAndId(type, id) {
        var i, item;
        for ( i = 0; i < this.length; i += 1) {
            item = this[i];
            if (item.getType() === type && item.getId() === id) {
                return i;
            }
        }
        return -1;
    }
    add(playlistItem) {
        var index = this.findItem(playlistItem);
        if (index === -1) {
            this.unshift(playlistItem);
        } else {
            this.splice(index, 1);
            this.unshift(playlistItem);
        }
        var serialized = this.getSerialized();
        this.dServcie.saveData('playlist', serialized);
    }
    remove(type, id) {
        var index = this.findItemByTypeAndId(type, id);
        if (index !== -1) {
            this.splice(index, 1);
        }
    }
    getSerialized() {
        var serialized = [], i;
        var item;
        for ( i = 0; i < this.length; i += 1) {
            item = this[i].serialize();
            serialized.push(item);
        }
        return serialized;
    }
}
class cDLPlayList extends __KERNEL__ implements ifSVCPlaylist {
    playlistInfoList;
    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('cDLPlayList');
        hx.log('cDLPlayList', 'create');
        this.playlistInfoList = new cPlayListInfoList(this);
        this.fnRestoreFromLocalStorage();
    }
    fnStoreToLocalStorage() {
        this.saveData('playlist', this.playlistInfoList.getSerialized());
    }
    fnRestoreFromLocalStorage() {
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
    }
    // API
    add(param): void {
        var item = param.item;
        this.playlistInfoList.add(new __MODEL__.cMediaPlaybackPlaylistItem(item));
    }
    remove(param): void {
        var type = param.type, id = param.id;
        this.playlistInfoList.remove(type, id);
    }
    getPlaylist(contentType): any {
        var playlist = [];
        var length = this.playlistInfoList.length;
        for (var i = 0; i < length; i += 1) {
            var info = this.playlistInfoList[i];
            if (info.getContentType() === contentType) {
                playlist.push(info);
            }
        }
        return playlist;
    }
}
export = cDLPlayList;