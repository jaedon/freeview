/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp, nrdp_platform, nrdpPartner*/

#define GIBBON_PUSH(property, value, syncValue) this._syncData[property] = value; this._object.setSyncProperty(this._properties[property], syncValue)
#define GIBBON_PUSH_APPEND(property, value, syncValue) this._syncData[property] = value; this._object.setSyncProperty(this._properties[property], syncValue, true)
#define GIBBON_PULL(property) (this._syncData.hasOwnProperty(property) ? this._syncData[property] : this._defaults[property])
#if defined(BUILD_DEBUG)
# define GIBBON_PROXY_PULL(property) nrdp.gibbon._proxyObject(this, property)
#else
# define GIBBON_PROXY_PULL(property) GIBBON_PULL(property)
#endif

nrdp.gibbon = {
    _callback: 1,
    _callbacks: {},

    _nextWidgetId: 1,
    _nextEffectId: 1,
    _nextImageId: 1,
    _nextChildId: 1,
    _nextAnimId: 1,
    _nextDebugEvent: 0,
    _debugKeyEvents: 0,
    _debugAnimations: 0,

    _sync_Widget: 1,
    _sync_Effect: 2,
    _sync_Image: 3,
    _sync_Text: 4,

    INT_MAX: 0x7fffffff,

    init: function init(cb, url) {
        function makeArray(elt) {
            return Array.isArray(elt) ? elt : [elt];
        }

        var urlList = url ? makeArray(url) : [];

        function load_urls() {
            if (urlList.length == 0) {
                if (cb) cb();
            } else {
                var u = urlList.shift();
                nrdp.gibbon.loadScript({ url: u }, load_urls);
            }
        }

        if (nrdp.isReady) {
            load_urls();
        } else {
            nrdp.addEventListener("init", load_urls);
            nrdp.init();
        }
    },

    _proxyObject: function _proxyObject(object, property) {
        var proxy = {};
        //proxy._gcTag = nrdp_platform.gctag("Gibbon::ProxyObject::" + property);
        function createProxyValue(valueName, value) {
            //nrdp.log.error(" + Creating: " + valueName + " = " + JSON.stringify(value));
            Object.defineProperty(proxy, valueName, { enumerable: true, get: function() { return value; },
                                                      set: function(v) {
                                                          //nrdp.log.error("Setting: " + object.classname + "(" + object._path + ")::" + property + "::" + valueName + " = " + JSON.stringify(value));
                                                          value = v; object[property] = proxy;
                                                      } });
        }

        var defaults = object._defaults[property];
        var pulled = object._syncData.hasOwnProperty(property) ? object._syncData[property] : defaults;
        if(!(pulled instanceof Object))
            return pulled;

        var values = undefined;
        if(defaults)
            values = Object.keys(defaults);
        else
            values = Object.keys(pulled);
        if(values) {
            //nrdp.log.error("Proxy : " + object.classname + "(" + object._path + ")::" + property + " = " + JSON.stringify(values));
            for(var i = 0; i < values.length; ++i) {
                var valueName = values[i];
                var value = pulled[valueName];
                if(value && valueName.toLowerCase().indexOf("color") != -1)
                    value = { r: value.r, g: value.g, b: value.b, a: (value.a === undefined ? 255 : value.a) };
                createProxyValue(valueName, value);
            }
        }
        return proxy;
    },

    // *** DEPRECATED
    get CURL_INIT_ERROR() { return -1; },

    // Debugger
    get DEBUGGER_ATTRIBUTE_MODIFIED() { return 1; },
    get DEBUGGER_SUBTREE_MODIFIED() { return 2; },
    get DEBUGGER_NODE_REMOVED() { return 3; },

    fonts: {
        get entries() { return nrdp.gibbon._syncData.font_entries; },
        set entries(f) { nrdp._setProperty("gibbon", "fonts", f); },
        get scripts() { return nrdp.gibbon._syncData.font_scripts; },
        get storeCapacity() { return nrdp.gibbon._syncData.fontStoreCapacity; },

        attributes: {
            get NONE() { return 0x00000000; },
            get EMBOLDEN() { return 0x00000001; },
            get SLANT() { return 0x00000002; },
            get MONOSPACE() { return 0x00000004; },
            get SYNTHESIZE() { return 0x00000008; },
            get HINTING() { return 0x00000100; },
            get AUTOHINTER() { return 0x00000200; },
            get HINTNORMAL() { return 0x00000400; },
            get HINTLIGHT() { return 0x00000800; },
            get HINTMONO() { return 0x00001000; },
            get HINTLCD() { return 0x00002000; },
            get HINTLCDV() { return 0x00004000; }
        },
        weight: {
            get NORMAL() { return 0; },
            get BOLD() { return 1; }
        },
        style: {
            get NORMAL() { return 0; },
            get ITALIC() { return 1; }
        },

        info: {
            get NORMAL() { return 0; },
            get CACHE() { return 1; },
            get LRU() { return 2; },

            types: {
                get Url() { return 1; },
                get Buffer() { return 2; },
                get Mmap() { return 3; }
            },

            get: function get(mode, cb) {
                var id = nrdp.gibbon._setValue(cb);
                if (mode === undefined)
                   mode = nrdp.gibbon.fonts.info.NORMAL;
                nrdp._invoke("gibbon", "fontManagerInfo", {mode: mode, id: id});
            }
        },

        _initFont: function _initFont(info, fileName) {
            if (!nrdp.gibbon._syncData.downloadableFonts)
                nrdp.gibbon._syncData.downloadableFonts = {};
            var key = nrdp.gibbon.fonts._makeKey(info.params);
            nrdp.gibbon._syncData.downloadableFonts[key] = info;

            var priority = info.params && info.params instanceof Object && info.params.priority;
            if (priority === undefined)
                priority = 100;
            nrdp._invoke("gibbon", "loadFont", { key: key, priority: priority, font: info.params, fileName: fileName });
        },
        init: function init() {
            if (nrdp.gibbon.fonts._inited)
                return;
            nrdp.gibbon.fonts._inited = true;
            var store = nrdp.gibbon.fonts._store;
            if (!store)
                return;
            store.query(nrdp.storage.NO_DEVICE_ACCOUNT, "", function(q) {
                var keys = Object.keys(q);
                keys.forEach(function(k) {
                    // verify that we have a _info for every non-info and the other way around
                    var hasOpposite = function(key, obj, info) {
                        if (info)
                            return obj.hasOwnProperty(key.substr(0, key.length - 5));
                        return obj.hasOwnProperty(key + "_info");
                    };
                    if (k.indexOf("_info", k.length - 5) !== -1) {
                        if (!hasOpposite(k, q, true)) {
                            store.remove(nrdp.storage.NO_DEVICE_ACCOUNT, k);
                            return;
                        }
                        store.read(nrdp.storage.NO_DEVICE_ACCOUNT, k, 0, -1, function(v) {
                            if (v.success) {
                                try {
                                    var data = nrdp.utf8toa(v.value);
                                    var json = JSON.parse(data);
                                } catch (e) {
                                    nrdp.log.error("Unable to JSON parse font data from " + k);
                                    return;
                                }
                                // now query the ttf filename
                                var url = k.substr(0, k.length - 5);
                                store.query(nrdp.storage.NO_DEVICE_ACCOUNT, url, function(f) {
                                    if (f.hasOwnProperty(url)) {
                                        nrdp.gibbon.fonts._initFont(json, f[url].fileName);
                                    } else {
                                        nrdp.log.error("Unable to query font store for font " + url);
                                    }
                                }, true /* validate */);
                            } else {
                                nrdp.log.error("Unable to read info for " + k);
                            }
                        });
                    } else {
                        if (!hasOpposite(k, q, false)) {
                            store.remove(nrdp.storage.NO_DEVICE_ACCOUNT, k);
                        }
                    }
                });
            });
        },

        get _store() {
            if (!nrdp.gibbon.fonts._storeContext || !nrdp.gibbon.fonts._storeContext.valid) {
                if (!nrdp.gibbon.fonts.storeCapacity || nrdp.gibbon.fonts.storeCapacity < 0)
                    return undefined;
                nrdp.gibbon.fonts._storeContext = (nrdp.storage.diskStoreContexts["font-store"] ||
                                                   nrdp.storage.createDiskStoreContext({ context: "font-store",
                                                                                         size: nrdp.gibbon.fonts.storeCapacity,
                                                                                         encrypted: false,
                                                                                         signature: true }));
            }
            return nrdp.gibbon.fonts._storeContext;
        },
        _makeKey: function _makeKey(req) {
            if (!req instanceof Object)
                return undefined;
            if (!req.hasOwnProperty("family"))
                return undefined;
            if (!req.hasOwnProperty("style"))
                return undefined;
            if (!req.hasOwnProperty("weight"))
                return undefined;
            return req.family + "-" + req.style + "-" + req.weight;
    },
        _storeContext: undefined,
        _inited: false
    },

    get email() {
        return this._syncData.email;
    },

    diskCache: {
        get capacity() {
            if(nrdp.gibbon._syncData.diskCacheConfiguration)
                return nrdp.gibbon._syncData.diskCacheConfiguration.capacity;
            return 0;
        },
        get maxPending() {
            if (nrdp.gibbon._syncData.diskCacheConfiguration)
                return nrdp.gibbon._syncData.diskCacheConfiguration.maxPending;
            return 0;
        },
        get writeLimit() {
            if(nrdp.gibbon._syncData.diskCacheConfiguration)
                return nrdp.gibbon._syncData.diskCacheConfiguration.writeLimit;
            return 0;
        },
        set writeLimit(limit) {
            nrdp._invoke("gibbon", "diskCacheSetWriteLimit", { limit: limit });
        },
        get catalogTimer() {
            if(nrdp.gibbon._syncData.diskCacheConfiguration)
                return nrdp.gibbon._syncData.diskCacheConfiguration.catalogTimer;
            return 0;
        },
        get writeSpeed() { // bytes/sec
            if(nrdp.gibbon._syncData.diskCacheConfiguration)
                return nrdp.gibbon._syncData.diskCacheConfiguration.writeSpeed;
            return 0;
        },
        insert: function insert(obj, cb) {
            obj.id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "diskCacheInsert", obj);
        },
        clear: function clear(cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "clearDiskCache", {id: id});
        },
        clearStats: function clear(cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "clearDiskCacheStats", {id: id});
        },
        flush: function flush(cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "flushDiskCache", {id: id});
        },
        reinit: function reinit(spec, cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "reinitDiskCache", {spec: spec, id: id});
        },
        info: function info(args, cb) {
            if (!args)
                args = {};
            args.id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "diskCacheInfo", args);
        },
        remove: function(cacheKey, cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "diskCacheRemove", {cacheKey: cacheKey, id: id});
        },
        purgeExpired: function(cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "diskCachePurgeExpired", {id: id});
        },
        _dump: function _dump(cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "dumpDiskCache", {id: id});
        }
    },

    surfaceCache: {
        get capacity() {
            if (nrdp.gibbon._syncData.surfaceCacheCapacity)
                return nrdp.gibbon._syncData.surfaceCacheCapacity.browse;
            return undefined;
        },
        set capacity(cap) {
            nrdp._invoke("gibbon", "surfaceCacheSetBrowseCapacity", { capacity: cap });
        },
        get playbackCapacity() {
            if (nrdp.gibbon._syncData.surfaceCacheCapacity)
                return nrdp.gibbon._syncData.surfaceCacheCapacity.playback;
            return undefined;
        },
        set playbackCapacity(cap) {
            nrdp._invoke("gibbon", "surfaceCacheSetPlaybackCapacity", { capacity: cap });
        },
        info: function(cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "surfaceCacheInfo", {id: id});
        }
    },

    resourceManager: {
        get capacity() {
            return nrdp.gibbon._syncData.resourceManagerCapacity;
        },
        set capacity(cap) {
            nrdp._invoke("gibbon", "resourceManagerSetCapacity", { capacity: cap });
        },
        info: function(cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "resourceManagerInfo", {id: id});
        },
        disable: function(cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "resourceManagerDisable", {id: id});
        },
        remove: function(cacheKey, cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "resourceManagerRemove", {id: id, cacheKey: cacheKey});
        },
        clear: function(cb) {
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "resourceManagerClear", {id: id});
        }
    },

    get debugFlags() {
        return this._syncData.debugFlags;
    },

    get password() {
        return this._syncData.password;
    },

    get imageFormats() {
        return this._syncData.imageFormats;
    },

    get defaultLocale() {
        return this._syncData.defaultLocale;
    },

    localeInfo: function localeInfo(l) {
        return nrdp._invoke("gibbon", "localeInfo", { locale: l });
    },

    set defaultLocale(l) {
        nrdp._setProperty("gibbon", "defaultLocale", l);
    },

    get defaultDirection() {
        return this._syncData.defaultDirection;
    },

    set defaultDirection(d) {
        nrdp._setProperty("gibbon", "defaultDirection", d);
    },

    get garbageCollectTimeout() {
        return this._syncData.garbageCollectTimeout;
    },

    set garbageCollectTimeout(t) {
        nrdp._setProperty("gibbon", "garbageCollectTimeout", t);
    },

    get prefetchUrls() {
        return this._syncData.prefetchUrls;
    },

    get prefetchKey() {
        return this._syncData.prefetchKey;
    },
    get effectivePrefetchUrls() {
        return this._syncData.effectivePrefetchUrls;
    },
    set prefetchUrls(urls) {
        if (urls && !(urls instanceof Object)) {
            nrdp.log.error("Invalid prefetch urls");
            return;
        }
        this._syncData.prefetchUrls = urls;
        nrdp._setProperty("gibbon", "prefetchUrls", urls);
    },

    _path: "gibbon",
    addEventListener: function addEventListener(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function removeEventListener(evt, listener) { nrdp._removeEventListener(this, evt, listener); },

    get effects() { return this.capabilities.effects; },
    get capabilities() { return nrdp.capabilities.gibbon; },

    setStat: function setStat(name, value) { nrdp._invoke("gibbon", "setStat", {name: name, value: value}); },

    get cookie() { return (this._syncData) ? this._syncData.cookie: ""; },
    set cookie(c) { nrdp._invoke("gibbon", "setCookie", {cookie: c}); },

    setCookie: function setCookie(cookie, callback) {
        var id = nrdp.gibbon._setValue(callback);
        nrdp._invoke("gibbon", "setCookie", {cookie: cookie, id: id});
    },

    get cookieFilters() {
        return nrdp.gibbon._syncData.cookieFilters;
    },
    set cookieFilters(filters) {
        if (!filters)
            filters = [];
        if (!(filters instanceof Array)) {
            nrdp.log.error("Invalid cookie filters");
            return;
        }
        nrdp.gibbon._syncData.cookieFilters = filters;
        nrdp._setProperty("gibbon", "cookieFilters", filters);
    },

    _createAnimation: function _createAnimation(property, start, end, duration, ease, append, flags) {
        if(append === undefined)
            append = false;
        if(start instanceof Object && start.relative)
            start.relative = start.relative._id;
        var value = { value: end,
                      animate: { id: nrdp.gibbon._nextAnimId++, start: start, end: end,
                                 duration: duration, ease: ease, append: append, flags: flags } };
#if 0
        if(typeof value.animate.ease == 'string' && value.animate.ease.length > 12 && value.animate.ease.slice(0, 12) == "cubic-bezier") {
            var bezier_regex = /^cubic-bezier *\((\d*(?:\.\d*)?), *(\d*(?:\.\d*)?), *(\d*(?:\.\d*)?), *(\d*(?:\.\d*)?)\) *$/;
            var bezier_matches = bezier_regex.exec(value.animate.ease);
            if(bezier_matches) {
                var flaggies = { x1: Math.max(0, Math.min(1, parseFloat(bezier_matches[1]))),
                                 y1: parseFloat(bezier_matches[2]),
                                 x2: Math.max(0, Math.min(1, parseFloat(bezier_matches[3]))),
                                 y2: parseFloat(bezier_matches[4]) };
                value.animate.ease = { type: "cubic-bezier", flags: flaggies };
            }
        }
#endif
        return value;
    },

    _loadSplash: function _loadSplash(request) {
        if(!(/^https?:/).test(nrdp.options.splash_screen) || nrdp.gibbon._splashUrl)
            return;

        var defaultSplash = nrdp.options.splash_screen;
        if(request && request.url && request.url.length) {
            var transientData = nrdp.storage.transientData || {};
            transientData.splash = request;
            nrdp.storage.transientData = transientData;
        } else {
            if(nrdp.storage.transientData)
                request = nrdp.storage.transientData.splash;
            if(!request || !request.url || !request.url.length)
                request = { url: defaultSplash };
        }
        nrdp.gibbon._splashUrl = request.url;
        nrdp.gibbon.loadScript(request, function(response) {
            if (response.exception || response.statusCode != 200) {
                nrdp.log.info("falling back to splash screen " + defaultSplash);
                nrdp.gibbon._splashUrl = defaultSplash;
                nrdp.gibbon.loadScript({ 'url': defaultSplash });
            } else {
                nrdp.gibbon.sync();
            }
        });
    },

    get location() {
        var result = this._baseUrl;
        if(!result) {
            // there is a race condition in c++ when the location is the
            // result of a redirect; the updated location may not come
            // until after nrdp isReady. but nrdp_platform can always be
            // right.
            if(typeof nrdp_platform !== "undefined" && typeof nrdp_platform.location === "function")
                result = nrdp_platform.location();
            else
                result = this._syncData.location;
            if(typeof result == 'object')
                result = result.url;
        }
        return result;
    },
    set location(l) {
        var args = l;
        this._baseUrl = undefined;
        if(typeof args == 'string')
            args = {url: args};
        args.url = this._resolveUrl(args.url);
        nrdp._setProperty("gibbon", "location", args);
    },

    get _locationCount() {
        var result;
        if(typeof nrdp_platform !== "undefined" && typeof nrdp_platform.location === "function")
            result = nrdp_platform.location();
        else
            result = this._syncData.location;
        if(typeof result == 'object')
            return result.count;
        return 0;
    },
    get queryParams() {
        if(!this._syncData['queryParams'])
            this._syncData['queryParams'] = nrdp.gibbon._parseQueryParams(this.location);
        return this._syncData['queryParams'];
    },

    loaded: function loaded() {
        nrdp._invoke("gibbon", "uiLoaded");
    },

    startTask: function startTask(cb, priority) {
        var id = nrdp.gibbon._setValue({ callback: cb });
        nrdp._invoke("gibbon", "startTask", { id: id, priority: priority });
        if(nrdp.gibbon.debugFlags.debugScriptEvents)
            nrdp.log.error("StartTask " + id);
        return id;
    },
    stopTask: function stopTask(id) {
        if(nrdp.gibbon._getValue(id) !== undefined) {
            if(nrdp.gibbon.debugFlags.debugScriptEvents)
                nrdp.log.error("StopTask " + id);
            nrdp.gibbon._deleteValue(id);
            nrdp._invoke("gibbon", "stopTask", { id: id });
        }
    },

    setTimeout: function setTimeout(cb, interval, singleShot) {
        if(singleShot === undefined) // default singleShot to true
            singleShot = true;

        var id = nrdp.gibbon._setValue({ callback: cb, singleShot: singleShot, interval: interval });
        nrdp._invoke("gibbon", "startTimer", { id: id, singleShot: singleShot, interval: interval });
        if (nrdp.gibbon.debugFlags.logZeroTimeouts && (interval === 0))
        {
            nrdp.log.info("Created[" + id + "]: " + singleShot + "::" + interval,
                          "TIMER");
            var st = nrdp.stacktrace();
            var a = st.split('\n');
            a.forEach(function(m) { nrdp.log.info(m, "TIMER"); });
        }
        if(nrdp.gibbon.debugFlags.debugScriptEvents)
            nrdp.log.error("StartTimer " + id + "@" + interval + ":" + singleShot);
        return id;
    },
    clearTimeout: function clearTimeout(id) {
        if(nrdp.gibbon._getValue(id) !== undefined) {
            if(nrdp.gibbon.debugFlags.debugScriptEvents)
                nrdp.log.error("StopTimer " + id);
            nrdp.gibbon._deleteValue(id);
            nrdp._invoke("gibbon", "stopTimer", { id: id });
        }
    },

    garbageCollect: function garbageCollect(callback, type) {
        var id = callback ? nrdp.gibbon._setValue(callback) : 0;
        nrdp._invoke("gibbon", "garbageCollect", { id: id, type: type });
    },
    eval: function eval(script, file) {
        return nrdp._invoke("gibbon", "eval", { script: script, file: file });
    },

    _load: function _load(request, callback) {
        request.id = nrdp.gibbon._setValue(callback);
        request.url = this._resolveUrl(request.url);
        nrdp._invoke("gibbon", "startRequest", request);
        if(nrdp.gibbon.debugFlags.debugScriptEvents)
            nrdp.log.error("StartRequest " + request.id + "@" + request.url);
        return request.id;
    },
    load: function load(request, callback) {
        if(typeof request !== "object") {
            if(typeof callback === "function")
                callback({});
            return -1;
        }
        request.eval = false;
        if(request.async === undefined)
            request.async = true;
        return nrdp.gibbon._load(request, callback);
    },
    loadScript: function loadScript(request, callback) {
        if(typeof request !== "object") {
            if(typeof callback === "function")
                callback({});
            return -1;
        }
        request.eval = true;
        if(request.async === undefined)
            request.async = false;
        return nrdp.gibbon._load(request, callback);
    },
    stopLoad: function stopLoad(id) {
        nrdp.gibbon._deleteValue(id);
        if(nrdp.gibbon.debugFlags.debugScriptEvents)
            nrdp.log.error("StopRequest " + id);
        nrdp._invoke("gibbon", "stopRequest", {id: id });
        return id;
    },

    addInjectJS: function addInjectJS(js, options) {
        nrdp._invoke("gibbon", "addInjectJS", { js: js, options: options });
    },
    removeInjectJS: function addInjectJS(url) {
        nrdp._invoke("gibbon", "removeInjectJS", { url: url });
    },

    setAtlasSize: function setAtlasSize(group, width, height) {
        nrdp._invoke("gibbon", "setAtlasSize", { group: group, width: width, height: height });
    },

    setGlyphAtlasSize: function setGlyphAtlasSize(width, height) {
        nrdp._invoke("gibbon", "setGlyphAtlasSize", { width: width, height: height });
    },

    sync: function sync() {
        nrdp._invoke("gibbon", "sync");
    },
    beginPendingSync: function beginPendingSync() {
        nrdp._invoke("gibbon", "beginPendingSync");
    },
    endPendingSync: function endPendingSync() {
        nrdp._invoke("gibbon", "endPendingSync");
    },

    _parseQueryParams: function _parseQueryParams(url)
    {
        var result = {};
        if(url) {
            var question = url.indexOf('?');
            if (question > 0) {
                url.substr(question+1).split('&').forEach(
                    function(e) {
                        if (e) {
                            var opt = e.split('=');
                            result[opt[0]] = (opt.length > 1 ? decodeURIComponent(opt[1]) : true);
                        }
                    });
            }
        }
        return result;
    },

    _compareColor: function _compareColor(color1, color2)
    {
        if(color1 instanceof Object && color2 instanceof Object)
            return (color1.r == color2.r && color1.g == color2.g &&
                    color1.b == color2.b && color1.a == color2.a);
        return color1 == color2;
    },
    _encodeColor: function _encodeColor(color)
    {
        if(color instanceof Object) {
            var result = 0;
            if(color.r)
                result |= (color.r & 0xFF) << 16;
            if(color.g)
                result |= (color.g & 0xFF) << 8;
            if(color.b)
                result |= (color.b & 0xFF) << 0;
            if(color.a === undefined)
                result |= 0xFF << 24;
            else if(color.a)
                result |= (color.a & 0xFF) << 24;
            return result;
        }
        return color;
    },

    _needPendingSync: function _needPendingSync() {
        return nrdp.gibbon._pendingSync;
    },

    _addDOMBreakpoint: function _addDOMBreakpoint(widgetId, type) {
        if (nrdp.gibbon._breaks === undefined)
            nrdp.gibbon._breaks = {};
        if (nrdp.gibbon._breaks[widgetId] == undefined)
            nrdp.gibbon._breaks[widgetId] = {};
        nrdp.gibbon._breaks[widgetId][type] = function() {
            nrdp.log.error("calling debugger!");
            debugger;
        };
    },
    _removeDOMBreakpoint: function _removeDOMBreakpoint(widgetId, type) {
        var hasAnyProperties = function(obj) {
            for (var p in obj) {
                if (obj[p] !== undefined)
                    return true;
            }
            return false;
        };
        delete nrdp.gibbon._breaks[widgetId][type];
        if (!hasAnyProperties(nrdp.gibbon._breaks[widgetId]))
            delete nrdp.gibbon._breaks[widgetId];
        if (!hasAnyProperties(nrdp.gibbon._breaks))
            nrdp.gibbon._breaks = undefined;
    },

    _debugObject: function _debugObject(obj, objname, num) {
        function getDescription(o) {
            if(typeof o === "object") {
                if(o instanceof Array)
                    return "Array[" + o.length + "]";
                else if(o instanceof Date)
                    return "Date";
                else if(o instanceof RegExp)
                    return "RegExp";
                else
                    return "Object";
            } else {
                return o + "";
            }
        }
        if(typeof obj === "object") {
            var arr = false;
            if(obj instanceof Array) {
                arr = true;
            }
            var list = [];
            var tp, stp, isobj, name;
            for (var i in obj) {
                tp = typeof obj[i];
                stp = undefined;
                if(tp === "object") {
                    if(obj[i] instanceof Array) {
                        stp = "array";
                    } else if(obj[i] instanceof Date)
                        stp = "date";
                    else if(obj[i] instanceof RegExp)
                        stp = "regexp";
                    isobj = true;
                } else {
                    isobj = false;
                }
                if(arr)
                    name = objname + "[" + i + "]";
                else
                    name = objname + "." + i;
                list.push({ name: i, value: { objectId: (isobj ? (name + ":" + num) : undefined),
                                              type: tp, subtype: stp,
                                              description: getDescription(obj[i])
                                            }
                          });
            }
            return list;
        } else {
            //return [{ value: { type: typeof obj, description: obj + "" }}];
        }
        return undefined;
    },
    get widgets() {
        var result = {};
        function recurse(widget) {
            if(widget) {
                result[widget._name] = widget;
                for(var i = 0; i < widget.children.length; ++i)
                    recurse(widget.children[i]);
            }
        }
        recurse(nrdp.gibbon.scene.root);
        recurse(nrdp.gibbon.scene.overlay);
        return result;
    },
    findWidget: function findWidget(id) {
        var widget;
        if(nrdp.gibbon.scene.root) {
            widget = nrdp.gibbon.scene.root.findWidget(id);
            if(widget)
                return widget;
        }
        if(nrdp.gibbon.scene.overlay) {
            widget = nrdp.gibbon.scene.overlay.findWidget(id);
            if(widget)
                return widget;
        }
        return undefined;
    },
    makeWidget: function makeWidget(obj) {
        var widgetid = nrdp.gibbon._nextWidgetId++;
        if(widgetid > nrdp.gibbon.INT_MAX)
            widgetid = 1;
        var widget = new nrdp.gibbon.Widget(widgetid);
        if(obj) {
            for(var property in obj) {
                if(property == "children") {
                    for(var childobj in obj.children) {
                        var childwidget = nrdp.gibbon.makeWidget(obj.children[childobj]);
                        childwidget.parent = widget;
                    }
                } else if(property == "animations") {
                    for(var aproperty in obj.animations)
                        widget.animate(obj.animations[aproperty].property, obj.animations[aproperty].duration,
                                       obj.animations[aproperty].ease);
                } else if(property == "effects") {
                    for(var eproperty in obj.effects)
                        widget.addEffect(obj.effects[eproperty].type, obj.effects[eproperty].params);
                } else if(property == "images") {
                    for(var iproperty in obj.images)
                        widget.addImage(obj.images[iproperty].background, obj.images[iproperty]);
                } else if(property == "parent" || property == "clone") {
                    widget[property] = nrdp.gibbon.findWidget(obj[property]);
                } else if(property == "image" || property == "backgroundImage") {
                    widget.addImage(property == "backgroundImage", obj[property]);
                } else {
                    widget[property] = obj[property];
                }
            }
        }
        return widget;
     },
    dump: function dump(cb) { nrdp.gibbon.scene.dump(cb); },
    _hookProperty : function _hookProperty(obj, property, hook) {
        var _hook_old = Object.getOwnPropertyDescriptor(obj, property);
        var _hook_new = Object.create(_hook_old);
        _hook_new.set = function(_value) {
            hook.call(this, _value, _hook_old.set);
        };
        Object.defineProperty(obj, property, _hook_new);
    },

    startFpsTimer: function startFpsTimer() { nrdp._invoke("gibbon", "startFpsTimer"); },
    stopFpsTimer: function stopFpsTimer() { nrdp._invoke("gibbon", "stopFpsTimer"); },
    getRenderedFps: function getRenderedFps(callback) {
        var id = nrdp.gibbon._setValue(callback);
        nrdp._invoke("gibbon", "getRenderedFps", {id: id});
    },
    getHeapSize: function getHeapSize(callback) {
        if (nrdp.ps3 && nrdp.ps3.getMemoryStats) {
            // grab the PS3 info
            nrdp.ps3.getMemoryStats(function(ps3data) {
                var id = nrdp.gibbon._setValue(function(data) {
                    data["systemallocated"] = ps3data.heap.mmap.used + ps3data.heap.used;
                    data["systemused"] = ps3data.heap.mmap.total + ps3data.heap.total;
                    callback(data);
                });
                nrdp._invoke("gibbon", "getHeapSize", {id: id});
            });
        } else {
            var id = nrdp.gibbon._setValue(callback);
            nrdp._invoke("gibbon", "getHeapSize", {id: id});
        }
    },

    moveMouse: function moveMouse(x, y) {
        nrdp._invoke("gibbon", "moveMouse", {x: x, y: y});
    },
    widgetsAt: function widgetsAt(x, y, cb) {
        var id = nrdp.gibbon._setValue(cb);
        nrdp._invoke("gibbon", "widgetsAt", {x: x, y: y, id: id});
    },

    reloadFonts: function reloadFonts() {
        nrdp._invoke("gibbon", "reloadFonts");
    },

    _fontTTFCallback: function _fontTTFCallback(key, data) {
        if (!nrdp.gibbon._syncData.downloadableFonts.hasOwnProperty(key))
            return;
        var info = nrdp.gibbon._syncData.downloadableFonts[key];
        var params = info.params;
        if (!params.hasOwnProperty("attributes"))
            params.attributes = nrdp.gibbon.fonts.attributes.HINTING | nrdp.gibbon.fonts.attributes.HINTNORMAL;
        if (data.errorcode !== 0 || (data.statusCode < 200 || data.statusCode >= 300) || !data.data.byteLength) {
            if (info.callback)
                info.callback({ success: false, error: "Error in download of TTF" });
            delete nrdp.gibbon._syncData.downloadableFonts[key];
            return;
        }

        var persist = (params && params instanceof Object && params.persist == true);
        var priority = params && params instanceof Object && params.priority;
        if (priority === undefined)
            priority = 100;

        if (persist) {
            var store = nrdp.gibbon.fonts._store;
            if (!store) {
                if (info.callback)
                    info.callback({ success: false, error: "Font store could not be created" });
                delete nrdp.gibbon._syncData.downloadableFonts[key];
                return;
            }
            store.create(nrdp.storage.NO_DEVICE_ACCOUNT, key, data.data, function(o) {
                if (o.success) {
                    // get the file name and mmap it in
                    store.query(nrdp.storage.NO_DEVICE_ACCOUNT, key, function(q) {
                        if (q.hasOwnProperty(key)) {
                            var fn = q[key].fileName;

                            var cb = info.callback;
                            delete info["callback"];
                            var id = nrdp.gibbon._setValue(cb);
                            nrdp._invoke("gibbon", "loadFont", { id: id, priority: priority, key: key, font: params, fileName: fn });
                        } else {
                            if (info.callback)
                                info.callback({ success: false, error: "Unable to query font from font store" });
                            delete nrdp.gibbon._syncData.downloadableFonts[key];
                        }
                    }, true /* validate */);
                    // also persist the info structure so we can read it in on startup next time
                    store.create(nrdp.storage.NO_DEVICE_ACCOUNT, key + "_info", JSON.stringify(info));
                } else {
                    if (info.callback)
                        info.callback({ success: false, error: "Unable to store font in font store" });
                    delete nrdp.gibbon._syncData.downloadableFonts[key];
                }
            });
        } else {
            var cb = info.callback;
            delete info["callback"];
            var id = nrdp.gibbon._setValue(cb);
            nrdp._invoke("gibbon", "loadFont", { id: id, priority: priority, key: key, font: params, data: data.data });
        }
    },

    addFont: function addFont(params, callback) {
        if (!(params instanceof Object)) {
            if (callback) {
                callback({ success: false, error: "Params is not an object" });
            }
            return;
        }
        if (!nrdp.gibbon._syncData.downloadableFonts)
            nrdp.gibbon._syncData.downloadableFonts = {};
        if (!params.hasOwnProperty("request") || !(params.request instanceof Object)) {
            if (callback) {
                callback({ success: false, error: "Request missing" });
            }
            return;
        }
        var key = nrdp.gibbon.fonts._makeKey(params);
        if (key === undefined) {
            // bad
            if (callback) {
                callback({ success: false, error: "Unable to make font key" });
            }
            return;
        }
        if (nrdp.gibbon._syncData.downloadableFonts.hasOwnProperty(key)) {
            // already have the font
            if (callback)
                callback({ success: false, error: "Font exists" });
            return;
        }
        params.request.format = "arraybuffer";
        params._key = key;
        nrdp.gibbon._syncData.downloadableFonts[key] = { callback: callback, params: params };

        nrdp.gibbon.load(params.request, function(data) { nrdp.gibbon._fontTTFCallback(key, data); });
    },

    removeFont: function removeFont(params, cb) {
        if (!nrdp.gibbon._syncData.downloadableFonts) {
            if (cb)
                cb({success: false, error: "No downloadable fonts"});
            return;
        }
        var key = nrdp.gibbon.fonts._makeKey(params);
        if (key === undefined) {
            // bad
            if (cb) {
                cb({ success: false, error: "Unable to make font key" });
                return;
            }
        }
        if (!nrdp.gibbon._syncData.downloadableFonts.hasOwnProperty(key)) {
            if (cb)
                cb({success: false, error: "No such font: " + key});
            return;
        }
        var data = nrdp.gibbon._syncData.downloadableFonts[key];
        delete nrdp.gibbon._syncData.downloadableFonts[key];

        var store = nrdp.gibbon.fonts._store;
        store.remove(nrdp.storage.NO_DEVICE_ACCOUNT, key);
        store.remove(nrdp.storage.NO_DEVICE_ACCOUNT, key + "_info");

        var id = nrdp.gibbon._setValue(cb);
        nrdp._invoke("gibbon", "removeFont", {family: params.family, weight: params.weight, style: params.style, id: id});
    },

    cpuProfileStart: function cpuProfileStart(profileId) {
        nrdp._invoke("gibbon", "cpuProfileStart", {profileId: profileId});
    },

    cpuProfileStop: function cpuProfileStop(profileId, callback) {
        var id = nrdp.gibbon._setValue(callback);
        nrdp._invoke("gibbon", "cpuProfileStop", {id: id, profileId: profileId});
    },

    _runConsole: function _runConsole(cmd) {
        nrdp._invoke("gibbon", "runConsole", {command: cmd});
    },

    get _baseUrl() {
        if(this._baseUrlVar)
            return this._baseUrlVar.url;
        return undefined;
    },
    set _baseUrl(u) {
        if(u) {
            var dir = u;
            var q = dir.indexOf("?");
            if(q != -1)
                dir = dir.substr(0, q);
            var s = dir.lastIndexOf("/");
            if(s != -1)
                dir = dir.substr(0, s + 1);
            this._baseUrlVar = { dir: dir, url: u };
        } else {
            this._baseUrlVar = undefined;
        }
    },
    _resolveUrl: function _resolveUrl(url) {
        if(this._baseUrlVar && typeof url == "string" && url.indexOf("://") == -1 && url.indexOf("data:") != 0) {
            var baseUrl = this._baseUrlVar.dir;
            if(url[0] == "/") {
                var s = baseUrl.indexOf("/", 8);
                if(s != -1)
                    baseUrl = baseUrl.substr(0, s);
            }
            return baseUrl + url;
        }
        return url;
    },

    _syncData: { debugFlags: { } },
    _updateProperty: function _updateProperty(property, value) {
        if(property == "networkErrorCodes" || property == "networkErrorGroups") {
            for(var prop in value)
                nrdp.gibbon[prop] = value[prop];
        } else {
            var evt;
            if(property == "location") {
                this._syncData['queryParams'] = undefined;
            } else if(property == "fonts") {
                evt = {
                    type: "fontschange"
                };
                nrdp.gibbon._syncData.font_scripts = value.scripts;
                delete value["scripts"];
                property = "font_entries";
            } else if(nrdp.isReady && property == "cookie" && nrdp.gibbon.cookie != value) {
                evt = {
                    type: "cookiechange",
                    old: nrdp.gibbon.cookie
                };
            } else if (property == "diskCacheConfiguration" && value && value.writeSpeed != this.diskCache.writeSpeed) {
                evt = {
                    type: "writeSpeedMeasured",
                    writeSpeed: value.writeSpeed
                };
            }

            this._syncData[property] = value;

            if(evt)
                nrdp._callEventListeners(this, evt);
        }
    },

    sendKey: function sendKey(key, userData) {
        var lookupKey = function(name) {
            if (nrdp.nrdpPartner && nrdp.nrdpPartner.Keys) {
                for (var c in nrdp.nrdpPartner.Keys) {
                    if (nrdp.nrdpPartner.Keys[c] == name)
                        return parseInt(c);
                }
            }
            return undefined;
        };
        var evt = { type: "key", data: { type: key.type, code: lookupKey(key.name), uiEvent: key.name, userData: userData } };
        nrdp._callEventListeners(this, evt);
    },

    _handleEvent: function _handleEvent(event) {
        var handled = true;
        var cb;
        if(event.name == "gibbonEvent") {
            var type = event.data.type;
            if(type == "click" || type == "press" || type == "release") {
                var evt = { type: "key", time: event.time , data: event.data };
                if(nrdp.gibbon._debugKeyEvents) {
                    var desc = (event.data.repeat) ? "repeat" : type;
                    {
                        var monoTime = nrdp.mono();
                        if(nrdp.gibbon._keyEventLastMonoTime === undefined)
                            nrdp.gibbon._keyEventLastMonoTime = monoTime;
                        desc += ": " + (monoTime-nrdp.gibbon._keyEventLastMonoTime);
                        nrdp.gibbon._keyEventLastMonoTime = monoTime;
                        if(nrdp.gibbon._keyEventLastTime === undefined)
                            nrdp.gibbon._keyEventLastTime = event.data.time;
                        desc += " (" + (event.time-nrdp.gibbon._keyEventLastTime) + ":" + (monoTime-event.time) + ")";
                        nrdp.gibbon._keyEventLastTime = event.time;
                    }
                    nrdp.log.error("Sending keyevent(" + nrdp.gibbon._debugKeyEvents++ + "): " + desc + " [" + JSON.stringify(event) + "]");
                }
                if(nrdpPartner && nrdpPartner.Keys && !evt.data.uiEvent)
                    evt.data.uiEvent = nrdpPartner.Keys[evt.data.code];
                if(type == "click") {
                    evt.data.type = "press";
                    nrdp._callEventListeners(this, evt);
                    if(!event.data.repeat)
                        evt.data.type = "release";
                    else
                        evt = 0;
                }
                if(evt) {
                    nrdp._callEventListeners(this, evt);
                }
            } else if(type == "focusChange") {
                nrdp._callEventListeners(this, { type: "focus", time: event.time, data: event.data });
            } else if(type == "mouseChange") {
                nrdp._callEventListeners(this, { type: "mouse", time: event.time, data: event.data });
            } else if(type == "mouseMove" || type == "mousePress" || type == "mouseRelease") {
               if(event.data.widgets) {
                   for(var i = 0; i < event.data.widgets.length; ++i)
                       event.data.widgets[i].widget = nrdp.gibbon.findWidget(event.data.widgets[i].id);
               }
                nrdp._callEventListeners(this, { type: "mouse", time: event.time, data: event.data });
           } else if(type == "touchMove" || type == "touchPress" || type == "touchRelease") {
               if(event.data.widgets) {
                   for(var j = 0; j < event.data.widgets.length; ++j)
                       event.data.widgets[j].widget = nrdp.gibbon.findWidget(event.data.widgets[j].id);
               }
               nrdp._callEventListeners(this, { type: "touch", time: event.time, data: event.data });
           } else {
               nrdp.log.error("unhandled gibbonEvent " + type);
           }
        } else if(event.name == "requestFinished") {
            cb = this._getValue(event.data.id);
            if(!event.data.refresh || event.data.state == "refresh")
                this._deleteValue(event.data.id);
            if(event.data.parsed && event.data.xml)
                nrdp._fixXml(event.data.xml);
            if(typeof cb == "function")
                cb(event.data);
        } else if(event.name == "task") {
            var task_id = event.data;
            var task = this._getValue(task_id);
            if(task !== undefined) {
                if(task.callback !== undefined)
                    task.callback();
                this._deleteValue(task_id);
            }
        } else if(event.name == "timer") {
            var timer_id = event.data;
            var timer = this._getValue(timer_id);
            if(timer !== undefined) {
                if(timer.callback !== undefined)
                    timer.callback();
                if(timer.singleShot)
                    this._deleteValue(timer_id);
            }
        } else if(event.name == "getRenderedFps"
                  || event.name == "diskCacheCleared"
                  || event.name == "diskCacheStatsCleared"
                  || event.name == "diskCacheFlushed"
                  || event.name == "diskCacheReinited"
                  || event.name == "diskCacheDumped"
                  || event.name == "diskCacheInfo"
                  || event.name == "diskCacheInsert"
                  || event.name == "diskCachePurgeExpired"
                  || event.name == "diskCacheRemove"
                  || event.name == "garbageCollected"
                  || event.name == "hash"
                  || event.name == "widgetsAt"
                  || event.name == "heapsize"
                  || event.name == "resourceManagerDisable"
                  || event.name == "resourceManagerInfo"
                  || event.name == "resourceManagerRemove"
                  || event.name == "resourceManagerClear"
                  || event.name == "setCookie"
                  || event.name == "cpuProfile"
                  || event.name == "fontLoaded"
                  || event.name == "fontRemoved"
                  || event.name == "fontManagerInfo"
                  || event.name == "surfaceCacheInfo") {
            cb = this._getValue(event.data.id);
            if(cb !== undefined) {
                if(event.name == "widgetsAt") {
                    if(event.data.widgets) {
                        for(var k = 0; k < event.data.widgets.length; ++k)
                            event.data.widgets[k].widget = nrdp.gibbon.findWidget(event.data.widgets[k].id);
                    }
                    cb(event.data);
                } else if(event.name == "getRenderFps") {
                    cb(event.data.fps);
                } else if(event.name == "heapsize"
                          || event.name == "cpuProfile") {
                    cb(event.data);
                } else if(event.name == "diskCacheDumped") {
                    cb(event.data.dump, event.data.stats);
                } else if(event.name == "diskCacheInfo"
                          || event.name == "fontLoaded"
                          || event.name == "fontRemoved") {
                    cb(event.data.data);
                } else if(event.name == "diskCacheInsert"
                          || event.name == "diskCacheRemove"
                          || event.name == "resourceManagerRemove") {
                    cb(event.data.success);
                } else if(event.name == "diskCachePurgeExpired") {
                    cb(event.data.count);
                } else if(event.name == "hash") {
                    cb(event.data.hash);
                } else if(event.name == "surfaceCacheInfo"
                          || event.name == "fontManagerInfo"
                          || event.name == "resourceManagerInfo") {
                    cb(event.data.info);
                } else {
                    cb();
                }
            }
            this._deleteValue(event.data.id);
        } else if (event.name == "capacityReached") {
            nrdp._callEventListeners(this, { type: "capacityReached", data: event.data });
        } else if (event.name == "codepointMissing") {
            nrdp._callEventListeners(this, { type: "codepointMissing", data: event.data });
        } else if (event.name == "diskCacheWriteLimiter") {
            nrdp._callEventListeners(this, { type: "diskCacheWriteLimiter", data: event.data });
        } else {
            handled = false;
        }
        return handled;
    },

    _setValue: function _setValue(value) {
        do {
            ++nrdp.gibbon._callback;
            if(nrdp.gibbon._callback >= nrdp.gibbon.INT_MAX)
                nrdp.gibbon._callback = 1;
        } while(nrdp.gibbon._callbacks[nrdp.gibbon._callback] !== undefined);
        nrdp.gibbon._callbacks[nrdp.gibbon._callback] = value;
        return nrdp.gibbon._callback;
    },
    _getValue: function _getValue(id) {
        return nrdp.gibbon._callbacks[id];
    },
    _deleteValue: function _deleteValue(id) {
        delete nrdp.gibbon._callbacks[id];
    },
    _invoke: function _invoke(object, method, args) {
        nrdp._invoke("gibbon", "invoke", {subObject:object, subMethod:method, subArgs:args});
    },

    addSocketizerConditions: function addSocketizerConditions(conditions) {
        nrdp._invoke("gibbon", "addSocketizerConditions", { conditions: conditions });
    },

    _hash: function (request, cb) {
        var req = request instanceof Object ? request : { url: request };
        req.id = nrdp.gibbon._setValue(cb);
        nrdp._invoke("gibbon", "hash", req);
    }
};
nrdp.addEventListener('init', function() {
    if(nrdp.gibbon.location != nrdp.system.bootURL) {
        nrdp.gibbon._loadSplash();
    }
    nrdp.gibbon.fonts.init();
});
nrdp.addEventListener('factoryReset', function() {
    if (nrdp.gibbon && nrdp.gibbon._syncData)
        nrdp.gibbon._syncData.downloadableFonts = {};
});

nrdp.describeEvent = function describeGibbonEvent(event) {
    if(event && event.type == "Event") {
        if(event.name == "timer" && event.object == "nrdp.gibbon")
            return "timer:" + event.data;
        else if(event.name == "requestFinished" && event.object == "nrdp.gibbon")
            return "requestFinished:" + event.data.id;
        else if(event.name == "gibbonEvent" && event.object == "nrdp.gibbon")
            return "gibbonEvent:" + JSON.stringify(event.data);
    }
    return nrdp._describeEvent(event);
},

nrdp._sendSyncdEvent = function(fn, that, event) {
    if(nrdp.gibbon.debugFlags.debugScriptEvents) {
        var monoTime = nrdp.mono();
        ++nrdp.gibbon._nextDebugEvent;
        nrdp.log.error("Sending event(" + nrdp.gibbon._nextDebugEvent + ") " + nrdp.describeEvent(event));
        fn.call(that, event);
        nrdp.log.error("~Sending event(" + nrdp.gibbon._nextDebugEvent + ") " + (nrdp.mono()-monoTime));
    } else {
        fn.call(that, event);
    }
};

if(typeof nrdpPartner === "undefined")
    nrdpPartner = {};
