/**
 * Created by bglee on 2015-03-20.
 */
/// <reference path="./q.d.ts" />
/// <reference path="../../def/jquery.d.ts" />
define(["require", "exports"], function (require, exports) {
    function __getAuthToken(url) {
        if (url === void 0) { url = 'https://auth-ctv.cp-test.digitaluk.co.uk'; }
        var deferred = Q.defer();
        $.ajax({
            type: 'head',
            url: url,
            success: function (str, num, res) {
                console.log('success: auth ');
                deferred.resolve({
                    "x-baseurl": res.getResponseHeader('x-baseurl'),
                    "x-auth-hash": res.getResponseHeader('x-auth-hash'),
                    "x-auth-timestamp": res.getResponseHeader('x-auth-timestamp')
                });
            },
            error: function (error) {
                console.log('fail: auth');
                deferred.reject(error);
            }
        });
        return deferred.promise;
    }
    var CMDSPoolFileReader = (function () {
        function CMDSPoolFileReader(count) {
            if (count === void 0) { count = 6; }
            this._pool = [];
            this._poolAvailable = [];
            var self = this;
            for (var i = 0; i < count; i++) {
                var fr = new FileReader();
                fr.__plantedIndex = i;
                fr.onload = function () {
                    var manager = self._pool[this.__plantedIndex];
                    manager.deferred.resolve(this.result);
                    manager.deferred = null;
                    self._poolAvailable.push(manager);
                };
                this._pool[i] = {
                    deferred: null,
                    fr: fr
                };
                this._poolAvailable[i] = this._pool[i];
            }
        }
        CMDSPoolFileReader.prototype.getDataURL = function (blob) {
            var manager = this._poolAvailable.shift();
            var deferred = Q.defer();
            if (!blob) {
                deferred.reject('arguments 0 must be blob object');
            }
            else if (!manager) {
                deferred.reject('file manager busy');
            }
            else {
                manager.deferred = deferred;
                manager.fr.readAsDataURL(blob);
            }
            return deferred.promise;
        };
        return CMDSPoolFileReader;
    })();
    var CMDSImageManager = (function () {
        function CMDSImageManager() {
            this._cache = {};
            this._poolFr = new CMDSPoolFileReader();
        }
        CMDSImageManager.prototype._requestURL = function (query) {
            var xhr = new XMLHttpRequest();
            var deferred = Q.defer();
            var self = this;
            xhr.onload = function () {
                var _this = this;
                if (this.readyState === 4) {
                    var plantedQuery = this.__plantedQuery;
                    var urlCached = this._getUrl(plantedQuery);
                    if (this.status === 200) {
                        self._poolFr.getDataURL(this.response).done(function (url) {
                            if (urlCached) {
                                console.log('url cache updated');
                            }
                            _this._setCacheURL(plantedQuery, url);
                            deferred.resolve(url);
                        }, deferred.reject);
                    }
                    else if (this.status === 304) {
                        deferred.resolve(urlCached);
                    }
                    else if (urlCached) {
                        deferred.resolve(urlCached);
                    }
                    else {
                        deferred.reject(this.status);
                    }
                }
            };
            xhr.open('GET', this._url_base + query, true);
            for (var key in this._header) {
                xhr.setRequestHeader(key, this._header[key]);
            }
            xhr.__plantedQuery = query;
            xhr.responseType = 'blob';
            xhr.send();
            return deferred.promise;
        };
        CMDSImageManager.prototype._setCacheURL = function (query, url) {
            this._cache[query] = url;
        };
        CMDSImageManager.prototype._getURL = function (query) {
            return this._cache[query];
        };
        CMDSImageManager.prototype.setHeader = function (header) {
            this._header = header;
        };
        CMDSImageManager.prototype.setBaseURL = function (url) {
            this._url_base = url;
        };
        CMDSImageManager.prototype.getImage = function (query) {
            if (!this._header) {
                this.onRequestHeader();
            }
            return this._requestURL(query);
        };
        CMDSImageManager.prototype.truncate = function () {
            this._cache = {};
        };
        CMDSImageManager.prototype._requestHeader = function () {
            var header = this.onRequestHeader();
            if (header) {
                throw '_requestHeader fail';
            }
            this._header = header;
        };
        return CMDSImageManager;
    })();
    var CMDSApi = (function () {
        function CMDSApi(urlAuth) {
            var _this = this;
            if (urlAuth === void 0) { urlAuth = 'https://auth-ctv.cp-test.digitaluk.co.uk'; }
            this._header = {};
            this._managerImage = new CMDSImageManager();
            this._url_auth = urlAuth;
            this.renewAuthInfo(urlAuth).done(function () {
                console.log('success: renew auth info');
            }, function () {
                console.log('fail: renew auth info');
            });
            this._managerImage.onRequestHeader = function () {
                return _this._getHeader();
            };
        }
        CMDSApi.prototype._setBaseURL = function (url) {
            this._url_base = url;
        };
        CMDSApi.prototype._getVersion = function () {
            if (!this._getBaseURL()) {
                var deferred = Q.defer();
                deferred.reject('need base url');
                return deferred.promise;
            }
            return this._call(this._getBaseURL() + '/version.json');
        };
        CMDSApi.prototype._getBaseURL = function () {
            return this._url_base;
        };
        CMDSApi.prototype._getHeader = function () {
            return this._header;
        };
        CMDSApi.prototype._call = function (url, deferred) {
            var deferred = deferred || Q.defer();
            $.ajax({
                type: 'get',
                url: url,
                headers: this._getHeader(),
                success: deferred.resolve,
                error: deferred.reject
            });
            return deferred.promise;
        };
        CMDSApi.prototype.renewAuthInfo = function (urlAuth) {
            var _this = this;
            var deferred = Q.defer();
            __getAuthToken(urlAuth).done(function (authInfo) {
                _this._setBaseURL(authInfo['x-baseurl']);
                _this.setHeader('x-auth-hash', authInfo['x-auth-hash']);
                _this.setHeader('x-auth-timestamp', authInfo['x-auth-timestamp']);
                _this._getVersion().done(function (json) {
                    _this.setHeader('Accept', 'application/vnd.fvc.v' + json.version[0] + '+xml');
                    _this._managerImage.setHeader(_this._getHeader());
                    deferred.resolve(authInfo);
                }, deferred.reject);
                _this._managerImage.setBaseURL(authInfo['x-baseurl'].replace('http://api', 'http://img'));
                _this._managerImage.setHeader(_this._getHeader());
            }, deferred.reject);
            return deferred.promise;
        };
        CMDSApi.prototype.setHeader = function (property, value) {
            console.log('setHeader: ' + property + ' | value: ' + value);
            this._header[property] = value;
        };
        CMDSApi.prototype.getBaseURL = function () {
            return this._getBaseURL();
        };
        CMDSApi.prototype.xml = function (query) {
            var _this = this;
            if (!this._getBaseURL()) {
                var deferred = Q.defer();
                this.renewAuthInfo(this._url_auth).done(function () {
                    _this._call(_this._getBaseURL() + query, deferred);
                });
                return deferred.promise;
            }
            return this._call(this._getBaseURL() + query);
        };
        CMDSApi.prototype.image = function (query) {
            return this._managerImage.getImage(query);
        };
        return CMDSApi;
    })();
    window.CMdSApi = CMDSApi;
    return CMDSApi;
});
