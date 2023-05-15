/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp */

nrdp.storage = {
    classname: "StorageBridge",
    _path: "storage",
    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    get NO_DEVICE_ACCOUNT() { return "NDAKADN"; },

    get size() { return this._syncData.data ? JSON.stringify(this._syncData.data).length : 0; },

    get flushCount() { return this._syncData.flushCount; },

    get secureStoreSize() { return this._syncData.secureStoreSize; },

    get transientData() { return this._syncData.transientData; },
    set transientData(d) {
        this._syncData.transientData = d;
        // no need to _setProperty here, it will be done as part of flush() on page exit
    },

    length: function length(dak) {
        var data = nrdp.storage._getData(dak);
        var count = 0;
        for (var key in data) {
            if (data[key] != undefined)
                count++;
        }
        return count;
    },

    key: function key(dak, n) {
        var data = nrdp.storage._getData(dak);
        for (var k in data) {
            if (data[k] == undefined)
                continue;
            if (n == 0)
                return k;
            n--;
        }
        return undefined;
    },

    _normalizeKey: function _normalizeKey(key) {
        if (key === undefined)
            return "undefined";
        else if (key === null)
            return "null";
        else
            return key.toString();
    },

    getItem: function getItem(dak, key) {
        var stor = nrdp.storage;
        var data = stor._getData(dak);
        return data[stor._normalizeKey(key)];
    },

    setItem: function setItem(dak, key, value) {
        var stor = nrdp.storage;
        var data = stor._getData(dak);
        key = stor._normalizeKey(key);
        data[key] = value;
        nrdp._invoke("storage", "setItem", {dak: dak, key: key, value: value});
    },

    removeItem: function removeItem(dak, key) {
        var stor = nrdp.storage;
        var data = stor._getData(dak);
        key = stor._normalizeKey(key);
        if (data.hasOwnProperty(key))
            delete data[key];
        nrdp._invoke("storage", "removeItem", {dak: dak, key: key});
    },

    clear: function clear(dak) {
        if (nrdp.storage._syncData.data)
            delete nrdp.storage._syncData.data[dak];

        nrdp._invoke("storage", "clear", {dak: dak});
        nrdp.storage.disk.clear(dak);
    },

    _clearAll: function _clearAll() {
        nrdp.storage._syncData = {
            data: {}
        };
        nrdp._invoke("storage", "clearAll");
        nrdp.storage.disk.clearAll();
    },

    get disk() {
        if (!this._diskContexts["ui-cache"]) {
            return nrdp.storage.createDiskStoreContext({ context: "ui-cache",
                                                         size: nrdp.options.ui_cache_capacity,
                                                         encrypted: true,
                                                         signature: true });
        }
        return nrdp.storage._diskContexts["ui-cache"];
    },

    _diskContextPrototype: {
        size: undefined,
        valid: undefined,
        signature: undefined,
        encrypted: undefined,
        getSize: function getSize(cb) {
            if (!this.valid)
                throw "getSize called on destroyed context";
            nrdp.storage._fn("diskStoreGetSize", { context: this.context }, cb);
        },

        create: function create(dak, key, value, cb) {
            if (!this.valid)
                throw "create called on destroyed context";
            var obj = {
                context: this.context,
                dak: dak,
                key: key,
                value: value
            };
            nrdp.storage._fn("diskStoreCreate", obj, cb);
        },

        clear: function clear(dak, cb) {
            if (!this.valid)
                throw "clear called on destroyed context";

            var obj = {
                context: this.context,
                dak: dak
            };

            nrdp.storage._fn("diskStoreClear", obj, cb);
        },
        clearAll: function clear(cb) {
            if (!this.valid)
                throw "clearAll called on destroyed context";
            var obj = {
                context: this.context
            };

            nrdp.storage._fn("diskStoreClearAll", obj, cb);
        },
        append: function append(dak, key, value, cb) {
            if (!this.valid)
                throw "append called on destroyed context";
            var obj = {
                context: this.context,
                dak: dak,
                key: key,
                value: value
            };
            nrdp.storage._fn("diskStoreAppend", obj, cb);
        },
        remove: function remote(dak, key, cb) {
            if (!this.valid)
                throw "remove called on destroyed context";
            var obj = {
                context: this.context,
                dak: dak,
                key: key
            };
            nrdp.storage._fn("diskStoreRemove", obj, cb);
        },
        read: function read(dak, key, begin, end, cb) {
            if (!this.valid)
                throw "read called on destroyed context";
            var obj = {
                context: this.context,
                dak: dak,
                key: key,
                begin: begin || 0,
                end: end || -1
            };
            nrdp.storage._fn("diskStoreRead", obj, cb);
        },
        query: function query(dak, prefix, cb, validate) {
            if (!this.valid)
                throw "query called on destroyed context";
            var obj = { context: this.context, dak: dak, prefix: prefix, validate: validate };
            nrdp.storage._fn("diskStoreQuery", obj, function(data) { if (cb) cb(data.keys); });
        },
        validate: function(cb) {
            if (!this.valid)
                throw "query called on destroyed context";
            var obj = { context: this.context, validate: true  };
            nrdp.storage._fn("diskStoreQuery", obj, function(data) { if (cb) cb(data.keys); });
        },
        info: function info(cb) {
            if (!this.valid)
                throw "info called on destroyed context";
            nrdp.storage._fn("diskStoreInfo", { context: this.context }, function(data) { cb(data.info); });
        },
        corrupt: function corrupt(dak, key, mode, cb) {
            if (!this.valid)
                throw "corrupt called on destroyed context";
            var obj = {
                context: this.context,
                mode: mode,
                dak: dak,
                key: key
            };
            nrdp.storage._fn("diskStoreCorrupt", obj, cb);
        }
    },

    _diskContext: function _diskContext(data) {
        this.context = data.context;
        this.encrypted = data.encrypted || false;
        this.signature = data.signature || false;
        this.size = data.size;
        this.valid = true;
    },

    createDiskStoreContext: function(object, cb) {
        if (!object.context || this._diskContexts.hasOwnProperty(object.context)) {
            var err = "Invalid context: " + object.context;
            nrdp.log.error(err);
            if (cb)
                cb({success: false, error: err});
            return undefined;
        }
        var result = nrdp._invoke("storage", "diskStoreCreateContext", object);
        if (result.success) {
            var ret = new nrdp.storage._diskContext(object);
            this._diskContexts[object.context] = ret;
            if (cb)
                cb({success: true});
            return ret;
        }
        if (cb)
            cb({success: false, error: result.error});
        return undefined;
    },

    destroyDiskStoreContext: function(ctx, cb) {
        var context;
        if (ctx instanceof Object) {
            context = ctx;
        } else {
            context = this._diskContexts[ctx];
        }
        if (!context || !context.valid) {
            if (cb)
                cb({success: false, error: "destroyDiskStoreContext called on destroyed context"});
            return false;
        }
        context.valid = false;
        delete this._diskContexts[context.context];
        var result = nrdp._invoke("storage", "diskStoreDestroyContext", { context: context.context });
        if (cb)
            cb(result);
        return result && result.success;
    },

    get diskStoreContexts() { return this._diskContexts; },

    _getData: function _getData(dak) {
        if (!this._syncData)
            this._syncData = { data: {} };
        else if (!this._syncData.data)
            this._syncData.data = {};
        if (!this._syncData.data[dak])
            this._syncData.data[dak] = {};
        return this._syncData.data[dak];
    },

    setPersistentData: function setPersistentData(dak, key, data, deflate, cb) {
        nrdp.storage._fn("setPersistentData",
                         {dak:dak, key:key, data:data, deflate:deflate}, cb);
    },
    unsetPersistentData: function unsetPersistentData(dak, key, cb) {
        nrdp.storage._fn("unsetPersistentData",
                         {dak:dak, key:key}, cb);
    },
    getPersistentData: function getPersistentData(dak, key, inflate, cb) {
        nrdp.storage._fn("getPersistentData",
                         {dak:dak, key:key, inflate:inflate}, cb);
    },

    getFlushCount: function getFlushCount() {
        nrdp._invoke("storage", "getFlushCount");
    },

    flush: function flush() {
        nrdp._setProperty("storage", "transientData", nrdp.storage._syncData.transientData);
        nrdp._invoke("storage", "flush");
    },

    getUsedSecureStoreSize: function getUsedSecureStoreSize() {
        return nrdp._invoke("storage", "getUsedSecureStoreSize");
    },

    _nextIdx: 1,
    _cbs: {},
    _syncData: {},
    _diskContexts: {},
    _fn: function _fn(name, args, cb) {
        if (!args)
            args = {};
        args.idx = this._nextIdx++;
        if (cb)
            this._cbs[args.idx] = cb;
        nrdp._invoke("storage", name, args);
    },

    _handleEvent: function _handleEvent(event) {
        if (event.data && event.data.idx) {
            if (typeof this._cbs[event.data.idx] == "function") {
                this._cbs[event.data.idx](event.data);
            }
            delete this._cbs[event.data.idx];
        } else if (event.name == "overbudget") {
            event.data.type = event.name;
            nrdp._callEventListeners(this, event.data);
        } else if (event.name == "flushCount") {
            event.data.type = event.name;
            nrdp._callEventListeners(this, event.data);
        } else {
            return false;
        }
        return true;
    },
    _updateProperty: function _updateProperty(property, value) {
        if (property === "diskStoreInfo") {
            var old = this._diskContexts;
            this._diskContexts = {};
            for (var v in value) {
                if (old.hasOwnProperty(v)) {
                    this._diskContexts[v] = old[v];
                    delete old[v];
                } else {
                    var vals = value[v];
                    vals.context = v;
                    this._diskContexts[v] = new nrdp.storage._diskContext(vals);
                }
            }
            for (var i in old) {
                old[i].valid = false;
            }
        } else {
            this._syncData[property] = value;
        }
    }
};

nrdp.storage._diskContext.prototype = nrdp.storage._diskContextPrototype;
