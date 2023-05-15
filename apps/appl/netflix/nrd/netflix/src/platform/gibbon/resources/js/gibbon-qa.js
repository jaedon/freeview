/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp*/
nrdp.gibbon.qa = {
    _path: "gibbon.qa",
    _nextIdx: 1,
    _cbs: {},
    _fn: function _fn(name, args, cb) {
        if (!args) args = {};
        args.id = this._nextIdx++;
        this._cbs[args.id] = cb;
        nrdp._invoke(this._path, name, args);
    },

    diskcache: function diskcache(cb) {
        this._fn("diskcache", {}, cb);
    },
    createFile: function createFile(file, contents) {
        return nrdp._invoke(this._path, "createFile", { file: file, contents: contents });
    },
    mkdir: function mkdir(dir) {
        return nrdp._invoke(this._path, "mkdir", { dir: dir });
    },
    remove: function remove(fileOrDir) {
        return nrdp._invoke(this._path, "remove", { file: fileOrDir });
    },
    listDirectory: function listDirectory(fileOrDir) {
        return nrdp._invoke(this._path, "listDirectory", { dir: fileOrDir });
    },
    readFile: function(file) {
        var result;
        nrdp.gibbon.load({url: "file://" + file, async: false}, function(ev) { result = ev.data; });
        return result;
    },
    _handleEvent: function _handleEvent(event) {
        if (event.name == "result") {
            var cb = this._cbs[event.data.id];
            if (typeof cb === "function")
                cb(event.data);
            delete this._cbs[event.data.id];
            return true;
        }
        return false;
    }
};
