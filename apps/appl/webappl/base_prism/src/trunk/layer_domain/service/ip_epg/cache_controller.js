define(["require", "exports"], function (require, exports) {
    var ICacheQueryParamKey = (function () {
        function ICacheQueryParamKey() {
        }
        ICacheQueryParamKey.ReTryCount = 'reTryCount';
        ICacheQueryParamKey.ReqPromise = 'promiseRet';
        ICacheQueryParamKey.ResIJQueryXHR = 'IJQueryXHRWithQuery';
        ICacheQueryParamKey.ResDate = 'resDate';
        return ICacheQueryParamKey;
    })();
    exports.ICacheQueryParamKey = ICacheQueryParamKey;
    function __isExpiresExpired(cacheInfo) {
        var expires = cacheInfo['Expires'];
        var ret = !!expires && new Date(expires).valueOf() <= Date.now();
        console.info('__isExpiresExpired: ' + ret);
        return ret;
    }
    function __isMaxAgeExpired(cacheInfo) {
        var date = cacheInfo['Date'];
        var cacheControl = cacheInfo['Cache-Control'];
        if (!date || !cacheControl) {
            return false;
        }
        var maxAge = /^max-age=([0-9]+)/.exec(cacheControl);
        if (!(maxAge && maxAge[1])) {
            return false;
        }
        var ret = new Date(date).valueOf() + (parseInt(maxAge[1]) * 1000) <= Date.now();
        console.info('__isMaxAgeExpired: ' + ret);
        return ret;
    }
    function __stringToHeader(header) {
        var ret = {};
        if (header) {
            var splitted = header.split('\n');
            splitted.forEach(function (header) {
                var data = header.split(': ');
                if (data[0] && data[1]) {
                    ret[data[0]] = data[1];
                }
            });
        }
        return ret;
    }
    var CCacheController = (function () {
        function CCacheController(categories, prevCache) {
            var _this = this;
            this._cache = {};
            this.util = {
                string2Header: __stringToHeader
            };
            if (prevCache) {
                categories.forEach(function (category) {
                    _this._cache[category] = prevCache[category] || {};
                });
            }
            else {
                categories.forEach(function (category) {
                    _this._cache[category] = {};
                });
            }
            this._categories = categories;
        }
        CCacheController.prototype.set = function (category, hash, header, status, data, param) {
            if (!this._cache[category]) {
                this._cache[category] = {};
            }
            this._cache[category][hash] = {
                header: header ? __stringToHeader(header) : (this._cache[category][hash] ? this._cache[category][hash].header : null),
                status: status,
                data: data || (this._cache[category][hash] ? this._cache[category][hash].data : null),
                param: this._cache[category][hash] ? this._cache[category][hash].param : []
            };
            var p = this._cache[category][hash].param;
            if (p) {
                var keyList = Object.keys(p);
                keyList.forEach(function (key) {
                    if (p[key] && param && param[key]) {
                        p[key] = param[key] ? param[key] : p[key];
                    }
                });
            }
            return this;
        };
        CCacheController.prototype.get = function (category, hash) {
            if (!this._cache[category]) {
                console.log('error cache set Error category ' + category);
                this._cache[category] = {};
            }
            return this._cache[category][hash] || null;
        };
        CCacheController.prototype.setCache = function (category, hash, cache) {
            if (!this._cache[category]) {
                console.log('error cache set Error category ' + category);
                this._cache[category] = {};
            }
            this._cache[category][hash] = cache;
        };
        CCacheController.prototype.del = function (category, hash) {
            this._cache[category][hash] = null;
            delete this._cache[category][hash];
            return this;
        };
        CCacheController.prototype.delData = function (category, hash) {
            if (this._cache[category] && this._cache[category][hash]) {
                this._cache[category][hash].data = null;
            }
            return this;
        };
        CCacheController.prototype.isExpired = function (header) {
            return __isMaxAgeExpired(header) || __isExpiresExpired(header);
        };
        CCacheController.prototype.getDb = function () {
            return this._cache;
        };
        CCacheController.prototype.setDb = function (db) {
            var _this = this;
            console.warn('cache db will be overrided');
            if (db) {
                this._categories.forEach(function (category) {
                    if (db[category]) {
                        _this._cache[category] = db[category];
                    }
                });
            }
            return this;
        };
        CCacheController.prototype.removeAll = function () {
            var _this = this;
            if (this._categories) {
                this._categories.forEach(function (category) {
                    if (_this._cache[category]) {
                        var cache = _this._cache[category];
                        var dataQueryList = Object.keys(cache);
                        dataQueryList.forEach(function (query) {
                            var data = cache[query];
                            var dataProperyList = Object.keys(data);
                            dataProperyList.forEach(function (property) {
                                if (data[property] instanceof XMLDocument) {
                                    //remove XML Root
                                    data[property].removeChild(data[property].documentElement);
                                    data[property] = null;
                                    delete data[property];
                                }
                                else {
                                    data[property] = null;
                                    delete data[property];
                                }
                            });
                            cache[query] = null;
                            delete cache[query];
                        });
                    }
                });
                console.log('cache_controller removeAll done');
            }
        };
        CCacheController.CACHE_DB_NAME = 'IP_EPG_CACHE_DB';
        return CCacheController;
    })();
    exports.CCacheController = CCacheController;
});
