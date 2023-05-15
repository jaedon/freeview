/**
 * Created by bglee on 2015-03-28.
 */
/// <reference path="../../../../def/jquery.d.ts" />
/// <reference path="../../../../def/q.d.ts" />
/// <reference path="../adapter.d.ts" />
/// <reference path="../../../../def/sugar.d.ts" />
define(["require", "exports"], function (require, exports) {
    var __debug = {
        retryAfter401: 'undefined'
    };
    var EErrorHandleProcessor;
    (function (EErrorHandleProcessor) {
        EErrorHandleProcessor[EErrorHandleProcessor["CHECK_RETRY_AFTER_HEADER"] = 0] = "CHECK_RETRY_AFTER_HEADER";
        EErrorHandleProcessor[EErrorHandleProcessor["BACK_OFF_MECHANISM"] = 1] = "BACK_OFF_MECHANISM";
        EErrorHandleProcessor[EErrorHandleProcessor["SEND_QUERY"] = 2] = "SEND_QUERY";
        EErrorHandleProcessor[EErrorHandleProcessor["WAIT_24_HOURS"] = 3] = "WAIT_24_HOURS";
        EErrorHandleProcessor[EErrorHandleProcessor["RE_AUTHENTICATE"] = 4] = "RE_AUTHENTICATE";
        EErrorHandleProcessor[EErrorHandleProcessor["BLOCK_QUERY_UNTIL_REBOOT"] = 5] = "BLOCK_QUERY_UNTIL_REBOOT";
    })(EErrorHandleProcessor || (EErrorHandleProcessor = {}));
    var __mechanism = [[
        5 /* BLOCK_QUERY_UNTIL_REBOOT */
    ], [
        0 /* CHECK_RETRY_AFTER_HEADER */,
        4 /* RE_AUTHENTICATE */
    ], [
        0 /* CHECK_RETRY_AFTER_HEADER */,
        2 /* SEND_QUERY */
    ], [
        2 /* SEND_QUERY */,
        3 /* WAIT_24_HOURS */,
        2 /* SEND_QUERY */,
        5 /* BLOCK_QUERY_UNTIL_REBOOT */
    ], [
        4 /* RE_AUTHENTICATE */,
        2 /* SEND_QUERY */,
        3 /* WAIT_24_HOURS */,
        2 /* SEND_QUERY */,
        5 /* BLOCK_QUERY_UNTIL_REBOOT */
    ], [
        1 /* BACK_OFF_MECHANISM */,
        4 /* RE_AUTHENTICATE */
    ], [
        1 /* BACK_OFF_MECHANISM */,
        4 /* RE_AUTHENTICATE */,
        2 /* SEND_QUERY */
    ], [
        1 /* BACK_OFF_MECHANISM */,
        0 /* CHECK_RETRY_AFTER_HEADER */,
        4 /* RE_AUTHENTICATE */
    ], [
        1 /* BACK_OFF_MECHANISM */,
        0 /* CHECK_RETRY_AFTER_HEADER */,
        2 /* SEND_QUERY */
    ]];
    var __hashmap_reponse_mechanism = {
        400: { auth: __mechanism[0], metadata: __mechanism[0], image: __mechanism[0] },
        401: { auth: __mechanism[1], metadata: __mechanism[2], image: __mechanism[0] },
        403: { auth: __mechanism[3], metadata: __mechanism[4], image: __mechanism[0] },
        404: { auth: __mechanism[3], metadata: __mechanism[4], image: __mechanism[0] },
        406: { auth: __mechanism[0], metadata: __mechanism[0], image: __mechanism[0] },
        500: { auth: __mechanism[5], metadata: __mechanism[6], image: __mechanism[0] },
        503: { auth: __mechanism[7], metadata: __mechanism[8], image: __mechanism[8] },
        504: { auth: __mechanism[5], metadata: __mechanism[6], image: __mechanism[0] }
    };
    function __setStack(res, queryType) {
        if (!res.traceInfo || res.traceInfo.status !== res.status) {
            res.traceInfo = {
                query: res.query,
                status: res.status,
                mechanism: __hashmap_reponse_mechanism[res.status][queryType],
                position: 0
            };
        }
        else {
            res.traceInfo.position++;
        }
        console.info('> __setStack: status: ' + res.traceInfo.status);
        console.info('> __setStack: ' + EErrorHandleProcessor[res.traceInfo.mechanism[res.traceInfo.position]]);
        return res.traceInfo.mechanism[res.traceInfo.position];
    }
    function __getAuthToken(url) {
        if (!url) {
            return Q.reject('auth url is null');
        }
        if (!hx.svc.SETTING_UTIL.getInternetAvailable()) {
            hx.log('mds', '[__getAuthToken] cannot make a request cos network(internet) is not available');
            return Q.reject('network is not available');
        }
        var deferred = Q.defer();
        $.ajax({
            type: 'head',
            url: url,
            headers: {
                'Accept': 'application/vnd.fvc.v1+xml'
            },
            success: function (str, num, res) {
                hx.log('mds', '[__getAuthToken] success: auth / res.status : ' + res.status);
                var dataResolved = {
                    status: res.status,
                    header: res.getAllResponseHeaders(),
                    data: {
                        "x-baseurl": res.getResponseHeader('x-baseurl'),
                        "x-auth-hash": res.getResponseHeader('x-auth-hash'),
                        "x-auth-timestamp": res.getResponseHeader('x-auth-timestamp')
                    },
                    query: ''
                };
                if (!res.getResponseHeader('x-baseurl') || !res.getResponseHeader('x-auth-hash') || !res.getResponseHeader('x-auth-timestamp')) {
                    console.error('##### AUTH return empty ######### status : ' + res.status);
                    var ret = res;
                    ret.status = 999;
                    ret.query = url;
                    deferred.reject(ret);
                    return;
                }
                deferred.resolve(dataResolved);
            },
            error: function (res) {
                hx.log('debug', '[__getAuthToken] error!! fail: auth');
                res.query = url;
                deferred.reject(res);
            }
        });
        return deferred.promise;
    }
    var CMdsPoolFileReader = (function () {
        function CMdsPoolFileReader() {
            this._fr = null;
            this._bProcessing = false;
            this._dataQueue = [];
            this._fr = new FileReader();
        }
        CMdsPoolFileReader.prototype.getDataUrl = function (aBlob) {
            var self = this;
            var aDeferred = Q.defer();
            self._dataQueue.push({
                deferred: aDeferred,
                blob: aBlob
            });
            if (!self._fr) {
                self._fr = new FileReader();
            }
            function fnProcess() {
                hx.log('debug', '[FileReader] self._dataQueue.length : ' + self._dataQueue.length);
                var aData = self._dataQueue.shift();
                if (aData === undefined) {
                    hx.log('debug', '[FileReader] all loading is done');
                    self._fr = null;
                    return;
                }
                self._bProcessing = true;
                (function fnLoadData(data) {
                    self._fr.onload = function () {
                        hx.log('debug', '[FileReader] onload');
                        data.deferred.resolve(this.result);
                        data = null;
                        self._bProcessing = false;
                        fnProcess();
                    };
                    self._fr.onerror = function () {
                        hx.log('debug', '[FileReader] onerror');
                        data.deferred.reject(null);
                        data = null;
                        self._bProcessing = false;
                        fnProcess();
                    };
                    self._fr.readAsDataURL(data.blob);
                    data.blob = null;
                })(aData);
                aData = null;
            }
            if (!self._bProcessing) {
                fnProcess();
            }
            aBlob = null;
            return aDeferred.promise;
        };
        return CMdsPoolFileReader;
    })();
    var CMdsImageManager = (function () {
        function CMdsImageManager() {
            //private _cache = {};
            this._poolFr = new CMdsPoolFileReader();
        }
        CMdsImageManager.prototype._requestUrl = function (query, header, type) {
            if (!hx.svc.SETTING_UTIL.getInternetAvailable()) {
                hx.log('mds', '[_requestUrl] cannot make a request cos network(internet) is not available');
                return Q.reject('network is not available');
            }
            var xhr = new XMLHttpRequest();
            var deferred = Q.defer();
            var self = this;
            !function (aXhr, aQuery) {
                aXhr.onload = function () {
                    if (this.readyState === 4) {
                        var dataResolved = {
                            status: this.status,
                            header: this.getAllResponseHeaders(),
                            data: null,
                            query: aQuery
                        };
                        if (this.status === 0 || this.status >= 400) {
                            hx.log('debug', '[_requestUrl] aXhr.onload! query Error(' + this.status + ') : ' + aQuery);
                            var ret = aXhr;
                            ret.query = aQuery;
                            deferred.reject(ret);
                            return;
                        }
                        if (this.status === 200) {
                            if (this.responseType === 'blob') {
                                self._poolFr.getDataUrl(this.response).done(function (url) {
                                    dataResolved.data = url;
                                    deferred.resolve(dataResolved);
                                }, deferred.reject);
                            }
                            else {
                                dataResolved.data = aQuery;
                                deferred.resolve(dataResolved);
                            }
                            return;
                        }
                        deferred.resolve(dataResolved);
                    }
                };
                aXhr.onerror = function () {
                    hx.log('debug', '[_requestUrl] aXhr.onerror!!! query Error(' + this.status + ') : ' + aQuery);
                    var ret = aXhr;
                    ret.query = aQuery;
                    deferred.reject(ret);
                };
            }(xhr, query);
            xhr.open('GET', query, true);
            // just request xhr without setting header to make a pair with <img>
            //(<any>xhr).__plantedQuery = query;
            if (type) {
                xhr.responseType = type;
            }
            xhr.send();
            return deferred.promise;
        };
        CMdsImageManager.prototype.setHeader = function (header) {
            this._header = header;
        };
        CMdsImageManager.prototype.getImage = function (query, header, type) {
            if (!this._header) {
                this.onRequestHeader();
            }
            return this._requestUrl(query, header, type);
        };
        CMdsImageManager.prototype._requestHeader = function () {
            var header = this.onRequestHeader();
            if (header) {
                throw '_requestHeader fail';
            }
            this._header = header;
        };
        return CMdsImageManager;
    })();
    var CMdSApi = (function () {
        function CMdSApi(urlAuthGetter) {
            var _this = this;
            this._header = {};
            this._qQueriesWaitingForAuth = [];
            this._managerImage = new CMdsImageManager();
            this._getterUrlAuth = urlAuthGetter;
            this._managerImage.onRequestHeader = function () {
                return _this.getHeader();
            };
        }
        //private _getVersion() {
        //    if (!this.getBaseUrl()) {
        //        var deferred = Q.defer();
        //        deferred.reject('need base url');
        //        return deferred.promise;
        //    }
        //    return this._call('/version.json');
        //}
        CMdSApi.prototype._call = function (cachedHeader, query) {
            if (!hx.svc.SETTING_UTIL.getInternetAvailable()) {
                hx.log('mds', '[_call] cannot make a request cos network(internet) is not available');
                return Q.reject('network is not available');
            }
            var deferred = Q.defer();
            var url;
            var self = this;
            var header = $.extend(cachedHeader, this.getHeader());
            var bIfModified = false;
            if (header['If-Modified-Since'] && header['If-Modified-Since'] != '') {
                bIfModified = true;
            }
            if (header['URL']) {
                url = header['URL'];
            }
            else {
                url = this.getBaseUrl() + query;
            }
            $.ajax({
                type: 'get',
                url: url,
                beforeSend: function (xhr) {
                    if (header['If-Modified-Since'] && header['If-Modified-Since'] != '') {
                        var date = header['If-Modified-Since'].trim();
                        xhr.setRequestHeader('If-Modified-Since', date);
                    }
                    if (header['Last-Modified'] && header['Last-Modified'] != '') {
                        var date = header['Last-Modified'].trim();
                        xhr.setRequestHeader('Last-Modified', date);
                    }
                },
                headers: header,
                ifModified: bIfModified,
                success: function (data, str, res) {
                    hx.log('mds', '[_call] ajax success!!! query : ' + query + ', res.status : ' + res.status);
                    console.log(url);
                    if (data instanceof XMLDocument) {
                        console.log(data.documentElement);
                    }
                    else {
                        console.log(data);
                    }
                    deferred.resolve({
                        status: res.status,
                        header: res.getAllResponseHeaders(),
                        data: data,
                        query: query
                    });
                },
                error: function (jqXHR, textStatus, errorThrown) {
                    var errorMsg = 'query: ' + query + '\n\t';
                    errorMsg += 'status(code): ' + jqXHR.status + '\n\t';
                    errorMsg += 'statusText: ' + jqXHR.statusText + '\n\t';
                    errorMsg += 'responseText: ' + jqXHR.responseText + '\n\t';
                    errorMsg += 'textStatus: ' + textStatus + '\n\t';
                    errorMsg += 'errorThrown: ' + errorThrown;
                    hx.log('debug', '[_call] ajax error!!! ' + errorMsg);
                    deferred.reject({
                        status: jqXHR.status,
                        query: query
                    });
                }
            });
            return deferred.promise;
        };
        /* public function */
        CMdSApi.prototype.setBaseUrl = function (url) {
            hx.log('mds', 'setBaseUrl : ' + url);
            this._urlBase = url;
        };
        CMdSApi.prototype.renewAuthInfo = function (retryAfter, mdsUrl) {
            var _this = this;
            if (this._timerRetryAfter) {
                console.error('> rejected: timer already has been set until ' + __debug.retryAfter401);
                return Q.reject('> rejected: timer already has been set.');
            }
            var deferred = Q.defer();
            var fxInner = function () {
                __getAuthToken(mdsUrl ? mdsUrl : _this._getterUrlAuth()).done(function (idefRes) {
                    //IDeferResolved+data<IMDSResAuth>
                    hx.log('mds', 'success: renew auth info');
                    var authInfo = idefRes.data;
                    _this.setBaseUrl(authInfo['x-baseurl']);
                    _this.setHeader('x-auth-hash', authInfo['x-auth-hash']);
                    _this.setHeader('x-auth-timestamp', authInfo['x-auth-timestamp']);
                    _this.setHeader('Accept', 'application/vnd.fvc.v1+xml');
                    //this._getVersion().done(
                    //    (json) => {
                    //        this.setHeader('Accept', 'application/vnd.fvc.v' + json.version[0] + '+xml');
                    //        this._managerImage.setHeader(this.getHeader());
                    //        deferred.resolve(authInfo);
                    //    },
                    //    (fail) => {
                    //        this.setHeader('Accept', 'application/vnd.fvc.v1+xml');
                    //        this._managerImage.setHeader(this.getHeader());
                    //        deferred.resolve(authInfo);
                    //    }
                    //);
                    //this._managerImage.setBaseUrl(authInfo['x-baseurl'].replace('http://api', 'http://img'));
                    _this._managerImage.setHeader(_this.getHeader());
                    var pending;
                    while (pending = _this._qQueriesWaitingForAuth.shift()) {
                        _this.xml(null, pending.query).done(pending.deferred.resolve, pending.deferred.reject);
                    }
                    deferred.resolve(idefRes);
                }, function (res) {
                    hx.log('debug', 'auth fail ph2: all waiting queries will be rejected');
                    var prev;
                    while (prev = _this._qQueriesWaitingForAuth.shift()) {
                        prev.deferred.reject(res);
                    }
                    _this.setBaseUrl(null);
                    deferred.reject(res);
                });
            };
            if (retryAfter) {
                this._timerRetryAfter = setTimeout(function () {
                    fxInner();
                    _this._timerRetryAfter = null;
                }, retryAfter * 1000);
                __debug.retryAfter401 = new Date(Date.now() + (retryAfter * 1000)).toString();
            }
            else {
                fxInner();
            }
            return deferred.promise;
        };
        /* public function */
        CMdSApi.prototype.getHeader = function () {
            return this._header;
        };
        /* public function */
        CMdSApi.prototype.setHeader = function (property, value) {
            console.log('setHeader: ' + property + ' | value: ' + value);
            this._header[property] = value;
        };
        /* public function */
        CMdSApi.prototype.getBaseUrl = function () {
            return this._urlBase;
        };
        /* public function */
        CMdSApi.prototype.xml = function (header, query, image_variant) {
            if (image_variant) {
                query = query + "&image_variant=" + encodeURIComponent(image_variant);
            }
            if (!this.getBaseUrl()) {
                var deferred = Q.defer();
                if (this._qQueriesWaitingForAuth.push({ query: query, deferred: deferred }) === 1) {
                    if (!this._timerRetryAfter) {
                        this.renewAuthInfo().done(null, function (err) {
                            console.error('auth fail: ' + err);
                        });
                    }
                }
                return deferred.promise;
            }
            return this._call(header, query);
        };
        /* public function */
        CMdSApi.prototype.image = function (header, query, type) {
            return this._managerImage.getImage(query, header, type);
        };
        return CMdSApi;
    })();
    return CMdSApi;
});